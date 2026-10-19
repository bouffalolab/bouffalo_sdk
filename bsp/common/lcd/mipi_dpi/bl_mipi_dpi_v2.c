/**
 * @file bl_mipi_dpi_v2.c
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

#if defined(LCD_V2_DPI_ENABLE)

#include "bl_mipi_dpi_v2.h"
#include "bflb_dpi.h"
#include "bflb_osd.h"
#include "bflb_mtimer.h"
#include "bflb_l1c.h"
#include "bflb_gpio.h"
#include "board.h"
#include <string.h>

struct bflb_device_s *dpi_dev, *osd_dev;

static volatile void *screen_last = NULL;
static volatile void *next_disp_buffer = NULL;

typedef void (*mipi_dpi_v2_callback)(void);
static volatile mipi_dpi_v2_callback lcd_mipi_dpi_frame_callback = NULL;
static volatile mipi_dpi_v2_callback lcd_mipi_dpi_frame_swap_callback = NULL;

static void osd0_layer_isr(int irq, void *arg);

/**
 * @brief Base (DPI background) layer swap, invoked from the OSD SEOF ISR.
 *
 * Weak no-op by default: a plain LVGL app that only uses the OSD0 overlay needs
 * nothing here (the base layer just scans black). An app that drives a video
 * background overrides this (see the video pipeline in dpi_manager.c) to latch
 * the next decoded frame into the DPI base layer at the frame boundary.
 */
__attribute__((weak)) void bl_mipi_dpi_v2_osd0_base_layer_swap(void)
{
}

/* Global DPI configuration */
static lcd_mipi_dpi_v2_init_t *dpi_para = NULL;

/**
 * @brief Initialize DPI controller
 *
 * @param init_config DPI configuration parameters
 * @return int 0 on success, negative value on error
 */
int bl_mipi_dpi_v2_init(lcd_mipi_dpi_v2_init_t *init_config)
{
    if (init_config == NULL) {
        return -1;
    }
    if (init_config->frame_buff == NULL) {
        return -1;
    }

    dpi_dev = bflb_device_get_by_name(BFLB_NAME_DPI);
    if (dpi_dev == NULL) {
        return -2;
    }
    dpi_para = init_config;

    board_dpi_gpio_init();
    /* Single OSD0-overlay path: the DPI base layer scans a YUV background
       (framebuffer_addr starts at 0 -> black; the video pipeline switches in real
       YUV frames at runtime via the base-layer swap), and OSD0 blends the LVGL
       content on top. board_dpi_gpio_init() is done by the application (main.c)
       before lcd_init(). */
    struct bflb_dpi_config_s dpi_config = {
        .width = init_config->width,
        .height = init_config->height,
        .hsw = init_config->hsw,
        .hbp = init_config->hbp,
        .hfp = init_config->hfp,
        .vsw = init_config->vsw,
        .vbp = init_config->vbp,
        .vfp = init_config->vfp,
        .interface = LCD_DPI_V2_INTERFACE_TYPE,
        .input_sel = LCD_DPI_V2_INPUT_SEL,
        .test_pattern = LCD_DPI_V2_TEST_PATTERN,
        .data_format = DPI_DATA_FORMAT_Y_UV_PLANAR,
        .framebuffer_addr = 0,
        .uv_framebuffer_addr = 0,
    };

    /* Initialize DPI */
    printf("dpi init\r\n");
    bflb_dpi_init(dpi_dev, &dpi_config);
    bflb_dpi_feature_control(dpi_dev, DPI_CMD_SET_BURST, DPI_BURST_INCR8);

    printf("dpi enable\r\n");
    bflb_dpi_enable(dpi_dev);

    struct bflb_osd_blend_config_s osd0_blend_config = {
        .blend_format = (init_config->pixel_format == LCD_MIPI_DPI_V2_PIXEL_FORMAT_RGB565) ? OSD_BLEND_FORMAT_RGB565 : OSD_BLEND_FORMAT_ARGB8888,
        .order_a = 3,
        .order_rv = 2,
        .order_gy = 1,
        .order_bu = 0,
        .coor = {
            .start_x = 0,
            .start_y = 0,
            .end_x = init_config->width,
            .end_y = init_config->height,
        },
        .layer_buffer_addr = (uint32_t)init_config->frame_buff,
    };
    osd_dev = bflb_device_get_by_name(BFLB_NAME_OSD0);
    if (osd_dev == NULL) {
        return -3;
    }

    bflb_osd_int_mask(osd_dev, false);
    bflb_irq_attach(osd_dev->irq_num, osd0_layer_isr, NULL);
    bflb_irq_enable(osd_dev->irq_num);

    /* init blend 0 layer (the LVGL overlay) */
    bflb_osd_blend_init(osd_dev, &osd0_blend_config);
    bflb_osd_blend_enable(osd_dev);

    return 0;
}

/**
 * @brief Switch to another framebuffer
 *
 * @param screen_buffer Pointer to the new framebuffer
 * @return int 0 on success, negative value on error
 */
int bl_mipi_dpi_v2_screen_switch(void *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }
    if (dpi_para == NULL) {
        return -2;
    }

    /* Clean cache for new framebuffer */
    if (screen_buffer != NULL) {
        bflb_l1c_dcache_clean_range(screen_buffer, LCD_W * LCD_H * (LCD_COLOR_DEPTH / 8));
    }

    /* Re-point the OSD0 blend overlay (the LVGL canvas) at the new buffer. */
    bflb_osd_blend_set_layer_buffer(osd_dev, (uint32_t)screen_buffer);

    /* Publish after programming hardware so an intervening SEOF can only delay completion. */
    next_disp_buffer = screen_buffer;

    return 0;
}

/**
 * @brief Get current framebuffer in use
 *
 * @return void* Pointer to current framebuffer
 */
void *bl_mipi_dpi_v2_get_screen_using(void)
{
    uint32_t addr = bflb_osd_blend_get_layer_buffer(osd_dev);
    return (void *)addr;
}

/**
 * @brief Register frame interrupt callback
 *
 * @param callback_type Type of callback (CYCLE or SWAP)
 * @param callback Callback function pointer
 * @return int 0 on success, negative value on error
 */
int bl_mipi_dpi_v2_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == LCD_MIPI_DPI_V2_FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_swap_callback = callback;
    } else if (callback_type == LCD_MIPI_DPI_V2_FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback = callback;
    }
    return 0;
}

static void osd0_layer_isr(int irq, void *arg)
{
    uint8_t swap_flag = 0;

    bflb_osd_int_clear(osd_dev);

    /* Base (DPI background) layer swap -- weak no-op unless an app overrides it. */
    bl_mipi_dpi_v2_osd0_base_layer_swap();

    if (screen_last != next_disp_buffer) {
        swap_flag = 1;
        screen_last = next_disp_buffer;
    }

    if (lcd_mipi_dpi_frame_callback != NULL) {
        lcd_mipi_dpi_frame_callback();
    }

    if (lcd_mipi_dpi_frame_swap_callback != 0 && swap_flag) {
        lcd_mipi_dpi_frame_swap_callback();
    }
}

#endif
