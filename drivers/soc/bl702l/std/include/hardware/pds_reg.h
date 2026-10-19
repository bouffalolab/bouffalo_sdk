/**
  ******************************************************************************
  * @file    pds_reg.h
  * @version V1.0
  * @date    2022-07-01
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
#ifndef  __PDS_REG_H__
#define  __PDS_REG_H__

#include "bl702l.h"

/* 0x0 : PDS_CTL */
#define PDS_CTL_OFFSET                                          (0x0)
#define PDS_START_PS                                            PDS_START_PS
#define PDS_START_PS_POS                                        (0U)
#define PDS_START_PS_LEN                                        (1U)
#define PDS_START_PS_MSK                                        (((1U<<PDS_START_PS_LEN)-1)<<PDS_START_PS_POS)
#define PDS_START_PS_UMSK                                       (~(((1U<<PDS_START_PS_LEN)-1)<<PDS_START_PS_POS))
#define PDS_CR_SLEEP_FOREVER                                    PDS_CR_SLEEP_FOREVER
#define PDS_CR_SLEEP_FOREVER_POS                                (1U)
#define PDS_CR_SLEEP_FOREVER_LEN                                (1U)
#define PDS_CR_SLEEP_FOREVER_MSK                                (((1U<<PDS_CR_SLEEP_FOREVER_LEN)-1)<<PDS_CR_SLEEP_FOREVER_POS)
#define PDS_CR_SLEEP_FOREVER_UMSK                               (~(((1U<<PDS_CR_SLEEP_FOREVER_LEN)-1)<<PDS_CR_SLEEP_FOREVER_POS))
#define PDS_CR_PDS_WAIT_RC32M_RDY                               PDS_CR_PDS_WAIT_RC32M_RDY
#define PDS_CR_PDS_WAIT_RC32M_RDY_POS                           (3U)
#define PDS_CR_PDS_WAIT_RC32M_RDY_LEN                           (1U)
#define PDS_CR_PDS_WAIT_RC32M_RDY_MSK                           (((1U<<PDS_CR_PDS_WAIT_RC32M_RDY_LEN)-1)<<PDS_CR_PDS_WAIT_RC32M_RDY_POS)
#define PDS_CR_PDS_WAIT_RC32M_RDY_UMSK                          (~(((1U<<PDS_CR_PDS_WAIT_RC32M_RDY_LEN)-1)<<PDS_CR_PDS_WAIT_RC32M_RDY_POS))
#define PDS_CR_PDS_PD_AVDD14                                    PDS_CR_PDS_PD_AVDD14
#define PDS_CR_PDS_PD_AVDD14_POS                                (4U)
#define PDS_CR_PDS_PD_AVDD14_LEN                                (1U)
#define PDS_CR_PDS_PD_AVDD14_MSK                                (((1U<<PDS_CR_PDS_PD_AVDD14_LEN)-1)<<PDS_CR_PDS_PD_AVDD14_POS)
#define PDS_CR_PDS_PD_AVDD14_UMSK                               (~(((1U<<PDS_CR_PDS_PD_AVDD14_LEN)-1)<<PDS_CR_PDS_PD_AVDD14_POS))
#define PDS_CR_PDS_PD_BG_SYS                                    PDS_CR_PDS_PD_BG_SYS
#define PDS_CR_PDS_PD_BG_SYS_POS                                (5U)
#define PDS_CR_PDS_PD_BG_SYS_LEN                                (1U)
#define PDS_CR_PDS_PD_BG_SYS_MSK                                (((1U<<PDS_CR_PDS_PD_BG_SYS_LEN)-1)<<PDS_CR_PDS_PD_BG_SYS_POS)
#define PDS_CR_PDS_PD_BG_SYS_UMSK                               (~(((1U<<PDS_CR_PDS_PD_BG_SYS_LEN)-1)<<PDS_CR_PDS_PD_BG_SYS_POS))
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT                        PDS_CR_PDS_GLB_REG_RESET_PROTECT
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS                    (6U)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN                    (1U)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_MSK                    (((1U<<PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN)-1)<<PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS)
#define PDS_CR_PDS_GLB_REG_RESET_PROTECT_UMSK                   (~(((1U<<PDS_CR_PDS_GLB_REG_RESET_PROTECT_LEN)-1)<<PDS_CR_PDS_GLB_REG_RESET_PROTECT_POS))
#define PDS_CR_PDS_CTRL_PU_FLASH                                PDS_CR_PDS_CTRL_PU_FLASH
#define PDS_CR_PDS_CTRL_PU_FLASH_POS                            (7U)
#define PDS_CR_PDS_CTRL_PU_FLASH_LEN                            (1U)
#define PDS_CR_PDS_CTRL_PU_FLASH_MSK                            (((1U<<PDS_CR_PDS_CTRL_PU_FLASH_LEN)-1)<<PDS_CR_PDS_CTRL_PU_FLASH_POS)
#define PDS_CR_PDS_CTRL_PU_FLASH_UMSK                           (~(((1U<<PDS_CR_PDS_CTRL_PU_FLASH_LEN)-1)<<PDS_CR_PDS_CTRL_PU_FLASH_POS))
#define PDS_CR_PDS_GATE_CLK                                     PDS_CR_PDS_GATE_CLK
#define PDS_CR_PDS_GATE_CLK_POS                                 (8U)
#define PDS_CR_PDS_GATE_CLK_LEN                                 (1U)
#define PDS_CR_PDS_GATE_CLK_MSK                                 (((1U<<PDS_CR_PDS_GATE_CLK_LEN)-1)<<PDS_CR_PDS_GATE_CLK_POS)
#define PDS_CR_PDS_GATE_CLK_UMSK                                (~(((1U<<PDS_CR_PDS_GATE_CLK_LEN)-1)<<PDS_CR_PDS_GATE_CLK_POS))
#define PDS_CR_PDS_MEM_STBY                                     PDS_CR_PDS_MEM_STBY
#define PDS_CR_PDS_MEM_STBY_POS                                 (9U)
#define PDS_CR_PDS_MEM_STBY_LEN                                 (1U)
#define PDS_CR_PDS_MEM_STBY_MSK                                 (((1U<<PDS_CR_PDS_MEM_STBY_LEN)-1)<<PDS_CR_PDS_MEM_STBY_POS)
#define PDS_CR_PDS_MEM_STBY_UMSK                                (~(((1U<<PDS_CR_PDS_MEM_STBY_LEN)-1)<<PDS_CR_PDS_MEM_STBY_POS))
#define PDS_CR_SW_PU_FLASH                                      PDS_CR_SW_PU_FLASH
#define PDS_CR_SW_PU_FLASH_POS                                  (10U)
#define PDS_CR_SW_PU_FLASH_LEN                                  (1U)
#define PDS_CR_SW_PU_FLASH_MSK                                  (((1U<<PDS_CR_SW_PU_FLASH_LEN)-1)<<PDS_CR_SW_PU_FLASH_POS)
#define PDS_CR_SW_PU_FLASH_UMSK                                 (~(((1U<<PDS_CR_SW_PU_FLASH_LEN)-1)<<PDS_CR_SW_PU_FLASH_POS))
#define PDS_CR_PDS_ISO_EN                                       PDS_CR_PDS_ISO_EN
#define PDS_CR_PDS_ISO_EN_POS                                   (11U)
#define PDS_CR_PDS_ISO_EN_LEN                                   (1U)
#define PDS_CR_PDS_ISO_EN_MSK                                   (((1U<<PDS_CR_PDS_ISO_EN_LEN)-1)<<PDS_CR_PDS_ISO_EN_POS)
#define PDS_CR_PDS_ISO_EN_UMSK                                  (~(((1U<<PDS_CR_PDS_ISO_EN_LEN)-1)<<PDS_CR_PDS_ISO_EN_POS))
#define PDS_CR_PDS_WAIT_XTAL_RDY                                PDS_CR_PDS_WAIT_XTAL_RDY
#define PDS_CR_PDS_WAIT_XTAL_RDY_POS                            (12U)
#define PDS_CR_PDS_WAIT_XTAL_RDY_LEN                            (1U)
#define PDS_CR_PDS_WAIT_XTAL_RDY_MSK                            (((1U<<PDS_CR_PDS_WAIT_XTAL_RDY_LEN)-1)<<PDS_CR_PDS_WAIT_XTAL_RDY_POS)
#define PDS_CR_PDS_WAIT_XTAL_RDY_UMSK                           (~(((1U<<PDS_CR_PDS_WAIT_XTAL_RDY_LEN)-1)<<PDS_CR_PDS_WAIT_XTAL_RDY_POS))
#define PDS_CR_PDS_PWR_OFF                                      PDS_CR_PDS_PWR_OFF
#define PDS_CR_PDS_PWR_OFF_POS                                  (13U)
#define PDS_CR_PDS_PWR_OFF_LEN                                  (1U)
#define PDS_CR_PDS_PWR_OFF_MSK                                  (((1U<<PDS_CR_PDS_PWR_OFF_LEN)-1)<<PDS_CR_PDS_PWR_OFF_POS)
#define PDS_CR_PDS_PWR_OFF_UMSK                                 (~(((1U<<PDS_CR_PDS_PWR_OFF_LEN)-1)<<PDS_CR_PDS_PWR_OFF_POS))
#define PDS_CR_PDS_PD_XTAL                                      PDS_CR_PDS_PD_XTAL
#define PDS_CR_PDS_PD_XTAL_POS                                  (14U)
#define PDS_CR_PDS_PD_XTAL_LEN                                  (1U)
#define PDS_CR_PDS_PD_XTAL_MSK                                  (((1U<<PDS_CR_PDS_PD_XTAL_LEN)-1)<<PDS_CR_PDS_PD_XTAL_POS)
#define PDS_CR_PDS_PD_XTAL_UMSK                                 (~(((1U<<PDS_CR_PDS_PD_XTAL_LEN)-1)<<PDS_CR_PDS_PD_XTAL_POS))
#define PDS_CR_PDS_CTRL_SOC_ENB                                 PDS_CR_PDS_CTRL_SOC_ENB
#define PDS_CR_PDS_CTRL_SOC_ENB_POS                             (15U)
#define PDS_CR_PDS_CTRL_SOC_ENB_LEN                             (1U)
#define PDS_CR_PDS_CTRL_SOC_ENB_MSK                             (((1U<<PDS_CR_PDS_CTRL_SOC_ENB_LEN)-1)<<PDS_CR_PDS_CTRL_SOC_ENB_POS)
#define PDS_CR_PDS_CTRL_SOC_ENB_UMSK                            (~(((1U<<PDS_CR_PDS_CTRL_SOC_ENB_LEN)-1)<<PDS_CR_PDS_CTRL_SOC_ENB_POS))
#define PDS_CR_PDS_RST_SOC_EN                                   PDS_CR_PDS_RST_SOC_EN
#define PDS_CR_PDS_RST_SOC_EN_POS                               (16U)
#define PDS_CR_PDS_RST_SOC_EN_LEN                               (1U)
#define PDS_CR_PDS_RST_SOC_EN_MSK                               (((1U<<PDS_CR_PDS_RST_SOC_EN_LEN)-1)<<PDS_CR_PDS_RST_SOC_EN_POS)
#define PDS_CR_PDS_RST_SOC_EN_UMSK                              (~(((1U<<PDS_CR_PDS_RST_SOC_EN_LEN)-1)<<PDS_CR_PDS_RST_SOC_EN_POS))
#define PDS_CR_PDS_RC32M_OFF_DIS                                PDS_CR_PDS_RC32M_OFF_DIS
#define PDS_CR_PDS_RC32M_OFF_DIS_POS                            (17U)
#define PDS_CR_PDS_RC32M_OFF_DIS_LEN                            (1U)
#define PDS_CR_PDS_RC32M_OFF_DIS_MSK                            (((1U<<PDS_CR_PDS_RC32M_OFF_DIS_LEN)-1)<<PDS_CR_PDS_RC32M_OFF_DIS_POS)
#define PDS_CR_PDS_RC32M_OFF_DIS_UMSK                           (~(((1U<<PDS_CR_PDS_RC32M_OFF_DIS_LEN)-1)<<PDS_CR_PDS_RC32M_OFF_DIS_POS))
#define PDS_CR_PDS_LDO_VSEL_EN                                  PDS_CR_PDS_LDO_VSEL_EN
#define PDS_CR_PDS_LDO_VSEL_EN_POS                              (18U)
#define PDS_CR_PDS_LDO_VSEL_EN_LEN                              (1U)
#define PDS_CR_PDS_LDO_VSEL_EN_MSK                              (((1U<<PDS_CR_PDS_LDO_VSEL_EN_LEN)-1)<<PDS_CR_PDS_LDO_VSEL_EN_POS)
#define PDS_CR_PDS_LDO_VSEL_EN_UMSK                             (~(((1U<<PDS_CR_PDS_LDO_VSEL_EN_LEN)-1)<<PDS_CR_PDS_LDO_VSEL_EN_POS))
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN                            PDS_CR_PDS_XTAL_CNT_RC32K_EN
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_POS                        (20U)
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_LEN                        (1U)
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_MSK                        (((1U<<PDS_CR_PDS_XTAL_CNT_RC32K_EN_LEN)-1)<<PDS_CR_PDS_XTAL_CNT_RC32K_EN_POS)
#define PDS_CR_PDS_XTAL_CNT_RC32K_EN_UMSK                       (~(((1U<<PDS_CR_PDS_XTAL_CNT_RC32K_EN_LEN)-1)<<PDS_CR_PDS_XTAL_CNT_RC32K_EN_POS))
#define PDS_CR_NP_WFI_MASK                                      PDS_CR_NP_WFI_MASK
#define PDS_CR_NP_WFI_MASK_POS                                  (21U)
#define PDS_CR_NP_WFI_MASK_LEN                                  (1U)
#define PDS_CR_NP_WFI_MASK_MSK                                  (((1U<<PDS_CR_NP_WFI_MASK_LEN)-1)<<PDS_CR_NP_WFI_MASK_POS)
#define PDS_CR_NP_WFI_MASK_UMSK                                 (~(((1U<<PDS_CR_NP_WFI_MASK_LEN)-1)<<PDS_CR_NP_WFI_MASK_POS))
#define PDS_CR_PDS_PD_LDO11                                     PDS_CR_PDS_PD_LDO11
#define PDS_CR_PDS_PD_LDO11_POS                                 (22U)
#define PDS_CR_PDS_PD_LDO11_LEN                                 (1U)
#define PDS_CR_PDS_PD_LDO11_MSK                                 (((1U<<PDS_CR_PDS_PD_LDO11_LEN)-1)<<PDS_CR_PDS_PD_LDO11_POS)
#define PDS_CR_PDS_PD_LDO11_UMSK                                (~(((1U<<PDS_CR_PDS_PD_LDO11_LEN)-1)<<PDS_CR_PDS_PD_LDO11_POS))
#define PDS_CR_PDS_CTRL_RF                                      PDS_CR_PDS_CTRL_RF
#define PDS_CR_PDS_CTRL_RF_POS                                  (23U)
#define PDS_CR_PDS_CTRL_RF_LEN                                  (1U)
#define PDS_CR_PDS_CTRL_RF_MSK                                  (((1U<<PDS_CR_PDS_CTRL_RF_LEN)-1)<<PDS_CR_PDS_CTRL_RF_POS)
#define PDS_CR_PDS_CTRL_RF_UMSK                                 (~(((1U<<PDS_CR_PDS_CTRL_RF_LEN)-1)<<PDS_CR_PDS_CTRL_RF_POS))
#define PDS_CFG_XTAL_DEG_EN                                     PDS_CFG_XTAL_DEG_EN
#define PDS_CFG_XTAL_DEG_EN_POS                                 (24U)
#define PDS_CFG_XTAL_DEG_EN_LEN                                 (1U)
#define PDS_CFG_XTAL_DEG_EN_MSK                                 (((1U<<PDS_CFG_XTAL_DEG_EN_LEN)-1)<<PDS_CFG_XTAL_DEG_EN_POS)
#define PDS_CFG_XTAL_DEG_EN_UMSK                                (~(((1U<<PDS_CFG_XTAL_DEG_EN_LEN)-1)<<PDS_CFG_XTAL_DEG_EN_POS))
#define PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS                        PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS
#define PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_POS                    (25U)
#define PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_LEN                    (1U)
#define PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_MSK                    (((1U<<PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_LEN)-1)<<PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_POS)
#define PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_UMSK                   (~(((1U<<PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_LEN)-1)<<PDS_CFG_BLE_WAKEUP_REQ_AFTER_PDS_POS))
#define PDS_CR_SW_GPIO_ISO_MODE                                 PDS_CR_SW_GPIO_ISO_MODE
#define PDS_CR_SW_GPIO_ISO_MODE_POS                             (26U)
#define PDS_CR_SW_GPIO_ISO_MODE_LEN                             (1U)
#define PDS_CR_SW_GPIO_ISO_MODE_MSK                             (((1U<<PDS_CR_SW_GPIO_ISO_MODE_LEN)-1)<<PDS_CR_SW_GPIO_ISO_MODE_POS)
#define PDS_CR_SW_GPIO_ISO_MODE_UMSK                            (~(((1U<<PDS_CR_SW_GPIO_ISO_MODE_LEN)-1)<<PDS_CR_SW_GPIO_ISO_MODE_POS))
#define PDS_CR_TURN_OFF_PDS_CLK                                 PDS_CR_TURN_OFF_PDS_CLK
#define PDS_CR_TURN_OFF_PDS_CLK_POS                             (27U)
#define PDS_CR_TURN_OFF_PDS_CLK_LEN                             (1U)
#define PDS_CR_TURN_OFF_PDS_CLK_MSK                             (((1U<<PDS_CR_TURN_OFF_PDS_CLK_LEN)-1)<<PDS_CR_TURN_OFF_PDS_CLK_POS)
#define PDS_CR_TURN_OFF_PDS_CLK_UMSK                            (~(((1U<<PDS_CR_TURN_OFF_PDS_CLK_LEN)-1)<<PDS_CR_TURN_OFF_PDS_CLK_POS))
#define PDS_CR_PDS_GPIO_KEEP_EN                                 PDS_CR_PDS_GPIO_KEEP_EN
#define PDS_CR_PDS_GPIO_KEEP_EN_POS                             (28U)
#define PDS_CR_PDS_GPIO_KEEP_EN_LEN                             (3U)
#define PDS_CR_PDS_GPIO_KEEP_EN_MSK                             (((1U<<PDS_CR_PDS_GPIO_KEEP_EN_LEN)-1)<<PDS_CR_PDS_GPIO_KEEP_EN_POS)
#define PDS_CR_PDS_GPIO_KEEP_EN_UMSK                            (~(((1U<<PDS_CR_PDS_GPIO_KEEP_EN_LEN)-1)<<PDS_CR_PDS_GPIO_KEEP_EN_POS))
#define PDS_CR_PDS_GPIO_ISO_MODE                                PDS_CR_PDS_GPIO_ISO_MODE
#define PDS_CR_PDS_GPIO_ISO_MODE_POS                            (31U)
#define PDS_CR_PDS_GPIO_ISO_MODE_LEN                            (1U)
#define PDS_CR_PDS_GPIO_ISO_MODE_MSK                            (((1U<<PDS_CR_PDS_GPIO_ISO_MODE_LEN)-1)<<PDS_CR_PDS_GPIO_ISO_MODE_POS)
#define PDS_CR_PDS_GPIO_ISO_MODE_UMSK                           (~(((1U<<PDS_CR_PDS_GPIO_ISO_MODE_LEN)-1)<<PDS_CR_PDS_GPIO_ISO_MODE_POS))

/* 0xC : PDS_INT */
#define PDS_INT_OFFSET                                          (0xC)
#define PDS_RO_PDS_WAKE_INT                                     PDS_RO_PDS_WAKE_INT
#define PDS_RO_PDS_WAKE_INT_POS                                 (0U)
#define PDS_RO_PDS_WAKE_INT_LEN                                 (1U)
#define PDS_RO_PDS_WAKE_INT_MSK                                 (((1U<<PDS_RO_PDS_WAKE_INT_LEN)-1)<<PDS_RO_PDS_WAKE_INT_POS)
#define PDS_RO_PDS_WAKE_INT_UMSK                                (~(((1U<<PDS_RO_PDS_WAKE_INT_LEN)-1)<<PDS_RO_PDS_WAKE_INT_POS))
#define PDS_RO_PDS_RF_DONE_INT                                  PDS_RO_PDS_RF_DONE_INT
#define PDS_RO_PDS_RF_DONE_INT_POS                              (2U)
#define PDS_RO_PDS_RF_DONE_INT_LEN                              (1U)
#define PDS_RO_PDS_RF_DONE_INT_MSK                              (((1U<<PDS_RO_PDS_RF_DONE_INT_LEN)-1)<<PDS_RO_PDS_RF_DONE_INT_POS)
#define PDS_RO_PDS_RF_DONE_INT_UMSK                             (~(((1U<<PDS_RO_PDS_RF_DONE_INT_LEN)-1)<<PDS_RO_PDS_RF_DONE_INT_POS))
#define PDS_RESET_EVENT                                         PDS_RESET_EVENT
#define PDS_RESET_EVENT_POS                                     (4U)
#define PDS_RESET_EVENT_LEN                                     (3U)
#define PDS_RESET_EVENT_MSK                                     (((1U<<PDS_RESET_EVENT_LEN)-1)<<PDS_RESET_EVENT_POS)
#define PDS_RESET_EVENT_UMSK                                    (~(((1U<<PDS_RESET_EVENT_LEN)-1)<<PDS_RESET_EVENT_POS))
#define PDS_CLR_RESET_EVENT                                     PDS_CLR_RESET_EVENT
#define PDS_CLR_RESET_EVENT_POS                                 (7U)
#define PDS_CLR_RESET_EVENT_LEN                                 (1U)
#define PDS_CLR_RESET_EVENT_MSK                                 (((1U<<PDS_CLR_RESET_EVENT_LEN)-1)<<PDS_CLR_RESET_EVENT_POS)
#define PDS_CLR_RESET_EVENT_UMSK                                (~(((1U<<PDS_CLR_RESET_EVENT_LEN)-1)<<PDS_CLR_RESET_EVENT_POS))
#define PDS_CR_PDS_WAKE_INT_MASK                                PDS_CR_PDS_WAKE_INT_MASK
#define PDS_CR_PDS_WAKE_INT_MASK_POS                            (8U)
#define PDS_CR_PDS_WAKE_INT_MASK_LEN                            (1U)
#define PDS_CR_PDS_WAKE_INT_MASK_MSK                            (((1U<<PDS_CR_PDS_WAKE_INT_MASK_LEN)-1)<<PDS_CR_PDS_WAKE_INT_MASK_POS)
#define PDS_CR_PDS_WAKE_INT_MASK_UMSK                           (~(((1U<<PDS_CR_PDS_WAKE_INT_MASK_LEN)-1)<<PDS_CR_PDS_WAKE_INT_MASK_POS))
#define PDS_PIN_WAKEUP_OPTION                                   PDS_PIN_WAKEUP_OPTION
#define PDS_PIN_WAKEUP_OPTION_POS                               (9U)
#define PDS_PIN_WAKEUP_OPTION_LEN                               (1U)
#define PDS_PIN_WAKEUP_OPTION_MSK                               (((1U<<PDS_PIN_WAKEUP_OPTION_LEN)-1)<<PDS_PIN_WAKEUP_OPTION_POS)
#define PDS_PIN_WAKEUP_OPTION_UMSK                              (~(((1U<<PDS_PIN_WAKEUP_OPTION_LEN)-1)<<PDS_PIN_WAKEUP_OPTION_POS))
#define PDS_CR_PDS_RF_DONE_INT_MASK                             PDS_CR_PDS_RF_DONE_INT_MASK
#define PDS_CR_PDS_RF_DONE_INT_MASK_POS                         (10U)
#define PDS_CR_PDS_RF_DONE_INT_MASK_LEN                         (1U)
#define PDS_CR_PDS_RF_DONE_INT_MASK_MSK                         (((1U<<PDS_CR_PDS_RF_DONE_INT_MASK_LEN)-1)<<PDS_CR_PDS_RF_DONE_INT_MASK_POS)
#define PDS_CR_PDS_RF_DONE_INT_MASK_UMSK                        (~(((1U<<PDS_CR_PDS_RF_DONE_INT_MASK_LEN)-1)<<PDS_CR_PDS_RF_DONE_INT_MASK_POS))
#define PDS_TIMEOUT_EVENT                                       PDS_TIMEOUT_EVENT
#define PDS_TIMEOUT_EVENT_POS                                   (11U)
#define PDS_TIMEOUT_EVENT_LEN                                   (3U)
#define PDS_TIMEOUT_EVENT_MSK                                   (((1U<<PDS_TIMEOUT_EVENT_LEN)-1)<<PDS_TIMEOUT_EVENT_POS)
#define PDS_TIMEOUT_EVENT_UMSK                                  (~(((1U<<PDS_TIMEOUT_EVENT_LEN)-1)<<PDS_TIMEOUT_EVENT_POS))
#define PDS_EN_PDS_TIMEOUT_EVENT                                PDS_EN_PDS_TIMEOUT_EVENT
#define PDS_EN_PDS_TIMEOUT_EVENT_POS                            (14U)
#define PDS_EN_PDS_TIMEOUT_EVENT_LEN                            (1U)
#define PDS_EN_PDS_TIMEOUT_EVENT_MSK                            (((1U<<PDS_EN_PDS_TIMEOUT_EVENT_LEN)-1)<<PDS_EN_PDS_TIMEOUT_EVENT_POS)
#define PDS_EN_PDS_TIMEOUT_EVENT_UMSK                           (~(((1U<<PDS_EN_PDS_TIMEOUT_EVENT_LEN)-1)<<PDS_EN_PDS_TIMEOUT_EVENT_POS))
#define PDS_CR_PDS_INT_CLR                                      PDS_CR_PDS_INT_CLR
#define PDS_CR_PDS_INT_CLR_POS                                  (15U)
#define PDS_CR_PDS_INT_CLR_LEN                                  (1U)
#define PDS_CR_PDS_INT_CLR_MSK                                  (((1U<<PDS_CR_PDS_INT_CLR_LEN)-1)<<PDS_CR_PDS_INT_CLR_POS)
#define PDS_CR_PDS_INT_CLR_UMSK                                 (~(((1U<<PDS_CR_PDS_INT_CLR_LEN)-1)<<PDS_CR_PDS_INT_CLR_POS))
#define PDS_CR_PDS_WAKEUP_SRC_EN                                PDS_CR_PDS_WAKEUP_SRC_EN
#define PDS_CR_PDS_WAKEUP_SRC_EN_POS                            (17U)
#define PDS_CR_PDS_WAKEUP_SRC_EN_LEN                            (7U)
#define PDS_CR_PDS_WAKEUP_SRC_EN_MSK                            (((1U<<PDS_CR_PDS_WAKEUP_SRC_EN_LEN)-1)<<PDS_CR_PDS_WAKEUP_SRC_EN_POS)
#define PDS_CR_PDS_WAKEUP_SRC_EN_UMSK                           (~(((1U<<PDS_CR_PDS_WAKEUP_SRC_EN_LEN)-1)<<PDS_CR_PDS_WAKEUP_SRC_EN_POS))
#define PDS_RO_PDS_WAKEUP_EVENT                                 PDS_RO_PDS_WAKEUP_EVENT
#define PDS_RO_PDS_WAKEUP_EVENT_POS                             (25U)
#define PDS_RO_PDS_WAKEUP_EVENT_LEN                             (7U)
#define PDS_RO_PDS_WAKEUP_EVENT_MSK                             (((1U<<PDS_RO_PDS_WAKEUP_EVENT_LEN)-1)<<PDS_RO_PDS_WAKEUP_EVENT_POS)
#define PDS_RO_PDS_WAKEUP_EVENT_UMSK                            (~(((1U<<PDS_RO_PDS_WAKEUP_EVENT_LEN)-1)<<PDS_RO_PDS_WAKEUP_EVENT_POS))

/* 0x10 : PDS_CTL2 */
#define PDS_CTL2_OFFSET                                         (0x10)
#define PDS_CR_PDS_FORCE_NP_PDS_RST                             PDS_CR_PDS_FORCE_NP_PDS_RST
#define PDS_CR_PDS_FORCE_NP_PDS_RST_POS                         (8U)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_LEN                         (1U)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_MSK                         (((1U<<PDS_CR_PDS_FORCE_NP_PDS_RST_LEN)-1)<<PDS_CR_PDS_FORCE_NP_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_NP_PDS_RST_UMSK                        (~(((1U<<PDS_CR_PDS_FORCE_NP_PDS_RST_LEN)-1)<<PDS_CR_PDS_FORCE_NP_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_BZ_PDS_RST                             PDS_CR_PDS_FORCE_BZ_PDS_RST
#define PDS_CR_PDS_FORCE_BZ_PDS_RST_POS                         (10U)
#define PDS_CR_PDS_FORCE_BZ_PDS_RST_LEN                         (1U)
#define PDS_CR_PDS_FORCE_BZ_PDS_RST_MSK                         (((1U<<PDS_CR_PDS_FORCE_BZ_PDS_RST_LEN)-1)<<PDS_CR_PDS_FORCE_BZ_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_BZ_PDS_RST_UMSK                        (~(((1U<<PDS_CR_PDS_FORCE_BZ_PDS_RST_LEN)-1)<<PDS_CR_PDS_FORCE_BZ_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_NP_GATE_CLK                            PDS_CR_PDS_FORCE_NP_GATE_CLK
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_POS                        (16U)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN                        (1U)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_MSK                        (((1U<<PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN)-1)<<PDS_CR_PDS_FORCE_NP_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_NP_GATE_CLK_UMSK                       (~(((1U<<PDS_CR_PDS_FORCE_NP_GATE_CLK_LEN)-1)<<PDS_CR_PDS_FORCE_NP_GATE_CLK_POS))
#define PDS_CR_PDS_FORCE_BZ_GATE_CLK                            PDS_CR_PDS_FORCE_BZ_GATE_CLK
#define PDS_CR_PDS_FORCE_BZ_GATE_CLK_POS                        (18U)
#define PDS_CR_PDS_FORCE_BZ_GATE_CLK_LEN                        (1U)
#define PDS_CR_PDS_FORCE_BZ_GATE_CLK_MSK                        (((1U<<PDS_CR_PDS_FORCE_BZ_GATE_CLK_LEN)-1)<<PDS_CR_PDS_FORCE_BZ_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_BZ_GATE_CLK_UMSK                       (~(((1U<<PDS_CR_PDS_FORCE_BZ_GATE_CLK_LEN)-1)<<PDS_CR_PDS_FORCE_BZ_GATE_CLK_POS))

/* 0x14 : PDS_CTL3 */
#define PDS_CTL3_OFFSET                                         (0x14)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF                           PDS_CR_PDS_FORCE_MISC_PWR_OFF
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS                       (1U)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN                       (1U)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_MSK                       (((1U<<PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN)-1)<<PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS)
#define PDS_CR_PDS_FORCE_MISC_PWR_OFF_UMSK                      (~(((1U<<PDS_CR_PDS_FORCE_MISC_PWR_OFF_LEN)-1)<<PDS_CR_PDS_FORCE_MISC_PWR_OFF_POS))
#define PDS_CR_PDS_FORCE_MISC_PDS_RST                           PDS_CR_PDS_FORCE_MISC_PDS_RST
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_POS                       (7U)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN                       (1U)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_MSK                       (((1U<<PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN)-1)<<PDS_CR_PDS_FORCE_MISC_PDS_RST_POS)
#define PDS_CR_PDS_FORCE_MISC_PDS_RST_UMSK                      (~(((1U<<PDS_CR_PDS_FORCE_MISC_PDS_RST_LEN)-1)<<PDS_CR_PDS_FORCE_MISC_PDS_RST_POS))
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK                          PDS_CR_PDS_FORCE_MISC_GATE_CLK
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS                      (13U)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN                      (1U)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_MSK                      (((1U<<PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN)-1)<<PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS)
#define PDS_CR_PDS_FORCE_MISC_GATE_CLK_UMSK                     (~(((1U<<PDS_CR_PDS_FORCE_MISC_GATE_CLK_LEN)-1)<<PDS_CR_PDS_FORCE_MISC_GATE_CLK_POS))
#define PDS_CR_PDS_MISC_ISO_EN                                  PDS_CR_PDS_MISC_ISO_EN
#define PDS_CR_PDS_MISC_ISO_EN_POS                              (30U)
#define PDS_CR_PDS_MISC_ISO_EN_LEN                              (1U)
#define PDS_CR_PDS_MISC_ISO_EN_MSK                              (((1U<<PDS_CR_PDS_MISC_ISO_EN_LEN)-1)<<PDS_CR_PDS_MISC_ISO_EN_POS)
#define PDS_CR_PDS_MISC_ISO_EN_UMSK                             (~(((1U<<PDS_CR_PDS_MISC_ISO_EN_LEN)-1)<<PDS_CR_PDS_MISC_ISO_EN_POS))

/* 0x18 : PDS_CTL4 */
#define PDS_CTL4_OFFSET                                         (0x18)
#define PDS_CR_PDS_NP_RESET                                     PDS_CR_PDS_NP_RESET
#define PDS_CR_PDS_NP_RESET_POS                                 (1U)
#define PDS_CR_PDS_NP_RESET_LEN                                 (1U)
#define PDS_CR_PDS_NP_RESET_MSK                                 (((1U<<PDS_CR_PDS_NP_RESET_LEN)-1)<<PDS_CR_PDS_NP_RESET_POS)
#define PDS_CR_PDS_NP_RESET_UMSK                                (~(((1U<<PDS_CR_PDS_NP_RESET_LEN)-1)<<PDS_CR_PDS_NP_RESET_POS))
#define PDS_CR_PDS_NP_GATE_CLK                                  PDS_CR_PDS_NP_GATE_CLK
#define PDS_CR_PDS_NP_GATE_CLK_POS                              (3U)
#define PDS_CR_PDS_NP_GATE_CLK_LEN                              (1U)
#define PDS_CR_PDS_NP_GATE_CLK_MSK                              (((1U<<PDS_CR_PDS_NP_GATE_CLK_LEN)-1)<<PDS_CR_PDS_NP_GATE_CLK_POS)
#define PDS_CR_PDS_NP_GATE_CLK_UMSK                             (~(((1U<<PDS_CR_PDS_NP_GATE_CLK_LEN)-1)<<PDS_CR_PDS_NP_GATE_CLK_POS))
#define PDS_CR_PDS_BZ_RESET                                     PDS_CR_PDS_BZ_RESET
#define PDS_CR_PDS_BZ_RESET_POS                                 (13U)
#define PDS_CR_PDS_BZ_RESET_LEN                                 (1U)
#define PDS_CR_PDS_BZ_RESET_MSK                                 (((1U<<PDS_CR_PDS_BZ_RESET_LEN)-1)<<PDS_CR_PDS_BZ_RESET_POS)
#define PDS_CR_PDS_BZ_RESET_UMSK                                (~(((1U<<PDS_CR_PDS_BZ_RESET_LEN)-1)<<PDS_CR_PDS_BZ_RESET_POS))
#define PDS_CR_PDS_BZ_GATE_CLK                                  PDS_CR_PDS_BZ_GATE_CLK
#define PDS_CR_PDS_BZ_GATE_CLK_POS                              (15U)
#define PDS_CR_PDS_BZ_GATE_CLK_LEN                              (1U)
#define PDS_CR_PDS_BZ_GATE_CLK_MSK                              (((1U<<PDS_CR_PDS_BZ_GATE_CLK_LEN)-1)<<PDS_CR_PDS_BZ_GATE_CLK_POS)
#define PDS_CR_PDS_BZ_GATE_CLK_UMSK                             (~(((1U<<PDS_CR_PDS_BZ_GATE_CLK_LEN)-1)<<PDS_CR_PDS_BZ_GATE_CLK_POS))
#define PDS_CR_PDS_MISC_PWR_OFF                                 PDS_CR_PDS_MISC_PWR_OFF
#define PDS_CR_PDS_MISC_PWR_OFF_POS                             (24U)
#define PDS_CR_PDS_MISC_PWR_OFF_LEN                             (1U)
#define PDS_CR_PDS_MISC_PWR_OFF_MSK                             (((1U<<PDS_CR_PDS_MISC_PWR_OFF_LEN)-1)<<PDS_CR_PDS_MISC_PWR_OFF_POS)
#define PDS_CR_PDS_MISC_PWR_OFF_UMSK                            (~(((1U<<PDS_CR_PDS_MISC_PWR_OFF_LEN)-1)<<PDS_CR_PDS_MISC_PWR_OFF_POS))
#define PDS_CR_PDS_MISC_RESET                                   PDS_CR_PDS_MISC_RESET
#define PDS_CR_PDS_MISC_RESET_POS                               (25U)
#define PDS_CR_PDS_MISC_RESET_LEN                               (1U)
#define PDS_CR_PDS_MISC_RESET_MSK                               (((1U<<PDS_CR_PDS_MISC_RESET_LEN)-1)<<PDS_CR_PDS_MISC_RESET_POS)
#define PDS_CR_PDS_MISC_RESET_UMSK                              (~(((1U<<PDS_CR_PDS_MISC_RESET_LEN)-1)<<PDS_CR_PDS_MISC_RESET_POS))
#define PDS_CR_PDS_MISC_GATE_CLK                                PDS_CR_PDS_MISC_GATE_CLK
#define PDS_CR_PDS_MISC_GATE_CLK_POS                            (27U)
#define PDS_CR_PDS_MISC_GATE_CLK_LEN                            (1U)
#define PDS_CR_PDS_MISC_GATE_CLK_MSK                            (((1U<<PDS_CR_PDS_MISC_GATE_CLK_LEN)-1)<<PDS_CR_PDS_MISC_GATE_CLK_POS)
#define PDS_CR_PDS_MISC_GATE_CLK_UMSK                           (~(((1U<<PDS_CR_PDS_MISC_GATE_CLK_LEN)-1)<<PDS_CR_PDS_MISC_GATE_CLK_POS))

/* 0x1C : pds_stat */
#define PDS_STAT_OFFSET                                         (0x1C)
#define PDS_RO_PDS_STATE                                        PDS_RO_PDS_STATE
#define PDS_RO_PDS_STATE_POS                                    (0U)
#define PDS_RO_PDS_STATE_LEN                                    (5U)
#define PDS_RO_PDS_STATE_MSK                                    (((1U<<PDS_RO_PDS_STATE_LEN)-1)<<PDS_RO_PDS_STATE_POS)
#define PDS_RO_PDS_STATE_UMSK                                   (~(((1U<<PDS_RO_PDS_STATE_LEN)-1)<<PDS_RO_PDS_STATE_POS))
#define PDS_RO_PDS_RF_STATE                                     PDS_RO_PDS_RF_STATE
#define PDS_RO_PDS_RF_STATE_POS                                 (8U)
#define PDS_RO_PDS_RF_STATE_LEN                                 (3U)
#define PDS_RO_PDS_RF_STATE_MSK                                 (((1U<<PDS_RO_PDS_RF_STATE_LEN)-1)<<PDS_RO_PDS_RF_STATE_POS)
#define PDS_RO_PDS_RF_STATE_UMSK                                (~(((1U<<PDS_RO_PDS_RF_STATE_LEN)-1)<<PDS_RO_PDS_RF_STATE_POS))

/* 0x20 : pds_ram1 */
#define PDS_RAM1_OFFSET                                         (0x20)
#define PDS_CR_PDS_RAM_RET1N                                    PDS_CR_PDS_RAM_RET1N
#define PDS_CR_PDS_RAM_RET1N_POS                                (0U)
#define PDS_CR_PDS_RAM_RET1N_LEN                                (5U)
#define PDS_CR_PDS_RAM_RET1N_MSK                                (((1U<<PDS_CR_PDS_RAM_RET1N_LEN)-1)<<PDS_CR_PDS_RAM_RET1N_POS)
#define PDS_CR_PDS_RAM_RET1N_UMSK                               (~(((1U<<PDS_CR_PDS_RAM_RET1N_LEN)-1)<<PDS_CR_PDS_RAM_RET1N_POS))
#define PDS_CR_PDS_RAM_RET2N                                    PDS_CR_PDS_RAM_RET2N
#define PDS_CR_PDS_RAM_RET2N_POS                                (8U)
#define PDS_CR_PDS_RAM_RET2N_LEN                                (5U)
#define PDS_CR_PDS_RAM_RET2N_MSK                                (((1U<<PDS_CR_PDS_RAM_RET2N_LEN)-1)<<PDS_CR_PDS_RAM_RET2N_POS)
#define PDS_CR_PDS_RAM_RET2N_UMSK                               (~(((1U<<PDS_CR_PDS_RAM_RET2N_LEN)-1)<<PDS_CR_PDS_RAM_RET2N_POS))
#define PDS_CR_PDS_RAM_PGEN                                     PDS_CR_PDS_RAM_PGEN
#define PDS_CR_PDS_RAM_PGEN_POS                                 (16U)
#define PDS_CR_PDS_RAM_PGEN_LEN                                 (5U)
#define PDS_CR_PDS_RAM_PGEN_MSK                                 (((1U<<PDS_CR_PDS_RAM_PGEN_LEN)-1)<<PDS_CR_PDS_RAM_PGEN_POS)
#define PDS_CR_PDS_RAM_PGEN_UMSK                                (~(((1U<<PDS_CR_PDS_RAM_PGEN_LEN)-1)<<PDS_CR_PDS_RAM_PGEN_POS))

/* 0x30 : pds_gpio_pd_set */
#define PDS_GPIO_PD_SET_OFFSET                                  (0x30)
#define PDS_CR_PDS_GPIO0_PD                                     PDS_CR_PDS_GPIO0_PD
#define PDS_CR_PDS_GPIO0_PD_POS                                 (0U)
#define PDS_CR_PDS_GPIO0_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO0_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO0_PD_LEN)-1)<<PDS_CR_PDS_GPIO0_PD_POS)
#define PDS_CR_PDS_GPIO0_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO0_PD_LEN)-1)<<PDS_CR_PDS_GPIO0_PD_POS))
#define PDS_CR_PDS_GPIO1_PD                                     PDS_CR_PDS_GPIO1_PD
#define PDS_CR_PDS_GPIO1_PD_POS                                 (1U)
#define PDS_CR_PDS_GPIO1_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO1_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO1_PD_LEN)-1)<<PDS_CR_PDS_GPIO1_PD_POS)
#define PDS_CR_PDS_GPIO1_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO1_PD_LEN)-1)<<PDS_CR_PDS_GPIO1_PD_POS))
#define PDS_CR_PDS_GPIO2_PD                                     PDS_CR_PDS_GPIO2_PD
#define PDS_CR_PDS_GPIO2_PD_POS                                 (2U)
#define PDS_CR_PDS_GPIO2_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO2_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO2_PD_LEN)-1)<<PDS_CR_PDS_GPIO2_PD_POS)
#define PDS_CR_PDS_GPIO2_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO2_PD_LEN)-1)<<PDS_CR_PDS_GPIO2_PD_POS))
#define PDS_CR_PDS_GPIO3_PD                                     PDS_CR_PDS_GPIO3_PD
#define PDS_CR_PDS_GPIO3_PD_POS                                 (3U)
#define PDS_CR_PDS_GPIO3_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO3_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO3_PD_LEN)-1)<<PDS_CR_PDS_GPIO3_PD_POS)
#define PDS_CR_PDS_GPIO3_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO3_PD_LEN)-1)<<PDS_CR_PDS_GPIO3_PD_POS))
#define PDS_CR_PDS_GPIO4_PD                                     PDS_CR_PDS_GPIO4_PD
#define PDS_CR_PDS_GPIO4_PD_POS                                 (4U)
#define PDS_CR_PDS_GPIO4_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO4_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO4_PD_LEN)-1)<<PDS_CR_PDS_GPIO4_PD_POS)
#define PDS_CR_PDS_GPIO4_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO4_PD_LEN)-1)<<PDS_CR_PDS_GPIO4_PD_POS))
#define PDS_CR_PDS_GPIO5_PD                                     PDS_CR_PDS_GPIO5_PD
#define PDS_CR_PDS_GPIO5_PD_POS                                 (5U)
#define PDS_CR_PDS_GPIO5_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO5_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO5_PD_LEN)-1)<<PDS_CR_PDS_GPIO5_PD_POS)
#define PDS_CR_PDS_GPIO5_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO5_PD_LEN)-1)<<PDS_CR_PDS_GPIO5_PD_POS))
#define PDS_CR_PDS_GPIO6_PD                                     PDS_CR_PDS_GPIO6_PD
#define PDS_CR_PDS_GPIO6_PD_POS                                 (6U)
#define PDS_CR_PDS_GPIO6_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO6_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO6_PD_LEN)-1)<<PDS_CR_PDS_GPIO6_PD_POS)
#define PDS_CR_PDS_GPIO6_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO6_PD_LEN)-1)<<PDS_CR_PDS_GPIO6_PD_POS))
#define PDS_CR_PDS_GPIO7_PD                                     PDS_CR_PDS_GPIO7_PD
#define PDS_CR_PDS_GPIO7_PD_POS                                 (7U)
#define PDS_CR_PDS_GPIO7_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO7_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO7_PD_LEN)-1)<<PDS_CR_PDS_GPIO7_PD_POS)
#define PDS_CR_PDS_GPIO7_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO7_PD_LEN)-1)<<PDS_CR_PDS_GPIO7_PD_POS))
#define PDS_CR_PDS_GPIO8_PD                                     PDS_CR_PDS_GPIO8_PD
#define PDS_CR_PDS_GPIO8_PD_POS                                 (8U)
#define PDS_CR_PDS_GPIO8_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO8_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO8_PD_LEN)-1)<<PDS_CR_PDS_GPIO8_PD_POS)
#define PDS_CR_PDS_GPIO8_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO8_PD_LEN)-1)<<PDS_CR_PDS_GPIO8_PD_POS))
#define PDS_CR_PDS_GPIO9_PD                                     PDS_CR_PDS_GPIO9_PD
#define PDS_CR_PDS_GPIO9_PD_POS                                 (9U)
#define PDS_CR_PDS_GPIO9_PD_LEN                                 (1U)
#define PDS_CR_PDS_GPIO9_PD_MSK                                 (((1U<<PDS_CR_PDS_GPIO9_PD_LEN)-1)<<PDS_CR_PDS_GPIO9_PD_POS)
#define PDS_CR_PDS_GPIO9_PD_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO9_PD_LEN)-1)<<PDS_CR_PDS_GPIO9_PD_POS))
#define PDS_CR_PDS_GPIO10_PD                                    PDS_CR_PDS_GPIO10_PD
#define PDS_CR_PDS_GPIO10_PD_POS                                (10U)
#define PDS_CR_PDS_GPIO10_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO10_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO10_PD_LEN)-1)<<PDS_CR_PDS_GPIO10_PD_POS)
#define PDS_CR_PDS_GPIO10_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO10_PD_LEN)-1)<<PDS_CR_PDS_GPIO10_PD_POS))
#define PDS_CR_PDS_GPIO11_PD                                    PDS_CR_PDS_GPIO11_PD
#define PDS_CR_PDS_GPIO11_PD_POS                                (11U)
#define PDS_CR_PDS_GPIO11_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO11_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO11_PD_LEN)-1)<<PDS_CR_PDS_GPIO11_PD_POS)
#define PDS_CR_PDS_GPIO11_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO11_PD_LEN)-1)<<PDS_CR_PDS_GPIO11_PD_POS))
#define PDS_CR_PDS_GPIO12_PD                                    PDS_CR_PDS_GPIO12_PD
#define PDS_CR_PDS_GPIO12_PD_POS                                (12U)
#define PDS_CR_PDS_GPIO12_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO12_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO12_PD_LEN)-1)<<PDS_CR_PDS_GPIO12_PD_POS)
#define PDS_CR_PDS_GPIO12_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO12_PD_LEN)-1)<<PDS_CR_PDS_GPIO12_PD_POS))
#define PDS_CR_PDS_GPIO13_PD                                    PDS_CR_PDS_GPIO13_PD
#define PDS_CR_PDS_GPIO13_PD_POS                                (13U)
#define PDS_CR_PDS_GPIO13_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO13_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO13_PD_LEN)-1)<<PDS_CR_PDS_GPIO13_PD_POS)
#define PDS_CR_PDS_GPIO13_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO13_PD_LEN)-1)<<PDS_CR_PDS_GPIO13_PD_POS))
#define PDS_CR_PDS_GPIO14_PD                                    PDS_CR_PDS_GPIO14_PD
#define PDS_CR_PDS_GPIO14_PD_POS                                (14U)
#define PDS_CR_PDS_GPIO14_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO14_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO14_PD_LEN)-1)<<PDS_CR_PDS_GPIO14_PD_POS)
#define PDS_CR_PDS_GPIO14_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO14_PD_LEN)-1)<<PDS_CR_PDS_GPIO14_PD_POS))
#define PDS_CR_PDS_GPIO15_PD                                    PDS_CR_PDS_GPIO15_PD
#define PDS_CR_PDS_GPIO15_PD_POS                                (15U)
#define PDS_CR_PDS_GPIO15_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO15_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO15_PD_LEN)-1)<<PDS_CR_PDS_GPIO15_PD_POS)
#define PDS_CR_PDS_GPIO15_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO15_PD_LEN)-1)<<PDS_CR_PDS_GPIO15_PD_POS))
#define PDS_CR_PDS_GPIO16_PD                                    PDS_CR_PDS_GPIO16_PD
#define PDS_CR_PDS_GPIO16_PD_POS                                (16U)
#define PDS_CR_PDS_GPIO16_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO16_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO16_PD_LEN)-1)<<PDS_CR_PDS_GPIO16_PD_POS)
#define PDS_CR_PDS_GPIO16_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO16_PD_LEN)-1)<<PDS_CR_PDS_GPIO16_PD_POS))
#define PDS_CR_PDS_GPIO17_PD                                    PDS_CR_PDS_GPIO17_PD
#define PDS_CR_PDS_GPIO17_PD_POS                                (17U)
#define PDS_CR_PDS_GPIO17_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO17_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO17_PD_LEN)-1)<<PDS_CR_PDS_GPIO17_PD_POS)
#define PDS_CR_PDS_GPIO17_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO17_PD_LEN)-1)<<PDS_CR_PDS_GPIO17_PD_POS))
#define PDS_CR_PDS_GPIO18_PD                                    PDS_CR_PDS_GPIO18_PD
#define PDS_CR_PDS_GPIO18_PD_POS                                (18U)
#define PDS_CR_PDS_GPIO18_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO18_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO18_PD_LEN)-1)<<PDS_CR_PDS_GPIO18_PD_POS)
#define PDS_CR_PDS_GPIO18_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO18_PD_LEN)-1)<<PDS_CR_PDS_GPIO18_PD_POS))
#define PDS_CR_PDS_GPIO19_PD                                    PDS_CR_PDS_GPIO19_PD
#define PDS_CR_PDS_GPIO19_PD_POS                                (19U)
#define PDS_CR_PDS_GPIO19_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO19_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO19_PD_LEN)-1)<<PDS_CR_PDS_GPIO19_PD_POS)
#define PDS_CR_PDS_GPIO19_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO19_PD_LEN)-1)<<PDS_CR_PDS_GPIO19_PD_POS))
#define PDS_CR_PDS_GPIO20_PD                                    PDS_CR_PDS_GPIO20_PD
#define PDS_CR_PDS_GPIO20_PD_POS                                (20U)
#define PDS_CR_PDS_GPIO20_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO20_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO20_PD_LEN)-1)<<PDS_CR_PDS_GPIO20_PD_POS)
#define PDS_CR_PDS_GPIO20_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO20_PD_LEN)-1)<<PDS_CR_PDS_GPIO20_PD_POS))
#define PDS_CR_PDS_GPIO21_PD                                    PDS_CR_PDS_GPIO21_PD
#define PDS_CR_PDS_GPIO21_PD_POS                                (21U)
#define PDS_CR_PDS_GPIO21_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO21_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO21_PD_LEN)-1)<<PDS_CR_PDS_GPIO21_PD_POS)
#define PDS_CR_PDS_GPIO21_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO21_PD_LEN)-1)<<PDS_CR_PDS_GPIO21_PD_POS))
#define PDS_CR_PDS_GPIO22_PD                                    PDS_CR_PDS_GPIO22_PD
#define PDS_CR_PDS_GPIO22_PD_POS                                (22U)
#define PDS_CR_PDS_GPIO22_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO22_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO22_PD_LEN)-1)<<PDS_CR_PDS_GPIO22_PD_POS)
#define PDS_CR_PDS_GPIO22_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO22_PD_LEN)-1)<<PDS_CR_PDS_GPIO22_PD_POS))
#define PDS_CR_PDS_GPIO23_PD                                    PDS_CR_PDS_GPIO23_PD
#define PDS_CR_PDS_GPIO23_PD_POS                                (23U)
#define PDS_CR_PDS_GPIO23_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO23_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO23_PD_LEN)-1)<<PDS_CR_PDS_GPIO23_PD_POS)
#define PDS_CR_PDS_GPIO23_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO23_PD_LEN)-1)<<PDS_CR_PDS_GPIO23_PD_POS))
#define PDS_CR_PDS_GPIO24_PD                                    PDS_CR_PDS_GPIO24_PD
#define PDS_CR_PDS_GPIO24_PD_POS                                (24U)
#define PDS_CR_PDS_GPIO24_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO24_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO24_PD_LEN)-1)<<PDS_CR_PDS_GPIO24_PD_POS)
#define PDS_CR_PDS_GPIO24_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO24_PD_LEN)-1)<<PDS_CR_PDS_GPIO24_PD_POS))
#define PDS_CR_PDS_GPIO25_PD                                    PDS_CR_PDS_GPIO25_PD
#define PDS_CR_PDS_GPIO25_PD_POS                                (25U)
#define PDS_CR_PDS_GPIO25_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO25_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO25_PD_LEN)-1)<<PDS_CR_PDS_GPIO25_PD_POS)
#define PDS_CR_PDS_GPIO25_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO25_PD_LEN)-1)<<PDS_CR_PDS_GPIO25_PD_POS))
#define PDS_CR_PDS_GPIO26_PD                                    PDS_CR_PDS_GPIO26_PD
#define PDS_CR_PDS_GPIO26_PD_POS                                (26U)
#define PDS_CR_PDS_GPIO26_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO26_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO26_PD_LEN)-1)<<PDS_CR_PDS_GPIO26_PD_POS)
#define PDS_CR_PDS_GPIO26_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO26_PD_LEN)-1)<<PDS_CR_PDS_GPIO26_PD_POS))
#define PDS_CR_PDS_GPIO27_PD                                    PDS_CR_PDS_GPIO27_PD
#define PDS_CR_PDS_GPIO27_PD_POS                                (27U)
#define PDS_CR_PDS_GPIO27_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO27_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO27_PD_LEN)-1)<<PDS_CR_PDS_GPIO27_PD_POS)
#define PDS_CR_PDS_GPIO27_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO27_PD_LEN)-1)<<PDS_CR_PDS_GPIO27_PD_POS))
#define PDS_CR_PDS_GPIO28_PD                                    PDS_CR_PDS_GPIO28_PD
#define PDS_CR_PDS_GPIO28_PD_POS                                (28U)
#define PDS_CR_PDS_GPIO28_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO28_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO28_PD_LEN)-1)<<PDS_CR_PDS_GPIO28_PD_POS)
#define PDS_CR_PDS_GPIO28_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO28_PD_LEN)-1)<<PDS_CR_PDS_GPIO28_PD_POS))
#define PDS_CR_PDS_GPIO29_PD                                    PDS_CR_PDS_GPIO29_PD
#define PDS_CR_PDS_GPIO29_PD_POS                                (29U)
#define PDS_CR_PDS_GPIO29_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO29_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO29_PD_LEN)-1)<<PDS_CR_PDS_GPIO29_PD_POS)
#define PDS_CR_PDS_GPIO29_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO29_PD_LEN)-1)<<PDS_CR_PDS_GPIO29_PD_POS))
#define PDS_CR_PDS_GPIO30_PD                                    PDS_CR_PDS_GPIO30_PD
#define PDS_CR_PDS_GPIO30_PD_POS                                (30U)
#define PDS_CR_PDS_GPIO30_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO30_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO30_PD_LEN)-1)<<PDS_CR_PDS_GPIO30_PD_POS)
#define PDS_CR_PDS_GPIO30_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO30_PD_LEN)-1)<<PDS_CR_PDS_GPIO30_PD_POS))
#define PDS_CR_PDS_GPIO31_PD                                    PDS_CR_PDS_GPIO31_PD
#define PDS_CR_PDS_GPIO31_PD_POS                                (31U)
#define PDS_CR_PDS_GPIO31_PD_LEN                                (1U)
#define PDS_CR_PDS_GPIO31_PD_MSK                                (((1U<<PDS_CR_PDS_GPIO31_PD_LEN)-1)<<PDS_CR_PDS_GPIO31_PD_POS)
#define PDS_CR_PDS_GPIO31_PD_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO31_PD_LEN)-1)<<PDS_CR_PDS_GPIO31_PD_POS))

/* 0x34 : pds_gpio_pu_set */
#define PDS_GPIO_PU_SET_OFFSET                                  (0x34)
#define PDS_CR_PDS_GPIO0_PU                                     PDS_CR_PDS_GPIO0_PU
#define PDS_CR_PDS_GPIO0_PU_POS                                 (0U)
#define PDS_CR_PDS_GPIO0_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO0_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO0_PU_LEN)-1)<<PDS_CR_PDS_GPIO0_PU_POS)
#define PDS_CR_PDS_GPIO0_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO0_PU_LEN)-1)<<PDS_CR_PDS_GPIO0_PU_POS))
#define PDS_CR_PDS_GPIO1_PU                                     PDS_CR_PDS_GPIO1_PU
#define PDS_CR_PDS_GPIO1_PU_POS                                 (1U)
#define PDS_CR_PDS_GPIO1_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO1_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO1_PU_LEN)-1)<<PDS_CR_PDS_GPIO1_PU_POS)
#define PDS_CR_PDS_GPIO1_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO1_PU_LEN)-1)<<PDS_CR_PDS_GPIO1_PU_POS))
#define PDS_CR_PDS_GPIO2_PU                                     PDS_CR_PDS_GPIO2_PU
#define PDS_CR_PDS_GPIO2_PU_POS                                 (2U)
#define PDS_CR_PDS_GPIO2_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO2_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO2_PU_LEN)-1)<<PDS_CR_PDS_GPIO2_PU_POS)
#define PDS_CR_PDS_GPIO2_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO2_PU_LEN)-1)<<PDS_CR_PDS_GPIO2_PU_POS))
#define PDS_CR_PDS_GPIO3_PU                                     PDS_CR_PDS_GPIO3_PU
#define PDS_CR_PDS_GPIO3_PU_POS                                 (3U)
#define PDS_CR_PDS_GPIO3_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO3_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO3_PU_LEN)-1)<<PDS_CR_PDS_GPIO3_PU_POS)
#define PDS_CR_PDS_GPIO3_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO3_PU_LEN)-1)<<PDS_CR_PDS_GPIO3_PU_POS))
#define PDS_CR_PDS_GPIO4_PU                                     PDS_CR_PDS_GPIO4_PU
#define PDS_CR_PDS_GPIO4_PU_POS                                 (4U)
#define PDS_CR_PDS_GPIO4_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO4_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO4_PU_LEN)-1)<<PDS_CR_PDS_GPIO4_PU_POS)
#define PDS_CR_PDS_GPIO4_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO4_PU_LEN)-1)<<PDS_CR_PDS_GPIO4_PU_POS))
#define PDS_CR_PDS_GPIO5_PU                                     PDS_CR_PDS_GPIO5_PU
#define PDS_CR_PDS_GPIO5_PU_POS                                 (5U)
#define PDS_CR_PDS_GPIO5_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO5_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO5_PU_LEN)-1)<<PDS_CR_PDS_GPIO5_PU_POS)
#define PDS_CR_PDS_GPIO5_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO5_PU_LEN)-1)<<PDS_CR_PDS_GPIO5_PU_POS))
#define PDS_CR_PDS_GPIO6_PU                                     PDS_CR_PDS_GPIO6_PU
#define PDS_CR_PDS_GPIO6_PU_POS                                 (6U)
#define PDS_CR_PDS_GPIO6_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO6_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO6_PU_LEN)-1)<<PDS_CR_PDS_GPIO6_PU_POS)
#define PDS_CR_PDS_GPIO6_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO6_PU_LEN)-1)<<PDS_CR_PDS_GPIO6_PU_POS))
#define PDS_CR_PDS_GPIO7_PU                                     PDS_CR_PDS_GPIO7_PU
#define PDS_CR_PDS_GPIO7_PU_POS                                 (7U)
#define PDS_CR_PDS_GPIO7_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO7_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO7_PU_LEN)-1)<<PDS_CR_PDS_GPIO7_PU_POS)
#define PDS_CR_PDS_GPIO7_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO7_PU_LEN)-1)<<PDS_CR_PDS_GPIO7_PU_POS))
#define PDS_CR_PDS_GPIO8_PU                                     PDS_CR_PDS_GPIO8_PU
#define PDS_CR_PDS_GPIO8_PU_POS                                 (8U)
#define PDS_CR_PDS_GPIO8_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO8_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO8_PU_LEN)-1)<<PDS_CR_PDS_GPIO8_PU_POS)
#define PDS_CR_PDS_GPIO8_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO8_PU_LEN)-1)<<PDS_CR_PDS_GPIO8_PU_POS))
#define PDS_CR_PDS_GPIO9_PU                                     PDS_CR_PDS_GPIO9_PU
#define PDS_CR_PDS_GPIO9_PU_POS                                 (9U)
#define PDS_CR_PDS_GPIO9_PU_LEN                                 (1U)
#define PDS_CR_PDS_GPIO9_PU_MSK                                 (((1U<<PDS_CR_PDS_GPIO9_PU_LEN)-1)<<PDS_CR_PDS_GPIO9_PU_POS)
#define PDS_CR_PDS_GPIO9_PU_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO9_PU_LEN)-1)<<PDS_CR_PDS_GPIO9_PU_POS))
#define PDS_CR_PDS_GPIO10_PU                                    PDS_CR_PDS_GPIO10_PU
#define PDS_CR_PDS_GPIO10_PU_POS                                (10U)
#define PDS_CR_PDS_GPIO10_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO10_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO10_PU_LEN)-1)<<PDS_CR_PDS_GPIO10_PU_POS)
#define PDS_CR_PDS_GPIO10_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO10_PU_LEN)-1)<<PDS_CR_PDS_GPIO10_PU_POS))
#define PDS_CR_PDS_GPIO11_PU                                    PDS_CR_PDS_GPIO11_PU
#define PDS_CR_PDS_GPIO11_PU_POS                                (11U)
#define PDS_CR_PDS_GPIO11_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO11_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO11_PU_LEN)-1)<<PDS_CR_PDS_GPIO11_PU_POS)
#define PDS_CR_PDS_GPIO11_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO11_PU_LEN)-1)<<PDS_CR_PDS_GPIO11_PU_POS))
#define PDS_CR_PDS_GPIO12_PU                                    PDS_CR_PDS_GPIO12_PU
#define PDS_CR_PDS_GPIO12_PU_POS                                (12U)
#define PDS_CR_PDS_GPIO12_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO12_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO12_PU_LEN)-1)<<PDS_CR_PDS_GPIO12_PU_POS)
#define PDS_CR_PDS_GPIO12_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO12_PU_LEN)-1)<<PDS_CR_PDS_GPIO12_PU_POS))
#define PDS_CR_PDS_GPIO13_PU                                    PDS_CR_PDS_GPIO13_PU
#define PDS_CR_PDS_GPIO13_PU_POS                                (13U)
#define PDS_CR_PDS_GPIO13_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO13_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO13_PU_LEN)-1)<<PDS_CR_PDS_GPIO13_PU_POS)
#define PDS_CR_PDS_GPIO13_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO13_PU_LEN)-1)<<PDS_CR_PDS_GPIO13_PU_POS))
#define PDS_CR_PDS_GPIO14_PU                                    PDS_CR_PDS_GPIO14_PU
#define PDS_CR_PDS_GPIO14_PU_POS                                (14U)
#define PDS_CR_PDS_GPIO14_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO14_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO14_PU_LEN)-1)<<PDS_CR_PDS_GPIO14_PU_POS)
#define PDS_CR_PDS_GPIO14_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO14_PU_LEN)-1)<<PDS_CR_PDS_GPIO14_PU_POS))
#define PDS_CR_PDS_GPIO15_PU                                    PDS_CR_PDS_GPIO15_PU
#define PDS_CR_PDS_GPIO15_PU_POS                                (15U)
#define PDS_CR_PDS_GPIO15_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO15_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO15_PU_LEN)-1)<<PDS_CR_PDS_GPIO15_PU_POS)
#define PDS_CR_PDS_GPIO15_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO15_PU_LEN)-1)<<PDS_CR_PDS_GPIO15_PU_POS))
#define PDS_CR_PDS_GPIO16_PU                                    PDS_CR_PDS_GPIO16_PU
#define PDS_CR_PDS_GPIO16_PU_POS                                (16U)
#define PDS_CR_PDS_GPIO16_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO16_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO16_PU_LEN)-1)<<PDS_CR_PDS_GPIO16_PU_POS)
#define PDS_CR_PDS_GPIO16_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO16_PU_LEN)-1)<<PDS_CR_PDS_GPIO16_PU_POS))
#define PDS_CR_PDS_GPIO17_PU                                    PDS_CR_PDS_GPIO17_PU
#define PDS_CR_PDS_GPIO17_PU_POS                                (17U)
#define PDS_CR_PDS_GPIO17_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO17_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO17_PU_LEN)-1)<<PDS_CR_PDS_GPIO17_PU_POS)
#define PDS_CR_PDS_GPIO17_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO17_PU_LEN)-1)<<PDS_CR_PDS_GPIO17_PU_POS))
#define PDS_CR_PDS_GPIO18_PU                                    PDS_CR_PDS_GPIO18_PU
#define PDS_CR_PDS_GPIO18_PU_POS                                (18U)
#define PDS_CR_PDS_GPIO18_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO18_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO18_PU_LEN)-1)<<PDS_CR_PDS_GPIO18_PU_POS)
#define PDS_CR_PDS_GPIO18_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO18_PU_LEN)-1)<<PDS_CR_PDS_GPIO18_PU_POS))
#define PDS_CR_PDS_GPIO19_PU                                    PDS_CR_PDS_GPIO19_PU
#define PDS_CR_PDS_GPIO19_PU_POS                                (19U)
#define PDS_CR_PDS_GPIO19_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO19_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO19_PU_LEN)-1)<<PDS_CR_PDS_GPIO19_PU_POS)
#define PDS_CR_PDS_GPIO19_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO19_PU_LEN)-1)<<PDS_CR_PDS_GPIO19_PU_POS))
#define PDS_CR_PDS_GPIO20_PU                                    PDS_CR_PDS_GPIO20_PU
#define PDS_CR_PDS_GPIO20_PU_POS                                (20U)
#define PDS_CR_PDS_GPIO20_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO20_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO20_PU_LEN)-1)<<PDS_CR_PDS_GPIO20_PU_POS)
#define PDS_CR_PDS_GPIO20_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO20_PU_LEN)-1)<<PDS_CR_PDS_GPIO20_PU_POS))
#define PDS_CR_PDS_GPIO21_PU                                    PDS_CR_PDS_GPIO21_PU
#define PDS_CR_PDS_GPIO21_PU_POS                                (21U)
#define PDS_CR_PDS_GPIO21_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO21_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO21_PU_LEN)-1)<<PDS_CR_PDS_GPIO21_PU_POS)
#define PDS_CR_PDS_GPIO21_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO21_PU_LEN)-1)<<PDS_CR_PDS_GPIO21_PU_POS))
#define PDS_CR_PDS_GPIO22_PU                                    PDS_CR_PDS_GPIO22_PU
#define PDS_CR_PDS_GPIO22_PU_POS                                (22U)
#define PDS_CR_PDS_GPIO22_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO22_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO22_PU_LEN)-1)<<PDS_CR_PDS_GPIO22_PU_POS)
#define PDS_CR_PDS_GPIO22_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO22_PU_LEN)-1)<<PDS_CR_PDS_GPIO22_PU_POS))
#define PDS_CR_PDS_GPIO23_PU                                    PDS_CR_PDS_GPIO23_PU
#define PDS_CR_PDS_GPIO23_PU_POS                                (23U)
#define PDS_CR_PDS_GPIO23_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO23_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO23_PU_LEN)-1)<<PDS_CR_PDS_GPIO23_PU_POS)
#define PDS_CR_PDS_GPIO23_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO23_PU_LEN)-1)<<PDS_CR_PDS_GPIO23_PU_POS))
#define PDS_CR_PDS_GPIO24_PU                                    PDS_CR_PDS_GPIO24_PU
#define PDS_CR_PDS_GPIO24_PU_POS                                (24U)
#define PDS_CR_PDS_GPIO24_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO24_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO24_PU_LEN)-1)<<PDS_CR_PDS_GPIO24_PU_POS)
#define PDS_CR_PDS_GPIO24_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO24_PU_LEN)-1)<<PDS_CR_PDS_GPIO24_PU_POS))
#define PDS_CR_PDS_GPIO25_PU                                    PDS_CR_PDS_GPIO25_PU
#define PDS_CR_PDS_GPIO25_PU_POS                                (25U)
#define PDS_CR_PDS_GPIO25_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO25_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO25_PU_LEN)-1)<<PDS_CR_PDS_GPIO25_PU_POS)
#define PDS_CR_PDS_GPIO25_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO25_PU_LEN)-1)<<PDS_CR_PDS_GPIO25_PU_POS))
#define PDS_CR_PDS_GPIO26_PU                                    PDS_CR_PDS_GPIO26_PU
#define PDS_CR_PDS_GPIO26_PU_POS                                (26U)
#define PDS_CR_PDS_GPIO26_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO26_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO26_PU_LEN)-1)<<PDS_CR_PDS_GPIO26_PU_POS)
#define PDS_CR_PDS_GPIO26_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO26_PU_LEN)-1)<<PDS_CR_PDS_GPIO26_PU_POS))
#define PDS_CR_PDS_GPIO27_PU                                    PDS_CR_PDS_GPIO27_PU
#define PDS_CR_PDS_GPIO27_PU_POS                                (27U)
#define PDS_CR_PDS_GPIO27_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO27_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO27_PU_LEN)-1)<<PDS_CR_PDS_GPIO27_PU_POS)
#define PDS_CR_PDS_GPIO27_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO27_PU_LEN)-1)<<PDS_CR_PDS_GPIO27_PU_POS))
#define PDS_CR_PDS_GPIO28_PU                                    PDS_CR_PDS_GPIO28_PU
#define PDS_CR_PDS_GPIO28_PU_POS                                (28U)
#define PDS_CR_PDS_GPIO28_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO28_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO28_PU_LEN)-1)<<PDS_CR_PDS_GPIO28_PU_POS)
#define PDS_CR_PDS_GPIO28_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO28_PU_LEN)-1)<<PDS_CR_PDS_GPIO28_PU_POS))
#define PDS_CR_PDS_GPIO29_PU                                    PDS_CR_PDS_GPIO29_PU
#define PDS_CR_PDS_GPIO29_PU_POS                                (29U)
#define PDS_CR_PDS_GPIO29_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO29_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO29_PU_LEN)-1)<<PDS_CR_PDS_GPIO29_PU_POS)
#define PDS_CR_PDS_GPIO29_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO29_PU_LEN)-1)<<PDS_CR_PDS_GPIO29_PU_POS))
#define PDS_CR_PDS_GPIO30_PU                                    PDS_CR_PDS_GPIO30_PU
#define PDS_CR_PDS_GPIO30_PU_POS                                (30U)
#define PDS_CR_PDS_GPIO30_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO30_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO30_PU_LEN)-1)<<PDS_CR_PDS_GPIO30_PU_POS)
#define PDS_CR_PDS_GPIO30_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO30_PU_LEN)-1)<<PDS_CR_PDS_GPIO30_PU_POS))
#define PDS_CR_PDS_GPIO31_PU                                    PDS_CR_PDS_GPIO31_PU
#define PDS_CR_PDS_GPIO31_PU_POS                                (31U)
#define PDS_CR_PDS_GPIO31_PU_LEN                                (1U)
#define PDS_CR_PDS_GPIO31_PU_MSK                                (((1U<<PDS_CR_PDS_GPIO31_PU_LEN)-1)<<PDS_CR_PDS_GPIO31_PU_POS)
#define PDS_CR_PDS_GPIO31_PU_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO31_PU_LEN)-1)<<PDS_CR_PDS_GPIO31_PU_POS))

/* 0x38 : pds_gpio_ie_set */
#define PDS_GPIO_IE_SET_OFFSET                                  (0x38)
#define PDS_CR_PDS_GPIO0_IE                                     PDS_CR_PDS_GPIO0_IE
#define PDS_CR_PDS_GPIO0_IE_POS                                 (0U)
#define PDS_CR_PDS_GPIO0_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO0_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO0_IE_LEN)-1)<<PDS_CR_PDS_GPIO0_IE_POS)
#define PDS_CR_PDS_GPIO0_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO0_IE_LEN)-1)<<PDS_CR_PDS_GPIO0_IE_POS))
#define PDS_CR_PDS_GPIO1_IE                                     PDS_CR_PDS_GPIO1_IE
#define PDS_CR_PDS_GPIO1_IE_POS                                 (1U)
#define PDS_CR_PDS_GPIO1_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO1_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO1_IE_LEN)-1)<<PDS_CR_PDS_GPIO1_IE_POS)
#define PDS_CR_PDS_GPIO1_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO1_IE_LEN)-1)<<PDS_CR_PDS_GPIO1_IE_POS))
#define PDS_CR_PDS_GPIO2_IE                                     PDS_CR_PDS_GPIO2_IE
#define PDS_CR_PDS_GPIO2_IE_POS                                 (2U)
#define PDS_CR_PDS_GPIO2_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO2_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO2_IE_LEN)-1)<<PDS_CR_PDS_GPIO2_IE_POS)
#define PDS_CR_PDS_GPIO2_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO2_IE_LEN)-1)<<PDS_CR_PDS_GPIO2_IE_POS))
#define PDS_CR_PDS_GPIO3_IE                                     PDS_CR_PDS_GPIO3_IE
#define PDS_CR_PDS_GPIO3_IE_POS                                 (3U)
#define PDS_CR_PDS_GPIO3_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO3_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO3_IE_LEN)-1)<<PDS_CR_PDS_GPIO3_IE_POS)
#define PDS_CR_PDS_GPIO3_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO3_IE_LEN)-1)<<PDS_CR_PDS_GPIO3_IE_POS))
#define PDS_CR_PDS_GPIO4_IE                                     PDS_CR_PDS_GPIO4_IE
#define PDS_CR_PDS_GPIO4_IE_POS                                 (4U)
#define PDS_CR_PDS_GPIO4_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO4_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO4_IE_LEN)-1)<<PDS_CR_PDS_GPIO4_IE_POS)
#define PDS_CR_PDS_GPIO4_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO4_IE_LEN)-1)<<PDS_CR_PDS_GPIO4_IE_POS))
#define PDS_CR_PDS_GPIO5_IE                                     PDS_CR_PDS_GPIO5_IE
#define PDS_CR_PDS_GPIO5_IE_POS                                 (5U)
#define PDS_CR_PDS_GPIO5_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO5_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO5_IE_LEN)-1)<<PDS_CR_PDS_GPIO5_IE_POS)
#define PDS_CR_PDS_GPIO5_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO5_IE_LEN)-1)<<PDS_CR_PDS_GPIO5_IE_POS))
#define PDS_CR_PDS_GPIO6_IE                                     PDS_CR_PDS_GPIO6_IE
#define PDS_CR_PDS_GPIO6_IE_POS                                 (6U)
#define PDS_CR_PDS_GPIO6_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO6_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO6_IE_LEN)-1)<<PDS_CR_PDS_GPIO6_IE_POS)
#define PDS_CR_PDS_GPIO6_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO6_IE_LEN)-1)<<PDS_CR_PDS_GPIO6_IE_POS))
#define PDS_CR_PDS_GPIO7_IE                                     PDS_CR_PDS_GPIO7_IE
#define PDS_CR_PDS_GPIO7_IE_POS                                 (7U)
#define PDS_CR_PDS_GPIO7_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO7_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO7_IE_LEN)-1)<<PDS_CR_PDS_GPIO7_IE_POS)
#define PDS_CR_PDS_GPIO7_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO7_IE_LEN)-1)<<PDS_CR_PDS_GPIO7_IE_POS))
#define PDS_CR_PDS_GPIO8_IE                                     PDS_CR_PDS_GPIO8_IE
#define PDS_CR_PDS_GPIO8_IE_POS                                 (8U)
#define PDS_CR_PDS_GPIO8_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO8_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO8_IE_LEN)-1)<<PDS_CR_PDS_GPIO8_IE_POS)
#define PDS_CR_PDS_GPIO8_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO8_IE_LEN)-1)<<PDS_CR_PDS_GPIO8_IE_POS))
#define PDS_CR_PDS_GPIO9_IE                                     PDS_CR_PDS_GPIO9_IE
#define PDS_CR_PDS_GPIO9_IE_POS                                 (9U)
#define PDS_CR_PDS_GPIO9_IE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO9_IE_MSK                                 (((1U<<PDS_CR_PDS_GPIO9_IE_LEN)-1)<<PDS_CR_PDS_GPIO9_IE_POS)
#define PDS_CR_PDS_GPIO9_IE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO9_IE_LEN)-1)<<PDS_CR_PDS_GPIO9_IE_POS))
#define PDS_CR_PDS_GPIO10_IE                                    PDS_CR_PDS_GPIO10_IE
#define PDS_CR_PDS_GPIO10_IE_POS                                (10U)
#define PDS_CR_PDS_GPIO10_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO10_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO10_IE_LEN)-1)<<PDS_CR_PDS_GPIO10_IE_POS)
#define PDS_CR_PDS_GPIO10_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO10_IE_LEN)-1)<<PDS_CR_PDS_GPIO10_IE_POS))
#define PDS_CR_PDS_GPIO11_IE                                    PDS_CR_PDS_GPIO11_IE
#define PDS_CR_PDS_GPIO11_IE_POS                                (11U)
#define PDS_CR_PDS_GPIO11_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO11_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO11_IE_LEN)-1)<<PDS_CR_PDS_GPIO11_IE_POS)
#define PDS_CR_PDS_GPIO11_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO11_IE_LEN)-1)<<PDS_CR_PDS_GPIO11_IE_POS))
#define PDS_CR_PDS_GPIO12_IE                                    PDS_CR_PDS_GPIO12_IE
#define PDS_CR_PDS_GPIO12_IE_POS                                (12U)
#define PDS_CR_PDS_GPIO12_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO12_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO12_IE_LEN)-1)<<PDS_CR_PDS_GPIO12_IE_POS)
#define PDS_CR_PDS_GPIO12_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO12_IE_LEN)-1)<<PDS_CR_PDS_GPIO12_IE_POS))
#define PDS_CR_PDS_GPIO13_IE                                    PDS_CR_PDS_GPIO13_IE
#define PDS_CR_PDS_GPIO13_IE_POS                                (13U)
#define PDS_CR_PDS_GPIO13_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO13_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO13_IE_LEN)-1)<<PDS_CR_PDS_GPIO13_IE_POS)
#define PDS_CR_PDS_GPIO13_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO13_IE_LEN)-1)<<PDS_CR_PDS_GPIO13_IE_POS))
#define PDS_CR_PDS_GPIO14_IE                                    PDS_CR_PDS_GPIO14_IE
#define PDS_CR_PDS_GPIO14_IE_POS                                (14U)
#define PDS_CR_PDS_GPIO14_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO14_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO14_IE_LEN)-1)<<PDS_CR_PDS_GPIO14_IE_POS)
#define PDS_CR_PDS_GPIO14_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO14_IE_LEN)-1)<<PDS_CR_PDS_GPIO14_IE_POS))
#define PDS_CR_PDS_GPIO15_IE                                    PDS_CR_PDS_GPIO15_IE
#define PDS_CR_PDS_GPIO15_IE_POS                                (15U)
#define PDS_CR_PDS_GPIO15_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO15_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO15_IE_LEN)-1)<<PDS_CR_PDS_GPIO15_IE_POS)
#define PDS_CR_PDS_GPIO15_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO15_IE_LEN)-1)<<PDS_CR_PDS_GPIO15_IE_POS))
#define PDS_CR_PDS_GPIO16_IE                                    PDS_CR_PDS_GPIO16_IE
#define PDS_CR_PDS_GPIO16_IE_POS                                (16U)
#define PDS_CR_PDS_GPIO16_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO16_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO16_IE_LEN)-1)<<PDS_CR_PDS_GPIO16_IE_POS)
#define PDS_CR_PDS_GPIO16_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO16_IE_LEN)-1)<<PDS_CR_PDS_GPIO16_IE_POS))
#define PDS_CR_PDS_GPIO17_IE                                    PDS_CR_PDS_GPIO17_IE
#define PDS_CR_PDS_GPIO17_IE_POS                                (17U)
#define PDS_CR_PDS_GPIO17_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO17_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO17_IE_LEN)-1)<<PDS_CR_PDS_GPIO17_IE_POS)
#define PDS_CR_PDS_GPIO17_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO17_IE_LEN)-1)<<PDS_CR_PDS_GPIO17_IE_POS))
#define PDS_CR_PDS_GPIO18_IE                                    PDS_CR_PDS_GPIO18_IE
#define PDS_CR_PDS_GPIO18_IE_POS                                (18U)
#define PDS_CR_PDS_GPIO18_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO18_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO18_IE_LEN)-1)<<PDS_CR_PDS_GPIO18_IE_POS)
#define PDS_CR_PDS_GPIO18_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO18_IE_LEN)-1)<<PDS_CR_PDS_GPIO18_IE_POS))
#define PDS_CR_PDS_GPIO19_IE                                    PDS_CR_PDS_GPIO19_IE
#define PDS_CR_PDS_GPIO19_IE_POS                                (19U)
#define PDS_CR_PDS_GPIO19_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO19_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO19_IE_LEN)-1)<<PDS_CR_PDS_GPIO19_IE_POS)
#define PDS_CR_PDS_GPIO19_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO19_IE_LEN)-1)<<PDS_CR_PDS_GPIO19_IE_POS))
#define PDS_CR_PDS_GPIO20_IE                                    PDS_CR_PDS_GPIO20_IE
#define PDS_CR_PDS_GPIO20_IE_POS                                (20U)
#define PDS_CR_PDS_GPIO20_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO20_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO20_IE_LEN)-1)<<PDS_CR_PDS_GPIO20_IE_POS)
#define PDS_CR_PDS_GPIO20_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO20_IE_LEN)-1)<<PDS_CR_PDS_GPIO20_IE_POS))
#define PDS_CR_PDS_GPIO21_IE                                    PDS_CR_PDS_GPIO21_IE
#define PDS_CR_PDS_GPIO21_IE_POS                                (21U)
#define PDS_CR_PDS_GPIO21_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO21_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO21_IE_LEN)-1)<<PDS_CR_PDS_GPIO21_IE_POS)
#define PDS_CR_PDS_GPIO21_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO21_IE_LEN)-1)<<PDS_CR_PDS_GPIO21_IE_POS))
#define PDS_CR_PDS_GPIO22_IE                                    PDS_CR_PDS_GPIO22_IE
#define PDS_CR_PDS_GPIO22_IE_POS                                (22U)
#define PDS_CR_PDS_GPIO22_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO22_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO22_IE_LEN)-1)<<PDS_CR_PDS_GPIO22_IE_POS)
#define PDS_CR_PDS_GPIO22_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO22_IE_LEN)-1)<<PDS_CR_PDS_GPIO22_IE_POS))
#define PDS_CR_PDS_GPIO23_IE                                    PDS_CR_PDS_GPIO23_IE
#define PDS_CR_PDS_GPIO23_IE_POS                                (23U)
#define PDS_CR_PDS_GPIO23_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO23_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO23_IE_LEN)-1)<<PDS_CR_PDS_GPIO23_IE_POS)
#define PDS_CR_PDS_GPIO23_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO23_IE_LEN)-1)<<PDS_CR_PDS_GPIO23_IE_POS))
#define PDS_CR_PDS_GPIO24_IE                                    PDS_CR_PDS_GPIO24_IE
#define PDS_CR_PDS_GPIO24_IE_POS                                (24U)
#define PDS_CR_PDS_GPIO24_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO24_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO24_IE_LEN)-1)<<PDS_CR_PDS_GPIO24_IE_POS)
#define PDS_CR_PDS_GPIO24_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO24_IE_LEN)-1)<<PDS_CR_PDS_GPIO24_IE_POS))
#define PDS_CR_PDS_GPIO25_IE                                    PDS_CR_PDS_GPIO25_IE
#define PDS_CR_PDS_GPIO25_IE_POS                                (25U)
#define PDS_CR_PDS_GPIO25_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO25_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO25_IE_LEN)-1)<<PDS_CR_PDS_GPIO25_IE_POS)
#define PDS_CR_PDS_GPIO25_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO25_IE_LEN)-1)<<PDS_CR_PDS_GPIO25_IE_POS))
#define PDS_CR_PDS_GPIO26_IE                                    PDS_CR_PDS_GPIO26_IE
#define PDS_CR_PDS_GPIO26_IE_POS                                (26U)
#define PDS_CR_PDS_GPIO26_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO26_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO26_IE_LEN)-1)<<PDS_CR_PDS_GPIO26_IE_POS)
#define PDS_CR_PDS_GPIO26_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO26_IE_LEN)-1)<<PDS_CR_PDS_GPIO26_IE_POS))
#define PDS_CR_PDS_GPIO27_IE                                    PDS_CR_PDS_GPIO27_IE
#define PDS_CR_PDS_GPIO27_IE_POS                                (27U)
#define PDS_CR_PDS_GPIO27_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO27_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO27_IE_LEN)-1)<<PDS_CR_PDS_GPIO27_IE_POS)
#define PDS_CR_PDS_GPIO27_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO27_IE_LEN)-1)<<PDS_CR_PDS_GPIO27_IE_POS))
#define PDS_CR_PDS_GPIO28_IE                                    PDS_CR_PDS_GPIO28_IE
#define PDS_CR_PDS_GPIO28_IE_POS                                (28U)
#define PDS_CR_PDS_GPIO28_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO28_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO28_IE_LEN)-1)<<PDS_CR_PDS_GPIO28_IE_POS)
#define PDS_CR_PDS_GPIO28_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO28_IE_LEN)-1)<<PDS_CR_PDS_GPIO28_IE_POS))
#define PDS_CR_PDS_GPIO29_IE                                    PDS_CR_PDS_GPIO29_IE
#define PDS_CR_PDS_GPIO29_IE_POS                                (29U)
#define PDS_CR_PDS_GPIO29_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO29_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO29_IE_LEN)-1)<<PDS_CR_PDS_GPIO29_IE_POS)
#define PDS_CR_PDS_GPIO29_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO29_IE_LEN)-1)<<PDS_CR_PDS_GPIO29_IE_POS))
#define PDS_CR_PDS_GPIO30_IE                                    PDS_CR_PDS_GPIO30_IE
#define PDS_CR_PDS_GPIO30_IE_POS                                (30U)
#define PDS_CR_PDS_GPIO30_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO30_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO30_IE_LEN)-1)<<PDS_CR_PDS_GPIO30_IE_POS)
#define PDS_CR_PDS_GPIO30_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO30_IE_LEN)-1)<<PDS_CR_PDS_GPIO30_IE_POS))
#define PDS_CR_PDS_GPIO31_IE                                    PDS_CR_PDS_GPIO31_IE
#define PDS_CR_PDS_GPIO31_IE_POS                                (31U)
#define PDS_CR_PDS_GPIO31_IE_LEN                                (1U)
#define PDS_CR_PDS_GPIO31_IE_MSK                                (((1U<<PDS_CR_PDS_GPIO31_IE_LEN)-1)<<PDS_CR_PDS_GPIO31_IE_POS)
#define PDS_CR_PDS_GPIO31_IE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO31_IE_LEN)-1)<<PDS_CR_PDS_GPIO31_IE_POS))

/* 0x3C : pds_gpio_oe_set */
#define PDS_GPIO_OE_SET_OFFSET                                  (0x3C)
#define PDS_CR_PDS_GPIO0_OE                                     PDS_CR_PDS_GPIO0_OE
#define PDS_CR_PDS_GPIO0_OE_POS                                 (0U)
#define PDS_CR_PDS_GPIO0_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO0_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO0_OE_LEN)-1)<<PDS_CR_PDS_GPIO0_OE_POS)
#define PDS_CR_PDS_GPIO0_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO0_OE_LEN)-1)<<PDS_CR_PDS_GPIO0_OE_POS))
#define PDS_CR_PDS_GPIO1_OE                                     PDS_CR_PDS_GPIO1_OE
#define PDS_CR_PDS_GPIO1_OE_POS                                 (1U)
#define PDS_CR_PDS_GPIO1_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO1_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO1_OE_LEN)-1)<<PDS_CR_PDS_GPIO1_OE_POS)
#define PDS_CR_PDS_GPIO1_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO1_OE_LEN)-1)<<PDS_CR_PDS_GPIO1_OE_POS))
#define PDS_CR_PDS_GPIO2_OE                                     PDS_CR_PDS_GPIO2_OE
#define PDS_CR_PDS_GPIO2_OE_POS                                 (2U)
#define PDS_CR_PDS_GPIO2_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO2_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO2_OE_LEN)-1)<<PDS_CR_PDS_GPIO2_OE_POS)
#define PDS_CR_PDS_GPIO2_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO2_OE_LEN)-1)<<PDS_CR_PDS_GPIO2_OE_POS))
#define PDS_CR_PDS_GPIO3_OE                                     PDS_CR_PDS_GPIO3_OE
#define PDS_CR_PDS_GPIO3_OE_POS                                 (3U)
#define PDS_CR_PDS_GPIO3_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO3_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO3_OE_LEN)-1)<<PDS_CR_PDS_GPIO3_OE_POS)
#define PDS_CR_PDS_GPIO3_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO3_OE_LEN)-1)<<PDS_CR_PDS_GPIO3_OE_POS))
#define PDS_CR_PDS_GPIO4_OE                                     PDS_CR_PDS_GPIO4_OE
#define PDS_CR_PDS_GPIO4_OE_POS                                 (4U)
#define PDS_CR_PDS_GPIO4_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO4_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO4_OE_LEN)-1)<<PDS_CR_PDS_GPIO4_OE_POS)
#define PDS_CR_PDS_GPIO4_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO4_OE_LEN)-1)<<PDS_CR_PDS_GPIO4_OE_POS))
#define PDS_CR_PDS_GPIO5_OE                                     PDS_CR_PDS_GPIO5_OE
#define PDS_CR_PDS_GPIO5_OE_POS                                 (5U)
#define PDS_CR_PDS_GPIO5_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO5_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO5_OE_LEN)-1)<<PDS_CR_PDS_GPIO5_OE_POS)
#define PDS_CR_PDS_GPIO5_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO5_OE_LEN)-1)<<PDS_CR_PDS_GPIO5_OE_POS))
#define PDS_CR_PDS_GPIO6_OE                                     PDS_CR_PDS_GPIO6_OE
#define PDS_CR_PDS_GPIO6_OE_POS                                 (6U)
#define PDS_CR_PDS_GPIO6_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO6_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO6_OE_LEN)-1)<<PDS_CR_PDS_GPIO6_OE_POS)
#define PDS_CR_PDS_GPIO6_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO6_OE_LEN)-1)<<PDS_CR_PDS_GPIO6_OE_POS))
#define PDS_CR_PDS_GPIO7_OE                                     PDS_CR_PDS_GPIO7_OE
#define PDS_CR_PDS_GPIO7_OE_POS                                 (7U)
#define PDS_CR_PDS_GPIO7_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO7_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO7_OE_LEN)-1)<<PDS_CR_PDS_GPIO7_OE_POS)
#define PDS_CR_PDS_GPIO7_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO7_OE_LEN)-1)<<PDS_CR_PDS_GPIO7_OE_POS))
#define PDS_CR_PDS_GPIO8_OE                                     PDS_CR_PDS_GPIO8_OE
#define PDS_CR_PDS_GPIO8_OE_POS                                 (8U)
#define PDS_CR_PDS_GPIO8_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO8_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO8_OE_LEN)-1)<<PDS_CR_PDS_GPIO8_OE_POS)
#define PDS_CR_PDS_GPIO8_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO8_OE_LEN)-1)<<PDS_CR_PDS_GPIO8_OE_POS))
#define PDS_CR_PDS_GPIO9_OE                                     PDS_CR_PDS_GPIO9_OE
#define PDS_CR_PDS_GPIO9_OE_POS                                 (9U)
#define PDS_CR_PDS_GPIO9_OE_LEN                                 (1U)
#define PDS_CR_PDS_GPIO9_OE_MSK                                 (((1U<<PDS_CR_PDS_GPIO9_OE_LEN)-1)<<PDS_CR_PDS_GPIO9_OE_POS)
#define PDS_CR_PDS_GPIO9_OE_UMSK                                (~(((1U<<PDS_CR_PDS_GPIO9_OE_LEN)-1)<<PDS_CR_PDS_GPIO9_OE_POS))
#define PDS_CR_PDS_GPIO10_OE                                    PDS_CR_PDS_GPIO10_OE
#define PDS_CR_PDS_GPIO10_OE_POS                                (10U)
#define PDS_CR_PDS_GPIO10_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO10_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO10_OE_LEN)-1)<<PDS_CR_PDS_GPIO10_OE_POS)
#define PDS_CR_PDS_GPIO10_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO10_OE_LEN)-1)<<PDS_CR_PDS_GPIO10_OE_POS))
#define PDS_CR_PDS_GPIO11_OE                                    PDS_CR_PDS_GPIO11_OE
#define PDS_CR_PDS_GPIO11_OE_POS                                (11U)
#define PDS_CR_PDS_GPIO11_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO11_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO11_OE_LEN)-1)<<PDS_CR_PDS_GPIO11_OE_POS)
#define PDS_CR_PDS_GPIO11_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO11_OE_LEN)-1)<<PDS_CR_PDS_GPIO11_OE_POS))
#define PDS_CR_PDS_GPIO12_OE                                    PDS_CR_PDS_GPIO12_OE
#define PDS_CR_PDS_GPIO12_OE_POS                                (12U)
#define PDS_CR_PDS_GPIO12_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO12_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO12_OE_LEN)-1)<<PDS_CR_PDS_GPIO12_OE_POS)
#define PDS_CR_PDS_GPIO12_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO12_OE_LEN)-1)<<PDS_CR_PDS_GPIO12_OE_POS))
#define PDS_CR_PDS_GPIO13_OE                                    PDS_CR_PDS_GPIO13_OE
#define PDS_CR_PDS_GPIO13_OE_POS                                (13U)
#define PDS_CR_PDS_GPIO13_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO13_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO13_OE_LEN)-1)<<PDS_CR_PDS_GPIO13_OE_POS)
#define PDS_CR_PDS_GPIO13_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO13_OE_LEN)-1)<<PDS_CR_PDS_GPIO13_OE_POS))
#define PDS_CR_PDS_GPIO14_OE                                    PDS_CR_PDS_GPIO14_OE
#define PDS_CR_PDS_GPIO14_OE_POS                                (14U)
#define PDS_CR_PDS_GPIO14_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO14_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO14_OE_LEN)-1)<<PDS_CR_PDS_GPIO14_OE_POS)
#define PDS_CR_PDS_GPIO14_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO14_OE_LEN)-1)<<PDS_CR_PDS_GPIO14_OE_POS))
#define PDS_CR_PDS_GPIO15_OE                                    PDS_CR_PDS_GPIO15_OE
#define PDS_CR_PDS_GPIO15_OE_POS                                (15U)
#define PDS_CR_PDS_GPIO15_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO15_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO15_OE_LEN)-1)<<PDS_CR_PDS_GPIO15_OE_POS)
#define PDS_CR_PDS_GPIO15_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO15_OE_LEN)-1)<<PDS_CR_PDS_GPIO15_OE_POS))
#define PDS_CR_PDS_GPIO16_OE                                    PDS_CR_PDS_GPIO16_OE
#define PDS_CR_PDS_GPIO16_OE_POS                                (16U)
#define PDS_CR_PDS_GPIO16_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO16_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO16_OE_LEN)-1)<<PDS_CR_PDS_GPIO16_OE_POS)
#define PDS_CR_PDS_GPIO16_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO16_OE_LEN)-1)<<PDS_CR_PDS_GPIO16_OE_POS))
#define PDS_CR_PDS_GPIO17_OE                                    PDS_CR_PDS_GPIO17_OE
#define PDS_CR_PDS_GPIO17_OE_POS                                (17U)
#define PDS_CR_PDS_GPIO17_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO17_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO17_OE_LEN)-1)<<PDS_CR_PDS_GPIO17_OE_POS)
#define PDS_CR_PDS_GPIO17_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO17_OE_LEN)-1)<<PDS_CR_PDS_GPIO17_OE_POS))
#define PDS_CR_PDS_GPIO18_OE                                    PDS_CR_PDS_GPIO18_OE
#define PDS_CR_PDS_GPIO18_OE_POS                                (18U)
#define PDS_CR_PDS_GPIO18_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO18_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO18_OE_LEN)-1)<<PDS_CR_PDS_GPIO18_OE_POS)
#define PDS_CR_PDS_GPIO18_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO18_OE_LEN)-1)<<PDS_CR_PDS_GPIO18_OE_POS))
#define PDS_CR_PDS_GPIO19_OE                                    PDS_CR_PDS_GPIO19_OE
#define PDS_CR_PDS_GPIO19_OE_POS                                (19U)
#define PDS_CR_PDS_GPIO19_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO19_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO19_OE_LEN)-1)<<PDS_CR_PDS_GPIO19_OE_POS)
#define PDS_CR_PDS_GPIO19_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO19_OE_LEN)-1)<<PDS_CR_PDS_GPIO19_OE_POS))
#define PDS_CR_PDS_GPIO20_OE                                    PDS_CR_PDS_GPIO20_OE
#define PDS_CR_PDS_GPIO20_OE_POS                                (20U)
#define PDS_CR_PDS_GPIO20_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO20_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO20_OE_LEN)-1)<<PDS_CR_PDS_GPIO20_OE_POS)
#define PDS_CR_PDS_GPIO20_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO20_OE_LEN)-1)<<PDS_CR_PDS_GPIO20_OE_POS))
#define PDS_CR_PDS_GPIO21_OE                                    PDS_CR_PDS_GPIO21_OE
#define PDS_CR_PDS_GPIO21_OE_POS                                (21U)
#define PDS_CR_PDS_GPIO21_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO21_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO21_OE_LEN)-1)<<PDS_CR_PDS_GPIO21_OE_POS)
#define PDS_CR_PDS_GPIO21_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO21_OE_LEN)-1)<<PDS_CR_PDS_GPIO21_OE_POS))
#define PDS_CR_PDS_GPIO22_OE                                    PDS_CR_PDS_GPIO22_OE
#define PDS_CR_PDS_GPIO22_OE_POS                                (22U)
#define PDS_CR_PDS_GPIO22_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO22_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO22_OE_LEN)-1)<<PDS_CR_PDS_GPIO22_OE_POS)
#define PDS_CR_PDS_GPIO22_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO22_OE_LEN)-1)<<PDS_CR_PDS_GPIO22_OE_POS))
#define PDS_CR_PDS_GPIO23_OE                                    PDS_CR_PDS_GPIO23_OE
#define PDS_CR_PDS_GPIO23_OE_POS                                (23U)
#define PDS_CR_PDS_GPIO23_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO23_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO23_OE_LEN)-1)<<PDS_CR_PDS_GPIO23_OE_POS)
#define PDS_CR_PDS_GPIO23_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO23_OE_LEN)-1)<<PDS_CR_PDS_GPIO23_OE_POS))
#define PDS_CR_PDS_GPIO24_OE                                    PDS_CR_PDS_GPIO24_OE
#define PDS_CR_PDS_GPIO24_OE_POS                                (24U)
#define PDS_CR_PDS_GPIO24_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO24_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO24_OE_LEN)-1)<<PDS_CR_PDS_GPIO24_OE_POS)
#define PDS_CR_PDS_GPIO24_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO24_OE_LEN)-1)<<PDS_CR_PDS_GPIO24_OE_POS))
#define PDS_CR_PDS_GPIO25_OE                                    PDS_CR_PDS_GPIO25_OE
#define PDS_CR_PDS_GPIO25_OE_POS                                (25U)
#define PDS_CR_PDS_GPIO25_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO25_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO25_OE_LEN)-1)<<PDS_CR_PDS_GPIO25_OE_POS)
#define PDS_CR_PDS_GPIO25_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO25_OE_LEN)-1)<<PDS_CR_PDS_GPIO25_OE_POS))
#define PDS_CR_PDS_GPIO26_OE                                    PDS_CR_PDS_GPIO26_OE
#define PDS_CR_PDS_GPIO26_OE_POS                                (26U)
#define PDS_CR_PDS_GPIO26_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO26_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO26_OE_LEN)-1)<<PDS_CR_PDS_GPIO26_OE_POS)
#define PDS_CR_PDS_GPIO26_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO26_OE_LEN)-1)<<PDS_CR_PDS_GPIO26_OE_POS))
#define PDS_CR_PDS_GPIO27_OE                                    PDS_CR_PDS_GPIO27_OE
#define PDS_CR_PDS_GPIO27_OE_POS                                (27U)
#define PDS_CR_PDS_GPIO27_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO27_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO27_OE_LEN)-1)<<PDS_CR_PDS_GPIO27_OE_POS)
#define PDS_CR_PDS_GPIO27_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO27_OE_LEN)-1)<<PDS_CR_PDS_GPIO27_OE_POS))
#define PDS_CR_PDS_GPIO28_OE                                    PDS_CR_PDS_GPIO28_OE
#define PDS_CR_PDS_GPIO28_OE_POS                                (28U)
#define PDS_CR_PDS_GPIO28_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO28_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO28_OE_LEN)-1)<<PDS_CR_PDS_GPIO28_OE_POS)
#define PDS_CR_PDS_GPIO28_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO28_OE_LEN)-1)<<PDS_CR_PDS_GPIO28_OE_POS))
#define PDS_CR_PDS_GPIO29_OE                                    PDS_CR_PDS_GPIO29_OE
#define PDS_CR_PDS_GPIO29_OE_POS                                (29U)
#define PDS_CR_PDS_GPIO29_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO29_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO29_OE_LEN)-1)<<PDS_CR_PDS_GPIO29_OE_POS)
#define PDS_CR_PDS_GPIO29_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO29_OE_LEN)-1)<<PDS_CR_PDS_GPIO29_OE_POS))
#define PDS_CR_PDS_GPIO30_OE                                    PDS_CR_PDS_GPIO30_OE
#define PDS_CR_PDS_GPIO30_OE_POS                                (30U)
#define PDS_CR_PDS_GPIO30_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO30_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO30_OE_LEN)-1)<<PDS_CR_PDS_GPIO30_OE_POS)
#define PDS_CR_PDS_GPIO30_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO30_OE_LEN)-1)<<PDS_CR_PDS_GPIO30_OE_POS))
#define PDS_CR_PDS_GPIO31_OE                                    PDS_CR_PDS_GPIO31_OE
#define PDS_CR_PDS_GPIO31_OE_POS                                (31U)
#define PDS_CR_PDS_GPIO31_OE_LEN                                (1U)
#define PDS_CR_PDS_GPIO31_OE_MSK                                (((1U<<PDS_CR_PDS_GPIO31_OE_LEN)-1)<<PDS_CR_PDS_GPIO31_OE_POS)
#define PDS_CR_PDS_GPIO31_OE_UMSK                               (~(((1U<<PDS_CR_PDS_GPIO31_OE_LEN)-1)<<PDS_CR_PDS_GPIO31_OE_POS))

/* 0x40 : pds_gpio_int_set */
#define PDS_GPIO_INT_SET_OFFSET                                 (0x40)
#define PDS_GPIO_SET0_INT_MODE                                  PDS_GPIO_SET0_INT_MODE
#define PDS_GPIO_SET0_INT_MODE_POS                              (0U)
#define PDS_GPIO_SET0_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET0_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET0_INT_MODE_LEN)-1)<<PDS_GPIO_SET0_INT_MODE_POS)
#define PDS_GPIO_SET0_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET0_INT_MODE_LEN)-1)<<PDS_GPIO_SET0_INT_MODE_POS))
#define PDS_GPIO_SET1_INT_MODE                                  PDS_GPIO_SET1_INT_MODE
#define PDS_GPIO_SET1_INT_MODE_POS                              (4U)
#define PDS_GPIO_SET1_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET1_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET1_INT_MODE_LEN)-1)<<PDS_GPIO_SET1_INT_MODE_POS)
#define PDS_GPIO_SET1_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET1_INT_MODE_LEN)-1)<<PDS_GPIO_SET1_INT_MODE_POS))
#define PDS_GPIO_SET2_INT_MODE                                  PDS_GPIO_SET2_INT_MODE
#define PDS_GPIO_SET2_INT_MODE_POS                              (8U)
#define PDS_GPIO_SET2_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET2_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET2_INT_MODE_LEN)-1)<<PDS_GPIO_SET2_INT_MODE_POS)
#define PDS_GPIO_SET2_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET2_INT_MODE_LEN)-1)<<PDS_GPIO_SET2_INT_MODE_POS))
#define PDS_GPIO_SET3_INT_MODE                                  PDS_GPIO_SET3_INT_MODE
#define PDS_GPIO_SET3_INT_MODE_POS                              (12U)
#define PDS_GPIO_SET3_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET3_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET3_INT_MODE_LEN)-1)<<PDS_GPIO_SET3_INT_MODE_POS)
#define PDS_GPIO_SET3_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET3_INT_MODE_LEN)-1)<<PDS_GPIO_SET3_INT_MODE_POS))
#define PDS_GPIO_SET4_INT_MODE                                  PDS_GPIO_SET4_INT_MODE
#define PDS_GPIO_SET4_INT_MODE_POS                              (16U)
#define PDS_GPIO_SET4_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET4_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET4_INT_MODE_LEN)-1)<<PDS_GPIO_SET4_INT_MODE_POS)
#define PDS_GPIO_SET4_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET4_INT_MODE_LEN)-1)<<PDS_GPIO_SET4_INT_MODE_POS))
#define PDS_GPIO_SET5_INT_MODE                                  PDS_GPIO_SET5_INT_MODE
#define PDS_GPIO_SET5_INT_MODE_POS                              (20U)
#define PDS_GPIO_SET5_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET5_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET5_INT_MODE_LEN)-1)<<PDS_GPIO_SET5_INT_MODE_POS)
#define PDS_GPIO_SET5_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET5_INT_MODE_LEN)-1)<<PDS_GPIO_SET5_INT_MODE_POS))
#define PDS_GPIO_SET6_INT_MODE                                  PDS_GPIO_SET6_INT_MODE
#define PDS_GPIO_SET6_INT_MODE_POS                              (24U)
#define PDS_GPIO_SET6_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET6_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET6_INT_MODE_LEN)-1)<<PDS_GPIO_SET6_INT_MODE_POS)
#define PDS_GPIO_SET6_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET6_INT_MODE_LEN)-1)<<PDS_GPIO_SET6_INT_MODE_POS))
#define PDS_GPIO_SET7_INT_MODE                                  PDS_GPIO_SET7_INT_MODE
#define PDS_GPIO_SET7_INT_MODE_POS                              (28U)
#define PDS_GPIO_SET7_INT_MODE_LEN                              (4U)
#define PDS_GPIO_SET7_INT_MODE_MSK                              (((1U<<PDS_GPIO_SET7_INT_MODE_LEN)-1)<<PDS_GPIO_SET7_INT_MODE_POS)
#define PDS_GPIO_SET7_INT_MODE_UMSK                             (~(((1U<<PDS_GPIO_SET7_INT_MODE_LEN)-1)<<PDS_GPIO_SET7_INT_MODE_POS))

/* 0x44 : pds_gpio_stat */
#define PDS_GPIO_STAT_OFFSET                                    (0x44)
#define PDS_GPIO_INT_STAT                                       PDS_GPIO_INT_STAT
#define PDS_GPIO_INT_STAT_POS                                   (0U)
#define PDS_GPIO_INT_STAT_LEN                                   (32U)
#define PDS_GPIO_INT_STAT_MSK                                   (((1U<<PDS_GPIO_INT_STAT_LEN)-1)<<PDS_GPIO_INT_STAT_POS)
#define PDS_GPIO_INT_STAT_UMSK                                  (~(((1U<<PDS_GPIO_INT_STAT_LEN)-1)<<PDS_GPIO_INT_STAT_POS))

/* 0x48 : cfg_pds_key_scan */
#define PDS_CFG_PDS_KEY_SCAN_OFFSET                             (0x48)
#define PDS_CR_PDS_GPIO_SET_O_VAL                               PDS_CR_PDS_GPIO_SET_O_VAL
#define PDS_CR_PDS_GPIO_SET_O_VAL_POS                           (0U)
#define PDS_CR_PDS_GPIO_SET_O_VAL_LEN                           (8U)
#define PDS_CR_PDS_GPIO_SET_O_VAL_MSK                           (((1U<<PDS_CR_PDS_GPIO_SET_O_VAL_LEN)-1)<<PDS_CR_PDS_GPIO_SET_O_VAL_POS)
#define PDS_CR_PDS_GPIO_SET_O_VAL_UMSK                          (~(((1U<<PDS_CR_PDS_GPIO_SET_O_VAL_LEN)-1)<<PDS_CR_PDS_GPIO_SET_O_VAL_POS))
#define PDS_CR_PDS_GPIO_SET_INT_MASK                            PDS_CR_PDS_GPIO_SET_INT_MASK
#define PDS_CR_PDS_GPIO_SET_INT_MASK_POS                        (8U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK_LEN                        (8U)
#define PDS_CR_PDS_GPIO_SET_INT_MASK_MSK                        (((1U<<PDS_CR_PDS_GPIO_SET_INT_MASK_LEN)-1)<<PDS_CR_PDS_GPIO_SET_INT_MASK_POS)
#define PDS_CR_PDS_GPIO_SET_INT_MASK_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO_SET_INT_MASK_LEN)-1)<<PDS_CR_PDS_GPIO_SET_INT_MASK_POS))
#define PDS_CR_PDS_GPIO0_SET_INT_CLR                            PDS_CR_PDS_GPIO0_SET_INT_CLR
#define PDS_CR_PDS_GPIO0_SET_INT_CLR_POS                        (24U)
#define PDS_CR_PDS_GPIO0_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO0_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO0_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO0_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO0_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO0_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO0_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO1_SET_INT_CLR                            PDS_CR_PDS_GPIO1_SET_INT_CLR
#define PDS_CR_PDS_GPIO1_SET_INT_CLR_POS                        (25U)
#define PDS_CR_PDS_GPIO1_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO1_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO1_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO1_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO1_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO1_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO1_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO2_SET_INT_CLR                            PDS_CR_PDS_GPIO2_SET_INT_CLR
#define PDS_CR_PDS_GPIO2_SET_INT_CLR_POS                        (26U)
#define PDS_CR_PDS_GPIO2_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO2_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO2_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO2_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO2_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO2_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO2_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO3_SET_INT_CLR                            PDS_CR_PDS_GPIO3_SET_INT_CLR
#define PDS_CR_PDS_GPIO3_SET_INT_CLR_POS                        (27U)
#define PDS_CR_PDS_GPIO3_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO3_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO3_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO3_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO3_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO3_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO3_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO4_SET_INT_CLR                            PDS_CR_PDS_GPIO4_SET_INT_CLR
#define PDS_CR_PDS_GPIO4_SET_INT_CLR_POS                        (28U)
#define PDS_CR_PDS_GPIO4_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO4_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO4_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO4_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO4_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO4_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO4_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO5_SET_INT_CLR                            PDS_CR_PDS_GPIO5_SET_INT_CLR
#define PDS_CR_PDS_GPIO5_SET_INT_CLR_POS                        (29U)
#define PDS_CR_PDS_GPIO5_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO5_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO5_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO5_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO5_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO5_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO5_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO6_SET_INT_CLR                            PDS_CR_PDS_GPIO6_SET_INT_CLR
#define PDS_CR_PDS_GPIO6_SET_INT_CLR_POS                        (30U)
#define PDS_CR_PDS_GPIO6_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO6_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO6_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO6_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO6_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO6_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO6_SET_INT_CLR_POS))
#define PDS_CR_PDS_GPIO7_SET_INT_CLR                            PDS_CR_PDS_GPIO7_SET_INT_CLR
#define PDS_CR_PDS_GPIO7_SET_INT_CLR_POS                        (31U)
#define PDS_CR_PDS_GPIO7_SET_INT_CLR_LEN                        (1U)
#define PDS_CR_PDS_GPIO7_SET_INT_CLR_MSK                        (((1U<<PDS_CR_PDS_GPIO7_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO7_SET_INT_CLR_POS)
#define PDS_CR_PDS_GPIO7_SET_INT_CLR_UMSK                       (~(((1U<<PDS_CR_PDS_GPIO7_SET_INT_CLR_LEN)-1)<<PDS_CR_PDS_GPIO7_SET_INT_CLR_POS))

/* 0x4C : XTAL_CNT_32K */
#define PDS_XTAL_CNT_32K_OFFSET                                 (0x4C)
#define PDS_RO_XTAL_CNT_32K_RES                                 PDS_RO_XTAL_CNT_32K_RES
#define PDS_RO_XTAL_CNT_32K_RES_POS                             (0U)
#define PDS_RO_XTAL_CNT_32K_RES_LEN                             (6U)
#define PDS_RO_XTAL_CNT_32K_RES_MSK                             (((1U<<PDS_RO_XTAL_CNT_32K_RES_LEN)-1)<<PDS_RO_XTAL_CNT_32K_RES_POS)
#define PDS_RO_XTAL_CNT_32K_RES_UMSK                            (~(((1U<<PDS_RO_XTAL_CNT_32K_RES_LEN)-1)<<PDS_RO_XTAL_CNT_32K_RES_POS))
#define PDS_RO_XTAL_CNT_32K_CNT                                 PDS_RO_XTAL_CNT_32K_CNT
#define PDS_RO_XTAL_CNT_32K_CNT_POS                             (6U)
#define PDS_RO_XTAL_CNT_32K_CNT_LEN                             (11U)
#define PDS_RO_XTAL_CNT_32K_CNT_MSK                             (((1U<<PDS_RO_XTAL_CNT_32K_CNT_LEN)-1)<<PDS_RO_XTAL_CNT_32K_CNT_POS)
#define PDS_RO_XTAL_CNT_32K_CNT_UMSK                            (~(((1U<<PDS_RO_XTAL_CNT_32K_CNT_LEN)-1)<<PDS_RO_XTAL_CNT_32K_CNT_POS))
#define PDS_REG_TOTAL_32K_CYCLE                                 PDS_REG_TOTAL_32K_CYCLE
#define PDS_REG_TOTAL_32K_CYCLE_POS                             (17U)
#define PDS_REG_TOTAL_32K_CYCLE_LEN                             (3U)
#define PDS_REG_TOTAL_32K_CYCLE_MSK                             (((1U<<PDS_REG_TOTAL_32K_CYCLE_LEN)-1)<<PDS_REG_TOTAL_32K_CYCLE_POS)
#define PDS_REG_TOTAL_32K_CYCLE_UMSK                            (~(((1U<<PDS_REG_TOTAL_32K_CYCLE_LEN)-1)<<PDS_REG_TOTAL_32K_CYCLE_POS))
#define PDS_XTAL_CNT_32K_PROCESS                                PDS_XTAL_CNT_32K_PROCESS
#define PDS_XTAL_CNT_32K_PROCESS_POS                            (29U)
#define PDS_XTAL_CNT_32K_PROCESS_LEN                            (1U)
#define PDS_XTAL_CNT_32K_PROCESS_MSK                            (((1U<<PDS_XTAL_CNT_32K_PROCESS_LEN)-1)<<PDS_XTAL_CNT_32K_PROCESS_POS)
#define PDS_XTAL_CNT_32K_PROCESS_UMSK                           (~(((1U<<PDS_XTAL_CNT_32K_PROCESS_LEN)-1)<<PDS_XTAL_CNT_32K_PROCESS_POS))
#define PDS_XTAL_CNT_32K_DONE                                   PDS_XTAL_CNT_32K_DONE
#define PDS_XTAL_CNT_32K_DONE_POS                               (30U)
#define PDS_XTAL_CNT_32K_DONE_LEN                               (1U)
#define PDS_XTAL_CNT_32K_DONE_MSK                               (((1U<<PDS_XTAL_CNT_32K_DONE_LEN)-1)<<PDS_XTAL_CNT_32K_DONE_POS)
#define PDS_XTAL_CNT_32K_DONE_UMSK                              (~(((1U<<PDS_XTAL_CNT_32K_DONE_LEN)-1)<<PDS_XTAL_CNT_32K_DONE_POS))

/* 0x50 : PDS_KYD_CTL */
#define PDS_KYD_CTL_OFFSET                                      (0x50)
#define PDS_CR_PDS_KYD_EN                                       PDS_CR_PDS_KYD_EN
#define PDS_CR_PDS_KYD_EN_POS                                   (0U)
#define PDS_CR_PDS_KYD_EN_LEN                                   (1U)
#define PDS_CR_PDS_KYD_EN_MSK                                   (((1U<<PDS_CR_PDS_KYD_EN_LEN)-1)<<PDS_CR_PDS_KYD_EN_POS)
#define PDS_CR_PDS_KYD_EN_UMSK                                  (~(((1U<<PDS_CR_PDS_KYD_EN_LEN)-1)<<PDS_CR_PDS_KYD_EN_POS))
#define PDS_CR_PDS_KYD_COL_O_VAL                                PDS_CR_PDS_KYD_COL_O_VAL
#define PDS_CR_PDS_KYD_COL_O_VAL_POS                            (1U)
#define PDS_CR_PDS_KYD_COL_O_VAL_LEN                            (1U)
#define PDS_CR_PDS_KYD_COL_O_VAL_MSK                            (((1U<<PDS_CR_PDS_KYD_COL_O_VAL_LEN)-1)<<PDS_CR_PDS_KYD_COL_O_VAL_POS)
#define PDS_CR_PDS_KYD_COL_O_VAL_UMSK                           (~(((1U<<PDS_CR_PDS_KYD_COL_O_VAL_LEN)-1)<<PDS_CR_PDS_KYD_COL_O_VAL_POS))
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE                          PDS_CR_PDS_KYD_ROW_MATRIX_SIZE
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS                      (2U)
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_LEN                      (3U)
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_MSK                      (((1U<<PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_LEN)-1)<<PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS)
#define PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_UMSK                     (~(((1U<<PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_LEN)-1)<<PDS_CR_PDS_KYD_ROW_MATRIX_SIZE_POS))
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE                          PDS_CR_PDS_KYD_COL_MATRIX_SIZE
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS                      (5U)
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_LEN                      (3U)
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_MSK                      (((1U<<PDS_CR_PDS_KYD_COL_MATRIX_SIZE_LEN)-1)<<PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS)
#define PDS_CR_PDS_KYD_COL_MATRIX_SIZE_UMSK                     (~(((1U<<PDS_CR_PDS_KYD_COL_MATRIX_SIZE_LEN)-1)<<PDS_CR_PDS_KYD_COL_MATRIX_SIZE_POS))
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN                            PDS_CR_PDS_KYD_ROW_I_PULL_EN
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS                        (8U)
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_LEN                        (1U)
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_MSK                        (((1U<<PDS_CR_PDS_KYD_ROW_I_PULL_EN_LEN)-1)<<PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS)
#define PDS_CR_PDS_KYD_ROW_I_PULL_EN_UMSK                       (~(((1U<<PDS_CR_PDS_KYD_ROW_I_PULL_EN_LEN)-1)<<PDS_CR_PDS_KYD_ROW_I_PULL_EN_POS))
#define PDS_RO_PDS_KYD_WAKEUP                                   PDS_RO_PDS_KYD_WAKEUP
#define PDS_RO_PDS_KYD_WAKEUP_POS                               (9U)
#define PDS_RO_PDS_KYD_WAKEUP_LEN                               (1U)
#define PDS_RO_PDS_KYD_WAKEUP_MSK                               (((1U<<PDS_RO_PDS_KYD_WAKEUP_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_POS)
#define PDS_RO_PDS_KYD_WAKEUP_UMSK                              (~(((1U<<PDS_RO_PDS_KYD_WAKEUP_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_POS))
#define PDS_CLR_PDS_KYD_WAKEUP                                  PDS_CLR_PDS_KYD_WAKEUP
#define PDS_CLR_PDS_KYD_WAKEUP_POS                              (10U)
#define PDS_CLR_PDS_KYD_WAKEUP_LEN                              (1U)
#define PDS_CLR_PDS_KYD_WAKEUP_MSK                              (((1U<<PDS_CLR_PDS_KYD_WAKEUP_LEN)-1)<<PDS_CLR_PDS_KYD_WAKEUP_POS)
#define PDS_CLR_PDS_KYD_WAKEUP_UMSK                             (~(((1U<<PDS_CLR_PDS_KYD_WAKEUP_LEN)-1)<<PDS_CLR_PDS_KYD_WAKEUP_POS))
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO                          PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS                      (16U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_LEN                      (5U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_MSK                      (((1U<<PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_UMSK                     (~(((1U<<PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_ROW_GPIO_POS))
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX                         PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS                     (21U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_LEN                     (3U)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_MSK                     (((1U<<PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS)
#define PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_UMSK                    (~(((1U<<PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_ROW_INDEX_POS))
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO                          PDS_RO_PDS_KYD_WAKEUP_COL_GPIO
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS                      (24U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_LEN                      (5U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_MSK                      (((1U<<PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS)
#define PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_UMSK                     (~(((1U<<PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_COL_GPIO_POS))
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX                         PDS_RO_PDS_KYD_WAKEUP_COL_INDEX
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS                     (29U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_LEN                     (3U)
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_MSK                     (((1U<<PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS)
#define PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_UMSK                    (~(((1U<<PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_LEN)-1)<<PDS_RO_PDS_KYD_WAKEUP_COL_INDEX_POS))

/* 0x54 : PDS_KYD_WHITE_SET */
#define PDS_KYD_WHITE_SET_OFFSET                                (0x54)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX                            PDS_CFG_WHITE_KEY0_ROW_INDEX
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_POS                        (0U)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY0_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY0_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY0_ROW_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY0_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY0_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY0_COL_INDEX                            PDS_CFG_WHITE_KEY0_COL_INDEX
#define PDS_CFG_WHITE_KEY0_COL_INDEX_POS                        (3U)
#define PDS_CFG_WHITE_KEY0_COL_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY0_COL_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY0_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY0_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY0_COL_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY0_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY0_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY0_KYD_MODE                             PDS_CFG_WHITE_KEY0_KYD_MODE
#define PDS_CFG_WHITE_KEY0_KYD_MODE_POS                         (6U)
#define PDS_CFG_WHITE_KEY0_KYD_MODE_LEN                         (2U)
#define PDS_CFG_WHITE_KEY0_KYD_MODE_MSK                         (((1U<<PDS_CFG_WHITE_KEY0_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY0_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY0_KYD_MODE_UMSK                        (~(((1U<<PDS_CFG_WHITE_KEY0_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY0_KYD_MODE_POS))
#define PDS_CFG_WHITE_KEY1_ROW_INDEX                            PDS_CFG_WHITE_KEY1_ROW_INDEX
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_POS                        (8U)
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY1_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY1_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY1_ROW_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY1_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY1_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY1_COL_INDEX                            PDS_CFG_WHITE_KEY1_COL_INDEX
#define PDS_CFG_WHITE_KEY1_COL_INDEX_POS                        (11U)
#define PDS_CFG_WHITE_KEY1_COL_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY1_COL_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY1_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY1_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY1_COL_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY1_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY1_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY1_KYD_MODE                             PDS_CFG_WHITE_KEY1_KYD_MODE
#define PDS_CFG_WHITE_KEY1_KYD_MODE_POS                         (14U)
#define PDS_CFG_WHITE_KEY1_KYD_MODE_LEN                         (2U)
#define PDS_CFG_WHITE_KEY1_KYD_MODE_MSK                         (((1U<<PDS_CFG_WHITE_KEY1_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY1_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY1_KYD_MODE_UMSK                        (~(((1U<<PDS_CFG_WHITE_KEY1_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY1_KYD_MODE_POS))
#define PDS_CFG_WHITE_KEY2_ROW_INDEX                            PDS_CFG_WHITE_KEY2_ROW_INDEX
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_POS                        (16U)
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY2_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY2_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY2_ROW_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY2_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY2_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY2_COL_INDEX                            PDS_CFG_WHITE_KEY2_COL_INDEX
#define PDS_CFG_WHITE_KEY2_COL_INDEX_POS                        (19U)
#define PDS_CFG_WHITE_KEY2_COL_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY2_COL_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY2_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY2_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY2_COL_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY2_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY2_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY2_KYD_MODE                             PDS_CFG_WHITE_KEY2_KYD_MODE
#define PDS_CFG_WHITE_KEY2_KYD_MODE_POS                         (22U)
#define PDS_CFG_WHITE_KEY2_KYD_MODE_LEN                         (2U)
#define PDS_CFG_WHITE_KEY2_KYD_MODE_MSK                         (((1U<<PDS_CFG_WHITE_KEY2_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY2_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY2_KYD_MODE_UMSK                        (~(((1U<<PDS_CFG_WHITE_KEY2_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY2_KYD_MODE_POS))
#define PDS_CFG_WHITE_KEY3_ROW_INDEX                            PDS_CFG_WHITE_KEY3_ROW_INDEX
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_POS                        (24U)
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY3_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY3_ROW_INDEX_POS)
#define PDS_CFG_WHITE_KEY3_ROW_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY3_ROW_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY3_ROW_INDEX_POS))
#define PDS_CFG_WHITE_KEY3_COL_INDEX                            PDS_CFG_WHITE_KEY3_COL_INDEX
#define PDS_CFG_WHITE_KEY3_COL_INDEX_POS                        (27U)
#define PDS_CFG_WHITE_KEY3_COL_INDEX_LEN                        (3U)
#define PDS_CFG_WHITE_KEY3_COL_INDEX_MSK                        (((1U<<PDS_CFG_WHITE_KEY3_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY3_COL_INDEX_POS)
#define PDS_CFG_WHITE_KEY3_COL_INDEX_UMSK                       (~(((1U<<PDS_CFG_WHITE_KEY3_COL_INDEX_LEN)-1)<<PDS_CFG_WHITE_KEY3_COL_INDEX_POS))
#define PDS_CFG_WHITE_KEY3_KYD_MODE                             PDS_CFG_WHITE_KEY3_KYD_MODE
#define PDS_CFG_WHITE_KEY3_KYD_MODE_POS                         (30U)
#define PDS_CFG_WHITE_KEY3_KYD_MODE_LEN                         (2U)
#define PDS_CFG_WHITE_KEY3_KYD_MODE_MSK                         (((1U<<PDS_CFG_WHITE_KEY3_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY3_KYD_MODE_POS)
#define PDS_CFG_WHITE_KEY3_KYD_MODE_UMSK                        (~(((1U<<PDS_CFG_WHITE_KEY3_KYD_MODE_LEN)-1)<<PDS_CFG_WHITE_KEY3_KYD_MODE_POS))

/* 0x60 : KYS_ROW_I_GPIO_SEL0 */
#define PDS_KYS_ROW_I_GPIO_SEL0_OFFSET                          (0x60)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL                            PDS_CFG_KEY_ROW_I_0_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_POS                        (0U)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_0_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_0_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_0_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_0_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_0_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL                            PDS_CFG_KEY_ROW_I_1_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_POS                        (8U)
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_1_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_1_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_1_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_1_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_1_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL                            PDS_CFG_KEY_ROW_I_2_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_POS                        (16U)
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_2_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_2_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_2_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_2_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_2_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL                            PDS_CFG_KEY_ROW_I_3_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_POS                        (24U)
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_3_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_3_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_3_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_3_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_3_GPIO_SEL_POS))
#define PDS_CFG_ROW_DETECT_CNT                                  PDS_CFG_ROW_DETECT_CNT
#define PDS_CFG_ROW_DETECT_CNT_POS                              (29U)
#define PDS_CFG_ROW_DETECT_CNT_LEN                              (3U)
#define PDS_CFG_ROW_DETECT_CNT_MSK                              (((1U<<PDS_CFG_ROW_DETECT_CNT_LEN)-1)<<PDS_CFG_ROW_DETECT_CNT_POS)
#define PDS_CFG_ROW_DETECT_CNT_UMSK                             (~(((1U<<PDS_CFG_ROW_DETECT_CNT_LEN)-1)<<PDS_CFG_ROW_DETECT_CNT_POS))

/* 0x64 : KYS_ROW_I_GPIO_SEL1 */
#define PDS_KYS_ROW_I_GPIO_SEL1_OFFSET                          (0x64)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL                            PDS_CFG_KEY_ROW_I_4_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_POS                        (0U)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_4_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_4_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_4_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_4_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_4_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL                            PDS_CFG_KEY_ROW_I_5_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_POS                        (8U)
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_5_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_5_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_5_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_5_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_5_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL                            PDS_CFG_KEY_ROW_I_6_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_POS                        (16U)
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_6_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_6_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_6_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_6_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_6_GPIO_SEL_POS))
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL                            PDS_CFG_KEY_ROW_I_7_GPIO_SEL
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_POS                        (24U)
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_ROW_I_7_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_7_GPIO_SEL_POS)
#define PDS_CFG_KEY_ROW_I_7_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_ROW_I_7_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_ROW_I_7_GPIO_SEL_POS))

/* 0x68 : KYS_COL_O_GPIO_SEL0 */
#define PDS_KYS_COL_O_GPIO_SEL0_OFFSET                          (0x68)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL                            PDS_CFG_KEY_COL_O_0_GPIO_SEL
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_POS                        (0U)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_0_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_0_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_0_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_0_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_0_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL                            PDS_CFG_KEY_COL_O_1_GPIO_SEL
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_POS                        (8U)
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_1_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_1_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_1_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_1_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_1_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL                            PDS_CFG_KEY_COL_O_2_GPIO_SEL
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_POS                        (16U)
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_2_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_2_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_2_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_2_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_2_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL                            PDS_CFG_KEY_COL_O_3_GPIO_SEL
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_POS                        (24U)
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_3_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_3_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_3_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_3_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_3_GPIO_SEL_POS))

/* 0x6C : KYS_COL_O_GPIO_SEL1 */
#define PDS_KYS_COL_O_GPIO_SEL1_OFFSET                          (0x6C)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL                            PDS_CFG_KEY_COL_O_4_GPIO_SEL
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_POS                        (0U)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_4_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_4_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_4_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_4_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_4_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL                            PDS_CFG_KEY_COL_O_5_GPIO_SEL
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_POS                        (8U)
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_5_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_5_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_5_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_5_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_5_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL                            PDS_CFG_KEY_COL_O_6_GPIO_SEL
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_POS                        (16U)
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_6_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_6_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_6_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_6_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_6_GPIO_SEL_POS))
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL                            PDS_CFG_KEY_COL_O_7_GPIO_SEL
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_POS                        (24U)
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_LEN                        (5U)
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_MSK                        (((1U<<PDS_CFG_KEY_COL_O_7_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_7_GPIO_SEL_POS)
#define PDS_CFG_KEY_COL_O_7_GPIO_SEL_UMSK                       (~(((1U<<PDS_CFG_KEY_COL_O_7_GPIO_SEL_LEN)-1)<<PDS_CFG_KEY_COL_O_7_GPIO_SEL_POS))


struct  pds_reg {
    /* 0x0 : PDS_CTL */
    union {
        struct {
            uint32_t pds_start_ps                   :  1; /* [    0],        w1p,        0x0 */
            uint32_t cr_sleep_forever               :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t cr_pds_wait_rc32m_rdy          :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_pd_avdd14               :  1; /* [    4],        r/w,        0x0 */
            uint32_t cr_pds_pd_bg_sys               :  1; /* [    5],        r/w,        0x0 */
            uint32_t cr_pds_glb_reg_reset_protect   :  1; /* [    6],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_pu_flash           :  1; /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_gate_clk                :  1; /* [    8],        r/w,        0x1 */
            uint32_t cr_pds_mem_stby                :  1; /* [    9],        r/w,        0x1 */
            uint32_t cr_sw_pu_flash                 :  1; /* [   10],        r/w,        0x1 */
            uint32_t cr_pds_iso_en                  :  1; /* [   11],        r/w,        0x1 */
            uint32_t cr_pds_wait_xtal_rdy           :  1; /* [   12],        r/w,        0x0 */
            uint32_t cr_pds_pwr_off                 :  1; /* [   13],        r/w,        0x1 */
            uint32_t cr_pds_pd_xtal                 :  1; /* [   14],        r/w,        0x1 */
            uint32_t cr_pds_ctrl_soc_enb            :  1; /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_rst_soc_en              :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_rc32m_off_dis           :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_pds_ldo_vsel_en             :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t cr_pds_xtal_cnt_rc32k_en       :  1; /* [   20],        r/w,        0x0 */
            uint32_t cr_np_wfi_mask                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t cr_pds_pd_ldo11                :  1; /* [   22],        r/w,        0x0 */
            uint32_t cr_pds_ctrl_rf                 :  1; /* [   23],        r/w,        0x0 */
            uint32_t cfg_xtal_deg_en                :  1; /* [   24],        r/w,        0x0 */
            uint32_t cfg_ble_wakeup_req_after_pds   :  1; /* [   25],        r/w,        0x0 */
            uint32_t cr_sw_gpio_iso_mode            :  1; /* [   26],        r/w,        0x0 */
            uint32_t cr_turn_off_pds_clk            :  1; /* [   27],        r/w,        0x1 */
            uint32_t cr_pds_gpio_keep_en            :  3; /* [30:28],        r/w,        0x7 */
            uint32_t cr_pds_gpio_iso_mode           :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_CTL;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[8];

    /* 0xC : PDS_INT */
    union {
        struct {
            uint32_t ro_pds_wake_int                :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t ro_pds_rf_done_int             :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t pds_reset_event                :  3; /* [ 6: 4],          r,        0x0 */
            uint32_t pds_clr_reset_event            :  1; /* [    7],          w,        0x0 */
            uint32_t cr_pds_wake_int_mask           :  1; /* [    8],        r/w,        0x0 */
            uint32_t pds_pin_wakeup_option          :  1; /* [    9],        r/w,        0x0 */
            uint32_t cr_pds_rf_done_int_mask        :  1; /* [   10],        r/w,        0x0 */
            uint32_t pds_timeout_event              :  3; /* [13:11],          r,        0x0 */
            uint32_t en_pds_timeout_event           :  1; /* [   14],        r/w,        0x0 */
            uint32_t cr_pds_int_clr                 :  1; /* [   15],        w1p,        0x0 */
            uint32_t reserved_16                    :  1; /* [   16],       rsvd,        0x0 */
            uint32_t cr_pds_wakeup_src_en           :  7; /* [23:17],        r/w,       0x7f */
            uint32_t reserved_24                    :  1; /* [   24],       rsvd,        0x0 */
            uint32_t ro_pds_wakeup_event            :  7; /* [31:25],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_INT;

    /* 0x10 : PDS_CTL2 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_pds_force_np_pds_rst        :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cr_pds_force_bz_pds_rst        :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t cr_pds_force_np_gate_clk       :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t cr_pds_force_bz_gate_clk       :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_CTL2;

    /* 0x14 : PDS_CTL3 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_pwr_off      :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_6                   :  5; /* [ 6: 2],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_pds_rst      :  1; /* [    7],        r/w,        0x0 */
            uint32_t reserved_8_12                  :  5; /* [12: 8],       rsvd,        0x0 */
            uint32_t cr_pds_force_misc_gate_clk     :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_29                 : 16; /* [29:14],       rsvd,        0x0 */
            uint32_t cr_pds_misc_iso_en             :  1; /* [   30],        r/w,        0x1 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_CTL3;

    /* 0x18 : PDS_CTL4 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t cr_pds_np_reset                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t cr_pds_np_gate_clk             :  1; /* [    3],        r/w,        0x1 */
            uint32_t reserved_4_12                  :  9; /* [12: 4],       rsvd,        0x0 */
            uint32_t cr_pds_bz_reset                :  1; /* [   13],        r/w,        0x1 */
            uint32_t reserved_14                    :  1; /* [   14],       rsvd,        0x0 */
            uint32_t cr_pds_bz_gate_clk             :  1; /* [   15],        r/w,        0x1 */
            uint32_t reserved_16_23                 :  8; /* [23:16],       rsvd,        0x0 */
            uint32_t cr_pds_misc_pwr_off            :  1; /* [   24],        r/w,        0x1 */
            uint32_t cr_pds_misc_reset              :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26                    :  1; /* [   26],       rsvd,        0x0 */
            uint32_t cr_pds_misc_gate_clk           :  1; /* [   27],        r/w,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_CTL4;

    /* 0x1C : pds_stat */
    union {
        struct {
            uint32_t ro_pds_state                   :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ro_pds_rf_state                :  3; /* [10: 8],          r,        0x0 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_stat;

    /* 0x20 : pds_ram1 */
    union {
        struct {
            uint32_t cr_pds_ram_ret1n               :  5; /* [ 4: 0],        r/w,       0x1f */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cr_pds_ram_ret2n               :  5; /* [12: 8],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cr_pds_ram_pgen                :  5; /* [20:16],        r/w,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_ram1;

    /* 0x24  reserved */
    uint8_t RESERVED0x24[12];

    /* 0x30 : pds_gpio_pd_set */
    union {
        struct {
            uint32_t cr_pds_gpio0_pd                :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_gpio1_pd                :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_pds_gpio2_pd                :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_gpio3_pd                :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_gpio4_pd                :  1; /* [    4],          r,        0x0 */
            uint32_t cr_pds_gpio5_pd                :  1; /* [    5],          r,        0x0 */
            uint32_t cr_pds_gpio6_pd                :  1; /* [    6],          r,        0x0 */
            uint32_t cr_pds_gpio7_pd                :  1; /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_gpio8_pd                :  1; /* [    8],        r/w,        0x0 */
            uint32_t cr_pds_gpio9_pd                :  1; /* [    9],          r,        0x0 */
            uint32_t cr_pds_gpio10_pd               :  1; /* [   10],          r,        0x0 */
            uint32_t cr_pds_gpio11_pd               :  1; /* [   11],          r,        0x0 */
            uint32_t cr_pds_gpio12_pd               :  1; /* [   12],          r,        0x0 */
            uint32_t cr_pds_gpio13_pd               :  1; /* [   13],          r,        0x0 */
            uint32_t cr_pds_gpio14_pd               :  1; /* [   14],        r/w,        0x0 */
            uint32_t cr_pds_gpio15_pd               :  1; /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_gpio16_pd               :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_gpio17_pd               :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_pds_gpio18_pd               :  1; /* [   18],        r/w,        0x0 */
            uint32_t cr_pds_gpio19_pd               :  1; /* [   19],        r/w,        0x0 */
            uint32_t cr_pds_gpio20_pd               :  1; /* [   20],        r/w,        0x0 */
            uint32_t cr_pds_gpio21_pd               :  1; /* [   21],        r/w,        0x0 */
            uint32_t cr_pds_gpio22_pd               :  1; /* [   22],        r/w,        0x0 */
            uint32_t cr_pds_gpio23_pd               :  1; /* [   23],        r/w,        0x0 */
            uint32_t cr_pds_gpio24_pd               :  1; /* [   24],        r/w,        0x0 */
            uint32_t cr_pds_gpio25_pd               :  1; /* [   25],        r/w,        0x0 */
            uint32_t cr_pds_gpio26_pd               :  1; /* [   26],        r/w,        0x0 */
            uint32_t cr_pds_gpio27_pd               :  1; /* [   27],        r/w,        0x0 */
            uint32_t cr_pds_gpio28_pd               :  1; /* [   28],        r/w,        0x0 */
            uint32_t cr_pds_gpio29_pd               :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_pds_gpio30_pd               :  1; /* [   30],          r,        0x0 */
            uint32_t cr_pds_gpio31_pd               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_gpio_pd_set;

    /* 0x34 : pds_gpio_pu_set */
    union {
        struct {
            uint32_t cr_pds_gpio0_pu                :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_gpio1_pu                :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_pds_gpio2_pu                :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_gpio3_pu                :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_gpio4_pu                :  1; /* [    4],          r,        0x0 */
            uint32_t cr_pds_gpio5_pu                :  1; /* [    5],          r,        0x0 */
            uint32_t cr_pds_gpio6_pu                :  1; /* [    6],          r,        0x0 */
            uint32_t cr_pds_gpio7_pu                :  1; /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_gpio8_pu                :  1; /* [    8],        r/w,        0x0 */
            uint32_t cr_pds_gpio9_pu                :  1; /* [    9],          r,        0x0 */
            uint32_t cr_pds_gpio10_pu               :  1; /* [   10],          r,        0x0 */
            uint32_t cr_pds_gpio11_pu               :  1; /* [   11],          r,        0x0 */
            uint32_t cr_pds_gpio12_pu               :  1; /* [   12],          r,        0x0 */
            uint32_t cr_pds_gpio13_pu               :  1; /* [   13],          r,        0x0 */
            uint32_t cr_pds_gpio14_pu               :  1; /* [   14],        r/w,        0x0 */
            uint32_t cr_pds_gpio15_pu               :  1; /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_gpio16_pu               :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_gpio17_pu               :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_pds_gpio18_pu               :  1; /* [   18],        r/w,        0x0 */
            uint32_t cr_pds_gpio19_pu               :  1; /* [   19],        r/w,        0x0 */
            uint32_t cr_pds_gpio20_pu               :  1; /* [   20],        r/w,        0x0 */
            uint32_t cr_pds_gpio21_pu               :  1; /* [   21],        r/w,        0x0 */
            uint32_t cr_pds_gpio22_pu               :  1; /* [   22],        r/w,        0x0 */
            uint32_t cr_pds_gpio23_pu               :  1; /* [   23],        r/w,        0x0 */
            uint32_t cr_pds_gpio24_pu               :  1; /* [   24],        r/w,        0x0 */
            uint32_t cr_pds_gpio25_pu               :  1; /* [   25],        r/w,        0x0 */
            uint32_t cr_pds_gpio26_pu               :  1; /* [   26],        r/w,        0x0 */
            uint32_t cr_pds_gpio27_pu               :  1; /* [   27],        r/w,        0x0 */
            uint32_t cr_pds_gpio28_pu               :  1; /* [   28],        r/w,        0x0 */
            uint32_t cr_pds_gpio29_pu               :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_pds_gpio30_pu               :  1; /* [   30],          r,        0x0 */
            uint32_t cr_pds_gpio31_pu               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_gpio_pu_set;

    /* 0x38 : pds_gpio_ie_set */
    union {
        struct {
            uint32_t cr_pds_gpio0_ie                :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_gpio1_ie                :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_pds_gpio2_ie                :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_gpio3_ie                :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_gpio4_ie                :  1; /* [    4],          r,        0x0 */
            uint32_t cr_pds_gpio5_ie                :  1; /* [    5],          r,        0x0 */
            uint32_t cr_pds_gpio6_ie                :  1; /* [    6],          r,        0x0 */
            uint32_t cr_pds_gpio7_ie                :  1; /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_gpio8_ie                :  1; /* [    8],        r/w,        0x0 */
            uint32_t cr_pds_gpio9_ie                :  1; /* [    9],          r,        0x0 */
            uint32_t cr_pds_gpio10_ie               :  1; /* [   10],          r,        0x0 */
            uint32_t cr_pds_gpio11_ie               :  1; /* [   11],          r,        0x0 */
            uint32_t cr_pds_gpio12_ie               :  1; /* [   12],          r,        0x0 */
            uint32_t cr_pds_gpio13_ie               :  1; /* [   13],          r,        0x0 */
            uint32_t cr_pds_gpio14_ie               :  1; /* [   14],        r/w,        0x0 */
            uint32_t cr_pds_gpio15_ie               :  1; /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_gpio16_ie               :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_gpio17_ie               :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_pds_gpio18_ie               :  1; /* [   18],        r/w,        0x0 */
            uint32_t cr_pds_gpio19_ie               :  1; /* [   19],        r/w,        0x0 */
            uint32_t cr_pds_gpio20_ie               :  1; /* [   20],        r/w,        0x0 */
            uint32_t cr_pds_gpio21_ie               :  1; /* [   21],        r/w,        0x0 */
            uint32_t cr_pds_gpio22_ie               :  1; /* [   22],        r/w,        0x0 */
            uint32_t cr_pds_gpio23_ie               :  1; /* [   23],        r/w,        0x0 */
            uint32_t cr_pds_gpio24_ie               :  1; /* [   24],        r/w,        0x0 */
            uint32_t cr_pds_gpio25_ie               :  1; /* [   25],        r/w,        0x0 */
            uint32_t cr_pds_gpio26_ie               :  1; /* [   26],        r/w,        0x0 */
            uint32_t cr_pds_gpio27_ie               :  1; /* [   27],        r/w,        0x0 */
            uint32_t cr_pds_gpio28_ie               :  1; /* [   28],        r/w,        0x0 */
            uint32_t cr_pds_gpio29_ie               :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_pds_gpio30_ie               :  1; /* [   30],          r,        0x0 */
            uint32_t cr_pds_gpio31_ie               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_gpio_ie_set;

    /* 0x3C : pds_gpio_oe_set */
    union {
        struct {
            uint32_t cr_pds_gpio0_oe                :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_gpio1_oe                :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_pds_gpio2_oe                :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_pds_gpio3_oe                :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_pds_gpio4_oe                :  1; /* [    4],          r,        0x0 */
            uint32_t cr_pds_gpio5_oe                :  1; /* [    5],          r,        0x0 */
            uint32_t cr_pds_gpio6_oe                :  1; /* [    6],          r,        0x0 */
            uint32_t cr_pds_gpio7_oe                :  1; /* [    7],        r/w,        0x0 */
            uint32_t cr_pds_gpio8_oe                :  1; /* [    8],        r/w,        0x0 */
            uint32_t cr_pds_gpio9_oe                :  1; /* [    9],          r,        0x0 */
            uint32_t cr_pds_gpio10_oe               :  1; /* [   10],          r,        0x0 */
            uint32_t cr_pds_gpio11_oe               :  1; /* [   11],          r,        0x0 */
            uint32_t cr_pds_gpio12_oe               :  1; /* [   12],          r,        0x0 */
            uint32_t cr_pds_gpio13_oe               :  1; /* [   13],          r,        0x0 */
            uint32_t cr_pds_gpio14_oe               :  1; /* [   14],        r/w,        0x0 */
            uint32_t cr_pds_gpio15_oe               :  1; /* [   15],        r/w,        0x0 */
            uint32_t cr_pds_gpio16_oe               :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_pds_gpio17_oe               :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_pds_gpio18_oe               :  1; /* [   18],        r/w,        0x0 */
            uint32_t cr_pds_gpio19_oe               :  1; /* [   19],        r/w,        0x0 */
            uint32_t cr_pds_gpio20_oe               :  1; /* [   20],        r/w,        0x0 */
            uint32_t cr_pds_gpio21_oe               :  1; /* [   21],        r/w,        0x0 */
            uint32_t cr_pds_gpio22_oe               :  1; /* [   22],        r/w,        0x0 */
            uint32_t cr_pds_gpio23_oe               :  1; /* [   23],        r/w,        0x0 */
            uint32_t cr_pds_gpio24_oe               :  1; /* [   24],        r/w,        0x0 */
            uint32_t cr_pds_gpio25_oe               :  1; /* [   25],        r/w,        0x0 */
            uint32_t cr_pds_gpio26_oe               :  1; /* [   26],        r/w,        0x0 */
            uint32_t cr_pds_gpio27_oe               :  1; /* [   27],        r/w,        0x0 */
            uint32_t cr_pds_gpio28_oe               :  1; /* [   28],        r/w,        0x0 */
            uint32_t cr_pds_gpio29_oe               :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_pds_gpio30_oe               :  1; /* [   30],          r,        0x0 */
            uint32_t cr_pds_gpio31_oe               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_gpio_oe_set;

    /* 0x40 : pds_gpio_int_set */
    union {
        struct {
            uint32_t pds_gpio_set0_int_mode         :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t pds_gpio_set1_int_mode         :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t pds_gpio_set2_int_mode         :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t pds_gpio_set3_int_mode         :  4; /* [15:12],        r/w,        0x0 */
            uint32_t pds_gpio_set4_int_mode         :  4; /* [19:16],        r/w,        0x0 */
            uint32_t pds_gpio_set5_int_mode         :  4; /* [23:20],        r/w,        0x0 */
            uint32_t pds_gpio_set6_int_mode         :  4; /* [27:24],        r/w,        0x0 */
            uint32_t pds_gpio_set7_int_mode         :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_gpio_int_set;

    /* 0x44 : pds_gpio_stat */
    union {
        struct {
            uint32_t pds_gpio_int_stat              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } pds_gpio_stat;

    /* 0x48 : cfg_pds_key_scan */
    union {
        struct {
            uint32_t cr_pds_gpio_set_o_val          :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t cr_pds_gpio_set_int_mask       :  8; /* [15: 8],        r/w,       0xff */
            uint32_t reserved_16_23                 :  8; /* [23:16],       rsvd,        0x0 */
            uint32_t cr_pds_gpio0_set_int_clr       :  1; /* [   24],        w1p,        0x0 */
            uint32_t cr_pds_gpio1_set_int_clr       :  1; /* [   25],        w1p,        0x0 */
            uint32_t cr_pds_gpio2_set_int_clr       :  1; /* [   26],        w1p,        0x0 */
            uint32_t cr_pds_gpio3_set_int_clr       :  1; /* [   27],        w1p,        0x0 */
            uint32_t cr_pds_gpio4_set_int_clr       :  1; /* [   28],        w1p,        0x0 */
            uint32_t cr_pds_gpio5_set_int_clr       :  1; /* [   29],        w1p,        0x0 */
            uint32_t cr_pds_gpio6_set_int_clr       :  1; /* [   30],        w1p,        0x0 */
            uint32_t cr_pds_gpio7_set_int_clr       :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } cfg_pds_key_scan;

    /* 0x4C : XTAL_CNT_32K */
    union {
        struct {
            uint32_t ro_xtal_cnt_32k_res            :  6; /* [ 5: 0],          r,       0x24 */
            uint32_t ro_xtal_cnt_32k_cnt            : 11; /* [16: 6],          r,      0x3d0 */
            uint32_t reg_total_32k_cycle            :  3; /* [19:17],        r/w,        0x0 */
            uint32_t reserved_20_28                 :  9; /* [28:20],       rsvd,        0x0 */
            uint32_t xtal_cnt_32k_process           :  1; /* [   29],          r,        0x0 */
            uint32_t xtal_cnt_32k_done              :  1; /* [   30],          r,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } XTAL_CNT_32K;

    /* 0x50 : PDS_KYD_CTL */
    union {
        struct {
            uint32_t cr_pds_kyd_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_pds_kyd_col_o_val           :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_pds_kyd_row_matrix_size     :  3; /* [ 4: 2],        r/w,        0x7 */
            uint32_t cr_pds_kyd_col_matrix_size     :  3; /* [ 7: 5],        r/w,        0x7 */
            uint32_t cr_pds_kyd_row_i_pull_en       :  1; /* [    8],        r/w,        0x0 */
            uint32_t ro_pds_kyd_wakeup              :  1; /* [    9],          r,        0x0 */
            uint32_t clr_pds_kyd_wakeup             :  1; /* [   10],        w1p,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t ro_pds_kyd_wakeup_row_gpio     :  5; /* [20:16],          r,        0x0 */
            uint32_t ro_pds_kyd_wakeup_row_index    :  3; /* [23:21],          r,        0x0 */
            uint32_t ro_pds_kyd_wakeup_col_gpio     :  5; /* [28:24],          r,        0x0 */
            uint32_t ro_pds_kyd_wakeup_col_index    :  3; /* [31:29],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_KYD_CTL;

    /* 0x54 : PDS_KYD_WHITE_SET */
    union {
        struct {
            uint32_t cfg_white_key0_row_index       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t cfg_white_key0_col_index       :  3; /* [ 5: 3],        r/w,        0x0 */
            uint32_t cfg_white_key0_kyd_mode        :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t cfg_white_key1_row_index       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t cfg_white_key1_col_index       :  3; /* [13:11],        r/w,        0x0 */
            uint32_t cfg_white_key1_kyd_mode        :  2; /* [15:14],        r/w,        0x0 */
            uint32_t cfg_white_key2_row_index       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t cfg_white_key2_col_index       :  3; /* [21:19],        r/w,        0x0 */
            uint32_t cfg_white_key2_kyd_mode        :  2; /* [23:22],        r/w,        0x0 */
            uint32_t cfg_white_key3_row_index       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t cfg_white_key3_col_index       :  3; /* [29:27],        r/w,        0x0 */
            uint32_t cfg_white_key3_kyd_mode        :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PDS_KYD_WHITE_SET;

    /* 0x58  reserved */
    uint8_t RESERVED0x58[8];

    /* 0x60 : KYS_ROW_I_GPIO_SEL0 */
    union {
        struct {
            uint32_t cfg_key_row_i_0_gpio_sel       :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_1_gpio_sel       :  5; /* [12: 8],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_2_gpio_sel       :  5; /* [20:16],        r/w,        0x2 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_3_gpio_sel       :  5; /* [28:24],        r/w,        0x3 */
            uint32_t cfg_row_detect_cnt             :  3; /* [31:29],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } KYS_ROW_I_GPIO_SEL0;

    /* 0x64 : KYS_ROW_I_GPIO_SEL1 */
    union {
        struct {
            uint32_t cfg_key_row_i_4_gpio_sel       :  5; /* [ 4: 0],        r/w,        0x4 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_5_gpio_sel       :  5; /* [12: 8],        r/w,        0x5 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_6_gpio_sel       :  5; /* [20:16],        r/w,        0x6 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t cfg_key_row_i_7_gpio_sel       :  5; /* [28:24],        r/w,        0x7 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } KYS_ROW_I_GPIO_SEL1;

    /* 0x68 : KYS_COL_O_GPIO_SEL0 */
    union {
        struct {
            uint32_t cfg_key_col_o_0_gpio_sel       :  5; /* [ 4: 0],        r/w,        0x8 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_1_gpio_sel       :  5; /* [12: 8],        r/w,        0x9 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_2_gpio_sel       :  5; /* [20:16],        r/w,        0xa */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_3_gpio_sel       :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } KYS_COL_O_GPIO_SEL0;

    /* 0x6C : KYS_COL_O_GPIO_SEL1 */
    union {
        struct {
            uint32_t cfg_key_col_o_4_gpio_sel       :  5; /* [ 4: 0],        r/w,        0xc */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_5_gpio_sel       :  5; /* [12: 8],        r/w,        0xd */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_6_gpio_sel       :  5; /* [20:16],        r/w,        0xe */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t cfg_key_col_o_7_gpio_sel       :  5; /* [28:24],        r/w,        0xf */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } KYS_COL_O_GPIO_SEL1;

};

typedef volatile struct pds_reg pds_reg_t;


#endif  /* __PDS_REG_H__ */
