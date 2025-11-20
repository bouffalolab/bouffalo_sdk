/**
 * @file standard_dpi.c
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
#include "standard_dpi.h"

#if defined(LCD_DPI_STANDARD)

#if (LCD_DPI_INTERFACE_TYPE == 2)

#include "bl_mipi_dpi_sim.h"

#define lcd_mipi_dpi_init                    lcd_mipi_dpi_sim_init
#define lcd_mipi_dpi_screen_switch           lcd_mipi_dpi_sim_screen_switch
#define lcd_mipi_dpi_get_screen_using        lcd_mipi_dpi_sim_get_screen_using
#define lcd_mipi_dpi_frame_callback_register lcd_mipi_dpi_sim_frame_callback_register
#define LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP     LCD_MIPI_DPI_SIM_FRAME_INT_TYPE_SWAP
#define LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE    LCD_MIPI_DPI_SIM_FRAME_INT_TYPE_CYCLE

/* mipi dpi (RGB) paramant */
static lcd_mipi_dpi_sim_init_t dpi_para = {
    .width = STANDARD_DPI_W,  /* LCD Active Width */
    .height = STANDARD_DPI_H, /* LCD Active Height */
                              /* Total Width = HSW + HBP + Active_Width + HFP */
    .hsw = STANDARD_DPI_HSW,  /* LCD HSW (Hsync Pulse Width) */
    .hbp = STANDARD_DPI_HBP,  /* LCD HBP (Hsync Back Porch) */
    .hfp = STANDARD_DPI_HFP,  /* LCD HFP (Hsync Front Porch) */
                              /* Total Height = VSW + VBP + Active_Height + VFP */
    .vsw = STANDARD_DPI_VSW,  /* LCD VSW (Vsync Pulse Width) */
    .vbp = STANDARD_DPI_VBP,  /* LCD VBP (Vsync Back Porch) */
    .vfp = STANDARD_DPI_VFP,  /* LCD VFP (Vsync Front Porch) */

    .frame_rate = STANDARD_DPI_FRAME_RATE, /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

#if (STANDARD_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_SIM_PIXEL_FORMAT_RGB565,
#endif

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
    .width = STANDARD_DPI_W,
    .height = STANDARD_DPI_H,
    .hsw = STANDARD_DPI_HSW,
    .hbp = STANDARD_DPI_HBP,
    .hfp = STANDARD_DPI_HFP,
    .vsw = STANDARD_DPI_VSW,
    .vbp = STANDARD_DPI_VBP,
    .vfp = STANDARD_DPI_VFP,

    .frame_rate = STANDARD_DPI_FRAME_RATE,
#if (STANDARD_DPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_MIPI_DPI_V2_PIXEL_FORMAT_RGB565,
#else
    .pixel_format = LCD_MIPI_DPI_V2_PIXEL_FORMAT_NRGB888,
#endif
    .frame_buff = NULL,
};

#else

#error "Configuration error"

#endif

int standard_dpi_init(standard_dpi_color_t *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    /* mipi dpi init */
    dpi_para.frame_buff = (void *)screen_buffer;
    return lcd_mipi_dpi_init(&dpi_para);
}

int standard_dpi_screen_switch(standard_dpi_color_t *screen_buffer)
{
    return lcd_mipi_dpi_screen_switch((void *)screen_buffer);
}

standard_dpi_color_t *standard_dpi_get_screen_using(void)
{
    return (standard_dpi_color_t *)lcd_mipi_dpi_get_screen_using();
}

int standard_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE, callback);
    } else if (callback_type == FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_callback_register(LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP, callback);
    }

    return 0;
}

#endif
