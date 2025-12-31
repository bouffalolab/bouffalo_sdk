/**
 * @file ring_buffer.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "ring_buffer.h"

#if defined(__riscv)
#define RB_FENCE_RW_RW() __asm__ __volatile__("fence rw, rw" ::: "memory")
#else
#define RB_FENCE_RW_RW() __asm__ __volatile__("" ::: "memory")
#endif

#define RB_POS_MIRROR_MASK (0x80000000UL)
#define RB_POS_INDEX_MASK  (0x7FFFFFFFUL)

static inline uint32_t rb_pos_make(uint32_t mirror, uint32_t index)
{
    return ((mirror & 1U) ? RB_POS_MIRROR_MASK : 0U) | (index & RB_POS_INDEX_MASK);
}

static inline uint32_t rb_pos_index(uint32_t pos)
{
    return (pos & RB_POS_INDEX_MASK);
}

static inline uint32_t rb_pos_mirror(uint32_t pos)
{
    return (pos & RB_POS_MIRROR_MASK) ? 1U : 0U;
}

static inline void rb_set_read_pos(Ring_Buffer_Type *rb, uint32_t pos)
{
    rb->readPos = pos;
}

static inline void rb_set_write_pos(Ring_Buffer_Type *rb, uint32_t pos)
{
    rb->writePos = pos;
}

static inline uint32_t rb_advance_pos(uint32_t pos, uint32_t size, uint32_t len)
{
    uint32_t idx = rb_pos_index(pos);
    uint32_t mir = rb_pos_mirror(pos);
    uint32_t new_idx = idx + len;

    while (new_idx >= size) {
        new_idx -= size;
        mir ^= 1U;
    }

    return rb_pos_make(mir, new_idx);
}

static inline uint32_t rb_pos_linear(uint32_t pos, uint32_t size)
{
    return (rb_pos_mirror(pos) * size) + rb_pos_index(pos);
}

static inline uint32_t rb_pos_distance(uint32_t fromPos, uint32_t toPos, uint32_t size)
{
    uint32_t span = size * 2U;
    uint32_t from = rb_pos_linear(fromPos, size);
    uint32_t to = rb_pos_linear(toPos, size);

    if (to >= from) {
        return to - from;
    }

    return span - (from - to);
}

static inline uint32_t rb_len_from_positions(uint32_t readPos, uint32_t writePos, uint32_t size)
{
    uint32_t len = rb_pos_distance(readPos, writePos, size);

    /* Clamp inconsistent observations (e.g., non-SPSC debug callers) */
    if (len > size) {
        len = size;
    }

    return len;
}

static inline Ring_Buffer_Status_Type rb_status_from_len(uint32_t len, uint32_t size)
{
    if (len == 0) {
        return RING_BUFFER_EMPTY;
    }
    if (len >= size) {
        return RING_BUFFER_FULL;
    }
    return RING_BUFFER_PARTIAL;
}

/****************************************************************************/ /**
 * @brief  Get current read index of ring buffer
 *
 * @param  rbType: Ring buffer type structure pointer
 *
 * @return Current read index in range [0, size)
 *
*******************************************************************************/
uint32_t Ring_Buffer_Get_Read_Index(Ring_Buffer_Type *rbType)
{
    return rb_pos_index(rbType->readPos);
}

/****************************************************************************/ /**
 * @brief  Advance ring buffer read pointer by length bytes
 *
 * This API is intended for DMA/zero-copy style consumers where data is
 * consumed directly from the ring buffer storage.
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  length: Bytes to advance
 *
 * @return Actual advanced bytes
 *
*******************************************************************************/
uint32_t Ring_Buffer_Advance_Read(Ring_Buffer_Type *rbType, uint32_t length)
{
    uint32_t advanced = length;
    uint32_t readPos = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (advanced == 0) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
        return 0;
    }

    if (advanced > rbType->size) {
        advanced = rbType->size;
    }

    readPos = rbType->readPos;
    RB_FENCE_RW_RW();
    readPos = rb_advance_pos(readPos, rbType->size, advanced);
    rb_set_read_pos(rbType, readPos);
    RB_FENCE_RW_RW();

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return advanced;
}

/****************************************************************************/ /**
 * @brief  Ring buffer init function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  buffer: Pointer of ring buffer
 * @param  size: Size of ring buffer
 * @param  lockCb: Ring buffer lock callback function pointer
 * @param  unlockCb: Ring buffer unlock callback function pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
void Ring_Buffer_Init(Ring_Buffer_Type *rbType, uint8_t *buffer, uint32_t size, ringBuffer_Lock_Callback *lockCb,
                      ringBuffer_Lock_Callback *unlockCb)
{
    /* Init ring buffer pointer */
    rbType->pointer = buffer;

    /* Init read/write mirror and index */
    rb_set_read_pos(rbType, 0);
    rb_set_write_pos(rbType, 0);

    /* Set ring buffer size */
    rbType->size = size;

    /* Set lock and unlock callback function */
    rbType->lock = lockCb;
    rbType->unlock = unlockCb;
}

/****************************************************************************/ /**
 * @brief  Ring buffer reset function
 *
 * @param  rbType: Ring buffer type structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
void Ring_Buffer_Reset(Ring_Buffer_Type *rbType)
{
    if (rbType->lock != NULL) {
        rbType->lock();
    }

    RB_FENCE_RW_RW();
    rb_set_read_pos(rbType, 0);
    rb_set_write_pos(rbType, 0);
    RB_FENCE_RW_RW();

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }
}

/****************************************************************************/ /**
 * @brief  Use callback function to write ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  length: Length of data want to write
 * @param  writeCb: Callback function pointer
 * @param  parameter: Parameter that callback function may use
 *
 * @return Length of data actually write
 *
*******************************************************************************/
uint32_t Ring_Buffer_Write_Callback(Ring_Buffer_Type *rbType, uint32_t length, ringBuffer_Write_Callback *writeCb,
                                    void *parameter)
{
    uint32_t sizeRemained = 0;
    uint32_t writePos = 0;
    uint32_t writeIndex = 0;
    uint32_t writeMirror = 0;
    uint32_t readPos = 0;
    uint32_t writePosSnap = 0;

    if (writeCb == NULL) {
        return 0;
    }

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (rbType->lock != NULL) {
        readPos = rbType->readPos;
        writePosSnap = rbType->writePos;
        sizeRemained = rbType->size - rb_len_from_positions(readPos, writePosSnap, rbType->size);
    } else {
        /* SPSC no-lock mode: conservative empty length */
        sizeRemained = Ring_Buffer_Get_Empty_Length(rbType);
    }

    /* Ring buffer has no space for new data */
    if (sizeRemained == 0) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }

        return 0;
    }

    /* Drop part of data when length out of space remained */
    if (length > sizeRemained) {
        length = sizeRemained;
    }

    /* Get size of space remained in current mirror */
    writePos = rbType->writePos;
    writeIndex = rb_pos_index(writePos);
    writeMirror = rb_pos_mirror(writePos);
    sizeRemained = rbType->size - writeIndex;

    if (sizeRemained > length) {
        /* Space remained is enough for data in current mirror */
        writeCb(parameter, &rbType->pointer[writeIndex], length);
        RB_FENCE_RW_RW();
        writeIndex += length;
        rb_set_write_pos(rbType, rb_pos_make(writeMirror, writeIndex));
        RB_FENCE_RW_RW();
    } else {
        /* Data is divided to two parts with different mirror */
        writeCb(parameter, &rbType->pointer[writeIndex], sizeRemained);
        writeCb(parameter, &rbType->pointer[0], length - sizeRemained);
        RB_FENCE_RW_RW();
        writeIndex = length - sizeRemained;
        writeMirror ^= 1U;
        rb_set_write_pos(rbType, rb_pos_make(writeMirror, writeIndex));
        RB_FENCE_RW_RW();
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return length;
}

/****************************************************************************/ /**
 * @brief  Copy data from data buffer to ring buffer function
 *
 * @param  parameter: Pointer to source pointer
 * @param  dest: Ring buffer to write
 * @param  length: Length of data to write
 *
 * @return None
 *
*******************************************************************************/
static void Ring_Buffer_Write_Copy(void *parameter, uint8_t *dest, uint32_t length)
{
    uint8_t **src = (uint8_t **)parameter;

    arch_memcpy_fast(dest, *src, length);
    *src += length;
}

/****************************************************************************/ /**
 * @brief  Write ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Data to write
 * @param  length: Length of data
 *
 * @return Length of data writted actually
 *
*******************************************************************************/
uint32_t Ring_Buffer_Write(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length)
{
    return Ring_Buffer_Write_Callback(rbType, length, Ring_Buffer_Write_Copy, &data);
}

/****************************************************************************/ /**
 * @brief  Write 1 byte to ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Data to write
 *
 * @return Length of data writted actually
 *
*******************************************************************************/
uint32_t Ring_Buffer_Write_Byte(Ring_Buffer_Type *rbType, const uint8_t data)
{
    uint32_t writePos = 0;
    uint32_t writeIndex = 0;
    uint32_t writeMirror = 0;
    uint32_t sizeRemained = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    /* Ring buffer has no space for new data */
    if (rbType->lock != NULL) {
        uint32_t readPos = rbType->readPos;
        uint32_t writePosSnap = rbType->writePos;
        sizeRemained = rbType->size - rb_len_from_positions(readPos, writePosSnap, rbType->size);
    } else {
        sizeRemained = Ring_Buffer_Get_Empty_Length(rbType);
    }

    if (!sizeRemained) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }

        return 0;
    }

    writePos = rbType->writePos;
    writeIndex = rb_pos_index(writePos);
    writeMirror = rb_pos_mirror(writePos);

    rbType->pointer[writeIndex] = data;

    /* Publish write pointer after data is visible */
    RB_FENCE_RW_RW();

    if (writeIndex != (rbType->size - 1)) {
        writeIndex++;
    } else {
        writeIndex = 0;
        writeMirror ^= 1U;
    }

    rb_set_write_pos(rbType, rb_pos_make(writeMirror, writeIndex));
    RB_FENCE_RW_RW();

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return 1;
}

/****************************************************************************/ /**
 * @brief  Write ring buffer function, old data will be covered by new data when ring buffer is
 *         full
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Data to write
 * @param  length: Length of data
 *
 * @return Length of data writted actually
 *
*******************************************************************************/
uint32_t Ring_Buffer_Write_Force(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length)
{
    uint32_t sizeRemained = 0;
    uint32_t writePos = 0;
    uint32_t writeIndex = 0;
    uint32_t writeMirror = 0;
    uint32_t indexRemained = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    writePos = rbType->writePos;
    writeIndex = rb_pos_index(writePos);
    writeMirror = rb_pos_mirror(writePos);
    indexRemained = rbType->size - writeIndex;

    if (rbType->lock != NULL) {
        uint32_t readPos = rbType->readPos;
        uint32_t writePosSnap = rbType->writePos;
        sizeRemained = rbType->size - rb_len_from_positions(readPos, writePosSnap, rbType->size);
    } else {
        sizeRemained = Ring_Buffer_Get_Empty_Length(rbType);
    }

    /* Drop extra data when data length is large than size of ring buffer */
    if (length > rbType->size) {
        data = &data[length - rbType->size];
        length = rbType->size;
    }

    if (indexRemained > length) {
        /* Space remained is enough for data in current mirror */
        arch_memcpy_fast(&rbType->pointer[writeIndex], data, length);
        RB_FENCE_RW_RW();
        writeIndex += length;
        rb_set_write_pos(rbType, rb_pos_make(writeMirror, writeIndex));
        RB_FENCE_RW_RW();

        /* Update read index */
        if (length > sizeRemained) {
            rb_set_read_pos(rbType, rbType->writePos);
        }
    } else {
        /* Data is divided to two parts with different mirror */
        arch_memcpy_fast(&rbType->pointer[writeIndex], data, indexRemained);
        arch_memcpy_fast(&rbType->pointer[0], &data[indexRemained], length - indexRemained);
        RB_FENCE_RW_RW();
        writeIndex = length - indexRemained;
        writeMirror ^= 1U;
        rb_set_write_pos(rbType, rb_pos_make(writeMirror, writeIndex));
        RB_FENCE_RW_RW();

        /* Update read index and mirror */
        if (length > sizeRemained) {
            rb_set_read_pos(rbType, rbType->writePos);
        }
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return length;
}

/****************************************************************************/ /**
 * @brief  Write 1 byte to ring buffer function, old data will be covered by new data when ring
 *         buffer is full
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Data to write
 *
 * @return Length of data writted actually
 *
*******************************************************************************/
uint32_t Ring_Buffer_Write_Byte_Force(Ring_Buffer_Type *rbType, const uint8_t data)
{
    Ring_Buffer_Status_Type status = RING_BUFFER_EMPTY;
    uint32_t writePos = 0;
    uint32_t writeIndex = 0;
    uint32_t writeMirror = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (rbType->lock != NULL) {
        uint32_t readPos = rbType->readPos;
        uint32_t writePosSnap = rbType->writePos;
        uint32_t len = rb_len_from_positions(readPos, writePosSnap, rbType->size);
        status = rb_status_from_len(len, rbType->size);
    } else {
        status = Ring_Buffer_Get_Status(rbType);
    }

    writePos = rbType->writePos;
    writeIndex = rb_pos_index(writePos);
    writeMirror = rb_pos_mirror(writePos);

    rbType->pointer[writeIndex] = data;

    /* Publish write pointer after data is visible */
    RB_FENCE_RW_RW();

    /* Judge to change index and mirror */
    if (writeIndex == rbType->size - 1) {
        writeIndex = 0;
        writeMirror ^= 1U;

        /* Update read index and mirror */
        if (status == RING_BUFFER_FULL) {
            rb_set_read_pos(rbType, rb_pos_make(writeMirror, writeIndex));
        }
    } else {
        writeIndex++;

        /* Update read index */
        if (status == RING_BUFFER_FULL) {
            rb_set_read_pos(rbType, rb_pos_make(writeMirror, writeIndex));
        }
    }

    rb_set_write_pos(rbType, rb_pos_make(writeMirror, writeIndex));
    RB_FENCE_RW_RW();

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return 1;
}

/****************************************************************************/ /**
 * @brief  Use callback function to read ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  length: Length of data want to read
 * @param  readCb: Callback function pointer
 * @param  parameter: Parameter that callback function may use
 *
 * @return Length of data actually read
 *
*******************************************************************************/
uint32_t Ring_Buffer_Read_Callback(Ring_Buffer_Type *rbType, uint32_t length, ringBuffer_Read_Callback *readCb,
                                   void *parameter)
{
    uint32_t size = 0;
    uint32_t readPos = 0;
    uint32_t readIndex = 0;
    uint32_t readMirror = 0;
    uint32_t writePos = 0;

    if (readCb == NULL) {
        return 0;
    }

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    if (rbType->lock != NULL) {
        readPos = rbType->readPos;
        writePos = rbType->writePos;
        size = rb_len_from_positions(readPos, writePos, rbType->size);
    } else {
        size = Ring_Buffer_Get_Length(rbType);
    }

    /* Ensure subsequent buffer reads happen after observing writePos via Get_Length() */
    RB_FENCE_RW_RW();

    /* Ring buffer has no data */
    if (!size) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }

        return 0;
    }

    /* Ring buffer do not have enough data */
    if (size < length) {
        length = size;
    }

    readPos = rbType->readPos;
    readIndex = rb_pos_index(readPos);
    readMirror = rb_pos_mirror(readPos);

    /* Acquire ordering for subsequent data reads */
    RB_FENCE_RW_RW();

    /* Get size of space remained in current mirror */
    size = rbType->size - readIndex;

    if (size > length) {
        /* Read all data needed */
        readCb(parameter, &rbType->pointer[readIndex], length);
        RB_FENCE_RW_RW();
        readIndex += length;
        rb_set_read_pos(rbType, rb_pos_make(readMirror, readIndex));
        RB_FENCE_RW_RW();
    } else {
        /* Read two part of data in different mirror */
        readCb(parameter, &rbType->pointer[readIndex], size);
        readCb(parameter, &rbType->pointer[0], length - size);
        RB_FENCE_RW_RW();
        readIndex = length - size;
        readMirror ^= 1U;
        rb_set_read_pos(rbType, rb_pos_make(readMirror, readIndex));
        RB_FENCE_RW_RW();
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return length;
}

/****************************************************************************/ /**
 * @brief  Copy data from ring buffer to data buffer function
 *
 * @param  parameter: Pointer to destination pointer
 * @param  data: Data buffer to copy
 * @param  length: Length of data to copy
 *
 * @return None
 *
*******************************************************************************/
static void Ring_Buffer_Read_Copy(void *parameter, uint8_t *data, uint32_t length)
{
    uint8_t **dest = (uint8_t **)parameter;

    arch_memcpy_fast(*dest, data, length);
    *dest += length;
}

/****************************************************************************/ /**
 * @brief  Read ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Buffer for data read
 * @param  length: Length of data to read
 *
 * @return Length of data read actually
 *
*******************************************************************************/
uint32_t Ring_Buffer_Read(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length)
{
    return Ring_Buffer_Read_Callback(rbType, length, Ring_Buffer_Read_Copy, &data);
}

/****************************************************************************/ /**
 * @brief  Read 1 byte from ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Data read
 *
 * @return Length of data actually read
 *
*******************************************************************************/
uint32_t Ring_Buffer_Read_Byte(Ring_Buffer_Type *rbType, uint8_t *data)
{
    uint32_t readPos = 0;
    uint32_t readIndex = 0;
    uint32_t readMirror = 0;
    uint32_t len = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    /* Ring buffer has no data */
    if (rbType->lock != NULL) {
        uint32_t writePos = rbType->writePos;
        readPos = rbType->readPos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
    } else {
        len = Ring_Buffer_Get_Length(rbType);
    }

    if (!len) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }

        return 0;
    }

    readPos = rbType->readPos;
    readIndex = rb_pos_index(readPos);
    readMirror = rb_pos_mirror(readPos);

    /* Acquire ordering for data read */
    RB_FENCE_RW_RW();

    /* Read data */
    *data = rbType->pointer[readIndex];

    /* Publish read pointer after consumption */
    RB_FENCE_RW_RW();

    if (readIndex == rbType->size - 1) {
        readIndex = 0;
        readMirror ^= 1U;
    } else {
        readIndex++;
    }

    rb_set_read_pos(rbType, rb_pos_make(readMirror, readIndex));
    RB_FENCE_RW_RW();

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return 1;
}

/****************************************************************************/ /**
 * @brief  Read ring buffer function, do not remove from buffer actually
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Buffer for data read
 * @param  length: Length of data to read
 *
 * @return Length of data read actually
 *
*******************************************************************************/
uint32_t Ring_Buffer_Peek(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length)
{
    uint32_t size = 0;
    uint32_t readPos = 0;
    uint32_t writePos = 0;
    uint32_t readIndex = 0;
    uint32_t writeIndex = 0;
    uint32_t readMirror = 0;
    uint32_t writeMirror = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    readPos = rbType->readPos;
    writePos = rbType->writePos;
    RB_FENCE_RW_RW();
    readIndex = rb_pos_index(readPos);
    writeIndex = rb_pos_index(writePos);
    readMirror = rb_pos_mirror(readPos);
    writeMirror = rb_pos_mirror(writePos);

    if (readMirror == writeMirror) {
        size = writeIndex - readIndex;
    } else {
        size = rbType->size - (readIndex - writeIndex);
    }

    /* Ring buffer has no data */
    if (!size) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }

        return 0;
    }

    /* Ring buffer do not have enough data */
    if (size < length) {
        length = size;
    }

    /* Get size of space remained in current mirror */
    size = rbType->size - readIndex;

    if (size > length) {
        /* Read all data needed */
        arch_memcpy_fast(data, &rbType->pointer[readIndex], length);
    } else {
        /* Read two part of data in different mirror */
        arch_memcpy_fast(data, &rbType->pointer[readIndex], size);
        arch_memcpy_fast(&data[size], &rbType->pointer[0], length - size);
    }

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return length;
}

/****************************************************************************/ /**
 * @brief  Read 1 byte from ring buffer function, do not remove from buffer actually
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  data: Data read
 *
 * @return Length of data actually read
 *
*******************************************************************************/
uint32_t Ring_Buffer_Peek_Byte(Ring_Buffer_Type *rbType, uint8_t *data)
{
    uint32_t size = 0;
    uint32_t readPos = 0;
    uint32_t writePos = 0;
    uint32_t readIndex = 0;
    uint32_t writeIndex = 0;
    uint32_t readMirror = 0;
    uint32_t writeMirror = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
    }

    readPos = rbType->readPos;
    writePos = rbType->writePos;
    RB_FENCE_RW_RW();
    readIndex = rb_pos_index(readPos);
    writeIndex = rb_pos_index(writePos);
    readMirror = rb_pos_mirror(readPos);
    writeMirror = rb_pos_mirror(writePos);

    if (readMirror == writeMirror) {
        size = writeIndex - readIndex;
    } else {
        size = rbType->size - (readIndex - writeIndex);
    }

    /* Ring buffer has no data */
    if (!size) {
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }

        return 0;
    }

    /* Read data */
    *data = rbType->pointer[readIndex];

    if (rbType->unlock != NULL) {
        rbType->unlock();
    }

    return 1;
}

/****************************************************************************/ /**
 * @brief  Get length of data in ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 *
 * @return Length of data
 *
*******************************************************************************/
uint32_t Ring_Buffer_Get_Length(Ring_Buffer_Type *rbType)
{
    uint32_t readPos = 0;
    uint32_t writePos = 0;
    uint32_t len = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
        readPos = rbType->readPos;
        writePos = rbType->writePos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
    } else {
        /* SPSC no-lock: prefer an underestimate for readers (stale writePos => smaller len) */
        writePos = rbType->writePos;
        RB_FENCE_RW_RW();
        readPos = rbType->readPos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
    }

    /* Acquire barrier for subsequent buffer reads in callers */
    RB_FENCE_RW_RW();

    return len;
}

/****************************************************************************/ /**
 * @brief  Get space remained in ring buffer function
 *
 * @param  rbType: Ring buffer type structure pointer
 *
 * @return Length of space remained
 *
*******************************************************************************/
uint32_t Ring_Buffer_Get_Empty_Length(Ring_Buffer_Type *rbType)
{
    uint32_t readPos = 0;
    uint32_t writePos = 0;
    uint32_t len = 0;

    if (rbType->lock != NULL) {
        rbType->lock();
        readPos = rbType->readPos;
        writePos = rbType->writePos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
    } else {
        /* SPSC no-lock: prefer an underestimate for writers (stale readPos => smaller empty) */
        readPos = rbType->readPos;
        RB_FENCE_RW_RW();
        writePos = rbType->writePos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
    }

    return (rbType->size - len);
}

/****************************************************************************/ /**
 * @brief  Get ring buffer status function
 *
 * @param  rbType: Ring buffer type structure pointer
 *
 * @return Status of ring buffer
 *
*******************************************************************************/
Ring_Buffer_Status_Type Ring_Buffer_Get_Status(Ring_Buffer_Type *rbType)
{
    uint32_t readPos = 0;
    uint32_t writePos = 0;
    uint32_t len = 0;
    Ring_Buffer_Status_Type status;

    if (rbType->lock != NULL) {
        rbType->lock();
        readPos = rbType->readPos;
        writePos = rbType->writePos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
        if (rbType->unlock != NULL) {
            rbType->unlock();
        }
    } else {
        /* SPSC no-lock: prefer an underestimate for readers */
        writePos = rbType->writePos;
        RB_FENCE_RW_RW();
        readPos = rbType->readPos;
        len = rb_len_from_positions(readPos, writePos, rbType->size);
    }

    status = rb_status_from_len(len, rbType->size);
    return status;
}

/*@} end of group RING_BUFFER_Public_Functions */

/*@} end of group RING_BUFFER */

/*@} end of group BL_Common_Component */
