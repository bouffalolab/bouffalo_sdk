/**
  ******************************************************************************
  * @file    pds_reg.h
  * @version V1.0
  * @date    2024-11-11
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
#ifndef __PDS_REG_H__
#define __PDS_REG_H__

#include "bl616d.h"

/* 0x0 : PDS_CTL */
#define PDS_CTL_OFFSET                            (0x0)
#define PDS_START_PS                              PDS_START_PS
#define PDS_START_PS_POS                          (0U)
#define PDS_START_PS_LEN                          (1U)
#define PDS_START_PS_MSK                          (((1U << PDS_START_PS_LEN) - 1) << PDS_START_PS_POS)
#define PDS_START_PS_UMSK                         (~(((1U << PDS_START_PS_LEN) - 1) << PDS_START_PS_POS))
#define PDS_CR_SLEEP_FOREVER                      PDS_CR_SLEEP_FOREVER
#define PDS_CR_SLEEP_FOREVER_POS                  (1U)
#define PDS_CR_SLEEP_FOREVER_LEN                  (1U)
#define PDS_CR_SLEEP_FOREVER_MSK                  (((1U << PDS_CR_SLEEP_FOREVER_LEN) - 1) << PDS_CR_SLEEP_FOREVER_POS)
#define PDS_CR_SLEEP_FOREVER_UMSK                 (~(((1U << PDS_CR_SLEEP_FOREVER_LEN) - 1) << PDS_CR_SLEEP_FOREVER_POS))
#define PDS_CR_XTAL_FORCE_OFF                     PDS_CR_XTAL_FORCE_OFF
#define PDS_CR_XTAL_FORCE_OFF_POS                 (2U)
#define PDS_CR_XTAL_FORCE_OFF_LEN                 (1U)
#define PDS_CR_XTAL_FORCE_OFF_MSK                 (((1U << PDS_CR_XTAL_FORCE_OFF_LEN) - 1) << PDS_CR_XTAL_FORCE_OFF_POS)
#define PDS_CR_XTAL_FORCE_OFF_UMSK                (~(((1U << PDS_CR_XTAL_FORCE_OFF_LEN) - 1) << PDS_CR_XTAL_FORCE_OFF_POS))
#define PDS_CR_PDS_WIFI_SAVE_STATE                PDS_CR_PDS_WIFI_SAVE_STATE
#define PDS_CR_PDS_WIFI_SAVE_STATE_POS            (3U)
#define PDS_CR_PDS_WIFI_SAVE_STATE_LEN            (1U)
#define PDS_CR_PDS_WIFI_SAVE_STATE_MSK            (((1U << PDS_CR_PDS_WIFI_SAVE_STATE_LEN) - 1) << PDS_CR_PDS_WIFI_SAVE_STATE_POS)
#define PDS_CR_PDS_WIFI_SAVE_STATE_UMSK           (~(((1U << PDS_CR_PDS_WIFI_SAVE_STATE_LEN) - 1) << PDS_CR_PDS_WIFI_SAVE_STATE_POS))
#define PDS_CR_PDS_PD_LDO09                       PDS_CR_PDS_PD_LDO09
#define PDS_CR_PDS_PD_LDO09_POS                   (4U)
#define PDS_CR_PDS_PD_LDO09_LEN                   (1U)
#define PDS_CR_PDS_PD_LDO09_MSK                   (((1U << PDS_CR_PDS_PD_LDO09_LEN) - 1) << PDS_CR_PDS_PD_LDO09_POS)
#define PDS_CR_PDS_PD_LDO09_UMSK                  (~(((1U << PDS_CR_PDS_PD_LDO09_LEN) - 1) << PDS_CR_PDS_PD_LDO09_POS))
#define PDS_CR_PDS_PD_BG_SYS                      PDS_CR_PDS_PD_BG_SYS
#define PDS_CR_PDS_PD_BG_SYS_POS                  (5U)
#define PDS_CR_PDS_PD_BG_SYS_LEN                  (1U)
#define PDS_CR_PDS_PD_BG_SYS_MSK                  (((1U << PDS_CR_PDS_PD_BG_SYS_LEN) - 1) << PDS_CR_PDS_PD_BG_SYS_POS)
#define PDS_CR_PDS_PD_BG_SYS_UMSK                 (~(((1U << PDS_CR_PDS_PD_BG_SYS_LEN) - 1) << PDS_CR_PDS_PD_BG_SYS_POS))
#define PDS_CR_PDS_CTRL_GPIO_IE_PU_PD             PDS_CR_PDS_CTRL_GPIO_IE_PU_PD
#define PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_POS         (6U)
#define PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_LEN         (1U)
#define PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_MSK         (((1U << PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_LEN) - 1) << PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_POS)
#define PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_UMSK        (~(((1U << PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_LEN) - 1) << PDS_CR_PDS_CTRL_GPIO_IE_PU_PD_POS))
#define PDS_CR_PDS_PD_DCDC12                      PDS_CR_PDS_PD_DCDC12
#define PDS_CR_PDS_PD_DCDC12_POS                  (7U)
#define PDS_CR_PDS_PD_DCDC12_LEN                  (1U)
#define PDS_CR_PDS_PD_DCDC12_MSK                  (((1U << PDS_CR_PDS_PD_DCDC12_LEN) - 1) << PDS_CR_PDS_PD_DCDC12_POS)
#define PDS_CR_PDS_PD_DCDC12_UMSK                 (~(((1U << PDS_CR_PDS_PD_DCDC12_LEN) - 1) << PDS_CR_PDS_PD_DCDC12_POS))
#define PDS_CR_PDS_GATE_CLK                       PDS_CR_PDS_GATE_CLK
#define PDS_CR_PDS_GATE_CLK_POS                   (8U)
#define PDS_CR_PDS_GATE_CLK_LEN                   (1U)
#define PDS_CR_PDS_GATE_CLK_MSK                   (((1U << PDS_CR_PDS_GATE_CLK_LEN) - 1) << PDS_CR_PDS_GATE_CLK_POS)
#define PDS_CR_PDS_GATE_CLK_UMSK                  (~(((1U << PDS_CR_PDS_GATE_CLK_LEN) - 1) << PDS_CR_PDS_GATE_CLK_POS))
#define PDS_CR_PDS_MEM_STBY                       PDS_CR_PDS_MEM_STBY
#define PDS_CR_PDS_MEM_STBY_POS                   (9U)
#define PDS_CR_PDS_MEM_STBY_LEN                   (1U)
#define PDS_CR_PDS_MEM_STBY_MSK                   (((1U << PDS_CR_PDS_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MEM_STBY_POS)
#define PDS_CR_PDS_MEM_STBY_UMSK                  (~(((1U << PDS_CR_PDS_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MEM_STBY_POS))
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT          PDS_CR_PDS_GLB_REG_RESET_PROTECT
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS      (10U)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN      (1U)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_MSK      (((1U << PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN) - 1) << PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_UMSK     (~(((1U << PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN) - 1) << PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS))
#define PDS_CR_PDS_ISO_EN                         PDS_CR_PDS_ISO_EN
#define PDS_CR_PDS_ISO_EN_POS                     (11U)
#define PDS_CR_PDS_ISO_EN_LEN                     (1U)
#define PDS_CR_PDS_ISO_EN_MSK                     (((1U << PDS_CR_PDS_ISO_EN_LEN) - 1) << PDS_CR_PDS_ISO_EN_POS)
#define PDS_CR_PDS_ISO_EN_UMSK                    (~(((1U << PDS_CR_PDS_ISO_EN_LEN) - 1) << PDS_CR_PDS_ISO_EN_POS))
#define PDS_CR_PDS_WAIT_XTAL_RDY                  PDS_CR_PDS_WAIT_XTAL_RDY
#define PDS_CR_PDS_WAIT_XTAL_RDY_POS              (12U)
#define PDS_CR_PDS_WAIT_XTAL_RDY_LEN              (1U)
#define PDS_CR_PDS_WAIT_XTAL_RDY_MSK              (((1U << PDS_CR_PDS_WAIT_XTAL_RDY_LEN) - 1) << PDS_CR_PDS_WAIT_XTAL_RDY_POS)
#define PDS_CR_PDS_WAIT_XTAL_RDY_UMSK             (~(((1U << PDS_CR_PDS_WAIT_XTAL_RDY_LEN) - 1) << PDS_CR_PDS_WAIT_XTAL_RDY_POS))
#define PDS_CR_PDS_PWR_OFF                        PDS_CR_PDS_PWR_OFF
#define PDS_CR_PDS_PWR_OFF_POS                    (13U)
#define PDS_CR_PDS_PWR_OFF_LEN                    (1U)
#define PDS_CR_PDS_PWR_OFF_MSK                    (((1U << PDS_CR_PDS_PWR_OFF_LEN) - 1) << PDS_CR_PDS_PWR_OFF_POS)
#define PDS_CR_PDS_PWR_OFF_UMSK                   (~(((1U << PDS_CR_PDS_PWR_OFF_LEN) - 1) << PDS_CR_PDS_PWR_OFF_POS))
#define PDS_CR_PDS_PD_XTAL                        PDS_CR_PDS_PD_XTAL
#define PDS_CR_PDS_PD_XTAL_POS                    (14U)
#define PDS_CR_PDS_PD_XTAL_LEN                    (1U)
#define PDS_CR_PDS_PD_XTAL_MSK                    (((1U << PDS_CR_PDS_PD_XTAL_LEN) - 1) << PDS_CR_PDS_PD_XTAL_POS)
#define PDS_CR_PDS_PD_XTAL_UMSK                   (~(((1U << PDS_CR_PDS_PD_XTAL_LEN) - 1) << PDS_CR_PDS_PD_XTAL_POS))
#define PDS_CR_PDS_CTRL_SOC_ENB                   PDS_CR_PDS_CTRL_SOC_ENB
#define PDS_CR_PDS_CTRL_SOC_ENB_POS               (15U)
#define PDS_CR_PDS_CTRL_SOC_ENB_LEN               (1U)
#define PDS_CR_PDS_CTRL_SOC_ENB_MSK               (((1U << PDS_CR_PDS_CTRL_SOC_ENB_LEN) - 1) << PDS_CR_PDS_CTRL_SOC_ENB_POS)
#define PDS_CR_PDS_CTRL_SOC_ENB_UMSK              (~(((1U << PDS_CR_PDS_CTRL_SOC_ENB_LEN) - 1) << PDS_CR_PDS_CTRL_SOC_ENB_POS))
#define PDS_CR_PDS_RST_SOC                        PDS_CR_PDS_RST_SOC
#define PDS_CR_PDS_RST_SOC_POS                    (16U)
#define PDS_CR_PDS_RST_SOC_LEN                    (1U)
#define PDS_CR_PDS_RST_SOC_MSK                    (((1U << PDS_CR_PDS_RST_SOC_LEN) - 1) << PDS_CR_PDS_RST_SOC_POS)
#define PDS_CR_PDS_RST_SOC_UMSK                   (~(((1U << PDS_CR_PDS_RST_SOC_LEN) - 1) << PDS_CR_PDS_RST_SOC_POS))
#define PDS_CR_PDS_RC32M_OFF_DIS                  PDS_CR_PDS_RC32M_OFF_DIS
#define PDS_CR_PDS_RC32M_OFF_DIS_POS              (17U)
#define PDS_CR_PDS_RC32M_OFF_DIS_LEN              (1U)
#define PDS_CR_PDS_RC32M_OFF_DIS_MSK              (((1U << PDS_CR_PDS_RC32M_OFF_DIS_LEN) - 1) << PDS_CR_PDS_RC32M_OFF_DIS_POS)
#define PDS_CR_PDS_RC32M_OFF_DIS_UMSK             (~(((1U << PDS_CR_PDS_RC32M_OFF_DIS_LEN) - 1) << PDS_CR_PDS_RC32M_OFF_DIS_POS))
#define PDS_CR_PDS_LDO11_VSEL_EN                  PDS_CR_PDS_LDO11_VSEL_EN
#define PDS_CR_PDS_LDO11_VSEL_EN_POS              (18U)
#define PDS_CR_PDS_LDO11_VSEL_EN_LEN              (1U)
#define PDS_CR_PDS_LDO11_VSEL_EN_MSK              (((1U << PDS_CR_PDS_LDO11_VSEL_EN_LEN) - 1) << PDS_CR_PDS_LDO11_VSEL_EN_POS)
#define PDS_CR_PDS_LDO11_VSEL_EN_UMSK             (~(((1U << PDS_CR_PDS_LDO11_VSEL_EN_LEN) - 1) << PDS_CR_PDS_LDO11_VSEL_EN_POS))
#define PDS_CR_PDS_CTRL_USBPLL_PD                 PDS_CR_PDS_CTRL_USBPLL_PD
#define PDS_CR_PDS_CTRL_USBPLL_PD_POS             (19U)
#define PDS_CR_PDS_CTRL_USBPLL_PD_LEN             (1U)
#define PDS_CR_PDS_CTRL_USBPLL_PD_MSK             (((1U << PDS_CR_PDS_CTRL_USBPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_USBPLL_PD_POS)
#define PDS_CR_PDS_CTRL_USBPLL_PD_UMSK            (~(((1U << PDS_CR_PDS_CTRL_USBPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_USBPLL_PD_POS))
#define PDS_CR_PDS_CTRL_AUPLL_PD                  PDS_CR_PDS_CTRL_AUPLL_PD
#define PDS_CR_PDS_CTRL_AUPLL_PD_POS              (20U)
#define PDS_CR_PDS_CTRL_AUPLL_PD_LEN              (1U)
#define PDS_CR_PDS_CTRL_AUPLL_PD_MSK              (((1U << PDS_CR_PDS_CTRL_AUPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_AUPLL_PD_POS)
#define PDS_CR_PDS_CTRL_AUPLL_PD_UMSK             (~(((1U << PDS_CR_PDS_CTRL_AUPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_AUPLL_PD_POS))
#define PDS_CR_PDS_CTRL_WIFIPLL_PD                PDS_CR_PDS_CTRL_WIFIPLL_PD
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_POS            (22U)
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_LEN            (1U)
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_MSK            (((1U << PDS_CR_PDS_CTRL_WIFIPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_WIFIPLL_PD_POS)
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_UMSK           (~(((1U << PDS_CR_PDS_CTRL_WIFIPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_WIFIPLL_PD_POS))
#define PDS_CR_PDS_LDO11_VOL                      PDS_CR_PDS_LDO11_VOL
#define PDS_CR_PDS_LDO11_VOL_POS                  (23U)
#define PDS_CR_PDS_LDO11_VOL_LEN                  (5U)
#define PDS_CR_PDS_LDO11_VOL_MSK                  (((1U << PDS_CR_PDS_LDO11_VOL_LEN) - 1) << PDS_CR_PDS_LDO11_VOL_POS)
#define PDS_CR_PDS_LDO11_VOL_UMSK                 (~(((1U << PDS_CR_PDS_LDO11_VOL_LEN) - 1) << PDS_CR_PDS_LDO11_VOL_POS))
#define PDS_CR_PDS_CTRL_RF                        PDS_CR_PDS_CTRL_RF
#define PDS_CR_PDS_CTRL_RF_POS                    (28U)
#define PDS_CR_PDS_CTRL_RF_LEN                    (2U)
#define PDS_CR_PDS_CTRL_RF_MSK                    (((1U << PDS_CR_PDS_CTRL_RF_LEN) - 1) << PDS_CR_PDS_CTRL_RF_POS)
#define PDS_CR_PDS_CTRL_RF_UMSK                   (~(((1U << PDS_CR_PDS_CTRL_RF_LEN) - 1) << PDS_CR_PDS_CTRL_RF_POS))
#define PDS_CR_PDS_START_USE_TBTT_SLEEP           PDS_CR_PDS_START_USE_TBTT_SLEEP
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_POS       (30U)
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_LEN       (1U)
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_MSK       (((1U << PDS_CR_PDS_START_USE_TBTT_SLEEP_LEN) - 1) << PDS_CR_PDS_START_USE_TBTT_SLEEP_POS)
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_UMSK      (~(((1U << PDS_CR_PDS_START_USE_TBTT_SLEEP_LEN) - 1) << PDS_CR_PDS_START_USE_TBTT_SLEEP_POS))
#define PDS_CR_PDS_GPIO_ISO_MODE                  PDS_CR_PDS_GPIO_ISO_MODE
#define PDS_CR_PDS_GPIO_ISO_MODE_POS              (31U)
#define PDS_CR_PDS_GPIO_ISO_MODE_LEN              (1U)
#define PDS_CR_PDS_GPIO_ISO_MODE_MSK              (((1U << PDS_CR_PDS_GPIO_ISO_MODE_LEN) - 1) << PDS_CR_PDS_GPIO_ISO_MODE_POS)
#define PDS_CR_PDS_GPIO_ISO_MODE_UMSK             (~(((1U << PDS_CR_PDS_GPIO_ISO_MODE_LEN) - 1) << PDS_CR_PDS_GPIO_ISO_MODE_POS))

/* 0x4 : PDS_TIME1 */
#define PDS_TIME1_OFFSET                          (0x4)
#define PDS_CR_SLEEP_DURATION                     PDS_CR_SLEEP_DURATION
#define PDS_CR_SLEEP_DURATION_POS                 (0U)
#define PDS_CR_SLEEP_DURATION_LEN                 (32U)
#define PDS_CR_SLEEP_DURATION_MSK                 (((1U << PDS_CR_SLEEP_DURATION_LEN) - 1) << PDS_CR_SLEEP_DURATION_POS)
#define PDS_CR_SLEEP_DURATION_UMSK                (~(((1U << PDS_CR_SLEEP_DURATION_LEN) - 1) << PDS_CR_SLEEP_DURATION_POS))

/* 0xC : PDS_INT */
#define PDS_INT_OFFSET                            (0xC)
#define PDS_RO_PDS_WAKE_INT                       PDS_RO_PDS_WAKE_INT
#define PDS_RO_PDS_WAKE_INT_POS                   (0U)
#define PDS_RO_PDS_WAKE_INT_LEN                   (1U)
#define PDS_RO_PDS_WAKE_INT_MSK                   (((1U << PDS_RO_PDS_WAKE_INT_LEN) - 1) << PDS_RO_PDS_WAKE_INT_POS)
#define PDS_RO_PDS_WAKE_INT_UMSK                  (~(((1U << PDS_RO_PDS_WAKE_INT_LEN) - 1) << PDS_RO_PDS_WAKE_INT_POS))
#define PDS_RO_PDS_RF_DONE_INT                    PDS_RO_PDS_RF_DONE_INT
#define PDS_RO_PDS_RF_DONE_INT_POS                (1U)
#define PDS_RO_PDS_RF_DONE_INT_LEN                (1U)
#define PDS_RO_PDS_RF_DONE_INT_MSK                (((1U << PDS_RO_PDS_RF_DONE_INT_LEN) - 1) << PDS_RO_PDS_RF_DONE_INT_POS)
#define PDS_RO_PDS_RF_DONE_INT_UMSK               (~(((1U << PDS_RO_PDS_RF_DONE_INT_LEN) - 1) << PDS_RO_PDS_RF_DONE_INT_POS))
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ            PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_POS        (2U)
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_LEN        (1U)
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_MSK        (((1U << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_POS)
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_UMSK       (~(((1U << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_POS))
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ           PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_POS       (3U)
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_LEN       (1U)
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_MSK       (((1U << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_POS)
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_UMSK      (~(((1U << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_POS))
#define PDS_CR_PDS_WAKE_INT_MASK                  PDS_CR_PDS_WAKE_INT_MASK
#define PDS_CR_PDS_WAKE_INT_MASK_POS              (4U)
#define PDS_CR_PDS_WAKE_INT_MASK_LEN              (1U)
#define PDS_CR_PDS_WAKE_INT_MASK_MSK              (((1U << PDS_CR_PDS_WAKE_INT_MASK_LEN) - 1) << PDS_CR_PDS_WAKE_INT_MASK_POS)
#define PDS_CR_PDS_WAKE_INT_MASK_UMSK             (~(((1U << PDS_CR_PDS_WAKE_INT_MASK_LEN) - 1) << PDS_CR_PDS_WAKE_INT_MASK_POS))
#define PDS_CR_PDS_RF_DONE_INT_MASK               PDS_CR_PDS_RF_DONE_INT_MASK
#define PDS_CR_PDS_RF_DONE_INT_MASK_POS           (5U)
#define PDS_CR_PDS_RF_DONE_INT_MASK_LEN           (1U)
#define PDS_CR_PDS_RF_DONE_INT_MASK_MSK           (((1U << PDS_CR_PDS_RF_DONE_INT_MASK_LEN) - 1) << PDS_CR_PDS_RF_DONE_INT_MASK_POS)
#define PDS_CR_PDS_RF_DONE_INT_MASK_UMSK          (~(((1U << PDS_CR_PDS_RF_DONE_INT_MASK_LEN) - 1) << PDS_CR_PDS_RF_DONE_INT_MASK_POS))
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK       PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_POS   (6U)
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_LEN   (1U)
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_MSK   (((1U << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_POS)
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_UMSK  (~(((1U << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_POS))
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK      PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_POS  (7U)
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_LEN  (1U)
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_MSK  (((1U << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_POS)
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_UMSK (~(((1U << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_POS))
#define PDS_CR_PDS_INT_CLR                        PDS_CR_PDS_INT_CLR
#define PDS_CR_PDS_INT_CLR_POS                    (8U)
#define PDS_CR_PDS_INT_CLR_LEN                    (1U)
#define PDS_CR_PDS_INT_CLR_MSK                    (((1U << PDS_CR_PDS_INT_CLR_LEN) - 1) << PDS_CR_PDS_INT_CLR_POS)
#define PDS_CR_PDS_INT_CLR_UMSK                   (~(((1U << PDS_CR_PDS_INT_CLR_LEN) - 1) << PDS_CR_PDS_INT_CLR_POS))
#define PDS_CR_PDS_WAKEUP_SRC_EN                  PDS_CR_PDS_WAKEUP_SRC_EN
#define PDS_CR_PDS_WAKEUP_SRC_EN_POS              (10U)
#define PDS_CR_PDS_WAKEUP_SRC_EN_LEN              (11U)
#define PDS_CR_PDS_WAKEUP_SRC_EN_MSK              (((1U << PDS_CR_PDS_WAKEUP_SRC_EN_LEN) - 1) << PDS_CR_PDS_WAKEUP_SRC_EN_POS)
#define PDS_CR_PDS_WAKEUP_SRC_EN_UMSK             (~(((1U << PDS_CR_PDS_WAKEUP_SRC_EN_LEN) - 1) << PDS_CR_PDS_WAKEUP_SRC_EN_POS))
#define PDS_RO_PDS_WAKEUP_EVENT                   PDS_RO_PDS_WAKEUP_EVENT
#define PDS_RO_PDS_WAKEUP_EVENT_POS               (21U)
#define PDS_RO_PDS_WAKEUP_EVENT_LEN               (11U)
#define PDS_RO_PDS_WAKEUP_EVENT_MSK               (((1U << PDS_RO_PDS_WAKEUP_EVENT_LEN) - 1) << PDS_RO_PDS_WAKEUP_EVENT_POS)
#define PDS_RO_PDS_WAKEUP_EVENT_UMSK              (~(((1U << PDS_RO_PDS_WAKEUP_EVENT_LEN) - 1) << PDS_RO_PDS_WAKEUP_EVENT_POS))

/* 0x10 : PDS_CTL2 */
#define PDS_CTL2_OFFSET                           (0x10)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF               PDS_CR_PDS_FORCE_NP_PWR_OFF
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_POS           (0U)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_LEN           (1U)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_MSK           (((1U << PDS_CR_PDS_FORCE_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_UMSK          (~(((1U << PDS_CR_PDS_FORCE_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_WB_PWR_OFF               PDS_CR_PDS_FORCE_WB_PWR_OFF
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_POS           (2U)
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_LEN           (1U)
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_MSK           (((1U << PDS_CR_PDS_FORCE_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_UMSK          (~(((1U << PDS_CR_PDS_FORCE_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_USB_PWR_OFF              PDS_CR_PDS_FORCE_USB_PWR_OFF
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_POS          (3U)
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_LEN          (1U)
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_MSK          (((1U << PDS_CR_PDS_FORCE_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_UMSK         (~(((1U << PDS_CR_PDS_FORCE_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_NP_ISO_EN                PDS_CR_PDS_FORCE_NP_ISO_EN
#define PDS_CR_PDS_FORCE_NP_ISO_EN_POS            (4U)
#define PDS_CR_PDS_FORCE_NP_ISO_EN_LEN            (1U)
#define PDS_CR_PDS_FORCE_NP_ISO_EN_MSK            (((1U << PDS_CR_PDS_FORCE_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_NP_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_NP_ISO_EN_UMSK           (~(((1U << PDS_CR_PDS_FORCE_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_NP_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_WB_ISO_EN                PDS_CR_PDS_FORCE_WB_ISO_EN
#define PDS_CR_PDS_FORCE_WB_ISO_EN_POS            (6U)
#define PDS_CR_PDS_FORCE_WB_ISO_EN_LEN            (1U)
#define PDS_CR_PDS_FORCE_WB_ISO_EN_MSK            (((1U << PDS_CR_PDS_FORCE_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_WB_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_WB_ISO_EN_UMSK           (~(((1U << PDS_CR_PDS_FORCE_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_WB_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_USB_ISO_EN               PDS_CR_PDS_FORCE_USB_ISO_EN
#define PDS_CR_PDS_FORCE_USB_ISO_EN_POS           (7U)
#define PDS_CR_PDS_FORCE_USB_ISO_EN_LEN           (1U)
#define PDS_CR_PDS_FORCE_USB_ISO_EN_MSK           (((1U << PDS_CR_PDS_FORCE_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_USB_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_USB_ISO_EN_UMSK          (~(((1U << PDS_CR_PDS_FORCE_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_USB_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_NP_PDS_RST               PDS_CR_PDS_FORCE_NP_PDS_RST
#define PDS_CR_PDS_FORCE_NP_PDS_RST_POS           (8U)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_LEN           (1U)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_MSK           (((1U << PDS_CR_PDS_FORCE_NP_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_UMSK          (~(((1U << PDS_CR_PDS_FORCE_NP_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_WB_PDS_RST               PDS_CR_PDS_FORCE_WB_PDS_RST
#define PDS_CR_PDS_FORCE_WB_PDS_RST_POS           (10U)
#define PDS_CR_PDS_FORCE_WB_PDS_RST_LEN           (1U)
#define PDS_CR_PDS_FORCE_WB_PDS_RST_MSK           (((1U << PDS_CR_PDS_FORCE_WB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_WB_PDS_RST_UMSK          (~(((1U << PDS_CR_PDS_FORCE_WB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_USB_PDS_RST              PDS_CR_PDS_FORCE_USB_PDS_RST
#define PDS_CR_PDS_FORCE_USB_PDS_RST_POS          (11U)
#define PDS_CR_PDS_FORCE_USB_PDS_RST_LEN          (1U)
#define PDS_CR_PDS_FORCE_USB_PDS_RST_MSK          (((1U << PDS_CR_PDS_FORCE_USB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_USB_PDS_RST_UMSK         (~(((1U << PDS_CR_PDS_FORCE_USB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_NP_MEM_STBY              PDS_CR_PDS_FORCE_NP_MEM_STBY
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_POS          (12U)
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_LEN          (1U)
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_MSK          (((1U << PDS_CR_PDS_FORCE_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_NP_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_UMSK         (~(((1U << PDS_CR_PDS_FORCE_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_NP_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_WB_MEM_STBY              PDS_CR_PDS_FORCE_WB_MEM_STBY
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_POS          (14U)
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_LEN          (1U)
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_MSK          (((1U << PDS_CR_PDS_FORCE_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_WB_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_UMSK         (~(((1U << PDS_CR_PDS_FORCE_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_WB_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_USB_MEM_STBY             PDS_CR_PDS_FORCE_USB_MEM_STBY
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_POS         (15U)
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_LEN         (1U)
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_MSK         (((1U << PDS_CR_PDS_FORCE_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_USB_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_UMSK        (~(((1U << PDS_CR_PDS_FORCE_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_USB_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_NP_GATE_CLK              PDS_CR_PDS_FORCE_NP_GATE_CLK
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_POS          (16U)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN          (1U)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_MSK          (((1U << PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_NP_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_UMSK         (~(((1U << PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_NP_GATE_CLK_POS))
#define PDS_CR_PDS_FORCE_WB_GATE_CLK              PDS_CR_PDS_FORCE_WB_GATE_CLK
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_POS          (18U)
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_LEN          (1U)
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_MSK          (((1U << PDS_CR_PDS_FORCE_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_WB_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_UMSK         (~(((1U << PDS_CR_PDS_FORCE_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_WB_GATE_CLK_POS))
#define PDS_CR_PDS_FORCE_USB_GATE_CLK             PDS_CR_PDS_FORCE_USB_GATE_CLK
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_POS         (19U)
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_LEN         (1U)
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_MSK         (((1U << PDS_CR_PDS_FORCE_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_USB_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_UMSK        (~(((1U << PDS_CR_PDS_FORCE_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_USB_GATE_CLK_POS))

/* 0x14 : PDS_CTL3 */
#define PDS_CTL3_OFFSET                           (0x14)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF             PDS_CR_PDS_FORCE_MISC_PWR_OFF
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS         (1U)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN         (1U)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_MSK         (((1U << PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_UMSK        (~(((1U << PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_MISC_ISO_EN              PDS_CR_PDS_FORCE_MISC_ISO_EN
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_POS          (4U)
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_LEN          (1U)
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_MSK          (((1U << PDS_CR_PDS_FORCE_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_UMSK         (~(((1U << PDS_CR_PDS_FORCE_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_MISC_PDS_RST             PDS_CR_PDS_FORCE_MISC_PDS_RST
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_POS         (7U)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN         (1U)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_MSK         (((1U << PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_UMSK        (~(((1U << PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY            PDS_CR_PDS_FORCE_MISC_MEM_STBY
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_POS        (10U)
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_LEN        (1U)
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_MSK        (((1U << PDS_CR_PDS_FORCE_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_UMSK       (~(((1U << PDS_CR_PDS_FORCE_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK            PDS_CR_PDS_FORCE_MISC_GATE_CLK
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS        (13U)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN        (1U)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_MSK        (((1U << PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_UMSK       (~(((1U << PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS))
#define PDS_CR_PDS_NP_ISO_EN                      PDS_CR_PDS_NP_ISO_EN
#define PDS_CR_PDS_NP_ISO_EN_POS                  (24U)
#define PDS_CR_PDS_NP_ISO_EN_LEN                  (1U)
#define PDS_CR_PDS_NP_ISO_EN_MSK                  (((1U << PDS_CR_PDS_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_NP_ISO_EN_POS)
#define PDS_CR_PDS_NP_ISO_EN_UMSK                 (~(((1U << PDS_CR_PDS_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_NP_ISO_EN_POS))
#define PDS_CR_PDS_WB_ISO_EN                      PDS_CR_PDS_WB_ISO_EN
#define PDS_CR_PDS_WB_ISO_EN_POS                  (27U)
#define PDS_CR_PDS_WB_ISO_EN_LEN                  (1U)
#define PDS_CR_PDS_WB_ISO_EN_MSK                  (((1U << PDS_CR_PDS_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_WB_ISO_EN_POS)
#define PDS_CR_PDS_WB_ISO_EN_UMSK                 (~(((1U << PDS_CR_PDS_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_WB_ISO_EN_POS))
#define PDS_CR_PDS_USB_ISO_EN                     PDS_CR_PDS_USB_ISO_EN
#define PDS_CR_PDS_USB_ISO_EN_POS                 (29U)
#define PDS_CR_PDS_USB_ISO_EN_LEN                 (1U)
#define PDS_CR_PDS_USB_ISO_EN_MSK                 (((1U << PDS_CR_PDS_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_USB_ISO_EN_POS)
#define PDS_CR_PDS_USB_ISO_EN_UMSK                (~(((1U << PDS_CR_PDS_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_USB_ISO_EN_POS))
#define PDS_CR_PDS_MISC_ISO_EN                    PDS_CR_PDS_MISC_ISO_EN
#define PDS_CR_PDS_MISC_ISO_EN_POS                (30U)
#define PDS_CR_PDS_MISC_ISO_EN_LEN                (1U)
#define PDS_CR_PDS_MISC_ISO_EN_MSK                (((1U << PDS_CR_PDS_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_MISC_ISO_EN_POS)
#define PDS_CR_PDS_MISC_ISO_EN_UMSK               (~(((1U << PDS_CR_PDS_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_MISC_ISO_EN_POS))

/* 0x18 : PDS_CTL4 */
#define PDS_CTL4_OFFSET                           (0x18)
#define PDS_CR_PDS_NP_PWR_OFF                     PDS_CR_PDS_NP_PWR_OFF
#define PDS_CR_PDS_NP_PWR_OFF_POS                 (0U)
#define PDS_CR_PDS_NP_PWR_OFF_LEN                 (1U)
#define PDS_CR_PDS_NP_PWR_OFF_MSK                 (((1U << PDS_CR_PDS_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_NP_PWR_OFF_POS)
#define PDS_CR_PDS_NP_PWR_OFF_UMSK                (~(((1U << PDS_CR_PDS_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_NP_PWR_OFF_POS))
#define PDS_CR_PDS_NP_RESET                       PDS_CR_PDS_NP_RESET
#define PDS_CR_PDS_NP_RESET_POS                   (1U)
#define PDS_CR_PDS_NP_RESET_LEN                   (1U)
#define PDS_CR_PDS_NP_RESET_MSK                   (((1U << PDS_CR_PDS_NP_RESET_LEN) - 1) << PDS_CR_PDS_NP_RESET_POS)
#define PDS_CR_PDS_NP_RESET_UMSK                  (~(((1U << PDS_CR_PDS_NP_RESET_LEN) - 1) << PDS_CR_PDS_NP_RESET_POS))
#define PDS_CR_PDS_NP_MEM_STBY                    PDS_CR_PDS_NP_MEM_STBY
#define PDS_CR_PDS_NP_MEM_STBY_POS                (2U)
#define PDS_CR_PDS_NP_MEM_STBY_LEN                (1U)
#define PDS_CR_PDS_NP_MEM_STBY_MSK                (((1U << PDS_CR_PDS_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_NP_MEM_STBY_POS)
#define PDS_CR_PDS_NP_MEM_STBY_UMSK               (~(((1U << PDS_CR_PDS_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_NP_MEM_STBY_POS))
#define PDS_CR_PDS_NP_GATE_CLK                    PDS_CR_PDS_NP_GATE_CLK
#define PDS_CR_PDS_NP_GATE_CLK_POS                (3U)
#define PDS_CR_PDS_NP_GATE_CLK_LEN                (1U)
#define PDS_CR_PDS_NP_GATE_CLK_MSK                (((1U << PDS_CR_PDS_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_NP_GATE_CLK_POS)
#define PDS_CR_PDS_NP_GATE_CLK_UMSK               (~(((1U << PDS_CR_PDS_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_NP_GATE_CLK_POS))
#define PDS_CR_PDS_WB_PWR_OFF                     PDS_CR_PDS_WB_PWR_OFF
#define PDS_CR_PDS_WB_PWR_OFF_POS                 (12U)
#define PDS_CR_PDS_WB_PWR_OFF_LEN                 (1U)
#define PDS_CR_PDS_WB_PWR_OFF_MSK                 (((1U << PDS_CR_PDS_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_WB_PWR_OFF_POS)
#define PDS_CR_PDS_WB_PWR_OFF_UMSK                (~(((1U << PDS_CR_PDS_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_WB_PWR_OFF_POS))
#define PDS_CR_PDS_WB_RESET                       PDS_CR_PDS_WB_RESET
#define PDS_CR_PDS_WB_RESET_POS                   (13U)
#define PDS_CR_PDS_WB_RESET_LEN                   (1U)
#define PDS_CR_PDS_WB_RESET_MSK                   (((1U << PDS_CR_PDS_WB_RESET_LEN) - 1) << PDS_CR_PDS_WB_RESET_POS)
#define PDS_CR_PDS_WB_RESET_UMSK                  (~(((1U << PDS_CR_PDS_WB_RESET_LEN) - 1) << PDS_CR_PDS_WB_RESET_POS))
#define PDS_CR_PDS_WB_MEM_STBY                    PDS_CR_PDS_WB_MEM_STBY
#define PDS_CR_PDS_WB_MEM_STBY_POS                (14U)
#define PDS_CR_PDS_WB_MEM_STBY_LEN                (1U)
#define PDS_CR_PDS_WB_MEM_STBY_MSK                (((1U << PDS_CR_PDS_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_WB_MEM_STBY_POS)
#define PDS_CR_PDS_WB_MEM_STBY_UMSK               (~(((1U << PDS_CR_PDS_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_WB_MEM_STBY_POS))
#define PDS_CR_PDS_WB_GATE_CLK                    PDS_CR_PDS_WB_GATE_CLK
#define PDS_CR_PDS_WB_GATE_CLK_POS                (15U)
#define PDS_CR_PDS_WB_GATE_CLK_LEN                (1U)
#define PDS_CR_PDS_WB_GATE_CLK_MSK                (((1U << PDS_CR_PDS_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_WB_GATE_CLK_POS)
#define PDS_CR_PDS_WB_GATE_CLK_UMSK               (~(((1U << PDS_CR_PDS_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_WB_GATE_CLK_POS))
#define PDS_CR_PDS_USB_PWR_OFF                    PDS_CR_PDS_USB_PWR_OFF
#define PDS_CR_PDS_USB_PWR_OFF_POS                (20U)
#define PDS_CR_PDS_USB_PWR_OFF_LEN                (1U)
#define PDS_CR_PDS_USB_PWR_OFF_MSK                (((1U << PDS_CR_PDS_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_USB_PWR_OFF_POS)
#define PDS_CR_PDS_USB_PWR_OFF_UMSK               (~(((1U << PDS_CR_PDS_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_USB_PWR_OFF_POS))
#define PDS_CR_PDS_USB_RESET                      PDS_CR_PDS_USB_RESET
#define PDS_CR_PDS_USB_RESET_POS                  (21U)
#define PDS_CR_PDS_USB_RESET_LEN                  (1U)
#define PDS_CR_PDS_USB_RESET_MSK                  (((1U << PDS_CR_PDS_USB_RESET_LEN) - 1) << PDS_CR_PDS_USB_RESET_POS)
#define PDS_CR_PDS_USB_RESET_UMSK                 (~(((1U << PDS_CR_PDS_USB_RESET_LEN) - 1) << PDS_CR_PDS_USB_RESET_POS))
#define PDS_CR_PDS_USB_MEM_STBY                   PDS_CR_PDS_USB_MEM_STBY
#define PDS_CR_PDS_USB_MEM_STBY_POS               (22U)
#define PDS_CR_PDS_USB_MEM_STBY_LEN               (1U)
#define PDS_CR_PDS_USB_MEM_STBY_MSK               (((1U << PDS_CR_PDS_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_USB_MEM_STBY_POS)
#define PDS_CR_PDS_USB_MEM_STBY_UMSK              (~(((1U << PDS_CR_PDS_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_USB_MEM_STBY_POS))
#define PDS_CR_PDS_USB_GATE_CLK                   PDS_CR_PDS_USB_GATE_CLK
#define PDS_CR_PDS_USB_GATE_CLK_POS               (23U)
#define PDS_CR_PDS_USB_GATE_CLK_LEN               (1U)
#define PDS_CR_PDS_USB_GATE_CLK_MSK               (((1U << PDS_CR_PDS_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_USB_GATE_CLK_POS)
#define PDS_CR_PDS_USB_GATE_CLK_UMSK              (~(((1U << PDS_CR_PDS_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_USB_GATE_CLK_POS))
#define PDS_CR_PDS_MISC_PWR_OFF                   PDS_CR_PDS_MISC_PWR_OFF
#define PDS_CR_PDS_MISC_PWR_OFF_POS               (24U)
#define PDS_CR_PDS_MISC_PWR_OFF_LEN               (1U)
#define PDS_CR_PDS_MISC_PWR_OFF_MSK               (((1U << PDS_CR_PDS_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_MISC_PWR_OFF_POS)
#define PDS_CR_PDS_MISC_PWR_OFF_UMSK              (~(((1U << PDS_CR_PDS_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_MISC_PWR_OFF_POS))
#define PDS_CR_PDS_MISC_RESET                     PDS_CR_PDS_MISC_RESET
#define PDS_CR_PDS_MISC_RESET_POS                 (25U)
#define PDS_CR_PDS_MISC_RESET_LEN                 (1U)
#define PDS_CR_PDS_MISC_RESET_MSK                 (((1U << PDS_CR_PDS_MISC_RESET_LEN) - 1) << PDS_CR_PDS_MISC_RESET_POS)
#define PDS_CR_PDS_MISC_RESET_UMSK                (~(((1U << PDS_CR_PDS_MISC_RESET_LEN) - 1) << PDS_CR_PDS_MISC_RESET_POS))
#define PDS_CR_PDS_MISC_MEM_STBY                  PDS_CR_PDS_MISC_MEM_STBY
#define PDS_CR_PDS_MISC_MEM_STBY_POS              (26U)
#define PDS_CR_PDS_MISC_MEM_STBY_LEN              (1U)
#define PDS_CR_PDS_MISC_MEM_STBY_MSK              (((1U << PDS_CR_PDS_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MISC_MEM_STBY_POS)
#define PDS_CR_PDS_MISC_MEM_STBY_UMSK             (~(((1U << PDS_CR_PDS_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MISC_MEM_STBY_POS))
#define PDS_CR_PDS_MISC_GATE_CLK                  PDS_CR_PDS_MISC_GATE_CLK
#define PDS_CR_PDS_MISC_GATE_CLK_POS              (27U)
#define PDS_CR_PDS_MISC_GATE_CLK_LEN              (1U)
#define PDS_CR_PDS_MISC_GATE_CLK_MSK              (((1U << PDS_CR_PDS_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_MISC_GATE_CLK_POS)
#define PDS_CR_PDS_MISC_GATE_CLK_UMSK             (~(((1U << PDS_CR_PDS_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_MISC_GATE_CLK_POS))

/* 0x1C : pds_stat */
#define PDS_STAT_OFFSET                           (0x1C)
#define PDS_RO_PDS_STATE                          PDS_RO_PDS_STATE
#define PDS_RO_PDS_STATE_POS                      (0U)
#define PDS_RO_PDS_STATE_LEN                      (5U)
#define PDS_RO_PDS_STATE_MSK                      (((1U << PDS_RO_PDS_STATE_LEN) - 1) << PDS_RO_PDS_STATE_POS)
#define PDS_RO_PDS_STATE_UMSK                     (~(((1U << PDS_RO_PDS_STATE_LEN) - 1) << PDS_RO_PDS_STATE_POS))
#define PDS_RO_PDS_RF_STATE                       PDS_RO_PDS_RF_STATE
#define PDS_RO_PDS_RF_STATE_POS                   (8U)
#define PDS_RO_PDS_RF_STATE_LEN                   (5U)
#define PDS_RO_PDS_RF_STATE_MSK                   (((1U << PDS_RO_PDS_RF_STATE_LEN) - 1) << PDS_RO_PDS_RF_STATE_POS)
#define PDS_RO_PDS_RF_STATE_UMSK                  (~(((1U << PDS_RO_PDS_RF_STATE_LEN) - 1) << PDS_RO_PDS_RF_STATE_POS))
#define PDS_RESET_EVENT                           PDS_RESET_EVENT
#define PDS_RESET_EVENT_POS                       (24U)
#define PDS_RESET_EVENT_LEN                       (3U)
#define PDS_RESET_EVENT_MSK                       (((1U << PDS_RESET_EVENT_LEN) - 1) << PDS_RESET_EVENT_POS)
#define PDS_RESET_EVENT_UMSK                      (~(((1U << PDS_RESET_EVENT_LEN) - 1) << PDS_RESET_EVENT_POS))
#define PDS_CLR_RESET_EVENT                       PDS_CLR_RESET_EVENT
#define PDS_CLR_RESET_EVENT_POS                   (31U)
#define PDS_CLR_RESET_EVENT_LEN                   (1U)
#define PDS_CLR_RESET_EVENT_MSK                   (((1U << PDS_CLR_RESET_EVENT_LEN) - 1) << PDS_CLR_RESET_EVENT_POS)
#define PDS_CLR_RESET_EVENT_UMSK                  (~(((1U << PDS_CLR_RESET_EVENT_LEN) - 1) << PDS_CLR_RESET_EVENT_POS))

/* 0x20 : pds_ram1 */
#define PDS_RAM1_OFFSET                           (0x20)
#define PDS_CR_PDS_RAM_CLK_CNT                    PDS_CR_PDS_RAM_CLK_CNT
#define PDS_CR_PDS_RAM_CLK_CNT_POS                (8U)
#define PDS_CR_PDS_RAM_CLK_CNT_LEN                (6U)
#define PDS_CR_PDS_RAM_CLK_CNT_MSK                (((1U << PDS_CR_PDS_RAM_CLK_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK_CNT_POS)
#define PDS_CR_PDS_RAM_CLK_CNT_UMSK               (~(((1U << PDS_CR_PDS_RAM_CLK_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK_CNT_POS))
#define PDS_CR_PDS_RAM_CLK2_CNT                   PDS_CR_PDS_RAM_CLK2_CNT
#define PDS_CR_PDS_RAM_CLK2_CNT_POS               (16U)
#define PDS_CR_PDS_RAM_CLK2_CNT_LEN               (6U)
#define PDS_CR_PDS_RAM_CLK2_CNT_MSK               (((1U << PDS_CR_PDS_RAM_CLK2_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK2_CNT_POS)
#define PDS_CR_PDS_RAM_CLK2_CNT_UMSK              (~(((1U << PDS_CR_PDS_RAM_CLK2_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK2_CNT_POS))
#define PDS_CR_PDS_CTRL_NP_RAM_CLK                PDS_CR_PDS_CTRL_NP_RAM_CLK
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_POS            (24U)
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_LEN            (1U)
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_MSK            (((1U << PDS_CR_PDS_CTRL_NP_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_NP_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_UMSK           (~(((1U << PDS_CR_PDS_CTRL_NP_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_NP_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_WB_RAM_CLK                PDS_CR_PDS_CTRL_WB_RAM_CLK
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_POS            (26U)
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_LEN            (1U)
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_MSK            (((1U << PDS_CR_PDS_CTRL_WB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_WB_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_UMSK           (~(((1U << PDS_CR_PDS_CTRL_WB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_WB_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_USB_RAM_CLK               PDS_CR_PDS_CTRL_USB_RAM_CLK
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_POS           (27U)
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_LEN           (1U)
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_MSK           (((1U << PDS_CR_PDS_CTRL_USB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_USB_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_UMSK          (~(((1U << PDS_CR_PDS_CTRL_USB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_USB_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK              PDS_CR_PDS_CTRL_MISC_RAM_CLK
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_POS          (28U)
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_LEN          (1U)
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_MSK          (((1U << PDS_CR_PDS_CTRL_MISC_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_MISC_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_UMSK         (~(((1U << PDS_CR_PDS_CTRL_MISC_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_MISC_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_RAM_CLK2                  PDS_CR_PDS_CTRL_RAM_CLK2
#define PDS_CR_PDS_CTRL_RAM_CLK2_POS              (30U)
#define PDS_CR_PDS_CTRL_RAM_CLK2_LEN              (1U)
#define PDS_CR_PDS_CTRL_RAM_CLK2_MSK              (((1U << PDS_CR_PDS_CTRL_RAM_CLK2_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK2_POS)
#define PDS_CR_PDS_CTRL_RAM_CLK2_UMSK             (~(((1U << PDS_CR_PDS_CTRL_RAM_CLK2_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK2_POS))
#define PDS_CR_PDS_CTRL_RAM_CLK                   PDS_CR_PDS_CTRL_RAM_CLK
#define PDS_CR_PDS_CTRL_RAM_CLK_POS               (31U)
#define PDS_CR_PDS_CTRL_RAM_CLK_LEN               (1U)
#define PDS_CR_PDS_CTRL_RAM_CLK_MSK               (((1U << PDS_CR_PDS_CTRL_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_RAM_CLK_UMSK              (~(((1U << PDS_CR_PDS_CTRL_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK_POS))

/* 0x24 : PDS_CTL5 */
#define PDS_CTL5_OFFSET                           (0x24)
#define PDS_CR_MAIN_MCU_WFI_MASK                  PDS_CR_MAIN_MCU_WFI_MASK
#define PDS_CR_MAIN_MCU_WFI_MASK_POS              (0U)
#define PDS_CR_MAIN_MCU_WFI_MASK_LEN              (1U)
#define PDS_CR_MAIN_MCU_WFI_MASK_MSK              (((1U << PDS_CR_MAIN_MCU_WFI_MASK_LEN) - 1) << PDS_CR_MAIN_MCU_WFI_MASK_POS)
#define PDS_CR_MAIN_MCU_WFI_MASK_UMSK             (~(((1U << PDS_CR_MAIN_MCU_WFI_MASK_LEN) - 1) << PDS_CR_MAIN_MCU_WFI_MASK_POS))
#define PDS_CR_PDS_PAD_OD_EN                      PDS_CR_PDS_PAD_OD_EN
#define PDS_CR_PDS_PAD_OD_EN_POS                  (1U)
#define PDS_CR_PDS_PAD_OD_EN_LEN                  (1U)
#define PDS_CR_PDS_PAD_OD_EN_MSK                  (((1U << PDS_CR_PDS_PAD_OD_EN_LEN) - 1) << PDS_CR_PDS_PAD_OD_EN_POS)
#define PDS_CR_PDS_PAD_OD_EN_UMSK                 (~(((1U << PDS_CR_PDS_PAD_OD_EN_LEN) - 1) << PDS_CR_PDS_PAD_OD_EN_POS))
#define PDS_CR_WL_MCU_WFI_MASK                    PDS_CR_WL_MCU_WFI_MASK
#define PDS_CR_WL_MCU_WFI_MASK_POS                (2U)
#define PDS_CR_WL_MCU_WFI_MASK_LEN                (1U)
#define PDS_CR_WL_MCU_WFI_MASK_MSK                (((1U << PDS_CR_WL_MCU_WFI_MASK_LEN) - 1) << PDS_CR_WL_MCU_WFI_MASK_POS)
#define PDS_CR_LPCPU_WFI_MASK                     PDS_CR_LPCPU_WFI_MASK
#define PDS_CR_LPCPU_WFI_MASK_POS                 (4U)
#define PDS_CR_LPCPU_WFI_MASK_LEN                 (1U)
#define PDS_CR_LPCPU_WFI_MASK_MSK                 (((1U << PDS_CR_LPCPU_WFI_MASK_LEN) - 1) << PDS_CR_LPCPU_WFI_MASK_POS)
#define PDS_CR_LPCPU_WFI_MASK_UMSK                (~(((1U << PDS_CR_LPCPU_WFI_MASK_LEN) - 1) << PDS_CR_LPCPU_WFI_MASK_POS))
#define PDS_CR_WL_MCU_WFI_MASK_UMSK               (~(((1U << PDS_CR_WL_MCU_WFI_MASK_LEN) - 1) << PDS_CR_WL_MCU_WFI_MASK_POS))
#define PDS_CR_PDS_CTRL_USB33                     PDS_CR_PDS_CTRL_USB33
#define PDS_CR_PDS_CTRL_USB33_POS                 (8U)
#define PDS_CR_PDS_CTRL_USB33_LEN                 (1U)
#define PDS_CR_PDS_CTRL_USB33_MSK                 (((1U << PDS_CR_PDS_CTRL_USB33_LEN) - 1) << PDS_CR_PDS_CTRL_USB33_POS)
#define PDS_CR_PDS_CTRL_USB33_UMSK                (~(((1U << PDS_CR_PDS_CTRL_USB33_LEN) - 1) << PDS_CR_PDS_CTRL_USB33_POS))
#define PDS_CR_PDS_PD_LDO18IO                     PDS_CR_PDS_PD_LDO18IO
#define PDS_CR_PDS_PD_LDO18IO_POS                 (9U)
#define PDS_CR_PDS_PD_LDO18IO_LEN                 (1U)
#define PDS_CR_PDS_PD_LDO18IO_MSK                 (((1U << PDS_CR_PDS_PD_LDO18IO_LEN) - 1) << PDS_CR_PDS_PD_LDO18IO_POS)
#define PDS_CR_PDS_PD_LDO18IO_UMSK                (~(((1U << PDS_CR_PDS_PD_LDO18IO_LEN) - 1) << PDS_CR_PDS_PD_LDO18IO_POS))
#define PDS_CR_PDS_GPIO_KEEP_EN                   PDS_CR_PDS_GPIO_KEEP_EN
#define PDS_CR_PDS_GPIO_KEEP_EN_POS               (16U)
#define PDS_CR_PDS_GPIO_KEEP_EN_LEN               (3U)
#define PDS_CR_PDS_GPIO_KEEP_EN_MSK               (((1U << PDS_CR_PDS_GPIO_KEEP_EN_LEN) - 1) << PDS_CR_PDS_GPIO_KEEP_EN_POS)
#define PDS_CR_PDS_GPIO_KEEP_EN_UMSK              (~(((1U << PDS_CR_PDS_GPIO_KEEP_EN_LEN) - 1) << PDS_CR_PDS_GPIO_KEEP_EN_POS))
#define PDS_CR_CORE_GPIO_LS1P8                    PDS_CR_CORE_GPIO_LS1P8
#define PDS_CR_CORE_GPIO_LS1P8_POS                (19U)
#define PDS_CR_CORE_GPIO_LS1P8_LEN                (3U)
#define PDS_CR_CORE_GPIO_LS1P8_MSK                (((1U << PDS_CR_CORE_GPIO_LS1P8_LEN) - 1) << PDS_CR_CORE_GPIO_LS1P8_POS)
#define PDS_CR_CORE_GPIO_LS1P8_UMSK               (~(((1U << PDS_CR_CORE_GPIO_LS1P8_LEN) - 1) << PDS_CR_CORE_GPIO_LS1P8_POS))

/* 0x28 : PDS_RAM2 */
#define PDS_RAM2_OFFSET                           (0x28)
#define PDS_CR_WRAM_SLP                           PDS_CR_WRAM_SLP
#define PDS_CR_WRAM_SLP_POS                       (0U)
#define PDS_CR_WRAM_SLP_LEN                       (10U)
#define PDS_CR_WRAM_SLP_MSK                       (((1U << PDS_CR_WRAM_SLP_LEN) - 1) << PDS_CR_WRAM_SLP_POS)
#define PDS_CR_WRAM_SLP_UMSK                      (~(((1U << PDS_CR_WRAM_SLP_LEN) - 1) << PDS_CR_WRAM_SLP_POS))
#define PDS_CR_WRAM_RET                           PDS_CR_WRAM_RET
#define PDS_CR_WRAM_RET_POS                       (10U)
#define PDS_CR_WRAM_RET_LEN                       (10U)
#define PDS_CR_WRAM_RET_MSK                       (((1U << PDS_CR_WRAM_RET_LEN) - 1) << PDS_CR_WRAM_RET_POS)
#define PDS_CR_WRAM_RET_UMSK                      (~(((1U << PDS_CR_WRAM_RET_LEN) - 1) << PDS_CR_WRAM_RET_POS))

// /* 0x30 : pds_gpio_i_set */
// #define PDS_GPIO_I_SET_OFFSET                     (0x30)
// #define PDS_CR_PDS_GPIO_IE_SET                    PDS_CR_PDS_GPIO_IE_SET
// #define PDS_CR_PDS_GPIO_IE_SET_POS                (0U)
// #define PDS_CR_PDS_GPIO_IE_SET_LEN                (2U)
// #define PDS_CR_PDS_GPIO_IE_SET_MSK                (((1U << PDS_CR_PDS_GPIO_IE_SET_LEN) - 1) << PDS_CR_PDS_GPIO_IE_SET_POS)
// #define PDS_CR_PDS_GPIO_IE_SET_UMSK               (~(((1U << PDS_CR_PDS_GPIO_IE_SET_LEN) - 1) << PDS_CR_PDS_GPIO_IE_SET_POS))
// #define PDS_CR_PDS_GPIO_PD_SET                    PDS_CR_PDS_GPIO_PD_SET
// #define PDS_CR_PDS_GPIO_PD_SET_POS                (3U)
// #define PDS_CR_PDS_GPIO_PD_SET_LEN                (2U)
// #define PDS_CR_PDS_GPIO_PD_SET_MSK                (((1U << PDS_CR_PDS_GPIO_PD_SET_LEN) - 1) << PDS_CR_PDS_GPIO_PD_SET_POS)
// #define PDS_CR_PDS_GPIO_PD_SET_UMSK               (~(((1U << PDS_CR_PDS_GPIO_PD_SET_LEN) - 1) << PDS_CR_PDS_GPIO_PD_SET_POS))
// #define PDS_CR_PDS_GPIO_PU_SET                    PDS_CR_PDS_GPIO_PU_SET
// #define PDS_CR_PDS_GPIO_PU_SET_POS                (6U)
// #define PDS_CR_PDS_GPIO_PU_SET_LEN                (2U)
// #define PDS_CR_PDS_GPIO_PU_SET_MSK                (((1U << PDS_CR_PDS_GPIO_PU_SET_LEN) - 1) << PDS_CR_PDS_GPIO_PU_SET_POS)
// #define PDS_CR_PDS_GPIO_PU_SET_UMSK               (~(((1U << PDS_CR_PDS_GPIO_PU_SET_LEN) - 1) << PDS_CR_PDS_GPIO_PU_SET_POS))

/* 0x34 : pds_gpio_pd_set */
#define PDS_GPIO_PD_SET_OFFSET                    (0x34)
#define PDS_CR_PDS_GPIO_SET_INT_MASK1             PDS_CR_PDS_GPIO_SET_INT_MASK1
#define PDS_CR_PDS_GPIO_SET_INT_MASK1_POS         (0U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK1_LEN         (32U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK1_MSK         (((1U << PDS_CR_PDS_GPIO_SET_INT_MASK1_LEN) - 1) << PDS_CR_PDS_GPIO_SET_INT_MASK1_POS)
#define PDS_CR_PDS_GPIO_SET_INT_MASK1_UMSK        (~(((1U << PDS_CR_PDS_GPIO_SET_INT_MASK1_LEN) - 1) << PDS_CR_PDS_GPIO_SET_INT_MASK1_POS))

/* 0x40 : pds_gpio_int */
#define PDS_GPIO_INT_OFFSET                       (0x40)
#define PDS_GPIO_SET1_INT_CLR                     PDS_GPIO_SET1_INT_CLR
#define PDS_GPIO_SET1_INT_CLR_POS                 (2U)
#define PDS_GPIO_SET1_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET1_INT_CLR_MSK                 (((1U << PDS_GPIO_SET1_INT_CLR_LEN) - 1) << PDS_GPIO_SET1_INT_CLR_POS)
#define PDS_GPIO_SET1_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET1_INT_CLR_LEN) - 1) << PDS_GPIO_SET1_INT_CLR_POS))
#define PDS_GPIO_SET1_INT_MODE                    PDS_GPIO_SET1_INT_MODE
#define PDS_GPIO_SET1_INT_MODE_POS                (4U)
#define PDS_GPIO_SET1_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET1_INT_MODE_MSK                (((1U << PDS_GPIO_SET1_INT_MODE_LEN) - 1) << PDS_GPIO_SET1_INT_MODE_POS)
#define PDS_GPIO_SET1_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET1_INT_MODE_LEN) - 1) << PDS_GPIO_SET1_INT_MODE_POS))
#define PDS_GPIO_SET2_INT_CLR                     PDS_GPIO_SET2_INT_CLR
#define PDS_GPIO_SET2_INT_CLR_POS                 (10U)
#define PDS_GPIO_SET2_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET2_INT_CLR_MSK                 (((1U << PDS_GPIO_SET2_INT_CLR_LEN) - 1) << PDS_GPIO_SET2_INT_CLR_POS)
#define PDS_GPIO_SET2_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET2_INT_CLR_LEN) - 1) << PDS_GPIO_SET2_INT_CLR_POS))
#define PDS_GPIO_SET2_INT_MODE                    PDS_GPIO_SET2_INT_MODE
#define PDS_GPIO_SET2_INT_MODE_POS                (12U)
#define PDS_GPIO_SET2_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET2_INT_MODE_MSK                (((1U << PDS_GPIO_SET2_INT_MODE_LEN) - 1) << PDS_GPIO_SET2_INT_MODE_POS)
#define PDS_GPIO_SET2_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET2_INT_MODE_LEN) - 1) << PDS_GPIO_SET2_INT_MODE_POS))
#define PDS_GPIO_SET3_INT_CLR                     PDS_GPIO_SET3_INT_CLR
#define PDS_GPIO_SET3_INT_CLR_POS                 (18U)
#define PDS_GPIO_SET3_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET3_INT_CLR_MSK                 (((1U << PDS_GPIO_SET3_INT_CLR_LEN) - 1) << PDS_GPIO_SET3_INT_CLR_POS)
#define PDS_GPIO_SET3_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET3_INT_CLR_LEN) - 1) << PDS_GPIO_SET3_INT_CLR_POS))
#define PDS_GPIO_SET3_INT_MODE                    PDS_GPIO_SET3_INT_MODE
#define PDS_GPIO_SET3_INT_MODE_POS                (20U)
#define PDS_GPIO_SET3_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET3_INT_MODE_MSK                (((1U << PDS_GPIO_SET3_INT_MODE_LEN) - 1) << PDS_GPIO_SET3_INT_MODE_POS)
#define PDS_GPIO_SET3_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET3_INT_MODE_LEN) - 1) << PDS_GPIO_SET3_INT_MODE_POS))
#define PDS_GPIO_SET4_INT_CLR                     PDS_GPIO_SET4_INT_CLR
#define PDS_GPIO_SET4_INT_CLR_POS                 (26U)
#define PDS_GPIO_SET4_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET4_INT_CLR_MSK                 (((1U << PDS_GPIO_SET4_INT_CLR_LEN) - 1) << PDS_GPIO_SET4_INT_CLR_POS)
#define PDS_GPIO_SET4_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET4_INT_CLR_LEN) - 1) << PDS_GPIO_SET4_INT_CLR_POS))
#define PDS_GPIO_SET4_INT_MODE                    PDS_GPIO_SET4_INT_MODE
#define PDS_GPIO_SET4_INT_MODE_POS                (28U)
#define PDS_GPIO_SET4_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET4_INT_MODE_MSK                (((1U << PDS_GPIO_SET4_INT_MODE_LEN) - 1) << PDS_GPIO_SET4_INT_MODE_POS)
#define PDS_GPIO_SET4_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET4_INT_MODE_LEN) - 1) << PDS_GPIO_SET4_INT_MODE_POS))

/* 0x44 : pds_gpio_stat */
#define PDS_GPIO_STAT_OFFSET                      (0x44)
#define PDS_GPIO_INT_STAT1                        PDS_GPIO_INT_STAT1
#define PDS_GPIO_INT_STAT1_POS                    (0U)
#define PDS_GPIO_INT_STAT1_LEN                    (32U)
#define PDS_GPIO_INT_STAT1_MSK                    (((1U << PDS_GPIO_INT_STAT1_LEN) - 1) << PDS_GPIO_INT_STAT1_POS)
#define PDS_GPIO_INT_STAT1_UMSK                   (~(((1U << PDS_GPIO_INT_STAT1_LEN) - 1) << PDS_GPIO_INT_STAT1_POS))

/* 0x48 : PDS_RAM3 */
#define PDS_RAM3_OFFSET                           (0x48)
#define PDS_CR_OCRAM_RET                          PDS_CR_OCRAM_RET
#define PDS_CR_OCRAM_RET_POS                      (0U)
#define PDS_CR_OCRAM_RET_LEN                      (20U)
#define PDS_CR_OCRAM_RET_MSK                      (((1U << PDS_CR_OCRAM_RET_LEN) - 1) << PDS_CR_OCRAM_RET_POS)
#define PDS_CR_OCRAM_RET_UMSK                     (~(((1U << PDS_CR_OCRAM_RET_LEN) - 1) << PDS_CR_OCRAM_RET_POS))

/* 0x4C : PDS_RAM4 */
#define PDS_RAM4_OFFSET                           (0x4C)
#define PDS_CR_OCRAM_SLP                          PDS_CR_OCRAM_SLP
#define PDS_CR_OCRAM_SLP_POS                      (0U)
#define PDS_CR_OCRAM_SLP_LEN                      (20U)
#define PDS_CR_OCRAM_SLP_MSK                      (((1U << PDS_CR_OCRAM_SLP_LEN) - 1) << PDS_CR_OCRAM_SLP_POS)
#define PDS_CR_OCRAM_SLP_UMSK                     (~(((1U << PDS_CR_OCRAM_SLP_LEN) - 1) << PDS_CR_OCRAM_SLP_POS))

/* 0x50 : XTAL_CNT_32K */
#define PDS_XTAL_CNT_32K_OFFSET                   (0x50)
#define PDS_RO_XTAL_CNT_32K_RES                   PDS_RO_XTAL_CNT_32K_RES
#define PDS_RO_XTAL_CNT_32K_RES_POS               (0U)
#define PDS_RO_XTAL_CNT_32K_RES_LEN               (6U)
#define PDS_RO_XTAL_CNT_32K_RES_MSK               (((1U << PDS_RO_XTAL_CNT_32K_RES_LEN) - 1) << PDS_RO_XTAL_CNT_32K_RES_POS)
#define PDS_RO_XTAL_CNT_32K_RES_UMSK              (~(((1U << PDS_RO_XTAL_CNT_32K_RES_LEN) - 1) << PDS_RO_XTAL_CNT_32K_RES_POS))
#define PDS_RO_XTAL_CNT_32K_CNT                   PDS_RO_XTAL_CNT_32K_CNT
#define PDS_RO_XTAL_CNT_32K_CNT_POS               (6U)
#define PDS_RO_XTAL_CNT_32K_CNT_LEN               (11U)
#define PDS_RO_XTAL_CNT_32K_CNT_MSK               (((1U << PDS_RO_XTAL_CNT_32K_CNT_LEN) - 1) << PDS_RO_XTAL_CNT_32K_CNT_POS)
#define PDS_RO_XTAL_CNT_32K_CNT_UMSK              (~(((1U << PDS_RO_XTAL_CNT_32K_CNT_LEN) - 1) << PDS_RO_XTAL_CNT_32K_CNT_POS))
#define PDS_REG_TOTAL_32K_CYCLE                   PDS_REG_TOTAL_32K_CYCLE
#define PDS_REG_TOTAL_32K_CYCLE_POS               (17U)
#define PDS_REG_TOTAL_32K_CYCLE_LEN               (3U)
#define PDS_REG_TOTAL_32K_CYCLE_MSK               (((1U << PDS_REG_TOTAL_32K_CYCLE_LEN) - 1) << PDS_REG_TOTAL_32K_CYCLE_POS)
#define PDS_REG_TOTAL_32K_CYCLE_UMSK              (~(((1U << PDS_REG_TOTAL_32K_CYCLE_LEN) - 1) << PDS_REG_TOTAL_32K_CYCLE_POS))
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN              PDS_CR_PDS_XTAL_CNT_RC32K_EN
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_POS          (20U)
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_LEN          (1U)
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_MSK          (((1U << PDS_CR_PDS_XTAL_CNT_RC32K_EN_LEN) - 1) << PDS_CR_PDS_XTAL_CNT_RC32K_EN_POS)
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_UMSK         (~(((1U << PDS_CR_PDS_XTAL_CNT_RC32K_EN_LEN) - 1) << PDS_CR_PDS_XTAL_CNT_RC32K_EN_POS))
#define PDS_XTAL_CNT_32K_PROCESS                  PDS_XTAL_CNT_32K_PROCESS
#define PDS_XTAL_CNT_32K_PROCESS_POS              (29U)
#define PDS_XTAL_CNT_32K_PROCESS_LEN              (1U)
#define PDS_XTAL_CNT_32K_PROCESS_MSK              (((1U << PDS_XTAL_CNT_32K_PROCESS_LEN) - 1) << PDS_XTAL_CNT_32K_PROCESS_POS)
#define PDS_XTAL_CNT_32K_PROCESS_UMSK             (~(((1U << PDS_XTAL_CNT_32K_PROCESS_LEN) - 1) << PDS_XTAL_CNT_32K_PROCESS_POS))
#define PDS_XTAL_CNT_32K_DONE                     PDS_XTAL_CNT_32K_DONE
#define PDS_XTAL_CNT_32K_DONE_POS                 (30U)
#define PDS_XTAL_CNT_32K_DONE_LEN                 (1U)
#define PDS_XTAL_CNT_32K_DONE_MSK                 (((1U << PDS_XTAL_CNT_32K_DONE_LEN) - 1) << PDS_XTAL_CNT_32K_DONE_POS)
#define PDS_XTAL_CNT_32K_DONE_UMSK                (~(((1U << PDS_XTAL_CNT_32K_DONE_LEN) - 1) << PDS_XTAL_CNT_32K_DONE_POS))

/* 0x54 : KYD_ROW_CFG */
#define PDS_KYD_ROW_CFG_OFFSET                    (0x54)
#define PDS_CFG_ROW_DETECT_CNT                    PDS_CFG_ROW_DETECT_CNT
#define PDS_CFG_ROW_DETECT_CNT_POS                (29U)
#define PDS_CFG_ROW_DETECT_CNT_LEN                (3U)
#define PDS_CFG_ROW_DETECT_CNT_MSK                (((1U << PDS_CFG_ROW_DETECT_CNT_LEN) - 1) << PDS_CFG_ROW_DETECT_CNT_POS)
#define PDS_CFG_ROW_DETECT_CNT_UMSK               (~(((1U << PDS_CFG_ROW_DETECT_CNT_LEN) - 1) << PDS_CFG_ROW_DETECT_CNT_POS))

/* 0x58 : KYD_CTL */
#define PDS_KYD_CTL_OFFSET                        (0x58)
#define PDS_CR_PDS_KYD_EN                         PDS_CR_PDS_KYD_EN
#define PDS_CR_PDS_KYD_EN_POS                     (0U)
#define PDS_CR_PDS_KYD_EN_LEN                     (1U)
#define PDS_CR_PDS_KYD_EN_MSK                     (((1U << PDS_CR_PDS_KYD_EN_LEN) - 1) << PDS_CR_PDS_KYD_EN_POS)
#define PDS_CR_PDS_KYD_EN_UMSK                    (~(((1U << PDS_CR_PDS_KYD_EN_LEN) - 1) << PDS_CR_PDS_KYD_EN_POS))
#define PDS_CR_PDS_KYD_COL_O_VAL                  PDS_CR_PDS_KYD_COL_O_VAL
#define PDS_CR_PDS_KYD_COL_O_VAL_POS              (1U)
#define PDS_CR_PDS_KYD_COL_O_VAL_LEN              (1U)
#define PDS_CR_PDS_KYD_COL_O_VAL_MSK              (((1U << PDS_CR_PDS_KYD_COL_O_VAL_LEN) - 1) << PDS_CR_PDS_KYD_COL_O_VAL_POS)
#define PDS_CR_PDS_KYD_COL_O_VAL_UMSK             (~(((1U << PDS_CR_PDS_KYD_COL_O_VAL_LEN) - 1) << PDS_CR_PDS_KYD_COL_O_VAL_POS))
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE            PDS_CR_PDS_KYD_ROW_MATRIX_SIZE
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS        (2U)
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_LEN        (3U)
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_MSK        (((1U << PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_LEN) - 1) << PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS)
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_UMSK       (~(((1U << PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_LEN) - 1) << PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS))
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE            PDS_CR_PDS_KYD_COL_MATRIX_SIZE
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS        (5U)
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_LEN        (3U)
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_MSK        (((1U << PDS_CR_PDS_KYD_COL_MATRIX_SIZE_LEN) - 1) << PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS)
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_UMSK       (~(((1U << PDS_CR_PDS_KYD_COL_MATRIX_SIZE_LEN) - 1) << PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS))
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN              PDS_CR_PDS_KYD_ROW_I_PULL_EN
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS          (8U)
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_LEN          (1U)
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_MSK          (((1U << PDS_CR_PDS_KYD_ROW_I_PULL_EN_LEN) - 1) << PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS)
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_UMSK         (~(((1U << PDS_CR_PDS_KYD_ROW_I_PULL_EN_LEN) - 1) << PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS))
#define PDS_RO_PDS_KYD_WAKEUP                     PDS_RO_PDS_KYD_WAKEUP
#define PDS_RO_PDS_KYD_WAKEUP_POS                 (9U)
#define PDS_RO_PDS_KYD_WAKEUP_LEN                 (1U)
#define PDS_RO_PDS_KYD_WAKEUP_MSK                 (((1U << PDS_RO_PDS_KYD_WAKEUP_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_POS)
#define PDS_RO_PDS_KYD_WAKEUP_UMSK                (~(((1U << PDS_RO_PDS_KYD_WAKEUP_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_POS))
#define PDS_CLR_PDS_KYD_WAKEUP                    PDS_CLR_PDS_KYD_WAKEUP
#define PDS_CLR_PDS_KYD_WAKEUP_POS                (10U)
#define PDS_CLR_PDS_KYD_WAKEUP_LEN                (1U)
#define PDS_CLR_PDS_KYD_WAKEUP_MSK                (((1U << PDS_CLR_PDS_KYD_WAKEUP_LEN) - 1) << PDS_CLR_PDS_KYD_WAKEUP_POS)
#define PDS_CLR_PDS_KYD_WAKEUP_UMSK               (~(((1U << PDS_CLR_PDS_KYD_WAKEUP_LEN) - 1) << PDS_CLR_PDS_KYD_WAKEUP_POS))
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO            PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS        (12U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_LEN        (6U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_MSK        (((1U << PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_UMSK       (~(((1U << PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS))
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX           PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS       (18U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_LEN       (3U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_MSK       (((1U << PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_UMSK      (~(((1U << PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS))
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX           PDS_RO_PDS_KYD_WAKEUP_COL_INDEX
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS       (21U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_LEN       (3U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_MSK       (((1U << PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS)
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_UMSK      (~(((1U << PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS))
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO            PDS_RO_PDS_KYD_WAKEUP_COL_GPIO
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS        (24U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_LEN        (6U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_MSK        (((1U << PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS)
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_UMSK       (~(((1U << PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_LEN) - 1) << PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS))

/* 0x5C : KYD_WHITE_SET */
#define PDS_KYD_WHITE_SET_OFFSET                  (0x5C)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX              PDS_CFG_WHITE_KEY0_ROW_INDEX
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_POS          (0U)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY0_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY0_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY0_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY0_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY0_COL_INDEX              PDS_CFG_WHITE_KEY0_COL_INDEX
#define PDS_CFG_WHITE_KEY0_COL_INDEX_POS          (3U)
#define PDS_CFG_WHITE_KEY0_COL_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY0_COL_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY0_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY0_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY0_COL_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY0_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY0_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY0_KYD_MODE               PDS_CFG_WHITE_KEY0_KYD_MODE
#define PDS_CFG_WHITE_KEY0_KYD_MODE_POS           (6U)
#define PDS_CFG_WHITE_KEY0_KYD_MODE_LEN           (2U)
#define PDS_CFG_WHITE_KEY0_KYD_MODE_MSK           (((1U << PDS_CFG_WHITE_KEY0_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY0_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY0_KYD_MODE_UMSK          (~(((1U << PDS_CFG_WHITE_KEY0_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY0_KYD_MODE_POS))
#define PDS_CFG_WHITE_KEY1_ROW_INDEX              PDS_CFG_WHITE_KEY1_ROW_INDEX
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_POS          (8U)
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY1_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY1_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY1_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY1_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY1_COL_INDEX              PDS_CFG_WHITE_KEY1_COL_INDEX
#define PDS_CFG_WHITE_KEY1_COL_INDEX_POS          (11U)
#define PDS_CFG_WHITE_KEY1_COL_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY1_COL_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY1_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY1_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY1_COL_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY1_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY1_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY1_KYD_MODE               PDS_CFG_WHITE_KEY1_KYD_MODE
#define PDS_CFG_WHITE_KEY1_KYD_MODE_POS           (14U)
#define PDS_CFG_WHITE_KEY1_KYD_MODE_LEN           (2U)
#define PDS_CFG_WHITE_KEY1_KYD_MODE_MSK           (((1U << PDS_CFG_WHITE_KEY1_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY1_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY1_KYD_MODE_UMSK          (~(((1U << PDS_CFG_WHITE_KEY1_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY1_KYD_MODE_POS))
#define PDS_CFG_WHITE_KEY2_ROW_INDEX              PDS_CFG_WHITE_KEY2_ROW_INDEX
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_POS          (16U)
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY2_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY2_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY2_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY2_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY2_COL_INDEX              PDS_CFG_WHITE_KEY2_COL_INDEX
#define PDS_CFG_WHITE_KEY2_COL_INDEX_POS          (19U)
#define PDS_CFG_WHITE_KEY2_COL_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY2_COL_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY2_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY2_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY2_COL_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY2_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY2_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY2_KYD_MODE               PDS_CFG_WHITE_KEY2_KYD_MODE
#define PDS_CFG_WHITE_KEY2_KYD_MODE_POS           (22U)
#define PDS_CFG_WHITE_KEY2_KYD_MODE_LEN           (2U)
#define PDS_CFG_WHITE_KEY2_KYD_MODE_MSK           (((1U << PDS_CFG_WHITE_KEY2_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY2_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY2_KYD_MODE_UMSK          (~(((1U << PDS_CFG_WHITE_KEY2_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY2_KYD_MODE_POS))
#define PDS_CFG_WHITE_KEY3_ROW_INDEX              PDS_CFG_WHITE_KEY3_ROW_INDEX
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_POS          (24U)
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY3_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY3_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY3_ROW_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY3_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY3_COL_INDEX              PDS_CFG_WHITE_KEY3_COL_INDEX
#define PDS_CFG_WHITE_KEY3_COL_INDEX_POS          (27U)
#define PDS_CFG_WHITE_KEY3_COL_INDEX_LEN          (3U)
#define PDS_CFG_WHITE_KEY3_COL_INDEX_MSK          (((1U << PDS_CFG_WHITE_KEY3_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY3_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY3_COL_INDEX_UMSK         (~(((1U << PDS_CFG_WHITE_KEY3_COL_INDEX_LEN) - 1) << PDS_CFG_WHITE_KEY3_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY3_KYD_MODE               PDS_CFG_WHITE_KEY3_KYD_MODE
#define PDS_CFG_WHITE_KEY3_KYD_MODE_POS           (30U)
#define PDS_CFG_WHITE_KEY3_KYD_MODE_LEN           (2U)
#define PDS_CFG_WHITE_KEY3_KYD_MODE_MSK           (((1U << PDS_CFG_WHITE_KEY3_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY3_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY3_KYD_MODE_UMSK          (~(((1U << PDS_CFG_WHITE_KEY3_KYD_MODE_LEN) - 1) << PDS_CFG_WHITE_KEY3_KYD_MODE_POS))

/* 0x60 : KYS_ROW_I_GPIO_SEL0 */
#define PDS_KYS_ROW_I_GPIO_SEL0_OFFSET            (0x60)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL              PDS_CFG_KEY_ROW_I_0_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_POS          (0U)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_0_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_0_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_0_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_0_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL              PDS_CFG_KEY_ROW_I_1_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_POS          (8U)
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_1_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_1_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_1_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_1_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL              PDS_CFG_KEY_ROW_I_2_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_POS          (16U)
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_2_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_2_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_2_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_2_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL              PDS_CFG_KEY_ROW_I_3_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_POS          (24U)
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_3_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_3_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_3_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_3_GPIO_SEL_POS))

/* 0x64 : KYS_ROW_I_GPIO_SEL1 */
#define PDS_KYS_ROW_I_GPIO_SEL1_OFFSET            (0x64)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL              PDS_CFG_KEY_ROW_I_4_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_POS          (0U)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_4_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_4_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_4_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_4_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL              PDS_CFG_KEY_ROW_I_5_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_POS          (8U)
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_5_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_5_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_5_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_5_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL              PDS_CFG_KEY_ROW_I_6_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_POS          (16U)
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_6_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_6_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_6_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_6_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL              PDS_CFG_KEY_ROW_I_7_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_POS          (24U)
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_ROW_I_7_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_7_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_ROW_I_7_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_ROW_I_7_GPIO_SEL_POS))

/* 0x68 : KYS_COL_O_GPIO_SEL0 */
#define PDS_KYS_COL_O_GPIO_SEL0_OFFSET            (0x68)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL              PDS_CFG_KEY_COL_O_0_GPIO_SEL
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_POS          (0U)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_0_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_0_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_0_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_0_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL              PDS_CFG_KEY_COL_O_1_GPIO_SEL
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_POS          (8U)
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_1_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_1_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_1_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_1_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL              PDS_CFG_KEY_COL_O_2_GPIO_SEL
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_POS          (16U)
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_2_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_2_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_2_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_2_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL              PDS_CFG_KEY_COL_O_3_GPIO_SEL
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_POS          (24U)
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_3_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_3_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_3_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_3_GPIO_SEL_POS))

/* 0x6C : KYS_COL_O_GPIO_SEL1 */
#define PDS_KYS_COL_O_GPIO_SEL1_OFFSET            (0x6C)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL              PDS_CFG_KEY_COL_O_4_GPIO_SEL
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_POS          (0U)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_4_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_4_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_4_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_4_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL              PDS_CFG_KEY_COL_O_5_GPIO_SEL
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_POS          (8U)
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_5_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_5_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_5_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_5_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL              PDS_CFG_KEY_COL_O_6_GPIO_SEL
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_POS          (16U)
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_6_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_6_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_6_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_6_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL              PDS_CFG_KEY_COL_O_7_GPIO_SEL
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_POS          (24U)
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_LEN          (6U)
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_MSK          (((1U << PDS_CFG_KEY_COL_O_7_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_7_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_UMSK         (~(((1U << PDS_CFG_KEY_COL_O_7_GPIO_SEL_LEN) - 1) << PDS_CFG_KEY_COL_O_7_GPIO_SEL_POS))

/* 0x70 : pds_gpio_pd_0 */
#define PDS_GPIO_PD_0_OFFSET                      (0x70)
#define PDS_CR_PDS_GPIO_PD_0                      PDS_CR_PDS_GPIO_PD_0
#define PDS_CR_PDS_GPIO_PD_0_POS                  (0U)
#define PDS_CR_PDS_GPIO_PD_0_LEN                  (32U)
#define PDS_CR_PDS_GPIO_PD_0_MSK                  (((1U << PDS_CR_PDS_GPIO_PD_0_LEN) - 1) << PDS_CR_PDS_GPIO_PD_0_POS)
#define PDS_CR_PDS_GPIO_PD_0_UMSK                 (~(((1U << PDS_CR_PDS_GPIO_PD_0_LEN) - 1) << PDS_CR_PDS_GPIO_PD_0_POS))

/* 0x74 : pds_gpio_pd_1 */
#define PDS_GPIO_PD_1_OFFSET                      (0x74)
#define PDS_CR_PDS_GPIO_PD_1                      PDS_CR_PDS_GPIO_PD_1
#define PDS_CR_PDS_GPIO_PD_1_POS                  (0U)
#define PDS_CR_PDS_GPIO_PD_1_LEN                  (21U)
#define PDS_CR_PDS_GPIO_PD_1_MSK                  (((1U << PDS_CR_PDS_GPIO_PD_1_LEN) - 1) << PDS_CR_PDS_GPIO_PD_1_POS)
#define PDS_CR_PDS_GPIO_PD_1_UMSK                 (~(((1U << PDS_CR_PDS_GPIO_PD_1_LEN) - 1) << PDS_CR_PDS_GPIO_PD_1_POS))

/* 0x78 : pds_gpio_pu_0 */
#define PDS_GPIO_PU_0_OFFSET                      (0x78)
#define PDS_CR_PDS_GPIO_PU_0                      PDS_CR_PDS_GPIO_PU_0
#define PDS_CR_PDS_GPIO_PU_0_POS                  (0U)
#define PDS_CR_PDS_GPIO_PU_0_LEN                  (32U)
#define PDS_CR_PDS_GPIO_PU_0_MSK                  (((1U << PDS_CR_PDS_GPIO_PU_0_LEN) - 1) << PDS_CR_PDS_GPIO_PU_0_POS)
#define PDS_CR_PDS_GPIO_PU_0_UMSK                 (~(((1U << PDS_CR_PDS_GPIO_PU_0_LEN) - 1) << PDS_CR_PDS_GPIO_PU_0_POS))

/* 0x7C : pds_gpio_pu_1 */
#define PDS_GPIO_PU_1_OFFSET                      (0x7C)
#define PDS_CR_PDS_GPIO_PU_1                      PDS_CR_PDS_GPIO_PU_1
#define PDS_CR_PDS_GPIO_PU_1_POS                  (0U)
#define PDS_CR_PDS_GPIO_PU_1_LEN                  (21U)
#define PDS_CR_PDS_GPIO_PU_1_MSK                  (((1U << PDS_CR_PDS_GPIO_PU_1_LEN) - 1) << PDS_CR_PDS_GPIO_PU_1_POS)
#define PDS_CR_PDS_GPIO_PU_1_UMSK                 (~(((1U << PDS_CR_PDS_GPIO_PU_1_LEN) - 1) << PDS_CR_PDS_GPIO_PU_1_POS))

/* 0x80 : pds_gpio_ie_0 */
#define PDS_GPIO_IE_0_OFFSET                      (0x80)
#define PDS_CR_PDS_GPIO_IE_0                      PDS_CR_PDS_GPIO_IE_0
#define PDS_CR_PDS_GPIO_IE_0_POS                  (0U)
#define PDS_CR_PDS_GPIO_IE_0_LEN                  (32U)
#define PDS_CR_PDS_GPIO_IE_0_MSK                  (((1U << PDS_CR_PDS_GPIO_IE_0_LEN) - 1) << PDS_CR_PDS_GPIO_IE_0_POS)
#define PDS_CR_PDS_GPIO_IE_0_UMSK                 (~(((1U << PDS_CR_PDS_GPIO_IE_0_LEN) - 1) << PDS_CR_PDS_GPIO_IE_0_POS))

/* 0x84 : pds_gpio_ie_1 */
#define PDS_GPIO_IE_1_OFFSET                      (0x84)
#define PDS_CR_PDS_GPIO_IE_1                      PDS_CR_PDS_GPIO_IE_1
#define PDS_CR_PDS_GPIO_IE_1_POS                  (0U)
#define PDS_CR_PDS_GPIO_IE_1_LEN                  (21U)
#define PDS_CR_PDS_GPIO_IE_1_MSK                  (((1U << PDS_CR_PDS_GPIO_IE_1_LEN) - 1) << PDS_CR_PDS_GPIO_IE_1_POS)
#define PDS_CR_PDS_GPIO_IE_1_UMSK                 (~(((1U << PDS_CR_PDS_GPIO_IE_1_LEN) - 1) << PDS_CR_PDS_GPIO_IE_1_POS))

/* 0x88 : pds_gpio_pd_set2 */
#define PDS_GPIO_PD_SET2_OFFSET                   (0x88)
#define PDS_CR_PDS_GPIO_SET_INT_MASK2             PDS_CR_PDS_GPIO_SET_INT_MASK2
#define PDS_CR_PDS_GPIO_SET_INT_MASK2_POS         (0U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK2_LEN         (21U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK2_MSK         (((1U << PDS_CR_PDS_GPIO_SET_INT_MASK2_LEN) - 1) << PDS_CR_PDS_GPIO_SET_INT_MASK2_POS)
#define PDS_CR_PDS_GPIO_SET_INT_MASK2_UMSK        (~(((1U << PDS_CR_PDS_GPIO_SET_INT_MASK2_LEN) - 1) << PDS_CR_PDS_GPIO_SET_INT_MASK2_POS))

/* 0x8C : pds_gpio_int2 */
#define PDS_GPIO_INT2_OFFSET                      (0x8C)
#define PDS_GPIO_SET5_INT_CLR                     PDS_GPIO_SET5_INT_CLR
#define PDS_GPIO_SET5_INT_CLR_POS                 (2U)
#define PDS_GPIO_SET5_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET5_INT_CLR_MSK                 (((1U << PDS_GPIO_SET5_INT_CLR_LEN) - 1) << PDS_GPIO_SET5_INT_CLR_POS)
#define PDS_GPIO_SET5_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET5_INT_CLR_LEN) - 1) << PDS_GPIO_SET5_INT_CLR_POS))
#define PDS_GPIO_SET5_INT_MODE                    PDS_GPIO_SET5_INT_MODE
#define PDS_GPIO_SET5_INT_MODE_POS                (4U)
#define PDS_GPIO_SET5_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET5_INT_MODE_MSK                (((1U << PDS_GPIO_SET5_INT_MODE_LEN) - 1) << PDS_GPIO_SET5_INT_MODE_POS)
#define PDS_GPIO_SET5_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET5_INT_MODE_LEN) - 1) << PDS_GPIO_SET5_INT_MODE_POS))
#define PDS_GPIO_SET6_INT_CLR                     PDS_GPIO_SET6_INT_CLR
#define PDS_GPIO_SET6_INT_CLR_POS                 (10U)
#define PDS_GPIO_SET6_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET6_INT_CLR_MSK                 (((1U << PDS_GPIO_SET6_INT_CLR_LEN) - 1) << PDS_GPIO_SET6_INT_CLR_POS)
#define PDS_GPIO_SET6_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET6_INT_CLR_LEN) - 1) << PDS_GPIO_SET6_INT_CLR_POS))
#define PDS_GPIO_SET6_INT_MODE                    PDS_GPIO_SET6_INT_MODE
#define PDS_GPIO_SET6_INT_MODE_POS                (12U)
#define PDS_GPIO_SET6_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET6_INT_MODE_MSK                (((1U << PDS_GPIO_SET6_INT_MODE_LEN) - 1) << PDS_GPIO_SET6_INT_MODE_POS)
#define PDS_GPIO_SET6_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET6_INT_MODE_LEN) - 1) << PDS_GPIO_SET6_INT_MODE_POS))
#define PDS_GPIO_SET7_INT_CLR                     PDS_GPIO_SET7_INT_CLR
#define PDS_GPIO_SET7_INT_CLR_POS                 (18U)
#define PDS_GPIO_SET7_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET7_INT_CLR_MSK                 (((1U << PDS_GPIO_SET7_INT_CLR_LEN) - 1) << PDS_GPIO_SET7_INT_CLR_POS)
#define PDS_GPIO_SET7_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET7_INT_CLR_LEN) - 1) << PDS_GPIO_SET7_INT_CLR_POS))
#define PDS_GPIO_SET7_INT_MODE                    PDS_GPIO_SET7_INT_MODE
#define PDS_GPIO_SET7_INT_MODE_POS                (20U)
#define PDS_GPIO_SET7_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET7_INT_MODE_MSK                (((1U << PDS_GPIO_SET7_INT_MODE_LEN) - 1) << PDS_GPIO_SET7_INT_MODE_POS)
#define PDS_GPIO_SET7_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET7_INT_MODE_LEN) - 1) << PDS_GPIO_SET7_INT_MODE_POS))
#define PDS_GPIO_SET8_INT_CLR                     PDS_GPIO_SET8_INT_CLR
#define PDS_GPIO_SET8_INT_CLR_POS                 (26U)
#define PDS_GPIO_SET8_INT_CLR_LEN                 (1U)
#define PDS_GPIO_SET8_INT_CLR_MSK                 (((1U << PDS_GPIO_SET8_INT_CLR_LEN) - 1) << PDS_GPIO_SET8_INT_CLR_POS)
#define PDS_GPIO_SET8_INT_CLR_UMSK                (~(((1U << PDS_GPIO_SET8_INT_CLR_LEN) - 1) << PDS_GPIO_SET8_INT_CLR_POS))
#define PDS_GPIO_SET8_INT_MODE                    PDS_GPIO_SET8_INT_MODE
#define PDS_GPIO_SET8_INT_MODE_POS                (28U)
#define PDS_GPIO_SET8_INT_MODE_LEN                (4U)
#define PDS_GPIO_SET8_INT_MODE_MSK                (((1U << PDS_GPIO_SET8_INT_MODE_LEN) - 1) << PDS_GPIO_SET8_INT_MODE_POS)
#define PDS_GPIO_SET8_INT_MODE_UMSK               (~(((1U << PDS_GPIO_SET8_INT_MODE_LEN) - 1) << PDS_GPIO_SET8_INT_MODE_POS))

/* 0x90 : pds_gpio_int_stat2 */
#define PDS_GPIO_INT_STAT2_OFFSET                 (0x90)
#define PDS_GPIO_INT_STAT2                        PDS_GPIO_INT_STAT2
#define PDS_GPIO_INT_STAT2_POS                    (0U)
#define PDS_GPIO_INT_STAT2_LEN                    (21U)
#define PDS_GPIO_INT_STAT2_MSK                    (((1U << PDS_GPIO_INT_STAT2_LEN) - 1) << PDS_GPIO_INT_STAT2_POS)
#define PDS_GPIO_INT_STAT2_UMSK                   (~(((1U << PDS_GPIO_INT_STAT2_LEN) - 1) << PDS_GPIO_INT_STAT2_POS))

/* 0x114 : cpu_core_cfg1 */
#define PDS_CPU_CORE_CFG1_OFFSET                  (0x114)
#define PDS_REG_PLL_SEL                           PDS_REG_PLL_SEL
#define PDS_REG_PLL_SEL_POS                       (4U)
#define PDS_REG_PLL_SEL_LEN                       (2U)
#define PDS_REG_PLL_SEL_MSK                       (((1U << PDS_REG_PLL_SEL_LEN) - 1) << PDS_REG_PLL_SEL_POS)
#define PDS_REG_PLL_SEL_UMSK                      (~(((1U << PDS_REG_PLL_SEL_LEN) - 1) << PDS_REG_PLL_SEL_POS))
#define PDS_REG_MCU_MAIN_CLK_EN                   PDS_REG_MCU_MAIN_CLK_EN
#define PDS_REG_MCU_MAIN_CLK_EN_POS               (8U)
#define PDS_REG_MCU_MAIN_CLK_EN_LEN               (1U)
#define PDS_REG_MCU_MAIN_CLK_EN_MSK               (((1U << PDS_REG_MCU_MAIN_CLK_EN_LEN) - 1) << PDS_REG_MCU_MAIN_CLK_EN_POS)
#define PDS_REG_MCU_MAIN_CLK_EN_UMSK              (~(((1U << PDS_REG_MCU_MAIN_CLK_EN_LEN) - 1) << PDS_REG_MCU_MAIN_CLK_EN_POS))
#define PDS_REG_MCU_WL_CLK_EN                     PDS_REG_MCU_WL_CLK_EN
#define PDS_REG_MCU_WL_CLK_EN_POS                 (9U)
#define PDS_REG_MCU_WL_CLK_EN_LEN                 (1U)
#define PDS_REG_MCU_WL_CLK_EN_MSK                 (((1U << PDS_REG_MCU_WL_CLK_EN_LEN) - 1) << PDS_REG_MCU_WL_CLK_EN_POS)
#define PDS_REG_MCU_WL_CLK_EN_UMSK                (~(((1U << PDS_REG_MCU_WL_CLK_EN_LEN) - 1) << PDS_REG_MCU_WL_CLK_EN_POS))

/* 0x148 : cpu_core_cfg14 */
#define PDS_CPU_CORE_CFG14_OFFSET                 (0x148)
#define PDS_MAIN_MCU_RST_ADDR                     PDS_MAIN_MCU_RST_ADDR
#define PDS_MAIN_MCU_RST_ADDR_POS                 (0U)
#define PDS_MAIN_MCU_RST_ADDR_LEN                 (32U)
#define PDS_MAIN_MCU_RST_ADDR_MSK                 (((1U << PDS_MAIN_MCU_RST_ADDR_LEN) - 1) << PDS_MAIN_MCU_RST_ADDR_POS)
#define PDS_MAIN_MCU_RST_ADDR_UMSK                (~(((1U << PDS_MAIN_MCU_RST_ADDR_LEN) - 1) << PDS_MAIN_MCU_RST_ADDR_POS))

/* 0x14C : tzc_pds */
#define PDS_TZC_PDS_OFFSET                        (0x14C)
#define PDS_CR_MAIN_MCU_CFG_WR_LOCK               PDS_CR_MAIN_MCU_CFG_WR_LOCK
#define PDS_CR_MAIN_MCU_CFG_WR_LOCK_POS           (1U)
#define PDS_CR_MAIN_MCU_CFG_WR_LOCK_LEN           (1U)
#define PDS_CR_MAIN_MCU_CFG_WR_LOCK_MSK           (((1U << PDS_CR_MAIN_MCU_CFG_WR_LOCK_LEN) - 1) << PDS_CR_MAIN_MCU_CFG_WR_LOCK_POS)
#define PDS_CR_MAIN_MCU_CFG_WR_LOCK_UMSK          (~(((1U << PDS_CR_MAIN_MCU_CFG_WR_LOCK_LEN) - 1) << PDS_CR_MAIN_MCU_CFG_WR_LOCK_POS))

/* 0x300 : rc32m_ctrl0 */
#define PDS_RC32M_CTRL0_OFFSET                    (0x300)
#define PDS_RC32M_CAL_DONE                        PDS_RC32M_CAL_DONE
#define PDS_RC32M_CAL_DONE_POS                    (0U)
#define PDS_RC32M_CAL_DONE_LEN                    (1U)
#define PDS_RC32M_CAL_DONE_MSK                    (((1U << PDS_RC32M_CAL_DONE_LEN) - 1) << PDS_RC32M_CAL_DONE_POS)
#define PDS_RC32M_CAL_DONE_UMSK                   (~(((1U << PDS_RC32M_CAL_DONE_LEN) - 1) << PDS_RC32M_CAL_DONE_POS))
#define PDS_RC32M_RDY                             PDS_RC32M_RDY
#define PDS_RC32M_RDY_POS                         (1U)
#define PDS_RC32M_RDY_LEN                         (1U)
#define PDS_RC32M_RDY_MSK                         (((1U << PDS_RC32M_RDY_LEN) - 1) << PDS_RC32M_RDY_POS)
#define PDS_RC32M_RDY_UMSK                        (~(((1U << PDS_RC32M_RDY_LEN) - 1) << PDS_RC32M_RDY_POS))
#define PDS_RC32M_CAL_INPROGRESS                  PDS_RC32M_CAL_INPROGRESS
#define PDS_RC32M_CAL_INPROGRESS_POS              (2U)
#define PDS_RC32M_CAL_INPROGRESS_LEN              (1U)
#define PDS_RC32M_CAL_INPROGRESS_MSK              (((1U << PDS_RC32M_CAL_INPROGRESS_LEN) - 1) << PDS_RC32M_CAL_INPROGRESS_POS)
#define PDS_RC32M_CAL_INPROGRESS_UMSK             (~(((1U << PDS_RC32M_CAL_INPROGRESS_LEN) - 1) << PDS_RC32M_CAL_INPROGRESS_POS))
#define PDS_RC32M_CAL_DIV                         PDS_RC32M_CAL_DIV
#define PDS_RC32M_CAL_DIV_POS                     (3U)
#define PDS_RC32M_CAL_DIV_LEN                     (2U)
#define PDS_RC32M_CAL_DIV_MSK                     (((1U << PDS_RC32M_CAL_DIV_LEN) - 1) << PDS_RC32M_CAL_DIV_POS)
#define PDS_RC32M_CAL_DIV_UMSK                    (~(((1U << PDS_RC32M_CAL_DIV_LEN) - 1) << PDS_RC32M_CAL_DIV_POS))
#define PDS_RC32M_CAL_PRECHARGE                   PDS_RC32M_CAL_PRECHARGE
#define PDS_RC32M_CAL_PRECHARGE_POS               (5U)
#define PDS_RC32M_CAL_PRECHARGE_LEN               (1U)
#define PDS_RC32M_CAL_PRECHARGE_MSK               (((1U << PDS_RC32M_CAL_PRECHARGE_LEN) - 1) << PDS_RC32M_CAL_PRECHARGE_POS)
#define PDS_RC32M_CAL_PRECHARGE_UMSK              (~(((1U << PDS_RC32M_CAL_PRECHARGE_LEN) - 1) << PDS_RC32M_CAL_PRECHARGE_POS))
#define PDS_RC32M_DIG_CODE_FR_CAL                 PDS_RC32M_DIG_CODE_FR_CAL
#define PDS_RC32M_DIG_CODE_FR_CAL_POS             (6U)
#define PDS_RC32M_DIG_CODE_FR_CAL_LEN             (8U)
#define PDS_RC32M_DIG_CODE_FR_CAL_MSK             (((1U << PDS_RC32M_DIG_CODE_FR_CAL_LEN) - 1) << PDS_RC32M_DIG_CODE_FR_CAL_POS)
#define PDS_RC32M_DIG_CODE_FR_CAL_UMSK            (~(((1U << PDS_RC32M_DIG_CODE_FR_CAL_LEN) - 1) << PDS_RC32M_DIG_CODE_FR_CAL_POS))
#define PDS_RC32M_ALLOW_CAL                       PDS_RC32M_ALLOW_CAL
#define PDS_RC32M_ALLOW_CAL_POS                   (17U)
#define PDS_RC32M_ALLOW_CAL_LEN                   (1U)
#define PDS_RC32M_ALLOW_CAL_MSK                   (((1U << PDS_RC32M_ALLOW_CAL_LEN) - 1) << PDS_RC32M_ALLOW_CAL_POS)
#define PDS_RC32M_ALLOW_CAL_UMSK                  (~(((1U << PDS_RC32M_ALLOW_CAL_LEN) - 1) << PDS_RC32M_ALLOW_CAL_POS))
#define PDS_RC32M_REFCLK_HALF                     PDS_RC32M_REFCLK_HALF
#define PDS_RC32M_REFCLK_HALF_POS                 (18U)
#define PDS_RC32M_REFCLK_HALF_LEN                 (1U)
#define PDS_RC32M_REFCLK_HALF_MSK                 (((1U << PDS_RC32M_REFCLK_HALF_LEN) - 1) << PDS_RC32M_REFCLK_HALF_POS)
#define PDS_RC32M_REFCLK_HALF_UMSK                (~(((1U << PDS_RC32M_REFCLK_HALF_LEN) - 1) << PDS_RC32M_REFCLK_HALF_POS))
#define PDS_RC32M_EXT_CODE_EN                     PDS_RC32M_EXT_CODE_EN
#define PDS_RC32M_EXT_CODE_EN_POS                 (19U)
#define PDS_RC32M_EXT_CODE_EN_LEN                 (1U)
#define PDS_RC32M_EXT_CODE_EN_MSK                 (((1U << PDS_RC32M_EXT_CODE_EN_LEN) - 1) << PDS_RC32M_EXT_CODE_EN_POS)
#define PDS_RC32M_EXT_CODE_EN_UMSK                (~(((1U << PDS_RC32M_EXT_CODE_EN_LEN) - 1) << PDS_RC32M_EXT_CODE_EN_POS))
#define PDS_RC32M_CAL_EN                          PDS_RC32M_CAL_EN
#define PDS_RC32M_CAL_EN_POS                      (20U)
#define PDS_RC32M_CAL_EN_LEN                      (1U)
#define PDS_RC32M_CAL_EN_MSK                      (((1U << PDS_RC32M_CAL_EN_LEN) - 1) << PDS_RC32M_CAL_EN_POS)
#define PDS_RC32M_CAL_EN_UMSK                     (~(((1U << PDS_RC32M_CAL_EN_LEN) - 1) << PDS_RC32M_CAL_EN_POS))
#define PDS_RC32M_PD                              PDS_RC32M_PD
#define PDS_RC32M_PD_POS                          (21U)
#define PDS_RC32M_PD_LEN                          (1U)
#define PDS_RC32M_PD_MSK                          (((1U << PDS_RC32M_PD_LEN) - 1) << PDS_RC32M_PD_POS)
#define PDS_RC32M_PD_UMSK                         (~(((1U << PDS_RC32M_PD_LEN) - 1) << PDS_RC32M_PD_POS))

/* 0x304 : rc32m_ctrl1 */
#define PDS_RC32M_CTRL1_OFFSET                    (0x304)
#define PDS_RC32M_TEST_EN                         PDS_RC32M_TEST_EN
#define PDS_RC32M_TEST_EN_POS                     (0U)
#define PDS_RC32M_TEST_EN_LEN                     (1U)
#define PDS_RC32M_TEST_EN_MSK                     (((1U << PDS_RC32M_TEST_EN_LEN) - 1) << PDS_RC32M_TEST_EN_POS)
#define PDS_RC32M_TEST_EN_UMSK                    (~(((1U << PDS_RC32M_TEST_EN_LEN) - 1) << PDS_RC32M_TEST_EN_POS))
#define PDS_RC32M_SOFT_RST                        PDS_RC32M_SOFT_RST
#define PDS_RC32M_SOFT_RST_POS                    (1U)
#define PDS_RC32M_SOFT_RST_LEN                    (1U)
#define PDS_RC32M_SOFT_RST_MSK                    (((1U << PDS_RC32M_SOFT_RST_LEN) - 1) << PDS_RC32M_SOFT_RST_POS)
#define PDS_RC32M_SOFT_RST_UMSK                   (~(((1U << PDS_RC32M_SOFT_RST_LEN) - 1) << PDS_RC32M_SOFT_RST_POS))
#define PDS_RC32M_CLK_SOFT_RST                    PDS_RC32M_CLK_SOFT_RST
#define PDS_RC32M_CLK_SOFT_RST_POS                (2U)
#define PDS_RC32M_CLK_SOFT_RST_LEN                (1U)
#define PDS_RC32M_CLK_SOFT_RST_MSK                (((1U << PDS_RC32M_CLK_SOFT_RST_LEN) - 1) << PDS_RC32M_CLK_SOFT_RST_POS)
#define PDS_RC32M_CLK_SOFT_RST_UMSK               (~(((1U << PDS_RC32M_CLK_SOFT_RST_LEN) - 1) << PDS_RC32M_CLK_SOFT_RST_POS))
#define PDS_RC32M_CLK_INV                         PDS_RC32M_CLK_INV
#define PDS_RC32M_CLK_INV_POS                     (3U)
#define PDS_RC32M_CLK_INV_LEN                     (1U)
#define PDS_RC32M_CLK_INV_MSK                     (((1U << PDS_RC32M_CLK_INV_LEN) - 1) << PDS_RC32M_CLK_INV_POS)
#define PDS_RC32M_CLK_INV_UMSK                    (~(((1U << PDS_RC32M_CLK_INV_LEN) - 1) << PDS_RC32M_CLK_INV_POS))
#define PDS_RC32M_CLK_FORCE_ON                    PDS_RC32M_CLK_FORCE_ON
#define PDS_RC32M_CLK_FORCE_ON_POS                (4U)
#define PDS_RC32M_CLK_FORCE_ON_LEN                (1U)
#define PDS_RC32M_CLK_FORCE_ON_MSK                (((1U << PDS_RC32M_CLK_FORCE_ON_LEN) - 1) << PDS_RC32M_CLK_FORCE_ON_POS)
#define PDS_RC32M_CLK_FORCE_ON_UMSK               (~(((1U << PDS_RC32M_CLK_FORCE_ON_LEN) - 1) << PDS_RC32M_CLK_FORCE_ON_POS))

/* 0x400 : pu_rst_clkpll */
#define PDS_PU_RST_CLKPLL_OFFSET                  (0x400)
#define PDS_CR_PDS_PU_CLKPLL_SFREG                PDS_CR_PDS_PU_CLKPLL_SFREG
#define PDS_CR_PDS_PU_CLKPLL_SFREG_POS            (9U)
#define PDS_CR_PDS_PU_CLKPLL_SFREG_LEN            (1U)
#define PDS_CR_PDS_PU_CLKPLL_SFREG_MSK            (((1U << PDS_CR_PDS_PU_CLKPLL_SFREG_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_SFREG_POS)
#define PDS_CR_PDS_PU_CLKPLL_SFREG_UMSK           (~(((1U << PDS_CR_PDS_PU_CLKPLL_SFREG_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_SFREG_POS))
#define PDS_CR_PDS_PU_CLKPLL                      PDS_CR_PDS_PU_CLKPLL
#define PDS_CR_PDS_PU_CLKPLL_POS                  (10U)
#define PDS_CR_PDS_PU_CLKPLL_LEN                  (1U)
#define PDS_CR_PDS_PU_CLKPLL_MSK                  (((1U << PDS_CR_PDS_PU_CLKPLL_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_POS)
#define PDS_CR_PDS_PU_CLKPLL_UMSK                 (~(((1U << PDS_CR_PDS_PU_CLKPLL_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_POS))

/* 0x500 : usb_ctrl_0 */
#define PDS_USB_CTRL_0_OFFSET                     (0x500)
#define PDS_CR_USB_SW_RST_N                       PDS_CR_USB_SW_RST_N
#define PDS_CR_USB_SW_RST_N_POS                   (0U)
#define PDS_CR_USB_SW_RST_N_LEN                   (1U)
#define PDS_CR_USB_SW_RST_N_MSK                   (((1U << PDS_CR_USB_SW_RST_N_LEN) - 1) << PDS_CR_USB_SW_RST_N_POS)
#define PDS_CR_USB_SW_RST_N_UMSK                  (~(((1U << PDS_CR_USB_SW_RST_N_LEN) - 1) << PDS_CR_USB_SW_RST_N_POS))
#define PDS_CR_USB_IDDIG                          PDS_CR_USB_IDDIG
#define PDS_CR_USB_IDDIG_POS                      (1U)
#define PDS_CR_USB_IDDIG_LEN                      (1U)
#define PDS_CR_USB_IDDIG_MSK                      (((1U << PDS_CR_USB_IDDIG_LEN) - 1) << PDS_CR_USB_IDDIG_POS)
#define PDS_CR_USB_IDDIG_UMSK                     (~(((1U << PDS_CR_USB_IDDIG_LEN) - 1) << PDS_CR_USB_IDDIG_POS))
#define PDS_CR_USB_L1_WAKEUP                      PDS_CR_USB_L1_WAKEUP
#define PDS_CR_USB_L1_WAKEUP_POS                  (2U)
#define PDS_CR_USB_L1_WAKEUP_LEN                  (1U)
#define PDS_CR_USB_L1_WAKEUP_MSK                  (((1U << PDS_CR_USB_L1_WAKEUP_LEN) - 1) << PDS_CR_USB_L1_WAKEUP_POS)
#define PDS_CR_USB_L1_WAKEUP_UMSK                 (~(((1U << PDS_CR_USB_L1_WAKEUP_LEN) - 1) << PDS_CR_USB_L1_WAKEUP_POS))
#define PDS_CR_USB_WAKEUP                         PDS_CR_USB_WAKEUP
#define PDS_CR_USB_WAKEUP_POS                     (3U)
#define PDS_CR_USB_WAKEUP_LEN                     (1U)
#define PDS_CR_USB_WAKEUP_MSK                     (((1U << PDS_CR_USB_WAKEUP_LEN) - 1) << PDS_CR_USB_WAKEUP_POS)
#define PDS_CR_USB_WAKEUP_UMSK                    (~(((1U << PDS_CR_USB_WAKEUP_LEN) - 1) << PDS_CR_USB_WAKEUP_POS))
#define PDS_CR_USB_VBUS_VLD_SW_MODE               PDS_CR_USB_VBUS_VLD_SW_MODE
#define PDS_CR_USB_VBUS_VLD_SW_MODE_POS           (4U)
#define PDS_CR_USB_VBUS_VLD_SW_MODE_LEN           (1U)
#define PDS_CR_USB_VBUS_VLD_SW_MODE_MSK           (((1U << PDS_CR_USB_VBUS_VLD_SW_MODE_LEN) - 1) << PDS_CR_USB_VBUS_VLD_SW_MODE_POS)
#define PDS_CR_USB_VBUS_VLD_SW_MODE_UMSK          (~(((1U << PDS_CR_USB_VBUS_VLD_SW_MODE_LEN) - 1) << PDS_CR_USB_VBUS_VLD_SW_MODE_POS))
#define PDS_CR_USB_VBUS_VLD_PHY_EN                PDS_CR_USB_VBUS_VLD_PHY_EN
#define PDS_CR_USB_VBUS_VLD_PHY_EN_POS            (5U)
#define PDS_CR_USB_VBUS_VLD_PHY_EN_LEN            (1U)
#define PDS_CR_USB_VBUS_VLD_PHY_EN_MSK            (((1U << PDS_CR_USB_VBUS_VLD_PHY_EN_LEN) - 1) << PDS_CR_USB_VBUS_VLD_PHY_EN_POS)
#define PDS_CR_USB_VBUS_VLD_PHY_EN_UMSK           (~(((1U << PDS_CR_USB_VBUS_VLD_PHY_EN_LEN) - 1) << PDS_CR_USB_VBUS_VLD_PHY_EN_POS))
#define PDS_CR_USB_VBUS_VLD_GPIO_EN               PDS_CR_USB_VBUS_VLD_GPIO_EN
#define PDS_CR_USB_VBUS_VLD_GPIO_EN_POS           (6U)
#define PDS_CR_USB_VBUS_VLD_GPIO_EN_LEN           (1U)
#define PDS_CR_USB_VBUS_VLD_GPIO_EN_MSK           (((1U << PDS_CR_USB_VBUS_VLD_GPIO_EN_LEN) - 1) << PDS_CR_USB_VBUS_VLD_GPIO_EN_POS)
#define PDS_CR_USB_VBUS_VLD_GPIO_EN_UMSK          (~(((1U << PDS_CR_USB_VBUS_VLD_GPIO_EN_LEN) - 1) << PDS_CR_USB_VBUS_VLD_GPIO_EN_POS))
#define PDS_CR_USB_UCLK_LPM_DIS                   PDS_CR_USB_UCLK_LPM_DIS
#define PDS_CR_USB_UCLK_LPM_DIS_POS               (7U)
#define PDS_CR_USB_UCLK_LPM_DIS_LEN               (1U)
#define PDS_CR_USB_UCLK_LPM_DIS_MSK               (((1U << PDS_CR_USB_UCLK_LPM_DIS_LEN) - 1) << PDS_CR_USB_UCLK_LPM_DIS_POS)
#define PDS_CR_USB_UCLK_LPM_DIS_UMSK              (~(((1U << PDS_CR_USB_UCLK_LPM_DIS_LEN) - 1) << PDS_CR_USB_UCLK_LPM_DIS_POS))
#define PDS_CR_USB_SUSPENDM_SWM                   PDS_CR_USB_SUSPENDM_SWM
#define PDS_CR_USB_SUSPENDM_SWM_POS               (8U)
#define PDS_CR_USB_SUSPENDM_SWM_LEN               (1U)
#define PDS_CR_USB_SUSPENDM_SWM_MSK               (((1U << PDS_CR_USB_SUSPENDM_SWM_LEN) - 1) << PDS_CR_USB_SUSPENDM_SWM_POS)
#define PDS_CR_USB_SUSPENDM_SWM_UMSK              (~(((1U << PDS_CR_USB_SUSPENDM_SWM_LEN) - 1) << PDS_CR_USB_SUSPENDM_SWM_POS))
#define PDS_CR_USB_SUSPENDM                       PDS_CR_USB_SUSPENDM
#define PDS_CR_USB_SUSPENDM_POS                   (9U)
#define PDS_CR_USB_SUSPENDM_LEN                   (1U)
#define PDS_CR_USB_SUSPENDM_MSK                   (((1U << PDS_CR_USB_SUSPENDM_LEN) - 1) << PDS_CR_USB_SUSPENDM_POS)
#define PDS_CR_USB_SUSPENDM_UMSK                  (~(((1U << PDS_CR_USB_SUSPENDM_LEN) - 1) << PDS_CR_USB_SUSPENDM_POS))
#define PDS_REG_PU_USB20_PSW                      PDS_REG_PU_USB20_PSW
#define PDS_REG_PU_USB20_PSW_POS                  (31U)
#define PDS_REG_PU_USB20_PSW_LEN                  (1U)
#define PDS_REG_PU_USB20_PSW_MSK                  (((1U << PDS_REG_PU_USB20_PSW_LEN) - 1) << PDS_REG_PU_USB20_PSW_POS)
#define PDS_REG_PU_USB20_PSW_UMSK                 (~(((1U << PDS_REG_PU_USB20_PSW_LEN) - 1) << PDS_REG_PU_USB20_PSW_POS))

/* 0x504 : u2phy_0 */
#define PDS_U2PHY_0_OFFSET                        (0x504)
#define PDS_PLLPTUNE                              PDS_PLLPTUNE
#define PDS_PLLPTUNE_POS                          (0U)
#define PDS_PLLPTUNE_LEN                          (4U)
#define PDS_PLLPTUNE_MSK                          (((1U << PDS_PLLPTUNE_LEN) - 1) << PDS_PLLPTUNE_POS)
#define PDS_PLLPTUNE_UMSK                         (~(((1U << PDS_PLLPTUNE_LEN) - 1) << PDS_PLLPTUNE_POS))
#define PDS_PLLITUNE                              PDS_PLLITUNE
#define PDS_PLLITUNE_POS                          (4U)
#define PDS_PLLITUNE_LEN                          (2U)
#define PDS_PLLITUNE_MSK                          (((1U << PDS_PLLITUNE_LEN) - 1) << PDS_PLLITUNE_POS)
#define PDS_PLLITUNE_UMSK                         (~(((1U << PDS_PLLITUNE_LEN) - 1) << PDS_PLLITUNE_POS))
#define PDS_PLLBTUNE                              PDS_PLLBTUNE
#define PDS_PLLBTUNE_POS                          (6U)
#define PDS_PLLBTUNE_LEN                          (1U)
#define PDS_PLLBTUNE_MSK                          (((1U << PDS_PLLBTUNE_LEN) - 1) << PDS_PLLBTUNE_POS)
#define PDS_PLLBTUNE_UMSK                         (~(((1U << PDS_PLLBTUNE_LEN) - 1) << PDS_PLLBTUNE_POS))
#define PDS_OTGTUNE0                              PDS_OTGTUNE0
#define PDS_OTGTUNE0_POS                          (7U)
#define PDS_OTGTUNE0_LEN                          (3U)
#define PDS_OTGTUNE0_MSK                          (((1U << PDS_OTGTUNE0_LEN) - 1) << PDS_OTGTUNE0_POS)
#define PDS_OTGTUNE0_UMSK                         (~(((1U << PDS_OTGTUNE0_LEN) - 1) << PDS_OTGTUNE0_POS))
#define PDS_OTGDISABLE0                           PDS_OTGDISABLE0
#define PDS_OTGDISABLE0_POS                       (10U)
#define PDS_OTGDISABLE0_LEN                       (1U)
#define PDS_OTGDISABLE0_MSK                       (((1U << PDS_OTGDISABLE0_LEN) - 1) << PDS_OTGDISABLE0_POS)
#define PDS_OTGDISABLE0_UMSK                      (~(((1U << PDS_OTGDISABLE0_LEN) - 1) << PDS_OTGDISABLE0_POS))
#define PDS_LOOPBACKENB0                          PDS_LOOPBACKENB0
#define PDS_LOOPBACKENB0_POS                      (11U)
#define PDS_LOOPBACKENB0_LEN                      (1U)
#define PDS_LOOPBACKENB0_MSK                      (((1U << PDS_LOOPBACKENB0_LEN) - 1) << PDS_LOOPBACKENB0_POS)
#define PDS_LOOPBACKENB0_UMSK                     (~(((1U << PDS_LOOPBACKENB0_LEN) - 1) << PDS_LOOPBACKENB0_POS))
#define PDS_IDPULLUP0                             PDS_IDPULLUP0
#define PDS_IDPULLUP0_POS                         (12U)
#define PDS_IDPULLUP0_LEN                         (1U)
#define PDS_IDPULLUP0_MSK                         (((1U << PDS_IDPULLUP0_LEN) - 1) << PDS_IDPULLUP0_POS)
#define PDS_IDPULLUP0_UMSK                        (~(((1U << PDS_IDPULLUP0_LEN) - 1) << PDS_IDPULLUP0_POS))
#define PDS_HSXCVREXTCTL0                         PDS_HSXCVREXTCTL0
#define PDS_HSXCVREXTCTL0_POS                     (13U)
#define PDS_HSXCVREXTCTL0_LEN                     (1U)
#define PDS_HSXCVREXTCTL0_MSK                     (((1U << PDS_HSXCVREXTCTL0_LEN) - 1) << PDS_HSXCVREXTCTL0_POS)
#define PDS_HSXCVREXTCTL0_UMSK                    (~(((1U << PDS_HSXCVREXTCTL0_LEN) - 1) << PDS_HSXCVREXTCTL0_POS))
#define PDS_FSXCVROWNER0                          PDS_FSXCVROWNER0
#define PDS_FSXCVROWNER0_POS                      (14U)
#define PDS_FSXCVROWNER0_LEN                      (1U)
#define PDS_FSXCVROWNER0_MSK                      (((1U << PDS_FSXCVROWNER0_LEN) - 1) << PDS_FSXCVROWNER0_POS)
#define PDS_FSXCVROWNER0_UMSK                     (~(((1U << PDS_FSXCVROWNER0_LEN) - 1) << PDS_FSXCVROWNER0_POS))
#define PDS_FSSE0EXT0                             PDS_FSSE0EXT0
#define PDS_FSSE0EXT0_POS                         (15U)
#define PDS_FSSE0EXT0_LEN                         (1U)
#define PDS_FSSE0EXT0_MSK                         (((1U << PDS_FSSE0EXT0_LEN) - 1) << PDS_FSSE0EXT0_POS)
#define PDS_FSSE0EXT0_UMSK                        (~(((1U << PDS_FSSE0EXT0_LEN) - 1) << PDS_FSSE0EXT0_POS))
#define PDS_FSEL                                  PDS_FSEL
#define PDS_FSEL_POS                              (16U)
#define PDS_FSEL_LEN                              (3U)
#define PDS_FSEL_MSK                              (((1U << PDS_FSEL_LEN) - 1) << PDS_FSEL_POS)
#define PDS_FSEL_UMSK                             (~(((1U << PDS_FSEL_LEN) - 1) << PDS_FSEL_POS))
#define PDS_DRVVBUS0                              PDS_DRVVBUS0
#define PDS_DRVVBUS0_POS                          (19U)
#define PDS_DRVVBUS0_LEN                          (1U)
#define PDS_DRVVBUS0_MSK                          (((1U << PDS_DRVVBUS0_LEN) - 1) << PDS_DRVVBUS0_POS)
#define PDS_DRVVBUS0_UMSK                         (~(((1U << PDS_DRVVBUS0_LEN) - 1) << PDS_DRVVBUS0_POS))
#define PDS_DCDENB0                               PDS_DCDENB0
#define PDS_DCDENB0_POS                           (20U)
#define PDS_DCDENB0_LEN                           (1U)
#define PDS_DCDENB0_MSK                           (((1U << PDS_DCDENB0_LEN) - 1) << PDS_DCDENB0_POS)
#define PDS_DCDENB0_UMSK                          (~(((1U << PDS_DCDENB0_LEN) - 1) << PDS_DCDENB0_POS))
#define PDS_COMPDISTUNE0                          PDS_COMPDISTUNE0
#define PDS_COMPDISTUNE0_POS                      (21U)
#define PDS_COMPDISTUNE0_LEN                      (3U)
#define PDS_COMPDISTUNE0_MSK                      (((1U << PDS_COMPDISTUNE0_LEN) - 1) << PDS_COMPDISTUNE0_POS)
#define PDS_COMPDISTUNE0_UMSK                     (~(((1U << PDS_COMPDISTUNE0_LEN) - 1) << PDS_COMPDISTUNE0_POS))
#define PDS_COMMONONN                             PDS_COMMONONN
#define PDS_COMMONONN_POS                         (24U)
#define PDS_COMMONONN_LEN                         (1U)
#define PDS_COMMONONN_MSK                         (((1U << PDS_COMMONONN_LEN) - 1) << PDS_COMMONONN_POS)
#define PDS_COMMONONN_UMSK                        (~(((1U << PDS_COMMONONN_LEN) - 1) << PDS_COMMONONN_POS))
#define PDS_CHRGSEL0                              PDS_CHRGSEL0
#define PDS_CHRGSEL0_POS                          (25U)
#define PDS_CHRGSEL0_LEN                          (1U)
#define PDS_CHRGSEL0_MSK                          (((1U << PDS_CHRGSEL0_LEN) - 1) << PDS_CHRGSEL0_POS)
#define PDS_CHRGSEL0_UMSK                         (~(((1U << PDS_CHRGSEL0_LEN) - 1) << PDS_CHRGSEL0_POS))
#define PDS_BYPASSSEL0                            PDS_BYPASSSEL0
#define PDS_BYPASSSEL0_POS                        (26U)
#define PDS_BYPASSSEL0_LEN                        (1U)
#define PDS_BYPASSSEL0_MSK                        (((1U << PDS_BYPASSSEL0_LEN) - 1) << PDS_BYPASSSEL0_POS)
#define PDS_BYPASSSEL0_UMSK                       (~(((1U << PDS_BYPASSSEL0_LEN) - 1) << PDS_BYPASSSEL0_POS))
#define PDS_BYPASSDPEN0                           PDS_BYPASSDPEN0
#define PDS_BYPASSDPEN0_POS                       (27U)
#define PDS_BYPASSDPEN0_LEN                       (1U)
#define PDS_BYPASSDPEN0_MSK                       (((1U << PDS_BYPASSDPEN0_LEN) - 1) << PDS_BYPASSDPEN0_POS)
#define PDS_BYPASSDPEN0_UMSK                      (~(((1U << PDS_BYPASSDPEN0_LEN) - 1) << PDS_BYPASSDPEN0_POS))
#define PDS_BYPASSDPDATA0                         PDS_BYPASSDPDATA0
#define PDS_BYPASSDPDATA0_POS                     (28U)
#define PDS_BYPASSDPDATA0_LEN                     (1U)
#define PDS_BYPASSDPDATA0_MSK                     (((1U << PDS_BYPASSDPDATA0_LEN) - 1) << PDS_BYPASSDPDATA0_POS)
#define PDS_BYPASSDPDATA0_UMSK                    (~(((1U << PDS_BYPASSDPDATA0_LEN) - 1) << PDS_BYPASSDPDATA0_POS))
#define PDS_BYPASSDMEN0                           PDS_BYPASSDMEN0
#define PDS_BYPASSDMEN0_POS                       (29U)
#define PDS_BYPASSDMEN0_LEN                       (1U)
#define PDS_BYPASSDMEN0_MSK                       (((1U << PDS_BYPASSDMEN0_LEN) - 1) << PDS_BYPASSDMEN0_POS)
#define PDS_BYPASSDMEN0_UMSK                      (~(((1U << PDS_BYPASSDMEN0_LEN) - 1) << PDS_BYPASSDMEN0_POS))
#define PDS_BYPASSDMDATA0                         PDS_BYPASSDMDATA0
#define PDS_BYPASSDMDATA0_POS                     (30U)
#define PDS_BYPASSDMDATA0_LEN                     (1U)
#define PDS_BYPASSDMDATA0_MSK                     (((1U << PDS_BYPASSDMDATA0_LEN) - 1) << PDS_BYPASSDMDATA0_POS)
#define PDS_BYPASSDMDATA0_UMSK                    (~(((1U << PDS_BYPASSDMDATA0_LEN) - 1) << PDS_BYPASSDMDATA0_POS))
#define PDS_ATERESET                              PDS_ATERESET
#define PDS_ATERESET_POS                          (31U)
#define PDS_ATERESET_LEN                          (1U)
#define PDS_ATERESET_MSK                          (((1U << PDS_ATERESET_LEN) - 1) << PDS_ATERESET_POS)
#define PDS_ATERESET_UMSK                         (~(((1U << PDS_ATERESET_LEN) - 1) << PDS_ATERESET_POS))

/* 0x508 : u2phy_1 */
#define PDS_U2PHY_1_OFFSET                        (0x508)
#define PDS_TXHSXVTUNE0                           PDS_TXHSXVTUNE0
#define PDS_TXHSXVTUNE0_POS                       (1U)
#define PDS_TXHSXVTUNE0_LEN                       (2U)
#define PDS_TXHSXVTUNE0_MSK                       (((1U << PDS_TXHSXVTUNE0_LEN) - 1) << PDS_TXHSXVTUNE0_POS)
#define PDS_TXHSXVTUNE0_UMSK                      (~(((1U << PDS_TXHSXVTUNE0_LEN) - 1) << PDS_TXHSXVTUNE0_POS))
#define PDS_TXFSLSTUNE0                           PDS_TXFSLSTUNE0
#define PDS_TXFSLSTUNE0_POS                       (3U)
#define PDS_TXFSLSTUNE0_LEN                       (4U)
#define PDS_TXFSLSTUNE0_MSK                       (((1U << PDS_TXFSLSTUNE0_LEN) - 1) << PDS_TXFSLSTUNE0_POS)
#define PDS_TXFSLSTUNE0_UMSK                      (~(((1U << PDS_TXFSLSTUNE0_LEN) - 1) << PDS_TXFSLSTUNE0_POS))
#define PDS_TXENABLEN0                            PDS_TXENABLEN0
#define PDS_TXENABLEN0_POS                        (7U)
#define PDS_TXENABLEN0_LEN                        (1U)
#define PDS_TXENABLEN0_MSK                        (((1U << PDS_TXENABLEN0_LEN) - 1) << PDS_TXENABLEN0_POS)
#define PDS_TXENABLEN0_UMSK                       (~(((1U << PDS_TXENABLEN0_LEN) - 1) << PDS_TXENABLEN0_POS))
#define PDS_TESTDATAOUTSEL0                       PDS_TESTDATAOUTSEL0
#define PDS_TESTDATAOUTSEL0_POS                   (8U)
#define PDS_TESTDATAOUTSEL0_LEN                   (1U)
#define PDS_TESTDATAOUTSEL0_MSK                   (((1U << PDS_TESTDATAOUTSEL0_LEN) - 1) << PDS_TESTDATAOUTSEL0_POS)
#define PDS_TESTDATAOUTSEL0_UMSK                  (~(((1U << PDS_TESTDATAOUTSEL0_LEN) - 1) << PDS_TESTDATAOUTSEL0_POS))
#define PDS_TESTDATAIN0                           PDS_TESTDATAIN0
#define PDS_TESTDATAIN0_POS                       (9U)
#define PDS_TESTDATAIN0_LEN                       (8U)
#define PDS_TESTDATAIN0_MSK                       (((1U << PDS_TESTDATAIN0_LEN) - 1) << PDS_TESTDATAIN0_POS)
#define PDS_TESTDATAIN0_UMSK                      (~(((1U << PDS_TESTDATAIN0_LEN) - 1) << PDS_TESTDATAIN0_POS))
#define PDS_TESTCLK0                              PDS_TESTCLK0
#define PDS_TESTCLK0_POS                          (17U)
#define PDS_TESTCLK0_LEN                          (1U)
#define PDS_TESTCLK0_MSK                          (((1U << PDS_TESTCLK0_LEN) - 1) << PDS_TESTCLK0_POS)
#define PDS_TESTCLK0_UMSK                         (~(((1U << PDS_TESTCLK0_LEN) - 1) << PDS_TESTCLK0_POS))
#define PDS_TESTBURNIN                            PDS_TESTBURNIN
#define PDS_TESTBURNIN_POS                        (18U)
#define PDS_TESTBURNIN_LEN                        (1U)
#define PDS_TESTBURNIN_MSK                        (((1U << PDS_TESTBURNIN_LEN) - 1) << PDS_TESTBURNIN_POS)
#define PDS_TESTBURNIN_UMSK                       (~(((1U << PDS_TESTBURNIN_LEN) - 1) << PDS_TESTBURNIN_POS))
#define PDS_TESTADDR0                             PDS_TESTADDR0
#define PDS_TESTADDR0_POS                         (19U)
#define PDS_TESTADDR0_LEN                         (4U)
#define PDS_TESTADDR0_MSK                         (((1U << PDS_TESTADDR0_LEN) - 1) << PDS_TESTADDR0_POS)
#define PDS_TESTADDR0_UMSK                        (~(((1U << PDS_TESTADDR0_LEN) - 1) << PDS_TESTADDR0_POS))
#define PDS_SUSPENDM0                             PDS_SUSPENDM0
#define PDS_SUSPENDM0_POS                         (23U)
#define PDS_SUSPENDM0_LEN                         (1U)
#define PDS_SUSPENDM0_MSK                         (((1U << PDS_SUSPENDM0_LEN) - 1) << PDS_SUSPENDM0_POS)
#define PDS_SUSPENDM0_UMSK                        (~(((1U << PDS_SUSPENDM0_LEN) - 1) << PDS_SUSPENDM0_POS))
#define PDS_SQRXTUNE0                             PDS_SQRXTUNE0
#define PDS_SQRXTUNE0_POS                         (24U)
#define PDS_SQRXTUNE0_LEN                         (3U)
#define PDS_SQRXTUNE0_MSK                         (((1U << PDS_SQRXTUNE0_LEN) - 1) << PDS_SQRXTUNE0_POS)
#define PDS_SQRXTUNE0_UMSK                        (~(((1U << PDS_SQRXTUNE0_LEN) - 1) << PDS_SQRXTUNE0_POS))
#define PDS_SLEEPM0                               PDS_SLEEPM0
#define PDS_SLEEPM0_POS                           (27U)
#define PDS_SLEEPM0_LEN                           (1U)
#define PDS_SLEEPM0_MSK                           (((1U << PDS_SLEEPM0_LEN) - 1) << PDS_SLEEPM0_POS)
#define PDS_SLEEPM0_UMSK                          (~(((1U << PDS_SLEEPM0_LEN) - 1) << PDS_SLEEPM0_POS))
#define PDS_SIDDQ                                 PDS_SIDDQ
#define PDS_SIDDQ_POS                             (28U)
#define PDS_SIDDQ_LEN                             (1U)
#define PDS_SIDDQ_MSK                             (((1U << PDS_SIDDQ_LEN) - 1) << PDS_SIDDQ_POS)
#define PDS_SIDDQ_UMSK                            (~(((1U << PDS_SIDDQ_LEN) - 1) << PDS_SIDDQ_POS))
#define PDS_RESREQIN                              PDS_RESREQIN
#define PDS_RESREQIN_POS                          (29U)
#define PDS_RESREQIN_LEN                          (1U)
#define PDS_RESREQIN_MSK                          (((1U << PDS_RESREQIN_LEN) - 1) << PDS_RESREQIN_POS)
#define PDS_RESREQIN_UMSK                         (~(((1U << PDS_RESREQIN_LEN) - 1) << PDS_RESREQIN_POS))
#define PDS_REFCLKSEL                             PDS_REFCLKSEL
#define PDS_REFCLKSEL_POS                         (30U)
#define PDS_REFCLKSEL_LEN                         (2U)
#define PDS_REFCLKSEL_MSK                         (((1U << PDS_REFCLKSEL_LEN) - 1) << PDS_REFCLKSEL_POS)
#define PDS_REFCLKSEL_UMSK                        (~(((1U << PDS_REFCLKSEL_LEN) - 1) << PDS_REFCLKSEL_POS))

/* 0x50C : u2phy_2 */
#define PDS_U2PHY_2_OFFSET                        (0x50C)
#define PDS_USB20_RREF_HIZ                        PDS_USB20_RREF_HIZ
#define PDS_USB20_RREF_HIZ_POS                    (3U)
#define PDS_USB20_RREF_HIZ_LEN                    (1U)
#define PDS_USB20_RREF_HIZ_MSK                    (((1U << PDS_USB20_RREF_HIZ_LEN) - 1) << PDS_USB20_RREF_HIZ_POS)
#define PDS_USB20_RREF_HIZ_UMSK                   (~(((1U << PDS_USB20_RREF_HIZ_LEN) - 1) << PDS_USB20_RREF_HIZ_POS))
#define PDS_USB20_RREF_CAL_VOUT_EN                PDS_USB20_RREF_CAL_VOUT_EN
#define PDS_USB20_RREF_CAL_VOUT_EN_POS            (4U)
#define PDS_USB20_RREF_CAL_VOUT_EN_LEN            (1U)
#define PDS_USB20_RREF_CAL_VOUT_EN_MSK            (((1U << PDS_USB20_RREF_CAL_VOUT_EN_LEN) - 1) << PDS_USB20_RREF_CAL_VOUT_EN_POS)
#define PDS_USB20_RREF_CAL_VOUT_EN_UMSK           (~(((1U << PDS_USB20_RREF_CAL_VOUT_EN_LEN) - 1) << PDS_USB20_RREF_CAL_VOUT_EN_POS))
#define PDS_USB20_RREF_CAL_IIN_EN                 PDS_USB20_RREF_CAL_IIN_EN
#define PDS_USB20_RREF_CAL_IIN_EN_POS             (5U)
#define PDS_USB20_RREF_CAL_IIN_EN_LEN             (1U)
#define PDS_USB20_RREF_CAL_IIN_EN_MSK             (((1U << PDS_USB20_RREF_CAL_IIN_EN_LEN) - 1) << PDS_USB20_RREF_CAL_IIN_EN_POS)
#define PDS_USB20_RREF_CAL_IIN_EN_UMSK            (~(((1U << PDS_USB20_RREF_CAL_IIN_EN_LEN) - 1) << PDS_USB20_RREF_CAL_IIN_EN_POS))
#define PDS_USB20_RCAL_CODE                       PDS_USB20_RCAL_CODE
#define PDS_USB20_RCAL_CODE_POS                   (6U)
#define PDS_USB20_RCAL_CODE_LEN                   (5U)
#define PDS_USB20_RCAL_CODE_MSK                   (((1U << PDS_USB20_RCAL_CODE_LEN) - 1) << PDS_USB20_RCAL_CODE_POS)
#define PDS_USB20_RCAL_CODE_UMSK                  (~(((1U << PDS_USB20_RCAL_CODE_LEN) - 1) << PDS_USB20_RCAL_CODE_POS))
#define PDS_USB20_RCAL_COARSE                     PDS_USB20_RCAL_COARSE
#define PDS_USB20_RCAL_COARSE_POS                 (11U)
#define PDS_USB20_RCAL_COARSE_LEN                 (2U)
#define PDS_USB20_RCAL_COARSE_MSK                 (((1U << PDS_USB20_RCAL_COARSE_LEN) - 1) << PDS_USB20_RCAL_COARSE_POS)
#define PDS_USB20_RCAL_COARSE_UMSK                (~(((1U << PDS_USB20_RCAL_COARSE_LEN) - 1) << PDS_USB20_RCAL_COARSE_POS))
#define PDS_VBUSROUDET                            PDS_VBUSROUDET
#define PDS_VBUSROUDET_POS                        (13U)
#define PDS_VBUSROUDET_LEN                        (1U)
#define PDS_VBUSROUDET_MSK                        (((1U << PDS_VBUSROUDET_LEN) - 1) << PDS_VBUSROUDET_POS)
#define PDS_VBUSROUDET_UMSK                       (~(((1U << PDS_VBUSROUDET_LEN) - 1) << PDS_VBUSROUDET_POS))
#define PDS_VDATSRCENB0                           PDS_VDATSRCENB0
#define PDS_VDATSRCENB0_POS                       (14U)
#define PDS_VDATSRCENB0_LEN                       (1U)
#define PDS_VDATSRCENB0_MSK                       (((1U << PDS_VDATSRCENB0_LEN) - 1) << PDS_VDATSRCENB0_POS)
#define PDS_VDATSRCENB0_UMSK                      (~(((1U << PDS_VDATSRCENB0_LEN) - 1) << PDS_VDATSRCENB0_POS))
#define PDS_VDATREFTUNE0                          PDS_VDATREFTUNE0
#define PDS_VDATREFTUNE0_POS                      (15U)
#define PDS_VDATREFTUNE0_LEN                      (2U)
#define PDS_VDATREFTUNE0_MSK                      (((1U << PDS_VDATREFTUNE0_LEN) - 1) << PDS_VDATREFTUNE0_POS)
#define PDS_VDATREFTUNE0_UMSK                     (~(((1U << PDS_VDATREFTUNE0_LEN) - 1) << PDS_VDATREFTUNE0_POS))
#define PDS_VDATDETENB0                           PDS_VDATDETENB0
#define PDS_VDATDETENB0_POS                       (17U)
#define PDS_VDATDETENB0_LEN                       (1U)
#define PDS_VDATDETENB0_MSK                       (((1U << PDS_VDATDETENB0_LEN) - 1) << PDS_VDATDETENB0_POS)
#define PDS_VDATDETENB0_UMSK                      (~(((1U << PDS_VDATDETENB0_LEN) - 1) << PDS_VDATDETENB0_POS))
#define PDS_VBUSVLDEXTSEL0                        PDS_VBUSVLDEXTSEL0
#define PDS_VBUSVLDEXTSEL0_POS                    (18U)
#define PDS_VBUSVLDEXTSEL0_LEN                    (1U)
#define PDS_VBUSVLDEXTSEL0_MSK                    (((1U << PDS_VBUSVLDEXTSEL0_LEN) - 1) << PDS_VBUSVLDEXTSEL0_POS)
#define PDS_VBUSVLDEXTSEL0_UMSK                   (~(((1U << PDS_VBUSVLDEXTSEL0_LEN) - 1) << PDS_VBUSVLDEXTSEL0_POS))
#define PDS_VATESTENB                             PDS_VATESTENB
#define PDS_VATESTENB_POS                         (19U)
#define PDS_VATESTENB_LEN                         (2U)
#define PDS_VATESTENB_MSK                         (((1U << PDS_VATESTENB_LEN) - 1) << PDS_VATESTENB_POS)
#define PDS_VATESTENB_UMSK                        (~(((1U << PDS_VATESTENB_LEN) - 1) << PDS_VATESTENB_POS))
#define PDS_TXVREFTUNE0                           PDS_TXVREFTUNE0
#define PDS_TXVREFTUNE0_POS                       (21U)
#define PDS_TXVREFTUNE0_LEN                       (4U)
#define PDS_TXVREFTUNE0_MSK                       (((1U << PDS_TXVREFTUNE0_LEN) - 1) << PDS_TXVREFTUNE0_POS)
#define PDS_TXVREFTUNE0_UMSK                      (~(((1U << PDS_TXVREFTUNE0_LEN) - 1) << PDS_TXVREFTUNE0_POS))
#define PDS_TXRISETUNE0                           PDS_TXRISETUNE0
#define PDS_TXRISETUNE0_POS                       (25U)
#define PDS_TXRISETUNE0_LEN                       (2U)
#define PDS_TXRISETUNE0_MSK                       (((1U << PDS_TXRISETUNE0_LEN) - 1) << PDS_TXRISETUNE0_POS)
#define PDS_TXRISETUNE0_UMSK                      (~(((1U << PDS_TXRISETUNE0_LEN) - 1) << PDS_TXRISETUNE0_POS))
#define PDS_TXRESTUNE0                            PDS_TXRESTUNE0
#define PDS_TXRESTUNE0_POS                        (27U)
#define PDS_TXRESTUNE0_LEN                        (2U)
#define PDS_TXRESTUNE0_MSK                        (((1U << PDS_TXRESTUNE0_LEN) - 1) << PDS_TXRESTUNE0_POS)
#define PDS_TXRESTUNE0_UMSK                       (~(((1U << PDS_TXRESTUNE0_LEN) - 1) << PDS_TXRESTUNE0_POS))
#define PDS_TXPREEMPPULSETUNE0                    PDS_TXPREEMPPULSETUNE0
#define PDS_TXPREEMPPULSETUNE0_POS                (29U)
#define PDS_TXPREEMPPULSETUNE0_LEN                (1U)
#define PDS_TXPREEMPPULSETUNE0_MSK                (((1U << PDS_TXPREEMPPULSETUNE0_LEN) - 1) << PDS_TXPREEMPPULSETUNE0_POS)
#define PDS_TXPREEMPPULSETUNE0_UMSK               (~(((1U << PDS_TXPREEMPPULSETUNE0_LEN) - 1) << PDS_TXPREEMPPULSETUNE0_POS))
#define PDS_TXPREEMPAMPTUNE0                      PDS_TXPREEMPAMPTUNE0
#define PDS_TXPREEMPAMPTUNE0_POS                  (30U)
#define PDS_TXPREEMPAMPTUNE0_LEN                  (2U)
#define PDS_TXPREEMPAMPTUNE0_MSK                  (((1U << PDS_TXPREEMPAMPTUNE0_LEN) - 1) << PDS_TXPREEMPAMPTUNE0_POS)
#define PDS_TXPREEMPAMPTUNE0_UMSK                 (~(((1U << PDS_TXPREEMPAMPTUNE0_LEN) - 1) << PDS_TXPREEMPAMPTUNE0_POS))

/* 0x510 : u2phy_3 */
#define PDS_U2PHY_3_OFFSET                        (0x510)
#define PDS_TESTDATAOUT0                          PDS_TESTDATAOUT0
#define PDS_TESTDATAOUT0_POS                      (0U)
#define PDS_TESTDATAOUT0_LEN                      (4U)
#define PDS_TESTDATAOUT0_MSK                      (((1U << PDS_TESTDATAOUT0_LEN) - 1) << PDS_TESTDATAOUT0_POS)
#define PDS_TESTDATAOUT0_UMSK                     (~(((1U << PDS_TESTDATAOUT0_LEN) - 1) << PDS_TESTDATAOUT0_POS))
#define PDS_HSSQUELCH0                            PDS_HSSQUELCH0
#define PDS_HSSQUELCH0_POS                        (4U)
#define PDS_HSSQUELCH0_LEN                        (1U)
#define PDS_HSSQUELCH0_MSK                        (((1U << PDS_HSSQUELCH0_LEN) - 1) << PDS_HSSQUELCH0_POS)
#define PDS_HSSQUELCH0_UMSK                       (~(((1U << PDS_HSSQUELCH0_LEN) - 1) << PDS_HSSQUELCH0_POS))
#define PDS_HSRXDAT0                              PDS_HSRXDAT0
#define PDS_HSRXDAT0_POS                          (5U)
#define PDS_HSRXDAT0_LEN                          (1U)
#define PDS_HSRXDAT0_MSK                          (((1U << PDS_HSRXDAT0_LEN) - 1) << PDS_HSRXDAT0_POS)
#define PDS_HSRXDAT0_UMSK                         (~(((1U << PDS_HSRXDAT0_LEN) - 1) << PDS_HSRXDAT0_POS))
#define PDS_VBUSVLDEXT0                           PDS_VBUSVLDEXT0
#define PDS_VBUSVLDEXT0_POS                       (28U)
#define PDS_VBUSVLDEXT0_LEN                       (1U)
#define PDS_VBUSVLDEXT0_MSK                       (((1U << PDS_VBUSVLDEXT0_LEN) - 1) << PDS_VBUSVLDEXT0_POS)
#define PDS_VBUSVLDEXT0_UMSK                      (~(((1U << PDS_VBUSVLDEXT0_LEN) - 1) << PDS_VBUSVLDEXT0_POS))
#define PDS_FSDATAEXT0                            PDS_FSDATAEXT0
#define PDS_FSDATAEXT0_POS                        (29U)
#define PDS_FSDATAEXT0_LEN                        (1U)
#define PDS_FSDATAEXT0_MSK                        (((1U << PDS_FSDATAEXT0_LEN) - 1) << PDS_FSDATAEXT0_POS)
#define PDS_FSDATAEXT0_UMSK                       (~(((1U << PDS_FSDATAEXT0_LEN) - 1) << PDS_FSDATAEXT0_POS))
#define PDS_PORTRESET0                            PDS_PORTRESET0
#define PDS_PORTRESET0_POS                        (30U)
#define PDS_PORTRESET0_LEN                        (1U)
#define PDS_PORTRESET0_MSK                        (((1U << PDS_PORTRESET0_LEN) - 1) << PDS_PORTRESET0_POS)
#define PDS_PORTRESET0_UMSK                       (~(((1U << PDS_PORTRESET0_LEN) - 1) << PDS_PORTRESET0_POS))
#define PDS_POR                                   PDS_POR
#define PDS_POR_POS                               (31U)
#define PDS_POR_LEN                               (1U)
#define PDS_POR_MSK                               (((1U << PDS_POR_LEN) - 1) << PDS_POR_POS)
#define PDS_POR_UMSK                              (~(((1U << PDS_POR_LEN) - 1) << PDS_POR_POS))

struct pds_reg {
    /* 0x0 : PDS_CTL */
    union {
        struct {
            uint32_t pds_start_ps                 : 1; /* [    0],        w1p,        0x0 */
            uint32_t cr_sleep_forever             : 1; /* [    1],        r/w,        0x0 */
            uint32_t cr_xtal_force_off            : 1; /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_wifi_save_state       : 1; /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_pd_ldo09              : 1; /* [    4],        r/w,        0x0 */
            uint32_t cr_pds_pd_bg_sys             : 1; /* [    5],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_gpio_ie_pu_pd    : 1; /* [    6],        r/w,        0x0 */
            uint32_t cr_pds_pd_dcdc12             : 1; /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_gate_clk              : 1; /* [    8],        r/w,        0x1 */
            uint32_t cr_pds_mem_stby              : 1; /* [    9],        r/w,        0x1 */
            uint32_t cr_pds_glb_reg_reset_protect : 1; /* [   10],        r/w,        0x0 */
            uint32_t cr_pds_iso_en                : 1; /* [   11],        r/w,        0x1 */
            uint32_t cr_pds_wait_xtal_rdy         : 1; /* [   12],        r/w,        0x0 */
            uint32_t cr_pds_pwr_off               : 1; /* [   13],        r/w,        0x1 */
            uint32_t cr_pds_pd_xtal               : 1; /* [   14],        r/w,        0x1 */
            uint32_t cr_pds_ctrl_soc_enb          : 1; /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_rst_soc               : 1; /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_rc32m_off_dis         : 1; /* [   17],        r/w,        0x0 */
            uint32_t cr_pds_ldo11_vsel_en         : 1; /* [   18],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_usbpll_pd        : 1; /* [   19],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_aupll_pd         : 1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21                  : 1; /* [   21],       rsvd,        0x0 */
            uint32_t cr_pds_ctrl_wifipll_pd       : 1; /* [   22],        r/w,        0x0 */
            uint32_t cr_pds_ldo11_vol             : 5; /* [27:23],        r/w,        0x8 */
            uint32_t cr_pds_ctrl_rf               : 2; /* [29:28],        r/w,        0x1 */
            uint32_t cr_pds_start_use_tbtt_sleep  : 1; /* [   30],        r/w,        0x0 */
            uint32_t cr_pds_gpio_iso_mode         : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_CTL;

    /* 0x4 : PDS_TIME1 */
    union {
        struct {
            uint32_t cr_sleep_duration : 32; /* [31: 0],        r/w,      0xca8 */
        } BF;
        uint32_t WORD;
    } PDS_TIME1;

    /* 0x8  reserved */
    uint8_t RESERVED0x8[4];

    /* 0xC : PDS_INT */
    union {
        struct {
            uint32_t ro_pds_wake_int                  : 1;  /* [    0],          r,        0x0 */
            uint32_t ro_pds_rf_done_int               : 1;  /* [    1],          r,        0x0 */
            uint32_t ro_pds_wifi_tbtt_sleep_irq       : 1;  /* [    2],          r,        0x0 */
            uint32_t ro_pds_wifi_tbtt_wakeup_irq      : 1;  /* [    3],          r,        0x0 */
            uint32_t cr_pds_wake_int_mask             : 1;  /* [    4],        r/w,        0x0 */
            uint32_t cr_pds_rf_done_int_mask          : 1;  /* [    5],        r/w,        0x0 */
            uint32_t cr_pds_wifi_tbtt_sleep_irq_mask  : 1;  /* [    6],        r/w,        0x0 */
            uint32_t cr_pds_wifi_tbtt_wakeup_irq_mask : 1;  /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_int_clr                   : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                       : 1;  /* [    9],       rsvd,        0x0 */
            uint32_t cr_pds_wakeup_src_en             : 11; /* [20:10],        r/w,      0x7ff */
            uint32_t ro_pds_wakeup_event              : 11; /* [31:21],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_INT;

    /* 0x10 : PDS_CTL2 */
    union {
        struct {
            uint32_t cr_pds_force_np_pwr_off   : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1                : 1;  /* [    1],       rsvd,        0x0 */
            uint32_t cr_pds_force_wb_pwr_off   : 1;  /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_force_usb_pwr_off  : 1;  /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_force_np_iso_en    : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5                : 1;  /* [    5],       rsvd,        0x0 */
            uint32_t cr_pds_force_wb_iso_en    : 1;  /* [    6],        r/w,        0x0 */
            uint32_t cr_pds_force_usb_iso_en   : 1;  /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_force_np_pds_rst   : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                : 1;  /* [    9],       rsvd,        0x0 */
            uint32_t cr_pds_force_wb_pds_rst   : 1;  /* [   10],        r/w,        0x0 */
            uint32_t cr_pds_force_usb_pds_rst  : 1;  /* [   11],        r/w,        0x0 */
            uint32_t cr_pds_force_np_mem_stby  : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13               : 1;  /* [   13],       rsvd,        0x0 */
            uint32_t cr_pds_force_wb_mem_stby  : 1;  /* [   14],        r/w,        0x0 */
            uint32_t cr_pds_force_usb_mem_stby : 1;  /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_force_np_gate_clk  : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17               : 1;  /* [   17],       rsvd,        0x0 */
            uint32_t cr_pds_force_wb_gate_clk  : 1;  /* [   18],        r/w,        0x0 */
            uint32_t cr_pds_force_usb_gate_clk : 1;  /* [   19],        r/w,        0x0 */
            uint32_t reserved_20_31            : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_CTL2;

    /* 0x14 : PDS_CTL3 */
    union {
        struct {
            uint32_t reserved_0                 : 1;  /* [    0],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_pwr_off  : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3               : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_iso_en   : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_6               : 2;  /* [ 6: 5],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_pds_rst  : 1;  /* [    7],        r/w,        0x0 */
            uint32_t reserved_8_9               : 2;  /* [ 9: 8],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_mem_stby : 1;  /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_12             : 2;  /* [12:11],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_gate_clk : 1;  /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_23             : 10; /* [23:14],       rsvd,        0x0 */
            uint32_t cr_pds_np_iso_en           : 1;  /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_26             : 2;  /* [26:25],       rsvd,        0x0 */
            uint32_t cr_pds_wb_iso_en           : 1;  /* [   27],        r/w,        0x1 */
            uint32_t reserved_28                : 1;  /* [   28],       rsvd,        0x0 */
            uint32_t cr_pds_usb_iso_en          : 1;  /* [   29],        r/w,        0x1 */
            uint32_t cr_pds_misc_iso_en         : 1;  /* [   30],        r/w,        0x1 */
            uint32_t reserved_31                : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_CTL3;

    /* 0x18 : PDS_CTL4 */
    union {
        struct {
            uint32_t cr_pds_np_pwr_off    : 1; /* [    0],        r/w,        0x1 */
            uint32_t cr_pds_np_reset      : 1; /* [    1],        r/w,        0x1 */
            uint32_t cr_pds_np_mem_stby   : 1; /* [    2],        r/w,        0x1 */
            uint32_t cr_pds_np_gate_clk   : 1; /* [    3],        r/w,        0x1 */
            uint32_t reserved_4_11        : 8; /* [11: 4],       rsvd,        0x0 */
            uint32_t cr_pds_wb_pwr_off    : 1; /* [   12],        r/w,        0x1 */
            uint32_t cr_pds_wb_reset      : 1; /* [   13],        r/w,        0x1 */
            uint32_t cr_pds_wb_mem_stby   : 1; /* [   14],        r/w,        0x1 */
            uint32_t cr_pds_wb_gate_clk   : 1; /* [   15],        r/w,        0x1 */
            uint32_t reserved_16_19       : 4; /* [19:16],       rsvd,        0x0 */
            uint32_t cr_pds_usb_pwr_off   : 1; /* [   20],        r/w,        0x1 */
            uint32_t cr_pds_usb_reset     : 1; /* [   21],        r/w,        0x1 */
            uint32_t cr_pds_usb_mem_stby  : 1; /* [   22],        r/w,        0x1 */
            uint32_t cr_pds_usb_gate_clk  : 1; /* [   23],        r/w,        0x1 */
            uint32_t cr_pds_misc_pwr_off  : 1; /* [   24],        r/w,        0x1 */
            uint32_t cr_pds_misc_reset    : 1; /* [   25],        r/w,        0x1 */
            uint32_t cr_pds_misc_mem_stby : 1; /* [   26],        r/w,        0x1 */
            uint32_t cr_pds_misc_gate_clk : 1; /* [   27],        r/w,        0x1 */
            uint32_t reserved_28_31       : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_CTL4;

    /* 0x1C : pds_stat */
    union {
        struct {
            uint32_t ro_pds_state        : 5;  /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_7        : 3;  /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ro_pds_rf_state     : 5;  /* [12: 8],          r,        0x0 */
            uint32_t reserved_13_23      : 11; /* [23:13],       rsvd,        0x0 */
            uint32_t pds_reset_event     : 3;  /* [26:24],          r,        0x0 */
            uint32_t reserved_27_30      : 4;  /* [30:27],       rsvd,        0x0 */
            uint32_t pds_clr_reset_event : 1;  /* [   31],        w1c,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_stat;

    /* 0x20 : pds_ram1 */
    union {
        struct {
            uint32_t reserved_0_7             : 8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_pds_ram_clk_cnt       : 6; /* [13: 8],        r/w,        0x8 */
            uint32_t reserved_14_15           : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_pds_ram_clk2_cnt      : 6; /* [21:16],        r/w,       0x18 */
            uint32_t reserved_22_23           : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t cr_pds_ctrl_np_ram_clk   : 1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25              : 1; /* [   25],       rsvd,        0x0 */
            uint32_t cr_pds_ctrl_wb_ram_clk   : 1; /* [   26],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_usb_ram_clk  : 1; /* [   27],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_misc_ram_clk : 1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29              : 1; /* [   29],       rsvd,        0x0 */
            uint32_t cr_pds_ctrl_ram_clk2     : 1; /* [   30],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_ram_clk      : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_ram1;

    /* 0x24 : PDS_CTL5 */
    union {
        struct {
            uint32_t cr_main_mcu_wfi_mask : 1;  /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_pad_od_en     : 1;  /* [    1],        r/w,        0x0 */
            uint32_t cr_wl_mcu_wfi_mask   : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3           : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t cr_lpcpu_wfi_mask     : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7         : 3;  /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cr_pds_ctrl_usb33    : 1;  /* [    8],        r/w,        0x0 */
            uint32_t cr_pds_pd_ldo18io    : 1;  /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_15       : 6;  /* [15:10],       rsvd,        0x0 */
            uint32_t cr_pds_gpio_keep_en  : 3;  /* [18:16],        r/w,        0x7 */
            uint32_t cr_core_gpio_ls1p8   : 3;  /* [21:19],        r/w,        0x0 */
            uint32_t reserved_22_31       : 10; /* [31:22],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_CTL5;

    /* 0x28 : PDS_RAM2 */
    union {
        struct {
            uint32_t cr_wram_slp    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t cr_wram_ret    : 10; /* [19:10],        r/w,        0x0 */
            uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_RAM2;

    /* 0x2c  reserved */
    uint8_t RESERVED0x2c[4];

    /* 0x30 : pds_gpio_i_set */
    union {
        struct {
            uint32_t cr_pds_gpio_ie_set : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2         : 1;  /* [    2],       rsvd,        0x0 */
            uint32_t cr_pds_gpio_pd_set : 2;  /* [ 4: 3],        r/w,        0x0 */
            uint32_t reserved_5         : 1;  /* [    5],       rsvd,        0x0 */
            uint32_t cr_pds_gpio_pu_set : 2;  /* [ 7: 6],        r/w,        0x0 */
            uint32_t reserved_8_31      : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_i_set;

    /* 0x34 : pds_gpio_pd_set */
    union {
        struct {
            uint32_t cr_pds_gpio_set_int_mask1 : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } pds_gpio_pd_set;

    /* 0x38  reserved */
    uint8_t RESERVED0x38[8];

    /* 0x40 : pds_gpio_int */
    union {
        struct {
            uint32_t reserved_0_1           : 2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t pds_gpio_set1_int_clr  : 1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3             : 1; /* [    3],       rsvd,        0x0 */
            uint32_t pds_gpio_set1_int_mode : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reserved_8_9           : 2; /* [ 9: 8],       rsvd,        0x0 */
            uint32_t pds_gpio_set2_int_clr  : 1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11            : 1; /* [   11],       rsvd,        0x0 */
            uint32_t pds_gpio_set2_int_mode : 4; /* [15:12],        r/w,        0x0 */
            uint32_t reserved_16_17         : 2; /* [17:16],       rsvd,        0x0 */
            uint32_t pds_gpio_set3_int_clr  : 1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19            : 1; /* [   19],       rsvd,        0x0 */
            uint32_t pds_gpio_set3_int_mode : 4; /* [23:20],        r/w,        0x0 */
            uint32_t reserved_24_25         : 2; /* [25:24],       rsvd,        0x0 */
            uint32_t pds_gpio_set4_int_clr  : 1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27            : 1; /* [   27],       rsvd,        0x0 */
            uint32_t pds_gpio_set4_int_mode : 4; /* [31:28],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_int;

    /* 0x44 : pds_gpio_stat */
    union {
        struct {
            uint32_t pds_gpio_int_stat1 : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_stat;

    /* 0x48 : PDS_RAM3 */
    union {
        struct {
            uint32_t cr_ocram_ret   : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_RAM3;

    /* 0x4C : PDS_RAM4 */
    union {
        struct {
            uint32_t cr_ocram_slp   : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_RAM4;

    /* 0x50 : XTAL_CNT_32K */
    union {
        struct {
            uint32_t ro_xtal_cnt_32k_res      : 6;  /* [ 5: 0],          r,       0x24 */
            uint32_t ro_xtal_cnt_32k_cnt      : 11; /* [16: 6],          r,      0x3d0 */
            uint32_t reg_total_32k_cycle      : 3;  /* [19:17],        r/w,        0x0 */
            uint32_t cr_pds_xtal_cnt_rc32k_en : 1;  /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_28           : 8;  /* [28:21],       rsvd,        0x0 */
            uint32_t xtal_cnt_32k_process     : 1;  /* [   29],          r,        0x0 */
            uint32_t xtal_cnt_32k_done        : 1;  /* [   30],          r,        0x0 */
            uint32_t reserved_31              : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } XTAL_CNT_32K;

    /* 0x54 : KYD_ROW_CFG */
    union {
        struct {
            uint32_t reserved_0_28      : 29; /* [28: 0],       rsvd,        0x0 */
            uint32_t cfg_row_detect_cnt : 3;  /* [31:29],        r/w,        0x4 */
        } BF;
        uint32_t WORD;
    } KYD_ROW_CFG;

    /* 0x58 : KYD_CTL */
    union {
        struct {
            uint32_t cr_pds_kyd_en               : 1; /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_kyd_col_o_val        : 1; /* [    1],        r/w,        0x0 */
            uint32_t cr_pds_kyd_row_matrix_size  : 3; /* [ 4: 2],        r/w,        0x7 */
            uint32_t cr_pds_kyd_col_matrix_size  : 3; /* [ 7: 5],        r/w,        0x7 */
            uint32_t cr_pds_kyd_row_i_pull_en    : 1; /* [    8],        r/w,        0x0 */
            uint32_t ro_pds_kyd_wakeup           : 1; /* [    9],          r,        0x0 */
            uint32_t clr_pds_kyd_wakeup          : 1; /* [   10],        w1p,        0x0 */
            uint32_t reserved_11                 : 1; /* [   11],       rsvd,        0x0 */
            uint32_t ro_pds_kyd_wakeup_row_gpio  : 6; /* [17:12],          r,        0x0 */
            uint32_t ro_pds_kyd_wakeup_row_index : 3; /* [20:18],          r,        0x0 */
            uint32_t ro_pds_kyd_wakeup_col_index : 3; /* [23:21],          r,        0x0 */
            uint32_t ro_pds_kyd_wakeup_col_gpio  : 6; /* [29:24],          r,        0x0 */
            uint32_t reserved_30_31              : 2; /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } KYD_CTL;

    /* 0x5C : KYD_WHITE_SET */
    union {
        struct {
            uint32_t cfg_white_key0_row_index : 3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t cfg_white_key0_col_index : 3; /* [ 5: 3],        r/w,        0x0 */
            uint32_t cfg_white_key0_kyd_mode  : 2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t cfg_white_key1_row_index : 3; /* [10: 8],        r/w,        0x0 */
            uint32_t cfg_white_key1_col_index : 3; /* [13:11],        r/w,        0x0 */
            uint32_t cfg_white_key1_kyd_mode  : 2; /* [15:14],        r/w,        0x0 */
            uint32_t cfg_white_key2_row_index : 3; /* [18:16],        r/w,        0x0 */
            uint32_t cfg_white_key2_col_index : 3; /* [21:19],        r/w,        0x0 */
            uint32_t cfg_white_key2_kyd_mode  : 2; /* [23:22],        r/w,        0x0 */
            uint32_t cfg_white_key3_row_index : 3; /* [26:24],        r/w,        0x0 */
            uint32_t cfg_white_key3_col_index : 3; /* [29:27],        r/w,        0x0 */
            uint32_t cfg_white_key3_kyd_mode  : 2; /* [31:30],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } KYD_WHITE_SET;

    /* 0x60 : KYS_ROW_I_GPIO_SEL0 */
    union {
        struct {
            uint32_t cfg_key_row_i_0_gpio_sel : 6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6_7             : 2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_1_gpio_sel : 6; /* [13: 8],        r/w,        0x1 */
            uint32_t reserved_14_15           : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_2_gpio_sel : 6; /* [21:16],        r/w,        0x2 */
            uint32_t reserved_22_23           : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_3_gpio_sel : 6; /* [29:24],        r/w,        0x3 */
            uint32_t reserved_30_31           : 2; /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } KYS_ROW_I_GPIO_SEL0;

    /* 0x64 : KYS_ROW_I_GPIO_SEL1 */
    union {
        struct {
            uint32_t cfg_key_row_i_4_gpio_sel : 6; /* [ 5: 0],        r/w,        0x4 */
            uint32_t reserved_6_7             : 2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_5_gpio_sel : 6; /* [13: 8],        r/w,        0x5 */
            uint32_t reserved_14_15           : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_6_gpio_sel : 6; /* [21:16],        r/w,        0x6 */
            uint32_t reserved_22_23           : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_7_gpio_sel : 6; /* [29:24],        r/w,        0x7 */
            uint32_t reserved_30_31           : 2; /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } KYS_ROW_I_GPIO_SEL1;

    /* 0x68 : KYS_COL_O_GPIO_SEL0 */
    union {
        struct {
            uint32_t cfg_key_col_o_0_gpio_sel : 6; /* [ 5: 0],        r/w,        0x8 */
            uint32_t reserved_6_7             : 2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_1_gpio_sel : 6; /* [13: 8],        r/w,        0x9 */
            uint32_t reserved_14_15           : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_2_gpio_sel : 6; /* [21:16],        r/w,        0xa */
            uint32_t reserved_22_23           : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_3_gpio_sel : 6; /* [29:24],        r/w,        0xb */
            uint32_t reserved_30_31           : 2; /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } KYS_COL_O_GPIO_SEL0;

    /* 0x6C : KYS_COL_O_GPIO_SEL1 */
    union {
        struct {
            uint32_t cfg_key_col_o_4_gpio_sel : 6; /* [ 5: 0],        r/w,        0xc */
            uint32_t reserved_6_7             : 2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_5_gpio_sel : 6; /* [13: 8],        r/w,        0xd */
            uint32_t reserved_14_15           : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_6_gpio_sel : 6; /* [21:16],        r/w,        0xe */
            uint32_t reserved_22_23           : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_7_gpio_sel : 6; /* [29:24],        r/w,        0xf */
            uint32_t reserved_30_31           : 2; /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } KYS_COL_O_GPIO_SEL1;

    /* 0x70 : pds_gpio_pd_0 */
    union {
        struct {
            uint32_t cr_pds_gpio_pd_0 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_pd_0;

    /* 0x74 : pds_gpio_pd_1 */
    union {
        struct {
            uint32_t cr_pds_gpio_pd_1 : 21; /* [20: 0],        r/w,        0x0 */
            uint32_t reserved_21_31   : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_pd_1;

    /* 0x78 : pds_gpio_pu_0 */
    union {
        struct {
            uint32_t cr_pds_gpio_pu_0 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_pu_0;

    /* 0x7C : pds_gpio_pu_1 */
    union {
        struct {
            uint32_t cr_pds_gpio_pu_1 : 21; /* [20: 0],        r/w,        0x0 */
            uint32_t reserved_21_31   : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_pu_1;

    /* 0x80 : pds_gpio_ie_0 */
    union {
        struct {
            uint32_t cr_pds_gpio_ie_0 : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_ie_0;

    /* 0x84 : pds_gpio_ie_1 */
    union {
        struct {
            uint32_t cr_pds_gpio_ie_1 : 21; /* [20: 0],        r/w,        0x0 */
            uint32_t reserved_21_31   : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_ie_1;

    /* 0x88 : pds_gpio_pd_set2 */
    union {
        struct {
            uint32_t cr_pds_gpio_set_int_mask2 : 21; /* [20: 0],        r/w,   0x1fffff */
            uint32_t reserved_21_31            : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_pd_set2;

    /* 0x8C : pds_gpio_int2 */
    union {
        struct {
            uint32_t reserved_0_1           : 2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t pds_gpio_set5_int_clr  : 1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3             : 1; /* [    3],       rsvd,        0x0 */
            uint32_t pds_gpio_set5_int_mode : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reserved_8_9           : 2; /* [ 9: 8],       rsvd,        0x0 */
            uint32_t pds_gpio_set6_int_clr  : 1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11            : 1; /* [   11],       rsvd,        0x0 */
            uint32_t pds_gpio_set6_int_mode : 4; /* [15:12],        r/w,        0x0 */
            uint32_t reserved_16_17         : 2; /* [17:16],       rsvd,        0x0 */
            uint32_t pds_gpio_set7_int_clr  : 1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19            : 1; /* [   19],       rsvd,        0x0 */
            uint32_t pds_gpio_set7_int_mode : 4; /* [23:20],        r/w,        0x0 */
            uint32_t reserved_24_25         : 2; /* [25:24],       rsvd,        0x0 */
            uint32_t pds_gpio_set8_int_clr  : 1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27            : 1; /* [   27],       rsvd,        0x0 */
            uint32_t pds_gpio_set8_int_mode : 4; /* [31:28],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_int2;

    /* 0x90 : pds_gpio_int_stat2 */
    union {
        struct {
            uint32_t pds_gpio_int_stat2 : 21; /* [20: 0],          r,        0x0 */
            uint32_t reserved_21_31     : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_int_stat2;

    /* 0x94  reserved */
    uint8_t RESERVED0x94[128];

    /* 0x114 : cpu_core_cfg1 */
    union {
        struct {
            uint32_t reserved_0_3        : 4;  /* [ 3: 0],       rsvd,        0x0 */
            uint32_t reg_pll_sel         : 2;  /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7        : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_mcu_main_clk_en : 1;  /* [    8],        r/w,        0x1 */
            uint32_t reg_mcu_wl_clk_en   : 1;  /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31      : 22; /* [31:10],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } cpu_core_cfg1;

    /* 0x118  reserved */
    uint8_t RESERVED0x118[48];

    /* 0x148 : cpu_core_cfg14 */
    union {
        struct {
            uint32_t main_mcu_rst_addr : 32; /* [31: 0],        r/w, 0x60f60000 */
        } BF;
        uint32_t WORD;
    } cpu_core_cfg14;

    /* 0x14C : tzc_pds */
    union {
        struct {
            uint32_t reserved_0              : 1;  /* [    0],       rsvd,        0x0 */
            uint32_t cr_main_mcu_cfg_wr_lock : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31           : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } tzc_pds;

    /* 0x150  reserved */
    uint8_t RESERVED0x150[432];

    /* 0x300 : rc32m_ctrl0 */
    union {
        struct {
            uint32_t rc32m_cal_done        : 1;  /* [    0],          r,        0x0 */
            uint32_t rc32m_rdy             : 1;  /* [    1],          r,        0x0 */
            uint32_t rc32m_cal_inprogress  : 1;  /* [    2],          r,        0x0 */
            uint32_t rc32m_cal_div         : 2;  /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32m_cal_precharge   : 1;  /* [    5],          r,        0x0 */
            uint32_t rc32m_dig_code_fr_cal : 8;  /* [13: 6],          r,        0x0 */
            uint32_t reserved_14_16        : 3;  /* [16:14],       rsvd,        0x0 */
            uint32_t rc32m_allow_cal       : 1;  /* [   17],        r/w,        0x0 */
            uint32_t rc32m_refclk_half     : 1;  /* [   18],        r/w,        0x0 */
            uint32_t rc32m_ext_code_en     : 1;  /* [   19],        r/w,        0x1 */
            uint32_t rc32m_cal_en          : 1;  /* [   20],        r/w,        0x0 */
            uint32_t rc32m_pd              : 1;  /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31        : 10; /* [31:22],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl0;

    /* 0x304 : rc32m_ctrl1 */
    union {
        struct {
            uint32_t rc32m_test_en      : 1;  /* [    0],        r/w,        0x0 */
            uint32_t rc32m_soft_rst     : 1;  /* [    1],        r/w,        0x0 */
            uint32_t rc32m_clk_soft_rst : 1;  /* [    2],        r/w,        0x0 */
            uint32_t rc32m_clk_inv      : 1;  /* [    3],        r/w,        0x0 */
            uint32_t rc32m_clk_force_on : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31      : 27; /* [31: 5],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl1;

    /* 0x308  reserved */
    uint8_t RESERVED0x308[248];

    /* 0x400 : pu_rst_clkpll */
    union {
        struct {
            uint32_t reserved_0_8           : 9;  /* [ 8: 0],       rsvd,        0x0 */
            uint32_t cr_pds_pu_clkpll_sfreg : 1;  /* [    9],        r/w,        0x0 */
            uint32_t cr_pds_pu_clkpll       : 1;  /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_31         : 21; /* [31:11],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pu_rst_clkpll;

    /* 0x404  reserved */
    uint8_t RESERVED0x404[252];

    /* 0x500 : usb_ctrl_0 */
    union {
        struct {
            uint32_t cr_usb_sw_rst_n         : 1;  /* [    0],        r/w,        0x1 */
            uint32_t cr_usb_iddig            : 1;  /* [    1],        r/w,        0x1 */
            uint32_t cr_usb_l1_wakeup        : 1;  /* [    2],        r/w,        0x0 */
            uint32_t cr_usb_wakeup           : 1;  /* [    3],        r/w,        0x0 */
            uint32_t cr_usb_vbus_vld_sw_mode : 1;  /* [    4],        r/w,        0x0 */
            uint32_t cr_usb_vbus_vld_phy_en  : 1;  /* [    5],        r/w,        0x1 */
            uint32_t cr_usb_vbus_vld_gpio_en : 1;  /* [    6],        r/w,        0x0 */
            uint32_t cr_usb_uclk_lpm_dis     : 1;  /* [    7],        r/w,        0x0 */
            uint32_t cr_usb_suspendm_swm     : 1;  /* [    8],        r/w,        0x1 */
            uint32_t cr_usb_suspendm         : 1;  /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_30          : 21; /* [30:10],       rsvd,        0x0 */
            uint32_t reg_pu_usb20_psw        : 1;  /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } usb_ctrl_0;

    /* 0x504 : u2phy_0 */
    union {
        struct {
            uint32_t PLLPTUNE      : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t PLLITUNE      : 2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t PLLBTUNE      : 1; /* [    6],        r/w,        0x0 */
            uint32_t OTGTUNE0      : 3; /* [ 9: 7],        r/w,        0x3 */
            uint32_t OTGDISABLE0   : 1; /* [   10],        r/w,        0x1 */
            uint32_t LOOPBACKENB0  : 1; /* [   11],        r/w,        0x0 */
            uint32_t IDPULLUP0     : 1; /* [   12],        r/w,        0x0 */
            uint32_t HSXCVREXTCTL0 : 1; /* [   13],        r/w,        0x0 */
            uint32_t FSXCVROWNER0  : 1; /* [   14],        r/w,        0x0 */
            uint32_t FSSE0EXT0     : 1; /* [   15],        r/w,        0x0 */
            uint32_t FSEL          : 3; /* [18:16],        r/w,        0x1 */
            uint32_t DRVVBUS0      : 1; /* [   19],        r/w,        0x0 */
            uint32_t DCDENB0       : 1; /* [   20],        r/w,        0x0 */
            uint32_t COMPDISTUNE0  : 3; /* [23:21],        r/w,        0x3 */
            uint32_t COMMONONN     : 1; /* [   24],        r/w,        0x1 */
            uint32_t CHRGSEL0      : 1; /* [   25],        r/w,        0x0 */
            uint32_t BYPASSSEL0    : 1; /* [   26],        r/w,        0x0 */
            uint32_t BYPASSDPEN0   : 1; /* [   27],        r/w,        0x0 */
            uint32_t BYPASSDPDATA0 : 1; /* [   28],        r/w,        0x0 */
            uint32_t BYPASSDMEN0   : 1; /* [   29],        r/w,        0x0 */
            uint32_t BYPASSDMDATA0 : 1; /* [   30],        r/w,        0x0 */
            uint32_t ATERESET      : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } u2phy_0;

    /* 0x508 : u2phy_1 */
    union {
        struct {
            uint32_t reserved_0      : 1; /* [    0],       rsvd,        0x0 */
            uint32_t TXHSXVTUNE0     : 2; /* [ 2: 1],        r/w,        0x3 */
            uint32_t TXFSLSTUNE0     : 4; /* [ 6: 3],        r/w,        0x3 */
            uint32_t TXENABLEN0      : 1; /* [    7],        r/w,        0x1 */
            uint32_t TESTDATAOUTSEL0 : 1; /* [    8],        r/w,        0x0 */
            uint32_t TESTDATAIN0     : 8; /* [16: 9],        r/w,        0x0 */
            uint32_t TESTCLK0        : 1; /* [   17],        r/w,        0x0 */
            uint32_t TESTBURNIN      : 1; /* [   18],        r/w,        0x0 */
            uint32_t TESTADDR0       : 4; /* [22:19],        r/w,        0x0 */
            uint32_t SUSPENDM0       : 1; /* [   23],        r/w,        0x1 */
            uint32_t SQRXTUNE0       : 3; /* [26:24],        r/w,        0x3 */
            uint32_t SLEEPM0         : 1; /* [   27],        r/w,        0x1 */
            uint32_t SIDDQ           : 1; /* [   28],        r/w,        0x1 */
            uint32_t RESREQIN        : 1; /* [   29],        r/w,        0x0 */
            uint32_t REFCLKSEL       : 2; /* [31:30],        r/w,        0x2 */
        } BF;
        uint32_t WORD;
    } u2phy_1;

    /* 0x50C : u2phy_2 */
    union {
        struct {
            uint32_t reserved_0_2           : 3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t USB20_RREF_HIZ         : 1; /* [    3],        r/w,        0x0 */
            uint32_t USB20_RREF_CAL_VOUT_EN : 1; /* [    4],        r/w,        0x0 */
            uint32_t USB20_RREF_CAL_IIN_EN  : 1; /* [    5],        r/w,        0x0 */
            uint32_t USB20_RCAL_CODE        : 5; /* [10: 6],        r/w,       0x10 */
            uint32_t USB20_RCAL_COARSE      : 2; /* [12:11],        r/w,        0x1 */
            uint32_t VBUSROUDET             : 1; /* [   13],        r/w,        0x0 */
            uint32_t VDATSRCENB0            : 1; /* [   14],        r/w,        0x0 */
            uint32_t VDATREFTUNE0           : 2; /* [16:15],        r/w,        0x1 */
            uint32_t VDATDETENB0            : 1; /* [   17],        r/w,        0x0 */
            uint32_t VBUSVLDEXTSEL0         : 1; /* [   18],        r/w,        0x0 */
            uint32_t VATESTENB              : 2; /* [20:19],        r/w,        0x0 */
            uint32_t TXVREFTUNE0            : 4; /* [24:21],        r/w,        0x5 */
            uint32_t TXRISETUNE0            : 2; /* [26:25],        r/w,        0x1 */
            uint32_t TXRESTUNE0             : 2; /* [28:27],        r/w,        0x1 */
            uint32_t TXPREEMPPULSETUNE0     : 1; /* [   29],        r/w,        0x0 */
            uint32_t TXPREEMPAMPTUNE0       : 2; /* [31:30],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } u2phy_2;

    /* 0x510 : u2phy_3 */
    union {
        struct {
            uint32_t TESTDATAOUT0  : 4;  /* [ 3: 0],          r,        0x0 */
            uint32_t HSSQUELCH0    : 1;  /* [    4],          r,        0x0 */
            uint32_t HSRXDAT0      : 1;  /* [    5],          r,        0x0 */
            uint32_t reserved_6_27 : 22; /* [27: 6],       rsvd,        0x0 */
            uint32_t VBUSVLDEXT0   : 1;  /* [   28],        r/w,        0x1 */
            uint32_t FSDATAEXT0    : 1;  /* [   29],        r/w,        0x0 */
            uint32_t PORTRESET0    : 1;  /* [   30],        r/w,        0x0 */
            uint32_t POR           : 1;  /* [   31],        r/w,        0x1 */
        } BF;
        uint32_t WORD;
    } u2phy_3;
};

typedef volatile struct pds_reg pds_reg_t;

#endif /* __PDS_REG_H__ */
