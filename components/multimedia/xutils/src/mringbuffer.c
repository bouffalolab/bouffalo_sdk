/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring
 *      https://github.com/RT-Thread/
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <xutils/debug.h>
#include <xutils/mspdef.h>
#include <xutils/mringbuffer.h>
#include <msp/kernel.h>

enum mringbuffer_state mringbuffer_status(struct mringbuffer *rb)
{
    if (rb->read_index == rb->write_index) {
        if (rb->read_mirror == rb->write_mirror) {
            return MRINGBUFFER_EMPTY;
        } else {
            return MRINGBUFFER_FULL;
        }
    }
    return MRINGBUFFER_HALFFULL;
}

/**
 * @brief Initialize the ring buffer object.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param pool      A pointer to the buffer.
 * @param size      The size of the buffer in bytes.
 */
void mringbuffer_init(struct mringbuffer *rb,
                        uint8_t           *pool,
                        int32_t            size)
{
    //assert(rb != NULL);
    //assert(size > 0);

    /* initialize read and write index */
    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = MSP_ALIGN_DOWN(size, MSP_ALIGN_SIZE);
}

/**
 * @brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will discard out-of-range data.
 *
 * @param rb            A pointer to the ring buffer object.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of data in bytes.
 *
 * @return Return the data size we put into the ring buffer.
 */
uint32_t mringbuffer_put(struct mringbuffer *rb,
                            const uint8_t     *ptr,
                            uint32_t           length)
{
    uint32_t size;

    //assert(rb != NULL);

    /* whether has enough space */
    size = mringbuffer_space_len(rb);

    /* no space */
    if (size == 0) {
        return 0;
    }

    /* drop some data */
    if (size < length) {
        length = size;
    }

    if (rb->buffer_size - rb->write_index > length) {
        /* read_index - write_index = empty space */
        if (ptr) {
            memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        }
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

    if (ptr) {
        memcpy(&rb->buffer_ptr[rb->write_index],
                &ptr[0],
                rb->buffer_size - rb->write_index);
        memcpy(&rb->buffer_ptr[0],
                &ptr[rb->buffer_size - rb->write_index],
                length - (rb->buffer_size - rb->write_index));
    }

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

uint32_t mringbuffer_put_fflush_cache(struct mringbuffer   *rb,
                            const uint8_t      *ptr,
                            uint32_t           length,
                            cache_operate_t    fflush_cache)
{
    uint32_t size;

    //assert(rb != NULL);

    /* whether has enough space */
    size = mringbuffer_space_len(rb);

    /* no space */
    if (size == 0) {
        return 0;
    }

    /* drop some data */
    if (size < length) {
        length = size;
    }

    if (rb->buffer_size - rb->write_index > length) {
        /* read_index - write_index = empty space */
        if (ptr) {
            memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
            if (fflush_cache) {
                fflush_cache((uint32_t *)&rb->buffer_ptr[rb->write_index], length);
            }
        }
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;
        return length;
    }

    if (ptr) {
        memcpy(&rb->buffer_ptr[rb->write_index],
                &ptr[0],
                rb->buffer_size - rb->write_index);
        memcpy(&rb->buffer_ptr[0],
                &ptr[rb->buffer_size - rb->write_index],
                length - (rb->buffer_size - rb->write_index));
        if (fflush_cache) {
            fflush_cache((uint32_t *)&rb->buffer_ptr[rb->write_index],
                rb->buffer_size - rb->write_index);
            fflush_cache((uint32_t *)&rb->buffer_ptr[0],
                length - (rb->buffer_size - rb->write_index));
        }
    }

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    return length;
}

/**
 * @brief Update w_index async
 *
 * @param rb            A pointer to the ring buffer object.
 * @param w_index       async index bytes.
 *
 * @return Return >=0:the data size we put into the ring buffer; <0: overrun
 */
int32_t mringbuffer_put_index(struct mringbuffer *rb, uint32_t w_index)
{
    uint32_t space_len;
    uint32_t tmp_len;
    uint32_t wi, ri, aw;
    uint32_t put_size;

    /* whether has enough space */
    space_len = mringbuffer_space_len(rb);

    if (space_len == 0) {
        // Over run because of no enough space. Please check whether the application software has long-term block logic
        return -1;
    }

    wi = rb->write_index;
    ri = rb->read_index;
    aw = w_index;

    if (wi >= ri) {
        if (aw > wi) {
            put_size = aw - wi;
        } else if (aw <= ri) {
            put_size = rb->buffer_size - (wi - aw);
        } else {
            // Over run because of no enough space. Please check whether the application software has long-term block logic
            return -2;
        }
    } else {
        if ((aw > wi) && (aw <= ri)) {
            put_size = aw - wi;
        } else {
            // Over run because of no enough space. Please check whether the application software has long-term block logic
            return -3;
        }
    }

    // check
    if (space_len < put_size) {
        MSP_LOGA("never here, please add lock befor call mringbuffer api, wi:%ld, ri:%ld, aw:%ld\r\n", wi, ri, aw);
    }

    if (put_size == 0) {
        // check
        MSP_LOGW("please add lock befor call mringbuffer api, wi:%ld, ri:%ld, aw:%ld\r\n", wi, ri, aw);
    } else {
        tmp_len = mringbuffer_put(rb, NULL, put_size);
        if (tmp_len != put_size) {
            MSP_LOGA("never here, please add lock befor call mringbuffer api, wi:%ld, ri:%ld, aw:%ld\r\n", wi, ri, aw);
        }
    }

    return put_size;
}


#if 0
/**
 * @brief Put a block of data into the ring buffer. If the capacity of ring buffer is insufficient, it will overwrite the existing data in the ring buffer.
 *
 * @param rb            A pointer to the ring buffer object.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of data in bytes.
 *
 * @return Return the data size we put into the ring buffer.
 */
uint32_t mringbuffer_put_force(struct mringbuffer *rb,
                                  const uint8_t     *ptr,
                                  uint32_t           length)
{
    uint32_t space_length;

    //assert(rb != NULL);

    space_length = mringbuffer_space_len(rb);

    if (length > rb->buffer_size)
    {
        ptr = &ptr[length - rb->buffer_size];
        length = rb->buffer_size;
    }

    if (rb->buffer_size - rb->write_index > length)
    {
        /* read_index - write_index = empty space */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->write_index += length;

        if (length > space_length)
            rb->read_index = rb->write_index;

        return length;
    }

    memcpy(&rb->buffer_ptr[rb->write_index],
              &ptr[0],
              rb->buffer_size - rb->write_index);
    memcpy(&rb->buffer_ptr[0],
              &ptr[rb->buffer_size - rb->write_index],
              length - (rb->buffer_size - rb->write_index));

    /* we are going into the other side of the mirror */
    rb->write_mirror = ~rb->write_mirror;
    rb->write_index = length - (rb->buffer_size - rb->write_index);

    if (length > space_length)
    {
        if (rb->write_index <= rb->read_index)
            rb->read_mirror = ~rb->read_mirror;
        rb->read_index = rb->write_index;
    }

    return length;
}
#endif

/**
 * @brief Get data from the ring buffer.
 *
 * @param rb            A pointer to the ring buffer.
 * @param ptr           A pointer to the data buffer.
 * @param length        The size of the data we want to read from the ring buffer.
 *
 * @return Return the data size we read from the ring buffer.
 */
uint32_t mringbuffer_get(struct mringbuffer *rb,
                            uint8_t           *ptr,
                            uint32_t           length)
{
    uint32_t size;

    //assert(rb != NULL);

    /* whether has enough data  */
    size = mringbuffer_data_len(rb);

    /* no data */
    if (size == 0) {
        return 0;
    }

    /* less data */
    if (size < length) {
        length = size;
    }

    if (rb->buffer_size - rb->read_index > length) {
        /* copy all of data */
        if (ptr) {
            memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        }

        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    if (ptr) {
        memcpy(&ptr[0],
                &rb->buffer_ptr[rb->read_index],
                rb->buffer_size - rb->read_index);
        memcpy(&ptr[rb->buffer_size - rb->read_index],
                &rb->buffer_ptr[0],
                length - (rb->buffer_size - rb->read_index));
    }

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

uint32_t mringbuffer_get_invalid_cache(struct mringbuffer *rb,
                            uint8_t           *ptr,
                            uint32_t           length,
                            cache_operate_t    remove_cache)
{
    uint32_t size;

    //assert(rb != NULL);

    /* whether has enough data  */
    size = mringbuffer_data_len(rb);

    /* no data */
    if (size == 0) {
        return 0;
    }

    /* less data */
    if (size < length) {
        length = size;
    }

    if (rb->buffer_size - rb->read_index > length) {
        /* copy all of data */
        if (ptr) {
            if (remove_cache) {
                remove_cache((uint32_t *)&rb->buffer_ptr[rb->read_index], length);
            }
            memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        }

        /* this should not cause overflow because there is enough space for
         * length of data in current mirror */
        rb->read_index += length;
        return length;
    }

    if (ptr) {
        if (remove_cache) {
            remove_cache((uint32_t *)&rb->buffer_ptr[rb->read_index],
                rb->buffer_size - rb->read_index);
            remove_cache((uint32_t *)&rb->buffer_ptr[0],
                length - (rb->buffer_size - rb->read_index));
        }
        memcpy(&ptr[0],
                &rb->buffer_ptr[rb->read_index],
                rb->buffer_size - rb->read_index);
        memcpy(&ptr[rb->buffer_size - rb->read_index],
                &rb->buffer_ptr[0],
                length - (rb->buffer_size - rb->read_index));
    }

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = length - (rb->buffer_size - rb->read_index);

    return length;
}

uint32_t mringbuffer_get_index(struct mringbuffer *rb, uint32_t r_index)
{
    uint32_t data_len;
    uint32_t tmp_len;
    uint32_t wi, ri, ar;
    uint32_t get_size;

    /* whether has enough data */
    data_len = mringbuffer_data_len(rb);

    if (data_len == 0) {
        // under run because of no enough data. wireless network signal instability? or enable wifi/bt but no coex?
        return -1;
    }

    wi = rb->write_index;
    ri = rb->read_index;
    ar = r_index;

    if (ri >= wi) {
        if (ar > ri) {
            get_size = ar - ri;
        } else if (ar <= wi) {
            get_size = rb->buffer_size - (ri - ar);
        } else {
            // under run because of no enough data. wireless network signal instability? or enable wifi/bt but no coex?
            return -2;
        }
    } else {
        if ((ar > ri) && (ar <= wi)) {
            get_size = ar - ri;
        } else {
            // under run because of no enough data. wireless network signal instability? or enable wifi/bt but no coex?
            return -3;
        }
    }

    // check
    if (data_len < get_size) {
        MSP_LOGA("never here, please add lock befor call mringbuffer api, wi:%ld, ri:%ld, ar:%ld\r\n", wi, ri, ar);
    }

    if (get_size == 0) {
        // check
        MSP_LOGW("please add lock befor call mringbuffer api, wi:%ld, ri:%ld, ar:%ld\r\n", wi, ri, ar);
    } else {
        tmp_len = mringbuffer_get(rb, NULL, get_size);
        if (tmp_len != get_size) {
            MSP_LOGA("never here, please add lock befor call mringbuffer api, wi:%ld, ri:%ld, ar:%ld\r\n", wi, ri, ar);
        }
    }

    return get_size;
}

/**
 * @brief Get the first readable byte of the ring buffer.
 *
 * @param rb        A pointer to the ringbuffer.
 * @param ptr       When this function return, *ptr is a pointer to the first readable byte of the ring buffer.
 *
 * @note It is recommended to read only one byte, otherwise it may cause buffer overflow.
 *
 * @return Return the size of the ring buffer.
 */
uint32_t mringbuffer_peek(struct mringbuffer *rb, uint8_t **ptr)
{
    uint32_t size;

    //assert(rb != NULL);

    *ptr = NULL;

    /* whether has enough data  */
    size = mringbuffer_data_len(rb);

    /* no data */
    if (size == 0) {
        return 0;
    }

    *ptr = &rb->buffer_ptr[rb->read_index];

    if ((uint32_t)(rb->buffer_size - rb->read_index) > size) {
        rb->read_index += size;
        return size;
    }

    size = rb->buffer_size - rb->read_index;

    /* we are going into the other side of the mirror */
    rb->read_mirror = ~rb->read_mirror;
    rb->read_index = 0;

    return size;
}

/**
 * @brief Put a byte into the ring buffer. If ring buffer is full, this operation will fail.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param ch        A byte put into the ring buffer.
 *
 * @return Return the data size we put into the ring buffer. The ring buffer is full if returns 0. Otherwise, it will return 1.
 */
uint32_t mringbuffer_putchar(struct mringbuffer *rb, const uint8_t ch)
{
    //assert(rb != NULL);

    /* whether has enough space */
    if (!mringbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1) {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    } else {
        rb->write_index++;
    }

    return 1;
}

#if 0
/**
 * @brief Put a byte into the ring buffer. If ring buffer is full, it will discard an old data and put into a new data.
 *
 * @param rb        A pointer to the ring buffer object.
 * @param ch        A byte put into the ring buffer.
 *
 * @return Return the data size we put into the ring buffer. Always return 1.
 */
uint32_t mringbuffer_putchar_force(struct mringbuffer *rb, const uint8_t ch)
{
    enum mringbuffer_state old_state;

    //assert(rb != NULL);

    old_state = mringbuffer_status(rb);

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size - 1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
        if (old_state == MRINGBUFFER_FULL)
        {
            rb->read_mirror = ~rb->read_mirror;
            rb->read_index = rb->write_index;
        }
    }
    else
    {
        rb->write_index++;
        if (old_state == MRINGBUFFER_FULL)
            rb->read_index = rb->write_index;
    }

    return 1;
}
#endif

/**
 * @brief Get a byte from the ring buffer.
 *
 * @param rb        The pointer to the ring buffer object.
 * @param ch        A pointer to the buffer, used to store one byte.
 *
 * @return 0    The ring buffer is empty.
 * @return 1    Success
 */
uint32_t mringbuffer_getchar(struct mringbuffer *rb, uint8_t *ch)
{
    //assert(rb != NULL);

    /* ringbuffer is empty */
    if (!mringbuffer_data_len(rb)) {
        return 0;
    }

    /* put byte */
    if (ch) {
        *ch = rb->buffer_ptr[rb->read_index];
    }

    if (rb->read_index == rb->buffer_size - 1) {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = 0;
    } else {
        rb->read_index++;
    }

    return 1;
}

/**
 * @brief Get the size of data in the ring buffer in bytes.
 *
 * @param rb        The pointer to the ring buffer object.
 *
 * @return Return the size of data in the ring buffer in bytes.
 */
uint32_t mringbuffer_data_len(struct mringbuffer *rb)
{
    switch (mringbuffer_status(rb)) {
        case MRINGBUFFER_EMPTY:
            return 0;
        case MRINGBUFFER_FULL:
            return rb->buffer_size;
        case MRINGBUFFER_HALFFULL:
        default: {
            uint32_t wi = rb->write_index, ri = rb->read_index;

            if (wi > ri) {
                return wi - ri;
            } else {
                return rb->buffer_size - (ri - wi);
            }
        }
    }
}

uint32_t mringbuffer_get_size(struct mringbuffer *rb)
{
    // assert(rb != NULL);
    return rb->buffer_size;
}

/**
 * @brief Reset the ring buffer object, and clear all contents in the buffer.
 *
 * @param rb        A pointer to the ring buffer object.
 */
void mringbuffer_reset(struct mringbuffer *rb)
{
    //assert(rb != NULL);

    rb->read_mirror = 0;
    rb->read_index = 0;
    rb->write_mirror = 0;
    rb->write_index = 0;
}

/**
 * @brief Create a ring buffer object with a given size.
 *
 * @param size      The size of the buffer in bytes.
 *
 * @return Return a pointer to ring buffer object. When the return value is NULL, it means this creation failed.
 */
struct mringbuffer *mringbuffer_create(uint32_t size)
{
    struct mringbuffer *rb;
    uint8_t *pool;

    //assert(size > 0);

    size = MSP_ALIGN_DOWN(size, MSP_ALIGN_SIZE);

    rb = (struct mringbuffer *)msp_malloc(sizeof(struct mringbuffer));
    if (rb == NULL) {
        goto exit;
    }

    pool = (uint8_t *)msp_malloc(size);
    if (pool == NULL) {
        msp_free(rb);
        rb = NULL;
        goto exit;
    }
    mringbuffer_init(rb, pool, size);

exit:
    return rb;
}

/**
 * @brief Destroy the ring buffer object, which is created by mringbuffer_create() .
 *
 * @param rb        A pointer to the ring buffer object.
 */
void mringbuffer_destroy(struct mringbuffer *rb)
{
    //assert(rb != NULL);

    msp_free(rb->buffer_ptr);
    msp_free(rb);
}
