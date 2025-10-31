/**
 * @file st7701s_dpi.c
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

#if defined(LCD_DPI_ST7701S)

#include "st7701s_dpi.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bl_mipi_dpi_sim.h"

#if (LCD_DPI_INIT_INTERFACE_TYPE == 1)
#include "bl_dpi_init_spi_soft_3.h"

#define lcd_dpi_init_init              lcd_dpi_init_spi_soft_3_init
#define lcd_dpi_init_transmit_cmd_para lcd_dpi_init_spi_soft_3_transmit_cmd_para
#endif

/* mipi dpi (RGB) paramant (HSW+HBP+HFP >= 4.5us) */
static lcd_mipi_dpi_init_t dpi_para = {
    .width = ST7701S_DPI_W,  /* LCD Active Width */
    .height = ST7701S_DPI_H, /* LCD Active Height */
                             /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 20 / 2,           /* LCD HSW (Hsync Pulse Width) */
    .hbp = 20 / 2,           /* LCD HBP (Hsync Back Porch) */
    .hfp = 20 / 2,           /* LCD HFP (Hsync Front Porch) */
                             /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 4,                /* LCD VSW (Vsync Pulse Width) */
    .vbp = 8,                /* LCD VBP (Vsync Back Porch) */
    .vfp = 4,                /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (ST7701S_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_PIXEL_FORMAT_RGB565,
#endif
    .de_mode_en = 1,
    .frame_buff = NULL,
};

static const st7701s_dpi_init_cmd_t st7701s_dpi_mode_init_cmds[] = {

    { 0x00, NULL, 0 },  /* nop */
    { 0x01, NULL, 0 },  /* soft reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0xFF, "\x77\x01\x00\x00\x13", 5 }, /* Command 2 BK3 */
    { 0xEF, "\x08", 1 },

    { 0xFF, "\x77\x01\x00\x00\x10", 5 }, /* Command 2 BK0 */
    { 0xC0, "\x3B\x00", 2 }, /* Display Line Setting, (0x3B+1)*8=480 */
    { 0xC1, "\x0B\x02", 2 }, /* Porch Control, VBP:15, VFP:2 */
    { 0xC2, "\x30\x02", 2 }, /* Inversion selection & Frame Rate Control */
    { 0xCC, "\x10", 1 },
    { 0xCD, "\x04", 1 }, /* MDT=0 */

    { 0xB0, "\x00\x0F\x16\x0E\x11\x07\x09\x09\x08\x23\x05\x11\x0F\x28\x2D\x18", 16 },
    { 0xB1, "\x00\x0F\x16\x0E\x11\x07\x09\x09\x08\x23\x05\x11\x0F\x28\x2D\x18", 16 },

    { 0xFF, "\x77\x01\x00\x00\x11", 5 }, /* Command 2 BK1 */
    { 0xB0, "\x4D", 1 },
    { 0xB1, "\x33", 1 },
    { 0xB2, "\x87", 1 },
    { 0xB5, "\x4B", 1 },
    { 0xB7, "\x8C", 1 },
    { 0xB8, "\x20", 1 },
    { 0xC1, "\x78", 1 },
    { 0xC2, "\x78", 1 },
    { 0xFF, NULL, 20 }, /* delay 20ms */

    { 0xE0, "\x00\x00\x02", 3 },
    { 0xE1, "\x02\xF0\x00\x00\x03\xF0\x00\x00\x00\x44\x44", 11 },
    { 0xE2, "\x10\x10\x40\x40\xF2\xF0\x00\x00\xF2\xF0\x00\x00", 12 },
    { 0xE3, "\x00\x00\x11\x11", 4 },
    { 0xE4, "\x44\x44", 2 },
    { 0xE5, "\x07\xEF\xF0\xF0\x09\xF1\xF0\xF0\x03\xF3\xF0\xF0\x05\xED\xF0\xF0", 16 },
    { 0xE6, "\x00\x00\x11\x11", 4 },
    { 0xE7, "\x44\x44", 2 },
    { 0xE8, "\x08\xf0\xf0\xf0\x0a\xf2\xf0\xf0\x04\xf4\xf0\xf0\x06\xee\xf0\xf0", 16 },
    { 0xEB, "\x00\x00\xE4\xE4\x44\x88\x40", 7 },
    { 0xEC, "\x78\x00", 2 },
    { 0xED, "\x20\xf9\x87\x76\x65\x54\x4F\xFF\xFF\xF4\x45\x56\x67\x78\x9F\x02", 16 },
    { 0xEF, "\x10\x0D\x04\x08\x3F\x1F", 6 },

    { 0xFF, "\x77\x01\x00\x00\x00", 5 }, /* disable the BK function of Command2 */

    { 0x35, "\x00", 1 }, /* Tearing Effect Line ON */

#if (ST7701S_DPI_RGB_ORDER_MODE)
    { 0x36, "\x08", 1 }, /* B-G-R */
#else
    { 0x36, "\x00", 1 }, /* R-G-B */
#endif

    { 0x3A, "\x66", 1 }, /* 18bit/pixel */
    
    { 0x11, NULL, 0 },
    { 0xFF, NULL, 120 }, /* delay 120ms */
    { 0x29, NULL, 0 },   /* display On */

};

int ATTR_TCM_SECTION st7701s_dpi_init(st7701s_dpi_color_t *screen_buffer)
{
    lcd_dpi_init_init();

    for (uint16_t i = 0; i < (sizeof(st7701s_dpi_mode_init_cmds) / sizeof(st7701s_dpi_init_cmd_t)); i++) {
        if ((st7701s_dpi_mode_init_cmds[i].cmd == 0xFF) && (st7701s_dpi_mode_init_cmds[i].data == NULL) && (st7701s_dpi_mode_init_cmds[i].databytes)) {
            /* delay */
            bflb_mtimer_delay_ms(st7701s_dpi_mode_init_cmds[i].databytes);
        } else {
            /* send cmd and para */
            lcd_dpi_init_transmit_cmd_para(st7701s_dpi_mode_init_cmds[i].cmd, (void *)(st7701s_dpi_mode_init_cmds[i].data), st7701s_dpi_mode_init_cmds[i].databytes);
        }
    }

    /* mipi dpi init */
    if (screen_buffer == NULL) {
        return -1;
    }
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int st7701s_dpi_screen_switch(st7701s_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

st7701s_dpi_color_t *st7701s_dpi_get_screen_using(void)
{
    return (st7701s_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int st7701s_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif