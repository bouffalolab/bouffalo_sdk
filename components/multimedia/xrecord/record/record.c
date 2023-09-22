/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>
#include "msp/kernel.h"
#include "msp/record.h"
#include "xutils/list.h"
#include "recio.h"

__attribute__((unused)) static const char *TAG = "record";

#define READ_PIECE_SIZE 4096

typedef struct _record_node {
    msp_slist_t         next;
    recio_t         *read_hdl;
    recio_t         *write_hdl;
    char            *from;
    char            *to;
    int             quit;
    msp_event_t     quit_event;
    data_ready_func d_rdy_func;
    char           *task_name;
    data_release_func d_release_func;
    void            *user_data;
    int             chunk_size;
    // TODO: for debug
    int             read_bytes;
    int             send_bytes;
    int             send_times;
} record_node_t;

static MSP_SLIST_HEAD(record_list);

static int recio_register_with_path(const char *path)
{
    msp_check_return_val(path, -1);

    if (strstr(path, "ws://")) {
        return recio_register_ws(path);
    } else if (strstr(path, "mem://")) {
        // return recio_register_mem(path);
    } else if (strstr(path, "rbuf://")) {
        return recio_register_ringb(path);
    } else if (strstr(path, "sdbuffer://")) {
        // return recio_register_sdbuf(path);
    } else if (strstr(path, "mic://")) {
        return recio_register_mic(path);
    } else if (strstr(path, "null://")) {
        return recio_register_null(path);
    }  else {
        LOGW(TAG, "not support yet");
    }

    return -1;
}

rec_hdl_t record_register(const char *from, const char *to)
{
    record_node_t *node = msp_zalloc_check(sizeof(record_node_t));
    if (node) {
        recio_register_with_path(from);
        recio_register_with_path(to);
        node->from = strdup(from);
        node->to = strdup(to);
        msp_slist_add_tail(&node->next, &record_list);
    }
    return (rec_hdl_t)node;
}

static void _record_task(void *arg)
{
    int bytes;
    record_node_t *node = (record_node_t *)arg;
    uint8_t *sendbuffer = msp_zalloc_check(node->chunk_size + 1);
    if (sendbuffer == NULL) {
        goto out;
    }
    while(!node->quit) {
        bytes = recio_read(node->read_hdl, sendbuffer, node->chunk_size, 0);
        //printf("recio_read from:%05s, bytes:%d\r\n", node->from, bytes);
        if (node->d_rdy_func) {
            if (node->d_rdy_func(node->user_data, sendbuffer, bytes) < 0) {
                continue;
            }
        }
        if (bytes <= 0) {
            // LOGI(TAG, "ws rec file finish, from:%s, to:%s, bytes:%d", node->from, node->to, bytes);
            if (node->d_rdy_func) {
                //node->d_rdy_func(node->user_data);
            } else {
                msp_msleep(40);
            }
            // break;
        } else if (bytes > 0) {
            // LOGD(TAG, "w %d", bytes);
            node->read_bytes += bytes;
            bytes = recio_write(node->write_hdl, sendbuffer, bytes, 0);
            //printf("recio_write to:%05s, bytes:%d\r\n", node->to, bytes);
            if (bytes > 0) {
                node->send_bytes += bytes;
            } else if (bytes < 0) {
                LOGE(TAG, "write e, ws rec file finish, from:%s, to:%s, send:%d", node->from, node->to, bytes);
                break;
            }
            node->send_times ++;
        }
    }
    msp_free(sendbuffer);
out:
    printf("_record %s task exit.\r\n", msp_task_get_name(msp_task_self()));
    msp_free(node->task_name);
    msp_event_set(&node->quit_event, 0x01, MSP_EVENT_OR);
    msp_task_exit(0);
}

static char *_str_cat(char *restrict newstring, const char *restrict delimiters)
{
    char *str = strstr(newstring, delimiters);
	if (str != NULL) {
		*str = '\0';
	}
    return newstring;
}

int record_start(rec_hdl_t hdl)
{
    record_node_t *node = (record_node_t *)hdl;
    char task_name[64] = {0};

    if (node == NULL) {
        return -1;
    }
    recio_t *f = recio_open(node->from);
    recio_t *t = recio_open(node->to);

    char *from = strdup(node->from);
    char *to = strdup(node->to);
    _str_cat(from, ":");
    _str_cat(to, ":");

    node->read_hdl = f;
    node->write_hdl = t;

    if (!(f && t)) {
        LOGE(TAG, "recio open failed..[%s]", node->to);
        return -1;
    }

    msp_event_new(&node->quit_event, 0);
    if (node->chunk_size <= 0) {
        node->chunk_size = READ_PIECE_SIZE;
    }
    sprintf(task_name, "rec:%s->%s", from, to);
    node->task_name = strdup(task_name);
    msp_task_new(node->task_name, _record_task, (void *)node, 4096);
    free(from);
    free(to);

    return 0;
}

int record_pre_stop(rec_hdl_t hdl)
{
    record_node_t *node = (record_node_t *)hdl;

    if (node == NULL) {
        return -1;
    }

    node->quit = 1;

    return 0;
}

int record_stop(rec_hdl_t hdl)
{
    unsigned int flags;
    record_node_t *node = (record_node_t *)hdl;

    if (node == NULL) {
        return -1;
    }

    node->quit = 1;
    if (node->d_release_func) {
        node->d_release_func(node->user_data);
    }
    if (node->quit_event)
        msp_event_get(&node->quit_event, 0x01, MSP_EVENT_OR_CLEAR, &flags, MSP_WAIT_FOREVER);
    if (node->read_hdl)
        recio_close(node->read_hdl);
    if (node->write_hdl)
        recio_close(node->write_hdl);
    if (node->quit_event)
        msp_event_free(&node->quit_event);

    return 0;
}

int record_unregister(rec_hdl_t hdl)
{
    record_node_t *node = (record_node_t *)hdl;

    if (hdl == NULL) {
        return -1;
    }
    recio_unregister(node->from);
    recio_unregister(node->to);
    msp_free(node->from);
    msp_free(node->to);
    msp_slist_del(&node->next, &record_list);
    msp_free(node);
    return 0;
}

void record_set_data_ready_cb(rec_hdl_t hdl, data_ready_func cb, void *arg)
{
    if (hdl) {
        record_node_t *node = (record_node_t *)hdl;
        node->d_rdy_func = cb;
        node->user_data = arg;
    }
}

void record_set_data_release_cb(rec_hdl_t hdl, data_release_func cb, void *arg)
{
    if (hdl) {
        record_node_t *node = (record_node_t *)hdl;
        node->d_release_func = cb;
        node->user_data = arg;
    }
}

void record_set_chunk_size(rec_hdl_t hdl, int chunk_size)
{
    if (hdl) {
        record_node_t *node = (record_node_t *)hdl;
        node->chunk_size = chunk_size;
    }
}

int record_get_count(rec_hdl_t hdl, int *read_bytes, int *sent_bytes)
{
    msp_check_param(hdl && read_bytes && sent_bytes);

    record_node_t *node = (record_node_t *)hdl;

    *read_bytes = node->read_bytes;
    *sent_bytes = node->send_bytes;

    return 0;
}
