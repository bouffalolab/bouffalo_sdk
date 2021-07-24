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
#include "fhm_onechannel_16k_20.h"

#define SPI0_CS           GPIO_PIN_10
#define ES8374_SLAVE_ADDR 0x10

typedef struct audio_dev {
    uint8_t *buff1;
    uint8_t *buff2;
    uint32_t buff1_data_size;
    uint32_t buff2_data_size;
    uint32_t buff_size_max;
    uint8_t buff_using;

    uint8_t audio_state;
    uint8_t audio_type;

    int (*audio_init)(struct audio_dev *audio_dev);
    int (*audio_control)(struct audio_dev *audio_dev, int cmd, void *args);

    int (*audio_callback)(struct audio_dev *audio_dev);
    struct device *device;
} audio_dev_t;

int record_callback(audio_dev_t *audio_dev);

audio_dev_t Audio_Dev = { 0 };

struct device *spi0;
struct device *i2c0;
struct device *dma_ch2; //spi tx

uint8_t test_buff[4 * 1024];

void dma_ch2_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    record_callback(&Audio_Dev);
}

uint8_t spi_init(void)
{
    spi_register(SPI0_INDEX, "spi0", DEVICE_OFLAG_RDWR);
    dma_register(DMA0_CH3_INDEX, "ch2", DEVICE_OFLAG_RDWR);
    spi0 = device_find("spi0");

    if (spi0) {
        ((spi_device_t *)spi0)->mode = SPI_SLVAE_MODE;
        ((spi_device_t *)spi0)->direction = SPI_MSB_BYTE0_DIRECTION_FIRST;
        ((spi_device_t *)spi0)->clk_polaraity = SPI_POLARITY_HIGH;
        ((spi_device_t *)spi0)->datasize = SPI_DATASIZE_16BIT;
        ((spi_device_t *)spi0)->clk_phase = SPI_PHASE_1EDGE;
        ((spi_device_t *)spi0)->fifo_threshold = 2;

        device_open(spi0, DEVICE_OFLAG_DMA_TX);
    }

    dma_ch2 = device_find("ch2");

    if (dma_ch2) {
        ((dma_device_t *)dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        ((dma_device_t *)dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        ((dma_device_t *)dma_ch2)->src_req = DMA_REQUEST_NONE;
        ((dma_device_t *)dma_ch2)->dst_req = DMA_REQUEST_SPI0_TX;
        ((dma_device_t *)dma_ch2)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        ((dma_device_t *)dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        device_open(dma_ch2, 0);
        device_set_callback(dma_ch2, dma_ch2_irq_callback);
        device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
        device_control(spi0, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch2);
    }

    return SUCCESS;
}

void i2c_init(void)
{
    i2c_register(I2C0_INDEX, "i2c", DEVICE_OFLAG_RDWR);
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
    bflb_platform_delay_ms(10); //DAC音量
    es8374_write_reg(0x37, 0x00);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x6D, 0x60);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x03, 0x20);
    bflb_platform_delay_ms(10);
    es8374_write_reg(0x06, 0x03);
    bflb_platform_delay_ms(10); // 0x0600 ---8K sample.   0x0300 -16K
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
    audio_dev->buff1 = (void *)fhm_onechannel_16k_20;
    audio_dev->buff_size_max = sizeof(fhm_onechannel_16k_20);
    audio_dev->buff1_data_size = sizeof(fhm_onechannel_16k_20);
    audio_dev->audio_state = 1;

    i2c_init();
    es8374_config_master();
    //es8374_config_dump();
    //memset(test_buff,0x55,sizeof(test_buff));

    spi_init();
    MSG("play start\r\n");
    device_write(spi0, 0, audio_dev->buff1, audio_dev->buff1_data_size);
    audio_dev->buff_using = 1;

    return 0;
}

int record_callback(audio_dev_t *audio_dev)
{
    MSG("play end\r\n");
    MSG("play start\r\n");
    device_write(spi0, 0, audio_dev->buff1, audio_dev->buff1_data_size);
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
