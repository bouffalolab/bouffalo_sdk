/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_ATEMPO2_H__
#define __AVFILTER_ATEMPO2_H__

#include "avfilter/avfilter_atempo.h"

__BEGIN_DECLS__

/**
 * @brief  open atempo2 avfilter(down to 16k for freq reduce)
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] atempop
 * @return NULL on error
 */
avfilter_t *avf_atempo2_open(const char *inst_name, const atempo_avfp_t *atempop);

#define avf_atempo2_control(f, cmd, arg, arg_size) avf_control(f, cmd, arg, arg_size)

#define avf_atempo2_filter_frame(f, i, o) avf_filter_frame(f, i, o)

#define avf_atempo2_close(f) avf_close(f)

__END_DECLS__

#endif /* __AVFILTER_ATEMPO2_H__ */

