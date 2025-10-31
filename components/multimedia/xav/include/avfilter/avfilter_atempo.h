/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_ATEMPO_H__
#define __AVFILTER_ATEMPO_H__

#include "avutil/sf.h"
#include "avfilter/avfilter.h"

__BEGIN_DECLS__

#define AVF_CMD_ATEMPO_SET_SPEED  (20)

typedef struct {
    sf_t                      sf;
    float                     speed;          ///< suggest: 0.5 ~ 2.0;
} atempo_avfp_t;

/**
 * @brief  open atempo avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] atempop
 * @return NULL on error
 */
avfilter_t *avf_atempo_open(const char *inst_name, const atempo_avfp_t *atempop);

#define avf_atempo_control(f, cmd, arg, arg_size) avf_control(f, cmd, arg, arg_size)

#define avf_atempo_filter_frame(f, i, o) avf_filter_frame(f, i, o)

#define avf_atempo_close(f) avf_close(f)

__END_DECLS__

#endif /* __AVFILTER_ATEMPO_H__ */

