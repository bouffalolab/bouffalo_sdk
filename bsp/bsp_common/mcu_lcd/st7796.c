/**
 * @file st7796_spi.c
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

#include "st7796.h"
#include "mcu_lcd.h"

#include "hal_spi.h"
#include "hal_dma.h"
#include "hal_gpio.h"

#define ST7796_SPI_ID        0
#define ST7796_SPI_INDEX     SPI0_INDEX
#define ST7796_SPI_DMA_INDEX DMA0_CH3_INDEX

static struct device *st7796_spi;
static struct device *dma_spi_tx;

const st7796_spi_init_cmd_t st7796_spi_init_cmds[] = {
    { 0x01, NULL, 0 },
    { 0xFF, NULL, 10 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 120 },

    { 0xF0, "\xC3", 1 },
    { 0xF0, "\x96", 1 },

    { 0x36, "\x48", 1 },

    { 0x3A, "\x05", 1 }, /* RGB565 */
    { 0xE6, "\x0F\xF2\x3F\x4F\x4F\x28\x0E\x00", 8 },
    { 0xC5, "\x2A", 1 },

/* Display Inversion */
#if 1
    { 0xB4, "\x01", 1 },
    { 0x21, NULL, 0 },
#endif

    { 0xE0, "\xF0\x03\x0A\x11\x14\x1C\x3B\x55\x4A\x0A\x13\x14\x1C\x1F", 14 }, /* Set Gamma */
    { 0XE1, "\xF0\x03\x0A\x0C\x0C\x09\x36\x54\x49\x0F\x1B\x18\x1B\x1F", 14 }, /* Set Gamma */

    { 0xF0, "\x3C", 1 },
    { 0xF0, "\x69", 1 },

    { 0x29, NULL, 0 }, /* Display on */
    { 0xFF, NULL, 10 },
};

/**
 * @brief st7796_spi_init
 *
 * @return int  0:succes  1:error
 */
static int st7796_spi_peripheral_init(void)
{
    gpio_set_mode(ST7796_SPI_CS_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ST7796_SPI_DC_PIN, GPIO_OUTPUT_MODE);
    ST7796_SPI_CS_HIGH;
    ST7796_SPI_DC_HIGH;

    st7796_spi = device_find("lcd_dev_ifs");
    if (st7796_spi) {
        device_close(st7796_spi);
    } else {
        spi_register(ST7796_SPI_INDEX, "lcd_dev_ifs");
        st7796_spi = device_find("lcd_dev_ifs");
    }
    if (st7796_spi) {
        SPI_DEV(st7796_spi)->mode = SPI_MASTER_MODE;
        SPI_DEV(st7796_spi)->clk = (36 * 1000 * 1000);
        SPI_DEV(st7796_spi)->direction = SPI_MSB_BYTE0_DIRECTION_FIRST;
        SPI_DEV(st7796_spi)->datasize = SPI_DATASIZE_8BIT;
        SPI_DEV(st7796_spi)->clk_polaraity = SPI_POLARITY_LOW;
        SPI_DEV(st7796_spi)->clk_phase = SPI_PHASE_1EDGE;
        SPI_DEV(st7796_spi)->fifo_threshold = 0;
        device_open(st7796_spi, DEVICE_OFLAG_STREAM_TX);
    } else {
        return 1;
    }

    dma_spi_tx = device_find("lcd_dev_ifs_dma");
    if (dma_spi_tx) {
        device_close(dma_spi_tx);
    } else {
        dma_register(ST7796_SPI_DMA_INDEX, "lcd_dev_ifs_dma");
        dma_spi_tx = device_find("lcd_dev_ifs_dma");
    }
    if (dma_spi_tx) {
        DMA_DEV(dma_spi_tx)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_spi_tx)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_spi_tx)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_spi_tx)->dst_req = DMA_REQUEST_SPI0_TX;
        DMA_DEV(dma_spi_tx)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_spi_tx)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_spi_tx)->src_burst_size = DMA_BURST_INCR1;
        DMA_DEV(dma_spi_tx)->dst_burst_size = DMA_BURST_INCR1;
        DMA_DEV(dma_spi_tx)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_spi_tx)->dst_width = DMA_TRANSFER_WIDTH_8BIT;

        device_open(dma_spi_tx, 0);
        device_set_callback(dma_spi_tx, NULL);
        device_control(st7796_spi, DEVICE_CTRL_ATTACH_TX_DMA, dma_spi_tx);
        device_control(dma_spi_tx, DEVICE_CTRL_CLR_INT, NULL);
    } else {
        return 1;
    }

    device_control(st7796_spi, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
    return 0;
}

/**
 * @brief st7796_spi_write_cmd
 *
 * @param cmd
 * @return int 0:succes  1:error
 */
static int st7796_spi_write_cmd(uint8_t cmd)
{
    ST7796_SPI_DC_LOW;
    ST7796_SPI_CS_LOW;
    int res = spi_transmit(st7796_spi, &cmd, 1, SPI_TRANSFER_TYPE_8BIT);
    ST7796_SPI_CS_HIGH;
    ST7796_SPI_DC_HIGH;
    return res;
}

/**
 * @brief st7796_spi_write_data_byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int st7796_spi_write_data_byte(uint8_t data)
{
    ST7796_SPI_CS_LOW;
    int res = spi_transmit(st7796_spi, &data, 1, SPI_TRANSFER_TYPE_8BIT);
    ST7796_SPI_CS_HIGH;
    return res;
}

/**
 * @brief st7796_spi_draw_is_busy, After the call st7796_spi_draw_picture_dma must check this,
 *         if st7796_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7796_spi_draw_is_busy(void)
{
    if (dma_channel_check_busy(SPI_DEV(st7796_spi)->tx_dma)) {
        return 1;
    } else {
        /* Wait for tx FIFO to be empty */
        while (device_control(st7796_spi, DEVICE_CTRL_SPI_GET_TX_FIFO, NULL) < SPI_FIFO_LEN)
            ;
        /* Wait for the SPI bus to be idle */
        while (device_control(st7796_spi, DEVICE_CTRL_SPI_GET_BUS_BUSY_STATUS, NULL) != 0)
            ;
        /* Switch the SPI to non-DMA mode */
        device_control(st7796_spi, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
        ST7796_SPI_CS_HIGH;

        return 0;
    }
}

/**
 * @brief st7796_spi_init
 *
 * @return int
 */
int st7796_spi_init()
{
    int res = st7796_spi_peripheral_init();
    if (res) {
        return res;
    }

    for (uint16_t i = 0; i < (sizeof(st7796_spi_init_cmds) / sizeof(st7796_spi_init_cmd_t)); i++) {
        if (st7796_spi_init_cmds[i].cmd == 0xFF) {
            bflb_platform_delay_ms(st7796_spi_init_cmds[i].databytes);
        } else {
            /* send register address */
            res |= st7796_spi_write_cmd(st7796_spi_init_cmds[i].cmd);

            /* send register data */
            for (uint8_t j = 0; j < (st7796_spi_init_cmds[i].databytes & 0x7F); j++) {
                res |= st7796_spi_write_data_byte(st7796_spi_init_cmds[i].data[j]);
            }

            if (res) {
                return res;
            }
        }
    }
    st7796_spi_set_draw_window(0, 0, ST7796_SPI_H, ST7796_SPI_W);
    return res;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int st7796_spi_set_dir(uint8_t dir)
{
    st7796_spi_write_cmd(0x36);
    switch (dir) {
        case 0:
            st7796_spi_write_data_byte(0x08);
            break;
        case 1:
            st7796_spi_write_data_byte(0x28);
            break;
        case 2:
            st7796_spi_write_data_byte(0x88);
            break;
        case 3:
            st7796_spi_write_data_byte(0xE8);
            break;
        default:
            return -1;
            break;
    }
    return dir;
}

/**
 * @brief st7796_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st7796_spi_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
#if ST7796_SPI_OFFSET_X
    x1 += ST7796_SPI_OFFSET_X;
    x2 += ST7796_SPI_OFFSET_X;
#endif
#if ST7796_SPI_OFFSET_Y
    y1 += ST7796_SPI_OFFSET_Y;
    y2 += ST7796_SPI_OFFSET_Y;
#endif

    st7796_spi_write_cmd(0x2a);
    st7796_spi_write_data_byte(x1 >> 8);
    st7796_spi_write_data_byte(x1);
    st7796_spi_write_data_byte(x2 >> 8);
    st7796_spi_write_data_byte(x2);

    st7796_spi_write_cmd(0x2b);
    st7796_spi_write_data_byte(y1 >> 8);
    st7796_spi_write_data_byte(y1);
    st7796_spi_write_data_byte(y2 >> 8);
    st7796_spi_write_data_byte(y2);

    st7796_spi_write_cmd(0x2c);
}

/**
 * @brief st7796_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7796_spi_draw_point(uint16_t x, uint16_t y, st7796_spi_color_t color)
{
    if (lcd_auto_swap_flag) {
        color = ((color >> 8) & 0xFF) | color << 8;
    }

    st7796_spi_set_draw_window(x, y, x, y);
    ST7796_SPI_CS_LOW;
    device_write(st7796_spi, 0, &color, 2);
    ST7796_SPI_CS_HIGH;
}

/**
 * @brief st7796_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7796_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7796_spi_color_t color)
{
    if (lcd_auto_swap_flag) {
        color = ((color >> 8) & 0xFF) | color << 8;
    }

    st7796_spi_set_draw_window(x1, y1, x2, y2);
    ST7796_SPI_CS_LOW;
    for (uint16_t i = y1; i <= y2; i++) {
        for (uint16_t j = x1; j <= x2; j++)
            spi_transmit(st7796_spi, &color, 1, SPI_TRANSFER_TYPE_16BIT);
    }
    ST7796_SPI_CS_HIGH;
}

/**
 * @brief st7796_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7796_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7796_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7796_spi_color_t *picture)
{
    size_t picture_size = (x2 - x1 + 1) * (y2 - y1 + 1);

    st7796_spi_set_draw_window(x1, y1, x2, y2);

    if (lcd_auto_swap_flag) {
        lcd_swap_color_data16(picture, picture, picture_size);
    }

    ST7796_SPI_CS_LOW;
    device_control(st7796_spi, DEVICE_CTRL_TX_DMA_RESUME, NULL);
    device_write(st7796_spi, 0, picture, (picture_size << 1));
}

/**
 * @brief st7796_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7796_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7796_spi_color_t *picture)
{
    st7796_spi_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    while (st7796_spi_draw_is_busy()) {
        BL_DRV_DUMMY;
        BL_DRV_DUMMY;
    };
}
