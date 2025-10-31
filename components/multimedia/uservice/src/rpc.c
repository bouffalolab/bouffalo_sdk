/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <msp/kernel.h>
#include <xutils/debug.h>
#include <xutils/list.h>

#include "internal.h"

static void rpc_buffer_free(rpc_buffer_t *data)
{
    if (data->buffer) {
        msp_free(data->buffer);
        data->buffer = NULL;
    }

    if (data->timeout_ms)
        msp_sem_free(&data->sem);

    msp_free(data);
}

int rpc_init(rpc_t *rpc, int cmd_id, int timeout_ms)
{
    msp_assert(rpc);

    int ret = 0;

    memset(rpc, 0, sizeof(rpc_t));
    rpc->cmd_id     = cmd_id;

    if (timeout_ms != 0) {
        rpc->data = msp_zalloc(sizeof(rpc_buffer_t));

        if (rpc->data == NULL)
            return -ENOMEM;

        rpc->data->timeout_ms = timeout_ms;
        ret = msp_sem_new(&rpc->data->sem, 0);
        if (ret != 0) {
            msp_free(rpc->data);
            rpc->data = NULL;
        }
    }

    return ret;
}

int rpc_wait(rpc_t *rpc)
{
    msp_assert(rpc);
    int ret = 0;
    if (rpc->data && rpc->data->timeout_ms)
        ret = msp_sem_wait(&rpc->data->sem, rpc->data->timeout_ms);
    return ret;
}

void rpc_free(rpc_t *rpc)
{
    msp_assert(rpc);
    if (rpc->data)
        rpc_buffer_free(rpc->data);
}

void rpc_deinit(rpc_t *rpc)
{
    msp_assert(rpc);

    if (rpc->data == NULL)
        return;

    int      cached = 0;
    utask_t *task   = rpc->srv->task;

    msp_assert(task);

    TASK_LOCK(task);

    rpc_buffer_t *data;
    msp_slist_for_each_entry(&task->rpc_buffer_gc_cache, data, rpc_buffer_t, next) {
        if (data == rpc->data) {
            msp_slist_del(&data->next, &task->rpc_buffer_gc_cache);
            rpc_buffer_free(data);

            cached = 1;
            break;
        }
    }

    if (cached == 0) {
        msp_slist_add_tail(&rpc->data->next, &task->rpc_buffer_gc_cache);
    }

    TASK_UNLOCK(task);
}

void rpc_reply(rpc_t *rpc)
{
    msp_assert(rpc);
    msp_assert(rpc->srv);
    msp_assert(rpc->srv->task);

    if (rpc->data) {
        if (rpc->data->timeout_ms) {
            msp_sem_signal(&(rpc->data->sem));
        } else {
            // is async call, free this data.
            // LOGI("RPC", "async rpc cannot have return value.");
            // rpc_buffer_free(rpc->data);
        }
        rpc_deinit(rpc);
    }
}

static uint8_t *rpc_buffer_need_size(rpc_buffer_t *rpc, int size)
{
    void *p = NULL;
    msp_assert(rpc);

    uint8_t *buf = msp_zalloc(size + rpc->buf_size);

    if (buf) {
        if (rpc->buffer) {
            memcpy(buf, rpc->buffer, rpc->buf_size);
            msp_free(rpc->buffer);
        }

        rpc->buffer = buf;
        p           = rpc->buffer + rpc->buf_size;
        rpc->buf_size += size;
    }

    return p;
};

void rpc_put_reset(rpc_t *rpc)
{
    msp_assert(rpc);
    if (rpc->data && rpc->data->buffer) {
        msp_free(rpc->data->buffer);
        rpc->data->buffer   = NULL;
        rpc->data->buf_size = 0;
        rpc->data->pos      = 0;
    }
}

void rpc_get_reset(rpc_t *rpc)
{
    msp_assert(rpc);
    if (rpc->data)
        rpc->data->pos = 0;
}

#define RPC_PUT(rpc, type, v) return rpc_put_buffer(rpc, &v, sizeof(v))

int rpc_put_int(rpc_t *rpc, int i)
{
    RPC_PUT(rpc, int, i);
}

int rpc_put_uint8(rpc_t *rpc, uint8_t c)
{
    RPC_PUT(rpc, uint8_t, c);
}

int rpc_put_double(rpc_t *rpc, double c)
{
    RPC_PUT(rpc, double, c);
}

int rpc_put_point(rpc_t *rpc, const void *p)
{
    RPC_PUT(rpc, void *, p);
}

int rpc_put_buffer(rpc_t *rpc, const void *data, size_t size)
{
    msp_assert(rpc);
    if (data == NULL || size == 0)
        return -EINVAL;

    if (rpc->data == NULL) {
        rpc->data = msp_zalloc(sizeof(rpc_buffer_t));

        if (rpc->data == NULL)
            return -ENOMEM;
    }

    uint8_t *p = rpc_buffer_need_size(rpc->data, size + sizeof(uint32_t));

    if (p) {
        memcpy(p, (uint8_t *)&size, sizeof(uint32_t));
        p += sizeof(uint32_t);
        memcpy(p, data, size);
    }

    return p ? 0 : -ENOMEM;
}

int rpc_put_string(rpc_t *rpc, char *text)
{
    return rpc_put_buffer(rpc, text, strlen(text) + 1);
}

#define RPC_GET(rpc, type)                                                                         \
    do {                                                                                           \
        int   count = -1;                                                                               \
        void *p = rpc_get_buffer(rpc, &count);                                                     \
        msp_assert(count == sizeof(type));                                                         \
        type v;                                                                                    \
        memcpy(&v, p, sizeof(type));                                                               \
        return v;                                                                                  \
    } while (0)

int rpc_get_int(rpc_t *rpc)
{
    RPC_GET(rpc, int);
}

uint8_t rpc_get_uint8(rpc_t *rpc)
{
    RPC_GET(rpc, uint8_t);
}

double rpc_get_double(rpc_t *rpc)
{
    RPC_GET(rpc, double);
}

void *rpc_get_point(rpc_t *rpc)
{
    RPC_GET(rpc, void *);
}

char *rpc_get_string(rpc_t *rpc)
{
    return rpc_get_buffer(rpc, NULL);
}

void *rpc_get_buffer(rpc_t *rpc, int *count)
{
    msp_assert(rpc);
    void *p = NULL;

    if (rpc->data) {
        int len;

        memcpy((uint8_t *)&len, (int *)(rpc->data->buffer + rpc->data->pos), sizeof(int));
        p       = rpc->data->buffer + rpc->data->pos + sizeof(uint32_t);
        rpc->data->pos += sizeof(uint32_t) + len;

        if (count)
            *count = len;
    }

    return p;
}
