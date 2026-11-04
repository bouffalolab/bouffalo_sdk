/* SPDX-License-Identifier: Apache-2.0 */
/* Production legacy controls must not mutate a PCM-owned shared device. */
#include "wave_player.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>

static unsigned critical_depth;
#define taskENTER_CRITICAL() (++critical_depth)
#define taskEXIT_CRITICAL()  (--critical_depth)

#if defined(CONFIG_BSP_AUDIO_PCM)
static struct {
    bool claimed;
    unsigned legacy_operations;
    unsigned control_operations;
} g_pcm;
#endif

static struct {
    audio_codec_dev_t play_codec;
    audio_codec_dev_t cap_codec;
    uint8_t volume;
    bool muted;
    bool rec_enable;
    bool rec_stop;
} g_audio;

static unsigned volume_calls;
static unsigned mute_calls;
static unsigned capture_volume_calls;
static unsigned pga_calls;
static unsigned codec_read_calls;
static uint8_t playback_volume;
static uint8_t capture_volume;
static uint8_t capture_pga;
static bool playback_muted;

int audio_codec_set_volume(audio_codec_dev_t *codec, int volume)
{
    assert(codec == &g_audio.play_codec);
    playback_volume = volume;
    volume_calls++;
    return 0;
}

int audio_codec_set_mute(audio_codec_dev_t *codec, bool mute)
{
    assert(codec == &g_audio.play_codec);
    playback_muted = mute;
    mute_calls++;
    return 0;
}

int audio_codec_set_record_volume(audio_codec_dev_t *codec, int volume)
{
    assert(codec == &g_audio.cap_codec);
    capture_volume = volume;
    capture_volume_calls++;
    return 0;
}

static int wave_player_apply_rec_mic_pga(uint8_t pga)
{
    capture_pga = pga;
    pga_calls++;
    return 0;
}

static int wave_player_codec_cli_read_internal(bool capture, uint32_t reg, uint8_t width, uint32_t *value)
{
    assert(!capture && reg == 0 && width == 8 && value != NULL);
    codec_read_calls++;
    *value = 0x5a;
    return 0;
}

#include "wave_player_ownership_under_test.h"
#include "wave_player_controls_under_test.h"

int main(void)
{
    uint32_t register_value = 0;
    assert(wave_player_codec_cli_read(false, 0, 8, &register_value) == 0);
    assert(register_value == 0x5a && codec_read_calls == 1);
    assert(wave_player_set_volume(37) == 0);
    assert(wave_player_set_mute(true) == 0);
    assert(wave_player_set_record_volume(71) == 0);
    assert(wave_player_set_record_pga(62) == 0);
    assert(g_audio.volume == 37 && g_audio.muted && playback_muted);
    assert(playback_volume == 37 && capture_volume == 71 && capture_pga == 62);
    assert(volume_calls == 1 && mute_calls == 1 && capture_volume_calls == 1 && pga_calls == 1);
    assert(wave_player_get_volume() == 37 && wave_player_get_mute());
    assert(wave_player_rec_stop() == 0 && !g_audio.rec_stop);
    g_audio.rec_enable = true;
    assert(wave_player_rec_stop() == 0 && g_audio.rec_stop);
    g_audio.rec_stop = false;

#if defined(CONFIG_BSP_AUDIO_PCM)
    g_pcm.claimed = true;
    register_value = 0xcc;
    assert(wave_player_codec_cli_read(false, 0, 8, &register_value) == -EBUSY);
    assert(register_value == 0xcc && codec_read_calls == 1);
    assert(wave_player_set_volume(11) == -EBUSY);
    assert(wave_player_set_mute(false) == -EBUSY);
    assert(wave_player_set_record_volume(12) == -EBUSY);
    assert(wave_player_set_record_pga(13) == -EBUSY);
    assert(wave_player_rec_stop() == -EBUSY);
    assert(g_audio.volume == 37 && g_audio.muted && playback_muted);
    assert(playback_volume == 37 && capture_volume == 71 && capture_pga == 62);
    assert(volume_calls == 1 && mute_calls == 1 && capture_volume_calls == 1 && pga_calls == 1);
    assert(g_audio.rec_enable && !g_audio.rec_stop);
    assert(wave_player_get_volume() == 37 && wave_player_get_mute());
    g_pcm.claimed = false;
    assert(g_pcm.legacy_operations == 0);
    /* Nested legacy entry points may reserve the same unclaimed device. */
    assert(wave_player_legacy_enter() && wave_player_legacy_enter());
    assert(g_pcm.legacy_operations == 2);
    wave_player_legacy_leave();
    wave_player_legacy_leave();
    assert(g_pcm.legacy_operations == 0);
    g_pcm.control_operations = 1;
    assert(wave_player_codec_cli_read(false, 0, 8, &register_value) == -EBUSY);
    assert(codec_read_calls == 1);
    assert(wave_player_set_volume(11) == -EBUSY && playback_volume == 37);
    assert(g_pcm.legacy_operations == 0);
    g_pcm.control_operations = 0;
#endif

    /* Disabled PCM and a released PCM owner preserve legacy clamping and stop. */
    assert(wave_player_set_volume(255) == 0 && playback_volume == 100);
    assert(wave_player_set_record_volume(255) == 0 && capture_volume == 100);
    assert(wave_player_set_record_pga(255) == 0 && capture_pga == 100);
    assert(wave_player_set_mute(false) == 0 && !playback_muted);
    assert(wave_player_rec_stop() == 0 && g_audio.rec_stop);
    assert(volume_calls == 2 && mute_calls == 2 && capture_volume_calls == 2 && pga_calls == 2);
    assert(wave_player_codec_cli_read(false, 0, 8, &register_value) == 0);
    assert(register_value == 0x5a && codec_read_calls == 2);
    assert(critical_depth == 0);
    puts("Legacy controls respect PCM ownership and retain legacy operation without an owner");
    return 0;
}
