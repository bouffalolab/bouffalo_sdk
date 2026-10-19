/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __X_RINGBUFFER__
#define __X_RINGBUFFER__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *buffer;
    uint32_t length;
    uint32_t ridx;
    uint32_t widx;
} x_ringbuf_t;

/**
 * @brief  create a ring buffer
 * @param[in] length length space of the ring buffer
 * @return  pointer to ring buffer
 */
int xringbuffer_create(x_ringbuf_t *rb, uint8_t *buffer, int length);

int xringbuffer_reset(x_ringbuf_t *rb);

/**
 * @brief   destroy the ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  null
 */
void xringbuffer_destroy(x_ringbuf_t *buffer);

/**
 * @brief   read data from ring buffer.
 * @param[in] buffer pointer to the ring buffer
 * @param[in] target pointer to buffer for data to read from ring buffer
 * @param[in] amount amount of data items to read
 * @return  number of actual read data
 */
int xringbuffer_read(x_ringbuf_t *buffer, uint8_t *target, uint32_t amount);

/**
 * @brief   write data to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @param[in] data pointer to buffer for data to write to ring buffer
 * @param[in] length length of data items to write
 * @return  0 for success
 */
int xringbuffer_write(x_ringbuf_t *buffer, uint8_t *data, uint32_t length);

/**
 * @brief   is the ring buffer empty?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int xringbuffer_empty(x_ringbuf_t *buffer);
/**
 * @brief   is the ring buffer full?
 * @param[in] buffer pointer to the ring buffer
 * @return  0 for success
 */
int xringbuffer_full(x_ringbuf_t *buffer);
/**
 * @brief   available write space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return   number of write spcae
 */
int xringbuffer_available_write_space(x_ringbuf_t *buffer);
/**
 * @brief    available read space to ring buffer
 * @param[in] buffer pointer to the ring buffer
 * @return  number of read spcae
 */
int xringbuffer_available_read_space(x_ringbuf_t *buffer);
/**
 * @brief    test the ringbuffer api
 * @param[in] buf
 * @param[in] len
 * @param[in] argc
 * @param[in] argv
 * @return  0 for success
 */
int xringbuffer_test(char *buf, int len, int argc, char **argv);

#define xringbuffer_available_write_space(B) (\
        (B)->length - xringbuffer_available_read_space(B))

#define xringbuffer_full(B) (xringbuffer_available_write_space(B) == 0)

#define xringbuffer_empty(B) (xringbuffer_available_read_space((B)) == 0)

#define xringbuffer_clear(B) ((B)->widx = (B)->ridx = 0)

#ifdef __cplusplus
}
#endif

#endif

