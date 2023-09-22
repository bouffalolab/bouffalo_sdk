/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>

#include <xutils/debug.h>
#include <msp/kernel.h>

#include "internal.h"

#define TAG "uSrv"


void uservice_lock(uservice_t *srv)
{
    msp_assert(srv);
    if (srv->task)
        TASK_LOCK(srv->task);
}

void uservice_unlock(uservice_t *srv)
{
    msp_assert(srv);
    if (srv->task)
        TASK_UNLOCK(srv->task);
}

#ifdef __linux__
int uservice_call(uservice_t *srv, rpc_t *rpc)
{
    msp_assert(srv);
    msp_assert(srv->task);
    rpc->srv = srv;

    int count = 10;
    while (count--) {
        if (mq_send(srv->task->queue, (const char*)rpc, sizeof(rpc_t), 0) == 0) {
            return rpc_wait(rpc);
        } else {
            if ( count == 1) {
                LOGW(TAG, "uService %s queue full,send id:%d cur id: %d", srv->name,rpc->cmd_id, srv->task->current_rpc->cmd_id);
            }
            msp_msleep(100);
        }
    }

    return -1;
}

#else
int uservice_call(uservice_t *srv, rpc_t *rpc)
{
    msp_assert(srv);
    msp_assert(srv->task);

    rpc->srv = srv;

    if(rpc->data && rpc->data->timeout_ms != 0) {
        if (msp_task_self() == srv->task->task) {
            LOGW(TAG, "cur task == uService task");
            return -1;
        }
    }

    int count = 10;
    while (count--) {
        if (msp_queue_send(&srv->task->queue, rpc, sizeof(rpc_t)) == 0) {
            return rpc_wait(rpc);
        } else {
            if ( count == 1) {
                LOGW(TAG, "uService %s queue full,send id:%d cur id: %d", srv->name,rpc->cmd_id, srv->task->current_rpc->cmd_id);
            }
            if (!msp_irq_context())
                msp_msleep(100);
        }
    }

    return -1;
}
#endif

int uservice_call_sync(uservice_t *srv, int cmd, void *param, void *resp, size_t size)
{
    msp_assert(srv);

    rpc_t rpc;
    int ret;

    ret = rpc_init(&rpc, cmd, MSP_WAIT_FOREVER);

    if (ret < 0)
        return ret;

    if (param)
        rpc_put_point(&rpc, param);

    ret = uservice_call(srv, &rpc);

    if (resp != NULL && size > 0 && ret == 0) {
        int count;
        void *data = rpc_get_buffer(&rpc, &count);
        msp_assert(count == size && data != NULL);
        if (data)
            memcpy(resp, data, size);
    }

    rpc_deinit(&rpc);

    return ret;
}

int uservice_call_async(uservice_t *srv, int cmd, void *param, size_t param_size)
{
    msp_assert(srv);

    rpc_t rpc;
    int ret;

    ret = rpc_init(&rpc, cmd, 0);

    if (ret == 0) {
        if (param && param_size > 0)
            ret = rpc_put_buffer(&rpc, param, param_size);
		if (ret < 0) {
            rpc_deinit(&rpc);
		    return ret;
		}

        ret = uservice_call(srv, &rpc);
        rpc_deinit(&rpc);
    }

    return ret;
}

uservice_t *uservice_new(const char *name, process_t process_rpc, void *context)
{
    msp_assert(process_rpc && name);
    uservice_t *srv = msp_zalloc(sizeof(uservice_t));

    if (srv == NULL)
        return NULL;

    srv->name        = name;
    srv->context     = context;
    srv->process_rpc = process_rpc;

    return srv;
}

void uservice_destroy(uservice_t *srv)
{
    msp_assert(srv);

    msp_free(srv);
}

int uservice_process(void *context, rpc_t *rpc, const rpc_process_t rpcs[])
{
    for (int i = 0; rpcs[i].process != NULL; i++) {
        if (rpcs[i].cmd_id == rpc->cmd_id) {
            if (rpcs[i].process(context, rpc) >= 0)
                rpc_reply(rpc);
            return 0;
        }
    }
    LOGW(TAG, "cmd is may be error. uService = %s, cmd_id = %d", rpc->srv->name, rpc->cmd_id);

    return -1;
}

static void uservice_event_process(uint32_t event_id, const void *data, void *context)
{
    uservice_t *srv = (uservice_t*)context;
    rpc_t rpc;

    msp_assert(srv);

    if (rpc_init(&rpc, event_id, 0) == 0) {
        rpc_put_point(&rpc, data);
        uservice_call(srv, &rpc);
        rpc_deinit(&rpc);
    }
}

void uservice_subscribe(uservice_t *srv, uint32_t event_id)
{
    msp_assert(srv);
    event_subscribe(event_id, uservice_event_process, srv);
}
