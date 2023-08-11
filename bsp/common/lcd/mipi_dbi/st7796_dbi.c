/**
 * @file st7796_dbi.c
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
#include "st7796_dbi.h"

#if defined(LCD_DBI_ST7796)

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
    .clock_freq = 40 * 1000 * 1000,
#if (ST7796_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (ST7796_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)

#else

#error "Configuration error"

#endif

const st7796_dbi_init_cmd_t st7796_dbi_init_cmds[] = {
    { 0x01, NULL, 0 },
    { 0xFF, NULL, 10 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 120 },

    { 0xF0, "\xC3", 1 },
    { 0xF0, "\x96", 1 },

    { 0x36, "\x00", 1 },

#if (ST7796_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (ST7796_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif

    { 0xE6, "\x0F\xF2\x3F\x4F\x4F\x28\x0E\x00", 8 },
    { 0xC5, "\x2A", 1 },

/* Color reversal */
#if ST7796_DBI_COLOR_REVERSAL
    { 0xB4, "\x01", 1 },
    { 0x21, NULL, 0 },
#endif

    { 0xE0, "\xF0\x03\x0A\x11\x14\x1C\x3B\x55\x4A\x0A\x13\x14\x1C\x1F", 14 }, /* Set Gamma */
    { 0XE1, "\xF0\x03\x0A\x0C\x0C\x09\x36\x54\x49\x0F\x1B\x18\x1B\x1F", 14 }, /* Set Gamma */

    { 0xF0, "\x3C", 1 },
    { 0xF0, "\x69", 1 },

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

/**
 * @brief st7796_dbi_async_callback_enable
 *
 * @return
 */
void st7796_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief st7796_dbi_async_callback_register
 *
 * @return
 */
void st7796_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief st7796_dbi_draw_is_busy, After the call st7796_dbi_draw_picture_dma must check this,
 *         if st7796_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7796_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief st7796_dbi_init
 *
 * @return int
 */
int st7796_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(st7796_dbi_init_cmds) / sizeof(st7796_dbi_init_cmd_t)); i++) {
        if (st7796_dbi_init_cmds[i].cmd == 0xFF && st7796_dbi_init_cmds[i].data == NULL && st7796_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(st7796_dbi_init_cmds[i].databytes);
        } else {
            lcd_dbi_transmit_cmd_para(st7796_dbi_init_cmds[i].cmd, (void *)(st7796_dbi_init_cmds[i].data), st7796_dbi_init_cmds[i].databytes);
        }
    }

    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int st7796_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;
    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x00;
            else
                param = 0x40;
            break;
        case 1:
            if (!mir_flag)
                param = 0x20;
            else
                param = 0xA0;
            break;
        case 2:
            if (!mir_flag)
                param = 0x80;
            else
                param = 0xC0;
            break;
        case 3:
            if (!mir_flag)
                param = 0xE0;
            else
                param = 0x60;

            break;
        default:
            return -1;
            break;
    }
    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
    return dir;
}

/**
 * @brief st7796_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st7796_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ST7796_DBI_OFFSET_X
    x1 += ST7796_DBI_OFFSET_X;
    x2 += ST7796_DBI_OFFSET_X;
#endif
#if ST7796_DBI_OFFSET_Y
    y1 += ST7796_DBI_OFFSET_Y;
    y2 += ST7796_DBI_OFFSET_Y;
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
 * @brief st7796_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7796_dbi_draw_point(uint16_t x, uint16_t y, st7796_dbi_color_t color)
{
    /* set window */
    st7796_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief st7796_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7796_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7796_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if ST7796_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif ST7796_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    st7796_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
}

/**
 * @brief st7796_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7796_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void st7796_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7796_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7796_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
}

/**
 * @brief st7796_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7796_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7796_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7796_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
}

#endif
