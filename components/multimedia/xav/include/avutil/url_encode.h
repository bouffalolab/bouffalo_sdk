/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __URL_ENCODE_H__
#define __URL_ENCODE_H__

#include "avutil/common.h"

__BEGIN_DECLS__

/**
 * @brief  url string decode
 * @param  [in] src
 * @param  [in] src_size
 * @param  [in] dst
 * @param  [in] dst_size
 * @return -1 on error
 */
int url_decode(const char* src, const size_t src_size, char* dst, const size_t dst_size);

/**
 * @brief  encode the origin url
 * @param  [in] src
 * @param  [in] src_size
 * @param  [in] dst
 * @param  [in] dst_size
 * @return -1 on error
 */
int url_encode(const char* src, const size_t src_size, char* dst, const size_t dst_size);

/**
 * @brief  encode the url(don't encode :/$#@, etc)
 * @param  [in] s
 * attention: need free the return val by caller
 * @return NULL on error
 */
char* url_encode2(const char* s);

__END_DECLS__

#endif /* __URL_ENCODE_H__ */

