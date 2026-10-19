/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <msp/msp.h>
#include "recio.h"
#include "uri_parse.h"

#define TAG "rec-mem"

typedef struct _mem_conf {
    uint32_t    address;
    int         size;
} mem_conf_t;

static int mem_open(recio_t *io, const char *path)
{
    int value;

    mem_conf_t *priv = msp_zalloc_check(sizeof(mem_conf_t));
    if (priv == NULL) {
        LOGE(TAG, "mem open e,mem out");
        return -1;
    }

    if (uri_get_item_value_int(path, "size", &value) == 0) {
        priv->size = value;
    } else {
        goto err;
    }
    if (uri_get_item_value_int(path, "address", &value) == 0) {
        priv->address = value;
    } else {
        goto err;
    }
    LOGD(TAG, "priv->address: 0x%x, %d", priv->address, priv->size);
    io->private = (void *)priv;
    return 0;
err:
    msp_free(priv);
    return -1;
}

static int mem_close(recio_t *io)
{
    if (io->private) {
        msp_free(io->private);
    }
    return 0;
}

static int mem_read(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    mem_conf_t *priv = (mem_conf_t *)io->private;

    memcpy(buffer, (void *)priv->address, priv->size);
    LOGD(TAG, "mem read:0x%x, %d", priv->address, priv->size);
    return priv->size;
}

static int mem_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return -1;
}

static int mem_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

const recio_cls_t mem = {
    .name = "mem",
    .open = mem_open,
    .close = mem_close,
    .write = mem_write,
    .read = mem_read,
    .seek = mem_seek,
};

int recio_register_mem(const char *path)
{
    LOGD(TAG, "%s, %d, path:%s", __func__, __LINE__, path);

    return recio_register(&mem, path);
}