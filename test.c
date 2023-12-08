#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define SYS_PAGE_SIZE 4096

#include "Utils/SQTree/NMap.h"
#include "Utils/String/String.h"

#include <string.h>

void main()
{
    srand(time(NULL));

    void *map = nmap_openStorage("fgjh ");
    void *t = nmap_alloc(map, 128ULL);
    printf("t 1: %llu\n", t);
    nmap_free(map, t);
    t = nmap_alloc(map, 128ULL);
    printf("t 2: %llu\n", t);
    nmap_free(map, t);
    t = nmap_alloc(map, 128ULL);
    printf("t 3: %llu\n", t);
    nmap_free(map, t);
    t = nmap_alloc(map, 128ULL);
    printf("t 4: %llu\n", t);
    nmap_free(map, t);
    t = nmap_alloc(map, 556ULL);
    printf("t 5: %llu\n", t);
    nmap_free(map, t);
    t = nmap_alloc(map, 128ULL);
    printf("t 6: %llu\n", t);
    printf("%s\n", nmap_getDbName(map));
    nmap_closeStorage(map);
}