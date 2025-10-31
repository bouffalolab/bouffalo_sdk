/**
 * @file spd2010_dbi.c
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
#include "spd2010_dbi_qspi.h"

#if defined(LCD_DBI_SPD2010)

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

#if (SPD2010_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (SPD2010_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif

#if (LCD_DBI_WORK_MODE == 4)
    .cmd_wire_dual_en = 0,
    .addr_wire_dual_en = 0,
    .data_wire_dual_en = 1,
#endif
};

#elif (LCD_DBI_INTERFACE_TYPE == 2)

#else

#error "Configuration error"

#endif

const spd2010_dbi_init_cmd_t spd2010_dbi_init_cmds[] = {
    /* Reset sequence (handled by hardware RST pin in original code) */
    { 0xFF, "\x20\x10\x10", 3 }, /* Command prefix */
    { 0x01, NULL, 0 },           /* Software Reset */ 
    { 0xFF, NULL, 120 },         /* Delay 120ms after reset */

    /* Initial configuration block 1 */
    { 0xFF, "\x20\x10\x10", 3 },
    { 0x62, "\x00", 1 },
    { 0x61, "\x00", 1 },
    { 0x5C, "\x00", 1 },
    { 0x58, "\x00", 1 },
    { 0x55, "\x55", 1 },
    { 0x54, "\x44", 1 },
    { 0x51, "\x11", 1 },
    { 0x4B, "\x00", 1 },
    { 0x4A, "\x00", 1 },
    { 0x49, "\x00", 1 },
    { 0x47, "\x77", 1 },
    { 0x46, "\x66", 1 },
    { 0x45, "\x55", 1 },
    { 0x44, "\x44", 1 },
    { 0x43, "\x33", 1 },
    { 0x42, "\x22", 1 },
    { 0x41, "\x11", 1 },
    { 0x37, "\x12", 1 },
    { 0x36, "\x12", 1 },
    { 0x35, "\x11", 1 },
    { 0x34, "\x11", 1 },
    { 0x33, "\x20", 1 },
    { 0x32, "\x00", 1 },
    { 0x31, "\x00", 1 },
    { 0x30, "\x00", 1 },
    { 0x27, "\x12", 1 },
    { 0x26, "\x12", 1 },
    { 0x25, "\x11", 1 },
    { 0x24, "\x11", 1 },
    { 0x23, "\x20", 1 },
    { 0x22, "\x72", 1 },
    { 0x21, "\x82", 1 },
    { 0x20, "\x81", 1 },
    { 0x1B, "\x00", 1 },
    { 0x1A, "\x00", 1 },
    { 0x16, "\x00", 1 },
    { 0x15, "\x00", 1 },
    { 0x11, "\x12", 1 },
    { 0x10, "\x01", 1 },
    { 0x0C, "\x12", 1 },
    { 0x0B, "\x43", 1 },

    /*Configuration block 2*/
    { 0xFF, "\x20\x10\x11", 3 },
    { 0x6A, "\x10", 1 },
    { 0x69, "\x03", 1 },
    { 0x68, "\x34", 1 },
    { 0x67, "\x04", 1 },
    { 0x66, "\x38", 1 },
    { 0x65, "\x03", 1 },
    { 0x64, "\x34", 1 },
    { 0x63, "\x04", 1 },
    { 0x62, "\x38", 1 },
    { 0x61, "\x03", 1 },
    { 0x60, "\x01", 1 },
    { 0x55, "\x06", 1 },
    { 0x50, "\x01", 1 },
    { 0x30, "\xEE", 1 },
    { 0x1E, "\x88", 1 },
    { 0x1D, "\x88", 1 },
    { 0x1C, "\x88", 1 },
    { 0x16, "\x99", 1 },
    { 0x15, "\x99", 1 },
    { 0x14, "\x34", 1 },
    { 0x13, "\xF0", 1 },
    { 0x0C, "\xF0", 1 },
    { 0x0B, "\xF0", 1 },
    { 0x0A, "\x00", 1 },
    { 0x09, "\x00", 1 },
    { 0x08, "\x70", 1 },

    /* Configuration block 3 */
    { 0xFF, "\x20\x10\x12", 3 },
    { 0x36, "\xA0", 1 },
    { 0x2E, "\x1E", 1 },
    { 0x2D, "\x2D", 1 },
    { 0x2C, "\x26", 1 },
    { 0x2B, "\x1E", 1 },
    { 0x2A, "\x2D", 1 },
    { 0x21, "\x70", 1 }, 
    { 0x1F, "\xE6", 1 },
    { 0x18, "\x00", 1 },
    { 0x15, "\x0F", 1 },
    { 0x12, "\x89", 1 },
    { 0x10, "\x0F", 1 },
    { 0x0D, "\x66", 1 },
    { 0x06, "\x06", 1 },
    { 0x00, "\xCC", 1 },

    /* Configuration block 4 */
    { 0xFF, "\x20\x10\x15", 3 },
    { 0x2F, "\x01", 1 },
    { 0x2E, "\x01", 1 },
    { 0x2D, "\x35", 1 },
    { 0x2C, "\x34", 1 },
    { 0x2B, "\x32", 1 },
    { 0x2A, "\x33", 1 },
    { 0x29, "\x1D", 1 },
    { 0x28, "\x1B", 1 },
    { 0x27, "\x19", 1 },
    { 0x26, "\x17", 1 },
    { 0x25, "\x09", 1 },
    { 0x24, "\x05", 1 },
    { 0x23, "\x00", 1 },
    { 0x22, "\x00", 1 },
    { 0x21, "\x00", 1 },
    { 0x20, "\x00", 1 },
    { 0x0F, "\x01", 1 },
    { 0x0E, "\x01", 1 },
    { 0x0D, "\x35", 1 },
    { 0x0C, "\x34", 1 },
    { 0x0B, "\x32", 1 },
    { 0x0A, "\x33", 1 },
    { 0x09, "\x16", 1 },
    { 0x08, "\x18", 1 },
    { 0x07, "\x1A", 1 },
    { 0x06, "\x1C", 1 },
    { 0x05, "\x04", 1 },
    { 0x04, "\x08", 1 },
    { 0x03, "\x00", 1 },
    { 0x02, "\x00", 1 },
    { 0x01, "\x00", 1 },
    { 0x00, "\x00", 1 },

    /* Configuration block 5 */
    { 0xFF, "\x20\x10\x16", 3 },
    { 0x2F, "\x01", 1 },
    { 0x2E, "\x01", 1 },
    { 0x2D, "\x35", 1 },
    { 0x2C, "\x34", 1 },
    { 0x2B, "\x32", 1 },
    { 0x2A, "\x33", 1 },
    { 0x29, "\x1C", 1 },
    { 0x28, "\x1A", 1 },
    { 0x27, "\x18", 1 },
    { 0x26, "\x16", 1 },
    { 0x25, "\x08", 1 },
    { 0x24, "\x04", 1 },
    { 0x23, "\x00", 1 },
    { 0x22, "\x00", 1 },
    { 0x21, "\x00", 1 },
    { 0x20, "\x00", 1 },
    { 0x0F, "\x01", 1 },
    { 0x0E, "\x01", 1 },
    { 0x0D, "\x35", 1 },
    { 0x0C, "\x34", 1 },
    { 0x0B, "\x32", 1 },
    { 0x0A, "\x33", 1 },
    { 0x09, "\x17", 1 },
    { 0x08, "\x19", 1 },
    { 0x07, "\x1B", 1 },
    { 0x06, "\x1D", 1 },
    { 0x05, "\x05", 1 },
    { 0x04, "\x09", 1 },
    { 0x03, "\x00", 1 },
    { 0x02, "\x00", 1 },
    { 0x01, "\x00", 1 },
    { 0x00, "\x00", 1 },

    /* Configuration block 6 */
    { 0xFF, "\x20\x10\x17", 3 },
    { 0x39, "\x3C", 1 },
    { 0x37, "\x00", 1 },
    { 0x1F, "\x80", 1 },
    { 0x1A, "\x80", 1 },
    { 0x18, "\xA0", 1 },
    { 0x16, "\x12", 1 },
    { 0x14, "\xAA", 1 },
    { 0x11, "\xAA", 1 },
    { 0x10, "\x0E", 1 },
    { 0x0B, "\xC3", 1 },

    /* Configuration block 7 */
    { 0xFF, "\x20\x10\x18", 3 },
    { 0x3A, "\x01", 1 },
    { 0x1F, "\x02", 1 },
    { 0x01, "\x01", 1 },
    { 0x00, "\x1E", 1 },

    /* Configuration block 8 */
    { 0xFF, "\x20\x10\x2D", 3 },
    { 0x02, "\x00", 1 },
    { 0x01, "\x3E", 1 },

    /* Configuration block 9 */
    { 0xFF, "\x20\x10\x31", 3 },
    { 0x39, "\xF0", 1 },
    { 0x38, "\x03", 1 },
    { 0x37, "\xE8", 1 },
    { 0x36, "\x03", 1 },
    { 0x35, "\xCF", 1 },
    { 0x34, "\x03", 1 },
    { 0x33, "\xBA", 1 },
    { 0x32, "\x03", 1 },
    { 0x31, "\xA2", 1 },
    { 0x30, "\x03", 1 },
    { 0x2F, "\x95", 1 },
    { 0x2E, "\x03", 1 },
    { 0x2D, "\x7E", 1 },
    { 0x2C, "\x03", 1 },
    { 0x2B, "\x62", 1 },
    { 0x2A, "\x03", 1 },
    { 0x29, "\x44", 1 },
    { 0x28, "\x03", 1 },
    { 0x27, "\xFC", 1 },
    { 0x26, "\x02", 1 },
    { 0x25, "\xD0", 1 },
    { 0x24, "\x02", 1 },
    { 0x23, "\x98", 1 },
    { 0x22, "\x02", 1 },
    { 0x21, "\x6F", 1 },
    { 0x20, "\x02", 1 },
    { 0x1F, "\x32", 1 },
    { 0x1E, "\x02", 1 },
    { 0x1D, "\xF6", 1 },
    { 0x1C, "\x01", 1 },
    { 0x1B, "\xB8", 1 },
    { 0x1A, "\x01", 1 },
    { 0x19, "\x6E", 1 },
    { 0x18, "\x01", 1 },
    { 0x17, "\x41", 1 },
    { 0x16, "\x01", 1 },
    { 0x15, "\xFD", 1 },
    { 0x14, "\x00", 1 },
    { 0x13, "\xCF", 1 },
    { 0x12, "\x00", 1 },
    { 0x11, "\x98", 1 },
    { 0x10, "\x00", 1 },
    { 0x0F, "\x89", 1 },
    { 0x0E, "\x00", 1 },
    { 0x0D, "\x79", 1 },
    { 0x0C, "\x00", 1 },
    { 0x0B, "\x67", 1 },
    { 0x0A, "\x00", 1 },
    { 0x09, "\x55", 1 },
    { 0x08, "\x00", 1 },
    { 0x07, "\x3F", 1 },
    { 0x06, "\x00", 1 },
    { 0x05, "\x28", 1 },
    { 0x04, "\x00", 1 },
    { 0x03, "\x0E", 1 },
    { 0x02, "\x00", 1 },

    /* Configuration block 10 (duplicate of block 9 with same values) */
    { 0xFF, "\x20\x10\x32", 3 },
    { 0x39, "\xF0", 1 },
    { 0x38, "\x03", 1 },
    { 0x37, "\xE8", 1 },
    { 0x36, "\x03", 1 },
    { 0x35, "\xCF", 1 },
    { 0x34, "\x03", 1 },
    { 0x33, "\xBA", 1 },
    { 0x32, "\x03", 1 },
    { 0x31, "\xA2", 1 },
    { 0x30, "\x03", 1 },
    { 0x2F, "\x95", 1 },
    { 0x2E, "\x03", 1 },
    { 0x2D, "\x7E", 1 },
    { 0x2C, "\x03", 1 },
    { 0x2B, "\x62", 1 },
    { 0x2A, "\x03", 1 },
    { 0x29, "\x44", 1 },
    { 0x28, "\x03", 1 },
    { 0x27, "\xFC", 1 },
    { 0x26, "\x02", 1 },
    { 0x25, "\xD0", 1 },
    { 0x24, "\x02", 1 },
    { 0x23, "\x98", 1 },
    { 0x22, "\x02", 1 },
    { 0x21, "\x6F", 1 },
    { 0x20, "\x02", 1 },
    { 0x1F, "\x32", 1 },
    { 0x1E, "\x02", 1 },
    { 0x1D, "\xF6", 1 },
    { 0x1C, "\x01", 1 },
    { 0x1B, "\xB8", 1 },
    { 0x1A, "\x01", 1 },
    { 0x19, "\x6E", 1 },
    { 0x18, "\x01", 1 },
    { 0x17, "\x41", 1 },
    { 0x16, "\x01", 1 },
    { 0x15, "\xFD", 1 },
    { 0x14, "\x00", 1 },
    { 0x13, "\xCF", 1 },
    { 0x12, "\x00", 1 },
    { 0x11, "\x98", 1 },
    { 0x10, "\x00", 1 },
    { 0x0F, "\x89", 1 },
    { 0x0E, "\x00", 1 },
    { 0x0D, "\x79", 1 },
    { 0x0C, "\x00", 1 },
    { 0x0B, "\x67", 1 },
    { 0x0A, "\x00", 1 },
    { 0x09, "\x55", 1 },
    { 0x08, "\x00", 1 },
    { 0x07, "\x3F", 1 },
    { 0x06, "\x00", 1 },
    { 0x05, "\x28", 1 },
    { 0x04, "\x00", 1 },
    { 0x03, "\x0E", 1 },
    { 0x02, "\x00", 1 },

    /* Configuration block 11 */
    { 0xFF, "\x20\x10\x40", 3 },
    { 0x86, "\x00", 1 },
    { 0x83, "\xC4", 1 },

    /* Configuration block 12 */
    { 0xFF, "\x20\x10\x42", 3 },
    { 0x06, "\x03", 1 },
    { 0x05, "\x3D", 1 },

    /* Configuration block 13 */
    { 0xFF, "\x20\x10\x43", 3 },
    { 0x03, "\x04", 1 },

    /* Configuration block 14 */
    { 0xFF, "\x20\x10\x45", 3 },
    { 0x03, "\x9C", 1 },
    { 0x01, "\x9C", 1 },

    /* Touch panel configuration (commented out as per original)
     { 0xFF, "\x20\x10\x50", 3 },
     { 0x08, "\x55", 1 },
     { 0x05, "\x08", 1 },
     { 0x01, "\xA6", 1 },
     { 0x00, "\xA6", 1 }, */

    /* Configuration block 15 */
    { 0xFF, "\x20\x10\xA0", 3 },
    { 0x08, "\xE6", 1 },

    /* Final display configuration */
    { 0xFF, "\x20\x10\x00", 3 },
    { 0x2A, "\x00\x00\x01\x9B", 4 }, /* Column address set */
    { 0x2B, "\x00\x00\x01\x9B", 4 }, /* Page address set */
    { 0x35, "\x00", 1 },             /* TE on */

    /* Pixel format configuration */
#if (SPD2010_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x05", 1 }, /* RGB16-bit (RGB565) */
#elif (SPD2010_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x07", 1 }, /* RGB24-bit (RGB888) */
#else
    { 0x3A, "\x06", 1 }, /* RGB18-bit (default) */
#endif

    /*Color reversal*/
#if SPD2010_DBI_COLOR_REVERSAL
    { 0x21, NULL, 0 }, /*Display inversion on*/
#else
    { 0x20, NULL, 0 }, /* Display inversion off */
#endif

    { 0x11, NULL, 0 },   /* Sleep out */
    { 0xFF, NULL, 100 }, /* Delay 100ms */

    { 0x29, NULL, 0 },   /* Display on */
    { 0xFF, NULL, 100 }, /* Delay 100ms */
};

/**
 * @brief spd2010_dbi_async_callback_enable
 *
 * @return
 */
void spd2010_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_sync_callback_enable(enable);
}

/**
 * @brief spd2010_dbi_async_callback_register
 *
 * @return
 */
void spd2010_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief spd2010_dbi_draw_is_busy, After the call spd2010_dbi_draw_picture_dma must check this,
 *         if spd2010_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int spd2010_dbi_draw_is_busy(void)
{
    return lcd_dbi_isbusy();
}

/**
 * @brief spd2010_dbi_init
 *
 * @return int
 */
int spd2010_dbi_init()
{
    printf("spd2010_dbi_ready\n");
    lcd_dbi_init(&dbi_para);
    printf("spd2010_dbi_init\n");
    for (uint16_t i = 0; i < (sizeof(spd2010_dbi_init_cmds) / sizeof(spd2010_dbi_init_cmd_t)); i++) {
        if (spd2010_dbi_init_cmds[i].cmd == 0xFF && spd2010_dbi_init_cmds[i].data == NULL && spd2010_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(spd2010_dbi_init_cmds[i].databytes);
        } else {
#if (LCD_DBI_WORK_MODE == 4)
            lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(spd2010_dbi_init_cmds[i].cmd));
            lcd_dbi_transmit_cmd_para(SPD2010_QSPI_CMD, (void *)(spd2010_dbi_init_cmds[i].data), spd2010_dbi_init_cmds[i].databytes);
#else
            lcd_dbi_transmit_cmd_para(spd2010_dbi_init_cmds[i].cmd, (void *)(spd2010_dbi_init_cmds[i].data), spd2010_dbi_init_cmds[i].databytes);
#endif
        }
    }
    printf("spd2010_dbi_init!!!!!!!!!!!!!\n");
    spd2010_dbi_set_dir(0, 0);
    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int spd2010_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    printf("dir:%d", dir);
    printf("flag:%d", mir_flag);
    uint8_t dir_param[4] = { 0x00, 0x60, 0xA0, 0xC0 };
    uint8_t mir_param[4] = { 0x40, 0xE0, 0x20, 0x80 };
    uint8_t param;

    if (dir >= 4) {
        return -1;
    }

#if SPD2010_DBI_DIR_MIRROR
    mir_flag = !mir_flag;
#endif

    if (mir_flag) {
        param = mir_param[dir];
    } else {
        param = dir_param[dir];
    }

/* Color RGB order */
#if SPD2010_DBI_COLOR_ORDER
    param |= 0x08;
#endif

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x36));
    lcd_dbi_transmit_cmd_para(SPD2010_QSPI_CMD, (void *)&param, 1);
#else
    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
#endif
    return dir;
}

/**
 * @brief spd2010_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void spd2010_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if SPD2010_DBI_OFFSET_X
    x1 += SPD2010_DBI_OFFSET_X;
    x2 += SPD2010_DBI_OFFSET_X;
#endif
#if SPD2010_DBI_OFFSET_Y
    y1 += SPD2010_DBI_OFFSET_Y;
    y2 += SPD2010_DBI_OFFSET_Y;
#endif

    uint8_t param[4];

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x2B));
    lcd_dbi_transmit_cmd_para(SPD2010_QSPI_CMD, (void *)param, 4);
#else
    lcd_dbi_transmit_cmd_para(0x2B, (void *)param, 4);
#endif

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x2A));
    lcd_dbi_transmit_cmd_para(SPD2010_QSPI_CMD, (void *)param, 4);
#else
    lcd_dbi_transmit_cmd_para(0x2A, (void *)param, 4);
#endif
}

/**
 * @brief spd2010_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void spd2010_dbi_draw_point(uint16_t x, uint16_t y, spd2010_dbi_color_t color)
{
    /* set window */
    spd2010_dbi_set_draw_window(x, y, x, y);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_sync(SPD2010_QSPI_CMD, (void *)&color, 1);
#else
    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
#endif
}

/**
 * @brief spd2010_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void spd2010_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, spd2010_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if SPD2010_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif SPD2010_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    spd2010_dbi_set_draw_window(x1, y1, x2, y2);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_fill_sync(SPD2010_QSPI_CMD, color_src, pixel_num);
#else
    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
#endif
}

/**
 * @brief spd2010_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (spd2010_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void spd2010_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, spd2010_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    spd2010_dbi_set_draw_window(x1, y1, x2, y2);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_async(SPD2010_QSPI_CMD, (void *)picture, pixel_num);
#else
    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
#endif
}

/**
 * @brief spd2010_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void spd2010_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, spd2010_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    spd2010_dbi_set_draw_window(x1, y1, x2, y2);

#if (LCD_DBI_WORK_MODE == 4)
    lcd_dbi_ex_qspi_addr_cfg(SPD2010_QSPI_ADDR_BYTE, SPD2010_QSPI_ADDR(0x2C));
    lcd_dbi_transmit_cmd_pixel_sync(SPD2010_QSPI_CMD, (void *)picture, pixel_num);
#else
    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
#endif
}

#endif
