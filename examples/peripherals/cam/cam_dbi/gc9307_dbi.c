/**
 * @file gc9307_dbi.c
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

#include "gc9307_dbi.h"
#include "bflb_dbi.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"

lcd_dbi_init_t dbi_para = {
    .clock_freq = 40 * 1000 * 1000,
#if (GC9307_DBI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_RGB565,
#elif (GC9307_DBI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

const gc9307_dbi_init_cmd_t gc9307_dbi_init_cmds[] = {
    { 0xFE, NULL, 0 },
    { 0xEF, NULL, 0 },

    { 0x36, "\x48", 1 },
#if (GC9307_DBI_PIXEL_FORMAT == 1)
    { 0x3A, "\x55", 1 }, /* Interface Pixel Format RGB565 */
#elif (GC9307_DBI_PIXEL_FORMAT == 2)
    { 0x3A, "\x66", 1 }, /* Interface Pixel Format RGB666 */
#endif
    { 0x86, "\x98", 1 },
    { 0x89, "\x03", 1 },
    { 0x8B, "\x80", 1 },
    { 0x8D, "\x22", 1 },
    { 0x8E, "\x0F", 1 },
    { 0xE8, "\x12\x00", 2 },
    { 0xC3, "\x47", 1 },
    { 0xC4, "\x28", 1 },
    { 0xC9, "\x08", 1 },
    { 0xFF, "\x62", 1 },
    { 0x99, "\x3E", 1 },
    { 0x9D, "\x4B", 1 },

    { 0xF0, "\x07\x0B\x0C\x0A\x06\x31", 6 },
    { 0xF2, "\x07\x07\x04\x06\x06\x21", 6 },
    { 0xF1, "\x4A\x78\x76\x33\x2F\xAF", 6 },
    { 0xF3, "\x38\x74\x72\x22\x28\x6F", 6 },

    { 0x35, "\x00", 1 },
    { 0x44, "\x00\x0A", 2 },

    { 0x11, NULL, 0 },
    { 0xFF, NULL, 120 },
    { 0x29, NULL, 0 },
    { 0xFF, NULL, 100 },
};

#define LCD_DBI_DMA_LLI_NUM (DBI_DBI_DATA_SIZE_MAX / 4 / 4064 + 1)

/* asynchronous flush interrupt callback */
typedef void (*dbi_callback)(void);
static volatile dbi_callback dbi_async_callback = NULL;
static volatile bool dbi_async_callback_en_flag = true;

/* dma device */
static struct bflb_device_s *dbi_dma_hd;
/* pec dbi typeB device */
static struct bflb_device_s *dbi_hd;

/* The memory space of DMA */
static struct bflb_dma_channel_lli_pool_s dma_tx_llipool[LCD_DBI_DMA_LLI_NUM];
static struct bflb_dma_channel_lli_transfer_s dma_tx_transfers[1];

static volatile bool dbi_async_flag = false;
static volatile bool dbi_async_cycle_fill_flag = false;

static void dbi_tc_callback(int irq, void *arg)
{
    /* disable dbi dma */
    bflb_dbi_link_txdma(dbi_hd, false);

    /* clear flag and mask int */
    bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);
    bflb_dbi_tcint_mask(dbi_hd, true);

    if (dbi_async_flag == false && dbi_async_cycle_fill_flag == false) {
        return;
    }

    if (dbi_async_cycle_fill_flag == true) {
        /* enable dma addr increment */
        bflb_dma_feature_control(dbi_dma_hd, DMA_CMD_SET_SRCADDR_INCREMENT, true);
    }

    /* disable DMA request for dbi */
    bflb_dbi_link_txdma(dbi_hd, false);

    dbi_async_flag = false;
    dbi_async_cycle_fill_flag = false;

    /* async callback */
    if (dbi_async_callback_en_flag && dbi_async_callback != NULL) {
        dbi_async_callback();
    }
}

int lcd_dbi_async_callback_register(void (*callback)(void))
{
    dbi_async_callback = callback;
    return 0;
}

int lcd_dbi_async_callback_enable(bool enable)
{
    dbi_async_callback_en_flag = enable;
    return 0;
}

int lcd_dbi_init(lcd_dbi_init_t *dbi_parra)
{
    struct bflb_device_s *gpio;

    /* dbi cfg */
    struct bflb_dbi_config_s dbi_cfg = {
        .clk_mode = DBI_CLOCK_MODE_1,
        .clk_freq_hz = dbi_parra->clock_freq,
        .tx_fifo_threshold = 3,
    };

    /* dma cfg */
    struct bflb_dma_channel_config_s dma_dbi_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_DBI_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    /* dbi typeB x8 mode */
    dbi_cfg.dbi_mode = DBI_MODE_TYPE_B;
    uint32_t dbi_gpio_func = GPIO_FUNC_DBI_B;
    uint8_t dbi_gpio_list[] = { LCD_DBI_TYPEB_PIN_WR, LCD_DBI_TYPEB_PIN_DC,
                                LCD_DBI_TYPEB_PIN_DAT0, LCD_DBI_TYPEB_PIN_DAT1, LCD_DBI_TYPEB_PIN_DAT2, LCD_DBI_TYPEB_PIN_DAT3,
                                LCD_DBI_TYPEB_PIN_DAT4, LCD_DBI_TYPEB_PIN_DAT5, LCD_DBI_TYPEB_PIN_DAT6, LCD_DBI_TYPEB_PIN_DAT7 };

    if (dbi_parra->pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        dbi_cfg.pixel_input_format = DBI_PIXEL_INPUT_FORMAT_NRGB_8888;
        dbi_cfg.pixel_output_format = DBI_PIXEL_OUTPUT_FORMAT_RGB_565;
    } else if (dbi_parra->pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        dbi_cfg.pixel_input_format = DBI_PIXEL_INPUT_FORMAT_NRGB_8888;
        dbi_cfg.pixel_output_format = DBI_PIXEL_OUTPUT_FORMAT_RGB_888;
    }

    /* gpio init */
    gpio = bflb_device_get_by_name("gpio");
    for (uint8_t i = 0; i < sizeof(dbi_gpio_list) / sizeof(dbi_gpio_list[0]); i++) {
        bflb_gpio_init(gpio, dbi_gpio_list[i], dbi_gpio_func | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    }
    bflb_gpio_init(gpio, LCD_DBI_TYPEB_PIN_CS, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, LCD_DBI_TYPEB_PIN_CS);
    bflb_gpio_init(gpio, LCD_DBI_TYPEB_PIN_RST, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, LCD_DBI_TYPEB_PIN_RST);
    bflb_mtimer_delay_ms(10);
    bflb_gpio_set(gpio, LCD_DBI_TYPEB_PIN_RST);

    /* dbi init */
    dbi_hd = bflb_device_get_by_name("dbi");
    bflb_dbi_init(dbi_hd, &dbi_cfg);
    /* cs continuous mode */
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_CS_CONTINUE, true);

    /* dbi tx intertupt init */
    bflb_dbi_tcint_mask(dbi_hd, true);
    bflb_irq_attach(dbi_hd->irq_num, dbi_tc_callback, NULL);
    bflb_irq_enable(dbi_hd->irq_num);

    /* dma init */
    dbi_dma_hd = bflb_device_get_by_name(LCD_DBI_DMA_NAME);
    bflb_dma_channel_init(dbi_dma_hd, &dma_dbi_config);

    return 0;
}

int lcd_dbi_is_busy(void)
{
    if (dbi_async_flag || dbi_async_cycle_fill_flag) {
        return 1;
    } else {
        return 0;
    }
}

int lcd_dbi_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num)
{
    bflb_dbi_send_cmd_data(dbi_hd, cmd, para_num, (void *)para);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, (void *)pixel);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
    uint32_t data_size;

    dbi_async_flag = true;

    /* get data size */
    data_size = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, pixel_num);

    /* dma transfers cfg */
    dma_tx_transfers[0].src_addr = (uint32_t)pixel;
    dma_tx_transfers[0].dst_addr = DMA_ADDR_DBI_TDR;
    dma_tx_transfers[0].nbytes = data_size;
    bflb_dma_channel_lli_reload(dbi_dma_hd, dma_tx_llipool, LCD_DBI_DMA_LLI_NUM, dma_tx_transfers, 1);

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)pixel, data_size);

    /* trigger dbi data transfer */
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, NULL);

    /* unmask int */
    bflb_dbi_tcint_mask(dbi_hd, false);

    /* enabled DMA request for dbi */
    bflb_dbi_link_txdma(dbi_hd, true);

    /* enable dma */
    bflb_dma_channel_start(dbi_dma_hd);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
    uint32_t words_cnt;
    uint8_t fifo_cnt;

    /* get data size */
    words_cnt = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, pixel_num) / 4;

    /* trigger dbi data transfer */
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, NULL);

    /* fill data into fifo */
    for (; words_cnt > 0;) {
        fifo_cnt = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_TX_FIFO_CNT, 0);
        fifo_cnt = words_cnt > fifo_cnt ? fifo_cnt : words_cnt;
        words_cnt -= fifo_cnt;

        for (; fifo_cnt > 0; fifo_cnt--) {
            putreg32(pixel_val, DMA_ADDR_DBI_TDR);
        }
    }

    /* wait transfer complete */
    while ((bflb_dbi_get_intstatus(dbi_hd) & DBI_INTSTS_TC) == 0) {
    };

    /* clear end flag */
    bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
    uint32_t data_size;

    dbi_async_cycle_fill_flag = true;

    /* get data size */
    data_size = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, pixel_num);

    /* dma cycle fill mode */
    bflb_dma_feature_control(dbi_dma_hd, DMA_CMD_SET_SRCADDR_INCREMENT, false);

    /* dma transfers cfg */
    dma_tx_transfers[0].src_addr = (uint32_t)&pixel_val;
    dma_tx_transfers[0].dst_addr = DMA_ADDR_DBI_TDR;
    dma_tx_transfers[0].nbytes = data_size;
    bflb_dma_channel_lli_reload(dbi_dma_hd, dma_tx_llipool, LCD_DBI_DMA_LLI_NUM, dma_tx_transfers, 1);

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)&pixel_val, sizeof(pixel_val));

    /* trigger dbi data transfer */
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, NULL);

    /* unmask int */
    bflb_dbi_tcint_mask(dbi_hd, false);

    /* enabled DMA request for dbi */
    bflb_dbi_link_txdma(dbi_hd, true);

    /* enable dma */
    bflb_dma_channel_start(dbi_dma_hd);

    return 0;
}

/**
 * @brief gc9307_dbi_async_callback_enable
 *
 * @return
 */
void gc9307_dbi_async_callback_enable(bool enable)
{
    lcd_dbi_async_callback_enable(enable);
}

/**
 * @brief gc9307_dbi_async_callback_register
 *
 * @return
 */
void gc9307_dbi_async_callback_register(void (*callback)(void))
{
    lcd_dbi_async_callback_register(callback);
}

/**
 * @brief gc9307_dbi_draw_is_busy, After the call gc9307_dbi_draw_picture_dma must check this,
 *         if gc9307_dbi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int gc9307_dbi_draw_is_busy(void)
{
    return lcd_dbi_is_busy();
}

/**
 * @brief gc9307_dbi_init
 *
 * @return int
 */
int gc9307_dbi_init()
{
    lcd_dbi_init(&dbi_para);

    for (uint16_t i = 0; i < (sizeof(gc9307_dbi_init_cmds) / sizeof(gc9307_dbi_init_cmd_t)); i++) {
        if (gc9307_dbi_init_cmds[i].cmd == 0xFF && gc9307_dbi_init_cmds[i].data == NULL && gc9307_dbi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(gc9307_dbi_init_cmds[i].databytes);
        } else {
            lcd_dbi_transmit_cmd_para(gc9307_dbi_init_cmds[i].cmd, (void *)(gc9307_dbi_init_cmds[i].data), gc9307_dbi_init_cmds[i].databytes);
        }
    }

    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int gc9307_dbi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;
    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x08;
            else
                param = 0x09;
            break;
        case 1:
            if (!mir_flag)
                param = 0x68;
            else
                param = 0x28;
            break;
        case 2:
            if (!mir_flag)
                param = 0xC8;
            else
                param = 0x88;
            break;
        case 3:
            if (!mir_flag)
                param = 0xA8;
            else
                param = 0xE8;

            break;
        default:
            return -1;
            break;
    }
    lcd_dbi_transmit_cmd_para(0x36, (void *)&param, 1);
    return dir;
}

/**
 * @brief gc9307_dbi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void gc9307_dbi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if GC9307_DBI_OFFSET_X
    x1 += GC9307_DBI_OFFSET_X;
    x2 += GC9307_DBI_OFFSET_X;
#endif
#if GC9307_DBI_OFFSET_Y
    y1 += GC9307_DBI_OFFSET_Y;
    y2 += GC9307_DBI_OFFSET_Y;
#endif

    uint8_t param[4];

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
    lcd_dbi_transmit_cmd_para(0x2B, (void *)param, 4);

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
    lcd_dbi_transmit_cmd_para(0x2A, (void *)param, 4);
}

/**
 * @brief gc9307_dbi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void gc9307_dbi_draw_point(uint16_t x, uint16_t y, gc9307_dbi_color_t color)
{
    /* set window */
    gc9307_dbi_set_draw_window(x, y, x, y);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief gc9307_dbi_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void gc9307_dbi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t color)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t color_src;

#if GC9307_DBI_COLOR_DEPTH == 16
    color_src = color << 16 | color;
#elif GC9307_DBI_COLOR_DEPTH == 32
    color_src = color;
#endif

    /* set window */
    gc9307_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_fill_sync(0x2C, color_src, pixel_num);
}

/**
 * @brief gc9307_dbi_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (gc9307_dbi_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */

void gc9307_dbi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    gc9307_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_num);
}

/**
 * @brief gc9307_dbi_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void gc9307_dbi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, gc9307_dbi_color_t *picture)
{
    uint32_t pixel_num = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    gc9307_dbi_set_draw_window(x1, y1, x2, y2);

    lcd_dbi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_num);
}

