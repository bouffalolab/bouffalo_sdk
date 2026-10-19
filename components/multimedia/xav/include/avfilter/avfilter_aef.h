/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_AEF_H__
#define __AVFILTER_AEF_H__

#include "avutil/sf.h"
#include "avfilter/avfilter.h"

__BEGIN_DECLS__

typedef struct {
    sf_t                      sf;
    uint8_t                   *conf;
    size_t                    conf_size;
    size_t                    nsamples_max;   ///< the max frame len for input on processing
} aef_avfp_t;

/**
 * @brief  open aef avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] aefp
 * @return NULL on error
 */
avfilter_t *avf_aef_open(const char *inst_name, const aef_avfp_t *aefp);

#define avf_aef_control(f, cmd, arg, arg_size) avf_control(f, cmd, arg, arg_size)

#define avf_aef_filter_frame(f, i, o) avf_filter_frame(f, i, o)

#define avf_aef_close(f) avf_close(f)

__END_DECLS__

#endif /* __AVFILTER_AEF_H__ */

