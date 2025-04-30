/**
 * @file st77926_dbi.c
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
#include "st77926_dbi_qspi.h"

#if defined(LCD_DBI_ST77926)

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
    /* typeC/qspi */
    .clock_freq = 40 * 1000 * 1000,
#endif

#if (ST77926_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (ST77926_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif

#if (LCD_DBI_WORK_MODE == 4)
    .cmd_wire_dual_en = 0,
    .addr_wire_dual_en = 1,
    .data_wire_dual_en = 1,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)

#else

#error "Configuration error"

#endif

const st77926_dbi_init_cmd_t st77926_dbi_init_cmds[] = {
    // { 0x01, NULL, 0 }, /* Software Reset */
    // { 0xFF, NULL, 5 }, /* delay 5ms */

    { 0xF1, "\x00", 1 },
    { 0x60, "\x00\x00\x00", 3 },
    { 0x65, "\x80", 1 },
    { 0x79, "\x06", 1 },
    { 0x7B, "\x00\x08\x08", 3 },
    { 0x80, "\x55\x62\x2F\x17\xF0\x52\x70\xD2\x52\x62\xEA", 11 },
    { 0x81, "\x26\x52\x72\x27", 4 },
    { 0x84, "\x92\x25", 2 },
    { 0x87, "\x10\x10\x58\x00\x02\x3A", 6 },
    { 0x88, "\x00\x00\x2C\x10\x04\x00\x00\x00\x01\x01\x01\x01\x01\x00\x06", 15 },
    { 0x89, "\x00\x00\x00", 3 },
    { 0x8A, "\x13\x00\x2C\x00\x00\x2C\x10\x10\x00\x3E\x19", 11 },
    { 0x8B, "\x15\xB1\xB1\x44\x96\x2C\x10\x97\x8E", 9 },
    { 0x8C, "\x1D\xB1\xB1\x44\x96\x2C\x10\x50\x0F\x01\xC5\x12\x09", 13 },
    { 0x8D, "\x0C", 1 },
    { 0x8E, "\x33\x01\x0C\x13\x01\x01", 6 },
    { 0xB3, "\x00\x30", 2 },
    { 0xF1, "\x00", 1 },
    { 0x71, "\xD0", 1 },
    { 0x66, "\x02\x3F", 2 },
    { 0xBE, "\x20\x00\x9D", 3 },
    { 0x70, "\x01\xA4\x11\x40\xE0\x00\x0F\x65\x00\x00\x00\x1A", 12 },
    { 0x90, "\x00\x44\x55\x31\xC4\x71\x44\x61\x61", 9 },
    { 0x91, "\x00\x44\x55\x36\x00\x76\x40\x61\x61", 9 },
    { 0x92, "\x00\x44\x55\x37\x00\x37\x00\x05\x61\x61", 10 },
    { 0x93, "\x00\x43\x11\x00\x00\x00\x00\x05\x61\x61", 10 },
    { 0x94, "\x00\x00\x00\x00\x00\x00", 6 },
    { 0x95, "\x99\x19\x00\x00\xFF", 5 },
    { 0x96, "\x44\x44\x07\x16\x00\x20\x04\x03\xC8\x61\x00\x40", 12 },
    { 0x97, "\x44\x44\x25\x34\x00\x20\x02\x01\xC8\x61\x00\x40", 12 },
    { 0xBA, "\x44\xC8\x61\xC8\x61", 5 },
    { 0x9A, "\x40\x00\x06\x00\x00\x00\x00", 7 },
    { 0x9B, "\x00\x00\x06\x00\x00\x00\x00", 7 },
    { 0x9C, "\x40\x12\x00\x00\x10\x12\x00\x00\x00\x12\x00\x00\x00", 13 },
    { 0x9D, "\x8C\x51\x00\x00\x00\x80\x1E\x01", 8 },
    { 0x9E, "\x51\x00\x00\x00\x80\x1E\x01", 7 },
    { 0xB4, "\x14\x1E\x10\x1C\x19\x1D\x1E\x18\x03\x0A\x01\x08", 12 },
    { 0xB5, "\x12\x1E\x10\x1C\x19\x1D\x1E\x18\x02\x0B\x00\x09", 12 },
    { 0xB6, "\x99\x99\x00\x0F\xBF\x0F\xBF", 7 },
    { 0x86, "\xC9\x04\xB1\x02\x58\x12\x58\x0C\x13\x01\xA5\x00\xA5\xA5", 14 },
    { 0xB7, "\x00\x0C\x0C\x0E\x0B\x07\x34\x03\x04\x49\x08\x15\x15\x2C\x32\x0F", 16 },
    { 0xB8, "\x00\x0C\x0C\x0E\x0B\x06\x34\x03\x04\x49\x08\x14\x14\x2C\x32\x0F", 16 },
    { 0xB9, "\x23\x23", 2 },
    { 0xBF, "\x0D\x11\x11\x0B\x0B\x0B", 6 },
    { 0xF2, "\x00", 1 },
    { 0x73, "\x04\xDA\x12\x52\x51", 5 },
    { 0x77, "\x6B\x5B\xFD\xC3\xC5", 5 },
    { 0x7A, "\x15\x27", 2 },
    { 0x7B, "\x04\x57", 2 },
    { 0x7E, "\x01\x0E", 2 },
    { 0xBF, "\x36", 1 },
    { 0xE3, "\x40\x40", 2 },
    { 0xF0, "\x00", 1 },
    { 0xD0, "\x00", 1 },
    { 0x2A, "\x00\x00\x01\x3F", 4 },
    { 0x2B, "\x00\x00\x01\xDF", 4 },

#if (ST77926_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x01", 1 }, /* RGB565 */
#elif (ST77926_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x03", 1 }, /* RGB888 */
#endif

/* Color reversal */
#if ST77926_DBI_COLOR_REVERSAL
    { 0x21, NULL, 0 },
#else
    { 0x20, NULL, 0 },
#endif

/* Color RGB order */
#if ST77926_DBI_COLOR_ORDER
    { 0x36, "\x08", 1 },
#else
    { 0x36, "\x00", 1 },
#endif

    { 0x11, NULL, 0 },   /* sleep out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0x29, NULL, 0 }, /* display on */

    { 0x35, "\x00", 1 }, /* enable TE out */
};

/**
 * @brief st77926_dbi_async_callback_enable
 *
 * @return
 */
void st77926_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief st77926_dbi_async_callback_register
 *
 * @return
 */
void st77926_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief st77926_dbi_draw_is_busy, After the call st77926_dbi_draw_picture_dma must check this,
 *         if st77926_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st77926_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief st77926_dbi_init
 *
 * @return int
 */
int st77926_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(st77926_dbi_init_cmds) / sizeof(st77926_dbi_init_cmd_t)); i++) {
        if (st77926_dbi_init_cmds[i].cmd == 0xFF && st77926_dbi_init_cmds[i].data == NULL && st77926_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(st77926_dbi_init_cmds[i].databytes);
        } else {
#if (LCD_DBI_WORK_MODE == 4)
            lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(st77926_dbi_init_cmds[i].cmd));
            lcd_dbi_transmit_cmd_para(ST77926_QSPI_CMD, (void *)(st77926_dbi_init_cmds[i].data), st77926_dbi_init_cmds[i].databytes);
#else
            lcd_dbi_transmit_cmd_para(st77926_dbi_init_cmds[i].cmd, (void *)(st77926_dbi_init_cmds[i].data), st77926_dbi_init_cmds[i].databytes);
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
int st77926_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
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
#if ST77926_DBI_COLOR_ORDER
    param |= 0x08;
#endif

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x36));
    lcd_dbi_transmit_cmd_para(ST77926_QSPI_CMD, (void *)&param, 1);
#else
    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
#endif
    return dir;
}

/**
 * @brief st77926_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st77926_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ST77926_DBI_OFFSET_X
    x1 += ST77926_DBI_OFFSET_X;
    x2 += ST77926_DBI_OFFSET_X;
#endif
#if ST77926_DBI_OFFSET_Y
    y1 += ST77926_DBI_OFFSET_Y;
    y2 += ST77926_DBI_OFFSET_Y;
#endif

    uint8_t param[4];

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x2B));
    lcd_dbi_transmit_cmd_para(ST77926_QSPI_CMD, (void *)param, 4);
#else
    lcd_dbi_transmit_cmd_para(0x2B, (void *)param, 4);
#endif

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x2A));
    lcd_dbi_transmit_cmd_para(ST77926_QSPI_CMD, (void *)param, 4);
#else
    lcd_dbi_transmit_cmd_para(0x2A, (void *)param, 4);
#endif
}

/**
 * @brief st77926_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st77926_dbi_draw_point(uint16_t x, uint16_t y, st77926_dbi_color_t color)
{
    /* set window */
    st77926_dbi_set_draw_window(x, y, x, y);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_sync(ST77926_QSPI_CMD, (void *)&color, 1);
#else
    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
#endif
}

/**
 * @brief st77926_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st77926_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st77926_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if ST77926_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif ST77926_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    st77926_dbi_set_draw_window(x1, y1, x2, y2);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_fill_sync(ST77926_QSPI_CMD, color_src, pixel_num);
#else
    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
#endif
}

/**
 * @brief st77926_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st77926_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void st77926_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st77926_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st77926_dbi_set_draw_window(x1, y1, x2, y2);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_async(ST77926_QSPI_CMD, (void *)picture, pixel_num);
#else
    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
#endif
}

/**
 * @brief st77926_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st77926_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st77926_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st77926_dbi_set_draw_window(x1, y1, x2, y2);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(ST77926_QSPI_ADDR_BYTE, ST77926_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_sync(ST77926_QSPI_CMD, (void *)picture, pixel_num);
#else
    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
#endif
}

#endif
