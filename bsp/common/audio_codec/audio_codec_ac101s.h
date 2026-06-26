#ifndef _BSP_COMMON_AUDIO_CODEC_AC101S_H_
#define _BSP_COMMON_AUDIO_CODEC_AC101S_H_

#include "audio_codec_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const audio_codec_driver_t audio_codec_ac101s_driver;

int audio_codec_ac101s_dump_registers(struct bflb_device_s *i2c, uint8_t addr);
int audio_codec_ac101s_set_adc_pattern(struct bflb_device_s *i2c, uint8_t addr, uint8_t pattern);
int audio_codec_ac101s_read_register(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *val);
int audio_codec_ac101s_write_register(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
