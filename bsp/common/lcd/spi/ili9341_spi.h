/**
 * @file ili9341.h
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

#ifndef _ILI9341_SPI_H_
#define _ILI9341_SPI_H_

#include "bflb_core.h"

#define ILI9341_SPI_CS_PIN GPIO_PIN_16
#define ILI9341_SPI_DC_PIN GPIO_PIN_18

#define ILI9341_SPI_CS_HIGH bflb_gpio_set(ili9341_gpio, ILI9341_SPI_CS_PIN)
#define ILI9341_SPI_CS_LOW  bflb_gpio_reset(ili9341_gpio, ILI9341_SPI_CS_PIN)
#define ILI9341_SPI_DC_HIGH bflb_gpio_set(ili9341_gpio, ILI9341_SPI_DC_PIN)
#define ILI9341_SPI_DC_LOW  bflb_gpio_reset(ili9341_gpio, ILI9341_SPI_DC_PIN)

#define ILI9341_SPI_W 240 /* ILI9341 LCD width */
#define ILI9341_SPI_H 320 /* ILI9341 LCD height */

#define ILI9341_SPI_OFFSET_X 0
#define ILI9341_SPI_OFFSET_Y 0

#define ILI9341_SPI_COLOR_DEPTH 16

typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} ili9341_spi_init_cmd_t;

typedef uint16_t ili9341_spi_color_t;

int ili9341_spi_init();
void ili9341_spi_async_callback_register(void (*callback)(void));
int ili9341_spi_set_dir(uint8_t dir, uint8_t mir_flag);
void ili9341_spi_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void ili9341_spi_draw_point(uint16_t x, uint16_t y, ili9341_spi_color_t color);
void ili9341_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t color);
void ili9341_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture);
void ili9341_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture);
int ili9341_spi_draw_is_busy(void);

#endif