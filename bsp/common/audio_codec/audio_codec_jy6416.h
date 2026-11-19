/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
/**
 * @file audio_codec_jy6416.h
 * @brief JY6311 DAC/reference and two JY6166 stereo microphone ADCs.
 *
 * Enable CONFIG_BSP_AUDIO_CODEC and CONFIG_AUDIO_CODEC_JY6416_ENABLE.
 * The board supplies a six-slot DSP-A PCM16 bus in SoC master mode;
 * BCLK is 96 * Fs and no external MCLK is needed. RX slots are
 * [DAC_DI reference, unused, ADC0-L, ADC0-R, ADC1-L, ADC1-R].
 * The mono DAC consumes TX slot 0. Its digital reference is not an
 * analog speaker-loopback measurement. Board GPIO/I2S setup is external.
 *
 * Microphone control masks select slots 2..5 (0x3c); each microphone
 * supports independent digital volume and native analog gain in dB.
 * Slot 0 exposes an unprocessed reference with no microphone controls.
 * Speaker volume/mute use mask 0x01. Calls must be serialized with other
 * codec/bus operations; stop PCM transfers before changing sample rate.
 */
#ifndef _BSP_COMMON_AUDIO_CODEC_JY6416_H_
#define _BSP_COMMON_AUDIO_CODEC_JY6416_H_

#include "audio_codec_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JY6416_CODEC_ADDR 0x44
#define JY6416_ADC0_ADDR  0x64
#define JY6416_ADC1_ADDR  0x65

/* Register tokens: 0x000..0x0ff = JY6311, 0x100..0x1ff = ADC0,
 * 0x200..0x2ff = ADC1. Each ADC occupies a stereo TDM group. */
#define JY6416_CODEC_REG(reg) (reg)
#define JY6416_ADC0_REG(reg) (0x100 | (reg))
#define JY6416_ADC1_REG(reg) (0x200 | (reg))

extern const audio_codec_driver_t audio_codec_jy6416_driver;

#ifdef __cplusplus
}
#endif
#endif
