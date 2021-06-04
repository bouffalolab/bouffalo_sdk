/**
 * @file hal_dac.c
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
#include "hal_dac.h"
#include "hal_dma.h"
#include "bl702_gpio.h"
#include "bl702_dac.h"
#include "bl702_dma.h"
#include "bl702_config.h"
#include "bl702_glb.h"
#include "dac_config.h"

static dac_device_t dacx_device[] = {
#ifdef BSP_USING_DAC0
    DAC_CONFIG,
#endif
};

int dac_open(struct device *dev, uint16_t oflag)
{
    GLB_GPIP_DAC_ChanA_Cfg_Type chCfg = {0};
    GLB_GPIP_DAC_Cfg_Type dacCfg = {0} ;
    dac_device_t *dac_device = (dac_device_t *)dev;
    uint8_t dac_ext_ref_pin =0;

    if(dac_device->pin.pin_num == 2){
        GLB_GPIO_Func_Init(GPIO_FUN_ANALOG, (GLB_GPIO_Type *)&dac_device->pin.dac0, 2);
    }else if(dac_device->pin.pin_num == 1){
        GLB_GPIO_Func_Init(GPIO_FUN_ANALOG, (GLB_GPIO_Type *)&dac_device->pin.dac0, 1);
    }else{
        return ERROR;
    }

    switch(dac_device->clk){
        case DAC_CLK_500KHZ:
            PDS_Set_Audio_PLL_Freq(AUDIO_PLL_24000000_HZ);
            /* 24.0000MHZ / 3 / 16 = 500KHZ*/
            GLB_Set_DAC_CLK(ENABLE,GLB_DAC_CLK_AUDIO_PLL,3);  
            dacCfg.div = DAC_CLK_DIV_16;  
            break;
        case DAC_CLK_8KHZ:
            /* set audio pll as 12.288MHZ*/
            PDS_Set_Audio_PLL_Freq(AUDIO_PLL_12288000_HZ);
            /* 12.2880MHZ / 24 / 64 = 8KHZ*/
            GLB_Set_DAC_CLK(ENABLE,GLB_DAC_CLK_AUDIO_PLL,24); 
            dacCfg.div = DAC_CLK_DIV_64;  
        case DAC_CLK_16KHZ:
            /* set audio pll as 12.288MHZ*/
            PDS_Set_Audio_PLL_Freq(AUDIO_PLL_12288000_HZ);
            /* 12.2880MHZ / 24 / 32 = 16KHZ*/
            GLB_Set_DAC_CLK(ENABLE,GLB_DAC_CLK_AUDIO_PLL,24); 
            dacCfg.div = DAC_CLK_DIV_32; 
        case DAC_CLK_44P1KHZ:
            /* set audio pll as 11.2896MHZ*/
            PDS_Set_Audio_PLL_Freq(AUDIO_PLL_11289600_HZ);
            /* 11.2896MHZ / 16 / 16 = 44.1KHZ*/
            GLB_Set_DAC_CLK(ENABLE,GLB_DAC_CLK_AUDIO_PLL,16);  
            dacCfg.div = DAC_CLK_DIV_16;  
            break;    
        default : break;
    }

    if(DAC_REF_SEL == GLB_DAC_REF_SEL_EXTERNAL){
        dac_ext_ref_pin = DAC_EXT_REF_GPIO;
        dacCfg.refSel = GLB_DAC_REF_SEL_EXTERNAL;
        GLB_GPIO_Func_Init(GPIO_FUN_ANALOG, (GLB_GPIO_Type *)&dac_ext_ref_pin, 1);
    }else{
    	dacCfg.refSel = GLB_DAC_REF_SEL_INTERNAL;
    }

    if (oflag & DEVICE_OFLAG_STREAM_TX){
        dacCfg.dmaEn=DISABLE;
        dacCfg.resetChanA = ENABLE;
        dacCfg.resetChanB = ENABLE;
        dacCfg.dmaFmt = GPIP_DAC_DMA_FORMAT_0;

        chCfg.chanCovtEn = ENABLE;
        chCfg.outputEn = ENABLE;
        chCfg.chanEn = ENABLE;
        chCfg.src = GPIP_DAC_ChanA_SRC_REG;

        GLB_GPIP_DAC_Init(&dacCfg);
        GLB_GPIP_DAC_Set_ChanA_Config(&chCfg);
        GLB_GPIP_DAC_Set_ChanB_Config((GLB_GPIP_DAC_ChanB_Cfg_Type *)&chCfg);

    }else if(oflag & DEVICE_OFLAG_DMA_TX){
        dacCfg.dmaEn=ENABLE;
        dacCfg.resetChanA = ENABLE;
        dacCfg.resetChanB = ENABLE;
        if(dac_device->pin.pin_num == 2){
            dacCfg.dmaFmt = GPIP_DAC_DMA_FORMAT_1; 
        }else{
            dacCfg.dmaFmt = GPIP_DAC_DMA_FORMAT_0;             
        }

        chCfg.chanCovtEn = ENABLE;
        chCfg.outputEn = ENABLE;
        chCfg.chanEn = ENABLE;
        chCfg.src = GPIP_DAC_ChanA_SRC_DMA;

        GLB_GPIP_DAC_Init(&dacCfg);
        GLB_GPIP_DAC_Set_ChanA_Config(&chCfg);
        GLB_GPIP_DAC_Set_ChanB_Config((GLB_GPIP_DAC_ChanB_Cfg_Type *)&chCfg);

        GPIP_Set_DAC_DMA_TX_FORMAT_SEL(dacCfg.dmaFmt);
        GPIP_Set_DAC_DMA_TX_Enable();

    }else{

    }
    
    return 0;
}
int dac_close(struct device *dev)
{
    return 0;
}
int dac_control(struct device *dev, int cmd, void *args)
{
    dac_device_t *dac_device = (dac_device_t *)dev;
    switch (cmd)
    {
    case DEVICE_CTRL_SET_INT /* constant-expression */:

        break;
    case DEVICE_CTRL_CLR_INT /* constant-expression */:

        break;
    case DEVICE_CTRL_GET_INT /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_CONFIG /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_RESUME /* constant-expression */:

        break;
    case DEVICE_CTRL_SUSPEND /* constant-expression */:

        break;
    case DEVICE_CTRL_ATTACH_TX_DMA /* constant-expression */:
        dac_device->tx_dma = (struct device *)args;
        break;
    default:
        break;
    }

    return 0;
}
int dac_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    dac_channel_t channel = (dac_channel_t)pos;
    dac_device_t *dac_device = (dac_device_t *)dev;
    uint32_t i=0;

    if (dev->oflag & DEVICE_OFLAG_DMA_TX)
    {
        struct device *dma_ch = (struct device *)dac_device->tx_dma;

        if (!dma_ch)
            return -1;
 
        dma_reload(dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_DAC_TDR, size);
        dma_channel_start(dma_ch);

    }
    if (dev->oflag & DEVICE_OFLAG_STREAM_TX){
        if(channel==DAC_CHANNEL_0){
            for(i=0;i<size;i++){
                GLB_DAC_Set_ChanA_Value(*((uint16_t *)buffer +i));
            }
        }else if(channel==DAC_CHANNEL_1){
            for(i=0;i<size;i++){
                GLB_DAC_Set_ChanB_Value(*((uint16_t *)buffer +i));
            }     
        }else{
            for(i=0;i<size;i++){
                GLB_DAC_Set_ChanA_Value(*((uint16_t *)buffer +i));   
                GLB_DAC_Set_ChanB_Value(*((uint16_t *)buffer +i));  
            }   
        }
    }
    return 0;
}
int dac_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{

    return 0;
}

int dac_register(enum dac_index_type index, const char *name, uint16_t flag)
{
    struct device *dev;

    if(DAC_MAX_INDEX == 0)
        return -DEVICE_EINVAL;

    dev = &(dacx_device[index].parent);

    dev->open = dac_open;
    dev->close = dac_close;
    dev->control = dac_control;
    dev->write = dac_write;
    dev->read = NULL;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_ADC;
    dev->handle = NULL;

    device_register(dev, name, flag);

    return 0;
}
