
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

#include "hal_i2s.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "fhm_onechannel_16k_20.h"
#include "bsp_es8388.h"


static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode      = ES8388_CODEC_MDOE,        /*!< ES8388 work mode */
    .role           = ES8388_SLAVE,             /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC,    /*!< ES8388 mic input mode */
    .mic_pga        = ES8388_MIC_PGA_3DB,      /*!< ES8388 mic PGA */
    .i2s_frame      = ES8388_LEFT_JUSTIFY_FRAME,     /*!< ES8388 I2S frame */
    .data_width     = ES8388_DATA_LEN_16,       /*!< ES8388 I2S dataWitdh */
};

int main(void)
{

    bflb_platform_init(0);

    /* init ES8388 Codec */
    ES8388_Init(&ES8388Cfg);
    ES8388_Set_Voice_Volume(60);
    
    MSG("Play Music Form flash\r\n");

    /* register & open i2s device */
    i2s_register(I2S0_INDEX,"I2S",DEVICE_OFLAG_RDWR);
    struct device* i2s  = device_find("I2S");

    ((i2s_device_t*)i2s)->iis_mode = I2S_MODE_MASTER;
    ((i2s_device_t*)i2s)->interface_mode = I2S_MODE_LEFT;
    ((i2s_device_t*)i2s)->sampl_freq_hz = 16*1000;
    ((i2s_device_t*)i2s)->channel_num = I2S_FS_CHANNELS_NUM_MONO;
    ((i2s_device_t*)i2s)->frame_size = I2S_FRAME_LEN_16;
    ((i2s_device_t*)i2s)->data_size = I2S_DATA_LEN_16;
    ((i2s_device_t*)i2s)->fifo_threshold = 8;

    if(i2s)
    {
        device_open(i2s,DEVICE_OFLAG_DMA_TX|DEVICE_OFLAG_DMA_RX);
        MSG("device open success\r\n");
    }


    /* register & open dma device */
    if(dma_allocate_register("i2s_dma",DEVICE_OFLAG_RDWR) == SUCCESS){
        MSG("dma allocate success\r\n");
    }

    struct device *i2s_dma = device_find("i2s_dma");

    if(i2s_dma)
    {
        ((dma_device_t*)i2s_dma)->direction = DMA_MEMORY_TO_PERIPH;
        ((dma_device_t*)i2s_dma)->transfer_mode = DMA_LLI_ONCE_MODE;
        ((dma_device_t*)i2s_dma)->src_req = DMA_REQUEST_NONE;
        ((dma_device_t*)i2s_dma)->dst_req = DMA_REQUEST_I2S_TX;
        ((dma_device_t*)i2s_dma)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        ((dma_device_t*)i2s_dma)->dst_width = DMA_TRANSFER_WIDTH_16BIT;

        device_open(i2s_dma, 0);

        /* connect i2s device and dma device */
        device_control(i2s,DEVICE_CTRL_I2S_ATTACH_TX_DMA,(void*)i2s_dma);

        /* start play source*/
        device_write(i2s,0,fhm_onechannel_16k_20,sizeof(fhm_onechannel_16k_20));
    }

    MSG("Play Music start \r\n");

    BL_CASE_SUCCESS;
}

