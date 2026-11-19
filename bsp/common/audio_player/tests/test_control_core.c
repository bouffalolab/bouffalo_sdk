/* SPDX-License-Identifier: Apache-2.0 */
/* Exercise the actual shared controls module and native codec dispatch. */
#include "wave_player.h"
#include "audio_codec_driver.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static struct {
    audio_codec_dev_t play_codec, cap_codec;
    wave_player_backend_t playback_backend;
    uint8_t tx_slot_count, tx_slot_word_bytes;
} g_audio;
static unsigned calls;
static int write_result;
static uint16_t last_mask;
static float last_value, hardware[AUDIO_CODEC_MAX_SLOTS];
static uint32_t unsupported;
static bool linked;
static uint16_t native_speaker_mask;
static uint16_t native_capture_mask;
static int caps_result;
static unsigned mute_calls;
static unsigned clean_calls;
static const float steps[] = { -12.0f, -6.0f, 0.0f, 6.0f, 12.0f };

static int native_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps, bool speaker)
{
    assert(dev == &g_audio.play_codec || dev == &g_audio.cap_codec);
    if (slot >= AUDIO_CODEC_MAX_SLOTS || (speaker && dev != &g_audio.play_codec) ||
        (!speaker && dev != &g_audio.cap_codec) ||
        !((speaker ? native_speaker_mask : native_capture_mask) & (1U << slot))) {
        return -2;
    }
    if (caps_result != 0) {
        /* A failed backend must not let stale capabilities escape the adapter. */
        *caps = (audio_codec_channel_caps_t){
            .controls = AUDIO_CODEC_CONTROL_MUTE,
            .independent_controls = AUDIO_CODEC_CONTROL_MUTE,
            .linked_mask = UINT16_MAX,
            .gain_steps_db = steps,
            .gain_step_count = sizeof(steps) / sizeof(steps[0]),
        };
        return caps_result;
    }
    *caps = (audio_codec_channel_caps_t){
        .controls = (AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE) & ~unsupported,
        .linked_mask = linked ? 3 : 0,
    };
    if (dev == &g_audio.cap_codec) {
        caps->controls |= AUDIO_CODEC_CONTROL_GAIN_DB & ~unsupported;
        caps->gain_steps_db = steps;
        caps->gain_step_count = sizeof(steps) / sizeof(steps[0]);
    }
    caps->independent_controls = linked ? 0 : caps->controls;
    return 0;
}

static int native_mic_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    return native_caps(dev, slot, caps, false);
}

static int native_speaker_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    return native_caps(dev, slot, caps, true);
}

static int native_mute(audio_codec_dev_t *dev, uint16_t mask, bool mute)
{
    assert(dev == &g_audio.play_codec && mask != 0U && !mute);
    ++mute_calls;
    return 0;
}

static int native_volume(audio_codec_dev_t *dev, uint16_t mask, int value)
{
    assert(dev == &g_audio.play_codec || dev == &g_audio.cap_codec);
    ++calls;
    last_mask = mask;
    last_value = value;
    if (write_result == -2) {
        return -2;
    }
    for (unsigned slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        if (mask & (1U << slot)) {
            hardware[slot] = value;
            if (write_result != 0) {
                return write_result; /* A bus failure can follow a partial hardware update. */
            }
        }
    }
    return 0;
}

static int native_gain(audio_codec_dev_t *dev, uint16_t mask, float value)
{
    assert(dev == &g_audio.cap_codec);
    ++calls;
    last_mask = mask;
    last_value = value;
    return write_result;
}

static const audio_codec_driver_t driver = {
    .get_mic_channel_caps = native_mic_caps,
    .get_speaker_channel_caps = native_speaker_caps,
    .set_mic_channel_volume = native_volume,
    .set_speaker_channel_volume = native_volume,
    .set_mic_channel_gain_db = native_gain,
    .set_speaker_channel_mute = native_mute,
};

#include "audio_codec_gain_under_test.h"

static void bflb_l1c_dcache_clean_range(void *buffer, uint32_t bytes)
{
    assert(buffer != NULL && bytes != 0);
    ++clean_calls;
}

#include "wave_player_pcm_fill_under_test.h"

static void prepare_roles(uint16_t speaker_mask, uint16_t mic_mask)
{
    memset(&g_audio, 0, sizeof(g_audio));
    g_audio.play_codec.drv = g_audio.cap_codec.drv = &driver;
    write_result = 0;
    unsupported = 0;
    linked = false;
    caps_result = 0;
    native_speaker_mask = speaker_mask == UINT16_C(0x8000) ? speaker_mask : 3;
    native_capture_mask = mic_mask;
}

static void reset_roles(uint16_t speaker_mask, uint16_t mic_mask)
{
    prepare_roles(speaker_mask, mic_mask);
    const audio_controls_config_t config = {
        .playback = &g_audio.play_codec, .capture = &g_audio.cap_codec,
        .speaker_mask = speaker_mask, .mic_mask = mic_mask,
        .tx_slots = AUDIO_CODEC_MAX_SLOTS, .tx_slot_bytes = sizeof(int16_t),
    };
    assert(audio_controls_init(&config) == 0);
}

static void assert_controls_reset(void)
{
    assert(audio_controls_get_mic_mask() == 0U);
    assert(audio_controls_get_speaker_mask() == 0U);
    assert(wave_player_get_mic_mute_mask() == 0U);
    assert(audio_controls_get_speaker_mute_mask() == 0U);
    assert(!audio_controls_playback_is_muted());
    assert(!wave_player_get_mic_mute() && !wave_player_get_speaker_mute());
    assert(wave_player_set_mic_channel_mute(1, true) == -ENODEV);
    assert(wave_player_set_speaker_channel_volume(1, 40) == -ENODEV);
    uint8_t volume = 77;
    assert(wave_player_get_speaker_volume(&volume) == -ENODEV && volume == 77);
}

static void initialization_validation_and_retry(void)
{
    reset_roles(1, 12);
    assert(wave_player_set_mic_mute(true) == 0);
    assert(wave_player_set_speaker_mute(true) == 0);
    audio_controls_config_t config = {
        .playback = &g_audio.play_codec,
        .capture = &g_audio.cap_codec,
        .speaker_mask = 1,
        .mic_mask = 12,
        .tx_slots = 2,
        .tx_slot_bytes = sizeof(int16_t),
    };
    unsigned before = calls;
    unsigned mute_before = mute_calls;
    config.speaker_mask = 4;
    assert(audio_controls_init(&config) == -EINVAL);
    assert(calls == before && mute_calls == mute_before);
    assert_controls_reset();
    config.speaker_mask = 1;
    assert(audio_controls_init(&config) == 0);
    assert(audio_controls_get_mic_mask() == 12 && audio_controls_get_speaker_mask() == 1);

    /* These are different devices: playback advertises no capture channels. */
    uint16_t capture_mask;
    uint16_t playback_mask;
    assert(audio_codec_get_channel_masks(config.playback, &capture_mask, &playback_mask) == 0);
    assert(capture_mask == 0U && playback_mask == 3);
    assert(audio_codec_get_channel_masks(config.capture, &capture_mask, &playback_mask) == 0);
    assert(capture_mask == 12 && playback_mask == 0U);

    before = calls;
    mute_before = mute_calls;
    config.mic_mask = 32;
    assert(audio_controls_init(&config) == -EINVAL);
    assert(calls == before && mute_calls == mute_before);
    assert_controls_reset();
    config.mic_mask = 12;
    assert(audio_controls_init(&config) == 0);

    before = calls;
    mute_before = mute_calls;
    config.capture = NULL;
    assert(audio_controls_init(&config) == -ENODEV);
    assert(calls == before && mute_calls == mute_before);
    assert_controls_reset();
    config.capture = &g_audio.cap_codec;
    g_audio.cap_codec.drv = NULL;
    assert(audio_controls_init(&config) == -ENODEV);
    assert(calls == before && mute_calls == mute_before);
    assert_controls_reset();
    const audio_codec_driver_t no_caps_driver = { 0 };
    g_audio.cap_codec.drv = &no_caps_driver;
    assert(audio_controls_init(&config) == -EINVAL);
    assert(calls == before && mute_calls == mute_before);
    assert_controls_reset();
    g_audio.cap_codec.drv = &driver;
    assert(audio_controls_init(&config) == 0);

    write_result = -1;
    assert(audio_controls_init(&config) == -EIO);
    assert_controls_reset();
    write_result = 0;
    assert(audio_controls_init(&config) == 0);

    config.capture = NULL;
    config.mic_mask = 0;
    assert(audio_controls_init(&config) == 0);
    assert(audio_controls_get_mic_mask() == 0U && audio_controls_get_speaker_mask() == 1);
    assert(wave_player_set_speaker_mute(true) == 0 && !audio_controls_playback_is_muted());
    assert(wave_player_set_mic_mute(true) == -ENODEV);
}

static void software_only_controls(void)
{
    prepare_roles(1, 4);
    unsupported = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB | AUDIO_CODEC_CONTROL_MUTE;
    const audio_controls_config_t config = {
        .playback = &g_audio.play_codec,
        .capture = &g_audio.cap_codec,
        .speaker_mask = 1,
        .mic_mask = 4,
        .tx_slots = 2,
        .tx_slot_bytes = sizeof(int16_t),
    };
    unsigned before = calls;
    unsigned mute_before = mute_calls;
    assert(audio_controls_init(&config) == 0);
    assert(calls == before && mute_calls == mute_before);
    audio_codec_channel_caps_t caps;
    assert(wave_player_get_mic_channel_caps(2, &caps) == 0);
    assert(caps.controls == AUDIO_CODEC_CONTROL_MUTE && caps.independent_controls == AUDIO_CODEC_CONTROL_MUTE);
    assert(wave_player_get_speaker_channel_caps(0, &caps) == 0);
    assert(caps.controls == AUDIO_CODEC_CONTROL_MUTE && caps.independent_controls == AUDIO_CODEC_CONTROL_MUTE);
    assert(wave_player_set_mic_channel_volume(4, 20) == -ENOTSUP);
    assert(wave_player_set_speaker_channel_volume(1, 20) == -ENOTSUP);
    assert(wave_player_set_mic_mute(true) == 0 && wave_player_get_mic_mute());
    assert(wave_player_set_speaker_mute(true) == 0 && wave_player_get_speaker_mute());
    assert(calls == before && mute_calls == mute_before);
}

static void failed_caps_do_not_publish_controls(void)
{
    reset_roles(1, 4);
    assert(wave_player_set_speaker_mute(true) == 0);
    const int failures[] = { -2, -1 };
    for (unsigned i = 0; i < sizeof(failures) / sizeof(failures[0]); ++i) {
        caps_result = failures[i];
        int expected = caps_result == -2 ? -ENOTSUP : -EIO;
        audio_codec_channel_caps_t caps;
        memset(&caps, 0xa5, sizeof(caps));
        assert(wave_player_get_mic_channel_caps(2, &caps) == expected);
        assert(caps.controls == 0U && caps.independent_controls == 0U && caps.linked_mask == 0U);
        assert(caps.gain_steps_db == NULL && caps.gain_step_count == 0U);
        memset(&caps, 0xa5, sizeof(caps));
        assert(wave_player_get_speaker_channel_caps(0, &caps) == expected);
        assert(caps.controls == 0U && caps.independent_controls == 0U && caps.linked_mask == 0U);
        assert(caps.gain_steps_db == NULL && caps.gain_step_count == 0U);
        assert(wave_player_set_speaker_mute(false) == expected);
        assert(audio_controls_get_speaker_mute_mask() == 1);
        audio_codec_channel_state_t state = {
            .gain_db = 17.0f,
            .volume_percent = 23,
            .valid = 0xa5,
            .muted = true,
        };
        assert(wave_player_get_mic_channel_state(2, &state) == expected);
        assert(state.gain_db == 17.0f && state.volume_percent == 23 && state.valid == 0xa5 && state.muted);
        assert(wave_player_get_speaker_channel_state(0, &state) == expected);
        assert(state.gain_db == 17.0f && state.volume_percent == 23 && state.valid == 0xa5 && state.muted);
    }
    caps_result = 0;
    assert(wave_player_set_speaker_mute(false) == 0 && !wave_player_get_speaker_mute());
    audio_codec_channel_state_t state;
    assert(wave_player_get_speaker_channel_state(0, &state) == 0 && !state.muted);
}

static void independent_controls(void)
{
    reset_roles(3, 12);
    audio_codec_channel_state_t state;
    assert(wave_player_get_mic_channel_state(2, &state) == 0 && (state.valid & AUDIO_CODEC_CONTROL_MUTE) != 0);
    unsigned before = calls;
    const uint16_t invalid[] = { 0, 1, 5, 16, UINT16_MAX };
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        assert(wave_player_set_mic_channel_volume(invalid[i], 50) == -EINVAL);
    }
    assert(wave_player_set_mic_channel_volume(4, 101) == -EINVAL);
    assert(wave_player_set_mic_channel_gain_db(4, NAN) == -EINVAL);
    assert(wave_player_set_mic_channel_gain_db(4, INFINITY) == -EINVAL);
    assert(wave_player_set_mic_channel_gain_db(4, 13) == -ERANGE && calls == before);
    assert(wave_player_set_mic_channel_volume(12, 31) == 0 && calls == before + 1);
    assert(last_mask == 12 && hardware[2] == 31 && hardware[3] == 31);
    assert(wave_player_set_mic_channel_gain_db(8, 9.0f) == 0 && last_value == 9.0f);
    assert(wave_player_get_mic_channel_state(3, &state) == 0 && state.gain_db == 6.0f);
    before = calls;
    assert(wave_player_set_mic_channel_mute(4, true) == 0 && calls == before);
    assert(wave_player_get_mic_mute_mask() == 4 && !wave_player_get_mic_mute());
    assert(wave_player_get_mic_channel_state(2, &state) == 0 && state.muted);
    assert(wave_player_get_mic_channel_state(3, &state) == 0 && !state.muted);
    write_result = -2;
    assert(wave_player_set_mic_channel_volume(12, 70) == -ENOTSUP);
    assert(wave_player_get_mic_channel_state(2, &state) == 0 && (state.valid & AUDIO_CODEC_CONTROL_VOLUME));
    write_result = -1;
    assert(wave_player_set_mic_channel_volume(12, 80) == -EIO);
    assert(hardware[2] == 80 && hardware[3] == 31);
    assert(wave_player_get_mic_channel_state(2, &state) == 0 && !(state.valid & AUDIO_CODEC_CONTROL_VOLUME));
    assert(wave_player_get_mic_channel_state(3, &state) == 0 && !(state.valid & AUDIO_CODEC_CONTROL_VOLUME));
    assert(state.valid & AUDIO_CODEC_CONTROL_GAIN_DB);
    write_result = 0;
    unsupported = AUDIO_CODEC_CONTROL_VOLUME;
    before = calls;
    assert(wave_player_set_mic_channel_volume(8, 50) == -ENOTSUP && calls == before);
    unsupported = 0;
    assert(wave_player_set_speaker_channel_volume(3, 50) == 0);
    uint8_t volume = 77;
    assert(wave_player_get_speaker_volume(&volume) == 0 && volume == 50);
    assert(wave_player_set_speaker_channel_volume(2, 30) == 0);
    assert(wave_player_get_speaker_volume(&volume) == -ENOTSUP && volume == 50);
}

static void linked_packing_and_initialization(void)
{
    reset_roles(3, 0);
    linked = true;
    unsigned before = calls;
    assert(wave_player_set_speaker_channel_volume(1, 50) == -ENOTSUP && calls == before);
    assert(wave_player_set_speaker_channel_volume(3, 50) == 0 && calls == before + 1 && last_mask == 3);
    write_result = -1;
    assert(wave_player_set_speaker_channel_volume(3, 70) == -EIO);
    audio_codec_channel_state_t state;
    assert(wave_player_get_speaker_channel_state(0, &state) == 0 && !(state.valid & AUDIO_CODEC_CONTROL_VOLUME));
    assert(wave_player_get_speaker_channel_state(1, &state) == 0 && !(state.valid & AUDIO_CODEC_CONTROL_VOLUME));
    write_result = 0;
    audio_controls_set_tx_layout(1, sizeof(int16_t));
    assert(wave_player_set_speaker_channel_mute(1, true) == -ENOTSUP);
    assert(wave_player_set_speaker_channel_mute(3, true) == 0 && wave_player_get_speaker_mute());
    audio_controls_set_tx_layout(2, sizeof(int16_t));
    assert(wave_player_set_speaker_channel_mute(1, false) == 0 && audio_controls_get_speaker_mute_mask() == 2);
    const audio_controls_config_t narrow = {
        .playback = &g_audio.play_codec, .capture = &g_audio.cap_codec,
        .speaker_mask = 1,
    };
    assert(audio_controls_init(&narrow) == 0 && audio_controls_get_speaker_mask() == 1);
    assert(audio_controls_get_speaker_mute_mask() == 0 && !wave_player_get_speaker_mute());
    assert(wave_player_get_speaker_channel_state(1, &state) == -EINVAL);
    audio_codec_channel_caps_t caps;
    assert(wave_player_get_speaker_channel_caps(0, &caps) == 0 && caps.linked_mask == 3);
    assert(!(caps.controls & AUDIO_CODEC_CONTROL_MUTE));
    assert(wave_player_set_speaker_channel_mute(1, true) == -ENOTSUP);
    reset_roles(UINT16_C(0x8000), UINT16_C(0x8000));
    assert(wave_player_set_mic_channel_volume(UINT16_C(0x8000), 20) == 0);
    assert(wave_player_get_mic_channel_state(AUDIO_CODEC_MAX_SLOTS - 1U, &state) == 0 && state.volume_percent == 20);
    assert(wave_player_get_mic_channel_state(AUDIO_CODEC_MAX_SLOTS, &state) == -EINVAL);
}

static void narrow_role_preserves_other_output(void)
{
    reset_roles(1, 0);
    audio_controls_set_tx_layout(2, sizeof(int16_t));
    g_audio.playback_backend = WAVE_PLAYER_BACKEND_I2S_CODEC;
    g_audio.tx_slot_count = 2;
    g_audio.tx_slot_word_bytes = sizeof(int16_t);
    assert(wave_player_set_speaker_mute(true) == 0);
    assert(wave_player_get_speaker_mute() && !audio_controls_playback_is_muted());
    assert(audio_controls_get_speaker_mute_mask() == 1);
    const int16_t source[] = { 11, 27, -33, 49 };
    const int16_t expected[] = { 0, 27, 0, 49 };
    int16_t output[4];
    clean_calls = 0;
    pcm_fill_pcm_block(output, source, sizeof(source), 2, 2, sizeof(output));
    assert(clean_calls == 1 && memcmp(output, expected, sizeof(output)) == 0);
    assert(wave_player_set_speaker_mute(false) == 0 && !wave_player_get_speaker_mute());
    pcm_fill_pcm_block(output, source, sizeof(source), 2, 2, sizeof(output));
    assert(memcmp(output, source, sizeof(output)) == 0);
    reset_roles(3, 0);
    assert(wave_player_set_speaker_mute(true) == 0 && audio_controls_playback_is_muted());
    audio_controls_reset();
    assert(!audio_controls_playback_is_muted());
}

int main(void)
{
    assert(wave_player_set_mic_channel_volume(1, 20) == -ENODEV);
    reset_roles(0, 0);
    assert(wave_player_set_mic_channel_volume(1, 20) == -ENOTSUP);
    independent_controls();
    linked_packing_and_initialization();
    narrow_role_preserves_other_output();
    initialization_validation_and_retry();
    software_only_controls();
    failed_caps_do_not_publish_controls();
    puts("Player controls: native masks, initialization retry, software mute and conservative failure state passed");
    return 0;
}
