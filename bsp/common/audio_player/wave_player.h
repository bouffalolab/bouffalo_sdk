/*
 * Audio Output Interface
 */

#ifndef WAVE_PLAYER_H
#define WAVE_PLAYER_H

#include <stdint.h>
#include <stdbool.h>

#include "audio_codec.h"
#include "audio_controls.h"

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

/* Zero derives bus slots from active I2S channels and the detected codec's
 * frame requirements. JY6316 uses four slots, including playback-only;
 * incompatible explicit slot counts are rejected during configuration. */
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

#ifndef CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB
#define WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB 39.0f
#else
#define WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB ((float)CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB)
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
    uint16_t speaker_mask; /* Zero derives the codec's real playback outputs. */
} wave_player_hw_cfg_t;

/* Direct playback lifecycle APIs are available only without BSP_AUDIO_PCM.
 * PCM builds use audio_pcm as the sole frontend to the shared engine.
 * Initialize once from a single owner before calling other player APIs.
 * A successful init is retained across stop/start and recording sessions;
 * another init returns -1 without changing that instance. deinit is not
 * implemented. On failure, no RTOS objects/tasks are retained and init may be
 * retried; board/codec writes are reapplied, not undone. No DMA is started by
 * init. Do not call start/recording APIs unless init returned 0. */
#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_init(const wave_player_hw_cfg_t *hw_cfg);
void wave_player_deinit(void);
/* Configure before the first start. After stop, start restores the retained
 * format and current recording-enable setting. Configuration errors propagate
 * without starting a playback task; an unconfigured start returns -EINVAL. */
int wave_player_start(void);
#endif
/* Start one background PCM source after configuring/starting playback. The
 * callback owns arg on success and must release its resources before returning.
 * Exit on a short feed or !is_running; stop waits for the callback to return.
 * The callback may call stop itself. Serialize start with other player controls. */
#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_start_source_task(void (*run)(void *), void *arg);
#endif
/* Snapshot for source cancellation; paused playback remains running. */
bool wave_player_is_running(void);
#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_suspend(void);
int wave_player_resume(void);
int wave_player_stop(void);
int wave_player_write(const int16_t *data, uint32_t samples);
#endif
#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_feed(uint8_t *data, uint32_t len);
#endif
/* Queued source PCM, excluding PCM prefetched by the player/resampler and
 * blocks scheduled for DMA. This is a snapshot, not a latency measurement
 * or a reservation. */
uint32_t wave_player_get_buffered_bytes(void);
/* Discard oldest complete source PCM frames beyond this byte budget. Returns
 * bytes discarded; preserves newer PCM, prefetched PCM and scheduled DMA
 * blocks. This limits queued backlog; trim(0) does not immediately silence
 * output. Use flush to invalidate pending source playback, and mute/stop
 * to control output. */
#if !defined(CONFIG_BSP_AUDIO_PCM)
uint32_t wave_player_trim_buffer(uint32_t max_bytes);
uint32_t wave_player_feed_blocking(uint8_t *data, uint32_t len);
#endif
/* Wait for queued/prefetched PCM and DMA output. Pausing keeps the wait active;
 * stopping cancels it. Call from the source task, not the playback task. */
#if !defined(CONFIG_BSP_AUDIO_PCM)
void wave_player_drain(void);
void wave_player_config(uint32_t samplerate_hz, uint32_t tx_block_bytes, uint32_t rx_block_bytes, uint8_t channels);
#endif
void wave_player_dump_stack(void);

/* Called from the player task with interleaved PCM scheduled for DMA, after
 * resampling and silence padding, before channel/slot packing. The buffer is
 * borrowed for the duration of the call. Keep the callback nonblocking. Set or
 * clear it only while playback is stopped. Useful as a software AEC reference.
 * Hardware loopback capture remains preferable where the board provides it. */
typedef void (*wave_player_playback_frame_cb_t)(const int16_t *samples, uint32_t samples_per_channel,
                                               uint32_t sample_rate_hz, uint8_t channels, void *user_data);
#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_set_playback_callback(wave_player_playback_frame_cb_t cb, void *user_data);
#endif
/* Discard queued source PCM. DMA already scheduled may play for two blocks. */
#if !defined(CONFIG_BSP_AUDIO_PCM)
void wave_player_flush(void);
#endif

#if WAVE_PLAYER_ENABLE_REC
/* Complete interleaved signed 16-bit PCM in DMA slot order:
 * samples[i * channels + c] belongs to slot c of sample i. No selection,
 * mixing or reference extraction is applied. Layout comes from hw_cfg.
 * PCM is raw and read-only: consumers must honor muted_mic_mask when copying
 * to their existing writable output. DMA storage must never be modified.
 * Called from the capture task, not the DMA ISR. The frame and PCM buffer
 * are borrowed until return; copy both before deferring work to another task.
 * Keep the callback nonblocking; do not call player control/stop APIs from it.
 * Register/clear it while recording is stopped. */
typedef struct {
    const int16_t *samples;
    uint32_t samples_per_channel;
    uint32_t sample_rate_hz;
    uint8_t channels;
    wave_player_capture_layout_t layout;
    /* Monotonic DMA completion sequence; gaps expose dropped descriptors. */
    uint32_t sequence;
    /* Changes whenever the shared capture DMA timeline is reconfigured. */
    uint32_t epoch;
    /* Samples elapsed in this DMA epoch, including dropped callback blocks. */
    uint64_t first_sample_index;
    uint16_t muted_mic_mask;
} wave_player_rec_frame_t;
typedef void (*wave_player_rec_cb_t)(const wave_player_rec_frame_t *frame, void *user_data);
#if !defined(CONFIG_BSP_AUDIO_PCM)
/* Optional application hook before legacy capture DMA starts. The format is final;
 * storage preparation may block, but must not call player controls. */
void wave_player_rec_prepare(uint32_t sample_rate_hz, uint8_t channels);
#endif
/* Called once when capture stops, after any in-flight frame callback returns.
 * Runs in the task stopping capture, before that stop completes. It may block
 * to finish storage, but must not call player control/stop APIs. Register/clear
 * it while recording is stopped. Pausing playback does not stop capture. */
typedef void (*wave_player_rec_stop_cb_t)(void *user_data);

#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_rec_start(uint32_t duration_ms, uint32_t sample_rate_hz, uint8_t channels);
int wave_player_rec_stop(void);
#endif
/* Export MIC-mask mono from the optional all-slot recording ring.
 * Slot playback reads the original samples without mixing. */
#if !defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_rec_get_mono(int16_t *dst, uint32_t max_samples, uint32_t *out_samples);
int wave_player_rec_set_callback(wave_player_rec_cb_t cb, void *user_data);
int wave_player_rec_set_stop_callback(wave_player_rec_stop_cb_t cb, void *user_data);
int wave_record_enable(bool enable);
bool wave_record_is_enabled(void);
#endif

#endif

#endif
