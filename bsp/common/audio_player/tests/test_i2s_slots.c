/* Keep bus slots independent of playback PCM channels and capture enable. */
#include "wave_player.h"
#include <assert.h>
#include <stdio.h>
#include "wave_player_slots_under_test.h"

int main(void)
{
    unsigned playback_only = wave_player_i2s_tx_slot_count(2, 2);
    unsigned full_duplex = wave_player_i2s_tx_slot_count(4, 2);
#if !WAVE_PLAYER_I2S_DSP_MODE
    assert(playback_only == 2 && full_duplex == 2);
#elif WAVE_PLAYER_I2S_TX_SLOTS == 0
    assert(playback_only == 2 && full_duplex == 4);
#else
    assert(playback_only == WAVE_PLAYER_I2S_TX_SLOTS && full_duplex == WAVE_PLAYER_I2S_TX_SLOTS);
#endif
    puts("I2S automatic/explicit TX slots and playback-only geometry passed");
}
