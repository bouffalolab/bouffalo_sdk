#include "tlsf.h"
#include "bflb_tlsf.h"

static tlsf_t tlsf_ptr = 0;

int bflb_mmheap_init(void *begin_addr, uint32_t size)
{
    tlsf_ptr = (tlsf_t)tlsf_create_with_pool(begin_addr, size);

    if (tlsf_ptr == 0) {
        return -1;
    }
    return 0;
}

void *bflb_malloc(size_t nbytes)
{
    void *ptr;
    ptr = tlsf_malloc(tlsf_ptr, nbytes);

    return ptr;
}

void bflb_free(void *ptr)
{
    tlsf_free(tlsf_ptr, ptr);
}

void *bflb_realloc(void *ptr, size_t nbytes)
{
    if (tlsf_ptr) {
        ptr = tlsf_realloc(tlsf_ptr, ptr, nbytes);
    }
    return ptr;
}

void *bflb_calloc(size_t count, size_t nbytes)
{
    void *ptr = NULL;
    size_t total_size;

    total_size = count * nbytes;
    ptr = tlsf_malloc(tlsf_ptr, nbytes);
    if (ptr != NULL) {
        /* clean memory */
        memset(ptr, 0, total_size);
    }

    return ptr;
}

void *bflb_malloc_align(size_t align, size_t size)
{
    void *ptr = NULL;

    ptr = tlsf_memalign(tlsf_ptr, align, size);

    return ptr;
}

void tlsf_size_walker(void* ptr, size_t size, int used, void* user)
{
    if (used) {
        ((bflb_tlsf_size_container_t *)user)->used += (unsigned int)size;
    }
    else {
        ((bflb_tlsf_size_container_t *)user)->free += (unsigned int)size;
    }
}

bflb_tlsf_size_container_t *bflb_tlsf_stats() {
    static bflb_tlsf_size_container_t sizes;
    sizes.free = 0;
    sizes.used = 0;
    tlsf_walk_pool(tlsf_get_pool(tlsf_ptr), tlsf_size_walker, &sizes);
    return &sizes;
}