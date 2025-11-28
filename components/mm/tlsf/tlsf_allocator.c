/**
 * @file tlsf_allocator.c
 * @brief TLSF memory allocator adaptation layer implementation
 * @version 1.0
 * @date 2025
 *
 * This file implements the adaptation layer for the TLSF memory allocator,
 * encapsulating it as a standard memory manager.
 *
 * Copyright (c) 2025 Bouffalo Lab
 */

#include "../mm.h" /* Include framework header for all type definitions */
#include "../mm_config.h"
#include "tlsf.h"
#include <string.h>

#define DBG_TAG "MM_TLSF"
#ifdef CONFIG_MM_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_MM_LOG_LEVEL
#endif
#include "log.h"

/**
 * @brief Initialize the TLSF allocator
 *
 *
 * @param heap_id Heap ID
 * @param mem_pool Start address of the memory pool
 * @param pool_size Size of the memory pool
 * @return Pointer to the heap, NULL on failure
 */
static void *tlsf_allocator_init(uint32_t heap_id, void *mem_pool, size_t pool_size)
{
    mm_heap_t *heap;
    tlsf_t tlsf_handle;
    size_t heap_struct_size = ALIGN_UP(sizeof(mm_heap_t), 64);

    /* Initialize heap structure */
    heap = (mm_heap_t *)mem_pool;
    memset(heap, 0, heap_struct_size);
    mem_pool = (void *)((uintptr_t)mem_pool + heap_struct_size);
    pool_size -= heap_struct_size;

    /* Initialize TLSF memory pool */
    tlsf_handle = tlsf_create_with_pool(mem_pool, pool_size);
    if (!tlsf_handle) {
        return NULL;
    }

    /* Set basic heap information */
    strncpy(heap->name, "tlsf_heap", CONFIG_MM_HEAP_NAME_MAX - 1);
    heap->heap_id = heap_id;
    heap->allocator_id = MM_ALLOCATOR_TLSF; /* Set allocator ID for parameter validation */
    heap->start_addr = (void *)((uintptr_t)mem_pool + tlsf_size());
    heap->total_size = pool_size - tlsf_size();
    heap->end_addr = (void *)((uintptr_t)heap->start_addr + heap->total_size);
    heap->is_active = false;

    /* Set heap's private data pointer to TLSF handle */
    heap->allocator_priv = tlsf_handle;

    return heap;
}

/**
 * @brief Deinitialize the TLSF allocator
 *
 * @param heap Pointer to the heap
 */
static void tlsf_allocator_deinit(struct mm_heap *heap)
{
    tlsf_t tlsf_handle = (tlsf_t)heap->allocator_priv;
    if (tlsf_handle) {
        /* Destroy TLSF memory pool */
        tlsf_destroy(tlsf_handle);
        heap->allocator_priv = NULL;
    }

    heap->is_active = false;
}

/**
 * @brief Allocate memory using the TLSF allocator
 *
 * Refer to bflb_malloc implementation, supports alignment and zeroing flags
 *
 * @param heap Pointer to the heap
 * @param size Size of memory to allocate
 * @param flags Allocation flags
 * @return Pointer to allocated memory, NULL on failure
 */
static void *tlsf_allocator_malloc(struct mm_heap *heap, size_t size, uint32_t flags)
{
    tlsf_t tlsf_handle = (tlsf_t)heap->allocator_priv;
    if (!tlsf_handle) {
        LOG_E("TLSF handle is NULL for heap %u\r\n", heap->heap_id);
        return NULL;
    }

    void *ptr;
    size_t alignment;

    /* Parse alignment requirements */
    uint32_t align_flags = flags & MM_FLAG_ALIGN_MASK;

    if (align_flags <= MM_FLAG_ALIGN_PTR) {
        /* Default alignment: use tlsf_malloc for better performance */
        ptr = tlsf_malloc(tlsf_handle, size);
    } else {
        /* Special alignment: use tlsf_memalign for specified alignment */
        switch (align_flags) {
            case MM_FLAG_ALIGN_4BYTE:
                alignment = 4;
                break;
            case MM_FLAG_ALIGN_8BYTE:
                alignment = 8;
                break;
            case MM_FLAG_ALIGN_16BYTE:
                alignment = 16;
                break;
            case MM_FLAG_ALIGN_32BYTE:
                alignment = 32;
                break;
            case MM_FLAG_ALIGN_64BYTE:
                alignment = 64;
                break;
            default:
                alignment = sizeof(void *);
                break;
        }

        /* Perform aligned allocation */
        ptr = tlsf_memalign(tlsf_handle, alignment, size);
    }

    /* Zero memory if required */
    if (ptr != NULL && (flags & MM_FLAG_PROP_ZERO)) {
        memset(ptr, 0, size);
    }

    /* Update heap-level statistics - optional */
#if CONFIG_MM_ENABLE_STATISTICS
    if (ptr != NULL) {
        heap->stats.alloc_count++;
    }
#endif

    if (ptr != NULL) {
        LOG_D("TLSF allocated %zu bytes at %p from heap %u\r\n", size, ptr, heap->heap_id);
    } else {
        LOG_D("TLSF allocation failed for %zu bytes from heap %u\r\n", size, heap->heap_id);
    }

    return ptr;
}

/**
 * @brief Free memory back to the TLSF allocator
 *
 * Refer to bflb_free implementation
 * Updates heap-level statistics
 *
 * @param heap Pointer to the heap
 * @param ptr Pointer to memory to free
 */
static void tlsf_allocator_free(struct mm_heap *heap, void *ptr)
{
    tlsf_t tlsf_handle = (tlsf_t)heap->allocator_priv;
    if (!tlsf_handle) {
        LOG_E("TLSF handle is NULL for heap %u\r\n", heap->heap_id);
        return;
    }

    /* Perform free operation */
    tlsf_free(tlsf_handle, ptr);

    /* Update heap-level statistics - optional */
#if CONFIG_MM_ENABLE_STATISTICS
    heap->stats.free_count++;
#endif

    LOG_D("TLSF freed memory at %p from heap %u\r\n", ptr, heap->heap_id);
}

/**
 * @brief Reallocate memory using the TLSF allocator
 *
 * Uses TLSF's realloc feature
 *
 * @param heap Pointer to the heap
 * @param ptr Original memory address
 * @param newsize New memory size
 * @return Pointer to new memory, NULL on failure
 */
static void *tlsf_allocator_realloc(struct mm_heap *heap, void *ptr, size_t newsize)
{
    void *new_ptr;

    tlsf_t tlsf_handle = (tlsf_t)heap->allocator_priv;
    if (!tlsf_handle) {
        LOG_E("TLSF handle is NULL for heap %u\r\n", heap->heap_id);
        return NULL;
    }

    new_ptr = tlsf_realloc(tlsf_handle, ptr, newsize);

    if (new_ptr) {
        LOG_D("TLSF reallocated %zu bytes from %p to %p in heap %u\r\n",
              newsize, ptr, new_ptr, heap->heap_id);
    } else {
        LOG_D("TLSF realloc failed for %zu bytes from heap %u\r\n",
              newsize, heap->heap_id);
    }

    return new_ptr;
}

/**
 * @brief TLSF memory usage information traversal function
 *
 * @param ptr Pointer to memory block
 * @param size Size of memory block
 * @param used Whether it is used
 * @param user User data pointer
 */
static void tlsf_usage_walker(void *ptr, size_t size, int used, void *user)
{
    struct mm_usage_info *info = (struct mm_usage_info *)user;

    info->total_size += tlsf_alloc_overhead() + size;
    info->used_size += tlsf_alloc_overhead();

    if (used) {
        info->alloc_node++;
        info->used_size += size;
    } else {
        info->free_node++;
        info->free_size += size;
        if (size > info->max_free_size) {
            info->max_free_size = size;
        }
    }
}

/**
 * @brief Get usage information from the TLSF allocator
 *
 * @param heap Pointer to the heap
 * @param info Pointer to usage information structure
 */
static void tlsf_allocator_get_usage_info(struct mm_heap *heap, struct mm_usage_info *info)
{
    memset(info, 0, sizeof(struct mm_usage_info));

    tlsf_t tlsf_handle = (tlsf_t)heap->allocator_priv;
    if (!tlsf_handle) {
        return;
    }

    /* Initialize statistics */
    info->used_size = tlsf_pool_overhead() - tlsf_alloc_overhead();
    info->total_size = info->used_size;

    /* Traverse TLSF memory pool to gather statistics */
    pool_t pool = tlsf_get_pool(tlsf_handle);
    if (pool) {
        uintptr_t flags = mm_lock_save();
        tlsf_walk_pool(pool, tlsf_usage_walker, info);
        mm_unlock_restore(flags);
    }
}

/**
 * @brief TLSF allocator complete definition
 */
const mm_allocator_t g_tlsf_allocator = {
    .name = "TLSF", /* Static string, copied to structure array */

    .init = tlsf_allocator_init,
    .deinit = tlsf_allocator_deinit,

    .malloc = tlsf_allocator_malloc,
    .free = tlsf_allocator_free,
    .realloc = tlsf_allocator_realloc,

    .get_usage_info = tlsf_allocator_get_usage_info,

    .validate_pool = NULL,
    .dump_pool = NULL,
};
