/**
 * @file main.c
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
#include "hal_spi.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "hal_dma.h"

#define SPI0_CS           GPIO_PIN_10
#define ES8374_SLAVE_ADDR 0x10

//播放控制块
typedef struct audio_dev {
    uint8_t *buff1;
    uint8_t *buff2;
    uint32_t buff1_data_size; //buff1内数据长度
    uint32_t buff2_data_size; //buff1内数据长度
    uint32_t buff_size_max;   //buff的大小
    uint8_t buff_using;       //正在使用的buff

    uint8_t audio_state; //状态
    uint8_t audio_type;  //类型

    int (*audio_init)(struct audio_dev *audio_dev); //初始化函数
    int (*audio_control)(struct audio_dev *audio_dev, int cmd, void *args);

    int (*audio_callback)(struct audio_dev *audio_dev); //中断回调函数，用来重新装载buff，

    struct device *device; //i2s的device，可以考虑换成指向其他外设
    //wav_information_t *wav_information; //wav信息结构体，可以考虑换成一个共用体指向其他音乐格式数据
} audio_dev_t;

int record_callback(audio_dev_t *audio_dev);

audio_dev_t Audio_Dev = { 0 };

struct device *spi0;
struct device *i2c0;
struct device *uart0;
struct device *dma_ch2; //uart tx
struct device *dma_ch3; //spi rx

uint8_t buff1[2 * 1024] __attribute__((section(".system_ram"), aligned(4)));
uint8_t buff2[2 * 1024] __attribute__((section(".system_ram"), aligned(4)));

int record_callback(audio_dev_t *audio_dev);

void dma_ch3_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    Audio_Dev.audio_callback(&Audio_Dev);
}

void uart0_rx_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == UART_EVENT_RX_FIFO) {
        if (memcmp((uint8_t *)args, "start record\r\n", 14) == 0) {
            //MSG("start recording...\r\n");
            Audio_Dev.audio_state = 1;
        } else if (memcmp((uint8_t *)args, "stop  record\r\n", 14) == 0) {
            //MSG("stop recording...\r\n");
            Audio_Dev.audio_state = 0;
        } else {
            //MSG("error code...\r\n");
        }
    } else if (state == UART_EVENT_RTO) {
        if (memcmp((uint8_t *)args, "start record\r\n", 14) == 0) {
            //MSG("start recording...\r\n");
            Audio_Dev.audio_state = 1;
        } else if (memcmp((uint8_t *)args, "stop  record\r\n", 14) == 0) {
            //MSG("stop recording...\r\n");
            Audio_Dev.audio_state = 0;
        } else {
            //MSG("error code...\r\n");
        }
    }
}

uint8_t spi_init(void)
{
    spi_register(SPI0_INDEX, "spi0");
    dma_register(DMA0_CH3_INDEX, "ch3");
    spi0 = device_find("spi0");

    if (spi0) {
        ((spi_device_t *)spi0)->mode = SPI_SLVAE_MODE;
        ((spi_device_t *)spi0)->direction = SPI_MSB_BYTE0_DIRECTION_FIRST;
        ((spi_device_t *)spi0)->clk_polaraity = SPI_POLARITY_LOW;
        ((spi_device_t *)spi0)->datasize = SPI_DATASIZE_16BIT;
        ((spi_device_t *)spi0)->clk_polaraity = SPI_POLARITY_LOW;
        ((spi_device_t *)spi0)->clk_phase = SPI_PHASE_1EDGE;
        ((spi_device_t *)spi0)->fifo_threshold = 2;

        device_open(spi0, DEVICE_OFLAG_DMA_RX);
    }

    dma_ch3 = device_find("ch3");

    if (dma_ch3) {
        ((dma_device_t *)dma_ch3)->direction = DMA_PERIPH_TO_MEMORY;
        ((dma_device_t *)dma_ch3)->transfer_mode = DMA_LLI_ONCE_MODE;
        ((dma_device_t *)dma_ch3)->src_req = DMA_REQUEST_SPI0_RX;
        ((dma_device_t *)dma_ch3)->dst_req = DMA_REQUEST_NONE;
        ((dma_device_t *)dma_ch3)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        ((dma_device_t *)dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        device_open(dma_ch3, 0);
        device_set_callback(dma_ch3, dma_ch3_irq_callback);
        device_control(dma_ch3, DEVICE_CTRL_SET_INT, NULL);
        device_control(spi0, DEVICE_CTRL_ATTACH_RX_DMA, dma_ch3);
    }

    return SUCCESS;
}

void uart_init(void)
{
    dma_register(DMA0_CH2_INDEX, "ch2");
    uart0 = device_find("debug_log");

    if (uart0) {
        device_close(uart0);
    } else {
        uart_register(UART0_INDEX, "debug_log");
    }

    uart0 = device_find("debug_log");

    if (uart0) {
        ((uart_device_t *)uart0)->baudrate = 2000000;
        ((uart_device_t *)uart0)->fifo_threshold = 64;
        device_open(uart0, DEVICE_OFLAG_DMA_TX | DEVICE_OFLAG_INT_RX);
        device_set_callback(uart0, uart0_rx_irq_callback);
        device_control(uart0, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
    }

    dma_ch2 = device_find("ch2");

    if (dma_ch2) {
        ((dma_device_t *)dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        ((dma_device_t *)dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        ((dma_device_t *)dma_ch2)->src_req = DMA_REQUEST_NONE;
        ((dma_device_t *)dma_ch2)->dst_req = DMA_REQUEST_UART0_TX;
        ((dma_device_t *)dma_ch2)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        ((dma_device_t *)dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch2, 0);
        device_set_callback(dma_ch2, NULL);
        device_control(dma_ch2, DEVICE_CTRL_CLR_INT, NULL);

        device_control(uart0, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch2);
    }
}

void i2c_init(void)
{
    i2c_register(I2C0_INDEX, "i2c");
    i2c0 = device_find("i2c");

    if (i2c0) {
        ((i2c_device_t *)i2c0)->id = 0;
        ((i2c_device_t *)i2c0)->mode = I2C_HW_MODE;
        ((i2c_device_t *)i2c0)->phase = 15;
        device_open(i2c0, 0);
    }
}

int es8374_write_reg(uint8_t addr, uint8_t data)
{
    i2c_msg_t msg1;
    msg1.slaveaddr = ES8374_SLAVE_ADDR,
    msg1.len = 1,
    msg1.buf = &data;
    msg1.flags = SUB_ADDR_1BYTE | I2C_WR;
    msg1.subaddr = addr;
    bflb_platform_delay_ms(10);
    return i2c_transfer(i2c0, &msg1, 1);
}

BL_Err_Type es8388_read_reg(uint8_t addr, uint8_t *rdata)
{
    i2c_msg_t msg1;
    msg1.len = 1,
    msg1.buf = rdata;
    msg1.subaddr = addr;
    msg1.slaveaddr = ES8374_SLAVE_ADDR,
    msg1.flags = SUB_ADDR_1BYTE | I2C_RD;
    return i2c_transfer(i2c0, &msg1, 1);
}

uint8_t es8374_config_master(void)
{
    es8374_write_reg(0x00, 0x3F);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x00, 0x03);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x01, 0x7F);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x05, 0x11);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x6F, 0xA0);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x72, 0x41);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x09, 0x01);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x0C, 0x08);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x0D, 0x13);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x0E, 0xE0);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x0A, 0x3A);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x0B, 0x08);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x09, 0x41);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x24, 0x08);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x36, 0x00);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x12, 0x30);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x13, 0x20);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x21, 0x50);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x22, 0x55);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x21, 0x24);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x00, 0x80);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x14, 0x8A);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x15, 0x40);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x1A, 0xA0);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x1B, 0x19);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x1C, 0x90);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x1D, 0x2B);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x1F, 0x00);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x1E, 0xA0);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x28, 0x00);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x25, 0x1F);
    bflb_platform_delay_ms(10); //增益
    es8374_write_reg(0x38, 0x00);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x37, 0x00);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x6D, 0x60);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x03, 0x20);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x06, 0x03);
    bflb_platform_delay_ms(10); // 0x0600 ---8K sample.   0x100  48k
    es8374_write_reg(0x07, 0x00);
    bflb_platform_delay_ms(10); //
    es8374_write_reg(0x0F, 0x95);
    bflb_platform_delay_ms(10); //0x9D 8K    0x95 16K
    es8374_write_reg(0x10, 0x0D);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x11, 0x0D);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x02, 0x08);
    bflb_platform_delay_ms(10);
    return 0;
}

uint8_t es8374_config_dump(void)
{
    uint8_t i = 0;
    uint8_t val = 0;

    for (i = 0; i < 0X6D; i++) {
        es8388_read_reg(i, &val);
        MSG("0x%x = 0x%x\r\n", i, val);
    }

    return 0;
}

int record_init(audio_dev_t *audio_dev)
{
    audio_dev->buff1 = buff1;
    audio_dev->buff2 = buff2;
    audio_dev->buff_size_max = sizeof(buff1);
    audio_dev->audio_state = 0;
    audio_dev->audio_init = record_init;
    audio_dev->audio_callback = record_callback;

    i2c_init();
    uart_init();
    es8374_config_master();
    //es8374_config_dump();

    spi_init();

    device_read(spi0, 0, audio_dev->buff1, audio_dev->buff_size_max);
    audio_dev->buff_using = 1;

    return 0;
}

int record_callback(audio_dev_t *audio_dev)
{
    if (audio_dev->buff_using == 1) {
        device_read(spi0, 0, audio_dev->buff2, audio_dev->buff_size_max);

        if (audio_dev->audio_state) {
            device_write(uart0, 0, audio_dev->buff1, audio_dev->buff_size_max);
        }

        audio_dev->buff_using = 2;
    } else if (audio_dev->buff_using == 2) {
        device_read(spi0, 0, audio_dev->buff1, audio_dev->buff_size_max);

        if (audio_dev->audio_state) {
            device_write(uart0, 0, audio_dev->buff2, audio_dev->buff_size_max);
        }

        audio_dev->buff_using = 1;
    }

    return 0;
}

int main(void)
{
    bflb_platform_init(0);

    MSG("spi_i2s_record case \r\n");

    record_init(&Audio_Dev);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
