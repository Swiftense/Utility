#ifndef NMAP_H
#define NMAP_H
#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned long long _nmap_size;
    typedef struct _nmap NMap;

    extern NMap *nmap_openStorage(const char *name);
    extern void nmap_closeStorage(NMap* map);
    extern void *nmap_openStorageOnDevice(char *name, const char *device);
    extern void nmap_close(void *map);
    extern const char *nmap_getDbName(struct _nmap *map);
    extern void *nmap_qalloc(struct _nmap *map, _nmap_size size);
    extern void *nmap_alloc(NMap *map, _nmap_size size);
    extern void nmap_free(NMap *map, void *addr);
    extern int nmap_top(NMap *map, void *addr);
    extern void *nmap_seek(NMap *map, void *addr, _nmap_size size);
    extern _nmap_size nmap_usableSize(void *addr);

#ifdef __cplusplus
}
#endif
#endif