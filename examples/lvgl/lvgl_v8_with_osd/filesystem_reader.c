/**
 * @file filesystem_reader.c
 * @brief Filesystem reader implementation for JPEG images from SD card
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

#include "filesystem_reader.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "fatfs_diskio_register.h"
#include "ff.h"
#include "log.h"

/* JPEG buffers */
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(32))) static uint8_t jpg_buffers[BUFFER_COUNT][JPG_BUFFER_SIZE];

/* Buffer descriptors */
static jpg_buffer_t buffer_desc[BUFFER_COUNT];

/* Queue handles */
#if defined(CONFIG_FREERTOS)
static QueueHandle_t full_buffer_queue = NULL;  /* Queue with buffers containing JPEG data */
static QueueHandle_t empty_buffer_queue = NULL; /* Queue with empty buffers ready for file reading */
#endif

/* FATFS filesystem */
static FATFS fs;

/**
 * @brief Initialize FATFS filesystem on SD card
 */
int filesystem_init(void)
{
    FRESULT ret;
    board_sdh_gpio_init();
    fatfs_sdh_driver_register();

    /* Mount filesystem */
    ret = f_mount(&fs, "/sd", 1);

    if (ret == FR_NO_FILESYSTEM) {
        LOG_E("No filesystem found on SD card\r\n");
        return -1;
    }
    if (ret == FR_OK) {
        LOG_I("FileSystem type: %s\r\n", fs.fs_type == 1 ? "FAT12" :
                                         fs.fs_type == 2 ? "FAT16" :
                                         fs.fs_type == 3 ? "FAT32" :
                                         fs.fs_type == 4 ? "exFAT" :
                                                           "unknown");
    } else {
        LOG_E("Failed to mount filesystem, error= %d\r\n", ret);
        return -1;
    }

    return 0;
}

/**
 * @brief Initialize filesystem reader queues and buffers
 */
int filesystem_reader_init(void)
{
#if defined(CONFIG_FREERTOS)
    /* Create queues */
    full_buffer_queue = xQueueCreate(BUFFER_COUNT, sizeof(jpg_buffer_t *));
    if (full_buffer_queue == NULL) {
        LOG_E("Failed to create full buffer queue\r\n");
        return -1;
    }
    empty_buffer_queue = xQueueCreate(BUFFER_COUNT, sizeof(jpg_buffer_t *));
    if (empty_buffer_queue == NULL) {
        LOG_E("Failed to create empty buffer queue\r\n");
        vQueueDelete(full_buffer_queue);
        return -1;
    }

    /* Initialize buffer descriptors and put all buffers into empty queue */
    for (int i = 0; i < BUFFER_COUNT; i++) {
        buffer_desc[i].data = jpg_buffers[i];
        buffer_desc[i].size = 0;
        buffer_desc[i].image_idx = 0;
        jpg_buffer_t *buf_ptr = &buffer_desc[i];
        if (xQueueSend(empty_buffer_queue, &buf_ptr, (TickType_t)0) != pdTRUE) {
            LOG_E("Failed to initialize buffer %d\r\n", i);
        }
    }
    return 0;
#else
    return -1;
#endif
}

/**
 * @brief Get the full buffer queue
 */
#if defined(CONFIG_FREERTOS)
QueueHandle_t filesystem_get_full_queue(void)
{
    return full_buffer_queue;
}
#endif

/**
 * @brief Get the empty buffer queue
 */
#if defined(CONFIG_FREERTOS)
QueueHandle_t filesystem_get_empty_queue(void)
{
    return empty_buffer_queue;
}
#endif

/**
 * @brief Filesystem reader task
 *
 * This task reads JPEG files from SD card and puts filled buffers into the queue.
 * It manages a pool of buffers and cycles through all 367 images continuously.
 */
void filesystem_reader_task(void *param)
{
#if defined(CONFIG_FREERTOS)
    char filename[64];
    FIL file;
    FRESULT res;
    UINT bytes_read;
    jpg_buffer_t *buffer;
    uint16_t img_idx = 1; /* Start from pic_00001.jpg */
    uint32_t total_read = 0;
    uint32_t total_files = 0;
    static bool fs_inited = false;

    (void)param;

    /* Initialize filesystem on first run */
    if (!fs_inited) {
        LOG_I("Initializing filesystem...\r\n");
        if (filesystem_init() < 0) {
        } else {
            fs_inited = true;
        }
    }

    LOG_I("Reading from /sd/pic/pic_00001.jpg to pic_%05d.jpg\r\n", NUM_JPEG_IMAGES);

    while (1) {
        /* Get an empty buffer from the queue */
        if (xQueueReceive(empty_buffer_queue, &buffer, (TickType_t)100) == pdTRUE) {
            snprintf(filename, sizeof(filename), "/sd/pic/pic_%05d.jpg", img_idx);

            res = f_open(&file, filename, FA_READ);
            if (res == FR_OK) {
                res = f_read(&file, buffer->data, JPG_BUFFER_SIZE, &bytes_read);
                if (res == FR_OK) {
                    buffer->size = bytes_read;
                    buffer->image_idx = img_idx;

                    /* Put filled buffer into full queue */
                    if (xQueueSend(full_buffer_queue, &buffer, (TickType_t)10) != pdTRUE) {
                        LOG_W("Full buffer queue is full\r\n");
                        xQueueSend(empty_buffer_queue, &buffer, (TickType_t)0);
                    } else {
                        total_read += bytes_read;
                        total_files++;
                        if (total_files % 100 == 0) {
                            LOG_I("Read %lu files, total %lu KB\r\n", total_files, total_read / 1024);
                            total_read = 0;
                            total_files = 0;
                        }
                    }
                } else {
                    LOG_E("Failed to read file %s, error=%d\r\n", filename, res);
                    xQueueSend(empty_buffer_queue, &buffer, (TickType_t)0);
                }
                f_close(&file);
            } else {
                LOG_E("Failed to open file %s, error=%d\r\n", filename, res);
                xQueueSend(empty_buffer_queue, &buffer, (TickType_t)0);
            }

            img_idx++;
            if (img_idx > NUM_JPEG_IMAGES) {
                img_idx = 1;
                LOG_I("Completed one cycle (%d images), restarting...\r\n", NUM_JPEG_IMAGES);
            }
        } else {
            vTaskDelay(1);
        }
    }
#else
    (void)param;
    while (1) {
        vTaskDelay(1000);
    }
#endif
}
