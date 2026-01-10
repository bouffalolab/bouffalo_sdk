#include "audio_codec_wm8978.h"

#include <stddef.h>

#include "bflb_i2c.h"
#include "bflb_mtimer.h"

#define WM8978_I2C_DEFAULT_ADDR 0x1A

#define WM8978_REG_NUM 58
//static uint16_t wm8978_reg_shadow[WM8978_REG_NUM];
static uint16_t wm8978_reg_shadow[58] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0100, 0x00FF,
    0x00FF, 0x0000, 0x012C, 0x002C, 0x002C, 0x002C, 0x002C, 0x0000,
    0x0032, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0038, 0x000B, 0x0032, 0x0000, 0x0008, 0x000C, 0x0093, 0x00E9,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0003, 0x0010, 0x0010, 0x0100,
    0x0100, 0x0002, 0x0001, 0x0001, 0x0039, 0x0039, 0x0039, 0x0039,
    0x0001, 0x0001
};
static int wm8978_write_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint16_t val)
{
    struct bflb_i2c_msg_s msgs[2];

    uint8_t reg_addr = (uint8_t)((reg << 1) | ((val >> 8) & 0x01));
    uint8_t reg_data = (uint8_t)(val & 0xFF);

    msgs[0].addr = addr;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg_addr;
    msgs[0].length = 1;

    msgs[1].addr = addr;
    msgs[1].flags = 0;
    msgs[1].buffer = &reg_data;
    msgs[1].length = 1;

    bflb_mtimer_delay_ms(1);
    int ret = bflb_i2c_transfer(i2c, msgs, 2);
    if (ret == 0 && reg < WM8978_REG_NUM) {
        wm8978_reg_shadow[reg] = val;
    }
    return ret;
}

#ifndef AUDIO_CODEC_UNUSED
#define AUDIO_CODEC_UNUSED __attribute__((unused))
#endif

static uint16_t wm8978_read_reg_shadow(uint8_t reg)
{
    if (reg < WM8978_REG_NUM) {
        return wm8978_reg_shadow[reg];
    }
    return 0;
}

int audio_codec_wm8978_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    /* WM8978 control interface is effectively write-only.
     * Best-effort probe: attempt a reset write and treat ACK as presence.
     */
    return wm8978_write_reg(i2c, addr, 0, 0);
}

static void wm8978_setvol_headset(struct bflb_device_s *i2c, uint8_t addr, uint8_t voll, uint8_t volr)
{
    voll &= 0x3F;
    volr &= 0x3F;
    if (voll == 0) {
        voll |= 1 << 6;
    }
    if (volr == 0) {
        volr |= 1 << 6;
    }
    (void)wm8978_write_reg(i2c, addr, 52, voll);
    (void)wm8978_write_reg(i2c, addr, 53, (uint16_t)(volr | (1 << 8)));
}

static void wm8978_setvol_speaker(struct bflb_device_s *i2c, uint8_t addr, uint8_t volx)
{
    volx &= 0x3F;
    if (volx == 0) {
        volx |= 1 << 6;
    }
    (void)wm8978_write_reg(i2c, addr, 54, volx);
    (void)wm8978_write_reg(i2c, addr, 55, (uint16_t)(volx | (1 << 8)));
}

static void wm8978_config_adda(struct bflb_device_s *i2c, uint8_t addr, uint8_t dacen, uint8_t adcen)
{
    /* WM8978 control interface is effectively write-only.
     * Some registers (e.g. R2/R3) carry multiple unrelated enable bits.
     * We must preserve the previously written value when toggling DAC/ADC.
     */
    uint16_t r3 = (3 < WM8978_REG_NUM) ? wm8978_reg_shadow[3] : 0;
    uint16_t r2 = (2 < WM8978_REG_NUM) ? wm8978_reg_shadow[2] : 0;

    if (dacen) {
        r3 |= (uint16_t)(3 << 0);
    } else {
        r3 &= (uint16_t)~(3 << 0);
    }

    if (adcen) {
        r2 |= (uint16_t)(3 << 0);
    } else {
        r2 &= (uint16_t)~(3 << 0);
    }

    (void)wm8978_write_reg(i2c, addr, 3, r3);
    (void)wm8978_write_reg(i2c, addr, 2, r2);
}

static void wm8978_config_output(struct bflb_device_s *i2c, uint8_t addr, uint8_t dacen, uint8_t bpsen)
{
    uint16_t regval = 0;
    if (dacen) {
        regval |= 1 << 0;
    }
    if (bpsen) {
        regval |= 1 << 1;
        regval |= 5 << 2;
    }
    (void)wm8978_write_reg(i2c, addr, 50, regval);
    (void)wm8978_write_reg(i2c, addr, 51, regval);
}

static void wm8978_config_i2s(struct bflb_device_s *i2c, uint8_t addr, audio_codec_i2s_fmt_t fmt, audio_codec_bits_t bits)
{
    uint8_t fmtv = 0;
    uint8_t bitv = 0;

    /* WM8978: fmt 0=Right,1=Left,2=I2S,3=DSP
     * bits: 0=16,1=20,2=24,3=32
     */
    switch (fmt) {
        case AUDIO_CODEC_I2S_FMT_LEFT_J: fmtv = 1; break;
        case AUDIO_CODEC_I2S_FMT_I2S: fmtv = 2; break;
        case AUDIO_CODEC_I2S_FMT_DSP: fmtv = 3; break;
        case AUDIO_CODEC_I2S_FMT_RIGHT_J:
        default: fmtv = 0; break;
    }

    switch (bits) {
        case AUDIO_CODEC_BITS_16: bitv = 0; break;
        case AUDIO_CODEC_BITS_24: bitv = 2; break;
        case AUDIO_CODEC_BITS_32: bitv = 3; break;
        default: bitv = 0; break;
    }

    (void)wm8978_write_reg(i2c, addr, 4, (uint16_t)((fmtv << 3) | (bitv << 5)));
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_setgain_mic(struct bflb_device_s *i2c, uint8_t addr, uint8_t gain)
{
    gain &= 0x3F;
    (void)wm8978_write_reg(i2c, addr, 45, gain);
    (void)wm8978_write_reg(i2c, addr, 46, (uint16_t)(gain | (1 << 8)));
}

static int wm8978_set_mic_gain_raw(struct bflb_device_s *i2c, uint8_t addr, uint8_t gain)
{
    /* WM8978 uses 6-bit mic gain in regs 45/46.
     * Bit8 in the right register is an update bit (mirror existing driver behavior).
     */
    gain &= 0x3F;
    int ret = 0;
    ret |= wm8978_write_reg(i2c, addr, 45, gain);
    ret |= wm8978_write_reg(i2c, addr, 46, (uint16_t)(gain | (1 << 8)));
    return ret;
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_setgain_linein(struct bflb_device_s *i2c, uint8_t addr, uint8_t gain)
{
    gain &= 0x07;

    uint16_t regval = wm8978_read_reg_shadow(47);
    regval &= (uint16_t)~(7 << 4);
    (void)wm8978_write_reg(i2c, addr, 47, (uint16_t)(regval | (gain << 4)));

    regval = wm8978_read_reg_shadow(48);
    regval &= (uint16_t)~(7 << 4);
    (void)wm8978_write_reg(i2c, addr, 48, (uint16_t)(regval | (gain << 4)));
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_setgain_aux(struct bflb_device_s *i2c, uint8_t addr, uint8_t gain)
{
    gain &= 0x07;

    uint16_t regval = wm8978_read_reg_shadow(47);
    regval &= (uint16_t)~(7 << 0);
    (void)wm8978_write_reg(i2c, addr, 47, (uint16_t)(regval | (gain << 0)));

    regval = wm8978_read_reg_shadow(48);
    regval &= (uint16_t)~(7 << 0);
    (void)wm8978_write_reg(i2c, addr, 48, (uint16_t)(regval | (gain << 0)));
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_input(struct bflb_device_s *i2c, uint8_t addr, uint8_t micen, uint8_t lineinen, uint8_t auxen)
{
    uint16_t regval = wm8978_read_reg_shadow(2);
    if (micen) {
        regval |= (uint16_t)(3 << 2);
    } else {
        regval &= (uint16_t)~(3 << 2);
    }
    (void)wm8978_write_reg(i2c, addr, 2, regval);

    regval = wm8978_read_reg_shadow(44);
    if (micen) {
        regval |= (uint16_t)((3 << 4) | (3 << 0));
    } else {
        regval &= (uint16_t)~((3 << 4) | (3 << 0));
    }
    (void)wm8978_write_reg(i2c, addr, 44, regval);

    if (lineinen) {
        wm8978_legacy_setgain_linein(i2c, addr, 5);
    } else {
        wm8978_legacy_setgain_linein(i2c, addr, 0);
    }

    if (auxen) {
        wm8978_legacy_setgain_aux(i2c, addr, 7);
    } else {
        wm8978_legacy_setgain_aux(i2c, addr, 0);
    }
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_3d(struct bflb_device_s *i2c, uint8_t addr, uint8_t depth)
{
    depth &= 0x0F;
    (void)wm8978_write_reg(i2c, addr, 41, depth);
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_3deq(struct bflb_device_s *i2c, uint8_t addr, uint8_t dir)
{
    uint16_t regval = wm8978_read_reg_shadow(18);
    if (dir) {
        regval |= (uint16_t)(1 << 8);
    } else {
        regval &= (uint16_t)~(1 << 8);
    }
    (void)wm8978_write_reg(i2c, addr, 18, regval);
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_eq1(struct bflb_device_s *i2c, uint8_t addr, uint8_t cfreq, uint8_t gain)
{
    cfreq &= 0x03;
    if (gain > 24) {
        gain = 24;
    }
    gain = (uint8_t)(24 - gain);

    uint16_t regval = wm8978_read_reg_shadow(18);
    regval &= 0x0100;
    regval |= (uint16_t)(cfreq << 5);
    regval |= gain;
    (void)wm8978_write_reg(i2c, addr, 18, regval);
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_eq2(struct bflb_device_s *i2c, uint8_t addr, uint8_t cfreq, uint8_t gain)
{
    cfreq &= 0x03;
    if (gain > 24) {
        gain = 24;
    }
    gain = (uint8_t)(24 - gain);

    uint16_t regval = 0;
    regval |= (uint16_t)(cfreq << 5);
    regval |= gain;
    (void)wm8978_write_reg(i2c, addr, 19, regval);
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_eq3(struct bflb_device_s *i2c, uint8_t addr, uint8_t cfreq, uint8_t gain)
{
    cfreq &= 0x03;
    if (gain > 24) {
        gain = 24;
    }
    gain = (uint8_t)(24 - gain);

    uint16_t regval = 0;
    regval |= (uint16_t)(cfreq << 5);
    regval |= gain;
    (void)wm8978_write_reg(i2c, addr, 20, regval);
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_eq4(struct bflb_device_s *i2c, uint8_t addr, uint8_t cfreq, uint8_t gain)
{
    cfreq &= 0x03;
    if (gain > 24) {
        gain = 24;
    }
    gain = (uint8_t)(24 - gain);

    uint16_t regval = 0;
    regval |= (uint16_t)(cfreq << 5);
    regval |= gain;
    (void)wm8978_write_reg(i2c, addr, 21, regval);
}

static void AUDIO_CODEC_UNUSED wm8978_legacy_config_eq5(struct bflb_device_s *i2c, uint8_t addr, uint8_t cfreq, uint8_t gain)
{
    cfreq &= 0x03;
    if (gain > 24) {
        gain = 24;
    }
    gain = (uint8_t)(24 - gain);

    uint16_t regval = 0;
    regval |= (uint16_t)(cfreq << 5);
    regval |= gain;
    (void)wm8978_write_reg(i2c, addr, 22, regval);
}

int audio_codec_wm8978_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || dev->i2c == NULL || cfg == NULL) {
        return -1;
    }

    if (dev->i2c_addr == 0) {
        dev->i2c_addr = WM8978_I2C_DEFAULT_ADDR;
    }

    if (wm8978_write_reg(dev->i2c, dev->i2c_addr, 0, 0) != 0) {
        return -2;
    }

    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 1, 0x1B);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 2, 0x1B0);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 3, 0x6C);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 6, 0);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 43, 1 << 4);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 47, 1 << 8);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 48, 1 << 8);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 49, 1 << 1);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 10, 1 << 3);
    (void)wm8978_write_reg(dev->i2c, dev->i2c_addr, 14, 1 << 3);

    wm8978_config_adda(dev->i2c, dev->i2c_addr, cfg->enable_dac ? 1 : 0, cfg->enable_adc ? 1 : 0);
    wm8978_config_output(dev->i2c, dev->i2c_addr, cfg->enable_dac ? 1 : 0, 0);
    wm8978_config_i2s(dev->i2c, dev->i2c_addr, cfg->i2s_fmt, cfg->bits);

    return 0;
}

int audio_codec_wm8978_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    uint8_t vol63 = (uint8_t)((volume_percent * 63) / 100);
    wm8978_setvol_headset(dev->i2c, dev->i2c_addr, vol63, vol63);
    wm8978_setvol_speaker(dev->i2c, dev->i2c_addr, vol63);
    return 0;
}

static int audio_codec_wm8978_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    /* WM8978 ADC Digital Volume:
     * - R15 (ADCVOLL[7:0]): 0x00 mute, 0x01=-127dB ... 0xFF=0dB (0.5dB/step)
     * - R16 (ADCVOLR[7:0]): same
     * - ADCVU (bit8 in R15/R16): volume update latch; write 1 to update both.
     *
     * Map percentage so 100% => 0dB (0xFF), 0% => mute (0x00).
     */
    uint8_t vol = (uint8_t)((volume_percent * 255 + 50) / 100);
    if (volume_percent > 0 && vol == 0) {
        vol = 1;
    }

    int ret = 0;
    ret |= wm8978_write_reg(dev->i2c, dev->i2c_addr, 15, vol);
    /* Set ADCVU=1 on the second write to latch both channels. */
    ret |= wm8978_write_reg(dev->i2c, dev->i2c_addr, 16, (uint16_t)(vol | (1 << 8)));
    return ret;
}

static int audio_codec_wm8978_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (pga_percent < 0) {
        pga_percent = 0;
    } else if (pga_percent > 100) {
        pga_percent = 100;
    }

    /* Map percentage to WM8978 mic gain range (0..63). */
    uint8_t step = (uint8_t)((pga_percent * 63 + 50) / 100);
    return wm8978_set_mic_gain_raw(dev->i2c, dev->i2c_addr, step);
}

int audio_codec_wm8978_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    (void)dev;
    (void)sample_rate_hz;
    /* Slave mode: codec follows LRCK. */
    return 0;
}

uint32_t audio_codec_wm8978_calc_mclk_hz(uint32_t sample_rate_hz)
{
    return sample_rate_hz * 256U;
}

static const uint8_t wm8978_addrs[] = { WM8978_I2C_DEFAULT_ADDR };

const audio_codec_driver_t audio_codec_wm8978_driver = {
    .type = AUDIO_CODEC_TYPE_WM8978,
    .name = "WM8978",
    .i2c_addrs = wm8978_addrs,
    .i2c_addrs_num = sizeof(wm8978_addrs) / sizeof(wm8978_addrs[0]),
    .probe = audio_codec_wm8978_probe,
    .init = audio_codec_wm8978_init,
    .set_volume = audio_codec_wm8978_set_volume,
    .set_record_volume = audio_codec_wm8978_set_record_volume,
    .set_mic_pga = audio_codec_wm8978_set_mic_pga,
    .set_sample_rate = audio_codec_wm8978_set_sample_rate,
    .calc_mclk_hz = audio_codec_wm8978_calc_mclk_hz,
};
