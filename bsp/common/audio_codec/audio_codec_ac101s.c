#include "audio_codec_ac101s.h"
#include "audio_codec_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "bflb_mtimer.h"

#define AC101S_I2C_ADDR0                 0x30
#define AC101S_I2C_ADDR1                 0x33

#define AC101S_CHIP_AUDIO_RST            0x00
#define AC101S_PWR_CTRL1                 0x01
#define AC101S_PWR_CTRL2                 0x02
#define AC101S_SYS_FUNC_CTRL             0x03
#define AC101S_ADC_CLK_SET               0x04
#define AC101S_DAC_CLK_SET               0x05
#define AC101S_SYS_CLK_ENA               0x06
#define AC101S_I2S_CTRL                  0x07
#define AC101S_I2S_BCLK_CTRL             0x08
#define AC101S_I2S_LRCK_CTRL1            0x09
#define AC101S_I2S_LRCK_CTRL2            0x0A
#define AC101S_I2S_FMT_CTRL1             0x0B
#define AC101S_I2S_FMT_CTRL2             0x0C
#define AC101S_I2S_FMT_CTRL3             0x0D
#define AC101S_I2S_SLOT_CTRL             0x0E
#define AC101S_I2S_TX_CTRL               0x0F
#define AC101S_I2S_TX_CHMP               0x11
#define AC101S_I2S_TX_MIX_SRC            0x13
#define AC101S_I2S_RX_CHMP               0x16
#define AC101S_I2S_RX_MIX_SRC            0x18
#define AC101S_ADC_DIG_CTRL              0x19
#define AC101S_ADC_DVC                   0x1A
#define AC101S_DAC_DIG_CTRL              0x1B
#define AC101S_DAC_DVC                   0x1C
#define AC101S_DAC_MIX_SRC               0x1D
#define AC101S_RESERVED_1E               0x1E
#define AC101S_DIG_PADDRV_CTRL           0x1F
#define AC101S_ADC_ANA_CTRL1             0x20
#define AC101S_ADC_ANA_CTRL2             0x21
#define AC101S_ADC_ANA_CTRL3             0x22
#define AC101S_ADC_ANA_CTRL4             0x23
#define AC101S_ADC_ANA_CTRL5             0x24
#define AC101S_DAC_ANA_CTRL1             0x25
#define AC101S_DAC_ANA_CTRL2             0x26
#define AC101S_DAC_ANA_CTRL3             0x27
#define AC101S_DAC_ANA_CTRL4             0x28
#define AC101S_AGC_CTRL                  0x31
#define AC101S_EQ_CTRL                   0x4F

#define AC101S_CHIP_ID                   0x12

#define AC101S_BIT(n)                    (1U << (n))

#define AC101S_SYS_CLK_ADC_DIG_EN        AC101S_BIT(0)
#define AC101S_SYS_CLK_AGC_HPF_EN        AC101S_BIT(1)
#define AC101S_SYS_CLK_DAC_DIG_EN        AC101S_BIT(2)
#define AC101S_SYS_CLK_EQ_EN             AC101S_BIT(3)
#define AC101S_SYS_CLK_I2S_EN            AC101S_BIT(4)
#define AC101S_SYS_CLK_SYSCLK_EN         AC101S_BIT(5)

#define AC101S_I2S_CTRL_BCLK_IOEN        AC101S_BIT(7)
#define AC101S_I2S_CTRL_LRCK_IOEN        AC101S_BIT(6)
#define AC101S_I2S_CTRL_SDO_EN           AC101S_BIT(4)
#define AC101S_I2S_CTRL_TXEN             AC101S_BIT(2)
#define AC101S_I2S_CTRL_RXEN             AC101S_BIT(1)
#define AC101S_I2S_CTRL_GEN              AC101S_BIT(0)

#define AC101S_I2S_BCLK_EDGE_TRANSFER    AC101S_BIT(5)
#define AC101S_I2S_BCLK_POLARITY         AC101S_BIT(4)
#define AC101S_I2S_BCLK_DIV_MASK         0x0FU

#define AC101S_I2S_LRCK_POLARITY         AC101S_BIT(4)

#define AC101S_I2S_FMT1_MODE_SHIFT       4
#define AC101S_I2S_FMT1_OFFSET           AC101S_BIT(2)
#define AC101S_I2S_FMT1_TX_SLOT_HIZ      AC101S_BIT(1)
#define AC101S_I2S_FMT1_TX_STATE         AC101S_BIT(0)

#define AC101S_I2S_FMT2_SLOT_WIDTH_SHIFT 4
#define AC101S_I2S_FMT2_SAMPLE_RES_SHIFT 0

#define AC101S_I2S_FMT3_OUT_MUTE         AC101S_BIT(3)

#define AC101S_I2S_SLOT_RX_CHSEL_SHIFT   2
#define AC101S_I2S_SLOT_TX_CHSEL_SHIFT   0

#define AC101S_ADC_DIG_EN                AC101S_BIT(0)
#define AC101S_ADC_ADOUT_DLY             AC101S_BIT(1)
#define AC101S_ADC_PTN_SHIFT             4
#define AC101S_ADC_DMIC_OSR              AC101S_BIT(6)
#define AC101S_ADC_DMIC_EN               AC101S_BIT(7)

#define AC101S_DAC_DIG_EN                AC101S_BIT(0)
#define AC101S_DAC_PTN_SHIFT             1
#define AC101S_DAC_DVC_ZCD_EN            AC101S_BIT(6)

#define AC101S_ADC_PGA_GEN               AC101S_BIT(0)
#define AC101S_ADC_PGA_GAIN_SHIFT        3

#define AC101S_SYS_DAC_ANA_OUT_EN        AC101S_BIT(0)
#define AC101S_SYS_MCLK_AUTO_DET_EN      AC101S_BIT(1)
#define AC101S_DAC_LINEOUT_GAIN_SHIFT    0
#define AC101S_DAC_LINEOUT_DIF_EN        AC101S_BIT(4)

#define AC101S_AGC_HPF_EN                AC101S_BIT(3)
#define AC101S_AGC_EN                    AC101S_BIT(4)
#define AC101S_AGC_NOISE_DET_EN          AC101S_BIT(2)

#define AC101S_I2S_MODE_PCM              0U
#define AC101S_I2S_MODE_LEFT_J           1U
#define AC101S_I2S_MODE_RIGHT_J          2U

#define AC101S_DVC_0DB                   0x81U
#define AC101S_DEFAULT_PGA_STEP          24U
#define AC101S_DEFAULT_PGA_PERCENT       ((AC101S_DEFAULT_PGA_STEP * 100U + 15U) / 31U)

/* The codec derives ADC_CLK/DAC_CLK (which must be 128*Fs) from SYSCLK, and
 * SYSCLK comes from the external MCLK pin. On bl616cldk the host outputs a
 * fixed 12.288MHz MCLK to the codec (see board_i2s_codec_gpio_init), regardless
 * of Fs. If a board feeds a different MCLK, update this value. */
#define AC101S_MCLK_HZ                   12288000U

typedef struct {
    audio_codec_cfg_t cfg;
    bool initialized;
    int playback_volume_percent;
    int record_volume_percent;
    int mic_pga_percent;
    bool muted;
} audio_codec_ac101s_ctx_t;

static audio_codec_ac101s_ctx_t g_ac101s_ctx;

static int ac101s_write(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    return audio_codec_i2c_write_reg8(i2c, addr, reg, val);
}

static int ac101s_read(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *val)
{
    return audio_codec_i2c_read_reg8(i2c, addr, reg, val);
}

static int ac101s_update_bits(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t mask, uint8_t val)
{
    uint8_t old_val = 0;
    if (ac101s_read(i2c, addr, reg, &old_val) != 0) {
        return -1;
    }

    uint8_t new_val = (uint8_t)((old_val & (uint8_t)(~mask)) | (val & mask));
    if (new_val == old_val) {
        return 0;
    }

    return ac101s_write(i2c, addr, reg, new_val);
}

static int ac101s_percent_clamp(int percent)
{
    if (percent < 0) {
        return 0;
    }
    if (percent > 100) {
        return 100;
    }
    return percent;
}

static uint8_t ac101s_percent_to_dvc(int percent)
{
    percent = ac101s_percent_clamp(percent);
    return (uint8_t)((percent * AC101S_DVC_0DB + 50) / 100);
}

static uint8_t ac101s_percent_to_pga(int percent)
{
    percent = ac101s_percent_clamp(percent);
    return (uint8_t)((percent * 31 + 50) / 100);
}

static int ac101s_map_i2s_format(audio_codec_i2s_fmt_t fmt, uint8_t *mode, uint8_t *offset)
{
    if (mode == NULL || offset == NULL) {
        return -1;
    }

    switch (fmt) {
        case AUDIO_CODEC_I2S_FMT_I2S:
            *mode = AC101S_I2S_MODE_LEFT_J;
            *offset = 1U;
            break;
        case AUDIO_CODEC_I2S_FMT_LEFT_J:
            *mode = AC101S_I2S_MODE_LEFT_J;
            *offset = 0U;
            break;
        case AUDIO_CODEC_I2S_FMT_RIGHT_J:
            *mode = AC101S_I2S_MODE_RIGHT_J;
            *offset = 0U;
            break;
        case AUDIO_CODEC_I2S_FMT_DSP:
            *mode = AC101S_I2S_MODE_PCM;
            *offset = 1U;
            break;
        default:
            return -1;
    }

    return 0;
}

static int ac101s_map_bits(audio_codec_bits_t bits, uint8_t *bits_out)
{
    if (bits_out == NULL) {
        return -1;
    }

    switch (bits) {
        case AUDIO_CODEC_BITS_16:
            *bits_out = 16U;
            break;
        case AUDIO_CODEC_BITS_24:
            *bits_out = 24U;
            break;
        case AUDIO_CODEC_BITS_32:
            *bits_out = 32U;
            break;
        default:
            return -1;
    }

    return 0;
}

static uint8_t ac101s_channel_select(uint8_t channels)
{
    if (channels == 0U) {
        channels = 1U;
    }
    if (channels > 2U) {
        channels = 2U;
    }
    return (uint8_t)(channels - 1U);
}

static int ac101s_slot_width_from_cfg(const audio_codec_cfg_t *cfg, uint8_t sample_bits, uint8_t *slot_width_out)
{
    uint8_t slot_width;

    if (cfg == NULL || slot_width_out == NULL) {
        return -1;
    }

    slot_width = sample_bits;

    if (slot_width < sample_bits) {
        return -1;
    }

    switch (slot_width) {
        case 8U:
        case 12U:
        case 16U:
        case 20U:
        case 24U:
        case 28U:
        case 32U:
            *slot_width_out = slot_width;
            return 0;
        default:
            return -1;
    }
}

static int ac101s_bclk_div_code(uint32_t mclk_hz, uint32_t fs, uint16_t frame_bclks, uint8_t *code)
{
    uint64_t target;
    uint32_t div;

    if (fs == 0U || frame_bclks == 0U || code == NULL) {
        return -1;
    }

    target = (uint64_t)fs * frame_bclks;
    if (target > UINT32_MAX || (mclk_hz % (uint32_t)target) != 0U) {
        return -1;
    }

    div = mclk_hz / (uint32_t)target;
    switch (div) {
        case 1U:
            *code = 1U;
            break;
        case 2U:
            *code = 2U;
            break;
        case 4U:
            *code = 3U;
            break;
        case 6U:
            *code = 4U;
            break;
        case 8U:
            *code = 5U;
            break;
        case 12U:
            *code = 6U;
            break;
        case 16U:
            *code = 7U;
            break;
        case 24U:
            *code = 8U;
            break;
        case 32U:
            *code = 9U;
            break;
        case 48U:
            *code = 10U;
            break;
        case 64U:
            *code = 11U;
            break;
        case 96U:
            *code = 12U;
            break;
        case 128U:
            *code = 13U;
            break;
        case 176U:
            *code = 14U;
            break;
        case 192U:
            *code = 15U;
            break;
        default:
            return -1;
    }

    return 0;
}

static uint8_t ac101s_fmt_ctrl3_from_cfg(const audio_codec_cfg_t *cfg)
{
    (void)cfg;
    /* 0x0D=0x60: TX MSB first, transfer 0 after each sample (sample resolution
     * may be < slot width), LRCK = 1 BCLK width (short frame for PCM), linear
     * PCM data mode. Valid for I2S/LJ/RJ/PCM (LRCK width bit is ignored when the
     * mode itself defines a 50% duty LRCK). Matches the vendor reference. */
    return 0x00;
}

static int ac101s_config_i2s(struct bflb_device_s *i2c, uint8_t addr, const audio_codec_cfg_t *cfg)
{
    uint8_t mode = 0;
    uint8_t offset = 0;
    uint8_t sample_bits = 0;
    uint8_t slot_width = 0;
    uint8_t bclk_div_code = 0;
    uint16_t lrck_period = 0;
    uint16_t frame_bclks = 0;

    if (ac101s_map_i2s_format(cfg->i2s_fmt, &mode, &offset) != 0 || ac101s_map_bits(cfg->bits, &sample_bits) != 0) {
        return -1;
    }

    if (ac101s_slot_width_from_cfg(cfg, sample_bits, &slot_width) != 0) {
        return -1;
    }

    uint8_t slot_width_reg = (uint8_t)((slot_width / 4U) - 1U);
    uint8_t sample_bits_reg = (uint8_t)((sample_bits / 4U) - 1U);
    uint8_t ch_sel = ac101s_channel_select(cfg->channels);

    if (cfg->i2s_fmt == AUDIO_CODEC_I2S_FMT_DSP) {
        lrck_period = (uint16_t)(slot_width * ((cfg->channels > 2U) ? cfg->channels : 2U));
        frame_bclks = lrck_period;
    } else {
        lrck_period = (uint16_t)slot_width;
        frame_bclks = (uint16_t)(2U * slot_width);
    }
    if (lrck_period == 0U) {
        return -1;
    }
    if (cfg->codec_master &&
        ac101s_bclk_div_code(AC101S_MCLK_HZ, cfg->sample_rate_hz, frame_bclks, &bclk_div_code) != 0) {
        return -1;
    }

    int ret = 0;
    ret |=
        ac101s_update_bits(i2c, addr, AC101S_I2S_CTRL, (uint8_t)(AC101S_I2S_CTRL_BCLK_IOEN | AC101S_I2S_CTRL_LRCK_IOEN),
                           cfg->codec_master ? (uint8_t)(AC101S_I2S_CTRL_BCLK_IOEN | AC101S_I2S_CTRL_LRCK_IOEN) : 0U);
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_CTRL, AC101S_I2S_CTRL_SDO_EN, AC101S_I2S_CTRL_SDO_EN);
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_BCLK_CTRL,
                              (uint8_t)(AC101S_I2S_BCLK_EDGE_TRANSFER | AC101S_I2S_BCLK_POLARITY), 0U);
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_BCLK_CTRL, AC101S_I2S_BCLK_DIV_MASK,
                              cfg->codec_master ? bclk_div_code : 0U);
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_LRCK_CTRL1, (uint8_t)(AC101S_I2S_LRCK_POLARITY | 0x03U),
                              (uint8_t)(((lrck_period - 1U) >> 8) & 0x03U));
    ret |= ac101s_write(i2c, addr, AC101S_I2S_LRCK_CTRL2, (uint8_t)((lrck_period - 1U) & 0xffU));
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_FMT_CTRL1,
                              (uint8_t)((0x03U << AC101S_I2S_FMT1_MODE_SHIFT) | AC101S_I2S_FMT1_OFFSET |
                                        AC101S_I2S_FMT1_TX_SLOT_HIZ | AC101S_I2S_FMT1_TX_STATE),
                              (uint8_t)((mode << AC101S_I2S_FMT1_MODE_SHIFT) | (offset ? AC101S_I2S_FMT1_OFFSET : 0U) |
                                        AC101S_I2S_FMT1_TX_STATE));
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_FMT_CTRL2,
                              (uint8_t)((0x07U << AC101S_I2S_FMT2_SLOT_WIDTH_SHIFT) |
                                        (0x07U << AC101S_I2S_FMT2_SAMPLE_RES_SHIFT)),
                              (uint8_t)((slot_width_reg << AC101S_I2S_FMT2_SLOT_WIDTH_SHIFT) |
                                        (sample_bits_reg << AC101S_I2S_FMT2_SAMPLE_RES_SHIFT)));
    ret |= ac101s_write(i2c, addr, AC101S_I2S_FMT_CTRL3, ac101s_fmt_ctrl3_from_cfg(cfg));
    ret |= ac101s_write(i2c, addr, AC101S_I2S_TX_CHMP, 0x0A);
    ret |= ac101s_write(i2c, addr, AC101S_I2S_RX_CHMP, 0xE4);
    ret |= ac101s_update_bits(
        i2c, addr, AC101S_I2S_SLOT_CTRL,
        (uint8_t)((0x03U << AC101S_I2S_SLOT_TX_CHSEL_SHIFT) | (0x03U << AC101S_I2S_SLOT_RX_CHSEL_SHIFT)),
        (uint8_t)((ch_sel << AC101S_I2S_SLOT_TX_CHSEL_SHIFT) | (ch_sel << AC101S_I2S_SLOT_RX_CHSEL_SHIFT)));

    if (ret != 0) {
        return -1;
    }

    return 0;
}

/* Map NADC/NDAC (SYSCLK -> 128*Fs) integer divider to its register code. */
static int ac101s_clk_div_code(uint32_t mclk_hz, uint32_t fs, uint8_t *code)
{
    uint32_t target;
    uint32_t div;

    if (fs == 0U || fs > (UINT32_MAX / 128U) || code == NULL) {
        return -1;
    }

    target = 128U * fs; /* ADC_CLK and DAC_CLK must equal 128*Fs */
    if ((mclk_hz % target) != 0U) {
        return -1;
    }

    div = mclk_hz / target; /* NADC = SYSCLK / (128*Fs) */
    switch (div) {
        case 1U:
            *code = 0U;
            break;
        case 2U:
            *code = 1U;
            break;
        case 3U:
            *code = 2U;
            break;
        case 4U:
            *code = 3U;
            break;
        case 6U:
            *code = 4U;
            break;
        case 8U:
            *code = 5U;
            break;
        case 12U:
            *code = 6U;
            break;
        case 16U:
            *code = 7U;
            break;
        case 24U:
            *code = 8U;
            break;
        default:
            return -1;
    }

    return 0;
}

static int ac101s_validate_cfg(const audio_codec_cfg_t *cfg)
{
    uint8_t mode = 0;
    uint8_t offset = 0;
    uint8_t sample_bits = 0;
    uint8_t slot_width = 0;
    uint8_t divider_code = 0;
    uint16_t frame_bclks;

    if (cfg == NULL || (!cfg->enable_adc && !cfg->enable_dac) || cfg->channels == 0U || cfg->channels > 2U) {
        return -1;
    }
    if (ac101s_map_i2s_format(cfg->i2s_fmt, &mode, &offset) != 0 || ac101s_map_bits(cfg->bits, &sample_bits) != 0 ||
        ac101s_slot_width_from_cfg(cfg, sample_bits, &slot_width) != 0 ||
        ac101s_clk_div_code(AC101S_MCLK_HZ, cfg->sample_rate_hz, &divider_code) != 0) {
        return -1;
    }

    if (cfg->codec_master) {
        frame_bclks = (uint16_t)(slot_width * 2U);
        if (ac101s_bclk_div_code(AC101S_MCLK_HZ, cfg->sample_rate_hz, frame_bclks, &divider_code) != 0) {
            return -1;
        }
    }

    return 0;
}

static int ac101s_config_power_and_clock(struct bflb_device_s *i2c, uint8_t addr, const audio_codec_cfg_t *cfg)
{
    int ret = 0;
    uint8_t clk_code = 0;

    if (ac101s_clk_div_code(AC101S_MCLK_HZ, cfg->sample_rate_hz, &clk_code) != 0) {
        return -1;
    }

    /* 0x01=0x73: ALDOOUT=1.8V, DLDOOUT=1.2V, MICBIAS=2.39V */
    ret |= ac101s_write(i2c, addr, AC101S_PWR_CTRL1, 0x73);
    /* 0x02=0x1F: ALDO/DLDO/MICBIAS/VREF/IREF all enable.
     * MICBIAS (bit2) is required to bias a single-ended/differential analog mic. */
    ret |= ac101s_write(i2c, addr, AC101S_PWR_CTRL2, 0x1F);
    ret |=
        ac101s_update_bits(i2c, addr, AC101S_SYS_CLK_ENA,
                           (uint8_t)(AC101S_SYS_CLK_ADC_DIG_EN | AC101S_SYS_CLK_AGC_HPF_EN | AC101S_SYS_CLK_DAC_DIG_EN |
                                     AC101S_SYS_CLK_EQ_EN | AC101S_SYS_CLK_I2S_EN | AC101S_SYS_CLK_SYSCLK_EN),
                           (uint8_t)(AC101S_SYS_CLK_ADC_DIG_EN | AC101S_SYS_CLK_AGC_HPF_EN | AC101S_SYS_CLK_DAC_DIG_EN |
                                     AC101S_SYS_CLK_EQ_EN | AC101S_SYS_CLK_I2S_EN | AC101S_SYS_CLK_SYSCLK_EN));
    /* SYSCLK comes from the external MCLK pin; ADC_CLK/DAC_CLK must be 128*Fs.
     * Keep MCLK auto-detect off because the host starts BCLK/LRCK after codec
     * initialization. Unsupported MCLK/Fs ratios are rejected above. */
    ret |= ac101s_write(i2c, addr, AC101S_ADC_CLK_SET, clk_code);
    ret |= ac101s_write(i2c, addr, AC101S_DAC_CLK_SET, clk_code);
    ret |= ac101s_update_bits(i2c, addr, AC101S_SYS_FUNC_CTRL, AC101S_SYS_MCLK_AUTO_DET_EN, 0U);

    return (ret == 0) ? 0 : -1;
}

static int ac101s_config_adc(struct bflb_device_s *i2c, uint8_t addr, const audio_codec_cfg_t *cfg)
{
    /* ADC digital enable; ADC out delay disabled (matches vendor reference). */
    uint8_t adc_dig = AC101S_ADC_DIG_EN;
    uint8_t channels = (cfg->channels == 0U) ? 1U : cfg->channels;

    if (cfg->mic_input_mode == AUDIO_CODEC_MIC_INPUT_DIGITAL) {
        adc_dig |= AC101S_ADC_DMIC_EN;
    }
    if (channels > 2U) {
        channels = 2U;
    }

    int ret = 0;
    ret |= ac101s_write(i2c, addr, AC101S_I2S_TX_MIX_SRC, 0x05);

    ret |= ac101s_write(i2c, addr, AC101S_I2S_TX_CTRL, (uint8_t)((1U << channels) - 1U));
    ret |= ac101s_write(i2c, addr, AC101S_ADC_DIG_CTRL, adc_dig);
    ret |= ac101s_write(i2c, addr, AC101S_ADC_DVC, AC101S_DVC_0DB);
    ret |= ac101s_write(i2c, addr, AC101S_DIG_PADDRV_CTRL, 0x15);
    /* PGA gain 24dB ((0x18<<3)) + PGA global enable. ADC_ANA_CTRL2~5 are left at
     * their reset defaults on purpose (the reference does not touch them;
     * forcing 0 can disable the DSM/OTA bias and break the ADC). */
    ret |= ac101s_write(i2c, addr, AC101S_ADC_ANA_CTRL1,
                        (uint8_t)((AC101S_DEFAULT_PGA_STEP << AC101S_ADC_PGA_GAIN_SHIFT) | AC101S_ADC_PGA_GEN));
    /* HPF and AGC both disabled. The HPF is an IIR filter whose coefficient
     * registers (0x42~0x45) are not initialized here; enabling HPF_EN with
     * uninitialized coefficients makes the filter unstable and injects broadband
     * noise (observed as a ~-22dBFS white-noise floor). Leaving HPF off yields a
     * clean capture with only a small DC offset, which is harmless for audio. */
    ret |= ac101s_write(i2c, addr, AC101S_AGC_CTRL, 0x00);
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_CTRL, (uint8_t)(AC101S_I2S_CTRL_TXEN | AC101S_I2S_CTRL_GEN),
                              (uint8_t)(AC101S_I2S_CTRL_TXEN | AC101S_I2S_CTRL_GEN));

    return (ret == 0) ? 0 : -1;
}

static int ac101s_config_dac(struct bflb_device_s *i2c, uint8_t addr)
{
    int ret = 0;

    /* RX Mixer = RXL&RXR (-6dB), DAC Mixer source = RXM. */
    ret |= ac101s_write(i2c, addr, AC101S_I2S_RX_MIX_SRC, 0x0F);
    ret |= ac101s_write(i2c, addr, AC101S_DAC_MIX_SRC, 0x01);
    /* DAC digital part enable (pattern = normal). */
    //ret |= ac101s_write(i2c, addr, AC101S_DAC_DIG_CTRL, AC101S_DAC_DIG_EN);
    ret |= ac101s_write(i2c, addr, AC101S_DAC_DIG_CTRL, 0x61);
    ret |= ac101s_write(i2c, addr, AC101S_DAC_DVC, AC101S_DVC_0DB);
    /* Line-out differential mode enable + gain (0dB). DAC_ANA_CTRL1/3/4 are
     * left at reset defaults: the analog DAC/line-out power-up sequence is run
     * internally once DAC_ANA_OUT_EN is set (DAC_PLAY_FUNC_EN defaults on). */
    ret |= ac101s_write(i2c, addr, AC101S_DAC_ANA_CTRL1, 0xA4);
    ret |= ac101s_write(i2c, addr, AC101S_DAC_ANA_CTRL2, 0x31);
    /* Enable the DAC analog output path (triggers the internal power sequence). */
    ret |= ac101s_update_bits(i2c, addr, AC101S_SYS_FUNC_CTRL, AC101S_SYS_DAC_ANA_OUT_EN, AC101S_SYS_DAC_ANA_OUT_EN);
    ret |= ac101s_update_bits(i2c, addr, AC101S_I2S_CTRL, (uint8_t)(AC101S_I2S_CTRL_RXEN | AC101S_I2S_CTRL_GEN),
                              (uint8_t)(AC101S_I2S_CTRL_RXEN | AC101S_I2S_CTRL_GEN));

    return (ret == 0) ? 0 : -1;
}

static int audio_codec_ac101s_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    uint8_t chip_id = 0;

    if (i2c == NULL) {
        return -1;
    }

    if (ac101s_read(i2c, addr, AC101S_CHIP_AUDIO_RST, &chip_id) != 0) {
        return -1;
    }
    return (chip_id == AC101S_CHIP_ID) ? 0 : -1;
}

int audio_codec_ac101s_dump_registers(struct bflb_device_s *i2c, uint8_t addr)
{
    int result = 0;
    static const uint8_t regs[] = {
        AC101S_CHIP_AUDIO_RST, AC101S_PWR_CTRL1,      AC101S_PWR_CTRL2,       AC101S_SYS_FUNC_CTRL,
        AC101S_ADC_CLK_SET,    AC101S_DAC_CLK_SET,    AC101S_SYS_CLK_ENA,     AC101S_I2S_CTRL,
        AC101S_I2S_BCLK_CTRL,  AC101S_I2S_LRCK_CTRL1, AC101S_I2S_LRCK_CTRL2,  AC101S_I2S_FMT_CTRL1,
        AC101S_I2S_FMT_CTRL2,  AC101S_I2S_FMT_CTRL3,  AC101S_I2S_SLOT_CTRL,   AC101S_I2S_TX_CTRL,
        AC101S_I2S_TX_CHMP,    AC101S_I2S_TX_MIX_SRC, AC101S_I2S_RX_CHMP,     AC101S_I2S_RX_MIX_SRC,
        AC101S_ADC_DIG_CTRL,   AC101S_ADC_DVC,        AC101S_DAC_DIG_CTRL,    AC101S_DAC_DVC,
        AC101S_DAC_MIX_SRC,    AC101S_RESERVED_1E,    AC101S_DIG_PADDRV_CTRL, AC101S_ADC_ANA_CTRL1,
        AC101S_ADC_ANA_CTRL2,  AC101S_ADC_ANA_CTRL3,  AC101S_ADC_ANA_CTRL4,   AC101S_ADC_ANA_CTRL5,
        AC101S_DAC_ANA_CTRL1,  AC101S_DAC_ANA_CTRL2,  AC101S_DAC_ANA_CTRL3,   AC101S_DAC_ANA_CTRL4,
        AC101S_AGC_CTRL,       AC101S_EQ_CTRL,
    };

    if (i2c == NULL) {
        return -1;
    }

    for (size_t i = 0; i < sizeof(regs); i++) {
        uint8_t val = 0U;
        int ret = ac101s_read(i2c, addr, regs[i], &val);
        if (ret == 0) {
            printf("[0x%02x]  0x%02x\r\n", regs[i], val);
        } else {
            printf("[0x%02x]  <read failed: %d>\r\n", regs[i], ret);
            result = -1;
        }
    }

    return result;
}

int audio_codec_ac101s_set_adc_pattern(struct bflb_device_s *i2c, uint8_t addr, uint8_t pattern)
{
    if (i2c == NULL || pattern > 3U) {
        return -1;
    }

    return ac101s_update_bits(i2c, addr, AC101S_ADC_DIG_CTRL, (uint8_t)(0x03U << AC101S_ADC_PTN_SHIFT),
                              (uint8_t)(pattern << AC101S_ADC_PTN_SHIFT));
}

int audio_codec_ac101s_read_register(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *val)
{
    if (i2c == NULL || val == NULL) {
        return -1;
    }

    return ac101s_read(i2c, addr, reg, val);
}

int audio_codec_ac101s_write_register(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    if (i2c == NULL) {
        return -1;
    }

    return ac101s_write(i2c, addr, reg, val);
}

static int audio_codec_ac101s_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    int ret = 0;

    if (dev == NULL || dev->i2c == NULL || ac101s_validate_cfg(cfg) != 0) {
        return -1;
    }

    if (!g_ac101s_ctx.initialized) {
        g_ac101s_ctx.playback_volume_percent = 100;
        g_ac101s_ctx.record_volume_percent = 100;
        g_ac101s_ctx.mic_pga_percent = AC101S_DEFAULT_PGA_PERCENT;
        g_ac101s_ctx.muted = false;
    }

    if (dev->i2c_addr == 0U) {
        dev->i2c_addr = AC101S_I2C_ADDR0;
    }

    if (ac101s_write(dev->i2c, dev->i2c_addr, AC101S_CHIP_AUDIO_RST, 0x34) != 0) {
        printf("ac101s write AUDIO_RST failed\r\n");
        return -1;
    }
    bflb_mtimer_delay_ms(10);

    uint8_t AUDIO_RST_REG_VALUE = 0;
    ac101s_read(dev->i2c, dev->i2c_addr, AC101S_CHIP_AUDIO_RST, &AUDIO_RST_REG_VALUE);

    if (ac101s_config_power_and_clock(dev->i2c, dev->i2c_addr, cfg) != 0 ||
        ac101s_config_i2s(dev->i2c, dev->i2c_addr, cfg) != 0) {
        return -1;
    }

    if (cfg->enable_adc && ac101s_config_adc(dev->i2c, dev->i2c_addr, cfg) != 0) {
        return -1;
    }
    if (cfg->enable_dac && ac101s_config_dac(dev->i2c, dev->i2c_addr) != 0) {
        return -1;
    }

    if (cfg->enable_adc) {
        ret |= ac101s_write(dev->i2c, dev->i2c_addr, AC101S_ADC_DVC,
                            ac101s_percent_to_dvc(g_ac101s_ctx.record_volume_percent));
        ret |= ac101s_update_bits(
            dev->i2c, dev->i2c_addr, AC101S_ADC_ANA_CTRL1, (uint8_t)(0x1FU << AC101S_ADC_PGA_GAIN_SHIFT),
            (uint8_t)(ac101s_percent_to_pga(g_ac101s_ctx.mic_pga_percent) << AC101S_ADC_PGA_GAIN_SHIFT));
    }
    if (cfg->enable_dac) {
        ret |= ac101s_write(dev->i2c, dev->i2c_addr, AC101S_DAC_DVC,
                            ac101s_percent_to_dvc(g_ac101s_ctx.playback_volume_percent));
        ret |= ac101s_update_bits(dev->i2c, dev->i2c_addr, AC101S_I2S_FMT_CTRL3, AC101S_I2S_FMT3_OUT_MUTE,
                                  g_ac101s_ctx.muted ? AC101S_I2S_FMT3_OUT_MUTE : 0U);
    }
    if (ret != 0) {
        return -1;
    }

    g_ac101s_ctx.cfg = *cfg;
    g_ac101s_ctx.initialized = true;
    dev->priv = &g_ac101s_ctx;

    return 0;
}

static int audio_codec_ac101s_set_mute(audio_codec_dev_t *dev, bool mute)
{
    int ret;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    ret = ac101s_update_bits(dev->i2c, dev->i2c_addr, AC101S_I2S_FMT_CTRL3, AC101S_I2S_FMT3_OUT_MUTE,
                             mute ? AC101S_I2S_FMT3_OUT_MUTE : 0U);
    if (ret == 0) {
        g_ac101s_ctx.muted = mute;
    }
    return ret;
}

static int audio_codec_ac101s_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    int ret;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    volume_percent = ac101s_percent_clamp(volume_percent);
    ret = ac101s_write(dev->i2c, dev->i2c_addr, AC101S_DAC_DVC, ac101s_percent_to_dvc(volume_percent));
    if (ret == 0) {
        g_ac101s_ctx.playback_volume_percent = volume_percent;
    }
    return ret;
}

static int audio_codec_ac101s_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    int ret;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    volume_percent = ac101s_percent_clamp(volume_percent);
    ret = ac101s_write(dev->i2c, dev->i2c_addr, AC101S_ADC_DVC, ac101s_percent_to_dvc(volume_percent));
    if (ret == 0) {
        g_ac101s_ctx.record_volume_percent = volume_percent;
    }
    return ret;
}

static int audio_codec_ac101s_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    int ret;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    pga_percent = ac101s_percent_clamp(pga_percent);
    ret =
        ac101s_update_bits(dev->i2c, dev->i2c_addr, AC101S_ADC_ANA_CTRL1, (uint8_t)(0x1FU << AC101S_ADC_PGA_GAIN_SHIFT),
                           (uint8_t)(ac101s_percent_to_pga(pga_percent) << AC101S_ADC_PGA_GAIN_SHIFT));
    if (ret == 0) {
        g_ac101s_ctx.mic_pga_percent = pga_percent;
    }
    return ret;
}

static int audio_codec_ac101s_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    audio_codec_cfg_t cfg;

    if (dev == NULL || dev->i2c == NULL || !g_ac101s_ctx.initialized) {
        return -1;
    }

    cfg = g_ac101s_ctx.cfg;
    cfg.sample_rate_hz = sample_rate_hz;
    return audio_codec_ac101s_init(dev, &cfg);
}

static uint32_t audio_codec_ac101s_calc_mclk_hz(uint32_t sample_rate_hz)
{
    (void)sample_rate_hz;
    /* AC101S needs an external MCLK on its MCLK pin (SYSCLK source). On this
     * board it is a fixed 12.288MHz, independent of Fs. */
    return AC101S_MCLK_HZ;
}

static const uint8_t ac101s_addrs[] = {
    AC101S_I2C_ADDR0,
    AC101S_I2C_ADDR1,
};

const audio_codec_driver_t audio_codec_ac101s_driver = {
    .name = "AC101S",
    .i2c_addrs = ac101s_addrs,
    .probe = audio_codec_ac101s_probe,
    .init = audio_codec_ac101s_init,
    .set_mute = audio_codec_ac101s_set_mute,
    .set_volume = audio_codec_ac101s_set_volume,
    .set_record_volume = audio_codec_ac101s_set_record_volume,
    .set_mic_pga = audio_codec_ac101s_set_mic_pga,
    .set_sample_rate = audio_codec_ac101s_set_sample_rate,
    .calc_mclk_hz = audio_codec_ac101s_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_AC101S,
    .i2c_addrs_num = (uint8_t)(sizeof(ac101s_addrs) / sizeof(ac101s_addrs[0])),
};
