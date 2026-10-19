/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_VOL_H__
#define __AVFILTER_VOL_H__

#include "avfilter/avfilter.h"

__BEGIN_DECLS__

#define AVF_CMD_VOL_SET       (10)

//AVF_CMD_VOL_SET
typedef struct {
    uint8_t                   vol_index;      ///< soft vol scale index (0~255)
} vol_set_t;

/**
 * @brief  open vol avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] vol_index : soft vol scale index (0~255)
 * @return NULL on error
 */
avfilter_t *avf_vol_open(const char *inst_name, uint8_t vol_index);

#define avf_vol_control(f, cmd, arg, arg_size) avf_control(f, cmd, arg, arg_size)

#define avf_vol_filter_frame(f, i, o) avf_filter_frame(f, i, o)

#define avf_vol_close(f) avf_close(f)

__END_DECLS__

#endif /* __AVFILTER_VOL_H__ */

