/* SPDX-License-Identifier: Apache-2.0 */
/* Exercise configured defaults through the actual controls and codec dispatch. */
#include "audio_controls.h"
#include "audio_codec_driver.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static audio_codec_dev_t playback;
static unsigned volume_writes;
static unsigned mute_writes;
static uint16_t hardware_volume_mask;
static uint8_t hardware_volume[2];
static bool hardware_muted[2] = { true, true };

static int native_speaker_caps(audio_codec_dev_t *dev, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    assert(dev == &playback);
    if (slot >= 2U) {
        return -2;
    }
    *caps = (audio_codec_channel_caps_t){
        .controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE,
        .independent_controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE,
    };
    return 0;
}

static int native_speaker_volume(audio_codec_dev_t *dev, uint16_t mask, int volume)
{
    assert(dev == &playback);
    assert(mask != 0U && (mask & ~3U) == 0U);
    assert(volume >= 0 && volume <= 100);
    ++volume_writes;
    hardware_volume_mask = mask;
    for (unsigned slot = 0; slot < 2U; ++slot) {
        if (mask & (1U << slot)) {
            hardware_volume[slot] = (uint8_t)volume;
        }
    }
    return 0;
}

static int native_speaker_mute(audio_codec_dev_t *dev, uint16_t mask, bool muted)
{
    assert(dev == &playback);
    assert(mask != 0U && (mask & ~3U) == 0U);
    ++mute_writes;
    for (unsigned slot = 0; slot < 2U; ++slot) {
        if (mask & (1U << slot)) {
            hardware_muted[slot] = muted;
        }
    }
    return 0;
}

static const audio_codec_driver_t driver = {
    .get_speaker_channel_caps = native_speaker_caps,
    .set_speaker_channel_volume = native_speaker_volume,
    .set_speaker_channel_mute = native_speaker_mute,
};

#include "audio_codec_gain_under_test.h"

static void assert_speaker_volume(uint8_t expected)
{
    uint8_t volume = 255;
    assert(wave_player_get_speaker_volume(&volume) == 0);
    assert(volume == expected);
    for (uint8_t slot = 0; slot < 2U; ++slot) {
        audio_codec_channel_state_t state;
        assert(wave_player_get_speaker_channel_state(slot, &state) == 0);
        assert((state.valid & AUDIO_CODEC_CONTROL_VOLUME) != 0U);
        assert(state.volume_percent == expected);
        assert((state.valid & AUDIO_CODEC_CONTROL_MUTE) != 0U);
        assert(!state.muted && !hardware_muted[slot]);
        assert(hardware_volume[slot] == expected);
    }
    assert(audio_controls_get_speaker_mute_mask() == 0U);
    assert(!wave_player_get_speaker_mute());
}

int main(void)
{
    playback.drv = &driver;
    const audio_controls_config_t config = {
        .playback = &playback,
        .speaker_mask = 3,
        .tx_slots = 2,
        .tx_slot_bytes = sizeof(int16_t),
    };
    assert(audio_controls_init(&config) == 0);
    assert(volume_writes == 1U && hardware_volume_mask == 3U);
    assert(mute_writes == 1U);
    assert_speaker_volume(WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME);

    assert(wave_player_set_speaker_volume(37) == 0);
    assert(volume_writes == 2U && hardware_volume_mask == 3U);
    assert(mute_writes == 1U);
    assert_speaker_volume(37);

    audio_controls_reset();
    printf("Player configured speaker default %u: native writes, state and override passed\n",
           (unsigned)WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME);
    return 0;
}
