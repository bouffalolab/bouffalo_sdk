/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include <math.h>
#include <stdlib.h>
#include "xcodec_dev.h"

xcodec_channel_cfg_t xcodec_channel_default_config = XCODEC_CHANNEL_DEFAULT_CONFIG();

typedef struct {
    xcodec_dev_cfg_t cfg;
    bool opened;
    bool data_enabled;
    bool codec_enabled;
    bool disable_when_closed;
    int volume;
    float gain;
    bool out_mute;
    bool in_mute;
    bool volume_set;
    bool gain_set;
    bool out_mute_set;
    bool in_mute_set;
} xcodec_dev_t;

static int check_direction(xcodec_dev_t *dev, xcodec_dev_type_t direction)
{
    if (!dev) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (!(dev->cfg.dev_type & direction)) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    return dev->opened ? XCODEC_DEV_OK : XCODEC_DEV_WRONG_STATE;
}

static float volume_db(int volume)
{
    return volume ? volume * 0.5f - 50.0f : -96.0f;
}

xcodec_dev_handle_t xcodec_dev_new(xcodec_dev_cfg_t *cfg)
{
    if (!cfg || !cfg->data_if || cfg->dev_type < XCODEC_DEV_TYPE_IN ||
        cfg->dev_type > XCODEC_DEV_TYPE_IN_OUT) {
        return NULL;
    }
    xcodec_dev_t *dev = calloc(1, sizeof(*dev));
    if (dev) {
        dev->cfg = *cfg;
        dev->disable_when_closed = true;
    }
    return dev;
}

const char *xcodec_dev_get_version(void)
{
    return "1.0.0";
}

int xcodec_dev_open(xcodec_dev_handle_t handle, xcodec_dev_sample_info_t *fs)
{
    xcodec_dev_t *dev = handle;
    if (!dev || !fs || !fs->sample_rate || !fs->channel || fs->channel > 16 ||
        (fs->bits_per_sample != 8 && fs->bits_per_sample != 16 &&
         fs->bits_per_sample != 24 && fs->bits_per_sample != 32) ||
        (fs->channel_mask >> fs->channel) || fs->mclk_multiple < 0) {
        return XCODEC_DEV_INVALID_ARG;
    }
    if (dev->opened) {
        return XCODEC_DEV_OK;
    }
    if (dev->data_enabled || (dev->codec_enabled && dev->disable_when_closed)) {
        return XCODEC_DEV_WRONG_STATE;
    }
    const xcodec_data_if_t *data = dev->cfg.data_if;
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (!data->enable || !data->set_fmt ||
        ((dev->cfg.dev_type & XCODEC_DEV_TYPE_IN) && !data->read) ||
        ((dev->cfg.dev_type & XCODEC_DEV_TYPE_OUT) && !data->write) ||
        (data->is_open && !data->is_open(data)) ||
        (codec && codec->is_open && !codec->is_open(codec))) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    int ret = data->set_fmt(data, dev->cfg.dev_type, fs);
    if (ret) {
        return ret;
    }
    if (codec && codec->set_fs) {
        ret = codec->set_fs(codec, fs);
        if (ret) {
            return ret;
        }
    }
    /* Track attempted enables so even a partially failed backend is disabled. */
    dev->data_enabled = true;
    ret = data->enable(data, dev->cfg.dev_type, true);
    if (!ret && codec && codec->enable) {
        dev->codec_enabled = true;
        ret = codec->enable(codec, true);
    }
    if (!ret && codec) {
        if (dev->volume_set && codec->set_vol) {
            ret = codec->set_vol(codec, volume_db(dev->volume));
        }
        if (!ret && dev->gain_set && codec->set_mic_gain) {
            ret = codec->set_mic_gain(codec, dev->gain);
        }
        if (!ret && dev->out_mute_set && codec->mute) {
            ret = codec->mute(codec, dev->out_mute);
        }
        if (!ret && dev->in_mute_set && codec->mute_mic) {
            ret = codec->mute_mic(codec, dev->in_mute);
        }
    }
    if (ret) {
        bool disable = dev->disable_when_closed;
        dev->disable_when_closed = true;
        xcodec_dev_close(dev);
        dev->disable_when_closed = disable;
        return ret;
    }
    dev->opened = true;
    return XCODEC_DEV_OK;
}

int xcodec_dev_read(xcodec_dev_handle_t handle, void *buffer, int len)
{
    xcodec_dev_t *dev = handle;
    if (!buffer || len <= 0) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_IN);
    if (ret) {
        return ret;
    }
    ret = dev->cfg.data_if->read(dev->cfg.data_if, buffer, len);
    return ret > 0 ? XCODEC_DEV_READ_FAIL : ret;
}

int xcodec_dev_write(xcodec_dev_handle_t handle, void *buffer, int len)
{
    xcodec_dev_t *dev = handle;
    if (!buffer || len <= 0) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_OUT);
    if (ret) {
        return ret;
    }
    ret = dev->cfg.data_if->write(dev->cfg.data_if, buffer, len);
    return ret > 0 ? XCODEC_DEV_WRITE_FAIL : ret;
}

int xcodec_dev_set_out_vol(xcodec_dev_handle_t handle, int volume)
{
    xcodec_dev_t *dev = handle;
    if (volume < 0 || volume > 100) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_OUT);
    if (ret) {
        return ret;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (!codec || !codec->set_vol) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    ret = codec->set_vol(codec, volume_db(volume));
    if (!ret) {
        dev->volume = volume;
        dev->volume_set = true;
    }
    return ret;
}

int xcodec_dev_get_out_vol(xcodec_dev_handle_t handle, int *volume)
{
    xcodec_dev_t *dev = handle;
    if (!volume) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_OUT);
    if (ret) {
        return ret;
    }
    if (!dev->cfg.codec_if || !dev->cfg.codec_if->set_vol) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    *volume = dev->volume;
    return XCODEC_DEV_OK;
}

int xcodec_dev_set_in_gain(xcodec_dev_handle_t handle, float gain)
{
    xcodec_dev_t *dev = handle;
    if (!isfinite(gain)) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_IN);
    if (ret) {
        return ret;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (!codec || !codec->set_mic_gain) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    ret = codec->set_mic_gain(codec, gain);
    if (!ret) {
        dev->gain = gain;
        dev->gain_set = true;
    }
    return ret;
}

int xcodec_dev_set_in_channel_gain(xcodec_dev_handle_t handle, uint16_t mask, float gain)
{
    xcodec_dev_t *dev = handle;
    if (!mask || !isfinite(gain)) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_IN);
    if (ret) {
        return ret;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    return codec && codec->set_mic_channel_gain ? codec->set_mic_channel_gain(codec, mask, gain) :
                                               XCODEC_DEV_NOT_SUPPORT;
}

int xcodec_dev_get_in_gain(xcodec_dev_handle_t handle, float *gain)
{
    xcodec_dev_t *dev = handle;
    if (!gain) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_IN);
    if (ret) {
        return ret;
    }
    if (!dev->cfg.codec_if || !dev->cfg.codec_if->set_mic_gain) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    *gain = dev->gain;
    return XCODEC_DEV_OK;
}

int xcodec_dev_set_out_mute(xcodec_dev_handle_t handle, bool mute)
{
    xcodec_dev_t *dev = handle;
    int ret = check_direction(dev, XCODEC_DEV_TYPE_OUT);
    if (ret) {
        return ret;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (!codec || !codec->mute) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    ret = codec->mute(codec, mute);
    if (!ret) {
        dev->out_mute = mute;
        dev->out_mute_set = true;
    }
    return ret;
}

int xcodec_dev_get_out_mute(xcodec_dev_handle_t handle, bool *mute)
{
    xcodec_dev_t *dev = handle;
    if (!mute) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_OUT);
    if (ret) {
        return ret;
    }
    if (!dev->cfg.codec_if || !dev->cfg.codec_if->mute) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    *mute = dev->out_mute;
    return XCODEC_DEV_OK;
}

int xcodec_dev_set_in_mute(xcodec_dev_handle_t handle, bool mute)
{
    xcodec_dev_t *dev = handle;
    int ret = check_direction(dev, XCODEC_DEV_TYPE_IN);
    if (ret) {
        return ret;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (!codec || !codec->mute_mic) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    ret = codec->mute_mic(codec, mute);
    if (!ret) {
        dev->in_mute = mute;
        dev->in_mute_set = true;
    }
    return ret;
}

int xcodec_dev_get_in_mute(xcodec_dev_handle_t handle, bool *mute)
{
    xcodec_dev_t *dev = handle;
    if (!mute) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = check_direction(dev, XCODEC_DEV_TYPE_IN);
    if (ret) {
        return ret;
    }
    if (!dev->cfg.codec_if || !dev->cfg.codec_if->mute_mic) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    *mute = dev->in_mute;
    return XCODEC_DEV_OK;
}

int xcodec_dev_read_reg(xcodec_dev_handle_t handle, int reg, int *value)
{
    xcodec_dev_t *dev = handle;
    if (!dev || !value) {
        return XCODEC_DEV_INVALID_ARG;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    return codec && codec->get_reg ? codec->get_reg(codec, reg, value) : XCODEC_DEV_NOT_SUPPORT;
}

int xcodec_dev_write_reg(xcodec_dev_handle_t handle, int reg, int value)
{
    xcodec_dev_t *dev = handle;
    if (!dev) {
        return XCODEC_DEV_INVALID_ARG;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    return codec && codec->set_reg ? codec->set_reg(codec, reg, value) : XCODEC_DEV_NOT_SUPPORT;
}

int xcodec_dev_dump_reg(xcodec_dev_handle_t handle)
{
    xcodec_dev_t *dev = handle;
    if (!dev) {
        return XCODEC_DEV_INVALID_ARG;
    }
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (!codec || !codec->dump_reg) {
        return XCODEC_DEV_NOT_SUPPORT;
    }
    codec->dump_reg(codec);
    return XCODEC_DEV_OK;
}

int xcodec_dev_set_disable_when_closed(xcodec_dev_handle_t handle, bool disable)
{
    xcodec_dev_t *dev = handle;
    if (!dev) {
        return XCODEC_DEV_INVALID_ARG;
    }
    dev->disable_when_closed = disable;
    return XCODEC_DEV_OK;
}

int xcodec_dev_close(xcodec_dev_handle_t handle)
{
    xcodec_dev_t *dev = handle;
    if (!dev) {
        return XCODEC_DEV_INVALID_ARG;
    }
    dev->opened = false;
    int ret = XCODEC_DEV_OK;
    const xcodec_if_t *codec = dev->cfg.codec_if;
    if (dev->codec_enabled && dev->disable_when_closed) {
        ret = codec->enable(codec, false);
        if (!ret) {
            dev->codec_enabled = false;
        }
    }
    if (dev->data_enabled) {
        int data_ret = dev->cfg.data_if->enable(dev->cfg.data_if, dev->cfg.dev_type, false);
        if (!data_ret) {
            dev->data_enabled = false;
        }
        if (!ret) {
            ret = data_ret;
        }
    }
    return ret;
}

void xcodec_dev_delete(xcodec_dev_handle_t handle)
{
    if (handle && xcodec_dev_close(handle) == XCODEC_DEV_OK) {
        free(handle);
    }
}
