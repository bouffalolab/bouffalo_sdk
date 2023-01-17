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
#include "mmheap.h"

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_FREERTOS

#define _IRQ_CONTEXT()          xPortIsInsideInterrupt()
#define _SCHED_LOCK()           (taskSCHEDULER_RUNNING != xTaskGetSchedulerState())
#define _ENTER_CRITICAL()       portENTER_CRITICAL()
#define _EXIT_CRITICAL()        portEXIT_CRITICAL()
#define _SEM_INIT(s, p, c)      xSemaphoreCreateRecursiveMutexStatic(s)
#define _SEM_WAIT(s)            xSemaphoreTakeRecursive((SemaphoreHandle_t)s, portMAX_DELAY)
#define _SEM_POST(s)            xSemaphoreGiveRecursive((SemaphoreHandle_t)s)

#else

#include "bflb_irq.h"
static volatile uintptr_t s_irq_flag;
static volatile uintptr_t s_irq_entry;

#define _IRQ_CONTEXT() (0)
#define _ENTER_CRITICAL()             \
    {                                 \
        s_irq_flag = bflb_irq_save(); \
        s_irq_entry += 1;             \
    }

#define _EXIT_CRITICAL()                       \
    {                                          \
        if (s_irq_entry > 0) {                 \
            s_irq_entry -= 1;                  \
            if (s_irq_entry == 0) {            \
                bflb_irq_restore(s_irq_flag); \
            }                                  \
        }                                      \
    }

#endif

#ifdef CONFIG_FREERTOS
#define CONFIG_MEM_USE_OS
#endif

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/* Adapted to the lock provided by the operating system */
#ifdef CONFIG_FREERTOS
typedef StaticSemaphore_t sem_t;
typedef TaskHandle_t task_t;
#else

#endif /* CONFIG_FREERTOS */

struct mem_delaynode_s
{
    struct mem_delaynode_s *flink;
};

/* Private memory management structure */
struct mem_heap_impl_s {
    struct heap_info info;
#ifdef CONFIG_MEM_USE_OS
    sem_t sem;

    /* Free delay list, for some situation can't do free immdiately */

    struct mem_delaynode_s *mem_delaylist;
#endif
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: bflb_mem_sem_init
 *
 * Description:
 *   Initialize the MM mutex
 *
 ****************************************************************************/

static void bflb_mem_sem_init(struct mem_heap_impl_s *impl)
{
#ifdef CONFIG_MEM_USE_OS
    /* Initialize the MM semaphore to one (to support one-at-a-time access to
     * private data sets).
     */

    _SEM_INIT(&impl->sem, 0, 1);
#endif /* CONFIG_MEM_USE_OS */
}

/****************************************************************************
 * Name: bflb_mem_sem_take
 *
 * Description:
 *   Take the MM mutex.  This is the normal action before all memory
 *   management actions.
 *
 ****************************************************************************/

static void bflb_mem_sem_take(struct mem_heap_impl_s *impl)
{
#ifdef CONFIG_MEM_USE_OS
    if (!_SCHED_LOCK()) {
        _SEM_WAIT(&impl->sem);
    } else {
        _ENTER_CRITICAL();
    } 
#else

    _ENTER_CRITICAL();

#endif /* CONFIG_MEM_USE_OS */
}

/****************************************************************************
 * Name: bflb_mem_sem_give
 *
 * Description:
 *   Release the MM mutex when it is not longer needed.
 *
 ****************************************************************************/

static void bflb_mem_sem_give(struct mem_heap_impl_s *impl)
{
#ifdef CONFIG_MEM_USE_OS
    if (!_SCHED_LOCK()) {
        _SEM_POST(&impl->sem);
    } else {
        _EXIT_CRITICAL();
    } 
#else

    _EXIT_CRITICAL();

#endif /* CONFIG_MEM_USE_OS */
}

/****************************************************************************
 * Name: bflb_mem_add_delaylist
 ****************************************************************************/

static void bflb_mem_add_delaylist(struct mem_heap_s *heap, void *mem)
{
#ifdef CONFIG_MEM_USE_OS
    struct mem_heap_impl_s *impl;
    struct mem_delaynode_s *tmp = mem;

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    /* Delay the deallocation until a more appropriate time. */

    _ENTER_CRITICAL();

    tmp->flink = impl->mem_delaylist;
    impl->mem_delaylist = tmp;

    _EXIT_CRITICAL();
#endif /* CONFIG_MEM_USE_OS */
}

/****************************************************************************
 * Name: bflb_mem_free_delaylist
 ****************************************************************************/

static void bflb_mem_free_delaylist(struct mem_heap_s *heap)
{
#ifdef CONFIG_MEM_USE_OS
    struct mem_heap_impl_s *impl;
    struct mem_delaynode_s *tmp;

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    /* Move the delay list to local */

    _ENTER_CRITICAL();

    tmp = impl->mem_delaylist;
    impl->mem_delaylist = NULL;

    _EXIT_CRITICAL();

    /* Test if the delayed is empty */

    while (tmp) {
        void *address;

        /* Get the first delayed deallocation */

        address = tmp;
        tmp = tmp->flink;

        /* The address should always be non-NULL since that was checked in the
         * 'while' condition above.
         */

        bflb_free(heap, address);
    }
#endif /* CONFIG_MEM_USE_OS */
}

/****************************************************************************
 * Functions
 ****************************************************************************/

void bflb_mem_init(struct mem_heap_s *heap, void *heapstart, size_t heapsize)
{
    struct mem_heap_impl_s *impl;
    struct heap_region region[] = {
        { NULL, 0 },
        { NULL, 0 }
    };

    /* Reserve a block space for mm_heap_impl_s context */

    MEM_ASSERT(heapsize > sizeof(struct mem_heap_impl_s));
    
    heap->mem_impl = (struct mem_heap_impl_s *)heapstart;

    heapstart += sizeof(struct mem_heap_impl_s);
    heapsize -= sizeof(struct mem_heap_impl_s);

    region[0].addr = heapstart;
    region[0].mem_size = heapsize;

    /* Zero implmeentation context */

    impl = heap->mem_impl;
    memset(impl, 0, sizeof(struct mem_heap_impl_s));

    bflb_mem_sem_init(impl);

    bflb_mmheap_init(&impl->info, region);
}

void *bflb_malloc(struct mem_heap_s *heap, size_t nbytes)
{
    struct mem_heap_impl_s *impl;
    void *ret = NULL;

    MEM_ASSERT(MEM_IS_VALID(heap));

    MEM_LOG("malloc %d\r\n", nbytes);

    impl = heap->mem_impl;

    /* Firstly, free mm_delaylist */

    bflb_mem_free_delaylist(heap);

    /* Allocate from the tlsf pool */

    bflb_mem_sem_take(impl);
    ret = bflb_mmheap_alloc(&impl->info, nbytes);
    bflb_mem_sem_give(impl);

    return ret;
}

void bflb_free(struct mem_heap_s *heap, void *ptr)
{
    struct mem_heap_impl_s *impl;

    MEM_LOG("Freeing %p\r\n", ptr);

    /* Protect against attempts to free a NULL reference */

    if (!ptr) {
        return;
    }

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    if (_IRQ_CONTEXT()) {

        /* We are in ISR, add to mm_delaylist */

        bflb_mem_add_delaylist(heap, ptr);
        return;
    }

    /* We need to hold the MM semaphore while we muck with the
     * nodelist.
     */

    bflb_mem_sem_take(impl);

    /* Return to the tlsf pool */

    bflb_mmheap_free(&heap->mem_impl->info, ptr);

    bflb_mem_sem_give(impl);
}

void *bflb_realloc(struct mem_heap_s *heap, void *ptr, size_t nbytes)
{
    struct mem_heap_impl_s *impl;
    void *ret;

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    /* Firstly, free mm_delaylist */

    bflb_mem_free_delaylist(heap);

    /* Allocate from the tlsf pool */

    bflb_mem_sem_take(impl);
    ret = bflb_mmheap_realloc(&impl->info, ptr, nbytes);
    bflb_mem_sem_give(impl);

    return ret;
}

void *bflb_calloc(struct mem_heap_s *heap, size_t count, size_t nbytes)
{
    struct mem_heap_impl_s *impl;
    void *ret;

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    /* Firstly, free mm_delaylist */

    bflb_mem_free_delaylist(heap);

    /* Allocate from the tlsf pool */

    bflb_mem_sem_take(impl);
    ret = bflb_mmheap_calloc(&impl->info, count, nbytes);
    bflb_mem_sem_give(impl);

    return ret;
}

void *bflb_malloc_align(struct mem_heap_s *heap, size_t align, size_t size)
{
    struct mem_heap_impl_s *impl;
    void *ret;

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    /* Firstly, free mm_delaylist */

    bflb_mem_free_delaylist(heap);

    /* Allocate from the tlsf pool */

    bflb_mem_sem_take(impl);
    ret = bflb_mmheap_align_alloc(&impl->info, align, size);
    bflb_mem_sem_give(impl);

    return ret;
}

void bflb_mem_usage(struct mem_heap_s *heap, struct meminfo *info)
{
    struct mem_heap_impl_s *impl;
    struct heap_state state;

    MEM_ASSERT(MEM_IS_VALID(heap));
    impl = heap->mem_impl;

    bflb_mmheap_get_state(&impl->info, &state);

    info->total_size = impl->info.total_size;
    info->free_node = state.free_node_num;
    info->max_free_size = state.max_node_size;
    info->free_size = state.remain_size;
    info->used_size = info->total_size - info->free_size;
}
