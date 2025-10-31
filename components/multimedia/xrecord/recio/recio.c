/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <xutils/list.h>
#include "recio.h"
#include <stdlib.h>
#include <msp/kernel.h>

#define TAG "recio"

typedef struct record_recio_list {
    msp_slist_t next;
    const recio_cls_t *cls;
    char *id;
} recio_node_t;

static MSP_SLIST_HEAD(recio_cls_list);

int recio_register(const recio_cls_t *cls, const char *path)
{
    recio_node_t *node = malloc(sizeof(recio_node_t));

    if (node) {
        node->cls = cls;
        node->id = strdup(path);
        msp_slist_add_tail(&node->next, &recio_cls_list);
        return 0;
    }

    return -1;
}

int recio_unregister(const char *path)
{
    recio_node_t *node;

    msp_slist_for_each_entry(&recio_cls_list, node, recio_node_t, next) {

        if (strcmp(node->id, path) == 0) {
            LOGD(TAG, "%s, %s", __func__, path);
            msp_slist_del(&node->next, &recio_cls_list);
            msp_free(node->id);
            msp_free(node);
            break;
        }
    }

    return 0;
}

recio_t *recio_open(const char *path)
{
    recio_t *io = NULL;

    LOGD(TAG, "%s, path: %s", __func__, path);
    if (path) {
        recio_node_t *node;
        
        msp_slist_for_each_entry(&recio_cls_list, node, recio_node_t, next) {
            if (strcmp(node->id, path) == 0) {
                if (node->cls->open) {
                    io = msp_zalloc(sizeof(recio_t));
                    io->cls = node->cls;
                    if (io->cls->open(io, path) < 0) {
                        msp_free(io);
                        LOGD(TAG, "open fail[%s]", node->id);
                        return NULL;
                    }
                }
                LOGD(TAG, "open break [%s]",node->id);
                break;
            }
        }
    }

    return io;
}

int recio_close(recio_t *io)
{
    int ret = -1;
    if (io->cls->close) {
        ret = io->cls->close(io);
        msp_free(io);
    }

    return ret;
}

int recio_read(recio_t *io, uint8_t *buffer, size_t lenght, int timeoutms)
{
    if (io->cls->read)
        return io->cls->read(io, buffer, lenght, timeoutms);

    return -1;
}

int recio_write(recio_t *io, uint8_t *buffer, size_t lenght, int timeoutms)
{
    if (io->cls->write)
        return io->cls->write(io, buffer, lenght, timeoutms);

    return -1;
}

int recio_seek(recio_t *io, size_t offset, int whence)
{
    if (io->cls->seek)
        return io->cls->seek(io, offset, whence);

    return -1;
}
