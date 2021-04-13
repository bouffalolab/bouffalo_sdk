/**
 * @file audio_core.h
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

#ifndef __AUDIO_PROTO__
#define __AUDIO_PROTO__

#include "misc.h"

typedef struct{

    uint8_t *buff1;
    uint8_t *buff2;
    uint32_t buff_size;  

    struct device * audio_dma;
    struct device * audio_device;
    
    int (*buffer_ready_callback)(char bufIndex);
    
}audio_core_t;

int audio_core_init(audio_core_t * audio_core_cfg);
int audio_core_start(void);
int audio_core_stop(void);
#endif