/**
 * @file ili9488_dpi.c
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

#if defined(LCD_DPI_ILI9488)

#include "ili9488_dpi.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bl_mipi_dpi_sim.h"

#if (LCD_DPI_INIT_INTERFACE_TYPE == 1)
#include "bl_dpi_init_spi_soft_3.h"

#define lcd_dpi_init_init              lcd_dpi_init_spi_soft_3_init
#define lcd_dpi_init_transmit_cmd_para lcd_dpi_init_spi_soft_3_transmit_cmd_para
#endif

/* mipi dpi (RGB) paramant */
static lcd_mipi_dpi_init_t dpi_para = {
    .width = ILI9488_DPI_W,  /* LCD Active Width */
    .height = ILI9488_DPI_H, /* LCD Active Height */
                             /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = 4,                /* LCD HSW (Hsync Pulse Width) */
    .hbp = 4,                /* LCD HBP (Hsync Back Porch) */
    .hfp = 12,               /* LCD HFP (Hsync Front Porch) */
                             /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = 6,                /* LCD VSW (Vsync Pulse Width) */
    .vbp = 6,                /* LCD VBP (Vsync Back Porch) */
    .vfp = 18,               /* LCD VFP (Vsync Front Porch) */

    .frame_rate = 60, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (ILI9488_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_PIXEL_FORMAT_RGB565,
#endif
    .de_mode_en = 1,
    .frame_buff = NULL,
};

static const ili9488_dpi_init_cmd_t ili9488_dpi_mode_init_cmds[] = {
    { 0x01, NULL, 0 },  /* software reset */
    { 0xFF, NULL, 10 }, /* delay 10ms */

    { 0x11, NULL, 0 },   /* Sleep Out */
    { 0xFF, NULL, 120 }, /* delay 120ms */

    { 0xE0, "\x00\x07\x0F\x0D\x1B\x0A\x3C\x78\x4A\x07\x0E\x09\x1B\x1E\x0F", 15 }, /* PGAMCTRL (Positive Gamma Control) */
    { 0xE1, "\x00\x22\x24\x26\x12\x07\x36\x47\x47\x06\x0A\x07\x30\x37\x0F", 15 }, /* NGAMCTRL (Negative Gamma Control) */

    { 0xC0, "\x10\x10", 2 }, /* Power Control 1 */
    { 0xC1, "\x41", 1 },     /* Power Control 2 */

    { 0xC5, "\x00\x20\xd0", 3 }, /* VCOM Control */
    { 0x36, "\x08", 1 },         /* Memory Access Control */

    { 0x3A, "\x50", 1 }, /* Interface Pixel 16bits/pixel*/

    { 0xB0, "\x00", 1 }, /* Interface Mode Control */
    { 0xB1, "\xB0", 1 }, /* Frame rate 70Hz */
    { 0xB4, "\x02", 1 }, /* Display Inversion Control */
    // { 0xB5, "\x08\x08\x06\x12", 4},/* Display Inversion Control */
    { 0xB6, "\x30\x22\x3B", 3 }, /* Display Function Control, DE Mode */
    { 0xBE, "\x00\x04", 1 },
    { 0xE9, "\x00", 1 }, /* Set Image Function */

    { 0xF7, "\xA9\x51\x2C\x82", 4 }, /* Adjust Control 3 */

    { 0x29, NULL, 0 }, /* Display On */
    { 0xFF, NULL, 10 },
};

int ili9488_dpi_init(ili9488_dpi_color_t *screen_buffer)
{
    lcd_dpi_init_init();

    for (uint16_t i = 0; i < (sizeof(ili9488_dpi_mode_init_cmds) / sizeof(ili9488_dpi_init_cmd_t)); i++) {
        if (ili9488_dpi_mode_init_cmds[i].cmd == 0xFF && (ili9488_dpi_mode_init_cmds[i].data == NULL) && (ili9488_dpi_mode_init_cmds[i].databytes) ) {
            bflb_mtimer_delay_ms(ili9488_dpi_mode_init_cmds[i].databytes);
        } else {
            /* send cmd and para */
            lcd_dpi_init_transmit_cmd_para(ili9488_dpi_mode_init_cmds[i].cmd, (void *)(ili9488_dpi_mode_init_cmds[i].data), ili9488_dpi_mode_init_cmds[i].databytes);
        }
    }

    /* mipi dpi init */
    if (screen_buffer == NULL) {
        return -1;
    }
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int ili9488_dpi_screen_switch(ili9488_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

ili9488_dpi_color_t *ili9488_dpi_get_screen_using(void)
{
    return (ili9488_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int ili9488_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif