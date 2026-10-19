/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <xutils/xringbuffer.h>

#define X_MIN(a, b) (a)<(b)? (a) : (b)

int xringbuffer_create(x_ringbuf_t *rb, uint8_t *buffer, int length)
{
    memset(buffer, 0, length);

    rb->length = length - 1;
    rb->ridx = 0;
    rb->widx = 0;
    rb->buffer = (uint8_t *)buffer;

    return 0;
}

int xringbuffer_reset(x_ringbuf_t *rb)
{
    rb->ridx = 0;
    rb->widx = 0;

    return 0;
}

void xringbuffer_destroy(x_ringbuf_t *rb)
{
    rb->buffer = NULL;
    rb->length = rb->ridx = rb->widx = 0;
}

int xringbuffer_available_read_space(x_ringbuf_t *rb)
{
    uint32_t ridx = rb->ridx;
    uint32_t widx = rb->widx;

    if (ridx == widx) {
        return 0;
    } else if (ridx < widx) {
        return widx - ridx;
    } else {
        return rb->length - (ridx - widx - 1);
    }
}

int xringbuffer_write(x_ringbuf_t *rb, uint8_t *data, uint32_t length)
{
    int i = 0;

    int space_len = xringbuffer_available_write_space(rb);
    if (space_len < length) {
        length = space_len;
    }

    for (i = 0; i < length; i++) {
        if (NULL != data) {
            rb->buffer[rb->widx] = data[i];
        }

        rb->widx++;

        if (rb->widx >= rb->length + 1) {
            rb->widx = 0;
        }
    }

    /* return real write len */
    return i;
}

int xringbuffer_read(x_ringbuf_t *rb, uint8_t *target, uint32_t amount)
{
    int copy_sz = 0;
    int i;

    if (amount == 0) {
        return -1;
    }

    if (xringbuffer_empty(rb)) {
        return 0;
    }

    /* get real read size */
    int buffer_size = xringbuffer_available_read_space(rb);
    copy_sz = X_MIN(amount, buffer_size);

//csi_dcache_clean_invalid_range(context->dma->node[i].dma_cfg.destDmaAddr, node_size);

    /* cp data to user buffer */
    if (NULL != target) {
        for (i = 0; i < copy_sz; i++) {
            target[i] = rb->buffer[rb->ridx];

            rb->ridx++;

            if (rb->ridx >= rb->length + 1) {
                rb->ridx = 0;
            }
        }
    } else {
        rb->ridx += copy_sz;

        if (rb->ridx >= rb->length + 1) {
            rb->ridx -= (rb->length + 1);
        }
    }

    return copy_sz;
}

int xringbuffer_test(char *buf, int len, int argc, char **argv)
{
    #define TEST_RB_MAX_LEN     (256)
    #define TEST_RB_CACHE_LEN   (10)
    int res;
    uint8_t buffer[TEST_RB_MAX_LEN];
    uint8_t wbuf[TEST_RB_CACHE_LEN] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    uint8_t rbuf[TEST_RB_CACHE_LEN];

    x_ringbuf_t rb;

    res = xringbuffer_create(&rb, buffer, TEST_RB_MAX_LEN);
    if (0 != res) {
        return -1;
    }

    res = xringbuffer_write(&rb, wbuf, TEST_RB_CACHE_LEN);
    if (res != TEST_RB_CACHE_LEN) {
        return -2;
    }

    res = xringbuffer_available_write_space(&rb);
    if ((TEST_RB_MAX_LEN - TEST_RB_CACHE_LEN - 1)  != res) {
        return -3;
    }

    res = xringbuffer_available_read_space(&rb);
    if (TEST_RB_CACHE_LEN != res) {
        return -4;
    }

    res = xringbuffer_read(&rb, rbuf, TEST_RB_CACHE_LEN);
    if (res != TEST_RB_CACHE_LEN) {
        return -5;
    }

    res = memcmp(wbuf, rbuf, TEST_RB_CACHE_LEN);
    if (0 != res) {
        return -6;
    }

    xringbuffer_destroy(&rb);

    return 0;
}
