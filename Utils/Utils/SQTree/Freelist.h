#ifndef FREELIST_H
#define FREELIST_H
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _freelink Freelist;

struct _freelink
{
    _nmap_size size;
    void* buff;

    struct _freelink *next;
};

#define flst_empty(fli) fli == null

#define flst_buff(fli) fli->buff

#define flst_size(fli) fli->size

#define flst_push(fli, size, buff, map)                                        \
    fli->next = (struct _freelink *)nmap_alloc(map, sizeof(struct _freelink)); \
    fli->next->size = size;                                                    \
    fli->next->buff = buff;

#define flst_pop(fli) fli = fli->next;

#ifdef __cplusplus
}
#endif
#endif