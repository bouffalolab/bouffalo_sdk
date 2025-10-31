/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MEM_BLOCK_H__
#define __MEM_BLOCK_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct {
    void            *data;
    void            *data_orip; ///< original pointer

    size_t          size;       ///< total capcity
    size_t          used;       ///< length of avilable data
    size_t          align;      ///< address of the data is align when addr_align != 0
} mblock_t;

/**
 * @brief  new a mem block
 * @param  [in] size
 * @param  [in] align : address of the data is align when addr_align != 0
 * @return 0/-1
 */
mblock_t* mblock_new(size_t size, size_t align);

/**
 * @brief  grow the mem block data size
 * @param  [in] mb
 * @param  [in] size
 * @return 0/-1
 */
int mblock_grow(mblock_t *mb, size_t size);

/**
 * @brief  free resource of the mem block
 * @param  [in] mb
 * @return
 */
int mblock_free(mblock_t *mb);

__END_DECLS__

#endif /* __MEM_BLOCK_H__ */

