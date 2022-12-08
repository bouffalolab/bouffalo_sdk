/**
 * @file ili9341_spi.c
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

#if defined(LCD_SPI_ILI9341)

#include "bflb_mtimer.h"
#include "bflb_spi.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "ili9341_spi.h"

#if (defined(BL808) || defined(BL606P)) && defined(CPU_D0)
#define ILI9341_SPI_ID       1
#define ILI9341_SPI_NAME     "spi1"
#define ILI9341_SPI_DMA_NAME "dma2_ch0"
#else
#define ILI9341_SPI_ID       0
#define ILI9341_SPI_NAME     "spi0"
#define ILI9341_SPI_DMA_NAME "dma0_ch3"
#endif

#define ILI9341_DMA_LLI_NUM (ILI9341_SPI_W * ILI9341_SPI_H / 4064 + 1)

/* asynchronous flush interrupt callback */
typedef void (*ili9341_spi_callback)(void);
static volatile ili9341_spi_callback ili9341_spi_async_callback = NULL;
static volatile bool ili9341_spi_sync_flush_flag = false;

static struct bflb_device_s *ili9341_gpio;
static struct bflb_device_s *ili9341_spi;
static struct bflb_device_s *ili9341_dma_spi_tx;

/* The memory space of DMA */
static struct bflb_dma_channel_lli_pool_s dam_tx_llipool[ILI9341_DMA_LLI_NUM];
static struct bflb_dma_channel_lli_transfer_s dma_tx_transfers[1];

const ili9341_spi_init_cmd_t ili9341_spi_init_cmds[] = {
    { 0x01, NULL, 0 },
    { 0xFF, NULL, 20 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 120 },

    { 0xCF, "\x00\xd9\x30", 3 },
    { 0xED, "\x64\x03\x12\x81", 4 },
    { 0xE8, "\x85\x01\x78", 3 },
    { 0xCB, "\x39\x2C\x00\x34\x02", 5 },
    { 0xF7, "\x20", 1 },
    { 0xEA, "\x00\x00", 2 },

    { 0xC0, "\x23", 1 }, /*Power control*/
    { 0xC1, "\x12", 1 }, /*Power control */
    { 0xC2, "\x11", 1 },
    { 0xC5, "\x40\x30", 2 }, /*VCOM control 1*/
    { 0xC7, "\xa9", 1 },     /*VCOM control 2*/
    { 0x36, "\x08", 1 },     /*Memory Access Control*/
    { 0x3A, "\x55", 1 },     /*Pixel Format Set*/
    { 0xB1, "\x00\x18", 2 }, /* Frame Rate Control */
    { 0xB6, "\x0a\xa2", 2 }, /* Display Function Control */
    { 0x0C, "\xd5", 1 },     /* display pixel format,RGB 16bits,MCU 16bits  */
    { 0xF2, "\x00", 1 },     /* 3Gamma Function Disable */
    { 0xF7, "\x20", 1 },

    { 0x26, "\x01", 1 },                                                          /* Gamma curve selected */
    { 0xE0, "\x1F\x1A\x18\x0A\x0F\x06\x45\x87\x32\x0A\x07\x02\x07\x05\x00", 15 }, /* Set Gamma */
    { 0XE1, "\x00\x25\x27\x05\x10\x09\x3A\x78\x4D\x05\x18\x0D\x38\x3A\x1F", 15 }, /* Set Gamma */
    { 0xB7, "\x07", 1 },

    { 0x29, NULL, 0 }, /* Display on */
    { 0xFF, NULL, 20 },
};

/**
 * @brief ili9341_spi_dma_flush_callback
 *
 * @return
 */
void ili9341_spi_dma_flush_callback(void *arg)
{
    if (ili9341_spi_sync_flush_flag == true) {
        ili9341_spi_sync_flush_flag = false;
        return;
    }

    while (ili9341_spi_draw_is_busy()) {
        __ASM volatile("nop");
    };

    if (ili9341_spi_async_callback != NULL) {
        ili9341_spi_async_callback();
    }
}

/**
 * @brief ili9341_spi_async_callback_register
 *
 * @return
 */
void ili9341_spi_async_callback_register(void (*callback)(void))
{
    ili9341_spi_async_callback = callback;
}

/**
 * @brief ili9341_spi_init
 *
 * @return int  0:succes  1:error
 */
static int ili9341_spi_peripheral_init(void)
{
    /* spi */
    struct bflb_spi_config_s spi_cfg = {
        .freq = 40 * 1000 * 1000,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE3,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    /* dma */
    struct bflb_dma_channel_config_s dma_spi_tx_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };

    /* CS and DC pin init */
    ili9341_gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(ili9341_gpio, ILI9341_SPI_CS_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(ili9341_gpio, ILI9341_SPI_DC_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    ILI9341_SPI_CS_HIGH;
    ILI9341_SPI_DC_HIGH;

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT) && (SPI_FIFO_BYTE_NUM_MAX > (2 * 4))
    /* SPI support burst*4 */
    spi_cfg.tx_fifo_threshold = 2 * 4 - 1;
    spi_cfg.rx_fifo_threshold = 2 * 4 - 1;
#elif (!SPI_FIFO_WIDTH_VARIABLE_SUPPORT) && (SPI_FIFO_WORD_NUM_MAX > 4)
    /* SPI support burst*4 */
    spi_cfg.tx_fifo_threshold = 4 - 1;
    spi_cfg.rx_fifo_threshold = 4 - 1;
#endif

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT)
    spi_cfg.byte_order = SPI_BYTE_MSB;
#endif

    /* spi init */
    ili9341_spi = bflb_device_get_by_name(ILI9341_SPI_NAME);
    bflb_spi_init(ili9341_spi, &spi_cfg);

    /* spi enabled continuous mode */
    bflb_spi_feature_control(ili9341_spi, SPI_CMD_SET_CS_INTERVAL, true);

#if (ILI9341_SPI_ID == 0)
    dma_spi_tx_cfg.dst_req = DMA_REQUEST_SPI0_TX;
#elif (ILI9341_SPI_ID == 1)
    dma_spi_tx_cfg.dst_req = DMA_REQUEST_SPI1_TX;
#endif

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT) && (SPI_FIFO_BYTE_NUM_MAX > (2 * 4))
    /* DMA support burst*4 */
    dma_spi_tx_cfg.src_burst_count = DMA_BURST_INCR4;
    dma_spi_tx_cfg.dst_burst_count = DMA_BURST_INCR4;
#elif (!SPI_FIFO_WIDTH_VARIABLE_SUPPORT) && (SPI_FIFO_WORD_NUM_MAX > 4)
    /* DMA support burst*4 */
    dma_spi_tx_cfg.src_burst_count = DMA_BURST_INCR4;
    dma_spi_tx_cfg.dst_burst_count = DMA_BURST_INCR4;
#endif

    /* dma init */
    ili9341_dma_spi_tx = bflb_device_get_by_name(ILI9341_SPI_DMA_NAME);
    bflb_dma_channel_init(ili9341_dma_spi_tx, &dma_spi_tx_cfg);

    /* dma int cfg */
    bflb_dma_channel_irq_attach(ili9341_dma_spi_tx, ili9341_spi_dma_flush_callback, NULL);

    return 0;
}

/**
 * @brief ili9341_spi_write_cmd
 *
 * @param cmd
 * @return int 0:succes  1:error
 */
static int ili9341_spi_write_cmd(uint8_t cmd)
{
    ILI9341_SPI_DC_LOW;
    ILI9341_SPI_CS_LOW;
    bflb_spi_poll_send(ili9341_spi, cmd);
    ILI9341_SPI_CS_HIGH;
    ILI9341_SPI_DC_HIGH;
    return 0;
}

/**
 * @brief ili9341_spi_write_data_byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int ili9341_spi_write_data_byte(uint8_t data)
{
    ILI9341_SPI_CS_LOW;
    bflb_spi_poll_send(ili9341_spi, data);
    ILI9341_SPI_CS_HIGH;
    return 0;
}

/**
 * @brief ili9341_spi_draw_is_busy, After the call ili9341_spi_draw_picture_dma must check this,
 *         if ili9341_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int ili9341_spi_draw_is_busy(void)
{
    if (bflb_dma_channel_isbusy(ili9341_dma_spi_tx)) {
        return 1;
    } else {
        /* Wait for the SPI bus to be idle */
        while (bflb_spi_isbusy(ili9341_spi)) {
            __ASM volatile("nop");
        };
        /* Switch the SPI to non-DMA mode */
        bflb_spi_link_txdma(ili9341_spi, false);
        /* clear rx fifo */
        bflb_spi_feature_control(ili9341_spi, SPI_CMD_CLEAR_RX_FIFO, 0);
        /*  */
        ILI9341_SPI_CS_HIGH;
        /* 8-bit data */
        bflb_spi_feature_control(ili9341_spi, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
        return 0;
    }
}

/**
 * @brief ili9341_spi_init
 *
 * @return int
 */
int ili9341_spi_init()
{
    int res = ili9341_spi_peripheral_init();
    if (res < 0) {
        return res;
    }

    for (uint16_t i = 0; i < (sizeof(ili9341_spi_init_cmds) / sizeof(ili9341_spi_init_cmd_t)); i++) {
        if (ili9341_spi_init_cmds[i].cmd == 0xFF && ili9341_spi_init_cmds[i].data == NULL) {
            bflb_mtimer_delay_ms(ili9341_spi_init_cmds[i].databytes);
        } else {
            /* send register address */
            ili9341_spi_write_cmd(ili9341_spi_init_cmds[i].cmd);

            /* send register data */
            for (uint8_t j = 0; j < (ili9341_spi_init_cmds[i].databytes); j++) {
                ili9341_spi_write_data_byte(ili9341_spi_init_cmds[i].data[j]);
            }
        }
    }
    ili9341_spi_set_draw_window(0, 0, ILI9341_SPI_H, ILI9341_SPI_W);
    return res;
}

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)

/**
 * @brief lcd_swap_color_data16
 *
 * @param dst destination
 * @param src source
 * @param color_num color num
 * @return int
 */
static int lcd_swap_color_data16(uint16_t *dst, uint16_t *src, uint32_t color_num)
{
    for (size_t i = 0; i < color_num; i++) {
        dst[i] = (src[i] << 8) | (src[i] >> 8);
    }
    return 0;
}

#endif

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int ili9341_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;

    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x08;
            else
                param = 0x48;
            break;
        case 1:
            if (!mir_flag)
                param = 0xA8;
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
                param = 0x68;
            else
                param = 0xE8;

            break;
        default:
            return -1;
            break;
    }

    ili9341_spi_write_cmd(0x36);
    ili9341_spi_write_data_byte(param);

    return dir;
}

/**
 * @brief ili9341_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void ili9341_spi_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
#if ILI9341_SPI_OFFSET_X
    x1 += ILI9341_SPI_OFFSET_X;
    x2 += ILI9341_SPI_OFFSET_X;
#endif
#if ILI9341_SPI_OFFSET_Y
    y1 += ILI9341_SPI_OFFSET_Y;
    y2 += ILI9341_SPI_OFFSET_Y;
#endif

    ili9341_spi_write_cmd(0x2a);
    ili9341_spi_write_data_byte(x1 >> 8);
    ili9341_spi_write_data_byte(x1);
    ili9341_spi_write_data_byte(x2 >> 8);
    ili9341_spi_write_data_byte(x2);

    ili9341_spi_write_cmd(0x2b);
    ili9341_spi_write_data_byte(y1 >> 8);
    ili9341_spi_write_data_byte(y1);
    ili9341_spi_write_data_byte(y2 >> 8);
    ili9341_spi_write_data_byte(y2);

    ili9341_spi_write_cmd(0x2c);
}

/**
 * @brief ili9341_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void ili9341_spi_draw_point(uint16_t x, uint16_t y, ili9341_spi_color_t color)
{
#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
    color = ((color >> 8) & 0xFF) | color << 8;
#endif

    ili9341_spi_set_draw_window(x, y, x, y);
    ILI9341_SPI_DC_HIGH;
    ILI9341_SPI_CS_LOW;
    bflb_spi_poll_send(ili9341_spi, color & 0x00ff);
    bflb_spi_poll_send(ili9341_spi, (color >> 8) & 0x00ff);
    ILI9341_SPI_CS_HIGH;
}

/**
 * @brief ili9341_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9341_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t color)
{
    uint32_t pixelDataSize, PixelCount = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t __attribute__((aligned(64))) color_src;

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT)
    color_src = color;
#else
    color_src = ((color >> 8) & 0xFF) | color << 8;
#endif

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)&color_src, sizeof(color_src));

    /* set window */
    ili9341_spi_set_draw_window(x1, y1, x2, y2);

    /* get pixel Data Size */
    pixelDataSize = PixelCount * 2;

    /* spi 16-bit mode */
    bflb_spi_feature_control(ili9341_spi, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);

    /* enable spi dma mode */
    bflb_spi_link_txdma(ili9341_spi, true);

    ILI9341_SPI_CS_LOW;

    /* sync mode flag, temporary shutdown interrupt */
    ili9341_spi_sync_flush_flag = true;

    /* disable dma src_addr_inc */
    bflb_dma_feature_control(ili9341_dma_spi_tx, DMA_CMD_SET_SRCADDR_INCREMENT, false);

    /* cfg and start dma */
    dma_tx_transfers[0].src_addr = (uint32_t)(uintptr_t)&color_src;
#if (defined(BL808) || defined(BL606P)) && defined(CPU_D0)
    dma_tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI1_TDR;
#else
    dma_tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
#endif
    dma_tx_transfers[0].nbytes = pixelDataSize;

    bflb_dma_channel_lli_reload(ili9341_dma_spi_tx, dam_tx_llipool, ILI9341_DMA_LLI_NUM, dma_tx_transfers, 1);
    bflb_dma_channel_start(ili9341_dma_spi_tx);

    /* Wait to finish, and cs high */
    while (ili9341_spi_draw_is_busy()) {
        __ASM volatile("nop");
    };

    /* enable src_addr_inc */
    bflb_dma_feature_control(ili9341_dma_spi_tx, DMA_CMD_SET_SRCADDR_INCREMENT, true);
}

/**
 * @brief ili9341_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (ili9341_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9341_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture)
{
    size_t picture_size = (x2 - x1 + 1) * (y2 - y1 + 1);

    ili9341_spi_set_draw_window(x1, y1, x2, y2);

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
    lcd_swap_color_data16(picture, picture, picture_size);
#endif

    /* clean dcache */
    bflb_l1c_dcache_clean_range((void *)(picture), (picture_size * 2));

    /* spi 16-bit data mode */
    bflb_spi_feature_control(ili9341_spi, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);

    /* spi dma mode enable */
    bflb_spi_link_txdma(ili9341_spi, true);

    ILI9341_SPI_CS_LOW;

    /* dma cfg and start */
    dma_tx_transfers[0].src_addr = (uint32_t)(uintptr_t)picture;
    dma_tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_SPI0_TDR;
    dma_tx_transfers[0].nbytes = picture_size * 2;
    bflb_dma_channel_lli_reload(ili9341_dma_spi_tx, dam_tx_llipool, sizeof(dam_tx_llipool) / sizeof(dam_tx_llipool[0]), dma_tx_transfers, 1);
    bflb_dma_channel_start(ili9341_dma_spi_tx);
}

/**
 * @brief ili9341_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9341_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, ili9341_spi_color_t *picture)
{
    /* sync mode, temporary shutdown interrupt */
    ili9341_spi_sync_flush_flag = true;

    ili9341_spi_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    while (ili9341_spi_draw_is_busy()) {
        __ASM volatile("nop");
    };
}

#endif
