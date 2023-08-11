/**
 * @file lcd.h
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

#ifndef _LCD_H_
#define _LCD_H_

#include "font.h"
#include "bflb_core.h"
#include "lcd_conf.h"

/* Do not modify the following */

#define LCD_INTERFACE_SPI       1
#define LCD_INTERFACE_DBI       2
#define LCD_INTERFACE_DPI       3
#define LCD_INTERFACE_DSI_VIDIO 4

#if defined LCD_DBI_ILI9488

#include "mipi_dbi/ili9488_dbi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DBI
#define LCD_W                        ILI9488_DBI_W
#define LCD_H                        ILI9488_DBI_H
#define LCD_COLOR_DEPTH              ILI9488_DBI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9488_dbi_##_func(__VA_ARGS__)

#elif defined LCD_DBI_ILI9486

#include "mipi_dbi/ili9486_dbi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DBI
#define LCD_W                        ILI9486_DBI_W
#define LCD_H                        ILI9486_DBI_H
#define LCD_COLOR_DEPTH              ILI9486_DBI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9486_dbi_##_func(__VA_ARGS__)

#elif defined LCD_DBI_ILI9341

#include "mipi_dbi/ili9341_dbi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DBI
#define LCD_W                        ILI9341_DBI_W
#define LCD_H                        ILI9341_DBI_H
#define LCD_COLOR_DEPTH              ILI9341_DBI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9341_dbi_##_func(__VA_ARGS__)

#elif defined LCD_DBI_NT35510

#include "mipi_dbi/nt35510_dbi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DBI
#define LCD_W                        NT35510_DBI_W
#define LCD_H                        NT35510_DBI_H
#define LCD_COLOR_DEPTH              NT35510_DBI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) nt35510_dbi_##_func(__VA_ARGS__)

#elif defined LCD_DBI_ST7796

#include "mipi_dbi/st7796_dbi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DBI
#define LCD_W                        ST7796_DBI_W
#define LCD_H                        ST7796_DBI_H
#define LCD_COLOR_DEPTH              ST7796_DBI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) st7796_dbi_##_func(__VA_ARGS__)

#elif defined LCD_DISP_QSPI_GC9C01

#include "disp_qspi/gc9c01_disp_qspi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DBI
#define LCD_W                        GC9C01_DISP_QSPI_W
#define LCD_H                        GC9C01_DISP_QSPI_H
#define LCD_COLOR_DEPTH              GC9C01_DISP_QSPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) gc9c01_disp_qspi_##_func(__VA_ARGS__)

#elif defined LCD_DPI_ILI9488

#include "mipi_dpi/ili9488_dpi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DPI
#define LCD_W                        ILI9488_DPI_W
#define LCD_H                        ILI9488_DPI_H
#define LCD_COLOR_DEPTH              ILI9488_DPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9488_dpi_##_func(__VA_ARGS__)

#elif defined LCD_DPI_GC9503V

#include "mipi_dpi/gc9503v_dpi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DPI
#define LCD_W                        GC9503V_DPI_W
#define LCD_H                        GC9503V_DPI_H
#define LCD_COLOR_DEPTH              GC9503V_DPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) gc9503v_dpi_##_func(__VA_ARGS__)

#elif defined LCD_DPI_ST7701S

#include "mipi_dpi/st7701s_dpi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DPI
#define LCD_W                        ST7701S_DPI_W
#define LCD_H                        ST7701S_DPI_H
#define LCD_COLOR_DEPTH              ST7701S_DPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) st7701s_dpi_##_func(__VA_ARGS__)

#elif defined LCD_DPI_STANDARD

#include "mipi_dpi/standard_dpi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DPI
#define LCD_W                        STANDARD_DPI_W
#define LCD_H                        STANDARD_DPI_H
#define LCD_COLOR_DEPTH              STANDARD_DPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) standard_dpi_##_func(__VA_ARGS__)

#elif defined LCD_DSI_VIDIO_ILI9881C

#include "mipi_dsi/ili9881c_dsi_vidio.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_DSI_VIDIO
#define LCD_W                        ILI9881C_DSI_VIDIO_W
#define LCD_H                        ILI9881C_DSI_VIDIO_H
#define LCD_COLOR_DEPTH              ILI9881C_DSI_VIDIO_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9881c_dsi_vidio_##_func(__VA_ARGS__)

#elif defined LCD_SPI_ILI9488

#include "spi/ili9488_spi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_SPI
#define LCD_W                        ILI9488_SPI_W
#define LCD_H                        ILI9488_SPI_H
#define LCD_COLOR_DEPTH              ILI9488_SPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9488_spi_##_func(__VA_ARGS__)

#elif defined LCD_SPI_ILI9341

#include "spi/ili9341_spi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_SPI
#define LCD_W                        ILI9341_SPI_W
#define LCD_H                        ILI9341_SPI_H
#define LCD_COLOR_DEPTH              ILI9341_SPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) ili9341_spi_##_func(__VA_ARGS__)

#elif defined LCD_SPI_ST7796

#include "spi/st7796_spi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_SPI
#define LCD_W                        ST7796_SPI_W
#define LCD_H                        ST7796_SPI_H
#define LCD_COLOR_DEPTH              ST7796_SPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) st7796_spi_##_func(__VA_ARGS__)

#elif defined LCD_SPI_ST7789V

#include "spi/st7789v_spi.h"
#define LCD_INTERFACE_TYPE           LCD_INTERFACE_SPI
#define LCD_W                        ST7789V_SPI_W
#define LCD_H                        ST7789V_SPI_H
#define LCD_COLOR_DEPTH              ST7789V_SPI_COLOR_DEPTH
#define _LCD_FUNC_DEFINE(_func, ...) st7789v_spi_##_func(__VA_ARGS__)

#else
#error "Please select a screen type"
#endif

#define LCD_COLOR_RGB888(r, g, b) (((r << 16) | (g << 8) | (b)) & 0xffffff)
#define LCD_COLOR_RGB565(r, g, b) (((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)) & 0xffff)

#if (LCD_COLOR_DEPTH == 16)
typedef uint16_t lcd_color_t;
#define LCD_COLOR_RGB(r, g, b) LCD_COLOR_RGB565(r, g, b)
#elif (LCD_COLOR_DEPTH == 32)
typedef uint32_t lcd_color_t;
#define LCD_COLOR_RGB(r, g, b) LCD_COLOR_RGB888(r, g, b)
#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

/* MCU LCD Common interface */
#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DBI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_SPI)

extern uint16_t lcd_max_x, lcd_max_y;

int lcd_init(void);
int lcd_async_callback_register(void (*callback)(void));
int lcd_set_dir(uint8_t dir, uint8_t mir_flag);
int lcd_draw_point(uint16_t x, uint16_t y, lcd_color_t color);
int lcd_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_clear(lcd_color_t color);
int lcd_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture);
int lcd_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture);
int lcd_draw_is_busy(void);
int lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, lcd_color_t color);
#if FONT_ASCII_16X8
int lcd_draw_str_ascii16(uint16_t x, uint16_t y, lcd_color_t color, lcd_color_t bk_color, uint8_t *str, uint8_t num);
#endif

/* RGB LCD Common interface */
#elif (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI) || (LCD_INTERFACE_TYPE == LCD_INTERFACE_DSI_VIDIO)

/* frame int callback and frame swap int callback */
#define FRAME_INT_TYPE_CYCLE 0
#define FRAME_INT_TYPE_SWAP  1

int lcd_init(lcd_color_t *screen_buffer);
int lcd_screen_switch(lcd_color_t *screen_buffer);
lcd_color_t *lcd_get_screen_using(void);
int lcd_frame_callback_register(uint32_t callback_type, void (*callback)(void));

int lcd_clear(lcd_color_t *screen_buffer, lcd_color_t color);
int lcd_draw_point(lcd_color_t *screen_buffer, uint16_t x, uint16_t y, lcd_color_t color);
int lcd_draw_area(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_picture(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture);
int lcd_draw_line(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_rectangle(lcd_color_t *screen_buffer, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_circle(lcd_color_t *screen_buffer, uint16_t x, uint16_t y, uint16_t r, lcd_color_t color);
#if FONT_ASCII_16X8
int lcd_draw_str_ascii16(lcd_color_t *screen_buffer, uint16_t x, uint16_t y, lcd_color_t color, lcd_color_t bk_color, uint8_t *str, uint8_t num);
#endif

#endif

#endif