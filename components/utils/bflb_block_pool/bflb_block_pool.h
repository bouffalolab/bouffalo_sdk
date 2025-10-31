/**
 * @file bflb_block_pool.h
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

#ifndef _BFLB_BLOCK_POOL_H
#define _BFLB_BLOCK_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define BFLB_BLOCK_POOL_ALIGN_1   0x00
#define BFLB_BLOCK_POOL_ALIGN_2   0x01
#define BFLB_BLOCK_POOL_ALIGN_4   0x02
#define BFLB_BLOCK_POOL_ALIGN_8   0x03
#define BFLB_BLOCK_POOL_ALIGN_16  0x04
#define BFLB_BLOCK_POOL_ALIGN_32  0x05
#define BFLB_BLOCK_POOL_ALIGN_64  0x06
#define BFLB_BLOCK_POOL_ALIGN_128 0x07

typedef struct {
    void *free_list;    /*!< free block list          */
    void *pool_addr;    /*!< pool start pointer       */
    void *blk_addr;     /*!< block area start pointer */
    uint32_t blk_size;  /*!< block size               */
    uint32_t blk_align; /*!< block align              */
    uint32_t blk_total; /*!< total block num          */
    uint32_t blk_free;  /*!< free block num           */

    int (*mtx_get)(uint32_t wait);
    void (*mtx_put)(void);
    int (*sem_get)(uint32_t wait);
    void (*sem_put)(void);
} bflb_block_pool_t;

extern int bflb_block_pool_create(bflb_block_pool_t *blk_pool, uint32_t blk_size, uint32_t blk_align, void *pool_addr, uint32_t pool_size);
extern int bflb_block_pool_delete(bflb_block_pool_t *blk_pool);

extern int bflb_block_pool_add_mtx(bflb_block_pool_t *blk_pool, int (*get)(uint32_t wait), void (*put)(void));
extern int bflb_block_pool_add_sem(bflb_block_pool_t *blk_pool, int (*get)(uint32_t wait), void (*put)(void));

extern int bflb_block_pool_alloc(bflb_block_pool_t *blk_pool, void **addr, uint32_t wait);
extern int bflb_block_pool_free(bflb_block_pool_t *blk_pool, void *addr);

extern void bflb_block_pool_info_get(bflb_block_pool_t *blk_pool, uint32_t *total_blk, uint32_t *free_blk);

#ifdef __cplusplus
}
#endif

#endif
