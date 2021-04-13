/**
 * @file adc_key.c
 * @author Boufflao Lab MCU Team (jychen@bouffalolab.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-23
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


#define KEY0_ADC_VALUE  0
#define KEY1_ADC_VALUE  100
#define KEY2_ADC_VALUE  200
#define KEY3_ADC_VALUE  300
#define KEY4_ADC_VALUE  430
#define KEY_NO_VALUE    3000
#define KEY_MEASURE_OFFSET    30

#define KEY_FILTER_NUM  200
typedef struct {
    uint32_t vaildKeyNum;
    uint16_t keyValue[KEY_FILTER_NUM];
}adc_keys_status;

uint8_t PinList[] = {GPIO_PIN_18};
adc_channel_t posChList[] = {ADC_CHANNEL8};
adc_channel_t negChList[] = {ADC_CHANNEL_GND};
adc_res_val_t result_val;

adc_keys_status key_machine;

/**
 * @brief init adc key machine
 * 
 */
static void key_machine_init(void){
    key_machine.vaildKeyNum = 0;
    memset(key_machine.keyValue,0xff,KEY_FILTER_NUM);
}

/**
 * @brief Get the adc value range object
 * 
 * @param Vmv input adc value ,unit is mV
 * @return uint16_t 
 */
static uint16_t get_adc_value_range(uint32_t Vmv){
    
    if(Vmv > KEY4_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY4_ADC_VALUE + KEY_MEASURE_OFFSET){
        return KEY4_ADC_VALUE;
    }else if(Vmv > KEY3_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY3_ADC_VALUE + KEY_MEASURE_OFFSET){
        return KEY3_ADC_VALUE;
    }else if(Vmv > KEY2_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY2_ADC_VALUE + KEY_MEASURE_OFFSET){
        return KEY2_ADC_VALUE;
    }else if(Vmv > KEY1_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY1_ADC_VALUE + KEY_MEASURE_OFFSET){
        return KEY1_ADC_VALUE;
    }else if(Vmv < KEY_MEASURE_OFFSET){
        return KEY0_ADC_VALUE;
    }else{
    	return KEY_NO_VALUE;
    }
}

/**
 * @brief Get the adc key value object
 * 
 * @param Vmv input adc value ,unit is mV
 * @return int 
 */
static int get_adc_key_value(uint32_t Vmv){
	volatile uint16_t key_num[5]={0};
    uint8_t bigger=0,i=0,j=0;

    if(Vmv>KEY0_ADC_VALUE && Vmv<KEY4_ADC_VALUE+KEY_MEASURE_OFFSET){
        key_machine.keyValue[key_machine.vaildKeyNum] = get_adc_value_range(Vmv);
        key_machine.vaildKeyNum++;

        if(key_machine.vaildKeyNum > KEY_FILTER_NUM){
            for(i=0;i<KEY_FILTER_NUM;i++){
                //Count the most Key Value
                switch(key_machine.keyValue[i]){
                    case KEY0_ADC_VALUE:  key_num[0]++; break;
                    case KEY1_ADC_VALUE:  key_num[1]++; break;
                    case KEY2_ADC_VALUE:  key_num[2]++; break;
                    case KEY3_ADC_VALUE:  key_num[3]++; break;
                    case KEY4_ADC_VALUE:  key_num[4]++; break;
                    default : break;
                }
            }
            for(i=0;i<5;i++){
            	for(j=0;j<5;j++){
            		if(key_num[i]>=key_num[j]){
            			bigger = 1;
            		}else{
            			bigger = 0;
            			break;
            		}
            	}
                if(bigger){
                	key_machine_init();
                	return i;
                }
            }

        }
    }
    return KEY_NO_VALUE;

}

int main(void)
{
    bflb_platform_init(0);
    uint16_t keyValue=0;

    adc_user_cfg_t adc_user_cfg;

    adc_user_cfg.dma_en = DISABLE;              
    adc_user_cfg.conv_mode = ADC_CON_CONV;      
    adc_user_cfg.in_mode = ADC_SINGLE_ENDED_IN;
    
    adc_user_cfg.pinList = PinList;
    adc_user_cfg.posChList = posChList;
    adc_user_cfg.negChList = negChList;
    adc_user_cfg.num = sizeof(posChList)/sizeof(adc_channel_t);

    adc_register(ADC0_INDEX, "adc_key", DEVICE_OFLAG_STREAM_RX, &adc_user_cfg);

    struct device* adc_test = device_find("adc_key");

    if(adc_test)
    {
        device_open(adc_test, DEVICE_OFLAG_STREAM_RX);
    }else{
        MSG("device open failed\r\n");
    }
    
    device_control(adc_test,DEVICE_CTRL_RESUME,0);

    key_machine_init();

    while (1)
    {
        device_read(adc_test,0,(void *)&result_val,1);
        keyValue = get_adc_key_value(result_val.volt * 1000);
        if( keyValue!=KEY_NO_VALUE){
            MSG("key %d pressed\r\n",keyValue);
        }     
    }
    
}