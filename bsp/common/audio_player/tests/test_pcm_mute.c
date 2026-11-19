/* SPDX-License-Identifier: Apache-2.0 */
/* Validate real PCM packing and software speaker mute without touching source PCM. */
#include "wave_player.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct {
    wave_player_backend_t playback_backend;
    uint8_t tx_slot_count;
    uint8_t tx_slot_word_bytes;
} g_audio;
static bool speaker_muted;
static bool physical_playback_muted;
static uint16_t speaker_muted_mask;

bool wave_player_get_speaker_mute(void)
{
    return speaker_muted;
}

bool audio_controls_playback_is_muted(void)
{
    return physical_playback_muted;
}

uint16_t audio_controls_get_speaker_mute_mask(void)
{
    return speaker_muted_mask;
}

static void *expected_clean;
static uint32_t expected_bytes;
static unsigned clean_calls;

static void bflb_l1c_dcache_clean_range(void *address, uint32_t bytes)
{
    assert(address == expected_clean && bytes == expected_bytes);
    ++clean_calls;
}

#include "wave_player_pcm_fill_under_test.h"

static void check_packing(unsigned src_channels, unsigned dst_channels, unsigned slots, uint16_t mask, bool all)
{
    const int16_t source[] = { 11, 27, -33, 49, 55, -71, 87, -103 };
    int16_t original[sizeof(source) / sizeof(source[0])];
    memcpy(original, source, sizeof(source));
    int16_t output[32];
    for (unsigned i = 0; i < sizeof(output) / sizeof(output[0]); ++i) {
        output[i] = 12345;
    }
    const unsigned frames = 4;
    unsigned packed = dst_channels;
#if WAVE_PLAYER_I2S_DSP_MODE
    packed = slots;
#endif
    unsigned words = (frames + 1) * packed;
    assert(words + 2 <= sizeof(output) / sizeof(output[0]));
    expected_clean = &output[1];
    expected_bytes = words * sizeof(int16_t);
    clean_calls = 0;
    speaker_muted = physical_playback_muted = all;
    speaker_muted_mask = mask;
    g_audio.playback_backend = WAVE_PLAYER_BACKEND_I2S_CODEC;
    g_audio.tx_slot_count = slots;
    g_audio.tx_slot_word_bytes = sizeof(int16_t);
    pcm_fill_pcm_block(&output[1], source, frames * src_channels * sizeof(int16_t), src_channels,
                       dst_channels, expected_bytes);
    assert(clean_calls == 1 && output[0] == 12345 && output[words + 1] == 12345);
    assert(memcmp(source, original, sizeof(source)) == 0);
    for (unsigned frame = 0; frame < frames + 1; ++frame) {
        for (unsigned slot = 0; slot < packed; ++slot) {
            int16_t value = 0;
            if (frame < frames && !all && !(mask & (1U << slot))) {
                int16_t left = source[frame * src_channels];
                int16_t right = src_channels == 2 ? source[frame * src_channels + 1] : left;
#if WAVE_PLAYER_I2S_DSP_MODE
                value = (slot & 1U) && dst_channels == 2 ? right : left;
#else
                value = dst_channels == 1 ? (int16_t)(((int32_t)left + right) / 2) : (slot ? right : left);
#endif
            }
            assert(output[1 + frame * packed + slot] == value);
        }
    }
}

static void check_narrow_role(void)
{
    const int16_t source[] = { 11, 27, -33, 49 };
    const int16_t expected[] = { 0, 27, 0, 49 };
    int16_t output[4];
    speaker_muted = true;
    physical_playback_muted = false;
    speaker_muted_mask = 1;
    g_audio.playback_backend = WAVE_PLAYER_BACKEND_I2S_CODEC;
    g_audio.tx_slot_count = 2;
    g_audio.tx_slot_word_bytes = sizeof(int16_t);
    expected_clean = output;
    expected_bytes = sizeof(output);
    clean_calls = 0;
    pcm_fill_pcm_block(output, source, sizeof(source), 2, 2, sizeof(output));
    assert(wave_player_get_speaker_mute() && clean_calls == 1);
    assert(memcmp(output, expected, sizeof(output)) == 0);
}

#if WAVE_PLAYER_I2S_DSP_MODE
static void check_dsp_boundaries(void)
{
    const int16_t source[] = { 11, 27, -33, 49, 55, -71, 87, -103 };
    const struct {
        uint32_t source_bytes;
        uint32_t source_channels;
        uint32_t destination_channels;
        uint8_t slots;
        uint32_t destination_bytes;
        uint16_t mute_mask;
        bool null_source;
        bool all_muted;
        int16_t expected[16];
    } cases[] = {
        { 8, 2, 2, 4, 16, 0, false, false, { 11, 27, 11, 27, -33, 49, -33, 49 } },
        { 7, 2, 2, 4, 16, 0, false, false, { 11, 27, 11, 27 } },
        { 0, 2, 2, 4, 16, 0, false, false, { 0 } },
        { 8, 2, 2, 4, 16, 0, true, false, { 0 } },
        { 16, 2, 2, 4, 8, 0, false, false, { 11, 27, 11, 27 } },
        { 8, 2, 2, 4, 11, 0, false, false, { 11, 27, 11, 27 } },
        { 8, 2, 2, 4, 7, 0, false, false, { 0 } },
        { 8, 2, 2, 4, 0, 0, false, false, { 0 } },
        { 4, 1, 1, 6, 24, 0x20, false, false, { 11, 11, 11, 11, 11, 0, 27, 27, 27, 27, 27, 0 } },
        { 4, 2, 2, 16, 32, 0x8001, false, false,
          { 0, 27, 11, 27, 11, 27, 11, 27, 11, 27, 11, 27, 11, 27, 11, 0 } },
        { 4, 0, 0, 4, 16, 0, false, false, { 11, 11, 11, 11, 27, 27, 27, 27 } },
        { 8, 3, 3, 4, 16, 0, false, false, { 11, 27, 11, 27, -33, 49, -33, 49 } },
        { 8, 2, 2, 4, 16, 0, false, true, { 0 } },
    };

    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        int16_t output[18];
        int16_t original[sizeof(source) / sizeof(source[0])];
        memcpy(original, source, sizeof(source));
        memset(output, 0x5a, sizeof(output));
        uint8_t *destination = (uint8_t *)&output[1];
        expected_clean = destination;
        expected_bytes = cases[i].destination_bytes;
        clean_calls = 0;
        speaker_muted = physical_playback_muted = cases[i].all_muted;
        speaker_muted_mask = cases[i].mute_mask;
        g_audio.playback_backend = WAVE_PLAYER_BACKEND_I2S_CODEC;
        g_audio.tx_slot_count = cases[i].slots;
        g_audio.tx_slot_word_bytes = sizeof(int16_t);
        pcm_fill_pcm_block((int16_t *)destination, cases[i].null_source ? NULL : source,
                          cases[i].source_bytes, cases[i].source_channels, cases[i].destination_channels,
                          cases[i].destination_bytes);
        assert(clean_calls == 1);
        assert(memcmp(destination, cases[i].expected, cases[i].destination_bytes) == 0);
        assert(memcmp(source, original, sizeof(source)) == 0);
        assert(output[0] == 0x5a5a);
        for (unsigned byte = sizeof(int16_t) + cases[i].destination_bytes; byte < sizeof(output); ++byte) {
            assert(((uint8_t *)output)[byte] == 0x5a);
        }
    }
}
#endif

int main(void)
{
    const uint16_t masks[] = { 0, 1, 2, 3 };
    for (unsigned source = 1; source <= 2; ++source) {
        for (unsigned destination = 1; destination <= 2; ++destination) {
            for (unsigned i = 0; i < sizeof(masks) / sizeof(masks[0]); ++i) {
                check_packing(source, destination, 4, masks[i], false);
            }
            check_packing(source, destination, 4, 0, true);
        }
    }
    check_narrow_role();
#if WAVE_PLAYER_I2S_DSP_MODE
    check_dsp_boundaries();
#endif
    puts("PCM packing: selected speaker slots silenced, global mute and source/tail guards passed");
    return 0;
}
