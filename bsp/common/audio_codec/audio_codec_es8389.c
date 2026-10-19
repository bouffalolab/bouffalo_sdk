#include "audio_codec_es8389.h"
#include "audio_codec_internal.h"

#include <stddef.h>

#include "bflb_mtimer.h"

#define ES8389_I2C_DEFAULT_ADDR  0x12

/*
 * ES8389 Register Definitions
 */
#define ES8389_RESET_REG00       0x00
#define ES8389_MASTER_MODE_REG01 0x01
#define ES8389_MASTER_CLK_REG02  0x02
#define ES8389_CLK_OFF_REG03     0x03
#define ES8389_CLK_DIV_REG04     0x04
#define ES8389_CLK_MUL_REG05     0x05
#define ES8389_CLK_MUX_REG06     0x06
#define ES8389_CLK_MUX_REG07     0x07
#define ES8389_CLK_CTL_REG08     0x08
#define ES8389_CLK_CTL_REG09     0x09
#define ES8389_CLK_CTL_REG0A     0x0A
#define ES8389_SCLK_DIV_REG0B    0x0B
#define ES8389_LRCK_DIVH_REG0C   0x0C
#define ES8389_LRCK_DIVL_REG0D   0x0D
#define ES8389_OSC_CLK_REG0F     0x0F
#define ES8389_CSM_JUMP_REG10    0x10
#define ES8389_CLK_DIV_REG11     0x11
#define ES8389_SYSTEM_REG12      0x12
#define ES8389_SYSTEM_REG13      0x13
#define ES8389_SYSTEM_REG14      0x14
#define ES8389_SYSTEM_REG15      0x15
#define ES8389_SYSTEM_REG16      0x16
#define ES8389_SYSTEM_REG17      0x17
#define ES8389_SYSTEM_REG18      0x18
#define ES8389_SYSTEM_REG19      0x19
#define ES8389_SYSTEM_REG1A      0x1A
#define ES8389_SYSTEM_REG1B      0x1B
#define ES8389_SYSTEM_REG1C      0x1C

#define ES8389_ADC_REG20         0x20
#define ES8389_ADC_REG21         0x21
#define ES8389_ADC_REG22         0x22
#define ES8389_ADC_MODE_REG23    0x23
#define ES8389_ADC_HPF1_REG24    0x24
#define ES8389_ADC_HPF2_REG25    0x25
#define ES8389_DMIC_VOL_REG26    0x26
#define ES8389_ADCL_VOL_REG27    0x27
#define ES8389_ADCR_VOL_REG28    0x28
#define ES8389_PTDM_SLOT_REG2A   0x2A
#define ES8389_ADC_MIX_REG31     0x31

#define ES8389_DAC_REG40         0x40
#define ES8389_DAC_REG41         0x41
#define ES8389_DAC_REG42         0x42
#define ES8389_DAC_REG43         0x43
#define ES8389_DAC_MIX_REG44     0x44
#define ES8389_DAC_INV_REG45     0x45
#define ES8389_DACL_VOL_REG46    0x46
#define ES8389_DACR_VOL_REG47    0x47
#define ES8389_MIX_VOL_REG48     0x48
#define ES8389_DAC_RAMP_REG49    0x49
#define ES8389_SYSTEM_REG4C      0x4C
#define ES8389_DAC_RESET_REG4D   0x4D

#define ES8389_VMID_REG60        0x60
#define ES8389_ANA_CTL1_REG61    0x61
#define ES8389_ANA_VSEL_REG62    0x62
#define ES8389_ANA_CTL2_REG63    0x63
#define ES8389_ADC_EN_REG64      0x64
#define ES8389_HPSW_REG69        0x69
#define ES8389_LOW_POWER1_REG6B  0x6B
#define ES8389_LOW_POWER2_REG6C  0x6C
#define ES8389_DMIC_EN_REG6D     0x6D
#define ES8389_PGA_SW_REG6E      0x6E
#define ES8389_MOD_SW1_REG6F     0x6F
#define ES8389_MOD_SW2_REG70     0x70
#define ES8389_MOD_SW3_REG71     0x71
#define ES8389_MIC1_GAIN_REG72   0x72
#define ES8389_MIC2_GAIN_REG73   0x73

#define ES8389_CHIP_MISC_REGF0   0xF0
#define ES8389_CSM_STATE1_REGF1  0xF1
#define ES8389_PULL_DOWN_REGF2   0xF2
#define ES8389_ISO_CTL_REGF3     0xF3
#define ES8389_CSM_STATE2_REGF4  0xF4

#define ES8389_CHIP_ID0_REGFD    0xFD
#define ES8389_CHIP_ID1_REGFE    0xFE

/* ES8389 Format definitions */
#define ES8389_DAIFMT_MASK       0x1C
#define ES8389_DAIFMT_I2S        (0 << 2)
#define ES8389_DAIFMT_LEFT_J     (1 << 2)
#define ES8389_DAIFMT_DSP_A      (2 << 2)
#define ES8389_DAIFMT_DSP_B      (3 << 2)

#define ES8389_DATA_LEN_MASK     0xE0
#define ES8389_S24_LE            (0 << 5)
#define ES8389_S20_3_LE          (1 << 5)
#define ES8389_S18_LE            (2 << 5)
#define ES8389_S16_LE            (3 << 5)
#define ES8389_S32_LE            (4 << 5)

#define ES8389_MCLK_LRCK_RATIO   256

#ifndef AUDIO_CODEC_UNUSED
#define AUDIO_CODEC_UNUSED __attribute__((unused))
#endif

/* I2C register access functions */
static int es8389_write_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    if (audio_codec_i2c_write_reg8(i2c, addr, reg, val) != 0) {
        printf("ES8389 i2c write reg failed:0x%02x\n", reg);
        return -1;
    }
    return 0;
}

static int es8389_read_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *out)
{
    return audio_codec_i2c_read_reg8(i2c, addr, reg, out);
}

static int es8389_update_bits(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg,
                              uint8_t mask, uint8_t val)
{
    uint8_t tmp;
    int ret;

    ret = es8389_read_reg(i2c, addr, reg, &tmp);
    if (ret < 0) {
        return ret;
    }

    tmp = (tmp & ~mask) | (val & mask);
    return es8389_write_reg(i2c, addr, reg, tmp);
}

/* Probe function - check if ES8389 is present */
int audio_codec_es8389_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    uint8_t tmp = 0;
    /* Try to read chip ID register */
    if (es8389_read_reg(i2c, addr, ES8389_CHIP_ID1_REGFE, &tmp) != 0) {
        return -1;
    }
    /* ES8389 chip ID should be 0x89 */
    if ((tmp & 0xFF) != 0x89) {
        return -1;
    }
    return 0;
}

/* Map audio_codec format to ES8389 format */
static uint8_t map_fmt(audio_codec_i2s_fmt_t fmt)
{
    switch (fmt) {
        case AUDIO_CODEC_I2S_FMT_I2S:
            return ES8389_DAIFMT_I2S;
        case AUDIO_CODEC_I2S_FMT_LEFT_J:
            return ES8389_DAIFMT_LEFT_J;
        case AUDIO_CODEC_I2S_FMT_RIGHT_J:
            return 0; /* Not directly supported, use I2S */
        case AUDIO_CODEC_I2S_FMT_DSP:
            return ES8389_DAIFMT_DSP_A;
        default:
            return ES8389_DAIFMT_I2S;
    }
}

/* Map audio_codec bits to ES8389 data length */
static uint8_t map_width(audio_codec_bits_t bits)
{
    switch (bits) {
        case AUDIO_CODEC_BITS_16:
            return ES8389_S16_LE;
        case AUDIO_CODEC_BITS_24:
            return ES8389_S24_LE;
        case AUDIO_CODEC_BITS_32:
            return ES8389_S32_LE;
        default:
            return ES8389_S16_LE;
    }
}

/* Calculate mic input gain register value */
static uint8_t map_mic_input(audio_codec_mic_input_t mic_input)
{
    /* Reg0x72/0x73: [7:4]=Input Select, [3:0]=PGA Gain */
    switch (mic_input) {
        case AUDIO_CODEC_MIC_INPUT_DIFF:
            return 0x10;   /* Diff MIC1P-MIC1N / Diff MIC2N-MIC2P */
        case AUDIO_CODEC_MIC_INPUT_SINGLE:
            return 0x50;   /* Single MIC1P / Single MIC2N */
        case AUDIO_CODEC_MIC_INPUT_DIGITAL:
            return 0x10;   /* Use same setting for DMIC, controlled via Reg0x6D */
        default:
            return 0x10;   /* Default differential */
    }
}

/* Main codec initialization function */
static int es8389_codec_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    const uint8_t addr = dev->i2c_addr;
    struct bflb_device_s *i2c = dev->i2c;
    uint8_t state;
    uint8_t mic_input_val;
    uint8_t pga_gain;

    /* Reset sequence */
    es8389_write_reg(i2c, addr, ES8389_ISO_CTL_REGF3, 0x00);
    es8389_write_reg(i2c, addr, ES8389_RESET_REG00, 0x7E);
    bflb_mtimer_delay_ms(10);
    es8389_write_reg(i2c, addr, ES8389_ISO_CTL_REGF3, 0x38);

    /* ADC HPF coefficients */
    es8389_write_reg(i2c, addr, ES8389_ADC_HPF1_REG24, 0x64);
    es8389_write_reg(i2c, addr, ES8389_ADC_HPF2_REG25, 0x04);
    es8389_write_reg(i2c, addr, ES8389_DAC_INV_REG45, 0x03);

    /* Analog power management - initial settings */
    es8389_write_reg(i2c, addr, ES8389_VMID_REG60, 0x2A);
    es8389_write_reg(i2c, addr, ES8389_ANA_CTL1_REG61, 0xC9);
    es8389_write_reg(i2c, addr, ES8389_ANA_VSEL_REG62, 0x4F);
    es8389_write_reg(i2c, addr, ES8389_ANA_CTL2_REG63, 0x06);
    es8389_write_reg(i2c, addr, ES8389_LOW_POWER1_REG6B, 0x00);

    /* PGA and switch settings */
    es8389_write_reg(i2c, addr, ES8389_PGA_SW_REG6E, 0xAA);
    es8389_write_reg(i2c, addr, ES8389_MOD_SW1_REG6F, 0x66);
    es8389_write_reg(i2c, addr, ES8389_MOD_SW2_REG70, 0x99);

    /* DMIC/AMIC configuration */
    if (cfg->mic_input_mode == AUDIO_CODEC_MIC_INPUT_DIGITAL) {
        /* Enable DMIC mode */
        es8389_write_reg(i2c, addr, ES8389_DMIC_EN_REG6D, 0x16 | 0xC0);
        es8389_update_bits(i2c, addr, ES8389_ADC_MODE_REG23, 0x03, 0x03);
    } else {
        /* AMIC mode - enable mic bias */
        es8389_write_reg(i2c, addr, ES8389_DMIC_EN_REG6D, 0x16);
        es8389_update_bits(i2c, addr, ES8389_ADC_MODE_REG23, 0x03, 0x00);
    }

    /* Microphone input configuration and PGA gain */
    mic_input_val = map_mic_input(cfg->mic_input_mode);
    /* Default PGA gain: use mid-range (8 = +24dB) */
    pga_gain = 8;

    es8389_write_reg(i2c, addr, ES8389_MIC1_GAIN_REG72, mic_input_val | pga_gain);
    es8389_write_reg(i2c, addr, ES8389_MIC2_GAIN_REG73, mic_input_val | pga_gain);

    /* CSM and master/slave mode */
    es8389_write_reg(i2c, addr, ES8389_CSM_JUMP_REG10, 0xC4);
    /* Reg[01]: bit7=BCLK_TRI, bit6=LRCK_TRI, bit5=ASDOUT_TRI, bit0=MS mode
     * For Slave mode: 0x08 (bit3=1 for internal clock, bit0=0 for slave)
     * For Master mode: 0x09 (bit3=1, bit0=1)
     */
    es8389_write_reg(i2c, addr, ES8389_MASTER_MODE_REG01, cfg->codec_master ? 0x09 : 0x08);
    es8389_write_reg(i2c, addr, ES8389_CSM_STATE1_REGF1, 0x00);

    /* System registers initialization */
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG12, 0x01);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG13, 0x01);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG14, 0x01);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG15, 0x01);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG16, 0x35);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG17, 0x09);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG18, 0x91);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG19, 0x28);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG1A, 0x01);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG1B, 0x01);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG1C, 0x11);

    /* Clock configuration */
    es8389_write_reg(i2c, addr, ES8389_CHIP_MISC_REGF0, 0x13);
    es8389_write_reg(i2c, addr, ES8389_MASTER_CLK_REG02, 0x00);
    es8389_write_reg(i2c, addr, ES8389_CLK_DIV_REG04, 0x00);
    es8389_write_reg(i2c, addr, ES8389_CLK_MUL_REG05, 0x10);
    es8389_write_reg(i2c, addr, ES8389_CLK_MUX_REG06, 0x00);
    es8389_write_reg(i2c, addr, ES8389_CLK_MUX_REG07, 0xC0);
    es8389_write_reg(i2c, addr, ES8389_CLK_CTL_REG08, 0x00);
    es8389_write_reg(i2c, addr, ES8389_CLK_CTL_REG09, 0xC0);
    es8389_write_reg(i2c, addr, ES8389_CLK_CTL_REG0A, 0x80);
    es8389_write_reg(i2c, addr, ES8389_SCLK_DIV_REG0B, 0x04);
    es8389_write_reg(i2c, addr, ES8389_LRCK_DIVH_REG0C, (ES8389_MCLK_LRCK_RATIO >> 8));
    es8389_write_reg(i2c, addr, ES8389_LRCK_DIVL_REG0D, (ES8389_MCLK_LRCK_RATIO & 0xFF));
    es8389_write_reg(i2c, addr, ES8389_OSC_CLK_REG0F, 0x10);  /* OSC_CLK: enable PLL */

    /* ADC/DAC initial settings */
    es8389_write_reg(i2c, addr, ES8389_ADC_REG21, 0x1F);
    es8389_write_reg(i2c, addr, ES8389_ADC_REG22, 0x7F);
    es8389_write_reg(i2c, addr, ES8389_ADC_MIX_REG31, 0x00);
    es8389_write_reg(i2c, addr, ES8389_DAC_MIX_REG44, 0x00);
    es8389_write_reg(i2c, addr, ES8389_DAC_REG41, 0x7F);
    es8389_write_reg(i2c, addr, ES8389_DAC_REG42, 0x7F);
    es8389_write_reg(i2c, addr, ES8389_DAC_REG43, 0x10);
    es8389_write_reg(i2c, addr, ES8389_DAC_RAMP_REG49, 0x0F);
    es8389_write_reg(i2c, addr, ES8389_SYSTEM_REG4C, 0xC0);
    es8389_write_reg(i2c, addr, ES8389_DAC_RESET_REG4D, 0x02);

    /* Power sequence before BIAS_ON */
    es8389_write_reg(i2c, addr, ES8389_RESET_REG00, 0x00);  /* Chip reset */
    es8389_write_reg(i2c, addr, ES8389_CLK_OFF_REG03, 0xC1);  /* Clock control */
    es8389_write_reg(i2c, addr, ES8389_RESET_REG00, 0x01);  /* Exit reset */

    /* Configure DMIC/ADC volume */
    es8389_write_reg(i2c, addr, ES8389_DMIC_VOL_REG26, 0xBF); /* 0dB */
    es8389_write_reg(i2c, addr, ES8389_ADCL_VOL_REG27, 0xBF);  /* 0dB */
    es8389_write_reg(i2c, addr, ES8389_ADCR_VOL_REG28, 0xBF);  /* 0dB */
    es8389_write_reg(i2c, addr, ES8389_DACL_VOL_REG46, 0xBF);  /* 0dB */
    es8389_write_reg(i2c, addr, ES8389_DACR_VOL_REG47, 0xBF);  /* 0dB */
    es8389_write_reg(i2c, addr, ES8389_MIX_VOL_REG48, 0xBE);   /* 0dB */

    /* Configure I2S format and bit width */
    state = 0;
    state |= map_width(cfg->bits);      /* Data length: bits [7:5] */
    state |= map_fmt(cfg->i2s_fmt);     /* I2S format: bits [4:2] */

    /* Apply format configuration to ADC and DAC */
    es8389_update_bits(i2c, addr, ES8389_ADC_REG20,
                      ES8389_DAIFMT_MASK | ES8389_DATA_LEN_MASK, state);
    es8389_update_bits(i2c, addr, ES8389_DAC_REG40,
                      ES8389_DAIFMT_MASK | ES8389_DATA_LEN_MASK, state);

    /* Configure master/slave mode */
    es8389_update_bits(i2c, addr, ES8389_ADC_REG20, 0x03,
                      cfg->codec_master ? 0x03 : 0x00);
    es8389_update_bits(i2c, addr, ES8389_DAC_REG40, 0x03,
                      cfg->codec_master ? 0x03 : 0x00);

    /*
     * BIAS_ON state - Critical steps to enable ADC/DAC
     * Reference: Linux driver es8389.c SND_SOC_BIAS_ON
     */
    es8389_write_reg(i2c, addr, ES8389_DAC_RESET_REG4D, 0x02);
    es8389_update_bits(i2c, addr, ES8389_HPSW_REG69, 0x20, 0x20);
    es8389_write_reg(i2c, addr, ES8389_ANA_CTL1_REG61, 0xD9);   /* Enable analog power */
    es8389_write_reg(i2c, addr, ES8389_ADC_EN_REG64, 0x8F);    /* Enable ADC */
    es8389_write_reg(i2c, addr, ES8389_CSM_JUMP_REG10, 0xE4);  /* State machine jump */
    es8389_write_reg(i2c, addr, ES8389_RESET_REG00, 0x01);
    es8389_write_reg(i2c, addr, ES8389_CLK_OFF_REG03, 0xC3);   /* Clock configuration */
    es8389_write_reg(i2c, addr, ES8389_ADC_HPF1_REG24, 0x00);  /* HPF COEF1 - clear HPF */
    es8389_write_reg(i2c, addr, ES8389_ADC_HPF2_REG25, 0x00);  /* HPF COEF2 - clear HPF */

    /* Enable ADC/DAC based on configuration */
    if (cfg->enable_adc) {
        es8389_update_bits(i2c, addr, ES8389_ADC_REG20, 0x03, 0x00); /* Unmute ADC */
    } else {
        es8389_update_bits(i2c, addr, ES8389_ADC_REG20, 0x03, 0x03); /* Mute ADC */
    }

    if (cfg->enable_dac) {
        es8389_update_bits(i2c, addr, ES8389_DAC_REG40, 0x03, 0x00); /* Unmute DAC */
    } else {
        es8389_update_bits(i2c, addr, ES8389_DAC_REG40, 0x03, 0x03); /* Mute DAC */
    }

    bflb_mtimer_delay_ms(70);
    es8389_write_reg(i2c, addr, ES8389_DAC_RESET_REG4D, 0x00);  /* Release DAC reset */

    return 0;
}

int audio_codec_es8389_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || dev->i2c == NULL || cfg == NULL) {
        return -1;
    }
    if (dev->i2c_addr == 0) {
        dev->i2c_addr = ES8389_I2C_DEFAULT_ADDR;
    }

    return es8389_codec_init(dev, cfg);
}

int audio_codec_es8389_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    /* ES8389 DAC volume: 0x00 = -95.5dB, 0xFF = 0dB, 0.5dB/step
     * 191 = 0dB reference
     * Map 0-100% to 0-255 range (linear mapping for simplicity)
     */
    uint8_t volume = (uint8_t)((volume_percent * 255 + 50) / 100);
    int res = 0;
    res |= es8389_write_reg(dev->i2c, dev->i2c_addr, ES8389_DACL_VOL_REG46, volume);
    res |= es8389_write_reg(dev->i2c, dev->i2c_addr, ES8389_DACR_VOL_REG47, volume);
    return res;
}

static int audio_codec_es8389_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    /* ES8389 ADC volume: 0x00 = -95.5dB, 0xFF = +30dB, 0.5dB/step
     * 191 = 0dB reference
     */
    uint8_t volume = (uint8_t)((volume_percent * 255 + 50) / 100);
    int res = 0;
    res |= es8389_write_reg(dev->i2c, dev->i2c_addr, ES8389_ADCL_VOL_REG27, volume);
    res |= es8389_write_reg(dev->i2c, dev->i2c_addr, ES8389_ADCR_VOL_REG28, volume);
    return res;
}

static int audio_codec_es8389_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (pga_percent < 0) {
        pga_percent = 0;
    } else if (pga_percent > 100) {
        pga_percent = 100;
    }

    /* ES8389 MIC PGA: 0x00 = 0dB, 0x0E = +42dB, 3dB/step
     * Map 0-100% to 0-14 range
     */
    uint8_t gain = (uint8_t)((pga_percent * 14 + 50) / 100);

    /* Read current register values to preserve input selection */
    uint8_t val1, val2;
    es8389_read_reg(dev->i2c, dev->i2c_addr, ES8389_MIC1_GAIN_REG72, &val1);
    es8389_read_reg(dev->i2c, dev->i2c_addr, ES8389_MIC2_GAIN_REG73, &val2);

    int res = 0;
    res |= es8389_write_reg(dev->i2c, dev->i2c_addr, ES8389_MIC1_GAIN_REG72, (val1 & 0xF0) | gain);
    res |= es8389_write_reg(dev->i2c, dev->i2c_addr, ES8389_MIC2_GAIN_REG73, (val2 & 0xF0) | gain);
    return res;
}

static int audio_codec_es8389_set_mute(audio_codec_dev_t *dev, bool mute)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    /* Only mute DAC, not ADC. ADC should remain enabled for recording.
     * This avoids I2C read-modify-write issues on ADC_REG20 during I2S operation. */
    uint8_t dac_val = mute ? 0x03 : 0x00;
    int res = 0;

    res |= es8389_update_bits(dev->i2c, dev->i2c_addr, ES8389_DAC_REG40, 0x03, dac_val);
    return res;
}

int audio_codec_es8389_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    (void)dev;
    (void)sample_rate_hz;
    /* In slave mode the codec tracks LRCK; nothing to do.
     * For master mode, additional clock configuration would be needed.
     */
    return 0;
}

uint32_t audio_codec_es8389_calc_mclk_hz(uint32_t sample_rate_hz)
{
    /* ES8389 typical MCLK = sample_rate * 256 */
    return sample_rate_hz * ES8389_MCLK_LRCK_RATIO;
}

static const uint8_t es8389_addrs[] = { ES8389_I2C_DEFAULT_ADDR };

const audio_codec_driver_t audio_codec_es8389_driver = {
    .type = AUDIO_CODEC_TYPE_ES8389,
    .name = "ES8389",
    .i2c_addrs = es8389_addrs,
    .i2c_addrs_num = sizeof(es8389_addrs) / sizeof(es8389_addrs[0]),
    .probe = audio_codec_es8389_probe,
    .init = audio_codec_es8389_init,
    .set_mute = audio_codec_es8389_set_mute,
    .set_volume = audio_codec_es8389_set_volume,
    .set_record_volume = audio_codec_es8389_set_record_volume,
    .set_mic_pga = audio_codec_es8389_set_mic_pga,
    .set_sample_rate = audio_codec_es8389_set_sample_rate,
    .calc_mclk_hz = audio_codec_es8389_calc_mclk_hz,
};
