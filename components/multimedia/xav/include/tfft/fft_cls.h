/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFT_CLS_H__
#define __FFT_CLS_H__

#include "avutil/common.h"

__BEGIN_DECLS__

typedef struct fftx           fftx_t;
typedef struct fftx_ops       fftx_ops_t;

struct fftx {
    size_t                    size;      // N

    void                      *priv;
    const struct fftx_ops     *ops;
    msp_mutex_t               lock;
};

struct fftx_ops {
    const char                *name;

    int      (*init)          (fftx_t *fft, size_t size);
    int      (*forward)       (fftx_t *fft, const int16_t *in, int16_t *out);
    int      (*backward)      (fftx_t *fft, const int16_t *in, int16_t *out);
    int      (*uninit)        (fftx_t *fft);
};

__END_DECLS__

#endif /* __FFT_CLS_H__ */

