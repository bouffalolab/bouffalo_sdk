/**
 * @file hal_timer.c
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
#include "hal_timer.h"
#include "timer_config.h"
#include "bl702_glb.h"
#include "bl702_timer.h"

#define TIMER_MAX_VALUE (0xFFFFFFFF)
#define TIMER_CLK_DIV   (0)

void TIMER_CH0_IRQ(void);
void TIMER_CH1_IRQ(void);

static timer_device_t timerx_device[TIMER_MAX_INDEX] = 
{
#ifdef BSP_USING_TIMER_CH0
    TIMER_CH0_CONFIG,    
#endif
#ifdef BSP_USING_TIMER_CH1
    TIMER_CH1_CONFIG,
#endif
};

/**
 * @brief 
 * 
 * @param dev 
 * @param oflag 
 * @return int 
 */
int timer_open(struct device *dev, uint16_t oflag)
{
    timer_device_t *timer_device = (timer_device_t *)dev;
    TIMER_CFG_Type timer_cfg = {0};

    timer_cfg.timerCh = timer_device->ch;
    timer_cfg.clkSrc  = TIMER_CLK_SRC;
    timer_cfg.plTrigSrc = timer_device->pl_trig_src;
    timer_cfg.countMode = timer_device->cnt_mode;
    timer_cfg.clockDivision = TIMER_CLK_DIV;
    timer_cfg.matchVal0 = TIMER_MAX_VALUE;
    timer_cfg.matchVal1 = TIMER_MAX_VALUE;
    timer_cfg.matchVal2 = TIMER_MAX_VALUE;
    timer_cfg.preLoadVal = TIMER_MAX_VALUE;

    /* Disable all interrupt */
    TIMER_IntMask(timer_device->ch,TIMER_INT_ALL, MASK);
    /* Disable timer before config */
    TIMER_Disable(timer_device->ch);

    /* Timer init with default configuration */
    TIMER_Init(&timer_cfg);

    /* Clear interrupt status*/
    TIMER_ClearIntStatus(timer_device->ch,TIMER_COMP_ID_0);
    TIMER_ClearIntStatus(timer_device->ch,TIMER_COMP_ID_1);
    TIMER_ClearIntStatus(timer_device->ch,TIMER_COMP_ID_2);

#ifdef BSP_USING_TIMER_CH0
    if(oflag == DEVICE_OFLAG_INT)
        if( timer_device->ch == TIMER_CH0)
            Interrupt_Handler_Register(TIMER_CH0_IRQn, TIMER_CH0_IRQ);
#endif
#ifdef BSP_USING_TIMER_CH1
    if(oflag == DEVICE_OFLAG_INT)
        if( timer_device->ch == TIMER_CH1)
            Interrupt_Handler_Register(TIMER_CH1_IRQn, TIMER_CH1_IRQ);
#endif

    return 0;
}

/**
 * @brief 
 * 
 * @param dev 
 * @return int 
 */
int timer_close(struct device *dev)
{
    timer_device_t *timer_device = (timer_device_t *)(dev);
    TIMER_Disable(timer_device->ch);
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
int timer_control(struct device *dev, int cmd, void *args)
{
    timer_device_t* timer_device = (timer_device_t*)dev;
    switch (cmd)
    {
    case DEVICE_CTRL_SET_INT /* constant-expression */:
    {
        if(timer_device->ch == TIMER_CH0)
        {
            NVIC_ClearPendingIRQ(TIMER_CH0_IRQn);
            NVIC_EnableIRQ(TIMER_CH0_IRQn);
        }
        else if(timer_device->ch == TIMER_CH1)
        {
            NVIC_ClearPendingIRQ(TIMER_CH1_IRQn);
            NVIC_EnableIRQ(TIMER_CH1_IRQn);
        }
        break;
    }
    case DEVICE_CTRL_CLR_INT /* constant-expression */:
    {
        timer_user_cfg_t *timer_user_cfg = ((timer_user_cfg_t *)(args));
        uint32_t offset = __builtin_ctz((uint32_t)timer_user_cfg->comp_it);

        if(timer_device->ch == TIMER_CH0)
        {
            NVIC_DisableIRQ(TIMER_CH0_IRQn);
        }
        else if (timer_device->ch == TIMER_CH1)
        {
            NVIC_DisableIRQ(TIMER_CH1_IRQn);
        }
        while((0 <= offset) && (offset < 4))
        {
            if ((uint32_t)timer_user_cfg->comp_it & (1 << offset))
            {
                TIMER_SetCompValue(timer_device->ch,offset,TIMER_MAX_VALUE);
                TIMER_IntMask(timer_device->ch, offset, MASK);
            }
            offset ++;
        }
        break;
    }
    case DEVICE_CTRL_GET_INT /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_CONFIG /* constant-expression */:
        /* code */
        break;
    case DEVICE_CTRL_RESUME /* constant-expression */:
    {
        /* Enable timer */
        TIMER_Enable(timer_device->ch);
        break;
    }
    case DEVICE_CTRL_SUSPEND /* constant-expression */:
    {
        TIMER_Disable(timer_device->ch);
        break;
    }
    case DEVICE_CTRL_TIMER_CH_START:
    {
        timer_user_cfg_t *timer_user_cfg = ((timer_user_cfg_t *)(args));

        uint32_t offset = __builtin_ctz((uint32_t)timer_user_cfg->comp_it);
        uint32_t timeout = (timer_user_cfg->timeout_val * 144);
        TIMER_SetPreloadValue(timer_device->ch, 0);
        
        while((0 <= offset) && (offset < 4))
        {
            if((uint32_t)timer_user_cfg->comp_it & (1 << offset))
            {
                TIMER_SetCompValue(timer_device->ch,offset,timeout);
                TIMER_IntMask(timer_device->ch, offset, UNMASK);
            }
            offset ++;
        }
        /* Enable timer */
        TIMER_Enable(timer_device->ch);
        break;
    }
    case DEVICE_CTRL_TIMER_CH_STOP:
    {
        timer_user_cfg_t *timer_user_cfg = ((timer_user_cfg_t *)(args));
        uint32_t offset = __builtin_ctz((uint32_t)timer_user_cfg->comp_it);

        if(timer_device->ch == TIMER_CH0)
        {
            NVIC_DisableIRQ(TIMER_CH0_IRQn);
        }
        else if (timer_device->ch == TIMER_CH1)
        {
            NVIC_DisableIRQ(TIMER_CH1_IRQn);
        }
        while((0 <= offset) && (offset < 4))
        {
            if ((uint32_t)timer_user_cfg->comp_it & (1 << offset))
            {
                TIMER_SetCompValue(timer_device->ch,offset,TIMER_MAX_VALUE);
                TIMER_IntMask(timer_device->ch, offset, MASK);
            }
            offset ++;
        }
        TIMER_Disable(timer_device->ch);
        break;
    }
    case DEVICE_CTRL_GET_CONFIG:
        return TIMER_GetCounterValue(timer_device->ch);
        break;
    case DEVICE_CTRL_GET_MATCH_STATUS:
    {
        uint32_t tmpval = (uint32_t)args;
        return TIMER_GetMatchStatus(timer_device->ch, tmpval);
        break;
    }
    default:
        break;
    }

    return 0;
}


int timer_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    return 0;
}
int timer_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    return 0;
}

/**
 * @brief timer register
 * 
 * @param index 
 * @param name 
 * @param flag 
 * @param timer_user_cfg 
 * @return int 
 */

int timer_register(enum timer_index_type index, const char *name, uint16_t flag)
{
    struct device *dev;

    if(TIMER_MAX_INDEX == 0)
        return -DEVICE_EINVAL;

    dev = &(timerx_device[index].parent);

    dev->open = timer_open;
    dev->close = timer_close;
    dev->control = timer_control;
    // dev->write = NULL;
    // dev->read = NULL;
    
    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_TIMER;
    dev->handle = NULL;

    return device_register(dev, name, flag);
}


void timer_isr(timer_device_t *handle)
{
    uint32_t intId = 0;
    uint32_t tmpVal = 0;
    uint32_t tmpAddr = 0;

    intId = BL_RD_WORD(TIMER_BASE + TIMER_TMSR2_OFFSET + 4 * handle->ch);
    tmpAddr = TIMER_BASE + TIMER_TICR2_OFFSET + 4 * handle->ch;
    tmpVal = BL_RD_WORD(tmpAddr);

    if(!handle->parent.callback)
        return;

    /* Comparator 0 match interrupt */
    if( BL_IS_REG_BIT_SET(intId, TIMER_TMSR_0) ){
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_0));
        handle->parent.callback(&handle->parent, NULL, 0, TIMER_EVENT_COMP0);
    }

    /* Comparator 1 match interrupt */
    if( BL_IS_REG_BIT_SET(intId,TIMER_TMSR_1) ){
        BL_WR_WORD(tmpAddr,BL_SET_REG_BIT(tmpVal,TIMER_TCLR_1));
        handle->parent.callback(&handle->parent, NULL, 0, TIMER_EVENT_COMP1);
    }

    /* Comparator 2 match interrupt */
    if( BL_IS_REG_BIT_SET(intId,TIMER_TMSR_2) ){
        BL_WR_WORD(tmpAddr,BL_SET_REG_BIT(tmpVal,TIMER_TCLR_2));
        handle->parent.callback(&handle->parent, NULL, 0, TIMER_EVENT_COMP2);
    }
    
}

#ifdef BSP_USING_TIMER_CH0
/**
 * @brief 
 * 
 */
void TIMER_CH0_IRQ(void)
{
    timer_isr(&timerx_device[TIMER_CH0_INDEX]);
}

#endif


#ifdef BSP_USING_TIMER_CH1
/**
 * @brief 
 * 
 */
void TIMER_CH1_IRQ(void)
{
    timer_isr(&timerx_device[TIMER_CH1_INDEX]);
}

#endif

