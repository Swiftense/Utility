#include "Freelist.h"

Freelist *fl_open(void *buffer, SString *name)
{
    // open file
    sstr_appendcs(name, ".fl");
openf:
    ((Freelist *)buffer)->fd = open(sstr_serialize(name), O_RDWR, S_IRUSR | S_IWUSR);

    // check if file exists
    if (((Freelist *)buffer)->fd == -1)
    {
        // create file if doesn't exist
        ((Freelist *)buffer)->fd = open(sstr_serialize(name), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        lseek(((Freelist *)buffer)->fd, DEFAULT_FREELIST_BUFFER, SEEK_CUR);
        char ch = 0;
        write(((Freelist *)buffer)->fd, &ch, 1);
        // close(fd);
        goto openf;
    }
    // get file stat
    struct stat sb;
    fstat(((Freelist *)buffer)->fd, &sb);

    sstr_removeFromEnd(name, 3);

    ((Freelist *)buffer)->capacity = sb.st_size;
    ((Freelist *)buffer)->mmap = mmap(0, ((Freelist *)buffer)->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, ((Freelist *)buffer)->fd, 0);
    if (((Freelist *)buffer)->size == 0)
        ((Freelist *)buffer)->size = *(_nmap_size *)((Freelist *)buffer)->mmap;
}

void fl_free(Freelist *fl, _nmap_size fb_size, _nmap_ptr fb)
{
    fl->size += sizeof(FreeLink);
    if (fl->capacity < fl->size)
    {
        _fl_remap(fl, fl->capacity * 2);
    }
    FreeLink *flp = (FreeLink *)(fl->mmap + fl->size);
    flp->fb = fb;
    flp->fb_size = fb_size;
}

_nmap_ptr fl_alloc(Freelist *fl, _nmap_size size)
{
    FreeLink *flp = (FreeLink *)(fl->mmap + fl->size);
    if (flp->fb_size >= size)
    {
        flp->fb_size -= size;
        if (flp->fb_size == 0)
            fl->size -= sizeof(FreeLink);
        return flp->fb;
    }
    return null;
}

static inline void _fl_remap(Freelist *fl, _nmap_size capacity)
{ //*(unsigned long     //*(unsigned long long*) nmap->mmap = nmap->size;long*) nmap->mmap = nmap->size;acity);
    munmap(fl->mmap, fl->capacity);
    lseek(fl->fd, capacity - 1, SEEK_SET);
    fl->capacity = capacity;
    char ch = 0;
    write(fl->fd, &ch, 1);
    fl->mmap = (char *)mmap(0, fl->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fl->fd, 0);
}

void fl_close(Freelist *fl)
{
    *(_nmap_size *)fl->mmap = fl->size;
    munmap(fl->mmap, fl->capacity);
    close(fl->fd);
}