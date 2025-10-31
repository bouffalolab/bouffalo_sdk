/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQ_TYPEDEF_H__
#define __EQ_TYPEDEF_H__

#include "avutil/common.h"

__BEGIN_DECLS__

enum eqf_type {
    EQF_TYPE_UNKNOWN,
    EQF_TYPE_PEAK,
    EQF_TYPE_NOTCH,
    EQF_TYPE_LP1,
    EQF_TYPE_HP1,
    EQF_TYPE_LP2,
    EQF_TYPE_HP2,
    EQF_TYPE_BP2,
    EQF_TYPE_MAX,
};

/* eq filter-param per segment*/
typedef struct {
    uint8_t                   enable;
    enum eqf_type             type;
    float                     gain;     ///< -24~18
    float                     q;        ///< 0.1~50
    uint32_t                  rate;
} eqfp_t;

__END_DECLS__

#endif /* __EQ_TYPEDEF_H__ */

