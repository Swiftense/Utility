#ifndef NMAP_H
    #define NMAP_H
    #ifdef __cplusplus
        extern "C" {
    #endif

    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/mman.h>

    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>

    #include "../../Utils/SString.h"
    #include "../../Utils/null.h"
    #include "../../Utils/debug.h"

    #include "NMap_types.h"
    #include "Freelist.h"    

    struct _nmap
    {
        // map
        _nmap_size size, capacity;
        int fd;
        char* mmap;

        // Freelist
        Freelist freelist;
    };

    NMap* nmap_open(SString*file);
    void nmap_openob(void* buffer, SString*name);
    // allocates block of <size> on map
    // returns the distance of it to the nmap->s_str member
    _nmap_ptr nmap_alloc(NMap*nmap, _nmap_size size);
    // get root Object of map (size shouldnt be changed)
    // HAS TO BE ACCESSED BEFORE FIRST NMAP_ALLOC CALL TO PREVENT CRASHES AND DATA LOSSES
    // returns the distance of it to the nmap->s_str member
    _nmap_ptr nmap_root(NMap*nmap, _nmap_size size);
    // adds the buffer of ptr to the free list
    void nmap_free(NMap*nmap, _nmap_ptr ptr);
    // quicker version of nmap_free, only works with buffers at the top/end of the mob
    void nmap_freeAppend(NMap* nmap, _nmap_ptr ptr);
    // converts nmap pointer <address> to memory address
    void* nmap_get(NMap*nmap, _nmap_ptr address);
    // converts memory address <address> to _nmap_ptr
    // may returns an invalid _nmap_ptr if <address> wasn't recieved by nmap_get
    _nmap_ptr nmap_getPtr(NMap*nmap, void* address);
    // extends block of <ptr>, block has to be last element on nmap
    void nmap_extend(NMap* nmap, _nmap_ptr ptr, _nmap_size amount);
    // Directly allocates a buffer.
    // Note that nmap_getSize(...) and nmap_free(...) won't work here.
    static inline _nmap_ptr _nmap_direct_alloc(NMap*nmap, _nmap_size size);
    // gets the size of the buffer behind <address>
    // note, that the size may vary from the size originally allocated,
    // since its just the size that theroetically could be written to without overwriding other data
    _nmap_size nmap_getSize(NMap*nmap, _nmap_ptr address);
    static inline void _nmap_remap(NMap* nmap, _nmap_size capacity);
    // writes all data to file, so that there is no rollback if the server crashes
    void nmap_close(NMap*nmap);

    #ifdef __cplusplus
        }
    #endif
#endif