/**
  ******************************************************************************
  * @file    hbn_reg.h
  * @version V1.0
  * @date    2022-02-15
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __HBN_REG_H__
#define __HBN_REG_H__

#include "bl808.h"

/* 0x0 : HBN_CTL */
#define HBN_CTL_OFFSET                (0x0)
#define HBN_RTC_CTL                   HBN_RTC_CTL
#define HBN_RTC_CTL_POS               (0U)
#define HBN_RTC_CTL_LEN               (4U)
#define HBN_RTC_CTL_MSK               (((1U << HBN_RTC_CTL_LEN) - 1) << HBN_RTC_CTL_POS)
#define HBN_RTC_CTL_UMSK              (~(((1U << HBN_RTC_CTL_LEN) - 1) << HBN_RTC_CTL_POS))
#define HBN_RTC_DLY_OPTION            HBN_RTC_DLY_OPTION
#define HBN_RTC_DLY_OPTION_POS        (4U)
#define HBN_RTC_DLY_OPTION_LEN        (1U)
#define HBN_RTC_DLY_OPTION_MSK        (((1U << HBN_RTC_DLY_OPTION_LEN) - 1) << HBN_RTC_DLY_OPTION_POS)
#define HBN_RTC_DLY_OPTION_UMSK       (~(((1U << HBN_RTC_DLY_OPTION_LEN) - 1) << HBN_RTC_DLY_OPTION_POS))
#define HBN_PU_LDO18IO_AON            HBN_PU_LDO18IO_AON
#define HBN_PU_LDO18IO_AON_POS        (5U)
#define HBN_PU_LDO18IO_AON_LEN        (1U)
#define HBN_PU_LDO18IO_AON_MSK        (((1U << HBN_PU_LDO18IO_AON_LEN) - 1) << HBN_PU_LDO18IO_AON_POS)
#define HBN_PU_LDO18IO_AON_UMSK       (~(((1U << HBN_PU_LDO18IO_AON_LEN) - 1) << HBN_PU_LDO18IO_AON_POS))
#define HBN_MODE                      HBN_MODE
#define HBN_MODE_POS                  (7U)
#define HBN_MODE_LEN                  (1U)
#define HBN_MODE_MSK                  (((1U << HBN_MODE_LEN) - 1) << HBN_MODE_POS)
#define HBN_MODE_UMSK                 (~(((1U << HBN_MODE_LEN) - 1) << HBN_MODE_POS))
#define HBN_TRAP_MODE                 HBN_TRAP_MODE
#define HBN_TRAP_MODE_POS             (8U)
#define HBN_TRAP_MODE_LEN             (1U)
#define HBN_TRAP_MODE_MSK             (((1U << HBN_TRAP_MODE_LEN) - 1) << HBN_TRAP_MODE_POS)
#define HBN_TRAP_MODE_UMSK            (~(((1U << HBN_TRAP_MODE_LEN) - 1) << HBN_TRAP_MODE_POS))
#define HBN_PWRDN_HBN_CORE            HBN_PWRDN_HBN_CORE
#define HBN_PWRDN_HBN_CORE_POS        (9U)
#define HBN_PWRDN_HBN_CORE_LEN        (1U)
#define HBN_PWRDN_HBN_CORE_MSK        (((1U << HBN_PWRDN_HBN_CORE_LEN) - 1) << HBN_PWRDN_HBN_CORE_POS)
#define HBN_PWRDN_HBN_CORE_UMSK       (~(((1U << HBN_PWRDN_HBN_CORE_LEN) - 1) << HBN_PWRDN_HBN_CORE_POS))
#define HBN_SW_RST                    HBN_SW_RST
#define HBN_SW_RST_POS                (12U)
#define HBN_SW_RST_LEN                (1U)
#define HBN_SW_RST_MSK                (((1U << HBN_SW_RST_LEN) - 1) << HBN_SW_RST_POS)
#define HBN_SW_RST_UMSK               (~(((1U << HBN_SW_RST_LEN) - 1) << HBN_SW_RST_POS))
#define HBN_DIS_PWR_OFF_LDO11         HBN_DIS_PWR_OFF_LDO11
#define HBN_DIS_PWR_OFF_LDO11_POS     (13U)
#define HBN_DIS_PWR_OFF_LDO11_LEN     (1U)
#define HBN_DIS_PWR_OFF_LDO11_MSK     (((1U << HBN_DIS_PWR_OFF_LDO11_LEN) - 1) << HBN_DIS_PWR_OFF_LDO11_POS)
#define HBN_DIS_PWR_OFF_LDO11_UMSK    (~(((1U << HBN_DIS_PWR_OFF_LDO11_LEN) - 1) << HBN_DIS_PWR_OFF_LDO11_POS))
#define HBN_DIS_PWR_OFF_LDO11_RT      HBN_DIS_PWR_OFF_LDO11_RT
#define HBN_DIS_PWR_OFF_LDO11_RT_POS  (14U)
#define HBN_DIS_PWR_OFF_LDO11_RT_LEN  (1U)
#define HBN_DIS_PWR_OFF_LDO11_RT_MSK  (((1U << HBN_DIS_PWR_OFF_LDO11_RT_LEN) - 1) << HBN_DIS_PWR_OFF_LDO11_RT_POS)
#define HBN_DIS_PWR_OFF_LDO11_RT_UMSK (~(((1U << HBN_DIS_PWR_OFF_LDO11_RT_LEN) - 1) << HBN_DIS_PWR_OFF_LDO11_RT_POS))
#define HBN_LDO11_RT_VOUT_SEL         HBN_LDO11_RT_VOUT_SEL
#define HBN_LDO11_RT_VOUT_SEL_POS     (15U)
#define HBN_LDO11_RT_VOUT_SEL_LEN     (4U)
#define HBN_LDO11_RT_VOUT_SEL_MSK     (((1U << HBN_LDO11_RT_VOUT_SEL_LEN) - 1) << HBN_LDO11_RT_VOUT_SEL_POS)
#define HBN_LDO11_RT_VOUT_SEL_UMSK    (~(((1U << HBN_LDO11_RT_VOUT_SEL_LEN) - 1) << HBN_LDO11_RT_VOUT_SEL_POS))
#define HBN_LDO11_AON_VOUT_SEL        HBN_LDO11_AON_VOUT_SEL
#define HBN_LDO11_AON_VOUT_SEL_POS    (19U)
#define HBN_LDO11_AON_VOUT_SEL_LEN    (4U)
#define HBN_LDO11_AON_VOUT_SEL_MSK    (((1U << HBN_LDO11_AON_VOUT_SEL_LEN) - 1) << HBN_LDO11_AON_VOUT_SEL_POS)
#define HBN_LDO11_AON_VOUT_SEL_UMSK   (~(((1U << HBN_LDO11_AON_VOUT_SEL_LEN) - 1) << HBN_LDO11_AON_VOUT_SEL_POS))
#define HBN_PU_DCDC_AON               HBN_PU_DCDC_AON
#define HBN_PU_DCDC_AON_POS           (23U)
#define HBN_PU_DCDC_AON_LEN           (1U)
#define HBN_PU_DCDC_AON_MSK           (((1U << HBN_PU_DCDC_AON_LEN) - 1) << HBN_PU_DCDC_AON_POS)
#define HBN_PU_DCDC_AON_UMSK          (~(((1U << HBN_PU_DCDC_AON_LEN) - 1) << HBN_PU_DCDC_AON_POS))
#define HBN_PU_DCDC18_AON             HBN_PU_DCDC18_AON
#define HBN_PU_DCDC18_AON_POS         (24U)
#define HBN_PU_DCDC18_AON_LEN         (1U)
#define HBN_PU_DCDC18_AON_MSK         (((1U << HBN_PU_DCDC18_AON_LEN) - 1) << HBN_PU_DCDC18_AON_POS)
#define HBN_PU_DCDC18_AON_UMSK        (~(((1U << HBN_PU_DCDC18_AON_LEN) - 1) << HBN_PU_DCDC18_AON_POS))
#define HBN_PWR_ON_OPTION             HBN_PWR_ON_OPTION
#define HBN_PWR_ON_OPTION_POS         (25U)
#define HBN_PWR_ON_OPTION_LEN         (1U)
#define HBN_PWR_ON_OPTION_MSK         (((1U << HBN_PWR_ON_OPTION_LEN) - 1) << HBN_PWR_ON_OPTION_POS)
#define HBN_PWR_ON_OPTION_UMSK        (~(((1U << HBN_PWR_ON_OPTION_LEN) - 1) << HBN_PWR_ON_OPTION_POS))
#define HBN_SRAM_SLP_OPTION           HBN_SRAM_SLP_OPTION
#define HBN_SRAM_SLP_OPTION_POS       (26U)
#define HBN_SRAM_SLP_OPTION_LEN       (1U)
#define HBN_SRAM_SLP_OPTION_MSK       (((1U << HBN_SRAM_SLP_OPTION_LEN) - 1) << HBN_SRAM_SLP_OPTION_POS)
#define HBN_SRAM_SLP_OPTION_UMSK      (~(((1U << HBN_SRAM_SLP_OPTION_LEN) - 1) << HBN_SRAM_SLP_OPTION_POS))
#define HBN_SRAM_SLP                  HBN_SRAM_SLP
#define HBN_SRAM_SLP_POS              (27U)
#define HBN_SRAM_SLP_LEN              (1U)
#define HBN_SRAM_SLP_MSK              (((1U << HBN_SRAM_SLP_LEN) - 1) << HBN_SRAM_SLP_POS)
#define HBN_SRAM_SLP_UMSK             (~(((1U << HBN_SRAM_SLP_LEN) - 1) << HBN_SRAM_SLP_POS))
#define HBN_STATE                     HBN_STATE
#define HBN_STATE_POS                 (28U)
#define HBN_STATE_LEN                 (4U)
#define HBN_STATE_MSK                 (((1U << HBN_STATE_LEN) - 1) << HBN_STATE_POS)
#define HBN_STATE_UMSK                (~(((1U << HBN_STATE_LEN) - 1) << HBN_STATE_POS))

/* 0x4 : HBN_TIME_L */
#define HBN_TIME_L_OFFSET (0x4)
#define HBN_TIME_L        HBN_TIME_L
#define HBN_TIME_L_POS    (0U)
#define HBN_TIME_L_LEN    (32U)
#define HBN_TIME_L_MSK    (((1U << HBN_TIME_L_LEN) - 1) << HBN_TIME_L_POS)
#define HBN_TIME_L_UMSK   (~(((1U << HBN_TIME_L_LEN) - 1) << HBN_TIME_L_POS))

/* 0x8 : HBN_TIME_H */
#define HBN_TIME_H_OFFSET (0x8)
#define HBN_TIME_H        HBN_TIME_H
#define HBN_TIME_H_POS    (0U)
#define HBN_TIME_H_LEN    (8U)
#define HBN_TIME_H_MSK    (((1U << HBN_TIME_H_LEN) - 1) << HBN_TIME_H_POS)
#define HBN_TIME_H_UMSK   (~(((1U << HBN_TIME_H_LEN) - 1) << HBN_TIME_H_POS))

/* 0xC : RTC_TIME_L */
#define HBN_RTC_TIME_L_OFFSET     (0xC)
#define HBN_RTC_TIME_LATCH_L      HBN_RTC_TIME_LATCH_L
#define HBN_RTC_TIME_LATCH_L_POS  (0U)
#define HBN_RTC_TIME_LATCH_L_LEN  (32U)
#define HBN_RTC_TIME_LATCH_L_MSK  (((1U << HBN_RTC_TIME_LATCH_L_LEN) - 1) << HBN_RTC_TIME_LATCH_L_POS)
#define HBN_RTC_TIME_LATCH_L_UMSK (~(((1U << HBN_RTC_TIME_LATCH_L_LEN) - 1) << HBN_RTC_TIME_LATCH_L_POS))

/* 0x10 : RTC_TIME_H */
#define HBN_RTC_TIME_H_OFFSET     (0x10)
#define HBN_RTC_TIME_LATCH_H      HBN_RTC_TIME_LATCH_H
#define HBN_RTC_TIME_LATCH_H_POS  (0U)
#define HBN_RTC_TIME_LATCH_H_LEN  (8U)
#define HBN_RTC_TIME_LATCH_H_MSK  (((1U << HBN_RTC_TIME_LATCH_H_LEN) - 1) << HBN_RTC_TIME_LATCH_H_POS)
#define HBN_RTC_TIME_LATCH_H_UMSK (~(((1U << HBN_RTC_TIME_LATCH_H_LEN) - 1) << HBN_RTC_TIME_LATCH_H_POS))
#define HBN_RTC_TIME_LATCH        HBN_RTC_TIME_LATCH
#define HBN_RTC_TIME_LATCH_POS    (31U)
#define HBN_RTC_TIME_LATCH_LEN    (1U)
#define HBN_RTC_TIME_LATCH_MSK    (((1U << HBN_RTC_TIME_LATCH_LEN) - 1) << HBN_RTC_TIME_LATCH_POS)
#define HBN_RTC_TIME_LATCH_UMSK   (~(((1U << HBN_RTC_TIME_LATCH_LEN) - 1) << HBN_RTC_TIME_LATCH_POS))

/* 0x14 : HBN_IRQ_MODE */
#define HBN_IRQ_MODE_OFFSET      (0x14)
#define HBN_PIN_WAKEUP_MODE      HBN_PIN_WAKEUP_MODE
#define HBN_PIN_WAKEUP_MODE_POS  (0U)
#define HBN_PIN_WAKEUP_MODE_LEN  (4U)
#define HBN_PIN_WAKEUP_MODE_MSK  (((1U << HBN_PIN_WAKEUP_MODE_LEN) - 1) << HBN_PIN_WAKEUP_MODE_POS)
#define HBN_PIN_WAKEUP_MODE_UMSK (~(((1U << HBN_PIN_WAKEUP_MODE_LEN) - 1) << HBN_PIN_WAKEUP_MODE_POS))
#define HBN_PIN_WAKEUP_MASK      HBN_PIN_WAKEUP_MASK
#define HBN_PIN_WAKEUP_MASK_POS  (4U)
#define HBN_PIN_WAKEUP_MASK_LEN  (9U)
#define HBN_PIN_WAKEUP_MASK_MSK  (((1U << HBN_PIN_WAKEUP_MASK_LEN) - 1) << HBN_PIN_WAKEUP_MASK_POS)
#define HBN_PIN_WAKEUP_MASK_UMSK (~(((1U << HBN_PIN_WAKEUP_MASK_LEN) - 1) << HBN_PIN_WAKEUP_MASK_POS))
#define HBN_REG_EN_HW_PU_PD      HBN_REG_EN_HW_PU_PD
#define HBN_REG_EN_HW_PU_PD_POS  (16U)
#define HBN_REG_EN_HW_PU_PD_LEN  (1U)
#define HBN_REG_EN_HW_PU_PD_MSK  (((1U << HBN_REG_EN_HW_PU_PD_LEN) - 1) << HBN_REG_EN_HW_PU_PD_POS)
#define HBN_REG_EN_HW_PU_PD_UMSK (~(((1U << HBN_REG_EN_HW_PU_PD_LEN) - 1) << HBN_REG_EN_HW_PU_PD_POS))
#define HBN_IRQ_BOR_EN           HBN_IRQ_BOR_EN
#define HBN_IRQ_BOR_EN_POS       (18U)
#define HBN_IRQ_BOR_EN_LEN       (1U)
#define HBN_IRQ_BOR_EN_MSK       (((1U << HBN_IRQ_BOR_EN_LEN) - 1) << HBN_IRQ_BOR_EN_POS)
#define HBN_IRQ_BOR_EN_UMSK      (~(((1U << HBN_IRQ_BOR_EN_LEN) - 1) << HBN_IRQ_BOR_EN_POS))
#define HBN_IRQ_ACOMP0_EN        HBN_IRQ_ACOMP0_EN
#define HBN_IRQ_ACOMP0_EN_POS    (20U)
#define HBN_IRQ_ACOMP0_EN_LEN    (2U)
#define HBN_IRQ_ACOMP0_EN_MSK    (((1U << HBN_IRQ_ACOMP0_EN_LEN) - 1) << HBN_IRQ_ACOMP0_EN_POS)
#define HBN_IRQ_ACOMP0_EN_UMSK   (~(((1U << HBN_IRQ_ACOMP0_EN_LEN) - 1) << HBN_IRQ_ACOMP0_EN_POS))
#define HBN_IRQ_ACOMP1_EN        HBN_IRQ_ACOMP1_EN
#define HBN_IRQ_ACOMP1_EN_POS    (22U)
#define HBN_IRQ_ACOMP1_EN_LEN    (2U)
#define HBN_IRQ_ACOMP1_EN_MSK    (((1U << HBN_IRQ_ACOMP1_EN_LEN) - 1) << HBN_IRQ_ACOMP1_EN_POS)
#define HBN_IRQ_ACOMP1_EN_UMSK   (~(((1U << HBN_IRQ_ACOMP1_EN_LEN) - 1) << HBN_IRQ_ACOMP1_EN_POS))
#define HBN_PIN_WAKEUP_SEL       HBN_PIN_WAKEUP_SEL
#define HBN_PIN_WAKEUP_SEL_POS   (24U)
#define HBN_PIN_WAKEUP_SEL_LEN   (3U)
#define HBN_PIN_WAKEUP_SEL_MSK   (((1U << HBN_PIN_WAKEUP_SEL_LEN) - 1) << HBN_PIN_WAKEUP_SEL_POS)
#define HBN_PIN_WAKEUP_SEL_UMSK  (~(((1U << HBN_PIN_WAKEUP_SEL_LEN) - 1) << HBN_PIN_WAKEUP_SEL_POS))
#define HBN_PIN_WAKEUP_EN        HBN_PIN_WAKEUP_EN
#define HBN_PIN_WAKEUP_EN_POS    (27U)
#define HBN_PIN_WAKEUP_EN_LEN    (1U)
#define HBN_PIN_WAKEUP_EN_MSK    (((1U << HBN_PIN_WAKEUP_EN_LEN) - 1) << HBN_PIN_WAKEUP_EN_POS)
#define HBN_PIN_WAKEUP_EN_UMSK   (~(((1U << HBN_PIN_WAKEUP_EN_LEN) - 1) << HBN_PIN_WAKEUP_EN_POS))

/* 0x18 : HBN_IRQ_STAT */
#define HBN_IRQ_STAT_OFFSET (0x18)
#define HBN_IRQ_STAT        HBN_IRQ_STAT
#define HBN_IRQ_STAT_POS    (0U)
#define HBN_IRQ_STAT_LEN    (32U)
#define HBN_IRQ_STAT_MSK    (((1U << HBN_IRQ_STAT_LEN) - 1) << HBN_IRQ_STAT_POS)
#define HBN_IRQ_STAT_UMSK   (~(((1U << HBN_IRQ_STAT_LEN) - 1) << HBN_IRQ_STAT_POS))

/* 0x1C : HBN_IRQ_CLR */
#define HBN_IRQ_CLR_OFFSET (0x1C)
#define HBN_IRQ_CLR        HBN_IRQ_CLR
#define HBN_IRQ_CLR_POS    (0U)
#define HBN_IRQ_CLR_LEN    (32U)
#define HBN_IRQ_CLR_MSK    (((1U << HBN_IRQ_CLR_LEN) - 1) << HBN_IRQ_CLR_POS)
#define HBN_IRQ_CLR_UMSK   (~(((1U << HBN_IRQ_CLR_LEN) - 1) << HBN_IRQ_CLR_POS))

/* 0x20 : HBN_PIR_CFG */
#define HBN_PIR_CFG_OFFSET   (0x20)
#define HBN_PIR_HPF_SEL      HBN_PIR_HPF_SEL
#define HBN_PIR_HPF_SEL_POS  (0U)
#define HBN_PIR_HPF_SEL_LEN  (2U)
#define HBN_PIR_HPF_SEL_MSK  (((1U << HBN_PIR_HPF_SEL_LEN) - 1) << HBN_PIR_HPF_SEL_POS)
#define HBN_PIR_HPF_SEL_UMSK (~(((1U << HBN_PIR_HPF_SEL_LEN) - 1) << HBN_PIR_HPF_SEL_POS))
#define HBN_PIR_LPF_SEL      HBN_PIR_LPF_SEL
#define HBN_PIR_LPF_SEL_POS  (2U)
#define HBN_PIR_LPF_SEL_LEN  (1U)
#define HBN_PIR_LPF_SEL_MSK  (((1U << HBN_PIR_LPF_SEL_LEN) - 1) << HBN_PIR_LPF_SEL_POS)
#define HBN_PIR_LPF_SEL_UMSK (~(((1U << HBN_PIR_LPF_SEL_LEN) - 1) << HBN_PIR_LPF_SEL_POS))
#define HBN_PIR_DIS          HBN_PIR_DIS
#define HBN_PIR_DIS_POS      (4U)
#define HBN_PIR_DIS_LEN      (2U)
#define HBN_PIR_DIS_MSK      (((1U << HBN_PIR_DIS_LEN) - 1) << HBN_PIR_DIS_POS)
#define HBN_PIR_DIS_UMSK     (~(((1U << HBN_PIR_DIS_LEN) - 1) << HBN_PIR_DIS_POS))
#define HBN_PIR_EN           HBN_PIR_EN
#define HBN_PIR_EN_POS       (7U)
#define HBN_PIR_EN_LEN       (1U)
#define HBN_PIR_EN_MSK       (((1U << HBN_PIR_EN_LEN) - 1) << HBN_PIR_EN_POS)
#define HBN_PIR_EN_UMSK      (~(((1U << HBN_PIR_EN_LEN) - 1) << HBN_PIR_EN_POS))
#define HBN_GPADC_CS         HBN_GPADC_CS
#define HBN_GPADC_CS_POS     (8U)
#define HBN_GPADC_CS_LEN     (1U)
#define HBN_GPADC_CS_MSK     (((1U << HBN_GPADC_CS_LEN) - 1) << HBN_GPADC_CS_POS)
#define HBN_GPADC_CS_UMSK    (~(((1U << HBN_GPADC_CS_LEN) - 1) << HBN_GPADC_CS_POS))

/* 0x24 : HBN_PIR_VTH */
#define HBN_PIR_VTH_OFFSET (0x24)
#define HBN_PIR_VTH        HBN_PIR_VTH
#define HBN_PIR_VTH_POS    (0U)
#define HBN_PIR_VTH_LEN    (14U)
#define HBN_PIR_VTH_MSK    (((1U << HBN_PIR_VTH_LEN) - 1) << HBN_PIR_VTH_POS)
#define HBN_PIR_VTH_UMSK   (~(((1U << HBN_PIR_VTH_LEN) - 1) << HBN_PIR_VTH_POS))

/* 0x28 : HBN_PIR_INTERVAL */
#define HBN_PIR_INTERVAL_OFFSET (0x28)
#define HBN_PIR_INTERVAL        HBN_PIR_INTERVAL
#define HBN_PIR_INTERVAL_POS    (0U)
#define HBN_PIR_INTERVAL_LEN    (12U)
#define HBN_PIR_INTERVAL_MSK    (((1U << HBN_PIR_INTERVAL_LEN) - 1) << HBN_PIR_INTERVAL_POS)
#define HBN_PIR_INTERVAL_UMSK   (~(((1U << HBN_PIR_INTERVAL_LEN) - 1) << HBN_PIR_INTERVAL_POS))

/* 0x2C : HBN_BOR_CFG */
#define HBN_BOR_CFG_OFFSET (0x2C)
#define HBN_BOD_SEL        HBN_BOD_SEL
#define HBN_BOD_SEL_POS    (0U)
#define HBN_BOD_SEL_LEN    (1U)
#define HBN_BOD_SEL_MSK    (((1U << HBN_BOD_SEL_LEN) - 1) << HBN_BOD_SEL_POS)
#define HBN_BOD_SEL_UMSK   (~(((1U << HBN_BOD_SEL_LEN) - 1) << HBN_BOD_SEL_POS))
#define HBN_BOD_VTH        HBN_BOD_VTH
#define HBN_BOD_VTH_POS    (1U)
#define HBN_BOD_VTH_LEN    (3U)
#define HBN_BOD_VTH_MSK    (((1U << HBN_BOD_VTH_LEN) - 1) << HBN_BOD_VTH_POS)
#define HBN_BOD_VTH_UMSK   (~(((1U << HBN_BOD_VTH_LEN) - 1) << HBN_BOD_VTH_POS))
#define HBN_PU_BOD         HBN_PU_BOD
#define HBN_PU_BOD_POS     (4U)
#define HBN_PU_BOD_LEN     (1U)
#define HBN_PU_BOD_MSK     (((1U << HBN_PU_BOD_LEN) - 1) << HBN_PU_BOD_POS)
#define HBN_PU_BOD_UMSK    (~(((1U << HBN_PU_BOD_LEN) - 1) << HBN_PU_BOD_POS))
#define HBN_R_BOD_OUT      HBN_R_BOD_OUT
#define HBN_R_BOD_OUT_POS  (5U)
#define HBN_R_BOD_OUT_LEN  (1U)
#define HBN_R_BOD_OUT_MSK  (((1U << HBN_R_BOD_OUT_LEN) - 1) << HBN_R_BOD_OUT_POS)
#define HBN_R_BOD_OUT_UMSK (~(((1U << HBN_R_BOD_OUT_LEN) - 1) << HBN_R_BOD_OUT_POS))

/* 0x30 : HBN_GLB */
#define HBN_GLB_OFFSET                    (0x30)
#define HBN_ROOT_CLK_SEL                  HBN_ROOT_CLK_SEL
#define HBN_ROOT_CLK_SEL_POS              (0U)
#define HBN_ROOT_CLK_SEL_LEN              (2U)
#define HBN_ROOT_CLK_SEL_MSK              (((1U << HBN_ROOT_CLK_SEL_LEN) - 1) << HBN_ROOT_CLK_SEL_POS)
#define HBN_ROOT_CLK_SEL_UMSK             (~(((1U << HBN_ROOT_CLK_SEL_LEN) - 1) << HBN_ROOT_CLK_SEL_POS))
#define HBN_UART_CLK_SEL                  HBN_UART_CLK_SEL
#define HBN_UART_CLK_SEL_POS              (2U)
#define HBN_UART_CLK_SEL_LEN              (1U)
#define HBN_UART_CLK_SEL_MSK              (((1U << HBN_UART_CLK_SEL_LEN) - 1) << HBN_UART_CLK_SEL_POS)
#define HBN_UART_CLK_SEL_UMSK             (~(((1U << HBN_UART_CLK_SEL_LEN) - 1) << HBN_UART_CLK_SEL_POS))
#define HBN_F32K_SEL                      HBN_F32K_SEL
#define HBN_F32K_SEL_POS                  (3U)
#define HBN_F32K_SEL_LEN                  (2U)
#define HBN_F32K_SEL_MSK                  (((1U << HBN_F32K_SEL_LEN) - 1) << HBN_F32K_SEL_POS)
#define HBN_F32K_SEL_UMSK                 (~(((1U << HBN_F32K_SEL_LEN) - 1) << HBN_F32K_SEL_POS))
#define HBN_RESET_EVENT                   HBN_RESET_EVENT
#define HBN_RESET_EVENT_POS               (7U)
#define HBN_RESET_EVENT_LEN               (6U)
#define HBN_RESET_EVENT_MSK               (((1U << HBN_RESET_EVENT_LEN) - 1) << HBN_RESET_EVENT_POS)
#define HBN_RESET_EVENT_UMSK              (~(((1U << HBN_RESET_EVENT_LEN) - 1) << HBN_RESET_EVENT_POS))
#define HBN_CLR_RESET_EVENT               HBN_CLR_RESET_EVENT
#define HBN_CLR_RESET_EVENT_POS           (13U)
#define HBN_CLR_RESET_EVENT_LEN           (1U)
#define HBN_CLR_RESET_EVENT_MSK           (((1U << HBN_CLR_RESET_EVENT_LEN) - 1) << HBN_CLR_RESET_EVENT_POS)
#define HBN_CLR_RESET_EVENT_UMSK          (~(((1U << HBN_CLR_RESET_EVENT_LEN) - 1) << HBN_CLR_RESET_EVENT_POS))
#define HBN_UART_CLK_SEL2                 HBN_UART_CLK_SEL2
#define HBN_UART_CLK_SEL2_POS             (15U)
#define HBN_UART_CLK_SEL2_LEN             (1U)
#define HBN_UART_CLK_SEL2_MSK             (((1U << HBN_UART_CLK_SEL2_LEN) - 1) << HBN_UART_CLK_SEL2_POS)
#define HBN_UART_CLK_SEL2_UMSK            (~(((1U << HBN_UART_CLK_SEL2_LEN) - 1) << HBN_UART_CLK_SEL2_POS))
#define HBN_SW_LDO11SOC_VOUT_SEL_AON      HBN_SW_LDO11SOC_VOUT_SEL_AON
#define HBN_SW_LDO11SOC_VOUT_SEL_AON_POS  (16U)
#define HBN_SW_LDO11SOC_VOUT_SEL_AON_LEN  (4U)
#define HBN_SW_LDO11SOC_VOUT_SEL_AON_MSK  (((1U << HBN_SW_LDO11SOC_VOUT_SEL_AON_LEN) - 1) << HBN_SW_LDO11SOC_VOUT_SEL_AON_POS)
#define HBN_SW_LDO11SOC_VOUT_SEL_AON_UMSK (~(((1U << HBN_SW_LDO11SOC_VOUT_SEL_AON_LEN) - 1) << HBN_SW_LDO11SOC_VOUT_SEL_AON_POS))
#define HBN_SW_LDO11_RT_VOUT_SEL          HBN_SW_LDO11_RT_VOUT_SEL
#define HBN_SW_LDO11_RT_VOUT_SEL_POS      (24U)
#define HBN_SW_LDO11_RT_VOUT_SEL_LEN      (4U)
#define HBN_SW_LDO11_RT_VOUT_SEL_MSK      (((1U << HBN_SW_LDO11_RT_VOUT_SEL_LEN) - 1) << HBN_SW_LDO11_RT_VOUT_SEL_POS)
#define HBN_SW_LDO11_RT_VOUT_SEL_UMSK     (~(((1U << HBN_SW_LDO11_RT_VOUT_SEL_LEN) - 1) << HBN_SW_LDO11_RT_VOUT_SEL_POS))
#define HBN_SW_LDO11_AON_VOUT_SEL         HBN_SW_LDO11_AON_VOUT_SEL
#define HBN_SW_LDO11_AON_VOUT_SEL_POS     (28U)
#define HBN_SW_LDO11_AON_VOUT_SEL_LEN     (4U)
#define HBN_SW_LDO11_AON_VOUT_SEL_MSK     (((1U << HBN_SW_LDO11_AON_VOUT_SEL_LEN) - 1) << HBN_SW_LDO11_AON_VOUT_SEL_POS)
#define HBN_SW_LDO11_AON_VOUT_SEL_UMSK    (~(((1U << HBN_SW_LDO11_AON_VOUT_SEL_LEN) - 1) << HBN_SW_LDO11_AON_VOUT_SEL_POS))

/* 0x34 : HBN_SRAM */
#define HBN_SRAM_OFFSET     (0x34)
#define HBN_RETRAM_RET      HBN_RETRAM_RET
#define HBN_RETRAM_RET_POS  (6U)
#define HBN_RETRAM_RET_LEN  (1U)
#define HBN_RETRAM_RET_MSK  (((1U << HBN_RETRAM_RET_LEN) - 1) << HBN_RETRAM_RET_POS)
#define HBN_RETRAM_RET_UMSK (~(((1U << HBN_RETRAM_RET_LEN) - 1) << HBN_RETRAM_RET_POS))
#define HBN_RETRAM_SLP      HBN_RETRAM_SLP
#define HBN_RETRAM_SLP_POS  (7U)
#define HBN_RETRAM_SLP_LEN  (1U)
#define HBN_RETRAM_SLP_MSK  (((1U << HBN_RETRAM_SLP_LEN) - 1) << HBN_RETRAM_SLP_POS)
#define HBN_RETRAM_SLP_UMSK (~(((1U << HBN_RETRAM_SLP_LEN) - 1) << HBN_RETRAM_SLP_POS))

/* 0x38 : HBN_PAD_CTRL_0 */
#define HBN_PAD_CTRL_0_OFFSET          (0x38)
#define HBN_REG_AON_PAD_IE_SMT         HBN_REG_AON_PAD_IE_SMT
#define HBN_REG_AON_PAD_IE_SMT_POS     (0U)
#define HBN_REG_AON_PAD_IE_SMT_LEN     (9U)
#define HBN_REG_AON_PAD_IE_SMT_MSK     (((1U << HBN_REG_AON_PAD_IE_SMT_LEN) - 1) << HBN_REG_AON_PAD_IE_SMT_POS)
#define HBN_REG_AON_PAD_IE_SMT_UMSK    (~(((1U << HBN_REG_AON_PAD_IE_SMT_LEN) - 1) << HBN_REG_AON_PAD_IE_SMT_POS))
#define HBN_REG_AON_LED_SEL            HBN_REG_AON_LED_SEL
#define HBN_REG_AON_LED_SEL_POS        (10U)
#define HBN_REG_AON_LED_SEL_LEN        (9U)
#define HBN_REG_AON_LED_SEL_MSK        (((1U << HBN_REG_AON_LED_SEL_LEN) - 1) << HBN_REG_AON_LED_SEL_POS)
#define HBN_REG_AON_LED_SEL_UMSK       (~(((1U << HBN_REG_AON_LED_SEL_LEN) - 1) << HBN_REG_AON_LED_SEL_POS))
#define HBN_REG_EN_AON_CTRL_GPIO       HBN_REG_EN_AON_CTRL_GPIO
#define HBN_REG_EN_AON_CTRL_GPIO_POS   (20U)
#define HBN_REG_EN_AON_CTRL_GPIO_LEN   (9U)
#define HBN_REG_EN_AON_CTRL_GPIO_MSK   (((1U << HBN_REG_EN_AON_CTRL_GPIO_LEN) - 1) << HBN_REG_EN_AON_CTRL_GPIO_POS)
#define HBN_REG_EN_AON_CTRL_GPIO_UMSK  (~(((1U << HBN_REG_EN_AON_CTRL_GPIO_LEN) - 1) << HBN_REG_EN_AON_CTRL_GPIO_POS))
#define HBN_REG_AON_GPIO_ISO_MODE      HBN_REG_AON_GPIO_ISO_MODE
#define HBN_REG_AON_GPIO_ISO_MODE_POS  (31U)
#define HBN_REG_AON_GPIO_ISO_MODE_LEN  (1U)
#define HBN_REG_AON_GPIO_ISO_MODE_MSK  (((1U << HBN_REG_AON_GPIO_ISO_MODE_LEN) - 1) << HBN_REG_AON_GPIO_ISO_MODE_POS)
#define HBN_REG_AON_GPIO_ISO_MODE_UMSK (~(((1U << HBN_REG_AON_GPIO_ISO_MODE_LEN) - 1) << HBN_REG_AON_GPIO_ISO_MODE_POS))

/* 0x3C : HBN_PAD_CTRL_1 */
#define HBN_PAD_CTRL_1_OFFSET   (0x3C)
#define HBN_REG_AON_PAD_OE      HBN_REG_AON_PAD_OE
#define HBN_REG_AON_PAD_OE_POS  (0U)
#define HBN_REG_AON_PAD_OE_LEN  (9U)
#define HBN_REG_AON_PAD_OE_MSK  (((1U << HBN_REG_AON_PAD_OE_LEN) - 1) << HBN_REG_AON_PAD_OE_POS)
#define HBN_REG_AON_PAD_OE_UMSK (~(((1U << HBN_REG_AON_PAD_OE_LEN) - 1) << HBN_REG_AON_PAD_OE_POS))
#define HBN_REG_AON_PAD_PD      HBN_REG_AON_PAD_PD
#define HBN_REG_AON_PAD_PD_POS  (10U)
#define HBN_REG_AON_PAD_PD_LEN  (9U)
#define HBN_REG_AON_PAD_PD_MSK  (((1U << HBN_REG_AON_PAD_PD_LEN) - 1) << HBN_REG_AON_PAD_PD_POS)
#define HBN_REG_AON_PAD_PD_UMSK (~(((1U << HBN_REG_AON_PAD_PD_LEN) - 1) << HBN_REG_AON_PAD_PD_POS))
#define HBN_REG_AON_PAD_PU      HBN_REG_AON_PAD_PU
#define HBN_REG_AON_PAD_PU_POS  (20U)
#define HBN_REG_AON_PAD_PU_LEN  (9U)
#define HBN_REG_AON_PAD_PU_MSK  (((1U << HBN_REG_AON_PAD_PU_LEN) - 1) << HBN_REG_AON_PAD_PU_POS)
#define HBN_REG_AON_PAD_PU_UMSK (~(((1U << HBN_REG_AON_PAD_PU_LEN) - 1) << HBN_REG_AON_PAD_PU_POS))

/* 0x100 : HBN_RSV0 */
#define HBN_RSV0_OFFSET (0x100)
#define HBN_RSV0        HBN_RSV0
#define HBN_RSV0_POS    (0U)
#define HBN_RSV0_LEN    (32U)
#define HBN_RSV0_MSK    (((1U << HBN_RSV0_LEN) - 1) << HBN_RSV0_POS)
#define HBN_RSV0_UMSK   (~(((1U << HBN_RSV0_LEN) - 1) << HBN_RSV0_POS))

/* 0x104 : HBN_RSV1 */
#define HBN_RSV1_OFFSET (0x104)
#define HBN_RSV1        HBN_RSV1
#define HBN_RSV1_POS    (0U)
#define HBN_RSV1_LEN    (32U)
#define HBN_RSV1_MSK    (((1U << HBN_RSV1_LEN) - 1) << HBN_RSV1_POS)
#define HBN_RSV1_UMSK   (~(((1U << HBN_RSV1_LEN) - 1) << HBN_RSV1_POS))

/* 0x108 : HBN_RSV2 */
#define HBN_RSV2_OFFSET (0x108)
#define HBN_RSV2        HBN_RSV2
#define HBN_RSV2_POS    (0U)
#define HBN_RSV2_LEN    (32U)
#define HBN_RSV2_MSK    (((1U << HBN_RSV2_LEN) - 1) << HBN_RSV2_POS)
#define HBN_RSV2_UMSK   (~(((1U << HBN_RSV2_LEN) - 1) << HBN_RSV2_POS))

/* 0x10C : HBN_RSV3 */
#define HBN_RSV3_OFFSET (0x10C)
#define HBN_RSV3        HBN_RSV3
#define HBN_RSV3_POS    (0U)
#define HBN_RSV3_LEN    (32U)
#define HBN_RSV3_MSK    (((1U << HBN_RSV3_LEN) - 1) << HBN_RSV3_POS)
#define HBN_RSV3_UMSK   (~(((1U << HBN_RSV3_LEN) - 1) << HBN_RSV3_POS))

/* 0x200 : rc32k_ctrl0 */
#define HBN_RC32K_CTRL0_OFFSET         (0x200)
#define HBN_RC32K_CAL_DONE             HBN_RC32K_CAL_DONE
#define HBN_RC32K_CAL_DONE_POS         (0U)
#define HBN_RC32K_CAL_DONE_LEN         (1U)
#define HBN_RC32K_CAL_DONE_MSK         (((1U << HBN_RC32K_CAL_DONE_LEN) - 1) << HBN_RC32K_CAL_DONE_POS)
#define HBN_RC32K_CAL_DONE_UMSK        (~(((1U << HBN_RC32K_CAL_DONE_LEN) - 1) << HBN_RC32K_CAL_DONE_POS))
#define HBN_RC32K_RDY                  HBN_RC32K_RDY
#define HBN_RC32K_RDY_POS              (1U)
#define HBN_RC32K_RDY_LEN              (1U)
#define HBN_RC32K_RDY_MSK              (((1U << HBN_RC32K_RDY_LEN) - 1) << HBN_RC32K_RDY_POS)
#define HBN_RC32K_RDY_UMSK             (~(((1U << HBN_RC32K_RDY_LEN) - 1) << HBN_RC32K_RDY_POS))
#define HBN_RC32K_CAL_INPROGRESS       HBN_RC32K_CAL_INPROGRESS
#define HBN_RC32K_CAL_INPROGRESS_POS   (2U)
#define HBN_RC32K_CAL_INPROGRESS_LEN   (1U)
#define HBN_RC32K_CAL_INPROGRESS_MSK   (((1U << HBN_RC32K_CAL_INPROGRESS_LEN) - 1) << HBN_RC32K_CAL_INPROGRESS_POS)
#define HBN_RC32K_CAL_INPROGRESS_UMSK  (~(((1U << HBN_RC32K_CAL_INPROGRESS_LEN) - 1) << HBN_RC32K_CAL_INPROGRESS_POS))
#define HBN_RC32K_CAL_DIV              HBN_RC32K_CAL_DIV
#define HBN_RC32K_CAL_DIV_POS          (3U)
#define HBN_RC32K_CAL_DIV_LEN          (2U)
#define HBN_RC32K_CAL_DIV_MSK          (((1U << HBN_RC32K_CAL_DIV_LEN) - 1) << HBN_RC32K_CAL_DIV_POS)
#define HBN_RC32K_CAL_DIV_UMSK         (~(((1U << HBN_RC32K_CAL_DIV_LEN) - 1) << HBN_RC32K_CAL_DIV_POS))
#define HBN_RC32K_CAL_PRECHARGE        HBN_RC32K_CAL_PRECHARGE
#define HBN_RC32K_CAL_PRECHARGE_POS    (5U)
#define HBN_RC32K_CAL_PRECHARGE_LEN    (1U)
#define HBN_RC32K_CAL_PRECHARGE_MSK    (((1U << HBN_RC32K_CAL_PRECHARGE_LEN) - 1) << HBN_RC32K_CAL_PRECHARGE_POS)
#define HBN_RC32K_CAL_PRECHARGE_UMSK   (~(((1U << HBN_RC32K_CAL_PRECHARGE_LEN) - 1) << HBN_RC32K_CAL_PRECHARGE_POS))
#define HBN_RC32K_DIG_CODE_FR_CAL      HBN_RC32K_DIG_CODE_FR_CAL
#define HBN_RC32K_DIG_CODE_FR_CAL_POS  (6U)
#define HBN_RC32K_DIG_CODE_FR_CAL_LEN  (10U)
#define HBN_RC32K_DIG_CODE_FR_CAL_MSK  (((1U << HBN_RC32K_DIG_CODE_FR_CAL_LEN) - 1) << HBN_RC32K_DIG_CODE_FR_CAL_POS)
#define HBN_RC32K_DIG_CODE_FR_CAL_UMSK (~(((1U << HBN_RC32K_DIG_CODE_FR_CAL_LEN) - 1) << HBN_RC32K_DIG_CODE_FR_CAL_POS))
#define HBN_RC32K_VREF_DLY             HBN_RC32K_VREF_DLY
#define HBN_RC32K_VREF_DLY_POS         (16U)
#define HBN_RC32K_VREF_DLY_LEN         (2U)
#define HBN_RC32K_VREF_DLY_MSK         (((1U << HBN_RC32K_VREF_DLY_LEN) - 1) << HBN_RC32K_VREF_DLY_POS)
#define HBN_RC32K_VREF_DLY_UMSK        (~(((1U << HBN_RC32K_VREF_DLY_LEN) - 1) << HBN_RC32K_VREF_DLY_POS))
#define HBN_RC32K_ALLOW_CAL            HBN_RC32K_ALLOW_CAL
#define HBN_RC32K_ALLOW_CAL_POS        (18U)
#define HBN_RC32K_ALLOW_CAL_LEN        (1U)
#define HBN_RC32K_ALLOW_CAL_MSK        (((1U << HBN_RC32K_ALLOW_CAL_LEN) - 1) << HBN_RC32K_ALLOW_CAL_POS)
#define HBN_RC32K_ALLOW_CAL_UMSK       (~(((1U << HBN_RC32K_ALLOW_CAL_LEN) - 1) << HBN_RC32K_ALLOW_CAL_POS))
#define HBN_RC32K_EXT_CODE_EN          HBN_RC32K_EXT_CODE_EN
#define HBN_RC32K_EXT_CODE_EN_POS      (19U)
#define HBN_RC32K_EXT_CODE_EN_LEN      (1U)
#define HBN_RC32K_EXT_CODE_EN_MSK      (((1U << HBN_RC32K_EXT_CODE_EN_LEN) - 1) << HBN_RC32K_EXT_CODE_EN_POS)
#define HBN_RC32K_EXT_CODE_EN_UMSK     (~(((1U << HBN_RC32K_EXT_CODE_EN_LEN) - 1) << HBN_RC32K_EXT_CODE_EN_POS))
#define HBN_RC32K_CAL_EN               HBN_RC32K_CAL_EN
#define HBN_RC32K_CAL_EN_POS           (20U)
#define HBN_RC32K_CAL_EN_LEN           (1U)
#define HBN_RC32K_CAL_EN_MSK           (((1U << HBN_RC32K_CAL_EN_LEN) - 1) << HBN_RC32K_CAL_EN_POS)
#define HBN_RC32K_CAL_EN_UMSK          (~(((1U << HBN_RC32K_CAL_EN_LEN) - 1) << HBN_RC32K_CAL_EN_POS))
#define HBN_PU_RC32K                   HBN_PU_RC32K
#define HBN_PU_RC32K_POS               (21U)
#define HBN_PU_RC32K_LEN               (1U)
#define HBN_PU_RC32K_MSK               (((1U << HBN_PU_RC32K_LEN) - 1) << HBN_PU_RC32K_POS)
#define HBN_PU_RC32K_UMSK              (~(((1U << HBN_PU_RC32K_LEN) - 1) << HBN_PU_RC32K_POS))
#define HBN_RC32K_CODE_FR_EXT          HBN_RC32K_CODE_FR_EXT
#define HBN_RC32K_CODE_FR_EXT_POS      (22U)
#define HBN_RC32K_CODE_FR_EXT_LEN      (10U)
#define HBN_RC32K_CODE_FR_EXT_MSK      (((1U << HBN_RC32K_CODE_FR_EXT_LEN) - 1) << HBN_RC32K_CODE_FR_EXT_POS)
#define HBN_RC32K_CODE_FR_EXT_UMSK     (~(((1U << HBN_RC32K_CODE_FR_EXT_LEN) - 1) << HBN_RC32K_CODE_FR_EXT_POS))

/* 0x204 : xtal32k */
#define HBN_XTAL32K_OFFSET            (0x204)
#define HBN_XTAL32K_EXT_SEL           HBN_XTAL32K_EXT_SEL
#define HBN_XTAL32K_EXT_SEL_POS       (2U)
#define HBN_XTAL32K_EXT_SEL_LEN       (1U)
#define HBN_XTAL32K_EXT_SEL_MSK       (((1U << HBN_XTAL32K_EXT_SEL_LEN) - 1) << HBN_XTAL32K_EXT_SEL_POS)
#define HBN_XTAL32K_EXT_SEL_UMSK      (~(((1U << HBN_XTAL32K_EXT_SEL_LEN) - 1) << HBN_XTAL32K_EXT_SEL_POS))
#define HBN_XTAL32K_AMP_CTRL          HBN_XTAL32K_AMP_CTRL
#define HBN_XTAL32K_AMP_CTRL_POS      (3U)
#define HBN_XTAL32K_AMP_CTRL_LEN      (2U)
#define HBN_XTAL32K_AMP_CTRL_MSK      (((1U << HBN_XTAL32K_AMP_CTRL_LEN) - 1) << HBN_XTAL32K_AMP_CTRL_POS)
#define HBN_XTAL32K_AMP_CTRL_UMSK     (~(((1U << HBN_XTAL32K_AMP_CTRL_LEN) - 1) << HBN_XTAL32K_AMP_CTRL_POS))
#define HBN_XTAL32K_REG               HBN_XTAL32K_REG
#define HBN_XTAL32K_REG_POS           (5U)
#define HBN_XTAL32K_REG_LEN           (2U)
#define HBN_XTAL32K_REG_MSK           (((1U << HBN_XTAL32K_REG_LEN) - 1) << HBN_XTAL32K_REG_POS)
#define HBN_XTAL32K_REG_UMSK          (~(((1U << HBN_XTAL32K_REG_LEN) - 1) << HBN_XTAL32K_REG_POS))
#define HBN_XTAL32K_OUTBUF_STRE       HBN_XTAL32K_OUTBUF_STRE
#define HBN_XTAL32K_OUTBUF_STRE_POS   (7U)
#define HBN_XTAL32K_OUTBUF_STRE_LEN   (1U)
#define HBN_XTAL32K_OUTBUF_STRE_MSK   (((1U << HBN_XTAL32K_OUTBUF_STRE_LEN) - 1) << HBN_XTAL32K_OUTBUF_STRE_POS)
#define HBN_XTAL32K_OUTBUF_STRE_UMSK  (~(((1U << HBN_XTAL32K_OUTBUF_STRE_LEN) - 1) << HBN_XTAL32K_OUTBUF_STRE_POS))
#define HBN_XTAL32K_OTF_SHORT         HBN_XTAL32K_OTF_SHORT
#define HBN_XTAL32K_OTF_SHORT_POS     (8U)
#define HBN_XTAL32K_OTF_SHORT_LEN     (1U)
#define HBN_XTAL32K_OTF_SHORT_MSK     (((1U << HBN_XTAL32K_OTF_SHORT_LEN) - 1) << HBN_XTAL32K_OTF_SHORT_POS)
#define HBN_XTAL32K_OTF_SHORT_UMSK    (~(((1U << HBN_XTAL32K_OTF_SHORT_LEN) - 1) << HBN_XTAL32K_OTF_SHORT_POS))
#define HBN_XTAL32K_INV_STRE          HBN_XTAL32K_INV_STRE
#define HBN_XTAL32K_INV_STRE_POS      (9U)
#define HBN_XTAL32K_INV_STRE_LEN      (2U)
#define HBN_XTAL32K_INV_STRE_MSK      (((1U << HBN_XTAL32K_INV_STRE_LEN) - 1) << HBN_XTAL32K_INV_STRE_POS)
#define HBN_XTAL32K_INV_STRE_UMSK     (~(((1U << HBN_XTAL32K_INV_STRE_LEN) - 1) << HBN_XTAL32K_INV_STRE_POS))
#define HBN_XTAL32K_CAPBANK           HBN_XTAL32K_CAPBANK
#define HBN_XTAL32K_CAPBANK_POS       (11U)
#define HBN_XTAL32K_CAPBANK_LEN       (6U)
#define HBN_XTAL32K_CAPBANK_MSK       (((1U << HBN_XTAL32K_CAPBANK_LEN) - 1) << HBN_XTAL32K_CAPBANK_POS)
#define HBN_XTAL32K_CAPBANK_UMSK      (~(((1U << HBN_XTAL32K_CAPBANK_LEN) - 1) << HBN_XTAL32K_CAPBANK_POS))
#define HBN_XTAL32K_AC_CAP_SHORT      HBN_XTAL32K_AC_CAP_SHORT
#define HBN_XTAL32K_AC_CAP_SHORT_POS  (17U)
#define HBN_XTAL32K_AC_CAP_SHORT_LEN  (1U)
#define HBN_XTAL32K_AC_CAP_SHORT_MSK  (((1U << HBN_XTAL32K_AC_CAP_SHORT_LEN) - 1) << HBN_XTAL32K_AC_CAP_SHORT_POS)
#define HBN_XTAL32K_AC_CAP_SHORT_UMSK (~(((1U << HBN_XTAL32K_AC_CAP_SHORT_LEN) - 1) << HBN_XTAL32K_AC_CAP_SHORT_POS))
#define HBN_PU_XTAL32K_BUF            HBN_PU_XTAL32K_BUF
#define HBN_PU_XTAL32K_BUF_POS        (18U)
#define HBN_PU_XTAL32K_BUF_LEN        (1U)
#define HBN_PU_XTAL32K_BUF_MSK        (((1U << HBN_PU_XTAL32K_BUF_LEN) - 1) << HBN_PU_XTAL32K_BUF_POS)
#define HBN_PU_XTAL32K_BUF_UMSK       (~(((1U << HBN_PU_XTAL32K_BUF_LEN) - 1) << HBN_PU_XTAL32K_BUF_POS))
#define HBN_PU_XTAL32K                HBN_PU_XTAL32K
#define HBN_PU_XTAL32K_POS            (19U)
#define HBN_PU_XTAL32K_LEN            (1U)
#define HBN_PU_XTAL32K_MSK            (((1U << HBN_PU_XTAL32K_LEN) - 1) << HBN_PU_XTAL32K_POS)
#define HBN_PU_XTAL32K_UMSK           (~(((1U << HBN_PU_XTAL32K_LEN) - 1) << HBN_PU_XTAL32K_POS))
#define HBN_XTAL32K_HIZ_EN            HBN_XTAL32K_HIZ_EN
#define HBN_XTAL32K_HIZ_EN_POS        (20U)
#define HBN_XTAL32K_HIZ_EN_LEN        (1U)
#define HBN_XTAL32K_HIZ_EN_MSK        (((1U << HBN_XTAL32K_HIZ_EN_LEN) - 1) << HBN_XTAL32K_HIZ_EN_POS)
#define HBN_XTAL32K_HIZ_EN_UMSK       (~(((1U << HBN_XTAL32K_HIZ_EN_LEN) - 1) << HBN_XTAL32K_HIZ_EN_POS))
#define HBN_DTEN_XTAL32K              HBN_DTEN_XTAL32K
#define HBN_DTEN_XTAL32K_POS          (22U)
#define HBN_DTEN_XTAL32K_LEN          (1U)
#define HBN_DTEN_XTAL32K_MSK          (((1U << HBN_DTEN_XTAL32K_LEN) - 1) << HBN_DTEN_XTAL32K_POS)
#define HBN_DTEN_XTAL32K_UMSK         (~(((1U << HBN_DTEN_XTAL32K_LEN) - 1) << HBN_DTEN_XTAL32K_POS))
#define HBN_TEN_XTAL32K               HBN_TEN_XTAL32K
#define HBN_TEN_XTAL32K_POS           (23U)
#define HBN_TEN_XTAL32K_LEN           (1U)
#define HBN_TEN_XTAL32K_MSK           (((1U << HBN_TEN_XTAL32K_LEN) - 1) << HBN_TEN_XTAL32K_POS)
#define HBN_TEN_XTAL32K_UMSK          (~(((1U << HBN_TEN_XTAL32K_LEN) - 1) << HBN_TEN_XTAL32K_POS))
#define HBN_F32K_SEL_RTC              HBN_F32K_SEL_RTC
#define HBN_F32K_SEL_RTC_POS          (24U)
#define HBN_F32K_SEL_RTC_LEN          (1U)
#define HBN_F32K_SEL_RTC_MSK          (((1U << HBN_F32K_SEL_RTC_LEN) - 1) << HBN_F32K_SEL_RTC_POS)
#define HBN_F32K_SEL_RTC_UMSK         (~(((1U << HBN_F32K_SEL_RTC_LEN) - 1) << HBN_F32K_SEL_RTC_POS))

/* 0x208 : rtc_rst_ctrl */
#define HBN_RTC_RST_CTRL_OFFSET       (0x208)
#define HBN_RTC_RST_WAIT_CNT_RTC      HBN_RTC_RST_WAIT_CNT_RTC
#define HBN_RTC_RST_WAIT_CNT_RTC_POS  (0U)
#define HBN_RTC_RST_WAIT_CNT_RTC_LEN  (16U)
#define HBN_RTC_RST_WAIT_CNT_RTC_MSK  (((1U << HBN_RTC_RST_WAIT_CNT_RTC_LEN) - 1) << HBN_RTC_RST_WAIT_CNT_RTC_POS)
#define HBN_RTC_RST_WAIT_CNT_RTC_UMSK (~(((1U << HBN_RTC_RST_WAIT_CNT_RTC_LEN) - 1) << HBN_RTC_RST_WAIT_CNT_RTC_POS))
#define HBN_RTC_RST_REFDIV_RTC        HBN_RTC_RST_REFDIV_RTC
#define HBN_RTC_RST_REFDIV_RTC_POS    (16U)
#define HBN_RTC_RST_REFDIV_RTC_LEN    (3U)
#define HBN_RTC_RST_REFDIV_RTC_MSK    (((1U << HBN_RTC_RST_REFDIV_RTC_LEN) - 1) << HBN_RTC_RST_REFDIV_RTC_POS)
#define HBN_RTC_RST_REFDIV_RTC_UMSK   (~(((1U << HBN_RTC_RST_REFDIV_RTC_LEN) - 1) << HBN_RTC_RST_REFDIV_RTC_POS))
#define HBN_RTC_RST_CTRL_MISC         HBN_RTC_RST_CTRL_MISC
#define HBN_RTC_RST_CTRL_MISC_POS     (19U)
#define HBN_RTC_RST_CTRL_MISC_LEN     (13U)
#define HBN_RTC_RST_CTRL_MISC_MSK     (((1U << HBN_RTC_RST_CTRL_MISC_LEN) - 1) << HBN_RTC_RST_CTRL_MISC_POS)
#define HBN_RTC_RST_CTRL_MISC_UMSK    (~(((1U << HBN_RTC_RST_CTRL_MISC_LEN) - 1) << HBN_RTC_RST_CTRL_MISC_POS))

/* 0x20C : rtc_rst_ctrl2 */
#define HBN_RTC_RST_CTRL2_OFFSET    (0x20C)
#define HBN_RTC_RESV                HBN_RTC_RESV
#define HBN_RTC_RESV_POS            (0U)
#define HBN_RTC_RESV_LEN            (8U)
#define HBN_RTC_RESV_MSK            (((1U << HBN_RTC_RESV_LEN) - 1) << HBN_RTC_RESV_POS)
#define HBN_RTC_RESV_UMSK           (~(((1U << HBN_RTC_RESV_LEN) - 1) << HBN_RTC_RESV_POS))
#define HBN_REG_EN_HW_PU_RC32K      HBN_REG_EN_HW_PU_RC32K
#define HBN_REG_EN_HW_PU_RC32K_POS  (8U)
#define HBN_REG_EN_HW_PU_RC32K_LEN  (1U)
#define HBN_REG_EN_HW_PU_RC32K_MSK  (((1U << HBN_REG_EN_HW_PU_RC32K_LEN) - 1) << HBN_REG_EN_HW_PU_RC32K_POS)
#define HBN_REG_EN_HW_PU_RC32K_UMSK (~(((1U << HBN_REG_EN_HW_PU_RC32K_LEN) - 1) << HBN_REG_EN_HW_PU_RC32K_POS))

struct hbn_reg {
    /* 0x0 : HBN_CTL */
    union {
        struct {
            uint32_t rtc_ctl                  : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t rtc_dly_option           : 1; /* [    4],        r/w,        0x0 */
            uint32_t pu_ldo18io_aon           : 1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6               : 1; /* [    6],       rsvd,        0x0 */
            uint32_t hbn_mode                 : 1; /* [    7],          w,        0x0 */
            uint32_t trap_mode                : 1; /* [    8],          r,        0x0 */
            uint32_t pwrdn_hbn_core           : 1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_11           : 2; /* [11:10],       rsvd,        0x0 */
            uint32_t sw_rst                   : 1; /* [   12],        r/w,        0x0 */
            uint32_t hbn_dis_pwr_off_ldo11    : 1; /* [   13],        r/w,        0x0 */
            uint32_t hbn_dis_pwr_off_ldo11_rt : 1; /* [   14],        r/w,        0x0 */
            uint32_t hbn_ldo11_rt_vout_sel    : 4; /* [18:15],        r/w,        0xa */
            uint32_t hbn_ldo11_aon_vout_sel   : 4; /* [22:19],        r/w,        0xa */
            uint32_t pu_dcdc_aon              : 1; /* [   23],        r/w,        0x1 */
            uint32_t pu_dcdc18_aon            : 1; /* [   24],        r/w,        0x1 */
            uint32_t pwr_on_option            : 1; /* [   25],        r/w,        0x0 */
            uint32_t sram_slp_option          : 1; /* [   26],        r/w,        0x0 */
            uint32_t sram_slp                 : 1; /* [   27],          r,        0x0 */
            uint32_t hbn_state                : 4; /* [31:28],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_CTL;

    /* 0x4 : HBN_TIME_L */
    union {
        struct {
            uint32_t hbn_time_l : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_TIME_L;

    /* 0x8 : HBN_TIME_H */
    union {
        struct {
            uint32_t hbn_time_h    : 8;  /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_31 : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_TIME_H;

    /* 0xC : RTC_TIME_L */
    union {
        struct {
            uint32_t rtc_time_latch_l : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } RTC_TIME_L;

    /* 0x10 : RTC_TIME_H */
    union {
        struct {
            uint32_t rtc_time_latch_h : 8;  /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_30    : 23; /* [30: 8],       rsvd,        0x0 */
            uint32_t rtc_time_latch   : 1;  /* [   31],          w,        0x0 */
        } BF;
        uint32_t WORD;
    } RTC_TIME_H;

    /* 0x14 : HBN_IRQ_MODE */
    union {
        struct {
            uint32_t hbn_pin_wakeup_mode : 4; /* [ 3: 0],        r/w,        0x5 */
            uint32_t hbn_pin_wakeup_mask : 9; /* [12: 4],        r/w,        0x0 */
            uint32_t reserved_13_15      : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t reg_en_hw_pu_pd     : 1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17         : 1; /* [   17],       rsvd,        0x0 */
            uint32_t irq_bor_en          : 1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19         : 1; /* [   19],       rsvd,        0x0 */
            uint32_t irq_acomp0_en       : 2; /* [21:20],        r/w,        0x0 */
            uint32_t irq_acomp1_en       : 2; /* [23:22],        r/w,        0x0 */
            uint32_t pin_wakeup_sel      : 3; /* [26:24],        r/w,        0x3 */
            uint32_t pin_wakeup_en       : 1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31      : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_IRQ_MODE;

    /* 0x18 : HBN_IRQ_STAT */
    union {
        struct {
            uint32_t irq_stat : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_IRQ_STAT;

    /* 0x1C : HBN_IRQ_CLR */
    union {
        struct {
            uint32_t irq_clr : 32; /* [31: 0],          w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_IRQ_CLR;

    /* 0x20 : HBN_PIR_CFG */
    union {
        struct {
            uint32_t pir_hpf_sel   : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pir_lpf_sel   : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3    : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t pir_dis       : 2;  /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6    : 1;  /* [    6],       rsvd,        0x0 */
            uint32_t pir_en        : 1;  /* [    7],        r/w,        0x0 */
            uint32_t gpadc_cs      : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31 : 23; /* [31: 9],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PIR_CFG;

    /* 0x24 : HBN_PIR_VTH */
    union {
        struct {
            uint32_t pir_vth        : 14; /* [13: 0],        r/w,      0x3ff */
            uint32_t reserved_14_31 : 18; /* [31:14],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PIR_VTH;

    /* 0x28 : HBN_PIR_INTERVAL */
    union {
        struct {
            uint32_t pir_interval   : 12; /* [11: 0],        r/w,      0xa3d */
            uint32_t reserved_12_31 : 20; /* [31:12],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PIR_INTERVAL;

    /* 0x2C : HBN_BOR_CFG */
    union {
        struct {
            uint32_t bod_sel       : 1;  /* [    0],        r/w,        0x0 */
            uint32_t bod_vth       : 3;  /* [ 3: 1],        r/w,        0x5 */
            uint32_t pu_bod        : 1;  /* [    4],        r/w,        0x0 */
            uint32_t r_bod_out     : 1;  /* [    5],          r,        0x0 */
            uint32_t reserved_6_31 : 26; /* [31: 6],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_BOR_CFG;

    /* 0x30 : HBN_GLB */
    union {
        struct {
            uint32_t hbn_root_clk_sel         : 2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t hbn_uart_clk_sel         : 1; /* [    2],        r/w,        0x0 */
            uint32_t hbn_f32k_sel             : 2; /* [ 4: 3],        r/w,        0x0 */
            uint32_t reserved_5_6             : 2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t hbn_reset_event          : 6; /* [12: 7],          r,        0x0 */
            uint32_t hbn_clr_reset_event      : 1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14              : 1; /* [   14],       rsvd,        0x0 */
            uint32_t hbn_uart_clk_sel2        : 1; /* [   15],        r/w,        0x0 */
            uint32_t sw_ldo11soc_vout_sel_aon : 4; /* [19:16],        r/w,        0xa */
            uint32_t reserved_20_23           : 4; /* [23:20],       rsvd,        0x0 */
            uint32_t sw_ldo11_rt_vout_sel     : 4; /* [27:24],        r/w,        0xa */
            uint32_t sw_ldo11_aon_vout_sel    : 4; /* [31:28],        r/w,        0xa */
        } BF;
        uint32_t WORD;
    } HBN_GLB;

    /* 0x34 : HBN_SRAM */
    union {
        struct {
            uint32_t reserved_0_5  : 6;  /* [ 5: 0],       rsvd,        0x0 */
            uint32_t retram_ret    : 1;  /* [    6],        r/w,        0x0 */
            uint32_t retram_slp    : 1;  /* [    7],        r/w,        0x0 */
            uint32_t reserved_8_31 : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_SRAM;

    /* 0x38 : HBN_PAD_CTRL_0 */
    union {
        struct {
            uint32_t reg_aon_pad_ie_smt    : 9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9            : 1; /* [    9],       rsvd,        0x0 */
            uint32_t reg_aon_led_sel       : 9; /* [18:10],        r/w,        0x0 */
            uint32_t reserved_19           : 1; /* [   19],       rsvd,        0x0 */
            uint32_t reg_en_aon_ctrl_gpio  : 9; /* [28:20],        r/w,      0x180 */
            uint32_t reserved_29_30        : 2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_aon_gpio_iso_mode : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PAD_CTRL_0;

    /* 0x3C : HBN_PAD_CTRL_1 */
    union {
        struct {
            uint32_t reg_aon_pad_oe : 9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9     : 1; /* [    9],       rsvd,        0x0 */
            uint32_t reg_aon_pad_pd : 9; /* [18:10],        r/w,        0x0 */
            uint32_t reserved_19    : 1; /* [   19],       rsvd,        0x0 */
            uint32_t reg_aon_pad_pu : 9; /* [28:20],        r/w,        0x0 */
            uint32_t reserved_29_31 : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PAD_CTRL_1;

    /* 0x40  reserved */
    uint8_t RESERVED0x40[192];

    /* 0x100 : HBN_RSV0 */
    union {
        struct {
            uint32_t HBN_RSV0 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_RSV0;

    /* 0x104 : HBN_RSV1 */
    union {
        struct {
            uint32_t HBN_RSV1 : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } HBN_RSV1;

    /* 0x108 : HBN_RSV2 */
    union {
        struct {
            uint32_t HBN_RSV2 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_RSV2;

    /* 0x10C : HBN_RSV3 */
    union {
        struct {
            uint32_t HBN_RSV3 : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } HBN_RSV3;

    /* 0x110  reserved */
    uint8_t RESERVED0x110[240];

    /* 0x200 : rc32k_ctrl0 */
    union {
        struct {
            uint32_t rc32k_cal_done        : 1;  /* [    0],          r,        0x1 */
            uint32_t rc32k_rdy             : 1;  /* [    1],          r,        0x1 */
            uint32_t rc32k_cal_inprogress  : 1;  /* [    2],          r,        0x0 */
            uint32_t rc32k_cal_div         : 2;  /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32k_cal_precharge   : 1;  /* [    5],          r,        0x0 */
            uint32_t rc32k_dig_code_fr_cal : 10; /* [15: 6],          r,      0x200 */
            uint32_t rc32k_vref_dly        : 2;  /* [17:16],        r/w,        0x0 */
            uint32_t rc32k_allow_cal       : 1;  /* [   18],        r/w,        0x0 */
            uint32_t rc32k_ext_code_en     : 1;  /* [   19],        r/w,        0x1 */
            uint32_t rc32k_cal_en          : 1;  /* [   20],        r/w,        0x0 */
            uint32_t pu_rc32k              : 1;  /* [   21],        r/w,        0x1 */
            uint32_t rc32k_code_fr_ext     : 10; /* [31:22],        r/w,      0x12c */
        } BF;
        uint32_t WORD;
    } rc32k_ctrl0;

    /* 0x204 : xtal32k */
    union {
        struct {
            uint32_t reserved_0_1         : 2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t xtal32k_ext_sel      : 1; /* [    2],        r/w,        0x0 */
            uint32_t xtal32k_amp_ctrl     : 2; /* [ 4: 3],        r/w,        0x1 */
            uint32_t xtal32k_reg          : 2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t xtal32k_outbuf_stre  : 1; /* [    7],        r/w,        0x0 */
            uint32_t xtal32k_otf_short    : 1; /* [    8],        r/w,        0x0 */
            uint32_t xtal32k_inv_stre     : 2; /* [10: 9],        r/w,        0x1 */
            uint32_t xtal32k_capbank      : 6; /* [16:11],        r/w,       0x20 */
            uint32_t xtal32k_ac_cap_short : 1; /* [   17],        r/w,        0x0 */
            uint32_t pu_xtal32k_buf       : 1; /* [   18],        r/w,        0x1 */
            uint32_t pu_xtal32k           : 1; /* [   19],        r/w,        0x0 */
            uint32_t xtal32k_hiz_en       : 1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21          : 1; /* [   21],       rsvd,        0x0 */
            uint32_t dten_xtal32k         : 1; /* [   22],        r/w,        0x0 */
            uint32_t ten_xtal32k          : 1; /* [   23],        r/w,        0x0 */
            uint32_t f32k_sel_rtc         : 1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31       : 7; /* [31:25],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal32k;

    /* 0x208 : rtc_rst_ctrl */
    union {
        struct {
            uint32_t rtc_rst_wait_cnt_rtc : 16; /* [15: 0],        r/w,     0x3c00 */
            uint32_t rtc_rst_refdiv_rtc   : 3;  /* [18:16],        r/w,        0x4 */
            uint32_t rtc_rst_ctrl_misc    : 13; /* [31:19],        r/w,      0xa12 */
        } BF;
        uint32_t WORD;
    } rtc_rst_ctrl;

    /* 0x20C : rtc_rst_ctrl2 */
    union {
        struct {
            uint32_t rtc_resv           : 8;  /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_en_hw_pu_rc32k : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31      : 23; /* [31: 9],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rtc_rst_ctrl2;
};

typedef volatile struct hbn_reg hbn_reg_t;

#endif /* __HBN_REG_H__ */
