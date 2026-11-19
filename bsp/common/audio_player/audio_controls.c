/* SPDX-License-Identifier: Apache-2.0 */
/* Shared microphone/speaker controls and their shell commands. */
#include "wave_player.h"
#include "audio_controls.h"
#include <FreeRTOS.h>
#include <task.h>
#include "audio_pcm.h"
#include "shell.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Shared microphone/speaker state; no PCM data or transport ownership lives here. */
static struct {
    audio_codec_dev_t *playback;
    audio_codec_dev_t *capture;
    uint16_t speaker_mask;
    uint16_t playback_mask;
    uint16_t mic_mask;
    volatile uint16_t speaker_muted_mask;
    volatile uint16_t mic_muted_mask;
    audio_codec_channel_state_t speaker_state[AUDIO_CODEC_MAX_SLOTS];
    audio_codec_channel_state_t mic_state[AUDIO_CODEC_MAX_SLOTS];
    uint8_t tx_slot_count;
    uint8_t tx_slot_word_bytes;
} g_controls;

void audio_controls_reset(void)
{
    memset(&g_controls, 0, sizeof(g_controls));
}

void audio_controls_set_tx_layout(uint8_t slots, uint8_t slot_bytes)
{
    taskENTER_CRITICAL();
    g_controls.tx_slot_count = slots;
    g_controls.tx_slot_word_bytes = slot_bytes;
    taskEXIT_CRITICAL();
}

uint16_t audio_controls_get_mic_mask(void)
{
    return g_controls.mic_mask;
}

uint16_t audio_controls_get_speaker_mask(void)
{
    return g_controls.speaker_mask;
}

uint16_t audio_controls_get_speaker_mute_mask(void)
{
    return g_controls.speaker_muted_mask;
}

bool audio_controls_playback_is_muted(void)
{
    return g_controls.playback_mask != 0U &&
           (g_controls.speaker_muted_mask & g_controls.playback_mask) == g_controls.playback_mask;
}

/* Speaker and microphone controls operate on the existing engine. */
static int wave_player_control_mask(const audio_codec_dev_t *codec, uint16_t available, uint16_t mask)
{
    if (codec == NULL || codec->drv == NULL) {
        return -ENODEV;
    }
    if (available == 0U) {
        return -ENOTSUP;
    }
    return mask != 0U && (mask & ~available) == 0U ? 0 : -EINVAL;
}

static int wave_player_native_gain(const audio_codec_channel_caps_t *caps, float requested, float *applied)
{
    if (!isfinite(requested)) {
        return -EINVAL;
    }
    const float *steps = caps->gain_steps_db;
    if (!(caps->controls & AUDIO_CODEC_CONTROL_GAIN_DB) || steps == NULL || caps->gain_step_count == 0U) {
        return -ENOTSUP;
    }
    if (requested < steps[0] || requested > steps[caps->gain_step_count - 1U]) {
        return -ERANGE;
    }
    *applied = steps[0];
    for (uint16_t i = 1; i < caps->gain_step_count; ++i) {
        if (fabsf(requested - steps[i]) < fabsf(requested - *applied)) {
            *applied = steps[i];
        }
    }
    return 0;
}

static int wave_player_prepare_mic_gain(uint16_t mask, float requested, float applied[AUDIO_CODEC_MAX_SLOTS])
{
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        if (mask & (1U << slot)) {
            audio_codec_channel_caps_t caps;
            int result = wave_player_get_mic_channel_caps(slot, &caps);
            if (result == 0) {
                result = wave_player_native_gain(&caps, requested, &applied[slot]);
            }
            if (result != 0) {
                return result;
            }
        }
    }
    return 0;
}

static int wave_player_control_result(audio_codec_channel_state_t *state, uint16_t mask, uint8_t control, int result)
{
    taskENTER_CRITICAL();
    if (result != 0 && result != -2) {
        for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
            if (mask & (1U << slot)) {
                state[slot].valid &= (uint8_t)~control;
            }
        }
    }
    taskEXIT_CRITICAL();
    return result == 0 ? 0 : (result == -2 ? -ENOTSUP : -EIO);
}

static int wave_player_save_volume(audio_codec_channel_state_t *state, uint16_t mask, uint8_t volume, int result)
{
    result = wave_player_control_result(state, mask, AUDIO_CODEC_CONTROL_VOLUME, result);
    taskENTER_CRITICAL();
    for (uint8_t slot = 0; result == 0 && slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        if (mask & (1U << slot)) {
            state[slot].volume_percent = volume;
            state[slot].valid |= AUDIO_CODEC_CONTROL_VOLUME;
        }
    }
    taskEXIT_CRITICAL();
    return result;
}

static int wave_player_save_mic_gain(uint16_t mask, const float applied[AUDIO_CODEC_MAX_SLOTS], int result)
{
    result = wave_player_control_result(g_controls.mic_state, mask, AUDIO_CODEC_CONTROL_GAIN_DB, result);
    taskENTER_CRITICAL();
    for (uint8_t slot = 0; result == 0 && slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        if (mask & (1U << slot)) {
            g_controls.mic_state[slot].gain_db = applied[slot];
            g_controls.mic_state[slot].valid |= AUDIO_CODEC_CONTROL_GAIN_DB;
        }
    }
    taskEXIT_CRITICAL();
    return result;
}

int wave_player_get_mic_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps)
{
    if (caps == NULL || slot >= AUDIO_CODEC_MAX_SLOTS || !(g_controls.mic_mask & (1U << slot))) {
        return -EINVAL;
    }
    if (g_controls.capture == NULL || g_controls.capture->drv == NULL) {
        return -ENODEV;
    }
    int result = audio_codec_get_mic_channel_caps(g_controls.capture, slot, caps);
    if (result != 0) {
        *caps = (audio_codec_channel_caps_t){ 0 };
        return result == -2 ? -ENOTSUP : -EIO;
    }
    /* Capture consumers implement mute when copying raw DMA samples. */
    caps->controls |= AUDIO_CODEC_CONTROL_MUTE;
    caps->independent_controls |= AUDIO_CODEC_CONTROL_MUTE;
    return 0;
}

int wave_player_get_speaker_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps)
{
    if (caps == NULL || slot >= AUDIO_CODEC_MAX_SLOTS || !(g_controls.speaker_mask & (1U << slot))) {
        return -EINVAL;
    }
    if (g_controls.playback == NULL || g_controls.playback->drv == NULL) {
        return -ENODEV;
    }
    int result = audio_codec_get_speaker_channel_caps(g_controls.playback, slot, caps);
    if (result != 0) {
        *caps = (audio_codec_channel_caps_t){ 0 };
        return result == -2 ? -ENOTSUP : -EIO;
    }
    caps->controls |= AUDIO_CODEC_CONTROL_MUTE;
    uint8_t writable_slots = g_controls.tx_slot_word_bytes == sizeof(int16_t) ? g_controls.tx_slot_count : 0U;
    if ((g_controls.speaker_mask >> writable_slots) == 0U) {
        caps->independent_controls |= AUDIO_CODEC_CONTROL_MUTE;
    } else {
        caps->independent_controls &= ~AUDIO_CODEC_CONTROL_MUTE;
        caps->linked_mask |= g_controls.speaker_mask;
        if ((caps->linked_mask & ~g_controls.speaker_mask) != 0U) {
            caps->controls &= ~AUDIO_CODEC_CONTROL_MUTE;
        }
    }
    return 0;
}

int wave_player_set_mic_channel_volume(uint16_t mask, uint8_t volume)
{
    int result = wave_player_control_mask(g_controls.capture, g_controls.mic_mask, mask);
    if (result != 0 || volume > 100U) {
        return result != 0 ? result : -EINVAL;
    }
    result = audio_codec_set_mic_channel_volume(g_controls.capture, mask, volume);
    result = wave_player_save_volume(g_controls.mic_state, mask, volume, result);
    return result;
}

int wave_player_set_mic_channel_gain_db(uint16_t mask, float gain_db)
{
    float applied[AUDIO_CODEC_MAX_SLOTS];
    int result = wave_player_control_mask(g_controls.capture, g_controls.mic_mask, mask);
    if (result == 0) {
        result = wave_player_prepare_mic_gain(mask, gain_db, applied);
    }
    if (result != 0) {
        return result;
    }
    result = audio_codec_set_mic_channel_gain_db(g_controls.capture, mask, gain_db);
    return wave_player_save_mic_gain(mask, applied, result);
}

int wave_player_set_mic_channel_mute(uint16_t mask, bool mute)
{
    int result = wave_player_control_mask(g_controls.capture, g_controls.mic_mask, mask);
    if (result != 0) {
        return result;
    }
    taskENTER_CRITICAL();
    if (mute) {
        g_controls.mic_muted_mask |= mask;
    } else {
        g_controls.mic_muted_mask &= (uint16_t)~mask;
    }
    taskEXIT_CRITICAL();
    return 0;
}

int wave_player_set_speaker_channel_volume(uint16_t mask, uint8_t volume)
{
    int result = wave_player_control_mask(g_controls.playback, g_controls.speaker_mask, mask);
    if (result != 0 || volume > 100U) {
        return result != 0 ? result : -EINVAL;
    }
    result = audio_codec_set_speaker_channel_volume(g_controls.playback, mask, volume);
    result = wave_player_save_volume(g_controls.speaker_state, mask, volume, result);
    return result;
}

int wave_player_set_speaker_channel_mute(uint16_t mask, bool mute)
{
    int result = wave_player_control_mask(g_controls.playback, g_controls.speaker_mask, mask);
    if (result != 0) {
        return result;
    }
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        if (mask & (1U << slot)) {
            audio_codec_channel_caps_t caps;
            result = wave_player_get_speaker_channel_caps(slot, &caps);
            if (result != 0) {
                return result;
            }
            uint16_t group =
                (caps.independent_controls & AUDIO_CODEC_CONTROL_MUTE) ? (uint16_t)(1U << slot) : caps.linked_mask;
            if (!(caps.controls & AUDIO_CODEC_CONTROL_MUTE) || group == 0U || (group & ~mask) != 0U) {
                return -ENOTSUP;
            }
        }
    }
    taskENTER_CRITICAL();
    if (mute) {
        g_controls.speaker_muted_mask |= mask;
    } else {
        g_controls.speaker_muted_mask &= (uint16_t)~mask;
    }
    taskEXIT_CRITICAL();
    return 0;
}

static int wave_player_channel_state(bool mic, uint8_t slot, audio_codec_channel_state_t *state)
{
    if (state == NULL) {
        return -EINVAL;
    }
    audio_codec_channel_caps_t caps;
    int result =
        mic ? wave_player_get_mic_channel_caps(slot, &caps) : wave_player_get_speaker_channel_caps(slot, &caps);
    if (result != 0) {
        return result;
    }
    taskENTER_CRITICAL();
    *state = mic ? g_controls.mic_state[slot] : g_controls.speaker_state[slot];
    if (caps.controls & AUDIO_CODEC_CONTROL_MUTE) {
        state->valid |= AUDIO_CODEC_CONTROL_MUTE;
    }
    uint16_t muted = mic ? g_controls.mic_muted_mask : g_controls.speaker_muted_mask;
    state->muted = (muted & (1U << slot)) != 0U;
    taskEXIT_CRITICAL();
    return 0;
}

int wave_player_get_mic_channel_state(uint8_t slot, audio_codec_channel_state_t *state)
{
    return wave_player_channel_state(true, slot, state);
}

int wave_player_get_speaker_channel_state(uint8_t slot, audio_codec_channel_state_t *state)
{
    return wave_player_channel_state(false, slot, state);
}

int wave_player_set_mic_volume(uint8_t volume)
{
    return wave_player_set_mic_channel_volume(g_controls.mic_mask, volume);
}

int wave_player_set_mic_gain_db(float gain_db)
{
    return wave_player_set_mic_channel_gain_db(g_controls.mic_mask, gain_db);
}

int wave_player_set_mic_mute(bool mute)
{
    return wave_player_set_mic_channel_mute(g_controls.mic_mask, mute);
}

int wave_player_set_speaker_volume(uint8_t volume)
{
    return wave_player_set_speaker_channel_volume(g_controls.speaker_mask, volume);
}

int wave_player_set_speaker_mute(bool mute)
{
    return wave_player_set_speaker_channel_mute(g_controls.speaker_mask, mute);
}

bool wave_player_get_mic_mute(void)
{
    return g_controls.mic_mask != 0U && g_controls.mic_muted_mask == g_controls.mic_mask;
}

bool wave_player_get_speaker_mute(void)
{
    return g_controls.speaker_mask != 0U && g_controls.speaker_muted_mask == g_controls.speaker_mask;
}

uint16_t wave_player_get_mic_mute_mask(void)
{
    return g_controls.mic_muted_mask;
}

int wave_player_get_speaker_volume(uint8_t *volume)
{
    if (volume == NULL) {
        return -EINVAL;
    }
    taskENTER_CRITICAL();
    bool found = false;
    uint8_t selected = 0;
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        if (!(g_controls.speaker_mask & (1U << slot))) {
            continue;
        }
        const audio_codec_channel_state_t *state = &g_controls.speaker_state[slot];
        if (!(state->valid & AUDIO_CODEC_CONTROL_VOLUME) || (found && selected != state->volume_percent)) {
            taskEXIT_CRITICAL();
            return -ENOTSUP;
        }
        selected = state->volume_percent;
        found = true;
    }
    if (!found) {
        taskEXIT_CRITICAL();
        return -ENODEV;
    }
    *volume = selected;
    taskEXIT_CRITICAL();
    return 0;
}

int audio_controls_init(const audio_controls_config_t *config)
{
    audio_controls_reset();
    if (config == NULL || config->playback == NULL || config->playback->drv == NULL) {
        return -ENODEV;
    }
    uint16_t capture_mask;
    uint16_t playback_mask;
    if (audio_codec_get_channel_masks(config->playback, &capture_mask, &playback_mask) != 0) {
        return -ENODEV;
    }
    uint16_t speaker_mask = playback_mask;
    if (config->speaker_mask != 0U) {
        if ((config->speaker_mask & ~speaker_mask) != 0U) {
            return -EINVAL;
        }
        speaker_mask = config->speaker_mask;
    }
    if (config->mic_mask != 0U) {
        uint16_t unused_playback_mask;
        if (config->capture == NULL || config->capture->drv == NULL) {
            return -ENODEV;
        }
        if (audio_codec_get_channel_masks(config->capture, &capture_mask, &unused_playback_mask) != 0) {
            return -ENODEV;
        }
        if ((config->mic_mask & ~capture_mask) != 0U) {
            return -EINVAL;
        }
    }
    g_controls.playback_mask = playback_mask;
    g_controls.playback = config->playback;
    g_controls.capture = config->capture;
    g_controls.speaker_mask = speaker_mask;
    g_controls.mic_mask = config->mic_mask;
    audio_controls_set_tx_layout(config->tx_slots, config->tx_slot_bytes);
    if (speaker_mask != 0U) {
        int result = audio_codec_set_speaker_channel_mute(config->playback, speaker_mask, false);
        if (result != 0 && result != -2) {
            audio_controls_reset();
            return -EIO;
        }
        result = wave_player_set_speaker_channel_volume(speaker_mask, WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME);
        if (result != 0 && result != -ENOTSUP) {
            audio_controls_reset();
            return result;
        }
    }
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        uint16_t bit = (uint16_t)(1U << slot);
        if (!(g_controls.mic_mask & bit)) {
            continue;
        }
        audio_codec_channel_caps_t caps;
        int result = wave_player_get_mic_channel_caps(slot, &caps);
        if (result != 0) {
            audio_controls_reset();
            return result;
        }
        if (!(caps.controls & AUDIO_CODEC_CONTROL_GAIN_DB) || caps.gain_steps_db == NULL ||
            caps.gain_step_count == 0U) {
            continue;
        }
        float gain = WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB;
        if (gain < caps.gain_steps_db[0]) {
            gain = caps.gain_steps_db[0];
        }
        if (gain > caps.gain_steps_db[caps.gain_step_count - 1U]) {
            gain = caps.gain_steps_db[caps.gain_step_count - 1U];
        }
        result = wave_player_set_mic_channel_gain_db(bit, gain);
        if (result != 0 && result != -ENOTSUP) {
            audio_controls_reset();
            return result;
        }
    }
    return 0;
}

static int parse_unsigned(const char *text, unsigned limit, unsigned *value)
{
    if (text == NULL || text[0] == '\0') {
        return -EINVAL;
    }
    unsigned parsed = 0;
    for (const char *p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return -EINVAL;
        }
        unsigned digit = (unsigned)(*p - '0');
        if (parsed > limit / 10U || (parsed == limit / 10U && digit > limit % 10U)) {
            return -EINVAL;
        }
        parsed = parsed * 10U + digit;
    }
    *value = parsed;
    return 0;
}

/* Keep diagnostics usable when the application's printf omits float support. */
static void format_gain_db(char *buffer, size_t bytes, float gain_db)
{
    int scaled = (int)(gain_db * 100.0f + (gain_db < 0.0f ? -0.5f : 0.5f));
    unsigned magnitude = (unsigned)(scaled < 0 ? -scaled : scaled);
    snprintf(buffer, bytes, "%s%u.%02u", scaled < 0 ? "-" : "", magnitude / 100U, magnitude % 100U);
}

static int parse_gain_db(const char *text, float *gain_db)
{
    if (text == NULL || text[0] == '\0') {
        return -EINVAL;
    }
    char *end = NULL;
    errno = 0;
    float value = strtof(text, &end);
    if (end == text || *end != '\0' || errno == ERANGE || !isfinite(value)) {
        return -EINVAL;
    }
    *gain_db = value;
    return 0;
}

static int cmd_speaker_vol(int argc, char **argv)
{
    unsigned value;
    unsigned mask = 0;
    if (argc < 2 || argc > 3 || parse_unsigned(argv[1], 100U, &value) != 0 ||
        (argc == 3 && (parse_unsigned(argv[2], UINT16_MAX, &mask) != 0 || mask == 0U))) {
        printf("usage: %s <0-100> [nonzero_slot_mask]\r\n", argv[0]);
        if (argc == 1) {
            uint8_t current;
#if defined(CONFIG_BSP_AUDIO_PCM)
            int result = audio_pcm_get_speaker_volume(&current);
#else
            int result = wave_player_get_speaker_volume(&current);
#endif
            if (result == 0) {
                printf("current speaker volume: %u\r\n", current);
            } else {
                printf("current speaker volume: mixed or unknown\r\n");
            }
        }
        return argc == 1 ? 0 : -EINVAL;
    }
#if defined(CONFIG_BSP_AUDIO_PCM)
    int result = argc == 3 ? audio_pcm_set_speaker_channel_volume((uint16_t)mask, (uint8_t)value) :
                             audio_pcm_set_speaker_volume((uint8_t)value);
#else
    int result = argc == 3 ? wave_player_set_speaker_channel_volume((uint16_t)mask, (uint8_t)value) :
                             wave_player_set_speaker_volume((uint8_t)value);
#endif
    if (result == 0) {
        printf("%s set to %u\r\n", argv[0], value);
    } else {
        printf("%s failed: %d\r\n", argv[0], result);
    }
    return result;
}

static int cmd_speaker_mute(int argc, char **argv)
{
    unsigned value;
    unsigned mask = 0;
    if (argc < 2 || argc > 3 || parse_unsigned(argv[1], 1U, &value) != 0 ||
        (argc == 3 && (parse_unsigned(argv[2], UINT16_MAX, &mask) != 0 || mask == 0U))) {
        printf("usage: %s <0|1> [nonzero_slot_mask]\r\n", argv[0]);
        if (argc == 1) {
#if defined(CONFIG_BSP_AUDIO_PCM)
            bool muted = audio_pcm_get_speaker_mute();
#else
            bool muted = wave_player_get_speaker_mute();
#endif
            printf("all speakers muted: %u\r\n", muted ? 1U : 0U);
        }
        return argc == 1 ? 0 : -EINVAL;
    }
#if defined(CONFIG_BSP_AUDIO_PCM)
    int result = argc == 3 ? audio_pcm_set_speaker_channel_mute((uint16_t)mask, value != 0U) :
                             audio_pcm_set_speaker_mute(value != 0U);
#else
    int result = argc == 3 ? wave_player_set_speaker_channel_mute((uint16_t)mask, value != 0U) :
                             wave_player_set_speaker_mute(value != 0U);
#endif
    if (result == 0) {
        printf("%s set to %u\r\n", argv[0], value);
    } else {
        printf("%s failed: %d\r\n", argv[0], result);
    }
    return result;
}

static int cmd_mic_vol(int argc, char **argv)
{
    unsigned value;
    unsigned mask = 0;
    if (argc < 2 || argc > 3 || parse_unsigned(argv[1], 100U, &value) != 0 ||
        (argc == 3 && (parse_unsigned(argv[2], UINT16_MAX, &mask) != 0 || mask == 0U))) {
        printf("usage: %s <0-100> [nonzero_slot_mask]\r\n", argv[0]);
        return argc == 1 ? 0 : -EINVAL;
    }
#if defined(CONFIG_BSP_AUDIO_PCM)
    int result = argc == 3 ? audio_pcm_set_mic_channel_volume((uint16_t)mask, (uint8_t)value) :
                             audio_pcm_set_mic_volume((uint8_t)value);
#else
    int result = argc == 3 ? wave_player_set_mic_channel_volume((uint16_t)mask, (uint8_t)value) :
                             wave_player_set_mic_volume((uint8_t)value);
#endif
    if (result == 0) {
        printf("%s set to %u\r\n", argv[0], value);
    } else {
        printf("%s failed: %d\r\n", argv[0], result);
    }
    return result;
}

static int cmd_mic_gain_db(int argc, char **argv)
{
    float value;
    unsigned mask = 0;
    if (argc < 2 || argc > 3 || parse_gain_db(argv[1], &value) != 0 ||
        (argc == 3 && (parse_unsigned(argv[2], UINT16_MAX, &mask) != 0 || mask == 0U))) {
        printf("usage: %s <dB> [nonzero_slot_mask]\r\n", argv[0]);
        return argc == 1 ? 0 : -EINVAL;
    }
#if defined(CONFIG_BSP_AUDIO_PCM)
    int result =
        argc == 3 ? audio_pcm_set_mic_channel_gain_db((uint16_t)mask, value) : audio_pcm_set_mic_gain_db(value);
#else
    int result =
        argc == 3 ? wave_player_set_mic_channel_gain_db((uint16_t)mask, value) : wave_player_set_mic_gain_db(value);
#endif
    if (result == 0) {
        printf("%s updated; use audio_channel for applied dB\r\n", argv[0]);
    } else {
        printf("%s failed: %d\r\n", argv[0], result);
    }
    return result;
}

static int cmd_mic_mute(int argc, char **argv)
{
    unsigned value;
    unsigned mask = 0;
    if (argc < 2 || argc > 3 || parse_unsigned(argv[1], 1U, &value) != 0 ||
        (argc == 3 && (parse_unsigned(argv[2], UINT16_MAX, &mask) != 0 || mask == 0U))) {
        printf("usage: %s <0|1> [nonzero_slot_mask]\r\n", argv[0]);
        if (argc == 1) {
#if defined(CONFIG_BSP_AUDIO_PCM)
            bool muted = audio_pcm_get_mic_mute();
#else
            bool muted = wave_player_get_mic_mute();
#endif
            printf("all microphones muted: %u\r\n", muted ? 1U : 0U);
        }
        return argc == 1 ? 0 : -EINVAL;
    }
#if defined(CONFIG_BSP_AUDIO_PCM)
    int result =
        argc == 3 ? audio_pcm_set_mic_channel_mute((uint16_t)mask, value != 0U) : audio_pcm_set_mic_mute(value != 0U);
#else
    int result = argc == 3 ? wave_player_set_mic_channel_mute((uint16_t)mask, value != 0U) :
                             wave_player_set_mic_mute(value != 0U);
#endif
    if (result == 0) {
        printf("%s set to %u\r\n", argv[0], value);
    } else {
        printf("%s failed: %d\r\n", argv[0], result);
    }
    return result;
}

static int cmd_audio_channel(int argc, char **argv)
{
    unsigned slot;
    if (argc != 3 || (strcmp(argv[1], "mic") != 0 && strcmp(argv[1], "speaker") != 0) ||
        parse_unsigned(argv[2], AUDIO_CODEC_MAX_SLOTS - 1U, &slot) != 0) {
        printf("usage: audio_channel <mic|speaker> <slot 0-%u>\r\n", AUDIO_CODEC_MAX_SLOTS - 1U);
        return -EINVAL;
    }
    bool capture = strcmp(argv[1], "mic") == 0;
    audio_codec_channel_caps_t caps;
    audio_codec_channel_state_t state;
#if defined(CONFIG_BSP_AUDIO_PCM)
    int result = capture ? audio_pcm_get_mic_channel_caps((uint8_t)slot, &caps) :
                           audio_pcm_get_speaker_channel_caps((uint8_t)slot, &caps);
#else
    int result = capture ? wave_player_get_mic_channel_caps((uint8_t)slot, &caps) :
                           wave_player_get_speaker_channel_caps((uint8_t)slot, &caps);
#endif
    if (result == 0) {
#if defined(CONFIG_BSP_AUDIO_PCM)
        result = capture ? audio_pcm_get_mic_channel_state((uint8_t)slot, &state) :
                           audio_pcm_get_speaker_channel_state((uint8_t)slot, &state);
#else
        result = capture ? wave_player_get_mic_channel_state((uint8_t)slot, &state) :
                           wave_player_get_speaker_channel_state((uint8_t)slot, &state);
#endif
    }
    if (result != 0) {
        printf("audio_channel role=%s slot=%u error=%d\r\n", argv[1], slot, result);
        return result;
    }
    char volume[4] = "-";
    char gain[16] = "-";
    if (state.valid & AUDIO_CODEC_CONTROL_VOLUME) {
        snprintf(volume, sizeof(volume), "%u", (unsigned)state.volume_percent);
    }
    if (state.valid & AUDIO_CODEC_CONTROL_GAIN_DB) {
        format_gain_db(gain, sizeof(gain), state.gain_db);
    }
    const char *mute = (state.valid & AUDIO_CODEC_CONTROL_MUTE) ? (state.muted ? "1" : "0") : "-";
    printf("audio_channel role=%s slot=%u controls=%lu independent=%lu linked=%u valid=%u "
           "volume=%s gain_db=%s mute=%s\r\n",
           argv[1], slot, (unsigned long)caps.controls, (unsigned long)caps.independent_controls,
           (unsigned)caps.linked_mask, (unsigned)state.valid, volume, gain, mute);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_speaker_vol, speaker_vol, "set speaker volume percent [slot_mask]");
SHELL_CMD_EXPORT_ALIAS(cmd_speaker_mute, speaker_mute, "set speaker mute (0/1) [slot_mask]");
SHELL_CMD_EXPORT_ALIAS(cmd_mic_vol, mic_vol, "set microphone digital volume percent [slot_mask]");
SHELL_CMD_EXPORT_ALIAS(cmd_mic_gain_db, mic_gain_db, "set microphone analog gain in dB [slot_mask]");
SHELL_CMD_EXPORT_ALIAS(cmd_mic_mute, mic_mute, "set microphone mute (0/1) [slot_mask]");
SHELL_CMD_EXPORT_ALIAS(cmd_audio_channel, audio_channel, "show channel capabilities and confirmed state");
