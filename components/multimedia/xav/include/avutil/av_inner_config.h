/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */
#ifndef __AV_INNER_CONFIG_H__
#define __AV_INNER_CONFIG_H__

// fft config
#ifndef CONFIG_FFTXER_SPEEX
#define CONFIG_FFTXER_SPEEX                            (0)
#endif
#ifndef CONFIG_FFTXER_IPC
#define CONFIG_FFTXER_IPC                              (0)
#endif

// debug
#ifndef CONFIG_AV_AEF_DEBUG
#define CONFIG_AV_AEF_DEBUG                            (0)
#endif
#ifndef CONFIG_AV_ERRNO_DEBUG
#define CONFIG_AV_ERRNO_DEBUG                          (0)
#endif

#ifndef CONFIG_AV_MP4_IDX_OPT
#define CONFIG_AV_MP4_IDX_OPT                          (1)
#endif

#ifndef CONFIG_USE_MINIALSA
#define CONFIG_USE_MINIALSA                            (1)
#endif

#ifndef CONFIG_AV_STREAM_INNER_BUF_SIZE
#define CONFIG_AV_STREAM_INNER_BUF_SIZE                (2*1024)
#endif
#ifndef CONFIG_AV_STREAM_RCV_TIMEOUT_DEFAULT
#define CONFIG_AV_STREAM_RCV_TIMEOUT_DEFAULT           (3*1000)     ///< ms
#endif

#ifndef CONFIG_AV_AO_CHANNEL_NUM
#define CONFIG_AV_AO_CHANNEL_NUM                       (1)          ///< for audio output. mono: 1 or stereo: 2 only
#endif
#ifndef CONFIG_AV_AUDIO_ONLY_SUPPORT
#define CONFIG_AV_AUDIO_ONLY_SUPPORT                   (1)
#endif

// player config
#ifndef CONFIG_PLAYER_YOC
#define CONFIG_PLAYER_YOC                              (1)
#endif
#ifndef CONFIG_PLAYER_TPLAYER
#define CONFIG_PLAYER_TPLAYER                          (0)
#endif

// eq config
#ifndef CONFIG_EQXER_SILAN
#define CONFIG_EQXER_SILAN                             (0)
#endif
#ifndef CONFIG_EQXER_IPC
#define CONFIG_EQXER_IPC                               (0)
#endif

// mca config
#ifndef CONFIG_MCAXER_LOCAL
#define CONFIG_MCAXER_LOCAL                            (0)
#endif
#ifndef CONFIG_MCAXER_IPC
#define CONFIG_MCAXER_IPC                              (0)
#endif

#ifndef CONFIG_AO_DIFF_SUPPORT
#define CONFIG_AO_DIFF_SUPPORT                         (0)
#endif

#ifndef CONFIG_AV_DBUS_SUPPORT
#define CONFIG_AV_DBUS_SUPPORT                         (0)
#endif

#ifndef ATEMPO_FREQ_REDUCE
#define ATEMPO_FREQ_REDUCE                             (0)
#endif

#ifndef XIAOYA_PATCH
#define XIAOYA_PATCH                                   (1)
#endif


#if 0
#include "avutil/common.h"

__BEGIN_DECLS__

/**
 * @brief  get number of audio output
 * @return
 */
int av_get_ao_channel_num();

/**
 * @brief  set number of audio output
 * @param  [in] num
 * @return
 */
void av_set_ao_channel_num(int num);

/**
 * @brief  get whether audio diff-output
 * @return
 */
int av_ao_diff_is_enable();

/**
 * @brief  enable/disable audio diff-output
 * @param  [in] enable
 * @return
 */
void av_ao_diff_enable(int enable);

__END_DECLS__
#endif
#endif /* __AV_INNER_CONFIG_H__ */

