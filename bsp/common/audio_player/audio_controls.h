/* SPDX-License-Identifier: Apache-2.0 */
#ifndef BSP_AUDIO_CONTROLS_H
#define BSP_AUDIO_CONTROLS_H

#include "audio_codec.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef CONFIG_WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME
#define WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME CONFIG_WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME
#else
#define WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME 50
#endif

#if WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME < 0 || WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME > 100
#error "CONFIG_WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME must be between 0 and 100"
#endif

/* Control state belongs to this module. The engine retains the codec devices.
 * Initialization/reset require a stopped, single-owner device. PCM adapters
 * serialize controls with their session mutex; direct callers serialize them.
 * Applications in PCM builds use audio_pcm_*; these shared control functions
 * are the adapter's implementation, not a second application frontend. */
typedef struct {
    audio_codec_dev_t *playback;
    audio_codec_dev_t *capture;
    uint16_t mic_mask;
    uint16_t speaker_mask;
    uint8_t tx_slots;
    uint8_t tx_slot_bytes;
} audio_controls_config_t;

int audio_controls_init(const audio_controls_config_t *config);
void audio_controls_reset(void);
void audio_controls_set_tx_layout(uint8_t slots, uint8_t slot_bytes);
uint16_t audio_controls_get_mic_mask(void);
uint16_t audio_controls_get_speaker_mask(void);
uint16_t audio_controls_get_speaker_mute_mask(void);
/* True only when every physical playback output is muted. */
bool audio_controls_playback_is_muted(void);

/* Global controls select all declared channels of that role. Channel masks
 * are nonzero PCM slot masks; reference/unknown slots are rejected. Percent is
 * 0..100; analog gain uses native dB steps. All-mute getters mean every declared
 * channel is muted; use per-channel state to distinguish a partial mute. */
int wave_player_set_mic_volume(uint8_t volume);
int wave_player_set_mic_channel_volume(uint16_t mask, uint8_t volume);
int wave_player_set_mic_gain_db(float gain_db);
int wave_player_set_mic_channel_gain_db(uint16_t mask, float gain_db);
int wave_player_set_mic_mute(bool mute);
int wave_player_set_mic_channel_mute(uint16_t mask, bool mute);
int wave_player_get_mic_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps);
int wave_player_get_mic_channel_state(uint8_t slot, audio_codec_channel_state_t *state);
bool wave_player_get_mic_mute(void);
int wave_player_set_speaker_volume(uint8_t volume);
int wave_player_set_speaker_channel_volume(uint16_t mask, uint8_t volume);
int wave_player_set_speaker_mute(bool mute);
int wave_player_set_speaker_channel_mute(uint16_t mask, bool mute);
int wave_player_get_speaker_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps);
int wave_player_get_speaker_channel_state(uint8_t slot, audio_codec_channel_state_t *state);
bool wave_player_get_speaker_mute(void);
uint16_t wave_player_get_mic_mute_mask(void);
/* Returns -ENOTSUP for mixed or not-yet-configured channel volumes. */
int wave_player_get_speaker_volume(uint8_t *volume);

#endif
