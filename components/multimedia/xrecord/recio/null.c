/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "recio.h"
#include "uri_parse.h"

#define TAG "rec-null"

static int __null_open(recio_t *io, const char *path)
{
    return 0;    
}

static int __null_close(recio_t *io)
{
    return 0;
}

static int __null_read(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return 0;
}

static int __null_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return 0;
}

static int __null_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

static const recio_cls_t __null_cls = {
    .name = "null",
    .open = __null_open,
    .close = __null_close,
    .write = __null_write,
    .read = __null_read,
    .seek = __null_seek,
};

int recio_register_null(const char *path)
{
    LOGD(TAG, "%s, %d, path:%s", __func__, __LINE__, path);

    return recio_register(&__null_cls, path);
}
