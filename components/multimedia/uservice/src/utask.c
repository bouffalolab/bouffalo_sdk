/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include <uservice/uservice.h>

#include <msp/kernel.h>
#include <xutils/debug.h>

#include "internal.h"

#define TAG "utask"

static int g_utask_softwdt_timeout = 0;
static MSP_SLIST_HEAD(utask_list);
#define LOOP_TIME_MS 1000
#define MIN_SOFTWDT_TIME 2000 /*ms*/

#ifdef __linux__
static void utask_entry(void *data)
{
    rpc_t  rpc;
    utask_t *task = (utask_t *)data;
    uint64_t nsec;
    struct timespec ts;
    struct timeval now;
    unsigned int timeout = LOOP_TIME_MS;

    while (task->running) {
        gettimeofday(&now, NULL);
        nsec       = now.tv_usec * 1000 + (timeout % 1000) * 1000000;
        ts.tv_nsec = nsec % 1000000000;
        ts.tv_sec  = now.tv_sec + nsec / 1000000000 + timeout / 1000;

        if (mq_timedreceive(task->queue, (char*)&rpc, sizeof(rpc_t), NULL, &ts) == sizeof(rpc_t)) {
            if (rpc.srv->process_rpc) {
                task->current_rpc = &rpc;
                rpc.srv->process_rpc(rpc.srv->context, &rpc);
                task->current_rpc = NULL;
            } else {
                rpc_reply(&rpc);
            }
        }
    }

    msp_sem_signal(&task->running_wait);
}

#else

static void task_will(void *args)
{
    utask_t *task = (utask_t*)args;

    LOGE(TAG, "utask: %s crash!", msp_task_get_name(&task->task));
    if (task->current_rpc) {
        LOGE(TAG, "uservice_name: %s, rpc_cmd: %d", task->current_rpc->srv->name, task->current_rpc->cmd_id);
    }
}

static void utask_entry(void *data)
{
    utask_t *task = (utask_t *)data;

    rpc_t  rpc;
    size_t size;

    while (task->running) {
        if (msp_queue_recv(&task->queue, LOOP_TIME_MS, &rpc, &size) == 0) {

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
            int count = msp_queue_get_count(&task->queue) + 1;
            if (count > task->queue_max_used)
                task->queue_max_used = count;

            struct rpc_record *node;
            int               found = 0;
            msp_slist_for_each_entry(&task->rpc_reclist, node, struct rpc_record, next) {
                if (node->cmd_id == rpc.cmd_id && node->srv == rpc.srv) {
                    node->count++;
                    found = 1;
                    break;
                }
            }

            if (found == 0) {
                node = msp_zalloc(sizeof(struct rpc_record));
                node->cmd_id = rpc.cmd_id;
                node->srv = rpc.srv;
                node->count = 1;
                msp_slist_add(&node->next, &task->rpc_reclist);
            }
#endif
            if (rpc.srv->process_rpc) {
                task->current_rpc = &rpc;

                if (g_utask_softwdt_timeout >= MIN_SOFTWDT_TIME) {
                    msp_task_wdt_attach(task_will, task);
                    msp_task_wdt_feed(g_utask_softwdt_timeout);
                    // simulaton timeout
                    // sleep(11);
                    //uservice_lock(rpc.srv);
                    rpc.srv->process_rpc(rpc.srv->context, &rpc);
                    //uservice_unlock(rpc.srv);
                    msp_task_wdt_detach();
                } else {
                    //uservice_lock(rpc.srv);
                    rpc.srv->process_rpc(rpc.srv->context, &rpc);
                    //uservice_unlock(rpc.srv);
                }
                task->current_rpc = NULL;
            } else {
                rpc_reply(&rpc);
            }
        }
    }

    msp_sem_signal(&task->running_wait);
    
    msp_task_exit(0);
}
#endif

utask_t *utask_new(const char *name, size_t stack_size, int queue_count, int prio)
{
#ifdef __linux__
    if (!(name && stack_size && queue_count)) {
        LOGE(TAG, "utask new param invalid!");
        return NULL;
    }
#else
    if (stack_size <= 0 || queue_count <= 0)
        return NULL;
#endif
    int queue_buffer_size = queue_count * sizeof(rpc_t);

    utask_t *task = msp_zalloc(sizeof(utask_t) + queue_buffer_size);

    if (task == NULL)
        return NULL;

    task->running = 1;
    msp_slist_init(&task->uservice_lists);
    msp_slist_init(&task->rpc_buffer_gc_cache);
    msp_slist_add_tail(&task->node, &utask_list);

#ifdef __linux__
    {
        struct mq_attr mqa = {0};
        size_t nlen = strlen(name);

        mqa.mq_maxmsg  = 10;//queue_count;
        mqa.mq_msgsize = sizeof(rpc_t);

        task->qname    = msp_zalloc(nlen + 2);
        task->qname[0] = '/';
        strcpy(task->qname + 1, name);
        task->qname[nlen + 1] = '\0';

        mq_unlink(task->qname);
        task->queue = mq_open(task->qname, O_CREAT | O_RDWR, 0600, &mqa);
        if (task->queue == -1) {
            LOGE(TAG, "queue open fail, qname = %s", task->qname);
            goto out0;
        }
    }
#else
    task->queue_count  = queue_count;
    task->queue_buffer = (uint8_t*)task + sizeof(utask_t);
    if (msp_queue_new(&task->queue, task->queue_buffer, queue_buffer_size, sizeof(rpc_t)) != 0)
        goto out0;
#endif

    if (msp_mutex_new(&task->mutex) != 0)
        goto out1;

    if (msp_sem_new(&task->running_wait, 0) != 0)
        goto out2;

    if (msp_task_new_ext(&task->task, name ? name : "utask", utask_entry, task, stack_size, prio) != 0)
        goto out3;

    return task;

out3:
    msp_sem_free(&task->running_wait);
out2:
    msp_mutex_free(&task->mutex);
out1:
#ifdef __linux__
    if (task->queue >= 0) {
        mq_close(task->queue);
        mq_unlink(task->qname);
    }
#else
    if (msp_queue_is_valid(&task->queue)) {
        msp_queue_free(&task->queue);
    }
#endif
out0:
#ifdef __linux__
    msp_free(task->qname);
#endif
    msp_free(task);

    return NULL;
}

void utask_join(utask_t *task)
{
    msp_assert(task);

    task->running = 0;
    msp_sem_wait(&task->running_wait, MSP_WAIT_FOREVER);

    msp_slist_del(&task->node, &utask_list);

    uservice_t *node;
    msp_slist_t *tmp;
    msp_slist_for_each_entry_safe(&task->uservice_lists, tmp, node, uservice_t, next) {
        utask_remove(task, node);
    }

    msp_sem_free(&task->running_wait);
    msp_mutex_free(&task->mutex);
#ifdef __linux__
    mq_close(task->queue);
    mq_unlink(task->qname);
    msp_free(task->qname);
#else
    msp_queue_free(&task->queue);
#endif
    msp_free(task);
}

void utask_destroy(utask_t *task)
{
    msp_assert(task);
    utask_join(task);
}

void utask_add(utask_t *task, uservice_t *srv)
{
    msp_assert(task && srv);

    if (srv->task == NULL) {
        TASK_LOCK(task);
        srv->task = task;
        msp_slist_add_tail(&srv->next, &task->uservice_lists);
        TASK_UNLOCK(task);
    }
}

void utask_remove(utask_t *task, uservice_t *srv)
{
    msp_assert(task && srv);

    TASK_LOCK(task);

#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
    struct rpc_record *node;
    msp_slist_t *tmp;
    msp_slist_for_each_entry_safe(&task->rpc_reclist, tmp, node, struct rpc_record, next) {
        if (node->srv == srv) {
            msp_slist_del(&node->next, &task->rpc_reclist);
            free(node);
        }
    }
#endif

    msp_slist_del(&srv->next, &task->uservice_lists);
    srv->task = NULL;

    TASK_UNLOCK(task);
}

void utask_lock(utask_t *task)
{
    msp_assert(task);
    TASK_LOCK(task);
}

void utask_unlock(utask_t *task)
{
    msp_assert(task);
    TASK_UNLOCK(task);
}

void utask_set_softwdt_timeout(int ms)
{
    if (ms == 0) {
        g_utask_softwdt_timeout = 0;
    } else {
        g_utask_softwdt_timeout = (ms < MIN_SOFTWDT_TIME) ? MIN_SOFTWDT_TIME : ms;
    }
}

uservice_t *task_get(utask_t *task, const char *name)
{
    msp_assert(task);

    uservice_t *node;

    TASK_LOCK(task);

    msp_slist_for_each_entry(&task->uservice_lists, node, uservice_t, next) {
        if (strcmp(node->name, name) == 0) {
            TASK_UNLOCK(task);
            return node;
        }
    }

    TASK_UNLOCK(task);

    return NULL;
}

void tasks_debug()
{
    utask_t *utask;
    msp_slist_for_each_entry(&utask_list, utask, utask_t, node) {
#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
        struct rpc_record *rpc;

        printf("utask: %s, queue total count: %d, max used: %d\n------------------------------------------------------\n",
               msp_task_get_name(&utask->task), utask->queue_count, utask->queue_max_used);
        printf("\t%05s\t\%05s\tSERVER\n", "CMD_ID", "COUNT");
        msp_slist_for_each_entry(&utask->rpc_reclist, rpc, struct rpc_record, next) {
            printf("\t%05d\t%05d\t%s\n", rpc->cmd_id, rpc->count, rpc->srv->name);
        }
        printf("\n");
#endif
    }
}
