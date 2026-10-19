/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STRAIGHT_FIFO_H__
#define __STRAIGHT_FIFO_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct straight_fifo sfifo_t;

/**
 * @brief  create a straight r/w fifo
 * @param  [in] size
 * @return NULL on err
 */
sfifo_t* sfifo_create(size_t size);

/**
 * @brief  get straight read pos
 * @param  [in] fifo
 * @param  [out] pos
 * @param  [in] timeout : ms
 * @return straight read size
 */
int sfifo_get_rpos(sfifo_t* fifo, char **pos, uint32_t timeout);

/**
 * @brief  set the read pos after read
 * @param  [in] fifo
 * @param  [in] count
 * @return 0/-1
 */
int sfifo_set_rpos(sfifo_t* fifo, size_t count);

/**
 * @brief  get straight write pos
 * @param  [in] fifo
 * @param  [out] pos
 * @param  [in] timeout : ms
 * @return straight write size
 */
int sfifo_get_wpos(sfifo_t* fifo, char **pos, uint32_t timeout);

/**
 * @brief  set the write pos after write
 * @param  [in] fifo
 * @param  [in] count
 * @return 0/-1
 */
int sfifo_set_wpos(sfifo_t* fifo, size_t count);

/**
 * @brief  set eof to the fifo before destroy, may be block if don't call this fun
 * @param  [in] fifo
 * @param  [in] reof : sfifo_get_rpos will return immediately when set
 * @param  [in] weof : sfifo_get_wpos will return immediately when set
 * @return 0/-1
 */
int sfifo_set_eof(sfifo_t* fifo, uint8_t reof, uint8_t weof);

/**
 * @brief  get eof flag from the fifo
 * @param  [in] fifo
 * @param  [out] reof : read eof flag
 * @param  [out] weof : write eof flag
 * @return 0/-1
 */
int sfifo_get_eof(sfifo_t* fifo, uint8_t *reof, uint8_t *weof);

/**
 * @brief  reset the fifo
 * @param  [in] fifo
 * @return 0/-1
 */
int sfifo_reset(sfifo_t *fifo);

/**
 * @brief  get valid data len of the fifo
 * @param  [in] fifo
 * @return -1 on error
 */
int sfifo_get_len(sfifo_t *fifo);

/**
 * @brief  destroy the fifo
 * @param  [in] fifo
 * @return 0/-1
 */
int sfifo_destroy(sfifo_t *fifo);

__END_DECLS__

#endif /* __STRAIGHT_FIFO_H__ */

