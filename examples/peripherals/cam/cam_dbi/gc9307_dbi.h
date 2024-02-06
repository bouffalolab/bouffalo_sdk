/**
 * @file gc9307_dbi.h
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

#ifndef _GC9307_DBI_H_
#define _GC9307_DBI_H_

#include "bflb_core.h"

/* Selecting pixel format
    1: rgb565 (16-bit, output rgb565)
    2: nrgb8888 (32-bit, output rgb888)
*/
#define GC9307_DBI_PIXEL_FORMAT 1

/* GC9307 LCD width and height */
#define GC9307_DBI_W ((uint16_t)320)
#define GC9307_DBI_H ((uint16_t)240)

/* The offset of the area can be displayed */
#define GC9307_DBI_OFFSET_X 0
#define GC9307_DBI_OFFSET_Y 0

/* Color reversal, Some screens are required
    0: disable
    1: enable
*/
#define GC9307_DBI_COLOR_REVERSAL 0

/* dma used by DBI */
#define LCD_DBI_DMA_NAME "dma0_ch3"

/* The maximum amount of data to be transferred affects the number of LLI memory pools */
#define DBI_DBI_DATA_SIZE_MAX (800 * 640 * 4)

/* pin cfg */
#define LCD_DBI_TYPEB_PIN_WR    GPIO_PIN_4
#define LCD_DBI_TYPEB_PIN_DC    GPIO_PIN_7
#define LCD_DBI_TYPEB_PIN_CS    GPIO_PIN_16
#define LCD_DBI_TYPEB_PIN_RST    GPIO_PIN_17

#define LCD_DBI_TYPEB_PIN_DAT0  GPIO_PIN_8
#define LCD_DBI_TYPEB_PIN_DAT1  GPIO_PIN_9
#define LCD_DBI_TYPEB_PIN_DAT2  GPIO_PIN_10
#define LCD_DBI_TYPEB_PIN_DAT3  GPIO_PIN_11
#define LCD_DBI_TYPEB_PIN_DAT4  GPIO_PIN_12
#define LCD_DBI_TYPEB_PIN_DAT5  GPIO_PIN_13
#define LCD_DBI_TYPEB_PIN_DAT6  GPIO_PIN_14
#define LCD_DBI_TYPEB_PIN_DAT7  GPIO_PIN_15

/* Do not modify the following */
#define GC9307_DBI_COLOR_DEPTH 32
typedef uint32_t gc9307_dbi_color_t;

/* Optional pixel format */
#define LCD_DBI_LCD_PIXEL_FORMAT_RGB565   1
#define LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888 2

typedef struct {
    uint32_t pixel_format;
    uint32_t clock_freq; /* No use */
} lcd_dbi_init_t;

typedef struct {
    uint8_t cmd; /* 0xFF : delay(databytes)ms */
    const char *data;
    uint8_t databytes; /* Num of data in data; or delay time */
} gc9307_dbi_init_cmd_t;

int lcd_dbi_init(lcd_dbi_init_t *dbi_parra);
int lcd_dbi_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num);
int lcd_dbi_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int lcd_dbi_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);
int lcd_dbi_is_busy(void);
int lcd_dbi_async_callback_enable(bool enable);
int lcd_dbi_async_callback_register(void (*callback)(void));
int lcd_dbi_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num);
int lcd_dbi_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num);

int gc9307_dbi_init();
int gc9307_dbi_set_dir(uint8_t dir, uint8_t mir_flag);
int gc9307_dbi_draw_is_busy(void);
void gc9307_dbi_async_callback_enable(bool enable);
void gc9307_dbi_async_callback_register(void (*callback)(void));
void gc9307_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void gc9307_dbi_draw_point(uint16_t x, uint16_t y, gc9307_dbi_color_t color);
void gc9307_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t color);
void gc9307_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t *picture);
void gc9307_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t *picture);

#endif
