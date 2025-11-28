/**
 * @file mm.c
 * @brief Core implementation of the Bouffalo SDK memory management framework
 * @version 1.0
 * @date 2025
 *
 * This file implements the core functions of the memory management framework,
 * including allocator management, heap management, and API interfaces.
 *
 * Copyright (c) 2025 Bouffalo Lab
 */

#include "mm.h"
#include "bflb_core.h"
#include <string.h>
#include <errno.h>

#define DBG_TAG "MM"
#ifdef CONFIG_MM_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_MM_LOG_LEVEL
#endif
#include "log.h"

/* Global manager instance - the only global variable */
mem_manager_t g_mem_manager;

/* ======================================================================== */
/*                      Weak Function Definitions - Critical Section Protection                                */
/* ======================================================================== */

/**
 * @brief Memory management lock save (weak function)
 *
 * Provides critical section protection mechanism
 * External code can reimplement this function to provide specific lock mechanism
 *
 * @return Lock status flag
 */
__attribute__((weak)) uintptr_t mm_lock_save(void)
{
    return bflb_irq_save();
}

/**
 * @brief Memory management lock restore (weak function)
 *
 * Restores critical section protection
 * External code can reimplement this function to provide specific lock mechanism
 *
 * @param flags Lock status flag
 */
__attribute__((weak)) void mm_unlock_restore(uintptr_t flags)
{
    bflb_irq_restore(flags);
}

/* ======================================================================== */
/*       Framework Initialization and Management Interfaces                 */
/* ======================================================================== */

/**
 * @brief Memory manager initialization
 *
 * @return 0 on success, negative value on failure
 */
int mem_manager_init(void)
{
    mem_manager_t *manager = &g_mem_manager;

    /* Check if already initialized */
    if (manager->initialized) {
        return 0;
    }

    /* Zero-initialize manager structure */
    memset(manager, 0, sizeof(mem_manager_t));

    /* Register compile-time configured managers */
#ifdef CONFIG_MM_TLSF_EN
    mm_register_allocator(MM_ALLOCATOR_TLSF, &g_tlsf_allocator);
#endif

#ifdef CONFIG_MM_ENABLE_FREERTOS_HEAP5
    /* Future addition of FreeRTOS heap5 allocator registration */
    // extern const mm_allocator_t freertos_heap5_allocator;
    // mm_register_allocator(MM_ALLOCATOR_HEAP5, &freertos_heap5_allocator);
#endif

    /* Set manager status */
    manager->initialized = true;

    return 0;
}

/**
 * @brief Register memory manager
 *
 * Provide index at usage, directly mapped to array index (excluding 0)
 *
 * @param allocator_id Manager ID (array index, excluding 0)
 * @param allocator Manager structure pointer
 * @return 0 on success, negative value on failure
 */
int mm_register_allocator(uint32_t allocator_id, const mm_allocator_t *allocator)
{
    mem_manager_t *manager = &g_mem_manager;
    uintptr_t flags;

    /* Parameter validation */
    if (allocator_id == 0 || allocator_id >= CONFIG_MM_ALLOCATOR_COUNT) {
        LOG_E("Invalid allocator ID: %u\r\n", allocator_id);
        return -EINVAL;
    }

    if (!allocator || strlen(allocator->name) == 0 ||
        !allocator->malloc || !allocator->free || !allocator->realloc) {
        LOG_E("Invalid allocator structure\r\n");
        return -EINVAL;
    }

    flags = mm_lock_save();

    /* Check if position is already occupied */
    if (manager->allocators[allocator_id] != NULL) {
        mm_unlock_restore(flags);
        LOG_E("Allocator ID %u already registered\r\n", allocator_id);
        return -EEXIST;
    }

    /* Directly record manager pointer to specified position */
    manager->allocators[allocator_id] = (mm_allocator_t *)allocator;

    mm_unlock_restore(flags);

    return 0;
}

/**
 * @brief Register and initialize memory pool
 *
 * Returns heap pointer directly from allocator->init, avoiding dynamic memory
 * allocation by the framework. allocator->init handles the allocation of heap
 * structure and private data internally.
 *
 * @param heap_id Heap ID (array index, excluding 0)
 * @param name Memory pool name
 * @param allocator_id Manager ID
 * @param start_addr Memory start address
 * @param size Memory size
 * @return Pointer to the heap, NULL on failure
 */
mm_heap_t *mm_register_heap(uint32_t heap_id, const char *name, uint32_t allocator_id,
                            void *start_addr, size_t size)
{
    mem_manager_t *manager = &g_mem_manager;
    const mm_allocator_t *allocator;
    mm_heap_t *heap;
    uintptr_t flags;

    /* Parameter validation */
    if (heap_id == 0 || heap_id >= CONFIG_MM_HEAP_COUNT) {
        LOG_E("Invalid heap ID: %u\r\n", heap_id);
        return NULL;
    }

    /* Framework initialization check */
    if (!manager->initialized) {
        LOG_E("Failed to initialize memory manager\r\n");
        return NULL;
    }

    flags = mm_lock_save();

    /* Check if position is already occupied */
    if (manager->heaps[heap_id] != NULL) {
        mm_unlock_restore(flags);
        LOG_E("Heap ID %u already registered\r\n", heap_id);
        return NULL;
    }

    /* Get allocator */
    if (allocator_id == 0 || allocator_id >= CONFIG_MM_ALLOCATOR_COUNT) {
        mm_unlock_restore(flags);
        LOG_E("Invalid allocator ID: %u\r\n", allocator_id);
        return NULL;
    }
    allocator = manager->allocators[allocator_id];
    if (!allocator) {
        mm_unlock_restore(flags);
        LOG_E("Allocator %u not found\r\n", allocator_id);
        return NULL;
    }

    /* Call allocator initialization */
    heap = (mm_heap_t *)allocator->init(heap_id, start_addr, size);
    if (!heap) {
        mm_unlock_restore(flags);
        LOG_E("Allocator %u init failed\r\n", allocator_id);
        return NULL;
    }

    /* Set heap name */
    if (name && strlen(name) > 0) {
        strncpy(heap->name, name, CONFIG_MM_HEAP_NAME_MAX - 1);
        heap->name[CONFIG_MM_HEAP_NAME_MAX - 1] = '\0';
    }

    /* Activate heap and add to manager */
    heap->is_active = true;
    manager->heaps[heap_id] = heap;

    mm_unlock_restore(flags);

    return heap;
}

/* ======================================================================== */
/*                      Core Memory Allocation Interfaces                   */
/* ======================================================================== */

/**
 * @brief Find the heap corresponding to a given address
 *
 * @param addr Memory address
 * @return Pointer to the heap, NULL if not found
 */
mm_heap_t *mm_find_heap_by_addr(void *addr)
{
    mem_manager_t *manager = &g_mem_manager;
    mm_heap_t *heap;

    if (!addr) {
        return NULL;
    }

    /* Linear search through all active heaps */
    for (uint32_t i = 1; i < CONFIG_MM_HEAP_COUNT; i++) {
        heap = manager->heaps[i];
        if (heap == NULL || !heap->is_active) {
            continue;
        }

        if ((uintptr_t)addr >= (uintptr_t)heap->start_addr &&
            (uintptr_t)addr < (uintptr_t)heap->end_addr) {
            return heap;
        }
    }

    return NULL;
}

/**
 * @brief kmalloc core implementation - optimized version
 *
 * Intelligent heap selection, supports automatic polling and specified heap allocation
 * Optimized loop range and statistics update logic
 *
 * @param size Allocation size
 * @param flags Allocation flags
 * @return Allocated memory address, NULL on failure
 */
void *kmalloc(size_t size, uint32_t flags)
{
    mem_manager_t *manager = &g_mem_manager;
    mm_heap_t *heap;
    const mm_allocator_t *allocator;
    void *ptr = NULL;
    uint32_t heap_id;
    uint32_t start_idx, end_idx;
    uintptr_t irq_flags;

    if (size == 0) {
        return NULL;
    }

    if (!manager->initialized) {
        LOG_E("Memory manager not initialized\r\n");
        return NULL;
    }

    /* Parse allocation parameters and determine loop range */
    heap_id = (flags & MM_FLAG_HEAP_MASK) >> MM_FLAG_HEAP_SHIFT;
    if (heap_id != MM_FLAG_HEAP_ANY) {
        /* Specified heap mode: direct access, single attempt */
        if (heap_id >= CONFIG_MM_HEAP_COUNT) {
            LOG_E("Invalid heap ID in flags: %u\r\n", heap_id);
            return NULL;
        }

        start_idx = heap_id;
        end_idx = heap_id + 1;
    } else {
        /* Automatic selection mode: poll all active heaps */
        start_idx = 1;
        end_idx = CONFIG_MM_HEAP_COUNT;
    }

    irq_flags = mm_lock_save();

    /* Heap allocation loop */
    for (uint32_t i = start_idx; i < end_idx; i++) {
        heap = manager->heaps[i];
        if (!heap || !heap->is_active) {
            continue;
        }

        /* Get allocator and attempt allocation */
        allocator = manager->allocators[heap->allocator_id];
        if (!allocator) {
            continue;
        }

        ptr = allocator->malloc(heap, size, flags);
        if (ptr != NULL) {
            break;
        }
    }

    /* Update statistics */
#if CONFIG_MM_ENABLE_STATISTICS
    if (ptr) {
        manager->stats.total_malloc_successes++;
    } else {
        manager->stats.total_malloc_failures++;
    }
#endif

    mm_unlock_restore(irq_flags);

    if (ptr) {
        LOG_D("Allocated %zu bytes at %p\r\n", size, ptr);
    } else {
        LOG_W("Failed to allocate %zu bytes\r\n", size);
    }

    return ptr;
}

/**
 * @brief kfree core implementation - optimized version
 *
 * Intelligent address recognition, uses dedicated lookup function to find corresponding heap
 * Code is more concise, logic is clearer
 *
 * @param ptr Memory address to be freed
 */
void kfree(void *ptr)
{
    mem_manager_t *manager = &g_mem_manager;
    mm_heap_t *heap;
    const mm_allocator_t *allocator;
    uintptr_t irq_flags;

    if (!ptr) {
        return;
    }

    if (!manager->initialized) {
        return;
    }

    irq_flags = mm_lock_save();

    /* Find corresponding heap by address */
    heap = mm_find_heap_by_addr(ptr);
    if (!heap) {
        /* Address not found in any matching heap - silent failure */
        mm_unlock_restore(irq_flags);
        LOG_W("Free address %p not found in any heap\r\n", ptr);
        return;
    }

    /* Get allocator and perform free */
    allocator = manager->allocators[heap->allocator_id];
    if (!allocator) {
        /* Allocator does not exist, silent failure */
        mm_unlock_restore(irq_flags);
        LOG_E("Allocator %u not found for heap %u\r\n", heap->allocator_id, heap->heap_id);
        return;
    }
    allocator->free(heap, ptr);

#if CONFIG_MM_ENABLE_STATISTICS
    /* Update framework-level statistics */
    manager->stats.total_free_successes++;
#endif

    mm_unlock_restore(irq_flags);

    LOG_D("Freed memory at %p\r\n", ptr);
}

/**
 * @brief krealloc reallocate memory
 *
 * Reallocate memory block size, preserving original data
 * Note: realloc does not support flags, allocation is done from the original memory's heap
 *
 * @param ptr Original memory address (can be NULL)
 * @param newsize New memory size
 * @return New memory address, NULL on failure
 */
void *krealloc(void *ptr, size_t newsize)
{
    mem_manager_t *manager = &g_mem_manager;
    mm_heap_t *heap;
    const mm_allocator_t *allocator;
    void *new_ptr = NULL;
    uintptr_t irq_flags;

    /* Handle special cases */
    if (newsize == 0) {
        if (ptr) {
            kfree(ptr);
        }
        return NULL;
    }

    if (ptr == NULL) {
        /* Equivalent to malloc, use default heap */
        return kmalloc(newsize, MM_FLAG_HEAP_ANY);
    }

    irq_flags = mm_lock_save();

    /* Find corresponding heap by address */
    heap = mm_find_heap_by_addr(ptr);
    if (!heap) {
        mm_unlock_restore(irq_flags);
        LOG_E("Cannot find heap for address %p\r\n", ptr);
        return NULL;
    }

    /* Get allocator */
    allocator = manager->allocators[heap->allocator_id];
    if (!allocator) {
        mm_unlock_restore(irq_flags);
        LOG_E("Allocator %u not found for heap %u\r\n", heap->allocator_id, heap->heap_id);
        return NULL;
    }

    /* Call allocator's realloc implementation */
    new_ptr = allocator->realloc(heap, ptr, newsize);

#if CONFIG_MM_ENABLE_STATISTICS
    if (new_ptr) {
        /* Allocator realloc succeeded, update framework-level statistics */
        manager->stats.total_malloc_successes++;
        LOG_D("Allocator realloc succeeded: %p -> %p, size: %zu\r\n", ptr, new_ptr, newsize);
    } else {
        /* Allocator realloc failed */
        manager->stats.total_malloc_failures++;
        LOG_W("Allocator realloc failed for %zu bytes\r\n", newsize);
    }
#else
    if (new_ptr) {
        LOG_D("Allocator realloc succeeded: %p -> %p, size: %zu\r\n", ptr, new_ptr, newsize);
    } else {
        LOG_W("Allocator realloc failed for %zu bytes\r\n", newsize);
    }
#endif

    mm_unlock_restore(irq_flags);

    return new_ptr;
}

/**
 * @brief Allocate memory with a specific alignment requirement.
 *
 * @param size Number of bytes to allocate.
 * @param align Desired alignment in bytes.
 * @return Pointer to aligned memory on success, NULL on failure.
 */
void *kmemalign(size_t size, size_t align)
{
    uint32_t flags = 0;

    if (align <= 4U) {
        flags |= MM_FLAG_ALIGN_4BYTE;
    } else if (align <= 8U) {
        flags |= MM_FLAG_ALIGN_8BYTE;
    } else if (align <= 16U) {
        flags |= MM_FLAG_ALIGN_16BYTE;
    } else if (align <= 32U) {
        flags |= MM_FLAG_ALIGN_32BYTE;
    } else if (align <= 64U) {
        flags |= MM_FLAG_ALIGN_64BYTE;
    } else if (align <= 128U) {
        flags |= MM_FLAG_ALIGN_128BYTE;
    } else {
        flags |= MM_FLAG_ALIGN_256BYTE;
    }

    return kmalloc(size, flags);
}
