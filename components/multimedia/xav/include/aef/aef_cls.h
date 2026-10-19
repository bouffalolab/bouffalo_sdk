/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AEF_CLS_H__
#define __AEF_CLS_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct audio_effect   aefx_t;
typedef struct aefx_ops       aefx_ops_t;

struct audio_effect {
    uint32_t                  rate;
    uint8_t                   *conf;
    size_t                    conf_size;
    size_t                    nsamples_max;   ///< the max frame len for input on processing

    void                      *priv;
    const struct aefx_ops     *ops;
    msp_mutex_t               lock;
};

struct aefx_ops {
    const char                *name;

    int      (*init)          (aefx_t *aef);
    int      (*process)       (aefx_t *aef, const int16_t *in, int16_t *out, size_t nb_samples);
    int      (*uninit)        (aefx_t *aef);
};

__END_DECLS__

#endif /* __AEF_CLS_H__ */

