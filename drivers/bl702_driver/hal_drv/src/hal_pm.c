/**
 * @file hal_pm.c
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
#include "bl702_hbn.h"
#include "bl702_clock.h"

#include "hal_pm.h"

#define PM_HBN_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_0P90V

static void HBN_OUT0_IRQ(void);
static void HBN_OUT1_IRQ(void);

static void (*hbn_irq_callback)(uint8_t event) = NULL;

/**
 * @brief
 *
 * power manage in hbn mode
 *
 * HNB_LEVEL   PD_AON       PD_AON_HNBRTC        PD_AON_HBNCORE        PD_CORE     PD_CORE_MISC_DIG    PD_CORE_MISC_ANA    PD_CORE_CPU     PD_BZ   PD_USB
 * HBN0          ON               ON                  ON                OFF            OFF                  OFF                 OFF         OFF     OFF
 * HBN1          ON               ON                  OFF               OFF            OFF                  OFF                 OFF         OFF     OFF
 * HBN2          ON               OFF                 OFF               OFF            OFF                  OFF                 OFF         OFF     OFF
 * @param hbn_level
 */
ATTR_TCM_SECTION void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level)
{
    uint32_t tmpVal;

    __disable_irq();

    for (GLB_GPIO_Type pin = GLB_GPIO_PIN_0; pin < GLB_GPIO_PIN_MAX; pin++) {
        if (pin == GLB_GPIO_PIN_10 || pin == GLB_GPIO_PIN_11 || pin == GLB_GPIO_PIN_12)
            continue;
        GLB_GPIO_Set_HZ(pin);
    }

    HBN_Clear_IRQ(HBN_INT_GPIO9);
    HBN_Clear_IRQ(HBN_INT_GPIO13);

    if (hbn_level >= PM_HBN_LEVEL_2)
        HBN_Power_Off_RC32K();
    else
        HBN_Power_On_RC32K();

    /* always disable HBN pin pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(DISABLE);

    HBN_Power_Down_Flash(NULL);
#if 1
    HBN_Set_Pad_23_28_Pullup();
#endif
    GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);
    /* power off xtal */
    AON_Power_Off_XTAL();

    /* HBN mode LDO level */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, PM_HBN_LDO_LEVEL_DEFAULT);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_RT_VOUT_SEL, PM_HBN_LDO_LEVEL_DEFAULT);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Select RC32M */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);

    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbn_level) {
        case PM_HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case PM_HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case PM_HBN_LEVEL_2:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;
        default:
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Enable HBN mode */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    while (1) {
        BL702_Delay_MS(1000);
    }
}

void pm_hbn_irq_register(void (*hbn_irq)(uint8_t event))
{
    hbn_irq_callback = hbn_irq;
    Interrupt_Handler_Register(HBN_OUT0_IRQn, HBN_OUT0_IRQ);
    Interrupt_Handler_Register(HBN_OUT1_IRQn, HBN_OUT1_IRQ);
    NVIC_EnableIRQ(HBN_OUT0_IRQn);
    NVIC_EnableIRQ(HBN_OUT1_IRQn);
}

static void HBN_OUT0_IRQ(void)
{
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO10)) {
        HBN_Clear_IRQ(HBN_INT_GPIO10);
        hbn_irq_callback(PM_HBN_GPIO10_WAKEUP_EVENT);
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO11)) {
        HBN_Clear_IRQ(HBN_INT_GPIO11);
        hbn_irq_callback(PM_HBN_GPIO11_WAKEUP_EVENT);
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO12)) {
        HBN_Clear_IRQ(HBN_INT_GPIO12);
        hbn_irq_callback(PM_HBN_GPIO12_WAKEUP_EVENT);
    }

    if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
        HBN_Clear_IRQ(HBN_INT_RTC);
        HBN_Clear_RTC_INT();
        hbn_irq_callback(PM_HBN_RTC_WAKEUP_EVENT);
    }
}

static void HBN_OUT1_IRQ(void)
{
    /* PIR */
    if (SET == HBN_Get_INT_State(HBN_INT_PIR)) {
        HBN_Clear_IRQ(HBN_INT_PIR);
    }

    /* BOR */
    if (SET == HBN_Get_INT_State(HBN_INT_BOR)) {
        HBN_Clear_IRQ(HBN_INT_BOR);
    }

    /* ACOMP0 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP0)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP0);
        hbn_irq_callback(PM_HBN_ACOMP0_WAKEUP_EVENT);
    }

    /* ACOMP1 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP1)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP1);
        hbn_irq_callback(PM_HBN_ACOMP1_WAKEUP_EVENT);
    }
}