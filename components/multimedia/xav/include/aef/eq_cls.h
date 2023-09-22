/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQ_CLS_H__
#define __EQ_CLS_H__

#include "avutil/eq_typedef.h"

__BEGIN_DECLS__

typedef struct equalizer      eqx_t;
typedef struct eqx_ops        eqx_ops_t;

struct equalizer {
    uint32_t                  rate;
    uint8_t                   enable;
    uint8_t                   eq_segments;
    size_t                    nsamples_max;
    eqfp_t                    *params;

    void                      *priv;
    const struct eqx_ops      *ops;
    msp_mutex_t               lock;
};

struct eqx_ops {
    const char                *name;

    int      (*init)          (eqx_t *eq, uint32_t rate, uint8_t eq_segments);
    int      (*set_enable)    (eqx_t *eq, uint8_t enable);
    int      (*set_param)     (eqx_t *eq, uint8_t segid, const eqfp_t *param);
    int      (*process)       (eqx_t *eq, const int16_t *in, int16_t *out, size_t nb_samples);
    int      (*uninit)        (eqx_t *eq);
};

__END_DECLS__


#endif /* __EQ_CLS_H__ */

