/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFILTER_SWR_H__
#define __AVFILTER_SWR_H__

#include "avutil/sf.h"
#include "avfilter/avfilter.h"

__BEGIN_DECLS__

typedef struct {
    sf_t                      isf;  ///< input sample format
    sf_t                      osf;  ///< output sample format
} swr_avfp_t;

/**
 * @brief  open swr avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] swrp
 * @return NULL on error
 */
avfilter_t *avf_swr_open(const char *inst_name, const swr_avfp_t *swrp);

#define avf_swr_control(f, cmd, arg, arg_size) avf_control(f, cmd, arg, arg_size)

#define avf_swr_filter_frame(f, i, o) avf_filter_frame(f, i, o)

#define avf_swr_close(f) avf_close(f)

__END_DECLS__

#endif /* __AVFILTER_SWR_H__ */

