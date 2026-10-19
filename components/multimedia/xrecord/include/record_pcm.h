/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __RECORD_PCM_H__
#define __RECORD_PCM_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*record_pcm_t)(void *priv, void *data, int size);

int record_pcm_init(int format, int sample_rate, int chn_num,
        int frame_ms, record_pcm_t cb, void *priv);

#ifdef __cplusplus
}
#endif

#endif
