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
#include "hal_dma.h"

uint8_t dma_src_buffer[8000] __attribute__((section(".system_ram")));
uint8_t dma_dst_buffer[8000] __attribute__((section(".system_ram")));

void dma_transfer_done(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint32_t index = 0;

    if (!state) {
        for (index = 0; index < 8000; index++) {
            if (dma_dst_buffer[index] != 0xff) {
                BL_CASE_FAIL;
                return;
            }
        }

        BL_CASE_SUCCESS;
    }
}

int main(void)
{
    bflb_platform_init(0);

    dma_register(DMA0_CH0_INDEX, "DMA", DEVICE_OFLAG_RDWR);

    struct device *dma = device_find("DMA");

    if (dma) {
        device_open(dma, 0);
        device_set_callback(dma, dma_transfer_done);
        device_control(dma, DEVICE_CTRL_SET_INT, NULL);
    }

    /* prepare test memory src as 0xff but dest as 0x0 */

    memset(dma_src_buffer, 0xff, 8000);
    memset(dma_dst_buffer, 0, 8000);

    dma_reload(dma, (uint32_t)dma_src_buffer, (uint32_t)dma_dst_buffer, 8000);

    dma_channel_start(dma);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
