#ifndef _BSP_COMMON_AUDIO_CODEC_DRIVER_H_
#define _BSP_COMMON_AUDIO_CODEC_DRIVER_H_

#include <stdint.h>

#include "audio_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct audio_codec_driver {
    const char *name;
    const uint8_t *i2c_addrs;
    int (*probe)(struct bflb_device_s *i2c, uint8_t addr);
    int (*init)(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg);
    int (*set_sample_rate)(audio_codec_dev_t *dev, uint32_t sample_rate_hz);
    uint32_t (*calc_mclk_hz)(uint32_t sample_rate_hz);
    audio_codec_type_t type;
    uint8_t i2c_addrs_num;
    int (*set_enable)(audio_codec_dev_t *dev, audio_codec_work_mode_t mode, bool enable);
    int (*set_reg)(audio_codec_dev_t *dev, int reg, int value);
    int (*get_reg)(audio_codec_dev_t *dev, int reg, int *value);
    int (*dump_reg)(audio_codec_dev_t *dev);
    int (*deinit)(audio_codec_dev_t *dev);
    int (*get_mic_channel_caps)(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps);
    int (*get_speaker_channel_caps)(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps);
    int (*set_mic_channel_volume)(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent);
    int (*set_mic_channel_gain_db)(audio_codec_dev_t *dev, uint16_t slot_mask, float gain_db);
    int (*set_mic_channel_mute)(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute);
    int (*set_speaker_channel_volume)(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent);
    int (*set_speaker_channel_mute)(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute);
} audio_codec_driver_t;


#ifdef __cplusplus
}
#endif

#endif
