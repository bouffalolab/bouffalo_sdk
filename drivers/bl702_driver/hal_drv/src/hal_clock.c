/**
 * @file hal_clock.c
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

#include "bl702_glb.h"
#include "hal_clock.h"

void system_clock_init(void)
{
    /*select root clock*/
    GLB_Set_System_CLK(CLOCK_XTAL,BSP_ROOT_CLOCK_SOURCE-2);
    /*set fclk/hclk and bclk clock*/
    GLB_Set_System_CLK_Div(BSP_HCLK_DIV,BSP_BCLK_DIV);
    /* Set MTimer the same frequency as SystemCoreClock */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 7);
}
void peripheral_clock_init(void)
{
#if defined(BSP_USING_UART0)||defined(BSP_USING_UART1)
#if BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_96M
    GLB_Set_UART_CLK(ENABLE,HBN_UART_CLK_96M,BSP_UART_CLOCK_DIV);
#elif BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_FCLK
    GLB_Set_UART_CLK(ENABLE,HBN_UART_CLK_FCLK,BSP_UART_CLOCK_DIV);
#endif
#endif

#if defined(BSP_USING_I2C0)
    GLB_Set_I2C_CLK(ENABLE,BSP_I2C_CLOCK_DIV);
#endif

#if defined(BSP_USING_SPI0)
    GLB_Set_SPI_CLK(ENABLE,BSP_SPI_CLOCK_DIV);
#endif

#if defined(BSP_USING_PWM)
#if BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_RC_32K
  
#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_BCLK

#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_XCLK

#endif
#endif

#if defined(BSP_USING_IR)
    GLB_Set_IR_CLK(ENABLE,0,BSP_IR_CLOCK_DIV);
#endif

#if defined(BSP_USING_I2S0)
    GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
    GLB_Set_I2S_CLK(ENABLE,GLB_I2S_OUT_REF_CLK_SRC);
#endif
#if defined(BSP_USING_ADC0)
#if BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL
    GLB_Set_ADC_CLK(ENABLE,GLB_ADC_CLK_AUDIO_PLL,BSP_ADC_CLOCK_DIV);
#elif BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_XCLK
    GLB_Set_ADC_CLK(ENABLE,GLB_ADC_CLK_XCLK,BSP_ADC_CLOCK_DIV);
#endif
    
#endif

#if defined(BSP_USING_DAC0)
#if BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL
    GLB_Set_DAC_CLK(ENABLE,GLB_ADC_CLK_AUDIO_PLL,BSP_DAC_CLOCK_DIV);
#elif BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_XCLK
    GLB_Set_DAC_CLK(ENABLE,GLB_ADC_CLK_XCLK,BSP_DAC_CLOCK_DIV);
#endif
#endif

#if defined(BSP_USING_USB)
    GLB_Set_USB_CLK(1);
#endif

}
uint32_t system_clock_get(enum system_clock_type type)
{
    switch (type)
    {
    case SYSTEM_CLOCK_ROOT_CLOCK:
        return SystemCoreClockGet();
    case SYSTEM_CLOCK_FCLK:
        return (SystemCoreClockGet()/(GLB_Get_HCLK_Div()+1)); 
    case SYSTEM_CLOCK_BCLK:
        return (SystemCoreClockGet()/((GLB_Get_HCLK_Div()+1)*(GLB_Get_BCLK_Div()+1)));
    case SYSTEM_CLOCK_XCLK:
        return 32000000;   
    default:
        break;
    }
    return 0;
}
uint32_t peripheral_clock_get(enum peripheral_clock_type type)
{
    uint32_t tmpVal;
    uint32_t div;
    switch (type)
    {
    case PERIPHERAL_CLOCK_UART:
    #if defined(BSP_USING_UART0)||defined(BSP_USING_UART1)
    #if BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_96M
        return 96000000;
    #elif BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_FCLK
        return system_clock_get(SYSTEM_CLOCK_FCLK)/(GLB_Get_HCLK_Div()+1));
    #endif
    #endif
    case PERIPHERAL_CLOCK_SPI:
#if defined(BSP_USING_SPI0)
#if BSP_SPI_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_BCLK
        tmpVal=BL_RD_REG(GLB_BASE,GLB_CLK_CFG3);
        div = BL_GET_REG_BITS_VAL(tmpVal,GLB_SPI_CLK_DIV);
        return system_clock_get(SYSTEM_CLOCK_BCLK)/(div+1);
#endif
#endif
    case PERIPHERAL_CLOCK_I2C:
#if defined(BSP_USING_I2C0)
#if BSP_I2C_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_DLL_BCLK
        tmpVal=BL_RD_REG(GLB_BASE,GLB_CLK_CFG3);
        div = BL_GET_REG_BITS_VAL(tmpVal,GLB_I2C_CLK_DIV);
        return system_clock_get(SYSTEM_CLOCK_BCLK)/(div+1);
#endif
#endif
    case PERIPHERAL_CLOCK_I2S:
        return 0;
    case PERIPHERAL_CLOCK_ADC:
        return 32000000;   
    default:
        break;
    }
    return 0;
}