
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <queue.h>

#include "spisync_port.h"
#include "desc_buf.h"

typedef struct {
    uint8_t     type;
    uint16_t    len;
    uint8_t     *buf;
    void        *(*cb)(void *arg);  // pbuf_free or complete used
    void        *cb_arg;            // pbuf_free or complete used
} desc_t;

int desc_init(desc_msg_t *q, uint32_t items)
{
    if (NULL == q) {
        return -1;
    }

    //printf("desc_init = %p\r\n", q);
    q->hdl = xQueueCreate(items, sizeof(desc_t));

    if (NULL == q->hdl) {
        return -2;
    }

    spisync_log("creat success.\r\n");
    return 0;
}

int desc_deinit(desc_msg_t *q)
{
    return 0;
}

int desc_push_tofront(desc_msg_t *q, void *buf, uint32_t len, uint32_t timeout_ms)//vip
{
    desc_t msg;
    int res;

    if ((NULL == q) || (NULL == buf) || (0 == len)) {
        return -1;
    }

    msg.buf = malloc(len);
    if (NULL == msg.buf) {
        return -2;
    }

    memcpy(msg.buf, buf, len);
    msg.len = len;

    res = xQueueSendToFront(q->hdl, &msg, (DESC_FOREVER == timeout_ms) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms));// fixme return
    if (pdTRUE != res) {
        //spisync_log("xQueueSendToFront res = %d\r\n", res);
        return -3;
    }

    spisync_log("desc_push_tofront success.\r\n");
    return 0;
}

int desc_push_toback(desc_msg_t *q, void *buf, uint32_t len, uint32_t timeout_ms)
{
    desc_t msg;
    int res;

    if ((NULL == q) || (NULL == buf) || (0 == len)) {
        return -1;
    }

    msg.buf = malloc(len);
    if (NULL == msg.buf) {
        return -2;
    }

    memcpy(msg.buf, buf, len);
    msg.len = len;

    res = xQueueSendToBack(q->hdl, &msg, (DESC_FOREVER == timeout_ms) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms));// fixme return
    if (pdTRUE != res) {
        //spisync_log("xQueueSendToBack res = %d\r\n", res);
        return -3;
    }

    spisync_log("desc_push_toback success.\r\n");
    return 0;
}

int desc_pop(desc_msg_t *q, void *buf, uint32_t *len_p, uint32_t timeout_ms)//timerout = 0xFFFFFFFF ms
{
    desc_t msg;
    int res;

    if (NULL == q) {
        return -1;
    }

    //printf("desc_pop = %p\r\n", desc_pop);

    res = xQueueReceive(q->hdl, &msg,
            (DESC_FOREVER == timeout_ms) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms));

    if (pdTRUE != res) {
        return -2;
    }

    if (buf) {
        memcpy(buf, msg.buf, msg.len);
    }

    if (len_p) {
        *len_p = msg.len;
    }

    free(msg.buf);

    return 0;
}

int desc_peek(desc_msg_t *q, uint32_t *len_p, uint32_t timeout_ms)
{
    desc_t msg;

    if ((NULL == q) || (NULL == len_p)) {
        return -1;
    }

    if (pdTRUE == xQueuePeek(q->hdl, &msg,
                (DESC_FOREVER == timeout_ms) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms))) {
        *len_p = msg.len;
    } else {
        *len_p = 0;
        return -2;
    }

    return 0;
}

