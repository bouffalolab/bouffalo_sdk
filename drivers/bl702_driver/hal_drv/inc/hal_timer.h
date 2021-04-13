/**
 * @file hal_timer.h
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
#ifndef __HAL_TIMER__H__
#define __HAL_TIMER__H__

#include "drv_device.h"
#include "bl702_config.h"


typedef struct timer_device
{
    struct device parent;
    // TIMER_CFG_Type *timerAttr;
} timer_device_t;

void timer_register(timer_device_t *device, const char *name, uint16_t flag);

extern timer_device_t timerx_device;

#endif