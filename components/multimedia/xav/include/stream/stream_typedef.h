/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STREAM_TYPEDEF_H__
#define __STREAM_TYPEDEF_H__

#include "avutil/common.h"

__BEGIN_DECLS__

enum stream_mode {
    STREAM_CREATE  = (1<<0),
    STREAM_READ    = (1<<1),
    STREAM_WRITE   = (1<<2),
};

typedef enum stream_type {
    STREAM_TYPE_UNKNOWN,
    STREAM_TYPE_FILE,
    STREAM_TYPE_HTTP,
    STREAM_TYPE_ASHX,
    STREAM_TYPE_MEM,
    STREAM_TYPE_FIFO,
    STREAM_TYPE_CRYPTO,
    STREAM_TYPE_HLS,
    STREAM_TYPE_DASH,
} stream_type_t;

__END_DECLS__

#endif /* __STREAM_TYPEDEF_H__ */

