/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFT_ICORE_INTERNAL_H__
#define __FFT_ICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "tfft/fft_cls.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define FFT_ICORE_AP_IDX          (0)
#define FFT_ICORE_CP_IDX          (1)
#else
#define FFT_ICORE_AP_IDX          (1)
#define FFT_ICORE_CP_IDX          (2)
#endif

#define IPC_CMD_FFT_ICORE         (60)
#define FFT_ICORE_IPC_SERIVCE_ID  (0x15)

enum {
    ICORE_CMD_FFT_INVALID,
    ICORE_CMD_FFT_NEW,
    ICORE_CMD_FFT_FORWARD,
    ICORE_CMD_FFT_BACKWARD,
    ICORE_CMD_FFT_FREE,
};

typedef struct {
    size_t            size;      // N
    void              *fft;      // resp: fftx_t
} fft_icore_new_t;

typedef struct {
    void              *fft;
    int16_t           *in;
    int16_t           *out;
} fft_icore_forward_t;

typedef struct {
    void              *fft;
    int16_t           *in;
    int16_t           *out;
} fft_icore_backward_t;

typedef struct {
    void      *fft;
} fft_icore_free_t;

__END_DECLS__

#endif /* __FFT_ICORE_INTERNAL_H__ */

