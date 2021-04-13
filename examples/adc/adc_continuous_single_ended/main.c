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
#include "hal_adc.h"
#include "hal_gpio.h"

uint8_t PinList[] = {GPIO_PIN_12};
adc_channel_t posChList[] = {ADC_CHANNEL4};
adc_channel_t negChList[] = {ADC_CHANNEL_GND};

adc_res_val_t result_val;

struct device* adc_test;

int main(void)
{
    bflb_platform_init(0);

    adc_user_cfg_t adc_user_cfg;

    adc_user_cfg.dma_en = DISABLE;              // disable dma
    adc_user_cfg.conv_mode = ADC_CON_CONV;      // continuous conversion
    adc_user_cfg.in_mode = ADC_SINGLE_ENDED_IN; // single-ended input mode
    
    adc_user_cfg.pinList = PinList;
    adc_user_cfg.posChList = posChList;
    adc_user_cfg.negChList = negChList;
    adc_user_cfg.num = sizeof(posChList)/sizeof(adc_channel_t);

    MSG("adc continue single ended test case \r\n");
    adc_register(ADC0_INDEX, "adc_continue_single", DEVICE_OFLAG_STREAM_RX, &adc_user_cfg);

    adc_test = device_find("adc_continue_single");
    if(adc_test)
    {
        device_open(adc_test, DEVICE_OFLAG_STREAM_RX);
        MSG("adc device find success\r\n");
    }
    
    device_control(adc_test,DEVICE_CTRL_RESUME,0);

    while (1)
    {
        device_read(adc_test, 0, (void *)&result_val,sizeof(result_val)/sizeof(adc_res_val_t));
        MSG("PosId = %d NegId = %d V= %d mV \r\n",result_val.posChan,result_val.negChan,(uint32_t)(result_val.volt * 1000));
        // bflb_platform_delay_ms(500);
    }
    
}