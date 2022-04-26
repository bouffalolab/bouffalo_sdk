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
#include "bflb_platform.h"
#include "hal_dma.h"
#include "hal_uart.h"

static volatile uint8_t dma_busy_flag = 0;

void dma_transfer_done(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    dma_busy_flag = 0;
    BL_CASE_SUCCESS;
}

int main(void)
{
    bflb_platform_init(0);

    uart_register(UART1_INDEX, "uart1");
    struct device *uart = device_find("uart1");

    if (uart) {
        //UART_DEV(uart)->fifo_threshold = 0;
        device_open(uart, DEVICE_OFLAG_DMA_TX | DEVICE_OFLAG_DMA_RX);
    }

    dma_register(DMA0_CH0_INDEX, "dma_ch0");

    struct device *dma_ch0 = device_find("dma_ch0");

    if (dma_ch0) {
        DMA_DEV(dma_ch0)->direction = DMA_PERIPH_TO_PERIPH;
        DMA_DEV(dma_ch0)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch0)->src_req = DMA_REQUEST_UART1_RX;
        DMA_DEV(dma_ch0)->dst_req = DMA_REQUEST_UART1_TX;
        DMA_DEV(dma_ch0)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch0)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch0)->src_burst_size = DMA_BURST_INCR1;
        DMA_DEV(dma_ch0)->dst_burst_size = DMA_BURST_INCR1;
        DMA_DEV(dma_ch0)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch0)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch0, 0);
        device_set_callback(dma_ch0, dma_transfer_done);
        device_control(dma_ch0, DEVICE_CTRL_SET_INT, NULL);
    }


    while (1) {
        if (dma_busy_flag == 0) {
            dma_busy_flag = 1;
            /* default fifo_threshold is 63 , so if size < 64 ,data will be stored in fifo not tdr.
            So you can modify fifo_threshold=0 at first to get one data trigger
            */
            dma_reload(dma_ch0, (uint32_t)DMA_ADDR_UART1_RDR, (uint32_t)DMA_ADDR_UART1_TDR, 64);
            dma_channel_start(dma_ch0);
        }
        bflb_platform_delay_ms(100);
    }
}
