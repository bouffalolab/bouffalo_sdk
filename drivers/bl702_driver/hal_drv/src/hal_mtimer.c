/**
 * @file hal_mtimer.c
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
#include "hal_mtimer.h"
#include "bl702_glb.h"
/**
 * @brief 
 * 
 */
void mtimer_init(void)
{
    NVIC_DisableIRQ(MTIME_IRQn);
    /* Set MTimer the same frequency as SystemCoreClock */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 7);
    *(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIME) = 0;
}
/**
 * @brief 
 * 
 */
void mtimer_deinit()
{
    NVIC_DisableIRQ(MTIME_IRQn);
    mtimer_stop();
}
/**
 * @brief 
 * 
 * @param time 
 * @param interruptFun 
 */
void mtimer_set_alarm_time(uint64_t time, void (*interruptFun)(void))
{
    uint32_t tmp;

    tmp = (SystemCoreClockGet() / (GLB_Get_BCLK_Div() + 1));
    tmp = (tmp >> 3) / 1000;

    time = time * tmp;

    *(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIMECMP) = (*(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIME) + time);

    Interrupt_Handler_Register(MTIME_IRQn, interruptFun);
    NVIC_EnableIRQ(MTIME_IRQn);
}
/**
 * @brief 
 * 
 */
void mtimer_start()
{
    *(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIME) = 0;
}
/**
 * @brief 
 * 
 */
void mtimer_stop()
{
}
/**
 * @brief 
 * 
 */
void mtimer_clear_time()
{
    *(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIME) = 0;
}

/**
 * @brief 
 * 
 * @return uint64_t 
 */
uint64_t mtimer_get_time_ms()
{
    return mtimer_get_time_us()/1000;
}
/**
 * @brief 
 * 
 * @return uint64_t 
 */
uint64_t mtimer_get_time_us()
{

    uint32_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint32_t cnt = 0, tmp;

    do
    {
        tmpValLow = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME);
        tmpValHigh = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4);
        tmpValLow1 = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME);
        tmpValHigh1 = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4);
        cnt++;
        if (cnt > 4)
        {
            break;
        }
    } while (tmpValLow > tmpValLow1 || tmpValHigh > tmpValHigh1);

    tmp = (SystemCoreClockGet() / (GLB_Get_BCLK_Div() + 1));
    tmp = (tmp >> 3) / 1000 / 1000;

    if (tmpValHigh1 == 0)
    {
        return (uint64_t)(tmpValLow1 / tmp);
    }
    else
    {
        return (((uint64_t)tmpValHigh1 << 32) + tmpValLow1) / tmp;
    }
}
/**
 * @brief 
 * 
 * @param time 
 */
void mtimer_delay_ms(uint32_t time)
{
    uint64_t cnt = 0;
    uint32_t clock = SystemCoreClockGet();
    uint64_t startTime = mtimer_get_time_ms();

    while (mtimer_get_time_ms() - startTime < time)
    {
        cnt++;
        /* assume BFLB_BSP_Get_Time_Ms take 32 cycles*/
        if (cnt > (time * (clock >> (10 + 5))) * 2)
        {
            break;
        }
    }
}
/**
 * @brief 
 * 
 * @param time 
 */
void mtimer_delay_us(uint32_t time)
{
    uint64_t cnt = 0;
    uint32_t clock = SystemCoreClockGet();
    uint64_t startTime = mtimer_get_time_us();

    while (mtimer_get_time_us() - startTime < time)
    {
        cnt++;
        /* assume BFLB_BSP_Get_Time_Ms take 32 cycles*/
        if (cnt > (time * (clock >> (10 + 5))) * 2)
        {
            break;
        }
    }
}

