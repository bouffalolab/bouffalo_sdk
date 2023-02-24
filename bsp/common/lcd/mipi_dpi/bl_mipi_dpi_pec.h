/**
 * @file bl_mipi_dpi.c
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

#ifndef _BL_MIPI_DPI_H_
#define _BL_MIPI_DPI_H_

#include "../lcd_conf.h"

#if defined LCD_PEC_SIMU_DPI_ENABLE

/* Do not modify this file ! */

#define LCD_MIPI_DPI_PIXEL_FORMAT_RGB565  0
#define LCD_MIPI_DPI_PIXEL_FORMAT_NRGB888 1

#define LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE 0
#define LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP  1

typedef struct {
    uint16_t width;  /* LCD Active Width */
    uint16_t height; /* LCD Active Height */
                     /* Total Width = HSW + HBP + Active_Width + HFP */
    uint16_t hsw;    /* LCD HSW (Hsync Pulse Width) */
    uint16_t hbp;    /* LCD HBP (Hsync Back Porch) */
    uint16_t hfp;    /* LCD HFP (Hsync Front Porch) */
                     /* Total Height = VSW + VBP + Active_Height + VFP */
    uint16_t vsw;    /* LCD VSW (Vsync Pulse Width) */
    uint16_t vbp;    /* LCD VBP (Vsync Back Porch) */
    uint16_t vfp;    /* LCD VFP (Vsync Front Porch) */

    uint16_t frame_rate; /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */

    uint16_t pixel_format;

    void *frame_buff; /* The frame buffer */
} lcd_mipi_dpi_init_t;

typedef struct {
    void *disp_buff;
    void *dma_lli;
    uint16_t lru_value;
} pec_dpi_dma_lli_cache_t;

/* PEC select */
#define PEC_ID_USE PEC0
#define SM_ID_USE  PEC_SM0

/* DMA select */
#define DMA_MAX_COUNT       4032
#define DMA_REQUEST_PEC_USE DMA_REQUEST_PEC0_SM0_TX

#if ((defined ILI9488_DPI_MODE) && (ILI9488_DPI_MODE == 2))
#define DPI_PEC_VALID_PIXEL_MAX (ILI9488_DPI_W * ILI9488_DPI_H)
#elif ((defined GC9503V_DPI_MODE) && (GC9503V_DPI_MODE == 2))
#define DPI_PEC_VALID_PIXEL_MAX (GC9503V_DPI_W * GC9503V_DPI_H)
#elif ((defined STANDARD_DPI_MODE) && (STANDARD_DPI_MODE == 2))
#define DPI_PEC_VALID_PIXEL_MAX (STANDARD_DPI_W * STANDARD_DPI_W)
#else
/* Maximum number of DPI valid pixels, >= (Width * Height) */
#define DPI_PEC_VALID_PIXEL_MAX (800 * 480)
#endif

int lcd_mipi_dpi_init(lcd_mipi_dpi_init_t *dpi_para);
int lcd_mipi_dpi_screen_switch(void *screen_buffer);
void *lcd_mipi_dpi_get_screen_using(void);
int lcd_mipi_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void));

#endif
#endif
