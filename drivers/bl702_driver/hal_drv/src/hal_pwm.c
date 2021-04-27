/**
 * @file hal_pwm.c
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
#include "hal_pwm.h"
#include "hal_gpio.h"
#include "bl702_pwm.h"
#include "bl702_gpio.h"
#include "bl702_glb.h"
#include "pwm_config.h"

pwm_device_t pwmx_device[PWM_MAX_INDEX] = {
#ifdef BSP_USING_PWM_CH0
    PWM_CH0_CONFIG,
#endif
#ifdef BSP_USING_PWM_CH1
    PWM_CH1_CONFIG,
#endif
#ifdef BSP_USING_PWM_CH2
    PWM_CH2_CONFIG,
#endif
#ifdef BSP_USING_PWM_CH3
    PWM_CH3_CONFIG,
#endif
#ifdef BSP_USING_PWM_CH4
    PWM_CH4_CONFIG,
#endif
};

typedef enum{
    HZ=0,
    KHZ,
    MHZ,
}frequency_unit_t;

static void pwm_set(struct device *dev, PWM_CH_CFG_Type * pwmCfg ,uint32_t freq ,frequency_unit_t unit, uint8_t duty){
    
    pwm_device_t *pwm_device = (pwm_device_t *)dev;
    
    switch(unit)
    {
    case HZ:
        if(freq <= 70){
            pwmCfg->clkDiv = 1200;
            pwmCfg->period = 60000/freq;
            pwmCfg->threshold2 = 600*duty/freq;
        }else if(freq <= 140){
            pwmCfg->clkDiv = 16;
            pwmCfg->period = 4500000/freq;
            pwmCfg->threshold2 = 45000*duty/freq;
        }else if(freq <= 275){
            pwmCfg->clkDiv = 8;
            pwmCfg->period = 9000000/freq;
            pwmCfg->threshold2 = 90000*duty/freq;
        }else if(freq <=550){
            pwmCfg->clkDiv = 4;
            pwmCfg->period = 18000000/freq;
            pwmCfg->threshold2 = 180000*duty/freq;
        }else if(freq <=1100){
            pwmCfg->clkDiv = 2;
            pwmCfg->period = 36000000/freq;
            pwmCfg->threshold2 = 360000*duty/freq;
        }else{
            pwmCfg->clkDiv = 1;
            pwmCfg->period = 72000000/freq;
            pwmCfg->threshold2 = 720000*duty/freq;
        }
        break;
    case KHZ:
        if(freq <= 2){
            pwmCfg->clkDiv = 2;
            pwmCfg->period = 36000/freq;
            pwmCfg->threshold2 = 360*duty/freq;
        }else{
            pwmCfg->clkDiv = 1;
            pwmCfg->period = 72000/freq;
            pwmCfg->threshold2 = 720*duty/freq;
        }
        break;
    case MHZ:
        pwmCfg->clkDiv = 1;
        pwmCfg->period = 72/freq;
        pwmCfg->threshold2 = 72*duty/freq/100;
        break;
    default:
        break;
        }

    PWM_Channel_Set_Div(pwm_device->ch  ,pwmCfg->clkDiv);
    PWM_Channel_Update(pwm_device->ch  ,pwmCfg->period,0,pwmCfg->threshold2);
    
}

int pwm_open(struct device *dev, uint16_t oflag)
{
    PWM_CH_CFG_Type pwmCfg = {
		0,                                   			     /* PWM channel */
		PWM_CLK_BCLK,                                        /* PWM Clock */
		PWM_STOP_GRACEFUL,                                   /* PWM Stop Mode */
		PWM_POL_NORMAL,                                      /* PWM mode type */
		1,                                                   /* PWM clkDiv num */
		100,                                                 /* PWM period set */
		0,                                                   /* PWM threshold1 num */
		50,                                                  /* PWM threshold2 num */
		0,                                                   /* PWM interrupt pulse count */
	};
    pwm_device_t *pwm_device = (pwm_device_t *)dev;

    pwmCfg.ch = pwm_device->ch;
    /* todo clk init at clock tree now clk fix set as bclk*/
    pwmCfg.clk = PWM_CLK_BCLK;

    pwmCfg.stopMode = PWM_STOP_MODE_SEL;
    pwmCfg.pol = PWM_POL_SEL;


    if(pwm_device->frequency < 1000){
        pwm_set(dev,&pwmCfg,pwm_device->frequency,HZ,pwm_device->dutyCycle);
    }else if(pwm_device->frequency < 999999){
        pwm_set(dev,&pwmCfg,pwm_device->frequency /1000,KHZ,pwm_device->dutyCycle);
    }else{
        pwm_set(dev,&pwmCfg,pwm_device->frequency /1000000,MHZ,pwm_device->dutyCycle);
    }
	
    PWM_Channel_Init(&pwmCfg);
    PWM_Channel_Enable(pwm_device->ch);
    return 0;
}
int pwm_close(struct device *dev)
{
    pwm_device_t *pwm_device = (pwm_device_t *)dev;
    PWM_Channel_Disable(pwm_device->ch);
    return 0;
}

int pwm_control(struct device *dev, int cmd, void *args)
{
    pwm_device_t *pwm_device = (pwm_device_t *)dev;
    pwm_config_t *pwm_config = (pwm_config_t *)args;
    PWM_CH_CFG_Type pwmCfg;
    switch (cmd)
    {
    case DEVICE_CTRL_CONFIG/* constant-expression */:
        if(pwm_device->frequency < 1000){
            pwm_set(dev,&pwmCfg,pwm_config->frequency,HZ,pwm_config->dutyCycle);
        }else if(pwm_device->frequency < 999999){
            pwm_set(dev,&pwmCfg,pwm_config->frequency /1000,KHZ,pwm_config->dutyCycle);
        }else{
            pwm_set(dev,&pwmCfg,pwm_config->frequency /1000000,MHZ,pwm_config->dutyCycle);
        }
        break;
    case DEVICE_CTRL_RESUME /* constant-expression */:
        PWM_Channel_Enable(pwm_device->ch);
        break;
    case DEVICE_CTRL_SUSPEND /* constant-expression */:
        PWM_Channel_Disable(pwm_device->ch);
        break;    
    default:
        break;
    }

    return 0;
}


int pwm_register(enum pwm_index_type index, const char *name, uint16_t flag)
{
    struct device *dev;

    if (PWM_MAX_INDEX == 0)
        return -DEVICE_EINVAL;

    dev = &(pwmx_device[index].parent);

    dev->open = pwm_open;
    dev->close = pwm_close;
    dev->control = pwm_control;
    dev->write = NULL;
    dev->read = NULL;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_PWM;
    dev->handle = NULL;

    return device_register(dev, name, flag);
}
