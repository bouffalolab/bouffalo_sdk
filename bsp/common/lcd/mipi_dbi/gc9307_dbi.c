/**
 * @file gc9307_dbi.c
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
#include "gc9307_dbi.h"

#if defined(LCD_DBI_GC9307)

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

#if (GC9307_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (GC9307_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)

#else

#error "Configuration error"

#endif

const gc9307_dbi_init_cmd_t gc9307_dbi_init_cmds[] = {
    { 0xFE, NULL, 0 },
    { 0xEF, NULL, 0 },

    { 0x86, "\x98", 1 },
    { 0x89, "\x03", 1 },
    { 0x8B, "\x80", 1 },
    { 0x8D, "\x22", 1 },
    { 0x8E, "\x0F", 1 },
    { 0xE8, "\x12\x00", 2 },
    { 0xC3, "\x47", 1 },
    { 0xC4, "\x28", 1 },
    { 0xC9, "\x08", 1 },
    { 0xFF, "\x62", 1 },
    { 0x99, "\x3E", 1 },
    { 0x9D, "\x4B", 1 },

    { 0xF0, "\x07\x0B\x0C\x0A\x06\x31", 6 },
    { 0xF2, "\x07\x07\x04\x06\x06\x21", 6 },
    { 0xF1, "\x4A\x78\x76\x33\x2F\xAF", 6 },
    { 0xF3, "\x38\x74\x72\x22\x28\x6F", 6 },

    { 0x35, "\x00", 1 },
    { 0x44, "\x00\x0A", 2 },

/* Color RGB order */
#if GC9307_DBI_COLOR_ORDER
    { 0x36, "\x08", 1 },
#else
    { 0x36, "\x00", 1 },
#endif

#if (GC9307_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (GC9307_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif

    { 0x11, NULL, 0 },   /* sleep out */
    { 0xFF, NULL, 120 }, /* delay 120ms */
    { 0x29, NULL, 0 },   /* display on */
    { 0xFF, NULL, 10 },
};

/**
 * @brief gc9307_dbi_async_callback_enable
 *
 * @return
 */
void gc9307_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief gc9307_dbi_async_callback_register
 *
 * @return
 */
void gc9307_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief gc9307_dbi_draw_is_busy, After the call gc9307_dbi_draw_picture_dma must check this,
 *         if gc9307_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int gc9307_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief gc9307_dbi_init
 *
 * @return int
 */
int gc9307_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(gc9307_dbi_init_cmds) / sizeof(gc9307_dbi_init_cmd_t)); i++) {
        if (gc9307_dbi_init_cmds[i].cmd == 0xFF && gc9307_dbi_init_cmds[i].data == NULL && gc9307_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(gc9307_dbi_init_cmds[i].databytes);
        } else {
            lcd_dbi_transmit_cmd_para(gc9307_dbi_init_cmds[i].cmd, (void *)(gc9307_dbi_init_cmds[i].data), gc9307_dbi_init_cmds[i].databytes);
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
int gc9307_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t dir_param[4] = { 0x00, 0xA0, 0xC0, 0x60 };
    uint8_t mir_param[4] = { 0x40, 0x20, 0x80, 0xE0 };
    uint8_t param;

    if (dir >= 4) {
        return -1;
    }

    if (mir_flag) {
        param = mir_param[dir];
    } else {
        param = dir_param[dir];
    }

/* Color RGB order */
#if GC9307_DBI_COLOR_ORDER
    param |= 0x08;
#endif

    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
    return dir;
}

/**
 * @brief gc9307_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void gc9307_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if GC9307_DBI_OFFSET_X
    x1 += GC9307_DBI_OFFSET_X;
    x2 += GC9307_DBI_OFFSET_X;
#endif
#if GC9307_DBI_OFFSET_Y
    y1 += GC9307_DBI_OFFSET_Y;
    y2 += GC9307_DBI_OFFSET_Y;
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
 * @brief gc9307_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void gc9307_dbi_draw_point(uint16_t x, uint16_t y, gc9307_dbi_color_t color)
{
    /* set window */
    gc9307_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief gc9307_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void gc9307_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if GC9307_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif GC9307_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    gc9307_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
}

/**
 * @brief gc9307_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (gc9307_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void gc9307_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    gc9307_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
}

/**
 * @brief gc9307_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void gc9307_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    gc9307_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
}

#endif
