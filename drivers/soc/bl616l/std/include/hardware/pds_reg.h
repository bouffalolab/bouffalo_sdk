/**
  ******************************************************************************
  * @file    pds_reg.h
  * @version V1.0
  * @date    2024-12-07
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

#include "bl616l.h"

/* 0x0 : PDS_CTL */
#define PDS_CTL_OFFSET                               (0x0)
#define PDS_START_PS                                 PDS_START_PS
#define PDS_START_PS_POS                             (0U)
#define PDS_START_PS_LEN                             (1U)
#define PDS_START_PS_MSK                             (((1U << PDS_START_PS_LEN) - 1) << PDS_START_PS_POS)
#define PDS_START_PS_UMSK                            (~(((1U << PDS_START_PS_LEN) - 1) << PDS_START_PS_POS))
#define PDS_CR_SLEEP_FOREVER                         PDS_CR_SLEEP_FOREVER
#define PDS_CR_SLEEP_FOREVER_POS                     (1U)
#define PDS_CR_SLEEP_FOREVER_LEN                     (1U)
#define PDS_CR_SLEEP_FOREVER_MSK                     (((1U << PDS_CR_SLEEP_FOREVER_LEN) - 1) << PDS_CR_SLEEP_FOREVER_POS)
#define PDS_CR_SLEEP_FOREVER_UMSK                    (~(((1U << PDS_CR_SLEEP_FOREVER_LEN) - 1) << PDS_CR_SLEEP_FOREVER_POS))
#define PDS_CR_XTAL_FORCE_OFF                        PDS_CR_XTAL_FORCE_OFF
#define PDS_CR_XTAL_FORCE_OFF_POS                    (2U)
#define PDS_CR_XTAL_FORCE_OFF_LEN                    (1U)
#define PDS_CR_XTAL_FORCE_OFF_MSK                    (((1U << PDS_CR_XTAL_FORCE_OFF_LEN) - 1) << PDS_CR_XTAL_FORCE_OFF_POS)
#define PDS_CR_XTAL_FORCE_OFF_UMSK                   (~(((1U << PDS_CR_XTAL_FORCE_OFF_LEN) - 1) << PDS_CR_XTAL_FORCE_OFF_POS))
#define PDS_CR_PDS_WIFI_SAVE_STATE                   PDS_CR_PDS_WIFI_SAVE_STATE
#define PDS_CR_PDS_WIFI_SAVE_STATE_POS               (3U)
#define PDS_CR_PDS_WIFI_SAVE_STATE_LEN               (1U)
#define PDS_CR_PDS_WIFI_SAVE_STATE_MSK               (((1U << PDS_CR_PDS_WIFI_SAVE_STATE_LEN) - 1) << PDS_CR_PDS_WIFI_SAVE_STATE_POS)
#define PDS_CR_PDS_WIFI_SAVE_STATE_UMSK              (~(((1U << PDS_CR_PDS_WIFI_SAVE_STATE_LEN) - 1) << PDS_CR_PDS_WIFI_SAVE_STATE_POS))
#define PDS_FAST_CLK_SEL                             PDS_FAST_CLK_SEL
#define PDS_FAST_CLK_SEL_POS                         (4U)
#define PDS_FAST_CLK_SEL_LEN                         (1U)
#define PDS_FAST_CLK_SEL_MSK                         (((1U << PDS_FAST_CLK_SEL_LEN) - 1) << PDS_FAST_CLK_SEL_POS)
#define PDS_FAST_CLK_SEL_UMSK                        (~(((1U << PDS_FAST_CLK_SEL_LEN) - 1) << PDS_FAST_CLK_SEL_POS))
#define PDS_MASK_PDS_ON_GPIO_DET_EN                  PDS_MASK_PDS_ON_GPIO_DET_EN
#define PDS_MASK_PDS_ON_GPIO_DET_EN_POS              (5U)
#define PDS_MASK_PDS_ON_GPIO_DET_EN_LEN              (1U)
#define PDS_MASK_PDS_ON_GPIO_DET_EN_MSK              (((1U << PDS_MASK_PDS_ON_GPIO_DET_EN_LEN) - 1) << PDS_MASK_PDS_ON_GPIO_DET_EN_POS)
#define PDS_MASK_PDS_ON_GPIO_DET_EN_UMSK             (~(((1U << PDS_MASK_PDS_ON_GPIO_DET_EN_LEN) - 1) << PDS_MASK_PDS_ON_GPIO_DET_EN_POS))
#define PDS_CR_PDS_GATE_CLK                          PDS_CR_PDS_GATE_CLK
#define PDS_CR_PDS_GATE_CLK_POS                      (8U)
#define PDS_CR_PDS_GATE_CLK_LEN                      (1U)
#define PDS_CR_PDS_GATE_CLK_MSK                      (((1U << PDS_CR_PDS_GATE_CLK_LEN) - 1) << PDS_CR_PDS_GATE_CLK_POS)
#define PDS_CR_PDS_GATE_CLK_UMSK                     (~(((1U << PDS_CR_PDS_GATE_CLK_LEN) - 1) << PDS_CR_PDS_GATE_CLK_POS))
#define PDS_CR_PDS_MEM_STBY                          PDS_CR_PDS_MEM_STBY
#define PDS_CR_PDS_MEM_STBY_POS                      (9U)
#define PDS_CR_PDS_MEM_STBY_LEN                      (1U)
#define PDS_CR_PDS_MEM_STBY_MSK                      (((1U << PDS_CR_PDS_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MEM_STBY_POS)
#define PDS_CR_PDS_MEM_STBY_UMSK                     (~(((1U << PDS_CR_PDS_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MEM_STBY_POS))
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT             PDS_CR_PDS_GLB_REG_RESET_PROTECT
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS         (10U)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN         (1U)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_MSK         (((1U << PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN) - 1) << PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_UMSK        (~(((1U << PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN) - 1) << PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS))
#define PDS_CR_PDS_ISO_EN                            PDS_CR_PDS_ISO_EN
#define PDS_CR_PDS_ISO_EN_POS                        (11U)
#define PDS_CR_PDS_ISO_EN_LEN                        (1U)
#define PDS_CR_PDS_ISO_EN_MSK                        (((1U << PDS_CR_PDS_ISO_EN_LEN) - 1) << PDS_CR_PDS_ISO_EN_POS)
#define PDS_CR_PDS_ISO_EN_UMSK                       (~(((1U << PDS_CR_PDS_ISO_EN_LEN) - 1) << PDS_CR_PDS_ISO_EN_POS))
#define PDS_CR_PDS_WAIT_XTAL_RDY                     PDS_CR_PDS_WAIT_XTAL_RDY
#define PDS_CR_PDS_WAIT_XTAL_RDY_POS                 (12U)
#define PDS_CR_PDS_WAIT_XTAL_RDY_LEN                 (1U)
#define PDS_CR_PDS_WAIT_XTAL_RDY_MSK                 (((1U << PDS_CR_PDS_WAIT_XTAL_RDY_LEN) - 1) << PDS_CR_PDS_WAIT_XTAL_RDY_POS)
#define PDS_CR_PDS_WAIT_XTAL_RDY_UMSK                (~(((1U << PDS_CR_PDS_WAIT_XTAL_RDY_LEN) - 1) << PDS_CR_PDS_WAIT_XTAL_RDY_POS))
#define PDS_CR_PDS_PWR_OFF                           PDS_CR_PDS_PWR_OFF
#define PDS_CR_PDS_PWR_OFF_POS                       (13U)
#define PDS_CR_PDS_PWR_OFF_LEN                       (1U)
#define PDS_CR_PDS_PWR_OFF_MSK                       (((1U << PDS_CR_PDS_PWR_OFF_LEN) - 1) << PDS_CR_PDS_PWR_OFF_POS)
#define PDS_CR_PDS_PWR_OFF_UMSK                      (~(((1U << PDS_CR_PDS_PWR_OFF_LEN) - 1) << PDS_CR_PDS_PWR_OFF_POS))
#define PDS_CR_PDS_PD_XTAL                           PDS_CR_PDS_PD_XTAL
#define PDS_CR_PDS_PD_XTAL_POS                       (14U)
#define PDS_CR_PDS_PD_XTAL_LEN                       (1U)
#define PDS_CR_PDS_PD_XTAL_MSK                       (((1U << PDS_CR_PDS_PD_XTAL_LEN) - 1) << PDS_CR_PDS_PD_XTAL_POS)
#define PDS_CR_PDS_PD_XTAL_UMSK                      (~(((1U << PDS_CR_PDS_PD_XTAL_LEN) - 1) << PDS_CR_PDS_PD_XTAL_POS))
#define PDS_CR_PDS_CTRL_SOC_ENB                      PDS_CR_PDS_CTRL_SOC_ENB
#define PDS_CR_PDS_CTRL_SOC_ENB_POS                  (15U)
#define PDS_CR_PDS_CTRL_SOC_ENB_LEN                  (1U)
#define PDS_CR_PDS_CTRL_SOC_ENB_MSK                  (((1U << PDS_CR_PDS_CTRL_SOC_ENB_LEN) - 1) << PDS_CR_PDS_CTRL_SOC_ENB_POS)
#define PDS_CR_PDS_CTRL_SOC_ENB_UMSK                 (~(((1U << PDS_CR_PDS_CTRL_SOC_ENB_LEN) - 1) << PDS_CR_PDS_CTRL_SOC_ENB_POS))
#define PDS_CR_PDS_RST_SOC                           PDS_CR_PDS_RST_SOC
#define PDS_CR_PDS_RST_SOC_POS                       (16U)
#define PDS_CR_PDS_RST_SOC_LEN                       (1U)
#define PDS_CR_PDS_RST_SOC_MSK                       (((1U << PDS_CR_PDS_RST_SOC_LEN) - 1) << PDS_CR_PDS_RST_SOC_POS)
#define PDS_CR_PDS_RST_SOC_UMSK                      (~(((1U << PDS_CR_PDS_RST_SOC_LEN) - 1) << PDS_CR_PDS_RST_SOC_POS))
#define PDS_CR_PDS_RC32M_OFF_DIS                     PDS_CR_PDS_RC32M_OFF_DIS
#define PDS_CR_PDS_RC32M_OFF_DIS_POS                 (17U)
#define PDS_CR_PDS_RC32M_OFF_DIS_LEN                 (1U)
#define PDS_CR_PDS_RC32M_OFF_DIS_MSK                 (((1U << PDS_CR_PDS_RC32M_OFF_DIS_LEN) - 1) << PDS_CR_PDS_RC32M_OFF_DIS_POS)
#define PDS_CR_PDS_RC32M_OFF_DIS_UMSK                (~(((1U << PDS_CR_PDS_RC32M_OFF_DIS_LEN) - 1) << PDS_CR_PDS_RC32M_OFF_DIS_POS))
#define PDS_CR_PDS_LDO_SOC_VSEL_EN                   PDS_CR_PDS_LDO_SOC_VSEL_EN
#define PDS_CR_PDS_LDO_SOC_VSEL_EN_POS               (18U)
#define PDS_CR_PDS_LDO_SOC_VSEL_EN_LEN               (1U)
#define PDS_CR_PDS_LDO_SOC_VSEL_EN_MSK               (((1U << PDS_CR_PDS_LDO_SOC_VSEL_EN_LEN) - 1) << PDS_CR_PDS_LDO_SOC_VSEL_EN_POS)
#define PDS_CR_PDS_LDO_SOC_VSEL_EN_UMSK              (~(((1U << PDS_CR_PDS_LDO_SOC_VSEL_EN_LEN) - 1) << PDS_CR_PDS_LDO_SOC_VSEL_EN_POS))
#define PDS_CR_PDS_CTRL_WIFIPLL_PD                   PDS_CR_PDS_CTRL_WIFIPLL_PD
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_POS               (22U)
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_LEN               (1U)
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_MSK               (((1U << PDS_CR_PDS_CTRL_WIFIPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_WIFIPLL_PD_POS)
#define PDS_CR_PDS_CTRL_WIFIPLL_PD_UMSK              (~(((1U << PDS_CR_PDS_CTRL_WIFIPLL_PD_LEN) - 1) << PDS_CR_PDS_CTRL_WIFIPLL_PD_POS))
#define PDS_GPIO_DET_CLK_SEL                         PDS_GPIO_DET_CLK_SEL
#define PDS_GPIO_DET_CLK_SEL_POS                     (25U)
#define PDS_GPIO_DET_CLK_SEL_LEN                     (2U)
#define PDS_GPIO_DET_CLK_SEL_MSK                     (((1U << PDS_GPIO_DET_CLK_SEL_LEN) - 1) << PDS_GPIO_DET_CLK_SEL_POS)
#define PDS_GPIO_DET_CLK_SEL_UMSK                    (~(((1U << PDS_GPIO_DET_CLK_SEL_LEN) - 1) << PDS_GPIO_DET_CLK_SEL_POS))
#define PDS_CR_PDS_ACCESS_WRAM_PATH                  PDS_CR_PDS_ACCESS_WRAM_PATH
#define PDS_CR_PDS_ACCESS_WRAM_PATH_POS              (27U)
#define PDS_CR_PDS_ACCESS_WRAM_PATH_LEN              (1U)
#define PDS_CR_PDS_ACCESS_WRAM_PATH_MSK              (((1U << PDS_CR_PDS_ACCESS_WRAM_PATH_LEN) - 1) << PDS_CR_PDS_ACCESS_WRAM_PATH_POS)
#define PDS_CR_PDS_ACCESS_WRAM_PATH_UMSK             (~(((1U << PDS_CR_PDS_ACCESS_WRAM_PATH_LEN) - 1) << PDS_CR_PDS_ACCESS_WRAM_PATH_POS))
#define PDS_CR_PDS_CTRL_RF                           PDS_CR_PDS_CTRL_RF
#define PDS_CR_PDS_CTRL_RF_POS                       (28U)
#define PDS_CR_PDS_CTRL_RF_LEN                       (2U)
#define PDS_CR_PDS_CTRL_RF_MSK                       (((1U << PDS_CR_PDS_CTRL_RF_LEN) - 1) << PDS_CR_PDS_CTRL_RF_POS)
#define PDS_CR_PDS_CTRL_RF_UMSK                      (~(((1U << PDS_CR_PDS_CTRL_RF_LEN) - 1) << PDS_CR_PDS_CTRL_RF_POS))
#define PDS_CR_PDS_START_USE_TBTT_SLEEP              PDS_CR_PDS_START_USE_TBTT_SLEEP
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_POS          (30U)
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_LEN          (1U)
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_MSK          (((1U << PDS_CR_PDS_START_USE_TBTT_SLEEP_LEN) - 1) << PDS_CR_PDS_START_USE_TBTT_SLEEP_POS)
#define PDS_CR_PDS_START_USE_TBTT_SLEEP_UMSK         (~(((1U << PDS_CR_PDS_START_USE_TBTT_SLEEP_LEN) - 1) << PDS_CR_PDS_START_USE_TBTT_SLEEP_POS))

/* 0x4 : PDS_TIME1 */
#define PDS_TIME1_OFFSET                             (0x4)
#define PDS_CR_SLEEP_DURATION                        PDS_CR_SLEEP_DURATION
#define PDS_CR_SLEEP_DURATION_POS                    (0U)
#define PDS_CR_SLEEP_DURATION_LEN                    (32U)
#define PDS_CR_SLEEP_DURATION_MSK                    (((1U << PDS_CR_SLEEP_DURATION_LEN) - 1) << PDS_CR_SLEEP_DURATION_POS)
#define PDS_CR_SLEEP_DURATION_UMSK                   (~(((1U << PDS_CR_SLEEP_DURATION_LEN) - 1) << PDS_CR_SLEEP_DURATION_POS))

/* 0x8 : PDS_GPIO_LAT_EN */
#define PDS_GPIO_LAT_EN_OFFSET                       (0x8)
#define PDS_GPIO_LAT_EN                              PDS_GPIO_LAT_EN
#define PDS_GPIO_LAT_EN_POS                          (0U)
#define PDS_GPIO_LAT_EN_LEN                          (31U)
#define PDS_GPIO_LAT_EN_MSK                          (((1U << PDS_GPIO_LAT_EN_LEN) - 1) << PDS_GPIO_LAT_EN_POS)
#define PDS_GPIO_LAT_EN_UMSK                         (~(((1U << PDS_GPIO_LAT_EN_LEN) - 1) << PDS_GPIO_LAT_EN_POS))

/* 0xC : PDS_INT */
#define PDS_INT_OFFSET                               (0xC)
#define PDS_RO_PDS_WAKE_INT                          PDS_RO_PDS_WAKE_INT
#define PDS_RO_PDS_WAKE_INT_POS                      (0U)
#define PDS_RO_PDS_WAKE_INT_LEN                      (1U)
#define PDS_RO_PDS_WAKE_INT_MSK                      (((1U << PDS_RO_PDS_WAKE_INT_LEN) - 1) << PDS_RO_PDS_WAKE_INT_POS)
#define PDS_RO_PDS_WAKE_INT_UMSK                     (~(((1U << PDS_RO_PDS_WAKE_INT_LEN) - 1) << PDS_RO_PDS_WAKE_INT_POS))
#define PDS_RO_PDS_RF_DONE_INT                       PDS_RO_PDS_RF_DONE_INT
#define PDS_RO_PDS_RF_DONE_INT_POS                   (1U)
#define PDS_RO_PDS_RF_DONE_INT_LEN                   (1U)
#define PDS_RO_PDS_RF_DONE_INT_MSK                   (((1U << PDS_RO_PDS_RF_DONE_INT_LEN) - 1) << PDS_RO_PDS_RF_DONE_INT_POS)
#define PDS_RO_PDS_RF_DONE_INT_UMSK                  (~(((1U << PDS_RO_PDS_RF_DONE_INT_LEN) - 1) << PDS_RO_PDS_RF_DONE_INT_POS))
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ               PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_POS           (2U)
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_LEN           (1U)
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_MSK           (((1U << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_POS)
#define PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_UMSK          (~(((1U << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_SLEEP_IRQ_POS))
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ              PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_POS          (3U)
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_LEN          (1U)
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_MSK          (((1U << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_POS)
#define PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_UMSK         (~(((1U << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_LEN) - 1) << PDS_RO_PDS_WIFI_TBTT_WAKEUP_IRQ_POS))
#define PDS_CR_PDS_WAKE_INT_MASK                     PDS_CR_PDS_WAKE_INT_MASK
#define PDS_CR_PDS_WAKE_INT_MASK_POS                 (4U)
#define PDS_CR_PDS_WAKE_INT_MASK_LEN                 (1U)
#define PDS_CR_PDS_WAKE_INT_MASK_MSK                 (((1U << PDS_CR_PDS_WAKE_INT_MASK_LEN) - 1) << PDS_CR_PDS_WAKE_INT_MASK_POS)
#define PDS_CR_PDS_WAKE_INT_MASK_UMSK                (~(((1U << PDS_CR_PDS_WAKE_INT_MASK_LEN) - 1) << PDS_CR_PDS_WAKE_INT_MASK_POS))
#define PDS_CR_PDS_RF_DONE_INT_MASK                  PDS_CR_PDS_RF_DONE_INT_MASK
#define PDS_CR_PDS_RF_DONE_INT_MASK_POS              (5U)
#define PDS_CR_PDS_RF_DONE_INT_MASK_LEN              (1U)
#define PDS_CR_PDS_RF_DONE_INT_MASK_MSK              (((1U << PDS_CR_PDS_RF_DONE_INT_MASK_LEN) - 1) << PDS_CR_PDS_RF_DONE_INT_MASK_POS)
#define PDS_CR_PDS_RF_DONE_INT_MASK_UMSK             (~(((1U << PDS_CR_PDS_RF_DONE_INT_MASK_LEN) - 1) << PDS_CR_PDS_RF_DONE_INT_MASK_POS))
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK          PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_POS      (6U)
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_LEN      (1U)
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_MSK      (((1U << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_POS)
#define PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_UMSK     (~(((1U << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK_POS))
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK         PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_POS     (7U)
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_LEN     (1U)
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_MSK     (((1U << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_POS)
#define PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_UMSK    (~(((1U << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_LEN) - 1) << PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK_POS))
#define PDS_CR_PDS_INT_CLR                           PDS_CR_PDS_INT_CLR
#define PDS_CR_PDS_INT_CLR_POS                       (8U)
#define PDS_CR_PDS_INT_CLR_LEN                       (1U)
#define PDS_CR_PDS_INT_CLR_MSK                       (((1U << PDS_CR_PDS_INT_CLR_LEN) - 1) << PDS_CR_PDS_INT_CLR_POS)
#define PDS_CR_PDS_INT_CLR_UMSK                      (~(((1U << PDS_CR_PDS_INT_CLR_LEN) - 1) << PDS_CR_PDS_INT_CLR_POS))
#define PDS_CR_PDS_WAKEUP_SRC_EN                     PDS_CR_PDS_WAKEUP_SRC_EN
#define PDS_CR_PDS_WAKEUP_SRC_EN_POS                 (10U)
#define PDS_CR_PDS_WAKEUP_SRC_EN_LEN                 (10U)
#define PDS_CR_PDS_WAKEUP_SRC_EN_MSK                 (((1U << PDS_CR_PDS_WAKEUP_SRC_EN_LEN) - 1) << PDS_CR_PDS_WAKEUP_SRC_EN_POS)
#define PDS_CR_PDS_WAKEUP_SRC_EN_UMSK                (~(((1U << PDS_CR_PDS_WAKEUP_SRC_EN_LEN) - 1) << PDS_CR_PDS_WAKEUP_SRC_EN_POS))
#define PDS_RO_PDS_WAKEUP_EVENT                      PDS_RO_PDS_WAKEUP_EVENT
#define PDS_RO_PDS_WAKEUP_EVENT_POS                  (21U)
#define PDS_RO_PDS_WAKEUP_EVENT_LEN                  (10U)
#define PDS_RO_PDS_WAKEUP_EVENT_MSK                  (((1U << PDS_RO_PDS_WAKEUP_EVENT_LEN) - 1) << PDS_RO_PDS_WAKEUP_EVENT_POS)
#define PDS_RO_PDS_WAKEUP_EVENT_UMSK                 (~(((1U << PDS_RO_PDS_WAKEUP_EVENT_LEN) - 1) << PDS_RO_PDS_WAKEUP_EVENT_POS))

/* 0x10 : PDS_CTL2 */
#define PDS_CTL2_OFFSET                              (0x10)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF                  PDS_CR_PDS_FORCE_NP_PWR_OFF
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_POS              (0U)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_LEN              (1U)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_MSK              (((1U << PDS_CR_PDS_FORCE_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_NP_PWR_OFF_UMSK             (~(((1U << PDS_CR_PDS_FORCE_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_WB_PWR_OFF                  PDS_CR_PDS_FORCE_WB_PWR_OFF
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_POS              (2U)
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_LEN              (1U)
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_MSK              (((1U << PDS_CR_PDS_FORCE_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_WB_PWR_OFF_UMSK             (~(((1U << PDS_CR_PDS_FORCE_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_USB_PWR_OFF                 PDS_CR_PDS_FORCE_USB_PWR_OFF
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_POS             (3U)
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_LEN             (1U)
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_MSK             (((1U << PDS_CR_PDS_FORCE_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_USB_PWR_OFF_UMSK            (~(((1U << PDS_CR_PDS_FORCE_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_NP_ISO_EN                   PDS_CR_PDS_FORCE_NP_ISO_EN
#define PDS_CR_PDS_FORCE_NP_ISO_EN_POS               (4U)
#define PDS_CR_PDS_FORCE_NP_ISO_EN_LEN               (1U)
#define PDS_CR_PDS_FORCE_NP_ISO_EN_MSK               (((1U << PDS_CR_PDS_FORCE_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_NP_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_NP_ISO_EN_UMSK              (~(((1U << PDS_CR_PDS_FORCE_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_NP_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_WB_ISO_EN                   PDS_CR_PDS_FORCE_WB_ISO_EN
#define PDS_CR_PDS_FORCE_WB_ISO_EN_POS               (6U)
#define PDS_CR_PDS_FORCE_WB_ISO_EN_LEN               (1U)
#define PDS_CR_PDS_FORCE_WB_ISO_EN_MSK               (((1U << PDS_CR_PDS_FORCE_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_WB_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_WB_ISO_EN_UMSK              (~(((1U << PDS_CR_PDS_FORCE_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_WB_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_USB_ISO_EN                  PDS_CR_PDS_FORCE_USB_ISO_EN
#define PDS_CR_PDS_FORCE_USB_ISO_EN_POS              (7U)
#define PDS_CR_PDS_FORCE_USB_ISO_EN_LEN              (1U)
#define PDS_CR_PDS_FORCE_USB_ISO_EN_MSK              (((1U << PDS_CR_PDS_FORCE_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_USB_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_USB_ISO_EN_UMSK             (~(((1U << PDS_CR_PDS_FORCE_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_USB_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_NP_PDS_RST                  PDS_CR_PDS_FORCE_NP_PDS_RST
#define PDS_CR_PDS_FORCE_NP_PDS_RST_POS              (8U)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_LEN              (1U)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_MSK              (((1U << PDS_CR_PDS_FORCE_NP_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_UMSK             (~(((1U << PDS_CR_PDS_FORCE_NP_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_NP_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_WB_PDS_RST                  PDS_CR_PDS_FORCE_WB_PDS_RST
#define PDS_CR_PDS_FORCE_WB_PDS_RST_POS              (10U)
#define PDS_CR_PDS_FORCE_WB_PDS_RST_LEN              (1U)
#define PDS_CR_PDS_FORCE_WB_PDS_RST_MSK              (((1U << PDS_CR_PDS_FORCE_WB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_WB_PDS_RST_UMSK             (~(((1U << PDS_CR_PDS_FORCE_WB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_WB_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_USB_PDS_RST                 PDS_CR_PDS_FORCE_USB_PDS_RST
#define PDS_CR_PDS_FORCE_USB_PDS_RST_POS             (11U)
#define PDS_CR_PDS_FORCE_USB_PDS_RST_LEN             (1U)
#define PDS_CR_PDS_FORCE_USB_PDS_RST_MSK             (((1U << PDS_CR_PDS_FORCE_USB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_USB_PDS_RST_UMSK            (~(((1U << PDS_CR_PDS_FORCE_USB_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_USB_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_NP_MEM_STBY                 PDS_CR_PDS_FORCE_NP_MEM_STBY
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_POS             (12U)
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_LEN             (1U)
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_MSK             (((1U << PDS_CR_PDS_FORCE_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_NP_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_NP_MEM_STBY_UMSK            (~(((1U << PDS_CR_PDS_FORCE_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_NP_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_WB_MEM_STBY                 PDS_CR_PDS_FORCE_WB_MEM_STBY
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_POS             (14U)
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_LEN             (1U)
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_MSK             (((1U << PDS_CR_PDS_FORCE_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_WB_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_WB_MEM_STBY_UMSK            (~(((1U << PDS_CR_PDS_FORCE_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_WB_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_USB_MEM_STBY                PDS_CR_PDS_FORCE_USB_MEM_STBY
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_POS            (15U)
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_LEN            (1U)
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_MSK            (((1U << PDS_CR_PDS_FORCE_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_USB_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_USB_MEM_STBY_UMSK           (~(((1U << PDS_CR_PDS_FORCE_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_USB_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_NP_GATE_CLK                 PDS_CR_PDS_FORCE_NP_GATE_CLK
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_POS             (16U)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN             (1U)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_MSK             (((1U << PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_NP_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_UMSK            (~(((1U << PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_NP_GATE_CLK_POS))
#define PDS_CR_PDS_FORCE_WB_GATE_CLK                 PDS_CR_PDS_FORCE_WB_GATE_CLK
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_POS             (18U)
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_LEN             (1U)
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_MSK             (((1U << PDS_CR_PDS_FORCE_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_WB_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_WB_GATE_CLK_UMSK            (~(((1U << PDS_CR_PDS_FORCE_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_WB_GATE_CLK_POS))
#define PDS_CR_PDS_FORCE_USB_GATE_CLK                PDS_CR_PDS_FORCE_USB_GATE_CLK
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_POS            (19U)
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_LEN            (1U)
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_MSK            (((1U << PDS_CR_PDS_FORCE_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_USB_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_USB_GATE_CLK_UMSK           (~(((1U << PDS_CR_PDS_FORCE_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_USB_GATE_CLK_POS))

/* 0x14 : PDS_CTL3 */
#define PDS_CTL3_OFFSET                              (0x14)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF                PDS_CR_PDS_FORCE_MISC_PWR_OFF
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS            (1U)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN            (1U)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_MSK            (((1U << PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_UMSK           (~(((1U << PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_MISC_ISO_EN                 PDS_CR_PDS_FORCE_MISC_ISO_EN
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_POS             (4U)
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_LEN             (1U)
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_MSK             (((1U << PDS_CR_PDS_FORCE_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_ISO_EN_POS)
#define PDS_CR_PDS_FORCE_MISC_ISO_EN_UMSK            (~(((1U << PDS_CR_PDS_FORCE_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_ISO_EN_POS))
#define PDS_CR_PDS_FORCE_MISC_PDS_RST                PDS_CR_PDS_FORCE_MISC_PDS_RST
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_POS            (7U)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN            (1U)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_MSK            (((1U << PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_UMSK           (~(((1U << PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY               PDS_CR_PDS_FORCE_MISC_MEM_STBY
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_POS           (10U)
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_LEN           (1U)
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_MSK           (((1U << PDS_CR_PDS_FORCE_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_MEM_STBY_POS)
#define PDS_CR_PDS_FORCE_MISC_MEM_STBY_UMSK          (~(((1U << PDS_CR_PDS_FORCE_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_MEM_STBY_POS))
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK               PDS_CR_PDS_FORCE_MISC_GATE_CLK
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS           (13U)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN           (1U)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_MSK           (((1U << PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_UMSK          (~(((1U << PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS))
#define PDS_CR_PDS_NP_ISO_EN                         PDS_CR_PDS_NP_ISO_EN
#define PDS_CR_PDS_NP_ISO_EN_POS                     (24U)
#define PDS_CR_PDS_NP_ISO_EN_LEN                     (1U)
#define PDS_CR_PDS_NP_ISO_EN_MSK                     (((1U << PDS_CR_PDS_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_NP_ISO_EN_POS)
#define PDS_CR_PDS_NP_ISO_EN_UMSK                    (~(((1U << PDS_CR_PDS_NP_ISO_EN_LEN) - 1) << PDS_CR_PDS_NP_ISO_EN_POS))
#define PDS_CR_PDS_WB_ISO_EN                         PDS_CR_PDS_WB_ISO_EN
#define PDS_CR_PDS_WB_ISO_EN_POS                     (27U)
#define PDS_CR_PDS_WB_ISO_EN_LEN                     (1U)
#define PDS_CR_PDS_WB_ISO_EN_MSK                     (((1U << PDS_CR_PDS_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_WB_ISO_EN_POS)
#define PDS_CR_PDS_WB_ISO_EN_UMSK                    (~(((1U << PDS_CR_PDS_WB_ISO_EN_LEN) - 1) << PDS_CR_PDS_WB_ISO_EN_POS))
#define PDS_CR_PDS_USB_ISO_EN                        PDS_CR_PDS_USB_ISO_EN
#define PDS_CR_PDS_USB_ISO_EN_POS                    (29U)
#define PDS_CR_PDS_USB_ISO_EN_LEN                    (1U)
#define PDS_CR_PDS_USB_ISO_EN_MSK                    (((1U << PDS_CR_PDS_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_USB_ISO_EN_POS)
#define PDS_CR_PDS_USB_ISO_EN_UMSK                   (~(((1U << PDS_CR_PDS_USB_ISO_EN_LEN) - 1) << PDS_CR_PDS_USB_ISO_EN_POS))
#define PDS_CR_PDS_MISC_ISO_EN                       PDS_CR_PDS_MISC_ISO_EN
#define PDS_CR_PDS_MISC_ISO_EN_POS                   (30U)
#define PDS_CR_PDS_MISC_ISO_EN_LEN                   (1U)
#define PDS_CR_PDS_MISC_ISO_EN_MSK                   (((1U << PDS_CR_PDS_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_MISC_ISO_EN_POS)
#define PDS_CR_PDS_MISC_ISO_EN_UMSK                  (~(((1U << PDS_CR_PDS_MISC_ISO_EN_LEN) - 1) << PDS_CR_PDS_MISC_ISO_EN_POS))

/* 0x18 : PDS_CTL4 */
#define PDS_CTL4_OFFSET                              (0x18)
#define PDS_CR_PDS_NP_PWR_OFF                        PDS_CR_PDS_NP_PWR_OFF
#define PDS_CR_PDS_NP_PWR_OFF_POS                    (0U)
#define PDS_CR_PDS_NP_PWR_OFF_LEN                    (1U)
#define PDS_CR_PDS_NP_PWR_OFF_MSK                    (((1U << PDS_CR_PDS_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_NP_PWR_OFF_POS)
#define PDS_CR_PDS_NP_PWR_OFF_UMSK                   (~(((1U << PDS_CR_PDS_NP_PWR_OFF_LEN) - 1) << PDS_CR_PDS_NP_PWR_OFF_POS))
#define PDS_CR_PDS_NP_RESET                          PDS_CR_PDS_NP_RESET
#define PDS_CR_PDS_NP_RESET_POS                      (1U)
#define PDS_CR_PDS_NP_RESET_LEN                      (1U)
#define PDS_CR_PDS_NP_RESET_MSK                      (((1U << PDS_CR_PDS_NP_RESET_LEN) - 1) << PDS_CR_PDS_NP_RESET_POS)
#define PDS_CR_PDS_NP_RESET_UMSK                     (~(((1U << PDS_CR_PDS_NP_RESET_LEN) - 1) << PDS_CR_PDS_NP_RESET_POS))
#define PDS_CR_PDS_NP_MEM_STBY                       PDS_CR_PDS_NP_MEM_STBY
#define PDS_CR_PDS_NP_MEM_STBY_POS                   (2U)
#define PDS_CR_PDS_NP_MEM_STBY_LEN                   (1U)
#define PDS_CR_PDS_NP_MEM_STBY_MSK                   (((1U << PDS_CR_PDS_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_NP_MEM_STBY_POS)
#define PDS_CR_PDS_NP_MEM_STBY_UMSK                  (~(((1U << PDS_CR_PDS_NP_MEM_STBY_LEN) - 1) << PDS_CR_PDS_NP_MEM_STBY_POS))
#define PDS_CR_PDS_NP_GATE_CLK                       PDS_CR_PDS_NP_GATE_CLK
#define PDS_CR_PDS_NP_GATE_CLK_POS                   (3U)
#define PDS_CR_PDS_NP_GATE_CLK_LEN                   (1U)
#define PDS_CR_PDS_NP_GATE_CLK_MSK                   (((1U << PDS_CR_PDS_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_NP_GATE_CLK_POS)
#define PDS_CR_PDS_NP_GATE_CLK_UMSK                  (~(((1U << PDS_CR_PDS_NP_GATE_CLK_LEN) - 1) << PDS_CR_PDS_NP_GATE_CLK_POS))
#define PDS_CR_PDS_WB_PWR_OFF                        PDS_CR_PDS_WB_PWR_OFF
#define PDS_CR_PDS_WB_PWR_OFF_POS                    (12U)
#define PDS_CR_PDS_WB_PWR_OFF_LEN                    (1U)
#define PDS_CR_PDS_WB_PWR_OFF_MSK                    (((1U << PDS_CR_PDS_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_WB_PWR_OFF_POS)
#define PDS_CR_PDS_WB_PWR_OFF_UMSK                   (~(((1U << PDS_CR_PDS_WB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_WB_PWR_OFF_POS))
#define PDS_CR_PDS_WB_RESET                          PDS_CR_PDS_WB_RESET
#define PDS_CR_PDS_WB_RESET_POS                      (13U)
#define PDS_CR_PDS_WB_RESET_LEN                      (1U)
#define PDS_CR_PDS_WB_RESET_MSK                      (((1U << PDS_CR_PDS_WB_RESET_LEN) - 1) << PDS_CR_PDS_WB_RESET_POS)
#define PDS_CR_PDS_WB_RESET_UMSK                     (~(((1U << PDS_CR_PDS_WB_RESET_LEN) - 1) << PDS_CR_PDS_WB_RESET_POS))
#define PDS_CR_PDS_WB_MEM_STBY                       PDS_CR_PDS_WB_MEM_STBY
#define PDS_CR_PDS_WB_MEM_STBY_POS                   (14U)
#define PDS_CR_PDS_WB_MEM_STBY_LEN                   (1U)
#define PDS_CR_PDS_WB_MEM_STBY_MSK                   (((1U << PDS_CR_PDS_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_WB_MEM_STBY_POS)
#define PDS_CR_PDS_WB_MEM_STBY_UMSK                  (~(((1U << PDS_CR_PDS_WB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_WB_MEM_STBY_POS))
#define PDS_CR_PDS_WB_GATE_CLK                       PDS_CR_PDS_WB_GATE_CLK
#define PDS_CR_PDS_WB_GATE_CLK_POS                   (15U)
#define PDS_CR_PDS_WB_GATE_CLK_LEN                   (1U)
#define PDS_CR_PDS_WB_GATE_CLK_MSK                   (((1U << PDS_CR_PDS_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_WB_GATE_CLK_POS)
#define PDS_CR_PDS_WB_GATE_CLK_UMSK                  (~(((1U << PDS_CR_PDS_WB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_WB_GATE_CLK_POS))
#define PDS_CR_PDS_USB_PWR_OFF                       PDS_CR_PDS_USB_PWR_OFF
#define PDS_CR_PDS_USB_PWR_OFF_POS                   (20U)
#define PDS_CR_PDS_USB_PWR_OFF_LEN                   (1U)
#define PDS_CR_PDS_USB_PWR_OFF_MSK                   (((1U << PDS_CR_PDS_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_USB_PWR_OFF_POS)
#define PDS_CR_PDS_USB_PWR_OFF_UMSK                  (~(((1U << PDS_CR_PDS_USB_PWR_OFF_LEN) - 1) << PDS_CR_PDS_USB_PWR_OFF_POS))
#define PDS_CR_PDS_USB_RESET                         PDS_CR_PDS_USB_RESET
#define PDS_CR_PDS_USB_RESET_POS                     (21U)
#define PDS_CR_PDS_USB_RESET_LEN                     (1U)
#define PDS_CR_PDS_USB_RESET_MSK                     (((1U << PDS_CR_PDS_USB_RESET_LEN) - 1) << PDS_CR_PDS_USB_RESET_POS)
#define PDS_CR_PDS_USB_RESET_UMSK                    (~(((1U << PDS_CR_PDS_USB_RESET_LEN) - 1) << PDS_CR_PDS_USB_RESET_POS))
#define PDS_CR_PDS_USB_MEM_STBY                      PDS_CR_PDS_USB_MEM_STBY
#define PDS_CR_PDS_USB_MEM_STBY_POS                  (22U)
#define PDS_CR_PDS_USB_MEM_STBY_LEN                  (1U)
#define PDS_CR_PDS_USB_MEM_STBY_MSK                  (((1U << PDS_CR_PDS_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_USB_MEM_STBY_POS)
#define PDS_CR_PDS_USB_MEM_STBY_UMSK                 (~(((1U << PDS_CR_PDS_USB_MEM_STBY_LEN) - 1) << PDS_CR_PDS_USB_MEM_STBY_POS))
#define PDS_CR_PDS_USB_GATE_CLK                      PDS_CR_PDS_USB_GATE_CLK
#define PDS_CR_PDS_USB_GATE_CLK_POS                  (23U)
#define PDS_CR_PDS_USB_GATE_CLK_LEN                  (1U)
#define PDS_CR_PDS_USB_GATE_CLK_MSK                  (((1U << PDS_CR_PDS_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_USB_GATE_CLK_POS)
#define PDS_CR_PDS_USB_GATE_CLK_UMSK                 (~(((1U << PDS_CR_PDS_USB_GATE_CLK_LEN) - 1) << PDS_CR_PDS_USB_GATE_CLK_POS))
#define PDS_CR_PDS_MISC_PWR_OFF                      PDS_CR_PDS_MISC_PWR_OFF
#define PDS_CR_PDS_MISC_PWR_OFF_POS                  (24U)
#define PDS_CR_PDS_MISC_PWR_OFF_LEN                  (1U)
#define PDS_CR_PDS_MISC_PWR_OFF_MSK                  (((1U << PDS_CR_PDS_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_MISC_PWR_OFF_POS)
#define PDS_CR_PDS_MISC_PWR_OFF_UMSK                 (~(((1U << PDS_CR_PDS_MISC_PWR_OFF_LEN) - 1) << PDS_CR_PDS_MISC_PWR_OFF_POS))
#define PDS_CR_PDS_MISC_RESET                        PDS_CR_PDS_MISC_RESET
#define PDS_CR_PDS_MISC_RESET_POS                    (25U)
#define PDS_CR_PDS_MISC_RESET_LEN                    (1U)
#define PDS_CR_PDS_MISC_RESET_MSK                    (((1U << PDS_CR_PDS_MISC_RESET_LEN) - 1) << PDS_CR_PDS_MISC_RESET_POS)
#define PDS_CR_PDS_MISC_RESET_UMSK                   (~(((1U << PDS_CR_PDS_MISC_RESET_LEN) - 1) << PDS_CR_PDS_MISC_RESET_POS))
#define PDS_CR_PDS_MISC_MEM_STBY                     PDS_CR_PDS_MISC_MEM_STBY
#define PDS_CR_PDS_MISC_MEM_STBY_POS                 (26U)
#define PDS_CR_PDS_MISC_MEM_STBY_LEN                 (1U)
#define PDS_CR_PDS_MISC_MEM_STBY_MSK                 (((1U << PDS_CR_PDS_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MISC_MEM_STBY_POS)
#define PDS_CR_PDS_MISC_MEM_STBY_UMSK                (~(((1U << PDS_CR_PDS_MISC_MEM_STBY_LEN) - 1) << PDS_CR_PDS_MISC_MEM_STBY_POS))
#define PDS_CR_PDS_MISC_GATE_CLK                     PDS_CR_PDS_MISC_GATE_CLK
#define PDS_CR_PDS_MISC_GATE_CLK_POS                 (27U)
#define PDS_CR_PDS_MISC_GATE_CLK_LEN                 (1U)
#define PDS_CR_PDS_MISC_GATE_CLK_MSK                 (((1U << PDS_CR_PDS_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_MISC_GATE_CLK_POS)
#define PDS_CR_PDS_MISC_GATE_CLK_UMSK                (~(((1U << PDS_CR_PDS_MISC_GATE_CLK_LEN) - 1) << PDS_CR_PDS_MISC_GATE_CLK_POS))

/* 0x1C : pds_stat */
#define PDS_STAT_OFFSET                              (0x1C)
#define PDS_RO_PDS_STATE                             PDS_RO_PDS_STATE
#define PDS_RO_PDS_STATE_POS                         (0U)
#define PDS_RO_PDS_STATE_LEN                         (5U)
#define PDS_RO_PDS_STATE_MSK                         (((1U << PDS_RO_PDS_STATE_LEN) - 1) << PDS_RO_PDS_STATE_POS)
#define PDS_RO_PDS_STATE_UMSK                        (~(((1U << PDS_RO_PDS_STATE_LEN) - 1) << PDS_RO_PDS_STATE_POS))
#define PDS_RO_PDS_RF_STATE                          PDS_RO_PDS_RF_STATE
#define PDS_RO_PDS_RF_STATE_POS                      (8U)
#define PDS_RO_PDS_RF_STATE_LEN                      (5U)
#define PDS_RO_PDS_RF_STATE_MSK                      (((1U << PDS_RO_PDS_RF_STATE_LEN) - 1) << PDS_RO_PDS_RF_STATE_POS)
#define PDS_RO_PDS_RF_STATE_UMSK                     (~(((1U << PDS_RO_PDS_RF_STATE_LEN) - 1) << PDS_RO_PDS_RF_STATE_POS))
#define PDS_RESET_EVENT                              PDS_RESET_EVENT
#define PDS_RESET_EVENT_POS                          (24U)
#define PDS_RESET_EVENT_LEN                          (3U)
#define PDS_RESET_EVENT_MSK                          (((1U << PDS_RESET_EVENT_LEN) - 1) << PDS_RESET_EVENT_POS)
#define PDS_RESET_EVENT_UMSK                         (~(((1U << PDS_RESET_EVENT_LEN) - 1) << PDS_RESET_EVENT_POS))
#define PDS_CLR_RESET_EVENT                          PDS_CLR_RESET_EVENT
#define PDS_CLR_RESET_EVENT_POS                      (31U)
#define PDS_CLR_RESET_EVENT_LEN                      (1U)
#define PDS_CLR_RESET_EVENT_MSK                      (((1U << PDS_CLR_RESET_EVENT_LEN) - 1) << PDS_CLR_RESET_EVENT_POS)
#define PDS_CLR_RESET_EVENT_UMSK                     (~(((1U << PDS_CLR_RESET_EVENT_LEN) - 1) << PDS_CLR_RESET_EVENT_POS))

/* 0x20 : pds_ram1 */
#define PDS_RAM1_OFFSET                              (0x20)
#define PDS_CR_PDS_RAM_CLK_CNT                       PDS_CR_PDS_RAM_CLK_CNT
#define PDS_CR_PDS_RAM_CLK_CNT_POS                   (8U)
#define PDS_CR_PDS_RAM_CLK_CNT_LEN                   (6U)
#define PDS_CR_PDS_RAM_CLK_CNT_MSK                   (((1U << PDS_CR_PDS_RAM_CLK_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK_CNT_POS)
#define PDS_CR_PDS_RAM_CLK_CNT_UMSK                  (~(((1U << PDS_CR_PDS_RAM_CLK_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK_CNT_POS))
#define PDS_CR_PDS_RAM_CLK2_CNT                      PDS_CR_PDS_RAM_CLK2_CNT
#define PDS_CR_PDS_RAM_CLK2_CNT_POS                  (16U)
#define PDS_CR_PDS_RAM_CLK2_CNT_LEN                  (6U)
#define PDS_CR_PDS_RAM_CLK2_CNT_MSK                  (((1U << PDS_CR_PDS_RAM_CLK2_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK2_CNT_POS)
#define PDS_CR_PDS_RAM_CLK2_CNT_UMSK                 (~(((1U << PDS_CR_PDS_RAM_CLK2_CNT_LEN) - 1) << PDS_CR_PDS_RAM_CLK2_CNT_POS))
#define PDS_CR_PDS_CTRL_NP_RAM_CLK                   PDS_CR_PDS_CTRL_NP_RAM_CLK
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_POS               (24U)
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_LEN               (1U)
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_MSK               (((1U << PDS_CR_PDS_CTRL_NP_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_NP_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_NP_RAM_CLK_UMSK              (~(((1U << PDS_CR_PDS_CTRL_NP_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_NP_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_WB_RAM_CLK                   PDS_CR_PDS_CTRL_WB_RAM_CLK
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_POS               (26U)
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_LEN               (1U)
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_MSK               (((1U << PDS_CR_PDS_CTRL_WB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_WB_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_WB_RAM_CLK_UMSK              (~(((1U << PDS_CR_PDS_CTRL_WB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_WB_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_USB_RAM_CLK                  PDS_CR_PDS_CTRL_USB_RAM_CLK
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_POS              (27U)
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_LEN              (1U)
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_MSK              (((1U << PDS_CR_PDS_CTRL_USB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_USB_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_USB_RAM_CLK_UMSK             (~(((1U << PDS_CR_PDS_CTRL_USB_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_USB_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK                 PDS_CR_PDS_CTRL_MISC_RAM_CLK
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_POS             (28U)
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_LEN             (1U)
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_MSK             (((1U << PDS_CR_PDS_CTRL_MISC_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_MISC_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_MISC_RAM_CLK_UMSK            (~(((1U << PDS_CR_PDS_CTRL_MISC_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_MISC_RAM_CLK_POS))
#define PDS_CR_PDS_CTRL_RAM_CLK2                     PDS_CR_PDS_CTRL_RAM_CLK2
#define PDS_CR_PDS_CTRL_RAM_CLK2_POS                 (30U)
#define PDS_CR_PDS_CTRL_RAM_CLK2_LEN                 (1U)
#define PDS_CR_PDS_CTRL_RAM_CLK2_MSK                 (((1U << PDS_CR_PDS_CTRL_RAM_CLK2_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK2_POS)
#define PDS_CR_PDS_CTRL_RAM_CLK2_UMSK                (~(((1U << PDS_CR_PDS_CTRL_RAM_CLK2_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK2_POS))
#define PDS_CR_PDS_CTRL_RAM_CLK                      PDS_CR_PDS_CTRL_RAM_CLK
#define PDS_CR_PDS_CTRL_RAM_CLK_POS                  (31U)
#define PDS_CR_PDS_CTRL_RAM_CLK_LEN                  (1U)
#define PDS_CR_PDS_CTRL_RAM_CLK_MSK                  (((1U << PDS_CR_PDS_CTRL_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK_POS)
#define PDS_CR_PDS_CTRL_RAM_CLK_UMSK                 (~(((1U << PDS_CR_PDS_CTRL_RAM_CLK_LEN) - 1) << PDS_CR_PDS_CTRL_RAM_CLK_POS))

/* 0x24 : PDS_CTL5 */
#define PDS_CTL5_OFFSET                              (0x24)
#define PDS_CR_NP_WFI_MASK                           PDS_CR_NP_WFI_MASK
#define PDS_CR_NP_WFI_MASK_POS                       (0U)
#define PDS_CR_NP_WFI_MASK_LEN                       (1U)
#define PDS_CR_NP_WFI_MASK_MSK                       (((1U << PDS_CR_NP_WFI_MASK_LEN) - 1) << PDS_CR_NP_WFI_MASK_POS)
#define PDS_CR_NP_WFI_MASK_UMSK                      (~(((1U << PDS_CR_NP_WFI_MASK_LEN) - 1) << PDS_CR_NP_WFI_MASK_POS))
#define PDS_CR_PDS_PAD_OD_EN                         PDS_CR_PDS_PAD_OD_EN
#define PDS_CR_PDS_PAD_OD_EN_POS                     (4U)
#define PDS_CR_PDS_PAD_OD_EN_LEN                     (4U)
#define PDS_CR_PDS_PAD_OD_EN_MSK                     (((1U << PDS_CR_PDS_PAD_OD_EN_LEN) - 1) << PDS_CR_PDS_PAD_OD_EN_POS)
#define PDS_CR_PDS_PAD_OD_EN_UMSK                    (~(((1U << PDS_CR_PDS_PAD_OD_EN_LEN) - 1) << PDS_CR_PDS_PAD_OD_EN_POS))
#define PDS_CR_PDS_CTRL_USB33                        PDS_CR_PDS_CTRL_USB33
#define PDS_CR_PDS_CTRL_USB33_POS                    (8U)
#define PDS_CR_PDS_CTRL_USB33_LEN                    (1U)
#define PDS_CR_PDS_CTRL_USB33_MSK                    (((1U << PDS_CR_PDS_CTRL_USB33_LEN) - 1) << PDS_CR_PDS_CTRL_USB33_POS)
#define PDS_CR_PDS_CTRL_USB33_UMSK                   (~(((1U << PDS_CR_PDS_CTRL_USB33_LEN) - 1) << PDS_CR_PDS_CTRL_USB33_POS))

/* 0x28 : PDS_RAM2 */
#define PDS_RAM2_OFFSET                              (0x28)
#define PDS_CR_WRAM_SLP                              PDS_CR_WRAM_SLP
#define PDS_CR_WRAM_SLP_POS                          (0U)
#define PDS_CR_WRAM_SLP_LEN                          (10U)
#define PDS_CR_WRAM_SLP_MSK                          (((1U << PDS_CR_WRAM_SLP_LEN) - 1) << PDS_CR_WRAM_SLP_POS)
#define PDS_CR_WRAM_SLP_UMSK                         (~(((1U << PDS_CR_WRAM_SLP_LEN) - 1) << PDS_CR_WRAM_SLP_POS))
#define PDS_CR_WRAM_RET                              PDS_CR_WRAM_RET
#define PDS_CR_WRAM_RET_POS                          (10U)
#define PDS_CR_WRAM_RET_LEN                          (10U)
#define PDS_CR_WRAM_RET_MSK                          (((1U << PDS_CR_WRAM_RET_LEN) - 1) << PDS_CR_WRAM_RET_POS)
#define PDS_CR_WRAM_RET_UMSK                         (~(((1U << PDS_CR_WRAM_RET_LEN) - 1) << PDS_CR_WRAM_RET_POS))

/* 0x30 : pds_gpio_int_msk_set */
#define PDS_GPIO_INT_MSK_SET_OFFSET                  (0x30)
#define PDS_CR_PDS_GPIO_SET_INT_MASK                 PDS_CR_PDS_GPIO_SET_INT_MASK
#define PDS_CR_PDS_GPIO_SET_INT_MASK_POS             (0U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK_LEN             (31U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK_MSK             (((1U << PDS_CR_PDS_GPIO_SET_INT_MASK_LEN) - 1) << PDS_CR_PDS_GPIO_SET_INT_MASK_POS)
#define PDS_CR_PDS_GPIO_SET_INT_MASK_UMSK            (~(((1U << PDS_CR_PDS_GPIO_SET_INT_MASK_LEN) - 1) << PDS_CR_PDS_GPIO_SET_INT_MASK_POS))

/* 0x34 : pds_gpio_int_clr */
#define PDS_GPIO_INT_CLR_OFFSET                      (0x34)
#define PDS_GPIO_INT_CLR                             PDS_GPIO_INT_CLR
#define PDS_GPIO_INT_CLR_POS                         (0U)
#define PDS_GPIO_INT_CLR_LEN                         (31U)
#define PDS_GPIO_INT_CLR_MSK                         (((1U << PDS_GPIO_INT_CLR_LEN) - 1) << PDS_GPIO_INT_CLR_POS)
#define PDS_GPIO_INT_CLR_UMSK                        (~(((1U << PDS_GPIO_INT_CLR_LEN) - 1) << PDS_GPIO_INT_CLR_POS))

/* 0x38 : pds_gpio_stat */
#define PDS_GPIO_STAT_OFFSET                         (0x38)
#define PDS_GPIO_INT_STAT                            PDS_GPIO_INT_STAT
#define PDS_GPIO_INT_STAT_POS                        (0U)
#define PDS_GPIO_INT_STAT_LEN                        (31U)
#define PDS_GPIO_INT_STAT_MSK                        (((1U << PDS_GPIO_INT_STAT_LEN) - 1) << PDS_GPIO_INT_STAT_POS)
#define PDS_GPIO_INT_STAT_UMSK                       (~(((1U << PDS_GPIO_INT_STAT_LEN) - 1) << PDS_GPIO_INT_STAT_POS))

/* 0x3C : PDS_RAM3 */
#define PDS_RAM3_OFFSET                              (0x3C)
#define PDS_CR_OCRAM_RET                             PDS_CR_OCRAM_RET
#define PDS_CR_OCRAM_RET_POS                         (0U)
#define PDS_CR_OCRAM_RET_LEN                         (20U)
#define PDS_CR_OCRAM_RET_MSK                         (((1U << PDS_CR_OCRAM_RET_LEN) - 1) << PDS_CR_OCRAM_RET_POS)
#define PDS_CR_OCRAM_RET_UMSK                        (~(((1U << PDS_CR_OCRAM_RET_LEN) - 1) << PDS_CR_OCRAM_RET_POS))

/* 0x40 : PDS_RAM4 */
#define PDS_RAM4_OFFSET                              (0x40)
#define PDS_CR_OCRAM_SLP                             PDS_CR_OCRAM_SLP
#define PDS_CR_OCRAM_SLP_POS                         (0U)
#define PDS_CR_OCRAM_SLP_LEN                         (20U)
#define PDS_CR_OCRAM_SLP_MSK                         (((1U << PDS_CR_OCRAM_SLP_LEN) - 1) << PDS_CR_OCRAM_SLP_POS)
#define PDS_CR_OCRAM_SLP_UMSK                        (~(((1U << PDS_CR_OCRAM_SLP_LEN) - 1) << PDS_CR_OCRAM_SLP_POS))

/* 0x4C : pds_gpio13_6_int_mode */
#define PDS_GPIO13_6_INT_MODE_OFFSET                 (0x4C)
#define PDS_CR_PDS_GPIO6_INT_MODE                    PDS_CR_PDS_GPIO6_INT_MODE
#define PDS_CR_PDS_GPIO6_INT_MODE_POS                (0U)
#define PDS_CR_PDS_GPIO6_INT_MODE_LEN                (4U)
#define PDS_CR_PDS_GPIO6_INT_MODE_MSK                (((1U << PDS_CR_PDS_GPIO6_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO6_INT_MODE_POS)
#define PDS_CR_PDS_GPIO6_INT_MODE_UMSK               (~(((1U << PDS_CR_PDS_GPIO6_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO6_INT_MODE_POS))
#define PDS_CR_PDS_GPIO7_INT_MODE                    PDS_CR_PDS_GPIO7_INT_MODE
#define PDS_CR_PDS_GPIO7_INT_MODE_POS                (4U)
#define PDS_CR_PDS_GPIO7_INT_MODE_LEN                (4U)
#define PDS_CR_PDS_GPIO7_INT_MODE_MSK                (((1U << PDS_CR_PDS_GPIO7_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO7_INT_MODE_POS)
#define PDS_CR_PDS_GPIO7_INT_MODE_UMSK               (~(((1U << PDS_CR_PDS_GPIO7_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO7_INT_MODE_POS))
#define PDS_CR_PDS_GPIO8_INT_MODE                    PDS_CR_PDS_GPIO8_INT_MODE
#define PDS_CR_PDS_GPIO8_INT_MODE_POS                (8U)
#define PDS_CR_PDS_GPIO8_INT_MODE_LEN                (4U)
#define PDS_CR_PDS_GPIO8_INT_MODE_MSK                (((1U << PDS_CR_PDS_GPIO8_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO8_INT_MODE_POS)
#define PDS_CR_PDS_GPIO8_INT_MODE_UMSK               (~(((1U << PDS_CR_PDS_GPIO8_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO8_INT_MODE_POS))
#define PDS_CR_PDS_GPIO9_INT_MODE                    PDS_CR_PDS_GPIO9_INT_MODE
#define PDS_CR_PDS_GPIO9_INT_MODE_POS                (12U)
#define PDS_CR_PDS_GPIO9_INT_MODE_LEN                (4U)
#define PDS_CR_PDS_GPIO9_INT_MODE_MSK                (((1U << PDS_CR_PDS_GPIO9_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO9_INT_MODE_POS)
#define PDS_CR_PDS_GPIO9_INT_MODE_UMSK               (~(((1U << PDS_CR_PDS_GPIO9_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO9_INT_MODE_POS))
#define PDS_CR_PDS_GPIO10_INT_MODE                   PDS_CR_PDS_GPIO10_INT_MODE
#define PDS_CR_PDS_GPIO10_INT_MODE_POS               (16U)
#define PDS_CR_PDS_GPIO10_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO10_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO10_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO10_INT_MODE_POS)
#define PDS_CR_PDS_GPIO10_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO10_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO10_INT_MODE_POS))
#define PDS_CR_PDS_GPIO11_INT_MODE                   PDS_CR_PDS_GPIO11_INT_MODE
#define PDS_CR_PDS_GPIO11_INT_MODE_POS               (20U)
#define PDS_CR_PDS_GPIO11_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO11_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO11_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO11_INT_MODE_POS)
#define PDS_CR_PDS_GPIO11_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO11_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO11_INT_MODE_POS))
#define PDS_CR_PDS_GPIO12_INT_MODE                   PDS_CR_PDS_GPIO12_INT_MODE
#define PDS_CR_PDS_GPIO12_INT_MODE_POS               (24U)
#define PDS_CR_PDS_GPIO12_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO12_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO12_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO12_INT_MODE_POS)
#define PDS_CR_PDS_GPIO12_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO12_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO12_INT_MODE_POS))
#define PDS_CR_PDS_GPIO13_INT_MODE                   PDS_CR_PDS_GPIO13_INT_MODE
#define PDS_CR_PDS_GPIO13_INT_MODE_POS               (28U)
#define PDS_CR_PDS_GPIO13_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO13_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO13_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO13_INT_MODE_POS)
#define PDS_CR_PDS_GPIO13_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO13_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO13_INT_MODE_POS))

/* 0x50 : pds_gpio21_14_int_mode */
#define PDS_GPIO21_14_INT_MODE_OFFSET                (0x50)
#define PDS_CR_PDS_GPIO14_INT_MODE                   PDS_CR_PDS_GPIO14_INT_MODE
#define PDS_CR_PDS_GPIO14_INT_MODE_POS               (0U)
#define PDS_CR_PDS_GPIO14_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO14_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO14_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO14_INT_MODE_POS)
#define PDS_CR_PDS_GPIO14_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO14_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO14_INT_MODE_POS))
#define PDS_CR_PDS_GPIO15_INT_MODE                   PDS_CR_PDS_GPIO15_INT_MODE
#define PDS_CR_PDS_GPIO15_INT_MODE_POS               (4U)
#define PDS_CR_PDS_GPIO15_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO15_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO15_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO15_INT_MODE_POS)
#define PDS_CR_PDS_GPIO15_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO15_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO15_INT_MODE_POS))
#define PDS_CR_PDS_GPIO16_INT_MODE                   PDS_CR_PDS_GPIO16_INT_MODE
#define PDS_CR_PDS_GPIO16_INT_MODE_POS               (8U)
#define PDS_CR_PDS_GPIO16_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO16_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO16_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO16_INT_MODE_POS)
#define PDS_CR_PDS_GPIO16_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO16_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO16_INT_MODE_POS))
#define PDS_CR_PDS_GPIO17_INT_MODE                   PDS_CR_PDS_GPIO17_INT_MODE
#define PDS_CR_PDS_GPIO17_INT_MODE_POS               (12U)
#define PDS_CR_PDS_GPIO17_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO17_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO17_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO17_INT_MODE_POS)
#define PDS_CR_PDS_GPIO17_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO17_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO17_INT_MODE_POS))
#define PDS_CR_PDS_GPIO18_INT_MODE                   PDS_CR_PDS_GPIO18_INT_MODE
#define PDS_CR_PDS_GPIO18_INT_MODE_POS               (16U)
#define PDS_CR_PDS_GPIO18_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO18_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO18_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO18_INT_MODE_POS)
#define PDS_CR_PDS_GPIO18_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO18_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO18_INT_MODE_POS))
#define PDS_CR_PDS_GPIO19_INT_MODE                   PDS_CR_PDS_GPIO19_INT_MODE
#define PDS_CR_PDS_GPIO19_INT_MODE_POS               (20U)
#define PDS_CR_PDS_GPIO19_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO19_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO19_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO19_INT_MODE_POS)
#define PDS_CR_PDS_GPIO19_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO19_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO19_INT_MODE_POS))
#define PDS_CR_PDS_GPIO20_INT_MODE                   PDS_CR_PDS_GPIO20_INT_MODE
#define PDS_CR_PDS_GPIO20_INT_MODE_POS               (24U)
#define PDS_CR_PDS_GPIO20_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO20_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO20_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO20_INT_MODE_POS)
#define PDS_CR_PDS_GPIO20_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO20_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO20_INT_MODE_POS))
#define PDS_CR_PDS_GPIO21_INT_MODE                   PDS_CR_PDS_GPIO21_INT_MODE
#define PDS_CR_PDS_GPIO21_INT_MODE_POS               (28U)
#define PDS_CR_PDS_GPIO21_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO21_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO21_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO21_INT_MODE_POS)
#define PDS_CR_PDS_GPIO21_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO21_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO21_INT_MODE_POS))

/* 0x54 : pds_gpio29_22_int_mode */
#define PDS_GPIO29_22_INT_MODE_OFFSET                (0x54)
#define PDS_CR_PDS_GPIO22_INT_MODE                   PDS_CR_PDS_GPIO22_INT_MODE
#define PDS_CR_PDS_GPIO22_INT_MODE_POS               (0U)
#define PDS_CR_PDS_GPIO22_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO22_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO22_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO22_INT_MODE_POS)
#define PDS_CR_PDS_GPIO22_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO22_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO22_INT_MODE_POS))
#define PDS_CR_PDS_GPIO23_INT_MODE                   PDS_CR_PDS_GPIO23_INT_MODE
#define PDS_CR_PDS_GPIO23_INT_MODE_POS               (4U)
#define PDS_CR_PDS_GPIO23_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO23_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO23_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO23_INT_MODE_POS)
#define PDS_CR_PDS_GPIO23_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO23_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO23_INT_MODE_POS))
#define PDS_CR_PDS_GPIO24_INT_MODE                   PDS_CR_PDS_GPIO24_INT_MODE
#define PDS_CR_PDS_GPIO24_INT_MODE_POS               (8U)
#define PDS_CR_PDS_GPIO24_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO24_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO24_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO24_INT_MODE_POS)
#define PDS_CR_PDS_GPIO24_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO24_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO24_INT_MODE_POS))
#define PDS_CR_PDS_GPIO25_INT_MODE                   PDS_CR_PDS_GPIO25_INT_MODE
#define PDS_CR_PDS_GPIO25_INT_MODE_POS               (12U)
#define PDS_CR_PDS_GPIO25_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO25_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO25_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO25_INT_MODE_POS)
#define PDS_CR_PDS_GPIO25_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO25_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO25_INT_MODE_POS))
#define PDS_CR_PDS_GPIO26_INT_MODE                   PDS_CR_PDS_GPIO26_INT_MODE
#define PDS_CR_PDS_GPIO26_INT_MODE_POS               (16U)
#define PDS_CR_PDS_GPIO26_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO26_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO26_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO26_INT_MODE_POS)
#define PDS_CR_PDS_GPIO26_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO26_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO26_INT_MODE_POS))
#define PDS_CR_PDS_GPIO27_INT_MODE                   PDS_CR_PDS_GPIO27_INT_MODE
#define PDS_CR_PDS_GPIO27_INT_MODE_POS               (20U)
#define PDS_CR_PDS_GPIO27_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO27_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO27_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO27_INT_MODE_POS)
#define PDS_CR_PDS_GPIO27_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO27_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO27_INT_MODE_POS))
#define PDS_CR_PDS_GPIO28_INT_MODE                   PDS_CR_PDS_GPIO28_INT_MODE
#define PDS_CR_PDS_GPIO28_INT_MODE_POS               (24U)
#define PDS_CR_PDS_GPIO28_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO28_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO28_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO28_INT_MODE_POS)
#define PDS_CR_PDS_GPIO28_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO28_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO28_INT_MODE_POS))
#define PDS_CR_PDS_GPIO29_INT_MODE                   PDS_CR_PDS_GPIO29_INT_MODE
#define PDS_CR_PDS_GPIO29_INT_MODE_POS               (28U)
#define PDS_CR_PDS_GPIO29_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO29_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO29_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO29_INT_MODE_POS)
#define PDS_CR_PDS_GPIO29_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO29_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO29_INT_MODE_POS))

/* 0x58 : pds_gpio36_30_int_mode */
#define PDS_GPIO36_30_INT_MODE_OFFSET                (0x58)
#define PDS_CR_PDS_GPIO30_INT_MODE                   PDS_CR_PDS_GPIO30_INT_MODE
#define PDS_CR_PDS_GPIO30_INT_MODE_POS               (0U)
#define PDS_CR_PDS_GPIO30_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO30_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO30_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO30_INT_MODE_POS)
#define PDS_CR_PDS_GPIO30_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO30_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO30_INT_MODE_POS))
#define PDS_CR_PDS_GPIO31_INT_MODE                   PDS_CR_PDS_GPIO31_INT_MODE
#define PDS_CR_PDS_GPIO31_INT_MODE_POS               (4U)
#define PDS_CR_PDS_GPIO31_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO31_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO31_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO31_INT_MODE_POS)
#define PDS_CR_PDS_GPIO31_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO31_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO31_INT_MODE_POS))
#define PDS_CR_PDS_GPIO32_INT_MODE                   PDS_CR_PDS_GPIO32_INT_MODE
#define PDS_CR_PDS_GPIO32_INT_MODE_POS               (8U)
#define PDS_CR_PDS_GPIO32_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO32_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO32_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO32_INT_MODE_POS)
#define PDS_CR_PDS_GPIO32_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO32_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO32_INT_MODE_POS))
#define PDS_CR_PDS_GPIO33_INT_MODE                   PDS_CR_PDS_GPIO33_INT_MODE
#define PDS_CR_PDS_GPIO33_INT_MODE_POS               (12U)
#define PDS_CR_PDS_GPIO33_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO33_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO33_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO33_INT_MODE_POS)
#define PDS_CR_PDS_GPIO33_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO33_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO33_INT_MODE_POS))
#define PDS_CR_PDS_GPIO34_INT_MODE                   PDS_CR_PDS_GPIO34_INT_MODE
#define PDS_CR_PDS_GPIO34_INT_MODE_POS               (16U)
#define PDS_CR_PDS_GPIO34_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO34_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO34_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO34_INT_MODE_POS)
#define PDS_CR_PDS_GPIO34_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO34_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO34_INT_MODE_POS))
#define PDS_CR_PDS_GPIO35_INT_MODE                   PDS_CR_PDS_GPIO35_INT_MODE
#define PDS_CR_PDS_GPIO35_INT_MODE_POS               (20U)
#define PDS_CR_PDS_GPIO35_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO35_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO35_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO35_INT_MODE_POS)
#define PDS_CR_PDS_GPIO35_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO35_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO35_INT_MODE_POS))
#define PDS_CR_PDS_GPIO36_INT_MODE                   PDS_CR_PDS_GPIO36_INT_MODE
#define PDS_CR_PDS_GPIO36_INT_MODE_POS               (24U)
#define PDS_CR_PDS_GPIO36_INT_MODE_LEN               (4U)
#define PDS_CR_PDS_GPIO36_INT_MODE_MSK               (((1U << PDS_CR_PDS_GPIO36_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO36_INT_MODE_POS)
#define PDS_CR_PDS_GPIO36_INT_MODE_UMSK              (~(((1U << PDS_CR_PDS_GPIO36_INT_MODE_LEN) - 1) << PDS_CR_PDS_GPIO36_INT_MODE_POS))

/* 0x5C : pds_version */
#define PDS_VERSION_OFFSET                           (0x5C)
#define PDS_RESV_FOR_SOFTWARE                        PDS_RESV_FOR_SOFTWARE
#define PDS_RESV_FOR_SOFTWARE_POS                    (0U)
#define PDS_RESV_FOR_SOFTWARE_LEN                    (24U)
#define PDS_RESV_FOR_SOFTWARE_MSK                    (((1U << PDS_RESV_FOR_SOFTWARE_LEN) - 1) << PDS_RESV_FOR_SOFTWARE_POS)
#define PDS_RESV_FOR_SOFTWARE_UMSK                   (~(((1U << PDS_RESV_FOR_SOFTWARE_LEN) - 1) << PDS_RESV_FOR_SOFTWARE_POS))
#define PDS_VERSION_ID                               PDS_VERSION_ID
#define PDS_VERSION_ID_POS                           (24U)
#define PDS_VERSION_ID_LEN                           (8U)
#define PDS_VERSION_ID_MSK                           (((1U << PDS_VERSION_ID_LEN) - 1) << PDS_VERSION_ID_POS)
#define PDS_VERSION_ID_UMSK                          (~(((1U << PDS_VERSION_ID_LEN) - 1) << PDS_VERSION_ID_POS))

/* 0x60 : pds_ldo_soc_lp_ctrl */
#define PDS_LDO_SOC_LP_CTRL_OFFSET                   (0x60)
#define PDS_CR_PDS_LDO_SOC_LP_EN_CNT                 PDS_CR_PDS_LDO_SOC_LP_EN_CNT
#define PDS_CR_PDS_LDO_SOC_LP_EN_CNT_POS             (0U)
#define PDS_CR_PDS_LDO_SOC_LP_EN_CNT_LEN             (16U)
#define PDS_CR_PDS_LDO_SOC_LP_EN_CNT_MSK             (((1U << PDS_CR_PDS_LDO_SOC_LP_EN_CNT_LEN) - 1) << PDS_CR_PDS_LDO_SOC_LP_EN_CNT_POS)
#define PDS_CR_PDS_LDO_SOC_LP_EN_CNT_UMSK            (~(((1U << PDS_CR_PDS_LDO_SOC_LP_EN_CNT_LEN) - 1) << PDS_CR_PDS_LDO_SOC_LP_EN_CNT_POS))
#define PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT               PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT
#define PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_POS           (16U)
#define PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_LEN           (16U)
#define PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_MSK           (((1U << PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_LEN) - 1) << PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_POS)
#define PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_UMSK          (~(((1U << PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_LEN) - 1) << PDS_CR_PDS_LDO_SOC_EXIT_LP_CNT_POS))

/* 0x64 : pds_ldo13_lp_ctrl */
#define PDS_LDO13_LP_CTRL_OFFSET                     (0x64)
#define PDS_CR_PDS_LDO13_LP_EN_CNT                   PDS_CR_PDS_LDO13_LP_EN_CNT
#define PDS_CR_PDS_LDO13_LP_EN_CNT_POS               (0U)
#define PDS_CR_PDS_LDO13_LP_EN_CNT_LEN               (16U)
#define PDS_CR_PDS_LDO13_LP_EN_CNT_MSK               (((1U << PDS_CR_PDS_LDO13_LP_EN_CNT_LEN) - 1) << PDS_CR_PDS_LDO13_LP_EN_CNT_POS)
#define PDS_CR_PDS_LDO13_LP_EN_CNT_UMSK              (~(((1U << PDS_CR_PDS_LDO13_LP_EN_CNT_LEN) - 1) << PDS_CR_PDS_LDO13_LP_EN_CNT_POS))
#define PDS_CR_PDS_LDO13_EXIT_LP_CNT                 PDS_CR_PDS_LDO13_EXIT_LP_CNT
#define PDS_CR_PDS_LDO13_EXIT_LP_CNT_POS             (16U)
#define PDS_CR_PDS_LDO13_EXIT_LP_CNT_LEN             (16U)
#define PDS_CR_PDS_LDO13_EXIT_LP_CNT_MSK             (((1U << PDS_CR_PDS_LDO13_EXIT_LP_CNT_LEN) - 1) << PDS_CR_PDS_LDO13_EXIT_LP_CNT_POS)
#define PDS_CR_PDS_LDO13_EXIT_LP_CNT_UMSK            (~(((1U << PDS_CR_PDS_LDO13_EXIT_LP_CNT_LEN) - 1) << PDS_CR_PDS_LDO13_EXIT_LP_CNT_POS))

/* 0x68 : pds_slp_config */
#define PDS_SLP_CONFIG_OFFSET                        (0x68)
#define PDS_CR_PDS_EISO_CNT                          PDS_CR_PDS_EISO_CNT
#define PDS_CR_PDS_EISO_CNT_POS                      (0U)
#define PDS_CR_PDS_EISO_CNT_LEN                      (16U)
#define PDS_CR_PDS_EISO_CNT_MSK                      (((1U << PDS_CR_PDS_EISO_CNT_LEN) - 1) << PDS_CR_PDS_EISO_CNT_POS)
#define PDS_CR_PDS_EISO_CNT_UMSK                     (~(((1U << PDS_CR_PDS_EISO_CNT_LEN) - 1) << PDS_CR_PDS_EISO_CNT_POS))
#define PDS_CR_PDS_SLP_USE_FAST_CLK                  PDS_CR_PDS_SLP_USE_FAST_CLK
#define PDS_CR_PDS_SLP_USE_FAST_CLK_POS              (16U)
#define PDS_CR_PDS_SLP_USE_FAST_CLK_LEN              (1U)
#define PDS_CR_PDS_SLP_USE_FAST_CLK_MSK              (((1U << PDS_CR_PDS_SLP_USE_FAST_CLK_LEN) - 1) << PDS_CR_PDS_SLP_USE_FAST_CLK_POS)
#define PDS_CR_PDS_SLP_USE_FAST_CLK_UMSK             (~(((1U << PDS_CR_PDS_SLP_USE_FAST_CLK_LEN) - 1) << PDS_CR_PDS_SLP_USE_FAST_CLK_POS))
#define PDS_CR_PDS_SLP_LDO_SOC_LP                    PDS_CR_PDS_SLP_LDO_SOC_LP
#define PDS_CR_PDS_SLP_LDO_SOC_LP_POS                (17U)
#define PDS_CR_PDS_SLP_LDO_SOC_LP_LEN                (1U)
#define PDS_CR_PDS_SLP_LDO_SOC_LP_MSK                (((1U << PDS_CR_PDS_SLP_LDO_SOC_LP_LEN) - 1) << PDS_CR_PDS_SLP_LDO_SOC_LP_POS)
#define PDS_CR_PDS_SLP_LDO_SOC_LP_UMSK               (~(((1U << PDS_CR_PDS_SLP_LDO_SOC_LP_LEN) - 1) << PDS_CR_PDS_SLP_LDO_SOC_LP_POS))
#define PDS_CR_PDS_SLP_LDO13_LP                      PDS_CR_PDS_SLP_LDO13_LP
#define PDS_CR_PDS_SLP_LDO13_LP_POS                  (18U)
#define PDS_CR_PDS_SLP_LDO13_LP_LEN                  (1U)
#define PDS_CR_PDS_SLP_LDO13_LP_MSK                  (((1U << PDS_CR_PDS_SLP_LDO13_LP_LEN) - 1) << PDS_CR_PDS_SLP_LDO13_LP_POS)
#define PDS_CR_PDS_SLP_LDO13_LP_UMSK                 (~(((1U << PDS_CR_PDS_SLP_LDO13_LP_LEN) - 1) << PDS_CR_PDS_SLP_LDO13_LP_POS))
#define PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS      PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS
#define PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_POS  (19U)
#define PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_LEN  (1U)
#define PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_MSK  (((1U << PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_LEN) - 1) << PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_POS)
#define PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_UMSK (~(((1U << PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_LEN) - 1) << PDS_CR_PDS_DIS_PU_WHEN_FAST_CLK_OFF_DIS_POS))
#define PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST          PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST
#define PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_POS      (20U)
#define PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_LEN      (1U)
#define PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_MSK      (((1U << PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_LEN) - 1) << PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_POS)
#define PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_UMSK     (~(((1U << PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_LEN) - 1) << PDS_CR_PDS_WAKEUP_SW_FAST_CLK_FIRST_POS))

/* 0x6C : cpu_core_cfg1 */
#define PDS_CPU_CORE_CFG1_OFFSET                     (0x6C)
#define PDS_REG_PLL_SEL                              PDS_REG_PLL_SEL
#define PDS_REG_PLL_SEL_POS                          (4U)
#define PDS_REG_PLL_SEL_LEN                          (2U)
#define PDS_REG_PLL_SEL_MSK                          (((1U << PDS_REG_PLL_SEL_LEN) - 1) << PDS_REG_PLL_SEL_POS)
#define PDS_REG_PLL_SEL_UMSK                         (~(((1U << PDS_REG_PLL_SEL_LEN) - 1) << PDS_REG_PLL_SEL_POS))
#define PDS_REG_MCU1_CLK_EN                          PDS_REG_MCU1_CLK_EN
#define PDS_REG_MCU1_CLK_EN_POS                      (8U)
#define PDS_REG_MCU1_CLK_EN_LEN                      (1U)
#define PDS_REG_MCU1_CLK_EN_MSK                      (((1U << PDS_REG_MCU1_CLK_EN_LEN) - 1) << PDS_REG_MCU1_CLK_EN_POS)
#define PDS_REG_MCU1_CLK_EN_UMSK                     (~(((1U << PDS_REG_MCU1_CLK_EN_LEN) - 1) << PDS_REG_MCU1_CLK_EN_POS))

/* 0x88 : cpu_core_cfg14 */
#define PDS_CPU_CORE_CFG14_OFFSET                    (0x88)
#define PDS_E906_RST_ADDR                            PDS_E906_RST_ADDR
#define PDS_E906_RST_ADDR_POS                        (0U)
#define PDS_E906_RST_ADDR_LEN                        (32U)
#define PDS_E906_RST_ADDR_MSK                        (((1U << PDS_E906_RST_ADDR_LEN) - 1) << PDS_E906_RST_ADDR_POS)
#define PDS_E906_RST_ADDR_UMSK                       (~(((1U << PDS_E906_RST_ADDR_LEN) - 1) << PDS_E906_RST_ADDR_POS))

/* 0x9C :  */
#define PDS_RC32M_CTRL0_OFFSET                       (0x9C)
#define PDS_RC32M_CAL_DONE                           PDS_RC32M_CAL_DONE
#define PDS_RC32M_CAL_DONE_POS                       (0U)
#define PDS_RC32M_CAL_DONE_LEN                       (1U)
#define PDS_RC32M_CAL_DONE_MSK                       (((1U << PDS_RC32M_CAL_DONE_LEN) - 1) << PDS_RC32M_CAL_DONE_POS)
#define PDS_RC32M_CAL_DONE_UMSK                      (~(((1U << PDS_RC32M_CAL_DONE_LEN) - 1) << PDS_RC32M_CAL_DONE_POS))
#define PDS_RCHF_RDY                                 PDS_RCHF_RDY
#define PDS_RCHF_RDY_POS                             (1U)
#define PDS_RCHF_RDY_LEN                             (1U)
#define PDS_RCHF_RDY_MSK                             (((1U << PDS_RCHF_RDY_LEN) - 1) << PDS_RCHF_RDY_POS)
#define PDS_RCHF_RDY_UMSK                            (~(((1U << PDS_RCHF_RDY_LEN) - 1) << PDS_RCHF_RDY_POS))
#define PDS_RC32M_CAL_IN_PROGRESS                    PDS_RC32M_CAL_IN_PROGRESS
#define PDS_RC32M_CAL_IN_PROGRESS_POS                (2U)
#define PDS_RC32M_CAL_IN_PROGRESS_LEN                (1U)
#define PDS_RC32M_CAL_IN_PROGRESS_MSK                (((1U << PDS_RC32M_CAL_IN_PROGRESS_LEN) - 1) << PDS_RC32M_CAL_IN_PROGRESS_POS)
#define PDS_RC32M_CAL_IN_PROGRESS_UMSK               (~(((1U << PDS_RC32M_CAL_IN_PROGRESS_LEN) - 1) << PDS_RC32M_CAL_IN_PROGRESS_POS))
#define PDS_RC32M_CAL_DIV                            PDS_RC32M_CAL_DIV
#define PDS_RC32M_CAL_DIV_POS                        (3U)
#define PDS_RC32M_CAL_DIV_LEN                        (2U)
#define PDS_RC32M_CAL_DIV_MSK                        (((1U << PDS_RC32M_CAL_DIV_LEN) - 1) << PDS_RC32M_CAL_DIV_POS)
#define PDS_RC32M_CAL_DIV_UMSK                       (~(((1U << PDS_RC32M_CAL_DIV_LEN) - 1) << PDS_RC32M_CAL_DIV_POS))
#define PDS_RC32M_CAL_PRECHARGE                      PDS_RC32M_CAL_PRECHARGE
#define PDS_RC32M_CAL_PRECHARGE_POS                  (5U)
#define PDS_RC32M_CAL_PRECHARGE_LEN                  (1U)
#define PDS_RC32M_CAL_PRECHARGE_MSK                  (((1U << PDS_RC32M_CAL_PRECHARGE_LEN) - 1) << PDS_RC32M_CAL_PRECHARGE_POS)
#define PDS_RC32M_CAL_PRECHARGE_UMSK                 (~(((1U << PDS_RC32M_CAL_PRECHARGE_LEN) - 1) << PDS_RC32M_CAL_PRECHARGE_POS))
#define PDS_RC32M_ALLOW_CAL                          PDS_RC32M_ALLOW_CAL
#define PDS_RC32M_ALLOW_CAL_POS                      (17U)
#define PDS_RC32M_ALLOW_CAL_LEN                      (1U)
#define PDS_RC32M_ALLOW_CAL_MSK                      (((1U << PDS_RC32M_ALLOW_CAL_LEN) - 1) << PDS_RC32M_ALLOW_CAL_POS)
#define PDS_RC32M_ALLOW_CAL_UMSK                     (~(((1U << PDS_RC32M_ALLOW_CAL_LEN) - 1) << PDS_RC32M_ALLOW_CAL_POS))
#define PDS_RC32M_REFCLK_HALF                        PDS_RC32M_REFCLK_HALF
#define PDS_RC32M_REFCLK_HALF_POS                    (18U)
#define PDS_RC32M_REFCLK_HALF_LEN                    (1U)
#define PDS_RC32M_REFCLK_HALF_MSK                    (((1U << PDS_RC32M_REFCLK_HALF_LEN) - 1) << PDS_RC32M_REFCLK_HALF_POS)
#define PDS_RC32M_REFCLK_HALF_UMSK                   (~(((1U << PDS_RC32M_REFCLK_HALF_LEN) - 1) << PDS_RC32M_REFCLK_HALF_POS))
#define PDS_RC32M_CAL_EN                             PDS_RC32M_CAL_EN
#define PDS_RC32M_CAL_EN_POS                         (20U)
#define PDS_RC32M_CAL_EN_LEN                         (1U)
#define PDS_RC32M_CAL_EN_MSK                         (((1U << PDS_RC32M_CAL_EN_LEN) - 1) << PDS_RC32M_CAL_EN_POS)
#define PDS_RC32M_CAL_EN_UMSK                        (~(((1U << PDS_RC32M_CAL_EN_LEN) - 1) << PDS_RC32M_CAL_EN_POS))
#define PDS_PU_RC32M                                 PDS_PU_RC32M
#define PDS_PU_RC32M_POS                             (21U)
#define PDS_PU_RC32M_LEN                             (1U)
#define PDS_PU_RC32M_MSK                             (((1U << PDS_PU_RC32M_LEN) - 1) << PDS_PU_RC32M_POS)
#define PDS_PU_RC32M_UMSK                            (~(((1U << PDS_PU_RC32M_LEN) - 1) << PDS_PU_RC32M_POS))

/* 0xA0 :  */
#define PDS_RC32M_CTRL1_OFFSET                       (0xA0)
#define PDS_TEN_RC32M                                PDS_TEN_RC32M
#define PDS_TEN_RC32M_POS                            (0U)
#define PDS_TEN_RC32M_LEN                            (1U)
#define PDS_TEN_RC32M_MSK                            (((1U << PDS_TEN_RC32M_LEN) - 1) << PDS_TEN_RC32M_POS)
#define PDS_TEN_RC32M_UMSK                           (~(((1U << PDS_TEN_RC32M_LEN) - 1) << PDS_TEN_RC32M_POS))
#define PDS_DTEN_RCHF                                PDS_DTEN_RCHF
#define PDS_DTEN_RCHF_POS                            (1U)
#define PDS_DTEN_RCHF_LEN                            (1U)
#define PDS_DTEN_RCHF_MSK                            (((1U << PDS_DTEN_RCHF_LEN) - 1) << PDS_DTEN_RCHF_POS)
#define PDS_DTEN_RCHF_UMSK                           (~(((1U << PDS_DTEN_RCHF_LEN) - 1) << PDS_DTEN_RCHF_POS))
#define PDS_RC32M_SOFT_RST                           PDS_RC32M_SOFT_RST
#define PDS_RC32M_SOFT_RST_POS                       (2U)
#define PDS_RC32M_SOFT_RST_LEN                       (1U)
#define PDS_RC32M_SOFT_RST_MSK                       (((1U << PDS_RC32M_SOFT_RST_LEN) - 1) << PDS_RC32M_SOFT_RST_POS)
#define PDS_RC32M_SOFT_RST_UMSK                      (~(((1U << PDS_RC32M_SOFT_RST_LEN) - 1) << PDS_RC32M_SOFT_RST_POS))
#define PDS_RC32M_CLK_SOFT_RST                       PDS_RC32M_CLK_SOFT_RST
#define PDS_RC32M_CLK_SOFT_RST_POS                   (3U)
#define PDS_RC32M_CLK_SOFT_RST_LEN                   (1U)
#define PDS_RC32M_CLK_SOFT_RST_MSK                   (((1U << PDS_RC32M_CLK_SOFT_RST_LEN) - 1) << PDS_RC32M_CLK_SOFT_RST_POS)
#define PDS_RC32M_CLK_SOFT_RST_UMSK                  (~(((1U << PDS_RC32M_CLK_SOFT_RST_LEN) - 1) << PDS_RC32M_CLK_SOFT_RST_POS))
#define PDS_RC32M_CLK_INV                            PDS_RC32M_CLK_INV
#define PDS_RC32M_CLK_INV_POS                        (4U)
#define PDS_RC32M_CLK_INV_LEN                        (1U)
#define PDS_RC32M_CLK_INV_MSK                        (((1U << PDS_RC32M_CLK_INV_LEN) - 1) << PDS_RC32M_CLK_INV_POS)
#define PDS_RC32M_CLK_INV_UMSK                       (~(((1U << PDS_RC32M_CLK_INV_LEN) - 1) << PDS_RC32M_CLK_INV_POS))
#define PDS_RC32M_CLK_FORCE_ON                       PDS_RC32M_CLK_FORCE_ON
#define PDS_RC32M_CLK_FORCE_ON_POS                   (8U)
#define PDS_RC32M_CLK_FORCE_ON_LEN                   (1U)
#define PDS_RC32M_CLK_FORCE_ON_MSK                   (((1U << PDS_RC32M_CLK_FORCE_ON_LEN) - 1) << PDS_RC32M_CLK_FORCE_ON_POS)
#define PDS_RC32M_CLK_FORCE_ON_UMSK                  (~(((1U << PDS_RC32M_CLK_FORCE_ON_LEN) - 1) << PDS_RC32M_CLK_FORCE_ON_POS))
#define PDS_RCHF_CAL_CODE_DOWN                       PDS_RCHF_CAL_CODE_DOWN
#define PDS_RCHF_CAL_CODE_DOWN_POS                   (9U)
#define PDS_RCHF_CAL_CODE_DOWN_LEN                   (1U)
#define PDS_RCHF_CAL_CODE_DOWN_MSK                   (((1U << PDS_RCHF_CAL_CODE_DOWN_LEN) - 1) << PDS_RCHF_CAL_CODE_DOWN_POS)
#define PDS_RCHF_CAL_CODE_DOWN_UMSK                  (~(((1U << PDS_RCHF_CAL_CODE_DOWN_LEN) - 1) << PDS_RCHF_CAL_CODE_DOWN_POS))
#define PDS_RCHF_CAL_STEP_DONE                       PDS_RCHF_CAL_STEP_DONE
#define PDS_RCHF_CAL_STEP_DONE_POS                   (10U)
#define PDS_RCHF_CAL_STEP_DONE_LEN                   (1U)
#define PDS_RCHF_CAL_STEP_DONE_MSK                   (((1U << PDS_RCHF_CAL_STEP_DONE_LEN) - 1) << PDS_RCHF_CAL_STEP_DONE_POS)
#define PDS_RCHF_CAL_STEP_DONE_UMSK                  (~(((1U << PDS_RCHF_CAL_STEP_DONE_LEN) - 1) << PDS_RCHF_CAL_STEP_DONE_POS))
#define PDS_RC32M_PD                                 PDS_RC32M_PD
#define PDS_RC32M_PD_POS                             (11U)
#define PDS_RC32M_PD_LEN                             (1U)
#define PDS_RC32M_PD_MSK                             (((1U << PDS_RC32M_PD_LEN) - 1) << PDS_RC32M_PD_POS)
#define PDS_RC32M_PD_UMSK                            (~(((1U << PDS_RC32M_PD_LEN) - 1) << PDS_RC32M_PD_POS))
#define PDS_RC32M_RESERVED                           PDS_RC32M_RESERVED
#define PDS_RC32M_RESERVED_POS                       (24U)
#define PDS_RC32M_RESERVED_LEN                       (8U)
#define PDS_RC32M_RESERVED_MSK                       (((1U << PDS_RC32M_RESERVED_LEN) - 1) << PDS_RC32M_RESERVED_POS)
#define PDS_RC32M_RESERVED_UMSK                      (~(((1U << PDS_RC32M_RESERVED_LEN) - 1) << PDS_RC32M_RESERVED_POS))

/* 0xA8 : pu_rst_clkpll */
#define PDS_PU_RST_CLKPLL_OFFSET                     (0xA8)
#define PDS_CR_PDS_PU_CLKPLL_SFREG                   PDS_CR_PDS_PU_CLKPLL_SFREG
#define PDS_CR_PDS_PU_CLKPLL_SFREG_POS               (9U)
#define PDS_CR_PDS_PU_CLKPLL_SFREG_LEN               (1U)
#define PDS_CR_PDS_PU_CLKPLL_SFREG_MSK               (((1U << PDS_CR_PDS_PU_CLKPLL_SFREG_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_SFREG_POS)
#define PDS_CR_PDS_PU_CLKPLL_SFREG_UMSK              (~(((1U << PDS_CR_PDS_PU_CLKPLL_SFREG_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_SFREG_POS))
#define PDS_CR_PDS_PU_CLKPLL                         PDS_CR_PDS_PU_CLKPLL
#define PDS_CR_PDS_PU_CLKPLL_POS                     (10U)
#define PDS_CR_PDS_PU_CLKPLL_LEN                     (1U)
#define PDS_CR_PDS_PU_CLKPLL_MSK                     (((1U << PDS_CR_PDS_PU_CLKPLL_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_POS)
#define PDS_CR_PDS_PU_CLKPLL_UMSK                    (~(((1U << PDS_CR_PDS_PU_CLKPLL_LEN) - 1) << PDS_CR_PDS_PU_CLKPLL_POS))

/* 0xC8 : usb_ctl */
#define PDS_USB_CTL_OFFSET                           (0xC8)

/* 0x500 : usb_ctrl_0 */
#define PDS_USB_CTRL_0_OFFSET                        (0x500)
#define PDS_CR_USB_SW_RST_N                          PDS_CR_USB_SW_RST_N
#define PDS_CR_USB_SW_RST_N_POS                      (0U)
#define PDS_CR_USB_SW_RST_N_LEN                      (1U)
#define PDS_CR_USB_SW_RST_N_MSK                      (((1U << PDS_CR_USB_SW_RST_N_LEN) - 1) << PDS_CR_USB_SW_RST_N_POS)
#define PDS_CR_USB_SW_RST_N_UMSK                     (~(((1U << PDS_CR_USB_SW_RST_N_LEN) - 1) << PDS_CR_USB_SW_RST_N_POS))
#define PDS_CR_USB_L1_WAKEUP                         PDS_CR_USB_L1_WAKEUP
#define PDS_CR_USB_L1_WAKEUP_POS                     (2U)
#define PDS_CR_USB_L1_WAKEUP_LEN                     (1U)
#define PDS_CR_USB_L1_WAKEUP_MSK                     (((1U << PDS_CR_USB_L1_WAKEUP_LEN) - 1) << PDS_CR_USB_L1_WAKEUP_POS)
#define PDS_CR_USB_L1_WAKEUP_UMSK                    (~(((1U << PDS_CR_USB_L1_WAKEUP_LEN) - 1) << PDS_CR_USB_L1_WAKEUP_POS))
#define PDS_CR_USB_WAKEUP                            PDS_CR_USB_WAKEUP
#define PDS_CR_USB_WAKEUP_POS                        (3U)
#define PDS_CR_USB_WAKEUP_LEN                        (1U)
#define PDS_CR_USB_WAKEUP_MSK                        (((1U << PDS_CR_USB_WAKEUP_LEN) - 1) << PDS_CR_USB_WAKEUP_POS)
#define PDS_CR_USB_WAKEUP_UMSK                       (~(((1U << PDS_CR_USB_WAKEUP_LEN) - 1) << PDS_CR_USB_WAKEUP_POS))
#define PDS_CR_USB_UCLK_LPM_DIS                      PDS_CR_USB_UCLK_LPM_DIS
#define PDS_CR_USB_UCLK_LPM_DIS_POS                  (7U)
#define PDS_CR_USB_UCLK_LPM_DIS_LEN                  (1U)
#define PDS_CR_USB_UCLK_LPM_DIS_MSK                  (((1U << PDS_CR_USB_UCLK_LPM_DIS_LEN) - 1) << PDS_CR_USB_UCLK_LPM_DIS_POS)
#define PDS_CR_USB_UCLK_LPM_DIS_UMSK                 (~(((1U << PDS_CR_USB_UCLK_LPM_DIS_LEN) - 1) << PDS_CR_USB_UCLK_LPM_DIS_POS))
#define PDS_REG_USB_EXT_SUSP_N                       PDS_REG_USB_EXT_SUSP_N
#define PDS_REG_USB_EXT_SUSP_N_POS                   (8U)
#define PDS_REG_USB_EXT_SUSP_N_LEN                   (1U)
#define PDS_REG_USB_EXT_SUSP_N_MSK                   (((1U << PDS_REG_USB_EXT_SUSP_N_LEN) - 1) << PDS_REG_USB_EXT_SUSP_N_POS)
#define PDS_REG_USB_EXT_SUSP_N_UMSK                  (~(((1U << PDS_REG_USB_EXT_SUSP_N_LEN) - 1) << PDS_REG_USB_EXT_SUSP_N_POS))
#define PDS_CR_USB_IDDIG                             PDS_CR_USB_IDDIG
#define PDS_CR_USB_IDDIG_POS                         (16U)
#define PDS_CR_USB_IDDIG_LEN                         (1U)
#define PDS_CR_USB_IDDIG_MSK                         (((1U << PDS_CR_USB_IDDIG_LEN) - 1) << PDS_CR_USB_IDDIG_POS)
#define PDS_CR_USB_IDDIG_UMSK                        (~(((1U << PDS_CR_USB_IDDIG_LEN) - 1) << PDS_CR_USB_IDDIG_POS))

/* 0x504 :  */
#define PDS_U2PHY_0_OFFSET                           (0x504)
#define PDS_PLLPTUNE                                 PDS_PLLPTUNE
#define PDS_PLLPTUNE_POS                             (0U)
#define PDS_PLLPTUNE_LEN                             (4U)
#define PDS_PLLPTUNE_MSK                             (((1U << PDS_PLLPTUNE_LEN) - 1) << PDS_PLLPTUNE_POS)
#define PDS_PLLPTUNE_UMSK                            (~(((1U << PDS_PLLPTUNE_LEN) - 1) << PDS_PLLPTUNE_POS))
#define PDS_PLLITUNE                                 PDS_PLLITUNE
#define PDS_PLLITUNE_POS                             (4U)
#define PDS_PLLITUNE_LEN                             (2U)
#define PDS_PLLITUNE_MSK                             (((1U << PDS_PLLITUNE_LEN) - 1) << PDS_PLLITUNE_POS)
#define PDS_PLLITUNE_UMSK                            (~(((1U << PDS_PLLITUNE_LEN) - 1) << PDS_PLLITUNE_POS))
#define PDS_PLLBTUNE                                 PDS_PLLBTUNE
#define PDS_PLLBTUNE_POS                             (6U)
#define PDS_PLLBTUNE_LEN                             (1U)
#define PDS_PLLBTUNE_MSK                             (((1U << PDS_PLLBTUNE_LEN) - 1) << PDS_PLLBTUNE_POS)
#define PDS_PLLBTUNE_UMSK                            (~(((1U << PDS_PLLBTUNE_LEN) - 1) << PDS_PLLBTUNE_POS))
#define PDS_OTGTUNE0                                 PDS_OTGTUNE0
#define PDS_OTGTUNE0_POS                             (7U)
#define PDS_OTGTUNE0_LEN                             (3U)
#define PDS_OTGTUNE0_MSK                             (((1U << PDS_OTGTUNE0_LEN) - 1) << PDS_OTGTUNE0_POS)
#define PDS_OTGTUNE0_UMSK                            (~(((1U << PDS_OTGTUNE0_LEN) - 1) << PDS_OTGTUNE0_POS))
#define PDS_OTGDISABLE0                              PDS_OTGDISABLE0
#define PDS_OTGDISABLE0_POS                          (10U)
#define PDS_OTGDISABLE0_LEN                          (1U)
#define PDS_OTGDISABLE0_MSK                          (((1U << PDS_OTGDISABLE0_LEN) - 1) << PDS_OTGDISABLE0_POS)
#define PDS_OTGDISABLE0_UMSK                         (~(((1U << PDS_OTGDISABLE0_LEN) - 1) << PDS_OTGDISABLE0_POS))
#define PDS_LOOPBACKENB0                             PDS_LOOPBACKENB0
#define PDS_LOOPBACKENB0_POS                         (11U)
#define PDS_LOOPBACKENB0_LEN                         (1U)
#define PDS_LOOPBACKENB0_MSK                         (((1U << PDS_LOOPBACKENB0_LEN) - 1) << PDS_LOOPBACKENB0_POS)
#define PDS_LOOPBACKENB0_UMSK                        (~(((1U << PDS_LOOPBACKENB0_LEN) - 1) << PDS_LOOPBACKENB0_POS))
#define PDS_IDPULLUP0                                PDS_IDPULLUP0
#define PDS_IDPULLUP0_POS                            (12U)
#define PDS_IDPULLUP0_LEN                            (1U)
#define PDS_IDPULLUP0_MSK                            (((1U << PDS_IDPULLUP0_LEN) - 1) << PDS_IDPULLUP0_POS)
#define PDS_IDPULLUP0_UMSK                           (~(((1U << PDS_IDPULLUP0_LEN) - 1) << PDS_IDPULLUP0_POS))
#define PDS_HSXCVREXTCTL0                            PDS_HSXCVREXTCTL0
#define PDS_HSXCVREXTCTL0_POS                        (13U)
#define PDS_HSXCVREXTCTL0_LEN                        (1U)
#define PDS_HSXCVREXTCTL0_MSK                        (((1U << PDS_HSXCVREXTCTL0_LEN) - 1) << PDS_HSXCVREXTCTL0_POS)
#define PDS_HSXCVREXTCTL0_UMSK                       (~(((1U << PDS_HSXCVREXTCTL0_LEN) - 1) << PDS_HSXCVREXTCTL0_POS))
#define PDS_FSXCVROWNER0                             PDS_FSXCVROWNER0
#define PDS_FSXCVROWNER0_POS                         (14U)
#define PDS_FSXCVROWNER0_LEN                         (1U)
#define PDS_FSXCVROWNER0_MSK                         (((1U << PDS_FSXCVROWNER0_LEN) - 1) << PDS_FSXCVROWNER0_POS)
#define PDS_FSXCVROWNER0_UMSK                        (~(((1U << PDS_FSXCVROWNER0_LEN) - 1) << PDS_FSXCVROWNER0_POS))
#define PDS_FSSE0EXT0                                PDS_FSSE0EXT0
#define PDS_FSSE0EXT0_POS                            (15U)
#define PDS_FSSE0EXT0_LEN                            (1U)
#define PDS_FSSE0EXT0_MSK                            (((1U << PDS_FSSE0EXT0_LEN) - 1) << PDS_FSSE0EXT0_POS)
#define PDS_FSSE0EXT0_UMSK                           (~(((1U << PDS_FSSE0EXT0_LEN) - 1) << PDS_FSSE0EXT0_POS))
#define PDS_FSEL                                     PDS_FSEL
#define PDS_FSEL_POS                                 (16U)
#define PDS_FSEL_LEN                                 (3U)
#define PDS_FSEL_MSK                                 (((1U << PDS_FSEL_LEN) - 1) << PDS_FSEL_POS)
#define PDS_FSEL_UMSK                                (~(((1U << PDS_FSEL_LEN) - 1) << PDS_FSEL_POS))
#define PDS_DRVVBUS0                                 PDS_DRVVBUS0
#define PDS_DRVVBUS0_POS                             (19U)
#define PDS_DRVVBUS0_LEN                             (1U)
#define PDS_DRVVBUS0_MSK                             (((1U << PDS_DRVVBUS0_LEN) - 1) << PDS_DRVVBUS0_POS)
#define PDS_DRVVBUS0_UMSK                            (~(((1U << PDS_DRVVBUS0_LEN) - 1) << PDS_DRVVBUS0_POS))
#define PDS_DCDENB0                                  PDS_DCDENB0
#define PDS_DCDENB0_POS                              (20U)
#define PDS_DCDENB0_LEN                              (1U)
#define PDS_DCDENB0_MSK                              (((1U << PDS_DCDENB0_LEN) - 1) << PDS_DCDENB0_POS)
#define PDS_DCDENB0_UMSK                             (~(((1U << PDS_DCDENB0_LEN) - 1) << PDS_DCDENB0_POS))
#define PDS_COMPDISTUNE0                             PDS_COMPDISTUNE0
#define PDS_COMPDISTUNE0_POS                         (21U)
#define PDS_COMPDISTUNE0_LEN                         (3U)
#define PDS_COMPDISTUNE0_MSK                         (((1U << PDS_COMPDISTUNE0_LEN) - 1) << PDS_COMPDISTUNE0_POS)
#define PDS_COMPDISTUNE0_UMSK                        (~(((1U << PDS_COMPDISTUNE0_LEN) - 1) << PDS_COMPDISTUNE0_POS))
#define PDS_COMMONONN                                PDS_COMMONONN
#define PDS_COMMONONN_POS                            (24U)
#define PDS_COMMONONN_LEN                            (1U)
#define PDS_COMMONONN_MSK                            (((1U << PDS_COMMONONN_LEN) - 1) << PDS_COMMONONN_POS)
#define PDS_COMMONONN_UMSK                           (~(((1U << PDS_COMMONONN_LEN) - 1) << PDS_COMMONONN_POS))
#define PDS_CHRGSEL0                                 PDS_CHRGSEL0
#define PDS_CHRGSEL0_POS                             (25U)
#define PDS_CHRGSEL0_LEN                             (1U)
#define PDS_CHRGSEL0_MSK                             (((1U << PDS_CHRGSEL0_LEN) - 1) << PDS_CHRGSEL0_POS)
#define PDS_CHRGSEL0_UMSK                            (~(((1U << PDS_CHRGSEL0_LEN) - 1) << PDS_CHRGSEL0_POS))
#define PDS_BYPASSSEL0                               PDS_BYPASSSEL0
#define PDS_BYPASSSEL0_POS                           (26U)
#define PDS_BYPASSSEL0_LEN                           (1U)
#define PDS_BYPASSSEL0_MSK                           (((1U << PDS_BYPASSSEL0_LEN) - 1) << PDS_BYPASSSEL0_POS)
#define PDS_BYPASSSEL0_UMSK                          (~(((1U << PDS_BYPASSSEL0_LEN) - 1) << PDS_BYPASSSEL0_POS))
#define PDS_BYPASSDPEN0                              PDS_BYPASSDPEN0
#define PDS_BYPASSDPEN0_POS                          (27U)
#define PDS_BYPASSDPEN0_LEN                          (1U)
#define PDS_BYPASSDPEN0_MSK                          (((1U << PDS_BYPASSDPEN0_LEN) - 1) << PDS_BYPASSDPEN0_POS)
#define PDS_BYPASSDPEN0_UMSK                         (~(((1U << PDS_BYPASSDPEN0_LEN) - 1) << PDS_BYPASSDPEN0_POS))
#define PDS_BYPASSDPDATA0                            PDS_BYPASSDPDATA0
#define PDS_BYPASSDPDATA0_POS                        (28U)
#define PDS_BYPASSDPDATA0_LEN                        (1U)
#define PDS_BYPASSDPDATA0_MSK                        (((1U << PDS_BYPASSDPDATA0_LEN) - 1) << PDS_BYPASSDPDATA0_POS)
#define PDS_BYPASSDPDATA0_UMSK                       (~(((1U << PDS_BYPASSDPDATA0_LEN) - 1) << PDS_BYPASSDPDATA0_POS))
#define PDS_BYPASSDMEN0                              PDS_BYPASSDMEN0
#define PDS_BYPASSDMEN0_POS                          (29U)
#define PDS_BYPASSDMEN0_LEN                          (1U)
#define PDS_BYPASSDMEN0_MSK                          (((1U << PDS_BYPASSDMEN0_LEN) - 1) << PDS_BYPASSDMEN0_POS)
#define PDS_BYPASSDMEN0_UMSK                         (~(((1U << PDS_BYPASSDMEN0_LEN) - 1) << PDS_BYPASSDMEN0_POS))
#define PDS_BYPASSDMDATA0                            PDS_BYPASSDMDATA0
#define PDS_BYPASSDMDATA0_POS                        (30U)
#define PDS_BYPASSDMDATA0_LEN                        (1U)
#define PDS_BYPASSDMDATA0_MSK                        (((1U << PDS_BYPASSDMDATA0_LEN) - 1) << PDS_BYPASSDMDATA0_POS)
#define PDS_BYPASSDMDATA0_UMSK                       (~(((1U << PDS_BYPASSDMDATA0_LEN) - 1) << PDS_BYPASSDMDATA0_POS))
#define PDS_ATERESET                                 PDS_ATERESET
#define PDS_ATERESET_POS                             (31U)
#define PDS_ATERESET_LEN                             (1U)
#define PDS_ATERESET_MSK                             (((1U << PDS_ATERESET_LEN) - 1) << PDS_ATERESET_POS)
#define PDS_ATERESET_UMSK                            (~(((1U << PDS_ATERESET_LEN) - 1) << PDS_ATERESET_POS))

/* 0x508 :  */
#define PDS_U2PHY_1_OFFSET                           (0x508)
#define PDS_SUSPENDM0_USE_REG                        PDS_SUSPENDM0_USE_REG
#define PDS_SUSPENDM0_USE_REG_POS                    (0U)
#define PDS_SUSPENDM0_USE_REG_LEN                    (1U)
#define PDS_SUSPENDM0_USE_REG_MSK                    (((1U << PDS_SUSPENDM0_USE_REG_LEN) - 1) << PDS_SUSPENDM0_USE_REG_POS)
#define PDS_SUSPENDM0_USE_REG_UMSK                   (~(((1U << PDS_SUSPENDM0_USE_REG_LEN) - 1) << PDS_SUSPENDM0_USE_REG_POS))
#define PDS_TXHSXVTUNE0                              PDS_TXHSXVTUNE0
#define PDS_TXHSXVTUNE0_POS                          (1U)
#define PDS_TXHSXVTUNE0_LEN                          (2U)
#define PDS_TXHSXVTUNE0_MSK                          (((1U << PDS_TXHSXVTUNE0_LEN) - 1) << PDS_TXHSXVTUNE0_POS)
#define PDS_TXHSXVTUNE0_UMSK                         (~(((1U << PDS_TXHSXVTUNE0_LEN) - 1) << PDS_TXHSXVTUNE0_POS))
#define PDS_TXFSLSTUNE0                              PDS_TXFSLSTUNE0
#define PDS_TXFSLSTUNE0_POS                          (3U)
#define PDS_TXFSLSTUNE0_LEN                          (4U)
#define PDS_TXFSLSTUNE0_MSK                          (((1U << PDS_TXFSLSTUNE0_LEN) - 1) << PDS_TXFSLSTUNE0_POS)
#define PDS_TXFSLSTUNE0_UMSK                         (~(((1U << PDS_TXFSLSTUNE0_LEN) - 1) << PDS_TXFSLSTUNE0_POS))
#define PDS_TXENABLEN0                               PDS_TXENABLEN0
#define PDS_TXENABLEN0_POS                           (7U)
#define PDS_TXENABLEN0_LEN                           (1U)
#define PDS_TXENABLEN0_MSK                           (((1U << PDS_TXENABLEN0_LEN) - 1) << PDS_TXENABLEN0_POS)
#define PDS_TXENABLEN0_UMSK                          (~(((1U << PDS_TXENABLEN0_LEN) - 1) << PDS_TXENABLEN0_POS))
#define PDS_TESTDATAOUTSEL0                          PDS_TESTDATAOUTSEL0
#define PDS_TESTDATAOUTSEL0_POS                      (8U)
#define PDS_TESTDATAOUTSEL0_LEN                      (1U)
#define PDS_TESTDATAOUTSEL0_MSK                      (((1U << PDS_TESTDATAOUTSEL0_LEN) - 1) << PDS_TESTDATAOUTSEL0_POS)
#define PDS_TESTDATAOUTSEL0_UMSK                     (~(((1U << PDS_TESTDATAOUTSEL0_LEN) - 1) << PDS_TESTDATAOUTSEL0_POS))
#define PDS_TESTDATAIN0                              PDS_TESTDATAIN0
#define PDS_TESTDATAIN0_POS                          (9U)
#define PDS_TESTDATAIN0_LEN                          (8U)
#define PDS_TESTDATAIN0_MSK                          (((1U << PDS_TESTDATAIN0_LEN) - 1) << PDS_TESTDATAIN0_POS)
#define PDS_TESTDATAIN0_UMSK                         (~(((1U << PDS_TESTDATAIN0_LEN) - 1) << PDS_TESTDATAIN0_POS))
#define PDS_TESTCLK0                                 PDS_TESTCLK0
#define PDS_TESTCLK0_POS                             (17U)
#define PDS_TESTCLK0_LEN                             (1U)
#define PDS_TESTCLK0_MSK                             (((1U << PDS_TESTCLK0_LEN) - 1) << PDS_TESTCLK0_POS)
#define PDS_TESTCLK0_UMSK                            (~(((1U << PDS_TESTCLK0_LEN) - 1) << PDS_TESTCLK0_POS))
#define PDS_TESTBURNIN                               PDS_TESTBURNIN
#define PDS_TESTBURNIN_POS                           (18U)
#define PDS_TESTBURNIN_LEN                           (1U)
#define PDS_TESTBURNIN_MSK                           (((1U << PDS_TESTBURNIN_LEN) - 1) << PDS_TESTBURNIN_POS)
#define PDS_TESTBURNIN_UMSK                          (~(((1U << PDS_TESTBURNIN_LEN) - 1) << PDS_TESTBURNIN_POS))
#define PDS_TESTADDR0                                PDS_TESTADDR0
#define PDS_TESTADDR0_POS                            (19U)
#define PDS_TESTADDR0_LEN                            (4U)
#define PDS_TESTADDR0_MSK                            (((1U << PDS_TESTADDR0_LEN) - 1) << PDS_TESTADDR0_POS)
#define PDS_TESTADDR0_UMSK                           (~(((1U << PDS_TESTADDR0_LEN) - 1) << PDS_TESTADDR0_POS))
#define PDS_SUSPENDM0                                PDS_SUSPENDM0
#define PDS_SUSPENDM0_POS                            (23U)
#define PDS_SUSPENDM0_LEN                            (1U)
#define PDS_SUSPENDM0_MSK                            (((1U << PDS_SUSPENDM0_LEN) - 1) << PDS_SUSPENDM0_POS)
#define PDS_SUSPENDM0_UMSK                           (~(((1U << PDS_SUSPENDM0_LEN) - 1) << PDS_SUSPENDM0_POS))
#define PDS_SQRXTUNE0                                PDS_SQRXTUNE0
#define PDS_SQRXTUNE0_POS                            (24U)
#define PDS_SQRXTUNE0_LEN                            (3U)
#define PDS_SQRXTUNE0_MSK                            (((1U << PDS_SQRXTUNE0_LEN) - 1) << PDS_SQRXTUNE0_POS)
#define PDS_SQRXTUNE0_UMSK                           (~(((1U << PDS_SQRXTUNE0_LEN) - 1) << PDS_SQRXTUNE0_POS))
#define PDS_SLEEPM0                                  PDS_SLEEPM0
#define PDS_SLEEPM0_POS                              (27U)
#define PDS_SLEEPM0_LEN                              (1U)
#define PDS_SLEEPM0_MSK                              (((1U << PDS_SLEEPM0_LEN) - 1) << PDS_SLEEPM0_POS)
#define PDS_SLEEPM0_UMSK                             (~(((1U << PDS_SLEEPM0_LEN) - 1) << PDS_SLEEPM0_POS))
#define PDS_SIDDQ                                    PDS_SIDDQ
#define PDS_SIDDQ_POS                                (28U)
#define PDS_SIDDQ_LEN                                (1U)
#define PDS_SIDDQ_MSK                                (((1U << PDS_SIDDQ_LEN) - 1) << PDS_SIDDQ_POS)
#define PDS_SIDDQ_UMSK                               (~(((1U << PDS_SIDDQ_LEN) - 1) << PDS_SIDDQ_POS))
#define PDS_RESREQIN                                 PDS_RESREQIN
#define PDS_RESREQIN_POS                             (29U)
#define PDS_RESREQIN_LEN                             (1U)
#define PDS_RESREQIN_MSK                             (((1U << PDS_RESREQIN_LEN) - 1) << PDS_RESREQIN_POS)
#define PDS_RESREQIN_UMSK                            (~(((1U << PDS_RESREQIN_LEN) - 1) << PDS_RESREQIN_POS))
#define PDS_REFCLKSEL                                PDS_REFCLKSEL
#define PDS_REFCLKSEL_POS                            (30U)
#define PDS_REFCLKSEL_LEN                            (2U)
#define PDS_REFCLKSEL_MSK                            (((1U << PDS_REFCLKSEL_LEN) - 1) << PDS_REFCLKSEL_POS)
#define PDS_REFCLKSEL_UMSK                           (~(((1U << PDS_REFCLKSEL_LEN) - 1) << PDS_REFCLKSEL_POS))

/* 0x50C :  */
#define PDS_U2PHY_2_OFFSET                           (0x50C)
#define PDS_IDPULLUP0_USE_REG                        PDS_IDPULLUP0_USE_REG
#define PDS_IDPULLUP0_USE_REG_POS                    (2U)
#define PDS_IDPULLUP0_USE_REG_LEN                    (1U)
#define PDS_IDPULLUP0_USE_REG_MSK                    (((1U << PDS_IDPULLUP0_USE_REG_LEN) - 1) << PDS_IDPULLUP0_USE_REG_POS)
#define PDS_IDPULLUP0_USE_REG_UMSK                   (~(((1U << PDS_IDPULLUP0_USE_REG_LEN) - 1) << PDS_IDPULLUP0_USE_REG_POS))
#define PDS_USB20_RREF_HIZ                           PDS_USB20_RREF_HIZ
#define PDS_USB20_RREF_HIZ_POS                       (3U)
#define PDS_USB20_RREF_HIZ_LEN                       (1U)
#define PDS_USB20_RREF_HIZ_MSK                       (((1U << PDS_USB20_RREF_HIZ_LEN) - 1) << PDS_USB20_RREF_HIZ_POS)
#define PDS_USB20_RREF_HIZ_UMSK                      (~(((1U << PDS_USB20_RREF_HIZ_LEN) - 1) << PDS_USB20_RREF_HIZ_POS))
#define PDS_USB20_RREF_CAL_VOUT_EN                   PDS_USB20_RREF_CAL_VOUT_EN
#define PDS_USB20_RREF_CAL_VOUT_EN_POS               (4U)
#define PDS_USB20_RREF_CAL_VOUT_EN_LEN               (1U)
#define PDS_USB20_RREF_CAL_VOUT_EN_MSK               (((1U << PDS_USB20_RREF_CAL_VOUT_EN_LEN) - 1) << PDS_USB20_RREF_CAL_VOUT_EN_POS)
#define PDS_USB20_RREF_CAL_VOUT_EN_UMSK              (~(((1U << PDS_USB20_RREF_CAL_VOUT_EN_LEN) - 1) << PDS_USB20_RREF_CAL_VOUT_EN_POS))
#define PDS_USB20_RREF_CAL_IIN_EN                    PDS_USB20_RREF_CAL_IIN_EN
#define PDS_USB20_RREF_CAL_IIN_EN_POS                (5U)
#define PDS_USB20_RREF_CAL_IIN_EN_LEN                (1U)
#define PDS_USB20_RREF_CAL_IIN_EN_MSK                (((1U << PDS_USB20_RREF_CAL_IIN_EN_LEN) - 1) << PDS_USB20_RREF_CAL_IIN_EN_POS)
#define PDS_USB20_RREF_CAL_IIN_EN_UMSK               (~(((1U << PDS_USB20_RREF_CAL_IIN_EN_LEN) - 1) << PDS_USB20_RREF_CAL_IIN_EN_POS))
#define PDS_USB20_RCAL_CODE                          PDS_USB20_RCAL_CODE
#define PDS_USB20_RCAL_CODE_POS                      (6U)
#define PDS_USB20_RCAL_CODE_LEN                      (5U)
#define PDS_USB20_RCAL_CODE_MSK                      (((1U << PDS_USB20_RCAL_CODE_LEN) - 1) << PDS_USB20_RCAL_CODE_POS)
#define PDS_USB20_RCAL_CODE_UMSK                     (~(((1U << PDS_USB20_RCAL_CODE_LEN) - 1) << PDS_USB20_RCAL_CODE_POS))
#define PDS_USB20_RCAL_COARSE                        PDS_USB20_RCAL_COARSE
#define PDS_USB20_RCAL_COARSE_POS                    (11U)
#define PDS_USB20_RCAL_COARSE_LEN                    (2U)
#define PDS_USB20_RCAL_COARSE_MSK                    (((1U << PDS_USB20_RCAL_COARSE_LEN) - 1) << PDS_USB20_RCAL_COARSE_POS)
#define PDS_USB20_RCAL_COARSE_UMSK                   (~(((1U << PDS_USB20_RCAL_COARSE_LEN) - 1) << PDS_USB20_RCAL_COARSE_POS))
#define PDS_VBUSROUDET                               PDS_VBUSROUDET
#define PDS_VBUSROUDET_POS                           (13U)
#define PDS_VBUSROUDET_LEN                           (1U)
#define PDS_VBUSROUDET_MSK                           (((1U << PDS_VBUSROUDET_LEN) - 1) << PDS_VBUSROUDET_POS)
#define PDS_VBUSROUDET_UMSK                          (~(((1U << PDS_VBUSROUDET_LEN) - 1) << PDS_VBUSROUDET_POS))
#define PDS_VDATSRCENB0                              PDS_VDATSRCENB0
#define PDS_VDATSRCENB0_POS                          (14U)
#define PDS_VDATSRCENB0_LEN                          (1U)
#define PDS_VDATSRCENB0_MSK                          (((1U << PDS_VDATSRCENB0_LEN) - 1) << PDS_VDATSRCENB0_POS)
#define PDS_VDATSRCENB0_UMSK                         (~(((1U << PDS_VDATSRCENB0_LEN) - 1) << PDS_VDATSRCENB0_POS))
#define PDS_VDATREFTUNE0                             PDS_VDATREFTUNE0
#define PDS_VDATREFTUNE0_POS                         (15U)
#define PDS_VDATREFTUNE0_LEN                         (2U)
#define PDS_VDATREFTUNE0_MSK                         (((1U << PDS_VDATREFTUNE0_LEN) - 1) << PDS_VDATREFTUNE0_POS)
#define PDS_VDATREFTUNE0_UMSK                        (~(((1U << PDS_VDATREFTUNE0_LEN) - 1) << PDS_VDATREFTUNE0_POS))
#define PDS_VDATDETENB0                              PDS_VDATDETENB0
#define PDS_VDATDETENB0_POS                          (17U)
#define PDS_VDATDETENB0_LEN                          (1U)
#define PDS_VDATDETENB0_MSK                          (((1U << PDS_VDATDETENB0_LEN) - 1) << PDS_VDATDETENB0_POS)
#define PDS_VDATDETENB0_UMSK                         (~(((1U << PDS_VDATDETENB0_LEN) - 1) << PDS_VDATDETENB0_POS))
#define PDS_VBUSVLDEXTSEL0                           PDS_VBUSVLDEXTSEL0
#define PDS_VBUSVLDEXTSEL0_POS                       (18U)
#define PDS_VBUSVLDEXTSEL0_LEN                       (1U)
#define PDS_VBUSVLDEXTSEL0_MSK                       (((1U << PDS_VBUSVLDEXTSEL0_LEN) - 1) << PDS_VBUSVLDEXTSEL0_POS)
#define PDS_VBUSVLDEXTSEL0_UMSK                      (~(((1U << PDS_VBUSVLDEXTSEL0_LEN) - 1) << PDS_VBUSVLDEXTSEL0_POS))
#define PDS_VATESTENB                                PDS_VATESTENB
#define PDS_VATESTENB_POS                            (19U)
#define PDS_VATESTENB_LEN                            (2U)
#define PDS_VATESTENB_MSK                            (((1U << PDS_VATESTENB_LEN) - 1) << PDS_VATESTENB_POS)
#define PDS_VATESTENB_UMSK                           (~(((1U << PDS_VATESTENB_LEN) - 1) << PDS_VATESTENB_POS))
#define PDS_TXVREFTUNE0                              PDS_TXVREFTUNE0
#define PDS_TXVREFTUNE0_POS                          (21U)
#define PDS_TXVREFTUNE0_LEN                          (4U)
#define PDS_TXVREFTUNE0_MSK                          (((1U << PDS_TXVREFTUNE0_LEN) - 1) << PDS_TXVREFTUNE0_POS)
#define PDS_TXVREFTUNE0_UMSK                         (~(((1U << PDS_TXVREFTUNE0_LEN) - 1) << PDS_TXVREFTUNE0_POS))
#define PDS_TXRISETUNE0                              PDS_TXRISETUNE0
#define PDS_TXRISETUNE0_POS                          (25U)
#define PDS_TXRISETUNE0_LEN                          (2U)
#define PDS_TXRISETUNE0_MSK                          (((1U << PDS_TXRISETUNE0_LEN) - 1) << PDS_TXRISETUNE0_POS)
#define PDS_TXRISETUNE0_UMSK                         (~(((1U << PDS_TXRISETUNE0_LEN) - 1) << PDS_TXRISETUNE0_POS))
#define PDS_TXRESTUNE0                               PDS_TXRESTUNE0
#define PDS_TXRESTUNE0_POS                           (27U)
#define PDS_TXRESTUNE0_LEN                           (2U)
#define PDS_TXRESTUNE0_MSK                           (((1U << PDS_TXRESTUNE0_LEN) - 1) << PDS_TXRESTUNE0_POS)
#define PDS_TXRESTUNE0_UMSK                          (~(((1U << PDS_TXRESTUNE0_LEN) - 1) << PDS_TXRESTUNE0_POS))
#define PDS_TXPREEMPPULSETUNE0                       PDS_TXPREEMPPULSETUNE0
#define PDS_TXPREEMPPULSETUNE0_POS                   (29U)
#define PDS_TXPREEMPPULSETUNE0_LEN                   (1U)
#define PDS_TXPREEMPPULSETUNE0_MSK                   (((1U << PDS_TXPREEMPPULSETUNE0_LEN) - 1) << PDS_TXPREEMPPULSETUNE0_POS)
#define PDS_TXPREEMPPULSETUNE0_UMSK                  (~(((1U << PDS_TXPREEMPPULSETUNE0_LEN) - 1) << PDS_TXPREEMPPULSETUNE0_POS))
#define PDS_TXPREEMPAMPTUNE0                         PDS_TXPREEMPAMPTUNE0
#define PDS_TXPREEMPAMPTUNE0_POS                     (30U)
#define PDS_TXPREEMPAMPTUNE0_LEN                     (2U)
#define PDS_TXPREEMPAMPTUNE0_MSK                     (((1U << PDS_TXPREEMPAMPTUNE0_LEN) - 1) << PDS_TXPREEMPAMPTUNE0_POS)
#define PDS_TXPREEMPAMPTUNE0_UMSK                    (~(((1U << PDS_TXPREEMPAMPTUNE0_LEN) - 1) << PDS_TXPREEMPAMPTUNE0_POS))

/* 0x510 :  */
#define PDS_U2PHY_3_OFFSET                           (0x510)
#define PDS_TESTDATAOUT0                             PDS_TESTDATAOUT0
#define PDS_TESTDATAOUT0_POS                         (0U)
#define PDS_TESTDATAOUT0_LEN                         (4U)
#define PDS_TESTDATAOUT0_MSK                         (((1U << PDS_TESTDATAOUT0_LEN) - 1) << PDS_TESTDATAOUT0_POS)
#define PDS_TESTDATAOUT0_UMSK                        (~(((1U << PDS_TESTDATAOUT0_LEN) - 1) << PDS_TESTDATAOUT0_POS))
#define PDS_HSSQUELCH0                               PDS_HSSQUELCH0
#define PDS_HSSQUELCH0_POS                           (4U)
#define PDS_HSSQUELCH0_LEN                           (1U)
#define PDS_HSSQUELCH0_MSK                           (((1U << PDS_HSSQUELCH0_LEN) - 1) << PDS_HSSQUELCH0_POS)
#define PDS_HSSQUELCH0_UMSK                          (~(((1U << PDS_HSSQUELCH0_LEN) - 1) << PDS_HSSQUELCH0_POS))
#define PDS_HSRXDAT0                                 PDS_HSRXDAT0
#define PDS_HSRXDAT0_POS                             (5U)
#define PDS_HSRXDAT0_LEN                             (1U)
#define PDS_HSRXDAT0_MSK                             (((1U << PDS_HSRXDAT0_LEN) - 1) << PDS_HSRXDAT0_POS)
#define PDS_HSRXDAT0_UMSK                            (~(((1U << PDS_HSRXDAT0_LEN) - 1) << PDS_HSRXDAT0_POS))
#define PDS_VBUSVLDEXT0                              PDS_VBUSVLDEXT0
#define PDS_VBUSVLDEXT0_POS                          (28U)
#define PDS_VBUSVLDEXT0_LEN                          (1U)
#define PDS_VBUSVLDEXT0_MSK                          (((1U << PDS_VBUSVLDEXT0_LEN) - 1) << PDS_VBUSVLDEXT0_POS)
#define PDS_VBUSVLDEXT0_UMSK                         (~(((1U << PDS_VBUSVLDEXT0_LEN) - 1) << PDS_VBUSVLDEXT0_POS))
#define PDS_FSDATAEXT0                               PDS_FSDATAEXT0
#define PDS_FSDATAEXT0_POS                           (29U)
#define PDS_FSDATAEXT0_LEN                           (1U)
#define PDS_FSDATAEXT0_MSK                           (((1U << PDS_FSDATAEXT0_LEN) - 1) << PDS_FSDATAEXT0_POS)
#define PDS_FSDATAEXT0_UMSK                          (~(((1U << PDS_FSDATAEXT0_LEN) - 1) << PDS_FSDATAEXT0_POS))
#define PDS_PORTRESET0                               PDS_PORTRESET0
#define PDS_PORTRESET0_POS                           (30U)
#define PDS_PORTRESET0_LEN                           (1U)
#define PDS_PORTRESET0_MSK                           (((1U << PDS_PORTRESET0_LEN) - 1) << PDS_PORTRESET0_POS)
#define PDS_PORTRESET0_UMSK                          (~(((1U << PDS_PORTRESET0_LEN) - 1) << PDS_PORTRESET0_POS))
#define PDS_POR                                      PDS_POR
#define PDS_POR_POS                                  (31U)
#define PDS_POR_LEN                                  (1U)
#define PDS_POR_MSK                                  (((1U << PDS_POR_LEN) - 1) << PDS_POR_POS)
#define PDS_POR_UMSK                                 (~(((1U << PDS_POR_LEN) - 1) << PDS_POR_POS))

struct pds_reg {
    /* 0x0 : PDS_CTL */
    union {
        struct {
            uint32_t pds_start_ps                 : 1; /* [    0],        w1p,        0x0 */
            uint32_t cr_sleep_forever             : 1; /* [    1],        r/w,        0x0 */
            uint32_t cr_xtal_force_off            : 1; /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_wifi_save_state       : 1; /* [    3],        r/w,        0x0 */
            uint32_t pds_fast_clk_sel             : 1; /* [    4],        r/w,        0x0 */
            uint32_t mask_pds_on_gpio_det_en      : 1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6_7                 : 2; /* [ 7: 6],       rsvd,        0x0 */
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
            uint32_t cr_pds_ldo_soc_vsel_en       : 1; /* [   18],        r/w,        0x1 */
            uint32_t reserved_19_21               : 3; /* [21:19],       rsvd,        0x0 */
            uint32_t cr_pds_ctrl_wifipll_pd       : 1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23_24               : 2; /* [24:23],       rsvd,        0x0 */
            uint32_t pds_gpio_det_clk_sel         : 2; /* [26:25],        r/w,        0x0 */
            uint32_t cr_pds_access_wram_path      : 1; /* [   27],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_rf               : 2; /* [29:28],        r/w,        0x1 */
            uint32_t cr_pds_start_use_tbtt_sleep  : 1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                  : 1; /* [   31],       rsvd,        0x0 */
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

    /* 0x8 : PDS_GPIO_LAT_EN */
    union {
        struct {
            uint32_t pds_gpio_lat_en : 31; /* [30: 0],        r/w,        0x0 */
            uint32_t reserved_31     : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_GPIO_LAT_EN;

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
            uint32_t cr_pds_wakeup_src_en             : 10; /* [19:10],        r/w,      0x3ff */
            uint32_t reserved_20                      : 1;  /* [   20],       rsvd,        0x0 */
            uint32_t ro_pds_wakeup_event              : 10; /* [30:21],          r,        0x0 */
            uint32_t reserved_31                      : 1;  /* [   31],       rsvd,        0x0 */
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
            uint32_t cr_np_wfi_mask    : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3      : 3;  /* [ 3: 1],       rsvd,        0x0 */
            uint32_t cr_pds_pad_od_en  : 4;  /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_usb33 : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31     : 23; /* [31: 9],       rsvd,        0x0 */
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

    /* 0x30 : pds_gpio_int_msk_set */
    union {
        struct {
            uint32_t cr_pds_gpio_set_int_mask : 31; /* [30: 0],        r/w, 0x7fffffff */
            uint32_t reserved_31              : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_int_msk_set;

    /* 0x34 : pds_gpio_int_clr */
    union {
        struct {
            uint32_t pds_gpio_int_clr : 31; /* [30: 0],        r/w,        0x0 */
            uint32_t reserved_31      : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_int_clr;

    /* 0x38 : pds_gpio_stat */
    union {
        struct {
            uint32_t pds_gpio_int_stat : 31; /* [30: 0],          r,        0x0 */
            uint32_t reserved_31       : 1;  /* [   31],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio_stat;

    /* 0x3C : PDS_RAM3 */
    union {
        struct {
            uint32_t cr_ocram_ret   : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_RAM3;

    /* 0x40 : PDS_RAM4 */
    union {
        struct {
            uint32_t cr_ocram_slp   : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } PDS_RAM4;

    /* 0x44  reserved */
    uint8_t RESERVED0x44[8];

    /* 0x4C : pds_gpio13_6_int_mode */
    union {
        struct {
            uint32_t cr_pds_gpio6_int_mode  : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_pds_gpio7_int_mode  : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_pds_gpio8_int_mode  : 4; /* [11: 8],        r/w,        0x0 */
            uint32_t cr_pds_gpio9_int_mode  : 4; /* [15:12],        r/w,        0x0 */
            uint32_t cr_pds_gpio10_int_mode : 4; /* [19:16],        r/w,        0x0 */
            uint32_t cr_pds_gpio11_int_mode : 4; /* [23:20],        r/w,        0x0 */
            uint32_t cr_pds_gpio12_int_mode : 4; /* [27:24],        r/w,        0x0 */
            uint32_t cr_pds_gpio13_int_mode : 4; /* [31:28],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio13_6_int_mode;

    /* 0x50 : pds_gpio21_14_int_mode */
    union {
        struct {
            uint32_t cr_pds_gpio14_int_mode : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_pds_gpio15_int_mode : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_pds_gpio16_int_mode : 4; /* [11: 8],        r/w,        0x0 */
            uint32_t cr_pds_gpio17_int_mode : 4; /* [15:12],        r/w,        0x0 */
            uint32_t cr_pds_gpio18_int_mode : 4; /* [19:16],        r/w,        0x0 */
            uint32_t cr_pds_gpio19_int_mode : 4; /* [23:20],        r/w,        0x0 */
            uint32_t cr_pds_gpio20_int_mode : 4; /* [27:24],        r/w,        0x0 */
            uint32_t cr_pds_gpio21_int_mode : 4; /* [31:28],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio21_14_int_mode;

    /* 0x54 : pds_gpio29_22_int_mode */
    union {
        struct {
            uint32_t cr_pds_gpio22_int_mode : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_pds_gpio23_int_mode : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_pds_gpio24_int_mode : 4; /* [11: 8],        r/w,        0x0 */
            uint32_t cr_pds_gpio25_int_mode : 4; /* [15:12],        r/w,        0x0 */
            uint32_t cr_pds_gpio26_int_mode : 4; /* [19:16],        r/w,        0x0 */
            uint32_t cr_pds_gpio27_int_mode : 4; /* [23:20],        r/w,        0x0 */
            uint32_t cr_pds_gpio28_int_mode : 4; /* [27:24],        r/w,        0x0 */
            uint32_t cr_pds_gpio29_int_mode : 4; /* [31:28],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio29_22_int_mode;

    /* 0x58 : pds_gpio36_30_int_mode */
    union {
        struct {
            uint32_t cr_pds_gpio30_int_mode : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_pds_gpio31_int_mode : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_pds_gpio32_int_mode : 4; /* [11: 8],        r/w,        0x0 */
            uint32_t cr_pds_gpio33_int_mode : 4; /* [15:12],        r/w,        0x0 */
            uint32_t cr_pds_gpio34_int_mode : 4; /* [19:16],        r/w,        0x0 */
            uint32_t cr_pds_gpio35_int_mode : 4; /* [23:20],        r/w,        0x0 */
            uint32_t cr_pds_gpio36_int_mode : 4; /* [27:24],        r/w,        0x0 */
            uint32_t reserved_28_31         : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_gpio36_30_int_mode;

    /* 0x5C : pds_version */
    union {
        struct {
            uint32_t pds_resv_for_software : 24; /* [23: 0],        r/w,        0x0 */
            uint32_t pds_version_id        : 8;  /* [31:24],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_version;

    /* 0x60 : pds_ldo_soc_lp_ctrl */
    union {
        struct {
            uint32_t cr_pds_ldo_soc_lp_en_cnt   : 16; /* [15: 0],        r/w,        0x1 */
            uint32_t cr_pds_ldo_soc_exit_lp_cnt : 16; /* [31:16],        r/w,        0x1 */
        } BF;
        uint32_t WORD;
    } pds_ldo_soc_lp_ctrl;

    /* 0x64 : pds_ldo13_lp_ctrl */
    union {
        struct {
            uint32_t cr_pds_ldo13_lp_en_cnt   : 16; /* [15: 0],        r/w,        0x2 */
            uint32_t cr_pds_ldo13_exit_lp_cnt : 16; /* [31:16],        r/w,        0x1 */
        } BF;
        uint32_t WORD;
    } pds_ldo13_lp_ctrl;

    /* 0x68 : pds_slp_config */
    union {
        struct {
            uint32_t cr_pds_eiso_cnt                     : 16; /* [15: 0],        r/w,        0x1 */
            uint32_t cr_pds_slp_use_fast_clk             : 1;  /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_slp_ldo_soc_lp               : 1;  /* [   17],        r/w,        0x1 */
            uint32_t cr_pds_slp_ldo13_lp                 : 1;  /* [   18],        r/w,        0x1 */
            uint32_t cr_pds_dis_pu_when_fast_clk_off_dis : 1;  /* [   19],        r/w,        0x0 */
            uint32_t cr_pds_wakeup_sw_fast_clk_first     : 1;  /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31                      : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pds_slp_config;

    /* 0x6C : cpu_core_cfg1 */
    union {
        struct {
            uint32_t reserved_0_3    : 4;  /* [ 3: 0],       rsvd,        0x0 */
            uint32_t reg_pll_sel     : 2;  /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7    : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_mcu1_clk_en : 1;  /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_31   : 23; /* [31: 9],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } cpu_core_cfg1;

    /* 0x70  reserved */
    uint8_t RESERVED0x70[24];

    /* 0x88 : cpu_core_cfg14 */
    union {
        struct {
            uint32_t e906_rst_addr : 32; /* [31: 0],        r/w, 0x90000000 */
        } BF;
        uint32_t WORD;
    } cpu_core_cfg14;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[16];

    /* 0x9C :  */
    union {
        struct {
            uint32_t rc32m_cal_done        : 1;  /* [    0],          r,        0x0 */
            uint32_t rchf_rdy              : 1;  /* [    1],          r,        0x0 */
            uint32_t rc32m_cal_in_progress : 1;  /* [    2],          r,        0x0 */
            uint32_t rc32m_cal_div         : 2;  /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32m_cal_precharge   : 1;  /* [    5],          r,        0x0 */
            uint32_t reserved_6_16         : 11; /* [16: 6],       rsvd,        0x0 */
            uint32_t rc32m_allow_cal       : 1;  /* [   17],        r/w,        0x0 */
            uint32_t rc32m_refclk_half     : 1;  /* [   18],        r/w,        0x0 */
            uint32_t reserved_19           : 1;  /* [   19],       rsvd,        0x0 */
            uint32_t rc32m_cal_en          : 1;  /* [   20],        r/w,        0x0 */
            uint32_t pu_rc32m              : 1;  /* [   21],          r,        0x0 */
            uint32_t reserved_22_31        : 10; /* [31:22],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl0;

    /* 0xA0 :  */
    union {
        struct {
            uint32_t ten_rc32m          : 1;  /* [    0],        r/w,        0x0 */
            uint32_t dten_rchf          : 1;  /* [    1],        r/w,        0x0 */
            uint32_t rc32m_soft_rst     : 1;  /* [    2],        r/w,        0x0 */
            uint32_t rc32m_clk_soft_rst : 1;  /* [    3],        r/w,        0x0 */
            uint32_t rc32m_clk_inv      : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7       : 3;  /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rc32m_clk_force_on : 1;  /* [    8],        r/w,        0x0 */
            uint32_t rchf_cal_code_down : 1;  /* [    9],          r,        0x0 */
            uint32_t rchf_cal_step_done : 1;  /* [   10],          r,        0x0 */
            uint32_t rc32m_pd           : 1;  /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_23     : 12; /* [23:12],       rsvd,        0x0 */
            uint32_t rc32m_reserved     : 8;  /* [31:24],        r/w,       0x55 */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl1;

    /* 0xa4  reserved */
    uint8_t RESERVED0xa4[4];

    /* 0xA8 : pu_rst_clkpll */
    union {
        struct {
            uint32_t reserved_0_8           : 9;  /* [ 8: 0],       rsvd,        0x0 */
            uint32_t cr_pds_pu_clkpll_sfreg : 1;  /* [    9],        r/w,        0x0 */
            uint32_t cr_pds_pu_clkpll       : 1;  /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_31         : 21; /* [31:11],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pu_rst_clkpll;

    /* 0xac  reserved */
    uint8_t RESERVED0xac[28];

    /* 0xC8 : usb_ctl */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } usb_ctl;

    /* 0xcc  reserved */
    uint8_t RESERVED0xcc[1076];

    /* 0x500 : usb_ctrl_0 */
    union {
        struct {
            uint32_t cr_usb_sw_rst_n     : 1;  /* [    0],        r/w,        0x1 */
            uint32_t reserved_1          : 1;  /* [    1],       rsvd,        0x0 */
            uint32_t cr_usb_l1_wakeup    : 1;  /* [    2],        r/w,        0x0 */
            uint32_t cr_usb_wakeup       : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_6        : 3;  /* [ 6: 4],       rsvd,        0x0 */
            uint32_t cr_usb_uclk_lpm_dis : 1;  /* [    7],        r/w,        0x0 */
            uint32_t reg_usb_ext_susp_n  : 1;  /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_15       : 7;  /* [15: 9],       rsvd,        0x0 */
            uint32_t cr_usb_iddig        : 1;  /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_31      : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } usb_ctrl_0;

    /* 0x504 :  */
    union {
        struct {
            uint32_t PLLPTUNE      : 4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t PLLITUNE      : 2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t PLLBTUNE      : 1; /* [    6],        r/w,        0x0 */
            uint32_t OTGTUNE0      : 3; /* [ 9: 7],        r/w,        0x0 */
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

    /* 0x508 :  */
    union {
        struct {
            uint32_t SUSPENDM0_USE_REG : 1; /* [    0],        r/w,        0x0 */
            uint32_t TXHSXVTUNE0       : 2; /* [ 2: 1],        r/w,        0x3 */
            uint32_t TXFSLSTUNE0       : 4; /* [ 6: 3],        r/w,        0x3 */
            uint32_t TXENABLEN0        : 1; /* [    7],        r/w,        0x1 */
            uint32_t TESTDATAOUTSEL0   : 1; /* [    8],        r/w,        0x0 */
            uint32_t TESTDATAIN0       : 8; /* [16: 9],        r/w,        0x0 */
            uint32_t TESTCLK0          : 1; /* [   17],        r/w,        0x0 */
            uint32_t TESTBURNIN        : 1; /* [   18],        r/w,        0x0 */
            uint32_t TESTADDR0         : 4; /* [22:19],        r/w,        0x0 */
            uint32_t SUSPENDM0         : 1; /* [   23],        r/w,        0x1 */
            uint32_t SQRXTUNE0         : 3; /* [26:24],        r/w,        0x3 */
            uint32_t SLEEPM0           : 1; /* [   27],        r/w,        0x1 */
            uint32_t SIDDQ             : 1; /* [   28],        r/w,        0x1 */
            uint32_t RESREQIN          : 1; /* [   29],        r/w,        0x0 */
            uint32_t REFCLKSEL         : 2; /* [31:30],        r/w,        0x2 */
        } BF;
        uint32_t WORD;
    } u2phy_1;

    /* 0x50C :  */
    union {
        struct {
            uint32_t reserved_0_1           : 2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t IDPULLUP0_USE_REG      : 1; /* [    2],        r/w,        0x0 */
            uint32_t USB20_RREF_HIZ         : 1; /* [    3],        r/w,        0x0 */
            uint32_t USB20_RREF_CAL_VOUT_EN : 1; /* [    4],        r/w,        0x0 */
            uint32_t USB20_RREF_CAL_IIN_EN  : 1; /* [    5],        r/w,        0x0 */
            uint32_t USB20_RCAL_CODE        : 5; /* [10: 6],        r/w,       0x10 */
            uint32_t USB20_RCAL_COARSE      : 2; /* [12:11],        r/w,        0x1 */
            uint32_t VBUSROUDET             : 1; /* [   13],        r/w,        0x1 */
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

    /* 0x510 :  */
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
