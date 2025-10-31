/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <xutils/xringbuffer.h>
#include "msp/kernel.h"
#include "recio.h"
#include "uri_parse.h"
#include "xutils/debug.h"
#define TAG "rec-ringb"

typedef struct _ringb_conf {
    x_ringbuf_t *handle;
} ringb_conf_t;

static int ringb_open(recio_t *io, const char *path)
{
    uint32_t value;

    ringb_conf_t *priv = msp_zalloc_check(sizeof(ringb_conf_t));
    if (priv == NULL) {
        LOGE(TAG, "mem open e,mem out");
        return -1;
    }

    if (uri_get_item_value_int(path, "handle", &value) == 0) {
        priv->handle = (x_ringbuf_t *)value;
    } else {
        goto err;
    }
    // LOGD(TAG, "priv->handle: 0x%x", priv->handle);
    io->private = (void *)priv;
    return 0;    
err:
    msp_free(priv);
    return -1;
}

static int ringb_close(recio_t *io)
{
    if (io->private) {
        msp_free(io->private);
    }
    return 0;
}

static int ringb_read(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    ringb_conf_t *priv = (ringb_conf_t *)io->private;

    int ret = xringbuffer_read(priv->handle, buffer, length);
    // LOGD(TAG, "22ringb r len:%d, ret:%d", length, ret);
    return ret;
}

static int ringb_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    ringb_conf_t *priv = (ringb_conf_t *)io->private;

    return xringbuffer_write(priv->handle, buffer, length);
}

static int ringb_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

const recio_cls_t ringb = {
    .name = "ringbuffer",
    .open = ringb_open,
    .close = ringb_close,
    .write = ringb_write,
    .read = ringb_read,
    .seek = ringb_seek,
};

int recio_register_ringb(const char *path)
{
    // LOGD(TAG, "%s, %d, path:%s", __func__, __LINE__, path);

    return recio_register(&ringb, path);
}
