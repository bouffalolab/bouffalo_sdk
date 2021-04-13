/**
 * @file audio_proto.c
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
#include "drv_device.h"
#include "audio_core.h"
#include "hal_dma.h"

volatile int (*buffer_ready_callback_func)(char bufIndex);
volatile audio_core_t * audio_core_cfg_store;
volatile int bufferIndex = 0;

void audio_core_transfer_done(struct device *dev, void *args, uint32_t size, uint32_t state){

    if(!state){
        if(buffer_ready_callback_func){
            buffer_ready_callback_func(bufferIndex);
        }

        if(bufferIndex){
            bufferIndex = 0;
            device_write(audio_core_cfg_store->audio_device,0,audio_core_cfg_store->buff1,audio_core_cfg->buff_size); 
        }else{
            bufferIndex = 1;
            device_write(audio_core_cfg_store->audio_device,0,audio_core_cfg_store->buff2,audio_core_cfg->buff_size);            
        }
    }
}

int audio_core_init(audio_core_t * audio_core_cfg){

    if(audio_core_cfg->audio_dma == NULL || 
        audio_core_cfg->buff_size == NULL|| 
        audio_core_cfg->buff1 == NULL    || 
        audio_core_cfg->buff2 == NULL){
        return -1;
    }


    memset(audio_core_cfg->buff1,0,audio_core_cfg->buff_size);
    memset(audio_core_cfg->buff2,0,audio_core_cfg->buff_size);

    device_control(audio_core_cfg->audio_device,DEVICE_CTRL_ATTACH_TX_DMA,(void*)audio_core_cfg->audio_dma);

    device_set_callback(audio_core_cfg->audio_dma, audio_core_transfer_done);

    buffer_ready_callback_func = audio_core_cfg->buffer_ready_callback;
    audio_core_cfg_store = audio_core_cfg;

    device_control(audio_core_cfg->audio_dma, DEVICE_CTRL_SET_INT, NULL);

}

int audio_core_start(void){
    device_write(audio_core_cfg_store->audio_device,0,audio_core_cfg_store->buff1,audio_core_cfg_store->buff_size); 
}

int audio_core_stop(void){
    dma_channel_stop(audio_core_cfg_store->audio_dma);
}

