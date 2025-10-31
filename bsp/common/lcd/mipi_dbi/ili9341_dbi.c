/**
 * @file ili9341_dbi.c
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

#include "../lcd.h"
#include "ili9341_dbi.h"

#if defined(LCD_DBI_ILI9341)

#if (LCD_DBI_INTERFACE_TYPE == 1)
/* dbi */
#include "bl_mipi_dbi.h"

#define lcd_dbi_init                          lcd_dbi_init
#define lcd_dbi_isbusy                        lcd_dbi_is_busy

#define lcd_dbi_transmit_cmd_para             lcd_dbi_transmit_cmd_para
#define lcd_dbi_transmit_cmd_pixel_sync       lcd_dbi_transmit_cmd_pixel_sync
#define lcd_dbi_transmit_cmd_pixel_fill_sync  lcd_dbi_transmit_cmd_pixel_fill_sync

#define lcd_dbi_sync_callback_enable          lcd_dbi_async_callback_enable
#define lcd_dbi_async_callback_register       lcd_dbi_async_callback_register
#define lcd_dbi_transmit_cmd_pixel_async      lcd_dbi_transmit_cmd_pixel_async
#define lcd_dbi_transmit_cmd_pixel_fill_async lcd_dbi_transmit_cmd_pixel_fill_async

lcd_dbi_init_t dbi_para = {
#if (LCD_DBI_WORK_MODE == 3)
    /* typeB */
    .clock_freq = 27 * 1000 * 1000,
#else
    /* typeC */
    .clock_freq = 40 * 1000 * 1000,
#endif

#if (ILI9341_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (ILI9341_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)
#else

#error "Configuration error"

#endif

const ili9341_dbi_init_cmd_t ili9341_dbi_init_cmds[] = {
    { 0x01, NULL, 0 },
    { 0xFF, NULL, 20 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 120 },

    { 0xCF, "\x00\xd9\x30", 3 },
    { 0xED, "\x64\x03\x12\x81", 4 },
    { 0xE8, "\x85\x01\x78", 3 },
    { 0xCB, "\x39\x2C\x00\x34\x02", 5 },
    { 0xF7, "\x20", 1 },
    { 0xEA, "\x00\x00", 2 },

    { 0xC0, "\x23", 1 }, /*Power control*/
    { 0xC1, "\x12", 1 }, /*Power control */
    { 0xC2, "\x11", 1 },
    { 0xC5, "\x40\x30", 2 }, /*VCOM control 1*/
    { 0xC7, "\xa9", 1 },     /*VCOM control 2*/

#if (ILI9341_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (ILI9341_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif

    { 0xB1, "\x00\x18", 2 }, /* Frame Rate Control */
    { 0xB6, "\x0a\xa2", 2 }, /* Display Function Control */
    { 0x0C, "\xd5", 1 },     /* display pixel format,RGB 16bits,MCU 16bits  */
    { 0xF2, "\x00", 1 },     /* 3Gamma Function Disable */
    { 0xF7, "\x20", 1 },

    { 0x26, "\x01", 1 },                                                          /* Gamma curve selected */
    { 0xE0, "\x1F\x1A\x18\x0A\x0F\x06\x45\x87\x32\x0A\x07\x02\x07\x05\x00", 15 }, /* Set Gamma */
    { 0XE1, "\x00\x25\x27\x05\x10\x09\x3A\x78\x4D\x05\x18\x0D\x38\x3A\x1F", 15 }, /* Set Gamma */
    { 0xB7, "\x07", 1 },

#if ILI9341_DBI_COLOR_REVERSAL
    { 0x21, NULL, 0 }, /* Color reversal */
#endif

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

/**
 * @brief ili9341_dbi_async_callback_enable
 *
 * @return
 */
void ili9341_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief ili9341_dbi_async_callback_register
 *
 * @return
 */
void ili9341_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief ili9341_dbi_draw_is_busy, After the call ili9341_dbi_draw_picture_dma must check this,
 *         if ili9341_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int ili9341_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief ili9341_dbi_init
 *
 * @return int
 */
int ili9341_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(ili9341_dbi_init_cmds) / sizeof(ili9341_dbi_init_cmd_t)); i++) {
        if (ili9341_dbi_init_cmds[i].cmd == 0xFF && ili9341_dbi_init_cmds[i].data == NULL && ili9341_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(ili9341_dbi_init_cmds[i].databytes);
        } else {
            lcd_dbi_transmit_cmd_para(ili9341_dbi_init_cmds[i].cmd, (void *)(ili9341_dbi_init_cmds[i].data), ili9341_dbi_init_cmds[i].databytes);
        }
    }

    ili9341_dbi_set_dir(0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int ili9341_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t dir_param[4] = { 0x00, 0x60, 0xA0, 0xC0 };
    uint8_t mir_param[4] = { 0x40, 0xE0, 0x20, 0x80 };
    uint8_t param;

    if (dir >= 4) {
        return -1;
    }

#if ILI9341_DBI_DIR_MIRROR
    mir_flag = !mir_flag;
#endif

    if (mir_flag) {
        param = mir_param[dir];
    } else {
        param = dir_param[dir];
    }

/* Color RGB order */
#if ILI9341_DBI_COLOR_ORDER
    param |= 0x08;
#endif

    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
    return dir;
}

/**
 * @brief ili9341_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void ili9341_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ILI9341_DBI_OFFSET_X
    x1 += ILI9341_DBI_OFFSET_X;
    x2 += ILI9341_DBI_OFFSET_X;
#endif
#if ILI9341_DBI_OFFSET_Y
    y1 += ILI9341_DBI_OFFSET_Y;
    y2 += ILI9341_DBI_OFFSET_Y;
#endif

    uint8_t param[4];

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
    lcd_dbi_transmit_cmd_para(0x2B, (void *)param, 4);

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
    lcd_dbi_transmit_cmd_para(0x2A, (void *)param, 4);
}

/**
 * @brief ili9341_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void ili9341_dbi_draw_point(uint16_t x, uint16_t y, ili9341_dbi_color_t color)
{
    /* set window */
    ili9341_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief ili9341_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9341_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if ILI9341_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif ILI9341_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    ili9341_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
}

/**
 * @brief ili9341_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (ili9341_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void ili9341_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    ili9341_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
}

/**
 * @brief ili9341_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9341_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    ili9341_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
}

#endif
