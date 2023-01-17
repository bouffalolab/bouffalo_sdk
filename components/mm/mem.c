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

struct mem_heap_s g_memheap;

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
    MEM_LOG("Heap: start=%p size=%zu\r\n", heapstart, heapsize);

    bflb_mem_init(KMEM_HEAP, heapstart, heapsize);
}

/****************************************************************************
 * Name: usr_mem_init
 *
 * Description:
 *   Allocate memory from the user heap.
 *
 * Input Parameters:
 *
 ****************************************************************************/

void umem_init(struct mem_heap_s *heap, void *heapstart, size_t heapsize)
{
    bflb_mem_init(heap, heapstart, heapsize);
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
    return bflb_malloc(KMEM_HEAP, size);
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
    return bflb_realloc(KMEM_HEAP, old, newlen);
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
    return bflb_calloc(KMEM_HEAP, size, len);
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
    return bflb_malloc_align(KMEM_HEAP, align, size);
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
    bflb_free(KMEM_HEAP, addr);
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

    sprintf(mem, "%-8d%-8d%-8d%-8d%-8d%-8d\r\n", info.total_size, info.free_size, info.used_size, info.max_free_size,
            info.free_node, info.used_node);

    printf(Header);
    printf(mem);

    free(mem);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_free, free, show memory usage);
#endif
