/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __SWRESAMPLE_H__
#define __SWRESAMPLE_H__

#include "avutil/avframe.h"

__BEGIN_DECLS__

typedef struct swr_context swr_t;

/**
 * @brief  new a swreampler
 * @param  [in] isf : original sample format
 * @param  [in] osf : destination sample format
 * TODO: (dst only support 16bit/!planar now)
 * @return NULL on error
 */
swr_t *swr_new(sf_t isf, sf_t osf);

/**
 * @brief  get the max out samples number per channel
 * @param  [in] s
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return -1 on error
 */
int swr_get_osamples_max(swr_t *s, size_t nb_isamples);

/**
 * @brief  convert nb_isamples from src to dst sample format
 * @param  [in] s
 * @param  [in] out
 * @param  [in] nb_osamples : amount of space available for output in samples per channel
 * @param  [in] in
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return number of samples output per channel, -1 on error
 */
int swr_convert(swr_t *s, void **out, size_t nb_osamples, const void **in, size_t nb_isamples);

/**
 * @brief  convert a audio avframe from src to dst sample format
 * @param  [in] s
 * @param  [in] iframe
 * @param  [in] oframe
 * @return number of samples output per channel, -1 on error
 */
int swr_convert_frame(swr_t *s, const avframe_t *iframe, avframe_t *oframe);

/**
 * @brief  free the swreasampler
 * @param  [in] s
 * @return
 */
void swr_free(swr_t *s);

__END_DECLS__


#endif /* __SWRESAMPLE_H__ */

