/*
 * Audio Output Interface
 */

#ifndef WAVE_PLAYER_H
#define WAVE_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#include "audio_codec.h"

#ifndef CONFIG_WAVE_PLAYER_OUT_SAMPLES
#define WAVE_PLAYER_OUT_SAMPLES 512
#else
#define WAVE_PLAYER_OUT_SAMPLES CONFIG_WAVE_PLAYER_OUT_SAMPLES
#endif
#define AUDIO_OUT_SAMPLES WAVE_PLAYER_OUT_SAMPLES

#ifndef CONFIG_WAVE_PLAYER_OUT_CHANNELS
#define WAVE_PLAYER_OUT_CHANNELS 2
#else
#define WAVE_PLAYER_OUT_CHANNELS CONFIG_WAVE_PLAYER_OUT_CHANNELS
#endif
#define AUDIO_OUT_CHANNELS WAVE_PLAYER_OUT_CHANNELS

#ifndef CONFIG_WAVE_PLAYER_ENABLE_REC
#define WAVE_PLAYER_ENABLE_REC 0
#else
#define WAVE_PLAYER_ENABLE_REC CONFIG_WAVE_PLAYER_ENABLE_REC
#endif

#ifndef CONFIG_I2S_DSP_MODE
#define WAVE_PLAYER_I2S_DSP_MODE 0
#else
#define WAVE_PLAYER_I2S_DSP_MODE CONFIG_I2S_DSP_MODE
#endif

/* Zero derives bus slots from active I2S channels; fixed-frame codecs may
 * need an explicit slot count even when recording is disabled. */
#ifndef CONFIG_WAVE_PLAYER_I2S_TX_SLOTS
#define WAVE_PLAYER_I2S_TX_SLOTS 0U
#else
#define WAVE_PLAYER_I2S_TX_SLOTS CONFIG_WAVE_PLAYER_I2S_TX_SLOTS
#endif
#if WAVE_PLAYER_I2S_DSP_MODE && WAVE_PLAYER_I2S_TX_SLOTS != 0U && \
    WAVE_PLAYER_I2S_TX_SLOTS != 1U && WAVE_PLAYER_I2S_TX_SLOTS != 2U && \
    WAVE_PLAYER_I2S_TX_SLOTS != 3U && WAVE_PLAYER_I2S_TX_SLOTS != 4U && \
    WAVE_PLAYER_I2S_TX_SLOTS != 6U
#error "WAVE_PLAYER_I2S_TX_SLOTS must be 0, 1, 2, 3, 4 or 6"
#endif

#ifndef CONFIG_WAVE_PLAYER_I2S_DSP_LR_MERGE
/* Disabled Kconfig booleans are emitted as #undef. */
#define WAVE_PLAYER_I2S_DSP_LR_MERGE 0
#else
#define WAVE_PLAYER_I2S_DSP_LR_MERGE CONFIG_WAVE_PLAYER_I2S_DSP_LR_MERGE
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

#ifndef CONFIG_WAVE_PLAYER_TASK_STACK_DEPTH
#define WAVE_PLAYER_TASK_STACK_DEPTH 256U
#else
#define WAVE_PLAYER_TASK_STACK_DEPTH CONFIG_WAVE_PLAYER_TASK_STACK_DEPTH
#endif

#ifndef CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_PGA
#define WAVE_PLAYER_REC_DEFAULT_MIC_PGA 90U
#else
#define WAVE_PLAYER_REC_DEFAULT_MIC_PGA CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_PGA
#endif

#if WAVE_PLAYER_ENABLE_REC
#ifndef CONFIG_WAVE_PLAYER_ENABLE_REC_RB
/* Disabled Kconfig booleans are emitted as #undef. */
#define WAVE_PLAYER_ENABLE_REC_RB 0
#else
#define WAVE_PLAYER_ENABLE_REC_RB CONFIG_WAVE_PLAYER_ENABLE_REC_RB
#endif

#ifndef CONFIG_WAVE_PLAYER_REC_FRAME_MS
#define WAVE_PLAYER_REC_FRAME_MS 20U
#else
#define WAVE_PLAYER_REC_FRAME_MS CONFIG_WAVE_PLAYER_REC_FRAME_MS
#endif
#ifndef CONFIG_WAVE_PLAYER_REC_MAX_FS_HZ
#define WAVE_PLAYER_REC_MAX_FS_HZ 32000U
#else
#define WAVE_PLAYER_REC_MAX_FS_HZ CONFIG_WAVE_PLAYER_REC_MAX_FS_HZ
#endif
#ifndef CONFIG_WAVE_PLAYER_REC_CHANNELS
#define WAVE_PLAYER_REC_CHANNELS 2U
#else
#define WAVE_PLAYER_REC_CHANNELS CONFIG_WAVE_PLAYER_REC_CHANNELS
#endif
#ifndef CONFIG_WAVE_PLAYER_REC_SECONDS
#define WAVE_PLAYER_REC_SECONDS 2U
#else
#define WAVE_PLAYER_REC_SECONDS CONFIG_WAVE_PLAYER_REC_SECONDS
#endif

#ifndef CONFIG_WAVE_PLAYER_REC_DMA_BLOCKS
#define WAVE_PLAYER_CAP_DMA_BLOCKS 8U
#else
#define WAVE_PLAYER_CAP_DMA_BLOCKS CONFIG_WAVE_PLAYER_REC_DMA_BLOCKS
#endif

#endif

typedef struct {
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
} audio_config_t;

typedef uint8_t wave_player_backend_t;
#define WAVE_PLAYER_BACKEND_AUTO      ((wave_player_backend_t)0)
#define WAVE_PLAYER_BACKEND_I2S_CODEC ((wave_player_backend_t)1)
#define WAVE_PLAYER_BACKEND_BL_CODEC  ((wave_player_backend_t)2)

#ifndef CONFIG_WAVE_PLAYER_PLAYBACK_BACKEND
#define WAVE_PLAYER_PLAYBACK_BACKEND_DEFAULT WAVE_PLAYER_BACKEND_AUTO
#else
#define WAVE_PLAYER_PLAYBACK_BACKEND_DEFAULT CONFIG_WAVE_PLAYER_PLAYBACK_BACKEND
#endif

#ifndef CONFIG_WAVE_PLAYER_CAPTURE_BACKEND
#define WAVE_PLAYER_CAPTURE_BACKEND_DEFAULT WAVE_PLAYER_BACKEND_AUTO
#else
#define WAVE_PLAYER_CAPTURE_BACKEND_DEFAULT CONFIG_WAVE_PLAYER_CAPTURE_BACKEND
#endif

/* Physical roles in interleaved DMA slot order, configured by the board's
 * audio integration. Bit N denotes slot N, not an analog ADC pin number.
 * Masks must be disjoint and fit the capture channel count. Unmarked slots
 * have unspecified roles (e.g. empty or auxiliary data); zero means unknown.
 * These describe available inputs, not application selection/mixing policy. */
typedef struct {
    uint32_t mic_mask;
    uint32_t ref_mask;
} wave_player_capture_layout_t;

typedef struct {
    const char *i2s_dev_name;
    const char *i2c_dev_name;
    const char *dma_tx_dev_name;
    const char *dma_rx_dev_name;
    const char *audac_dev_name;
    const char *auadc_dev_name;
    wave_player_backend_t playback_backend;
    wave_player_backend_t capture_backend;
    /* AUDIO_CODEC_TYPE_UNKNOWN accepts the first codec found by the scanner. */
    audio_codec_type_t required_ext_codec_type;
    /* Zero preserves the default priorities (playback=3, capture=2).
     * Streaming applications can run DMA consumers above their DSP/network. */
    uint8_t playback_task_priority;
    uint8_t capture_task_priority;
    wave_player_capture_layout_t capture_layout;
} wave_player_hw_cfg_t;

/* Initialize once from a single owner before calling other player APIs.
 * A successful init is retained across stop/start and recording sessions;
 * another init returns -1 without changing that instance. deinit is not
 * implemented. On failure, no RTOS objects/tasks are retained and init may be
 * retried; board/codec writes are reapplied, not undone. No DMA is started by
 * init. Do not call start/recording APIs unless init returned 0. */
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
int wave_player_set_record_volume(uint8_t volume);
int wave_player_set_record_pga(uint8_t pga);
int wave_player_feed(uint8_t *data, uint32_t len);
uint32_t wave_player_feed_blocking(uint8_t *data, uint32_t len);
void wave_player_config(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels);
void wave_player_dump_stack(void);

/* Called from the player task with interleaved PCM scheduled for DMA, after
 * resampling and silence padding, before channel/slot packing. The buffer is
 * borrowed for the duration of the call. Keep the callback nonblocking. Set or
 * clear it only while playback is stopped. Useful as a software AEC reference.
 * Hardware loopback capture remains preferable where the board provides it. */
typedef void (*wave_player_playback_frame_cb_t)(const int16_t *samples, uint32_t samples_per_channel,
                                               uint32_t sample_rate_hz, uint8_t channels, void *user_data);
int wave_player_set_playback_callback(wave_player_playback_frame_cb_t cb, void *user_data);
/* Discard queued source PCM. DMA already scheduled may play for two blocks. */
void wave_player_flush(void);

#if WAVE_PLAYER_ENABLE_REC
/* Complete interleaved signed 16-bit PCM in DMA slot order:
 * samples[i * channels + c] belongs to slot c of sample i. No selection,
 * mixing or reference extraction is applied. Layout comes from hw_cfg.
 * Called from the capture task, not the DMA ISR. The frame and PCM buffer
 * are borrowed until return; copy both before deferring work to another task.
 * Keep the callback nonblocking; register/clear it while recording is stopped. */
typedef struct {
    const int16_t *samples;
    uint32_t samples_per_channel;
    uint32_t sample_rate_hz;
    uint8_t channels;
    wave_player_capture_layout_t layout;
} wave_player_rec_frame_t;
typedef void (*wave_player_rec_cb_t)(const wave_player_rec_frame_t *frame, void *user_data);

int wave_player_rec_start(uint32_t duration_ms, uint32_t sample_rate_hz, uint8_t channels);
int wave_player_rec_stop(void);
/* Optional legacy mono ring: averages hw_cfg.capture_layout.mic_mask only.
 * A nonzero mic_mask is required to store mono samples; an unknown layout
 * leaves this ring empty and warns at init. The raw callback is unaffected. */
int wave_player_rec_get_mono(int16_t *dst, uint32_t max_samples, uint32_t *out_samples);
int wave_player_rec_set_callback(wave_player_rec_cb_t cb, void *user_data);
int wave_record_enable(bool enable);
bool wave_record_is_enabled(void);

#endif

#endif
