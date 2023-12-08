/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AFORMAT_CONV_H__
#define __AFORMAT_CONV_H__

#include "avutil/common.h"
#include "avutil/sf.h"

__BEGIN_DECLS__

/**
 * @brief dont't do resample here, only audio format convert
 */
typedef struct aformat_conv   afconv_t;

/**
 * @brief  new a audio-converter
 * @param  [in] inf      : original sample format
 * @param  [in] outf     : destination sample format
 * @param  [in] channels : 1/2
 * @return NULL on error
 */
afconv_t *afconv_new(const aformat_t *inf, const aformat_t *outf, uint8_t channels);

/**
 * @brief  convert nb_isamples from src to dst sample format
 * @param  [in] s
 * @param  [in] out
 * @param  [in] nb_osamples : amount of space available for output in samples per channel
 * @param  [in] in
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return number of samples output per channel, -1 on error
 */
int afconv_convert(afconv_t *ac, void **out, size_t nb_osamples, const void **in, size_t nb_isamples);

/**
 * @brief  free the audio-converter
 * @param  [in] s
 * @return
 */
void afconv_free(afconv_t *ac);

#endif /* __AFORMAT_CONV_H__ */

