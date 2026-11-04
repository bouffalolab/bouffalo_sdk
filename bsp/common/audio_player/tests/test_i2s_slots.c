/* Keep bus slots independent of playback PCM channels and capture enable. */
#include "wave_player.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include "bflb_i2s.h"
#include "wave_player_slots_under_test.h"

int main(void)
{
    unsigned playback_only = wave_player_i2s_tx_slot_count(2, 2, AUDIO_CODEC_TYPE_ES8388);
    unsigned full_duplex = wave_player_i2s_tx_slot_count(4, 2, AUDIO_CODEC_TYPE_ES8388);
#if !WAVE_PLAYER_I2S_DSP_MODE
    assert(playback_only == 2 && full_duplex == 2);
#elif WAVE_PLAYER_I2S_TX_SLOTS == 0
    assert(playback_only == 2 && full_duplex == 4);
#else
    unsigned expected = WAVE_PLAYER_I2S_TX_SLOTS < 2U ? 2U : WAVE_PLAYER_I2S_TX_SLOTS;
    assert(playback_only == expected && full_duplex == expected);
#endif
    unsigned jy_play = wave_player_i2s_tx_slot_count(2, 2, AUDIO_CODEC_TYPE_JY6316);
    unsigned jy_duplex = wave_player_i2s_tx_slot_count(4, 2, AUDIO_CODEC_TYPE_JY6316);
#if WAVE_PLAYER_I2S_DSP_MODE
#if WAVE_PLAYER_I2S_TX_SLOTS == 0 || WAVE_PLAYER_I2S_TX_SLOTS == 4
    assert(jy_play == 4 && jy_duplex == 4);
    assert(wave_player_i2s_tx_slot_count(6, 2, AUDIO_CODEC_TYPE_JY6316) == 0);
#else
    assert(jy_play == 0 && jy_duplex == 0);
#endif
#else
    assert(jy_play == 2 && jy_duplex == 2);
#endif
    struct bflb_i2s_config_s bus;
    audio_fill_i2s_cfg(&bus, 8000, (uint8_t)playback_only);
    assert(bus.bclk_freq_hz == 8000U * playback_only * (WAVE_PLAYER_I2S_DSP_MODE ? 16U : 32U));
    puts("I2S automatic/explicit TX slots and playback-only geometry passed");
}
