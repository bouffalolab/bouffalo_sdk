/**
 * @file audio_codec_jy6166.c
 * @brief JY6166 stereo ADC helper driver.
 */

#include "audio_codec_jy6166.h"

#include <stddef.h>
#include <string.h>

#include "bflb_mtimer.h"

#include "audio_codec_internal.h"

#define JY6166_CHIP_ID_VALUE (0x27)
#define JY6166_REG_SRST      (0x00)
#define JY6166_REG_PWR_CTRL  (0x01)
#define JY6166_REG_SYSCLK    (0x0a)
#define JY6166_REG_MODCLK_EN (0x0b)
#define JY6166_REG_ADC_FS    (0x0c)
#define JY6166_REG_ADC_CLK   (0x0d)
#define JY6166_REG_I2S_CTRL  (0x10)
#define JY6166_REG_I2S_FMT3  (0x13)
#define JY6166_REG_TX_CONF1  (0x14)
#define JY6166_REG_TX_CONF2  (0x15)
#define JY6166_REG_I2S_PAD   (0x18)
#define JY6166_REG_ADC_DIG   (0x20)
#define JY6166_REG_ADC_HPF   (0x21)
#define JY6166_REG_ADC0_DVOL (0x22)
#define JY6166_REG_ADC1_DVOL (0x23)
#define JY6166_REG_ADC0_ANA  (0x30)
#define JY6166_REG_ADC1_ANA  (0x31)
#define JY6166_REG_TUNE1     (0x36)
#define JY6166_REG_TUNE2     (0x37)
#define JY6166_REG_CHIP_ID   (0xff)

#define JY6166_I2S_FMT_I2S   (0x00)
#define JY6166_I2S_FMT_LEFTJ (0x10)
#define JY6166_I2S_FMT_DSPA  (0x20)
#define JY6166_I2S_WL_16BIT  (0x00)

#define JY6166_SYSCLK_EN           (0x01)
#define JY6166_SYSCLK_SRC_BCLK     (0x02)
#define JY6166_SYSCLK_SRC_MCLK     (0x00)
#define JY6166_MODCLK_ADC_ALL_EN   (0x0f)
#define JY6166_MODCLK_ADC_SYNC_DIS (0x10)
#define JY6166_ADC_DIG_DITHER_EN   (0x04)
#define JY6166_ADC_DIG_ALL_EN      (0x03)
#define JY6166_ADC_HPF_70HZ        (0x0c)
#define JY6166_ADC_HPF_ALL_EN      (0x03)
#define JY6166_ANA_PGA_HPF_12K5    (0x0c)
#define JY6166_ANA_SDM_PGA_EN      (0x03)
#define JY6166_TX_HIZ              (0x03)

#define JY6166_OSR_32_REG (0U)
#define JY6166_OSR_64_REG (1U)

typedef struct {
    struct bflb_device_s *i2c;
    audio_codec_cfg_t cfg;
    uint8_t i2c_addr;
    int record_volume_percent;
    int mic_pga_percent;
    bool initialized;
} audio_codec_jy6166_ctx_t;

static audio_codec_jy6166_ctx_t g_jy6166_ctx;

static int jy6166_write(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t val)
{
    return audio_codec_i2c_write_reg8(i2c, addr, reg, val);
}

static int jy6166_read(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t *val)
{
    return audio_codec_i2c_read_reg8(i2c, addr, reg, val);
}

static int jy6166_update_bits(struct bflb_device_s *i2c, uint8_t addr, uint8_t reg, uint8_t mask, uint8_t val)
{
    uint8_t val_old;
    uint8_t val_new;

    if (jy6166_read(i2c, addr, reg, &val_old) != 0) {
        return -1;
    }

    val_new = (uint8_t)((val_old & (uint8_t)~mask) | (val & mask));
    if (val_new == val_old) {
        return 0;
    }

    return jy6166_write(i2c, addr, reg, val_new);
}

static uint8_t jy6166_percent_to_dig_vol(int volume_percent)
{
    if (volume_percent <= 0) {
        return 0x00;
    }
    if (volume_percent >= 100) {
        return 0xbf;
    }

    return (uint8_t)(((volume_percent * 0xbf) + 50) / 100);
}

static uint8_t jy6166_percent_to_pga_gain(int pga_percent)
{
    uint8_t gain;

    if (pga_percent <= 0) {
        gain = 0U;
    } else if (pga_percent >= 100) {
        gain = 14U;
    } else {
        gain = (uint8_t)(((pga_percent * 14) + 50) / 100);
    }

    if (gain == 1U) {
        gain = 2U;
    }

    return gain;
}

static int jy6166_sample_rate_to_reg(uint32_t sample_rate_hz, uint8_t *reg_val)
{
    if (reg_val == NULL) {
        return -1;
    }

    switch (sample_rate_hz) {
        case 8000:
            *reg_val = 0;
            return 0;
        case 11025:
        case 12000:
            *reg_val = 1;
            return 0;
        case 16000:
            *reg_val = 2;
            return 0;
        case 22050:
        case 24000:
            *reg_val = 3;
            return 0;
        case 32000:
            *reg_val = 4;
            return 0;
        case 44100:
        case 48000:
            *reg_val = 5;
            return 0;
        case 88200:
        case 96000:
            *reg_val = 6;
            return 0;
        case 176400:
        case 192000:
            *reg_val = 7;
            return 0;
        default:
            return -1;
    }
}

static int jy6166_i2s_fmt3(audio_codec_i2s_fmt_t fmt, audio_codec_bits_t bits, bool tx_hiz, uint8_t *reg_val)
{
    uint8_t fmt_bits;
    uint8_t wl_bits;

    if (reg_val == NULL) {
        return -1;
    }

    switch (fmt) {
        case AUDIO_CODEC_I2S_FMT_LEFT_J:
            fmt_bits = JY6166_I2S_FMT_LEFTJ;
            break;
        case AUDIO_CODEC_I2S_FMT_DSP:
            fmt_bits = JY6166_I2S_FMT_DSPA;
            break;
        case AUDIO_CODEC_I2S_FMT_I2S:
            fmt_bits = JY6166_I2S_FMT_I2S;
            break;
        default:
            return -1;
    }

    switch (bits) {
        case AUDIO_CODEC_BITS_24:
            wl_bits = 0x08;
            break;
        case AUDIO_CODEC_BITS_32:
            wl_bits = 0x0c;
            break;
        case AUDIO_CODEC_BITS_16:
        default:
            wl_bits = JY6166_I2S_WL_16BIT;
            break;
    }

    *reg_val = (uint8_t)(fmt_bits | wl_bits | (tx_hiz ? JY6166_TX_HIZ : 0U));
    return 0;
}

static int jy6166_divider_to_reg(uint8_t div, uint8_t *reg_val)
{
    static const uint8_t div_tbl[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24 };
    uint8_t i;

    if (reg_val == NULL) {
        return -1;
    }

    for (i = 0; i < (uint8_t)(sizeof(div_tbl) / sizeof(div_tbl[0])); i++) {
        if (div_tbl[i] == div) {
            *reg_val = i;
            return 0;
        }
    }

    return -1;
}

static uint8_t jy6166_sample_rate_to_osr(uint32_t sample_rate_hz)
{
    switch (sample_rate_hz) {
        case 176400:
        case 192000:
            return JY6166_OSR_32_REG;
        default:
            return JY6166_OSR_64_REG;
    }
}

static uint8_t jy6166_osr_to_ratio(uint8_t osr_reg)
{
    return (osr_reg == JY6166_OSR_32_REG) ? 32U : 64U;
}

static uint8_t jy6166_cfg_physical_slots(const audio_codec_cfg_t *cfg, const audio_codec_jy6166_tdm_cfg_t *tdm_cfg)
{
    if (tdm_cfg != NULL && tdm_cfg->slot_count > 0U) {
        return (uint8_t)(tdm_cfg->slot_count * 2U);
    }
    if (cfg != NULL && cfg->channels > 2U) {
        return cfg->channels;
    }

    return 2U;
}

static int jy6166_clock_regs_from_cfg(const audio_codec_cfg_t *cfg, const audio_codec_jy6166_tdm_cfg_t *tdm_cfg,
                                      uint8_t *sysclk_reg, uint8_t *adc_clk_reg)
{
    uint8_t osr_reg;
    uint8_t osr_ratio;
    uint8_t div_reg;
    uint32_t bclk_lrck_ratio;
    uint8_t physical_slots;

    if (cfg == NULL || sysclk_reg == NULL || adc_clk_reg == NULL || cfg->codec_master) {
        return -1;
    }

    osr_reg = jy6166_sample_rate_to_osr(cfg->sample_rate_hz);
    osr_ratio = jy6166_osr_to_ratio(osr_reg);
    physical_slots = jy6166_cfg_physical_slots(cfg, tdm_cfg);
    bclk_lrck_ratio = (uint32_t)physical_slots * (uint32_t)cfg->bits;

    if (bclk_lrck_ratio >= osr_ratio && (bclk_lrck_ratio % osr_ratio) == 0U &&
        jy6166_divider_to_reg((uint8_t)(bclk_lrck_ratio / osr_ratio), &div_reg) == 0) {
        *sysclk_reg = JY6166_SYSCLK_SRC_BCLK | JY6166_SYSCLK_EN;
        *adc_clk_reg = (uint8_t)((osr_reg << 4) | div_reg);
        return 0;
    }

    /* Fallback for boards wiring MCLK at 256 * Fs, matching vendor base examples. */
    if (jy6166_divider_to_reg((uint8_t)(256U / osr_ratio), &div_reg) == 0) {
        *sysclk_reg = JY6166_SYSCLK_SRC_MCLK | JY6166_SYSCLK_EN;
        *adc_clk_reg = (uint8_t)((osr_reg << 4) | div_reg);
        return 0;
    }

    return -1;
}

static int jy6166_write_tdm_cfg(struct bflb_device_s *i2c, uint8_t addr, const audio_codec_jy6166_tdm_cfg_t *tdm_cfg)
{
    uint8_t slot_count = 1U;
    uint8_t slot_index = 1U;
    uint8_t tx_conf1;
    uint8_t tx_conf2;

    if (tdm_cfg != NULL) {
        slot_count = tdm_cfg->slot_count;
        slot_index = tdm_cfg->slot_index;
    }
    if (slot_count == 0U || slot_count > 8U || slot_index == 0U || slot_index > slot_count) {
        return -1;
    }

    tx_conf1 = (uint8_t)(0x20 | (tdm_cfg != NULL && tdm_cfg->multi_speed ? 0x08 : 0x00) | (slot_count - 1U));
    tx_conf2 = (uint8_t)(slot_index - 1U);

    if (jy6166_write(i2c, addr, JY6166_REG_TX_CONF1, tx_conf1) != 0) {
        return -1;
    }
    if (jy6166_write(i2c, addr, JY6166_REG_TX_CONF2, tx_conf2) != 0) {
        return -1;
    }

    return 0;
}

int audio_codec_jy6166_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    uint8_t id = 0;

    if (i2c == NULL) {
        return -1;
    }

    if (jy6166_read(i2c, addr, JY6166_REG_CHIP_ID, &id) != 0 || id != JY6166_CHIP_ID_VALUE) {
        return -1;
    }

    return 0;
}

int audio_codec_jy6166_set_record_volume(struct bflb_device_s *i2c, uint8_t addr, int volume_percent)
{
    uint8_t volume_reg;

    if (i2c == NULL) {
        return -1;
    }

    volume_reg = jy6166_percent_to_dig_vol(volume_percent);
    if (jy6166_write(i2c, addr, JY6166_REG_ADC0_DVOL, volume_reg) != 0) {
        return -1;
    }
    if (jy6166_write(i2c, addr, JY6166_REG_ADC1_DVOL, volume_reg) != 0) {
        return -1;
    }

    return 0;
}

int audio_codec_jy6166_set_mic_pga(struct bflb_device_s *i2c, uint8_t addr, int pga_percent)
{
    uint8_t pga_gain;

    if (i2c == NULL) {
        return -1;
    }

    pga_gain = jy6166_percent_to_pga_gain(pga_percent);
    if (jy6166_update_bits(i2c, addr, JY6166_REG_ADC0_ANA, 0xfc,
                           (uint8_t)((pga_gain << 4) | JY6166_ANA_PGA_HPF_12K5)) != 0) {
        return -1;
    }
    if (jy6166_update_bits(i2c, addr, JY6166_REG_ADC1_ANA, 0xfc,
                           (uint8_t)((pga_gain << 4) | JY6166_ANA_PGA_HPF_12K5)) != 0) {
        return -1;
    }

    return 0;
}

int audio_codec_jy6166_set_sample_rate(struct bflb_device_s *i2c, uint8_t addr, uint32_t sample_rate_hz)
{
    uint8_t fs_reg;

    if (i2c == NULL || jy6166_sample_rate_to_reg(sample_rate_hz, &fs_reg) != 0) {
        return -1;
    }

    return jy6166_write(i2c, addr, JY6166_REG_ADC_FS, fs_reg);
}

int audio_codec_jy6166_init_adc(struct bflb_device_s *i2c, uint8_t addr, const audio_codec_cfg_t *cfg,
                                const audio_codec_jy6166_tdm_cfg_t *tdm_cfg, int record_volume_percent,
                                int mic_pga_percent)
{
    audio_codec_jy6166_tdm_cfg_t effective_tdm_cfg;
    const audio_codec_jy6166_tdm_cfg_t *active_tdm_cfg = tdm_cfg;
    uint8_t fs_reg;
    uint8_t i2s_fmt3_reg;
    uint8_t sysclk_reg;
    uint8_t adc_clk_reg;
    bool tx_hiz;

    if (i2c == NULL || cfg == NULL) {
        return -1;
    }

    if (active_tdm_cfg == NULL && cfg->i2s_fmt == AUDIO_CODEC_I2S_FMT_DSP && cfg->channels > 2U) {
        if ((cfg->channels & 0x01U) != 0U) {
            return -1;
        }
        effective_tdm_cfg.slot_count = (uint8_t)(cfg->channels / 2U);
        effective_tdm_cfg.slot_index = 1U;
        effective_tdm_cfg.multi_speed = false;
        active_tdm_cfg = &effective_tdm_cfg;
    }

    tx_hiz = (active_tdm_cfg != NULL && active_tdm_cfg->slot_count > 1U);
    if (jy6166_sample_rate_to_reg(cfg->sample_rate_hz, &fs_reg) != 0 ||
        jy6166_i2s_fmt3(cfg->i2s_fmt, cfg->bits, tx_hiz, &i2s_fmt3_reg) != 0 ||
        jy6166_clock_regs_from_cfg(cfg, active_tdm_cfg, &sysclk_reg, &adc_clk_reg) != 0) {
        return -1;
    }

    if (jy6166_write(i2c, addr, JY6166_REG_SRST, 0x01) != 0) {
        return -1;
    }
    bflb_mtimer_delay_ms(2);

    if (jy6166_write(i2c, addr, JY6166_REG_I2S_PAD, 0x00) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_SYSCLK, 0x00) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_MODCLK_EN,
                     JY6166_MODCLK_ADC_ALL_EN | JY6166_MODCLK_ADC_SYNC_DIS) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_ADC_HPF, JY6166_ADC_HPF_70HZ) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_TUNE1, 0x66) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_TUNE2, 0x52) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_PWR_CTRL, 0x2d) != 0) {
        return -1;
    }
    bflb_mtimer_delay_ms(10);

    if (jy6166_write(i2c, addr, JY6166_REG_ADC_FS, fs_reg) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_ADC_CLK, adc_clk_reg) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_SYSCLK, sysclk_reg) != 0) {
        return -1;
    }

    if (jy6166_write(i2c, addr, JY6166_REG_I2S_CTRL, 0x00) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_I2S_FMT3, i2s_fmt3_reg) != 0 ||
        jy6166_write_tdm_cfg(i2c, addr, active_tdm_cfg) != 0) {
        return -1;
    }

    if (audio_codec_jy6166_set_record_volume(i2c, addr, record_volume_percent) != 0 ||
        audio_codec_jy6166_set_mic_pga(i2c, addr, mic_pga_percent) != 0) {
        return -1;
    }

    if (jy6166_update_bits(i2c, addr, JY6166_REG_ADC0_ANA, JY6166_ANA_SDM_PGA_EN, JY6166_ANA_SDM_PGA_EN) != 0 ||
        jy6166_update_bits(i2c, addr, JY6166_REG_ADC1_ANA, JY6166_ANA_SDM_PGA_EN, JY6166_ANA_SDM_PGA_EN) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_ADC_HPF, JY6166_ADC_HPF_70HZ | JY6166_ADC_HPF_ALL_EN) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_ADC_DIG, JY6166_ADC_DIG_DITHER_EN | JY6166_ADC_DIG_ALL_EN) != 0 ||
        jy6166_write(i2c, addr, JY6166_REG_I2S_PAD, 0x0f) != 0 ||
        jy6166_update_bits(i2c, addr, JY6166_REG_I2S_CTRL, 0x03, 0x03) != 0) {
        return -1;
    }

    return 0;
}

static int audio_codec_jy6166_driver_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || dev->i2c == NULL || cfg == NULL) {
        return -1;
    }
    if (cfg->enable_dac || !cfg->enable_adc) {
        return -1;
    }

    if (!g_jy6166_ctx.initialized) {
        g_jy6166_ctx.record_volume_percent = 100;
        g_jy6166_ctx.mic_pga_percent = 0;
    }

    if (dev->i2c_addr == 0U) {
        dev->i2c_addr = JY6166_I2C_ADDR_0;
    }

    if (audio_codec_jy6166_init_adc(dev->i2c, dev->i2c_addr, cfg, NULL, g_jy6166_ctx.record_volume_percent,
                                    g_jy6166_ctx.mic_pga_percent) != 0) {
        return -1;
    }

    g_jy6166_ctx.i2c = dev->i2c;
    g_jy6166_ctx.i2c_addr = dev->i2c_addr;
    g_jy6166_ctx.cfg = *cfg;
    g_jy6166_ctx.initialized = true;
    dev->priv = &g_jy6166_ctx;

    return 0;
}

static int audio_codec_jy6166_driver_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    g_jy6166_ctx.record_volume_percent = volume_percent;
    return audio_codec_jy6166_set_record_volume(dev->i2c, dev->i2c_addr, volume_percent);
}

static int audio_codec_jy6166_driver_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    g_jy6166_ctx.mic_pga_percent = pga_percent;
    return audio_codec_jy6166_set_mic_pga(dev->i2c, dev->i2c_addr, pga_percent);
}

static int audio_codec_jy6166_driver_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    audio_codec_cfg_t cfg;

    if (dev == NULL || dev->i2c == NULL || !g_jy6166_ctx.initialized) {
        return -1;
    }

    cfg = g_jy6166_ctx.cfg;
    cfg.sample_rate_hz = sample_rate_hz;
    return audio_codec_jy6166_driver_init(dev, &cfg);
}

static uint32_t audio_codec_jy6166_calc_mclk_hz(uint32_t sample_rate_hz)
{
    return sample_rate_hz * 256U;
}

static const uint8_t jy6166_addrs[] = {
    JY6166_I2C_ADDR_0,
    JY6166_I2C_ADDR_1,
    JY6166_I2C_ADDR_2,
    JY6166_I2C_ADDR_3,
    JY6166_I2C_ADDR_4,
    JY6166_I2C_ADDR_5,
    JY6166_I2C_ADDR_6,
    JY6166_I2C_ADDR_7,
};

const audio_codec_driver_t audio_codec_jy6166_driver = {
    .name = "JY6166",
    .i2c_addrs = jy6166_addrs,
    .probe = audio_codec_jy6166_probe,
    .init = audio_codec_jy6166_driver_init,
    .set_mute = NULL,
    .set_volume = NULL,
    .set_record_volume = audio_codec_jy6166_driver_set_record_volume,
    .set_mic_pga = audio_codec_jy6166_driver_set_mic_pga,
    .set_sample_rate = audio_codec_jy6166_driver_set_sample_rate,
    .calc_mclk_hz = audio_codec_jy6166_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_JY6166,
    .i2c_addrs_num = (uint8_t)(sizeof(jy6166_addrs) / sizeof(jy6166_addrs[0])),
};
