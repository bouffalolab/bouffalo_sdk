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
#include "audio_codec_jy6311.h"

typedef struct {
    audio_codec_dev_t jy6311;
    struct bflb_device_s *i2c;
    audio_codec_cfg_t cfg;
    uint8_t jy6311_addr;
    uint8_t jy6166_addr;
    int record_volume_percent;
    int mic_pga_percent;
    bool dac_muted;
    bool initialized;
} audio_codec_jy6316_ctx_t;

static audio_codec_jy6316_ctx_t g_jy6316_ctx;

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

    memset(&g_jy6316_ctx.jy6311, 0, sizeof(g_jy6316_ctx.jy6311));
    g_jy6316_ctx.i2c = dev->i2c;
    g_jy6316_ctx.jy6311_addr = JY6316_JY6311_I2C_ADDR;
    g_jy6316_ctx.jy6166_addr = JY6316_JY6166_I2C_ADDR;
    if (!g_jy6316_ctx.initialized) {
        g_jy6316_ctx.record_volume_percent = 100;
        g_jy6316_ctx.mic_pga_percent = 71;
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
        if (audio_codec_jy6166_init_adc(dev->i2c, JY6316_JY6166_I2C_ADDR, cfg, &jy6166_tdm_cfg,
                                        g_jy6316_ctx.record_volume_percent, g_jy6316_ctx.mic_pga_percent) != 0) {
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
    if (dev == NULL || dev->i2c == NULL || audio_codec_jy6311_driver.set_mute == NULL) {
        return -1;
    }

    g_jy6316_ctx.dac_muted = mute;
    return audio_codec_jy6311_driver.set_mute(&g_jy6316_ctx.jy6311, mute);
}

static int audio_codec_jy6316_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL || audio_codec_jy6311_driver.set_volume == NULL) {
        return -1;
    }

    return audio_codec_jy6311_driver.set_volume(&g_jy6316_ctx.jy6311, volume_percent);
}

static int audio_codec_jy6316_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    g_jy6316_ctx.record_volume_percent = volume_percent;
    return audio_codec_jy6166_set_record_volume(dev->i2c, JY6316_JY6166_I2C_ADDR, volume_percent);
}

static int audio_codec_jy6316_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    g_jy6316_ctx.mic_pga_percent = pga_percent;
    return audio_codec_jy6166_set_mic_pga(dev->i2c, JY6316_JY6166_I2C_ADDR, pga_percent);
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

static uint32_t audio_codec_jy6316_calc_mclk_hz(uint32_t sample_rate_hz)
{
    (void)sample_rate_hz;
    return bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_I2S, 0);
}

static const uint8_t jy6316_addrs[] = { JY6316_JY6311_I2C_ADDR };

const audio_codec_driver_t audio_codec_jy6316_driver = {
    .name = "JY6316",
    .i2c_addrs = jy6316_addrs,
    .probe = audio_codec_jy6316_probe,
    .init = audio_codec_jy6316_init,
    .set_mute = audio_codec_jy6316_set_mute,
    .set_volume = audio_codec_jy6316_set_volume,
    .set_record_volume = audio_codec_jy6316_set_record_volume,
    .set_mic_pga = audio_codec_jy6316_set_mic_pga,
    .set_sample_rate = audio_codec_jy6316_set_sample_rate,
    .calc_mclk_hz = audio_codec_jy6316_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_JY6316,
    .i2c_addrs_num = (uint8_t)(sizeof(jy6316_addrs) / sizeof(jy6316_addrs[0])),
};
