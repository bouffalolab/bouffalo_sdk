/**
 * @file st7735_spi.c
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

#if defined(LCD_SPI_ST7735)

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
#if (ST7735_SPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_RGB565,
#elif (ST7735_SPI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#else

#error "Configuration error"

#endif
const st7735_spi_init_cmd_t st7735_spi_init_cmds[] = {
    { 0x30, "\x00\x01\x00\xA0", 4 }, /* Partial Area */

    { 0x3A, "\x05", 1 }, /* RGB565 */

    { 0xB1, "\x05\x59\x59", 3 },             /* Frame Rate Control in normal mode/full colors */
    { 0xB2, "\x05\x59\x59", 3 },             /* Frame Rate Control in idle mode/8 colors */
    { 0xB3, "\x05\x59\x59\x05\x59\x59", 6 }, /* Frame Rate Control in partial mode/full colors */

#if (ST7735_SPI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (ST7735_SPI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif

/* Color reversal */
#if ST7735_SPI_COLOR_REVERSAL
    { 0xB4, "\x01", 1 },
    { 0x21, NULL, 0 },
#endif

    { 0xC0, "\x62\x02\x04", 3 }, /* Power Control 1 */
    { 0xC1, "\xC0", 1 },         /* Power Control 2 */
    { 0xC4, "\x8D\xEE", 2 },     /* Power Control 5 */
    { 0xC5, "\x0E", 1 },

    { 0xE0, "\x10\x0E\x02\x03\x0E\x07\x02\x07\x0A\x12\x27\x37\x00\x0D\x0E\x10", 16 }, /* Set Gamma '+'Polarity Correction Characteristics Setting */
    { 0XE1, "\x10\x0E\x03\x03\x0F\x06\x02\x08\x0A\x13\x26\x36\x00\x0D\x0E\x10", 16 }, /* Set Gamma '-'Polarity Correction Characteristics Setting */

    { 0x29, NULL, 0 }, /* Display on */
    { 0xFF, NULL, 10 },

    { 0x11, NULL, 0 }, /* Exit sleep */
};

/**
 * @brief st7735_spi_async_callback_enable
 *
 * @return
 */
void st7735_spi_async_callback_enable(bool enable)
{
    lcd_spi_sync_callback_enable(enable);
}

/**
 * @brief st7735_spi_async_callback_register
 *
 * @return
 */
void st7735_spi_async_callback_register(void (*callback)(void))
{
    lcd_spi_async_callback_register(callback);
}

/**
 * @brief st7735_spi_draw_is_busy, After the call st7735_spi_draw_picture_dma must check this,
 *         if st7735_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7735_spi_draw_is_busy(void)
{
    return lcd_spi_isbusy();
}

/**
 * @brief st7735_spi_init
 *
 * @return int
 */
int st7735_spi_init()
{
    lcd_spi_init(&spi_para);

    for (uint16_t i = 0; i < (sizeof(st7735_spi_init_cmds) / sizeof(st7735_spi_init_cmds[0])); i++) {
        if (st7735_spi_init_cmds[i].cmd == 0xFF && st7735_spi_init_cmds[i].data == NULL && st7735_spi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(st7735_spi_init_cmds[i].databytes);
        } else {
            lcd_spi_transmit_cmd_para(st7735_spi_init_cmds[i].cmd, (void *)(st7735_spi_init_cmds[i].data), st7735_spi_init_cmds[i].databytes);
        }
    }

    st7735_spi_set_dir(0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int st7735_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t dir_param[4] = { 0x00, 0x60, 0xA0, 0xC0 };
    uint8_t mir_param[4] = { 0x40, 0xE0, 0x20, 0x80 };
    uint8_t param;

    if (dir >= 4) {
        return -1;
    }

#if ST7735_SPI_DIR_MIRROR
    mir_flag = !mir_flag;
#endif

    if (mir_flag) {
        param = mir_param[dir];
    } else {
        param = dir_param[dir];
    }

/* Color RGB order */
#if ST7735_SPI_COLOR_ORDER
    param |= 0x08;
#endif

    lcd_spi_transmit_cmd_para(0x36, (void *)&param, 1);

    return dir;
}

/**
 * @brief st7735_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st7735_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ST7735_SPI_OFFSET_X
    x1 += ST7735_SPI_OFFSET_X;
    x2 += ST7735_SPI_OFFSET_X;
#endif
#if ST7735_SPI_OFFSET_Y
    y1 += ST7735_SPI_OFFSET_Y;
    y2 += ST7735_SPI_OFFSET_Y;
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
 * @brief st7735_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7735_spi_draw_point(uint16_t x, uint16_t y, st7735_spi_color_t color)
{
    /* set window */
    st7735_spi_set_draw_window(x, y, x, y);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief st7735_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7735_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7735_spi_color_t color)
{
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7735_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_fill_sync(0x2C, (uint32_t)color, pixel_cnt);
}

/**
 * @brief st7735_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7735_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7735_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7735_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7735_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_cnt);
}

/**
 * @brief st7735_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7735_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7735_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7735_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_cnt);
}

#endif
