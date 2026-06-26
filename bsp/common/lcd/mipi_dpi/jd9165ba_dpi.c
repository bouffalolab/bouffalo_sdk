/**
 * @file jd9165ba_dpi.c
 * @brief JD9165BA TTL 24-bit parallel RGB panel, initialized over software SPI
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

#if defined(LCD_DPI_JD9165BA)

#include "jd9165ba_dpi.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"

#if (LCD_DPI_INIT_INTERFACE_TYPE == 1)

#include "bl_dpi_init_spi_soft_3.h"

#define lcd_dpi_init_init              lcd_dpi_init_spi_soft_3_init
#define lcd_dpi_init_transmit_cmd_para lcd_dpi_init_spi_soft_3_transmit_cmd_para

#elif (LCD_DPI_INIT_INTERFACE_TYPE == 2)

#include "bl_dpi_init_spi_soft_4.h"

#define lcd_dpi_init_init              lcd_dpi_init_spi_soft_4_init
#define lcd_dpi_init_transmit_cmd_para lcd_dpi_init_spi_soft_4_transmit_cmd_para

#endif
/* LCD_DPI_INIT_INTERFACE_TYPE == 0: plain parallel-RGB panel that needs no
 * register init (like standard_dpi). The vendor init sequence below and the
 * SW-SPI bring-up are compiled out, so nothing touches the SPI/CS pins. */

#if (LCD_DPI_INTERFACE_TYPE == 3)

#include "bl_mipi_dpi_v2.h"

#define lcd_mipi_dpi_init                    bl_mipi_dpi_v2_init
#define lcd_mipi_dpi_screen_switch           bl_mipi_dpi_v2_screen_switch
#define lcd_mipi_dpi_get_screen_using        bl_mipi_dpi_v2_get_screen_using
#define lcd_mipi_dpi_frame_callback_register bl_mipi_dpi_v2_frame_callback_register
#define LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP     LCD_MIPI_DPI_V2_FRAME_INT_TYPE_SWAP
#define LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE    LCD_MIPI_DPI_V2_FRAME_INT_TYPE_CYCLE

static lcd_mipi_dpi_v2_init_t dpi_para = {
    .width = JD9165BA_DPI_W,
    .height = JD9165BA_DPI_H,
                     /* RGB timing for 1024RGBx600 (datasheet 6.2 typ values), fixed by the panel.
                        Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 24,       /* LCD HSW (Hsync Pulse Width) */
    .hbp = 160,      /* LCD HBP (Hsync Back Porch) */
    .hfp = 160,      /* LCD HFP (Hsync Front Porch) */
                     /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 2,        /* LCD VSW (Vsync Pulse Width) */
    .vbp = 23,       /* LCD VBP (Vsync Back Porch) */
    .vfp = 12,       /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (JD9165BA_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_V2_PIXEL_FORMAT_RGB565,
#else
    .pixel_format = LCD_MIPI_DPI_V2_PIXEL_FORMAT_NRGB888,
#endif
    .de_mode_en = 1,
    .frame_buff = NULL,
};

#else

#error "JD9165BA only supports LCD_DPI_INTERFACE_TYPE == 3 (DPI v2)"

#endif

/*
 * JD9165BA vendor init sequence for the 1024x600 panel (GOA timing + gamma).
 * Registers are organized in pages selected via R30h; each page configures a
 * block (page 1: power/timing, page 2: GOA mapping + gamma, page 6/0A/0D:
 * source/VCOM tuning), then page 0 issues the standard SLPOUT/DISPON.
 *
 * The chip's SPI and DBI register-write formats are identical (page select +
 * register write), so this sequence is shared with MIPI-DSI/DBI deployments of
 * the same panel.
 *
 * Convention (matches the other DPI panels in this directory):
 *   {0xFF, NULL, N} -> delay N ms
 *   {cmd, data, N}  -> write cmd with N parameter bytes
 */
#if (LCD_DPI_INIT_INTERFACE_TYPE != 0)
static const jd9165ba_dpi_init_cmd_t jd9165ba_dpi_mode_init_cmds[] = {

    /* page 0 */
    { 0x30, "\x00", 1 },
    { 0xF7, "\x49\x61\x02\x00", 4 },

    /* page 1: power / interface / timing */
    { 0x30, "\x01", 1 },
    { 0x04, "\x0C", 1 },
    { 0x05, "\x00", 1 },
    { 0x06, "\x00", 1 },
    { 0x0B, "\x11", 1 },
    { 0x17, "\x00", 1 },
    { 0x20, "\x04", 1 },
    { 0x1F, "\x05", 1 },
    { 0x23, "\x00", 1 },
    { 0x25, "\x19", 1 },
    { 0x28, "\x18", 1 },
    { 0x29, "\x04", 1 },
    { 0x2A, "\x01", 1 },
    { 0x2B, "\x04", 1 },
    { 0x2C, "\x01", 1 },

    /* page 2: GOA mapping */
    { 0x30, "\x02", 1 },
    { 0x01, "\x22", 1 },
    { 0x03, "\x12", 1 },
    { 0x04, "\x00", 1 },
    { 0x05, "\x64", 1 },
    { 0x0A, "\x08", 1 },
    { 0x0B, "\x0A\x1A\x0B\x0D\x0D\x11\x10\x06\x08\x1F\x1D", 11 },
    { 0x0C, "\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D", 11 },
    { 0x0D, "\x16\x1B\x0B\x0D\x0D\x11\x10\x07\x09\x1E\x1C", 11 },
    { 0x0E, "\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D", 11 },
    { 0x0F, "\x16\x1B\x0D\x0B\x0D\x11\x10\x1C\x1E\x09\x07", 11 },
    { 0x10, "\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D", 11 },
    { 0x11, "\x0A\x1A\x0D\x0B\x0D\x11\x10\x1D\x1F\x08\x06", 11 },
    { 0x12, "\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D", 11 },
    { 0x14, "\x00\x00\x11\x11", 4 },
    { 0x18, "\x99", 1 },

    /* page 6: gamma */
    { 0x30, "\x06", 1 },
    { 0x12, "\x36\x2C\x2E\x3C\x38\x35\x35\x32\x2E\x1D\x2B\x21\x16\x29", 14 },
    { 0x13, "\x36\x2C\x2E\x3C\x38\x35\x35\x32\x2E\x1D\x2B\x21\x16\x29", 14 },

    /* page A: source */
    { 0x30, "\x0A", 1 },
    { 0x02, "\x4F", 1 },
    { 0x0B, "\x40", 1 },
    { 0x12, "\x3E", 1 },
    { 0x13, "\x78", 1 },

    /* page D: VCOM */
    { 0x30, "\x0D", 1 },
    { 0x0D, "\x04", 1 },
    { 0x10, "\x0C", 1 },
    { 0x11, "\x0C", 1 },
    { 0x12, "\x0C", 1 },
    { 0x13, "\x0C", 1 },

    /* back to page 0: sleep out + display on */
    { 0x30, "\x00", 1 },
    { 0x11, NULL, 0 },   /* SLPOUT (sleep out) */
    { 0xFF, NULL, 120 }, /* delay 120ms */
    { 0x29, NULL, 0 },   /* DISPON (display on) */
    { 0xFF, NULL, 20 },  /* delay 20ms */

};
#endif /* LCD_DPI_INIT_INTERFACE_TYPE != 0 */

int ATTR_TCM_SECTION jd9165ba_dpi_init(jd9165ba_dpi_color_t *screen_buffer)
{
#if (LCD_DPI_INIT_INTERFACE_TYPE != 0)
    lcd_dpi_init_init();

    for (uint16_t i = 0; i < (sizeof(jd9165ba_dpi_mode_init_cmds) / sizeof(jd9165ba_dpi_init_cmd_t)); i++) {
        if ((jd9165ba_dpi_mode_init_cmds[i].cmd == 0xFF) && (jd9165ba_dpi_mode_init_cmds[i].data == NULL) && (jd9165ba_dpi_mode_init_cmds[i].databytes)) {
            /* delay */
            bflb_mtimer_delay_ms(jd9165ba_dpi_mode_init_cmds[i].databytes);
        } else {
            /* send cmd and para */
            lcd_dpi_init_transmit_cmd_para(jd9165ba_dpi_mode_init_cmds[i].cmd, (void *)(jd9165ba_dpi_mode_init_cmds[i].data), jd9165ba_dpi_mode_init_cmds[i].databytes);
        }
    }
#endif /* LCD_DPI_INIT_INTERFACE_TYPE != 0 */

    /* mipi dpi init */
    if (screen_buffer == NULL) {
        return -1;
    }
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int jd9165ba_dpi_screen_switch(jd9165ba_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

jd9165ba_dpi_color_t *jd9165ba_dpi_get_screen_using(void)
{
    return (jd9165ba_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int jd9165ba_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif
