/**
 * @file st7701p_dpi.c
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

#if defined(LCD_DPI_ST7701P)

#include "st7701p_dpi.h"
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

#if (LCD_DPI_INTERFACE_TYPE == 2)
#include "bl_mipi_dpi_sim.h"
#define lcd_mipi_dpi_init                    lcd_mipi_dpi_sim_init
#define lcd_mipi_dpi_screen_switch           lcd_mipi_dpi_sim_screen_switch
#define lcd_mipi_dpi_get_screen_using        lcd_mipi_dpi_sim_get_screen_using
#define lcd_mipi_dpi_frame_callback_register lcd_mipi_dpi_sim_frame_callback_register
#define LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP     LCD_MIPI_DPI_SIM_FRAME_INT_TYPE_SWAP
#define LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE    LCD_MIPI_DPI_SIM_FRAME_INT_TYPE_CYCLE

/* mipi dpi (RGB) paramant (HSW+HBP+HFP >= 4.5us) */
static lcd_mipi_dpi_sim_init_t dpi_para = {
    .width = ST7701P_DPI_W,  /* LCD Active Width */
    .height = ST7701P_DPI_H, /* LCD Active Height */
                             /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 10,               /* LCD HSW (Hsync Pulse Width) */
    .hbp = 20,               /* LCD HBP (Hsync Back Porch) */
    .hfp = 10,               /* LCD HFP (Hsync Front Porch) */
                             /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 2,                /* LCD VSW (Vsync Pulse Width) */
    .vbp = 20,               /* LCD VBP (Vsync Back Porch) */
    .vfp = 12,               /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (ST7701P_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_SIM_PIXEL_FORMAT_RGB565,
#endif
    .de_mode_en = 1,
    .frame_buff = NULL,
};
#elif (LCD_DPI_INTERFACE_TYPE == 3)

#include "bl_mipi_dpi_v2.h"

#define lcd_mipi_dpi_init                    bl_mipi_dpi_v2_init
#define lcd_mipi_dpi_screen_switch           bl_mipi_dpi_v2_screen_switch
#define lcd_mipi_dpi_get_screen_using        bl_mipi_dpi_v2_get_screen_using
#define lcd_mipi_dpi_frame_callback_register bl_mipi_dpi_v2_frame_callback_register
#define LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP     LCD_MIPI_DPI_V2_FRAME_INT_TYPE_SWAP
#define LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE    LCD_MIPI_DPI_V2_FRAME_INT_TYPE_CYCLE

static lcd_mipi_dpi_v2_init_t dpi_para = {
    .width = ST7701P_DPI_W,
    .height = ST7701P_DPI_H,
                             /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 10,               /* LCD HSW (Hsync Pulse Width) */
    .hbp = 20,               /* LCD HBP (Hsync Back Porch) */
    .hfp = 10,               /* LCD HFP (Hsync Front Porch) */
                             /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 2,                /* LCD VSW (Vsync Pulse Width) */
    .vbp = 20,               /* LCD VBP (Vsync Back Porch) */
    .vfp = 12,               /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (ST7701P_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_V2_PIXEL_FORMAT_RGB565,
#elif (ST7701P_DPI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_MIPI_DPI_V2_PIXEL_FORMAT_NRGB888,
#endif
    .de_mode_en = 1,
    .frame_buff = NULL,
};
#endif
static const st7701p_dpi_init_cmd_t st7701p_dpi_mode_init_cmds[] = {
    { 0xFF, "\x77\x01\x00\x00\x13", 5 },
    { 0xEF, "\x08", 1 },

    { 0xFF, "\x77\x01\x00\x00\x10", 5 },
    { 0xC0, "\x3B\x00", 2 },
    { 0xC1, "\x10\x0C", 2 },
    { 0xC2, "\x17\x0A", 2 },
    { 0xC3, "\x02", 1 },
    { 0xCC, "\x10", 1 },
    { 0xCD, "\x08", 1 },

    { 0xB0, "\x40\x0E\x58\x0E\x12\x08\x0C\x09\x09\x27\x07\x18\x15\x78\x26\xC7", 16 },
    { 0xB1, "\x40\x13\x5B\x0D\x11\x06\x0A\x08\x08\x26\x03\x13\x12\x79\x28\xC9", 16 },

    { 0xFF, "\x77\x01\x00\x00\x11", 5 },
    { 0xB0, "\x6D", 1 },
    { 0xB1, "\x38", 1 },
    { 0xB2, "\x81", 1 },
    { 0xB3, "\x80", 1 },
    { 0xB5, "\x4E", 1 },
    { 0xB7, "\x85", 1 },
    { 0xB8, "\x20", 1 },
    { 0xC1, "\x78", 1 },
    { 0xC2, "\x78", 1 },
    { 0xD0, "\x88", 1 },

    { 0xE0, "\x00\x00\x02", 3 },
    { 0xE1, "\x06\x30\x08\x30\x05\x30\x07\x30\x00\x33\x33", 11 },
    { 0xE2, "\x11\x11\x33\x33\xF4\x00\x00\x00\xF4\x00\x00\x00", 12 },
    { 0xE3, "\x00\x00\x11\x11", 4 },
    { 0xE4, "\x44\x44", 2 },
    { 0xE5, "\x0D\xF5\x30\xF0\x0F\xF7\x30\xF0\x09\xF1\x30\xF0\x0B\xF3\x30\xF0", 16 },
    { 0xE6, "\x00\x00\x11\x11", 4 },
    { 0xE7, "\x44\x44", 2 },
    { 0xE8, "\x0C\xF4\x30\xF0\x0E\xF6\x30\xF0\x08\xF0\x30\xF0\x0A\xF2\x30\xF0", 16 },
    { 0xE9, "\x36\x01", 2 },
    { 0xEB, "\x00\x01\xE4\xE4\x44\x88\x40", 7 },
    { 0xED, "\xFF\x45\x67\xFA\x01\x2B\xCF\xFF\xFF\xFC\xB2\x10\xAF\x76\x54\xFF", 16 },
    { 0xEF, "\x10\x0D\x04\x08\x3F\x1F", 6 },

    { 0xFF, "\x77\x01\x00\x00\x00", 5 },
#if (ST7701P_DPI_RGB_ORDER_MODE)
    { 0x36, "\x08", 1 },
#else
    { 0x36, "\x00", 1 },
#endif
    { 0x11, NULL, 0 },
    { 0xFF, NULL, 120 },
    { 0x29, NULL, 0 },
    { 0x35, "\x00", 1 },
};

int ATTR_TCM_SECTION st7701p_dpi_init(st7701p_dpi_color_t *screen_buffer)
{
    lcd_dpi_init_init();

    for (uint16_t i = 0; i < (sizeof(st7701p_dpi_mode_init_cmds) / sizeof(st7701p_dpi_init_cmd_t)); i++) {
        if ((st7701p_dpi_mode_init_cmds[i].cmd == 0xFF) && (st7701p_dpi_mode_init_cmds[i].data == NULL) && (st7701p_dpi_mode_init_cmds[i].databytes)) {
            /* delay */
            bflb_mtimer_delay_ms(st7701p_dpi_mode_init_cmds[i].databytes);
        } else {
            /* send cmd and para */
            lcd_dpi_init_transmit_cmd_para(st7701p_dpi_mode_init_cmds[i].cmd, (void *)(st7701p_dpi_mode_init_cmds[i].data), st7701p_dpi_mode_init_cmds[i].databytes);
        }
    }

    /* mipi dpi init */
    if (screen_buffer == NULL) {
        return -1;
    }
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int st7701p_dpi_screen_switch(st7701p_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

st7701p_dpi_color_t *st7701p_dpi_get_screen_using(void)
{
    return (st7701p_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int st7701p_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif