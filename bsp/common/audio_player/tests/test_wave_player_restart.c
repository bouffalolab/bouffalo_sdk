/* Exercise the production public start wrapper and its configuration boundary. */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static struct {
    bool io_started;
    bool rec_enable;
    uint32_t src_hz;
    uint32_t logical_tx_block_bytes;
    uint32_t rx_block_bytes;
    uint8_t src_channels;
} g_audio;
static unsigned config_calls, start_calls;
static int config_result, start_result;
static bool configured_capture;

static int wave_player_config_internal(uint32_t rate, uint32_t tx_bytes, uint32_t rx_bytes, uint8_t channels)
{
    ++config_calls;
    assert(rate == g_audio.src_hz && tx_bytes == g_audio.logical_tx_block_bytes);
    assert(rx_bytes == g_audio.rx_block_bytes && channels == g_audio.src_channels);
    configured_capture = g_audio.rec_enable;
    if (config_result == 0) {
        g_audio.io_started = true;
    }
    return config_result;
}

static int wave_player_start_internal(void)
{
    assert(g_audio.io_started);
    ++start_calls;
    return start_result;
}

/* Generated from wave_player.c by the runner; no copied start implementation. */
#include "wave_player_restart_under_test.h"

static void reset_state(void)
{
    memset(&g_audio, 0, sizeof(g_audio));
    config_calls = start_calls = 0;
    config_result = start_result = 0;
    configured_capture = false;
    g_audio.src_hz = 16000;
    g_audio.logical_tx_block_bytes = 1280;
    g_audio.rx_block_bytes = 2560;
    g_audio.src_channels = 2;
}

int main(void)
{
    reset_state();
    g_audio.logical_tx_block_bytes = 0;
    assert(wave_player_start() == -EINVAL && config_calls == 0 && start_calls == 0);

    for (unsigned recording = 0; recording <= 1; ++recording) {
        reset_state();
        g_audio.rec_enable = recording != 0;
        assert(wave_player_start() == 0 && config_calls == 1 && start_calls == 1);
        assert(configured_capture == g_audio.rec_enable);
        assert(wave_player_start() == 0 && config_calls == 1 && start_calls == 2);

        /* Stop clears active transport; the current REC flag applies on restart. */
        g_audio.io_started = false;
        g_audio.rec_enable = !g_audio.rec_enable;
        assert(wave_player_start() == 0 && config_calls == 2 && start_calls == 3);
        assert(configured_capture == g_audio.rec_enable);
    }

    reset_state();
    config_result = -EIO;
    assert(wave_player_start() == -EIO && config_calls == 1 && start_calls == 0);
    assert(!g_audio.io_started);
    config_result = 0;
    assert(wave_player_start() == 0 && config_calls == 2 && start_calls == 1);

    reset_state();
    g_audio.io_started = true;
    start_result = -ENOMEM;
    assert(wave_player_start() == -ENOMEM && config_calls == 0 && start_calls == 1);
    puts("Wave restart: retained format, REC flag, active IO and error propagation passed");
    return 0;
}
