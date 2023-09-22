/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __RESAMPLE_H__
#define __RESAMPLE_H__

#include "swresample/resample_cls.h"

__BEGIN_DECLS__

/**
 * @brief  regist resample ops
 * @param  [in] ops
 * @return 0/-1
 */
int resx_ops_register(const struct resx_ops *ops);

/**
 * @brief  new a resampler
 * @param  [in] irate    : input sample rate
 * @param  [in] orate    : output sample rate
 * @param  [in] channels : 1/2, mono/stereo
 * @param  [in] bits     : 16 only
 * @return NULL on error
 */
resx_t *resx_new(uint32_t irate, uint32_t orate, uint8_t channels, uint8_t bits);

/**
 * @brief  get the max out samples number per channel
 * @param  [in] irate
 * @param  [in] orate
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return -1 on error
 */
int resx_get_osamples_max(uint32_t irate, uint32_t orate, size_t nb_isamples);

/**
 * @brief  convert nb_samples from src to dst sample format
 * @param  [in] r
 * @param  [in] out
 * @param  [in] nb_osamples : amount of space available for output in samples per channel
 * @param  [in] in
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return number of samples output per channel, -1 on error
 */
int resx_convert(resx_t *r, void **out, size_t nb_osamples, const void **in, size_t nb_isamples);

/**
 * @brief  free the resample
 * @param  [in] r
 * @return
 */
void resx_free(resx_t *r);

__END_DECLS__

#endif /* __RESAMPLE_H__ */

