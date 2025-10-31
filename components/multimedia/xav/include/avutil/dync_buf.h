/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __DYNC_BUF_H__
#define __DYNC_BUF_H__

#include "avutil/common.h"

__BEGIN_DECLS__

#define DYNC_BUF_IS_UNLIMITED(capacity) (0 == capacity)

typedef struct dync_buf {
    size_t     len;
    size_t     capacity;
    size_t     max_capacity;
    char       *data;
} dync_buf_t;

/**
 * @brief  init the dynamic buf
 * @param  [in] buf
 * @param  [in] capacity     : initial size of the dbuf
 * @param  [in] max_capacity : space of the dynamic buf will not exceed the value
 *                             0 present unlimited
 * @return
 */
int dync_buf_init(dync_buf_t *dbuf, size_t capacity, size_t max_capacity);

/**
 * @brief  increase capacity of the dynamic buf
 * @param  [in] dbuf
 * @param  [in] inc : increase size
 * @return 0/-1
 */
int dync_buf_increase(dync_buf_t *dbuf, size_t inc);

/**
 * @brief  add char to the dbuf
 * @param  [in] dbuf
 * @param  [in] ch
 * @return 0/-1
 */
int dync_buf_add_char(dync_buf_t *dbuf, char ch);

/**
 * @brief  add buf to the dbuf
 * @param  [in] dbuf
 * @param  [in] data
 * @param  [in] len
 * @return 0/-1
 */
int dync_buf_add_bytes(dync_buf_t *dbuf, const void *data, size_t len);

/**
 * @brief  add string to the dbuf
 * @param  [in] dbuf
 * @param  [in] str
 * @return 0/-1
 */
int dync_buf_add_string(dync_buf_t *dbuf, const char *str);

/**
 * @brief  add format data to the dbuf
 * @param  [in] dbuf
 * @param  [in] fmt
 * @return 0/-1
 */
int dync_buf_add_fmt(dync_buf_t *dbuf, const char *fmt, ...);

/**
 * @brief  reset the dbuf(not release related resource)
 * @param  [in] dbuf
 * @return 0/-1
 */
int dync_buf_reset(dync_buf_t *dbuf);

/**
 * @brief  get the availabe write size
 * @param  [in] dbuf
 * @return -1 on err
 */
int dync_buf_get_wlen(dync_buf_t *dbuf);

/**
 * @brief  get available write pos and size
 * @param  [in] dbuf
 * @param  [in] pos
 * @return availiable write size
 */
int dync_buf_get_wpos(dync_buf_t *dbuf, char **pos);

/**
 * @brief  set write size
 * @param  [in] dbuf
 * @param  [in] count
 * @return 0/-1
 */
int dync_buf_set_wpos(dync_buf_t *dbuf, size_t count);

/**
 * @brief  get data start point of the dbuf
 * @param  [in] dbuf
 * @return NULL on err
 */
char* dync_buf_get_data(dync_buf_t *dbuf);

/**
 * @brief  get the actully take size
 * @param  [in] dbuf
 * @return -1 on err
 */
int dync_buf_get_len(dync_buf_t *dbuf);

/**
 * @brief  uninit the dbug
 * @param  [in] dbuf
 * @return 0/-1
 */
int dync_buf_uninit(dync_buf_t *dbuf);

__END_DECLS__

#endif /* __DYNC_BUF_H__ */

