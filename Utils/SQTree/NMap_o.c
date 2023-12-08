#include "NMap.h"

NMap* nmap_open(SString*name)
{
    struct _nmap*ret = (struct _nmap*)malloc(sizeof(struct _nmap));
    memset(ret, 0, sizeof(struct _nmap));
    nmap_openob(ret, name);
    return ret;
}

void nmap_openob(void* buffer, SString*name)
{
    sstr_appendcs(name, ".ndb");
    // open file
    openf:((NMap*)buffer)->fd = open(sstr_serialize(name), O_RDWR, S_IRUSR | S_IWUSR);
    
    // check if file exists
    if(((NMap*)buffer)->fd == -1)
    {
        // create file if doesn't exist
        ((NMap *)buffer)->fd = open(sstr_serialize(name), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        lseek(((NMap*)buffer)->fd, __NMAP_DEFAULT_MAIN_BUFFER_SIZE, SEEK_CUR);
        char ch = 0;
        write(((NMap*)buffer)->fd, &ch, 1);
        //close(fd);
        goto openf;
    }

    // get file stat
    struct stat sb;
    fstat(((NMap*)buffer)->fd, &sb);
    sstr_removeFromEnd(name, 4);

    ((NMap*)buffer)->capacity = sb.st_size;
    ((NMap*)buffer)->mmap = (char*)mmap(0, ((NMap*)buffer)->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, ((NMap*)buffer)->fd, 0);
    fl_open(&(((NMap*)buffer)->freelist), name);
    if(((NMap*)buffer)->size == 0)
        ((NMap*)buffer)->size = *(_nmap_size*)((NMap*)buffer)->mmap;
    if(((NMap*)buffer)->size == 0)
        ((NMap*)buffer)->size = sizeof(_nmap_size);
    sstr_delete(name);
}

_nmap_ptr nmap_alloc(NMap* nmap, _nmap_size _size)
{
    _size+=sizeof(_nmap_size); // add to encounter size of header
    // round up
    _nmap_size size = (_size - (_size%__NMAP_DEFAULT_PAGESIZE))+__NMAP_DEFAULT_PAGESIZE;
    _nmap_ptr res = fl_alloc(&(nmap->freelist), size);
    if(res == null)
    {
        // if freelist hasn't free buffer of size, allocate new space
        res = nmap->size;
        _nmap_size s_new = nmap->size + size;
        while(s_new > nmap->capacity)
            _nmap_remap(nmap, nmap->capacity << 1); /* double map capacity */
        nmap->size = s_new;
    }
    *(_nmap_size*)nmap_get(nmap, res) = size; // set header
    res += sizeof(_nmap_size); // make that header isn't on the return pointer
    return res;
}

static inline _nmap_ptr _nmap_direct_alloc(NMap* nmap, _nmap_size size)
{
    unsigned long long res = nmap->size;
    unsigned long long s_new = nmap->size + size;
    while(s_new > nmap->capacity)
    {
        _nmap_remap(nmap, nmap->capacity*2);
    }
    nmap->size = s_new;
    // no header here
    return res;
}

unsigned long long nmap_root(NMap* nmap, _nmap_size size)
{
    if(nmap->size == sizeof(_nmap_size))
    {
        return _nmap_direct_alloc(nmap, size);
    }
    return sizeof(_nmap_size);
}

void nmap_free(NMap* nmap, _nmap_ptr ptr)
{  
    fl_free(&(nmap->freelist), *(_nmap_size*)(nmap_get(nmap, ptr-8)), ptr-8);
}

void nmap_freeAppend(NMap* nmap, _nmap_ptr ptr)
{
    nmap->size = ptr -8;
}

void nmap_extend(NMap* nmap, _nmap_ptr ptr, _nmap_size amount)
{
    _nmap_size size = (amount - (amount%__NMAP_DEFAULT_PAGESIZE))+__NMAP_DEFAULT_PAGESIZE;
    _nmap_size s_new = nmap->size + size;
    while(s_new > nmap->capacity)
        _nmap_remap(nmap, nmap->capacity<<1); /* double capacity */
    nmap->size = s_new;
    // current size of block
    _nmap_size* bsize = ((_nmap_size*)nmap_get(nmap, ptr-8));
    *bsize = *bsize + amount;
}

_nmap_size nmap_getSize(NMap* nmap, _nmap_ptr address)
{
    return *(_nmap_size*)nmap_get(nmap, address-sizeof(_nmap_size));
}

_nmap_ptr nmap_getPtr(NMap*nmap, void* address)
{
    return ((_nmap_ptr)address) - ((_nmap_ptr)nmap->mmap);
}

void* nmap_get(NMap* nmap, _nmap_ptr address)
{
    return nmap->mmap + address;
}

static inline void _nmap_remap(NMap* nmap, _nmap_size capacity)
{    //*(unsigned long     //*(unsigned long long*) nmap->mmap = nmap->size;long*) nmap->mmap = nmap->size;acity);
    printf("resize\n");
    munmap(nmap->mmap, nmap->capacity); /* remove munmap, since its useless */
    lseek(nmap->fd, capacity-1, SEEK_SET);
    nmap->capacity = capacity;
    char ch = 0;
    write(nmap->fd, &ch, 1);
    nmap->mmap = (char *)mmap(0, nmap->capacity, PROT_READ | PROT_WRITE, MAP_SHARED, nmap->fd, 0);
    printf("resize done\n");
}

void nmap_close(NMap* nmap)
{
    fl_close(&(nmap->freelist));
    *(_nmap_size*)nmap->mmap = nmap->size;
    munmap(nmap->mmap, nmap->capacity);
    close(nmap->fd);
}
