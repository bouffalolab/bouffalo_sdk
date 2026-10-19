/**
 * @file heap5_allocator.c
 * @brief heap5 allocator adaptation layer for the mm framework
 */

#include "../mm.h"
#include "../mm_config.h"
#include "heap5.h"
#include <stdint.h>
#include <string.h>

#define DBG_TAG "MM_HEAP5"
#ifdef CONFIG_MM_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_MM_LOG_LEVEL
#endif
#include "log.h"

#define HEAP5_ALLOC_MAGIC ((uintptr_t)0x48354150U)

typedef struct heap5_alloc_header {
    void *raw_ptr;
    size_t requested_size;
    size_t alignment;
    uintptr_t magic;
} heap5_alloc_header_t;

static size_t heap5_allocator_get_alignment(uint32_t flags)
{
    uint32_t align_flags = flags & MM_FLAG_ALIGN_MASK;

    switch (align_flags) {
        case MM_FLAG_ALIGN_4BYTE:
            return 4U;
        case MM_FLAG_ALIGN_8BYTE:
            return 8U;
        case MM_FLAG_ALIGN_16BYTE:
            return 16U;
        case MM_FLAG_ALIGN_32BYTE:
            return 32U;
        case MM_FLAG_ALIGN_64BYTE:
            return 64U;
        case MM_FLAG_ALIGN_128BYTE:
            return 128U;
        case MM_FLAG_ALIGN_256BYTE:
            return 256U;
        default:
            return sizeof(void *);
    }
}

static heap5_alloc_header_t *heap5_allocator_get_header(void *ptr)
{
    if (!ptr) {
        return NULL;
    }

    return (heap5_alloc_header_t *)((uint8_t *)ptr - sizeof(heap5_alloc_header_t));
}

static void *heap5_allocator_alloc_aligned(struct mm_heap *heap, size_t size,
                                           size_t alignment, uint32_t flags)
{
    heap5_t heap5_handle = (heap5_t)heap->allocator_priv;
    heap5_alloc_header_t *header;
    uintptr_t user_addr;
    void *raw_ptr;
    void *user_ptr;
    size_t total_size;

    if (!heap5_handle) {
        return NULL;
    }

    if (alignment < sizeof(void *)) {
        alignment = sizeof(void *);
    }

    if (size > (SIZE_MAX - sizeof(heap5_alloc_header_t) - (alignment - 1U))) {
        return NULL;
    }

    total_size = size + sizeof(heap5_alloc_header_t) + alignment - 1U;
    raw_ptr = heap5_malloc(heap5_handle, total_size);
    if (!raw_ptr) {
        return NULL;
    }

    user_addr = ((uintptr_t)raw_ptr + sizeof(heap5_alloc_header_t) + alignment - 1U) &
                ~(uintptr_t)(alignment - 1U);
    user_ptr = (void *)user_addr;
    header = heap5_allocator_get_header(user_ptr);
    header->raw_ptr = raw_ptr;
    header->requested_size = size;
    header->alignment = alignment;
    header->magic = HEAP5_ALLOC_MAGIC;

    if (flags & MM_FLAG_PROP_ZERO) {
        memset(user_ptr, 0, size);
    }

    return user_ptr;
}

static void *heap5_allocator_init(uint32_t heap_id, void *mem_pool, size_t pool_size)
{
    mm_heap_t *heap;
    heap5_t heap5_handle;
    size_t heap_struct_size = ALIGN_UP(sizeof(mm_heap_t), 64);
    size_t control_size = ALIGN_UP(heap5_control_size(), sizeof(void *));

    if (!mem_pool || pool_size <= (heap_struct_size + control_size)) {
        return NULL;
    }

    heap = (mm_heap_t *)mem_pool;
    memset(heap, 0, heap_struct_size);
    mem_pool = (void *)((uintptr_t)mem_pool + heap_struct_size);
    pool_size -= heap_struct_size;

    heap5_handle = (heap5_t)mem_pool;
    heap5_init(heap5_handle);

    mem_pool = (void *)((uintptr_t)mem_pool + control_size);
    pool_size -= control_size;

    if (heap5_add_region(heap5_handle, mem_pool, pool_size) != 0) {
        return NULL;
    }

    strncpy(heap->name, "heap5_heap", CONFIG_MM_HEAP_NAME_MAX - 1);
    heap->heap_id = heap_id;
    heap->allocator_id = MM_ALLOCATOR_HEAP5;
    heap->start_addr = mem_pool;
    heap->total_size = heap5_get_total_size(heap5_handle);
    heap->end_addr = (void *)((uintptr_t)mem_pool + pool_size);
    heap->allocator_priv = heap5_handle;
    heap->is_active = false;

    return heap;
}

static void heap5_allocator_deinit(struct mm_heap *heap)
{
    if (!heap) {
        return;
    }

    heap->allocator_priv = NULL;
    heap->is_active = false;
}

static void *heap5_allocator_malloc(struct mm_heap *heap, size_t size, uint32_t flags)
{
    void *ptr;
    size_t alignment;

    if (!heap || !heap->allocator_priv || size == 0U) {
        return NULL;
    }

    alignment = heap5_allocator_get_alignment(flags);
    ptr = heap5_allocator_alloc_aligned(heap, size, alignment, flags);

#if IS_ENABLED(CONFIG_MM_ENABLE_STATISTICS)
    if (ptr != NULL) {
        heap->stats.alloc_count++;
    }
#endif

    return ptr;
}

static void heap5_allocator_free(struct mm_heap *heap, void *ptr)
{
    heap5_t heap5_handle;
    heap5_alloc_header_t *header;

    if (!heap || !ptr) {
        return;
    }

    heap5_handle = (heap5_t)heap->allocator_priv;
    if (!heap5_handle) {
        return;
    }

    header = heap5_allocator_get_header(ptr);
    if (!header || header->magic != HEAP5_ALLOC_MAGIC) {
        LOG_E("Invalid heap5 allocation header for heap %u\r\n", heap->heap_id);
        return;
    }

    heap5_free(heap5_handle, header->raw_ptr);

#if IS_ENABLED(CONFIG_MM_ENABLE_STATISTICS)
    heap->stats.free_count++;
#endif
}

static void *heap5_allocator_realloc(struct mm_heap *heap, void *ptr, size_t newsize)
{
    heap5_alloc_header_t *header;
    void *new_ptr;
    size_t copy_size;

    if (!heap || !heap->allocator_priv) {
        return NULL;
    }

    if (!ptr) {
        return heap5_allocator_malloc(heap, newsize, MM_FLAG_ALIGN_PTR);
    }

    if (newsize == 0U) {
        heap5_allocator_free(heap, ptr);
        return NULL;
    }

    header = heap5_allocator_get_header(ptr);
    if (!header || header->magic != HEAP5_ALLOC_MAGIC) {
        return NULL;
    }

    new_ptr = heap5_allocator_alloc_aligned(heap, newsize, header->alignment, MM_FLAG_ALIGN_PTR);
    if (!new_ptr) {
        return NULL;
    }

    copy_size = header->requested_size < newsize ? header->requested_size : newsize;
    memcpy(new_ptr, ptr, copy_size);
    heap5_allocator_free(heap, ptr);

    return new_ptr;
}

static size_t heap5_allocator_get_block_size(struct mm_heap *heap, void *ptr)
{
    heap5_alloc_header_t *header;

    (void)heap;

    header = heap5_allocator_get_header(ptr);
    if (!header || header->magic != HEAP5_ALLOC_MAGIC) {
        return 0U;
    }

    return header->requested_size;
}

static size_t heap5_allocator_get_free_size(struct mm_heap *heap)
{
    heap5_t heap5_handle;

    if (!heap) {
        return 0U;
    }

    heap5_handle = (heap5_t)heap->allocator_priv;
    return heap5_get_free_size(heap5_handle);
}

static void heap5_allocator_get_usage_info(struct mm_heap *heap, struct mm_usage_info *info)
{
    struct heap5_stats stats;
    heap5_t heap5_handle;

    memset(info, 0, sizeof(*info));

    if (!heap) {
        return;
    }

    heap5_handle = (heap5_t)heap->allocator_priv;
    if (!heap5_handle) {
        return;
    }

    heap5_get_stats(heap5_handle, &stats);

    info->total_size = (uint32_t)stats.total_size;
    info->free_size = (uint32_t)stats.free_size;
    info->used_size = (uint32_t)(stats.total_size - stats.free_size);
    info->alloc_node = (uint32_t)stats.alloc_block_count;
    info->free_node = (uint32_t)stats.free_block_count;
    info->max_free_size = (uint32_t)stats.largest_free_block;
}

const mm_allocator_t g_heap5_allocator = {
    .name = "HEAP5",

    .init = heap5_allocator_init,
    .deinit = heap5_allocator_deinit,

    .malloc = heap5_allocator_malloc,
    .free = heap5_allocator_free,
    .realloc = heap5_allocator_realloc,

    .get_block_size = heap5_allocator_get_block_size,
    .get_free_size = heap5_allocator_get_free_size,

    .get_usage_info = heap5_allocator_get_usage_info,
    .walk_allocated_blocks = NULL,

    .validate_pool = NULL,
    .dump_pool = NULL,
};
