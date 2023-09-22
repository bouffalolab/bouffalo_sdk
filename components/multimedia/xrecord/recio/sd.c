/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <vfs.h>
#include <msp/kernel.h>
#include "recio.h"
#include "uri_parse.h"

#define TAG "rec-sd"

typedef struct _sdbuf_conf {
    int wtfd;
    int rdfd;
} sdbuf_conf_t;

static int sdbuf_open(recio_t *io, const char *path)
{
    char filename[128] = {0};

    sdbuf_conf_t *priv = msp_zalloc_check(sizeof(sdbuf_conf_t));
    if (priv == NULL) {
        LOGE(TAG, "mem open e,mem out");
        return -1;
    }

    if (uri_get_item_value(path, "handle", filename, 128) == 0) {
        LOGD(TAG, "sdbuf open file %s", filename);
        // priv->wtfd = msp_open(filename, O_WRONLY | O_CREAT | O_TRUNC);
        priv->wtfd = 0;
        priv->rdfd = msp_open(filename, O_RDONLY);

        if (priv->wtfd < 0 || priv->rdfd < 0) {
            LOGW(TAG, "open file failed %d %d", priv->rdfd, priv->wtfd);
            goto err;
        }
    } else {
        goto err;
    }
    io->private = (void *)priv;
    return 0;

err:
    msp_free(priv);
    return -1;
}

static int sdbuf_close(recio_t *io)
{
    sdbuf_conf_t *priv = (sdbuf_conf_t *)io->private;

    if (priv) {
        msp_close(priv->wtfd);
        msp_close(priv->rdfd);
    }

    if (io->private) {
        msp_free(io->private);
    }
    return 0;
}

static int sdbuf_read(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    sdbuf_conf_t *priv = (sdbuf_conf_t *)io->private;

    int rnum;
    rnum = msp_read(priv->rdfd, buffer, length);
    if (rnum <= 0) {
        return -1;
    }

    return rnum;
}

static int sdbuf_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    // sdbuf_conf_t *priv = (sdbuf_conf_t *)io->private;
    return 0;
}

static int sdbuf_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

const recio_cls_t sd_op = {
    .name = "sdbuffer",
    .open = sdbuf_open,
    .close = sdbuf_close,
    .write = sdbuf_write,
    .read = sdbuf_read,
    .seek = sdbuf_seek,
};

int recio_register_sdbuf(const char *path)
{
    // LOGD(TAG, "%s, %d, path:%s", __func__, __LINE__, path);
    return recio_register(&sd_op, path);
}