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

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct mem_heap_s g_kmemheap;
struct mem_heap_s g_pmemheap;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
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

void kmem_init(void *heapstart, size_t heapsize)
{
    MEM_LOG("Heap: start=%p size=%u\r\n", heapstart, heapsize);

    bflb_mem_init(KMEM_HEAP, heapstart, heapsize);
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

void *kmalloc(size_t size)
{
    MEM_LOG("kmalloc %d\r\n", size);

    return bflb_malloc(KMEM_HEAP, size);
}

void *pvPortMallocStack(size_t xSize)
{
    return kmalloc(xSize);
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

    bflb_free(KMEM_HEAP, addr);
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
    return bflb_calloc(KMEM_HEAP, size, len);
}

/****************************************************************************
 * Name: kmem_init
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

    bflb_mem_init(PMEM_HEAP, heapstart, heapsize);
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

    return bflb_malloc(PMEM_HEAP, size);
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
    return bflb_realloc(PMEM_HEAP, old, newlen);
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
    return bflb_calloc(PMEM_HEAP, size, len);
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
    return bflb_malloc_align(PMEM_HEAP, align, size);
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

    bflb_free(PMEM_HEAP, addr);
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

    bflb_mem_usage(KMEM_HEAP, &info);

    return info.free_size;
}

uint32_t pfree_size(void)
{
#if defined(CONFIG_PSRAM) && defined(BL616) // only for bl618
    struct meminfo info;

    bflb_mem_usage(PMEM_HEAP, &info);

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
    bflb_mem_usage(KMEM_HEAP, &info);

    snprintf(mem, 64, "%-8d%-8d%-8d%-8d%-8d%-8d\r\n", info.total_size, info.free_size, info.used_size, info.max_free_size,
            info.free_node, info.used_node);

    printf(Header);
    printf(mem);

    free(mem);

#if defined(CONFIG_PSRAM) && defined(BL616) // only for bl618
    mem = malloc(64);
    bflb_mem_usage(PMEM_HEAP, &info);

    snprintf(mem, 64, "%-8d%-8d%-8d%-8d%-8d%-8d\r\n", info.total_size, info.free_size, info.used_size, info.max_free_size,
            info.free_node, info.used_node);

    printf(Header);
    printf(mem);

    free(mem);
#endif

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_free, free, show memory usage);
#endif
