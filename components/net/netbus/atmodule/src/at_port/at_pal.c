#include "at_pal.h"
#include "mm.h"

void *at_malloc(size_t size)
{
    return kmalloc(size, MM_FLAG_HEAP_ANY);
}

void *at_calloc(size_t count, size_t size)
{
    return kmalloc(count * size, MM_FLAG_HEAP_ANY | MM_FLAG_PROP_ZERO);
}

void at_free(void *ptr)
{
    if (ptr) {
        kfree(ptr);
    }
}
