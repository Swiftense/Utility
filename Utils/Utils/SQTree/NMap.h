#ifndef NMAP_H
#define NMAP_H
#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned long long _nmap_ptr;
    typedef unsigned long long _nmap_size;
    typedef struct _nmap NMap;

#define nmap_array(type) unsigned long long
#define nmap_ptr(type) unsigned long long
/* get size on nmap of datatype <type> */
#define nmap_sizeof(type) ((sizeof(type) - (sizeof(type) % __NMAP_DEFAULT_PAGESIZE)) + __NMAP_DEFAULT_PAGESIZE)

extern NMap *nmap_openStorage(const char *name);
extern void *nmap_openStorageOnDevice(char *name, const char *deviceId);
extern void* nmap_alloc(NMap* map, _nmap_size size);
extern _nmap_size nmap_usableSize(void* addr);

#ifdef __cplusplus
}
#endif
#endif