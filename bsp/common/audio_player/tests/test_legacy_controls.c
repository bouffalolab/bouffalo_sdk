/* SPDX-License-Identifier: Apache-2.0 */
/* Exercise production legacy controls when PCM ownership is disabled. */
#include "wave_player.h"
#include <math.h>
#include "FreeRTOS.h"
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned critical_depth;
#define taskENTER_CRITICAL() (++critical_depth)
#define taskEXIT_CRITICAL()  (--critical_depth)


static struct {
    audio_codec_dev_t play_codec;
    audio_codec_dev_t cap_codec;
    uint8_t tx_slot_count, tx_slot_word_bytes;
    void *dma_ch_rx;
    wave_player_capture_layout_t capture_layout;
    bool io_started;
    bool rec_enable;
    bool rec_stop;
    uint32_t rec_write;
    bool rec_filled;
} g_audio;

static unsigned volume_calls;
static unsigned mute_calls;
static unsigned capture_volume_calls;
static unsigned pga_calls;
static unsigned codec_read_calls;
static uint8_t playback_volume;
static uint8_t capture_volume;
static float capture_gain_db;
static uint8_t capture_volume_slots[AUDIO_CODEC_MAX_SLOTS];
static float capture_gain_slots[AUDIO_CODEC_MAX_SLOTS];
static TickType_t ticks;
static int config_result;
static int start_result;
static int stop_result;
static unsigned config_calls;
static unsigned start_calls;
static unsigned stop_calls;
static unsigned record_calls;
static int volume_error;
static int mute_error;

static int wave_player_config_internal(uint32_t rate, uint32_t tx, uint32_t rx, uint8_t channels)
{
    assert(rate == 16000 && tx == rx && channels == 2);
    assert(tx == WAVE_PLAYER_OUT_SAMPLES * WAVE_PLAYER_OUT_CHANNELS * sizeof(int16_t));
    config_calls++;
    record_calls++;
    if (config_result == 0) {
        g_audio.io_started = true;
    }
    return config_result;
}

int wave_player_start(void)
{
    start_calls++;
    record_calls++;
    if (start_result != 0) {
        g_audio.io_started = false;
    }
    return start_result;
}

int wave_player_stop(void)
{
    stop_calls++;
    record_calls++;
    g_audio.io_started = false;
    return stop_result;
}

static TickType_t xTaskGetTickCount(void)
{
    record_calls++;
    return ticks;
}

static void vTaskDelay(TickType_t delay)
{
    record_calls++;
    ticks += delay;
}

#include "mock_codec_channel_caps.h"

int audio_codec_set_speaker_channel_volume(audio_codec_dev_t *codec, uint16_t mask, int volume)
{
    assert(codec == &g_audio.play_codec && mask != 0 && (mask & ~3U) == 0U);
    volume_calls++;
    if (volume_error == 0) {
        playback_volume = volume;
    }
    return volume_error;
}

int audio_codec_set_speaker_channel_mute(audio_codec_dev_t *codec, uint16_t mask, bool mute)
{
    assert(codec == &g_audio.play_codec && mask != 0 && (mask & ~3U) == 0U);
    mute_calls++;
    if (mute_error == 0) {
        (void)mute;
    }
    return mute_error;
}

int audio_codec_set_mic_channel_volume(audio_codec_dev_t *codec, uint16_t mask, int volume)
{
    assert(codec == &g_audio.cap_codec && mask != 0 && (mask & ~12U) == 0U);
    capture_volume = volume;
    capture_volume_calls++;
    for (unsigned c = 0; c < AUDIO_CODEC_MAX_SLOTS; ++c) {
        if (mask & (1U << c)) {
            capture_volume_slots[c] = volume;
        }
    }
    return 0;
}

int audio_codec_set_mic_channel_gain_db(audio_codec_dev_t *codec, uint16_t mask, float gain_db)
{
    int step = audio_codec_gain_step_index(mock_mic_gain_steps,
                                         sizeof(mock_mic_gain_steps) / sizeof(mock_mic_gain_steps[0]), gain_db);
    assert(step >= 0);
    assert(codec == &g_audio.cap_codec && mask != 0 && (mask & ~12U) == 0U);
    capture_gain_db = mock_mic_gain_steps[step];
    pga_calls++;
    for (unsigned c = 0; c < AUDIO_CODEC_MAX_SLOTS; ++c) {
        if (mask & (1U << c)) {
            capture_gain_slots[c] = mock_mic_gain_steps[step];
        }
    }
    return 0;
}


int audio_codec_set_mic_channel_mute(audio_codec_dev_t *codec, uint16_t mask, bool mute)
{
    assert(codec == &g_audio.cap_codec && mask != 0 && (mask & ~12U) == 0U);
    (void)mute;
    return 0;
}

static uint8_t speaker_volume(void)
{
    uint8_t volume = 0;
    assert(wave_player_get_speaker_volume(&volume) == 0);
    return volume;
}

static int wave_player_codec_cli_read_internal(bool capture, uint32_t reg, uint8_t width, uint32_t *value)
{
    assert(!capture && reg == 0 && width == 8 && value != NULL);
    codec_read_calls++;
    *value = 0x5a;
    return 0;
}

#include "wave_player_controls_under_test.h"
#include "wave_player_rec_start_under_test.h"

#if !defined(CONFIG_BSP_AUDIO_PCM)
static char cli_message[160];

static int capture_cli_message(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    int result = vsnprintf(cli_message, sizeof(cli_message), format, arguments);
    va_end(arguments);
    assert(result >= 0 && (size_t)result < sizeof(cli_message));
    return result;
}

#define printf capture_cli_message
#include "wave_player_cli_controls_under_test.h"
#undef printf

static void test_legacy_cli_controls(void)
{
    char *volume[] = { "speaker_vol", "20" };
    char *mute_on[] = { "speaker_mute", "1" };
    char *mute_off[] = { "speaker_mute", "0" };
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 20);
    volume[1] = "90";
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 90);
    assert(cmd_speaker_mute(2, mute_on) == 0 && wave_player_get_speaker_mute());
    assert(cmd_speaker_mute(2, mute_off) == 0 && !wave_player_get_speaker_mute());
    assert(cmd_speaker_vol(1, volume) == 0 && strstr(cli_message, "90") != NULL);
    char *query[] = { "audio_channel", "speaker", "0" };
    unsigned before_query = volume_calls + mute_calls + capture_volume_calls + pga_calls;
    assert(cmd_audio_channel(3, query) == 0 && strstr(cli_message, "role=speaker slot=0") != NULL);
    assert(strstr(cli_message, "volume=90") != NULL);
    assert(volume_calls + mute_calls + capture_volume_calls + pga_calls == before_query);
    unsigned calls = volume_calls;
    volume_error = -1;
    volume[1] = "20";
    assert(cmd_speaker_vol(2, volume) == -EIO && volume_calls == calls + 1);
    uint8_t uncertain = 77;
    assert(wave_player_get_speaker_volume(&uncertain) == -ENOTSUP && uncertain == 77);
    volume_error = 0;
    volume[1] = "90";
    assert(cmd_speaker_vol(2, volume) == 0 && speaker_volume() == 90);
    const char *invalid_percent[] = { "",      "-1",   "101", "255", "text",
                                      "1tail", "0x10", "1.5", " 1",  "42949672960000000000" };
    for (unsigned i = 0; i < sizeof(invalid_percent) / sizeof(invalid_percent[0]); ++i) {
        volume[1] = (char *)invalid_percent[i];
        calls = volume_calls;
        assert(cmd_speaker_vol(2, volume) == -EINVAL && volume_calls == calls);
#if WAVE_PLAYER_ENABLE_REC
        assert(cmd_mic_vol(2, volume) == -EINVAL);
#endif
    }
    char *surplus[] = { "speaker_vol", "30", "1", "2" };
    assert(cmd_speaker_vol(4, surplus) == -EINVAL);
    assert(cmd_speaker_mute(4, surplus) == -EINVAL);
    const char *invalid_bool[] = { "", "2", "-1", "true", "1tail" };
    for (unsigned i = 0; i < sizeof(invalid_bool) / sizeof(invalid_bool[0]); ++i) {
        mute_on[1] = (char *)invalid_bool[i];
        assert(cmd_speaker_mute(2, mute_on) == -EINVAL);
        assert(cmd_mic_mute(2, mute_on) == -EINVAL);
    }
#if WAVE_PLAYER_ENABLE_REC
    char *mic_volume[] = { "mic_vol", "83" };
    char *mic_gain[] = { "mic_gain_db", "-6.0" };
    const char *gain_values[] = { "+9.25", "-9.25", ".5", "1e1", "0x1p3" };
    const float applied[] = { 12.0f, -12.0f, 0.0f, 12.0f, 6.0f };
    for (unsigned i = 0; i < sizeof(gain_values) / sizeof(gain_values[0]); ++i) {
        mic_gain[1] = (char *)gain_values[i];
        assert(cmd_mic_gain_db(2, mic_gain) == 0);
        assert(capture_gain_slots[2] == applied[i] && capture_gain_slots[3] == applied[i]);
    }
    mic_gain[1] = "-6.0";
    assert(cmd_mic_vol(2, mic_volume) == 0 && capture_volume_slots[2] == 83 && capture_volume_slots[3] == 83);
    assert(cmd_mic_gain_db(2, mic_gain) == 0 && capture_gain_slots[2] == -6.0f && capture_gain_slots[3] == -6.0f);
    char *selected[] = { "mic_vol", "31", "4" };
    assert(cmd_mic_vol(3, selected) == 0 && capture_volume_slots[2] == 31 && capture_volume_slots[3] == 83);
    selected[1] = "10.5";
    selected[2] = "8";
    assert(cmd_mic_gain_db(3, selected) == 0 && capture_gain_slots[2] == -6.0f && capture_gain_slots[3] == 12.0f);
    char *partial[] = { "mic_mute", "1", "4" };
    assert(cmd_mic_mute(3, partial) == 0 && wave_player_get_mic_mute_mask() == 4);
    assert(!wave_player_get_mic_mute());
    partial[1] = "0";
    assert(cmd_mic_mute(3, partial) == 0 && wave_player_get_mic_mute_mask() == 0);
    const char *invalid_masks[] = { "0", "1", "5", "16", "65536", "-1", "0x4", "4tail", "" };
    for (unsigned i = 0; i < sizeof(invalid_masks) / sizeof(invalid_masks[0]); ++i) {
        selected[1] = "6";
        selected[2] = (char *)invalid_masks[i];
        unsigned before = capture_volume_calls + pga_calls;
        assert(cmd_mic_vol(3, selected) == -EINVAL);
        assert(cmd_mic_gain_db(3, selected) == -EINVAL);
        assert(capture_volume_calls + pga_calls == before);
    }
    mic_gain[1] = "25";
    unsigned before_gain = pga_calls;
    assert(cmd_mic_gain_db(2, mic_gain) == -ERANGE && pga_calls == before_gain);
    assert(capture_gain_slots[2] == -6.0f && capture_gain_slots[3] == 12.0f);
    const char *invalid_gain[] = { "nan", "inf", "-inf", "6tail", "1e1000", "1e-1000", "" };
    for (unsigned i = 0; i < sizeof(invalid_gain) / sizeof(invalid_gain[0]); ++i) {
        mic_gain[1] = (char *)invalid_gain[i];
        assert(cmd_mic_gain_db(2, mic_gain) == -EINVAL);
    }
#else
    char *mic_on[] = { "mic_mute", "1" };
    char *mic_volume[] = { "mic_vol", "50" };
    char *mic_gain[] = { "mic_gain_db", "6" };
    assert(cmd_mic_mute(2, mic_on) == -ENOTSUP);
    assert(cmd_mic_vol(2, mic_volume) == -ENOTSUP);
    assert(cmd_mic_gain_db(2, mic_gain) == -ENOTSUP);
#endif
}
#endif

static void test_record_mute(void)
{
#if WAVE_PLAYER_ENABLE_REC
    assert(wave_player_set_mic_mute(false) == 0 && !wave_player_get_mic_mute());
    assert(wave_player_set_mic_channel_mute(4, true) == 0 && wave_player_get_mic_mute_mask() == 4);
    assert(!wave_player_get_mic_mute());
    assert(wave_player_set_mic_mute(true) == 0 && wave_player_get_mic_mute());
    assert(wave_player_set_mic_channel_mute(1, false) == -EINVAL);
    assert(wave_player_set_mic_channel_mute(0, false) == -EINVAL);
    assert(wave_player_set_mic_mute(false) == 0 && wave_player_get_mic_mute_mask() == 0);
#else
    assert(wave_player_set_mic_mute(true) == -ENOTSUP);
#endif
}

int main(void)
{
    g_audio.capture_layout = (wave_player_capture_layout_t){ .mic_mask = 12, .ref_mask = 1 };
    g_audio.play_codec.drv = g_audio.cap_codec.drv = &mock_codec_driver;
    g_audio.tx_slot_count = 2;
    g_audio.tx_slot_word_bytes = sizeof(int16_t);
    const audio_controls_config_t controls = {
        .playback = &g_audio.play_codec, .capture = &g_audio.cap_codec,
        .speaker_mask = 3, .mic_mask = WAVE_PLAYER_ENABLE_REC ? 12 : 0,
        .tx_slots = 2, .tx_slot_bytes = sizeof(int16_t),
    };
    assert(audio_controls_init(&controls) == 0);
    volume_calls = mute_calls = capture_volume_calls = pga_calls = 0;
    uint32_t register_value = 0;
    assert(wave_player_codec_cli_read(false, 0, 8, &register_value) == 0);
    assert(register_value == 0x5a && codec_read_calls == 1);
    assert(wave_player_set_speaker_volume(37) == 0);
    assert(wave_player_set_speaker_mute(true) == 0);
#if WAVE_PLAYER_ENABLE_REC
    assert(wave_player_set_mic_volume(71) == 0);
    assert(wave_player_set_mic_gain_db(12.0f) == 0);
    assert(capture_volume_slots[2] == 71 && capture_volume_slots[3] == 71 &&
           capture_gain_slots[2] == 12.0f && capture_gain_slots[3] == 12.0f);
#else
    assert(wave_player_set_mic_volume(71) == -ENOTSUP);
    assert(wave_player_set_mic_gain_db(12.0f) == -ENOTSUP);
#endif
    assert(speaker_volume() == 37 && wave_player_get_speaker_mute());
    assert(playback_volume == 37);
#if WAVE_PLAYER_ENABLE_REC
    assert(capture_volume == 71 && capture_gain_db == 12.0f);
    assert(capture_volume_calls == 1 && pga_calls == 1);
#else
    assert(capture_volume_calls == 0 && pga_calls == 0);
#endif
    assert(volume_calls == 1 && mute_calls == 0);
    assert(speaker_volume() == 37 && wave_player_get_speaker_mute());
    assert(wave_player_rec_stop() == 0 && !g_audio.rec_stop);
    assert(wave_record_enable(false) == 0 && !g_audio.rec_enable);
    assert(wave_record_enable(true) == 0 && g_audio.rec_enable);
    assert(wave_record_enable(true) == 0 && g_audio.rec_enable);
    assert(wave_record_enable(false) == 0 && !g_audio.rec_enable);
    g_audio.io_started = true;
    g_audio.rec_write = 7;
    g_audio.rec_filled = true;
    g_audio.rec_stop = true;
    for (unsigned enabled = 0; enabled < 2; ++enabled) {
        g_audio.rec_enable = enabled != 0;
        for (unsigned requested = 0; requested < 2; ++requested) {
            assert(wave_record_enable(requested != 0) == -EBUSY);
            assert(g_audio.rec_enable == (enabled != 0));
        }
        assert(wave_player_rec_start(6U, 16000U, 2U) == -EBUSY);
        assert(g_audio.io_started && g_audio.rec_enable == (enabled != 0));
        assert(g_audio.rec_write == 7 && g_audio.rec_filled && g_audio.rec_stop);
        assert(record_calls == 0);
    }
    g_audio.io_started = false;
    g_audio.rec_enable = false;
    assert(wave_player_rec_start(6U, 16000U, 2U) == 0);
    assert(record_calls != 0 && ticks == 6U);
    assert(!g_audio.io_started && !g_audio.rec_enable && !g_audio.rec_stop);
#if WAVE_PLAYER_ENABLE_REC_RB
    assert(g_audio.rec_write == 0 && !g_audio.rec_filled);
#else
    assert(g_audio.rec_write == 7 && g_audio.rec_filled);
#endif
    assert(wave_record_enable(false) == 0 && !g_audio.rec_enable);
    assert(wave_record_enable(true) == 0 && g_audio.rec_enable);
    assert(wave_player_rec_stop() == 0 && g_audio.rec_stop);
    g_audio.rec_stop = false;


    /* Invalid values never reach hardware. */
    assert(wave_player_set_speaker_volume(255) == -EINVAL && playback_volume == 37);
#if WAVE_PLAYER_ENABLE_REC
    assert(wave_player_set_mic_volume(255) == -EINVAL && capture_volume == 71);
    assert(wave_player_set_mic_gain_db(NAN) == -EINVAL && capture_gain_db == 12.0f);
#endif
    assert(wave_player_set_speaker_mute(false) == 0 && !wave_player_get_speaker_mute());
    assert(wave_player_rec_stop() == 0 && g_audio.rec_stop);
    assert(volume_calls == 1 && mute_calls == 0);
#if WAVE_PLAYER_ENABLE_REC
    assert(capture_volume_calls == 1 && pga_calls == 1);
#endif
    assert(wave_player_codec_cli_read(false, 0, 8, &register_value) == 0);
    assert(register_value == 0x5a && codec_read_calls == 2);
    for (unsigned previous = 0; previous < 2; previous++) {
        g_audio.rec_enable = previous != 0;
        g_audio.rec_stop = false;
        config_calls = 0;
        start_calls = 0;
        stop_calls = 0;
        ticks = 0;
        config_result = -EINVAL;
        assert(wave_player_rec_start(100, 16000, 2) == -EINVAL);
        assert(config_calls == 1 && start_calls == 0 && stop_calls == 0 && ticks == 0);
        assert(g_audio.rec_enable == (previous != 0) && !g_audio.rec_stop);
        config_result = 0;
        start_result = -ENOMEM;
        assert(wave_player_rec_start(100, 16000, 2) == -ENOMEM);
        assert(config_calls == 2 && start_calls == 1 && stop_calls == 0 && ticks == 0);
        assert(g_audio.rec_enable == (previous != 0) && !g_audio.rec_stop);
        start_result = 0;
        stop_result = -ETIMEDOUT;
        assert(wave_player_rec_start(100, 16000, 2) == -ETIMEDOUT);
        assert(stop_calls == 1 && ticks >= 100);
        assert(g_audio.rec_enable == (previous != 0) && !g_audio.rec_stop);
        stop_result = 0;
        assert(wave_player_rec_start(100, 16000, 2) == 0);
        assert(stop_calls == 2 && ticks >= 200);
        assert(g_audio.rec_enable == (previous != 0) && !g_audio.rec_stop);
    }
    test_record_mute();
#if !defined(CONFIG_BSP_AUDIO_PCM)
    test_legacy_cli_controls();
#endif
    assert(critical_depth == 0);
    puts("Legacy controls, native failures, strict CLI arguments, active recording guards and retry passed");
    return 0;
}
