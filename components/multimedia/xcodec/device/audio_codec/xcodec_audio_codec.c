/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "xcodec_audio_codec.h"

#if defined(CONFIG_BSP_AUDIO_CODEC)
#include "audio_codec_driver.h"

typedef struct {
    xcodec_if_t base;
    xcodec_audio_codec_cfg_t cfg;
    uint16_t channel_mask;
    bool opened;
    bool enabled;
    bool initialized;
} audio_codec_t;

static int codec_result(int ret)
{
    return ret == -2 ? XCODEC_DEV_NOT_SUPPORT : ret ? XCODEC_DEV_DRV_ERR : XCODEC_DEV_OK;
}

static int codec_lock(const audio_codec_t *codec)
{
    if (!codec->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    return codec->cfg.acquire ? codec->cfg.acquire() : XCODEC_DEV_OK;
}

static int codec_unlock(const audio_codec_t *codec, int ret)
{
    if (codec->cfg.release) {
        codec->cfg.release();
    }
    return codec_result(ret);
}

/* Stream masks declare input slots; the converter reports physical outputs.
 * Read-only reference/empty slots do not participate in global controls. */
static int codec_control_mask(const audio_codec_t *codec, bool input, uint32_t control, uint16_t *mask)
{
    uint16_t capture_mask;
    uint16_t playback_mask;
    int ret = audio_codec_get_channel_masks(codec->cfg.codec, &capture_mask, &playback_mask);
    if (ret) {
        return ret;
    }
    uint16_t selected = input ? capture_mask : playback_mask;
    if (input && codec->channel_mask) {
        selected &= codec->channel_mask;
    }
    *mask = 0;
    for (uint8_t slot = 0; slot < AUDIO_CODEC_MAX_SLOTS; ++slot) {
        uint16_t bit = (uint16_t)(1U << slot);
        if (!(selected & bit)) {
            continue;
        }
        audio_codec_channel_caps_t caps;
        ret = input ? audio_codec_get_mic_channel_caps(codec->cfg.codec, slot, &caps) :
                      audio_codec_get_speaker_channel_caps(codec->cfg.codec, slot, &caps);
        if (ret) {
            return ret;
        }
        if ((caps.controls & control) &&
            (control != AUDIO_CODEC_CONTROL_GAIN_DB || (caps.gain_steps_db && caps.gain_step_count))) {
            *mask |= bit;
        }
    }
    return *mask ? 0 : -2;
}

static bool codec_is_open(const xcodec_if_t *h)
{
    return ((const audio_codec_t *)h)->opened;
}

static int codec_open(const xcodec_if_t *h, void *cfg, int cfg_size)
{
    audio_codec_t *codec = (audio_codec_t *)h;
    const xcodec_audio_codec_cfg_t *config = cfg;
    if (!config || cfg_size != sizeof(*config) || !config->codec || !config->codec->drv ||
        (!!config->acquire != !!config->release) || config->mode > AUDIO_CODEC_WORK_MODE_PLAYBACK) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (codec->opened) {
        return XCODEC_DEV_WRONG_STATE;
    }
    codec->cfg = *config;
    int ret = config->acquire ? config->acquire() : XCODEC_DEV_OK;
    if (ret) {
        return ret;
    }
    if (config->init_cfg) {
        ret = audio_codec_init(config->codec, config->init_cfg);
        codec->initialized = ret == 0;
    }
    if (!ret && config->mode != AUDIO_CODEC_WORK_MODE_RECORDING) {
        uint16_t mask;
        ret = codec_control_mask(codec, false, AUDIO_CODEC_CONTROL_MUTE, &mask);
        if (!ret) {
            ret = audio_codec_set_speaker_channel_mute(config->codec, mask, true);
        }
    }
    if (ret && codec->initialized) {
        (void)audio_codec_deinit(config->codec);
        codec->initialized = false;
    }
    codec->opened = ret == 0;
    return codec_unlock(codec, ret);
}

static int codec_set_fs(const xcodec_if_t *h, xcodec_dev_sample_info_t *fs)
{
    audio_codec_t *codec = (audio_codec_t *)h;
    int ret = codec_lock(codec);
    if (ret) {
        return ret;
    }
    ret = audio_codec_set_sample_rate(codec->cfg.codec, fs->sample_rate);
    if (!ret) {
        codec->channel_mask = fs->channel_mask ? fs->channel_mask : (1U << fs->channel) - 1U;
    }
    return codec_unlock(codec, ret);
}

static int codec_enable(const xcodec_if_t *h, bool enable)
{
    audio_codec_t *codec = (audio_codec_t *)h;
    int ret = codec_lock(codec);
    if (ret) {
        return ret;
    }
    if (enable || codec->enabled) {
        ret = audio_codec_set_enable(codec->cfg.codec, codec->cfg.mode, enable);
        /* Hardware errors need a disable retry; unsupported calls change no state. */
        if (ret != -2) {
            codec->enabled = enable || ret != 0;
        }
    }
    return codec_unlock(codec, ret);
}

static int codec_mute(const xcodec_if_t *h, bool mute)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    int ret = codec_lock(codec);
    if (ret) {
        return ret;
    }
    uint16_t mask;
    ret = codec_control_mask(codec, false, AUDIO_CODEC_CONTROL_MUTE, &mask);
    return codec_unlock(codec, ret ? ret : audio_codec_set_speaker_channel_mute(codec->cfg.codec, mask, mute));
}

static int codec_volume(const xcodec_if_t *h, float db)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    if (!isfinite(db) || db < -96 || db > 0) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = codec_lock(codec);
    if (ret) {
        return ret;
    }
    /* Preserve the BSP percentage curve used by the device volume setting. */
    int volume = db <= -50 ? 0 : (int)((db + 50) * 2 + 0.5f);
    uint16_t mask;
    ret = codec_control_mask(codec, false, AUDIO_CODEC_CONTROL_VOLUME, &mask);
    return codec_unlock(codec, ret ? ret : audio_codec_set_speaker_channel_volume(codec->cfg.codec, mask, volume));
}

static int codec_gain(const xcodec_if_t *h, float db)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    int ret = codec_lock(codec);
    if (ret) {
        return ret;
    }
    uint16_t mask;
    ret = codec_control_mask(codec, true, AUDIO_CODEC_CONTROL_GAIN_DB, &mask);
    return codec_unlock(codec, ret ? ret : audio_codec_set_mic_channel_gain_db(codec->cfg.codec, mask, db));
}

static int codec_channel_gain(const xcodec_if_t *h, uint16_t mask, float db)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    if (!mask || !isfinite(db) || (codec->channel_mask && (mask & ~codec->channel_mask))) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = codec_lock(codec);
    return ret ? ret : codec_unlock(codec, audio_codec_set_mic_channel_gain_db(codec->cfg.codec, mask, db));
}

static int codec_mic_mute(const xcodec_if_t *h, bool mute)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    int ret = codec_lock(codec);
    if (ret) {
        return ret;
    }
    uint16_t mask;
    ret = codec_control_mask(codec, true, AUDIO_CODEC_CONTROL_MUTE, &mask);
    return codec_unlock(codec, ret ? ret : audio_codec_set_mic_channel_mute(codec->cfg.codec, mask, mute));
}

static int codec_set_reg(const xcodec_if_t *h, int reg, int value)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    int ret = codec_lock(codec);
    return ret ? ret : codec_unlock(codec, audio_codec_set_reg(codec->cfg.codec, reg, value));
}

static int codec_get_reg(const xcodec_if_t *h, int reg, int *value)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    int ret = codec_lock(codec);
    return ret ? ret : codec_unlock(codec, audio_codec_get_reg(codec->cfg.codec, reg, value));
}

static void codec_dump(const xcodec_if_t *h)
{
    const audio_codec_t *codec = (const audio_codec_t *)h;
    int ret = codec_lock(codec);
    if (!ret) {
        ret = codec_unlock(codec, audio_codec_dump_reg(codec->cfg.codec));
    }
    if (ret) {
        printf("xcodec: register dump failed: %d\r\n", ret);
    }
}

static int codec_close(const xcodec_if_t *h)
{
    audio_codec_t *codec = (audio_codec_t *)h;
    if (!codec->opened) {
        return XCODEC_DEV_OK;
    }
    if (codec->enabled) {
        int ret = codec_enable(h, false);
        if (ret) {
            return ret;
        }
    }
    if (codec->cfg.mode != AUDIO_CODEC_WORK_MODE_RECORDING) {
        int ret = codec_mute(h, true);
        if (ret) {
            return ret;
        }
    }
    if (codec->initialized) {
        int ret = codec_lock(codec);
        if (ret) {
            return ret;
        }
        ret = codec_unlock(codec, audio_codec_deinit(codec->cfg.codec));
        if (ret && ret != XCODEC_DEV_NOT_SUPPORT) {
            return ret;
        }
        codec->initialized = false;
    }
    codec->opened = false;
    return XCODEC_DEV_OK;
}

const xcodec_if_t *xcodec_ctrl_if_new(const xcodec_audio_codec_cfg_t *cfg)
{
    audio_codec_t *codec = calloc(1, sizeof(*codec));
    if (!codec) {
        return NULL;
    }
    codec->base.open = codec_open;
    codec->base.is_open = codec_is_open;
    codec->base.set_fs = codec_set_fs;
    codec->base.mute = codec_mute;
    codec->base.set_vol = codec_volume;
    codec->base.set_mic_gain = codec_gain;
    codec->base.set_mic_channel_gain = codec_channel_gain;
    codec->base.mute_mic = codec_mic_mute;
    codec->base.set_reg = codec_set_reg;
    codec->base.get_reg = codec_get_reg;
    codec->base.dump_reg = codec_dump;
    codec->base.close = codec_close;
    if (codec->base.open(&codec->base, (void *)cfg, sizeof(*cfg))) {
        free(codec);
        return NULL;
    }
    codec->base.enable = cfg->codec->drv->set_enable ? codec_enable : NULL;
    return &codec->base;
}
#endif
