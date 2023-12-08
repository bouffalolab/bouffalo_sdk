/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVPARSER_H__
#define __AVPARSER_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avformat/avparser_cls.h"

__BEGIN_DECLS__

#define AVPARSER_OPS_MAX            (6)

/**
 * @brief  regist avparser ops
 * @param  [in] ops
 * @return 0/-1
 */
int avparser_ops_register(const struct avparser_ops *ops);

/**
 * @brief  open/create one avparser
 * @param  [in] id
 * @param  [in] extradata
 * @param  [in] extradata_size
 * @return NULL on err
 */
avparser_t* avparser_open(avcodec_id_t id, uint8_t *extradata, size_t extradata_size);

/**
 * @brief  parse the in-buf
 * @param  [in] psr
 * @param  [in] ibuf
 * @param  [in] isize
 * @param  [in] ipos  : new pos of ibuf parsed
 * @param  [in] obuf  : out buf after parsed
 * @param  [in] osize
 * @return 0/-1
 */
int avparser_parse(avparser_t *psr, const uint8_t *ibuf, size_t isize, size_t *ipos, uint8_t **obuf, size_t *osize);

/**
 * @brief  close/destroy avparser
 * @param  [in] psr
 * @return 0/-1
 */
int avparser_close(avparser_t *psr);

__END_DECLS__

#endif /* __AVPARSER_H__ */

