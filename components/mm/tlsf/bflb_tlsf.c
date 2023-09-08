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
#include "tlsf.h"
#include "bflb_irq.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/
#define TLSF_MALLOC_ASSERT(heap, x, size)                                                                                     \
    {                                                                                                                         \
        if (!(x)) {                                                                                                           \
            printf("tlsf malloc %d bytes failed at function %s using heap base:%p\r\n", size, __FUNCTION__, heap->heapstart); \
            bflb_irq_save();                                                                                                  \
            while (1)                                                                                                         \
                ;                                                                                                             \
        }                                                                                                                     \
    }

/****************************************************************************
 * Name: mem_tlfsinfo_walker
 ****************************************************************************/

static void mem_tlfsinfo_walker(void *ptr, size_t size, int used,
                                void *user)
{
    struct meminfo *info = user;

    if (!used) {
        info->free_node++;
        info->free_size += size;
        if (size > info->max_free_size) {
            info->max_free_size = size;
        }
    }
}

/****************************************************************************
 * Functions
 ****************************************************************************/

void bflb_mem_init(struct mem_heap_s *heap, void *heapstart, size_t heapsize)
{
    heap->heapstart = heapstart + tlsf_size();
    heap->heapsize = heapsize - tlsf_size();
    heap->priv = tlsf_create_with_pool(heapstart, heapsize);
}

void *bflb_malloc(struct mem_heap_s *heap, size_t nbytes)
{
    void *ret = NULL;
    uintptr_t flag;

    flag = bflb_irq_save();

    ret = tlsf_memalign(heap->priv, 32, nbytes);
    TLSF_MALLOC_ASSERT(heap, ret != NULL, nbytes);

    bflb_irq_restore(flag);

    return ret;
}

void bflb_free(struct mem_heap_s *heap, void *ptr)
{
    uintptr_t flag;

    flag = bflb_irq_save();

    tlsf_free(heap->priv, ptr);

    bflb_irq_restore(flag);
}

void *bflb_realloc(struct mem_heap_s *heap, void *ptr, size_t nbytes)
{
    void *ret = NULL;
    uintptr_t flag;

    flag = bflb_irq_save();

    ret = tlsf_realloc(heap->priv, ptr, nbytes);
    TLSF_MALLOC_ASSERT(heap, ret != NULL, nbytes);

    bflb_irq_restore(flag);

    return ret;
}

void *bflb_calloc(struct mem_heap_s *heap, size_t count, size_t size)
{
    void *ptr = NULL;
    size_t total = count * size;
    uintptr_t flag;

    flag = bflb_irq_save();

    if (count > 0 && size > 0) {
        if (count <= (SIZE_MAX / size)) {
            ptr = tlsf_malloc(heap->priv, total);
            if (ptr) {
                memset(ptr, 0, total);
            }
        }
    }

    bflb_irq_restore(flag);

    return ptr;
}

void *bflb_malloc_align(struct mem_heap_s *heap, size_t align, size_t size)
{
    void *ret = NULL;
    uintptr_t flag;

    flag = bflb_irq_save();

    ret = tlsf_memalign(heap->priv, align, size);
    TLSF_MALLOC_ASSERT(heap, ret != NULL, size);

    bflb_irq_restore(flag);

    return ret;
}

void bflb_mem_usage(struct mem_heap_s *heap, struct meminfo *info)
{
    uintptr_t flag;

    memset(info, 0, sizeof(struct meminfo));

    flag = bflb_irq_save();
    tlsf_walk_pool(heap->heapstart,
                   mem_tlfsinfo_walker, info);
    bflb_irq_restore(flag);

    info->total_size = heap->heapsize;
    info->used_size = info->total_size - info->free_size;
}
