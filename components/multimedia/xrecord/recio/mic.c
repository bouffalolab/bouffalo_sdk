/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <xutils/xringbuffer.h>
#include "msp/kernel.h"
#include "recio.h"
#include "uri_parse.h"
#include <alsa/pcm.h>
#include "xutils/debug.h"
#define TAG "rec-mic"

typedef struct _mic_dev {
	msp_pcm_t *pcmC;
	int format;
	int sample_rate;
	int chn_num;
	int frame_ms;
	uint8_t *pcm_data;
} mic_dev_t;

static msp_pcm_t *__capture_init(const char *devname, unsigned int sample_rate /*16000*/, int chn_num,
                               int bit_format /*16bit*/, msp_pcm_uframes_t peroid_size)
{
    msp_pcm_hw_params_t *params;
    msp_pcm_t           *pcm = NULL;

    int err = 0;

    err = msp_pcm_open(&pcm, devname, MSP_PCM_STREAM_CAPTURE, 0);

    if (err < 0) {
        LOGE(TAG, "msp_pcm_open %s error", devname);
        return NULL;
    }

    msp_pcm_hw_params_alloca(&params);
    err = msp_pcm_hw_params_any(pcm, params);

    if (err < 0) {
        LOGE(TAG, "Broken configuration for this PCM: no configurations available");
        msp_pcm_close(pcm);
        return NULL;
    }

    err = msp_pcm_hw_params_set_access(pcm, params, MSP_PCM_ACCESS_RW_NONINTERLEAVED);

    if (err < 0) {
        LOGE(TAG, "Access type not available");
        msp_pcm_close(pcm);
        return NULL;
    }

    err = msp_pcm_hw_params_set_format(pcm, params, bit_format);

    if (err < 0) {
        LOGE(TAG, "Sample bit_format non available");
        msp_pcm_close(pcm);
        return NULL;
    }

    err = msp_pcm_hw_params_set_channels(pcm, params, chn_num);

    if (err < 0) {
        LOGE(TAG, "Channels count non available");
        msp_pcm_close(pcm);
        return NULL;
    }

    msp_pcm_hw_params_set_rate_near(pcm, params, &sample_rate, 0);

    msp_pcm_uframes_t val_peroid_size = peroid_size;
    msp_pcm_hw_params_set_period_size_near(pcm, params, &val_peroid_size, 0);

    msp_pcm_uframes_t val_buffer_frames = val_peroid_size * 23; /*buffer保存16个frame*/
    msp_pcm_hw_params_set_buffer_size_near(pcm, params, &val_buffer_frames);

    err = msp_pcm_hw_params(pcm, params);

    if (err < 0) {
        LOGE(TAG, "msp_pcm_hw_params error");
        msp_pcm_close(pcm);
        return NULL;
    }

    return pcm;
}

static int __mic_pcm_open(recio_t *io, const char *path)
{
    uint32_t value;

    mic_dev_t *priv = msp_zalloc_check(sizeof(mic_dev_t));
    if (priv == NULL) {
        LOGE(TAG, "mem open e,mem out");
        return -1;
    }

    /*
     * mic://format=16&sample=16000&chan=1&frame_ms=20
     */

    if (uri_get_item_value_int(path, "format", &value) != 0) {
        goto err;
    }
    priv->format = value;

    if (uri_get_item_value_int(path, "sample", &value) != 0) {
        goto err;
    }
    priv->sample_rate = value;

    if (uri_get_item_value_int(path, "chan", &value) != 0) {
        goto err;
    }
    priv->chn_num = value;

    if (uri_get_item_value_int(path, "frame_ms", &value) != 0) {
        goto err;
    }
    priv->frame_ms = value;

    /* Single frame single channel sample count */
    msp_pcm_uframes_t peroid_size = priv->frame_ms * (priv->sample_rate / 1000);

    priv->pcmC = __capture_init("pcmC0", priv->sample_rate, priv->chn_num, priv->format, peroid_size);

    if (priv->pcmC == NULL) {
        goto err;
    }

    //ssize_t capture_byte = msp_pcm_frames_to_bytes(priv->pcmC, peroid_size);

    io->private = (void *)priv;
    return 0;
err:
    msp_free(priv);
    return -1;
}

static int __mic_pcm_close(recio_t *io)
{
    if (io->private) {
        msp_free(io->private);
    }
    return 0;
}

static int __mic_pcm_read(recio_t *io, uint8_t *buffer, int len, int timeoutms)
{
	mic_dev_t *priv = (mic_dev_t *)io->private;
    int rlen = 0;
    int ret = 0;

    if (priv == NULL || priv->pcmC == NULL) {
        return -1;
    }

    while (msp_pcm_avail(priv->pcmC) < msp_pcm_bytes_to_frames(priv->pcmC, len)) {
        ret = msp_pcm_wait(priv->pcmC, MSP_WAIT_FOREVER);
    }

    if (ret < 0) {
        static long long last_time = 0;

        long long now = msp_now_ms();
        if (now - last_time > 3000) {
            LOGW(TAG, "pcm read XRUN\r\n");
            last_time = now;
        }
        return 0;
    }

    rlen = msp_pcm_readn(priv->pcmC, (void**)buffer, msp_pcm_bytes_to_frames(priv->pcmC, len));
    rlen = msp_pcm_frames_to_bytes(priv->pcmC, rlen);

    return rlen;
}

static int __mic_pcm_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return 0;
}

static int __mic_pcm_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

static const recio_cls_t __g_mic = {
    .name = "mic_pcm",
    .open = __mic_pcm_open,
    .close = __mic_pcm_close,
    .write = __mic_pcm_write,
    .read = __mic_pcm_read,
    .seek = __mic_pcm_seek,
};

int recio_register_mic(const char *path)
{
    return recio_register(&__g_mic, path);
}
