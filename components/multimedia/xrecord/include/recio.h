/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __RECIO_H__
#define __RECIO_H__

#include <stdio.h>
#include <stdint.h>
#include <xutils/debug.h>

typedef struct recio_cls recio_cls_t;

typedef struct {
    const recio_cls_t *cls;
    void *private;
} recio_t;

struct recio_cls {
    const char *name;
    int (*open)(recio_t *io, const char *path);
    int (*close)(recio_t *io);

    int (*read)(recio_t *io, uint8_t *buffer, int length, int timeoutms);
    int (*write)(recio_t *io, uint8_t *buffer, int length, int timeoutms);
    int (*remove)(recio_t *io);
    int (*seek)(recio_t *io, size_t offset, int whence);
};

int recio_register(const recio_cls_t *cls, const char *path);
int recio_unregister(const char *path);

int recio_register_ws(const char *path);
int recio_register_mem(const char *path);
int recio_register_ringb(const char *path);
int recio_register_sdbuf(const char *path);
int recio_register_mic(const char *path);
int recio_register_null(const char *path);

recio_t *recio_open(const char *path);
int recio_close(recio_t *io);
int recio_read(recio_t *io, uint8_t *buffer, size_t lenght, int timeoutms);
int recio_write(recio_t *io, uint8_t *buffer, size_t lenght, int timeoutms);
int recio_seek(recio_t *io, size_t offset, int whence);

#endif
