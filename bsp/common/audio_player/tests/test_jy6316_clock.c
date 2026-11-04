/* Exercise the real module and converter clock programming through I2C. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "audio_codec_jy6316.h"
#include "audio_codec_jy6311.h"
#include "bflb_i2c.h"
#include "bflb_clock.h"
#include "wave_player.h"
#include "bflb_i2s.h"
#include "wave_player_slots_under_test.h"

static uint8_t regs[128][256];
static struct bflb_device_s i2c;

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

int main(void)
{
    const uint32_t rates[] = { 8000, 12000, 16000, 22050, 24000, 32000, 44100, 48000 };
    for (unsigned i = 0; i < sizeof(rates) / sizeof(rates[0]); i++) {
        check_rate(rates[i], true, true);
        check_rate(rates[i], true, false);
        check_rate(rates[i], false, true);
    }
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
