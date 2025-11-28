/**
 * @file mm.h
 * @brief Memory management framework core header
 * @version 1.0
 * @date 2025
 *
 * Copyright (c) 2025 Bouffalo Lab
 */

#ifndef __MM_H
#define __MM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "mm_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 *                      Memory allocator type enum
 * ======================================================================== */

/**
 * @brief Memory allocator type enum
 */
typedef enum {
    MM_ALLOCATOR_INVALID = 0, ///< Invalid allocator
    MM_ALLOCATOR_TLSF,        ///< TLSF allocator
    MM_ALLOCATOR_HEAP5,       ///< FreeRTOS heap5 allocator

    MM_ALLOCATOR_DEFAULT = MM_ALLOCATOR_TLSF, ///< Default allocator
    MM_ALLOCATOR_MAX                          ///< Maximum allocator count
} mm_allocator_type_t;

/**
 * @brief Memory heap type enum
 */
typedef enum {
    MM_HEAP_INVALID = 0, ///< Invalid heap
    MM_HEAP_PSRAM_0,     ///< PSRAM 0
    MM_HEAP_OCRAM_0,     ///< Internal OCRAM 0
    MM_HEAP_WRAM_0,      ///< Internal WRAM 0

    MM_HEAP_MAX ///< Maximum heap count
} mm_heap_type_t;

/* ========================================================================
 *                      Memory allocation flag bitfield
 * ======================================================================== */

/**
 * @brief Memory allocation flag layout (32-bit):
 * - Bits 0-3:   Heap selection (4 bits)
 * - Bits 4-6:   Alignment (3 bits)
 * - Bits 7-11:  Properties (5 bits)
 * - Bits 12-31: Reserved (20 bits)
 */

#define MM_FLAG_HEAP_SHIFT    0          ///< Heap ID shift
#define MM_FLAG_HEAP_MASK     0x0000000F ///< Heap selection mask

#define MM_FLAG_ALIGN_SHIFT   4          ///< Alignment shift
#define MM_FLAG_ALIGN_MASK    0x00000070 ///< Alignment mask

#define MM_FLAG_PROP_SHIFT    7          ///< Property shift
#define MM_FLAG_PROP_MASK     0x00000F80 ///< Property mask

/* ========================================================================
 *                      Heap selection flags
 * ======================================================================== */

/**
 * @brief Heap selection flags
 */
#define MM_FLAG_HEAP_ANY      0x00000000                              ///< Auto heap selection
#define MM_FLAG_HEAP_INVALID  0x00000000                              ///< Invalid heap
#define MM_FLAG_HEAP_OCRAM_0  (MM_HEAP_OCRAM_0 << MM_FLAG_HEAP_SHIFT) ///< OCRAM 0
#define MM_FLAG_HEAP_WRAM_0   (MM_HEAP_WRAM_0 << MM_FLAG_HEAP_SHIFT)  ///< WRAM 0
#define MM_FLAG_HEAP_PSRAM_0  (MM_HEAP_PSRAM_0 << MM_FLAG_HEAP_SHIFT) ///< PSRAM 0
#define MM_FLAG_HEAP_DEFAULT  MM_FLAG_HEAP_OCRAM_0                    ///< Default heap

/* ========================================================================
 *                      Alignment flags
 * ======================================================================== */

/**
 * @brief Alignment requirement flags
 */
#define MM_FLAG_ALIGN_NONE    0x00000000                    ///< Default alignment (sizeof(void*))
#define MM_FLAG_ALIGN_4BYTE   (0x01 << MM_FLAG_ALIGN_SHIFT) ///< 4-byte alignment
#define MM_FLAG_ALIGN_8BYTE   (0x02 << MM_FLAG_ALIGN_SHIFT) ///< 8-byte alignment
#define MM_FLAG_ALIGN_16BYTE  (0x03 << MM_FLAG_ALIGN_SHIFT) ///< 16-byte alignment
#define MM_FLAG_ALIGN_32BYTE  (0x04 << MM_FLAG_ALIGN_SHIFT) ///< 32-byte alignment
#define MM_FLAG_ALIGN_64BYTE  (0x05 << MM_FLAG_ALIGN_SHIFT) ///< 64-byte alignment
#define MM_FLAG_ALIGN_128BYTE (0x06 << MM_FLAG_ALIGN_SHIFT) ///< 128-byte alignment
#define MM_FLAG_ALIGN_256BYTE (0x07 << MM_FLAG_ALIGN_SHIFT) ///< 256-byte alignment

/* CPU architecture detection */
#if defined(CONFIG_CPU_RISCV32) || defined(CONFIG_CPU_ARM)
#define MM_FLAG_ALIGN_PTR   MM_FLAG_ALIGN_4BYTE
#define MM_FLAG_ALIGN_CACHE MM_FLAG_ALIGN_32BYTE ///< 32-bit CPU: 32-byte alignment
#elif defined(CONFIG_CPU_RISCV64) || defined(CONFIG_CPU_ARM64)
#define MM_FLAG_ALIGN_PTR   MM_FLAG_ALIGN_8BYTE
#define MM_FLAG_ALIGN_CACHE MM_FLAG_ALIGN_64BYTE ///< 64-bit CPU: 64-byte alignment
#else
#define MM_FLAG_ALIGN_PTR   MM_FLAG_ALIGN_4BYTE
#define MM_FLAG_ALIGN_CACHE MM_FLAG_ALIGN_32BYTE ///< Other architectures: 32-byte alignment
#endif

/* ======================================================================== */
/*                     MM_FLAG allocation property flags                   */
/* ======================================================================== */

/**
 * @brief MM_FLAG allocation property flags
 *
 * Control the behavior and characteristics of memory allocation
 */
#define MM_FLAG_PROP_NONE     0x00000000                   ///< No special properties
#define MM_FLAG_PROP_ZERO     (0x01 << MM_FLAG_PROP_SHIFT) ///< Zero memory after allocation
#define MM_FLAG_PROP_ATOMIC   (0x02 << MM_FLAG_PROP_SHIFT) ///< Atomic allocation
#define MM_FLAG_PROP_NOWAIT   (0x04 << MM_FLAG_PROP_SHIFT) ///< Non-blocking allocation
#define MM_FLAG_PROP_DMA      (0x08 << MM_FLAG_PROP_SHIFT) ///< DMA compatible
#define MM_FLAG_PROP_READONLY (0x10 << MM_FLAG_PROP_SHIFT) ///< Read-only memory

/* ======================================================================== */
/*                      Core data structure definitions                    */
/* ======================================================================== */

/* Forward declarations */
struct mm_heap;
struct mm_allocator;

/**
 * @brief Memory usage information structure
 */
struct mm_usage_info {
    uint32_t total_size;    ///< Total memory size
    uint32_t used_size;     ///< Used memory size
    uint32_t free_size;     ///< Free memory size
    uint32_t alloc_node;    ///< Number of allocated nodes
    uint32_t free_node;     ///< Number of freed nodes
    uint32_t max_free_size; ///< Maximum free node size
};

/**
 * @brief Memory allocator and operation interface
 *
 * Combines allocator information and operation interfaces to simplify the structure.
 * Directly pass the mm_heap_t pointer to provide complete heap information access.
 */
typedef struct mm_allocator {
    char name[CONFIG_MM_ALLOCATOR_NAME_MAX]; ///< Allocator name (array to avoid pointer issues)
    uint32_t allocator_id;                   ///< Allocator ID for debugging

    /* Allocator lifecycle management */
    void *(*init)(uint32_t heap_id, void *mem_pool, size_t pool_size); ///< Returns heap pointer
    void (*deinit)(struct mm_heap *heap);

    /* Core memory operations - must be implemented */
    void *(*malloc)(struct mm_heap *heap, size_t size, uint32_t flags);
    void (*free)(struct mm_heap *heap, void *ptr);
    void *(*realloc)(struct mm_heap *heap, void *ptr, size_t newsize);

    /* State query interface */
    void (*get_usage_info)(struct mm_heap *heap, struct mm_usage_info *info);

    /* Debug and validation interfaces - optional */
    void (*dump_pool)(struct mm_heap *heap);
    int (*validate_pool)(struct mm_heap *heap);
} mm_allocator_t;

/**
 * @brief Memory heap instance structure
 *
 * Each heap instance corresponds to a specific memory region and can use different allocators.
 * Contains basic heap information, statistics, and a pointer to allocator private data.
 */
typedef struct mm_heap {
    /* Basic heap attributes */
    uint32_t heap_id;                   ///< Heap ID for selection
    char name[CONFIG_MM_HEAP_NAME_MAX]; ///< Heap name (array to avoid pointer issues)
    uint32_t allocator_id;              ///< Allocator ID indicating the allocator in use

    /* Memory region information */
    void *start_addr;  ///< Start address of the memory region
    void *end_addr;    ///< End address of the memory region
    size_t total_size; ///< Total memory size

    /* Allocator private data */
    void *allocator_priv; ///< Pointer to allocator private data

    /* Heap state management */
    bool is_active; ///< Whether the heap is active

#if CONFIG_MM_ENABLE_STATISTICS
    /* Basic statistics information (optional, controlled by configuration) */
    struct {
        uint32_t alloc_count; ///< Allocation count
        uint32_t free_count;  ///< Free count
    } stats;
#endif
} mm_heap_t;

/**
 * @brief Memory manager main structure
 *
 */
typedef struct mem_manager {
    /* Allocator registry - stores pointers to statically defined allocators */
    mm_allocator_t *allocators[CONFIG_MM_ALLOCATOR_COUNT];
    uint32_t allocator_count;

    /* Heap instance storage - stores pointers to dynamically created heap instances */
    mm_heap_t *heaps[CONFIG_MM_HEAP_COUNT]; ///< Array of heap instance pointers

#if CONFIG_MM_ENABLE_STATISTICS
    /* Global statistics information (optional, controlled by configuration) */
    struct {
        uint32_t total_malloc_successes; ///< Total successful malloc count
        uint32_t total_free_successes;   ///< Total successful free count
        uint32_t total_malloc_failures;  ///< Total malloc failure count
    } stats;
#endif

    /* Framework state */
    bool initialized; ///< Whether the framework is initialized
} mem_manager_t;

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
uintptr_t mm_lock_save(void);

/**
 * @brief Memory management lock restore (weak function)
 *
 * Restores critical section protection
 * External code can reimplement this function to provide specific lock mechanism
 *
 * @param flags Lock status flag
 */
void mm_unlock_restore(uintptr_t flags);

/* ======================================================================== */
/*                      Core API declarations                              */
/* ======================================================================== */

/**
 * @brief Initialize the memory manager
 *
 * @return 0 on success, negative value on failure
 */
int mem_manager_init(void);

/**
 * @brief Register a memory allocator
 *
 * @param allocator_id Manager ID (array index, excluding 0)
 * @param allocator Pointer to allocator structure
 * @return 0 on success, negative value on failure
 */
int mm_register_allocator(uint32_t allocator_id, const mm_allocator_t *allocator);

/**
 * @brief Register and initialize a memory pool
 *
 * @param heap_id Heap ID (array index, excluding 0)
 * @param name Memory pool name
 * @param allocator_id Manager ID
 * @param start_addr Memory start address
 * @param size Memory size
 * @return Heap pointer, NULL on failure
 */
mm_heap_t *mm_register_heap(uint32_t heap_id, const char *name, uint32_t allocator_id,
                            void *start_addr, size_t size);

/**
 * @brief Find the heap corresponding to a given address
 *
 * Searches through all active heaps to find the one corresponding to the given memory address.
 *
 * @param addr Memory address
 * @return Pointer to the heap, NULL if not found
 */
mm_heap_t *mm_find_heap_by_addr(void *addr);

/**
 * @brief Core implementation of kmalloc
 *
 * Intelligent heap selection, supports automatic polling and specified heap allocation.
 *
 * @param size Allocation size
 * @param flags Allocation flags
 * @return Allocated memory address, NULL on failure
 */
void *kmalloc(size_t size, uint32_t flags);

/**
 * @brief Core implementation of kfree
 *
 * Intelligent address recognition, uses a dedicated lookup function to find the corresponding heap.
 *
 * @param ptr Memory address to be freed
 */
void kfree(void *ptr);

/**
 * @brief Reallocate memory
 *
 * Reallocate the size of a memory block, preserving the original data.
 * Preferably uses the allocator's realloc implementation, falls back to malloc+copy+free.
 * Note: realloc does not support flags and will allocate from the heap of the original memory.
 *
 * @param ptr Original memory address (can be NULL)
 * @param newsize New memory size
 * @return New memory address, NULL on failure
 */
void *krealloc(void *ptr, size_t newsize);

/**
 * @brief Allocate memory with a specific alignment requirement.
 *
 * @param size Number of bytes to allocate.
 * @param align Desired alignment in bytes.
 * @return Pointer to aligned memory on success, NULL on failure.
 */
void *kmemalign(size_t size, size_t align);

/* TLSF allocator local declaration */
extern const mm_allocator_t g_tlsf_allocator;

/* Global manager instance - the only global variable */
extern mem_manager_t g_mem_manager;

#endif /* __MM_H */