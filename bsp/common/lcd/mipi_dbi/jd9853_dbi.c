/**
 * @file jd9853_dbi.c
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
#include "jd9853_dbi.h"

#if defined(LCD_DBI_JD9853)

#if (LCD_DBI_INTERFACE_TYPE == 1)
/* dbi */
#include "bl_mipi_dbi.h"

#define lcd_dbi_init                          lcd_dbi_init
#define lcd_dbi_isbusy                        lcd_dbi_is_busy

#define lcd_dbi_transmit_cmd_para             lcd_dbi_transmit_cmd_para
#define lcd_dbi_transmit_cmd_pixel_sync       lcd_dbi_transmit_cmd_pixel_sync
#define lcd_dbi_transmit_cmd_pixel_fill_sync  lcd_dbi_transmit_cmd_pixel_fill_sync

#define lcd_dbi_async_callback_enable         lcd_dbi_async_callback_enable
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

#if (JD9853_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (JD9853_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)

#else

#error "Configuration error"

#endif

const jd9853_dbi_init_cmd_t jd9853_dbi_init_cmds[] = {
    { 0x01, NULL, 0 }, /* software reset */
    { 0xFF, NULL, 5 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xDF, "\x98\x53", 2 },
    { 0xDE, "\x00", 1 },

    { 0xB2, "\x6C", 1 },
    { 0xB7, "\x10\x21\x10\x49", 4 },
    { 0xBB, "\x34\x2B\x55\x73\x63\xF0", 6 },

    { 0xC0, "\x44\xA4", 2 },
    { 0xC1, "\x12", 1 },
    { 0xC3, "\x7D\x07\x14\x06\xC8\x6A\x6C\x77", 8 },
    { 0xC4, "\x00\x00\xA0\x6F\x1E\x1A\x16\x79\x1E\x1A\x16\x82", 12 },
    { 0xC8, "\x3F\x2D\x28\x25\x2B\x33\x32\x34\x36\x38\x39\x35\x33\x2D\x07\x00\x3B\x2D\x28\x25\x2B\x33\x32\x34\x36\x38\x39\x35\x33\x2D\x07\x00", 32 },

    { 0xD0, "\x04\x06\x62\x0F\x00", 5 },
    { 0xD7, "\x00\x30", 2 },

    { 0xE6, "\x14", 1 },

    { 0xDE, "\x01", 1 },

    { 0xB7, "\x03\x13\xEF\x35\x35", 5 },

    { 0xC1, "\x14\x15\xC0", 3 },
    { 0xC2, "\x06\x3A", 2 },
    { 0xC4, "\x72\x12", 2 },

    { 0xBE, "\x00", 1 },
    { 0xDE, "\x00", 1 },
    { 0xDE, "\x00", 1 },
    { 0xDE, "\x02", 1 },

    { 0xE5, "\x00\x02\x00", 3 },
    { 0xE5, "\x01\x02\x00", 3 },
    { 0xDE, "\x00", 1 },

    { 0xD8, "\x08\x00", 2 },

    { 0xDE, "\x02", 1 },
    { 0xE5, "\x00\x02\x00", 3 },
    { 0xDE, "\x00", 1 },

#if (JD9853_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (JD9853_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif

    { 0x35, "\x00", 1 }, /* enable Tearing Effect Output line */

#if JD9853_DBI_COLOR_REVERSAL
    { 0x21, NULL, 0 }, /* Color reversal */
#endif

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 5 },
};

/**
 * @brief jd9853_dbi_async_callback_enable
 *
 * @return
 */
void jd9853_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_async_callback_enable(enable);
}

/**
 * @brief jd9853_dbi_async_callback_register
 *
 * @return
 */
void jd9853_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief jd9853_dbi_draw_is_busy, After the call jd9853_dbi_draw_picture_dma must check this,
 *         if jd9853_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int jd9853_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief jd9853_dbi_init
 *
 * @return int
 */
int jd9853_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(jd9853_dbi_init_cmds) / sizeof(jd9853_dbi_init_cmd_t)); i++) {
        if (jd9853_dbi_init_cmds[i].cmd == 0xFF && jd9853_dbi_init_cmds[i].data == NULL && jd9853_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(jd9853_dbi_init_cmds[i].databytes);
        } else {
            lcd_dbi_transmit_cmd_para(jd9853_dbi_init_cmds[i].cmd, (void *)(jd9853_dbi_init_cmds[i].data), jd9853_dbi_init_cmds[i].databytes);
        }
    }

    jd9853_dbi_set_dir(0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int jd9853_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t dir_param[4] = { 0x00, 0x60, 0xA0, 0xC0 };
    uint8_t mir_param[4] = { 0x40, 0xE0, 0x20, 0x80 };
    uint8_t param;

    if (dir >= 4) {
        return -1;
    }

#if JD9853_DBI_DIR_MIRROR
    mir_flag = !mir_flag;
#endif

    if (mir_flag) {
        param = mir_param[dir];
    } else {
        param = dir_param[dir];
    }

/* Color RGB order */
#if JD9853_DBI_COLOR_ORDER
    param |= 0x08;
#endif

    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
    return dir;
}

/**
 * @brief jd9853_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void jd9853_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if JD9853_DBI_OFFSET_X
    x1 += JD9853_DBI_OFFSET_X;
    x2 += JD9853_DBI_OFFSET_X;
#endif
#if JD9853_DBI_OFFSET_Y
    y1 += JD9853_DBI_OFFSET_Y;
    y2 += JD9853_DBI_OFFSET_Y;
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
 * @brief jd9853_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void jd9853_dbi_draw_point(uint16_t x, uint16_t y, jd9853_dbi_color_t color)
{
    /* set window */
    jd9853_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief jd9853_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void jd9853_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, jd9853_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if JD9853_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif JD9853_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    jd9853_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
}

/**
 * @brief jd9853_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (jd9853_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void jd9853_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, jd9853_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    jd9853_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
}

/**
 * @brief jd9853_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void jd9853_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, jd9853_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    jd9853_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
}

#endif
