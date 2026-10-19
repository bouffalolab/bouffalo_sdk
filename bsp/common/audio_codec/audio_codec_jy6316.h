/**
 * @file audio_codec_jy6316.h
 * @brief JY6316 board-level codec driver: JY6311 playback/AEC reference + JY6166 stereo ADC
 */

#ifndef _BSP_COMMON_AUDIO_CODEC_JY6316_H_
#define _BSP_COMMON_AUDIO_CODEC_JY6316_H_

#include "audio_codec_driver.h"
#include "audio_codec_jy6166.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JY6316_JY6311_I2C_ADDR (0x44)
#define JY6316_JY6166_I2C_ADDR JY6166_I2C_ADDR_0

extern const audio_codec_driver_t audio_codec_jy6316_driver;

#ifdef __cplusplus
}
#endif

#endif
