/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __DICT_H__
#define __DICT_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct {
    char *key;
    char *val;
} keyval_t;

typedef struct {
    keyval_t *keyvals;
    int      alloc;
    int      count;
} dict_t;

/**
 * @brief  init the dict
 * @param  [in] d
 * @param  [in] capacity : the number of key-val pair in the dict
 * @return 0/-1
 */
int dict_init(dict_t *d, int capacity);

/**
 * @brief  increase capacity of the dict
 * @param  [in] d
 * @param  [in] inc : increase size
 * @return 0/-1
 */
int dict_increase(dict_t *d, size_t inc);

/**
 * @brief  add one kv pair
 * @param  [in] d
 * @param  [in] key
 * @param  [in] val
 * @return 0/-1
 */
int dict_add(dict_t *d, const char *key, const char *val);

/**
 * @brief  add one kv pair(set len of the val)
 * @param  [in] d
 * @param  [in] key
 * @param  [in] val
 * @param  [in] valn : length of the val string
 * @return 0/-1
 */
int dict_addn(dict_t *d, const char *key, const char *val, size_t valn);

/**
 * @brief  add one kv pair(the val is integer)
 * @param  [in] d
 * @param  [in] key
 * @param  [in] val
 * @return 0/-1
 */
int dict_add_int(dict_t *d, const char *key, int val);

/**
 * @brief  reduce memory of the dict
 * @param  [in] d
 * @return 0/-1
 */
int dict_shrink(dict_t *d);

/**
 * @brief  get count of the real kv pair
 * @param  [in] d
 * @return -1 on err
 */
int dict_count(const dict_t *d);

/**
 * @brief  uninit the dict
 * @param  [in] d
 * @return 0/-1
 */
int dict_uninit(dict_t *d);

/**
 * @brief  get the value by key
 * @param  [in] d
 * @param  [in] key
 * @return NULL on err
 */
const char *dict_get_val(const dict_t *d, const char *key);

__END_DECLS__

#endif /* __DICT_H__ */

