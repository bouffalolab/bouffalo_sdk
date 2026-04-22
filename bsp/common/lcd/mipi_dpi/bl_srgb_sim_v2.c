/**
 * @file bl_srgb_sim_v2.c
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

#if defined(LCD_DPI_SIM_V2_ENABLE)

#include "hardware/dma_reg.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "bflb_pec_v2_instance.h"

#include "bl_srgb_sim_v2.h"

#define DMA_MAX_COUNT 4032

#ifndef LCD_SIM_DPI_V2_DMA_LLI_NUM
#define LCD_SIM_DPI_V2_DMA_LLI_NUM 64
#endif

static volatile void *screen_last, *next_disp_buffer;
static uint16_t width, height;
static uint16_t pix_format, pixel_unit;

typedef void (*srgb_sim_v2_callback_t)(void);
static volatile srgb_sim_v2_callback_t frame_callback;
static volatile srgb_sim_v2_callback_t frame_swap_callback;

static struct bflb_device_s *pec_dpi_dev, *dma_ch_dev;
static uint32_t dma_lli_num, frame_nword_cnt;

static __ALIGNED(64) struct bflb_dma_channel_lli_pool_s dma_lli_pool[1][LCD_SIM_DPI_V2_DMA_LLI_NUM];

/* Initialize DMA linked list */
static void sim_v2_dma_lli_init(void)
{
    union bflb_dma_lli_control_s control = {
        .bits.TransferSize = DMA_MAX_COUNT,
        .bits.SBSize = DMA_BURST_INCR4,
        .bits.dst_min_mode = 0,
        .bits.DBSize = DMA_BURST_INCR4,
        .bits.dst_add_mode = 0,
        .bits.SWidth = DMA_DATA_WIDTH_32BIT,
        .bits.DWidth = DMA_DATA_WIDTH_32BIT,
        .bits.fix_cnt = 0,
        .bits.SI = DMA_ADDR_INCREMENT_ENABLE,
        .bits.DI = DMA_ADDR_INCREMENT_DISABLE,
        .bits.I = 0
    };

#if defined(BL616CL)
    /* DMA, ch0~ch1: 64Byte, ch2~ch5: 32Byte, ch6~ch7: 16Byte */
    if (pec_dpi_dev->idx == 0 && dma_ch_dev->sub_idx < 2) {
        control.bits.SBSize = DMA_BURST_INCR16;
        control.bits.DBSize = DMA_BURST_INCR16;
    } else if (dma_ch_dev->sub_idx < 6) {
        control.bits.SBSize = DMA_BURST_INCR8;
        control.bits.DBSize = DMA_BURST_INCR8;
    }
#endif

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < dma_lli_num; j++) {
            dma_lli_pool[i][j].control.WORD = control.WORD;
            dma_lli_pool[i][j].src_addr = (uint32_t)(uintptr_t)NULL;
            dma_lli_pool[i][j].nextlli = (uint32_t)(uintptr_t)&dma_lli_pool[i][j + 1];

            if (pec_dpi_dev->idx == 0) {
                dma_lli_pool[i][j].dst_addr = DMA_ADDR_PEC_SM0_TDR;
            } else if (pec_dpi_dev->idx == 1) {
                dma_lli_pool[i][j].dst_addr = DMA_ADDR_PEC_SM1_TDR;
            }
#if defined(DMA_ADDR_PEC_SM2_TDR)
            else if (pec_dpi_dev->idx == 2) {
                dma_lli_pool[i][j].dst_addr = DMA_ADDR_PEC_SM2_TDR;
            }
#elif defined(DMA_ADDR_PEC_SM3_TDR)
            else if (pec_dpi_dev->idx == 3) {
                dma_lli_pool[i][j].dst_addr = DMA_ADDR_PEC_SM3_TDR;
            }
#endif
        }
        /* last LLI */
        dma_lli_pool[i][dma_lli_num - 1].nextlli = (uint32_t)(uintptr_t)NULL;
        dma_lli_pool[i][dma_lli_num - 1].control.bits.TransferSize = frame_nword_cnt - (DMA_MAX_COUNT * (dma_lli_num - 1));
        dma_lli_pool[i][dma_lli_num - 1].control.bits.I = 1;
    }
}

/* Update DMA linked list with new framebuffer address */
static void sim_v2_dma_lli_update(void *framebuffer, struct bflb_dma_channel_lli_pool_s *dma_lli_pool)
{
    for (int i = 0; i < dma_lli_num; i++) {
        dma_lli_pool[i].src_addr = DMA_MAX_COUNT * 4 * i + (uint32_t)(uintptr_t)framebuffer;
    }
}

/* update lli to register and start dma */
static void sim_v2_dma_lli_start(struct bflb_dma_channel_lli_pool_s *dma_lli_pool)
{
    uint32_t channel_base = dma_ch_dev->reg_base;
    putreg32(dma_lli_pool->src_addr, channel_base + DMA_CxSRCADDR_OFFSET);
    putreg32(dma_lli_pool->dst_addr, channel_base + DMA_CxDSTADDR_OFFSET);
    putreg32(dma_lli_pool->nextlli, channel_base + DMA_CxLLI_OFFSET);
    putreg32(dma_lli_pool->control.WORD, channel_base + DMA_CxCONTROL_OFFSET);

    bflb_l1c_dcache_clean_range((void *)dma_lli_pool, sizeof(struct bflb_dma_channel_lli_pool_s) * dma_lli_num);

    bflb_dma_channel_start(dma_ch_dev);
}

/* DMA interrupt service routine */
static void sim_v2_dma_isr(void *arg)
{
    bool swap_flag = false;

    (void)arg;

    if (next_disp_buffer != screen_last) {
        swap_flag = true;
        screen_last = next_disp_buffer;

        sim_v2_dma_lli_update((void *)screen_last, dma_lli_pool[0]);
    }

    /* update lli to register and start dma */
    sim_v2_dma_lli_start(dma_lli_pool[0]);

    if (frame_callback != NULL) {
        frame_callback();
    }

    if ((frame_swap_callback != NULL) && swap_flag) {
        frame_swap_callback();
    }
}

/* Initialize the sRGB simulation with given parameters */
int lcd_srgb_sim_v2_init(lcd_srgb_sim_v2_init_t *dpi_para)
{
    int ret;

    if ((dpi_para == NULL) || (dpi_para->frame_buff == NULL)) {
        return -1;
    }

    width = dpi_para->width;
    height = dpi_para->height;
    pix_format = dpi_para->pixel_format;
    screen_last = dpi_para->frame_buff;
    next_disp_buffer = dpi_para->frame_buff;

    pec_dpi_dev = bflb_device_get_by_name(LCD_DPI_SIM_V2_PEC_NAME);
    dma_ch_dev = bflb_device_get_by_name(LCD_SIM_DPI_V2_DMA_NAME);

    /* sim gpio init */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, LCD_DPI_SIM_V2_PIN_PCLK, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, LCD_DPI_SIM_V2_PIN_HSYNC, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, LCD_DPI_SIM_V2_PIN_VSYNC, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, LCD_DPI_SIM_V2_PIN_DE, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    for (uint8_t i = 0; i < 8; i++) {
        bflb_gpio_init(gpio, LCD_DPI_SIM_V2_PIN_DATA_START + i, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    }

    /* sim srgb cfg */
    struct bflb_pec_srgb_s srgb_cfg = {
        .mem = 0,                                        /*!< memory address of first instruction */
        .format = PEC_SRGB_FORMAT_NRGB888,               /*!< sRGB data format, PEC_SRGB_FORMAT_xxx */
        .clk_freq = PEC_SRGB_CLOCK_27MHZ,                /*!< sRGB clock frequency, PEC_SRGB_CLOCK_xxx */
        .dma_enable = true,                              /*!< enable or disable dma */
        .fifo_threshold = 4 - 1,                         /*!< tx fifo threshold */
        .pin_data_start = LCD_DPI_SIM_V2_PIN_DATA_START, /*!< sRGB data pin start index, data pin number must be continuous */
        .pin_clk = LCD_DPI_SIM_V2_PIN_PCLK,              /*!< sRGB clock pin */
        .pin_vsync = LCD_DPI_SIM_V2_PIN_VSYNC,           /*!< sRGB vsync pin */
        .pin_hsync = LCD_DPI_SIM_V2_PIN_HSYNC,           /*!< sRGB hsync pin */
        .pin_de = LCD_DPI_SIM_V2_PIN_DE,                 /*!< sRGB de pin */
        .level_clk = LCD_SIM_DPI_V2_PCLK_POL,            /*!< sRGB clock level, 0: sample on falling edge, others: sample on rising edge */
        .level_vsync = LCD_SIM_DPI_V2_VSYNC_POL,         /*!< sRGB vsync level in vertical synchronization segment */
        .level_hsync = LCD_SIM_DPI_V2_HSYNC_POL,         /*!< sRGB hsync level in horizontal synchronization segment */
        .level_de = LCD_SIM_DPI_V2_DE_POL,               /*!< sRGB de level when data is enabled */
        .hsync = dpi_para->hsw,                          /*!< sRGB clock count in horizontal synchronization segment, max is 1024 */
        .hbp = dpi_para->hbp,                            /*!< sRGB clock count in horizontal back porch segment, max is 2048 */
        .hfp = dpi_para->hfp,                            /*!< sRGB clock count in horizontal front porch segment, max is 2048 */
        .vsync = dpi_para->vsw,                          /*!< sRGB row count in vertical synchronization segment, max is 1024 */
        .vbp = dpi_para->vbp,                            /*!< sRGB row count in vertical back porch segment, max is 2048 */
        .vfp = dpi_para->vfp,                            /*!< sRGB row count in vertical front porch segment, max is 2048 */
        .width = width,                                  /*!< sRGB actual picture width, max is 65536 */
        .height = height,                                /*!< sRGB actual picture height, max is 65536 */
    };

    /* sim dma cfg */
    struct bflb_dma_channel_config_s dma_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_PEC_SM0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
    if (pec_dpi_dev->idx == 0) {
        dma_cfg.dst_req = DMA_REQUEST_PEC_SM0_TX;
    } else if (pec_dpi_dev->idx == 1) {
        dma_cfg.dst_req = DMA_REQUEST_PEC_SM1_TX;
    }
#if defined(DMA_ADDR_PEC_SM2_TDR)
    else if (pec_dpi_dev->idx == 2) {
        dma_cfg.dst_req = DMA_REQUEST_PEC_SM2_TX;
    }
#endif
#if defined(DMA_ADDR_PEC_SM3_TDR)
    else if (pec_dpi_dev->idx == 3) {
        dma_cfg.dst_req = DMA_REQUEST_PEC_SM3_TX;
    }
#endif

    /* fifo threshold cfg */
#if defined(BL616CL)
    /* PEC, sm0: tx-4x32,rx-4x16; sm0: tx/rx 4x16 */
    /* DMA, ch0~ch1: 64Byte, ch2~ch5: 32Byte, ch6~ch7: 16Byte */
    if (pec_dpi_dev->idx == 0 && dma_ch_dev->sub_idx < 2) {
        srgb_cfg.fifo_threshold = 16 - 1;
        dma_cfg.src_burst_count = DMA_BURST_INCR16;
        dma_cfg.dst_burst_count = DMA_BURST_INCR16;
    } else if (dma_ch_dev->sub_idx < 6) {
        srgb_cfg.fifo_threshold = 8 - 1;
        dma_cfg.src_burst_count = DMA_BURST_INCR8;
        dma_cfg.dst_burst_count = DMA_BURST_INCR8;
    }
#endif

    /* pixel format cfg */
    if (pix_format == LCD_SRGB_SIM_V2_PIXEL_FORMAT_RGB565) {
        pixel_unit = 2;
        srgb_cfg.format = PEC_SRGB_FORMAT_RGB565;
    } else if (pix_format == LCD_SRGB_SIM_V2_PIXEL_FORMAT_RGB888) {
        pixel_unit = 3;
        srgb_cfg.format = PEC_SRGB_FORMAT_RGB888;
    } else if (pix_format == LCD_SRGB_SIM_V2_PIXEL_FORMAT_NRGB8888) {
        pixel_unit = 4;
        srgb_cfg.format = PEC_SRGB_FORMAT_NRGB888;
    } else {
        return -1;
    }
    frame_nword_cnt = (width * height * pixel_unit) / 4;
    dma_lli_num = (frame_nword_cnt + DMA_MAX_COUNT - 1) / DMA_MAX_COUNT;
    if (dma_lli_num > LCD_SIM_DPI_V2_DMA_LLI_NUM) {
        return -1;
    }

    /* sim srgb init */
    ret = bflb_pec_srgb_init(pec_dpi_dev, &srgb_cfg);
    if (ret < 0) {
        return ret;
    }

    /* sim dma init */
    bflb_dma_channel_init(dma_ch_dev, &dma_cfg);
    bflb_dma_channel_irq_attach(dma_ch_dev, sim_v2_dma_isr, NULL);
    /* Initialize DMA linked list */
    sim_v2_dma_lli_init();

    /* clean dcache */
    bflb_l1c_dcache_clean_range((void *)screen_last, frame_nword_cnt * 4);

    /* update DMA linked list with new framebuffer address */
    sim_v2_dma_lli_update((void *)screen_last, dma_lli_pool[0]);
    /* update lli to register and start dma */
    sim_v2_dma_lli_start(dma_lli_pool[0]);

    return 0;
}

/* Switch to a new screen buffer for the next frame */
int lcd_srgb_sim_v2_screen_switch(void *screen_buffer)
{
    if (screen_buffer == NULL) {
        return -1;
    }

    bflb_l1c_dcache_clean_range(screen_buffer, frame_nword_cnt * 4);
    next_disp_buffer = screen_buffer;

    return 0;
}

/* Get the current screen buffer being displayed */
void *lcd_srgb_sim_v2_get_screen_using(void)
{
    return (void *)screen_last;
}

/* Register frame callbacks for cycle and swap events */
int lcd_srgb_sim_v2_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == LCD_SRGB_SIM_V2_FRAME_INT_TYPE_SWAP) {
        frame_swap_callback = callback;
    } else if (callback_type == LCD_SRGB_SIM_V2_FRAME_INT_TYPE_CYCLE) {
        frame_callback = callback;
    }

    return 0;
}

#endif