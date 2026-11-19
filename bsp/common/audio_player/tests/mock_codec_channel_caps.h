/* SPDX-License-Identifier: Apache-2.0 */
#include "audio_codec_driver.h"
#include "audio_codec_native_gain_under_test.h"

static const audio_codec_driver_t mock_codec_driver = { 0 };

/* The fixture exposes two independent speaker slots and two microphone slots;
 * capture slot 0 remains an observable reference with no microphone controls. */
static const float mock_mic_gain_steps[] = { -12.0f, -6.0f, 0.0f, 6.0f, 12.0f, 18.0f, 24.0f };

int audio_codec_get_mic_channel_caps(audio_codec_dev_t *codec, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    assert(codec == &g_audio.cap_codec && caps != NULL);
    if (slot != 0 && slot != 2 && slot != 3) {
        return -2;
    }
    memset(caps, 0, sizeof(*caps));
    if (slot != 0) {
        caps->controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB | AUDIO_CODEC_CONTROL_MUTE;
        caps->independent_controls = caps->controls;
        caps->gain_steps_db = mock_mic_gain_steps;
        caps->gain_step_count = sizeof(mock_mic_gain_steps) / sizeof(mock_mic_gain_steps[0]);
    }
    return 0;
}

int audio_codec_get_speaker_channel_caps(audio_codec_dev_t *codec, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    assert(codec == &g_audio.play_codec && caps != NULL);
    if (slot > 1) {
        return -2;
    }
    memset(caps, 0, sizeof(*caps));
    caps->controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE;
    caps->independent_controls = caps->controls;
    return 0;
}

int audio_codec_get_channel_masks(audio_codec_dev_t *codec, uint16_t *capture, uint16_t *playback)
{
    assert(codec == &g_audio.cap_codec || codec == &g_audio.play_codec);
    if (capture != NULL) {
        *capture = codec == &g_audio.cap_codec ? 13 : 0;
    }
    if (playback != NULL) {
        *playback = codec == &g_audio.play_codec ? 3 : 0;
    }
    return 0;
}
