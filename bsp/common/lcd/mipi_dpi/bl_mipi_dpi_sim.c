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

#include "../lcd.h"

#if (defined LCD_SIM_DPI_ENABLE)

#if (defined(BL616))

#include "bl_mipi_dpi_sim.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "bflb_dma.h"

#include "hardware/dma_reg.h"

/* Video memory used for this frame, Update in frame interrupt*/
static volatile void *screen_last = NULL;

typedef void (*standard_dpi_callback)(void);
static volatile standard_dpi_callback lcd_mipi_dpi_frame_callback = NULL;
static volatile standard_dpi_callback lcd_mipi_dpi_frame_swap_callback = NULL;

struct bflb_device_s *sim_dpi_dma_handle;

/* DMA linked list of SIM controls data */
static struct bflb_dma_channel_lli_pool_s dma_lli_dpi_header, dma_dpi_lli_tail;

/* DMA linked list of display buffer */
static struct bflb_dma_channel_lli_pool_s dma_lli_dpi_pool[LCD_DPI_SIM_DMA_LLI_CACHE_NUM][DPI_SIM_VALID_PIXEL_MAX / DMA_MAX_COUNT + 1];

/* Invalid row control parameter, Will be sent to SIM before and after the buffer */
static uint32_t sim_dpi_invalid_lin_para[(LCD_DPI_SIM_INVALID_LIN_MAX + 1) * 2];

/* dma lli cache table */
static volatile sim_dpi_dma_lli_cache_t dma_lli_cache_table[LCD_DPI_SIM_DMA_LLI_CACHE_NUM];

/* Total number of dma_transfer */
static volatile uint32_t sim_dpi_dma_lli_num;

/* next display buffer */
static volatile void *next_disp_buffer;

/* record pix_format, width, height */
static volatile uint16_t pix_format, width, height;

static int sim_dpi_dma_lli_init(lcd_mipi_dpi_init_t *dpi_para);
static int sim_dpi_dma_lli_update(void *disp_buff);
static void sim_dpi_dma_interrupt(void *arg);

int lcd_mipi_dpi_init(lcd_mipi_dpi_init_t *dpi_para)
{
    if (dpi_para == NULL) {
        return -1;
    }
    if (dpi_para->frame_buff == NULL) {
        return -1;
    }

    /* sim gpio init */
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");

    /* clk pin */
    bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_CLK, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);

    if (dpi_para->de_mode_en) {
        /* de pin */
        bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_CLK + 1, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    }

#if (LCD_SIM_DPI_DATA_LATCH_MODE)
    /* latch pin */
    bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_CLK + 2, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
#endif

    /* hsync pin */
    bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_HSYNC, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    /* vsync pin */
    bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_HSYNC + 1, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);

#if (LCD_SIM_DPI_DATA_LATCH_MODE)
    /* data pin x8 */
    for (uint8_t i = 0; i < 8; i++) {
        bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_DATA_START + i, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    }
#else
    /* data pin x16 */
    for (uint8_t i = 0; i < 16; i++) {
        bflb_gpio_init(gpio, LCD_DPI_SIM_PIN_DATA_START + i, GPIO_FUNC_SIM | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    }
#endif

    /* sim init */
    struct bflb_sim_dpi_s dpi_sim;
    pix_format = dpi_para->pixel_format;
    width = dpi_para->width;
    height = dpi_para->height;

    if (dpi_para->pixel_format == LCD_MIPI_DPI_PIXEL_FORMAT_RGB565) {
        dpi_sim.data_format = 16;
    } else if (dpi_para->pixel_format == LCD_MIPI_DPI_PIXEL_FORMAT_NRGB888) {
        dpi_sim.data_format = 24;
    } else {
        return -2;
    }

    /* sim init */
    dpi_sim.width = dpi_para->width;
    dpi_sim.height = dpi_para->height;
    dpi_sim.hsw = dpi_para->hsw;
    dpi_sim.hbp = dpi_para->hbp;
    dpi_sim.hfp = dpi_para->hfp;
    dpi_sim.vsw = dpi_para->vsw;
    dpi_sim.vbp = dpi_para->vbp;
    dpi_sim.vfp = dpi_para->vfp;

    dpi_sim.data_latch_mode = LCD_SIM_DPI_DATA_LATCH_MODE;
    dpi_sim.de_sig_enable = 1;
    dpi_sim.de_sig_polarity = LCD_SIM_DPI_DE_SIN_POL;
    dpi_sim.v_sync_sig_polarity = LCD_SIM_DPI_V_SYNC_SIN_POL;
    dpi_sim.h_sync_sig_polarity = LCD_SIM_DPI_H_SYNC_SIN_POL;

    dpi_sim.invalid_row_para = sim_dpi_invalid_lin_para;
    dpi_sim.frame_rate = dpi_para->frame_rate;

    dpi_sim.pin_clk = LCD_DPI_SIM_PIN_CLK;
    dpi_sim.pin_hs = LCD_DPI_SIM_PIN_HSYNC;
    dpi_sim.pin_data = LCD_DPI_SIM_PIN_DATA_START;

    sim_glb_clock_set(1, 0, 0);
    bflb_sim_dpi_init(&dpi_sim);

    /* dma lli init */
    sim_dpi_dma_lli_init(dpi_para);
    sim_dpi_dma_lli_update(dpi_para->frame_buff);

    screen_last = dpi_para->frame_buff;
    next_disp_buffer = screen_last;

    /* init DMA */
    sim_dpi_dma_handle = bflb_device_get_by_name(LCD_SIM_DPI_DMA_NAME);
    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_DPI_USE,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
    bflb_dma_channel_init(sim_dpi_dma_handle, &tx_config);

    /* attach interrupt */
    bflb_dma_channel_irq_attach(sim_dpi_dma_handle, sim_dpi_dma_interrupt, NULL);

    /* update lli to register */
    uint32_t channel_base = sim_dpi_dma_handle->reg_base;
    putreg32(dma_lli_dpi_header.src_addr, channel_base + DMA_CxSRCADDR_OFFSET);
    putreg32(dma_lli_dpi_header.dst_addr, channel_base + DMA_CxDSTADDR_OFFSET);
    putreg32(dma_lli_dpi_header.nextlli, channel_base + DMA_CxLLI_OFFSET);
    putreg32(dma_lli_dpi_header.control.WORD, channel_base + DMA_CxCONTROL_OFFSET);

    /* start dma */
    bflb_dma_channel_start(sim_dpi_dma_handle);

    return 0;
}

int lcd_mipi_dpi_screen_switch(void *screen_buffer)
{
    /* clean cache */
    if (pix_format == LCD_MIPI_DPI_PIXEL_FORMAT_RGB565) {
        bflb_l1c_dcache_clean_range((void *)(screen_buffer), width * height * 2);
    } else if (pix_format == LCD_MIPI_DPI_PIXEL_FORMAT_NRGB888) {
        bflb_l1c_dcache_clean_range((void *)(screen_buffer), width * height * 4);
    }

    next_disp_buffer = screen_buffer;
    return 0;
}

void *lcd_mipi_dpi_get_screen_using(void)
{
    return (void *)screen_last;
}

static void sim_dpi_dma_interrupt(void *arg)
{
    uint8_t swap_flag = 0;

    /* update dma for frame */
    sim_dpi_dma_lli_update((void *)next_disp_buffer);

    /* stop dma channel */
    bflb_dma_channel_stop(sim_dpi_dma_handle);

    /* update lli to register */
    uint32_t channel_base = sim_dpi_dma_handle->reg_base;
    putreg32(dma_lli_dpi_header.src_addr, channel_base + DMA_CxSRCADDR_OFFSET);
    putreg32(dma_lli_dpi_header.dst_addr, channel_base + DMA_CxDSTADDR_OFFSET);
    putreg32(dma_lli_dpi_header.nextlli, channel_base + DMA_CxLLI_OFFSET);
    putreg32(dma_lli_dpi_header.control.WORD, channel_base + DMA_CxCONTROL_OFFSET);

    /* start dma channel */
    bflb_dma_channel_start(sim_dpi_dma_handle);

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

int lcd_mipi_dpi_frame_callback_register(uint32_t callback_type, void (*callback)(void))
{
    if (callback_type == LCD_MIPI_DPI_FRAME_INT_TYPE_SWAP) {
        lcd_mipi_dpi_frame_swap_callback = callback;
    } else if (callback_type == LCD_MIPI_DPI_FRAME_INT_TYPE_CYCLE) {
        lcd_mipi_dpi_frame_callback = callback;
    }
    return 0;
}

static int sim_dpi_dma_lli_init(lcd_mipi_dpi_init_t *dpi_para)
{
    uint16_t i, j;
    uint32_t temp, dma_count;

    /* get total number of dma_transfer */
    if (dpi_para->pixel_format == LCD_MIPI_DPI_PIXEL_FORMAT_RGB565) {
        dma_count = (dpi_para->width * dpi_para->height) * 2 / 4;
    } else if (dpi_para->pixel_format == LCD_MIPI_DPI_PIXEL_FORMAT_NRGB888) {
        dma_count = (dpi_para->width * dpi_para->height);
    } else {
        return -1;
    }

    if (dma_count == 0) {
        return -1;
    }

    /* Initialize DMA and controls data lli */
    dma_lli_dpi_header.control.bits.SBSize = DMA_BURST_INCR4;
    dma_lli_dpi_header.control.bits.dst_min_mode = 0;
    dma_lli_dpi_header.control.bits.DBSize = DMA_BURST_INCR4;
    dma_lli_dpi_header.control.bits.dst_add_mode = 0;
    dma_lli_dpi_header.control.bits.SWidth = DMA_DATA_WIDTH_32BIT;
    dma_lli_dpi_header.control.bits.DWidth = DMA_DATA_WIDTH_32BIT;
    dma_lli_dpi_header.control.bits.fix_cnt = 0;
    dma_lli_dpi_header.control.bits.SI = DMA_ADDR_INCREMENT_ENABLE;
    dma_lli_dpi_header.control.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
    dma_lli_dpi_header.control.bits.I = 0;
    dma_lli_dpi_header.dst_addr = 0x2005A010; // SIM_BASE + SIM_TXF0_OFFSET + SM_ID_USE * 4;
    dma_lli_dpi_header.src_addr = (uint32_t)(uintptr_t)sim_dpi_invalid_lin_para;
    dma_lli_dpi_header.control.bits.TransferSize = (dpi_para->vsw + dpi_para->vbp) * 2 + 1;
    dma_lli_dpi_header.nextlli = (uint32_t)(uintptr_t)NULL;

    dma_dpi_lli_tail = dma_lli_dpi_header;
    dma_dpi_lli_tail.control.bits.I = 1;
    dma_dpi_lli_tail.src_addr = (uint32_t)(uintptr_t)&sim_dpi_invalid_lin_para[(dpi_para->vsw + dpi_para->vbp + 1) * 2];
    dma_dpi_lli_tail.control.bits.TransferSize = dpi_para->vfp * 2;

    bflb_l1c_dcache_clean_range(sim_dpi_invalid_lin_para, sizeof(sim_dpi_invalid_lin_para));

    bflb_l1c_dcache_clean_range((void *)&dma_dpi_lli_tail, sizeof(dma_dpi_lli_tail));

    /* Initialize display buffer data lli, but do not load data */
    dma_lli_dpi_pool[0][0].control.bits.SBSize = DMA_BURST_INCR4;
    dma_lli_dpi_pool[0][0].control.bits.dst_min_mode = 0;
    dma_lli_dpi_pool[0][0].control.bits.DBSize = DMA_BURST_INCR4;
    dma_lli_dpi_pool[0][0].control.bits.dst_add_mode = 0;
    dma_lli_dpi_pool[0][0].control.bits.SWidth = DMA_DATA_WIDTH_32BIT;
    dma_lli_dpi_pool[0][0].control.bits.DWidth = DMA_DATA_WIDTH_32BIT;
    dma_lli_dpi_pool[0][0].control.bits.fix_cnt = 0;
    dma_lli_dpi_pool[0][0].control.bits.SI = DMA_ADDR_INCREMENT_ENABLE;
    dma_lli_dpi_pool[0][0].control.bits.DI = DMA_ADDR_INCREMENT_DISABLE;
    dma_lli_dpi_pool[0][0].control.bits.I = 0;
    dma_lli_dpi_pool[0][0].dst_addr = 0x2005A010; //SIM_BASE + SIM_TXF0_OFFSET + SM_ID_USE * 4;
    dma_lli_dpi_pool[0][0].src_addr = (uint32_t)(uintptr_t)(NULL);
    dma_lli_dpi_pool[0][0].control.bits.TransferSize = DMA_MAX_COUNT;
    dma_lli_dpi_pool[0][0].nextlli = (uint32_t)(uintptr_t)NULL;

    for (i = 0; i < LCD_DPI_SIM_DMA_LLI_CACHE_NUM; i++) {
        temp = dma_count;
        for (j = 0; temp > 0; j++) {
            dma_lli_dpi_pool[i][j] = dma_lli_dpi_pool[0][0];
            dma_lli_dpi_pool[i][j].nextlli = (uint32_t)(uintptr_t)(&dma_lli_dpi_pool[i][j + 1]);
            temp = (temp > DMA_MAX_COUNT) ? (temp - DMA_MAX_COUNT) : (0);
        }

        dma_lli_dpi_pool[i][j - 1].nextlli = (uint32_t)(uintptr_t)(&dma_dpi_lli_tail);
        dma_lli_dpi_pool[i][j - 1].control.bits.TransferSize = dma_count % DMA_MAX_COUNT;

        bflb_l1c_dcache_clean_range((void *)dma_lli_dpi_pool[i], sizeof(struct bflb_dma_channel_lli_pool_s) * (j + 1));
    }
    sim_dpi_dma_lli_num = j;

    for (i = 0; i < LCD_DPI_SIM_DMA_LLI_CACHE_NUM; i++) {
        dma_lli_cache_table[i].disp_buff = NULL;
        dma_lli_cache_table[i].dma_lli = dma_lli_dpi_pool[i];
        dma_lli_cache_table[i].lru_value = LCD_DPI_SIM_DMA_LLI_CACHE_NUM;
    }

    return 0;
}

static int sim_dpi_dma_lli_update(void *disp_buff)
{
    uint16_t idx, idx_oldest = 0, lru_v = 0;

    /* Looking for the cache table */
    for (idx = 0; idx < LCD_DPI_SIM_DMA_LLI_CACHE_NUM; idx++) {
        if (dma_lli_cache_table[idx].disp_buff == disp_buff) {
            lru_v = dma_lli_cache_table[idx].lru_value;
            break;
        } else if (dma_lli_cache_table[idx].lru_value > lru_v) {
            lru_v = dma_lli_cache_table[idx].lru_value;
            idx_oldest = idx;
        }
    }

    /* Not cached, Add to the cache table */
    if (idx >= LCD_DPI_SIM_DMA_LLI_CACHE_NUM) {
        idx = idx_oldest;
        /* update dma lli */
        struct bflb_dma_channel_lli_pool_s *lli_p = dma_lli_cache_table[idx].dma_lli;
        for (uint16_t i = 0; i < sim_dpi_dma_lli_num; i++) {
            lli_p[i].src_addr = (uint32_t)(uintptr_t)(disp_buff) + (i * DMA_MAX_COUNT * 4);
        }
        bflb_l1c_dcache_clean_range((void *)lli_p, sizeof(struct bflb_dma_channel_lli_pool_s) * sim_dpi_dma_lli_num);

        dma_lli_cache_table[idx].disp_buff = disp_buff;
    }

    dma_lli_dpi_header.nextlli = (uint32_t)(uintptr_t)(dma_lli_cache_table[idx].dma_lli);

    /* update cache table lru_value */
    dma_lli_cache_table[idx].lru_value = 0;
    for (idx = 0; idx < LCD_DPI_SIM_DMA_LLI_CACHE_NUM; idx++) {
        if (dma_lli_cache_table[idx].lru_value < lru_v) {
            dma_lli_cache_table[idx].lru_value++;
        }
    }

    return 0;
}

#else
#error "Devices that do not support SIM-DPI! Replace the driver port (lcd_conf.h)"
#endif

#endif