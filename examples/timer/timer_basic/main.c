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

#include "hal_timer.h"
#include "bflb_platform.h"

volatile uint32_t cnt = 0;
int main(void)
{
    bflb_platform_init(0);

    timer_user_cfg_t timer_user_cfg0;
    timer_user_cfg0.timeout_val = 1000*10;   /* us */
    timer_user_cfg0.comp_it     = TIMER_COMP0_IT;

    timer_user_cfg_t timer_user_cfg1;
    timer_user_cfg1.timeout_val = 1000*100;   /* us */
    timer_user_cfg1.comp_it     = TIMER_COMP1_IT;

    timer_user_cfg_t timer_user_cfg2;
    timer_user_cfg2.timeout_val = 1000*1000;   /* us */
    timer_user_cfg2.comp_it     = TIMER_COMP2_IT;

    timer_register(TIMER_CH1_INDEX, "timer_ch1", DEVICE_OFLAG_RDWR);

    struct device* timer_ch1 = device_find("timer_ch1");

    if(timer_ch1)
    {
        device_open(timer_ch1, DEVICE_OFLAG_POLL);
        device_control(timer_ch1, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg0));
        device_control(timer_ch1, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg1));
        device_control(timer_ch1, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg2));
    }
    else
    {
        MSG("timer device open failed! \n");
    }

    while(1)
    {
        if(SET == device_control(timer_ch1,DEVICE_CTRL_GET_MATCH_STATUS,(void*)TIMER_EVENT_COMP2))
        {
            MSG("Comparator 2 match, timer reload\n");
            device_control(timer_ch1, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg0));
            device_control(timer_ch1, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg1));
            device_control(timer_ch1, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg2));
            MSG("Counter value=%d,M0=%d, M1=%d,M2=%d \n", device_control(timer_ch1, DEVICE_CTRL_GET_CONFIG,NULL),
                                            device_control(timer_ch1, DEVICE_CTRL_GET_MATCH_STATUS, (void*)TIMER_EVENT_COMP0),
                                            device_control(timer_ch1, DEVICE_CTRL_GET_MATCH_STATUS, (void*)TIMER_EVENT_COMP1),
                                            device_control(timer_ch1, DEVICE_CTRL_GET_MATCH_STATUS, (void*)TIMER_EVENT_COMP2));
            break;
        }
        MSG("Counter value=%d,M0=%d, M1=%d,M2=%d \n", device_control(timer_ch1, DEVICE_CTRL_GET_CONFIG,NULL),
                                         device_control(timer_ch1, DEVICE_CTRL_GET_MATCH_STATUS, (void*)TIMER_EVENT_COMP0),
                                         device_control(timer_ch1, DEVICE_CTRL_GET_MATCH_STATUS, (void*)TIMER_EVENT_COMP1),
                                         device_control(timer_ch1, DEVICE_CTRL_GET_MATCH_STATUS, (void*)TIMER_EVENT_COMP2));
    }
    BL_CASE_SUCCESS;
}
