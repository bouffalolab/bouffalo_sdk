/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVPARSER_CLS_H__
#define __AVPARSER_CLS_H__

#include "avutil/av_typedef.h"
#include "avutil/avpacket.h"
#include "stream/stream_cls.h"

__BEGIN_DECLS__

typedef struct avparser         avparser_t;

struct avparser_ops {
    const char                  *name;
    avcodec_id_t                id;

    int      (*open)            (avparser_t *psr, uint8_t *extradata, size_t extradata_size);
    int      (*parse)           (avparser_t *psr, const uint8_t *ibuf, size_t isize, size_t *ipos, uint8_t **obuf, size_t *osize);
    int      (*close)           (avparser_t *psr);
};

struct avparser {
    msp_mutex_t                 lock;
    void                        *priv;
    const struct avparser_ops   *ops;
};

__END_DECLS__

#endif /* __AVPARSER_CLS_H__ */

