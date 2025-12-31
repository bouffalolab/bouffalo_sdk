/**
 * @file ring_buffer.h
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
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#ifndef RING_BUFFER_HOST_TEST
#include "bflb_core.h"
#else
/* Host test build: avoid pulling in MCU chip headers and provide minimal defs. */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

static inline void arch_memcpy_fast(void *dst, const void *src, uint32_t len)
{
    (void)memcpy(dst, src, (size_t)len);
}
#endif

/** @addtogroup  BL_Common_Component
 *  @{
 */

/** @addtogroup  RING_BUFFER
 *  @{
 */

/** @defgroup  RING_BUFFER_Public_Types
 *  @{
 */

/**
 *  @brief Ring buffer status type definition
 */
typedef enum {
    RING_BUFFER_EMPTY,   /*!< Ring buffer is empty */
    RING_BUFFER_PARTIAL, /*!< Ring buffer has partial data */
    RING_BUFFER_FULL,    /*!< Ring buffer is full */
} Ring_Buffer_Status_Type;

/**
 *  @brief Ring buffer structure definition
 */
typedef struct {
    uint8_t *pointer;           /*!< Pointer of ring buffer */
    volatile uint32_t readPos;  /*!< Packed read position: bit31=mirror, bit[30:0]=index */
    volatile uint32_t writePos; /*!< Packed write position: bit31=mirror, bit[30:0]=index */
    uint32_t size;              /*!< Size of ring buffer */
    void (*lock)(void);         /*!< Lock ring buffer */
    void (*unlock)(void);       /*!< Unlock ring buffer */
} Ring_Buffer_Type;

/*@} end of group RING_BUFFER_Public_Types */

/** @defgroup  RING_BUFFER_Public_Constants
 *  @{
 */

/** @defgroup  RING_BUFFER_STATUS_TYPE
 *  @{
 */
#define IS_RING_BUFFER_STATUS_TYPE(type) \
    (((type) == RING_BUFFER_EMPTY) || ((type) == RING_BUFFER_PARTIAL) || ((type) == RING_BUFFER_FULL))

/*@} end of group RING_BUFFER_Public_Constants */

/** @defgroup  RING_BUFFER_Public_Macros
 *  @{
 */
typedef void(ringBuffer_Lock_Callback)(void);
typedef void(ringBuffer_Read_Callback)(void *, uint8_t *, uint32_t);
typedef void(ringBuffer_Write_Callback)(void *, uint8_t *, uint32_t);

/*@} end of group RING_BUFFER_Public_Macros */

/** @defgroup  RING_BUFFER_Public_Functions
 *  @{
 */
void Ring_Buffer_Init(Ring_Buffer_Type *rbType, uint8_t *buffer, uint32_t size, ringBuffer_Lock_Callback *lockCb,
                      ringBuffer_Lock_Callback *unlockCb);
void Ring_Buffer_Reset(Ring_Buffer_Type *rbType);
uint32_t Ring_Buffer_Write_Callback(Ring_Buffer_Type *rbType, uint32_t length, ringBuffer_Write_Callback *writeCb,
                                    void *parameter);
uint32_t Ring_Buffer_Write(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Write_Byte(Ring_Buffer_Type *rbType, const uint8_t data);
uint32_t Ring_Buffer_Write_Force(Ring_Buffer_Type *rbType, const uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Write_Byte_Force(Ring_Buffer_Type *rbType, const uint8_t data);
uint32_t Ring_Buffer_Read_Callback(Ring_Buffer_Type *rbType, uint32_t length, ringBuffer_Read_Callback *readCb,
                                   void *parameter);
uint32_t Ring_Buffer_Read(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Read_Byte(Ring_Buffer_Type *rbType, uint8_t *data);
uint32_t Ring_Buffer_Peek(Ring_Buffer_Type *rbType, uint8_t *data, uint32_t length);
uint32_t Ring_Buffer_Peek_Byte(Ring_Buffer_Type *rbType, uint8_t *data);
uint32_t Ring_Buffer_Get_Length(Ring_Buffer_Type *rbType);
uint32_t Ring_Buffer_Get_Empty_Length(Ring_Buffer_Type *rbType);
Ring_Buffer_Status_Type Ring_Buffer_Get_Status(Ring_Buffer_Type *rbType);

/**
 * @brief  Get current read index of ring buffer
 *
 * @param  rbType: Ring buffer type structure pointer
 *
 * @return Current read index in range [0, size)
 */
uint32_t Ring_Buffer_Get_Read_Index(Ring_Buffer_Type *rbType);

/**
 * @brief  Advance ring buffer read pointer by length bytes
 *
 * This API is intended for DMA/zero-copy style consumers where data is
 * consumed directly from the ring buffer storage.
 *
 * @param  rbType: Ring buffer type structure pointer
 * @param  length: Bytes to advance
 *
 * @return Actual advanced bytes
 */
uint32_t Ring_Buffer_Advance_Read(Ring_Buffer_Type *rbType, uint32_t length);

/*@} end of group RING_BUFFER_Public_Functions */

/*@} end of group RING_BUFFER */

/*@} end of group BL_Common_Component */

#endif /* __RING_BUFFER_H__ */
