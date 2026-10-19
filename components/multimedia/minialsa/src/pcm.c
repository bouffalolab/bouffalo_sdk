/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
#include <xutils/device.h>
#include <xutils/driver.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>

#include <alsa/pcm.h>
#include <alsa/snd.h>

#define TAG "pcm"

#define PCM_LOCK(pcm) msp_mutex_lock(&pcm->mutex, MSP_WAIT_FOREVER)
#define PCM_UNLOCK(pcm) msp_mutex_unlock(&pcm->mutex)

#define PCM_CHECK_RETURN_EINVAL(X) msp_check_return_val((X), -EINVAL)
#define PCM_CHECK_RETURN_ENOMEM(X) msp_check_return_val((X), -ENOMEM)

#define hw_params(pcm) pcm->hw_params
#define sw_params(pcm) pcm->sw_params

static int pcm_physical_bits(const msp_pcm_hw_params_t *params);

int snd_pcm_format_signed(snd_pcm_format_t format)
{
    switch (format) {
        case SND_PCM_FORMAT_S8:
        case SND_PCM_FORMAT_S16_LE:
        case SND_PCM_FORMAT_S16_BE:
        case SND_PCM_FORMAT_S24_LE:
        case SND_PCM_FORMAT_S24_BE:
        case SND_PCM_FORMAT_S32_LE:
        case SND_PCM_FORMAT_S32_BE:
        case SND_PCM_FORMAT_S20_LE:
        case SND_PCM_FORMAT_S20_BE:
        case SND_PCM_FORMAT_S24_3LE:
        case SND_PCM_FORMAT_S24_3BE:
        case SND_PCM_FORMAT_S20_3LE:
        case SND_PCM_FORMAT_S20_3BE:
        case SND_PCM_FORMAT_S18_3LE:
        case SND_PCM_FORMAT_S18_3BE:
        case SND_PCM_FORMAT_FLOAT_LE:
        case SND_PCM_FORMAT_FLOAT_BE:
        case SND_PCM_FORMAT_FLOAT64_LE:
        case SND_PCM_FORMAT_FLOAT64_BE:
            return 1;
        default:
            return 0;
    }
}

int snd_pcm_format_float(snd_pcm_format_t format)
{
    switch (format) {
        case SND_PCM_FORMAT_FLOAT_LE:
        case SND_PCM_FORMAT_FLOAT_BE:
        case SND_PCM_FORMAT_FLOAT64_LE:
        case SND_PCM_FORMAT_FLOAT64_BE:
            return 1;
        default:
            return 0;
    }
}

int snd_pcm_format_little_endian(snd_pcm_format_t format)
{
    switch (format) {
        case SND_PCM_FORMAT_S16_LE:
        case SND_PCM_FORMAT_U16_LE:
        case SND_PCM_FORMAT_S24_LE:
        case SND_PCM_FORMAT_U24_LE:
        case SND_PCM_FORMAT_S32_LE:
        case SND_PCM_FORMAT_U32_LE:
        case SND_PCM_FORMAT_FLOAT_LE:
        case SND_PCM_FORMAT_FLOAT64_LE:
        case SND_PCM_FORMAT_S20_LE:
        case SND_PCM_FORMAT_U20_LE:
        case SND_PCM_FORMAT_S24_3LE:
        case SND_PCM_FORMAT_U24_3LE:
        case SND_PCM_FORMAT_S20_3LE:
        case SND_PCM_FORMAT_U20_3LE:
        case SND_PCM_FORMAT_S18_3LE:
        case SND_PCM_FORMAT_U18_3LE:
            return 1;
        default:
            return 0;
    }
}

int snd_pcm_format_big_endian(snd_pcm_format_t format)
{
    switch (format) {
        case SND_PCM_FORMAT_S16_BE:
        case SND_PCM_FORMAT_U16_BE:
        case SND_PCM_FORMAT_S24_BE:
        case SND_PCM_FORMAT_U24_BE:
        case SND_PCM_FORMAT_S32_BE:
        case SND_PCM_FORMAT_U32_BE:
        case SND_PCM_FORMAT_FLOAT_BE:
        case SND_PCM_FORMAT_FLOAT64_BE:
        case SND_PCM_FORMAT_S20_BE:
        case SND_PCM_FORMAT_U20_BE:
        case SND_PCM_FORMAT_S24_3BE:
        case SND_PCM_FORMAT_U24_3BE:
        case SND_PCM_FORMAT_S20_3BE:
        case SND_PCM_FORMAT_U20_3BE:
        case SND_PCM_FORMAT_S18_3BE:
        case SND_PCM_FORMAT_U18_3BE:
            return 1;
        default:
            return 0;
    }
}

int snd_pcm_format_width(snd_pcm_format_t format)
{
    switch (format) {
        case SND_PCM_FORMAT_S8:
        case SND_PCM_FORMAT_U8:
            return 8;
        case SND_PCM_FORMAT_S16_LE:
        case SND_PCM_FORMAT_S16_BE:
        case SND_PCM_FORMAT_U16_LE:
        case SND_PCM_FORMAT_U16_BE:
            return 16;
        case SND_PCM_FORMAT_S18_3LE:
        case SND_PCM_FORMAT_S18_3BE:
        case SND_PCM_FORMAT_U18_3LE:
        case SND_PCM_FORMAT_U18_3BE:
            return 18;
        case SND_PCM_FORMAT_S20_LE:
        case SND_PCM_FORMAT_S20_BE:
        case SND_PCM_FORMAT_U20_LE:
        case SND_PCM_FORMAT_U20_BE:
        case SND_PCM_FORMAT_S20_3LE:
        case SND_PCM_FORMAT_S20_3BE:
        case SND_PCM_FORMAT_U20_3LE:
        case SND_PCM_FORMAT_U20_3BE:
            return 20;
        case SND_PCM_FORMAT_S24_LE:
        case SND_PCM_FORMAT_S24_BE:
        case SND_PCM_FORMAT_U24_LE:
        case SND_PCM_FORMAT_U24_BE:
        case SND_PCM_FORMAT_S24_3LE:
        case SND_PCM_FORMAT_S24_3BE:
        case SND_PCM_FORMAT_U24_3LE:
        case SND_PCM_FORMAT_U24_3BE:
            return 24;
        case SND_PCM_FORMAT_S32_LE:
        case SND_PCM_FORMAT_S32_BE:
        case SND_PCM_FORMAT_U32_LE:
        case SND_PCM_FORMAT_U32_BE:
        case SND_PCM_FORMAT_FLOAT_LE:
        case SND_PCM_FORMAT_FLOAT_BE:
            return 32;
        case SND_PCM_FORMAT_FLOAT64_LE:
        case SND_PCM_FORMAT_FLOAT64_BE:
            return 64;
        default:
            return -EINVAL;
    }
}

int snd_pcm_format_physical_width(snd_pcm_format_t format)
{
    switch (format) {
        case SND_PCM_FORMAT_S8:
        case SND_PCM_FORMAT_U8:
            return 8;
        case SND_PCM_FORMAT_S16_LE:
        case SND_PCM_FORMAT_S16_BE:
        case SND_PCM_FORMAT_U16_LE:
        case SND_PCM_FORMAT_U16_BE:
            return 16;
        case SND_PCM_FORMAT_S18_3LE:
        case SND_PCM_FORMAT_S18_3BE:
        case SND_PCM_FORMAT_U18_3LE:
        case SND_PCM_FORMAT_U18_3BE:
        case SND_PCM_FORMAT_S20_3LE:
        case SND_PCM_FORMAT_S20_3BE:
        case SND_PCM_FORMAT_U20_3LE:
        case SND_PCM_FORMAT_U20_3BE:
        case SND_PCM_FORMAT_S24_3LE:
        case SND_PCM_FORMAT_S24_3BE:
        case SND_PCM_FORMAT_U24_3LE:
        case SND_PCM_FORMAT_U24_3BE:
            return 24;
        case SND_PCM_FORMAT_S20_LE:
        case SND_PCM_FORMAT_S20_BE:
        case SND_PCM_FORMAT_U20_LE:
        case SND_PCM_FORMAT_U20_BE:
        case SND_PCM_FORMAT_S24_LE:
        case SND_PCM_FORMAT_S24_BE:
        case SND_PCM_FORMAT_U24_LE:
        case SND_PCM_FORMAT_U24_BE:
        case SND_PCM_FORMAT_S32_LE:
        case SND_PCM_FORMAT_S32_BE:
        case SND_PCM_FORMAT_U32_LE:
        case SND_PCM_FORMAT_U32_BE:
        case SND_PCM_FORMAT_FLOAT_LE:
        case SND_PCM_FORMAT_FLOAT_BE:
            return 32;
        case SND_PCM_FORMAT_FLOAT64_LE:
        case SND_PCM_FORMAT_FLOAT64_BE:
            return 64;
        default:
            return -EINVAL;
    }
}

static int pcm_sample_bits(const msp_pcm_hw_params_t *params)
{
    int width;

    if (params == NULL) {
        return 0;
    }

    if (params->sample_bits > 0) {
        return params->sample_bits;
    }

    width = snd_pcm_format_width((snd_pcm_format_t)params->format);
    return width > 0 ? width : 0;
}

static int pcm_physical_bits(const msp_pcm_hw_params_t *params)
{
    int width;

    if (params == NULL) {
        return 0;
    }

    width = snd_pcm_format_physical_width((snd_pcm_format_t)params->format);
    if (width > 0) {
        return width;
    }

    return pcm_sample_bits(params);
}

static int pcm_frame_bytes_from_params(const msp_pcm_hw_params_t *params)
{
    int physical_bits;

    if (params == NULL || params->channels <= 0) {
        return 0;
    }

    physical_bits = pcm_physical_bits(params);
    if (physical_bits <= 0) {
        return 0;
    }

    return (physical_bits * params->channels) / 8;
}

static void pcm_recalc_hw_params(msp_pcm_hw_params_t *params)
{
    int sample_bits, physical_bits, frame_bytes;

    if (params == NULL) {
        return;
    }

    sample_bits = pcm_sample_bits(params);
    if (sample_bits <= 0) {
        return;
    }
    physical_bits = pcm_physical_bits(params);
    if (physical_bits <= 0) {
        return;
    }

    params->sample_bits = sample_bits;
    if (params->channels > 0) {
        params->frame_bits = physical_bits * params->channels;
    }
    if (params->rate > 0 && params->period_size > 0) {
        params->period_time = (params->period_size * 1000000ULL) / params->rate;
    }
    if (params->rate > 0 && params->buffer_size > 0) {
        params->buffer_time = (params->buffer_size * 1000000ULL) / params->rate;
    }
    frame_bytes = pcm_frame_bytes_from_params(params);
    if (frame_bytes > 0 && params->period_size > 0) {
        params->period_bytes = params->period_size * frame_bytes;
    }
    if (frame_bytes > 0 && params->buffer_size > 0) {
        params->buffer_bytes = params->buffer_size * frame_bytes;
    }
}

static void pcm_init_default_hw_params(msp_pcm_hw_params_t *params)
{
    if (params == NULL) {
        return;
    }

    memset(params, 0, sizeof(*params));
    params->access = MSP_PCM_ACCESS_RW_INTERLEAVED;
    params->channels = 2;
    params->rate = 16000;
    params->format = SND_PCM_FORMAT_S16;
    params->sample_bits = 16;
    pcm_recalc_hw_params(params);
}

static void pcm_init_default_sw_params(msp_pcm_t *pcm, msp_pcm_sw_params_t *params)
{
    if (params == NULL) {
        return;
    }

    memset(params, 0, sizeof(*params));
    if (pcm != NULL && pcm->hw_params != NULL) {
        if (pcm->hw_params->period_size > 0) {
            params->avail_min = pcm->hw_params->period_size;
            params->start_threshold = pcm->hw_params->period_size;
        } else {
            params->avail_min = 1;
            params->start_threshold = 1;
        }
        if (pcm->hw_params->buffer_size > 0) {
            params->stop_threshold = pcm->hw_params->buffer_size;
            params->boundary = pcm->hw_params->buffer_size;
        }
    }
}

static void pcm_event(msp_pcm_t *pcm, int event_id, void *priv)
{
    (void)priv;

    if (event_id == PCM_EVT_XRUN) {
        msp_event_set(&pcm->evt, 0, MSP_EVENT_AND);
    }

    msp_event_set(&pcm->evt, event_id, MSP_EVENT_OR);
}

static int pcm_runtime_init(msp_pcm_t *pcm)
{
    int mutex_created = 0;

    PCM_CHECK_RETURN_EINVAL(pcm);

    if (!msp_mutex_is_valid(&pcm->mutex)) {
        if (msp_mutex_new(&pcm->mutex) != 0) {
            return -ENOMEM;
        }
        mutex_created = 1;
    }
    if (!msp_event_is_valid(&pcm->evt) && msp_event_new(&pcm->evt, 0) != 0) {
        if (mutex_created) {
            msp_mutex_free(&pcm->mutex);
        }
        return -ENOMEM;
    }
    if (pcm->event.cb == NULL) {
        pcm->event.cb = pcm_event;
        pcm->event.priv = NULL;
    }

    return 0;
}

static msp_dev_t *pcm_get_dev(msp_pcm_t *pcm)
{
    if (pcm == NULL || pcm->device == 0) {
        return NULL;
    }

    return &((msp_pcm_dev_t *)((char *)pcm - offsetof(msp_pcm_dev_t, pcm)))->device;
}

static int pcm_store_hw_params(msp_pcm_t *pcm, const msp_pcm_hw_params_t *params)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    if (pcm->hw_params == NULL) {
        pcm->hw_params = msp_zalloc(sizeof(*pcm->hw_params));
        PCM_CHECK_RETURN_ENOMEM(pcm->hw_params);
    }

    memcpy(pcm->hw_params, params, sizeof(*params));
    pcm_recalc_hw_params(pcm->hw_params);

    return 0;
}

static int pcm_store_sw_params(msp_pcm_t *pcm, const msp_pcm_sw_params_t *params)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    if (pcm->sw_params == NULL) {
        pcm->sw_params = msp_zalloc(sizeof(*pcm->sw_params));
        PCM_CHECK_RETURN_ENOMEM(pcm->sw_params);
    }

    memcpy(pcm->sw_params, params, sizeof(*params));

    return 0;
}

static void pcm_deinterleave(const msp_pcm_t *pcm, const void *src, void **dsts, int bytes)
{
    const char *input = src;
    int channels = pcm->hw_params->channels;
    int sample_bytes = pcm_physical_bits(pcm->hw_params) / 8;
    int frame_bytes = sample_bytes * channels;
    int frames = bytes / frame_bytes;
    int frame;
    int channel;

    if (channels == 1) {
        memcpy(dsts[0], src, bytes);
        return;
    }

    for (frame = 0; frame < frames; frame++) {
        for (channel = 0; channel < channels; channel++) {
            memcpy((char *)dsts[channel] + frame * sample_bytes,
                   input + frame * frame_bytes + channel * sample_bytes,
                   sample_bytes);
        }
    }
}

int msp_pcm_new(msp_pcm_t **pcm_ret, int type, const char *name, msp_pcm_stream_t stream, int mode)
{
    msp_pcm_t *pcm;

    (void)type;
    PCM_CHECK_RETURN_EINVAL(pcm_ret && name);

    pcm = msp_calloc_check(sizeof(msp_pcm_t), 1);

    pcm->pcm_name = name;
    pcm->mode = mode;
    pcm->stream = stream;
    pcm->state = MSP_PCM_STATE_OPEN;
    pcm->device = 0;
    if (pcm_runtime_init(pcm) != 0) {
        msp_free(pcm);
        *pcm_ret = NULL;
        return -ENOMEM;
    }
    *pcm_ret = pcm;

    return 0;
}

int msp_pcm_open(msp_pcm_t **pcm_ret, const char *name, msp_pcm_stream_t stream, int mode)
{
    msp_pcm_t *pcm;
    card_dev_t *card;

    PCM_CHECK_RETURN_EINVAL(pcm_ret && name);
    *pcm_ret = NULL;

    msp_card_attach("card0", &card); // Do not put it behind of " (msp_pcm_dev_t *)msp_device_open(name)"

    if (card == NULL) {
        *pcm_ret = NULL;
        return -1;
    }

    msp_pcm_dev_t *dev = (msp_pcm_dev_t *)msp_device_open(name);

    if(dev == NULL) {
        *pcm_ret = NULL;
        return -1;
    }

    pcm = &dev->pcm;
    if (pcm_runtime_init(pcm) != 0) {
        msp_device_close((msp_dev_t *)dev);
        *pcm_ret = NULL;
        return -ENOMEM;
    }
    pcm->stream = stream;
    pcm->mode   = mode;
    pcm->device = 1;
    //xringbuffer_create(&pcm->ringbuffer, msp_zalloc_check(1024), 1024);
    pcm->state = MSP_PCM_STATE_OPEN;
    pcm->pcm_name = name;
    *pcm_ret = pcm;
    return 0;
}

int msp_pcm_close(msp_pcm_t *pcm)
{
    msp_dev_t *dev;

    PCM_CHECK_RETURN_EINVAL(pcm);

    //LOGE(TAG, "pcm close");
    dev = pcm_get_dev(pcm);
    if (dev != NULL) {
        msp_device_close(dev);
        if (dev->ref > 0) {
            return 0;
        }
    }

    msp_mutex_free(&pcm->mutex);
    msp_event_free(&pcm->evt);

    if (pcm->hw_params) {
        msp_free(pcm->hw_params);
        pcm->hw_params = NULL;
    }

    if (pcm->sw_params) {
        msp_free(pcm->sw_params);
        pcm->sw_params = NULL;
    }

    //msp_free(pcm->ringbuffer.buffer);
    //xringbuffer_destroy(&pcm->ringbuffer);

    if (dev == NULL) {
        msp_free(pcm);
    }

    return 0;
}

int msp_pcm_hw_params_any(msp_pcm_t *pcm, msp_pcm_hw_params_t *params)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    pcm_init_default_hw_params(params);
    return 0;
}

int msp_pcm_hw_params_set_access(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_access_t _access)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    params->access = _access;

    return 0;
}

int msp_pcm_hw_params_set_format(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, snd_pcm_format_t val)
{
    int sample_bits;

    PCM_CHECK_RETURN_EINVAL(pcm && params);
    sample_bits = snd_pcm_format_width((snd_pcm_format_t)val);
    PCM_CHECK_RETURN_EINVAL(sample_bits > 0);

    params->format = val;
    params->sample_bits = sample_bits;
    pcm_recalc_hw_params(params);

    return 0;
}

int msp_pcm_hw_params_set_channels(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    params->channels = val;
    pcm_recalc_hw_params(params);

    return 0;
}

int msp_pcm_hw_params_set_rate(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val, int dir)
{
    (void)dir;
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    params->rate = val;
    pcm_recalc_hw_params(params);

    return 0;
}

int msp_pcm_hw_params_set_rate_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params && val);

    if (dir) {
        *dir = 0;
    }
    params->rate = *val;
    pcm_recalc_hw_params(params);

    return 0;
}

int msp_pcm_hw_params_set_buffer_time_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params && val);

    if (dir) {
        *dir = 0;
    }
    params->buffer_time = *val;
    if (params->rate > 0) {
        params->buffer_size = ((uint64_t)params->buffer_time * params->rate) / 1000000ULL;
        if (params->buffer_size == 0) {
            params->buffer_size = 1;
        }
        pcm_recalc_hw_params(params);
    }

    return 0;
}

int msp_pcm_hw_params_set_buffer_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params && val);

    params->buffer_size = *val;
    pcm_recalc_hw_params(params);

    return 0;
}

int msp_pcm_hw_params_set_period_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val, int *dir)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params && val);

    if (dir) {
        *dir = 0;
    }
    params->period_size = *val;
    pcm_recalc_hw_params(params);

    return 0;
}

int msp_pcm_hw_params_get_period_size(const msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val, int *dir)
{
    PCM_CHECK_RETURN_EINVAL(params && val);

    *val = params->period_size;
    if (dir) {
        *dir = 0;
    }

    return 0;
}

int msp_pcm_hw_params_get_buffer_size(const msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val)
{
    PCM_CHECK_RETURN_EINVAL(params && val);

    *val = params->buffer_size;
    return 0;
}

int msp_pcm_hw_params_get_rate(const msp_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    PCM_CHECK_RETURN_EINVAL(params && val);

    *val = params->rate;
    if (dir) {
        *dir = 0;
    }

    return 0;
}

int msp_pcm_hw_params_get_channels(const msp_pcm_hw_params_t *params, unsigned int *val)
{
    PCM_CHECK_RETURN_EINVAL(params && val);

    *val = params->channels;
    return 0;
}

int msp_pcm_hw_params_get_format(const msp_pcm_hw_params_t *params, snd_pcm_format_t *val)
{
    PCM_CHECK_RETURN_EINVAL(params && val);

    *val = params->format;
    return 0;
}

int msp_pcm_hw_params_current(msp_pcm_t *pcm, msp_pcm_hw_params_t *params)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params && pcm->hw_params);

    memcpy(params, pcm->hw_params, sizeof(*params));
    return 0;
}

int msp_pcm_set_params(msp_pcm_t *pcm, snd_pcm_format_t format, msp_pcm_access_t acc, unsigned int channels, unsigned int rate, int soft_resample, unsigned int latency)
{
    msp_pcm_hw_params_t params;
    unsigned int buffer_size;
    unsigned int period_size;
    int sample_bits;

    (void)soft_resample;

    PCM_CHECK_RETURN_EINVAL(pcm);

    pcm_init_default_hw_params(&params);
    sample_bits = snd_pcm_format_width((snd_pcm_format_t)format);
    PCM_CHECK_RETURN_EINVAL(sample_bits > 0);
    params.format = format;
    params.sample_bits = sample_bits;
    params.access = acc;
    params.channels = channels;
    params.rate = rate;

    buffer_size = ((uint64_t)rate * latency) / 1000000ULL;
    if (buffer_size == 0) {
        buffer_size = rate / 5;
    }
    if (buffer_size == 0) {
        buffer_size = 1;
    }
    period_size = buffer_size / 4;
    if (period_size == 0) {
        period_size = 1;
    }
    if (buffer_size < period_size) {
        buffer_size = period_size;
    }

    params.buffer_size = buffer_size;
    params.period_size = period_size;
    pcm_recalc_hw_params(&params);

    return msp_pcm_hw_params(pcm, &params);
}

int msp_pcm_hw_params(msp_pcm_t *pcm, msp_pcm_hw_params_t *params)
{
    int ret;
    PCM_CHECK_RETURN_EINVAL(pcm && params && pcm->ops && pcm->ops->hw_params_set);

    pcm_recalc_hw_params(params);

    PCM_LOCK(pcm);
    ret = pcm->ops->hw_params_set(pcm, params);
    PCM_UNLOCK(pcm);
    if (ret < 0) {
        return ret;
    }

    ret = pcm_store_hw_params(pcm, params);
    if (ret < 0) {
        return ret;
    }
    pcm->state = MSP_PCM_STATE_PREPARED;

    return 0;
}

int msp_pcm_sw_params_current(msp_pcm_t *pcm, msp_pcm_sw_params_t *params)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    if (pcm->sw_params != NULL) {
        memcpy(params, pcm->sw_params, sizeof(*params));
    } else {
        pcm_init_default_sw_params(pcm, params);
    }
    return 0;
}

int msp_pcm_sw_params_set_start_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    params->start_threshold = val;

    return 0;
}

int msp_pcm_sw_params_set_stop_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    params->stop_threshold = val;

    return 0;
}

int msp_pcm_sw_params_get_start_threshold(const msp_pcm_sw_params_t *params, msp_pcm_uframes_t *val)
{
    PCM_CHECK_RETURN_EINVAL(params && val);

    *val = params->start_threshold;

    return 0;
}

int msp_pcm_sw_params(msp_pcm_t *pcm, msp_pcm_sw_params_t *params)
{
    PCM_CHECK_RETURN_EINVAL(pcm && params);

    return pcm_store_sw_params(pcm, params);
}

msp_pcm_sframes_t msp_pcm_writei(msp_pcm_t *pcm, const void *buffer, msp_pcm_uframes_t size)
{
    int ret = 0;
    int written = 0;
    int w_size;
    unsigned int actl_flags = 0;
    char *send;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->write && pcm->hw_params &&
                            pcm->stream == MSP_PCM_STREAM_PLAYBACK &&
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_INTERLEAVED);

    PCM_LOCK(pcm);

    w_size = msp_pcm_frames_to_bytes(pcm, size);
    send = (char *)buffer;

    while (w_size) {
        ret = pcm->ops->write(pcm, (void *)(send), w_size);

        if (ret < 0) {
            break;
        }

        written += ret;
        w_size -= ret;
        send += ret;
        if (w_size == 0) {
            break;
        }

        actl_flags = 0;
        msp_event_get(&pcm->evt, PCM_EVT_WRITE | PCM_EVT_XRUN, MSP_EVENT_OR_CLEAR, &actl_flags, MSP_WAIT_FOREVER);
        if ((actl_flags & PCM_EVT_XRUN) == PCM_EVT_XRUN) {
            LOGW(TAG,"pcm write PCM_EVT_XRUN\r\n");
            ret = -EPIPE;
            break;
        }
        if (ret == 0 && (actl_flags & PCM_EVT_WRITE) == 0) {
            ret = -EIO;
            break;
        }
    }

    PCM_UNLOCK(pcm);
    if (ret < 0) {
        return ret;
    }

    return msp_pcm_bytes_to_frames(pcm, written);
}

msp_pcm_sframes_t msp_pcm_avail(msp_pcm_t *pcm)
{
    int bytes;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->hw_get_remain_size &&
                            pcm->hw_params && pcm->stream == MSP_PCM_STREAM_CAPTURE);

    PCM_LOCK(pcm);
    bytes = pcm->ops->hw_get_remain_size(pcm);
    PCM_UNLOCK(pcm);

    return (msp_pcm_bytes_to_frames(pcm, bytes));
}

msp_pcm_sframes_t msp_pcm_readi(msp_pcm_t *pcm, void *buffer, msp_pcm_uframes_t size)
{
    int bytes;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->read && pcm->hw_params &&
                            pcm->stream == MSP_PCM_STREAM_CAPTURE &&
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_INTERLEAVED);

    PCM_LOCK(pcm);
    bytes = pcm->ops->read(pcm, buffer, msp_pcm_frames_to_bytes(pcm, size));
    PCM_UNLOCK(pcm);

    if (bytes <= 0) {
        return bytes;
    }

    return (msp_pcm_bytes_to_frames(pcm, bytes));
}

msp_pcm_sframes_t msp_pcm_readn(msp_pcm_t *pcm, void **bufs, msp_pcm_uframes_t size)
{
    int bytes;
    void *buffer;
    int i;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->read && pcm->hw_params && bufs &&
                            pcm->stream == MSP_PCM_STREAM_CAPTURE &&
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_NONINTERLEAVED &&
                            bufs[0] != NULL);

    for (i = 0; i < pcm->hw_params->channels; i++) {
        PCM_CHECK_RETURN_EINVAL(bufs[i] != NULL);
    }

    bytes = msp_pcm_frames_to_bytes(pcm, size);
    if (bytes == 0) {
        return 0;
    }
    buffer = msp_zalloc(bytes);
    PCM_CHECK_RETURN_ENOMEM(buffer);

    PCM_LOCK(pcm);
    bytes = pcm->ops->read(pcm, buffer, bytes);
    PCM_UNLOCK(pcm);

    if (bytes <= 0) {
        msp_free(buffer);
        return bytes;
    }

    pcm_deinterleave(pcm, buffer, bufs, bytes);
    msp_free(buffer);

    return msp_pcm_bytes_to_frames(pcm, bytes);
}

int msp_pcm_hw_params_malloc(msp_pcm_hw_params_t **p)
{
    PCM_CHECK_RETURN_EINVAL(p);

    *p = msp_zalloc_check(sizeof(msp_pcm_hw_params_t));
    return 0;
}

void msp_pcm_hw_params_free(msp_pcm_hw_params_t *obj)
{
    if (obj != NULL) {
        msp_free(obj);
    }

    obj = NULL;
}

int msp_pcm_hw_params_alloca(msp_pcm_hw_params_t **p)
{
    PCM_CHECK_RETURN_EINVAL(p);

    *p = msp_zalloc_check(sizeof(msp_pcm_hw_params_t));

    return 0;
}

int msp_pcm_sw_params_alloca(msp_pcm_sw_params_t **p)
{
    PCM_CHECK_RETURN_EINVAL(p);

    *p = msp_zalloc_check(sizeof(msp_pcm_sw_params_t));

    return 0;
}

void msp_pcm_sw_params_free(msp_pcm_sw_params_t *obj)
{
    if (obj != NULL) {
        msp_free(obj);
    }
}

int msp_pcm_drop(msp_pcm_t *pcm)
{
    (void)pcm;

    return 0;
}

int msp_pcm_drain(msp_pcm_t *pcm)
{
    //FIXME
    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->hw_params);

    pcm->state = MSP_PCM_STATE_DRAINING;
    if (pcm->state == MSP_PCM_STATE_DRAINING && pcm->hw_params != NULL && pcm->stream == MSP_PCM_STREAM_PLAYBACK) {
        if (pcm->ops->hw_get_remain_size) {
            int size;

            while(1) {
                size = pcm->ops->hw_get_remain_size(pcm);

                if (size) {
                    int lli_size;
                    msp_pcm_hw_params_t *params = pcm->hw_params;
                    lli_size = (params->rate / 1000) * pcm_frame_bytes_from_params(params);
                    PCM_CHECK_RETURN_EINVAL(lli_size > 0);
                    msp_msleep((size / lli_size) + 1);
                } else {
                    break;
                }
            }
        } else {
            unsigned int actl_flags;

            msp_event_get(&pcm->evt, PCM_EVT_XRUN, MSP_EVENT_OR_CLEAR, &actl_flags, MSP_WAIT_FOREVER);
        }
    }

    return 0;
}

int msp_pcm_pause(msp_pcm_t *pcm, int enable)
{
    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->pause);

    PCM_LOCK(pcm);
    pcm->ops->pause(pcm, enable);
    PCM_UNLOCK(pcm);

    return 0;
}

int msp_pcm_wait(msp_pcm_t *pcm, int timeout)
{
    int ret;
    unsigned int actl_flags = 0;
    int i = 0;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->hw_get_remain_size && pcm->hw_params);
    PCM_CHECK_RETURN_EINVAL(hw_params(pcm)->period_bytes > 0);

    do {
        PCM_LOCK(pcm);

        ret = pcm->ops->hw_get_remain_size(pcm);
        PCM_UNLOCK(pcm);

        if (ret >= hw_params(pcm)->period_bytes) {
            return 0;
        }

        if (i > 0) {
            LOGW(TAG, "pcm wait(%d) size(%d)<%d,wait again\r\n", i, ret, hw_params(pcm)->period_bytes);
        }
        i++;

        actl_flags = 0;
        ret = msp_event_get(&pcm->evt, PCM_EVT_READ | PCM_EVT_XRUN, MSP_EVENT_OR_CLEAR, &actl_flags, timeout);

        if (actl_flags & PCM_EVT_XRUN) {
            LOGW(TAG,"pcm read PCM_EVT_XRUN\r\n");
            return -EPIPE;
        }
        if (ret < 0) {
            return -ETIMEDOUT;
        }
    } while(1);

    return 0;
}

msp_pcm_sframes_t msp_pcm_bytes_to_frames(msp_pcm_t *pcm, ssize_t bytes)
{
    int frame_bytes;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->hw_params);

    frame_bytes = pcm_frame_bytes_from_params(hw_params(pcm));
    PCM_CHECK_RETURN_EINVAL(frame_bytes > 0);

    return (bytes / frame_bytes);
}

ssize_t msp_pcm_frames_to_bytes(msp_pcm_t *pcm, msp_pcm_sframes_t frames)
{
    int frame_bytes;

    PCM_CHECK_RETURN_EINVAL(pcm && pcm->hw_params);

    frame_bytes = pcm_frame_bytes_from_params(hw_params(pcm));
    PCM_CHECK_RETURN_EINVAL(frame_bytes > 0);

    return (frames * frame_bytes);
}

void msp_pcm_set_ops(msp_pcm_t *pcm, int direction, struct msp_pcm_ops *ops)
{
    (void)direction;

    msp_check_param(pcm && ops);

    pcm->ops = ops;
}

int msp_pcm_recover(msp_pcm_t *pcm, int err, int silent)
{
    if (err == (-EPIPE)) {
        int ret;
        msp_dev_t *dev;

        PCM_CHECK_RETURN_EINVAL(pcm && pcm->ops && pcm->ops->hw_params_set && pcm->hw_params);

        if (pcm->stream == MSP_PCM_STREAM_CAPTURE) {
            if (silent) {
                LOGD(TAG, "overrun occurred");
            }
        } else {
            if (silent) {
                LOGD(TAG, "underrun occurred");
            }
        }
        PCM_LOCK(pcm);
        dev = pcm_get_dev(pcm);
        if (dev != NULL) {
            msp_device_close(dev);
        }
        msp_event_set(&pcm->evt, 0, MSP_EVENT_AND);
        if (dev != NULL && msp_device_open(pcm->pcm_name) == NULL) {
            PCM_UNLOCK(pcm);
            return -EIO;
        }
        ret = pcm->ops->hw_params_set(pcm, pcm->hw_params);
        PCM_UNLOCK(pcm);
        if (ret < 0) {
            return ret;
        }

        return 0;
    }

    return err;
}
