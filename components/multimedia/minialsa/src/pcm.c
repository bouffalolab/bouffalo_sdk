/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
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

#define hw_params(pcm) pcm->hw_params
#define sw_params(pcm) pcm->sw_params

typedef struct {
    char *channel;
    int offset;
} pcm_access_t;

static void pcm_event(msp_pcm_t *pcm, int event_id, void *priv)
{
    if (event_id == PCM_EVT_XRUN) {
        msp_event_set(&pcm->evt, 0, MSP_EVENT_AND);
    }

    msp_event_set(&pcm->evt, event_id, MSP_EVENT_OR);
}

int msp_pcm_new(msp_pcm_t **pcm_ret, int type, const char *name, msp_pcm_stream_t stream, int mode)
{
    msp_pcm_t *pcm = msp_calloc_check(sizeof(msp_pcm_t), 1);

    pcm->pcm_name = name;
    pcm->mode = mode;
    pcm->stream = stream;
    *pcm_ret = pcm;

    return 0;
}

int msp_pcm_open(msp_pcm_t **pcm_ret, const char *name, msp_pcm_stream_t stream, int mode)
{
    msp_pcm_t *pcm;
    card_dev_t *card;

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
    msp_mutex_new(&pcm->mutex);
    msp_event_new(&pcm->evt, 0);
    pcm->stream = stream;
    if(mode == 0) {
        pcm->event.cb = pcm_event;
        pcm->event.priv = NULL;
    }
    pcm->mode   = mode;
    //xringbuffer_create(&pcm->ringbuffer, msp_zalloc_check(1024), 1024);
    pcm->state = MSP_PCM_STATE_OPEN;
    pcm->pcm_name = name;
    *pcm_ret = pcm;
    return 0;
}

int msp_pcm_close(msp_pcm_t *pcm)
{
    msp_check_return_einval(pcm);

    //LOGE(TAG, "pcm close");
    //FIXME: close pcm-device first
    msp_dev_t *dev = (msp_dev_t *)(((int)(pcm)) + sizeof(msp_pcm_t) - sizeof(msp_pcm_dev_t));

    msp_device_close(dev);

    msp_mutex_free(&pcm->mutex);
    msp_event_free(&pcm->evt);

    if (pcm->hw_params) {
        msp_free(pcm->hw_params);
    }

    if (pcm->sw_params) {
        msp_free(pcm->sw_params);
    }

    //msp_free(pcm->ringbuffer.buffer);
    //xringbuffer_destroy(&pcm->ringbuffer);

    return 0;
}

int msp_pcm_hw_params_any(msp_pcm_t *pcm, msp_pcm_hw_params_t *params)
{
    msp_check_return_einval(pcm);

    params->access      = MSP_PCM_ACCESS_RW_INTERLEAVED;
    params->channels    = 2;
    params->rate        = 16000;
    params->sample_bits = 16;

    pcm->hw_params = params;
    return 0;
}

int msp_pcm_hw_params_set_access(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_access_t _access)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->access = _access;

    return 0;
}

int msp_pcm_hw_params_set_format(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, int val)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->format = val;

    return 0;
}

int msp_pcm_hw_params_set_channels(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int val)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->channels = val;

    return 0;
}

int msp_pcm_hw_params_set_rate_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->rate = *val;

    return 0;
}

int msp_pcm_hw_params_set_buffer_time_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, unsigned int *val, int *dir)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->buffer_time = *val;

    return 0;
}

int msp_pcm_hw_params_set_buffer_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->buffer_size = *val;

    return 0;
}

int msp_pcm_hw_params_set_period_size_near(msp_pcm_t *pcm, msp_pcm_hw_params_t *params, msp_pcm_uframes_t *val, int *dir)
{
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->period_size = *val;

    return 0;
}

int msp_pcm_set_params(msp_pcm_t *pcm, int format, msp_pcm_access_t acc, unsigned int channels, unsigned int rate, int soft_resample, unsigned int latency)
{
    msp_check_return_einval(pcm);

    hw_params(pcm)->format = format;
    hw_params(pcm)->access = acc;
    hw_params(pcm)->channels = channels;
    hw_params(pcm)->rate = rate;

    PCM_LOCK(pcm);
    pcm->ops->hw_params_set(pcm, pcm->hw_params);
    pcm->state = MSP_PCM_STATE_PREPARED;
    PCM_UNLOCK(pcm);

    return 0;
}

int msp_pcm_hw_params(msp_pcm_t *pcm, msp_pcm_hw_params_t *params)
{
    int ret;
    msp_check_return_einval(pcm && params);

    hw_params(pcm)->period_bytes = (hw_params(pcm)->period_size * ( hw_params(pcm)->format / 8)) * hw_params(pcm)->channels;
    hw_params(pcm)->buffer_bytes = (hw_params(pcm)->buffer_size * ( hw_params(pcm)->format / 8)) * hw_params(pcm)->channels;

    if (pcm->ops == NULL) {
        return -1;
    }

    PCM_LOCK(pcm);
    ret = pcm->ops->hw_params_set(pcm, params);
    pcm->state = MSP_PCM_STATE_PREPARED;
    PCM_UNLOCK(pcm);

    return ret;
}

int msp_pcm_sw_params_current(msp_pcm_t *pcm, msp_pcm_sw_params_t *params)
{
    msp_check_return_einval(pcm && params);

    pcm->sw_params = params;
    return 0;
}

int msp_pcm_sw_params_set_start_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val)
{
    msp_check_return_einval(pcm && params);

    sw_params(pcm)->start_threshold = val;

    return 0;
}

int msp_pcm_sw_params_set_stop_threshold(msp_pcm_t *pcm, msp_pcm_sw_params_t *params, msp_pcm_uframes_t val)
{
    msp_check_return_einval(pcm && params);

    sw_params(pcm)->stop_threshold = val;

    return 0;
}

int msp_pcm_sw_params(msp_pcm_t *pcm, msp_pcm_sw_params_t *params)
{
    msp_check_return_einval(pcm && params);

    return 0;
}

msp_pcm_sframes_t msp_pcm_writei(msp_pcm_t *pcm, const void *buffer, msp_pcm_uframes_t size)
{
    msp_check_return_einval(pcm && pcm->stream == MSP_PCM_STREAM_PLAYBACK && \
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_INTERLEAVED);

    PCM_LOCK(pcm);

    int ret = -1;
    int w_size = msp_pcm_frames_to_bytes(pcm, size);
    unsigned int actl_flags = 0;
    char *send = (char *)buffer;

    while (w_size) {
        ret = pcm->ops->write(pcm, (void *)(send), w_size);
        if (ret < w_size) {
            msp_event_get(&pcm->evt, PCM_EVT_WRITE | PCM_EVT_XRUN, MSP_EVENT_OR_CLEAR, &actl_flags, MSP_WAIT_FOREVER);
        } else {
            msp_event_get(&pcm->evt, PCM_EVT_WRITE | PCM_EVT_XRUN, MSP_EVENT_OR_CLEAR, &actl_flags, 0);
        }
        if ((actl_flags & PCM_EVT_XRUN) == PCM_EVT_XRUN) {
            LOGW(TAG,"pcm write PCM_EVT_XRUN\r\n");
            ret = -EPIPE;
            break;
        }

        w_size -= ret;
        send += ret;
    }

    PCM_UNLOCK(pcm);
    if (ret < 0) {
        return ret;
    }
    return msp_pcm_bytes_to_frames(pcm, ret);
}

static void pcm_access(msp_pcm_t *pcm, void *buffer, int bytes)
{
    char *recv = msp_zalloc_check(bytes);
    pcm_access_t *c = msp_zalloc_check(pcm->hw_params->channels * sizeof(pcm_access_t));
    int channel_size = bytes / pcm->hw_params->channels;
    int frame_size = pcm->hw_params->format / 8;

    memcpy(recv, buffer, bytes);
    for (int i = 0; i < pcm->hw_params->channels; i++) {
        (c+i)->channel = recv + channel_size * i;
        (c+i)->offset  = 0;
    }

    char *dec = (char *)buffer;
    int offset = 0;

    while (offset < bytes) {
        for (int j = 0; j < pcm->hw_params->channels; j++) {
            pcm_access_t *p = (c + j);

            memcpy(dec + offset, p->channel + p->offset, frame_size);
            offset += frame_size;
            p->offset += frame_size;
        }
    }

    msp_free(recv);
    msp_free(c);
}

msp_pcm_sframes_t msp_pcm_avail(msp_pcm_t *pcm)
{
    msp_check_return_einval(pcm && pcm->stream == MSP_PCM_STREAM_CAPTURE && \
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_NONINTERLEAVED);

    PCM_LOCK(pcm);
    int bytes = pcm->ops->hw_get_remain_size(pcm);
    PCM_UNLOCK(pcm);

    return (msp_pcm_bytes_to_frames(pcm, bytes));
}

msp_pcm_sframes_t msp_pcm_readi(msp_pcm_t *pcm, void *buffer, msp_pcm_uframes_t size)
{
    msp_check_return_einval(pcm && pcm->stream == MSP_PCM_STREAM_CAPTURE && \
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_INTERLEAVED);

    PCM_LOCK(pcm);
    int bytes = pcm->ops->read(pcm, buffer, msp_pcm_frames_to_bytes(pcm, size));
    PCM_UNLOCK(pcm);

    if (bytes <= 0) {
        return 0;
    }


    pcm_access(pcm, buffer, bytes);

    return (msp_pcm_bytes_to_frames(pcm, bytes));
}

msp_pcm_sframes_t msp_pcm_readn(msp_pcm_t *pcm, void **bufs, msp_pcm_uframes_t size)
{
    msp_check_return_einval(pcm && pcm->stream == MSP_PCM_STREAM_CAPTURE && \
                            pcm->hw_params->access == MSP_PCM_ACCESS_RW_NONINTERLEAVED);

    PCM_LOCK(pcm);
    int bytes = pcm->ops->read(pcm, (void *)bufs, msp_pcm_frames_to_bytes(pcm, size));
    PCM_UNLOCK(pcm);

    return (msp_pcm_bytes_to_frames(pcm, bytes));
}

int msp_pcm_hw_params_alloca(msp_pcm_hw_params_t **p)
{
    *p = msp_zalloc_check(sizeof(msp_pcm_hw_params_t));

    return 0;
}

int msp_pcm_sw_params_alloca(msp_pcm_sw_params_t **p)
{
    *p = msp_zalloc_check(sizeof(msp_pcm_sw_params_t));

    return 0;
}

int msp_pcm_drop(msp_pcm_t *pcm)
{

    return 0;
}

int msp_pcm_drain(msp_pcm_t *pcm)
{
    //FIXME
    msp_check_return_einval(pcm);

    pcm->state = MSP_PCM_STATE_DRAINING;
    if (pcm->state == MSP_PCM_STATE_DRAINING && pcm->hw_params != NULL && pcm->stream == MSP_PCM_STREAM_PLAYBACK) {
        if (pcm->ops->hw_get_remain_size) {
            int size;

            while(1) {
                size = pcm->ops->hw_get_remain_size(pcm);

                if (size) {
                    int lli_size;
                    msp_pcm_hw_params_t *params = pcm->hw_params;
                    lli_size = (params->rate / 1000) * (params->sample_bits / 8) * params->channels;
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
    msp_check_return_einval(pcm);

    PCM_LOCK(pcm);
    pcm->ops->pause(pcm, enable);
    PCM_UNLOCK(pcm);

    return 0;
}

int msp_pcm_wait(msp_pcm_t *pcm, int timeout)
{
    msp_check_return_einval(pcm);
    unsigned int actl_flags = 0;
    int i = 0;

    do {
        PCM_LOCK(pcm);

        int ret = 0;
        if (pcm->ops->hw_get_remain_size) {
            ret = pcm->ops->hw_get_remain_size(pcm);

            if (ret >= (hw_params(pcm)->period_bytes) ) {
                PCM_UNLOCK(pcm);
                //msp_event_set(&pcm->evt, 0, MSP_EVENT_AND);
                return 0;
            }
        }

        if (i > 0) {
            LOGW(TAG, "pcm wait(%d) size(%d)<%d,wait again\r\n", i, ret, hw_params(pcm)->period_bytes);
        }
        i++;

        msp_event_get(&pcm->evt, PCM_EVT_READ | PCM_EVT_XRUN, MSP_EVENT_OR_CLEAR, &actl_flags, timeout);

        PCM_UNLOCK(pcm);

        if (actl_flags & PCM_EVT_XRUN) {
            LOGW(TAG,"pcm read PCM_EVT_XRUN\r\n");
            //msp_event_set(&pcm->evt, 0, MSP_EVENT_AND);
            return -EPIPE;
        }
    } while(1);

    return 0;
}

msp_pcm_sframes_t msp_pcm_bytes_to_frames(msp_pcm_t *pcm, ssize_t bytes)
{
    msp_check_return_einval(pcm);

    return (bytes / (hw_params(pcm)->channels * hw_params(pcm)->format / 8));
}

ssize_t msp_pcm_frames_to_bytes(msp_pcm_t *pcm, msp_pcm_sframes_t frames)
{
    msp_check_return_einval(pcm);

    return (frames * (hw_params(pcm)->channels * hw_params(pcm)->format / 8));
}

void msp_pcm_set_ops(msp_pcm_t *pcm, int direction, struct msp_pcm_ops *ops)
{
    msp_check_param(pcm && ops);

    pcm->ops = ops;
}

int msp_pcm_recover(msp_pcm_t *pcm, int err, int silent)
{
    if (err == (-EPIPE)) {
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
        msp_dev_t *dev = (msp_dev_t *)(((int)(pcm)) + sizeof(msp_pcm_t) - sizeof(msp_pcm_dev_t));

        msp_device_close(dev);
        msp_event_set(&pcm->evt, 0, MSP_EVENT_AND);
        msp_device_open(pcm->pcm_name);
        pcm->ops->hw_params_set(pcm, pcm->hw_params);
        PCM_UNLOCK(pcm);

        return 0;
    }

    return err;
}
