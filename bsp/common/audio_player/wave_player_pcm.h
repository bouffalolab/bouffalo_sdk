/* SPDX-License-Identifier: Apache-2.0 */
#ifndef BSP_WAVE_PLAYER_PCM_H
#define BSP_WAVE_PLAYER_PCM_H

/* Private boundary between the PCM session owner and the wave-player engine. */
#include "audio_pcm.h"


#if defined(CONFIG_BSP_AUDIO_PCM)
int wave_player_init(const wave_player_hw_cfg_t *hw_cfg);

typedef struct {
    bool playback_running;
    bool called_from_audio_task;
    bool stopped;
    bool muted;
    uint32_t capture_sequence;
    uint32_t capture_epoch;
    uint64_t tx_total_samples;
    uint32_t tx_period_samples;
} wave_player_pcm_engine_state_t;

/* Snapshot and callback changes do not block. They may be called inside the
 * owner's critical section to keep a frame reservation atomic. */
void wave_player_pcm_engine_snapshot(wave_player_pcm_engine_state_t *state);
void wave_player_pcm_engine_get_caps(audio_pcm_caps_t *caps);
void wave_player_pcm_engine_attach(
#if WAVE_PLAYER_ENABLE_REC
    wave_player_rec_cb_t capture,
#endif
    wave_player_playback_frame_cb_t playback, bool capture_enabled);
void wave_player_pcm_engine_detach(void);
void wave_player_pcm_engine_capture_enable(bool enabled);
int wave_player_pcm_engine_configure(uint32_t rate, uint32_t period, uint8_t channels);
int wave_player_pcm_engine_start_playback(uint8_t channels);
int wave_player_pcm_engine_resume_tx(void);
int wave_player_pcm_engine_stop(uint32_t timeout_ms);
void wave_player_pcm_engine_stop_io(void);
int wave_player_pcm_engine_write(const void *samples, size_t bytes, uint32_t frame_bytes);
void wave_player_pcm_engine_flush(void);
uint32_t wave_player_pcm_engine_trim(uint32_t max_bytes);

/* The engine calls these owner hooks; session state stays in audio_pcm.c. */
bool audio_pcm_owned(void);
bool audio_pcm_keep_capture(void);
bool audio_pcm_capture_prepared(void);
void audio_pcm_player_cleanup(void);
bool audio_pcm_player_wait_reference(void);
void audio_pcm_player_started(void);
void audio_pcm_player_failed(void);
void audio_pcm_prepare_playback(int16_t *samples, size_t bytes);
#else
static inline bool audio_pcm_owned(void)
{
    return false;
}

static inline bool audio_pcm_keep_capture(void)
{
    return false;
}

static inline bool audio_pcm_capture_prepared(void)
{
    return false;
}

static inline void audio_pcm_player_cleanup(void)
{
}

static inline bool audio_pcm_player_wait_reference(void)
{
    return true;
}

static inline void audio_pcm_player_started(void)
{
}

static inline void audio_pcm_player_failed(void)
{
}

static inline void audio_pcm_prepare_playback(int16_t *samples, size_t bytes)
{
    (void)samples;
    (void)bytes;
}
#endif
#endif
