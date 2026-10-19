/**
 * @file dpi_manager.h
 * @brief DPI Manager - Encapsulates DPI and MJDEC initialization and control logic
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

#ifndef _DPI_MANAGER_H_
#define _DPI_MANAGER_H_

#include <stdint.h>
#include "bflb_core.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "semphr.h"
#endif

/* Use SW GPIO for DPI pixel clock output */
#define DPI_PIXEL_CLOCK_USE_SW_GPIO 1

/* LCD configuration */
#define LCD_WIDTH                   800
#define LCD_HEIGHT                  480
#define LCD_PIXEL                   (LCD_WIDTH * LCD_HEIGHT)

/**
 * @brief Initialize DPI and MJDEC with default configuration
 *
 * @return int 0 on success, negative value on error
 */
int dpi_manager_init(void);

/**
 * @brief Parse JPEG SOF0 header to get image info
 */
uint8_t dpi_parse_sof0(uint8_t *header, uint16_t *x, uint16_t *y, uint8_t *format, uint16_t *skip);

/**
 * @brief Decode one JPEG frame using MJDEC
 */
void dpi_mjdec_decode_one_frame(uint8_t *jpg, uint8_t *output);

/**
 * @brief Stop MJDEC and pop one frame
 */
void dpi_mjdec_stop(void);

/**
 * @brief MJDEC interrupt handler (exported for irq_attach)
 */
void dpi_mjdec_isr(int irq, void *arg);

/**
 * @brief Get picture count flag
 */
volatile uint32_t *dpi_mjdec_get_pic_count(void);

#if defined(CONFIG_FREERTOS)
/**
 * @brief Image switch task - processes JPEG buffers from filesystem
 * @param param Task parameter (unused)
 */
void image_switch_task(void *param);
#endif

#endif /* _DPI_MANAGER_H_ */
