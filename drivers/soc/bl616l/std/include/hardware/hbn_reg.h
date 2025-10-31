/**
  ******************************************************************************
  * @file    hbn_reg.h
  * @version V1.0
  * @date    2024-12-09
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

#include "bl616l.h"

/* 0x0 : HBN_CTL */
#define HBN_CTL_OFFSET                      (0x0)
#define HBN_RTC_CTL                         HBN_RTC_CTL
#define HBN_RTC_CTL_POS                     (0U)
#define HBN_RTC_CTL_LEN                     (4U)
#define HBN_RTC_CTL_MSK                     (((1U << HBN_RTC_CTL_LEN) - 1) << HBN_RTC_CTL_POS)
#define HBN_RTC_CTL_UMSK                    (~(((1U << HBN_RTC_CTL_LEN) - 1) << HBN_RTC_CTL_POS))
#define HBN_RTC_DLY_OPTION                  HBN_RTC_DLY_OPTION
#define HBN_RTC_DLY_OPTION_POS              (4U)
#define HBN_RTC_DLY_OPTION_LEN              (1U)
#define HBN_RTC_DLY_OPTION_MSK              (((1U << HBN_RTC_DLY_OPTION_LEN) - 1) << HBN_RTC_DLY_OPTION_POS)
#define HBN_RTC_DLY_OPTION_UMSK             (~(((1U << HBN_RTC_DLY_OPTION_LEN) - 1) << HBN_RTC_DLY_OPTION_POS))
#define HBN_SWRST_WDT                       HBN_SWRST_WDT
#define HBN_SWRST_WDT_POS                   (6U)
#define HBN_SWRST_WDT_LEN                   (1U)
#define HBN_SWRST_WDT_MSK                   (((1U << HBN_SWRST_WDT_LEN) - 1) << HBN_SWRST_WDT_POS)
#define HBN_SWRST_WDT_UMSK                  (~(((1U << HBN_SWRST_WDT_LEN) - 1) << HBN_SWRST_WDT_POS))
#define HBN_MODE                            HBN_MODE
#define HBN_MODE_POS                        (7U)
#define HBN_MODE_LEN                        (1U)
#define HBN_MODE_MSK                        (((1U << HBN_MODE_LEN) - 1) << HBN_MODE_POS)
#define HBN_MODE_UMSK                       (~(((1U << HBN_MODE_LEN) - 1) << HBN_MODE_POS))
#define HBN_TRAP_MODE                       HBN_TRAP_MODE
#define HBN_TRAP_MODE_POS                   (8U)
#define HBN_TRAP_MODE_LEN                   (1U)
#define HBN_TRAP_MODE_MSK                   (((1U << HBN_TRAP_MODE_LEN) - 1) << HBN_TRAP_MODE_POS)
#define HBN_TRAP_MODE_UMSK                  (~(((1U << HBN_TRAP_MODE_LEN) - 1) << HBN_TRAP_MODE_POS))
#define HBN_PWRDN_HBN_CORE                  HBN_PWRDN_HBN_CORE
#define HBN_PWRDN_HBN_CORE_POS              (9U)
#define HBN_PWRDN_HBN_CORE_LEN              (1U)
#define HBN_PWRDN_HBN_CORE_MSK              (((1U << HBN_PWRDN_HBN_CORE_LEN) - 1) << HBN_PWRDN_HBN_CORE_POS)
#define HBN_PWRDN_HBN_CORE_UMSK             (~(((1U << HBN_PWRDN_HBN_CORE_LEN) - 1) << HBN_PWRDN_HBN_CORE_POS))
#define HBN_PWRDN_HBN_RTC                   HBN_PWRDN_HBN_RTC
#define HBN_PWRDN_HBN_RTC_POS               (11U)
#define HBN_PWRDN_HBN_RTC_LEN               (1U)
#define HBN_PWRDN_HBN_RTC_MSK               (((1U << HBN_PWRDN_HBN_RTC_LEN) - 1) << HBN_PWRDN_HBN_RTC_POS)
#define HBN_PWRDN_HBN_RTC_UMSK              (~(((1U << HBN_PWRDN_HBN_RTC_LEN) - 1) << HBN_PWRDN_HBN_RTC_POS))
#define HBN_SW_RST                          HBN_SW_RST
#define HBN_SW_RST_POS                      (12U)
#define HBN_SW_RST_LEN                      (1U)
#define HBN_SW_RST_MSK                      (((1U << HBN_SW_RST_LEN) - 1) << HBN_SW_RST_POS)
#define HBN_SW_RST_UMSK                     (~(((1U << HBN_SW_RST_LEN) - 1) << HBN_SW_RST_POS))
#define HBN_DIS_PWR_OFF_LDO_SOC             HBN_DIS_PWR_OFF_LDO_SOC
#define HBN_DIS_PWR_OFF_LDO_SOC_POS         (13U)
#define HBN_DIS_PWR_OFF_LDO_SOC_LEN         (1U)
#define HBN_DIS_PWR_OFF_LDO_SOC_MSK         (((1U << HBN_DIS_PWR_OFF_LDO_SOC_LEN) - 1) << HBN_DIS_PWR_OFF_LDO_SOC_POS)
#define HBN_DIS_PWR_OFF_LDO_SOC_UMSK        (~(((1U << HBN_DIS_PWR_OFF_LDO_SOC_LEN) - 1) << HBN_DIS_PWR_OFF_LDO_SOC_POS))
#define HBN_DIS_PWR_OFF_LDO13SYS            HBN_DIS_PWR_OFF_LDO13SYS
#define HBN_DIS_PWR_OFF_LDO13SYS_POS        (14U)
#define HBN_DIS_PWR_OFF_LDO13SYS_LEN        (1U)
#define HBN_DIS_PWR_OFF_LDO13SYS_MSK        (((1U << HBN_DIS_PWR_OFF_LDO13SYS_LEN) - 1) << HBN_DIS_PWR_OFF_LDO13SYS_POS)
#define HBN_DIS_PWR_OFF_LDO13SYS_UMSK       (~(((1U << HBN_DIS_PWR_OFF_LDO13SYS_LEN) - 1) << HBN_DIS_PWR_OFF_LDO13SYS_POS))
#define HBN_LDO08AON_VOUT_TRIM_AON          HBN_LDO08AON_VOUT_TRIM_AON
#define HBN_LDO08AON_VOUT_TRIM_AON_POS      (15U)
#define HBN_LDO08AON_VOUT_TRIM_AON_LEN      (4U)
#define HBN_LDO08AON_VOUT_TRIM_AON_MSK      (((1U << HBN_LDO08AON_VOUT_TRIM_AON_LEN) - 1) << HBN_LDO08AON_VOUT_TRIM_AON_POS)
#define HBN_LDO08AON_VOUT_TRIM_AON_UMSK     (~(((1U << HBN_LDO08AON_VOUT_TRIM_AON_LEN) - 1) << HBN_LDO08AON_VOUT_TRIM_AON_POS))
#define HBN_LDO08AON_VOUT_SEL_AON           HBN_LDO08AON_VOUT_SEL_AON
#define HBN_LDO08AON_VOUT_SEL_AON_POS       (19U)
#define HBN_LDO08AON_VOUT_SEL_AON_LEN       (4U)
#define HBN_LDO08AON_VOUT_SEL_AON_MSK       (((1U << HBN_LDO08AON_VOUT_SEL_AON_LEN) - 1) << HBN_LDO08AON_VOUT_SEL_AON_POS)
#define HBN_LDO08AON_VOUT_SEL_AON_UMSK      (~(((1U << HBN_LDO08AON_VOUT_SEL_AON_LEN) - 1) << HBN_LDO08AON_VOUT_SEL_AON_POS))
#define HBN_LDO_LP_SOC_EN_AON_EN            HBN_LDO_LP_SOC_EN_AON_EN
#define HBN_LDO_LP_SOC_EN_AON_EN_POS        (23U)
#define HBN_LDO_LP_SOC_EN_AON_EN_LEN        (1U)
#define HBN_LDO_LP_SOC_EN_AON_EN_MSK        (((1U << HBN_LDO_LP_SOC_EN_AON_EN_LEN) - 1) << HBN_LDO_LP_SOC_EN_AON_EN_POS)
#define HBN_LDO_LP_SOC_EN_AON_EN_UMSK       (~(((1U << HBN_LDO_LP_SOC_EN_AON_EN_LEN) - 1) << HBN_LDO_LP_SOC_EN_AON_EN_POS))
#define HBN_PWR_ON_OPTION                   HBN_PWR_ON_OPTION
#define HBN_PWR_ON_OPTION_POS               (25U)
#define HBN_PWR_ON_OPTION_LEN               (1U)
#define HBN_PWR_ON_OPTION_MSK               (((1U << HBN_PWR_ON_OPTION_LEN) - 1) << HBN_PWR_ON_OPTION_POS)
#define HBN_PWR_ON_OPTION_UMSK              (~(((1U << HBN_PWR_ON_OPTION_LEN) - 1) << HBN_PWR_ON_OPTION_POS))
#define HBN_SRAM_SLP_OPTION                 HBN_SRAM_SLP_OPTION
#define HBN_SRAM_SLP_OPTION_POS             (26U)
#define HBN_SRAM_SLP_OPTION_LEN             (1U)
#define HBN_SRAM_SLP_OPTION_MSK             (((1U << HBN_SRAM_SLP_OPTION_LEN) - 1) << HBN_SRAM_SLP_OPTION_POS)
#define HBN_SRAM_SLP_OPTION_UMSK            (~(((1U << HBN_SRAM_SLP_OPTION_LEN) - 1) << HBN_SRAM_SLP_OPTION_POS))
#define HBN_SRAM_SLP                        HBN_SRAM_SLP
#define HBN_SRAM_SLP_POS                    (27U)
#define HBN_SRAM_SLP_LEN                    (1U)
#define HBN_SRAM_SLP_MSK                    (((1U << HBN_SRAM_SLP_LEN) - 1) << HBN_SRAM_SLP_POS)
#define HBN_SRAM_SLP_UMSK                   (~(((1U << HBN_SRAM_SLP_LEN) - 1) << HBN_SRAM_SLP_POS))
#define HBN_STATE                           HBN_STATE
#define HBN_STATE_POS                       (28U)
#define HBN_STATE_LEN                       (4U)
#define HBN_STATE_MSK                       (((1U << HBN_STATE_LEN) - 1) << HBN_STATE_POS)
#define HBN_STATE_UMSK                      (~(((1U << HBN_STATE_LEN) - 1) << HBN_STATE_POS))

/* 0x4 : HBN_TIME_L */
#define HBN_TIME_L_OFFSET                   (0x4)
#define HBN_TIME_L                          HBN_TIME_L
#define HBN_TIME_L_POS                      (0U)
#define HBN_TIME_L_LEN                      (32U)
#define HBN_TIME_L_MSK                      (((1U << HBN_TIME_L_LEN) - 1) << HBN_TIME_L_POS)
#define HBN_TIME_L_UMSK                     (~(((1U << HBN_TIME_L_LEN) - 1) << HBN_TIME_L_POS))

/* 0x8 : HBN_TIME_H */
#define HBN_TIME_H_OFFSET                   (0x8)
#define HBN_TIME_H                          HBN_TIME_H
#define HBN_TIME_H_POS                      (0U)
#define HBN_TIME_H_LEN                      (16U)
#define HBN_TIME_H_MSK                      (((1U << HBN_TIME_H_LEN) - 1) << HBN_TIME_H_POS)
#define HBN_TIME_H_UMSK                     (~(((1U << HBN_TIME_H_LEN) - 1) << HBN_TIME_H_POS))
#define HBN_PDS_HBN_XTAL_DIV                HBN_PDS_HBN_XTAL_DIV
#define HBN_PDS_HBN_XTAL_DIV_POS            (16U)
#define HBN_PDS_HBN_XTAL_DIV_LEN            (11U)
#define HBN_PDS_HBN_XTAL_DIV_MSK            (((1U << HBN_PDS_HBN_XTAL_DIV_LEN) - 1) << HBN_PDS_HBN_XTAL_DIV_POS)
#define HBN_PDS_HBN_XTAL_DIV_UMSK           (~(((1U << HBN_PDS_HBN_XTAL_DIV_LEN) - 1) << HBN_PDS_HBN_XTAL_DIV_POS))
#define HBN_PDS_HBN_XTAL_DIV_EN             HBN_PDS_HBN_XTAL_DIV_EN
#define HBN_PDS_HBN_XTAL_DIV_EN_POS         (27U)
#define HBN_PDS_HBN_XTAL_DIV_EN_LEN         (1U)
#define HBN_PDS_HBN_XTAL_DIV_EN_MSK         (((1U << HBN_PDS_HBN_XTAL_DIV_EN_LEN) - 1) << HBN_PDS_HBN_XTAL_DIV_EN_POS)
#define HBN_PDS_HBN_XTAL_DIV_EN_UMSK        (~(((1U << HBN_PDS_HBN_XTAL_DIV_EN_LEN) - 1) << HBN_PDS_HBN_XTAL_DIV_EN_POS))
#define HBN_PDS_HBN_RTC_SLOW_CLK_SEL        HBN_PDS_HBN_RTC_SLOW_CLK_SEL
#define HBN_PDS_HBN_RTC_SLOW_CLK_SEL_POS    (28U)
#define HBN_PDS_HBN_RTC_SLOW_CLK_SEL_LEN    (1U)
#define HBN_PDS_HBN_RTC_SLOW_CLK_SEL_MSK    (((1U << HBN_PDS_HBN_RTC_SLOW_CLK_SEL_LEN) - 1) << HBN_PDS_HBN_RTC_SLOW_CLK_SEL_POS)
#define HBN_PDS_HBN_RTC_SLOW_CLK_SEL_UMSK   (~(((1U << HBN_PDS_HBN_RTC_SLOW_CLK_SEL_LEN) - 1) << HBN_PDS_HBN_RTC_SLOW_CLK_SEL_POS))

/* 0xC : RTC_TIME_L */
#define HBN_RTC_TIME_L_OFFSET               (0xC)
#define HBN_RTC_TIME_LATCH_L                HBN_RTC_TIME_LATCH_L
#define HBN_RTC_TIME_LATCH_L_POS            (0U)
#define HBN_RTC_TIME_LATCH_L_LEN            (32U)
#define HBN_RTC_TIME_LATCH_L_MSK            (((1U << HBN_RTC_TIME_LATCH_L_LEN) - 1) << HBN_RTC_TIME_LATCH_L_POS)
#define HBN_RTC_TIME_LATCH_L_UMSK           (~(((1U << HBN_RTC_TIME_LATCH_L_LEN) - 1) << HBN_RTC_TIME_LATCH_L_POS))

/* 0x10 : RTC_TIME_H */
#define HBN_RTC_TIME_H_OFFSET               (0x10)
#define HBN_RTC_TIME_LATCH_H                HBN_RTC_TIME_LATCH_H
#define HBN_RTC_TIME_LATCH_H_POS            (0U)
#define HBN_RTC_TIME_LATCH_H_LEN            (16U)
#define HBN_RTC_TIME_LATCH_H_MSK            (((1U << HBN_RTC_TIME_LATCH_H_LEN) - 1) << HBN_RTC_TIME_LATCH_H_POS)
#define HBN_RTC_TIME_LATCH_H_UMSK           (~(((1U << HBN_RTC_TIME_LATCH_H_LEN) - 1) << HBN_RTC_TIME_LATCH_H_POS))
#define HBN_RTC_TIME_LATCH                  HBN_RTC_TIME_LATCH
#define HBN_RTC_TIME_LATCH_POS              (31U)
#define HBN_RTC_TIME_LATCH_LEN              (1U)
#define HBN_RTC_TIME_LATCH_MSK              (((1U << HBN_RTC_TIME_LATCH_LEN) - 1) << HBN_RTC_TIME_LATCH_POS)
#define HBN_RTC_TIME_LATCH_UMSK             (~(((1U << HBN_RTC_TIME_LATCH_LEN) - 1) << HBN_RTC_TIME_LATCH_POS))

/* 0x14 : HBN_IRQ_MODE */
#define HBN_IRQ_MODE_OFFSET                 (0x14)
#define HBN_PIN_WAKEUP_MODE                 HBN_PIN_WAKEUP_MODE
#define HBN_PIN_WAKEUP_MODE_POS             (0U)
#define HBN_PIN_WAKEUP_MODE_LEN             (4U)
#define HBN_PIN_WAKEUP_MODE_MSK             (((1U << HBN_PIN_WAKEUP_MODE_LEN) - 1) << HBN_PIN_WAKEUP_MODE_POS)
#define HBN_PIN_WAKEUP_MODE_UMSK            (~(((1U << HBN_PIN_WAKEUP_MODE_LEN) - 1) << HBN_PIN_WAKEUP_MODE_POS))
#define HBN_PIN_WAKEUP_MASK                 HBN_PIN_WAKEUP_MASK
#define HBN_PIN_WAKEUP_MASK_POS             (4U)
#define HBN_PIN_WAKEUP_MASK_LEN             (6U)
#define HBN_PIN_WAKEUP_MASK_MSK             (((1U << HBN_PIN_WAKEUP_MASK_LEN) - 1) << HBN_PIN_WAKEUP_MASK_POS)
#define HBN_PIN_WAKEUP_MASK_UMSK            (~(((1U << HBN_PIN_WAKEUP_MASK_LEN) - 1) << HBN_PIN_WAKEUP_MASK_POS))
#define HBN_REG_EN_HW_PU_PD                 HBN_REG_EN_HW_PU_PD
#define HBN_REG_EN_HW_PU_PD_POS             (16U)
#define HBN_REG_EN_HW_PU_PD_LEN             (1U)
#define HBN_REG_EN_HW_PU_PD_MSK             (((1U << HBN_REG_EN_HW_PU_PD_LEN) - 1) << HBN_REG_EN_HW_PU_PD_POS)
#define HBN_REG_EN_HW_PU_PD_UMSK            (~(((1U << HBN_REG_EN_HW_PU_PD_LEN) - 1) << HBN_REG_EN_HW_PU_PD_POS))
#define HBN_IRQ_BOR_EN                      HBN_IRQ_BOR_EN
#define HBN_IRQ_BOR_EN_POS                  (18U)
#define HBN_IRQ_BOR_EN_LEN                  (1U)
#define HBN_IRQ_BOR_EN_MSK                  (((1U << HBN_IRQ_BOR_EN_LEN) - 1) << HBN_IRQ_BOR_EN_POS)
#define HBN_IRQ_BOR_EN_UMSK                 (~(((1U << HBN_IRQ_BOR_EN_LEN) - 1) << HBN_IRQ_BOR_EN_POS))
#define HBN_IRQ_ACOMP0_EN                   HBN_IRQ_ACOMP0_EN
#define HBN_IRQ_ACOMP0_EN_POS               (20U)
#define HBN_IRQ_ACOMP0_EN_LEN               (2U)
#define HBN_IRQ_ACOMP0_EN_MSK               (((1U << HBN_IRQ_ACOMP0_EN_LEN) - 1) << HBN_IRQ_ACOMP0_EN_POS)
#define HBN_IRQ_ACOMP0_EN_UMSK              (~(((1U << HBN_IRQ_ACOMP0_EN_LEN) - 1) << HBN_IRQ_ACOMP0_EN_POS))
#define HBN_IRQ_ACOMP1_EN                   HBN_IRQ_ACOMP1_EN
#define HBN_IRQ_ACOMP1_EN_POS               (22U)
#define HBN_IRQ_ACOMP1_EN_LEN               (2U)
#define HBN_IRQ_ACOMP1_EN_MSK               (((1U << HBN_IRQ_ACOMP1_EN_LEN) - 1) << HBN_IRQ_ACOMP1_EN_POS)
#define HBN_IRQ_ACOMP1_EN_UMSK              (~(((1U << HBN_IRQ_ACOMP1_EN_LEN) - 1) << HBN_IRQ_ACOMP1_EN_POS))
#define HBN_PIN_WAKEUP_SEL                  HBN_PIN_WAKEUP_SEL
#define HBN_PIN_WAKEUP_SEL_POS              (24U)
#define HBN_PIN_WAKEUP_SEL_LEN              (3U)
#define HBN_PIN_WAKEUP_SEL_MSK              (((1U << HBN_PIN_WAKEUP_SEL_LEN) - 1) << HBN_PIN_WAKEUP_SEL_POS)
#define HBN_PIN_WAKEUP_SEL_UMSK             (~(((1U << HBN_PIN_WAKEUP_SEL_LEN) - 1) << HBN_PIN_WAKEUP_SEL_POS))
#define HBN_PIN_WAKEUP_EN                   HBN_PIN_WAKEUP_EN
#define HBN_PIN_WAKEUP_EN_POS               (27U)
#define HBN_PIN_WAKEUP_EN_LEN               (1U)
#define HBN_PIN_WAKEUP_EN_MSK               (((1U << HBN_PIN_WAKEUP_EN_LEN) - 1) << HBN_PIN_WAKEUP_EN_POS)
#define HBN_PIN_WAKEUP_EN_UMSK              (~(((1U << HBN_PIN_WAKEUP_EN_LEN) - 1) << HBN_PIN_WAKEUP_EN_POS))

/* 0x18 : HBN_IRQ_STAT */
#define HBN_IRQ_STAT_OFFSET                 (0x18)
#define HBN_IRQ_STAT                        HBN_IRQ_STAT
#define HBN_IRQ_STAT_POS                    (0U)
#define HBN_IRQ_STAT_LEN                    (32U)
#define HBN_IRQ_STAT_MSK                    (((1U << HBN_IRQ_STAT_LEN) - 1) << HBN_IRQ_STAT_POS)
#define HBN_IRQ_STAT_UMSK                   (~(((1U << HBN_IRQ_STAT_LEN) - 1) << HBN_IRQ_STAT_POS))

/* 0x1C : HBN_IRQ_CLR */
#define HBN_IRQ_CLR_OFFSET                  (0x1C)
#define HBN_IRQ_CLR                         HBN_IRQ_CLR
#define HBN_IRQ_CLR_POS                     (0U)
#define HBN_IRQ_CLR_LEN                     (32U)
#define HBN_IRQ_CLR_MSK                     (((1U << HBN_IRQ_CLR_LEN) - 1) << HBN_IRQ_CLR_POS)
#define HBN_IRQ_CLR_UMSK                    (~(((1U << HBN_IRQ_CLR_LEN) - 1) << HBN_IRQ_CLR_POS))

/* 0x20 : HBN_EXT_DCDC_CFG */
#define HBN_EXT_DCDC_CFG_OFFSET             (0x20)
#define HBN_EXT_DCDC1_AON_CTRL_HW_EN        HBN_EXT_DCDC1_AON_CTRL_HW_EN
#define HBN_EXT_DCDC1_AON_CTRL_HW_EN_POS    (0U)
#define HBN_EXT_DCDC1_AON_CTRL_HW_EN_LEN    (1U)
#define HBN_EXT_DCDC1_AON_CTRL_HW_EN_MSK    (((1U << HBN_EXT_DCDC1_AON_CTRL_HW_EN_LEN) - 1) << HBN_EXT_DCDC1_AON_CTRL_HW_EN_POS)
#define HBN_EXT_DCDC1_AON_CTRL_HW_EN_UMSK   (~(((1U << HBN_EXT_DCDC1_AON_CTRL_HW_EN_LEN) - 1) << HBN_EXT_DCDC1_AON_CTRL_HW_EN_POS))
#define HBN_EXT_DCDC1_AON_GPIO_SEL          HBN_EXT_DCDC1_AON_GPIO_SEL
#define HBN_EXT_DCDC1_AON_GPIO_SEL_POS      (1U)
#define HBN_EXT_DCDC1_AON_GPIO_SEL_LEN      (3U)
#define HBN_EXT_DCDC1_AON_GPIO_SEL_MSK      (((1U << HBN_EXT_DCDC1_AON_GPIO_SEL_LEN) - 1) << HBN_EXT_DCDC1_AON_GPIO_SEL_POS)
#define HBN_EXT_DCDC1_AON_GPIO_SEL_UMSK     (~(((1U << HBN_EXT_DCDC1_AON_GPIO_SEL_LEN) - 1) << HBN_EXT_DCDC1_AON_GPIO_SEL_POS))
#define HBN_HW_EXT_DCDC1_ON_EN              HBN_HW_EXT_DCDC1_ON_EN
#define HBN_HW_EXT_DCDC1_ON_EN_POS          (4U)
#define HBN_HW_EXT_DCDC1_ON_EN_LEN          (1U)
#define HBN_HW_EXT_DCDC1_ON_EN_MSK          (((1U << HBN_HW_EXT_DCDC1_ON_EN_LEN) - 1) << HBN_HW_EXT_DCDC1_ON_EN_POS)
#define HBN_HW_EXT_DCDC1_ON_EN_UMSK         (~(((1U << HBN_HW_EXT_DCDC1_ON_EN_LEN) - 1) << HBN_HW_EXT_DCDC1_ON_EN_POS))
#define HBN_EXT_DCDC2_AON_CTRL_HW_EN        HBN_EXT_DCDC2_AON_CTRL_HW_EN
#define HBN_EXT_DCDC2_AON_CTRL_HW_EN_POS    (5U)
#define HBN_EXT_DCDC2_AON_CTRL_HW_EN_LEN    (1U)
#define HBN_EXT_DCDC2_AON_CTRL_HW_EN_MSK    (((1U << HBN_EXT_DCDC2_AON_CTRL_HW_EN_LEN) - 1) << HBN_EXT_DCDC2_AON_CTRL_HW_EN_POS)
#define HBN_EXT_DCDC2_AON_CTRL_HW_EN_UMSK   (~(((1U << HBN_EXT_DCDC2_AON_CTRL_HW_EN_LEN) - 1) << HBN_EXT_DCDC2_AON_CTRL_HW_EN_POS))
#define HBN_EXT_DCDC2_AON_GPIO_SEL          HBN_EXT_DCDC2_AON_GPIO_SEL
#define HBN_EXT_DCDC2_AON_GPIO_SEL_POS      (6U)
#define HBN_EXT_DCDC2_AON_GPIO_SEL_LEN      (3U)
#define HBN_EXT_DCDC2_AON_GPIO_SEL_MSK      (((1U << HBN_EXT_DCDC2_AON_GPIO_SEL_LEN) - 1) << HBN_EXT_DCDC2_AON_GPIO_SEL_POS)
#define HBN_EXT_DCDC2_AON_GPIO_SEL_UMSK     (~(((1U << HBN_EXT_DCDC2_AON_GPIO_SEL_LEN) - 1) << HBN_EXT_DCDC2_AON_GPIO_SEL_POS))
#define HBN_HW_EXT_DCDC2_ON_EN              HBN_HW_EXT_DCDC2_ON_EN
#define HBN_HW_EXT_DCDC2_ON_EN_POS          (9U)
#define HBN_HW_EXT_DCDC2_ON_EN_LEN          (1U)
#define HBN_HW_EXT_DCDC2_ON_EN_MSK          (((1U << HBN_HW_EXT_DCDC2_ON_EN_LEN) - 1) << HBN_HW_EXT_DCDC2_ON_EN_POS)
#define HBN_HW_EXT_DCDC2_ON_EN_UMSK         (~(((1U << HBN_HW_EXT_DCDC2_ON_EN_LEN) - 1) << HBN_HW_EXT_DCDC2_ON_EN_POS))

/* 0x2C : HBN_BOR_CFG */
#define HBN_BOR_CFG_OFFSET                  (0x2C)
#define HBN_BOD_SEL_AON                     HBN_BOD_SEL_AON
#define HBN_BOD_SEL_AON_POS                 (0U)
#define HBN_BOD_SEL_AON_LEN                 (1U)
#define HBN_BOD_SEL_AON_MSK                 (((1U << HBN_BOD_SEL_AON_LEN) - 1) << HBN_BOD_SEL_AON_POS)
#define HBN_BOD_SEL_AON_UMSK                (~(((1U << HBN_BOD_SEL_AON_LEN) - 1) << HBN_BOD_SEL_AON_POS))
#define HBN_BOD_TH_AON                      HBN_BOD_TH_AON
#define HBN_BOD_TH_AON_POS                  (1U)
#define HBN_BOD_TH_AON_LEN                  (3U)
#define HBN_BOD_TH_AON_MSK                  (((1U << HBN_BOD_TH_AON_LEN) - 1) << HBN_BOD_TH_AON_POS)
#define HBN_BOD_TH_AON_UMSK                 (~(((1U << HBN_BOD_TH_AON_LEN) - 1) << HBN_BOD_TH_AON_POS))
#define HBN_PU_BOD_AON                      HBN_PU_BOD_AON
#define HBN_PU_BOD_AON_POS                  (4U)
#define HBN_PU_BOD_AON_LEN                  (1U)
#define HBN_PU_BOD_AON_MSK                  (((1U << HBN_PU_BOD_AON_LEN) - 1) << HBN_PU_BOD_AON_POS)
#define HBN_PU_BOD_AON_UMSK                 (~(((1U << HBN_PU_BOD_AON_LEN) - 1) << HBN_PU_BOD_AON_POS))
#define HBN_BOD_OUT                         HBN_BOD_OUT
#define HBN_BOD_OUT_POS                     (5U)
#define HBN_BOD_OUT_LEN                     (1U)
#define HBN_BOD_OUT_MSK                     (((1U << HBN_BOD_OUT_LEN) - 1) << HBN_BOD_OUT_POS)
#define HBN_BOD_OUT_UMSK                    (~(((1U << HBN_BOD_OUT_LEN) - 1) << HBN_BOD_OUT_POS))
#define HBN_POR_OUT                         HBN_POR_OUT
#define HBN_POR_OUT_POS                     (6U)
#define HBN_POR_OUT_LEN                     (1U)
#define HBN_POR_OUT_MSK                     (((1U << HBN_POR_OUT_LEN) - 1) << HBN_POR_OUT_POS)
#define HBN_POR_OUT_UMSK                    (~(((1U << HBN_POR_OUT_LEN) - 1) << HBN_POR_OUT_POS))

/* 0x30 : HBN_GLB */
#define HBN_GLB_OFFSET                      (0x30)
#define HBN_ROOT_CLK_SEL                    HBN_ROOT_CLK_SEL
#define HBN_ROOT_CLK_SEL_POS                (0U)
#define HBN_ROOT_CLK_SEL_LEN                (2U)
#define HBN_ROOT_CLK_SEL_MSK                (((1U << HBN_ROOT_CLK_SEL_LEN) - 1) << HBN_ROOT_CLK_SEL_POS)
#define HBN_ROOT_CLK_SEL_UMSK               (~(((1U << HBN_ROOT_CLK_SEL_LEN) - 1) << HBN_ROOT_CLK_SEL_POS))
#define HBN_UART_CLK_SEL                    HBN_UART_CLK_SEL
#define HBN_UART_CLK_SEL_POS                (2U)
#define HBN_UART_CLK_SEL_LEN                (1U)
#define HBN_UART_CLK_SEL_MSK                (((1U << HBN_UART_CLK_SEL_LEN) - 1) << HBN_UART_CLK_SEL_POS)
#define HBN_UART_CLK_SEL_UMSK               (~(((1U << HBN_UART_CLK_SEL_LEN) - 1) << HBN_UART_CLK_SEL_POS))
#define HBN_F32K_SEL                        HBN_F32K_SEL
#define HBN_F32K_SEL_POS                    (3U)
#define HBN_F32K_SEL_LEN                    (2U)
#define HBN_F32K_SEL_MSK                    (((1U << HBN_F32K_SEL_LEN) - 1) << HBN_F32K_SEL_POS)
#define HBN_F32K_SEL_UMSK                   (~(((1U << HBN_F32K_SEL_LEN) - 1) << HBN_F32K_SEL_POS))
#define HBN_PU_RC32K                        HBN_PU_RC32K
#define HBN_PU_RC32K_POS                    (5U)
#define HBN_PU_RC32K_LEN                    (1U)
#define HBN_PU_RC32K_MSK                    (((1U << HBN_PU_RC32K_LEN) - 1) << HBN_PU_RC32K_POS)
#define HBN_PU_RC32K_UMSK                   (~(((1U << HBN_PU_RC32K_LEN) - 1) << HBN_PU_RC32K_POS))
#define HBN_RESET_EVENT                     HBN_RESET_EVENT
#define HBN_RESET_EVENT_POS                 (7U)
#define HBN_RESET_EVENT_LEN                 (6U)
#define HBN_RESET_EVENT_MSK                 (((1U << HBN_RESET_EVENT_LEN) - 1) << HBN_RESET_EVENT_POS)
#define HBN_RESET_EVENT_UMSK                (~(((1U << HBN_RESET_EVENT_LEN) - 1) << HBN_RESET_EVENT_POS))
#define HBN_CLR_RESET_EVENT                 HBN_CLR_RESET_EVENT
#define HBN_CLR_RESET_EVENT_POS             (13U)
#define HBN_CLR_RESET_EVENT_LEN             (1U)
#define HBN_CLR_RESET_EVENT_MSK             (((1U << HBN_CLR_RESET_EVENT_LEN) - 1) << HBN_CLR_RESET_EVENT_POS)
#define HBN_CLR_RESET_EVENT_UMSK            (~(((1U << HBN_CLR_RESET_EVENT_LEN) - 1) << HBN_CLR_RESET_EVENT_POS))
#define HBN_GPIO_INT_DET_CLK_SEL            HBN_GPIO_INT_DET_CLK_SEL
#define HBN_GPIO_INT_DET_CLK_SEL_POS        (14U)
#define HBN_GPIO_INT_DET_CLK_SEL_LEN        (1U)
#define HBN_GPIO_INT_DET_CLK_SEL_MSK        (((1U << HBN_GPIO_INT_DET_CLK_SEL_LEN) - 1) << HBN_GPIO_INT_DET_CLK_SEL_POS)
#define HBN_GPIO_INT_DET_CLK_SEL_UMSK       (~(((1U << HBN_GPIO_INT_DET_CLK_SEL_LEN) - 1) << HBN_GPIO_INT_DET_CLK_SEL_POS))
#define HBN_UART_CLK_SEL2                   HBN_UART_CLK_SEL2
#define HBN_UART_CLK_SEL2_POS               (15U)
#define HBN_UART_CLK_SEL2_LEN               (1U)
#define HBN_UART_CLK_SEL2_MSK               (((1U << HBN_UART_CLK_SEL2_LEN) - 1) << HBN_UART_CLK_SEL2_POS)
#define HBN_UART_CLK_SEL2_UMSK              (~(((1U << HBN_UART_CLK_SEL2_LEN) - 1) << HBN_UART_CLK_SEL2_POS))
#define HBN_SW_LDO08AON_VOUT_TRIM_AON       HBN_SW_LDO08AON_VOUT_TRIM_AON
#define HBN_SW_LDO08AON_VOUT_TRIM_AON_POS   (24U)
#define HBN_SW_LDO08AON_VOUT_TRIM_AON_LEN   (4U)
#define HBN_SW_LDO08AON_VOUT_TRIM_AON_MSK   (((1U << HBN_SW_LDO08AON_VOUT_TRIM_AON_LEN) - 1) << HBN_SW_LDO08AON_VOUT_TRIM_AON_POS)
#define HBN_SW_LDO08AON_VOUT_TRIM_AON_UMSK  (~(((1U << HBN_SW_LDO08AON_VOUT_TRIM_AON_LEN) - 1) << HBN_SW_LDO08AON_VOUT_TRIM_AON_POS))
#define HBN_SW_LDO08AON_VOUT_SEL_AON        HBN_SW_LDO08AON_VOUT_SEL_AON
#define HBN_SW_LDO08AON_VOUT_SEL_AON_POS    (28U)
#define HBN_SW_LDO08AON_VOUT_SEL_AON_LEN    (4U)
#define HBN_SW_LDO08AON_VOUT_SEL_AON_MSK    (((1U << HBN_SW_LDO08AON_VOUT_SEL_AON_LEN) - 1) << HBN_SW_LDO08AON_VOUT_SEL_AON_POS)
#define HBN_SW_LDO08AON_VOUT_SEL_AON_UMSK   (~(((1U << HBN_SW_LDO08AON_VOUT_SEL_AON_LEN) - 1) << HBN_SW_LDO08AON_VOUT_SEL_AON_POS))

/* 0x34 : HBN_SRAM */
#define HBN_SRAM_OFFSET                     (0x34)
#define HBN_RETRAM_RET                      HBN_RETRAM_RET
#define HBN_RETRAM_RET_POS                  (6U)
#define HBN_RETRAM_RET_LEN                  (1U)
#define HBN_RETRAM_RET_MSK                  (((1U << HBN_RETRAM_RET_LEN) - 1) << HBN_RETRAM_RET_POS)
#define HBN_RETRAM_RET_UMSK                 (~(((1U << HBN_RETRAM_RET_LEN) - 1) << HBN_RETRAM_RET_POS))
#define HBN_RETRAM_SLP                      HBN_RETRAM_SLP
#define HBN_RETRAM_SLP_POS                  (7U)
#define HBN_RETRAM_SLP_LEN                  (1U)
#define HBN_RETRAM_SLP_MSK                  (((1U << HBN_RETRAM_SLP_LEN) - 1) << HBN_RETRAM_SLP_POS)
#define HBN_RETRAM_SLP_UMSK                 (~(((1U << HBN_RETRAM_SLP_LEN) - 1) << HBN_RETRAM_SLP_POS))
#define HBN_CR_HBNRAM_DVS_0                 HBN_CR_HBNRAM_DVS_0
#define HBN_CR_HBNRAM_DVS_0_POS             (8U)
#define HBN_CR_HBNRAM_DVS_0_LEN             (2U)
#define HBN_CR_HBNRAM_DVS_0_MSK             (((1U << HBN_CR_HBNRAM_DVS_0_LEN) - 1) << HBN_CR_HBNRAM_DVS_0_POS)
#define HBN_CR_HBNRAM_DVS_0_UMSK            (~(((1U << HBN_CR_HBNRAM_DVS_0_LEN) - 1) << HBN_CR_HBNRAM_DVS_0_POS))
#define HBN_CR_HBNRAM_DVSE_0                HBN_CR_HBNRAM_DVSE_0
#define HBN_CR_HBNRAM_DVSE_0_POS            (10U)
#define HBN_CR_HBNRAM_DVSE_0_LEN            (1U)
#define HBN_CR_HBNRAM_DVSE_0_MSK            (((1U << HBN_CR_HBNRAM_DVSE_0_LEN) - 1) << HBN_CR_HBNRAM_DVSE_0_POS)
#define HBN_CR_HBNRAM_DVSE_0_UMSK           (~(((1U << HBN_CR_HBNRAM_DVSE_0_LEN) - 1) << HBN_CR_HBNRAM_DVSE_0_POS))
#define HBN_CR_HBNRAM_NAP_0                 HBN_CR_HBNRAM_NAP_0
#define HBN_CR_HBNRAM_NAP_0_POS             (11U)
#define HBN_CR_HBNRAM_NAP_0_LEN             (1U)
#define HBN_CR_HBNRAM_NAP_0_MSK             (((1U << HBN_CR_HBNRAM_NAP_0_LEN) - 1) << HBN_CR_HBNRAM_NAP_0_POS)
#define HBN_CR_HBNRAM_NAP_0_UMSK            (~(((1U << HBN_CR_HBNRAM_NAP_0_LEN) - 1) << HBN_CR_HBNRAM_NAP_0_POS))
#define HBN_CR_HBNRAM_TMCK_0                HBN_CR_HBNRAM_TMCK_0
#define HBN_CR_HBNRAM_TMCK_0_POS            (14U)
#define HBN_CR_HBNRAM_TMCK_0_LEN            (1U)
#define HBN_CR_HBNRAM_TMCK_0_MSK            (((1U << HBN_CR_HBNRAM_TMCK_0_LEN) - 1) << HBN_CR_HBNRAM_TMCK_0_POS)
#define HBN_CR_HBNRAM_TMCK_0_UMSK           (~(((1U << HBN_CR_HBNRAM_TMCK_0_LEN) - 1) << HBN_CR_HBNRAM_TMCK_0_POS))

/* 0x38 : HBN_PAD_CTRL_0 */
#define HBN_PAD_CTRL_0_OFFSET               (0x38)
#define HBN_REG_AON_PAD_IE_SMT              HBN_REG_AON_PAD_IE_SMT
#define HBN_REG_AON_PAD_IE_SMT_POS          (0U)
#define HBN_REG_AON_PAD_IE_SMT_LEN          (6U)
#define HBN_REG_AON_PAD_IE_SMT_MSK          (((1U << HBN_REG_AON_PAD_IE_SMT_LEN) - 1) << HBN_REG_AON_PAD_IE_SMT_POS)
#define HBN_REG_AON_PAD_IE_SMT_UMSK         (~(((1U << HBN_REG_AON_PAD_IE_SMT_LEN) - 1) << HBN_REG_AON_PAD_IE_SMT_POS))
#define HBN_REG_AON_LED_SEL                 HBN_REG_AON_LED_SEL
#define HBN_REG_AON_LED_SEL_POS             (10U)
#define HBN_REG_AON_LED_SEL_LEN             (9U)
#define HBN_REG_AON_LED_SEL_MSK             (((1U << HBN_REG_AON_LED_SEL_LEN) - 1) << HBN_REG_AON_LED_SEL_POS)
#define HBN_REG_AON_LED_SEL_UMSK            (~(((1U << HBN_REG_AON_LED_SEL_LEN) - 1) << HBN_REG_AON_LED_SEL_POS))
#define HBN_REG_EN_AON_CTRL_GPIO            HBN_REG_EN_AON_CTRL_GPIO
#define HBN_REG_EN_AON_CTRL_GPIO_POS        (20U)
#define HBN_REG_EN_AON_CTRL_GPIO_LEN        (6U)
#define HBN_REG_EN_AON_CTRL_GPIO_MSK        (((1U << HBN_REG_EN_AON_CTRL_GPIO_LEN) - 1) << HBN_REG_EN_AON_CTRL_GPIO_POS)
#define HBN_REG_EN_AON_CTRL_GPIO_UMSK       (~(((1U << HBN_REG_EN_AON_CTRL_GPIO_LEN) - 1) << HBN_REG_EN_AON_CTRL_GPIO_POS))
#define HBN_CR_GPIO_KEEP_EN                 HBN_CR_GPIO_KEEP_EN
#define HBN_CR_GPIO_KEEP_EN_POS             (28U)
#define HBN_CR_GPIO_KEEP_EN_LEN             (3U)
#define HBN_CR_GPIO_KEEP_EN_MSK             (((1U << HBN_CR_GPIO_KEEP_EN_LEN) - 1) << HBN_CR_GPIO_KEEP_EN_POS)
#define HBN_CR_GPIO_KEEP_EN_UMSK            (~(((1U << HBN_CR_GPIO_KEEP_EN_LEN) - 1) << HBN_CR_GPIO_KEEP_EN_POS))
#define HBN_REG_AON_GPIO_ISO_MODE           HBN_REG_AON_GPIO_ISO_MODE
#define HBN_REG_AON_GPIO_ISO_MODE_POS       (31U)
#define HBN_REG_AON_GPIO_ISO_MODE_LEN       (1U)
#define HBN_REG_AON_GPIO_ISO_MODE_MSK       (((1U << HBN_REG_AON_GPIO_ISO_MODE_LEN) - 1) << HBN_REG_AON_GPIO_ISO_MODE_POS)
#define HBN_REG_AON_GPIO_ISO_MODE_UMSK      (~(((1U << HBN_REG_AON_GPIO_ISO_MODE_LEN) - 1) << HBN_REG_AON_GPIO_ISO_MODE_POS))

/* 0x3C : HBN_PAD_CTRL_1 */
#define HBN_PAD_CTRL_1_OFFSET               (0x3C)
#define HBN_REG_AON_PAD_OE                  HBN_REG_AON_PAD_OE
#define HBN_REG_AON_PAD_OE_POS              (0U)
#define HBN_REG_AON_PAD_OE_LEN              (6U)
#define HBN_REG_AON_PAD_OE_MSK              (((1U << HBN_REG_AON_PAD_OE_LEN) - 1) << HBN_REG_AON_PAD_OE_POS)
#define HBN_REG_AON_PAD_OE_UMSK             (~(((1U << HBN_REG_AON_PAD_OE_LEN) - 1) << HBN_REG_AON_PAD_OE_POS))
#define HBN_REG_AON_PAD_PD                  HBN_REG_AON_PAD_PD
#define HBN_REG_AON_PAD_PD_POS              (10U)
#define HBN_REG_AON_PAD_PD_LEN              (6U)
#define HBN_REG_AON_PAD_PD_MSK              (((1U << HBN_REG_AON_PAD_PD_LEN) - 1) << HBN_REG_AON_PAD_PD_POS)
#define HBN_REG_AON_PAD_PD_UMSK             (~(((1U << HBN_REG_AON_PAD_PD_LEN) - 1) << HBN_REG_AON_PAD_PD_POS))
#define HBN_REG_AON_PAD_PU                  HBN_REG_AON_PAD_PU
#define HBN_REG_AON_PAD_PU_POS              (20U)
#define HBN_REG_AON_PAD_PU_LEN              (6U)
#define HBN_REG_AON_PAD_PU_MSK              (((1U << HBN_REG_AON_PAD_PU_LEN) - 1) << HBN_REG_AON_PAD_PU_POS)
#define HBN_REG_AON_PAD_PU_UMSK             (~(((1U << HBN_REG_AON_PAD_PU_LEN) - 1) << HBN_REG_AON_PAD_PU_POS))

/* 0x40 : vbat_ldo */
#define HBN_VBAT_LDO_OFFSET                 (0x40)

/* 0x44 : HBN_RSV0 */
#define HBN_RSV0_OFFSET                     (0x44)
#define HBN_RSV0                            HBN_RSV0
#define HBN_RSV0_POS                        (0U)
#define HBN_RSV0_LEN                        (32U)
#define HBN_RSV0_MSK                        (((1U << HBN_RSV0_LEN) - 1) << HBN_RSV0_POS)
#define HBN_RSV0_UMSK                       (~(((1U << HBN_RSV0_LEN) - 1) << HBN_RSV0_POS))

/* 0x48 : HBN_RSV1 */
#define HBN_RSV1_OFFSET                     (0x48)
#define HBN_RSV1                            HBN_RSV1
#define HBN_RSV1_POS                        (0U)
#define HBN_RSV1_LEN                        (32U)
#define HBN_RSV1_MSK                        (((1U << HBN_RSV1_LEN) - 1) << HBN_RSV1_POS)
#define HBN_RSV1_UMSK                       (~(((1U << HBN_RSV1_LEN) - 1) << HBN_RSV1_POS))

/* 0x4C : HBN_RSV2 */
#define HBN_RSV2_OFFSET                     (0x4C)
#define HBN_RSV2                            HBN_RSV2
#define HBN_RSV2_POS                        (0U)
#define HBN_RSV2_LEN                        (32U)
#define HBN_RSV2_MSK                        (((1U << HBN_RSV2_LEN) - 1) << HBN_RSV2_POS)
#define HBN_RSV2_UMSK                       (~(((1U << HBN_RSV2_LEN) - 1) << HBN_RSV2_POS))

/* 0x50 : HBN_RSV3 */
#define HBN_RSV3_OFFSET                     (0x50)
#define HBN_RSV3                            HBN_RSV3
#define HBN_RSV3_POS                        (0U)
#define HBN_RSV3_LEN                        (32U)
#define HBN_RSV3_MSK                        (((1U << HBN_RSV3_LEN) - 1) << HBN_RSV3_POS)
#define HBN_RSV3_UMSK                       (~(((1U << HBN_RSV3_LEN) - 1) << HBN_RSV3_POS))

/* 0x54 : HBN_VERSION */
#define HBN_VERSION_OFFSET                  (0x54)
#define HBN_RESV_FOR_SOFTWARE               HBN_RESV_FOR_SOFTWARE
#define HBN_RESV_FOR_SOFTWARE_POS           (0U)
#define HBN_RESV_FOR_SOFTWARE_LEN           (24U)
#define HBN_RESV_FOR_SOFTWARE_MSK           (((1U << HBN_RESV_FOR_SOFTWARE_LEN) - 1) << HBN_RESV_FOR_SOFTWARE_POS)
#define HBN_RESV_FOR_SOFTWARE_UMSK          (~(((1U << HBN_RESV_FOR_SOFTWARE_LEN) - 1) << HBN_RESV_FOR_SOFTWARE_POS))
#define HBN_VERSION_ID                      HBN_VERSION_ID
#define HBN_VERSION_ID_POS                  (24U)
#define HBN_VERSION_ID_LEN                  (8U)
#define HBN_VERSION_ID_MSK                  (((1U << HBN_VERSION_ID_LEN) - 1) << HBN_VERSION_ID_POS)
#define HBN_VERSION_ID_UMSK                 (~(((1U << HBN_VERSION_ID_LEN) - 1) << HBN_VERSION_ID_POS))

/* 0x58 : Digital Control */
#define HBN_LDO_0_OFFSET                    (0x58)
#define HBN_LDO_SOC_PULLDOWN_AON            HBN_LDO_SOC_PULLDOWN_AON
#define HBN_LDO_SOC_PULLDOWN_AON_POS        (0U)
#define HBN_LDO_SOC_PULLDOWN_AON_LEN        (1U)
#define HBN_LDO_SOC_PULLDOWN_AON_MSK        (((1U << HBN_LDO_SOC_PULLDOWN_AON_LEN) - 1) << HBN_LDO_SOC_PULLDOWN_AON_POS)
#define HBN_LDO_SOC_PULLDOWN_AON_UMSK       (~(((1U << HBN_LDO_SOC_PULLDOWN_AON_LEN) - 1) << HBN_LDO_SOC_PULLDOWN_AON_POS))
#define HBN_LDO_SOC_PULLDOWN_SEL_AON        HBN_LDO_SOC_PULLDOWN_SEL_AON
#define HBN_LDO_SOC_PULLDOWN_SEL_AON_POS    (1U)
#define HBN_LDO_SOC_PULLDOWN_SEL_AON_LEN    (1U)
#define HBN_LDO_SOC_PULLDOWN_SEL_AON_MSK    (((1U << HBN_LDO_SOC_PULLDOWN_SEL_AON_LEN) - 1) << HBN_LDO_SOC_PULLDOWN_SEL_AON_POS)
#define HBN_LDO_SOC_PULLDOWN_SEL_AON_UMSK   (~(((1U << HBN_LDO_SOC_PULLDOWN_SEL_AON_LEN) - 1) << HBN_LDO_SOC_PULLDOWN_SEL_AON_POS))
#define HBN_LDO_SOC_LP_EN_AON               HBN_LDO_SOC_LP_EN_AON
#define HBN_LDO_SOC_LP_EN_AON_POS           (2U)
#define HBN_LDO_SOC_LP_EN_AON_LEN           (1U)
#define HBN_LDO_SOC_LP_EN_AON_MSK           (((1U << HBN_LDO_SOC_LP_EN_AON_LEN) - 1) << HBN_LDO_SOC_LP_EN_AON_POS)
#define HBN_LDO_SOC_LP_EN_AON_UMSK          (~(((1U << HBN_LDO_SOC_LP_EN_AON_LEN) - 1) << HBN_LDO_SOC_LP_EN_AON_POS))
#define HBN_LDO_SOC_LP_EN_AON_CTRL_HW       HBN_LDO_SOC_LP_EN_AON_CTRL_HW
#define HBN_LDO_SOC_LP_EN_AON_CTRL_HW_POS   (3U)
#define HBN_LDO_SOC_LP_EN_AON_CTRL_HW_LEN   (1U)
#define HBN_LDO_SOC_LP_EN_AON_CTRL_HW_MSK   (((1U << HBN_LDO_SOC_LP_EN_AON_CTRL_HW_LEN) - 1) << HBN_LDO_SOC_LP_EN_AON_CTRL_HW_POS)
#define HBN_LDO_SOC_LP_EN_AON_CTRL_HW_UMSK  (~(((1U << HBN_LDO_SOC_LP_EN_AON_CTRL_HW_LEN) - 1) << HBN_LDO_SOC_LP_EN_AON_CTRL_HW_POS))
#define HBN_LDO_SOC_VOUT_SEL_AON            HBN_LDO_SOC_VOUT_SEL_AON
#define HBN_LDO_SOC_VOUT_SEL_AON_POS        (4U)
#define HBN_LDO_SOC_VOUT_SEL_AON_LEN        (4U)
#define HBN_LDO_SOC_VOUT_SEL_AON_MSK        (((1U << HBN_LDO_SOC_VOUT_SEL_AON_LEN) - 1) << HBN_LDO_SOC_VOUT_SEL_AON_POS)
#define HBN_LDO_SOC_VOUT_SEL_AON_UMSK       (~(((1U << HBN_LDO_SOC_VOUT_SEL_AON_LEN) - 1) << HBN_LDO_SOC_VOUT_SEL_AON_POS))
#define HBN_LDO_SOC_VOUT_TRIM_AON           HBN_LDO_SOC_VOUT_TRIM_AON
#define HBN_LDO_SOC_VOUT_TRIM_AON_POS       (8U)
#define HBN_LDO_SOC_VOUT_TRIM_AON_LEN       (4U)
#define HBN_LDO_SOC_VOUT_TRIM_AON_MSK       (((1U << HBN_LDO_SOC_VOUT_TRIM_AON_LEN) - 1) << HBN_LDO_SOC_VOUT_TRIM_AON_POS)
#define HBN_LDO_SOC_VOUT_TRIM_AON_UMSK      (~(((1U << HBN_LDO_SOC_VOUT_TRIM_AON_LEN) - 1) << HBN_LDO_SOC_VOUT_TRIM_AON_POS))
#define HBN_PU_LDO_SOC_AON                  HBN_PU_LDO_SOC_AON
#define HBN_PU_LDO_SOC_AON_POS              (12U)
#define HBN_PU_LDO_SOC_AON_LEN              (1U)
#define HBN_PU_LDO_SOC_AON_MSK              (((1U << HBN_PU_LDO_SOC_AON_LEN) - 1) << HBN_PU_LDO_SOC_AON_POS)
#define HBN_PU_LDO_SOC_AON_UMSK             (~(((1U << HBN_PU_LDO_SOC_AON_LEN) - 1) << HBN_PU_LDO_SOC_AON_POS))
#define HBN_PDS_LDO_SOC_VOUT_SEL_AON        HBN_PDS_LDO_SOC_VOUT_SEL_AON
#define HBN_PDS_LDO_SOC_VOUT_SEL_AON_POS    (13U)
#define HBN_PDS_LDO_SOC_VOUT_SEL_AON_LEN    (4U)
#define HBN_PDS_LDO_SOC_VOUT_SEL_AON_MSK    (((1U << HBN_PDS_LDO_SOC_VOUT_SEL_AON_LEN) - 1) << HBN_PDS_LDO_SOC_VOUT_SEL_AON_POS)
#define HBN_PDS_LDO_SOC_VOUT_SEL_AON_UMSK   (~(((1U << HBN_PDS_LDO_SOC_VOUT_SEL_AON_LEN) - 1) << HBN_PDS_LDO_SOC_VOUT_SEL_AON_POS))
#define HBN_PDS_LDO_SOC_VOUT_TRIM_AON       HBN_PDS_LDO_SOC_VOUT_TRIM_AON
#define HBN_PDS_LDO_SOC_VOUT_TRIM_AON_POS   (17U)
#define HBN_PDS_LDO_SOC_VOUT_TRIM_AON_LEN   (4U)
#define HBN_PDS_LDO_SOC_VOUT_TRIM_AON_MSK   (((1U << HBN_PDS_LDO_SOC_VOUT_TRIM_AON_LEN) - 1) << HBN_PDS_LDO_SOC_VOUT_TRIM_AON_POS)
#define HBN_PDS_LDO_SOC_VOUT_TRIM_AON_UMSK  (~(((1U << HBN_PDS_LDO_SOC_VOUT_TRIM_AON_LEN) - 1) << HBN_PDS_LDO_SOC_VOUT_TRIM_AON_POS))
#define HBN_LDO_SOC_LP_EN_AON_HW            HBN_LDO_SOC_LP_EN_AON_HW
#define HBN_LDO_SOC_LP_EN_AON_HW_POS        (21U)
#define HBN_LDO_SOC_LP_EN_AON_HW_LEN        (1U)
#define HBN_LDO_SOC_LP_EN_AON_HW_MSK        (((1U << HBN_LDO_SOC_LP_EN_AON_HW_LEN) - 1) << HBN_LDO_SOC_LP_EN_AON_HW_POS)
#define HBN_LDO_SOC_LP_EN_AON_HW_UMSK       (~(((1U << HBN_LDO_SOC_LP_EN_AON_HW_LEN) - 1) << HBN_LDO_SOC_LP_EN_AON_HW_POS))
#define HBN_PU_LDO_SOC_AON_HW               HBN_PU_LDO_SOC_AON_HW
#define HBN_PU_LDO_SOC_AON_HW_POS           (22U)
#define HBN_PU_LDO_SOC_AON_HW_LEN           (1U)
#define HBN_PU_LDO_SOC_AON_HW_MSK           (((1U << HBN_PU_LDO_SOC_AON_HW_LEN) - 1) << HBN_PU_LDO_SOC_AON_HW_POS)
#define HBN_PU_LDO_SOC_AON_HW_UMSK          (~(((1U << HBN_PU_LDO_SOC_AON_HW_LEN) - 1) << HBN_PU_LDO_SOC_AON_HW_POS))
#define HBN_PD_LDO_SOC_AON                  HBN_PD_LDO_SOC_AON
#define HBN_PD_LDO_SOC_AON_POS              (23U)
#define HBN_PD_LDO_SOC_AON_LEN              (1U)
#define HBN_PD_LDO_SOC_AON_MSK              (((1U << HBN_PD_LDO_SOC_AON_LEN) - 1) << HBN_PD_LDO_SOC_AON_POS)
#define HBN_PD_LDO_SOC_AON_UMSK             (~(((1U << HBN_PD_LDO_SOC_AON_LEN) - 1) << HBN_PD_LDO_SOC_AON_POS))

/* 0x5C : Digital Control */
#define HBN_LDO_1_OFFSET                    (0x5C)
#define HBN_LDO13SYS_PULLDOWN_AON           HBN_LDO13SYS_PULLDOWN_AON
#define HBN_LDO13SYS_PULLDOWN_AON_POS       (0U)
#define HBN_LDO13SYS_PULLDOWN_AON_LEN       (1U)
#define HBN_LDO13SYS_PULLDOWN_AON_MSK       (((1U << HBN_LDO13SYS_PULLDOWN_AON_LEN) - 1) << HBN_LDO13SYS_PULLDOWN_AON_POS)
#define HBN_LDO13SYS_PULLDOWN_AON_UMSK      (~(((1U << HBN_LDO13SYS_PULLDOWN_AON_LEN) - 1) << HBN_LDO13SYS_PULLDOWN_AON_POS))
#define HBN_LDO13SYS_PULLDOWN_SEL_AON       HBN_LDO13SYS_PULLDOWN_SEL_AON
#define HBN_LDO13SYS_PULLDOWN_SEL_AON_POS   (1U)
#define HBN_LDO13SYS_PULLDOWN_SEL_AON_LEN   (1U)
#define HBN_LDO13SYS_PULLDOWN_SEL_AON_MSK   (((1U << HBN_LDO13SYS_PULLDOWN_SEL_AON_LEN) - 1) << HBN_LDO13SYS_PULLDOWN_SEL_AON_POS)
#define HBN_LDO13SYS_PULLDOWN_SEL_AON_UMSK  (~(((1U << HBN_LDO13SYS_PULLDOWN_SEL_AON_LEN) - 1) << HBN_LDO13SYS_PULLDOWN_SEL_AON_POS))
#define HBN_LDO13SYS_LP_EN_AON              HBN_LDO13SYS_LP_EN_AON
#define HBN_LDO13SYS_LP_EN_AON_POS          (2U)
#define HBN_LDO13SYS_LP_EN_AON_LEN          (1U)
#define HBN_LDO13SYS_LP_EN_AON_MSK          (((1U << HBN_LDO13SYS_LP_EN_AON_LEN) - 1) << HBN_LDO13SYS_LP_EN_AON_POS)
#define HBN_LDO13SYS_LP_EN_AON_UMSK         (~(((1U << HBN_LDO13SYS_LP_EN_AON_LEN) - 1) << HBN_LDO13SYS_LP_EN_AON_POS))
#define HBN_LDO13SYS_LP_EN_AON_CTRL_HW      HBN_LDO13SYS_LP_EN_AON_CTRL_HW
#define HBN_LDO13SYS_LP_EN_AON_CTRL_HW_POS  (3U)
#define HBN_LDO13SYS_LP_EN_AON_CTRL_HW_LEN  (1U)
#define HBN_LDO13SYS_LP_EN_AON_CTRL_HW_MSK  (((1U << HBN_LDO13SYS_LP_EN_AON_CTRL_HW_LEN) - 1) << HBN_LDO13SYS_LP_EN_AON_CTRL_HW_POS)
#define HBN_LDO13SYS_LP_EN_AON_CTRL_HW_UMSK (~(((1U << HBN_LDO13SYS_LP_EN_AON_CTRL_HW_LEN) - 1) << HBN_LDO13SYS_LP_EN_AON_CTRL_HW_POS))
#define HBN_LDO13SYS_VOUT_SEL_AON           HBN_LDO13SYS_VOUT_SEL_AON
#define HBN_LDO13SYS_VOUT_SEL_AON_POS       (4U)
#define HBN_LDO13SYS_VOUT_SEL_AON_LEN       (4U)
#define HBN_LDO13SYS_VOUT_SEL_AON_MSK       (((1U << HBN_LDO13SYS_VOUT_SEL_AON_LEN) - 1) << HBN_LDO13SYS_VOUT_SEL_AON_POS)
#define HBN_LDO13SYS_VOUT_SEL_AON_UMSK      (~(((1U << HBN_LDO13SYS_VOUT_SEL_AON_LEN) - 1) << HBN_LDO13SYS_VOUT_SEL_AON_POS))
#define HBN_PU_LDO13SYS_AON                 HBN_PU_LDO13SYS_AON
#define HBN_PU_LDO13SYS_AON_POS             (8U)
#define HBN_PU_LDO13SYS_AON_LEN             (1U)
#define HBN_PU_LDO13SYS_AON_MSK             (((1U << HBN_PU_LDO13SYS_AON_LEN) - 1) << HBN_PU_LDO13SYS_AON_POS)
#define HBN_PU_LDO13SYS_AON_UMSK            (~(((1U << HBN_PU_LDO13SYS_AON_LEN) - 1) << HBN_PU_LDO13SYS_AON_POS))
#define HBN_PD_LDO13SYS_AON                 HBN_PD_LDO13SYS_AON
#define HBN_PD_LDO13SYS_AON_POS             (9U)
#define HBN_PD_LDO13SYS_AON_LEN             (1U)
#define HBN_PD_LDO13SYS_AON_MSK             (((1U << HBN_PD_LDO13SYS_AON_LEN) - 1) << HBN_PD_LDO13SYS_AON_POS)
#define HBN_PD_LDO13SYS_AON_UMSK            (~(((1U << HBN_PD_LDO13SYS_AON_LEN) - 1) << HBN_PD_LDO13SYS_AON_POS))
#define HBN_CR_LDO_SOC_NO_WAIT              HBN_CR_LDO_SOC_NO_WAIT
#define HBN_CR_LDO_SOC_NO_WAIT_POS          (10U)
#define HBN_CR_LDO_SOC_NO_WAIT_LEN          (1U)
#define HBN_CR_LDO_SOC_NO_WAIT_MSK          (((1U << HBN_CR_LDO_SOC_NO_WAIT_LEN) - 1) << HBN_CR_LDO_SOC_NO_WAIT_POS)
#define HBN_CR_LDO_SOC_NO_WAIT_UMSK         (~(((1U << HBN_CR_LDO_SOC_NO_WAIT_LEN) - 1) << HBN_CR_LDO_SOC_NO_WAIT_POS))
#define HBN_PDS_LDO13SYS_VOUT_SEL_AON       HBN_PDS_LDO13SYS_VOUT_SEL_AON
#define HBN_PDS_LDO13SYS_VOUT_SEL_AON_POS   (12U)
#define HBN_PDS_LDO13SYS_VOUT_SEL_AON_LEN   (4U)
#define HBN_PDS_LDO13SYS_VOUT_SEL_AON_MSK   (((1U << HBN_PDS_LDO13SYS_VOUT_SEL_AON_LEN) - 1) << HBN_PDS_LDO13SYS_VOUT_SEL_AON_POS)
#define HBN_PDS_LDO13SYS_VOUT_SEL_AON_UMSK  (~(((1U << HBN_PDS_LDO13SYS_VOUT_SEL_AON_LEN) - 1) << HBN_PDS_LDO13SYS_VOUT_SEL_AON_POS))
#define HBN_LDO13SYS_LP_EN_AON_HW           HBN_LDO13SYS_LP_EN_AON_HW
#define HBN_LDO13SYS_LP_EN_AON_HW_POS       (16U)
#define HBN_LDO13SYS_LP_EN_AON_HW_LEN       (1U)
#define HBN_LDO13SYS_LP_EN_AON_HW_MSK       (((1U << HBN_LDO13SYS_LP_EN_AON_HW_LEN) - 1) << HBN_LDO13SYS_LP_EN_AON_HW_POS)
#define HBN_LDO13SYS_LP_EN_AON_HW_UMSK      (~(((1U << HBN_LDO13SYS_LP_EN_AON_HW_LEN) - 1) << HBN_LDO13SYS_LP_EN_AON_HW_POS))
#define HBN_PU_LDO13SYS_AON_HW              HBN_PU_LDO13SYS_AON_HW
#define HBN_PU_LDO13SYS_AON_HW_POS          (17U)
#define HBN_PU_LDO13SYS_AON_HW_LEN          (1U)
#define HBN_PU_LDO13SYS_AON_HW_MSK          (((1U << HBN_PU_LDO13SYS_AON_HW_LEN) - 1) << HBN_PU_LDO13SYS_AON_HW_POS)
#define HBN_PU_LDO13SYS_AON_HW_UMSK         (~(((1U << HBN_PU_LDO13SYS_AON_HW_LEN) - 1) << HBN_PU_LDO13SYS_AON_HW_POS))
#define HBN_CR_LDO13SYS_PON_CNT             HBN_CR_LDO13SYS_PON_CNT
#define HBN_CR_LDO13SYS_PON_CNT_POS         (18U)
#define HBN_CR_LDO13SYS_PON_CNT_LEN         (10U)
#define HBN_CR_LDO13SYS_PON_CNT_MSK         (((1U << HBN_CR_LDO13SYS_PON_CNT_LEN) - 1) << HBN_CR_LDO13SYS_PON_CNT_POS)
#define HBN_CR_LDO13SYS_PON_CNT_UMSK        (~(((1U << HBN_CR_LDO13SYS_PON_CNT_LEN) - 1) << HBN_CR_LDO13SYS_PON_CNT_POS))
#define HBN_LDO13SYS_VOUT_TRIM_AON          HBN_LDO13SYS_VOUT_TRIM_AON
#define HBN_LDO13SYS_VOUT_TRIM_AON_POS      (28U)
#define HBN_LDO13SYS_VOUT_TRIM_AON_LEN      (4U)
#define HBN_LDO13SYS_VOUT_TRIM_AON_MSK      (((1U << HBN_LDO13SYS_VOUT_TRIM_AON_LEN) - 1) << HBN_LDO13SYS_VOUT_TRIM_AON_POS)
#define HBN_LDO13SYS_VOUT_TRIM_AON_UMSK     (~(((1U << HBN_LDO13SYS_VOUT_TRIM_AON_LEN) - 1) << HBN_LDO13SYS_VOUT_TRIM_AON_POS))

/* 0x60 : Digital Control */
#define HBN_LDO_2_OFFSET                    (0x60)
#define HBN_LDO18IO_PULLDOWN_AON            HBN_LDO18IO_PULLDOWN_AON
#define HBN_LDO18IO_PULLDOWN_AON_POS        (0U)
#define HBN_LDO18IO_PULLDOWN_AON_LEN        (1U)
#define HBN_LDO18IO_PULLDOWN_AON_MSK        (((1U << HBN_LDO18IO_PULLDOWN_AON_LEN) - 1) << HBN_LDO18IO_PULLDOWN_AON_POS)
#define HBN_LDO18IO_PULLDOWN_AON_UMSK       (~(((1U << HBN_LDO18IO_PULLDOWN_AON_LEN) - 1) << HBN_LDO18IO_PULLDOWN_AON_POS))
#define HBN_LDO18IO_PULLDOWN_SEL_AON        HBN_LDO18IO_PULLDOWN_SEL_AON
#define HBN_LDO18IO_PULLDOWN_SEL_AON_POS    (1U)
#define HBN_LDO18IO_PULLDOWN_SEL_AON_LEN    (1U)
#define HBN_LDO18IO_PULLDOWN_SEL_AON_MSK    (((1U << HBN_LDO18IO_PULLDOWN_SEL_AON_LEN) - 1) << HBN_LDO18IO_PULLDOWN_SEL_AON_POS)
#define HBN_LDO18IO_PULLDOWN_SEL_AON_UMSK   (~(((1U << HBN_LDO18IO_PULLDOWN_SEL_AON_LEN) - 1) << HBN_LDO18IO_PULLDOWN_SEL_AON_POS))
#define HBN_PU_LDO18IO_AON                  HBN_PU_LDO18IO_AON
#define HBN_PU_LDO18IO_AON_POS              (2U)
#define HBN_PU_LDO18IO_AON_LEN              (1U)
#define HBN_PU_LDO18IO_AON_MSK              (((1U << HBN_PU_LDO18IO_AON_LEN) - 1) << HBN_PU_LDO18IO_AON_POS)
#define HBN_PU_LDO18IO_AON_UMSK             (~(((1U << HBN_PU_LDO18IO_AON_LEN) - 1) << HBN_PU_LDO18IO_AON_POS))
#define HBN_EXT_DCDC2_PD_CNT                HBN_EXT_DCDC2_PD_CNT
#define HBN_EXT_DCDC2_PD_CNT_POS            (4U)
#define HBN_EXT_DCDC2_PD_CNT_LEN            (4U)
#define HBN_EXT_DCDC2_PD_CNT_MSK            (((1U << HBN_EXT_DCDC2_PD_CNT_LEN) - 1) << HBN_EXT_DCDC2_PD_CNT_POS)
#define HBN_EXT_DCDC2_PD_CNT_UMSK           (~(((1U << HBN_EXT_DCDC2_PD_CNT_LEN) - 1) << HBN_EXT_DCDC2_PD_CNT_POS))
#define HBN_EXT_DCDC1_PD_CNT                HBN_EXT_DCDC1_PD_CNT
#define HBN_EXT_DCDC1_PD_CNT_POS            (8U)
#define HBN_EXT_DCDC1_PD_CNT_LEN            (4U)
#define HBN_EXT_DCDC1_PD_CNT_MSK            (((1U << HBN_EXT_DCDC1_PD_CNT_LEN) - 1) << HBN_EXT_DCDC1_PD_CNT_POS)
#define HBN_EXT_DCDC1_PD_CNT_UMSK           (~(((1U << HBN_EXT_DCDC1_PD_CNT_LEN) - 1) << HBN_EXT_DCDC1_PD_CNT_POS))
#define HBN_LDO13SYS_PD_CNT                 HBN_LDO13SYS_PD_CNT
#define HBN_LDO13SYS_PD_CNT_POS             (12U)
#define HBN_LDO13SYS_PD_CNT_LEN             (4U)
#define HBN_LDO13SYS_PD_CNT_MSK             (((1U << HBN_LDO13SYS_PD_CNT_LEN) - 1) << HBN_LDO13SYS_PD_CNT_POS)
#define HBN_LDO13SYS_PD_CNT_UMSK            (~(((1U << HBN_LDO13SYS_PD_CNT_LEN) - 1) << HBN_LDO13SYS_PD_CNT_POS))
#define HBN_LDO_SOC_PD_CNT                  HBN_LDO_SOC_PD_CNT
#define HBN_LDO_SOC_PD_CNT_POS              (16U)
#define HBN_LDO_SOC_PD_CNT_LEN              (4U)
#define HBN_LDO_SOC_PD_CNT_MSK              (((1U << HBN_LDO_SOC_PD_CNT_LEN) - 1) << HBN_LDO_SOC_PD_CNT_POS)
#define HBN_LDO_SOC_PD_CNT_UMSK             (~(((1U << HBN_LDO_SOC_PD_CNT_LEN) - 1) << HBN_LDO_SOC_PD_CNT_POS))
#define HBN_CR_LDO_SOC_PON_CNT              HBN_CR_LDO_SOC_PON_CNT
#define HBN_CR_LDO_SOC_PON_CNT_POS          (20U)
#define HBN_CR_LDO_SOC_PON_CNT_LEN          (10U)
#define HBN_CR_LDO_SOC_PON_CNT_MSK          (((1U << HBN_CR_LDO_SOC_PON_CNT_LEN) - 1) << HBN_CR_LDO_SOC_PON_CNT_POS)
#define HBN_CR_LDO_SOC_PON_CNT_UMSK         (~(((1U << HBN_CR_LDO_SOC_PON_CNT_LEN) - 1) << HBN_CR_LDO_SOC_PON_CNT_POS))

/* 0x64 : Digital Control */
#define HBN_RC32K_0_OFFSET                  (0x64)
#define HBN_RC32K_CAP_SEL_AON               HBN_RC32K_CAP_SEL_AON
#define HBN_RC32K_CAP_SEL_AON_POS           (0U)
#define HBN_RC32K_CAP_SEL_AON_LEN           (4U)
#define HBN_RC32K_CAP_SEL_AON_MSK           (((1U << HBN_RC32K_CAP_SEL_AON_LEN) - 1) << HBN_RC32K_CAP_SEL_AON_POS)
#define HBN_RC32K_CAP_SEL_AON_UMSK          (~(((1U << HBN_RC32K_CAP_SEL_AON_LEN) - 1) << HBN_RC32K_CAP_SEL_AON_POS))
#define HBN_RC32K_CODE_FR_CAL_AON           HBN_RC32K_CODE_FR_CAL_AON
#define HBN_RC32K_CODE_FR_CAL_AON_POS       (4U)
#define HBN_RC32K_CODE_FR_CAL_AON_LEN       (10U)
#define HBN_RC32K_CODE_FR_CAL_AON_MSK       (((1U << HBN_RC32K_CODE_FR_CAL_AON_LEN) - 1) << HBN_RC32K_CODE_FR_CAL_AON_POS)
#define HBN_RC32K_CODE_FR_CAL_AON_UMSK      (~(((1U << HBN_RC32K_CODE_FR_CAL_AON_LEN) - 1) << HBN_RC32K_CODE_FR_CAL_AON_POS))
#define HBN_RC32K_TCOMP_CODE_AON            HBN_RC32K_TCOMP_CODE_AON
#define HBN_RC32K_TCOMP_CODE_AON_POS        (16U)
#define HBN_RC32K_TCOMP_CODE_AON_LEN        (10U)
#define HBN_RC32K_TCOMP_CODE_AON_MSK        (((1U << HBN_RC32K_TCOMP_CODE_AON_LEN) - 1) << HBN_RC32K_TCOMP_CODE_AON_POS)
#define HBN_RC32K_TCOMP_CODE_AON_UMSK       (~(((1U << HBN_RC32K_TCOMP_CODE_AON_LEN) - 1) << HBN_RC32K_TCOMP_CODE_AON_POS))
#define HBN_RC32K_TCOMP_EN_AON              HBN_RC32K_TCOMP_EN_AON
#define HBN_RC32K_TCOMP_EN_AON_POS          (26U)
#define HBN_RC32K_TCOMP_EN_AON_LEN          (1U)
#define HBN_RC32K_TCOMP_EN_AON_MSK          (((1U << HBN_RC32K_TCOMP_EN_AON_LEN) - 1) << HBN_RC32K_TCOMP_EN_AON_POS)
#define HBN_RC32K_TCOMP_EN_AON_UMSK         (~(((1U << HBN_RC32K_TCOMP_EN_AON_LEN) - 1) << HBN_RC32K_TCOMP_EN_AON_POS))
#define HBN_RC32K_VREF_DLY_AON              HBN_RC32K_VREF_DLY_AON
#define HBN_RC32K_VREF_DLY_AON_POS          (28U)
#define HBN_RC32K_VREF_DLY_AON_LEN          (2U)
#define HBN_RC32K_VREF_DLY_AON_MSK          (((1U << HBN_RC32K_VREF_DLY_AON_LEN) - 1) << HBN_RC32K_VREF_DLY_AON_POS)
#define HBN_RC32K_VREF_DLY_AON_UMSK         (~(((1U << HBN_RC32K_VREF_DLY_AON_LEN) - 1) << HBN_RC32K_VREF_DLY_AON_POS))

/* 0x68 : Digital Control */
#define HBN_RC32K_1_OFFSET                  (0x68)
#define HBN_RC32K_RDY                       HBN_RC32K_RDY
#define HBN_RC32K_RDY_POS                   (0U)
#define HBN_RC32K_RDY_LEN                   (1U)
#define HBN_RC32K_RDY_MSK                   (((1U << HBN_RC32K_RDY_LEN) - 1) << HBN_RC32K_RDY_POS)
#define HBN_RC32K_RDY_UMSK                  (~(((1U << HBN_RC32K_RDY_LEN) - 1) << HBN_RC32K_RDY_POS))
#define HBN_RC32K_CODE_VALID                HBN_RC32K_CODE_VALID
#define HBN_RC32K_CODE_VALID_POS            (1U)
#define HBN_RC32K_CODE_VALID_LEN            (1U)
#define HBN_RC32K_CODE_VALID_MSK            (((1U << HBN_RC32K_CODE_VALID_LEN) - 1) << HBN_RC32K_CODE_VALID_POS)
#define HBN_RC32K_CODE_VALID_UMSK           (~(((1U << HBN_RC32K_CODE_VALID_LEN) - 1) << HBN_RC32K_CODE_VALID_POS))
#define HBN_RC32K_IPTAT_BM_AON              HBN_RC32K_IPTAT_BM_AON
#define HBN_RC32K_IPTAT_BM_AON_POS          (4U)
#define HBN_RC32K_IPTAT_BM_AON_LEN          (3U)
#define HBN_RC32K_IPTAT_BM_AON_MSK          (((1U << HBN_RC32K_IPTAT_BM_AON_LEN) - 1) << HBN_RC32K_IPTAT_BM_AON_POS)
#define HBN_RC32K_IPTAT_BM_AON_UMSK         (~(((1U << HBN_RC32K_IPTAT_BM_AON_LEN) - 1) << HBN_RC32K_IPTAT_BM_AON_POS))
#define HBN_PU_RC32K_AON                    HBN_PU_RC32K_AON
#define HBN_PU_RC32K_AON_POS                (7U)
#define HBN_PU_RC32K_AON_LEN                (1U)
#define HBN_PU_RC32K_AON_MSK                (((1U << HBN_PU_RC32K_AON_LEN) - 1) << HBN_PU_RC32K_AON_POS)
#define HBN_PU_RC32K_AON_UMSK               (~(((1U << HBN_PU_RC32K_AON_LEN) - 1) << HBN_PU_RC32K_AON_POS))
#define HBN_PU_RC32K_IPTAT_GEN_AON          HBN_PU_RC32K_IPTAT_GEN_AON
#define HBN_PU_RC32K_IPTAT_GEN_AON_POS      (8U)
#define HBN_PU_RC32K_IPTAT_GEN_AON_LEN      (1U)
#define HBN_PU_RC32K_IPTAT_GEN_AON_MSK      (((1U << HBN_PU_RC32K_IPTAT_GEN_AON_LEN) - 1) << HBN_PU_RC32K_IPTAT_GEN_AON_POS)
#define HBN_PU_RC32K_IPTAT_GEN_AON_UMSK     (~(((1U << HBN_PU_RC32K_IPTAT_GEN_AON_LEN) - 1) << HBN_PU_RC32K_IPTAT_GEN_AON_POS))
#define HBN_RC32K_EXT_CODE_EN               HBN_RC32K_EXT_CODE_EN
#define HBN_RC32K_EXT_CODE_EN_POS           (9U)
#define HBN_RC32K_EXT_CODE_EN_LEN           (1U)
#define HBN_RC32K_EXT_CODE_EN_MSK           (((1U << HBN_RC32K_EXT_CODE_EN_LEN) - 1) << HBN_RC32K_EXT_CODE_EN_POS)
#define HBN_RC32K_EXT_CODE_EN_UMSK          (~(((1U << HBN_RC32K_EXT_CODE_EN_LEN) - 1) << HBN_RC32K_EXT_CODE_EN_POS))
#define HBN_RC32K_CODE_FR_CAL_AON_HW        HBN_RC32K_CODE_FR_CAL_AON_HW
#define HBN_RC32K_CODE_FR_CAL_AON_HW_POS    (10U)
#define HBN_RC32K_CODE_FR_CAL_AON_HW_LEN    (10U)
#define HBN_RC32K_CODE_FR_CAL_AON_HW_MSK    (((1U << HBN_RC32K_CODE_FR_CAL_AON_HW_LEN) - 1) << HBN_RC32K_CODE_FR_CAL_AON_HW_POS)
#define HBN_RC32K_CODE_FR_CAL_AON_HW_UMSK   (~(((1U << HBN_RC32K_CODE_FR_CAL_AON_HW_LEN) - 1) << HBN_RC32K_CODE_FR_CAL_AON_HW_POS))
#define HBN_RC32K_CODE_CAP_SEL_AON_HW       HBN_RC32K_CODE_CAP_SEL_AON_HW
#define HBN_RC32K_CODE_CAP_SEL_AON_HW_POS   (20U)
#define HBN_RC32K_CODE_CAP_SEL_AON_HW_LEN   (4U)
#define HBN_RC32K_CODE_CAP_SEL_AON_HW_MSK   (((1U << HBN_RC32K_CODE_CAP_SEL_AON_HW_LEN) - 1) << HBN_RC32K_CODE_CAP_SEL_AON_HW_POS)
#define HBN_RC32K_CODE_CAP_SEL_AON_HW_UMSK  (~(((1U << HBN_RC32K_CODE_CAP_SEL_AON_HW_LEN) - 1) << HBN_RC32K_CODE_CAP_SEL_AON_HW_POS))
#define HBN_RC32K_FIN_CAL_START             HBN_RC32K_FIN_CAL_START
#define HBN_RC32K_FIN_CAL_START_POS         (24U)
#define HBN_RC32K_FIN_CAL_START_LEN         (1U)
#define HBN_RC32K_FIN_CAL_START_MSK         (((1U << HBN_RC32K_FIN_CAL_START_LEN) - 1) << HBN_RC32K_FIN_CAL_START_POS)
#define HBN_RC32K_FIN_CAL_START_UMSK        (~(((1U << HBN_RC32K_FIN_CAL_START_LEN) - 1) << HBN_RC32K_FIN_CAL_START_POS))
#define HBN_RC32K_COA_CAL_START             HBN_RC32K_COA_CAL_START
#define HBN_RC32K_COA_CAL_START_POS         (25U)
#define HBN_RC32K_COA_CAL_START_LEN         (1U)
#define HBN_RC32K_COA_CAL_START_MSK         (((1U << HBN_RC32K_COA_CAL_START_LEN) - 1) << HBN_RC32K_COA_CAL_START_POS)
#define HBN_RC32K_COA_CAL_START_UMSK        (~(((1U << HBN_RC32K_COA_CAL_START_LEN) - 1) << HBN_RC32K_COA_CAL_START_POS))
#define HBN_RC32K_CAL_EN                    HBN_RC32K_CAL_EN
#define HBN_RC32K_CAL_EN_POS                (26U)
#define HBN_RC32K_CAL_EN_LEN                (1U)
#define HBN_RC32K_CAL_EN_MSK                (((1U << HBN_RC32K_CAL_EN_LEN) - 1) << HBN_RC32K_CAL_EN_POS)
#define HBN_RC32K_CAL_EN_UMSK               (~(((1U << HBN_RC32K_CAL_EN_LEN) - 1) << HBN_RC32K_CAL_EN_POS))
#define HBN_RC32K_CAL_DONE                  HBN_RC32K_CAL_DONE
#define HBN_RC32K_CAL_DONE_POS              (27U)
#define HBN_RC32K_CAL_DONE_LEN              (1U)
#define HBN_RC32K_CAL_DONE_MSK              (((1U << HBN_RC32K_CAL_DONE_LEN) - 1) << HBN_RC32K_CAL_DONE_POS)
#define HBN_RC32K_CAL_DONE_UMSK             (~(((1U << HBN_RC32K_CAL_DONE_LEN) - 1) << HBN_RC32K_CAL_DONE_POS))
#define HBN_RC32K_CAL_SUCCESS               HBN_RC32K_CAL_SUCCESS
#define HBN_RC32K_CAL_SUCCESS_POS           (28U)
#define HBN_RC32K_CAL_SUCCESS_LEN           (1U)
#define HBN_RC32K_CAL_SUCCESS_MSK           (((1U << HBN_RC32K_CAL_SUCCESS_LEN) - 1) << HBN_RC32K_CAL_SUCCESS_POS)
#define HBN_RC32K_CAL_SUCCESS_UMSK          (~(((1U << HBN_RC32K_CAL_SUCCESS_LEN) - 1) << HBN_RC32K_CAL_SUCCESS_POS))
#define HBN_CAL_AVG_XTAL_CNT_START          HBN_CAL_AVG_XTAL_CNT_START
#define HBN_CAL_AVG_XTAL_CNT_START_POS      (29U)
#define HBN_CAL_AVG_XTAL_CNT_START_LEN      (1U)
#define HBN_CAL_AVG_XTAL_CNT_START_MSK      (((1U << HBN_CAL_AVG_XTAL_CNT_START_LEN) - 1) << HBN_CAL_AVG_XTAL_CNT_START_POS)
#define HBN_CAL_AVG_XTAL_CNT_START_UMSK     (~(((1U << HBN_CAL_AVG_XTAL_CNT_START_LEN) - 1) << HBN_CAL_AVG_XTAL_CNT_START_POS))
#define HBN_CAL_AVG_XTAL_CNT_DONE           HBN_CAL_AVG_XTAL_CNT_DONE
#define HBN_CAL_AVG_XTAL_CNT_DONE_POS       (30U)
#define HBN_CAL_AVG_XTAL_CNT_DONE_LEN       (1U)
#define HBN_CAL_AVG_XTAL_CNT_DONE_MSK       (((1U << HBN_CAL_AVG_XTAL_CNT_DONE_LEN) - 1) << HBN_CAL_AVG_XTAL_CNT_DONE_POS)
#define HBN_CAL_AVG_XTAL_CNT_DONE_UMSK      (~(((1U << HBN_CAL_AVG_XTAL_CNT_DONE_LEN) - 1) << HBN_CAL_AVG_XTAL_CNT_DONE_POS))

/* 0x6C : Digital Control */
#define HBN_RC32K_COA_CAL_OFFSET            (0x6C)
#define HBN_COA_CAL_TARGET_CNT              HBN_COA_CAL_TARGET_CNT
#define HBN_COA_CAL_TARGET_CNT_POS          (0U)
#define HBN_COA_CAL_TARGET_CNT_LEN          (18U)
#define HBN_COA_CAL_TARGET_CNT_MSK          (((1U << HBN_COA_CAL_TARGET_CNT_LEN) - 1) << HBN_COA_CAL_TARGET_CNT_POS)
#define HBN_COA_CAL_TARGET_CNT_UMSK         (~(((1U << HBN_COA_CAL_TARGET_CNT_LEN) - 1) << HBN_COA_CAL_TARGET_CNT_POS))
#define HBN_COA_CAL_TARGET_DIFF             HBN_COA_CAL_TARGET_DIFF
#define HBN_COA_CAL_TARGET_DIFF_POS         (18U)
#define HBN_COA_CAL_TARGET_DIFF_LEN         (14U)
#define HBN_COA_CAL_TARGET_DIFF_MSK         (((1U << HBN_COA_CAL_TARGET_DIFF_LEN) - 1) << HBN_COA_CAL_TARGET_DIFF_POS)
#define HBN_COA_CAL_TARGET_DIFF_UMSK        (~(((1U << HBN_COA_CAL_TARGET_DIFF_LEN) - 1) << HBN_COA_CAL_TARGET_DIFF_POS))

/* 0x70 : Digital Control */
#define HBN_RC32K_FIN_CAL_0_OFFSET          (0x70)
#define HBN_FIN_CAL_TARGET_CNT              HBN_FIN_CAL_TARGET_CNT
#define HBN_FIN_CAL_TARGET_CNT_POS          (0U)
#define HBN_FIN_CAL_TARGET_CNT_LEN          (32U)
#define HBN_FIN_CAL_TARGET_CNT_MSK          (((1U << HBN_FIN_CAL_TARGET_CNT_LEN) - 1) << HBN_FIN_CAL_TARGET_CNT_POS)
#define HBN_FIN_CAL_TARGET_CNT_UMSK         (~(((1U << HBN_FIN_CAL_TARGET_CNT_LEN) - 1) << HBN_FIN_CAL_TARGET_CNT_POS))

/* 0x74 : Digital Control */
#define HBN_RC32K_FIN_CAL_1_OFFSET          (0x74)
#define HBN_FIN_CAL_TARGET_DIFF             HBN_FIN_CAL_TARGET_DIFF
#define HBN_FIN_CAL_TARGET_DIFF_POS         (0U)
#define HBN_FIN_CAL_TARGET_DIFF_LEN         (32U)
#define HBN_FIN_CAL_TARGET_DIFF_MSK         (((1U << HBN_FIN_CAL_TARGET_DIFF_LEN) - 1) << HBN_FIN_CAL_TARGET_DIFF_POS)
#define HBN_FIN_CAL_TARGET_DIFF_UMSK        (~(((1U << HBN_FIN_CAL_TARGET_DIFF_LEN) - 1) << HBN_FIN_CAL_TARGET_DIFF_POS))

/* 0x78 : Digital Control */
#define HBN_RC32K_2_OFFSET                  (0x78)
#define HBN_FIN_CAL_CLK_NUM                 HBN_FIN_CAL_CLK_NUM
#define HBN_FIN_CAL_CLK_NUM_POS             (0U)
#define HBN_FIN_CAL_CLK_NUM_LEN             (5U)
#define HBN_FIN_CAL_CLK_NUM_MSK             (((1U << HBN_FIN_CAL_CLK_NUM_LEN) - 1) << HBN_FIN_CAL_CLK_NUM_POS)
#define HBN_FIN_CAL_CLK_NUM_UMSK            (~(((1U << HBN_FIN_CAL_CLK_NUM_LEN) - 1) << HBN_FIN_CAL_CLK_NUM_POS))
#define HBN_COA_CAL_CLK_NUM                 HBN_COA_CAL_CLK_NUM
#define HBN_COA_CAL_CLK_NUM_POS             (5U)
#define HBN_COA_CAL_CLK_NUM_LEN             (3U)
#define HBN_COA_CAL_CLK_NUM_MSK             (((1U << HBN_COA_CAL_CLK_NUM_LEN) - 1) << HBN_COA_CAL_CLK_NUM_POS)
#define HBN_COA_CAL_CLK_NUM_UMSK            (~(((1U << HBN_COA_CAL_CLK_NUM_LEN) - 1) << HBN_COA_CAL_CLK_NUM_POS))
#define HBN_STABLE_TIME_AFT_COA_CAL         HBN_STABLE_TIME_AFT_COA_CAL
#define HBN_STABLE_TIME_AFT_COA_CAL_POS     (8U)
#define HBN_STABLE_TIME_AFT_COA_CAL_LEN     (2U)
#define HBN_STABLE_TIME_AFT_COA_CAL_MSK     (((1U << HBN_STABLE_TIME_AFT_COA_CAL_LEN) - 1) << HBN_STABLE_TIME_AFT_COA_CAL_POS)
#define HBN_STABLE_TIME_AFT_COA_CAL_UMSK    (~(((1U << HBN_STABLE_TIME_AFT_COA_CAL_LEN) - 1) << HBN_STABLE_TIME_AFT_COA_CAL_POS))
#define HBN_STABLE_TIME_AFT_FIN_CAL         HBN_STABLE_TIME_AFT_FIN_CAL
#define HBN_STABLE_TIME_AFT_FIN_CAL_POS     (10U)
#define HBN_STABLE_TIME_AFT_FIN_CAL_LEN     (2U)
#define HBN_STABLE_TIME_AFT_FIN_CAL_MSK     (((1U << HBN_STABLE_TIME_AFT_FIN_CAL_LEN) - 1) << HBN_STABLE_TIME_AFT_FIN_CAL_POS)
#define HBN_STABLE_TIME_AFT_FIN_CAL_UMSK    (~(((1U << HBN_STABLE_TIME_AFT_FIN_CAL_LEN) - 1) << HBN_STABLE_TIME_AFT_FIN_CAL_POS))
#define HBN_AVG_XTAL_CNT_32K_RES            HBN_AVG_XTAL_CNT_32K_RES
#define HBN_AVG_XTAL_CNT_32K_RES_POS        (12U)
#define HBN_AVG_XTAL_CNT_32K_RES_LEN        (6U)
#define HBN_AVG_XTAL_CNT_32K_RES_MSK        (((1U << HBN_AVG_XTAL_CNT_32K_RES_LEN) - 1) << HBN_AVG_XTAL_CNT_32K_RES_POS)
#define HBN_AVG_XTAL_CNT_32K_RES_UMSK       (~(((1U << HBN_AVG_XTAL_CNT_32K_RES_LEN) - 1) << HBN_AVG_XTAL_CNT_32K_RES_POS))
#define HBN_AVG_XTAL_CNT_32K_CNT            HBN_AVG_XTAL_CNT_32K_CNT
#define HBN_AVG_XTAL_CNT_32K_CNT_POS        (20U)
#define HBN_AVG_XTAL_CNT_32K_CNT_LEN        (11U)
#define HBN_AVG_XTAL_CNT_32K_CNT_MSK        (((1U << HBN_AVG_XTAL_CNT_32K_CNT_LEN) - 1) << HBN_AVG_XTAL_CNT_32K_CNT_POS)
#define HBN_AVG_XTAL_CNT_32K_CNT_UMSK       (~(((1U << HBN_AVG_XTAL_CNT_32K_CNT_LEN) - 1) << HBN_AVG_XTAL_CNT_32K_CNT_POS))

/* 0x7C : Digital Control */
#define HBN_XTAL32K_OFFSET                  (0x7C)
#define HBN_XTAL32K_OUTBUF_EN_AON           HBN_XTAL32K_OUTBUF_EN_AON
#define HBN_XTAL32K_OUTBUF_EN_AON_POS       (0U)
#define HBN_XTAL32K_OUTBUF_EN_AON_LEN       (1U)
#define HBN_XTAL32K_OUTBUF_EN_AON_MSK       (((1U << HBN_XTAL32K_OUTBUF_EN_AON_LEN) - 1) << HBN_XTAL32K_OUTBUF_EN_AON_POS)
#define HBN_XTAL32K_OUTBUF_EN_AON_UMSK      (~(((1U << HBN_XTAL32K_OUTBUF_EN_AON_LEN) - 1) << HBN_XTAL32K_OUTBUF_EN_AON_POS))
#define HBN_XTAL32K_FAST_STARTUP_AON        HBN_XTAL32K_FAST_STARTUP_AON
#define HBN_XTAL32K_FAST_STARTUP_AON_POS    (1U)
#define HBN_XTAL32K_FAST_STARTUP_AON_LEN    (1U)
#define HBN_XTAL32K_FAST_STARTUP_AON_MSK    (((1U << HBN_XTAL32K_FAST_STARTUP_AON_LEN) - 1) << HBN_XTAL32K_FAST_STARTUP_AON_POS)
#define HBN_XTAL32K_FAST_STARTUP_AON_UMSK   (~(((1U << HBN_XTAL32K_FAST_STARTUP_AON_LEN) - 1) << HBN_XTAL32K_FAST_STARTUP_AON_POS))
#define HBN_XTAL32K_AMP_CTRL_AON            HBN_XTAL32K_AMP_CTRL_AON
#define HBN_XTAL32K_AMP_CTRL_AON_POS        (2U)
#define HBN_XTAL32K_AMP_CTRL_AON_LEN        (2U)
#define HBN_XTAL32K_AMP_CTRL_AON_MSK        (((1U << HBN_XTAL32K_AMP_CTRL_AON_LEN) - 1) << HBN_XTAL32K_AMP_CTRL_AON_POS)
#define HBN_XTAL32K_AMP_CTRL_AON_UMSK       (~(((1U << HBN_XTAL32K_AMP_CTRL_AON_LEN) - 1) << HBN_XTAL32K_AMP_CTRL_AON_POS))
#define HBN_XTAL32K_EN_VCLAMP_AON           HBN_XTAL32K_EN_VCLAMP_AON
#define HBN_XTAL32K_EN_VCLAMP_AON_POS       (4U)
#define HBN_XTAL32K_EN_VCLAMP_AON_LEN       (1U)
#define HBN_XTAL32K_EN_VCLAMP_AON_MSK       (((1U << HBN_XTAL32K_EN_VCLAMP_AON_LEN) - 1) << HBN_XTAL32K_EN_VCLAMP_AON_POS)
#define HBN_XTAL32K_EN_VCLAMP_AON_UMSK      (~(((1U << HBN_XTAL32K_EN_VCLAMP_AON_LEN) - 1) << HBN_XTAL32K_EN_VCLAMP_AON_POS))
#define HBN_XTAL32K_HYST_SEL_AON            HBN_XTAL32K_HYST_SEL_AON
#define HBN_XTAL32K_HYST_SEL_AON_POS        (5U)
#define HBN_XTAL32K_HYST_SEL_AON_LEN        (1U)
#define HBN_XTAL32K_HYST_SEL_AON_MSK        (((1U << HBN_XTAL32K_HYST_SEL_AON_LEN) - 1) << HBN_XTAL32K_HYST_SEL_AON_POS)
#define HBN_XTAL32K_HYST_SEL_AON_UMSK       (~(((1U << HBN_XTAL32K_HYST_SEL_AON_LEN) - 1) << HBN_XTAL32K_HYST_SEL_AON_POS))
#define HBN_XTAL32K_EXT_SEL_AON             HBN_XTAL32K_EXT_SEL_AON
#define HBN_XTAL32K_EXT_SEL_AON_POS         (10U)
#define HBN_XTAL32K_EXT_SEL_AON_LEN         (1U)
#define HBN_XTAL32K_EXT_SEL_AON_MSK         (((1U << HBN_XTAL32K_EXT_SEL_AON_LEN) - 1) << HBN_XTAL32K_EXT_SEL_AON_POS)
#define HBN_XTAL32K_EXT_SEL_AON_UMSK        (~(((1U << HBN_XTAL32K_EXT_SEL_AON_LEN) - 1) << HBN_XTAL32K_EXT_SEL_AON_POS))
#define HBN_XTAL32K_HIZ_EN_AON              HBN_XTAL32K_HIZ_EN_AON
#define HBN_XTAL32K_HIZ_EN_AON_POS          (11U)
#define HBN_XTAL32K_HIZ_EN_AON_LEN          (1U)
#define HBN_XTAL32K_HIZ_EN_AON_MSK          (((1U << HBN_XTAL32K_HIZ_EN_AON_LEN) - 1) << HBN_XTAL32K_HIZ_EN_AON_POS)
#define HBN_XTAL32K_HIZ_EN_AON_UMSK         (~(((1U << HBN_XTAL32K_HIZ_EN_AON_LEN) - 1) << HBN_XTAL32K_HIZ_EN_AON_POS))
#define HBN_XTAL32K_INV_STRE_AON            HBN_XTAL32K_INV_STRE_AON
#define HBN_XTAL32K_INV_STRE_AON_POS        (12U)
#define HBN_XTAL32K_INV_STRE_AON_LEN        (3U)
#define HBN_XTAL32K_INV_STRE_AON_MSK        (((1U << HBN_XTAL32K_INV_STRE_AON_LEN) - 1) << HBN_XTAL32K_INV_STRE_AON_POS)
#define HBN_XTAL32K_INV_STRE_AON_UMSK       (~(((1U << HBN_XTAL32K_INV_STRE_AON_LEN) - 1) << HBN_XTAL32K_INV_STRE_AON_POS))
#define HBN_XTAL32K_OTF_SHORT_AON           HBN_XTAL32K_OTF_SHORT_AON
#define HBN_XTAL32K_OTF_SHORT_AON_POS       (16U)
#define HBN_XTAL32K_OTF_SHORT_AON_LEN       (1U)
#define HBN_XTAL32K_OTF_SHORT_AON_MSK       (((1U << HBN_XTAL32K_OTF_SHORT_AON_LEN) - 1) << HBN_XTAL32K_OTF_SHORT_AON_POS)
#define HBN_XTAL32K_OTF_SHORT_AON_UMSK      (~(((1U << HBN_XTAL32K_OTF_SHORT_AON_LEN) - 1) << HBN_XTAL32K_OTF_SHORT_AON_POS))
#define HBN_XTAL32K_OUTBUF_STRE_AON         HBN_XTAL32K_OUTBUF_STRE_AON
#define HBN_XTAL32K_OUTBUF_STRE_AON_POS     (17U)
#define HBN_XTAL32K_OUTBUF_STRE_AON_LEN     (1U)
#define HBN_XTAL32K_OUTBUF_STRE_AON_MSK     (((1U << HBN_XTAL32K_OUTBUF_STRE_AON_LEN) - 1) << HBN_XTAL32K_OUTBUF_STRE_AON_POS)
#define HBN_XTAL32K_OUTBUF_STRE_AON_UMSK    (~(((1U << HBN_XTAL32K_OUTBUF_STRE_AON_LEN) - 1) << HBN_XTAL32K_OUTBUF_STRE_AON_POS))
#define HBN_XTAL32K_RDY_RSTB_AON            HBN_XTAL32K_RDY_RSTB_AON
#define HBN_XTAL32K_RDY_RSTB_AON_POS        (18U)
#define HBN_XTAL32K_RDY_RSTB_AON_LEN        (1U)
#define HBN_XTAL32K_RDY_RSTB_AON_MSK        (((1U << HBN_XTAL32K_RDY_RSTB_AON_LEN) - 1) << HBN_XTAL32K_RDY_RSTB_AON_POS)
#define HBN_XTAL32K_RDY_RSTB_AON_UMSK       (~(((1U << HBN_XTAL32K_RDY_RSTB_AON_LEN) - 1) << HBN_XTAL32K_RDY_RSTB_AON_POS))
#define HBN_XTAL32K_GM_PD_CTRL_AON          HBN_XTAL32K_GM_PD_CTRL_AON
#define HBN_XTAL32K_GM_PD_CTRL_AON_POS      (19U)
#define HBN_XTAL32K_GM_PD_CTRL_AON_LEN      (1U)
#define HBN_XTAL32K_GM_PD_CTRL_AON_MSK      (((1U << HBN_XTAL32K_GM_PD_CTRL_AON_LEN) - 1) << HBN_XTAL32K_GM_PD_CTRL_AON_POS)
#define HBN_XTAL32K_GM_PD_CTRL_AON_UMSK     (~(((1U << HBN_XTAL32K_GM_PD_CTRL_AON_LEN) - 1) << HBN_XTAL32K_GM_PD_CTRL_AON_POS))
#define HBN_XTAL32K_RDY_SEL_AON             HBN_XTAL32K_RDY_SEL_AON
#define HBN_XTAL32K_RDY_SEL_AON_POS         (20U)
#define HBN_XTAL32K_RDY_SEL_AON_LEN         (2U)
#define HBN_XTAL32K_RDY_SEL_AON_MSK         (((1U << HBN_XTAL32K_RDY_SEL_AON_LEN) - 1) << HBN_XTAL32K_RDY_SEL_AON_POS)
#define HBN_XTAL32K_RDY_SEL_AON_UMSK        (~(((1U << HBN_XTAL32K_RDY_SEL_AON_LEN) - 1) << HBN_XTAL32K_RDY_SEL_AON_POS))
#define HBN_XTAL32K_REG_AON                 HBN_XTAL32K_REG_AON
#define HBN_XTAL32K_REG_AON_POS             (24U)
#define HBN_XTAL32K_REG_AON_LEN             (2U)
#define HBN_XTAL32K_REG_AON_MSK             (((1U << HBN_XTAL32K_REG_AON_LEN) - 1) << HBN_XTAL32K_REG_AON_POS)
#define HBN_XTAL32K_REG_AON_UMSK            (~(((1U << HBN_XTAL32K_REG_AON_LEN) - 1) << HBN_XTAL32K_REG_AON_POS))
#define HBN_XTAL32K_CLK_RDY                 HBN_XTAL32K_CLK_RDY
#define HBN_XTAL32K_CLK_RDY_POS             (26U)
#define HBN_XTAL32K_CLK_RDY_LEN             (1U)
#define HBN_XTAL32K_CLK_RDY_MSK             (((1U << HBN_XTAL32K_CLK_RDY_LEN) - 1) << HBN_XTAL32K_CLK_RDY_POS)
#define HBN_XTAL32K_CLK_RDY_UMSK            (~(((1U << HBN_XTAL32K_CLK_RDY_LEN) - 1) << HBN_XTAL32K_CLK_RDY_POS))
#define HBN_PU_XTAL32K_AON                  HBN_PU_XTAL32K_AON
#define HBN_PU_XTAL32K_AON_POS              (27U)
#define HBN_PU_XTAL32K_AON_LEN              (1U)
#define HBN_PU_XTAL32K_AON_MSK              (((1U << HBN_PU_XTAL32K_AON_LEN) - 1) << HBN_PU_XTAL32K_AON_POS)
#define HBN_PU_XTAL32K_AON_UMSK             (~(((1U << HBN_PU_XTAL32K_AON_LEN) - 1) << HBN_PU_XTAL32K_AON_POS))

/* 0x80 : Digital Control */
#define HBN_RC32K_XTAL32K_TEN_OFFSET        (0x80)
#define HBN_TEN_XTAL32K_AON                 HBN_TEN_XTAL32K_AON
#define HBN_TEN_XTAL32K_AON_POS             (12U)
#define HBN_TEN_XTAL32K_AON_LEN             (1U)
#define HBN_TEN_XTAL32K_AON_MSK             (((1U << HBN_TEN_XTAL32K_AON_LEN) - 1) << HBN_TEN_XTAL32K_AON_POS)
#define HBN_TEN_XTAL32K_AON_UMSK            (~(((1U << HBN_TEN_XTAL32K_AON_LEN) - 1) << HBN_TEN_XTAL32K_AON_POS))
#define HBN_DTEN_RC32K_AON                  HBN_DTEN_RC32K_AON
#define HBN_DTEN_RC32K_AON_POS              (15U)
#define HBN_DTEN_RC32K_AON_LEN              (1U)
#define HBN_DTEN_RC32K_AON_MSK              (((1U << HBN_DTEN_RC32K_AON_LEN) - 1) << HBN_DTEN_RC32K_AON_POS)
#define HBN_DTEN_RC32K_AON_UMSK             (~(((1U << HBN_DTEN_RC32K_AON_LEN) - 1) << HBN_DTEN_RC32K_AON_POS))
#define HBN_DTEN_XTAL32K_CLK_AON            HBN_DTEN_XTAL32K_CLK_AON
#define HBN_DTEN_XTAL32K_CLK_AON_POS        (20U)
#define HBN_DTEN_XTAL32K_CLK_AON_LEN        (1U)
#define HBN_DTEN_XTAL32K_CLK_AON_MSK        (((1U << HBN_DTEN_XTAL32K_CLK_AON_LEN) - 1) << HBN_DTEN_XTAL32K_CLK_AON_POS)
#define HBN_DTEN_XTAL32K_CLK_AON_UMSK       (~(((1U << HBN_DTEN_XTAL32K_CLK_AON_LEN) - 1) << HBN_DTEN_XTAL32K_CLK_AON_POS))
#define HBN_DTEN_XTAL32K_PU_AON             HBN_DTEN_XTAL32K_PU_AON
#define HBN_DTEN_XTAL32K_PU_AON_POS         (21U)
#define HBN_DTEN_XTAL32K_PU_AON_LEN         (1U)
#define HBN_DTEN_XTAL32K_PU_AON_MSK         (((1U << HBN_DTEN_XTAL32K_PU_AON_LEN) - 1) << HBN_DTEN_XTAL32K_PU_AON_POS)
#define HBN_DTEN_XTAL32K_PU_AON_UMSK        (~(((1U << HBN_DTEN_XTAL32K_PU_AON_LEN) - 1) << HBN_DTEN_XTAL32K_PU_AON_POS))
#define HBN_DTEN_XTAL32K_RDY_AON            HBN_DTEN_XTAL32K_RDY_AON
#define HBN_DTEN_XTAL32K_RDY_AON_POS        (22U)
#define HBN_DTEN_XTAL32K_RDY_AON_LEN        (1U)
#define HBN_DTEN_XTAL32K_RDY_AON_MSK        (((1U << HBN_DTEN_XTAL32K_RDY_AON_LEN) - 1) << HBN_DTEN_XTAL32K_RDY_AON_POS)
#define HBN_DTEN_XTAL32K_RDY_AON_UMSK       (~(((1U << HBN_DTEN_XTAL32K_RDY_AON_LEN) - 1) << HBN_DTEN_XTAL32K_RDY_AON_POS))

/* 0x84 : Digital Control */
#define HBN_LDO_3_OFFSET                    (0x84)
#define HBN_LDO13SYS_PU_CNT                 HBN_LDO13SYS_PU_CNT
#define HBN_LDO13SYS_PU_CNT_POS             (0U)
#define HBN_LDO13SYS_PU_CNT_LEN             (10U)
#define HBN_LDO13SYS_PU_CNT_MSK             (((1U << HBN_LDO13SYS_PU_CNT_LEN) - 1) << HBN_LDO13SYS_PU_CNT_POS)
#define HBN_LDO13SYS_PU_CNT_UMSK            (~(((1U << HBN_LDO13SYS_PU_CNT_LEN) - 1) << HBN_LDO13SYS_PU_CNT_POS))
#define HBN_EXT_DCDC2_PU_CNT                HBN_EXT_DCDC2_PU_CNT
#define HBN_EXT_DCDC2_PU_CNT_POS            (10U)
#define HBN_EXT_DCDC2_PU_CNT_LEN            (10U)
#define HBN_EXT_DCDC2_PU_CNT_MSK            (((1U << HBN_EXT_DCDC2_PU_CNT_LEN) - 1) << HBN_EXT_DCDC2_PU_CNT_POS)
#define HBN_EXT_DCDC2_PU_CNT_UMSK           (~(((1U << HBN_EXT_DCDC2_PU_CNT_LEN) - 1) << HBN_EXT_DCDC2_PU_CNT_POS))
#define HBN_EXT_DCDC1_PU_CNT                HBN_EXT_DCDC1_PU_CNT
#define HBN_EXT_DCDC1_PU_CNT_POS            (20U)
#define HBN_EXT_DCDC1_PU_CNT_LEN            (10U)
#define HBN_EXT_DCDC1_PU_CNT_MSK            (((1U << HBN_EXT_DCDC1_PU_CNT_LEN) - 1) << HBN_EXT_DCDC1_PU_CNT_POS)
#define HBN_EXT_DCDC1_PU_CNT_UMSK           (~(((1U << HBN_EXT_DCDC1_PU_CNT_LEN) - 1) << HBN_EXT_DCDC1_PU_CNT_POS))

/* 0x100 : hbn_rc32m_ctrl0 */
#define HBN_RC32M_CTRL0_OFFSET              (0x100)
#define HBN_RC32M_CODE_FR_EXT               HBN_RC32M_CODE_FR_EXT
#define HBN_RC32M_CODE_FR_EXT_POS           (0U)
#define HBN_RC32M_CODE_FR_EXT_LEN           (8U)
#define HBN_RC32M_CODE_FR_EXT_MSK           (((1U << HBN_RC32M_CODE_FR_EXT_LEN) - 1) << HBN_RC32M_CODE_FR_EXT_POS)
#define HBN_RC32M_CODE_FR_EXT_UMSK          (~(((1U << HBN_RC32M_CODE_FR_EXT_LEN) - 1) << HBN_RC32M_CODE_FR_EXT_POS))
#define HBN_RC32M_CODE_FR_EXT2              HBN_RC32M_CODE_FR_EXT2
#define HBN_RC32M_CODE_FR_EXT2_POS          (8U)
#define HBN_RC32M_CODE_FR_EXT2_LEN          (8U)
#define HBN_RC32M_CODE_FR_EXT2_MSK          (((1U << HBN_RC32M_CODE_FR_EXT2_LEN) - 1) << HBN_RC32M_CODE_FR_EXT2_POS)
#define HBN_RC32M_CODE_FR_EXT2_UMSK         (~(((1U << HBN_RC32M_CODE_FR_EXT2_LEN) - 1) << HBN_RC32M_CODE_FR_EXT2_POS))
#define HBN_RC32M_EXT_CODE_EN               HBN_RC32M_EXT_CODE_EN
#define HBN_RC32M_EXT_CODE_EN_POS           (16U)
#define HBN_RC32M_EXT_CODE_EN_LEN           (1U)
#define HBN_RC32M_EXT_CODE_EN_MSK           (((1U << HBN_RC32M_EXT_CODE_EN_LEN) - 1) << HBN_RC32M_EXT_CODE_EN_POS)
#define HBN_RC32M_EXT_CODE_EN_UMSK          (~(((1U << HBN_RC32M_EXT_CODE_EN_LEN) - 1) << HBN_RC32M_EXT_CODE_EN_POS))
#define HBN_RC32M_EXT_CODE_SEL              HBN_RC32M_EXT_CODE_SEL
#define HBN_RC32M_EXT_CODE_SEL_POS          (17U)
#define HBN_RC32M_EXT_CODE_SEL_LEN          (1U)
#define HBN_RC32M_EXT_CODE_SEL_MSK          (((1U << HBN_RC32M_EXT_CODE_SEL_LEN) - 1) << HBN_RC32M_EXT_CODE_SEL_POS)
#define HBN_RC32M_EXT_CODE_SEL_UMSK         (~(((1U << HBN_RC32M_EXT_CODE_SEL_LEN) - 1) << HBN_RC32M_EXT_CODE_SEL_POS))
#define HBN_RCHF_CLKX2_EN                   HBN_RCHF_CLKX2_EN
#define HBN_RCHF_CLKX2_EN_POS               (18U)
#define HBN_RCHF_CLKX2_EN_LEN               (1U)
#define HBN_RCHF_CLKX2_EN_MSK               (((1U << HBN_RCHF_CLKX2_EN_LEN) - 1) << HBN_RCHF_CLKX2_EN_POS)
#define HBN_RCHF_CLKX2_EN_UMSK              (~(((1U << HBN_RCHF_CLKX2_EN_LEN) - 1) << HBN_RCHF_CLKX2_EN_POS))
#define HBN_RCHF_16M_SEL                    HBN_RCHF_16M_SEL
#define HBN_RCHF_16M_SEL_POS                (19U)
#define HBN_RCHF_16M_SEL_LEN                (1U)
#define HBN_RCHF_16M_SEL_MSK                (((1U << HBN_RCHF_16M_SEL_LEN) - 1) << HBN_RCHF_16M_SEL_POS)
#define HBN_RCHF_16M_SEL_UMSK               (~(((1U << HBN_RCHF_16M_SEL_LEN) - 1) << HBN_RCHF_16M_SEL_POS))
#define HBN_RC32M_CODE_FR_CAL               HBN_RC32M_CODE_FR_CAL
#define HBN_RC32M_CODE_FR_CAL_POS           (24U)
#define HBN_RC32M_CODE_FR_CAL_LEN           (8U)
#define HBN_RC32M_CODE_FR_CAL_MSK           (((1U << HBN_RC32M_CODE_FR_CAL_LEN) - 1) << HBN_RC32M_CODE_FR_CAL_POS)
#define HBN_RC32M_CODE_FR_CAL_UMSK          (~(((1U << HBN_RC32M_CODE_FR_CAL_LEN) - 1) << HBN_RC32M_CODE_FR_CAL_POS))

/* 0x364 : Watch-dog reset/interrupt Mode */
#define HBN_WMER_OFFSET                     (0x364)
#define HBN_WE                              HBN_WE
#define HBN_WE_POS                          (0U)
#define HBN_WE_LEN                          (1U)
#define HBN_WE_MSK                          (((1U << HBN_WE_LEN) - 1) << HBN_WE_POS)
#define HBN_WE_UMSK                         (~(((1U << HBN_WE_LEN) - 1) << HBN_WE_POS))
#define HBN_WRIE                            HBN_WRIE
#define HBN_WRIE_POS                        (1U)
#define HBN_WRIE_LEN                        (1U)
#define HBN_WRIE_MSK                        (((1U << HBN_WRIE_LEN) - 1) << HBN_WRIE_POS)
#define HBN_WRIE_UMSK                       (~(((1U << HBN_WRIE_LEN) - 1) << HBN_WRIE_POS))

/* 0x368 : Watch-dog Match Value */
#define HBN_WMR_OFFSET                      (0x368)
#define HBN_WMR                             HBN_WMR
#define HBN_WMR_POS                         (0U)
#define HBN_WMR_LEN                         (16U)
#define HBN_WMR_MSK                         (((1U << HBN_WMR_LEN) - 1) << HBN_WMR_POS)
#define HBN_WMR_UMSK                        (~(((1U << HBN_WMR_LEN) - 1) << HBN_WMR_POS))
#define HBN_WDT_ALIGN                       HBN_WDT_ALIGN
#define HBN_WDT_ALIGN_POS                   (16U)
#define HBN_WDT_ALIGN_LEN                   (1U)
#define HBN_WDT_ALIGN_MSK                   (((1U << HBN_WDT_ALIGN_LEN) - 1) << HBN_WDT_ALIGN_POS)
#define HBN_WDT_ALIGN_UMSK                  (~(((1U << HBN_WDT_ALIGN_LEN) - 1) << HBN_WDT_ALIGN_POS))

/* 0x36C : Watch-dog Counter Value */
#define HBN_WVR_OFFSET                      (0x36C)
#define HBN_WDT_CNT                         HBN_WDT_CNT
#define HBN_WDT_CNT_POS                     (0U)
#define HBN_WDT_CNT_LEN                     (16U)
#define HBN_WDT_CNT_MSK                     (((1U << HBN_WDT_CNT_LEN) - 1) << HBN_WDT_CNT_POS)
#define HBN_WDT_CNT_UMSK                    (~(((1U << HBN_WDT_CNT_LEN) - 1) << HBN_WDT_CNT_POS))

/* 0x370 : Watch-dog Reset Status */
#define HBN_WSR_OFFSET                      (0x370)
#define HBN_WTS                             HBN_WTS
#define HBN_WTS_POS                         (0U)
#define HBN_WTS_LEN                         (1U)
#define HBN_WTS_MSK                         (((1U << HBN_WTS_LEN) - 1) << HBN_WTS_POS)
#define HBN_WTS_UMSK                        (~(((1U << HBN_WTS_LEN) - 1) << HBN_WTS_POS))

/* 0x380 : WDT Interrupt Clear */
#define HBN_WICR_OFFSET                     (0x380)
#define HBN_WICLR                           HBN_WICLR
#define HBN_WICLR_POS                       (0U)
#define HBN_WICLR_LEN                       (1U)
#define HBN_WICLR_MSK                       (((1U << HBN_WICLR_LEN) - 1) << HBN_WICLR_POS)
#define HBN_WICLR_UMSK                      (~(((1U << HBN_WICLR_LEN) - 1) << HBN_WICLR_POS))

/* 0x398 : WDT Counter Reset */
#define HBN_WCR_OFFSET                      (0x398)
#define HBN_WCR                             HBN_WCR
#define HBN_WCR_POS                         (0U)
#define HBN_WCR_LEN                         (1U)
#define HBN_WCR_MSK                         (((1U << HBN_WCR_LEN) - 1) << HBN_WCR_POS)
#define HBN_WCR_UMSK                        (~(((1U << HBN_WCR_LEN) - 1) << HBN_WCR_POS))

/* 0x39C : WDT Access Key1 */
#define HBN_WFAR_OFFSET                     (0x39C)
#define HBN_WFAR                            HBN_WFAR
#define HBN_WFAR_POS                        (0U)
#define HBN_WFAR_LEN                        (16U)
#define HBN_WFAR_MSK                        (((1U << HBN_WFAR_LEN) - 1) << HBN_WFAR_POS)
#define HBN_WFAR_UMSK                       (~(((1U << HBN_WFAR_LEN) - 1) << HBN_WFAR_POS))

/* 0x3A0 : WDT Access Key2 */
#define HBN_WSAR_OFFSET                     (0x3A0)
#define HBN_WSAR                            HBN_WSAR
#define HBN_WSAR_POS                        (0U)
#define HBN_WSAR_LEN                        (16U)
#define HBN_WSAR_MSK                        (((1U << HBN_WSAR_LEN) - 1) << HBN_WSAR_POS)
#define HBN_WSAR_UMSK                       (~(((1U << HBN_WSAR_LEN) - 1) << HBN_WSAR_POS))

/* 0x3CC : Timer Division Force */
#define HBN_TCDR_FORCE_OFFSET               (0x3CC)
#define HBN_TCDR2_FORCE                     HBN_TCDR2_FORCE
#define HBN_TCDR2_FORCE_POS                 (1U)
#define HBN_TCDR2_FORCE_LEN                 (1U)
#define HBN_TCDR2_FORCE_MSK                 (((1U << HBN_TCDR2_FORCE_LEN) - 1) << HBN_TCDR2_FORCE_POS)
#define HBN_TCDR2_FORCE_UMSK                (~(((1U << HBN_TCDR2_FORCE_LEN) - 1) << HBN_TCDR2_FORCE_POS))
#define HBN_TCDR3_FORCE                     HBN_TCDR3_FORCE
#define HBN_TCDR3_FORCE_POS                 (2U)
#define HBN_TCDR3_FORCE_LEN                 (1U)
#define HBN_TCDR3_FORCE_MSK                 (((1U << HBN_TCDR3_FORCE_LEN) - 1) << HBN_TCDR3_FORCE_POS)
#define HBN_TCDR3_FORCE_UMSK                (~(((1U << HBN_TCDR3_FORCE_LEN) - 1) << HBN_TCDR3_FORCE_POS))
#define HBN_WCDR_FORCE                      HBN_WCDR_FORCE
#define HBN_WCDR_FORCE_POS                  (4U)
#define HBN_WCDR_FORCE_LEN                  (1U)
#define HBN_WCDR_FORCE_MSK                  (((1U << HBN_WCDR_FORCE_LEN) - 1) << HBN_WCDR_FORCE_POS)
#define HBN_WCDR_FORCE_UMSK                 (~(((1U << HBN_WCDR_FORCE_LEN) - 1) << HBN_WCDR_FORCE_POS))

/* 0x3F0 : tmr_version */
#define HBN_TMR_VERSION_OFFSET              (0x3F0)
#define HBN_TMR_RESV_FOR_SOFTWARE           HBN_TMR_RESV_FOR_SOFTWARE
#define HBN_TMR_RESV_FOR_SOFTWARE_POS       (0U)
#define HBN_TMR_RESV_FOR_SOFTWARE_LEN       (24U)
#define HBN_TMR_RESV_FOR_SOFTWARE_MSK       (((1U << HBN_TMR_RESV_FOR_SOFTWARE_LEN) - 1) << HBN_TMR_RESV_FOR_SOFTWARE_POS)
#define HBN_TMR_RESV_FOR_SOFTWARE_UMSK      (~(((1U << HBN_TMR_RESV_FOR_SOFTWARE_LEN) - 1) << HBN_TMR_RESV_FOR_SOFTWARE_POS))
#define HBN_TMR_VERSION_ID                  HBN_TMR_VERSION_ID
#define HBN_TMR_VERSION_ID_POS              (24U)
#define HBN_TMR_VERSION_ID_LEN              (8U)
#define HBN_TMR_VERSION_ID_MSK              (((1U << HBN_TMR_VERSION_ID_LEN) - 1) << HBN_TMR_VERSION_ID_POS)
#define HBN_TMR_VERSION_ID_UMSK             (~(((1U << HBN_TMR_VERSION_ID_LEN) - 1) << HBN_TMR_VERSION_ID_POS))

struct hbn_reg {
    /* 0x0 : HBN_CTL */
    union {
        struct {
            uint32_t rtc_ctl                    : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t rtc_dly_option             : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5                 : 1; /* [    5],       rsvd,        0x0 */
            uint32_t swrst_wdt                  : 1; /* [    6],        r/w,        0x0 */
            uint32_t hbn_mode                   : 1; /* [    7],          w,        0x0 */
            uint32_t trap_mode                  : 1; /* [    8],          r,        0x0 */
            uint32_t pwrdn_hbn_core             : 1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10                : 1; /* [   10],       rsvd,        0x0 */
            uint32_t pwrdn_hbn_rtc              : 1; /* [   11],        r/w,        0x0 */
            uint32_t sw_rst                     : 1; /* [   12],        r/w,        0x0 */
            uint32_t hbn_dis_pwr_off_ldo_soc    : 1; /* [   13],        r/w,        0x0 */
            uint32_t hbn_dis_pwr_off_ldo13sys   : 1; /* [   14],        r/w,        0x0 */
            uint32_t hbn_ldo08aon_vout_trim_aon : 4; /* [18:15],        r/w,        0x8 */
            uint32_t hbn_ldo08aon_vout_sel_aon  : 4; /* [22:19],        r/w,        0x9 */
            uint32_t ldo_lp_soc_en_aon_en       : 1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24                : 1; /* [   24],       rsvd,        0x0 */
            uint32_t pwr_on_option              : 1; /* [   25],        r/w,        0x0 */
            uint32_t sram_slp_option            : 1; /* [   26],        r/w,        0x0 */
            uint32_t sram_slp                   : 1; /* [   27],          r,        0x0 */
            uint32_t hbn_state                  : 4; /* [31:28],          r,        0x0 */
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
            uint32_t hbn_time_h               : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pds_hbn_xtal_div         : 11; /* [26:16],        r/w,      0x4e2 */
            uint32_t pds_hbn_xtal_div_en      : 1;  /* [   27],        r/w,        0x1 */
            uint32_t pds_hbn_rtc_slow_clk_sel : 1;  /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31           : 3;  /* [31:29],       rsvd,        0x0 */
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
            uint32_t rtc_time_latch_h : 16; /* [15: 0],          r,        0x0 */
            uint32_t reserved_16_30   : 15; /* [30:16],       rsvd,        0x0 */
            uint32_t rtc_time_latch   : 1;  /* [   31],          w,        0x0 */
        } BF;
        uint32_t WORD;
    } RTC_TIME_H;

    /* 0x14 : HBN_IRQ_MODE */
    union {
        struct {
            uint32_t hbn_pin_wakeup_mode : 4; /* [ 3: 0],        r/w,        0x5 */
            uint32_t hbn_pin_wakeup_mask : 6; /* [ 9: 4],        r/w,        0x0 */
            uint32_t reserved_10_15      : 6; /* [15:10],       rsvd,        0x0 */
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

    /* 0x20 : HBN_EXT_DCDC_CFG */
    /* 0x20 : HBN_PIR_CFG */
    union {
        struct {
            uint32_t ext_dcdc1_aon_ctrl_hw_en : 1;  /* [    0],        r/w,        0x0 */
            uint32_t ext_dcdc1_aon_gpio_sel   : 3;  /* [ 3: 1],        r/w,        0x0 */
            uint32_t hw_ext_dcdc1_on_en       : 1;  /* [    4],        r/w,        0x0 */
            uint32_t ext_dcdc2_aon_ctrl_hw_en : 1;  /* [    5],        r/w,        0x0 */
            uint32_t ext_dcdc2_aon_gpio_sel   : 3;  /* [ 8: 6],        r/w,        0x0 */
            uint32_t hw_ext_dcdc2_on_en       : 1;  /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31           : 22; /* [31:10],       rsvd,        0x0 */
        } BF_HBN_EXT_DCDC_CFG;
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF_HBN_PIR_CFG;
        uint32_t WORD;
    } HBN_EXT_DCDC_CFG_HBN_PIR_CFG;

    /* 0x24 : HBN_PIR_VTH */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PIR_VTH;

    /* 0x28 : HBN_PIR_INTERVAL */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PIR_INTERVAL;

    /* 0x2C : HBN_BOR_CFG */
    union {
        struct {
            uint32_t bod_sel_aon   : 1;  /* [    0],        r/w,        0x0 */
            uint32_t bod_th_aon    : 3;  /* [ 3: 1],        r/w,        0x4 */
            uint32_t pu_bod_aon    : 1;  /* [    4],        r/w,        0x0 */
            uint32_t bod_out       : 1;  /* [    5],          r,        0x0 */
            uint32_t por_out       : 1;  /* [    6],          r,        0x0 */
            uint32_t reserved_7_31 : 25; /* [31: 7],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_BOR_CFG;

    /* 0x30 : HBN_GLB */
    union {
        struct {
            uint32_t hbn_root_clk_sel       : 2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t hbn_uart_clk_sel       : 1; /* [    2],        r/w,        0x0 */
            uint32_t hbn_f32k_sel           : 2; /* [ 4: 3],        r/w,        0x0 */
            uint32_t hbn_pu_rc32k           : 1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6             : 1; /* [    6],       rsvd,        0x0 */
            uint32_t hbn_reset_event        : 6; /* [12: 7],          r,        0x0 */
            uint32_t hbn_clr_reset_event    : 1; /* [   13],        r/w,        0x0 */
            uint32_t gpio_int_det_clk_sel   : 1; /* [   14],        r/w,        0x1 */
            uint32_t hbn_uart_clk_sel2      : 1; /* [   15],        r/w,        0x0 */
            uint32_t reserved_16_23         : 8; /* [23:16],       rsvd,        0x0 */
            uint32_t ldo08aon_vout_trim_aon : 4; /* [27:24],        r/w,        0x8 */
            uint32_t ldo08aon_vout_sel_aon  : 4; /* [31:28],        r/w,        0x9 */
        } BF;
        uint32_t WORD;
    } HBN_GLB;

    /* 0x34 : HBN_SRAM */
    union {
        struct {
            uint32_t reserved_0_5     : 6;  /* [ 5: 0],       rsvd,        0x0 */
            uint32_t retram_ret       : 1;  /* [    6],        r/w,        0x0 */
            uint32_t retram_slp       : 1;  /* [    7],        r/w,        0x0 */
            uint32_t cr_hbnram_dvs_0  : 2;  /* [ 9: 8],        r/w,        0x1 */
            uint32_t cr_hbnram_dvse_0 : 1;  /* [   10],        r/w,        0x0 */
            uint32_t cr_hbnram_nap_0  : 1;  /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_13   : 2;  /* [13:12],       rsvd,        0x0 */
            uint32_t cr_hbnram_tmck_0 : 1;  /* [   14],        r/w,        0x0 */
            uint32_t reserved_15_31   : 17; /* [31:15],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_SRAM;

    /* 0x38 : HBN_PAD_CTRL_0 */
    union {
        struct {
            uint32_t reg_aon_pad_ie_smt    : 6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_9          : 4; /* [ 9: 6],       rsvd,        0x0 */
            uint32_t reg_aon_led_sel       : 9; /* [18:10],        r/w,        0x0 */
            uint32_t reserved_19           : 1; /* [   19],       rsvd,        0x0 */
            uint32_t reg_en_aon_ctrl_gpio  : 6; /* [25:20],        r/w,        0x0 */
            uint32_t reserved_26_27        : 2; /* [27:26],       rsvd,        0x0 */
            uint32_t cr_gpio_keep_en       : 3; /* [30:28],        r/w,        0x0 */
            uint32_t reg_aon_gpio_iso_mode : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PAD_CTRL_0;

    /* 0x3C : HBN_PAD_CTRL_1 */
    union {
        struct {
            uint32_t reg_aon_pad_oe : 6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_9   : 4; /* [ 9: 6],       rsvd,        0x0 */
            uint32_t reg_aon_pad_pd : 6; /* [15:10],        r/w,        0x0 */
            uint32_t reserved_16_19 : 4; /* [19:16],       rsvd,        0x0 */
            uint32_t reg_aon_pad_pu : 6; /* [25:20],        r/w,        0x0 */
            uint32_t reserved_26_31 : 6; /* [31:26],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_PAD_CTRL_1;

    /* 0x40 : vbat_ldo */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } vbat_ldo;

    /* 0x44 : HBN_RSV0 */
    union {
        struct {
            uint32_t HBN_RSV0 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_RSV0;

    /* 0x48 : HBN_RSV1 */
    union {
        struct {
            uint32_t HBN_RSV1 : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } HBN_RSV1;

    /* 0x4C : HBN_RSV2 */
    union {
        struct {
            uint32_t HBN_RSV2 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_RSV2;

    /* 0x50 : HBN_RSV3 */
    union {
        struct {
            uint32_t HBN_RSV3 : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } HBN_RSV3;

    /* 0x54 : HBN_VERSION */
    union {
        struct {
            uint32_t hbn_resv_for_software : 24; /* [23: 0],        r/w,        0x0 */
            uint32_t hbn_version_id        : 8;  /* [31:24],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } HBN_VERSION;

    /* 0x58 : Digital Control */
    union {
        struct {
            uint32_t ldo_soc_pulldown_aon      : 1; /* [    0],        r/w,        0x0 */
            uint32_t ldo_soc_pulldown_sel_aon  : 1; /* [    1],        r/w,        0x0 */
            uint32_t ldo_soc_lp_en_aon         : 1; /* [    2],        r/w,        0x0 */
            uint32_t ldo_soc_lp_en_aon_ctrl_hw : 1; /* [    3],        r/w,        0x0 */
            uint32_t ldo_soc_vout_sel_aon      : 4; /* [ 7: 4],        r/w,        0x4 */
            uint32_t ldo_soc_vout_trim_aon     : 4; /* [11: 8],        r/w,        0x7 */
            uint32_t pu_ldo_soc_aon            : 1; /* [   12],         rw,        0x0 */
            uint32_t pds_ldo_soc_vout_sel_aon  : 4; /* [16:13],        r/w,        0x4 */
            uint32_t pds_ldo_soc_vout_trim_aon : 4; /* [20:17],        r/w,        0x7 */
            uint32_t ldo_soc_lp_en_aon_hw      : 1; /* [   21],          r,        0x0 */
            uint32_t pu_ldo_soc_aon_hw         : 1; /* [   22],          r,        0x0 */
            uint32_t pd_ldo_soc_aon            : 1; /* [   23],         rw,        0x0 */
            uint32_t reserved_24_31            : 8; /* [31:24],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } hbn_ldo_0;

    /* 0x5C : Digital Control */
    union {
        struct {
            uint32_t ldo13sys_pulldown_aon      : 1;  /* [    0],        r/w,        0x0 */
            uint32_t ldo13sys_pulldown_sel_aon  : 1;  /* [    1],        r/w,        0x0 */
            uint32_t ldo13sys_lp_en_aon         : 1;  /* [    2],         rw,        0x0 */
            uint32_t ldo13sys_lp_en_aon_ctrl_hw : 1;  /* [    3],        r/w,        0x0 */
            uint32_t ldo13sys_vout_sel_aon      : 4;  /* [ 7: 4],        r/w,        0x4 */
            uint32_t pu_ldo13sys_aon            : 1;  /* [    8],         rw,        0x0 */
            uint32_t pd_ldo13sys_aon            : 1;  /* [    9],         rw,        0x0 */
            uint32_t cr_ldo_soc_no_wait         : 1;  /* [   10],         rw,        0x0 */
            uint32_t reserved_11                : 1;  /* [   11],       rsvd,        0x0 */
            uint32_t pds_ldo13sys_vout_sel_aon  : 4;  /* [15:12],        r/w,        0x4 */
            uint32_t ldo13sys_lp_en_aon_hw      : 1;  /* [   16],          r,        0x0 */
            uint32_t pu_ldo13sys_aon_hw         : 1;  /* [   17],          r,        0x0 */
            uint32_t cr_ldo13sys_pon_cnt        : 10; /* [27:18],        r/w,       0x12 */
            uint32_t ldo13sys_vout_trim_aon     : 4;  /* [31:28],        r/w,        0x7 */
        } BF;
        uint32_t WORD;
    } hbn_ldo_1;

    /* 0x60 : Digital Control */
    union {
        struct {
            uint32_t ldo18io_pulldown_aon     : 1;  /* [    0],        r/w,        0x0 */
            uint32_t ldo18io_pulldown_sel_aon : 1;  /* [    1],        r/w,        0x0 */
            uint32_t pu_ldo18io_aon           : 1;  /* [    2],        r/w,        0x1 */
            uint32_t reserved_3               : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t ext_dcdc2_pd_cnt         : 4;  /* [ 7: 4],        r/w,        0x2 */
            uint32_t ext_dcdc1_pd_cnt         : 4;  /* [11: 8],        r/w,        0x4 */
            uint32_t ldo13sys_pd_cnt          : 4;  /* [15:12],        r/w,        0x3 */
            uint32_t ldo_soc_pd_cnt           : 4;  /* [19:16],        r/w,        0x1 */
            uint32_t cr_ldo_soc_pon_cnt       : 10; /* [29:20],        r/w,        0x1 */
            uint32_t reserved_30_31           : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } hbn_ldo_2;

    /* 0x64 : Digital Control */
    union {
        struct {
            uint32_t rc32k_cap_sel_aon     : 4;  /* [ 3: 0],        r/w,        0x8 */
            uint32_t rc32k_code_fr_cal_aon : 10; /* [13: 4],        r/w,      0x200 */
            uint32_t reserved_14_15        : 2;  /* [15:14],       rsvd,        0x0 */
            uint32_t rc32k_tcomp_code_aon  : 10; /* [25:16],        r/w,        0x0 */
            uint32_t rc32k_tcomp_en_aon    : 1;  /* [   26],        r/w,        0x0 */
            uint32_t reserved_27           : 1;  /* [   27],       rsvd,        0x0 */
            uint32_t rc32k_vref_dly_aon    : 2;  /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31        : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32k_0;

    /* 0x68 : Digital Control */
    union {
        struct {
            uint32_t rc32k_rdy                 : 1;  /* [    0],          r,        0x0 */
            uint32_t rc32k_code_valid          : 1;  /* [    1],          r,        0x0 */
            uint32_t reserved_2_3              : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t rc32k_iptat_bm_aon        : 3;  /* [ 6: 4],        r/w,        0x3 */
            uint32_t pu_rc32k_aon              : 1;  /* [    7],          r,        0x1 */
            uint32_t pu_rc32k_iptat_gen_aon    : 1;  /* [    8],        r/w,        0x1 */
            uint32_t rc32k_ext_code_en         : 1;  /* [    9],        r/w,        0x1 */
            uint32_t rc32k_code_fr_cal_aon_hw  : 10; /* [19:10],          r,      0x200 */
            uint32_t rc32k_code_cap_sel_aon_hw : 4;  /* [23:20],          r,        0x8 */
            uint32_t rc32k_fin_cal_start       : 1;  /* [   24],        r/w,        0x0 */
            uint32_t rc32k_coa_cal_start       : 1;  /* [   25],        r/w,        0x0 */
            uint32_t rc32k_cal_en              : 1;  /* [   26],        r/w,        0x0 */
            uint32_t rc32k_cal_done            : 1;  /* [   27],          r,        0x0 */
            uint32_t rc32k_cal_success         : 1;  /* [   28],          r,        0x0 */
            uint32_t cal_avg_xtal_cnt_start    : 1;  /* [   29],        r/w,        0x0 */
            uint32_t cal_avg_xtal_cnt_done     : 1;  /* [   30],          r,        0x0 */
            uint32_t reserved_31               : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32k_1;

    /* 0x6C : Digital Control */
    union {
        struct {
            uint32_t coa_cal_target_cnt  : 18; /* [17: 0],        r/w,   0x186a00 */
            uint32_t coa_cal_target_diff : 14; /* [31:18],        r/w,      0x1f4 */
        } BF;
        uint32_t WORD;
    } rc32k_coa_cal;

    /* 0x70 : Digital Control */
    union {
        struct {
            uint32_t fin_cal_target_cnt : 32; /* [31: 0],        r/w,   0x186a00 */
        } BF;
        uint32_t WORD;
    } rc32k_fin_cal_0;

    /* 0x74 : Digital Control */
    union {
        struct {
            uint32_t fin_cal_target_diff : 32; /* [31: 0],        r/w,     0x1388 */
        } BF;
        uint32_t WORD;
    } rc32k_fin_cal_1;

    /* 0x78 : Digital Control */
    union {
        struct {
            uint32_t fin_cal_clk_num         : 5;  /* [ 4: 0],        r/w,        0x7 */
            uint32_t coa_cal_clk_num         : 3;  /* [ 7: 5],        r/w,        0x3 */
            uint32_t stable_time_aft_coa_cal : 2;  /* [ 9: 8],        r/w,        0x0 */
            uint32_t stable_time_aft_fin_cal : 2;  /* [11:10],        r/w,        0x0 */
            uint32_t avg_xtal_cnt_32k_res    : 6;  /* [17:12],          r,       0x24 */
            uint32_t reserved_18_19          : 2;  /* [19:18],       rsvd,        0x0 */
            uint32_t avg_xtal_cnt_32k_cnt    : 11; /* [30:20],          r,      0x3d0 */
            uint32_t reserved_31             : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32k_2;

    /* 0x7C : Digital Control */
    union {
        struct {
            uint32_t xtal32k_outbuf_en_aon    : 1; /* [    0],        r/w,        0x1 */
            uint32_t xtal32k_fast_startup_aon : 1; /* [    1],        r/w,        0x0 */
            uint32_t xtal32k_amp_ctrl_aon     : 2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t xtal32k_en_vclamp_aon    : 1; /* [    4],        r/w,        0x1 */
            uint32_t xtal32k_hyst_sel_aon     : 1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_9             : 4; /* [ 9: 6],       rsvd,        0x0 */
            uint32_t xtal32k_ext_sel_aon      : 1; /* [   10],        r/w,        0x0 */
            uint32_t xtal32k_hiz_en_aon       : 1; /* [   11],        r/w,        0x1 */
            uint32_t xtal32k_inv_stre_aon     : 3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15              : 1; /* [   15],       rsvd,        0x0 */
            uint32_t xtal32k_otf_short_aon    : 1; /* [   16],        r/w,        0x0 */
            uint32_t xtal32k_outbuf_stre_aon  : 1; /* [   17],        r/w,        0x0 */
            uint32_t xtal32k_rdy_rstb_aon     : 1; /* [   18],        r/w,        0x1 */
            uint32_t xtal32k_gm_pd_ctrl_aon   : 1; /* [   19],        r/w,        0x0 */
            uint32_t xtal32k_rdy_sel_aon      : 2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23           : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t xtal32k_reg_aon          : 2; /* [25:24],        r/w,        0x1 */
            uint32_t xtal32k_clk_rdy          : 1; /* [   26],          r,        0x0 */
            uint32_t pu_xtal32k_aon           : 1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31           : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal32k;

    /* 0x80 : Digital Control */
    union {
        struct {
            uint32_t reserved_0_11        : 12; /* [11: 0],       rsvd,        0x0 */
            uint32_t ten_xtal32k_aon      : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_14       : 2;  /* [14:13],       rsvd,        0x0 */
            uint32_t dten_rc32k_aon       : 1;  /* [   15],        r/w,        0x0 */
            uint32_t reserved_16_19       : 4;  /* [19:16],       rsvd,        0x0 */
            uint32_t dten_xtal32k_clk_aon : 1;  /* [   20],        r/w,        0x0 */
            uint32_t dten_xtal32k_pu_aon  : 1;  /* [   21],        r/w,        0x0 */
            uint32_t dten_xtal32k_rdy_aon : 1;  /* [   22],        r/w,        0x0 */
            uint32_t reserved_23_31       : 9;  /* [31:23],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32k_xtal32k_ten;

    /* 0x84 : Digital Control */
    union {
        struct {
            uint32_t ldo13sys_pu_cnt  : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t ext_dcdc2_pu_cnt : 10; /* [19:10],        r/w,        0x0 */
            uint32_t ext_dcdc1_pu_cnt : 10; /* [29:20],        r/w,        0x0 */
            uint32_t reserved_30_31   : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } hbn_ldo_3;

    /* 0x88  reserved */
    uint8_t RESERVED0x88[120];

    /* 0x100 : hbn_rc32m_ctrl0 */
    union {
        struct {
            uint32_t rc32m_code_fr_ext  : 8; /* [ 7: 0],        r/w,       0x7f */
            uint32_t rc32m_code_fr_ext2 : 8; /* [15: 8],        r/w,       0x7f */
            uint32_t rc32m_ext_code_en  : 1; /* [   16],        r/w,        0x1 */
            uint32_t rc32m_ext_code_sel : 1; /* [   17],        r/w,        0x0 */
            uint32_t rchf_clkx2_en      : 1; /* [   18],        r/w,        0x1 */
            uint32_t rchf_16m_sel       : 1; /* [   19],        r/w,        0x1 */
            uint32_t reserved_20_23     : 4; /* [23:20],       rsvd,        0x0 */
            uint32_t rc32m_code_fr_cal  : 8; /* [31:24],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } hbn_rc32m_ctrl0;

    /* 0x104  reserved */
    uint8_t RESERVED0x104[608];

    /* 0x364 : Watch-dog reset/interrupt Mode */
    union {
        struct {
            uint32_t we            : 1;  /* [    0],        r/w,        0x0 */
            uint32_t wrie          : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31 : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WMER;

    /* 0x368 : Watch-dog Match Value */
    union {
        struct {
            uint32_t wmr            : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t wdt_align      : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31 : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WMR;

    /* 0x36C : Watch-dog Counter Value */
    union {
        struct {
            uint32_t wdt_cnt        : 16; /* [15: 0],          r,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WVR;

    /* 0x370 : Watch-dog Reset Status */
    union {
        struct {
            uint32_t wts           : 1;  /* [    0],          w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WSR;

    /* 0x374  reserved */
    uint8_t RESERVED0x374[12];

    /* 0x380 : WDT Interrupt Clear */
    union {
        struct {
            uint32_t wiclr         : 1;  /* [    0],          w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WICR;

    /* 0x384  reserved */
    uint8_t RESERVED0x384[20];

    /* 0x398 : WDT Counter Reset */
    union {
        struct {
            uint32_t wcr           : 1;  /* [    0],          w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WCR;

    /* 0x39C : WDT Access Key1 */
    union {
        struct {
            uint32_t wfar           : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WFAR;

    /* 0x3A0 : WDT Access Key2 */
    union {
        struct {
            uint32_t wsar           : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WSAR;

    /* 0x3a4  reserved */
    uint8_t RESERVED0x3a4[40];

    /* 0x3CC : Timer Division Force */
    union {
        struct {
            uint32_t reserved_0    : 1;  /* [    0],       rsvd,        0x0 */
            uint32_t tcdr2_force   : 1;  /* [    1],        r/w,        0x0 */
            uint32_t tcdr3_force   : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3    : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t wcdr_force    : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31 : 27; /* [31: 5],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TCDR_FORCE;

    /* 0x3d0  reserved */
    uint8_t RESERVED0x3d0[32];

    /* 0x3F0 : tmr_version */
    union {
        struct {
            uint32_t tmr_resv_for_software : 24; /* [23: 0],        r/w,        0x0 */
            uint32_t tmr_version_id        : 8;  /* [31:24],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } tmr_version;
};

typedef volatile struct hbn_reg hbn_reg_t;

#endif /* __HBN_REG_H__ */
