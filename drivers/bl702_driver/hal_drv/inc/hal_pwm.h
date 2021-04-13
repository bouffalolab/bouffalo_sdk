/**
 * @file hal_pwm.h
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
#ifndef __HAL_PWM__H__
#define __HAL_PWM__H__

#include "drv_device.h"
#include "bl702_config.h"


enum pwm_index_type
{
#ifdef BSP_USING_UART0
    PWM0_INDEX,
#endif
#ifdef BSP_USING_UART1
    PWM1_INDEX,
#endif
#ifdef BSP_USING_UART2
    PWM2_INDEX,
#endif
#ifdef BSP_USING_UART3
    PWM3_INDEX,
#endif
#ifdef BSP_USING_UART4
    PWM4_INDEX,
#endif
    PWM_MAX_INDEX
};

typedef struct
{
    uint32_t frequency;
    uint8_t dutyCycle;
} pwm_config_t;
typedef struct pwm_device
{
    struct device parent;
    uint8_t ch;
    uint32_t frequency;
    uint8_t dutyCycle;
} pwm_device_t;


void pwm_register(enum pwm_index_type index, const char *name, uint16_t flag);

#endif