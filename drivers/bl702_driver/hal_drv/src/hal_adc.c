/**
 * @file hal_adc.c
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
#include "hal_clock.h"
#include "bl702_glb.h"
#include "bl702_dma.h"
#include "bl702_adc.h"
#include "adc_config.h"

static adc_device_t adcx_device[ADC_MAX_INDEX] = {
#ifdef BSP_USING_ADC0
    ADC0_CONFIG,
#endif
};

/**
 * @brief 
 * 
 * @param dev 
 * @param oflag 
 * @return int 
 */
int adc_open(struct device *dev, uint16_t oflag)
{
    adc_device_t *adc_device = (adc_device_t *)dev;
    ADC_CFG_Type adc_cfg = {0};
    ADC_FIFO_Cfg_Type adc_fifo_cfg = {0};
 
    ADC_Disable();
    ADC_Reset();

    adc_cfg.clkDiv = adc_device->clk_div;

    adc_cfg.vref = adc_device->vref;
    adc_cfg.resWidth = adc_device->data_width;
    adc_cfg.inputMode = adc_device->differential_mode;

    adc_cfg.v18Sel = ADC_V18_SELECT;
    adc_cfg.v11Sel = ADC_V11_SELECT;
    adc_cfg.gain1  = ADC_PGA_GAIN1;
    adc_cfg.gain2  = ADC_PGA_GAIN2;
    adc_cfg.chopMode = ADC_CHOP_MODE;
    adc_cfg.biasSel  = ADC_BIAS_SELECT;
    adc_cfg.vcm = ADC_PGA_VCM;
    adc_cfg.offsetCalibEn = ADC_OFFSET_CALIB_EN;
    adc_cfg.offsetCalibVal = ADC_OFFSER_CALIB_VAL;

    adc_fifo_cfg.dmaEn = DISABLE;

    if (oflag & DEVICE_OFLAG_STREAM_TX)
    {
    }
    if ((oflag & DEVICE_OFLAG_INT_TX) || (oflag & DEVICE_OFLAG_INT_RX))
    {

    }
    if (oflag & DEVICE_OFLAG_DMA_TX)
    {

    }
    if (oflag & DEVICE_OFLAG_DMA_RX)
    {
        adc_fifo_cfg.dmaEn = ENABLE;
    }

    adc_fifo_cfg.fifoThreshold = adc_device->fifo_threshold;

    ADC_Init(&adc_cfg);

    ADC_FIFO_Cfg(&adc_fifo_cfg);
    
    ADC_Enable();
    return 0;
}
/**
 * @brief 
 * 
 * @param dev 
 * @return int 
 */
int adc_close(struct device *dev)
{
    ADC_Disable();
    return 0;
}
/**
 * @brief 
 * 
 * @param dev 
 * @param cmd 
 * @param args 
 * @return int 
 */
int adc_control(struct device *dev, int cmd, void *args)
{
    adc_device_t* adc_device = (adc_device_t *)dev;
    adc_channel_cfg_t* adc_channel_cfg = (adc_channel_cfg_t *)args;

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
    case DEVICE_CTRL_ADC_CHANNEL_CONFIG /* constant-expression */:
        if(adc_channel_cfg->num == 1)
            ADC_Channel_Config(*adc_channel_cfg->pos_channel,*adc_channel_cfg->neg_channel,adc_device->continuous_conv_mode);
        else
        {
            ADC_Scan_Channel_Config(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel,adc_channel_cfg->num,adc_device->continuous_conv_mode);
        }
        break;
    case DEVICE_CTRL_ADC_CHANNEL_START /* constant-expression */:
        /* code */
        ADC_Start();
        break;
    case DEVICE_CTRL_ADC_CHANNEL_STOP /* constant-expression */:
        /* code */
        ADC_Stop();
        break;
    case DEVICE_CTRL_ADC_VBAT_ON:
        ADC_Vbat_Enable();
        break;
    case DEVICE_CTRL_ADC_VBAT_OFF:
        ADC_Vbat_Disable();
        break;
    case DEVICE_CTRL_ADC_TSEN_ON:
        ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
        break;
    case DEVICE_CTRL_ADC_TSEN_OFF:

        break;
    default:
        break;
    }

    return 0;
}
// int adc_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
// {

//     return 0;
// }
/**
 * @brief 
 * 
 * @param dev 
 * @param pos 
 * @param buffer 
 * @param size 
 * @return int 
 */
int adc_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{

    if(dev->oflag & DEVICE_OFLAG_STREAM_RX)
    {
        while (ADC_Get_FIFO_Count() == 0);
        do
        {
            uint32_t regVal = ADC_Read_FIFO();
            if (regVal)
            {
                ADC_Result_Type *result = (ADC_Result_Type *)buffer;
                ADC_Parse_Result(&regVal, size, result);
            }
        } while (ADC_Get_FIFO_Count() != 0);

    }
    return 0;
}

int adc_trim_tsen(uint16_t * tsen_offset)
{
    return ADC_Trim_TSEN(tsen_offset);
}

float adc_get_tsen(uint16_t tsen_offset)
{
    return TSEN_Get_Temp(tsen_offset);
}

/**
 * @brief 
 * 
 * @param index 
 * @param name 
 * @param flag 
 * @param adc_user_cfg 
 * @return int 
 */
int adc_register(enum adc_index_type index, const char *name, uint16_t flag)
{
    struct device *dev;

    if (ADC_MAX_INDEX == 0)
        return -DEVICE_EINVAL;

    dev = &(adcx_device[index].parent);

    dev->open = adc_open;
    dev->close = adc_close;
    dev->control = adc_control;
    dev->write = NULL;
    dev->read = adc_read;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_ADC;
    dev->handle = NULL;

    return device_register(dev, name, flag);
}
