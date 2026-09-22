#ifndef _BSP_COMMON_AUDIO_CODEC_ACM8852_H_
#define _BSP_COMMON_AUDIO_CODEC_ACM8852_H_

#include "audio_codec_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const audio_codec_driver_t audio_codec_acm8852_driver;

int audio_codec_acm8852_read_register(struct bflb_device_s *i2c, uint8_t addr, uint8_t page, uint8_t reg, uint8_t *val);
int audio_codec_acm8852_write_register(struct bflb_device_s *i2c, uint8_t addr, uint8_t page, uint8_t reg, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
