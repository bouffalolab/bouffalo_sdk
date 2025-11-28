#include <string.h>
#include "at_pal.h"
#include "mm.h"   /* use kmalloc/kfree from kernel heap */

void *at_malloc(size_t size)
{
    return kmalloc(size, 0);
}

void *at_calloc(size_t count, size_t size)
{
    size_t total = count * size;
    void *ptr = kmalloc(total, 0);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void at_free(void *ptr)
{
    if (ptr) {
        kfree(ptr);
    }
}
