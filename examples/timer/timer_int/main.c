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

struct device *timer_ch0;

volatile uint32_t cnt = 0;

void timer_ch0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    MSG("timer ch0 interrupt! \n");

    if (state == TIMER_EVENT_COMP0) {
        cnt++;
        MSG("timer event comp0! cnt=%d\n", cnt);
    } else if (state == TIMER_EVENT_COMP1) {
        MSG("timer event comp1! \n");
    } else if (state == TIMER_EVENT_COMP2) {
        MSG("timer event comp2! \n");
    }
}

int main(void)
{
    bflb_platform_init(0);

    timer_user_cfg_t timer_user_cfg;
    timer_user_cfg.timeout_val = 1000 * 1000; /* us */
    timer_user_cfg.comp_it = TIMER_COMP0_IT;

    timer_register(TIMER_CH0_INDEX, "timer_ch0", DEVICE_OFLAG_RDWR);

    timer_ch0 = device_find("timer_ch0");

    if (timer_ch0) {
        device_open(timer_ch0, DEVICE_OFLAG_INT);
        device_set_callback(timer_ch0, timer_ch0_irq_callback);
        device_control(timer_ch0, DEVICE_CTRL_SET_INT, NULL);
        device_control(timer_ch0, DEVICE_CTRL_TIMER_CH_START, (void *)(&timer_user_cfg));
    } else {
        MSG("timer device open failed! \n");
    }

    while (1) {
        if (cnt == 10) {
            device_control(timer_ch0, DEVICE_CTRL_TIMER_CH_STOP, (void *)(&timer_user_cfg));
            MSG("timer test over! \n");
            BL_CASE_SUCCESS;
        }
    }
}
