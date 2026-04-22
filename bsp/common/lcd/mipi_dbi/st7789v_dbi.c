/**
 * @file st7789v_dbi.c
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

#if defined(LCD_DBI_ST7789V)

#if (LCD_DBI_INTERFACE_TYPE == 1)
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
    .clock_freq = 15 * 1000 * 1000,
#else
    /* typeC */
    .clock_freq = 40 * 1000 * 1000,
#endif

#if (ST7789V_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (ST7789V_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#else

#error "Configuration error"

#endif
const st7789v_dbi_init_cmd_t st7789v_dbi_init_cmds[] = {
    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* Delay 120ms */

    /* TEON and Not consist of HBlanking */
    { 0x35, "\x00", 1 },

    /* Set Tear Scanline, when display 0th line, the TE output */
    { 0x44, "\x00\x00", 2 },

    /* Display and color format setting */
    { 0x36, "\x00", 1 }, /* Memory Data Access Control */
    { 0x3A, "\x05", 1 }, /* Interface Pixel Format */
    { 0x21, NULL, 0 },   /* Display Inversion On */

    /* Frame rate setting */
    { 0xB2, "\x0C\x0C\x00\x33\x33", 5 }, /* Porch Setting */
    { 0xB7, "\x35", 1 },                 /* Gate Control */

    /* Power setting */
    { 0xBB, "\x19", 1 },     /* VCOM Setting */
    { 0xC0, "\x2C", 1 },     /* LCM Control */
    { 0xC2, "\x01", 1 },     /* VDV and VRH Command Enable */
    { 0xC3, "\x12", 1 },     /* VRH Set */
    { 0xC4, "\x20", 1 },     /* VDV Set */
    { 0xC6, "\x10", 1 },     /* Frame Rate Control in Normal Mode */
    { 0xD0, "\xA4\xA1", 2 }, /* Power Control 1 */

    /* Gamma setting */
    { 0xE0, "\xF0\x08\x0E\x09\x08\x16\x33\x43\x4A\x38\x14\x14\x2E\x32", 14 }, /* Positive Voltage Gamma Control */
    { 0xE1, "\xF0\x0A\x10\x0A\x09\x04\x33\x33\x49\x0A\x16\x15\x2C\x30", 14 }, /* Negative Voltage Gamma Control */

#if (ST7789V_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x05", 1 }, /* Interface Pixel Format RGB565 */
#elif (ST7789V_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif

    { 0x35, "\x00", 1 }, /* Tearing Effect Line On */

    { 0x29, NULL, 0 }, /* Display on */
};

/**
 * @brief st7789v_dbi_async_callback_enable
 *
 * @return
 */
void st7789v_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief st7789v_dbi_async_callback_register
 *
 * @return
 */
void st7789v_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief st7789v_dbi_draw_is_busy, After the call st7789v_dbi_draw_picture_dma must check this,
 *         if st7789v_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7789v_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief st7789v_dbi_init
 *
 * @return int
 */
int st7789v_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(st7789v_dbi_init_cmds) / sizeof(st7789v_dbi_init_cmds[0])); i++) {
        if (st7789v_dbi_init_cmds[i].cmd == 0xFF && st7789v_dbi_init_cmds[i].data == NULL && st7789v_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(st7789v_dbi_init_cmds[i].databytes);
        } else {
            lcd_dbi_transmit_cmd_para(st7789v_dbi_init_cmds[i].cmd, (void *)(st7789v_dbi_init_cmds[i].data), st7789v_dbi_init_cmds[i].databytes);
        }
    }

    st7789v_dbi_set_dir(0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int st7789v_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t dir_param[4] = { 0x00, 0x60, 0xC0, 0xA0 };
    uint8_t mir_param[4] = { 0x40, 0xE0, 0x80, 0x20 };
    uint8_t param;

    if (dir >= 4) {
        return -1;
    }

#if defined(ST7789V_DBI_DIR_MIRROR) && ST7789V_DBI_DIR_MIRROR
    mir_flag = !mir_flag;
#endif

    if (mir_flag) {
        param = mir_param[dir];
    } else {
        param = dir_param[dir];
    }

/* Color RGB order */
#if defined(ST7789V_DBI_COLOR_ORDER) && ST7789V_DBI_COLOR_ORDER
    param |= 0x08;
#endif

    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);

    return dir;
}

/**
 * @brief st7789v_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st7789v_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if defined(ST7789V_DBI_OFFSET_X) && ST7789V_DBI_OFFSET_X
    x1 += ST7789V_DBI_OFFSET_X;
    x2 += ST7789V_DBI_OFFSET_X;
#endif
#if defined(ST7789V_DBI_OFFSET_Y) && ST7789V_DBI_OFFSET_Y
    y1 += ST7789V_DBI_OFFSET_Y;
    y2 += ST7789V_DBI_OFFSET_Y;
#endif

    int8_t param[4];

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;

    lcd_dbi_transmit_cmd_para(0x2A, (void *)param, 4);

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;

    lcd_dbi_transmit_cmd_para(0x2B, (void *)param, 4);
}

/**
 * @brief st7789v_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7789v_dbi_draw_point(uint16_t x, uint16_t y, st7789v_dbi_color_t color)
{
    /* set window */
    st7789v_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief st7789v_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7789v_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7789v_dbi_color_t color)
{
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7789v_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, (uint32_t)color, pixel_cnt);
}

/**
 * @brief st7789v_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7789v_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7789v_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7789v_dbi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7789v_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_cnt);
}

/**
 * @brief st7789v_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7789v_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7789v_dbi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7789v_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_cnt);
}

#endif
