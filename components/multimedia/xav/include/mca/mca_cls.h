/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCA_CLS_H__
#define __MCA_CLS_H__

#include "avutil/common.h"
#include "mca/csky_mca.h"

__BEGIN_DECLS__

typedef struct mcax                         mcax_t;
typedef struct mcax_ops                     mcax_ops_t;

struct mcax {
    int32_t                                 type;

    void                                    *priv;
    const struct mcax_ops                   *ops;
    msp_mutex_t                             lock;
};

struct mcax_ops {
    const char                              *name;

    int      (*init)                        (mcax_t *mca, int32_t type);
    int      (*iir_fxp32_coeff32_config)    (mcax_t *mca, const fxp32_t *coeff);
    int      (*iir_fxp32)                   (mcax_t *mca, const fxp32_t *input, size_t input_size,
            fxp32_t yn1, fxp32_t yn2, fxp32_t *output);
    int      (*uninit)                      (mcax_t *mca);
};

__END_DECLS__

#endif /* __MCA_CLS_H__ */

