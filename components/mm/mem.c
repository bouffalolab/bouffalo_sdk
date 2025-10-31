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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "mem.h"
#include "bflb_irq.h"
#include "tlsf.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define MEM_MALLOC_ASSERT(ptr, size)    \
{                                       \
    if (!ptr) {                         \
        printf("mm malloc %d bytes failed at function %s\r\n", size, __func__); \
    }                                   \
}

#define MEM_HEAP_GET(mm_type) (&g_unified_heap.pools[mm_type].handle)

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct unified_heap_s g_unified_heap = {0};


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static inline struct mem_heap_s *find_pool_handle(void *vptr)
{
    for (int i = 0; i < MEM_HEAP_RAM_MAX; i++) {
        if (g_unified_heap.pools[i].is_active && 
            (uint32_t)g_unified_heap.pools[i].handle.heapstart <= (uint32_t)vptr && 
            (uint32_t)g_unified_heap.pools[i].handle.heapstart + g_unified_heap.pools[i].handle.heapsize >= (uint32_t)vptr) {
            return &g_unified_heap.pools[i].handle;
        }
    }
    return NULL;
}

/****************************************************************************
 * Functions
 ****************************************************************************/

void kmem_init(void *heap_start, size_t heap_size)
{
    uintptr_t flag;
    if (g_unified_heap.initialized) {
        MEM_LOG("Unified heap already initialized\n");
        return;
    }

    if (!heap_start || heap_size == 0) {
        MEM_LOG("Invalid heap parameters\n");
        return;
    }

    flag = bflb_irq_save();
    memset(&g_unified_heap, 0, sizeof(g_unified_heap));
    bflb_mem_init(&g_unified_heap.pools[0].handle, heap_start, heap_size);
    g_unified_heap.pools[0].pool_handle = tlsf_get_pool(g_unified_heap.pools[0].handle.priv);
    g_unified_heap.pools[0].is_active = 1;
    g_unified_heap.pool_count = 1;
    g_unified_heap.initialized = 1;
    bflb_irq_restore(flag);

    MEM_LOG("Unified heap initialized with main pool: %p, size: %zu\n", 
                     heap_start, heap_size);
}

int kmem_heap_add(void *pool_start, size_t pool_size, mm_heap_t mm_type)
{
    uintptr_t flag;
    int slot = mm_type;

    if (!g_unified_heap.initialized) {
        MEM_LOG("Unified heap not initialized\n");
        return -1;
    }

    if (!pool_start || pool_size == 0) {
        MEM_LOG("Invalid pool parameters\n");
        return -1;
    }

    if (g_unified_heap.pool_count >= MEM_HEAP_RAM_MAX || mm_type >= MEM_HEAP_RAM_MAX) {
        MEM_LOG("Maximum pool count reached\n");
        return -1;
    }

    if (g_unified_heap.pools[slot].is_active) {
        MEM_LOG("the mm_type %d already added\n", slot);
        return -1;
    }

    flag = bflb_irq_save();
    bflb_mem_init(&g_unified_heap.pools[slot].handle, pool_start, pool_size);
    g_unified_heap.pools[slot].pool_handle = tlsf_get_pool(g_unified_heap.pools[slot].handle.priv);
    g_unified_heap.pools[slot].is_active = 1;
    g_unified_heap.pool_count++;
    bflb_irq_restore(flag);

    MEM_LOG("Added pool: %p, size: %zu\n", 
                     pool_start, pool_size);
    
    return 0;
}

/**
 * kmalloc 
 * - allocate memory from the kernel heap 
 * - Allocates a contiguous block from kernel heap, at least pointer-size aligned.
 * - Returns NULL on failure. 
 *
 * @size: number of bytes to allocate.
 * @flags: allocation context flags (accepted for compatibility; ignored here).
 *
 *
 * Common flags (compatibility notes; not functionally enforced here):
 * %MM_KERNEL  — normal kernel allocation (sleeping not implemented).
 * %MM_ZERO    — Zero the allocated memory before returning. Also see kzalloc().
 * %MM_ATOMIC  — atomic-context allocation, may use emergency pools (not provided).
 * %MM_DMA     — Allocate memory directly accessible by DMA (not provided).
 * ...
 *
 * Notes:
 * - For alignment-sensitive allocations, use memalign(); for resizing, use realloc().
 */

void *kmalloc(size_t size, int flag)
{
    void *ptr = NULL;
    int mmtf;
    struct mem_heap_s *heap;
    int heap_mask = flag & ((1UL << MEM_HEAP_RAM_MAX) - 1);

    MEM_LOG("kmalloc %d %x\r\n", size, flag);

    /* If no heap type is specified, default to K_RAM */
    if (heap_mask == 0) {
        heap_mask = (1UL << MEM_HEAP_RAM_MAX) - 1;
    }

    /* Try to allocate memory in priority order */
    while (heap_mask != 0) {
        /* Use efficient bit operation to find the first set bit */
        mmtf = __builtin_ffs(heap_mask) - 1;
        
        /* Check if this heap is active */
        if (mmtf >= 0 && mmtf < MEM_HEAP_RAM_MAX && g_unified_heap.pools[mmtf].is_active) {
            heap = &g_unified_heap.pools[mmtf].handle;
            ptr = bflb_malloc_align(heap, (flag & MM_ALIGN32) ? 32 : sizeof(void *), size);
            
            if (ptr) {
                break;
            }
            MEM_LOG("kmalloc failed from heap %d, trying next...\n", mmtf);
        }
        
        /* Clear the current bit and continue to next */
        heap_mask &= ~(1UL << mmtf);
    }

    MEM_MALLOC_ASSERT(ptr, size);
    /* Zero the memory if MM_ZERO flag is set */
    if (ptr && (flag & MM_ZERO)) {
        memset(ptr, 0, size);
    }

    return ptr;
}

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

void *krealloc(void *old, size_t newlen)
{
    if (!old) {
        return kmalloc(newlen, MM_KERNEL|MM_PSRAM);
    }
    return bflb_realloc(find_pool_handle(old), old, newlen);
}

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

void *kmemalign(size_t align, size_t size)
{
    return bflb_malloc_align(MEM_HEAP_GET(MEM_HEAP_K_RAM), align, size);
}

void *pvPortMallocStack(size_t xSize)
{
    return kmalloc(xSize, 0);
}

/****************************************************************************
 * Name: free
 *
 * Description:
 *   Returns a chunk of user memory to the list of free nodes,  merging with
 *   adjacent free chunks if possible.
 *
 ****************************************************************************/

void kfree(void *addr)
{
    MEM_LOG("kfree %p\r\n", addr);

    if (addr) {
        bflb_free(find_pool_handle(addr), addr);
    }
}

void vPortFreeStack(void *pv)
{
    return kfree(pv);
}

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

void *kcalloc(size_t size, size_t len)
{
    return kmalloc(size * len, MM_KERNEL|MM_PSRAM|MM_ZERO);
}

/****************************************************************************
 * Name: pmem_init
 *
 * Description:
 *   Allocate memory from the user heap.
 *
 * Input Parameters:
 *
 ****************************************************************************/

void pmem_init(void *heapstart, size_t heapsize)
{
    MEM_LOG("Heap: start=%p size=%u\r\n", heapstart, heapsize);

    kmem_heap_add(heapstart, heapsize, MEM_HEAP_PS_RAM);
}

/****************************************************************************
 * Name: malloc
 *
 * Description:
 *   Allocate memory from the user heap.
 *
 * Input Parameters:
 *   size - Size (in bytes) of the memory region to be allocated.
 *
 * Returned Value:
 *   The address of the allocated memory (NULL on failure to allocate)
 *
 ****************************************************************************/

void *malloc(size_t size)
{
    MEM_LOG("malloc %d\r\n", size);

    return kmalloc(size, MM_KERNEL|MM_PSRAM|MM_ALIGN32);
}

/****************************************************************************
 * Name: realloc
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

void *realloc(void *old, size_t newlen)
{
    return krealloc(old, newlen);
}

/****************************************************************************
 * Name: calloc
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

void *calloc(size_t size, size_t len)
{
    return kmalloc(size * len, MM_KERNEL|MM_PSRAM|MM_ALIGN32|MM_ZERO);
}

/****************************************************************************
 * Name: memalign
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

void *memalign(size_t align, size_t size)
{
    return bflb_malloc_align(MEM_HEAP_GET(MEM_HEAP_K_RAM), align, size);
}

/****************************************************************************
 * Name: free
 *
 * Description:
 *   Returns a chunk of user memory to the list of free nodes,  merging with
 *   adjacent free chunks if possible.
 *
 ****************************************************************************/

void free(void *addr)
{
    MEM_LOG("free %p\r\n", addr);

    if (addr) {
        bflb_free(find_pool_handle(addr), addr);
    }
}


/****************************************************************************
 * Name: kfree_size
 *
 * Description:
 *   Returns the free heap size
 *
 ****************************************************************************/

uint32_t kfree_size(void)
{
    struct meminfo info;

    bflb_mem_usage(MEM_HEAP_GET(MEM_HEAP_K_RAM), &info);

    return info.free_size;
}

uint32_t pfree_size(void)
{
#if defined(CONFIG_PSRAM) && defined(BL616) // only for bl618
    struct meminfo info;

    bflb_mem_usage(MEM_HEAP_GET(MEM_HEAP_PS_RAM), &info);

    return info.free_size;
#else
    return 0;
#endif
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_free(int argc, char **argv)
{
    const char *Header = "total   free    alloc   mxblk   frnode  alnode  \r\n";
    struct meminfo info;
    char *mem;

    mem = malloc(64);


    for (int i = 0; i < MEM_HEAP_RAM_MAX; i++) {
        if (g_unified_heap.pools[i].is_active) {

            bflb_mem_usage(&g_unified_heap.pools[i].handle, &info);

            snprintf(mem, 64, "%-8d%-8d%-8d%-8d%-8d%-8d\r\n", info.total_size, info.free_size, info.used_size, info.max_free_size,
                     info.free_node, info.used_node);

            printf(Header);
            printf(mem);
        }
    }
    
    free(mem);

    return 0;
}

int cmd_mmdump(int argc, char **argv)
{
    
    /* Perform comprehensive memory analysis */
#ifdef CONFIG_TLSF_DEBUG
extern int tlsf_comprehensive_dump(struct mem_heap_s *heap);
    for (int i = 0; i < MEM_HEAP_RAM_MAX; i++) {
        if (g_unified_heap.pools[i].is_active) {
            tlsf_comprehensive_dump(&g_unified_heap.pools[i].handle);
        }
    }
#endif 
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mmdump, mmdump, show memory usage);
SHELL_CMD_EXPORT_ALIAS(cmd_free, free, show memory usage);
#endif
