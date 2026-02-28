/*
 * Audio Output Interface
 */

#ifndef WAVE_PLAYER_H
#define WAVE_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#ifndef CONFIG_WAVE_PLAYER_OUT_SAMPLES
#define AUDIO_OUT_SAMPLES 512
#else
#define AUDIO_OUT_SAMPLES CONFIG_WAVE_PLAYER_OUT_SAMPLES
#endif

#ifndef CONFIG_WAVE_PLAYER_OUT_CHANNELS
#define AUDIO_OUT_CHANNELS 2
#else
#define AUDIO_OUT_CHANNELS CONFIG_WAVE_PLAYER_OUT_CHANNELS
#endif

#ifndef CONFIG_WAVE_PLAYER_ENABLE_REC
#define WAVE_PLAYER_ENABLE_REC 0
#else
#define WAVE_PLAYER_ENABLE_REC CONFIG_WAVE_PLAYER_ENABLE_REC
#endif

#ifndef CONFIG_WAVE_PLAYER_TEST_WAVE
#define WAVE_PLAYER_TEST_WAVE 0
#else
#define WAVE_PLAYER_TEST_WAVE CONFIG_WAVE_PLAYER_TEST_WAVE
#endif

#ifndef CONFIG_WAVE_PLAYER_FIX_SAMPLE_RATE
#define WAVE_PLAYER_FIX_SAMPLE_RATE 0U
#else
#define WAVE_PLAYER_FIX_SAMPLE_RATE CONFIG_WAVE_PLAYER_FIX_SAMPLE_RATE
#endif

#ifndef CONFIG_WAVE_PLAYER_ENABLE_RESAMPLE
#define WAVE_PLAYER_ENABLE_RESAMPLE 0
#else
#define WAVE_PLAYER_ENABLE_RESAMPLE CONFIG_WAVE_PLAYER_ENABLE_RESAMPLE
#endif

#ifndef CONFIG_WAVE_PLAYER_RESAMPLE_QUALITY
#define WAVE_PLAYER_RESAMPLE_QUALITY 5
#else
#define WAVE_PLAYER_RESAMPLE_QUALITY CONFIG_WAVE_PLAYER_RESAMPLE_QUALITY
#endif

#ifndef CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_PGA
#define WAVE_PLAYER_REC_DEFAULT_MIC_PGA 90U
#else
#define WAVE_PLAYER_REC_DEFAULT_MIC_PGA CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_PGA
#endif

#if WAVE_PLAYER_ENABLE_REC
#ifndef CONFIG_WAVE_PLAYER_ENABLE_REC_RB
#define WAVE_PLAYER_ENABLE_REC_RB 1
#else
#define WAVE_PLAYER_ENABLE_REC_RB CONFIG_WAVE_PLAYER_ENABLE_REC_RB
#endif

#ifndef WAVE_PLAYER_REC_FRAME_MS
#define WAVE_PLAYER_REC_FRAME_MS 20U
#endif
#ifndef WAVE_PLAYER_REC_MAX_FS_HZ
#define WAVE_PLAYER_REC_MAX_FS_HZ 32000U
#endif
#ifndef WAVE_PLAYER_REC_CHANNELS
#define WAVE_PLAYER_REC_CHANNELS 2U
#endif
#ifndef WAVE_PLAYER_REC_SECONDS
#define WAVE_PLAYER_REC_SECONDS 2U
#endif

#ifndef CONFIG_WAVE_PLAYER_REC_DMA_BLOCKS
#define WAVE_PLAYER_CAP_DMA_BLOCKS 8U
#else
#define WAVE_PLAYER_CAP_DMA_BLOCKS CONFIG_WAVE_PLAYER_REC_DMA_BLOCKS
#endif

/* 0: left, 1: right, 2: average(L/R) */
#ifndef CONFIG_WAVE_PLAYER_REC_MONO_SOURCE
#define WAVE_PLAYER_REC_MONO_SOURCE 0U
#else
#define WAVE_PLAYER_REC_MONO_SOURCE CONFIG_WAVE_PLAYER_REC_MONO_SOURCE
#endif
#endif

typedef struct {
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
} audio_config_t;

typedef struct {
    const char *i2s_dev_name;
    const char *i2c_dev_name;
    const char *dma_tx_dev_name;
    const char *dma_rx_dev_name;
} wave_player_hw_cfg_t;

int wave_player_init(const wave_player_hw_cfg_t *hw_cfg);
void wave_player_deinit(void);
int wave_player_start(void);
int wave_player_suspend(void);
int wave_player_resume(void);
int wave_player_stop(void);
int wave_player_write(const int16_t *data, uint32_t samples);
int wave_player_set_volume(uint8_t volume);
uint8_t wave_player_get_volume(void);
int wave_player_set_mute(bool mute);
bool wave_player_get_mute(void);
int wave_player_feed(uint8_t *data, uint32_t len);
uint32_t wave_player_feed_blocking(uint8_t *data, uint32_t len);
void wave_player_config(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels);
void wave_player_dump_stack(void);

#if WAVE_PLAYER_ENABLE_REC
typedef void (*wave_player_rec_mono_frame_cb_t)(const int16_t *samples, uint32_t sample_count, uint32_t sample_rate_hz,
                                                void *user_data);

int wave_player_rec_start(uint32_t duration_ms, uint32_t sample_rate_hz, uint8_t channels);
int wave_player_rec_stop(void);
int wave_player_rec_get_mono(int16_t *dst, uint32_t max_samples, uint32_t *out_samples);
int wave_player_rec_set_callback(wave_player_rec_mono_frame_cb_t cb, void *user_data);
int wave_record_enable(bool enable);
bool wave_record_is_enabled(void);
#endif

#endif
