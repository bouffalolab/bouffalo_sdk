/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ATEMPO_CLS_H__
#define __ATEMPO_CLS_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct atempo_ops     atempo_ops_t;

typedef struct {
    uint32_t                  rate;           ///< use rate now
    float                     speed;          ///< suggest: 0.5 ~ 2.0;

    void                      *priv;
    const struct atempo_ops   *ops;
    msp_mutex_t               lock;
} atempo_t;

struct atempo_ops {
    const char                *name;

    int      (*init)          (atempo_t *atempo);
    int      (*write)         (atempo_t *atempo, const int16_t *in, size_t isamples);
    int      (*read)          (atempo_t *atempo, int16_t *out, size_t osamples);
    int      (*set_speed)     (atempo_t *atempo, float speed);
    int      (*flush)         (atempo_t *atempo);
    int      (*uninit)        (atempo_t *atempo);
};

__END_DECLS__

#endif /* __ATEMPO_CLS_H__ */

