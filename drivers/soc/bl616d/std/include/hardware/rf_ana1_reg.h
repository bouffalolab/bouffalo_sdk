/**
  ******************************************************************************
  * @file    rf_ana1_reg.h
  * @version V1.0
  * @date    2024-06-03
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
#ifndef  __RF_ANA1_REG_H__
#define  __RF_ANA1_REG_H__

#include "bl616d.h"

/* 0x0 : Silicon revision */
#define RF_ANA1_RF_REV_OFFSET                                   (0x0)
#define RF_ANA1_RF_ID                                           RF_ANA1_RF_ID
#define RF_ANA1_RF_ID_POS                                       (0U)
#define RF_ANA1_RF_ID_LEN                                       (8U)
#define RF_ANA1_RF_ID_MSK                                       (((1U<<RF_ANA1_RF_ID_LEN)-1)<<RF_ANA1_RF_ID_POS)
#define RF_ANA1_RF_ID_UMSK                                      (~(((1U<<RF_ANA1_RF_ID_LEN)-1)<<RF_ANA1_RF_ID_POS))
#define RF_ANA1_FW_REV                                          RF_ANA1_FW_REV
#define RF_ANA1_FW_REV_POS                                      (8U)
#define RF_ANA1_FW_REV_LEN                                      (8U)
#define RF_ANA1_FW_REV_MSK                                      (((1U<<RF_ANA1_FW_REV_LEN)-1)<<RF_ANA1_FW_REV_POS)
#define RF_ANA1_FW_REV_UMSK                                     (~(((1U<<RF_ANA1_FW_REV_LEN)-1)<<RF_ANA1_FW_REV_POS))
#define RF_ANA1_HW_REV                                          RF_ANA1_HW_REV
#define RF_ANA1_HW_REV_POS                                      (16U)
#define RF_ANA1_HW_REV_LEN                                      (8U)
#define RF_ANA1_HW_REV_MSK                                      (((1U<<RF_ANA1_HW_REV_LEN)-1)<<RF_ANA1_HW_REV_POS)
#define RF_ANA1_HW_REV_UMSK                                     (~(((1U<<RF_ANA1_HW_REV_LEN)-1)<<RF_ANA1_HW_REV_POS))

/* 0x4 : Digital Control */
#define RF_ANA1_RF_FSM_CTRL_HW_OFFSET                           (0x4)
#define RF_ANA1_RF_FSM0_CTRL_EN                                 RF_ANA1_RF_FSM0_CTRL_EN
#define RF_ANA1_RF_FSM0_CTRL_EN_POS                             (1U)
#define RF_ANA1_RF_FSM0_CTRL_EN_LEN                             (1U)
#define RF_ANA1_RF_FSM0_CTRL_EN_MSK                             (((1U<<RF_ANA1_RF_FSM0_CTRL_EN_LEN)-1)<<RF_ANA1_RF_FSM0_CTRL_EN_POS)
#define RF_ANA1_RF_FSM0_CTRL_EN_UMSK                            (~(((1U<<RF_ANA1_RF_FSM0_CTRL_EN_LEN)-1)<<RF_ANA1_RF_FSM0_CTRL_EN_POS))
#define RF_ANA1_RF_FSM0_T2R_CAL_MODE                            RF_ANA1_RF_FSM0_T2R_CAL_MODE
#define RF_ANA1_RF_FSM0_T2R_CAL_MODE_POS                        (2U)
#define RF_ANA1_RF_FSM0_T2R_CAL_MODE_LEN                        (2U)
#define RF_ANA1_RF_FSM0_T2R_CAL_MODE_MSK                        (((1U<<RF_ANA1_RF_FSM0_T2R_CAL_MODE_LEN)-1)<<RF_ANA1_RF_FSM0_T2R_CAL_MODE_POS)
#define RF_ANA1_RF_FSM0_T2R_CAL_MODE_UMSK                       (~(((1U<<RF_ANA1_RF_FSM0_T2R_CAL_MODE_LEN)-1)<<RF_ANA1_RF_FSM0_T2R_CAL_MODE_POS))
#define RF_ANA1_RF_FSM0_STATE                                   RF_ANA1_RF_FSM0_STATE
#define RF_ANA1_RF_FSM0_STATE_POS                               (4U)
#define RF_ANA1_RF_FSM0_STATE_LEN                               (3U)
#define RF_ANA1_RF_FSM0_STATE_MSK                               (((1U<<RF_ANA1_RF_FSM0_STATE_LEN)-1)<<RF_ANA1_RF_FSM0_STATE_POS)
#define RF_ANA1_RF_FSM0_STATE_UMSK                              (~(((1U<<RF_ANA1_RF_FSM0_STATE_LEN)-1)<<RF_ANA1_RF_FSM0_STATE_POS))
#define RF_ANA1_RF_FSM0_RC_STATE_DBG                            RF_ANA1_RF_FSM0_RC_STATE_DBG
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_POS                        (8U)
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_LEN                        (3U)
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_MSK                        (((1U<<RF_ANA1_RF_FSM0_RC_STATE_DBG_LEN)-1)<<RF_ANA1_RF_FSM0_RC_STATE_DBG_POS)
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_UMSK                       (~(((1U<<RF_ANA1_RF_FSM0_RC_STATE_DBG_LEN)-1)<<RF_ANA1_RF_FSM0_RC_STATE_DBG_POS))
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_EN                         RF_ANA1_RF_FSM0_RC_STATE_DBG_EN
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_POS                     (11U)
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_LEN                     (1U)
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_MSK                     (((1U<<RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_LEN)-1)<<RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_POS)
#define RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_UMSK                    (~(((1U<<RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_LEN)-1)<<RF_ANA1_RF_FSM0_RC_STATE_DBG_EN_POS))
#define RF_ANA1_RF_FSM0_ST_INT_SEL                              RF_ANA1_RF_FSM0_ST_INT_SEL
#define RF_ANA1_RF_FSM0_ST_INT_SEL_POS                          (12U)
#define RF_ANA1_RF_FSM0_ST_INT_SEL_LEN                          (3U)
#define RF_ANA1_RF_FSM0_ST_INT_SEL_MSK                          (((1U<<RF_ANA1_RF_FSM0_ST_INT_SEL_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_SEL_POS)
#define RF_ANA1_RF_FSM0_ST_INT_SEL_UMSK                         (~(((1U<<RF_ANA1_RF_FSM0_ST_INT_SEL_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_SEL_POS))
#define RF_ANA1_RF_FSM0_ST_INT                                  RF_ANA1_RF_FSM0_ST_INT
#define RF_ANA1_RF_FSM0_ST_INT_POS                              (16U)
#define RF_ANA1_RF_FSM0_ST_INT_LEN                              (1U)
#define RF_ANA1_RF_FSM0_ST_INT_MSK                              (((1U<<RF_ANA1_RF_FSM0_ST_INT_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_POS)
#define RF_ANA1_RF_FSM0_ST_INT_UMSK                             (~(((1U<<RF_ANA1_RF_FSM0_ST_INT_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_POS))
#define RF_ANA1_RF_FSM0_ST_INT_CLR                              RF_ANA1_RF_FSM0_ST_INT_CLR
#define RF_ANA1_RF_FSM0_ST_INT_CLR_POS                          (20U)
#define RF_ANA1_RF_FSM0_ST_INT_CLR_LEN                          (1U)
#define RF_ANA1_RF_FSM0_ST_INT_CLR_MSK                          (((1U<<RF_ANA1_RF_FSM0_ST_INT_CLR_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_CLR_POS)
#define RF_ANA1_RF_FSM0_ST_INT_CLR_UMSK                         (~(((1U<<RF_ANA1_RF_FSM0_ST_INT_CLR_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_CLR_POS))
#define RF_ANA1_RF_FSM0_ST_INT_SET                              RF_ANA1_RF_FSM0_ST_INT_SET
#define RF_ANA1_RF_FSM0_ST_INT_SET_POS                          (24U)
#define RF_ANA1_RF_FSM0_ST_INT_SET_LEN                          (1U)
#define RF_ANA1_RF_FSM0_ST_INT_SET_MSK                          (((1U<<RF_ANA1_RF_FSM0_ST_INT_SET_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_SET_POS)
#define RF_ANA1_RF_FSM0_ST_INT_SET_UMSK                         (~(((1U<<RF_ANA1_RF_FSM0_ST_INT_SET_LEN)-1)<<RF_ANA1_RF_FSM0_ST_INT_SET_POS))
#define RF_ANA1_RF_FSM0_RC_STATE_VALUE                          RF_ANA1_RF_FSM0_RC_STATE_VALUE
#define RF_ANA1_RF_FSM0_RC_STATE_VALUE_POS                      (28U)
#define RF_ANA1_RF_FSM0_RC_STATE_VALUE_LEN                      (3U)
#define RF_ANA1_RF_FSM0_RC_STATE_VALUE_MSK                      (((1U<<RF_ANA1_RF_FSM0_RC_STATE_VALUE_LEN)-1)<<RF_ANA1_RF_FSM0_RC_STATE_VALUE_POS)
#define RF_ANA1_RF_FSM0_RC_STATE_VALUE_UMSK                     (~(((1U<<RF_ANA1_RF_FSM0_RC_STATE_VALUE_LEN)-1)<<RF_ANA1_RF_FSM0_RC_STATE_VALUE_POS))

/* 0x8 : rfsm status reg */
#define RF_ANA1_RF_FSM_CTRL_SW_OFFSET                           (0x8)
#define RF_ANA1_RF_FSM_SW_ST                                    RF_ANA1_RF_FSM_SW_ST
#define RF_ANA1_RF_FSM_SW_ST_POS                                (0U)
#define RF_ANA1_RF_FSM_SW_ST_LEN                                (5U)
#define RF_ANA1_RF_FSM_SW_ST_MSK                                (((1U<<RF_ANA1_RF_FSM_SW_ST_LEN)-1)<<RF_ANA1_RF_FSM_SW_ST_POS)
#define RF_ANA1_RF_FSM_SW_ST_UMSK                               (~(((1U<<RF_ANA1_RF_FSM_SW_ST_LEN)-1)<<RF_ANA1_RF_FSM_SW_ST_POS))
#define RF_ANA1_RF_FSM_SW_ST_VLD                                RF_ANA1_RF_FSM_SW_ST_VLD
#define RF_ANA1_RF_FSM_SW_ST_VLD_POS                            (8U)
#define RF_ANA1_RF_FSM_SW_ST_VLD_LEN                            (1U)
#define RF_ANA1_RF_FSM_SW_ST_VLD_MSK                            (((1U<<RF_ANA1_RF_FSM_SW_ST_VLD_LEN)-1)<<RF_ANA1_RF_FSM_SW_ST_VLD_POS)
#define RF_ANA1_RF_FSM_SW_ST_VLD_UMSK                           (~(((1U<<RF_ANA1_RF_FSM_SW_ST_VLD_LEN)-1)<<RF_ANA1_RF_FSM_SW_ST_VLD_POS))
#define RF_ANA1_FULL_CAL_EN                                     RF_ANA1_FULL_CAL_EN
#define RF_ANA1_FULL_CAL_EN_POS                                 (12U)
#define RF_ANA1_FULL_CAL_EN_LEN                                 (1U)
#define RF_ANA1_FULL_CAL_EN_MSK                                 (((1U<<RF_ANA1_FULL_CAL_EN_LEN)-1)<<RF_ANA1_FULL_CAL_EN_POS)
#define RF_ANA1_FULL_CAL_EN_UMSK                                (~(((1U<<RF_ANA1_FULL_CAL_EN_LEN)-1)<<RF_ANA1_FULL_CAL_EN_POS))
#define RF_ANA1_INC_CAL_TIMEOUT                                 RF_ANA1_INC_CAL_TIMEOUT
#define RF_ANA1_INC_CAL_TIMEOUT_POS                             (16U)
#define RF_ANA1_INC_CAL_TIMEOUT_LEN                             (1U)
#define RF_ANA1_INC_CAL_TIMEOUT_MSK                             (((1U<<RF_ANA1_INC_CAL_TIMEOUT_LEN)-1)<<RF_ANA1_INC_CAL_TIMEOUT_POS)
#define RF_ANA1_INC_CAL_TIMEOUT_UMSK                            (~(((1U<<RF_ANA1_INC_CAL_TIMEOUT_LEN)-1)<<RF_ANA1_INC_CAL_TIMEOUT_POS))
#define RF_ANA1_LO_UNLOCKED                                     RF_ANA1_LO_UNLOCKED
#define RF_ANA1_LO_UNLOCKED_POS                                 (20U)
#define RF_ANA1_LO_UNLOCKED_LEN                                 (1U)
#define RF_ANA1_LO_UNLOCKED_MSK                                 (((1U<<RF_ANA1_LO_UNLOCKED_LEN)-1)<<RF_ANA1_LO_UNLOCKED_POS)
#define RF_ANA1_LO_UNLOCKED_UMSK                                (~(((1U<<RF_ANA1_LO_UNLOCKED_LEN)-1)<<RF_ANA1_LO_UNLOCKED_POS))

/* 0xC : Control logic switch */
#define RF_ANA1_RFCTRL_HW_EN_OFFSET                             (0xC)
#define RF_ANA1_RF_RESV0                                        RF_ANA1_RF_RESV0
#define RF_ANA1_RF_RESV0_POS                                    (0U)
#define RF_ANA1_RF_RESV0_LEN                                    (32U)
#define RF_ANA1_RF_RESV0_MSK                                    (((1U<<RF_ANA1_RF_RESV0_LEN)-1)<<RF_ANA1_RF_RESV0_POS)
#define RF_ANA1_RF_RESV0_UMSK                                   (~(((1U<<RF_ANA1_RF_RESV0_LEN)-1)<<RF_ANA1_RF_RESV0_POS))

/* 0x10 : temp_comp */
#define RF_ANA1_TEMP_COMP_OFFSET                                (0x10)

/* 0x14 : Calibration mode register */
#define RF_ANA1_RFCAL_CTRLEN_OFFSET                             (0x14)
#define RF_ANA1_INC_FCAL_EN                                     RF_ANA1_INC_FCAL_EN
#define RF_ANA1_INC_FCAL_EN_POS                                 (0U)
#define RF_ANA1_INC_FCAL_EN_LEN                                 (1U)
#define RF_ANA1_INC_FCAL_EN_MSK                                 (((1U<<RF_ANA1_INC_FCAL_EN_LEN)-1)<<RF_ANA1_INC_FCAL_EN_POS)
#define RF_ANA1_INC_FCAL_EN_UMSK                                (~(((1U<<RF_ANA1_INC_FCAL_EN_LEN)-1)<<RF_ANA1_INC_FCAL_EN_POS))
#define RF_ANA1_INC_ACAL_EN                                     RF_ANA1_INC_ACAL_EN
#define RF_ANA1_INC_ACAL_EN_POS                                 (1U)
#define RF_ANA1_INC_ACAL_EN_LEN                                 (1U)
#define RF_ANA1_INC_ACAL_EN_MSK                                 (((1U<<RF_ANA1_INC_ACAL_EN_LEN)-1)<<RF_ANA1_INC_ACAL_EN_POS)
#define RF_ANA1_INC_ACAL_EN_UMSK                                (~(((1U<<RF_ANA1_INC_ACAL_EN_LEN)-1)<<RF_ANA1_INC_ACAL_EN_POS))
#define RF_ANA1_LNA_PWRDET_CAL_EN                               RF_ANA1_LNA_PWRDET_CAL_EN
#define RF_ANA1_LNA_PWRDET_CAL_EN_POS                           (8U)
#define RF_ANA1_LNA_PWRDET_CAL_EN_LEN                           (1U)
#define RF_ANA1_LNA_PWRDET_CAL_EN_MSK                           (((1U<<RF_ANA1_LNA_PWRDET_CAL_EN_LEN)-1)<<RF_ANA1_LNA_PWRDET_CAL_EN_POS)
#define RF_ANA1_LNA_PWRDET_CAL_EN_UMSK                          (~(((1U<<RF_ANA1_LNA_PWRDET_CAL_EN_LEN)-1)<<RF_ANA1_LNA_PWRDET_CAL_EN_POS))
#define RF_ANA1_RCCAL_EN                                        RF_ANA1_RCCAL_EN
#define RF_ANA1_RCCAL_EN_POS                                    (9U)
#define RF_ANA1_RCCAL_EN_LEN                                    (1U)
#define RF_ANA1_RCCAL_EN_MSK                                    (((1U<<RF_ANA1_RCCAL_EN_LEN)-1)<<RF_ANA1_RCCAL_EN_POS)
#define RF_ANA1_RCCAL_EN_UMSK                                   (~(((1U<<RF_ANA1_RCCAL_EN_LEN)-1)<<RF_ANA1_RCCAL_EN_POS))
#define RF_ANA1_DPD_EN                                          RF_ANA1_DPD_EN
#define RF_ANA1_DPD_EN_POS                                      (10U)
#define RF_ANA1_DPD_EN_LEN                                      (1U)
#define RF_ANA1_DPD_EN_MSK                                      (((1U<<RF_ANA1_DPD_EN_LEN)-1)<<RF_ANA1_DPD_EN_POS)
#define RF_ANA1_DPD_EN_UMSK                                     (~(((1U<<RF_ANA1_DPD_EN_LEN)-1)<<RF_ANA1_DPD_EN_POS))
#define RF_ANA1_INC_ROSCAL_EN                                   RF_ANA1_INC_ROSCAL_EN
#define RF_ANA1_INC_ROSCAL_EN_POS                               (11U)
#define RF_ANA1_INC_ROSCAL_EN_LEN                               (1U)
#define RF_ANA1_INC_ROSCAL_EN_MSK                               (((1U<<RF_ANA1_INC_ROSCAL_EN_LEN)-1)<<RF_ANA1_INC_ROSCAL_EN_POS)
#define RF_ANA1_INC_ROSCAL_EN_UMSK                              (~(((1U<<RF_ANA1_INC_ROSCAL_EN_LEN)-1)<<RF_ANA1_INC_ROSCAL_EN_POS))

/* 0x18 : ZRF Control register 0 */
#define RF_ANA1_RF_BASE_CTRL1_OFFSET                            (0x18)
#define RF_ANA1_PPU_LEAD                                        RF_ANA1_PPU_LEAD
#define RF_ANA1_PPU_LEAD_POS                                    (8U)
#define RF_ANA1_PPU_LEAD_LEN                                    (2U)
#define RF_ANA1_PPU_LEAD_MSK                                    (((1U<<RF_ANA1_PPU_LEAD_LEN)-1)<<RF_ANA1_PPU_LEAD_POS)
#define RF_ANA1_PPU_LEAD_UMSK                                   (~(((1U<<RF_ANA1_PPU_LEAD_LEN)-1)<<RF_ANA1_PPU_LEAD_POS))
#define RF_ANA1_PUD_VCO_DLY                                     RF_ANA1_PUD_VCO_DLY
#define RF_ANA1_PUD_VCO_DLY_POS                                 (10U)
#define RF_ANA1_PUD_VCO_DLY_LEN                                 (2U)
#define RF_ANA1_PUD_VCO_DLY_MSK                                 (((1U<<RF_ANA1_PUD_VCO_DLY_LEN)-1)<<RF_ANA1_PUD_VCO_DLY_POS)
#define RF_ANA1_PUD_VCO_DLY_UMSK                                (~(((1U<<RF_ANA1_PUD_VCO_DLY_LEN)-1)<<RF_ANA1_PUD_VCO_DLY_POS))

/* 0x1C : ZRF Control register 0 */
#define RF_ANA1_RF_BASE_CTRL2_OFFSET                            (0x1C)
#define RF_ANA1_PU_CTRL_HW                                      RF_ANA1_PU_CTRL_HW
#define RF_ANA1_PU_CTRL_HW_POS                                  (0U)
#define RF_ANA1_PU_CTRL_HW_LEN                                  (1U)
#define RF_ANA1_PU_CTRL_HW_MSK                                  (((1U<<RF_ANA1_PU_CTRL_HW_LEN)-1)<<RF_ANA1_PU_CTRL_HW_POS)
#define RF_ANA1_PU_CTRL_HW_UMSK                                 (~(((1U<<RF_ANA1_PU_CTRL_HW_LEN)-1)<<RF_ANA1_PU_CTRL_HW_POS))
#define RF_ANA1_RX_GAIN_CTRL_HW                                 RF_ANA1_RX_GAIN_CTRL_HW
#define RF_ANA1_RX_GAIN_CTRL_HW_POS                             (1U)
#define RF_ANA1_RX_GAIN_CTRL_HW_LEN                             (1U)
#define RF_ANA1_RX_GAIN_CTRL_HW_MSK                             (((1U<<RF_ANA1_RX_GAIN_CTRL_HW_LEN)-1)<<RF_ANA1_RX_GAIN_CTRL_HW_POS)
#define RF_ANA1_RX_GAIN_CTRL_HW_UMSK                            (~(((1U<<RF_ANA1_RX_GAIN_CTRL_HW_LEN)-1)<<RF_ANA1_RX_GAIN_CTRL_HW_POS))
#define RF_ANA1_TX_GAIN_CTRL_HW                                 RF_ANA1_TX_GAIN_CTRL_HW
#define RF_ANA1_TX_GAIN_CTRL_HW_POS                             (2U)
#define RF_ANA1_TX_GAIN_CTRL_HW_LEN                             (1U)
#define RF_ANA1_TX_GAIN_CTRL_HW_MSK                             (((1U<<RF_ANA1_TX_GAIN_CTRL_HW_LEN)-1)<<RF_ANA1_TX_GAIN_CTRL_HW_POS)
#define RF_ANA1_TX_GAIN_CTRL_HW_UMSK                            (~(((1U<<RF_ANA1_TX_GAIN_CTRL_HW_LEN)-1)<<RF_ANA1_TX_GAIN_CTRL_HW_POS))
#define RF_ANA1_LNA_CTRL_HW                                     RF_ANA1_LNA_CTRL_HW
#define RF_ANA1_LNA_CTRL_HW_POS                                 (3U)
#define RF_ANA1_LNA_CTRL_HW_LEN                                 (1U)
#define RF_ANA1_LNA_CTRL_HW_MSK                                 (((1U<<RF_ANA1_LNA_CTRL_HW_LEN)-1)<<RF_ANA1_LNA_CTRL_HW_POS)
#define RF_ANA1_LNA_CTRL_HW_UMSK                                (~(((1U<<RF_ANA1_LNA_CTRL_HW_LEN)-1)<<RF_ANA1_LNA_CTRL_HW_POS))
#define RF_ANA1_RXCOMP_CTRL_HW                                  RF_ANA1_RXCOMP_CTRL_HW
#define RF_ANA1_RXCOMP_CTRL_HW_POS                              (4U)
#define RF_ANA1_RXCOMP_CTRL_HW_LEN                              (1U)
#define RF_ANA1_RXCOMP_CTRL_HW_MSK                              (((1U<<RF_ANA1_RXCOMP_CTRL_HW_LEN)-1)<<RF_ANA1_RXCOMP_CTRL_HW_POS)
#define RF_ANA1_RXCOMP_CTRL_HW_UMSK                             (~(((1U<<RF_ANA1_RXCOMP_CTRL_HW_LEN)-1)<<RF_ANA1_RXCOMP_CTRL_HW_POS))
#define RF_ANA1_PA_MODE_CTRL_HW                                 RF_ANA1_PA_MODE_CTRL_HW
#define RF_ANA1_PA_MODE_CTRL_HW_POS                             (5U)
#define RF_ANA1_PA_MODE_CTRL_HW_LEN                             (1U)
#define RF_ANA1_PA_MODE_CTRL_HW_MSK                             (((1U<<RF_ANA1_PA_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_PA_MODE_CTRL_HW_POS)
#define RF_ANA1_PA_MODE_CTRL_HW_UMSK                            (~(((1U<<RF_ANA1_PA_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_PA_MODE_CTRL_HW_POS))
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW                  RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_POS              (6U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN              (1U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_MSK              (((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_POS)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_UMSK             (~(((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_CTRL_HW_POS))
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW                RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_POS            (7U)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_LEN            (1U)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_MSK            (((1U<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_POS)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_UMSK           (~(((1U<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_POS))
#define RF_ANA1_VCO_IDAC_CTRL_HW                                RF_ANA1_VCO_IDAC_CTRL_HW
#define RF_ANA1_VCO_IDAC_CTRL_HW_POS                            (8U)
#define RF_ANA1_VCO_IDAC_CTRL_HW_LEN                            (1U)
#define RF_ANA1_VCO_IDAC_CTRL_HW_MSK                            (((1U<<RF_ANA1_VCO_IDAC_CTRL_HW_LEN)-1)<<RF_ANA1_VCO_IDAC_CTRL_HW_POS)
#define RF_ANA1_VCO_IDAC_CTRL_HW_UMSK                           (~(((1U<<RF_ANA1_VCO_IDAC_CTRL_HW_LEN)-1)<<RF_ANA1_VCO_IDAC_CTRL_HW_POS))
#define RF_ANA1_ADPLL_LF_CTRL_HW                                RF_ANA1_ADPLL_LF_CTRL_HW
#define RF_ANA1_ADPLL_LF_CTRL_HW_POS                            (9U)
#define RF_ANA1_ADPLL_LF_CTRL_HW_LEN                            (1U)
#define RF_ANA1_ADPLL_LF_CTRL_HW_MSK                            (((1U<<RF_ANA1_ADPLL_LF_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_CTRL_HW_POS)
#define RF_ANA1_ADPLL_LF_CTRL_HW_UMSK                           (~(((1U<<RF_ANA1_ADPLL_LF_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_CTRL_HW_POS))
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW                 RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_POS             (10U)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_LEN             (1U)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_MSK             (((1U<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_POS)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_UMSK            (~(((1U<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_CTRL_HW_POS))
#define RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW                        RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW
#define RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_POS                    (11U)
#define RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_LEN                    (1U)
#define RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_MSK                    (((1U<<RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_POS)
#define RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_UMSK                   (~(((1U<<RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_CTRL_HW_POS))
#define RF_ANA1_ADPLL_SDM_IN_CTRL_HW                            RF_ANA1_ADPLL_SDM_IN_CTRL_HW
#define RF_ANA1_ADPLL_SDM_IN_CTRL_HW_POS                        (12U)
#define RF_ANA1_ADPLL_SDM_IN_CTRL_HW_LEN                        (1U)
#define RF_ANA1_ADPLL_SDM_IN_CTRL_HW_MSK                        (((1U<<RF_ANA1_ADPLL_SDM_IN_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_IN_CTRL_HW_POS)
#define RF_ANA1_ADPLL_SDM_IN_CTRL_HW_UMSK                       (~(((1U<<RF_ANA1_ADPLL_SDM_IN_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_IN_CTRL_HW_POS))
#define RF_ANA1_ADPLL_LP_MODE_CTRL_HW                           RF_ANA1_ADPLL_LP_MODE_CTRL_HW
#define RF_ANA1_ADPLL_LP_MODE_CTRL_HW_POS                       (13U)
#define RF_ANA1_ADPLL_LP_MODE_CTRL_HW_LEN                       (1U)
#define RF_ANA1_ADPLL_LP_MODE_CTRL_HW_MSK                       (((1U<<RF_ANA1_ADPLL_LP_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_LP_MODE_CTRL_HW_POS)
#define RF_ANA1_ADPLL_LP_MODE_CTRL_HW_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LP_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_ADPLL_LP_MODE_CTRL_HW_POS))
#define RF_ANA1_LOBUF_EN_CTRL_HW                                RF_ANA1_LOBUF_EN_CTRL_HW
#define RF_ANA1_LOBUF_EN_CTRL_HW_POS                            (14U)
#define RF_ANA1_LOBUF_EN_CTRL_HW_LEN                            (1U)
#define RF_ANA1_LOBUF_EN_CTRL_HW_MSK                            (((1U<<RF_ANA1_LOBUF_EN_CTRL_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_CTRL_HW_POS)
#define RF_ANA1_LOBUF_EN_CTRL_HW_UMSK                           (~(((1U<<RF_ANA1_LOBUF_EN_CTRL_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_CTRL_HW_POS))
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW                     RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_POS                 (15U)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_LEN                 (1U)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_MSK                 (((1U<<RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_POS)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_UMSK                (~(((1U<<RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RSTN_CTRL_HW_POS))
#define RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW                      RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW
#define RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_POS                  (16U)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN                  (1U)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_MSK                  (((1U<<RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_POS)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_UMSK                 (~(((1U<<RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_OUT_RSTN_CTRL_HW_POS))
#define RF_ANA1_RBB_PKDET_EN_CTRL_HW                            RF_ANA1_RBB_PKDET_EN_CTRL_HW
#define RF_ANA1_RBB_PKDET_EN_CTRL_HW_POS                        (17U)
#define RF_ANA1_RBB_PKDET_EN_CTRL_HW_LEN                        (1U)
#define RF_ANA1_RBB_PKDET_EN_CTRL_HW_MSK                        (((1U<<RF_ANA1_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_EN_CTRL_HW_POS)
#define RF_ANA1_RBB_PKDET_EN_CTRL_HW_UMSK                       (~(((1U<<RF_ANA1_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_EN_CTRL_HW_POS))
#define RF_ANA1_RBB_BT_MODE_CTRL_HW                             RF_ANA1_RBB_BT_MODE_CTRL_HW
#define RF_ANA1_RBB_BT_MODE_CTRL_HW_POS                         (18U)
#define RF_ANA1_RBB_BT_MODE_CTRL_HW_LEN                         (1U)
#define RF_ANA1_RBB_BT_MODE_CTRL_HW_MSK                         (((1U<<RF_ANA1_RBB_BT_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_RBB_BT_MODE_CTRL_HW_POS)
#define RF_ANA1_RBB_BT_MODE_CTRL_HW_UMSK                        (~(((1U<<RF_ANA1_RBB_BT_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_RBB_BT_MODE_CTRL_HW_POS))
#define RF_ANA1_LPVCO_IDAC_CTRL_HW                              RF_ANA1_LPVCO_IDAC_CTRL_HW
#define RF_ANA1_LPVCO_IDAC_CTRL_HW_POS                          (19U)
#define RF_ANA1_LPVCO_IDAC_CTRL_HW_LEN                          (1U)
#define RF_ANA1_LPVCO_IDAC_CTRL_HW_MSK                          (((1U<<RF_ANA1_LPVCO_IDAC_CTRL_HW_LEN)-1)<<RF_ANA1_LPVCO_IDAC_CTRL_HW_POS)
#define RF_ANA1_LPVCO_IDAC_CTRL_HW_UMSK                         (~(((1U<<RF_ANA1_LPVCO_IDAC_CTRL_HW_LEN)-1)<<RF_ANA1_LPVCO_IDAC_CTRL_HW_POS))
#define RF_ANA1_RXADC_CTRL_HW                                   RF_ANA1_RXADC_CTRL_HW
#define RF_ANA1_RXADC_CTRL_HW_POS                               (27U)
#define RF_ANA1_RXADC_CTRL_HW_LEN                               (1U)
#define RF_ANA1_RXADC_CTRL_HW_MSK                               (((1U<<RF_ANA1_RXADC_CTRL_HW_LEN)-1)<<RF_ANA1_RXADC_CTRL_HW_POS)
#define RF_ANA1_RXADC_CTRL_HW_UMSK                              (~(((1U<<RF_ANA1_RXADC_CTRL_HW_LEN)-1)<<RF_ANA1_RXADC_CTRL_HW_POS))
#define RF_ANA1_TXCAL_CTRL_HW                                   RF_ANA1_TXCAL_CTRL_HW
#define RF_ANA1_TXCAL_CTRL_HW_POS                               (28U)
#define RF_ANA1_TXCAL_CTRL_HW_LEN                               (1U)
#define RF_ANA1_TXCAL_CTRL_HW_MSK                               (((1U<<RF_ANA1_TXCAL_CTRL_HW_LEN)-1)<<RF_ANA1_TXCAL_CTRL_HW_POS)
#define RF_ANA1_TXCAL_CTRL_HW_UMSK                              (~(((1U<<RF_ANA1_TXCAL_CTRL_HW_LEN)-1)<<RF_ANA1_TXCAL_CTRL_HW_POS))
#define RF_ANA1_DPD_CTRL_HW                                     RF_ANA1_DPD_CTRL_HW
#define RF_ANA1_DPD_CTRL_HW_POS                                 (29U)
#define RF_ANA1_DPD_CTRL_HW_LEN                                 (1U)
#define RF_ANA1_DPD_CTRL_HW_MSK                                 (((1U<<RF_ANA1_DPD_CTRL_HW_LEN)-1)<<RF_ANA1_DPD_CTRL_HW_POS)
#define RF_ANA1_DPD_CTRL_HW_UMSK                                (~(((1U<<RF_ANA1_DPD_CTRL_HW_LEN)-1)<<RF_ANA1_DPD_CTRL_HW_POS))
#define RF_ANA1_PWRDET_CTRL_HW                                  RF_ANA1_PWRDET_CTRL_HW
#define RF_ANA1_PWRDET_CTRL_HW_POS                              (30U)
#define RF_ANA1_PWRDET_CTRL_HW_LEN                              (1U)
#define RF_ANA1_PWRDET_CTRL_HW_MSK                              (((1U<<RF_ANA1_PWRDET_CTRL_HW_LEN)-1)<<RF_ANA1_PWRDET_CTRL_HW_POS)
#define RF_ANA1_PWRDET_CTRL_HW_UMSK                             (~(((1U<<RF_ANA1_PWRDET_CTRL_HW_LEN)-1)<<RF_ANA1_PWRDET_CTRL_HW_POS))

/* 0x20 : pucr */
#define RF_ANA1_PUCR_OFFSET                                     (0x20)
#define RF_ANA1_PU_TOSDAC                                       RF_ANA1_PU_TOSDAC
#define RF_ANA1_PU_TOSDAC_POS                                   (3U)
#define RF_ANA1_PU_TOSDAC_LEN                                   (1U)
#define RF_ANA1_PU_TOSDAC_MSK                                   (((1U<<RF_ANA1_PU_TOSDAC_LEN)-1)<<RF_ANA1_PU_TOSDAC_POS)
#define RF_ANA1_PU_TOSDAC_UMSK                                  (~(((1U<<RF_ANA1_PU_TOSDAC_LEN)-1)<<RF_ANA1_PU_TOSDAC_POS))
#define RF_ANA1_PU_ROSDAC                                       RF_ANA1_PU_ROSDAC
#define RF_ANA1_PU_ROSDAC_POS                                   (4U)
#define RF_ANA1_PU_ROSDAC_LEN                                   (1U)
#define RF_ANA1_PU_ROSDAC_MSK                                   (((1U<<RF_ANA1_PU_ROSDAC_LEN)-1)<<RF_ANA1_PU_ROSDAC_POS)
#define RF_ANA1_PU_ROSDAC_UMSK                                  (~(((1U<<RF_ANA1_PU_ROSDAC_LEN)-1)<<RF_ANA1_PU_ROSDAC_POS))
#define RF_ANA1_PU_PKDET                                        RF_ANA1_PU_PKDET
#define RF_ANA1_PU_PKDET_POS                                    (5U)
#define RF_ANA1_PU_PKDET_LEN                                    (1U)
#define RF_ANA1_PU_PKDET_MSK                                    (((1U<<RF_ANA1_PU_PKDET_LEN)-1)<<RF_ANA1_PU_PKDET_POS)
#define RF_ANA1_PU_PKDET_UMSK                                   (~(((1U<<RF_ANA1_PU_PKDET_LEN)-1)<<RF_ANA1_PU_PKDET_POS))
#define RF_ANA1_PU_LNA_PWRDET                                   RF_ANA1_PU_LNA_PWRDET
#define RF_ANA1_PU_LNA_PWRDET_POS                               (6U)
#define RF_ANA1_PU_LNA_PWRDET_LEN                               (1U)
#define RF_ANA1_PU_LNA_PWRDET_MSK                               (((1U<<RF_ANA1_PU_LNA_PWRDET_LEN)-1)<<RF_ANA1_PU_LNA_PWRDET_POS)
#define RF_ANA1_PU_LNA_PWRDET_UMSK                              (~(((1U<<RF_ANA1_PU_LNA_PWRDET_LEN)-1)<<RF_ANA1_PU_LNA_PWRDET_POS))
#define RF_ANA1_PU_PWRDET_5G_OSDAC                              RF_ANA1_PU_PWRDET_5G_OSDAC
#define RF_ANA1_PU_PWRDET_5G_OSDAC_POS                          (7U)
#define RF_ANA1_PU_PWRDET_5G_OSDAC_LEN                          (1U)
#define RF_ANA1_PU_PWRDET_5G_OSDAC_MSK                          (((1U<<RF_ANA1_PU_PWRDET_5G_OSDAC_LEN)-1)<<RF_ANA1_PU_PWRDET_5G_OSDAC_POS)
#define RF_ANA1_PU_PWRDET_5G_OSDAC_UMSK                         (~(((1U<<RF_ANA1_PU_PWRDET_5G_OSDAC_LEN)-1)<<RF_ANA1_PU_PWRDET_5G_OSDAC_POS))
#define RF_ANA1_PU_PWRDET_5G                                    RF_ANA1_PU_PWRDET_5G
#define RF_ANA1_PU_PWRDET_5G_POS                                (8U)
#define RF_ANA1_PU_PWRDET_5G_LEN                                (1U)
#define RF_ANA1_PU_PWRDET_5G_MSK                                (((1U<<RF_ANA1_PU_PWRDET_5G_LEN)-1)<<RF_ANA1_PU_PWRDET_5G_POS)
#define RF_ANA1_PU_PWRDET_5G_UMSK                               (~(((1U<<RF_ANA1_PU_PWRDET_5G_LEN)-1)<<RF_ANA1_PU_PWRDET_5G_POS))
#define RF_ANA1_PU_PWRDET_2G_OSDAC                              RF_ANA1_PU_PWRDET_2G_OSDAC
#define RF_ANA1_PU_PWRDET_2G_OSDAC_POS                          (9U)
#define RF_ANA1_PU_PWRDET_2G_OSDAC_LEN                          (1U)
#define RF_ANA1_PU_PWRDET_2G_OSDAC_MSK                          (((1U<<RF_ANA1_PU_PWRDET_2G_OSDAC_LEN)-1)<<RF_ANA1_PU_PWRDET_2G_OSDAC_POS)
#define RF_ANA1_PU_PWRDET_2G_OSDAC_UMSK                         (~(((1U<<RF_ANA1_PU_PWRDET_2G_OSDAC_LEN)-1)<<RF_ANA1_PU_PWRDET_2G_OSDAC_POS))
#define RF_ANA1_PU_PWRDET_2G                                    RF_ANA1_PU_PWRDET_2G
#define RF_ANA1_PU_PWRDET_2G_POS                                (10U)
#define RF_ANA1_PU_PWRDET_2G_LEN                                (1U)
#define RF_ANA1_PU_PWRDET_2G_MSK                                (((1U<<RF_ANA1_PU_PWRDET_2G_LEN)-1)<<RF_ANA1_PU_PWRDET_2G_POS)
#define RF_ANA1_PU_PWRDET_2G_UMSK                               (~(((1U<<RF_ANA1_PU_PWRDET_2G_LEN)-1)<<RF_ANA1_PU_PWRDET_2G_POS))
#define RF_ANA1_PU_ATEST                                        RF_ANA1_PU_ATEST
#define RF_ANA1_PU_ATEST_POS                                    (11U)
#define RF_ANA1_PU_ATEST_LEN                                    (1U)
#define RF_ANA1_PU_ATEST_MSK                                    (((1U<<RF_ANA1_PU_ATEST_LEN)-1)<<RF_ANA1_PU_ATEST_POS)
#define RF_ANA1_PU_ATEST_UMSK                                   (~(((1U<<RF_ANA1_PU_ATEST_LEN)-1)<<RF_ANA1_PU_ATEST_POS))
#define RF_ANA1_PPU_ATEST                                       RF_ANA1_PPU_ATEST
#define RF_ANA1_PPU_ATEST_POS                                   (12U)
#define RF_ANA1_PPU_ATEST_LEN                                   (1U)
#define RF_ANA1_PPU_ATEST_MSK                                   (((1U<<RF_ANA1_PPU_ATEST_LEN)-1)<<RF_ANA1_PPU_ATEST_POS)
#define RF_ANA1_PPU_ATEST_UMSK                                  (~(((1U<<RF_ANA1_PPU_ATEST_LEN)-1)<<RF_ANA1_PPU_ATEST_POS))

/* 0x24 : pucr_fsm */
#define RF_ANA1_PUCR_FSM_OFFSET                                 (0x24)
#define RF_ANA1_RXSW_EN                                         RF_ANA1_RXSW_EN
#define RF_ANA1_RXSW_EN_POS                                     (0U)
#define RF_ANA1_RXSW_EN_LEN                                     (2U)
#define RF_ANA1_RXSW_EN_MSK                                     (((1U<<RF_ANA1_RXSW_EN_LEN)-1)<<RF_ANA1_RXSW_EN_POS)
#define RF_ANA1_RXSW_EN_UMSK                                    (~(((1U<<RF_ANA1_RXSW_EN_LEN)-1)<<RF_ANA1_RXSW_EN_POS))
#define RF_ANA1_PU_LNA                                          RF_ANA1_PU_LNA
#define RF_ANA1_PU_LNA_POS                                      (2U)
#define RF_ANA1_PU_LNA_LEN                                      (1U)
#define RF_ANA1_PU_LNA_MSK                                      (((1U<<RF_ANA1_PU_LNA_LEN)-1)<<RF_ANA1_PU_LNA_POS)
#define RF_ANA1_PU_LNA_UMSK                                     (~(((1U<<RF_ANA1_PU_LNA_LEN)-1)<<RF_ANA1_PU_LNA_POS))
#define RF_ANA1_PU_RMXGM                                        RF_ANA1_PU_RMXGM
#define RF_ANA1_PU_RMXGM_POS                                    (3U)
#define RF_ANA1_PU_RMXGM_LEN                                    (1U)
#define RF_ANA1_PU_RMXGM_MSK                                    (((1U<<RF_ANA1_PU_RMXGM_LEN)-1)<<RF_ANA1_PU_RMXGM_POS)
#define RF_ANA1_PU_RMXGM_UMSK                                   (~(((1U<<RF_ANA1_PU_RMXGM_LEN)-1)<<RF_ANA1_PU_RMXGM_POS))
#define RF_ANA1_PU_RMX                                          RF_ANA1_PU_RMX
#define RF_ANA1_PU_RMX_POS                                      (4U)
#define RF_ANA1_PU_RMX_LEN                                      (1U)
#define RF_ANA1_PU_RMX_MSK                                      (((1U<<RF_ANA1_PU_RMX_LEN)-1)<<RF_ANA1_PU_RMX_POS)
#define RF_ANA1_PU_RMX_UMSK                                     (~(((1U<<RF_ANA1_PU_RMX_LEN)-1)<<RF_ANA1_PU_RMX_POS))
#define RF_ANA1_PU_RBB                                          RF_ANA1_PU_RBB
#define RF_ANA1_PU_RBB_POS                                      (5U)
#define RF_ANA1_PU_RBB_LEN                                      (1U)
#define RF_ANA1_PU_RBB_MSK                                      (((1U<<RF_ANA1_PU_RBB_LEN)-1)<<RF_ANA1_PU_RBB_POS)
#define RF_ANA1_PU_RBB_UMSK                                     (~(((1U<<RF_ANA1_PU_RBB_LEN)-1)<<RF_ANA1_PU_RBB_POS))
#define RF_ANA1_PU_RXADC                                        RF_ANA1_PU_RXADC
#define RF_ANA1_PU_RXADC_POS                                    (6U)
#define RF_ANA1_PU_RXADC_LEN                                    (1U)
#define RF_ANA1_PU_RXADC_MSK                                    (((1U<<RF_ANA1_PU_RXADC_LEN)-1)<<RF_ANA1_PU_RXADC_POS)
#define RF_ANA1_PU_RXADC_UMSK                                   (~(((1U<<RF_ANA1_PU_RXADC_LEN)-1)<<RF_ANA1_PU_RXADC_POS))
#define RF_ANA1_RXADC_CLK_EN                                    RF_ANA1_RXADC_CLK_EN
#define RF_ANA1_RXADC_CLK_EN_POS                                (7U)
#define RF_ANA1_RXADC_CLK_EN_LEN                                (1U)
#define RF_ANA1_RXADC_CLK_EN_MSK                                (((1U<<RF_ANA1_RXADC_CLK_EN_LEN)-1)<<RF_ANA1_RXADC_CLK_EN_POS)
#define RF_ANA1_RXADC_CLK_EN_UMSK                               (~(((1U<<RF_ANA1_RXADC_CLK_EN_LEN)-1)<<RF_ANA1_RXADC_CLK_EN_POS))
#define RF_ANA1_PU_PA_2G                                        RF_ANA1_PU_PA_2G
#define RF_ANA1_PU_PA_2G_POS                                    (8U)
#define RF_ANA1_PU_PA_2G_LEN                                    (1U)
#define RF_ANA1_PU_PA_2G_MSK                                    (((1U<<RF_ANA1_PU_PA_2G_LEN)-1)<<RF_ANA1_PU_PA_2G_POS)
#define RF_ANA1_PU_PA_2G_UMSK                                   (~(((1U<<RF_ANA1_PU_PA_2G_LEN)-1)<<RF_ANA1_PU_PA_2G_POS))
#define RF_ANA1_PU_TMX_2G                                       RF_ANA1_PU_TMX_2G
#define RF_ANA1_PU_TMX_2G_POS                                   (9U)
#define RF_ANA1_PU_TMX_2G_LEN                                   (1U)
#define RF_ANA1_PU_TMX_2G_MSK                                   (((1U<<RF_ANA1_PU_TMX_2G_LEN)-1)<<RF_ANA1_PU_TMX_2G_POS)
#define RF_ANA1_PU_TMX_2G_UMSK                                  (~(((1U<<RF_ANA1_PU_TMX_2G_LEN)-1)<<RF_ANA1_PU_TMX_2G_POS))
#define RF_ANA1_PU_PA_5G                                        RF_ANA1_PU_PA_5G
#define RF_ANA1_PU_PA_5G_POS                                    (10U)
#define RF_ANA1_PU_PA_5G_LEN                                    (1U)
#define RF_ANA1_PU_PA_5G_MSK                                    (((1U<<RF_ANA1_PU_PA_5G_LEN)-1)<<RF_ANA1_PU_PA_5G_POS)
#define RF_ANA1_PU_PA_5G_UMSK                                   (~(((1U<<RF_ANA1_PU_PA_5G_LEN)-1)<<RF_ANA1_PU_PA_5G_POS))
#define RF_ANA1_PU_TMX_5G                                       RF_ANA1_PU_TMX_5G
#define RF_ANA1_PU_TMX_5G_POS                                   (11U)
#define RF_ANA1_PU_TMX_5G_LEN                                   (1U)
#define RF_ANA1_PU_TMX_5G_MSK                                   (((1U<<RF_ANA1_PU_TMX_5G_LEN)-1)<<RF_ANA1_PU_TMX_5G_POS)
#define RF_ANA1_PU_TMX_5G_UMSK                                  (~(((1U<<RF_ANA1_PU_TMX_5G_LEN)-1)<<RF_ANA1_PU_TMX_5G_POS))
#define RF_ANA1_PU_TBB                                          RF_ANA1_PU_TBB
#define RF_ANA1_PU_TBB_POS                                      (12U)
#define RF_ANA1_PU_TBB_LEN                                      (1U)
#define RF_ANA1_PU_TBB_MSK                                      (((1U<<RF_ANA1_PU_TBB_LEN)-1)<<RF_ANA1_PU_TBB_POS)
#define RF_ANA1_PU_TBB_UMSK                                     (~(((1U<<RF_ANA1_PU_TBB_LEN)-1)<<RF_ANA1_PU_TBB_POS))
#define RF_ANA1_PU_DAC                                          RF_ANA1_PU_DAC
#define RF_ANA1_PU_DAC_POS                                      (13U)
#define RF_ANA1_PU_DAC_LEN                                      (1U)
#define RF_ANA1_PU_DAC_MSK                                      (((1U<<RF_ANA1_PU_DAC_LEN)-1)<<RF_ANA1_PU_DAC_POS)
#define RF_ANA1_PU_DAC_UMSK                                     (~(((1U<<RF_ANA1_PU_DAC_LEN)-1)<<RF_ANA1_PU_DAC_POS))
#define RF_ANA1_PU_RCAL                                         RF_ANA1_PU_RCAL
#define RF_ANA1_PU_RCAL_POS                                     (14U)
#define RF_ANA1_PU_RCAL_LEN                                     (1U)
#define RF_ANA1_PU_RCAL_MSK                                     (((1U<<RF_ANA1_PU_RCAL_LEN)-1)<<RF_ANA1_PU_RCAL_POS)
#define RF_ANA1_PU_RCAL_UMSK                                    (~(((1U<<RF_ANA1_PU_RCAL_LEN)-1)<<RF_ANA1_PU_RCAL_POS))
#define RF_ANA1_PU_ADPLL                                        RF_ANA1_PU_ADPLL
#define RF_ANA1_PU_ADPLL_POS                                    (16U)
#define RF_ANA1_PU_ADPLL_LEN                                    (1U)
#define RF_ANA1_PU_ADPLL_MSK                                    (((1U<<RF_ANA1_PU_ADPLL_LEN)-1)<<RF_ANA1_PU_ADPLL_POS)
#define RF_ANA1_PU_ADPLL_UMSK                                   (~(((1U<<RF_ANA1_PU_ADPLL_LEN)-1)<<RF_ANA1_PU_ADPLL_POS))
#define RF_ANA1_PU_VCO                                          RF_ANA1_PU_VCO
#define RF_ANA1_PU_VCO_POS                                      (17U)
#define RF_ANA1_PU_VCO_LEN                                      (1U)
#define RF_ANA1_PU_VCO_MSK                                      (((1U<<RF_ANA1_PU_VCO_LEN)-1)<<RF_ANA1_PU_VCO_POS)
#define RF_ANA1_PU_VCO_UMSK                                     (~(((1U<<RF_ANA1_PU_VCO_LEN)-1)<<RF_ANA1_PU_VCO_POS))
#define RF_ANA1_PU_OSMX                                         RF_ANA1_PU_OSMX
#define RF_ANA1_PU_OSMX_POS                                     (18U)
#define RF_ANA1_PU_OSMX_LEN                                     (1U)
#define RF_ANA1_PU_OSMX_MSK                                     (((1U<<RF_ANA1_PU_OSMX_LEN)-1)<<RF_ANA1_PU_OSMX_POS)
#define RF_ANA1_PU_OSMX_UMSK                                    (~(((1U<<RF_ANA1_PU_OSMX_LEN)-1)<<RF_ANA1_PU_OSMX_POS))
#define RF_ANA1_PU_FBDIV                                        RF_ANA1_PU_FBDIV
#define RF_ANA1_PU_FBDIV_POS                                    (19U)
#define RF_ANA1_PU_FBDIV_LEN                                    (1U)
#define RF_ANA1_PU_FBDIV_MSK                                    (((1U<<RF_ANA1_PU_FBDIV_LEN)-1)<<RF_ANA1_PU_FBDIV_POS)
#define RF_ANA1_PU_FBDIV_UMSK                                   (~(((1U<<RF_ANA1_PU_FBDIV_LEN)-1)<<RF_ANA1_PU_FBDIV_POS))
#define RF_ANA1_PU_LOBUF                                        RF_ANA1_PU_LOBUF
#define RF_ANA1_PU_LOBUF_POS                                    (20U)
#define RF_ANA1_PU_LOBUF_LEN                                    (1U)
#define RF_ANA1_PU_LOBUF_MSK                                    (((1U<<RF_ANA1_PU_LOBUF_LEN)-1)<<RF_ANA1_PU_LOBUF_POS)
#define RF_ANA1_PU_LOBUF_UMSK                                   (~(((1U<<RF_ANA1_PU_LOBUF_LEN)-1)<<RF_ANA1_PU_LOBUF_POS))
#define RF_ANA1_PU_VBUF_LODIST                                  RF_ANA1_PU_VBUF_LODIST
#define RF_ANA1_PU_VBUF_LODIST_POS                              (21U)
#define RF_ANA1_PU_VBUF_LODIST_LEN                              (1U)
#define RF_ANA1_PU_VBUF_LODIST_MSK                              (((1U<<RF_ANA1_PU_VBUF_LODIST_LEN)-1)<<RF_ANA1_PU_VBUF_LODIST_POS)
#define RF_ANA1_PU_VBUF_LODIST_UMSK                             (~(((1U<<RF_ANA1_PU_VBUF_LODIST_LEN)-1)<<RF_ANA1_PU_VBUF_LODIST_POS))
#define RF_ANA1_PU_DTC                                          RF_ANA1_PU_DTC
#define RF_ANA1_PU_DTC_POS                                      (22U)
#define RF_ANA1_PU_DTC_LEN                                      (1U)
#define RF_ANA1_PU_DTC_MSK                                      (((1U<<RF_ANA1_PU_DTC_LEN)-1)<<RF_ANA1_PU_DTC_POS)
#define RF_ANA1_PU_DTC_UMSK                                     (~(((1U<<RF_ANA1_PU_DTC_LEN)-1)<<RF_ANA1_PU_DTC_POS))
#define RF_ANA1_PU_TDC                                          RF_ANA1_PU_TDC
#define RF_ANA1_PU_TDC_POS                                      (23U)
#define RF_ANA1_PU_TDC_LEN                                      (1U)
#define RF_ANA1_PU_TDC_MSK                                      (((1U<<RF_ANA1_PU_TDC_LEN)-1)<<RF_ANA1_PU_TDC_POS)
#define RF_ANA1_PU_TDC_UMSK                                     (~(((1U<<RF_ANA1_PU_TDC_LEN)-1)<<RF_ANA1_PU_TDC_POS))
#define RF_ANA1_PU_LPVCO                                        RF_ANA1_PU_LPVCO
#define RF_ANA1_PU_LPVCO_POS                                    (24U)
#define RF_ANA1_PU_LPVCO_LEN                                    (1U)
#define RF_ANA1_PU_LPVCO_MSK                                    (((1U<<RF_ANA1_PU_LPVCO_LEN)-1)<<RF_ANA1_PU_LPVCO_POS)
#define RF_ANA1_PU_LPVCO_UMSK                                   (~(((1U<<RF_ANA1_PU_LPVCO_LEN)-1)<<RF_ANA1_PU_LPVCO_POS))

/* 0x28 : read only from hardware logic */
#define RF_ANA1_PUCR_FSM_HW_OFFSET                              (0x28)
#define RF_ANA1_RXSW_EN_HW                                      RF_ANA1_RXSW_EN_HW
#define RF_ANA1_RXSW_EN_HW_POS                                  (0U)
#define RF_ANA1_RXSW_EN_HW_LEN                                  (2U)
#define RF_ANA1_RXSW_EN_HW_MSK                                  (((1U<<RF_ANA1_RXSW_EN_HW_LEN)-1)<<RF_ANA1_RXSW_EN_HW_POS)
#define RF_ANA1_RXSW_EN_HW_UMSK                                 (~(((1U<<RF_ANA1_RXSW_EN_HW_LEN)-1)<<RF_ANA1_RXSW_EN_HW_POS))
#define RF_ANA1_PU_LNA_HW                                       RF_ANA1_PU_LNA_HW
#define RF_ANA1_PU_LNA_HW_POS                                   (2U)
#define RF_ANA1_PU_LNA_HW_LEN                                   (1U)
#define RF_ANA1_PU_LNA_HW_MSK                                   (((1U<<RF_ANA1_PU_LNA_HW_LEN)-1)<<RF_ANA1_PU_LNA_HW_POS)
#define RF_ANA1_PU_LNA_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_LNA_HW_LEN)-1)<<RF_ANA1_PU_LNA_HW_POS))
#define RF_ANA1_PU_RMXGM_HW                                     RF_ANA1_PU_RMXGM_HW
#define RF_ANA1_PU_RMXGM_HW_POS                                 (3U)
#define RF_ANA1_PU_RMXGM_HW_LEN                                 (1U)
#define RF_ANA1_PU_RMXGM_HW_MSK                                 (((1U<<RF_ANA1_PU_RMXGM_HW_LEN)-1)<<RF_ANA1_PU_RMXGM_HW_POS)
#define RF_ANA1_PU_RMXGM_HW_UMSK                                (~(((1U<<RF_ANA1_PU_RMXGM_HW_LEN)-1)<<RF_ANA1_PU_RMXGM_HW_POS))
#define RF_ANA1_PU_RMX_HW                                       RF_ANA1_PU_RMX_HW
#define RF_ANA1_PU_RMX_HW_POS                                   (4U)
#define RF_ANA1_PU_RMX_HW_LEN                                   (1U)
#define RF_ANA1_PU_RMX_HW_MSK                                   (((1U<<RF_ANA1_PU_RMX_HW_LEN)-1)<<RF_ANA1_PU_RMX_HW_POS)
#define RF_ANA1_PU_RMX_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_RMX_HW_LEN)-1)<<RF_ANA1_PU_RMX_HW_POS))
#define RF_ANA1_PU_RBB_HW                                       RF_ANA1_PU_RBB_HW
#define RF_ANA1_PU_RBB_HW_POS                                   (5U)
#define RF_ANA1_PU_RBB_HW_LEN                                   (1U)
#define RF_ANA1_PU_RBB_HW_MSK                                   (((1U<<RF_ANA1_PU_RBB_HW_LEN)-1)<<RF_ANA1_PU_RBB_HW_POS)
#define RF_ANA1_PU_RBB_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_RBB_HW_LEN)-1)<<RF_ANA1_PU_RBB_HW_POS))
#define RF_ANA1_PU_RXADC_HW                                     RF_ANA1_PU_RXADC_HW
#define RF_ANA1_PU_RXADC_HW_POS                                 (6U)
#define RF_ANA1_PU_RXADC_HW_LEN                                 (1U)
#define RF_ANA1_PU_RXADC_HW_MSK                                 (((1U<<RF_ANA1_PU_RXADC_HW_LEN)-1)<<RF_ANA1_PU_RXADC_HW_POS)
#define RF_ANA1_PU_RXADC_HW_UMSK                                (~(((1U<<RF_ANA1_PU_RXADC_HW_LEN)-1)<<RF_ANA1_PU_RXADC_HW_POS))
#define RF_ANA1_RXADC_CLK_EN_HW                                 RF_ANA1_RXADC_CLK_EN_HW
#define RF_ANA1_RXADC_CLK_EN_HW_POS                             (7U)
#define RF_ANA1_RXADC_CLK_EN_HW_LEN                             (1U)
#define RF_ANA1_RXADC_CLK_EN_HW_MSK                             (((1U<<RF_ANA1_RXADC_CLK_EN_HW_LEN)-1)<<RF_ANA1_RXADC_CLK_EN_HW_POS)
#define RF_ANA1_RXADC_CLK_EN_HW_UMSK                            (~(((1U<<RF_ANA1_RXADC_CLK_EN_HW_LEN)-1)<<RF_ANA1_RXADC_CLK_EN_HW_POS))
#define RF_ANA1_PU_PA_2G_HW                                     RF_ANA1_PU_PA_2G_HW
#define RF_ANA1_PU_PA_2G_HW_POS                                 (8U)
#define RF_ANA1_PU_PA_2G_HW_LEN                                 (1U)
#define RF_ANA1_PU_PA_2G_HW_MSK                                 (((1U<<RF_ANA1_PU_PA_2G_HW_LEN)-1)<<RF_ANA1_PU_PA_2G_HW_POS)
#define RF_ANA1_PU_PA_2G_HW_UMSK                                (~(((1U<<RF_ANA1_PU_PA_2G_HW_LEN)-1)<<RF_ANA1_PU_PA_2G_HW_POS))
#define RF_ANA1_PU_TMX_2G_HW                                    RF_ANA1_PU_TMX_2G_HW
#define RF_ANA1_PU_TMX_2G_HW_POS                                (9U)
#define RF_ANA1_PU_TMX_2G_HW_LEN                                (1U)
#define RF_ANA1_PU_TMX_2G_HW_MSK                                (((1U<<RF_ANA1_PU_TMX_2G_HW_LEN)-1)<<RF_ANA1_PU_TMX_2G_HW_POS)
#define RF_ANA1_PU_TMX_2G_HW_UMSK                               (~(((1U<<RF_ANA1_PU_TMX_2G_HW_LEN)-1)<<RF_ANA1_PU_TMX_2G_HW_POS))
#define RF_ANA1_PU_PA_5G_HW                                     RF_ANA1_PU_PA_5G_HW
#define RF_ANA1_PU_PA_5G_HW_POS                                 (10U)
#define RF_ANA1_PU_PA_5G_HW_LEN                                 (1U)
#define RF_ANA1_PU_PA_5G_HW_MSK                                 (((1U<<RF_ANA1_PU_PA_5G_HW_LEN)-1)<<RF_ANA1_PU_PA_5G_HW_POS)
#define RF_ANA1_PU_PA_5G_HW_UMSK                                (~(((1U<<RF_ANA1_PU_PA_5G_HW_LEN)-1)<<RF_ANA1_PU_PA_5G_HW_POS))
#define RF_ANA1_PU_TMX_5G_HW                                    RF_ANA1_PU_TMX_5G_HW
#define RF_ANA1_PU_TMX_5G_HW_POS                                (11U)
#define RF_ANA1_PU_TMX_5G_HW_LEN                                (1U)
#define RF_ANA1_PU_TMX_5G_HW_MSK                                (((1U<<RF_ANA1_PU_TMX_5G_HW_LEN)-1)<<RF_ANA1_PU_TMX_5G_HW_POS)
#define RF_ANA1_PU_TMX_5G_HW_UMSK                               (~(((1U<<RF_ANA1_PU_TMX_5G_HW_LEN)-1)<<RF_ANA1_PU_TMX_5G_HW_POS))
#define RF_ANA1_PU_TBB_HW                                       RF_ANA1_PU_TBB_HW
#define RF_ANA1_PU_TBB_HW_POS                                   (12U)
#define RF_ANA1_PU_TBB_HW_LEN                                   (1U)
#define RF_ANA1_PU_TBB_HW_MSK                                   (((1U<<RF_ANA1_PU_TBB_HW_LEN)-1)<<RF_ANA1_PU_TBB_HW_POS)
#define RF_ANA1_PU_TBB_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_TBB_HW_LEN)-1)<<RF_ANA1_PU_TBB_HW_POS))
#define RF_ANA1_PU_DAC_HW                                       RF_ANA1_PU_DAC_HW
#define RF_ANA1_PU_DAC_HW_POS                                   (13U)
#define RF_ANA1_PU_DAC_HW_LEN                                   (1U)
#define RF_ANA1_PU_DAC_HW_MSK                                   (((1U<<RF_ANA1_PU_DAC_HW_LEN)-1)<<RF_ANA1_PU_DAC_HW_POS)
#define RF_ANA1_PU_DAC_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_DAC_HW_LEN)-1)<<RF_ANA1_PU_DAC_HW_POS))
#define RF_ANA1_PU_RCAL_HW                                      RF_ANA1_PU_RCAL_HW
#define RF_ANA1_PU_RCAL_HW_POS                                  (14U)
#define RF_ANA1_PU_RCAL_HW_LEN                                  (1U)
#define RF_ANA1_PU_RCAL_HW_MSK                                  (((1U<<RF_ANA1_PU_RCAL_HW_LEN)-1)<<RF_ANA1_PU_RCAL_HW_POS)
#define RF_ANA1_PU_RCAL_HW_UMSK                                 (~(((1U<<RF_ANA1_PU_RCAL_HW_LEN)-1)<<RF_ANA1_PU_RCAL_HW_POS))
#define RF_ANA1_PU_ADPLL_HW                                     RF_ANA1_PU_ADPLL_HW
#define RF_ANA1_PU_ADPLL_HW_POS                                 (16U)
#define RF_ANA1_PU_ADPLL_HW_LEN                                 (1U)
#define RF_ANA1_PU_ADPLL_HW_MSK                                 (((1U<<RF_ANA1_PU_ADPLL_HW_LEN)-1)<<RF_ANA1_PU_ADPLL_HW_POS)
#define RF_ANA1_PU_ADPLL_HW_UMSK                                (~(((1U<<RF_ANA1_PU_ADPLL_HW_LEN)-1)<<RF_ANA1_PU_ADPLL_HW_POS))
#define RF_ANA1_PU_VCO_HW                                       RF_ANA1_PU_VCO_HW
#define RF_ANA1_PU_VCO_HW_POS                                   (17U)
#define RF_ANA1_PU_VCO_HW_LEN                                   (1U)
#define RF_ANA1_PU_VCO_HW_MSK                                   (((1U<<RF_ANA1_PU_VCO_HW_LEN)-1)<<RF_ANA1_PU_VCO_HW_POS)
#define RF_ANA1_PU_VCO_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_VCO_HW_LEN)-1)<<RF_ANA1_PU_VCO_HW_POS))
#define RF_ANA1_PU_OSMX_HW                                      RF_ANA1_PU_OSMX_HW
#define RF_ANA1_PU_OSMX_HW_POS                                  (18U)
#define RF_ANA1_PU_OSMX_HW_LEN                                  (1U)
#define RF_ANA1_PU_OSMX_HW_MSK                                  (((1U<<RF_ANA1_PU_OSMX_HW_LEN)-1)<<RF_ANA1_PU_OSMX_HW_POS)
#define RF_ANA1_PU_OSMX_HW_UMSK                                 (~(((1U<<RF_ANA1_PU_OSMX_HW_LEN)-1)<<RF_ANA1_PU_OSMX_HW_POS))
#define RF_ANA1_PU_FBDIV_HW                                     RF_ANA1_PU_FBDIV_HW
#define RF_ANA1_PU_FBDIV_HW_POS                                 (19U)
#define RF_ANA1_PU_FBDIV_HW_LEN                                 (1U)
#define RF_ANA1_PU_FBDIV_HW_MSK                                 (((1U<<RF_ANA1_PU_FBDIV_HW_LEN)-1)<<RF_ANA1_PU_FBDIV_HW_POS)
#define RF_ANA1_PU_FBDIV_HW_UMSK                                (~(((1U<<RF_ANA1_PU_FBDIV_HW_LEN)-1)<<RF_ANA1_PU_FBDIV_HW_POS))
#define RF_ANA1_PU_LOBUF_HW                                     RF_ANA1_PU_LOBUF_HW
#define RF_ANA1_PU_LOBUF_HW_POS                                 (20U)
#define RF_ANA1_PU_LOBUF_HW_LEN                                 (1U)
#define RF_ANA1_PU_LOBUF_HW_MSK                                 (((1U<<RF_ANA1_PU_LOBUF_HW_LEN)-1)<<RF_ANA1_PU_LOBUF_HW_POS)
#define RF_ANA1_PU_LOBUF_HW_UMSK                                (~(((1U<<RF_ANA1_PU_LOBUF_HW_LEN)-1)<<RF_ANA1_PU_LOBUF_HW_POS))
#define RF_ANA1_PU_VBUF_LODIST_HW                               RF_ANA1_PU_VBUF_LODIST_HW
#define RF_ANA1_PU_VBUF_LODIST_HW_POS                           (21U)
#define RF_ANA1_PU_VBUF_LODIST_HW_LEN                           (1U)
#define RF_ANA1_PU_VBUF_LODIST_HW_MSK                           (((1U<<RF_ANA1_PU_VBUF_LODIST_HW_LEN)-1)<<RF_ANA1_PU_VBUF_LODIST_HW_POS)
#define RF_ANA1_PU_VBUF_LODIST_HW_UMSK                          (~(((1U<<RF_ANA1_PU_VBUF_LODIST_HW_LEN)-1)<<RF_ANA1_PU_VBUF_LODIST_HW_POS))
#define RF_ANA1_PU_DTC_HW                                       RF_ANA1_PU_DTC_HW
#define RF_ANA1_PU_DTC_HW_POS                                   (22U)
#define RF_ANA1_PU_DTC_HW_LEN                                   (1U)
#define RF_ANA1_PU_DTC_HW_MSK                                   (((1U<<RF_ANA1_PU_DTC_HW_LEN)-1)<<RF_ANA1_PU_DTC_HW_POS)
#define RF_ANA1_PU_DTC_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_DTC_HW_LEN)-1)<<RF_ANA1_PU_DTC_HW_POS))
#define RF_ANA1_PU_TDC_HW                                       RF_ANA1_PU_TDC_HW
#define RF_ANA1_PU_TDC_HW_POS                                   (23U)
#define RF_ANA1_PU_TDC_HW_LEN                                   (1U)
#define RF_ANA1_PU_TDC_HW_MSK                                   (((1U<<RF_ANA1_PU_TDC_HW_LEN)-1)<<RF_ANA1_PU_TDC_HW_POS)
#define RF_ANA1_PU_TDC_HW_UMSK                                  (~(((1U<<RF_ANA1_PU_TDC_HW_LEN)-1)<<RF_ANA1_PU_TDC_HW_POS))
#define RF_ANA1_PU_LPVCO_HW                                     RF_ANA1_PU_LPVCO_HW
#define RF_ANA1_PU_LPVCO_HW_POS                                 (24U)
#define RF_ANA1_PU_LPVCO_HW_LEN                                 (1U)
#define RF_ANA1_PU_LPVCO_HW_MSK                                 (((1U<<RF_ANA1_PU_LPVCO_HW_LEN)-1)<<RF_ANA1_PU_LPVCO_HW_POS)
#define RF_ANA1_PU_LPVCO_HW_UMSK                                (~(((1U<<RF_ANA1_PU_LPVCO_HW_LEN)-1)<<RF_ANA1_PU_LPVCO_HW_POS))

/* 0x2C : ppu_pud_hw */
#define RF_ANA1_PPU_PUD_HW_OFFSET                               (0x2C)
#define RF_ANA1_PPU_LNA_HW                                      RF_ANA1_PPU_LNA_HW
#define RF_ANA1_PPU_LNA_HW_POS                                  (0U)
#define RF_ANA1_PPU_LNA_HW_LEN                                  (1U)
#define RF_ANA1_PPU_LNA_HW_MSK                                  (((1U<<RF_ANA1_PPU_LNA_HW_LEN)-1)<<RF_ANA1_PPU_LNA_HW_POS)
#define RF_ANA1_PPU_LNA_HW_UMSK                                 (~(((1U<<RF_ANA1_PPU_LNA_HW_LEN)-1)<<RF_ANA1_PPU_LNA_HW_POS))
#define RF_ANA1_PPU_RMXGM_HW                                    RF_ANA1_PPU_RMXGM_HW
#define RF_ANA1_PPU_RMXGM_HW_POS                                (1U)
#define RF_ANA1_PPU_RMXGM_HW_LEN                                (1U)
#define RF_ANA1_PPU_RMXGM_HW_MSK                                (((1U<<RF_ANA1_PPU_RMXGM_HW_LEN)-1)<<RF_ANA1_PPU_RMXGM_HW_POS)
#define RF_ANA1_PPU_RMXGM_HW_UMSK                               (~(((1U<<RF_ANA1_PPU_RMXGM_HW_LEN)-1)<<RF_ANA1_PPU_RMXGM_HW_POS))
#define RF_ANA1_PPU_RBB_HW                                      RF_ANA1_PPU_RBB_HW
#define RF_ANA1_PPU_RBB_HW_POS                                  (2U)
#define RF_ANA1_PPU_RBB_HW_LEN                                  (1U)
#define RF_ANA1_PPU_RBB_HW_MSK                                  (((1U<<RF_ANA1_PPU_RBB_HW_LEN)-1)<<RF_ANA1_PPU_RBB_HW_POS)
#define RF_ANA1_PPU_RBB_HW_UMSK                                 (~(((1U<<RF_ANA1_PPU_RBB_HW_LEN)-1)<<RF_ANA1_PPU_RBB_HW_POS))
#define RF_ANA1_PPU_RXADC_HW                                    RF_ANA1_PPU_RXADC_HW
#define RF_ANA1_PPU_RXADC_HW_POS                                (3U)
#define RF_ANA1_PPU_RXADC_HW_LEN                                (1U)
#define RF_ANA1_PPU_RXADC_HW_MSK                                (((1U<<RF_ANA1_PPU_RXADC_HW_LEN)-1)<<RF_ANA1_PPU_RXADC_HW_POS)
#define RF_ANA1_PPU_RXADC_HW_UMSK                               (~(((1U<<RF_ANA1_PPU_RXADC_HW_LEN)-1)<<RF_ANA1_PPU_RXADC_HW_POS))
#define RF_ANA1_PPU_VCO_HW                                      RF_ANA1_PPU_VCO_HW
#define RF_ANA1_PPU_VCO_HW_POS                                  (4U)
#define RF_ANA1_PPU_VCO_HW_LEN                                  (1U)
#define RF_ANA1_PPU_VCO_HW_MSK                                  (((1U<<RF_ANA1_PPU_VCO_HW_LEN)-1)<<RF_ANA1_PPU_VCO_HW_POS)
#define RF_ANA1_PPU_VCO_HW_UMSK                                 (~(((1U<<RF_ANA1_PPU_VCO_HW_LEN)-1)<<RF_ANA1_PPU_VCO_HW_POS))
#define RF_ANA1_PPU_OSMX_HW                                     RF_ANA1_PPU_OSMX_HW
#define RF_ANA1_PPU_OSMX_HW_POS                                 (5U)
#define RF_ANA1_PPU_OSMX_HW_LEN                                 (1U)
#define RF_ANA1_PPU_OSMX_HW_MSK                                 (((1U<<RF_ANA1_PPU_OSMX_HW_LEN)-1)<<RF_ANA1_PPU_OSMX_HW_POS)
#define RF_ANA1_PPU_OSMX_HW_UMSK                                (~(((1U<<RF_ANA1_PPU_OSMX_HW_LEN)-1)<<RF_ANA1_PPU_OSMX_HW_POS))
#define RF_ANA1_PPU_FBDIV_HW                                    RF_ANA1_PPU_FBDIV_HW
#define RF_ANA1_PPU_FBDIV_HW_POS                                (6U)
#define RF_ANA1_PPU_FBDIV_HW_LEN                                (1U)
#define RF_ANA1_PPU_FBDIV_HW_MSK                                (((1U<<RF_ANA1_PPU_FBDIV_HW_LEN)-1)<<RF_ANA1_PPU_FBDIV_HW_POS)
#define RF_ANA1_PPU_FBDIV_HW_UMSK                               (~(((1U<<RF_ANA1_PPU_FBDIV_HW_LEN)-1)<<RF_ANA1_PPU_FBDIV_HW_POS))
#define RF_ANA1_PPU_LOBUF_HW                                    RF_ANA1_PPU_LOBUF_HW
#define RF_ANA1_PPU_LOBUF_HW_POS                                (7U)
#define RF_ANA1_PPU_LOBUF_HW_LEN                                (1U)
#define RF_ANA1_PPU_LOBUF_HW_MSK                                (((1U<<RF_ANA1_PPU_LOBUF_HW_LEN)-1)<<RF_ANA1_PPU_LOBUF_HW_POS)
#define RF_ANA1_PPU_LOBUF_HW_UMSK                               (~(((1U<<RF_ANA1_PPU_LOBUF_HW_LEN)-1)<<RF_ANA1_PPU_LOBUF_HW_POS))
#define RF_ANA1_PPU_VBUF_LODIST_HW                              RF_ANA1_PPU_VBUF_LODIST_HW
#define RF_ANA1_PPU_VBUF_LODIST_HW_POS                          (8U)
#define RF_ANA1_PPU_VBUF_LODIST_HW_LEN                          (1U)
#define RF_ANA1_PPU_VBUF_LODIST_HW_MSK                          (((1U<<RF_ANA1_PPU_VBUF_LODIST_HW_LEN)-1)<<RF_ANA1_PPU_VBUF_LODIST_HW_POS)
#define RF_ANA1_PPU_VBUF_LODIST_HW_UMSK                         (~(((1U<<RF_ANA1_PPU_VBUF_LODIST_HW_LEN)-1)<<RF_ANA1_PPU_VBUF_LODIST_HW_POS))
#define RF_ANA1_PPU_DTC_HW                                      RF_ANA1_PPU_DTC_HW
#define RF_ANA1_PPU_DTC_HW_POS                                  (9U)
#define RF_ANA1_PPU_DTC_HW_LEN                                  (1U)
#define RF_ANA1_PPU_DTC_HW_MSK                                  (((1U<<RF_ANA1_PPU_DTC_HW_LEN)-1)<<RF_ANA1_PPU_DTC_HW_POS)
#define RF_ANA1_PPU_DTC_HW_UMSK                                 (~(((1U<<RF_ANA1_PPU_DTC_HW_LEN)-1)<<RF_ANA1_PPU_DTC_HW_POS))
#define RF_ANA1_PPU_TDC_HW                                      RF_ANA1_PPU_TDC_HW
#define RF_ANA1_PPU_TDC_HW_POS                                  (10U)
#define RF_ANA1_PPU_TDC_HW_LEN                                  (1U)
#define RF_ANA1_PPU_TDC_HW_MSK                                  (((1U<<RF_ANA1_PPU_TDC_HW_LEN)-1)<<RF_ANA1_PPU_TDC_HW_POS)
#define RF_ANA1_PPU_TDC_HW_UMSK                                 (~(((1U<<RF_ANA1_PPU_TDC_HW_LEN)-1)<<RF_ANA1_PPU_TDC_HW_POS))
#define RF_ANA1_PPU_LPVCO_HW                                    RF_ANA1_PPU_LPVCO_HW
#define RF_ANA1_PPU_LPVCO_HW_POS                                (11U)
#define RF_ANA1_PPU_LPVCO_HW_LEN                                (1U)
#define RF_ANA1_PPU_LPVCO_HW_MSK                                (((1U<<RF_ANA1_PPU_LPVCO_HW_LEN)-1)<<RF_ANA1_PPU_LPVCO_HW_POS)
#define RF_ANA1_PPU_LPVCO_HW_UMSK                               (~(((1U<<RF_ANA1_PPU_LPVCO_HW_LEN)-1)<<RF_ANA1_PPU_LPVCO_HW_POS))
#define RF_ANA1_PUD_VCO_HW                                      RF_ANA1_PUD_VCO_HW
#define RF_ANA1_PUD_VCO_HW_POS                                  (16U)
#define RF_ANA1_PUD_VCO_HW_LEN                                  (1U)
#define RF_ANA1_PUD_VCO_HW_MSK                                  (((1U<<RF_ANA1_PUD_VCO_HW_LEN)-1)<<RF_ANA1_PUD_VCO_HW_POS)
#define RF_ANA1_PUD_VCO_HW_UMSK                                 (~(((1U<<RF_ANA1_PUD_VCO_HW_LEN)-1)<<RF_ANA1_PUD_VCO_HW_POS))
#define RF_ANA1_PUD_DTC_HW                                      RF_ANA1_PUD_DTC_HW
#define RF_ANA1_PUD_DTC_HW_POS                                  (17U)
#define RF_ANA1_PUD_DTC_HW_LEN                                  (1U)
#define RF_ANA1_PUD_DTC_HW_MSK                                  (((1U<<RF_ANA1_PUD_DTC_HW_LEN)-1)<<RF_ANA1_PUD_DTC_HW_POS)
#define RF_ANA1_PUD_DTC_HW_UMSK                                 (~(((1U<<RF_ANA1_PUD_DTC_HW_LEN)-1)<<RF_ANA1_PUD_DTC_HW_POS))
#define RF_ANA1_PUD_LPVCO_HW                                    RF_ANA1_PUD_LPVCO_HW
#define RF_ANA1_PUD_LPVCO_HW_POS                                (18U)
#define RF_ANA1_PUD_LPVCO_HW_LEN                                (1U)
#define RF_ANA1_PUD_LPVCO_HW_MSK                                (((1U<<RF_ANA1_PUD_LPVCO_HW_LEN)-1)<<RF_ANA1_PUD_LPVCO_HW_POS)
#define RF_ANA1_PUD_LPVCO_HW_UMSK                               (~(((1U<<RF_ANA1_PUD_LPVCO_HW_LEN)-1)<<RF_ANA1_PUD_LPVCO_HW_POS))

/* 0x30 : gain control1 */
#define RF_ANA1_TRX_GAIN1_OFFSET                                (0x30)
#define RF_ANA1_GC_LNA                                          RF_ANA1_GC_LNA
#define RF_ANA1_GC_LNA_POS                                      (0U)
#define RF_ANA1_GC_LNA_LEN                                      (3U)
#define RF_ANA1_GC_LNA_MSK                                      (((1U<<RF_ANA1_GC_LNA_LEN)-1)<<RF_ANA1_GC_LNA_POS)
#define RF_ANA1_GC_LNA_UMSK                                     (~(((1U<<RF_ANA1_GC_LNA_LEN)-1)<<RF_ANA1_GC_LNA_POS))
#define RF_ANA1_GC_RMXGM                                        RF_ANA1_GC_RMXGM
#define RF_ANA1_GC_RMXGM_POS                                    (3U)
#define RF_ANA1_GC_RMXGM_LEN                                    (2U)
#define RF_ANA1_GC_RMXGM_MSK                                    (((1U<<RF_ANA1_GC_RMXGM_LEN)-1)<<RF_ANA1_GC_RMXGM_POS)
#define RF_ANA1_GC_RMXGM_UMSK                                   (~(((1U<<RF_ANA1_GC_RMXGM_LEN)-1)<<RF_ANA1_GC_RMXGM_POS))
#define RF_ANA1_GC_RBB1                                         RF_ANA1_GC_RBB1
#define RF_ANA1_GC_RBB1_POS                                     (5U)
#define RF_ANA1_GC_RBB1_LEN                                     (2U)
#define RF_ANA1_GC_RBB1_MSK                                     (((1U<<RF_ANA1_GC_RBB1_LEN)-1)<<RF_ANA1_GC_RBB1_POS)
#define RF_ANA1_GC_RBB1_UMSK                                    (~(((1U<<RF_ANA1_GC_RBB1_LEN)-1)<<RF_ANA1_GC_RBB1_POS))
#define RF_ANA1_GC_RBB2                                         RF_ANA1_GC_RBB2
#define RF_ANA1_GC_RBB2_POS                                     (7U)
#define RF_ANA1_GC_RBB2_LEN                                     (3U)
#define RF_ANA1_GC_RBB2_MSK                                     (((1U<<RF_ANA1_GC_RBB2_LEN)-1)<<RF_ANA1_GC_RBB2_POS)
#define RF_ANA1_GC_RBB2_UMSK                                    (~(((1U<<RF_ANA1_GC_RBB2_LEN)-1)<<RF_ANA1_GC_RBB2_POS))
#define RF_ANA1_GC_PA                                           RF_ANA1_GC_PA
#define RF_ANA1_GC_PA_POS                                       (10U)
#define RF_ANA1_GC_PA_LEN                                       (2U)
#define RF_ANA1_GC_PA_MSK                                       (((1U<<RF_ANA1_GC_PA_LEN)-1)<<RF_ANA1_GC_PA_POS)
#define RF_ANA1_GC_PA_UMSK                                      (~(((1U<<RF_ANA1_GC_PA_LEN)-1)<<RF_ANA1_GC_PA_POS))
#define RF_ANA1_GC_TMX                                          RF_ANA1_GC_TMX
#define RF_ANA1_GC_TMX_POS                                      (12U)
#define RF_ANA1_GC_TMX_LEN                                      (4U)
#define RF_ANA1_GC_TMX_MSK                                      (((1U<<RF_ANA1_GC_TMX_LEN)-1)<<RF_ANA1_GC_TMX_POS)
#define RF_ANA1_GC_TMX_UMSK                                     (~(((1U<<RF_ANA1_GC_TMX_LEN)-1)<<RF_ANA1_GC_TMX_POS))
#define RF_ANA1_GC_TBB                                          RF_ANA1_GC_TBB
#define RF_ANA1_GC_TBB_POS                                      (20U)
#define RF_ANA1_GC_TBB_LEN                                      (6U)
#define RF_ANA1_GC_TBB_MSK                                      (((1U<<RF_ANA1_GC_TBB_LEN)-1)<<RF_ANA1_GC_TBB_POS)
#define RF_ANA1_GC_TBB_UMSK                                     (~(((1U<<RF_ANA1_GC_TBB_LEN)-1)<<RF_ANA1_GC_TBB_POS))
#define RF_ANA1_GC_TBB_BOOST                                    RF_ANA1_GC_TBB_BOOST
#define RF_ANA1_GC_TBB_BOOST_POS                                (27U)
#define RF_ANA1_GC_TBB_BOOST_LEN                                (2U)
#define RF_ANA1_GC_TBB_BOOST_MSK                                (((1U<<RF_ANA1_GC_TBB_BOOST_LEN)-1)<<RF_ANA1_GC_TBB_BOOST_POS)
#define RF_ANA1_GC_TBB_BOOST_UMSK                               (~(((1U<<RF_ANA1_GC_TBB_BOOST_LEN)-1)<<RF_ANA1_GC_TBB_BOOST_POS))

/* 0x34 : trx gain hardware readback */
#define RF_ANA1_TRX_GAIN_HW_OFFSET                              (0x34)
#define RF_ANA1_GC_LNA_HW                                       RF_ANA1_GC_LNA_HW
#define RF_ANA1_GC_LNA_HW_POS                                   (0U)
#define RF_ANA1_GC_LNA_HW_LEN                                   (3U)
#define RF_ANA1_GC_LNA_HW_MSK                                   (((1U<<RF_ANA1_GC_LNA_HW_LEN)-1)<<RF_ANA1_GC_LNA_HW_POS)
#define RF_ANA1_GC_LNA_HW_UMSK                                  (~(((1U<<RF_ANA1_GC_LNA_HW_LEN)-1)<<RF_ANA1_GC_LNA_HW_POS))
#define RF_ANA1_GC_RMXGM_HW                                     RF_ANA1_GC_RMXGM_HW
#define RF_ANA1_GC_RMXGM_HW_POS                                 (3U)
#define RF_ANA1_GC_RMXGM_HW_LEN                                 (2U)
#define RF_ANA1_GC_RMXGM_HW_MSK                                 (((1U<<RF_ANA1_GC_RMXGM_HW_LEN)-1)<<RF_ANA1_GC_RMXGM_HW_POS)
#define RF_ANA1_GC_RMXGM_HW_UMSK                                (~(((1U<<RF_ANA1_GC_RMXGM_HW_LEN)-1)<<RF_ANA1_GC_RMXGM_HW_POS))
#define RF_ANA1_GC_RBB1_HW                                      RF_ANA1_GC_RBB1_HW
#define RF_ANA1_GC_RBB1_HW_POS                                  (5U)
#define RF_ANA1_GC_RBB1_HW_LEN                                  (2U)
#define RF_ANA1_GC_RBB1_HW_MSK                                  (((1U<<RF_ANA1_GC_RBB1_HW_LEN)-1)<<RF_ANA1_GC_RBB1_HW_POS)
#define RF_ANA1_GC_RBB1_HW_UMSK                                 (~(((1U<<RF_ANA1_GC_RBB1_HW_LEN)-1)<<RF_ANA1_GC_RBB1_HW_POS))
#define RF_ANA1_GC_RBB2_HW                                      RF_ANA1_GC_RBB2_HW
#define RF_ANA1_GC_RBB2_HW_POS                                  (7U)
#define RF_ANA1_GC_RBB2_HW_LEN                                  (3U)
#define RF_ANA1_GC_RBB2_HW_MSK                                  (((1U<<RF_ANA1_GC_RBB2_HW_LEN)-1)<<RF_ANA1_GC_RBB2_HW_POS)
#define RF_ANA1_GC_RBB2_HW_UMSK                                 (~(((1U<<RF_ANA1_GC_RBB2_HW_LEN)-1)<<RF_ANA1_GC_RBB2_HW_POS))
#define RF_ANA1_GC_PA_HW                                        RF_ANA1_GC_PA_HW
#define RF_ANA1_GC_PA_HW_POS                                    (10U)
#define RF_ANA1_GC_PA_HW_LEN                                    (2U)
#define RF_ANA1_GC_PA_HW_MSK                                    (((1U<<RF_ANA1_GC_PA_HW_LEN)-1)<<RF_ANA1_GC_PA_HW_POS)
#define RF_ANA1_GC_PA_HW_UMSK                                   (~(((1U<<RF_ANA1_GC_PA_HW_LEN)-1)<<RF_ANA1_GC_PA_HW_POS))
#define RF_ANA1_GC_TMX_HW                                       RF_ANA1_GC_TMX_HW
#define RF_ANA1_GC_TMX_HW_POS                                   (12U)
#define RF_ANA1_GC_TMX_HW_LEN                                   (4U)
#define RF_ANA1_GC_TMX_HW_MSK                                   (((1U<<RF_ANA1_GC_TMX_HW_LEN)-1)<<RF_ANA1_GC_TMX_HW_POS)
#define RF_ANA1_GC_TMX_HW_UMSK                                  (~(((1U<<RF_ANA1_GC_TMX_HW_LEN)-1)<<RF_ANA1_GC_TMX_HW_POS))
#define RF_ANA1_GC_TBB_HW                                       RF_ANA1_GC_TBB_HW
#define RF_ANA1_GC_TBB_HW_POS                                   (20U)
#define RF_ANA1_GC_TBB_HW_LEN                                   (6U)
#define RF_ANA1_GC_TBB_HW_MSK                                   (((1U<<RF_ANA1_GC_TBB_HW_LEN)-1)<<RF_ANA1_GC_TBB_HW_POS)
#define RF_ANA1_GC_TBB_HW_UMSK                                  (~(((1U<<RF_ANA1_GC_TBB_HW_LEN)-1)<<RF_ANA1_GC_TBB_HW_POS))
#define RF_ANA1_GC_TBB_BOOST_HW                                 RF_ANA1_GC_TBB_BOOST_HW
#define RF_ANA1_GC_TBB_BOOST_HW_POS                             (27U)
#define RF_ANA1_GC_TBB_BOOST_HW_LEN                             (2U)
#define RF_ANA1_GC_TBB_BOOST_HW_MSK                             (((1U<<RF_ANA1_GC_TBB_BOOST_HW_LEN)-1)<<RF_ANA1_GC_TBB_BOOST_HW_POS)
#define RF_ANA1_GC_TBB_BOOST_HW_UMSK                            (~(((1U<<RF_ANA1_GC_TBB_BOOST_HW_LEN)-1)<<RF_ANA1_GC_TBB_BOOST_HW_POS))

/* 0x38 : dc test register */
#define RF_ANA1_TEN_DC_OFFSET                                   (0x38)
#define RF_ANA1_ATEST_DC_TP_12_INT_EN                           RF_ANA1_ATEST_DC_TP_12_INT_EN
#define RF_ANA1_ATEST_DC_TP_12_INT_EN_POS                       (0U)
#define RF_ANA1_ATEST_DC_TP_12_INT_EN_LEN                       (1U)
#define RF_ANA1_ATEST_DC_TP_12_INT_EN_MSK                       (((1U<<RF_ANA1_ATEST_DC_TP_12_INT_EN_LEN)-1)<<RF_ANA1_ATEST_DC_TP_12_INT_EN_POS)
#define RF_ANA1_ATEST_DC_TP_12_INT_EN_UMSK                      (~(((1U<<RF_ANA1_ATEST_DC_TP_12_INT_EN_LEN)-1)<<RF_ANA1_ATEST_DC_TP_12_INT_EN_POS))
#define RF_ANA1_ATEST_DC_TP_12_EN                               RF_ANA1_ATEST_DC_TP_12_EN
#define RF_ANA1_ATEST_DC_TP_12_EN_POS                           (1U)
#define RF_ANA1_ATEST_DC_TP_12_EN_LEN                           (1U)
#define RF_ANA1_ATEST_DC_TP_12_EN_MSK                           (((1U<<RF_ANA1_ATEST_DC_TP_12_EN_LEN)-1)<<RF_ANA1_ATEST_DC_TP_12_EN_POS)
#define RF_ANA1_ATEST_DC_TP_12_EN_UMSK                          (~(((1U<<RF_ANA1_ATEST_DC_TP_12_EN_LEN)-1)<<RF_ANA1_ATEST_DC_TP_12_EN_POS))
#define RF_ANA1_ATEST_DC_TP_18_EN                               RF_ANA1_ATEST_DC_TP_18_EN
#define RF_ANA1_ATEST_DC_TP_18_EN_POS                           (2U)
#define RF_ANA1_ATEST_DC_TP_18_EN_LEN                           (1U)
#define RF_ANA1_ATEST_DC_TP_18_EN_MSK                           (((1U<<RF_ANA1_ATEST_DC_TP_18_EN_LEN)-1)<<RF_ANA1_ATEST_DC_TP_18_EN_POS)
#define RF_ANA1_ATEST_DC_TP_18_EN_UMSK                          (~(((1U<<RF_ANA1_ATEST_DC_TP_18_EN_LEN)-1)<<RF_ANA1_ATEST_DC_TP_18_EN_POS))
#define RF_ANA1_TEN_ATEST                                       RF_ANA1_TEN_ATEST
#define RF_ANA1_TEN_ATEST_POS                                   (3U)
#define RF_ANA1_TEN_ATEST_LEN                                   (1U)
#define RF_ANA1_TEN_ATEST_MSK                                   (((1U<<RF_ANA1_TEN_ATEST_LEN)-1)<<RF_ANA1_TEN_ATEST_POS)
#define RF_ANA1_TEN_ATEST_UMSK                                  (~(((1U<<RF_ANA1_TEN_ATEST_LEN)-1)<<RF_ANA1_TEN_ATEST_POS))
#define RF_ANA1_TEN_RRF_2G_LNA                                  RF_ANA1_TEN_RRF_2G_LNA
#define RF_ANA1_TEN_RRF_2G_LNA_POS                              (4U)
#define RF_ANA1_TEN_RRF_2G_LNA_LEN                              (1U)
#define RF_ANA1_TEN_RRF_2G_LNA_MSK                              (((1U<<RF_ANA1_TEN_RRF_2G_LNA_LEN)-1)<<RF_ANA1_TEN_RRF_2G_LNA_POS)
#define RF_ANA1_TEN_RRF_2G_LNA_UMSK                             (~(((1U<<RF_ANA1_TEN_RRF_2G_LNA_LEN)-1)<<RF_ANA1_TEN_RRF_2G_LNA_POS))
#define RF_ANA1_TEN_RRF_5G_LNA                                  RF_ANA1_TEN_RRF_5G_LNA
#define RF_ANA1_TEN_RRF_5G_LNA_POS                              (5U)
#define RF_ANA1_TEN_RRF_5G_LNA_LEN                              (1U)
#define RF_ANA1_TEN_RRF_5G_LNA_MSK                              (((1U<<RF_ANA1_TEN_RRF_5G_LNA_LEN)-1)<<RF_ANA1_TEN_RRF_5G_LNA_POS)
#define RF_ANA1_TEN_RRF_5G_LNA_UMSK                             (~(((1U<<RF_ANA1_TEN_RRF_5G_LNA_LEN)-1)<<RF_ANA1_TEN_RRF_5G_LNA_POS))
#define RF_ANA1_TEN_RRF_RMXGM                                   RF_ANA1_TEN_RRF_RMXGM
#define RF_ANA1_TEN_RRF_RMXGM_POS                               (6U)
#define RF_ANA1_TEN_RRF_RMXGM_LEN                               (1U)
#define RF_ANA1_TEN_RRF_RMXGM_MSK                               (((1U<<RF_ANA1_TEN_RRF_RMXGM_LEN)-1)<<RF_ANA1_TEN_RRF_RMXGM_POS)
#define RF_ANA1_TEN_RRF_RMXGM_UMSK                              (~(((1U<<RF_ANA1_TEN_RRF_RMXGM_LEN)-1)<<RF_ANA1_TEN_RRF_RMXGM_POS))
#define RF_ANA1_TEN_RRF_RMX                                     RF_ANA1_TEN_RRF_RMX
#define RF_ANA1_TEN_RRF_RMX_POS                                 (7U)
#define RF_ANA1_TEN_RRF_RMX_LEN                                 (1U)
#define RF_ANA1_TEN_RRF_RMX_MSK                                 (((1U<<RF_ANA1_TEN_RRF_RMX_LEN)-1)<<RF_ANA1_TEN_RRF_RMX_POS)
#define RF_ANA1_TEN_RRF_RMX_UMSK                                (~(((1U<<RF_ANA1_TEN_RRF_RMX_LEN)-1)<<RF_ANA1_TEN_RRF_RMX_POS))
#define RF_ANA1_TEN_TIA_0                                       RF_ANA1_TEN_TIA_0
#define RF_ANA1_TEN_TIA_0_POS                                   (8U)
#define RF_ANA1_TEN_TIA_0_LEN                                   (1U)
#define RF_ANA1_TEN_TIA_0_MSK                                   (((1U<<RF_ANA1_TEN_TIA_0_LEN)-1)<<RF_ANA1_TEN_TIA_0_POS)
#define RF_ANA1_TEN_TIA_0_UMSK                                  (~(((1U<<RF_ANA1_TEN_TIA_0_LEN)-1)<<RF_ANA1_TEN_TIA_0_POS))
#define RF_ANA1_TEN_TIA_1                                       RF_ANA1_TEN_TIA_1
#define RF_ANA1_TEN_TIA_1_POS                                   (9U)
#define RF_ANA1_TEN_TIA_1_LEN                                   (1U)
#define RF_ANA1_TEN_TIA_1_MSK                                   (((1U<<RF_ANA1_TEN_TIA_1_LEN)-1)<<RF_ANA1_TEN_TIA_1_POS)
#define RF_ANA1_TEN_TIA_1_UMSK                                  (~(((1U<<RF_ANA1_TEN_TIA_1_LEN)-1)<<RF_ANA1_TEN_TIA_1_POS))
#define RF_ANA1_TEN_BQ_0                                        RF_ANA1_TEN_BQ_0
#define RF_ANA1_TEN_BQ_0_POS                                    (10U)
#define RF_ANA1_TEN_BQ_0_LEN                                    (1U)
#define RF_ANA1_TEN_BQ_0_MSK                                    (((1U<<RF_ANA1_TEN_BQ_0_LEN)-1)<<RF_ANA1_TEN_BQ_0_POS)
#define RF_ANA1_TEN_BQ_0_UMSK                                   (~(((1U<<RF_ANA1_TEN_BQ_0_LEN)-1)<<RF_ANA1_TEN_BQ_0_POS))
#define RF_ANA1_TEN_BQ_1                                        RF_ANA1_TEN_BQ_1
#define RF_ANA1_TEN_BQ_1_POS                                    (11U)
#define RF_ANA1_TEN_BQ_1_LEN                                    (1U)
#define RF_ANA1_TEN_BQ_1_MSK                                    (((1U<<RF_ANA1_TEN_BQ_1_LEN)-1)<<RF_ANA1_TEN_BQ_1_POS)
#define RF_ANA1_TEN_BQ_1_UMSK                                   (~(((1U<<RF_ANA1_TEN_BQ_1_LEN)-1)<<RF_ANA1_TEN_BQ_1_POS))
#define RF_ANA1_TEN_RXADC                                       RF_ANA1_TEN_RXADC
#define RF_ANA1_TEN_RXADC_POS                                   (12U)
#define RF_ANA1_TEN_RXADC_LEN                                   (1U)
#define RF_ANA1_TEN_RXADC_MSK                                   (((1U<<RF_ANA1_TEN_RXADC_LEN)-1)<<RF_ANA1_TEN_RXADC_POS)
#define RF_ANA1_TEN_RXADC_UMSK                                  (~(((1U<<RF_ANA1_TEN_RXADC_LEN)-1)<<RF_ANA1_TEN_RXADC_POS))
#define RF_ANA1_TEN_PA_2G                                       RF_ANA1_TEN_PA_2G
#define RF_ANA1_TEN_PA_2G_POS                                   (16U)
#define RF_ANA1_TEN_PA_2G_LEN                                   (1U)
#define RF_ANA1_TEN_PA_2G_MSK                                   (((1U<<RF_ANA1_TEN_PA_2G_LEN)-1)<<RF_ANA1_TEN_PA_2G_POS)
#define RF_ANA1_TEN_PA_2G_UMSK                                  (~(((1U<<RF_ANA1_TEN_PA_2G_LEN)-1)<<RF_ANA1_TEN_PA_2G_POS))
#define RF_ANA1_TEN_PA_5G                                       RF_ANA1_TEN_PA_5G
#define RF_ANA1_TEN_PA_5G_POS                                   (17U)
#define RF_ANA1_TEN_PA_5G_LEN                                   (1U)
#define RF_ANA1_TEN_PA_5G_MSK                                   (((1U<<RF_ANA1_TEN_PA_5G_LEN)-1)<<RF_ANA1_TEN_PA_5G_POS)
#define RF_ANA1_TEN_PA_5G_UMSK                                  (~(((1U<<RF_ANA1_TEN_PA_5G_LEN)-1)<<RF_ANA1_TEN_PA_5G_POS))
#define RF_ANA1_TEN_PWRDET_2G                                   RF_ANA1_TEN_PWRDET_2G
#define RF_ANA1_TEN_PWRDET_2G_POS                               (18U)
#define RF_ANA1_TEN_PWRDET_2G_LEN                               (1U)
#define RF_ANA1_TEN_PWRDET_2G_MSK                               (((1U<<RF_ANA1_TEN_PWRDET_2G_LEN)-1)<<RF_ANA1_TEN_PWRDET_2G_POS)
#define RF_ANA1_TEN_PWRDET_2G_UMSK                              (~(((1U<<RF_ANA1_TEN_PWRDET_2G_LEN)-1)<<RF_ANA1_TEN_PWRDET_2G_POS))
#define RF_ANA1_TEN_PWRDET_5G                                   RF_ANA1_TEN_PWRDET_5G
#define RF_ANA1_TEN_PWRDET_5G_POS                               (19U)
#define RF_ANA1_TEN_PWRDET_5G_LEN                               (1U)
#define RF_ANA1_TEN_PWRDET_5G_MSK                               (((1U<<RF_ANA1_TEN_PWRDET_5G_LEN)-1)<<RF_ANA1_TEN_PWRDET_5G_POS)
#define RF_ANA1_TEN_PWRDET_5G_UMSK                              (~(((1U<<RF_ANA1_TEN_PWRDET_5G_LEN)-1)<<RF_ANA1_TEN_PWRDET_5G_POS))
#define RF_ANA1_TEN_TRF_PTAT                                    RF_ANA1_TEN_TRF_PTAT
#define RF_ANA1_TEN_TRF_PTAT_POS                                (20U)
#define RF_ANA1_TEN_TRF_PTAT_LEN                                (1U)
#define RF_ANA1_TEN_TRF_PTAT_MSK                                (((1U<<RF_ANA1_TEN_TRF_PTAT_LEN)-1)<<RF_ANA1_TEN_TRF_PTAT_POS)
#define RF_ANA1_TEN_TRF_PTAT_UMSK                               (~(((1U<<RF_ANA1_TEN_TRF_PTAT_LEN)-1)<<RF_ANA1_TEN_TRF_PTAT_POS))
#define RF_ANA1_TEN_TBB                                         RF_ANA1_TEN_TBB
#define RF_ANA1_TEN_TBB_POS                                     (21U)
#define RF_ANA1_TEN_TBB_LEN                                     (1U)
#define RF_ANA1_TEN_TBB_MSK                                     (((1U<<RF_ANA1_TEN_TBB_LEN)-1)<<RF_ANA1_TEN_TBB_POS)
#define RF_ANA1_TEN_TBB_UMSK                                    (~(((1U<<RF_ANA1_TEN_TBB_LEN)-1)<<RF_ANA1_TEN_TBB_POS))
#define RF_ANA1_TEN_DAC_I                                       RF_ANA1_TEN_DAC_I
#define RF_ANA1_TEN_DAC_I_POS                                   (22U)
#define RF_ANA1_TEN_DAC_I_LEN                                   (1U)
#define RF_ANA1_TEN_DAC_I_MSK                                   (((1U<<RF_ANA1_TEN_DAC_I_LEN)-1)<<RF_ANA1_TEN_DAC_I_POS)
#define RF_ANA1_TEN_DAC_I_UMSK                                  (~(((1U<<RF_ANA1_TEN_DAC_I_LEN)-1)<<RF_ANA1_TEN_DAC_I_POS))
#define RF_ANA1_TEN_DAC_Q                                       RF_ANA1_TEN_DAC_Q
#define RF_ANA1_TEN_DAC_Q_POS                                   (23U)
#define RF_ANA1_TEN_DAC_Q_LEN                                   (1U)
#define RF_ANA1_TEN_DAC_Q_MSK                                   (((1U<<RF_ANA1_TEN_DAC_Q_LEN)-1)<<RF_ANA1_TEN_DAC_Q_POS)
#define RF_ANA1_TEN_DAC_Q_UMSK                                  (~(((1U<<RF_ANA1_TEN_DAC_Q_LEN)-1)<<RF_ANA1_TEN_DAC_Q_POS))
#define RF_ANA1_TEN_VCO                                         RF_ANA1_TEN_VCO
#define RF_ANA1_TEN_VCO_POS                                     (24U)
#define RF_ANA1_TEN_VCO_LEN                                     (1U)
#define RF_ANA1_TEN_VCO_MSK                                     (((1U<<RF_ANA1_TEN_VCO_LEN)-1)<<RF_ANA1_TEN_VCO_POS)
#define RF_ANA1_TEN_VCO_UMSK                                    (~(((1U<<RF_ANA1_TEN_VCO_LEN)-1)<<RF_ANA1_TEN_VCO_POS))
#define RF_ANA1_TEN_OSMX                                        RF_ANA1_TEN_OSMX
#define RF_ANA1_TEN_OSMX_POS                                    (25U)
#define RF_ANA1_TEN_OSMX_LEN                                    (1U)
#define RF_ANA1_TEN_OSMX_MSK                                    (((1U<<RF_ANA1_TEN_OSMX_LEN)-1)<<RF_ANA1_TEN_OSMX_POS)
#define RF_ANA1_TEN_OSMX_UMSK                                   (~(((1U<<RF_ANA1_TEN_OSMX_LEN)-1)<<RF_ANA1_TEN_OSMX_POS))
#define RF_ANA1_TEN_LOBUF                                       RF_ANA1_TEN_LOBUF
#define RF_ANA1_TEN_LOBUF_POS                                   (26U)
#define RF_ANA1_TEN_LOBUF_LEN                                   (1U)
#define RF_ANA1_TEN_LOBUF_MSK                                   (((1U<<RF_ANA1_TEN_LOBUF_LEN)-1)<<RF_ANA1_TEN_LOBUF_POS)
#define RF_ANA1_TEN_LOBUF_UMSK                                  (~(((1U<<RF_ANA1_TEN_LOBUF_LEN)-1)<<RF_ANA1_TEN_LOBUF_POS))
#define RF_ANA1_TEN_LODIST_RX                                   RF_ANA1_TEN_LODIST_RX
#define RF_ANA1_TEN_LODIST_RX_POS                               (27U)
#define RF_ANA1_TEN_LODIST_RX_LEN                               (1U)
#define RF_ANA1_TEN_LODIST_RX_MSK                               (((1U<<RF_ANA1_TEN_LODIST_RX_LEN)-1)<<RF_ANA1_TEN_LODIST_RX_POS)
#define RF_ANA1_TEN_LODIST_RX_UMSK                              (~(((1U<<RF_ANA1_TEN_LODIST_RX_LEN)-1)<<RF_ANA1_TEN_LODIST_RX_POS))
#define RF_ANA1_TEN_DTC                                         RF_ANA1_TEN_DTC
#define RF_ANA1_TEN_DTC_POS                                     (28U)
#define RF_ANA1_TEN_DTC_LEN                                     (1U)
#define RF_ANA1_TEN_DTC_MSK                                     (((1U<<RF_ANA1_TEN_DTC_LEN)-1)<<RF_ANA1_TEN_DTC_POS)
#define RF_ANA1_TEN_DTC_UMSK                                    (~(((1U<<RF_ANA1_TEN_DTC_LEN)-1)<<RF_ANA1_TEN_DTC_POS))
#define RF_ANA1_TEN_TDC                                         RF_ANA1_TEN_TDC
#define RF_ANA1_TEN_TDC_POS                                     (29U)
#define RF_ANA1_TEN_TDC_LEN                                     (1U)
#define RF_ANA1_TEN_TDC_MSK                                     (((1U<<RF_ANA1_TEN_TDC_LEN)-1)<<RF_ANA1_TEN_TDC_POS)
#define RF_ANA1_TEN_TDC_UMSK                                    (~(((1U<<RF_ANA1_TEN_TDC_LEN)-1)<<RF_ANA1_TEN_TDC_POS))

/* 0x3C : digital test register */
#define RF_ANA1_TEN_DIG_OFFSET                                  (0x3C)
#define RF_ANA1_DTEN_DTC_IN                                     RF_ANA1_DTEN_DTC_IN
#define RF_ANA1_DTEN_DTC_IN_POS                                 (6U)
#define RF_ANA1_DTEN_DTC_IN_LEN                                 (1U)
#define RF_ANA1_DTEN_DTC_IN_MSK                                 (((1U<<RF_ANA1_DTEN_DTC_IN_LEN)-1)<<RF_ANA1_DTEN_DTC_IN_POS)
#define RF_ANA1_DTEN_DTC_IN_UMSK                                (~(((1U<<RF_ANA1_DTEN_DTC_IN_LEN)-1)<<RF_ANA1_DTEN_DTC_IN_POS))
#define RF_ANA1_DTEN_DTC_OUT                                    RF_ANA1_DTEN_DTC_OUT
#define RF_ANA1_DTEN_DTC_OUT_POS                                (8U)
#define RF_ANA1_DTEN_DTC_OUT_LEN                                (1U)
#define RF_ANA1_DTEN_DTC_OUT_MSK                                (((1U<<RF_ANA1_DTEN_DTC_OUT_LEN)-1)<<RF_ANA1_DTEN_DTC_OUT_POS)
#define RF_ANA1_DTEN_DTC_OUT_UMSK                               (~(((1U<<RF_ANA1_DTEN_DTC_OUT_LEN)-1)<<RF_ANA1_DTEN_DTC_OUT_POS))
#define RF_ANA1_DTEN_FBDIV_DIGCLK                               RF_ANA1_DTEN_FBDIV_DIGCLK
#define RF_ANA1_DTEN_FBDIV_DIGCLK_POS                           (9U)
#define RF_ANA1_DTEN_FBDIV_DIGCLK_LEN                           (1U)
#define RF_ANA1_DTEN_FBDIV_DIGCLK_MSK                           (((1U<<RF_ANA1_DTEN_FBDIV_DIGCLK_LEN)-1)<<RF_ANA1_DTEN_FBDIV_DIGCLK_POS)
#define RF_ANA1_DTEN_FBDIV_DIGCLK_UMSK                          (~(((1U<<RF_ANA1_DTEN_FBDIV_DIGCLK_LEN)-1)<<RF_ANA1_DTEN_FBDIV_DIGCLK_POS))
#define RF_ANA1_DTEN_FBDIV_PCLK                                 RF_ANA1_DTEN_FBDIV_PCLK
#define RF_ANA1_DTEN_FBDIV_PCLK_POS                             (23U)
#define RF_ANA1_DTEN_FBDIV_PCLK_LEN                             (1U)
#define RF_ANA1_DTEN_FBDIV_PCLK_MSK                             (((1U<<RF_ANA1_DTEN_FBDIV_PCLK_LEN)-1)<<RF_ANA1_DTEN_FBDIV_PCLK_POS)
#define RF_ANA1_DTEN_FBDIV_PCLK_UMSK                            (~(((1U<<RF_ANA1_DTEN_FBDIV_PCLK_LEN)-1)<<RF_ANA1_DTEN_FBDIV_PCLK_POS))

/* 0x40 : ac test register */
#define RF_ANA1_TEN_AC_OFFSET                                   (0x40)
#define RF_ANA1_ATEST_AC_OUT_EN                                 RF_ANA1_ATEST_AC_OUT_EN
#define RF_ANA1_ATEST_AC_OUT_EN_POS                             (0U)
#define RF_ANA1_ATEST_AC_OUT_EN_LEN                             (1U)
#define RF_ANA1_ATEST_AC_OUT_EN_MSK                             (((1U<<RF_ANA1_ATEST_AC_OUT_EN_LEN)-1)<<RF_ANA1_ATEST_AC_OUT_EN_POS)
#define RF_ANA1_ATEST_AC_OUT_EN_UMSK                            (~(((1U<<RF_ANA1_ATEST_AC_OUT_EN_LEN)-1)<<RF_ANA1_ATEST_AC_OUT_EN_POS))
#define RF_ANA1_ATEST_AC_IN_EN_Q                                RF_ANA1_ATEST_AC_IN_EN_Q
#define RF_ANA1_ATEST_AC_IN_EN_Q_POS                            (1U)
#define RF_ANA1_ATEST_AC_IN_EN_Q_LEN                            (1U)
#define RF_ANA1_ATEST_AC_IN_EN_Q_MSK                            (((1U<<RF_ANA1_ATEST_AC_IN_EN_Q_LEN)-1)<<RF_ANA1_ATEST_AC_IN_EN_Q_POS)
#define RF_ANA1_ATEST_AC_IN_EN_Q_UMSK                           (~(((1U<<RF_ANA1_ATEST_AC_IN_EN_Q_LEN)-1)<<RF_ANA1_ATEST_AC_IN_EN_Q_POS))
#define RF_ANA1_ATEST_AC_IN_EN_I                                RF_ANA1_ATEST_AC_IN_EN_I
#define RF_ANA1_ATEST_AC_IN_EN_I_POS                            (2U)
#define RF_ANA1_ATEST_AC_IN_EN_I_LEN                            (1U)
#define RF_ANA1_ATEST_AC_IN_EN_I_MSK                            (((1U<<RF_ANA1_ATEST_AC_IN_EN_I_LEN)-1)<<RF_ANA1_ATEST_AC_IN_EN_I_POS)
#define RF_ANA1_ATEST_AC_IN_EN_I_UMSK                           (~(((1U<<RF_ANA1_ATEST_AC_IN_EN_I_LEN)-1)<<RF_ANA1_ATEST_AC_IN_EN_I_POS))
#define RF_ANA1_ATEST_DAC_AC_TEST_EN                            RF_ANA1_ATEST_DAC_AC_TEST_EN
#define RF_ANA1_ATEST_DAC_AC_TEST_EN_POS                        (3U)
#define RF_ANA1_ATEST_DAC_AC_TEST_EN_LEN                        (1U)
#define RF_ANA1_ATEST_DAC_AC_TEST_EN_MSK                        (((1U<<RF_ANA1_ATEST_DAC_AC_TEST_EN_LEN)-1)<<RF_ANA1_ATEST_DAC_AC_TEST_EN_POS)
#define RF_ANA1_ATEST_DAC_AC_TEST_EN_UMSK                       (~(((1U<<RF_ANA1_ATEST_DAC_AC_TEST_EN_LEN)-1)<<RF_ANA1_ATEST_DAC_AC_TEST_EN_POS))
#define RF_ANA1_ATEST_GAIN_R9                                   RF_ANA1_ATEST_GAIN_R9
#define RF_ANA1_ATEST_GAIN_R9_POS                               (4U)
#define RF_ANA1_ATEST_GAIN_R9_LEN                               (3U)
#define RF_ANA1_ATEST_GAIN_R9_MSK                               (((1U<<RF_ANA1_ATEST_GAIN_R9_LEN)-1)<<RF_ANA1_ATEST_GAIN_R9_POS)
#define RF_ANA1_ATEST_GAIN_R9_UMSK                              (~(((1U<<RF_ANA1_ATEST_GAIN_R9_LEN)-1)<<RF_ANA1_ATEST_GAIN_R9_POS))
#define RF_ANA1_ATEST_OP_CC                                     RF_ANA1_ATEST_OP_CC
#define RF_ANA1_ATEST_OP_CC_POS                                 (8U)
#define RF_ANA1_ATEST_OP_CC_LEN                                 (4U)
#define RF_ANA1_ATEST_OP_CC_MSK                                 (((1U<<RF_ANA1_ATEST_OP_CC_LEN)-1)<<RF_ANA1_ATEST_OP_CC_POS)
#define RF_ANA1_ATEST_OP_CC_UMSK                                (~(((1U<<RF_ANA1_ATEST_OP_CC_LEN)-1)<<RF_ANA1_ATEST_OP_CC_POS))
#define RF_ANA1_ATEST_BM_OP_IN                                  RF_ANA1_ATEST_BM_OP_IN
#define RF_ANA1_ATEST_BM_OP_IN_POS                              (12U)
#define RF_ANA1_ATEST_BM_OP_IN_LEN                              (3U)
#define RF_ANA1_ATEST_BM_OP_IN_MSK                              (((1U<<RF_ANA1_ATEST_BM_OP_IN_LEN)-1)<<RF_ANA1_ATEST_BM_OP_IN_POS)
#define RF_ANA1_ATEST_BM_OP_IN_UMSK                             (~(((1U<<RF_ANA1_ATEST_BM_OP_IN_LEN)-1)<<RF_ANA1_ATEST_BM_OP_IN_POS))
#define RF_ANA1_ATEST_BM_OP_OUT                                 RF_ANA1_ATEST_BM_OP_OUT
#define RF_ANA1_ATEST_BM_OP_OUT_POS                             (16U)
#define RF_ANA1_ATEST_BM_OP_OUT_LEN                             (2U)
#define RF_ANA1_ATEST_BM_OP_OUT_MSK                             (((1U<<RF_ANA1_ATEST_BM_OP_OUT_LEN)-1)<<RF_ANA1_ATEST_BM_OP_OUT_POS)
#define RF_ANA1_ATEST_BM_OP_OUT_UMSK                            (~(((1U<<RF_ANA1_ATEST_BM_OP_OUT_LEN)-1)<<RF_ANA1_ATEST_BM_OP_OUT_POS))
#define RF_ANA1_ATEST_VCM                                       RF_ANA1_ATEST_VCM
#define RF_ANA1_ATEST_VCM_POS                                   (18U)
#define RF_ANA1_ATEST_VCM_LEN                                   (2U)
#define RF_ANA1_ATEST_VCM_MSK                                   (((1U<<RF_ANA1_ATEST_VCM_LEN)-1)<<RF_ANA1_ATEST_VCM_POS)
#define RF_ANA1_ATEST_VCM_UMSK                                  (~(((1U<<RF_ANA1_ATEST_VCM_LEN)-1)<<RF_ANA1_ATEST_VCM_POS))
#define RF_ANA1_ATEST_VSTARTUP                                  RF_ANA1_ATEST_VSTARTUP
#define RF_ANA1_ATEST_VSTARTUP_POS                              (20U)
#define RF_ANA1_ATEST_VSTARTUP_LEN                              (2U)
#define RF_ANA1_ATEST_VSTARTUP_MSK                              (((1U<<RF_ANA1_ATEST_VSTARTUP_LEN)-1)<<RF_ANA1_ATEST_VSTARTUP_POS)
#define RF_ANA1_ATEST_VSTARTUP_UMSK                             (~(((1U<<RF_ANA1_ATEST_VSTARTUP_LEN)-1)<<RF_ANA1_ATEST_VSTARTUP_POS))
#define RF_ANA1_ATEST_STARTUP_SET                               RF_ANA1_ATEST_STARTUP_SET
#define RF_ANA1_ATEST_STARTUP_SET_POS                           (22U)
#define RF_ANA1_ATEST_STARTUP_SET_LEN                           (1U)
#define RF_ANA1_ATEST_STARTUP_SET_MSK                           (((1U<<RF_ANA1_ATEST_STARTUP_SET_LEN)-1)<<RF_ANA1_ATEST_STARTUP_SET_POS)
#define RF_ANA1_ATEST_STARTUP_SET_UMSK                          (~(((1U<<RF_ANA1_ATEST_STARTUP_SET_LEN)-1)<<RF_ANA1_ATEST_STARTUP_SET_POS))
#define RF_ANA1_ATEST_BIAS_SEL_CGM                              RF_ANA1_ATEST_BIAS_SEL_CGM
#define RF_ANA1_ATEST_BIAS_SEL_CGM_POS                          (23U)
#define RF_ANA1_ATEST_BIAS_SEL_CGM_LEN                          (1U)
#define RF_ANA1_ATEST_BIAS_SEL_CGM_MSK                          (((1U<<RF_ANA1_ATEST_BIAS_SEL_CGM_LEN)-1)<<RF_ANA1_ATEST_BIAS_SEL_CGM_POS)
#define RF_ANA1_ATEST_BIAS_SEL_CGM_UMSK                         (~(((1U<<RF_ANA1_ATEST_BIAS_SEL_CGM_LEN)-1)<<RF_ANA1_ATEST_BIAS_SEL_CGM_POS))
#define RF_ANA1_ATEST_DAC_VCM                                   RF_ANA1_ATEST_DAC_VCM
#define RF_ANA1_ATEST_DAC_VCM_POS                               (24U)
#define RF_ANA1_ATEST_DAC_VCM_LEN                               (2U)
#define RF_ANA1_ATEST_DAC_VCM_MSK                               (((1U<<RF_ANA1_ATEST_DAC_VCM_LEN)-1)<<RF_ANA1_ATEST_DAC_VCM_POS)
#define RF_ANA1_ATEST_DAC_VCM_UMSK                              (~(((1U<<RF_ANA1_ATEST_DAC_VCM_LEN)-1)<<RF_ANA1_ATEST_DAC_VCM_POS))
#define RF_ANA1_ATEST_CBANK_FC                                  RF_ANA1_ATEST_CBANK_FC
#define RF_ANA1_ATEST_CBANK_FC_POS                              (28U)
#define RF_ANA1_ATEST_CBANK_FC_LEN                              (4U)
#define RF_ANA1_ATEST_CBANK_FC_MSK                              (((1U<<RF_ANA1_ATEST_CBANK_FC_LEN)-1)<<RF_ANA1_ATEST_CBANK_FC_POS)
#define RF_ANA1_ATEST_CBANK_FC_UMSK                             (~(((1U<<RF_ANA1_ATEST_CBANK_FC_LEN)-1)<<RF_ANA1_ATEST_CBANK_FC_POS))

/* 0x44 : RX normal bias mode registers */
#define RF_ANA1_CIP_0_OFFSET                                    (0x44)
#define RF_ANA1_RCAL_ICX_CODE                                   RF_ANA1_RCAL_ICX_CODE
#define RF_ANA1_RCAL_ICX_CODE_POS                               (0U)
#define RF_ANA1_RCAL_ICX_CODE_LEN                               (6U)
#define RF_ANA1_RCAL_ICX_CODE_MSK                               (((1U<<RF_ANA1_RCAL_ICX_CODE_LEN)-1)<<RF_ANA1_RCAL_ICX_CODE_POS)
#define RF_ANA1_RCAL_ICX_CODE_UMSK                              (~(((1U<<RF_ANA1_RCAL_ICX_CODE_LEN)-1)<<RF_ANA1_RCAL_ICX_CODE_POS))
#define RF_ANA1_RCAL_ICX_OUT_EN                                 RF_ANA1_RCAL_ICX_OUT_EN
#define RF_ANA1_RCAL_ICX_OUT_EN_POS                             (6U)
#define RF_ANA1_RCAL_ICX_OUT_EN_LEN                             (1U)
#define RF_ANA1_RCAL_ICX_OUT_EN_MSK                             (((1U<<RF_ANA1_RCAL_ICX_OUT_EN_LEN)-1)<<RF_ANA1_RCAL_ICX_OUT_EN_POS)
#define RF_ANA1_RCAL_ICX_OUT_EN_UMSK                            (~(((1U<<RF_ANA1_RCAL_ICX_OUT_EN_LEN)-1)<<RF_ANA1_RCAL_ICX_OUT_EN_POS))
#define RF_ANA1_RCAL_IPTAT_CODE                                 RF_ANA1_RCAL_IPTAT_CODE
#define RF_ANA1_RCAL_IPTAT_CODE_POS                             (8U)
#define RF_ANA1_RCAL_IPTAT_CODE_LEN                             (5U)
#define RF_ANA1_RCAL_IPTAT_CODE_MSK                             (((1U<<RF_ANA1_RCAL_IPTAT_CODE_LEN)-1)<<RF_ANA1_RCAL_IPTAT_CODE_POS)
#define RF_ANA1_RCAL_IPTAT_CODE_UMSK                            (~(((1U<<RF_ANA1_RCAL_IPTAT_CODE_LEN)-1)<<RF_ANA1_RCAL_IPTAT_CODE_POS))
#define RF_ANA1_RCAL_IPTAT_OUT_EN                               RF_ANA1_RCAL_IPTAT_OUT_EN
#define RF_ANA1_RCAL_IPTAT_OUT_EN_POS                           (13U)
#define RF_ANA1_RCAL_IPTAT_OUT_EN_LEN                           (1U)
#define RF_ANA1_RCAL_IPTAT_OUT_EN_MSK                           (((1U<<RF_ANA1_RCAL_IPTAT_OUT_EN_LEN)-1)<<RF_ANA1_RCAL_IPTAT_OUT_EN_POS)
#define RF_ANA1_RCAL_IPTAT_OUT_EN_UMSK                          (~(((1U<<RF_ANA1_RCAL_IPTAT_OUT_EN_LEN)-1)<<RF_ANA1_RCAL_IPTAT_OUT_EN_POS))
#define RF_ANA1_RCAL_IMIX_OUT_EN                                RF_ANA1_RCAL_IMIX_OUT_EN
#define RF_ANA1_RCAL_IMIX_OUT_EN_POS                            (14U)
#define RF_ANA1_RCAL_IMIX_OUT_EN_LEN                            (1U)
#define RF_ANA1_RCAL_IMIX_OUT_EN_MSK                            (((1U<<RF_ANA1_RCAL_IMIX_OUT_EN_LEN)-1)<<RF_ANA1_RCAL_IMIX_OUT_EN_POS)
#define RF_ANA1_RCAL_IMIX_OUT_EN_UMSK                           (~(((1U<<RF_ANA1_RCAL_IMIX_OUT_EN_LEN)-1)<<RF_ANA1_RCAL_IMIX_OUT_EN_POS))
#define RF_ANA1_PA_ICX_SCALE                                    RF_ANA1_PA_ICX_SCALE
#define RF_ANA1_PA_ICX_SCALE_POS                                (16U)
#define RF_ANA1_PA_ICX_SCALE_LEN                                (4U)
#define RF_ANA1_PA_ICX_SCALE_MSK                                (((1U<<RF_ANA1_PA_ICX_SCALE_LEN)-1)<<RF_ANA1_PA_ICX_SCALE_POS)
#define RF_ANA1_PA_ICX_SCALE_UMSK                               (~(((1U<<RF_ANA1_PA_ICX_SCALE_LEN)-1)<<RF_ANA1_PA_ICX_SCALE_POS))
#define RF_ANA1_PA_IPTAT_SCALE                                  RF_ANA1_PA_IPTAT_SCALE
#define RF_ANA1_PA_IPTAT_SCALE_POS                              (20U)
#define RF_ANA1_PA_IPTAT_SCALE_LEN                              (4U)
#define RF_ANA1_PA_IPTAT_SCALE_MSK                              (((1U<<RF_ANA1_PA_IPTAT_SCALE_LEN)-1)<<RF_ANA1_PA_IPTAT_SCALE_POS)
#define RF_ANA1_PA_IPTAT_SCALE_UMSK                             (~(((1U<<RF_ANA1_PA_IPTAT_SCALE_LEN)-1)<<RF_ANA1_PA_IPTAT_SCALE_POS))
#define RF_ANA1_TXDAC_ICX_SCALE                                 RF_ANA1_TXDAC_ICX_SCALE
#define RF_ANA1_TXDAC_ICX_SCALE_POS                             (24U)
#define RF_ANA1_TXDAC_ICX_SCALE_LEN                             (4U)
#define RF_ANA1_TXDAC_ICX_SCALE_MSK                             (((1U<<RF_ANA1_TXDAC_ICX_SCALE_LEN)-1)<<RF_ANA1_TXDAC_ICX_SCALE_POS)
#define RF_ANA1_TXDAC_ICX_SCALE_UMSK                            (~(((1U<<RF_ANA1_TXDAC_ICX_SCALE_LEN)-1)<<RF_ANA1_TXDAC_ICX_SCALE_POS))
#define RF_ANA1_TXDAC_IPTAT_SCALE                               RF_ANA1_TXDAC_IPTAT_SCALE
#define RF_ANA1_TXDAC_IPTAT_SCALE_POS                           (28U)
#define RF_ANA1_TXDAC_IPTAT_SCALE_LEN                           (4U)
#define RF_ANA1_TXDAC_IPTAT_SCALE_MSK                           (((1U<<RF_ANA1_TXDAC_IPTAT_SCALE_LEN)-1)<<RF_ANA1_TXDAC_IPTAT_SCALE_POS)
#define RF_ANA1_TXDAC_IPTAT_SCALE_UMSK                          (~(((1U<<RF_ANA1_TXDAC_IPTAT_SCALE_LEN)-1)<<RF_ANA1_TXDAC_IPTAT_SCALE_POS))

/* 0x48 : pa_0 */
#define RF_ANA1_PA_0_OFFSET                                     (0x48)
#define RF_ANA1_PA_VBCAS1                                       RF_ANA1_PA_VBCAS1
#define RF_ANA1_PA_VBCAS1_POS                                   (0U)
#define RF_ANA1_PA_VBCAS1_LEN                                   (4U)
#define RF_ANA1_PA_VBCAS1_MSK                                   (((1U<<RF_ANA1_PA_VBCAS1_LEN)-1)<<RF_ANA1_PA_VBCAS1_POS)
#define RF_ANA1_PA_VBCAS1_UMSK                                  (~(((1U<<RF_ANA1_PA_VBCAS1_LEN)-1)<<RF_ANA1_PA_VBCAS1_POS))
#define RF_ANA1_PA_VBCORE                                       RF_ANA1_PA_VBCORE
#define RF_ANA1_PA_VBCORE_POS                                   (4U)
#define RF_ANA1_PA_VBCORE_LEN                                   (5U)
#define RF_ANA1_PA_VBCORE_MSK                                   (((1U<<RF_ANA1_PA_VBCORE_LEN)-1)<<RF_ANA1_PA_VBCORE_POS)
#define RF_ANA1_PA_VBCORE_UMSK                                  (~(((1U<<RF_ANA1_PA_VBCORE_LEN)-1)<<RF_ANA1_PA_VBCORE_POS))
#define RF_ANA1_PA_IET                                          RF_ANA1_PA_IET
#define RF_ANA1_PA_IET_POS                                      (12U)
#define RF_ANA1_PA_IET_LEN                                      (5U)
#define RF_ANA1_PA_IET_MSK                                      (((1U<<RF_ANA1_PA_IET_LEN)-1)<<RF_ANA1_PA_IET_POS)
#define RF_ANA1_PA_IET_UMSK                                     (~(((1U<<RF_ANA1_PA_IET_LEN)-1)<<RF_ANA1_PA_IET_POS))
#define RF_ANA1_PA_IAQ                                          RF_ANA1_PA_IAQ
#define RF_ANA1_PA_IAQ_POS                                      (20U)
#define RF_ANA1_PA_IAQ_LEN                                      (3U)
#define RF_ANA1_PA_IAQ_MSK                                      (((1U<<RF_ANA1_PA_IAQ_LEN)-1)<<RF_ANA1_PA_IAQ_POS)
#define RF_ANA1_PA_IAQ_UMSK                                     (~(((1U<<RF_ANA1_PA_IAQ_LEN)-1)<<RF_ANA1_PA_IAQ_POS))
#define RF_ANA1_PA_VBPMOS                                       RF_ANA1_PA_VBPMOS
#define RF_ANA1_PA_VBPMOS_POS                                   (24U)
#define RF_ANA1_PA_VBPMOS_LEN                                   (4U)
#define RF_ANA1_PA_VBPMOS_MSK                                   (((1U<<RF_ANA1_PA_VBPMOS_LEN)-1)<<RF_ANA1_PA_VBPMOS_POS)
#define RF_ANA1_PA_VBPMOS_UMSK                                  (~(((1U<<RF_ANA1_PA_VBPMOS_LEN)-1)<<RF_ANA1_PA_VBPMOS_POS))

/* 0x4C : pa_1 */
#define RF_ANA1_PA_1_OFFSET                                     (0x4C)
#define RF_ANA1_PA_VBCAS1_HW                                    RF_ANA1_PA_VBCAS1_HW
#define RF_ANA1_PA_VBCAS1_HW_POS                                (0U)
#define RF_ANA1_PA_VBCAS1_HW_LEN                                (4U)
#define RF_ANA1_PA_VBCAS1_HW_MSK                                (((1U<<RF_ANA1_PA_VBCAS1_HW_LEN)-1)<<RF_ANA1_PA_VBCAS1_HW_POS)
#define RF_ANA1_PA_VBCAS1_HW_UMSK                               (~(((1U<<RF_ANA1_PA_VBCAS1_HW_LEN)-1)<<RF_ANA1_PA_VBCAS1_HW_POS))
#define RF_ANA1_PA_VBCORE_HW                                    RF_ANA1_PA_VBCORE_HW
#define RF_ANA1_PA_VBCORE_HW_POS                                (4U)
#define RF_ANA1_PA_VBCORE_HW_LEN                                (5U)
#define RF_ANA1_PA_VBCORE_HW_MSK                                (((1U<<RF_ANA1_PA_VBCORE_HW_LEN)-1)<<RF_ANA1_PA_VBCORE_HW_POS)
#define RF_ANA1_PA_VBCORE_HW_UMSK                               (~(((1U<<RF_ANA1_PA_VBCORE_HW_LEN)-1)<<RF_ANA1_PA_VBCORE_HW_POS))
#define RF_ANA1_PA_IET_HW                                       RF_ANA1_PA_IET_HW
#define RF_ANA1_PA_IET_HW_POS                                   (12U)
#define RF_ANA1_PA_IET_HW_LEN                                   (5U)
#define RF_ANA1_PA_IET_HW_MSK                                   (((1U<<RF_ANA1_PA_IET_HW_LEN)-1)<<RF_ANA1_PA_IET_HW_POS)
#define RF_ANA1_PA_IET_HW_UMSK                                  (~(((1U<<RF_ANA1_PA_IET_HW_LEN)-1)<<RF_ANA1_PA_IET_HW_POS))
#define RF_ANA1_PA_IAQ_HW                                       RF_ANA1_PA_IAQ_HW
#define RF_ANA1_PA_IAQ_HW_POS                                   (20U)
#define RF_ANA1_PA_IAQ_HW_LEN                                   (3U)
#define RF_ANA1_PA_IAQ_HW_MSK                                   (((1U<<RF_ANA1_PA_IAQ_HW_LEN)-1)<<RF_ANA1_PA_IAQ_HW_POS)
#define RF_ANA1_PA_IAQ_HW_UMSK                                  (~(((1U<<RF_ANA1_PA_IAQ_HW_LEN)-1)<<RF_ANA1_PA_IAQ_HW_POS))
#define RF_ANA1_PA_VBPMOS_HW                                    RF_ANA1_PA_VBPMOS_HW
#define RF_ANA1_PA_VBPMOS_HW_POS                                (24U)
#define RF_ANA1_PA_VBPMOS_HW_LEN                                (4U)
#define RF_ANA1_PA_VBPMOS_HW_MSK                                (((1U<<RF_ANA1_PA_VBPMOS_HW_LEN)-1)<<RF_ANA1_PA_VBPMOS_HW_POS)
#define RF_ANA1_PA_VBPMOS_HW_UMSK                               (~(((1U<<RF_ANA1_PA_VBPMOS_HW_LEN)-1)<<RF_ANA1_PA_VBPMOS_HW_POS))

/* 0x50 : pa_2g_0 */
#define RF_ANA1_PA_2G_0_OFFSET                                  (0x50)
#define RF_ANA1_PA_2G_VBCAS2                                    RF_ANA1_PA_2G_VBCAS2
#define RF_ANA1_PA_2G_VBCAS2_POS                                (0U)
#define RF_ANA1_PA_2G_VBCAS2_LEN                                (3U)
#define RF_ANA1_PA_2G_VBCAS2_MSK                                (((1U<<RF_ANA1_PA_2G_VBCAS2_LEN)-1)<<RF_ANA1_PA_2G_VBCAS2_POS)
#define RF_ANA1_PA_2G_VBCAS2_UMSK                               (~(((1U<<RF_ANA1_PA_2G_VBCAS2_LEN)-1)<<RF_ANA1_PA_2G_VBCAS2_POS))
#define RF_ANA1_PA_2G_BM_SF                                     RF_ANA1_PA_2G_BM_SF
#define RF_ANA1_PA_2G_BM_SF_POS                                 (4U)
#define RF_ANA1_PA_2G_BM_SF_LEN                                 (3U)
#define RF_ANA1_PA_2G_BM_SF_MSK                                 (((1U<<RF_ANA1_PA_2G_BM_SF_LEN)-1)<<RF_ANA1_PA_2G_BM_SF_POS)
#define RF_ANA1_PA_2G_BM_SF_UMSK                                (~(((1U<<RF_ANA1_PA_2G_BM_SF_LEN)-1)<<RF_ANA1_PA_2G_BM_SF_POS))
#define RF_ANA1_PA_2G_LZ_BIAS_EN                                RF_ANA1_PA_2G_LZ_BIAS_EN
#define RF_ANA1_PA_2G_LZ_BIAS_EN_POS                            (8U)
#define RF_ANA1_PA_2G_LZ_BIAS_EN_LEN                            (1U)
#define RF_ANA1_PA_2G_LZ_BIAS_EN_MSK                            (((1U<<RF_ANA1_PA_2G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_PA_2G_LZ_BIAS_EN_POS)
#define RF_ANA1_PA_2G_LZ_BIAS_EN_UMSK                           (~(((1U<<RF_ANA1_PA_2G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_PA_2G_LZ_BIAS_EN_POS))
#define RF_ANA1_PA_2G_OP_CC                                     RF_ANA1_PA_2G_OP_CC
#define RF_ANA1_PA_2G_OP_CC_POS                                 (12U)
#define RF_ANA1_PA_2G_OP_CC_LEN                                 (3U)
#define RF_ANA1_PA_2G_OP_CC_MSK                                 (((1U<<RF_ANA1_PA_2G_OP_CC_LEN)-1)<<RF_ANA1_PA_2G_OP_CC_POS)
#define RF_ANA1_PA_2G_OP_CC_UMSK                                (~(((1U<<RF_ANA1_PA_2G_OP_CC_LEN)-1)<<RF_ANA1_PA_2G_OP_CC_POS))
#define RF_ANA1_PA_2G_CN_SEL                                    RF_ANA1_PA_2G_CN_SEL
#define RF_ANA1_PA_2G_CN_SEL_POS                                (16U)
#define RF_ANA1_PA_2G_CN_SEL_LEN                                (1U)
#define RF_ANA1_PA_2G_CN_SEL_MSK                                (((1U<<RF_ANA1_PA_2G_CN_SEL_LEN)-1)<<RF_ANA1_PA_2G_CN_SEL_POS)
#define RF_ANA1_PA_2G_CN_SEL_UMSK                               (~(((1U<<RF_ANA1_PA_2G_CN_SEL_LEN)-1)<<RF_ANA1_PA_2G_CN_SEL_POS))
#define RF_ANA1_PA_2G_IBC_OP                                    RF_ANA1_PA_2G_IBC_OP
#define RF_ANA1_PA_2G_IBC_OP_POS                                (20U)
#define RF_ANA1_PA_2G_IBC_OP_LEN                                (3U)
#define RF_ANA1_PA_2G_IBC_OP_MSK                                (((1U<<RF_ANA1_PA_2G_IBC_OP_LEN)-1)<<RF_ANA1_PA_2G_IBC_OP_POS)
#define RF_ANA1_PA_2G_IBC_OP_UMSK                               (~(((1U<<RF_ANA1_PA_2G_IBC_OP_LEN)-1)<<RF_ANA1_PA_2G_IBC_OP_POS))
#define RF_ANA1_PA_2G_GC_ATT                                    RF_ANA1_PA_2G_GC_ATT
#define RF_ANA1_PA_2G_GC_ATT_POS                                (24U)
#define RF_ANA1_PA_2G_GC_ATT_LEN                                (3U)
#define RF_ANA1_PA_2G_GC_ATT_MSK                                (((1U<<RF_ANA1_PA_2G_GC_ATT_LEN)-1)<<RF_ANA1_PA_2G_GC_ATT_POS)
#define RF_ANA1_PA_2G_GC_ATT_UMSK                               (~(((1U<<RF_ANA1_PA_2G_GC_ATT_LEN)-1)<<RF_ANA1_PA_2G_GC_ATT_POS))
#define RF_ANA1_PA_2G_COUT_SEL                                  RF_ANA1_PA_2G_COUT_SEL
#define RF_ANA1_PA_2G_COUT_SEL_POS                              (28U)
#define RF_ANA1_PA_2G_COUT_SEL_LEN                              (3U)
#define RF_ANA1_PA_2G_COUT_SEL_MSK                              (((1U<<RF_ANA1_PA_2G_COUT_SEL_LEN)-1)<<RF_ANA1_PA_2G_COUT_SEL_POS)
#define RF_ANA1_PA_2G_COUT_SEL_UMSK                             (~(((1U<<RF_ANA1_PA_2G_COUT_SEL_LEN)-1)<<RF_ANA1_PA_2G_COUT_SEL_POS))

/* 0x54 : pa_2g_1 */
#define RF_ANA1_PA_2G_1_OFFSET                                  (0x54)
#define RF_ANA1_PA_2G_VBPMOS_RANGE                              RF_ANA1_PA_2G_VBPMOS_RANGE
#define RF_ANA1_PA_2G_VBPMOS_RANGE_POS                          (0U)
#define RF_ANA1_PA_2G_VBPMOS_RANGE_LEN                          (1U)
#define RF_ANA1_PA_2G_VBPMOS_RANGE_MSK                          (((1U<<RF_ANA1_PA_2G_VBPMOS_RANGE_LEN)-1)<<RF_ANA1_PA_2G_VBPMOS_RANGE_POS)
#define RF_ANA1_PA_2G_VBPMOS_RANGE_UMSK                         (~(((1U<<RF_ANA1_PA_2G_VBPMOS_RANGE_LEN)-1)<<RF_ANA1_PA_2G_VBPMOS_RANGE_POS))
#define RF_ANA1_PA_2G_IPTAT_SEL                                 RF_ANA1_PA_2G_IPTAT_SEL
#define RF_ANA1_PA_2G_IPTAT_SEL_POS                             (4U)
#define RF_ANA1_PA_2G_IPTAT_SEL_LEN                             (1U)
#define RF_ANA1_PA_2G_IPTAT_SEL_MSK                             (((1U<<RF_ANA1_PA_2G_IPTAT_SEL_LEN)-1)<<RF_ANA1_PA_2G_IPTAT_SEL_POS)
#define RF_ANA1_PA_2G_IPTAT_SEL_UMSK                            (~(((1U<<RF_ANA1_PA_2G_IPTAT_SEL_LEN)-1)<<RF_ANA1_PA_2G_IPTAT_SEL_POS))

/* 0x58 : tmx_2g */
#define RF_ANA1_TMX_2G_OFFSET                                   (0x58)
#define RF_ANA1_TMX_2G_VBSW                                     RF_ANA1_TMX_2G_VBSW
#define RF_ANA1_TMX_2G_VBSW_POS                                 (0U)
#define RF_ANA1_TMX_2G_VBSW_LEN                                 (3U)
#define RF_ANA1_TMX_2G_VBSW_MSK                                 (((1U<<RF_ANA1_TMX_2G_VBSW_LEN)-1)<<RF_ANA1_TMX_2G_VBSW_POS)
#define RF_ANA1_TMX_2G_VBSW_UMSK                                (~(((1U<<RF_ANA1_TMX_2G_VBSW_LEN)-1)<<RF_ANA1_TMX_2G_VBSW_POS))
#define RF_ANA1_TMX_2G_VBCAS                                    RF_ANA1_TMX_2G_VBCAS
#define RF_ANA1_TMX_2G_VBCAS_POS                                (4U)
#define RF_ANA1_TMX_2G_VBCAS_LEN                                (3U)
#define RF_ANA1_TMX_2G_VBCAS_MSK                                (((1U<<RF_ANA1_TMX_2G_VBCAS_LEN)-1)<<RF_ANA1_TMX_2G_VBCAS_POS)
#define RF_ANA1_TMX_2G_VBCAS_UMSK                               (~(((1U<<RF_ANA1_TMX_2G_VBCAS_LEN)-1)<<RF_ANA1_TMX_2G_VBCAS_POS))
#define RF_ANA1_TMX_2G_VBCAS2                                   RF_ANA1_TMX_2G_VBCAS2
#define RF_ANA1_TMX_2G_VBCAS2_POS                               (8U)
#define RF_ANA1_TMX_2G_VBCAS2_LEN                               (3U)
#define RF_ANA1_TMX_2G_VBCAS2_MSK                               (((1U<<RF_ANA1_TMX_2G_VBCAS2_LEN)-1)<<RF_ANA1_TMX_2G_VBCAS2_POS)
#define RF_ANA1_TMX_2G_VBCAS2_UMSK                              (~(((1U<<RF_ANA1_TMX_2G_VBCAS2_LEN)-1)<<RF_ANA1_TMX_2G_VBCAS2_POS))
#define RF_ANA1_TMX_2G_BM_CAS_BULK                              RF_ANA1_TMX_2G_BM_CAS_BULK
#define RF_ANA1_TMX_2G_BM_CAS_BULK_POS                          (12U)
#define RF_ANA1_TMX_2G_BM_CAS_BULK_LEN                          (3U)
#define RF_ANA1_TMX_2G_BM_CAS_BULK_MSK                          (((1U<<RF_ANA1_TMX_2G_BM_CAS_BULK_LEN)-1)<<RF_ANA1_TMX_2G_BM_CAS_BULK_POS)
#define RF_ANA1_TMX_2G_BM_CAS_BULK_UMSK                         (~(((1U<<RF_ANA1_TMX_2G_BM_CAS_BULK_LEN)-1)<<RF_ANA1_TMX_2G_BM_CAS_BULK_POS))
#define RF_ANA1_TMX_2G_DEQ                                      RF_ANA1_TMX_2G_DEQ
#define RF_ANA1_TMX_2G_DEQ_POS                                  (16U)
#define RF_ANA1_TMX_2G_DEQ_LEN                                  (3U)
#define RF_ANA1_TMX_2G_DEQ_MSK                                  (((1U<<RF_ANA1_TMX_2G_DEQ_LEN)-1)<<RF_ANA1_TMX_2G_DEQ_POS)
#define RF_ANA1_TMX_2G_DEQ_UMSK                                 (~(((1U<<RF_ANA1_TMX_2G_DEQ_LEN)-1)<<RF_ANA1_TMX_2G_DEQ_POS))

/* 0x5C : pwrdet_2g */
#define RF_ANA1_PWRDET_2G_OFFSET                                (0x5C)
#define RF_ANA1_PWRDET_2G_MUL_SHORT_EN                          RF_ANA1_PWRDET_2G_MUL_SHORT_EN
#define RF_ANA1_PWRDET_2G_MUL_SHORT_EN_POS                      (1U)
#define RF_ANA1_PWRDET_2G_MUL_SHORT_EN_LEN                      (1U)
#define RF_ANA1_PWRDET_2G_MUL_SHORT_EN_MSK                      (((1U<<RF_ANA1_PWRDET_2G_MUL_SHORT_EN_LEN)-1)<<RF_ANA1_PWRDET_2G_MUL_SHORT_EN_POS)
#define RF_ANA1_PWRDET_2G_MUL_SHORT_EN_UMSK                     (~(((1U<<RF_ANA1_PWRDET_2G_MUL_SHORT_EN_LEN)-1)<<RF_ANA1_PWRDET_2G_MUL_SHORT_EN_POS))
#define RF_ANA1_PWRDET_2G_MUL_GC_ATT                            RF_ANA1_PWRDET_2G_MUL_GC_ATT
#define RF_ANA1_PWRDET_2G_MUL_GC_ATT_POS                        (4U)
#define RF_ANA1_PWRDET_2G_MUL_GC_ATT_LEN                        (2U)
#define RF_ANA1_PWRDET_2G_MUL_GC_ATT_MSK                        (((1U<<RF_ANA1_PWRDET_2G_MUL_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_2G_MUL_GC_ATT_POS)
#define RF_ANA1_PWRDET_2G_MUL_GC_ATT_UMSK                       (~(((1U<<RF_ANA1_PWRDET_2G_MUL_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_2G_MUL_GC_ATT_POS))
#define RF_ANA1_PWRDET_2G_GC_ATT                                RF_ANA1_PWRDET_2G_GC_ATT
#define RF_ANA1_PWRDET_2G_GC_ATT_POS                            (8U)
#define RF_ANA1_PWRDET_2G_GC_ATT_LEN                            (3U)
#define RF_ANA1_PWRDET_2G_GC_ATT_MSK                            (((1U<<RF_ANA1_PWRDET_2G_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_2G_GC_ATT_POS)
#define RF_ANA1_PWRDET_2G_GC_ATT_UMSK                           (~(((1U<<RF_ANA1_PWRDET_2G_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_2G_GC_ATT_POS))
#define RF_ANA1_PWRDET_2G_OSDAC_CODE                            RF_ANA1_PWRDET_2G_OSDAC_CODE
#define RF_ANA1_PWRDET_2G_OSDAC_CODE_POS                        (12U)
#define RF_ANA1_PWRDET_2G_OSDAC_CODE_LEN                        (5U)
#define RF_ANA1_PWRDET_2G_OSDAC_CODE_MSK                        (((1U<<RF_ANA1_PWRDET_2G_OSDAC_CODE_LEN)-1)<<RF_ANA1_PWRDET_2G_OSDAC_CODE_POS)
#define RF_ANA1_PWRDET_2G_OSDAC_CODE_UMSK                       (~(((1U<<RF_ANA1_PWRDET_2G_OSDAC_CODE_LEN)-1)<<RF_ANA1_PWRDET_2G_OSDAC_CODE_POS))
#define RF_ANA1_PWRDET_2G_OSDAC_BM                              RF_ANA1_PWRDET_2G_OSDAC_BM
#define RF_ANA1_PWRDET_2G_OSDAC_BM_POS                          (20U)
#define RF_ANA1_PWRDET_2G_OSDAC_BM_LEN                          (3U)
#define RF_ANA1_PWRDET_2G_OSDAC_BM_MSK                          (((1U<<RF_ANA1_PWRDET_2G_OSDAC_BM_LEN)-1)<<RF_ANA1_PWRDET_2G_OSDAC_BM_POS)
#define RF_ANA1_PWRDET_2G_OSDAC_BM_UMSK                         (~(((1U<<RF_ANA1_PWRDET_2G_OSDAC_BM_LEN)-1)<<RF_ANA1_PWRDET_2G_OSDAC_BM_POS))
#define RF_ANA1_PWRDET_2G_MUL_BM                                RF_ANA1_PWRDET_2G_MUL_BM
#define RF_ANA1_PWRDET_2G_MUL_BM_POS                            (24U)
#define RF_ANA1_PWRDET_2G_MUL_BM_LEN                            (3U)
#define RF_ANA1_PWRDET_2G_MUL_BM_MSK                            (((1U<<RF_ANA1_PWRDET_2G_MUL_BM_LEN)-1)<<RF_ANA1_PWRDET_2G_MUL_BM_POS)
#define RF_ANA1_PWRDET_2G_MUL_BM_UMSK                           (~(((1U<<RF_ANA1_PWRDET_2G_MUL_BM_LEN)-1)<<RF_ANA1_PWRDET_2G_MUL_BM_POS))

/* 0x60 : pa_5g_0 */
#define RF_ANA1_PA_5G_0_OFFSET                                  (0x60)
#define RF_ANA1_PA_5G_VBCAS2                                    RF_ANA1_PA_5G_VBCAS2
#define RF_ANA1_PA_5G_VBCAS2_POS                                (0U)
#define RF_ANA1_PA_5G_VBCAS2_LEN                                (3U)
#define RF_ANA1_PA_5G_VBCAS2_MSK                                (((1U<<RF_ANA1_PA_5G_VBCAS2_LEN)-1)<<RF_ANA1_PA_5G_VBCAS2_POS)
#define RF_ANA1_PA_5G_VBCAS2_UMSK                               (~(((1U<<RF_ANA1_PA_5G_VBCAS2_LEN)-1)<<RF_ANA1_PA_5G_VBCAS2_POS))
#define RF_ANA1_PA_5G_BM_SF                                     RF_ANA1_PA_5G_BM_SF
#define RF_ANA1_PA_5G_BM_SF_POS                                 (4U)
#define RF_ANA1_PA_5G_BM_SF_LEN                                 (3U)
#define RF_ANA1_PA_5G_BM_SF_MSK                                 (((1U<<RF_ANA1_PA_5G_BM_SF_LEN)-1)<<RF_ANA1_PA_5G_BM_SF_POS)
#define RF_ANA1_PA_5G_BM_SF_UMSK                                (~(((1U<<RF_ANA1_PA_5G_BM_SF_LEN)-1)<<RF_ANA1_PA_5G_BM_SF_POS))
#define RF_ANA1_PA_5G_LZ_BIAS_EN                                RF_ANA1_PA_5G_LZ_BIAS_EN
#define RF_ANA1_PA_5G_LZ_BIAS_EN_POS                            (8U)
#define RF_ANA1_PA_5G_LZ_BIAS_EN_LEN                            (1U)
#define RF_ANA1_PA_5G_LZ_BIAS_EN_MSK                            (((1U<<RF_ANA1_PA_5G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_PA_5G_LZ_BIAS_EN_POS)
#define RF_ANA1_PA_5G_LZ_BIAS_EN_UMSK                           (~(((1U<<RF_ANA1_PA_5G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_PA_5G_LZ_BIAS_EN_POS))
#define RF_ANA1_PA_5G_OP_CC                                     RF_ANA1_PA_5G_OP_CC
#define RF_ANA1_PA_5G_OP_CC_POS                                 (12U)
#define RF_ANA1_PA_5G_OP_CC_LEN                                 (3U)
#define RF_ANA1_PA_5G_OP_CC_MSK                                 (((1U<<RF_ANA1_PA_5G_OP_CC_LEN)-1)<<RF_ANA1_PA_5G_OP_CC_POS)
#define RF_ANA1_PA_5G_OP_CC_UMSK                                (~(((1U<<RF_ANA1_PA_5G_OP_CC_LEN)-1)<<RF_ANA1_PA_5G_OP_CC_POS))
#define RF_ANA1_PA_5G_CN_SEL                                    RF_ANA1_PA_5G_CN_SEL
#define RF_ANA1_PA_5G_CN_SEL_POS                                (16U)
#define RF_ANA1_PA_5G_CN_SEL_LEN                                (1U)
#define RF_ANA1_PA_5G_CN_SEL_MSK                                (((1U<<RF_ANA1_PA_5G_CN_SEL_LEN)-1)<<RF_ANA1_PA_5G_CN_SEL_POS)
#define RF_ANA1_PA_5G_CN_SEL_UMSK                               (~(((1U<<RF_ANA1_PA_5G_CN_SEL_LEN)-1)<<RF_ANA1_PA_5G_CN_SEL_POS))
#define RF_ANA1_PA_5G_IBC_OP                                    RF_ANA1_PA_5G_IBC_OP
#define RF_ANA1_PA_5G_IBC_OP_POS                                (20U)
#define RF_ANA1_PA_5G_IBC_OP_LEN                                (3U)
#define RF_ANA1_PA_5G_IBC_OP_MSK                                (((1U<<RF_ANA1_PA_5G_IBC_OP_LEN)-1)<<RF_ANA1_PA_5G_IBC_OP_POS)
#define RF_ANA1_PA_5G_IBC_OP_UMSK                               (~(((1U<<RF_ANA1_PA_5G_IBC_OP_LEN)-1)<<RF_ANA1_PA_5G_IBC_OP_POS))
#define RF_ANA1_PA_5G_GC_ATT                                    RF_ANA1_PA_5G_GC_ATT
#define RF_ANA1_PA_5G_GC_ATT_POS                                (24U)
#define RF_ANA1_PA_5G_GC_ATT_LEN                                (3U)
#define RF_ANA1_PA_5G_GC_ATT_MSK                                (((1U<<RF_ANA1_PA_5G_GC_ATT_LEN)-1)<<RF_ANA1_PA_5G_GC_ATT_POS)
#define RF_ANA1_PA_5G_GC_ATT_UMSK                               (~(((1U<<RF_ANA1_PA_5G_GC_ATT_LEN)-1)<<RF_ANA1_PA_5G_GC_ATT_POS))
#define RF_ANA1_PA_5G_COUT_SEL                                  RF_ANA1_PA_5G_COUT_SEL
#define RF_ANA1_PA_5G_COUT_SEL_POS                              (28U)
#define RF_ANA1_PA_5G_COUT_SEL_LEN                              (3U)
#define RF_ANA1_PA_5G_COUT_SEL_MSK                              (((1U<<RF_ANA1_PA_5G_COUT_SEL_LEN)-1)<<RF_ANA1_PA_5G_COUT_SEL_POS)
#define RF_ANA1_PA_5G_COUT_SEL_UMSK                             (~(((1U<<RF_ANA1_PA_5G_COUT_SEL_LEN)-1)<<RF_ANA1_PA_5G_COUT_SEL_POS))

/* 0x64 : pa_5g_1 */
#define RF_ANA1_PA_5G_1_OFFSET                                  (0x64)
#define RF_ANA1_PA_5G_VBPMOS_RANGE                              RF_ANA1_PA_5G_VBPMOS_RANGE
#define RF_ANA1_PA_5G_VBPMOS_RANGE_POS                          (0U)
#define RF_ANA1_PA_5G_VBPMOS_RANGE_LEN                          (1U)
#define RF_ANA1_PA_5G_VBPMOS_RANGE_MSK                          (((1U<<RF_ANA1_PA_5G_VBPMOS_RANGE_LEN)-1)<<RF_ANA1_PA_5G_VBPMOS_RANGE_POS)
#define RF_ANA1_PA_5G_VBPMOS_RANGE_UMSK                         (~(((1U<<RF_ANA1_PA_5G_VBPMOS_RANGE_LEN)-1)<<RF_ANA1_PA_5G_VBPMOS_RANGE_POS))
#define RF_ANA1_PA_5G_IPTAT_SEL                                 RF_ANA1_PA_5G_IPTAT_SEL
#define RF_ANA1_PA_5G_IPTAT_SEL_POS                             (4U)
#define RF_ANA1_PA_5G_IPTAT_SEL_LEN                             (1U)
#define RF_ANA1_PA_5G_IPTAT_SEL_MSK                             (((1U<<RF_ANA1_PA_5G_IPTAT_SEL_LEN)-1)<<RF_ANA1_PA_5G_IPTAT_SEL_POS)
#define RF_ANA1_PA_5G_IPTAT_SEL_UMSK                            (~(((1U<<RF_ANA1_PA_5G_IPTAT_SEL_LEN)-1)<<RF_ANA1_PA_5G_IPTAT_SEL_POS))
#define RF_ANA1_PA_5G_CD_SEL                                    RF_ANA1_PA_5G_CD_SEL
#define RF_ANA1_PA_5G_CD_SEL_POS                                (8U)
#define RF_ANA1_PA_5G_CD_SEL_LEN                                (3U)
#define RF_ANA1_PA_5G_CD_SEL_MSK                                (((1U<<RF_ANA1_PA_5G_CD_SEL_LEN)-1)<<RF_ANA1_PA_5G_CD_SEL_POS)
#define RF_ANA1_PA_5G_CD_SEL_UMSK                               (~(((1U<<RF_ANA1_PA_5G_CD_SEL_LEN)-1)<<RF_ANA1_PA_5G_CD_SEL_POS))

/* 0x68 : tmx_5g */
#define RF_ANA1_TMX_5G_OFFSET                                   (0x68)
#define RF_ANA1_TMX_5G_VBSW                                     RF_ANA1_TMX_5G_VBSW
#define RF_ANA1_TMX_5G_VBSW_POS                                 (0U)
#define RF_ANA1_TMX_5G_VBSW_LEN                                 (3U)
#define RF_ANA1_TMX_5G_VBSW_MSK                                 (((1U<<RF_ANA1_TMX_5G_VBSW_LEN)-1)<<RF_ANA1_TMX_5G_VBSW_POS)
#define RF_ANA1_TMX_5G_VBSW_UMSK                                (~(((1U<<RF_ANA1_TMX_5G_VBSW_LEN)-1)<<RF_ANA1_TMX_5G_VBSW_POS))
#define RF_ANA1_TMX_5G_VBCAS                                    RF_ANA1_TMX_5G_VBCAS
#define RF_ANA1_TMX_5G_VBCAS_POS                                (4U)
#define RF_ANA1_TMX_5G_VBCAS_LEN                                (3U)
#define RF_ANA1_TMX_5G_VBCAS_MSK                                (((1U<<RF_ANA1_TMX_5G_VBCAS_LEN)-1)<<RF_ANA1_TMX_5G_VBCAS_POS)
#define RF_ANA1_TMX_5G_VBCAS_UMSK                               (~(((1U<<RF_ANA1_TMX_5G_VBCAS_LEN)-1)<<RF_ANA1_TMX_5G_VBCAS_POS))
#define RF_ANA1_TMX_5G_VBCAS2                                   RF_ANA1_TMX_5G_VBCAS2
#define RF_ANA1_TMX_5G_VBCAS2_POS                               (8U)
#define RF_ANA1_TMX_5G_VBCAS2_LEN                               (3U)
#define RF_ANA1_TMX_5G_VBCAS2_MSK                               (((1U<<RF_ANA1_TMX_5G_VBCAS2_LEN)-1)<<RF_ANA1_TMX_5G_VBCAS2_POS)
#define RF_ANA1_TMX_5G_VBCAS2_UMSK                              (~(((1U<<RF_ANA1_TMX_5G_VBCAS2_LEN)-1)<<RF_ANA1_TMX_5G_VBCAS2_POS))
#define RF_ANA1_TMX_5G_BM_CAS_BULK                              RF_ANA1_TMX_5G_BM_CAS_BULK
#define RF_ANA1_TMX_5G_BM_CAS_BULK_POS                          (12U)
#define RF_ANA1_TMX_5G_BM_CAS_BULK_LEN                          (3U)
#define RF_ANA1_TMX_5G_BM_CAS_BULK_MSK                          (((1U<<RF_ANA1_TMX_5G_BM_CAS_BULK_LEN)-1)<<RF_ANA1_TMX_5G_BM_CAS_BULK_POS)
#define RF_ANA1_TMX_5G_BM_CAS_BULK_UMSK                         (~(((1U<<RF_ANA1_TMX_5G_BM_CAS_BULK_LEN)-1)<<RF_ANA1_TMX_5G_BM_CAS_BULK_POS))
#define RF_ANA1_TMX_5G_DEQ                                      RF_ANA1_TMX_5G_DEQ
#define RF_ANA1_TMX_5G_DEQ_POS                                  (16U)
#define RF_ANA1_TMX_5G_DEQ_LEN                                  (3U)
#define RF_ANA1_TMX_5G_DEQ_MSK                                  (((1U<<RF_ANA1_TMX_5G_DEQ_LEN)-1)<<RF_ANA1_TMX_5G_DEQ_POS)
#define RF_ANA1_TMX_5G_DEQ_UMSK                                 (~(((1U<<RF_ANA1_TMX_5G_DEQ_LEN)-1)<<RF_ANA1_TMX_5G_DEQ_POS))
#define RF_ANA1_TMX_CS                                          RF_ANA1_TMX_CS
#define RF_ANA1_TMX_CS_POS                                      (20U)
#define RF_ANA1_TMX_CS_LEN                                      (5U)
#define RF_ANA1_TMX_CS_MSK                                      (((1U<<RF_ANA1_TMX_CS_LEN)-1)<<RF_ANA1_TMX_CS_POS)
#define RF_ANA1_TMX_CS_UMSK                                     (~(((1U<<RF_ANA1_TMX_CS_LEN)-1)<<RF_ANA1_TMX_CS_POS))
#define RF_ANA1_TMX_CS_HW                                       RF_ANA1_TMX_CS_HW
#define RF_ANA1_TMX_CS_HW_POS                                   (25U)
#define RF_ANA1_TMX_CS_HW_LEN                                   (5U)
#define RF_ANA1_TMX_CS_HW_MSK                                   (((1U<<RF_ANA1_TMX_CS_HW_LEN)-1)<<RF_ANA1_TMX_CS_HW_POS)
#define RF_ANA1_TMX_CS_HW_UMSK                                  (~(((1U<<RF_ANA1_TMX_CS_HW_LEN)-1)<<RF_ANA1_TMX_CS_HW_POS))

/* 0x6C : pwrdet_5g_0 */
#define RF_ANA1_PWRDET_5G_0_OFFSET                              (0x6C)
#define RF_ANA1_PWRDET_5G_PR_EN                                 RF_ANA1_PWRDET_5G_PR_EN
#define RF_ANA1_PWRDET_5G_PR_EN_POS                             (1U)
#define RF_ANA1_PWRDET_5G_PR_EN_LEN                             (1U)
#define RF_ANA1_PWRDET_5G_PR_EN_MSK                             (((1U<<RF_ANA1_PWRDET_5G_PR_EN_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_EN_POS)
#define RF_ANA1_PWRDET_5G_PR_EN_UMSK                            (~(((1U<<RF_ANA1_PWRDET_5G_PR_EN_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_EN_POS))
#define RF_ANA1_PWRDET_5G_PR_GC                                 RF_ANA1_PWRDET_5G_PR_GC
#define RF_ANA1_PWRDET_5G_PR_GC_POS                             (3U)
#define RF_ANA1_PWRDET_5G_PR_GC_LEN                             (1U)
#define RF_ANA1_PWRDET_5G_PR_GC_MSK                             (((1U<<RF_ANA1_PWRDET_5G_PR_GC_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_GC_POS)
#define RF_ANA1_PWRDET_5G_PR_GC_UMSK                            (~(((1U<<RF_ANA1_PWRDET_5G_PR_GC_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_GC_POS))
#define RF_ANA1_PWRDET_5G_MUL_GC_ATT                            RF_ANA1_PWRDET_5G_MUL_GC_ATT
#define RF_ANA1_PWRDET_5G_MUL_GC_ATT_POS                        (4U)
#define RF_ANA1_PWRDET_5G_MUL_GC_ATT_LEN                        (2U)
#define RF_ANA1_PWRDET_5G_MUL_GC_ATT_MSK                        (((1U<<RF_ANA1_PWRDET_5G_MUL_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_5G_MUL_GC_ATT_POS)
#define RF_ANA1_PWRDET_5G_MUL_GC_ATT_UMSK                       (~(((1U<<RF_ANA1_PWRDET_5G_MUL_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_5G_MUL_GC_ATT_POS))
#define RF_ANA1_PWRDET_5G_GC_ATT                                RF_ANA1_PWRDET_5G_GC_ATT
#define RF_ANA1_PWRDET_5G_GC_ATT_POS                            (8U)
#define RF_ANA1_PWRDET_5G_GC_ATT_LEN                            (3U)
#define RF_ANA1_PWRDET_5G_GC_ATT_MSK                            (((1U<<RF_ANA1_PWRDET_5G_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_5G_GC_ATT_POS)
#define RF_ANA1_PWRDET_5G_GC_ATT_UMSK                           (~(((1U<<RF_ANA1_PWRDET_5G_GC_ATT_LEN)-1)<<RF_ANA1_PWRDET_5G_GC_ATT_POS))
#define RF_ANA1_PWRDET_5G_OSDAC_CODE                            RF_ANA1_PWRDET_5G_OSDAC_CODE
#define RF_ANA1_PWRDET_5G_OSDAC_CODE_POS                        (12U)
#define RF_ANA1_PWRDET_5G_OSDAC_CODE_LEN                        (5U)
#define RF_ANA1_PWRDET_5G_OSDAC_CODE_MSK                        (((1U<<RF_ANA1_PWRDET_5G_OSDAC_CODE_LEN)-1)<<RF_ANA1_PWRDET_5G_OSDAC_CODE_POS)
#define RF_ANA1_PWRDET_5G_OSDAC_CODE_UMSK                       (~(((1U<<RF_ANA1_PWRDET_5G_OSDAC_CODE_LEN)-1)<<RF_ANA1_PWRDET_5G_OSDAC_CODE_POS))
#define RF_ANA1_PWRDET_5G_OSDAC_BM                              RF_ANA1_PWRDET_5G_OSDAC_BM
#define RF_ANA1_PWRDET_5G_OSDAC_BM_POS                          (20U)
#define RF_ANA1_PWRDET_5G_OSDAC_BM_LEN                          (3U)
#define RF_ANA1_PWRDET_5G_OSDAC_BM_MSK                          (((1U<<RF_ANA1_PWRDET_5G_OSDAC_BM_LEN)-1)<<RF_ANA1_PWRDET_5G_OSDAC_BM_POS)
#define RF_ANA1_PWRDET_5G_OSDAC_BM_UMSK                         (~(((1U<<RF_ANA1_PWRDET_5G_OSDAC_BM_LEN)-1)<<RF_ANA1_PWRDET_5G_OSDAC_BM_POS))
#define RF_ANA1_PWRDET_5G_MUL_BM                                RF_ANA1_PWRDET_5G_MUL_BM
#define RF_ANA1_PWRDET_5G_MUL_BM_POS                            (24U)
#define RF_ANA1_PWRDET_5G_MUL_BM_LEN                            (3U)
#define RF_ANA1_PWRDET_5G_MUL_BM_MSK                            (((1U<<RF_ANA1_PWRDET_5G_MUL_BM_LEN)-1)<<RF_ANA1_PWRDET_5G_MUL_BM_POS)
#define RF_ANA1_PWRDET_5G_MUL_BM_UMSK                           (~(((1U<<RF_ANA1_PWRDET_5G_MUL_BM_LEN)-1)<<RF_ANA1_PWRDET_5G_MUL_BM_POS))

/* 0x70 : pwrdet_5g_1 */
#define RF_ANA1_PWRDET_5G_1_OFFSET                              (0x70)
#define RF_ANA1_PWRDET_5G_PR_SWQ                                RF_ANA1_PWRDET_5G_PR_SWQ
#define RF_ANA1_PWRDET_5G_PR_SWQ_POS                            (12U)
#define RF_ANA1_PWRDET_5G_PR_SWQ_LEN                            (2U)
#define RF_ANA1_PWRDET_5G_PR_SWQ_MSK                            (((1U<<RF_ANA1_PWRDET_5G_PR_SWQ_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_SWQ_POS)
#define RF_ANA1_PWRDET_5G_PR_SWQ_UMSK                           (~(((1U<<RF_ANA1_PWRDET_5G_PR_SWQ_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_SWQ_POS))
#define RF_ANA1_PWRDET_5G_PR_SWI                                RF_ANA1_PWRDET_5G_PR_SWI
#define RF_ANA1_PWRDET_5G_PR_SWI_POS                            (16U)
#define RF_ANA1_PWRDET_5G_PR_SWI_LEN                            (2U)
#define RF_ANA1_PWRDET_5G_PR_SWI_MSK                            (((1U<<RF_ANA1_PWRDET_5G_PR_SWI_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_SWI_POS)
#define RF_ANA1_PWRDET_5G_PR_SWI_UMSK                           (~(((1U<<RF_ANA1_PWRDET_5G_PR_SWI_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_SWI_POS))
#define RF_ANA1_PWRDET_5G_PR_BM                                 RF_ANA1_PWRDET_5G_PR_BM
#define RF_ANA1_PWRDET_5G_PR_BM_POS                             (20U)
#define RF_ANA1_PWRDET_5G_PR_BM_LEN                             (2U)
#define RF_ANA1_PWRDET_5G_PR_BM_MSK                             (((1U<<RF_ANA1_PWRDET_5G_PR_BM_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_BM_POS)
#define RF_ANA1_PWRDET_5G_PR_BM_UMSK                            (~(((1U<<RF_ANA1_PWRDET_5G_PR_BM_LEN)-1)<<RF_ANA1_PWRDET_5G_PR_BM_POS))

/* 0x74 : trf_iptat_gen */
#define RF_ANA1_TRF_IPTAT_GEN_OFFSET                            (0x74)
#define RF_ANA1_TRF_PTAT_DA_SCALE                               RF_ANA1_TRF_PTAT_DA_SCALE
#define RF_ANA1_TRF_PTAT_DA_SCALE_POS                           (0U)
#define RF_ANA1_TRF_PTAT_DA_SCALE_LEN                           (4U)
#define RF_ANA1_TRF_PTAT_DA_SCALE_MSK                           (((1U<<RF_ANA1_TRF_PTAT_DA_SCALE_LEN)-1)<<RF_ANA1_TRF_PTAT_DA_SCALE_POS)
#define RF_ANA1_TRF_PTAT_DA_SCALE_UMSK                          (~(((1U<<RF_ANA1_TRF_PTAT_DA_SCALE_LEN)-1)<<RF_ANA1_TRF_PTAT_DA_SCALE_POS))
#define RF_ANA1_TRF_PTAT_PA_SCALE                               RF_ANA1_TRF_PTAT_PA_SCALE
#define RF_ANA1_TRF_PTAT_PA_SCALE_POS                           (4U)
#define RF_ANA1_TRF_PTAT_PA_SCALE_LEN                           (4U)
#define RF_ANA1_TRF_PTAT_PA_SCALE_MSK                           (((1U<<RF_ANA1_TRF_PTAT_PA_SCALE_LEN)-1)<<RF_ANA1_TRF_PTAT_PA_SCALE_POS)
#define RF_ANA1_TRF_PTAT_PA_SCALE_UMSK                          (~(((1U<<RF_ANA1_TRF_PTAT_PA_SCALE_LEN)-1)<<RF_ANA1_TRF_PTAT_PA_SCALE_POS))
#define RF_ANA1_TRF_PTAT_RCAL_CODE                              RF_ANA1_TRF_PTAT_RCAL_CODE
#define RF_ANA1_TRF_PTAT_RCAL_CODE_POS                          (8U)
#define RF_ANA1_TRF_PTAT_RCAL_CODE_LEN                          (5U)
#define RF_ANA1_TRF_PTAT_RCAL_CODE_MSK                          (((1U<<RF_ANA1_TRF_PTAT_RCAL_CODE_LEN)-1)<<RF_ANA1_TRF_PTAT_RCAL_CODE_POS)
#define RF_ANA1_TRF_PTAT_RCAL_CODE_UMSK                         (~(((1U<<RF_ANA1_TRF_PTAT_RCAL_CODE_LEN)-1)<<RF_ANA1_TRF_PTAT_RCAL_CODE_POS))
#define RF_ANA1_TRF_PTAT_MODE_SEL                               RF_ANA1_TRF_PTAT_MODE_SEL
#define RF_ANA1_TRF_PTAT_MODE_SEL_POS                           (16U)
#define RF_ANA1_TRF_PTAT_MODE_SEL_LEN                           (1U)
#define RF_ANA1_TRF_PTAT_MODE_SEL_MSK                           (((1U<<RF_ANA1_TRF_PTAT_MODE_SEL_LEN)-1)<<RF_ANA1_TRF_PTAT_MODE_SEL_POS)
#define RF_ANA1_TRF_PTAT_MODE_SEL_UMSK                          (~(((1U<<RF_ANA1_TRF_PTAT_MODE_SEL_LEN)-1)<<RF_ANA1_TRF_PTAT_MODE_SEL_POS))
#define RF_ANA1_TRF_PTAT_2G_5G_SEL                              RF_ANA1_TRF_PTAT_2G_5G_SEL
#define RF_ANA1_TRF_PTAT_2G_5G_SEL_POS                          (20U)
#define RF_ANA1_TRF_PTAT_2G_5G_SEL_LEN                          (1U)
#define RF_ANA1_TRF_PTAT_2G_5G_SEL_MSK                          (((1U<<RF_ANA1_TRF_PTAT_2G_5G_SEL_LEN)-1)<<RF_ANA1_TRF_PTAT_2G_5G_SEL_POS)
#define RF_ANA1_TRF_PTAT_2G_5G_SEL_UMSK                         (~(((1U<<RF_ANA1_TRF_PTAT_2G_5G_SEL_LEN)-1)<<RF_ANA1_TRF_PTAT_2G_5G_SEL_POS))
#define RF_ANA1_TRF_PTAT_EN                                     RF_ANA1_TRF_PTAT_EN
#define RF_ANA1_TRF_PTAT_EN_POS                                 (24U)
#define RF_ANA1_TRF_PTAT_EN_LEN                                 (1U)
#define RF_ANA1_TRF_PTAT_EN_MSK                                 (((1U<<RF_ANA1_TRF_PTAT_EN_LEN)-1)<<RF_ANA1_TRF_PTAT_EN_POS)
#define RF_ANA1_TRF_PTAT_EN_UMSK                                (~(((1U<<RF_ANA1_TRF_PTAT_EN_LEN)-1)<<RF_ANA1_TRF_PTAT_EN_POS))

/* 0x78 : tbb1 */
#define RF_ANA1_TBB1_OFFSET                                     (0x78)
#define RF_ANA1_TBB_VCM                                         RF_ANA1_TBB_VCM
#define RF_ANA1_TBB_VCM_POS                                     (16U)
#define RF_ANA1_TBB_VCM_LEN                                     (2U)
#define RF_ANA1_TBB_VCM_MSK                                     (((1U<<RF_ANA1_TBB_VCM_LEN)-1)<<RF_ANA1_TBB_VCM_POS)
#define RF_ANA1_TBB_VCM_UMSK                                    (~(((1U<<RF_ANA1_TBB_VCM_LEN)-1)<<RF_ANA1_TBB_VCM_POS))
#define RF_ANA1_TBB_CFLT                                        RF_ANA1_TBB_CFLT
#define RF_ANA1_TBB_CFLT_POS                                    (18U)
#define RF_ANA1_TBB_CFLT_LEN                                    (2U)
#define RF_ANA1_TBB_CFLT_MSK                                    (((1U<<RF_ANA1_TBB_CFLT_LEN)-1)<<RF_ANA1_TBB_CFLT_POS)
#define RF_ANA1_TBB_CFLT_UMSK                                   (~(((1U<<RF_ANA1_TBB_CFLT_LEN)-1)<<RF_ANA1_TBB_CFLT_POS))
#define RF_ANA1_TBB_BM_SF                                       RF_ANA1_TBB_BM_SF
#define RF_ANA1_TBB_BM_SF_POS                                   (20U)
#define RF_ANA1_TBB_BM_SF_LEN                                   (2U)
#define RF_ANA1_TBB_BM_SF_MSK                                   (((1U<<RF_ANA1_TBB_BM_SF_LEN)-1)<<RF_ANA1_TBB_BM_SF_POS)
#define RF_ANA1_TBB_BM_SF_UMSK                                  (~(((1U<<RF_ANA1_TBB_BM_SF_LEN)-1)<<RF_ANA1_TBB_BM_SF_POS))
#define RF_ANA1_TBB_BM_CG                                       RF_ANA1_TBB_BM_CG
#define RF_ANA1_TBB_BM_CG_POS                                   (22U)
#define RF_ANA1_TBB_BM_CG_LEN                                   (3U)
#define RF_ANA1_TBB_BM_CG_MSK                                   (((1U<<RF_ANA1_TBB_BM_CG_LEN)-1)<<RF_ANA1_TBB_BM_CG_POS)
#define RF_ANA1_TBB_BM_CG_UMSK                                  (~(((1U<<RF_ANA1_TBB_BM_CG_LEN)-1)<<RF_ANA1_TBB_BM_CG_POS))
#define RF_ANA1_TBB_IQ_BIAS_SHORT                               RF_ANA1_TBB_IQ_BIAS_SHORT
#define RF_ANA1_TBB_IQ_BIAS_SHORT_POS                           (26U)
#define RF_ANA1_TBB_IQ_BIAS_SHORT_LEN                           (1U)
#define RF_ANA1_TBB_IQ_BIAS_SHORT_MSK                           (((1U<<RF_ANA1_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_ANA1_TBB_IQ_BIAS_SHORT_POS)
#define RF_ANA1_TBB_IQ_BIAS_SHORT_UMSK                          (~(((1U<<RF_ANA1_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_ANA1_TBB_IQ_BIAS_SHORT_POS))
#define RF_ANA1_TBB_ATEST_OUT_EN                                RF_ANA1_TBB_ATEST_OUT_EN
#define RF_ANA1_TBB_ATEST_OUT_EN_POS                            (27U)
#define RF_ANA1_TBB_ATEST_OUT_EN_LEN                            (1U)
#define RF_ANA1_TBB_ATEST_OUT_EN_MSK                            (((1U<<RF_ANA1_TBB_ATEST_OUT_EN_LEN)-1)<<RF_ANA1_TBB_ATEST_OUT_EN_POS)
#define RF_ANA1_TBB_ATEST_OUT_EN_UMSK                           (~(((1U<<RF_ANA1_TBB_ATEST_OUT_EN_LEN)-1)<<RF_ANA1_TBB_ATEST_OUT_EN_POS))
#define RF_ANA1_TBB_ATEST_RES_SEL                               RF_ANA1_TBB_ATEST_RES_SEL
#define RF_ANA1_TBB_ATEST_RES_SEL_POS                           (28U)
#define RF_ANA1_TBB_ATEST_RES_SEL_LEN                           (3U)
#define RF_ANA1_TBB_ATEST_RES_SEL_MSK                           (((1U<<RF_ANA1_TBB_ATEST_RES_SEL_LEN)-1)<<RF_ANA1_TBB_ATEST_RES_SEL_POS)
#define RF_ANA1_TBB_ATEST_RES_SEL_UMSK                          (~(((1U<<RF_ANA1_TBB_ATEST_RES_SEL_LEN)-1)<<RF_ANA1_TBB_ATEST_RES_SEL_POS))
#define RF_ANA1_TBB_ATEST_IN_EN                                 RF_ANA1_TBB_ATEST_IN_EN
#define RF_ANA1_TBB_ATEST_IN_EN_POS                             (31U)
#define RF_ANA1_TBB_ATEST_IN_EN_LEN                             (1U)
#define RF_ANA1_TBB_ATEST_IN_EN_MSK                             (((1U<<RF_ANA1_TBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_TBB_ATEST_IN_EN_POS)
#define RF_ANA1_TBB_ATEST_IN_EN_UMSK                            (~(((1U<<RF_ANA1_TBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_TBB_ATEST_IN_EN_POS))

/* 0x7C : tbb2 */
#define RF_ANA1_TBB2_OFFSET                                     (0x7C)
#define RF_ANA1_TOSDAC_Q                                        RF_ANA1_TOSDAC_Q
#define RF_ANA1_TOSDAC_Q_POS                                    (0U)
#define RF_ANA1_TOSDAC_Q_LEN                                    (6U)
#define RF_ANA1_TOSDAC_Q_MSK                                    (((1U<<RF_ANA1_TOSDAC_Q_LEN)-1)<<RF_ANA1_TOSDAC_Q_POS)
#define RF_ANA1_TOSDAC_Q_UMSK                                   (~(((1U<<RF_ANA1_TOSDAC_Q_LEN)-1)<<RF_ANA1_TOSDAC_Q_POS))
#define RF_ANA1_TOSDAC_I                                        RF_ANA1_TOSDAC_I
#define RF_ANA1_TOSDAC_I_POS                                    (8U)
#define RF_ANA1_TOSDAC_I_LEN                                    (6U)
#define RF_ANA1_TOSDAC_I_MSK                                    (((1U<<RF_ANA1_TOSDAC_I_LEN)-1)<<RF_ANA1_TOSDAC_I_POS)
#define RF_ANA1_TOSDAC_I_UMSK                                   (~(((1U<<RF_ANA1_TOSDAC_I_LEN)-1)<<RF_ANA1_TOSDAC_I_POS))
#define RF_ANA1_TOSDAC_Q_HW                                     RF_ANA1_TOSDAC_Q_HW
#define RF_ANA1_TOSDAC_Q_HW_POS                                 (16U)
#define RF_ANA1_TOSDAC_Q_HW_LEN                                 (6U)
#define RF_ANA1_TOSDAC_Q_HW_MSK                                 (((1U<<RF_ANA1_TOSDAC_Q_HW_LEN)-1)<<RF_ANA1_TOSDAC_Q_HW_POS)
#define RF_ANA1_TOSDAC_Q_HW_UMSK                                (~(((1U<<RF_ANA1_TOSDAC_Q_HW_LEN)-1)<<RF_ANA1_TOSDAC_Q_HW_POS))
#define RF_ANA1_TOSDAC_I_HW                                     RF_ANA1_TOSDAC_I_HW
#define RF_ANA1_TOSDAC_I_HW_POS                                 (24U)
#define RF_ANA1_TOSDAC_I_HW_LEN                                 (6U)
#define RF_ANA1_TOSDAC_I_HW_MSK                                 (((1U<<RF_ANA1_TOSDAC_I_HW_LEN)-1)<<RF_ANA1_TOSDAC_I_HW_POS)
#define RF_ANA1_TOSDAC_I_HW_UMSK                                (~(((1U<<RF_ANA1_TOSDAC_I_HW_LEN)-1)<<RF_ANA1_TOSDAC_I_HW_POS))

/* 0x80 : rrf1 */
#define RF_ANA1_RRF1_OFFSET                                     (0x80)
#define RF_ANA1_LNA_BM                                          RF_ANA1_LNA_BM
#define RF_ANA1_LNA_BM_POS                                      (0U)
#define RF_ANA1_LNA_BM_LEN                                      (4U)
#define RF_ANA1_LNA_BM_MSK                                      (((1U<<RF_ANA1_LNA_BM_LEN)-1)<<RF_ANA1_LNA_BM_POS)
#define RF_ANA1_LNA_BM_UMSK                                     (~(((1U<<RF_ANA1_LNA_BM_LEN)-1)<<RF_ANA1_LNA_BM_POS))
#define RF_ANA1_LNA_BM_HW                                       RF_ANA1_LNA_BM_HW
#define RF_ANA1_LNA_BM_HW_POS                                   (4U)
#define RF_ANA1_LNA_BM_HW_LEN                                   (4U)
#define RF_ANA1_LNA_BM_HW_MSK                                   (((1U<<RF_ANA1_LNA_BM_HW_LEN)-1)<<RF_ANA1_LNA_BM_HW_POS)
#define RF_ANA1_LNA_BM_HW_UMSK                                  (~(((1U<<RF_ANA1_LNA_BM_HW_LEN)-1)<<RF_ANA1_LNA_BM_HW_POS))
#define RF_ANA1_LNA_LOAD_CSW                                    RF_ANA1_LNA_LOAD_CSW
#define RF_ANA1_LNA_LOAD_CSW_POS                                (8U)
#define RF_ANA1_LNA_LOAD_CSW_LEN                                (4U)
#define RF_ANA1_LNA_LOAD_CSW_MSK                                (((1U<<RF_ANA1_LNA_LOAD_CSW_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_POS)
#define RF_ANA1_LNA_LOAD_CSW_UMSK                               (~(((1U<<RF_ANA1_LNA_LOAD_CSW_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_POS))
#define RF_ANA1_LNA_LOAD_CSW_HW                                 RF_ANA1_LNA_LOAD_CSW_HW
#define RF_ANA1_LNA_LOAD_CSW_HW_POS                             (12U)
#define RF_ANA1_LNA_LOAD_CSW_HW_LEN                             (4U)
#define RF_ANA1_LNA_LOAD_CSW_HW_MSK                             (((1U<<RF_ANA1_LNA_LOAD_CSW_HW_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_HW_POS)
#define RF_ANA1_LNA_LOAD_CSW_HW_UMSK                            (~(((1U<<RF_ANA1_LNA_LOAD_CSW_HW_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_HW_POS))
#define RF_ANA1_LNA_LOAD_CSW_GC                                 RF_ANA1_LNA_LOAD_CSW_GC
#define RF_ANA1_LNA_LOAD_CSW_GC_POS                             (16U)
#define RF_ANA1_LNA_LOAD_CSW_GC_LEN                             (2U)
#define RF_ANA1_LNA_LOAD_CSW_GC_MSK                             (((1U<<RF_ANA1_LNA_LOAD_CSW_GC_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_GC_POS)
#define RF_ANA1_LNA_LOAD_CSW_GC_UMSK                            (~(((1U<<RF_ANA1_LNA_LOAD_CSW_GC_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_GC_POS))
#define RF_ANA1_LNA_RFB1                                        RF_ANA1_LNA_RFB1
#define RF_ANA1_LNA_RFB1_POS                                    (20U)
#define RF_ANA1_LNA_RFB1_LEN                                    (3U)
#define RF_ANA1_LNA_RFB1_MSK                                    (((1U<<RF_ANA1_LNA_RFB1_LEN)-1)<<RF_ANA1_LNA_RFB1_POS)
#define RF_ANA1_LNA_RFB1_UMSK                                   (~(((1U<<RF_ANA1_LNA_RFB1_LEN)-1)<<RF_ANA1_LNA_RFB1_POS))
#define RF_ANA1_LNA_RFB2                                        RF_ANA1_LNA_RFB2
#define RF_ANA1_LNA_RFB2_POS                                    (24U)
#define RF_ANA1_LNA_RFB2_LEN                                    (3U)
#define RF_ANA1_LNA_RFB2_MSK                                    (((1U<<RF_ANA1_LNA_RFB2_LEN)-1)<<RF_ANA1_LNA_RFB2_POS)
#define RF_ANA1_LNA_RFB2_UMSK                                   (~(((1U<<RF_ANA1_LNA_RFB2_LEN)-1)<<RF_ANA1_LNA_RFB2_POS))
#define RF_ANA1_LNA_RFB3                                        RF_ANA1_LNA_RFB3
#define RF_ANA1_LNA_RFB3_POS                                    (28U)
#define RF_ANA1_LNA_RFB3_LEN                                    (3U)
#define RF_ANA1_LNA_RFB3_MSK                                    (((1U<<RF_ANA1_LNA_RFB3_LEN)-1)<<RF_ANA1_LNA_RFB3_POS)
#define RF_ANA1_LNA_RFB3_UMSK                                   (~(((1U<<RF_ANA1_LNA_RFB3_LEN)-1)<<RF_ANA1_LNA_RFB3_POS))

/* 0x84 : rrf2 */
#define RF_ANA1_RRF2_OFFSET                                     (0x84)
#define RF_ANA1_LNA_SEL                                         RF_ANA1_LNA_SEL
#define RF_ANA1_LNA_SEL_POS                                     (0U)
#define RF_ANA1_LNA_SEL_LEN                                     (1U)
#define RF_ANA1_LNA_SEL_MSK                                     (((1U<<RF_ANA1_LNA_SEL_LEN)-1)<<RF_ANA1_LNA_SEL_POS)
#define RF_ANA1_LNA_SEL_UMSK                                    (~(((1U<<RF_ANA1_LNA_SEL_LEN)-1)<<RF_ANA1_LNA_SEL_POS))
#define RF_ANA1_LNA_SEL_HW                                      RF_ANA1_LNA_SEL_HW
#define RF_ANA1_LNA_SEL_HW_POS                                  (1U)
#define RF_ANA1_LNA_SEL_HW_LEN                                  (1U)
#define RF_ANA1_LNA_SEL_HW_MSK                                  (((1U<<RF_ANA1_LNA_SEL_HW_LEN)-1)<<RF_ANA1_LNA_SEL_HW_POS)
#define RF_ANA1_LNA_SEL_HW_UMSK                                 (~(((1U<<RF_ANA1_LNA_SEL_HW_LEN)-1)<<RF_ANA1_LNA_SEL_HW_POS))
#define RF_ANA1_LNA_CASBIAS_SEL                                 RF_ANA1_LNA_CASBIAS_SEL
#define RF_ANA1_LNA_CASBIAS_SEL_POS                             (2U)
#define RF_ANA1_LNA_CASBIAS_SEL_LEN                             (2U)
#define RF_ANA1_LNA_CASBIAS_SEL_MSK                             (((1U<<RF_ANA1_LNA_CASBIAS_SEL_LEN)-1)<<RF_ANA1_LNA_CASBIAS_SEL_POS)
#define RF_ANA1_LNA_CASBIAS_SEL_UMSK                            (~(((1U<<RF_ANA1_LNA_CASBIAS_SEL_LEN)-1)<<RF_ANA1_LNA_CASBIAS_SEL_POS))
#define RF_ANA1_LNA_CAP1                                        RF_ANA1_LNA_CAP1
#define RF_ANA1_LNA_CAP1_POS                                    (4U)
#define RF_ANA1_LNA_CAP1_LEN                                    (3U)
#define RF_ANA1_LNA_CAP1_MSK                                    (((1U<<RF_ANA1_LNA_CAP1_LEN)-1)<<RF_ANA1_LNA_CAP1_POS)
#define RF_ANA1_LNA_CAP1_UMSK                                   (~(((1U<<RF_ANA1_LNA_CAP1_LEN)-1)<<RF_ANA1_LNA_CAP1_POS))
#define RF_ANA1_LNA_CAP2                                        RF_ANA1_LNA_CAP2
#define RF_ANA1_LNA_CAP2_POS                                    (8U)
#define RF_ANA1_LNA_CAP2_LEN                                    (3U)
#define RF_ANA1_LNA_CAP2_MSK                                    (((1U<<RF_ANA1_LNA_CAP2_LEN)-1)<<RF_ANA1_LNA_CAP2_POS)
#define RF_ANA1_LNA_CAP2_UMSK                                   (~(((1U<<RF_ANA1_LNA_CAP2_LEN)-1)<<RF_ANA1_LNA_CAP2_POS))
#define RF_ANA1_LNA_LG_CAP                                      RF_ANA1_LNA_LG_CAP
#define RF_ANA1_LNA_LG_CAP_POS                                  (12U)
#define RF_ANA1_LNA_LG_CAP_LEN                                  (2U)
#define RF_ANA1_LNA_LG_CAP_MSK                                  (((1U<<RF_ANA1_LNA_LG_CAP_LEN)-1)<<RF_ANA1_LNA_LG_CAP_POS)
#define RF_ANA1_LNA_LG_CAP_UMSK                                 (~(((1U<<RF_ANA1_LNA_LG_CAP_LEN)-1)<<RF_ANA1_LNA_LG_CAP_POS))
#define RF_ANA1_LNA_BM_BOOST                                    RF_ANA1_LNA_BM_BOOST
#define RF_ANA1_LNA_BM_BOOST_POS                                (16U)
#define RF_ANA1_LNA_BM_BOOST_LEN                                (1U)
#define RF_ANA1_LNA_BM_BOOST_MSK                                (((1U<<RF_ANA1_LNA_BM_BOOST_LEN)-1)<<RF_ANA1_LNA_BM_BOOST_POS)
#define RF_ANA1_LNA_BM_BOOST_UMSK                               (~(((1U<<RF_ANA1_LNA_BM_BOOST_LEN)-1)<<RF_ANA1_LNA_BM_BOOST_POS))
#define RF_ANA1_RRF_BM_BOOST_SEL                                RF_ANA1_RRF_BM_BOOST_SEL
#define RF_ANA1_RRF_BM_BOOST_SEL_POS                            (18U)
#define RF_ANA1_RRF_BM_BOOST_SEL_LEN                            (1U)
#define RF_ANA1_RRF_BM_BOOST_SEL_MSK                            (((1U<<RF_ANA1_RRF_BM_BOOST_SEL_LEN)-1)<<RF_ANA1_RRF_BM_BOOST_SEL_POS)
#define RF_ANA1_RRF_BM_BOOST_SEL_UMSK                           (~(((1U<<RF_ANA1_RRF_BM_BOOST_SEL_LEN)-1)<<RF_ANA1_RRF_BM_BOOST_SEL_POS))
#define RF_ANA1_LNA_CAP1_LG                                     RF_ANA1_LNA_CAP1_LG
#define RF_ANA1_LNA_CAP1_LG_POS                                 (20U)
#define RF_ANA1_LNA_CAP1_LG_LEN                                 (3U)
#define RF_ANA1_LNA_CAP1_LG_MSK                                 (((1U<<RF_ANA1_LNA_CAP1_LG_LEN)-1)<<RF_ANA1_LNA_CAP1_LG_POS)
#define RF_ANA1_LNA_CAP1_LG_UMSK                                (~(((1U<<RF_ANA1_LNA_CAP1_LG_LEN)-1)<<RF_ANA1_LNA_CAP1_LG_POS))
#define RF_ANA1_LNA_CAP1_HG                                     RF_ANA1_LNA_CAP1_HG
#define RF_ANA1_LNA_CAP1_HG_POS                                 (24U)
#define RF_ANA1_LNA_CAP1_HG_LEN                                 (3U)
#define RF_ANA1_LNA_CAP1_HG_MSK                                 (((1U<<RF_ANA1_LNA_CAP1_HG_LEN)-1)<<RF_ANA1_LNA_CAP1_HG_POS)
#define RF_ANA1_LNA_CAP1_HG_UMSK                                (~(((1U<<RF_ANA1_LNA_CAP1_HG_LEN)-1)<<RF_ANA1_LNA_CAP1_HG_POS))
#define RF_ANA1_LNA_CAP1_HW                                     RF_ANA1_LNA_CAP1_HW
#define RF_ANA1_LNA_CAP1_HW_POS                                 (28U)
#define RF_ANA1_LNA_CAP1_HW_LEN                                 (3U)
#define RF_ANA1_LNA_CAP1_HW_MSK                                 (((1U<<RF_ANA1_LNA_CAP1_HW_LEN)-1)<<RF_ANA1_LNA_CAP1_HW_POS)
#define RF_ANA1_LNA_CAP1_HW_UMSK                                (~(((1U<<RF_ANA1_LNA_CAP1_HW_LEN)-1)<<RF_ANA1_LNA_CAP1_HW_POS))

/* 0x88 : rrf3 */
#define RF_ANA1_RRF3_OFFSET                                     (0x88)
#define RF_ANA1_LNA_LOAD_CSW_LG                                 RF_ANA1_LNA_LOAD_CSW_LG
#define RF_ANA1_LNA_LOAD_CSW_LG_POS                             (0U)
#define RF_ANA1_LNA_LOAD_CSW_LG_LEN                             (4U)
#define RF_ANA1_LNA_LOAD_CSW_LG_MSK                             (((1U<<RF_ANA1_LNA_LOAD_CSW_LG_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_LG_POS)
#define RF_ANA1_LNA_LOAD_CSW_LG_UMSK                            (~(((1U<<RF_ANA1_LNA_LOAD_CSW_LG_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_LG_POS))
#define RF_ANA1_LNA_LOAD_CSW_HG                                 RF_ANA1_LNA_LOAD_CSW_HG
#define RF_ANA1_LNA_LOAD_CSW_HG_POS                             (4U)
#define RF_ANA1_LNA_LOAD_CSW_HG_LEN                             (4U)
#define RF_ANA1_LNA_LOAD_CSW_HG_MSK                             (((1U<<RF_ANA1_LNA_LOAD_CSW_HG_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_HG_POS)
#define RF_ANA1_LNA_LOAD_CSW_HG_UMSK                            (~(((1U<<RF_ANA1_LNA_LOAD_CSW_HG_LEN)-1)<<RF_ANA1_LNA_LOAD_CSW_HG_POS))
#define RF_ANA1_LNA_BM_LG                                       RF_ANA1_LNA_BM_LG
#define RF_ANA1_LNA_BM_LG_POS                                   (8U)
#define RF_ANA1_LNA_BM_LG_LEN                                   (4U)
#define RF_ANA1_LNA_BM_LG_MSK                                   (((1U<<RF_ANA1_LNA_BM_LG_LEN)-1)<<RF_ANA1_LNA_BM_LG_POS)
#define RF_ANA1_LNA_BM_LG_UMSK                                  (~(((1U<<RF_ANA1_LNA_BM_LG_LEN)-1)<<RF_ANA1_LNA_BM_LG_POS))
#define RF_ANA1_LNA_BM_LG_LPMODE                                RF_ANA1_LNA_BM_LG_LPMODE
#define RF_ANA1_LNA_BM_LG_LPMODE_POS                            (12U)
#define RF_ANA1_LNA_BM_LG_LPMODE_LEN                            (4U)
#define RF_ANA1_LNA_BM_LG_LPMODE_MSK                            (((1U<<RF_ANA1_LNA_BM_LG_LPMODE_LEN)-1)<<RF_ANA1_LNA_BM_LG_LPMODE_POS)
#define RF_ANA1_LNA_BM_LG_LPMODE_UMSK                           (~(((1U<<RF_ANA1_LNA_BM_LG_LPMODE_LEN)-1)<<RF_ANA1_LNA_BM_LG_LPMODE_POS))
#define RF_ANA1_LNA_BM_HG                                       RF_ANA1_LNA_BM_HG
#define RF_ANA1_LNA_BM_HG_POS                                   (16U)
#define RF_ANA1_LNA_BM_HG_LEN                                   (4U)
#define RF_ANA1_LNA_BM_HG_MSK                                   (((1U<<RF_ANA1_LNA_BM_HG_LEN)-1)<<RF_ANA1_LNA_BM_HG_POS)
#define RF_ANA1_LNA_BM_HG_UMSK                                  (~(((1U<<RF_ANA1_LNA_BM_HG_LEN)-1)<<RF_ANA1_LNA_BM_HG_POS))
#define RF_ANA1_LNA_BM_HG_LPMODE                                RF_ANA1_LNA_BM_HG_LPMODE
#define RF_ANA1_LNA_BM_HG_LPMODE_POS                            (20U)
#define RF_ANA1_LNA_BM_HG_LPMODE_LEN                            (4U)
#define RF_ANA1_LNA_BM_HG_LPMODE_MSK                            (((1U<<RF_ANA1_LNA_BM_HG_LPMODE_LEN)-1)<<RF_ANA1_LNA_BM_HG_LPMODE_POS)
#define RF_ANA1_LNA_BM_HG_LPMODE_UMSK                           (~(((1U<<RF_ANA1_LNA_BM_HG_LPMODE_LEN)-1)<<RF_ANA1_LNA_BM_HG_LPMODE_POS))

/* 0x8C : rrf4 */
#define RF_ANA1_RRF4_OFFSET                                     (0x8C)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN                             RF_ANA1_LNA_PWRDET_OUT_RSTN
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_POS                         (0U)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_LEN                         (1U)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_MSK                         (((1U<<RF_ANA1_LNA_PWRDET_OUT_RSTN_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RSTN_POS)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_UMSK                        (~(((1U<<RF_ANA1_LNA_PWRDET_OUT_RSTN_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RSTN_POS))
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_HW                          RF_ANA1_LNA_PWRDET_OUT_RSTN_HW
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_POS                      (1U)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_LEN                      (1U)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_MSK                      (((1U<<RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_POS)
#define RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_UMSK                     (~(((1U<<RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RSTN_HW_POS))
#define RF_ANA1_LNA_PWRDET_OUT_RAW                              RF_ANA1_LNA_PWRDET_OUT_RAW
#define RF_ANA1_LNA_PWRDET_OUT_RAW_POS                          (2U)
#define RF_ANA1_LNA_PWRDET_OUT_RAW_LEN                          (1U)
#define RF_ANA1_LNA_PWRDET_OUT_RAW_MSK                          (((1U<<RF_ANA1_LNA_PWRDET_OUT_RAW_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RAW_POS)
#define RF_ANA1_LNA_PWRDET_OUT_RAW_UMSK                         (~(((1U<<RF_ANA1_LNA_PWRDET_OUT_RAW_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_RAW_POS))
#define RF_ANA1_LNA_PWRDET_OUT_LATCH                            RF_ANA1_LNA_PWRDET_OUT_LATCH
#define RF_ANA1_LNA_PWRDET_OUT_LATCH_POS                        (3U)
#define RF_ANA1_LNA_PWRDET_OUT_LATCH_LEN                        (1U)
#define RF_ANA1_LNA_PWRDET_OUT_LATCH_MSK                        (((1U<<RF_ANA1_LNA_PWRDET_OUT_LATCH_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_LATCH_POS)
#define RF_ANA1_LNA_PWRDET_OUT_LATCH_UMSK                       (~(((1U<<RF_ANA1_LNA_PWRDET_OUT_LATCH_LEN)-1)<<RF_ANA1_LNA_PWRDET_OUT_LATCH_POS))
#define RF_ANA1_LNA_PWRDET_RC                                   RF_ANA1_LNA_PWRDET_RC
#define RF_ANA1_LNA_PWRDET_RC_POS                               (4U)
#define RF_ANA1_LNA_PWRDET_RC_LEN                               (2U)
#define RF_ANA1_LNA_PWRDET_RC_MSK                               (((1U<<RF_ANA1_LNA_PWRDET_RC_LEN)-1)<<RF_ANA1_LNA_PWRDET_RC_POS)
#define RF_ANA1_LNA_PWRDET_RC_UMSK                              (~(((1U<<RF_ANA1_LNA_PWRDET_RC_LEN)-1)<<RF_ANA1_LNA_PWRDET_RC_POS))
#define RF_ANA1_LNA_PWRDET_VOUT_RBANK                           RF_ANA1_LNA_PWRDET_VOUT_RBANK
#define RF_ANA1_LNA_PWRDET_VOUT_RBANK_POS                       (6U)
#define RF_ANA1_LNA_PWRDET_VOUT_RBANK_LEN                       (2U)
#define RF_ANA1_LNA_PWRDET_VOUT_RBANK_MSK                       (((1U<<RF_ANA1_LNA_PWRDET_VOUT_RBANK_LEN)-1)<<RF_ANA1_LNA_PWRDET_VOUT_RBANK_POS)
#define RF_ANA1_LNA_PWRDET_VOUT_RBANK_UMSK                      (~(((1U<<RF_ANA1_LNA_PWRDET_VOUT_RBANK_LEN)-1)<<RF_ANA1_LNA_PWRDET_VOUT_RBANK_POS))
#define RF_ANA1_LNA_PWRDET_COMP_VREF                            RF_ANA1_LNA_PWRDET_COMP_VREF
#define RF_ANA1_LNA_PWRDET_COMP_VREF_POS                        (8U)
#define RF_ANA1_LNA_PWRDET_COMP_VREF_LEN                        (2U)
#define RF_ANA1_LNA_PWRDET_COMP_VREF_MSK                        (((1U<<RF_ANA1_LNA_PWRDET_COMP_VREF_LEN)-1)<<RF_ANA1_LNA_PWRDET_COMP_VREF_POS)
#define RF_ANA1_LNA_PWRDET_COMP_VREF_UMSK                       (~(((1U<<RF_ANA1_LNA_PWRDET_COMP_VREF_LEN)-1)<<RF_ANA1_LNA_PWRDET_COMP_VREF_POS))
#define RF_ANA1_LNA_PWRDET_OSDAC                                RF_ANA1_LNA_PWRDET_OSDAC
#define RF_ANA1_LNA_PWRDET_OSDAC_POS                            (10U)
#define RF_ANA1_LNA_PWRDET_OSDAC_LEN                            (6U)
#define RF_ANA1_LNA_PWRDET_OSDAC_MSK                            (((1U<<RF_ANA1_LNA_PWRDET_OSDAC_LEN)-1)<<RF_ANA1_LNA_PWRDET_OSDAC_POS)
#define RF_ANA1_LNA_PWRDET_OSDAC_UMSK                           (~(((1U<<RF_ANA1_LNA_PWRDET_OSDAC_LEN)-1)<<RF_ANA1_LNA_PWRDET_OSDAC_POS))
#define RF_ANA1_RMXGM_BM_BOOST                                  RF_ANA1_RMXGM_BM_BOOST
#define RF_ANA1_RMXGM_BM_BOOST_POS                              (16U)
#define RF_ANA1_RMXGM_BM_BOOST_LEN                              (1U)
#define RF_ANA1_RMXGM_BM_BOOST_MSK                              (((1U<<RF_ANA1_RMXGM_BM_BOOST_LEN)-1)<<RF_ANA1_RMXGM_BM_BOOST_POS)
#define RF_ANA1_RMXGM_BM_BOOST_UMSK                             (~(((1U<<RF_ANA1_RMXGM_BM_BOOST_LEN)-1)<<RF_ANA1_RMXGM_BM_BOOST_POS))
#define RF_ANA1_RMXGM_BM                                        RF_ANA1_RMXGM_BM
#define RF_ANA1_RMXGM_BM_POS                                    (20U)
#define RF_ANA1_RMXGM_BM_LEN                                    (3U)
#define RF_ANA1_RMXGM_BM_MSK                                    (((1U<<RF_ANA1_RMXGM_BM_LEN)-1)<<RF_ANA1_RMXGM_BM_POS)
#define RF_ANA1_RMXGM_BM_UMSK                                   (~(((1U<<RF_ANA1_RMXGM_BM_LEN)-1)<<RF_ANA1_RMXGM_BM_POS))
#define RF_ANA1_RMXGM_CASBIAS_SEL                               RF_ANA1_RMXGM_CASBIAS_SEL
#define RF_ANA1_RMXGM_CASBIAS_SEL_POS                           (24U)
#define RF_ANA1_RMXGM_CASBIAS_SEL_LEN                           (2U)
#define RF_ANA1_RMXGM_CASBIAS_SEL_MSK                           (((1U<<RF_ANA1_RMXGM_CASBIAS_SEL_LEN)-1)<<RF_ANA1_RMXGM_CASBIAS_SEL_POS)
#define RF_ANA1_RMXGM_CASBIAS_SEL_UMSK                          (~(((1U<<RF_ANA1_RMXGM_CASBIAS_SEL_LEN)-1)<<RF_ANA1_RMXGM_CASBIAS_SEL_POS))
#define RF_ANA1_RMX_BM                                          RF_ANA1_RMX_BM
#define RF_ANA1_RMX_BM_POS                                      (28U)
#define RF_ANA1_RMX_BM_LEN                                      (3U)
#define RF_ANA1_RMX_BM_MSK                                      (((1U<<RF_ANA1_RMX_BM_LEN)-1)<<RF_ANA1_RMX_BM_POS)
#define RF_ANA1_RMX_BM_UMSK                                     (~(((1U<<RF_ANA1_RMX_BM_LEN)-1)<<RF_ANA1_RMX_BM_POS))

/* 0x90 : rbb1 */
#define RF_ANA1_RBB1_OFFSET                                     (0x90)
#define RF_ANA1_ROSDAC_Q                                        RF_ANA1_ROSDAC_Q
#define RF_ANA1_ROSDAC_Q_POS                                    (0U)
#define RF_ANA1_ROSDAC_Q_LEN                                    (6U)
#define RF_ANA1_ROSDAC_Q_MSK                                    (((1U<<RF_ANA1_ROSDAC_Q_LEN)-1)<<RF_ANA1_ROSDAC_Q_POS)
#define RF_ANA1_ROSDAC_Q_UMSK                                   (~(((1U<<RF_ANA1_ROSDAC_Q_LEN)-1)<<RF_ANA1_ROSDAC_Q_POS))
#define RF_ANA1_ROSDAC_I                                        RF_ANA1_ROSDAC_I
#define RF_ANA1_ROSDAC_I_POS                                    (8U)
#define RF_ANA1_ROSDAC_I_LEN                                    (6U)
#define RF_ANA1_ROSDAC_I_MSK                                    (((1U<<RF_ANA1_ROSDAC_I_LEN)-1)<<RF_ANA1_ROSDAC_I_POS)
#define RF_ANA1_ROSDAC_I_UMSK                                   (~(((1U<<RF_ANA1_ROSDAC_I_LEN)-1)<<RF_ANA1_ROSDAC_I_POS))
#define RF_ANA1_ROSDAC_Q_HW                                     RF_ANA1_ROSDAC_Q_HW
#define RF_ANA1_ROSDAC_Q_HW_POS                                 (16U)
#define RF_ANA1_ROSDAC_Q_HW_LEN                                 (6U)
#define RF_ANA1_ROSDAC_Q_HW_MSK                                 (((1U<<RF_ANA1_ROSDAC_Q_HW_LEN)-1)<<RF_ANA1_ROSDAC_Q_HW_POS)
#define RF_ANA1_ROSDAC_Q_HW_UMSK                                (~(((1U<<RF_ANA1_ROSDAC_Q_HW_LEN)-1)<<RF_ANA1_ROSDAC_Q_HW_POS))
#define RF_ANA1_ROSDAC_I_HW                                     RF_ANA1_ROSDAC_I_HW
#define RF_ANA1_ROSDAC_I_HW_POS                                 (24U)
#define RF_ANA1_ROSDAC_I_HW_LEN                                 (6U)
#define RF_ANA1_ROSDAC_I_HW_MSK                                 (((1U<<RF_ANA1_ROSDAC_I_HW_LEN)-1)<<RF_ANA1_ROSDAC_I_HW_POS)
#define RF_ANA1_ROSDAC_I_HW_UMSK                                (~(((1U<<RF_ANA1_ROSDAC_I_HW_LEN)-1)<<RF_ANA1_ROSDAC_I_HW_POS))
#define RF_ANA1_ROSDAC_RANGE                                    RF_ANA1_ROSDAC_RANGE
#define RF_ANA1_ROSDAC_RANGE_POS                                (30U)
#define RF_ANA1_ROSDAC_RANGE_LEN                                (1U)
#define RF_ANA1_ROSDAC_RANGE_MSK                                (((1U<<RF_ANA1_ROSDAC_RANGE_LEN)-1)<<RF_ANA1_ROSDAC_RANGE_POS)
#define RF_ANA1_ROSDAC_RANGE_UMSK                               (~(((1U<<RF_ANA1_ROSDAC_RANGE_LEN)-1)<<RF_ANA1_ROSDAC_RANGE_POS))

/* 0x94 : rbb2 */
#define RF_ANA1_RBB2_OFFSET                                     (0x94)
#define RF_ANA1_RBB_CAP2_FC_Q                                   RF_ANA1_RBB_CAP2_FC_Q
#define RF_ANA1_RBB_CAP2_FC_Q_POS                               (0U)
#define RF_ANA1_RBB_CAP2_FC_Q_LEN                               (7U)
#define RF_ANA1_RBB_CAP2_FC_Q_MSK                               (((1U<<RF_ANA1_RBB_CAP2_FC_Q_LEN)-1)<<RF_ANA1_RBB_CAP2_FC_Q_POS)
#define RF_ANA1_RBB_CAP2_FC_Q_UMSK                              (~(((1U<<RF_ANA1_RBB_CAP2_FC_Q_LEN)-1)<<RF_ANA1_RBB_CAP2_FC_Q_POS))
#define RF_ANA1_RBB_CAP2_FC_I                                   RF_ANA1_RBB_CAP2_FC_I
#define RF_ANA1_RBB_CAP2_FC_I_POS                               (8U)
#define RF_ANA1_RBB_CAP2_FC_I_LEN                               (7U)
#define RF_ANA1_RBB_CAP2_FC_I_MSK                               (((1U<<RF_ANA1_RBB_CAP2_FC_I_LEN)-1)<<RF_ANA1_RBB_CAP2_FC_I_POS)
#define RF_ANA1_RBB_CAP2_FC_I_UMSK                              (~(((1U<<RF_ANA1_RBB_CAP2_FC_I_LEN)-1)<<RF_ANA1_RBB_CAP2_FC_I_POS))
#define RF_ANA1_RBB_CAP1_FC_Q                                   RF_ANA1_RBB_CAP1_FC_Q
#define RF_ANA1_RBB_CAP1_FC_Q_POS                               (16U)
#define RF_ANA1_RBB_CAP1_FC_Q_LEN                               (7U)
#define RF_ANA1_RBB_CAP1_FC_Q_MSK                               (((1U<<RF_ANA1_RBB_CAP1_FC_Q_LEN)-1)<<RF_ANA1_RBB_CAP1_FC_Q_POS)
#define RF_ANA1_RBB_CAP1_FC_Q_UMSK                              (~(((1U<<RF_ANA1_RBB_CAP1_FC_Q_LEN)-1)<<RF_ANA1_RBB_CAP1_FC_Q_POS))
#define RF_ANA1_RBB_CAP1_FC_I                                   RF_ANA1_RBB_CAP1_FC_I
#define RF_ANA1_RBB_CAP1_FC_I_POS                               (24U)
#define RF_ANA1_RBB_CAP1_FC_I_LEN                               (7U)
#define RF_ANA1_RBB_CAP1_FC_I_MSK                               (((1U<<RF_ANA1_RBB_CAP1_FC_I_LEN)-1)<<RF_ANA1_RBB_CAP1_FC_I_POS)
#define RF_ANA1_RBB_CAP1_FC_I_UMSK                              (~(((1U<<RF_ANA1_RBB_CAP1_FC_I_LEN)-1)<<RF_ANA1_RBB_CAP1_FC_I_POS))

/* 0x98 : rbb3 */
#define RF_ANA1_RBB3_OFFSET                                     (0x98)
#define RF_ANA1_RBB_RCCAL_IQ_SWAP                               RF_ANA1_RBB_RCCAL_IQ_SWAP
#define RF_ANA1_RBB_RCCAL_IQ_SWAP_POS                           (0U)
#define RF_ANA1_RBB_RCCAL_IQ_SWAP_LEN                           (1U)
#define RF_ANA1_RBB_RCCAL_IQ_SWAP_MSK                           (((1U<<RF_ANA1_RBB_RCCAL_IQ_SWAP_LEN)-1)<<RF_ANA1_RBB_RCCAL_IQ_SWAP_POS)
#define RF_ANA1_RBB_RCCAL_IQ_SWAP_UMSK                          (~(((1U<<RF_ANA1_RBB_RCCAL_IQ_SWAP_LEN)-1)<<RF_ANA1_RBB_RCCAL_IQ_SWAP_POS))
#define RF_ANA1_RBB_PWR_DET_EN                                  RF_ANA1_RBB_PWR_DET_EN
#define RF_ANA1_RBB_PWR_DET_EN_POS                              (1U)
#define RF_ANA1_RBB_PWR_DET_EN_LEN                              (1U)
#define RF_ANA1_RBB_PWR_DET_EN_MSK                              (((1U<<RF_ANA1_RBB_PWR_DET_EN_LEN)-1)<<RF_ANA1_RBB_PWR_DET_EN_POS)
#define RF_ANA1_RBB_PWR_DET_EN_UMSK                             (~(((1U<<RF_ANA1_RBB_PWR_DET_EN_LEN)-1)<<RF_ANA1_RBB_PWR_DET_EN_POS))
#define RF_ANA1_RBB_BQ_IQBIAS_SHORT                             RF_ANA1_RBB_BQ_IQBIAS_SHORT
#define RF_ANA1_RBB_BQ_IQBIAS_SHORT_POS                         (2U)
#define RF_ANA1_RBB_BQ_IQBIAS_SHORT_LEN                         (1U)
#define RF_ANA1_RBB_BQ_IQBIAS_SHORT_MSK                         (((1U<<RF_ANA1_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_RBB_BQ_IQBIAS_SHORT_POS)
#define RF_ANA1_RBB_BQ_IQBIAS_SHORT_UMSK                        (~(((1U<<RF_ANA1_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_RBB_BQ_IQBIAS_SHORT_POS))
#define RF_ANA1_RBB_TIA_IQBIAS_SHORT                            RF_ANA1_RBB_TIA_IQBIAS_SHORT
#define RF_ANA1_RBB_TIA_IQBIAS_SHORT_POS                        (3U)
#define RF_ANA1_RBB_TIA_IQBIAS_SHORT_LEN                        (1U)
#define RF_ANA1_RBB_TIA_IQBIAS_SHORT_MSK                        (((1U<<RF_ANA1_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_RBB_TIA_IQBIAS_SHORT_POS)
#define RF_ANA1_RBB_TIA_IQBIAS_SHORT_UMSK                       (~(((1U<<RF_ANA1_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_RBB_TIA_IQBIAS_SHORT_POS))
#define RF_ANA1_RBB_CGM_R_TUNE                                  RF_ANA1_RBB_CGM_R_TUNE
#define RF_ANA1_RBB_CGM_R_TUNE_POS                              (4U)
#define RF_ANA1_RBB_CGM_R_TUNE_LEN                              (3U)
#define RF_ANA1_RBB_CGM_R_TUNE_MSK                              (((1U<<RF_ANA1_RBB_CGM_R_TUNE_LEN)-1)<<RF_ANA1_RBB_CGM_R_TUNE_POS)
#define RF_ANA1_RBB_CGM_R_TUNE_UMSK                             (~(((1U<<RF_ANA1_RBB_CGM_R_TUNE_LEN)-1)<<RF_ANA1_RBB_CGM_R_TUNE_POS))
#define RF_ANA1_RBB_BIAS_SEL_CGM                                RF_ANA1_RBB_BIAS_SEL_CGM
#define RF_ANA1_RBB_BIAS_SEL_CGM_POS                            (8U)
#define RF_ANA1_RBB_BIAS_SEL_CGM_LEN                            (1U)
#define RF_ANA1_RBB_BIAS_SEL_CGM_MSK                            (((1U<<RF_ANA1_RBB_BIAS_SEL_CGM_LEN)-1)<<RF_ANA1_RBB_BIAS_SEL_CGM_POS)
#define RF_ANA1_RBB_BIAS_SEL_CGM_UMSK                           (~(((1U<<RF_ANA1_RBB_BIAS_SEL_CGM_LEN)-1)<<RF_ANA1_RBB_BIAS_SEL_CGM_POS))
#define RF_ANA1_RBB_STARTUP_SET                                 RF_ANA1_RBB_STARTUP_SET
#define RF_ANA1_RBB_STARTUP_SET_POS                             (9U)
#define RF_ANA1_RBB_STARTUP_SET_LEN                             (1U)
#define RF_ANA1_RBB_STARTUP_SET_MSK                             (((1U<<RF_ANA1_RBB_STARTUP_SET_LEN)-1)<<RF_ANA1_RBB_STARTUP_SET_POS)
#define RF_ANA1_RBB_STARTUP_SET_UMSK                            (~(((1U<<RF_ANA1_RBB_STARTUP_SET_LEN)-1)<<RF_ANA1_RBB_STARTUP_SET_POS))
#define RF_ANA1_RBB_VCM                                         RF_ANA1_RBB_VCM
#define RF_ANA1_RBB_VCM_POS                                     (12U)
#define RF_ANA1_RBB_VCM_LEN                                     (2U)
#define RF_ANA1_RBB_VCM_MSK                                     (((1U<<RF_ANA1_RBB_VCM_LEN)-1)<<RF_ANA1_RBB_VCM_POS)
#define RF_ANA1_RBB_VCM_UMSK                                    (~(((1U<<RF_ANA1_RBB_VCM_LEN)-1)<<RF_ANA1_RBB_VCM_POS))
#define RF_ANA1_RBB_VSTARTUP                                    RF_ANA1_RBB_VSTARTUP
#define RF_ANA1_RBB_VSTARTUP_POS                                (14U)
#define RF_ANA1_RBB_VSTARTUP_LEN                                (2U)
#define RF_ANA1_RBB_VSTARTUP_MSK                                (((1U<<RF_ANA1_RBB_VSTARTUP_LEN)-1)<<RF_ANA1_RBB_VSTARTUP_POS)
#define RF_ANA1_RBB_VSTARTUP_UMSK                               (~(((1U<<RF_ANA1_RBB_VSTARTUP_LEN)-1)<<RF_ANA1_RBB_VSTARTUP_POS))
#define RF_ANA1_RBB_RCCAL_DAC_VCM                               RF_ANA1_RBB_RCCAL_DAC_VCM
#define RF_ANA1_RBB_RCCAL_DAC_VCM_POS                           (16U)
#define RF_ANA1_RBB_RCCAL_DAC_VCM_LEN                           (2U)
#define RF_ANA1_RBB_RCCAL_DAC_VCM_MSK                           (((1U<<RF_ANA1_RBB_RCCAL_DAC_VCM_LEN)-1)<<RF_ANA1_RBB_RCCAL_DAC_VCM_POS)
#define RF_ANA1_RBB_RCCAL_DAC_VCM_UMSK                          (~(((1U<<RF_ANA1_RBB_RCCAL_DAC_VCM_LEN)-1)<<RF_ANA1_RBB_RCCAL_DAC_VCM_POS))
#define RF_ANA1_RBB_BT_FIF_TUNE                                 RF_ANA1_RBB_BT_FIF_TUNE
#define RF_ANA1_RBB_BT_FIF_TUNE_POS                             (20U)
#define RF_ANA1_RBB_BT_FIF_TUNE_LEN                             (2U)
#define RF_ANA1_RBB_BT_FIF_TUNE_MSK                             (((1U<<RF_ANA1_RBB_BT_FIF_TUNE_LEN)-1)<<RF_ANA1_RBB_BT_FIF_TUNE_POS)
#define RF_ANA1_RBB_BT_FIF_TUNE_UMSK                            (~(((1U<<RF_ANA1_RBB_BT_FIF_TUNE_LEN)-1)<<RF_ANA1_RBB_BT_FIF_TUNE_POS))
#define RF_ANA1_RBB_CAP3_WIDEBAND_FC                            RF_ANA1_RBB_CAP3_WIDEBAND_FC
#define RF_ANA1_RBB_CAP3_WIDEBAND_FC_POS                        (24U)
#define RF_ANA1_RBB_CAP3_WIDEBAND_FC_LEN                        (4U)
#define RF_ANA1_RBB_CAP3_WIDEBAND_FC_MSK                        (((1U<<RF_ANA1_RBB_CAP3_WIDEBAND_FC_LEN)-1)<<RF_ANA1_RBB_CAP3_WIDEBAND_FC_POS)
#define RF_ANA1_RBB_CAP3_WIDEBAND_FC_UMSK                       (~(((1U<<RF_ANA1_RBB_CAP3_WIDEBAND_FC_LEN)-1)<<RF_ANA1_RBB_CAP3_WIDEBAND_FC_POS))
#define RF_ANA1_RBB_BT_MODE                                     RF_ANA1_RBB_BT_MODE
#define RF_ANA1_RBB_BT_MODE_POS                                 (28U)
#define RF_ANA1_RBB_BT_MODE_LEN                                 (1U)
#define RF_ANA1_RBB_BT_MODE_MSK                                 (((1U<<RF_ANA1_RBB_BT_MODE_LEN)-1)<<RF_ANA1_RBB_BT_MODE_POS)
#define RF_ANA1_RBB_BT_MODE_UMSK                                (~(((1U<<RF_ANA1_RBB_BT_MODE_LEN)-1)<<RF_ANA1_RBB_BT_MODE_POS))
#define RF_ANA1_RBB_BT_MODE_HW                                  RF_ANA1_RBB_BT_MODE_HW
#define RF_ANA1_RBB_BT_MODE_HW_POS                              (29U)
#define RF_ANA1_RBB_BT_MODE_HW_LEN                              (1U)
#define RF_ANA1_RBB_BT_MODE_HW_MSK                              (((1U<<RF_ANA1_RBB_BT_MODE_HW_LEN)-1)<<RF_ANA1_RBB_BT_MODE_HW_POS)
#define RF_ANA1_RBB_BT_MODE_HW_UMSK                             (~(((1U<<RF_ANA1_RBB_BT_MODE_HW_LEN)-1)<<RF_ANA1_RBB_BT_MODE_HW_POS))

/* 0x9C : rbb4 */
#define RF_ANA1_RBB4_OFFSET                                     (0x9C)
#define RF_ANA1_RBB_OP1_CC                                      RF_ANA1_RBB_OP1_CC
#define RF_ANA1_RBB_OP1_CC_POS                                  (0U)
#define RF_ANA1_RBB_OP1_CC_LEN                                  (4U)
#define RF_ANA1_RBB_OP1_CC_MSK                                  (((1U<<RF_ANA1_RBB_OP1_CC_LEN)-1)<<RF_ANA1_RBB_OP1_CC_POS)
#define RF_ANA1_RBB_OP1_CC_UMSK                                 (~(((1U<<RF_ANA1_RBB_OP1_CC_LEN)-1)<<RF_ANA1_RBB_OP1_CC_POS))
#define RF_ANA1_RBB_OP2_CC                                      RF_ANA1_RBB_OP2_CC
#define RF_ANA1_RBB_OP2_CC_POS                                  (4U)
#define RF_ANA1_RBB_OP2_CC_LEN                                  (4U)
#define RF_ANA1_RBB_OP2_CC_MSK                                  (((1U<<RF_ANA1_RBB_OP2_CC_LEN)-1)<<RF_ANA1_RBB_OP2_CC_POS)
#define RF_ANA1_RBB_OP2_CC_UMSK                                 (~(((1U<<RF_ANA1_RBB_OP2_CC_LEN)-1)<<RF_ANA1_RBB_OP2_CC_POS))
#define RF_ANA1_RBB_OP3_CC                                      RF_ANA1_RBB_OP3_CC
#define RF_ANA1_RBB_OP3_CC_POS                                  (8U)
#define RF_ANA1_RBB_OP3_CC_LEN                                  (4U)
#define RF_ANA1_RBB_OP3_CC_MSK                                  (((1U<<RF_ANA1_RBB_OP3_CC_LEN)-1)<<RF_ANA1_RBB_OP3_CC_POS)
#define RF_ANA1_RBB_OP3_CC_UMSK                                 (~(((1U<<RF_ANA1_RBB_OP3_CC_LEN)-1)<<RF_ANA1_RBB_OP3_CC_POS))
#define RF_ANA1_RBB_DEQ                                         RF_ANA1_RBB_DEQ
#define RF_ANA1_RBB_DEQ_POS                                     (12U)
#define RF_ANA1_RBB_DEQ_LEN                                     (2U)
#define RF_ANA1_RBB_DEQ_MSK                                     (((1U<<RF_ANA1_RBB_DEQ_LEN)-1)<<RF_ANA1_RBB_DEQ_POS)
#define RF_ANA1_RBB_DEQ_UMSK                                    (~(((1U<<RF_ANA1_RBB_DEQ_LEN)-1)<<RF_ANA1_RBB_DEQ_POS))
#define RF_ANA1_RBB_OP2_TWO_POLE_EN                             RF_ANA1_RBB_OP2_TWO_POLE_EN
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_POS                         (15U)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_LEN                         (1U)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_MSK                         (((1U<<RF_ANA1_RBB_OP2_TWO_POLE_EN_LEN)-1)<<RF_ANA1_RBB_OP2_TWO_POLE_EN_POS)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_UMSK                        (~(((1U<<RF_ANA1_RBB_OP2_TWO_POLE_EN_LEN)-1)<<RF_ANA1_RBB_OP2_TWO_POLE_EN_POS))
#define RF_ANA1_RBB_OP2_RZ                                      RF_ANA1_RBB_OP2_RZ
#define RF_ANA1_RBB_OP2_RZ_POS                                  (24U)
#define RF_ANA1_RBB_OP2_RZ_LEN                                  (4U)
#define RF_ANA1_RBB_OP2_RZ_MSK                                  (((1U<<RF_ANA1_RBB_OP2_RZ_LEN)-1)<<RF_ANA1_RBB_OP2_RZ_POS)
#define RF_ANA1_RBB_OP2_RZ_UMSK                                 (~(((1U<<RF_ANA1_RBB_OP2_RZ_LEN)-1)<<RF_ANA1_RBB_OP2_RZ_POS))
#define RF_ANA1_RBB_RMX_ROUT_TUNE                               RF_ANA1_RBB_RMX_ROUT_TUNE
#define RF_ANA1_RBB_RMX_ROUT_TUNE_POS                           (28U)
#define RF_ANA1_RBB_RMX_ROUT_TUNE_LEN                           (4U)
#define RF_ANA1_RBB_RMX_ROUT_TUNE_MSK                           (((1U<<RF_ANA1_RBB_RMX_ROUT_TUNE_LEN)-1)<<RF_ANA1_RBB_RMX_ROUT_TUNE_POS)
#define RF_ANA1_RBB_RMX_ROUT_TUNE_UMSK                          (~(((1U<<RF_ANA1_RBB_RMX_ROUT_TUNE_LEN)-1)<<RF_ANA1_RBB_RMX_ROUT_TUNE_POS))

/* 0xA0 : rbb5 */
#define RF_ANA1_RBB5_OFFSET                                     (0xA0)
#define RF_ANA1_RBB_OP1_CC_BT                                   RF_ANA1_RBB_OP1_CC_BT
#define RF_ANA1_RBB_OP1_CC_BT_POS                               (0U)
#define RF_ANA1_RBB_OP1_CC_BT_LEN                               (4U)
#define RF_ANA1_RBB_OP1_CC_BT_MSK                               (((1U<<RF_ANA1_RBB_OP1_CC_BT_LEN)-1)<<RF_ANA1_RBB_OP1_CC_BT_POS)
#define RF_ANA1_RBB_OP1_CC_BT_UMSK                              (~(((1U<<RF_ANA1_RBB_OP1_CC_BT_LEN)-1)<<RF_ANA1_RBB_OP1_CC_BT_POS))
#define RF_ANA1_RBB_OP2_CC_BT                                   RF_ANA1_RBB_OP2_CC_BT
#define RF_ANA1_RBB_OP2_CC_BT_POS                               (4U)
#define RF_ANA1_RBB_OP2_CC_BT_LEN                               (4U)
#define RF_ANA1_RBB_OP2_CC_BT_MSK                               (((1U<<RF_ANA1_RBB_OP2_CC_BT_LEN)-1)<<RF_ANA1_RBB_OP2_CC_BT_POS)
#define RF_ANA1_RBB_OP2_CC_BT_UMSK                              (~(((1U<<RF_ANA1_RBB_OP2_CC_BT_LEN)-1)<<RF_ANA1_RBB_OP2_CC_BT_POS))
#define RF_ANA1_RBB_OP3_CC_BT                                   RF_ANA1_RBB_OP3_CC_BT
#define RF_ANA1_RBB_OP3_CC_BT_POS                               (8U)
#define RF_ANA1_RBB_OP3_CC_BT_LEN                               (4U)
#define RF_ANA1_RBB_OP3_CC_BT_MSK                               (((1U<<RF_ANA1_RBB_OP3_CC_BT_LEN)-1)<<RF_ANA1_RBB_OP3_CC_BT_POS)
#define RF_ANA1_RBB_OP3_CC_BT_UMSK                              (~(((1U<<RF_ANA1_RBB_OP3_CC_BT_LEN)-1)<<RF_ANA1_RBB_OP3_CC_BT_POS))
#define RF_ANA1_RBB_DEQ_BT                                      RF_ANA1_RBB_DEQ_BT
#define RF_ANA1_RBB_DEQ_BT_POS                                  (12U)
#define RF_ANA1_RBB_DEQ_BT_LEN                                  (2U)
#define RF_ANA1_RBB_DEQ_BT_MSK                                  (((1U<<RF_ANA1_RBB_DEQ_BT_LEN)-1)<<RF_ANA1_RBB_DEQ_BT_POS)
#define RF_ANA1_RBB_DEQ_BT_UMSK                                 (~(((1U<<RF_ANA1_RBB_DEQ_BT_LEN)-1)<<RF_ANA1_RBB_DEQ_BT_POS))
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_BT                          RF_ANA1_RBB_OP2_TWO_POLE_EN_BT
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_POS                      (15U)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_LEN                      (1U)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_MSK                      (((1U<<RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_LEN)-1)<<RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_POS)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_UMSK                     (~(((1U<<RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_LEN)-1)<<RF_ANA1_RBB_OP2_TWO_POLE_EN_BT_POS))
#define RF_ANA1_RBB_OP1_CC_HW                                   RF_ANA1_RBB_OP1_CC_HW
#define RF_ANA1_RBB_OP1_CC_HW_POS                               (16U)
#define RF_ANA1_RBB_OP1_CC_HW_LEN                               (4U)
#define RF_ANA1_RBB_OP1_CC_HW_MSK                               (((1U<<RF_ANA1_RBB_OP1_CC_HW_LEN)-1)<<RF_ANA1_RBB_OP1_CC_HW_POS)
#define RF_ANA1_RBB_OP1_CC_HW_UMSK                              (~(((1U<<RF_ANA1_RBB_OP1_CC_HW_LEN)-1)<<RF_ANA1_RBB_OP1_CC_HW_POS))
#define RF_ANA1_RBB_OP2_CC_HW                                   RF_ANA1_RBB_OP2_CC_HW
#define RF_ANA1_RBB_OP2_CC_HW_POS                               (20U)
#define RF_ANA1_RBB_OP2_CC_HW_LEN                               (4U)
#define RF_ANA1_RBB_OP2_CC_HW_MSK                               (((1U<<RF_ANA1_RBB_OP2_CC_HW_LEN)-1)<<RF_ANA1_RBB_OP2_CC_HW_POS)
#define RF_ANA1_RBB_OP2_CC_HW_UMSK                              (~(((1U<<RF_ANA1_RBB_OP2_CC_HW_LEN)-1)<<RF_ANA1_RBB_OP2_CC_HW_POS))
#define RF_ANA1_RBB_OP3_CC_HW                                   RF_ANA1_RBB_OP3_CC_HW
#define RF_ANA1_RBB_OP3_CC_HW_POS                               (24U)
#define RF_ANA1_RBB_OP3_CC_HW_LEN                               (4U)
#define RF_ANA1_RBB_OP3_CC_HW_MSK                               (((1U<<RF_ANA1_RBB_OP3_CC_HW_LEN)-1)<<RF_ANA1_RBB_OP3_CC_HW_POS)
#define RF_ANA1_RBB_OP3_CC_HW_UMSK                              (~(((1U<<RF_ANA1_RBB_OP3_CC_HW_LEN)-1)<<RF_ANA1_RBB_OP3_CC_HW_POS))
#define RF_ANA1_RBB_DEQ_HW                                      RF_ANA1_RBB_DEQ_HW
#define RF_ANA1_RBB_DEQ_HW_POS                                  (28U)
#define RF_ANA1_RBB_DEQ_HW_LEN                                  (2U)
#define RF_ANA1_RBB_DEQ_HW_MSK                                  (((1U<<RF_ANA1_RBB_DEQ_HW_LEN)-1)<<RF_ANA1_RBB_DEQ_HW_POS)
#define RF_ANA1_RBB_DEQ_HW_UMSK                                 (~(((1U<<RF_ANA1_RBB_DEQ_HW_LEN)-1)<<RF_ANA1_RBB_DEQ_HW_POS))
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_HW                          RF_ANA1_RBB_OP2_TWO_POLE_EN_HW
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_POS                      (31U)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_LEN                      (1U)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_MSK                      (((1U<<RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_LEN)-1)<<RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_POS)
#define RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_UMSK                     (~(((1U<<RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_LEN)-1)<<RF_ANA1_RBB_OP2_TWO_POLE_EN_HW_POS))

/* 0xA4 : rbb6 */
#define RF_ANA1_RBB6_OFFSET                                     (0xA4)
#define RF_ANA1_RBB_BM_OP                                       RF_ANA1_RBB_BM_OP
#define RF_ANA1_RBB_BM_OP_POS                                   (0U)
#define RF_ANA1_RBB_BM_OP_LEN                                   (3U)
#define RF_ANA1_RBB_BM_OP_MSK                                   (((1U<<RF_ANA1_RBB_BM_OP_LEN)-1)<<RF_ANA1_RBB_BM_OP_POS)
#define RF_ANA1_RBB_BM_OP_UMSK                                  (~(((1U<<RF_ANA1_RBB_BM_OP_LEN)-1)<<RF_ANA1_RBB_BM_OP_POS))
#define RF_ANA1_RBB_BM_OP1_OUT                                  RF_ANA1_RBB_BM_OP1_OUT
#define RF_ANA1_RBB_BM_OP1_OUT_POS                              (3U)
#define RF_ANA1_RBB_BM_OP1_OUT_LEN                              (3U)
#define RF_ANA1_RBB_BM_OP1_OUT_MSK                              (((1U<<RF_ANA1_RBB_BM_OP1_OUT_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_POS)
#define RF_ANA1_RBB_BM_OP1_OUT_UMSK                             (~(((1U<<RF_ANA1_RBB_BM_OP1_OUT_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_POS))
#define RF_ANA1_RBB_BM_OP2_OUT                                  RF_ANA1_RBB_BM_OP2_OUT
#define RF_ANA1_RBB_BM_OP2_OUT_POS                              (6U)
#define RF_ANA1_RBB_BM_OP2_OUT_LEN                              (3U)
#define RF_ANA1_RBB_BM_OP2_OUT_MSK                              (((1U<<RF_ANA1_RBB_BM_OP2_OUT_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_POS)
#define RF_ANA1_RBB_BM_OP2_OUT_UMSK                             (~(((1U<<RF_ANA1_RBB_BM_OP2_OUT_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_POS))
#define RF_ANA1_RBB_BM_OP3_OUT                                  RF_ANA1_RBB_BM_OP3_OUT
#define RF_ANA1_RBB_BM_OP3_OUT_POS                              (9U)
#define RF_ANA1_RBB_BM_OP3_OUT_LEN                              (3U)
#define RF_ANA1_RBB_BM_OP3_OUT_MSK                              (((1U<<RF_ANA1_RBB_BM_OP3_OUT_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_POS)
#define RF_ANA1_RBB_BM_OP3_OUT_UMSK                             (~(((1U<<RF_ANA1_RBB_BM_OP3_OUT_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_POS))
#define RF_ANA1_RBB_BW                                          RF_ANA1_RBB_BW
#define RF_ANA1_RBB_BW_POS                                      (12U)
#define RF_ANA1_RBB_BW_LEN                                      (2U)
#define RF_ANA1_RBB_BW_MSK                                      (((1U<<RF_ANA1_RBB_BW_LEN)-1)<<RF_ANA1_RBB_BW_POS)
#define RF_ANA1_RBB_BW_UMSK                                     (~(((1U<<RF_ANA1_RBB_BW_LEN)-1)<<RF_ANA1_RBB_BW_POS))
#define RF_ANA1_RBB_BM_OP_HW                                    RF_ANA1_RBB_BM_OP_HW
#define RF_ANA1_RBB_BM_OP_HW_POS                                (16U)
#define RF_ANA1_RBB_BM_OP_HW_LEN                                (3U)
#define RF_ANA1_RBB_BM_OP_HW_MSK                                (((1U<<RF_ANA1_RBB_BM_OP_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP_HW_POS)
#define RF_ANA1_RBB_BM_OP_HW_UMSK                               (~(((1U<<RF_ANA1_RBB_BM_OP_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP_HW_POS))
#define RF_ANA1_RBB_BM_OP1_OUT_HW                               RF_ANA1_RBB_BM_OP1_OUT_HW
#define RF_ANA1_RBB_BM_OP1_OUT_HW_POS                           (19U)
#define RF_ANA1_RBB_BM_OP1_OUT_HW_LEN                           (3U)
#define RF_ANA1_RBB_BM_OP1_OUT_HW_MSK                           (((1U<<RF_ANA1_RBB_BM_OP1_OUT_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_HW_POS)
#define RF_ANA1_RBB_BM_OP1_OUT_HW_UMSK                          (~(((1U<<RF_ANA1_RBB_BM_OP1_OUT_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_HW_POS))
#define RF_ANA1_RBB_BM_OP2_OUT_HW                               RF_ANA1_RBB_BM_OP2_OUT_HW
#define RF_ANA1_RBB_BM_OP2_OUT_HW_POS                           (22U)
#define RF_ANA1_RBB_BM_OP2_OUT_HW_LEN                           (3U)
#define RF_ANA1_RBB_BM_OP2_OUT_HW_MSK                           (((1U<<RF_ANA1_RBB_BM_OP2_OUT_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_HW_POS)
#define RF_ANA1_RBB_BM_OP2_OUT_HW_UMSK                          (~(((1U<<RF_ANA1_RBB_BM_OP2_OUT_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_HW_POS))
#define RF_ANA1_RBB_BM_OP3_OUT_HW                               RF_ANA1_RBB_BM_OP3_OUT_HW
#define RF_ANA1_RBB_BM_OP3_OUT_HW_POS                           (25U)
#define RF_ANA1_RBB_BM_OP3_OUT_HW_LEN                           (3U)
#define RF_ANA1_RBB_BM_OP3_OUT_HW_MSK                           (((1U<<RF_ANA1_RBB_BM_OP3_OUT_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_HW_POS)
#define RF_ANA1_RBB_BM_OP3_OUT_HW_UMSK                          (~(((1U<<RF_ANA1_RBB_BM_OP3_OUT_HW_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_HW_POS))
#define RF_ANA1_RBB_BW_HW                                       RF_ANA1_RBB_BW_HW
#define RF_ANA1_RBB_BW_HW_POS                                   (28U)
#define RF_ANA1_RBB_BW_HW_LEN                                   (2U)
#define RF_ANA1_RBB_BW_HW_MSK                                   (((1U<<RF_ANA1_RBB_BW_HW_LEN)-1)<<RF_ANA1_RBB_BW_HW_POS)
#define RF_ANA1_RBB_BW_HW_UMSK                                  (~(((1U<<RF_ANA1_RBB_BW_HW_LEN)-1)<<RF_ANA1_RBB_BW_HW_POS))

/* 0xA8 : rbb7 */
#define RF_ANA1_RBB7_OFFSET                                     (0xA8)
#define RF_ANA1_RBB_BM_OP_BT                                    RF_ANA1_RBB_BM_OP_BT
#define RF_ANA1_RBB_BM_OP_BT_POS                                (0U)
#define RF_ANA1_RBB_BM_OP_BT_LEN                                (3U)
#define RF_ANA1_RBB_BM_OP_BT_MSK                                (((1U<<RF_ANA1_RBB_BM_OP_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP_BT_POS)
#define RF_ANA1_RBB_BM_OP_BT_UMSK                               (~(((1U<<RF_ANA1_RBB_BM_OP_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP_BT_POS))
#define RF_ANA1_RBB_BM_OP1_OUT_BT                               RF_ANA1_RBB_BM_OP1_OUT_BT
#define RF_ANA1_RBB_BM_OP1_OUT_BT_POS                           (3U)
#define RF_ANA1_RBB_BM_OP1_OUT_BT_LEN                           (3U)
#define RF_ANA1_RBB_BM_OP1_OUT_BT_MSK                           (((1U<<RF_ANA1_RBB_BM_OP1_OUT_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_BT_POS)
#define RF_ANA1_RBB_BM_OP1_OUT_BT_UMSK                          (~(((1U<<RF_ANA1_RBB_BM_OP1_OUT_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_BT_POS))
#define RF_ANA1_RBB_BM_OP2_OUT_BT                               RF_ANA1_RBB_BM_OP2_OUT_BT
#define RF_ANA1_RBB_BM_OP2_OUT_BT_POS                           (6U)
#define RF_ANA1_RBB_BM_OP2_OUT_BT_LEN                           (3U)
#define RF_ANA1_RBB_BM_OP2_OUT_BT_MSK                           (((1U<<RF_ANA1_RBB_BM_OP2_OUT_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_BT_POS)
#define RF_ANA1_RBB_BM_OP2_OUT_BT_UMSK                          (~(((1U<<RF_ANA1_RBB_BM_OP2_OUT_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_BT_POS))
#define RF_ANA1_RBB_BM_OP3_OUT_BT                               RF_ANA1_RBB_BM_OP3_OUT_BT
#define RF_ANA1_RBB_BM_OP3_OUT_BT_POS                           (9U)
#define RF_ANA1_RBB_BM_OP3_OUT_BT_LEN                           (3U)
#define RF_ANA1_RBB_BM_OP3_OUT_BT_MSK                           (((1U<<RF_ANA1_RBB_BM_OP3_OUT_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_BT_POS)
#define RF_ANA1_RBB_BM_OP3_OUT_BT_UMSK                          (~(((1U<<RF_ANA1_RBB_BM_OP3_OUT_BT_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_BT_POS))
#define RF_ANA1_RBB_BW_BT                                       RF_ANA1_RBB_BW_BT
#define RF_ANA1_RBB_BW_BT_POS                                   (12U)
#define RF_ANA1_RBB_BW_BT_LEN                                   (2U)
#define RF_ANA1_RBB_BW_BT_MSK                                   (((1U<<RF_ANA1_RBB_BW_BT_LEN)-1)<<RF_ANA1_RBB_BW_BT_POS)
#define RF_ANA1_RBB_BW_BT_UMSK                                  (~(((1U<<RF_ANA1_RBB_BW_BT_LEN)-1)<<RF_ANA1_RBB_BW_BT_POS))
#define RF_ANA1_RBB_BM_OP_LPMODE                                RF_ANA1_RBB_BM_OP_LPMODE
#define RF_ANA1_RBB_BM_OP_LPMODE_POS                            (16U)
#define RF_ANA1_RBB_BM_OP_LPMODE_LEN                            (3U)
#define RF_ANA1_RBB_BM_OP_LPMODE_MSK                            (((1U<<RF_ANA1_RBB_BM_OP_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP_LPMODE_POS)
#define RF_ANA1_RBB_BM_OP_LPMODE_UMSK                           (~(((1U<<RF_ANA1_RBB_BM_OP_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP_LPMODE_POS))
#define RF_ANA1_RBB_BM_OP1_OUT_LPMODE                           RF_ANA1_RBB_BM_OP1_OUT_LPMODE
#define RF_ANA1_RBB_BM_OP1_OUT_LPMODE_POS                       (19U)
#define RF_ANA1_RBB_BM_OP1_OUT_LPMODE_LEN                       (3U)
#define RF_ANA1_RBB_BM_OP1_OUT_LPMODE_MSK                       (((1U<<RF_ANA1_RBB_BM_OP1_OUT_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_LPMODE_POS)
#define RF_ANA1_RBB_BM_OP1_OUT_LPMODE_UMSK                      (~(((1U<<RF_ANA1_RBB_BM_OP1_OUT_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP1_OUT_LPMODE_POS))
#define RF_ANA1_RBB_BM_OP2_OUT_LPMODE                           RF_ANA1_RBB_BM_OP2_OUT_LPMODE
#define RF_ANA1_RBB_BM_OP2_OUT_LPMODE_POS                       (22U)
#define RF_ANA1_RBB_BM_OP2_OUT_LPMODE_LEN                       (3U)
#define RF_ANA1_RBB_BM_OP2_OUT_LPMODE_MSK                       (((1U<<RF_ANA1_RBB_BM_OP2_OUT_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_LPMODE_POS)
#define RF_ANA1_RBB_BM_OP2_OUT_LPMODE_UMSK                      (~(((1U<<RF_ANA1_RBB_BM_OP2_OUT_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP2_OUT_LPMODE_POS))
#define RF_ANA1_RBB_BM_OP3_OUT_LPMODE                           RF_ANA1_RBB_BM_OP3_OUT_LPMODE
#define RF_ANA1_RBB_BM_OP3_OUT_LPMODE_POS                       (25U)
#define RF_ANA1_RBB_BM_OP3_OUT_LPMODE_LEN                       (3U)
#define RF_ANA1_RBB_BM_OP3_OUT_LPMODE_MSK                       (((1U<<RF_ANA1_RBB_BM_OP3_OUT_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_LPMODE_POS)
#define RF_ANA1_RBB_BM_OP3_OUT_LPMODE_UMSK                      (~(((1U<<RF_ANA1_RBB_BM_OP3_OUT_LPMODE_LEN)-1)<<RF_ANA1_RBB_BM_OP3_OUT_LPMODE_POS))
#define RF_ANA1_RBB_BW_LPMODE                                   RF_ANA1_RBB_BW_LPMODE
#define RF_ANA1_RBB_BW_LPMODE_POS                               (28U)
#define RF_ANA1_RBB_BW_LPMODE_LEN                               (2U)
#define RF_ANA1_RBB_BW_LPMODE_MSK                               (((1U<<RF_ANA1_RBB_BW_LPMODE_LEN)-1)<<RF_ANA1_RBB_BW_LPMODE_POS)
#define RF_ANA1_RBB_BW_LPMODE_UMSK                              (~(((1U<<RF_ANA1_RBB_BW_LPMODE_LEN)-1)<<RF_ANA1_RBB_BW_LPMODE_POS))

/* 0xAC : rbb_atest */
#define RF_ANA1_RBB_ATEST_OFFSET                                (0xAC)
#define RF_ANA1_RBB_ATEST_GAIN_R7_Q                             RF_ANA1_RBB_ATEST_GAIN_R7_Q
#define RF_ANA1_RBB_ATEST_GAIN_R7_Q_POS                         (0U)
#define RF_ANA1_RBB_ATEST_GAIN_R7_Q_LEN                         (3U)
#define RF_ANA1_RBB_ATEST_GAIN_R7_Q_MSK                         (((1U<<RF_ANA1_RBB_ATEST_GAIN_R7_Q_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R7_Q_POS)
#define RF_ANA1_RBB_ATEST_GAIN_R7_Q_UMSK                        (~(((1U<<RF_ANA1_RBB_ATEST_GAIN_R7_Q_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R7_Q_POS))
#define RF_ANA1_RBB_ATEST_GAIN_R7_I                             RF_ANA1_RBB_ATEST_GAIN_R7_I
#define RF_ANA1_RBB_ATEST_GAIN_R7_I_POS                         (4U)
#define RF_ANA1_RBB_ATEST_GAIN_R7_I_LEN                         (3U)
#define RF_ANA1_RBB_ATEST_GAIN_R7_I_MSK                         (((1U<<RF_ANA1_RBB_ATEST_GAIN_R7_I_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R7_I_POS)
#define RF_ANA1_RBB_ATEST_GAIN_R7_I_UMSK                        (~(((1U<<RF_ANA1_RBB_ATEST_GAIN_R7_I_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R7_I_POS))
#define RF_ANA1_RBB_ATEST_GAIN_R8_Q                             RF_ANA1_RBB_ATEST_GAIN_R8_Q
#define RF_ANA1_RBB_ATEST_GAIN_R8_Q_POS                         (8U)
#define RF_ANA1_RBB_ATEST_GAIN_R8_Q_LEN                         (3U)
#define RF_ANA1_RBB_ATEST_GAIN_R8_Q_MSK                         (((1U<<RF_ANA1_RBB_ATEST_GAIN_R8_Q_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R8_Q_POS)
#define RF_ANA1_RBB_ATEST_GAIN_R8_Q_UMSK                        (~(((1U<<RF_ANA1_RBB_ATEST_GAIN_R8_Q_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R8_Q_POS))
#define RF_ANA1_RBB_ATEST_GAIN_R8_I                             RF_ANA1_RBB_ATEST_GAIN_R8_I
#define RF_ANA1_RBB_ATEST_GAIN_R8_I_POS                         (12U)
#define RF_ANA1_RBB_ATEST_GAIN_R8_I_LEN                         (3U)
#define RF_ANA1_RBB_ATEST_GAIN_R8_I_MSK                         (((1U<<RF_ANA1_RBB_ATEST_GAIN_R8_I_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R8_I_POS)
#define RF_ANA1_RBB_ATEST_GAIN_R8_I_UMSK                        (~(((1U<<RF_ANA1_RBB_ATEST_GAIN_R8_I_LEN)-1)<<RF_ANA1_RBB_ATEST_GAIN_R8_I_POS))
#define RF_ANA1_RBB_ATEST_OUT_EN                                RF_ANA1_RBB_ATEST_OUT_EN
#define RF_ANA1_RBB_ATEST_OUT_EN_POS                            (16U)
#define RF_ANA1_RBB_ATEST_OUT_EN_LEN                            (1U)
#define RF_ANA1_RBB_ATEST_OUT_EN_MSK                            (((1U<<RF_ANA1_RBB_ATEST_OUT_EN_LEN)-1)<<RF_ANA1_RBB_ATEST_OUT_EN_POS)
#define RF_ANA1_RBB_ATEST_OUT_EN_UMSK                           (~(((1U<<RF_ANA1_RBB_ATEST_OUT_EN_LEN)-1)<<RF_ANA1_RBB_ATEST_OUT_EN_POS))
#define RF_ANA1_RBB_ATEST_IN_EN                                 RF_ANA1_RBB_ATEST_IN_EN
#define RF_ANA1_RBB_ATEST_IN_EN_POS                             (17U)
#define RF_ANA1_RBB_ATEST_IN_EN_LEN                             (1U)
#define RF_ANA1_RBB_ATEST_IN_EN_MSK                             (((1U<<RF_ANA1_RBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_RBB_ATEST_IN_EN_POS)
#define RF_ANA1_RBB_ATEST_IN_EN_UMSK                            (~(((1U<<RF_ANA1_RBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_RBB_ATEST_IN_EN_POS))
#define RF_ANA1_RBB_GAIN_R6                                     RF_ANA1_RBB_GAIN_R6
#define RF_ANA1_RBB_GAIN_R6_POS                                 (20U)
#define RF_ANA1_RBB_GAIN_R6_LEN                                 (3U)
#define RF_ANA1_RBB_GAIN_R6_MSK                                 (((1U<<RF_ANA1_RBB_GAIN_R6_LEN)-1)<<RF_ANA1_RBB_GAIN_R6_POS)
#define RF_ANA1_RBB_GAIN_R6_UMSK                                (~(((1U<<RF_ANA1_RBB_GAIN_R6_LEN)-1)<<RF_ANA1_RBB_GAIN_R6_POS))
#define RF_ANA1_RBB_GAIN_R5                                     RF_ANA1_RBB_GAIN_R5
#define RF_ANA1_RBB_GAIN_R5_POS                                 (24U)
#define RF_ANA1_RBB_GAIN_R5_LEN                                 (7U)
#define RF_ANA1_RBB_GAIN_R5_MSK                                 (((1U<<RF_ANA1_RBB_GAIN_R5_LEN)-1)<<RF_ANA1_RBB_GAIN_R5_POS)
#define RF_ANA1_RBB_GAIN_R5_UMSK                                (~(((1U<<RF_ANA1_RBB_GAIN_R5_LEN)-1)<<RF_ANA1_RBB_GAIN_R5_POS))

/* 0xB0 : rbb_pkdet */
#define RF_ANA1_RBB_PKDET_OFFSET                                (0xB0)
#define RF_ANA1_RBB_PKDET_VTH                                   RF_ANA1_RBB_PKDET_VTH
#define RF_ANA1_RBB_PKDET_VTH_POS                               (0U)
#define RF_ANA1_RBB_PKDET_VTH_LEN                               (4U)
#define RF_ANA1_RBB_PKDET_VTH_MSK                               (((1U<<RF_ANA1_RBB_PKDET_VTH_LEN)-1)<<RF_ANA1_RBB_PKDET_VTH_POS)
#define RF_ANA1_RBB_PKDET_VTH_UMSK                              (~(((1U<<RF_ANA1_RBB_PKDET_VTH_LEN)-1)<<RF_ANA1_RBB_PKDET_VTH_POS))
#define RF_ANA1_RBB_PKDET_EN                                    RF_ANA1_RBB_PKDET_EN
#define RF_ANA1_RBB_PKDET_EN_POS                                (8U)
#define RF_ANA1_RBB_PKDET_EN_LEN                                (1U)
#define RF_ANA1_RBB_PKDET_EN_MSK                                (((1U<<RF_ANA1_RBB_PKDET_EN_LEN)-1)<<RF_ANA1_RBB_PKDET_EN_POS)
#define RF_ANA1_RBB_PKDET_EN_UMSK                               (~(((1U<<RF_ANA1_RBB_PKDET_EN_LEN)-1)<<RF_ANA1_RBB_PKDET_EN_POS))
#define RF_ANA1_RBB_PKDET_EN_HW                                 RF_ANA1_RBB_PKDET_EN_HW
#define RF_ANA1_RBB_PKDET_EN_HW_POS                             (9U)
#define RF_ANA1_RBB_PKDET_EN_HW_LEN                             (1U)
#define RF_ANA1_RBB_PKDET_EN_HW_MSK                             (((1U<<RF_ANA1_RBB_PKDET_EN_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_EN_HW_POS)
#define RF_ANA1_RBB_PKDET_EN_HW_UMSK                            (~(((1U<<RF_ANA1_RBB_PKDET_EN_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_EN_HW_POS))
#define RF_ANA1_RBB_PKDET_OUT_RSTN                              RF_ANA1_RBB_PKDET_OUT_RSTN
#define RF_ANA1_RBB_PKDET_OUT_RSTN_POS                          (10U)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_LEN                          (1U)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_MSK                          (((1U<<RF_ANA1_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_ANA1_RBB_PKDET_OUT_RSTN_POS)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_UMSK                         (~(((1U<<RF_ANA1_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_ANA1_RBB_PKDET_OUT_RSTN_POS))
#define RF_ANA1_RBB_PKDET_OUT_RSTN_HW                           RF_ANA1_RBB_PKDET_OUT_RSTN_HW
#define RF_ANA1_RBB_PKDET_OUT_RSTN_HW_POS                       (11U)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_HW_LEN                       (1U)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_HW_MSK                       (((1U<<RF_ANA1_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_OUT_RSTN_HW_POS)
#define RF_ANA1_RBB_PKDET_OUT_RSTN_HW_UMSK                      (~(((1U<<RF_ANA1_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_ANA1_RBB_PKDET_OUT_RSTN_HW_POS))
#define RF_ANA1_PKDET_OUT_RAW                                   RF_ANA1_PKDET_OUT_RAW
#define RF_ANA1_PKDET_OUT_RAW_POS                               (16U)
#define RF_ANA1_PKDET_OUT_RAW_LEN                               (1U)
#define RF_ANA1_PKDET_OUT_RAW_MSK                               (((1U<<RF_ANA1_PKDET_OUT_RAW_LEN)-1)<<RF_ANA1_PKDET_OUT_RAW_POS)
#define RF_ANA1_PKDET_OUT_RAW_UMSK                              (~(((1U<<RF_ANA1_PKDET_OUT_RAW_LEN)-1)<<RF_ANA1_PKDET_OUT_RAW_POS))
#define RF_ANA1_PKDET_OUT_LATCH                                 RF_ANA1_PKDET_OUT_LATCH
#define RF_ANA1_PKDET_OUT_LATCH_POS                             (17U)
#define RF_ANA1_PKDET_OUT_LATCH_LEN                             (1U)
#define RF_ANA1_PKDET_OUT_LATCH_MSK                             (((1U<<RF_ANA1_PKDET_OUT_LATCH_LEN)-1)<<RF_ANA1_PKDET_OUT_LATCH_POS)
#define RF_ANA1_PKDET_OUT_LATCH_UMSK                            (~(((1U<<RF_ANA1_PKDET_OUT_LATCH_LEN)-1)<<RF_ANA1_PKDET_OUT_LATCH_POS))
#define RF_ANA1_RX_LP_MODE_EN_HW                                RF_ANA1_RX_LP_MODE_EN_HW
#define RF_ANA1_RX_LP_MODE_EN_HW_POS                            (20U)
#define RF_ANA1_RX_LP_MODE_EN_HW_LEN                            (1U)
#define RF_ANA1_RX_LP_MODE_EN_HW_MSK                            (((1U<<RF_ANA1_RX_LP_MODE_EN_HW_LEN)-1)<<RF_ANA1_RX_LP_MODE_EN_HW_POS)
#define RF_ANA1_RX_LP_MODE_EN_HW_UMSK                           (~(((1U<<RF_ANA1_RX_LP_MODE_EN_HW_LEN)-1)<<RF_ANA1_RX_LP_MODE_EN_HW_POS))
#define RF_ANA1_RX_LP_MODE_EN                                   RF_ANA1_RX_LP_MODE_EN
#define RF_ANA1_RX_LP_MODE_EN_POS                               (22U)
#define RF_ANA1_RX_LP_MODE_EN_LEN                               (1U)
#define RF_ANA1_RX_LP_MODE_EN_MSK                               (((1U<<RF_ANA1_RX_LP_MODE_EN_LEN)-1)<<RF_ANA1_RX_LP_MODE_EN_POS)
#define RF_ANA1_RX_LP_MODE_EN_UMSK                              (~(((1U<<RF_ANA1_RX_LP_MODE_EN_LEN)-1)<<RF_ANA1_RX_LP_MODE_EN_POS))
#define RF_ANA1_RX_LP_MODE_CTRL_HW                              RF_ANA1_RX_LP_MODE_CTRL_HW
#define RF_ANA1_RX_LP_MODE_CTRL_HW_POS                          (23U)
#define RF_ANA1_RX_LP_MODE_CTRL_HW_LEN                          (1U)
#define RF_ANA1_RX_LP_MODE_CTRL_HW_MSK                          (((1U<<RF_ANA1_RX_LP_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_RX_LP_MODE_CTRL_HW_POS)
#define RF_ANA1_RX_LP_MODE_CTRL_HW_UMSK                         (~(((1U<<RF_ANA1_RX_LP_MODE_CTRL_HW_LEN)-1)<<RF_ANA1_RX_LP_MODE_CTRL_HW_POS))

/* 0xB4 : Digital Control */
#define RF_ANA1_RXADC_0_OFFSET                                  (0xB4)
#define RF_ANA1_RXADC_VREF_SEL                                  RF_ANA1_RXADC_VREF_SEL
#define RF_ANA1_RXADC_VREF_SEL_POS                              (0U)
#define RF_ANA1_RXADC_VREF_SEL_LEN                              (2U)
#define RF_ANA1_RXADC_VREF_SEL_MSK                              (((1U<<RF_ANA1_RXADC_VREF_SEL_LEN)-1)<<RF_ANA1_RXADC_VREF_SEL_POS)
#define RF_ANA1_RXADC_VREF_SEL_UMSK                             (~(((1U<<RF_ANA1_RXADC_VREF_SEL_LEN)-1)<<RF_ANA1_RXADC_VREF_SEL_POS))
#define RF_ANA1_RXADC_DLY_CTL                                   RF_ANA1_RXADC_DLY_CTL
#define RF_ANA1_RXADC_DLY_CTL_POS                               (4U)
#define RF_ANA1_RXADC_DLY_CTL_LEN                               (2U)
#define RF_ANA1_RXADC_DLY_CTL_MSK                               (((1U<<RF_ANA1_RXADC_DLY_CTL_LEN)-1)<<RF_ANA1_RXADC_DLY_CTL_POS)
#define RF_ANA1_RXADC_DLY_CTL_UMSK                              (~(((1U<<RF_ANA1_RXADC_DLY_CTL_LEN)-1)<<RF_ANA1_RXADC_DLY_CTL_POS))
#define RF_ANA1_RXADC_DVDD_SEL                                  RF_ANA1_RXADC_DVDD_SEL
#define RF_ANA1_RXADC_DVDD_SEL_POS                              (8U)
#define RF_ANA1_RXADC_DVDD_SEL_LEN                              (2U)
#define RF_ANA1_RXADC_DVDD_SEL_MSK                              (((1U<<RF_ANA1_RXADC_DVDD_SEL_LEN)-1)<<RF_ANA1_RXADC_DVDD_SEL_POS)
#define RF_ANA1_RXADC_DVDD_SEL_UMSK                             (~(((1U<<RF_ANA1_RXADC_DVDD_SEL_LEN)-1)<<RF_ANA1_RXADC_DVDD_SEL_POS))
#define RF_ANA1_RXADC_GT_RM                                     RF_ANA1_RXADC_GT_RM
#define RF_ANA1_RXADC_GT_RM_POS                                 (12U)
#define RF_ANA1_RXADC_GT_RM_LEN                                 (1U)
#define RF_ANA1_RXADC_GT_RM_MSK                                 (((1U<<RF_ANA1_RXADC_GT_RM_LEN)-1)<<RF_ANA1_RXADC_GT_RM_POS)
#define RF_ANA1_RXADC_GT_RM_UMSK                                (~(((1U<<RF_ANA1_RXADC_GT_RM_LEN)-1)<<RF_ANA1_RXADC_GT_RM_POS))
#define RF_ANA1_RXADC_CLK_SYNC_INV                              RF_ANA1_RXADC_CLK_SYNC_INV
#define RF_ANA1_RXADC_CLK_SYNC_INV_POS                          (13U)
#define RF_ANA1_RXADC_CLK_SYNC_INV_LEN                          (1U)
#define RF_ANA1_RXADC_CLK_SYNC_INV_MSK                          (((1U<<RF_ANA1_RXADC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_RXADC_CLK_SYNC_INV_POS)
#define RF_ANA1_RXADC_CLK_SYNC_INV_UMSK                         (~(((1U<<RF_ANA1_RXADC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_RXADC_CLK_SYNC_INV_POS))
#define RF_ANA1_RXADC_CLK_SYNC_SEL                              RF_ANA1_RXADC_CLK_SYNC_SEL
#define RF_ANA1_RXADC_CLK_SYNC_SEL_POS                          (14U)
#define RF_ANA1_RXADC_CLK_SYNC_SEL_LEN                          (1U)
#define RF_ANA1_RXADC_CLK_SYNC_SEL_MSK                          (((1U<<RF_ANA1_RXADC_CLK_SYNC_SEL_LEN)-1)<<RF_ANA1_RXADC_CLK_SYNC_SEL_POS)
#define RF_ANA1_RXADC_CLK_SYNC_SEL_UMSK                         (~(((1U<<RF_ANA1_RXADC_CLK_SYNC_SEL_LEN)-1)<<RF_ANA1_RXADC_CLK_SYNC_SEL_POS))
#define RF_ANA1_RXADC_CLK_INV                                   RF_ANA1_RXADC_CLK_INV
#define RF_ANA1_RXADC_CLK_INV_POS                               (16U)
#define RF_ANA1_RXADC_CLK_INV_LEN                               (1U)
#define RF_ANA1_RXADC_CLK_INV_MSK                               (((1U<<RF_ANA1_RXADC_CLK_INV_LEN)-1)<<RF_ANA1_RXADC_CLK_INV_POS)
#define RF_ANA1_RXADC_CLK_INV_UMSK                              (~(((1U<<RF_ANA1_RXADC_CLK_INV_LEN)-1)<<RF_ANA1_RXADC_CLK_INV_POS))
#define RF_ANA1_RXADC_CLK_DIV_SEL                               RF_ANA1_RXADC_CLK_DIV_SEL
#define RF_ANA1_RXADC_CLK_DIV_SEL_POS                           (20U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_LEN                           (1U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_MSK                           (((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_POS)
#define RF_ANA1_RXADC_CLK_DIV_SEL_UMSK                          (~(((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_POS))
#define RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE                        RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE
#define RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_POS                    (21U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_LEN                    (1U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_MSK                    (((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_POS)
#define RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_UMSK                   (~(((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_LPMODE_POS))
#define RF_ANA1_RXADC_CLK_DIV_SEL_BT                            RF_ANA1_RXADC_CLK_DIV_SEL_BT
#define RF_ANA1_RXADC_CLK_DIV_SEL_BT_POS                        (22U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_BT_LEN                        (1U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_BT_MSK                        (((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_BT_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_BT_POS)
#define RF_ANA1_RXADC_CLK_DIV_SEL_BT_UMSK                       (~(((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_BT_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_BT_POS))
#define RF_ANA1_RXADC_CLK_DIV_SEL_HW                            RF_ANA1_RXADC_CLK_DIV_SEL_HW
#define RF_ANA1_RXADC_CLK_DIV_SEL_HW_POS                        (23U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_HW_LEN                        (1U)
#define RF_ANA1_RXADC_CLK_DIV_SEL_HW_MSK                        (((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_HW_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_HW_POS)
#define RF_ANA1_RXADC_CLK_DIV_SEL_HW_UMSK                       (~(((1U<<RF_ANA1_RXADC_CLK_DIV_SEL_HW_LEN)-1)<<RF_ANA1_RXADC_CLK_DIV_SEL_HW_POS))
#define RF_ANA1_RXADC_TI_EN                                     RF_ANA1_RXADC_TI_EN
#define RF_ANA1_RXADC_TI_EN_POS                                 (24U)
#define RF_ANA1_RXADC_TI_EN_LEN                                 (1U)
#define RF_ANA1_RXADC_TI_EN_MSK                                 (((1U<<RF_ANA1_RXADC_TI_EN_LEN)-1)<<RF_ANA1_RXADC_TI_EN_POS)
#define RF_ANA1_RXADC_TI_EN_UMSK                                (~(((1U<<RF_ANA1_RXADC_TI_EN_LEN)-1)<<RF_ANA1_RXADC_TI_EN_POS))
#define RF_ANA1_RXADC_TI_EN_LPMODE                              RF_ANA1_RXADC_TI_EN_LPMODE
#define RF_ANA1_RXADC_TI_EN_LPMODE_POS                          (25U)
#define RF_ANA1_RXADC_TI_EN_LPMODE_LEN                          (1U)
#define RF_ANA1_RXADC_TI_EN_LPMODE_MSK                          (((1U<<RF_ANA1_RXADC_TI_EN_LPMODE_LEN)-1)<<RF_ANA1_RXADC_TI_EN_LPMODE_POS)
#define RF_ANA1_RXADC_TI_EN_LPMODE_UMSK                         (~(((1U<<RF_ANA1_RXADC_TI_EN_LPMODE_LEN)-1)<<RF_ANA1_RXADC_TI_EN_LPMODE_POS))
#define RF_ANA1_RXADC_TI_EN_BT                                  RF_ANA1_RXADC_TI_EN_BT
#define RF_ANA1_RXADC_TI_EN_BT_POS                              (26U)
#define RF_ANA1_RXADC_TI_EN_BT_LEN                              (1U)
#define RF_ANA1_RXADC_TI_EN_BT_MSK                              (((1U<<RF_ANA1_RXADC_TI_EN_BT_LEN)-1)<<RF_ANA1_RXADC_TI_EN_BT_POS)
#define RF_ANA1_RXADC_TI_EN_BT_UMSK                             (~(((1U<<RF_ANA1_RXADC_TI_EN_BT_LEN)-1)<<RF_ANA1_RXADC_TI_EN_BT_POS))
#define RF_ANA1_RXADC_TI_EN_HW                                  RF_ANA1_RXADC_TI_EN_HW
#define RF_ANA1_RXADC_TI_EN_HW_POS                              (27U)
#define RF_ANA1_RXADC_TI_EN_HW_LEN                              (1U)
#define RF_ANA1_RXADC_TI_EN_HW_MSK                              (((1U<<RF_ANA1_RXADC_TI_EN_HW_LEN)-1)<<RF_ANA1_RXADC_TI_EN_HW_POS)
#define RF_ANA1_RXADC_TI_EN_HW_UMSK                             (~(((1U<<RF_ANA1_RXADC_TI_EN_HW_LEN)-1)<<RF_ANA1_RXADC_TI_EN_HW_POS))
#define RF_ANA1_RXADC_VREF_CP_EN                                RF_ANA1_RXADC_VREF_CP_EN
#define RF_ANA1_RXADC_VREF_CP_EN_POS                            (28U)
#define RF_ANA1_RXADC_VREF_CP_EN_LEN                            (1U)
#define RF_ANA1_RXADC_VREF_CP_EN_MSK                            (((1U<<RF_ANA1_RXADC_VREF_CP_EN_LEN)-1)<<RF_ANA1_RXADC_VREF_CP_EN_POS)
#define RF_ANA1_RXADC_VREF_CP_EN_UMSK                           (~(((1U<<RF_ANA1_RXADC_VREF_CP_EN_LEN)-1)<<RF_ANA1_RXADC_VREF_CP_EN_POS))
#define RF_ANA1_RXADC_VBUF_SEL                                  RF_ANA1_RXADC_VBUF_SEL
#define RF_ANA1_RXADC_VBUF_SEL_POS                              (29U)
#define RF_ANA1_RXADC_VBUF_SEL_LEN                              (2U)
#define RF_ANA1_RXADC_VBUF_SEL_MSK                              (((1U<<RF_ANA1_RXADC_VBUF_SEL_LEN)-1)<<RF_ANA1_RXADC_VBUF_SEL_POS)
#define RF_ANA1_RXADC_VBUF_SEL_UMSK                             (~(((1U<<RF_ANA1_RXADC_VBUF_SEL_LEN)-1)<<RF_ANA1_RXADC_VBUF_SEL_POS))

/* 0xB8 : Digital Control */
#define RF_ANA1_RXADC_1_OFFSET                                  (0xB8)
#define RF_ANA1_RXADC_DOUT_I                                    RF_ANA1_RXADC_DOUT_I
#define RF_ANA1_RXADC_DOUT_I_POS                                (0U)
#define RF_ANA1_RXADC_DOUT_I_LEN                                (11U)
#define RF_ANA1_RXADC_DOUT_I_MSK                                (((1U<<RF_ANA1_RXADC_DOUT_I_LEN)-1)<<RF_ANA1_RXADC_DOUT_I_POS)
#define RF_ANA1_RXADC_DOUT_I_UMSK                               (~(((1U<<RF_ANA1_RXADC_DOUT_I_LEN)-1)<<RF_ANA1_RXADC_DOUT_I_POS))
#define RF_ANA1_RXADC_DOUT_Q                                    RF_ANA1_RXADC_DOUT_Q
#define RF_ANA1_RXADC_DOUT_Q_POS                                (16U)
#define RF_ANA1_RXADC_DOUT_Q_LEN                                (11U)
#define RF_ANA1_RXADC_DOUT_Q_MSK                                (((1U<<RF_ANA1_RXADC_DOUT_Q_LEN)-1)<<RF_ANA1_RXADC_DOUT_Q_POS)
#define RF_ANA1_RXADC_DOUT_Q_UMSK                               (~(((1U<<RF_ANA1_RXADC_DOUT_Q_LEN)-1)<<RF_ANA1_RXADC_DOUT_Q_POS))

/* 0xBC : Digital Control */
#define RF_ANA1_WRF_DAC_0_OFFSET                                (0xBC)
#define RF_ANA1_DAC_DVDD_SEL                                    RF_ANA1_DAC_DVDD_SEL
#define RF_ANA1_DAC_DVDD_SEL_POS                                (0U)
#define RF_ANA1_DAC_DVDD_SEL_LEN                                (3U)
#define RF_ANA1_DAC_DVDD_SEL_MSK                                (((1U<<RF_ANA1_DAC_DVDD_SEL_LEN)-1)<<RF_ANA1_DAC_DVDD_SEL_POS)
#define RF_ANA1_DAC_DVDD_SEL_UMSK                               (~(((1U<<RF_ANA1_DAC_DVDD_SEL_LEN)-1)<<RF_ANA1_DAC_DVDD_SEL_POS))
#define RF_ANA1_DAC_BIAS_ENHANCE                                RF_ANA1_DAC_BIAS_ENHANCE
#define RF_ANA1_DAC_BIAS_ENHANCE_POS                            (3U)
#define RF_ANA1_DAC_BIAS_ENHANCE_LEN                            (1U)
#define RF_ANA1_DAC_BIAS_ENHANCE_MSK                            (((1U<<RF_ANA1_DAC_BIAS_ENHANCE_LEN)-1)<<RF_ANA1_DAC_BIAS_ENHANCE_POS)
#define RF_ANA1_DAC_BIAS_ENHANCE_UMSK                           (~(((1U<<RF_ANA1_DAC_BIAS_ENHANCE_LEN)-1)<<RF_ANA1_DAC_BIAS_ENHANCE_POS))
#define RF_ANA1_DAC_DVDD_DEC_SEL                                RF_ANA1_DAC_DVDD_DEC_SEL
#define RF_ANA1_DAC_DVDD_DEC_SEL_POS                            (4U)
#define RF_ANA1_DAC_DVDD_DEC_SEL_LEN                            (3U)
#define RF_ANA1_DAC_DVDD_DEC_SEL_MSK                            (((1U<<RF_ANA1_DAC_DVDD_DEC_SEL_LEN)-1)<<RF_ANA1_DAC_DVDD_DEC_SEL_POS)
#define RF_ANA1_DAC_DVDD_DEC_SEL_UMSK                           (~(((1U<<RF_ANA1_DAC_DVDD_DEC_SEL_LEN)-1)<<RF_ANA1_DAC_DVDD_DEC_SEL_POS))
#define RF_ANA1_DAC_BIAS_SEL                                    RF_ANA1_DAC_BIAS_SEL
#define RF_ANA1_DAC_BIAS_SEL_POS                                (8U)
#define RF_ANA1_DAC_BIAS_SEL_LEN                                (2U)
#define RF_ANA1_DAC_BIAS_SEL_MSK                                (((1U<<RF_ANA1_DAC_BIAS_SEL_LEN)-1)<<RF_ANA1_DAC_BIAS_SEL_POS)
#define RF_ANA1_DAC_BIAS_SEL_UMSK                               (~(((1U<<RF_ANA1_DAC_BIAS_SEL_LEN)-1)<<RF_ANA1_DAC_BIAS_SEL_POS))
#define RF_ANA1_DAC_BIAS_SEL_HW                                 RF_ANA1_DAC_BIAS_SEL_HW
#define RF_ANA1_DAC_BIAS_SEL_HW_POS                             (10U)
#define RF_ANA1_DAC_BIAS_SEL_HW_LEN                             (2U)
#define RF_ANA1_DAC_BIAS_SEL_HW_MSK                             (((1U<<RF_ANA1_DAC_BIAS_SEL_HW_LEN)-1)<<RF_ANA1_DAC_BIAS_SEL_HW_POS)
#define RF_ANA1_DAC_BIAS_SEL_HW_UMSK                            (~(((1U<<RF_ANA1_DAC_BIAS_SEL_HW_LEN)-1)<<RF_ANA1_DAC_BIAS_SEL_HW_POS))
#define RF_ANA1_DAC_RCCAL_SEL                                   RF_ANA1_DAC_RCCAL_SEL
#define RF_ANA1_DAC_RCCAL_SEL_POS                               (12U)
#define RF_ANA1_DAC_RCCAL_SEL_LEN                               (1U)
#define RF_ANA1_DAC_RCCAL_SEL_MSK                               (((1U<<RF_ANA1_DAC_RCCAL_SEL_LEN)-1)<<RF_ANA1_DAC_RCCAL_SEL_POS)
#define RF_ANA1_DAC_RCCAL_SEL_UMSK                              (~(((1U<<RF_ANA1_DAC_RCCAL_SEL_LEN)-1)<<RF_ANA1_DAC_RCCAL_SEL_POS))
#define RF_ANA1_DAC_CLK_SYNC_960M_INV                           RF_ANA1_DAC_CLK_SYNC_960M_INV
#define RF_ANA1_DAC_CLK_SYNC_960M_INV_POS                       (16U)
#define RF_ANA1_DAC_CLK_SYNC_960M_INV_LEN                       (1U)
#define RF_ANA1_DAC_CLK_SYNC_960M_INV_MSK                       (((1U<<RF_ANA1_DAC_CLK_SYNC_960M_INV_LEN)-1)<<RF_ANA1_DAC_CLK_SYNC_960M_INV_POS)
#define RF_ANA1_DAC_CLK_SYNC_960M_INV_UMSK                      (~(((1U<<RF_ANA1_DAC_CLK_SYNC_960M_INV_LEN)-1)<<RF_ANA1_DAC_CLK_SYNC_960M_INV_POS))
#define RF_ANA1_DAC_CLK_320M_INV                                RF_ANA1_DAC_CLK_320M_INV
#define RF_ANA1_DAC_CLK_320M_INV_POS                            (17U)
#define RF_ANA1_DAC_CLK_320M_INV_LEN                            (1U)
#define RF_ANA1_DAC_CLK_320M_INV_MSK                            (((1U<<RF_ANA1_DAC_CLK_320M_INV_LEN)-1)<<RF_ANA1_DAC_CLK_320M_INV_POS)
#define RF_ANA1_DAC_CLK_320M_INV_UMSK                           (~(((1U<<RF_ANA1_DAC_CLK_320M_INV_LEN)-1)<<RF_ANA1_DAC_CLK_320M_INV_POS))
#define RF_ANA1_DAC_RCCAL_ATEST_EN                              RF_ANA1_DAC_RCCAL_ATEST_EN
#define RF_ANA1_DAC_RCCAL_ATEST_EN_POS                          (20U)
#define RF_ANA1_DAC_RCCAL_ATEST_EN_LEN                          (1U)
#define RF_ANA1_DAC_RCCAL_ATEST_EN_MSK                          (((1U<<RF_ANA1_DAC_RCCAL_ATEST_EN_LEN)-1)<<RF_ANA1_DAC_RCCAL_ATEST_EN_POS)
#define RF_ANA1_DAC_RCCAL_ATEST_EN_UMSK                         (~(((1U<<RF_ANA1_DAC_RCCAL_ATEST_EN_LEN)-1)<<RF_ANA1_DAC_RCCAL_ATEST_EN_POS))
#define RF_ANA1_DAC_CLK_EN_DIV3                                 RF_ANA1_DAC_CLK_EN_DIV3
#define RF_ANA1_DAC_CLK_EN_DIV3_POS                             (21U)
#define RF_ANA1_DAC_CLK_EN_DIV3_LEN                             (1U)
#define RF_ANA1_DAC_CLK_EN_DIV3_MSK                             (((1U<<RF_ANA1_DAC_CLK_EN_DIV3_LEN)-1)<<RF_ANA1_DAC_CLK_EN_DIV3_POS)
#define RF_ANA1_DAC_CLK_EN_DIV3_UMSK                            (~(((1U<<RF_ANA1_DAC_CLK_EN_DIV3_LEN)-1)<<RF_ANA1_DAC_CLK_EN_DIV3_POS))
#define RF_ANA1_DAC_ATEST_CH_SEL                                RF_ANA1_DAC_ATEST_CH_SEL
#define RF_ANA1_DAC_ATEST_CH_SEL_POS                            (22U)
#define RF_ANA1_DAC_ATEST_CH_SEL_LEN                            (1U)
#define RF_ANA1_DAC_ATEST_CH_SEL_MSK                            (((1U<<RF_ANA1_DAC_ATEST_CH_SEL_LEN)-1)<<RF_ANA1_DAC_ATEST_CH_SEL_POS)
#define RF_ANA1_DAC_ATEST_CH_SEL_UMSK                           (~(((1U<<RF_ANA1_DAC_ATEST_CH_SEL_LEN)-1)<<RF_ANA1_DAC_ATEST_CH_SEL_POS))
#define RF_ANA1_DAC_VLOW_SEL                                    RF_ANA1_DAC_VLOW_SEL
#define RF_ANA1_DAC_VLOW_SEL_POS                                (24U)
#define RF_ANA1_DAC_VLOW_SEL_LEN                                (2U)
#define RF_ANA1_DAC_VLOW_SEL_MSK                                (((1U<<RF_ANA1_DAC_VLOW_SEL_LEN)-1)<<RF_ANA1_DAC_VLOW_SEL_POS)
#define RF_ANA1_DAC_VLOW_SEL_UMSK                               (~(((1U<<RF_ANA1_DAC_VLOW_SEL_LEN)-1)<<RF_ANA1_DAC_VLOW_SEL_POS))
#define RF_ANA1_DAC_NSINK_SEL                                   RF_ANA1_DAC_NSINK_SEL
#define RF_ANA1_DAC_NSINK_SEL_POS                               (28U)
#define RF_ANA1_DAC_NSINK_SEL_LEN                               (2U)
#define RF_ANA1_DAC_NSINK_SEL_MSK                               (((1U<<RF_ANA1_DAC_NSINK_SEL_LEN)-1)<<RF_ANA1_DAC_NSINK_SEL_POS)
#define RF_ANA1_DAC_NSINK_SEL_UMSK                              (~(((1U<<RF_ANA1_DAC_NSINK_SEL_LEN)-1)<<RF_ANA1_DAC_NSINK_SEL_POS))

/* 0xC0 : wrf_dac_1 */
#define RF_ANA1_WRF_DAC_1_OFFSET                                (0xC0)
#define RF_ANA1_DAC_DIN_I_PART0                                 RF_ANA1_DAC_DIN_I_PART0
#define RF_ANA1_DAC_DIN_I_PART0_POS                             (0U)
#define RF_ANA1_DAC_DIN_I_PART0_LEN                             (11U)
#define RF_ANA1_DAC_DIN_I_PART0_MSK                             (((1U<<RF_ANA1_DAC_DIN_I_PART0_LEN)-1)<<RF_ANA1_DAC_DIN_I_PART0_POS)
#define RF_ANA1_DAC_DIN_I_PART0_UMSK                            (~(((1U<<RF_ANA1_DAC_DIN_I_PART0_LEN)-1)<<RF_ANA1_DAC_DIN_I_PART0_POS))
#define RF_ANA1_DAC_DIN_I_PART1                                 RF_ANA1_DAC_DIN_I_PART1
#define RF_ANA1_DAC_DIN_I_PART1_POS                             (16U)
#define RF_ANA1_DAC_DIN_I_PART1_LEN                             (11U)
#define RF_ANA1_DAC_DIN_I_PART1_MSK                             (((1U<<RF_ANA1_DAC_DIN_I_PART1_LEN)-1)<<RF_ANA1_DAC_DIN_I_PART1_POS)
#define RF_ANA1_DAC_DIN_I_PART1_UMSK                            (~(((1U<<RF_ANA1_DAC_DIN_I_PART1_LEN)-1)<<RF_ANA1_DAC_DIN_I_PART1_POS))

/* 0xC4 : wrf_dac_2 */
#define RF_ANA1_WRF_DAC_2_OFFSET                                (0xC4)
#define RF_ANA1_DAC_DIN_I_PART2                                 RF_ANA1_DAC_DIN_I_PART2
#define RF_ANA1_DAC_DIN_I_PART2_POS                             (0U)
#define RF_ANA1_DAC_DIN_I_PART2_LEN                             (11U)
#define RF_ANA1_DAC_DIN_I_PART2_MSK                             (((1U<<RF_ANA1_DAC_DIN_I_PART2_LEN)-1)<<RF_ANA1_DAC_DIN_I_PART2_POS)
#define RF_ANA1_DAC_DIN_I_PART2_UMSK                            (~(((1U<<RF_ANA1_DAC_DIN_I_PART2_LEN)-1)<<RF_ANA1_DAC_DIN_I_PART2_POS))

/* 0xC8 : wrf_dac_3 */
#define RF_ANA1_WRF_DAC_3_OFFSET                                (0xC8)
#define RF_ANA1_DAC_DIN_Q_PART0                                 RF_ANA1_DAC_DIN_Q_PART0
#define RF_ANA1_DAC_DIN_Q_PART0_POS                             (0U)
#define RF_ANA1_DAC_DIN_Q_PART0_LEN                             (11U)
#define RF_ANA1_DAC_DIN_Q_PART0_MSK                             (((1U<<RF_ANA1_DAC_DIN_Q_PART0_LEN)-1)<<RF_ANA1_DAC_DIN_Q_PART0_POS)
#define RF_ANA1_DAC_DIN_Q_PART0_UMSK                            (~(((1U<<RF_ANA1_DAC_DIN_Q_PART0_LEN)-1)<<RF_ANA1_DAC_DIN_Q_PART0_POS))
#define RF_ANA1_DAC_DIN_Q_PART1                                 RF_ANA1_DAC_DIN_Q_PART1
#define RF_ANA1_DAC_DIN_Q_PART1_POS                             (16U)
#define RF_ANA1_DAC_DIN_Q_PART1_LEN                             (11U)
#define RF_ANA1_DAC_DIN_Q_PART1_MSK                             (((1U<<RF_ANA1_DAC_DIN_Q_PART1_LEN)-1)<<RF_ANA1_DAC_DIN_Q_PART1_POS)
#define RF_ANA1_DAC_DIN_Q_PART1_UMSK                            (~(((1U<<RF_ANA1_DAC_DIN_Q_PART1_LEN)-1)<<RF_ANA1_DAC_DIN_Q_PART1_POS))

/* 0xCC : wrf_dac_4 */
#define RF_ANA1_WRF_DAC_4_OFFSET                                (0xCC)
#define RF_ANA1_DAC_DIN_Q_PART2                                 RF_ANA1_DAC_DIN_Q_PART2
#define RF_ANA1_DAC_DIN_Q_PART2_POS                             (0U)
#define RF_ANA1_DAC_DIN_Q_PART2_LEN                             (11U)
#define RF_ANA1_DAC_DIN_Q_PART2_MSK                             (((1U<<RF_ANA1_DAC_DIN_Q_PART2_LEN)-1)<<RF_ANA1_DAC_DIN_Q_PART2_POS)
#define RF_ANA1_DAC_DIN_Q_PART2_UMSK                            (~(((1U<<RF_ANA1_DAC_DIN_Q_PART2_LEN)-1)<<RF_ANA1_DAC_DIN_Q_PART2_POS))

/* 0xD0 : wifipll_ana_ctrl */
#define RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET                         (0xD0)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK                    RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK
#define RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_POS                (0U)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_LEN                (2U)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_MSK                (((1U<<RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_LEN)-1)<<RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_POS)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_UMSK               (~(((1U<<RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_LEN)-1)<<RF_ANA1_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_POS))
#define RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK                     RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK
#define RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_POS                 (2U)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_LEN                 (1U)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_MSK                 (((1U<<RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_POS)
#define RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_UMSK                (~(((1U<<RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<RF_ANA1_WIFIPLL_FBDIV_SEL_ADPLL_CLK_POS))
#define RF_ANA1_WIFIPLL_DTC_R_SEL                               RF_ANA1_WIFIPLL_DTC_R_SEL
#define RF_ANA1_WIFIPLL_DTC_R_SEL_POS                           (4U)
#define RF_ANA1_WIFIPLL_DTC_R_SEL_LEN                           (2U)
#define RF_ANA1_WIFIPLL_DTC_R_SEL_MSK                           (((1U<<RF_ANA1_WIFIPLL_DTC_R_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_DTC_R_SEL_POS)
#define RF_ANA1_WIFIPLL_DTC_R_SEL_UMSK                          (~(((1U<<RF_ANA1_WIFIPLL_DTC_R_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_DTC_R_SEL_POS))
#define RF_ANA1_WIFIPLL_DTC_BYPASS_EN                           RF_ANA1_WIFIPLL_DTC_BYPASS_EN
#define RF_ANA1_WIFIPLL_DTC_BYPASS_EN_POS                       (6U)
#define RF_ANA1_WIFIPLL_DTC_BYPASS_EN_LEN                       (1U)
#define RF_ANA1_WIFIPLL_DTC_BYPASS_EN_MSK                       (((1U<<RF_ANA1_WIFIPLL_DTC_BYPASS_EN_LEN)-1)<<RF_ANA1_WIFIPLL_DTC_BYPASS_EN_POS)
#define RF_ANA1_WIFIPLL_DTC_BYPASS_EN_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_DTC_BYPASS_EN_LEN)-1)<<RF_ANA1_WIFIPLL_DTC_BYPASS_EN_POS))
#define RF_ANA1_WIFIPLL_VCO_SPEED                               RF_ANA1_WIFIPLL_VCO_SPEED
#define RF_ANA1_WIFIPLL_VCO_SPEED_POS                           (8U)
#define RF_ANA1_WIFIPLL_VCO_SPEED_LEN                           (3U)
#define RF_ANA1_WIFIPLL_VCO_SPEED_MSK                           (((1U<<RF_ANA1_WIFIPLL_VCO_SPEED_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_SPEED_POS)
#define RF_ANA1_WIFIPLL_VCO_SPEED_UMSK                          (~(((1U<<RF_ANA1_WIFIPLL_VCO_SPEED_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_SPEED_POS))
#define RF_ANA1_WIFIPLL_VCO_480M_EN                             RF_ANA1_WIFIPLL_VCO_480M_EN
#define RF_ANA1_WIFIPLL_VCO_480M_EN_POS                         (11U)
#define RF_ANA1_WIFIPLL_VCO_480M_EN_LEN                         (1U)
#define RF_ANA1_WIFIPLL_VCO_480M_EN_MSK                         (((1U<<RF_ANA1_WIFIPLL_VCO_480M_EN_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_480M_EN_POS)
#define RF_ANA1_WIFIPLL_VCO_480M_EN_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_VCO_480M_EN_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_480M_EN_POS))
#define RF_ANA1_WIFIPLL_VCO_LDO_BYPASS                          RF_ANA1_WIFIPLL_VCO_LDO_BYPASS
#define RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_POS                      (12U)
#define RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_LEN                      (1U)
#define RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_MSK                      (((1U<<RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_POS)
#define RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_LDO_BYPASS_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_SEL                            RF_ANA1_WIFIPLL_VCO_RSHT_SEL
#define RF_ANA1_WIFIPLL_VCO_RSHT_SEL_POS                        (13U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_SEL_LEN                        (2U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_SEL_MSK                        (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_SEL_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_SEL_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_SEL_POS))
#define RF_ANA1_WIFIPLL_DCOMASH_BYPASS                          RF_ANA1_WIFIPLL_DCOMASH_BYPASS
#define RF_ANA1_WIFIPLL_DCOMASH_BYPASS_POS                      (15U)
#define RF_ANA1_WIFIPLL_DCOMASH_BYPASS_LEN                      (1U)
#define RF_ANA1_WIFIPLL_DCOMASH_BYPASS_MSK                      (((1U<<RF_ANA1_WIFIPLL_DCOMASH_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_DCOMASH_BYPASS_POS)
#define RF_ANA1_WIFIPLL_DCOMASH_BYPASS_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_DCOMASH_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_DCOMASH_BYPASS_POS))
#define RF_ANA1_WIFIPLL_REFCLK_SEL                              RF_ANA1_WIFIPLL_REFCLK_SEL
#define RF_ANA1_WIFIPLL_REFCLK_SEL_POS                          (16U)
#define RF_ANA1_WIFIPLL_REFCLK_SEL_LEN                          (2U)
#define RF_ANA1_WIFIPLL_REFCLK_SEL_MSK                          (((1U<<RF_ANA1_WIFIPLL_REFCLK_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_REFCLK_SEL_POS)
#define RF_ANA1_WIFIPLL_REFCLK_SEL_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_REFCLK_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_REFCLK_SEL_POS))
#define RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO                        RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO
#define RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_POS                    (20U)
#define RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_LEN                    (4U)
#define RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_MSK                    (((1U<<RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_LEN)-1)<<RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_POS)
#define RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_UMSK                   (~(((1U<<RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_LEN)-1)<<RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO_POS))
#define RF_ANA1_WIFIPLL_RSTB                                    RF_ANA1_WIFIPLL_RSTB
#define RF_ANA1_WIFIPLL_RSTB_POS                                (24U)
#define RF_ANA1_WIFIPLL_RSTB_LEN                                (1U)
#define RF_ANA1_WIFIPLL_RSTB_MSK                                (((1U<<RF_ANA1_WIFIPLL_RSTB_LEN)-1)<<RF_ANA1_WIFIPLL_RSTB_POS)
#define RF_ANA1_WIFIPLL_RSTB_UMSK                               (~(((1U<<RF_ANA1_WIFIPLL_RSTB_LEN)-1)<<RF_ANA1_WIFIPLL_RSTB_POS))
#define RF_ANA1_PU_WIFIPLL                                      RF_ANA1_PU_WIFIPLL
#define RF_ANA1_PU_WIFIPLL_POS                                  (28U)
#define RF_ANA1_PU_WIFIPLL_LEN                                  (1U)
#define RF_ANA1_PU_WIFIPLL_MSK                                  (((1U<<RF_ANA1_PU_WIFIPLL_LEN)-1)<<RF_ANA1_PU_WIFIPLL_POS)
#define RF_ANA1_PU_WIFIPLL_UMSK                                 (~(((1U<<RF_ANA1_PU_WIFIPLL_LEN)-1)<<RF_ANA1_PU_WIFIPLL_POS))

/* 0xD4 : wifipll_hw_ctrl */
#define RF_ANA1_WIFIPLL_HW_CTRL_OFFSET                          (0xD4)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX                            RF_ANA1_WIFIPLL_PI_BYPASS_TX
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_POS                        (0U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_LEN                        (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_MSK                        (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX                            RF_ANA1_WIFIPLL_PI_BYPASS_RX
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_POS                        (2U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_LEN                        (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_MSK                        (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ                         RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_POS                     (4U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_LEN                     (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_MSK                     (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_TX_BZ_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ                         RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_POS                     (6U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_LEN                     (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_MSK                     (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI                      RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI
#define RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_POS                  (8U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_LEN                  (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_MSK                  (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_UMSK                 (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_SEL_WIFI_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS_HW                            RF_ANA1_WIFIPLL_PI_BYPASS_HW
#define RF_ANA1_WIFIPLL_PI_BYPASS_HW_POS                        (9U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_HW_LEN                        (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_HW_MSK                        (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_HW_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_HW_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_HW_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_HW_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_HW_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX                          RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_POS                      (12U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_LEN                      (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_MSK                      (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX                          RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_POS                      (13U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_LEN                      (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_MSK                      (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ                       RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_POS                   (16U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_LEN                   (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_MSK                   (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_UMSK                  (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ                       RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_POS                   (17U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_LEN                   (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_MSK                   (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_UMSK                  (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_RX_BZ_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI                    RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_POS                (20U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_LEN                (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_MSK                (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_UMSK               (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN                             RF_ANA1_WIFIPLL_VCO_RSHT_EN
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_POS                         (21U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_LEN                         (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_MSK                         (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW                          RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_POS                      (22U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_LEN                      (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_MSK                      (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_HW_POS))
#define RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW                         RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW
#define RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_POS                     (24U)
#define RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_LEN                     (1U)
#define RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_MSK                     (((1U<<RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_POS)
#define RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_WIFI_EN_CTRL_HW_POS))
#define RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW                           RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW
#define RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_POS                       (25U)
#define RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_LEN                       (1U)
#define RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_MSK                       (((1U<<RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_POS)
#define RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_BZ_EN_CTRL_HW_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW                       RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW
#define RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_POS                   (26U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_LEN                   (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_MSK                   (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_UMSK                  (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_CTRL_HW_POS))
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW                     RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_POS                 (27U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_LEN                 (1U)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_MSK                 (((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_POS)
#define RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_UMSK                (~(((1U<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_RSHT_EN_CTRL_HW_POS))
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW                RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_POS            (28U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN            (1U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_MSK            (((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_POS)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_UMSK           (~(((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_POS))
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI               RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_POS           (29U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_LEN           (1U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_MSK           (((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_POS)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_UMSK          (~(((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_POS))

/* 0xD8 : wifipll_spd_fcal */
#define RF_ANA1_WIFIPLL_SPD_FCAL_OFFSET                         (0xD8)
#define RF_ANA1_WIFIPLL_FCAL_THRESHOLD                          RF_ANA1_WIFIPLL_FCAL_THRESHOLD
#define RF_ANA1_WIFIPLL_FCAL_THRESHOLD_POS                      (0U)
#define RF_ANA1_WIFIPLL_FCAL_THRESHOLD_LEN                      (1U)
#define RF_ANA1_WIFIPLL_FCAL_THRESHOLD_MSK                      (((1U<<RF_ANA1_WIFIPLL_FCAL_THRESHOLD_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_THRESHOLD_POS)
#define RF_ANA1_WIFIPLL_FCAL_THRESHOLD_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_FCAL_THRESHOLD_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_THRESHOLD_POS))
#define RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT                       RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT
#define RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_POS                   (1U)
#define RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_LEN                   (1U)
#define RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_MSK                   (((1U<<RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_POS)
#define RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_UMSK                  (~(((1U<<RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_PHAERR_LIMIT_POS))
#define RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN                        RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN
#define RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_POS                    (4U)
#define RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_LEN                    (1U)
#define RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_MSK                    (((1U<<RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_POS)
#define RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_UMSK                   (~(((1U<<RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_NDIV_ADJ_EN_POS))
#define RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT                     RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT
#define RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_POS                 (8U)
#define RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_LEN                 (7U)
#define RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_MSK                 (((1U<<RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_POS)
#define RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_UMSK                (~(((1U<<RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_MOMCAP_INI_EXT_POS))
#define RF_ANA1_WIFIPLL_FCAL_MODE                               RF_ANA1_WIFIPLL_FCAL_MODE
#define RF_ANA1_WIFIPLL_FCAL_MODE_POS                           (16U)
#define RF_ANA1_WIFIPLL_FCAL_MODE_LEN                           (2U)
#define RF_ANA1_WIFIPLL_FCAL_MODE_MSK                           (((1U<<RF_ANA1_WIFIPLL_FCAL_MODE_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_MODE_POS)
#define RF_ANA1_WIFIPLL_FCAL_MODE_UMSK                          (~(((1U<<RF_ANA1_WIFIPLL_FCAL_MODE_LEN)-1)<<RF_ANA1_WIFIPLL_FCAL_MODE_POS))
#define RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN                   RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN
#define RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_POS               (20U)
#define RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_LEN               (1U)
#define RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_MSK               (((1U<<RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_POS)
#define RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_UMSK              (~(((1U<<RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_DVAR_COMP_EN_POS))
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN                        RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_POS                    (21U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_LEN                    (1U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_MSK                    (((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_POS)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_UMSK                   (~(((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_POS))
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW                     RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_POS                 (22U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_LEN                 (1U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_MSK                 (((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_POS)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_UMSK                (~(((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_EN_HW_POS))
#define RF_ANA1_WIFIPLL_INC_FCAL_EN                             RF_ANA1_WIFIPLL_INC_FCAL_EN
#define RF_ANA1_WIFIPLL_INC_FCAL_EN_POS                         (23U)
#define RF_ANA1_WIFIPLL_INC_FCAL_EN_LEN                         (1U)
#define RF_ANA1_WIFIPLL_INC_FCAL_EN_MSK                         (((1U<<RF_ANA1_WIFIPLL_INC_FCAL_EN_LEN)-1)<<RF_ANA1_WIFIPLL_INC_FCAL_EN_POS)
#define RF_ANA1_WIFIPLL_INC_FCAL_EN_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_INC_FCAL_EN_LEN)-1)<<RF_ANA1_WIFIPLL_INC_FCAL_EN_POS))
#define RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY                          RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY
#define RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_POS                      (24U)
#define RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_LEN                      (1U)
#define RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_MSK                      (((1U<<RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_LEN)-1)<<RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_POS)
#define RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_LEN)-1)<<RF_ANA1_WIFIPLL_SPD_UNLOCK_DLY_POS))
#define RF_ANA1_WIFIPLL_SPD_GAIN                                RF_ANA1_WIFIPLL_SPD_GAIN
#define RF_ANA1_WIFIPLL_SPD_GAIN_POS                            (26U)
#define RF_ANA1_WIFIPLL_SPD_GAIN_LEN                            (2U)
#define RF_ANA1_WIFIPLL_SPD_GAIN_MSK                            (((1U<<RF_ANA1_WIFIPLL_SPD_GAIN_LEN)-1)<<RF_ANA1_WIFIPLL_SPD_GAIN_POS)
#define RF_ANA1_WIFIPLL_SPD_GAIN_UMSK                           (~(((1U<<RF_ANA1_WIFIPLL_SPD_GAIN_LEN)-1)<<RF_ANA1_WIFIPLL_SPD_GAIN_POS))
#define RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON                    RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON
#define RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_POS                (28U)
#define RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_LEN                (1U)
#define RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_MSK                (((1U<<RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_POS)
#define RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_UMSK               (~(((1U<<RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<RF_ANA1_WIFIPLL_COARSE_PATH_FORCE_ON_POS))
#define RF_ANA1_WIFIPLL_COARSE_GAIN                             RF_ANA1_WIFIPLL_COARSE_GAIN
#define RF_ANA1_WIFIPLL_COARSE_GAIN_POS                         (29U)
#define RF_ANA1_WIFIPLL_COARSE_GAIN_LEN                         (2U)
#define RF_ANA1_WIFIPLL_COARSE_GAIN_MSK                         (((1U<<RF_ANA1_WIFIPLL_COARSE_GAIN_LEN)-1)<<RF_ANA1_WIFIPLL_COARSE_GAIN_POS)
#define RF_ANA1_WIFIPLL_COARSE_GAIN_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_COARSE_GAIN_LEN)-1)<<RF_ANA1_WIFIPLL_COARSE_GAIN_POS))

/* 0xDC : wifipll_lf_vctrl */
#define RF_ANA1_WIFIPLL_LF_VCTRL_OFFSET                         (0xDC)
#define RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD                       RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD
#define RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_POS                   (0U)
#define RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_LEN                   (2U)
#define RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_MSK                   (((1U<<RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_LEN)-1)<<RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_POS)
#define RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_UMSK                  (~(((1U<<RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_LEN)-1)<<RF_ANA1_WIFIPLL_MOM_UPDATE_PERIOD_POS))
#define RF_ANA1_WIFIPLL_MOM_FORCE_HOLD                          RF_ANA1_WIFIPLL_MOM_FORCE_HOLD
#define RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_POS                      (3U)
#define RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_LEN                      (1U)
#define RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_MSK                      (((1U<<RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_LEN)-1)<<RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_POS)
#define RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_LEN)-1)<<RF_ANA1_WIFIPLL_MOM_FORCE_HOLD_POS))
#define RF_ANA1_WIFIPLL_MOM_CODE_BYPASS                         RF_ANA1_WIFIPLL_MOM_CODE_BYPASS
#define RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_POS                     (4U)
#define RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_LEN                     (1U)
#define RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_MSK                     (((1U<<RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_POS)
#define RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_MOM_CODE_BYPASS_POS))
#define RF_ANA1_WIFIPLL_LO_OPEN                                 RF_ANA1_WIFIPLL_LO_OPEN
#define RF_ANA1_WIFIPLL_LO_OPEN_POS                             (8U)
#define RF_ANA1_WIFIPLL_LO_OPEN_LEN                             (1U)
#define RF_ANA1_WIFIPLL_LO_OPEN_MSK                             (((1U<<RF_ANA1_WIFIPLL_LO_OPEN_LEN)-1)<<RF_ANA1_WIFIPLL_LO_OPEN_POS)
#define RF_ANA1_WIFIPLL_LO_OPEN_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_LO_OPEN_LEN)-1)<<RF_ANA1_WIFIPLL_LO_OPEN_POS))
#define RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE                      RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE
#define RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_POS                  (10U)
#define RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_LEN                  (1U)
#define RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_MSK                  (((1U<<RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_LEN)-1)<<RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_POS)
#define RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_UMSK                 (~(((1U<<RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_LEN)-1)<<RF_ANA1_WIFIPLL_FORCE_LF_FAST_MODE_POS))
#define RF_ANA1_WIFIPLL_LF_LSB_EXT                              RF_ANA1_WIFIPLL_LF_LSB_EXT
#define RF_ANA1_WIFIPLL_LF_LSB_EXT_POS                          (12U)
#define RF_ANA1_WIFIPLL_LF_LSB_EXT_LEN                          (6U)
#define RF_ANA1_WIFIPLL_LF_LSB_EXT_MSK                          (((1U<<RF_ANA1_WIFIPLL_LF_LSB_EXT_LEN)-1)<<RF_ANA1_WIFIPLL_LF_LSB_EXT_POS)
#define RF_ANA1_WIFIPLL_LF_LSB_EXT_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_LF_LSB_EXT_LEN)-1)<<RF_ANA1_WIFIPLL_LF_LSB_EXT_POS))
#define RF_ANA1_WIFIPLL_LF_BETA_FAST                            RF_ANA1_WIFIPLL_LF_BETA_FAST
#define RF_ANA1_WIFIPLL_LF_BETA_FAST_POS                        (20U)
#define RF_ANA1_WIFIPLL_LF_BETA_FAST_LEN                        (1U)
#define RF_ANA1_WIFIPLL_LF_BETA_FAST_MSK                        (((1U<<RF_ANA1_WIFIPLL_LF_BETA_FAST_LEN)-1)<<RF_ANA1_WIFIPLL_LF_BETA_FAST_POS)
#define RF_ANA1_WIFIPLL_LF_BETA_FAST_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_LF_BETA_FAST_LEN)-1)<<RF_ANA1_WIFIPLL_LF_BETA_FAST_POS))
#define RF_ANA1_WIFIPLL_LF_BETA_EXP                             RF_ANA1_WIFIPLL_LF_BETA_EXP
#define RF_ANA1_WIFIPLL_LF_BETA_EXP_POS                         (21U)
#define RF_ANA1_WIFIPLL_LF_BETA_EXP_LEN                         (3U)
#define RF_ANA1_WIFIPLL_LF_BETA_EXP_MSK                         (((1U<<RF_ANA1_WIFIPLL_LF_BETA_EXP_LEN)-1)<<RF_ANA1_WIFIPLL_LF_BETA_EXP_POS)
#define RF_ANA1_WIFIPLL_LF_BETA_EXP_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_LF_BETA_EXP_LEN)-1)<<RF_ANA1_WIFIPLL_LF_BETA_EXP_POS))
#define RF_ANA1_WIFIPLL_LF_BETA_BASE                            RF_ANA1_WIFIPLL_LF_BETA_BASE
#define RF_ANA1_WIFIPLL_LF_BETA_BASE_POS                        (24U)
#define RF_ANA1_WIFIPLL_LF_BETA_BASE_LEN                        (2U)
#define RF_ANA1_WIFIPLL_LF_BETA_BASE_MSK                        (((1U<<RF_ANA1_WIFIPLL_LF_BETA_BASE_LEN)-1)<<RF_ANA1_WIFIPLL_LF_BETA_BASE_POS)
#define RF_ANA1_WIFIPLL_LF_BETA_BASE_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_LF_BETA_BASE_LEN)-1)<<RF_ANA1_WIFIPLL_LF_BETA_BASE_POS))
#define RF_ANA1_WIFIPLL_LF_ALPHA_FAST                           RF_ANA1_WIFIPLL_LF_ALPHA_FAST
#define RF_ANA1_WIFIPLL_LF_ALPHA_FAST_POS                       (26U)
#define RF_ANA1_WIFIPLL_LF_ALPHA_FAST_LEN                       (2U)
#define RF_ANA1_WIFIPLL_LF_ALPHA_FAST_MSK                       (((1U<<RF_ANA1_WIFIPLL_LF_ALPHA_FAST_LEN)-1)<<RF_ANA1_WIFIPLL_LF_ALPHA_FAST_POS)
#define RF_ANA1_WIFIPLL_LF_ALPHA_FAST_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_LF_ALPHA_FAST_LEN)-1)<<RF_ANA1_WIFIPLL_LF_ALPHA_FAST_POS))
#define RF_ANA1_WIFIPLL_LF_ALPHA_EXP                            RF_ANA1_WIFIPLL_LF_ALPHA_EXP
#define RF_ANA1_WIFIPLL_LF_ALPHA_EXP_POS                        (28U)
#define RF_ANA1_WIFIPLL_LF_ALPHA_EXP_LEN                        (3U)
#define RF_ANA1_WIFIPLL_LF_ALPHA_EXP_MSK                        (((1U<<RF_ANA1_WIFIPLL_LF_ALPHA_EXP_LEN)-1)<<RF_ANA1_WIFIPLL_LF_ALPHA_EXP_POS)
#define RF_ANA1_WIFIPLL_LF_ALPHA_EXP_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_LF_ALPHA_EXP_LEN)-1)<<RF_ANA1_WIFIPLL_LF_ALPHA_EXP_POS))
#define RF_ANA1_WIFIPLL_LF_ALPHA_BASE                           RF_ANA1_WIFIPLL_LF_ALPHA_BASE
#define RF_ANA1_WIFIPLL_LF_ALPHA_BASE_POS                       (31U)
#define RF_ANA1_WIFIPLL_LF_ALPHA_BASE_LEN                       (1U)
#define RF_ANA1_WIFIPLL_LF_ALPHA_BASE_MSK                       (((1U<<RF_ANA1_WIFIPLL_LF_ALPHA_BASE_LEN)-1)<<RF_ANA1_WIFIPLL_LF_ALPHA_BASE_POS)
#define RF_ANA1_WIFIPLL_LF_ALPHA_BASE_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_LF_ALPHA_BASE_LEN)-1)<<RF_ANA1_WIFIPLL_LF_ALPHA_BASE_POS))

/* 0xE0 : wifipll_sdmin */
#define RF_ANA1_WIFIPLL_SDMIN_OFFSET                            (0xE0)
#define RF_ANA1_WIFIPLL_SDM_IN                                  RF_ANA1_WIFIPLL_SDM_IN
#define RF_ANA1_WIFIPLL_SDM_IN_POS                              (0U)
#define RF_ANA1_WIFIPLL_SDM_IN_LEN                              (25U)
#define RF_ANA1_WIFIPLL_SDM_IN_MSK                              (((1U<<RF_ANA1_WIFIPLL_SDM_IN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_IN_POS)
#define RF_ANA1_WIFIPLL_SDM_IN_UMSK                             (~(((1U<<RF_ANA1_WIFIPLL_SDM_IN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_IN_POS))

/* 0xE4 : wifipll_pi_sdm_lms */
#define RF_ANA1_WIFIPLL_PI_SDM_LMS_OFFSET                       (0xE4)
#define RF_ANA1_WIFIPLL_LMS_EXT_VALUE                           RF_ANA1_WIFIPLL_LMS_EXT_VALUE
#define RF_ANA1_WIFIPLL_LMS_EXT_VALUE_POS                       (0U)
#define RF_ANA1_WIFIPLL_LMS_EXT_VALUE_LEN                       (6U)
#define RF_ANA1_WIFIPLL_LMS_EXT_VALUE_MSK                       (((1U<<RF_ANA1_WIFIPLL_LMS_EXT_VALUE_LEN)-1)<<RF_ANA1_WIFIPLL_LMS_EXT_VALUE_POS)
#define RF_ANA1_WIFIPLL_LMS_EXT_VALUE_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_LMS_EXT_VALUE_LEN)-1)<<RF_ANA1_WIFIPLL_LMS_EXT_VALUE_POS))
#define RF_ANA1_WIFIPLL_LMS_EXT_EN                              RF_ANA1_WIFIPLL_LMS_EXT_EN
#define RF_ANA1_WIFIPLL_LMS_EXT_EN_POS                          (7U)
#define RF_ANA1_WIFIPLL_LMS_EXT_EN_LEN                          (1U)
#define RF_ANA1_WIFIPLL_LMS_EXT_EN_MSK                          (((1U<<RF_ANA1_WIFIPLL_LMS_EXT_EN_LEN)-1)<<RF_ANA1_WIFIPLL_LMS_EXT_EN_POS)
#define RF_ANA1_WIFIPLL_LMS_EXT_EN_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_LMS_EXT_EN_LEN)-1)<<RF_ANA1_WIFIPLL_LMS_EXT_EN_POS))
#define RF_ANA1_WIFIPLL_LMS_POLARITY                            RF_ANA1_WIFIPLL_LMS_POLARITY
#define RF_ANA1_WIFIPLL_LMS_POLARITY_POS                        (8U)
#define RF_ANA1_WIFIPLL_LMS_POLARITY_LEN                        (1U)
#define RF_ANA1_WIFIPLL_LMS_POLARITY_MSK                        (((1U<<RF_ANA1_WIFIPLL_LMS_POLARITY_LEN)-1)<<RF_ANA1_WIFIPLL_LMS_POLARITY_POS)
#define RF_ANA1_WIFIPLL_LMS_POLARITY_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_LMS_POLARITY_LEN)-1)<<RF_ANA1_WIFIPLL_LMS_POLARITY_POS))
#define RF_ANA1_WIFIPLL_PHA_CANCEL_EN                           RF_ANA1_WIFIPLL_PHA_CANCEL_EN
#define RF_ANA1_WIFIPLL_PHA_CANCEL_EN_POS                       (12U)
#define RF_ANA1_WIFIPLL_PHA_CANCEL_EN_LEN                       (1U)
#define RF_ANA1_WIFIPLL_PHA_CANCEL_EN_MSK                       (((1U<<RF_ANA1_WIFIPLL_PHA_CANCEL_EN_LEN)-1)<<RF_ANA1_WIFIPLL_PHA_CANCEL_EN_POS)
#define RF_ANA1_WIFIPLL_PHA_CANCEL_EN_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_PHA_CANCEL_EN_LEN)-1)<<RF_ANA1_WIFIPLL_PHA_CANCEL_EN_POS))
#define RF_ANA1_WIFIPLL_SDM_REGRST_EN                           RF_ANA1_WIFIPLL_SDM_REGRST_EN
#define RF_ANA1_WIFIPLL_SDM_REGRST_EN_POS                       (15U)
#define RF_ANA1_WIFIPLL_SDM_REGRST_EN_LEN                       (1U)
#define RF_ANA1_WIFIPLL_SDM_REGRST_EN_MSK                       (((1U<<RF_ANA1_WIFIPLL_SDM_REGRST_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_REGRST_EN_POS)
#define RF_ANA1_WIFIPLL_SDM_REGRST_EN_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_SDM_REGRST_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_REGRST_EN_POS))
#define RF_ANA1_WIFIPLL_SDM_OUT_DLY                             RF_ANA1_WIFIPLL_SDM_OUT_DLY
#define RF_ANA1_WIFIPLL_SDM_OUT_DLY_POS                         (16U)
#define RF_ANA1_WIFIPLL_SDM_OUT_DLY_LEN                         (1U)
#define RF_ANA1_WIFIPLL_SDM_OUT_DLY_MSK                         (((1U<<RF_ANA1_WIFIPLL_SDM_OUT_DLY_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_OUT_DLY_POS)
#define RF_ANA1_WIFIPLL_SDM_OUT_DLY_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_SDM_OUT_DLY_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_OUT_DLY_POS))
#define RF_ANA1_WIFIPLL_SDM_ORDER                               RF_ANA1_WIFIPLL_SDM_ORDER
#define RF_ANA1_WIFIPLL_SDM_ORDER_POS                           (17U)
#define RF_ANA1_WIFIPLL_SDM_ORDER_LEN                           (1U)
#define RF_ANA1_WIFIPLL_SDM_ORDER_MSK                           (((1U<<RF_ANA1_WIFIPLL_SDM_ORDER_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_ORDER_POS)
#define RF_ANA1_WIFIPLL_SDM_ORDER_UMSK                          (~(((1U<<RF_ANA1_WIFIPLL_SDM_ORDER_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_ORDER_POS))
#define RF_ANA1_WIFIPLL_SDM_NOI_DITH                            RF_ANA1_WIFIPLL_SDM_NOI_DITH
#define RF_ANA1_WIFIPLL_SDM_NOI_DITH_POS                        (18U)
#define RF_ANA1_WIFIPLL_SDM_NOI_DITH_LEN                        (1U)
#define RF_ANA1_WIFIPLL_SDM_NOI_DITH_MSK                        (((1U<<RF_ANA1_WIFIPLL_SDM_NOI_DITH_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_NOI_DITH_POS)
#define RF_ANA1_WIFIPLL_SDM_NOI_DITH_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_SDM_NOI_DITH_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_NOI_DITH_POS))
#define RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN                        RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN
#define RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_POS                    (19U)
#define RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_LEN                    (1U)
#define RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_MSK                    (((1U<<RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_POS)
#define RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_UMSK                   (~(((1U<<RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_DITH_PRBS_EN_POS))
#define RF_ANA1_WIFIPLL_SDM_SIG_DITH                            RF_ANA1_WIFIPLL_SDM_SIG_DITH
#define RF_ANA1_WIFIPLL_SDM_SIG_DITH_POS                        (20U)
#define RF_ANA1_WIFIPLL_SDM_SIG_DITH_LEN                        (2U)
#define RF_ANA1_WIFIPLL_SDM_SIG_DITH_MSK                        (((1U<<RF_ANA1_WIFIPLL_SDM_SIG_DITH_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_SIG_DITH_POS)
#define RF_ANA1_WIFIPLL_SDM_SIG_DITH_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_SDM_SIG_DITH_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_SIG_DITH_POS))
#define RF_ANA1_WIFIPLL_SDM_DITH_EN                             RF_ANA1_WIFIPLL_SDM_DITH_EN
#define RF_ANA1_WIFIPLL_SDM_DITH_EN_POS                         (22U)
#define RF_ANA1_WIFIPLL_SDM_DITH_EN_LEN                         (1U)
#define RF_ANA1_WIFIPLL_SDM_DITH_EN_MSK                         (((1U<<RF_ANA1_WIFIPLL_SDM_DITH_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_DITH_EN_POS)
#define RF_ANA1_WIFIPLL_SDM_DITH_EN_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_SDM_DITH_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_DITH_EN_POS))
#define RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN                       RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN
#define RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_POS                   (23U)
#define RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_LEN                   (1U)
#define RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_MSK                   (((1U<<RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_POS)
#define RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_UMSK                  (~(((1U<<RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN_POS))
#define RF_ANA1_WIFIPLL_SDM_BYPASS                              RF_ANA1_WIFIPLL_SDM_BYPASS
#define RF_ANA1_WIFIPLL_SDM_BYPASS_POS                          (24U)
#define RF_ANA1_WIFIPLL_SDM_BYPASS_LEN                          (1U)
#define RF_ANA1_WIFIPLL_SDM_BYPASS_MSK                          (((1U<<RF_ANA1_WIFIPLL_SDM_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_BYPASS_POS)
#define RF_ANA1_WIFIPLL_SDM_BYPASS_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_SDM_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_SDM_BYPASS_POS))
#define RF_ANA1_WIFIPLL_PI_SPACE_SEL                            RF_ANA1_WIFIPLL_PI_SPACE_SEL
#define RF_ANA1_WIFIPLL_PI_SPACE_SEL_POS                        (27U)
#define RF_ANA1_WIFIPLL_PI_SPACE_SEL_LEN                        (1U)
#define RF_ANA1_WIFIPLL_PI_SPACE_SEL_MSK                        (((1U<<RF_ANA1_WIFIPLL_PI_SPACE_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_PI_SPACE_SEL_POS)
#define RF_ANA1_WIFIPLL_PI_SPACE_SEL_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_PI_SPACE_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_PI_SPACE_SEL_POS))
#define RF_ANA1_WIFIPLL_PI_MOD_SEL                              RF_ANA1_WIFIPLL_PI_MOD_SEL
#define RF_ANA1_WIFIPLL_PI_MOD_SEL_POS                          (28U)
#define RF_ANA1_WIFIPLL_PI_MOD_SEL_LEN                          (2U)
#define RF_ANA1_WIFIPLL_PI_MOD_SEL_MSK                          (((1U<<RF_ANA1_WIFIPLL_PI_MOD_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_PI_MOD_SEL_POS)
#define RF_ANA1_WIFIPLL_PI_MOD_SEL_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_PI_MOD_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_PI_MOD_SEL_POS))
#define RF_ANA1_WIFIPLL_PI_CLK_SEL                              RF_ANA1_WIFIPLL_PI_CLK_SEL
#define RF_ANA1_WIFIPLL_PI_CLK_SEL_POS                          (30U)
#define RF_ANA1_WIFIPLL_PI_CLK_SEL_LEN                          (1U)
#define RF_ANA1_WIFIPLL_PI_CLK_SEL_MSK                          (((1U<<RF_ANA1_WIFIPLL_PI_CLK_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_PI_CLK_SEL_POS)
#define RF_ANA1_WIFIPLL_PI_CLK_SEL_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_PI_CLK_SEL_LEN)-1)<<RF_ANA1_WIFIPLL_PI_CLK_SEL_POS))
#define RF_ANA1_WIFIPLL_PI_BYPASS                               RF_ANA1_WIFIPLL_PI_BYPASS
#define RF_ANA1_WIFIPLL_PI_BYPASS_POS                           (31U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_LEN                           (1U)
#define RF_ANA1_WIFIPLL_PI_BYPASS_MSK                           (((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_POS)
#define RF_ANA1_WIFIPLL_PI_BYPASS_UMSK                          (~(((1U<<RF_ANA1_WIFIPLL_PI_BYPASS_LEN)-1)<<RF_ANA1_WIFIPLL_PI_BYPASS_POS))

/* 0xE8 : wifipll_clktree_rf */
#define RF_ANA1_WIFIPLL_CLKTREE_RF_OFFSET                       (0xE8)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ                        RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_POS                    (0U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_LEN                    (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_MSK                    (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_UMSK                   (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_BZ_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ                         RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_POS                     (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_LEN                     (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_MSK                     (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_BZ_POS))
#define RF_ANA1_WIFIPLL_EN_BZ_DIV30                             RF_ANA1_WIFIPLL_EN_BZ_DIV30
#define RF_ANA1_WIFIPLL_EN_BZ_DIV30_POS                         (4U)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV30_LEN                         (1U)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV30_MSK                         (((1U<<RF_ANA1_WIFIPLL_EN_BZ_DIV30_LEN)-1)<<RF_ANA1_WIFIPLL_EN_BZ_DIV30_POS)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV30_UMSK                        (~(((1U<<RF_ANA1_WIFIPLL_EN_BZ_DIV30_LEN)-1)<<RF_ANA1_WIFIPLL_EN_BZ_DIV30_POS))
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW                           RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_POS                       (8U)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_LEN                       (1U)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_MSK                       (((1U<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_LEN)-1)<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_POS)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_LEN)-1)<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_HW_POS))
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6                              RF_ANA1_WIFIPLL_EN_BZ_DIV6
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_POS                          (10U)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_LEN                          (1U)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_MSK                          (((1U<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_POS)
#define RF_ANA1_WIFIPLL_EN_BZ_DIV6_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_EN_BZ_DIV6_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF                         RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_POS                     (12U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_LEN                     (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_MSK                     (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_RF_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF                         RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_POS                     (14U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_LEN                     (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_MSK                     (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_RF_POS))
#define RF_ANA1_WIFIPLL_SEL_DIV3_DIV6                           RF_ANA1_WIFIPLL_SEL_DIV3_DIV6
#define RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_POS                       (18U)
#define RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_LEN                       (1U)
#define RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_MSK                       (((1U<<RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_POS)
#define RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_SEL_DIV3_DIV6_POS))
#define RF_ANA1_WIFIPLL_SEL_DIV1_DIV2                           RF_ANA1_WIFIPLL_SEL_DIV1_DIV2
#define RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_POS                       (19U)
#define RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_LEN                       (1U)
#define RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_MSK                       (((1U<<RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_LEN)-1)<<RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_POS)
#define RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_LEN)-1)<<RF_ANA1_WIFIPLL_SEL_DIV1_DIV2_POS))
#define RF_ANA1_WIFIPLL_EN_RF_DIV3                              RF_ANA1_WIFIPLL_EN_RF_DIV3
#define RF_ANA1_WIFIPLL_EN_RF_DIV3_POS                          (20U)
#define RF_ANA1_WIFIPLL_EN_RF_DIV3_LEN                          (1U)
#define RF_ANA1_WIFIPLL_EN_RF_DIV3_MSK                          (((1U<<RF_ANA1_WIFIPLL_EN_RF_DIV3_LEN)-1)<<RF_ANA1_WIFIPLL_EN_RF_DIV3_POS)
#define RF_ANA1_WIFIPLL_EN_RF_DIV3_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_EN_RF_DIV3_LEN)-1)<<RF_ANA1_WIFIPLL_EN_RF_DIV3_POS))
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_HW                           RF_ANA1_WIFIPLL_EN_RF_DIV1_HW
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_POS                       (21U)
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_LEN                       (1U)
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_MSK                       (((1U<<RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_LEN)-1)<<RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_POS)
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_LEN)-1)<<RF_ANA1_WIFIPLL_EN_RF_DIV1_HW_POS))
#define RF_ANA1_WIFIPLL_EN_RF_DIV1                              RF_ANA1_WIFIPLL_EN_RF_DIV1
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_POS                          (22U)
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_LEN                          (1U)
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_MSK                          (((1U<<RF_ANA1_WIFIPLL_EN_RF_DIV1_LEN)-1)<<RF_ANA1_WIFIPLL_EN_RF_DIV1_POS)
#define RF_ANA1_WIFIPLL_EN_RF_DIV1_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_EN_RF_DIV1_LEN)-1)<<RF_ANA1_WIFIPLL_EN_RF_DIV1_POS))
#define RF_ANA1_WIFIPLL_POSTDIV_EN                              RF_ANA1_WIFIPLL_POSTDIV_EN
#define RF_ANA1_WIFIPLL_POSTDIV_EN_POS                          (23U)
#define RF_ANA1_WIFIPLL_POSTDIV_EN_LEN                          (1U)
#define RF_ANA1_WIFIPLL_POSTDIV_EN_MSK                          (((1U<<RF_ANA1_WIFIPLL_POSTDIV_EN_LEN)-1)<<RF_ANA1_WIFIPLL_POSTDIV_EN_POS)
#define RF_ANA1_WIFIPLL_POSTDIV_EN_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_POSTDIV_EN_LEN)-1)<<RF_ANA1_WIFIPLL_POSTDIV_EN_POS))
#define RF_ANA1_WIFIPLL_POSTDIV                                 RF_ANA1_WIFIPLL_POSTDIV
#define RF_ANA1_WIFIPLL_POSTDIV_POS                             (24U)
#define RF_ANA1_WIFIPLL_POSTDIV_LEN                             (8U)
#define RF_ANA1_WIFIPLL_POSTDIV_MSK                             (((1U<<RF_ANA1_WIFIPLL_POSTDIV_LEN)-1)<<RF_ANA1_WIFIPLL_POSTDIV_POS)
#define RF_ANA1_WIFIPLL_POSTDIV_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_POSTDIV_LEN)-1)<<RF_ANA1_WIFIPLL_POSTDIV_POS))

/* 0xEC : wifipll_clktree_dig */
#define RF_ANA1_WIFIPLL_CLKTREE_DIG_OFFSET                      (0xEC)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30                           RF_ANA1_WIFIPLL_DL_CTRL_DIV30
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_POS                       (0U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_LEN                       (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_MSK                       (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV30_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV30_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV20                           RF_ANA1_WIFIPLL_DL_CTRL_DIV20
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV20_POS                       (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV20_LEN                       (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV20_MSK                       (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV20_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV20_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV20_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV20_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV20_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV12                           RF_ANA1_WIFIPLL_DL_CTRL_DIV12
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV12_POS                       (2U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV12_LEN                       (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV12_MSK                       (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV12_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV12_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV12_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV12_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV12_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV10                           RF_ANA1_WIFIPLL_DL_CTRL_DIV10
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV10_POS                       (3U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV10_LEN                       (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV10_MSK                       (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV10_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV10_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV10_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV10_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV10_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV8                            RF_ANA1_WIFIPLL_DL_CTRL_DIV8
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV8_POS                        (4U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV8_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV8_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV8_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV8_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV8_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV8_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV8_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6                            RF_ANA1_WIFIPLL_DL_CTRL_DIV6
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_POS                        (5U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV6_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV6_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV5                            RF_ANA1_WIFIPLL_DL_CTRL_DIV5
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV5_POS                        (6U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV5_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV5_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV5_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV5_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV5_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV5_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV5_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV4                            RF_ANA1_WIFIPLL_DL_CTRL_DIV4
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV4_POS                        (7U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV4_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV4_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV4_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV4_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV4_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV4_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV4_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3                            RF_ANA1_WIFIPLL_DL_CTRL_DIV3
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_POS                        (8U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV3_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV3_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV2                            RF_ANA1_WIFIPLL_DL_CTRL_DIV2
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV2_POS                        (9U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV2_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV2_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV2_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV2_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV2_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV2_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV2_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1                            RF_ANA1_WIFIPLL_DL_CTRL_DIV1
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_POS                        (10U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_LEN                        (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_MSK                        (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_DIV1_UMSK                       (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_DIV1_POS))
#define RF_ANA1_WIFIPLL_EN_DIV30                                RF_ANA1_WIFIPLL_EN_DIV30
#define RF_ANA1_WIFIPLL_EN_DIV30_POS                            (16U)
#define RF_ANA1_WIFIPLL_EN_DIV30_LEN                            (1U)
#define RF_ANA1_WIFIPLL_EN_DIV30_MSK                            (((1U<<RF_ANA1_WIFIPLL_EN_DIV30_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV30_POS)
#define RF_ANA1_WIFIPLL_EN_DIV30_UMSK                           (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV30_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV30_POS))
#define RF_ANA1_WIFIPLL_EN_DIV20                                RF_ANA1_WIFIPLL_EN_DIV20
#define RF_ANA1_WIFIPLL_EN_DIV20_POS                            (17U)
#define RF_ANA1_WIFIPLL_EN_DIV20_LEN                            (1U)
#define RF_ANA1_WIFIPLL_EN_DIV20_MSK                            (((1U<<RF_ANA1_WIFIPLL_EN_DIV20_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV20_POS)
#define RF_ANA1_WIFIPLL_EN_DIV20_UMSK                           (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV20_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV20_POS))
#define RF_ANA1_WIFIPLL_EN_DIV12                                RF_ANA1_WIFIPLL_EN_DIV12
#define RF_ANA1_WIFIPLL_EN_DIV12_POS                            (18U)
#define RF_ANA1_WIFIPLL_EN_DIV12_LEN                            (1U)
#define RF_ANA1_WIFIPLL_EN_DIV12_MSK                            (((1U<<RF_ANA1_WIFIPLL_EN_DIV12_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV12_POS)
#define RF_ANA1_WIFIPLL_EN_DIV12_UMSK                           (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV12_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV12_POS))
#define RF_ANA1_WIFIPLL_EN_DIV10                                RF_ANA1_WIFIPLL_EN_DIV10
#define RF_ANA1_WIFIPLL_EN_DIV10_POS                            (19U)
#define RF_ANA1_WIFIPLL_EN_DIV10_LEN                            (1U)
#define RF_ANA1_WIFIPLL_EN_DIV10_MSK                            (((1U<<RF_ANA1_WIFIPLL_EN_DIV10_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV10_POS)
#define RF_ANA1_WIFIPLL_EN_DIV10_UMSK                           (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV10_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV10_POS))
#define RF_ANA1_WIFIPLL_EN_DIV8                                 RF_ANA1_WIFIPLL_EN_DIV8
#define RF_ANA1_WIFIPLL_EN_DIV8_POS                             (20U)
#define RF_ANA1_WIFIPLL_EN_DIV8_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV8_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV8_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV8_POS)
#define RF_ANA1_WIFIPLL_EN_DIV8_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV8_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV8_POS))
#define RF_ANA1_WIFIPLL_EN_DIV6                                 RF_ANA1_WIFIPLL_EN_DIV6
#define RF_ANA1_WIFIPLL_EN_DIV6_POS                             (21U)
#define RF_ANA1_WIFIPLL_EN_DIV6_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV6_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV6_POS)
#define RF_ANA1_WIFIPLL_EN_DIV6_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV6_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV6_POS))
#define RF_ANA1_WIFIPLL_EN_DIV5                                 RF_ANA1_WIFIPLL_EN_DIV5
#define RF_ANA1_WIFIPLL_EN_DIV5_POS                             (22U)
#define RF_ANA1_WIFIPLL_EN_DIV5_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV5_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV5_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV5_POS)
#define RF_ANA1_WIFIPLL_EN_DIV5_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV5_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV5_POS))
#define RF_ANA1_WIFIPLL_EN_DIV4                                 RF_ANA1_WIFIPLL_EN_DIV4
#define RF_ANA1_WIFIPLL_EN_DIV4_POS                             (23U)
#define RF_ANA1_WIFIPLL_EN_DIV4_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV4_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV4_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV4_POS)
#define RF_ANA1_WIFIPLL_EN_DIV4_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV4_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV4_POS))
#define RF_ANA1_WIFIPLL_EN_DIV3                                 RF_ANA1_WIFIPLL_EN_DIV3
#define RF_ANA1_WIFIPLL_EN_DIV3_POS                             (24U)
#define RF_ANA1_WIFIPLL_EN_DIV3_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV3_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV3_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV3_POS)
#define RF_ANA1_WIFIPLL_EN_DIV3_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV3_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV3_POS))
#define RF_ANA1_WIFIPLL_EN_DIV2                                 RF_ANA1_WIFIPLL_EN_DIV2
#define RF_ANA1_WIFIPLL_EN_DIV2_POS                             (25U)
#define RF_ANA1_WIFIPLL_EN_DIV2_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV2_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV2_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV2_POS)
#define RF_ANA1_WIFIPLL_EN_DIV2_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV2_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV2_POS))
#define RF_ANA1_WIFIPLL_EN_DIV1                                 RF_ANA1_WIFIPLL_EN_DIV1
#define RF_ANA1_WIFIPLL_EN_DIV1_POS                             (26U)
#define RF_ANA1_WIFIPLL_EN_DIV1_LEN                             (1U)
#define RF_ANA1_WIFIPLL_EN_DIV1_MSK                             (((1U<<RF_ANA1_WIFIPLL_EN_DIV1_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV1_POS)
#define RF_ANA1_WIFIPLL_EN_DIV1_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_EN_DIV1_LEN)-1)<<RF_ANA1_WIFIPLL_EN_DIV1_POS))
#define RF_ANA1_WIFIPLL_EN_USB2_DIV48                           RF_ANA1_WIFIPLL_EN_USB2_DIV48
#define RF_ANA1_WIFIPLL_EN_USB2_DIV48_POS                       (27U)
#define RF_ANA1_WIFIPLL_EN_USB2_DIV48_LEN                       (1U)
#define RF_ANA1_WIFIPLL_EN_USB2_DIV48_MSK                       (((1U<<RF_ANA1_WIFIPLL_EN_USB2_DIV48_LEN)-1)<<RF_ANA1_WIFIPLL_EN_USB2_DIV48_POS)
#define RF_ANA1_WIFIPLL_EN_USB2_DIV48_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_EN_USB2_DIV48_LEN)-1)<<RF_ANA1_WIFIPLL_EN_USB2_DIV48_POS))
#define RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48                      RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48
#define RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_POS                  (28U)
#define RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_LEN                  (1U)
#define RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_MSK                  (((1U<<RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_POS)
#define RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_UMSK                 (~(((1U<<RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_LEN)-1)<<RF_ANA1_WIFIPLL_DL_CTRL_USB2_DIV48_POS))

/* 0xF0 : wifipll_reserve */
#define RF_ANA1_WIFIPLL_RESERVE_OFFSET                          (0xF0)
#define RF_ANA1_WIFIPLL_RESV0                                   RF_ANA1_WIFIPLL_RESV0
#define RF_ANA1_WIFIPLL_RESV0_POS                               (0U)
#define RF_ANA1_WIFIPLL_RESV0_LEN                               (8U)
#define RF_ANA1_WIFIPLL_RESV0_MSK                               (((1U<<RF_ANA1_WIFIPLL_RESV0_LEN)-1)<<RF_ANA1_WIFIPLL_RESV0_POS)
#define RF_ANA1_WIFIPLL_RESV0_UMSK                              (~(((1U<<RF_ANA1_WIFIPLL_RESV0_LEN)-1)<<RF_ANA1_WIFIPLL_RESV0_POS))
#define RF_ANA1_WIFIPLL_RESV1                                   RF_ANA1_WIFIPLL_RESV1
#define RF_ANA1_WIFIPLL_RESV1_POS                               (8U)
#define RF_ANA1_WIFIPLL_RESV1_LEN                               (8U)
#define RF_ANA1_WIFIPLL_RESV1_MSK                               (((1U<<RF_ANA1_WIFIPLL_RESV1_LEN)-1)<<RF_ANA1_WIFIPLL_RESV1_POS)
#define RF_ANA1_WIFIPLL_RESV1_UMSK                              (~(((1U<<RF_ANA1_WIFIPLL_RESV1_LEN)-1)<<RF_ANA1_WIFIPLL_RESV1_POS))
#define RF_ANA1_WIFIPLL_VCO_FREQ_RECORD                         RF_ANA1_WIFIPLL_VCO_FREQ_RECORD
#define RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_POS                     (16U)
#define RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_LEN                     (16U)
#define RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_MSK                     (((1U<<RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_POS)
#define RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_LEN)-1)<<RF_ANA1_WIFIPLL_VCO_FREQ_RECORD_POS))

/* 0xF4 : wifipll_test */
#define RF_ANA1_WIFIPLL_TEST_OFFSET                             (0xF4)
#define RF_ANA1_WIFIPLL_TEST_DATA_MUX                           RF_ANA1_WIFIPLL_TEST_DATA_MUX
#define RF_ANA1_WIFIPLL_TEST_DATA_MUX_POS                       (0U)
#define RF_ANA1_WIFIPLL_TEST_DATA_MUX_LEN                       (1U)
#define RF_ANA1_WIFIPLL_TEST_DATA_MUX_MSK                       (((1U<<RF_ANA1_WIFIPLL_TEST_DATA_MUX_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_DATA_MUX_POS)
#define RF_ANA1_WIFIPLL_TEST_DATA_MUX_UMSK                      (~(((1U<<RF_ANA1_WIFIPLL_TEST_DATA_MUX_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_DATA_MUX_POS))
#define RF_ANA1_WIFIPLL_TEST_START_MUX                          RF_ANA1_WIFIPLL_TEST_START_MUX
#define RF_ANA1_WIFIPLL_TEST_START_MUX_POS                      (4U)
#define RF_ANA1_WIFIPLL_TEST_START_MUX_LEN                      (1U)
#define RF_ANA1_WIFIPLL_TEST_START_MUX_MSK                      (((1U<<RF_ANA1_WIFIPLL_TEST_START_MUX_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_START_MUX_POS)
#define RF_ANA1_WIFIPLL_TEST_START_MUX_UMSK                     (~(((1U<<RF_ANA1_WIFIPLL_TEST_START_MUX_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_START_MUX_POS))
#define RF_ANA1_WIFIPLL_TEST_EN                                 RF_ANA1_WIFIPLL_TEST_EN
#define RF_ANA1_WIFIPLL_TEST_EN_POS                             (8U)
#define RF_ANA1_WIFIPLL_TEST_EN_LEN                             (1U)
#define RF_ANA1_WIFIPLL_TEST_EN_MSK                             (((1U<<RF_ANA1_WIFIPLL_TEST_EN_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_EN_POS)
#define RF_ANA1_WIFIPLL_TEST_EN_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_TEST_EN_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_EN_POS))
#define RF_ANA1_DTEN_WIFIPLL_REFCLK                             RF_ANA1_DTEN_WIFIPLL_REFCLK
#define RF_ANA1_DTEN_WIFIPLL_REFCLK_POS                         (10U)
#define RF_ANA1_DTEN_WIFIPLL_REFCLK_LEN                         (1U)
#define RF_ANA1_DTEN_WIFIPLL_REFCLK_MSK                         (((1U<<RF_ANA1_DTEN_WIFIPLL_REFCLK_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_REFCLK_POS)
#define RF_ANA1_DTEN_WIFIPLL_REFCLK_UMSK                        (~(((1U<<RF_ANA1_DTEN_WIFIPLL_REFCLK_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_REFCLK_POS))
#define RF_ANA1_DTEN_WIFIPLL_POSTDIV                            RF_ANA1_DTEN_WIFIPLL_POSTDIV
#define RF_ANA1_DTEN_WIFIPLL_POSTDIV_POS                        (12U)
#define RF_ANA1_DTEN_WIFIPLL_POSTDIV_LEN                        (1U)
#define RF_ANA1_DTEN_WIFIPLL_POSTDIV_MSK                        (((1U<<RF_ANA1_DTEN_WIFIPLL_POSTDIV_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_POSTDIV_POS)
#define RF_ANA1_DTEN_WIFIPLL_POSTDIV_UMSK                       (~(((1U<<RF_ANA1_DTEN_WIFIPLL_POSTDIV_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_POSTDIV_POS))
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM                         RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_POS                     (16U)
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_LEN                     (1U)
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_MSK                     (((1U<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_POS)
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_UMSK                    (~(((1U<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FSDM_POS))
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB                          RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_POS                      (20U)
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_LEN                      (1U)
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_MSK                      (((1U<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_POS)
#define RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_UMSK                     (~(((1U<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_FBDIV_FFB_POS))
#define RF_ANA1_DTEN_WIFIPLL_CLK32M                             RF_ANA1_DTEN_WIFIPLL_CLK32M
#define RF_ANA1_DTEN_WIFIPLL_CLK32M_POS                         (24U)
#define RF_ANA1_DTEN_WIFIPLL_CLK32M_LEN                         (1U)
#define RF_ANA1_DTEN_WIFIPLL_CLK32M_MSK                         (((1U<<RF_ANA1_DTEN_WIFIPLL_CLK32M_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_CLK32M_POS)
#define RF_ANA1_DTEN_WIFIPLL_CLK32M_UMSK                        (~(((1U<<RF_ANA1_DTEN_WIFIPLL_CLK32M_LEN)-1)<<RF_ANA1_DTEN_WIFIPLL_CLK32M_POS))
#define RF_ANA1_TEN_WIFIPLL                                     RF_ANA1_TEN_WIFIPLL
#define RF_ANA1_TEN_WIFIPLL_POS                                 (28U)
#define RF_ANA1_TEN_WIFIPLL_LEN                                 (1U)
#define RF_ANA1_TEN_WIFIPLL_MSK                                 (((1U<<RF_ANA1_TEN_WIFIPLL_LEN)-1)<<RF_ANA1_TEN_WIFIPLL_POS)
#define RF_ANA1_TEN_WIFIPLL_UMSK                                (~(((1U<<RF_ANA1_TEN_WIFIPLL_LEN)-1)<<RF_ANA1_TEN_WIFIPLL_POS))

/* 0xF8 : wifipll_readback */
#define RF_ANA1_WIFIPLL_READBACK_OFFSET                         (0xF8)
#define RF_ANA1_WIFIPLL_TEST_START                              RF_ANA1_WIFIPLL_TEST_START
#define RF_ANA1_WIFIPLL_TEST_START_POS                          (9U)
#define RF_ANA1_WIFIPLL_TEST_START_LEN                          (1U)
#define RF_ANA1_WIFIPLL_TEST_START_MSK                          (((1U<<RF_ANA1_WIFIPLL_TEST_START_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_START_POS)
#define RF_ANA1_WIFIPLL_TEST_START_UMSK                         (~(((1U<<RF_ANA1_WIFIPLL_TEST_START_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_START_POS))
#define RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE                         RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE
#define RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_POS                     (12U)
#define RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_LEN                     (1U)
#define RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_MSK                     (((1U<<RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_LEN)-1)<<RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_POS)
#define RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_UMSK                    (~(((1U<<RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_LEN)-1)<<RF_ANA1_WIFIPLL_VCTRL_OUT_RANGE_POS))
#define RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU                 RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU
#define RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_POS             (16U)
#define RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN             (2U)
#define RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_MSK             (((1U<<RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN)-1)<<RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_POS)
#define RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_UMSK            (~(((1U<<RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN)-1)<<RF_ANA1_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_POS))
#define RF_ANA1_WIFIPLL_LO_UNLOCK                               RF_ANA1_WIFIPLL_LO_UNLOCK
#define RF_ANA1_WIFIPLL_LO_UNLOCK_POS                           (20U)
#define RF_ANA1_WIFIPLL_LO_UNLOCK_LEN                           (1U)
#define RF_ANA1_WIFIPLL_LO_UNLOCK_MSK                           (((1U<<RF_ANA1_WIFIPLL_LO_UNLOCK_LEN)-1)<<RF_ANA1_WIFIPLL_LO_UNLOCK_POS)
#define RF_ANA1_WIFIPLL_LO_UNLOCK_UMSK                          (~(((1U<<RF_ANA1_WIFIPLL_LO_UNLOCK_LEN)-1)<<RF_ANA1_WIFIPLL_LO_UNLOCK_POS))
#define RF_ANA1_WIFIPLL_LO_LOCK                                 RF_ANA1_WIFIPLL_LO_LOCK
#define RF_ANA1_WIFIPLL_LO_LOCK_POS                             (24U)
#define RF_ANA1_WIFIPLL_LO_LOCK_LEN                             (1U)
#define RF_ANA1_WIFIPLL_LO_LOCK_MSK                             (((1U<<RF_ANA1_WIFIPLL_LO_LOCK_LEN)-1)<<RF_ANA1_WIFIPLL_LO_LOCK_POS)
#define RF_ANA1_WIFIPLL_LO_LOCK_UMSK                            (~(((1U<<RF_ANA1_WIFIPLL_LO_LOCK_LEN)-1)<<RF_ANA1_WIFIPLL_LO_LOCK_POS))
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW                    RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_POS                (28U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_LEN                (1U)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_MSK                (((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_POS)
#define RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_UMSK               (~(((1U<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_LEN)-1)<<RF_ANA1_WIFIPLL_FAST_INC_FCAL_OVFLOW_POS))

/* 0xFC : wifipll_adpll_test_out */
#define RF_ANA1_WIFIPLL_ADPLL_TEST_OUT_OFFSET                   (0xFC)
#define RF_ANA1_WIFIPLL_TEST_OUT                                RF_ANA1_WIFIPLL_TEST_OUT
#define RF_ANA1_WIFIPLL_TEST_OUT_POS                            (0U)
#define RF_ANA1_WIFIPLL_TEST_OUT_LEN                            (32U)
#define RF_ANA1_WIFIPLL_TEST_OUT_MSK                            (((1U<<RF_ANA1_WIFIPLL_TEST_OUT_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_OUT_POS)
#define RF_ANA1_WIFIPLL_TEST_OUT_UMSK                           (~(((1U<<RF_ANA1_WIFIPLL_TEST_OUT_LEN)-1)<<RF_ANA1_WIFIPLL_TEST_OUT_POS))

/* 0x100 : lo_vco */
#define RF_ANA1_LO_VCO_OFFSET                                   (0x100)
#define RF_ANA1_VCO_VBIAS_CW                                    RF_ANA1_VCO_VBIAS_CW
#define RF_ANA1_VCO_VBIAS_CW_POS                                (0U)
#define RF_ANA1_VCO_VBIAS_CW_LEN                                (3U)
#define RF_ANA1_VCO_VBIAS_CW_MSK                                (((1U<<RF_ANA1_VCO_VBIAS_CW_LEN)-1)<<RF_ANA1_VCO_VBIAS_CW_POS)
#define RF_ANA1_VCO_VBIAS_CW_UMSK                               (~(((1U<<RF_ANA1_VCO_VBIAS_CW_LEN)-1)<<RF_ANA1_VCO_VBIAS_CW_POS))
#define RF_ANA1_VCO_IDAC_CW                                     RF_ANA1_VCO_IDAC_CW
#define RF_ANA1_VCO_IDAC_CW_POS                                 (8U)
#define RF_ANA1_VCO_IDAC_CW_LEN                                 (7U)
#define RF_ANA1_VCO_IDAC_CW_MSK                                 (((1U<<RF_ANA1_VCO_IDAC_CW_LEN)-1)<<RF_ANA1_VCO_IDAC_CW_POS)
#define RF_ANA1_VCO_IDAC_CW_UMSK                                (~(((1U<<RF_ANA1_VCO_IDAC_CW_LEN)-1)<<RF_ANA1_VCO_IDAC_CW_POS))
#define RF_ANA1_VCO_IDAC_CW_HW                                  RF_ANA1_VCO_IDAC_CW_HW
#define RF_ANA1_VCO_IDAC_CW_HW_POS                              (20U)
#define RF_ANA1_VCO_IDAC_CW_HW_LEN                              (7U)
#define RF_ANA1_VCO_IDAC_CW_HW_MSK                              (((1U<<RF_ANA1_VCO_IDAC_CW_HW_LEN)-1)<<RF_ANA1_VCO_IDAC_CW_HW_POS)
#define RF_ANA1_VCO_IDAC_CW_HW_UMSK                             (~(((1U<<RF_ANA1_VCO_IDAC_CW_HW_LEN)-1)<<RF_ANA1_VCO_IDAC_CW_HW_POS))

/* 0x104 : lo_vco1 */
#define RF_ANA1_LO_VCO1_OFFSET                                  (0x104)
#define RF_ANA1_VCO_ACAL_UD                                     RF_ANA1_VCO_ACAL_UD
#define RF_ANA1_VCO_ACAL_UD_POS                                 (0U)
#define RF_ANA1_VCO_ACAL_UD_LEN                                 (1U)
#define RF_ANA1_VCO_ACAL_UD_MSK                                 (((1U<<RF_ANA1_VCO_ACAL_UD_LEN)-1)<<RF_ANA1_VCO_ACAL_UD_POS)
#define RF_ANA1_VCO_ACAL_UD_UMSK                                (~(((1U<<RF_ANA1_VCO_ACAL_UD_LEN)-1)<<RF_ANA1_VCO_ACAL_UD_POS))
#define RF_ANA1_VCO_ACAL_VREF_CW                                RF_ANA1_VCO_ACAL_VREF_CW
#define RF_ANA1_VCO_ACAL_VREF_CW_POS                            (4U)
#define RF_ANA1_VCO_ACAL_VREF_CW_LEN                            (4U)
#define RF_ANA1_VCO_ACAL_VREF_CW_MSK                            (((1U<<RF_ANA1_VCO_ACAL_VREF_CW_LEN)-1)<<RF_ANA1_VCO_ACAL_VREF_CW_POS)
#define RF_ANA1_VCO_ACAL_VREF_CW_UMSK                           (~(((1U<<RF_ANA1_VCO_ACAL_VREF_CW_LEN)-1)<<RF_ANA1_VCO_ACAL_VREF_CW_POS))
#define RF_ANA1_VCO_ACAL_EN                                     RF_ANA1_VCO_ACAL_EN
#define RF_ANA1_VCO_ACAL_EN_POS                                 (8U)
#define RF_ANA1_VCO_ACAL_EN_LEN                                 (1U)
#define RF_ANA1_VCO_ACAL_EN_MSK                                 (((1U<<RF_ANA1_VCO_ACAL_EN_LEN)-1)<<RF_ANA1_VCO_ACAL_EN_POS)
#define RF_ANA1_VCO_ACAL_EN_UMSK                                (~(((1U<<RF_ANA1_VCO_ACAL_EN_LEN)-1)<<RF_ANA1_VCO_ACAL_EN_POS))
#define RF_ANA1_VCO_ACAL_MODE                                   RF_ANA1_VCO_ACAL_MODE
#define RF_ANA1_VCO_ACAL_MODE_POS                               (10U)
#define RF_ANA1_VCO_ACAL_MODE_LEN                               (2U)
#define RF_ANA1_VCO_ACAL_MODE_MSK                               (((1U<<RF_ANA1_VCO_ACAL_MODE_LEN)-1)<<RF_ANA1_VCO_ACAL_MODE_POS)
#define RF_ANA1_VCO_ACAL_MODE_UMSK                              (~(((1U<<RF_ANA1_VCO_ACAL_MODE_LEN)-1)<<RF_ANA1_VCO_ACAL_MODE_POS))
#define RF_ANA1_VCO_INC_ACAL_EN                                 RF_ANA1_VCO_INC_ACAL_EN
#define RF_ANA1_VCO_INC_ACAL_EN_POS                             (12U)
#define RF_ANA1_VCO_INC_ACAL_EN_LEN                             (1U)
#define RF_ANA1_VCO_INC_ACAL_EN_MSK                             (((1U<<RF_ANA1_VCO_INC_ACAL_EN_LEN)-1)<<RF_ANA1_VCO_INC_ACAL_EN_POS)
#define RF_ANA1_VCO_INC_ACAL_EN_UMSK                            (~(((1U<<RF_ANA1_VCO_INC_ACAL_EN_LEN)-1)<<RF_ANA1_VCO_INC_ACAL_EN_POS))
#define RF_ANA1_VCO_INC_ACAL_EN_HW                              RF_ANA1_VCO_INC_ACAL_EN_HW
#define RF_ANA1_VCO_INC_ACAL_EN_HW_POS                          (13U)
#define RF_ANA1_VCO_INC_ACAL_EN_HW_LEN                          (1U)
#define RF_ANA1_VCO_INC_ACAL_EN_HW_MSK                          (((1U<<RF_ANA1_VCO_INC_ACAL_EN_HW_LEN)-1)<<RF_ANA1_VCO_INC_ACAL_EN_HW_POS)
#define RF_ANA1_VCO_INC_ACAL_EN_HW_UMSK                         (~(((1U<<RF_ANA1_VCO_INC_ACAL_EN_HW_LEN)-1)<<RF_ANA1_VCO_INC_ACAL_EN_HW_POS))
#define RF_ANA1_VCO_LODISTBUF_BOOST                             RF_ANA1_VCO_LODISTBUF_BOOST
#define RF_ANA1_VCO_LODISTBUF_BOOST_POS                         (16U)
#define RF_ANA1_VCO_LODISTBUF_BOOST_LEN                         (1U)
#define RF_ANA1_VCO_LODISTBUF_BOOST_MSK                         (((1U<<RF_ANA1_VCO_LODISTBUF_BOOST_LEN)-1)<<RF_ANA1_VCO_LODISTBUF_BOOST_POS)
#define RF_ANA1_VCO_LODISTBUF_BOOST_UMSK                        (~(((1U<<RF_ANA1_VCO_LODISTBUF_BOOST_LEN)-1)<<RF_ANA1_VCO_LODISTBUF_BOOST_POS))
#define RF_ANA1_VCO_PUD_GATED_EN                                RF_ANA1_VCO_PUD_GATED_EN
#define RF_ANA1_VCO_PUD_GATED_EN_POS                            (20U)
#define RF_ANA1_VCO_PUD_GATED_EN_LEN                            (1U)
#define RF_ANA1_VCO_PUD_GATED_EN_MSK                            (((1U<<RF_ANA1_VCO_PUD_GATED_EN_LEN)-1)<<RF_ANA1_VCO_PUD_GATED_EN_POS)
#define RF_ANA1_VCO_PUD_GATED_EN_UMSK                           (~(((1U<<RF_ANA1_VCO_PUD_GATED_EN_LEN)-1)<<RF_ANA1_VCO_PUD_GATED_EN_POS))
#define RF_ANA1_VCO_VBIAS_FLT_SHORT                             RF_ANA1_VCO_VBIAS_FLT_SHORT
#define RF_ANA1_VCO_VBIAS_FLT_SHORT_POS                         (24U)
#define RF_ANA1_VCO_VBIAS_FLT_SHORT_LEN                         (1U)
#define RF_ANA1_VCO_VBIAS_FLT_SHORT_MSK                         (((1U<<RF_ANA1_VCO_VBIAS_FLT_SHORT_LEN)-1)<<RF_ANA1_VCO_VBIAS_FLT_SHORT_POS)
#define RF_ANA1_VCO_VBIAS_FLT_SHORT_UMSK                        (~(((1U<<RF_ANA1_VCO_VBIAS_FLT_SHORT_LEN)-1)<<RF_ANA1_VCO_VBIAS_FLT_SHORT_POS))
#define RF_ANA1_VCO_IDAC_FLT_SHORT                              RF_ANA1_VCO_IDAC_FLT_SHORT
#define RF_ANA1_VCO_IDAC_FLT_SHORT_POS                          (28U)
#define RF_ANA1_VCO_IDAC_FLT_SHORT_LEN                          (1U)
#define RF_ANA1_VCO_IDAC_FLT_SHORT_MSK                          (((1U<<RF_ANA1_VCO_IDAC_FLT_SHORT_LEN)-1)<<RF_ANA1_VCO_IDAC_FLT_SHORT_POS)
#define RF_ANA1_VCO_IDAC_FLT_SHORT_UMSK                         (~(((1U<<RF_ANA1_VCO_IDAC_FLT_SHORT_LEN)-1)<<RF_ANA1_VCO_IDAC_FLT_SHORT_POS))

/* 0x108 : lo_dtc */
#define RF_ANA1_LO_DTC_OFFSET                                   (0x108)
#define RF_ANA1_ADPLL_DTC_R                                     RF_ANA1_ADPLL_DTC_R
#define RF_ANA1_ADPLL_DTC_R_POS                                 (0U)
#define RF_ANA1_ADPLL_DTC_R_LEN                                 (2U)
#define RF_ANA1_ADPLL_DTC_R_MSK                                 (((1U<<RF_ANA1_ADPLL_DTC_R_LEN)-1)<<RF_ANA1_ADPLL_DTC_R_POS)
#define RF_ANA1_ADPLL_DTC_R_UMSK                                (~(((1U<<RF_ANA1_ADPLL_DTC_R_LEN)-1)<<RF_ANA1_ADPLL_DTC_R_POS))
#define RF_ANA1_ADPLL_DTC_R_HW                                  RF_ANA1_ADPLL_DTC_R_HW
#define RF_ANA1_ADPLL_DTC_R_HW_POS                              (2U)
#define RF_ANA1_ADPLL_DTC_R_HW_LEN                              (2U)
#define RF_ANA1_ADPLL_DTC_R_HW_MSK                              (((1U<<RF_ANA1_ADPLL_DTC_R_HW_LEN)-1)<<RF_ANA1_ADPLL_DTC_R_HW_POS)
#define RF_ANA1_ADPLL_DTC_R_HW_UMSK                             (~(((1U<<RF_ANA1_ADPLL_DTC_R_HW_LEN)-1)<<RF_ANA1_ADPLL_DTC_R_HW_POS))
#define RF_ANA1_ADPLL_DTC_FLT_SHT                               RF_ANA1_ADPLL_DTC_FLT_SHT
#define RF_ANA1_ADPLL_DTC_FLT_SHT_POS                           (4U)
#define RF_ANA1_ADPLL_DTC_FLT_SHT_LEN                           (1U)
#define RF_ANA1_ADPLL_DTC_FLT_SHT_MSK                           (((1U<<RF_ANA1_ADPLL_DTC_FLT_SHT_LEN)-1)<<RF_ANA1_ADPLL_DTC_FLT_SHT_POS)
#define RF_ANA1_ADPLL_DTC_FLT_SHT_UMSK                          (~(((1U<<RF_ANA1_ADPLL_DTC_FLT_SHT_LEN)-1)<<RF_ANA1_ADPLL_DTC_FLT_SHT_POS))
#define RF_ANA1_ADPLL_DTC_CLK_INV_EN                            RF_ANA1_ADPLL_DTC_CLK_INV_EN
#define RF_ANA1_ADPLL_DTC_CLK_INV_EN_POS                        (8U)
#define RF_ANA1_ADPLL_DTC_CLK_INV_EN_LEN                        (1U)
#define RF_ANA1_ADPLL_DTC_CLK_INV_EN_MSK                        (((1U<<RF_ANA1_ADPLL_DTC_CLK_INV_EN_LEN)-1)<<RF_ANA1_ADPLL_DTC_CLK_INV_EN_POS)
#define RF_ANA1_ADPLL_DTC_CLK_INV_EN_UMSK                       (~(((1U<<RF_ANA1_ADPLL_DTC_CLK_INV_EN_LEN)-1)<<RF_ANA1_ADPLL_DTC_CLK_INV_EN_POS))
#define RF_ANA1_ADPLL_DTC_PUD_GATED_EN                          RF_ANA1_ADPLL_DTC_PUD_GATED_EN
#define RF_ANA1_ADPLL_DTC_PUD_GATED_EN_POS                      (12U)
#define RF_ANA1_ADPLL_DTC_PUD_GATED_EN_LEN                      (1U)
#define RF_ANA1_ADPLL_DTC_PUD_GATED_EN_MSK                      (((1U<<RF_ANA1_ADPLL_DTC_PUD_GATED_EN_LEN)-1)<<RF_ANA1_ADPLL_DTC_PUD_GATED_EN_POS)
#define RF_ANA1_ADPLL_DTC_PUD_GATED_EN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_DTC_PUD_GATED_EN_LEN)-1)<<RF_ANA1_ADPLL_DTC_PUD_GATED_EN_POS))
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN                            RF_ANA1_ADPLL_DTC_LP_MODE_EN
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_POS                        (16U)
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_LEN                        (1U)
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_MSK                        (((1U<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_LEN)-1)<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_POS)
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_UMSK                       (~(((1U<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_LEN)-1)<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_POS))
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW                         RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_POS                     (17U)
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_LEN                     (1U)
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_MSK                     (((1U<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_POS)
#define RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_UMSK                    (~(((1U<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_DTC_LP_MODE_EN_HW_POS))
#define RF_ANA1_ADPLL_DTC_BYPASS                                RF_ANA1_ADPLL_DTC_BYPASS
#define RF_ANA1_ADPLL_DTC_BYPASS_POS                            (20U)
#define RF_ANA1_ADPLL_DTC_BYPASS_LEN                            (1U)
#define RF_ANA1_ADPLL_DTC_BYPASS_MSK                            (((1U<<RF_ANA1_ADPLL_DTC_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_DTC_BYPASS_POS)
#define RF_ANA1_ADPLL_DTC_BYPASS_UMSK                           (~(((1U<<RF_ANA1_ADPLL_DTC_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_DTC_BYPASS_POS))

/* 0x10C : osmx */
#define RF_ANA1_OSMX_OFFSET                                     (0x10C)
#define RF_ANA1_OSMX_DIV2_RLOAD_SEL                             RF_ANA1_OSMX_DIV2_RLOAD_SEL
#define RF_ANA1_OSMX_DIV2_RLOAD_SEL_POS                         (0U)
#define RF_ANA1_OSMX_DIV2_RLOAD_SEL_LEN                         (1U)
#define RF_ANA1_OSMX_DIV2_RLOAD_SEL_MSK                         (((1U<<RF_ANA1_OSMX_DIV2_RLOAD_SEL_LEN)-1)<<RF_ANA1_OSMX_DIV2_RLOAD_SEL_POS)
#define RF_ANA1_OSMX_DIV2_RLOAD_SEL_UMSK                        (~(((1U<<RF_ANA1_OSMX_DIV2_RLOAD_SEL_LEN)-1)<<RF_ANA1_OSMX_DIV2_RLOAD_SEL_POS))
#define RF_ANA1_OSMX_BUF2G_RLOAD_SEL                            RF_ANA1_OSMX_BUF2G_RLOAD_SEL
#define RF_ANA1_OSMX_BUF2G_RLOAD_SEL_POS                        (4U)
#define RF_ANA1_OSMX_BUF2G_RLOAD_SEL_LEN                        (1U)
#define RF_ANA1_OSMX_BUF2G_RLOAD_SEL_MSK                        (((1U<<RF_ANA1_OSMX_BUF2G_RLOAD_SEL_LEN)-1)<<RF_ANA1_OSMX_BUF2G_RLOAD_SEL_POS)
#define RF_ANA1_OSMX_BUF2G_RLOAD_SEL_UMSK                       (~(((1U<<RF_ANA1_OSMX_BUF2G_RLOAD_SEL_LEN)-1)<<RF_ANA1_OSMX_BUF2G_RLOAD_SEL_POS))
#define RF_ANA1_OSMX_COMP_EN                                    RF_ANA1_OSMX_COMP_EN
#define RF_ANA1_OSMX_COMP_EN_POS                                (8U)
#define RF_ANA1_OSMX_COMP_EN_LEN                                (1U)
#define RF_ANA1_OSMX_COMP_EN_MSK                                (((1U<<RF_ANA1_OSMX_COMP_EN_LEN)-1)<<RF_ANA1_OSMX_COMP_EN_POS)
#define RF_ANA1_OSMX_COMP_EN_UMSK                               (~(((1U<<RF_ANA1_OSMX_COMP_EN_LEN)-1)<<RF_ANA1_OSMX_COMP_EN_POS))
#define RF_ANA1_OSMX_CAP_SEL                                    RF_ANA1_OSMX_CAP_SEL
#define RF_ANA1_OSMX_CAP_SEL_POS                                (12U)
#define RF_ANA1_OSMX_CAP_SEL_LEN                                (5U)
#define RF_ANA1_OSMX_CAP_SEL_MSK                                (((1U<<RF_ANA1_OSMX_CAP_SEL_LEN)-1)<<RF_ANA1_OSMX_CAP_SEL_POS)
#define RF_ANA1_OSMX_CAP_SEL_UMSK                               (~(((1U<<RF_ANA1_OSMX_CAP_SEL_LEN)-1)<<RF_ANA1_OSMX_CAP_SEL_POS))
#define RF_ANA1_OSMX_BM_DIV2                                    RF_ANA1_OSMX_BM_DIV2
#define RF_ANA1_OSMX_BM_DIV2_POS                                (20U)
#define RF_ANA1_OSMX_BM_DIV2_LEN                                (2U)
#define RF_ANA1_OSMX_BM_DIV2_MSK                                (((1U<<RF_ANA1_OSMX_BM_DIV2_LEN)-1)<<RF_ANA1_OSMX_BM_DIV2_POS)
#define RF_ANA1_OSMX_BM_DIV2_UMSK                               (~(((1U<<RF_ANA1_OSMX_BM_DIV2_LEN)-1)<<RF_ANA1_OSMX_BM_DIV2_POS))
#define RF_ANA1_OSMX_BM_CORE                                    RF_ANA1_OSMX_BM_CORE
#define RF_ANA1_OSMX_BM_CORE_POS                                (24U)
#define RF_ANA1_OSMX_BM_CORE_LEN                                (2U)
#define RF_ANA1_OSMX_BM_CORE_MSK                                (((1U<<RF_ANA1_OSMX_BM_CORE_LEN)-1)<<RF_ANA1_OSMX_BM_CORE_POS)
#define RF_ANA1_OSMX_BM_CORE_UMSK                               (~(((1U<<RF_ANA1_OSMX_BM_CORE_LEN)-1)<<RF_ANA1_OSMX_BM_CORE_POS))

/* 0x110 : osmx1 */
#define RF_ANA1_OSMX1_OFFSET                                    (0x110)
#define RF_ANA1_OSMX_VBIAS_LO                                   RF_ANA1_OSMX_VBIAS_LO
#define RF_ANA1_OSMX_VBIAS_LO_POS                               (0U)
#define RF_ANA1_OSMX_VBIAS_LO_LEN                               (2U)
#define RF_ANA1_OSMX_VBIAS_LO_MSK                               (((1U<<RF_ANA1_OSMX_VBIAS_LO_LEN)-1)<<RF_ANA1_OSMX_VBIAS_LO_POS)
#define RF_ANA1_OSMX_VBIAS_LO_UMSK                              (~(((1U<<RF_ANA1_OSMX_VBIAS_LO_LEN)-1)<<RF_ANA1_OSMX_VBIAS_LO_POS))
#define RF_ANA1_OSMX_VBIAS_CAP                                  RF_ANA1_OSMX_VBIAS_CAP
#define RF_ANA1_OSMX_VBIAS_CAP_POS                              (4U)
#define RF_ANA1_OSMX_VBIAS_CAP_LEN                              (2U)
#define RF_ANA1_OSMX_VBIAS_CAP_MSK                              (((1U<<RF_ANA1_OSMX_VBIAS_CAP_LEN)-1)<<RF_ANA1_OSMX_VBIAS_CAP_POS)
#define RF_ANA1_OSMX_VBIAS_CAP_UMSK                             (~(((1U<<RF_ANA1_OSMX_VBIAS_CAP_LEN)-1)<<RF_ANA1_OSMX_VBIAS_CAP_POS))
#define RF_ANA1_OSMX_SUPPLY_BOOST                               RF_ANA1_OSMX_SUPPLY_BOOST
#define RF_ANA1_OSMX_SUPPLY_BOOST_POS                           (8U)
#define RF_ANA1_OSMX_SUPPLY_BOOST_LEN                           (1U)
#define RF_ANA1_OSMX_SUPPLY_BOOST_MSK                           (((1U<<RF_ANA1_OSMX_SUPPLY_BOOST_LEN)-1)<<RF_ANA1_OSMX_SUPPLY_BOOST_POS)
#define RF_ANA1_OSMX_SUPPLY_BOOST_UMSK                          (~(((1U<<RF_ANA1_OSMX_SUPPLY_BOOST_LEN)-1)<<RF_ANA1_OSMX_SUPPLY_BOOST_POS))
#define RF_ANA1_OSMX_PU_PKDET                                   RF_ANA1_OSMX_PU_PKDET
#define RF_ANA1_OSMX_PU_PKDET_POS                               (12U)
#define RF_ANA1_OSMX_PU_PKDET_LEN                               (1U)
#define RF_ANA1_OSMX_PU_PKDET_MSK                               (((1U<<RF_ANA1_OSMX_PU_PKDET_LEN)-1)<<RF_ANA1_OSMX_PU_PKDET_POS)
#define RF_ANA1_OSMX_PU_PKDET_UMSK                              (~(((1U<<RF_ANA1_OSMX_PU_PKDET_LEN)-1)<<RF_ANA1_OSMX_PU_PKDET_POS))
#define RF_ANA1_OSMX_COMP_OUT                                   RF_ANA1_OSMX_COMP_OUT
#define RF_ANA1_OSMX_COMP_OUT_POS                               (13U)
#define RF_ANA1_OSMX_COMP_OUT_LEN                               (1U)
#define RF_ANA1_OSMX_COMP_OUT_MSK                               (((1U<<RF_ANA1_OSMX_COMP_OUT_LEN)-1)<<RF_ANA1_OSMX_COMP_OUT_POS)
#define RF_ANA1_OSMX_COMP_OUT_UMSK                              (~(((1U<<RF_ANA1_OSMX_COMP_OUT_LEN)-1)<<RF_ANA1_OSMX_COMP_OUT_POS))
#define RF_ANA1_OSMX_PKDET_VREF_BAND                            RF_ANA1_OSMX_PKDET_VREF_BAND
#define RF_ANA1_OSMX_PKDET_VREF_BAND_POS                        (16U)
#define RF_ANA1_OSMX_PKDET_VREF_BAND_LEN                        (1U)
#define RF_ANA1_OSMX_PKDET_VREF_BAND_MSK                        (((1U<<RF_ANA1_OSMX_PKDET_VREF_BAND_LEN)-1)<<RF_ANA1_OSMX_PKDET_VREF_BAND_POS)
#define RF_ANA1_OSMX_PKDET_VREF_BAND_UMSK                       (~(((1U<<RF_ANA1_OSMX_PKDET_VREF_BAND_LEN)-1)<<RF_ANA1_OSMX_PKDET_VREF_BAND_POS))
#define RF_ANA1_OSMX_PKDET_VREF                                 RF_ANA1_OSMX_PKDET_VREF
#define RF_ANA1_OSMX_PKDET_VREF_POS                             (20U)
#define RF_ANA1_OSMX_PKDET_VREF_LEN                             (4U)
#define RF_ANA1_OSMX_PKDET_VREF_MSK                             (((1U<<RF_ANA1_OSMX_PKDET_VREF_LEN)-1)<<RF_ANA1_OSMX_PKDET_VREF_POS)
#define RF_ANA1_OSMX_PKDET_VREF_UMSK                            (~(((1U<<RF_ANA1_OSMX_PKDET_VREF_LEN)-1)<<RF_ANA1_OSMX_PKDET_VREF_POS))
#define RF_ANA1_OSMX_GM_BOOST                                   RF_ANA1_OSMX_GM_BOOST
#define RF_ANA1_OSMX_GM_BOOST_POS                               (24U)
#define RF_ANA1_OSMX_GM_BOOST_LEN                               (2U)
#define RF_ANA1_OSMX_GM_BOOST_MSK                               (((1U<<RF_ANA1_OSMX_GM_BOOST_LEN)-1)<<RF_ANA1_OSMX_GM_BOOST_POS)
#define RF_ANA1_OSMX_GM_BOOST_UMSK                              (~(((1U<<RF_ANA1_OSMX_GM_BOOST_LEN)-1)<<RF_ANA1_OSMX_GM_BOOST_POS))
#define RF_ANA1_OSMX_EN_BIAS                                    RF_ANA1_OSMX_EN_BIAS
#define RF_ANA1_OSMX_EN_BIAS_POS                                (28U)
#define RF_ANA1_OSMX_EN_BIAS_LEN                                (1U)
#define RF_ANA1_OSMX_EN_BIAS_MSK                                (((1U<<RF_ANA1_OSMX_EN_BIAS_LEN)-1)<<RF_ANA1_OSMX_EN_BIAS_POS)
#define RF_ANA1_OSMX_EN_BIAS_UMSK                               (~(((1U<<RF_ANA1_OSMX_EN_BIAS_LEN)-1)<<RF_ANA1_OSMX_EN_BIAS_POS))

/* 0x114 : lobuf */
#define RF_ANA1_LOBUF_OFFSET                                    (0x114)
#define RF_ANA1_TXBUF_5G_BOOST                                  RF_ANA1_TXBUF_5G_BOOST
#define RF_ANA1_TXBUF_5G_BOOST_POS                              (0U)
#define RF_ANA1_TXBUF_5G_BOOST_LEN                              (2U)
#define RF_ANA1_TXBUF_5G_BOOST_MSK                              (((1U<<RF_ANA1_TXBUF_5G_BOOST_LEN)-1)<<RF_ANA1_TXBUF_5G_BOOST_POS)
#define RF_ANA1_TXBUF_5G_BOOST_UMSK                             (~(((1U<<RF_ANA1_TXBUF_5G_BOOST_LEN)-1)<<RF_ANA1_TXBUF_5G_BOOST_POS))
#define RF_ANA1_TXBUF_2G_BOOST                                  RF_ANA1_TXBUF_2G_BOOST
#define RF_ANA1_TXBUF_2G_BOOST_POS                              (2U)
#define RF_ANA1_TXBUF_2G_BOOST_LEN                              (2U)
#define RF_ANA1_TXBUF_2G_BOOST_MSK                              (((1U<<RF_ANA1_TXBUF_2G_BOOST_LEN)-1)<<RF_ANA1_TXBUF_2G_BOOST_POS)
#define RF_ANA1_TXBUF_2G_BOOST_UMSK                             (~(((1U<<RF_ANA1_TXBUF_2G_BOOST_LEN)-1)<<RF_ANA1_TXBUF_2G_BOOST_POS))
#define RF_ANA1_RXBUF_5G_BOOST                                  RF_ANA1_RXBUF_5G_BOOST
#define RF_ANA1_RXBUF_5G_BOOST_POS                              (4U)
#define RF_ANA1_RXBUF_5G_BOOST_LEN                              (2U)
#define RF_ANA1_RXBUF_5G_BOOST_MSK                              (((1U<<RF_ANA1_RXBUF_5G_BOOST_LEN)-1)<<RF_ANA1_RXBUF_5G_BOOST_POS)
#define RF_ANA1_RXBUF_5G_BOOST_UMSK                             (~(((1U<<RF_ANA1_RXBUF_5G_BOOST_LEN)-1)<<RF_ANA1_RXBUF_5G_BOOST_POS))
#define RF_ANA1_RXBUF_2G_BOOST                                  RF_ANA1_RXBUF_2G_BOOST
#define RF_ANA1_RXBUF_2G_BOOST_POS                              (6U)
#define RF_ANA1_RXBUF_2G_BOOST_LEN                              (2U)
#define RF_ANA1_RXBUF_2G_BOOST_MSK                              (((1U<<RF_ANA1_RXBUF_2G_BOOST_LEN)-1)<<RF_ANA1_RXBUF_2G_BOOST_POS)
#define RF_ANA1_RXBUF_2G_BOOST_UMSK                             (~(((1U<<RF_ANA1_RXBUF_2G_BOOST_LEN)-1)<<RF_ANA1_RXBUF_2G_BOOST_POS))
#define RF_ANA1_LOBUF_EN_TXBUF                                  RF_ANA1_LOBUF_EN_TXBUF
#define RF_ANA1_LOBUF_EN_TXBUF_POS                              (8U)
#define RF_ANA1_LOBUF_EN_TXBUF_LEN                              (1U)
#define RF_ANA1_LOBUF_EN_TXBUF_MSK                              (((1U<<RF_ANA1_LOBUF_EN_TXBUF_LEN)-1)<<RF_ANA1_LOBUF_EN_TXBUF_POS)
#define RF_ANA1_LOBUF_EN_TXBUF_UMSK                             (~(((1U<<RF_ANA1_LOBUF_EN_TXBUF_LEN)-1)<<RF_ANA1_LOBUF_EN_TXBUF_POS))
#define RF_ANA1_LOBUF_EN_TXBUF_HW                               RF_ANA1_LOBUF_EN_TXBUF_HW
#define RF_ANA1_LOBUF_EN_TXBUF_HW_POS                           (9U)
#define RF_ANA1_LOBUF_EN_TXBUF_HW_LEN                           (1U)
#define RF_ANA1_LOBUF_EN_TXBUF_HW_MSK                           (((1U<<RF_ANA1_LOBUF_EN_TXBUF_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_TXBUF_HW_POS)
#define RF_ANA1_LOBUF_EN_TXBUF_HW_UMSK                          (~(((1U<<RF_ANA1_LOBUF_EN_TXBUF_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_TXBUF_HW_POS))
#define RF_ANA1_LOBUF_EN_RXBUF                                  RF_ANA1_LOBUF_EN_RXBUF
#define RF_ANA1_LOBUF_EN_RXBUF_POS                              (12U)
#define RF_ANA1_LOBUF_EN_RXBUF_LEN                              (1U)
#define RF_ANA1_LOBUF_EN_RXBUF_MSK                              (((1U<<RF_ANA1_LOBUF_EN_RXBUF_LEN)-1)<<RF_ANA1_LOBUF_EN_RXBUF_POS)
#define RF_ANA1_LOBUF_EN_RXBUF_UMSK                             (~(((1U<<RF_ANA1_LOBUF_EN_RXBUF_LEN)-1)<<RF_ANA1_LOBUF_EN_RXBUF_POS))
#define RF_ANA1_LOBUF_EN_RXBUF_HW                               RF_ANA1_LOBUF_EN_RXBUF_HW
#define RF_ANA1_LOBUF_EN_RXBUF_HW_POS                           (13U)
#define RF_ANA1_LOBUF_EN_RXBUF_HW_LEN                           (1U)
#define RF_ANA1_LOBUF_EN_RXBUF_HW_MSK                           (((1U<<RF_ANA1_LOBUF_EN_RXBUF_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_RXBUF_HW_POS)
#define RF_ANA1_LOBUF_EN_RXBUF_HW_UMSK                          (~(((1U<<RF_ANA1_LOBUF_EN_RXBUF_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_RXBUF_HW_POS))
#define RF_ANA1_LOBUF_EN_IQDIV_TX                               RF_ANA1_LOBUF_EN_IQDIV_TX
#define RF_ANA1_LOBUF_EN_IQDIV_TX_POS                           (16U)
#define RF_ANA1_LOBUF_EN_IQDIV_TX_LEN                           (1U)
#define RF_ANA1_LOBUF_EN_IQDIV_TX_MSK                           (((1U<<RF_ANA1_LOBUF_EN_IQDIV_TX_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_TX_POS)
#define RF_ANA1_LOBUF_EN_IQDIV_TX_UMSK                          (~(((1U<<RF_ANA1_LOBUF_EN_IQDIV_TX_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_TX_POS))
#define RF_ANA1_LOBUF_EN_IQDIV_TX_HW                            RF_ANA1_LOBUF_EN_IQDIV_TX_HW
#define RF_ANA1_LOBUF_EN_IQDIV_TX_HW_POS                        (17U)
#define RF_ANA1_LOBUF_EN_IQDIV_TX_HW_LEN                        (1U)
#define RF_ANA1_LOBUF_EN_IQDIV_TX_HW_MSK                        (((1U<<RF_ANA1_LOBUF_EN_IQDIV_TX_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_TX_HW_POS)
#define RF_ANA1_LOBUF_EN_IQDIV_TX_HW_UMSK                       (~(((1U<<RF_ANA1_LOBUF_EN_IQDIV_TX_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_TX_HW_POS))
#define RF_ANA1_LOBUF_EN_IQDIV_RX                               RF_ANA1_LOBUF_EN_IQDIV_RX
#define RF_ANA1_LOBUF_EN_IQDIV_RX_POS                           (20U)
#define RF_ANA1_LOBUF_EN_IQDIV_RX_LEN                           (1U)
#define RF_ANA1_LOBUF_EN_IQDIV_RX_MSK                           (((1U<<RF_ANA1_LOBUF_EN_IQDIV_RX_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_RX_POS)
#define RF_ANA1_LOBUF_EN_IQDIV_RX_UMSK                          (~(((1U<<RF_ANA1_LOBUF_EN_IQDIV_RX_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_RX_POS))
#define RF_ANA1_LOBUF_EN_IQDIV_RX_HW                            RF_ANA1_LOBUF_EN_IQDIV_RX_HW
#define RF_ANA1_LOBUF_EN_IQDIV_RX_HW_POS                        (21U)
#define RF_ANA1_LOBUF_EN_IQDIV_RX_HW_LEN                        (1U)
#define RF_ANA1_LOBUF_EN_IQDIV_RX_HW_MSK                        (((1U<<RF_ANA1_LOBUF_EN_IQDIV_RX_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_RX_HW_POS)
#define RF_ANA1_LOBUF_EN_IQDIV_RX_HW_UMSK                       (~(((1U<<RF_ANA1_LOBUF_EN_IQDIV_RX_HW_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_RX_HW_POS))
#define RF_ANA1_LOBUF_EN_5G                                     RF_ANA1_LOBUF_EN_5G
#define RF_ANA1_LOBUF_EN_5G_POS                                 (24U)
#define RF_ANA1_LOBUF_EN_5G_LEN                                 (1U)
#define RF_ANA1_LOBUF_EN_5G_MSK                                 (((1U<<RF_ANA1_LOBUF_EN_5G_LEN)-1)<<RF_ANA1_LOBUF_EN_5G_POS)
#define RF_ANA1_LOBUF_EN_5G_UMSK                                (~(((1U<<RF_ANA1_LOBUF_EN_5G_LEN)-1)<<RF_ANA1_LOBUF_EN_5G_POS))
#define RF_ANA1_LOBUF_EN_2G                                     RF_ANA1_LOBUF_EN_2G
#define RF_ANA1_LOBUF_EN_2G_POS                                 (28U)
#define RF_ANA1_LOBUF_EN_2G_LEN                                 (1U)
#define RF_ANA1_LOBUF_EN_2G_MSK                                 (((1U<<RF_ANA1_LOBUF_EN_2G_LEN)-1)<<RF_ANA1_LOBUF_EN_2G_POS)
#define RF_ANA1_LOBUF_EN_2G_UMSK                                (~(((1U<<RF_ANA1_LOBUF_EN_2G_LEN)-1)<<RF_ANA1_LOBUF_EN_2G_POS))
#define RF_ANA1_LOBUF_EN_DPD                                    RF_ANA1_LOBUF_EN_DPD
#define RF_ANA1_LOBUF_EN_DPD_POS                                (29U)
#define RF_ANA1_LOBUF_EN_DPD_LEN                                (1U)
#define RF_ANA1_LOBUF_EN_DPD_MSK                                (((1U<<RF_ANA1_LOBUF_EN_DPD_LEN)-1)<<RF_ANA1_LOBUF_EN_DPD_POS)
#define RF_ANA1_LOBUF_EN_DPD_UMSK                               (~(((1U<<RF_ANA1_LOBUF_EN_DPD_LEN)-1)<<RF_ANA1_LOBUF_EN_DPD_POS))
#define RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX                         RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX
#define RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_POS                     (30U)
#define RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_LEN                     (1U)
#define RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_MSK                     (((1U<<RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_POS)
#define RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_UMSK                    (~(((1U<<RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_LEN)-1)<<RF_ANA1_LOBUF_EN_IQDIV_BOOST_RX_POS))
#define RF_ANA1_LOBUF_VG10_SEL                                  RF_ANA1_LOBUF_VG10_SEL
#define RF_ANA1_LOBUF_VG10_SEL_POS                              (31U)
#define RF_ANA1_LOBUF_VG10_SEL_LEN                              (1U)
#define RF_ANA1_LOBUF_VG10_SEL_MSK                              (((1U<<RF_ANA1_LOBUF_VG10_SEL_LEN)-1)<<RF_ANA1_LOBUF_VG10_SEL_POS)
#define RF_ANA1_LOBUF_VG10_SEL_UMSK                             (~(((1U<<RF_ANA1_LOBUF_VG10_SEL_LEN)-1)<<RF_ANA1_LOBUF_VG10_SEL_POS))

/* 0x118 : lpvco */
#define RF_ANA1_LPVCO_OFFSET                                    (0x118)
#define RF_ANA1_LPVCO_VBIAS_CW                                  RF_ANA1_LPVCO_VBIAS_CW
#define RF_ANA1_LPVCO_VBIAS_CW_POS                              (0U)
#define RF_ANA1_LPVCO_VBIAS_CW_LEN                              (3U)
#define RF_ANA1_LPVCO_VBIAS_CW_MSK                              (((1U<<RF_ANA1_LPVCO_VBIAS_CW_LEN)-1)<<RF_ANA1_LPVCO_VBIAS_CW_POS)
#define RF_ANA1_LPVCO_VBIAS_CW_UMSK                             (~(((1U<<RF_ANA1_LPVCO_VBIAS_CW_LEN)-1)<<RF_ANA1_LPVCO_VBIAS_CW_POS))
#define RF_ANA1_LPVCO_SHT_VBIAS_FLT                             RF_ANA1_LPVCO_SHT_VBIAS_FLT
#define RF_ANA1_LPVCO_SHT_VBIAS_FLT_POS                         (3U)
#define RF_ANA1_LPVCO_SHT_VBIAS_FLT_LEN                         (1U)
#define RF_ANA1_LPVCO_SHT_VBIAS_FLT_MSK                         (((1U<<RF_ANA1_LPVCO_SHT_VBIAS_FLT_LEN)-1)<<RF_ANA1_LPVCO_SHT_VBIAS_FLT_POS)
#define RF_ANA1_LPVCO_SHT_VBIAS_FLT_UMSK                        (~(((1U<<RF_ANA1_LPVCO_SHT_VBIAS_FLT_LEN)-1)<<RF_ANA1_LPVCO_SHT_VBIAS_FLT_POS))
#define RF_ANA1_LPVCO_SHT_IDAC_FLT                              RF_ANA1_LPVCO_SHT_IDAC_FLT
#define RF_ANA1_LPVCO_SHT_IDAC_FLT_POS                          (4U)
#define RF_ANA1_LPVCO_SHT_IDAC_FLT_LEN                          (1U)
#define RF_ANA1_LPVCO_SHT_IDAC_FLT_MSK                          (((1U<<RF_ANA1_LPVCO_SHT_IDAC_FLT_LEN)-1)<<RF_ANA1_LPVCO_SHT_IDAC_FLT_POS)
#define RF_ANA1_LPVCO_SHT_IDAC_FLT_UMSK                         (~(((1U<<RF_ANA1_LPVCO_SHT_IDAC_FLT_LEN)-1)<<RF_ANA1_LPVCO_SHT_IDAC_FLT_POS))
#define RF_ANA1_LPVCO_INC_ACAL_EN                               RF_ANA1_LPVCO_INC_ACAL_EN
#define RF_ANA1_LPVCO_INC_ACAL_EN_POS                           (5U)
#define RF_ANA1_LPVCO_INC_ACAL_EN_LEN                           (1U)
#define RF_ANA1_LPVCO_INC_ACAL_EN_MSK                           (((1U<<RF_ANA1_LPVCO_INC_ACAL_EN_LEN)-1)<<RF_ANA1_LPVCO_INC_ACAL_EN_POS)
#define RF_ANA1_LPVCO_INC_ACAL_EN_UMSK                          (~(((1U<<RF_ANA1_LPVCO_INC_ACAL_EN_LEN)-1)<<RF_ANA1_LPVCO_INC_ACAL_EN_POS))
#define RF_ANA1_LPVCO_IDAC_CW_HW                                RF_ANA1_LPVCO_IDAC_CW_HW
#define RF_ANA1_LPVCO_IDAC_CW_HW_POS                            (6U)
#define RF_ANA1_LPVCO_IDAC_CW_HW_LEN                            (6U)
#define RF_ANA1_LPVCO_IDAC_CW_HW_MSK                            (((1U<<RF_ANA1_LPVCO_IDAC_CW_HW_LEN)-1)<<RF_ANA1_LPVCO_IDAC_CW_HW_POS)
#define RF_ANA1_LPVCO_IDAC_CW_HW_UMSK                           (~(((1U<<RF_ANA1_LPVCO_IDAC_CW_HW_LEN)-1)<<RF_ANA1_LPVCO_IDAC_CW_HW_POS))
#define RF_ANA1_LPVCO_IDAC_CW                                   RF_ANA1_LPVCO_IDAC_CW
#define RF_ANA1_LPVCO_IDAC_CW_POS                               (12U)
#define RF_ANA1_LPVCO_IDAC_CW_LEN                               (6U)
#define RF_ANA1_LPVCO_IDAC_CW_MSK                               (((1U<<RF_ANA1_LPVCO_IDAC_CW_LEN)-1)<<RF_ANA1_LPVCO_IDAC_CW_POS)
#define RF_ANA1_LPVCO_IDAC_CW_UMSK                              (~(((1U<<RF_ANA1_LPVCO_IDAC_CW_LEN)-1)<<RF_ANA1_LPVCO_IDAC_CW_POS))
#define RF_ANA1_LPVCO_COMP_START                                RF_ANA1_LPVCO_COMP_START
#define RF_ANA1_LPVCO_COMP_START_POS                            (20U)
#define RF_ANA1_LPVCO_COMP_START_LEN                            (1U)
#define RF_ANA1_LPVCO_COMP_START_MSK                            (((1U<<RF_ANA1_LPVCO_COMP_START_LEN)-1)<<RF_ANA1_LPVCO_COMP_START_POS)
#define RF_ANA1_LPVCO_COMP_START_UMSK                           (~(((1U<<RF_ANA1_LPVCO_COMP_START_LEN)-1)<<RF_ANA1_LPVCO_COMP_START_POS))
#define RF_ANA1_LPVCO_ACAL_VREF_CW                              RF_ANA1_LPVCO_ACAL_VREF_CW
#define RF_ANA1_LPVCO_ACAL_VREF_CW_POS                          (24U)
#define RF_ANA1_LPVCO_ACAL_VREF_CW_LEN                          (4U)
#define RF_ANA1_LPVCO_ACAL_VREF_CW_MSK                          (((1U<<RF_ANA1_LPVCO_ACAL_VREF_CW_LEN)-1)<<RF_ANA1_LPVCO_ACAL_VREF_CW_POS)
#define RF_ANA1_LPVCO_ACAL_VREF_CW_UMSK                         (~(((1U<<RF_ANA1_LPVCO_ACAL_VREF_CW_LEN)-1)<<RF_ANA1_LPVCO_ACAL_VREF_CW_POS))
#define RF_ANA1_LPVCO_ACAL_EN                                   RF_ANA1_LPVCO_ACAL_EN
#define RF_ANA1_LPVCO_ACAL_EN_POS                               (28U)
#define RF_ANA1_LPVCO_ACAL_EN_LEN                               (1U)
#define RF_ANA1_LPVCO_ACAL_EN_MSK                               (((1U<<RF_ANA1_LPVCO_ACAL_EN_LEN)-1)<<RF_ANA1_LPVCO_ACAL_EN_POS)
#define RF_ANA1_LPVCO_ACAL_EN_UMSK                              (~(((1U<<RF_ANA1_LPVCO_ACAL_EN_LEN)-1)<<RF_ANA1_LPVCO_ACAL_EN_POS))
#define RF_ANA1_LPVCO_ACAL_UD                                   RF_ANA1_LPVCO_ACAL_UD
#define RF_ANA1_LPVCO_ACAL_UD_POS                               (31U)
#define RF_ANA1_LPVCO_ACAL_UD_LEN                               (1U)
#define RF_ANA1_LPVCO_ACAL_UD_MSK                               (((1U<<RF_ANA1_LPVCO_ACAL_UD_LEN)-1)<<RF_ANA1_LPVCO_ACAL_UD_POS)
#define RF_ANA1_LPVCO_ACAL_UD_UMSK                              (~(((1U<<RF_ANA1_LPVCO_ACAL_UD_LEN)-1)<<RF_ANA1_LPVCO_ACAL_UD_POS))

/* 0x11C : lo_resv */
#define RF_ANA1_LO_RESV_OFFSET                                  (0x11C)
#define RF_ANA1_LO_RESV0                                        RF_ANA1_LO_RESV0
#define RF_ANA1_LO_RESV0_POS                                    (0U)
#define RF_ANA1_LO_RESV0_LEN                                    (8U)
#define RF_ANA1_LO_RESV0_MSK                                    (((1U<<RF_ANA1_LO_RESV0_LEN)-1)<<RF_ANA1_LO_RESV0_POS)
#define RF_ANA1_LO_RESV0_UMSK                                   (~(((1U<<RF_ANA1_LO_RESV0_LEN)-1)<<RF_ANA1_LO_RESV0_POS))
#define RF_ANA1_LO_RESV1                                        RF_ANA1_LO_RESV1
#define RF_ANA1_LO_RESV1_POS                                    (8U)
#define RF_ANA1_LO_RESV1_LEN                                    (8U)
#define RF_ANA1_LO_RESV1_MSK                                    (((1U<<RF_ANA1_LO_RESV1_LEN)-1)<<RF_ANA1_LO_RESV1_POS)
#define RF_ANA1_LO_RESV1_UMSK                                   (~(((1U<<RF_ANA1_LO_RESV1_LEN)-1)<<RF_ANA1_LO_RESV1_POS))

/* 0x120 : lo_fbdiv */
#define RF_ANA1_LO_FBDIV_OFFSET                                 (0x120)
#define RF_ANA1_FBDIV_HALF_STEP_EN                              RF_ANA1_FBDIV_HALF_STEP_EN
#define RF_ANA1_FBDIV_HALF_STEP_EN_POS                          (0U)
#define RF_ANA1_FBDIV_HALF_STEP_EN_LEN                          (1U)
#define RF_ANA1_FBDIV_HALF_STEP_EN_MSK                          (((1U<<RF_ANA1_FBDIV_HALF_STEP_EN_LEN)-1)<<RF_ANA1_FBDIV_HALF_STEP_EN_POS)
#define RF_ANA1_FBDIV_HALF_STEP_EN_UMSK                         (~(((1U<<RF_ANA1_FBDIV_HALF_STEP_EN_LEN)-1)<<RF_ANA1_FBDIV_HALF_STEP_EN_POS))
#define RF_ANA1_FBDIV_HALF_STEP_EN_HW                           RF_ANA1_FBDIV_HALF_STEP_EN_HW
#define RF_ANA1_FBDIV_HALF_STEP_EN_HW_POS                       (1U)
#define RF_ANA1_FBDIV_HALF_STEP_EN_HW_LEN                       (1U)
#define RF_ANA1_FBDIV_HALF_STEP_EN_HW_MSK                       (((1U<<RF_ANA1_FBDIV_HALF_STEP_EN_HW_LEN)-1)<<RF_ANA1_FBDIV_HALF_STEP_EN_HW_POS)
#define RF_ANA1_FBDIV_HALF_STEP_EN_HW_UMSK                      (~(((1U<<RF_ANA1_FBDIV_HALF_STEP_EN_HW_LEN)-1)<<RF_ANA1_FBDIV_HALF_STEP_EN_HW_POS))
#define RF_ANA1_FBDIV_RESAMPLE_EN                               RF_ANA1_FBDIV_RESAMPLE_EN
#define RF_ANA1_FBDIV_RESAMPLE_EN_POS                           (4U)
#define RF_ANA1_FBDIV_RESAMPLE_EN_LEN                           (1U)
#define RF_ANA1_FBDIV_RESAMPLE_EN_MSK                           (((1U<<RF_ANA1_FBDIV_RESAMPLE_EN_LEN)-1)<<RF_ANA1_FBDIV_RESAMPLE_EN_POS)
#define RF_ANA1_FBDIV_RESAMPLE_EN_UMSK                          (~(((1U<<RF_ANA1_FBDIV_RESAMPLE_EN_LEN)-1)<<RF_ANA1_FBDIV_RESAMPLE_EN_POS))
#define RF_ANA1_FBDIV_RESAMPLE_EN_HW                            RF_ANA1_FBDIV_RESAMPLE_EN_HW
#define RF_ANA1_FBDIV_RESAMPLE_EN_HW_POS                        (5U)
#define RF_ANA1_FBDIV_RESAMPLE_EN_HW_LEN                        (1U)
#define RF_ANA1_FBDIV_RESAMPLE_EN_HW_MSK                        (((1U<<RF_ANA1_FBDIV_RESAMPLE_EN_HW_LEN)-1)<<RF_ANA1_FBDIV_RESAMPLE_EN_HW_POS)
#define RF_ANA1_FBDIV_RESAMPLE_EN_HW_UMSK                       (~(((1U<<RF_ANA1_FBDIV_RESAMPLE_EN_HW_LEN)-1)<<RF_ANA1_FBDIV_RESAMPLE_EN_HW_POS))
#define RF_ANA1_FBDIV_SEL_PCLK                                  RF_ANA1_FBDIV_SEL_PCLK
#define RF_ANA1_FBDIV_SEL_PCLK_POS                              (8U)
#define RF_ANA1_FBDIV_SEL_PCLK_LEN                              (1U)
#define RF_ANA1_FBDIV_SEL_PCLK_MSK                              (((1U<<RF_ANA1_FBDIV_SEL_PCLK_LEN)-1)<<RF_ANA1_FBDIV_SEL_PCLK_POS)
#define RF_ANA1_FBDIV_SEL_PCLK_UMSK                             (~(((1U<<RF_ANA1_FBDIV_SEL_PCLK_LEN)-1)<<RF_ANA1_FBDIV_SEL_PCLK_POS))
#define RF_ANA1_FBDIV_SEL_PCLK_HW                               RF_ANA1_FBDIV_SEL_PCLK_HW
#define RF_ANA1_FBDIV_SEL_PCLK_HW_POS                           (10U)
#define RF_ANA1_FBDIV_SEL_PCLK_HW_LEN                           (1U)
#define RF_ANA1_FBDIV_SEL_PCLK_HW_MSK                           (((1U<<RF_ANA1_FBDIV_SEL_PCLK_HW_LEN)-1)<<RF_ANA1_FBDIV_SEL_PCLK_HW_POS)
#define RF_ANA1_FBDIV_SEL_PCLK_HW_UMSK                          (~(((1U<<RF_ANA1_FBDIV_SEL_PCLK_HW_LEN)-1)<<RF_ANA1_FBDIV_SEL_PCLK_HW_POS))
#define RF_ANA1_FBDIV_SEL_FFB                                   RF_ANA1_FBDIV_SEL_FFB
#define RF_ANA1_FBDIV_SEL_FFB_POS                               (12U)
#define RF_ANA1_FBDIV_SEL_FFB_LEN                               (1U)
#define RF_ANA1_FBDIV_SEL_FFB_MSK                               (((1U<<RF_ANA1_FBDIV_SEL_FFB_LEN)-1)<<RF_ANA1_FBDIV_SEL_FFB_POS)
#define RF_ANA1_FBDIV_SEL_FFB_UMSK                              (~(((1U<<RF_ANA1_FBDIV_SEL_FFB_LEN)-1)<<RF_ANA1_FBDIV_SEL_FFB_POS))
#define RF_ANA1_FBDIV_SEL_FFB_HW                                RF_ANA1_FBDIV_SEL_FFB_HW
#define RF_ANA1_FBDIV_SEL_FFB_HW_POS                            (14U)
#define RF_ANA1_FBDIV_SEL_FFB_HW_LEN                            (1U)
#define RF_ANA1_FBDIV_SEL_FFB_HW_MSK                            (((1U<<RF_ANA1_FBDIV_SEL_FFB_HW_LEN)-1)<<RF_ANA1_FBDIV_SEL_FFB_HW_POS)
#define RF_ANA1_FBDIV_SEL_FFB_HW_UMSK                           (~(((1U<<RF_ANA1_FBDIV_SEL_FFB_HW_LEN)-1)<<RF_ANA1_FBDIV_SEL_FFB_HW_POS))
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK                             RF_ANA1_FBDIV_SEL_ADPLL_CLK
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_POS                         (16U)
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_LEN                         (1U)
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_MSK                         (((1U<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_POS)
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_UMSK                        (~(((1U<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_POS))
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW                          RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_POS                      (17U)
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_LEN                      (1U)
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_MSK                      (((1U<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_LEN)-1)<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_POS)
#define RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_UMSK                     (~(((1U<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_LEN)-1)<<RF_ANA1_FBDIV_SEL_ADPLL_CLK_HW_POS))

/* 0x124 : lo_tdc */
#define RF_ANA1_LO_TDC_OFFSET                                   (0x124)
#define RF_ANA1_TDC_SLPGEN_CLK_LATCH                            RF_ANA1_TDC_SLPGEN_CLK_LATCH
#define RF_ANA1_TDC_SLPGEN_CLK_LATCH_POS                        (0U)
#define RF_ANA1_TDC_SLPGEN_CLK_LATCH_LEN                        (1U)
#define RF_ANA1_TDC_SLPGEN_CLK_LATCH_MSK                        (((1U<<RF_ANA1_TDC_SLPGEN_CLK_LATCH_LEN)-1)<<RF_ANA1_TDC_SLPGEN_CLK_LATCH_POS)
#define RF_ANA1_TDC_SLPGEN_CLK_LATCH_UMSK                       (~(((1U<<RF_ANA1_TDC_SLPGEN_CLK_LATCH_LEN)-1)<<RF_ANA1_TDC_SLPGEN_CLK_LATCH_POS))
#define RF_ANA1_TDC_SLPGEN_CLK_INV                              RF_ANA1_TDC_SLPGEN_CLK_INV
#define RF_ANA1_TDC_SLPGEN_CLK_INV_POS                          (1U)
#define RF_ANA1_TDC_SLPGEN_CLK_INV_LEN                          (1U)
#define RF_ANA1_TDC_SLPGEN_CLK_INV_MSK                          (((1U<<RF_ANA1_TDC_SLPGEN_CLK_INV_LEN)-1)<<RF_ANA1_TDC_SLPGEN_CLK_INV_POS)
#define RF_ANA1_TDC_SLPGEN_CLK_INV_UMSK                         (~(((1U<<RF_ANA1_TDC_SLPGEN_CLK_INV_LEN)-1)<<RF_ANA1_TDC_SLPGEN_CLK_INV_POS))
#define RF_ANA1_TDC_SLPGEN_VCMSEL                               RF_ANA1_TDC_SLPGEN_VCMSEL
#define RF_ANA1_TDC_SLPGEN_VCMSEL_POS                           (4U)
#define RF_ANA1_TDC_SLPGEN_VCMSEL_LEN                           (2U)
#define RF_ANA1_TDC_SLPGEN_VCMSEL_MSK                           (((1U<<RF_ANA1_TDC_SLPGEN_VCMSEL_LEN)-1)<<RF_ANA1_TDC_SLPGEN_VCMSEL_POS)
#define RF_ANA1_TDC_SLPGEN_VCMSEL_UMSK                          (~(((1U<<RF_ANA1_TDC_SLPGEN_VCMSEL_LEN)-1)<<RF_ANA1_TDC_SLPGEN_VCMSEL_POS))
#define RF_ANA1_TDC_SLPGEN_CSEL                                 RF_ANA1_TDC_SLPGEN_CSEL
#define RF_ANA1_TDC_SLPGEN_CSEL_POS                             (6U)
#define RF_ANA1_TDC_SLPGEN_CSEL_LEN                             (2U)
#define RF_ANA1_TDC_SLPGEN_CSEL_MSK                             (((1U<<RF_ANA1_TDC_SLPGEN_CSEL_LEN)-1)<<RF_ANA1_TDC_SLPGEN_CSEL_POS)
#define RF_ANA1_TDC_SLPGEN_CSEL_UMSK                            (~(((1U<<RF_ANA1_TDC_SLPGEN_CSEL_LEN)-1)<<RF_ANA1_TDC_SLPGEN_CSEL_POS))
#define RF_ANA1_TDC_ADC_VREF_SEL                                RF_ANA1_TDC_ADC_VREF_SEL
#define RF_ANA1_TDC_ADC_VREF_SEL_POS                            (8U)
#define RF_ANA1_TDC_ADC_VREF_SEL_LEN                            (2U)
#define RF_ANA1_TDC_ADC_VREF_SEL_MSK                            (((1U<<RF_ANA1_TDC_ADC_VREF_SEL_LEN)-1)<<RF_ANA1_TDC_ADC_VREF_SEL_POS)
#define RF_ANA1_TDC_ADC_VREF_SEL_UMSK                           (~(((1U<<RF_ANA1_TDC_ADC_VREF_SEL_LEN)-1)<<RF_ANA1_TDC_ADC_VREF_SEL_POS))
#define RF_ANA1_TDC_ADC_VBUF_SEL                                RF_ANA1_TDC_ADC_VBUF_SEL
#define RF_ANA1_TDC_ADC_VBUF_SEL_POS                            (12U)
#define RF_ANA1_TDC_ADC_VBUF_SEL_LEN                            (2U)
#define RF_ANA1_TDC_ADC_VBUF_SEL_MSK                            (((1U<<RF_ANA1_TDC_ADC_VBUF_SEL_LEN)-1)<<RF_ANA1_TDC_ADC_VBUF_SEL_POS)
#define RF_ANA1_TDC_ADC_VBUF_SEL_UMSK                           (~(((1U<<RF_ANA1_TDC_ADC_VBUF_SEL_LEN)-1)<<RF_ANA1_TDC_ADC_VBUF_SEL_POS))
#define RF_ANA1_TDC_ADC_SIGN_SEL                                RF_ANA1_TDC_ADC_SIGN_SEL
#define RF_ANA1_TDC_ADC_SIGN_SEL_POS                            (16U)
#define RF_ANA1_TDC_ADC_SIGN_SEL_LEN                            (1U)
#define RF_ANA1_TDC_ADC_SIGN_SEL_MSK                            (((1U<<RF_ANA1_TDC_ADC_SIGN_SEL_LEN)-1)<<RF_ANA1_TDC_ADC_SIGN_SEL_POS)
#define RF_ANA1_TDC_ADC_SIGN_SEL_UMSK                           (~(((1U<<RF_ANA1_TDC_ADC_SIGN_SEL_LEN)-1)<<RF_ANA1_TDC_ADC_SIGN_SEL_POS))
#define RF_ANA1_TDC_ADC_GT_RM                                   RF_ANA1_TDC_ADC_GT_RM
#define RF_ANA1_TDC_ADC_GT_RM_POS                               (20U)
#define RF_ANA1_TDC_ADC_GT_RM_LEN                               (1U)
#define RF_ANA1_TDC_ADC_GT_RM_MSK                               (((1U<<RF_ANA1_TDC_ADC_GT_RM_LEN)-1)<<RF_ANA1_TDC_ADC_GT_RM_POS)
#define RF_ANA1_TDC_ADC_GT_RM_UMSK                              (~(((1U<<RF_ANA1_TDC_ADC_GT_RM_LEN)-1)<<RF_ANA1_TDC_ADC_GT_RM_POS))
#define RF_ANA1_TDC_ADC_DLY_CTRL                                RF_ANA1_TDC_ADC_DLY_CTRL
#define RF_ANA1_TDC_ADC_DLY_CTRL_POS                            (24U)
#define RF_ANA1_TDC_ADC_DLY_CTRL_LEN                            (2U)
#define RF_ANA1_TDC_ADC_DLY_CTRL_MSK                            (((1U<<RF_ANA1_TDC_ADC_DLY_CTRL_LEN)-1)<<RF_ANA1_TDC_ADC_DLY_CTRL_POS)
#define RF_ANA1_TDC_ADC_DLY_CTRL_UMSK                           (~(((1U<<RF_ANA1_TDC_ADC_DLY_CTRL_LEN)-1)<<RF_ANA1_TDC_ADC_DLY_CTRL_POS))
#define RF_ANA1_TDC_ADC_CLK_INV                                 RF_ANA1_TDC_ADC_CLK_INV
#define RF_ANA1_TDC_ADC_CLK_INV_POS                             (28U)
#define RF_ANA1_TDC_ADC_CLK_INV_LEN                             (1U)
#define RF_ANA1_TDC_ADC_CLK_INV_MSK                             (((1U<<RF_ANA1_TDC_ADC_CLK_INV_LEN)-1)<<RF_ANA1_TDC_ADC_CLK_INV_POS)
#define RF_ANA1_TDC_ADC_CLK_INV_UMSK                            (~(((1U<<RF_ANA1_TDC_ADC_CLK_INV_LEN)-1)<<RF_ANA1_TDC_ADC_CLK_INV_POS))
#define RF_ANA1_TDC_ADC_CLK_EN                                  RF_ANA1_TDC_ADC_CLK_EN
#define RF_ANA1_TDC_ADC_CLK_EN_POS                              (29U)
#define RF_ANA1_TDC_ADC_CLK_EN_LEN                              (1U)
#define RF_ANA1_TDC_ADC_CLK_EN_MSK                              (((1U<<RF_ANA1_TDC_ADC_CLK_EN_LEN)-1)<<RF_ANA1_TDC_ADC_CLK_EN_POS)
#define RF_ANA1_TDC_ADC_CLK_EN_UMSK                             (~(((1U<<RF_ANA1_TDC_ADC_CLK_EN_LEN)-1)<<RF_ANA1_TDC_ADC_CLK_EN_POS))

/* 0x128 : lo_dcomash */
#define RF_ANA1_LO_DCOMASH_OFFSET                               (0x128)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN                            RF_ANA1_DCOMASH_GAIN_COMP_EN
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_POS                        (4U)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_LEN                        (1U)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_MSK                        (((1U<<RF_ANA1_DCOMASH_GAIN_COMP_EN_LEN)-1)<<RF_ANA1_DCOMASH_GAIN_COMP_EN_POS)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_UMSK                       (~(((1U<<RF_ANA1_DCOMASH_GAIN_COMP_EN_LEN)-1)<<RF_ANA1_DCOMASH_GAIN_COMP_EN_POS))
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_HW                         RF_ANA1_DCOMASH_GAIN_COMP_EN_HW
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_POS                     (5U)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_LEN                     (1U)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_MSK                     (((1U<<RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_LEN)-1)<<RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_POS)
#define RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_UMSK                    (~(((1U<<RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_LEN)-1)<<RF_ANA1_DCOMASH_GAIN_COMP_EN_HW_POS))
#define RF_ANA1_DCOMASH_BYPASS                                  RF_ANA1_DCOMASH_BYPASS
#define RF_ANA1_DCOMASH_BYPASS_POS                              (8U)
#define RF_ANA1_DCOMASH_BYPASS_LEN                              (1U)
#define RF_ANA1_DCOMASH_BYPASS_MSK                              (((1U<<RF_ANA1_DCOMASH_BYPASS_LEN)-1)<<RF_ANA1_DCOMASH_BYPASS_POS)
#define RF_ANA1_DCOMASH_BYPASS_UMSK                             (~(((1U<<RF_ANA1_DCOMASH_BYPASS_LEN)-1)<<RF_ANA1_DCOMASH_BYPASS_POS))
#define RF_ANA1_DCOMASH_NDIV_DLY                                RF_ANA1_DCOMASH_NDIV_DLY
#define RF_ANA1_DCOMASH_NDIV_DLY_POS                            (12U)
#define RF_ANA1_DCOMASH_NDIV_DLY_LEN                            (1U)
#define RF_ANA1_DCOMASH_NDIV_DLY_MSK                            (((1U<<RF_ANA1_DCOMASH_NDIV_DLY_LEN)-1)<<RF_ANA1_DCOMASH_NDIV_DLY_POS)
#define RF_ANA1_DCOMASH_NDIV_DLY_UMSK                           (~(((1U<<RF_ANA1_DCOMASH_NDIV_DLY_LEN)-1)<<RF_ANA1_DCOMASH_NDIV_DLY_POS))

/* 0x12C : lo_adpll_top */
#define RF_ANA1_LO_ADPLL_TOP_OFFSET                             (0x12C)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN                      RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN
#define RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_POS                  (0U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_LEN                  (1U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_MSK                  (((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_POS)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_UMSK                 (~(((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_LMS_EN_POS))
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN                          RF_ANA1_ADPLL_FAST_INC_FCAL_EN
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_POS                      (1U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_LEN                      (1U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_MSK                      (((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_POS)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_POS))
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW                       RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_POS                   (2U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_LEN                   (1U)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_MSK                   (((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_POS)
#define RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_UMSK                  (~(((1U<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_FCAL_EN_HW_POS))
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL                    RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_POS                (4U)
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_LEN                (5U)
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_MSK                (((1U<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_POS)
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_UMSK               (~(((1U<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_VAL_POS))
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN                     RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_POS                 (9U)
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_LEN                 (1U)
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_MSK                 (((1U<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_POS)
#define RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_UMSK                (~(((1U<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<RF_ANA1_ADPLL_FAST_INC_DVAR_COMP_EN_POS))
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE                        RF_ANA1_ADPLL_FORCE_LF_FAST_MODE
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_POS                    (12U)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_LEN                    (1U)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_MSK                    (((1U<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_LEN)-1)<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_POS)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_UMSK                   (~(((1U<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_LEN)-1)<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_POS))
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW                     RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_POS                 (13U)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_LEN                 (1U)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_MSK                 (((1U<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_LEN)-1)<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_POS)
#define RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_UMSK                (~(((1U<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_LEN)-1)<<RF_ANA1_ADPLL_FORCE_LF_FAST_MODE_HW_POS))
#define RF_ANA1_ADPLL_FSM_BYPASS                                RF_ANA1_ADPLL_FSM_BYPASS
#define RF_ANA1_ADPLL_FSM_BYPASS_POS                            (16U)
#define RF_ANA1_ADPLL_FSM_BYPASS_LEN                            (1U)
#define RF_ANA1_ADPLL_FSM_BYPASS_MSK                            (((1U<<RF_ANA1_ADPLL_FSM_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_FSM_BYPASS_POS)
#define RF_ANA1_ADPLL_FSM_BYPASS_UMSK                           (~(((1U<<RF_ANA1_ADPLL_FSM_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_FSM_BYPASS_POS))
#define RF_ANA1_ADPLL_FSM_EN                                    RF_ANA1_ADPLL_FSM_EN
#define RF_ANA1_ADPLL_FSM_EN_POS                                (20U)
#define RF_ANA1_ADPLL_FSM_EN_LEN                                (1U)
#define RF_ANA1_ADPLL_FSM_EN_MSK                                (((1U<<RF_ANA1_ADPLL_FSM_EN_LEN)-1)<<RF_ANA1_ADPLL_FSM_EN_POS)
#define RF_ANA1_ADPLL_FSM_EN_UMSK                               (~(((1U<<RF_ANA1_ADPLL_FSM_EN_LEN)-1)<<RF_ANA1_ADPLL_FSM_EN_POS))
#define RF_ANA1_ADPLL_EN                                        RF_ANA1_ADPLL_EN
#define RF_ANA1_ADPLL_EN_POS                                    (24U)
#define RF_ANA1_ADPLL_EN_LEN                                    (1U)
#define RF_ANA1_ADPLL_EN_MSK                                    (((1U<<RF_ANA1_ADPLL_EN_LEN)-1)<<RF_ANA1_ADPLL_EN_POS)
#define RF_ANA1_ADPLL_EN_UMSK                                   (~(((1U<<RF_ANA1_ADPLL_EN_LEN)-1)<<RF_ANA1_ADPLL_EN_POS))
#define RF_ANA1_RSTB_ADPLL                                      RF_ANA1_RSTB_ADPLL
#define RF_ANA1_RSTB_ADPLL_POS                                  (25U)
#define RF_ANA1_RSTB_ADPLL_LEN                                  (1U)
#define RF_ANA1_RSTB_ADPLL_MSK                                  (((1U<<RF_ANA1_RSTB_ADPLL_LEN)-1)<<RF_ANA1_RSTB_ADPLL_POS)
#define RF_ANA1_RSTB_ADPLL_UMSK                                 (~(((1U<<RF_ANA1_RSTB_ADPLL_LEN)-1)<<RF_ANA1_RSTB_ADPLL_POS))
#define RF_ANA1_RSTB_ADPLL_HW                                   RF_ANA1_RSTB_ADPLL_HW
#define RF_ANA1_RSTB_ADPLL_HW_POS                               (26U)
#define RF_ANA1_RSTB_ADPLL_HW_LEN                               (1U)
#define RF_ANA1_RSTB_ADPLL_HW_MSK                               (((1U<<RF_ANA1_RSTB_ADPLL_HW_LEN)-1)<<RF_ANA1_RSTB_ADPLL_HW_POS)
#define RF_ANA1_RSTB_ADPLL_HW_UMSK                              (~(((1U<<RF_ANA1_RSTB_ADPLL_HW_LEN)-1)<<RF_ANA1_RSTB_ADPLL_HW_POS))
#define RF_ANA1_ADPLL_FREF_DIV2_EN                              RF_ANA1_ADPLL_FREF_DIV2_EN
#define RF_ANA1_ADPLL_FREF_DIV2_EN_POS                          (28U)
#define RF_ANA1_ADPLL_FREF_DIV2_EN_LEN                          (1U)
#define RF_ANA1_ADPLL_FREF_DIV2_EN_MSK                          (((1U<<RF_ANA1_ADPLL_FREF_DIV2_EN_LEN)-1)<<RF_ANA1_ADPLL_FREF_DIV2_EN_POS)
#define RF_ANA1_ADPLL_FREF_DIV2_EN_UMSK                         (~(((1U<<RF_ANA1_ADPLL_FREF_DIV2_EN_LEN)-1)<<RF_ANA1_ADPLL_FREF_DIV2_EN_POS))
#define RF_ANA1_ADPLL_FREF_DIV2_EN_HW                           RF_ANA1_ADPLL_FREF_DIV2_EN_HW
#define RF_ANA1_ADPLL_FREF_DIV2_EN_HW_POS                       (29U)
#define RF_ANA1_ADPLL_FREF_DIV2_EN_HW_LEN                       (1U)
#define RF_ANA1_ADPLL_FREF_DIV2_EN_HW_MSK                       (((1U<<RF_ANA1_ADPLL_FREF_DIV2_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_FREF_DIV2_EN_HW_POS)
#define RF_ANA1_ADPLL_FREF_DIV2_EN_HW_UMSK                      (~(((1U<<RF_ANA1_ADPLL_FREF_DIV2_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_FREF_DIV2_EN_HW_POS))
#define RF_ANA1_ADPLL_LP_MODE_EN                                RF_ANA1_ADPLL_LP_MODE_EN
#define RF_ANA1_ADPLL_LP_MODE_EN_POS                            (30U)
#define RF_ANA1_ADPLL_LP_MODE_EN_LEN                            (1U)
#define RF_ANA1_ADPLL_LP_MODE_EN_MSK                            (((1U<<RF_ANA1_ADPLL_LP_MODE_EN_LEN)-1)<<RF_ANA1_ADPLL_LP_MODE_EN_POS)
#define RF_ANA1_ADPLL_LP_MODE_EN_UMSK                           (~(((1U<<RF_ANA1_ADPLL_LP_MODE_EN_LEN)-1)<<RF_ANA1_ADPLL_LP_MODE_EN_POS))
#define RF_ANA1_ADPLL_LP_MODE_EN_HW                             RF_ANA1_ADPLL_LP_MODE_EN_HW
#define RF_ANA1_ADPLL_LP_MODE_EN_HW_POS                         (31U)
#define RF_ANA1_ADPLL_LP_MODE_EN_HW_LEN                         (1U)
#define RF_ANA1_ADPLL_LP_MODE_EN_HW_MSK                         (((1U<<RF_ANA1_ADPLL_LP_MODE_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_LP_MODE_EN_HW_POS)
#define RF_ANA1_ADPLL_LP_MODE_EN_HW_UMSK                        (~(((1U<<RF_ANA1_ADPLL_LP_MODE_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_LP_MODE_EN_HW_POS))

/* 0x130 : lo_adpll_fsm */
#define RF_ANA1_LO_ADPLL_FSM_OFFSET                             (0x130)
#define RF_ANA1_ADPLL_TIMEOUT_CNT                               RF_ANA1_ADPLL_TIMEOUT_CNT
#define RF_ANA1_ADPLL_TIMEOUT_CNT_POS                           (0U)
#define RF_ANA1_ADPLL_TIMEOUT_CNT_LEN                           (1U)
#define RF_ANA1_ADPLL_TIMEOUT_CNT_MSK                           (((1U<<RF_ANA1_ADPLL_TIMEOUT_CNT_LEN)-1)<<RF_ANA1_ADPLL_TIMEOUT_CNT_POS)
#define RF_ANA1_ADPLL_TIMEOUT_CNT_UMSK                          (~(((1U<<RF_ANA1_ADPLL_TIMEOUT_CNT_LEN)-1)<<RF_ANA1_ADPLL_TIMEOUT_CNT_POS))
#define RF_ANA1_ADPLL_TIMEOUT_CNT1                              RF_ANA1_ADPLL_TIMEOUT_CNT1
#define RF_ANA1_ADPLL_TIMEOUT_CNT1_POS                          (1U)
#define RF_ANA1_ADPLL_TIMEOUT_CNT1_LEN                          (1U)
#define RF_ANA1_ADPLL_TIMEOUT_CNT1_MSK                          (((1U<<RF_ANA1_ADPLL_TIMEOUT_CNT1_LEN)-1)<<RF_ANA1_ADPLL_TIMEOUT_CNT1_POS)
#define RF_ANA1_ADPLL_TIMEOUT_CNT1_UMSK                         (~(((1U<<RF_ANA1_ADPLL_TIMEOUT_CNT1_LEN)-1)<<RF_ANA1_ADPLL_TIMEOUT_CNT1_POS))
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL                RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_POS            (2U)
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_LEN            (1U)
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_MSK            (((1U<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_LEN)-1)<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_POS)
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_UMSK           (~(((1U<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_LEN)-1)<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_POS))
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR                    RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_POS                (3U)
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_LEN                (1U)
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_MSK                (((1U<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_LEN)-1)<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_POS)
#define RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_UMSK               (~(((1U<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_LEN)-1)<<RF_ANA1_ADPLL_LO_UNLOCK_INTRPT_CLEAR_POS))
#define RF_ANA1_ADPLL_LO_OPEN                                   RF_ANA1_ADPLL_LO_OPEN
#define RF_ANA1_ADPLL_LO_OPEN_POS                               (4U)
#define RF_ANA1_ADPLL_LO_OPEN_LEN                               (1U)
#define RF_ANA1_ADPLL_LO_OPEN_MSK                               (((1U<<RF_ANA1_ADPLL_LO_OPEN_LEN)-1)<<RF_ANA1_ADPLL_LO_OPEN_POS)
#define RF_ANA1_ADPLL_LO_OPEN_UMSK                              (~(((1U<<RF_ANA1_ADPLL_LO_OPEN_LEN)-1)<<RF_ANA1_ADPLL_LO_OPEN_POS))
#define RF_ANA1_ADPLL_LO_LOCK_SEL                               RF_ANA1_ADPLL_LO_LOCK_SEL
#define RF_ANA1_ADPLL_LO_LOCK_SEL_POS                           (5U)
#define RF_ANA1_ADPLL_LO_LOCK_SEL_LEN                           (1U)
#define RF_ANA1_ADPLL_LO_LOCK_SEL_MSK                           (((1U<<RF_ANA1_ADPLL_LO_LOCK_SEL_LEN)-1)<<RF_ANA1_ADPLL_LO_LOCK_SEL_POS)
#define RF_ANA1_ADPLL_LO_LOCK_SEL_UMSK                          (~(((1U<<RF_ANA1_ADPLL_LO_LOCK_SEL_LEN)-1)<<RF_ANA1_ADPLL_LO_LOCK_SEL_POS))
#define RF_ANA1_ADPLL_INC_FCAL_EN                               RF_ANA1_ADPLL_INC_FCAL_EN
#define RF_ANA1_ADPLL_INC_FCAL_EN_POS                           (6U)
#define RF_ANA1_ADPLL_INC_FCAL_EN_LEN                           (1U)
#define RF_ANA1_ADPLL_INC_FCAL_EN_MSK                           (((1U<<RF_ANA1_ADPLL_INC_FCAL_EN_LEN)-1)<<RF_ANA1_ADPLL_INC_FCAL_EN_POS)
#define RF_ANA1_ADPLL_INC_FCAL_EN_UMSK                          (~(((1U<<RF_ANA1_ADPLL_INC_FCAL_EN_LEN)-1)<<RF_ANA1_ADPLL_INC_FCAL_EN_POS))
#define RF_ANA1_ADPLL_LOCK_FAIL_EN                              RF_ANA1_ADPLL_LOCK_FAIL_EN
#define RF_ANA1_ADPLL_LOCK_FAIL_EN_POS                          (7U)
#define RF_ANA1_ADPLL_LOCK_FAIL_EN_LEN                          (1U)
#define RF_ANA1_ADPLL_LOCK_FAIL_EN_MSK                          (((1U<<RF_ANA1_ADPLL_LOCK_FAIL_EN_LEN)-1)<<RF_ANA1_ADPLL_LOCK_FAIL_EN_POS)
#define RF_ANA1_ADPLL_LOCK_FAIL_EN_UMSK                         (~(((1U<<RF_ANA1_ADPLL_LOCK_FAIL_EN_LEN)-1)<<RF_ANA1_ADPLL_LOCK_FAIL_EN_POS))
#define RF_ANA1_ADPLL_ABNORMAL_DEALED                           RF_ANA1_ADPLL_ABNORMAL_DEALED
#define RF_ANA1_ADPLL_ABNORMAL_DEALED_POS                       (8U)
#define RF_ANA1_ADPLL_ABNORMAL_DEALED_LEN                       (1U)
#define RF_ANA1_ADPLL_ABNORMAL_DEALED_MSK                       (((1U<<RF_ANA1_ADPLL_ABNORMAL_DEALED_LEN)-1)<<RF_ANA1_ADPLL_ABNORMAL_DEALED_POS)
#define RF_ANA1_ADPLL_ABNORMAL_DEALED_UMSK                      (~(((1U<<RF_ANA1_ADPLL_ABNORMAL_DEALED_LEN)-1)<<RF_ANA1_ADPLL_ABNORMAL_DEALED_POS))
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_START                       RF_ANA1_ADPLL_EXT_VCTRL_DET_START
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_START_POS                   (12U)
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_START_LEN                   (1U)
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_START_MSK                   (((1U<<RF_ANA1_ADPLL_EXT_VCTRL_DET_START_LEN)-1)<<RF_ANA1_ADPLL_EXT_VCTRL_DET_START_POS)
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_START_UMSK                  (~(((1U<<RF_ANA1_ADPLL_EXT_VCTRL_DET_START_LEN)-1)<<RF_ANA1_ADPLL_EXT_VCTRL_DET_START_POS))
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_EN                          RF_ANA1_ADPLL_EXT_VCTRL_DET_EN
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_POS                      (13U)
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_LEN                      (1U)
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_MSK                      (((1U<<RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_POS)
#define RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_VCTRL_DET_EN_POS))
#define RF_ANA1_ADPLL_EXT_RST_SPD_DET                           RF_ANA1_ADPLL_EXT_RST_SPD_DET
#define RF_ANA1_ADPLL_EXT_RST_SPD_DET_POS                       (14U)
#define RF_ANA1_ADPLL_EXT_RST_SPD_DET_LEN                       (1U)
#define RF_ANA1_ADPLL_EXT_RST_SPD_DET_MSK                       (((1U<<RF_ANA1_ADPLL_EXT_RST_SPD_DET_LEN)-1)<<RF_ANA1_ADPLL_EXT_RST_SPD_DET_POS)
#define RF_ANA1_ADPLL_EXT_RST_SPD_DET_UMSK                      (~(((1U<<RF_ANA1_ADPLL_EXT_RST_SPD_DET_LEN)-1)<<RF_ANA1_ADPLL_EXT_RST_SPD_DET_POS))
#define RF_ANA1_ADPLL_EXT_RST_COARSE_DET                        RF_ANA1_ADPLL_EXT_RST_COARSE_DET
#define RF_ANA1_ADPLL_EXT_RST_COARSE_DET_POS                    (15U)
#define RF_ANA1_ADPLL_EXT_RST_COARSE_DET_LEN                    (1U)
#define RF_ANA1_ADPLL_EXT_RST_COARSE_DET_MSK                    (((1U<<RF_ANA1_ADPLL_EXT_RST_COARSE_DET_LEN)-1)<<RF_ANA1_ADPLL_EXT_RST_COARSE_DET_POS)
#define RF_ANA1_ADPLL_EXT_RST_COARSE_DET_UMSK                   (~(((1U<<RF_ANA1_ADPLL_EXT_RST_COARSE_DET_LEN)-1)<<RF_ANA1_ADPLL_EXT_RST_COARSE_DET_POS))
#define RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB                        RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB
#define RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_POS                    (16U)
#define RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_LEN                    (1U)
#define RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_MSK                    (((1U<<RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_LEN)-1)<<RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_POS)
#define RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_UMSK                   (~(((1U<<RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_LEN)-1)<<RF_ANA1_ADPLL_EXT_MOMHOLD_LMSENB_POS))
#define RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN                         RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN
#define RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_POS                     (17U)
#define RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_LEN                     (1U)
#define RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_MSK                     (((1U<<RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_POS)
#define RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_UMSK                    (~(((1U<<RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_MOM_UPDATE_EN_POS))
#define RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN                         RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN
#define RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_POS                     (18U)
#define RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_LEN                     (1U)
#define RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_MSK                     (((1U<<RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_POS)
#define RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_UMSK                    (~(((1U<<RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_MOM_SEARCH_EN_POS))
#define RF_ANA1_ADPLL_EXT_LOOP_LOCK                             RF_ANA1_ADPLL_EXT_LOOP_LOCK
#define RF_ANA1_ADPLL_EXT_LOOP_LOCK_POS                         (19U)
#define RF_ANA1_ADPLL_EXT_LOOP_LOCK_LEN                         (1U)
#define RF_ANA1_ADPLL_EXT_LOOP_LOCK_MSK                         (((1U<<RF_ANA1_ADPLL_EXT_LOOP_LOCK_LEN)-1)<<RF_ANA1_ADPLL_EXT_LOOP_LOCK_POS)
#define RF_ANA1_ADPLL_EXT_LOOP_LOCK_UMSK                        (~(((1U<<RF_ANA1_ADPLL_EXT_LOOP_LOCK_LEN)-1)<<RF_ANA1_ADPLL_EXT_LOOP_LOCK_POS))
#define RF_ANA1_ADPLL_EXT_LO_FSM_EN                             RF_ANA1_ADPLL_EXT_LO_FSM_EN
#define RF_ANA1_ADPLL_EXT_LO_FSM_EN_POS                         (20U)
#define RF_ANA1_ADPLL_EXT_LO_FSM_EN_LEN                         (1U)
#define RF_ANA1_ADPLL_EXT_LO_FSM_EN_MSK                         (((1U<<RF_ANA1_ADPLL_EXT_LO_FSM_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_LO_FSM_EN_POS)
#define RF_ANA1_ADPLL_EXT_LO_FSM_EN_UMSK                        (~(((1U<<RF_ANA1_ADPLL_EXT_LO_FSM_EN_LEN)-1)<<RF_ANA1_ADPLL_EXT_LO_FSM_EN_POS))
#define RF_ANA1_ADPLL_EXT_FREQERR_DET_START                     RF_ANA1_ADPLL_EXT_FREQERR_DET_START
#define RF_ANA1_ADPLL_EXT_FREQERR_DET_START_POS                 (21U)
#define RF_ANA1_ADPLL_EXT_FREQERR_DET_START_LEN                 (1U)
#define RF_ANA1_ADPLL_EXT_FREQERR_DET_START_MSK                 (((1U<<RF_ANA1_ADPLL_EXT_FREQERR_DET_START_LEN)-1)<<RF_ANA1_ADPLL_EXT_FREQERR_DET_START_POS)
#define RF_ANA1_ADPLL_EXT_FREQERR_DET_START_UMSK                (~(((1U<<RF_ANA1_ADPLL_EXT_FREQERR_DET_START_LEN)-1)<<RF_ANA1_ADPLL_EXT_FREQERR_DET_START_POS))
#define RF_ANA1_ADPLL_EXT_FCAL_START                            RF_ANA1_ADPLL_EXT_FCAL_START
#define RF_ANA1_ADPLL_EXT_FCAL_START_POS                        (22U)
#define RF_ANA1_ADPLL_EXT_FCAL_START_LEN                        (1U)
#define RF_ANA1_ADPLL_EXT_FCAL_START_MSK                        (((1U<<RF_ANA1_ADPLL_EXT_FCAL_START_LEN)-1)<<RF_ANA1_ADPLL_EXT_FCAL_START_POS)
#define RF_ANA1_ADPLL_EXT_FCAL_START_UMSK                       (~(((1U<<RF_ANA1_ADPLL_EXT_FCAL_START_LEN)-1)<<RF_ANA1_ADPLL_EXT_FCAL_START_POS))
#define RF_ANA1_ADPLL_EXT_FCAL_DONE                             RF_ANA1_ADPLL_EXT_FCAL_DONE
#define RF_ANA1_ADPLL_EXT_FCAL_DONE_POS                         (23U)
#define RF_ANA1_ADPLL_EXT_FCAL_DONE_LEN                         (1U)
#define RF_ANA1_ADPLL_EXT_FCAL_DONE_MSK                         (((1U<<RF_ANA1_ADPLL_EXT_FCAL_DONE_LEN)-1)<<RF_ANA1_ADPLL_EXT_FCAL_DONE_POS)
#define RF_ANA1_ADPLL_EXT_FCAL_DONE_UMSK                        (~(((1U<<RF_ANA1_ADPLL_EXT_FCAL_DONE_LEN)-1)<<RF_ANA1_ADPLL_EXT_FCAL_DONE_POS))

/* 0x134 : lo_adpll_fcal */
#define RF_ANA1_LO_ADPLL_FCAL_OFFSET                            (0x134)
#define RF_ANA1_ADPLL_FCAL_POLARITY                             RF_ANA1_ADPLL_FCAL_POLARITY
#define RF_ANA1_ADPLL_FCAL_POLARITY_POS                         (0U)
#define RF_ANA1_ADPLL_FCAL_POLARITY_LEN                         (1U)
#define RF_ANA1_ADPLL_FCAL_POLARITY_MSK                         (((1U<<RF_ANA1_ADPLL_FCAL_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_FCAL_POLARITY_POS)
#define RF_ANA1_ADPLL_FCAL_POLARITY_UMSK                        (~(((1U<<RF_ANA1_ADPLL_FCAL_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_FCAL_POLARITY_POS))
#define RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT                         RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT
#define RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_POS                     (1U)
#define RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_LEN                     (1U)
#define RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_MSK                     (((1U<<RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_POS)
#define RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_UMSK                    (~(((1U<<RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<RF_ANA1_ADPLL_FCAL_PHAERR_LIMIT_POS))
#define RF_ANA1_ADPLL_FCAL_MOMCAP_EXT                           RF_ANA1_ADPLL_FCAL_MOMCAP_EXT
#define RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_POS                       (4U)
#define RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_LEN                       (9U)
#define RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_MSK                       (((1U<<RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_LEN)-1)<<RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_POS)
#define RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_UMSK                      (~(((1U<<RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_LEN)-1)<<RF_ANA1_ADPLL_FCAL_MOMCAP_EXT_POS))
#define RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT                       RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT
#define RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_POS                   (16U)
#define RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_LEN                   (1U)
#define RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_MSK                   (((1U<<RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_LEN)-1)<<RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_POS)
#define RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_UMSK                  (~(((1U<<RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_LEN)-1)<<RF_ANA1_ADPLL_FCAL_MOM_TOGGLE_CNT_POS))
#define RF_ANA1_ADPLL_FCAL_MODE                                 RF_ANA1_ADPLL_FCAL_MODE
#define RF_ANA1_ADPLL_FCAL_MODE_POS                             (20U)
#define RF_ANA1_ADPLL_FCAL_MODE_LEN                             (2U)
#define RF_ANA1_ADPLL_FCAL_MODE_MSK                             (((1U<<RF_ANA1_ADPLL_FCAL_MODE_LEN)-1)<<RF_ANA1_ADPLL_FCAL_MODE_POS)
#define RF_ANA1_ADPLL_FCAL_MODE_UMSK                            (~(((1U<<RF_ANA1_ADPLL_FCAL_MODE_LEN)-1)<<RF_ANA1_ADPLL_FCAL_MODE_POS))
#define RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN                          RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN
#define RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_POS                      (24U)
#define RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_LEN                      (1U)
#define RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_MSK                      (((1U<<RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_POS)
#define RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<RF_ANA1_ADPLL_FCAL_NDIV_ADJ_EN_POS))
#define RF_ANA1_ADPLL_FCAL_THRESHOLD                            RF_ANA1_ADPLL_FCAL_THRESHOLD
#define RF_ANA1_ADPLL_FCAL_THRESHOLD_POS                        (28U)
#define RF_ANA1_ADPLL_FCAL_THRESHOLD_LEN                        (2U)
#define RF_ANA1_ADPLL_FCAL_THRESHOLD_MSK                        (((1U<<RF_ANA1_ADPLL_FCAL_THRESHOLD_LEN)-1)<<RF_ANA1_ADPLL_FCAL_THRESHOLD_POS)
#define RF_ANA1_ADPLL_FCAL_THRESHOLD_UMSK                       (~(((1U<<RF_ANA1_ADPLL_FCAL_THRESHOLD_LEN)-1)<<RF_ANA1_ADPLL_FCAL_THRESHOLD_POS))
#define RF_ANA1_ADPLL_FCAL_CLK_PERIOD                           RF_ANA1_ADPLL_FCAL_CLK_PERIOD
#define RF_ANA1_ADPLL_FCAL_CLK_PERIOD_POS                       (30U)
#define RF_ANA1_ADPLL_FCAL_CLK_PERIOD_LEN                       (2U)
#define RF_ANA1_ADPLL_FCAL_CLK_PERIOD_MSK                       (((1U<<RF_ANA1_ADPLL_FCAL_CLK_PERIOD_LEN)-1)<<RF_ANA1_ADPLL_FCAL_CLK_PERIOD_POS)
#define RF_ANA1_ADPLL_FCAL_CLK_PERIOD_UMSK                      (~(((1U<<RF_ANA1_ADPLL_FCAL_CLK_PERIOD_LEN)-1)<<RF_ANA1_ADPLL_FCAL_CLK_PERIOD_POS))

/* 0x138 : lo_adpll_spd */
#define RF_ANA1_LO_ADPLL_SPD_OFFSET                             (0x138)
#define RF_ANA1_ADPLL_SPD_THRESHOLD                             RF_ANA1_ADPLL_SPD_THRESHOLD
#define RF_ANA1_ADPLL_SPD_THRESHOLD_POS                         (0U)
#define RF_ANA1_ADPLL_SPD_THRESHOLD_LEN                         (2U)
#define RF_ANA1_ADPLL_SPD_THRESHOLD_MSK                         (((1U<<RF_ANA1_ADPLL_SPD_THRESHOLD_LEN)-1)<<RF_ANA1_ADPLL_SPD_THRESHOLD_POS)
#define RF_ANA1_ADPLL_SPD_THRESHOLD_UMSK                        (~(((1U<<RF_ANA1_ADPLL_SPD_THRESHOLD_LEN)-1)<<RF_ANA1_ADPLL_SPD_THRESHOLD_POS))
#define RF_ANA1_ADPLL_SPD_SIGN_DET_EN                           RF_ANA1_ADPLL_SPD_SIGN_DET_EN
#define RF_ANA1_ADPLL_SPD_SIGN_DET_EN_POS                       (2U)
#define RF_ANA1_ADPLL_SPD_SIGN_DET_EN_LEN                       (1U)
#define RF_ANA1_ADPLL_SPD_SIGN_DET_EN_MSK                       (((1U<<RF_ANA1_ADPLL_SPD_SIGN_DET_EN_LEN)-1)<<RF_ANA1_ADPLL_SPD_SIGN_DET_EN_POS)
#define RF_ANA1_ADPLL_SPD_SIGN_DET_EN_UMSK                      (~(((1U<<RF_ANA1_ADPLL_SPD_SIGN_DET_EN_LEN)-1)<<RF_ANA1_ADPLL_SPD_SIGN_DET_EN_POS))
#define RF_ANA1_ADPLL_SPD_SIGN_DET_CNT                          RF_ANA1_ADPLL_SPD_SIGN_DET_CNT
#define RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_POS                      (3U)
#define RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_LEN                      (2U)
#define RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_MSK                      (((1U<<RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_LEN)-1)<<RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_POS)
#define RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_LEN)-1)<<RF_ANA1_ADPLL_SPD_SIGN_DET_CNT_POS))
#define RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL                     RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL
#define RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_POS                 (5U)
#define RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_LEN                 (1U)
#define RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_MSK                 (((1U<<RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_LEN)-1)<<RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_POS)
#define RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_UMSK                (~(((1U<<RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_LEN)-1)<<RF_ANA1_ADPLL_SPD_OUTRANGE_GATE_SEL_POS))
#define RF_ANA1_ADPLL_SPD_OUTRANGE_DLY                          RF_ANA1_ADPLL_SPD_OUTRANGE_DLY
#define RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_POS                      (6U)
#define RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_LEN                      (2U)
#define RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_MSK                      (((1U<<RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_LEN)-1)<<RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_POS)
#define RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_LEN)-1)<<RF_ANA1_ADPLL_SPD_OUTRANGE_DLY_POS))
#define RF_ANA1_ADPLL_SPD_OFFSET_POLAR                          RF_ANA1_ADPLL_SPD_OFFSET_POLAR
#define RF_ANA1_ADPLL_SPD_OFFSET_POLAR_POS                      (8U)
#define RF_ANA1_ADPLL_SPD_OFFSET_POLAR_LEN                      (1U)
#define RF_ANA1_ADPLL_SPD_OFFSET_POLAR_MSK                      (((1U<<RF_ANA1_ADPLL_SPD_OFFSET_POLAR_LEN)-1)<<RF_ANA1_ADPLL_SPD_OFFSET_POLAR_POS)
#define RF_ANA1_ADPLL_SPD_OFFSET_POLAR_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SPD_OFFSET_POLAR_LEN)-1)<<RF_ANA1_ADPLL_SPD_OFFSET_POLAR_POS))
#define RF_ANA1_ADPLL_SPD_OFFSET_MAG                            RF_ANA1_ADPLL_SPD_OFFSET_MAG
#define RF_ANA1_ADPLL_SPD_OFFSET_MAG_POS                        (9U)
#define RF_ANA1_ADPLL_SPD_OFFSET_MAG_LEN                        (2U)
#define RF_ANA1_ADPLL_SPD_OFFSET_MAG_MSK                        (((1U<<RF_ANA1_ADPLL_SPD_OFFSET_MAG_LEN)-1)<<RF_ANA1_ADPLL_SPD_OFFSET_MAG_POS)
#define RF_ANA1_ADPLL_SPD_OFFSET_MAG_UMSK                       (~(((1U<<RF_ANA1_ADPLL_SPD_OFFSET_MAG_LEN)-1)<<RF_ANA1_ADPLL_SPD_OFFSET_MAG_POS))
#define RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN                          RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN
#define RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_POS                      (11U)
#define RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_LEN                      (1U)
#define RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_MSK                      (((1U<<RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_LEN)-1)<<RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_POS)
#define RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_LEN)-1)<<RF_ANA1_ADPLL_SPD_LMS_SSTP_WIN_POS))
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY1                          RF_ANA1_ADPLL_SPD_INRANGE_DLY1
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY1_POS                      (12U)
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY1_LEN                      (2U)
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY1_MSK                      (((1U<<RF_ANA1_ADPLL_SPD_INRANGE_DLY1_LEN)-1)<<RF_ANA1_ADPLL_SPD_INRANGE_DLY1_POS)
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY1_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SPD_INRANGE_DLY1_LEN)-1)<<RF_ANA1_ADPLL_SPD_INRANGE_DLY1_POS))
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY                           RF_ANA1_ADPLL_SPD_INRANGE_DLY
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY_POS                       (14U)
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY_LEN                       (2U)
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY_MSK                       (((1U<<RF_ANA1_ADPLL_SPD_INRANGE_DLY_LEN)-1)<<RF_ANA1_ADPLL_SPD_INRANGE_DLY_POS)
#define RF_ANA1_ADPLL_SPD_INRANGE_DLY_UMSK                      (~(((1U<<RF_ANA1_ADPLL_SPD_INRANGE_DLY_LEN)-1)<<RF_ANA1_ADPLL_SPD_INRANGE_DLY_POS))
#define RF_ANA1_ADPLL_SPD_INRANGE_CONS                          RF_ANA1_ADPLL_SPD_INRANGE_CONS
#define RF_ANA1_ADPLL_SPD_INRANGE_CONS_POS                      (16U)
#define RF_ANA1_ADPLL_SPD_INRANGE_CONS_LEN                      (2U)
#define RF_ANA1_ADPLL_SPD_INRANGE_CONS_MSK                      (((1U<<RF_ANA1_ADPLL_SPD_INRANGE_CONS_LEN)-1)<<RF_ANA1_ADPLL_SPD_INRANGE_CONS_POS)
#define RF_ANA1_ADPLL_SPD_INRANGE_CONS_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SPD_INRANGE_CONS_LEN)-1)<<RF_ANA1_ADPLL_SPD_INRANGE_CONS_POS))
#define RF_ANA1_ADPLL_SPD_GAIN                                  RF_ANA1_ADPLL_SPD_GAIN
#define RF_ANA1_ADPLL_SPD_GAIN_POS                              (18U)
#define RF_ANA1_ADPLL_SPD_GAIN_LEN                              (2U)
#define RF_ANA1_ADPLL_SPD_GAIN_MSK                              (((1U<<RF_ANA1_ADPLL_SPD_GAIN_LEN)-1)<<RF_ANA1_ADPLL_SPD_GAIN_POS)
#define RF_ANA1_ADPLL_SPD_GAIN_UMSK                             (~(((1U<<RF_ANA1_ADPLL_SPD_GAIN_LEN)-1)<<RF_ANA1_ADPLL_SPD_GAIN_POS))
#define RF_ANA1_ADPLL_COARSE_PHAERR_DLY                         RF_ANA1_ADPLL_COARSE_PHAERR_DLY
#define RF_ANA1_ADPLL_COARSE_PHAERR_DLY_POS                     (20U)
#define RF_ANA1_ADPLL_COARSE_PHAERR_DLY_LEN                     (1U)
#define RF_ANA1_ADPLL_COARSE_PHAERR_DLY_MSK                     (((1U<<RF_ANA1_ADPLL_COARSE_PHAERR_DLY_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PHAERR_DLY_POS)
#define RF_ANA1_ADPLL_COARSE_PHAERR_DLY_UMSK                    (~(((1U<<RF_ANA1_ADPLL_COARSE_PHAERR_DLY_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PHAERR_DLY_POS))
#define RF_ANA1_ADPLL_COARSE_PHAERR_EN                          RF_ANA1_ADPLL_COARSE_PHAERR_EN
#define RF_ANA1_ADPLL_COARSE_PHAERR_EN_POS                      (21U)
#define RF_ANA1_ADPLL_COARSE_PHAERR_EN_LEN                      (1U)
#define RF_ANA1_ADPLL_COARSE_PHAERR_EN_MSK                      (((1U<<RF_ANA1_ADPLL_COARSE_PHAERR_EN_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PHAERR_EN_POS)
#define RF_ANA1_ADPLL_COARSE_PHAERR_EN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_COARSE_PHAERR_EN_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PHAERR_EN_POS))
#define RF_ANA1_ADPLL_COARSE_PATH_TURNOFF                       RF_ANA1_ADPLL_COARSE_PATH_TURNOFF
#define RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_POS                   (22U)
#define RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_LEN                   (2U)
#define RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_MSK                   (((1U<<RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_POS)
#define RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_UMSK                  (~(((1U<<RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PATH_TURNOFF_POS))
#define RF_ANA1_ADPLL_COARSE_PATH_OFFTIME                       RF_ANA1_ADPLL_COARSE_PATH_OFFTIME
#define RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_POS                   (24U)
#define RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_LEN                   (1U)
#define RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_MSK                   (((1U<<RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_POS)
#define RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_UMSK                  (~(((1U<<RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PATH_OFFTIME_POS))
#define RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON                      RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON
#define RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_POS                  (25U)
#define RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_LEN                  (1U)
#define RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_MSK                  (((1U<<RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_POS)
#define RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_UMSK                 (~(((1U<<RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<RF_ANA1_ADPLL_COARSE_PATH_FORCE_ON_POS))
#define RF_ANA1_ADPLL_COARSE_INRANGE_CONS                       RF_ANA1_ADPLL_COARSE_INRANGE_CONS
#define RF_ANA1_ADPLL_COARSE_INRANGE_CONS_POS                   (28U)
#define RF_ANA1_ADPLL_COARSE_INRANGE_CONS_LEN                   (2U)
#define RF_ANA1_ADPLL_COARSE_INRANGE_CONS_MSK                   (((1U<<RF_ANA1_ADPLL_COARSE_INRANGE_CONS_LEN)-1)<<RF_ANA1_ADPLL_COARSE_INRANGE_CONS_POS)
#define RF_ANA1_ADPLL_COARSE_INRANGE_CONS_UMSK                  (~(((1U<<RF_ANA1_ADPLL_COARSE_INRANGE_CONS_LEN)-1)<<RF_ANA1_ADPLL_COARSE_INRANGE_CONS_POS))
#define RF_ANA1_ADPLL_COARSE_GAIN                               RF_ANA1_ADPLL_COARSE_GAIN
#define RF_ANA1_ADPLL_COARSE_GAIN_POS                           (30U)
#define RF_ANA1_ADPLL_COARSE_GAIN_LEN                           (2U)
#define RF_ANA1_ADPLL_COARSE_GAIN_MSK                           (((1U<<RF_ANA1_ADPLL_COARSE_GAIN_LEN)-1)<<RF_ANA1_ADPLL_COARSE_GAIN_POS)
#define RF_ANA1_ADPLL_COARSE_GAIN_UMSK                          (~(((1U<<RF_ANA1_ADPLL_COARSE_GAIN_LEN)-1)<<RF_ANA1_ADPLL_COARSE_GAIN_POS))

/* 0x13C : lo_adpll_lpf */
#define RF_ANA1_LO_ADPLL_LPF_OFFSET                             (0x13C)
#define RF_ANA1_ADPLL_LF_FP3                                    RF_ANA1_ADPLL_LF_FP3
#define RF_ANA1_ADPLL_LF_FP3_POS                                (0U)
#define RF_ANA1_ADPLL_LF_FP3_LEN                                (2U)
#define RF_ANA1_ADPLL_LF_FP3_MSK                                (((1U<<RF_ANA1_ADPLL_LF_FP3_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_POS)
#define RF_ANA1_ADPLL_LF_FP3_UMSK                               (~(((1U<<RF_ANA1_ADPLL_LF_FP3_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_POS))
#define RF_ANA1_ADPLL_LF_BETA_FAST                              RF_ANA1_ADPLL_LF_BETA_FAST
#define RF_ANA1_ADPLL_LF_BETA_FAST_POS                          (4U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_LEN                          (1U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_MSK                          (((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_POS)
#define RF_ANA1_ADPLL_LF_BETA_FAST_UMSK                         (~(((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_POS))
#define RF_ANA1_ADPLL_LF_BETA_EXP                               RF_ANA1_ADPLL_LF_BETA_EXP
#define RF_ANA1_ADPLL_LF_BETA_EXP_POS                           (5U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_LEN                           (3U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_MSK                           (((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_POS)
#define RF_ANA1_ADPLL_LF_BETA_EXP_UMSK                          (~(((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_POS))
#define RF_ANA1_ADPLL_LF_BETA_BASE                              RF_ANA1_ADPLL_LF_BETA_BASE
#define RF_ANA1_ADPLL_LF_BETA_BASE_POS                          (8U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_LEN                          (2U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_MSK                          (((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_POS)
#define RF_ANA1_ADPLL_LF_BETA_BASE_UMSK                         (~(((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_POS))
#define RF_ANA1_ADPLL_LF_AVG_EN                                 RF_ANA1_ADPLL_LF_AVG_EN
#define RF_ANA1_ADPLL_LF_AVG_EN_POS                             (12U)
#define RF_ANA1_ADPLL_LF_AVG_EN_LEN                             (1U)
#define RF_ANA1_ADPLL_LF_AVG_EN_MSK                             (((1U<<RF_ANA1_ADPLL_LF_AVG_EN_LEN)-1)<<RF_ANA1_ADPLL_LF_AVG_EN_POS)
#define RF_ANA1_ADPLL_LF_AVG_EN_UMSK                            (~(((1U<<RF_ANA1_ADPLL_LF_AVG_EN_LEN)-1)<<RF_ANA1_ADPLL_LF_AVG_EN_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_FAST                             RF_ANA1_ADPLL_LF_ALPHA_FAST
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_POS                         (16U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_LEN                         (2U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_MSK                         (((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_UMSK                        (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_EXP                              RF_ANA1_ADPLL_LF_ALPHA_EXP
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_POS                          (20U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_LEN                          (3U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_MSK                          (((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_UMSK                         (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_BASE                             RF_ANA1_ADPLL_LF_ALPHA_BASE
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_POS                         (24U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_LEN                         (1U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_MSK                         (((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_UMSK                        (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_POS))

/* 0x140 : lo_adpll_lpf_tx */
#define RF_ANA1_LO_ADPLL_LPF_TX_OFFSET                          (0x140)
#define RF_ANA1_ADPLL_LF_FP3_TX                                 RF_ANA1_ADPLL_LF_FP3_TX
#define RF_ANA1_ADPLL_LF_FP3_TX_POS                             (0U)
#define RF_ANA1_ADPLL_LF_FP3_TX_LEN                             (2U)
#define RF_ANA1_ADPLL_LF_FP3_TX_MSK                             (((1U<<RF_ANA1_ADPLL_LF_FP3_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_TX_POS)
#define RF_ANA1_ADPLL_LF_FP3_TX_UMSK                            (~(((1U<<RF_ANA1_ADPLL_LF_FP3_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_TX_POS))
#define RF_ANA1_ADPLL_LF_BETA_FAST_TX                           RF_ANA1_ADPLL_LF_BETA_FAST_TX
#define RF_ANA1_ADPLL_LF_BETA_FAST_TX_POS                       (4U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_TX_LEN                       (1U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_TX_MSK                       (((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_TX_POS)
#define RF_ANA1_ADPLL_LF_BETA_FAST_TX_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_TX_POS))
#define RF_ANA1_ADPLL_LF_BETA_EXP_TX                            RF_ANA1_ADPLL_LF_BETA_EXP_TX
#define RF_ANA1_ADPLL_LF_BETA_EXP_TX_POS                        (5U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_TX_LEN                        (3U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_TX_MSK                        (((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_TX_POS)
#define RF_ANA1_ADPLL_LF_BETA_EXP_TX_UMSK                       (~(((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_TX_POS))
#define RF_ANA1_ADPLL_LF_BETA_BASE_TX                           RF_ANA1_ADPLL_LF_BETA_BASE_TX
#define RF_ANA1_ADPLL_LF_BETA_BASE_TX_POS                       (8U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_TX_LEN                       (2U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_TX_MSK                       (((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_TX_POS)
#define RF_ANA1_ADPLL_LF_BETA_BASE_TX_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_TX_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_TX                          RF_ANA1_ADPLL_LF_ALPHA_FAST_TX
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_POS                      (16U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_LEN                      (2U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_MSK                      (((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_TX_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_TX                           RF_ANA1_ADPLL_LF_ALPHA_EXP_TX
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_POS                       (20U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_LEN                       (3U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_MSK                       (((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_TX_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_TX                          RF_ANA1_ADPLL_LF_ALPHA_BASE_TX
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_POS                      (24U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_LEN                      (1U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_MSK                      (((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_TX_POS))

/* 0x144 : lo_adpll_lpf_rx */
#define RF_ANA1_LO_ADPLL_LPF_RX_OFFSET                          (0x144)
#define RF_ANA1_ADPLL_LF_FP3_RX                                 RF_ANA1_ADPLL_LF_FP3_RX
#define RF_ANA1_ADPLL_LF_FP3_RX_POS                             (0U)
#define RF_ANA1_ADPLL_LF_FP3_RX_LEN                             (2U)
#define RF_ANA1_ADPLL_LF_FP3_RX_MSK                             (((1U<<RF_ANA1_ADPLL_LF_FP3_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_RX_POS)
#define RF_ANA1_ADPLL_LF_FP3_RX_UMSK                            (~(((1U<<RF_ANA1_ADPLL_LF_FP3_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_RX_POS))
#define RF_ANA1_ADPLL_LF_BETA_FAST_RX                           RF_ANA1_ADPLL_LF_BETA_FAST_RX
#define RF_ANA1_ADPLL_LF_BETA_FAST_RX_POS                       (4U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_RX_LEN                       (1U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_RX_MSK                       (((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_RX_POS)
#define RF_ANA1_ADPLL_LF_BETA_FAST_RX_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_RX_POS))
#define RF_ANA1_ADPLL_LF_BETA_EXP_RX                            RF_ANA1_ADPLL_LF_BETA_EXP_RX
#define RF_ANA1_ADPLL_LF_BETA_EXP_RX_POS                        (5U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_RX_LEN                        (3U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_RX_MSK                        (((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_RX_POS)
#define RF_ANA1_ADPLL_LF_BETA_EXP_RX_UMSK                       (~(((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_RX_POS))
#define RF_ANA1_ADPLL_LF_BETA_BASE_RX                           RF_ANA1_ADPLL_LF_BETA_BASE_RX
#define RF_ANA1_ADPLL_LF_BETA_BASE_RX_POS                       (8U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_RX_LEN                       (2U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_RX_MSK                       (((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_RX_POS)
#define RF_ANA1_ADPLL_LF_BETA_BASE_RX_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_RX_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_RX                          RF_ANA1_ADPLL_LF_ALPHA_FAST_RX
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_POS                      (16U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_LEN                      (2U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_MSK                      (((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_RX_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_RX                           RF_ANA1_ADPLL_LF_ALPHA_EXP_RX
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_POS                       (20U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_LEN                       (3U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_MSK                       (((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_RX_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_RX                          RF_ANA1_ADPLL_LF_ALPHA_BASE_RX
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_POS                      (24U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_LEN                      (1U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_MSK                      (((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_RX_POS))

/* 0x148 : lo_adpll_lpf_hw */
#define RF_ANA1_LO_ADPLL_LPF_HW_OFFSET                          (0x148)
#define RF_ANA1_ADPLL_LF_FP3_HW                                 RF_ANA1_ADPLL_LF_FP3_HW
#define RF_ANA1_ADPLL_LF_FP3_HW_POS                             (0U)
#define RF_ANA1_ADPLL_LF_FP3_HW_LEN                             (2U)
#define RF_ANA1_ADPLL_LF_FP3_HW_MSK                             (((1U<<RF_ANA1_ADPLL_LF_FP3_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_HW_POS)
#define RF_ANA1_ADPLL_LF_FP3_HW_UMSK                            (~(((1U<<RF_ANA1_ADPLL_LF_FP3_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_FP3_HW_POS))
#define RF_ANA1_ADPLL_LF_BETA_FAST_HW                           RF_ANA1_ADPLL_LF_BETA_FAST_HW
#define RF_ANA1_ADPLL_LF_BETA_FAST_HW_POS                       (4U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_HW_LEN                       (1U)
#define RF_ANA1_ADPLL_LF_BETA_FAST_HW_MSK                       (((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_HW_POS)
#define RF_ANA1_ADPLL_LF_BETA_FAST_HW_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_BETA_FAST_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_FAST_HW_POS))
#define RF_ANA1_ADPLL_LF_BETA_EXP_HW                            RF_ANA1_ADPLL_LF_BETA_EXP_HW
#define RF_ANA1_ADPLL_LF_BETA_EXP_HW_POS                        (5U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_HW_LEN                        (3U)
#define RF_ANA1_ADPLL_LF_BETA_EXP_HW_MSK                        (((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_HW_POS)
#define RF_ANA1_ADPLL_LF_BETA_EXP_HW_UMSK                       (~(((1U<<RF_ANA1_ADPLL_LF_BETA_EXP_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_EXP_HW_POS))
#define RF_ANA1_ADPLL_LF_BETA_BASE_HW                           RF_ANA1_ADPLL_LF_BETA_BASE_HW
#define RF_ANA1_ADPLL_LF_BETA_BASE_HW_POS                       (8U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_HW_LEN                       (2U)
#define RF_ANA1_ADPLL_LF_BETA_BASE_HW_MSK                       (((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_HW_POS)
#define RF_ANA1_ADPLL_LF_BETA_BASE_HW_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_BETA_BASE_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_BETA_BASE_HW_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_HW                          RF_ANA1_ADPLL_LF_ALPHA_FAST_HW
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_POS                      (16U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_LEN                      (2U)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_MSK                      (((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_FAST_HW_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_HW                           RF_ANA1_ADPLL_LF_ALPHA_EXP_HW
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_POS                       (20U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_LEN                       (3U)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_MSK                       (((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_EXP_HW_POS))
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_HW                          RF_ANA1_ADPLL_LF_ALPHA_BASE_HW
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_POS                      (24U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_LEN                      (1U)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_MSK                      (((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_POS)
#define RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_LEN)-1)<<RF_ANA1_ADPLL_LF_ALPHA_BASE_HW_POS))

/* 0x14C : lo_adpll_vctrl */
#define RF_ANA1_LO_ADPLL_VCTRL_OFFSET                           (0x14C)
#define RF_ANA1_ADPLL_MOM_UPDATE_PERIOD                         RF_ANA1_ADPLL_MOM_UPDATE_PERIOD
#define RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_POS                     (0U)
#define RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_LEN                     (2U)
#define RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_MSK                     (((1U<<RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_LEN)-1)<<RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_POS)
#define RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_UMSK                    (~(((1U<<RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_LEN)-1)<<RF_ANA1_ADPLL_MOM_UPDATE_PERIOD_POS))
#define RF_ANA1_ADPLL_MOM_FORCE_HOLD                            RF_ANA1_ADPLL_MOM_FORCE_HOLD
#define RF_ANA1_ADPLL_MOM_FORCE_HOLD_POS                        (2U)
#define RF_ANA1_ADPLL_MOM_FORCE_HOLD_LEN                        (1U)
#define RF_ANA1_ADPLL_MOM_FORCE_HOLD_MSK                        (((1U<<RF_ANA1_ADPLL_MOM_FORCE_HOLD_LEN)-1)<<RF_ANA1_ADPLL_MOM_FORCE_HOLD_POS)
#define RF_ANA1_ADPLL_MOM_FORCE_HOLD_UMSK                       (~(((1U<<RF_ANA1_ADPLL_MOM_FORCE_HOLD_LEN)-1)<<RF_ANA1_ADPLL_MOM_FORCE_HOLD_POS))
#define RF_ANA1_ADPLL_MOM_CODE_BYPASS                           RF_ANA1_ADPLL_MOM_CODE_BYPASS
#define RF_ANA1_ADPLL_MOM_CODE_BYPASS_POS                       (3U)
#define RF_ANA1_ADPLL_MOM_CODE_BYPASS_LEN                       (1U)
#define RF_ANA1_ADPLL_MOM_CODE_BYPASS_MSK                       (((1U<<RF_ANA1_ADPLL_MOM_CODE_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_MOM_CODE_BYPASS_POS)
#define RF_ANA1_ADPLL_MOM_CODE_BYPASS_UMSK                      (~(((1U<<RF_ANA1_ADPLL_MOM_CODE_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_MOM_CODE_BYPASS_POS))
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN                        RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_POS                    (4U)
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_LEN                    (1U)
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_MSK                    (((1U<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_POS)
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_UMSK                   (~(((1U<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_EN_POS))
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT                           RF_ANA1_ADPLL_VCTRL_RANGE_EXT
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_POS                       (5U)
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_LEN                       (2U)
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_MSK                       (((1U<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_POS)
#define RF_ANA1_ADPLL_VCTRL_RANGE_EXT_UMSK                      (~(((1U<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_RANGE_EXT_POS))
#define RF_ANA1_ADPLL_VCTRL_MONI_WIN                            RF_ANA1_ADPLL_VCTRL_MONI_WIN
#define RF_ANA1_ADPLL_VCTRL_MONI_WIN_POS                        (8U)
#define RF_ANA1_ADPLL_VCTRL_MONI_WIN_LEN                        (1U)
#define RF_ANA1_ADPLL_VCTRL_MONI_WIN_MSK                        (((1U<<RF_ANA1_ADPLL_VCTRL_MONI_WIN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_MONI_WIN_POS)
#define RF_ANA1_ADPLL_VCTRL_MONI_WIN_UMSK                       (~(((1U<<RF_ANA1_ADPLL_VCTRL_MONI_WIN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_MONI_WIN_POS))
#define RF_ANA1_ADPLL_VCTRL_LOCK_WIN                            RF_ANA1_ADPLL_VCTRL_LOCK_WIN
#define RF_ANA1_ADPLL_VCTRL_LOCK_WIN_POS                        (9U)
#define RF_ANA1_ADPLL_VCTRL_LOCK_WIN_LEN                        (1U)
#define RF_ANA1_ADPLL_VCTRL_LOCK_WIN_MSK                        (((1U<<RF_ANA1_ADPLL_VCTRL_LOCK_WIN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_LOCK_WIN_POS)
#define RF_ANA1_ADPLL_VCTRL_LOCK_WIN_UMSK                       (~(((1U<<RF_ANA1_ADPLL_VCTRL_LOCK_WIN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_LOCK_WIN_POS))
#define RF_ANA1_ADPLL_VCTRL_DET_CONS_EN                         RF_ANA1_ADPLL_VCTRL_DET_CONS_EN
#define RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_POS                     (12U)
#define RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_LEN                     (1U)
#define RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_MSK                     (((1U<<RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_POS)
#define RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_UMSK                    (~(((1U<<RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_LEN)-1)<<RF_ANA1_ADPLL_VCTRL_DET_CONS_EN_POS))
#define RF_ANA1_ADPLL_LP_POLARITY                               RF_ANA1_ADPLL_LP_POLARITY
#define RF_ANA1_ADPLL_LP_POLARITY_POS                           (16U)
#define RF_ANA1_ADPLL_LP_POLARITY_LEN                           (1U)
#define RF_ANA1_ADPLL_LP_POLARITY_MSK                           (((1U<<RF_ANA1_ADPLL_LP_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_LP_POLARITY_POS)
#define RF_ANA1_ADPLL_LP_POLARITY_UMSK                          (~(((1U<<RF_ANA1_ADPLL_LP_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_LP_POLARITY_POS))
#define RF_ANA1_ADPLL_LP_MOM_POLARITY                           RF_ANA1_ADPLL_LP_MOM_POLARITY
#define RF_ANA1_ADPLL_LP_MOM_POLARITY_POS                       (20U)
#define RF_ANA1_ADPLL_LP_MOM_POLARITY_LEN                       (1U)
#define RF_ANA1_ADPLL_LP_MOM_POLARITY_MSK                       (((1U<<RF_ANA1_ADPLL_LP_MOM_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_LP_MOM_POLARITY_POS)
#define RF_ANA1_ADPLL_LP_MOM_POLARITY_UMSK                      (~(((1U<<RF_ANA1_ADPLL_LP_MOM_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_LP_MOM_POLARITY_POS))
#define RF_ANA1_ADPLL_LF_MSB_EXT                                RF_ANA1_ADPLL_LF_MSB_EXT
#define RF_ANA1_ADPLL_LF_MSB_EXT_POS                            (24U)
#define RF_ANA1_ADPLL_LF_MSB_EXT_LEN                            (7U)
#define RF_ANA1_ADPLL_LF_MSB_EXT_MSK                            (((1U<<RF_ANA1_ADPLL_LF_MSB_EXT_LEN)-1)<<RF_ANA1_ADPLL_LF_MSB_EXT_POS)
#define RF_ANA1_ADPLL_LF_MSB_EXT_UMSK                           (~(((1U<<RF_ANA1_ADPLL_LF_MSB_EXT_LEN)-1)<<RF_ANA1_ADPLL_LF_MSB_EXT_POS))

/* 0x150 : lo_adpll_sdm */
#define RF_ANA1_LO_ADPLL_SDM_OFFSET                             (0x150)
#define RF_ANA1_ADPLL_SDM_IN_HW                                 RF_ANA1_ADPLL_SDM_IN_HW
#define RF_ANA1_ADPLL_SDM_IN_HW_POS                             (0U)
#define RF_ANA1_ADPLL_SDM_IN_HW_LEN                             (30U)
#define RF_ANA1_ADPLL_SDM_IN_HW_MSK                             (((1U<<RF_ANA1_ADPLL_SDM_IN_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_IN_HW_POS)
#define RF_ANA1_ADPLL_SDM_IN_HW_UMSK                            (~(((1U<<RF_ANA1_ADPLL_SDM_IN_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_IN_HW_POS))

/* 0x154 : lo_adpll_sdm1 */
#define RF_ANA1_LO_ADPLL_SDM1_OFFSET                            (0x154)
#define RF_ANA1_ADPLL_SDM_IN                                    RF_ANA1_ADPLL_SDM_IN
#define RF_ANA1_ADPLL_SDM_IN_POS                                (0U)
#define RF_ANA1_ADPLL_SDM_IN_LEN                                (30U)
#define RF_ANA1_ADPLL_SDM_IN_MSK                                (((1U<<RF_ANA1_ADPLL_SDM_IN_LEN)-1)<<RF_ANA1_ADPLL_SDM_IN_POS)
#define RF_ANA1_ADPLL_SDM_IN_UMSK                               (~(((1U<<RF_ANA1_ADPLL_SDM_IN_LEN)-1)<<RF_ANA1_ADPLL_SDM_IN_POS))

/* 0x158 : lo_adpll_sdm2 */
#define RF_ANA1_LO_ADPLL_SDM2_OFFSET                            (0x158)
#define RF_ANA1_ADPLL_SDM_OUT_DLY                               RF_ANA1_ADPLL_SDM_OUT_DLY
#define RF_ANA1_ADPLL_SDM_OUT_DLY_POS                           (0U)
#define RF_ANA1_ADPLL_SDM_OUT_DLY_LEN                           (2U)
#define RF_ANA1_ADPLL_SDM_OUT_DLY_MSK                           (((1U<<RF_ANA1_ADPLL_SDM_OUT_DLY_LEN)-1)<<RF_ANA1_ADPLL_SDM_OUT_DLY_POS)
#define RF_ANA1_ADPLL_SDM_OUT_DLY_UMSK                          (~(((1U<<RF_ANA1_ADPLL_SDM_OUT_DLY_LEN)-1)<<RF_ANA1_ADPLL_SDM_OUT_DLY_POS))
#define RF_ANA1_ADPLL_SDM_SIG_DITH                              RF_ANA1_ADPLL_SDM_SIG_DITH
#define RF_ANA1_ADPLL_SDM_SIG_DITH_POS                          (4U)
#define RF_ANA1_ADPLL_SDM_SIG_DITH_LEN                          (2U)
#define RF_ANA1_ADPLL_SDM_SIG_DITH_MSK                          (((1U<<RF_ANA1_ADPLL_SDM_SIG_DITH_LEN)-1)<<RF_ANA1_ADPLL_SDM_SIG_DITH_POS)
#define RF_ANA1_ADPLL_SDM_SIG_DITH_UMSK                         (~(((1U<<RF_ANA1_ADPLL_SDM_SIG_DITH_LEN)-1)<<RF_ANA1_ADPLL_SDM_SIG_DITH_POS))
#define RF_ANA1_ADPLL_SDM_ORDER                                 RF_ANA1_ADPLL_SDM_ORDER
#define RF_ANA1_ADPLL_SDM_ORDER_POS                             (8U)
#define RF_ANA1_ADPLL_SDM_ORDER_LEN                             (1U)
#define RF_ANA1_ADPLL_SDM_ORDER_MSK                             (((1U<<RF_ANA1_ADPLL_SDM_ORDER_LEN)-1)<<RF_ANA1_ADPLL_SDM_ORDER_POS)
#define RF_ANA1_ADPLL_SDM_ORDER_UMSK                            (~(((1U<<RF_ANA1_ADPLL_SDM_ORDER_LEN)-1)<<RF_ANA1_ADPLL_SDM_ORDER_POS))
#define RF_ANA1_ADPLL_SDM_NOI_DITH                              RF_ANA1_ADPLL_SDM_NOI_DITH
#define RF_ANA1_ADPLL_SDM_NOI_DITH_POS                          (12U)
#define RF_ANA1_ADPLL_SDM_NOI_DITH_LEN                          (2U)
#define RF_ANA1_ADPLL_SDM_NOI_DITH_MSK                          (((1U<<RF_ANA1_ADPLL_SDM_NOI_DITH_LEN)-1)<<RF_ANA1_ADPLL_SDM_NOI_DITH_POS)
#define RF_ANA1_ADPLL_SDM_NOI_DITH_UMSK                         (~(((1U<<RF_ANA1_ADPLL_SDM_NOI_DITH_LEN)-1)<<RF_ANA1_ADPLL_SDM_NOI_DITH_POS))
#define RF_ANA1_ADPLL_SDM_DITH_PRBS_EN                          RF_ANA1_ADPLL_SDM_DITH_PRBS_EN
#define RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_POS                      (16U)
#define RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_LEN                      (1U)
#define RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_MSK                      (((1U<<RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_POS)
#define RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_PRBS_EN_POS))
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN                         RF_ANA1_ADPLL_SDM_DITH_FORCE_EN
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_POS                     (20U)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_LEN                     (1U)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_MSK                     (((1U<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_POS)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_UMSK                    (~(((1U<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_POS))
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW                      RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_POS                  (21U)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_LEN                  (1U)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_MSK                  (((1U<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_POS)
#define RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_UMSK                 (~(((1U<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_FORCE_EN_HW_POS))
#define RF_ANA1_ADPLL_SDM_DITH_EN                               RF_ANA1_ADPLL_SDM_DITH_EN
#define RF_ANA1_ADPLL_SDM_DITH_EN_POS                           (24U)
#define RF_ANA1_ADPLL_SDM_DITH_EN_LEN                           (1U)
#define RF_ANA1_ADPLL_SDM_DITH_EN_MSK                           (((1U<<RF_ANA1_ADPLL_SDM_DITH_EN_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_EN_POS)
#define RF_ANA1_ADPLL_SDM_DITH_EN_UMSK                          (~(((1U<<RF_ANA1_ADPLL_SDM_DITH_EN_LEN)-1)<<RF_ANA1_ADPLL_SDM_DITH_EN_POS))
#define RF_ANA1_ADPLL_SDM_BYPASS                                RF_ANA1_ADPLL_SDM_BYPASS
#define RF_ANA1_ADPLL_SDM_BYPASS_POS                            (28U)
#define RF_ANA1_ADPLL_SDM_BYPASS_LEN                            (1U)
#define RF_ANA1_ADPLL_SDM_BYPASS_MSK                            (((1U<<RF_ANA1_ADPLL_SDM_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_POS)
#define RF_ANA1_ADPLL_SDM_BYPASS_UMSK                           (~(((1U<<RF_ANA1_ADPLL_SDM_BYPASS_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_POS))
#define RF_ANA1_ADPLL_SDM_BYPASS_HW                             RF_ANA1_ADPLL_SDM_BYPASS_HW
#define RF_ANA1_ADPLL_SDM_BYPASS_HW_POS                         (29U)
#define RF_ANA1_ADPLL_SDM_BYPASS_HW_LEN                         (1U)
#define RF_ANA1_ADPLL_SDM_BYPASS_HW_MSK                         (((1U<<RF_ANA1_ADPLL_SDM_BYPASS_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_HW_POS)
#define RF_ANA1_ADPLL_SDM_BYPASS_HW_UMSK                        (~(((1U<<RF_ANA1_ADPLL_SDM_BYPASS_HW_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_HW_POS))

/* 0x15C : lo_adpll_lms */
#define RF_ANA1_LO_ADPLL_LMS_OFFSET                             (0x15C)
#define RF_ANA1_ADPLL_PHA_PRBS_MODE                             RF_ANA1_ADPLL_PHA_PRBS_MODE
#define RF_ANA1_ADPLL_PHA_PRBS_MODE_POS                         (0U)
#define RF_ANA1_ADPLL_PHA_PRBS_MODE_LEN                         (2U)
#define RF_ANA1_ADPLL_PHA_PRBS_MODE_MSK                         (((1U<<RF_ANA1_ADPLL_PHA_PRBS_MODE_LEN)-1)<<RF_ANA1_ADPLL_PHA_PRBS_MODE_POS)
#define RF_ANA1_ADPLL_PHA_PRBS_MODE_UMSK                        (~(((1U<<RF_ANA1_ADPLL_PHA_PRBS_MODE_LEN)-1)<<RF_ANA1_ADPLL_PHA_PRBS_MODE_POS))
#define RF_ANA1_ADPLL_PHA_MSB_ERR                               RF_ANA1_ADPLL_PHA_MSB_ERR
#define RF_ANA1_ADPLL_PHA_MSB_ERR_POS                           (2U)
#define RF_ANA1_ADPLL_PHA_MSB_ERR_LEN                           (2U)
#define RF_ANA1_ADPLL_PHA_MSB_ERR_MSK                           (((1U<<RF_ANA1_ADPLL_PHA_MSB_ERR_LEN)-1)<<RF_ANA1_ADPLL_PHA_MSB_ERR_POS)
#define RF_ANA1_ADPLL_PHA_MSB_ERR_UMSK                          (~(((1U<<RF_ANA1_ADPLL_PHA_MSB_ERR_LEN)-1)<<RF_ANA1_ADPLL_PHA_MSB_ERR_POS))
#define RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN                       RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN
#define RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_POS                   (4U)
#define RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_LEN                   (1U)
#define RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_MSK                   (((1U<<RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_LEN)-1)<<RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_POS)
#define RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_UMSK                  (~(((1U<<RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_LEN)-1)<<RF_ANA1_ADPLL_PHA_MSB_ERR_PRBS_EN_POS))
#define RF_ANA1_ADPLL_PHA_DEM_EN                                RF_ANA1_ADPLL_PHA_DEM_EN
#define RF_ANA1_ADPLL_PHA_DEM_EN_POS                            (5U)
#define RF_ANA1_ADPLL_PHA_DEM_EN_LEN                            (1U)
#define RF_ANA1_ADPLL_PHA_DEM_EN_MSK                            (((1U<<RF_ANA1_ADPLL_PHA_DEM_EN_LEN)-1)<<RF_ANA1_ADPLL_PHA_DEM_EN_POS)
#define RF_ANA1_ADPLL_PHA_DEM_EN_UMSK                           (~(((1U<<RF_ANA1_ADPLL_PHA_DEM_EN_LEN)-1)<<RF_ANA1_ADPLL_PHA_DEM_EN_POS))
#define RF_ANA1_ADPLL_PHA_CANCEL_EN                             RF_ANA1_ADPLL_PHA_CANCEL_EN
#define RF_ANA1_ADPLL_PHA_CANCEL_EN_POS                         (8U)
#define RF_ANA1_ADPLL_PHA_CANCEL_EN_LEN                         (1U)
#define RF_ANA1_ADPLL_PHA_CANCEL_EN_MSK                         (((1U<<RF_ANA1_ADPLL_PHA_CANCEL_EN_LEN)-1)<<RF_ANA1_ADPLL_PHA_CANCEL_EN_POS)
#define RF_ANA1_ADPLL_PHA_CANCEL_EN_UMSK                        (~(((1U<<RF_ANA1_ADPLL_PHA_CANCEL_EN_LEN)-1)<<RF_ANA1_ADPLL_PHA_CANCEL_EN_POS))
#define RF_ANA1_ADPLL_PHA_CANCEL_DLY                            RF_ANA1_ADPLL_PHA_CANCEL_DLY
#define RF_ANA1_ADPLL_PHA_CANCEL_DLY_POS                        (9U)
#define RF_ANA1_ADPLL_PHA_CANCEL_DLY_LEN                        (2U)
#define RF_ANA1_ADPLL_PHA_CANCEL_DLY_MSK                        (((1U<<RF_ANA1_ADPLL_PHA_CANCEL_DLY_LEN)-1)<<RF_ANA1_ADPLL_PHA_CANCEL_DLY_POS)
#define RF_ANA1_ADPLL_PHA_CANCEL_DLY_UMSK                       (~(((1U<<RF_ANA1_ADPLL_PHA_CANCEL_DLY_LEN)-1)<<RF_ANA1_ADPLL_PHA_CANCEL_DLY_POS))
#define RF_ANA1_ADPLL_LMS_STEP_ENLARGE                          RF_ANA1_ADPLL_LMS_STEP_ENLARGE
#define RF_ANA1_ADPLL_LMS_STEP_ENLARGE_POS                      (11U)
#define RF_ANA1_ADPLL_LMS_STEP_ENLARGE_LEN                      (1U)
#define RF_ANA1_ADPLL_LMS_STEP_ENLARGE_MSK                      (((1U<<RF_ANA1_ADPLL_LMS_STEP_ENLARGE_LEN)-1)<<RF_ANA1_ADPLL_LMS_STEP_ENLARGE_POS)
#define RF_ANA1_ADPLL_LMS_STEP_ENLARGE_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LMS_STEP_ENLARGE_LEN)-1)<<RF_ANA1_ADPLL_LMS_STEP_ENLARGE_POS))
#define RF_ANA1_ADPLL_LMS_STEP                                  RF_ANA1_ADPLL_LMS_STEP
#define RF_ANA1_ADPLL_LMS_STEP_POS                              (12U)
#define RF_ANA1_ADPLL_LMS_STEP_LEN                              (2U)
#define RF_ANA1_ADPLL_LMS_STEP_MSK                              (((1U<<RF_ANA1_ADPLL_LMS_STEP_LEN)-1)<<RF_ANA1_ADPLL_LMS_STEP_POS)
#define RF_ANA1_ADPLL_LMS_STEP_UMSK                             (~(((1U<<RF_ANA1_ADPLL_LMS_STEP_LEN)-1)<<RF_ANA1_ADPLL_LMS_STEP_POS))
#define RF_ANA1_ADPLL_LMS_STEP_HW                               RF_ANA1_ADPLL_LMS_STEP_HW
#define RF_ANA1_ADPLL_LMS_STEP_HW_POS                           (14U)
#define RF_ANA1_ADPLL_LMS_STEP_HW_LEN                           (2U)
#define RF_ANA1_ADPLL_LMS_STEP_HW_MSK                           (((1U<<RF_ANA1_ADPLL_LMS_STEP_HW_LEN)-1)<<RF_ANA1_ADPLL_LMS_STEP_HW_POS)
#define RF_ANA1_ADPLL_LMS_STEP_HW_UMSK                          (~(((1U<<RF_ANA1_ADPLL_LMS_STEP_HW_LEN)-1)<<RF_ANA1_ADPLL_LMS_STEP_HW_POS))
#define RF_ANA1_ADPLL_LMS_Q_DLY                                 RF_ANA1_ADPLL_LMS_Q_DLY
#define RF_ANA1_ADPLL_LMS_Q_DLY_POS                             (16U)
#define RF_ANA1_ADPLL_LMS_Q_DLY_LEN                             (2U)
#define RF_ANA1_ADPLL_LMS_Q_DLY_MSK                             (((1U<<RF_ANA1_ADPLL_LMS_Q_DLY_LEN)-1)<<RF_ANA1_ADPLL_LMS_Q_DLY_POS)
#define RF_ANA1_ADPLL_LMS_Q_DLY_UMSK                            (~(((1U<<RF_ANA1_ADPLL_LMS_Q_DLY_LEN)-1)<<RF_ANA1_ADPLL_LMS_Q_DLY_POS))
#define RF_ANA1_ADPLL_LMS_POLARITY                              RF_ANA1_ADPLL_LMS_POLARITY
#define RF_ANA1_ADPLL_LMS_POLARITY_POS                          (18U)
#define RF_ANA1_ADPLL_LMS_POLARITY_LEN                          (1U)
#define RF_ANA1_ADPLL_LMS_POLARITY_MSK                          (((1U<<RF_ANA1_ADPLL_LMS_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_LMS_POLARITY_POS)
#define RF_ANA1_ADPLL_LMS_POLARITY_UMSK                         (~(((1U<<RF_ANA1_ADPLL_LMS_POLARITY_LEN)-1)<<RF_ANA1_ADPLL_LMS_POLARITY_POS))
#define RF_ANA1_ADPLL_LMS_EXT_VALUE                             RF_ANA1_ADPLL_LMS_EXT_VALUE
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_POS                         (20U)
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_LEN                         (9U)
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_MSK                         (((1U<<RF_ANA1_ADPLL_LMS_EXT_VALUE_LEN)-1)<<RF_ANA1_ADPLL_LMS_EXT_VALUE_POS)
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_UMSK                        (~(((1U<<RF_ANA1_ADPLL_LMS_EXT_VALUE_LEN)-1)<<RF_ANA1_ADPLL_LMS_EXT_VALUE_POS))
#define RF_ANA1_ADPLL_LMS_EXT_EN                                RF_ANA1_ADPLL_LMS_EXT_EN
#define RF_ANA1_ADPLL_LMS_EXT_EN_POS                            (31U)
#define RF_ANA1_ADPLL_LMS_EXT_EN_LEN                            (1U)
#define RF_ANA1_ADPLL_LMS_EXT_EN_MSK                            (((1U<<RF_ANA1_ADPLL_LMS_EXT_EN_LEN)-1)<<RF_ANA1_ADPLL_LMS_EXT_EN_POS)
#define RF_ANA1_ADPLL_LMS_EXT_EN_UMSK                           (~(((1U<<RF_ANA1_ADPLL_LMS_EXT_EN_LEN)-1)<<RF_ANA1_ADPLL_LMS_EXT_EN_POS))

/* 0x160 : lo_adpll_test */
#define RF_ANA1_LO_ADPLL_TEST_OFFSET                            (0x160)
#define RF_ANA1_ADPLL_TEST_DATA_MUX                             RF_ANA1_ADPLL_TEST_DATA_MUX
#define RF_ANA1_ADPLL_TEST_DATA_MUX_POS                         (0U)
#define RF_ANA1_ADPLL_TEST_DATA_MUX_LEN                         (2U)
#define RF_ANA1_ADPLL_TEST_DATA_MUX_MSK                         (((1U<<RF_ANA1_ADPLL_TEST_DATA_MUX_LEN)-1)<<RF_ANA1_ADPLL_TEST_DATA_MUX_POS)
#define RF_ANA1_ADPLL_TEST_DATA_MUX_UMSK                        (~(((1U<<RF_ANA1_ADPLL_TEST_DATA_MUX_LEN)-1)<<RF_ANA1_ADPLL_TEST_DATA_MUX_POS))
#define RF_ANA1_ADPLL_TEST_START_MUX                            RF_ANA1_ADPLL_TEST_START_MUX
#define RF_ANA1_ADPLL_TEST_START_MUX_POS                        (4U)
#define RF_ANA1_ADPLL_TEST_START_MUX_LEN                        (2U)
#define RF_ANA1_ADPLL_TEST_START_MUX_MSK                        (((1U<<RF_ANA1_ADPLL_TEST_START_MUX_LEN)-1)<<RF_ANA1_ADPLL_TEST_START_MUX_POS)
#define RF_ANA1_ADPLL_TEST_START_MUX_UMSK                       (~(((1U<<RF_ANA1_ADPLL_TEST_START_MUX_LEN)-1)<<RF_ANA1_ADPLL_TEST_START_MUX_POS))
#define RF_ANA1_ADPLL_TEST_EN                                   RF_ANA1_ADPLL_TEST_EN
#define RF_ANA1_ADPLL_TEST_EN_POS                               (8U)
#define RF_ANA1_ADPLL_TEST_EN_LEN                               (1U)
#define RF_ANA1_ADPLL_TEST_EN_MSK                               (((1U<<RF_ANA1_ADPLL_TEST_EN_LEN)-1)<<RF_ANA1_ADPLL_TEST_EN_POS)
#define RF_ANA1_ADPLL_TEST_EN_UMSK                              (~(((1U<<RF_ANA1_ADPLL_TEST_EN_LEN)-1)<<RF_ANA1_ADPLL_TEST_EN_POS))
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_HW                          RF_ANA1_ADPLL_LMS_EXT_VALUE_HW
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_POS                      (20U)
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_LEN                      (9U)
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_MSK                      (((1U<<RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_LEN)-1)<<RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_POS)
#define RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_UMSK                     (~(((1U<<RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_LEN)-1)<<RF_ANA1_ADPLL_LMS_EXT_VALUE_HW_POS))

/* 0x164 : lo_adpll_resv */
#define RF_ANA1_LO_ADPLL_RESV_OFFSET                            (0x164)
#define RF_ANA1_ADPLL_RESV1                                     RF_ANA1_ADPLL_RESV1
#define RF_ANA1_ADPLL_RESV1_POS                                 (0U)
#define RF_ANA1_ADPLL_RESV1_LEN                                 (16U)
#define RF_ANA1_ADPLL_RESV1_MSK                                 (((1U<<RF_ANA1_ADPLL_RESV1_LEN)-1)<<RF_ANA1_ADPLL_RESV1_POS)
#define RF_ANA1_ADPLL_RESV1_UMSK                                (~(((1U<<RF_ANA1_ADPLL_RESV1_LEN)-1)<<RF_ANA1_ADPLL_RESV1_POS))
#define RF_ANA1_ADPLL_RESV0                                     RF_ANA1_ADPLL_RESV0
#define RF_ANA1_ADPLL_RESV0_POS                                 (16U)
#define RF_ANA1_ADPLL_RESV0_LEN                                 (16U)
#define RF_ANA1_ADPLL_RESV0_MSK                                 (((1U<<RF_ANA1_ADPLL_RESV0_LEN)-1)<<RF_ANA1_ADPLL_RESV0_POS)
#define RF_ANA1_ADPLL_RESV0_UMSK                                (~(((1U<<RF_ANA1_ADPLL_RESV0_LEN)-1)<<RF_ANA1_ADPLL_RESV0_POS))

/* 0x168 : lo_adpll_output */
#define RF_ANA1_LO_ADPLL_OUTPUT_OFFSET                          (0x168)
#define RF_ANA1_ADPLL_UNLOCK_INTRPT                             RF_ANA1_ADPLL_UNLOCK_INTRPT
#define RF_ANA1_ADPLL_UNLOCK_INTRPT_POS                         (0U)
#define RF_ANA1_ADPLL_UNLOCK_INTRPT_LEN                         (1U)
#define RF_ANA1_ADPLL_UNLOCK_INTRPT_MSK                         (((1U<<RF_ANA1_ADPLL_UNLOCK_INTRPT_LEN)-1)<<RF_ANA1_ADPLL_UNLOCK_INTRPT_POS)
#define RF_ANA1_ADPLL_UNLOCK_INTRPT_UMSK                        (~(((1U<<RF_ANA1_ADPLL_UNLOCK_INTRPT_LEN)-1)<<RF_ANA1_ADPLL_UNLOCK_INTRPT_POS))
#define RF_ANA1_ADPLL_LO_LOCK                                   RF_ANA1_ADPLL_LO_LOCK
#define RF_ANA1_ADPLL_LO_LOCK_POS                               (1U)
#define RF_ANA1_ADPLL_LO_LOCK_LEN                               (1U)
#define RF_ANA1_ADPLL_LO_LOCK_MSK                               (((1U<<RF_ANA1_ADPLL_LO_LOCK_LEN)-1)<<RF_ANA1_ADPLL_LO_LOCK_POS)
#define RF_ANA1_ADPLL_LO_LOCK_UMSK                              (~(((1U<<RF_ANA1_ADPLL_LO_LOCK_LEN)-1)<<RF_ANA1_ADPLL_LO_LOCK_POS))
#define RF_ANA1_ADPLL_FSM_STATE                                 RF_ANA1_ADPLL_FSM_STATE
#define RF_ANA1_ADPLL_FSM_STATE_POS                             (4U)
#define RF_ANA1_ADPLL_FSM_STATE_LEN                             (4U)
#define RF_ANA1_ADPLL_FSM_STATE_MSK                             (((1U<<RF_ANA1_ADPLL_FSM_STATE_LEN)-1)<<RF_ANA1_ADPLL_FSM_STATE_POS)
#define RF_ANA1_ADPLL_FSM_STATE_UMSK                            (~(((1U<<RF_ANA1_ADPLL_FSM_STATE_LEN)-1)<<RF_ANA1_ADPLL_FSM_STATE_POS))
#define RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE                       RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE
#define RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_POS                   (8U)
#define RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_LEN                   (1U)
#define RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_MSK                   (((1U<<RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_LEN)-1)<<RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_POS)
#define RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_UMSK                  (~(((1U<<RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_LEN)-1)<<RF_ANA1_ADPLL_SOC_VCTRL_OUT_RANGE_POS))
#define RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE                        RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE
#define RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_POS                    (9U)
#define RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_LEN                    (1U)
#define RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_MSK                    (((1U<<RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_LEN)-1)<<RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_POS)
#define RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_UMSK                   (~(((1U<<RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_LEN)-1)<<RF_ANA1_ADPLL_SOC_VCTRL_DET_DONE_POS))
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN                       RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_POS                   (10U)
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_LEN                   (1U)
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_MSK                   (((1U<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_LEN)-1)<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_POS)
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_UMSK                  (~(((1U<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_LEN)-1)<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_SIGN_POS))
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK                            RF_ANA1_ADPLL_SOC_SPD_UNLOCK
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_POS                        (11U)
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_LEN                        (1U)
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_MSK                        (((1U<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_LEN)-1)<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_POS)
#define RF_ANA1_ADPLL_SOC_SPD_UNLOCK_UMSK                       (~(((1U<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_LEN)-1)<<RF_ANA1_ADPLL_SOC_SPD_UNLOCK_POS))
#define RF_ANA1_ADPLL_SOC_SPD_LOCK                              RF_ANA1_ADPLL_SOC_SPD_LOCK
#define RF_ANA1_ADPLL_SOC_SPD_LOCK_POS                          (12U)
#define RF_ANA1_ADPLL_SOC_SPD_LOCK_LEN                          (1U)
#define RF_ANA1_ADPLL_SOC_SPD_LOCK_MSK                          (((1U<<RF_ANA1_ADPLL_SOC_SPD_LOCK_LEN)-1)<<RF_ANA1_ADPLL_SOC_SPD_LOCK_POS)
#define RF_ANA1_ADPLL_SOC_SPD_LOCK_UMSK                         (~(((1U<<RF_ANA1_ADPLL_SOC_SPD_LOCK_LEN)-1)<<RF_ANA1_ADPLL_SOC_SPD_LOCK_POS))
#define RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU                         RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU
#define RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_POS                     (13U)
#define RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_LEN                     (1U)
#define RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_MSK                     (((1U<<RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_POS)
#define RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_UMSK                    (~(((1U<<RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_MOM_UPDATE_OU_POS))
#define RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU                          RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU
#define RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_POS                      (14U)
#define RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_LEN                      (2U)
#define RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_MSK                      (((1U<<RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_POS)
#define RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_MOM_TOTAL_OU_POS))
#define RF_ANA1_ADPLL_SOC_FREQERR_SIGN                          RF_ANA1_ADPLL_SOC_FREQERR_SIGN
#define RF_ANA1_ADPLL_SOC_FREQERR_SIGN_POS                      (16U)
#define RF_ANA1_ADPLL_SOC_FREQERR_SIGN_LEN                      (1U)
#define RF_ANA1_ADPLL_SOC_FREQERR_SIGN_MSK                      (((1U<<RF_ANA1_ADPLL_SOC_FREQERR_SIGN_LEN)-1)<<RF_ANA1_ADPLL_SOC_FREQERR_SIGN_POS)
#define RF_ANA1_ADPLL_SOC_FREQERR_SIGN_UMSK                     (~(((1U<<RF_ANA1_ADPLL_SOC_FREQERR_SIGN_LEN)-1)<<RF_ANA1_ADPLL_SOC_FREQERR_SIGN_POS))
#define RF_ANA1_ADPLL_SOC_FREQERR_OU                            RF_ANA1_ADPLL_SOC_FREQERR_OU
#define RF_ANA1_ADPLL_SOC_FREQERR_OU_POS                        (17U)
#define RF_ANA1_ADPLL_SOC_FREQERR_OU_LEN                        (1U)
#define RF_ANA1_ADPLL_SOC_FREQERR_OU_MSK                        (((1U<<RF_ANA1_ADPLL_SOC_FREQERR_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_FREQERR_OU_POS)
#define RF_ANA1_ADPLL_SOC_FREQERR_OU_UMSK                       (~(((1U<<RF_ANA1_ADPLL_SOC_FREQERR_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_FREQERR_OU_POS))
#define RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE                      RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE
#define RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_POS                  (18U)
#define RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_LEN                  (1U)
#define RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_MSK                  (((1U<<RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_LEN)-1)<<RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_POS)
#define RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_UMSK                 (~(((1U<<RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_LEN)-1)<<RF_ANA1_ADPLL_SOC_FREQERR_DET_DONE_POS))
#define RF_ANA1_ADPLL_SOC_FCAL_DONE                             RF_ANA1_ADPLL_SOC_FCAL_DONE
#define RF_ANA1_ADPLL_SOC_FCAL_DONE_POS                         (19U)
#define RF_ANA1_ADPLL_SOC_FCAL_DONE_LEN                         (1U)
#define RF_ANA1_ADPLL_SOC_FCAL_DONE_MSK                         (((1U<<RF_ANA1_ADPLL_SOC_FCAL_DONE_LEN)-1)<<RF_ANA1_ADPLL_SOC_FCAL_DONE_POS)
#define RF_ANA1_ADPLL_SOC_FCAL_DONE_UMSK                        (~(((1U<<RF_ANA1_ADPLL_SOC_FCAL_DONE_LEN)-1)<<RF_ANA1_ADPLL_SOC_FCAL_DONE_POS))
#define RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU                      RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU
#define RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_POS                  (20U)
#define RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_LEN                  (1U)
#define RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_MSK                  (((1U<<RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_POS)
#define RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_UMSK                 (~(((1U<<RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_LEN)-1)<<RF_ANA1_ADPLL_SOC_FAST_INC_FCAL_OU_POS))
#define RF_ANA1_ADPLL_SOC_CAPCODE_UD                            RF_ANA1_ADPLL_SOC_CAPCODE_UD
#define RF_ANA1_ADPLL_SOC_CAPCODE_UD_POS                        (21U)
#define RF_ANA1_ADPLL_SOC_CAPCODE_UD_LEN                        (1U)
#define RF_ANA1_ADPLL_SOC_CAPCODE_UD_MSK                        (((1U<<RF_ANA1_ADPLL_SOC_CAPCODE_UD_LEN)-1)<<RF_ANA1_ADPLL_SOC_CAPCODE_UD_POS)
#define RF_ANA1_ADPLL_SOC_CAPCODE_UD_UMSK                       (~(((1U<<RF_ANA1_ADPLL_SOC_CAPCODE_UD_LEN)-1)<<RF_ANA1_ADPLL_SOC_CAPCODE_UD_POS))
#define RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE                     RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE
#define RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_POS                 (22U)
#define RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_LEN                 (1U)
#define RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_MSK                 (((1U<<RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_LEN)-1)<<RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_POS)
#define RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_UMSK                (~(((1U<<RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_LEN)-1)<<RF_ANA1_ADPLL_SOC_CAPCODE_OUT_RANGE_POS))
#define RF_ANA1_ADPLL_TEST_START                                RF_ANA1_ADPLL_TEST_START
#define RF_ANA1_ADPLL_TEST_START_POS                            (28U)
#define RF_ANA1_ADPLL_TEST_START_LEN                            (1U)
#define RF_ANA1_ADPLL_TEST_START_MSK                            (((1U<<RF_ANA1_ADPLL_TEST_START_LEN)-1)<<RF_ANA1_ADPLL_TEST_START_POS)
#define RF_ANA1_ADPLL_TEST_START_UMSK                           (~(((1U<<RF_ANA1_ADPLL_TEST_START_LEN)-1)<<RF_ANA1_ADPLL_TEST_START_POS))

/* 0x16C : lo_adpll_testout */
#define RF_ANA1_LO_ADPLL_TESTOUT_OFFSET                         (0x16C)
#define RF_ANA1_ADPLL_TEST_OUT                                  RF_ANA1_ADPLL_TEST_OUT
#define RF_ANA1_ADPLL_TEST_OUT_POS                              (0U)
#define RF_ANA1_ADPLL_TEST_OUT_LEN                              (32U)
#define RF_ANA1_ADPLL_TEST_OUT_MSK                              (((1U<<RF_ANA1_ADPLL_TEST_OUT_LEN)-1)<<RF_ANA1_ADPLL_TEST_OUT_POS)
#define RF_ANA1_ADPLL_TEST_OUT_UMSK                             (~(((1U<<RF_ANA1_ADPLL_TEST_OUT_LEN)-1)<<RF_ANA1_ADPLL_TEST_OUT_POS))

/* 0x170 : lo0_adpll_sdm_hw1 */
#define RF_ANA1_LO0_ADPLL_SDM_HW1_OFFSET                        (0x170)
#define RF_ANA1_ADPLL_SDM_FRAC_TH                               RF_ANA1_ADPLL_SDM_FRAC_TH
#define RF_ANA1_ADPLL_SDM_FRAC_TH_POS                           (0U)
#define RF_ANA1_ADPLL_SDM_FRAC_TH_LEN                           (22U)
#define RF_ANA1_ADPLL_SDM_FRAC_TH_MSK                           (((1U<<RF_ANA1_ADPLL_SDM_FRAC_TH_LEN)-1)<<RF_ANA1_ADPLL_SDM_FRAC_TH_POS)
#define RF_ANA1_ADPLL_SDM_FRAC_TH_UMSK                          (~(((1U<<RF_ANA1_ADPLL_SDM_FRAC_TH_LEN)-1)<<RF_ANA1_ADPLL_SDM_FRAC_TH_POS))
#define RF_ANA1_ADPLL_SDM_BYPASS_TH                             RF_ANA1_ADPLL_SDM_BYPASS_TH
#define RF_ANA1_ADPLL_SDM_BYPASS_TH_POS                         (26U)
#define RF_ANA1_ADPLL_SDM_BYPASS_TH_LEN                         (6U)
#define RF_ANA1_ADPLL_SDM_BYPASS_TH_MSK                         (((1U<<RF_ANA1_ADPLL_SDM_BYPASS_TH_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_TH_POS)
#define RF_ANA1_ADPLL_SDM_BYPASS_TH_UMSK                        (~(((1U<<RF_ANA1_ADPLL_SDM_BYPASS_TH_LEN)-1)<<RF_ANA1_ADPLL_SDM_BYPASS_TH_POS))

/* 0x1C0 : top_dly_ctrl */
#define RF_ANA1_TOP_DLY_CTRL_OFFSET                             (0x1C0)
#define RF_ANA1_PUD_DLY_VCO                                     RF_ANA1_PUD_DLY_VCO
#define RF_ANA1_PUD_DLY_VCO_POS                                 (4U)
#define RF_ANA1_PUD_DLY_VCO_LEN                                 (3U)
#define RF_ANA1_PUD_DLY_VCO_MSK                                 (((1U<<RF_ANA1_PUD_DLY_VCO_LEN)-1)<<RF_ANA1_PUD_DLY_VCO_POS)
#define RF_ANA1_PUD_DLY_VCO_UMSK                                (~(((1U<<RF_ANA1_PUD_DLY_VCO_LEN)-1)<<RF_ANA1_PUD_DLY_VCO_POS))
#define RF_ANA1_PUD_DLY_DTC                                     RF_ANA1_PUD_DLY_DTC
#define RF_ANA1_PUD_DLY_DTC_POS                                 (8U)
#define RF_ANA1_PUD_DLY_DTC_LEN                                 (3U)
#define RF_ANA1_PUD_DLY_DTC_MSK                                 (((1U<<RF_ANA1_PUD_DLY_DTC_LEN)-1)<<RF_ANA1_PUD_DLY_DTC_POS)
#define RF_ANA1_PUD_DLY_DTC_UMSK                                (~(((1U<<RF_ANA1_PUD_DLY_DTC_LEN)-1)<<RF_ANA1_PUD_DLY_DTC_POS))
#define RF_ANA1_PUD_DLY_RPT                                     RF_ANA1_PUD_DLY_RPT
#define RF_ANA1_PUD_DLY_RPT_POS                                 (12U)
#define RF_ANA1_PUD_DLY_RPT_LEN                                 (3U)
#define RF_ANA1_PUD_DLY_RPT_MSK                                 (((1U<<RF_ANA1_PUD_DLY_RPT_LEN)-1)<<RF_ANA1_PUD_DLY_RPT_POS)
#define RF_ANA1_PUD_DLY_RPT_UMSK                                (~(((1U<<RF_ANA1_PUD_DLY_RPT_LEN)-1)<<RF_ANA1_PUD_DLY_RPT_POS))
#define RF_ANA1_PUD_DLY_PA                                      RF_ANA1_PUD_DLY_PA
#define RF_ANA1_PUD_DLY_PA_POS                                  (16U)
#define RF_ANA1_PUD_DLY_PA_LEN                                  (3U)
#define RF_ANA1_PUD_DLY_PA_MSK                                  (((1U<<RF_ANA1_PUD_DLY_PA_LEN)-1)<<RF_ANA1_PUD_DLY_PA_POS)
#define RF_ANA1_PUD_DLY_PA_UMSK                                 (~(((1U<<RF_ANA1_PUD_DLY_PA_LEN)-1)<<RF_ANA1_PUD_DLY_PA_POS))
#define RF_ANA1_PUD_DLY_LPVCO                                   RF_ANA1_PUD_DLY_LPVCO
#define RF_ANA1_PUD_DLY_LPVCO_POS                               (20U)
#define RF_ANA1_PUD_DLY_LPVCO_LEN                               (3U)
#define RF_ANA1_PUD_DLY_LPVCO_MSK                               (((1U<<RF_ANA1_PUD_DLY_LPVCO_LEN)-1)<<RF_ANA1_PUD_DLY_LPVCO_POS)
#define RF_ANA1_PUD_DLY_LPVCO_UMSK                              (~(((1U<<RF_ANA1_PUD_DLY_LPVCO_LEN)-1)<<RF_ANA1_PUD_DLY_LPVCO_POS))

/* 0x1C4 : lo_dly_ctrl */
#define RF_ANA1_LO_DLY_CTRL_OFFSET                              (0x1C4)
#define RF_ANA1_LO_LOCK_CHECK_DLY                               RF_ANA1_LO_LOCK_CHECK_DLY
#define RF_ANA1_LO_LOCK_CHECK_DLY_POS                           (16U)
#define RF_ANA1_LO_LOCK_CHECK_DLY_LEN                           (2U)
#define RF_ANA1_LO_LOCK_CHECK_DLY_MSK                           (((1U<<RF_ANA1_LO_LOCK_CHECK_DLY_LEN)-1)<<RF_ANA1_LO_LOCK_CHECK_DLY_POS)
#define RF_ANA1_LO_LOCK_CHECK_DLY_UMSK                          (~(((1U<<RF_ANA1_LO_LOCK_CHECK_DLY_LEN)-1)<<RF_ANA1_LO_LOCK_CHECK_DLY_POS))
#define RF_ANA1_INC_FCAL_WINDOW                                 RF_ANA1_INC_FCAL_WINDOW
#define RF_ANA1_INC_FCAL_WINDOW_POS                             (18U)
#define RF_ANA1_INC_FCAL_WINDOW_LEN                             (2U)
#define RF_ANA1_INC_FCAL_WINDOW_MSK                             (((1U<<RF_ANA1_INC_FCAL_WINDOW_LEN)-1)<<RF_ANA1_INC_FCAL_WINDOW_POS)
#define RF_ANA1_INC_FCAL_WINDOW_UMSK                            (~(((1U<<RF_ANA1_INC_FCAL_WINDOW_LEN)-1)<<RF_ANA1_INC_FCAL_WINDOW_POS))
#define RF_ANA1_FAST_INC_FCAL_WINDOW                            RF_ANA1_FAST_INC_FCAL_WINDOW
#define RF_ANA1_FAST_INC_FCAL_WINDOW_POS                        (20U)
#define RF_ANA1_FAST_INC_FCAL_WINDOW_LEN                        (2U)
#define RF_ANA1_FAST_INC_FCAL_WINDOW_MSK                        (((1U<<RF_ANA1_FAST_INC_FCAL_WINDOW_LEN)-1)<<RF_ANA1_FAST_INC_FCAL_WINDOW_POS)
#define RF_ANA1_FAST_INC_FCAL_WINDOW_UMSK                       (~(((1U<<RF_ANA1_FAST_INC_FCAL_WINDOW_LEN)-1)<<RF_ANA1_FAST_INC_FCAL_WINDOW_POS))
#define RF_ANA1_RSTB_FBDIV_WINDOW                               RF_ANA1_RSTB_FBDIV_WINDOW
#define RF_ANA1_RSTB_FBDIV_WINDOW_POS                           (22U)
#define RF_ANA1_RSTB_FBDIV_WINDOW_LEN                           (2U)
#define RF_ANA1_RSTB_FBDIV_WINDOW_MSK                           (((1U<<RF_ANA1_RSTB_FBDIV_WINDOW_LEN)-1)<<RF_ANA1_RSTB_FBDIV_WINDOW_POS)
#define RF_ANA1_RSTB_FBDIV_WINDOW_UMSK                          (~(((1U<<RF_ANA1_RSTB_FBDIV_WINDOW_LEN)-1)<<RF_ANA1_RSTB_FBDIV_WINDOW_POS))
#define RF_ANA1_RSTB_ADPLL_WINDOW                               RF_ANA1_RSTB_ADPLL_WINDOW
#define RF_ANA1_RSTB_ADPLL_WINDOW_POS                           (24U)
#define RF_ANA1_RSTB_ADPLL_WINDOW_LEN                           (2U)
#define RF_ANA1_RSTB_ADPLL_WINDOW_MSK                           (((1U<<RF_ANA1_RSTB_ADPLL_WINDOW_LEN)-1)<<RF_ANA1_RSTB_ADPLL_WINDOW_POS)
#define RF_ANA1_RSTB_ADPLL_WINDOW_UMSK                          (~(((1U<<RF_ANA1_RSTB_ADPLL_WINDOW_LEN)-1)<<RF_ANA1_RSTB_ADPLL_WINDOW_POS))
#define RF_ANA1_RSTB_DLY                                        RF_ANA1_RSTB_DLY
#define RF_ANA1_RSTB_DLY_POS                                    (26U)
#define RF_ANA1_RSTB_DLY_LEN                                    (2U)
#define RF_ANA1_RSTB_DLY_MSK                                    (((1U<<RF_ANA1_RSTB_DLY_LEN)-1)<<RF_ANA1_RSTB_DLY_POS)
#define RF_ANA1_RSTB_DLY_UMSK                                   (~(((1U<<RF_ANA1_RSTB_DLY_LEN)-1)<<RF_ANA1_RSTB_DLY_POS))
#define RF_ANA1_RPT_SYNC_DLY1                                   RF_ANA1_RPT_SYNC_DLY1
#define RF_ANA1_RPT_SYNC_DLY1_POS                               (28U)
#define RF_ANA1_RPT_SYNC_DLY1_LEN                               (2U)
#define RF_ANA1_RPT_SYNC_DLY1_MSK                               (((1U<<RF_ANA1_RPT_SYNC_DLY1_LEN)-1)<<RF_ANA1_RPT_SYNC_DLY1_POS)
#define RF_ANA1_RPT_SYNC_DLY1_UMSK                              (~(((1U<<RF_ANA1_RPT_SYNC_DLY1_LEN)-1)<<RF_ANA1_RPT_SYNC_DLY1_POS))
#define RF_ANA1_RPT_SYNC_DLY                                    RF_ANA1_RPT_SYNC_DLY
#define RF_ANA1_RPT_SYNC_DLY_POS                                (30U)
#define RF_ANA1_RPT_SYNC_DLY_LEN                                (2U)
#define RF_ANA1_RPT_SYNC_DLY_MSK                                (((1U<<RF_ANA1_RPT_SYNC_DLY_LEN)-1)<<RF_ANA1_RPT_SYNC_DLY_POS)
#define RF_ANA1_RPT_SYNC_DLY_UMSK                               (~(((1U<<RF_ANA1_RPT_SYNC_DLY_LEN)-1)<<RF_ANA1_RPT_SYNC_DLY_POS))

/* 0x1C8 : ss0_rx_gain */
#define RF_ANA1_SS0_RX_GAIN_OFFSET                              (0x1C8)
#define RF_ANA1_SS0_GC_RBB2                                     RF_ANA1_SS0_GC_RBB2
#define RF_ANA1_SS0_GC_RBB2_POS                                 (0U)
#define RF_ANA1_SS0_GC_RBB2_LEN                                 (3U)
#define RF_ANA1_SS0_GC_RBB2_MSK                                 (((1U<<RF_ANA1_SS0_GC_RBB2_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_POS)
#define RF_ANA1_SS0_GC_RBB2_UMSK                                (~(((1U<<RF_ANA1_SS0_GC_RBB2_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_POS))
#define RF_ANA1_SS0_GC_RBB1                                     RF_ANA1_SS0_GC_RBB1
#define RF_ANA1_SS0_GC_RBB1_POS                                 (4U)
#define RF_ANA1_SS0_GC_RBB1_LEN                                 (2U)
#define RF_ANA1_SS0_GC_RBB1_MSK                                 (((1U<<RF_ANA1_SS0_GC_RBB1_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_POS)
#define RF_ANA1_SS0_GC_RBB1_UMSK                                (~(((1U<<RF_ANA1_SS0_GC_RBB1_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_POS))
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT                          RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_POS                      (8U)
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_LEN                      (3U)
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_MSK                      (((1U<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_POS)
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_UMSK                     (~(((1U<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_POS))
#define RF_ANA1_SS0_GC_RMXGM_5G                                 RF_ANA1_SS0_GC_RMXGM_5G
#define RF_ANA1_SS0_GC_RMXGM_5G_POS                             (12U)
#define RF_ANA1_SS0_GC_RMXGM_5G_LEN                             (2U)
#define RF_ANA1_SS0_GC_RMXGM_5G_MSK                             (((1U<<RF_ANA1_SS0_GC_RMXGM_5G_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_5G_POS)
#define RF_ANA1_SS0_GC_RMXGM_5G_UMSK                            (~(((1U<<RF_ANA1_SS0_GC_RMXGM_5G_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_5G_POS))
#define RF_ANA1_SS0_GC_LNA_5G                                   RF_ANA1_SS0_GC_LNA_5G
#define RF_ANA1_SS0_GC_LNA_5G_POS                               (16U)
#define RF_ANA1_SS0_GC_LNA_5G_LEN                               (3U)
#define RF_ANA1_SS0_GC_LNA_5G_MSK                               (((1U<<RF_ANA1_SS0_GC_LNA_5G_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_POS)
#define RF_ANA1_SS0_GC_LNA_5G_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_LNA_5G_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_POS))
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT                          RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_POS                      (20U)
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_LEN                      (3U)
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_MSK                      (((1U<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_POS)
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_UMSK                     (~(((1U<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_POS))
#define RF_ANA1_SS0_GC_RMXGM_2G                                 RF_ANA1_SS0_GC_RMXGM_2G
#define RF_ANA1_SS0_GC_RMXGM_2G_POS                             (24U)
#define RF_ANA1_SS0_GC_RMXGM_2G_LEN                             (2U)
#define RF_ANA1_SS0_GC_RMXGM_2G_MSK                             (((1U<<RF_ANA1_SS0_GC_RMXGM_2G_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_2G_POS)
#define RF_ANA1_SS0_GC_RMXGM_2G_UMSK                            (~(((1U<<RF_ANA1_SS0_GC_RMXGM_2G_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_2G_POS))
#define RF_ANA1_SS0_GC_LNA_2G                                   RF_ANA1_SS0_GC_LNA_2G
#define RF_ANA1_SS0_GC_LNA_2G_POS                               (28U)
#define RF_ANA1_SS0_GC_LNA_2G_LEN                               (3U)
#define RF_ANA1_SS0_GC_LNA_2G_MSK                               (((1U<<RF_ANA1_SS0_GC_LNA_2G_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_POS)
#define RF_ANA1_SS0_GC_LNA_2G_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_LNA_2G_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_POS))

/* 0x1CC : ss0_rx_gain_hw */
#define RF_ANA1_SS0_RX_GAIN_HW_OFFSET                           (0x1CC)
#define RF_ANA1_SS0_GC_RBB2_HW                                  RF_ANA1_SS0_GC_RBB2_HW
#define RF_ANA1_SS0_GC_RBB2_HW_POS                              (0U)
#define RF_ANA1_SS0_GC_RBB2_HW_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_HW_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_HW_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_HW_POS)
#define RF_ANA1_SS0_GC_RBB2_HW_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_HW_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_HW_POS))
#define RF_ANA1_SS0_GC_RBB1_HW                                  RF_ANA1_SS0_GC_RBB1_HW
#define RF_ANA1_SS0_GC_RBB1_HW_POS                              (4U)
#define RF_ANA1_SS0_GC_RBB1_HW_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_HW_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_HW_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_HW_POS)
#define RF_ANA1_SS0_GC_RBB1_HW_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_HW_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_HW_POS))
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW                       RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_POS                   (8U)
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_LEN                   (3U)
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_MSK                   (((1U<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_POS)
#define RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_UMSK                  (~(((1U<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_LOAD_ATT_HW_POS))
#define RF_ANA1_SS0_GC_RMXGM_5G_HW                              RF_ANA1_SS0_GC_RMXGM_5G_HW
#define RF_ANA1_SS0_GC_RMXGM_5G_HW_POS                          (12U)
#define RF_ANA1_SS0_GC_RMXGM_5G_HW_LEN                          (2U)
#define RF_ANA1_SS0_GC_RMXGM_5G_HW_MSK                          (((1U<<RF_ANA1_SS0_GC_RMXGM_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_5G_HW_POS)
#define RF_ANA1_SS0_GC_RMXGM_5G_HW_UMSK                         (~(((1U<<RF_ANA1_SS0_GC_RMXGM_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_5G_HW_POS))
#define RF_ANA1_SS0_GC_LNA_5G_HW                                RF_ANA1_SS0_GC_LNA_5G_HW
#define RF_ANA1_SS0_GC_LNA_5G_HW_POS                            (16U)
#define RF_ANA1_SS0_GC_LNA_5G_HW_LEN                            (3U)
#define RF_ANA1_SS0_GC_LNA_5G_HW_MSK                            (((1U<<RF_ANA1_SS0_GC_LNA_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_HW_POS)
#define RF_ANA1_SS0_GC_LNA_5G_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_LNA_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_5G_HW_POS))
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW                       RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_POS                   (20U)
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_LEN                   (3U)
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_MSK                   (((1U<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_POS)
#define RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_UMSK                  (~(((1U<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_LOAD_ATT_HW_POS))
#define RF_ANA1_SS0_GC_RMXGM_2G_HW                              RF_ANA1_SS0_GC_RMXGM_2G_HW
#define RF_ANA1_SS0_GC_RMXGM_2G_HW_POS                          (24U)
#define RF_ANA1_SS0_GC_RMXGM_2G_HW_LEN                          (2U)
#define RF_ANA1_SS0_GC_RMXGM_2G_HW_MSK                          (((1U<<RF_ANA1_SS0_GC_RMXGM_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_2G_HW_POS)
#define RF_ANA1_SS0_GC_RMXGM_2G_HW_UMSK                         (~(((1U<<RF_ANA1_SS0_GC_RMXGM_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_RMXGM_2G_HW_POS))
#define RF_ANA1_SS0_GC_LNA_2G_HW                                RF_ANA1_SS0_GC_LNA_2G_HW
#define RF_ANA1_SS0_GC_LNA_2G_HW_POS                            (28U)
#define RF_ANA1_SS0_GC_LNA_2G_HW_LEN                            (3U)
#define RF_ANA1_SS0_GC_LNA_2G_HW_MSK                            (((1U<<RF_ANA1_SS0_GC_LNA_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_HW_POS)
#define RF_ANA1_SS0_GC_LNA_2G_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_LNA_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_LNA_2G_HW_POS))

/* 0x1D0 : ss0_tx_gain0 */
#define RF_ANA1_SS0_TX_GAIN0_OFFSET                             (0x1D0)
#define RF_ANA1_SS0_DA_2G_CS                                    RF_ANA1_SS0_DA_2G_CS
#define RF_ANA1_SS0_DA_2G_CS_POS                                (0U)
#define RF_ANA1_SS0_DA_2G_CS_LEN                                (4U)
#define RF_ANA1_SS0_DA_2G_CS_MSK                                (((1U<<RF_ANA1_SS0_DA_2G_CS_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_POS)
#define RF_ANA1_SS0_DA_2G_CS_UMSK                               (~(((1U<<RF_ANA1_SS0_DA_2G_CS_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_POS))
#define RF_ANA1_SS0_TMX_2G_CS                                   RF_ANA1_SS0_TMX_2G_CS
#define RF_ANA1_SS0_TMX_2G_CS_POS                               (4U)
#define RF_ANA1_SS0_TMX_2G_CS_LEN                               (5U)
#define RF_ANA1_SS0_TMX_2G_CS_MSK                               (((1U<<RF_ANA1_SS0_TMX_2G_CS_LEN)-1)<<RF_ANA1_SS0_TMX_2G_CS_POS)
#define RF_ANA1_SS0_TMX_2G_CS_UMSK                              (~(((1U<<RF_ANA1_SS0_TMX_2G_CS_LEN)-1)<<RF_ANA1_SS0_TMX_2G_CS_POS))
#define RF_ANA1_SS0_GC_PA_2G                                    RF_ANA1_SS0_GC_PA_2G
#define RF_ANA1_SS0_GC_PA_2G_POS                                (10U)
#define RF_ANA1_SS0_GC_PA_2G_LEN                                (2U)
#define RF_ANA1_SS0_GC_PA_2G_MSK                                (((1U<<RF_ANA1_SS0_GC_PA_2G_LEN)-1)<<RF_ANA1_SS0_GC_PA_2G_POS)
#define RF_ANA1_SS0_GC_PA_2G_UMSK                               (~(((1U<<RF_ANA1_SS0_GC_PA_2G_LEN)-1)<<RF_ANA1_SS0_GC_PA_2G_POS))
#define RF_ANA1_SS0_GC_TMX_2G                                   RF_ANA1_SS0_GC_TMX_2G
#define RF_ANA1_SS0_GC_TMX_2G_POS                               (20U)
#define RF_ANA1_SS0_GC_TMX_2G_LEN                               (4U)
#define RF_ANA1_SS0_GC_TMX_2G_MSK                               (((1U<<RF_ANA1_SS0_GC_TMX_2G_LEN)-1)<<RF_ANA1_SS0_GC_TMX_2G_POS)
#define RF_ANA1_SS0_GC_TMX_2G_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_TMX_2G_LEN)-1)<<RF_ANA1_SS0_GC_TMX_2G_POS))
#define RF_ANA1_SS0_GC_TBB                                      RF_ANA1_SS0_GC_TBB
#define RF_ANA1_SS0_GC_TBB_POS                                  (24U)
#define RF_ANA1_SS0_GC_TBB_LEN                                  (6U)
#define RF_ANA1_SS0_GC_TBB_MSK                                  (((1U<<RF_ANA1_SS0_GC_TBB_LEN)-1)<<RF_ANA1_SS0_GC_TBB_POS)
#define RF_ANA1_SS0_GC_TBB_UMSK                                 (~(((1U<<RF_ANA1_SS0_GC_TBB_LEN)-1)<<RF_ANA1_SS0_GC_TBB_POS))
#define RF_ANA1_SS0_GC_TBB_BOOST                                RF_ANA1_SS0_GC_TBB_BOOST
#define RF_ANA1_SS0_GC_TBB_BOOST_POS                            (30U)
#define RF_ANA1_SS0_GC_TBB_BOOST_LEN                            (2U)
#define RF_ANA1_SS0_GC_TBB_BOOST_MSK                            (((1U<<RF_ANA1_SS0_GC_TBB_BOOST_LEN)-1)<<RF_ANA1_SS0_GC_TBB_BOOST_POS)
#define RF_ANA1_SS0_GC_TBB_BOOST_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_TBB_BOOST_LEN)-1)<<RF_ANA1_SS0_GC_TBB_BOOST_POS))

/* 0x1D4 : ss0_tx_gain1 */
#define RF_ANA1_SS0_TX_GAIN1_OFFSET                             (0x1D4)
#define RF_ANA1_SS0_DA_5G_CS                                    RF_ANA1_SS0_DA_5G_CS
#define RF_ANA1_SS0_DA_5G_CS_POS                                (0U)
#define RF_ANA1_SS0_DA_5G_CS_LEN                                (5U)
#define RF_ANA1_SS0_DA_5G_CS_MSK                                (((1U<<RF_ANA1_SS0_DA_5G_CS_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_POS)
#define RF_ANA1_SS0_DA_5G_CS_UMSK                               (~(((1U<<RF_ANA1_SS0_DA_5G_CS_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_POS))
#define RF_ANA1_SS0_TMX_5G_CS                                   RF_ANA1_SS0_TMX_5G_CS
#define RF_ANA1_SS0_TMX_5G_CS_POS                               (8U)
#define RF_ANA1_SS0_TMX_5G_CS_LEN                               (5U)
#define RF_ANA1_SS0_TMX_5G_CS_MSK                               (((1U<<RF_ANA1_SS0_TMX_5G_CS_LEN)-1)<<RF_ANA1_SS0_TMX_5G_CS_POS)
#define RF_ANA1_SS0_TMX_5G_CS_UMSK                              (~(((1U<<RF_ANA1_SS0_TMX_5G_CS_LEN)-1)<<RF_ANA1_SS0_TMX_5G_CS_POS))
#define RF_ANA1_SS0_GC_PA_5G                                    RF_ANA1_SS0_GC_PA_5G
#define RF_ANA1_SS0_GC_PA_5G_POS                                (16U)
#define RF_ANA1_SS0_GC_PA_5G_LEN                                (2U)
#define RF_ANA1_SS0_GC_PA_5G_MSK                                (((1U<<RF_ANA1_SS0_GC_PA_5G_LEN)-1)<<RF_ANA1_SS0_GC_PA_5G_POS)
#define RF_ANA1_SS0_GC_PA_5G_UMSK                               (~(((1U<<RF_ANA1_SS0_GC_PA_5G_LEN)-1)<<RF_ANA1_SS0_GC_PA_5G_POS))
#define RF_ANA1_SS0_GC_DA_5G_BLD                                RF_ANA1_SS0_GC_DA_5G_BLD
#define RF_ANA1_SS0_GC_DA_5G_BLD_POS                            (20U)
#define RF_ANA1_SS0_GC_DA_5G_BLD_LEN                            (2U)
#define RF_ANA1_SS0_GC_DA_5G_BLD_MSK                            (((1U<<RF_ANA1_SS0_GC_DA_5G_BLD_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_BLD_POS)
#define RF_ANA1_SS0_GC_DA_5G_BLD_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_DA_5G_BLD_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_BLD_POS))
#define RF_ANA1_SS0_GC_DA_5G                                    RF_ANA1_SS0_GC_DA_5G
#define RF_ANA1_SS0_GC_DA_5G_POS                                (24U)
#define RF_ANA1_SS0_GC_DA_5G_LEN                                (3U)
#define RF_ANA1_SS0_GC_DA_5G_MSK                                (((1U<<RF_ANA1_SS0_GC_DA_5G_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_POS)
#define RF_ANA1_SS0_GC_DA_5G_UMSK                               (~(((1U<<RF_ANA1_SS0_GC_DA_5G_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_POS))
#define RF_ANA1_SS0_GC_TMX_5G                                   RF_ANA1_SS0_GC_TMX_5G
#define RF_ANA1_SS0_GC_TMX_5G_POS                               (28U)
#define RF_ANA1_SS0_GC_TMX_5G_LEN                               (4U)
#define RF_ANA1_SS0_GC_TMX_5G_MSK                               (((1U<<RF_ANA1_SS0_GC_TMX_5G_LEN)-1)<<RF_ANA1_SS0_GC_TMX_5G_POS)
#define RF_ANA1_SS0_GC_TMX_5G_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_TMX_5G_LEN)-1)<<RF_ANA1_SS0_GC_TMX_5G_POS))

/* 0x1D8 : ss0_tx_gain0_hw */
#define RF_ANA1_SS0_TX_GAIN0_HW_OFFSET                          (0x1D8)
#define RF_ANA1_SS0_DA_2G_CS_HW                                 RF_ANA1_SS0_DA_2G_CS_HW
#define RF_ANA1_SS0_DA_2G_CS_HW_POS                             (0U)
#define RF_ANA1_SS0_DA_2G_CS_HW_LEN                             (4U)
#define RF_ANA1_SS0_DA_2G_CS_HW_MSK                             (((1U<<RF_ANA1_SS0_DA_2G_CS_HW_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_HW_POS)
#define RF_ANA1_SS0_DA_2G_CS_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_DA_2G_CS_HW_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_HW_POS))
#define RF_ANA1_SS0_TMX_2G_CS_HW                                RF_ANA1_SS0_TMX_2G_CS_HW
#define RF_ANA1_SS0_TMX_2G_CS_HW_POS                            (4U)
#define RF_ANA1_SS0_TMX_2G_CS_HW_LEN                            (5U)
#define RF_ANA1_SS0_TMX_2G_CS_HW_MSK                            (((1U<<RF_ANA1_SS0_TMX_2G_CS_HW_LEN)-1)<<RF_ANA1_SS0_TMX_2G_CS_HW_POS)
#define RF_ANA1_SS0_TMX_2G_CS_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_TMX_2G_CS_HW_LEN)-1)<<RF_ANA1_SS0_TMX_2G_CS_HW_POS))
#define RF_ANA1_SS0_GC_PA_2G_HW                                 RF_ANA1_SS0_GC_PA_2G_HW
#define RF_ANA1_SS0_GC_PA_2G_HW_POS                             (10U)
#define RF_ANA1_SS0_GC_PA_2G_HW_LEN                             (2U)
#define RF_ANA1_SS0_GC_PA_2G_HW_MSK                             (((1U<<RF_ANA1_SS0_GC_PA_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_PA_2G_HW_POS)
#define RF_ANA1_SS0_GC_PA_2G_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_GC_PA_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_PA_2G_HW_POS))
#define RF_ANA1_SS0_DAC_BIAS_SEL_HW                             RF_ANA1_SS0_DAC_BIAS_SEL_HW
#define RF_ANA1_SS0_DAC_BIAS_SEL_HW_POS                         (12U)
#define RF_ANA1_SS0_DAC_BIAS_SEL_HW_LEN                         (2U)
#define RF_ANA1_SS0_DAC_BIAS_SEL_HW_MSK                         (((1U<<RF_ANA1_SS0_DAC_BIAS_SEL_HW_LEN)-1)<<RF_ANA1_SS0_DAC_BIAS_SEL_HW_POS)
#define RF_ANA1_SS0_DAC_BIAS_SEL_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_DAC_BIAS_SEL_HW_LEN)-1)<<RF_ANA1_SS0_DAC_BIAS_SEL_HW_POS))
#define RF_ANA1_SS0_TBB_BM_CG_HW                                RF_ANA1_SS0_TBB_BM_CG_HW
#define RF_ANA1_SS0_TBB_BM_CG_HW_POS                            (16U)
#define RF_ANA1_SS0_TBB_BM_CG_HW_LEN                            (3U)
#define RF_ANA1_SS0_TBB_BM_CG_HW_MSK                            (((1U<<RF_ANA1_SS0_TBB_BM_CG_HW_LEN)-1)<<RF_ANA1_SS0_TBB_BM_CG_HW_POS)
#define RF_ANA1_SS0_TBB_BM_CG_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_TBB_BM_CG_HW_LEN)-1)<<RF_ANA1_SS0_TBB_BM_CG_HW_POS))
#define RF_ANA1_SS0_GC_TMX_2G_HW                                RF_ANA1_SS0_GC_TMX_2G_HW
#define RF_ANA1_SS0_GC_TMX_2G_HW_POS                            (20U)
#define RF_ANA1_SS0_GC_TMX_2G_HW_LEN                            (4U)
#define RF_ANA1_SS0_GC_TMX_2G_HW_MSK                            (((1U<<RF_ANA1_SS0_GC_TMX_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_TMX_2G_HW_POS)
#define RF_ANA1_SS0_GC_TMX_2G_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_TMX_2G_HW_LEN)-1)<<RF_ANA1_SS0_GC_TMX_2G_HW_POS))
#define RF_ANA1_SS0_GC_TBB_HW                                   RF_ANA1_SS0_GC_TBB_HW
#define RF_ANA1_SS0_GC_TBB_HW_POS                               (24U)
#define RF_ANA1_SS0_GC_TBB_HW_LEN                               (6U)
#define RF_ANA1_SS0_GC_TBB_HW_MSK                               (((1U<<RF_ANA1_SS0_GC_TBB_HW_LEN)-1)<<RF_ANA1_SS0_GC_TBB_HW_POS)
#define RF_ANA1_SS0_GC_TBB_HW_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_TBB_HW_LEN)-1)<<RF_ANA1_SS0_GC_TBB_HW_POS))
#define RF_ANA1_SS0_GC_TBB_BOOST_HW                             RF_ANA1_SS0_GC_TBB_BOOST_HW
#define RF_ANA1_SS0_GC_TBB_BOOST_HW_POS                         (30U)
#define RF_ANA1_SS0_GC_TBB_BOOST_HW_LEN                         (2U)
#define RF_ANA1_SS0_GC_TBB_BOOST_HW_MSK                         (((1U<<RF_ANA1_SS0_GC_TBB_BOOST_HW_LEN)-1)<<RF_ANA1_SS0_GC_TBB_BOOST_HW_POS)
#define RF_ANA1_SS0_GC_TBB_BOOST_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_GC_TBB_BOOST_HW_LEN)-1)<<RF_ANA1_SS0_GC_TBB_BOOST_HW_POS))

/* 0x1DC : ss0_tx_gain1_hw */
#define RF_ANA1_SS0_TX_GAIN1_HW_OFFSET                          (0x1DC)
#define RF_ANA1_SS0_DA_5G_CS_HW                                 RF_ANA1_SS0_DA_5G_CS_HW
#define RF_ANA1_SS0_DA_5G_CS_HW_POS                             (0U)
#define RF_ANA1_SS0_DA_5G_CS_HW_LEN                             (5U)
#define RF_ANA1_SS0_DA_5G_CS_HW_MSK                             (((1U<<RF_ANA1_SS0_DA_5G_CS_HW_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_HW_POS)
#define RF_ANA1_SS0_DA_5G_CS_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_DA_5G_CS_HW_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_HW_POS))
#define RF_ANA1_SS0_TMX_5G_CS_HW                                RF_ANA1_SS0_TMX_5G_CS_HW
#define RF_ANA1_SS0_TMX_5G_CS_HW_POS                            (8U)
#define RF_ANA1_SS0_TMX_5G_CS_HW_LEN                            (5U)
#define RF_ANA1_SS0_TMX_5G_CS_HW_MSK                            (((1U<<RF_ANA1_SS0_TMX_5G_CS_HW_LEN)-1)<<RF_ANA1_SS0_TMX_5G_CS_HW_POS)
#define RF_ANA1_SS0_TMX_5G_CS_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_TMX_5G_CS_HW_LEN)-1)<<RF_ANA1_SS0_TMX_5G_CS_HW_POS))
#define RF_ANA1_SS0_GC_PA_5G_HW                                 RF_ANA1_SS0_GC_PA_5G_HW
#define RF_ANA1_SS0_GC_PA_5G_HW_POS                             (16U)
#define RF_ANA1_SS0_GC_PA_5G_HW_LEN                             (2U)
#define RF_ANA1_SS0_GC_PA_5G_HW_MSK                             (((1U<<RF_ANA1_SS0_GC_PA_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_PA_5G_HW_POS)
#define RF_ANA1_SS0_GC_PA_5G_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_GC_PA_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_PA_5G_HW_POS))
#define RF_ANA1_SS0_GC_DA_5G_BLD_HW                             RF_ANA1_SS0_GC_DA_5G_BLD_HW
#define RF_ANA1_SS0_GC_DA_5G_BLD_HW_POS                         (20U)
#define RF_ANA1_SS0_GC_DA_5G_BLD_HW_LEN                         (2U)
#define RF_ANA1_SS0_GC_DA_5G_BLD_HW_MSK                         (((1U<<RF_ANA1_SS0_GC_DA_5G_BLD_HW_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_BLD_HW_POS)
#define RF_ANA1_SS0_GC_DA_5G_BLD_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_GC_DA_5G_BLD_HW_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_BLD_HW_POS))
#define RF_ANA1_SS0_GC_DA_5G_HW                                 RF_ANA1_SS0_GC_DA_5G_HW
#define RF_ANA1_SS0_GC_DA_5G_HW_POS                             (24U)
#define RF_ANA1_SS0_GC_DA_5G_HW_LEN                             (3U)
#define RF_ANA1_SS0_GC_DA_5G_HW_MSK                             (((1U<<RF_ANA1_SS0_GC_DA_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_HW_POS)
#define RF_ANA1_SS0_GC_DA_5G_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_GC_DA_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_DA_5G_HW_POS))
#define RF_ANA1_SS0_GC_TMX_5G_HW                                RF_ANA1_SS0_GC_TMX_5G_HW
#define RF_ANA1_SS0_GC_TMX_5G_HW_POS                            (28U)
#define RF_ANA1_SS0_GC_TMX_5G_HW_LEN                            (4U)
#define RF_ANA1_SS0_GC_TMX_5G_HW_MSK                            (((1U<<RF_ANA1_SS0_GC_TMX_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_TMX_5G_HW_POS)
#define RF_ANA1_SS0_GC_TMX_5G_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_TMX_5G_HW_LEN)-1)<<RF_ANA1_SS0_GC_TMX_5G_HW_POS))

/* 0x1E0 : wf_tmp_reg0 */
#define RF_ANA1_WF_TMP_REG0_OFFSET                              (0x1E0)
#define RF_ANA1_SS0_RXSW_2G_EN_HW                               RF_ANA1_SS0_RXSW_2G_EN_HW
#define RF_ANA1_SS0_RXSW_2G_EN_HW_POS                           (8U)
#define RF_ANA1_SS0_RXSW_2G_EN_HW_LEN                           (2U)
#define RF_ANA1_SS0_RXSW_2G_EN_HW_MSK                           (((1U<<RF_ANA1_SS0_RXSW_2G_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXSW_2G_EN_HW_POS)
#define RF_ANA1_SS0_RXSW_2G_EN_HW_UMSK                          (~(((1U<<RF_ANA1_SS0_RXSW_2G_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXSW_2G_EN_HW_POS))
#define RF_ANA1_SS0_RXSW_5G_EN_HW                               RF_ANA1_SS0_RXSW_5G_EN_HW
#define RF_ANA1_SS0_RXSW_5G_EN_HW_POS                           (10U)
#define RF_ANA1_SS0_RXSW_5G_EN_HW_LEN                           (2U)
#define RF_ANA1_SS0_RXSW_5G_EN_HW_MSK                           (((1U<<RF_ANA1_SS0_RXSW_5G_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXSW_5G_EN_HW_POS)
#define RF_ANA1_SS0_RXSW_5G_EN_HW_UMSK                          (~(((1U<<RF_ANA1_SS0_RXSW_5G_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXSW_5G_EN_HW_POS))
#define RF_ANA1_LOBUF_EN_TESTMODE                               RF_ANA1_LOBUF_EN_TESTMODE
#define RF_ANA1_LOBUF_EN_TESTMODE_POS                           (12U)
#define RF_ANA1_LOBUF_EN_TESTMODE_LEN                           (1U)
#define RF_ANA1_LOBUF_EN_TESTMODE_MSK                           (((1U<<RF_ANA1_LOBUF_EN_TESTMODE_LEN)-1)<<RF_ANA1_LOBUF_EN_TESTMODE_POS)
#define RF_ANA1_LOBUF_EN_TESTMODE_UMSK                          (~(((1U<<RF_ANA1_LOBUF_EN_TESTMODE_LEN)-1)<<RF_ANA1_LOBUF_EN_TESTMODE_POS))

/* 0x200 : ss0_pa_2g_0 */
#define RF_ANA1_SS0_PA_2G_0_OFFSET                              (0x200)
#define RF_ANA1_SS0_PA_2G_VBCAS2                                RF_ANA1_SS0_PA_2G_VBCAS2
#define RF_ANA1_SS0_PA_2G_VBCAS2_POS                            (0U)
#define RF_ANA1_SS0_PA_2G_VBCAS2_LEN                            (3U)
#define RF_ANA1_SS0_PA_2G_VBCAS2_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCAS2_POS)
#define RF_ANA1_SS0_PA_2G_VBCAS2_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCAS2_POS))
#define RF_ANA1_SS0_PA_2G_OP_VREF                               RF_ANA1_SS0_PA_2G_OP_VREF
#define RF_ANA1_SS0_PA_2G_OP_VREF_POS                           (4U)
#define RF_ANA1_SS0_PA_2G_OP_VREF_LEN                           (3U)
#define RF_ANA1_SS0_PA_2G_OP_VREF_MSK                           (((1U<<RF_ANA1_SS0_PA_2G_OP_VREF_LEN)-1)<<RF_ANA1_SS0_PA_2G_OP_VREF_POS)
#define RF_ANA1_SS0_PA_2G_OP_VREF_UMSK                          (~(((1U<<RF_ANA1_SS0_PA_2G_OP_VREF_LEN)-1)<<RF_ANA1_SS0_PA_2G_OP_VREF_POS))
#define RF_ANA1_SS0_PA_2G_LZ_BIAS_EN                            RF_ANA1_SS0_PA_2G_LZ_BIAS_EN
#define RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_POS                        (7U)
#define RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_LEN                        (1U)
#define RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_MSK                        (((1U<<RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_POS)
#define RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_UMSK                       (~(((1U<<RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_SS0_PA_2G_LZ_BIAS_EN_POS))
#define RF_ANA1_SS0_PA_2G_OP_CC                                 RF_ANA1_SS0_PA_2G_OP_CC
#define RF_ANA1_SS0_PA_2G_OP_CC_POS                             (8U)
#define RF_ANA1_SS0_PA_2G_OP_CC_LEN                             (3U)
#define RF_ANA1_SS0_PA_2G_OP_CC_MSK                             (((1U<<RF_ANA1_SS0_PA_2G_OP_CC_LEN)-1)<<RF_ANA1_SS0_PA_2G_OP_CC_POS)
#define RF_ANA1_SS0_PA_2G_OP_CC_UMSK                            (~(((1U<<RF_ANA1_SS0_PA_2G_OP_CC_LEN)-1)<<RF_ANA1_SS0_PA_2G_OP_CC_POS))
#define RF_ANA1_SS0_PA_2G_CN_SEL                                RF_ANA1_SS0_PA_2G_CN_SEL
#define RF_ANA1_SS0_PA_2G_CN_SEL_POS                            (11U)
#define RF_ANA1_SS0_PA_2G_CN_SEL_LEN                            (1U)
#define RF_ANA1_SS0_PA_2G_CN_SEL_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_CN_SEL_LEN)-1)<<RF_ANA1_SS0_PA_2G_CN_SEL_POS)
#define RF_ANA1_SS0_PA_2G_CN_SEL_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_CN_SEL_LEN)-1)<<RF_ANA1_SS0_PA_2G_CN_SEL_POS))
#define RF_ANA1_SS0_PA_2G_IBC_OP                                RF_ANA1_SS0_PA_2G_IBC_OP
#define RF_ANA1_SS0_PA_2G_IBC_OP_POS                            (12U)
#define RF_ANA1_SS0_PA_2G_IBC_OP_LEN                            (3U)
#define RF_ANA1_SS0_PA_2G_IBC_OP_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_IBC_OP_LEN)-1)<<RF_ANA1_SS0_PA_2G_IBC_OP_POS)
#define RF_ANA1_SS0_PA_2G_IBC_OP_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_IBC_OP_LEN)-1)<<RF_ANA1_SS0_PA_2G_IBC_OP_POS))
#define RF_ANA1_SS0_PA_2G_GC_ATT                                RF_ANA1_SS0_PA_2G_GC_ATT
#define RF_ANA1_SS0_PA_2G_GC_ATT_POS                            (16U)
#define RF_ANA1_SS0_PA_2G_GC_ATT_LEN                            (3U)
#define RF_ANA1_SS0_PA_2G_GC_ATT_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_GC_ATT_LEN)-1)<<RF_ANA1_SS0_PA_2G_GC_ATT_POS)
#define RF_ANA1_SS0_PA_2G_GC_ATT_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_GC_ATT_LEN)-1)<<RF_ANA1_SS0_PA_2G_GC_ATT_POS))
#define RF_ANA1_SS0_PA_2G_COUT_SEL                              RF_ANA1_SS0_PA_2G_COUT_SEL
#define RF_ANA1_SS0_PA_2G_COUT_SEL_POS                          (20U)
#define RF_ANA1_SS0_PA_2G_COUT_SEL_LEN                          (3U)
#define RF_ANA1_SS0_PA_2G_COUT_SEL_MSK                          (((1U<<RF_ANA1_SS0_PA_2G_COUT_SEL_LEN)-1)<<RF_ANA1_SS0_PA_2G_COUT_SEL_POS)
#define RF_ANA1_SS0_PA_2G_COUT_SEL_UMSK                         (~(((1U<<RF_ANA1_SS0_PA_2G_COUT_SEL_LEN)-1)<<RF_ANA1_SS0_PA_2G_COUT_SEL_POS))

/* 0x204 : ss0_pa_2g_1 */
#define RF_ANA1_SS0_PA_2G_1_OFFSET                              (0x204)
#define RF_ANA1_SS0_PA_2G_VBCAS1                                RF_ANA1_SS0_PA_2G_VBCAS1
#define RF_ANA1_SS0_PA_2G_VBCAS1_POS                            (0U)
#define RF_ANA1_SS0_PA_2G_VBCAS1_LEN                            (4U)
#define RF_ANA1_SS0_PA_2G_VBCAS1_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCAS1_POS)
#define RF_ANA1_SS0_PA_2G_VBCAS1_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCAS1_POS))
#define RF_ANA1_SS0_PA_2G_IAQ                                   RF_ANA1_SS0_PA_2G_IAQ
#define RF_ANA1_SS0_PA_2G_IAQ_POS                               (4U)
#define RF_ANA1_SS0_PA_2G_IAQ_LEN                               (3U)
#define RF_ANA1_SS0_PA_2G_IAQ_MSK                               (((1U<<RF_ANA1_SS0_PA_2G_IAQ_LEN)-1)<<RF_ANA1_SS0_PA_2G_IAQ_POS)
#define RF_ANA1_SS0_PA_2G_IAQ_UMSK                              (~(((1U<<RF_ANA1_SS0_PA_2G_IAQ_LEN)-1)<<RF_ANA1_SS0_PA_2G_IAQ_POS))
#define RF_ANA1_SS0_PA_2G_IET                                   RF_ANA1_SS0_PA_2G_IET
#define RF_ANA1_SS0_PA_2G_IET_POS                               (8U)
#define RF_ANA1_SS0_PA_2G_IET_LEN                               (5U)
#define RF_ANA1_SS0_PA_2G_IET_MSK                               (((1U<<RF_ANA1_SS0_PA_2G_IET_LEN)-1)<<RF_ANA1_SS0_PA_2G_IET_POS)
#define RF_ANA1_SS0_PA_2G_IET_UMSK                              (~(((1U<<RF_ANA1_SS0_PA_2G_IET_LEN)-1)<<RF_ANA1_SS0_PA_2G_IET_POS))
#define RF_ANA1_SS0_PA_2G_VBCORE                                RF_ANA1_SS0_PA_2G_VBCORE
#define RF_ANA1_SS0_PA_2G_VBCORE_POS                            (16U)
#define RF_ANA1_SS0_PA_2G_VBCORE_LEN                            (5U)
#define RF_ANA1_SS0_PA_2G_VBCORE_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_VBCORE_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCORE_POS)
#define RF_ANA1_SS0_PA_2G_VBCORE_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_VBCORE_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCORE_POS))
#define RF_ANA1_SS0_PA_2G_VBPMOS                                RF_ANA1_SS0_PA_2G_VBPMOS
#define RF_ANA1_SS0_PA_2G_VBPMOS_POS                            (24U)
#define RF_ANA1_SS0_PA_2G_VBPMOS_LEN                            (4U)
#define RF_ANA1_SS0_PA_2G_VBPMOS_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBPMOS_POS)
#define RF_ANA1_SS0_PA_2G_VBPMOS_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBPMOS_POS))
#define RF_ANA1_SS0_PA_2G_BM_CAS2_BULK                          RF_ANA1_SS0_PA_2G_BM_CAS2_BULK
#define RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_POS                      (28U)
#define RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_LEN                      (3U)
#define RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_MSK                      (((1U<<RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_LEN)-1)<<RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_POS)
#define RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_UMSK                     (~(((1U<<RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_LEN)-1)<<RF_ANA1_SS0_PA_2G_BM_CAS2_BULK_POS))
#define RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN                    RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN
#define RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_POS                (31U)
#define RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_LEN                (1U)
#define RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_MSK                (((1U<<RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_LEN)-1)<<RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_POS)
#define RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_UMSK               (~(((1U<<RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_LEN)-1)<<RF_ANA1_SS0_PA_2G_CAS2_BULK_PULLDOWN_POS))

/* 0x208 : ss0_pa_2g_3 */
#define RF_ANA1_SS0_PA_2G_3_OFFSET                              (0x208)
#define RF_ANA1_SS0_PA_2G_VBCAS1_HW                             RF_ANA1_SS0_PA_2G_VBCAS1_HW
#define RF_ANA1_SS0_PA_2G_VBCAS1_HW_POS                         (0U)
#define RF_ANA1_SS0_PA_2G_VBCAS1_HW_LEN                         (4U)
#define RF_ANA1_SS0_PA_2G_VBCAS1_HW_MSK                         (((1U<<RF_ANA1_SS0_PA_2G_VBCAS1_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCAS1_HW_POS)
#define RF_ANA1_SS0_PA_2G_VBCAS1_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_PA_2G_VBCAS1_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCAS1_HW_POS))
#define RF_ANA1_SS0_PA_2G_IAQ_HW                                RF_ANA1_SS0_PA_2G_IAQ_HW
#define RF_ANA1_SS0_PA_2G_IAQ_HW_POS                            (4U)
#define RF_ANA1_SS0_PA_2G_IAQ_HW_LEN                            (3U)
#define RF_ANA1_SS0_PA_2G_IAQ_HW_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_IAQ_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_IAQ_HW_POS)
#define RF_ANA1_SS0_PA_2G_IAQ_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_IAQ_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_IAQ_HW_POS))
#define RF_ANA1_SS0_PA_2G_IET_HW                                RF_ANA1_SS0_PA_2G_IET_HW
#define RF_ANA1_SS0_PA_2G_IET_HW_POS                            (8U)
#define RF_ANA1_SS0_PA_2G_IET_HW_LEN                            (5U)
#define RF_ANA1_SS0_PA_2G_IET_HW_MSK                            (((1U<<RF_ANA1_SS0_PA_2G_IET_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_IET_HW_POS)
#define RF_ANA1_SS0_PA_2G_IET_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_2G_IET_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_IET_HW_POS))
#define RF_ANA1_SS0_PA_2G_VBCORE_HW                             RF_ANA1_SS0_PA_2G_VBCORE_HW
#define RF_ANA1_SS0_PA_2G_VBCORE_HW_POS                         (16U)
#define RF_ANA1_SS0_PA_2G_VBCORE_HW_LEN                         (5U)
#define RF_ANA1_SS0_PA_2G_VBCORE_HW_MSK                         (((1U<<RF_ANA1_SS0_PA_2G_VBCORE_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCORE_HW_POS)
#define RF_ANA1_SS0_PA_2G_VBCORE_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_PA_2G_VBCORE_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBCORE_HW_POS))
#define RF_ANA1_SS0_PA_2G_VBPMOS_HW                             RF_ANA1_SS0_PA_2G_VBPMOS_HW
#define RF_ANA1_SS0_PA_2G_VBPMOS_HW_POS                         (24U)
#define RF_ANA1_SS0_PA_2G_VBPMOS_HW_LEN                         (4U)
#define RF_ANA1_SS0_PA_2G_VBPMOS_HW_MSK                         (((1U<<RF_ANA1_SS0_PA_2G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBPMOS_HW_POS)
#define RF_ANA1_SS0_PA_2G_VBPMOS_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_PA_2G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_PA_2G_VBPMOS_HW_POS))

/* 0x20C : ss0_da_2g_0 */
#define RF_ANA1_SS0_DA_2G_0_OFFSET                              (0x20C)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX                            RF_ANA1_SS0_DA_2G_VBCORE_AUX
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_POS                        (0U)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_LEN                        (4U)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_MSK                        (((1U<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_POS)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_UMSK                       (~(((1U<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_POS))
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN                         RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_POS                     (4U)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_LEN                     (1U)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_MSK                     (((1U<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_POS)
#define RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_UMSK                    (~(((1U<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCORE_AUX_EN_POS))
#define RF_ANA1_SS0_DA_2G_VBCORE                                RF_ANA1_SS0_DA_2G_VBCORE
#define RF_ANA1_SS0_DA_2G_VBCORE_POS                            (8U)
#define RF_ANA1_SS0_DA_2G_VBCORE_LEN                            (4U)
#define RF_ANA1_SS0_DA_2G_VBCORE_MSK                            (((1U<<RF_ANA1_SS0_DA_2G_VBCORE_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCORE_POS)
#define RF_ANA1_SS0_DA_2G_VBCORE_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_2G_VBCORE_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCORE_POS))
#define RF_ANA1_SS0_DA_2G_VBCAS2                                RF_ANA1_SS0_DA_2G_VBCAS2
#define RF_ANA1_SS0_DA_2G_VBCAS2_POS                            (12U)
#define RF_ANA1_SS0_DA_2G_VBCAS2_LEN                            (3U)
#define RF_ANA1_SS0_DA_2G_VBCAS2_MSK                            (((1U<<RF_ANA1_SS0_DA_2G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCAS2_POS)
#define RF_ANA1_SS0_DA_2G_VBCAS2_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_2G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCAS2_POS))
#define RF_ANA1_SS0_DA_2G_VBCAS1                                RF_ANA1_SS0_DA_2G_VBCAS1
#define RF_ANA1_SS0_DA_2G_VBCAS1_POS                            (16U)
#define RF_ANA1_SS0_DA_2G_VBCAS1_LEN                            (4U)
#define RF_ANA1_SS0_DA_2G_VBCAS1_MSK                            (((1U<<RF_ANA1_SS0_DA_2G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCAS1_POS)
#define RF_ANA1_SS0_DA_2G_VBCAS1_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_2G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBCAS1_POS))
#define RF_ANA1_SS0_DA_2G_CS_L                                  RF_ANA1_SS0_DA_2G_CS_L
#define RF_ANA1_SS0_DA_2G_CS_L_POS                              (20U)
#define RF_ANA1_SS0_DA_2G_CS_L_LEN                              (4U)
#define RF_ANA1_SS0_DA_2G_CS_L_MSK                              (((1U<<RF_ANA1_SS0_DA_2G_CS_L_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_L_POS)
#define RF_ANA1_SS0_DA_2G_CS_L_UMSK                             (~(((1U<<RF_ANA1_SS0_DA_2G_CS_L_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_L_POS))
#define RF_ANA1_SS0_DA_2G_CS_H                                  RF_ANA1_SS0_DA_2G_CS_H
#define RF_ANA1_SS0_DA_2G_CS_H_POS                              (24U)
#define RF_ANA1_SS0_DA_2G_CS_H_LEN                              (4U)
#define RF_ANA1_SS0_DA_2G_CS_H_MSK                              (((1U<<RF_ANA1_SS0_DA_2G_CS_H_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_H_POS)
#define RF_ANA1_SS0_DA_2G_CS_H_UMSK                             (~(((1U<<RF_ANA1_SS0_DA_2G_CS_H_LEN)-1)<<RF_ANA1_SS0_DA_2G_CS_H_POS))

/* 0x210 : ss0_tmx_2g */
#define RF_ANA1_SS0_TMX_2G_OFFSET                               (0x210)
#define RF_ANA1_SS0_TMX_2G_VBSW                                 RF_ANA1_SS0_TMX_2G_VBSW
#define RF_ANA1_SS0_TMX_2G_VBSW_POS                             (0U)
#define RF_ANA1_SS0_TMX_2G_VBSW_LEN                             (3U)
#define RF_ANA1_SS0_TMX_2G_VBSW_MSK                             (((1U<<RF_ANA1_SS0_TMX_2G_VBSW_LEN)-1)<<RF_ANA1_SS0_TMX_2G_VBSW_POS)
#define RF_ANA1_SS0_TMX_2G_VBSW_UMSK                            (~(((1U<<RF_ANA1_SS0_TMX_2G_VBSW_LEN)-1)<<RF_ANA1_SS0_TMX_2G_VBSW_POS))
#define RF_ANA1_SS0_TMX_2G_VBCAS                                RF_ANA1_SS0_TMX_2G_VBCAS
#define RF_ANA1_SS0_TMX_2G_VBCAS_POS                            (4U)
#define RF_ANA1_SS0_TMX_2G_VBCAS_LEN                            (3U)
#define RF_ANA1_SS0_TMX_2G_VBCAS_MSK                            (((1U<<RF_ANA1_SS0_TMX_2G_VBCAS_LEN)-1)<<RF_ANA1_SS0_TMX_2G_VBCAS_POS)
#define RF_ANA1_SS0_TMX_2G_VBCAS_UMSK                           (~(((1U<<RF_ANA1_SS0_TMX_2G_VBCAS_LEN)-1)<<RF_ANA1_SS0_TMX_2G_VBCAS_POS))
#define RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE                      RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE
#define RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_POS                  (8U)
#define RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_LEN                  (5U)
#define RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_MSK                  (((1U<<RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_LEN)-1)<<RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_POS)
#define RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_UMSK                 (~(((1U<<RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_LEN)-1)<<RF_ANA1_SS0_TRF_2G_LOCAL_RCAL_CODE_POS))
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN                        RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_POS                    (13U)
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_LEN                    (1U)
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_MSK                    (((1U<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_LEN)-1)<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_POS)
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_UMSK                   (~(((1U<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_LEN)-1)<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_EN_POS))
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL                  RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_POS              (14U)
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_LEN              (1U)
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_MSK              (((1U<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_LEN)-1)<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_POS)
#define RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_UMSK             (~(((1U<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_LEN)-1)<<RF_ANA1_SS0_TRF_2G_LOCAL_PTAT_MODE_SEL_POS))
#define RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE                      RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE
#define RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_POS                  (16U)
#define RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_LEN                  (4U)
#define RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_MSK                  (((1U<<RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_POS)
#define RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_UMSK                 (~(((1U<<RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_DA_2G_LOCAL_PTAT_SCALE_POS))
#define RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE                      RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE
#define RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_POS                  (20U)
#define RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_LEN                  (4U)
#define RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_MSK                  (((1U<<RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_POS)
#define RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_UMSK                 (~(((1U<<RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_PA_2G_LOCAL_PTAT_SCALE_POS))
#define RF_ANA1_SS0_TMX_2G_DEQ                                  RF_ANA1_SS0_TMX_2G_DEQ
#define RF_ANA1_SS0_TMX_2G_DEQ_POS                              (24U)
#define RF_ANA1_SS0_TMX_2G_DEQ_LEN                              (3U)
#define RF_ANA1_SS0_TMX_2G_DEQ_MSK                              (((1U<<RF_ANA1_SS0_TMX_2G_DEQ_LEN)-1)<<RF_ANA1_SS0_TMX_2G_DEQ_POS)
#define RF_ANA1_SS0_TMX_2G_DEQ_UMSK                             (~(((1U<<RF_ANA1_SS0_TMX_2G_DEQ_LEN)-1)<<RF_ANA1_SS0_TMX_2G_DEQ_POS))
#define RF_ANA1_SS0_DA_2G_DEQ                                   RF_ANA1_SS0_DA_2G_DEQ
#define RF_ANA1_SS0_DA_2G_DEQ_POS                               (28U)
#define RF_ANA1_SS0_DA_2G_DEQ_LEN                               (3U)
#define RF_ANA1_SS0_DA_2G_DEQ_MSK                               (((1U<<RF_ANA1_SS0_DA_2G_DEQ_LEN)-1)<<RF_ANA1_SS0_DA_2G_DEQ_POS)
#define RF_ANA1_SS0_DA_2G_DEQ_UMSK                              (~(((1U<<RF_ANA1_SS0_DA_2G_DEQ_LEN)-1)<<RF_ANA1_SS0_DA_2G_DEQ_POS))

/* 0x214 : ss0_selfmx_2g */
#define RF_ANA1_SS0_SELFMX_2G_OFFSET                            (0x214)
#define RF_ANA1_SS0_GC_SELFMX_2G                                RF_ANA1_SS0_GC_SELFMX_2G
#define RF_ANA1_SS0_GC_SELFMX_2G_POS                            (0U)
#define RF_ANA1_SS0_GC_SELFMX_2G_LEN                            (3U)
#define RF_ANA1_SS0_GC_SELFMX_2G_MSK                            (((1U<<RF_ANA1_SS0_GC_SELFMX_2G_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_2G_POS)
#define RF_ANA1_SS0_GC_SELFMX_2G_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_SELFMX_2G_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_2G_POS))
#define RF_ANA1_SS0_GC_SELFMX_2G_BOOST                          RF_ANA1_SS0_GC_SELFMX_2G_BOOST
#define RF_ANA1_SS0_GC_SELFMX_2G_BOOST_POS                      (3U)
#define RF_ANA1_SS0_GC_SELFMX_2G_BOOST_LEN                      (1U)
#define RF_ANA1_SS0_GC_SELFMX_2G_BOOST_MSK                      (((1U<<RF_ANA1_SS0_GC_SELFMX_2G_BOOST_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_2G_BOOST_POS)
#define RF_ANA1_SS0_GC_SELFMX_2G_BOOST_UMSK                     (~(((1U<<RF_ANA1_SS0_GC_SELFMX_2G_BOOST_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_2G_BOOST_POS))
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_BM                          RF_ANA1_SS0_SELFMX_2G_OSDAC_BM
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_POS                      (4U)
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_LEN                      (2U)
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_MSK                      (((1U<<RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_POS)
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_UMSK                     (~(((1U<<RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_2G_OSDAC_BM_POS))
#define RF_ANA1_SS0_SELFMX_2G_BM                                RF_ANA1_SS0_SELFMX_2G_BM
#define RF_ANA1_SS0_SELFMX_2G_BM_POS                            (8U)
#define RF_ANA1_SS0_SELFMX_2G_BM_LEN                            (3U)
#define RF_ANA1_SS0_SELFMX_2G_BM_MSK                            (((1U<<RF_ANA1_SS0_SELFMX_2G_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_2G_BM_POS)
#define RF_ANA1_SS0_SELFMX_2G_BM_UMSK                           (~(((1U<<RF_ANA1_SS0_SELFMX_2G_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_2G_BM_POS))
#define RF_ANA1_SS0_SELFMX_2G_OSDAC                             RF_ANA1_SS0_SELFMX_2G_OSDAC
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_POS                         (12U)
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_LEN                         (6U)
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_MSK                         (((1U<<RF_ANA1_SS0_SELFMX_2G_OSDAC_LEN)-1)<<RF_ANA1_SS0_SELFMX_2G_OSDAC_POS)
#define RF_ANA1_SS0_SELFMX_2G_OSDAC_UMSK                        (~(((1U<<RF_ANA1_SS0_SELFMX_2G_OSDAC_LEN)-1)<<RF_ANA1_SS0_SELFMX_2G_OSDAC_POS))
#define RF_ANA1_SS0_DA_2G_VBPMOS                                RF_ANA1_SS0_DA_2G_VBPMOS
#define RF_ANA1_SS0_DA_2G_VBPMOS_POS                            (20U)
#define RF_ANA1_SS0_DA_2G_VBPMOS_LEN                            (4U)
#define RF_ANA1_SS0_DA_2G_VBPMOS_MSK                            (((1U<<RF_ANA1_SS0_DA_2G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBPMOS_POS)
#define RF_ANA1_SS0_DA_2G_VBPMOS_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_2G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBPMOS_POS))
#define RF_ANA1_SS0_DA_2G_VBPMOS_HW                             RF_ANA1_SS0_DA_2G_VBPMOS_HW
#define RF_ANA1_SS0_DA_2G_VBPMOS_HW_POS                         (24U)
#define RF_ANA1_SS0_DA_2G_VBPMOS_HW_LEN                         (4U)
#define RF_ANA1_SS0_DA_2G_VBPMOS_HW_MSK                         (((1U<<RF_ANA1_SS0_DA_2G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBPMOS_HW_POS)
#define RF_ANA1_SS0_DA_2G_VBPMOS_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_DA_2G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_DA_2G_VBPMOS_HW_POS))

/* 0x218 : ss0_pwrdet_2g_0 */
#define RF_ANA1_SS0_PWRDET_2G_0_OFFSET                          (0x218)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN                      RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_POS                  (1U)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_LEN                  (1U)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_MSK                  (((1U<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_POS)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_UMSK                 (~(((1U<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_POS))
#define RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT                        RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT
#define RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_POS                    (4U)
#define RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_LEN                    (2U)
#define RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_MSK                    (((1U<<RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_POS)
#define RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_UMSK                   (~(((1U<<RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_2G_MUL_ATT_POS))
#define RF_ANA1_SS0_GC_PWRDET_2G_ATT                            RF_ANA1_SS0_GC_PWRDET_2G_ATT
#define RF_ANA1_SS0_GC_PWRDET_2G_ATT_POS                        (8U)
#define RF_ANA1_SS0_GC_PWRDET_2G_ATT_LEN                        (3U)
#define RF_ANA1_SS0_GC_PWRDET_2G_ATT_MSK                        (((1U<<RF_ANA1_SS0_GC_PWRDET_2G_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_2G_ATT_POS)
#define RF_ANA1_SS0_GC_PWRDET_2G_ATT_UMSK                       (~(((1U<<RF_ANA1_SS0_GC_PWRDET_2G_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_2G_ATT_POS))
#define RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE                        RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE
#define RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_POS                    (12U)
#define RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_LEN                    (5U)
#define RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_MSK                    (((1U<<RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_POS)
#define RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_UMSK                   (~(((1U<<RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_OSDAC_CODE_POS))
#define RF_ANA1_SS0_PWRDET_2G_MUL_BM                            RF_ANA1_SS0_PWRDET_2G_MUL_BM
#define RF_ANA1_SS0_PWRDET_2G_MUL_BM_POS                        (20U)
#define RF_ANA1_SS0_PWRDET_2G_MUL_BM_LEN                        (3U)
#define RF_ANA1_SS0_PWRDET_2G_MUL_BM_MSK                        (((1U<<RF_ANA1_SS0_PWRDET_2G_MUL_BM_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_MUL_BM_POS)
#define RF_ANA1_SS0_PWRDET_2G_MUL_BM_UMSK                       (~(((1U<<RF_ANA1_SS0_PWRDET_2G_MUL_BM_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_MUL_BM_POS))

/* 0x21C : ss0_pwrdet_2g_1 */
#define RF_ANA1_SS0_PWRDET_2G_1_OFFSET                          (0x21C)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW                   RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_POS               (1U)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_LEN               (1U)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_MSK               (((1U<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_POS)
#define RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_UMSK              (~(((1U<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_LEN)-1)<<RF_ANA1_SS0_PWRDET_2G_MUL_SHORT_EN_HW_POS))

/* 0x220 : ss0_pwrdet_2g_2 */
#define RF_ANA1_SS0_PWRDET_2G_2_OFFSET                          (0x220)

/* 0x224 : ss0_pa_5g_0 */
#define RF_ANA1_SS0_PA_5G_0_OFFSET                              (0x224)
#define RF_ANA1_SS0_PA_5G_VBCAS2                                RF_ANA1_SS0_PA_5G_VBCAS2
#define RF_ANA1_SS0_PA_5G_VBCAS2_POS                            (0U)
#define RF_ANA1_SS0_PA_5G_VBCAS2_LEN                            (3U)
#define RF_ANA1_SS0_PA_5G_VBCAS2_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCAS2_POS)
#define RF_ANA1_SS0_PA_5G_VBCAS2_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCAS2_POS))
#define RF_ANA1_SS0_PA_5G_OP_VREF                               RF_ANA1_SS0_PA_5G_OP_VREF
#define RF_ANA1_SS0_PA_5G_OP_VREF_POS                           (4U)
#define RF_ANA1_SS0_PA_5G_OP_VREF_LEN                           (3U)
#define RF_ANA1_SS0_PA_5G_OP_VREF_MSK                           (((1U<<RF_ANA1_SS0_PA_5G_OP_VREF_LEN)-1)<<RF_ANA1_SS0_PA_5G_OP_VREF_POS)
#define RF_ANA1_SS0_PA_5G_OP_VREF_UMSK                          (~(((1U<<RF_ANA1_SS0_PA_5G_OP_VREF_LEN)-1)<<RF_ANA1_SS0_PA_5G_OP_VREF_POS))
#define RF_ANA1_SS0_PA_5G_LZ_BIAS_EN                            RF_ANA1_SS0_PA_5G_LZ_BIAS_EN
#define RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_POS                        (7U)
#define RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_LEN                        (1U)
#define RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_MSK                        (((1U<<RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_POS)
#define RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_UMSK                       (~(((1U<<RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_LEN)-1)<<RF_ANA1_SS0_PA_5G_LZ_BIAS_EN_POS))
#define RF_ANA1_SS0_PA_5G_OP_CC                                 RF_ANA1_SS0_PA_5G_OP_CC
#define RF_ANA1_SS0_PA_5G_OP_CC_POS                             (8U)
#define RF_ANA1_SS0_PA_5G_OP_CC_LEN                             (3U)
#define RF_ANA1_SS0_PA_5G_OP_CC_MSK                             (((1U<<RF_ANA1_SS0_PA_5G_OP_CC_LEN)-1)<<RF_ANA1_SS0_PA_5G_OP_CC_POS)
#define RF_ANA1_SS0_PA_5G_OP_CC_UMSK                            (~(((1U<<RF_ANA1_SS0_PA_5G_OP_CC_LEN)-1)<<RF_ANA1_SS0_PA_5G_OP_CC_POS))
#define RF_ANA1_SS0_PA_5G_CN_SEL                                RF_ANA1_SS0_PA_5G_CN_SEL
#define RF_ANA1_SS0_PA_5G_CN_SEL_POS                            (11U)
#define RF_ANA1_SS0_PA_5G_CN_SEL_LEN                            (1U)
#define RF_ANA1_SS0_PA_5G_CN_SEL_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_CN_SEL_LEN)-1)<<RF_ANA1_SS0_PA_5G_CN_SEL_POS)
#define RF_ANA1_SS0_PA_5G_CN_SEL_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_CN_SEL_LEN)-1)<<RF_ANA1_SS0_PA_5G_CN_SEL_POS))
#define RF_ANA1_SS0_PA_5G_IBC_OP                                RF_ANA1_SS0_PA_5G_IBC_OP
#define RF_ANA1_SS0_PA_5G_IBC_OP_POS                            (12U)
#define RF_ANA1_SS0_PA_5G_IBC_OP_LEN                            (3U)
#define RF_ANA1_SS0_PA_5G_IBC_OP_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_IBC_OP_LEN)-1)<<RF_ANA1_SS0_PA_5G_IBC_OP_POS)
#define RF_ANA1_SS0_PA_5G_IBC_OP_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_IBC_OP_LEN)-1)<<RF_ANA1_SS0_PA_5G_IBC_OP_POS))
#define RF_ANA1_SS0_PA_5G_GC_ATT                                RF_ANA1_SS0_PA_5G_GC_ATT
#define RF_ANA1_SS0_PA_5G_GC_ATT_POS                            (16U)
#define RF_ANA1_SS0_PA_5G_GC_ATT_LEN                            (3U)
#define RF_ANA1_SS0_PA_5G_GC_ATT_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_GC_ATT_LEN)-1)<<RF_ANA1_SS0_PA_5G_GC_ATT_POS)
#define RF_ANA1_SS0_PA_5G_GC_ATT_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_GC_ATT_LEN)-1)<<RF_ANA1_SS0_PA_5G_GC_ATT_POS))
#define RF_ANA1_SS0_PA_5G_COUT_SEL                              RF_ANA1_SS0_PA_5G_COUT_SEL
#define RF_ANA1_SS0_PA_5G_COUT_SEL_POS                          (20U)
#define RF_ANA1_SS0_PA_5G_COUT_SEL_LEN                          (3U)
#define RF_ANA1_SS0_PA_5G_COUT_SEL_MSK                          (((1U<<RF_ANA1_SS0_PA_5G_COUT_SEL_LEN)-1)<<RF_ANA1_SS0_PA_5G_COUT_SEL_POS)
#define RF_ANA1_SS0_PA_5G_COUT_SEL_UMSK                         (~(((1U<<RF_ANA1_SS0_PA_5G_COUT_SEL_LEN)-1)<<RF_ANA1_SS0_PA_5G_COUT_SEL_POS))
#define RF_ANA1_SS0_PA_5G_CD_SEL                                RF_ANA1_SS0_PA_5G_CD_SEL
#define RF_ANA1_SS0_PA_5G_CD_SEL_POS                            (24U)
#define RF_ANA1_SS0_PA_5G_CD_SEL_LEN                            (3U)
#define RF_ANA1_SS0_PA_5G_CD_SEL_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_CD_SEL_LEN)-1)<<RF_ANA1_SS0_PA_5G_CD_SEL_POS)
#define RF_ANA1_SS0_PA_5G_CD_SEL_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_CD_SEL_LEN)-1)<<RF_ANA1_SS0_PA_5G_CD_SEL_POS))

/* 0x228 : ss0_pa_5g_1 */
#define RF_ANA1_SS0_PA_5G_1_OFFSET                              (0x228)
#define RF_ANA1_SS0_PA_5G_VBCAS1                                RF_ANA1_SS0_PA_5G_VBCAS1
#define RF_ANA1_SS0_PA_5G_VBCAS1_POS                            (0U)
#define RF_ANA1_SS0_PA_5G_VBCAS1_LEN                            (4U)
#define RF_ANA1_SS0_PA_5G_VBCAS1_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCAS1_POS)
#define RF_ANA1_SS0_PA_5G_VBCAS1_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCAS1_POS))
#define RF_ANA1_SS0_PA_5G_IAQ                                   RF_ANA1_SS0_PA_5G_IAQ
#define RF_ANA1_SS0_PA_5G_IAQ_POS                               (4U)
#define RF_ANA1_SS0_PA_5G_IAQ_LEN                               (3U)
#define RF_ANA1_SS0_PA_5G_IAQ_MSK                               (((1U<<RF_ANA1_SS0_PA_5G_IAQ_LEN)-1)<<RF_ANA1_SS0_PA_5G_IAQ_POS)
#define RF_ANA1_SS0_PA_5G_IAQ_UMSK                              (~(((1U<<RF_ANA1_SS0_PA_5G_IAQ_LEN)-1)<<RF_ANA1_SS0_PA_5G_IAQ_POS))
#define RF_ANA1_SS0_PA_5G_IET                                   RF_ANA1_SS0_PA_5G_IET
#define RF_ANA1_SS0_PA_5G_IET_POS                               (8U)
#define RF_ANA1_SS0_PA_5G_IET_LEN                               (5U)
#define RF_ANA1_SS0_PA_5G_IET_MSK                               (((1U<<RF_ANA1_SS0_PA_5G_IET_LEN)-1)<<RF_ANA1_SS0_PA_5G_IET_POS)
#define RF_ANA1_SS0_PA_5G_IET_UMSK                              (~(((1U<<RF_ANA1_SS0_PA_5G_IET_LEN)-1)<<RF_ANA1_SS0_PA_5G_IET_POS))
#define RF_ANA1_SS0_PA_5G_VBCORE                                RF_ANA1_SS0_PA_5G_VBCORE
#define RF_ANA1_SS0_PA_5G_VBCORE_POS                            (16U)
#define RF_ANA1_SS0_PA_5G_VBCORE_LEN                            (5U)
#define RF_ANA1_SS0_PA_5G_VBCORE_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_VBCORE_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCORE_POS)
#define RF_ANA1_SS0_PA_5G_VBCORE_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_VBCORE_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCORE_POS))
#define RF_ANA1_SS0_PA_5G_VBPMOS                                RF_ANA1_SS0_PA_5G_VBPMOS
#define RF_ANA1_SS0_PA_5G_VBPMOS_POS                            (24U)
#define RF_ANA1_SS0_PA_5G_VBPMOS_LEN                            (4U)
#define RF_ANA1_SS0_PA_5G_VBPMOS_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBPMOS_POS)
#define RF_ANA1_SS0_PA_5G_VBPMOS_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBPMOS_POS))
#define RF_ANA1_SS0_PA_5G_BM_CAS2_BULK                          RF_ANA1_SS0_PA_5G_BM_CAS2_BULK
#define RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_POS                      (28U)
#define RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_LEN                      (3U)
#define RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_MSK                      (((1U<<RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_LEN)-1)<<RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_POS)
#define RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_UMSK                     (~(((1U<<RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_LEN)-1)<<RF_ANA1_SS0_PA_5G_BM_CAS2_BULK_POS))
#define RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN                    RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN
#define RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_POS                (31U)
#define RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_LEN                (1U)
#define RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_MSK                (((1U<<RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_LEN)-1)<<RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_POS)
#define RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_UMSK               (~(((1U<<RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_LEN)-1)<<RF_ANA1_SS0_PA_5G_CAS2_BULK_PULLDOWN_POS))

/* 0x22C : ss0_pa_5g_3 */
#define RF_ANA1_SS0_PA_5G_3_OFFSET                              (0x22C)
#define RF_ANA1_SS0_PA_5G_VBCAS1_HW                             RF_ANA1_SS0_PA_5G_VBCAS1_HW
#define RF_ANA1_SS0_PA_5G_VBCAS1_HW_POS                         (0U)
#define RF_ANA1_SS0_PA_5G_VBCAS1_HW_LEN                         (4U)
#define RF_ANA1_SS0_PA_5G_VBCAS1_HW_MSK                         (((1U<<RF_ANA1_SS0_PA_5G_VBCAS1_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCAS1_HW_POS)
#define RF_ANA1_SS0_PA_5G_VBCAS1_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_PA_5G_VBCAS1_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCAS1_HW_POS))
#define RF_ANA1_SS0_PA_5G_IAQ_HW                                RF_ANA1_SS0_PA_5G_IAQ_HW
#define RF_ANA1_SS0_PA_5G_IAQ_HW_POS                            (4U)
#define RF_ANA1_SS0_PA_5G_IAQ_HW_LEN                            (3U)
#define RF_ANA1_SS0_PA_5G_IAQ_HW_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_IAQ_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_IAQ_HW_POS)
#define RF_ANA1_SS0_PA_5G_IAQ_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_IAQ_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_IAQ_HW_POS))
#define RF_ANA1_SS0_PA_5G_IET_HW                                RF_ANA1_SS0_PA_5G_IET_HW
#define RF_ANA1_SS0_PA_5G_IET_HW_POS                            (8U)
#define RF_ANA1_SS0_PA_5G_IET_HW_LEN                            (5U)
#define RF_ANA1_SS0_PA_5G_IET_HW_MSK                            (((1U<<RF_ANA1_SS0_PA_5G_IET_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_IET_HW_POS)
#define RF_ANA1_SS0_PA_5G_IET_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_5G_IET_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_IET_HW_POS))
#define RF_ANA1_SS0_PA_5G_VBCORE_HW                             RF_ANA1_SS0_PA_5G_VBCORE_HW
#define RF_ANA1_SS0_PA_5G_VBCORE_HW_POS                         (16U)
#define RF_ANA1_SS0_PA_5G_VBCORE_HW_LEN                         (5U)
#define RF_ANA1_SS0_PA_5G_VBCORE_HW_MSK                         (((1U<<RF_ANA1_SS0_PA_5G_VBCORE_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCORE_HW_POS)
#define RF_ANA1_SS0_PA_5G_VBCORE_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_PA_5G_VBCORE_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBCORE_HW_POS))
#define RF_ANA1_SS0_PA_5G_VBPMOS_HW                             RF_ANA1_SS0_PA_5G_VBPMOS_HW
#define RF_ANA1_SS0_PA_5G_VBPMOS_HW_POS                         (24U)
#define RF_ANA1_SS0_PA_5G_VBPMOS_HW_LEN                         (4U)
#define RF_ANA1_SS0_PA_5G_VBPMOS_HW_MSK                         (((1U<<RF_ANA1_SS0_PA_5G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBPMOS_HW_POS)
#define RF_ANA1_SS0_PA_5G_VBPMOS_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_PA_5G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_PA_5G_VBPMOS_HW_POS))

/* 0x230 : ss0_da_5g_0 */
#define RF_ANA1_SS0_DA_5G_0_OFFSET                              (0x230)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX                            RF_ANA1_SS0_DA_5G_VBCORE_AUX
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_POS                        (0U)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_LEN                        (4U)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_MSK                        (((1U<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_POS)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_UMSK                       (~(((1U<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_POS))
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN                         RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_POS                     (4U)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_LEN                     (1U)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_MSK                     (((1U<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_POS)
#define RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_UMSK                    (~(((1U<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCORE_AUX_EN_POS))
#define RF_ANA1_SS0_DA_5G_VBCORE                                RF_ANA1_SS0_DA_5G_VBCORE
#define RF_ANA1_SS0_DA_5G_VBCORE_POS                            (8U)
#define RF_ANA1_SS0_DA_5G_VBCORE_LEN                            (4U)
#define RF_ANA1_SS0_DA_5G_VBCORE_MSK                            (((1U<<RF_ANA1_SS0_DA_5G_VBCORE_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCORE_POS)
#define RF_ANA1_SS0_DA_5G_VBCORE_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_5G_VBCORE_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCORE_POS))
#define RF_ANA1_SS0_DA_5G_VBCAS2                                RF_ANA1_SS0_DA_5G_VBCAS2
#define RF_ANA1_SS0_DA_5G_VBCAS2_POS                            (12U)
#define RF_ANA1_SS0_DA_5G_VBCAS2_LEN                            (3U)
#define RF_ANA1_SS0_DA_5G_VBCAS2_MSK                            (((1U<<RF_ANA1_SS0_DA_5G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCAS2_POS)
#define RF_ANA1_SS0_DA_5G_VBCAS2_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_5G_VBCAS2_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCAS2_POS))
#define RF_ANA1_SS0_DA_5G_VBCAS1                                RF_ANA1_SS0_DA_5G_VBCAS1
#define RF_ANA1_SS0_DA_5G_VBCAS1_POS                            (16U)
#define RF_ANA1_SS0_DA_5G_VBCAS1_LEN                            (4U)
#define RF_ANA1_SS0_DA_5G_VBCAS1_MSK                            (((1U<<RF_ANA1_SS0_DA_5G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCAS1_POS)
#define RF_ANA1_SS0_DA_5G_VBCAS1_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_5G_VBCAS1_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBCAS1_POS))
#define RF_ANA1_SS0_DA_5G_CS_L                                  RF_ANA1_SS0_DA_5G_CS_L
#define RF_ANA1_SS0_DA_5G_CS_L_POS                              (20U)
#define RF_ANA1_SS0_DA_5G_CS_L_LEN                              (5U)
#define RF_ANA1_SS0_DA_5G_CS_L_MSK                              (((1U<<RF_ANA1_SS0_DA_5G_CS_L_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_L_POS)
#define RF_ANA1_SS0_DA_5G_CS_L_UMSK                             (~(((1U<<RF_ANA1_SS0_DA_5G_CS_L_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_L_POS))
#define RF_ANA1_SS0_DA_5G_CS_H                                  RF_ANA1_SS0_DA_5G_CS_H
#define RF_ANA1_SS0_DA_5G_CS_H_POS                              (26U)
#define RF_ANA1_SS0_DA_5G_CS_H_LEN                              (5U)
#define RF_ANA1_SS0_DA_5G_CS_H_MSK                              (((1U<<RF_ANA1_SS0_DA_5G_CS_H_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_H_POS)
#define RF_ANA1_SS0_DA_5G_CS_H_UMSK                             (~(((1U<<RF_ANA1_SS0_DA_5G_CS_H_LEN)-1)<<RF_ANA1_SS0_DA_5G_CS_H_POS))

/* 0x234 : ss0_tmx_5g */
#define RF_ANA1_SS0_TMX_5G_OFFSET                               (0x234)
#define RF_ANA1_SS0_TMX_5G_VBSW                                 RF_ANA1_SS0_TMX_5G_VBSW
#define RF_ANA1_SS0_TMX_5G_VBSW_POS                             (0U)
#define RF_ANA1_SS0_TMX_5G_VBSW_LEN                             (3U)
#define RF_ANA1_SS0_TMX_5G_VBSW_MSK                             (((1U<<RF_ANA1_SS0_TMX_5G_VBSW_LEN)-1)<<RF_ANA1_SS0_TMX_5G_VBSW_POS)
#define RF_ANA1_SS0_TMX_5G_VBSW_UMSK                            (~(((1U<<RF_ANA1_SS0_TMX_5G_VBSW_LEN)-1)<<RF_ANA1_SS0_TMX_5G_VBSW_POS))
#define RF_ANA1_SS0_TMX_5G_VBCAS                                RF_ANA1_SS0_TMX_5G_VBCAS
#define RF_ANA1_SS0_TMX_5G_VBCAS_POS                            (4U)
#define RF_ANA1_SS0_TMX_5G_VBCAS_LEN                            (3U)
#define RF_ANA1_SS0_TMX_5G_VBCAS_MSK                            (((1U<<RF_ANA1_SS0_TMX_5G_VBCAS_LEN)-1)<<RF_ANA1_SS0_TMX_5G_VBCAS_POS)
#define RF_ANA1_SS0_TMX_5G_VBCAS_UMSK                           (~(((1U<<RF_ANA1_SS0_TMX_5G_VBCAS_LEN)-1)<<RF_ANA1_SS0_TMX_5G_VBCAS_POS))
#define RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE                      RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE
#define RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_POS                  (8U)
#define RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_LEN                  (5U)
#define RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_MSK                  (((1U<<RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_LEN)-1)<<RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_POS)
#define RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_UMSK                 (~(((1U<<RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_LEN)-1)<<RF_ANA1_SS0_TRF_5G_LOCAL_RCAL_CODE_POS))
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN                        RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_POS                    (13U)
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_LEN                    (1U)
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_MSK                    (((1U<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_LEN)-1)<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_POS)
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_UMSK                   (~(((1U<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_LEN)-1)<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_EN_POS))
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL                  RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_POS              (14U)
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_LEN              (1U)
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_MSK              (((1U<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_LEN)-1)<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_POS)
#define RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_UMSK             (~(((1U<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_LEN)-1)<<RF_ANA1_SS0_TRF_5G_LOCAL_PTAT_MODE_SEL_POS))
#define RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE                      RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE
#define RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_POS                  (16U)
#define RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_LEN                  (4U)
#define RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_MSK                  (((1U<<RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_POS)
#define RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_UMSK                 (~(((1U<<RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_DA_5G_LOCAL_PTAT_SCALE_POS))
#define RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE                      RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE
#define RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_POS                  (20U)
#define RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_LEN                  (4U)
#define RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_MSK                  (((1U<<RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_POS)
#define RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_UMSK                 (~(((1U<<RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_LEN)-1)<<RF_ANA1_SS0_PA_5G_LOCAL_PTAT_SCALE_POS))
#define RF_ANA1_SS0_TMX_5G_DEQ                                  RF_ANA1_SS0_TMX_5G_DEQ
#define RF_ANA1_SS0_TMX_5G_DEQ_POS                              (24U)
#define RF_ANA1_SS0_TMX_5G_DEQ_LEN                              (3U)
#define RF_ANA1_SS0_TMX_5G_DEQ_MSK                              (((1U<<RF_ANA1_SS0_TMX_5G_DEQ_LEN)-1)<<RF_ANA1_SS0_TMX_5G_DEQ_POS)
#define RF_ANA1_SS0_TMX_5G_DEQ_UMSK                             (~(((1U<<RF_ANA1_SS0_TMX_5G_DEQ_LEN)-1)<<RF_ANA1_SS0_TMX_5G_DEQ_POS))
#define RF_ANA1_SS0_DA_5G_DEQ                                   RF_ANA1_SS0_DA_5G_DEQ
#define RF_ANA1_SS0_DA_5G_DEQ_POS                               (28U)
#define RF_ANA1_SS0_DA_5G_DEQ_LEN                               (3U)
#define RF_ANA1_SS0_DA_5G_DEQ_MSK                               (((1U<<RF_ANA1_SS0_DA_5G_DEQ_LEN)-1)<<RF_ANA1_SS0_DA_5G_DEQ_POS)
#define RF_ANA1_SS0_DA_5G_DEQ_UMSK                              (~(((1U<<RF_ANA1_SS0_DA_5G_DEQ_LEN)-1)<<RF_ANA1_SS0_DA_5G_DEQ_POS))

/* 0x238 : ss0_selfmx_5g */
#define RF_ANA1_SS0_SELFMX_5G_OFFSET                            (0x238)
#define RF_ANA1_SS0_GC_SELFMX_5G                                RF_ANA1_SS0_GC_SELFMX_5G
#define RF_ANA1_SS0_GC_SELFMX_5G_POS                            (0U)
#define RF_ANA1_SS0_GC_SELFMX_5G_LEN                            (3U)
#define RF_ANA1_SS0_GC_SELFMX_5G_MSK                            (((1U<<RF_ANA1_SS0_GC_SELFMX_5G_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_5G_POS)
#define RF_ANA1_SS0_GC_SELFMX_5G_UMSK                           (~(((1U<<RF_ANA1_SS0_GC_SELFMX_5G_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_5G_POS))
#define RF_ANA1_SS0_GC_SELFMX_5G_BOOST                          RF_ANA1_SS0_GC_SELFMX_5G_BOOST
#define RF_ANA1_SS0_GC_SELFMX_5G_BOOST_POS                      (3U)
#define RF_ANA1_SS0_GC_SELFMX_5G_BOOST_LEN                      (1U)
#define RF_ANA1_SS0_GC_SELFMX_5G_BOOST_MSK                      (((1U<<RF_ANA1_SS0_GC_SELFMX_5G_BOOST_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_5G_BOOST_POS)
#define RF_ANA1_SS0_GC_SELFMX_5G_BOOST_UMSK                     (~(((1U<<RF_ANA1_SS0_GC_SELFMX_5G_BOOST_LEN)-1)<<RF_ANA1_SS0_GC_SELFMX_5G_BOOST_POS))
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_BM                          RF_ANA1_SS0_SELFMX_5G_OSDAC_BM
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_POS                      (4U)
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_LEN                      (2U)
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_MSK                      (((1U<<RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_POS)
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_UMSK                     (~(((1U<<RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_5G_OSDAC_BM_POS))
#define RF_ANA1_SS0_SELFMX_5G_BM                                RF_ANA1_SS0_SELFMX_5G_BM
#define RF_ANA1_SS0_SELFMX_5G_BM_POS                            (8U)
#define RF_ANA1_SS0_SELFMX_5G_BM_LEN                            (3U)
#define RF_ANA1_SS0_SELFMX_5G_BM_MSK                            (((1U<<RF_ANA1_SS0_SELFMX_5G_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_5G_BM_POS)
#define RF_ANA1_SS0_SELFMX_5G_BM_UMSK                           (~(((1U<<RF_ANA1_SS0_SELFMX_5G_BM_LEN)-1)<<RF_ANA1_SS0_SELFMX_5G_BM_POS))
#define RF_ANA1_SS0_SELFMX_5G_OSDAC                             RF_ANA1_SS0_SELFMX_5G_OSDAC
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_POS                         (12U)
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_LEN                         (6U)
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_MSK                         (((1U<<RF_ANA1_SS0_SELFMX_5G_OSDAC_LEN)-1)<<RF_ANA1_SS0_SELFMX_5G_OSDAC_POS)
#define RF_ANA1_SS0_SELFMX_5G_OSDAC_UMSK                        (~(((1U<<RF_ANA1_SS0_SELFMX_5G_OSDAC_LEN)-1)<<RF_ANA1_SS0_SELFMX_5G_OSDAC_POS))
#define RF_ANA1_SS0_DA_5G_VBPMOS                                RF_ANA1_SS0_DA_5G_VBPMOS
#define RF_ANA1_SS0_DA_5G_VBPMOS_POS                            (20U)
#define RF_ANA1_SS0_DA_5G_VBPMOS_LEN                            (4U)
#define RF_ANA1_SS0_DA_5G_VBPMOS_MSK                            (((1U<<RF_ANA1_SS0_DA_5G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBPMOS_POS)
#define RF_ANA1_SS0_DA_5G_VBPMOS_UMSK                           (~(((1U<<RF_ANA1_SS0_DA_5G_VBPMOS_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBPMOS_POS))
#define RF_ANA1_SS0_DA_5G_VBPMOS_HW                             RF_ANA1_SS0_DA_5G_VBPMOS_HW
#define RF_ANA1_SS0_DA_5G_VBPMOS_HW_POS                         (24U)
#define RF_ANA1_SS0_DA_5G_VBPMOS_HW_LEN                         (4U)
#define RF_ANA1_SS0_DA_5G_VBPMOS_HW_MSK                         (((1U<<RF_ANA1_SS0_DA_5G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBPMOS_HW_POS)
#define RF_ANA1_SS0_DA_5G_VBPMOS_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_DA_5G_VBPMOS_HW_LEN)-1)<<RF_ANA1_SS0_DA_5G_VBPMOS_HW_POS))

/* 0x23C : ss0_pwrdet_5g_0 */
#define RF_ANA1_SS0_PWRDET_5G_0_OFFSET                          (0x23C)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN                             RF_ANA1_SS0_PWRDET_5G_PR_EN
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_POS                         (1U)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_LEN                         (1U)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_MSK                         (((1U<<RF_ANA1_SS0_PWRDET_5G_PR_EN_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_EN_POS)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_UMSK                        (~(((1U<<RF_ANA1_SS0_PWRDET_5G_PR_EN_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_EN_POS))
#define RF_ANA1_SS0_GC_PWRDET_5G_PR                             RF_ANA1_SS0_GC_PWRDET_5G_PR
#define RF_ANA1_SS0_GC_PWRDET_5G_PR_POS                         (3U)
#define RF_ANA1_SS0_GC_PWRDET_5G_PR_LEN                         (1U)
#define RF_ANA1_SS0_GC_PWRDET_5G_PR_MSK                         (((1U<<RF_ANA1_SS0_GC_PWRDET_5G_PR_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_5G_PR_POS)
#define RF_ANA1_SS0_GC_PWRDET_5G_PR_UMSK                        (~(((1U<<RF_ANA1_SS0_GC_PWRDET_5G_PR_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_5G_PR_POS))
#define RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT                        RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT
#define RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_POS                    (4U)
#define RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_LEN                    (2U)
#define RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_MSK                    (((1U<<RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_POS)
#define RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_UMSK                   (~(((1U<<RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_5G_MUL_ATT_POS))
#define RF_ANA1_SS0_GC_PWRDET_5G_ATT                            RF_ANA1_SS0_GC_PWRDET_5G_ATT
#define RF_ANA1_SS0_GC_PWRDET_5G_ATT_POS                        (8U)
#define RF_ANA1_SS0_GC_PWRDET_5G_ATT_LEN                        (3U)
#define RF_ANA1_SS0_GC_PWRDET_5G_ATT_MSK                        (((1U<<RF_ANA1_SS0_GC_PWRDET_5G_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_5G_ATT_POS)
#define RF_ANA1_SS0_GC_PWRDET_5G_ATT_UMSK                       (~(((1U<<RF_ANA1_SS0_GC_PWRDET_5G_ATT_LEN)-1)<<RF_ANA1_SS0_GC_PWRDET_5G_ATT_POS))
#define RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE                        RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE
#define RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_POS                    (12U)
#define RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_LEN                    (5U)
#define RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_MSK                    (((1U<<RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_POS)
#define RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_UMSK                   (~(((1U<<RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_OSDAC_CODE_POS))
#define RF_ANA1_SS0_PWRDET_5G_MUL_BM                            RF_ANA1_SS0_PWRDET_5G_MUL_BM
#define RF_ANA1_SS0_PWRDET_5G_MUL_BM_POS                        (20U)
#define RF_ANA1_SS0_PWRDET_5G_MUL_BM_LEN                        (3U)
#define RF_ANA1_SS0_PWRDET_5G_MUL_BM_MSK                        (((1U<<RF_ANA1_SS0_PWRDET_5G_MUL_BM_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_MUL_BM_POS)
#define RF_ANA1_SS0_PWRDET_5G_MUL_BM_UMSK                       (~(((1U<<RF_ANA1_SS0_PWRDET_5G_MUL_BM_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_MUL_BM_POS))

/* 0x240 : ss0_pwrdet_5g_1 */
#define RF_ANA1_SS0_PWRDET_5G_1_OFFSET                          (0x240)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_HW                          RF_ANA1_SS0_PWRDET_5G_PR_EN_HW
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_POS                      (1U)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_LEN                      (1U)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_MSK                      (((1U<<RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_POS)
#define RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_UMSK                     (~(((1U<<RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_EN_HW_POS))
#define RF_ANA1_SS0_PWRDET_5G_PR_SWQ                            RF_ANA1_SS0_PWRDET_5G_PR_SWQ
#define RF_ANA1_SS0_PWRDET_5G_PR_SWQ_POS                        (12U)
#define RF_ANA1_SS0_PWRDET_5G_PR_SWQ_LEN                        (2U)
#define RF_ANA1_SS0_PWRDET_5G_PR_SWQ_MSK                        (((1U<<RF_ANA1_SS0_PWRDET_5G_PR_SWQ_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_SWQ_POS)
#define RF_ANA1_SS0_PWRDET_5G_PR_SWQ_UMSK                       (~(((1U<<RF_ANA1_SS0_PWRDET_5G_PR_SWQ_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_SWQ_POS))
#define RF_ANA1_SS0_PWRDET_5G_PR_SWI                            RF_ANA1_SS0_PWRDET_5G_PR_SWI
#define RF_ANA1_SS0_PWRDET_5G_PR_SWI_POS                        (16U)
#define RF_ANA1_SS0_PWRDET_5G_PR_SWI_LEN                        (2U)
#define RF_ANA1_SS0_PWRDET_5G_PR_SWI_MSK                        (((1U<<RF_ANA1_SS0_PWRDET_5G_PR_SWI_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_SWI_POS)
#define RF_ANA1_SS0_PWRDET_5G_PR_SWI_UMSK                       (~(((1U<<RF_ANA1_SS0_PWRDET_5G_PR_SWI_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_SWI_POS))
#define RF_ANA1_SS0_PWRDET_5G_PR_BM                             RF_ANA1_SS0_PWRDET_5G_PR_BM
#define RF_ANA1_SS0_PWRDET_5G_PR_BM_POS                         (20U)
#define RF_ANA1_SS0_PWRDET_5G_PR_BM_LEN                         (2U)
#define RF_ANA1_SS0_PWRDET_5G_PR_BM_MSK                         (((1U<<RF_ANA1_SS0_PWRDET_5G_PR_BM_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_BM_POS)
#define RF_ANA1_SS0_PWRDET_5G_PR_BM_UMSK                        (~(((1U<<RF_ANA1_SS0_PWRDET_5G_PR_BM_LEN)-1)<<RF_ANA1_SS0_PWRDET_5G_PR_BM_POS))

/* 0x244 : ss0_pwrdet_5g_2 */
#define RF_ANA1_SS0_PWRDET_5G_2_OFFSET                          (0x244)

/* 0x248 : ss0_tbb */
#define RF_ANA1_SS0_TBB_OFFSET                                  (0x248)
#define RF_ANA1_SS0_TBB_ATEST_RES_SEL                           RF_ANA1_SS0_TBB_ATEST_RES_SEL
#define RF_ANA1_SS0_TBB_ATEST_RES_SEL_POS                       (0U)
#define RF_ANA1_SS0_TBB_ATEST_RES_SEL_LEN                       (2U)
#define RF_ANA1_SS0_TBB_ATEST_RES_SEL_MSK                       (((1U<<RF_ANA1_SS0_TBB_ATEST_RES_SEL_LEN)-1)<<RF_ANA1_SS0_TBB_ATEST_RES_SEL_POS)
#define RF_ANA1_SS0_TBB_ATEST_RES_SEL_UMSK                      (~(((1U<<RF_ANA1_SS0_TBB_ATEST_RES_SEL_LEN)-1)<<RF_ANA1_SS0_TBB_ATEST_RES_SEL_POS))
#define RF_ANA1_SS0_TBB_ATEST_IN_EN                             RF_ANA1_SS0_TBB_ATEST_IN_EN
#define RF_ANA1_SS0_TBB_ATEST_IN_EN_POS                         (2U)
#define RF_ANA1_SS0_TBB_ATEST_IN_EN_LEN                         (1U)
#define RF_ANA1_SS0_TBB_ATEST_IN_EN_MSK                         (((1U<<RF_ANA1_SS0_TBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_SS0_TBB_ATEST_IN_EN_POS)
#define RF_ANA1_SS0_TBB_ATEST_IN_EN_UMSK                        (~(((1U<<RF_ANA1_SS0_TBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_SS0_TBB_ATEST_IN_EN_POS))
#define RF_ANA1_SS0_TBB_IQ_BIAS_SHORT                           RF_ANA1_SS0_TBB_IQ_BIAS_SHORT
#define RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_POS                       (3U)
#define RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_LEN                       (1U)
#define RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_MSK                       (((1U<<RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_POS)
#define RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_UMSK                      (~(((1U<<RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_ANA1_SS0_TBB_IQ_BIAS_SHORT_POS))
#define RF_ANA1_SS0_TBB_VCM                                     RF_ANA1_SS0_TBB_VCM
#define RF_ANA1_SS0_TBB_VCM_POS                                 (4U)
#define RF_ANA1_SS0_TBB_VCM_LEN                                 (2U)
#define RF_ANA1_SS0_TBB_VCM_MSK                                 (((1U<<RF_ANA1_SS0_TBB_VCM_LEN)-1)<<RF_ANA1_SS0_TBB_VCM_POS)
#define RF_ANA1_SS0_TBB_VCM_UMSK                                (~(((1U<<RF_ANA1_SS0_TBB_VCM_LEN)-1)<<RF_ANA1_SS0_TBB_VCM_POS))
#define RF_ANA1_SS0_TBB_CFLT                                    RF_ANA1_SS0_TBB_CFLT
#define RF_ANA1_SS0_TBB_CFLT_POS                                (6U)
#define RF_ANA1_SS0_TBB_CFLT_LEN                                (2U)
#define RF_ANA1_SS0_TBB_CFLT_MSK                                (((1U<<RF_ANA1_SS0_TBB_CFLT_LEN)-1)<<RF_ANA1_SS0_TBB_CFLT_POS)
#define RF_ANA1_SS0_TBB_CFLT_UMSK                               (~(((1U<<RF_ANA1_SS0_TBB_CFLT_LEN)-1)<<RF_ANA1_SS0_TBB_CFLT_POS))
#define RF_ANA1_SS0_TBB_BM_SF                                   RF_ANA1_SS0_TBB_BM_SF
#define RF_ANA1_SS0_TBB_BM_SF_POS                               (8U)
#define RF_ANA1_SS0_TBB_BM_SF_LEN                               (2U)
#define RF_ANA1_SS0_TBB_BM_SF_MSK                               (((1U<<RF_ANA1_SS0_TBB_BM_SF_LEN)-1)<<RF_ANA1_SS0_TBB_BM_SF_POS)
#define RF_ANA1_SS0_TBB_BM_SF_UMSK                              (~(((1U<<RF_ANA1_SS0_TBB_BM_SF_LEN)-1)<<RF_ANA1_SS0_TBB_BM_SF_POS))
#define RF_ANA1_SS0_TOSDAC_Q                                    RF_ANA1_SS0_TOSDAC_Q
#define RF_ANA1_SS0_TOSDAC_Q_POS                                (16U)
#define RF_ANA1_SS0_TOSDAC_Q_LEN                                (6U)
#define RF_ANA1_SS0_TOSDAC_Q_MSK                                (((1U<<RF_ANA1_SS0_TOSDAC_Q_LEN)-1)<<RF_ANA1_SS0_TOSDAC_Q_POS)
#define RF_ANA1_SS0_TOSDAC_Q_UMSK                               (~(((1U<<RF_ANA1_SS0_TOSDAC_Q_LEN)-1)<<RF_ANA1_SS0_TOSDAC_Q_POS))
#define RF_ANA1_SS0_TOSDAC_I                                    RF_ANA1_SS0_TOSDAC_I
#define RF_ANA1_SS0_TOSDAC_I_POS                                (22U)
#define RF_ANA1_SS0_TOSDAC_I_LEN                                (6U)
#define RF_ANA1_SS0_TOSDAC_I_MSK                                (((1U<<RF_ANA1_SS0_TOSDAC_I_LEN)-1)<<RF_ANA1_SS0_TOSDAC_I_POS)
#define RF_ANA1_SS0_TOSDAC_I_UMSK                               (~(((1U<<RF_ANA1_SS0_TOSDAC_I_LEN)-1)<<RF_ANA1_SS0_TOSDAC_I_POS))
#define RF_ANA1_SS0_TBB_ATEST_OUT_EN                            RF_ANA1_SS0_TBB_ATEST_OUT_EN
#define RF_ANA1_SS0_TBB_ATEST_OUT_EN_POS                        (28U)
#define RF_ANA1_SS0_TBB_ATEST_OUT_EN_LEN                        (1U)
#define RF_ANA1_SS0_TBB_ATEST_OUT_EN_MSK                        (((1U<<RF_ANA1_SS0_TBB_ATEST_OUT_EN_LEN)-1)<<RF_ANA1_SS0_TBB_ATEST_OUT_EN_POS)
#define RF_ANA1_SS0_TBB_ATEST_OUT_EN_UMSK                       (~(((1U<<RF_ANA1_SS0_TBB_ATEST_OUT_EN_LEN)-1)<<RF_ANA1_SS0_TBB_ATEST_OUT_EN_POS))

/* 0x24C : Digital Control */
#define RF_ANA1_SS0_DAC_0_OFFSET                                (0x24C)
#define RF_ANA1_SS0_DAC_DVDD_SEL                                RF_ANA1_SS0_DAC_DVDD_SEL
#define RF_ANA1_SS0_DAC_DVDD_SEL_POS                            (0U)
#define RF_ANA1_SS0_DAC_DVDD_SEL_LEN                            (3U)
#define RF_ANA1_SS0_DAC_DVDD_SEL_MSK                            (((1U<<RF_ANA1_SS0_DAC_DVDD_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_DVDD_SEL_POS)
#define RF_ANA1_SS0_DAC_DVDD_SEL_UMSK                           (~(((1U<<RF_ANA1_SS0_DAC_DVDD_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_DVDD_SEL_POS))
#define RF_ANA1_SS0_DAC_BIAS_ENHANCE                            RF_ANA1_SS0_DAC_BIAS_ENHANCE
#define RF_ANA1_SS0_DAC_BIAS_ENHANCE_POS                        (4U)
#define RF_ANA1_SS0_DAC_BIAS_ENHANCE_LEN                        (1U)
#define RF_ANA1_SS0_DAC_BIAS_ENHANCE_MSK                        (((1U<<RF_ANA1_SS0_DAC_BIAS_ENHANCE_LEN)-1)<<RF_ANA1_SS0_DAC_BIAS_ENHANCE_POS)
#define RF_ANA1_SS0_DAC_BIAS_ENHANCE_UMSK                       (~(((1U<<RF_ANA1_SS0_DAC_BIAS_ENHANCE_LEN)-1)<<RF_ANA1_SS0_DAC_BIAS_ENHANCE_POS))
#define RF_ANA1_SS0_DAC_BIAS_SEL                                RF_ANA1_SS0_DAC_BIAS_SEL
#define RF_ANA1_SS0_DAC_BIAS_SEL_POS                            (8U)
#define RF_ANA1_SS0_DAC_BIAS_SEL_LEN                            (2U)
#define RF_ANA1_SS0_DAC_BIAS_SEL_MSK                            (((1U<<RF_ANA1_SS0_DAC_BIAS_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_BIAS_SEL_POS)
#define RF_ANA1_SS0_DAC_BIAS_SEL_UMSK                           (~(((1U<<RF_ANA1_SS0_DAC_BIAS_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_BIAS_SEL_POS))
#define RF_ANA1_SS0_DAC_RCCALSEL                                RF_ANA1_SS0_DAC_RCCALSEL
#define RF_ANA1_SS0_DAC_RCCALSEL_POS                            (12U)
#define RF_ANA1_SS0_DAC_RCCALSEL_LEN                            (1U)
#define RF_ANA1_SS0_DAC_RCCALSEL_MSK                            (((1U<<RF_ANA1_SS0_DAC_RCCALSEL_LEN)-1)<<RF_ANA1_SS0_DAC_RCCALSEL_POS)
#define RF_ANA1_SS0_DAC_RCCALSEL_UMSK                           (~(((1U<<RF_ANA1_SS0_DAC_RCCALSEL_LEN)-1)<<RF_ANA1_SS0_DAC_RCCALSEL_POS))
#define RF_ANA1_SS0_DAC_CLK_SYNC_INV                            RF_ANA1_SS0_DAC_CLK_SYNC_INV
#define RF_ANA1_SS0_DAC_CLK_SYNC_INV_POS                        (16U)
#define RF_ANA1_SS0_DAC_CLK_SYNC_INV_LEN                        (1U)
#define RF_ANA1_SS0_DAC_CLK_SYNC_INV_MSK                        (((1U<<RF_ANA1_SS0_DAC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_SS0_DAC_CLK_SYNC_INV_POS)
#define RF_ANA1_SS0_DAC_CLK_SYNC_INV_UMSK                       (~(((1U<<RF_ANA1_SS0_DAC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_SS0_DAC_CLK_SYNC_INV_POS))
#define RF_ANA1_SS0_DAC_RCCAL_SWAP_EN                           RF_ANA1_SS0_DAC_RCCAL_SWAP_EN
#define RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_POS                       (20U)
#define RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_LEN                       (1U)
#define RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_MSK                       (((1U<<RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_LEN)-1)<<RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_POS)
#define RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_UMSK                      (~(((1U<<RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_LEN)-1)<<RF_ANA1_SS0_DAC_RCCAL_SWAP_EN_POS))
#define RF_ANA1_SS0_DAC_VLOW_SEL                                RF_ANA1_SS0_DAC_VLOW_SEL
#define RF_ANA1_SS0_DAC_VLOW_SEL_POS                            (24U)
#define RF_ANA1_SS0_DAC_VLOW_SEL_LEN                            (2U)
#define RF_ANA1_SS0_DAC_VLOW_SEL_MSK                            (((1U<<RF_ANA1_SS0_DAC_VLOW_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_VLOW_SEL_POS)
#define RF_ANA1_SS0_DAC_VLOW_SEL_UMSK                           (~(((1U<<RF_ANA1_SS0_DAC_VLOW_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_VLOW_SEL_POS))
#define RF_ANA1_SS0_DAC_NSINK_SEL                               RF_ANA1_SS0_DAC_NSINK_SEL
#define RF_ANA1_SS0_DAC_NSINK_SEL_POS                           (28U)
#define RF_ANA1_SS0_DAC_NSINK_SEL_LEN                           (2U)
#define RF_ANA1_SS0_DAC_NSINK_SEL_MSK                           (((1U<<RF_ANA1_SS0_DAC_NSINK_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_NSINK_SEL_POS)
#define RF_ANA1_SS0_DAC_NSINK_SEL_UMSK                          (~(((1U<<RF_ANA1_SS0_DAC_NSINK_SEL_LEN)-1)<<RF_ANA1_SS0_DAC_NSINK_SEL_POS))

/* 0x254 : ss0_pa_config_force */
#define RF_ANA1_SS0_PA_CONFIG_FORCE_OFFSET                      (0x254)
#define RF_ANA1_SS0_WIFI_MODE_IND                               RF_ANA1_SS0_WIFI_MODE_IND
#define RF_ANA1_SS0_WIFI_MODE_IND_POS                           (0U)
#define RF_ANA1_SS0_WIFI_MODE_IND_LEN                           (2U)
#define RF_ANA1_SS0_WIFI_MODE_IND_MSK                           (((1U<<RF_ANA1_SS0_WIFI_MODE_IND_LEN)-1)<<RF_ANA1_SS0_WIFI_MODE_IND_POS)
#define RF_ANA1_SS0_WIFI_MODE_IND_UMSK                          (~(((1U<<RF_ANA1_SS0_WIFI_MODE_IND_LEN)-1)<<RF_ANA1_SS0_WIFI_MODE_IND_POS))
#define RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN                      RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN
#define RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_POS                  (2U)
#define RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_LEN                  (1U)
#define RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_MSK                  (((1U<<RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_LEN)-1)<<RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_POS)
#define RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_UMSK                 (~(((1U<<RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_LEN)-1)<<RF_ANA1_SS0_WIFI_MODE_IND_FORCE_EN_POS))

/* 0x258 : ss0_2g_pa_config_b0 */
#define RF_ANA1_SS0_2G_PA_CONFIG_B0_OFFSET                      (0x258)
#define RF_ANA1_SS0_2G_PA_VBCORE_B0                             RF_ANA1_SS0_2G_PA_VBCORE_B0
#define RF_ANA1_SS0_2G_PA_VBCORE_B0_POS                         (0U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B0_LEN                         (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B0_MSK                         (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B0_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B0_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_B0_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B0_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B0_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_B1                             RF_ANA1_SS0_2G_PA_VBCORE_B1
#define RF_ANA1_SS0_2G_PA_VBCORE_B1_POS                         (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B1_LEN                         (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B1_MSK                         (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B1_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B1_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_B1_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B1_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B1_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_B2                             RF_ANA1_SS0_2G_PA_VBCORE_B2
#define RF_ANA1_SS0_2G_PA_VBCORE_B2_POS                         (10U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B2_LEN                         (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B2_MSK                         (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B2_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B2_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_B2_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B2_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B2_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_B3                             RF_ANA1_SS0_2G_PA_VBCORE_B3
#define RF_ANA1_SS0_2G_PA_VBCORE_B3_POS                         (15U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B3_LEN                         (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_B3_MSK                         (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B3_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B3_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_B3_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_B3_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_B3_POS))
#define RF_ANA1_SS0_2G_PA_VBCAS1_B                              RF_ANA1_SS0_2G_PA_VBCAS1_B
#define RF_ANA1_SS0_2G_PA_VBCAS1_B_POS                          (20U)
#define RF_ANA1_SS0_2G_PA_VBCAS1_B_LEN                          (4U)
#define RF_ANA1_SS0_2G_PA_VBCAS1_B_MSK                          (((1U<<RF_ANA1_SS0_2G_PA_VBCAS1_B_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCAS1_B_POS)
#define RF_ANA1_SS0_2G_PA_VBCAS1_B_UMSK                         (~(((1U<<RF_ANA1_SS0_2G_PA_VBCAS1_B_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCAS1_B_POS))

/* 0x25C : ss0_2g_pa_config_b1 */
#define RF_ANA1_SS0_2G_PA_CONFIG_B1_OFFSET                      (0x25C)
#define RF_ANA1_SS0_2G_PA_IET_B0                                RF_ANA1_SS0_2G_PA_IET_B0
#define RF_ANA1_SS0_2G_PA_IET_B0_POS                            (0U)
#define RF_ANA1_SS0_2G_PA_IET_B0_LEN                            (5U)
#define RF_ANA1_SS0_2G_PA_IET_B0_MSK                            (((1U<<RF_ANA1_SS0_2G_PA_IET_B0_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B0_POS)
#define RF_ANA1_SS0_2G_PA_IET_B0_UMSK                           (~(((1U<<RF_ANA1_SS0_2G_PA_IET_B0_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B0_POS))
#define RF_ANA1_SS0_2G_PA_IET_B1                                RF_ANA1_SS0_2G_PA_IET_B1
#define RF_ANA1_SS0_2G_PA_IET_B1_POS                            (5U)
#define RF_ANA1_SS0_2G_PA_IET_B1_LEN                            (5U)
#define RF_ANA1_SS0_2G_PA_IET_B1_MSK                            (((1U<<RF_ANA1_SS0_2G_PA_IET_B1_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B1_POS)
#define RF_ANA1_SS0_2G_PA_IET_B1_UMSK                           (~(((1U<<RF_ANA1_SS0_2G_PA_IET_B1_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B1_POS))
#define RF_ANA1_SS0_2G_PA_IET_B2                                RF_ANA1_SS0_2G_PA_IET_B2
#define RF_ANA1_SS0_2G_PA_IET_B2_POS                            (10U)
#define RF_ANA1_SS0_2G_PA_IET_B2_LEN                            (5U)
#define RF_ANA1_SS0_2G_PA_IET_B2_MSK                            (((1U<<RF_ANA1_SS0_2G_PA_IET_B2_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B2_POS)
#define RF_ANA1_SS0_2G_PA_IET_B2_UMSK                           (~(((1U<<RF_ANA1_SS0_2G_PA_IET_B2_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B2_POS))
#define RF_ANA1_SS0_2G_PA_IET_B3                                RF_ANA1_SS0_2G_PA_IET_B3
#define RF_ANA1_SS0_2G_PA_IET_B3_POS                            (15U)
#define RF_ANA1_SS0_2G_PA_IET_B3_LEN                            (5U)
#define RF_ANA1_SS0_2G_PA_IET_B3_MSK                            (((1U<<RF_ANA1_SS0_2G_PA_IET_B3_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B3_POS)
#define RF_ANA1_SS0_2G_PA_IET_B3_UMSK                           (~(((1U<<RF_ANA1_SS0_2G_PA_IET_B3_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_B3_POS))
#define RF_ANA1_SS0_2G_PA_IAQ_B                                 RF_ANA1_SS0_2G_PA_IAQ_B
#define RF_ANA1_SS0_2G_PA_IAQ_B_POS                             (20U)
#define RF_ANA1_SS0_2G_PA_IAQ_B_LEN                             (3U)
#define RF_ANA1_SS0_2G_PA_IAQ_B_MSK                             (((1U<<RF_ANA1_SS0_2G_PA_IAQ_B_LEN)-1)<<RF_ANA1_SS0_2G_PA_IAQ_B_POS)
#define RF_ANA1_SS0_2G_PA_IAQ_B_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_PA_IAQ_B_LEN)-1)<<RF_ANA1_SS0_2G_PA_IAQ_B_POS))

/* 0x260 : ss0_2g_pa_config_gn0 */
#define RF_ANA1_SS0_2G_PA_CONFIG_GN0_OFFSET                     (0x260)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN0                            RF_ANA1_SS0_2G_PA_VBCORE_GN0
#define RF_ANA1_SS0_2G_PA_VBCORE_GN0_POS                        (0U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN0_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN0_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN0_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN0_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN0_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN0_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN0_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_GN1                            RF_ANA1_SS0_2G_PA_VBCORE_GN1
#define RF_ANA1_SS0_2G_PA_VBCORE_GN1_POS                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN1_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN1_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN1_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN1_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN1_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN1_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN1_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_GN2                            RF_ANA1_SS0_2G_PA_VBCORE_GN2
#define RF_ANA1_SS0_2G_PA_VBCORE_GN2_POS                        (10U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN2_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN2_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN2_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN2_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN2_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN2_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN2_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_GN3                            RF_ANA1_SS0_2G_PA_VBCORE_GN3
#define RF_ANA1_SS0_2G_PA_VBCORE_GN3_POS                        (15U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN3_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN3_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN3_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN3_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_GN3_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_GN3_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_GN3_POS))
#define RF_ANA1_SS0_2G_PA_VBCAS1_GN                             RF_ANA1_SS0_2G_PA_VBCAS1_GN
#define RF_ANA1_SS0_2G_PA_VBCAS1_GN_POS                         (20U)
#define RF_ANA1_SS0_2G_PA_VBCAS1_GN_LEN                         (4U)
#define RF_ANA1_SS0_2G_PA_VBCAS1_GN_MSK                         (((1U<<RF_ANA1_SS0_2G_PA_VBCAS1_GN_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCAS1_GN_POS)
#define RF_ANA1_SS0_2G_PA_VBCAS1_GN_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_PA_VBCAS1_GN_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCAS1_GN_POS))

/* 0x264 : ss0_2g_pa_config_gn1 */
#define RF_ANA1_SS0_2G_PA_CONFIG_GN1_OFFSET                     (0x264)
#define RF_ANA1_SS0_2G_PA_IET_GN0                               RF_ANA1_SS0_2G_PA_IET_GN0
#define RF_ANA1_SS0_2G_PA_IET_GN0_POS                           (0U)
#define RF_ANA1_SS0_2G_PA_IET_GN0_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_GN0_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_GN0_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN0_POS)
#define RF_ANA1_SS0_2G_PA_IET_GN0_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_GN0_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN0_POS))
#define RF_ANA1_SS0_2G_PA_IET_GN1                               RF_ANA1_SS0_2G_PA_IET_GN1
#define RF_ANA1_SS0_2G_PA_IET_GN1_POS                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_GN1_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_GN1_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_GN1_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN1_POS)
#define RF_ANA1_SS0_2G_PA_IET_GN1_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_GN1_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN1_POS))
#define RF_ANA1_SS0_2G_PA_IET_GN2                               RF_ANA1_SS0_2G_PA_IET_GN2
#define RF_ANA1_SS0_2G_PA_IET_GN2_POS                           (10U)
#define RF_ANA1_SS0_2G_PA_IET_GN2_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_GN2_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_GN2_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN2_POS)
#define RF_ANA1_SS0_2G_PA_IET_GN2_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_GN2_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN2_POS))
#define RF_ANA1_SS0_2G_PA_IET_GN3                               RF_ANA1_SS0_2G_PA_IET_GN3
#define RF_ANA1_SS0_2G_PA_IET_GN3_POS                           (15U)
#define RF_ANA1_SS0_2G_PA_IET_GN3_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_GN3_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_GN3_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN3_POS)
#define RF_ANA1_SS0_2G_PA_IET_GN3_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_GN3_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_GN3_POS))
#define RF_ANA1_SS0_2G_PA_IAQ_GN                                RF_ANA1_SS0_2G_PA_IAQ_GN
#define RF_ANA1_SS0_2G_PA_IAQ_GN_POS                            (20U)
#define RF_ANA1_SS0_2G_PA_IAQ_GN_LEN                            (3U)
#define RF_ANA1_SS0_2G_PA_IAQ_GN_MSK                            (((1U<<RF_ANA1_SS0_2G_PA_IAQ_GN_LEN)-1)<<RF_ANA1_SS0_2G_PA_IAQ_GN_POS)
#define RF_ANA1_SS0_2G_PA_IAQ_GN_UMSK                           (~(((1U<<RF_ANA1_SS0_2G_PA_IAQ_GN_LEN)-1)<<RF_ANA1_SS0_2G_PA_IAQ_GN_POS))

/* 0x268 : ss0_2g_pa_config_ax0 */
#define RF_ANA1_SS0_2G_PA_CONFIG_AX0_OFFSET                     (0x268)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX0                            RF_ANA1_SS0_2G_PA_VBCORE_AX0
#define RF_ANA1_SS0_2G_PA_VBCORE_AX0_POS                        (0U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX0_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX0_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX0_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX0_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX0_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX0_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX0_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_AX1                            RF_ANA1_SS0_2G_PA_VBCORE_AX1
#define RF_ANA1_SS0_2G_PA_VBCORE_AX1_POS                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX1_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX1_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX1_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX1_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX1_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX1_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX1_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_AX2                            RF_ANA1_SS0_2G_PA_VBCORE_AX2
#define RF_ANA1_SS0_2G_PA_VBCORE_AX2_POS                        (10U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX2_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX2_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX2_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX2_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX2_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX2_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX2_POS))
#define RF_ANA1_SS0_2G_PA_VBCORE_AX3                            RF_ANA1_SS0_2G_PA_VBCORE_AX3
#define RF_ANA1_SS0_2G_PA_VBCORE_AX3_POS                        (15U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX3_LEN                        (5U)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX3_MSK                        (((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX3_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX3_POS)
#define RF_ANA1_SS0_2G_PA_VBCORE_AX3_UMSK                       (~(((1U<<RF_ANA1_SS0_2G_PA_VBCORE_AX3_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCORE_AX3_POS))
#define RF_ANA1_SS0_2G_PA_VBCAS1_AX                             RF_ANA1_SS0_2G_PA_VBCAS1_AX
#define RF_ANA1_SS0_2G_PA_VBCAS1_AX_POS                         (20U)
#define RF_ANA1_SS0_2G_PA_VBCAS1_AX_LEN                         (4U)
#define RF_ANA1_SS0_2G_PA_VBCAS1_AX_MSK                         (((1U<<RF_ANA1_SS0_2G_PA_VBCAS1_AX_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCAS1_AX_POS)
#define RF_ANA1_SS0_2G_PA_VBCAS1_AX_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_PA_VBCAS1_AX_LEN)-1)<<RF_ANA1_SS0_2G_PA_VBCAS1_AX_POS))

/* 0x26C : ss0_2g_pa_config_ax1 */
#define RF_ANA1_SS0_2G_PA_CONFIG_AX1_OFFSET                     (0x26C)
#define RF_ANA1_SS0_2G_PA_IET_AX0                               RF_ANA1_SS0_2G_PA_IET_AX0
#define RF_ANA1_SS0_2G_PA_IET_AX0_POS                           (0U)
#define RF_ANA1_SS0_2G_PA_IET_AX0_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_AX0_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_AX0_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX0_POS)
#define RF_ANA1_SS0_2G_PA_IET_AX0_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_AX0_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX0_POS))
#define RF_ANA1_SS0_2G_PA_IET_AX1                               RF_ANA1_SS0_2G_PA_IET_AX1
#define RF_ANA1_SS0_2G_PA_IET_AX1_POS                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_AX1_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_AX1_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_AX1_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX1_POS)
#define RF_ANA1_SS0_2G_PA_IET_AX1_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_AX1_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX1_POS))
#define RF_ANA1_SS0_2G_PA_IET_AX2                               RF_ANA1_SS0_2G_PA_IET_AX2
#define RF_ANA1_SS0_2G_PA_IET_AX2_POS                           (10U)
#define RF_ANA1_SS0_2G_PA_IET_AX2_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_AX2_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_AX2_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX2_POS)
#define RF_ANA1_SS0_2G_PA_IET_AX2_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_AX2_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX2_POS))
#define RF_ANA1_SS0_2G_PA_IET_AX3                               RF_ANA1_SS0_2G_PA_IET_AX3
#define RF_ANA1_SS0_2G_PA_IET_AX3_POS                           (15U)
#define RF_ANA1_SS0_2G_PA_IET_AX3_LEN                           (5U)
#define RF_ANA1_SS0_2G_PA_IET_AX3_MSK                           (((1U<<RF_ANA1_SS0_2G_PA_IET_AX3_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX3_POS)
#define RF_ANA1_SS0_2G_PA_IET_AX3_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_PA_IET_AX3_LEN)-1)<<RF_ANA1_SS0_2G_PA_IET_AX3_POS))
#define RF_ANA1_SS0_2G_PA_IAQ_AX                                RF_ANA1_SS0_2G_PA_IAQ_AX
#define RF_ANA1_SS0_2G_PA_IAQ_AX_POS                            (20U)
#define RF_ANA1_SS0_2G_PA_IAQ_AX_LEN                            (3U)
#define RF_ANA1_SS0_2G_PA_IAQ_AX_MSK                            (((1U<<RF_ANA1_SS0_2G_PA_IAQ_AX_LEN)-1)<<RF_ANA1_SS0_2G_PA_IAQ_AX_POS)
#define RF_ANA1_SS0_2G_PA_IAQ_AX_UMSK                           (~(((1U<<RF_ANA1_SS0_2G_PA_IAQ_AX_LEN)-1)<<RF_ANA1_SS0_2G_PA_IAQ_AX_POS))

/* 0x270 : ss0_pa_config_bz */
#define RF_ANA1_SS0_PA_CONFIG_BZ_OFFSET                         (0x270)
#define RF_ANA1_SS0_PA_IET_BZ                                   RF_ANA1_SS0_PA_IET_BZ
#define RF_ANA1_SS0_PA_IET_BZ_POS                               (0U)
#define RF_ANA1_SS0_PA_IET_BZ_LEN                               (5U)
#define RF_ANA1_SS0_PA_IET_BZ_MSK                               (((1U<<RF_ANA1_SS0_PA_IET_BZ_LEN)-1)<<RF_ANA1_SS0_PA_IET_BZ_POS)
#define RF_ANA1_SS0_PA_IET_BZ_UMSK                              (~(((1U<<RF_ANA1_SS0_PA_IET_BZ_LEN)-1)<<RF_ANA1_SS0_PA_IET_BZ_POS))
#define RF_ANA1_SS0_PA_IAQ_BZ                                   RF_ANA1_SS0_PA_IAQ_BZ
#define RF_ANA1_SS0_PA_IAQ_BZ_POS                               (5U)
#define RF_ANA1_SS0_PA_IAQ_BZ_LEN                               (3U)
#define RF_ANA1_SS0_PA_IAQ_BZ_MSK                               (((1U<<RF_ANA1_SS0_PA_IAQ_BZ_LEN)-1)<<RF_ANA1_SS0_PA_IAQ_BZ_POS)
#define RF_ANA1_SS0_PA_IAQ_BZ_UMSK                              (~(((1U<<RF_ANA1_SS0_PA_IAQ_BZ_LEN)-1)<<RF_ANA1_SS0_PA_IAQ_BZ_POS))
#define RF_ANA1_SS0_PA_VBCORE_BZ                                RF_ANA1_SS0_PA_VBCORE_BZ
#define RF_ANA1_SS0_PA_VBCORE_BZ_POS                            (8U)
#define RF_ANA1_SS0_PA_VBCORE_BZ_LEN                            (5U)
#define RF_ANA1_SS0_PA_VBCORE_BZ_MSK                            (((1U<<RF_ANA1_SS0_PA_VBCORE_BZ_LEN)-1)<<RF_ANA1_SS0_PA_VBCORE_BZ_POS)
#define RF_ANA1_SS0_PA_VBCORE_BZ_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_VBCORE_BZ_LEN)-1)<<RF_ANA1_SS0_PA_VBCORE_BZ_POS))
#define RF_ANA1_SS0_PA_VBCAS1_BZ                                RF_ANA1_SS0_PA_VBCAS1_BZ
#define RF_ANA1_SS0_PA_VBCAS1_BZ_POS                            (16U)
#define RF_ANA1_SS0_PA_VBCAS1_BZ_LEN                            (4U)
#define RF_ANA1_SS0_PA_VBCAS1_BZ_MSK                            (((1U<<RF_ANA1_SS0_PA_VBCAS1_BZ_LEN)-1)<<RF_ANA1_SS0_PA_VBCAS1_BZ_POS)
#define RF_ANA1_SS0_PA_VBCAS1_BZ_UMSK                           (~(((1U<<RF_ANA1_SS0_PA_VBCAS1_BZ_LEN)-1)<<RF_ANA1_SS0_PA_VBCAS1_BZ_POS))

/* 0x274 : ss0_5g_pa_config_agn0 */
#define RF_ANA1_SS0_5G_PA_CONFIG_AGN0_OFFSET                    (0x274)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN0                           RF_ANA1_SS0_5G_PA_VBCORE_AGN0
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN0_POS                       (0U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN0_LEN                       (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN0_MSK                       (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN0_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN0_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN0_UMSK                      (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN0_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN0_POS))
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN1                           RF_ANA1_SS0_5G_PA_VBCORE_AGN1
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN1_POS                       (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN1_LEN                       (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN1_MSK                       (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN1_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN1_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN1_UMSK                      (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN1_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN1_POS))
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN2                           RF_ANA1_SS0_5G_PA_VBCORE_AGN2
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN2_POS                       (10U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN2_LEN                       (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN2_MSK                       (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN2_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN2_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN2_UMSK                      (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN2_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN2_POS))
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN3                           RF_ANA1_SS0_5G_PA_VBCORE_AGN3
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN3_POS                       (15U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN3_LEN                       (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN3_MSK                       (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN3_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN3_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AGN3_UMSK                      (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AGN3_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AGN3_POS))
#define RF_ANA1_SS0_5G_PA_VBCAS1_AGN                            RF_ANA1_SS0_5G_PA_VBCAS1_AGN
#define RF_ANA1_SS0_5G_PA_VBCAS1_AGN_POS                        (20U)
#define RF_ANA1_SS0_5G_PA_VBCAS1_AGN_LEN                        (4U)
#define RF_ANA1_SS0_5G_PA_VBCAS1_AGN_MSK                        (((1U<<RF_ANA1_SS0_5G_PA_VBCAS1_AGN_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCAS1_AGN_POS)
#define RF_ANA1_SS0_5G_PA_VBCAS1_AGN_UMSK                       (~(((1U<<RF_ANA1_SS0_5G_PA_VBCAS1_AGN_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCAS1_AGN_POS))

/* 0x278 : ss0_5g_pa_config_agn1 */
#define RF_ANA1_SS0_5G_PA_CONFIG_AGN1_OFFSET                    (0x278)
#define RF_ANA1_SS0_5G_PA_IET_AGN0                              RF_ANA1_SS0_5G_PA_IET_AGN0
#define RF_ANA1_SS0_5G_PA_IET_AGN0_POS                          (0U)
#define RF_ANA1_SS0_5G_PA_IET_AGN0_LEN                          (5U)
#define RF_ANA1_SS0_5G_PA_IET_AGN0_MSK                          (((1U<<RF_ANA1_SS0_5G_PA_IET_AGN0_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN0_POS)
#define RF_ANA1_SS0_5G_PA_IET_AGN0_UMSK                         (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AGN0_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN0_POS))
#define RF_ANA1_SS0_5G_PA_IET_AGN1                              RF_ANA1_SS0_5G_PA_IET_AGN1
#define RF_ANA1_SS0_5G_PA_IET_AGN1_POS                          (5U)
#define RF_ANA1_SS0_5G_PA_IET_AGN1_LEN                          (5U)
#define RF_ANA1_SS0_5G_PA_IET_AGN1_MSK                          (((1U<<RF_ANA1_SS0_5G_PA_IET_AGN1_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN1_POS)
#define RF_ANA1_SS0_5G_PA_IET_AGN1_UMSK                         (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AGN1_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN1_POS))
#define RF_ANA1_SS0_5G_PA_IET_AGN2                              RF_ANA1_SS0_5G_PA_IET_AGN2
#define RF_ANA1_SS0_5G_PA_IET_AGN2_POS                          (10U)
#define RF_ANA1_SS0_5G_PA_IET_AGN2_LEN                          (5U)
#define RF_ANA1_SS0_5G_PA_IET_AGN2_MSK                          (((1U<<RF_ANA1_SS0_5G_PA_IET_AGN2_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN2_POS)
#define RF_ANA1_SS0_5G_PA_IET_AGN2_UMSK                         (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AGN2_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN2_POS))
#define RF_ANA1_SS0_5G_PA_IET_AGN3                              RF_ANA1_SS0_5G_PA_IET_AGN3
#define RF_ANA1_SS0_5G_PA_IET_AGN3_POS                          (15U)
#define RF_ANA1_SS0_5G_PA_IET_AGN3_LEN                          (5U)
#define RF_ANA1_SS0_5G_PA_IET_AGN3_MSK                          (((1U<<RF_ANA1_SS0_5G_PA_IET_AGN3_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN3_POS)
#define RF_ANA1_SS0_5G_PA_IET_AGN3_UMSK                         (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AGN3_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AGN3_POS))
#define RF_ANA1_SS0_5G_PA_IAQ_AGN                               RF_ANA1_SS0_5G_PA_IAQ_AGN
#define RF_ANA1_SS0_5G_PA_IAQ_AGN_POS                           (20U)
#define RF_ANA1_SS0_5G_PA_IAQ_AGN_LEN                           (3U)
#define RF_ANA1_SS0_5G_PA_IAQ_AGN_MSK                           (((1U<<RF_ANA1_SS0_5G_PA_IAQ_AGN_LEN)-1)<<RF_ANA1_SS0_5G_PA_IAQ_AGN_POS)
#define RF_ANA1_SS0_5G_PA_IAQ_AGN_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_PA_IAQ_AGN_LEN)-1)<<RF_ANA1_SS0_5G_PA_IAQ_AGN_POS))

/* 0x27C : ss0_5g_pa_config_ax0 */
#define RF_ANA1_SS0_5G_PA_CONFIG_AX0_OFFSET                     (0x27C)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX0                            RF_ANA1_SS0_5G_PA_VBCORE_AX0
#define RF_ANA1_SS0_5G_PA_VBCORE_AX0_POS                        (0U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX0_LEN                        (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX0_MSK                        (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX0_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX0_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX0_UMSK                       (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX0_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX0_POS))
#define RF_ANA1_SS0_5G_PA_VBCORE_AX1                            RF_ANA1_SS0_5G_PA_VBCORE_AX1
#define RF_ANA1_SS0_5G_PA_VBCORE_AX1_POS                        (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX1_LEN                        (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX1_MSK                        (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX1_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX1_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX1_UMSK                       (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX1_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX1_POS))
#define RF_ANA1_SS0_5G_PA_VBCORE_AX2                            RF_ANA1_SS0_5G_PA_VBCORE_AX2
#define RF_ANA1_SS0_5G_PA_VBCORE_AX2_POS                        (10U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX2_LEN                        (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX2_MSK                        (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX2_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX2_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX2_UMSK                       (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX2_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX2_POS))
#define RF_ANA1_SS0_5G_PA_VBCORE_AX3                            RF_ANA1_SS0_5G_PA_VBCORE_AX3
#define RF_ANA1_SS0_5G_PA_VBCORE_AX3_POS                        (15U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX3_LEN                        (5U)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX3_MSK                        (((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX3_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX3_POS)
#define RF_ANA1_SS0_5G_PA_VBCORE_AX3_UMSK                       (~(((1U<<RF_ANA1_SS0_5G_PA_VBCORE_AX3_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCORE_AX3_POS))
#define RF_ANA1_SS0_5G_PA_VBCAS1_AX                             RF_ANA1_SS0_5G_PA_VBCAS1_AX
#define RF_ANA1_SS0_5G_PA_VBCAS1_AX_POS                         (20U)
#define RF_ANA1_SS0_5G_PA_VBCAS1_AX_LEN                         (4U)
#define RF_ANA1_SS0_5G_PA_VBCAS1_AX_MSK                         (((1U<<RF_ANA1_SS0_5G_PA_VBCAS1_AX_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCAS1_AX_POS)
#define RF_ANA1_SS0_5G_PA_VBCAS1_AX_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_PA_VBCAS1_AX_LEN)-1)<<RF_ANA1_SS0_5G_PA_VBCAS1_AX_POS))

/* 0x280 : ss0_5g_pa_config_ax1 */
#define RF_ANA1_SS0_5G_PA_CONFIG_AX1_OFFSET                     (0x280)
#define RF_ANA1_SS0_5G_PA_IET_AX0                               RF_ANA1_SS0_5G_PA_IET_AX0
#define RF_ANA1_SS0_5G_PA_IET_AX0_POS                           (0U)
#define RF_ANA1_SS0_5G_PA_IET_AX0_LEN                           (5U)
#define RF_ANA1_SS0_5G_PA_IET_AX0_MSK                           (((1U<<RF_ANA1_SS0_5G_PA_IET_AX0_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX0_POS)
#define RF_ANA1_SS0_5G_PA_IET_AX0_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AX0_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX0_POS))
#define RF_ANA1_SS0_5G_PA_IET_AX1                               RF_ANA1_SS0_5G_PA_IET_AX1
#define RF_ANA1_SS0_5G_PA_IET_AX1_POS                           (5U)
#define RF_ANA1_SS0_5G_PA_IET_AX1_LEN                           (5U)
#define RF_ANA1_SS0_5G_PA_IET_AX1_MSK                           (((1U<<RF_ANA1_SS0_5G_PA_IET_AX1_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX1_POS)
#define RF_ANA1_SS0_5G_PA_IET_AX1_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AX1_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX1_POS))
#define RF_ANA1_SS0_5G_PA_IET_AX2                               RF_ANA1_SS0_5G_PA_IET_AX2
#define RF_ANA1_SS0_5G_PA_IET_AX2_POS                           (10U)
#define RF_ANA1_SS0_5G_PA_IET_AX2_LEN                           (5U)
#define RF_ANA1_SS0_5G_PA_IET_AX2_MSK                           (((1U<<RF_ANA1_SS0_5G_PA_IET_AX2_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX2_POS)
#define RF_ANA1_SS0_5G_PA_IET_AX2_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AX2_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX2_POS))
#define RF_ANA1_SS0_5G_PA_IET_AX3                               RF_ANA1_SS0_5G_PA_IET_AX3
#define RF_ANA1_SS0_5G_PA_IET_AX3_POS                           (15U)
#define RF_ANA1_SS0_5G_PA_IET_AX3_LEN                           (5U)
#define RF_ANA1_SS0_5G_PA_IET_AX3_MSK                           (((1U<<RF_ANA1_SS0_5G_PA_IET_AX3_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX3_POS)
#define RF_ANA1_SS0_5G_PA_IET_AX3_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_PA_IET_AX3_LEN)-1)<<RF_ANA1_SS0_5G_PA_IET_AX3_POS))
#define RF_ANA1_SS0_5G_PA_IAQ_AX                                RF_ANA1_SS0_5G_PA_IAQ_AX
#define RF_ANA1_SS0_5G_PA_IAQ_AX_POS                            (20U)
#define RF_ANA1_SS0_5G_PA_IAQ_AX_LEN                            (3U)
#define RF_ANA1_SS0_5G_PA_IAQ_AX_MSK                            (((1U<<RF_ANA1_SS0_5G_PA_IAQ_AX_LEN)-1)<<RF_ANA1_SS0_5G_PA_IAQ_AX_POS)
#define RF_ANA1_SS0_5G_PA_IAQ_AX_UMSK                           (~(((1U<<RF_ANA1_SS0_5G_PA_IAQ_AX_LEN)-1)<<RF_ANA1_SS0_5G_PA_IAQ_AX_POS))

/* 0x300 : Digital Control */
#define RF_ANA1_SS0_RRF_5G_OFFSET                               (0x300)
#define RF_ANA1_SS0_TRXIQBUF_5G_GC                              RF_ANA1_SS0_TRXIQBUF_5G_GC
#define RF_ANA1_SS0_TRXIQBUF_5G_GC_POS                          (0U)
#define RF_ANA1_SS0_TRXIQBUF_5G_GC_LEN                          (1U)
#define RF_ANA1_SS0_TRXIQBUF_5G_GC_MSK                          (((1U<<RF_ANA1_SS0_TRXIQBUF_5G_GC_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_5G_GC_POS)
#define RF_ANA1_SS0_TRXIQBUF_5G_GC_UMSK                         (~(((1U<<RF_ANA1_SS0_TRXIQBUF_5G_GC_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_5G_GC_POS))
#define RF_ANA1_SS0_TRXIQBUF_5G_EN                              RF_ANA1_SS0_TRXIQBUF_5G_EN
#define RF_ANA1_SS0_TRXIQBUF_5G_EN_POS                          (4U)
#define RF_ANA1_SS0_TRXIQBUF_5G_EN_LEN                          (1U)
#define RF_ANA1_SS0_TRXIQBUF_5G_EN_MSK                          (((1U<<RF_ANA1_SS0_TRXIQBUF_5G_EN_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_5G_EN_POS)
#define RF_ANA1_SS0_TRXIQBUF_5G_EN_UMSK                         (~(((1U<<RF_ANA1_SS0_TRXIQBUF_5G_EN_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_5G_EN_POS))
#define RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL                     RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL
#define RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_POS                 (8U)
#define RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_LEN                 (2U)
#define RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_MSK                 (((1U<<RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_UMSK                (~(((1U<<RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_5G_CASBIAS_SEL_POS))
#define RF_ANA1_SS0_RRF_5G_BM_SEL                               RF_ANA1_SS0_RRF_5G_BM_SEL
#define RF_ANA1_SS0_RRF_5G_BM_SEL_POS                           (16U)
#define RF_ANA1_SS0_RRF_5G_BM_SEL_LEN                           (4U)
#define RF_ANA1_SS0_RRF_5G_BM_SEL_MSK                           (((1U<<RF_ANA1_SS0_RRF_5G_BM_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_5G_BM_SEL_POS)
#define RF_ANA1_SS0_RRF_5G_BM_SEL_UMSK                          (~(((1U<<RF_ANA1_SS0_RRF_5G_BM_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_5G_BM_SEL_POS))
#define RF_ANA1_SS0_RMXGM_5G_BM_BOOST                           RF_ANA1_SS0_RMXGM_5G_BM_BOOST
#define RF_ANA1_SS0_RMXGM_5G_BM_BOOST_POS                       (20U)
#define RF_ANA1_SS0_RMXGM_5G_BM_BOOST_LEN                       (1U)
#define RF_ANA1_SS0_RMXGM_5G_BM_BOOST_MSK                       (((1U<<RF_ANA1_SS0_RMXGM_5G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_RMXGM_5G_BM_BOOST_POS)
#define RF_ANA1_SS0_RMXGM_5G_BM_BOOST_UMSK                      (~(((1U<<RF_ANA1_SS0_RMXGM_5G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_RMXGM_5G_BM_BOOST_POS))
#define RF_ANA1_SS0_LNA_5G_BM_BOOST                             RF_ANA1_SS0_LNA_5G_BM_BOOST
#define RF_ANA1_SS0_LNA_5G_BM_BOOST_POS                         (24U)
#define RF_ANA1_SS0_LNA_5G_BM_BOOST_LEN                         (1U)
#define RF_ANA1_SS0_LNA_5G_BM_BOOST_MSK                         (((1U<<RF_ANA1_SS0_LNA_5G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_BOOST_POS)
#define RF_ANA1_SS0_LNA_5G_BM_BOOST_UMSK                        (~(((1U<<RF_ANA1_SS0_LNA_5G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_BOOST_POS))
#define RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL                         RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL
#define RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_POS                     (28U)
#define RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_LEN                     (1U)
#define RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_MSK                     (((1U<<RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_POS)
#define RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_UMSK                    (~(((1U<<RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_5G_BM_BOOST_SEL_POS))

/* 0x304 : Digital Control */
#define RF_ANA1_SS0_LNA_5G_1_OFFSET                             (0x304)
#define RF_ANA1_SS0_LNA_5G_RFB3                                 RF_ANA1_SS0_LNA_5G_RFB3
#define RF_ANA1_SS0_LNA_5G_RFB3_POS                             (0U)
#define RF_ANA1_SS0_LNA_5G_RFB3_LEN                             (3U)
#define RF_ANA1_SS0_LNA_5G_RFB3_MSK                             (((1U<<RF_ANA1_SS0_LNA_5G_RFB3_LEN)-1)<<RF_ANA1_SS0_LNA_5G_RFB3_POS)
#define RF_ANA1_SS0_LNA_5G_RFB3_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_5G_RFB3_LEN)-1)<<RF_ANA1_SS0_LNA_5G_RFB3_POS))
#define RF_ANA1_SS0_LNA_5G_RFB2                                 RF_ANA1_SS0_LNA_5G_RFB2
#define RF_ANA1_SS0_LNA_5G_RFB2_POS                             (4U)
#define RF_ANA1_SS0_LNA_5G_RFB2_LEN                             (3U)
#define RF_ANA1_SS0_LNA_5G_RFB2_MSK                             (((1U<<RF_ANA1_SS0_LNA_5G_RFB2_LEN)-1)<<RF_ANA1_SS0_LNA_5G_RFB2_POS)
#define RF_ANA1_SS0_LNA_5G_RFB2_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_5G_RFB2_LEN)-1)<<RF_ANA1_SS0_LNA_5G_RFB2_POS))
#define RF_ANA1_SS0_LNA_5G_RFB1                                 RF_ANA1_SS0_LNA_5G_RFB1
#define RF_ANA1_SS0_LNA_5G_RFB1_POS                             (8U)
#define RF_ANA1_SS0_LNA_5G_RFB1_LEN                             (3U)
#define RF_ANA1_SS0_LNA_5G_RFB1_MSK                             (((1U<<RF_ANA1_SS0_LNA_5G_RFB1_LEN)-1)<<RF_ANA1_SS0_LNA_5G_RFB1_POS)
#define RF_ANA1_SS0_LNA_5G_RFB1_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_5G_RFB1_LEN)-1)<<RF_ANA1_SS0_LNA_5G_RFB1_POS))
#define RF_ANA1_SS0_LNA_5G_CAP2                                 RF_ANA1_SS0_LNA_5G_CAP2
#define RF_ANA1_SS0_LNA_5G_CAP2_POS                             (12U)
#define RF_ANA1_SS0_LNA_5G_CAP2_LEN                             (3U)
#define RF_ANA1_SS0_LNA_5G_CAP2_MSK                             (((1U<<RF_ANA1_SS0_LNA_5G_CAP2_LEN)-1)<<RF_ANA1_SS0_LNA_5G_CAP2_POS)
#define RF_ANA1_SS0_LNA_5G_CAP2_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_5G_CAP2_LEN)-1)<<RF_ANA1_SS0_LNA_5G_CAP2_POS))
#define RF_ANA1_SS0_LNA_5G_CAP1                                 RF_ANA1_SS0_LNA_5G_CAP1
#define RF_ANA1_SS0_LNA_5G_CAP1_POS                             (16U)
#define RF_ANA1_SS0_LNA_5G_CAP1_LEN                             (3U)
#define RF_ANA1_SS0_LNA_5G_CAP1_MSK                             (((1U<<RF_ANA1_SS0_LNA_5G_CAP1_LEN)-1)<<RF_ANA1_SS0_LNA_5G_CAP1_POS)
#define RF_ANA1_SS0_LNA_5G_CAP1_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_5G_CAP1_LEN)-1)<<RF_ANA1_SS0_LNA_5G_CAP1_POS))
#define RF_ANA1_SS0_LNA_5G_BM                                   RF_ANA1_SS0_LNA_5G_BM
#define RF_ANA1_SS0_LNA_5G_BM_POS                               (20U)
#define RF_ANA1_SS0_LNA_5G_BM_LEN                               (4U)
#define RF_ANA1_SS0_LNA_5G_BM_MSK                               (((1U<<RF_ANA1_SS0_LNA_5G_BM_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_POS)
#define RF_ANA1_SS0_LNA_5G_BM_UMSK                              (~(((1U<<RF_ANA1_SS0_LNA_5G_BM_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_POS))
#define RF_ANA1_SS0_LNA_5G_BM_HW                                RF_ANA1_SS0_LNA_5G_BM_HW
#define RF_ANA1_SS0_LNA_5G_BM_HW_POS                            (24U)
#define RF_ANA1_SS0_LNA_5G_BM_HW_LEN                            (4U)
#define RF_ANA1_SS0_LNA_5G_BM_HW_MSK                            (((1U<<RF_ANA1_SS0_LNA_5G_BM_HW_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_HW_POS)
#define RF_ANA1_SS0_LNA_5G_BM_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_LNA_5G_BM_HW_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_HW_POS))
#define RF_ANA1_SS0_LNA_5G_SEL                                  RF_ANA1_SS0_LNA_5G_SEL
#define RF_ANA1_SS0_LNA_5G_SEL_POS                              (28U)
#define RF_ANA1_SS0_LNA_5G_SEL_LEN                              (1U)
#define RF_ANA1_SS0_LNA_5G_SEL_MSK                              (((1U<<RF_ANA1_SS0_LNA_5G_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_5G_SEL_POS)
#define RF_ANA1_SS0_LNA_5G_SEL_UMSK                             (~(((1U<<RF_ANA1_SS0_LNA_5G_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_5G_SEL_POS))

/* 0x308 : Digital Control */
#define RF_ANA1_SS0_LNA_5G_2_OFFSET                             (0x308)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC                          RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_POS                      (0U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_LEN                      (2U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_MSK                      (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_GC_POS))
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW                             RF_ANA1_SS0_LNA_5G_LOAD_CSW
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_POS                         (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_LEN                         (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_MSK                         (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_UMSK                        (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_POS))
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW                          RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_POS                      (8U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_LEN                      (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_MSK                      (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HW_POS))
#define RF_ANA1_SS0_LNA_5G_LG_CAP                               RF_ANA1_SS0_LNA_5G_LG_CAP
#define RF_ANA1_SS0_LNA_5G_LG_CAP_POS                           (12U)
#define RF_ANA1_SS0_LNA_5G_LG_CAP_LEN                           (2U)
#define RF_ANA1_SS0_LNA_5G_LG_CAP_MSK                           (((1U<<RF_ANA1_SS0_LNA_5G_LG_CAP_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LG_CAP_POS)
#define RF_ANA1_SS0_LNA_5G_LG_CAP_UMSK                          (~(((1U<<RF_ANA1_SS0_LNA_5G_LG_CAP_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LG_CAP_POS))
#define RF_ANA1_SS0_LNA_5G_CASBIAS_SEL                          RF_ANA1_SS0_LNA_5G_CASBIAS_SEL
#define RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_POS                      (16U)
#define RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_LEN                      (2U)
#define RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_MSK                      (((1U<<RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_5G_CASBIAS_SEL_POS))

/* 0x30C : Digital Control */
#define RF_ANA1_SS0_LNA_AUX_5G_1_OFFSET                         (0x30C)
#define RF_ANA1_SS0_LNA_AUX_5G_CAS_EN                           RF_ANA1_SS0_LNA_AUX_5G_CAS_EN
#define RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_POS                       (0U)
#define RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_LEN                       (3U)
#define RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_MSK                       (((1U<<RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_UMSK                      (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CAS_EN_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_LG_CAP                           RF_ANA1_SS0_LNA_AUX_5G_LG_CAP
#define RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_POS                       (4U)
#define RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_LEN                       (2U)
#define RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_MSK                       (((1U<<RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_UMSK                      (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_LG_CAP_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL                      RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL
#define RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_POS                  (8U)
#define RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_LEN                  (2U)
#define RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_MSK                  (((1U<<RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_UMSK                 (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CASBIAS_SEL_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_RFB                              RF_ANA1_SS0_LNA_AUX_5G_RFB
#define RF_ANA1_SS0_LNA_AUX_5G_RFB_POS                          (12U)
#define RF_ANA1_SS0_LNA_AUX_5G_RFB_LEN                          (3U)
#define RF_ANA1_SS0_LNA_AUX_5G_RFB_MSK                          (((1U<<RF_ANA1_SS0_LNA_AUX_5G_RFB_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_RFB_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_RFB_UMSK                         (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_RFB_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_RFB_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_CAP2                             RF_ANA1_SS0_LNA_AUX_5G_CAP2
#define RF_ANA1_SS0_LNA_AUX_5G_CAP2_POS                         (16U)
#define RF_ANA1_SS0_LNA_AUX_5G_CAP2_LEN                         (3U)
#define RF_ANA1_SS0_LNA_AUX_5G_CAP2_MSK                         (((1U<<RF_ANA1_SS0_LNA_AUX_5G_CAP2_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CAP2_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_CAP2_UMSK                        (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_CAP2_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CAP2_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_CAP1                             RF_ANA1_SS0_LNA_AUX_5G_CAP1
#define RF_ANA1_SS0_LNA_AUX_5G_CAP1_POS                         (20U)
#define RF_ANA1_SS0_LNA_AUX_5G_CAP1_LEN                         (3U)
#define RF_ANA1_SS0_LNA_AUX_5G_CAP1_MSK                         (((1U<<RF_ANA1_SS0_LNA_AUX_5G_CAP1_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CAP1_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_CAP1_UMSK                        (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_CAP1_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_CAP1_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_BM                               RF_ANA1_SS0_LNA_AUX_5G_BM
#define RF_ANA1_SS0_LNA_AUX_5G_BM_POS                           (24U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_LEN                           (4U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_MSK                           (((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_UMSK                          (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HW                            RF_ANA1_SS0_LNA_AUX_5G_BM_HW
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HW_POS                        (28U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HW_LEN                        (4U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HW_MSK                        (((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_HW_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_HW_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HW_UMSK                       (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_HW_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_HW_POS))

/* 0x310 : Digital Control */
#define RF_ANA1_SS0_RMXGM_RMX_5G_1_OFFSET                       (0x310)
#define RF_ANA1_SS0_RMX_5G_BM                                   RF_ANA1_SS0_RMX_5G_BM
#define RF_ANA1_SS0_RMX_5G_BM_POS                               (0U)
#define RF_ANA1_SS0_RMX_5G_BM_LEN                               (3U)
#define RF_ANA1_SS0_RMX_5G_BM_MSK                               (((1U<<RF_ANA1_SS0_RMX_5G_BM_LEN)-1)<<RF_ANA1_SS0_RMX_5G_BM_POS)
#define RF_ANA1_SS0_RMX_5G_BM_UMSK                              (~(((1U<<RF_ANA1_SS0_RMX_5G_BM_LEN)-1)<<RF_ANA1_SS0_RMX_5G_BM_POS))
#define RF_ANA1_SS0_RMX_5G_BIAS_SEL                             RF_ANA1_SS0_RMX_5G_BIAS_SEL
#define RF_ANA1_SS0_RMX_5G_BIAS_SEL_POS                         (4U)
#define RF_ANA1_SS0_RMX_5G_BIAS_SEL_LEN                         (1U)
#define RF_ANA1_SS0_RMX_5G_BIAS_SEL_MSK                         (((1U<<RF_ANA1_SS0_RMX_5G_BIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMX_5G_BIAS_SEL_POS)
#define RF_ANA1_SS0_RMX_5G_BIAS_SEL_UMSK                        (~(((1U<<RF_ANA1_SS0_RMX_5G_BIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMX_5G_BIAS_SEL_POS))
#define RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL                        RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL
#define RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_POS                    (16U)
#define RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_LEN                    (2U)
#define RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_MSK                    (((1U<<RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_UMSK                   (~(((1U<<RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMXGM_5G_CASBIAS_SEL_POS))
#define RF_ANA1_SS0_RMXGM_5G_BM                                 RF_ANA1_SS0_RMXGM_5G_BM
#define RF_ANA1_SS0_RMXGM_5G_BM_POS                             (20U)
#define RF_ANA1_SS0_RMXGM_5G_BM_LEN                             (3U)
#define RF_ANA1_SS0_RMXGM_5G_BM_MSK                             (((1U<<RF_ANA1_SS0_RMXGM_5G_BM_LEN)-1)<<RF_ANA1_SS0_RMXGM_5G_BM_POS)
#define RF_ANA1_SS0_RMXGM_5G_BM_UMSK                            (~(((1U<<RF_ANA1_SS0_RMXGM_5G_BM_LEN)-1)<<RF_ANA1_SS0_RMXGM_5G_BM_POS))

/* 0x314 : Digital Control */
#define RF_ANA1_SS0_RRF_2G_OFFSET                               (0x314)
#define RF_ANA1_SS0_TRXIQBUF_2G_GC                              RF_ANA1_SS0_TRXIQBUF_2G_GC
#define RF_ANA1_SS0_TRXIQBUF_2G_GC_POS                          (0U)
#define RF_ANA1_SS0_TRXIQBUF_2G_GC_LEN                          (1U)
#define RF_ANA1_SS0_TRXIQBUF_2G_GC_MSK                          (((1U<<RF_ANA1_SS0_TRXIQBUF_2G_GC_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_2G_GC_POS)
#define RF_ANA1_SS0_TRXIQBUF_2G_GC_UMSK                         (~(((1U<<RF_ANA1_SS0_TRXIQBUF_2G_GC_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_2G_GC_POS))
#define RF_ANA1_SS0_TRXIQBUF_2G_EN                              RF_ANA1_SS0_TRXIQBUF_2G_EN
#define RF_ANA1_SS0_TRXIQBUF_2G_EN_POS                          (4U)
#define RF_ANA1_SS0_TRXIQBUF_2G_EN_LEN                          (1U)
#define RF_ANA1_SS0_TRXIQBUF_2G_EN_MSK                          (((1U<<RF_ANA1_SS0_TRXIQBUF_2G_EN_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_2G_EN_POS)
#define RF_ANA1_SS0_TRXIQBUF_2G_EN_UMSK                         (~(((1U<<RF_ANA1_SS0_TRXIQBUF_2G_EN_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_2G_EN_POS))
#define RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL                     RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL
#define RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_POS                 (8U)
#define RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_LEN                 (2U)
#define RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_MSK                 (((1U<<RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_UMSK                (~(((1U<<RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_TRXIQBUF_2G_CASBIAS_SEL_POS))
#define RF_ANA1_SS0_RRF_2G_BM_SEL                               RF_ANA1_SS0_RRF_2G_BM_SEL
#define RF_ANA1_SS0_RRF_2G_BM_SEL_POS                           (16U)
#define RF_ANA1_SS0_RRF_2G_BM_SEL_LEN                           (4U)
#define RF_ANA1_SS0_RRF_2G_BM_SEL_MSK                           (((1U<<RF_ANA1_SS0_RRF_2G_BM_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_2G_BM_SEL_POS)
#define RF_ANA1_SS0_RRF_2G_BM_SEL_UMSK                          (~(((1U<<RF_ANA1_SS0_RRF_2G_BM_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_2G_BM_SEL_POS))
#define RF_ANA1_SS0_RMXGM_2G_BM_BOOST                           RF_ANA1_SS0_RMXGM_2G_BM_BOOST
#define RF_ANA1_SS0_RMXGM_2G_BM_BOOST_POS                       (20U)
#define RF_ANA1_SS0_RMXGM_2G_BM_BOOST_LEN                       (1U)
#define RF_ANA1_SS0_RMXGM_2G_BM_BOOST_MSK                       (((1U<<RF_ANA1_SS0_RMXGM_2G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_RMXGM_2G_BM_BOOST_POS)
#define RF_ANA1_SS0_RMXGM_2G_BM_BOOST_UMSK                      (~(((1U<<RF_ANA1_SS0_RMXGM_2G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_RMXGM_2G_BM_BOOST_POS))
#define RF_ANA1_SS0_LNA_2G_BM_BOOST                             RF_ANA1_SS0_LNA_2G_BM_BOOST
#define RF_ANA1_SS0_LNA_2G_BM_BOOST_POS                         (24U)
#define RF_ANA1_SS0_LNA_2G_BM_BOOST_LEN                         (1U)
#define RF_ANA1_SS0_LNA_2G_BM_BOOST_MSK                         (((1U<<RF_ANA1_SS0_LNA_2G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_BOOST_POS)
#define RF_ANA1_SS0_LNA_2G_BM_BOOST_UMSK                        (~(((1U<<RF_ANA1_SS0_LNA_2G_BM_BOOST_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_BOOST_POS))
#define RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL                         RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL
#define RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_POS                     (28U)
#define RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_LEN                     (1U)
#define RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_MSK                     (((1U<<RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_POS)
#define RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_UMSK                    (~(((1U<<RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_LEN)-1)<<RF_ANA1_SS0_RRF_2G_BM_BOOST_SEL_POS))

/* 0x318 : Digital Control */
#define RF_ANA1_SS0_LNA_2G_1_OFFSET                             (0x318)
#define RF_ANA1_SS0_LNA_2G_RFB3                                 RF_ANA1_SS0_LNA_2G_RFB3
#define RF_ANA1_SS0_LNA_2G_RFB3_POS                             (0U)
#define RF_ANA1_SS0_LNA_2G_RFB3_LEN                             (3U)
#define RF_ANA1_SS0_LNA_2G_RFB3_MSK                             (((1U<<RF_ANA1_SS0_LNA_2G_RFB3_LEN)-1)<<RF_ANA1_SS0_LNA_2G_RFB3_POS)
#define RF_ANA1_SS0_LNA_2G_RFB3_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_2G_RFB3_LEN)-1)<<RF_ANA1_SS0_LNA_2G_RFB3_POS))
#define RF_ANA1_SS0_LNA_2G_RFB2                                 RF_ANA1_SS0_LNA_2G_RFB2
#define RF_ANA1_SS0_LNA_2G_RFB2_POS                             (4U)
#define RF_ANA1_SS0_LNA_2G_RFB2_LEN                             (3U)
#define RF_ANA1_SS0_LNA_2G_RFB2_MSK                             (((1U<<RF_ANA1_SS0_LNA_2G_RFB2_LEN)-1)<<RF_ANA1_SS0_LNA_2G_RFB2_POS)
#define RF_ANA1_SS0_LNA_2G_RFB2_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_2G_RFB2_LEN)-1)<<RF_ANA1_SS0_LNA_2G_RFB2_POS))
#define RF_ANA1_SS0_LNA_2G_RFB1                                 RF_ANA1_SS0_LNA_2G_RFB1
#define RF_ANA1_SS0_LNA_2G_RFB1_POS                             (8U)
#define RF_ANA1_SS0_LNA_2G_RFB1_LEN                             (3U)
#define RF_ANA1_SS0_LNA_2G_RFB1_MSK                             (((1U<<RF_ANA1_SS0_LNA_2G_RFB1_LEN)-1)<<RF_ANA1_SS0_LNA_2G_RFB1_POS)
#define RF_ANA1_SS0_LNA_2G_RFB1_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_2G_RFB1_LEN)-1)<<RF_ANA1_SS0_LNA_2G_RFB1_POS))
#define RF_ANA1_SS0_LNA_2G_CAP2                                 RF_ANA1_SS0_LNA_2G_CAP2
#define RF_ANA1_SS0_LNA_2G_CAP2_POS                             (12U)
#define RF_ANA1_SS0_LNA_2G_CAP2_LEN                             (3U)
#define RF_ANA1_SS0_LNA_2G_CAP2_MSK                             (((1U<<RF_ANA1_SS0_LNA_2G_CAP2_LEN)-1)<<RF_ANA1_SS0_LNA_2G_CAP2_POS)
#define RF_ANA1_SS0_LNA_2G_CAP2_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_2G_CAP2_LEN)-1)<<RF_ANA1_SS0_LNA_2G_CAP2_POS))
#define RF_ANA1_SS0_LNA_2G_CAP1                                 RF_ANA1_SS0_LNA_2G_CAP1
#define RF_ANA1_SS0_LNA_2G_CAP1_POS                             (16U)
#define RF_ANA1_SS0_LNA_2G_CAP1_LEN                             (3U)
#define RF_ANA1_SS0_LNA_2G_CAP1_MSK                             (((1U<<RF_ANA1_SS0_LNA_2G_CAP1_LEN)-1)<<RF_ANA1_SS0_LNA_2G_CAP1_POS)
#define RF_ANA1_SS0_LNA_2G_CAP1_UMSK                            (~(((1U<<RF_ANA1_SS0_LNA_2G_CAP1_LEN)-1)<<RF_ANA1_SS0_LNA_2G_CAP1_POS))
#define RF_ANA1_SS0_LNA_2G_BM                                   RF_ANA1_SS0_LNA_2G_BM
#define RF_ANA1_SS0_LNA_2G_BM_POS                               (20U)
#define RF_ANA1_SS0_LNA_2G_BM_LEN                               (4U)
#define RF_ANA1_SS0_LNA_2G_BM_MSK                               (((1U<<RF_ANA1_SS0_LNA_2G_BM_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_POS)
#define RF_ANA1_SS0_LNA_2G_BM_UMSK                              (~(((1U<<RF_ANA1_SS0_LNA_2G_BM_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_POS))
#define RF_ANA1_SS0_LNA_2G_BM_HW                                RF_ANA1_SS0_LNA_2G_BM_HW
#define RF_ANA1_SS0_LNA_2G_BM_HW_POS                            (24U)
#define RF_ANA1_SS0_LNA_2G_BM_HW_LEN                            (4U)
#define RF_ANA1_SS0_LNA_2G_BM_HW_MSK                            (((1U<<RF_ANA1_SS0_LNA_2G_BM_HW_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_HW_POS)
#define RF_ANA1_SS0_LNA_2G_BM_HW_UMSK                           (~(((1U<<RF_ANA1_SS0_LNA_2G_BM_HW_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_HW_POS))

/* 0x31C : Digital Control */
#define RF_ANA1_SS0_LNA_2G_2_OFFSET                             (0x31C)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC                          RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_POS                      (0U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_LEN                      (2U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_MSK                      (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_GC_POS))
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW                             RF_ANA1_SS0_LNA_2G_LOAD_CSW
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_POS                         (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_LEN                         (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_MSK                         (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_UMSK                        (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_POS))
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW                          RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_POS                      (8U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_LEN                      (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_MSK                      (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HW_POS))
#define RF_ANA1_SS0_LNA_2G_LG_CAP                               RF_ANA1_SS0_LNA_2G_LG_CAP
#define RF_ANA1_SS0_LNA_2G_LG_CAP_POS                           (12U)
#define RF_ANA1_SS0_LNA_2G_LG_CAP_LEN                           (2U)
#define RF_ANA1_SS0_LNA_2G_LG_CAP_MSK                           (((1U<<RF_ANA1_SS0_LNA_2G_LG_CAP_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LG_CAP_POS)
#define RF_ANA1_SS0_LNA_2G_LG_CAP_UMSK                          (~(((1U<<RF_ANA1_SS0_LNA_2G_LG_CAP_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LG_CAP_POS))
#define RF_ANA1_SS0_LNA_2G_CASBIAS_SEL                          RF_ANA1_SS0_LNA_2G_CASBIAS_SEL
#define RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_POS                      (16U)
#define RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_LEN                      (2U)
#define RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_MSK                      (((1U<<RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_LNA_2G_CASBIAS_SEL_POS))

/* 0x320 : Digital Control */
#define RF_ANA1_SS0_RMXGM_RMX_2G_1_OFFSET                       (0x320)
#define RF_ANA1_SS0_RMX_2G_BM                                   RF_ANA1_SS0_RMX_2G_BM
#define RF_ANA1_SS0_RMX_2G_BM_POS                               (0U)
#define RF_ANA1_SS0_RMX_2G_BM_LEN                               (3U)
#define RF_ANA1_SS0_RMX_2G_BM_MSK                               (((1U<<RF_ANA1_SS0_RMX_2G_BM_LEN)-1)<<RF_ANA1_SS0_RMX_2G_BM_POS)
#define RF_ANA1_SS0_RMX_2G_BM_UMSK                              (~(((1U<<RF_ANA1_SS0_RMX_2G_BM_LEN)-1)<<RF_ANA1_SS0_RMX_2G_BM_POS))
#define RF_ANA1_SS0_RMX_2G_BIAS_SEL                             RF_ANA1_SS0_RMX_2G_BIAS_SEL
#define RF_ANA1_SS0_RMX_2G_BIAS_SEL_POS                         (4U)
#define RF_ANA1_SS0_RMX_2G_BIAS_SEL_LEN                         (1U)
#define RF_ANA1_SS0_RMX_2G_BIAS_SEL_MSK                         (((1U<<RF_ANA1_SS0_RMX_2G_BIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMX_2G_BIAS_SEL_POS)
#define RF_ANA1_SS0_RMX_2G_BIAS_SEL_UMSK                        (~(((1U<<RF_ANA1_SS0_RMX_2G_BIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMX_2G_BIAS_SEL_POS))
#define RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL                        RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL
#define RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_POS                    (16U)
#define RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_LEN                    (2U)
#define RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_MSK                    (((1U<<RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_POS)
#define RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_UMSK                   (~(((1U<<RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_LEN)-1)<<RF_ANA1_SS0_RMXGM_2G_CASBIAS_SEL_POS))
#define RF_ANA1_SS0_RMXGM_2G_BM                                 RF_ANA1_SS0_RMXGM_2G_BM
#define RF_ANA1_SS0_RMXGM_2G_BM_POS                             (20U)
#define RF_ANA1_SS0_RMXGM_2G_BM_LEN                             (3U)
#define RF_ANA1_SS0_RMXGM_2G_BM_MSK                             (((1U<<RF_ANA1_SS0_RMXGM_2G_BM_LEN)-1)<<RF_ANA1_SS0_RMXGM_2G_BM_POS)
#define RF_ANA1_SS0_RMXGM_2G_BM_UMSK                            (~(((1U<<RF_ANA1_SS0_RMXGM_2G_BM_LEN)-1)<<RF_ANA1_SS0_RMXGM_2G_BM_POS))

/* 0x324 : ss0_rbb1 */
#define RF_ANA1_SS0_RBB1_OFFSET                                 (0x324)
#define RF_ANA1_SS0_ROSDAC_Q                                    RF_ANA1_SS0_ROSDAC_Q
#define RF_ANA1_SS0_ROSDAC_Q_POS                                (0U)
#define RF_ANA1_SS0_ROSDAC_Q_LEN                                (6U)
#define RF_ANA1_SS0_ROSDAC_Q_MSK                                (((1U<<RF_ANA1_SS0_ROSDAC_Q_LEN)-1)<<RF_ANA1_SS0_ROSDAC_Q_POS)
#define RF_ANA1_SS0_ROSDAC_Q_UMSK                               (~(((1U<<RF_ANA1_SS0_ROSDAC_Q_LEN)-1)<<RF_ANA1_SS0_ROSDAC_Q_POS))
#define RF_ANA1_SS0_ROSDAC_I                                    RF_ANA1_SS0_ROSDAC_I
#define RF_ANA1_SS0_ROSDAC_I_POS                                (8U)
#define RF_ANA1_SS0_ROSDAC_I_LEN                                (6U)
#define RF_ANA1_SS0_ROSDAC_I_MSK                                (((1U<<RF_ANA1_SS0_ROSDAC_I_LEN)-1)<<RF_ANA1_SS0_ROSDAC_I_POS)
#define RF_ANA1_SS0_ROSDAC_I_UMSK                               (~(((1U<<RF_ANA1_SS0_ROSDAC_I_LEN)-1)<<RF_ANA1_SS0_ROSDAC_I_POS))
#define RF_ANA1_SS0_ROSDAC_Q_HW                                 RF_ANA1_SS0_ROSDAC_Q_HW
#define RF_ANA1_SS0_ROSDAC_Q_HW_POS                             (16U)
#define RF_ANA1_SS0_ROSDAC_Q_HW_LEN                             (6U)
#define RF_ANA1_SS0_ROSDAC_Q_HW_MSK                             (((1U<<RF_ANA1_SS0_ROSDAC_Q_HW_LEN)-1)<<RF_ANA1_SS0_ROSDAC_Q_HW_POS)
#define RF_ANA1_SS0_ROSDAC_Q_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_ROSDAC_Q_HW_LEN)-1)<<RF_ANA1_SS0_ROSDAC_Q_HW_POS))
#define RF_ANA1_SS0_ROSDAC_I_HW                                 RF_ANA1_SS0_ROSDAC_I_HW
#define RF_ANA1_SS0_ROSDAC_I_HW_POS                             (24U)
#define RF_ANA1_SS0_ROSDAC_I_HW_LEN                             (6U)
#define RF_ANA1_SS0_ROSDAC_I_HW_MSK                             (((1U<<RF_ANA1_SS0_ROSDAC_I_HW_LEN)-1)<<RF_ANA1_SS0_ROSDAC_I_HW_POS)
#define RF_ANA1_SS0_ROSDAC_I_HW_UMSK                            (~(((1U<<RF_ANA1_SS0_ROSDAC_I_HW_LEN)-1)<<RF_ANA1_SS0_ROSDAC_I_HW_POS))
#define RF_ANA1_SS0_ROSDAC_RANGE                                RF_ANA1_SS0_ROSDAC_RANGE
#define RF_ANA1_SS0_ROSDAC_RANGE_POS                            (31U)
#define RF_ANA1_SS0_ROSDAC_RANGE_LEN                            (1U)
#define RF_ANA1_SS0_ROSDAC_RANGE_MSK                            (((1U<<RF_ANA1_SS0_ROSDAC_RANGE_LEN)-1)<<RF_ANA1_SS0_ROSDAC_RANGE_POS)
#define RF_ANA1_SS0_ROSDAC_RANGE_UMSK                           (~(((1U<<RF_ANA1_SS0_ROSDAC_RANGE_LEN)-1)<<RF_ANA1_SS0_ROSDAC_RANGE_POS))

/* 0x328 : ss0_rbb2 */
#define RF_ANA1_SS0_RBB2_OFFSET                                 (0x328)
#define RF_ANA1_SS0_RBB_CAP2_FC_Q                               RF_ANA1_SS0_RBB_CAP2_FC_Q
#define RF_ANA1_SS0_RBB_CAP2_FC_Q_POS                           (0U)
#define RF_ANA1_SS0_RBB_CAP2_FC_Q_LEN                           (7U)
#define RF_ANA1_SS0_RBB_CAP2_FC_Q_MSK                           (((1U<<RF_ANA1_SS0_RBB_CAP2_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP2_FC_Q_POS)
#define RF_ANA1_SS0_RBB_CAP2_FC_Q_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_CAP2_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP2_FC_Q_POS))
#define RF_ANA1_SS0_RBB_CAP2_FC_I                               RF_ANA1_SS0_RBB_CAP2_FC_I
#define RF_ANA1_SS0_RBB_CAP2_FC_I_POS                           (8U)
#define RF_ANA1_SS0_RBB_CAP2_FC_I_LEN                           (7U)
#define RF_ANA1_SS0_RBB_CAP2_FC_I_MSK                           (((1U<<RF_ANA1_SS0_RBB_CAP2_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP2_FC_I_POS)
#define RF_ANA1_SS0_RBB_CAP2_FC_I_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_CAP2_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP2_FC_I_POS))
#define RF_ANA1_SS0_RBB_CAP1_FC_Q                               RF_ANA1_SS0_RBB_CAP1_FC_Q
#define RF_ANA1_SS0_RBB_CAP1_FC_Q_POS                           (16U)
#define RF_ANA1_SS0_RBB_CAP1_FC_Q_LEN                           (7U)
#define RF_ANA1_SS0_RBB_CAP1_FC_Q_MSK                           (((1U<<RF_ANA1_SS0_RBB_CAP1_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP1_FC_Q_POS)
#define RF_ANA1_SS0_RBB_CAP1_FC_Q_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_CAP1_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP1_FC_Q_POS))
#define RF_ANA1_SS0_RBB_CAP1_FC_I                               RF_ANA1_SS0_RBB_CAP1_FC_I
#define RF_ANA1_SS0_RBB_CAP1_FC_I_POS                           (24U)
#define RF_ANA1_SS0_RBB_CAP1_FC_I_LEN                           (7U)
#define RF_ANA1_SS0_RBB_CAP1_FC_I_MSK                           (((1U<<RF_ANA1_SS0_RBB_CAP1_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP1_FC_I_POS)
#define RF_ANA1_SS0_RBB_CAP1_FC_I_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_CAP1_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP1_FC_I_POS))

/* 0x32C : ss0_rbb3 */
#define RF_ANA1_SS0_RBB3_OFFSET                                 (0x32C)
#define RF_ANA1_SS0_RBB_CGM_R_TUNE                              RF_ANA1_SS0_RBB_CGM_R_TUNE
#define RF_ANA1_SS0_RBB_CGM_R_TUNE_POS                          (0U)
#define RF_ANA1_SS0_RBB_CGM_R_TUNE_LEN                          (4U)
#define RF_ANA1_SS0_RBB_CGM_R_TUNE_MSK                          (((1U<<RF_ANA1_SS0_RBB_CGM_R_TUNE_LEN)-1)<<RF_ANA1_SS0_RBB_CGM_R_TUNE_POS)
#define RF_ANA1_SS0_RBB_CGM_R_TUNE_UMSK                         (~(((1U<<RF_ANA1_SS0_RBB_CGM_R_TUNE_LEN)-1)<<RF_ANA1_SS0_RBB_CGM_R_TUNE_POS))
#define RF_ANA1_SS0_RBB_STARTUP_SET                             RF_ANA1_SS0_RBB_STARTUP_SET
#define RF_ANA1_SS0_RBB_STARTUP_SET_POS                         (4U)
#define RF_ANA1_SS0_RBB_STARTUP_SET_LEN                         (1U)
#define RF_ANA1_SS0_RBB_STARTUP_SET_MSK                         (((1U<<RF_ANA1_SS0_RBB_STARTUP_SET_LEN)-1)<<RF_ANA1_SS0_RBB_STARTUP_SET_POS)
#define RF_ANA1_SS0_RBB_STARTUP_SET_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_STARTUP_SET_LEN)-1)<<RF_ANA1_SS0_RBB_STARTUP_SET_POS))
#define RF_ANA1_SS0_RBB_BT_FIF_TUNE                             RF_ANA1_SS0_RBB_BT_FIF_TUNE
#define RF_ANA1_SS0_RBB_BT_FIF_TUNE_POS                         (5U)
#define RF_ANA1_SS0_RBB_BT_FIF_TUNE_LEN                         (2U)
#define RF_ANA1_SS0_RBB_BT_FIF_TUNE_MSK                         (((1U<<RF_ANA1_SS0_RBB_BT_FIF_TUNE_LEN)-1)<<RF_ANA1_SS0_RBB_BT_FIF_TUNE_POS)
#define RF_ANA1_SS0_RBB_BT_FIF_TUNE_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_BT_FIF_TUNE_LEN)-1)<<RF_ANA1_SS0_RBB_BT_FIF_TUNE_POS))
#define RF_ANA1_SS0_RBB_DEQ                                     RF_ANA1_SS0_RBB_DEQ
#define RF_ANA1_SS0_RBB_DEQ_POS                                 (8U)
#define RF_ANA1_SS0_RBB_DEQ_LEN                                 (2U)
#define RF_ANA1_SS0_RBB_DEQ_MSK                                 (((1U<<RF_ANA1_SS0_RBB_DEQ_LEN)-1)<<RF_ANA1_SS0_RBB_DEQ_POS)
#define RF_ANA1_SS0_RBB_DEQ_UMSK                                (~(((1U<<RF_ANA1_SS0_RBB_DEQ_LEN)-1)<<RF_ANA1_SS0_RBB_DEQ_POS))
#define RF_ANA1_SS0_RBB_BM_OP                                   RF_ANA1_SS0_RBB_BM_OP
#define RF_ANA1_SS0_RBB_BM_OP_POS                               (12U)
#define RF_ANA1_SS0_RBB_BM_OP_LEN                               (3U)
#define RF_ANA1_SS0_RBB_BM_OP_MSK                               (((1U<<RF_ANA1_SS0_RBB_BM_OP_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP_POS)
#define RF_ANA1_SS0_RBB_BM_OP_UMSK                              (~(((1U<<RF_ANA1_SS0_RBB_BM_OP_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP_POS))
#define RF_ANA1_SS0_RBB_VCM                                     RF_ANA1_SS0_RBB_VCM
#define RF_ANA1_SS0_RBB_VCM_POS                                 (16U)
#define RF_ANA1_SS0_RBB_VCM_LEN                                 (2U)
#define RF_ANA1_SS0_RBB_VCM_MSK                                 (((1U<<RF_ANA1_SS0_RBB_VCM_LEN)-1)<<RF_ANA1_SS0_RBB_VCM_POS)
#define RF_ANA1_SS0_RBB_VCM_UMSK                                (~(((1U<<RF_ANA1_SS0_RBB_VCM_LEN)-1)<<RF_ANA1_SS0_RBB_VCM_POS))
#define RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT                         RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT
#define RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_POS                     (20U)
#define RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_LEN                     (1U)
#define RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_MSK                     (((1U<<RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_POS)
#define RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_SS0_RBB_BQ_IQBIAS_SHORT_POS))
#define RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT                        RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT
#define RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_POS                    (21U)
#define RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_LEN                    (1U)
#define RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_MSK                    (((1U<<RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_POS)
#define RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_UMSK                   (~(((1U<<RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_ANA1_SS0_RBB_TIA_IQBIAS_SHORT_POS))
#define RF_ANA1_SS0_RBB_BW                                      RF_ANA1_SS0_RBB_BW
#define RF_ANA1_SS0_RBB_BW_POS                                  (24U)
#define RF_ANA1_SS0_RBB_BW_LEN                                  (2U)
#define RF_ANA1_SS0_RBB_BW_MSK                                  (((1U<<RF_ANA1_SS0_RBB_BW_LEN)-1)<<RF_ANA1_SS0_RBB_BW_POS)
#define RF_ANA1_SS0_RBB_BW_UMSK                                 (~(((1U<<RF_ANA1_SS0_RBB_BW_LEN)-1)<<RF_ANA1_SS0_RBB_BW_POS))
#define RF_ANA1_SS0_PWR_DET_EN_HW                               RF_ANA1_SS0_PWR_DET_EN_HW
#define RF_ANA1_SS0_PWR_DET_EN_HW_POS                           (29U)
#define RF_ANA1_SS0_PWR_DET_EN_HW_LEN                           (1U)
#define RF_ANA1_SS0_PWR_DET_EN_HW_MSK                           (((1U<<RF_ANA1_SS0_PWR_DET_EN_HW_LEN)-1)<<RF_ANA1_SS0_PWR_DET_EN_HW_POS)
#define RF_ANA1_SS0_PWR_DET_EN_HW_UMSK                          (~(((1U<<RF_ANA1_SS0_PWR_DET_EN_HW_LEN)-1)<<RF_ANA1_SS0_PWR_DET_EN_HW_POS))
#define RF_ANA1_SS0_PWR_DET_EN                                  RF_ANA1_SS0_PWR_DET_EN
#define RF_ANA1_SS0_PWR_DET_EN_POS                              (31U)
#define RF_ANA1_SS0_PWR_DET_EN_LEN                              (1U)
#define RF_ANA1_SS0_PWR_DET_EN_MSK                              (((1U<<RF_ANA1_SS0_PWR_DET_EN_LEN)-1)<<RF_ANA1_SS0_PWR_DET_EN_POS)
#define RF_ANA1_SS0_PWR_DET_EN_UMSK                             (~(((1U<<RF_ANA1_SS0_PWR_DET_EN_LEN)-1)<<RF_ANA1_SS0_PWR_DET_EN_POS))

/* 0x330 : ss0_rbb4 */
#define RF_ANA1_SS0_RBB4_OFFSET                                 (0x330)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_I                            RF_ANA1_SS0_RBB_CAP_DPD_FC_I
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_I_POS                        (0U)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_I_LEN                        (4U)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_I_MSK                        (((1U<<RF_ANA1_SS0_RBB_CAP_DPD_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_DPD_FC_I_POS)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_I_UMSK                       (~(((1U<<RF_ANA1_SS0_RBB_CAP_DPD_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_DPD_FC_I_POS))
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_Q                            RF_ANA1_SS0_RBB_CAP_DPD_FC_Q
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_POS                        (4U)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_LEN                        (4U)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_MSK                        (((1U<<RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_POS)
#define RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_UMSK                       (~(((1U<<RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_DPD_FC_Q_POS))
#define RF_ANA1_SS0_RBB_BM_OP1                                  RF_ANA1_SS0_RBB_BM_OP1
#define RF_ANA1_SS0_RBB_BM_OP1_POS                              (8U)
#define RF_ANA1_SS0_RBB_BM_OP1_LEN                              (2U)
#define RF_ANA1_SS0_RBB_BM_OP1_MSK                              (((1U<<RF_ANA1_SS0_RBB_BM_OP1_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP1_POS)
#define RF_ANA1_SS0_RBB_BM_OP1_UMSK                             (~(((1U<<RF_ANA1_SS0_RBB_BM_OP1_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP1_POS))
#define RF_ANA1_SS0_RBB_BM_OP2                                  RF_ANA1_SS0_RBB_BM_OP2
#define RF_ANA1_SS0_RBB_BM_OP2_POS                              (10U)
#define RF_ANA1_SS0_RBB_BM_OP2_LEN                              (2U)
#define RF_ANA1_SS0_RBB_BM_OP2_MSK                              (((1U<<RF_ANA1_SS0_RBB_BM_OP2_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP2_POS)
#define RF_ANA1_SS0_RBB_BM_OP2_UMSK                             (~(((1U<<RF_ANA1_SS0_RBB_BM_OP2_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP2_POS))
#define RF_ANA1_SS0_RBB_BM_OP3                                  RF_ANA1_SS0_RBB_BM_OP3
#define RF_ANA1_SS0_RBB_BM_OP3_POS                              (12U)
#define RF_ANA1_SS0_RBB_BM_OP3_LEN                              (2U)
#define RF_ANA1_SS0_RBB_BM_OP3_MSK                              (((1U<<RF_ANA1_SS0_RBB_BM_OP3_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP3_POS)
#define RF_ANA1_SS0_RBB_BM_OP3_UMSK                             (~(((1U<<RF_ANA1_SS0_RBB_BM_OP3_LEN)-1)<<RF_ANA1_SS0_RBB_BM_OP3_POS))
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R5                           RF_ANA1_SS0_RBB_ATEST_GAIN_R5
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R5_POS                       (16U)
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R5_LEN                       (4U)
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R5_MSK                       (((1U<<RF_ANA1_SS0_RBB_ATEST_GAIN_R5_LEN)-1)<<RF_ANA1_SS0_RBB_ATEST_GAIN_R5_POS)
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R5_UMSK                      (~(((1U<<RF_ANA1_SS0_RBB_ATEST_GAIN_R5_LEN)-1)<<RF_ANA1_SS0_RBB_ATEST_GAIN_R5_POS))
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R6                           RF_ANA1_SS0_RBB_ATEST_GAIN_R6
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R6_POS                       (24U)
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R6_LEN                       (3U)
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R6_MSK                       (((1U<<RF_ANA1_SS0_RBB_ATEST_GAIN_R6_LEN)-1)<<RF_ANA1_SS0_RBB_ATEST_GAIN_R6_POS)
#define RF_ANA1_SS0_RBB_ATEST_GAIN_R6_UMSK                      (~(((1U<<RF_ANA1_SS0_RBB_ATEST_GAIN_R6_LEN)-1)<<RF_ANA1_SS0_RBB_ATEST_GAIN_R6_POS))
#define RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN                      RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN
#define RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_POS                  (27U)
#define RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_LEN                  (1U)
#define RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_MSK                  (((1U<<RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_LEN)-1)<<RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_POS)
#define RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_UMSK                 (~(((1U<<RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_LEN)-1)<<RF_ANA1_SS0_ATEST_WIFI_RBB_TEST_EN_POS))
#define RF_ANA1_SS0_RBB_BT_MODE                                 RF_ANA1_SS0_RBB_BT_MODE
#define RF_ANA1_SS0_RBB_BT_MODE_POS                             (28U)
#define RF_ANA1_SS0_RBB_BT_MODE_LEN                             (1U)
#define RF_ANA1_SS0_RBB_BT_MODE_MSK                             (((1U<<RF_ANA1_SS0_RBB_BT_MODE_LEN)-1)<<RF_ANA1_SS0_RBB_BT_MODE_POS)
#define RF_ANA1_SS0_RBB_BT_MODE_UMSK                            (~(((1U<<RF_ANA1_SS0_RBB_BT_MODE_LEN)-1)<<RF_ANA1_SS0_RBB_BT_MODE_POS))
#define RF_ANA1_SS0_RBB_BT_MODE_HW                              RF_ANA1_SS0_RBB_BT_MODE_HW
#define RF_ANA1_SS0_RBB_BT_MODE_HW_POS                          (29U)
#define RF_ANA1_SS0_RBB_BT_MODE_HW_LEN                          (1U)
#define RF_ANA1_SS0_RBB_BT_MODE_HW_MSK                          (((1U<<RF_ANA1_SS0_RBB_BT_MODE_HW_LEN)-1)<<RF_ANA1_SS0_RBB_BT_MODE_HW_POS)
#define RF_ANA1_SS0_RBB_BT_MODE_HW_UMSK                         (~(((1U<<RF_ANA1_SS0_RBB_BT_MODE_HW_LEN)-1)<<RF_ANA1_SS0_RBB_BT_MODE_HW_POS))
#define RF_ANA1_SS0_RBB_ATEST_IN_EN                             RF_ANA1_SS0_RBB_ATEST_IN_EN
#define RF_ANA1_SS0_RBB_ATEST_IN_EN_POS                         (30U)
#define RF_ANA1_SS0_RBB_ATEST_IN_EN_LEN                         (1U)
#define RF_ANA1_SS0_RBB_ATEST_IN_EN_MSK                         (((1U<<RF_ANA1_SS0_RBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_SS0_RBB_ATEST_IN_EN_POS)
#define RF_ANA1_SS0_RBB_ATEST_IN_EN_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_ATEST_IN_EN_LEN)-1)<<RF_ANA1_SS0_RBB_ATEST_IN_EN_POS))
#define RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP                           RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP
#define RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_POS                       (31U)
#define RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_LEN                       (1U)
#define RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_MSK                       (((1U<<RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_LEN)-1)<<RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_POS)
#define RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_UMSK                      (~(((1U<<RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_LEN)-1)<<RF_ANA1_SS0_RBB_RCCAL_IQ_SWAP_POS))

/* 0x334 : ss0_rbb5 */
#define RF_ANA1_SS0_RBB5_OFFSET                                 (0x334)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I                         RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_POS                     (0U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_LEN                     (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_MSK                     (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q                         RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_POS                     (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_LEN                     (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_MSK                     (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I                         RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_POS                     (8U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_LEN                     (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_MSK                     (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q                         RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_POS                     (14U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_LEN                     (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_MSK                     (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I                         RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_POS                     (20U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_LEN                     (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_MSK                     (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q                         RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_POS                     (26U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_LEN                     (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_MSK                     (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_UMSK                    (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_POS))

/* 0x338 : ss0_rbb6 */
#define RF_ANA1_SS0_RBB6_OFFSET                                 (0x338)
#define RF_ANA1_SS0_RBB_RZ_OP1_I                                RF_ANA1_SS0_RBB_RZ_OP1_I
#define RF_ANA1_SS0_RBB_RZ_OP1_I_POS                            (0U)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_LEN                            (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_MSK                            (((1U<<RF_ANA1_SS0_RBB_RZ_OP1_I_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_I_POS)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_UMSK                           (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP1_I_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_I_POS))
#define RF_ANA1_SS0_RBB_RZ_OP1_Q                                RF_ANA1_SS0_RBB_RZ_OP1_Q
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_POS                            (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_LEN                            (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_MSK                            (((1U<<RF_ANA1_SS0_RBB_RZ_OP1_Q_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_Q_POS)
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_UMSK                           (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP1_Q_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_Q_POS))
#define RF_ANA1_SS0_RBB_RZ_OP2_I                                RF_ANA1_SS0_RBB_RZ_OP2_I
#define RF_ANA1_SS0_RBB_RZ_OP2_I_POS                            (8U)
#define RF_ANA1_SS0_RBB_RZ_OP2_I_LEN                            (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_I_MSK                            (((1U<<RF_ANA1_SS0_RBB_RZ_OP2_I_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_I_POS)
#define RF_ANA1_SS0_RBB_RZ_OP2_I_UMSK                           (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP2_I_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_I_POS))
#define RF_ANA1_SS0_RBB_RZ_OP2_Q                                RF_ANA1_SS0_RBB_RZ_OP2_Q
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_POS                            (12U)
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_LEN                            (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_MSK                            (((1U<<RF_ANA1_SS0_RBB_RZ_OP2_Q_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_Q_POS)
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_UMSK                           (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP2_Q_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_Q_POS))
#define RF_ANA1_SS0_RBB_RZ_OP3_I                                RF_ANA1_SS0_RBB_RZ_OP3_I
#define RF_ANA1_SS0_RBB_RZ_OP3_I_POS                            (16U)
#define RF_ANA1_SS0_RBB_RZ_OP3_I_LEN                            (2U)
#define RF_ANA1_SS0_RBB_RZ_OP3_I_MSK                            (((1U<<RF_ANA1_SS0_RBB_RZ_OP3_I_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_I_POS)
#define RF_ANA1_SS0_RBB_RZ_OP3_I_UMSK                           (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP3_I_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_I_POS))
#define RF_ANA1_SS0_RBB_RZ_OP3_Q                                RF_ANA1_SS0_RBB_RZ_OP3_Q
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_POS                            (20U)
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_LEN                            (2U)
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_MSK                            (((1U<<RF_ANA1_SS0_RBB_RZ_OP3_Q_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_Q_POS)
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_UMSK                           (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP3_Q_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_Q_POS))

/* 0x33C : ss0_rbb7 */
#define RF_ANA1_SS0_RBB7_OFFSET                                 (0x33C)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW                      RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_POS                  (0U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_LEN                  (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_I_HW_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW                      RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_POS                  (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_LEN                  (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_Q_HW_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW                      RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_POS                  (8U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_LEN                  (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_I_HW_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW                      RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_POS                  (14U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_LEN                  (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_Q_HW_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW                      RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_POS                  (20U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_LEN                  (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_I_HW_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW                      RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_POS                  (26U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_LEN                  (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_Q_HW_POS))

/* 0x340 : ss0_rbb8 */
#define RF_ANA1_SS0_RBB8_OFFSET                                 (0x340)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_HW                             RF_ANA1_SS0_RBB_RZ_OP1_I_HW
#define RF_ANA1_SS0_RBB_RZ_OP1_I_HW_POS                         (0U)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_HW_LEN                         (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_HW_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP1_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_I_HW_POS)
#define RF_ANA1_SS0_RBB_RZ_OP1_I_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP1_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_I_HW_POS))
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_HW                             RF_ANA1_SS0_RBB_RZ_OP1_Q_HW
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_POS                         (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_LEN                         (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_POS)
#define RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_Q_HW_POS))
#define RF_ANA1_SS0_RBB_RZ_OP2_I_HW                             RF_ANA1_SS0_RBB_RZ_OP2_I_HW
#define RF_ANA1_SS0_RBB_RZ_OP2_I_HW_POS                         (8U)
#define RF_ANA1_SS0_RBB_RZ_OP2_I_HW_LEN                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_I_HW_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP2_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_I_HW_POS)
#define RF_ANA1_SS0_RBB_RZ_OP2_I_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP2_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_I_HW_POS))
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_HW                             RF_ANA1_SS0_RBB_RZ_OP2_Q_HW
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_POS                         (12U)
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_LEN                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_POS)
#define RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_Q_HW_POS))
#define RF_ANA1_SS0_RBB_RZ_OP3_I_HW                             RF_ANA1_SS0_RBB_RZ_OP3_I_HW
#define RF_ANA1_SS0_RBB_RZ_OP3_I_HW_POS                         (16U)
#define RF_ANA1_SS0_RBB_RZ_OP3_I_HW_LEN                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP3_I_HW_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP3_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_I_HW_POS)
#define RF_ANA1_SS0_RBB_RZ_OP3_I_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP3_I_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_I_HW_POS))
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_HW                             RF_ANA1_SS0_RBB_RZ_OP3_Q_HW
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_POS                         (20U)
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_LEN                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_POS)
#define RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_Q_HW_POS))

/* 0x344 : ss0_rssibuf1 */
#define RF_ANA1_SS0_RSSIBUF1_OFFSET                             (0x344)
#define RF_ANA1_SS0_RSSIBUF_I_EN                                RF_ANA1_SS0_RSSIBUF_I_EN
#define RF_ANA1_SS0_RSSIBUF_I_EN_POS                            (0U)
#define RF_ANA1_SS0_RSSIBUF_I_EN_LEN                            (1U)
#define RF_ANA1_SS0_RSSIBUF_I_EN_MSK                            (((1U<<RF_ANA1_SS0_RSSIBUF_I_EN_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_I_EN_POS)
#define RF_ANA1_SS0_RSSIBUF_I_EN_UMSK                           (~(((1U<<RF_ANA1_SS0_RSSIBUF_I_EN_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_I_EN_POS))
#define RF_ANA1_SS0_RSSIBUF_Q_EN                                RF_ANA1_SS0_RSSIBUF_Q_EN
#define RF_ANA1_SS0_RSSIBUF_Q_EN_POS                            (1U)
#define RF_ANA1_SS0_RSSIBUF_Q_EN_LEN                            (1U)
#define RF_ANA1_SS0_RSSIBUF_Q_EN_MSK                            (((1U<<RF_ANA1_SS0_RSSIBUF_Q_EN_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_Q_EN_POS)
#define RF_ANA1_SS0_RSSIBUF_Q_EN_UMSK                           (~(((1U<<RF_ANA1_SS0_RSSIBUF_Q_EN_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_Q_EN_POS))
#define RF_ANA1_SS0_RSSIBUF_GAIN_RF                             RF_ANA1_SS0_RSSIBUF_GAIN_RF
#define RF_ANA1_SS0_RSSIBUF_GAIN_RF_POS                         (4U)
#define RF_ANA1_SS0_RSSIBUF_GAIN_RF_LEN                         (4U)
#define RF_ANA1_SS0_RSSIBUF_GAIN_RF_MSK                         (((1U<<RF_ANA1_SS0_RSSIBUF_GAIN_RF_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_GAIN_RF_POS)
#define RF_ANA1_SS0_RSSIBUF_GAIN_RF_UMSK                        (~(((1U<<RF_ANA1_SS0_RSSIBUF_GAIN_RF_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_GAIN_RF_POS))
#define RF_ANA1_SS0_RSSIBUF_GAIN_RIN                            RF_ANA1_SS0_RSSIBUF_GAIN_RIN
#define RF_ANA1_SS0_RSSIBUF_GAIN_RIN_POS                        (8U)
#define RF_ANA1_SS0_RSSIBUF_GAIN_RIN_LEN                        (3U)
#define RF_ANA1_SS0_RSSIBUF_GAIN_RIN_MSK                        (((1U<<RF_ANA1_SS0_RSSIBUF_GAIN_RIN_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_GAIN_RIN_POS)
#define RF_ANA1_SS0_RSSIBUF_GAIN_RIN_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIBUF_GAIN_RIN_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_GAIN_RIN_POS))
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_I                            RF_ANA1_SS0_RSSIBUF_CAP_FC_I
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_I_POS                        (20U)
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_I_LEN                        (4U)
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_I_MSK                        (((1U<<RF_ANA1_SS0_RSSIBUF_CAP_FC_I_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_FC_I_POS)
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_I_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIBUF_CAP_FC_I_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_FC_I_POS))
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_Q                            RF_ANA1_SS0_RSSIBUF_CAP_FC_Q
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_POS                        (24U)
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_LEN                        (4U)
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_MSK                        (((1U<<RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_POS)
#define RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_FC_Q_POS))

/* 0x348 : ss0_rssibuf2 */
#define RF_ANA1_SS0_RSSIBUF2_OFFSET                             (0x348)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_I                             RF_ANA1_SS0_RSSIBUF_RZ_OP_I
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_I_POS                         (8U)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_I_LEN                         (2U)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_I_MSK                         (((1U<<RF_ANA1_SS0_RSSIBUF_RZ_OP_I_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_RZ_OP_I_POS)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_I_UMSK                        (~(((1U<<RF_ANA1_SS0_RSSIBUF_RZ_OP_I_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_RZ_OP_I_POS))
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_Q                             RF_ANA1_SS0_RSSIBUF_RZ_OP_Q
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_POS                         (12U)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_LEN                         (2U)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_MSK                         (((1U<<RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_POS)
#define RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_UMSK                        (~(((1U<<RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_RZ_OP_Q_POS))
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I                      RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_POS                  (16U)
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_LEN                  (5U)
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_MSK                  (((1U<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_POS)
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_UMSK                 (~(((1U<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_I_POS))
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q                      RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_POS                  (24U)
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_LEN                  (5U)
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_MSK                  (((1U<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_POS)
#define RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_UMSK                 (~(((1U<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_LEN)-1)<<RF_ANA1_SS0_RSSIBUF_CAP_CC_OP_FC_Q_POS))

/* 0x34C : ss0_rssiadc_0 */
#define RF_ANA1_SS0_RSSIADC_0_OFFSET                            (0x34C)
#define RF_ANA1_SS0_RSSIADC_CLK_EN                              RF_ANA1_SS0_RSSIADC_CLK_EN
#define RF_ANA1_SS0_RSSIADC_CLK_EN_POS                          (0U)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_LEN                          (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_MSK                          (((1U<<RF_ANA1_SS0_RSSIADC_CLK_EN_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_EN_POS)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_UMSK                         (~(((1U<<RF_ANA1_SS0_RSSIADC_CLK_EN_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_EN_POS))
#define RF_ANA1_SS0_RSSIADC_CLK_EN_I                            RF_ANA1_SS0_RSSIADC_CLK_EN_I
#define RF_ANA1_SS0_RSSIADC_CLK_EN_I_POS                        (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_I_LEN                        (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_I_MSK                        (((1U<<RF_ANA1_SS0_RSSIADC_CLK_EN_I_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_EN_I_POS)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_I_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIADC_CLK_EN_I_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_EN_I_POS))
#define RF_ANA1_SS0_RSSIADC_CLK_EN_Q                            RF_ANA1_SS0_RSSIADC_CLK_EN_Q
#define RF_ANA1_SS0_RSSIADC_CLK_EN_Q_POS                        (2U)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_Q_LEN                        (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_Q_MSK                        (((1U<<RF_ANA1_SS0_RSSIADC_CLK_EN_Q_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_EN_Q_POS)
#define RF_ANA1_SS0_RSSIADC_CLK_EN_Q_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIADC_CLK_EN_Q_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_EN_Q_POS))
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CK                          RF_ANA1_SS0_RSSIADC_DLY_CTL_CK
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_POS                      (4U)
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_LEN                      (3U)
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_MSK                      (((1U<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_LEN)-1)<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_POS)
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_UMSK                     (~(((1U<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_LEN)-1)<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CK_POS))
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC                    RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_POS                (8U)
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_LEN                (2U)
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_MSK                (((1U<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_LEN)-1)<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_POS)
#define RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_UMSK               (~(((1U<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_LEN)-1)<<RF_ANA1_SS0_RSSIADC_DLY_CTL_CLKASYNC_POS))
#define RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV                        RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV
#define RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_POS                    (12U)
#define RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_LEN                    (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_MSK                    (((1U<<RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_POS)
#define RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_UMSK                   (~(((1U<<RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_SYNC_INV_POS))
#define RF_ANA1_SS0_RSSIADC_CLK_INV                             RF_ANA1_SS0_RSSIADC_CLK_INV
#define RF_ANA1_SS0_RSSIADC_CLK_INV_POS                         (13U)
#define RF_ANA1_SS0_RSSIADC_CLK_INV_LEN                         (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_INV_MSK                         (((1U<<RF_ANA1_SS0_RSSIADC_CLK_INV_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_INV_POS)
#define RF_ANA1_SS0_RSSIADC_CLK_INV_UMSK                        (~(((1U<<RF_ANA1_SS0_RSSIADC_CLK_INV_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_INV_POS))
#define RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL                         RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL
#define RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_POS                     (14U)
#define RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_LEN                     (1U)
#define RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_MSK                     (((1U<<RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_POS)
#define RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_UMSK                    (~(((1U<<RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_CLK_DIV_SEL_POS))
#define RF_ANA1_SS0_RSSIADC_VBUF_SEL                            RF_ANA1_SS0_RSSIADC_VBUF_SEL
#define RF_ANA1_SS0_RSSIADC_VBUF_SEL_POS                        (16U)
#define RF_ANA1_SS0_RSSIADC_VBUF_SEL_LEN                        (2U)
#define RF_ANA1_SS0_RSSIADC_VBUF_SEL_MSK                        (((1U<<RF_ANA1_SS0_RSSIADC_VBUF_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_VBUF_SEL_POS)
#define RF_ANA1_SS0_RSSIADC_VBUF_SEL_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIADC_VBUF_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_VBUF_SEL_POS))
#define RF_ANA1_SS0_RSSIADC_VCM_SEL                             RF_ANA1_SS0_RSSIADC_VCM_SEL
#define RF_ANA1_SS0_RSSIADC_VCM_SEL_POS                         (20U)
#define RF_ANA1_SS0_RSSIADC_VCM_SEL_LEN                         (2U)
#define RF_ANA1_SS0_RSSIADC_VCM_SEL_MSK                         (((1U<<RF_ANA1_SS0_RSSIADC_VCM_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_VCM_SEL_POS)
#define RF_ANA1_SS0_RSSIADC_VCM_SEL_UMSK                        (~(((1U<<RF_ANA1_SS0_RSSIADC_VCM_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_VCM_SEL_POS))
#define RF_ANA1_SS0_RSSIADC_VREF_SEL                            RF_ANA1_SS0_RSSIADC_VREF_SEL
#define RF_ANA1_SS0_RSSIADC_VREF_SEL_POS                        (24U)
#define RF_ANA1_SS0_RSSIADC_VREF_SEL_LEN                        (2U)
#define RF_ANA1_SS0_RSSIADC_VREF_SEL_MSK                        (((1U<<RF_ANA1_SS0_RSSIADC_VREF_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_VREF_SEL_POS)
#define RF_ANA1_SS0_RSSIADC_VREF_SEL_UMSK                       (~(((1U<<RF_ANA1_SS0_RSSIADC_VREF_SEL_LEN)-1)<<RF_ANA1_SS0_RSSIADC_VREF_SEL_POS))

/* 0x354 : Digital Control */
#define RF_ANA1_SS0_RXADC_0_OFFSET                              (0x354)
#define RF_ANA1_SS0_RXADC_VREF_SEL                              RF_ANA1_SS0_RXADC_VREF_SEL
#define RF_ANA1_SS0_RXADC_VREF_SEL_POS                          (0U)
#define RF_ANA1_SS0_RXADC_VREF_SEL_LEN                          (2U)
#define RF_ANA1_SS0_RXADC_VREF_SEL_MSK                          (((1U<<RF_ANA1_SS0_RXADC_VREF_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_VREF_SEL_POS)
#define RF_ANA1_SS0_RXADC_VREF_SEL_UMSK                         (~(((1U<<RF_ANA1_SS0_RXADC_VREF_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_VREF_SEL_POS))
#define RF_ANA1_SS0_RXADC_DLY_CTL                               RF_ANA1_SS0_RXADC_DLY_CTL
#define RF_ANA1_SS0_RXADC_DLY_CTL_POS                           (4U)
#define RF_ANA1_SS0_RXADC_DLY_CTL_LEN                           (2U)
#define RF_ANA1_SS0_RXADC_DLY_CTL_MSK                           (((1U<<RF_ANA1_SS0_RXADC_DLY_CTL_LEN)-1)<<RF_ANA1_SS0_RXADC_DLY_CTL_POS)
#define RF_ANA1_SS0_RXADC_DLY_CTL_UMSK                          (~(((1U<<RF_ANA1_SS0_RXADC_DLY_CTL_LEN)-1)<<RF_ANA1_SS0_RXADC_DLY_CTL_POS))
#define RF_ANA1_SS0_RXADC_DVDD_SEL                              RF_ANA1_SS0_RXADC_DVDD_SEL
#define RF_ANA1_SS0_RXADC_DVDD_SEL_POS                          (8U)
#define RF_ANA1_SS0_RXADC_DVDD_SEL_LEN                          (2U)
#define RF_ANA1_SS0_RXADC_DVDD_SEL_MSK                          (((1U<<RF_ANA1_SS0_RXADC_DVDD_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_DVDD_SEL_POS)
#define RF_ANA1_SS0_RXADC_DVDD_SEL_UMSK                         (~(((1U<<RF_ANA1_SS0_RXADC_DVDD_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_DVDD_SEL_POS))
#define RF_ANA1_SS0_RXADC_GT_RM                                 RF_ANA1_SS0_RXADC_GT_RM
#define RF_ANA1_SS0_RXADC_GT_RM_POS                             (12U)
#define RF_ANA1_SS0_RXADC_GT_RM_LEN                             (1U)
#define RF_ANA1_SS0_RXADC_GT_RM_MSK                             (((1U<<RF_ANA1_SS0_RXADC_GT_RM_LEN)-1)<<RF_ANA1_SS0_RXADC_GT_RM_POS)
#define RF_ANA1_SS0_RXADC_GT_RM_UMSK                            (~(((1U<<RF_ANA1_SS0_RXADC_GT_RM_LEN)-1)<<RF_ANA1_SS0_RXADC_GT_RM_POS))
#define RF_ANA1_SS0_RXADC_CLK_SYNC_INV                          RF_ANA1_SS0_RXADC_CLK_SYNC_INV
#define RF_ANA1_SS0_RXADC_CLK_SYNC_INV_POS                      (13U)
#define RF_ANA1_SS0_RXADC_CLK_SYNC_INV_LEN                      (1U)
#define RF_ANA1_SS0_RXADC_CLK_SYNC_INV_MSK                      (((1U<<RF_ANA1_SS0_RXADC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_SYNC_INV_POS)
#define RF_ANA1_SS0_RXADC_CLK_SYNC_INV_UMSK                     (~(((1U<<RF_ANA1_SS0_RXADC_CLK_SYNC_INV_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_SYNC_INV_POS))
#define RF_ANA1_SS0_RXADC_CLK_SYNC_SEL                          RF_ANA1_SS0_RXADC_CLK_SYNC_SEL
#define RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_POS                      (14U)
#define RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_LEN                      (1U)
#define RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_MSK                      (((1U<<RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_POS)
#define RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_UMSK                     (~(((1U<<RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_SYNC_SEL_POS))
#define RF_ANA1_SS0_RXADC_CLK_INV                               RF_ANA1_SS0_RXADC_CLK_INV
#define RF_ANA1_SS0_RXADC_CLK_INV_POS                           (16U)
#define RF_ANA1_SS0_RXADC_CLK_INV_LEN                           (1U)
#define RF_ANA1_SS0_RXADC_CLK_INV_MSK                           (((1U<<RF_ANA1_SS0_RXADC_CLK_INV_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_INV_POS)
#define RF_ANA1_SS0_RXADC_CLK_INV_UMSK                          (~(((1U<<RF_ANA1_SS0_RXADC_CLK_INV_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_INV_POS))
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL                           RF_ANA1_SS0_RXADC_CLK_DIV_SEL
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_POS                       (20U)
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_LEN                       (1U)
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_MSK                       (((1U<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_POS)
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_UMSK                      (~(((1U<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_POS))
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW                        RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_POS                    (22U)
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_LEN                    (1U)
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_MSK                    (((1U<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_POS)
#define RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_UMSK                   (~(((1U<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_DIV_SEL_HW_POS))
#define RF_ANA1_SS0_RXADC_VREF_CP_EN                            RF_ANA1_SS0_RXADC_VREF_CP_EN
#define RF_ANA1_SS0_RXADC_VREF_CP_EN_POS                        (24U)
#define RF_ANA1_SS0_RXADC_VREF_CP_EN_LEN                        (1U)
#define RF_ANA1_SS0_RXADC_VREF_CP_EN_MSK                        (((1U<<RF_ANA1_SS0_RXADC_VREF_CP_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_VREF_CP_EN_POS)
#define RF_ANA1_SS0_RXADC_VREF_CP_EN_UMSK                       (~(((1U<<RF_ANA1_SS0_RXADC_VREF_CP_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_VREF_CP_EN_POS))
#define RF_ANA1_SS0_RXADC_TI_EN                                 RF_ANA1_SS0_RXADC_TI_EN
#define RF_ANA1_SS0_RXADC_TI_EN_POS                             (25U)
#define RF_ANA1_SS0_RXADC_TI_EN_LEN                             (1U)
#define RF_ANA1_SS0_RXADC_TI_EN_MSK                             (((1U<<RF_ANA1_SS0_RXADC_TI_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_TI_EN_POS)
#define RF_ANA1_SS0_RXADC_TI_EN_UMSK                            (~(((1U<<RF_ANA1_SS0_RXADC_TI_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_TI_EN_POS))
#define RF_ANA1_SS0_RXADC_TI_EN_HW                              RF_ANA1_SS0_RXADC_TI_EN_HW
#define RF_ANA1_SS0_RXADC_TI_EN_HW_POS                          (27U)
#define RF_ANA1_SS0_RXADC_TI_EN_HW_LEN                          (1U)
#define RF_ANA1_SS0_RXADC_TI_EN_HW_MSK                          (((1U<<RF_ANA1_SS0_RXADC_TI_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_TI_EN_HW_POS)
#define RF_ANA1_SS0_RXADC_TI_EN_HW_UMSK                         (~(((1U<<RF_ANA1_SS0_RXADC_TI_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_TI_EN_HW_POS))
#define RF_ANA1_SS0_RXADC_VBUF_SEL                              RF_ANA1_SS0_RXADC_VBUF_SEL
#define RF_ANA1_SS0_RXADC_VBUF_SEL_POS                          (28U)
#define RF_ANA1_SS0_RXADC_VBUF_SEL_LEN                          (2U)
#define RF_ANA1_SS0_RXADC_VBUF_SEL_MSK                          (((1U<<RF_ANA1_SS0_RXADC_VBUF_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_VBUF_SEL_POS)
#define RF_ANA1_SS0_RXADC_VBUF_SEL_UMSK                         (~(((1U<<RF_ANA1_SS0_RXADC_VBUF_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_VBUF_SEL_POS))
#define RF_ANA1_SS0_RXADC_CLK_EN                                RF_ANA1_SS0_RXADC_CLK_EN
#define RF_ANA1_SS0_RXADC_CLK_EN_POS                            (30U)
#define RF_ANA1_SS0_RXADC_CLK_EN_LEN                            (1U)
#define RF_ANA1_SS0_RXADC_CLK_EN_MSK                            (((1U<<RF_ANA1_SS0_RXADC_CLK_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_EN_POS)
#define RF_ANA1_SS0_RXADC_CLK_EN_UMSK                           (~(((1U<<RF_ANA1_SS0_RXADC_CLK_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_EN_POS))
#define RF_ANA1_SS0_RXADC_CLK_EN_HW                             RF_ANA1_SS0_RXADC_CLK_EN_HW
#define RF_ANA1_SS0_RXADC_CLK_EN_HW_POS                         (31U)
#define RF_ANA1_SS0_RXADC_CLK_EN_HW_LEN                         (1U)
#define RF_ANA1_SS0_RXADC_CLK_EN_HW_MSK                         (((1U<<RF_ANA1_SS0_RXADC_CLK_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_EN_HW_POS)
#define RF_ANA1_SS0_RXADC_CLK_EN_HW_UMSK                        (~(((1U<<RF_ANA1_SS0_RXADC_CLK_EN_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_EN_HW_POS))

/* 0x358 : Digital Control */
#define RF_ANA1_SS0_RXADC_1_OFFSET                              (0x358)
#define RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN                        RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN
#define RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_POS                    (0U)
#define RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_LEN                    (1U)
#define RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_MSK                    (((1U<<RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_POS)
#define RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_UMSK                   (~(((1U<<RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_80M_OUT_EN_POS))
#define RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN                     RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN
#define RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_POS                 (2U)
#define RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_LEN                 (1U)
#define RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_MSK                 (((1U<<RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_POS)
#define RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_UMSK                (~(((1U<<RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_EXT_CLK_80M_IN_EN_POS))
#define RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN                       RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN
#define RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_POS                   (4U)
#define RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_LEN                   (1U)
#define RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_MSK                   (((1U<<RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_POS)
#define RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_UMSK                  (~(((1U<<RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_CLK_160M_OUT_EN_POS))
#define RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN                    RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN
#define RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_POS                (6U)
#define RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_LEN                (1U)
#define RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_MSK                (((1U<<RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_POS)
#define RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_UMSK               (~(((1U<<RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_EXT_CLK_160M_IN_EN_POS))
#define RF_ANA1_SS0_RXADC_BUF_STARTUP_SET                       RF_ANA1_SS0_RXADC_BUF_STARTUP_SET
#define RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_POS                   (8U)
#define RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_LEN                   (1U)
#define RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_MSK                   (((1U<<RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_POS)
#define RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_UMSK                  (~(((1U<<RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_STARTUP_SET_POS))
#define RF_ANA1_SS0_RXADC_BUF_OP_EN                             RF_ANA1_SS0_RXADC_BUF_OP_EN
#define RF_ANA1_SS0_RXADC_BUF_OP_EN_POS                         (10U)
#define RF_ANA1_SS0_RXADC_BUF_OP_EN_LEN                         (1U)
#define RF_ANA1_SS0_RXADC_BUF_OP_EN_MSK                         (((1U<<RF_ANA1_SS0_RXADC_BUF_OP_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_OP_EN_POS)
#define RF_ANA1_SS0_RXADC_BUF_OP_EN_UMSK                        (~(((1U<<RF_ANA1_SS0_RXADC_BUF_OP_EN_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_OP_EN_POS))
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT                         RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_POS                     (12U)
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_LEN                     (2U)
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_MSK                     (((1U<<RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_POS)
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_UMSK                    (~(((1U<<RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BM_OP_OUT_POS))
#define RF_ANA1_SS0_RXADC_BUF_BM_OP                             RF_ANA1_SS0_RXADC_BUF_BM_OP
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_POS                         (16U)
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_LEN                         (3U)
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_MSK                         (((1U<<RF_ANA1_SS0_RXADC_BUF_BM_OP_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BM_OP_POS)
#define RF_ANA1_SS0_RXADC_BUF_BM_OP_UMSK                        (~(((1U<<RF_ANA1_SS0_RXADC_BUF_BM_OP_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BM_OP_POS))
#define RF_ANA1_SS0_RXADC_BUF_OP_RZ                             RF_ANA1_SS0_RXADC_BUF_OP_RZ
#define RF_ANA1_SS0_RXADC_BUF_OP_RZ_POS                         (20U)
#define RF_ANA1_SS0_RXADC_BUF_OP_RZ_LEN                         (4U)
#define RF_ANA1_SS0_RXADC_BUF_OP_RZ_MSK                         (((1U<<RF_ANA1_SS0_RXADC_BUF_OP_RZ_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_OP_RZ_POS)
#define RF_ANA1_SS0_RXADC_BUF_OP_RZ_UMSK                        (~(((1U<<RF_ANA1_SS0_RXADC_BUF_OP_RZ_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_OP_RZ_POS))
#define RF_ANA1_SS0_RXADC_BUF_OP_CC                             RF_ANA1_SS0_RXADC_BUF_OP_CC
#define RF_ANA1_SS0_RXADC_BUF_OP_CC_POS                         (24U)
#define RF_ANA1_SS0_RXADC_BUF_OP_CC_LEN                         (5U)
#define RF_ANA1_SS0_RXADC_BUF_OP_CC_MSK                         (((1U<<RF_ANA1_SS0_RXADC_BUF_OP_CC_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_OP_CC_POS)
#define RF_ANA1_SS0_RXADC_BUF_OP_CC_UMSK                        (~(((1U<<RF_ANA1_SS0_RXADC_BUF_OP_CC_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_OP_CC_POS))

/* 0x35C : Digital Control */
#define RF_ANA1_SS0_RXADC_2_OFFSET                              (0x35C)
#define RF_ANA1_SS0_RXADC_BUF_R3                                RF_ANA1_SS0_RXADC_BUF_R3
#define RF_ANA1_SS0_RXADC_BUF_R3_POS                            (0U)
#define RF_ANA1_SS0_RXADC_BUF_R3_LEN                            (3U)
#define RF_ANA1_SS0_RXADC_BUF_R3_MSK                            (((1U<<RF_ANA1_SS0_RXADC_BUF_R3_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_R3_POS)
#define RF_ANA1_SS0_RXADC_BUF_R3_UMSK                           (~(((1U<<RF_ANA1_SS0_RXADC_BUF_R3_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_R3_POS))
#define RF_ANA1_SS0_RXADC_BUF_R2                                RF_ANA1_SS0_RXADC_BUF_R2
#define RF_ANA1_SS0_RXADC_BUF_R2_POS                            (4U)
#define RF_ANA1_SS0_RXADC_BUF_R2_LEN                            (3U)
#define RF_ANA1_SS0_RXADC_BUF_R2_MSK                            (((1U<<RF_ANA1_SS0_RXADC_BUF_R2_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_R2_POS)
#define RF_ANA1_SS0_RXADC_BUF_R2_UMSK                           (~(((1U<<RF_ANA1_SS0_RXADC_BUF_R2_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_R2_POS))
#define RF_ANA1_SS0_RXADC_BUF_R1                                RF_ANA1_SS0_RXADC_BUF_R1
#define RF_ANA1_SS0_RXADC_BUF_R1_POS                            (8U)
#define RF_ANA1_SS0_RXADC_BUF_R1_LEN                            (3U)
#define RF_ANA1_SS0_RXADC_BUF_R1_MSK                            (((1U<<RF_ANA1_SS0_RXADC_BUF_R1_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_R1_POS)
#define RF_ANA1_SS0_RXADC_BUF_R1_UMSK                           (~(((1U<<RF_ANA1_SS0_RXADC_BUF_R1_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_R1_POS))
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL                            RF_ANA1_SS0_RXADC_BUF_BW_SEL
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_POS                        (12U)
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_LEN                        (2U)
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_MSK                        (((1U<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_POS)
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_UMSK                       (~(((1U<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_POS))
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW                         RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_POS                     (14U)
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_LEN                     (2U)
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_MSK                     (((1U<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_POS)
#define RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_UMSK                    (~(((1U<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_BW_SEL_HW_POS))
#define RF_ANA1_SS0_RXADC_BUF_VCM_SEL                           RF_ANA1_SS0_RXADC_BUF_VCM_SEL
#define RF_ANA1_SS0_RXADC_BUF_VCM_SEL_POS                       (16U)
#define RF_ANA1_SS0_RXADC_BUF_VCM_SEL_LEN                       (2U)
#define RF_ANA1_SS0_RXADC_BUF_VCM_SEL_MSK                       (((1U<<RF_ANA1_SS0_RXADC_BUF_VCM_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_VCM_SEL_POS)
#define RF_ANA1_SS0_RXADC_BUF_VCM_SEL_UMSK                      (~(((1U<<RF_ANA1_SS0_RXADC_BUF_VCM_SEL_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_VCM_SEL_POS))
#define RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE                        RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE
#define RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_POS                    (20U)
#define RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_LEN                    (4U)
#define RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_MSK                    (((1U<<RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_POS)
#define RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_UMSK                   (~(((1U<<RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_CGM_R_TUNE_POS))
#define RF_ANA1_SS0_RXADC_BUF_CAP2_FC                           RF_ANA1_SS0_RXADC_BUF_CAP2_FC
#define RF_ANA1_SS0_RXADC_BUF_CAP2_FC_POS                       (24U)
#define RF_ANA1_SS0_RXADC_BUF_CAP2_FC_LEN                       (3U)
#define RF_ANA1_SS0_RXADC_BUF_CAP2_FC_MSK                       (((1U<<RF_ANA1_SS0_RXADC_BUF_CAP2_FC_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_CAP2_FC_POS)
#define RF_ANA1_SS0_RXADC_BUF_CAP2_FC_UMSK                      (~(((1U<<RF_ANA1_SS0_RXADC_BUF_CAP2_FC_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_CAP2_FC_POS))
#define RF_ANA1_SS0_RXADC_BUF_CAP1_FC                           RF_ANA1_SS0_RXADC_BUF_CAP1_FC
#define RF_ANA1_SS0_RXADC_BUF_CAP1_FC_POS                       (28U)
#define RF_ANA1_SS0_RXADC_BUF_CAP1_FC_LEN                       (3U)
#define RF_ANA1_SS0_RXADC_BUF_CAP1_FC_MSK                       (((1U<<RF_ANA1_SS0_RXADC_BUF_CAP1_FC_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_CAP1_FC_POS)
#define RF_ANA1_SS0_RXADC_BUF_CAP1_FC_UMSK                      (~(((1U<<RF_ANA1_SS0_RXADC_BUF_CAP1_FC_LEN)-1)<<RF_ANA1_SS0_RXADC_BUF_CAP1_FC_POS))

/* 0x360 : Digital Control */
#define RF_ANA1_SS0_RXADC_3_OFFSET                              (0x360)
#define RF_ANA1_SS0_RXADC_DOUT_I                                RF_ANA1_SS0_RXADC_DOUT_I
#define RF_ANA1_SS0_RXADC_DOUT_I_POS                            (0U)
#define RF_ANA1_SS0_RXADC_DOUT_I_LEN                            (11U)
#define RF_ANA1_SS0_RXADC_DOUT_I_MSK                            (((1U<<RF_ANA1_SS0_RXADC_DOUT_I_LEN)-1)<<RF_ANA1_SS0_RXADC_DOUT_I_POS)
#define RF_ANA1_SS0_RXADC_DOUT_I_UMSK                           (~(((1U<<RF_ANA1_SS0_RXADC_DOUT_I_LEN)-1)<<RF_ANA1_SS0_RXADC_DOUT_I_POS))
#define RF_ANA1_SS0_RXADC_DOUT_Q                                RF_ANA1_SS0_RXADC_DOUT_Q
#define RF_ANA1_SS0_RXADC_DOUT_Q_POS                            (16U)
#define RF_ANA1_SS0_RXADC_DOUT_Q_LEN                            (11U)
#define RF_ANA1_SS0_RXADC_DOUT_Q_MSK                            (((1U<<RF_ANA1_SS0_RXADC_DOUT_Q_LEN)-1)<<RF_ANA1_SS0_RXADC_DOUT_Q_POS)
#define RF_ANA1_SS0_RXADC_DOUT_Q_UMSK                           (~(((1U<<RF_ANA1_SS0_RXADC_DOUT_Q_LEN)-1)<<RF_ANA1_SS0_RXADC_DOUT_Q_POS))

/* 0x364 : Digital Control */
#define RF_ANA1_SS0_ATEST_CONFIG_OFFSET                         (0x364)
#define RF_ANA1_SS0_ATEST_BM_OP                                 RF_ANA1_SS0_ATEST_BM_OP
#define RF_ANA1_SS0_ATEST_BM_OP_POS                             (0U)
#define RF_ANA1_SS0_ATEST_BM_OP_LEN                             (3U)
#define RF_ANA1_SS0_ATEST_BM_OP_MSK                             (((1U<<RF_ANA1_SS0_ATEST_BM_OP_LEN)-1)<<RF_ANA1_SS0_ATEST_BM_OP_POS)
#define RF_ANA1_SS0_ATEST_BM_OP_UMSK                            (~(((1U<<RF_ANA1_SS0_ATEST_BM_OP_LEN)-1)<<RF_ANA1_SS0_ATEST_BM_OP_POS))
#define RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN                        RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN
#define RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_POS                    (4U)
#define RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_LEN                    (1U)
#define RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_MSK                    (((1U<<RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_LEN)-1)<<RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_POS)
#define RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_UMSK                   (~(((1U<<RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_LEN)-1)<<RF_ANA1_SS0_ATEST_DAC_AC_TEST_EN_POS))
#define RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN                        RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN
#define RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_POS                    (5U)
#define RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_LEN                    (1U)
#define RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_MSK                    (((1U<<RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_LEN)-1)<<RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_POS)
#define RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_UMSK                   (~(((1U<<RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_LEN)-1)<<RF_ANA1_SS0_ATEST_BZ_RBB_TEST_EN_POS))
#define RF_ANA1_SS0_ATEST_GAIN_R9                               RF_ANA1_SS0_ATEST_GAIN_R9
#define RF_ANA1_SS0_ATEST_GAIN_R9_POS                           (8U)
#define RF_ANA1_SS0_ATEST_GAIN_R9_LEN                           (3U)
#define RF_ANA1_SS0_ATEST_GAIN_R9_MSK                           (((1U<<RF_ANA1_SS0_ATEST_GAIN_R9_LEN)-1)<<RF_ANA1_SS0_ATEST_GAIN_R9_POS)
#define RF_ANA1_SS0_ATEST_GAIN_R9_UMSK                          (~(((1U<<RF_ANA1_SS0_ATEST_GAIN_R9_LEN)-1)<<RF_ANA1_SS0_ATEST_GAIN_R9_POS))
#define RF_ANA1_SS0_ATEST_GAIN_R8                               RF_ANA1_SS0_ATEST_GAIN_R8
#define RF_ANA1_SS0_ATEST_GAIN_R8_POS                           (12U)
#define RF_ANA1_SS0_ATEST_GAIN_R8_LEN                           (3U)
#define RF_ANA1_SS0_ATEST_GAIN_R8_MSK                           (((1U<<RF_ANA1_SS0_ATEST_GAIN_R8_LEN)-1)<<RF_ANA1_SS0_ATEST_GAIN_R8_POS)
#define RF_ANA1_SS0_ATEST_GAIN_R8_UMSK                          (~(((1U<<RF_ANA1_SS0_ATEST_GAIN_R8_LEN)-1)<<RF_ANA1_SS0_ATEST_GAIN_R8_POS))
#define RF_ANA1_SS0_ATEST_GAIN_R7                               RF_ANA1_SS0_ATEST_GAIN_R7
#define RF_ANA1_SS0_ATEST_GAIN_R7_POS                           (16U)
#define RF_ANA1_SS0_ATEST_GAIN_R7_LEN                           (3U)
#define RF_ANA1_SS0_ATEST_GAIN_R7_MSK                           (((1U<<RF_ANA1_SS0_ATEST_GAIN_R7_LEN)-1)<<RF_ANA1_SS0_ATEST_GAIN_R7_POS)
#define RF_ANA1_SS0_ATEST_GAIN_R7_UMSK                          (~(((1U<<RF_ANA1_SS0_ATEST_GAIN_R7_LEN)-1)<<RF_ANA1_SS0_ATEST_GAIN_R7_POS))
#define RF_ANA1_SS0_ATEST_OP_CC                                 RF_ANA1_SS0_ATEST_OP_CC
#define RF_ANA1_SS0_ATEST_OP_CC_POS                             (20U)
#define RF_ANA1_SS0_ATEST_OP_CC_LEN                             (4U)
#define RF_ANA1_SS0_ATEST_OP_CC_MSK                             (((1U<<RF_ANA1_SS0_ATEST_OP_CC_LEN)-1)<<RF_ANA1_SS0_ATEST_OP_CC_POS)
#define RF_ANA1_SS0_ATEST_OP_CC_UMSK                            (~(((1U<<RF_ANA1_SS0_ATEST_OP_CC_LEN)-1)<<RF_ANA1_SS0_ATEST_OP_CC_POS))
#define RF_ANA1_SS0_ATEST_STARTUP_SET                           RF_ANA1_SS0_ATEST_STARTUP_SET
#define RF_ANA1_SS0_ATEST_STARTUP_SET_POS                       (24U)
#define RF_ANA1_SS0_ATEST_STARTUP_SET_LEN                       (1U)
#define RF_ANA1_SS0_ATEST_STARTUP_SET_MSK                       (((1U<<RF_ANA1_SS0_ATEST_STARTUP_SET_LEN)-1)<<RF_ANA1_SS0_ATEST_STARTUP_SET_POS)
#define RF_ANA1_SS0_ATEST_STARTUP_SET_UMSK                      (~(((1U<<RF_ANA1_SS0_ATEST_STARTUP_SET_LEN)-1)<<RF_ANA1_SS0_ATEST_STARTUP_SET_POS))
#define RF_ANA1_SS0_ATEST_VCM_SEL                               RF_ANA1_SS0_ATEST_VCM_SEL
#define RF_ANA1_SS0_ATEST_VCM_SEL_POS                           (28U)
#define RF_ANA1_SS0_ATEST_VCM_SEL_LEN                           (2U)
#define RF_ANA1_SS0_ATEST_VCM_SEL_MSK                           (((1U<<RF_ANA1_SS0_ATEST_VCM_SEL_LEN)-1)<<RF_ANA1_SS0_ATEST_VCM_SEL_POS)
#define RF_ANA1_SS0_ATEST_VCM_SEL_UMSK                          (~(((1U<<RF_ANA1_SS0_ATEST_VCM_SEL_LEN)-1)<<RF_ANA1_SS0_ATEST_VCM_SEL_POS))

/* 0x368 : Digital Control */
#define RF_ANA1_SS0_LNA_5G_CTRL_HW_MUX_OFFSET                   (0x368)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG                          RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_POS                      (0U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_LEN                      (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_MSK                      (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_HG_POS))
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG                          RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_POS                      (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_LEN                      (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_MSK                      (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_LG_POS))
#define RF_ANA1_SS0_LNA_5G_BM_HG                                RF_ANA1_SS0_LNA_5G_BM_HG
#define RF_ANA1_SS0_LNA_5G_BM_HG_POS                            (8U)
#define RF_ANA1_SS0_LNA_5G_BM_HG_LEN                            (4U)
#define RF_ANA1_SS0_LNA_5G_BM_HG_MSK                            (((1U<<RF_ANA1_SS0_LNA_5G_BM_HG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_HG_POS)
#define RF_ANA1_SS0_LNA_5G_BM_HG_UMSK                           (~(((1U<<RF_ANA1_SS0_LNA_5G_BM_HG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_HG_POS))
#define RF_ANA1_SS0_LNA_5G_BM_LG                                RF_ANA1_SS0_LNA_5G_BM_LG
#define RF_ANA1_SS0_LNA_5G_BM_LG_POS                            (12U)
#define RF_ANA1_SS0_LNA_5G_BM_LG_LEN                            (4U)
#define RF_ANA1_SS0_LNA_5G_BM_LG_MSK                            (((1U<<RF_ANA1_SS0_LNA_5G_BM_LG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_LG_POS)
#define RF_ANA1_SS0_LNA_5G_BM_LG_UMSK                           (~(((1U<<RF_ANA1_SS0_LNA_5G_BM_LG_LEN)-1)<<RF_ANA1_SS0_LNA_5G_BM_LG_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HG                            RF_ANA1_SS0_LNA_AUX_5G_BM_HG
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HG_POS                        (16U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HG_LEN                        (4U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HG_MSK                        (((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_HG_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_HG_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_HG_UMSK                       (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_HG_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_HG_POS))
#define RF_ANA1_SS0_LNA_AUX_5G_BM_LG                            RF_ANA1_SS0_LNA_AUX_5G_BM_LG
#define RF_ANA1_SS0_LNA_AUX_5G_BM_LG_POS                        (20U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_LG_LEN                        (4U)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_LG_MSK                        (((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_LG_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_LG_POS)
#define RF_ANA1_SS0_LNA_AUX_5G_BM_LG_UMSK                       (~(((1U<<RF_ANA1_SS0_LNA_AUX_5G_BM_LG_LEN)-1)<<RF_ANA1_SS0_LNA_AUX_5G_BM_LG_POS))
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_H                           RF_ANA1_SS0_LNA_5G_LOAD_CSW_H
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_POS                       (24U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_LEN                       (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_MSK                       (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_UMSK                      (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_H_POS))
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_L                           RF_ANA1_SS0_LNA_5G_LOAD_CSW_L
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_POS                       (28U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_LEN                       (4U)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_MSK                       (((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_POS)
#define RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_UMSK                      (~(((1U<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_LEN)-1)<<RF_ANA1_SS0_LNA_5G_LOAD_CSW_L_POS))

/* 0x36C : Digital Control */
#define RF_ANA1_SS0_LNA_2G_CTRL_HW_MUX_OFFSET                   (0x36C)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG                          RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_POS                      (0U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_LEN                      (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_MSK                      (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_HG_POS))
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG                          RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_POS                      (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_LEN                      (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_MSK                      (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_UMSK                     (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_LG_POS))
#define RF_ANA1_SS0_LNA_2G_BM_HG                                RF_ANA1_SS0_LNA_2G_BM_HG
#define RF_ANA1_SS0_LNA_2G_BM_HG_POS                            (8U)
#define RF_ANA1_SS0_LNA_2G_BM_HG_LEN                            (4U)
#define RF_ANA1_SS0_LNA_2G_BM_HG_MSK                            (((1U<<RF_ANA1_SS0_LNA_2G_BM_HG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_HG_POS)
#define RF_ANA1_SS0_LNA_2G_BM_HG_UMSK                           (~(((1U<<RF_ANA1_SS0_LNA_2G_BM_HG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_HG_POS))
#define RF_ANA1_SS0_LNA_2G_BM_LG                                RF_ANA1_SS0_LNA_2G_BM_LG
#define RF_ANA1_SS0_LNA_2G_BM_LG_POS                            (12U)
#define RF_ANA1_SS0_LNA_2G_BM_LG_LEN                            (4U)
#define RF_ANA1_SS0_LNA_2G_BM_LG_MSK                            (((1U<<RF_ANA1_SS0_LNA_2G_BM_LG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_LG_POS)
#define RF_ANA1_SS0_LNA_2G_BM_LG_UMSK                           (~(((1U<<RF_ANA1_SS0_LNA_2G_BM_LG_LEN)-1)<<RF_ANA1_SS0_LNA_2G_BM_LG_POS))
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_H                           RF_ANA1_SS0_LNA_2G_LOAD_CSW_H
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_POS                       (24U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_LEN                       (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_MSK                       (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_UMSK                      (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_H_POS))
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_L                           RF_ANA1_SS0_LNA_2G_LOAD_CSW_L
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_POS                       (28U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_LEN                       (4U)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_MSK                       (((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_POS)
#define RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_UMSK                      (~(((1U<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_LEN)-1)<<RF_ANA1_SS0_LNA_2G_LOAD_CSW_L_POS))

/* 0x370 : Digital Control */
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_CTRL_HW_MUX_OFFSET        (0x370)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ                        RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_POS                    (0U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_LEN                    (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_MSK                    (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_UMSK                   (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_BZ_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI                      RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_POS                  (8U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_LEN                  (4U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP1_FC_WIFI_POS))

/* 0x374 : Digital Control */
#define RF_ANA1_SS0_RBB_RZ_OP1_CTRL_HW_MUX_OFFSET               (0x374)
#define RF_ANA1_SS0_RBB_RZ_OP1_BZ                               RF_ANA1_SS0_RBB_RZ_OP1_BZ
#define RF_ANA1_SS0_RBB_RZ_OP1_BZ_POS                           (0U)
#define RF_ANA1_SS0_RBB_RZ_OP1_BZ_LEN                           (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_BZ_MSK                           (((1U<<RF_ANA1_SS0_RBB_RZ_OP1_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_BZ_POS)
#define RF_ANA1_SS0_RBB_RZ_OP1_BZ_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP1_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_BZ_POS))
#define RF_ANA1_SS0_RBB_RZ_OP1_WIFI                             RF_ANA1_SS0_RBB_RZ_OP1_WIFI
#define RF_ANA1_SS0_RBB_RZ_OP1_WIFI_POS                         (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_WIFI_LEN                         (4U)
#define RF_ANA1_SS0_RBB_RZ_OP1_WIFI_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP1_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_WIFI_POS)
#define RF_ANA1_SS0_RBB_RZ_OP1_WIFI_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP1_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP1_WIFI_POS))

/* 0x378 : Digital Control */
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_CTRL_HW_MUX_OFFSET        (0x378)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ                        RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_POS                    (0U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_LEN                    (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_MSK                    (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_UMSK                   (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_BZ_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI                      RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_POS                  (8U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_LEN                  (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP2_FC_WIFI_POS))

/* 0x37C : Digital Control */
#define RF_ANA1_SS0_RBB_RZ_OP2_CTRL_HW_MUX_OFFSET               (0x37C)
#define RF_ANA1_SS0_RBB_RZ_OP2_BZ                               RF_ANA1_SS0_RBB_RZ_OP2_BZ
#define RF_ANA1_SS0_RBB_RZ_OP2_BZ_POS                           (0U)
#define RF_ANA1_SS0_RBB_RZ_OP2_BZ_LEN                           (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_BZ_MSK                           (((1U<<RF_ANA1_SS0_RBB_RZ_OP2_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_BZ_POS)
#define RF_ANA1_SS0_RBB_RZ_OP2_BZ_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP2_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_BZ_POS))
#define RF_ANA1_SS0_RBB_RZ_OP2_WIFI                             RF_ANA1_SS0_RBB_RZ_OP2_WIFI
#define RF_ANA1_SS0_RBB_RZ_OP2_WIFI_POS                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_WIFI_LEN                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP2_WIFI_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP2_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_WIFI_POS)
#define RF_ANA1_SS0_RBB_RZ_OP2_WIFI_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP2_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP2_WIFI_POS))

/* 0x380 : Digital Control */
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_CTRL_HW_MUX_OFFSET        (0x380)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ                        RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_POS                    (0U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_LEN                    (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_MSK                    (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_UMSK                   (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_BZ_POS))
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI                      RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_POS                  (8U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_LEN                  (5U)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_MSK                  (((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_POS)
#define RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_UMSK                 (~(((1U<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_CAP_CC_OP3_FC_WIFI_POS))

/* 0x384 : Digital Control */
#define RF_ANA1_SS0_RBB_RZ_OP3_CTRL_HW_MUX_OFFSET               (0x384)
#define RF_ANA1_SS0_RBB_RZ_OP3_BZ                               RF_ANA1_SS0_RBB_RZ_OP3_BZ
#define RF_ANA1_SS0_RBB_RZ_OP3_BZ_POS                           (0U)
#define RF_ANA1_SS0_RBB_RZ_OP3_BZ_LEN                           (2U)
#define RF_ANA1_SS0_RBB_RZ_OP3_BZ_MSK                           (((1U<<RF_ANA1_SS0_RBB_RZ_OP3_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_BZ_POS)
#define RF_ANA1_SS0_RBB_RZ_OP3_BZ_UMSK                          (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP3_BZ_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_BZ_POS))
#define RF_ANA1_SS0_RBB_RZ_OP3_WIFI                             RF_ANA1_SS0_RBB_RZ_OP3_WIFI
#define RF_ANA1_SS0_RBB_RZ_OP3_WIFI_POS                         (4U)
#define RF_ANA1_SS0_RBB_RZ_OP3_WIFI_LEN                         (2U)
#define RF_ANA1_SS0_RBB_RZ_OP3_WIFI_MSK                         (((1U<<RF_ANA1_SS0_RBB_RZ_OP3_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_WIFI_POS)
#define RF_ANA1_SS0_RBB_RZ_OP3_WIFI_UMSK                        (~(((1U<<RF_ANA1_SS0_RBB_RZ_OP3_WIFI_LEN)-1)<<RF_ANA1_SS0_RBB_RZ_OP3_WIFI_POS))

/* 0x388 : ss0_resv */
#define RF_ANA1_SS0_RESV_OFFSET                                 (0x388)
#define RF_ANA1_SS0_RESV0                                       RF_ANA1_SS0_RESV0
#define RF_ANA1_SS0_RESV0_POS                                   (0U)
#define RF_ANA1_SS0_RESV0_LEN                                   (16U)
#define RF_ANA1_SS0_RESV0_MSK                                   (((1U<<RF_ANA1_SS0_RESV0_LEN)-1)<<RF_ANA1_SS0_RESV0_POS)
#define RF_ANA1_SS0_RESV0_UMSK                                  (~(((1U<<RF_ANA1_SS0_RESV0_LEN)-1)<<RF_ANA1_SS0_RESV0_POS))
#define RF_ANA1_SS0_RESV1                                       RF_ANA1_SS0_RESV1
#define RF_ANA1_SS0_RESV1_POS                                   (16U)
#define RF_ANA1_SS0_RESV1_LEN                                   (16U)
#define RF_ANA1_SS0_RESV1_MSK                                   (((1U<<RF_ANA1_SS0_RESV1_LEN)-1)<<RF_ANA1_SS0_RESV1_POS)
#define RF_ANA1_SS0_RESV1_UMSK                                  (~(((1U<<RF_ANA1_SS0_RESV1_LEN)-1)<<RF_ANA1_SS0_RESV1_POS))

/* 0x38C : ss0_resv1 */
#define RF_ANA1_SS0_RESV1_OFFSET                                (0x38C)
#define RF_ANA1_SS0_RESV_RB                                     RF_ANA1_SS0_RESV_RB
#define RF_ANA1_SS0_RESV_RB_POS                                 (0U)
#define RF_ANA1_SS0_RESV_RB_LEN                                 (32U)
#define RF_ANA1_SS0_RESV_RB_MSK                                 (((1U<<RF_ANA1_SS0_RESV_RB_LEN)-1)<<RF_ANA1_SS0_RESV_RB_POS)
#define RF_ANA1_SS0_RESV_RB_UMSK                                (~(((1U<<RF_ANA1_SS0_RESV_RB_LEN)-1)<<RF_ANA1_SS0_RESV_RB_POS))

/* 0x700 : ss0_2g_rx_gain_tbl0 */
#define RF_ANA1_SS0_2G_RX_GAIN_TBL0_OFFSET                      (0x700)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_POS                    (0U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_0_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_0                               RF_ANA1_SS0_2G_GC_RMXGM_0
#define RF_ANA1_SS0_2G_GC_RMXGM_0_POS                           (3U)
#define RF_ANA1_SS0_2G_GC_RMXGM_0_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_0_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_0_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_0_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_0_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_0_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_0_POS))
#define RF_ANA1_SS0_2G_GC_LNA_0                                 RF_ANA1_SS0_2G_GC_LNA_0
#define RF_ANA1_SS0_2G_GC_LNA_0_POS                             (5U)
#define RF_ANA1_SS0_2G_GC_LNA_0_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_0_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_0_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_0_POS)
#define RF_ANA1_SS0_2G_GC_LNA_0_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_0_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_0_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_POS                    (8U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_1_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_1                               RF_ANA1_SS0_2G_GC_RMXGM_1
#define RF_ANA1_SS0_2G_GC_RMXGM_1_POS                           (11U)
#define RF_ANA1_SS0_2G_GC_RMXGM_1_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_1_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_1_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_1_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_1_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_1_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_1_POS))
#define RF_ANA1_SS0_2G_GC_LNA_1                                 RF_ANA1_SS0_2G_GC_LNA_1
#define RF_ANA1_SS0_2G_GC_LNA_1_POS                             (13U)
#define RF_ANA1_SS0_2G_GC_LNA_1_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_1_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_1_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_1_POS)
#define RF_ANA1_SS0_2G_GC_LNA_1_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_1_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_1_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_POS                    (16U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_2_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_2                               RF_ANA1_SS0_2G_GC_RMXGM_2
#define RF_ANA1_SS0_2G_GC_RMXGM_2_POS                           (19U)
#define RF_ANA1_SS0_2G_GC_RMXGM_2_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_2_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_2_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_2_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_2_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_2_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_2_POS))
#define RF_ANA1_SS0_2G_GC_LNA_2                                 RF_ANA1_SS0_2G_GC_LNA_2
#define RF_ANA1_SS0_2G_GC_LNA_2_POS                             (21U)
#define RF_ANA1_SS0_2G_GC_LNA_2_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_2_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_2_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_2_POS)
#define RF_ANA1_SS0_2G_GC_LNA_2_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_2_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_2_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_POS                    (24U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_3_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_3                               RF_ANA1_SS0_2G_GC_RMXGM_3
#define RF_ANA1_SS0_2G_GC_RMXGM_3_POS                           (27U)
#define RF_ANA1_SS0_2G_GC_RMXGM_3_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_3_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_3_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_3_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_3_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_3_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_3_POS))
#define RF_ANA1_SS0_2G_GC_LNA_3                                 RF_ANA1_SS0_2G_GC_LNA_3
#define RF_ANA1_SS0_2G_GC_LNA_3_POS                             (29U)
#define RF_ANA1_SS0_2G_GC_LNA_3_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_3_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_3_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_3_POS)
#define RF_ANA1_SS0_2G_GC_LNA_3_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_3_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_3_POS))

/* 0x704 : ss0_2g_rx_gain_tbl1 */
#define RF_ANA1_SS0_2G_RX_GAIN_TBL1_OFFSET                      (0x704)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_POS                    (0U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_4_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_4                               RF_ANA1_SS0_2G_GC_RMXGM_4
#define RF_ANA1_SS0_2G_GC_RMXGM_4_POS                           (3U)
#define RF_ANA1_SS0_2G_GC_RMXGM_4_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_4_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_4_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_4_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_4_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_4_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_4_POS))
#define RF_ANA1_SS0_2G_GC_LNA_4                                 RF_ANA1_SS0_2G_GC_LNA_4
#define RF_ANA1_SS0_2G_GC_LNA_4_POS                             (5U)
#define RF_ANA1_SS0_2G_GC_LNA_4_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_4_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_4_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_4_POS)
#define RF_ANA1_SS0_2G_GC_LNA_4_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_4_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_4_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_POS                    (8U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_5_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_5                               RF_ANA1_SS0_2G_GC_RMXGM_5
#define RF_ANA1_SS0_2G_GC_RMXGM_5_POS                           (11U)
#define RF_ANA1_SS0_2G_GC_RMXGM_5_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_5_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_5_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_5_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_5_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_5_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_5_POS))
#define RF_ANA1_SS0_2G_GC_LNA_5                                 RF_ANA1_SS0_2G_GC_LNA_5
#define RF_ANA1_SS0_2G_GC_LNA_5_POS                             (13U)
#define RF_ANA1_SS0_2G_GC_LNA_5_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_5_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_5_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_5_POS)
#define RF_ANA1_SS0_2G_GC_LNA_5_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_5_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_5_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_POS                    (16U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_6_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_6                               RF_ANA1_SS0_2G_GC_RMXGM_6
#define RF_ANA1_SS0_2G_GC_RMXGM_6_POS                           (19U)
#define RF_ANA1_SS0_2G_GC_RMXGM_6_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_6_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_6_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_6_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_6_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_6_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_6_POS))
#define RF_ANA1_SS0_2G_GC_LNA_6                                 RF_ANA1_SS0_2G_GC_LNA_6
#define RF_ANA1_SS0_2G_GC_LNA_6_POS                             (21U)
#define RF_ANA1_SS0_2G_GC_LNA_6_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_6_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_6_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_6_POS)
#define RF_ANA1_SS0_2G_GC_LNA_6_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_6_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_6_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_POS                    (24U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_7_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_7                               RF_ANA1_SS0_2G_GC_RMXGM_7
#define RF_ANA1_SS0_2G_GC_RMXGM_7_POS                           (27U)
#define RF_ANA1_SS0_2G_GC_RMXGM_7_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_7_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_7_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_7_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_7_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_7_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_7_POS))
#define RF_ANA1_SS0_2G_GC_LNA_7                                 RF_ANA1_SS0_2G_GC_LNA_7
#define RF_ANA1_SS0_2G_GC_LNA_7_POS                             (29U)
#define RF_ANA1_SS0_2G_GC_LNA_7_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_7_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_7_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_7_POS)
#define RF_ANA1_SS0_2G_GC_LNA_7_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_7_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_7_POS))

/* 0x708 : ss0_2g_rx_gain_tbl2 */
#define RF_ANA1_SS0_2G_RX_GAIN_TBL2_OFFSET                      (0x708)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_POS                    (0U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_8_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_8                               RF_ANA1_SS0_2G_GC_RMXGM_8
#define RF_ANA1_SS0_2G_GC_RMXGM_8_POS                           (3U)
#define RF_ANA1_SS0_2G_GC_RMXGM_8_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_8_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_8_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_8_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_8_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_8_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_8_POS))
#define RF_ANA1_SS0_2G_GC_LNA_8                                 RF_ANA1_SS0_2G_GC_LNA_8
#define RF_ANA1_SS0_2G_GC_LNA_8_POS                             (5U)
#define RF_ANA1_SS0_2G_GC_LNA_8_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_8_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_8_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_8_POS)
#define RF_ANA1_SS0_2G_GC_LNA_8_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_8_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_8_POS))
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9                        RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_POS                    (8U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_LEN                    (3U)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_MSK                    (((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_POS)
#define RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_UMSK                   (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_LOAD_ATT_9_POS))
#define RF_ANA1_SS0_2G_GC_RMXGM_9                               RF_ANA1_SS0_2G_GC_RMXGM_9
#define RF_ANA1_SS0_2G_GC_RMXGM_9_POS                           (11U)
#define RF_ANA1_SS0_2G_GC_RMXGM_9_LEN                           (2U)
#define RF_ANA1_SS0_2G_GC_RMXGM_9_MSK                           (((1U<<RF_ANA1_SS0_2G_GC_RMXGM_9_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_9_POS)
#define RF_ANA1_SS0_2G_GC_RMXGM_9_UMSK                          (~(((1U<<RF_ANA1_SS0_2G_GC_RMXGM_9_LEN)-1)<<RF_ANA1_SS0_2G_GC_RMXGM_9_POS))
#define RF_ANA1_SS0_2G_GC_LNA_9                                 RF_ANA1_SS0_2G_GC_LNA_9
#define RF_ANA1_SS0_2G_GC_LNA_9_POS                             (13U)
#define RF_ANA1_SS0_2G_GC_LNA_9_LEN                             (3U)
#define RF_ANA1_SS0_2G_GC_LNA_9_MSK                             (((1U<<RF_ANA1_SS0_2G_GC_LNA_9_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_9_POS)
#define RF_ANA1_SS0_2G_GC_LNA_9_UMSK                            (~(((1U<<RF_ANA1_SS0_2G_GC_LNA_9_LEN)-1)<<RF_ANA1_SS0_2G_GC_LNA_9_POS))

/* 0x70C : ss0_5g_rx_gain_tbl0 */
#define RF_ANA1_SS0_5G_RX_GAIN_TBL0_OFFSET                      (0x70C)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_POS                    (0U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_0_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_0                               RF_ANA1_SS0_5G_GC_RMXGM_0
#define RF_ANA1_SS0_5G_GC_RMXGM_0_POS                           (3U)
#define RF_ANA1_SS0_5G_GC_RMXGM_0_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_0_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_0_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_0_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_0_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_0_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_0_POS))
#define RF_ANA1_SS0_5G_GC_LNA_0                                 RF_ANA1_SS0_5G_GC_LNA_0
#define RF_ANA1_SS0_5G_GC_LNA_0_POS                             (5U)
#define RF_ANA1_SS0_5G_GC_LNA_0_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_0_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_0_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_0_POS)
#define RF_ANA1_SS0_5G_GC_LNA_0_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_0_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_0_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_POS                    (8U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_1_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_1                               RF_ANA1_SS0_5G_GC_RMXGM_1
#define RF_ANA1_SS0_5G_GC_RMXGM_1_POS                           (11U)
#define RF_ANA1_SS0_5G_GC_RMXGM_1_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_1_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_1_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_1_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_1_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_1_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_1_POS))
#define RF_ANA1_SS0_5G_GC_LNA_1                                 RF_ANA1_SS0_5G_GC_LNA_1
#define RF_ANA1_SS0_5G_GC_LNA_1_POS                             (13U)
#define RF_ANA1_SS0_5G_GC_LNA_1_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_1_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_1_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_1_POS)
#define RF_ANA1_SS0_5G_GC_LNA_1_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_1_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_1_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_POS                    (16U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_2_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_2                               RF_ANA1_SS0_5G_GC_RMXGM_2
#define RF_ANA1_SS0_5G_GC_RMXGM_2_POS                           (19U)
#define RF_ANA1_SS0_5G_GC_RMXGM_2_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_2_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_2_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_2_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_2_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_2_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_2_POS))
#define RF_ANA1_SS0_5G_GC_LNA_2                                 RF_ANA1_SS0_5G_GC_LNA_2
#define RF_ANA1_SS0_5G_GC_LNA_2_POS                             (21U)
#define RF_ANA1_SS0_5G_GC_LNA_2_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_2_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_2_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_2_POS)
#define RF_ANA1_SS0_5G_GC_LNA_2_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_2_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_2_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_POS                    (24U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_3_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_3                               RF_ANA1_SS0_5G_GC_RMXGM_3
#define RF_ANA1_SS0_5G_GC_RMXGM_3_POS                           (27U)
#define RF_ANA1_SS0_5G_GC_RMXGM_3_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_3_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_3_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_3_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_3_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_3_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_3_POS))
#define RF_ANA1_SS0_5G_GC_LNA_3                                 RF_ANA1_SS0_5G_GC_LNA_3
#define RF_ANA1_SS0_5G_GC_LNA_3_POS                             (29U)
#define RF_ANA1_SS0_5G_GC_LNA_3_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_3_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_3_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_3_POS)
#define RF_ANA1_SS0_5G_GC_LNA_3_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_3_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_3_POS))

/* 0x710 : ss0_5g_rx_gain_tbl1 */
#define RF_ANA1_SS0_5G_RX_GAIN_TBL1_OFFSET                      (0x710)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_POS                    (0U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_4_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_4                               RF_ANA1_SS0_5G_GC_RMXGM_4
#define RF_ANA1_SS0_5G_GC_RMXGM_4_POS                           (3U)
#define RF_ANA1_SS0_5G_GC_RMXGM_4_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_4_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_4_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_4_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_4_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_4_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_4_POS))
#define RF_ANA1_SS0_5G_GC_LNA_4                                 RF_ANA1_SS0_5G_GC_LNA_4
#define RF_ANA1_SS0_5G_GC_LNA_4_POS                             (5U)
#define RF_ANA1_SS0_5G_GC_LNA_4_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_4_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_4_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_4_POS)
#define RF_ANA1_SS0_5G_GC_LNA_4_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_4_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_4_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_POS                    (8U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_5_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_5                               RF_ANA1_SS0_5G_GC_RMXGM_5
#define RF_ANA1_SS0_5G_GC_RMXGM_5_POS                           (11U)
#define RF_ANA1_SS0_5G_GC_RMXGM_5_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_5_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_5_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_5_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_5_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_5_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_5_POS))
#define RF_ANA1_SS0_5G_GC_LNA_5                                 RF_ANA1_SS0_5G_GC_LNA_5
#define RF_ANA1_SS0_5G_GC_LNA_5_POS                             (13U)
#define RF_ANA1_SS0_5G_GC_LNA_5_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_5_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_5_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_5_POS)
#define RF_ANA1_SS0_5G_GC_LNA_5_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_5_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_5_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_POS                    (16U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_6_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_6                               RF_ANA1_SS0_5G_GC_RMXGM_6
#define RF_ANA1_SS0_5G_GC_RMXGM_6_POS                           (19U)
#define RF_ANA1_SS0_5G_GC_RMXGM_6_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_6_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_6_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_6_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_6_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_6_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_6_POS))
#define RF_ANA1_SS0_5G_GC_LNA_6                                 RF_ANA1_SS0_5G_GC_LNA_6
#define RF_ANA1_SS0_5G_GC_LNA_6_POS                             (21U)
#define RF_ANA1_SS0_5G_GC_LNA_6_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_6_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_6_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_6_POS)
#define RF_ANA1_SS0_5G_GC_LNA_6_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_6_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_6_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_POS                    (24U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_7_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_7                               RF_ANA1_SS0_5G_GC_RMXGM_7
#define RF_ANA1_SS0_5G_GC_RMXGM_7_POS                           (27U)
#define RF_ANA1_SS0_5G_GC_RMXGM_7_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_7_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_7_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_7_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_7_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_7_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_7_POS))
#define RF_ANA1_SS0_5G_GC_LNA_7                                 RF_ANA1_SS0_5G_GC_LNA_7
#define RF_ANA1_SS0_5G_GC_LNA_7_POS                             (29U)
#define RF_ANA1_SS0_5G_GC_LNA_7_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_7_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_7_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_7_POS)
#define RF_ANA1_SS0_5G_GC_LNA_7_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_7_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_7_POS))

/* 0x714 : ss0_5g_rx_gain_tbl2 */
#define RF_ANA1_SS0_5G_RX_GAIN_TBL2_OFFSET                      (0x714)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_POS                    (0U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_8_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_8                               RF_ANA1_SS0_5G_GC_RMXGM_8
#define RF_ANA1_SS0_5G_GC_RMXGM_8_POS                           (3U)
#define RF_ANA1_SS0_5G_GC_RMXGM_8_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_8_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_8_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_8_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_8_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_8_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_8_POS))
#define RF_ANA1_SS0_5G_GC_LNA_8                                 RF_ANA1_SS0_5G_GC_LNA_8
#define RF_ANA1_SS0_5G_GC_LNA_8_POS                             (5U)
#define RF_ANA1_SS0_5G_GC_LNA_8_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_8_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_8_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_8_POS)
#define RF_ANA1_SS0_5G_GC_LNA_8_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_8_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_8_POS))
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9                        RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_POS                    (8U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_LEN                    (3U)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_MSK                    (((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_POS)
#define RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_UMSK                   (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_LOAD_ATT_9_POS))
#define RF_ANA1_SS0_5G_GC_RMXGM_9                               RF_ANA1_SS0_5G_GC_RMXGM_9
#define RF_ANA1_SS0_5G_GC_RMXGM_9_POS                           (11U)
#define RF_ANA1_SS0_5G_GC_RMXGM_9_LEN                           (2U)
#define RF_ANA1_SS0_5G_GC_RMXGM_9_MSK                           (((1U<<RF_ANA1_SS0_5G_GC_RMXGM_9_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_9_POS)
#define RF_ANA1_SS0_5G_GC_RMXGM_9_UMSK                          (~(((1U<<RF_ANA1_SS0_5G_GC_RMXGM_9_LEN)-1)<<RF_ANA1_SS0_5G_GC_RMXGM_9_POS))
#define RF_ANA1_SS0_5G_GC_LNA_9                                 RF_ANA1_SS0_5G_GC_LNA_9
#define RF_ANA1_SS0_5G_GC_LNA_9_POS                             (13U)
#define RF_ANA1_SS0_5G_GC_LNA_9_LEN                             (3U)
#define RF_ANA1_SS0_5G_GC_LNA_9_MSK                             (((1U<<RF_ANA1_SS0_5G_GC_LNA_9_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_9_POS)
#define RF_ANA1_SS0_5G_GC_LNA_9_UMSK                            (~(((1U<<RF_ANA1_SS0_5G_GC_LNA_9_LEN)-1)<<RF_ANA1_SS0_5G_GC_LNA_9_POS))

/* 0x730 : ss0_rbb_tbl0 */
#define RF_ANA1_SS0_RBB_TBL0_OFFSET                             (0x730)
#define RF_ANA1_SS0_GC_RBB1_0                                   RF_ANA1_SS0_GC_RBB1_0
#define RF_ANA1_SS0_GC_RBB1_0_POS                               (0U)
#define RF_ANA1_SS0_GC_RBB1_0_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_0_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_0_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_0_POS)
#define RF_ANA1_SS0_GC_RBB1_0_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_0_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_0_POS))
#define RF_ANA1_SS0_GC_RBB2_0                                   RF_ANA1_SS0_GC_RBB2_0
#define RF_ANA1_SS0_GC_RBB2_0_POS                               (4U)
#define RF_ANA1_SS0_GC_RBB2_0_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_0_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_0_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_0_POS)
#define RF_ANA1_SS0_GC_RBB2_0_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_0_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_0_POS))
#define RF_ANA1_SS0_GC_RBB1_1                                   RF_ANA1_SS0_GC_RBB1_1
#define RF_ANA1_SS0_GC_RBB1_1_POS                               (8U)
#define RF_ANA1_SS0_GC_RBB1_1_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_1_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_1_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_1_POS)
#define RF_ANA1_SS0_GC_RBB1_1_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_1_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_1_POS))
#define RF_ANA1_SS0_GC_RBB2_1                                   RF_ANA1_SS0_GC_RBB2_1
#define RF_ANA1_SS0_GC_RBB2_1_POS                               (12U)
#define RF_ANA1_SS0_GC_RBB2_1_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_1_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_1_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_1_POS)
#define RF_ANA1_SS0_GC_RBB2_1_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_1_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_1_POS))
#define RF_ANA1_SS0_GC_RBB1_2                                   RF_ANA1_SS0_GC_RBB1_2
#define RF_ANA1_SS0_GC_RBB1_2_POS                               (16U)
#define RF_ANA1_SS0_GC_RBB1_2_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_2_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_2_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_2_POS)
#define RF_ANA1_SS0_GC_RBB1_2_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_2_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_2_POS))
#define RF_ANA1_SS0_GC_RBB2_2                                   RF_ANA1_SS0_GC_RBB2_2
#define RF_ANA1_SS0_GC_RBB2_2_POS                               (20U)
#define RF_ANA1_SS0_GC_RBB2_2_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_2_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_2_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_2_POS)
#define RF_ANA1_SS0_GC_RBB2_2_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_2_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_2_POS))
#define RF_ANA1_SS0_GC_RBB1_3                                   RF_ANA1_SS0_GC_RBB1_3
#define RF_ANA1_SS0_GC_RBB1_3_POS                               (24U)
#define RF_ANA1_SS0_GC_RBB1_3_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_3_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_3_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_3_POS)
#define RF_ANA1_SS0_GC_RBB1_3_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_3_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_3_POS))
#define RF_ANA1_SS0_GC_RBB2_3                                   RF_ANA1_SS0_GC_RBB2_3
#define RF_ANA1_SS0_GC_RBB2_3_POS                               (28U)
#define RF_ANA1_SS0_GC_RBB2_3_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_3_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_3_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_3_POS)
#define RF_ANA1_SS0_GC_RBB2_3_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_3_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_3_POS))

/* 0x734 : ss0_rbb_tbl1 */
#define RF_ANA1_SS0_RBB_TBL1_OFFSET                             (0x734)
#define RF_ANA1_SS0_GC_RBB1_4                                   RF_ANA1_SS0_GC_RBB1_4
#define RF_ANA1_SS0_GC_RBB1_4_POS                               (0U)
#define RF_ANA1_SS0_GC_RBB1_4_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_4_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_4_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_4_POS)
#define RF_ANA1_SS0_GC_RBB1_4_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_4_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_4_POS))
#define RF_ANA1_SS0_GC_RBB2_4                                   RF_ANA1_SS0_GC_RBB2_4
#define RF_ANA1_SS0_GC_RBB2_4_POS                               (4U)
#define RF_ANA1_SS0_GC_RBB2_4_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_4_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_4_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_4_POS)
#define RF_ANA1_SS0_GC_RBB2_4_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_4_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_4_POS))
#define RF_ANA1_SS0_GC_RBB1_5                                   RF_ANA1_SS0_GC_RBB1_5
#define RF_ANA1_SS0_GC_RBB1_5_POS                               (8U)
#define RF_ANA1_SS0_GC_RBB1_5_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_5_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_5_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_5_POS)
#define RF_ANA1_SS0_GC_RBB1_5_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_5_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_5_POS))
#define RF_ANA1_SS0_GC_RBB2_5                                   RF_ANA1_SS0_GC_RBB2_5
#define RF_ANA1_SS0_GC_RBB2_5_POS                               (12U)
#define RF_ANA1_SS0_GC_RBB2_5_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_5_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_5_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_5_POS)
#define RF_ANA1_SS0_GC_RBB2_5_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_5_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_5_POS))
#define RF_ANA1_SS0_GC_RBB1_6                                   RF_ANA1_SS0_GC_RBB1_6
#define RF_ANA1_SS0_GC_RBB1_6_POS                               (16U)
#define RF_ANA1_SS0_GC_RBB1_6_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_6_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_6_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_6_POS)
#define RF_ANA1_SS0_GC_RBB1_6_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_6_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_6_POS))
#define RF_ANA1_SS0_GC_RBB2_6                                   RF_ANA1_SS0_GC_RBB2_6
#define RF_ANA1_SS0_GC_RBB2_6_POS                               (20U)
#define RF_ANA1_SS0_GC_RBB2_6_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_6_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_6_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_6_POS)
#define RF_ANA1_SS0_GC_RBB2_6_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_6_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_6_POS))
#define RF_ANA1_SS0_GC_RBB1_7                                   RF_ANA1_SS0_GC_RBB1_7
#define RF_ANA1_SS0_GC_RBB1_7_POS                               (24U)
#define RF_ANA1_SS0_GC_RBB1_7_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_7_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_7_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_7_POS)
#define RF_ANA1_SS0_GC_RBB1_7_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_7_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_7_POS))
#define RF_ANA1_SS0_GC_RBB2_7                                   RF_ANA1_SS0_GC_RBB2_7
#define RF_ANA1_SS0_GC_RBB2_7_POS                               (28U)
#define RF_ANA1_SS0_GC_RBB2_7_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_7_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_7_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_7_POS)
#define RF_ANA1_SS0_GC_RBB2_7_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_7_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_7_POS))

/* 0x738 : ss0_rbb_tbl2 */
#define RF_ANA1_SS0_RBB_TBL2_OFFSET                             (0x738)
#define RF_ANA1_SS0_GC_RBB1_8                                   RF_ANA1_SS0_GC_RBB1_8
#define RF_ANA1_SS0_GC_RBB1_8_POS                               (0U)
#define RF_ANA1_SS0_GC_RBB1_8_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_8_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_8_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_8_POS)
#define RF_ANA1_SS0_GC_RBB1_8_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_8_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_8_POS))
#define RF_ANA1_SS0_GC_RBB2_8                                   RF_ANA1_SS0_GC_RBB2_8
#define RF_ANA1_SS0_GC_RBB2_8_POS                               (4U)
#define RF_ANA1_SS0_GC_RBB2_8_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_8_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_8_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_8_POS)
#define RF_ANA1_SS0_GC_RBB2_8_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_8_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_8_POS))
#define RF_ANA1_SS0_GC_RBB1_9                                   RF_ANA1_SS0_GC_RBB1_9
#define RF_ANA1_SS0_GC_RBB1_9_POS                               (8U)
#define RF_ANA1_SS0_GC_RBB1_9_LEN                               (2U)
#define RF_ANA1_SS0_GC_RBB1_9_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB1_9_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_9_POS)
#define RF_ANA1_SS0_GC_RBB1_9_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB1_9_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_9_POS))
#define RF_ANA1_SS0_GC_RBB2_9                                   RF_ANA1_SS0_GC_RBB2_9
#define RF_ANA1_SS0_GC_RBB2_9_POS                               (12U)
#define RF_ANA1_SS0_GC_RBB2_9_LEN                               (3U)
#define RF_ANA1_SS0_GC_RBB2_9_MSK                               (((1U<<RF_ANA1_SS0_GC_RBB2_9_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_9_POS)
#define RF_ANA1_SS0_GC_RBB2_9_UMSK                              (~(((1U<<RF_ANA1_SS0_GC_RBB2_9_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_9_POS))
#define RF_ANA1_SS0_GC_RBB1_10                                  RF_ANA1_SS0_GC_RBB1_10
#define RF_ANA1_SS0_GC_RBB1_10_POS                              (16U)
#define RF_ANA1_SS0_GC_RBB1_10_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_10_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_10_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_10_POS)
#define RF_ANA1_SS0_GC_RBB1_10_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_10_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_10_POS))
#define RF_ANA1_SS0_GC_RBB2_10                                  RF_ANA1_SS0_GC_RBB2_10
#define RF_ANA1_SS0_GC_RBB2_10_POS                              (20U)
#define RF_ANA1_SS0_GC_RBB2_10_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_10_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_10_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_10_POS)
#define RF_ANA1_SS0_GC_RBB2_10_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_10_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_10_POS))
#define RF_ANA1_SS0_GC_RBB1_11                                  RF_ANA1_SS0_GC_RBB1_11
#define RF_ANA1_SS0_GC_RBB1_11_POS                              (24U)
#define RF_ANA1_SS0_GC_RBB1_11_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_11_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_11_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_11_POS)
#define RF_ANA1_SS0_GC_RBB1_11_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_11_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_11_POS))
#define RF_ANA1_SS0_GC_RBB2_11                                  RF_ANA1_SS0_GC_RBB2_11
#define RF_ANA1_SS0_GC_RBB2_11_POS                              (28U)
#define RF_ANA1_SS0_GC_RBB2_11_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_11_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_11_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_11_POS)
#define RF_ANA1_SS0_GC_RBB2_11_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_11_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_11_POS))

/* 0x73C : ss0_rbb_tbl3 */
#define RF_ANA1_SS0_RBB_TBL3_OFFSET                             (0x73C)
#define RF_ANA1_SS0_GC_RBB1_12                                  RF_ANA1_SS0_GC_RBB1_12
#define RF_ANA1_SS0_GC_RBB1_12_POS                              (0U)
#define RF_ANA1_SS0_GC_RBB1_12_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_12_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_12_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_12_POS)
#define RF_ANA1_SS0_GC_RBB1_12_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_12_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_12_POS))
#define RF_ANA1_SS0_GC_RBB2_12                                  RF_ANA1_SS0_GC_RBB2_12
#define RF_ANA1_SS0_GC_RBB2_12_POS                              (4U)
#define RF_ANA1_SS0_GC_RBB2_12_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_12_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_12_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_12_POS)
#define RF_ANA1_SS0_GC_RBB2_12_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_12_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_12_POS))
#define RF_ANA1_SS0_GC_RBB1_13                                  RF_ANA1_SS0_GC_RBB1_13
#define RF_ANA1_SS0_GC_RBB1_13_POS                              (8U)
#define RF_ANA1_SS0_GC_RBB1_13_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_13_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_13_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_13_POS)
#define RF_ANA1_SS0_GC_RBB1_13_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_13_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_13_POS))
#define RF_ANA1_SS0_GC_RBB2_13                                  RF_ANA1_SS0_GC_RBB2_13
#define RF_ANA1_SS0_GC_RBB2_13_POS                              (12U)
#define RF_ANA1_SS0_GC_RBB2_13_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_13_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_13_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_13_POS)
#define RF_ANA1_SS0_GC_RBB2_13_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_13_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_13_POS))
#define RF_ANA1_SS0_GC_RBB1_14                                  RF_ANA1_SS0_GC_RBB1_14
#define RF_ANA1_SS0_GC_RBB1_14_POS                              (16U)
#define RF_ANA1_SS0_GC_RBB1_14_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_14_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_14_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_14_POS)
#define RF_ANA1_SS0_GC_RBB1_14_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_14_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_14_POS))
#define RF_ANA1_SS0_GC_RBB2_14                                  RF_ANA1_SS0_GC_RBB2_14
#define RF_ANA1_SS0_GC_RBB2_14_POS                              (20U)
#define RF_ANA1_SS0_GC_RBB2_14_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_14_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_14_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_14_POS)
#define RF_ANA1_SS0_GC_RBB2_14_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_14_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_14_POS))
#define RF_ANA1_SS0_GC_RBB1_15                                  RF_ANA1_SS0_GC_RBB1_15
#define RF_ANA1_SS0_GC_RBB1_15_POS                              (24U)
#define RF_ANA1_SS0_GC_RBB1_15_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_15_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_15_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_15_POS)
#define RF_ANA1_SS0_GC_RBB1_15_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_15_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_15_POS))
#define RF_ANA1_SS0_GC_RBB2_15                                  RF_ANA1_SS0_GC_RBB2_15
#define RF_ANA1_SS0_GC_RBB2_15_POS                              (28U)
#define RF_ANA1_SS0_GC_RBB2_15_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_15_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_15_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_15_POS)
#define RF_ANA1_SS0_GC_RBB2_15_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_15_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_15_POS))

/* 0x740 : ss0_rbb_tbl4 */
#define RF_ANA1_SS0_RBB_TBL4_OFFSET                             (0x740)
#define RF_ANA1_SS0_GC_RBB1_16                                  RF_ANA1_SS0_GC_RBB1_16
#define RF_ANA1_SS0_GC_RBB1_16_POS                              (0U)
#define RF_ANA1_SS0_GC_RBB1_16_LEN                              (2U)
#define RF_ANA1_SS0_GC_RBB1_16_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB1_16_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_16_POS)
#define RF_ANA1_SS0_GC_RBB1_16_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB1_16_LEN)-1)<<RF_ANA1_SS0_GC_RBB1_16_POS))
#define RF_ANA1_SS0_GC_RBB2_16                                  RF_ANA1_SS0_GC_RBB2_16
#define RF_ANA1_SS0_GC_RBB2_16_POS                              (4U)
#define RF_ANA1_SS0_GC_RBB2_16_LEN                              (3U)
#define RF_ANA1_SS0_GC_RBB2_16_MSK                              (((1U<<RF_ANA1_SS0_GC_RBB2_16_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_16_POS)
#define RF_ANA1_SS0_GC_RBB2_16_UMSK                             (~(((1U<<RF_ANA1_SS0_GC_RBB2_16_LEN)-1)<<RF_ANA1_SS0_GC_RBB2_16_POS))

/* 0x758 : ss0_2g_rosdac_tbl0 */
#define RF_ANA1_SS0_2G_ROSDAC_TBL0_OFFSET                       (0x758)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC0                             RF_ANA1_SS0_2G_ROSDAC_I_GC0
#define RF_ANA1_SS0_2G_ROSDAC_I_GC0_POS                         (0U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC0_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC0_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC0_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC0_POS)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC0_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC0_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC0_POS))
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC0                             RF_ANA1_SS0_2G_ROSDAC_Q_GC0
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC0_POS                         (8U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC0_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC0_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC0_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC0_POS)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC0_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC0_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC0_POS))
#define RF_ANA1_SS0_2G_ROSDAC_I_GC1                             RF_ANA1_SS0_2G_ROSDAC_I_GC1
#define RF_ANA1_SS0_2G_ROSDAC_I_GC1_POS                         (16U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC1_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC1_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC1_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC1_POS)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC1_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC1_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC1_POS))
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC1                             RF_ANA1_SS0_2G_ROSDAC_Q_GC1
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC1_POS                         (24U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC1_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC1_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC1_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC1_POS)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC1_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC1_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC1_POS))

/* 0x75C : ss0_2g_rosdac_tbl1 */
#define RF_ANA1_SS0_2G_ROSDAC_TBL1_OFFSET                       (0x75C)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC2                             RF_ANA1_SS0_2G_ROSDAC_I_GC2
#define RF_ANA1_SS0_2G_ROSDAC_I_GC2_POS                         (0U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC2_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC2_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC2_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC2_POS)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC2_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC2_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC2_POS))
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC2                             RF_ANA1_SS0_2G_ROSDAC_Q_GC2
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC2_POS                         (8U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC2_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC2_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC2_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC2_POS)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC2_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC2_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC2_POS))
#define RF_ANA1_SS0_2G_ROSDAC_I_GC3                             RF_ANA1_SS0_2G_ROSDAC_I_GC3
#define RF_ANA1_SS0_2G_ROSDAC_I_GC3_POS                         (16U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC3_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC3_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC3_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC3_POS)
#define RF_ANA1_SS0_2G_ROSDAC_I_GC3_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_I_GC3_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_I_GC3_POS))
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC3                             RF_ANA1_SS0_2G_ROSDAC_Q_GC3
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC3_POS                         (24U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC3_LEN                         (6U)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC3_MSK                         (((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC3_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC3_POS)
#define RF_ANA1_SS0_2G_ROSDAC_Q_GC3_UMSK                        (~(((1U<<RF_ANA1_SS0_2G_ROSDAC_Q_GC3_LEN)-1)<<RF_ANA1_SS0_2G_ROSDAC_Q_GC3_POS))

/* 0x760 : ss0_5g_rosdac_tbl0 */
#define RF_ANA1_SS0_5G_ROSDAC_TBL0_OFFSET                       (0x760)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC0                             RF_ANA1_SS0_5G_ROSDAC_I_GC0
#define RF_ANA1_SS0_5G_ROSDAC_I_GC0_POS                         (0U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC0_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC0_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC0_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC0_POS)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC0_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC0_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC0_POS))
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC0                             RF_ANA1_SS0_5G_ROSDAC_Q_GC0
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC0_POS                         (8U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC0_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC0_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC0_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC0_POS)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC0_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC0_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC0_POS))
#define RF_ANA1_SS0_5G_ROSDAC_I_GC1                             RF_ANA1_SS0_5G_ROSDAC_I_GC1
#define RF_ANA1_SS0_5G_ROSDAC_I_GC1_POS                         (16U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC1_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC1_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC1_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC1_POS)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC1_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC1_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC1_POS))
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC1                             RF_ANA1_SS0_5G_ROSDAC_Q_GC1
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC1_POS                         (24U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC1_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC1_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC1_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC1_POS)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC1_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC1_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC1_POS))

/* 0x764 : ss0_5g_rosdac_tbl1 */
#define RF_ANA1_SS0_5G_ROSDAC_TBL1_OFFSET                       (0x764)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC2                             RF_ANA1_SS0_5G_ROSDAC_I_GC2
#define RF_ANA1_SS0_5G_ROSDAC_I_GC2_POS                         (0U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC2_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC2_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC2_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC2_POS)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC2_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC2_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC2_POS))
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC2                             RF_ANA1_SS0_5G_ROSDAC_Q_GC2
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC2_POS                         (8U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC2_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC2_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC2_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC2_POS)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC2_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC2_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC2_POS))
#define RF_ANA1_SS0_5G_ROSDAC_I_GC3                             RF_ANA1_SS0_5G_ROSDAC_I_GC3
#define RF_ANA1_SS0_5G_ROSDAC_I_GC3_POS                         (16U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC3_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC3_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC3_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC3_POS)
#define RF_ANA1_SS0_5G_ROSDAC_I_GC3_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_I_GC3_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_I_GC3_POS))
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC3                             RF_ANA1_SS0_5G_ROSDAC_Q_GC3
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC3_POS                         (24U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC3_LEN                         (6U)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC3_MSK                         (((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC3_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC3_POS)
#define RF_ANA1_SS0_5G_ROSDAC_Q_GC3_UMSK                        (~(((1U<<RF_ANA1_SS0_5G_ROSDAC_Q_GC3_LEN)-1)<<RF_ANA1_SS0_5G_ROSDAC_Q_GC3_POS))

/* 0x778 : ss0_2g_rxiq_ctrl_tbl0 */
#define RF_ANA1_SS0_2G_RXIQ_CTRL_TBL0_OFFSET                    (0x778)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0                     RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_POS                 (0U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_LEN                 (10U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_MSK                 (((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_POS)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_UMSK                (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC0_POS))
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0                      RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_POS                  (16U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_LEN                  (11U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_MSK                  (((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_POS)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_UMSK                 (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC0_POS))

/* 0x77C : ss0_2g_rxiq_ctrl_tbl1 */
#define RF_ANA1_SS0_2G_RXIQ_CTRL_TBL1_OFFSET                    (0x77C)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1                     RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_POS                 (0U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_LEN                 (10U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_MSK                 (((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_POS)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_UMSK                (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC1_POS))
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1                      RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_POS                  (16U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_LEN                  (11U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_MSK                  (((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_POS)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_UMSK                 (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC1_POS))

/* 0x780 : ss0_2g_rxiq_ctrl_tbl2 */
#define RF_ANA1_SS0_2G_RXIQ_CTRL_TBL2_OFFSET                    (0x780)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2                     RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_POS                 (0U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_LEN                 (10U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_MSK                 (((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_POS)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_UMSK                (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC2_POS))
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2                      RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_POS                  (16U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_LEN                  (11U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_MSK                  (((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_POS)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_UMSK                 (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC2_POS))

/* 0x784 : ss0_2g_rxiq_ctrl_tbl3 */
#define RF_ANA1_SS0_2G_RXIQ_CTRL_TBL3_OFFSET                    (0x784)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3                     RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_POS                 (0U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_LEN                 (10U)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_MSK                 (((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_POS)
#define RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_UMSK                (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_PHASE_COMP_GC3_POS))
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3                      RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_POS                  (16U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_LEN                  (11U)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_MSK                  (((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_POS)
#define RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_UMSK                 (~(((1U<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_2G_RX_IQ_GAIN_COMP_GC3_POS))

/* 0x788 : ss0_5g_rxiq_ctrl_tbl0 */
#define RF_ANA1_SS0_5G_RXIQ_CTRL_TBL0_OFFSET                    (0x788)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0                     RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_POS                 (0U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_LEN                 (10U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_MSK                 (((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_POS)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_UMSK                (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC0_POS))
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0                      RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_POS                  (16U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_LEN                  (11U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_MSK                  (((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_POS)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_UMSK                 (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC0_POS))

/* 0x78C : ss0_5g_rxiq_ctrl_tbl1 */
#define RF_ANA1_SS0_5G_RXIQ_CTRL_TBL1_OFFSET                    (0x78C)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1                     RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_POS                 (0U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_LEN                 (10U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_MSK                 (((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_POS)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_UMSK                (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC1_POS))
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1                      RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_POS                  (16U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_LEN                  (11U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_MSK                  (((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_POS)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_UMSK                 (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC1_POS))

/* 0x790 : ss0_5g_rxiq_ctrl_tbl2 */
#define RF_ANA1_SS0_5G_RXIQ_CTRL_TBL2_OFFSET                    (0x790)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2                     RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_POS                 (0U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_LEN                 (10U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_MSK                 (((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_POS)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_UMSK                (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC2_POS))
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2                      RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_POS                  (16U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_LEN                  (11U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_MSK                  (((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_POS)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_UMSK                 (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC2_POS))

/* 0x794 : ss0_5g_rxiq_ctrl_tbl3 */
#define RF_ANA1_SS0_5G_RXIQ_CTRL_TBL3_OFFSET                    (0x794)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3                     RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_POS                 (0U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_LEN                 (10U)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_MSK                 (((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_POS)
#define RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_UMSK                (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_PHASE_COMP_GC3_POS))
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3                      RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_POS                  (16U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_LEN                  (11U)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_MSK                  (((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_POS)
#define RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_UMSK                 (~(((1U<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_ANA1_SS0_5G_RX_IQ_GAIN_COMP_GC3_POS))


struct  rf_ana1_reg {
    /* 0x0 : Silicon revision */
    union {
        struct {
            uint32_t rf_id                          :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t fw_rev                         :  8; /* [15: 8],          r,        0x0 */
            uint32_t hw_rev                         :  8; /* [23:16],          r,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_rev;

    /* 0x4 : Digital Control */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t rf_fsm0_ctrl_en                :  1; /* [    1],        r/w,        0x1 */
            uint32_t rf_fsm0_t2r_cal_mode           :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t rf_fsm0_state                  :  3; /* [ 6: 4],          r,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rf_fsm0_rc_state_dbg           :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t rf_fsm0_rc_state_dbg_en        :  1; /* [   11],        r/w,        0x0 */
            uint32_t rf_fsm0_st_int_sel             :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rf_fsm0_st_int                 :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rf_fsm0_st_int_clr             :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rf_fsm0_st_int_set             :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t rf_fsm0_rc_state_value         :  3; /* [30:28],          r,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl_hw;

    /* 0x8 : rfsm status reg */
    union {
        struct {
            uint32_t rf_fsm_sw_st                   :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rf_fsm_sw_st_vld               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t full_cal_en                    :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t inc_cal_timeout                :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_unlocked                    :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl_sw;

    /* 0xC : Control logic switch */
    union {
        struct {
            uint32_t rf_resv0                       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rfctrl_hw_en;

    /* 0x10 : temp_comp */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } temp_comp;

    /* 0x14 : Calibration mode register */
    union {
        struct {
            uint32_t inc_fcal_en                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t inc_acal_en                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t lna_pwrdet_cal_en              :  1; /* [    8],        r/w,        0x0 */
            uint32_t rccal_en                       :  1; /* [    9],        r/w,        0x0 */
            uint32_t dpd_en                         :  1; /* [   10],        r/w,        0x0 */
            uint32_t inc_roscal_en                  :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_ctrlen;

    /* 0x18 : ZRF Control register 0 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t ppu_lead                       :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t pud_vco_dly                    :  2; /* [11:10],        r/w,        0x1 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_base_ctrl1;

    /* 0x1C : ZRF Control register 0 */
    union {
        struct {
            uint32_t pu_ctrl_hw                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t rx_gain_ctrl_hw                :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx_gain_ctrl_hw                :  1; /* [    2],        r/w,        0x1 */
            uint32_t lna_ctrl_hw                    :  1; /* [    3],        r/w,        0x1 */
            uint32_t rxcomp_ctrl_hw                 :  1; /* [    4],        r/w,        0x1 */
            uint32_t pa_mode_ctrl_hw                :  1; /* [    5],        r/w,        0x1 */
            uint32_t adpll_fast_inc_fcal_en_ctrl_hw :  1; /* [    6],        r/w,        0x1 */
            uint32_t adpll_force_lf_fast_mode_ctrl_hw :  1; /* [    7],        r/w,        0x1 */
            uint32_t vco_idac_ctrl_hw               :  1; /* [    8],        r/w,        0x1 */
            uint32_t adpll_lf_ctrl_hw               :  1; /* [    9],        r/w,        0x1 */
            uint32_t adpll_sdm_dith_force_en_ctrl_hw :  1; /* [   10],        r/w,        0x1 */
            uint32_t adpll_sdm_bypass_ctrl_hw       :  1; /* [   11],        r/w,        0x1 */
            uint32_t adpll_sdm_in_ctrl_hw           :  1; /* [   12],        r/w,        0x1 */
            uint32_t adpll_lp_mode_ctrl_hw          :  1; /* [   13],        r/w,        0x1 */
            uint32_t lobuf_en_ctrl_hw               :  1; /* [   14],        r/w,        0x1 */
            uint32_t lna_pwrdet_out_rstn_ctrl_hw    :  1; /* [   15],        r/w,        0x1 */
            uint32_t rbb_pkdet_out_rstn_ctrl_hw     :  1; /* [   16],        r/w,        0x1 */
            uint32_t rbb_pkdet_en_ctrl_hw           :  1; /* [   17],        r/w,        0x1 */
            uint32_t rbb_bt_mode_ctrl_hw            :  1; /* [   18],        r/w,        0x1 */
            uint32_t lpvco_idac_ctrl_hw             :  1; /* [   19],        r/w,        0x1 */
            uint32_t reserved_20_26                 :  7; /* [26:20],       rsvd,        0x0 */
            uint32_t rxadc_ctrl_hw                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t txcal_ctrl_hw                  :  1; /* [   28],        r/w,        0x0 */
            uint32_t dpd_ctrl_hw                    :  1; /* [   29],        r/w,        0x0 */
            uint32_t pwrdet_ctrl_hw                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_base_ctrl2;

    /* 0x20 : pucr */
    union {
        struct {
            uint32_t reserved_0_2                   :  3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t pu_tosdac                      :  1; /* [    3],        r/w,        0x0 */
            uint32_t pu_rosdac                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t pu_pkdet                       :  1; /* [    5],        r/w,        0x0 */
            uint32_t pu_lna_pwrdet                  :  1; /* [    6],        r/w,        0x0 */
            uint32_t pu_pwrdet_5g_osdac             :  1; /* [    7],        r/w,        0x0 */
            uint32_t pu_pwrdet_5g                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t pu_pwrdet_2g_osdac             :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_pwrdet_2g                   :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_atest                       :  1; /* [   11],        r/w,        0x0 */
            uint32_t ppu_atest                      :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr;

    /* 0x24 : pucr_fsm */
    union {
        struct {
            uint32_t rxsw_en                        :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t pu_lna                         :  1; /* [    2],        r/w,        0x0 */
            uint32_t pu_rmxgm                       :  1; /* [    3],        r/w,        0x0 */
            uint32_t pu_rmx                         :  1; /* [    4],        r/w,        0x0 */
            uint32_t pu_rbb                         :  1; /* [    5],        r/w,        0x0 */
            uint32_t pu_rxadc                       :  1; /* [    6],        r/w,        0x0 */
            uint32_t rxadc_clk_en                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t pu_pa_2g                       :  1; /* [    8],        r/w,        0x0 */
            uint32_t pu_tmx_2g                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_pa_5g                       :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_tmx_5g                      :  1; /* [   11],        r/w,        0x0 */
            uint32_t pu_tbb                         :  1; /* [   12],        r/w,        0x0 */
            uint32_t pu_dac                         :  1; /* [   13],        r/w,        0x0 */
            uint32_t pu_rcal                        :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pu_adpll                       :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_vco                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_osmx                        :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_fbdiv                       :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_lobuf                       :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_vbuf_lodist                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_dtc                         :  1; /* [   22],        r/w,        0x0 */
            uint32_t pu_tdc                         :  1; /* [   23],        r/w,        0x0 */
            uint32_t pu_lpvco                       :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_fsm;

    /* 0x28 : read only from hardware logic */
    union {
        struct {
            uint32_t rxsw_en_hw                     :  2; /* [ 1: 0],          r,        0x3 */
            uint32_t pu_lna_hw                      :  1; /* [    2],          r,        0x0 */
            uint32_t pu_rmxgm_hw                    :  1; /* [    3],          r,        0x0 */
            uint32_t pu_rmx_hw                      :  1; /* [    4],          r,        0x0 */
            uint32_t pu_rbb_hw                      :  1; /* [    5],          r,        0x0 */
            uint32_t pu_rxadc_hw                    :  1; /* [    6],          r,        0x0 */
            uint32_t rxadc_clk_en_hw                :  1; /* [    7],          r,        0x0 */
            uint32_t pu_pa_2g_hw                    :  1; /* [    8],          r,        0x0 */
            uint32_t pu_tmx_2g_hw                   :  1; /* [    9],          r,        0x0 */
            uint32_t pu_pa_5g_hw                    :  1; /* [   10],          r,        0x0 */
            uint32_t pu_tmx_5g_hw                   :  1; /* [   11],          r,        0x0 */
            uint32_t pu_tbb_hw                      :  1; /* [   12],          r,        0x0 */
            uint32_t pu_dac_hw                      :  1; /* [   13],          r,        0x0 */
            uint32_t pu_rcal_hw                     :  1; /* [   14],          r,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pu_adpll_hw                    :  1; /* [   16],          r,        0x0 */
            uint32_t pu_vco_hw                      :  1; /* [   17],          r,        0x0 */
            uint32_t pu_osmx_hw                     :  1; /* [   18],          r,        0x0 */
            uint32_t pu_fbdiv_hw                    :  1; /* [   19],          r,        0x0 */
            uint32_t pu_lobuf_hw                    :  1; /* [   20],          r,        0x0 */
            uint32_t pu_vbuf_lodist_hw              :  1; /* [   21],          r,        0x0 */
            uint32_t pu_dtc_hw                      :  1; /* [   22],          r,        0x0 */
            uint32_t pu_tdc_hw                      :  1; /* [   23],          r,        0x0 */
            uint32_t pu_lpvco_hw                    :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_fsm_hw;

    /* 0x2C : ppu_pud_hw */
    union {
        struct {
            uint32_t ppu_lna_hw                     :  1; /* [    0],          r,        0x1 */
            uint32_t ppu_rmxgm_hw                   :  1; /* [    1],          r,        0x1 */
            uint32_t ppu_rbb_hw                     :  1; /* [    2],          r,        0x1 */
            uint32_t ppu_rxadc_hw                   :  1; /* [    3],          r,        0x1 */
            uint32_t ppu_vco_hw                     :  1; /* [    4],          r,        0x1 */
            uint32_t ppu_osmx_hw                    :  1; /* [    5],          r,        0x1 */
            uint32_t ppu_fbdiv_hw                   :  1; /* [    6],          r,        0x1 */
            uint32_t ppu_lobuf_hw                   :  1; /* [    7],          r,        0x1 */
            uint32_t ppu_vbuf_lodist_hw             :  1; /* [    8],          r,        0x1 */
            uint32_t ppu_dtc_hw                     :  1; /* [    9],          r,        0x1 */
            uint32_t ppu_tdc_hw                     :  1; /* [   10],          r,        0x1 */
            uint32_t ppu_lpvco_hw                   :  1; /* [   11],          r,        0x1 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t pud_vco_hw                     :  1; /* [   16],          r,        0x1 */
            uint32_t pud_dtc_hw                     :  1; /* [   17],          r,        0x1 */
            uint32_t pud_lpvco_hw                   :  1; /* [   18],          r,        0x1 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ppu_pud_hw;

    /* 0x30 : gain control1 */
    union {
        struct {
            uint32_t gc_lna                         :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t gc_rmxgm                       :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t gc_rbb1                        :  2; /* [ 6: 5],        r/w,        0x3 */
            uint32_t gc_rbb2                        :  3; /* [ 9: 7],        r/w,        0x4 */
            uint32_t gc_pa                          :  2; /* [11:10],        r/w,        0x3 */
            uint32_t gc_tmx                         :  4; /* [15:12],        r/w,        0xf */
            uint32_t reserved_16_19                 :  4; /* [19:16],       rsvd,        0x0 */
            uint32_t gc_tbb                         :  6; /* [25:20],        r/w,       0x15 */
            uint32_t reserved_26                    :  1; /* [   26],       rsvd,        0x0 */
            uint32_t gc_tbb_boost                   :  2; /* [28:27],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain1;

    /* 0x34 : trx gain hardware readback */
    union {
        struct {
            uint32_t gc_lna_hw                      :  3; /* [ 2: 0],          r,        0x7 */
            uint32_t gc_rmxgm_hw                    :  2; /* [ 4: 3],          r,        0x3 */
            uint32_t gc_rbb1_hw                     :  2; /* [ 6: 5],          r,        0x3 */
            uint32_t gc_rbb2_hw                     :  3; /* [ 9: 7],          r,        0x4 */
            uint32_t gc_pa_hw                       :  2; /* [11:10],          r,        0x3 */
            uint32_t gc_tmx_hw                      :  4; /* [15:12],          r,       0x14 */
            uint32_t reserved_16_19                 :  4; /* [19:16],       rsvd,        0x0 */
            uint32_t gc_tbb_hw                      :  6; /* [25:20],          r,       0x15 */
            uint32_t reserved_26                    :  1; /* [   26],       rsvd,        0x0 */
            uint32_t gc_tbb_boost_hw                :  2; /* [28:27],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain_hw;

    /* 0x38 : dc test register */
    union {
        struct {
            uint32_t atest_dc_tp_12_int_en          :  1; /* [    0],        r/w,        0x0 */
            uint32_t atest_dc_tp_12_en              :  1; /* [    1],        r/w,        0x0 */
            uint32_t atest_dc_tp_18_en              :  1; /* [    2],        r/w,        0x0 */
            uint32_t ten_atest                      :  1; /* [    3],        r/w,        0x0 */
            uint32_t ten_rrf_2g_lna                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t ten_rrf_5g_lna                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t ten_rrf_rmxgm                  :  1; /* [    6],        r/w,        0x0 */
            uint32_t ten_rrf_rmx                    :  1; /* [    7],        r/w,        0x0 */
            uint32_t ten_tia_0                      :  1; /* [    8],        r/w,        0x0 */
            uint32_t ten_tia_1                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t ten_bq_0                       :  1; /* [   10],        r/w,        0x0 */
            uint32_t ten_bq_1                       :  1; /* [   11],        r/w,        0x0 */
            uint32_t ten_rxadc                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ten_pa_2g                      :  1; /* [   16],        r/w,        0x0 */
            uint32_t ten_pa_5g                      :  1; /* [   17],        r/w,        0x0 */
            uint32_t ten_pwrdet_2g                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t ten_pwrdet_5g                  :  1; /* [   19],        r/w,        0x0 */
            uint32_t ten_trf_ptat                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t ten_tbb                        :  1; /* [   21],        r/w,        0x0 */
            uint32_t ten_dac_i                      :  1; /* [   22],        r/w,        0x0 */
            uint32_t ten_dac_q                      :  1; /* [   23],        r/w,        0x0 */
            uint32_t ten_vco                        :  1; /* [   24],        r/w,        0x0 */
            uint32_t ten_osmx                       :  1; /* [   25],        r/w,        0x0 */
            uint32_t ten_lobuf                      :  1; /* [   26],        r/w,        0x0 */
            uint32_t ten_lodist_rx                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t ten_dtc                        :  1; /* [   28],        r/w,        0x0 */
            uint32_t ten_tdc                        :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_dc;

    /* 0x3C : digital test register */
    union {
        struct {
            uint32_t reserved_0_5                   :  6; /* [ 5: 0],       rsvd,        0x0 */
            uint32_t dten_dtc_in                    :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t dten_dtc_out                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t dten_fbdiv_digclk              :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_22                 : 13; /* [22:10],       rsvd,        0x0 */
            uint32_t dten_fbdiv_pclk                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_dig;

    /* 0x40 : ac test register */
    union {
        struct {
            uint32_t atest_ac_out_en                :  1; /* [    0],        r/w,        0x0 */
            uint32_t atest_ac_in_en_q               :  1; /* [    1],        r/w,        0x0 */
            uint32_t atest_ac_in_en_i               :  1; /* [    2],        r/w,        0x0 */
            uint32_t atest_dac_ac_test_en           :  1; /* [    3],        r/w,        0x0 */
            uint32_t atest_gain_r9                  :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t atest_op_cc                    :  4; /* [11: 8],        r/w,        0x4 */
            uint32_t atest_bm_op_in                 :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t atest_bm_op_out                :  2; /* [17:16],        r/w,        0x1 */
            uint32_t atest_vcm                      :  2; /* [19:18],        r/w,        0x2 */
            uint32_t atest_vstartup                 :  2; /* [21:20],        r/w,        0x2 */
            uint32_t atest_startup_set              :  1; /* [   22],        r/w,        0x0 */
            uint32_t atest_bias_sel_cgm             :  1; /* [   23],        r/w,        0x1 */
            uint32_t atest_dac_vcm                  :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t atest_cbank_fc                 :  4; /* [31:28],        r/w,        0x6 */
        }BF;
        uint32_t WORD;
    } ten_ac;

    /* 0x44 : RX normal bias mode registers */
    union {
        struct {
            uint32_t rcal_icx_code                  :  6; /* [ 5: 0],        r/w,       0x1c */
            uint32_t rcal_icx_out_en                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rcal_iptat_code                :  5; /* [12: 8],        r/w,        0xf */
            uint32_t rcal_iptat_out_en              :  1; /* [   13],        r/w,        0x0 */
            uint32_t rcal_imix_out_en               :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pa_icx_scale                   :  4; /* [19:16],        r/w,        0x7 */
            uint32_t pa_iptat_scale                 :  4; /* [23:20],        r/w,        0x8 */
            uint32_t txdac_icx_scale                :  4; /* [27:24],        r/w,        0x7 */
            uint32_t txdac_iptat_scale              :  4; /* [31:28],        r/w,        0x8 */
        }BF;
        uint32_t WORD;
    } cip_0;

    /* 0x48 : pa_0 */
    union {
        struct {
            uint32_t pa_vbcas1                      :  4; /* [ 3: 0],        r/w,        0x7 */
            uint32_t pa_vbcore                      :  5; /* [ 8: 4],        r/w,        0xc */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pa_iet                         :  5; /* [16:12],        r/w,        0xc */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pa_iaq                         :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pa_vbpmos                      :  4; /* [27:24],        r/w,        0x9 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_0;

    /* 0x4C : pa_1 */
    union {
        struct {
            uint32_t pa_vbcas1_hw                   :  4; /* [ 3: 0],          r,        0x7 */
            uint32_t pa_vbcore_hw                   :  5; /* [ 8: 4],          r,        0xc */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pa_iet_hw                      :  5; /* [16:12],          r,        0xc */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pa_iaq_hw                      :  3; /* [22:20],          r,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pa_vbpmos_hw                   :  4; /* [27:24],          r,        0x9 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_1;

    /* 0x50 : pa_2g_0 */
    union {
        struct {
            uint32_t pa_2g_vbcas2                   :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t pa_2g_bm_sf                    :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t pa_2g_lz_bias_en               :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pa_2g_op_cc                    :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pa_2g_cn_sel                   :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pa_2g_ibc_op                   :  3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pa_2g_gc_att                   :  3; /* [26:24],        r/w,        0x7 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pa_2g_cout_sel                 :  3; /* [30:28],        r/w,        0x2 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_2g_0;

    /* 0x54 : pa_2g_1 */
    union {
        struct {
            uint32_t pa_2g_vbpmos_range             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t pa_2g_iptat_sel                :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_2g_1;

    /* 0x58 : tmx_2g */
    union {
        struct {
            uint32_t tmx_2g_vbsw                    :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t tmx_2g_vbcas                   :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tmx_2g_vbcas2                  :  3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t tmx_2g_bm_cas_bulk             :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tmx_2g_deq                     :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tmx_2g;

    /* 0x5C : pwrdet_2g */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t pwrdet_2g_mul_short_en         :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t pwrdet_2g_mul_gc_att           :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t pwrdet_2g_gc_att               :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t pwrdet_2g_osdac_code           :  5; /* [16:12],        r/w,        0xf */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pwrdet_2g_osdac_bm             :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pwrdet_2g_mul_bm               :  3; /* [26:24],        r/w,        0x2 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwrdet_2g;

    /* 0x60 : pa_5g_0 */
    union {
        struct {
            uint32_t pa_5g_vbcas2                   :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t pa_5g_bm_sf                    :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t pa_5g_lz_bias_en               :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pa_5g_op_cc                    :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pa_5g_cn_sel                   :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pa_5g_ibc_op                   :  3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pa_5g_gc_att                   :  3; /* [26:24],        r/w,        0x7 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pa_5g_cout_sel                 :  3; /* [30:28],        r/w,        0x2 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_5g_0;

    /* 0x64 : pa_5g_1 */
    union {
        struct {
            uint32_t pa_5g_vbpmos_range             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t pa_5g_iptat_sel                :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t pa_5g_cd_sel                   :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_5g_1;

    /* 0x68 : tmx_5g */
    union {
        struct {
            uint32_t tmx_5g_vbsw                    :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t tmx_5g_vbcas                   :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tmx_5g_vbcas2                  :  3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t tmx_5g_bm_cas_bulk             :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tmx_5g_deq                     :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t tmx_cs                         :  5; /* [24:20],        r/w,        0x6 */
            uint32_t tmx_cs_hw                      :  5; /* [29:25],          r,        0x6 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tmx_5g;

    /* 0x6C : pwrdet_5g_0 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t pwrdet_5g_pr_en                :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t pwrdet_5g_pr_gc                :  1; /* [    3],        r/w,        0x0 */
            uint32_t pwrdet_5g_mul_gc_att           :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t pwrdet_5g_gc_att               :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t pwrdet_5g_osdac_code           :  5; /* [16:12],        r/w,        0xf */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pwrdet_5g_osdac_bm             :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pwrdet_5g_mul_bm               :  3; /* [26:24],        r/w,        0x2 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwrdet_5g_0;

    /* 0x70 : pwrdet_5g_1 */
    union {
        struct {
            uint32_t reserved_0_11                  : 12; /* [11: 0],       rsvd,        0x0 */
            uint32_t pwrdet_5g_pr_swq               :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t pwrdet_5g_pr_swi               :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t pwrdet_5g_pr_bm                :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pwrdet_5g_1;

    /* 0x74 : trf_iptat_gen */
    union {
        struct {
            uint32_t trf_ptat_da_scale              :  4; /* [ 3: 0],        r/w,        0x7 */
            uint32_t trf_ptat_pa_scale              :  4; /* [ 7: 4],        r/w,        0x7 */
            uint32_t trf_ptat_rcal_code             :  5; /* [12: 8],        r/w,       0x1c */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t trf_ptat_mode_sel              :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t trf_ptat_2g_5g_sel             :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t trf_ptat_en                    :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trf_iptat_gen;

    /* 0x78 : tbb1 */
    union {
        struct {
            uint32_t reserved_0_15                  : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t tbb_vcm                        :  2; /* [17:16],        r/w,        0x2 */
            uint32_t tbb_cflt                       :  2; /* [19:18],        r/w,        0x0 */
            uint32_t tbb_bm_sf                      :  2; /* [21:20],        r/w,        0x2 */
            uint32_t tbb_bm_cg                      :  3; /* [24:22],        r/w,        0x4 */
            uint32_t reserved_25                    :  1; /* [   25],       rsvd,        0x0 */
            uint32_t tbb_iq_bias_short              :  1; /* [   26],        r/w,        0x1 */
            uint32_t tbb_atest_out_en               :  1; /* [   27],        r/w,        0x0 */
            uint32_t tbb_atest_res_sel              :  3; /* [30:28],        r/w,        0x0 */
            uint32_t tbb_atest_in_en                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb1;

    /* 0x7C : tbb2 */
    union {
        struct {
            uint32_t tosdac_q                       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tosdac_i                       :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t tosdac_q_hw                    :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tosdac_i_hw                    :  6; /* [29:24],          r,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb2;

    /* 0x80 : rrf1 */
    union {
        struct {
            uint32_t lna_bm                         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t lna_bm_hw                      :  4; /* [ 7: 4],          r,        0x6 */
            uint32_t lna_load_csw                   :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t lna_load_csw_hw                :  4; /* [15:12],          r,        0x6 */
            uint32_t lna_load_csw_gc                :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lna_rfb1                       :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t lna_rfb2                       :  3; /* [26:24],        r/w,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t lna_rfb3                       :  3; /* [30:28],        r/w,        0x4 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf1;

    /* 0x84 : rrf2 */
    union {
        struct {
            uint32_t lna_sel                        :  1; /* [    0],        r/w,        0x0 */
            uint32_t lna_sel_hw                     :  1; /* [    1],          r,        0x0 */
            uint32_t lna_casbias_sel                :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lna_cap1                       :  3; /* [ 6: 4],        r/w,        0x2 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t lna_cap2                       :  3; /* [10: 8],        r/w,        0x2 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t lna_lg_cap                     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lna_bm_boost                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t rrf_bm_boost_sel               :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t lna_cap1_lg                    :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t lna_cap1_hg                    :  3; /* [26:24],        r/w,        0x2 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t lna_cap1_hw                    :  3; /* [30:28],          r,        0x2 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf2;

    /* 0x88 : rrf3 */
    union {
        struct {
            uint32_t lna_load_csw_lg                :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t lna_load_csw_hg                :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t lna_bm_lg                      :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t lna_bm_lg_lpmode               :  4; /* [15:12],        r/w,        0x6 */
            uint32_t lna_bm_hg                      :  4; /* [19:16],        r/w,        0x6 */
            uint32_t lna_bm_hg_lpmode               :  4; /* [23:20],        r/w,        0x6 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf3;

    /* 0x8C : rrf4 */
    union {
        struct {
            uint32_t lna_pwrdet_out_rstn            :  1; /* [    0],        r/w,        0x0 */
            uint32_t lna_pwrdet_out_rstn_hw         :  1; /* [    1],          r,        0x0 */
            uint32_t lna_pwrdet_out_raw             :  1; /* [    2],          r,        0x0 */
            uint32_t lna_pwrdet_out_latch           :  1; /* [    3],          r,        0x0 */
            uint32_t lna_pwrdet_rc                  :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lna_pwrdet_vout_rbank          :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lna_pwrdet_comp_vref           :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t lna_pwrdet_osdac               :  6; /* [15:10],        r/w,        0x0 */
            uint32_t rmxgm_bm_boost                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rmxgm_bm                       :  3; /* [22:20],        r/w,        0x1 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rmxgm_casbias_sel              :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t rmx_bm                         :  3; /* [30:28],        r/w,        0x5 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf4;

    /* 0x90 : rbb1 */
    union {
        struct {
            uint32_t rosdac_q                       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_i                       :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_q_hw                    :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_i_hw                    :  6; /* [29:24],          r,       0x20 */
            uint32_t rosdac_range                   :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb1;

    /* 0x94 : rbb2 */
    union {
        struct {
            uint32_t rbb_cap2_fc_q                  :  7; /* [ 6: 0],        r/w,       0x34 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_cap2_fc_i                  :  7; /* [14: 8],        r/w,       0x34 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_q                  :  7; /* [22:16],        r/w,       0x34 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_i                  :  7; /* [30:24],        r/w,       0x34 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb2;

    /* 0x98 : rbb3 */
    union {
        struct {
            uint32_t rbb_rccal_iq_swap              :  1; /* [    0],        r/w,        0x0 */
            uint32_t rbb_pwr_det_en                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t rbb_bq_iqbias_short            :  1; /* [    2],        r/w,        0x1 */
            uint32_t rbb_tia_iqbias_short           :  1; /* [    3],        r/w,        0x1 */
            uint32_t rbb_cgm_r_tune                 :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_bias_sel_cgm               :  1; /* [    8],        r/w,        0x1 */
            uint32_t rbb_startup_set                :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t rbb_vcm                        :  2; /* [13:12],        r/w,        0x2 */
            uint32_t rbb_vstartup                   :  2; /* [15:14],        r/w,        0x2 */
            uint32_t rbb_rccal_dac_vcm              :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t rbb_bt_fif_tune                :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_cap3_wideband_fc           :  4; /* [27:24],        r/w,        0x8 */
            uint32_t rbb_bt_mode                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t rbb_bt_mode_hw                 :  1; /* [   29],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb3;

    /* 0x9C : rbb4 */
    union {
        struct {
            uint32_t rbb_op1_cc                     :  4; /* [ 3: 0],        r/w,        0x4 */
            uint32_t rbb_op2_cc                     :  4; /* [ 7: 4],        r/w,        0x8 */
            uint32_t rbb_op3_cc                     :  4; /* [11: 8],        r/w,        0x4 */
            uint32_t rbb_deq                        :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14                    :  1; /* [   14],       rsvd,        0x0 */
            uint32_t rbb_op2_two_pole_en            :  1; /* [   15],        r/w,        0x1 */
            uint32_t reserved_16_23                 :  8; /* [23:16],       rsvd,        0x0 */
            uint32_t rbb_op2_rz                     :  4; /* [27:24],        r/w,        0x2 */
            uint32_t rbb_rmx_rout_tune              :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb4;

    /* 0xA0 : rbb5 */
    union {
        struct {
            uint32_t rbb_op1_cc_bt                  :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t rbb_op2_cc_bt                  :  4; /* [ 7: 4],        r/w,        0xf */
            uint32_t rbb_op3_cc_bt                  :  4; /* [11: 8],        r/w,        0xf */
            uint32_t rbb_deq_bt                     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14                    :  1; /* [   14],       rsvd,        0x0 */
            uint32_t rbb_op2_two_pole_en_bt         :  1; /* [   15],        r/w,        0x1 */
            uint32_t rbb_op1_cc_hw                  :  4; /* [19:16],          r,        0x8 */
            uint32_t rbb_op2_cc_hw                  :  4; /* [23:20],          r,        0xf */
            uint32_t rbb_op3_cc_hw                  :  4; /* [27:24],          r,        0xf */
            uint32_t rbb_deq_hw                     :  2; /* [29:28],          r,        0x1 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t rbb_op2_two_pole_en_hw         :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } rbb5;

    /* 0xA4 : rbb6 */
    union {
        struct {
            uint32_t rbb_bm_op                      :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t rbb_bm_op1_out                 :  3; /* [ 5: 3],        r/w,        0x5 */
            uint32_t rbb_bm_op2_out                 :  3; /* [ 8: 6],        r/w,        0x5 */
            uint32_t rbb_bm_op3_out                 :  3; /* [11: 9],        r/w,        0x5 */
            uint32_t rbb_bw                         :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_bm_op_hw                   :  3; /* [18:16],          r,        0x2 */
            uint32_t rbb_bm_op1_out_hw              :  3; /* [21:19],          r,        0x5 */
            uint32_t rbb_bm_op2_out_hw              :  3; /* [24:22],          r,        0x5 */
            uint32_t rbb_bm_op3_out_hw              :  3; /* [27:25],          r,        0x5 */
            uint32_t rbb_bw_hw                      :  2; /* [29:28],          r,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb6;

    /* 0xA8 : rbb7 */
    union {
        struct {
            uint32_t rbb_bm_op_bt                   :  3; /* [ 2: 0],        r/w,        0x1 */
            uint32_t rbb_bm_op1_out_bt              :  3; /* [ 5: 3],        r/w,        0x3 */
            uint32_t rbb_bm_op2_out_bt              :  3; /* [ 8: 6],        r/w,        0x3 */
            uint32_t rbb_bm_op3_out_bt              :  3; /* [11: 9],        r/w,        0x3 */
            uint32_t rbb_bw_bt                      :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_bm_op_lpmode               :  3; /* [18:16],        r/w,        0x1 */
            uint32_t rbb_bm_op1_out_lpmode          :  3; /* [21:19],        r/w,        0x3 */
            uint32_t rbb_bm_op2_out_lpmode          :  3; /* [24:22],        r/w,        0x3 */
            uint32_t rbb_bm_op3_out_lpmode          :  3; /* [27:25],        r/w,        0x3 */
            uint32_t rbb_bw_lpmode                  :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb7;

    /* 0xAC : rbb_atest */
    union {
        struct {
            uint32_t rbb_atest_gain_r7_q            :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rbb_atest_gain_r7_i            :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_atest_gain_r8_q            :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rbb_atest_gain_r8_i            :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rbb_atest_out_en               :  1; /* [   16],        r/w,        0x0 */
            uint32_t rbb_atest_in_en                :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t rbb_gain_r6                    :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rbb_gain_r5                    :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_atest;

    /* 0xB0 : rbb_pkdet */
    union {
        struct {
            uint32_t rbb_pkdet_vth                  :  4; /* [ 3: 0],        r/w,        0x5 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t rbb_pkdet_en                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t rbb_pkdet_en_hw                :  1; /* [    9],          r,        0x0 */
            uint32_t rbb_pkdet_out_rstn             :  1; /* [   10],        r/w,        0x0 */
            uint32_t rbb_pkdet_out_rstn_hw          :  1; /* [   11],          r,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t pkdet_out_raw                  :  1; /* [   16],          r,        0x0 */
            uint32_t pkdet_out_latch                :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t rx_lp_mode_en_hw               :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21                    :  1; /* [   21],       rsvd,        0x0 */
            uint32_t rx_lp_mode_en                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_lp_mode_ctrl_hw             :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_pkdet;

    /* 0xB4 : Digital Control */
    union {
        struct {
            uint32_t rxadc_vref_sel                 :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t rxadc_dly_ctl                  :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rxadc_dvdd_sel                 :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t rxadc_gt_rm                    :  1; /* [   12],        r/w,        0x1 */
            uint32_t rxadc_clk_sync_inv             :  1; /* [   13],        r/w,        0x0 */
            uint32_t rxadc_clk_sync_sel             :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rxadc_clk_inv                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rxadc_clk_div_sel              :  1; /* [   20],        r/w,        0x1 */
            uint32_t rxadc_clk_div_sel_lpmode       :  1; /* [   21],        r/w,        0x0 */
            uint32_t rxadc_clk_div_sel_bt           :  1; /* [   22],        r/w,        0x0 */
            uint32_t rxadc_clk_div_sel_hw           :  1; /* [   23],          r,        0x1 */
            uint32_t rxadc_ti_en                    :  1; /* [   24],        r/w,        0x1 */
            uint32_t rxadc_ti_en_lpmode             :  1; /* [   25],        r/w,        0x0 */
            uint32_t rxadc_ti_en_bt                 :  1; /* [   26],        r/w,        0x0 */
            uint32_t rxadc_ti_en_hw                 :  1; /* [   27],          r,        0x1 */
            uint32_t rxadc_vref_cp_en               :  1; /* [   28],        r/w,        0x1 */
            uint32_t rxadc_vbuf_sel                 :  2; /* [30:29],        r/w,        0x1 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxadc_0;

    /* 0xB8 : Digital Control */
    union {
        struct {
            uint32_t rxadc_dout_i                   : 11; /* [10: 0],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t rxadc_dout_q                   : 11; /* [26:16],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxadc_1;

    /* 0xBC : Digital Control */
    union {
        struct {
            uint32_t dac_dvdd_sel                   :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t dac_bias_enhance               :  1; /* [    3],        r/w,        0x1 */
            uint32_t dac_dvdd_dec_sel               :  3; /* [ 6: 4],        r/w,        0x2 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t dac_bias_sel                   :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t dac_bias_sel_hw                :  2; /* [11:10],          r,        0x1 */
            uint32_t dac_rccal_sel                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t dac_clk_sync_960m_inv          :  1; /* [   16],        r/w,        0x1 */
            uint32_t dac_clk_320m_inv               :  1; /* [   17],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t dac_rccal_atest_en             :  1; /* [   20],        r/w,        0x0 */
            uint32_t dac_clk_en_div3                :  1; /* [   21],        r/w,        0x1 */
            uint32_t dac_atest_ch_sel               :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t dac_vlow_sel                   :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t dac_nsink_sel                  :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wrf_dac_0;

    /* 0xC0 : wrf_dac_1 */
    union {
        struct {
            uint32_t dac_din_i_part0                : 11; /* [10: 0],          r,      0x400 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t dac_din_i_part1                : 11; /* [26:16],          r,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wrf_dac_1;

    /* 0xC4 : wrf_dac_2 */
    union {
        struct {
            uint32_t dac_din_i_part2                : 11; /* [10: 0],          r,      0x400 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wrf_dac_2;

    /* 0xC8 : wrf_dac_3 */
    union {
        struct {
            uint32_t dac_din_q_part0                : 11; /* [10: 0],          r,      0x400 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t dac_din_q_part1                : 11; /* [26:16],          r,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wrf_dac_3;

    /* 0xCC : wrf_dac_4 */
    union {
        struct {
            uint32_t dac_din_q_part2                : 11; /* [10: 0],          r,      0x400 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wrf_dac_4;

    /* 0xD0 : wifipll_ana_ctrl */
    union {
        struct {
            uint32_t wifipll_fbdiv_sel_sample_clk   :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t wifipll_fbdiv_sel_adpll_clk    :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t wifipll_dtc_r_sel              :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t wifipll_dtc_bypass_en          :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t wifipll_vco_speed              :  3; /* [10: 8],        r/w,        0x5 */
            uint32_t wifipll_vco_480m_en            :  1; /* [   11],        r/w,        0x0 */
            uint32_t wifipll_vco_ldo_bypass         :  1; /* [   12],        r/w,        0x0 */
            uint32_t wifipll_vco_rsht_sel           :  2; /* [14:13],        r/w,        0x0 */
            uint32_t wifipll_dcomash_bypass         :  1; /* [   15],        r/w,        0x0 */
            uint32_t wifipll_refclk_sel             :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t wifipll_refclk_div_ratio       :  4; /* [23:20],        r/w,        0x2 */
            uint32_t wifipll_rstb                   :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t pu_wifipll                     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_ana_ctrl;

    /* 0xD4 : wifipll_hw_ctrl */
    union {
        struct {
            uint32_t wifipll_pi_bypass_tx           :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t wifipll_pi_bypass_rx           :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t wifipll_pi_bypass_tx_bz        :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5                     :  1; /* [    5],       rsvd,        0x0 */
            uint32_t wifipll_pi_bypass_rx_bz        :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t wifipll_pi_bypass_sel_wifi     :  1; /* [    8],        r/w,        0x1 */
            uint32_t wifipll_pi_bypass_hw           :  1; /* [    9],          r,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t wifipll_vco_rsht_en_tx         :  1; /* [   12],        r/w,        0x1 */
            uint32_t wifipll_vco_rsht_en_rx         :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t wifipll_vco_rsht_en_tx_bz      :  1; /* [   16],        r/w,        0x1 */
            uint32_t wifipll_vco_rsht_en_rx_bz      :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t wifipll_vco_rsht_en_sel_wifi   :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifipll_vco_rsht_en            :  1; /* [   21],        r/w,        0x0 */
            uint32_t wifipll_vco_rsht_en_hw         :  1; /* [   22],          r,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t wifipll_wifi_en_ctrl_hw        :  1; /* [   24],        r/w,        0x1 */
            uint32_t wifipll_bz_en_ctrl_hw          :  1; /* [   25],        r/w,        0x1 */
            uint32_t wifipll_pi_bypass_ctrl_hw      :  1; /* [   26],        r/w,        0x1 */
            uint32_t wifipll_vco_rsht_en_ctrl_hw    :  1; /* [   27],        r/w,        0x1 */
            uint32_t wifipll_fast_inc_fcal_en_ctrl_hw :  1; /* [   28],        r/w,        0x1 */
            uint32_t wifipll_fast_inc_fcal_en_sel_wifi :  1; /* [   29],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_hw_ctrl;

    /* 0xD8 : wifipll_spd_fcal */
    union {
        struct {
            uint32_t wifipll_fcal_threshold         :  1; /* [    0],        r/w,        0x1 */
            uint32_t wifipll_fcal_phaerr_limit      :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t wifipll_fcal_ndiv_adj_en       :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t wifipll_fcal_momcap_ini_ext    :  7; /* [14: 8],        r/w,       0x20 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t wifipll_fcal_mode              :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t wifipll_fast_inc_dvar_comp_en  :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifipll_fast_inc_fcal_en       :  1; /* [   21],        r/w,        0x0 */
            uint32_t wifipll_fast_inc_fcal_en_hw    :  1; /* [   22],          r,        0x0 */
            uint32_t wifipll_inc_fcal_en            :  1; /* [   23],        r/w,        0x0 */
            uint32_t wifipll_spd_unlock_dly         :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25                    :  1; /* [   25],       rsvd,        0x0 */
            uint32_t wifipll_spd_gain               :  2; /* [27:26],        r/w,        0x1 */
            uint32_t wifipll_coarse_path_force_on   :  1; /* [   28],        r/w,        0x0 */
            uint32_t wifipll_coarse_gain            :  2; /* [30:29],        r/w,        0x2 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_spd_fcal;

    /* 0xDC : wifipll_lf_vctrl */
    union {
        struct {
            uint32_t wifipll_mom_update_period      :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t wifipll_mom_force_hold         :  1; /* [    3],        r/w,        0x0 */
            uint32_t wifipll_mom_code_bypass        :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t wifipll_lo_open                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t wifipll_force_lf_fast_mode     :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t wifipll_lf_lsb_ext             :  6; /* [17:12],        r/w,       0x20 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t wifipll_lf_beta_fast           :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifipll_lf_beta_exp            :  3; /* [23:21],        r/w,        0x2 */
            uint32_t wifipll_lf_beta_base           :  2; /* [25:24],        r/w,        0x3 */
            uint32_t wifipll_lf_alpha_fast          :  2; /* [27:26],        r/w,        0x3 */
            uint32_t wifipll_lf_alpha_exp           :  3; /* [30:28],        r/w,        0x3 */
            uint32_t wifipll_lf_alpha_base          :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_lf_vctrl;

    /* 0xE0 : wifipll_sdmin */
    union {
        struct {
            uint32_t wifipll_sdm_in                 : 25; /* [24: 0],        r/w,   0x600000 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_sdmin;

    /* 0xE4 : wifipll_pi_sdm_lms */
    union {
        struct {
            uint32_t wifipll_lms_ext_value          :  6; /* [ 5: 0],        r/w,       0x2b */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t wifipll_lms_ext_en             :  1; /* [    7],        r/w,        0x1 */
            uint32_t wifipll_lms_polarity           :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t wifipll_pha_cancel_en          :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t wifipll_sdm_regrst_en          :  1; /* [   15],        r/w,        0x0 */
            uint32_t wifipll_sdm_out_dly            :  1; /* [   16],        r/w,        0x0 */
            uint32_t wifipll_sdm_order              :  1; /* [   17],        r/w,        0x1 */
            uint32_t wifipll_sdm_noi_dith           :  1; /* [   18],        r/w,        0x0 */
            uint32_t wifipll_sdm_dith_prbs_en       :  1; /* [   19],        r/w,        0x0 */
            uint32_t wifipll_sdm_sig_dith           :  2; /* [21:20],        r/w,        0x2 */
            uint32_t wifipll_sdm_dith_en            :  1; /* [   22],        r/w,        0x1 */
            uint32_t wifipll_sdm_dith_force_en      :  1; /* [   23],        r/w,        0x1 */
            uint32_t wifipll_sdm_bypass             :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_26                 :  2; /* [26:25],       rsvd,        0x0 */
            uint32_t wifipll_pi_space_sel           :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifipll_pi_mod_sel             :  2; /* [29:28],        r/w,        0x3 */
            uint32_t wifipll_pi_clk_sel             :  1; /* [   30],        r/w,        0x1 */
            uint32_t wifipll_pi_bypass              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_pi_sdm_lms;

    /* 0xE8 : wifipll_clktree_rf */
    union {
        struct {
            uint32_t wifipll_dl_ctrl_div30_bz       :  1; /* [    0],        r/w,        0x1 */
            uint32_t wifipll_dl_ctrl_div6_bz        :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t wifipll_en_bz_div30            :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t wifipll_en_bz_div6_hw          :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t wifipll_en_bz_div6             :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t wifipll_dl_ctrl_div3_rf        :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t wifipll_dl_ctrl_div1_rf        :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15_17                 :  3; /* [17:15],       rsvd,        0x0 */
            uint32_t wifipll_sel_div3_div6          :  1; /* [   18],        r/w,        0x0 */
            uint32_t wifipll_sel_div1_div2          :  1; /* [   19],        r/w,        0x0 */
            uint32_t wifipll_en_rf_div3             :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifipll_en_rf_div1_hw          :  1; /* [   21],          r,        0x0 */
            uint32_t wifipll_en_rf_div1             :  1; /* [   22],        r/w,        0x1 */
            uint32_t wifipll_postdiv_en             :  1; /* [   23],        r/w,        0x1 */
            uint32_t wifipll_postdiv                :  8; /* [31:24],        r/w,       0x9c */
        }BF;
        uint32_t WORD;
    } wifipll_clktree_rf;

    /* 0xEC : wifipll_clktree_dig */
    union {
        struct {
            uint32_t wifipll_dl_ctrl_div30          :  1; /* [    0],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div20          :  1; /* [    1],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div12          :  1; /* [    2],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div10          :  1; /* [    3],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div8           :  1; /* [    4],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div6           :  1; /* [    5],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div5           :  1; /* [    6],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div4           :  1; /* [    7],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div3           :  1; /* [    8],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div2           :  1; /* [    9],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_div1           :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t wifipll_en_div30               :  1; /* [   16],        r/w,        0x1 */
            uint32_t wifipll_en_div20               :  1; /* [   17],        r/w,        0x1 */
            uint32_t wifipll_en_div12               :  1; /* [   18],        r/w,        0x1 */
            uint32_t wifipll_en_div10               :  1; /* [   19],        r/w,        0x1 */
            uint32_t wifipll_en_div8                :  1; /* [   20],        r/w,        0x1 */
            uint32_t wifipll_en_div6                :  1; /* [   21],        r/w,        0x1 */
            uint32_t wifipll_en_div5                :  1; /* [   22],        r/w,        0x1 */
            uint32_t wifipll_en_div4                :  1; /* [   23],        r/w,        0x1 */
            uint32_t wifipll_en_div3                :  1; /* [   24],        r/w,        0x1 */
            uint32_t wifipll_en_div2                :  1; /* [   25],        r/w,        0x1 */
            uint32_t wifipll_en_div1                :  1; /* [   26],        r/w,        0x1 */
            uint32_t wifipll_en_usb2_div48          :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_usb2_div48     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_clktree_dig;

    /* 0xF0 : wifipll_reserve */
    union {
        struct {
            uint32_t wifipll_resv0                  :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t wifipll_resv1                  :  8; /* [15: 8],        r/w,       0xff */
            uint32_t wifipll_vco_freq_record        : 16; /* [31:16],        r/w,      0x3c0 */
        }BF;
        uint32_t WORD;
    } wifipll_reserve;

    /* 0xF4 : wifipll_test */
    union {
        struct {
            uint32_t wifipll_test_data_mux          :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t wifipll_test_start_mux         :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t wifipll_test_en                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t dten_wifipll_refclk            :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t dten_wifipll_postdiv           :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t dten_wifipll_fbdiv_fsdm        :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t dten_wifipll_fbdiv_ffb         :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t dten_wifipll_clk32m            :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t ten_wifipll                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_test;

    /* 0xF8 : wifipll_readback */
    union {
        struct {
            uint32_t reserved_0_8                   :  9; /* [ 8: 0],       rsvd,        0x0 */
            uint32_t wifipll_test_start             :  1; /* [    9],          r,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t wifipll_vctrl_out_range        :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t wifipll_soc_mom_update_total_ou :  2; /* [17:16],          r,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t wifipll_lo_unlock              :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t wifipll_lo_lock                :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t wifipll_fast_inc_fcal_ovflow   :  1; /* [   28],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_readback;

    /* 0xFC : wifipll_adpll_test_out */
    union {
        struct {
            uint32_t wifipll_test_out               : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } wifipll_adpll_test_out;

    /* 0x100 : lo_vco */
    union {
        struct {
            uint32_t vco_vbias_cw                   :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t vco_idac_cw                    :  7; /* [14: 8],        r/w,       0x40 */
            uint32_t reserved_15_19                 :  5; /* [19:15],       rsvd,        0x0 */
            uint32_t vco_idac_cw_hw                 :  7; /* [26:20],          r,       0x40 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_vco;

    /* 0x104 : lo_vco1 */
    union {
        struct {
            uint32_t vco_acal_ud                    :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t vco_acal_vref_cw               :  4; /* [ 7: 4],        r/w,        0x7 */
            uint32_t vco_acal_en                    :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t vco_acal_mode                  :  2; /* [11:10],        r/w,        0x2 */
            uint32_t vco_inc_acal_en                :  1; /* [   12],        r/w,        0x0 */
            uint32_t vco_inc_acal_en_hw             :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t vco_lodistbuf_boost            :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t vco_pud_gated_en               :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t vco_vbias_flt_short            :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t vco_idac_flt_short             :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_vco1;

    /* 0x108 : lo_dtc */
    union {
        struct {
            uint32_t adpll_dtc_r                    :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t adpll_dtc_r_hw                 :  2; /* [ 3: 2],          r,        0x2 */
            uint32_t adpll_dtc_flt_sht              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t adpll_dtc_clk_inv_en           :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t adpll_dtc_pud_gated_en         :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adpll_dtc_lp_mode_en           :  1; /* [   16],        r/w,        0x0 */
            uint32_t adpll_dtc_lp_mode_en_hw        :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t adpll_dtc_bypass               :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_dtc;

    /* 0x10C : osmx */
    union {
        struct {
            uint32_t osmx_div2_rload_sel            :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t osmx_buf2g_rload_sel           :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t osmx_comp_en                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t osmx_cap_sel                   :  5; /* [16:12],        r/w,       0x10 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t osmx_bm_div2                   :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t osmx_bm_core                   :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } osmx;

    /* 0x110 : osmx1 */
    union {
        struct {
            uint32_t osmx_vbias_lo                  :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t osmx_vbias_cap                 :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t osmx_supply_boost              :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t osmx_pu_pkdet                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t osmx_comp_out                  :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t osmx_pkdet_vref_band           :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t osmx_pkdet_vref                :  4; /* [23:20],        r/w,        0x4 */
            uint32_t osmx_gm_boost                  :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t osmx_en_bias                   :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } osmx1;

    /* 0x114 : lobuf */
    union {
        struct {
            uint32_t txbuf_5g_boost                 :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t txbuf_2g_boost                 :  2; /* [ 3: 2],        r/w,        0x2 */
            uint32_t rxbuf_5g_boost                 :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t rxbuf_2g_boost                 :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lobuf_en_txbuf                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t lobuf_en_txbuf_hw              :  1; /* [    9],          r,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lobuf_en_rxbuf                 :  1; /* [   12],        r/w,        0x0 */
            uint32_t lobuf_en_rxbuf_hw              :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lobuf_en_iqdiv_tx              :  1; /* [   16],        r/w,        0x0 */
            uint32_t lobuf_en_iqdiv_tx_hw           :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lobuf_en_iqdiv_rx              :  1; /* [   20],        r/w,        0x0 */
            uint32_t lobuf_en_iqdiv_rx_hw           :  1; /* [   21],          r,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lobuf_en_5g                    :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t lobuf_en_2g                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t lobuf_en_dpd                   :  1; /* [   29],        r/w,        0x0 */
            uint32_t lobuf_en_iqdiv_boost_rx        :  1; /* [   30],        r/w,        0x0 */
            uint32_t lobuf_vg10_sel                 :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lobuf;

    /* 0x118 : lpvco */
    union {
        struct {
            uint32_t lpvco_vbias_cw                 :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t lpvco_sht_vbias_flt            :  1; /* [    3],        r/w,        0x0 */
            uint32_t lpvco_sht_idac_flt             :  1; /* [    4],        r/w,        0x0 */
            uint32_t lpvco_inc_acal_en              :  1; /* [    5],        r/w,        0x0 */
            uint32_t lpvco_idac_cw_hw               :  6; /* [11: 6],          r,       0x20 */
            uint32_t lpvco_idac_cw                  :  6; /* [17:12],        r/w,       0x20 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lpvco_comp_start               :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lpvco_acal_vref_cw             :  4; /* [27:24],        r/w,        0x7 */
            uint32_t lpvco_acal_en                  :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t lpvco_acal_ud                  :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } lpvco;

    /* 0x11C : lo_resv */
    union {
        struct {
            uint32_t lo_resv0                       :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t lo_resv1                       :  8; /* [15: 8],        r/w,       0xff */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_resv;

    /* 0x120 : lo_fbdiv */
    union {
        struct {
            uint32_t fbdiv_half_step_en             :  1; /* [    0],        r/w,        0x0 */
            uint32_t fbdiv_half_step_en_hw          :  1; /* [    1],          r,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t fbdiv_resample_en              :  1; /* [    4],        r/w,        0x1 */
            uint32_t fbdiv_resample_en_hw           :  1; /* [    5],          r,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t fbdiv_sel_pclk                 :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t fbdiv_sel_pclk_hw              :  1; /* [   10],          r,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t fbdiv_sel_ffb                  :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t fbdiv_sel_ffb_hw               :  1; /* [   14],          r,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t fbdiv_sel_adpll_clk            :  1; /* [   16],        r/w,        0x1 */
            uint32_t fbdiv_sel_adpll_clk_hw         :  1; /* [   17],          r,        0x1 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_fbdiv;

    /* 0x124 : lo_tdc */
    union {
        struct {
            uint32_t tdc_slpgen_clk_latch           :  1; /* [    0],        r/w,        0x1 */
            uint32_t tdc_slpgen_clk_inv             :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t tdc_slpgen_vcmsel              :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t tdc_slpgen_csel                :  2; /* [ 7: 6],        r/w,        0x2 */
            uint32_t tdc_adc_vref_sel               :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t tdc_adc_vbuf_sel               :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t tdc_adc_sign_sel               :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t tdc_adc_gt_rm                  :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t tdc_adc_dly_ctrl               :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t tdc_adc_clk_inv                :  1; /* [   28],        r/w,        0x0 */
            uint32_t tdc_adc_clk_en                 :  1; /* [   29],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_tdc;

    /* 0x128 : lo_dcomash */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t dcomash_gain_comp_en           :  1; /* [    4],        r/w,        0x1 */
            uint32_t dcomash_gain_comp_en_hw        :  1; /* [    5],          r,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t dcomash_bypass                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t dcomash_ndiv_dly               :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_dcomash;

    /* 0x12C : lo_adpll_top */
    union {
        struct {
            uint32_t adpll_fast_inc_fcal_lms_en     :  1; /* [    0],        r/w,        0x1 */
            uint32_t adpll_fast_inc_fcal_en         :  1; /* [    1],        r/w,        0x0 */
            uint32_t adpll_fast_inc_fcal_en_hw      :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t adpll_fast_inc_dvar_comp_val   :  5; /* [ 8: 4],        r/w,       0x14 */
            uint32_t adpll_fast_inc_dvar_comp_en    :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_force_lf_fast_mode       :  1; /* [   12],        r/w,        0x0 */
            uint32_t adpll_force_lf_fast_mode_hw    :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t adpll_fsm_bypass               :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adpll_fsm_en                   :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t adpll_en                       :  1; /* [   24],        r/w,        0x1 */
            uint32_t rstb_adpll                     :  1; /* [   25],        r/w,        0x1 */
            uint32_t rstb_adpll_hw                  :  1; /* [   26],          r,        0x1 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t adpll_fref_div2_en             :  1; /* [   28],        r/w,        0x0 */
            uint32_t adpll_fref_div2_en_hw          :  1; /* [   29],          r,        0x0 */
            uint32_t adpll_lp_mode_en               :  1; /* [   30],        r/w,        0x0 */
            uint32_t adpll_lp_mode_en_hw            :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_top;

    /* 0x130 : lo_adpll_fsm */
    union {
        struct {
            uint32_t adpll_timeout_cnt              :  1; /* [    0],        r/w,        0x0 */
            uint32_t adpll_timeout_cnt1             :  1; /* [    1],        r/w,        0x0 */
            uint32_t adpll_lo_unlock_intrpt_clear_sel :  1; /* [    2],        r/w,        0x1 */
            uint32_t adpll_lo_unlock_intrpt_clear   :  1; /* [    3],        r/w,        0x0 */
            uint32_t adpll_lo_open                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t adpll_lo_lock_sel              :  1; /* [    5],        r/w,        0x0 */
            uint32_t adpll_inc_fcal_en              :  1; /* [    6],        r/w,        0x0 */
            uint32_t adpll_lock_fail_en             :  1; /* [    7],        r/w,        0x0 */
            uint32_t adpll_abnormal_dealed          :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t adpll_ext_vctrl_det_start      :  1; /* [   12],        r/w,        0x0 */
            uint32_t adpll_ext_vctrl_det_en         :  1; /* [   13],        r/w,        0x0 */
            uint32_t adpll_ext_rst_spd_det          :  1; /* [   14],        r/w,        0x0 */
            uint32_t adpll_ext_rst_coarse_det       :  1; /* [   15],        r/w,        0x0 */
            uint32_t adpll_ext_momhold_lmsenb       :  1; /* [   16],        r/w,        0x0 */
            uint32_t adpll_ext_mom_update_en        :  1; /* [   17],        r/w,        0x0 */
            uint32_t adpll_ext_mom_search_en        :  1; /* [   18],        r/w,        0x0 */
            uint32_t adpll_ext_loop_lock            :  1; /* [   19],        r/w,        0x0 */
            uint32_t adpll_ext_lo_fsm_en            :  1; /* [   20],        r/w,        0x0 */
            uint32_t adpll_ext_freqerr_det_start    :  1; /* [   21],        r/w,        0x0 */
            uint32_t adpll_ext_fcal_start           :  1; /* [   22],        r/w,        0x0 */
            uint32_t adpll_ext_fcal_done            :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_fsm;

    /* 0x134 : lo_adpll_fcal */
    union {
        struct {
            uint32_t adpll_fcal_polarity            :  1; /* [    0],        r/w,        0x0 */
            uint32_t adpll_fcal_phaerr_limit        :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_fcal_momcap_ext          :  9; /* [12: 4],        r/w,      0x100 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adpll_fcal_mom_toggle_cnt      :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adpll_fcal_mode                :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_fcal_ndiv_adj_en         :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t adpll_fcal_threshold           :  2; /* [29:28],        r/w,        0x2 */
            uint32_t adpll_fcal_clk_period          :  2; /* [31:30],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } lo_adpll_fcal;

    /* 0x138 : lo_adpll_spd */
    union {
        struct {
            uint32_t adpll_spd_threshold            :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t adpll_spd_sign_det_en          :  1; /* [    2],        r/w,        0x1 */
            uint32_t adpll_spd_sign_det_cnt         :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t adpll_spd_outrange_gate_sel    :  1; /* [    5],        r/w,        0x0 */
            uint32_t adpll_spd_outrange_dly         :  2; /* [ 7: 6],        r/w,        0x2 */
            uint32_t adpll_spd_offset_polar         :  1; /* [    8],        r/w,        0x0 */
            uint32_t adpll_spd_offset_mag           :  2; /* [10: 9],        r/w,        0x0 */
            uint32_t adpll_spd_lms_sstp_win         :  1; /* [   11],        r/w,        0x0 */
            uint32_t adpll_spd_inrange_dly1         :  2; /* [13:12],        r/w,        0x0 */
            uint32_t adpll_spd_inrange_dly          :  2; /* [15:14],        r/w,        0x2 */
            uint32_t adpll_spd_inrange_cons         :  2; /* [17:16],        r/w,        0x3 */
            uint32_t adpll_spd_gain                 :  2; /* [19:18],        r/w,        0x1 */
            uint32_t adpll_coarse_phaerr_dly        :  1; /* [   20],        r/w,        0x0 */
            uint32_t adpll_coarse_phaerr_en         :  1; /* [   21],        r/w,        0x1 */
            uint32_t adpll_coarse_path_turnoff      :  2; /* [23:22],        r/w,        0x2 */
            uint32_t adpll_coarse_path_offtime      :  1; /* [   24],        r/w,        0x1 */
            uint32_t adpll_coarse_path_force_on     :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t adpll_coarse_inrange_cons      :  2; /* [29:28],        r/w,        0x2 */
            uint32_t adpll_coarse_gain              :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lo_adpll_spd;

    /* 0x13C : lo_adpll_lpf */
    union {
        struct {
            uint32_t adpll_lf_fp3                   :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast             :  1; /* [    4],        r/w,        0x1 */
            uint32_t adpll_lf_beta_exp              :  3; /* [ 7: 5],        r/w,        0x3 */
            uint32_t adpll_lf_beta_base             :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_lf_avg_en                :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast            :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp             :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_base            :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_lpf;

    /* 0x140 : lo_adpll_lpf_tx */
    union {
        struct {
            uint32_t adpll_lf_fp3_tx                :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast_tx          :  1; /* [    4],        r/w,        0x1 */
            uint32_t adpll_lf_beta_exp_tx           :  3; /* [ 7: 5],        r/w,        0x3 */
            uint32_t adpll_lf_beta_base_tx          :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast_tx         :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp_tx          :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_base_tx         :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_lpf_tx;

    /* 0x144 : lo_adpll_lpf_rx */
    union {
        struct {
            uint32_t adpll_lf_fp3_rx                :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast_rx          :  1; /* [    4],        r/w,        0x1 */
            uint32_t adpll_lf_beta_exp_rx           :  3; /* [ 7: 5],        r/w,        0x3 */
            uint32_t adpll_lf_beta_base_rx          :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast_rx         :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp_rx          :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_base_rx         :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_lpf_rx;

    /* 0x148 : lo_adpll_lpf_hw */
    union {
        struct {
            uint32_t adpll_lf_fp3_hw                :  2; /* [ 1: 0],          r,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast_hw          :  1; /* [    4],          r,        0x1 */
            uint32_t adpll_lf_beta_exp_hw           :  3; /* [ 7: 5],          r,        0x3 */
            uint32_t adpll_lf_beta_base_hw          :  2; /* [ 9: 8],          r,        0x3 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast_hw         :  2; /* [17:16],          r,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp_hw          :  3; /* [22:20],          r,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_base_hw         :  1; /* [   24],          r,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_lpf_hw;

    /* 0x14C : lo_adpll_vctrl */
    union {
        struct {
            uint32_t adpll_mom_update_period        :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t adpll_mom_force_hold           :  1; /* [    2],        r/w,        0x0 */
            uint32_t adpll_mom_code_bypass          :  1; /* [    3],        r/w,        0x0 */
            uint32_t adpll_vctrl_range_ext_en       :  1; /* [    4],        r/w,        0x0 */
            uint32_t adpll_vctrl_range_ext          :  2; /* [ 6: 5],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t adpll_vctrl_moni_win           :  1; /* [    8],        r/w,        0x0 */
            uint32_t adpll_vctrl_lock_win           :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_vctrl_det_cons_en        :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adpll_lp_polarity              :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adpll_lp_mom_polarity          :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t adpll_lf_msb_ext               :  7; /* [30:24],        r/w,       0x40 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_vctrl;

    /* 0x150 : lo_adpll_sdm */
    union {
        struct {
            uint32_t adpll_sdm_in_hw                : 30; /* [29: 0],          r, 0x16800000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_sdm;

    /* 0x154 : lo_adpll_sdm1 */
    union {
        struct {
            uint32_t adpll_sdm_in                   : 30; /* [29: 0],        r/w, 0x16800000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_sdm1;

    /* 0x158 : lo_adpll_sdm2 */
    union {
        struct {
            uint32_t adpll_sdm_out_dly              :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_sdm_sig_dith             :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t adpll_sdm_order                :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t adpll_sdm_noi_dith             :  2; /* [13:12],        r/w,        0x3 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t adpll_sdm_dith_prbs_en         :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adpll_sdm_dith_force_en        :  1; /* [   20],        r/w,        0x0 */
            uint32_t adpll_sdm_dith_force_en_hw     :  1; /* [   21],          r,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_sdm_dith_en              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t adpll_sdm_bypass               :  1; /* [   28],        r/w,        0x0 */
            uint32_t adpll_sdm_bypass_hw            :  1; /* [   29],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_sdm2;

    /* 0x15C : lo_adpll_lms */
    union {
        struct {
            uint32_t adpll_pha_prbs_mode            :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t adpll_pha_msb_err              :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t adpll_pha_msb_err_prbs_en      :  1; /* [    4],        r/w,        0x0 */
            uint32_t adpll_pha_dem_en               :  1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t adpll_pha_cancel_en            :  1; /* [    8],        r/w,        0x1 */
            uint32_t adpll_pha_cancel_dly           :  2; /* [10: 9],        r/w,        0x0 */
            uint32_t adpll_lms_step_enlarge         :  1; /* [   11],        r/w,        0x1 */
            uint32_t adpll_lms_step                 :  2; /* [13:12],        r/w,        0x1 */
            uint32_t adpll_lms_step_hw              :  2; /* [15:14],          r,        0x1 */
            uint32_t adpll_lms_q_dly                :  2; /* [17:16],        r/w,        0x1 */
            uint32_t adpll_lms_polarity             :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t adpll_lms_ext_value            :  9; /* [28:20],        r/w,      0x11c */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t adpll_lms_ext_en               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_lms;

    /* 0x160 : lo_adpll_test */
    union {
        struct {
            uint32_t adpll_test_data_mux            :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_test_start_mux           :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t adpll_test_en                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_19                  : 11; /* [19: 9],       rsvd,        0x0 */
            uint32_t adpll_lms_ext_value_hw         :  9; /* [28:20],          r,      0x11c */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_test;

    /* 0x164 : lo_adpll_resv */
    union {
        struct {
            uint32_t adpll_resv1                    : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t adpll_resv0                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_resv;

    /* 0x168 : lo_adpll_output */
    union {
        struct {
            uint32_t adpll_unlock_intrpt            :  1; /* [    0],          r,        0x0 */
            uint32_t adpll_lo_lock                  :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_fsm_state                :  4; /* [ 7: 4],          r,        0x0 */
            uint32_t adpll_soc_vctrl_out_range      :  1; /* [    8],          r,        0x0 */
            uint32_t adpll_soc_vctrl_det_done       :  1; /* [    9],          r,        0x0 */
            uint32_t adpll_soc_spd_unlock_sign      :  1; /* [   10],          r,        0x0 */
            uint32_t adpll_soc_spd_unlock           :  1; /* [   11],          r,        0x0 */
            uint32_t adpll_soc_spd_lock             :  1; /* [   12],          r,        0x0 */
            uint32_t adpll_soc_mom_update_ou        :  1; /* [   13],          r,        0x0 */
            uint32_t adpll_soc_mom_total_ou         :  2; /* [15:14],          r,        0x0 */
            uint32_t adpll_soc_freqerr_sign         :  1; /* [   16],          r,        0x0 */
            uint32_t adpll_soc_freqerr_ou           :  1; /* [   17],          r,        0x0 */
            uint32_t adpll_soc_freqerr_det_done     :  1; /* [   18],          r,        0x0 */
            uint32_t adpll_soc_fcal_done            :  1; /* [   19],          r,        0x0 */
            uint32_t adpll_soc_fast_inc_fcal_ou     :  1; /* [   20],          r,        0x0 */
            uint32_t adpll_soc_capcode_ud           :  1; /* [   21],          r,        0x0 */
            uint32_t adpll_soc_capcode_out_range    :  1; /* [   22],          r,        0x0 */
            uint32_t reserved_23_27                 :  5; /* [27:23],       rsvd,        0x0 */
            uint32_t adpll_test_start               :  1; /* [   28],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_output;

    /* 0x16C : lo_adpll_testout */
    union {
        struct {
            uint32_t adpll_test_out                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_adpll_testout;

    /* 0x170 : lo0_adpll_sdm_hw1 */
    union {
        struct {
            uint32_t adpll_sdm_frac_th              : 22; /* [21: 0],        r/w,     0x1fe0 */
            uint32_t reserved_22_25                 :  4; /* [25:22],       rsvd,        0x0 */
            uint32_t adpll_sdm_bypass_th            :  6; /* [31:26],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } lo0_adpll_sdm_hw1;

    /* 0x174  reserved */
    uint8_t RESERVED0x174[76];

    /* 0x1C0 : top_dly_ctrl */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t pud_dly_vco                    :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t pud_dly_dtc                    :  3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t pud_dly_rpt                    :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pud_dly_pa                     :  3; /* [18:16],        r/w,        0x1 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t pud_dly_lpvco                  :  3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } top_dly_ctrl;

    /* 0x1C4 : lo_dly_ctrl */
    union {
        struct {
            uint32_t reserved_0_15                  : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t lo_lock_check_dly              :  2; /* [17:16],        r/w,        0x1 */
            uint32_t inc_fcal_window                :  2; /* [19:18],        r/w,        0x1 */
            uint32_t fast_inc_fcal_window           :  2; /* [21:20],        r/w,        0x1 */
            uint32_t rstb_fbdiv_window              :  2; /* [23:22],        r/w,        0x1 */
            uint32_t rstb_adpll_window              :  2; /* [25:24],        r/w,        0x1 */
            uint32_t rstb_dly                       :  2; /* [27:26],        r/w,        0x1 */
            uint32_t rpt_sync_dly1                  :  2; /* [29:28],        r/w,        0x1 */
            uint32_t rpt_sync_dly                   :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lo_dly_ctrl;

    /* 0x1C8 : ss0_rx_gain */
    union {
        struct {
            uint32_t ss0_gc_rbb2                    :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1                    :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_5g_load_att         :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_gc_rmxgm_5g                :  2; /* [13:12],        r/w,        0x3 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_5g                  :  3; /* [18:16],        r/w,        0x7 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_2g_load_att         :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_gc_rmxgm_2g                :  2; /* [25:24],        r/w,        0x3 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_2g                  :  3; /* [30:28],        r/w,        0x7 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rx_gain;

    /* 0x1CC : ss0_rx_gain_hw */
    union {
        struct {
            uint32_t ss0_gc_rbb2_hw                 :  3; /* [ 2: 0],          r,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_hw                 :  2; /* [ 5: 4],          r,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_5g_load_att_hw      :  3; /* [10: 8],          r,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_gc_rmxgm_5g_hw             :  2; /* [13:12],          r,        0x3 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_5g_hw               :  3; /* [18:16],          r,        0x7 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_2g_load_att_hw      :  3; /* [22:20],          r,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_gc_rmxgm_2g_hw             :  2; /* [25:24],          r,        0x3 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_gc_lna_2g_hw               :  3; /* [30:28],          r,        0x7 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rx_gain_hw;

    /* 0x1D0 : ss0_tx_gain0 */
    union {
        struct {
            uint32_t ss0_da_2g_cs                   :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t ss0_tmx_2g_cs                  :  5; /* [ 8: 4],        r/w,        0x8 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t ss0_gc_pa_2g                   :  2; /* [11:10],        r/w,        0x3 */
            uint32_t reserved_12_19                 :  8; /* [19:12],       rsvd,        0x0 */
            uint32_t ss0_gc_tmx_2g                  :  4; /* [23:20],        r/w,        0xe */
            uint32_t ss0_gc_tbb                     :  6; /* [29:24],        r/w,        0x9 */
            uint32_t ss0_gc_tbb_boost               :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_tx_gain0;

    /* 0x1D4 : ss0_tx_gain1 */
    union {
        struct {
            uint32_t ss0_da_5g_cs                   :  5; /* [ 4: 0],        r/w,        0x8 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_tmx_5g_cs                  :  5; /* [12: 8],        r/w,        0x8 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_gc_pa_5g                   :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_gc_da_5g_bld               :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_gc_da_5g                   :  3; /* [26:24],        r/w,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t ss0_gc_tmx_5g                  :  4; /* [31:28],        r/w,        0xe */
        }BF;
        uint32_t WORD;
    } ss0_tx_gain1;

    /* 0x1D8 : ss0_tx_gain0_hw */
    union {
        struct {
            uint32_t ss0_da_2g_cs_hw                :  4; /* [ 3: 0],          r,        0x6 */
            uint32_t ss0_tmx_2g_cs_hw               :  5; /* [ 8: 4],          r,        0x8 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t ss0_gc_pa_2g_hw                :  2; /* [11:10],          r,        0x3 */
            uint32_t ss0_dac_bias_sel_hw            :  2; /* [13:12],          r,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_tbb_bm_cg_hw               :  3; /* [18:16],          r,        0x4 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_gc_tmx_2g_hw               :  4; /* [23:20],          r,        0xe */
            uint32_t ss0_gc_tbb_hw                  :  6; /* [29:24],          r,        0x9 */
            uint32_t ss0_gc_tbb_boost_hw            :  2; /* [31:30],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_tx_gain0_hw;

    /* 0x1DC : ss0_tx_gain1_hw */
    union {
        struct {
            uint32_t ss0_da_5g_cs_hw                :  5; /* [ 4: 0],          r,        0x8 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_tmx_5g_cs_hw               :  5; /* [12: 8],          r,        0x8 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_gc_pa_5g_hw                :  2; /* [17:16],          r,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_gc_da_5g_bld_hw            :  2; /* [21:20],          r,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_gc_da_5g_hw                :  3; /* [26:24],          r,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t ss0_gc_tmx_5g_hw               :  4; /* [31:28],          r,        0xe */
        }BF;
        uint32_t WORD;
    } ss0_tx_gain1_hw;

    /* 0x1E0 : wf_tmp_reg0 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t ss0_rxsw_2g_en_hw              :  2; /* [ 9: 8],          r,        0x0 */
            uint32_t ss0_rxsw_5g_en_hw              :  2; /* [11:10],          r,        0x0 */
            uint32_t lobuf_en_testmode              :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wf_tmp_reg0;

    /* 0x1e4  reserved */
    uint8_t RESERVED0x1e4[28];

    /* 0x200 : ss0_pa_2g_0 */
    union {
        struct {
            uint32_t ss0_pa_2g_vbcas2               :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_op_vref              :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t ss0_pa_2g_lz_bias_en           :  1; /* [    7],        r/w,        0x1 */
            uint32_t ss0_pa_2g_op_cc                :  3; /* [10: 8],        r/w,        0x5 */
            uint32_t ss0_pa_2g_cn_sel               :  1; /* [   11],        r/w,        0x0 */
            uint32_t ss0_pa_2g_ibc_op               :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_gc_att               :  3; /* [18:16],        r/w,        0x7 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_cout_sel             :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_2g_0;

    /* 0x204 : ss0_pa_2g_1 */
    union {
        struct {
            uint32_t ss0_pa_2g_vbcas1               :  4; /* [ 3: 0],        r/w,        0xa */
            uint32_t ss0_pa_2g_iaq                  :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_iet                  :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_vbcore               :  5; /* [20:16],        r/w,       0x11 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_vbpmos               :  4; /* [27:24],        r/w,        0x7 */
            uint32_t ss0_pa_2g_bm_cas2_bulk         :  3; /* [30:28],        r/w,        0x7 */
            uint32_t ss0_pa_2g_cas2_bulk_pulldown   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_2g_1;

    /* 0x208 : ss0_pa_2g_3 */
    union {
        struct {
            uint32_t ss0_pa_2g_vbcas1_hw            :  4; /* [ 3: 0],          r,        0xa */
            uint32_t ss0_pa_2g_iaq_hw               :  3; /* [ 6: 4],          r,        0x2 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_iet_hw               :  5; /* [12: 8],          r,        0xb */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_vbcore_hw            :  5; /* [20:16],          r,       0x11 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_pa_2g_vbpmos_hw            :  4; /* [27:24],          r,        0x7 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_2g_3;

    /* 0x20C : ss0_da_2g_0 */
    union {
        struct {
            uint32_t ss0_da_2g_vbcore_aux           :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t ss0_da_2g_vbcore_aux_en        :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_da_2g_vbcore               :  4; /* [11: 8],        r/w,        0xc */
            uint32_t ss0_da_2g_vbcas2               :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_da_2g_vbcas1               :  4; /* [19:16],        r/w,        0x7 */
            uint32_t ss0_da_2g_cs_l                 :  4; /* [23:20],        r/w,        0xa */
            uint32_t ss0_da_2g_cs_h                 :  4; /* [27:24],        r/w,        0x3 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_da_2g_0;

    /* 0x210 : ss0_tmx_2g */
    union {
        struct {
            uint32_t ss0_tmx_2g_vbsw                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_tmx_2g_vbcas               :  3; /* [ 6: 4],        r/w,        0x5 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_trf_2g_local_rcal_code     :  5; /* [12: 8],        r/w,        0xf */
            uint32_t ss0_trf_2g_local_ptat_en       :  1; /* [   13],        r/w,        0x0 */
            uint32_t ss0_trf_2g_local_ptat_mode_sel :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_da_2g_local_ptat_scale     :  4; /* [19:16],        r/w,        0x7 */
            uint32_t ss0_pa_2g_local_ptat_scale     :  4; /* [23:20],        r/w,        0x7 */
            uint32_t ss0_tmx_2g_deq                 :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t ss0_da_2g_deq                  :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_tmx_2g;

    /* 0x214 : ss0_selfmx_2g */
    union {
        struct {
            uint32_t ss0_gc_selfmx_2g               :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t ss0_gc_selfmx_2g_boost         :  1; /* [    3],        r/w,        0x0 */
            uint32_t ss0_selfmx_2g_osdac_bm         :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_selfmx_2g_bm               :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_selfmx_2g_osdac            :  6; /* [17:12],        r/w,       0x1f */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_da_2g_vbpmos               :  4; /* [23:20],        r/w,        0x7 */
            uint32_t ss0_da_2g_vbpmos_hw            :  4; /* [27:24],          r,        0x7 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_selfmx_2g;

    /* 0x218 : ss0_pwrdet_2g_0 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_2g_mul_short_en     :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_gc_pwrdet_2g_mul_att       :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_gc_pwrdet_2g_att           :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_2g_osdac_code       :  5; /* [16:12],        r/w,        0xf */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_2g_mul_bm           :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pwrdet_2g_0;

    /* 0x21C : ss0_pwrdet_2g_1 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_2g_mul_short_en_hw  :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pwrdet_2g_1;

    /* 0x220 : ss0_pwrdet_2g_2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pwrdet_2g_2;

    /* 0x224 : ss0_pa_5g_0 */
    union {
        struct {
            uint32_t ss0_pa_5g_vbcas2               :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_op_vref              :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t ss0_pa_5g_lz_bias_en           :  1; /* [    7],        r/w,        0x1 */
            uint32_t ss0_pa_5g_op_cc                :  3; /* [10: 8],        r/w,        0x5 */
            uint32_t ss0_pa_5g_cn_sel               :  1; /* [   11],        r/w,        0x0 */
            uint32_t ss0_pa_5g_ibc_op               :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_gc_att               :  3; /* [18:16],        r/w,        0x7 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_cout_sel             :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_cd_sel               :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_5g_0;

    /* 0x228 : ss0_pa_5g_1 */
    union {
        struct {
            uint32_t ss0_pa_5g_vbcas1               :  4; /* [ 3: 0],        r/w,        0xa */
            uint32_t ss0_pa_5g_iaq                  :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_iet                  :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_vbcore               :  5; /* [20:16],        r/w,       0x11 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_vbpmos               :  4; /* [27:24],        r/w,        0x7 */
            uint32_t ss0_pa_5g_bm_cas2_bulk         :  3; /* [30:28],        r/w,        0x7 */
            uint32_t ss0_pa_5g_cas2_bulk_pulldown   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_5g_1;

    /* 0x22C : ss0_pa_5g_3 */
    union {
        struct {
            uint32_t ss0_pa_5g_vbcas1_hw            :  4; /* [ 3: 0],          r,        0xa */
            uint32_t ss0_pa_5g_iaq_hw               :  3; /* [ 6: 4],          r,        0x2 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_iet_hw               :  5; /* [12: 8],          r,        0xb */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_vbcore_hw            :  5; /* [20:16],          r,       0x11 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_pa_5g_vbpmos_hw            :  4; /* [27:24],          r,        0x7 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_5g_3;

    /* 0x230 : ss0_da_5g_0 */
    union {
        struct {
            uint32_t ss0_da_5g_vbcore_aux           :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t ss0_da_5g_vbcore_aux_en        :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_da_5g_vbcore               :  4; /* [11: 8],        r/w,        0xc */
            uint32_t ss0_da_5g_vbcas2               :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_da_5g_vbcas1               :  4; /* [19:16],        r/w,        0x7 */
            uint32_t ss0_da_5g_cs_l                 :  5; /* [24:20],        r/w,        0xa */
            uint32_t reserved_25                    :  1; /* [   25],       rsvd,        0x0 */
            uint32_t ss0_da_5g_cs_h                 :  5; /* [30:26],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_da_5g_0;

    /* 0x234 : ss0_tmx_5g */
    union {
        struct {
            uint32_t ss0_tmx_5g_vbsw                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_tmx_5g_vbcas               :  3; /* [ 6: 4],        r/w,        0x5 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_trf_5g_local_rcal_code     :  5; /* [12: 8],        r/w,        0xf */
            uint32_t ss0_trf_5g_local_ptat_en       :  1; /* [   13],        r/w,        0x0 */
            uint32_t ss0_trf_5g_local_ptat_mode_sel :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_da_5g_local_ptat_scale     :  4; /* [19:16],        r/w,        0x7 */
            uint32_t ss0_pa_5g_local_ptat_scale     :  4; /* [23:20],        r/w,        0x7 */
            uint32_t ss0_tmx_5g_deq                 :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t ss0_da_5g_deq                  :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_tmx_5g;

    /* 0x238 : ss0_selfmx_5g */
    union {
        struct {
            uint32_t ss0_gc_selfmx_5g               :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t ss0_gc_selfmx_5g_boost         :  1; /* [    3],        r/w,        0x0 */
            uint32_t ss0_selfmx_5g_osdac_bm         :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_selfmx_5g_bm               :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_selfmx_5g_osdac            :  6; /* [17:12],        r/w,       0x1f */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_da_5g_vbpmos               :  4; /* [23:20],        r/w,        0x7 */
            uint32_t ss0_da_5g_vbpmos_hw            :  4; /* [27:24],          r,        0x7 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_selfmx_5g;

    /* 0x23C : ss0_pwrdet_5g_0 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_pr_en            :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t ss0_gc_pwrdet_5g_pr            :  1; /* [    3],        r/w,        0x0 */
            uint32_t ss0_gc_pwrdet_5g_mul_att       :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_gc_pwrdet_5g_att           :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_osdac_code       :  5; /* [16:12],        r/w,        0xf */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_mul_bm           :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pwrdet_5g_0;

    /* 0x240 : ss0_pwrdet_5g_1 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_pr_en_hw         :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_11                  : 10; /* [11: 2],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_pr_swq           :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_pr_swi           :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_pwrdet_5g_pr_bm            :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pwrdet_5g_1;

    /* 0x244 : ss0_pwrdet_5g_2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pwrdet_5g_2;

    /* 0x248 : ss0_tbb */
    union {
        struct {
            uint32_t ss0_tbb_atest_res_sel          :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t ss0_tbb_atest_in_en            :  1; /* [    2],        r/w,        0x0 */
            uint32_t ss0_tbb_iq_bias_short          :  1; /* [    3],        r/w,        0x1 */
            uint32_t ss0_tbb_vcm                    :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t ss0_tbb_cflt                   :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t ss0_tbb_bm_sf                  :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_tosdac_q                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t ss0_tosdac_i                   :  6; /* [27:22],        r/w,       0x20 */
            uint32_t ss0_tbb_atest_out_en           :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_tbb;

    /* 0x24C : Digital Control */
    union {
        struct {
            uint32_t ss0_dac_dvdd_sel               :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_dac_bias_enhance           :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_dac_bias_sel               :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_dac_rccalsel               :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_dac_clk_sync_inv           :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t ss0_dac_rccal_swap_en          :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_dac_vlow_sel               :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_dac_nsink_sel              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_dac_0;

    /* 0x250  reserved */
    uint8_t RESERVED0x250[4];

    /* 0x254 : ss0_pa_config_force */
    union {
        struct {
            uint32_t ss0_wifi_mode_ind              :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t ss0_wifi_mode_ind_force_en     :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31                  : 29; /* [31: 3],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_config_force;

    /* 0x258 : ss0_2g_pa_config_b0 */
    union {
        struct {
            uint32_t ss0_2g_pa_vbcore_b0            :  5; /* [ 4: 0],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_b1            :  5; /* [ 9: 5],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_b2            :  5; /* [14:10],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_b3            :  5; /* [19:15],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcas1_b             :  4; /* [23:20],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_pa_config_b0;

    /* 0x25C : ss0_2g_pa_config_b1 */
    union {
        struct {
            uint32_t ss0_2g_pa_iet_b0               :  5; /* [ 4: 0],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_b1               :  5; /* [ 9: 5],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_b2               :  5; /* [14:10],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_b3               :  5; /* [19:15],        r/w,        0xb */
            uint32_t ss0_2g_pa_iaq_b                :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_pa_config_b1;

    /* 0x260 : ss0_2g_pa_config_gn0 */
    union {
        struct {
            uint32_t ss0_2g_pa_vbcore_gn0           :  5; /* [ 4: 0],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_gn1           :  5; /* [ 9: 5],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_gn2           :  5; /* [14:10],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_gn3           :  5; /* [19:15],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcas1_gn            :  4; /* [23:20],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_pa_config_gn0;

    /* 0x264 : ss0_2g_pa_config_gn1 */
    union {
        struct {
            uint32_t ss0_2g_pa_iet_gn0              :  5; /* [ 4: 0],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_gn1              :  5; /* [ 9: 5],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_gn2              :  5; /* [14:10],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_gn3              :  5; /* [19:15],        r/w,        0xb */
            uint32_t ss0_2g_pa_iaq_gn               :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_pa_config_gn1;

    /* 0x268 : ss0_2g_pa_config_ax0 */
    union {
        struct {
            uint32_t ss0_2g_pa_vbcore_ax0           :  5; /* [ 4: 0],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_ax1           :  5; /* [ 9: 5],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_ax2           :  5; /* [14:10],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcore_ax3           :  5; /* [19:15],        r/w,       0x11 */
            uint32_t ss0_2g_pa_vbcas1_ax            :  4; /* [23:20],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_pa_config_ax0;

    /* 0x26C : ss0_2g_pa_config_ax1 */
    union {
        struct {
            uint32_t ss0_2g_pa_iet_ax0              :  5; /* [ 4: 0],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_ax1              :  5; /* [ 9: 5],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_ax2              :  5; /* [14:10],        r/w,        0xb */
            uint32_t ss0_2g_pa_iet_ax3              :  5; /* [19:15],        r/w,        0xb */
            uint32_t ss0_2g_pa_iaq_ax               :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_pa_config_ax1;

    /* 0x270 : ss0_pa_config_bz */
    union {
        struct {
            uint32_t ss0_pa_iet_bz                  :  5; /* [ 4: 0],        r/w,        0xb */
            uint32_t ss0_pa_iaq_bz                  :  3; /* [ 7: 5],        r/w,        0x4 */
            uint32_t ss0_pa_vbcore_bz               :  5; /* [12: 8],        r/w,       0x11 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t ss0_pa_vbcas1_bz               :  4; /* [19:16],        r/w,        0xa */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_pa_config_bz;

    /* 0x274 : ss0_5g_pa_config_agn0 */
    union {
        struct {
            uint32_t ss0_5g_pa_vbcore_agn0          :  5; /* [ 4: 0],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcore_agn1          :  5; /* [ 9: 5],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcore_agn2          :  5; /* [14:10],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcore_agn3          :  5; /* [19:15],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcas1_agn           :  4; /* [23:20],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_pa_config_agn0;

    /* 0x278 : ss0_5g_pa_config_agn1 */
    union {
        struct {
            uint32_t ss0_5g_pa_iet_agn0             :  5; /* [ 4: 0],        r/w,        0xb */
            uint32_t ss0_5g_pa_iet_agn1             :  5; /* [ 9: 5],        r/w,        0xb */
            uint32_t ss0_5g_pa_iet_agn2             :  5; /* [14:10],        r/w,        0xb */
            uint32_t ss0_5g_pa_iet_agn3             :  5; /* [19:15],        r/w,        0xb */
            uint32_t ss0_5g_pa_iaq_agn              :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_pa_config_agn1;

    /* 0x27C : ss0_5g_pa_config_ax0 */
    union {
        struct {
            uint32_t ss0_5g_pa_vbcore_ax0           :  5; /* [ 4: 0],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcore_ax1           :  5; /* [ 9: 5],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcore_ax2           :  5; /* [14:10],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcore_ax3           :  5; /* [19:15],        r/w,       0x11 */
            uint32_t ss0_5g_pa_vbcas1_ax            :  4; /* [23:20],        r/w,        0xa */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_pa_config_ax0;

    /* 0x280 : ss0_5g_pa_config_ax1 */
    union {
        struct {
            uint32_t ss0_5g_pa_iet_ax0              :  5; /* [ 4: 0],        r/w,        0xb */
            uint32_t ss0_5g_pa_iet_ax1              :  5; /* [ 9: 5],        r/w,        0xb */
            uint32_t ss0_5g_pa_iet_ax2              :  5; /* [14:10],        r/w,        0xb */
            uint32_t ss0_5g_pa_iet_ax3              :  5; /* [19:15],        r/w,        0xb */
            uint32_t ss0_5g_pa_iaq_ax               :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_pa_config_ax1;

    /* 0x284  reserved */
    uint8_t RESERVED0x284[124];

    /* 0x300 : Digital Control */
    union {
        struct {
            uint32_t ss0_trxiqbuf_5g_gc             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t ss0_trxiqbuf_5g_en             :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_trxiqbuf_5g_casbias_sel    :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_rrf_5g_bm_sel              :  4; /* [19:16],        r/w,        0x0 */
            uint32_t ss0_rmxgm_5g_bm_boost          :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_bm_boost            :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t ss0_rrf_5g_bm_boost_sel        :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rrf_5g;

    /* 0x304 : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_5g_rfb3                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_rfb2                :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_rfb1                :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_cap2                :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_cap1                :  3; /* [18:16],        r/w,        0x1 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_bm                  :  4; /* [23:20],        r/w,        0x3 */
            uint32_t ss0_lna_5g_bm_hw               :  4; /* [27:24],          r,        0x3 */
            uint32_t ss0_lna_5g_sel                 :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_lna_5g_1;

    /* 0x308 : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_5g_load_csw_gc         :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_load_csw            :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t ss0_lna_5g_load_csw_hw         :  4; /* [11: 8],          r,        0x6 */
            uint32_t ss0_lna_5g_lg_cap              :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_lna_5g_casbias_sel         :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_lna_5g_2;

    /* 0x30C : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_aux_5g_cas_en          :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_lna_aux_5g_lg_cap          :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_lna_aux_5g_casbias_sel     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_lna_aux_5g_rfb             :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_lna_aux_5g_cap2            :  3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_lna_aux_5g_cap1            :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_lna_aux_5g_bm              :  4; /* [27:24],        r/w,        0x3 */
            uint32_t ss0_lna_aux_5g_bm_hw           :  4; /* [31:28],          r,        0x3 */
        }BF;
        uint32_t WORD;
    } ss0_lna_aux_5g_1;

    /* 0x310 : Digital Control */
    union {
        struct {
            uint32_t ss0_rmx_5g_bm                  :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_rmx_5g_bias_sel            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_15                  : 11; /* [15: 5],       rsvd,        0x0 */
            uint32_t ss0_rmxgm_5g_casbias_sel       :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rmxgm_5g_bm                :  3; /* [22:20],        r/w,        0x1 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rmxgm_rmx_5g_1;

    /* 0x314 : Digital Control */
    union {
        struct {
            uint32_t ss0_trxiqbuf_2g_gc             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t ss0_trxiqbuf_2g_en             :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_trxiqbuf_2g_casbias_sel    :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_rrf_2g_bm_sel              :  4; /* [19:16],        r/w,        0x0 */
            uint32_t ss0_rmxgm_2g_bm_boost          :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_bm_boost            :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t ss0_rrf_2g_bm_boost_sel        :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rrf_2g;

    /* 0x318 : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_2g_rfb3                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_rfb2                :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_rfb1                :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_cap2                :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_cap1                :  3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_bm                  :  4; /* [23:20],        r/w,        0x3 */
            uint32_t ss0_lna_2g_bm_hw               :  4; /* [27:24],          r,        0x3 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_lna_2g_1;

    /* 0x31C : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_2g_load_csw_gc         :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_load_csw            :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t ss0_lna_2g_load_csw_hw         :  4; /* [11: 8],          r,        0x6 */
            uint32_t ss0_lna_2g_lg_cap              :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_casbias_sel         :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_lna_2g_2;

    /* 0x320 : Digital Control */
    union {
        struct {
            uint32_t ss0_rmx_2g_bm                  :  3; /* [ 2: 0],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_rmx_2g_bias_sel            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_15                  : 11; /* [15: 5],       rsvd,        0x0 */
            uint32_t ss0_rmxgm_2g_casbias_sel       :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rmxgm_2g_bm                :  3; /* [22:20],        r/w,        0x1 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rmxgm_rmx_2g_1;

    /* 0x324 : ss0_rbb1 */
    union {
        struct {
            uint32_t ss0_rosdac_q                   :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_rosdac_i                   :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_rosdac_q_hw                :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_rosdac_i_hw                :  6; /* [29:24],          r,       0x20 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t ss0_rosdac_range               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb1;

    /* 0x328 : ss0_rbb2 */
    union {
        struct {
            uint32_t ss0_rbb_cap2_fc_q              :  7; /* [ 6: 0],        r/w,       0x34 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap2_fc_i              :  7; /* [14: 8],        r/w,       0x34 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap1_fc_q              :  7; /* [22:16],        r/w,       0x34 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap1_fc_i              :  7; /* [30:24],        r/w,       0x34 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb2;

    /* 0x32C : ss0_rbb3 */
    union {
        struct {
            uint32_t ss0_rbb_cgm_r_tune             :  4; /* [ 3: 0],        r/w,        0x7 */
            uint32_t ss0_rbb_startup_set            :  1; /* [    4],        r/w,        0x0 */
            uint32_t ss0_rbb_bt_fif_tune            :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_rbb_deq                    :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_rbb_bm_op                  :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_rbb_vcm                    :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rbb_bq_iqbias_short        :  1; /* [   20],        r/w,        0x1 */
            uint32_t ss0_rbb_tia_iqbias_short       :  1; /* [   21],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_rbb_bw                     :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_28                 :  3; /* [28:26],       rsvd,        0x0 */
            uint32_t ss0_pwr_det_en_hw              :  1; /* [   29],          r,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t ss0_pwr_det_en                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb3;

    /* 0x330 : ss0_rbb4 */
    union {
        struct {
            uint32_t ss0_rbb_cap_dpd_fc_i           :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t ss0_rbb_cap_dpd_fc_q           :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t ss0_rbb_bm_op1                 :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t ss0_rbb_bm_op2                 :  2; /* [11:10],        r/w,        0x0 */
            uint32_t ss0_rbb_bm_op3                 :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_rbb_atest_gain_r5          :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t ss0_rbb_atest_gain_r6          :  3; /* [26:24],        r/w,        0x0 */
            uint32_t ss0_atest_wifi_rbb_test_en     :  1; /* [   27],        r/w,        0x0 */
            uint32_t ss0_rbb_bt_mode                :  1; /* [   28],        r/w,        0x0 */
            uint32_t ss0_rbb_bt_mode_hw             :  1; /* [   29],          r,        0x0 */
            uint32_t ss0_rbb_atest_in_en            :  1; /* [   30],        r/w,        0x0 */
            uint32_t ss0_rbb_rccal_iq_swap          :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb4;

    /* 0x334 : ss0_rbb5 */
    union {
        struct {
            uint32_t ss0_rbb_cap_cc_op1_fc_i        :  4; /* [ 3: 0],        r/w,        0x3 */
            uint32_t ss0_rbb_cap_cc_op1_fc_q        :  4; /* [ 7: 4],        r/w,        0x3 */
            uint32_t ss0_rbb_cap_cc_op2_fc_i        :  5; /* [12: 8],        r/w,        0x6 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op2_fc_q        :  5; /* [18:14],        r/w,        0x6 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op3_fc_i        :  5; /* [24:20],        r/w,        0x6 */
            uint32_t reserved_25                    :  1; /* [   25],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op3_fc_q        :  5; /* [30:26],        r/w,        0x6 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb5;

    /* 0x338 : ss0_rbb6 */
    union {
        struct {
            uint32_t ss0_rbb_rz_op1_i               :  4; /* [ 3: 0],        r/w,        0xa */
            uint32_t ss0_rbb_rz_op1_q               :  4; /* [ 7: 4],        r/w,        0xa */
            uint32_t ss0_rbb_rz_op2_i               :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op2_q               :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op3_i               :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op3_q               :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb6;

    /* 0x33C : ss0_rbb7 */
    union {
        struct {
            uint32_t ss0_rbb_cap_cc_op1_fc_i_hw     :  4; /* [ 3: 0],          r,        0x3 */
            uint32_t ss0_rbb_cap_cc_op1_fc_q_hw     :  4; /* [ 7: 4],          r,        0x3 */
            uint32_t ss0_rbb_cap_cc_op2_fc_i_hw     :  5; /* [12: 8],          r,        0x6 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op2_fc_q_hw     :  5; /* [18:14],          r,        0x6 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op3_fc_i_hw     :  5; /* [24:20],          r,        0x6 */
            uint32_t reserved_25                    :  1; /* [   25],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op3_fc_q_hw     :  5; /* [30:26],          r,        0x6 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb7;

    /* 0x340 : ss0_rbb8 */
    union {
        struct {
            uint32_t ss0_rbb_rz_op1_i_hw            :  4; /* [ 3: 0],          r,        0x6 */
            uint32_t ss0_rbb_rz_op1_q_hw            :  4; /* [ 7: 4],          r,        0x6 */
            uint32_t ss0_rbb_rz_op2_i_hw            :  2; /* [ 9: 8],          r,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op2_q_hw            :  2; /* [13:12],          r,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op3_i_hw            :  2; /* [17:16],          r,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op3_q_hw            :  2; /* [21:20],          r,        0x2 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb8;

    /* 0x344 : ss0_rssibuf1 */
    union {
        struct {
            uint32_t ss0_rssibuf_i_en               :  1; /* [    0],        r/w,        0x1 */
            uint32_t ss0_rssibuf_q_en               :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_rssibuf_gain_rf            :  4; /* [ 7: 4],        r/w,        0x2 */
            uint32_t ss0_rssibuf_gain_rin           :  3; /* [10: 8],        r/w,        0x2 */
            uint32_t reserved_11_19                 :  9; /* [19:11],       rsvd,        0x0 */
            uint32_t ss0_rssibuf_cap_fc_i           :  4; /* [23:20],        r/w,        0x8 */
            uint32_t ss0_rssibuf_cap_fc_q           :  4; /* [27:24],        r/w,        0x8 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rssibuf1;

    /* 0x348 : ss0_rssibuf2 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t ss0_rssibuf_rz_op_i            :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_rssibuf_rz_op_q            :  2; /* [13:12],        r/w,        0x2 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_rssibuf_cap_cc_op_fc_i     :  5; /* [20:16],        r/w,        0x6 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t ss0_rssibuf_cap_cc_op_fc_q     :  5; /* [28:24],        r/w,        0x6 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rssibuf2;

    /* 0x34C : ss0_rssiadc_0 */
    union {
        struct {
            uint32_t ss0_rssiadc_clk_en             :  1; /* [    0],        r/w,        0x1 */
            uint32_t ss0_rssiadc_clk_en_i           :  1; /* [    1],        r/w,        0x1 */
            uint32_t ss0_rssiadc_clk_en_q           :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_rssiadc_dly_ctl_ck         :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_rssiadc_dly_ctl_clkasync   :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_rssiadc_clk_sync_inv       :  1; /* [   12],        r/w,        0x0 */
            uint32_t ss0_rssiadc_clk_inv            :  1; /* [   13],        r/w,        0x1 */
            uint32_t ss0_rssiadc_clk_div_sel        :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_rssiadc_vbuf_sel           :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rssiadc_vcm_sel            :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_rssiadc_vref_sel           :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rssiadc_0;

    /* 0x350  reserved */
    uint8_t RESERVED0x350[4];

    /* 0x354 : Digital Control */
    union {
        struct {
            uint32_t ss0_rxadc_vref_sel             :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_rxadc_dly_ctl              :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_rxadc_dvdd_sel             :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_rxadc_gt_rm                :  1; /* [   12],        r/w,        0x0 */
            uint32_t ss0_rxadc_clk_sync_inv         :  1; /* [   13],        r/w,        0x0 */
            uint32_t ss0_rxadc_clk_sync_sel         :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_rxadc_clk_inv              :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t ss0_rxadc_clk_div_sel          :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21                    :  1; /* [   21],       rsvd,        0x0 */
            uint32_t ss0_rxadc_clk_div_sel_hw       :  1; /* [   22],          r,        0x1 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_rxadc_vref_cp_en           :  1; /* [   24],        r/w,        0x1 */
            uint32_t ss0_rxadc_ti_en                :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26                    :  1; /* [   26],       rsvd,        0x0 */
            uint32_t ss0_rxadc_ti_en_hw             :  1; /* [   27],          r,        0x1 */
            uint32_t ss0_rxadc_vbuf_sel             :  2; /* [29:28],        r/w,        0x1 */
            uint32_t ss0_rxadc_clk_en               :  1; /* [   30],        r/w,        0x1 */
            uint32_t ss0_rxadc_clk_en_hw            :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } ss0_rxadc_0;

    /* 0x358 : Digital Control */
    union {
        struct {
            uint32_t ss0_rxadc_clk_80m_out_en       :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t ss0_rxadc_ext_clk_80m_in_en    :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_rxadc_clk_160m_out_en      :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5                     :  1; /* [    5],       rsvd,        0x0 */
            uint32_t ss0_rxadc_ext_clk_160m_in_en   :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_startup_set      :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_op_en            :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_bm_op_out        :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_bm_op            :  3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_op_rz            :  4; /* [23:20],        r/w,        0x2 */
            uint32_t ss0_rxadc_buf_op_cc            :  5; /* [28:24],        r/w,        0x6 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rxadc_1;

    /* 0x35C : Digital Control */
    union {
        struct {
            uint32_t ss0_rxadc_buf_r3               :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_r2               :  3; /* [ 6: 4],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_r1               :  3; /* [10: 8],        r/w,        0x2 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_bw_sel           :  2; /* [13:12],        r/w,        0x3 */
            uint32_t ss0_rxadc_buf_bw_sel_hw        :  2; /* [15:14],          r,        0x3 */
            uint32_t ss0_rxadc_buf_vcm_sel          :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_cgm_r_tune       :  4; /* [23:20],        r/w,        0x7 */
            uint32_t ss0_rxadc_buf_cap2_fc          :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t ss0_rxadc_buf_cap1_fc          :  3; /* [30:28],        r/w,        0x3 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rxadc_2;

    /* 0x360 : Digital Control */
    union {
        struct {
            uint32_t ss0_rxadc_dout_i               : 11; /* [10: 0],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t ss0_rxadc_dout_q               : 11; /* [26:16],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rxadc_3;

    /* 0x364 : Digital Control */
    union {
        struct {
            uint32_t ss0_atest_bm_op                :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ss0_atest_dac_ac_test_en       :  1; /* [    4],        r/w,        0x0 */
            uint32_t ss0_atest_bz_rbb_test_en       :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_atest_gain_r9              :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t ss0_atest_gain_r8              :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_atest_gain_r7              :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t ss0_atest_op_cc                :  4; /* [23:20],        r/w,        0x0 */
            uint32_t ss0_atest_startup_set          :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t ss0_atest_vcm_sel              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_atest_config;

    /* 0x368 : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_5g_load_csw_hg         :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t ss0_lna_5g_load_csw_lg         :  4; /* [ 7: 4],        r/w,        0x8 */
            uint32_t ss0_lna_5g_bm_hg               :  4; /* [11: 8],        r/w,        0x3 */
            uint32_t ss0_lna_5g_bm_lg               :  4; /* [15:12],        r/w,        0x3 */
            uint32_t ss0_lna_aux_5g_bm_hg           :  4; /* [19:16],        r/w,        0x3 */
            uint32_t ss0_lna_aux_5g_bm_lg           :  4; /* [23:20],        r/w,        0x3 */
            uint32_t ss0_lna_5g_load_csw_h          :  4; /* [27:24],        r/w,        0x0 */
            uint32_t ss0_lna_5g_load_csw_l          :  4; /* [31:28],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } ss0_lna_5g_ctrl_hw_mux;

    /* 0x36C : Digital Control */
    union {
        struct {
            uint32_t ss0_lna_2g_load_csw_hg         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t ss0_lna_2g_load_csw_lg         :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t ss0_lna_2g_bm_hg               :  4; /* [11: 8],        r/w,        0x3 */
            uint32_t ss0_lna_2g_bm_lg               :  4; /* [15:12],        r/w,        0x3 */
            uint32_t reserved_16_23                 :  8; /* [23:16],       rsvd,        0x0 */
            uint32_t ss0_lna_2g_load_csw_h          :  4; /* [27:24],        r/w,        0x7 */
            uint32_t ss0_lna_2g_load_csw_l          :  4; /* [31:28],        r/w,        0xb */
        }BF;
        uint32_t WORD;
    } ss0_lna_2g_ctrl_hw_mux;

    /* 0x370 : Digital Control */
    union {
        struct {
            uint32_t ss0_rbb_cap_cc_op1_fc_bz       :  4; /* [ 3: 0],        r/w,        0xb */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op1_fc_wifi     :  4; /* [11: 8],        r/w,        0x3 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_cap_cc_op1_fc_ctrl_hw_mux;

    /* 0x374 : Digital Control */
    union {
        struct {
            uint32_t ss0_rbb_rz_op1_bz              :  4; /* [ 3: 0],        r/w,        0x2 */
            uint32_t ss0_rbb_rz_op1_wifi            :  4; /* [ 7: 4],        r/w,        0xa */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_rz_op1_ctrl_hw_mux;

    /* 0x378 : Digital Control */
    union {
        struct {
            uint32_t ss0_rbb_cap_cc_op2_fc_bz       :  5; /* [ 4: 0],        r/w,       0x17 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op2_fc_wifi     :  5; /* [12: 8],        r/w,        0x6 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_cap_cc_op2_fc_ctrl_hw_mux;

    /* 0x37C : Digital Control */
    union {
        struct {
            uint32_t ss0_rbb_rz_op2_bz              :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t ss0_rbb_rz_op2_wifi            :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_rz_op2_ctrl_hw_mux;

    /* 0x380 : Digital Control */
    union {
        struct {
            uint32_t ss0_rbb_cap_cc_op3_fc_bz       :  5; /* [ 4: 0],        r/w,       0x17 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ss0_rbb_cap_cc_op3_fc_wifi     :  5; /* [12: 8],        r/w,        0x6 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_cap_cc_op3_fc_ctrl_hw_mux;

    /* 0x384 : Digital Control */
    union {
        struct {
            uint32_t ss0_rbb_rz_op3_bz              :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_rbb_rz_op3_wifi            :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_rz_op3_ctrl_hw_mux;

    /* 0x388 : ss0_resv */
    union {
        struct {
            uint32_t ss0_resv0                      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t ss0_resv1                      : 16; /* [31:16],        r/w,     0xffff */
        }BF;
        uint32_t WORD;
    } ss0_resv;

    /* 0x38C : ss0_resv1 */
    union {
        struct {
            uint32_t ss0_resv_rb                    : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_resv1;

    /* 0x390  reserved */
    uint8_t RESERVED0x390[880];

    /* 0x700 : ss0_2g_rx_gain_tbl0 */
    union {
        struct {
            uint32_t ss0_2g_gc_lna_load_att_0       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_0              :  2; /* [ 4: 3],        r/w,        0x1 */
            uint32_t ss0_2g_gc_lna_0                :  3; /* [ 7: 5],        r/w,        0x0 */
            uint32_t ss0_2g_gc_lna_load_att_1       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_1              :  2; /* [12:11],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_1                :  3; /* [15:13],        r/w,        0x0 */
            uint32_t ss0_2g_gc_lna_load_att_2       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_2              :  2; /* [20:19],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_2                :  3; /* [23:21],        r/w,        0x1 */
            uint32_t ss0_2g_gc_lna_load_att_3       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_3              :  2; /* [28:27],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_3                :  3; /* [31:29],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rx_gain_tbl0;

    /* 0x704 : ss0_2g_rx_gain_tbl1 */
    union {
        struct {
            uint32_t ss0_2g_gc_lna_load_att_4       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_4              :  2; /* [ 4: 3],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_4                :  3; /* [ 7: 5],        r/w,        0x3 */
            uint32_t ss0_2g_gc_lna_load_att_5       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_5              :  2; /* [12:11],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_5                :  3; /* [15:13],        r/w,        0x4 */
            uint32_t ss0_2g_gc_lna_load_att_6       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_6              :  2; /* [20:19],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_6                :  3; /* [23:21],        r/w,        0x5 */
            uint32_t ss0_2g_gc_lna_load_att_7       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_7              :  2; /* [28:27],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_7                :  3; /* [31:29],        r/w,        0x6 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rx_gain_tbl1;

    /* 0x708 : ss0_2g_rx_gain_tbl2 */
    union {
        struct {
            uint32_t ss0_2g_gc_lna_load_att_8       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_8              :  2; /* [ 4: 3],        r/w,        0x2 */
            uint32_t ss0_2g_gc_lna_8                :  3; /* [ 7: 5],        r/w,        0x7 */
            uint32_t ss0_2g_gc_lna_load_att_9       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t ss0_2g_gc_rmxgm_9              :  2; /* [12:11],        r/w,        0x3 */
            uint32_t ss0_2g_gc_lna_9                :  3; /* [15:13],        r/w,        0x7 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rx_gain_tbl2;

    /* 0x70C : ss0_5g_rx_gain_tbl0 */
    union {
        struct {
            uint32_t ss0_5g_gc_lna_load_att_0       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_0              :  2; /* [ 4: 3],        r/w,        0x1 */
            uint32_t ss0_5g_gc_lna_0                :  3; /* [ 7: 5],        r/w,        0x0 */
            uint32_t ss0_5g_gc_lna_load_att_1       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_1              :  2; /* [12:11],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_1                :  3; /* [15:13],        r/w,        0x0 */
            uint32_t ss0_5g_gc_lna_load_att_2       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_2              :  2; /* [20:19],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_2                :  3; /* [23:21],        r/w,        0x1 */
            uint32_t ss0_5g_gc_lna_load_att_3       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_3              :  2; /* [28:27],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_3                :  3; /* [31:29],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rx_gain_tbl0;

    /* 0x710 : ss0_5g_rx_gain_tbl1 */
    union {
        struct {
            uint32_t ss0_5g_gc_lna_load_att_4       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_4              :  2; /* [ 4: 3],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_4                :  3; /* [ 7: 5],        r/w,        0x3 */
            uint32_t ss0_5g_gc_lna_load_att_5       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_5              :  2; /* [12:11],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_5                :  3; /* [15:13],        r/w,        0x4 */
            uint32_t ss0_5g_gc_lna_load_att_6       :  3; /* [18:16],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_6              :  2; /* [20:19],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_6                :  3; /* [23:21],        r/w,        0x5 */
            uint32_t ss0_5g_gc_lna_load_att_7       :  3; /* [26:24],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_7              :  2; /* [28:27],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_7                :  3; /* [31:29],        r/w,        0x6 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rx_gain_tbl1;

    /* 0x714 : ss0_5g_rx_gain_tbl2 */
    union {
        struct {
            uint32_t ss0_5g_gc_lna_load_att_8       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_8              :  2; /* [ 4: 3],        r/w,        0x2 */
            uint32_t ss0_5g_gc_lna_8                :  3; /* [ 7: 5],        r/w,        0x7 */
            uint32_t ss0_5g_gc_lna_load_att_9       :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t ss0_5g_gc_rmxgm_9              :  2; /* [12:11],        r/w,        0x3 */
            uint32_t ss0_5g_gc_lna_9                :  3; /* [15:13],        r/w,        0x7 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rx_gain_tbl2;

    /* 0x718  reserved */
    uint8_t RESERVED0x718[24];

    /* 0x730 : ss0_rbb_tbl0 */
    union {
        struct {
            uint32_t ss0_gc_rbb1_0                  :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_0                  :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_1                  :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_1                  :  3; /* [14:12],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_2                  :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_2                  :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_3                  :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_3                  :  3; /* [30:28],        r/w,        0x3 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_tbl0;

    /* 0x734 : ss0_rbb_tbl1 */
    union {
        struct {
            uint32_t ss0_gc_rbb1_4                  :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_4                  :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_5                  :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_5                  :  3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_6                  :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_6                  :  3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_7                  :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_7                  :  3; /* [30:28],        r/w,        0x4 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_tbl1;

    /* 0x738 : ss0_rbb_tbl2 */
    union {
        struct {
            uint32_t ss0_gc_rbb1_8                  :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_8                  :  3; /* [ 6: 4],        r/w,        0x5 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_9                  :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_9                  :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_10                 :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_10                 :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_11                 :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_11                 :  3; /* [30:28],        r/w,        0x5 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_tbl2;

    /* 0x73C : ss0_rbb_tbl3 */
    union {
        struct {
            uint32_t ss0_gc_rbb1_12                 :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_12                 :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_13                 :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_13                 :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_14                 :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_14                 :  3; /* [22:20],        r/w,        0x5 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb1_15                 :  2; /* [25:24],        r/w,        0x3 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_15                 :  3; /* [30:28],        r/w,        0x6 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_tbl3;

    /* 0x740 : ss0_rbb_tbl4 */
    union {
        struct {
            uint32_t ss0_gc_rbb1_16                 :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t ss0_gc_rbb2_16                 :  3; /* [ 6: 4],        r/w,        0x7 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_rbb_tbl4;

    /* 0x744  reserved */
    uint8_t RESERVED0x744[20];

    /* 0x758 : ss0_2g_rosdac_tbl0 */
    union {
        struct {
            uint32_t ss0_2g_rosdac_i_gc0            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_2g_rosdac_q_gc0            :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_2g_rosdac_i_gc1            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_2g_rosdac_q_gc1            :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rosdac_tbl0;

    /* 0x75C : ss0_2g_rosdac_tbl1 */
    union {
        struct {
            uint32_t ss0_2g_rosdac_i_gc2            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_2g_rosdac_q_gc2            :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_2g_rosdac_i_gc3            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_2g_rosdac_q_gc3            :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rosdac_tbl1;

    /* 0x760 : ss0_5g_rosdac_tbl0 */
    union {
        struct {
            uint32_t ss0_5g_rosdac_i_gc0            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_5g_rosdac_q_gc0            :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_5g_rosdac_i_gc1            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_5g_rosdac_q_gc1            :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rosdac_tbl0;

    /* 0x764 : ss0_5g_rosdac_tbl1 */
    union {
        struct {
            uint32_t ss0_5g_rosdac_i_gc2            :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ss0_5g_rosdac_q_gc2            :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ss0_5g_rosdac_i_gc3            :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ss0_5g_rosdac_q_gc3            :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rosdac_tbl1;

    /* 0x768  reserved */
    uint8_t RESERVED0x768[16];

    /* 0x778 : ss0_2g_rxiq_ctrl_tbl0 */
    union {
        struct {
            uint32_t ss0_2g_rx_iq_phase_comp_gc0    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_2g_rx_iq_gain_comp_gc0     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rxiq_ctrl_tbl0;

    /* 0x77C : ss0_2g_rxiq_ctrl_tbl1 */
    union {
        struct {
            uint32_t ss0_2g_rx_iq_phase_comp_gc1    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_2g_rx_iq_gain_comp_gc1     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rxiq_ctrl_tbl1;

    /* 0x780 : ss0_2g_rxiq_ctrl_tbl2 */
    union {
        struct {
            uint32_t ss0_2g_rx_iq_phase_comp_gc2    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_2g_rx_iq_gain_comp_gc2     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rxiq_ctrl_tbl2;

    /* 0x784 : ss0_2g_rxiq_ctrl_tbl3 */
    union {
        struct {
            uint32_t ss0_2g_rx_iq_phase_comp_gc3    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_2g_rx_iq_gain_comp_gc3     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_2g_rxiq_ctrl_tbl3;

    /* 0x788 : ss0_5g_rxiq_ctrl_tbl0 */
    union {
        struct {
            uint32_t ss0_5g_rx_iq_phase_comp_gc0    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_5g_rx_iq_gain_comp_gc0     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rxiq_ctrl_tbl0;

    /* 0x78C : ss0_5g_rxiq_ctrl_tbl1 */
    union {
        struct {
            uint32_t ss0_5g_rx_iq_phase_comp_gc1    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_5g_rx_iq_gain_comp_gc1     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rxiq_ctrl_tbl1;

    /* 0x790 : ss0_5g_rxiq_ctrl_tbl2 */
    union {
        struct {
            uint32_t ss0_5g_rx_iq_phase_comp_gc2    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_5g_rx_iq_gain_comp_gc2     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rxiq_ctrl_tbl2;

    /* 0x794 : ss0_5g_rxiq_ctrl_tbl3 */
    union {
        struct {
            uint32_t ss0_5g_rx_iq_phase_comp_gc3    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t ss0_5g_rx_iq_gain_comp_gc3     : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ss0_5g_rxiq_ctrl_tbl3;

};

typedef volatile struct rf_ana1_reg rf_ana1_reg_t;


#endif  /* __RF_ANA1_REG_H__ */
