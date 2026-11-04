/* SPDX-License-Identifier: Apache-2.0 */
#ifndef BSP_AUDIO_PCM_H
#define BSP_AUDIO_PCM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "wave_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct audio_pcm_session *audio_pcm_handle_t;

typedef enum {
    AUDIO_PCM_PLAYBACK = 0,
    AUDIO_PCM_CAPTURE = 1,
} audio_pcm_direction_t;

typedef struct {
    uint32_t sample_rate_hz;
    uint32_t period_samples;
    uint8_t channels;
    uint8_t bits_per_sample;
} audio_pcm_config_t;

typedef struct {
    bool playback_supported;
    bool capture_supported;
    uint8_t bits_per_sample;
    uint8_t capture_channels;
    uint8_t playback_channels;
    uint8_t playback_source_channels_max;
    /* Zero allows device-supported rates; nonzero requires this exact rate. */
    uint32_t fixed_sample_rate_hz;
    /* Zero means that no device-wide rate ceiling has been enumerated. */
    uint32_t capture_max_rate_hz;
    uint32_t capture_block_samples;
    uint32_t playback_period_samples_max;
    wave_player_capture_layout_t capture_layout;
} audio_pcm_caps_t;

#define AUDIO_PCM_FRAME_DISCONTINUITY (1U << 0)

typedef struct {
    uint32_t sample_rate_hz;
    uint32_t samples_per_channel;
    uint32_t sequence;
    uint32_t epoch;
    uint32_t flags;
    uint64_t first_sample_index;
    uint64_t pts_us;
    uint8_t channels;
    uint8_t bits_per_sample;
    wave_player_capture_layout_t layout;
} audio_pcm_frame_info_t;

/* PCM and metadata are borrowed together until the callback returns. The
 * callback runs in an audio task and must not block, open or close a session.
 * Deferred consumers must copy both into their own bounded storage. */
typedef void (*audio_pcm_frame_cb_t)(const int16_t *samples, const audio_pcm_frame_info_t *info, void *context);

/* One shared physical device is initialized once. Device names are copied by
 * init and may contain at most 31 bytes excluding the trailing zero. Each direction has one
 * logical owner. Capture and playback may coexist at a compatible physical
 * rate; closing one direction does not close the other direction. */
int audio_pcm_init(const wave_player_hw_cfg_t *hardware);
int audio_pcm_get_caps(audio_pcm_caps_t *caps);
/* Returns -EBUSY while a legacy operation or asynchronous legacy source still
 * owns the shared engine; the caller may retry after that operation finishes. */
int audio_pcm_open(audio_pcm_direction_t direction, const audio_pcm_config_t *config, audio_pcm_handle_t *session);
/* Replacement/unregistration returns -EBUSY while that callback is in flight.
 * Retain its context and retry after it returns. A successful close provides
 * the final callback barrier. */
int audio_pcm_set_capture_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context);
/* Playback references share one sequence/sample clock per engine epoch.
 * Producer changes are discontinuities. Capture-only silence omits stale
 * intervals and may shorten an overlapping frame to its unreported suffix. */
int audio_pcm_set_playback_callback(audio_pcm_handle_t session, audio_pcm_frame_cb_t callback, void *context);

/* Nonblocking byte write: a short nonnegative result is ordinary backpressure.
 * -EAGAIN means that a concurrent control operation holds the device lock.
 * Input must contain complete interleaved signed 16-bit PCM sample frames. */
int audio_pcm_write(audio_pcm_handle_t session, const void *samples, size_t bytes);

/* Abort stops delivery for this logical session. close waits for in-flight
 * callbacks and leaves the handle valid for retry if its deadline expires.
 * A successful close invalidates the handle. A zero timeout never waits. */
int audio_pcm_abort(audio_pcm_handle_t session);
int audio_pcm_close(audio_pcm_handle_t session, uint32_t timeout_ms);

uint32_t audio_pcm_buffered_bytes(audio_pcm_handle_t session);
uint32_t audio_pcm_trim_buffer(audio_pcm_handle_t session, uint32_t max_bytes);
int audio_pcm_flush(audio_pcm_handle_t session);
/* Controls reject an active legacy engine or in-flight legacy operation with
 * -EBUSY, including before the first PCM session is opened. */
int audio_pcm_set_volume(uint8_t volume);
uint8_t audio_pcm_get_volume(void);
int audio_pcm_set_mute(bool mute);
bool audio_pcm_get_mute(void);
int audio_pcm_set_record_volume(uint8_t volume);
int audio_pcm_set_record_pga(uint8_t pga);

#ifdef __cplusplus
}
#endif
#endif
