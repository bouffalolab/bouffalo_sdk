/**
 * @file ili9488_spi.c
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

#if defined(LCD_SPI_ILI9488)

#if (LCD_SPI_INTERFACE_TYPE == 1)
#include "bl_spi_hard_4.h"

#define lcd_spi_init                          lcd_spi_hard_4_init
#define lcd_spi_isbusy                        lcd_spi_hard_4_is_busy

#define lcd_spi_transmit_cmd_para             lcd_spi_hard_4_transmit_cmd_para
#define lcd_spi_transmit_cmd_pixel_sync       lcd_spi_hard_4_transmit_cmd_pixel_sync
#define lcd_spi_transmit_cmd_pixel_fill_sync  lcd_spi_hard_4_transmit_cmd_pixel_fill_sync

#define lcd_spi_sync_callback_enable          lcd_spi_hard_4_async_callback_enable
#define lcd_spi_async_callback_register       lcd_spi_hard_4_async_callback_register
#define lcd_spi_transmit_cmd_pixel_async      lcd_spi_hard_4_transmit_cmd_pixel_async
#define lcd_spi_transmit_cmd_pixel_fill_async lcd_spi_hard_4_transmit_cmd_pixel_fill_async

static lcd_spi_hard_4_init_t spi_para = {
    .clock_freq = 40 * 1000 * 1000,
#if (ILI9488_SPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_RGB565,
#elif (ILI9488_SPI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#else

#error "Configuration error"

#endif

const ili9488_spi_init_cmd_t ili9488_spi_init_cmds[] = {
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xE0, "\x00\x07\x0F\x0D\x1B\x0A\x3C\x78\x4A\x07\x0E\x09\x1B\x1E\x0F", 15 }, /* PGAMCTRL (Positive Gamma Control) */
    { 0xE1, "\x00\x22\x24\x26\x12\x07\x36\x47\x47\x06\x0A\x07\x30\x37\x0F", 15 }, /* NGAMCTRL (Negative Gamma Control) */

    { 0xC0, "\x10\x10", 2 }, /* Power Control 1 */
    { 0xC1, "\x41", 1 },     /* Power Control 2 */

    { 0xC5, "\x00\x2C\x80", 3 }, /* VCOM Control */
    { 0x36, "\x08", 1 },         /* Memory Access Control */

    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */

    { 0xB0, "\x00", 1 },     /* Interface Mode Control */
    { 0xB1, "\xB0", 1 },     /* Frame rate 70Hz */
    { 0xB4, "\x02", 1 },     /* Display Inversion Control */
    { 0xB6, "\x02\x22", 2 }, /* Display Function Control */
    { 0xE9, "\x00", 1 },     /* Set Image Function */

    { 0xF7, "\xA9\x51\x2C\x82", 4 }, /* Adjust Control 3 */

#if ILI9488_SPI_COLOR_REVERSAL
    { 0x21, NULL, 0 }, /* Color reversal */
#endif

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

/**
 * @brief ili9488_spi_async_callback_enable
 *
 * @return
 */
void ili9488_spi_async_callback_enable(bool enable)
{
    lcd_spi_sync_callback_enable(enable);
}

/**
 * @brief ili9488_spi_async_callback_register
 *
 * @return
 */
void ili9488_spi_async_callback_register(void (*callback)(void))
{
    lcd_spi_async_callback_register(callback);
}

/**
 * @brief ili9488_spi_draw_is_busy, After the call ili9488_spi_draw_picture_dma must check this,
 *         if ili9488_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int ili9488_spi_draw_is_busy(void)
{
    return lcd_spi_isbusy();
}

/**
 * @brief ili9488_spi_init
 *
 * @return int
 */
int ili9488_spi_init()
{
    lcd_spi_init(&spi_para);

    for (uint16_t i = 0; i < (sizeof(ili9488_spi_init_cmds) / sizeof(ili9488_spi_init_cmds[0])); i++) {
        if (ili9488_spi_init_cmds[i].cmd == 0xFF && ili9488_spi_init_cmds[i].data == NULL && ili9488_spi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(ili9488_spi_init_cmds[i].databytes);
        } else {
            lcd_spi_transmit_cmd_para(ili9488_spi_init_cmds[i].cmd, (void *)(ili9488_spi_init_cmds[i].data), ili9488_spi_init_cmds[i].databytes);
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
int ili9488_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;

    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x00;
            else
                param = 0x01;
            break;
        case 1:
            if (!mir_flag)
                param = 0x60;
            else
                param = 0x20;
            break;
        case 2:
            if (!mir_flag)
                param = 0xC0;
            else
                param = 0x80;
            break;
        case 3:
            if (!mir_flag)
                param = 0xA0;
            else
                param = 0xE0;

            break;
        default:
            return -1;
            break;
    }

    lcd_spi_transmit_cmd_para(0x36, (void *)&param, 1);

    return dir;
}

/**
 * @brief ili9488_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void ili9488_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ILI9488_SPI_OFFSET_X
    x1 += ILI9488_SPI_OFFSET_X;
    x2 += ILI9488_SPI_OFFSET_X;
#endif
#if ILI9488_SPI_OFFSET_Y
    y1 += ILI9488_SPI_OFFSET_Y;
    y2 += ILI9488_SPI_OFFSET_Y;
#endif

    int8_t param[4];

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;

    lcd_spi_transmit_cmd_para(0x2A, (void *)param, 4);

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;

    lcd_spi_transmit_cmd_para(0x2B, (void *)param, 4);
}

/**
 * @brief ili9488_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void ili9488_spi_draw_point(uint16_t x, uint16_t y, ili9488_spi_color_t color)
{
    /* set window */
    ili9488_spi_set_draw_window(x, y, x, y);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief ili9488_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9488_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t color)
{
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    ili9488_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_fill_sync(0x2C, (uint32_t)color, pixel_cnt);
}

/**
 * @brief ili9488_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (ili9488_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9488_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    ili9488_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_cnt);
}

/**
 * @brief ili9488_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9488_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9488_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    ili9488_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_cnt);
}

#endif