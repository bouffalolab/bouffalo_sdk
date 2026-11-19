/* SPDX-License-Identifier: Apache-2.0 */
/* Read-only CLI routing, strict slot parsing and confirmed-value diagnostics. */
#include "audio_pcm.h"
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned pcm_caps_calls, pcm_state_calls, legacy_caps_calls, legacy_state_calls;
static int pcm_caps_result, pcm_state_result;
static uint8_t valid = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB | AUDIO_CODEC_CONTROL_MUTE;
static char message[256];
static float confirmed_gain_db = 9.25f;

static int get_caps(bool pcm, bool mic, uint8_t slot, audio_codec_channel_caps_t *caps)
{
    assert(slot == (mic ? 3 : 0));
    if (pcm) {
        ++pcm_caps_calls;
        if (pcm_caps_result != 0) {
            return pcm_caps_result;
        }
    } else {
        ++legacy_caps_calls;
    }
    uint8_t controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE;
    if (mic) {
        controls |= AUDIO_CODEC_CONTROL_GAIN_DB;
    }
    *caps = (audio_codec_channel_caps_t){
        .controls = controls,
        .independent_controls = controls,
        .linked_mask = 0,
    };
    return 0;
}

static int get_state(bool pcm, bool mic, uint8_t slot, audio_codec_channel_state_t *state)
{
    assert(slot == (mic ? 3 : 0));
    if (pcm) {
        ++pcm_state_calls;
        if (pcm_state_result != 0) {
            return pcm_state_result;
        }
    } else {
        ++legacy_state_calls;
    }
    *state = (audio_codec_channel_state_t){
        .gain_db = confirmed_gain_db,
        .volume_percent = 69,
        .valid = mic ? valid : valid & ~AUDIO_CODEC_CONTROL_GAIN_DB,
        .muted = true,
    };
    return 0;
}

#define CAPS_GETTER(prefix, role, is_pcm, is_mic) \
    int prefix##_get_##role##_channel_caps(uint8_t slot, audio_codec_channel_caps_t *caps) \
    { \
        return get_caps(is_pcm, is_mic, slot, caps); \
    }
#define STATE_GETTER(prefix, role, is_pcm, is_mic) \
    int prefix##_get_##role##_channel_state(uint8_t slot, audio_codec_channel_state_t *state) \
    { \
        return get_state(is_pcm, is_mic, slot, state); \
    }
#if defined(CONFIG_BSP_AUDIO_PCM)
CAPS_GETTER(audio_pcm, mic, true, true)
CAPS_GETTER(audio_pcm, speaker, true, false)
STATE_GETTER(audio_pcm, mic, true, true)
STATE_GETTER(audio_pcm, speaker, true, false)
#else
CAPS_GETTER(wave_player, mic, false, true)
CAPS_GETTER(wave_player, speaker, false, false)
STATE_GETTER(wave_player, mic, false, true)
STATE_GETTER(wave_player, speaker, false, false)
#endif

static int capture_output(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);
    assert(result >= 0 && (size_t)result < sizeof(message));
    return result;
}

#define printf capture_output
#include "audio_channel_cli_under_test.h"
#undef printf

static void test_gain_parser(void)
{
    const struct {
        const char *text;
        float value;
    } values[] = { { "+9.25", 9.25f }, { "-9.25", -9.25f }, { ".5", 0.5f },
                   { "-.5", -0.5f }, { "+0.5", 0.5f }, { "0", 0.0f },
                   { "1e1", 10.0f }, { "1E+1", 10.0f }, { "0x1p3", 8.0f }, { " 1", 1.0f } };
    for (unsigned i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        float parsed = 77.0f;
        assert(parse_gain_db(values[i].text, &parsed) == 0 && parsed == values[i].value);
    }
    const char *invalid[] = { "", ".", "+.", "-", "--1", "1..2", "1.2tail", "1 ", "nan", "inf",
                              "-inf", "NAN(1)", "1e1000", "1e-1000",
                              "9999999999999999999999999999999999999999999999999999999999999999" };
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        float parsed = 77.0f;
        assert(parse_gain_db(invalid[i], &parsed) == -EINVAL && parsed == 77.0f);
    }
}

int main(void)
{
    test_gain_parser();
    char *query[] = { "audio_channel", "speaker", "0", "extra" };
    assert(cmd_audio_channel(1, query) == -EINVAL && cmd_audio_channel(4, query) == -EINVAL);
    query[1] = "MIC";
    assert(cmd_audio_channel(3, query) == -EINVAL);
    query[1] = "speaker";
    const char *invalid_slots[] = { "", "-1", "16", "65536", "0x1", "1tail", "1.0", " 1", "42949672960000000000" };
    for (unsigned i = 0; i < sizeof(invalid_slots) / sizeof(invalid_slots[0]); ++i) {
        query[2] = (char *)invalid_slots[i];
        assert(cmd_audio_channel(3, query) == -EINVAL);
    }
    assert(pcm_caps_calls + pcm_state_calls + legacy_caps_calls + legacy_state_calls == 0);
    query[2] = "0";
    assert(cmd_audio_channel(3, query) == 0);
    assert(strcmp(message, "audio_channel role=speaker slot=0 controls=5 independent=5 linked=0 valid=5 "
                           "volume=69 gain_db=- mute=1\r\n") == 0);
#if defined(CONFIG_BSP_AUDIO_PCM)
    assert(pcm_caps_calls == 1 && pcm_state_calls == 1 && legacy_caps_calls == 0 && legacy_state_calls == 0);
    const int failures[] = { -ENODEV, -EBUSY, -EIO, -ENOTSUP, -EINVAL };
    for (unsigned i = 0; i < sizeof(failures) / sizeof(failures[0]); ++i) {
        pcm_caps_result = failures[i];
        assert(cmd_audio_channel(3, query) == failures[i]);
        assert(legacy_caps_calls == 0 && legacy_state_calls == 0);
    }
    pcm_caps_result = 0;
    pcm_state_result = -EBUSY;
    assert(cmd_audio_channel(3, query) == -EBUSY && legacy_state_calls == 0);
    pcm_state_result = -ENODEV;
    assert(cmd_audio_channel(3, query) == -ENODEV && legacy_caps_calls == 0 && legacy_state_calls == 0);
    pcm_caps_result = pcm_state_result = 0;
#else
    assert(pcm_caps_calls == 0 && pcm_state_calls == 0 && legacy_caps_calls == 1 && legacy_state_calls == 1);
#endif
    query[1] = "mic";
    query[2] = "3";
    confirmed_gain_db = -9.25f;
    assert(cmd_audio_channel(3, query) == 0 && strstr(message, "gain_db=-9.25") != NULL);
    confirmed_gain_db = 99.99f;
    assert(cmd_audio_channel(3, query) == 0 && strstr(message, "gain_db=99.99") != NULL);
    confirmed_gain_db = -99.99f;
    assert(cmd_audio_channel(3, query) == 0 && strstr(message, "gain_db=-99.99") != NULL);
    confirmed_gain_db = -0.0f;
    assert(cmd_audio_channel(3, query) == 0 && strstr(message, "gain_db=0.00") != NULL);
    valid = 0;
    confirmed_gain_db = NAN; /* Unknown data must never reach integer formatting. */
    assert(cmd_audio_channel(3, query) == 0);
    assert(strstr(message, "role=mic slot=3") != NULL);
    assert(strstr(message, "valid=0 volume=- gain_db=- mute=-") != NULL);
    puts("audio_channel: strict arguments, compile-time backend selection without fallback, read-only confirmed state passed");
    return 0;
}
