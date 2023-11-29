#include "../null.h"
#include "../Vector.h"
#include "../String/String.h"

#include "NMap.h"
#include "Freelist.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#define __NMAP_DEFAULT_INITBUFFERSIZE sizeof(struct _nmap)
#define __NMAP_DEFAULT_PAGESIZE 24 /* this needs to be in an between of not to small to make the free list \
                                      insufficient to not to large for slowing down cache speeds (24 is a good in between), it should also be 64 bitaligned */

/* these values assume an mmap and shm page size of 4096 */
#define __NMAP_DB_CAP 0x10000000000ULL        /* size cap (in bytes) per database shouldn't be larger than 128T to fit inside address space boundaries of linux  */
                                              /* needs to be significantly larger than 2¹⁶ to prevent conflicts when generating the file descriptor */
#define __NMAP_DB_ENTRYPOINT 0xC0000000000ULL /* entry point for address space that is used for shared mappings */

#define __NMAP_DIR /*HOME_DIR*/ "/.nmdb"
#define __NMAP_INF /*__NMAP_DIR*/ "/inf.dat"
#define __NMAP_DBDIR /*__NMAP_DIR*/ "/db"

struct _nmap
{
    int dbfd;
    _nmap_size dbsize;
    _nmap_size dbcapacity;
    struct _freelink *dbfreelist;
    void *map_addr; /* allthough mmap spaces arent shared they shouldn't overlap */
    const char *name;
};

typedef _nmap_size _nmap_head; /* header to store size */

struct _nmap *nmap_openStorage(const char *name)
{
    char *t = getenv("HOME");
    if (t == null)
    {
        printf("nmap_openStorage getenv: HOME env not set\n");
        return null;
    }
    xstrcreateft(str, t);

    /* default nmap storage directory */
    xstrappends(&str, __NMAP_DIR);
    mkdir(xstrserialize(str), S_IRUSR | S_IWUSR);

    /* database directory */
    xstrappends(&str, __NMAP_DBDIR);
    mkdir(xstrserialize(str), S_IRUSR | S_IWUSR);

    xstrappendc(&str, '/');
    xstrappends(&str, (char *)name); /* database name */
    xstrappends(&str, ".ndb");

    printf("%s\n", xstrserialize(str));

    /* open shared db memory */
    register int db_shm = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (db_shm == -1)
    {
        perror("nmap_openStorage shm_open");
        return null;
    }
    lseek(db_shm, SEEK_SET, sizeof(struct _nmap));
    write(db_shm, t, 1);

    /* attach shared db memory */
    struct _nmap *map;
    if ((map = (struct _nmap *)mmap(null, sizeof(struct _nmap), PROT_READ | PROT_WRITE, MAP_SHARED, db_shm, 0)) == MAP_FAILED)
    {
        perror("nmap_openStorage mmap 1");
        return null;
    }

    /* load database file */
    register int dbfd;
    if ((dbfd = open(str._str /* string was already serialized above */, O_RDWR, S_IRUSR | S_IWUSR)) == -1)
    {
        if ((dbfd = open(str._str, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
        {
            perror("nmap_openStorage open 1");
            return null;
        }

        /* collect address info */
        str._size -= strlen(name) + 4 + sizeof(__NMAP_DBDIR) /* .ndb */; /* restore old string length to access info file */
        /* collect map addresses from info directory */
        xstrappends(&str, __NMAP_INF);
        register int info;
        if (((info = open(xstrserialize(str), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR))) == -1)
        {
            perror("nmap_openStorage open 2");
            return null;
        }
        read(info, &map->map_addr, sizeof(void *));

        map->map_addr += __NMAP_DB_CAP;

        map->dbfd = (int)6000 + (((unsigned long long)map->map_addr) / __NMAP_DB_CAP); /* creates a unique fd if __NMAP_DB_CAP is significantly larger than 2¹⁶ */
        lseek(info, 0, SEEK_SET); /* needed because open for some reason doesn't open the file with offset set to 0 */
        write(info, &map->map_addr, sizeof(void *));
        close(info);

        lseek(dbfd, __NMAP_DEFAULT_INITBUFFERSIZE, SEEK_CUR);
        write(dbfd, &(char){0}, 1); /* write dummy byte to complete truncate operation */

        map->dbcapacity = __NMAP_DEFAULT_INITBUFFERSIZE;
        map->dbsize = sizeof(struct _nmap);
    }
    else
    {
        read(dbfd, map, sizeof(struct _nmap)); /* collect database object */
    }

    /* load mmap */
    if (mmap(map->map_addr, map->dbcapacity, PROT_READ | PROT_WRITE, MAP_SHARED, dbfd, 0) == MAP_FAILED)
    {
        perror("nmap_openStorage mmap 2");
        return null;
    }
    if (dup2(dbfd, map->dbfd) == -1) /* should work in most cases to produce an unique descriptor by shifting the map address 16 bytes and casting it to int */
    {
        perror("nmap_openStorage dup2");
        return null;
    }

    return (void *)map;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
static __attribute__((__always_inline__)) void map_grow(struct _nmap *map)
{
    munmap(map->map_addr, map->dbcapacity);
    lseek(map->dbfd, map->dbcapacity *= 2, SEEK_SET);
    write(map->dbfd, &(char *){0}, 1);
    mmap(map->map_addr, map->dbcapacity, PROT_READ | PROT_WRITE, MAP_SHARED, map->dbfd, 0);
}

__attribute__((__always_inline__)) void *nmap_alloc(struct _nmap *map, _nmap_size size)
{
    size += sizeof(_nmap_head); /* add header size */
    /* round up to multiple of page size */
    size = (size - (size % __NMAP_DEFAULT_PAGESIZE)) + __NMAP_DEFAULT_PAGESIZE;

    register void *ret;
    if (!flst_empty(map->dbfreelist) && flst_size(map->dbfreelist) >= size)
    {
        ret = (void *)flst_buff(map->dbfreelist);
        map->dbfreelist = (flst_size(map->dbfreelist) -= size > 0) ? map->dbfreelist : map->dbfreelist->next;
        flst_pop(map->dbfreelist);
        return ret;
    }

    ret = map->map_addr + map->dbsize;
    map->dbsize += size;

    while (map->dbsize > map->dbcapacity)
        map_grow(map);

    *(_nmap_head *)ret = size; /* set size header */

    printf("size: %lld, capacity: %lld, dbsize: %lld\n", size, map->dbcapacity, map->dbsize);

    return ret + sizeof(_nmap_head);
}

_nmap_size nmap_usableSize(void* addr)
{
    return *(((_nmap_head*)addr)-1);
}
#pragma GCC diagnostic pop

void *nmap_openStorageOnDevice(char *name, const char *deviceId)
{
}
