/**
 * @file drv_mmheap.h
 * @brief 
 * 
 * Copyright (c) 2021 Bouffalolab team
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
 */
#ifndef _DRV_MMHEAP_H_
#define _DRV_MMHEAP_H_

#include "stdint.h"
#include "string.h"

#define MEMHEAP_STATUS_OK 0
#define MEMHEAP_STATUS_INVALID_ADDR -1
#define MEMHEAP_STATUS_INVALID_SIZE -2
#define MEMHEAP_STATUS_OVERFLOW -3
#define MEMHEAP_STATUS_ALREADY_NOT_EXIST -4
#define MEMHEAP_STATUS_ALREADY_EXIST -5

/**
 * log2 of number of linear subdivisions of block sizes. Larger
 * values require more memory in the control structure. Values of
 * 4 or 5 are typical.
 */
#define MMHEAP_SL_INDEX_COUNT_LOG2    5

/* All allocation sizes and addresses are aligned to 4 bytes. */
#define MMHEAP_ALIGN_SIZE_LOG2        2
#define MMHEAP_ALIGN_SIZE             (1 << MMHEAP_ALIGN_SIZE_LOG2)

/*
 * We support allocations of sizes up to (1 << MMHEAP_FL_INDEX_MAX) bits.
 * However, because we linearly subdivide the second-level lists, and
 * our minimum size granularity is 4 bytes, it doesn't make sense to
 * create first-level lists for sizes smaller than K_MMHEAP_SL_INDEX_COUNT * 4,
 * or (1 << (K_MMHEAP_SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
 * trying to split size ranges into more slots than we have available.
 * Instead, we calculate the minimum threshold size, and place all
 * blocks below that size into the 0th first-level list.
 */
#define MMHEAP_FL_INDEX_MAX           30
#define MMHEAP_SL_INDEX_COUNT         (1 << MMHEAP_SL_INDEX_COUNT_LOG2)
#define MMHEAP_FL_INDEX_SHIFT         (MMHEAP_SL_INDEX_COUNT_LOG2 + MMHEAP_ALIGN_SIZE_LOG2)
#define MMHEAP_FL_INDEX_COUNT         (MMHEAP_FL_INDEX_MAX - MMHEAP_FL_INDEX_SHIFT + 1)

#define MMHEAP_SMALL_BLOCK_SIZE       (1 << MMHEAP_FL_INDEX_SHIFT)

#define MMHEAP_BLOCK_CURR_FREE        (1 << 0)
#define MMHEAP_BLOCK_PREV_FREE        (1 << 1)
#define MMHEAP_BLOCK_SIZE_MASK        ~(MMHEAP_BLOCK_CURR_FREE | MMHEAP_BLOCK_PREV_FREE)
#define MMHEAP_BLOCK_STATE_MASK       (MMHEAP_BLOCK_CURR_FREE | MMHEAP_BLOCK_PREV_FREE)

typedef struct 
{
    uint32_t    used; /* space is used */
    uint32_t    free; /* space is free */
} mmheap_info_t;

/**
 * Block structure.
 *
 * There are several implementation subtleties involved:
 * - The prev_phys_block field is only valid if the previous block is free.
 * - The prev_phys_block field is actually stored at the end of the
 *   previous block. It appears at the beginning of this structure only to
 *   simplify the implementation.
 * - The next_free / prev_free fields are only valid if the block is free.
 */
typedef struct mmheap_blk_st {
    struct mmheap_blk_st *prev_phys_blk;

    size_t size;

    struct mmheap_blk_st *next_free;
    struct mmheap_blk_st *prev_free;
} mmheap_blk_t;

/**
 * A free block must be large enough to store its header minus the size of
 * the prev_phys_block field, and no larger than the number of addressable
 * bits for FL_INDEX.
 */
#define MMHEAP_BLK_SIZE_MIN           (sizeof(mmheap_blk_t) - sizeof(mmheap_blk_t *))
#define MMHEAP_BLK_SIZE_MAX           (1 << MMHEAP_FL_INDEX_MAX)

#define MMHEAP_BLK_HEADER_OVERHEAD    (sizeof(size_t))
#define MMHEAP_BLK_START_OFFSET       (((uint32_t)&(((mmheap_blk_t *)0)->size))+ sizeof(size_t))

#define MMHEAP_POOL_MAX               3

/**
 * memory heap control
 */
typedef struct 
{
    int             pool_cnt;
    void           *pool_start[MMHEAP_POOL_MAX];

    mmheap_blk_t    block_null; /**< Empty lists point at this block to indicate they are free. */

    uint32_t        fl_bitmap; /**< Bitmaps for free lists. */
    uint32_t        sl_bitmap[MMHEAP_FL_INDEX_COUNT];

    mmheap_blk_t   *blocks[MMHEAP_FL_INDEX_COUNT][MMHEAP_SL_INDEX_COUNT]; /**< Head of free lists. */
} mmheap_ctl_t;

/**
 * @brief Add a pool.
 * Add addtional pool to the heap.
 *
 * @attention None
 *
 * @param[in]   pool_start  start address of the pool.
 * @param[in]   pool_size   size of the pool.
 *
 * @return  errcode
 */
int mmheap_pool_add(void *pool_start, size_t pool_size);

/**
 * @brief Alloc memory.
 * Allocate size bytes and returns a pointer to the allocated memory.
 *
 * @attention size should no bigger than MMHEAP_BLK_SIZE_MAX.
 *
 * @param[in]   size    size of the memory.
 *
 * @return  the pointer to the allocated memory.
 */
void   *mmheap_alloc(size_t size);

void   *mmheap_calloc(size_t num, size_t size);

/**
 * @brief Alloc start address aligned memory from the heap.
 * Alloc aligned address and specified size memory from the heap.
 *
 * @attention
 *
 * @param[in]   size    size of the memory.
 * @param[in]   align   address align mask of the memory.
 *
 * @return  the pointer to the allocated memory.
 */
void   *mmheap_aligned_alloc(size_t size, size_t align);

/**
 * @brief Realloc memory from the heap.
 * Change the size of the memory block pointed to by ptr to size bytes.
 *
 * @attention
 * <ul>
 * <li> if ptr is NULL, then the call is equivalent to mmheap_alloc(size), for all values of size.
 * <li> if ptr is if size is equal to zero, and ptr is not K_NULL, then the call is equivalent to mmheap_free(ptr).
 * </ul>
 *
 * @param[in]   ptr     old pointer to the memory space.
 * @param[in]   size    new size of the memory space.
 *
 * @return  the new pointer to the allocated memory.
 */
void   *mmheap_realloc(void *ptr, size_t size);

/**
 * @brief Free the memory.
 * Free the memory space pointed to by ptr, which must have been returned by a previous call to mmheap_alloc(), mmheap_aligned_alloc(), or mmheap_realloc().
 *
 * @attention
 *
 * @param[in]   ptr     pointer to the memory.
 *
 * @return  None.
 */
void    mmheap_free(void *ptr);


int mmheap_init_with_pool(void *pool_start, size_t pool_size);


#endif /* _DRV_MMHEAP_H_ */