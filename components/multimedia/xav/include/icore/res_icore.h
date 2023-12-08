/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __RES_ICORE_H__
#define __RES_ICORE_H__

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>
#include "swresample/resample.h"

__BEGIN_DECLS__

typedef struct res_icore res_icore_t;

/**
 * @brief  init the icore resample
 * @return 0/-1
 */
int res_icore_init();

/**
 * @brief  new a resampler
 * @param  [in] irate    : input sample rate
 * @param  [in] orate    : output sample rate
 * @param  [in] channels : 1/2, mono/stereo, support mono now only
 * @param  [in] bits     : 16 only
 * @return NULL on error
 */
res_icore_t *res_icore_new(uint32_t irate, uint32_t orate, uint8_t channels, uint8_t bits);

/**
 * @brief  get the max out samples number per channel
 * @param  [in] irate
 * @param  [in] orate
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return -1 on error
 */
int res_icore_get_osamples_max(uint32_t irate, uint32_t orate, size_t nb_isamples);

/**
 * @brief  convert nb_samples from src to dst sample format
 * @param  [in] hdl
 * @param  [in] out
 * @param  [in] nb_osamples : amount of space available for output in samples per channel
 * @param  [in] in
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return number of samples output per channel, -1 on error
 */
int res_icore_convert(res_icore_t *hdl, void **out, size_t nb_osamples, const void **in, size_t nb_isamples);

/**
 * @brief  free the resample
 * @param  [in] hdl
 * @return 0/-1
 */
int res_icore_free(res_icore_t *hdl);

__END_DECLS__

#endif /* __RES_ICORE_H__ */

