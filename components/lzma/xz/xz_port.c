#include "stdio.h"
#include <stddef.h>
#include <string.h>

static uint8_t *mallocBuf = NULL;
static uint32_t malloced = 0;
static uint32_t bufsize = 0;

void simple_malloc_init(uint8_t *buf, uint32_t len)
{
    mallocBuf = buf;
    malloced = 0;
    bufsize = len;
}

void *simple_malloc(uint32_t size)
{
    uint8_t *p;

    // ensure 4-bytes align
    malloced = (malloced + 4 - 1) & ~(4 - 1);
    if (malloced + size < bufsize) {
        p = mallocBuf + malloced;
        malloced += size;
        return p;
    }

    return NULL;
}

void *simple_calloc(size_t num, size_t size)
{
    size_t totalSize = num * size; // calculates total memory size
    void* ptr = simple_malloc(totalSize); // use simple_malloc allocate memory

    if (ptr != NULL) {
        memset(ptr, 0, totalSize); // use memset initialize memory to zero
    }

    return ptr; // return a pointer
}

void simple_free(void *p)
{
    //printf("Simple Free %08x\r\n", p);
    return;
}
