/**
 * @file filesystem_reader.h
 * @brief Filesystem reader for JPEG images from SD card
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

#ifndef _FILESYSTEM_READER_H
#define _FILESYSTEM_READER_H

#include <stdint.h>
#include <stddef.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "queue.h"
#endif

/* Configuration */
#define JPG_BUFFER_SIZE (58 * 1024) /* 58KB max JPEG size */
#define BUFFER_COUNT    2           /* 2 buffers for queue (114KB total, fits in NOCACHE_RAM) */
#define NUM_JPEG_IMAGES 367         /* Total 367 JPG files */

/**
 * @brief JPEG buffer descriptor
 */
typedef struct {
    uint8_t *data;      /* Pointer to JPEG data */
    size_t size;        /* Actual data size */
    uint16_t image_idx; /* Image index (1-367) */
} jpg_buffer_t;

/**
 * @brief Initialize FATFS filesystem on SD card
 * @return 0 on success, -1 on failure
 */
int filesystem_init(void);

/**
 * @brief Initialize filesystem reader queues and buffers
 * @return 0 on success, -1 on failure
 */
int filesystem_reader_init(void);

/**
 * @brief Get the full buffer queue (contains buffers with JPEG data)
 * @return Queue handle
 */
#if defined(CONFIG_FREERTOS)
QueueHandle_t filesystem_get_full_queue(void);
#endif

/**
 * @brief Get the empty buffer queue (contains empty buffers for reading)
 * @return Queue handle
 */
#if defined(CONFIG_FREERTOS)
QueueHandle_t filesystem_get_empty_queue(void);
#endif

/**
 * @brief Filesystem reader task
 * @param param Task parameter (unused)
 */
void filesystem_reader_task(void *param);

#endif /* _FILESYSTEM_READER_H */
