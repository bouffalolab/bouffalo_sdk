/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __RIFF_RW_H__
#define __RIFF_RW_H__

#include "avutil/common.h"
#include "avutil/av_typedef.h"

__BEGIN_DECLS__


#define WAV_FORMAT_PCM         (0x0001U)
#define WAV_FORMAT_ADPCM_MS    (0x0002U)
#define WAV_FORMAT_FLOAT       (0x0003U)
#define WAV_FORMAT_ALAW        (0x0006U)
#define WAV_FORMAT_MULAW       (0x0007U)
#define WAV_FORMAT_WMAV1       (0x0160U)
#define WAV_FORMAT_WMAV2       (0x0161U)
#define WAV_FORMAT_EXTENSIBLE  (0xfffeU)

/**
 * @brief  get codec id by tag val
 * @param  [in] tag
 * @return
 */
avcodec_id_t wav_get_codec_id(uint32_t tag);

__END_DECLS__

#endif /* __RIFF_RW_H__ */

