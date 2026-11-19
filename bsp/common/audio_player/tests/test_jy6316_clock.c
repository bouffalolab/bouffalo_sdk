/* Exercise the real module and converter clock programming through I2C. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "audio_codec_jy6316.h"
#include "audio_codec_jy6311.h"
#include "audio_codec_jy6166.h"
#include "bflb_i2c.h"
#include "bflb_clock.h"
#include "wave_player.h"
#include "bflb_i2s.h"
#include "wave_player_slots_under_test.h"
#include "audio_codec_gain_under_test.h"

static uint8_t regs[128][256];
static struct bflb_device_s i2c;
static int failed_reg = -1;
static int failed_value = -1;
static uint8_t failed_value_mask = UINT8_MAX;
static unsigned jy6311_writes;
static uint8_t failed_addr = JY6316_JY6166_I2C_ADDR;

uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t index)
{
    assert(type == BFLB_DEVICE_TYPE_I2S && index == 0);
    return 24576000;
}

struct bflb_device_s *bflb_device_get_by_name(const char *name)
{
    (void)name;
    return &i2c;
}

void bflb_i2c_init(struct bflb_device_s *dev, uint32_t hz)
{
    (void)dev;
    (void)hz;
}

void bflb_mtimer_delay_ms(uint32_t ms)
{
    (void)ms;
}

uint64_t bflb_mtimer_get_time_us(void)
{
    return 0;
}

int bflb_i2c_transfer(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msg, int count)
{
    assert(dev == &i2c && count == 2 && msg[0].length == 1 && msg[1].length == 1);
    uint8_t *reg = &regs[msg[0].addr][msg[0].buffer[0]];
    if (msg[1].flags & I2C_M_READ) {
        msg[1].buffer[0] = *reg;
    } else {
        if (msg[0].addr == JY6316_JY6311_I2C_ADDR) {
            ++jy6311_writes;
        }
        if (msg[0].addr == failed_addr && msg[0].buffer[0] == failed_reg &&
            (failed_value < 0 || (msg[1].buffer[0] & failed_value_mask) == failed_value)) {
            return -1;
        }
        *reg = msg[1].buffer[0];
    }
    return 0;
}

static void check_rate(uint32_t hz, bool play, bool capture)
{
    static const unsigned divs[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24 };
    static const unsigned osrs[] = { 32, 64, 128, 256 };
    audio_codec_dev_t dev = { .i2c = &i2c };
    audio_codec_cfg_t cfg = {
        .sample_rate_hz = hz, .channels = capture ? 4 : 2,
        .bits = AUDIO_CODEC_BITS_16, .i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP,
        .enable_adc = capture, .enable_dac = play,
    };
    /* Cross-check the converter's registers against the production player's
     * I2S request, including automatic slots during playback-only. */
    uint8_t slots = wave_player_i2s_tx_slot_count(capture ? 4 : 2, play ? 2 : 0,
                                                AUDIO_CODEC_TYPE_JY6316);
    assert(slots == 4);
    struct bflb_i2s_config_s bus;
    audio_fill_i2s_cfg(&bus, hz, slots);
    assert(bus.channel_mode == I2S_CHANNEL_MODE_NUM_4);
    assert(bus.frame_width == I2S_SLOT_WIDTH_16 && bus.data_width == I2S_SLOT_WIDTH_16);

    memset(regs, 0, sizeof(regs));
    assert(audio_codec_jy6316_driver.init(&dev, &cfg) == 0);
    const uint8_t *r = regs[JY6316_JY6311_I2C_ADDR];
    unsigned source = (r[SYSCLK_CTRL] & SYSCLK_SRC_Msk) >> SYSCLK_SRC_Pos;
    uint64_t sysclk = bus.bclk_freq_hz;
    if (source == JY6311_SYSCLK_SRC_PLL) {
        assert(r[SYSCLK_CTRL] & PLLCLK_SRC_Msk); /* PLL input BCLK. */
        unsigned n = r[PLL_CFG2] + 1;
        unsigned p = (r[PLL_CFG1] & 31) + 1;
        unsigned m = 2 * (r[PLL_CFG3] & 15);
        assert(m != 0);
        sysclk = sysclk * n / (p * m);
    } else {
        assert(source == JY6311_SYSCLK_SRC_BCLK);
    }
    if (play) {
        assert(sysclk == (uint64_t)hz * osrs[r[DAC_CLK_CTRL] >> 4] * divs[r[DAC_CLK_CTRL] & 15]);
    }
    if (capture) {
        assert(sysclk == (uint64_t)hz * osrs[r[ADC_CLK_CTRL] >> 4] * divs[r[ADC_CLK_CTRL] & 15]);
    }
}

static void check_mic_slots(void)
{
    audio_codec_dev_t dev = { .i2c = &i2c, .drv = &audio_codec_jy6316_driver };
    audio_codec_cfg_t cfg = {
        .sample_rate_hz = 16000,
        .channels = 4,
        .bits = AUDIO_CODEC_BITS_16,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP,
        .enable_adc = true,
        .enable_dac = true,
    };
    uint8_t reference[256];
    uint8_t adc[256];
    uint8_t *mic = regs[JY6316_JY6166_I2C_ADDR];

    assert(audio_codec_jy6316_driver.init(&dev, &cfg) == 0);
    audio_codec_channel_caps_t caps;
    uint16_t capture_mask;
    uint16_t playback_mask;
    assert(audio_codec_get_channel_masks(&dev, &capture_mask, &playback_mask) == 0);
    assert(capture_mask == 0x0f && playback_mask == 0x01);
    assert(audio_codec_get_mic_channel_caps(&dev, 0, &caps) == 0 && caps.controls == 0);
    assert(audio_codec_get_mic_channel_caps(&dev, 3, &caps) == 0);
    assert(caps.gain_step_count == 14 && caps.gain_steps_db[0] == 0 && caps.gain_steps_db[1] == 6);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 3.0f) == 0);
    assert((mic[0x31] & 0xf0) == 0);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 4.0f) == 0);
    assert((mic[0x31] & 0xf0) == 0x20);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 43.0f) == -1);
    memcpy(reference, regs[JY6316_JY6311_I2C_ADDR], sizeof(reference));
    memcpy(adc, mic, sizeof(adc));
    assert(audio_codec_set_mic_channel_volume(&dev, 0x08, 31) == 0);
    adc[0x23] = (31 * 191 + 50) / 100;
    assert(memcmp(adc, mic, sizeof(adc)) == 0);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 12.0f) == 0);
    adc[0x31] = (adc[0x31] & 0x03) | 0x4c;
    assert(memcmp(adc, mic, sizeof(adc)) == 0);
    assert(audio_codec_set_mic_channel_volume(&dev, 0x04, 72) == 0);
    adc[0x22] = (72 * 191 + 50) / 100;
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x04, 33.0f) == 0);
    adc[0x30] = (adc[0x30] & 0x03) | 0xbc;
    assert(memcmp(adc, mic, sizeof(adc)) == 0);
    assert(memcmp(reference, regs[JY6316_JY6311_I2C_ADDR], sizeof(reference)) == 0);

    assert(audio_codec_set_mic_channel_volume(&dev, 0, 50) == -1);
    assert(audio_codec_set_mic_channel_volume(&dev, 0x09, 50) == -2);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x01, 24.0f) == -2);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x10, 50) == -2);
    assert(audio_codec_set_mic_channel_volume(&dev, 0x08, -1) == -1);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 101) == -1);
    assert(memcmp(adc, mic, sizeof(adc)) == 0);
    assert(memcmp(reference, regs[JY6316_JY6311_I2C_ADDR], sizeof(reference)) == 0);

    /* Failed writes must not replace the value restored on reconfiguration. */
    failed_reg = 0x23;
    assert(audio_codec_set_mic_channel_volume(&dev, 0x08, 90) == -1);
    failed_reg = 0x31;
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 100) == -1);
    failed_reg = -1;
    assert(audio_codec_jy6316_driver.set_sample_rate(&dev, 32000) == 0);
    assert(mic[0x22] == adc[0x22] && mic[0x23] == adc[0x23]);
    assert(mic[0x30] == adc[0x30] && mic[0x31] == adc[0x31]);

    const audio_codec_driver_t unsupported = { .get_mic_channel_caps = audio_codec_jy6316_driver.get_mic_channel_caps };
    dev.drv = &unsupported;
    assert(audio_codec_set_mic_channel_volume(&dev, 0x08, 20) == -2);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x08, 20) == -2);
    assert(memcmp(adc + 0x22, mic + 0x22, 2) == 0);
    assert(audio_codec_set_mic_channel_volume(NULL, 0x01, 50) == -1);

    /* Standalone ADC slots 0/1 use the same per-channel state restoration. */
    dev.drv = &audio_codec_jy6166_driver;
    dev.i2c_addr = JY6166_I2C_ADDR_0;
    cfg.channels = 2;
    cfg.enable_dac = false;
    assert(audio_codec_jy6166_driver.init(&dev, &cfg) == 0);
    assert(audio_codec_set_mic_channel_volume(&dev, 0x01, 20) == 0);
    assert(audio_codec_set_mic_channel_volume(&dev, 0x02, 80) == 0);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x01, 12.0f) == 0);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x02, 30.0f) == 0);
    memcpy(adc, mic, sizeof(adc));
    assert(audio_codec_jy6166_driver.set_sample_rate(&dev, 48000) == 0);
    assert(mic[0x22] == adc[0x22] && mic[0x23] == adc[0x23]);
    assert(mic[0x30] == adc[0x30] && mic[0x31] == adc[0x31]);
    assert(audio_codec_set_mic_channel_volume(&dev, 0x04, 50) == -2);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x05, 50) == -2);
    assert(audio_codec_jy6166_set_mic_channel_volume(&i2c, dev.i2c_addr, 4, 50) == -1);
    assert(audio_codec_jy6166_set_mic_channel_gain_db(&i2c, dev.i2c_addr, 0, 50) == -1);

    dev.drv = &audio_codec_jy6311_driver;
    dev.i2c_addr = JY6316_JY6311_I2C_ADDR;
    assert(audio_codec_set_mic_channel_volume(&dev, 0x01, 80) == 0);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x01, 24.0f) == 0);
    assert(audio_codec_set_speaker_channel_volume(&dev, 0x01, 70) == 0);
    uint8_t speaker_volume = regs[dev.i2c_addr][DAC_DVC_CTRL];
    assert(audio_codec_set_mic_channel_volume(&dev, 0x02, 80) == -2);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x03, 50) == -2);
    uint8_t volume = regs[dev.i2c_addr][ADC_DVC_CTRL];
    uint8_t pga = regs[dev.i2c_addr][ANA_ADC_CTRL1] & ADC_PGA_GAIN_Msk;
    failed_addr = dev.i2c_addr;
    failed_reg = DAC_DVC_CTRL;
    assert(audio_codec_set_speaker_channel_volume(&dev, 0x01, 20) == -1);
    assert(audio_codec_set_speaker_channel_mute(&dev, 0x01, true) == -1);
    failed_reg = ADC_DVC_CTRL;
    assert(audio_codec_set_mic_channel_volume(&dev, 0x01, 20) == -1);
    failed_reg = ANA_ADC_CTRL1;
    assert(audio_codec_set_mic_channel_gain_db(&dev, 0x01, 0.0f) == -1);
    failed_reg = -1;
    assert(audio_codec_jy6311_driver.set_sample_rate(&dev, 16000) == 0);
    assert(regs[dev.i2c_addr][ADC_DVC_CTRL] == volume);
    assert((regs[dev.i2c_addr][ANA_ADC_CTRL1] & ADC_PGA_GAIN_Msk) == pga);
    assert(regs[dev.i2c_addr][DAC_DVC_CTRL] == speaker_volume);
    puts("Mic slots: ADC register isolation, reference preservation, strict masks and reconfiguration passed");
}

static void check_native_restore_retry(void)
{
    audio_codec_dev_t dev = {
        .i2c = &i2c, .i2c_addr = JY6316_JY6311_I2C_ADDR, .drv = &audio_codec_jy6311_driver
    };
    audio_codec_cfg_t cfg = {
        .sample_rate_hz = 16000, .channels = 2, .bits = AUDIO_CODEC_BITS_16,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S, .enable_adc = true, .enable_dac = true
    };
    assert(audio_codec_jy6311_driver.init(&dev, &cfg) == 0);
    assert(audio_codec_set_speaker_channel_volume(&dev, 1, 70) == 0);
    assert(audio_codec_set_mic_channel_volume(&dev, 1, 80) == 0);
    assert(audio_codec_set_mic_channel_gain_db(&dev, 1, 24.0f) == 0);
    assert(audio_codec_set_speaker_channel_mute(&dev, 1, false) == 0);
    assert(audio_codec_set_mic_channel_mute(&dev, 1, false) == 0);
    uint8_t *r = regs[dev.i2c_addr];
    const uint8_t dac = r[DAC_DVC_CTRL];
    const uint8_t adc = r[ADC_DVC_CTRL];
    const uint8_t pga = r[ANA_ADC_CTRL1] & ADC_PGA_GAIN_Msk;
    const uint8_t registers[] = { DAC_DVC_CTRL, ADC_DVC_CTRL, ANA_ADC_CTRL1 };
    const uint8_t values[] = { dac, adc, pga };
    for (unsigned reg = 0; reg < 3; ++reg) {
        for (unsigned retry_new_rate = 0; retry_new_rate < 2; ++retry_new_rate) {
            assert(audio_codec_jy6311_driver.set_sample_rate(&dev, 16000) == 0);
            failed_addr = dev.i2c_addr;
            failed_reg = registers[reg];
            failed_value = values[reg];
            failed_value_mask = reg == 2 ? ADC_PGA_GAIN_Msk : UINT8_MAX;
            assert(audio_codec_jy6311_driver.set_sample_rate(&dev, 32000) == -1);
            failed_reg = failed_value = -1;
            unsigned before = jy6311_writes;
            uint32_t requested = retry_new_rate ? 32000 : 16000;
            assert(audio_codec_jy6311_driver.set_sample_rate(&dev, requested) == 0);
            assert(jy6311_writes > before); /* Neither target nor old Fs may take a stale fast path. */
            assert(r[DAC_DVC_CTRL] == dac && r[ADC_DVC_CTRL] == adc);
            assert((r[ANA_ADC_CTRL1] & ADC_PGA_GAIN_Msk) == pga);
            before = jy6311_writes;
            assert(audio_codec_jy6311_driver.set_sample_rate(&dev, requested) == 0);
            assert(jy6311_writes == before); /* Successful configuration commits the rate. */
        }
    }
    failed_value_mask = UINT8_MAX;
    puts("JY6311 restore: DAC/ADC/PGA failures, same-target retry and old-rate rollback retain native controls");
}

int main(void)
{
    const uint32_t rates[] = { 8000, 12000, 16000, 22050, 24000, 32000, 44100, 48000 };
    for (unsigned i = 0; i < sizeof(rates) / sizeof(rates[0]); i++) {
        check_rate(rates[i], true, true);
        check_rate(rates[i], true, false);
        check_rate(rates[i], false, true);
    }
    check_mic_slots();
    check_native_restore_retry();
    /* An unlisted PLL combination must fail initialization, not silently
     * continue with the vendor initializer's MCLK divider. */
    audio_codec_dev_t dev = { .i2c = &i2c };
    audio_codec_cfg_t unsupported = {
        .sample_rate_hz = 96000, .channels = 4,
        .bits = AUDIO_CODEC_BITS_16, .i2s_fmt = AUDIO_CODEC_I2S_FMT_DSP,
        .enable_adc = true, .enable_dac = true,
    };
    assert(audio_codec_jy6316_driver.init(&dev, &unsupported) != 0);
    puts("JY6316 BCLK: 24 player/codec clock profiles and unsupported PLL rejection passed");
}
