/**
 * @file nt35510_dbi.c
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
#include "nt35510_dbi.h"

#if defined(LCD_DBI_NT35510)

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
    .clock_freq = 80 * 1000 * 1000,
#if (NT35510_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (NT35510_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)

#else

#error "Configuration error"

#endif

#define NT35510_CMD_PARA_CONTINUOUS_EN 0

const nt35510_dbi_init_cmd_t nt35510_dbi_init_cmds[] = {
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xFF, "\xAA\x55\xA5\x80", 4 },
    { 0xF2, "\x00\x84\x08", 3 },

    { 0x6F, "\x0A", 1 },
    { 0xF4, "\x13", 1 },

    { 0xFF, "\xAA\x55\xA5\x00", 4 },
    { 0xFF, "\x55\xAA\x52\x08\x01", 5 }, /* enable page 1 */

    { 0xB0, "\x0C\x0C\x0C", 3 }, /* AVDD Set AVDD 5.2V */
    { 0xB6, "\x46\x46\x46", 3 }, /* AVDD ratio */
    { 0xB1, "\x0C\x0C\x0C", 3 }, /* AVEE  -5.2V */
    { 0xB7, "\x26\x26\x26", 3 }, /* AVEE ratio */
    { 0xB2, "\x00\x00\x00", 3 }, /* VCL -2.5V */
    { 0xB8, "\x34\x34\x34", 3 }, /* VCL -2.5V */

    { 0xBF, "\x01", 1 },
    { 0xB3, "\x08\x08\x08", 3 }, /* VGH 15V (Free pump) */
    { 0xB9, "\x26\x26\x26", 3 }, /* VGH ratio */
    { 0xB5, "\x08\x08\x08", 3 }, /* VGL_REG -10V  */
    { 0xBA, "\x36\x36\x36", 3 }, /* VGLX ratio */
    { 0xBC, "\x00\x80\x00", 3 }, /* VGMP/VGSP 4.5V/0V */
    { 0xBD, "\x00\x80\x00", 3 }, /* VGMP/VGSP 4.5V/0V */
    { 0xBE, "\x00\x55", 2 },     /* VCOM */

    { 0xD1, "\x00\x01\x00\x1C\x00\x4E\x00\x6A\x00\x85\x00\xAB\x00\xC4\x00\xFC\x01\x23\x01\x61\x01\x94\x01\xE4\x02\x27\x02\x29\x02\x65\x02\xA6\x02\xCA\x02\xFD\x03\x1D\x03\x4D\x03\x6A\x03\x95\x03\xAC\x03\xCB\x03\xEA\x03\xEF", 52 }, /* amma Setting */
    { 0xD2, "\x00\x01\x00\x1C\x00\x4E\x00\x6A\x00\x85\x00\xAB\x00\xC4\x00\xFC\x01\x23\x01\x61\x01\x94\x01\xE4\x02\x27\x02\x29\x02\x65\x02\xA6\x02\xCA\x02\xFD\x03\x1D\x03\x4D\x03\x6A\x03\x95\x03\xAC\x03\xCB\x03\xEA\x03\xEF", 52 }, /* amma Setting */
    { 0xD3, "\x00\x01\x00\x1C\x00\x4E\x00\x6A\x00\x85\x00\xAB\x00\xC4\x00\xFC\x01\x23\x01\x61\x01\x94\x01\xE4\x02\x27\x02\x29\x02\x65\x02\xA6\x02\xCA\x02\xFD\x03\x1D\x03\x4D\x03\x6A\x03\x95\x03\xAC\x03\xCB\x03\xEA\x03\xEF", 52 }, /* amma Setting */
    { 0xD4, "\x00\x01\x00\x1C\x00\x4E\x00\x6A\x00\x85\x00\xAB\x00\xC4\x00\xFC\x01\x23\x01\x61\x01\x94\x01\xE4\x02\x27\x02\x29\x02\x65\x02\xA6\x02\xCA\x02\xFD\x03\x1D\x03\x4D\x03\x6A\x03\x95\x03\xAC\x03\xCB\x03\xEA\x03\xEF", 52 }, /* amma Setting */
    { 0xD5, "\x00\x01\x00\x1C\x00\x4E\x00\x6A\x00\x85\x00\xAB\x00\xC4\x00\xFC\x01\x23\x01\x61\x01\x94\x01\xE4\x02\x27\x02\x29\x02\x65\x02\xA6\x02\xCA\x02\xFD\x03\x1D\x03\x4D\x03\x6A\x03\x95\x03\xAC\x03\xCB\x03\xEA\x03\xEF", 52 }, /* amma Setting */
    { 0xD6, "\x00\x01\x00\x1C\x00\x4E\x00\x6A\x00\x85\x00\xAB\x00\xC4\x00\xFC\x01\x23\x01\x61\x01\x94\x01\xE4\x02\x27\x02\x29\x02\x65\x02\xA6\x02\xCA\x02\xFD\x03\x1D\x03\x4D\x03\x6A\x03\x95\x03\xAC\x03\xCB\x03\xEA\x03\xEF", 52 }, /* amma Setting */

    { 0xF0, "\x55\xAA\x52\x08\x00", 5 }, /* enable page 0 */

    { 0xB1, "\xCC\x00", 2 }, /* Display control, MCU I/F & mipi cmd mode: CC, RGB I/F  & mipi video mode: FC */

    { 0xB4, "\x10", 1 },
    { 0xB6, "\x05", 1 },             /* Source hold time */
    { 0xB7, "\x70\x70", 2 },         /* Gate EQ control */
    { 0xB8, "\x01\x05\x05\x05", 4 }, /* Source EQ control (Mode 2) */

    { 0xBC, "\x02\x00\x00", 3 }, /* Inversion mode */

    { 0xCC, "\x03\x50\x50", 3 }, /* BOE's Setting (default) */

    { 0xBD, "\x01\x84\x07\x31\x00", 5 }, /* Porch Adjust */
    { 0xBE, "\x01\x84\x07\x31\x00", 5 },
    { 0xBF, "\x01\x84\x07\x31\x00", 5 },

    { 0x35, "\x00", 1 },
    { 0x36, "\x08", 1 }, /* Display direction control */

#if (NT35510_DBI_COLOR_REVERSAL == 0)
    { 0x20, NULL, 0 },
#else
    { 0x21, NULL, 0 },
#endif

#if (NT35510_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 },
#elif (NT35510_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x77", 1 },
#endif

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

/**
 * @brief nt35510_dbi_async_callback_enable
 *
 * @return
 */
void nt35510_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief nt35510_dbi_async_callback_register
 *
 * @return
 */
void nt35510_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief nt35510_dbi_draw_is_busy, After the call nt35510_dbi_draw_picture_dma must check this,
 *         if nt35510_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int nt35510_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief nt35510_dbi_init
 *
 * @return int
 */
int nt35510_dbi_init()
{
    uint8_t nt35510_para[128] = { 0 };

    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(nt35510_dbi_init_cmds) / sizeof(nt35510_dbi_init_cmd_t)); i++) {
        if (nt35510_dbi_init_cmds[i].cmd == 0xFF && nt35510_dbi_init_cmds[i].data == NULL && nt35510_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(nt35510_dbi_init_cmds[i].databytes);
        } else {
#if NT35510_CMD_PARA_CONTINUOUS_EN
            /* Change the data to 16bit */
            for (uint8_t j = 0; j < nt35510_dbi_init_cmds[i].databytes; j++) {
                nt35510_para[j * 2] = 0;
                nt35510_para[j * 2 + 1] = nt35510_dbi_init_cmds[i].data[j];
            }

            lcd_dbi_transmit_cmd_para(nt35510_dbi_init_cmds[i].cmd, NULL, 0);
            lcd_dbi_transmit_cmd_para(0, (void *)nt35510_para, nt35510_dbi_init_cmds[i].databytes * 2);
#else
            if (nt35510_dbi_init_cmds[i].databytes == 0) {
                lcd_dbi_transmit_cmd_para(nt35510_dbi_init_cmds[i].cmd, NULL, 0);
                lcd_dbi_transmit_cmd_para(0, NULL, 0);
            } else {
                for (uint8_t j = 0; j < nt35510_dbi_init_cmds[i].databytes; j++) {
                    nt35510_para[0] = 0;
                    nt35510_para[1] = nt35510_dbi_init_cmds[i].data[j];
                    lcd_dbi_transmit_cmd_para(nt35510_dbi_init_cmds[i].cmd, NULL, 0);
                    lcd_dbi_transmit_cmd_para(j, (void *)nt35510_para, 2);
                }
            }

#endif
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
int nt35510_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param[2] = { 0 };

    switch (dir) {
        case 0:
            if (!mir_flag)
                param[1] = 0x08;
            else
                param[1] = 0x09;
            break;
        case 1:
            if (!mir_flag)
                param[1] = 0x68;
            else
                param[1] = 0x28;
            break;
        case 2:
            if (!mir_flag)
                param[1] = 0xC8;
            else
                param[1] = 0x88;
            break;
        case 3:
            if (!mir_flag)
                param[1] = 0xA8;
            else
                param[1] = 0xE8;

            break;
        default:
            return -1;
            break;
    }
    lcd_dbi_transmit_cmd_para(0x36, NULL, 0);
    lcd_dbi_transmit_cmd_para(0, (void *)&param, 2);
    return dir;
}

/**
 * @brief nt35510_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void nt35510_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if NT35510_DBI_OFFSET_X
    x1 += NT35510_DBI_OFFSET_X;
    x2 += NT35510_DBI_OFFSET_X;
#endif
#if NT35510_DBI_OFFSET_Y
    y1 += NT35510_DBI_OFFSET_Y;
    y2 += NT35510_DBI_OFFSET_Y;
#endif

    uint8_t param[4];
    uint8_t tx_buff[4] = { 0 };

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
    // lcd_dbi_transmit_cmd_para(0x2A, NULL, 0);
    // lcd_dbi_transmit_cmd_para(0, (void *)param, 8);

    for (uint8_t i = 0; i < 4; i++) {
        tx_buff[0] = 0;
        tx_buff[1] = param[i];

        lcd_dbi_transmit_cmd_para(0x2A, NULL, 0);
        lcd_dbi_transmit_cmd_para(i, (void *)tx_buff, 2);
    }

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
    // lcd_dbi_transmit_cmd_para(0x2B, NULL, 0);
    // lcd_dbi_transmit_cmd_para(0, (void *)param, 8);

    for (uint8_t i = 0; i < 4; i++) {
        tx_buff[0] = 0;
        tx_buff[1] = param[i];

        lcd_dbi_transmit_cmd_para(0x2B, NULL, 0);
        lcd_dbi_transmit_cmd_para(i, (void *)tx_buff, 2);
    }
}

/**
 * @brief nt35510_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void nt35510_dbi_draw_point(uint16_t x, uint16_t y, nt35510_dbi_color_t color)
{
    /* set window */
    nt35510_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_para(0x2C, NULL, 0);
    lcd_dbi_transmit_cmd_pixel_sync(0, (void *)&color, 1);
}

/**
 * @brief nt35510_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void nt35510_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nt35510_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if NT35510_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif NT35510_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    nt35510_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_para(0x2C, NULL, 0);
    lcd_dbi_transmit_cmd_pixel_fill_sync(0, color_src, pixel_num);
}

/**
 * @brief nt35510_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (nt35510_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void nt35510_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nt35510_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    nt35510_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_para(0x2C, NULL, 0);
    lcd_dbi_transmit_cmd_pixel_async(0, (void *)picture, pixel_num);
}

/**
 * @brief nt35510_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void nt35510_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, nt35510_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    nt35510_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_para(0x2C, NULL, 0);
    lcd_dbi_transmit_cmd_pixel_sync(0, (void *)picture, pixel_num);
}

#endif
