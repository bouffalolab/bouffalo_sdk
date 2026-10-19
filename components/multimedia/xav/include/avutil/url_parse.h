/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __URL_PARSE_H__
#define __URL_PARSE_H__

#include "avutil/common.h"

__BEGIN_DECLS__

/**
 * @brief  get item value from a url
 * @param  [in] url
 * @param  [in] item : name of the item
 * @param  [out] value
 * @param  [in] len  : length of the value
 * @return 0/-1
 */
int url_get_item_value(const char *url, const char *item, char *value, size_t len);

/**
 * @brief  get item int value from a url
 * @param  [in] url
 * @param  [in] item : name of the item
 * @param  [out] value
 * @return 0/-1
 */
int url_get_item_value_int(const char *url, const char *item, int *value);

/**
 * @brief  get item long value from a url
 * @param  [in] url
 * @param  [in] item : name of the item
 * @param  [out] value
 * @return 0/-1
 */
int url_get_item_value_long(const char *url, const char *item, long *value);

__END_DECLS__

#endif /* __URL_PARSE_H__ */

