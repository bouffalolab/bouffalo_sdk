/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BFLB_MM_MEM_H
#define __BFLB_MM_MEM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MEM_ASSERT(x)                                                     \
    {                                                                     \
        if (!(x)) {                                                       \
            printf(#x " assert failed at function %s\r\n", __FUNCTION__); \
            bflb_irq_save();                                              \
            while (1)                                                     \
                ;                                                         \
        }                                                                 \
    }

#define MEM_LOG(...)  //printf(__VA_ARGS__)

#define MEM_IS_VALID(heap) ((heap) != NULL && (heap)->mem_impl != NULL)

typedef enum {
    MEM_HEAP_K_RAM = 0,
    MEM_HEAP_W_RAM,
    MEM_HEAP_PS_RAM,
    MEM_HEAP_RAM_MAX,
} mm_heap_t;

/****************************************************************************
 * Allocation Flags (bitmasks; may be OR'ed)
 *
 * Flags are split into two groups:
 * - Context flags (mutually exclusive in intent, but not enforced here):
 *     MM_KERNEL, MM_ZERO
 *
 * Examples:
 *   kmalloc(256, MM_KERNEL | MM_ZERO);
 *
 * If no context flag is provided, callers should treat it as MM_KERNEL.
 ****************************************************************************/
#define MM_NORMAL      (0<<0)                          /* Traverse allocation from all heaps (equivalent to MM_KERNEL|MM_WRAM|MM_PSRAM|...) */
#define MM_KERNEL      (1<<MEM_HEAP_K_RAM)             /* normal kernel allocation (compatibility) */
#define MM_WRAM        (1<<MEM_HEAP_W_RAM)             /* Allocate memory space from WRAM */
#define MM_PSRAM       (1<<MEM_HEAP_PS_RAM)            /* Allocate memory space from PSRAM */
#define MM_ZERO        (1<<MEM_HEAP_RAM_MAX)           /* zero memory before return */
#define MM_ALIGN32     (1<<(MEM_HEAP_RAM_MAX+1))       /* Use 32 byte alignment */
//#define MM_ATOMIC      (__MM_NORMAL|__MM_NOWAIT) /* atomic context (compatibility hint) */
//#define MM_DMA         (__MM_NORMAL|__MM_DMA)    /* DMA-capable memory (compatibility hint) */


/****************************************************************************
 * Public Types
 ****************************************************************************/
typedef void *pool_t;

struct mem_heap_s {
    void *priv;
    void *heapstart;
    size_t heapsize;
};

struct meminfo {
    int total_size;    /* This is the total size of memory allocated
                        * for use by malloc in bytes. */
    int free_node;     /* This is the number of free (not in use) chunks */
    int used_node;     /* This is the number of allocated (in use) chunks */
    int max_free_size; /* Size of the largest free (not in use) chunk */
    int used_size;     /* This is the total size of memory occupied by
                        * chunks handed out by malloc. */
    int free_size;     /* This is the total size of memory occupied
                        * by free (not in use) chunks. */
};

struct unified_pool_info {
    struct mem_heap_s handle;
    pool_t pool_handle;    /* TLSF handle*/ 
    uint8_t is_active;     /* is active */
};

struct unified_heap_s {
    struct unified_pool_info pools[MEM_HEAP_RAM_MAX]; /* memory pools */
    int pool_count;                               /* now pool counts */
    int initialized;                              /* is initialized*/
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: kmem_init
 *
 * Description:
 *   Allocate memory from the user heap.
 *
 * Input Parameters:
 *
 ****************************************************************************/

void kmem_init(void *heapstart, size_t heapsize);

int kmem_heap_add(void *pool_start, size_t pool_size, mm_heap_t mm_type);

/**
 * Name: kmalloc 
 *
 * Description:
 * - allocate memory from the kernel heap 
 * - Allocates a contiguous block from kernel heap, at least pointer-size aligned.
 * - Returns NULL on failure. 
 *
 * @size: number of bytes to allocate.
 * @flags: allocation context flags (accepted for compatibility; ignored here).
 *
 * Common flags (compatibility notes; not functionally enforced here):
 * %MM_KERNEL  — normal kernel allocation, may sleep (sleeping not implemented).
 * %MM_ZERO    — Zero the allocated memory before returning. Also see kzalloc().
 * %MM_ATOMIC  — atomic-context allocation, may use emergency pools (not provided).
 * %MM_DMA     — Allocate memory directly accessible by DMA (not provided).
 *
 * Notes:
 * - For alignment-sensitive allocations, use memalign(); for resizing, use realloc().
 */
void *kmalloc(size_t size, int flag);

/****************************************************************************
 * Name: kmemalign
 *
 * Description:
 *   memalign requests more than enough space from malloc, finds a region
 *   within that chunk that meets the alignment request and then frees any
 *   leading or trailing space.
 *
 *   The alignment argument must be a power of two (not checked).  8-byte
 *   alignment is guaranteed by normal malloc calls.
 *
 ****************************************************************************/
void *kmemalign(size_t align, size_t size);

/****************************************************************************
 * Name: krealloc
 *
 * Description:
 *   Re-allocate memory in the user heap.
 *
 * Input Parameters:
 *   oldmem  - The old memory allocated
 *   newsize - Size (in bytes) of the new memory region to be re-allocated.
 *
 * Returned Value:
 *   The address of the re-allocated memory (NULL on failure to re-allocate)
 *
 ****************************************************************************/
void *krealloc(void *old, size_t newlen);

/****************************************************************************
 * Name: kcalloc
 *
 * Description:
 *   Allocate and zero memory from the user heap.
 *
 * Input Parameters:
 *   size - Size (in bytes) of the memory region to be allocated.
 *
 * Returned Value:
 *   The address of the allocated memory (NULL on failure to allocate)
 *
 ****************************************************************************/
void *kcalloc(size_t size, size_t len);

/****************************************************************************
 * Name: free
 *
 * Description:
 *   Returns a chunk of user memory to the list of free nodes,  merging with
 *   adjacent free chunks if possible.
 *
 ****************************************************************************/

void kfree(void *addr);

/****************************************************************************
 * Name: kfree_size
 *
 * Description:
 *   Returns the free heap size
 *
 ****************************************************************************/
uint32_t kfree_size(void);

void pmem_init(void *heapstart, size_t heapsize);
uint32_t pfree_size(void);

/* private api for mm*/

void bflb_mem_init(struct mem_heap_s *heap, void *heapstart, size_t heapsize);

void *bflb_malloc(struct mem_heap_s *heap, size_t nbytes);

void bflb_free(struct mem_heap_s *heap, void *ptr);

void *bflb_realloc(struct mem_heap_s *heap, void *ptr, size_t nbytes);

void *bflb_calloc(struct mem_heap_s *heap, size_t count, size_t size);

void *bflb_malloc_align(struct mem_heap_s *heap, size_t align, size_t size);

void bflb_mem_usage(struct mem_heap_s *heap, struct meminfo *info);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __BFLB_MM_MEM_H */
