#ifndef _BSP_COMMON_AUDIO_CODEC_BL_H_
#define _BSP_COMMON_AUDIO_CODEC_BL_H_

#include "audio_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

int audio_codec_bl_open(audio_codec_dev_t *out_dev, const audio_codec_bl_hw_cfg_t *cfg);

extern const struct audio_codec_driver audio_codec_bl_driver;

#ifdef __cplusplus
}
#endif

#endif
