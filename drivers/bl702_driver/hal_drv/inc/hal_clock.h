/**
 * @file hal_clock.h
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
#ifndef __HAL_CLOCK__H__
#define __HAL_CLOCK__H__

#include "drv_device.h"
#include "bl702_config.h"

#define INTERNAL_XTAL_32M 2
#define EXTERNAL_XTAL_32M 1

#define ROOT_CLOCK_SOURCE_RC_32K     0
#define ROOT_CLOCK_SOURCE_XTAL_32K   1
#define ROOT_CLOCK_SOURCE_RC_32M     2

#define ROOT_CLOCK_SOURCE_XTAL_32M   3
#define ROOT_CLOCK_SOURCE_PLL_57P6M  4
#define ROOT_CLOCK_SOURCE_PLL_96M    5
#define ROOT_CLOCK_SOURCE_PLL_144M   6

#define ROOT_CLOCK_SOURCE_XCLK   ROOT_CLOCK_SOURCE_XTAL_32M
#define ROOT_CLOCK_SOURCE_FCLK   7
#define ROOT_CLOCK_SOURCE_HCLK   7
#define ROOT_CLOCK_SOURCE_BCLK   8


#define ROOT_CLOCK_SOURCE_AUPLL      9
#define AUDIO_PLL_CLOCK_12288000_HZ  0x90
#define AUDIO_PLL_CLOCK_11289600_HZ  0x91
#define AUDIO_PLL_CLOCK_5644800_HZ   0x92
#define AUDIO_PLL_CLOCK_24576000_HZ  0x93
#define AUDIO_PLL_CLOCK_24000000_HZ  0x94

#define OUTPUT_REF_CLOCK_SOURCE_NONE 0
#define OUTPUT_REF_CLOCK_SOURCE_I2S  1

#if (BSP_ROOT_CLOCK_SOURCE > 2) && (BSP_ROOT_CLOCK_SOURCE < 7)
#define CLOCK_XTAL          EXTERNAL_XTAL_32M
#else
#define CLOCK_XTAL          INTERNAL_XTAL_32M
#endif

enum system_clock_type
{
    SYSTEM_CLOCK_ROOT_CLOCK = 0,
    SYSTEM_CLOCK_FCLK,
    SYSTEM_CLOCK_BCLK,
    SYSTEM_CLOCK_XCLK,
    SYSTEM_CLOCK_AUPLL,
};
enum peripheral_clock_type
{
    PERIPHERAL_CLOCK_UART = 0,
    PERIPHERAL_CLOCK_SPI,
    PERIPHERAL_CLOCK_I2C,
    PERIPHERAL_CLOCK_ADC,
    PERIPHERAL_CLOCK_DAC,
    PERIPHERAL_CLOCK_I2S,
    PERIPHERAL_CLOCK_PWM,
    PERIPHERAL_CLOCK_CAM,
};

void system_clock_init(void);
void peripheral_clock_init(void);
uint32_t system_clock_get(enum system_clock_type type);
uint32_t peripheral_clock_get(enum peripheral_clock_type type);
#endif