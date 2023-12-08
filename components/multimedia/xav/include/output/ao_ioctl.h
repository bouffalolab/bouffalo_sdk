/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AO_IOCTL_H__
#define __AO_IOCTL_H__

#include "aef/eq_cls.h"

__BEGIN_DECLS__

enum {
    AO_CMD_UNKNOWN,
    AO_CMD_EQ_ENABLE,        ///< equalizer enable, may be not support for some hardware.
    AO_CMD_EQ_SET_PARAM,     ///< equalizer param config, may be not support for some hardware.
    AO_CMD_VOL_SET,          ///< soft vol
    AO_CMD_ATEMPO_SET_SPEED, ///< atempo play speed set
};

/* equalizer typedef below */
//AO_CMD_EQ_SET_PARAM
typedef struct {
    uint8_t                   segid;
    eqfp_t                    param;
} oeq_setpa_t;

//AO_CMD_EQ_SET_ENABLE
typedef struct {
    uint8_t                   enable;
} oeq_seten_t;

//AO_CMD_VOL_SET
typedef struct {
    uint8_t                   vol_index;      ///< soft vol scale index (0~255)
} ovol_set_t;

//AO_CMD_ATEMPO_SET_SPEED
/* float speed; */

__END_DECLS__

#endif /* __AO_IOCTL_H__ */

