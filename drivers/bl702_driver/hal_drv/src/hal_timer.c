/**
 * @file hal_timer.c
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
#include "bl702_timer.h"

// extern struct device * timer;
// /****************************************************************************/ /**
//  * @brief  TIMER match 0 interrupt calllbaeck funtion
//  *
//  * @param  None
//  *
//  * @return None
//  *
// *******************************************************************************/
// static void Timer_Match0_Cbf(void)
// {
//     MSG("Timer_Match0_Cbf\r\n"); //match0Arrived=1;
//     //device_close(timer);
// }

// /****************************************************************************/ /**
//  * @brief  TIMER match 1 interrupt calllbaeck funtion
//  *
//  * @param  None
//  *
//  * @return None
//  *
// *******************************************************************************/
// static void Timer_Match1_Cbf(void)
// {
//     MSG("Timer_Match1_Cbf\r\n"); //match1Arrived=1;
//   //  device_close(timer);
// }

// /****************************************************************************/ /**
//  * @brief  TIMER match 2 interrupt calllbaeck funtion
//  *
//  * @param  None
//  *
//  * @return None
//  *
// *******************************************************************************/
// static void Timer_Match2_Cbf(void)
// {
//     MSG("Timer_Match2_Cbf\r\n"); //match2Arrived=1;
//     device_close(timer);
// }

int timer_open(struct device *dev, uint16_t oflag)
{

    return 0;
}
int timer_close(struct device *dev)
{

    return 0;
}
int timer_control(struct device *dev, int cmd, void *args)
{

    switch (cmd)
    {
    case DEVICE_CTRL_SET_INT /* constant-expression */:
    
        break;
    case DEVICE_CTRL_CLR_INT /* constant-expression */:
        /* code */

        break;
    case DEVICE_CTRL_GET_INT /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_CONFIG /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_RESUME /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_SUSPEND /* constant-expression */:
        /* code */
        break;
    default:
        break;
    }

    return 0;


    return 0;
}
int timer_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{

    return 0;
}
int timer_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{

    return 0;
}

void timer_register(timer_device_t *device, const char *name, uint16_t flag)
{
    struct device *dev;

    dev = &(device->parent);

    dev->open = timer_open;
    dev->close = timer_close;
    dev->control = timer_control;
    dev->write = NULL;
    dev->read = NULL;
    
    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_TIMER;
    dev->handle = NULL;

    device_register(dev, name, flag);
}
