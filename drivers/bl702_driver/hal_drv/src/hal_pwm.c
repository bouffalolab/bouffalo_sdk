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

static pwm_device_t pwmx_device[PWM_MAX_INDEX] = {
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
static void PWM_IRQ(void);

static void pwm_channel_config(uint8_t ch,uint32_t frequence,uint8_t dutycycle)
{
    PWM_CH_CFG_Type pwmCfg = {0};

    if(frequence <= 70)
    {
        pwmCfg.clkDiv = 1200;
        pwmCfg.period = 60000/frequence;
        pwmCfg.threshold2 = 600*dutycycle/frequence;
    }
    else if(frequence <= 140)
    {
        pwmCfg.clkDiv = 16;
        pwmCfg.period = 4500000/frequence;
        pwmCfg.threshold2 = 45000*dutycycle/frequence;
    }
    else if(frequence <= 275)
    {
        pwmCfg.clkDiv = 8;
        pwmCfg.period = 9000000/frequence;
        pwmCfg.threshold2 = 90000*dutycycle/frequence;        
    }
    else if(frequence <= 550)
    {
        pwmCfg.clkDiv = 4;
        pwmCfg.period = 18000000/frequence;
        pwmCfg.threshold2 = 180000*dutycycle/frequence;        
    }
    else if(frequence <= 1100)
    {
        pwmCfg.clkDiv = 2;
        pwmCfg.period = 36000000/frequence;
        pwmCfg.threshold2 = 360000*dutycycle/frequence;        
    }
    else
    {
        pwmCfg.clkDiv = 1;
        pwmCfg.period = 72000000/frequence;
        pwmCfg.threshold2 = 720000*dutycycle/frequence;
    }

    PWM_Channel_Set_Div(ch,pwmCfg.clkDiv);
    PWM_Channel_Update(ch,pwmCfg.period,0,pwmCfg.threshold2);
    
}

int pwm_open(struct device *dev, uint16_t oflag)
{
    pwm_device_t *pwm_device = (pwm_device_t *)dev;

    PWM_CH_CFG_Type pwmCfg = {0};

    PWM_Channel_Disable(pwm_device->ch);
    PWM_IntMask(pwm_device->ch,PWM_INT_ALL,MASK);
    NVIC_DisableIRQ(PWM_IRQn);

    pwmCfg.ch = pwm_device->ch;
    /* todo clk init at clock tree now clk fix set as bclk*/
    pwmCfg.clk = PWM_CLK_BCLK;
    pwmCfg.stopMode = PWM_STOP_MODE_SEL;
    pwmCfg.pol = PWM_POL_SEL;

    PWM_Channel_Init(&pwmCfg);
    pwm_channel_config(pwm_device->ch,pwm_device->frequency,pwm_device->dutycycle);

    if (oflag & DEVICE_OFLAG_INT_TX)
    {
        pwmCfg.intPulseCnt = pwm_device->it_pulse_count;
        Interrupt_Handler_Register(PWM_IRQn,PWM_IRQ);
        PWM_IntMask(pwm_device->ch,PWM_INT_PULSE_CNT,UNMASK);
        NVIC_EnableIRQ(PWM_IRQn);
    }

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
    switch (cmd)
    {
    case DEVICE_CTRL_CONFIG/* constant-expression */:
        pwm_channel_config(pwm_device->ch,pwm_config->frequency,pwm_config->dutycycle);
        break;
    case DEVICE_CTRL_RESUME /* constant-expression */:
        PWM_Channel_Enable(pwm_device->ch);
        break;
    case DEVICE_CTRL_SUSPEND /* constant-expression */:
        PWM_Channel_Disable(pwm_device->ch);
        break;    
    case DEIVCE_CTRL_PWM_IT_PULSE_COUNT_CONFIG:
    {
        /* Config interrupt pulse count */
        uint32_t pwm_ch_addr = PWM_BASE+PWM_CHANNEL_OFFSET+(pwm_device->ch)*0x20;
        uint32_t tmpVal = BL_RD_REG(pwm_ch_addr, PWM_INTERRUPT);
        BL_WR_REG(pwm_ch_addr, PWM_INTERRUPT, BL_SET_REG_BITS_VAL(tmpVal, PWM_INT_PERIOD_CNT, (uint32_t)args));

        if((uint32_t)args)
        {
            PWM_IntMask(pwm_device->ch,PWM_INT_PULSE_CNT,UNMASK);
            NVIC_EnableIRQ(PWM_IRQn);
        }
        else
        {
            PWM_IntMask(pwm_device->ch,PWM_INT_PULSE_CNT,MASK);
            NVIC_DisableIRQ(PWM_IRQn);
        }
        break;    
    }
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

static void pwm_isr(pwm_device_t *handle)
{
    uint32_t i;
    uint32_t tmpVal;
    uint32_t timeoutCnt = 160*1000;
    /* Get channel register */
    uint32_t PWMx = PWM_BASE;
    
    for (i = 0; i < PWM_MAX_INDEX; i++) {
        tmpVal = BL_RD_REG(PWMx, PWM_INT_CONFIG);
        if ((BL_GET_REG_BITS_VAL(tmpVal, PWM_INTERRUPT_STS) & (1 << handle[i].ch)) != 0) {
            /* Clear interrupt */
            tmpVal |= (1 << (handle[i].ch + PWM_INT_CLEAR_POS));
            BL_WR_REG(PWMx, PWM_INT_CONFIG, tmpVal);
            /* FIXME: we need set pwm_int_clear to 0 by software and 
               before this,we must make sure pwm_interrupt_sts is 0*/
            do{
                tmpVal = BL_RD_REG(PWMx, PWM_INT_CONFIG);
                timeoutCnt--;
                if(timeoutCnt == 0){
                    break;
                }
            }while(BL_GET_REG_BITS_VAL(tmpVal,PWM_INTERRUPT_STS)&(1 << handle[i].ch));
            
            tmpVal &= (~(1 << (handle[i].ch + PWM_INT_CLEAR_POS)));
            BL_WR_REG(PWMx, PWM_INT_CONFIG, tmpVal);

            if (handle[i].parent.callback)
            {
                handle[i].parent.callback(&handle[i].parent, NULL, 0, PWM_EVENT_COMPLETE);
            }
                
        }
    }
}

static void PWM_IRQ(void)
{
    pwm_isr(&pwmx_device[0]);
}