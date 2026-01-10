#include "audio_codec_es8311.h"
#include "audio_codec_internal.h"

#include <stdio.h>

/* ES8311 I2C registers */
#define ES8311_RESET_REG00 0x00
#define ES8311_CLK_MANAGER_REG01 0x01
#define ES8311_CLK_MANAGER_REG02 0x02
#define ES8311_CLK_MANAGER_REG03 0x03
#define ES8311_CLK_MANAGER_REG04 0x04
#define ES8311_CLK_MANAGER_REG05 0x05
#define ES8311_CLK_MANAGER_REG06 0x06
#define ES8311_CLK_MANAGER_REG07 0x07
#define ES8311_CLK_MANAGER_REG08 0x08
#define ES8311_SDPIN_REG09 0x09
#define ES8311_SDPOUT_REG0A 0x0A
#define ES8311_SYSTEM_REG0B 0x0B
#define ES8311_SYSTEM_REG0C 0x0C
#define ES8311_SYSTEM_REG0D 0x0D
#define ES8311_SYSTEM_REG0E 0x0E
#define ES8311_SYSTEM_REG10 0x10
#define ES8311_SYSTEM_REG11 0x11
#define ES8311_SYSTEM_REG12 0x12
#define ES8311_SYSTEM_REG13 0x13
#define ES8311_SYSTEM_REG14 0x14
#define ES8311_ADC_REG15 0x15
#define ES8311_ADC_REG16 0x16
#define ES8311_ADC_REG17 0x17
#define ES8311_ADC_REG1B 0x1B
#define ES8311_ADC_REG1C 0x1C
#define ES8311_DAC_REG31 0x31
#define ES8311_DAC_REG32 0x32
#define ES8311_DAC_REG37 0x37
#define ES8311_GPIO_REG44 0x44
#define ES8311_GP_REG45 0x45

/* ES8311 IDs */
#define ES8311_CHIP_ID1 0xFD
#define ES8311_CHIP_ID2 0xFE
#define ES8311_DEFAULT_ID1 0x83
#define ES8311_DEFAULT_ID2 0x11

#define REG_0D_DEFAULT 0x01

#ifndef BIT
#define BIT(nr) (1UL << (nr))
#endif

#define FROM_MCLK_PIN (0)
#define FROM_SCLK_PIN (1)
#define MCLK_DIV_FRE (256)

typedef uint8_t es_module_t;
#define ES_MODULE_ADC (0x01)
#define ES_MODULE_DAC (0x02)
#define ES_MODULE_ADC_DAC (0x03)
#define ES_MODULE_LINE (0x04)

typedef uint8_t es8311_mode_t;
#define ES8311_MODE_SLAVE (0x00)
#define ES8311_MODE_MASTER (0x01)

typedef uint8_t es8311_format_t;
#define ES8311_I2S_NORMAL (0)
#define ES8311_I2S_LEFT (1)
#define ES8311_I2S_RIGHT (2)
#define ES8311_I2S_DSP (3)

typedef uint8_t es8311_bits_t;
#define ES8311_BIT_LENGTH_16BITS (1)
#define ES8311_BIT_LENGTH_24BITS (2)
#define ES8311_BIT_LENGTH_32BITS (3)

typedef struct {
    es8311_mode_t mode;
    es8311_format_t fmt;
    uint32_t samples_hz;
    es8311_bits_t bits;
} es8311_i2s_cfg_t;

struct coeff_div {
    uint32_t mclk;
    uint32_t rate;
    uint8_t pre_div;
    uint8_t pre_multi;
    uint8_t adc_div;
    uint8_t dac_div;
    uint8_t fs_mode;
    uint8_t lrck_h;
    uint8_t lrck_l;
    uint8_t bclk_div;
    uint8_t adc_osr;
    uint8_t dac_osr;
};

#ifndef AUDIO_CODEC_UNUSED
#define AUDIO_CODEC_UNUSED __attribute__((unused))
#endif

/* Clock coefficient table used by es8311_config_clk().
 *
 * This table encodes one valid clock tree configuration for a given
 * (mclk, sample_rate) pair. In this driver we assume $mclk = rate * 256$
 * (see MCLK_DIV_FRE), then pick a row via get_coeff() and program the
 * ES8311 clock manager registers.
 *
 * Fields mapping (as used in es8311_config_clk()):
 * - pre_div:   master clock pre-divider value written as (pre_div-1) into
 *              ES8311_CLK_MANAGER_REG02[7:5]
 * - pre_multi: pre-multiplier selector encoded into REG02[4:3]
 *              (1->0, 2->1, 4->2, 8->3)
 * - adc_div:   ADC clock divider written as (adc_div-1) into
 *              ES8311_CLK_MANAGER_REG05[7:4]
 * - dac_div:   DAC clock divider written as (dac_div-1) into
 *              ES8311_CLK_MANAGER_REG05[3:0]
 * - fs_mode:   sample-rate mode written into ES8311_CLK_MANAGER_REG03[6]
 * - adc_osr:   ADC OSR (oversampling ratio) written into REG03[5:0]
 * - dac_osr:   DAC OSR (oversampling ratio) written into REG04[5:0]
 * - lrck_h/lrck_l: LRCK divider high/low bytes written into
 *                  ES8311_CLK_MANAGER_REG07/REG08
 * - bclk_div:  BCLK divider written into ES8311_CLK_MANAGER_REG06[4:0]
 *              (driver uses (bclk_div-1) for values < 19, else uses raw)
 */
static const struct coeff_div coeff_div_tbl[] = {
    /* 8k */
    {12288000, 8000 , 0x06, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {18432000, 8000 , 0x03, 0x02, 0x03, 0x03, 0x00, 0x05, 0xff, 0x18, 0x10, 0x20},
    {16384000, 8000 , 0x08, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {8192000 , 8000 , 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 8000 , 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {4096000 , 8000 , 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2048000 , 8000 , 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 8000 , 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1024000 , 8000 , 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 11.025k */
    {11289600, 11025, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {5644800 , 11025, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2822400 , 11025, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1411200 , 11025, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 12k */
    {12288000, 12000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 12000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 12000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 12000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 16k */
    {12288000, 16000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {18432000, 16000, 0x03, 0x02, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x20},
    {16384000, 16000, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {8192000 , 16000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {6144000 , 16000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {4096000 , 16000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {3072000 , 16000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {2048000 , 16000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1536000 , 16000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},
    {1024000 , 16000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x20},

    /* 22.05k */
    {11289600, 22050, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 22050, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 22050, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 22050, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 24k */
    {12288000, 24000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 24000, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 24000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 24000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 24000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 32k */
    {12288000, 32000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 32000, 0x03, 0x04, 0x03, 0x03, 0x00, 0x02, 0xff, 0x0c, 0x10, 0x10},
    {16384000, 32000, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000 , 32000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 32000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {4096000 , 32000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2048000 , 32000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 32000, 0x03, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
    {1024000 , 32000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 44.1k */
    {11289600, 44100, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 44100, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 44100, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 44100, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 48k */
    {12288000, 48000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 48000, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 48000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 48000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 48000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},

    /* 64k (legacy table rows; kept for reference/compatibility) */
    {12288000, 64000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 64000, 0x03, 0x04, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {16384000, 64000, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {8192000 , 64000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 64000, 0x01, 0x04, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {4096000 , 64000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 64000, 0x01, 0x08, 0x03, 0x03, 0x01, 0x01, 0x7f, 0x06, 0x10, 0x10},
    {2048000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0xbf, 0x03, 0x18, 0x18},
    {1024000 , 64000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 88.2k (legacy table rows; kept for reference/compatibility) */
    {11289600, 88200, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {5644800 , 88200, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {2822400 , 88200, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1411200 , 88200, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},

    /* 96k (legacy table rows; kept for reference/compatibility) */
    {12288000, 96000, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {18432000, 96000, 0x03, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {6144000 , 96000, 0x01, 0x04, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {3072000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x00, 0x00, 0xff, 0x04, 0x10, 0x10},
    {1536000 , 96000, 0x01, 0x08, 0x01, 0x01, 0x01, 0x00, 0x7f, 0x02, 0x10, 0x10},
};

static int get_es8311_mclk_src(void)
{
    return FROM_SCLK_PIN;
}

static int get_coeff(uint32_t mclk, uint32_t rate)
{
    for (size_t i = 0; i < (sizeof(coeff_div_tbl) / sizeof(coeff_div_tbl[0])); i++) {
        if (coeff_div_tbl[i].rate == rate && coeff_div_tbl[i].mclk == mclk) {
            return (int)i;
        }
    }
    return -1;
}

static int es8311_write_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t data)
{
    return audio_codec_i2c_write_reg8(i2c, addr, reg, data);
}

static int es8311_read_reg(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *rdata)
{
    return audio_codec_i2c_read_reg8(i2c, addr, reg, rdata);
}

static int AUDIO_CODEC_UNUSED es8311_set_adc_scale(struct bflb_device_s *i2c, uint8_t addr, uint8_t adc_scale)
{
    /* REGISTER 0x16 (ADC):
     * [2:0] ADC_SCALE, other bits are ADC_SYNC/INV/RAMCLR.
     * Only touch ADC_SCALE to avoid changing clock/polarity behavior.
     */
    adc_scale &= 0x07;
    uint8_t regv = 0;
    if (es8311_read_reg(i2c, addr, ES8311_ADC_REG16, &regv) != 0) {
        return -1;
    }
    regv = (uint8_t)((regv & 0xF8) | adc_scale);
    return es8311_write_reg(i2c, addr, ES8311_ADC_REG16, regv);
}

static int es8311_set_pga_gain(struct bflb_device_s *i2c, uint8_t addr, uint8_t pga_gain)
{
    /* REGISTER 0x14 (SYSTEM):
     * - [3:0] PGAGAIN: ADC analog PGA gain (0..10 => 0dB..30dB, 3dB step)
     * - [4]   LINSEL: input select (Mic1p-Mic1n when 1)
     * - [6]   DMIC_ON
     * Preserve all bits except PGAGAIN.
     */
    if (pga_gain > 10) {
        pga_gain = 10;
    }
    uint8_t regv = 0;
    if (es8311_read_reg(i2c, addr, ES8311_SYSTEM_REG14, &regv) != 0) {
        return -1;
    }
    regv = (uint8_t)((regv & 0xF0) | (pga_gain & 0x0F));
    return es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG14, regv);
}

int audio_codec_es8311_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    uint8_t id1 = 0, id2 = 0;
    if (es8311_read_reg(i2c, addr, ES8311_CHIP_ID1, &id1) != 0) {
        return -1;
    }
    if (es8311_read_reg(i2c, addr, ES8311_CHIP_ID2, &id2) != 0) {
        return -1;
    }
    return (id1 == ES8311_DEFAULT_ID1 && id2 == ES8311_DEFAULT_ID2) ? 0 : -2;
}

static int es8311_config_fmt(struct bflb_device_s *i2c, uint8_t addr, es8311_format_t fmt)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;

    ret |= es8311_read_reg(i2c, addr, ES8311_SDPIN_REG09, &dac_iface);
    ret |= es8311_read_reg(i2c, addr, ES8311_SDPOUT_REG0A, &adc_iface);

    switch (fmt) {
        case ES8311_I2S_NORMAL:
            dac_iface &= 0xFC;
            adc_iface &= 0xFC;
            break;
        case ES8311_I2S_LEFT:
        case ES8311_I2S_RIGHT:
            adc_iface &= 0xFC;
            dac_iface &= 0xFC;
            adc_iface |= 0x01;
            dac_iface |= 0x01;
            break;
        case ES8311_I2S_DSP:
            adc_iface &= 0xDC;
            dac_iface &= 0xDC;
            adc_iface |= 0x03;
            dac_iface |= 0x03;
            break;
        default:
            dac_iface &= 0xFC;
            adc_iface &= 0xFC;
            break;
    }

    ret |= es8311_write_reg(i2c, addr, ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(i2c, addr, ES8311_SDPOUT_REG0A, adc_iface);
    return ret;
}

static int es8311_set_bits_per_sample(struct bflb_device_s *i2c, uint8_t addr, es8311_bits_t bits)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;
    ret |= es8311_read_reg(i2c, addr, ES8311_SDPIN_REG09, &dac_iface);
    ret |= es8311_read_reg(i2c, addr, ES8311_SDPOUT_REG0A, &adc_iface);

    adc_iface &= ~(0x1C);
    dac_iface &= ~(0x1C);

    switch (bits) {
        case ES8311_BIT_LENGTH_16BITS:
            dac_iface |= 0x0C;
            adc_iface |= 0x0C;
            break;
        case ES8311_BIT_LENGTH_32BITS:
            dac_iface |= 0x10;
            adc_iface |= 0x10;
            break;
        case ES8311_BIT_LENGTH_24BITS:
        default:
            dac_iface |= 0x0C;
            adc_iface |= 0x0C;
            break;
    }

    ret |= es8311_write_reg(i2c, addr, ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(i2c, addr, ES8311_SDPOUT_REG0A, adc_iface);
    return ret;
}

static int es8311_init_internal(struct bflb_device_s *i2c, uint8_t addr, const es8311_i2s_cfg_t *i2s_cfg)
{
    int ret = 0;
    uint8_t read_data = 0;
    uint8_t datmp = 0;
    int coeff = 0;

    ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG01, 0x10);
    ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG02, 0x00);
    ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG03, 0x10);
    ret |= es8311_write_reg(i2c, addr, ES8311_ADC_REG16, 0x04);
    ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG04, 0x10);
    ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG05, 0x00);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG0B, 0x00);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG0C, 0x20);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG10, 0x13);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG11, 0x7C);
    ret |= es8311_write_reg(i2c, addr, ES8311_RESET_REG00, 0x80);

    ret |= es8311_read_reg(i2c, addr, ES8311_RESET_REG00, &read_data);
    switch (i2s_cfg->mode) {
        case ES8311_MODE_MASTER:
            read_data |= 0x40;
            break;
        case ES8311_MODE_SLAVE:
        default:
            read_data &= 0xBF;
            break;
    }
    ret |= es8311_write_reg(i2c, addr, ES8311_RESET_REG00, read_data);
    ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG01, 0x1F);

    switch (get_es8311_mclk_src()) {
        case FROM_MCLK_PIN:
            ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG01, &read_data);
            read_data &= 0x7F;
            ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG01, read_data);
            break;
        case FROM_SCLK_PIN:
        default:
            ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG01, &read_data);
            read_data |= 0x80;
            ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG01, read_data);
            break;
    }

    uint32_t sample_fre = i2s_cfg->samples_hz;
    uint32_t mclk_fre = sample_fre * MCLK_DIV_FRE;
    coeff = get_coeff(mclk_fre, sample_fre);
    if (coeff < 0) {
        printf("Unable to configure sample rate %luHz with %luHz MCLK\r\n", (unsigned long)sample_fre, (unsigned long)mclk_fre);
    }

    if (coeff >= 0) {
        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG02, &read_data);
        read_data &= 0x07;
        read_data |= (coeff_div_tbl[coeff].pre_div - 1) << 5;

        datmp = 0;
        switch (coeff_div_tbl[coeff].pre_multi) {
            case 1: datmp = 0; break;
            case 2: datmp = 1; break;
            case 4: datmp = 2; break;
            case 8: datmp = 3; break;
            default: break;
        }

        if (get_es8311_mclk_src() == FROM_SCLK_PIN) {
            datmp = 3;
        }

        read_data |= (datmp) << 3;
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG02, read_data);

        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG05, &read_data);
        read_data &= 0x00;
        read_data |= (coeff_div_tbl[coeff].adc_div - 1) << 4;
        read_data |= (coeff_div_tbl[coeff].dac_div - 1) << 0;
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG05, read_data);

        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG03, &read_data);
        read_data &= 0x80;
        read_data |= coeff_div_tbl[coeff].fs_mode << 6;
        read_data |= coeff_div_tbl[coeff].adc_osr << 0;
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG03, read_data);

        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG04, &read_data);
        read_data &= 0x80;
        read_data |= coeff_div_tbl[coeff].dac_osr << 0;
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG04, read_data);

        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG07, &read_data);
        read_data &= 0xC0;
        read_data |= coeff_div_tbl[coeff].lrck_h << 0;
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG07, read_data);

        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG08, &read_data);
        read_data &= 0x00;
        read_data |= coeff_div_tbl[coeff].lrck_l << 0;
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG08, read_data);

        ret |= es8311_read_reg(i2c, addr, ES8311_CLK_MANAGER_REG06, &read_data);
        read_data &= 0xE0;
        if (coeff_div_tbl[coeff].bclk_div < 19) {
            read_data |= (coeff_div_tbl[coeff].bclk_div - 1) << 0;
        } else {
            read_data |= (coeff_div_tbl[coeff].bclk_div) << 0;
        }
        ret |= es8311_write_reg(i2c, addr, ES8311_CLK_MANAGER_REG06, read_data);
    }

    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG13, 0x10);
    ret |= es8311_write_reg(i2c, addr, ES8311_ADC_REG1B, 0x0A);
    ret |= es8311_write_reg(i2c, addr, ES8311_ADC_REG1C, 0x6A);

    ret |= es8311_config_fmt(i2c, addr, i2s_cfg->fmt);
    ret |= es8311_set_bits_per_sample(i2c, addr, i2s_cfg->bits);

    return ret;
}

static int es8311_start(struct bflb_device_s *i2c, uint8_t addr, es_module_t mode)
{
    int ret = 0;
    uint8_t adc_iface = 0, dac_iface = 0;

    ret |= es8311_read_reg(i2c, addr, ES8311_SDPIN_REG09, &dac_iface);
    dac_iface &= 0xBF;
    ret |= es8311_read_reg(i2c, addr, ES8311_SDPOUT_REG0A, &adc_iface);
    adc_iface &= 0xBF;
    adc_iface |= BIT(6);
    dac_iface |= BIT(6);

    if (mode == ES_MODULE_LINE) {
        return -1;
    }
    if (mode == ES_MODULE_ADC || mode == ES_MODULE_ADC_DAC) {
        adc_iface &= ~(BIT(6));
    }
    if (mode == ES_MODULE_DAC || mode == ES_MODULE_ADC_DAC) {
        dac_iface &= ~(BIT(6));
    }

    ret |= es8311_write_reg(i2c, addr, ES8311_SDPIN_REG09, dac_iface);
    ret |= es8311_write_reg(i2c, addr, ES8311_SDPOUT_REG0A, adc_iface);

    ret |= es8311_write_reg(i2c, addr, ES8311_ADC_REG17, 0xBF);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG0E, 0x02);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG12, 0x00);
    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG14, 0x15);

    ret |= es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG0D, REG_0D_DEFAULT);
    ret |= es8311_write_reg(i2c, addr, ES8311_ADC_REG15, 0x40);
    ret |= es8311_write_reg(i2c, addr, ES8311_DAC_REG37, 0x48);
    ret |= es8311_write_reg(i2c, addr, ES8311_GP_REG45, 0x00);
    ret |= es8311_write_reg(i2c, addr, ES8311_GPIO_REG44, 0x50);
    return ret;
}

static es8311_format_t map_i2s_fmt(audio_codec_i2s_fmt_t fmt)
{
    switch (fmt) {
        case AUDIO_CODEC_I2S_FMT_I2S: return ES8311_I2S_NORMAL;
        case AUDIO_CODEC_I2S_FMT_LEFT_J: return ES8311_I2S_LEFT;
        case AUDIO_CODEC_I2S_FMT_RIGHT_J: return ES8311_I2S_RIGHT;
        case AUDIO_CODEC_I2S_FMT_DSP: return ES8311_I2S_DSP;
        default: return ES8311_I2S_NORMAL;
    }
}

static es8311_bits_t map_bits(audio_codec_bits_t bits)
{
    switch (bits) {
        case AUDIO_CODEC_BITS_16: return ES8311_BIT_LENGTH_16BITS;
        case AUDIO_CODEC_BITS_32: return ES8311_BIT_LENGTH_32BITS;
        case AUDIO_CODEC_BITS_24:
        default: return ES8311_BIT_LENGTH_16BITS;
    }
}

int audio_codec_es8311_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || dev->i2c == NULL || cfg == NULL) {
        return -1;
    }

    es8311_i2s_cfg_t i2s_cfg = {
        .mode = cfg->codec_master ? ES8311_MODE_MASTER : ES8311_MODE_SLAVE,
        .fmt = map_i2s_fmt(cfg->i2s_fmt),
        .samples_hz = cfg->sample_rate_hz,
        .bits = map_bits(cfg->bits),
    };

    int ret = es8311_init_internal(dev->i2c, dev->i2c_addr, &i2s_cfg);
    if (ret != 0) {
        return ret;
    }

    /* PGA is configured explicitly via audio_codec_set_mic_pga() after init. */

    es_module_t module = ES_MODULE_DAC;
    if (cfg->enable_adc && cfg->enable_dac) {
        module = ES_MODULE_ADC_DAC;
    } else if (cfg->enable_adc) {
        module = ES_MODULE_ADC;
    } else {
        module = ES_MODULE_DAC;
    }
    return es8311_start(dev->i2c, dev->i2c_addr, module);
}

static void AUDIO_CODEC_UNUSED es8311_legacy_suspend(struct bflb_device_s *i2c, uint8_t addr)
{
    (void)es8311_write_reg(i2c, addr, ES8311_DAC_REG32, 0x00);
    (void)es8311_write_reg(i2c, addr, ES8311_ADC_REG17, 0x00);
    (void)es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG0E, 0xFF);
    (void)es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG12, 0x02);
    (void)es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG14, 0x00);
    (void)es8311_write_reg(i2c, addr, ES8311_SYSTEM_REG0D, 0xFA);
    (void)es8311_write_reg(i2c, addr, ES8311_ADC_REG15, 0x00);
    (void)es8311_write_reg(i2c, addr, ES8311_DAC_REG37, 0x08);
    (void)es8311_write_reg(i2c, addr, ES8311_GP_REG45, 0x01);
}

static int AUDIO_CODEC_UNUSED es8311_legacy_reg_dump(struct bflb_device_s *i2c, uint8_t addr)
{
    for (uint8_t reg = 0; reg < 0x45; reg++) {
        uint8_t tmp = 0;
        if (es8311_read_reg(i2c, addr, reg, &tmp) != 0) {
            return -1;
        }
        printf("Reg[%02u]=0x%02x\r\n", (unsigned)reg, tmp);
    }
    return 0;
}

static int AUDIO_CODEC_UNUSED es8311_legacy_stop(struct bflb_device_s *i2c, uint8_t addr)
{
    es8311_legacy_suspend(i2c, addr);
    return 0;
}

static int AUDIO_CODEC_UNUSED es8311_legacy_set_voice_volume(struct bflb_device_s *i2c, uint8_t addr, int volume)
{
    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }
    uint8_t vol = (uint8_t)((volume * 2550) / 1000);
    return es8311_write_reg(i2c, addr, ES8311_DAC_REG32, vol);
}

static int AUDIO_CODEC_UNUSED es8311_legacy_get_voice_volume(struct bflb_device_s *i2c, uint8_t addr, int *volume)
{
    if (volume == NULL) {
        return -1;
    }
    uint8_t regv = 0;
    if (es8311_read_reg(i2c, addr, ES8311_DAC_REG32, &regv) != 0) {
        return -2;
    }
    *volume = (int)((regv * 100) / 256);
    return 0;
}

static int AUDIO_CODEC_UNUSED es8311_legacy_get_voice_mute(struct bflb_device_s *i2c, uint8_t addr, int *mute)
{
    if (mute == NULL) {
        return -1;
    }
    uint8_t regv = 0;
    if (es8311_read_reg(i2c, addr, ES8311_DAC_REG31, &regv) != 0) {
        return -2;
    }
    *mute = ((regv & 0x20) != 0) ? 1 : 0;
    return 0;
}

static int AUDIO_CODEC_UNUSED es8311_legacy_set_mic_gain_raw(struct bflb_device_s *i2c, uint8_t addr, uint8_t gain)
{
    return es8311_write_reg(i2c, addr, ES8311_ADC_REG16, gain);
}

int audio_codec_es8311_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    uint8_t vol = (uint8_t)((volume_percent * 2550) / 1000);
    return es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_DAC_REG32, vol);
}

static int audio_codec_es8311_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    /* Use ADC_REG17 (ADC volume) for recording volume control.
     * Unify semantics across codecs: 100% => 0dB, 0% => minimum.
     * ES8311 ADC_VOLUME:
     * - 0x00 = -95.5dB
     * - 0xBF = 0dB
     * - 0xC0..0xFF = +0.5..+32dB (not used by record_volume)
     * Note: when ALC is on, ADC_VOLUME may be overridden by MAXGAIN.
     */
    uint8_t vol = (uint8_t)((volume_percent * 0xBF + 50) / 100);
    return es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_ADC_REG17, vol);
}

static int audio_codec_es8311_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (pga_percent < 0) {
        pga_percent = 0;
    } else if (pga_percent > 100) {
        pga_percent = 100;
    }

    /* ES8311 PGAGAIN supports 0..10 steps (0dB..30dB, 3dB/step). */
    uint8_t step = (uint8_t)((pga_percent * 10 + 50) / 100);
    return es8311_set_pga_gain(dev->i2c, dev->i2c_addr, step);
}

static int audio_codec_es8311_set_mute(audio_codec_dev_t *dev, bool mute)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    uint8_t regv = 0;
    if (es8311_read_reg(dev->i2c, dev->i2c_addr, ES8311_DAC_REG31, &regv) != 0) {
        return -2;
    }

    regv &= 0x9f;
    int res = 0;
    if (mute) {
        res |= es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_SYSTEM_REG12, 0x02);
        res |= es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_DAC_REG31, (uint8_t)(regv | 0x60));
        //res |= es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_DAC_REG32, 0x00);
        //res |= es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_DAC_REG37, 0x08);
        return res;
    }else{
        regv &= (~(0x60));
        res |= es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_DAC_REG31, regv);
        res |= es8311_write_reg(dev->i2c, dev->i2c_addr, ES8311_SYSTEM_REG12, 0x00);
        return res;
    }
    return 0;
}

int audio_codec_es8311_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }
    /* Re-run init to update dividers when sample rate changes.
     * In SLAVE mode this is typically not required, but it keeps behavior consistent.
     */
    audio_codec_cfg_t cfg = {
        .sample_rate_hz = sample_rate_hz,
        .channels = 2,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_I2S,
        .bits = AUDIO_CODEC_BITS_16,
        .codec_master = false,
        .enable_adc = false,
        .enable_dac = true,
    };
    return audio_codec_es8311_init(dev, &cfg);
}

uint32_t audio_codec_es8311_calc_mclk_hz(uint32_t sample_rate_hz)
{
    return sample_rate_hz * 256U;
}

static const uint8_t es8311_addrs[] = { 0x18, 0x19 };

const audio_codec_driver_t audio_codec_es8311_driver = {
    .type = AUDIO_CODEC_TYPE_ES8311,
    .name = "ES8311",
    .i2c_addrs = es8311_addrs,
    .i2c_addrs_num = sizeof(es8311_addrs) / sizeof(es8311_addrs[0]),
    .probe = audio_codec_es8311_probe,
    .init = audio_codec_es8311_init,
    .set_mute = audio_codec_es8311_set_mute,
    .set_volume = audio_codec_es8311_set_volume,
    .set_record_volume = audio_codec_es8311_set_record_volume,
    .set_mic_pga = audio_codec_es8311_set_mic_pga,
    .set_sample_rate = audio_codec_es8311_set_sample_rate,
    .calc_mclk_hz = audio_codec_es8311_calc_mclk_hz,
};
