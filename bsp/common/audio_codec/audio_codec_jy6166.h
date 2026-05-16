/**
 * @file audio_codec_jy6166.h
 * @brief JY6166 stereo ADC helper driver.
 */

#ifndef _BSP_COMMON_AUDIO_CODEC_JY6166_H_
#define _BSP_COMMON_AUDIO_CODEC_JY6166_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_codec.h"
#include "audio_codec_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JY6166_I2C_ADDR_0 (0x60)
#define JY6166_I2C_ADDR_1 (0x61)
#define JY6166_I2C_ADDR_2 (0x62)
#define JY6166_I2C_ADDR_3 (0x63)
#define JY6166_I2C_ADDR_4 (0x64)
#define JY6166_I2C_ADDR_5 (0x65)
#define JY6166_I2C_ADDR_6 (0x66)
#define JY6166_I2C_ADDR_7 (0x67)

typedef struct {
    uint8_t slot_count; /* 1..8 JY6166 stereo TDM chip slots, each slot group is 2 channels. */
    uint8_t slot_index; /* 1..8, selects the JY6166 stereo slot group. */
    bool multi_speed;
} audio_codec_jy6166_tdm_cfg_t;

int audio_codec_jy6166_probe(struct bflb_device_s *i2c, uint8_t addr);
int audio_codec_jy6166_init_adc(struct bflb_device_s *i2c, uint8_t addr, const audio_codec_cfg_t *cfg,
                                const audio_codec_jy6166_tdm_cfg_t *tdm_cfg, int record_volume_percent,
                                int mic_pga_percent);
int audio_codec_jy6166_set_record_volume(struct bflb_device_s *i2c, uint8_t addr, int volume_percent);
int audio_codec_jy6166_set_mic_pga(struct bflb_device_s *i2c, uint8_t addr, int pga_percent);
int audio_codec_jy6166_set_sample_rate(struct bflb_device_s *i2c, uint8_t addr, uint32_t sample_rate_hz);

extern const audio_codec_driver_t audio_codec_jy6166_driver;

#ifdef __cplusplus
}
#endif

#endif
