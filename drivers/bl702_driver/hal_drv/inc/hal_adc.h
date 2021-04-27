/**
 * @file hal_adc.h
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
#ifndef __HAL_ADC__H__
#define __HAL_ADC__H__

#include "drv_device.h"
#include "bl702_config.h"

enum adc_index_type
{
#ifdef BSP_USING_ADC0
    ADC0_INDEX,
#endif
    ADC_MAX_INDEX
};

#define ADC_CLK_MAX (2000000)

#define LEFT_SHIFT_BIT(x) (1 << x)

typedef enum {
    ADC_CHANNEL0,                              /* GPIO 0, ADC channel 0 */
    ADC_CHANNEL1,                              /* GPIO 1, ADC channel 1 */
    ADC_CHANNEL2,                              /* GPIO 2, ADC channel 2 */
    ADC_CHANNEL3,                              /* GPIO 3, ADC channel 3 */
    ADC_CHANNEL4,                              /* GPIO 4, ADC channel 4 */
    ADC_CHANNEL5,                              /* GPIO 5, ADC channel 5 */
    ADC_CHANNEL6,                              /* GPIO 6, ADC channel 6 */
    ADC_CHANNEL7,                              /* GPIO 7, ADC channel 7 */
    ADC_CHANNEL8,                              /* GPIO 8, ADC channel 8 */
    ADC_CHANNEL9,                              /* GPIO 9, ADC channel 9 */
    ADC_CHANNEL10,                             /* GPIO 10, ADC channel 10 */
    ADC_CHANNEL11,                             /* GPIO 11, ADC channel 11 */
    ADC_CHANNEL_DAC_OUTA,                      /* DACA, ADC channel 12 */
    ADC_CHANNEL_DAC_OUTB,                      /* DACB, ADC channel 13 */
    ADC_CHANNEL_TSEN_P,                        /* TSenp, ADC channel 14 */
    ADC_CHANNEL_TSEN_N,                        /* TSenn, ADC channel 15 */
    ADC_CHANNEL_VREF,                          /* Vref, ADC channel 16 */
    ADC_CHANNEL_DCTEST,                        /* DCTest, ADC channel 17 */
    ADC_CHANNEL_VABT_HALF,                     /* VBAT/2, ADC channel 18 */
    ADC_CHANNEL_SENP3,                         /* SenVP3, ADC channel 19 */
    ADC_CHANNEL_SENP2,                         /* SenVP2, ADC channel 20 */
    ADC_CHANNEL_SENP1,                         /* SenVP1, ADC channel 21 */
    ADC_CHANNEL_SENP0,                         /* SenVP0, ADC channel 22 */
    ADC_CHANNEL_GND,                           /* GND, ADC channel 23 */
}adc_channel_t;

typedef enum {
    ADC_CLK_2MHZ,
    ADC_CLK_1P3MHZ,
    ADC_CLK_1MHZ,
    ADC_CLK_500KHZ,   
}adc_clk_t;

typedef enum {
    ADC_VREF_3P2V = 0,                          /* ADC select 3.2V as reference voltage */
    ADC_VREF_2P0V = 1,                          /* ADC select 2V as reference voltage */
}adc_vref_t;

typedef enum {
    ADC_ONE_CONV = 0,                           /* ADC One shot conversion mode  */
    ADC_CON_CONV = 1,                           /* ADC Continuous conversion mode */
}adc_conv_t;

/**
 *  @brief ADC data width type definition
 */
typedef enum {
    ADC_DATA_WD_12,                      /*!< ADC 12 bits */
    ADC_DATA_WD_14_WITH_16_AVERAGE,      /*!< ADC 14 bits,and the value is average of 16 converts */
    ADC_DATA_WD_16_WITH_64_AVERAGE,      /*!< ADC 16 bits,and the value is average of 64 converts */
    ADC_DATA_WD_16_WITH_128_AVERAGE,     /*!< ADC 16 bits,and the value is average of 128 converts */
    ADC_DATA_WD_16_WITH_256_AVERAGE,     /*!< ADC 16 bits,and the value is average of 256 converts */
}adc_data_width_t;

typedef enum {
    ADC_SINGLE_ENDED_IN = 0,                    /* ADC signal is single-ended input */
    ADC_DIFFERENTIAL_IN = 1,                    /* ADC signal is differential input*/
}adc_inputmode_t;

typedef struct {
    uint8_t *pinList;
    uint8_t *posChList;
    uint8_t *negChList;
    uint8_t  num;
    uint8_t  dma_en;    
    adc_conv_t   conv_mode;
    adc_inputmode_t in_mode;    
} adc_user_cfg_t;

typedef struct {
    int8_t posChan;                         /*!< Positive channel */
    int8_t negChan;                         /*!< Negative channel */
    uint16_t value;                         /*!< ADC value */
    float volt;                             /*!< ADC voltage result */
}adc_res_val_t;

typedef struct adc_device
{
    struct device parent;
    adc_clk_t  clk;                  /* CLK is not more than 2Mhz */
    adc_vref_t vref;                 /* ADC voltage reference*/
    adc_data_width_t resWidth;
} adc_device_t;

#define ADC_DEV(dev) ((adc_device_t*)dev)

int adc_register(enum adc_index_type index, const char *name, uint16_t flag, adc_user_cfg_t *adc_user_cfg);

#endif