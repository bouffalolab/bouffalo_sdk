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
#ifndef __HAL_POWER__H__
#define __HAL_POWER__H__

#include "drv_device.h"
#include "bl702_config.h"

typedef enum
{
    CLOCK_AHB_UART0_GATE               = 0x10,
    CLOCK_AHB_UART1_GATE               = 0x11,
    CLOCK_AHB_SPI_GATE                 = 0x12,
    CLOCK_AHB_I2C_GATE                 = 0x13,
    CLOCK_AHB_PWM_GATE                 = 0x14,
    CLOCK_AHB_TMR_GATE                 = 0x15,
    CLOCK_AHB_IRR_GATE                 = 0x16,
    CLOCK_AHB_CKS_GATE                 = 0x17,
    CLOCK_AHB_QDEC_GATE                = 0x18,
    CLOCK_AHB_KYS_GATE                 = 0x19,
    CLOCK_AHB_I2S_GATE                 = 0x1A,
    CLOCK_AHB_USB_GATE                 = 0x1C,
    CLOCK_AHB_CAM_GATE                 = 0x1D,
    CLOCK_AHB_MJPEG_GATE               = 0x1E,
    CLOCK_AHB_ALL                      = 0x1F,
}Clock_Gate_Type;

int lp_set_clock_gate(uint8_t enable , Clock_Gate_Type clockType);
void lp_set_all_gpio_hz(void);
void lp_power_off_dll(void);

int lp_enter_wfi(void);
int lp_enter_pds(uint32_t sleep_time ,void (*preCbFun)(void),void (*postCbFun)(void));
int lp_enter_hbn(uint32_t sleepTime);

#endif
