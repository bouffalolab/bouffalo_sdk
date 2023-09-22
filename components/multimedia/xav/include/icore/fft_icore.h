/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFT_ICORE_H__
#define __FFT_ICORE_H__

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>

__BEGIN_DECLS__

typedef struct fft_icore fft_icore_t;

/**
 * @brief  init the icore inter-core fft
 * @return 0/-1
 */
int fft_icore_init();

/**
 * @brief  alloc a inter-core fft
 * @param  [in] size : N of the fft
 * @return NULL on error
 */
fft_icore_t* fft_icore_new(size_t size);

/**
 * @brief  fft forward
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int fft_icore_forward(fft_icore_t *hdl, const int16_t *in, int16_t *out);

/**
 * @brief  fft backward
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int fft_icore_backward(fft_icore_t *hdl, const int16_t *in, int16_t *out);

/**
 * @brief  free the inter-core fft
 * @param  [in] hdl
 * @return 0/-1
 */
int fft_icore_free(fft_icore_t *hdl);

__END_DECLS__

#endif /* __FFT_ICORE_H__ */

