/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFT_H__
#define __FFT_H__

#include "tfft/fft_cls.h"

__BEGIN_DECLS__

/**
 * @brief  regist fft ops
 * @param  [in] ops
 * @return 0/-1
 */
int fftx_ops_register(const struct fftx_ops *ops);

/**
 * @brief  alloc a fft
 * @param  [in] size : N
 * @return NULL on error
 */
fftx_t* fftx_new(size_t size);

/**
 * @brief  fft forward
 * @param  [in] fft
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int fftx_forward(fftx_t *fft, const int16_t *in, int16_t *out);

/**
 * @brief  fft backward
 * @param  [in] fft
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int fftx_backward(fftx_t *fft, const int16_t *in, int16_t *out);

/**
 * @brief  free the fft
 * @param  [in] fft
 * @return 0/-1
 */
int fftx_free(fftx_t *fft);

__END_DECLS__

#endif /* __FFT_H__ */

