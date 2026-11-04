/* SPDX-License-Identifier: Apache-2.0 */
/* Execute the production codec initializer and LHAL DAC configuration against
 * a register bank; hardware DMA progress is validated separately on the board. */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "audio_codec_bl.h"
#include "bflb_audac.h"
#include "hardware/audac_reg.h"
#include "hardware/dac_reg.h"

#define GLB_BASE                         0x1000U
#define GLB_GPDAC_ANA_CLK_SEL            (1U << 12U)
#define GLB_GPDAC_DAT_CHA_SEL            (1U << 13U)
#define GLB_GPDAC_DAT_CHB_SEL            (1U << 14U)
#define LHAL_PARAM_ASSERT(value)         assert(value)
#define AUDIO_CODEC_BL_AUDAC_OUTPUT_MODE AUDAC_OUTPUT_MODE_PWM

struct bflb_device_s {
    uint32_t reg_base;
};

static uint32_t registers[64];
static uint32_t control_writes[8];
static unsigned control_write_count;
static unsigned volume_calls;
static unsigned link_calls;
static unsigned mute_calls;
static int requested_volume;
static bool requested_mute;
static bool fail_hal_init;
static volatile uint32_t g_audac_channel_mode;

static uint32_t getreg32(uint32_t address)
{
    assert(address < sizeof(registers) && (address % sizeof(uint32_t)) == 0U);
    return registers[address / sizeof(uint32_t)];
}

static void putreg32(uint32_t value, uint32_t address)
{
    assert(address < sizeof(registers) && (address % sizeof(uint32_t)) == 0U);
    registers[address / sizeof(uint32_t)] = value;
    if (address == AUDAC_0_OFFSET) {
        assert(control_write_count < sizeof(control_writes) / sizeof(control_writes[0]));
        control_writes[control_write_count++] = value;
    }
}

#define bflb_audac_init actual_audac_init
#include "audac_init_under_test.h"
#undef bflb_audac_init

int bflb_audac_init(struct bflb_device_s *device, const struct bflb_audac_init_config_s *config)
{
    assert((getreg32(device->reg_base + AUDAC_0_OFFSET) & AUDAC_CKG_ENA) == 0U);
    if (fail_hal_init) {
        return -13;
    }
    return actual_audac_init(device, config);
}

int bflb_audac_volume_init(struct bflb_device_s *device, const struct bflb_audac_volume_config_s *config)
{
    assert(device != NULL && config->mute_ramp_en);
    assert(config->volume_update_mode == AUDAC_VOLUME_UPDATE_MODE_RAMP);
    ++volume_calls;
    return 0;
}

int bflb_audac_link_rxdma(struct bflb_device_s *device, bool enable)
{
    assert(device != NULL && !enable);
    ++link_calls;
    return 0;
}

int bflb_audac_feature_control(struct bflb_device_s *device, int command, size_t value)
{
    assert(device != NULL);
    if (command == AUDAC_CMD_SET_MUTE) {
        assert(value == requested_mute);
        ++mute_calls;
    } else {
        assert(command == AUDAC_CMD_SET_VOLUME_VAL);
        assert((int)value == requested_volume);
    }
    return 0;
}

#include "builtin_playback_under_test.h"

int main(void)
{
    struct bflb_device_s device = { .reg_base = 0 };
    const audio_codec_bl_caps_t caps = { .playback_supported = true };
    audio_codec_bl_state_t state = { .audac_dev = &device, .caps = &caps, .volume_percent = 37, .mute = true };
    audio_codec_cfg_t config = { .sample_rate_hz = 16000 };
    const uint32_t rates[] = { 8000, 16000, 22050, 24000, 32000, 44100, 48000 };
    requested_mute = true;
    requested_volume = ((37 * 190) + 50) / 100 - 190;
    for (unsigned i = 0; i < sizeof(rates) / sizeof(rates[0]); ++i) {
        for (unsigned enabled = 0; enabled < 2; ++enabled) {
            memset(registers, 0, sizeof(registers));
            uint32_t prior = (4U << AUDAC_AU_PWM_MODE_SHIFT) | AUDAC_DAC_ITF_EN | AUDAC_DAC_0_EN;
            if (enabled) {
                prior |= AUDAC_CKG_ENA;
            }
            registers[AUDAC_0_OFFSET / sizeof(uint32_t)] = prior;
            control_write_count = volume_calls = link_calls = mute_calls = 0;
            config.sample_rate_hz = rates[i];
            assert(audio_codec_bl_init_playback(&state, &config) == 0);
            uint8_t rate = 0;
            assert(audio_codec_bl_map_audac_rate(rates[i], &rate) == 0);
            assert(control_write_count == 2);
            assert(control_writes[0] == (prior & ~(uint32_t)AUDAC_CKG_ENA));
            assert((control_writes[1] & AUDAC_CKG_ENA) != 0U);
            assert((control_writes[1] & AUDAC_AU_PWM_MODE_MASK) == (uint32_t)rate << AUDAC_AU_PWM_MODE_SHIFT);
            assert(volume_calls == 1 && link_calls == 1 && mute_calls == 1);
            assert(state.volume_percent == 37 && state.mute);
        }
    }
    uint32_t prior = registers[AUDAC_0_OFFSET / sizeof(uint32_t)];
    fail_hal_init = true;
    control_write_count = volume_calls = link_calls = mute_calls = 0;
    assert(audio_codec_bl_init_playback(&state, &config) == -13);
    assert(control_write_count == 2 && registers[AUDAC_0_OFFSET / sizeof(uint32_t)] == prior);
    assert(volume_calls == 0 && link_calls == 0 && mute_calls == 0);
    fail_hal_init = false;
    config.sample_rate_hz = 12345;
    control_write_count = 0;
    assert(audio_codec_bl_init_playback(&state, &config) == -3 && control_write_count == 0);
    assert(audio_codec_bl_init_playback(NULL, &config) == -2);
    assert(audio_codec_bl_init_playback(&state, NULL) == -2);
    puts("Builtin playback re-arms clock control before rate initialization and retains volume/mute");
    return 0;
}
