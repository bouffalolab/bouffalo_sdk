#include "audio_codec_es8388.h"
#include "audio_codec_internal.h"

#include <stddef.h>

#include "bflb_mtimer.h"

#define ES8388_I2C_DEFAULT_ADDR 0x10

/* ES8388 mute control (see e.g. ES8388 DACCONTROL3 bit2 in common drivers) */
#define ES8388_DACCONTROL3 0x19
#define ES8388_DACMUTE_MASK 0x04

#ifndef CONFIG_AUDIO_CODEC_ES8388_ENABLE_ADC_ALC
#define AUDIO_CODEC_ES8388_ENABLE_ADC_ALC 0
#else
#define AUDIO_CODEC_ES8388_ENABLE_ADC_ALC CONFIG_AUDIO_CODEC_ES8388_ENABLE_ADC_ALC
#endif

#ifndef AUDIO_CODEC_UNUSED
#define AUDIO_CODEC_UNUSED __attribute__((unused))
#endif

static int es8388_write_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    return audio_codec_i2c_write_reg8(i2c, addr, reg, val);
}

static int es8388_read_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *out)
{
    return audio_codec_i2c_read_reg8(i2c, addr, reg, out);
}

int audio_codec_es8388_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    /* ES8388 doesn't provide a simple chip-id register in this SDK driver.
     * We treat a successful I2C read as presence.
     */
    uint8_t tmp = 0;
    if (es8388_read_reg(i2c, addr, 0x00, &tmp) != 0) {
        return -1;
    }
    return 0;
}

static uint8_t map_fmt(audio_codec_i2s_fmt_t fmt)
{
    /* ES8388: 0=I2S, 1=Left, 2=Right, 3=DSP */
    switch (fmt) {
        case AUDIO_CODEC_I2S_FMT_I2S: return 0;
        case AUDIO_CODEC_I2S_FMT_LEFT_J: return 1;
        case AUDIO_CODEC_I2S_FMT_RIGHT_J: return 2;
        case AUDIO_CODEC_I2S_FMT_DSP: return 3;
        default: return 0;
    }
}

static uint8_t map_width(audio_codec_bits_t bits)
{
    /* In the original BSP: ES8388_DATA_LEN_16=3, 18=2, 20=1, 24=0, 32=4.
     * Keep common cases.
     */
    switch (bits) {
        case AUDIO_CODEC_BITS_16: return 3;
        case AUDIO_CODEC_BITS_32: return 4;
        case AUDIO_CODEC_BITS_24:
        default: return 0;
    }
}

static uint8_t es8388_pack_mic_pga(uint8_t mic_pga)
{
    /* Legacy BSP uses: (mic_pga << 4) | mic_pga */
    /* ES8388 MicAmpL[7:4]/MicAmpR[3:0] valid codes: 0..8 => 0..+24dB, 3dB/step. */
    if (mic_pga > 8) {
        mic_pga = 8;
    }
    return (uint8_t)((mic_pga << 4) | mic_pga);
}

static int es8388_config_adc_dynamics(struct bflb_device_s *i2c, uint8_t addr, bool enable_alc)
{
    int ret = 0;

    if (enable_alc) {
        ret |= es8388_write_reg(i2c, addr, 0x12, 0xE2);
        ret |= es8388_write_reg(i2c, addr, 0x13, 0xC0);
        ret |= es8388_write_reg(i2c, addr, 0x14, 0x12);
        ret |= es8388_write_reg(i2c, addr, 0x15, 0x06);
        ret |= es8388_write_reg(i2c, addr, 0x16, 0xC3);
    } else {
        /* Disable ALC and noise gate to avoid capture gain pumping. */
        ret |= es8388_write_reg(i2c, addr, 0x12, 0x00);
        ret |= es8388_write_reg(i2c, addr, 0x13, 0x00);
        ret |= es8388_write_reg(i2c, addr, 0x14, 0x00);
        ret |= es8388_write_reg(i2c, addr, 0x15, 0x00);
        ret |= es8388_write_reg(i2c, addr, 0x16, 0x00);
    }

    return ret;
}

static int es8388_codec_mode_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    const uint8_t addr = dev->i2c_addr;
    struct bflb_device_s *i2c = dev->i2c;

    uint8_t tempVal = 0;

    es8388_write_reg(i2c, addr, 0x01, 0x58);
    es8388_write_reg(i2c, addr, 0x01, 0x50);

    es8388_write_reg(i2c, addr, 0x02, 0xf3);
    es8388_write_reg(i2c, addr, 0x02, 0xf0);

    es8388_write_reg(i2c, addr, 0x2B, 0x80);
    es8388_write_reg(i2c, addr, 0x00, 0x36);

    /* role */
    if (cfg->codec_master) {
        es8388_write_reg(i2c, addr, 0x08, 0x8D);
    } else {
        es8388_write_reg(i2c, addr, 0x08, 0x00);
    }

    es8388_write_reg(i2c, addr, 0x04, 0x00);
    es8388_write_reg(i2c, addr, 0x05, 0x00);
    es8388_write_reg(i2c, addr, 0x06, 0xc3);

    /* Mic input topology (ported from the original ES8388_Codec_Mode()) */
    es8388_write_reg(i2c, addr, 0x0A, 0xf8);
    if (cfg->mic_input_mode == AUDIO_CODEC_MIC_INPUT_SINGLE) {
        es8388_write_reg(i2c, addr, 0x0B, 0x88);
    } else {
        es8388_write_reg(i2c, addr, 0x0B, 0x82);
    }

    tempVal = map_width(cfg->bits);
    tempVal <<= 2;
    tempVal |= map_fmt(cfg->i2s_fmt);
    es8388_write_reg(i2c, addr, 0x0C, tempVal);

    tempVal = map_width(cfg->bits);
    tempVal <<= 2;
    tempVal |= map_fmt(cfg->i2s_fmt);
    tempVal <<= 1;
    es8388_write_reg(i2c, addr, 0x17, tempVal);

    if (cfg->codec_master) {
        es8388_write_reg(i2c, addr, 0x0d, 0x06);
        es8388_write_reg(i2c, addr, 0x18, 0x06);
    }

    /* volumes 0dB */
    es8388_write_reg(i2c, addr, 0x10, 0x00);
    es8388_write_reg(i2c, addr, 0x11, 0x00);
    es8388_write_reg(i2c, addr, 0x1A, 0x00);
    es8388_write_reg(i2c, addr, 0x1B, 0x00);

    /* ADC PGA */
    if (cfg->enable_adc) {
        /* Default to 0dB; configure via audio_codec_set_mic_pga() if needed. */
        es8388_write_reg(i2c, addr, 0x09, es8388_pack_mic_pga(0));
    }

    if (es8388_config_adc_dynamics(i2c, addr, AUDIO_CODEC_ES8388_ENABLE_ADC_ALC != 0) != 0) {
        return -1;
    }

    /* Mixer */
    es8388_write_reg(i2c, addr, 0x27, 0xB8);
    es8388_write_reg(i2c, addr, 0x2A, 0xB8);

    es8388_write_reg(i2c, addr, 0x02, 0x00);
    bflb_mtimer_delay_ms(50);

    es8388_write_reg(i2c, addr, 0x2e, 0x1E);
    es8388_write_reg(i2c, addr, 0x2f, 0x1E);
    es8388_write_reg(i2c, addr, 0x30, 0x1E);
    es8388_write_reg(i2c, addr, 0x31, 0x1E);

    es8388_write_reg(i2c, addr, 0x04, 0x24);
    es8388_write_reg(i2c, addr, 0x26, 0x01);

    if (cfg->enable_adc) {
        es8388_write_reg(i2c, addr, 0x03, 0x09);
    } else {
        es8388_write_reg(i2c, addr, 0x03, 0x00);
    }

    return 0;
}

static int es8388_recording_mode_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    const uint8_t addr = dev->i2c_addr;
    struct bflb_device_s *i2c = dev->i2c;

    uint8_t tempVal = 0;

    es8388_write_reg(i2c, addr, 0x01, 0x58);
    es8388_write_reg(i2c, addr, 0x01, 0x50);

    es8388_write_reg(i2c, addr, 0x02, 0xf3);
    es8388_write_reg(i2c, addr, 0x02, 0xf0);
    es8388_write_reg(i2c, addr, 0x2B, 0x80);
    es8388_write_reg(i2c, addr, 0x00, 0x16);

    if (cfg->codec_master) {
        es8388_write_reg(i2c, addr, 0x08, 0x80);
    } else {
        es8388_write_reg(i2c, addr, 0x08, 0x00);
    }

    es8388_write_reg(i2c, addr, 0x04, 0xc0);
    es8388_write_reg(i2c, addr, 0x05, 0x00);
    es8388_write_reg(i2c, addr, 0x06, 0xc3);

    es8388_write_reg(i2c, addr, 0x0A, 0xf8);
    if (cfg->mic_input_mode == AUDIO_CODEC_MIC_INPUT_SINGLE) {
        es8388_write_reg(i2c, addr, 0x0B, 0x88);
    } else {
        es8388_write_reg(i2c, addr, 0x0B, 0x82);
    }

    tempVal = map_width(cfg->bits);
    tempVal <<= 2;
    tempVal |= map_fmt(cfg->i2s_fmt);
    es8388_write_reg(i2c, addr, 0x0C, tempVal);

    es8388_write_reg(i2c, addr, 0x0d, 0x02);

    es8388_write_reg(i2c, addr, 0x10, 0x00);
    es8388_write_reg(i2c, addr, 0x11, 0x00);

    /* ADC PGA */
    /* Default to 0dB; configure via audio_codec_set_mic_pga() if needed. */
    es8388_write_reg(i2c, addr, 0x09, es8388_pack_mic_pga(0));

    if (es8388_config_adc_dynamics(i2c, addr, AUDIO_CODEC_ES8388_ENABLE_ADC_ALC != 0) != 0) {
        return -1;
    }

    es8388_write_reg(i2c, addr, 0x02, 0x55);
    if (cfg->enable_adc) {
        es8388_write_reg(i2c, addr, 0x03, 0x09);
    } else {
        es8388_write_reg(i2c, addr, 0x03, 0x00);
    }

    return 0;
}

static int es8388_playback_mode_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    const uint8_t addr = dev->i2c_addr;
    struct bflb_device_s *i2c = dev->i2c;

    uint8_t tempVal = 0;

    es8388_write_reg(i2c, addr, 0x00, 0x80);
    es8388_write_reg(i2c, addr, 0x00, 0x06);
    bflb_mtimer_delay_ms(10);
    es8388_write_reg(i2c, addr, 0x02, 0xF3);

    if (cfg->codec_master) {
        es8388_write_reg(i2c, addr, 0x08, 0x80);
    } else {
        es8388_write_reg(i2c, addr, 0x08, 0x00);
    }

    es8388_write_reg(i2c, addr, 0x2B, 0x80);
    es8388_write_reg(i2c, addr, 0x00, 0x05);
    es8388_write_reg(i2c, addr, 0x01, 0x40);
    bflb_mtimer_delay_ms(30);

    /* ADC off in playback mode */
    es8388_write_reg(i2c, addr, 0x03, 0x00);

    if (cfg->enable_dac) {
        es8388_write_reg(i2c, addr, 0x04, 0x3F);
    } else {
        es8388_write_reg(i2c, addr, 0x04, 0x00);
    }

    /* Keep legacy ADC path settings (harmless even if ADC is off) */
    es8388_write_reg(i2c, addr, 0x09, 0x77);
    es8388_write_reg(i2c, addr, 0x0A, 0xF0);
    es8388_write_reg(i2c, addr, 0x0B, 0x02);

    tempVal = map_width(cfg->bits);
    tempVal <<= 2;
    tempVal |= map_fmt(cfg->i2s_fmt);
    es8388_write_reg(i2c, addr, 0x0C, tempVal);

    es8388_write_reg(i2c, addr, 0x0D, 0x04);
    es8388_write_reg(i2c, addr, 0x10, 0x00);
    es8388_write_reg(i2c, addr, 0x11, 0x00);
    if (es8388_config_adc_dynamics(i2c, addr, AUDIO_CODEC_ES8388_ENABLE_ADC_ALC != 0) != 0) {
        return -1;
    }

    tempVal = map_width(cfg->bits);
    tempVal <<= 2;
    tempVal |= map_fmt(cfg->i2s_fmt);
    tempVal <<= 1;

    es8388_write_reg(i2c, addr, 0x18, 0x04);
    es8388_write_reg(i2c, addr, 0x1A, 0x00);
    es8388_write_reg(i2c, addr, 0x1B, 0x00);
    es8388_write_reg(i2c, addr, 0x26, 0x00);
    es8388_write_reg(i2c, addr, 0x27, 0xB8);
    es8388_write_reg(i2c, addr, 0x28, 0x38);
    es8388_write_reg(i2c, addr, 0x29, 0x38);
    es8388_write_reg(i2c, addr, 0x2A, 0xB8);
    es8388_write_reg(i2c, addr, 0x2E, 0x1E);
    es8388_write_reg(i2c, addr, 0x2F, 0x1E);
    es8388_write_reg(i2c, addr, 0x30, 0x1E);
    es8388_write_reg(i2c, addr, 0x31, 0x1E);
    es8388_write_reg(i2c, addr, 0x02, 0x00);

    return 0;
}

int audio_codec_es8388_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || dev->i2c == NULL || cfg == NULL) {
        return -1;
    }
    if (dev->i2c_addr == 0) {
        dev->i2c_addr = ES8388_I2C_DEFAULT_ADDR;
    }

    switch (cfg->work_mode) {
        case AUDIO_CODEC_WORK_MODE_RECORDING:
            return es8388_recording_mode_init(dev, cfg);
        case AUDIO_CODEC_WORK_MODE_PLAYBACK:
            return es8388_playback_mode_init(dev, cfg);
        case AUDIO_CODEC_WORK_MODE_CODEC:
        default:
            return es8388_codec_mode_init(dev, cfg);
    }
}

int audio_codec_es8388_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    uint8_t volume = (uint8_t)(volume_percent / 3);

    int res = 0;
    res |= es8388_write_reg(dev->i2c, dev->i2c_addr, 0x2e, volume);
    res |= es8388_write_reg(dev->i2c, dev->i2c_addr, 0x2f, volume);
    res |= es8388_write_reg(dev->i2c, dev->i2c_addr, 0x30, volume);
    res |= es8388_write_reg(dev->i2c, dev->i2c_addr, 0x31, volume);
    return res;
}

int audio_codec_es8388_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    (void)dev;
    (void)sample_rate_hz;
    /* In slave mode the codec tracks LRCK; nothing to do. */
    return 0;
}

static int audio_codec_es8388_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    /* ES8388 Reg16/17 (0x10/0x11): LADCVOL/RADCVOL
     * 0x00 = 0dB, 0xC0 = -96dB, 0.5dB/step (attenuation).
     * Map percentage so 100% => 0dB (0x00), 0% => -96dB (0xC0).
     */
    uint32_t att_steps = (uint32_t)(((100 - volume_percent) * 192 + 50) / 100);
    if (att_steps > 192U) {
        att_steps = 192U;
    }

    uint8_t regv = (uint8_t)att_steps;
    int ret = 0;
    ret |= es8388_write_reg(dev->i2c, dev->i2c_addr, 0x10, regv);
    ret |= es8388_write_reg(dev->i2c, dev->i2c_addr, 0x11, regv);
    return ret;
}

static int audio_codec_es8388_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (pga_percent < 0) {
        pga_percent = 0;
    } else if (pga_percent > 100) {
        pga_percent = 100;
    }

    /* Map percentage to ES8388 PGA code 0..8 (0..+24dB, 3dB/step). */
    uint8_t step = (uint8_t)((pga_percent * 8 + 50) / 100);
    return es8388_write_reg(dev->i2c, dev->i2c_addr, 0x09, es8388_pack_mic_pga(step));
}

uint32_t audio_codec_es8388_calc_mclk_hz(uint32_t sample_rate_hz)
{
    return sample_rate_hz * 256U;
}

static int audio_codec_es8388_set_mute(audio_codec_dev_t *dev, bool mute)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    uint8_t regv = 0;
    if (es8388_read_reg(dev->i2c, dev->i2c_addr, ES8388_DACCONTROL3, &regv) != 0) {
        return -1;
    }

    regv &= (uint8_t)(~ES8388_DACMUTE_MASK);
    if (mute) {
        regv |= ES8388_DACMUTE_MASK;
    }

    return es8388_write_reg(dev->i2c, dev->i2c_addr, ES8388_DACCONTROL3, regv);
}

static const uint8_t es8388_addrs[] = { ES8388_I2C_DEFAULT_ADDR };

const audio_codec_driver_t audio_codec_es8388_driver = {
    .type = AUDIO_CODEC_TYPE_ES8388,
    .name = "ES8388",
    .i2c_addrs = es8388_addrs,
    .i2c_addrs_num = sizeof(es8388_addrs) / sizeof(es8388_addrs[0]),
    .probe = audio_codec_es8388_probe,
    .init = audio_codec_es8388_init,
    .set_volume = audio_codec_es8388_set_volume,
    .set_mute = audio_codec_es8388_set_mute,
    .set_record_volume = audio_codec_es8388_set_record_volume,
    .set_mic_pga = audio_codec_es8388_set_mic_pga,
    .set_sample_rate = audio_codec_es8388_set_sample_rate,
    .calc_mclk_hz = audio_codec_es8388_calc_mclk_hz,
};

static int AUDIO_CODEC_UNUSED es8388_legacy_reg_dump(struct bflb_device_s *i2c, uint8_t addr)
{
    for (int i = 0; i < 0x3F; i++) {
        uint8_t tmp = 0;
        if (es8388_read_reg(i2c, addr, (uint8_t)i, &tmp) != 0) {
            printf("iic read err\r\n");
            return -1;
        }
        printf("Reg[%02d]=0x%02x \n", i, tmp);
    }
    return 0;
}
