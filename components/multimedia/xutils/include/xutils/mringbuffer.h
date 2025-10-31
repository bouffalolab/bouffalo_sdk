/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring
 *      https://github.com/RT-Thread/
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __MRINGBUFFER_H__
#define __MRINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

/* ring buffer */
typedef struct mringbuffer {
    uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     */

    uint32_t read_mirror : 1;
    uint32_t read_index : 31;
    uint32_t write_mirror : 1;
    uint32_t write_index : 31;
    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    int32_t buffer_size;
} m_ringbuf_t;

typedef void (*cache_operate_t)(void *addr, int32_t dsize);

enum mringbuffer_state {
    MRINGBUFFER_EMPTY,
    MRINGBUFFER_FULL,
    /* half full is neither full nor empty */
    MRINGBUFFER_HALFFULL,
};

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
void mringbuffer_init(struct mringbuffer *rb, uint8_t *pool, int32_t size);
void mringbuffer_reset(struct mringbuffer *rb);
uint32_t mringbuffer_put(struct mringbuffer *rb, const uint8_t *ptr, uint32_t length);
uint32_t mringbuffer_put_fflush_cache(struct mringbuffer   *rb,
                            const uint8_t      *ptr,
                            uint32_t           length,
                            cache_operate_t    fflush_cache);
int32_t mringbuffer_put_index(struct mringbuffer *rb, uint32_t w_index);
// uint32_t mringbuffer_put_force(struct mringbuffer *rb, const uint8_t *ptr, uint32_t length);
uint32_t mringbuffer_putchar(struct mringbuffer *rb, const uint8_t ch);
// uint32_t mringbuffer_putchar_force(struct mringbuffer *rb, const uint8_t ch);

uint32_t mringbuffer_get(struct mringbuffer *rb, uint8_t *ptr, uint32_t length);
uint32_t mringbuffer_get_invalid_cache(struct mringbuffer *rb,
                            uint8_t           *ptr,
                            uint32_t           length,
                            cache_operate_t    remove_cache);
uint32_t mringbuffer_get_index(struct mringbuffer *rb, uint32_t r_index);
uint32_t mringbuffer_peek(struct mringbuffer *rb, uint8_t **ptr);
uint32_t mringbuffer_getchar(struct mringbuffer *rb, uint8_t *ch);
uint32_t mringbuffer_data_len(struct mringbuffer *rb);

struct mringbuffer* mringbuffer_create(uint32_t length);
void mringbuffer_destroy(struct mringbuffer *rb);

uint32_t mringbuffer_get_size(struct mringbuffer *rb);// total size

/** return the size of empty space in rb */
#define mringbuffer_space_len(rb) ((rb)->buffer_size - mringbuffer_data_len(rb))


#ifdef __cplusplus
}
#endif

#endif
