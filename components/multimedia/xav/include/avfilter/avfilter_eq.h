/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_EQ_H__
#define __AVFILTER_EQ_H__

#include "avfilter/avfilter.h"
#include "avutil/av_typedef.h"
#include "aef/eq_cls.h"

__BEGIN_DECLS__

#define AVF_CMD_EQ_SET_ENABLE      (1)
#define AVF_CMD_EQ_SET_PARAM       (2)

typedef struct {
    sf_t                      sf;
    uint8_t                   eq_segments;
} eq_avfp_t;

//AVF_CMD_EQ_SET_PARAM
typedef struct {
    uint8_t                   segid;
    eqfp_t                    param;
} eq_setpa_t;

//AVF_CMD_EQ_SET_ENABLE
typedef struct {
    uint8_t                   enable;
} eq_seten_t;

/**
 * @brief  open eq avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] eqp
 * @return NULL on error
 */
avfilter_t *avf_eq_open(const char *inst_name, const eq_avfp_t *eqp);

#define avf_eq_control(f, cmd, arg, arg_size) avf_control(f, cmd, arg, arg_size)

#define avf_eq_filter_frame(f, i, o) avf_filter_frame(f, i, o)

#define avf_eq_close(f) avf_close(f)

__END_DECLS__

#endif /* __AVFILTER_EQ_H__ */

