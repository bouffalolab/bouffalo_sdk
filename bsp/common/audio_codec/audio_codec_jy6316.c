/**
 * @file audio_codec_jy6316.c
 * @brief Board-level JY6316 codec made from JY6311 + JY6166.
 */

#include "audio_codec_jy6316.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "bflb_clock.h"
#include "bflb_core.h"
#include "audio_codec_jy6166.h"
#include "audio_codec_internal.h"
#include "audio_codec_jy6311.h"

typedef struct {
    audio_codec_dev_t jy6311;
    struct bflb_device_s *i2c;
    audio_codec_cfg_t cfg;
    uint8_t jy6311_addr;
    uint8_t jy6166_addr;
    uint8_t record_volume_percent[2];
    uint8_t mic_gain_code[2];
    bool dac_muted;
    bool initialized;
} audio_codec_jy6316_ctx_t;

static audio_codec_jy6316_ctx_t g_jy6316_ctx;

static const float jy6316_gain_steps_db[] = { 0, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42 };

static int audio_codec_jy6316_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    if (i2c == NULL || addr != JY6316_JY6311_I2C_ADDR) {
        return -1;
    }

    if (audio_codec_jy6311_driver.probe == NULL || audio_codec_jy6311_driver.probe(i2c, addr) != 0) {
        return -1;
    }

    if (audio_codec_jy6166_probe(i2c, JY6316_JY6166_I2C_ADDR) != 0) {
        return -1;
    }

    return 0;
}

static int audio_codec_jy6316_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    audio_codec_cfg_t jy6311_cfg;
    const audio_codec_jy6166_tdm_cfg_t jy6166_tdm_cfg = {
        .slot_count = 2,
        .slot_index = 2,
        .multi_speed = false,
    };

    if (dev == NULL || cfg == NULL || dev->i2c == NULL) {
        return -1;
    }

    /* A partial reconfiguration must not be mistaken for the old format. */
    g_jy6316_ctx.cfg.sample_rate_hz = 0;
    memset(&g_jy6316_ctx.jy6311, 0, sizeof(g_jy6316_ctx.jy6311));
    g_jy6316_ctx.i2c = dev->i2c;
    g_jy6316_ctx.jy6311_addr = JY6316_JY6311_I2C_ADDR;
    g_jy6316_ctx.jy6166_addr = JY6316_JY6166_I2C_ADDR;
    if (!g_jy6316_ctx.initialized) {
        memset(g_jy6316_ctx.record_volume_percent, 100, sizeof(g_jy6316_ctx.record_volume_percent));
        memset(g_jy6316_ctx.mic_gain_code, 10, sizeof(g_jy6316_ctx.mic_gain_code));
        g_jy6316_ctx.dac_muted = false;
    }

    g_jy6316_ctx.jy6311.name = "JY6311";
    g_jy6316_ctx.jy6311.i2c = dev->i2c;
    g_jy6316_ctx.jy6311.drv = &audio_codec_jy6311_driver;
    g_jy6316_ctx.jy6311.type = AUDIO_CODEC_TYPE_JY6311;
    g_jy6316_ctx.jy6311.i2c_addr = JY6316_JY6311_I2C_ADDR;

    jy6311_cfg = *cfg;
    jy6311_cfg.enable_adc = cfg->enable_adc;
    jy6311_cfg.enable_dac = cfg->enable_dac;
    jy6311_cfg.i2s_fmt = cfg->i2s_fmt;
    jy6311_cfg.work_mode = AUDIO_CODEC_WORK_MODE_CODEC;
    if (cfg->i2s_fmt == AUDIO_CODEC_I2S_FMT_DSP) {
        jy6311_cfg.channels = 4;
    }

    if (audio_codec_jy6311_driver.init == NULL || audio_codec_jy6311_driver.init(&g_jy6316_ctx.jy6311, &jy6311_cfg) != 0) {
        return -1;
    }

    /* The module has four physical 16-bit TDM slots, including during
     * playback-only operation. Derive the converter clock from that bus:
     * MCLK routing/frequency varies between boards, and the JY6311 vendor
     * initializer assumes a fixed 4.096 MHz MCLK. At 8 kHz that assumption
     * can leave the DAC running at a different rate from the incoming PCM. */
    if (!cfg->codec_master && cfg->i2s_fmt == AUDIO_CODEC_I2S_FMT_DSP) {
        JY6311_SysclkCfgTypeDef clock_cfg = {
            .extclk_freq = cfg->sample_rate_hz * (uint32_t)cfg->bits *
                           (uint32_t)jy6166_tdm_cfg.slot_count * 2U,
            .extclk_src = JY6311_EXT_CLK_SRC_BCLK,
            .play_smp_rate = cfg->enable_dac ? cfg->sample_rate_hz : JY6311_USER_SR_NONE,
            .rec_smp_rate = cfg->enable_adc ? cfg->sample_rate_hz : JY6311_USER_SR_NONE,
            .work_mode = JY6311_ADDA_WORK_MODE_NORMAL,
            .i2s_lrck_period = 0,
        };
        if (jy6311_sysclk_cfg(JY6316_JY6311_I2C_ADDR, &clock_cfg) != JY6311_OK) {
            return -1;
        }
    }

    if (cfg->enable_adc) {
        /* Slot 0 supplies the DAC reference; JY6166 microphones use slots 2/3. */
        if (jy6311_update_bits(JY6316_JY6311_I2C_ADDR, I2S_TXM_CTRL,
                              TX_MIXL_SRC_Msk, TX_MIXL_SRC_DAC_DI_Msk) != 0) {
            return -1;
        }
        if (audio_codec_jy6166_init_adc(dev->i2c, JY6316_JY6166_I2C_ADDR, cfg, &jy6166_tdm_cfg,
                                        g_jy6316_ctx.record_volume_percent[0], 0) != 0 ||
            audio_codec_jy6166_set_mic_channel_volume(dev->i2c, JY6316_JY6166_I2C_ADDR, 0x02,
                                                      g_jy6316_ctx.record_volume_percent[1]) != 0 ||
            audio_codec_jy6166_set_mic_channel_gain_db(dev->i2c, JY6316_JY6166_I2C_ADDR, 0x01,
                                                       g_jy6316_ctx.mic_gain_code[0] * 3.0f) != 0 ||
            audio_codec_jy6166_set_mic_channel_gain_db(dev->i2c, JY6316_JY6166_I2C_ADDR, 0x02,
                                                       g_jy6316_ctx.mic_gain_code[1] * 3.0f) != 0) {
            return -1;
        }
    }

    g_jy6316_ctx.cfg = *cfg;
    g_jy6316_ctx.initialized = true;
    dev->priv = &g_jy6316_ctx;
    dev->i2c_addr = JY6316_JY6311_I2C_ADDR;

    return 0;
}

static int audio_codec_jy6316_set_mute(audio_codec_dev_t *dev, bool mute)
{
    if (dev == NULL || dev->i2c == NULL || audio_codec_jy6311_driver.set_speaker_channel_mute == NULL) {
        return -1;
    }

    int ret = audio_codec_jy6311_driver.set_speaker_channel_mute(&g_jy6316_ctx.jy6311, 0x01, mute);
    if (ret == 0) {
        g_jy6316_ctx.dac_muted = mute;
    }
    return ret;
}

static int audio_codec_jy6316_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL || audio_codec_jy6311_driver.set_speaker_channel_volume == NULL) {
        return -1;
    }

    return audio_codec_jy6311_driver.set_speaker_channel_volume(&g_jy6316_ctx.jy6311, 0x01, volume_percent);
}

static int audio_codec_jy6316_set_mic_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL || slot_mask == 0 || volume_percent < 0 || volume_percent > 100) {
        return -1;
    }
    if ((slot_mask & ~0x0cU) != 0) {
        return -2;
    }

    /* JY6166 ADC0/1 occupy PCM slots 2/3; slot 0 is the DAC reference. */
    for (uint8_t channel = 0; channel < 2; channel++) {
        if ((slot_mask & (1U << (channel + 2))) != 0) {
            if (audio_codec_jy6166_set_mic_channel_volume(dev->i2c, JY6316_JY6166_I2C_ADDR, 1U << channel,
                                                          volume_percent) != 0) {
                return -1;
            }
            g_jy6316_ctx.record_volume_percent[channel] = (uint8_t)volume_percent;
        }
    }

    return 0;
}

static int audio_codec_jy6316_set_mic_channel_gain_db(audio_codec_dev_t *dev, uint16_t slot_mask, float gain_db)
{
    int step = audio_codec_gain_step_index(jy6316_gain_steps_db, 14, gain_db);
    if (dev == NULL || dev->i2c == NULL || slot_mask == 0 || step < 0) {
        return -1;
    }
    if ((slot_mask & ~0x0cU) != 0) {
        return -2;
    }
    for (uint8_t channel = 0; channel < 2; channel++) {
        if ((slot_mask & (1U << (channel + 2))) != 0) {
            if (audio_codec_jy6166_set_mic_channel_gain_db(dev->i2c, JY6316_JY6166_I2C_ADDR, 1U << channel, gain_db) !=
                0) {
                return -1;
            }
            g_jy6316_ctx.mic_gain_code[channel] = step == 0 ? 0 : (uint8_t)(step + 1);
        }
    }
    return 0;
}

static int audio_codec_jy6316_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    audio_codec_cfg_t cfg;

    if (dev == NULL || dev->i2c == NULL || !g_jy6316_ctx.initialized) {
        return -1;
    }

    if (sample_rate_hz == g_jy6316_ctx.cfg.sample_rate_hz) {
        return 0;
    }

    cfg = g_jy6316_ctx.cfg;
    cfg.sample_rate_hz = sample_rate_hz;
    return audio_codec_jy6316_init(dev, &cfg);
}

static int audio_codec_jy6316_set_enable(audio_codec_dev_t *dev, audio_codec_work_mode_t mode, bool enable)
{
    int ret = 0;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }
    if (mode != AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        ret = audio_codec_jy6166_set_adc_enable(dev->i2c, JY6316_JY6166_I2C_ADDR, enable);
    }
    if (mode != AUDIO_CODEC_WORK_MODE_RECORDING) {
        int output_ret = audio_codec_jy6316_set_mute(dev, !enable);
        if (ret == 0) {
            ret = output_ret;
        }
    }
    return ret;
}

static int audio_codec_jy6316_set_reg(audio_codec_dev_t *dev, int reg, int value)
{
    if (dev == NULL || dev->i2c == NULL || reg < 0 || reg > 0x1ff || value < 0 || value > 0xff) {
        return -1;
    }
    if ((reg >> 8) == 1) {
        return audio_codec_jy6166_set_reg(dev->i2c, JY6316_JY6166_I2C_ADDR, reg & 0xff, value);
    }
    return audio_codec_jy6311_driver.set_reg(&g_jy6316_ctx.jy6311, reg, value);
}

static int audio_codec_jy6316_get_reg(audio_codec_dev_t *dev, int reg, int *value)
{
    if (dev == NULL || dev->i2c == NULL || value == NULL || reg < 0 || reg > 0x1ff) {
        return -1;
    }
    if ((reg >> 8) == 1) {
        return audio_codec_jy6166_get_reg(dev->i2c, JY6316_JY6166_I2C_ADDR, reg & 0xff, value);
    }
    return audio_codec_jy6311_driver.get_reg(&g_jy6316_ctx.jy6311, reg, value);
}

static int audio_codec_jy6316_dump_reg(audio_codec_dev_t *dev)
{
    if (dev == NULL || dev->i2c == NULL ||
        audio_codec_jy6311_driver.dump_reg(&g_jy6316_ctx.jy6311) != 0) {
        return -1;
    }
    return audio_codec_jy6166_dump_reg(dev->i2c, JY6316_JY6166_I2C_ADDR);
}

static int audio_codec_jy6316_deinit(audio_codec_dev_t *dev)
{
    int ret;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }
    ret = audio_codec_jy6166_deinit(dev->i2c, JY6316_JY6166_I2C_ADDR);
    if (audio_codec_jy6311_driver.deinit(&g_jy6316_ctx.jy6311) != 0 || ret != 0) {
        return -1;
    }
    g_jy6316_ctx.initialized = false;
    dev->priv = NULL;
    return 0;
}

static uint32_t audio_codec_jy6316_calc_mclk_hz(uint32_t sample_rate_hz)
{
    (void)sample_rate_hz;
    return bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_I2S, 0);
}

static int audio_codec_jy6316_get_mic_channel_caps(audio_codec_dev_t *dev, uint8_t slot,
                                                   audio_codec_channel_caps_t *caps)
{
    (void)dev;
    static const audio_codec_channel_caps_t native = {
        .controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB,
        .independent_controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB,
        .linked_mask = 0x0c,
        .gain_steps_db = jy6316_gain_steps_db,
        .gain_step_count = 14,
    };
    static const audio_codec_channel_caps_t reference = { 0 };
    return audio_codec_native_caps(0x0f, slot, slot < 2 ? &reference : &native, caps);
}

static int audio_codec_jy6316_get_speaker_channel_caps(audio_codec_dev_t *dev, uint8_t slot,
                                                       audio_codec_channel_caps_t *caps)
{
    (void)dev;
    static const audio_codec_channel_caps_t native = {
        .controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE,
        .independent_controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_MUTE,
        .linked_mask = 0x01,
    };
    return audio_codec_native_caps(0x01, slot, &native, caps);
}

static int audio_codec_jy6316_set_speaker_channel_volume(audio_codec_dev_t *dev, uint16_t mask, int percent)
{
    return mask == 0x01 ? audio_codec_jy6316_set_volume(dev, percent) : -2;
}

static int audio_codec_jy6316_set_speaker_channel_mute(audio_codec_dev_t *dev, uint16_t mask, bool mute)
{
    return mask == 0x01 ? audio_codec_jy6316_set_mute(dev, mute) : -2;
}

static const uint8_t jy6316_addrs[] = { JY6316_JY6311_I2C_ADDR };

const audio_codec_driver_t audio_codec_jy6316_driver = {
    .set_enable = audio_codec_jy6316_set_enable,
    .set_reg = audio_codec_jy6316_set_reg,
    .get_reg = audio_codec_jy6316_get_reg,
    .dump_reg = audio_codec_jy6316_dump_reg,
    .deinit = audio_codec_jy6316_deinit,
    .name = "JY6316",
    .i2c_addrs = jy6316_addrs,
    .probe = audio_codec_jy6316_probe,
    .init = audio_codec_jy6316_init,
    .set_sample_rate = audio_codec_jy6316_set_sample_rate,
    .calc_mclk_hz = audio_codec_jy6316_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_JY6316,
    .i2c_addrs_num = (uint8_t)(sizeof(jy6316_addrs) / sizeof(jy6316_addrs[0])),
    .get_mic_channel_caps = audio_codec_jy6316_get_mic_channel_caps,
    .get_speaker_channel_caps = audio_codec_jy6316_get_speaker_channel_caps,
    .set_speaker_channel_volume = audio_codec_jy6316_set_speaker_channel_volume,
    .set_speaker_channel_mute = audio_codec_jy6316_set_speaker_channel_mute,
    .set_mic_channel_gain_db = audio_codec_jy6316_set_mic_channel_gain_db,
    .set_mic_channel_volume = audio_codec_jy6316_set_mic_channel_volume,
};
