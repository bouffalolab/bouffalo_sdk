/**
 * @file hal_dac.h
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
#ifndef __HAL_DAC__H__
#define __HAL_DAC__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"

enum dac_index_type {
#ifdef BSP_USING_DAC0
    DAC0_INDEX,
#endif
    DAC_MAX_INDEX
};

typedef struct
{
    uint8_t dac0;
    uint8_t dac1;
    uint8_t pin_num;
} dac_pin_t;

typedef enum {
    DAC_CHANNEL_0,
    DAC_CHANNEL_1,
    DAC_CHANNEL_ALL,
} dac_channel_t;

typedef enum {
    DAC_CLK_500KHZ,
    DAC_CLK_44P1KHZ,
    DAC_CLK_16KHZ,
    DAC_CLK_8KHZ,
} dac_clk_t;

typedef struct dac_device {
    struct device parent;
    dac_clk_t clk;
    dac_pin_t pin;

    void *tx_dma;
} dac_device_t;

#define DAC_DEV(dev) ((adc_device_t *)dev)

int dac_register(enum dac_index_type index, const char *name);
#ifdef __cplusplus
}
#endif
#endif
