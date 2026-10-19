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
    int (*set_mute)(audio_codec_dev_t *dev, bool mute);
    int (*set_volume)(audio_codec_dev_t *dev, int volume_percent);
    /* Recording/capture volume. Percentage is codec-specific mapped.
     * If a codec has separate analog PGA and digital volume, this function should map to
     * the digital ADC/capture volume first.
     */
    int (*set_record_volume)(audio_codec_dev_t *dev, int volume_percent);

    /* Analog mic PGA gain as percentage (0..100).
     * Driver maps to codec-specific step range.
     */
    int (*set_mic_pga)(audio_codec_dev_t *dev, int pga_percent);
    int (*set_sample_rate)(audio_codec_dev_t *dev, uint32_t sample_rate_hz);
    uint32_t (*calc_mclk_hz)(uint32_t sample_rate_hz);

    audio_codec_type_t type;
    uint8_t i2c_addrs_num;
} audio_codec_driver_t;


#ifdef __cplusplus
}
#endif

#endif
