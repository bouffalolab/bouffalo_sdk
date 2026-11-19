/**
 * @file audio_codec_jy6416.c
 * @brief JY6416: JY6311 and two JY6166 ADCs, clocked from six-slot BCLK.
 */

#include "audio_codec_jy6416.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "audio_codec_jy6166.h"
#include "audio_codec_jy6311.h"
#include "audio_codec_internal.h"

static const uint8_t adc_addresses[] = { JY6416_ADC0_ADDR, JY6416_ADC1_ADDR };
#define JY6416_SLOTS 6U
#define ADC_COUNT (sizeof(adc_addresses) / sizeof(adc_addresses[0]))

typedef struct {
    audio_codec_dev_t jy6311;
    audio_codec_cfg_t cfg;
    uint8_t record_volume_percent[4];
    uint8_t mic_gain_code[4];
    int speaker_volume_percent;
    bool speaker_muted;
    bool initialized;
} audio_codec_jy6416_ctx_t;

static audio_codec_jy6416_ctx_t g_jy6416_ctx;

static const float jy6416_gain_steps_db[] = { 0, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42 };

static int audio_codec_jy6416_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    if (i2c == NULL || addr != JY6416_CODEC_ADDR) {
        return -1;
    }

    if (audio_codec_jy6311_driver.probe == NULL || audio_codec_jy6311_driver.probe(i2c, addr) != 0) {
        return -1;
    }

    for (unsigned i = 0; i < ADC_COUNT; ++i) {
        if (audio_codec_jy6166_probe(i2c, adc_addresses[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

static int jy6416_stop_adc(struct bflb_device_s *i2c, uint8_t addr)
{
    int ret = audio_codec_jy6166_deinit(i2c, addr);
    int clock_ret = audio_codec_jy6166_set_reg(i2c, addr, 0x0a, 0x00);
    int pll_ret = audio_codec_jy6166_set_reg(i2c, addr, 0x03, 0x02);
    return ret != 0 || clock_ret != 0 || pll_ret != 0 ? -1 : 0;
}

static int jy6416_update_reference(uint8_t reg, uint8_t mask, uint8_t value)
{
    int old_value;

    if (audio_codec_get_reg(&g_jy6416_ctx.jy6311, reg, &old_value) != 0) {
        return -1;
    }
    int new_value = (old_value & ~mask) | (value & mask);
    return new_value == old_value ? 0 : audio_codec_set_reg(&g_jy6416_ctx.jy6311, reg, new_value);
}

static int audio_codec_jy6416_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    audio_codec_cfg_t active;
    audio_codec_jy6166_tdm_cfg_t jy6166_tdm_cfg = {
        .slot_count = JY6416_SLOTS / 2U,
        .slot_index = 2,
        .multi_speed = false,
        .bclk_pll = true,
    };

    if (dev == NULL || cfg == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (cfg->codec_master || cfg->i2s_fmt != AUDIO_CODEC_I2S_FMT_DSP ||
        cfg->bits != AUDIO_CODEC_BITS_16 || cfg->channels != JY6416_SLOTS ||
        cfg->work_mode > AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        return -1;
    }
    switch (cfg->sample_rate_hz) {
        case 8000:
        case 11025:
        case 16000:
        case 22050:
        case 32000:
        case 44100:
        case 48000:
            break;
        default:
            return -2;
    }
    active = *cfg;
    if (active.work_mode == AUDIO_CODEC_WORK_MODE_RECORDING) {
        active.enable_dac = false;
    } else if (active.work_mode == AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        active.enable_adc = false;
    }
    if (!active.enable_adc && !active.enable_dac) {
        return -1;
    }
    /* A partial reconfiguration must not be mistaken for the old format. */
    g_jy6416_ctx.cfg.sample_rate_hz = 0;
    memset(&g_jy6416_ctx.jy6311, 0, sizeof(g_jy6416_ctx.jy6311));
    if (!g_jy6416_ctx.initialized) {
        memset(g_jy6416_ctx.record_volume_percent, 100, sizeof(g_jy6416_ctx.record_volume_percent));
        memset(g_jy6416_ctx.mic_gain_code, 10, sizeof(g_jy6416_ctx.mic_gain_code));
        g_jy6416_ctx.speaker_volume_percent = 100;
        g_jy6416_ctx.speaker_muted = false;
    }

    g_jy6416_ctx.jy6311.name = "JY6311";
    g_jy6416_ctx.jy6311.i2c = dev->i2c;
    g_jy6416_ctx.jy6311.drv = &audio_codec_jy6311_driver;
    g_jy6416_ctx.jy6311.type = AUDIO_CODEC_TYPE_JY6311;
    g_jy6416_ctx.jy6311.i2c_addr = JY6416_CODEC_ADDR;

    /* The common JY6311 driver derives its BCLK clock plan from all six
     * physical slots. Keep CODEC mode to use the unprocessed reference path. */
    audio_codec_cfg_t jy6311_cfg = active;
    jy6311_cfg.work_mode = AUDIO_CODEC_WORK_MODE_CODEC;
    if (audio_codec_init(&g_jy6416_ctx.jy6311, &jy6311_cfg) != 0) {
        goto fail;
    }

    if (active.enable_dac &&
        (audio_codec_set_speaker_channel_volume(&g_jy6416_ctx.jy6311, 0x01,
                                                g_jy6416_ctx.speaker_volume_percent) != 0 ||
         audio_codec_set_speaker_channel_mute(&g_jy6416_ctx.jy6311, 0x01,
                                              g_jy6416_ctx.speaker_muted) != 0)) {
        goto fail;
    }

    if (active.enable_adc) {
        /* Slot 0 supplies REF; the two ADCs occupy slots 2/3 and 4/5. */
        if (jy6416_update_reference(I2S_TXM_CTRL, TX_MIXL_SRC_Msk, TX_MIXL_SRC_DAC_DI_Msk) != 0) {
            goto fail;
        }
        /* JY6311 transmits REF only in physical slot 0. Release the shared
         * data line during the other five slots and word boundaries. */
        if (jy6416_update_reference(I2S_FMT3, TX_WORD_HIZ_Msk | TX_STATE_Msk,
                                     TX_WORD_HIZ_Msk | TX_STATE_Msk) != 0 ||
            jy6416_update_reference(I2S_TX_CTRL, TX_SLOT_EN_Msk, TX_SLOT0_EN_Msk) != 0) {
            goto fail;
        }
        int fmt3, tx_ctrl, tx_mix;
        if (audio_codec_jy6311_driver.get_reg(&g_jy6416_ctx.jy6311, I2S_FMT3, &fmt3) != 0 ||
            audio_codec_jy6311_driver.get_reg(&g_jy6416_ctx.jy6311, I2S_TX_CTRL, &tx_ctrl) != 0 ||
            audio_codec_jy6311_driver.get_reg(&g_jy6416_ctx.jy6311, I2S_TXM_CTRL, &tx_mix) != 0 ||
            (fmt3 & (TX_WORD_HIZ_Msk | TX_STATE_Msk)) != (TX_WORD_HIZ_Msk | TX_STATE_Msk) ||
            (tx_ctrl & TX_SLOT_EN_Msk) != TX_SLOT0_EN_Msk ||
            (tx_mix & TX_MIXL_SRC_Msk) != TX_MIXL_SRC_DAC_DI_Msk) {
            goto fail;
        }
        for (unsigned i = 0; i < ADC_COUNT; ++i) {
            jy6166_tdm_cfg.slot_index = (uint8_t)(i + 2U);
            if (audio_codec_jy6166_init_adc(dev->i2c, adc_addresses[i], &active, &jy6166_tdm_cfg,
                                          g_jy6416_ctx.record_volume_percent[2U * i], 0) != 0) {
                goto fail;
            }
            for (unsigned channel = 0; channel < 2; ++channel) {
                unsigned mic = 2U * i + channel;
                if (audio_codec_jy6166_set_mic_channel_volume(dev->i2c, adc_addresses[i], 1U << channel,
                                                              g_jy6416_ctx.record_volume_percent[mic]) != 0 ||
                    audio_codec_jy6166_set_mic_channel_gain_db(dev->i2c, adc_addresses[i], 1U << channel,
                                                               g_jy6416_ctx.mic_gain_code[mic] * 3.0f) != 0) {
                    goto fail;
                }
            }
        }
    } else {
        for (unsigned i = 0; i < ADC_COUNT; ++i) {
            if (jy6416_stop_adc(dev->i2c, adc_addresses[i]) != 0) {
                goto fail;
            }
        }
    }

    g_jy6416_ctx.cfg = active;
    g_jy6416_ctx.initialized = true;
    dev->priv = &g_jy6416_ctx;
    dev->i2c_addr = JY6416_CODEC_ADDR;

    return 0;

fail:
    /* Stop all converters after partial configuration, including the ADC
     * whose final transaction failed. Never reuse a cached old format. */
    for (unsigned i = 0; i < ADC_COUNT; ++i) {
        (void)jy6416_stop_adc(dev->i2c, adc_addresses[i]);
    }
    (void)audio_codec_deinit(&g_jy6416_ctx.jy6311);
    dev->priv = NULL;
    return -1;
}

static int audio_codec_jy6416_set_speaker_channel_mute(audio_codec_dev_t *dev, uint16_t slot_mask, bool mute)
{
    if (dev == NULL || dev->i2c == NULL || dev->priv != &g_jy6416_ctx || slot_mask == 0) {
        return -1;
    }
    if (slot_mask != 0x01) {
        return -2;
    }
    int ret = audio_codec_set_speaker_channel_mute(&g_jy6416_ctx.jy6311, 0x01, mute);
    if (ret == 0) {
        g_jy6416_ctx.speaker_muted = mute;
    }
    return ret;
}

static int audio_codec_jy6416_set_speaker_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask,
                                                         int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL || dev->priv != &g_jy6416_ctx || slot_mask == 0 ||
        volume_percent < 0 || volume_percent > 100) {
        return -1;
    }
    if (slot_mask != 0x01) {
        return -2;
    }
    int ret = audio_codec_set_speaker_channel_volume(&g_jy6416_ctx.jy6311, 0x01, volume_percent);
    if (ret == 0) {
        g_jy6416_ctx.speaker_volume_percent = volume_percent;
    }
    return ret;
}

static int audio_codec_jy6416_set_mic_channel_volume(audio_codec_dev_t *dev, uint16_t slot_mask, int volume_percent)
{
    if (dev == NULL || dev->i2c == NULL || dev->priv != &g_jy6416_ctx || slot_mask == 0 ||
        volume_percent < 0 || volume_percent > 100) {
        return -1;
    }
    if ((slot_mask & ~0x3cU) != 0) {
        return -2;
    }
    for (unsigned mic = 0; mic < 4; ++mic) {
        if ((slot_mask & (1U << (mic + 2U))) != 0) {
            if (audio_codec_jy6166_set_mic_channel_volume(dev->i2c, adc_addresses[mic / 2U],
                                                          1U << (mic % 2U), volume_percent) != 0) {
                return -1;
            }
            g_jy6416_ctx.record_volume_percent[mic] = (uint8_t)volume_percent;
        }
    }
    return 0;
}

static int audio_codec_jy6416_set_mic_channel_gain_db(audio_codec_dev_t *dev, uint16_t slot_mask, float gain_db)
{
    int step = audio_codec_gain_step_index(jy6416_gain_steps_db, 14, gain_db);
    if (dev == NULL || dev->i2c == NULL || dev->priv != &g_jy6416_ctx || slot_mask == 0 || step < 0) {
        return -1;
    }
    if ((slot_mask & ~0x3cU) != 0) {
        return -2;
    }
    for (unsigned mic = 0; mic < 4; ++mic) {
        if ((slot_mask & (1U << (mic + 2U))) != 0) {
            if (audio_codec_jy6166_set_mic_channel_gain_db(dev->i2c, adc_addresses[mic / 2U],
                                                           1U << (mic % 2U), gain_db) != 0) {
                return -1;
            }
            g_jy6416_ctx.mic_gain_code[mic] = step == 0 ? 0 : (uint8_t)(step + 1);
        }
    }
    return 0;
}

static int audio_codec_jy6416_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    audio_codec_cfg_t cfg;

    if (dev == NULL || dev->i2c == NULL || !g_jy6416_ctx.initialized) {
        return -1;
    }

    if (sample_rate_hz == 0U) {
        return -1;
    }
    if (sample_rate_hz == g_jy6416_ctx.cfg.sample_rate_hz) {
        return 0;
    }

    cfg = g_jy6416_ctx.cfg;
    cfg.sample_rate_hz = sample_rate_hz;
    return audio_codec_jy6416_init(dev, &cfg);
}

static int audio_codec_jy6416_set_enable(audio_codec_dev_t *dev, audio_codec_work_mode_t mode, bool enable)
{
    int ret = 0;

    if (dev == NULL || dev->i2c == NULL || dev->priv != &g_jy6416_ctx ||
        mode > AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        return -1;
    }
    if (mode != AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        for (unsigned i = 0; i < ADC_COUNT; ++i) {
            if (audio_codec_jy6166_set_adc_enable(dev->i2c, adc_addresses[i], enable) != 0) {
                ret = -1;
            }
        }
    }
    if (mode != AUDIO_CODEC_WORK_MODE_RECORDING) {
        int output_ret = audio_codec_set_speaker_channel_mute(&g_jy6416_ctx.jy6311, 0x01,
                                                               !enable || g_jy6416_ctx.speaker_muted);
        if (ret == 0) {
            ret = output_ret;
        }
    }
    return ret;
}

static int audio_codec_jy6416_set_reg(audio_codec_dev_t *dev, int reg, int value)
{
    if (dev == NULL || dev->i2c == NULL || reg < 0 || reg > (int)(ADC_COUNT * 0x100U + 0xffU) || value < 0 || value > 0xff) {
        return -1;
    }
    if (reg >> 8) {
        return audio_codec_jy6166_set_reg(dev->i2c, adc_addresses[(reg >> 8) - 1], reg & 0xff, value);
    }
    return audio_codec_jy6311_driver.set_reg(&g_jy6416_ctx.jy6311, reg, value);
}

static int audio_codec_jy6416_get_reg(audio_codec_dev_t *dev, int reg, int *value)
{
    if (dev == NULL || dev->i2c == NULL || value == NULL || reg < 0 || reg > (int)(ADC_COUNT * 0x100U + 0xffU)) {
        return -1;
    }
    if (reg >> 8) {
        return audio_codec_jy6166_get_reg(dev->i2c, adc_addresses[(reg >> 8) - 1], reg & 0xff, value);
    }
    return audio_codec_jy6311_driver.get_reg(&g_jy6416_ctx.jy6311, reg, value);
}

static int audio_codec_jy6416_dump_reg(audio_codec_dev_t *dev)
{
    if (dev == NULL || dev->i2c == NULL ||
        audio_codec_jy6311_driver.dump_reg(&g_jy6416_ctx.jy6311) != 0) {
        return -1;
    }
    for (unsigned i = 0; i < ADC_COUNT; ++i) {
        printf("%s ADC%u addr=0x%02x\n", "JY6416", i, adc_addresses[i]);
        if (audio_codec_jy6166_dump_reg(dev->i2c, adc_addresses[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

static int audio_codec_jy6416_deinit(audio_codec_dev_t *dev)
{
    int ret;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }
    g_jy6416_ctx.cfg.sample_rate_hz = 0;
    dev->priv = NULL;
    ret = 0;
    for (unsigned i = 0; i < ADC_COUNT; ++i) {
        if (jy6416_stop_adc(dev->i2c, adc_addresses[i]) != 0) {
            ret = -1;
        }
    }
    if (audio_codec_jy6311_driver.deinit(&g_jy6416_ctx.jy6311) != 0 || ret != 0) {
        return -1;
    }
    g_jy6416_ctx.initialized = false;
    dev->priv = NULL;
    return 0;
}

static uint32_t audio_codec_jy6416_calc_mclk_hz(uint32_t sample_rate_hz)
{
    (void)sample_rate_hz;
    return 0; /* No external MCLK is required. */
}

static int audio_codec_jy6416_get_mic_channel_caps(audio_codec_dev_t *dev, uint8_t slot,
                                                   audio_codec_channel_caps_t *caps)
{
    (void)dev;
    static const audio_codec_channel_caps_t native = {
        .controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB,
        .independent_controls = AUDIO_CODEC_CONTROL_VOLUME | AUDIO_CODEC_CONTROL_GAIN_DB,
        .linked_mask = 0x3c,
        .gain_steps_db = jy6416_gain_steps_db,
        .gain_step_count = 14,
    };
    static const audio_codec_channel_caps_t reference = { 0 };
    return audio_codec_native_caps(0x3d, slot, slot == 0 ? &reference : &native, caps);
}

static int audio_codec_jy6416_get_speaker_channel_caps(audio_codec_dev_t *dev, uint8_t slot,
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

static const uint8_t jy6416_addrs[] = { JY6416_CODEC_ADDR };

const audio_codec_driver_t audio_codec_jy6416_driver = {
    .set_enable = audio_codec_jy6416_set_enable,
    .set_reg = audio_codec_jy6416_set_reg,
    .get_reg = audio_codec_jy6416_get_reg,
    .dump_reg = audio_codec_jy6416_dump_reg,
    .deinit = audio_codec_jy6416_deinit,
    .name = "JY6416",
    .i2c_addrs = jy6416_addrs,
    .probe = audio_codec_jy6416_probe,
    .init = audio_codec_jy6416_init,
    .get_mic_channel_caps = audio_codec_jy6416_get_mic_channel_caps,
    .get_speaker_channel_caps = audio_codec_jy6416_get_speaker_channel_caps,
    .set_speaker_channel_volume = audio_codec_jy6416_set_speaker_channel_volume,
    .set_speaker_channel_mute = audio_codec_jy6416_set_speaker_channel_mute,
    .set_mic_channel_volume = audio_codec_jy6416_set_mic_channel_volume,
    .set_mic_channel_gain_db = audio_codec_jy6416_set_mic_channel_gain_db,
    .set_sample_rate = audio_codec_jy6416_set_sample_rate,
    .calc_mclk_hz = audio_codec_jy6416_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_JY6416,
    .i2c_addrs_num = (uint8_t)(sizeof(jy6416_addrs) / sizeof(jy6416_addrs[0])),
};
