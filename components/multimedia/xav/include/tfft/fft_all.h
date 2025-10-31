/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __FFT_ALL_H__
#define __FFT_ALL_H__

#include "avutil/common.h"
#include "avutil/av_config.h"

__BEGIN_DECLS__

#define REGISTER_FFTXER(X, x)                                          \
    {                                                                 \
        extern int fftx_register_##x();                                \
        if (CONFIG_FFTXER_##X)                                         \
            fftx_register_##x();                                       \
    }

/**
 * @brief  regist fft for speex
 * @return 0/-1
 */
int fftx_register_speex();

/**
 * @brief  regist fft for ipc
 * @return 0/-1
 */
int fftx_register_ipc();

/**
 * @brief  regist audio equalizer
 * @return 0/-1
 */
static inline int fftx_register()
{
#if defined(CONFIG_FFTXER_SPEEX)
    REGISTER_FFTXER(SPEEX, speex);
#endif
#if defined(CONFIG_FFTXER_IPC)
    REGISTER_FFTXER(IPC, ipc);
#endif
    return 0;
}

__END_DECLS__

#endif /* __FFT_ALL_H__ */

