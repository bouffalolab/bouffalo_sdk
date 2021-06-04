/**
  ******************************************************************************
  * @file    rf_reg.h
  * @version V1.2
  * @date    2019-11-20
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
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
#ifndef  __RF_REG_H__
#define  __RF_REG_H__

#include "bl602.h"

/* 0x0 : Silicon revision */
#define RF_REV_OFFSET                                           (0x0)
#define RF_ID                                                   RF_ID
#define RF_ID_POS                                               (0U)
#define RF_ID_LEN                                               (8U)
#define RF_ID_MSK                                               (((1U<<RF_ID_LEN)-1)<<RF_ID_POS)
#define RF_ID_UMSK                                              (~(((1U<<RF_ID_LEN)-1)<<RF_ID_POS))
#define RF_FW_REV                                               RF_FW_REV
#define RF_FW_REV_POS                                           (8U)
#define RF_FW_REV_LEN                                           (8U)
#define RF_FW_REV_MSK                                           (((1U<<RF_FW_REV_LEN)-1)<<RF_FW_REV_POS)
#define RF_FW_REV_UMSK                                          (~(((1U<<RF_FW_REV_LEN)-1)<<RF_FW_REV_POS))
#define RF_HW_REV                                               RF_HW_REV
#define RF_HW_REV_POS                                           (16U)
#define RF_HW_REV_LEN                                           (8U)
#define RF_HW_REV_MSK                                           (((1U<<RF_HW_REV_LEN)-1)<<RF_HW_REV_POS)
#define RF_HW_REV_UMSK                                          (~(((1U<<RF_HW_REV_LEN)-1)<<RF_HW_REV_POS))

/* 0x4 : Digital Control */
#define RF_FSM_CTRL_HW_OFFSET                                   (0x4)
#define RF_FSM_CTRL_EN                                          RF_FSM_CTRL_EN
#define RF_FSM_CTRL_EN_POS                                      (1U)
#define RF_FSM_CTRL_EN_LEN                                      (1U)
#define RF_FSM_CTRL_EN_MSK                                      (((1U<<RF_FSM_CTRL_EN_LEN)-1)<<RF_FSM_CTRL_EN_POS)
#define RF_FSM_CTRL_EN_UMSK                                     (~(((1U<<RF_FSM_CTRL_EN_LEN)-1)<<RF_FSM_CTRL_EN_POS))
#define RF_FSM_T2R_CAL_MODE                                     RF_FSM_T2R_CAL_MODE
#define RF_FSM_T2R_CAL_MODE_POS                                 (2U)
#define RF_FSM_T2R_CAL_MODE_LEN                                 (2U)
#define RF_FSM_T2R_CAL_MODE_MSK                                 (((1U<<RF_FSM_T2R_CAL_MODE_LEN)-1)<<RF_FSM_T2R_CAL_MODE_POS)
#define RF_FSM_T2R_CAL_MODE_UMSK                                (~(((1U<<RF_FSM_T2R_CAL_MODE_LEN)-1)<<RF_FSM_T2R_CAL_MODE_POS))
#define RF_FSM_STATE                                            RF_FSM_STATE
#define RF_FSM_STATE_POS                                        (4U)
#define RF_FSM_STATE_LEN                                        (3U)
#define RF_FSM_STATE_MSK                                        (((1U<<RF_FSM_STATE_LEN)-1)<<RF_FSM_STATE_POS)
#define RF_FSM_STATE_UMSK                                       (~(((1U<<RF_FSM_STATE_LEN)-1)<<RF_FSM_STATE_POS))
#define RF_RC_STATE_DBG                                         RF_RC_STATE_DBG
#define RF_RC_STATE_DBG_POS                                     (8U)
#define RF_RC_STATE_DBG_LEN                                     (3U)
#define RF_RC_STATE_DBG_MSK                                     (((1U<<RF_RC_STATE_DBG_LEN)-1)<<RF_RC_STATE_DBG_POS)
#define RF_RC_STATE_DBG_UMSK                                    (~(((1U<<RF_RC_STATE_DBG_LEN)-1)<<RF_RC_STATE_DBG_POS))
#define RF_RC_STATE_DBG_EN                                      RF_RC_STATE_DBG_EN
#define RF_RC_STATE_DBG_EN_POS                                  (11U)
#define RF_RC_STATE_DBG_EN_LEN                                  (1U)
#define RF_RC_STATE_DBG_EN_MSK                                  (((1U<<RF_RC_STATE_DBG_EN_LEN)-1)<<RF_RC_STATE_DBG_EN_POS)
#define RF_RC_STATE_DBG_EN_UMSK                                 (~(((1U<<RF_RC_STATE_DBG_EN_LEN)-1)<<RF_RC_STATE_DBG_EN_POS))
#define RF_FSM_ST_INT_SEL                                       RF_FSM_ST_INT_SEL
#define RF_FSM_ST_INT_SEL_POS                                   (12U)
#define RF_FSM_ST_INT_SEL_LEN                                   (3U)
#define RF_FSM_ST_INT_SEL_MSK                                   (((1U<<RF_FSM_ST_INT_SEL_LEN)-1)<<RF_FSM_ST_INT_SEL_POS)
#define RF_FSM_ST_INT_SEL_UMSK                                  (~(((1U<<RF_FSM_ST_INT_SEL_LEN)-1)<<RF_FSM_ST_INT_SEL_POS))
#define RF_FSM_ST_INT                                           RF_FSM_ST_INT
#define RF_FSM_ST_INT_POS                                       (16U)
#define RF_FSM_ST_INT_LEN                                       (1U)
#define RF_FSM_ST_INT_MSK                                       (((1U<<RF_FSM_ST_INT_LEN)-1)<<RF_FSM_ST_INT_POS)
#define RF_FSM_ST_INT_UMSK                                      (~(((1U<<RF_FSM_ST_INT_LEN)-1)<<RF_FSM_ST_INT_POS))
#define RF_FSM_ST_INT_CLR                                       RF_FSM_ST_INT_CLR
#define RF_FSM_ST_INT_CLR_POS                                   (20U)
#define RF_FSM_ST_INT_CLR_LEN                                   (1U)
#define RF_FSM_ST_INT_CLR_MSK                                   (((1U<<RF_FSM_ST_INT_CLR_LEN)-1)<<RF_FSM_ST_INT_CLR_POS)
#define RF_FSM_ST_INT_CLR_UMSK                                  (~(((1U<<RF_FSM_ST_INT_CLR_LEN)-1)<<RF_FSM_ST_INT_CLR_POS))
#define RF_FSM_ST_INT_SET                                       RF_FSM_ST_INT_SET
#define RF_FSM_ST_INT_SET_POS                                   (24U)
#define RF_FSM_ST_INT_SET_LEN                                   (1U)
#define RF_FSM_ST_INT_SET_MSK                                   (((1U<<RF_FSM_ST_INT_SET_LEN)-1)<<RF_FSM_ST_INT_SET_POS)
#define RF_FSM_ST_INT_SET_UMSK                                  (~(((1U<<RF_FSM_ST_INT_SET_LEN)-1)<<RF_FSM_ST_INT_SET_POS))
#define RF_RC_STATE_VALUE                                       RF_RC_STATE_VALUE
#define RF_RC_STATE_VALUE_POS                                   (28U)
#define RF_RC_STATE_VALUE_LEN                                   (3U)
#define RF_RC_STATE_VALUE_MSK                                   (((1U<<RF_RC_STATE_VALUE_LEN)-1)<<RF_RC_STATE_VALUE_POS)
#define RF_RC_STATE_VALUE_UMSK                                  (~(((1U<<RF_RC_STATE_VALUE_LEN)-1)<<RF_RC_STATE_VALUE_POS))

/* 0x8 : rfsm status reg */
#define RF_FSM_CTRL_SW_OFFSET                                   (0x8)
#define RF_FSM_SW_ST                                            RF_FSM_SW_ST
#define RF_FSM_SW_ST_POS                                        (0U)
#define RF_FSM_SW_ST_LEN                                        (5U)
#define RF_FSM_SW_ST_MSK                                        (((1U<<RF_FSM_SW_ST_LEN)-1)<<RF_FSM_SW_ST_POS)
#define RF_FSM_SW_ST_UMSK                                       (~(((1U<<RF_FSM_SW_ST_LEN)-1)<<RF_FSM_SW_ST_POS))
#define RF_FSM_SW_ST_VLD                                        RF_FSM_SW_ST_VLD
#define RF_FSM_SW_ST_VLD_POS                                    (8U)
#define RF_FSM_SW_ST_VLD_LEN                                    (1U)
#define RF_FSM_SW_ST_VLD_MSK                                    (((1U<<RF_FSM_SW_ST_VLD_LEN)-1)<<RF_FSM_SW_ST_VLD_POS)
#define RF_FSM_SW_ST_VLD_UMSK                                   (~(((1U<<RF_FSM_SW_ST_VLD_LEN)-1)<<RF_FSM_SW_ST_VLD_POS))
#define RF_FULL_CAL_EN                                          RF_FULL_CAL_EN
#define RF_FULL_CAL_EN_POS                                      (12U)
#define RF_FULL_CAL_EN_LEN                                      (1U)
#define RF_FULL_CAL_EN_MSK                                      (((1U<<RF_FULL_CAL_EN_LEN)-1)<<RF_FULL_CAL_EN_POS)
#define RF_FULL_CAL_EN_UMSK                                     (~(((1U<<RF_FULL_CAL_EN_LEN)-1)<<RF_FULL_CAL_EN_POS))
#define RF_INC_CAL_TIMEOUT                                      RF_INC_CAL_TIMEOUT
#define RF_INC_CAL_TIMEOUT_POS                                  (16U)
#define RF_INC_CAL_TIMEOUT_LEN                                  (1U)
#define RF_INC_CAL_TIMEOUT_MSK                                  (((1U<<RF_INC_CAL_TIMEOUT_LEN)-1)<<RF_INC_CAL_TIMEOUT_POS)
#define RF_INC_CAL_TIMEOUT_UMSK                                 (~(((1U<<RF_INC_CAL_TIMEOUT_LEN)-1)<<RF_INC_CAL_TIMEOUT_POS))
#define RF_LO_UNLOCKED                                          RF_LO_UNLOCKED
#define RF_LO_UNLOCKED_POS                                      (20U)
#define RF_LO_UNLOCKED_LEN                                      (1U)
#define RF_LO_UNLOCKED_MSK                                      (((1U<<RF_LO_UNLOCKED_LEN)-1)<<RF_LO_UNLOCKED_POS)
#define RF_LO_UNLOCKED_UMSK                                     (~(((1U<<RF_LO_UNLOCKED_LEN)-1)<<RF_LO_UNLOCKED_POS))

/* 0xC : Control logic switch */
#define RFCTRL_HW_EN_OFFSET                                     (0xC)
#define RF_PU_CTRL_HW                                           RF_PU_CTRL_HW
#define RF_PU_CTRL_HW_POS                                       (0U)
#define RF_PU_CTRL_HW_LEN                                       (1U)
#define RF_PU_CTRL_HW_MSK                                       (((1U<<RF_PU_CTRL_HW_LEN)-1)<<RF_PU_CTRL_HW_POS)
#define RF_PU_CTRL_HW_UMSK                                      (~(((1U<<RF_PU_CTRL_HW_LEN)-1)<<RF_PU_CTRL_HW_POS))
#define RF_RX_GAIN_CTRL_HW                                      RF_RX_GAIN_CTRL_HW
#define RF_RX_GAIN_CTRL_HW_POS                                  (1U)
#define RF_RX_GAIN_CTRL_HW_LEN                                  (1U)
#define RF_RX_GAIN_CTRL_HW_MSK                                  (((1U<<RF_RX_GAIN_CTRL_HW_LEN)-1)<<RF_RX_GAIN_CTRL_HW_POS)
#define RF_RX_GAIN_CTRL_HW_UMSK                                 (~(((1U<<RF_RX_GAIN_CTRL_HW_LEN)-1)<<RF_RX_GAIN_CTRL_HW_POS))
#define RF_TX_GAIN_CTRL_HW                                      RF_TX_GAIN_CTRL_HW
#define RF_TX_GAIN_CTRL_HW_POS                                  (2U)
#define RF_TX_GAIN_CTRL_HW_LEN                                  (1U)
#define RF_TX_GAIN_CTRL_HW_MSK                                  (((1U<<RF_TX_GAIN_CTRL_HW_LEN)-1)<<RF_TX_GAIN_CTRL_HW_POS)
#define RF_TX_GAIN_CTRL_HW_UMSK                                 (~(((1U<<RF_TX_GAIN_CTRL_HW_LEN)-1)<<RF_TX_GAIN_CTRL_HW_POS))
#define RF_LNA_CTRL_HW                                          RF_LNA_CTRL_HW
#define RF_LNA_CTRL_HW_POS                                      (3U)
#define RF_LNA_CTRL_HW_LEN                                      (1U)
#define RF_LNA_CTRL_HW_MSK                                      (((1U<<RF_LNA_CTRL_HW_LEN)-1)<<RF_LNA_CTRL_HW_POS)
#define RF_LNA_CTRL_HW_UMSK                                     (~(((1U<<RF_LNA_CTRL_HW_LEN)-1)<<RF_LNA_CTRL_HW_POS))
#define RF_RBB_BW_CTRL_HW                                       RF_RBB_BW_CTRL_HW
#define RF_RBB_BW_CTRL_HW_POS                                   (4U)
#define RF_RBB_BW_CTRL_HW_LEN                                   (1U)
#define RF_RBB_BW_CTRL_HW_MSK                                   (((1U<<RF_RBB_BW_CTRL_HW_LEN)-1)<<RF_RBB_BW_CTRL_HW_POS)
#define RF_RBB_BW_CTRL_HW_UMSK                                  (~(((1U<<RF_RBB_BW_CTRL_HW_LEN)-1)<<RF_RBB_BW_CTRL_HW_POS))
#define RF_TRXCAL_CTRL_HW                                       RF_TRXCAL_CTRL_HW
#define RF_TRXCAL_CTRL_HW_POS                                   (5U)
#define RF_TRXCAL_CTRL_HW_LEN                                   (1U)
#define RF_TRXCAL_CTRL_HW_MSK                                   (((1U<<RF_TRXCAL_CTRL_HW_LEN)-1)<<RF_TRXCAL_CTRL_HW_POS)
#define RF_TRXCAL_CTRL_HW_UMSK                                  (~(((1U<<RF_TRXCAL_CTRL_HW_LEN)-1)<<RF_TRXCAL_CTRL_HW_POS))
#define RF_LO_CTRL_HW                                           RF_LO_CTRL_HW
#define RF_LO_CTRL_HW_POS                                       (6U)
#define RF_LO_CTRL_HW_LEN                                       (1U)
#define RF_LO_CTRL_HW_MSK                                       (((1U<<RF_LO_CTRL_HW_LEN)-1)<<RF_LO_CTRL_HW_POS)
#define RF_LO_CTRL_HW_UMSK                                      (~(((1U<<RF_LO_CTRL_HW_LEN)-1)<<RF_LO_CTRL_HW_POS))
#define RF_INC_ACAL_CTRL_EN_HW                                  RF_INC_ACAL_CTRL_EN_HW
#define RF_INC_ACAL_CTRL_EN_HW_POS                              (7U)
#define RF_INC_ACAL_CTRL_EN_HW_LEN                              (1U)
#define RF_INC_ACAL_CTRL_EN_HW_MSK                              (((1U<<RF_INC_ACAL_CTRL_EN_HW_LEN)-1)<<RF_INC_ACAL_CTRL_EN_HW_POS)
#define RF_INC_ACAL_CTRL_EN_HW_UMSK                             (~(((1U<<RF_INC_ACAL_CTRL_EN_HW_LEN)-1)<<RF_INC_ACAL_CTRL_EN_HW_POS))
#define RF_INC_FCAL_CTRL_EN_HW                                  RF_INC_FCAL_CTRL_EN_HW
#define RF_INC_FCAL_CTRL_EN_HW_POS                              (8U)
#define RF_INC_FCAL_CTRL_EN_HW_LEN                              (1U)
#define RF_INC_FCAL_CTRL_EN_HW_MSK                              (((1U<<RF_INC_FCAL_CTRL_EN_HW_LEN)-1)<<RF_INC_FCAL_CTRL_EN_HW_POS)
#define RF_INC_FCAL_CTRL_EN_HW_UMSK                             (~(((1U<<RF_INC_FCAL_CTRL_EN_HW_LEN)-1)<<RF_INC_FCAL_CTRL_EN_HW_POS))
#define RF_SDM_CTRL_HW                                          RF_SDM_CTRL_HW
#define RF_SDM_CTRL_HW_POS                                      (9U)
#define RF_SDM_CTRL_HW_LEN                                      (1U)
#define RF_SDM_CTRL_HW_MSK                                      (((1U<<RF_SDM_CTRL_HW_LEN)-1)<<RF_SDM_CTRL_HW_POS)
#define RF_SDM_CTRL_HW_UMSK                                     (~(((1U<<RF_SDM_CTRL_HW_LEN)-1)<<RF_SDM_CTRL_HW_POS))
#define RF_RBB_PKDET_EN_CTRL_HW                                 RF_RBB_PKDET_EN_CTRL_HW
#define RF_RBB_PKDET_EN_CTRL_HW_POS                             (10U)
#define RF_RBB_PKDET_EN_CTRL_HW_LEN                             (1U)
#define RF_RBB_PKDET_EN_CTRL_HW_MSK                             (((1U<<RF_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_EN_CTRL_HW_POS)
#define RF_RBB_PKDET_EN_CTRL_HW_UMSK                            (~(((1U<<RF_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_EN_CTRL_HW_POS))
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW                           RF_RBB_PKDET_OUT_RSTN_CTRL_HW
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS                       (11U)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN                       (1U)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_MSK                       (((1U<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_UMSK                      (~(((1U<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS))
#define RF_ADDA_CTRL_HW                                         RF_ADDA_CTRL_HW
#define RF_ADDA_CTRL_HW_POS                                     (12U)
#define RF_ADDA_CTRL_HW_LEN                                     (1U)
#define RF_ADDA_CTRL_HW_MSK                                     (((1U<<RF_ADDA_CTRL_HW_LEN)-1)<<RF_ADDA_CTRL_HW_POS)
#define RF_ADDA_CTRL_HW_UMSK                                    (~(((1U<<RF_ADDA_CTRL_HW_LEN)-1)<<RF_ADDA_CTRL_HW_POS))

/* 0x10 : temp_comp */
#define RF_TEMP_COMP_OFFSET                                     (0x10)
#define RF_CONST_ACAL                                           RF_CONST_ACAL
#define RF_CONST_ACAL_POS                                       (0U)
#define RF_CONST_ACAL_LEN                                       (8U)
#define RF_CONST_ACAL_MSK                                       (((1U<<RF_CONST_ACAL_LEN)-1)<<RF_CONST_ACAL_POS)
#define RF_CONST_ACAL_UMSK                                      (~(((1U<<RF_CONST_ACAL_LEN)-1)<<RF_CONST_ACAL_POS))
#define RF_CONST_FCAL                                           RF_CONST_FCAL
#define RF_CONST_FCAL_POS                                       (8U)
#define RF_CONST_FCAL_LEN                                       (8U)
#define RF_CONST_FCAL_MSK                                       (((1U<<RF_CONST_FCAL_LEN)-1)<<RF_CONST_FCAL_POS)
#define RF_CONST_FCAL_UMSK                                      (~(((1U<<RF_CONST_FCAL_LEN)-1)<<RF_CONST_FCAL_POS))
#define RF_TEMP_COMP_EN                                         RF_TEMP_COMP_EN
#define RF_TEMP_COMP_EN_POS                                     (16U)
#define RF_TEMP_COMP_EN_LEN                                     (1U)
#define RF_TEMP_COMP_EN_MSK                                     (((1U<<RF_TEMP_COMP_EN_LEN)-1)<<RF_TEMP_COMP_EN_POS)
#define RF_TEMP_COMP_EN_UMSK                                    (~(((1U<<RF_TEMP_COMP_EN_LEN)-1)<<RF_TEMP_COMP_EN_POS))

/* 0x14 : rfcal_status */
#define RFCAL_STATUS_OFFSET                                     (0x14)
#define RF_RCAL_STATUS                                          RF_RCAL_STATUS
#define RF_RCAL_STATUS_POS                                      (0U)
#define RF_RCAL_STATUS_LEN                                      (2U)
#define RF_RCAL_STATUS_MSK                                      (((1U<<RF_RCAL_STATUS_LEN)-1)<<RF_RCAL_STATUS_POS)
#define RF_RCAL_STATUS_UMSK                                     (~(((1U<<RF_RCAL_STATUS_LEN)-1)<<RF_RCAL_STATUS_POS))
#define RF_ADC_OSCAL_STATUS                                     RF_ADC_OSCAL_STATUS
#define RF_ADC_OSCAL_STATUS_POS                                 (2U)
#define RF_ADC_OSCAL_STATUS_LEN                                 (2U)
#define RF_ADC_OSCAL_STATUS_MSK                                 (((1U<<RF_ADC_OSCAL_STATUS_LEN)-1)<<RF_ADC_OSCAL_STATUS_POS)
#define RF_ADC_OSCAL_STATUS_UMSK                                (~(((1U<<RF_ADC_OSCAL_STATUS_LEN)-1)<<RF_ADC_OSCAL_STATUS_POS))
#define RF_FCAL_STATUS                                          RF_FCAL_STATUS
#define RF_FCAL_STATUS_POS                                      (4U)
#define RF_FCAL_STATUS_LEN                                      (2U)
#define RF_FCAL_STATUS_MSK                                      (((1U<<RF_FCAL_STATUS_LEN)-1)<<RF_FCAL_STATUS_POS)
#define RF_FCAL_STATUS_UMSK                                     (~(((1U<<RF_FCAL_STATUS_LEN)-1)<<RF_FCAL_STATUS_POS))
#define RF_ACAL_STATUS                                          RF_ACAL_STATUS
#define RF_ACAL_STATUS_POS                                      (6U)
#define RF_ACAL_STATUS_LEN                                      (2U)
#define RF_ACAL_STATUS_MSK                                      (((1U<<RF_ACAL_STATUS_LEN)-1)<<RF_ACAL_STATUS_POS)
#define RF_ACAL_STATUS_UMSK                                     (~(((1U<<RF_ACAL_STATUS_LEN)-1)<<RF_ACAL_STATUS_POS))
#define RF_INC_FCAL_STATUS                                      RF_INC_FCAL_STATUS
#define RF_INC_FCAL_STATUS_POS                                  (8U)
#define RF_INC_FCAL_STATUS_LEN                                  (2U)
#define RF_INC_FCAL_STATUS_MSK                                  (((1U<<RF_INC_FCAL_STATUS_LEN)-1)<<RF_INC_FCAL_STATUS_POS)
#define RF_INC_FCAL_STATUS_UMSK                                 (~(((1U<<RF_INC_FCAL_STATUS_LEN)-1)<<RF_INC_FCAL_STATUS_POS))
#define RF_INC_ACAL_STATUS                                      RF_INC_ACAL_STATUS
#define RF_INC_ACAL_STATUS_POS                                  (10U)
#define RF_INC_ACAL_STATUS_LEN                                  (2U)
#define RF_INC_ACAL_STATUS_MSK                                  (((1U<<RF_INC_ACAL_STATUS_LEN)-1)<<RF_INC_ACAL_STATUS_POS)
#define RF_INC_ACAL_STATUS_UMSK                                 (~(((1U<<RF_INC_ACAL_STATUS_LEN)-1)<<RF_INC_ACAL_STATUS_POS))
#define RF_CLKPLL_CAL_STATUS                                    RF_CLKPLL_CAL_STATUS
#define RF_CLKPLL_CAL_STATUS_POS                                (12U)
#define RF_CLKPLL_CAL_STATUS_LEN                                (2U)
#define RF_CLKPLL_CAL_STATUS_MSK                                (((1U<<RF_CLKPLL_CAL_STATUS_LEN)-1)<<RF_CLKPLL_CAL_STATUS_POS)
#define RF_CLKPLL_CAL_STATUS_UMSK                               (~(((1U<<RF_CLKPLL_CAL_STATUS_LEN)-1)<<RF_CLKPLL_CAL_STATUS_POS))
#define RF_ROS_STATUS                                           RF_ROS_STATUS
#define RF_ROS_STATUS_POS                                       (14U)
#define RF_ROS_STATUS_LEN                                       (2U)
#define RF_ROS_STATUS_MSK                                       (((1U<<RF_ROS_STATUS_LEN)-1)<<RF_ROS_STATUS_POS)
#define RF_ROS_STATUS_UMSK                                      (~(((1U<<RF_ROS_STATUS_LEN)-1)<<RF_ROS_STATUS_POS))
#define RF_TOS_STATUS                                           RF_TOS_STATUS
#define RF_TOS_STATUS_POS                                       (16U)
#define RF_TOS_STATUS_LEN                                       (2U)
#define RF_TOS_STATUS_MSK                                       (((1U<<RF_TOS_STATUS_LEN)-1)<<RF_TOS_STATUS_POS)
#define RF_TOS_STATUS_UMSK                                      (~(((1U<<RF_TOS_STATUS_LEN)-1)<<RF_TOS_STATUS_POS))
#define RF_RCCAL_STATUS                                         RF_RCCAL_STATUS
#define RF_RCCAL_STATUS_POS                                     (18U)
#define RF_RCCAL_STATUS_LEN                                     (2U)
#define RF_RCCAL_STATUS_MSK                                     (((1U<<RF_RCCAL_STATUS_LEN)-1)<<RF_RCCAL_STATUS_POS)
#define RF_RCCAL_STATUS_UMSK                                    (~(((1U<<RF_RCCAL_STATUS_LEN)-1)<<RF_RCCAL_STATUS_POS))
#define RF_LO_LEAKCAL_STATUS                                    RF_LO_LEAKCAL_STATUS
#define RF_LO_LEAKCAL_STATUS_POS                                (20U)
#define RF_LO_LEAKCAL_STATUS_LEN                                (2U)
#define RF_LO_LEAKCAL_STATUS_MSK                                (((1U<<RF_LO_LEAKCAL_STATUS_LEN)-1)<<RF_LO_LEAKCAL_STATUS_POS)
#define RF_LO_LEAKCAL_STATUS_UMSK                               (~(((1U<<RF_LO_LEAKCAL_STATUS_LEN)-1)<<RF_LO_LEAKCAL_STATUS_POS))
#define RF_TIQCAL_STATUS_RESV                                   RF_TIQCAL_STATUS_RESV
#define RF_TIQCAL_STATUS_RESV_POS                               (22U)
#define RF_TIQCAL_STATUS_RESV_LEN                               (2U)
#define RF_TIQCAL_STATUS_RESV_MSK                               (((1U<<RF_TIQCAL_STATUS_RESV_LEN)-1)<<RF_TIQCAL_STATUS_RESV_POS)
#define RF_TIQCAL_STATUS_RESV_UMSK                              (~(((1U<<RF_TIQCAL_STATUS_RESV_LEN)-1)<<RF_TIQCAL_STATUS_RESV_POS))
#define RF_RIQCAL_STATUS_RESV                                   RF_RIQCAL_STATUS_RESV
#define RF_RIQCAL_STATUS_RESV_POS                               (24U)
#define RF_RIQCAL_STATUS_RESV_LEN                               (2U)
#define RF_RIQCAL_STATUS_RESV_MSK                               (((1U<<RF_RIQCAL_STATUS_RESV_LEN)-1)<<RF_RIQCAL_STATUS_RESV_POS)
#define RF_RIQCAL_STATUS_RESV_UMSK                              (~(((1U<<RF_RIQCAL_STATUS_RESV_LEN)-1)<<RF_RIQCAL_STATUS_RESV_POS))
#define RF_PWDET_CAL_STATUS                                     RF_PWDET_CAL_STATUS
#define RF_PWDET_CAL_STATUS_POS                                 (26U)
#define RF_PWDET_CAL_STATUS_LEN                                 (2U)
#define RF_PWDET_CAL_STATUS_MSK                                 (((1U<<RF_PWDET_CAL_STATUS_LEN)-1)<<RF_PWDET_CAL_STATUS_POS)
#define RF_PWDET_CAL_STATUS_UMSK                                (~(((1U<<RF_PWDET_CAL_STATUS_LEN)-1)<<RF_PWDET_CAL_STATUS_POS))
#define RF_TENSCAL_STATUS                                       RF_TENSCAL_STATUS
#define RF_TENSCAL_STATUS_POS                                   (28U)
#define RF_TENSCAL_STATUS_LEN                                   (2U)
#define RF_TENSCAL_STATUS_MSK                                   (((1U<<RF_TENSCAL_STATUS_LEN)-1)<<RF_TENSCAL_STATUS_POS)
#define RF_TENSCAL_STATUS_UMSK                                  (~(((1U<<RF_TENSCAL_STATUS_LEN)-1)<<RF_TENSCAL_STATUS_POS))
#define RF_DPD_STATUS                                           RF_DPD_STATUS
#define RF_DPD_STATUS_POS                                       (30U)
#define RF_DPD_STATUS_LEN                                       (2U)
#define RF_DPD_STATUS_MSK                                       (((1U<<RF_DPD_STATUS_LEN)-1)<<RF_DPD_STATUS_POS)
#define RF_DPD_STATUS_UMSK                                      (~(((1U<<RF_DPD_STATUS_LEN)-1)<<RF_DPD_STATUS_POS))

/* 0x18 : rfcal_status2 */
#define RFCAL_STATUS2_OFFSET                                    (0x18)
#define RF_DL_RFCAL_TABLE_STATUS                                RF_DL_RFCAL_TABLE_STATUS
#define RF_DL_RFCAL_TABLE_STATUS_POS                            (0U)
#define RF_DL_RFCAL_TABLE_STATUS_LEN                            (2U)
#define RF_DL_RFCAL_TABLE_STATUS_MSK                            (((1U<<RF_DL_RFCAL_TABLE_STATUS_LEN)-1)<<RF_DL_RFCAL_TABLE_STATUS_POS)
#define RF_DL_RFCAL_TABLE_STATUS_UMSK                           (~(((1U<<RF_DL_RFCAL_TABLE_STATUS_LEN)-1)<<RF_DL_RFCAL_TABLE_STATUS_POS))

/* 0x1C : Calibration mode register */
#define RFCAL_CTRLEN_OFFSET                                     (0x1C)
#define RF_RCAL_EN_RESV                                         RF_RCAL_EN_RESV
#define RF_RCAL_EN_RESV_POS                                     (0U)
#define RF_RCAL_EN_RESV_LEN                                     (1U)
#define RF_RCAL_EN_RESV_MSK                                     (((1U<<RF_RCAL_EN_RESV_LEN)-1)<<RF_RCAL_EN_RESV_POS)
#define RF_RCAL_EN_RESV_UMSK                                    (~(((1U<<RF_RCAL_EN_RESV_LEN)-1)<<RF_RCAL_EN_RESV_POS))
#define RF_ADC_OSCAL_EN                                         RF_ADC_OSCAL_EN
#define RF_ADC_OSCAL_EN_POS                                     (1U)
#define RF_ADC_OSCAL_EN_LEN                                     (1U)
#define RF_ADC_OSCAL_EN_MSK                                     (((1U<<RF_ADC_OSCAL_EN_LEN)-1)<<RF_ADC_OSCAL_EN_POS)
#define RF_ADC_OSCAL_EN_UMSK                                    (~(((1U<<RF_ADC_OSCAL_EN_LEN)-1)<<RF_ADC_OSCAL_EN_POS))
#define RF_DL_RFCAL_TABLE_EN                                    RF_DL_RFCAL_TABLE_EN
#define RF_DL_RFCAL_TABLE_EN_POS                                (2U)
#define RF_DL_RFCAL_TABLE_EN_LEN                                (1U)
#define RF_DL_RFCAL_TABLE_EN_MSK                                (((1U<<RF_DL_RFCAL_TABLE_EN_LEN)-1)<<RF_DL_RFCAL_TABLE_EN_POS)
#define RF_DL_RFCAL_TABLE_EN_UMSK                               (~(((1U<<RF_DL_RFCAL_TABLE_EN_LEN)-1)<<RF_DL_RFCAL_TABLE_EN_POS))
#define RF_FCAL_EN                                              RF_FCAL_EN
#define RF_FCAL_EN_POS                                          (3U)
#define RF_FCAL_EN_LEN                                          (1U)
#define RF_FCAL_EN_MSK                                          (((1U<<RF_FCAL_EN_LEN)-1)<<RF_FCAL_EN_POS)
#define RF_FCAL_EN_UMSK                                         (~(((1U<<RF_FCAL_EN_LEN)-1)<<RF_FCAL_EN_POS))
#define RF_ACAL_EN                                              RF_ACAL_EN
#define RF_ACAL_EN_POS                                          (4U)
#define RF_ACAL_EN_LEN                                          (1U)
#define RF_ACAL_EN_MSK                                          (((1U<<RF_ACAL_EN_LEN)-1)<<RF_ACAL_EN_POS)
#define RF_ACAL_EN_UMSK                                         (~(((1U<<RF_ACAL_EN_LEN)-1)<<RF_ACAL_EN_POS))
#define RF_FCAL_INC_EN                                          RF_FCAL_INC_EN
#define RF_FCAL_INC_EN_POS                                      (5U)
#define RF_FCAL_INC_EN_LEN                                      (1U)
#define RF_FCAL_INC_EN_MSK                                      (((1U<<RF_FCAL_INC_EN_LEN)-1)<<RF_FCAL_INC_EN_POS)
#define RF_FCAL_INC_EN_UMSK                                     (~(((1U<<RF_FCAL_INC_EN_LEN)-1)<<RF_FCAL_INC_EN_POS))
#define RF_ACAL_INC_EN                                          RF_ACAL_INC_EN
#define RF_ACAL_INC_EN_POS                                      (6U)
#define RF_ACAL_INC_EN_LEN                                      (1U)
#define RF_ACAL_INC_EN_MSK                                      (((1U<<RF_ACAL_INC_EN_LEN)-1)<<RF_ACAL_INC_EN_POS)
#define RF_ACAL_INC_EN_UMSK                                     (~(((1U<<RF_ACAL_INC_EN_LEN)-1)<<RF_ACAL_INC_EN_POS))
#define RF_ROSCAL_INC_EN                                        RF_ROSCAL_INC_EN
#define RF_ROSCAL_INC_EN_POS                                    (7U)
#define RF_ROSCAL_INC_EN_LEN                                    (1U)
#define RF_ROSCAL_INC_EN_MSK                                    (((1U<<RF_ROSCAL_INC_EN_LEN)-1)<<RF_ROSCAL_INC_EN_POS)
#define RF_ROSCAL_INC_EN_UMSK                                   (~(((1U<<RF_ROSCAL_INC_EN_LEN)-1)<<RF_ROSCAL_INC_EN_POS))
#define RF_CLKPLL_CAL_EN                                        RF_CLKPLL_CAL_EN
#define RF_CLKPLL_CAL_EN_POS                                    (8U)
#define RF_CLKPLL_CAL_EN_LEN                                    (1U)
#define RF_CLKPLL_CAL_EN_MSK                                    (((1U<<RF_CLKPLL_CAL_EN_LEN)-1)<<RF_CLKPLL_CAL_EN_POS)
#define RF_CLKPLL_CAL_EN_UMSK                                   (~(((1U<<RF_CLKPLL_CAL_EN_LEN)-1)<<RF_CLKPLL_CAL_EN_POS))
#define RF_ROSCAL_EN                                            RF_ROSCAL_EN
#define RF_ROSCAL_EN_POS                                        (9U)
#define RF_ROSCAL_EN_LEN                                        (1U)
#define RF_ROSCAL_EN_MSK                                        (((1U<<RF_ROSCAL_EN_LEN)-1)<<RF_ROSCAL_EN_POS)
#define RF_ROSCAL_EN_UMSK                                       (~(((1U<<RF_ROSCAL_EN_LEN)-1)<<RF_ROSCAL_EN_POS))
#define RF_TOSCAL_EN                                            RF_TOSCAL_EN
#define RF_TOSCAL_EN_POS                                        (10U)
#define RF_TOSCAL_EN_LEN                                        (1U)
#define RF_TOSCAL_EN_MSK                                        (((1U<<RF_TOSCAL_EN_LEN)-1)<<RF_TOSCAL_EN_POS)
#define RF_TOSCAL_EN_UMSK                                       (~(((1U<<RF_TOSCAL_EN_LEN)-1)<<RF_TOSCAL_EN_POS))
#define RF_RCCAL_EN                                             RF_RCCAL_EN
#define RF_RCCAL_EN_POS                                         (11U)
#define RF_RCCAL_EN_LEN                                         (1U)
#define RF_RCCAL_EN_MSK                                         (((1U<<RF_RCCAL_EN_LEN)-1)<<RF_RCCAL_EN_POS)
#define RF_RCCAL_EN_UMSK                                        (~(((1U<<RF_RCCAL_EN_LEN)-1)<<RF_RCCAL_EN_POS))
#define RF_LO_LEAKCAL_EN                                        RF_LO_LEAKCAL_EN
#define RF_LO_LEAKCAL_EN_POS                                    (12U)
#define RF_LO_LEAKCAL_EN_LEN                                    (1U)
#define RF_LO_LEAKCAL_EN_MSK                                    (((1U<<RF_LO_LEAKCAL_EN_LEN)-1)<<RF_LO_LEAKCAL_EN_POS)
#define RF_LO_LEAKCAL_EN_UMSK                                   (~(((1U<<RF_LO_LEAKCAL_EN_LEN)-1)<<RF_LO_LEAKCAL_EN_POS))
#define RF_TIQCAL_EN                                            RF_TIQCAL_EN
#define RF_TIQCAL_EN_POS                                        (13U)
#define RF_TIQCAL_EN_LEN                                        (1U)
#define RF_TIQCAL_EN_MSK                                        (((1U<<RF_TIQCAL_EN_LEN)-1)<<RF_TIQCAL_EN_POS)
#define RF_TIQCAL_EN_UMSK                                       (~(((1U<<RF_TIQCAL_EN_LEN)-1)<<RF_TIQCAL_EN_POS))
#define RF_RIQCAL_EN                                            RF_RIQCAL_EN
#define RF_RIQCAL_EN_POS                                        (14U)
#define RF_RIQCAL_EN_LEN                                        (1U)
#define RF_RIQCAL_EN_MSK                                        (((1U<<RF_RIQCAL_EN_LEN)-1)<<RF_RIQCAL_EN_POS)
#define RF_RIQCAL_EN_UMSK                                       (~(((1U<<RF_RIQCAL_EN_LEN)-1)<<RF_RIQCAL_EN_POS))
#define RF_PWDET_CAL_EN                                         RF_PWDET_CAL_EN
#define RF_PWDET_CAL_EN_POS                                     (15U)
#define RF_PWDET_CAL_EN_LEN                                     (1U)
#define RF_PWDET_CAL_EN_MSK                                     (((1U<<RF_PWDET_CAL_EN_LEN)-1)<<RF_PWDET_CAL_EN_POS)
#define RF_PWDET_CAL_EN_UMSK                                    (~(((1U<<RF_PWDET_CAL_EN_LEN)-1)<<RF_PWDET_CAL_EN_POS))
#define RF_TSENCAL_EN                                           RF_TSENCAL_EN
#define RF_TSENCAL_EN_POS                                       (16U)
#define RF_TSENCAL_EN_LEN                                       (1U)
#define RF_TSENCAL_EN_MSK                                       (((1U<<RF_TSENCAL_EN_LEN)-1)<<RF_TSENCAL_EN_POS)
#define RF_TSENCAL_EN_UMSK                                      (~(((1U<<RF_TSENCAL_EN_LEN)-1)<<RF_TSENCAL_EN_POS))
#define RF_DPD_EN                                               RF_DPD_EN
#define RF_DPD_EN_POS                                           (17U)
#define RF_DPD_EN_LEN                                           (1U)
#define RF_DPD_EN_MSK                                           (((1U<<RF_DPD_EN_LEN)-1)<<RF_DPD_EN_POS)
#define RF_DPD_EN_UMSK                                          (~(((1U<<RF_DPD_EN_LEN)-1)<<RF_DPD_EN_POS))

/* 0x20 : rf calibration state enabl in full cal list */
#define RFCAL_STATEEN_OFFSET                                    (0x20)
#define RF_RCAL_STEN_RESV                                       RF_RCAL_STEN_RESV
#define RF_RCAL_STEN_RESV_POS                                   (0U)
#define RF_RCAL_STEN_RESV_LEN                                   (1U)
#define RF_RCAL_STEN_RESV_MSK                                   (((1U<<RF_RCAL_STEN_RESV_LEN)-1)<<RF_RCAL_STEN_RESV_POS)
#define RF_RCAL_STEN_RESV_UMSK                                  (~(((1U<<RF_RCAL_STEN_RESV_LEN)-1)<<RF_RCAL_STEN_RESV_POS))
#define RF_ADC_OSCAL_STEN                                       RF_ADC_OSCAL_STEN
#define RF_ADC_OSCAL_STEN_POS                                   (1U)
#define RF_ADC_OSCAL_STEN_LEN                                   (1U)
#define RF_ADC_OSCAL_STEN_MSK                                   (((1U<<RF_ADC_OSCAL_STEN_LEN)-1)<<RF_ADC_OSCAL_STEN_POS)
#define RF_ADC_OSCAL_STEN_UMSK                                  (~(((1U<<RF_ADC_OSCAL_STEN_LEN)-1)<<RF_ADC_OSCAL_STEN_POS))
#define RF_DL_RFCAL_TABLE_STEN                                  RF_DL_RFCAL_TABLE_STEN
#define RF_DL_RFCAL_TABLE_STEN_POS                              (2U)
#define RF_DL_RFCAL_TABLE_STEN_LEN                              (1U)
#define RF_DL_RFCAL_TABLE_STEN_MSK                              (((1U<<RF_DL_RFCAL_TABLE_STEN_LEN)-1)<<RF_DL_RFCAL_TABLE_STEN_POS)
#define RF_DL_RFCAL_TABLE_STEN_UMSK                             (~(((1U<<RF_DL_RFCAL_TABLE_STEN_LEN)-1)<<RF_DL_RFCAL_TABLE_STEN_POS))
#define RF_FCAL_STEN                                            RF_FCAL_STEN
#define RF_FCAL_STEN_POS                                        (3U)
#define RF_FCAL_STEN_LEN                                        (1U)
#define RF_FCAL_STEN_MSK                                        (((1U<<RF_FCAL_STEN_LEN)-1)<<RF_FCAL_STEN_POS)
#define RF_FCAL_STEN_UMSK                                       (~(((1U<<RF_FCAL_STEN_LEN)-1)<<RF_FCAL_STEN_POS))
#define RF_ACAL_STEN                                            RF_ACAL_STEN
#define RF_ACAL_STEN_POS                                        (4U)
#define RF_ACAL_STEN_LEN                                        (1U)
#define RF_ACAL_STEN_MSK                                        (((1U<<RF_ACAL_STEN_LEN)-1)<<RF_ACAL_STEN_POS)
#define RF_ACAL_STEN_UMSK                                       (~(((1U<<RF_ACAL_STEN_LEN)-1)<<RF_ACAL_STEN_POS))
#define RF_INC_FCAL_STEN                                        RF_INC_FCAL_STEN
#define RF_INC_FCAL_STEN_POS                                    (5U)
#define RF_INC_FCAL_STEN_LEN                                    (1U)
#define RF_INC_FCAL_STEN_MSK                                    (((1U<<RF_INC_FCAL_STEN_LEN)-1)<<RF_INC_FCAL_STEN_POS)
#define RF_INC_FCAL_STEN_UMSK                                   (~(((1U<<RF_INC_FCAL_STEN_LEN)-1)<<RF_INC_FCAL_STEN_POS))
#define RF_INC_ACAL_STEN                                        RF_INC_ACAL_STEN
#define RF_INC_ACAL_STEN_POS                                    (6U)
#define RF_INC_ACAL_STEN_LEN                                    (1U)
#define RF_INC_ACAL_STEN_MSK                                    (((1U<<RF_INC_ACAL_STEN_LEN)-1)<<RF_INC_ACAL_STEN_POS)
#define RF_INC_ACAL_STEN_UMSK                                   (~(((1U<<RF_INC_ACAL_STEN_LEN)-1)<<RF_INC_ACAL_STEN_POS))
#define RF_CLKPLL_CAL_STEN                                      RF_CLKPLL_CAL_STEN
#define RF_CLKPLL_CAL_STEN_POS                                  (7U)
#define RF_CLKPLL_CAL_STEN_LEN                                  (1U)
#define RF_CLKPLL_CAL_STEN_MSK                                  (((1U<<RF_CLKPLL_CAL_STEN_LEN)-1)<<RF_CLKPLL_CAL_STEN_POS)
#define RF_CLKPLL_CAL_STEN_UMSK                                 (~(((1U<<RF_CLKPLL_CAL_STEN_LEN)-1)<<RF_CLKPLL_CAL_STEN_POS))
#define RF_ROSCAL_STEN                                          RF_ROSCAL_STEN
#define RF_ROSCAL_STEN_POS                                      (8U)
#define RF_ROSCAL_STEN_LEN                                      (1U)
#define RF_ROSCAL_STEN_MSK                                      (((1U<<RF_ROSCAL_STEN_LEN)-1)<<RF_ROSCAL_STEN_POS)
#define RF_ROSCAL_STEN_UMSK                                     (~(((1U<<RF_ROSCAL_STEN_LEN)-1)<<RF_ROSCAL_STEN_POS))
#define RF_TOSCAL_STEN_RESV                                     RF_TOSCAL_STEN_RESV
#define RF_TOSCAL_STEN_RESV_POS                                 (9U)
#define RF_TOSCAL_STEN_RESV_LEN                                 (1U)
#define RF_TOSCAL_STEN_RESV_MSK                                 (((1U<<RF_TOSCAL_STEN_RESV_LEN)-1)<<RF_TOSCAL_STEN_RESV_POS)
#define RF_TOSCAL_STEN_RESV_UMSK                                (~(((1U<<RF_TOSCAL_STEN_RESV_LEN)-1)<<RF_TOSCAL_STEN_RESV_POS))
#define RF_RCCAL_STEN                                           RF_RCCAL_STEN
#define RF_RCCAL_STEN_POS                                       (10U)
#define RF_RCCAL_STEN_LEN                                       (1U)
#define RF_RCCAL_STEN_MSK                                       (((1U<<RF_RCCAL_STEN_LEN)-1)<<RF_RCCAL_STEN_POS)
#define RF_RCCAL_STEN_UMSK                                      (~(((1U<<RF_RCCAL_STEN_LEN)-1)<<RF_RCCAL_STEN_POS))
#define RF_LO_LEAKCAL_STEN                                      RF_LO_LEAKCAL_STEN
#define RF_LO_LEAKCAL_STEN_POS                                  (11U)
#define RF_LO_LEAKCAL_STEN_LEN                                  (1U)
#define RF_LO_LEAKCAL_STEN_MSK                                  (((1U<<RF_LO_LEAKCAL_STEN_LEN)-1)<<RF_LO_LEAKCAL_STEN_POS)
#define RF_LO_LEAKCAL_STEN_UMSK                                 (~(((1U<<RF_LO_LEAKCAL_STEN_LEN)-1)<<RF_LO_LEAKCAL_STEN_POS))
#define RF_TIQCAL_STEN                                          RF_TIQCAL_STEN
#define RF_TIQCAL_STEN_POS                                      (12U)
#define RF_TIQCAL_STEN_LEN                                      (1U)
#define RF_TIQCAL_STEN_MSK                                      (((1U<<RF_TIQCAL_STEN_LEN)-1)<<RF_TIQCAL_STEN_POS)
#define RF_TIQCAL_STEN_UMSK                                     (~(((1U<<RF_TIQCAL_STEN_LEN)-1)<<RF_TIQCAL_STEN_POS))
#define RF_RIQCAL_STEN                                          RF_RIQCAL_STEN
#define RF_RIQCAL_STEN_POS                                      (13U)
#define RF_RIQCAL_STEN_LEN                                      (1U)
#define RF_RIQCAL_STEN_MSK                                      (((1U<<RF_RIQCAL_STEN_LEN)-1)<<RF_RIQCAL_STEN_POS)
#define RF_RIQCAL_STEN_UMSK                                     (~(((1U<<RF_RIQCAL_STEN_LEN)-1)<<RF_RIQCAL_STEN_POS))
#define RF_PWDET_CAL_STEN                                       RF_PWDET_CAL_STEN
#define RF_PWDET_CAL_STEN_POS                                   (14U)
#define RF_PWDET_CAL_STEN_LEN                                   (1U)
#define RF_PWDET_CAL_STEN_MSK                                   (((1U<<RF_PWDET_CAL_STEN_LEN)-1)<<RF_PWDET_CAL_STEN_POS)
#define RF_PWDET_CAL_STEN_UMSK                                  (~(((1U<<RF_PWDET_CAL_STEN_LEN)-1)<<RF_PWDET_CAL_STEN_POS))
#define RF_TSENCAL_STEN                                         RF_TSENCAL_STEN
#define RF_TSENCAL_STEN_POS                                     (15U)
#define RF_TSENCAL_STEN_LEN                                     (1U)
#define RF_TSENCAL_STEN_MSK                                     (((1U<<RF_TSENCAL_STEN_LEN)-1)<<RF_TSENCAL_STEN_POS)
#define RF_TSENCAL_STEN_UMSK                                    (~(((1U<<RF_TSENCAL_STEN_LEN)-1)<<RF_TSENCAL_STEN_POS))
#define RF_DPD_STEN                                             RF_DPD_STEN
#define RF_DPD_STEN_POS                                         (16U)
#define RF_DPD_STEN_LEN                                         (1U)
#define RF_DPD_STEN_MSK                                         (((1U<<RF_DPD_STEN_LEN)-1)<<RF_DPD_STEN_POS)
#define RF_DPD_STEN_UMSK                                        (~(((1U<<RF_DPD_STEN_LEN)-1)<<RF_DPD_STEN_POS))
#define RFCAL_LEVEL                                             RFCAL_LEVEL
#define RFCAL_LEVEL_POS                                         (30U)
#define RFCAL_LEVEL_LEN                                         (2U)
#define RFCAL_LEVEL_MSK                                         (((1U<<RFCAL_LEVEL_LEN)-1)<<RFCAL_LEVEL_POS)
#define RFCAL_LEVEL_UMSK                                        (~(((1U<<RFCAL_LEVEL_LEN)-1)<<RFCAL_LEVEL_POS))

/* 0x24 : SARADC Control Registers */
#define RF_SARADC_RESV_OFFSET                                   (0x24)

/* 0x28 : ZRF Control register 0 */
#define RF_BASE_CTRL1_OFFSET                                    (0x28)
#define RF_AUPLL_SDM_RST_DLY                                    RF_AUPLL_SDM_RST_DLY
#define RF_AUPLL_SDM_RST_DLY_POS                                (0U)
#define RF_AUPLL_SDM_RST_DLY_LEN                                (2U)
#define RF_AUPLL_SDM_RST_DLY_MSK                                (((1U<<RF_AUPLL_SDM_RST_DLY_LEN)-1)<<RF_AUPLL_SDM_RST_DLY_POS)
#define RF_AUPLL_SDM_RST_DLY_UMSK                               (~(((1U<<RF_AUPLL_SDM_RST_DLY_LEN)-1)<<RF_AUPLL_SDM_RST_DLY_POS))
#define RF_LO_SDM_RST_DLY                                       RF_LO_SDM_RST_DLY
#define RF_LO_SDM_RST_DLY_POS                                   (2U)
#define RF_LO_SDM_RST_DLY_LEN                                   (2U)
#define RF_LO_SDM_RST_DLY_MSK                                   (((1U<<RF_LO_SDM_RST_DLY_LEN)-1)<<RF_LO_SDM_RST_DLY_POS)
#define RF_LO_SDM_RST_DLY_UMSK                                  (~(((1U<<RF_LO_SDM_RST_DLY_LEN)-1)<<RF_LO_SDM_RST_DLY_POS))
#define RF_PPU_LEAD                                             RF_PPU_LEAD
#define RF_PPU_LEAD_POS                                         (8U)
#define RF_PPU_LEAD_LEN                                         (2U)
#define RF_PPU_LEAD_MSK                                         (((1U<<RF_PPU_LEAD_LEN)-1)<<RF_PPU_LEAD_POS)
#define RF_PPU_LEAD_UMSK                                        (~(((1U<<RF_PPU_LEAD_LEN)-1)<<RF_PPU_LEAD_POS))
#define RF_PUD_VCO_DLY                                          RF_PUD_VCO_DLY
#define RF_PUD_VCO_DLY_POS                                      (10U)
#define RF_PUD_VCO_DLY_LEN                                      (2U)
#define RF_PUD_VCO_DLY_MSK                                      (((1U<<RF_PUD_VCO_DLY_LEN)-1)<<RF_PUD_VCO_DLY_POS)
#define RF_PUD_VCO_DLY_UMSK                                     (~(((1U<<RF_PUD_VCO_DLY_LEN)-1)<<RF_PUD_VCO_DLY_POS))
#define RF_PUD_IREF_DLY                                         RF_PUD_IREF_DLY
#define RF_PUD_IREF_DLY_POS                                     (12U)
#define RF_PUD_IREF_DLY_LEN                                     (2U)
#define RF_PUD_IREF_DLY_MSK                                     (((1U<<RF_PUD_IREF_DLY_LEN)-1)<<RF_PUD_IREF_DLY_POS)
#define RF_PUD_IREF_DLY_UMSK                                    (~(((1U<<RF_PUD_IREF_DLY_LEN)-1)<<RF_PUD_IREF_DLY_POS))
#define RF_PUD_PA_DLY                                           RF_PUD_PA_DLY
#define RF_PUD_PA_DLY_POS                                       (14U)
#define RF_PUD_PA_DLY_LEN                                       (2U)
#define RF_PUD_PA_DLY_MSK                                       (((1U<<RF_PUD_PA_DLY_LEN)-1)<<RF_PUD_PA_DLY_POS)
#define RF_PUD_PA_DLY_UMSK                                      (~(((1U<<RF_PUD_PA_DLY_LEN)-1)<<RF_PUD_PA_DLY_POS))
#define RF_MBG_TRIM                                             RF_MBG_TRIM
#define RF_MBG_TRIM_POS                                         (27U)
#define RF_MBG_TRIM_LEN                                         (2U)
#define RF_MBG_TRIM_MSK                                         (((1U<<RF_MBG_TRIM_LEN)-1)<<RF_MBG_TRIM_POS)
#define RF_MBG_TRIM_UMSK                                        (~(((1U<<RF_MBG_TRIM_LEN)-1)<<RF_MBG_TRIM_POS))

/* 0x2C : ZRF Control register 0 */
#define RF_BASE_CTRL2_OFFSET                                    (0x2C)

/* 0x30 : pucr1 */
#define RF_PUCR1_OFFSET                                         (0x30)
#define RF_PU_SFREG                                             RF_PU_SFREG
#define RF_PU_SFREG_POS                                         (0U)
#define RF_PU_SFREG_LEN                                         (1U)
#define RF_PU_SFREG_MSK                                         (((1U<<RF_PU_SFREG_LEN)-1)<<RF_PU_SFREG_POS)
#define RF_PU_SFREG_UMSK                                        (~(((1U<<RF_PU_SFREG_LEN)-1)<<RF_PU_SFREG_POS))
#define RF_PU_LNA                                               RF_PU_LNA
#define RF_PU_LNA_POS                                           (8U)
#define RF_PU_LNA_LEN                                           (1U)
#define RF_PU_LNA_MSK                                           (((1U<<RF_PU_LNA_LEN)-1)<<RF_PU_LNA_POS)
#define RF_PU_LNA_UMSK                                          (~(((1U<<RF_PU_LNA_LEN)-1)<<RF_PU_LNA_POS))
#define RF_PU_RMXGM                                             RF_PU_RMXGM
#define RF_PU_RMXGM_POS                                         (9U)
#define RF_PU_RMXGM_LEN                                         (1U)
#define RF_PU_RMXGM_MSK                                         (((1U<<RF_PU_RMXGM_LEN)-1)<<RF_PU_RMXGM_POS)
#define RF_PU_RMXGM_UMSK                                        (~(((1U<<RF_PU_RMXGM_LEN)-1)<<RF_PU_RMXGM_POS))
#define RF_PU_RMX                                               RF_PU_RMX
#define RF_PU_RMX_POS                                           (10U)
#define RF_PU_RMX_LEN                                           (1U)
#define RF_PU_RMX_MSK                                           (((1U<<RF_PU_RMX_LEN)-1)<<RF_PU_RMX_POS)
#define RF_PU_RMX_UMSK                                          (~(((1U<<RF_PU_RMX_LEN)-1)<<RF_PU_RMX_POS))
#define RF_PU_RBB                                               RF_PU_RBB
#define RF_PU_RBB_POS                                           (11U)
#define RF_PU_RBB_LEN                                           (1U)
#define RF_PU_RBB_MSK                                           (((1U<<RF_PU_RBB_LEN)-1)<<RF_PU_RBB_POS)
#define RF_PU_RBB_UMSK                                          (~(((1U<<RF_PU_RBB_LEN)-1)<<RF_PU_RBB_POS))
#define RF_PU_ADDA_LDO                                          RF_PU_ADDA_LDO
#define RF_PU_ADDA_LDO_POS                                      (12U)
#define RF_PU_ADDA_LDO_LEN                                      (1U)
#define RF_PU_ADDA_LDO_MSK                                      (((1U<<RF_PU_ADDA_LDO_LEN)-1)<<RF_PU_ADDA_LDO_POS)
#define RF_PU_ADDA_LDO_UMSK                                     (~(((1U<<RF_PU_ADDA_LDO_LEN)-1)<<RF_PU_ADDA_LDO_POS))
#define RF_ADC_CLK_EN                                           RF_ADC_CLK_EN
#define RF_ADC_CLK_EN_POS                                       (13U)
#define RF_ADC_CLK_EN_LEN                                       (1U)
#define RF_ADC_CLK_EN_MSK                                       (((1U<<RF_ADC_CLK_EN_LEN)-1)<<RF_ADC_CLK_EN_POS)
#define RF_ADC_CLK_EN_UMSK                                      (~(((1U<<RF_ADC_CLK_EN_LEN)-1)<<RF_ADC_CLK_EN_POS))
#define RF_PU_ADC                                               RF_PU_ADC
#define RF_PU_ADC_POS                                           (14U)
#define RF_PU_ADC_LEN                                           (1U)
#define RF_PU_ADC_MSK                                           (((1U<<RF_PU_ADC_LEN)-1)<<RF_PU_ADC_POS)
#define RF_PU_ADC_UMSK                                          (~(((1U<<RF_PU_ADC_LEN)-1)<<RF_PU_ADC_POS))
#define RF_PU_OP_ATEST                                          RF_PU_OP_ATEST
#define RF_PU_OP_ATEST_POS                                      (15U)
#define RF_PU_OP_ATEST_LEN                                      (1U)
#define RF_PU_OP_ATEST_MSK                                      (((1U<<RF_PU_OP_ATEST_LEN)-1)<<RF_PU_OP_ATEST_POS)
#define RF_PU_OP_ATEST_UMSK                                     (~(((1U<<RF_PU_OP_ATEST_LEN)-1)<<RF_PU_OP_ATEST_POS))
#define RF_PU_PA                                                RF_PU_PA
#define RF_PU_PA_POS                                            (16U)
#define RF_PU_PA_LEN                                            (1U)
#define RF_PU_PA_MSK                                            (((1U<<RF_PU_PA_LEN)-1)<<RF_PU_PA_POS)
#define RF_PU_PA_UMSK                                           (~(((1U<<RF_PU_PA_LEN)-1)<<RF_PU_PA_POS))
#define RF_PU_TMX                                               RF_PU_TMX
#define RF_PU_TMX_POS                                           (17U)
#define RF_PU_TMX_LEN                                           (1U)
#define RF_PU_TMX_MSK                                           (((1U<<RF_PU_TMX_LEN)-1)<<RF_PU_TMX_POS)
#define RF_PU_TMX_UMSK                                          (~(((1U<<RF_PU_TMX_LEN)-1)<<RF_PU_TMX_POS))
#define RF_PU_TBB                                               RF_PU_TBB
#define RF_PU_TBB_POS                                           (18U)
#define RF_PU_TBB_LEN                                           (1U)
#define RF_PU_TBB_MSK                                           (((1U<<RF_PU_TBB_LEN)-1)<<RF_PU_TBB_POS)
#define RF_PU_TBB_UMSK                                          (~(((1U<<RF_PU_TBB_LEN)-1)<<RF_PU_TBB_POS))
#define RF_PU_DAC                                               RF_PU_DAC
#define RF_PU_DAC_POS                                           (19U)
#define RF_PU_DAC_LEN                                           (1U)
#define RF_PU_DAC_MSK                                           (((1U<<RF_PU_DAC_LEN)-1)<<RF_PU_DAC_POS)
#define RF_PU_DAC_UMSK                                          (~(((1U<<RF_PU_DAC_LEN)-1)<<RF_PU_DAC_POS))
#define RF_PU_VCO                                               RF_PU_VCO
#define RF_PU_VCO_POS                                           (20U)
#define RF_PU_VCO_LEN                                           (1U)
#define RF_PU_VCO_MSK                                           (((1U<<RF_PU_VCO_LEN)-1)<<RF_PU_VCO_POS)
#define RF_PU_VCO_UMSK                                          (~(((1U<<RF_PU_VCO_LEN)-1)<<RF_PU_VCO_POS))
#define RF_PU_FBDV                                              RF_PU_FBDV
#define RF_PU_FBDV_POS                                          (21U)
#define RF_PU_FBDV_LEN                                          (1U)
#define RF_PU_FBDV_MSK                                          (((1U<<RF_PU_FBDV_LEN)-1)<<RF_PU_FBDV_POS)
#define RF_PU_FBDV_UMSK                                         (~(((1U<<RF_PU_FBDV_LEN)-1)<<RF_PU_FBDV_POS))
#define RF_PU_PFD                                               RF_PU_PFD
#define RF_PU_PFD_POS                                           (22U)
#define RF_PU_PFD_LEN                                           (1U)
#define RF_PU_PFD_MSK                                           (((1U<<RF_PU_PFD_LEN)-1)<<RF_PU_PFD_POS)
#define RF_PU_PFD_UMSK                                          (~(((1U<<RF_PU_PFD_LEN)-1)<<RF_PU_PFD_POS))
#define RF_PU_OSMX                                              RF_PU_OSMX
#define RF_PU_OSMX_POS                                          (23U)
#define RF_PU_OSMX_LEN                                          (1U)
#define RF_PU_OSMX_MSK                                          (((1U<<RF_PU_OSMX_LEN)-1)<<RF_PU_OSMX_POS)
#define RF_PU_OSMX_UMSK                                         (~(((1U<<RF_PU_OSMX_LEN)-1)<<RF_PU_OSMX_POS))
#define RF_PU_RXBUF                                             RF_PU_RXBUF
#define RF_PU_RXBUF_POS                                         (24U)
#define RF_PU_RXBUF_LEN                                         (1U)
#define RF_PU_RXBUF_MSK                                         (((1U<<RF_PU_RXBUF_LEN)-1)<<RF_PU_RXBUF_POS)
#define RF_PU_RXBUF_UMSK                                        (~(((1U<<RF_PU_RXBUF_LEN)-1)<<RF_PU_RXBUF_POS))
#define RF_PU_TXBUF                                             RF_PU_TXBUF
#define RF_PU_TXBUF_POS                                         (25U)
#define RF_PU_TXBUF_LEN                                         (1U)
#define RF_PU_TXBUF_MSK                                         (((1U<<RF_PU_TXBUF_LEN)-1)<<RF_PU_TXBUF_POS)
#define RF_PU_TXBUF_UMSK                                        (~(((1U<<RF_PU_TXBUF_LEN)-1)<<RF_PU_TXBUF_POS))
#define RF_TRSW_EN                                              RF_TRSW_EN
#define RF_TRSW_EN_POS                                          (26U)
#define RF_TRSW_EN_LEN                                          (1U)
#define RF_TRSW_EN_MSK                                          (((1U<<RF_TRSW_EN_LEN)-1)<<RF_TRSW_EN_POS)
#define RF_TRSW_EN_UMSK                                         (~(((1U<<RF_TRSW_EN_LEN)-1)<<RF_TRSW_EN_POS))
#define RF_PU_PKDET                                             RF_PU_PKDET
#define RF_PU_PKDET_POS                                         (28U)
#define RF_PU_PKDET_LEN                                         (1U)
#define RF_PU_PKDET_MSK                                         (((1U<<RF_PU_PKDET_LEN)-1)<<RF_PU_PKDET_POS)
#define RF_PU_PKDET_UMSK                                        (~(((1U<<RF_PU_PKDET_LEN)-1)<<RF_PU_PKDET_POS))
#define RF_PU_ROSDAC                                            RF_PU_ROSDAC
#define RF_PU_ROSDAC_POS                                        (29U)
#define RF_PU_ROSDAC_LEN                                        (1U)
#define RF_PU_ROSDAC_MSK                                        (((1U<<RF_PU_ROSDAC_LEN)-1)<<RF_PU_ROSDAC_POS)
#define RF_PU_ROSDAC_UMSK                                       (~(((1U<<RF_PU_ROSDAC_LEN)-1)<<RF_PU_ROSDAC_POS))
#define RF_PU_PWRMX                                             RF_PU_PWRMX
#define RF_PU_PWRMX_POS                                         (30U)
#define RF_PU_PWRMX_LEN                                         (1U)
#define RF_PU_PWRMX_MSK                                         (((1U<<RF_PU_PWRMX_LEN)-1)<<RF_PU_PWRMX_POS)
#define RF_PU_PWRMX_UMSK                                        (~(((1U<<RF_PU_PWRMX_LEN)-1)<<RF_PU_PWRMX_POS))
#define RF_PU_TOSDAC                                            RF_PU_TOSDAC
#define RF_PU_TOSDAC_POS                                        (31U)
#define RF_PU_TOSDAC_LEN                                        (1U)
#define RF_PU_TOSDAC_MSK                                        (((1U<<RF_PU_TOSDAC_LEN)-1)<<RF_PU_TOSDAC_POS)
#define RF_PU_TOSDAC_UMSK                                       (~(((1U<<RF_PU_TOSDAC_LEN)-1)<<RF_PU_TOSDAC_POS))

/* 0x34 : read only from hardware logic */
#define RF_PUCR1_HW_OFFSET                                      (0x34)
#define RF_PU_SFREG_HW                                          RF_PU_SFREG_HW
#define RF_PU_SFREG_HW_POS                                      (0U)
#define RF_PU_SFREG_HW_LEN                                      (1U)
#define RF_PU_SFREG_HW_MSK                                      (((1U<<RF_PU_SFREG_HW_LEN)-1)<<RF_PU_SFREG_HW_POS)
#define RF_PU_SFREG_HW_UMSK                                     (~(((1U<<RF_PU_SFREG_HW_LEN)-1)<<RF_PU_SFREG_HW_POS))
#define RF_PU_LNA_HW                                            RF_PU_LNA_HW
#define RF_PU_LNA_HW_POS                                        (8U)
#define RF_PU_LNA_HW_LEN                                        (1U)
#define RF_PU_LNA_HW_MSK                                        (((1U<<RF_PU_LNA_HW_LEN)-1)<<RF_PU_LNA_HW_POS)
#define RF_PU_LNA_HW_UMSK                                       (~(((1U<<RF_PU_LNA_HW_LEN)-1)<<RF_PU_LNA_HW_POS))
#define RF_PU_RMXGM_HW                                          RF_PU_RMXGM_HW
#define RF_PU_RMXGM_HW_POS                                      (9U)
#define RF_PU_RMXGM_HW_LEN                                      (1U)
#define RF_PU_RMXGM_HW_MSK                                      (((1U<<RF_PU_RMXGM_HW_LEN)-1)<<RF_PU_RMXGM_HW_POS)
#define RF_PU_RMXGM_HW_UMSK                                     (~(((1U<<RF_PU_RMXGM_HW_LEN)-1)<<RF_PU_RMXGM_HW_POS))
#define RF_PU_RMX_HW                                            RF_PU_RMX_HW
#define RF_PU_RMX_HW_POS                                        (10U)
#define RF_PU_RMX_HW_LEN                                        (1U)
#define RF_PU_RMX_HW_MSK                                        (((1U<<RF_PU_RMX_HW_LEN)-1)<<RF_PU_RMX_HW_POS)
#define RF_PU_RMX_HW_UMSK                                       (~(((1U<<RF_PU_RMX_HW_LEN)-1)<<RF_PU_RMX_HW_POS))
#define RF_PU_RBB_HW                                            RF_PU_RBB_HW
#define RF_PU_RBB_HW_POS                                        (11U)
#define RF_PU_RBB_HW_LEN                                        (1U)
#define RF_PU_RBB_HW_MSK                                        (((1U<<RF_PU_RBB_HW_LEN)-1)<<RF_PU_RBB_HW_POS)
#define RF_PU_RBB_HW_UMSK                                       (~(((1U<<RF_PU_RBB_HW_LEN)-1)<<RF_PU_RBB_HW_POS))
#define RF_PU_ADDA_LDO_HW                                       RF_PU_ADDA_LDO_HW
#define RF_PU_ADDA_LDO_HW_POS                                   (12U)
#define RF_PU_ADDA_LDO_HW_LEN                                   (1U)
#define RF_PU_ADDA_LDO_HW_MSK                                   (((1U<<RF_PU_ADDA_LDO_HW_LEN)-1)<<RF_PU_ADDA_LDO_HW_POS)
#define RF_PU_ADDA_LDO_HW_UMSK                                  (~(((1U<<RF_PU_ADDA_LDO_HW_LEN)-1)<<RF_PU_ADDA_LDO_HW_POS))
#define RF_ADC_CLK_EN_HW                                        RF_ADC_CLK_EN_HW
#define RF_ADC_CLK_EN_HW_POS                                    (13U)
#define RF_ADC_CLK_EN_HW_LEN                                    (1U)
#define RF_ADC_CLK_EN_HW_MSK                                    (((1U<<RF_ADC_CLK_EN_HW_LEN)-1)<<RF_ADC_CLK_EN_HW_POS)
#define RF_ADC_CLK_EN_HW_UMSK                                   (~(((1U<<RF_ADC_CLK_EN_HW_LEN)-1)<<RF_ADC_CLK_EN_HW_POS))
#define RF_PU_ADC_HW                                            RF_PU_ADC_HW
#define RF_PU_ADC_HW_POS                                        (14U)
#define RF_PU_ADC_HW_LEN                                        (1U)
#define RF_PU_ADC_HW_MSK                                        (((1U<<RF_PU_ADC_HW_LEN)-1)<<RF_PU_ADC_HW_POS)
#define RF_PU_ADC_HW_UMSK                                       (~(((1U<<RF_PU_ADC_HW_LEN)-1)<<RF_PU_ADC_HW_POS))
#define RF_PU_PA_HW                                             RF_PU_PA_HW
#define RF_PU_PA_HW_POS                                         (16U)
#define RF_PU_PA_HW_LEN                                         (1U)
#define RF_PU_PA_HW_MSK                                         (((1U<<RF_PU_PA_HW_LEN)-1)<<RF_PU_PA_HW_POS)
#define RF_PU_PA_HW_UMSK                                        (~(((1U<<RF_PU_PA_HW_LEN)-1)<<RF_PU_PA_HW_POS))
#define RF_PU_TMX_HW                                            RF_PU_TMX_HW
#define RF_PU_TMX_HW_POS                                        (17U)
#define RF_PU_TMX_HW_LEN                                        (1U)
#define RF_PU_TMX_HW_MSK                                        (((1U<<RF_PU_TMX_HW_LEN)-1)<<RF_PU_TMX_HW_POS)
#define RF_PU_TMX_HW_UMSK                                       (~(((1U<<RF_PU_TMX_HW_LEN)-1)<<RF_PU_TMX_HW_POS))
#define RF_PU_TBB_HW                                            RF_PU_TBB_HW
#define RF_PU_TBB_HW_POS                                        (18U)
#define RF_PU_TBB_HW_LEN                                        (1U)
#define RF_PU_TBB_HW_MSK                                        (((1U<<RF_PU_TBB_HW_LEN)-1)<<RF_PU_TBB_HW_POS)
#define RF_PU_TBB_HW_UMSK                                       (~(((1U<<RF_PU_TBB_HW_LEN)-1)<<RF_PU_TBB_HW_POS))
#define RF_PU_DAC_HW                                            RF_PU_DAC_HW
#define RF_PU_DAC_HW_POS                                        (19U)
#define RF_PU_DAC_HW_LEN                                        (1U)
#define RF_PU_DAC_HW_MSK                                        (((1U<<RF_PU_DAC_HW_LEN)-1)<<RF_PU_DAC_HW_POS)
#define RF_PU_DAC_HW_UMSK                                       (~(((1U<<RF_PU_DAC_HW_LEN)-1)<<RF_PU_DAC_HW_POS))
#define RF_PU_VCO_HW                                            RF_PU_VCO_HW
#define RF_PU_VCO_HW_POS                                        (20U)
#define RF_PU_VCO_HW_LEN                                        (1U)
#define RF_PU_VCO_HW_MSK                                        (((1U<<RF_PU_VCO_HW_LEN)-1)<<RF_PU_VCO_HW_POS)
#define RF_PU_VCO_HW_UMSK                                       (~(((1U<<RF_PU_VCO_HW_LEN)-1)<<RF_PU_VCO_HW_POS))
#define RF_PU_FBDV_HW                                           RF_PU_FBDV_HW
#define RF_PU_FBDV_HW_POS                                       (21U)
#define RF_PU_FBDV_HW_LEN                                       (1U)
#define RF_PU_FBDV_HW_MSK                                       (((1U<<RF_PU_FBDV_HW_LEN)-1)<<RF_PU_FBDV_HW_POS)
#define RF_PU_FBDV_HW_UMSK                                      (~(((1U<<RF_PU_FBDV_HW_LEN)-1)<<RF_PU_FBDV_HW_POS))
#define RF_PU_PFD_HW                                            RF_PU_PFD_HW
#define RF_PU_PFD_HW_POS                                        (22U)
#define RF_PU_PFD_HW_LEN                                        (1U)
#define RF_PU_PFD_HW_MSK                                        (((1U<<RF_PU_PFD_HW_LEN)-1)<<RF_PU_PFD_HW_POS)
#define RF_PU_PFD_HW_UMSK                                       (~(((1U<<RF_PU_PFD_HW_LEN)-1)<<RF_PU_PFD_HW_POS))
#define RF_PU_OSMX_HW                                           RF_PU_OSMX_HW
#define RF_PU_OSMX_HW_POS                                       (23U)
#define RF_PU_OSMX_HW_LEN                                       (1U)
#define RF_PU_OSMX_HW_MSK                                       (((1U<<RF_PU_OSMX_HW_LEN)-1)<<RF_PU_OSMX_HW_POS)
#define RF_PU_OSMX_HW_UMSK                                      (~(((1U<<RF_PU_OSMX_HW_LEN)-1)<<RF_PU_OSMX_HW_POS))
#define RF_PU_RXBUF_HW                                          RF_PU_RXBUF_HW
#define RF_PU_RXBUF_HW_POS                                      (24U)
#define RF_PU_RXBUF_HW_LEN                                      (1U)
#define RF_PU_RXBUF_HW_MSK                                      (((1U<<RF_PU_RXBUF_HW_LEN)-1)<<RF_PU_RXBUF_HW_POS)
#define RF_PU_RXBUF_HW_UMSK                                     (~(((1U<<RF_PU_RXBUF_HW_LEN)-1)<<RF_PU_RXBUF_HW_POS))
#define RF_PU_TXBUF_HW                                          RF_PU_TXBUF_HW
#define RF_PU_TXBUF_HW_POS                                      (25U)
#define RF_PU_TXBUF_HW_LEN                                      (1U)
#define RF_PU_TXBUF_HW_MSK                                      (((1U<<RF_PU_TXBUF_HW_LEN)-1)<<RF_PU_TXBUF_HW_POS)
#define RF_PU_TXBUF_HW_UMSK                                     (~(((1U<<RF_PU_TXBUF_HW_LEN)-1)<<RF_PU_TXBUF_HW_POS))
#define RF_TRSW_EN_HW                                           RF_TRSW_EN_HW
#define RF_TRSW_EN_HW_POS                                       (26U)
#define RF_TRSW_EN_HW_LEN                                       (1U)
#define RF_TRSW_EN_HW_MSK                                       (((1U<<RF_TRSW_EN_HW_LEN)-1)<<RF_TRSW_EN_HW_POS)
#define RF_TRSW_EN_HW_UMSK                                      (~(((1U<<RF_TRSW_EN_HW_LEN)-1)<<RF_TRSW_EN_HW_POS))
#define RF_PU_PKDET_HW                                          RF_PU_PKDET_HW
#define RF_PU_PKDET_HW_POS                                      (28U)
#define RF_PU_PKDET_HW_LEN                                      (1U)
#define RF_PU_PKDET_HW_MSK                                      (((1U<<RF_PU_PKDET_HW_LEN)-1)<<RF_PU_PKDET_HW_POS)
#define RF_PU_PKDET_HW_UMSK                                     (~(((1U<<RF_PU_PKDET_HW_LEN)-1)<<RF_PU_PKDET_HW_POS))
#define RF_PU_ROSDAC_HW                                         RF_PU_ROSDAC_HW
#define RF_PU_ROSDAC_HW_POS                                     (29U)
#define RF_PU_ROSDAC_HW_LEN                                     (1U)
#define RF_PU_ROSDAC_HW_MSK                                     (((1U<<RF_PU_ROSDAC_HW_LEN)-1)<<RF_PU_ROSDAC_HW_POS)
#define RF_PU_ROSDAC_HW_UMSK                                    (~(((1U<<RF_PU_ROSDAC_HW_LEN)-1)<<RF_PU_ROSDAC_HW_POS))
#define RF_PU_TOSDAC_HW                                         RF_PU_TOSDAC_HW
#define RF_PU_TOSDAC_HW_POS                                     (31U)
#define RF_PU_TOSDAC_HW_LEN                                     (1U)
#define RF_PU_TOSDAC_HW_MSK                                     (((1U<<RF_PU_TOSDAC_HW_LEN)-1)<<RF_PU_TOSDAC_HW_POS)
#define RF_PU_TOSDAC_HW_UMSK                                    (~(((1U<<RF_PU_TOSDAC_HW_LEN)-1)<<RF_PU_TOSDAC_HW_POS))

/* 0x38 : pucr2 */
#define RF_PUCR2_OFFSET                                         (0x38)

/* 0x3C : pucr2_hw */
#define RF_PUCR2_HW_OFFSET                                      (0x3C)

/* 0x40 : ppu_ctrl_hw */
#define RF_PPU_CTRL_HW_OFFSET                                   (0x40)
#define RF_PPU_LNA_HW                                           RF_PPU_LNA_HW
#define RF_PPU_LNA_HW_POS                                       (8U)
#define RF_PPU_LNA_HW_LEN                                       (1U)
#define RF_PPU_LNA_HW_MSK                                       (((1U<<RF_PPU_LNA_HW_LEN)-1)<<RF_PPU_LNA_HW_POS)
#define RF_PPU_LNA_HW_UMSK                                      (~(((1U<<RF_PPU_LNA_HW_LEN)-1)<<RF_PPU_LNA_HW_POS))
#define RF_PPU_RMXGM_HW                                         RF_PPU_RMXGM_HW
#define RF_PPU_RMXGM_HW_POS                                     (9U)
#define RF_PPU_RMXGM_HW_LEN                                     (1U)
#define RF_PPU_RMXGM_HW_MSK                                     (((1U<<RF_PPU_RMXGM_HW_LEN)-1)<<RF_PPU_RMXGM_HW_POS)
#define RF_PPU_RMXGM_HW_UMSK                                    (~(((1U<<RF_PPU_RMXGM_HW_LEN)-1)<<RF_PPU_RMXGM_HW_POS))
#define RF_PPU_RBB_HW                                           RF_PPU_RBB_HW
#define RF_PPU_RBB_HW_POS                                       (11U)
#define RF_PPU_RBB_HW_LEN                                       (1U)
#define RF_PPU_RBB_HW_MSK                                       (((1U<<RF_PPU_RBB_HW_LEN)-1)<<RF_PPU_RBB_HW_POS)
#define RF_PPU_RBB_HW_UMSK                                      (~(((1U<<RF_PPU_RBB_HW_LEN)-1)<<RF_PPU_RBB_HW_POS))
#define RF_PPU_VCO_HW                                           RF_PPU_VCO_HW
#define RF_PPU_VCO_HW_POS                                       (20U)
#define RF_PPU_VCO_HW_LEN                                       (1U)
#define RF_PPU_VCO_HW_MSK                                       (((1U<<RF_PPU_VCO_HW_LEN)-1)<<RF_PPU_VCO_HW_POS)
#define RF_PPU_VCO_HW_UMSK                                      (~(((1U<<RF_PPU_VCO_HW_LEN)-1)<<RF_PPU_VCO_HW_POS))
#define RF_PPU_FBDV_HW                                          RF_PPU_FBDV_HW
#define RF_PPU_FBDV_HW_POS                                      (21U)
#define RF_PPU_FBDV_HW_LEN                                      (1U)
#define RF_PPU_FBDV_HW_MSK                                      (((1U<<RF_PPU_FBDV_HW_LEN)-1)<<RF_PPU_FBDV_HW_POS)
#define RF_PPU_FBDV_HW_UMSK                                     (~(((1U<<RF_PPU_FBDV_HW_LEN)-1)<<RF_PPU_FBDV_HW_POS))
#define RF_PPU_PFD_HW                                           RF_PPU_PFD_HW
#define RF_PPU_PFD_HW_POS                                       (22U)
#define RF_PPU_PFD_HW_LEN                                       (1U)
#define RF_PPU_PFD_HW_MSK                                       (((1U<<RF_PPU_PFD_HW_LEN)-1)<<RF_PPU_PFD_HW_POS)
#define RF_PPU_PFD_HW_UMSK                                      (~(((1U<<RF_PPU_PFD_HW_LEN)-1)<<RF_PPU_PFD_HW_POS))
#define RF_PPU_OSMX_HW                                          RF_PPU_OSMX_HW
#define RF_PPU_OSMX_HW_POS                                      (23U)
#define RF_PPU_OSMX_HW_LEN                                      (1U)
#define RF_PPU_OSMX_HW_MSK                                      (((1U<<RF_PPU_OSMX_HW_LEN)-1)<<RF_PPU_OSMX_HW_POS)
#define RF_PPU_OSMX_HW_UMSK                                     (~(((1U<<RF_PPU_OSMX_HW_LEN)-1)<<RF_PPU_OSMX_HW_POS))
#define RF_PPU_RXBUF_HW                                         RF_PPU_RXBUF_HW
#define RF_PPU_RXBUF_HW_POS                                     (24U)
#define RF_PPU_RXBUF_HW_LEN                                     (1U)
#define RF_PPU_RXBUF_HW_MSK                                     (((1U<<RF_PPU_RXBUF_HW_LEN)-1)<<RF_PPU_RXBUF_HW_POS)
#define RF_PPU_RXBUF_HW_UMSK                                    (~(((1U<<RF_PPU_RXBUF_HW_LEN)-1)<<RF_PPU_RXBUF_HW_POS))
#define RF_PPU_TXBUF_HW                                         RF_PPU_TXBUF_HW
#define RF_PPU_TXBUF_HW_POS                                     (25U)
#define RF_PPU_TXBUF_HW_LEN                                     (1U)
#define RF_PPU_TXBUF_HW_MSK                                     (((1U<<RF_PPU_TXBUF_HW_LEN)-1)<<RF_PPU_TXBUF_HW_POS)
#define RF_PPU_TXBUF_HW_UMSK                                    (~(((1U<<RF_PPU_TXBUF_HW_LEN)-1)<<RF_PPU_TXBUF_HW_POS))

/* 0x44 : pud_ctrl_hw */
#define RF_PUD_CTRL_HW_OFFSET                                   (0x44)
#define RF_PUD_VCO_HW                                           RF_PUD_VCO_HW
#define RF_PUD_VCO_HW_POS                                       (20U)
#define RF_PUD_VCO_HW_LEN                                       (1U)
#define RF_PUD_VCO_HW_MSK                                       (((1U<<RF_PUD_VCO_HW_LEN)-1)<<RF_PUD_VCO_HW_POS)
#define RF_PUD_VCO_HW_UMSK                                      (~(((1U<<RF_PUD_VCO_HW_LEN)-1)<<RF_PUD_VCO_HW_POS))

/* 0x48 : gain control1 */
#define RF_TRX_GAIN1_OFFSET                                     (0x48)
#define RF_GC_LNA                                               RF_GC_LNA
#define RF_GC_LNA_POS                                           (0U)
#define RF_GC_LNA_LEN                                           (3U)
#define RF_GC_LNA_MSK                                           (((1U<<RF_GC_LNA_LEN)-1)<<RF_GC_LNA_POS)
#define RF_GC_LNA_UMSK                                          (~(((1U<<RF_GC_LNA_LEN)-1)<<RF_GC_LNA_POS))
#define RF_GC_RMXGM                                             RF_GC_RMXGM
#define RF_GC_RMXGM_POS                                         (4U)
#define RF_GC_RMXGM_LEN                                         (2U)
#define RF_GC_RMXGM_MSK                                         (((1U<<RF_GC_RMXGM_LEN)-1)<<RF_GC_RMXGM_POS)
#define RF_GC_RMXGM_UMSK                                        (~(((1U<<RF_GC_RMXGM_LEN)-1)<<RF_GC_RMXGM_POS))
#define RF_GC_RBB1                                              RF_GC_RBB1
#define RF_GC_RBB1_POS                                          (8U)
#define RF_GC_RBB1_LEN                                          (2U)
#define RF_GC_RBB1_MSK                                          (((1U<<RF_GC_RBB1_LEN)-1)<<RF_GC_RBB1_POS)
#define RF_GC_RBB1_UMSK                                         (~(((1U<<RF_GC_RBB1_LEN)-1)<<RF_GC_RBB1_POS))
#define RF_GC_RBB2                                              RF_GC_RBB2
#define RF_GC_RBB2_POS                                          (12U)
#define RF_GC_RBB2_LEN                                          (3U)
#define RF_GC_RBB2_MSK                                          (((1U<<RF_GC_RBB2_LEN)-1)<<RF_GC_RBB2_POS)
#define RF_GC_RBB2_UMSK                                         (~(((1U<<RF_GC_RBB2_LEN)-1)<<RF_GC_RBB2_POS))
#define RF_GC_TMX                                               RF_GC_TMX
#define RF_GC_TMX_POS                                           (16U)
#define RF_GC_TMX_LEN                                           (3U)
#define RF_GC_TMX_MSK                                           (((1U<<RF_GC_TMX_LEN)-1)<<RF_GC_TMX_POS)
#define RF_GC_TMX_UMSK                                          (~(((1U<<RF_GC_TMX_LEN)-1)<<RF_GC_TMX_POS))
#define RF_GC_TBB                                               RF_GC_TBB
#define RF_GC_TBB_POS                                           (20U)
#define RF_GC_TBB_LEN                                           (5U)
#define RF_GC_TBB_MSK                                           (((1U<<RF_GC_TBB_LEN)-1)<<RF_GC_TBB_POS)
#define RF_GC_TBB_UMSK                                          (~(((1U<<RF_GC_TBB_LEN)-1)<<RF_GC_TBB_POS))
#define RF_GC_TBB_BOOST                                         RF_GC_TBB_BOOST
#define RF_GC_TBB_BOOST_POS                                     (28U)
#define RF_GC_TBB_BOOST_LEN                                     (2U)
#define RF_GC_TBB_BOOST_MSK                                     (((1U<<RF_GC_TBB_BOOST_LEN)-1)<<RF_GC_TBB_BOOST_POS)
#define RF_GC_TBB_BOOST_UMSK                                    (~(((1U<<RF_GC_TBB_BOOST_LEN)-1)<<RF_GC_TBB_BOOST_POS))

/* 0x4C : trx gain hardware readback */
#define RF_TRX_GAIN_HW_OFFSET                                   (0x4C)
#define RF_GC_LNA_HW                                            RF_GC_LNA_HW
#define RF_GC_LNA_HW_POS                                        (0U)
#define RF_GC_LNA_HW_LEN                                        (3U)
#define RF_GC_LNA_HW_MSK                                        (((1U<<RF_GC_LNA_HW_LEN)-1)<<RF_GC_LNA_HW_POS)
#define RF_GC_LNA_HW_UMSK                                       (~(((1U<<RF_GC_LNA_HW_LEN)-1)<<RF_GC_LNA_HW_POS))
#define RF_GC_RMXGM_HW                                          RF_GC_RMXGM_HW
#define RF_GC_RMXGM_HW_POS                                      (4U)
#define RF_GC_RMXGM_HW_LEN                                      (2U)
#define RF_GC_RMXGM_HW_MSK                                      (((1U<<RF_GC_RMXGM_HW_LEN)-1)<<RF_GC_RMXGM_HW_POS)
#define RF_GC_RMXGM_HW_UMSK                                     (~(((1U<<RF_GC_RMXGM_HW_LEN)-1)<<RF_GC_RMXGM_HW_POS))
#define RF_GC_RBB1_HW                                           RF_GC_RBB1_HW
#define RF_GC_RBB1_HW_POS                                       (8U)
#define RF_GC_RBB1_HW_LEN                                       (2U)
#define RF_GC_RBB1_HW_MSK                                       (((1U<<RF_GC_RBB1_HW_LEN)-1)<<RF_GC_RBB1_HW_POS)
#define RF_GC_RBB1_HW_UMSK                                      (~(((1U<<RF_GC_RBB1_HW_LEN)-1)<<RF_GC_RBB1_HW_POS))
#define RF_GC_RBB2_HW                                           RF_GC_RBB2_HW
#define RF_GC_RBB2_HW_POS                                       (12U)
#define RF_GC_RBB2_HW_LEN                                       (3U)
#define RF_GC_RBB2_HW_MSK                                       (((1U<<RF_GC_RBB2_HW_LEN)-1)<<RF_GC_RBB2_HW_POS)
#define RF_GC_RBB2_HW_UMSK                                      (~(((1U<<RF_GC_RBB2_HW_LEN)-1)<<RF_GC_RBB2_HW_POS))
#define RF_GC_TMX_HW                                            RF_GC_TMX_HW
#define RF_GC_TMX_HW_POS                                        (16U)
#define RF_GC_TMX_HW_LEN                                        (3U)
#define RF_GC_TMX_HW_MSK                                        (((1U<<RF_GC_TMX_HW_LEN)-1)<<RF_GC_TMX_HW_POS)
#define RF_GC_TMX_HW_UMSK                                       (~(((1U<<RF_GC_TMX_HW_LEN)-1)<<RF_GC_TMX_HW_POS))
#define RF_GC_TBB_HW                                            RF_GC_TBB_HW
#define RF_GC_TBB_HW_POS                                        (20U)
#define RF_GC_TBB_HW_LEN                                        (5U)
#define RF_GC_TBB_HW_MSK                                        (((1U<<RF_GC_TBB_HW_LEN)-1)<<RF_GC_TBB_HW_POS)
#define RF_GC_TBB_HW_UMSK                                       (~(((1U<<RF_GC_TBB_HW_LEN)-1)<<RF_GC_TBB_HW_POS))
#define RF_GC_TBB_BOOST_HW                                      RF_GC_TBB_BOOST_HW
#define RF_GC_TBB_BOOST_HW_POS                                  (28U)
#define RF_GC_TBB_BOOST_HW_LEN                                  (2U)
#define RF_GC_TBB_BOOST_HW_MSK                                  (((1U<<RF_GC_TBB_BOOST_HW_LEN)-1)<<RF_GC_TBB_BOOST_HW_POS)
#define RF_GC_TBB_BOOST_HW_UMSK                                 (~(((1U<<RF_GC_TBB_BOOST_HW_LEN)-1)<<RF_GC_TBB_BOOST_HW_POS))

/* 0x50 : dc test register */
#define RF_TEN_DC_OFFSET                                        (0x50)
#define RF_TMUX                                                 RF_TMUX
#define RF_TMUX_POS                                             (0U)
#define RF_TMUX_LEN                                             (3U)
#define RF_TMUX_MSK                                             (((1U<<RF_TMUX_LEN)-1)<<RF_TMUX_POS)
#define RF_TMUX_UMSK                                            (~(((1U<<RF_TMUX_LEN)-1)<<RF_TMUX_POS))
#define RF_DC_TP_EN                                             RF_DC_TP_EN
#define RF_DC_TP_EN_POS                                         (3U)
#define RF_DC_TP_EN_LEN                                         (1U)
#define RF_DC_TP_EN_MSK                                         (((1U<<RF_DC_TP_EN_LEN)-1)<<RF_DC_TP_EN_POS)
#define RF_DC_TP_EN_UMSK                                        (~(((1U<<RF_DC_TP_EN_LEN)-1)<<RF_DC_TP_EN_POS))
#define RF_DC_TP_CLKPLL_EN                                      RF_DC_TP_CLKPLL_EN
#define RF_DC_TP_CLKPLL_EN_POS                                  (4U)
#define RF_DC_TP_CLKPLL_EN_LEN                                  (1U)
#define RF_DC_TP_CLKPLL_EN_MSK                                  (((1U<<RF_DC_TP_CLKPLL_EN_LEN)-1)<<RF_DC_TP_CLKPLL_EN_POS)
#define RF_DC_TP_CLKPLL_EN_UMSK                                 (~(((1U<<RF_DC_TP_CLKPLL_EN_LEN)-1)<<RF_DC_TP_CLKPLL_EN_POS))
#define RF_TEN_CLKPLL                                           RF_TEN_CLKPLL
#define RF_TEN_CLKPLL_POS                                       (8U)
#define RF_TEN_CLKPLL_LEN                                       (1U)
#define RF_TEN_CLKPLL_MSK                                       (((1U<<RF_TEN_CLKPLL_LEN)-1)<<RF_TEN_CLKPLL_POS)
#define RF_TEN_CLKPLL_UMSK                                      (~(((1U<<RF_TEN_CLKPLL_LEN)-1)<<RF_TEN_CLKPLL_POS))
#define RF_TEN_CLKPLL_SFREG                                     RF_TEN_CLKPLL_SFREG
#define RF_TEN_CLKPLL_SFREG_POS                                 (9U)
#define RF_TEN_CLKPLL_SFREG_LEN                                 (1U)
#define RF_TEN_CLKPLL_SFREG_MSK                                 (((1U<<RF_TEN_CLKPLL_SFREG_LEN)-1)<<RF_TEN_CLKPLL_SFREG_POS)
#define RF_TEN_CLKPLL_SFREG_UMSK                                (~(((1U<<RF_TEN_CLKPLL_SFREG_LEN)-1)<<RF_TEN_CLKPLL_SFREG_POS))
#define RF_TEN_RRF_0                                            RF_TEN_RRF_0
#define RF_TEN_RRF_0_POS                                        (12U)
#define RF_TEN_RRF_0_LEN                                        (1U)
#define RF_TEN_RRF_0_MSK                                        (((1U<<RF_TEN_RRF_0_LEN)-1)<<RF_TEN_RRF_0_POS)
#define RF_TEN_RRF_0_UMSK                                       (~(((1U<<RF_TEN_RRF_0_LEN)-1)<<RF_TEN_RRF_0_POS))
#define RF_TEN_RRF_1                                            RF_TEN_RRF_1
#define RF_TEN_RRF_1_POS                                        (13U)
#define RF_TEN_RRF_1_LEN                                        (1U)
#define RF_TEN_RRF_1_MSK                                        (((1U<<RF_TEN_RRF_1_LEN)-1)<<RF_TEN_RRF_1_POS)
#define RF_TEN_RRF_1_UMSK                                       (~(((1U<<RF_TEN_RRF_1_LEN)-1)<<RF_TEN_RRF_1_POS))
#define RF_TEN_PA                                               RF_TEN_PA
#define RF_TEN_PA_POS                                           (14U)
#define RF_TEN_PA_LEN                                           (1U)
#define RF_TEN_PA_MSK                                           (((1U<<RF_TEN_PA_LEN)-1)<<RF_TEN_PA_POS)
#define RF_TEN_PA_UMSK                                          (~(((1U<<RF_TEN_PA_LEN)-1)<<RF_TEN_PA_POS))
#define RF_TEN_TMX                                              RF_TEN_TMX
#define RF_TEN_TMX_POS                                          (15U)
#define RF_TEN_TMX_LEN                                          (1U)
#define RF_TEN_TMX_MSK                                          (((1U<<RF_TEN_TMX_LEN)-1)<<RF_TEN_TMX_POS)
#define RF_TEN_TMX_UMSK                                         (~(((1U<<RF_TEN_TMX_LEN)-1)<<RF_TEN_TMX_POS))
#define RF_TEN_TIA                                              RF_TEN_TIA
#define RF_TEN_TIA_POS                                          (16U)
#define RF_TEN_TIA_LEN                                          (1U)
#define RF_TEN_TIA_MSK                                          (((1U<<RF_TEN_TIA_LEN)-1)<<RF_TEN_TIA_POS)
#define RF_TEN_TIA_UMSK                                         (~(((1U<<RF_TEN_TIA_LEN)-1)<<RF_TEN_TIA_POS))
#define RF_TEN_BQ                                               RF_TEN_BQ
#define RF_TEN_BQ_POS                                           (17U)
#define RF_TEN_BQ_LEN                                           (1U)
#define RF_TEN_BQ_MSK                                           (((1U<<RF_TEN_BQ_LEN)-1)<<RF_TEN_BQ_POS)
#define RF_TEN_BQ_UMSK                                          (~(((1U<<RF_TEN_BQ_LEN)-1)<<RF_TEN_BQ_POS))
#define RF_TEN_ATEST                                            RF_TEN_ATEST
#define RF_TEN_ATEST_POS                                        (18U)
#define RF_TEN_ATEST_LEN                                        (1U)
#define RF_TEN_ATEST_MSK                                        (((1U<<RF_TEN_ATEST_LEN)-1)<<RF_TEN_ATEST_POS)
#define RF_TEN_ATEST_UMSK                                       (~(((1U<<RF_TEN_ATEST_LEN)-1)<<RF_TEN_ATEST_POS))
#define RF_TEN_TBB                                              RF_TEN_TBB
#define RF_TEN_TBB_POS                                          (19U)
#define RF_TEN_TBB_LEN                                          (1U)
#define RF_TEN_TBB_MSK                                          (((1U<<RF_TEN_TBB_LEN)-1)<<RF_TEN_TBB_POS)
#define RF_TEN_TBB_UMSK                                         (~(((1U<<RF_TEN_TBB_LEN)-1)<<RF_TEN_TBB_POS))
#define RF_TEN_ADC                                              RF_TEN_ADC
#define RF_TEN_ADC_POS                                          (20U)
#define RF_TEN_ADC_LEN                                          (1U)
#define RF_TEN_ADC_MSK                                          (((1U<<RF_TEN_ADC_LEN)-1)<<RF_TEN_ADC_POS)
#define RF_TEN_ADC_UMSK                                         (~(((1U<<RF_TEN_ADC_LEN)-1)<<RF_TEN_ADC_POS))
#define RF_TEN_DAC_I                                            RF_TEN_DAC_I
#define RF_TEN_DAC_I_POS                                        (21U)
#define RF_TEN_DAC_I_LEN                                        (1U)
#define RF_TEN_DAC_I_MSK                                        (((1U<<RF_TEN_DAC_I_LEN)-1)<<RF_TEN_DAC_I_POS)
#define RF_TEN_DAC_I_UMSK                                       (~(((1U<<RF_TEN_DAC_I_LEN)-1)<<RF_TEN_DAC_I_POS))
#define RF_TEN_DAC_Q                                            RF_TEN_DAC_Q
#define RF_TEN_DAC_Q_POS                                        (22U)
#define RF_TEN_DAC_Q_LEN                                        (1U)
#define RF_TEN_DAC_Q_MSK                                        (((1U<<RF_TEN_DAC_Q_LEN)-1)<<RF_TEN_DAC_Q_POS)
#define RF_TEN_DAC_Q_UMSK                                       (~(((1U<<RF_TEN_DAC_Q_LEN)-1)<<RF_TEN_DAC_Q_POS))
#define RF_TEN_VCO                                              RF_TEN_VCO
#define RF_TEN_VCO_POS                                          (24U)
#define RF_TEN_VCO_LEN                                          (1U)
#define RF_TEN_VCO_MSK                                          (((1U<<RF_TEN_VCO_LEN)-1)<<RF_TEN_VCO_POS)
#define RF_TEN_VCO_UMSK                                         (~(((1U<<RF_TEN_VCO_LEN)-1)<<RF_TEN_VCO_POS))
#define RF_TEN_PFDCP                                            RF_TEN_PFDCP
#define RF_TEN_PFDCP_POS                                        (25U)
#define RF_TEN_PFDCP_LEN                                        (1U)
#define RF_TEN_PFDCP_MSK                                        (((1U<<RF_TEN_PFDCP_LEN)-1)<<RF_TEN_PFDCP_POS)
#define RF_TEN_PFDCP_UMSK                                       (~(((1U<<RF_TEN_PFDCP_LEN)-1)<<RF_TEN_PFDCP_POS))
#define RF_TEN_LF                                               RF_TEN_LF
#define RF_TEN_LF_POS                                           (26U)
#define RF_TEN_LF_LEN                                           (1U)
#define RF_TEN_LF_MSK                                           (((1U<<RF_TEN_LF_LEN)-1)<<RF_TEN_LF_POS)
#define RF_TEN_LF_UMSK                                          (~(((1U<<RF_TEN_LF_LEN)-1)<<RF_TEN_LF_POS))
#define RF_TEN_LODIST                                           RF_TEN_LODIST
#define RF_TEN_LODIST_POS                                       (27U)
#define RF_TEN_LODIST_LEN                                       (1U)
#define RF_TEN_LODIST_MSK                                       (((1U<<RF_TEN_LODIST_LEN)-1)<<RF_TEN_LODIST_POS)
#define RF_TEN_LODIST_UMSK                                      (~(((1U<<RF_TEN_LODIST_LEN)-1)<<RF_TEN_LODIST_POS))

/* 0x54 : digital test register */
#define RF_TEN_DIG_OFFSET                                       (0x54)
#define RF_DTEN_CLKPLL_POSTDIV_CLK                              RF_DTEN_CLKPLL_POSTDIV_CLK
#define RF_DTEN_CLKPLL_POSTDIV_CLK_POS                          (0U)
#define RF_DTEN_CLKPLL_POSTDIV_CLK_LEN                          (1U)
#define RF_DTEN_CLKPLL_POSTDIV_CLK_MSK                          (((1U<<RF_DTEN_CLKPLL_POSTDIV_CLK_LEN)-1)<<RF_DTEN_CLKPLL_POSTDIV_CLK_POS)
#define RF_DTEN_CLKPLL_POSTDIV_CLK_UMSK                         (~(((1U<<RF_DTEN_CLKPLL_POSTDIV_CLK_LEN)-1)<<RF_DTEN_CLKPLL_POSTDIV_CLK_POS))
#define RF_DTEN_CLKPLL_CLK96M                                   RF_DTEN_CLKPLL_CLK96M
#define RF_DTEN_CLKPLL_CLK96M_POS                               (1U)
#define RF_DTEN_CLKPLL_CLK96M_LEN                               (1U)
#define RF_DTEN_CLKPLL_CLK96M_MSK                               (((1U<<RF_DTEN_CLKPLL_CLK96M_LEN)-1)<<RF_DTEN_CLKPLL_CLK96M_POS)
#define RF_DTEN_CLKPLL_CLK96M_UMSK                              (~(((1U<<RF_DTEN_CLKPLL_CLK96M_LEN)-1)<<RF_DTEN_CLKPLL_CLK96M_POS))
#define RF_DTEN_CLKPLL_CLK32M                                   RF_DTEN_CLKPLL_CLK32M
#define RF_DTEN_CLKPLL_CLK32M_POS                               (2U)
#define RF_DTEN_CLKPLL_CLK32M_LEN                               (1U)
#define RF_DTEN_CLKPLL_CLK32M_MSK                               (((1U<<RF_DTEN_CLKPLL_CLK32M_LEN)-1)<<RF_DTEN_CLKPLL_CLK32M_POS)
#define RF_DTEN_CLKPLL_CLK32M_UMSK                              (~(((1U<<RF_DTEN_CLKPLL_CLK32M_LEN)-1)<<RF_DTEN_CLKPLL_CLK32M_POS))
#define RF_DTEN_CLKPLL_FSDM                                     RF_DTEN_CLKPLL_FSDM
#define RF_DTEN_CLKPLL_FSDM_POS                                 (3U)
#define RF_DTEN_CLKPLL_FSDM_LEN                                 (1U)
#define RF_DTEN_CLKPLL_FSDM_MSK                                 (((1U<<RF_DTEN_CLKPLL_FSDM_LEN)-1)<<RF_DTEN_CLKPLL_FSDM_POS)
#define RF_DTEN_CLKPLL_FSDM_UMSK                                (~(((1U<<RF_DTEN_CLKPLL_FSDM_LEN)-1)<<RF_DTEN_CLKPLL_FSDM_POS))
#define RF_DTEN_CLKPLL_FREF                                     RF_DTEN_CLKPLL_FREF
#define RF_DTEN_CLKPLL_FREF_POS                                 (4U)
#define RF_DTEN_CLKPLL_FREF_LEN                                 (1U)
#define RF_DTEN_CLKPLL_FREF_MSK                                 (((1U<<RF_DTEN_CLKPLL_FREF_LEN)-1)<<RF_DTEN_CLKPLL_FREF_POS)
#define RF_DTEN_CLKPLL_FREF_UMSK                                (~(((1U<<RF_DTEN_CLKPLL_FREF_LEN)-1)<<RF_DTEN_CLKPLL_FREF_POS))
#define RF_DTEN_CLKPLL_FIN                                      RF_DTEN_CLKPLL_FIN
#define RF_DTEN_CLKPLL_FIN_POS                                  (5U)
#define RF_DTEN_CLKPLL_FIN_LEN                                  (1U)
#define RF_DTEN_CLKPLL_FIN_MSK                                  (((1U<<RF_DTEN_CLKPLL_FIN_LEN)-1)<<RF_DTEN_CLKPLL_FIN_POS)
#define RF_DTEN_CLKPLL_FIN_UMSK                                 (~(((1U<<RF_DTEN_CLKPLL_FIN_LEN)-1)<<RF_DTEN_CLKPLL_FIN_POS))
#define RF_DTEN_LO_FSDM                                         RF_DTEN_LO_FSDM
#define RF_DTEN_LO_FSDM_POS                                     (6U)
#define RF_DTEN_LO_FSDM_LEN                                     (1U)
#define RF_DTEN_LO_FSDM_MSK                                     (((1U<<RF_DTEN_LO_FSDM_LEN)-1)<<RF_DTEN_LO_FSDM_POS)
#define RF_DTEN_LO_FSDM_UMSK                                    (~(((1U<<RF_DTEN_LO_FSDM_LEN)-1)<<RF_DTEN_LO_FSDM_POS))
#define RF_DTEN_LO_FREF                                         RF_DTEN_LO_FREF
#define RF_DTEN_LO_FREF_POS                                     (8U)
#define RF_DTEN_LO_FREF_LEN                                     (1U)
#define RF_DTEN_LO_FREF_MSK                                     (((1U<<RF_DTEN_LO_FREF_LEN)-1)<<RF_DTEN_LO_FREF_POS)
#define RF_DTEN_LO_FREF_UMSK                                    (~(((1U<<RF_DTEN_LO_FREF_LEN)-1)<<RF_DTEN_LO_FREF_POS))
#define RF_DTEST_PULL_DOWN                                      RF_DTEST_PULL_DOWN
#define RF_DTEST_PULL_DOWN_POS                                  (9U)
#define RF_DTEST_PULL_DOWN_LEN                                  (1U)
#define RF_DTEST_PULL_DOWN_MSK                                  (((1U<<RF_DTEST_PULL_DOWN_LEN)-1)<<RF_DTEST_PULL_DOWN_POS)
#define RF_DTEST_PULL_DOWN_UMSK                                 (~(((1U<<RF_DTEST_PULL_DOWN_LEN)-1)<<RF_DTEST_PULL_DOWN_POS))
#define RF_DTEST_EN                                             RF_DTEST_EN
#define RF_DTEST_EN_POS                                         (23U)
#define RF_DTEST_EN_LEN                                         (1U)
#define RF_DTEST_EN_MSK                                         (((1U<<RF_DTEST_EN_LEN)-1)<<RF_DTEST_EN_POS)
#define RF_DTEST_EN_UMSK                                        (~(((1U<<RF_DTEST_EN_LEN)-1)<<RF_DTEST_EN_POS))

/* 0x58 : ac test register */
#define RF_TEN_AC_OFFSET                                        (0x58)
#define RF_ATEST_OP_CC                                          RF_ATEST_OP_CC
#define RF_ATEST_OP_CC_POS                                      (0U)
#define RF_ATEST_OP_CC_LEN                                      (4U)
#define RF_ATEST_OP_CC_MSK                                      (((1U<<RF_ATEST_OP_CC_LEN)-1)<<RF_ATEST_OP_CC_POS)
#define RF_ATEST_OP_CC_UMSK                                     (~(((1U<<RF_ATEST_OP_CC_LEN)-1)<<RF_ATEST_OP_CC_POS))
#define RF_ATEST_DAC_EN                                         RF_ATEST_DAC_EN
#define RF_ATEST_DAC_EN_POS                                     (4U)
#define RF_ATEST_DAC_EN_LEN                                     (1U)
#define RF_ATEST_DAC_EN_MSK                                     (((1U<<RF_ATEST_DAC_EN_LEN)-1)<<RF_ATEST_DAC_EN_POS)
#define RF_ATEST_DAC_EN_UMSK                                    (~(((1U<<RF_ATEST_DAC_EN_LEN)-1)<<RF_ATEST_DAC_EN_POS))
#define RF_ATEST_IN_TRX_SW                                      RF_ATEST_IN_TRX_SW
#define RF_ATEST_IN_TRX_SW_POS                                  (5U)
#define RF_ATEST_IN_TRX_SW_LEN                                  (1U)
#define RF_ATEST_IN_TRX_SW_MSK                                  (((1U<<RF_ATEST_IN_TRX_SW_LEN)-1)<<RF_ATEST_IN_TRX_SW_POS)
#define RF_ATEST_IN_TRX_SW_UMSK                                 (~(((1U<<RF_ATEST_IN_TRX_SW_LEN)-1)<<RF_ATEST_IN_TRX_SW_POS))
#define RF_ATEST_IN_EN                                          RF_ATEST_IN_EN
#define RF_ATEST_IN_EN_POS                                      (6U)
#define RF_ATEST_IN_EN_LEN                                      (1U)
#define RF_ATEST_IN_EN_MSK                                      (((1U<<RF_ATEST_IN_EN_LEN)-1)<<RF_ATEST_IN_EN_POS)
#define RF_ATEST_IN_EN_UMSK                                     (~(((1U<<RF_ATEST_IN_EN_LEN)-1)<<RF_ATEST_IN_EN_POS))
#define RF_ATEST_GAIN_R9                                        RF_ATEST_GAIN_R9
#define RF_ATEST_GAIN_R9_POS                                    (8U)
#define RF_ATEST_GAIN_R9_LEN                                    (2U)
#define RF_ATEST_GAIN_R9_MSK                                    (((1U<<RF_ATEST_GAIN_R9_LEN)-1)<<RF_ATEST_GAIN_R9_POS)
#define RF_ATEST_GAIN_R9_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R9_LEN)-1)<<RF_ATEST_GAIN_R9_POS))
#define RF_ATEST_GAIN_R8                                        RF_ATEST_GAIN_R8
#define RF_ATEST_GAIN_R8_POS                                    (10U)
#define RF_ATEST_GAIN_R8_LEN                                    (2U)
#define RF_ATEST_GAIN_R8_MSK                                    (((1U<<RF_ATEST_GAIN_R8_LEN)-1)<<RF_ATEST_GAIN_R8_POS)
#define RF_ATEST_GAIN_R8_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R8_LEN)-1)<<RF_ATEST_GAIN_R8_POS))
#define RF_ATEST_GAIN_R7                                        RF_ATEST_GAIN_R7
#define RF_ATEST_GAIN_R7_POS                                    (12U)
#define RF_ATEST_GAIN_R7_LEN                                    (2U)
#define RF_ATEST_GAIN_R7_MSK                                    (((1U<<RF_ATEST_GAIN_R7_LEN)-1)<<RF_ATEST_GAIN_R7_POS)
#define RF_ATEST_GAIN_R7_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R7_LEN)-1)<<RF_ATEST_GAIN_R7_POS))
#define RF_ATEST_GAIN_R6                                        RF_ATEST_GAIN_R6
#define RF_ATEST_GAIN_R6_POS                                    (14U)
#define RF_ATEST_GAIN_R6_LEN                                    (2U)
#define RF_ATEST_GAIN_R6_MSK                                    (((1U<<RF_ATEST_GAIN_R6_LEN)-1)<<RF_ATEST_GAIN_R6_POS)
#define RF_ATEST_GAIN_R6_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R6_LEN)-1)<<RF_ATEST_GAIN_R6_POS))
#define RF_ATEST_GAIN_R5                                        RF_ATEST_GAIN_R5
#define RF_ATEST_GAIN_R5_POS                                    (16U)
#define RF_ATEST_GAIN_R5_LEN                                    (3U)
#define RF_ATEST_GAIN_R5_MSK                                    (((1U<<RF_ATEST_GAIN_R5_LEN)-1)<<RF_ATEST_GAIN_R5_POS)
#define RF_ATEST_GAIN_R5_UMSK                                   (~(((1U<<RF_ATEST_GAIN_R5_LEN)-1)<<RF_ATEST_GAIN_R5_POS))
#define RF_ATEST_OUT_EN_Q                                       RF_ATEST_OUT_EN_Q
#define RF_ATEST_OUT_EN_Q_POS                                   (20U)
#define RF_ATEST_OUT_EN_Q_LEN                                   (1U)
#define RF_ATEST_OUT_EN_Q_MSK                                   (((1U<<RF_ATEST_OUT_EN_Q_LEN)-1)<<RF_ATEST_OUT_EN_Q_POS)
#define RF_ATEST_OUT_EN_Q_UMSK                                  (~(((1U<<RF_ATEST_OUT_EN_Q_LEN)-1)<<RF_ATEST_OUT_EN_Q_POS))
#define RF_ATEST_OUT_EN_I                                       RF_ATEST_OUT_EN_I
#define RF_ATEST_OUT_EN_I_POS                                   (21U)
#define RF_ATEST_OUT_EN_I_LEN                                   (1U)
#define RF_ATEST_OUT_EN_I_MSK                                   (((1U<<RF_ATEST_OUT_EN_I_LEN)-1)<<RF_ATEST_OUT_EN_I_POS)
#define RF_ATEST_OUT_EN_I_UMSK                                  (~(((1U<<RF_ATEST_OUT_EN_I_LEN)-1)<<RF_ATEST_OUT_EN_I_POS))
#define RF_ATEST_IN_EN_Q                                        RF_ATEST_IN_EN_Q
#define RF_ATEST_IN_EN_Q_POS                                    (22U)
#define RF_ATEST_IN_EN_Q_LEN                                    (1U)
#define RF_ATEST_IN_EN_Q_MSK                                    (((1U<<RF_ATEST_IN_EN_Q_LEN)-1)<<RF_ATEST_IN_EN_Q_POS)
#define RF_ATEST_IN_EN_Q_UMSK                                   (~(((1U<<RF_ATEST_IN_EN_Q_LEN)-1)<<RF_ATEST_IN_EN_Q_POS))
#define RF_ATEST_IN_EN_I                                        RF_ATEST_IN_EN_I
#define RF_ATEST_IN_EN_I_POS                                    (23U)
#define RF_ATEST_IN_EN_I_LEN                                    (1U)
#define RF_ATEST_IN_EN_I_MSK                                    (((1U<<RF_ATEST_IN_EN_I_LEN)-1)<<RF_ATEST_IN_EN_I_POS)
#define RF_ATEST_IN_EN_I_UMSK                                   (~(((1U<<RF_ATEST_IN_EN_I_LEN)-1)<<RF_ATEST_IN_EN_I_POS))

/* 0x5C : pmip_mv2aon */
#define RF_PMIP_MV2AON_OFFSET                                   (0x5C)

/* 0x60 : RX normal bias mode registers */
#define RF_CIP_OFFSET                                           (0x60)
#define RF_VG11_SEL                                             RF_VG11_SEL
#define RF_VG11_SEL_POS                                         (0U)
#define RF_VG11_SEL_LEN                                         (2U)
#define RF_VG11_SEL_MSK                                         (((1U<<RF_VG11_SEL_LEN)-1)<<RF_VG11_SEL_POS)
#define RF_VG11_SEL_UMSK                                        (~(((1U<<RF_VG11_SEL_LEN)-1)<<RF_VG11_SEL_POS))
#define RF_VG13_SEL                                             RF_VG13_SEL
#define RF_VG13_SEL_POS                                         (2U)
#define RF_VG13_SEL_LEN                                         (2U)
#define RF_VG13_SEL_MSK                                         (((1U<<RF_VG13_SEL_LEN)-1)<<RF_VG13_SEL_POS)
#define RF_VG13_SEL_UMSK                                        (~(((1U<<RF_VG13_SEL_LEN)-1)<<RF_VG13_SEL_POS))

/* 0x64 : pa1 */
#define RF_PA1_OFFSET                                           (0x64)
#define RF_PA_IAQ                                               RF_PA_IAQ
#define RF_PA_IAQ_POS                                           (0U)
#define RF_PA_IAQ_LEN                                           (3U)
#define RF_PA_IAQ_MSK                                           (((1U<<RF_PA_IAQ_LEN)-1)<<RF_PA_IAQ_POS)
#define RF_PA_IAQ_UMSK                                          (~(((1U<<RF_PA_IAQ_LEN)-1)<<RF_PA_IAQ_POS))
#define RF_PA_ETB_EN                                            RF_PA_ETB_EN
#define RF_PA_ETB_EN_POS                                        (3U)
#define RF_PA_ETB_EN_LEN                                        (1U)
#define RF_PA_ETB_EN_MSK                                        (((1U<<RF_PA_ETB_EN_LEN)-1)<<RF_PA_ETB_EN_POS)
#define RF_PA_ETB_EN_UMSK                                       (~(((1U<<RF_PA_ETB_EN_LEN)-1)<<RF_PA_ETB_EN_POS))
#define RF_PA_IET                                               RF_PA_IET
#define RF_PA_IET_POS                                           (4U)
#define RF_PA_IET_LEN                                           (4U)
#define RF_PA_IET_MSK                                           (((1U<<RF_PA_IET_LEN)-1)<<RF_PA_IET_POS)
#define RF_PA_IET_UMSK                                          (~(((1U<<RF_PA_IET_LEN)-1)<<RF_PA_IET_POS))
#define RF_PA_VBCORE                                            RF_PA_VBCORE
#define RF_PA_VBCORE_POS                                        (8U)
#define RF_PA_VBCORE_LEN                                        (4U)
#define RF_PA_VBCORE_MSK                                        (((1U<<RF_PA_VBCORE_LEN)-1)<<RF_PA_VBCORE_POS)
#define RF_PA_VBCORE_UMSK                                       (~(((1U<<RF_PA_VBCORE_LEN)-1)<<RF_PA_VBCORE_POS))
#define RF_PA_VBCAS                                             RF_PA_VBCAS
#define RF_PA_VBCAS_POS                                         (12U)
#define RF_PA_VBCAS_LEN                                         (3U)
#define RF_PA_VBCAS_MSK                                         (((1U<<RF_PA_VBCAS_LEN)-1)<<RF_PA_VBCAS_POS)
#define RF_PA_VBCAS_UMSK                                        (~(((1U<<RF_PA_VBCAS_LEN)-1)<<RF_PA_VBCAS_POS))
#define RF_PA_HALF_ON                                           RF_PA_HALF_ON
#define RF_PA_HALF_ON_POS                                       (15U)
#define RF_PA_HALF_ON_LEN                                       (1U)
#define RF_PA_HALF_ON_MSK                                       (((1U<<RF_PA_HALF_ON_LEN)-1)<<RF_PA_HALF_ON_POS)
#define RF_PA_HALF_ON_UMSK                                      (~(((1U<<RF_PA_HALF_ON_LEN)-1)<<RF_PA_HALF_ON_POS))
#define RF_PA_IB_FIX                                            RF_PA_IB_FIX
#define RF_PA_IB_FIX_POS                                        (16U)
#define RF_PA_IB_FIX_LEN                                        (1U)
#define RF_PA_IB_FIX_MSK                                        (((1U<<RF_PA_IB_FIX_LEN)-1)<<RF_PA_IB_FIX_POS)
#define RF_PA_IB_FIX_UMSK                                       (~(((1U<<RF_PA_IB_FIX_LEN)-1)<<RF_PA_IB_FIX_POS))
#define RF_PA_LZ_BIAS_EN                                        RF_PA_LZ_BIAS_EN
#define RF_PA_LZ_BIAS_EN_POS                                    (17U)
#define RF_PA_LZ_BIAS_EN_LEN                                    (1U)
#define RF_PA_LZ_BIAS_EN_MSK                                    (((1U<<RF_PA_LZ_BIAS_EN_LEN)-1)<<RF_PA_LZ_BIAS_EN_POS)
#define RF_PA_LZ_BIAS_EN_UMSK                                   (~(((1U<<RF_PA_LZ_BIAS_EN_LEN)-1)<<RF_PA_LZ_BIAS_EN_POS))
#define RF_PA_PWRMX_OSDAC                                       RF_PA_PWRMX_OSDAC
#define RF_PA_PWRMX_OSDAC_POS                                   (18U)
#define RF_PA_PWRMX_OSDAC_LEN                                   (4U)
#define RF_PA_PWRMX_OSDAC_MSK                                   (((1U<<RF_PA_PWRMX_OSDAC_LEN)-1)<<RF_PA_PWRMX_OSDAC_POS)
#define RF_PA_PWRMX_OSDAC_UMSK                                  (~(((1U<<RF_PA_PWRMX_OSDAC_LEN)-1)<<RF_PA_PWRMX_OSDAC_POS))
#define RF_PA_PWRMX_DAC_PN_SWITCH                               RF_PA_PWRMX_DAC_PN_SWITCH
#define RF_PA_PWRMX_DAC_PN_SWITCH_POS                           (22U)
#define RF_PA_PWRMX_DAC_PN_SWITCH_LEN                           (1U)
#define RF_PA_PWRMX_DAC_PN_SWITCH_MSK                           (((1U<<RF_PA_PWRMX_DAC_PN_SWITCH_LEN)-1)<<RF_PA_PWRMX_DAC_PN_SWITCH_POS)
#define RF_PA_PWRMX_DAC_PN_SWITCH_UMSK                          (~(((1U<<RF_PA_PWRMX_DAC_PN_SWITCH_LEN)-1)<<RF_PA_PWRMX_DAC_PN_SWITCH_POS))
#define RF_PA_PWRMX_BM                                          RF_PA_PWRMX_BM
#define RF_PA_PWRMX_BM_POS                                      (24U)
#define RF_PA_PWRMX_BM_LEN                                      (3U)
#define RF_PA_PWRMX_BM_MSK                                      (((1U<<RF_PA_PWRMX_BM_LEN)-1)<<RF_PA_PWRMX_BM_POS)
#define RF_PA_PWRMX_BM_UMSK                                     (~(((1U<<RF_PA_PWRMX_BM_LEN)-1)<<RF_PA_PWRMX_BM_POS))
#define RF_PA_ATT_GC                                            RF_PA_ATT_GC
#define RF_PA_ATT_GC_POS                                        (28U)
#define RF_PA_ATT_GC_LEN                                        (4U)
#define RF_PA_ATT_GC_MSK                                        (((1U<<RF_PA_ATT_GC_LEN)-1)<<RF_PA_ATT_GC_POS)
#define RF_PA_ATT_GC_UMSK                                       (~(((1U<<RF_PA_ATT_GC_LEN)-1)<<RF_PA_ATT_GC_POS))

/* 0x68 : RX normal bias mode registers */
#define RF_PA2_OFFSET                                           (0x68)
#define RF_PA_ETB_EN_HW                                         RF_PA_ETB_EN_HW
#define RF_PA_ETB_EN_HW_POS                                     (3U)
#define RF_PA_ETB_EN_HW_LEN                                     (1U)
#define RF_PA_ETB_EN_HW_MSK                                     (((1U<<RF_PA_ETB_EN_HW_LEN)-1)<<RF_PA_ETB_EN_HW_POS)
#define RF_PA_ETB_EN_HW_UMSK                                    (~(((1U<<RF_PA_ETB_EN_HW_LEN)-1)<<RF_PA_ETB_EN_HW_POS))
#define RF_PA_IET_HW                                            RF_PA_IET_HW
#define RF_PA_IET_HW_POS                                        (4U)
#define RF_PA_IET_HW_LEN                                        (4U)
#define RF_PA_IET_HW_MSK                                        (((1U<<RF_PA_IET_HW_LEN)-1)<<RF_PA_IET_HW_POS)
#define RF_PA_IET_HW_UMSK                                       (~(((1U<<RF_PA_IET_HW_LEN)-1)<<RF_PA_IET_HW_POS))
#define RF_PA_VBCORE_HW                                         RF_PA_VBCORE_HW
#define RF_PA_VBCORE_HW_POS                                     (8U)
#define RF_PA_VBCORE_HW_LEN                                     (4U)
#define RF_PA_VBCORE_HW_MSK                                     (((1U<<RF_PA_VBCORE_HW_LEN)-1)<<RF_PA_VBCORE_HW_POS)
#define RF_PA_VBCORE_HW_UMSK                                    (~(((1U<<RF_PA_VBCORE_HW_LEN)-1)<<RF_PA_VBCORE_HW_POS))
#define RF_PA_VBCAS_HW                                          RF_PA_VBCAS_HW
#define RF_PA_VBCAS_HW_POS                                      (12U)
#define RF_PA_VBCAS_HW_LEN                                      (3U)
#define RF_PA_VBCAS_HW_MSK                                      (((1U<<RF_PA_VBCAS_HW_LEN)-1)<<RF_PA_VBCAS_HW_POS)
#define RF_PA_VBCAS_HW_UMSK                                     (~(((1U<<RF_PA_VBCAS_HW_LEN)-1)<<RF_PA_VBCAS_HW_POS))
#define RF_PA_HALF_ON_HW                                        RF_PA_HALF_ON_HW
#define RF_PA_HALF_ON_HW_POS                                    (16U)
#define RF_PA_HALF_ON_HW_LEN                                    (1U)
#define RF_PA_HALF_ON_HW_MSK                                    (((1U<<RF_PA_HALF_ON_HW_LEN)-1)<<RF_PA_HALF_ON_HW_POS)
#define RF_PA_HALF_ON_HW_UMSK                                   (~(((1U<<RF_PA_HALF_ON_HW_LEN)-1)<<RF_PA_HALF_ON_HW_POS))
#define RF_PA_IB_FIX_HW                                         RF_PA_IB_FIX_HW
#define RF_PA_IB_FIX_HW_POS                                     (17U)
#define RF_PA_IB_FIX_HW_LEN                                     (1U)
#define RF_PA_IB_FIX_HW_MSK                                     (((1U<<RF_PA_IB_FIX_HW_LEN)-1)<<RF_PA_IB_FIX_HW_POS)
#define RF_PA_IB_FIX_HW_UMSK                                    (~(((1U<<RF_PA_IB_FIX_HW_LEN)-1)<<RF_PA_IB_FIX_HW_POS))

/* 0x6C : tmx */
#define RF_TMX_OFFSET                                           (0x6C)
#define RF_TMX_CS                                               RF_TMX_CS
#define RF_TMX_CS_POS                                           (0U)
#define RF_TMX_CS_LEN                                           (3U)
#define RF_TMX_CS_MSK                                           (((1U<<RF_TMX_CS_LEN)-1)<<RF_TMX_CS_POS)
#define RF_TMX_CS_UMSK                                          (~(((1U<<RF_TMX_CS_LEN)-1)<<RF_TMX_CS_POS))
#define RF_TMX_BM_SW                                            RF_TMX_BM_SW
#define RF_TMX_BM_SW_POS                                        (4U)
#define RF_TMX_BM_SW_LEN                                        (3U)
#define RF_TMX_BM_SW_MSK                                        (((1U<<RF_TMX_BM_SW_LEN)-1)<<RF_TMX_BM_SW_POS)
#define RF_TMX_BM_SW_UMSK                                       (~(((1U<<RF_TMX_BM_SW_LEN)-1)<<RF_TMX_BM_SW_POS))
#define RF_TMX_BM_CAS                                           RF_TMX_BM_CAS
#define RF_TMX_BM_CAS_POS                                       (8U)
#define RF_TMX_BM_CAS_LEN                                       (3U)
#define RF_TMX_BM_CAS_MSK                                       (((1U<<RF_TMX_BM_CAS_LEN)-1)<<RF_TMX_BM_CAS_POS)
#define RF_TMX_BM_CAS_UMSK                                      (~(((1U<<RF_TMX_BM_CAS_LEN)-1)<<RF_TMX_BM_CAS_POS))
#define RF_TMX_BM_CAS_BULK                                      RF_TMX_BM_CAS_BULK
#define RF_TMX_BM_CAS_BULK_POS                                  (12U)
#define RF_TMX_BM_CAS_BULK_LEN                                  (3U)
#define RF_TMX_BM_CAS_BULK_MSK                                  (((1U<<RF_TMX_BM_CAS_BULK_LEN)-1)<<RF_TMX_BM_CAS_BULK_POS)
#define RF_TMX_BM_CAS_BULK_UMSK                                 (~(((1U<<RF_TMX_BM_CAS_BULK_LEN)-1)<<RF_TMX_BM_CAS_BULK_POS))
#define RF_TX_TSENSE_EN                                         RF_TX_TSENSE_EN
#define RF_TX_TSENSE_EN_POS                                     (16U)
#define RF_TX_TSENSE_EN_LEN                                     (1U)
#define RF_TX_TSENSE_EN_MSK                                     (((1U<<RF_TX_TSENSE_EN_LEN)-1)<<RF_TX_TSENSE_EN_POS)
#define RF_TX_TSENSE_EN_UMSK                                    (~(((1U<<RF_TX_TSENSE_EN_LEN)-1)<<RF_TX_TSENSE_EN_POS))

/* 0x70 : tbb */
#define RF_TBB_OFFSET                                           (0x70)
#define RF_TBB_BM_SF                                            RF_TBB_BM_SF
#define RF_TBB_BM_SF_POS                                        (0U)
#define RF_TBB_BM_SF_LEN                                        (2U)
#define RF_TBB_BM_SF_MSK                                        (((1U<<RF_TBB_BM_SF_LEN)-1)<<RF_TBB_BM_SF_POS)
#define RF_TBB_BM_SF_UMSK                                       (~(((1U<<RF_TBB_BM_SF_LEN)-1)<<RF_TBB_BM_SF_POS))
#define RF_TBB_BM_CG                                            RF_TBB_BM_CG
#define RF_TBB_BM_CG_POS                                        (4U)
#define RF_TBB_BM_CG_LEN                                        (2U)
#define RF_TBB_BM_CG_MSK                                        (((1U<<RF_TBB_BM_CG_LEN)-1)<<RF_TBB_BM_CG_POS)
#define RF_TBB_BM_CG_UMSK                                       (~(((1U<<RF_TBB_BM_CG_LEN)-1)<<RF_TBB_BM_CG_POS))
#define RF_TBB_VCM                                              RF_TBB_VCM
#define RF_TBB_VCM_POS                                          (8U)
#define RF_TBB_VCM_LEN                                          (2U)
#define RF_TBB_VCM_MSK                                          (((1U<<RF_TBB_VCM_LEN)-1)<<RF_TBB_VCM_POS)
#define RF_TBB_VCM_UMSK                                         (~(((1U<<RF_TBB_VCM_LEN)-1)<<RF_TBB_VCM_POS))
#define RF_TBB_CFLT                                             RF_TBB_CFLT
#define RF_TBB_CFLT_POS                                         (12U)
#define RF_TBB_CFLT_LEN                                         (2U)
#define RF_TBB_CFLT_MSK                                         (((1U<<RF_TBB_CFLT_LEN)-1)<<RF_TBB_CFLT_POS)
#define RF_TBB_CFLT_UMSK                                        (~(((1U<<RF_TBB_CFLT_LEN)-1)<<RF_TBB_CFLT_POS))
#define RF_TBB_IQ_BIAS_SHORT                                    RF_TBB_IQ_BIAS_SHORT
#define RF_TBB_IQ_BIAS_SHORT_POS                                (14U)
#define RF_TBB_IQ_BIAS_SHORT_LEN                                (1U)
#define RF_TBB_IQ_BIAS_SHORT_MSK                                (((1U<<RF_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_TBB_IQ_BIAS_SHORT_POS)
#define RF_TBB_IQ_BIAS_SHORT_UMSK                               (~(((1U<<RF_TBB_IQ_BIAS_SHORT_LEN)-1)<<RF_TBB_IQ_BIAS_SHORT_POS))
#define RF_TBB_ATEST_OUT_EN                                     RF_TBB_ATEST_OUT_EN
#define RF_TBB_ATEST_OUT_EN_POS                                 (15U)
#define RF_TBB_ATEST_OUT_EN_LEN                                 (1U)
#define RF_TBB_ATEST_OUT_EN_MSK                                 (((1U<<RF_TBB_ATEST_OUT_EN_LEN)-1)<<RF_TBB_ATEST_OUT_EN_POS)
#define RF_TBB_ATEST_OUT_EN_UMSK                                (~(((1U<<RF_TBB_ATEST_OUT_EN_LEN)-1)<<RF_TBB_ATEST_OUT_EN_POS))
#define RF_TBB_TOSDAC_Q                                         RF_TBB_TOSDAC_Q
#define RF_TBB_TOSDAC_Q_POS                                     (16U)
#define RF_TBB_TOSDAC_Q_LEN                                     (6U)
#define RF_TBB_TOSDAC_Q_MSK                                     (((1U<<RF_TBB_TOSDAC_Q_LEN)-1)<<RF_TBB_TOSDAC_Q_POS)
#define RF_TBB_TOSDAC_Q_UMSK                                    (~(((1U<<RF_TBB_TOSDAC_Q_LEN)-1)<<RF_TBB_TOSDAC_Q_POS))
#define RF_TBB_TOSDAC_I                                         RF_TBB_TOSDAC_I
#define RF_TBB_TOSDAC_I_POS                                     (24U)
#define RF_TBB_TOSDAC_I_LEN                                     (6U)
#define RF_TBB_TOSDAC_I_MSK                                     (((1U<<RF_TBB_TOSDAC_I_LEN)-1)<<RF_TBB_TOSDAC_I_POS)
#define RF_TBB_TOSDAC_I_UMSK                                    (~(((1U<<RF_TBB_TOSDAC_I_LEN)-1)<<RF_TBB_TOSDAC_I_POS))

/* 0x74 : lna */
#define RF_LNA_OFFSET                                           (0x74)
#define RF_LNA_BM                                               RF_LNA_BM
#define RF_LNA_BM_POS                                           (0U)
#define RF_LNA_BM_LEN                                           (4U)
#define RF_LNA_BM_MSK                                           (((1U<<RF_LNA_BM_LEN)-1)<<RF_LNA_BM_POS)
#define RF_LNA_BM_UMSK                                          (~(((1U<<RF_LNA_BM_LEN)-1)<<RF_LNA_BM_POS))
#define RF_LNA_BM_HW                                            RF_LNA_BM_HW
#define RF_LNA_BM_HW_POS                                        (4U)
#define RF_LNA_BM_HW_LEN                                        (4U)
#define RF_LNA_BM_HW_MSK                                        (((1U<<RF_LNA_BM_HW_LEN)-1)<<RF_LNA_BM_HW_POS)
#define RF_LNA_BM_HW_UMSK                                       (~(((1U<<RF_LNA_BM_HW_LEN)-1)<<RF_LNA_BM_HW_POS))
#define RF_LNA_LOAD_CSW                                         RF_LNA_LOAD_CSW
#define RF_LNA_LOAD_CSW_POS                                     (8U)
#define RF_LNA_LOAD_CSW_LEN                                     (4U)
#define RF_LNA_LOAD_CSW_MSK                                     (((1U<<RF_LNA_LOAD_CSW_LEN)-1)<<RF_LNA_LOAD_CSW_POS)
#define RF_LNA_LOAD_CSW_UMSK                                    (~(((1U<<RF_LNA_LOAD_CSW_LEN)-1)<<RF_LNA_LOAD_CSW_POS))
#define RF_LNA_LOAD_CSW_HW                                      RF_LNA_LOAD_CSW_HW
#define RF_LNA_LOAD_CSW_HW_POS                                  (12U)
#define RF_LNA_LOAD_CSW_HW_LEN                                  (4U)
#define RF_LNA_LOAD_CSW_HW_MSK                                  (((1U<<RF_LNA_LOAD_CSW_HW_LEN)-1)<<RF_LNA_LOAD_CSW_HW_POS)
#define RF_LNA_LOAD_CSW_HW_UMSK                                 (~(((1U<<RF_LNA_LOAD_CSW_HW_LEN)-1)<<RF_LNA_LOAD_CSW_HW_POS))
#define RF_LNA_RFB_MATCH                                        RF_LNA_RFB_MATCH
#define RF_LNA_RFB_MATCH_POS                                    (16U)
#define RF_LNA_RFB_MATCH_LEN                                    (3U)
#define RF_LNA_RFB_MATCH_MSK                                    (((1U<<RF_LNA_RFB_MATCH_LEN)-1)<<RF_LNA_RFB_MATCH_POS)
#define RF_LNA_RFB_MATCH_UMSK                                   (~(((1U<<RF_LNA_RFB_MATCH_LEN)-1)<<RF_LNA_RFB_MATCH_POS))
#define RF_LNA_CAP_LG                                           RF_LNA_CAP_LG
#define RF_LNA_CAP_LG_POS                                       (20U)
#define RF_LNA_CAP_LG_LEN                                       (2U)
#define RF_LNA_CAP_LG_MSK                                       (((1U<<RF_LNA_CAP_LG_LEN)-1)<<RF_LNA_CAP_LG_POS)
#define RF_LNA_CAP_LG_UMSK                                      (~(((1U<<RF_LNA_CAP_LG_LEN)-1)<<RF_LNA_CAP_LG_POS))
#define RF_LNA_LG_GSEL                                          RF_LNA_LG_GSEL
#define RF_LNA_LG_GSEL_POS                                      (24U)
#define RF_LNA_LG_GSEL_LEN                                      (3U)
#define RF_LNA_LG_GSEL_MSK                                      (((1U<<RF_LNA_LG_GSEL_LEN)-1)<<RF_LNA_LG_GSEL_POS)
#define RF_LNA_LG_GSEL_UMSK                                     (~(((1U<<RF_LNA_LG_GSEL_LEN)-1)<<RF_LNA_LG_GSEL_POS))

/* 0x78 : rmxgm */
#define RF_RMXGM_OFFSET                                         (0x78)
#define RF_RMX_BM                                               RF_RMX_BM
#define RF_RMX_BM_POS                                           (0U)
#define RF_RMX_BM_LEN                                           (3U)
#define RF_RMX_BM_MSK                                           (((1U<<RF_RMX_BM_LEN)-1)<<RF_RMX_BM_POS)
#define RF_RMX_BM_UMSK                                          (~(((1U<<RF_RMX_BM_LEN)-1)<<RF_RMX_BM_POS))
#define RF_RMXGM_BM                                             RF_RMXGM_BM
#define RF_RMXGM_BM_POS                                         (4U)
#define RF_RMXGM_BM_LEN                                         (3U)
#define RF_RMXGM_BM_MSK                                         (((1U<<RF_RMXGM_BM_LEN)-1)<<RF_RMXGM_BM_POS)
#define RF_RMXGM_BM_UMSK                                        (~(((1U<<RF_RMXGM_BM_LEN)-1)<<RF_RMXGM_BM_POS))
#define RF_RMXGM_10M_MODE_EN                                    RF_RMXGM_10M_MODE_EN
#define RF_RMXGM_10M_MODE_EN_POS                                (8U)
#define RF_RMXGM_10M_MODE_EN_LEN                                (1U)
#define RF_RMXGM_10M_MODE_EN_MSK                                (((1U<<RF_RMXGM_10M_MODE_EN_LEN)-1)<<RF_RMXGM_10M_MODE_EN_POS)
#define RF_RMXGM_10M_MODE_EN_UMSK                               (~(((1U<<RF_RMXGM_10M_MODE_EN_LEN)-1)<<RF_RMXGM_10M_MODE_EN_POS))

/* 0x7C : rbb1 */
#define RF_RBB1_OFFSET                                          (0x7C)
#define RF_ROSDAC_Q                                             RF_ROSDAC_Q
#define RF_ROSDAC_Q_POS                                         (0U)
#define RF_ROSDAC_Q_LEN                                         (6U)
#define RF_ROSDAC_Q_MSK                                         (((1U<<RF_ROSDAC_Q_LEN)-1)<<RF_ROSDAC_Q_POS)
#define RF_ROSDAC_Q_UMSK                                        (~(((1U<<RF_ROSDAC_Q_LEN)-1)<<RF_ROSDAC_Q_POS))
#define RF_ROSDAC_I                                             RF_ROSDAC_I
#define RF_ROSDAC_I_POS                                         (8U)
#define RF_ROSDAC_I_LEN                                         (6U)
#define RF_ROSDAC_I_MSK                                         (((1U<<RF_ROSDAC_I_LEN)-1)<<RF_ROSDAC_I_POS)
#define RF_ROSDAC_I_UMSK                                        (~(((1U<<RF_ROSDAC_I_LEN)-1)<<RF_ROSDAC_I_POS))
#define RF_ROSDAC_Q_HW                                          RF_ROSDAC_Q_HW
#define RF_ROSDAC_Q_HW_POS                                      (16U)
#define RF_ROSDAC_Q_HW_LEN                                      (6U)
#define RF_ROSDAC_Q_HW_MSK                                      (((1U<<RF_ROSDAC_Q_HW_LEN)-1)<<RF_ROSDAC_Q_HW_POS)
#define RF_ROSDAC_Q_HW_UMSK                                     (~(((1U<<RF_ROSDAC_Q_HW_LEN)-1)<<RF_ROSDAC_Q_HW_POS))
#define RF_ROSDAC_I_HW                                          RF_ROSDAC_I_HW
#define RF_ROSDAC_I_HW_POS                                      (24U)
#define RF_ROSDAC_I_HW_LEN                                      (6U)
#define RF_ROSDAC_I_HW_MSK                                      (((1U<<RF_ROSDAC_I_HW_LEN)-1)<<RF_ROSDAC_I_HW_POS)
#define RF_ROSDAC_I_HW_UMSK                                     (~(((1U<<RF_ROSDAC_I_HW_LEN)-1)<<RF_ROSDAC_I_HW_POS))
#define RF_ROSDAC_RANGE                                         RF_ROSDAC_RANGE
#define RF_ROSDAC_RANGE_POS                                     (31U)
#define RF_ROSDAC_RANGE_LEN                                     (1U)
#define RF_ROSDAC_RANGE_MSK                                     (((1U<<RF_ROSDAC_RANGE_LEN)-1)<<RF_ROSDAC_RANGE_POS)
#define RF_ROSDAC_RANGE_UMSK                                    (~(((1U<<RF_ROSDAC_RANGE_LEN)-1)<<RF_ROSDAC_RANGE_POS))

/* 0x80 : rbb2 */
#define RF_RBB2_OFFSET                                          (0x80)
#define RF_RBB_CAP2_FC_Q                                        RF_RBB_CAP2_FC_Q
#define RF_RBB_CAP2_FC_Q_POS                                    (0U)
#define RF_RBB_CAP2_FC_Q_LEN                                    (6U)
#define RF_RBB_CAP2_FC_Q_MSK                                    (((1U<<RF_RBB_CAP2_FC_Q_LEN)-1)<<RF_RBB_CAP2_FC_Q_POS)
#define RF_RBB_CAP2_FC_Q_UMSK                                   (~(((1U<<RF_RBB_CAP2_FC_Q_LEN)-1)<<RF_RBB_CAP2_FC_Q_POS))
#define RF_RBB_CAP2_FC_I                                        RF_RBB_CAP2_FC_I
#define RF_RBB_CAP2_FC_I_POS                                    (8U)
#define RF_RBB_CAP2_FC_I_LEN                                    (6U)
#define RF_RBB_CAP2_FC_I_MSK                                    (((1U<<RF_RBB_CAP2_FC_I_LEN)-1)<<RF_RBB_CAP2_FC_I_POS)
#define RF_RBB_CAP2_FC_I_UMSK                                   (~(((1U<<RF_RBB_CAP2_FC_I_LEN)-1)<<RF_RBB_CAP2_FC_I_POS))
#define RF_RBB_CAP1_FC_Q                                        RF_RBB_CAP1_FC_Q
#define RF_RBB_CAP1_FC_Q_POS                                    (16U)
#define RF_RBB_CAP1_FC_Q_LEN                                    (6U)
#define RF_RBB_CAP1_FC_Q_MSK                                    (((1U<<RF_RBB_CAP1_FC_Q_LEN)-1)<<RF_RBB_CAP1_FC_Q_POS)
#define RF_RBB_CAP1_FC_Q_UMSK                                   (~(((1U<<RF_RBB_CAP1_FC_Q_LEN)-1)<<RF_RBB_CAP1_FC_Q_POS))
#define RF_RBB_CAP1_FC_I                                        RF_RBB_CAP1_FC_I
#define RF_RBB_CAP1_FC_I_POS                                    (24U)
#define RF_RBB_CAP1_FC_I_LEN                                    (6U)
#define RF_RBB_CAP1_FC_I_MSK                                    (((1U<<RF_RBB_CAP1_FC_I_LEN)-1)<<RF_RBB_CAP1_FC_I_POS)
#define RF_RBB_CAP1_FC_I_UMSK                                   (~(((1U<<RF_RBB_CAP1_FC_I_LEN)-1)<<RF_RBB_CAP1_FC_I_POS))

/* 0x84 : rbb3 */
#define RF_RBB3_OFFSET                                          (0x84)
#define RF_RBB_BT_MODE_HW                                       RF_RBB_BT_MODE_HW
#define RF_RBB_BT_MODE_HW_POS                                   (0U)
#define RF_RBB_BT_MODE_HW_LEN                                   (1U)
#define RF_RBB_BT_MODE_HW_MSK                                   (((1U<<RF_RBB_BT_MODE_HW_LEN)-1)<<RF_RBB_BT_MODE_HW_POS)
#define RF_RBB_BT_MODE_HW_UMSK                                  (~(((1U<<RF_RBB_BT_MODE_HW_LEN)-1)<<RF_RBB_BT_MODE_HW_POS))
#define RF_RBB_BT_MODE                                          RF_RBB_BT_MODE
#define RF_RBB_BT_MODE_POS                                      (4U)
#define RF_RBB_BT_MODE_LEN                                      (1U)
#define RF_RBB_BT_MODE_MSK                                      (((1U<<RF_RBB_BT_MODE_LEN)-1)<<RF_RBB_BT_MODE_POS)
#define RF_RBB_BT_MODE_UMSK                                     (~(((1U<<RF_RBB_BT_MODE_LEN)-1)<<RF_RBB_BT_MODE_POS))
#define RF_RBB_BT_FIF_TUNE                                      RF_RBB_BT_FIF_TUNE
#define RF_RBB_BT_FIF_TUNE_POS                                  (5U)
#define RF_RBB_BT_FIF_TUNE_LEN                                  (2U)
#define RF_RBB_BT_FIF_TUNE_MSK                                  (((1U<<RF_RBB_BT_FIF_TUNE_LEN)-1)<<RF_RBB_BT_FIF_TUNE_POS)
#define RF_RBB_BT_FIF_TUNE_UMSK                                 (~(((1U<<RF_RBB_BT_FIF_TUNE_LEN)-1)<<RF_RBB_BT_FIF_TUNE_POS))
#define RF_RBB_DEQ                                              RF_RBB_DEQ
#define RF_RBB_DEQ_POS                                          (8U)
#define RF_RBB_DEQ_LEN                                          (2U)
#define RF_RBB_DEQ_MSK                                          (((1U<<RF_RBB_DEQ_LEN)-1)<<RF_RBB_DEQ_POS)
#define RF_RBB_DEQ_UMSK                                         (~(((1U<<RF_RBB_DEQ_LEN)-1)<<RF_RBB_DEQ_POS))
#define RF_RBB_BM_OP                                            RF_RBB_BM_OP
#define RF_RBB_BM_OP_POS                                        (12U)
#define RF_RBB_BM_OP_LEN                                        (3U)
#define RF_RBB_BM_OP_MSK                                        (((1U<<RF_RBB_BM_OP_LEN)-1)<<RF_RBB_BM_OP_POS)
#define RF_RBB_BM_OP_UMSK                                       (~(((1U<<RF_RBB_BM_OP_LEN)-1)<<RF_RBB_BM_OP_POS))
#define RF_RBB_VCM                                              RF_RBB_VCM
#define RF_RBB_VCM_POS                                          (16U)
#define RF_RBB_VCM_LEN                                          (2U)
#define RF_RBB_VCM_MSK                                          (((1U<<RF_RBB_VCM_LEN)-1)<<RF_RBB_VCM_POS)
#define RF_RBB_VCM_UMSK                                         (~(((1U<<RF_RBB_VCM_LEN)-1)<<RF_RBB_VCM_POS))
#define RF_RBB_BQ_IQBIAS_SHORT                                  RF_RBB_BQ_IQBIAS_SHORT
#define RF_RBB_BQ_IQBIAS_SHORT_POS                              (20U)
#define RF_RBB_BQ_IQBIAS_SHORT_LEN                              (1U)
#define RF_RBB_BQ_IQBIAS_SHORT_MSK                              (((1U<<RF_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_RBB_BQ_IQBIAS_SHORT_POS)
#define RF_RBB_BQ_IQBIAS_SHORT_UMSK                             (~(((1U<<RF_RBB_BQ_IQBIAS_SHORT_LEN)-1)<<RF_RBB_BQ_IQBIAS_SHORT_POS))
#define RF_RBB_TIA_IQBIAS_SHORT                                 RF_RBB_TIA_IQBIAS_SHORT
#define RF_RBB_TIA_IQBIAS_SHORT_POS                             (21U)
#define RF_RBB_TIA_IQBIAS_SHORT_LEN                             (1U)
#define RF_RBB_TIA_IQBIAS_SHORT_MSK                             (((1U<<RF_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_RBB_TIA_IQBIAS_SHORT_POS)
#define RF_RBB_TIA_IQBIAS_SHORT_UMSK                            (~(((1U<<RF_RBB_TIA_IQBIAS_SHORT_LEN)-1)<<RF_RBB_TIA_IQBIAS_SHORT_POS))
#define RF_RBB_BW                                               RF_RBB_BW
#define RF_RBB_BW_POS                                           (24U)
#define RF_RBB_BW_LEN                                           (2U)
#define RF_RBB_BW_MSK                                           (((1U<<RF_RBB_BW_LEN)-1)<<RF_RBB_BW_POS)
#define RF_RBB_BW_UMSK                                          (~(((1U<<RF_RBB_BW_LEN)-1)<<RF_RBB_BW_POS))
#define RF_RXIQCAL_EN                                           RF_RXIQCAL_EN
#define RF_RXIQCAL_EN_POS                                       (28U)
#define RF_RXIQCAL_EN_LEN                                       (1U)
#define RF_RXIQCAL_EN_MSK                                       (((1U<<RF_RXIQCAL_EN_LEN)-1)<<RF_RXIQCAL_EN_POS)
#define RF_RXIQCAL_EN_UMSK                                      (~(((1U<<RF_RXIQCAL_EN_LEN)-1)<<RF_RXIQCAL_EN_POS))
#define RF_PWR_DET_EN                                           RF_PWR_DET_EN
#define RF_PWR_DET_EN_POS                                       (31U)
#define RF_PWR_DET_EN_LEN                                       (1U)
#define RF_PWR_DET_EN_MSK                                       (((1U<<RF_PWR_DET_EN_LEN)-1)<<RF_PWR_DET_EN_POS)
#define RF_PWR_DET_EN_UMSK                                      (~(((1U<<RF_PWR_DET_EN_LEN)-1)<<RF_PWR_DET_EN_POS))

/* 0x88 : rbb4 */
#define RF_RBB4_OFFSET                                          (0x88)
#define RF_RBB_PKDET_VTH                                        RF_RBB_PKDET_VTH
#define RF_RBB_PKDET_VTH_POS                                    (0U)
#define RF_RBB_PKDET_VTH_LEN                                    (4U)
#define RF_RBB_PKDET_VTH_MSK                                    (((1U<<RF_RBB_PKDET_VTH_LEN)-1)<<RF_RBB_PKDET_VTH_POS)
#define RF_RBB_PKDET_VTH_UMSK                                   (~(((1U<<RF_RBB_PKDET_VTH_LEN)-1)<<RF_RBB_PKDET_VTH_POS))
#define RF_RBB_PKDET_OUT_RSTN                                   RF_RBB_PKDET_OUT_RSTN
#define RF_RBB_PKDET_OUT_RSTN_POS                               (4U)
#define RF_RBB_PKDET_OUT_RSTN_LEN                               (1U)
#define RF_RBB_PKDET_OUT_RSTN_MSK                               (((1U<<RF_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_POS)
#define RF_RBB_PKDET_OUT_RSTN_UMSK                              (~(((1U<<RF_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_POS))
#define RF_RBB_PKDET_EN                                         RF_RBB_PKDET_EN
#define RF_RBB_PKDET_EN_POS                                     (8U)
#define RF_RBB_PKDET_EN_LEN                                     (1U)
#define RF_RBB_PKDET_EN_MSK                                     (((1U<<RF_RBB_PKDET_EN_LEN)-1)<<RF_RBB_PKDET_EN_POS)
#define RF_RBB_PKDET_EN_UMSK                                    (~(((1U<<RF_RBB_PKDET_EN_LEN)-1)<<RF_RBB_PKDET_EN_POS))
#define RF_RBB_PKDET_OUT_RSTN_HW                                RF_RBB_PKDET_OUT_RSTN_HW
#define RF_RBB_PKDET_OUT_RSTN_HW_POS                            (12U)
#define RF_RBB_PKDET_OUT_RSTN_HW_LEN                            (1U)
#define RF_RBB_PKDET_OUT_RSTN_HW_MSK                            (((1U<<RF_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_HW_POS)
#define RF_RBB_PKDET_OUT_RSTN_HW_UMSK                           (~(((1U<<RF_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_HW_POS))
#define RF_RBB_PKDET_EN_HW                                      RF_RBB_PKDET_EN_HW
#define RF_RBB_PKDET_EN_HW_POS                                  (16U)
#define RF_RBB_PKDET_EN_HW_LEN                                  (1U)
#define RF_RBB_PKDET_EN_HW_MSK                                  (((1U<<RF_RBB_PKDET_EN_HW_LEN)-1)<<RF_RBB_PKDET_EN_HW_POS)
#define RF_RBB_PKDET_EN_HW_UMSK                                 (~(((1U<<RF_RBB_PKDET_EN_HW_LEN)-1)<<RF_RBB_PKDET_EN_HW_POS))
#define RF_PKDET_OUT_RAW                                        RF_PKDET_OUT_RAW
#define RF_PKDET_OUT_RAW_POS                                    (20U)
#define RF_PKDET_OUT_RAW_LEN                                    (1U)
#define RF_PKDET_OUT_RAW_MSK                                    (((1U<<RF_PKDET_OUT_RAW_LEN)-1)<<RF_PKDET_OUT_RAW_POS)
#define RF_PKDET_OUT_RAW_UMSK                                   (~(((1U<<RF_PKDET_OUT_RAW_LEN)-1)<<RF_PKDET_OUT_RAW_POS))
#define RF_PKDET_OUT_LATCH                                      RF_PKDET_OUT_LATCH
#define RF_PKDET_OUT_LATCH_POS                                  (24U)
#define RF_PKDET_OUT_LATCH_LEN                                  (1U)
#define RF_PKDET_OUT_LATCH_MSK                                  (((1U<<RF_PKDET_OUT_LATCH_LEN)-1)<<RF_PKDET_OUT_LATCH_POS)
#define RF_PKDET_OUT_LATCH_UMSK                                 (~(((1U<<RF_PKDET_OUT_LATCH_LEN)-1)<<RF_PKDET_OUT_LATCH_POS))

/* 0x8C : adda1 */
#define RF_ADDA1_OFFSET                                         (0x8C)
#define RF_DAC_DVDD_SEL                                         RF_DAC_DVDD_SEL
#define RF_DAC_DVDD_SEL_POS                                     (0U)
#define RF_DAC_DVDD_SEL_LEN                                     (2U)
#define RF_DAC_DVDD_SEL_MSK                                     (((1U<<RF_DAC_DVDD_SEL_LEN)-1)<<RF_DAC_DVDD_SEL_POS)
#define RF_DAC_DVDD_SEL_UMSK                                    (~(((1U<<RF_DAC_DVDD_SEL_LEN)-1)<<RF_DAC_DVDD_SEL_POS))
#define RF_DAC_BIAS_SEL                                         RF_DAC_BIAS_SEL
#define RF_DAC_BIAS_SEL_POS                                     (4U)
#define RF_DAC_BIAS_SEL_LEN                                     (2U)
#define RF_DAC_BIAS_SEL_MSK                                     (((1U<<RF_DAC_BIAS_SEL_LEN)-1)<<RF_DAC_BIAS_SEL_POS)
#define RF_DAC_BIAS_SEL_UMSK                                    (~(((1U<<RF_DAC_BIAS_SEL_LEN)-1)<<RF_DAC_BIAS_SEL_POS))
#define RF_DAC_CLK_SEL                                          RF_DAC_CLK_SEL
#define RF_DAC_CLK_SEL_POS                                      (8U)
#define RF_DAC_CLK_SEL_LEN                                      (2U)
#define RF_DAC_CLK_SEL_MSK                                      (((1U<<RF_DAC_CLK_SEL_LEN)-1)<<RF_DAC_CLK_SEL_POS)
#define RF_DAC_CLK_SEL_UMSK                                     (~(((1U<<RF_DAC_CLK_SEL_LEN)-1)<<RF_DAC_CLK_SEL_POS))
#define RF_DAC_RCCALSEL                                         RF_DAC_RCCALSEL
#define RF_DAC_RCCALSEL_POS                                     (12U)
#define RF_DAC_RCCALSEL_LEN                                     (1U)
#define RF_DAC_RCCALSEL_MSK                                     (((1U<<RF_DAC_RCCALSEL_LEN)-1)<<RF_DAC_RCCALSEL_POS)
#define RF_DAC_RCCALSEL_UMSK                                    (~(((1U<<RF_DAC_RCCALSEL_LEN)-1)<<RF_DAC_RCCALSEL_POS))
#define RF_DAC_CLK_SYNC_INV                                     RF_DAC_CLK_SYNC_INV
#define RF_DAC_CLK_SYNC_INV_POS                                 (13U)
#define RF_DAC_CLK_SYNC_INV_LEN                                 (1U)
#define RF_DAC_CLK_SYNC_INV_MSK                                 (((1U<<RF_DAC_CLK_SYNC_INV_LEN)-1)<<RF_DAC_CLK_SYNC_INV_POS)
#define RF_DAC_CLK_SYNC_INV_UMSK                                (~(((1U<<RF_DAC_CLK_SYNC_INV_LEN)-1)<<RF_DAC_CLK_SYNC_INV_POS))
#define RF_ADDA_LDO_BYPS                                        RF_ADDA_LDO_BYPS
#define RF_ADDA_LDO_BYPS_POS                                    (16U)
#define RF_ADDA_LDO_BYPS_LEN                                    (1U)
#define RF_ADDA_LDO_BYPS_MSK                                    (((1U<<RF_ADDA_LDO_BYPS_LEN)-1)<<RF_ADDA_LDO_BYPS_POS)
#define RF_ADDA_LDO_BYPS_UMSK                                   (~(((1U<<RF_ADDA_LDO_BYPS_LEN)-1)<<RF_ADDA_LDO_BYPS_POS))
#define RF_ADDA_LDO_DVDD_SEL                                    RF_ADDA_LDO_DVDD_SEL
#define RF_ADDA_LDO_DVDD_SEL_POS                                (20U)
#define RF_ADDA_LDO_DVDD_SEL_LEN                                (3U)
#define RF_ADDA_LDO_DVDD_SEL_MSK                                (((1U<<RF_ADDA_LDO_DVDD_SEL_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_POS)
#define RF_ADDA_LDO_DVDD_SEL_UMSK                               (~(((1U<<RF_ADDA_LDO_DVDD_SEL_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_POS))
#define RF_ADDA_LDO_DVDD_SEL_HW                                 RF_ADDA_LDO_DVDD_SEL_HW
#define RF_ADDA_LDO_DVDD_SEL_HW_POS                             (24U)
#define RF_ADDA_LDO_DVDD_SEL_HW_LEN                             (3U)
#define RF_ADDA_LDO_DVDD_SEL_HW_MSK                             (((1U<<RF_ADDA_LDO_DVDD_SEL_HW_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_HW_POS)
#define RF_ADDA_LDO_DVDD_SEL_HW_UMSK                            (~(((1U<<RF_ADDA_LDO_DVDD_SEL_HW_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_HW_POS))

/* 0x90 : adda2 */
#define RF_ADDA2_OFFSET                                         (0x90)
#define RF_ADC_VREF_SEL                                         RF_ADC_VREF_SEL
#define RF_ADC_VREF_SEL_POS                                     (0U)
#define RF_ADC_VREF_SEL_LEN                                     (2U)
#define RF_ADC_VREF_SEL_MSK                                     (((1U<<RF_ADC_VREF_SEL_LEN)-1)<<RF_ADC_VREF_SEL_POS)
#define RF_ADC_VREF_SEL_UMSK                                    (~(((1U<<RF_ADC_VREF_SEL_LEN)-1)<<RF_ADC_VREF_SEL_POS))
#define RF_ADC_DLY_CTL                                          RF_ADC_DLY_CTL
#define RF_ADC_DLY_CTL_POS                                      (4U)
#define RF_ADC_DLY_CTL_LEN                                      (2U)
#define RF_ADC_DLY_CTL_MSK                                      (((1U<<RF_ADC_DLY_CTL_LEN)-1)<<RF_ADC_DLY_CTL_POS)
#define RF_ADC_DLY_CTL_UMSK                                     (~(((1U<<RF_ADC_DLY_CTL_LEN)-1)<<RF_ADC_DLY_CTL_POS))
#define RF_ADC_DVDD_SEL                                         RF_ADC_DVDD_SEL
#define RF_ADC_DVDD_SEL_POS                                     (8U)
#define RF_ADC_DVDD_SEL_LEN                                     (2U)
#define RF_ADC_DVDD_SEL_MSK                                     (((1U<<RF_ADC_DVDD_SEL_LEN)-1)<<RF_ADC_DVDD_SEL_POS)
#define RF_ADC_DVDD_SEL_UMSK                                    (~(((1U<<RF_ADC_DVDD_SEL_LEN)-1)<<RF_ADC_DVDD_SEL_POS))
#define RF_ADC_SAR_ASCAL_EN                                     RF_ADC_SAR_ASCAL_EN
#define RF_ADC_SAR_ASCAL_EN_POS                                 (12U)
#define RF_ADC_SAR_ASCAL_EN_LEN                                 (1U)
#define RF_ADC_SAR_ASCAL_EN_MSK                                 (((1U<<RF_ADC_SAR_ASCAL_EN_LEN)-1)<<RF_ADC_SAR_ASCAL_EN_POS)
#define RF_ADC_SAR_ASCAL_EN_UMSK                                (~(((1U<<RF_ADC_SAR_ASCAL_EN_LEN)-1)<<RF_ADC_SAR_ASCAL_EN_POS))
#define RF_ADC_GT_RM                                            RF_ADC_GT_RM
#define RF_ADC_GT_RM_POS                                        (16U)
#define RF_ADC_GT_RM_LEN                                        (1U)
#define RF_ADC_GT_RM_MSK                                        (((1U<<RF_ADC_GT_RM_LEN)-1)<<RF_ADC_GT_RM_POS)
#define RF_ADC_GT_RM_UMSK                                       (~(((1U<<RF_ADC_GT_RM_LEN)-1)<<RF_ADC_GT_RM_POS))
#define RF_ADC_CLK_SYNC_INV                                     RF_ADC_CLK_SYNC_INV
#define RF_ADC_CLK_SYNC_INV_POS                                 (20U)
#define RF_ADC_CLK_SYNC_INV_LEN                                 (1U)
#define RF_ADC_CLK_SYNC_INV_MSK                                 (((1U<<RF_ADC_CLK_SYNC_INV_LEN)-1)<<RF_ADC_CLK_SYNC_INV_POS)
#define RF_ADC_CLK_SYNC_INV_UMSK                                (~(((1U<<RF_ADC_CLK_SYNC_INV_LEN)-1)<<RF_ADC_CLK_SYNC_INV_POS))
#define RF_ADC_CLK_INV                                          RF_ADC_CLK_INV
#define RF_ADC_CLK_INV_POS                                      (24U)
#define RF_ADC_CLK_INV_LEN                                      (1U)
#define RF_ADC_CLK_INV_MSK                                      (((1U<<RF_ADC_CLK_INV_LEN)-1)<<RF_ADC_CLK_INV_POS)
#define RF_ADC_CLK_INV_UMSK                                     (~(((1U<<RF_ADC_CLK_INV_LEN)-1)<<RF_ADC_CLK_INV_POS))
#define RF_ADC_CLK_DIV_SEL                                      RF_ADC_CLK_DIV_SEL
#define RF_ADC_CLK_DIV_SEL_POS                                  (28U)
#define RF_ADC_CLK_DIV_SEL_LEN                                  (1U)
#define RF_ADC_CLK_DIV_SEL_MSK                                  (((1U<<RF_ADC_CLK_DIV_SEL_LEN)-1)<<RF_ADC_CLK_DIV_SEL_POS)
#define RF_ADC_CLK_DIV_SEL_UMSK                                 (~(((1U<<RF_ADC_CLK_DIV_SEL_LEN)-1)<<RF_ADC_CLK_DIV_SEL_POS))

/* 0xA0 : vco1 */
#define RF_VCO1_OFFSET                                          (0xA0)
#define RF_LO_VCO_FREQ_CW                                       RF_LO_VCO_FREQ_CW
#define RF_LO_VCO_FREQ_CW_POS                                   (0U)
#define RF_LO_VCO_FREQ_CW_LEN                                   (8U)
#define RF_LO_VCO_FREQ_CW_MSK                                   (((1U<<RF_LO_VCO_FREQ_CW_LEN)-1)<<RF_LO_VCO_FREQ_CW_POS)
#define RF_LO_VCO_FREQ_CW_UMSK                                  (~(((1U<<RF_LO_VCO_FREQ_CW_LEN)-1)<<RF_LO_VCO_FREQ_CW_POS))
#define RF_LO_VCO_FREQ_CW_HW                                    RF_LO_VCO_FREQ_CW_HW
#define RF_LO_VCO_FREQ_CW_HW_POS                                (8U)
#define RF_LO_VCO_FREQ_CW_HW_LEN                                (8U)
#define RF_LO_VCO_FREQ_CW_HW_MSK                                (((1U<<RF_LO_VCO_FREQ_CW_HW_LEN)-1)<<RF_LO_VCO_FREQ_CW_HW_POS)
#define RF_LO_VCO_FREQ_CW_HW_UMSK                               (~(((1U<<RF_LO_VCO_FREQ_CW_HW_LEN)-1)<<RF_LO_VCO_FREQ_CW_HW_POS))
#define RF_LO_VCO_IDAC_CW                                       RF_LO_VCO_IDAC_CW
#define RF_LO_VCO_IDAC_CW_POS                                   (16U)
#define RF_LO_VCO_IDAC_CW_LEN                                   (5U)
#define RF_LO_VCO_IDAC_CW_MSK                                   (((1U<<RF_LO_VCO_IDAC_CW_LEN)-1)<<RF_LO_VCO_IDAC_CW_POS)
#define RF_LO_VCO_IDAC_CW_UMSK                                  (~(((1U<<RF_LO_VCO_IDAC_CW_LEN)-1)<<RF_LO_VCO_IDAC_CW_POS))
#define RF_LO_VCO_IDAC_CW_HW                                    RF_LO_VCO_IDAC_CW_HW
#define RF_LO_VCO_IDAC_CW_HW_POS                                (24U)
#define RF_LO_VCO_IDAC_CW_HW_LEN                                (5U)
#define RF_LO_VCO_IDAC_CW_HW_MSK                                (((1U<<RF_LO_VCO_IDAC_CW_HW_LEN)-1)<<RF_LO_VCO_IDAC_CW_HW_POS)
#define RF_LO_VCO_IDAC_CW_HW_UMSK                               (~(((1U<<RF_LO_VCO_IDAC_CW_HW_LEN)-1)<<RF_LO_VCO_IDAC_CW_HW_POS))

/* 0xA4 : vco2 */
#define RF_VCO2_OFFSET                                          (0xA4)
#define RF_LO_VCO_VBIAS_CW                                      RF_LO_VCO_VBIAS_CW
#define RF_LO_VCO_VBIAS_CW_POS                                  (0U)
#define RF_LO_VCO_VBIAS_CW_LEN                                  (2U)
#define RF_LO_VCO_VBIAS_CW_MSK                                  (((1U<<RF_LO_VCO_VBIAS_CW_LEN)-1)<<RF_LO_VCO_VBIAS_CW_POS)
#define RF_LO_VCO_VBIAS_CW_UMSK                                 (~(((1U<<RF_LO_VCO_VBIAS_CW_LEN)-1)<<RF_LO_VCO_VBIAS_CW_POS))
#define RF_LO_VCO_IDAC_BOOT                                     RF_LO_VCO_IDAC_BOOT
#define RF_LO_VCO_IDAC_BOOT_POS                                 (4U)
#define RF_LO_VCO_IDAC_BOOT_LEN                                 (1U)
#define RF_LO_VCO_IDAC_BOOT_MSK                                 (((1U<<RF_LO_VCO_IDAC_BOOT_LEN)-1)<<RF_LO_VCO_IDAC_BOOT_POS)
#define RF_LO_VCO_IDAC_BOOT_UMSK                                (~(((1U<<RF_LO_VCO_IDAC_BOOT_LEN)-1)<<RF_LO_VCO_IDAC_BOOT_POS))
#define RF_LO_VCO_SHORT_VBIAS_FILTER                            RF_LO_VCO_SHORT_VBIAS_FILTER
#define RF_LO_VCO_SHORT_VBIAS_FILTER_POS                        (5U)
#define RF_LO_VCO_SHORT_VBIAS_FILTER_LEN                        (1U)
#define RF_LO_VCO_SHORT_VBIAS_FILTER_MSK                        (((1U<<RF_LO_VCO_SHORT_VBIAS_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_VBIAS_FILTER_POS)
#define RF_LO_VCO_SHORT_VBIAS_FILTER_UMSK                       (~(((1U<<RF_LO_VCO_SHORT_VBIAS_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_VBIAS_FILTER_POS))
#define RF_LO_VCO_SHORT_IDAC_FILTER                             RF_LO_VCO_SHORT_IDAC_FILTER
#define RF_LO_VCO_SHORT_IDAC_FILTER_POS                         (6U)
#define RF_LO_VCO_SHORT_IDAC_FILTER_LEN                         (1U)
#define RF_LO_VCO_SHORT_IDAC_FILTER_MSK                         (((1U<<RF_LO_VCO_SHORT_IDAC_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_IDAC_FILTER_POS)
#define RF_LO_VCO_SHORT_IDAC_FILTER_UMSK                        (~(((1U<<RF_LO_VCO_SHORT_IDAC_FILTER_LEN)-1)<<RF_LO_VCO_SHORT_IDAC_FILTER_POS))
#define RF_ACAL_VREF_CW                                         RF_ACAL_VREF_CW
#define RF_ACAL_VREF_CW_POS                                     (8U)
#define RF_ACAL_VREF_CW_LEN                                     (3U)
#define RF_ACAL_VREF_CW_MSK                                     (((1U<<RF_ACAL_VREF_CW_LEN)-1)<<RF_ACAL_VREF_CW_POS)
#define RF_ACAL_VREF_CW_UMSK                                    (~(((1U<<RF_ACAL_VREF_CW_LEN)-1)<<RF_ACAL_VREF_CW_POS))
#define RF_ACAL_VCO_UD                                          RF_ACAL_VCO_UD
#define RF_ACAL_VCO_UD_POS                                      (12U)
#define RF_ACAL_VCO_UD_LEN                                      (1U)
#define RF_ACAL_VCO_UD_MSK                                      (((1U<<RF_ACAL_VCO_UD_LEN)-1)<<RF_ACAL_VCO_UD_POS)
#define RF_ACAL_VCO_UD_UMSK                                     (~(((1U<<RF_ACAL_VCO_UD_LEN)-1)<<RF_ACAL_VCO_UD_POS))
#define RF_ACAL_INC_EN_HW                                       RF_ACAL_INC_EN_HW
#define RF_ACAL_INC_EN_HW_POS                                   (16U)
#define RF_ACAL_INC_EN_HW_LEN                                   (1U)
#define RF_ACAL_INC_EN_HW_MSK                                   (((1U<<RF_ACAL_INC_EN_HW_LEN)-1)<<RF_ACAL_INC_EN_HW_POS)
#define RF_ACAL_INC_EN_HW_UMSK                                  (~(((1U<<RF_ACAL_INC_EN_HW_LEN)-1)<<RF_ACAL_INC_EN_HW_POS))

/* 0xA8 : vco3 */
#define RF_VCO3_OFFSET                                          (0xA8)
#define RF_FCAL_DIV                                             RF_FCAL_DIV
#define RF_FCAL_DIV_POS                                         (0U)
#define RF_FCAL_DIV_LEN                                         (16U)
#define RF_FCAL_DIV_MSK                                         (((1U<<RF_FCAL_DIV_LEN)-1)<<RF_FCAL_DIV_POS)
#define RF_FCAL_DIV_UMSK                                        (~(((1U<<RF_FCAL_DIV_LEN)-1)<<RF_FCAL_DIV_POS))
#define RF_FCAL_CNT_OP                                          RF_FCAL_CNT_OP
#define RF_FCAL_CNT_OP_POS                                      (16U)
#define RF_FCAL_CNT_OP_LEN                                      (16U)
#define RF_FCAL_CNT_OP_MSK                                      (((1U<<RF_FCAL_CNT_OP_LEN)-1)<<RF_FCAL_CNT_OP_POS)
#define RF_FCAL_CNT_OP_UMSK                                     (~(((1U<<RF_FCAL_CNT_OP_LEN)-1)<<RF_FCAL_CNT_OP_POS))

/* 0xAC : vco4 */
#define RF_VCO4_OFFSET                                          (0xAC)
#define RF_FCAL_CNT_START                                       RF_FCAL_CNT_START
#define RF_FCAL_CNT_START_POS                                   (4U)
#define RF_FCAL_CNT_START_LEN                                   (1U)
#define RF_FCAL_CNT_START_MSK                                   (((1U<<RF_FCAL_CNT_START_LEN)-1)<<RF_FCAL_CNT_START_POS)
#define RF_FCAL_CNT_START_UMSK                                  (~(((1U<<RF_FCAL_CNT_START_LEN)-1)<<RF_FCAL_CNT_START_POS))
#define RF_FCAL_INC_EN_HW                                       RF_FCAL_INC_EN_HW
#define RF_FCAL_INC_EN_HW_POS                                   (8U)
#define RF_FCAL_INC_EN_HW_LEN                                   (1U)
#define RF_FCAL_INC_EN_HW_MSK                                   (((1U<<RF_FCAL_INC_EN_HW_LEN)-1)<<RF_FCAL_INC_EN_HW_POS)
#define RF_FCAL_INC_EN_HW_UMSK                                  (~(((1U<<RF_FCAL_INC_EN_HW_LEN)-1)<<RF_FCAL_INC_EN_HW_POS))
#define RF_FCAL_INC_LARGE_RANGE                                 RF_FCAL_INC_LARGE_RANGE
#define RF_FCAL_INC_LARGE_RANGE_POS                             (16U)
#define RF_FCAL_INC_LARGE_RANGE_LEN                             (1U)
#define RF_FCAL_INC_LARGE_RANGE_MSK                             (((1U<<RF_FCAL_INC_LARGE_RANGE_LEN)-1)<<RF_FCAL_INC_LARGE_RANGE_POS)
#define RF_FCAL_INC_LARGE_RANGE_UMSK                            (~(((1U<<RF_FCAL_INC_LARGE_RANGE_LEN)-1)<<RF_FCAL_INC_LARGE_RANGE_POS))
#define RF_FCAL_CNT_RDY                                         RF_FCAL_CNT_RDY
#define RF_FCAL_CNT_RDY_POS                                     (20U)
#define RF_FCAL_CNT_RDY_LEN                                     (1U)
#define RF_FCAL_CNT_RDY_MSK                                     (((1U<<RF_FCAL_CNT_RDY_LEN)-1)<<RF_FCAL_CNT_RDY_POS)
#define RF_FCAL_CNT_RDY_UMSK                                    (~(((1U<<RF_FCAL_CNT_RDY_LEN)-1)<<RF_FCAL_CNT_RDY_POS))
#define RF_FCAL_INC_VCTRL_UD                                    RF_FCAL_INC_VCTRL_UD
#define RF_FCAL_INC_VCTRL_UD_POS                                (24U)
#define RF_FCAL_INC_VCTRL_UD_LEN                                (2U)
#define RF_FCAL_INC_VCTRL_UD_MSK                                (((1U<<RF_FCAL_INC_VCTRL_UD_LEN)-1)<<RF_FCAL_INC_VCTRL_UD_POS)
#define RF_FCAL_INC_VCTRL_UD_UMSK                               (~(((1U<<RF_FCAL_INC_VCTRL_UD_LEN)-1)<<RF_FCAL_INC_VCTRL_UD_POS))

/* 0xB0 : pfdcp */
#define RF_PFDCP_OFFSET                                         (0xB0)
#define RF_LO_CP_SEL                                            RF_LO_CP_SEL
#define RF_LO_CP_SEL_POS                                        (0U)
#define RF_LO_CP_SEL_LEN                                        (1U)
#define RF_LO_CP_SEL_MSK                                        (((1U<<RF_LO_CP_SEL_LEN)-1)<<RF_LO_CP_SEL_POS)
#define RF_LO_CP_SEL_UMSK                                       (~(((1U<<RF_LO_CP_SEL_LEN)-1)<<RF_LO_CP_SEL_POS))
#define RF_LO_CP_SEL_HW                                         RF_LO_CP_SEL_HW
#define RF_LO_CP_SEL_HW_POS                                     (4U)
#define RF_LO_CP_SEL_HW_LEN                                     (1U)
#define RF_LO_CP_SEL_HW_MSK                                     (((1U<<RF_LO_CP_SEL_HW_LEN)-1)<<RF_LO_CP_SEL_HW_POS)
#define RF_LO_CP_SEL_HW_UMSK                                    (~(((1U<<RF_LO_CP_SEL_HW_LEN)-1)<<RF_LO_CP_SEL_HW_POS))
#define RF_LO_CP_STARTUP_EN                                     RF_LO_CP_STARTUP_EN
#define RF_LO_CP_STARTUP_EN_POS                                 (8U)
#define RF_LO_CP_STARTUP_EN_LEN                                 (1U)
#define RF_LO_CP_STARTUP_EN_MSK                                 (((1U<<RF_LO_CP_STARTUP_EN_LEN)-1)<<RF_LO_CP_STARTUP_EN_POS)
#define RF_LO_CP_STARTUP_EN_UMSK                                (~(((1U<<RF_LO_CP_STARTUP_EN_LEN)-1)<<RF_LO_CP_STARTUP_EN_POS))
#define RF_LO_CP_OTA_EN                                         RF_LO_CP_OTA_EN
#define RF_LO_CP_OTA_EN_POS                                     (12U)
#define RF_LO_CP_OTA_EN_LEN                                     (1U)
#define RF_LO_CP_OTA_EN_MSK                                     (((1U<<RF_LO_CP_OTA_EN_LEN)-1)<<RF_LO_CP_OTA_EN_POS)
#define RF_LO_CP_OTA_EN_UMSK                                    (~(((1U<<RF_LO_CP_OTA_EN_LEN)-1)<<RF_LO_CP_OTA_EN_POS))
#define RF_LO_CP_OPAMP_EN                                       RF_LO_CP_OPAMP_EN
#define RF_LO_CP_OPAMP_EN_POS                                   (16U)
#define RF_LO_CP_OPAMP_EN_LEN                                   (1U)
#define RF_LO_CP_OPAMP_EN_MSK                                   (((1U<<RF_LO_CP_OPAMP_EN_LEN)-1)<<RF_LO_CP_OPAMP_EN_POS)
#define RF_LO_CP_OPAMP_EN_UMSK                                  (~(((1U<<RF_LO_CP_OPAMP_EN_LEN)-1)<<RF_LO_CP_OPAMP_EN_POS))
#define RF_LO_CP_HIZ                                            RF_LO_CP_HIZ
#define RF_LO_CP_HIZ_POS                                        (20U)
#define RF_LO_CP_HIZ_LEN                                        (1U)
#define RF_LO_CP_HIZ_MSK                                        (((1U<<RF_LO_CP_HIZ_LEN)-1)<<RF_LO_CP_HIZ_POS)
#define RF_LO_CP_HIZ_UMSK                                       (~(((1U<<RF_LO_CP_HIZ_LEN)-1)<<RF_LO_CP_HIZ_POS))
#define RF_LO_PFD_RVDD_BOOST                                    RF_LO_PFD_RVDD_BOOST
#define RF_LO_PFD_RVDD_BOOST_POS                                (24U)
#define RF_LO_PFD_RVDD_BOOST_LEN                                (1U)
#define RF_LO_PFD_RVDD_BOOST_MSK                                (((1U<<RF_LO_PFD_RVDD_BOOST_LEN)-1)<<RF_LO_PFD_RVDD_BOOST_POS)
#define RF_LO_PFD_RVDD_BOOST_UMSK                               (~(((1U<<RF_LO_PFD_RVDD_BOOST_LEN)-1)<<RF_LO_PFD_RVDD_BOOST_POS))
#define RF_LO_PFD_RST_CSD                                       RF_LO_PFD_RST_CSD
#define RF_LO_PFD_RST_CSD_POS                                   (28U)
#define RF_LO_PFD_RST_CSD_LEN                                   (1U)
#define RF_LO_PFD_RST_CSD_MSK                                   (((1U<<RF_LO_PFD_RST_CSD_LEN)-1)<<RF_LO_PFD_RST_CSD_POS)
#define RF_LO_PFD_RST_CSD_UMSK                                  (~(((1U<<RF_LO_PFD_RST_CSD_LEN)-1)<<RF_LO_PFD_RST_CSD_POS))
#define RF_LO_PFD_RST_CSD_HW                                    RF_LO_PFD_RST_CSD_HW
#define RF_LO_PFD_RST_CSD_HW_POS                                (29U)
#define RF_LO_PFD_RST_CSD_HW_LEN                                (1U)
#define RF_LO_PFD_RST_CSD_HW_MSK                                (((1U<<RF_LO_PFD_RST_CSD_HW_LEN)-1)<<RF_LO_PFD_RST_CSD_HW_POS)
#define RF_LO_PFD_RST_CSD_HW_UMSK                               (~(((1U<<RF_LO_PFD_RST_CSD_HW_LEN)-1)<<RF_LO_PFD_RST_CSD_HW_POS))

/* 0xB4 : lo */
#define RF_LO_OFFSET                                            (0xB4)
#define RF_LO_LF_RZ_HW                                          RF_LO_LF_RZ_HW
#define RF_LO_LF_RZ_HW_POS                                      (0U)
#define RF_LO_LF_RZ_HW_LEN                                      (2U)
#define RF_LO_LF_RZ_HW_MSK                                      (((1U<<RF_LO_LF_RZ_HW_LEN)-1)<<RF_LO_LF_RZ_HW_POS)
#define RF_LO_LF_RZ_HW_UMSK                                     (~(((1U<<RF_LO_LF_RZ_HW_LEN)-1)<<RF_LO_LF_RZ_HW_POS))
#define RF_LO_LF_R4_HW                                          RF_LO_LF_R4_HW
#define RF_LO_LF_R4_HW_POS                                      (4U)
#define RF_LO_LF_R4_HW_LEN                                      (2U)
#define RF_LO_LF_R4_HW_MSK                                      (((1U<<RF_LO_LF_R4_HW_LEN)-1)<<RF_LO_LF_R4_HW_POS)
#define RF_LO_LF_R4_HW_UMSK                                     (~(((1U<<RF_LO_LF_R4_HW_LEN)-1)<<RF_LO_LF_R4_HW_POS))
#define RF_LO_LF_CZ_HW                                          RF_LO_LF_CZ_HW
#define RF_LO_LF_CZ_HW_POS                                      (8U)
#define RF_LO_LF_CZ_HW_LEN                                      (2U)
#define RF_LO_LF_CZ_HW_MSK                                      (((1U<<RF_LO_LF_CZ_HW_LEN)-1)<<RF_LO_LF_CZ_HW_POS)
#define RF_LO_LF_CZ_HW_UMSK                                     (~(((1U<<RF_LO_LF_CZ_HW_LEN)-1)<<RF_LO_LF_CZ_HW_POS))
#define RF_LO_LF_RZ                                             RF_LO_LF_RZ
#define RF_LO_LF_RZ_POS                                         (12U)
#define RF_LO_LF_RZ_LEN                                         (2U)
#define RF_LO_LF_RZ_MSK                                         (((1U<<RF_LO_LF_RZ_LEN)-1)<<RF_LO_LF_RZ_POS)
#define RF_LO_LF_RZ_UMSK                                        (~(((1U<<RF_LO_LF_RZ_LEN)-1)<<RF_LO_LF_RZ_POS))
#define RF_LO_LF_CZ                                             RF_LO_LF_CZ
#define RF_LO_LF_CZ_POS                                         (14U)
#define RF_LO_LF_CZ_LEN                                         (2U)
#define RF_LO_LF_CZ_MSK                                         (((1U<<RF_LO_LF_CZ_LEN)-1)<<RF_LO_LF_CZ_POS)
#define RF_LO_LF_CZ_UMSK                                        (~(((1U<<RF_LO_LF_CZ_LEN)-1)<<RF_LO_LF_CZ_POS))
#define RF_LO_LF_R4                                             RF_LO_LF_R4
#define RF_LO_LF_R4_POS                                         (16U)
#define RF_LO_LF_R4_LEN                                         (2U)
#define RF_LO_LF_R4_MSK                                         (((1U<<RF_LO_LF_R4_LEN)-1)<<RF_LO_LF_R4_POS)
#define RF_LO_LF_R4_UMSK                                        (~(((1U<<RF_LO_LF_R4_LEN)-1)<<RF_LO_LF_R4_POS))
#define RF_LO_LF_R4_SHORT                                       RF_LO_LF_R4_SHORT
#define RF_LO_LF_R4_SHORT_POS                                   (18U)
#define RF_LO_LF_R4_SHORT_LEN                                   (1U)
#define RF_LO_LF_R4_SHORT_MSK                                   (((1U<<RF_LO_LF_R4_SHORT_LEN)-1)<<RF_LO_LF_R4_SHORT_POS)
#define RF_LO_LF_R4_SHORT_UMSK                                  (~(((1U<<RF_LO_LF_R4_SHORT_LEN)-1)<<RF_LO_LF_R4_SHORT_POS))
#define RF_LO_SLIPPED_DN                                        RF_LO_SLIPPED_DN
#define RF_LO_SLIPPED_DN_POS                                    (20U)
#define RF_LO_SLIPPED_DN_LEN                                    (1U)
#define RF_LO_SLIPPED_DN_MSK                                    (((1U<<RF_LO_SLIPPED_DN_LEN)-1)<<RF_LO_SLIPPED_DN_POS)
#define RF_LO_SLIPPED_DN_UMSK                                   (~(((1U<<RF_LO_SLIPPED_DN_LEN)-1)<<RF_LO_SLIPPED_DN_POS))
#define RF_LO_SLIPPED_UP                                        RF_LO_SLIPPED_UP
#define RF_LO_SLIPPED_UP_POS                                    (24U)
#define RF_LO_SLIPPED_UP_LEN                                    (1U)
#define RF_LO_SLIPPED_UP_MSK                                    (((1U<<RF_LO_SLIPPED_UP_LEN)-1)<<RF_LO_SLIPPED_UP_POS)
#define RF_LO_SLIPPED_UP_UMSK                                   (~(((1U<<RF_LO_SLIPPED_UP_LEN)-1)<<RF_LO_SLIPPED_UP_POS))

/* 0xB8 : fbdv */
#define RF_FBDV_OFFSET                                          (0xB8)
#define RF_LO_FBDV_HALFSTEP_EN_HW                               RF_LO_FBDV_HALFSTEP_EN_HW
#define RF_LO_FBDV_HALFSTEP_EN_HW_POS                           (0U)
#define RF_LO_FBDV_HALFSTEP_EN_HW_LEN                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_HW_MSK                           (((1U<<RF_LO_FBDV_HALFSTEP_EN_HW_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_HW_POS)
#define RF_LO_FBDV_HALFSTEP_EN_HW_UMSK                          (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_HW_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_HW_POS))
#define RF_LO_FBDV_HALFSTEP_EN                                  RF_LO_FBDV_HALFSTEP_EN
#define RF_LO_FBDV_HALFSTEP_EN_POS                              (4U)
#define RF_LO_FBDV_HALFSTEP_EN_LEN                              (1U)
#define RF_LO_FBDV_HALFSTEP_EN_MSK                              (((1U<<RF_LO_FBDV_HALFSTEP_EN_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_POS)
#define RF_LO_FBDV_HALFSTEP_EN_UMSK                             (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_POS))
#define RF_LO_FBDV_SEL_SAMPLE_CLK                               RF_LO_FBDV_SEL_SAMPLE_CLK
#define RF_LO_FBDV_SEL_SAMPLE_CLK_POS                           (8U)
#define RF_LO_FBDV_SEL_SAMPLE_CLK_LEN                           (2U)
#define RF_LO_FBDV_SEL_SAMPLE_CLK_MSK                           (((1U<<RF_LO_FBDV_SEL_SAMPLE_CLK_LEN)-1)<<RF_LO_FBDV_SEL_SAMPLE_CLK_POS)
#define RF_LO_FBDV_SEL_SAMPLE_CLK_UMSK                          (~(((1U<<RF_LO_FBDV_SEL_SAMPLE_CLK_LEN)-1)<<RF_LO_FBDV_SEL_SAMPLE_CLK_POS))
#define RF_LO_FBDV_SEL_FB_CLK                                   RF_LO_FBDV_SEL_FB_CLK
#define RF_LO_FBDV_SEL_FB_CLK_POS                               (12U)
#define RF_LO_FBDV_SEL_FB_CLK_LEN                               (2U)
#define RF_LO_FBDV_SEL_FB_CLK_MSK                               (((1U<<RF_LO_FBDV_SEL_FB_CLK_LEN)-1)<<RF_LO_FBDV_SEL_FB_CLK_POS)
#define RF_LO_FBDV_SEL_FB_CLK_UMSK                              (~(((1U<<RF_LO_FBDV_SEL_FB_CLK_LEN)-1)<<RF_LO_FBDV_SEL_FB_CLK_POS))
#define RF_LO_FBDV_RST                                          RF_LO_FBDV_RST
#define RF_LO_FBDV_RST_POS                                      (16U)
#define RF_LO_FBDV_RST_LEN                                      (1U)
#define RF_LO_FBDV_RST_MSK                                      (((1U<<RF_LO_FBDV_RST_LEN)-1)<<RF_LO_FBDV_RST_POS)
#define RF_LO_FBDV_RST_UMSK                                     (~(((1U<<RF_LO_FBDV_RST_LEN)-1)<<RF_LO_FBDV_RST_POS))
#define RF_LO_FBDV_RST_HW                                       RF_LO_FBDV_RST_HW
#define RF_LO_FBDV_RST_HW_POS                                   (20U)
#define RF_LO_FBDV_RST_HW_LEN                                   (1U)
#define RF_LO_FBDV_RST_HW_MSK                                   (((1U<<RF_LO_FBDV_RST_HW_LEN)-1)<<RF_LO_FBDV_RST_HW_POS)
#define RF_LO_FBDV_RST_HW_UMSK                                  (~(((1U<<RF_LO_FBDV_RST_HW_LEN)-1)<<RF_LO_FBDV_RST_HW_POS))

/* 0xBC : lodist */
#define RF_LODIST_OFFSET                                        (0xBC)
#define RF_LO_OSMX_XGM_BOOST                                    RF_LO_OSMX_XGM_BOOST
#define RF_LO_OSMX_XGM_BOOST_POS                                (0U)
#define RF_LO_OSMX_XGM_BOOST_LEN                                (1U)
#define RF_LO_OSMX_XGM_BOOST_MSK                                (((1U<<RF_LO_OSMX_XGM_BOOST_LEN)-1)<<RF_LO_OSMX_XGM_BOOST_POS)
#define RF_LO_OSMX_XGM_BOOST_UMSK                               (~(((1U<<RF_LO_OSMX_XGM_BOOST_LEN)-1)<<RF_LO_OSMX_XGM_BOOST_POS))
#define RF_LO_OSMX_EN_XGM                                       RF_LO_OSMX_EN_XGM
#define RF_LO_OSMX_EN_XGM_POS                                   (4U)
#define RF_LO_OSMX_EN_XGM_LEN                                   (1U)
#define RF_LO_OSMX_EN_XGM_MSK                                   (((1U<<RF_LO_OSMX_EN_XGM_LEN)-1)<<RF_LO_OSMX_EN_XGM_POS)
#define RF_LO_OSMX_EN_XGM_UMSK                                  (~(((1U<<RF_LO_OSMX_EN_XGM_LEN)-1)<<RF_LO_OSMX_EN_XGM_POS))
#define RF_LO_OSMX_FIX_CAP                                      RF_LO_OSMX_FIX_CAP
#define RF_LO_OSMX_FIX_CAP_POS                                  (8U)
#define RF_LO_OSMX_FIX_CAP_LEN                                  (1U)
#define RF_LO_OSMX_FIX_CAP_MSK                                  (((1U<<RF_LO_OSMX_FIX_CAP_LEN)-1)<<RF_LO_OSMX_FIX_CAP_POS)
#define RF_LO_OSMX_FIX_CAP_UMSK                                 (~(((1U<<RF_LO_OSMX_FIX_CAP_LEN)-1)<<RF_LO_OSMX_FIX_CAP_POS))
#define RF_LO_OSMX_VBUF_STRE                                    RF_LO_OSMX_VBUF_STRE
#define RF_LO_OSMX_VBUF_STRE_POS                                (12U)
#define RF_LO_OSMX_VBUF_STRE_LEN                                (1U)
#define RF_LO_OSMX_VBUF_STRE_MSK                                (((1U<<RF_LO_OSMX_VBUF_STRE_LEN)-1)<<RF_LO_OSMX_VBUF_STRE_POS)
#define RF_LO_OSMX_VBUF_STRE_UMSK                               (~(((1U<<RF_LO_OSMX_VBUF_STRE_LEN)-1)<<RF_LO_OSMX_VBUF_STRE_POS))
#define RF_LO_OSMX_CAPBANK_BIAS                                 RF_LO_OSMX_CAPBANK_BIAS
#define RF_LO_OSMX_CAPBANK_BIAS_POS                             (16U)
#define RF_LO_OSMX_CAPBANK_BIAS_LEN                             (2U)
#define RF_LO_OSMX_CAPBANK_BIAS_MSK                             (((1U<<RF_LO_OSMX_CAPBANK_BIAS_LEN)-1)<<RF_LO_OSMX_CAPBANK_BIAS_POS)
#define RF_LO_OSMX_CAPBANK_BIAS_UMSK                            (~(((1U<<RF_LO_OSMX_CAPBANK_BIAS_LEN)-1)<<RF_LO_OSMX_CAPBANK_BIAS_POS))
#define RF_LO_OSMX_CAP                                          RF_LO_OSMX_CAP
#define RF_LO_OSMX_CAP_POS                                      (20U)
#define RF_LO_OSMX_CAP_LEN                                      (4U)
#define RF_LO_OSMX_CAP_MSK                                      (((1U<<RF_LO_OSMX_CAP_LEN)-1)<<RF_LO_OSMX_CAP_POS)
#define RF_LO_OSMX_CAP_UMSK                                     (~(((1U<<RF_LO_OSMX_CAP_LEN)-1)<<RF_LO_OSMX_CAP_POS))
#define RF_LO_LODIST_TXBUF_STRE                                 RF_LO_LODIST_TXBUF_STRE
#define RF_LO_LODIST_TXBUF_STRE_POS                             (24U)
#define RF_LO_LODIST_TXBUF_STRE_LEN                             (1U)
#define RF_LO_LODIST_TXBUF_STRE_MSK                             (((1U<<RF_LO_LODIST_TXBUF_STRE_LEN)-1)<<RF_LO_LODIST_TXBUF_STRE_POS)
#define RF_LO_LODIST_TXBUF_STRE_UMSK                            (~(((1U<<RF_LO_LODIST_TXBUF_STRE_LEN)-1)<<RF_LO_LODIST_TXBUF_STRE_POS))
#define RF_LO_LODIST_RXBUF_STRE                                 RF_LO_LODIST_RXBUF_STRE
#define RF_LO_LODIST_RXBUF_STRE_POS                             (28U)
#define RF_LO_LODIST_RXBUF_STRE_LEN                             (1U)
#define RF_LO_LODIST_RXBUF_STRE_MSK                             (((1U<<RF_LO_LODIST_RXBUF_STRE_LEN)-1)<<RF_LO_LODIST_RXBUF_STRE_POS)
#define RF_LO_LODIST_RXBUF_STRE_UMSK                            (~(((1U<<RF_LO_LODIST_RXBUF_STRE_LEN)-1)<<RF_LO_LODIST_RXBUF_STRE_POS))

/* 0xC0 : sdm1 */
#define RF_SDM1_OFFSET                                          (0xC0)
#define RF_LO_SDM_DITHER_SEL_HW                                 RF_LO_SDM_DITHER_SEL_HW
#define RF_LO_SDM_DITHER_SEL_HW_POS                             (0U)
#define RF_LO_SDM_DITHER_SEL_HW_LEN                             (2U)
#define RF_LO_SDM_DITHER_SEL_HW_MSK                             (((1U<<RF_LO_SDM_DITHER_SEL_HW_LEN)-1)<<RF_LO_SDM_DITHER_SEL_HW_POS)
#define RF_LO_SDM_DITHER_SEL_HW_UMSK                            (~(((1U<<RF_LO_SDM_DITHER_SEL_HW_LEN)-1)<<RF_LO_SDM_DITHER_SEL_HW_POS))
#define RF_LO_SDM_BYPASS_HW                                     RF_LO_SDM_BYPASS_HW
#define RF_LO_SDM_BYPASS_HW_POS                                 (4U)
#define RF_LO_SDM_BYPASS_HW_LEN                                 (1U)
#define RF_LO_SDM_BYPASS_HW_MSK                                 (((1U<<RF_LO_SDM_BYPASS_HW_LEN)-1)<<RF_LO_SDM_BYPASS_HW_POS)
#define RF_LO_SDM_BYPASS_HW_UMSK                                (~(((1U<<RF_LO_SDM_BYPASS_HW_LEN)-1)<<RF_LO_SDM_BYPASS_HW_POS))
#define RF_LO_SDM_DITHER_SEL                                    RF_LO_SDM_DITHER_SEL
#define RF_LO_SDM_DITHER_SEL_POS                                (8U)
#define RF_LO_SDM_DITHER_SEL_LEN                                (2U)
#define RF_LO_SDM_DITHER_SEL_MSK                                (((1U<<RF_LO_SDM_DITHER_SEL_LEN)-1)<<RF_LO_SDM_DITHER_SEL_POS)
#define RF_LO_SDM_DITHER_SEL_UMSK                               (~(((1U<<RF_LO_SDM_DITHER_SEL_LEN)-1)<<RF_LO_SDM_DITHER_SEL_POS))
#define RF_LO_SDM_BYPASS                                        RF_LO_SDM_BYPASS
#define RF_LO_SDM_BYPASS_POS                                    (12U)
#define RF_LO_SDM_BYPASS_LEN                                    (1U)
#define RF_LO_SDM_BYPASS_MSK                                    (((1U<<RF_LO_SDM_BYPASS_LEN)-1)<<RF_LO_SDM_BYPASS_POS)
#define RF_LO_SDM_BYPASS_UMSK                                   (~(((1U<<RF_LO_SDM_BYPASS_LEN)-1)<<RF_LO_SDM_BYPASS_POS))
#define RF_LO_SDM_RSTB                                          RF_LO_SDM_RSTB
#define RF_LO_SDM_RSTB_POS                                      (16U)
#define RF_LO_SDM_RSTB_LEN                                      (1U)
#define RF_LO_SDM_RSTB_MSK                                      (((1U<<RF_LO_SDM_RSTB_LEN)-1)<<RF_LO_SDM_RSTB_POS)
#define RF_LO_SDM_RSTB_UMSK                                     (~(((1U<<RF_LO_SDM_RSTB_LEN)-1)<<RF_LO_SDM_RSTB_POS))
#define RF_LO_SDM_RSTB_HW                                       RF_LO_SDM_RSTB_HW
#define RF_LO_SDM_RSTB_HW_POS                                   (17U)
#define RF_LO_SDM_RSTB_HW_LEN                                   (1U)
#define RF_LO_SDM_RSTB_HW_MSK                                   (((1U<<RF_LO_SDM_RSTB_HW_LEN)-1)<<RF_LO_SDM_RSTB_HW_POS)
#define RF_LO_SDM_RSTB_HW_UMSK                                  (~(((1U<<RF_LO_SDM_RSTB_HW_LEN)-1)<<RF_LO_SDM_RSTB_HW_POS))
#define RF_LO_SDM_FLAG                                          RF_LO_SDM_FLAG
#define RF_LO_SDM_FLAG_POS                                      (20U)
#define RF_LO_SDM_FLAG_LEN                                      (1U)
#define RF_LO_SDM_FLAG_MSK                                      (((1U<<RF_LO_SDM_FLAG_LEN)-1)<<RF_LO_SDM_FLAG_POS)
#define RF_LO_SDM_FLAG_UMSK                                     (~(((1U<<RF_LO_SDM_FLAG_LEN)-1)<<RF_LO_SDM_FLAG_POS))

/* 0xC4 : sdm2 */
#define RF_SDM2_OFFSET                                          (0xC4)
#define RF_LO_SDMIN                                             RF_LO_SDMIN
#define RF_LO_SDMIN_POS                                         (0U)
#define RF_LO_SDMIN_LEN                                         (30U)
#define RF_LO_SDMIN_MSK                                         (((1U<<RF_LO_SDMIN_LEN)-1)<<RF_LO_SDMIN_POS)
#define RF_LO_SDMIN_UMSK                                        (~(((1U<<RF_LO_SDMIN_LEN)-1)<<RF_LO_SDMIN_POS))

/* 0xC8 : sdm3 */
#define RF_SDM3_OFFSET                                          (0xC8)
#define RF_LO_SDMIN_HW                                          RF_LO_SDMIN_HW
#define RF_LO_SDMIN_HW_POS                                      (0U)
#define RF_LO_SDMIN_HW_LEN                                      (30U)
#define RF_LO_SDMIN_HW_MSK                                      (((1U<<RF_LO_SDMIN_HW_LEN)-1)<<RF_LO_SDMIN_HW_POS)
#define RF_LO_SDMIN_HW_UMSK                                     (~(((1U<<RF_LO_SDMIN_HW_LEN)-1)<<RF_LO_SDMIN_HW_POS))

/* 0xEC : rf_resv_reg_0 */
#define RF_RESV_REG_0_OFFSET                                    (0xEC)
#define RF_RESERVED0                                            RF_RESERVED0
#define RF_RESERVED0_POS                                        (0U)
#define RF_RESERVED0_LEN                                        (32U)
#define RF_RESERVED0_MSK                                        (((1U<<RF_RESERVED0_LEN)-1)<<RF_RESERVED0_POS)
#define RF_RESERVED0_UMSK                                       (~(((1U<<RF_RESERVED0_LEN)-1)<<RF_RESERVED0_POS))

/* 0xF0 : rf_resv_reg_1 */
#define RF_RESV_REG_1_OFFSET                                    (0xF0)
#define RF_RESERVED1                                            RF_RESERVED1
#define RF_RESERVED1_POS                                        (0U)
#define RF_RESERVED1_LEN                                        (32U)
#define RF_RESERVED1_MSK                                        (((1U<<RF_RESERVED1_LEN)-1)<<RF_RESERVED1_POS)
#define RF_RESERVED1_UMSK                                       (~(((1U<<RF_RESERVED1_LEN)-1)<<RF_RESERVED1_POS))

/* 0xF4 : rf_resv_reg_2 */
#define RF_RESV_REG_2_OFFSET                                    (0xF4)
#define RF_RESERVED2                                            RF_RESERVED2
#define RF_RESERVED2_POS                                        (0U)
#define RF_RESERVED2_LEN                                        (32U)
#define RF_RESERVED2_MSK                                        (((1U<<RF_RESERVED2_LEN)-1)<<RF_RESERVED2_POS)
#define RF_RESERVED2_UMSK                                       (~(((1U<<RF_RESERVED2_LEN)-1)<<RF_RESERVED2_POS))

/* 0xF8 : rrf_gain_index1 */
#define RF_RRF_GAIN_INDEX1_OFFSET                               (0xF8)
#define RF_GAIN_CTRL0_GC_RMXGM                                  RF_GAIN_CTRL0_GC_RMXGM
#define RF_GAIN_CTRL0_GC_RMXGM_POS                              (0U)
#define RF_GAIN_CTRL0_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL0_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL0_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL0_GC_RMXGM_POS)
#define RF_GAIN_CTRL0_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL0_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL0_GC_RMXGM_POS))
#define RF_GAIN_CTRL0_GC_LNA                                    RF_GAIN_CTRL0_GC_LNA
#define RF_GAIN_CTRL0_GC_LNA_POS                                (2U)
#define RF_GAIN_CTRL0_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL0_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL0_GC_LNA_LEN)-1)<<RF_GAIN_CTRL0_GC_LNA_POS)
#define RF_GAIN_CTRL0_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL0_GC_LNA_LEN)-1)<<RF_GAIN_CTRL0_GC_LNA_POS))
#define RF_GAIN_CTRL1_GC_RMXGM                                  RF_GAIN_CTRL1_GC_RMXGM
#define RF_GAIN_CTRL1_GC_RMXGM_POS                              (5U)
#define RF_GAIN_CTRL1_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL1_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL1_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL1_GC_RMXGM_POS)
#define RF_GAIN_CTRL1_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL1_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL1_GC_RMXGM_POS))
#define RF_GAIN_CTRL1_GC_LNA                                    RF_GAIN_CTRL1_GC_LNA
#define RF_GAIN_CTRL1_GC_LNA_POS                                (7U)
#define RF_GAIN_CTRL1_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL1_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL1_GC_LNA_LEN)-1)<<RF_GAIN_CTRL1_GC_LNA_POS)
#define RF_GAIN_CTRL1_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL1_GC_LNA_LEN)-1)<<RF_GAIN_CTRL1_GC_LNA_POS))
#define RF_GAIN_CTRL2_GC_RMXGM                                  RF_GAIN_CTRL2_GC_RMXGM
#define RF_GAIN_CTRL2_GC_RMXGM_POS                              (10U)
#define RF_GAIN_CTRL2_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL2_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL2_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL2_GC_RMXGM_POS)
#define RF_GAIN_CTRL2_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL2_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL2_GC_RMXGM_POS))
#define RF_GAIN_CTRL2_GC_LNA                                    RF_GAIN_CTRL2_GC_LNA
#define RF_GAIN_CTRL2_GC_LNA_POS                                (12U)
#define RF_GAIN_CTRL2_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL2_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL2_GC_LNA_LEN)-1)<<RF_GAIN_CTRL2_GC_LNA_POS)
#define RF_GAIN_CTRL2_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL2_GC_LNA_LEN)-1)<<RF_GAIN_CTRL2_GC_LNA_POS))
#define RF_GAIN_CTRL3_GC_RMXGM                                  RF_GAIN_CTRL3_GC_RMXGM
#define RF_GAIN_CTRL3_GC_RMXGM_POS                              (15U)
#define RF_GAIN_CTRL3_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL3_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL3_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL3_GC_RMXGM_POS)
#define RF_GAIN_CTRL3_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL3_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL3_GC_RMXGM_POS))
#define RF_GAIN_CTRL3_GC_LNA                                    RF_GAIN_CTRL3_GC_LNA
#define RF_GAIN_CTRL3_GC_LNA_POS                                (17U)
#define RF_GAIN_CTRL3_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL3_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL3_GC_LNA_LEN)-1)<<RF_GAIN_CTRL3_GC_LNA_POS)
#define RF_GAIN_CTRL3_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL3_GC_LNA_LEN)-1)<<RF_GAIN_CTRL3_GC_LNA_POS))
#define RF_GAIN_CTRL4_GC_RMXGM                                  RF_GAIN_CTRL4_GC_RMXGM
#define RF_GAIN_CTRL4_GC_RMXGM_POS                              (20U)
#define RF_GAIN_CTRL4_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL4_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL4_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL4_GC_RMXGM_POS)
#define RF_GAIN_CTRL4_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL4_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL4_GC_RMXGM_POS))
#define RF_GAIN_CTRL4_GC_LNA                                    RF_GAIN_CTRL4_GC_LNA
#define RF_GAIN_CTRL4_GC_LNA_POS                                (22U)
#define RF_GAIN_CTRL4_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL4_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL4_GC_LNA_LEN)-1)<<RF_GAIN_CTRL4_GC_LNA_POS)
#define RF_GAIN_CTRL4_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL4_GC_LNA_LEN)-1)<<RF_GAIN_CTRL4_GC_LNA_POS))
#define RF_GAIN_CTRL5_GC_RMXGM                                  RF_GAIN_CTRL5_GC_RMXGM
#define RF_GAIN_CTRL5_GC_RMXGM_POS                              (25U)
#define RF_GAIN_CTRL5_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL5_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL5_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL5_GC_RMXGM_POS)
#define RF_GAIN_CTRL5_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL5_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL5_GC_RMXGM_POS))
#define RF_GAIN_CTRL5_GC_LNA                                    RF_GAIN_CTRL5_GC_LNA
#define RF_GAIN_CTRL5_GC_LNA_POS                                (27U)
#define RF_GAIN_CTRL5_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL5_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL5_GC_LNA_LEN)-1)<<RF_GAIN_CTRL5_GC_LNA_POS)
#define RF_GAIN_CTRL5_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL5_GC_LNA_LEN)-1)<<RF_GAIN_CTRL5_GC_LNA_POS))

/* 0xFC : rrf_gain_index2 */
#define RF_RRF_GAIN_INDEX2_OFFSET                               (0xFC)
#define RF_GAIN_CTRL8_GC_RMXGM                                  RF_GAIN_CTRL8_GC_RMXGM
#define RF_GAIN_CTRL8_GC_RMXGM_POS                              (0U)
#define RF_GAIN_CTRL8_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL8_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL8_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL8_GC_RMXGM_POS)
#define RF_GAIN_CTRL8_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL8_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL8_GC_RMXGM_POS))
#define RF_GAIN_CTRL8_GC_LNA                                    RF_GAIN_CTRL8_GC_LNA
#define RF_GAIN_CTRL8_GC_LNA_POS                                (2U)
#define RF_GAIN_CTRL8_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL8_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL8_GC_LNA_LEN)-1)<<RF_GAIN_CTRL8_GC_LNA_POS)
#define RF_GAIN_CTRL8_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL8_GC_LNA_LEN)-1)<<RF_GAIN_CTRL8_GC_LNA_POS))
#define RF_GAIN_CTRL7_GC_RMXGM                                  RF_GAIN_CTRL7_GC_RMXGM
#define RF_GAIN_CTRL7_GC_RMXGM_POS                              (5U)
#define RF_GAIN_CTRL7_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL7_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL7_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL7_GC_RMXGM_POS)
#define RF_GAIN_CTRL7_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL7_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL7_GC_RMXGM_POS))
#define RF_GAIN_CTRL7_GC_LNA                                    RF_GAIN_CTRL7_GC_LNA
#define RF_GAIN_CTRL7_GC_LNA_POS                                (7U)
#define RF_GAIN_CTRL7_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL7_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL7_GC_LNA_LEN)-1)<<RF_GAIN_CTRL7_GC_LNA_POS)
#define RF_GAIN_CTRL7_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL7_GC_LNA_LEN)-1)<<RF_GAIN_CTRL7_GC_LNA_POS))
#define RF_GAIN_CTRL6_GC_RMXGM                                  RF_GAIN_CTRL6_GC_RMXGM
#define RF_GAIN_CTRL6_GC_RMXGM_POS                              (10U)
#define RF_GAIN_CTRL6_GC_RMXGM_LEN                              (2U)
#define RF_GAIN_CTRL6_GC_RMXGM_MSK                              (((1U<<RF_GAIN_CTRL6_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL6_GC_RMXGM_POS)
#define RF_GAIN_CTRL6_GC_RMXGM_UMSK                             (~(((1U<<RF_GAIN_CTRL6_GC_RMXGM_LEN)-1)<<RF_GAIN_CTRL6_GC_RMXGM_POS))
#define RF_GAIN_CTRL6_GC_LNA                                    RF_GAIN_CTRL6_GC_LNA
#define RF_GAIN_CTRL6_GC_LNA_POS                                (12U)
#define RF_GAIN_CTRL6_GC_LNA_LEN                                (3U)
#define RF_GAIN_CTRL6_GC_LNA_MSK                                (((1U<<RF_GAIN_CTRL6_GC_LNA_LEN)-1)<<RF_GAIN_CTRL6_GC_LNA_POS)
#define RF_GAIN_CTRL6_GC_LNA_UMSK                               (~(((1U<<RF_GAIN_CTRL6_GC_LNA_LEN)-1)<<RF_GAIN_CTRL6_GC_LNA_POS))

/* 0x100 : lna_ctrl_hw_mux */
#define RF_LNA_CTRL_HW_MUX_OFFSET                               (0x100)
#define RF_LNA_BM_HG                                            RF_LNA_BM_HG
#define RF_LNA_BM_HG_POS                                        (0U)
#define RF_LNA_BM_HG_LEN                                        (4U)
#define RF_LNA_BM_HG_MSK                                        (((1U<<RF_LNA_BM_HG_LEN)-1)<<RF_LNA_BM_HG_POS)
#define RF_LNA_BM_HG_UMSK                                       (~(((1U<<RF_LNA_BM_HG_LEN)-1)<<RF_LNA_BM_HG_POS))
#define RF_LNA_BM_LG                                            RF_LNA_BM_LG
#define RF_LNA_BM_LG_POS                                        (4U)
#define RF_LNA_BM_LG_LEN                                        (4U)
#define RF_LNA_BM_LG_MSK                                        (((1U<<RF_LNA_BM_LG_LEN)-1)<<RF_LNA_BM_LG_POS)
#define RF_LNA_BM_LG_UMSK                                       (~(((1U<<RF_LNA_BM_LG_LEN)-1)<<RF_LNA_BM_LG_POS))
#define RF_LNA_LOAD_CSW_HG                                      RF_LNA_LOAD_CSW_HG
#define RF_LNA_LOAD_CSW_HG_POS                                  (8U)
#define RF_LNA_LOAD_CSW_HG_LEN                                  (4U)
#define RF_LNA_LOAD_CSW_HG_MSK                                  (((1U<<RF_LNA_LOAD_CSW_HG_LEN)-1)<<RF_LNA_LOAD_CSW_HG_POS)
#define RF_LNA_LOAD_CSW_HG_UMSK                                 (~(((1U<<RF_LNA_LOAD_CSW_HG_LEN)-1)<<RF_LNA_LOAD_CSW_HG_POS))
#define RF_LNA_LOAD_CSW_LG                                      RF_LNA_LOAD_CSW_LG
#define RF_LNA_LOAD_CSW_LG_POS                                  (12U)
#define RF_LNA_LOAD_CSW_LG_LEN                                  (4U)
#define RF_LNA_LOAD_CSW_LG_MSK                                  (((1U<<RF_LNA_LOAD_CSW_LG_LEN)-1)<<RF_LNA_LOAD_CSW_LG_POS)
#define RF_LNA_LOAD_CSW_LG_UMSK                                 (~(((1U<<RF_LNA_LOAD_CSW_LG_LEN)-1)<<RF_LNA_LOAD_CSW_LG_POS))

/* 0x104 : rbb_gain_index1 */
#define RF_RBB_GAIN_INDEX1_OFFSET                               (0x104)
#define RF_GAIN_CTRL0_GC_RBB1                                   RF_GAIN_CTRL0_GC_RBB1
#define RF_GAIN_CTRL0_GC_RBB1_POS                               (0U)
#define RF_GAIN_CTRL0_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL0_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL0_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB1_POS)
#define RF_GAIN_CTRL0_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL0_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB1_POS))
#define RF_GAIN_CTRL0_GC_RBB2                                   RF_GAIN_CTRL0_GC_RBB2
#define RF_GAIN_CTRL0_GC_RBB2_POS                               (4U)
#define RF_GAIN_CTRL0_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL0_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL0_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB2_POS)
#define RF_GAIN_CTRL0_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL0_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL0_GC_RBB2_POS))
#define RF_GAIN_CTRL1_GC_RBB1                                   RF_GAIN_CTRL1_GC_RBB1
#define RF_GAIN_CTRL1_GC_RBB1_POS                               (8U)
#define RF_GAIN_CTRL1_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL1_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL1_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB1_POS)
#define RF_GAIN_CTRL1_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL1_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB1_POS))
#define RF_GAIN_CTRL1_GC_RBB2                                   RF_GAIN_CTRL1_GC_RBB2
#define RF_GAIN_CTRL1_GC_RBB2_POS                               (12U)
#define RF_GAIN_CTRL1_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL1_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL1_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB2_POS)
#define RF_GAIN_CTRL1_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL1_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL1_GC_RBB2_POS))
#define RF_GAIN_CTRL2_GC_RBB1                                   RF_GAIN_CTRL2_GC_RBB1
#define RF_GAIN_CTRL2_GC_RBB1_POS                               (16U)
#define RF_GAIN_CTRL2_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL2_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL2_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB1_POS)
#define RF_GAIN_CTRL2_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL2_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB1_POS))
#define RF_GAIN_CTRL2_GC_RBB2                                   RF_GAIN_CTRL2_GC_RBB2
#define RF_GAIN_CTRL2_GC_RBB2_POS                               (20U)
#define RF_GAIN_CTRL2_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL2_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL2_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB2_POS)
#define RF_GAIN_CTRL2_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL2_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL2_GC_RBB2_POS))
#define RF_GAIN_CTRL3_GC_RBB1                                   RF_GAIN_CTRL3_GC_RBB1
#define RF_GAIN_CTRL3_GC_RBB1_POS                               (24U)
#define RF_GAIN_CTRL3_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL3_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL3_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB1_POS)
#define RF_GAIN_CTRL3_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL3_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB1_POS))
#define RF_GAIN_CTRL3_GC_RBB2                                   RF_GAIN_CTRL3_GC_RBB2
#define RF_GAIN_CTRL3_GC_RBB2_POS                               (28U)
#define RF_GAIN_CTRL3_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL3_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL3_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB2_POS)
#define RF_GAIN_CTRL3_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL3_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL3_GC_RBB2_POS))

/* 0x108 : rbb_gain_index2 */
#define RF_RBB_GAIN_INDEX2_OFFSET                               (0x108)
#define RF_GAIN_CTRL4_GC_RBB1                                   RF_GAIN_CTRL4_GC_RBB1
#define RF_GAIN_CTRL4_GC_RBB1_POS                               (0U)
#define RF_GAIN_CTRL4_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL4_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL4_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB1_POS)
#define RF_GAIN_CTRL4_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL4_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB1_POS))
#define RF_GAIN_CTRL4_GC_RBB2                                   RF_GAIN_CTRL4_GC_RBB2
#define RF_GAIN_CTRL4_GC_RBB2_POS                               (4U)
#define RF_GAIN_CTRL4_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL4_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL4_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB2_POS)
#define RF_GAIN_CTRL4_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL4_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL4_GC_RBB2_POS))
#define RF_GAIN_CTRL5_GC_RBB1                                   RF_GAIN_CTRL5_GC_RBB1
#define RF_GAIN_CTRL5_GC_RBB1_POS                               (8U)
#define RF_GAIN_CTRL5_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL5_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL5_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB1_POS)
#define RF_GAIN_CTRL5_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL5_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB1_POS))
#define RF_GAIN_CTRL5_GC_RBB2                                   RF_GAIN_CTRL5_GC_RBB2
#define RF_GAIN_CTRL5_GC_RBB2_POS                               (12U)
#define RF_GAIN_CTRL5_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL5_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL5_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB2_POS)
#define RF_GAIN_CTRL5_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL5_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL5_GC_RBB2_POS))
#define RF_GAIN_CTRL6_GC_RBB1                                   RF_GAIN_CTRL6_GC_RBB1
#define RF_GAIN_CTRL6_GC_RBB1_POS                               (16U)
#define RF_GAIN_CTRL6_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL6_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL6_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB1_POS)
#define RF_GAIN_CTRL6_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL6_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB1_POS))
#define RF_GAIN_CTRL6_GC_RBB2                                   RF_GAIN_CTRL6_GC_RBB2
#define RF_GAIN_CTRL6_GC_RBB2_POS                               (20U)
#define RF_GAIN_CTRL6_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL6_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL6_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB2_POS)
#define RF_GAIN_CTRL6_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL6_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL6_GC_RBB2_POS))
#define RF_GAIN_CTRL7_GC_RBB1                                   RF_GAIN_CTRL7_GC_RBB1
#define RF_GAIN_CTRL7_GC_RBB1_POS                               (24U)
#define RF_GAIN_CTRL7_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL7_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL7_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB1_POS)
#define RF_GAIN_CTRL7_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL7_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB1_POS))
#define RF_GAIN_CTRL7_GC_RBB2                                   RF_GAIN_CTRL7_GC_RBB2
#define RF_GAIN_CTRL7_GC_RBB2_POS                               (28U)
#define RF_GAIN_CTRL7_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL7_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL7_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB2_POS)
#define RF_GAIN_CTRL7_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL7_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL7_GC_RBB2_POS))

/* 0x10C : rbb_gain_index3 */
#define RF_RBB_GAIN_INDEX3_OFFSET                               (0x10C)
#define RF_GAIN_CTRL8_GC_RBB1                                   RF_GAIN_CTRL8_GC_RBB1
#define RF_GAIN_CTRL8_GC_RBB1_POS                               (0U)
#define RF_GAIN_CTRL8_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL8_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL8_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB1_POS)
#define RF_GAIN_CTRL8_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL8_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB1_POS))
#define RF_GAIN_CTRL8_GC_RBB2                                   RF_GAIN_CTRL8_GC_RBB2
#define RF_GAIN_CTRL8_GC_RBB2_POS                               (4U)
#define RF_GAIN_CTRL8_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL8_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL8_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB2_POS)
#define RF_GAIN_CTRL8_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL8_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL8_GC_RBB2_POS))
#define RF_GAIN_CTRL9_GC_RBB1                                   RF_GAIN_CTRL9_GC_RBB1
#define RF_GAIN_CTRL9_GC_RBB1_POS                               (8U)
#define RF_GAIN_CTRL9_GC_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL9_GC_RBB1_MSK                               (((1U<<RF_GAIN_CTRL9_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB1_POS)
#define RF_GAIN_CTRL9_GC_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL9_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB1_POS))
#define RF_GAIN_CTRL9_GC_RBB2                                   RF_GAIN_CTRL9_GC_RBB2
#define RF_GAIN_CTRL9_GC_RBB2_POS                               (12U)
#define RF_GAIN_CTRL9_GC_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL9_GC_RBB2_MSK                               (((1U<<RF_GAIN_CTRL9_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB2_POS)
#define RF_GAIN_CTRL9_GC_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL9_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL9_GC_RBB2_POS))
#define RF_GAIN_CTRL10_GC_RBB1                                  RF_GAIN_CTRL10_GC_RBB1
#define RF_GAIN_CTRL10_GC_RBB1_POS                              (16U)
#define RF_GAIN_CTRL10_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL10_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL10_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB1_POS)
#define RF_GAIN_CTRL10_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL10_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB1_POS))
#define RF_GAIN_CTRL10_GC_RBB2                                  RF_GAIN_CTRL10_GC_RBB2
#define RF_GAIN_CTRL10_GC_RBB2_POS                              (20U)
#define RF_GAIN_CTRL10_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL10_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL10_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB2_POS)
#define RF_GAIN_CTRL10_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL10_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL10_GC_RBB2_POS))
#define RF_GAIN_CTRL11_GC_RBB1                                  RF_GAIN_CTRL11_GC_RBB1
#define RF_GAIN_CTRL11_GC_RBB1_POS                              (24U)
#define RF_GAIN_CTRL11_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL11_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL11_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB1_POS)
#define RF_GAIN_CTRL11_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL11_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB1_POS))
#define RF_GAIN_CTRL11_GC_RBB2                                  RF_GAIN_CTRL11_GC_RBB2
#define RF_GAIN_CTRL11_GC_RBB2_POS                              (28U)
#define RF_GAIN_CTRL11_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL11_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL11_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB2_POS)
#define RF_GAIN_CTRL11_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL11_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL11_GC_RBB2_POS))

/* 0x110 : rbb_gain_index4 */
#define RF_RBB_GAIN_INDEX4_OFFSET                               (0x110)
#define RF_GAIN_CTRL12_GC_RBB1                                  RF_GAIN_CTRL12_GC_RBB1
#define RF_GAIN_CTRL12_GC_RBB1_POS                              (0U)
#define RF_GAIN_CTRL12_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL12_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL12_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB1_POS)
#define RF_GAIN_CTRL12_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL12_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB1_POS))
#define RF_GAIN_CTRL12_GC_RBB2                                  RF_GAIN_CTRL12_GC_RBB2
#define RF_GAIN_CTRL12_GC_RBB2_POS                              (4U)
#define RF_GAIN_CTRL12_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL12_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL12_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB2_POS)
#define RF_GAIN_CTRL12_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL12_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL12_GC_RBB2_POS))
#define RF_GAIN_CTRL13_GC_RBB1                                  RF_GAIN_CTRL13_GC_RBB1
#define RF_GAIN_CTRL13_GC_RBB1_POS                              (8U)
#define RF_GAIN_CTRL13_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL13_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL13_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB1_POS)
#define RF_GAIN_CTRL13_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL13_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB1_POS))
#define RF_GAIN_CTRL13_GC_RBB2                                  RF_GAIN_CTRL13_GC_RBB2
#define RF_GAIN_CTRL13_GC_RBB2_POS                              (12U)
#define RF_GAIN_CTRL13_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL13_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL13_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB2_POS)
#define RF_GAIN_CTRL13_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL13_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL13_GC_RBB2_POS))
#define RF_GAIN_CTRL14_GC_RBB1                                  RF_GAIN_CTRL14_GC_RBB1
#define RF_GAIN_CTRL14_GC_RBB1_POS                              (16U)
#define RF_GAIN_CTRL14_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL14_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL14_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB1_POS)
#define RF_GAIN_CTRL14_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL14_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB1_POS))
#define RF_GAIN_CTRL14_GC_RBB2                                  RF_GAIN_CTRL14_GC_RBB2
#define RF_GAIN_CTRL14_GC_RBB2_POS                              (20U)
#define RF_GAIN_CTRL14_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL14_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL14_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB2_POS)
#define RF_GAIN_CTRL14_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL14_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL14_GC_RBB2_POS))
#define RF_GAIN_CTRL15_GC_RBB1                                  RF_GAIN_CTRL15_GC_RBB1
#define RF_GAIN_CTRL15_GC_RBB1_POS                              (24U)
#define RF_GAIN_CTRL15_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL15_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL15_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB1_POS)
#define RF_GAIN_CTRL15_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL15_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB1_POS))
#define RF_GAIN_CTRL15_GC_RBB2                                  RF_GAIN_CTRL15_GC_RBB2
#define RF_GAIN_CTRL15_GC_RBB2_POS                              (28U)
#define RF_GAIN_CTRL15_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL15_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL15_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB2_POS)
#define RF_GAIN_CTRL15_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL15_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL15_GC_RBB2_POS))

/* 0x114 : rbb_gain_index5 */
#define RF_RBB_GAIN_INDEX5_OFFSET                               (0x114)
#define RF_GAIN_CTRL16_GC_RBB1                                  RF_GAIN_CTRL16_GC_RBB1
#define RF_GAIN_CTRL16_GC_RBB1_POS                              (0U)
#define RF_GAIN_CTRL16_GC_RBB1_LEN                              (2U)
#define RF_GAIN_CTRL16_GC_RBB1_MSK                              (((1U<<RF_GAIN_CTRL16_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB1_POS)
#define RF_GAIN_CTRL16_GC_RBB1_UMSK                             (~(((1U<<RF_GAIN_CTRL16_GC_RBB1_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB1_POS))
#define RF_GAIN_CTRL16_GC_RBB2                                  RF_GAIN_CTRL16_GC_RBB2
#define RF_GAIN_CTRL16_GC_RBB2_POS                              (4U)
#define RF_GAIN_CTRL16_GC_RBB2_LEN                              (3U)
#define RF_GAIN_CTRL16_GC_RBB2_MSK                              (((1U<<RF_GAIN_CTRL16_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB2_POS)
#define RF_GAIN_CTRL16_GC_RBB2_UMSK                             (~(((1U<<RF_GAIN_CTRL16_GC_RBB2_LEN)-1)<<RF_GAIN_CTRL16_GC_RBB2_POS))

/* 0x118 : tbb_gain_index1 */
#define RF_TBB_GAIN_INDEX1_OFFSET                               (0x118)
#define RF_GAIN_CTRL0_GC_TBB                                    RF_GAIN_CTRL0_GC_TBB
#define RF_GAIN_CTRL0_GC_TBB_POS                                (0U)
#define RF_GAIN_CTRL0_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL0_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL0_GC_TBB_LEN)-1)<<RF_GAIN_CTRL0_GC_TBB_POS)
#define RF_GAIN_CTRL0_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL0_GC_TBB_LEN)-1)<<RF_GAIN_CTRL0_GC_TBB_POS))
#define RF_GAIN_CTRL0_GC_TMX                                    RF_GAIN_CTRL0_GC_TMX
#define RF_GAIN_CTRL0_GC_TMX_POS                                (8U)
#define RF_GAIN_CTRL0_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL0_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL0_GC_TMX_LEN)-1)<<RF_GAIN_CTRL0_GC_TMX_POS)
#define RF_GAIN_CTRL0_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL0_GC_TMX_LEN)-1)<<RF_GAIN_CTRL0_GC_TMX_POS))
#define RF_GAIN_CTRL0_DAC_BIAS_SEL                              RF_GAIN_CTRL0_DAC_BIAS_SEL
#define RF_GAIN_CTRL0_DAC_BIAS_SEL_POS                          (12U)
#define RF_GAIN_CTRL0_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL0_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL0_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL0_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL0_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL0_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL0_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL0_GC_TBB_BOOST                              RF_GAIN_CTRL0_GC_TBB_BOOST
#define RF_GAIN_CTRL0_GC_TBB_BOOST_POS                          (14U)
#define RF_GAIN_CTRL0_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL0_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL0_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL0_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL0_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL0_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL0_GC_TBB_BOOST_POS))
#define RF_GAIN_CTRL1_GC_TBB                                    RF_GAIN_CTRL1_GC_TBB
#define RF_GAIN_CTRL1_GC_TBB_POS                                (16U)
#define RF_GAIN_CTRL1_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL1_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL1_GC_TBB_LEN)-1)<<RF_GAIN_CTRL1_GC_TBB_POS)
#define RF_GAIN_CTRL1_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL1_GC_TBB_LEN)-1)<<RF_GAIN_CTRL1_GC_TBB_POS))
#define RF_GAIN_CTRL1_GC_TMX                                    RF_GAIN_CTRL1_GC_TMX
#define RF_GAIN_CTRL1_GC_TMX_POS                                (24U)
#define RF_GAIN_CTRL1_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL1_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL1_GC_TMX_LEN)-1)<<RF_GAIN_CTRL1_GC_TMX_POS)
#define RF_GAIN_CTRL1_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL1_GC_TMX_LEN)-1)<<RF_GAIN_CTRL1_GC_TMX_POS))
#define RF_GAIN_CTRL1_DAC_BIAS_SEL                              RF_GAIN_CTRL1_DAC_BIAS_SEL
#define RF_GAIN_CTRL1_DAC_BIAS_SEL_POS                          (28U)
#define RF_GAIN_CTRL1_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL1_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL1_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL1_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL1_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL1_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL1_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL1_GC_TBB_BOOST                              RF_GAIN_CTRL1_GC_TBB_BOOST
#define RF_GAIN_CTRL1_GC_TBB_BOOST_POS                          (30U)
#define RF_GAIN_CTRL1_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL1_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL1_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL1_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL1_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL1_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL1_GC_TBB_BOOST_POS))

/* 0x11C : tbb_gain_index2 */
#define RF_TBB_GAIN_INDEX2_OFFSET                               (0x11C)
#define RF_GAIN_CTRL2_GC_TBB                                    RF_GAIN_CTRL2_GC_TBB
#define RF_GAIN_CTRL2_GC_TBB_POS                                (0U)
#define RF_GAIN_CTRL2_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL2_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL2_GC_TBB_LEN)-1)<<RF_GAIN_CTRL2_GC_TBB_POS)
#define RF_GAIN_CTRL2_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL2_GC_TBB_LEN)-1)<<RF_GAIN_CTRL2_GC_TBB_POS))
#define RF_GAIN_CTRL2_GC_TMX                                    RF_GAIN_CTRL2_GC_TMX
#define RF_GAIN_CTRL2_GC_TMX_POS                                (8U)
#define RF_GAIN_CTRL2_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL2_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL2_GC_TMX_LEN)-1)<<RF_GAIN_CTRL2_GC_TMX_POS)
#define RF_GAIN_CTRL2_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL2_GC_TMX_LEN)-1)<<RF_GAIN_CTRL2_GC_TMX_POS))
#define RF_GAIN_CTRL2_DAC_BIAS_SEL                              RF_GAIN_CTRL2_DAC_BIAS_SEL
#define RF_GAIN_CTRL2_DAC_BIAS_SEL_POS                          (12U)
#define RF_GAIN_CTRL2_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL2_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL2_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL2_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL2_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL2_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL2_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL2_GC_TBB_BOOST                              RF_GAIN_CTRL2_GC_TBB_BOOST
#define RF_GAIN_CTRL2_GC_TBB_BOOST_POS                          (14U)
#define RF_GAIN_CTRL2_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL2_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL2_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL2_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL2_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL2_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL2_GC_TBB_BOOST_POS))
#define RF_GAIN_CTRL3_GC_TBB                                    RF_GAIN_CTRL3_GC_TBB
#define RF_GAIN_CTRL3_GC_TBB_POS                                (16U)
#define RF_GAIN_CTRL3_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL3_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL3_GC_TBB_LEN)-1)<<RF_GAIN_CTRL3_GC_TBB_POS)
#define RF_GAIN_CTRL3_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL3_GC_TBB_LEN)-1)<<RF_GAIN_CTRL3_GC_TBB_POS))
#define RF_GAIN_CTRL3_GC_TMX                                    RF_GAIN_CTRL3_GC_TMX
#define RF_GAIN_CTRL3_GC_TMX_POS                                (24U)
#define RF_GAIN_CTRL3_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL3_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL3_GC_TMX_LEN)-1)<<RF_GAIN_CTRL3_GC_TMX_POS)
#define RF_GAIN_CTRL3_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL3_GC_TMX_LEN)-1)<<RF_GAIN_CTRL3_GC_TMX_POS))
#define RF_GAIN_CTRL3_DAC_BIAS_SEL                              RF_GAIN_CTRL3_DAC_BIAS_SEL
#define RF_GAIN_CTRL3_DAC_BIAS_SEL_POS                          (28U)
#define RF_GAIN_CTRL3_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL3_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL3_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL3_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL3_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL3_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL3_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL3_GC_TBB_BOOST                              RF_GAIN_CTRL3_GC_TBB_BOOST
#define RF_GAIN_CTRL3_GC_TBB_BOOST_POS                          (30U)
#define RF_GAIN_CTRL3_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL3_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL3_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL3_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL3_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL3_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL3_GC_TBB_BOOST_POS))

/* 0x120 : tbb_gain_index3 */
#define RF_TBB_GAIN_INDEX3_OFFSET                               (0x120)
#define RF_GAIN_CTRL4_GC_TBB                                    RF_GAIN_CTRL4_GC_TBB
#define RF_GAIN_CTRL4_GC_TBB_POS                                (0U)
#define RF_GAIN_CTRL4_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL4_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL4_GC_TBB_LEN)-1)<<RF_GAIN_CTRL4_GC_TBB_POS)
#define RF_GAIN_CTRL4_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL4_GC_TBB_LEN)-1)<<RF_GAIN_CTRL4_GC_TBB_POS))
#define RF_GAIN_CTRL4_GC_TMX                                    RF_GAIN_CTRL4_GC_TMX
#define RF_GAIN_CTRL4_GC_TMX_POS                                (8U)
#define RF_GAIN_CTRL4_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL4_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL4_GC_TMX_LEN)-1)<<RF_GAIN_CTRL4_GC_TMX_POS)
#define RF_GAIN_CTRL4_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL4_GC_TMX_LEN)-1)<<RF_GAIN_CTRL4_GC_TMX_POS))
#define RF_GAIN_CTRL4_DAC_BIAS_SEL                              RF_GAIN_CTRL4_DAC_BIAS_SEL
#define RF_GAIN_CTRL4_DAC_BIAS_SEL_POS                          (12U)
#define RF_GAIN_CTRL4_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL4_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL4_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL4_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL4_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL4_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL4_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL4_GC_TBB_BOOST                              RF_GAIN_CTRL4_GC_TBB_BOOST
#define RF_GAIN_CTRL4_GC_TBB_BOOST_POS                          (14U)
#define RF_GAIN_CTRL4_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL4_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL4_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL4_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL4_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL4_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL4_GC_TBB_BOOST_POS))
#define RF_GAIN_CTRL5_GC_TBB                                    RF_GAIN_CTRL5_GC_TBB
#define RF_GAIN_CTRL5_GC_TBB_POS                                (16U)
#define RF_GAIN_CTRL5_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL5_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL5_GC_TBB_LEN)-1)<<RF_GAIN_CTRL5_GC_TBB_POS)
#define RF_GAIN_CTRL5_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL5_GC_TBB_LEN)-1)<<RF_GAIN_CTRL5_GC_TBB_POS))
#define RF_GAIN_CTRL5_GC_TMX                                    RF_GAIN_CTRL5_GC_TMX
#define RF_GAIN_CTRL5_GC_TMX_POS                                (24U)
#define RF_GAIN_CTRL5_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL5_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL5_GC_TMX_LEN)-1)<<RF_GAIN_CTRL5_GC_TMX_POS)
#define RF_GAIN_CTRL5_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL5_GC_TMX_LEN)-1)<<RF_GAIN_CTRL5_GC_TMX_POS))
#define RF_GAIN_CTRL5_DAC_BIAS_SEL                              RF_GAIN_CTRL5_DAC_BIAS_SEL
#define RF_GAIN_CTRL5_DAC_BIAS_SEL_POS                          (28U)
#define RF_GAIN_CTRL5_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL5_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL5_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL5_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL5_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL5_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL5_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL5_GC_TBB_BOOST                              RF_GAIN_CTRL5_GC_TBB_BOOST
#define RF_GAIN_CTRL5_GC_TBB_BOOST_POS                          (30U)
#define RF_GAIN_CTRL5_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL5_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL5_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL5_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL5_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL5_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL5_GC_TBB_BOOST_POS))

/* 0x124 : tbb_gain_index4 */
#define RF_TBB_GAIN_INDEX4_OFFSET                               (0x124)
#define RF_GAIN_CTRL6_GC_TBB                                    RF_GAIN_CTRL6_GC_TBB
#define RF_GAIN_CTRL6_GC_TBB_POS                                (0U)
#define RF_GAIN_CTRL6_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL6_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL6_GC_TBB_LEN)-1)<<RF_GAIN_CTRL6_GC_TBB_POS)
#define RF_GAIN_CTRL6_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL6_GC_TBB_LEN)-1)<<RF_GAIN_CTRL6_GC_TBB_POS))
#define RF_GAIN_CTRL6_GC_TMX                                    RF_GAIN_CTRL6_GC_TMX
#define RF_GAIN_CTRL6_GC_TMX_POS                                (8U)
#define RF_GAIN_CTRL6_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL6_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL6_GC_TMX_LEN)-1)<<RF_GAIN_CTRL6_GC_TMX_POS)
#define RF_GAIN_CTRL6_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL6_GC_TMX_LEN)-1)<<RF_GAIN_CTRL6_GC_TMX_POS))
#define RF_GAIN_CTRL6_DAC_BIAS_SEL                              RF_GAIN_CTRL6_DAC_BIAS_SEL
#define RF_GAIN_CTRL6_DAC_BIAS_SEL_POS                          (12U)
#define RF_GAIN_CTRL6_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL6_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL6_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL6_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL6_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL6_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL6_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL6_GC_TBB_BOOST                              RF_GAIN_CTRL6_GC_TBB_BOOST
#define RF_GAIN_CTRL6_GC_TBB_BOOST_POS                          (14U)
#define RF_GAIN_CTRL6_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL6_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL6_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL6_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL6_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL6_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL6_GC_TBB_BOOST_POS))
#define RF_GAIN_CTRL7_GC_TBB                                    RF_GAIN_CTRL7_GC_TBB
#define RF_GAIN_CTRL7_GC_TBB_POS                                (16U)
#define RF_GAIN_CTRL7_GC_TBB_LEN                                (5U)
#define RF_GAIN_CTRL7_GC_TBB_MSK                                (((1U<<RF_GAIN_CTRL7_GC_TBB_LEN)-1)<<RF_GAIN_CTRL7_GC_TBB_POS)
#define RF_GAIN_CTRL7_GC_TBB_UMSK                               (~(((1U<<RF_GAIN_CTRL7_GC_TBB_LEN)-1)<<RF_GAIN_CTRL7_GC_TBB_POS))
#define RF_GAIN_CTRL7_GC_TMX                                    RF_GAIN_CTRL7_GC_TMX
#define RF_GAIN_CTRL7_GC_TMX_POS                                (24U)
#define RF_GAIN_CTRL7_GC_TMX_LEN                                (3U)
#define RF_GAIN_CTRL7_GC_TMX_MSK                                (((1U<<RF_GAIN_CTRL7_GC_TMX_LEN)-1)<<RF_GAIN_CTRL7_GC_TMX_POS)
#define RF_GAIN_CTRL7_GC_TMX_UMSK                               (~(((1U<<RF_GAIN_CTRL7_GC_TMX_LEN)-1)<<RF_GAIN_CTRL7_GC_TMX_POS))
#define RF_GAIN_CTRL7_DAC_BIAS_SEL                              RF_GAIN_CTRL7_DAC_BIAS_SEL
#define RF_GAIN_CTRL7_DAC_BIAS_SEL_POS                          (28U)
#define RF_GAIN_CTRL7_DAC_BIAS_SEL_LEN                          (2U)
#define RF_GAIN_CTRL7_DAC_BIAS_SEL_MSK                          (((1U<<RF_GAIN_CTRL7_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL7_DAC_BIAS_SEL_POS)
#define RF_GAIN_CTRL7_DAC_BIAS_SEL_UMSK                         (~(((1U<<RF_GAIN_CTRL7_DAC_BIAS_SEL_LEN)-1)<<RF_GAIN_CTRL7_DAC_BIAS_SEL_POS))
#define RF_GAIN_CTRL7_GC_TBB_BOOST                              RF_GAIN_CTRL7_GC_TBB_BOOST
#define RF_GAIN_CTRL7_GC_TBB_BOOST_POS                          (30U)
#define RF_GAIN_CTRL7_GC_TBB_BOOST_LEN                          (2U)
#define RF_GAIN_CTRL7_GC_TBB_BOOST_MSK                          (((1U<<RF_GAIN_CTRL7_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL7_GC_TBB_BOOST_POS)
#define RF_GAIN_CTRL7_GC_TBB_BOOST_UMSK                         (~(((1U<<RF_GAIN_CTRL7_GC_TBB_BOOST_LEN)-1)<<RF_GAIN_CTRL7_GC_TBB_BOOST_POS))

/* 0x128 : pa_reg_ctrl_hw1 */
#define RF_PA_REG_CTRL_HW1_OFFSET                               (0x128)
#define RF_PA_IET_11N                                           RF_PA_IET_11N
#define RF_PA_IET_11N_POS                                       (12U)
#define RF_PA_IET_11N_LEN                                       (4U)
#define RF_PA_IET_11N_MSK                                       (((1U<<RF_PA_IET_11N_LEN)-1)<<RF_PA_IET_11N_POS)
#define RF_PA_IET_11N_UMSK                                      (~(((1U<<RF_PA_IET_11N_LEN)-1)<<RF_PA_IET_11N_POS))
#define RF_PA_VBCORE_11N                                        RF_PA_VBCORE_11N
#define RF_PA_VBCORE_11N_POS                                    (16U)
#define RF_PA_VBCORE_11N_LEN                                    (4U)
#define RF_PA_VBCORE_11N_MSK                                    (((1U<<RF_PA_VBCORE_11N_LEN)-1)<<RF_PA_VBCORE_11N_POS)
#define RF_PA_VBCORE_11N_UMSK                                   (~(((1U<<RF_PA_VBCORE_11N_LEN)-1)<<RF_PA_VBCORE_11N_POS))
#define RF_PA_VBCAS_11N                                         RF_PA_VBCAS_11N
#define RF_PA_VBCAS_11N_POS                                     (20U)
#define RF_PA_VBCAS_11N_LEN                                     (3U)
#define RF_PA_VBCAS_11N_MSK                                     (((1U<<RF_PA_VBCAS_11N_LEN)-1)<<RF_PA_VBCAS_11N_POS)
#define RF_PA_VBCAS_11N_UMSK                                    (~(((1U<<RF_PA_VBCAS_11N_LEN)-1)<<RF_PA_VBCAS_11N_POS))

/* 0x12C : pa_reg_ctrl_hw2 */
#define RF_PA_REG_CTRL_HW2_OFFSET                               (0x12C)
#define RF_PA_IET_11G                                           RF_PA_IET_11G
#define RF_PA_IET_11G_POS                                       (0U)
#define RF_PA_IET_11G_LEN                                       (4U)
#define RF_PA_IET_11G_MSK                                       (((1U<<RF_PA_IET_11G_LEN)-1)<<RF_PA_IET_11G_POS)
#define RF_PA_IET_11G_UMSK                                      (~(((1U<<RF_PA_IET_11G_LEN)-1)<<RF_PA_IET_11G_POS))
#define RF_PA_VBCORE_11G                                        RF_PA_VBCORE_11G
#define RF_PA_VBCORE_11G_POS                                    (4U)
#define RF_PA_VBCORE_11G_LEN                                    (4U)
#define RF_PA_VBCORE_11G_MSK                                    (((1U<<RF_PA_VBCORE_11G_LEN)-1)<<RF_PA_VBCORE_11G_POS)
#define RF_PA_VBCORE_11G_UMSK                                   (~(((1U<<RF_PA_VBCORE_11G_LEN)-1)<<RF_PA_VBCORE_11G_POS))
#define RF_PA_VBCAS_11G                                         RF_PA_VBCAS_11G
#define RF_PA_VBCAS_11G_POS                                     (8U)
#define RF_PA_VBCAS_11G_LEN                                     (3U)
#define RF_PA_VBCAS_11G_MSK                                     (((1U<<RF_PA_VBCAS_11G_LEN)-1)<<RF_PA_VBCAS_11G_POS)
#define RF_PA_VBCAS_11G_UMSK                                    (~(((1U<<RF_PA_VBCAS_11G_LEN)-1)<<RF_PA_VBCAS_11G_POS))
#define RF_PA_IET_11B                                           RF_PA_IET_11B
#define RF_PA_IET_11B_POS                                       (12U)
#define RF_PA_IET_11B_LEN                                       (4U)
#define RF_PA_IET_11B_MSK                                       (((1U<<RF_PA_IET_11B_LEN)-1)<<RF_PA_IET_11B_POS)
#define RF_PA_IET_11B_UMSK                                      (~(((1U<<RF_PA_IET_11B_LEN)-1)<<RF_PA_IET_11B_POS))
#define RF_PA_VBCORE_11B                                        RF_PA_VBCORE_11B
#define RF_PA_VBCORE_11B_POS                                    (16U)
#define RF_PA_VBCORE_11B_LEN                                    (4U)
#define RF_PA_VBCORE_11B_MSK                                    (((1U<<RF_PA_VBCORE_11B_LEN)-1)<<RF_PA_VBCORE_11B_POS)
#define RF_PA_VBCORE_11B_UMSK                                   (~(((1U<<RF_PA_VBCORE_11B_LEN)-1)<<RF_PA_VBCORE_11B_POS))
#define RF_PA_VBCAS_11B                                         RF_PA_VBCAS_11B
#define RF_PA_VBCAS_11B_POS                                     (20U)
#define RF_PA_VBCAS_11B_LEN                                     (3U)
#define RF_PA_VBCAS_11B_MSK                                     (((1U<<RF_PA_VBCAS_11B_LEN)-1)<<RF_PA_VBCAS_11B_POS)
#define RF_PA_VBCAS_11B_UMSK                                    (~(((1U<<RF_PA_VBCAS_11B_LEN)-1)<<RF_PA_VBCAS_11B_POS))

/* 0x130 : pa_reg_wifi_ctrl_hw */
#define RF_PA_REG_WIFI_CTRL_HW_OFFSET                           (0x130)
#define RF_PA_HALF_ON_WIFI                                      RF_PA_HALF_ON_WIFI
#define RF_PA_HALF_ON_WIFI_POS                                  (0U)
#define RF_PA_HALF_ON_WIFI_LEN                                  (1U)
#define RF_PA_HALF_ON_WIFI_MSK                                  (((1U<<RF_PA_HALF_ON_WIFI_LEN)-1)<<RF_PA_HALF_ON_WIFI_POS)
#define RF_PA_HALF_ON_WIFI_UMSK                                 (~(((1U<<RF_PA_HALF_ON_WIFI_LEN)-1)<<RF_PA_HALF_ON_WIFI_POS))
#define RF_PA_ETB_EN_WIFI                                       RF_PA_ETB_EN_WIFI
#define RF_PA_ETB_EN_WIFI_POS                                   (8U)
#define RF_PA_ETB_EN_WIFI_LEN                                   (1U)
#define RF_PA_ETB_EN_WIFI_MSK                                   (((1U<<RF_PA_ETB_EN_WIFI_LEN)-1)<<RF_PA_ETB_EN_WIFI_POS)
#define RF_PA_ETB_EN_WIFI_UMSK                                  (~(((1U<<RF_PA_ETB_EN_WIFI_LEN)-1)<<RF_PA_ETB_EN_WIFI_POS))
#define RF_PA_IB_FIX_WIFI                                       RF_PA_IB_FIX_WIFI
#define RF_PA_IB_FIX_WIFI_POS                                   (16U)
#define RF_PA_IB_FIX_WIFI_LEN                                   (1U)
#define RF_PA_IB_FIX_WIFI_MSK                                   (((1U<<RF_PA_IB_FIX_WIFI_LEN)-1)<<RF_PA_IB_FIX_WIFI_POS)
#define RF_PA_IB_FIX_WIFI_UMSK                                  (~(((1U<<RF_PA_IB_FIX_WIFI_LEN)-1)<<RF_PA_IB_FIX_WIFI_POS))

/* 0x134 : adda_reg_ctrl_hw */
#define RF_ADDA_REG_CTRL_HW_OFFSET                              (0x134)
#define RF_ADDA_LDO_DVDD_SEL_RX                                 RF_ADDA_LDO_DVDD_SEL_RX
#define RF_ADDA_LDO_DVDD_SEL_RX_POS                             (0U)
#define RF_ADDA_LDO_DVDD_SEL_RX_LEN                             (3U)
#define RF_ADDA_LDO_DVDD_SEL_RX_MSK                             (((1U<<RF_ADDA_LDO_DVDD_SEL_RX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_RX_POS)
#define RF_ADDA_LDO_DVDD_SEL_RX_UMSK                            (~(((1U<<RF_ADDA_LDO_DVDD_SEL_RX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_RX_POS))
#define RF_ADDA_LDO_DVDD_SEL_TX                                 RF_ADDA_LDO_DVDD_SEL_TX
#define RF_ADDA_LDO_DVDD_SEL_TX_POS                             (4U)
#define RF_ADDA_LDO_DVDD_SEL_TX_LEN                             (3U)
#define RF_ADDA_LDO_DVDD_SEL_TX_MSK                             (((1U<<RF_ADDA_LDO_DVDD_SEL_TX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_TX_POS)
#define RF_ADDA_LDO_DVDD_SEL_TX_UMSK                            (~(((1U<<RF_ADDA_LDO_DVDD_SEL_TX_LEN)-1)<<RF_ADDA_LDO_DVDD_SEL_TX_POS))

/* 0x138 : lo_reg_ctrl_hw1 */
#define RF_LO_REG_CTRL_HW1_OFFSET                               (0x138)
#define RF_LO_FBDV_HALFSTEP_EN_RX                               RF_LO_FBDV_HALFSTEP_EN_RX
#define RF_LO_FBDV_HALFSTEP_EN_RX_POS                           (0U)
#define RF_LO_FBDV_HALFSTEP_EN_RX_LEN                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_RX_MSK                           (((1U<<RF_LO_FBDV_HALFSTEP_EN_RX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_RX_POS)
#define RF_LO_FBDV_HALFSTEP_EN_RX_UMSK                          (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_RX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_RX_POS))
#define RF_LO_FBDV_HALFSTEP_EN_TX                               RF_LO_FBDV_HALFSTEP_EN_TX
#define RF_LO_FBDV_HALFSTEP_EN_TX_POS                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_TX_LEN                           (1U)
#define RF_LO_FBDV_HALFSTEP_EN_TX_MSK                           (((1U<<RF_LO_FBDV_HALFSTEP_EN_TX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_TX_POS)
#define RF_LO_FBDV_HALFSTEP_EN_TX_UMSK                          (~(((1U<<RF_LO_FBDV_HALFSTEP_EN_TX_LEN)-1)<<RF_LO_FBDV_HALFSTEP_EN_TX_POS))
#define RF_LO_CP_SEL_RX                                         RF_LO_CP_SEL_RX
#define RF_LO_CP_SEL_RX_POS                                     (2U)
#define RF_LO_CP_SEL_RX_LEN                                     (1U)
#define RF_LO_CP_SEL_RX_MSK                                     (((1U<<RF_LO_CP_SEL_RX_LEN)-1)<<RF_LO_CP_SEL_RX_POS)
#define RF_LO_CP_SEL_RX_UMSK                                    (~(((1U<<RF_LO_CP_SEL_RX_LEN)-1)<<RF_LO_CP_SEL_RX_POS))
#define RF_LO_CP_SEL_TX                                         RF_LO_CP_SEL_TX
#define RF_LO_CP_SEL_TX_POS                                     (3U)
#define RF_LO_CP_SEL_TX_LEN                                     (1U)
#define RF_LO_CP_SEL_TX_MSK                                     (((1U<<RF_LO_CP_SEL_TX_LEN)-1)<<RF_LO_CP_SEL_TX_POS)
#define RF_LO_CP_SEL_TX_UMSK                                    (~(((1U<<RF_LO_CP_SEL_TX_LEN)-1)<<RF_LO_CP_SEL_TX_POS))
#define RF_LO_LF_CZ_RX                                          RF_LO_LF_CZ_RX
#define RF_LO_LF_CZ_RX_POS                                      (4U)
#define RF_LO_LF_CZ_RX_LEN                                      (2U)
#define RF_LO_LF_CZ_RX_MSK                                      (((1U<<RF_LO_LF_CZ_RX_LEN)-1)<<RF_LO_LF_CZ_RX_POS)
#define RF_LO_LF_CZ_RX_UMSK                                     (~(((1U<<RF_LO_LF_CZ_RX_LEN)-1)<<RF_LO_LF_CZ_RX_POS))
#define RF_LO_LF_CZ_TX                                          RF_LO_LF_CZ_TX
#define RF_LO_LF_CZ_TX_POS                                      (8U)
#define RF_LO_LF_CZ_TX_LEN                                      (2U)
#define RF_LO_LF_CZ_TX_MSK                                      (((1U<<RF_LO_LF_CZ_TX_LEN)-1)<<RF_LO_LF_CZ_TX_POS)
#define RF_LO_LF_CZ_TX_UMSK                                     (~(((1U<<RF_LO_LF_CZ_TX_LEN)-1)<<RF_LO_LF_CZ_TX_POS))
#define RF_LO_LF_RZ_RX                                          RF_LO_LF_RZ_RX
#define RF_LO_LF_RZ_RX_POS                                      (12U)
#define RF_LO_LF_RZ_RX_LEN                                      (2U)
#define RF_LO_LF_RZ_RX_MSK                                      (((1U<<RF_LO_LF_RZ_RX_LEN)-1)<<RF_LO_LF_RZ_RX_POS)
#define RF_LO_LF_RZ_RX_UMSK                                     (~(((1U<<RF_LO_LF_RZ_RX_LEN)-1)<<RF_LO_LF_RZ_RX_POS))
#define RF_LO_LF_RZ_TX                                          RF_LO_LF_RZ_TX
#define RF_LO_LF_RZ_TX_POS                                      (16U)
#define RF_LO_LF_RZ_TX_LEN                                      (2U)
#define RF_LO_LF_RZ_TX_MSK                                      (((1U<<RF_LO_LF_RZ_TX_LEN)-1)<<RF_LO_LF_RZ_TX_POS)
#define RF_LO_LF_RZ_TX_UMSK                                     (~(((1U<<RF_LO_LF_RZ_TX_LEN)-1)<<RF_LO_LF_RZ_TX_POS))
#define RF_LO_LF_R4_RX                                          RF_LO_LF_R4_RX
#define RF_LO_LF_R4_RX_POS                                      (20U)
#define RF_LO_LF_R4_RX_LEN                                      (2U)
#define RF_LO_LF_R4_RX_MSK                                      (((1U<<RF_LO_LF_R4_RX_LEN)-1)<<RF_LO_LF_R4_RX_POS)
#define RF_LO_LF_R4_RX_UMSK                                     (~(((1U<<RF_LO_LF_R4_RX_LEN)-1)<<RF_LO_LF_R4_RX_POS))
#define RF_LO_LF_R4_TX                                          RF_LO_LF_R4_TX
#define RF_LO_LF_R4_TX_POS                                      (24U)
#define RF_LO_LF_R4_TX_LEN                                      (2U)
#define RF_LO_LF_R4_TX_MSK                                      (((1U<<RF_LO_LF_R4_TX_LEN)-1)<<RF_LO_LF_R4_TX_POS)
#define RF_LO_LF_R4_TX_UMSK                                     (~(((1U<<RF_LO_LF_R4_TX_LEN)-1)<<RF_LO_LF_R4_TX_POS))

/* 0x13C : lo_cal_ctrl_hw1 */
#define RF_LO_CAL_CTRL_HW1_OFFSET                               (0x13C)
#define RF_LO_VCO_IDAC_CW_2404                                  RF_LO_VCO_IDAC_CW_2404
#define RF_LO_VCO_IDAC_CW_2404_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2404_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2404_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2404_LEN)-1)<<RF_LO_VCO_IDAC_CW_2404_POS)
#define RF_LO_VCO_IDAC_CW_2404_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2404_LEN)-1)<<RF_LO_VCO_IDAC_CW_2404_POS))
#define RF_LO_VCO_FREQ_CW_2404                                  RF_LO_VCO_FREQ_CW_2404
#define RF_LO_VCO_FREQ_CW_2404_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2404_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2404_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2404_LEN)-1)<<RF_LO_VCO_FREQ_CW_2404_POS)
#define RF_LO_VCO_FREQ_CW_2404_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2404_LEN)-1)<<RF_LO_VCO_FREQ_CW_2404_POS))
#define RF_LO_VCO_IDAC_CW_2408                                  RF_LO_VCO_IDAC_CW_2408
#define RF_LO_VCO_IDAC_CW_2408_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2408_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2408_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2408_LEN)-1)<<RF_LO_VCO_IDAC_CW_2408_POS)
#define RF_LO_VCO_IDAC_CW_2408_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2408_LEN)-1)<<RF_LO_VCO_IDAC_CW_2408_POS))
#define RF_LO_VCO_FREQ_CW_2408                                  RF_LO_VCO_FREQ_CW_2408
#define RF_LO_VCO_FREQ_CW_2408_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2408_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2408_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2408_LEN)-1)<<RF_LO_VCO_FREQ_CW_2408_POS)
#define RF_LO_VCO_FREQ_CW_2408_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2408_LEN)-1)<<RF_LO_VCO_FREQ_CW_2408_POS))

/* 0x140 : lo_cal_ctrl_hw2 */
#define RF_LO_CAL_CTRL_HW2_OFFSET                               (0x140)
#define RF_LO_VCO_IDAC_CW_2412                                  RF_LO_VCO_IDAC_CW_2412
#define RF_LO_VCO_IDAC_CW_2412_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2412_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2412_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2412_LEN)-1)<<RF_LO_VCO_IDAC_CW_2412_POS)
#define RF_LO_VCO_IDAC_CW_2412_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2412_LEN)-1)<<RF_LO_VCO_IDAC_CW_2412_POS))
#define RF_LO_VCO_FREQ_CW_2412                                  RF_LO_VCO_FREQ_CW_2412
#define RF_LO_VCO_FREQ_CW_2412_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2412_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2412_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2412_LEN)-1)<<RF_LO_VCO_FREQ_CW_2412_POS)
#define RF_LO_VCO_FREQ_CW_2412_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2412_LEN)-1)<<RF_LO_VCO_FREQ_CW_2412_POS))
#define RF_LO_VCO_IDAC_CW_2416                                  RF_LO_VCO_IDAC_CW_2416
#define RF_LO_VCO_IDAC_CW_2416_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2416_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2416_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2416_LEN)-1)<<RF_LO_VCO_IDAC_CW_2416_POS)
#define RF_LO_VCO_IDAC_CW_2416_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2416_LEN)-1)<<RF_LO_VCO_IDAC_CW_2416_POS))
#define RF_LO_VCO_FREQ_CW_2416                                  RF_LO_VCO_FREQ_CW_2416
#define RF_LO_VCO_FREQ_CW_2416_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2416_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2416_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2416_LEN)-1)<<RF_LO_VCO_FREQ_CW_2416_POS)
#define RF_LO_VCO_FREQ_CW_2416_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2416_LEN)-1)<<RF_LO_VCO_FREQ_CW_2416_POS))

/* 0x144 : lo_cal_ctrl_hw3 */
#define RF_LO_CAL_CTRL_HW3_OFFSET                               (0x144)
#define RF_LO_VCO_IDAC_CW_2420                                  RF_LO_VCO_IDAC_CW_2420
#define RF_LO_VCO_IDAC_CW_2420_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2420_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2420_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2420_LEN)-1)<<RF_LO_VCO_IDAC_CW_2420_POS)
#define RF_LO_VCO_IDAC_CW_2420_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2420_LEN)-1)<<RF_LO_VCO_IDAC_CW_2420_POS))
#define RF_LO_VCO_FREQ_CW_2420                                  RF_LO_VCO_FREQ_CW_2420
#define RF_LO_VCO_FREQ_CW_2420_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2420_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2420_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2420_LEN)-1)<<RF_LO_VCO_FREQ_CW_2420_POS)
#define RF_LO_VCO_FREQ_CW_2420_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2420_LEN)-1)<<RF_LO_VCO_FREQ_CW_2420_POS))
#define RF_LO_VCO_IDAC_CW_2424                                  RF_LO_VCO_IDAC_CW_2424
#define RF_LO_VCO_IDAC_CW_2424_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2424_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2424_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2424_LEN)-1)<<RF_LO_VCO_IDAC_CW_2424_POS)
#define RF_LO_VCO_IDAC_CW_2424_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2424_LEN)-1)<<RF_LO_VCO_IDAC_CW_2424_POS))
#define RF_LO_VCO_FREQ_CW_2424                                  RF_LO_VCO_FREQ_CW_2424
#define RF_LO_VCO_FREQ_CW_2424_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2424_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2424_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2424_LEN)-1)<<RF_LO_VCO_FREQ_CW_2424_POS)
#define RF_LO_VCO_FREQ_CW_2424_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2424_LEN)-1)<<RF_LO_VCO_FREQ_CW_2424_POS))

/* 0x148 : lo_cal_ctrl_hw4 */
#define RF_LO_CAL_CTRL_HW4_OFFSET                               (0x148)
#define RF_LO_VCO_IDAC_CW_2428                                  RF_LO_VCO_IDAC_CW_2428
#define RF_LO_VCO_IDAC_CW_2428_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2428_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2428_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2428_LEN)-1)<<RF_LO_VCO_IDAC_CW_2428_POS)
#define RF_LO_VCO_IDAC_CW_2428_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2428_LEN)-1)<<RF_LO_VCO_IDAC_CW_2428_POS))
#define RF_LO_VCO_FREQ_CW_2428                                  RF_LO_VCO_FREQ_CW_2428
#define RF_LO_VCO_FREQ_CW_2428_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2428_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2428_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2428_LEN)-1)<<RF_LO_VCO_FREQ_CW_2428_POS)
#define RF_LO_VCO_FREQ_CW_2428_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2428_LEN)-1)<<RF_LO_VCO_FREQ_CW_2428_POS))
#define RF_LO_VCO_IDAC_CW_2432                                  RF_LO_VCO_IDAC_CW_2432
#define RF_LO_VCO_IDAC_CW_2432_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2432_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2432_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2432_LEN)-1)<<RF_LO_VCO_IDAC_CW_2432_POS)
#define RF_LO_VCO_IDAC_CW_2432_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2432_LEN)-1)<<RF_LO_VCO_IDAC_CW_2432_POS))
#define RF_LO_VCO_FREQ_CW_2432                                  RF_LO_VCO_FREQ_CW_2432
#define RF_LO_VCO_FREQ_CW_2432_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2432_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2432_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2432_LEN)-1)<<RF_LO_VCO_FREQ_CW_2432_POS)
#define RF_LO_VCO_FREQ_CW_2432_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2432_LEN)-1)<<RF_LO_VCO_FREQ_CW_2432_POS))

/* 0x14C : lo_cal_ctrl_hw5 */
#define RF_LO_CAL_CTRL_HW5_OFFSET                               (0x14C)
#define RF_LO_VCO_IDAC_CW_2436                                  RF_LO_VCO_IDAC_CW_2436
#define RF_LO_VCO_IDAC_CW_2436_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2436_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2436_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2436_LEN)-1)<<RF_LO_VCO_IDAC_CW_2436_POS)
#define RF_LO_VCO_IDAC_CW_2436_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2436_LEN)-1)<<RF_LO_VCO_IDAC_CW_2436_POS))
#define RF_LO_VCO_FREQ_CW_2436                                  RF_LO_VCO_FREQ_CW_2436
#define RF_LO_VCO_FREQ_CW_2436_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2436_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2436_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2436_LEN)-1)<<RF_LO_VCO_FREQ_CW_2436_POS)
#define RF_LO_VCO_FREQ_CW_2436_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2436_LEN)-1)<<RF_LO_VCO_FREQ_CW_2436_POS))
#define RF_LO_VCO_IDAC_CW_2440                                  RF_LO_VCO_IDAC_CW_2440
#define RF_LO_VCO_IDAC_CW_2440_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2440_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2440_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2440_LEN)-1)<<RF_LO_VCO_IDAC_CW_2440_POS)
#define RF_LO_VCO_IDAC_CW_2440_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2440_LEN)-1)<<RF_LO_VCO_IDAC_CW_2440_POS))
#define RF_LO_VCO_FREQ_CW_2440                                  RF_LO_VCO_FREQ_CW_2440
#define RF_LO_VCO_FREQ_CW_2440_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2440_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2440_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2440_LEN)-1)<<RF_LO_VCO_FREQ_CW_2440_POS)
#define RF_LO_VCO_FREQ_CW_2440_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2440_LEN)-1)<<RF_LO_VCO_FREQ_CW_2440_POS))

/* 0x150 : lo_cal_ctrl_hw6 */
#define RF_LO_CAL_CTRL_HW6_OFFSET                               (0x150)
#define RF_LO_VCO_IDAC_CW_2444                                  RF_LO_VCO_IDAC_CW_2444
#define RF_LO_VCO_IDAC_CW_2444_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2444_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2444_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2444_LEN)-1)<<RF_LO_VCO_IDAC_CW_2444_POS)
#define RF_LO_VCO_IDAC_CW_2444_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2444_LEN)-1)<<RF_LO_VCO_IDAC_CW_2444_POS))
#define RF_LO_VCO_FREQ_CW_2444                                  RF_LO_VCO_FREQ_CW_2444
#define RF_LO_VCO_FREQ_CW_2444_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2444_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2444_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2444_LEN)-1)<<RF_LO_VCO_FREQ_CW_2444_POS)
#define RF_LO_VCO_FREQ_CW_2444_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2444_LEN)-1)<<RF_LO_VCO_FREQ_CW_2444_POS))
#define RF_LO_VCO_IDAC_CW_2448                                  RF_LO_VCO_IDAC_CW_2448
#define RF_LO_VCO_IDAC_CW_2448_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2448_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2448_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2448_LEN)-1)<<RF_LO_VCO_IDAC_CW_2448_POS)
#define RF_LO_VCO_IDAC_CW_2448_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2448_LEN)-1)<<RF_LO_VCO_IDAC_CW_2448_POS))
#define RF_LO_VCO_FREQ_CW_2448                                  RF_LO_VCO_FREQ_CW_2448
#define RF_LO_VCO_FREQ_CW_2448_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2448_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2448_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2448_LEN)-1)<<RF_LO_VCO_FREQ_CW_2448_POS)
#define RF_LO_VCO_FREQ_CW_2448_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2448_LEN)-1)<<RF_LO_VCO_FREQ_CW_2448_POS))

/* 0x154 : lo_cal_ctrl_hw7 */
#define RF_LO_CAL_CTRL_HW7_OFFSET                               (0x154)
#define RF_LO_VCO_IDAC_CW_2452                                  RF_LO_VCO_IDAC_CW_2452
#define RF_LO_VCO_IDAC_CW_2452_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2452_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2452_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2452_LEN)-1)<<RF_LO_VCO_IDAC_CW_2452_POS)
#define RF_LO_VCO_IDAC_CW_2452_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2452_LEN)-1)<<RF_LO_VCO_IDAC_CW_2452_POS))
#define RF_LO_VCO_FREQ_CW_2452                                  RF_LO_VCO_FREQ_CW_2452
#define RF_LO_VCO_FREQ_CW_2452_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2452_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2452_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2452_LEN)-1)<<RF_LO_VCO_FREQ_CW_2452_POS)
#define RF_LO_VCO_FREQ_CW_2452_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2452_LEN)-1)<<RF_LO_VCO_FREQ_CW_2452_POS))
#define RF_LO_VCO_IDAC_CW_2456                                  RF_LO_VCO_IDAC_CW_2456
#define RF_LO_VCO_IDAC_CW_2456_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2456_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2456_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2456_LEN)-1)<<RF_LO_VCO_IDAC_CW_2456_POS)
#define RF_LO_VCO_IDAC_CW_2456_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2456_LEN)-1)<<RF_LO_VCO_IDAC_CW_2456_POS))
#define RF_LO_VCO_FREQ_CW_2456                                  RF_LO_VCO_FREQ_CW_2456
#define RF_LO_VCO_FREQ_CW_2456_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2456_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2456_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2456_LEN)-1)<<RF_LO_VCO_FREQ_CW_2456_POS)
#define RF_LO_VCO_FREQ_CW_2456_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2456_LEN)-1)<<RF_LO_VCO_FREQ_CW_2456_POS))

/* 0x158 : lo_cal_ctrl_hw8 */
#define RF_LO_CAL_CTRL_HW8_OFFSET                               (0x158)
#define RF_LO_VCO_IDAC_CW_2460                                  RF_LO_VCO_IDAC_CW_2460
#define RF_LO_VCO_IDAC_CW_2460_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2460_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2460_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2460_LEN)-1)<<RF_LO_VCO_IDAC_CW_2460_POS)
#define RF_LO_VCO_IDAC_CW_2460_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2460_LEN)-1)<<RF_LO_VCO_IDAC_CW_2460_POS))
#define RF_LO_VCO_FREQ_CW_2460                                  RF_LO_VCO_FREQ_CW_2460
#define RF_LO_VCO_FREQ_CW_2460_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2460_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2460_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2460_LEN)-1)<<RF_LO_VCO_FREQ_CW_2460_POS)
#define RF_LO_VCO_FREQ_CW_2460_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2460_LEN)-1)<<RF_LO_VCO_FREQ_CW_2460_POS))
#define RF_LO_VCO_IDAC_CW_2464                                  RF_LO_VCO_IDAC_CW_2464
#define RF_LO_VCO_IDAC_CW_2464_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2464_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2464_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2464_LEN)-1)<<RF_LO_VCO_IDAC_CW_2464_POS)
#define RF_LO_VCO_IDAC_CW_2464_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2464_LEN)-1)<<RF_LO_VCO_IDAC_CW_2464_POS))
#define RF_LO_VCO_FREQ_CW_2464                                  RF_LO_VCO_FREQ_CW_2464
#define RF_LO_VCO_FREQ_CW_2464_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2464_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2464_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2464_LEN)-1)<<RF_LO_VCO_FREQ_CW_2464_POS)
#define RF_LO_VCO_FREQ_CW_2464_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2464_LEN)-1)<<RF_LO_VCO_FREQ_CW_2464_POS))

/* 0x15C : lo_cal_ctrl_hw9 */
#define RF_LO_CAL_CTRL_HW9_OFFSET                               (0x15C)
#define RF_LO_VCO_IDAC_CW_2468                                  RF_LO_VCO_IDAC_CW_2468
#define RF_LO_VCO_IDAC_CW_2468_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2468_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2468_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2468_LEN)-1)<<RF_LO_VCO_IDAC_CW_2468_POS)
#define RF_LO_VCO_IDAC_CW_2468_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2468_LEN)-1)<<RF_LO_VCO_IDAC_CW_2468_POS))
#define RF_LO_VCO_FREQ_CW_2468                                  RF_LO_VCO_FREQ_CW_2468
#define RF_LO_VCO_FREQ_CW_2468_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2468_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2468_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2468_LEN)-1)<<RF_LO_VCO_FREQ_CW_2468_POS)
#define RF_LO_VCO_FREQ_CW_2468_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2468_LEN)-1)<<RF_LO_VCO_FREQ_CW_2468_POS))
#define RF_LO_VCO_IDAC_CW_2472                                  RF_LO_VCO_IDAC_CW_2472
#define RF_LO_VCO_IDAC_CW_2472_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2472_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2472_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2472_LEN)-1)<<RF_LO_VCO_IDAC_CW_2472_POS)
#define RF_LO_VCO_IDAC_CW_2472_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2472_LEN)-1)<<RF_LO_VCO_IDAC_CW_2472_POS))
#define RF_LO_VCO_FREQ_CW_2472                                  RF_LO_VCO_FREQ_CW_2472
#define RF_LO_VCO_FREQ_CW_2472_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2472_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2472_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2472_LEN)-1)<<RF_LO_VCO_FREQ_CW_2472_POS)
#define RF_LO_VCO_FREQ_CW_2472_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2472_LEN)-1)<<RF_LO_VCO_FREQ_CW_2472_POS))

/* 0x160 : lo_cal_ctrl_hw10 */
#define RF_LO_CAL_CTRL_HW10_OFFSET                              (0x160)
#define RF_LO_VCO_IDAC_CW_2476                                  RF_LO_VCO_IDAC_CW_2476
#define RF_LO_VCO_IDAC_CW_2476_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2476_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2476_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2476_LEN)-1)<<RF_LO_VCO_IDAC_CW_2476_POS)
#define RF_LO_VCO_IDAC_CW_2476_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2476_LEN)-1)<<RF_LO_VCO_IDAC_CW_2476_POS))
#define RF_LO_VCO_FREQ_CW_2476                                  RF_LO_VCO_FREQ_CW_2476
#define RF_LO_VCO_FREQ_CW_2476_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2476_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2476_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2476_LEN)-1)<<RF_LO_VCO_FREQ_CW_2476_POS)
#define RF_LO_VCO_FREQ_CW_2476_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2476_LEN)-1)<<RF_LO_VCO_FREQ_CW_2476_POS))
#define RF_LO_VCO_IDAC_CW_2480                                  RF_LO_VCO_IDAC_CW_2480
#define RF_LO_VCO_IDAC_CW_2480_POS                              (16U)
#define RF_LO_VCO_IDAC_CW_2480_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2480_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2480_LEN)-1)<<RF_LO_VCO_IDAC_CW_2480_POS)
#define RF_LO_VCO_IDAC_CW_2480_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2480_LEN)-1)<<RF_LO_VCO_IDAC_CW_2480_POS))
#define RF_LO_VCO_FREQ_CW_2480                                  RF_LO_VCO_FREQ_CW_2480
#define RF_LO_VCO_FREQ_CW_2480_POS                              (24U)
#define RF_LO_VCO_FREQ_CW_2480_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2480_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2480_LEN)-1)<<RF_LO_VCO_FREQ_CW_2480_POS)
#define RF_LO_VCO_FREQ_CW_2480_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2480_LEN)-1)<<RF_LO_VCO_FREQ_CW_2480_POS))

/* 0x164 : lo_cal_ctrl_hw11 */
#define RF_LO_CAL_CTRL_HW11_OFFSET                              (0x164)
#define RF_LO_VCO_IDAC_CW_2484                                  RF_LO_VCO_IDAC_CW_2484
#define RF_LO_VCO_IDAC_CW_2484_POS                              (0U)
#define RF_LO_VCO_IDAC_CW_2484_LEN                              (5U)
#define RF_LO_VCO_IDAC_CW_2484_MSK                              (((1U<<RF_LO_VCO_IDAC_CW_2484_LEN)-1)<<RF_LO_VCO_IDAC_CW_2484_POS)
#define RF_LO_VCO_IDAC_CW_2484_UMSK                             (~(((1U<<RF_LO_VCO_IDAC_CW_2484_LEN)-1)<<RF_LO_VCO_IDAC_CW_2484_POS))
#define RF_LO_VCO_FREQ_CW_2484                                  RF_LO_VCO_FREQ_CW_2484
#define RF_LO_VCO_FREQ_CW_2484_POS                              (8U)
#define RF_LO_VCO_FREQ_CW_2484_LEN                              (8U)
#define RF_LO_VCO_FREQ_CW_2484_MSK                              (((1U<<RF_LO_VCO_FREQ_CW_2484_LEN)-1)<<RF_LO_VCO_FREQ_CW_2484_POS)
#define RF_LO_VCO_FREQ_CW_2484_UMSK                             (~(((1U<<RF_LO_VCO_FREQ_CW_2484_LEN)-1)<<RF_LO_VCO_FREQ_CW_2484_POS))

/* 0x168 : rosdac_ctrl_hw1 */
#define RF_ROSDAC_CTRL_HW1_OFFSET                               (0x168)
#define RF_ROSDAC_I_GC0                                         RF_ROSDAC_I_GC0
#define RF_ROSDAC_I_GC0_POS                                     (0U)
#define RF_ROSDAC_I_GC0_LEN                                     (6U)
#define RF_ROSDAC_I_GC0_MSK                                     (((1U<<RF_ROSDAC_I_GC0_LEN)-1)<<RF_ROSDAC_I_GC0_POS)
#define RF_ROSDAC_I_GC0_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC0_LEN)-1)<<RF_ROSDAC_I_GC0_POS))
#define RF_ROSDAC_Q_GC0                                         RF_ROSDAC_Q_GC0
#define RF_ROSDAC_Q_GC0_POS                                     (8U)
#define RF_ROSDAC_Q_GC0_LEN                                     (6U)
#define RF_ROSDAC_Q_GC0_MSK                                     (((1U<<RF_ROSDAC_Q_GC0_LEN)-1)<<RF_ROSDAC_Q_GC0_POS)
#define RF_ROSDAC_Q_GC0_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC0_LEN)-1)<<RF_ROSDAC_Q_GC0_POS))
#define RF_ROSDAC_I_GC1                                         RF_ROSDAC_I_GC1
#define RF_ROSDAC_I_GC1_POS                                     (16U)
#define RF_ROSDAC_I_GC1_LEN                                     (6U)
#define RF_ROSDAC_I_GC1_MSK                                     (((1U<<RF_ROSDAC_I_GC1_LEN)-1)<<RF_ROSDAC_I_GC1_POS)
#define RF_ROSDAC_I_GC1_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC1_LEN)-1)<<RF_ROSDAC_I_GC1_POS))
#define RF_ROSDAC_Q_GC1                                         RF_ROSDAC_Q_GC1
#define RF_ROSDAC_Q_GC1_POS                                     (24U)
#define RF_ROSDAC_Q_GC1_LEN                                     (6U)
#define RF_ROSDAC_Q_GC1_MSK                                     (((1U<<RF_ROSDAC_Q_GC1_LEN)-1)<<RF_ROSDAC_Q_GC1_POS)
#define RF_ROSDAC_Q_GC1_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC1_LEN)-1)<<RF_ROSDAC_Q_GC1_POS))

/* 0x16C : rosdac_ctrl_hw2 */
#define RF_ROSDAC_CTRL_HW2_OFFSET                               (0x16C)
#define RF_ROSDAC_I_GC2                                         RF_ROSDAC_I_GC2
#define RF_ROSDAC_I_GC2_POS                                     (0U)
#define RF_ROSDAC_I_GC2_LEN                                     (6U)
#define RF_ROSDAC_I_GC2_MSK                                     (((1U<<RF_ROSDAC_I_GC2_LEN)-1)<<RF_ROSDAC_I_GC2_POS)
#define RF_ROSDAC_I_GC2_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC2_LEN)-1)<<RF_ROSDAC_I_GC2_POS))
#define RF_ROSDAC_Q_GC2                                         RF_ROSDAC_Q_GC2
#define RF_ROSDAC_Q_GC2_POS                                     (8U)
#define RF_ROSDAC_Q_GC2_LEN                                     (6U)
#define RF_ROSDAC_Q_GC2_MSK                                     (((1U<<RF_ROSDAC_Q_GC2_LEN)-1)<<RF_ROSDAC_Q_GC2_POS)
#define RF_ROSDAC_Q_GC2_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC2_LEN)-1)<<RF_ROSDAC_Q_GC2_POS))
#define RF_ROSDAC_I_GC3                                         RF_ROSDAC_I_GC3
#define RF_ROSDAC_I_GC3_POS                                     (16U)
#define RF_ROSDAC_I_GC3_LEN                                     (6U)
#define RF_ROSDAC_I_GC3_MSK                                     (((1U<<RF_ROSDAC_I_GC3_LEN)-1)<<RF_ROSDAC_I_GC3_POS)
#define RF_ROSDAC_I_GC3_UMSK                                    (~(((1U<<RF_ROSDAC_I_GC3_LEN)-1)<<RF_ROSDAC_I_GC3_POS))
#define RF_ROSDAC_Q_GC3                                         RF_ROSDAC_Q_GC3
#define RF_ROSDAC_Q_GC3_POS                                     (24U)
#define RF_ROSDAC_Q_GC3_LEN                                     (6U)
#define RF_ROSDAC_Q_GC3_MSK                                     (((1U<<RF_ROSDAC_Q_GC3_LEN)-1)<<RF_ROSDAC_Q_GC3_POS)
#define RF_ROSDAC_Q_GC3_UMSK                                    (~(((1U<<RF_ROSDAC_Q_GC3_LEN)-1)<<RF_ROSDAC_Q_GC3_POS))

/* 0x170 : rxiq_ctrl_hw1 */
#define RF_RXIQ_CTRL_HW1_OFFSET                                 (0x170)
#define RF_RX_IQ_PHASE_COMP_GC0                                 RF_RX_IQ_PHASE_COMP_GC0
#define RF_RX_IQ_PHASE_COMP_GC0_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC0_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC0_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC0_POS)
#define RF_RX_IQ_PHASE_COMP_GC0_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC0_POS))
#define RF_RX_IQ_GAIN_COMP_GC0                                  RF_RX_IQ_GAIN_COMP_GC0
#define RF_RX_IQ_GAIN_COMP_GC0_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC0_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC0_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC0_POS)
#define RF_RX_IQ_GAIN_COMP_GC0_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC0_POS))

/* 0x174 : rxiq_ctrl_hw2 */
#define RF_RXIQ_CTRL_HW2_OFFSET                                 (0x174)
#define RF_RX_IQ_PHASE_COMP_GC1                                 RF_RX_IQ_PHASE_COMP_GC1
#define RF_RX_IQ_PHASE_COMP_GC1_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC1_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC1_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC1_POS)
#define RF_RX_IQ_PHASE_COMP_GC1_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC1_POS))
#define RF_RX_IQ_GAIN_COMP_GC1                                  RF_RX_IQ_GAIN_COMP_GC1
#define RF_RX_IQ_GAIN_COMP_GC1_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC1_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC1_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC1_POS)
#define RF_RX_IQ_GAIN_COMP_GC1_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC1_POS))

/* 0x178 : rxiq_ctrl_hw3 */
#define RF_RXIQ_CTRL_HW3_OFFSET                                 (0x178)
#define RF_RX_IQ_PHASE_COMP_GC2                                 RF_RX_IQ_PHASE_COMP_GC2
#define RF_RX_IQ_PHASE_COMP_GC2_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC2_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC2_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC2_POS)
#define RF_RX_IQ_PHASE_COMP_GC2_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC2_POS))
#define RF_RX_IQ_GAIN_COMP_GC2                                  RF_RX_IQ_GAIN_COMP_GC2
#define RF_RX_IQ_GAIN_COMP_GC2_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC2_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC2_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC2_POS)
#define RF_RX_IQ_GAIN_COMP_GC2_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC2_POS))

/* 0x17C : rxiq_ctrl_hw4 */
#define RF_RXIQ_CTRL_HW4_OFFSET                                 (0x17C)
#define RF_RX_IQ_PHASE_COMP_GC3                                 RF_RX_IQ_PHASE_COMP_GC3
#define RF_RX_IQ_PHASE_COMP_GC3_POS                             (0U)
#define RF_RX_IQ_PHASE_COMP_GC3_LEN                             (10U)
#define RF_RX_IQ_PHASE_COMP_GC3_MSK                             (((1U<<RF_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC3_POS)
#define RF_RX_IQ_PHASE_COMP_GC3_UMSK                            (~(((1U<<RF_RX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_RX_IQ_PHASE_COMP_GC3_POS))
#define RF_RX_IQ_GAIN_COMP_GC3                                  RF_RX_IQ_GAIN_COMP_GC3
#define RF_RX_IQ_GAIN_COMP_GC3_POS                              (16U)
#define RF_RX_IQ_GAIN_COMP_GC3_LEN                              (11U)
#define RF_RX_IQ_GAIN_COMP_GC3_MSK                              (((1U<<RF_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC3_POS)
#define RF_RX_IQ_GAIN_COMP_GC3_UMSK                             (~(((1U<<RF_RX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_RX_IQ_GAIN_COMP_GC3_POS))

/* 0x180 : tosdac_ctrl_hw1 */
#define RF_TOSDAC_CTRL_HW1_OFFSET                               (0x180)
#define RF_TBB_TOSDAC_I_GC0                                     RF_TBB_TOSDAC_I_GC0
#define RF_TBB_TOSDAC_I_GC0_POS                                 (0U)
#define RF_TBB_TOSDAC_I_GC0_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC0_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC0_LEN)-1)<<RF_TBB_TOSDAC_I_GC0_POS)
#define RF_TBB_TOSDAC_I_GC0_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC0_LEN)-1)<<RF_TBB_TOSDAC_I_GC0_POS))
#define RF_TBB_TOSDAC_Q_GC0                                     RF_TBB_TOSDAC_Q_GC0
#define RF_TBB_TOSDAC_Q_GC0_POS                                 (8U)
#define RF_TBB_TOSDAC_Q_GC0_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC0_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC0_LEN)-1)<<RF_TBB_TOSDAC_Q_GC0_POS)
#define RF_TBB_TOSDAC_Q_GC0_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC0_LEN)-1)<<RF_TBB_TOSDAC_Q_GC0_POS))
#define RF_TBB_TOSDAC_I_GC1                                     RF_TBB_TOSDAC_I_GC1
#define RF_TBB_TOSDAC_I_GC1_POS                                 (16U)
#define RF_TBB_TOSDAC_I_GC1_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC1_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC1_LEN)-1)<<RF_TBB_TOSDAC_I_GC1_POS)
#define RF_TBB_TOSDAC_I_GC1_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC1_LEN)-1)<<RF_TBB_TOSDAC_I_GC1_POS))
#define RF_TBB_TOSDAC_Q_GC1                                     RF_TBB_TOSDAC_Q_GC1
#define RF_TBB_TOSDAC_Q_GC1_POS                                 (24U)
#define RF_TBB_TOSDAC_Q_GC1_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC1_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC1_LEN)-1)<<RF_TBB_TOSDAC_Q_GC1_POS)
#define RF_TBB_TOSDAC_Q_GC1_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC1_LEN)-1)<<RF_TBB_TOSDAC_Q_GC1_POS))

/* 0x184 : tosdac_ctrl_hw2 */
#define RF_TOSDAC_CTRL_HW2_OFFSET                               (0x184)
#define RF_TBB_TOSDAC_I_GC2                                     RF_TBB_TOSDAC_I_GC2
#define RF_TBB_TOSDAC_I_GC2_POS                                 (0U)
#define RF_TBB_TOSDAC_I_GC2_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC2_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC2_LEN)-1)<<RF_TBB_TOSDAC_I_GC2_POS)
#define RF_TBB_TOSDAC_I_GC2_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC2_LEN)-1)<<RF_TBB_TOSDAC_I_GC2_POS))
#define RF_TBB_TOSDAC_Q_GC2                                     RF_TBB_TOSDAC_Q_GC2
#define RF_TBB_TOSDAC_Q_GC2_POS                                 (8U)
#define RF_TBB_TOSDAC_Q_GC2_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC2_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC2_LEN)-1)<<RF_TBB_TOSDAC_Q_GC2_POS)
#define RF_TBB_TOSDAC_Q_GC2_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC2_LEN)-1)<<RF_TBB_TOSDAC_Q_GC2_POS))
#define RF_TBB_TOSDAC_I_GC3                                     RF_TBB_TOSDAC_I_GC3
#define RF_TBB_TOSDAC_I_GC3_POS                                 (16U)
#define RF_TBB_TOSDAC_I_GC3_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC3_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC3_LEN)-1)<<RF_TBB_TOSDAC_I_GC3_POS)
#define RF_TBB_TOSDAC_I_GC3_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC3_LEN)-1)<<RF_TBB_TOSDAC_I_GC3_POS))
#define RF_TBB_TOSDAC_Q_GC3                                     RF_TBB_TOSDAC_Q_GC3
#define RF_TBB_TOSDAC_Q_GC3_POS                                 (24U)
#define RF_TBB_TOSDAC_Q_GC3_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC3_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC3_LEN)-1)<<RF_TBB_TOSDAC_Q_GC3_POS)
#define RF_TBB_TOSDAC_Q_GC3_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC3_LEN)-1)<<RF_TBB_TOSDAC_Q_GC3_POS))

/* 0x188 : tosdac_ctrl_hw3 */
#define RF_TOSDAC_CTRL_HW3_OFFSET                               (0x188)
#define RF_TBB_TOSDAC_I_GC4                                     RF_TBB_TOSDAC_I_GC4
#define RF_TBB_TOSDAC_I_GC4_POS                                 (0U)
#define RF_TBB_TOSDAC_I_GC4_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC4_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC4_LEN)-1)<<RF_TBB_TOSDAC_I_GC4_POS)
#define RF_TBB_TOSDAC_I_GC4_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC4_LEN)-1)<<RF_TBB_TOSDAC_I_GC4_POS))
#define RF_TBB_TOSDAC_Q_GC4                                     RF_TBB_TOSDAC_Q_GC4
#define RF_TBB_TOSDAC_Q_GC4_POS                                 (8U)
#define RF_TBB_TOSDAC_Q_GC4_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC4_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC4_LEN)-1)<<RF_TBB_TOSDAC_Q_GC4_POS)
#define RF_TBB_TOSDAC_Q_GC4_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC4_LEN)-1)<<RF_TBB_TOSDAC_Q_GC4_POS))
#define RF_TBB_TOSDAC_I_GC5                                     RF_TBB_TOSDAC_I_GC5
#define RF_TBB_TOSDAC_I_GC5_POS                                 (16U)
#define RF_TBB_TOSDAC_I_GC5_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC5_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC5_LEN)-1)<<RF_TBB_TOSDAC_I_GC5_POS)
#define RF_TBB_TOSDAC_I_GC5_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC5_LEN)-1)<<RF_TBB_TOSDAC_I_GC5_POS))
#define RF_TBB_TOSDAC_Q_GC5                                     RF_TBB_TOSDAC_Q_GC5
#define RF_TBB_TOSDAC_Q_GC5_POS                                 (24U)
#define RF_TBB_TOSDAC_Q_GC5_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC5_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC5_LEN)-1)<<RF_TBB_TOSDAC_Q_GC5_POS)
#define RF_TBB_TOSDAC_Q_GC5_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC5_LEN)-1)<<RF_TBB_TOSDAC_Q_GC5_POS))

/* 0x18C : tosdac_ctrl_hw4 */
#define RF_TOSDAC_CTRL_HW4_OFFSET                               (0x18C)
#define RF_TBB_TOSDAC_I_GC6                                     RF_TBB_TOSDAC_I_GC6
#define RF_TBB_TOSDAC_I_GC6_POS                                 (0U)
#define RF_TBB_TOSDAC_I_GC6_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC6_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC6_LEN)-1)<<RF_TBB_TOSDAC_I_GC6_POS)
#define RF_TBB_TOSDAC_I_GC6_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC6_LEN)-1)<<RF_TBB_TOSDAC_I_GC6_POS))
#define RF_TBB_TOSDAC_Q_GC6                                     RF_TBB_TOSDAC_Q_GC6
#define RF_TBB_TOSDAC_Q_GC6_POS                                 (8U)
#define RF_TBB_TOSDAC_Q_GC6_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC6_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC6_LEN)-1)<<RF_TBB_TOSDAC_Q_GC6_POS)
#define RF_TBB_TOSDAC_Q_GC6_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC6_LEN)-1)<<RF_TBB_TOSDAC_Q_GC6_POS))
#define RF_TBB_TOSDAC_I_GC7                                     RF_TBB_TOSDAC_I_GC7
#define RF_TBB_TOSDAC_I_GC7_POS                                 (16U)
#define RF_TBB_TOSDAC_I_GC7_LEN                                 (6U)
#define RF_TBB_TOSDAC_I_GC7_MSK                                 (((1U<<RF_TBB_TOSDAC_I_GC7_LEN)-1)<<RF_TBB_TOSDAC_I_GC7_POS)
#define RF_TBB_TOSDAC_I_GC7_UMSK                                (~(((1U<<RF_TBB_TOSDAC_I_GC7_LEN)-1)<<RF_TBB_TOSDAC_I_GC7_POS))
#define RF_TBB_TOSDAC_Q_GC7                                     RF_TBB_TOSDAC_Q_GC7
#define RF_TBB_TOSDAC_Q_GC7_POS                                 (24U)
#define RF_TBB_TOSDAC_Q_GC7_LEN                                 (6U)
#define RF_TBB_TOSDAC_Q_GC7_MSK                                 (((1U<<RF_TBB_TOSDAC_Q_GC7_LEN)-1)<<RF_TBB_TOSDAC_Q_GC7_POS)
#define RF_TBB_TOSDAC_Q_GC7_UMSK                                (~(((1U<<RF_TBB_TOSDAC_Q_GC7_LEN)-1)<<RF_TBB_TOSDAC_Q_GC7_POS))

/* 0x190 : tx_iq_gain_hw0 */
#define RF_TX_IQ_GAIN_HW0_OFFSET                                (0x190)
#define RF_TX_IQ_PHASE_COMP_GC0                                 RF_TX_IQ_PHASE_COMP_GC0
#define RF_TX_IQ_PHASE_COMP_GC0_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC0_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC0_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC0_POS)
#define RF_TX_IQ_PHASE_COMP_GC0_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC0_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC0_POS))
#define RF_TX_IQ_GAIN_COMP_GC0                                  RF_TX_IQ_GAIN_COMP_GC0
#define RF_TX_IQ_GAIN_COMP_GC0_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC0_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC0_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC0_POS)
#define RF_TX_IQ_GAIN_COMP_GC0_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC0_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC0_POS))

/* 0x194 : tx_iq_gain_hw1 */
#define RF_TX_IQ_GAIN_HW1_OFFSET                                (0x194)
#define RF_TX_IQ_PHASE_COMP_GC1                                 RF_TX_IQ_PHASE_COMP_GC1
#define RF_TX_IQ_PHASE_COMP_GC1_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC1_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC1_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC1_POS)
#define RF_TX_IQ_PHASE_COMP_GC1_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC1_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC1_POS))
#define RF_TX_IQ_GAIN_COMP_GC1                                  RF_TX_IQ_GAIN_COMP_GC1
#define RF_TX_IQ_GAIN_COMP_GC1_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC1_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC1_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC1_POS)
#define RF_TX_IQ_GAIN_COMP_GC1_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC1_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC1_POS))

/* 0x198 : tx_iq_gain_hw2 */
#define RF_TX_IQ_GAIN_HW2_OFFSET                                (0x198)
#define RF_TX_IQ_PHASE_COMP_GC2                                 RF_TX_IQ_PHASE_COMP_GC2
#define RF_TX_IQ_PHASE_COMP_GC2_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC2_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC2_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC2_POS)
#define RF_TX_IQ_PHASE_COMP_GC2_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC2_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC2_POS))
#define RF_TX_IQ_GAIN_COMP_GC2                                  RF_TX_IQ_GAIN_COMP_GC2
#define RF_TX_IQ_GAIN_COMP_GC2_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC2_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC2_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC2_POS)
#define RF_TX_IQ_GAIN_COMP_GC2_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC2_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC2_POS))

/* 0x19C : tx_iq_gain_hw3 */
#define RF_TX_IQ_GAIN_HW3_OFFSET                                (0x19C)
#define RF_TX_IQ_PHASE_COMP_GC3                                 RF_TX_IQ_PHASE_COMP_GC3
#define RF_TX_IQ_PHASE_COMP_GC3_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC3_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC3_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC3_POS)
#define RF_TX_IQ_PHASE_COMP_GC3_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC3_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC3_POS))
#define RF_TX_IQ_GAIN_COMP_GC3                                  RF_TX_IQ_GAIN_COMP_GC3
#define RF_TX_IQ_GAIN_COMP_GC3_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC3_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC3_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC3_POS)
#define RF_TX_IQ_GAIN_COMP_GC3_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC3_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC3_POS))

/* 0x1A0 : tx_iq_gain_hw4 */
#define RF_TX_IQ_GAIN_HW4_OFFSET                                (0x1A0)
#define RF_TX_IQ_PHASE_COMP_GC4                                 RF_TX_IQ_PHASE_COMP_GC4
#define RF_TX_IQ_PHASE_COMP_GC4_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC4_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC4_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC4_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC4_POS)
#define RF_TX_IQ_PHASE_COMP_GC4_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC4_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC4_POS))
#define RF_TX_IQ_GAIN_COMP_GC4                                  RF_TX_IQ_GAIN_COMP_GC4
#define RF_TX_IQ_GAIN_COMP_GC4_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC4_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC4_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC4_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC4_POS)
#define RF_TX_IQ_GAIN_COMP_GC4_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC4_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC4_POS))

/* 0x1A4 : tx_iq_gain_hw5 */
#define RF_TX_IQ_GAIN_HW5_OFFSET                                (0x1A4)
#define RF_TX_IQ_PHASE_COMP_GC5                                 RF_TX_IQ_PHASE_COMP_GC5
#define RF_TX_IQ_PHASE_COMP_GC5_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC5_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC5_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC5_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC5_POS)
#define RF_TX_IQ_PHASE_COMP_GC5_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC5_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC5_POS))
#define RF_TX_IQ_GAIN_COMP_GC5                                  RF_TX_IQ_GAIN_COMP_GC5
#define RF_TX_IQ_GAIN_COMP_GC5_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC5_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC5_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC5_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC5_POS)
#define RF_TX_IQ_GAIN_COMP_GC5_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC5_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC5_POS))

/* 0x1A8 : tx_iq_gain_hw6 */
#define RF_TX_IQ_GAIN_HW6_OFFSET                                (0x1A8)
#define RF_TX_IQ_PHASE_COMP_GC6                                 RF_TX_IQ_PHASE_COMP_GC6
#define RF_TX_IQ_PHASE_COMP_GC6_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC6_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC6_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC6_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC6_POS)
#define RF_TX_IQ_PHASE_COMP_GC6_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC6_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC6_POS))
#define RF_TX_IQ_GAIN_COMP_GC6                                  RF_TX_IQ_GAIN_COMP_GC6
#define RF_TX_IQ_GAIN_COMP_GC6_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC6_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC6_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC6_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC6_POS)
#define RF_TX_IQ_GAIN_COMP_GC6_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC6_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC6_POS))

/* 0x1AC : tx_iq_gain_hw7 */
#define RF_TX_IQ_GAIN_HW7_OFFSET                                (0x1AC)
#define RF_TX_IQ_PHASE_COMP_GC7                                 RF_TX_IQ_PHASE_COMP_GC7
#define RF_TX_IQ_PHASE_COMP_GC7_POS                             (0U)
#define RF_TX_IQ_PHASE_COMP_GC7_LEN                             (10U)
#define RF_TX_IQ_PHASE_COMP_GC7_MSK                             (((1U<<RF_TX_IQ_PHASE_COMP_GC7_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC7_POS)
#define RF_TX_IQ_PHASE_COMP_GC7_UMSK                            (~(((1U<<RF_TX_IQ_PHASE_COMP_GC7_LEN)-1)<<RF_TX_IQ_PHASE_COMP_GC7_POS))
#define RF_TX_IQ_GAIN_COMP_GC7                                  RF_TX_IQ_GAIN_COMP_GC7
#define RF_TX_IQ_GAIN_COMP_GC7_POS                              (16U)
#define RF_TX_IQ_GAIN_COMP_GC7_LEN                              (11U)
#define RF_TX_IQ_GAIN_COMP_GC7_MSK                              (((1U<<RF_TX_IQ_GAIN_COMP_GC7_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC7_POS)
#define RF_TX_IQ_GAIN_COMP_GC7_UMSK                             (~(((1U<<RF_TX_IQ_GAIN_COMP_GC7_LEN)-1)<<RF_TX_IQ_GAIN_COMP_GC7_POS))

/* 0x1B0 : lo_sdm_ctrl_hw1 */
#define RF_LO_SDM_CTRL_HW1_OFFSET                               (0x1B0)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412                          RF_LO_SDM_DITHER_SEL_WLAN_2412
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_POS                      (0U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2412_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2412_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2412_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2417                          RF_LO_SDM_DITHER_SEL_WLAN_2417
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_POS                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2417_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2417_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2417_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2417_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2417_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2422                          RF_LO_SDM_DITHER_SEL_WLAN_2422
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_POS                      (4U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2422_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2422_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2422_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2427                          RF_LO_SDM_DITHER_SEL_WLAN_2427
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_POS                      (6U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2427_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2427_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2427_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2427_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2427_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2432                          RF_LO_SDM_DITHER_SEL_WLAN_2432
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_POS                      (8U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2432_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2432_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2432_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2437                          RF_LO_SDM_DITHER_SEL_WLAN_2437
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_POS                      (10U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2437_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2437_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2437_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2437_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2437_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2442                          RF_LO_SDM_DITHER_SEL_WLAN_2442
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_POS                      (12U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2442_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2442_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2442_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2447                          RF_LO_SDM_DITHER_SEL_WLAN_2447
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_POS                      (14U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2447_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2447_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2447_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2447_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2447_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2452                          RF_LO_SDM_DITHER_SEL_WLAN_2452
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_POS                      (16U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2452_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2452_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2452_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2457                          RF_LO_SDM_DITHER_SEL_WLAN_2457
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_POS                      (18U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2457_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2457_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2457_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2457_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2457_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2462                          RF_LO_SDM_DITHER_SEL_WLAN_2462
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_POS                      (20U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2462_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2462_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2462_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2467                          RF_LO_SDM_DITHER_SEL_WLAN_2467
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_POS                      (22U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2467_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2467_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2467_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2467_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2467_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2472                          RF_LO_SDM_DITHER_SEL_WLAN_2472
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_POS                      (24U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2472_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2472_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2472_POS))
#define RF_LO_SDM_DITHER_SEL_WLAN_2484                          RF_LO_SDM_DITHER_SEL_WLAN_2484
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_POS                      (26U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_LEN                      (2U)
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_MSK                      (((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2484_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2484_POS)
#define RF_LO_SDM_DITHER_SEL_WLAN_2484_UMSK                     (~(((1U<<RF_LO_SDM_DITHER_SEL_WLAN_2484_LEN)-1)<<RF_LO_SDM_DITHER_SEL_WLAN_2484_POS))

/* 0x1B4 : lo_sdm_ctrl_hw2 */
#define RF_LO_SDM_CTRL_HW2_OFFSET                               (0x1B4)
#define RF_LO_SDM_DITHER_SEL_BLE_2402                           RF_LO_SDM_DITHER_SEL_BLE_2402
#define RF_LO_SDM_DITHER_SEL_BLE_2402_POS                       (0U)
#define RF_LO_SDM_DITHER_SEL_BLE_2402_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2402_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2402_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2402_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2402_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2402_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2402_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2404                           RF_LO_SDM_DITHER_SEL_BLE_2404
#define RF_LO_SDM_DITHER_SEL_BLE_2404_POS                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2404_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2404_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2404_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2404_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2404_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2404_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2404_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2406                           RF_LO_SDM_DITHER_SEL_BLE_2406
#define RF_LO_SDM_DITHER_SEL_BLE_2406_POS                       (4U)
#define RF_LO_SDM_DITHER_SEL_BLE_2406_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2406_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2406_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2406_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2406_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2406_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2406_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2408                           RF_LO_SDM_DITHER_SEL_BLE_2408
#define RF_LO_SDM_DITHER_SEL_BLE_2408_POS                       (6U)
#define RF_LO_SDM_DITHER_SEL_BLE_2408_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2408_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2408_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2408_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2408_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2408_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2408_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2410                           RF_LO_SDM_DITHER_SEL_BLE_2410
#define RF_LO_SDM_DITHER_SEL_BLE_2410_POS                       (8U)
#define RF_LO_SDM_DITHER_SEL_BLE_2410_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2410_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2410_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2410_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2410_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2410_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2410_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2412                           RF_LO_SDM_DITHER_SEL_BLE_2412
#define RF_LO_SDM_DITHER_SEL_BLE_2412_POS                       (10U)
#define RF_LO_SDM_DITHER_SEL_BLE_2412_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2412_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2412_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2412_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2412_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2412_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2414                           RF_LO_SDM_DITHER_SEL_BLE_2414
#define RF_LO_SDM_DITHER_SEL_BLE_2414_POS                       (12U)
#define RF_LO_SDM_DITHER_SEL_BLE_2414_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2414_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2414_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2414_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2414_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2414_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2414_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2416                           RF_LO_SDM_DITHER_SEL_BLE_2416
#define RF_LO_SDM_DITHER_SEL_BLE_2416_POS                       (14U)
#define RF_LO_SDM_DITHER_SEL_BLE_2416_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2416_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2416_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2416_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2416_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2416_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2416_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2418                           RF_LO_SDM_DITHER_SEL_BLE_2418
#define RF_LO_SDM_DITHER_SEL_BLE_2418_POS                       (16U)
#define RF_LO_SDM_DITHER_SEL_BLE_2418_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2418_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2418_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2418_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2418_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2418_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2418_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2420                           RF_LO_SDM_DITHER_SEL_BLE_2420
#define RF_LO_SDM_DITHER_SEL_BLE_2420_POS                       (18U)
#define RF_LO_SDM_DITHER_SEL_BLE_2420_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2420_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2420_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2420_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2420_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2420_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2420_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2422                           RF_LO_SDM_DITHER_SEL_BLE_2422
#define RF_LO_SDM_DITHER_SEL_BLE_2422_POS                       (20U)
#define RF_LO_SDM_DITHER_SEL_BLE_2422_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2422_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2422_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2422_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2422_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2422_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2424                           RF_LO_SDM_DITHER_SEL_BLE_2424
#define RF_LO_SDM_DITHER_SEL_BLE_2424_POS                       (22U)
#define RF_LO_SDM_DITHER_SEL_BLE_2424_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2424_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2424_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2424_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2424_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2424_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2424_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2426                           RF_LO_SDM_DITHER_SEL_BLE_2426
#define RF_LO_SDM_DITHER_SEL_BLE_2426_POS                       (24U)
#define RF_LO_SDM_DITHER_SEL_BLE_2426_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2426_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2426_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2426_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2426_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2426_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2426_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2428                           RF_LO_SDM_DITHER_SEL_BLE_2428
#define RF_LO_SDM_DITHER_SEL_BLE_2428_POS                       (26U)
#define RF_LO_SDM_DITHER_SEL_BLE_2428_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2428_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2428_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2428_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2428_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2428_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2428_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2430                           RF_LO_SDM_DITHER_SEL_BLE_2430
#define RF_LO_SDM_DITHER_SEL_BLE_2430_POS                       (28U)
#define RF_LO_SDM_DITHER_SEL_BLE_2430_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2430_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2430_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2430_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2430_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2430_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2430_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2432                           RF_LO_SDM_DITHER_SEL_BLE_2432
#define RF_LO_SDM_DITHER_SEL_BLE_2432_POS                       (30U)
#define RF_LO_SDM_DITHER_SEL_BLE_2432_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2432_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2432_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2432_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2432_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2432_POS))

/* 0x1B8 : lo_sdm_ctrl_hw3 */
#define RF_LO_SDM_CTRL_HW3_OFFSET                               (0x1B8)
#define RF_LO_SDM_DITHER_SEL_BLE_2434                           RF_LO_SDM_DITHER_SEL_BLE_2434
#define RF_LO_SDM_DITHER_SEL_BLE_2434_POS                       (0U)
#define RF_LO_SDM_DITHER_SEL_BLE_2434_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2434_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2434_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2434_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2434_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2434_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2434_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2436                           RF_LO_SDM_DITHER_SEL_BLE_2436
#define RF_LO_SDM_DITHER_SEL_BLE_2436_POS                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2436_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2436_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2436_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2436_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2436_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2436_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2436_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2438                           RF_LO_SDM_DITHER_SEL_BLE_2438
#define RF_LO_SDM_DITHER_SEL_BLE_2438_POS                       (4U)
#define RF_LO_SDM_DITHER_SEL_BLE_2438_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2438_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2438_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2438_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2438_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2438_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2438_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2440                           RF_LO_SDM_DITHER_SEL_BLE_2440
#define RF_LO_SDM_DITHER_SEL_BLE_2440_POS                       (6U)
#define RF_LO_SDM_DITHER_SEL_BLE_2440_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2440_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2440_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2440_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2440_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2440_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2440_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2442                           RF_LO_SDM_DITHER_SEL_BLE_2442
#define RF_LO_SDM_DITHER_SEL_BLE_2442_POS                       (8U)
#define RF_LO_SDM_DITHER_SEL_BLE_2442_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2442_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2442_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2442_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2442_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2442_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2444                           RF_LO_SDM_DITHER_SEL_BLE_2444
#define RF_LO_SDM_DITHER_SEL_BLE_2444_POS                       (10U)
#define RF_LO_SDM_DITHER_SEL_BLE_2444_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2444_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2444_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2444_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2444_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2444_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2444_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2446                           RF_LO_SDM_DITHER_SEL_BLE_2446
#define RF_LO_SDM_DITHER_SEL_BLE_2446_POS                       (12U)
#define RF_LO_SDM_DITHER_SEL_BLE_2446_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2446_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2446_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2446_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2446_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2446_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2446_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2448                           RF_LO_SDM_DITHER_SEL_BLE_2448
#define RF_LO_SDM_DITHER_SEL_BLE_2448_POS                       (14U)
#define RF_LO_SDM_DITHER_SEL_BLE_2448_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2448_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2448_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2448_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2448_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2448_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2448_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2450                           RF_LO_SDM_DITHER_SEL_BLE_2450
#define RF_LO_SDM_DITHER_SEL_BLE_2450_POS                       (16U)
#define RF_LO_SDM_DITHER_SEL_BLE_2450_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2450_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2450_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2450_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2450_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2450_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2450_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2452                           RF_LO_SDM_DITHER_SEL_BLE_2452
#define RF_LO_SDM_DITHER_SEL_BLE_2452_POS                       (18U)
#define RF_LO_SDM_DITHER_SEL_BLE_2452_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2452_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2452_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2452_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2452_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2452_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2454                           RF_LO_SDM_DITHER_SEL_BLE_2454
#define RF_LO_SDM_DITHER_SEL_BLE_2454_POS                       (20U)
#define RF_LO_SDM_DITHER_SEL_BLE_2454_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2454_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2454_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2454_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2454_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2454_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2454_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2456                           RF_LO_SDM_DITHER_SEL_BLE_2456
#define RF_LO_SDM_DITHER_SEL_BLE_2456_POS                       (22U)
#define RF_LO_SDM_DITHER_SEL_BLE_2456_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2456_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2456_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2456_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2456_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2456_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2456_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2458                           RF_LO_SDM_DITHER_SEL_BLE_2458
#define RF_LO_SDM_DITHER_SEL_BLE_2458_POS                       (24U)
#define RF_LO_SDM_DITHER_SEL_BLE_2458_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2458_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2458_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2458_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2458_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2458_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2458_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2460                           RF_LO_SDM_DITHER_SEL_BLE_2460
#define RF_LO_SDM_DITHER_SEL_BLE_2460_POS                       (26U)
#define RF_LO_SDM_DITHER_SEL_BLE_2460_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2460_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2460_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2460_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2460_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2460_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2460_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2462                           RF_LO_SDM_DITHER_SEL_BLE_2462
#define RF_LO_SDM_DITHER_SEL_BLE_2462_POS                       (28U)
#define RF_LO_SDM_DITHER_SEL_BLE_2462_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2462_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2462_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2462_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2462_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2462_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2464                           RF_LO_SDM_DITHER_SEL_BLE_2464
#define RF_LO_SDM_DITHER_SEL_BLE_2464_POS                       (30U)
#define RF_LO_SDM_DITHER_SEL_BLE_2464_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2464_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2464_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2464_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2464_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2464_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2464_POS))

/* 0x1BC : lo_sdm_ctrl_hw4 */
#define RF_LO_SDM_CTRL_HW4_OFFSET                               (0x1BC)
#define RF_LO_SDM_DITHER_SEL_BLE_2466                           RF_LO_SDM_DITHER_SEL_BLE_2466
#define RF_LO_SDM_DITHER_SEL_BLE_2466_POS                       (0U)
#define RF_LO_SDM_DITHER_SEL_BLE_2466_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2466_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2466_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2466_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2466_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2466_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2466_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2468                           RF_LO_SDM_DITHER_SEL_BLE_2468
#define RF_LO_SDM_DITHER_SEL_BLE_2468_POS                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2468_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2468_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2468_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2468_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2468_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2468_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2468_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2470                           RF_LO_SDM_DITHER_SEL_BLE_2470
#define RF_LO_SDM_DITHER_SEL_BLE_2470_POS                       (4U)
#define RF_LO_SDM_DITHER_SEL_BLE_2470_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2470_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2470_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2470_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2470_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2470_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2470_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2472                           RF_LO_SDM_DITHER_SEL_BLE_2472
#define RF_LO_SDM_DITHER_SEL_BLE_2472_POS                       (6U)
#define RF_LO_SDM_DITHER_SEL_BLE_2472_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2472_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2472_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2472_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2472_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2472_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2474                           RF_LO_SDM_DITHER_SEL_BLE_2474
#define RF_LO_SDM_DITHER_SEL_BLE_2474_POS                       (8U)
#define RF_LO_SDM_DITHER_SEL_BLE_2474_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2474_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2474_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2474_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2474_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2474_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2474_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2476                           RF_LO_SDM_DITHER_SEL_BLE_2476
#define RF_LO_SDM_DITHER_SEL_BLE_2476_POS                       (10U)
#define RF_LO_SDM_DITHER_SEL_BLE_2476_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2476_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2476_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2476_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2476_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2476_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2476_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2478                           RF_LO_SDM_DITHER_SEL_BLE_2478
#define RF_LO_SDM_DITHER_SEL_BLE_2478_POS                       (12U)
#define RF_LO_SDM_DITHER_SEL_BLE_2478_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2478_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2478_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2478_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2478_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2478_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2478_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_2480                           RF_LO_SDM_DITHER_SEL_BLE_2480
#define RF_LO_SDM_DITHER_SEL_BLE_2480_POS                       (14U)
#define RF_LO_SDM_DITHER_SEL_BLE_2480_LEN                       (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_2480_MSK                       (((1U<<RF_LO_SDM_DITHER_SEL_BLE_2480_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2480_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_2480_UMSK                      (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_2480_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_2480_POS))
#define RF_LO_SDM_DITHER_SEL_BLE_TX                             RF_LO_SDM_DITHER_SEL_BLE_TX
#define RF_LO_SDM_DITHER_SEL_BLE_TX_POS                         (16U)
#define RF_LO_SDM_DITHER_SEL_BLE_TX_LEN                         (2U)
#define RF_LO_SDM_DITHER_SEL_BLE_TX_MSK                         (((1U<<RF_LO_SDM_DITHER_SEL_BLE_TX_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_TX_POS)
#define RF_LO_SDM_DITHER_SEL_BLE_TX_UMSK                        (~(((1U<<RF_LO_SDM_DITHER_SEL_BLE_TX_LEN)-1)<<RF_LO_SDM_DITHER_SEL_BLE_TX_POS))

/* 0x1C0 : lo_sdm_ctrl_hw5 */
#define RF_LO_SDM_CTRL_HW5_OFFSET                               (0x1C0)
#define RF_LO_CENTER_FREQ_MHZ                                   RF_LO_CENTER_FREQ_MHZ
#define RF_LO_CENTER_FREQ_MHZ_POS                               (0U)
#define RF_LO_CENTER_FREQ_MHZ_LEN                               (12U)
#define RF_LO_CENTER_FREQ_MHZ_MSK                               (((1U<<RF_LO_CENTER_FREQ_MHZ_LEN)-1)<<RF_LO_CENTER_FREQ_MHZ_POS)
#define RF_LO_CENTER_FREQ_MHZ_UMSK                              (~(((1U<<RF_LO_CENTER_FREQ_MHZ_LEN)-1)<<RF_LO_CENTER_FREQ_MHZ_POS))
#define RF_LO_SDM_BYPASS_MODE                                   RF_LO_SDM_BYPASS_MODE
#define RF_LO_SDM_BYPASS_MODE_POS                               (12U)
#define RF_LO_SDM_BYPASS_MODE_LEN                               (6U)
#define RF_LO_SDM_BYPASS_MODE_MSK                               (((1U<<RF_LO_SDM_BYPASS_MODE_LEN)-1)<<RF_LO_SDM_BYPASS_MODE_POS)
#define RF_LO_SDM_BYPASS_MODE_UMSK                              (~(((1U<<RF_LO_SDM_BYPASS_MODE_LEN)-1)<<RF_LO_SDM_BYPASS_MODE_POS))

/* 0x1C4 : lo_sdm_ctrl_hw6 */
#define RF_LO_SDM_CTRL_HW6_OFFSET                               (0x1C4)
#define RF_LO_SDMIN_CENTER                                      RF_LO_SDMIN_CENTER
#define RF_LO_SDMIN_CENTER_POS                                  (0U)
#define RF_LO_SDMIN_CENTER_LEN                                  (29U)
#define RF_LO_SDMIN_CENTER_MSK                                  (((1U<<RF_LO_SDMIN_CENTER_LEN)-1)<<RF_LO_SDMIN_CENTER_POS)
#define RF_LO_SDMIN_CENTER_UMSK                                 (~(((1U<<RF_LO_SDMIN_CENTER_LEN)-1)<<RF_LO_SDMIN_CENTER_POS))

/* 0x1C8 : lo_sdm_ctrl_hw7 */
#define RF_LO_SDM_CTRL_HW7_OFFSET                               (0x1C8)
#define RF_LO_SDMIN_1M                                          RF_LO_SDMIN_1M
#define RF_LO_SDMIN_1M_POS                                      (0U)
#define RF_LO_SDMIN_1M_LEN                                      (20U)
#define RF_LO_SDMIN_1M_MSK                                      (((1U<<RF_LO_SDMIN_1M_LEN)-1)<<RF_LO_SDMIN_1M_POS)
#define RF_LO_SDMIN_1M_UMSK                                     (~(((1U<<RF_LO_SDMIN_1M_LEN)-1)<<RF_LO_SDMIN_1M_POS))

/* 0x1CC : lo_sdm_ctrl_hw8 */
#define RF_LO_SDM_CTRL_HW8_OFFSET                               (0x1CC)
#define RF_LO_SDMIN_IF                                          RF_LO_SDMIN_IF
#define RF_LO_SDMIN_IF_POS                                      (0U)
#define RF_LO_SDMIN_IF_LEN                                      (20U)
#define RF_LO_SDMIN_IF_MSK                                      (((1U<<RF_LO_SDMIN_IF_LEN)-1)<<RF_LO_SDMIN_IF_POS)
#define RF_LO_SDMIN_IF_UMSK                                     (~(((1U<<RF_LO_SDMIN_IF_LEN)-1)<<RF_LO_SDMIN_IF_POS))

/* 0x1D0 : rbb_bw_ctrl_hw */
#define RF_RBB_BW_CTRL_HW_OFFSET                                (0x1D0)
#define RF_RBB_BT_MODE_BLE                                      RF_RBB_BT_MODE_BLE
#define RF_RBB_BT_MODE_BLE_POS                                  (0U)
#define RF_RBB_BT_MODE_BLE_LEN                                  (1U)
#define RF_RBB_BT_MODE_BLE_MSK                                  (((1U<<RF_RBB_BT_MODE_BLE_LEN)-1)<<RF_RBB_BT_MODE_BLE_POS)
#define RF_RBB_BT_MODE_BLE_UMSK                                 (~(((1U<<RF_RBB_BT_MODE_BLE_LEN)-1)<<RF_RBB_BT_MODE_BLE_POS))

/* 0x20C : singen_ctrl0 */
#define RF_SINGEN_CTRL0_OFFSET                                  (0x20C)
#define RF_SINGEN_INC_STEP1                                     RF_SINGEN_INC_STEP1
#define RF_SINGEN_INC_STEP1_POS                                 (0U)
#define RF_SINGEN_INC_STEP1_LEN                                 (10U)
#define RF_SINGEN_INC_STEP1_MSK                                 (((1U<<RF_SINGEN_INC_STEP1_LEN)-1)<<RF_SINGEN_INC_STEP1_POS)
#define RF_SINGEN_INC_STEP1_UMSK                                (~(((1U<<RF_SINGEN_INC_STEP1_LEN)-1)<<RF_SINGEN_INC_STEP1_POS))
#define RF_SINGEN_INC_STEP0                                     RF_SINGEN_INC_STEP0
#define RF_SINGEN_INC_STEP0_POS                                 (16U)
#define RF_SINGEN_INC_STEP0_LEN                                 (10U)
#define RF_SINGEN_INC_STEP0_MSK                                 (((1U<<RF_SINGEN_INC_STEP0_LEN)-1)<<RF_SINGEN_INC_STEP0_POS)
#define RF_SINGEN_INC_STEP0_UMSK                                (~(((1U<<RF_SINGEN_INC_STEP0_LEN)-1)<<RF_SINGEN_INC_STEP0_POS))
#define RF_SINGEN_UNSIGN_EN                                     RF_SINGEN_UNSIGN_EN
#define RF_SINGEN_UNSIGN_EN_POS                                 (28U)
#define RF_SINGEN_UNSIGN_EN_LEN                                 (1U)
#define RF_SINGEN_UNSIGN_EN_MSK                                 (((1U<<RF_SINGEN_UNSIGN_EN_LEN)-1)<<RF_SINGEN_UNSIGN_EN_POS)
#define RF_SINGEN_UNSIGN_EN_UMSK                                (~(((1U<<RF_SINGEN_UNSIGN_EN_LEN)-1)<<RF_SINGEN_UNSIGN_EN_POS))
#define RF_SINGEN_CLKDIV_N                                      RF_SINGEN_CLKDIV_N
#define RF_SINGEN_CLKDIV_N_POS                                  (29U)
#define RF_SINGEN_CLKDIV_N_LEN                                  (2U)
#define RF_SINGEN_CLKDIV_N_MSK                                  (((1U<<RF_SINGEN_CLKDIV_N_LEN)-1)<<RF_SINGEN_CLKDIV_N_POS)
#define RF_SINGEN_CLKDIV_N_UMSK                                 (~(((1U<<RF_SINGEN_CLKDIV_N_LEN)-1)<<RF_SINGEN_CLKDIV_N_POS))
#define RF_SINGEN_EN                                            RF_SINGEN_EN
#define RF_SINGEN_EN_POS                                        (31U)
#define RF_SINGEN_EN_LEN                                        (1U)
#define RF_SINGEN_EN_MSK                                        (((1U<<RF_SINGEN_EN_LEN)-1)<<RF_SINGEN_EN_POS)
#define RF_SINGEN_EN_UMSK                                       (~(((1U<<RF_SINGEN_EN_LEN)-1)<<RF_SINGEN_EN_POS))

/* 0x210 : singen_ctrl1 */
#define RF_SINGEN_CTRL1_OFFSET                                  (0x210)
#define RF_SINGEN_CLKDIV_Q                                      RF_SINGEN_CLKDIV_Q
#define RF_SINGEN_CLKDIV_Q_POS                                  (0U)
#define RF_SINGEN_CLKDIV_Q_LEN                                  (10U)
#define RF_SINGEN_CLKDIV_Q_MSK                                  (((1U<<RF_SINGEN_CLKDIV_Q_LEN)-1)<<RF_SINGEN_CLKDIV_Q_POS)
#define RF_SINGEN_CLKDIV_Q_UMSK                                 (~(((1U<<RF_SINGEN_CLKDIV_Q_LEN)-1)<<RF_SINGEN_CLKDIV_Q_POS))
#define RF_SINGEN_MODE_Q                                        RF_SINGEN_MODE_Q
#define RF_SINGEN_MODE_Q_POS                                    (12U)
#define RF_SINGEN_MODE_Q_LEN                                    (4U)
#define RF_SINGEN_MODE_Q_MSK                                    (((1U<<RF_SINGEN_MODE_Q_LEN)-1)<<RF_SINGEN_MODE_Q_POS)
#define RF_SINGEN_MODE_Q_UMSK                                   (~(((1U<<RF_SINGEN_MODE_Q_LEN)-1)<<RF_SINGEN_MODE_Q_POS))
#define RF_SINGEN_CLKDIV_I                                      RF_SINGEN_CLKDIV_I
#define RF_SINGEN_CLKDIV_I_POS                                  (16U)
#define RF_SINGEN_CLKDIV_I_LEN                                  (10U)
#define RF_SINGEN_CLKDIV_I_MSK                                  (((1U<<RF_SINGEN_CLKDIV_I_LEN)-1)<<RF_SINGEN_CLKDIV_I_POS)
#define RF_SINGEN_CLKDIV_I_UMSK                                 (~(((1U<<RF_SINGEN_CLKDIV_I_LEN)-1)<<RF_SINGEN_CLKDIV_I_POS))
#define RF_SINGEN_MODE_I                                        RF_SINGEN_MODE_I
#define RF_SINGEN_MODE_I_POS                                    (28U)
#define RF_SINGEN_MODE_I_LEN                                    (4U)
#define RF_SINGEN_MODE_I_MSK                                    (((1U<<RF_SINGEN_MODE_I_LEN)-1)<<RF_SINGEN_MODE_I_POS)
#define RF_SINGEN_MODE_I_UMSK                                   (~(((1U<<RF_SINGEN_MODE_I_LEN)-1)<<RF_SINGEN_MODE_I_POS))

/* 0x214 : singen_ctrl2 */
#define RF_SINGEN_CTRL2_OFFSET                                  (0x214)
#define RF_SINGEN_GAIN_I                                        RF_SINGEN_GAIN_I
#define RF_SINGEN_GAIN_I_POS                                    (0U)
#define RF_SINGEN_GAIN_I_LEN                                    (11U)
#define RF_SINGEN_GAIN_I_MSK                                    (((1U<<RF_SINGEN_GAIN_I_LEN)-1)<<RF_SINGEN_GAIN_I_POS)
#define RF_SINGEN_GAIN_I_UMSK                                   (~(((1U<<RF_SINGEN_GAIN_I_LEN)-1)<<RF_SINGEN_GAIN_I_POS))
#define RF_SINGEN_START_ADDR1_I                                 RF_SINGEN_START_ADDR1_I
#define RF_SINGEN_START_ADDR1_I_POS                             (12U)
#define RF_SINGEN_START_ADDR1_I_LEN                             (10U)
#define RF_SINGEN_START_ADDR1_I_MSK                             (((1U<<RF_SINGEN_START_ADDR1_I_LEN)-1)<<RF_SINGEN_START_ADDR1_I_POS)
#define RF_SINGEN_START_ADDR1_I_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR1_I_LEN)-1)<<RF_SINGEN_START_ADDR1_I_POS))
#define RF_SINGEN_START_ADDR0_I                                 RF_SINGEN_START_ADDR0_I
#define RF_SINGEN_START_ADDR0_I_POS                             (22U)
#define RF_SINGEN_START_ADDR0_I_LEN                             (10U)
#define RF_SINGEN_START_ADDR0_I_MSK                             (((1U<<RF_SINGEN_START_ADDR0_I_LEN)-1)<<RF_SINGEN_START_ADDR0_I_POS)
#define RF_SINGEN_START_ADDR0_I_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR0_I_LEN)-1)<<RF_SINGEN_START_ADDR0_I_POS))

/* 0x218 : singen_ctrl3 */
#define RF_SINGEN_CTRL3_OFFSET                                  (0x218)
#define RF_SINGEN_GAIN_Q                                        RF_SINGEN_GAIN_Q
#define RF_SINGEN_GAIN_Q_POS                                    (0U)
#define RF_SINGEN_GAIN_Q_LEN                                    (11U)
#define RF_SINGEN_GAIN_Q_MSK                                    (((1U<<RF_SINGEN_GAIN_Q_LEN)-1)<<RF_SINGEN_GAIN_Q_POS)
#define RF_SINGEN_GAIN_Q_UMSK                                   (~(((1U<<RF_SINGEN_GAIN_Q_LEN)-1)<<RF_SINGEN_GAIN_Q_POS))
#define RF_SINGEN_START_ADDR1_Q                                 RF_SINGEN_START_ADDR1_Q
#define RF_SINGEN_START_ADDR1_Q_POS                             (12U)
#define RF_SINGEN_START_ADDR1_Q_LEN                             (10U)
#define RF_SINGEN_START_ADDR1_Q_MSK                             (((1U<<RF_SINGEN_START_ADDR1_Q_LEN)-1)<<RF_SINGEN_START_ADDR1_Q_POS)
#define RF_SINGEN_START_ADDR1_Q_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR1_Q_LEN)-1)<<RF_SINGEN_START_ADDR1_Q_POS))
#define RF_SINGEN_START_ADDR0_Q                                 RF_SINGEN_START_ADDR0_Q
#define RF_SINGEN_START_ADDR0_Q_POS                             (22U)
#define RF_SINGEN_START_ADDR0_Q_LEN                             (10U)
#define RF_SINGEN_START_ADDR0_Q_MSK                             (((1U<<RF_SINGEN_START_ADDR0_Q_LEN)-1)<<RF_SINGEN_START_ADDR0_Q_POS)
#define RF_SINGEN_START_ADDR0_Q_UMSK                            (~(((1U<<RF_SINGEN_START_ADDR0_Q_LEN)-1)<<RF_SINGEN_START_ADDR0_Q_POS))

/* 0x21C : singen_ctrl4 */
#define RF_SINGEN_CTRL4_OFFSET                                  (0x21C)
#define RF_SINGEN_FIX_Q                                         RF_SINGEN_FIX_Q
#define RF_SINGEN_FIX_Q_POS                                     (0U)
#define RF_SINGEN_FIX_Q_LEN                                     (12U)
#define RF_SINGEN_FIX_Q_MSK                                     (((1U<<RF_SINGEN_FIX_Q_LEN)-1)<<RF_SINGEN_FIX_Q_POS)
#define RF_SINGEN_FIX_Q_UMSK                                    (~(((1U<<RF_SINGEN_FIX_Q_LEN)-1)<<RF_SINGEN_FIX_Q_POS))
#define RF_SINGEN_FIX_EN_Q                                      RF_SINGEN_FIX_EN_Q
#define RF_SINGEN_FIX_EN_Q_POS                                  (12U)
#define RF_SINGEN_FIX_EN_Q_LEN                                  (1U)
#define RF_SINGEN_FIX_EN_Q_MSK                                  (((1U<<RF_SINGEN_FIX_EN_Q_LEN)-1)<<RF_SINGEN_FIX_EN_Q_POS)
#define RF_SINGEN_FIX_EN_Q_UMSK                                 (~(((1U<<RF_SINGEN_FIX_EN_Q_LEN)-1)<<RF_SINGEN_FIX_EN_Q_POS))
#define RF_SINGEN_FIX_I                                         RF_SINGEN_FIX_I
#define RF_SINGEN_FIX_I_POS                                     (16U)
#define RF_SINGEN_FIX_I_LEN                                     (12U)
#define RF_SINGEN_FIX_I_MSK                                     (((1U<<RF_SINGEN_FIX_I_LEN)-1)<<RF_SINGEN_FIX_I_POS)
#define RF_SINGEN_FIX_I_UMSK                                    (~(((1U<<RF_SINGEN_FIX_I_LEN)-1)<<RF_SINGEN_FIX_I_POS))
#define RF_SINGEN_FIX_EN_I                                      RF_SINGEN_FIX_EN_I
#define RF_SINGEN_FIX_EN_I_POS                                  (28U)
#define RF_SINGEN_FIX_EN_I_LEN                                  (1U)
#define RF_SINGEN_FIX_EN_I_MSK                                  (((1U<<RF_SINGEN_FIX_EN_I_LEN)-1)<<RF_SINGEN_FIX_EN_I_POS)
#define RF_SINGEN_FIX_EN_I_UMSK                                 (~(((1U<<RF_SINGEN_FIX_EN_I_LEN)-1)<<RF_SINGEN_FIX_EN_I_POS))

/* 0x220 : rfif_dfe_ctrl0 */
#define RFIF_DFE_CTRL0_OFFSET                                   (0x220)
#define RFCKG_RXCLK_4S_ON                                       RFCKG_RXCLK_4S_ON
#define RFCKG_RXCLK_4S_ON_POS                                   (0U)
#define RFCKG_RXCLK_4S_ON_LEN                                   (1U)
#define RFCKG_RXCLK_4S_ON_MSK                                   (((1U<<RFCKG_RXCLK_4S_ON_LEN)-1)<<RFCKG_RXCLK_4S_ON_POS)
#define RFCKG_RXCLK_4S_ON_UMSK                                  (~(((1U<<RFCKG_RXCLK_4S_ON_LEN)-1)<<RFCKG_RXCLK_4S_ON_POS))
#define RFCKG_TXCLK_4S_ON                                       RFCKG_TXCLK_4S_ON
#define RFCKG_TXCLK_4S_ON_POS                                   (1U)
#define RFCKG_TXCLK_4S_ON_LEN                                   (1U)
#define RFCKG_TXCLK_4S_ON_MSK                                   (((1U<<RFCKG_TXCLK_4S_ON_LEN)-1)<<RFCKG_TXCLK_4S_ON_POS)
#define RFCKG_TXCLK_4S_ON_UMSK                                  (~(((1U<<RFCKG_TXCLK_4S_ON_LEN)-1)<<RFCKG_TXCLK_4S_ON_POS))
#define RFCKG_ADC_AFIFO_INV                                     RFCKG_ADC_AFIFO_INV
#define RFCKG_ADC_AFIFO_INV_POS                                 (2U)
#define RFCKG_ADC_AFIFO_INV_LEN                                 (1U)
#define RFCKG_ADC_AFIFO_INV_MSK                                 (((1U<<RFCKG_ADC_AFIFO_INV_LEN)-1)<<RFCKG_ADC_AFIFO_INV_POS)
#define RFCKG_ADC_AFIFO_INV_UMSK                                (~(((1U<<RFCKG_ADC_AFIFO_INV_LEN)-1)<<RFCKG_ADC_AFIFO_INV_POS))
#define RFCKG_ADC_CLKOUT_SEL                                    RFCKG_ADC_CLKOUT_SEL
#define RFCKG_ADC_CLKOUT_SEL_POS                                (3U)
#define RFCKG_ADC_CLKOUT_SEL_LEN                                (1U)
#define RFCKG_ADC_CLKOUT_SEL_MSK                                (((1U<<RFCKG_ADC_CLKOUT_SEL_LEN)-1)<<RFCKG_ADC_CLKOUT_SEL_POS)
#define RFCKG_ADC_CLKOUT_SEL_UMSK                               (~(((1U<<RFCKG_ADC_CLKOUT_SEL_LEN)-1)<<RFCKG_ADC_CLKOUT_SEL_POS))
#define RFCKG_DAC_AFIFO_INV                                     RFCKG_DAC_AFIFO_INV
#define RFCKG_DAC_AFIFO_INV_POS                                 (4U)
#define RFCKG_DAC_AFIFO_INV_LEN                                 (1U)
#define RFCKG_DAC_AFIFO_INV_MSK                                 (((1U<<RFCKG_DAC_AFIFO_INV_LEN)-1)<<RFCKG_DAC_AFIFO_INV_POS)
#define RFCKG_DAC_AFIFO_INV_UMSK                                (~(((1U<<RFCKG_DAC_AFIFO_INV_LEN)-1)<<RFCKG_DAC_AFIFO_INV_POS))
#define RF_RX_DFE_EN_4S                                         RF_RX_DFE_EN_4S
#define RF_RX_DFE_EN_4S_POS                                     (5U)
#define RF_RX_DFE_EN_4S_LEN                                     (1U)
#define RF_RX_DFE_EN_4S_MSK                                     (((1U<<RF_RX_DFE_EN_4S_LEN)-1)<<RF_RX_DFE_EN_4S_POS)
#define RF_RX_DFE_EN_4S_UMSK                                    (~(((1U<<RF_RX_DFE_EN_4S_LEN)-1)<<RF_RX_DFE_EN_4S_POS))
#define RF_RX_DFE_EN_4S_EN                                      RF_RX_DFE_EN_4S_EN
#define RF_RX_DFE_EN_4S_EN_POS                                  (6U)
#define RF_RX_DFE_EN_4S_EN_LEN                                  (1U)
#define RF_RX_DFE_EN_4S_EN_MSK                                  (((1U<<RF_RX_DFE_EN_4S_EN_LEN)-1)<<RF_RX_DFE_EN_4S_EN_POS)
#define RF_RX_DFE_EN_4S_EN_UMSK                                 (~(((1U<<RF_RX_DFE_EN_4S_EN_LEN)-1)<<RF_RX_DFE_EN_4S_EN_POS))
#define RF_TX_DFE_EN_4S                                         RF_TX_DFE_EN_4S
#define RF_TX_DFE_EN_4S_POS                                     (7U)
#define RF_TX_DFE_EN_4S_LEN                                     (1U)
#define RF_TX_DFE_EN_4S_MSK                                     (((1U<<RF_TX_DFE_EN_4S_LEN)-1)<<RF_TX_DFE_EN_4S_POS)
#define RF_TX_DFE_EN_4S_UMSK                                    (~(((1U<<RF_TX_DFE_EN_4S_LEN)-1)<<RF_TX_DFE_EN_4S_POS))
#define RF_TX_DFE_EN_4S_EN                                      RF_TX_DFE_EN_4S_EN
#define RF_TX_DFE_EN_4S_EN_POS                                  (8U)
#define RF_TX_DFE_EN_4S_EN_LEN                                  (1U)
#define RF_TX_DFE_EN_4S_EN_MSK                                  (((1U<<RF_TX_DFE_EN_4S_EN_LEN)-1)<<RF_TX_DFE_EN_4S_EN_POS)
#define RF_TX_DFE_EN_4S_EN_UMSK                                 (~(((1U<<RF_TX_DFE_EN_4S_EN_LEN)-1)<<RF_TX_DFE_EN_4S_EN_POS))
#define RF_RX_TEST_SEL                                          RF_RX_TEST_SEL
#define RF_RX_TEST_SEL_POS                                      (9U)
#define RF_RX_TEST_SEL_LEN                                      (2U)
#define RF_RX_TEST_SEL_MSK                                      (((1U<<RF_RX_TEST_SEL_LEN)-1)<<RF_RX_TEST_SEL_POS)
#define RF_RX_TEST_SEL_UMSK                                     (~(((1U<<RF_RX_TEST_SEL_LEN)-1)<<RF_RX_TEST_SEL_POS))
#define RF_TX_TEST_SEL                                          RF_TX_TEST_SEL
#define RF_TX_TEST_SEL_POS                                      (11U)
#define RF_TX_TEST_SEL_LEN                                      (2U)
#define RF_TX_TEST_SEL_MSK                                      (((1U<<RF_TX_TEST_SEL_LEN)-1)<<RF_TX_TEST_SEL_POS)
#define RF_TX_TEST_SEL_UMSK                                     (~(((1U<<RF_TX_TEST_SEL_LEN)-1)<<RF_TX_TEST_SEL_POS))
#define RF_PAD_ADC_CLKOUT_INV_EN                                RF_PAD_ADC_CLKOUT_INV_EN
#define RF_PAD_ADC_CLKOUT_INV_EN_POS                            (13U)
#define RF_PAD_ADC_CLKOUT_INV_EN_LEN                            (1U)
#define RF_PAD_ADC_CLKOUT_INV_EN_MSK                            (((1U<<RF_PAD_ADC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_ADC_CLKOUT_INV_EN_POS)
#define RF_PAD_ADC_CLKOUT_INV_EN_UMSK                           (~(((1U<<RF_PAD_ADC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_ADC_CLKOUT_INV_EN_POS))
#define RF_PAD_DAC_CLKOUT_INV_EN                                RF_PAD_DAC_CLKOUT_INV_EN
#define RF_PAD_DAC_CLKOUT_INV_EN_POS                            (14U)
#define RF_PAD_DAC_CLKOUT_INV_EN_LEN                            (1U)
#define RF_PAD_DAC_CLKOUT_INV_EN_MSK                            (((1U<<RF_PAD_DAC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_DAC_CLKOUT_INV_EN_POS)
#define RF_PAD_DAC_CLKOUT_INV_EN_UMSK                           (~(((1U<<RF_PAD_DAC_CLKOUT_INV_EN_LEN)-1)<<RF_PAD_DAC_CLKOUT_INV_EN_POS))
#define RF_CH_IND_BLE_4S                                        RF_CH_IND_BLE_4S
#define RF_CH_IND_BLE_4S_POS                                    (15U)
#define RF_CH_IND_BLE_4S_LEN                                    (7U)
#define RF_CH_IND_BLE_4S_MSK                                    (((1U<<RF_CH_IND_BLE_4S_LEN)-1)<<RF_CH_IND_BLE_4S_POS)
#define RF_CH_IND_BLE_4S_UMSK                                   (~(((1U<<RF_CH_IND_BLE_4S_LEN)-1)<<RF_CH_IND_BLE_4S_POS))
#define RF_CH_IND_BLE_4S_EN                                     RF_CH_IND_BLE_4S_EN
#define RF_CH_IND_BLE_4S_EN_POS                                 (22U)
#define RF_CH_IND_BLE_4S_EN_LEN                                 (1U)
#define RF_CH_IND_BLE_4S_EN_MSK                                 (((1U<<RF_CH_IND_BLE_4S_EN_LEN)-1)<<RF_CH_IND_BLE_4S_EN_POS)
#define RF_CH_IND_BLE_4S_EN_UMSK                                (~(((1U<<RF_CH_IND_BLE_4S_EN_LEN)-1)<<RF_CH_IND_BLE_4S_EN_POS))
#define RF_WIFIMODE_4S                                          RF_WIFIMODE_4S
#define RF_WIFIMODE_4S_POS                                      (23U)
#define RF_WIFIMODE_4S_LEN                                      (2U)
#define RF_WIFIMODE_4S_MSK                                      (((1U<<RF_WIFIMODE_4S_LEN)-1)<<RF_WIFIMODE_4S_POS)
#define RF_WIFIMODE_4S_UMSK                                     (~(((1U<<RF_WIFIMODE_4S_LEN)-1)<<RF_WIFIMODE_4S_POS))
#define RF_WIFIMODE_4S_EN                                       RF_WIFIMODE_4S_EN
#define RF_WIFIMODE_4S_EN_POS                                   (25U)
#define RF_WIFIMODE_4S_EN_LEN                                   (1U)
#define RF_WIFIMODE_4S_EN_MSK                                   (((1U<<RF_WIFIMODE_4S_EN_LEN)-1)<<RF_WIFIMODE_4S_EN_POS)
#define RF_WIFIMODE_4S_EN_UMSK                                  (~(((1U<<RF_WIFIMODE_4S_EN_LEN)-1)<<RF_WIFIMODE_4S_EN_POS))
#define RF_BBMODE_4S                                            RF_BBMODE_4S
#define RF_BBMODE_4S_POS                                        (26U)
#define RF_BBMODE_4S_LEN                                        (1U)
#define RF_BBMODE_4S_MSK                                        (((1U<<RF_BBMODE_4S_LEN)-1)<<RF_BBMODE_4S_POS)
#define RF_BBMODE_4S_UMSK                                       (~(((1U<<RF_BBMODE_4S_LEN)-1)<<RF_BBMODE_4S_POS))
#define RF_BBMODE_4S_EN                                         RF_BBMODE_4S_EN
#define RF_BBMODE_4S_EN_POS                                     (27U)
#define RF_BBMODE_4S_EN_LEN                                     (1U)
#define RF_BBMODE_4S_EN_MSK                                     (((1U<<RF_BBMODE_4S_EN_LEN)-1)<<RF_BBMODE_4S_EN_POS)
#define RF_BBMODE_4S_EN_UMSK                                    (~(((1U<<RF_BBMODE_4S_EN_LEN)-1)<<RF_BBMODE_4S_EN_POS))
#define RF_TEST_SEL                                             RF_TEST_SEL
#define RF_TEST_SEL_POS                                         (28U)
#define RF_TEST_SEL_LEN                                         (4U)
#define RF_TEST_SEL_MSK                                         (((1U<<RF_TEST_SEL_LEN)-1)<<RF_TEST_SEL_POS)
#define RF_TEST_SEL_UMSK                                        (~(((1U<<RF_TEST_SEL_LEN)-1)<<RF_TEST_SEL_POS))

/* 0x224 : rfif_test_read */
#define RFIF_TEST_READ_OFFSET                                   (0x224)
#define RF_TEST_READ                                            RF_TEST_READ
#define RF_TEST_READ_POS                                        (0U)
#define RF_TEST_READ_LEN                                        (32U)
#define RF_TEST_READ_MSK                                        (((1U<<RF_TEST_READ_LEN)-1)<<RF_TEST_READ_POS)
#define RF_TEST_READ_UMSK                                       (~(((1U<<RF_TEST_READ_LEN)-1)<<RF_TEST_READ_POS))

/* 0x228 : rfif_dig_ctrl */
#define RFIF_DIG_CTRL_OFFSET                                    (0x228)
#define RF_TEST_FROM_PAD_EN                                     RF_TEST_FROM_PAD_EN
#define RF_TEST_FROM_PAD_EN_POS                                 (0U)
#define RF_TEST_FROM_PAD_EN_LEN                                 (1U)
#define RF_TEST_FROM_PAD_EN_MSK                                 (((1U<<RF_TEST_FROM_PAD_EN_LEN)-1)<<RF_TEST_FROM_PAD_EN_POS)
#define RF_TEST_FROM_PAD_EN_UMSK                                (~(((1U<<RF_TEST_FROM_PAD_EN_LEN)-1)<<RF_TEST_FROM_PAD_EN_POS))
#define RF_TEST_GC_FROM_PAD_EN                                  RF_TEST_GC_FROM_PAD_EN
#define RF_TEST_GC_FROM_PAD_EN_POS                              (1U)
#define RF_TEST_GC_FROM_PAD_EN_LEN                              (1U)
#define RF_TEST_GC_FROM_PAD_EN_MSK                              (((1U<<RF_TEST_GC_FROM_PAD_EN_LEN)-1)<<RF_TEST_GC_FROM_PAD_EN_POS)
#define RF_TEST_GC_FROM_PAD_EN_UMSK                             (~(((1U<<RF_TEST_GC_FROM_PAD_EN_LEN)-1)<<RF_TEST_GC_FROM_PAD_EN_POS))
#define RFCKG_RXCLK_DIV2_MODE                                   RFCKG_RXCLK_DIV2_MODE
#define RFCKG_RXCLK_DIV2_MODE_POS                               (2U)
#define RFCKG_RXCLK_DIV2_MODE_LEN                               (1U)
#define RFCKG_RXCLK_DIV2_MODE_MSK                               (((1U<<RFCKG_RXCLK_DIV2_MODE_LEN)-1)<<RFCKG_RXCLK_DIV2_MODE_POS)
#define RFCKG_RXCLK_DIV2_MODE_UMSK                              (~(((1U<<RFCKG_RXCLK_DIV2_MODE_LEN)-1)<<RFCKG_RXCLK_DIV2_MODE_POS))
#define RFIF_INT_LO_UNLOCKED_MASK                               RFIF_INT_LO_UNLOCKED_MASK
#define RFIF_INT_LO_UNLOCKED_MASK_POS                           (3U)
#define RFIF_INT_LO_UNLOCKED_MASK_LEN                           (1U)
#define RFIF_INT_LO_UNLOCKED_MASK_MSK                           (((1U<<RFIF_INT_LO_UNLOCKED_MASK_LEN)-1)<<RFIF_INT_LO_UNLOCKED_MASK_POS)
#define RFIF_INT_LO_UNLOCKED_MASK_UMSK                          (~(((1U<<RFIF_INT_LO_UNLOCKED_MASK_LEN)-1)<<RFIF_INT_LO_UNLOCKED_MASK_POS))
#define RFIF_PPUD_CNT2                                          RFIF_PPUD_CNT2
#define RFIF_PPUD_CNT2_POS                                      (16U)
#define RFIF_PPUD_CNT2_LEN                                      (9U)
#define RFIF_PPUD_CNT2_MSK                                      (((1U<<RFIF_PPUD_CNT2_LEN)-1)<<RFIF_PPUD_CNT2_POS)
#define RFIF_PPUD_CNT2_UMSK                                     (~(((1U<<RFIF_PPUD_CNT2_LEN)-1)<<RFIF_PPUD_CNT2_POS))
#define RFIF_PPUD_CNT1                                          RFIF_PPUD_CNT1
#define RFIF_PPUD_CNT1_POS                                      (25U)
#define RFIF_PPUD_CNT1_LEN                                      (5U)
#define RFIF_PPUD_CNT1_MSK                                      (((1U<<RFIF_PPUD_CNT1_LEN)-1)<<RFIF_PPUD_CNT1_POS)
#define RFIF_PPUD_CNT1_UMSK                                     (~(((1U<<RFIF_PPUD_CNT1_LEN)-1)<<RFIF_PPUD_CNT1_POS))
#define RFIF_PPUD_MANAUAL_EN                                    RFIF_PPUD_MANAUAL_EN
#define RFIF_PPUD_MANAUAL_EN_POS                                (30U)
#define RFIF_PPUD_MANAUAL_EN_LEN                                (1U)
#define RFIF_PPUD_MANAUAL_EN_MSK                                (((1U<<RFIF_PPUD_MANAUAL_EN_LEN)-1)<<RFIF_PPUD_MANAUAL_EN_POS)
#define RFIF_PPUD_MANAUAL_EN_UMSK                               (~(((1U<<RFIF_PPUD_MANAUAL_EN_LEN)-1)<<RFIF_PPUD_MANAUAL_EN_POS))

/* 0x22C : rf_data_temp_0 */
#define RF_DATA_TEMP_0_OFFSET                                   (0x22C)
#define RF_DATA_TEMP_0                                          RF_DATA_TEMP_0
#define RF_DATA_TEMP_0_POS                                      (0U)
#define RF_DATA_TEMP_0_LEN                                      (32U)
#define RF_DATA_TEMP_0_MSK                                      (((1U<<RF_DATA_TEMP_0_LEN)-1)<<RF_DATA_TEMP_0_POS)
#define RF_DATA_TEMP_0_UMSK                                     (~(((1U<<RF_DATA_TEMP_0_LEN)-1)<<RF_DATA_TEMP_0_POS))

/* 0x230 : rf_data_temp_1 */
#define RF_DATA_TEMP_1_OFFSET                                   (0x230)
#define RF_DATA_TEMP_1                                          RF_DATA_TEMP_1
#define RF_DATA_TEMP_1_POS                                      (0U)
#define RF_DATA_TEMP_1_LEN                                      (32U)
#define RF_DATA_TEMP_1_MSK                                      (((1U<<RF_DATA_TEMP_1_LEN)-1)<<RF_DATA_TEMP_1_POS)
#define RF_DATA_TEMP_1_UMSK                                     (~(((1U<<RF_DATA_TEMP_1_LEN)-1)<<RF_DATA_TEMP_1_POS))

/* 0x234 : rf_data_temp_2 */
#define RF_DATA_TEMP_2_OFFSET                                   (0x234)
#define RF_DATA_TEMP_2                                          RF_DATA_TEMP_2
#define RF_DATA_TEMP_2_POS                                      (0U)
#define RF_DATA_TEMP_2_LEN                                      (32U)
#define RF_DATA_TEMP_2_MSK                                      (((1U<<RF_DATA_TEMP_2_LEN)-1)<<RF_DATA_TEMP_2_POS)
#define RF_DATA_TEMP_2_UMSK                                     (~(((1U<<RF_DATA_TEMP_2_LEN)-1)<<RF_DATA_TEMP_2_POS))

/* 0x238 : rf_data_temp_3 */
#define RF_DATA_TEMP_3_OFFSET                                   (0x238)
#define RF_DATA_TEMP_3                                          RF_DATA_TEMP_3
#define RF_DATA_TEMP_3_POS                                      (0U)
#define RF_DATA_TEMP_3_LEN                                      (32U)
#define RF_DATA_TEMP_3_MSK                                      (((1U<<RF_DATA_TEMP_3_LEN)-1)<<RF_DATA_TEMP_3_POS)
#define RF_DATA_TEMP_3_UMSK                                     (~(((1U<<RF_DATA_TEMP_3_LEN)-1)<<RF_DATA_TEMP_3_POS))

/* 0x23C : rf_sram_ctrl0 */
#define RF_SRAM_CTRL0_OFFSET                                    (0x23C)
#define RF_SRAM_LINK_DLY                                        RF_SRAM_LINK_DLY
#define RF_SRAM_LINK_DLY_POS                                    (0U)
#define RF_SRAM_LINK_DLY_LEN                                    (16U)
#define RF_SRAM_LINK_DLY_MSK                                    (((1U<<RF_SRAM_LINK_DLY_LEN)-1)<<RF_SRAM_LINK_DLY_POS)
#define RF_SRAM_LINK_DLY_UMSK                                   (~(((1U<<RF_SRAM_LINK_DLY_LEN)-1)<<RF_SRAM_LINK_DLY_POS))
#define RF_SRAM_LINK_MODE                                       RF_SRAM_LINK_MODE
#define RF_SRAM_LINK_MODE_POS                                   (16U)
#define RF_SRAM_LINK_MODE_LEN                                   (2U)
#define RF_SRAM_LINK_MODE_MSK                                   (((1U<<RF_SRAM_LINK_MODE_LEN)-1)<<RF_SRAM_LINK_MODE_POS)
#define RF_SRAM_LINK_MODE_UMSK                                  (~(((1U<<RF_SRAM_LINK_MODE_LEN)-1)<<RF_SRAM_LINK_MODE_POS))
#define RF_SRAM_SWAP                                            RF_SRAM_SWAP
#define RF_SRAM_SWAP_POS                                        (18U)
#define RF_SRAM_SWAP_LEN                                        (1U)
#define RF_SRAM_SWAP_MSK                                        (((1U<<RF_SRAM_SWAP_LEN)-1)<<RF_SRAM_SWAP_POS)
#define RF_SRAM_SWAP_UMSK                                       (~(((1U<<RF_SRAM_SWAP_LEN)-1)<<RF_SRAM_SWAP_POS))
#define RF_SRAM_EXT_CLR                                         RF_SRAM_EXT_CLR
#define RF_SRAM_EXT_CLR_POS                                     (19U)
#define RF_SRAM_EXT_CLR_LEN                                     (1U)
#define RF_SRAM_EXT_CLR_MSK                                     (((1U<<RF_SRAM_EXT_CLR_LEN)-1)<<RF_SRAM_EXT_CLR_POS)
#define RF_SRAM_EXT_CLR_UMSK                                    (~(((1U<<RF_SRAM_EXT_CLR_LEN)-1)<<RF_SRAM_EXT_CLR_POS))

/* 0x240 : rf_sram_ctrl1 */
#define RF_SRAM_CTRL1_OFFSET                                    (0x240)
#define RF_SRAM_ADC_DONE                                        RF_SRAM_ADC_DONE
#define RF_SRAM_ADC_DONE_POS                                    (0U)
#define RF_SRAM_ADC_DONE_LEN                                    (1U)
#define RF_SRAM_ADC_DONE_MSK                                    (((1U<<RF_SRAM_ADC_DONE_LEN)-1)<<RF_SRAM_ADC_DONE_POS)
#define RF_SRAM_ADC_DONE_UMSK                                   (~(((1U<<RF_SRAM_ADC_DONE_LEN)-1)<<RF_SRAM_ADC_DONE_POS))
#define RF_SRAM_ADC_EN                                          RF_SRAM_ADC_EN
#define RF_SRAM_ADC_EN_POS                                      (1U)
#define RF_SRAM_ADC_EN_LEN                                      (1U)
#define RF_SRAM_ADC_EN_MSK                                      (((1U<<RF_SRAM_ADC_EN_LEN)-1)<<RF_SRAM_ADC_EN_POS)
#define RF_SRAM_ADC_EN_UMSK                                     (~(((1U<<RF_SRAM_ADC_EN_LEN)-1)<<RF_SRAM_ADC_EN_POS))
#define RF_SRAM_ADC_LOOP_EN                                     RF_SRAM_ADC_LOOP_EN
#define RF_SRAM_ADC_LOOP_EN_POS                                 (2U)
#define RF_SRAM_ADC_LOOP_EN_LEN                                 (1U)
#define RF_SRAM_ADC_LOOP_EN_MSK                                 (((1U<<RF_SRAM_ADC_LOOP_EN_LEN)-1)<<RF_SRAM_ADC_LOOP_EN_POS)
#define RF_SRAM_ADC_LOOP_EN_UMSK                                (~(((1U<<RF_SRAM_ADC_LOOP_EN_LEN)-1)<<RF_SRAM_ADC_LOOP_EN_POS))
#define RF_SRAM_ADC_STS_CLR                                     RF_SRAM_ADC_STS_CLR
#define RF_SRAM_ADC_STS_CLR_POS                                 (3U)
#define RF_SRAM_ADC_STS_CLR_LEN                                 (1U)
#define RF_SRAM_ADC_STS_CLR_MSK                                 (((1U<<RF_SRAM_ADC_STS_CLR_LEN)-1)<<RF_SRAM_ADC_STS_CLR_POS)
#define RF_SRAM_ADC_STS_CLR_UMSK                                (~(((1U<<RF_SRAM_ADC_STS_CLR_LEN)-1)<<RF_SRAM_ADC_STS_CLR_POS))
#define RF_SRAM_ADC_DONE_CNT                                    RF_SRAM_ADC_DONE_CNT
#define RF_SRAM_ADC_DONE_CNT_POS                                (16U)
#define RF_SRAM_ADC_DONE_CNT_LEN                                (16U)
#define RF_SRAM_ADC_DONE_CNT_MSK                                (((1U<<RF_SRAM_ADC_DONE_CNT_LEN)-1)<<RF_SRAM_ADC_DONE_CNT_POS)
#define RF_SRAM_ADC_DONE_CNT_UMSK                               (~(((1U<<RF_SRAM_ADC_DONE_CNT_LEN)-1)<<RF_SRAM_ADC_DONE_CNT_POS))

/* 0x244 : rf_sram_ctrl2 */
#define RF_SRAM_CTRL2_OFFSET                                    (0x244)
#define RF_SRAM_ADC_ADDR_END                                    RF_SRAM_ADC_ADDR_END
#define RF_SRAM_ADC_ADDR_END_POS                                (0U)
#define RF_SRAM_ADC_ADDR_END_LEN                                (16U)
#define RF_SRAM_ADC_ADDR_END_MSK                                (((1U<<RF_SRAM_ADC_ADDR_END_LEN)-1)<<RF_SRAM_ADC_ADDR_END_POS)
#define RF_SRAM_ADC_ADDR_END_UMSK                               (~(((1U<<RF_SRAM_ADC_ADDR_END_LEN)-1)<<RF_SRAM_ADC_ADDR_END_POS))
#define RF_SRAM_ADC_ADDR_START                                  RF_SRAM_ADC_ADDR_START
#define RF_SRAM_ADC_ADDR_START_POS                              (16U)
#define RF_SRAM_ADC_ADDR_START_LEN                              (16U)
#define RF_SRAM_ADC_ADDR_START_MSK                              (((1U<<RF_SRAM_ADC_ADDR_START_LEN)-1)<<RF_SRAM_ADC_ADDR_START_POS)
#define RF_SRAM_ADC_ADDR_START_UMSK                             (~(((1U<<RF_SRAM_ADC_ADDR_START_LEN)-1)<<RF_SRAM_ADC_ADDR_START_POS))

/* 0x248 : rf_sram_ctrl3 */
#define RF_SRAM_CTRL3_OFFSET                                    (0x248)
#define RF_SRAM_ADC_STS                                         RF_SRAM_ADC_STS
#define RF_SRAM_ADC_STS_POS                                     (0U)
#define RF_SRAM_ADC_STS_LEN                                     (32U)
#define RF_SRAM_ADC_STS_MSK                                     (((1U<<RF_SRAM_ADC_STS_LEN)-1)<<RF_SRAM_ADC_STS_POS)
#define RF_SRAM_ADC_STS_UMSK                                    (~(((1U<<RF_SRAM_ADC_STS_LEN)-1)<<RF_SRAM_ADC_STS_POS))

/* 0x24C : rf_sram_ctrl4 */
#define RF_SRAM_CTRL4_OFFSET                                    (0x24C)
#define RF_SRAM_DAC_DONE                                        RF_SRAM_DAC_DONE
#define RF_SRAM_DAC_DONE_POS                                    (0U)
#define RF_SRAM_DAC_DONE_LEN                                    (1U)
#define RF_SRAM_DAC_DONE_MSK                                    (((1U<<RF_SRAM_DAC_DONE_LEN)-1)<<RF_SRAM_DAC_DONE_POS)
#define RF_SRAM_DAC_DONE_UMSK                                   (~(((1U<<RF_SRAM_DAC_DONE_LEN)-1)<<RF_SRAM_DAC_DONE_POS))
#define RF_SRAM_DAC_EN                                          RF_SRAM_DAC_EN
#define RF_SRAM_DAC_EN_POS                                      (1U)
#define RF_SRAM_DAC_EN_LEN                                      (1U)
#define RF_SRAM_DAC_EN_MSK                                      (((1U<<RF_SRAM_DAC_EN_LEN)-1)<<RF_SRAM_DAC_EN_POS)
#define RF_SRAM_DAC_EN_UMSK                                     (~(((1U<<RF_SRAM_DAC_EN_LEN)-1)<<RF_SRAM_DAC_EN_POS))
#define RF_SRAM_DAC_LOOP_EN                                     RF_SRAM_DAC_LOOP_EN
#define RF_SRAM_DAC_LOOP_EN_POS                                 (2U)
#define RF_SRAM_DAC_LOOP_EN_LEN                                 (1U)
#define RF_SRAM_DAC_LOOP_EN_MSK                                 (((1U<<RF_SRAM_DAC_LOOP_EN_LEN)-1)<<RF_SRAM_DAC_LOOP_EN_POS)
#define RF_SRAM_DAC_LOOP_EN_UMSK                                (~(((1U<<RF_SRAM_DAC_LOOP_EN_LEN)-1)<<RF_SRAM_DAC_LOOP_EN_POS))
#define RF_SRAM_DAC_STS_CLR                                     RF_SRAM_DAC_STS_CLR
#define RF_SRAM_DAC_STS_CLR_POS                                 (3U)
#define RF_SRAM_DAC_STS_CLR_LEN                                 (1U)
#define RF_SRAM_DAC_STS_CLR_MSK                                 (((1U<<RF_SRAM_DAC_STS_CLR_LEN)-1)<<RF_SRAM_DAC_STS_CLR_POS)
#define RF_SRAM_DAC_STS_CLR_UMSK                                (~(((1U<<RF_SRAM_DAC_STS_CLR_LEN)-1)<<RF_SRAM_DAC_STS_CLR_POS))
#define RF_SRAM_DAC_DONE_CNT                                    RF_SRAM_DAC_DONE_CNT
#define RF_SRAM_DAC_DONE_CNT_POS                                (16U)
#define RF_SRAM_DAC_DONE_CNT_LEN                                (16U)
#define RF_SRAM_DAC_DONE_CNT_MSK                                (((1U<<RF_SRAM_DAC_DONE_CNT_LEN)-1)<<RF_SRAM_DAC_DONE_CNT_POS)
#define RF_SRAM_DAC_DONE_CNT_UMSK                               (~(((1U<<RF_SRAM_DAC_DONE_CNT_LEN)-1)<<RF_SRAM_DAC_DONE_CNT_POS))

/* 0x250 : rf_sram_ctrl5 */
#define RF_SRAM_CTRL5_OFFSET                                    (0x250)
#define RF_SRAM_DAC_ADDR_END                                    RF_SRAM_DAC_ADDR_END
#define RF_SRAM_DAC_ADDR_END_POS                                (0U)
#define RF_SRAM_DAC_ADDR_END_LEN                                (16U)
#define RF_SRAM_DAC_ADDR_END_MSK                                (((1U<<RF_SRAM_DAC_ADDR_END_LEN)-1)<<RF_SRAM_DAC_ADDR_END_POS)
#define RF_SRAM_DAC_ADDR_END_UMSK                               (~(((1U<<RF_SRAM_DAC_ADDR_END_LEN)-1)<<RF_SRAM_DAC_ADDR_END_POS))
#define RF_SRAM_DAC_ADDR_START                                  RF_SRAM_DAC_ADDR_START
#define RF_SRAM_DAC_ADDR_START_POS                              (16U)
#define RF_SRAM_DAC_ADDR_START_LEN                              (16U)
#define RF_SRAM_DAC_ADDR_START_MSK                              (((1U<<RF_SRAM_DAC_ADDR_START_LEN)-1)<<RF_SRAM_DAC_ADDR_START_POS)
#define RF_SRAM_DAC_ADDR_START_UMSK                             (~(((1U<<RF_SRAM_DAC_ADDR_START_LEN)-1)<<RF_SRAM_DAC_ADDR_START_POS))

/* 0x254 : rf_sram_ctrl6 */
#define RF_SRAM_CTRL6_OFFSET                                    (0x254)
#define RF_SRAM_DAC_STS                                         RF_SRAM_DAC_STS
#define RF_SRAM_DAC_STS_POS                                     (0U)
#define RF_SRAM_DAC_STS_LEN                                     (32U)
#define RF_SRAM_DAC_STS_MSK                                     (((1U<<RF_SRAM_DAC_STS_LEN)-1)<<RF_SRAM_DAC_STS_POS)
#define RF_SRAM_DAC_STS_UMSK                                    (~(((1U<<RF_SRAM_DAC_STS_LEN)-1)<<RF_SRAM_DAC_STS_POS))

/* 0x258 : rf_ical_ctrl0 */
#define RF_ICAL_CTRL0_OFFSET                                    (0x258)
#define RF_ICAL_R_CNT_N                                         RF_ICAL_R_CNT_N
#define RF_ICAL_R_CNT_N_POS                                     (0U)
#define RF_ICAL_R_CNT_N_LEN                                     (10U)
#define RF_ICAL_R_CNT_N_MSK                                     (((1U<<RF_ICAL_R_CNT_N_LEN)-1)<<RF_ICAL_R_CNT_N_POS)
#define RF_ICAL_R_CNT_N_UMSK                                    (~(((1U<<RF_ICAL_R_CNT_N_LEN)-1)<<RF_ICAL_R_CNT_N_POS))
#define RF_ICAL_A_CNT_N                                         RF_ICAL_A_CNT_N
#define RF_ICAL_A_CNT_N_POS                                     (10U)
#define RF_ICAL_A_CNT_N_LEN                                     (10U)
#define RF_ICAL_A_CNT_N_MSK                                     (((1U<<RF_ICAL_A_CNT_N_LEN)-1)<<RF_ICAL_A_CNT_N_POS)
#define RF_ICAL_A_CNT_N_UMSK                                    (~(((1U<<RF_ICAL_A_CNT_N_LEN)-1)<<RF_ICAL_A_CNT_N_POS))
#define RF_ICAL_F_CNT_N                                         RF_ICAL_F_CNT_N
#define RF_ICAL_F_CNT_N_POS                                     (20U)
#define RF_ICAL_F_CNT_N_LEN                                     (10U)
#define RF_ICAL_F_CNT_N_MSK                                     (((1U<<RF_ICAL_F_CNT_N_LEN)-1)<<RF_ICAL_F_CNT_N_POS)
#define RF_ICAL_F_CNT_N_UMSK                                    (~(((1U<<RF_ICAL_F_CNT_N_LEN)-1)<<RF_ICAL_F_CNT_N_POS))
#define RF_ICAL_A_UD_INV_EN                                     RF_ICAL_A_UD_INV_EN
#define RF_ICAL_A_UD_INV_EN_POS                                 (30U)
#define RF_ICAL_A_UD_INV_EN_LEN                                 (1U)
#define RF_ICAL_A_UD_INV_EN_MSK                                 (((1U<<RF_ICAL_A_UD_INV_EN_LEN)-1)<<RF_ICAL_A_UD_INV_EN_POS)
#define RF_ICAL_A_UD_INV_EN_UMSK                                (~(((1U<<RF_ICAL_A_UD_INV_EN_LEN)-1)<<RF_ICAL_A_UD_INV_EN_POS))
#define RF_ICAL_F_UD_INV_EN                                     RF_ICAL_F_UD_INV_EN
#define RF_ICAL_F_UD_INV_EN_POS                                 (31U)
#define RF_ICAL_F_UD_INV_EN_LEN                                 (1U)
#define RF_ICAL_F_UD_INV_EN_MSK                                 (((1U<<RF_ICAL_F_UD_INV_EN_LEN)-1)<<RF_ICAL_F_UD_INV_EN_POS)
#define RF_ICAL_F_UD_INV_EN_UMSK                                (~(((1U<<RF_ICAL_F_UD_INV_EN_LEN)-1)<<RF_ICAL_F_UD_INV_EN_POS))

/* 0x25C : rf_ical_ctrl1 */
#define RF_ICAL_CTRL1_OFFSET                                    (0x25C)
#define RF_ICAL_R_AVG_N                                         RF_ICAL_R_AVG_N
#define RF_ICAL_R_AVG_N_POS                                     (0U)
#define RF_ICAL_R_AVG_N_LEN                                     (5U)
#define RF_ICAL_R_AVG_N_MSK                                     (((1U<<RF_ICAL_R_AVG_N_LEN)-1)<<RF_ICAL_R_AVG_N_POS)
#define RF_ICAL_R_AVG_N_UMSK                                    (~(((1U<<RF_ICAL_R_AVG_N_LEN)-1)<<RF_ICAL_R_AVG_N_POS))
#define RF_ICAL_R_OS_Q                                          RF_ICAL_R_OS_Q
#define RF_ICAL_R_OS_Q_POS                                      (10U)
#define RF_ICAL_R_OS_Q_LEN                                      (10U)
#define RF_ICAL_R_OS_Q_MSK                                      (((1U<<RF_ICAL_R_OS_Q_LEN)-1)<<RF_ICAL_R_OS_Q_POS)
#define RF_ICAL_R_OS_Q_UMSK                                     (~(((1U<<RF_ICAL_R_OS_Q_LEN)-1)<<RF_ICAL_R_OS_Q_POS))
#define RF_ICAL_R_OS_I                                          RF_ICAL_R_OS_I
#define RF_ICAL_R_OS_I_POS                                      (20U)
#define RF_ICAL_R_OS_I_LEN                                      (10U)
#define RF_ICAL_R_OS_I_MSK                                      (((1U<<RF_ICAL_R_OS_I_LEN)-1)<<RF_ICAL_R_OS_I_POS)
#define RF_ICAL_R_OS_I_UMSK                                     (~(((1U<<RF_ICAL_R_OS_I_LEN)-1)<<RF_ICAL_R_OS_I_POS))

/* 0x260 : rf_ical_ctrl2 */
#define RF_ICAL_CTRL2_OFFSET                                    (0x260)
#define RF_ICAL_PERIOD_N                                        RF_ICAL_PERIOD_N
#define RF_ICAL_PERIOD_N_POS                                    (0U)
#define RF_ICAL_PERIOD_N_LEN                                    (16U)
#define RF_ICAL_PERIOD_N_MSK                                    (((1U<<RF_ICAL_PERIOD_N_LEN)-1)<<RF_ICAL_PERIOD_N_POS)
#define RF_ICAL_PERIOD_N_UMSK                                   (~(((1U<<RF_ICAL_PERIOD_N_LEN)-1)<<RF_ICAL_PERIOD_N_POS))

/* 0x264 : rf_fsm_ctrl0 */
#define RF_FSM_CTRL0_OFFSET                                     (0x264)
#define RF_CH_IND_WIFI                                          RF_CH_IND_WIFI
#define RF_CH_IND_WIFI_POS                                      (0U)
#define RF_CH_IND_WIFI_LEN                                      (12U)
#define RF_CH_IND_WIFI_MSK                                      (((1U<<RF_CH_IND_WIFI_LEN)-1)<<RF_CH_IND_WIFI_POS)
#define RF_CH_IND_WIFI_UMSK                                     (~(((1U<<RF_CH_IND_WIFI_LEN)-1)<<RF_CH_IND_WIFI_POS))

/* 0x268 : rf_fsm_ctrl1 */
#define RF_FSM_CTRL1_OFFSET                                     (0x268)
#define RF_FSM_LO_TIME                                          RF_FSM_LO_TIME
#define RF_FSM_LO_TIME_POS                                      (0U)
#define RF_FSM_LO_TIME_LEN                                      (16U)
#define RF_FSM_LO_TIME_MSK                                      (((1U<<RF_FSM_LO_TIME_LEN)-1)<<RF_FSM_LO_TIME_POS)
#define RF_FSM_LO_TIME_UMSK                                     (~(((1U<<RF_FSM_LO_TIME_LEN)-1)<<RF_FSM_LO_TIME_POS))
#define RF_FSM_LO_RDY                                           RF_FSM_LO_RDY
#define RF_FSM_LO_RDY_POS                                       (16U)
#define RF_FSM_LO_RDY_LEN                                       (1U)
#define RF_FSM_LO_RDY_MSK                                       (((1U<<RF_FSM_LO_RDY_LEN)-1)<<RF_FSM_LO_RDY_POS)
#define RF_FSM_LO_RDY_UMSK                                      (~(((1U<<RF_FSM_LO_RDY_LEN)-1)<<RF_FSM_LO_RDY_POS))
#define RF_FSM_LO_RDY_RST                                       RF_FSM_LO_RDY_RST
#define RF_FSM_LO_RDY_RST_POS                                   (17U)
#define RF_FSM_LO_RDY_RST_LEN                                   (1U)
#define RF_FSM_LO_RDY_RST_MSK                                   (((1U<<RF_FSM_LO_RDY_RST_LEN)-1)<<RF_FSM_LO_RDY_RST_POS)
#define RF_FSM_LO_RDY_RST_UMSK                                  (~(((1U<<RF_FSM_LO_RDY_RST_LEN)-1)<<RF_FSM_LO_RDY_RST_POS))
#define RF_FSM_LO_RDY_4S_1                                      RF_FSM_LO_RDY_4S_1
#define RF_FSM_LO_RDY_4S_1_POS                                  (18U)
#define RF_FSM_LO_RDY_4S_1_LEN                                  (1U)
#define RF_FSM_LO_RDY_4S_1_MSK                                  (((1U<<RF_FSM_LO_RDY_4S_1_LEN)-1)<<RF_FSM_LO_RDY_4S_1_POS)
#define RF_FSM_LO_RDY_4S_1_UMSK                                 (~(((1U<<RF_FSM_LO_RDY_4S_1_LEN)-1)<<RF_FSM_LO_RDY_4S_1_POS))
#define RF_FSM_LO_RDY_SBCLR                                     RF_FSM_LO_RDY_SBCLR
#define RF_FSM_LO_RDY_SBCLR_POS                                 (19U)
#define RF_FSM_LO_RDY_SBCLR_LEN                                 (1U)
#define RF_FSM_LO_RDY_SBCLR_MSK                                 (((1U<<RF_FSM_LO_RDY_SBCLR_LEN)-1)<<RF_FSM_LO_RDY_SBCLR_POS)
#define RF_FSM_LO_RDY_SBCLR_UMSK                                (~(((1U<<RF_FSM_LO_RDY_SBCLR_LEN)-1)<<RF_FSM_LO_RDY_SBCLR_POS))
#define RF_FSM_PU_PA_DLY_N                                      RF_FSM_PU_PA_DLY_N
#define RF_FSM_PU_PA_DLY_N_POS                                  (20U)
#define RF_FSM_PU_PA_DLY_N_LEN                                  (10U)
#define RF_FSM_PU_PA_DLY_N_MSK                                  (((1U<<RF_FSM_PU_PA_DLY_N_LEN)-1)<<RF_FSM_PU_PA_DLY_N_POS)
#define RF_FSM_PU_PA_DLY_N_UMSK                                 (~(((1U<<RF_FSM_PU_PA_DLY_N_LEN)-1)<<RF_FSM_PU_PA_DLY_N_POS))

/* 0x26C : rf_fsm_ctrl2 */
#define RF_FSM_CTRL2_OFFSET                                     (0x26C)
#define RF_FSM_ST_DBG                                           RF_FSM_ST_DBG
#define RF_FSM_ST_DBG_POS                                       (0U)
#define RF_FSM_ST_DBG_LEN                                       (3U)
#define RF_FSM_ST_DBG_MSK                                       (((1U<<RF_FSM_ST_DBG_LEN)-1)<<RF_FSM_ST_DBG_POS)
#define RF_FSM_ST_DBG_UMSK                                      (~(((1U<<RF_FSM_ST_DBG_LEN)-1)<<RF_FSM_ST_DBG_POS))
#define RF_FSM_ST_DBG_EN                                        RF_FSM_ST_DBG_EN
#define RF_FSM_ST_DBG_EN_POS                                    (3U)
#define RF_FSM_ST_DBG_EN_LEN                                    (1U)
#define RF_FSM_ST_DBG_EN_MSK                                    (((1U<<RF_FSM_ST_DBG_EN_LEN)-1)<<RF_FSM_ST_DBG_EN_POS)
#define RF_FSM_ST_DBG_EN_UMSK                                   (~(((1U<<RF_FSM_ST_DBG_EN_LEN)-1)<<RF_FSM_ST_DBG_EN_POS))
#define RF_TRX_EN_BLE_4S                                        RF_TRX_EN_BLE_4S
#define RF_TRX_EN_BLE_4S_POS                                    (4U)
#define RF_TRX_EN_BLE_4S_LEN                                    (1U)
#define RF_TRX_EN_BLE_4S_MSK                                    (((1U<<RF_TRX_EN_BLE_4S_LEN)-1)<<RF_TRX_EN_BLE_4S_POS)
#define RF_TRX_EN_BLE_4S_UMSK                                   (~(((1U<<RF_TRX_EN_BLE_4S_LEN)-1)<<RF_TRX_EN_BLE_4S_POS))
#define RF_TRX_SW_BLE_4S                                        RF_TRX_SW_BLE_4S
#define RF_TRX_SW_BLE_4S_POS                                    (5U)
#define RF_TRX_SW_BLE_4S_LEN                                    (1U)
#define RF_TRX_SW_BLE_4S_MSK                                    (((1U<<RF_TRX_SW_BLE_4S_LEN)-1)<<RF_TRX_SW_BLE_4S_POS)
#define RF_TRX_SW_BLE_4S_UMSK                                   (~(((1U<<RF_TRX_SW_BLE_4S_LEN)-1)<<RF_TRX_SW_BLE_4S_POS))
#define RF_TRX_BLE_4S_EN                                        RF_TRX_BLE_4S_EN
#define RF_TRX_BLE_4S_EN_POS                                    (6U)
#define RF_TRX_BLE_4S_EN_LEN                                    (1U)
#define RF_TRX_BLE_4S_EN_MSK                                    (((1U<<RF_TRX_BLE_4S_EN_LEN)-1)<<RF_TRX_BLE_4S_EN_POS)
#define RF_TRX_BLE_4S_EN_UMSK                                   (~(((1U<<RF_TRX_BLE_4S_EN_LEN)-1)<<RF_TRX_BLE_4S_EN_POS))
#define RF_FSM_DFE_TX_DLY_N                                     RF_FSM_DFE_TX_DLY_N
#define RF_FSM_DFE_TX_DLY_N_POS                                 (10U)
#define RF_FSM_DFE_TX_DLY_N_LEN                                 (10U)
#define RF_FSM_DFE_TX_DLY_N_MSK                                 (((1U<<RF_FSM_DFE_TX_DLY_N_LEN)-1)<<RF_FSM_DFE_TX_DLY_N_POS)
#define RF_FSM_DFE_TX_DLY_N_UMSK                                (~(((1U<<RF_FSM_DFE_TX_DLY_N_LEN)-1)<<RF_FSM_DFE_TX_DLY_N_POS))
#define RF_FSM_DFE_RX_DLY_N                                     RF_FSM_DFE_RX_DLY_N
#define RF_FSM_DFE_RX_DLY_N_POS                                 (20U)
#define RF_FSM_DFE_RX_DLY_N_LEN                                 (10U)
#define RF_FSM_DFE_RX_DLY_N_MSK                                 (((1U<<RF_FSM_DFE_RX_DLY_N_LEN)-1)<<RF_FSM_DFE_RX_DLY_N_POS)
#define RF_FSM_DFE_RX_DLY_N_UMSK                                (~(((1U<<RF_FSM_DFE_RX_DLY_N_LEN)-1)<<RF_FSM_DFE_RX_DLY_N_POS))

/* 0x270 : rf_pkdet_ctrl0 */
#define RF_PKDET_CTRL0_OFFSET                                   (0x270)
#define RF_PKDET_OUT_CNT_STS                                    RF_PKDET_OUT_CNT_STS
#define RF_PKDET_OUT_CNT_STS_POS                                (0U)
#define RF_PKDET_OUT_CNT_STS_LEN                                (4U)
#define RF_PKDET_OUT_CNT_STS_MSK                                (((1U<<RF_PKDET_OUT_CNT_STS_LEN)-1)<<RF_PKDET_OUT_CNT_STS_POS)
#define RF_PKDET_OUT_CNT_STS_UMSK                               (~(((1U<<RF_PKDET_OUT_CNT_STS_LEN)-1)<<RF_PKDET_OUT_CNT_STS_POS))
#define RF_PKDET_OUT_CNT_EN                                     RF_PKDET_OUT_CNT_EN
#define RF_PKDET_OUT_CNT_EN_POS                                 (4U)
#define RF_PKDET_OUT_CNT_EN_LEN                                 (1U)
#define RF_PKDET_OUT_CNT_EN_MSK                                 (((1U<<RF_PKDET_OUT_CNT_EN_LEN)-1)<<RF_PKDET_OUT_CNT_EN_POS)
#define RF_PKDET_OUT_CNT_EN_UMSK                                (~(((1U<<RF_PKDET_OUT_CNT_EN_LEN)-1)<<RF_PKDET_OUT_CNT_EN_POS))
#define RF_PKDET_OUT_MODE                                       RF_PKDET_OUT_MODE
#define RF_PKDET_OUT_MODE_POS                                   (5U)
#define RF_PKDET_OUT_MODE_LEN                                   (1U)
#define RF_PKDET_OUT_MODE_MSK                                   (((1U<<RF_PKDET_OUT_MODE_LEN)-1)<<RF_PKDET_OUT_MODE_POS)
#define RF_PKDET_OUT_MODE_UMSK                                  (~(((1U<<RF_PKDET_OUT_MODE_LEN)-1)<<RF_PKDET_OUT_MODE_POS))

/* 0x600 : dfe_ctrl_0 */
#define RF_DFE_CTRL_0_OFFSET                                    (0x600)
#define RF_TX_IQC_PHASE                                         RF_TX_IQC_PHASE
#define RF_TX_IQC_PHASE_POS                                     (0U)
#define RF_TX_IQC_PHASE_LEN                                     (10U)
#define RF_TX_IQC_PHASE_MSK                                     (((1U<<RF_TX_IQC_PHASE_LEN)-1)<<RF_TX_IQC_PHASE_POS)
#define RF_TX_IQC_PHASE_UMSK                                    (~(((1U<<RF_TX_IQC_PHASE_LEN)-1)<<RF_TX_IQC_PHASE_POS))
#define RF_TX_IQC_PHASE_EN                                      RF_TX_IQC_PHASE_EN
#define RF_TX_IQC_PHASE_EN_POS                                  (10U)
#define RF_TX_IQC_PHASE_EN_LEN                                  (1U)
#define RF_TX_IQC_PHASE_EN_MSK                                  (((1U<<RF_TX_IQC_PHASE_EN_LEN)-1)<<RF_TX_IQC_PHASE_EN_POS)
#define RF_TX_IQC_PHASE_EN_UMSK                                 (~(((1U<<RF_TX_IQC_PHASE_EN_LEN)-1)<<RF_TX_IQC_PHASE_EN_POS))
#define RF_TX_IQC_GAIN                                          RF_TX_IQC_GAIN
#define RF_TX_IQC_GAIN_POS                                      (12U)
#define RF_TX_IQC_GAIN_LEN                                      (11U)
#define RF_TX_IQC_GAIN_MSK                                      (((1U<<RF_TX_IQC_GAIN_LEN)-1)<<RF_TX_IQC_GAIN_POS)
#define RF_TX_IQC_GAIN_UMSK                                     (~(((1U<<RF_TX_IQC_GAIN_LEN)-1)<<RF_TX_IQC_GAIN_POS))
#define RF_TX_IQC_GAIN_EN                                       RF_TX_IQC_GAIN_EN
#define RF_TX_IQC_GAIN_EN_POS                                   (23U)
#define RF_TX_IQC_GAIN_EN_LEN                                   (1U)
#define RF_TX_IQC_GAIN_EN_MSK                                   (((1U<<RF_TX_IQC_GAIN_EN_LEN)-1)<<RF_TX_IQC_GAIN_EN_POS)
#define RF_TX_IQC_GAIN_EN_UMSK                                  (~(((1U<<RF_TX_IQC_GAIN_EN_LEN)-1)<<RF_TX_IQC_GAIN_EN_POS))
#define RF_TX_DVGA_GAIN_QDB                                     RF_TX_DVGA_GAIN_QDB
#define RF_TX_DVGA_GAIN_QDB_POS                                 (24U)
#define RF_TX_DVGA_GAIN_QDB_LEN                                 (7U)
#define RF_TX_DVGA_GAIN_QDB_MSK                                 (((1U<<RF_TX_DVGA_GAIN_QDB_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_POS)
#define RF_TX_DVGA_GAIN_QDB_UMSK                                (~(((1U<<RF_TX_DVGA_GAIN_QDB_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_POS))
#define RF_TX_DVGA_GAIN_CTRL_HW                                 RF_TX_DVGA_GAIN_CTRL_HW
#define RF_TX_DVGA_GAIN_CTRL_HW_POS                             (31U)
#define RF_TX_DVGA_GAIN_CTRL_HW_LEN                             (1U)
#define RF_TX_DVGA_GAIN_CTRL_HW_MSK                             (((1U<<RF_TX_DVGA_GAIN_CTRL_HW_LEN)-1)<<RF_TX_DVGA_GAIN_CTRL_HW_POS)
#define RF_TX_DVGA_GAIN_CTRL_HW_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_CTRL_HW_LEN)-1)<<RF_TX_DVGA_GAIN_CTRL_HW_POS))

/* 0x604 : dfe_ctrl_1 */
#define RF_DFE_CTRL_1_OFFSET                                    (0x604)
#define RF_TX_DAC_OS_I                                          RF_TX_DAC_OS_I
#define RF_TX_DAC_OS_I_POS                                      (0U)
#define RF_TX_DAC_OS_I_LEN                                      (12U)
#define RF_TX_DAC_OS_I_MSK                                      (((1U<<RF_TX_DAC_OS_I_LEN)-1)<<RF_TX_DAC_OS_I_POS)
#define RF_TX_DAC_OS_I_UMSK                                     (~(((1U<<RF_TX_DAC_OS_I_LEN)-1)<<RF_TX_DAC_OS_I_POS))
#define RF_TX_DAC_OS_Q                                          RF_TX_DAC_OS_Q
#define RF_TX_DAC_OS_Q_POS                                      (16U)
#define RF_TX_DAC_OS_Q_LEN                                      (12U)
#define RF_TX_DAC_OS_Q_MSK                                      (((1U<<RF_TX_DAC_OS_Q_LEN)-1)<<RF_TX_DAC_OS_Q_POS)
#define RF_TX_DAC_OS_Q_UMSK                                     (~(((1U<<RF_TX_DAC_OS_Q_LEN)-1)<<RF_TX_DAC_OS_Q_POS))
#define RF_TX_DAC_DAT_FORMAT                                    RF_TX_DAC_DAT_FORMAT
#define RF_TX_DAC_DAT_FORMAT_POS                                (30U)
#define RF_TX_DAC_DAT_FORMAT_LEN                                (1U)
#define RF_TX_DAC_DAT_FORMAT_MSK                                (((1U<<RF_TX_DAC_DAT_FORMAT_LEN)-1)<<RF_TX_DAC_DAT_FORMAT_POS)
#define RF_TX_DAC_DAT_FORMAT_UMSK                               (~(((1U<<RF_TX_DAC_DAT_FORMAT_LEN)-1)<<RF_TX_DAC_DAT_FORMAT_POS))
#define RF_TX_DAC_IQ_SWAP                                       RF_TX_DAC_IQ_SWAP
#define RF_TX_DAC_IQ_SWAP_POS                                   (31U)
#define RF_TX_DAC_IQ_SWAP_LEN                                   (1U)
#define RF_TX_DAC_IQ_SWAP_MSK                                   (((1U<<RF_TX_DAC_IQ_SWAP_LEN)-1)<<RF_TX_DAC_IQ_SWAP_POS)
#define RF_TX_DAC_IQ_SWAP_UMSK                                  (~(((1U<<RF_TX_DAC_IQ_SWAP_LEN)-1)<<RF_TX_DAC_IQ_SWAP_POS))

/* 0x608 : dfe_ctrl_2 */
#define RF_DFE_CTRL_2_OFFSET                                    (0x608)
#define RF_RX_ADC_OS_I                                          RF_RX_ADC_OS_I
#define RF_RX_ADC_OS_I_POS                                      (0U)
#define RF_RX_ADC_OS_I_LEN                                      (10U)
#define RF_RX_ADC_OS_I_MSK                                      (((1U<<RF_RX_ADC_OS_I_LEN)-1)<<RF_RX_ADC_OS_I_POS)
#define RF_RX_ADC_OS_I_UMSK                                     (~(((1U<<RF_RX_ADC_OS_I_LEN)-1)<<RF_RX_ADC_OS_I_POS))
#define RF_RX_ADC_OS_Q                                          RF_RX_ADC_OS_Q
#define RF_RX_ADC_OS_Q_POS                                      (16U)
#define RF_RX_ADC_OS_Q_LEN                                      (10U)
#define RF_RX_ADC_OS_Q_MSK                                      (((1U<<RF_RX_ADC_OS_Q_LEN)-1)<<RF_RX_ADC_OS_Q_POS)
#define RF_RX_ADC_OS_Q_UMSK                                     (~(((1U<<RF_RX_ADC_OS_Q_LEN)-1)<<RF_RX_ADC_OS_Q_POS))
#define RF_RX_ADC_DCE_FLT_EN                                    RF_RX_ADC_DCE_FLT_EN
#define RF_RX_ADC_DCE_FLT_EN_POS                                (28U)
#define RF_RX_ADC_DCE_FLT_EN_LEN                                (1U)
#define RF_RX_ADC_DCE_FLT_EN_MSK                                (((1U<<RF_RX_ADC_DCE_FLT_EN_LEN)-1)<<RF_RX_ADC_DCE_FLT_EN_POS)
#define RF_RX_ADC_DCE_FLT_EN_UMSK                               (~(((1U<<RF_RX_ADC_DCE_FLT_EN_LEN)-1)<<RF_RX_ADC_DCE_FLT_EN_POS))
#define RF_RX_ADC_LOW_POW_EN                                    RF_RX_ADC_LOW_POW_EN
#define RF_RX_ADC_LOW_POW_EN_POS                                (29U)
#define RF_RX_ADC_LOW_POW_EN_LEN                                (1U)
#define RF_RX_ADC_LOW_POW_EN_MSK                                (((1U<<RF_RX_ADC_LOW_POW_EN_LEN)-1)<<RF_RX_ADC_LOW_POW_EN_POS)
#define RF_RX_ADC_LOW_POW_EN_UMSK                               (~(((1U<<RF_RX_ADC_LOW_POW_EN_LEN)-1)<<RF_RX_ADC_LOW_POW_EN_POS))
#define RF_RX_ADC_DAT_FORMAT                                    RF_RX_ADC_DAT_FORMAT
#define RF_RX_ADC_DAT_FORMAT_POS                                (30U)
#define RF_RX_ADC_DAT_FORMAT_LEN                                (1U)
#define RF_RX_ADC_DAT_FORMAT_MSK                                (((1U<<RF_RX_ADC_DAT_FORMAT_LEN)-1)<<RF_RX_ADC_DAT_FORMAT_POS)
#define RF_RX_ADC_DAT_FORMAT_UMSK                               (~(((1U<<RF_RX_ADC_DAT_FORMAT_LEN)-1)<<RF_RX_ADC_DAT_FORMAT_POS))
#define RF_RX_ADC_IQ_SWAP                                       RF_RX_ADC_IQ_SWAP
#define RF_RX_ADC_IQ_SWAP_POS                                   (31U)
#define RF_RX_ADC_IQ_SWAP_LEN                                   (1U)
#define RF_RX_ADC_IQ_SWAP_MSK                                   (((1U<<RF_RX_ADC_IQ_SWAP_LEN)-1)<<RF_RX_ADC_IQ_SWAP_POS)
#define RF_RX_ADC_IQ_SWAP_UMSK                                  (~(((1U<<RF_RX_ADC_IQ_SWAP_LEN)-1)<<RF_RX_ADC_IQ_SWAP_POS))

/* 0x60C : dfe_ctrl_3 */
#define RF_DFE_CTRL_3_OFFSET                                    (0x60C)
#define RF_RX_ADC_4S_I_VAL                                      RF_RX_ADC_4S_I_VAL
#define RF_RX_ADC_4S_I_VAL_POS                                  (0U)
#define RF_RX_ADC_4S_I_VAL_LEN                                  (10U)
#define RF_RX_ADC_4S_I_VAL_MSK                                  (((1U<<RF_RX_ADC_4S_I_VAL_LEN)-1)<<RF_RX_ADC_4S_I_VAL_POS)
#define RF_RX_ADC_4S_I_VAL_UMSK                                 (~(((1U<<RF_RX_ADC_4S_I_VAL_LEN)-1)<<RF_RX_ADC_4S_I_VAL_POS))
#define RF_RX_ADC_4S_I_EN                                       RF_RX_ADC_4S_I_EN
#define RF_RX_ADC_4S_I_EN_POS                                   (10U)
#define RF_RX_ADC_4S_I_EN_LEN                                   (1U)
#define RF_RX_ADC_4S_I_EN_MSK                                   (((1U<<RF_RX_ADC_4S_I_EN_LEN)-1)<<RF_RX_ADC_4S_I_EN_POS)
#define RF_RX_ADC_4S_I_EN_UMSK                                  (~(((1U<<RF_RX_ADC_4S_I_EN_LEN)-1)<<RF_RX_ADC_4S_I_EN_POS))
#define RF_RX_ADC_4S_Q_VAL                                      RF_RX_ADC_4S_Q_VAL
#define RF_RX_ADC_4S_Q_VAL_POS                                  (16U)
#define RF_RX_ADC_4S_Q_VAL_LEN                                  (10U)
#define RF_RX_ADC_4S_Q_VAL_MSK                                  (((1U<<RF_RX_ADC_4S_Q_VAL_LEN)-1)<<RF_RX_ADC_4S_Q_VAL_POS)
#define RF_RX_ADC_4S_Q_VAL_UMSK                                 (~(((1U<<RF_RX_ADC_4S_Q_VAL_LEN)-1)<<RF_RX_ADC_4S_Q_VAL_POS))
#define RF_RX_ADC_4S_Q_EN                                       RF_RX_ADC_4S_Q_EN
#define RF_RX_ADC_4S_Q_EN_POS                                   (26U)
#define RF_RX_ADC_4S_Q_EN_LEN                                   (1U)
#define RF_RX_ADC_4S_Q_EN_MSK                                   (((1U<<RF_RX_ADC_4S_Q_EN_LEN)-1)<<RF_RX_ADC_4S_Q_EN_POS)
#define RF_RX_ADC_4S_Q_EN_UMSK                                  (~(((1U<<RF_RX_ADC_4S_Q_EN_LEN)-1)<<RF_RX_ADC_4S_Q_EN_POS))

/* 0x610 : dfe_ctrl_4 */
#define RF_DFE_CTRL_4_OFFSET                                    (0x610)
#define RF_RX_PF_TH2                                            RF_RX_PF_TH2
#define RF_RX_PF_TH2_POS                                        (0U)
#define RF_RX_PF_TH2_LEN                                        (10U)
#define RF_RX_PF_TH2_MSK                                        (((1U<<RF_RX_PF_TH2_LEN)-1)<<RF_RX_PF_TH2_POS)
#define RF_RX_PF_TH2_UMSK                                       (~(((1U<<RF_RX_PF_TH2_LEN)-1)<<RF_RX_PF_TH2_POS))
#define RF_RX_PF_TH1                                            RF_RX_PF_TH1
#define RF_RX_PF_TH1_POS                                        (16U)
#define RF_RX_PF_TH1_LEN                                        (10U)
#define RF_RX_PF_TH1_MSK                                        (((1U<<RF_RX_PF_TH1_LEN)-1)<<RF_RX_PF_TH1_POS)
#define RF_RX_PF_TH1_UMSK                                       (~(((1U<<RF_RX_PF_TH1_LEN)-1)<<RF_RX_PF_TH1_POS))
#define RF_RX_PF_Q_EN                                           RF_RX_PF_Q_EN
#define RF_RX_PF_Q_EN_POS                                       (30U)
#define RF_RX_PF_Q_EN_LEN                                       (1U)
#define RF_RX_PF_Q_EN_MSK                                       (((1U<<RF_RX_PF_Q_EN_LEN)-1)<<RF_RX_PF_Q_EN_POS)
#define RF_RX_PF_Q_EN_UMSK                                      (~(((1U<<RF_RX_PF_Q_EN_LEN)-1)<<RF_RX_PF_Q_EN_POS))
#define RF_RX_PF_I_EN                                           RF_RX_PF_I_EN
#define RF_RX_PF_I_EN_POS                                       (31U)
#define RF_RX_PF_I_EN_LEN                                       (1U)
#define RF_RX_PF_I_EN_MSK                                       (((1U<<RF_RX_PF_I_EN_LEN)-1)<<RF_RX_PF_I_EN_POS)
#define RF_RX_PF_I_EN_UMSK                                      (~(((1U<<RF_RX_PF_I_EN_LEN)-1)<<RF_RX_PF_I_EN_POS))

/* 0x614 : dfe_ctrl_5 */
#define RF_DFE_CTRL_5_OFFSET                                    (0x614)
#define RF_RX_IQC_PHASE                                         RF_RX_IQC_PHASE
#define RF_RX_IQC_PHASE_POS                                     (0U)
#define RF_RX_IQC_PHASE_LEN                                     (10U)
#define RF_RX_IQC_PHASE_MSK                                     (((1U<<RF_RX_IQC_PHASE_LEN)-1)<<RF_RX_IQC_PHASE_POS)
#define RF_RX_IQC_PHASE_UMSK                                    (~(((1U<<RF_RX_IQC_PHASE_LEN)-1)<<RF_RX_IQC_PHASE_POS))
#define RF_RX_IQC_PHASE_EN                                      RF_RX_IQC_PHASE_EN
#define RF_RX_IQC_PHASE_EN_POS                                  (10U)
#define RF_RX_IQC_PHASE_EN_LEN                                  (1U)
#define RF_RX_IQC_PHASE_EN_MSK                                  (((1U<<RF_RX_IQC_PHASE_EN_LEN)-1)<<RF_RX_IQC_PHASE_EN_POS)
#define RF_RX_IQC_PHASE_EN_UMSK                                 (~(((1U<<RF_RX_IQC_PHASE_EN_LEN)-1)<<RF_RX_IQC_PHASE_EN_POS))
#define RF_RX_IQC_GAIN                                          RF_RX_IQC_GAIN
#define RF_RX_IQC_GAIN_POS                                      (12U)
#define RF_RX_IQC_GAIN_LEN                                      (11U)
#define RF_RX_IQC_GAIN_MSK                                      (((1U<<RF_RX_IQC_GAIN_LEN)-1)<<RF_RX_IQC_GAIN_POS)
#define RF_RX_IQC_GAIN_UMSK                                     (~(((1U<<RF_RX_IQC_GAIN_LEN)-1)<<RF_RX_IQC_GAIN_POS))
#define RF_RX_IQC_GAIN_EN                                       RF_RX_IQC_GAIN_EN
#define RF_RX_IQC_GAIN_EN_POS                                   (23U)
#define RF_RX_IQC_GAIN_EN_LEN                                   (1U)
#define RF_RX_IQC_GAIN_EN_MSK                                   (((1U<<RF_RX_IQC_GAIN_EN_LEN)-1)<<RF_RX_IQC_GAIN_EN_POS)
#define RF_RX_IQC_GAIN_EN_UMSK                                  (~(((1U<<RF_RX_IQC_GAIN_EN_LEN)-1)<<RF_RX_IQC_GAIN_EN_POS))

/* 0x618 : dfe_ctrl_6 */
#define RF_DFE_CTRL_6_OFFSET                                    (0x618)
#define RF_RX_PM_FREQSHIFT_CW                                   RF_RX_PM_FREQSHIFT_CW
#define RF_RX_PM_FREQSHIFT_CW_POS                               (0U)
#define RF_RX_PM_FREQSHIFT_CW_LEN                               (20U)
#define RF_RX_PM_FREQSHIFT_CW_MSK                               (((1U<<RF_RX_PM_FREQSHIFT_CW_LEN)-1)<<RF_RX_PM_FREQSHIFT_CW_POS)
#define RF_RX_PM_FREQSHIFT_CW_UMSK                              (~(((1U<<RF_RX_PM_FREQSHIFT_CW_LEN)-1)<<RF_RX_PM_FREQSHIFT_CW_POS))
#define RF_RX_PM_FREQSHIFT_EN                                   RF_RX_PM_FREQSHIFT_EN
#define RF_RX_PM_FREQSHIFT_EN_POS                               (20U)
#define RF_RX_PM_FREQSHIFT_EN_LEN                               (1U)
#define RF_RX_PM_FREQSHIFT_EN_MSK                               (((1U<<RF_RX_PM_FREQSHIFT_EN_LEN)-1)<<RF_RX_PM_FREQSHIFT_EN_POS)
#define RF_RX_PM_FREQSHIFT_EN_UMSK                              (~(((1U<<RF_RX_PM_FREQSHIFT_EN_LEN)-1)<<RF_RX_PM_FREQSHIFT_EN_POS))
#define RF_RX_PM_DONE                                           RF_RX_PM_DONE
#define RF_RX_PM_DONE_POS                                       (28U)
#define RF_RX_PM_DONE_LEN                                       (1U)
#define RF_RX_PM_DONE_MSK                                       (((1U<<RF_RX_PM_DONE_LEN)-1)<<RF_RX_PM_DONE_POS)
#define RF_RX_PM_DONE_UMSK                                      (~(((1U<<RF_RX_PM_DONE_LEN)-1)<<RF_RX_PM_DONE_POS))
#define RF_RX_PM_EN                                             RF_RX_PM_EN
#define RF_RX_PM_EN_POS                                         (29U)
#define RF_RX_PM_EN_LEN                                         (1U)
#define RF_RX_PM_EN_MSK                                         (((1U<<RF_RX_PM_EN_LEN)-1)<<RF_RX_PM_EN_POS)
#define RF_RX_PM_EN_UMSK                                        (~(((1U<<RF_RX_PM_EN_LEN)-1)<<RF_RX_PM_EN_POS))
#define RF_RX_PM_IN_SEL                                         RF_RX_PM_IN_SEL
#define RF_RX_PM_IN_SEL_POS                                     (30U)
#define RF_RX_PM_IN_SEL_LEN                                     (2U)
#define RF_RX_PM_IN_SEL_MSK                                     (((1U<<RF_RX_PM_IN_SEL_LEN)-1)<<RF_RX_PM_IN_SEL_POS)
#define RF_RX_PM_IN_SEL_UMSK                                    (~(((1U<<RF_RX_PM_IN_SEL_LEN)-1)<<RF_RX_PM_IN_SEL_POS))

/* 0x61C : dfe_ctrl_7 */
#define RF_DFE_CTRL_7_OFFSET                                    (0x61C)
#define RF_RX_PM_START_OFS                                      RF_RX_PM_START_OFS
#define RF_RX_PM_START_OFS_POS                                  (0U)
#define RF_RX_PM_START_OFS_LEN                                  (16U)
#define RF_RX_PM_START_OFS_MSK                                  (((1U<<RF_RX_PM_START_OFS_LEN)-1)<<RF_RX_PM_START_OFS_POS)
#define RF_RX_PM_START_OFS_UMSK                                 (~(((1U<<RF_RX_PM_START_OFS_LEN)-1)<<RF_RX_PM_START_OFS_POS))
#define RF_RX_PM_ACC_LEN                                        RF_RX_PM_ACC_LEN
#define RF_RX_PM_ACC_LEN_POS                                    (16U)
#define RF_RX_PM_ACC_LEN_LEN                                    (16U)
#define RF_RX_PM_ACC_LEN_MSK                                    (((1U<<RF_RX_PM_ACC_LEN_LEN)-1)<<RF_RX_PM_ACC_LEN_POS)
#define RF_RX_PM_ACC_LEN_UMSK                                   (~(((1U<<RF_RX_PM_ACC_LEN_LEN)-1)<<RF_RX_PM_ACC_LEN_POS))

/* 0x620 : dfe_ctrl_8 */
#define RF_DFE_CTRL_8_OFFSET                                    (0x620)
#define RF_RX_PM_IQACC_I                                        RF_RX_PM_IQACC_I
#define RF_RX_PM_IQACC_I_POS                                    (0U)
#define RF_RX_PM_IQACC_I_LEN                                    (25U)
#define RF_RX_PM_IQACC_I_MSK                                    (((1U<<RF_RX_PM_IQACC_I_LEN)-1)<<RF_RX_PM_IQACC_I_POS)
#define RF_RX_PM_IQACC_I_UMSK                                   (~(((1U<<RF_RX_PM_IQACC_I_LEN)-1)<<RF_RX_PM_IQACC_I_POS))

/* 0x624 : dfe_ctrl_9 */
#define RF_DFE_CTRL_9_OFFSET                                    (0x624)
#define RF_RX_PM_IQACC_Q                                        RF_RX_PM_IQACC_Q
#define RF_RX_PM_IQACC_Q_POS                                    (0U)
#define RF_RX_PM_IQACC_Q_LEN                                    (25U)
#define RF_RX_PM_IQACC_Q_MSK                                    (((1U<<RF_RX_PM_IQACC_Q_LEN)-1)<<RF_RX_PM_IQACC_Q_POS)
#define RF_RX_PM_IQACC_Q_UMSK                                   (~(((1U<<RF_RX_PM_IQACC_Q_LEN)-1)<<RF_RX_PM_IQACC_Q_POS))

/* 0x628 : dfe_ctrl_10 */
#define RF_DFE_CTRL_10_OFFSET                                   (0x628)
#define RF_DFE_DAC_RAW_I                                        RF_DFE_DAC_RAW_I
#define RF_DFE_DAC_RAW_I_POS                                    (0U)
#define RF_DFE_DAC_RAW_I_LEN                                    (11U)
#define RF_DFE_DAC_RAW_I_MSK                                    (((1U<<RF_DFE_DAC_RAW_I_LEN)-1)<<RF_DFE_DAC_RAW_I_POS)
#define RF_DFE_DAC_RAW_I_UMSK                                   (~(((1U<<RF_DFE_DAC_RAW_I_LEN)-1)<<RF_DFE_DAC_RAW_I_POS))
#define RF_DFE_DAC_RAW_Q                                        RF_DFE_DAC_RAW_Q
#define RF_DFE_DAC_RAW_Q_POS                                    (16U)
#define RF_DFE_DAC_RAW_Q_LEN                                    (11U)
#define RF_DFE_DAC_RAW_Q_MSK                                    (((1U<<RF_DFE_DAC_RAW_Q_LEN)-1)<<RF_DFE_DAC_RAW_Q_POS)
#define RF_DFE_DAC_RAW_Q_UMSK                                   (~(((1U<<RF_DFE_DAC_RAW_Q_LEN)-1)<<RF_DFE_DAC_RAW_Q_POS))

/* 0x62C : dfe_ctrl_11 */
#define RF_DFE_CTRL_11_OFFSET                                   (0x62C)
#define RF_DFE_ADC_RAW_I                                        RF_DFE_ADC_RAW_I
#define RF_DFE_ADC_RAW_I_POS                                    (0U)
#define RF_DFE_ADC_RAW_I_LEN                                    (10U)
#define RF_DFE_ADC_RAW_I_MSK                                    (((1U<<RF_DFE_ADC_RAW_I_LEN)-1)<<RF_DFE_ADC_RAW_I_POS)
#define RF_DFE_ADC_RAW_I_UMSK                                   (~(((1U<<RF_DFE_ADC_RAW_I_LEN)-1)<<RF_DFE_ADC_RAW_I_POS))
#define RF_DFE_ADC_RAW_Q                                        RF_DFE_ADC_RAW_Q
#define RF_DFE_ADC_RAW_Q_POS                                    (16U)
#define RF_DFE_ADC_RAW_Q_LEN                                    (10U)
#define RF_DFE_ADC_RAW_Q_MSK                                    (((1U<<RF_DFE_ADC_RAW_Q_LEN)-1)<<RF_DFE_ADC_RAW_Q_POS)
#define RF_DFE_ADC_RAW_Q_UMSK                                   (~(((1U<<RF_DFE_ADC_RAW_Q_LEN)-1)<<RF_DFE_ADC_RAW_Q_POS))

/* 0x630 : dfe_ctrl_12 */
#define RF_DFE_CTRL_12_OFFSET                                   (0x630)
#define RF_TX_DVGA_GAIN_QDB_GC0                                 RF_TX_DVGA_GAIN_QDB_GC0
#define RF_TX_DVGA_GAIN_QDB_GC0_POS                             (0U)
#define RF_TX_DVGA_GAIN_QDB_GC0_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC0_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC0_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC0_POS)
#define RF_TX_DVGA_GAIN_QDB_GC0_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC0_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC0_POS))
#define RF_TX_DVGA_GAIN_QDB_GC1                                 RF_TX_DVGA_GAIN_QDB_GC1
#define RF_TX_DVGA_GAIN_QDB_GC1_POS                             (8U)
#define RF_TX_DVGA_GAIN_QDB_GC1_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC1_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC1_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC1_POS)
#define RF_TX_DVGA_GAIN_QDB_GC1_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC1_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC1_POS))
#define RF_TX_DVGA_GAIN_QDB_GC2                                 RF_TX_DVGA_GAIN_QDB_GC2
#define RF_TX_DVGA_GAIN_QDB_GC2_POS                             (16U)
#define RF_TX_DVGA_GAIN_QDB_GC2_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC2_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC2_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC2_POS)
#define RF_TX_DVGA_GAIN_QDB_GC2_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC2_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC2_POS))
#define RF_TX_DVGA_GAIN_QDB_GC3                                 RF_TX_DVGA_GAIN_QDB_GC3
#define RF_TX_DVGA_GAIN_QDB_GC3_POS                             (24U)
#define RF_TX_DVGA_GAIN_QDB_GC3_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC3_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC3_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC3_POS)
#define RF_TX_DVGA_GAIN_QDB_GC3_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC3_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC3_POS))

/* 0x634 : dfe_ctrl_13 */
#define RF_DFE_CTRL_13_OFFSET                                   (0x634)
#define RF_TX_DVGA_GAIN_QDB_GC4                                 RF_TX_DVGA_GAIN_QDB_GC4
#define RF_TX_DVGA_GAIN_QDB_GC4_POS                             (0U)
#define RF_TX_DVGA_GAIN_QDB_GC4_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC4_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC4_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC4_POS)
#define RF_TX_DVGA_GAIN_QDB_GC4_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC4_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC4_POS))
#define RF_TX_DVGA_GAIN_QDB_GC5                                 RF_TX_DVGA_GAIN_QDB_GC5
#define RF_TX_DVGA_GAIN_QDB_GC5_POS                             (8U)
#define RF_TX_DVGA_GAIN_QDB_GC5_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC5_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC5_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC5_POS)
#define RF_TX_DVGA_GAIN_QDB_GC5_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC5_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC5_POS))
#define RF_TX_DVGA_GAIN_QDB_GC6                                 RF_TX_DVGA_GAIN_QDB_GC6
#define RF_TX_DVGA_GAIN_QDB_GC6_POS                             (16U)
#define RF_TX_DVGA_GAIN_QDB_GC6_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC6_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC6_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC6_POS)
#define RF_TX_DVGA_GAIN_QDB_GC6_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC6_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC6_POS))
#define RF_TX_DVGA_GAIN_QDB_GC7                                 RF_TX_DVGA_GAIN_QDB_GC7
#define RF_TX_DVGA_GAIN_QDB_GC7_POS                             (24U)
#define RF_TX_DVGA_GAIN_QDB_GC7_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC7_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC7_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC7_POS)
#define RF_TX_DVGA_GAIN_QDB_GC7_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC7_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC7_POS))

/* 0x638 : dfe_ctrl_14 */
#define RF_DFE_CTRL_14_OFFSET                                   (0x638)
#define RF_TX_DVGA_GAIN_QDB_GC8                                 RF_TX_DVGA_GAIN_QDB_GC8
#define RF_TX_DVGA_GAIN_QDB_GC8_POS                             (0U)
#define RF_TX_DVGA_GAIN_QDB_GC8_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC8_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC8_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC8_POS)
#define RF_TX_DVGA_GAIN_QDB_GC8_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC8_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC8_POS))
#define RF_TX_DVGA_GAIN_QDB_GC9                                 RF_TX_DVGA_GAIN_QDB_GC9
#define RF_TX_DVGA_GAIN_QDB_GC9_POS                             (8U)
#define RF_TX_DVGA_GAIN_QDB_GC9_LEN                             (7U)
#define RF_TX_DVGA_GAIN_QDB_GC9_MSK                             (((1U<<RF_TX_DVGA_GAIN_QDB_GC9_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC9_POS)
#define RF_TX_DVGA_GAIN_QDB_GC9_UMSK                            (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC9_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC9_POS))
#define RF_TX_DVGA_GAIN_QDB_GC10                                RF_TX_DVGA_GAIN_QDB_GC10
#define RF_TX_DVGA_GAIN_QDB_GC10_POS                            (16U)
#define RF_TX_DVGA_GAIN_QDB_GC10_LEN                            (7U)
#define RF_TX_DVGA_GAIN_QDB_GC10_MSK                            (((1U<<RF_TX_DVGA_GAIN_QDB_GC10_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC10_POS)
#define RF_TX_DVGA_GAIN_QDB_GC10_UMSK                           (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC10_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC10_POS))
#define RF_TX_DVGA_GAIN_QDB_GC11                                RF_TX_DVGA_GAIN_QDB_GC11
#define RF_TX_DVGA_GAIN_QDB_GC11_POS                            (24U)
#define RF_TX_DVGA_GAIN_QDB_GC11_LEN                            (7U)
#define RF_TX_DVGA_GAIN_QDB_GC11_MSK                            (((1U<<RF_TX_DVGA_GAIN_QDB_GC11_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC11_POS)
#define RF_TX_DVGA_GAIN_QDB_GC11_UMSK                           (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC11_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC11_POS))

/* 0x63C : dfe_ctrl_15 */
#define RF_DFE_CTRL_15_OFFSET                                   (0x63C)
#define RF_TX_DVGA_GAIN_QDB_GC12                                RF_TX_DVGA_GAIN_QDB_GC12
#define RF_TX_DVGA_GAIN_QDB_GC12_POS                            (0U)
#define RF_TX_DVGA_GAIN_QDB_GC12_LEN                            (7U)
#define RF_TX_DVGA_GAIN_QDB_GC12_MSK                            (((1U<<RF_TX_DVGA_GAIN_QDB_GC12_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC12_POS)
#define RF_TX_DVGA_GAIN_QDB_GC12_UMSK                           (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC12_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC12_POS))
#define RF_TX_DVGA_GAIN_QDB_GC13                                RF_TX_DVGA_GAIN_QDB_GC13
#define RF_TX_DVGA_GAIN_QDB_GC13_POS                            (8U)
#define RF_TX_DVGA_GAIN_QDB_GC13_LEN                            (7U)
#define RF_TX_DVGA_GAIN_QDB_GC13_MSK                            (((1U<<RF_TX_DVGA_GAIN_QDB_GC13_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC13_POS)
#define RF_TX_DVGA_GAIN_QDB_GC13_UMSK                           (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC13_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC13_POS))
#define RF_TX_DVGA_GAIN_QDB_GC14                                RF_TX_DVGA_GAIN_QDB_GC14
#define RF_TX_DVGA_GAIN_QDB_GC14_POS                            (16U)
#define RF_TX_DVGA_GAIN_QDB_GC14_LEN                            (7U)
#define RF_TX_DVGA_GAIN_QDB_GC14_MSK                            (((1U<<RF_TX_DVGA_GAIN_QDB_GC14_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC14_POS)
#define RF_TX_DVGA_GAIN_QDB_GC14_UMSK                           (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC14_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC14_POS))
#define RF_TX_DVGA_GAIN_QDB_GC15                                RF_TX_DVGA_GAIN_QDB_GC15
#define RF_TX_DVGA_GAIN_QDB_GC15_POS                            (24U)
#define RF_TX_DVGA_GAIN_QDB_GC15_LEN                            (7U)
#define RF_TX_DVGA_GAIN_QDB_GC15_MSK                            (((1U<<RF_TX_DVGA_GAIN_QDB_GC15_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC15_POS)
#define RF_TX_DVGA_GAIN_QDB_GC15_UMSK                           (~(((1U<<RF_TX_DVGA_GAIN_QDB_GC15_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_GC15_POS))

/* 0x640 : dfe_ctrl_16 */
#define RF_DFE_CTRL_16_OFFSET                                   (0x640)
#define RF_TBB_IND_GC0                                          RF_TBB_IND_GC0
#define RF_TBB_IND_GC0_POS                                      (0U)
#define RF_TBB_IND_GC0_LEN                                      (3U)
#define RF_TBB_IND_GC0_MSK                                      (((1U<<RF_TBB_IND_GC0_LEN)-1)<<RF_TBB_IND_GC0_POS)
#define RF_TBB_IND_GC0_UMSK                                     (~(((1U<<RF_TBB_IND_GC0_LEN)-1)<<RF_TBB_IND_GC0_POS))
#define RF_TBB_IND_GC1                                          RF_TBB_IND_GC1
#define RF_TBB_IND_GC1_POS                                      (4U)
#define RF_TBB_IND_GC1_LEN                                      (3U)
#define RF_TBB_IND_GC1_MSK                                      (((1U<<RF_TBB_IND_GC1_LEN)-1)<<RF_TBB_IND_GC1_POS)
#define RF_TBB_IND_GC1_UMSK                                     (~(((1U<<RF_TBB_IND_GC1_LEN)-1)<<RF_TBB_IND_GC1_POS))
#define RF_TBB_IND_GC2                                          RF_TBB_IND_GC2
#define RF_TBB_IND_GC2_POS                                      (8U)
#define RF_TBB_IND_GC2_LEN                                      (3U)
#define RF_TBB_IND_GC2_MSK                                      (((1U<<RF_TBB_IND_GC2_LEN)-1)<<RF_TBB_IND_GC2_POS)
#define RF_TBB_IND_GC2_UMSK                                     (~(((1U<<RF_TBB_IND_GC2_LEN)-1)<<RF_TBB_IND_GC2_POS))
#define RF_TBB_IND_GC3                                          RF_TBB_IND_GC3
#define RF_TBB_IND_GC3_POS                                      (12U)
#define RF_TBB_IND_GC3_LEN                                      (3U)
#define RF_TBB_IND_GC3_MSK                                      (((1U<<RF_TBB_IND_GC3_LEN)-1)<<RF_TBB_IND_GC3_POS)
#define RF_TBB_IND_GC3_UMSK                                     (~(((1U<<RF_TBB_IND_GC3_LEN)-1)<<RF_TBB_IND_GC3_POS))
#define RF_TBB_IND_GC4                                          RF_TBB_IND_GC4
#define RF_TBB_IND_GC4_POS                                      (16U)
#define RF_TBB_IND_GC4_LEN                                      (3U)
#define RF_TBB_IND_GC4_MSK                                      (((1U<<RF_TBB_IND_GC4_LEN)-1)<<RF_TBB_IND_GC4_POS)
#define RF_TBB_IND_GC4_UMSK                                     (~(((1U<<RF_TBB_IND_GC4_LEN)-1)<<RF_TBB_IND_GC4_POS))
#define RF_TBB_IND_GC5                                          RF_TBB_IND_GC5
#define RF_TBB_IND_GC5_POS                                      (20U)
#define RF_TBB_IND_GC5_LEN                                      (3U)
#define RF_TBB_IND_GC5_MSK                                      (((1U<<RF_TBB_IND_GC5_LEN)-1)<<RF_TBB_IND_GC5_POS)
#define RF_TBB_IND_GC5_UMSK                                     (~(((1U<<RF_TBB_IND_GC5_LEN)-1)<<RF_TBB_IND_GC5_POS))
#define RF_TBB_IND_GC6                                          RF_TBB_IND_GC6
#define RF_TBB_IND_GC6_POS                                      (24U)
#define RF_TBB_IND_GC6_LEN                                      (3U)
#define RF_TBB_IND_GC6_MSK                                      (((1U<<RF_TBB_IND_GC6_LEN)-1)<<RF_TBB_IND_GC6_POS)
#define RF_TBB_IND_GC6_UMSK                                     (~(((1U<<RF_TBB_IND_GC6_LEN)-1)<<RF_TBB_IND_GC6_POS))
#define RF_TBB_IND_GC7                                          RF_TBB_IND_GC7
#define RF_TBB_IND_GC7_POS                                      (28U)
#define RF_TBB_IND_GC7_LEN                                      (3U)
#define RF_TBB_IND_GC7_MSK                                      (((1U<<RF_TBB_IND_GC7_LEN)-1)<<RF_TBB_IND_GC7_POS)
#define RF_TBB_IND_GC7_UMSK                                     (~(((1U<<RF_TBB_IND_GC7_LEN)-1)<<RF_TBB_IND_GC7_POS))

/* 0x644 : dfe_ctrl_17 */
#define RF_DFE_CTRL_17_OFFSET                                   (0x644)
#define RF_TBB_IND_GC8                                          RF_TBB_IND_GC8
#define RF_TBB_IND_GC8_POS                                      (0U)
#define RF_TBB_IND_GC8_LEN                                      (3U)
#define RF_TBB_IND_GC8_MSK                                      (((1U<<RF_TBB_IND_GC8_LEN)-1)<<RF_TBB_IND_GC8_POS)
#define RF_TBB_IND_GC8_UMSK                                     (~(((1U<<RF_TBB_IND_GC8_LEN)-1)<<RF_TBB_IND_GC8_POS))
#define RF_TBB_IND_GC9                                          RF_TBB_IND_GC9
#define RF_TBB_IND_GC9_POS                                      (4U)
#define RF_TBB_IND_GC9_LEN                                      (3U)
#define RF_TBB_IND_GC9_MSK                                      (((1U<<RF_TBB_IND_GC9_LEN)-1)<<RF_TBB_IND_GC9_POS)
#define RF_TBB_IND_GC9_UMSK                                     (~(((1U<<RF_TBB_IND_GC9_LEN)-1)<<RF_TBB_IND_GC9_POS))
#define RF_TBB_IND_GC10                                         RF_TBB_IND_GC10
#define RF_TBB_IND_GC10_POS                                     (8U)
#define RF_TBB_IND_GC10_LEN                                     (3U)
#define RF_TBB_IND_GC10_MSK                                     (((1U<<RF_TBB_IND_GC10_LEN)-1)<<RF_TBB_IND_GC10_POS)
#define RF_TBB_IND_GC10_UMSK                                    (~(((1U<<RF_TBB_IND_GC10_LEN)-1)<<RF_TBB_IND_GC10_POS))
#define RF_TBB_IND_GC11                                         RF_TBB_IND_GC11
#define RF_TBB_IND_GC11_POS                                     (12U)
#define RF_TBB_IND_GC11_LEN                                     (3U)
#define RF_TBB_IND_GC11_MSK                                     (((1U<<RF_TBB_IND_GC11_LEN)-1)<<RF_TBB_IND_GC11_POS)
#define RF_TBB_IND_GC11_UMSK                                    (~(((1U<<RF_TBB_IND_GC11_LEN)-1)<<RF_TBB_IND_GC11_POS))
#define RF_TBB_IND_GC12                                         RF_TBB_IND_GC12
#define RF_TBB_IND_GC12_POS                                     (16U)
#define RF_TBB_IND_GC12_LEN                                     (3U)
#define RF_TBB_IND_GC12_MSK                                     (((1U<<RF_TBB_IND_GC12_LEN)-1)<<RF_TBB_IND_GC12_POS)
#define RF_TBB_IND_GC12_UMSK                                    (~(((1U<<RF_TBB_IND_GC12_LEN)-1)<<RF_TBB_IND_GC12_POS))
#define RF_TBB_IND_GC13                                         RF_TBB_IND_GC13
#define RF_TBB_IND_GC13_POS                                     (20U)
#define RF_TBB_IND_GC13_LEN                                     (3U)
#define RF_TBB_IND_GC13_MSK                                     (((1U<<RF_TBB_IND_GC13_LEN)-1)<<RF_TBB_IND_GC13_POS)
#define RF_TBB_IND_GC13_UMSK                                    (~(((1U<<RF_TBB_IND_GC13_LEN)-1)<<RF_TBB_IND_GC13_POS))
#define RF_TBB_IND_GC14                                         RF_TBB_IND_GC14
#define RF_TBB_IND_GC14_POS                                     (24U)
#define RF_TBB_IND_GC14_LEN                                     (3U)
#define RF_TBB_IND_GC14_MSK                                     (((1U<<RF_TBB_IND_GC14_LEN)-1)<<RF_TBB_IND_GC14_POS)
#define RF_TBB_IND_GC14_UMSK                                    (~(((1U<<RF_TBB_IND_GC14_LEN)-1)<<RF_TBB_IND_GC14_POS))
#define RF_TBB_IND_GC15                                         RF_TBB_IND_GC15
#define RF_TBB_IND_GC15_POS                                     (28U)
#define RF_TBB_IND_GC15_LEN                                     (3U)
#define RF_TBB_IND_GC15_MSK                                     (((1U<<RF_TBB_IND_GC15_LEN)-1)<<RF_TBB_IND_GC15_POS)
#define RF_TBB_IND_GC15_UMSK                                    (~(((1U<<RF_TBB_IND_GC15_LEN)-1)<<RF_TBB_IND_GC15_POS))

/* 0x648 : dfe_ctrl_18 */
#define RF_DFE_CTRL_18_OFFSET                                   (0x648)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC0                             RF_TX_DVGA_GAIN_QDB_BLE_GC0
#define RF_TX_DVGA_GAIN_QDB_BLE_GC0_POS                         (0U)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC0_LEN                         (7U)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC0_MSK                         (((1U<<RF_TX_DVGA_GAIN_QDB_BLE_GC0_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_BLE_GC0_POS)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC0_UMSK                        (~(((1U<<RF_TX_DVGA_GAIN_QDB_BLE_GC0_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_BLE_GC0_POS))
#define RF_TX_DVGA_GAIN_QDB_BLE_GC1                             RF_TX_DVGA_GAIN_QDB_BLE_GC1
#define RF_TX_DVGA_GAIN_QDB_BLE_GC1_POS                         (8U)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC1_LEN                         (7U)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC1_MSK                         (((1U<<RF_TX_DVGA_GAIN_QDB_BLE_GC1_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_BLE_GC1_POS)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC1_UMSK                        (~(((1U<<RF_TX_DVGA_GAIN_QDB_BLE_GC1_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_BLE_GC1_POS))
#define RF_TX_DVGA_GAIN_QDB_BLE_GC2                             RF_TX_DVGA_GAIN_QDB_BLE_GC2
#define RF_TX_DVGA_GAIN_QDB_BLE_GC2_POS                         (16U)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC2_LEN                         (7U)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC2_MSK                         (((1U<<RF_TX_DVGA_GAIN_QDB_BLE_GC2_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_BLE_GC2_POS)
#define RF_TX_DVGA_GAIN_QDB_BLE_GC2_UMSK                        (~(((1U<<RF_TX_DVGA_GAIN_QDB_BLE_GC2_LEN)-1)<<RF_TX_DVGA_GAIN_QDB_BLE_GC2_POS))


struct  rf_reg {
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
            uint32_t rf_fsm_ctrl_en                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t rf_fsm_t2r_cal_mode            :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t rf_fsm_state                   :  3; /* [ 6: 4],          r,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rf_rc_state_dbg                :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t rf_rc_state_dbg_en             :  1; /* [   11],        r/w,        0x0 */
            uint32_t rf_fsm_st_int_sel              :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rf_fsm_st_int                  :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rf_fsm_st_int_clr              :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rf_fsm_st_int_set              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t rf_rc_state_value              :  3; /* [30:28],          r,        0x0 */
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
            uint32_t pu_ctrl_hw                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t rx_gain_ctrl_hw                :  1; /* [    1],        r/w,        0x1 */
            uint32_t tx_gain_ctrl_hw                :  1; /* [    2],        r/w,        0x1 */
            uint32_t lna_ctrl_hw                    :  1; /* [    3],        r/w,        0x1 */
            uint32_t rbb_bw_ctrl_hw                 :  1; /* [    4],        r/w,        0x1 */
            uint32_t trxcal_ctrl_hw                 :  1; /* [    5],        r/w,        0x1 */
            uint32_t lo_ctrl_hw                     :  1; /* [    6],        r/w,        0x1 */
            uint32_t inc_acal_ctrl_en_hw            :  1; /* [    7],        r/w,        0x1 */
            uint32_t inc_fcal_ctrl_en_hw            :  1; /* [    8],        r/w,        0x1 */
            uint32_t sdm_ctrl_hw                    :  1; /* [    9],        r/w,        0x1 */
            uint32_t rbb_pkdet_en_ctrl_hw           :  1; /* [   10],        r/w,        0x1 */
            uint32_t rbb_pkdet_out_rstn_ctrl_hw     :  1; /* [   11],        r/w,        0x1 */
            uint32_t adda_ctrl_hw                   :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfctrl_hw_en;

    /* 0x10 : temp_comp */
    union {
        struct {
            uint32_t const_acal                     :  8; /* [ 7: 0],        r/w,       0x1a */
            uint32_t const_fcal                     :  8; /* [15: 8],        r/w,       0x74 */
            uint32_t temp_comp_en                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } temp_comp;

    /* 0x14 : rfcal_status */
    union {
        struct {
            uint32_t rcal_status                    :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t adc_oscal_status               :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t fcal_status                    :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t acal_status                    :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t inc_fcal_status                :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t inc_acal_status                :  2; /* [11:10],        r/w,        0x0 */
            uint32_t clkpll_cal_status              :  2; /* [13:12],        r/w,        0x0 */
            uint32_t ros_status                     :  2; /* [15:14],        r/w,        0x0 */
            uint32_t tos_status                     :  2; /* [17:16],        r/w,        0x0 */
            uint32_t rccal_status                   :  2; /* [19:18],        r/w,        0x0 */
            uint32_t lo_leakcal_status              :  2; /* [21:20],        r/w,        0x0 */
            uint32_t tiqcal_status_resv             :  2; /* [23:22],        r/w,        0x0 */
            uint32_t riqcal_status_resv             :  2; /* [25:24],        r/w,        0x0 */
            uint32_t pwdet_cal_status               :  2; /* [27:26],        r/w,        0x0 */
            uint32_t tenscal_status                 :  2; /* [29:28],        r/w,        0x0 */
            uint32_t dpd_status                     :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_status;

    /* 0x18 : rfcal_status2 */
    union {
        struct {
            uint32_t dl_rfcal_table_status          :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_status2;

    /* 0x1C : Calibration mode register */
    union {
        struct {
            uint32_t rcal_en_resv                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t adc_oscal_en                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t dl_rfcal_table_en              :  1; /* [    2],        r/w,        0x0 */
            uint32_t fcal_en                        :  1; /* [    3],        r/w,        0x0 */
            uint32_t acal_en                        :  1; /* [    4],        r/w,        0x0 */
            uint32_t fcal_inc_en                    :  1; /* [    5],        r/w,        0x1 */
            uint32_t acal_inc_en                    :  1; /* [    6],        r/w,        0x1 */
            uint32_t roscal_inc_en                  :  1; /* [    7],        r/w,        0x1 */
            uint32_t clkpll_cal_en                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t roscal_en                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t toscal_en                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t rccal_en                       :  1; /* [   11],        r/w,        0x0 */
            uint32_t lo_leakcal_en                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t tiqcal_en                      :  1; /* [   13],        r/w,        0x0 */
            uint32_t riqcal_en                      :  1; /* [   14],        r/w,        0x0 */
            uint32_t pwdet_cal_en                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t tsencal_en                     :  1; /* [   16],        r/w,        0x0 */
            uint32_t dpd_en                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfcal_ctrlen;

    /* 0x20 : rf calibration state enabl in full cal list */
    union {
        struct {
            uint32_t rcal_sten_resv                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t adc_oscal_sten                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t dl_rfcal_table_sten            :  1; /* [    2],        r/w,        0x1 */
            uint32_t fcal_sten                      :  1; /* [    3],        r/w,        0x1 */
            uint32_t acal_sten                      :  1; /* [    4],        r/w,        0x1 */
            uint32_t inc_fcal_sten                  :  1; /* [    5],        r/w,        0x1 */
            uint32_t inc_acal_sten                  :  1; /* [    6],        r/w,        0x1 */
            uint32_t clkpll_cal_sten                :  1; /* [    7],        r/w,        0x1 */
            uint32_t roscal_sten                    :  1; /* [    8],        r/w,        0x1 */
            uint32_t toscal_sten_resv               :  1; /* [    9],        r/w,        0x0 */
            uint32_t rccal_sten                     :  1; /* [   10],        r/w,        0x1 */
            uint32_t lo_leakcal_sten                :  1; /* [   11],        r/w,        0x1 */
            uint32_t tiqcal_sten                    :  1; /* [   12],        r/w,        0x1 */
            uint32_t riqcal_sten                    :  1; /* [   13],        r/w,        0x0 */
            uint32_t pwdet_cal_sten                 :  1; /* [   14],        r/w,        0x1 */
            uint32_t tsencal_sten                   :  1; /* [   15],        r/w,        0x1 */
            uint32_t dpd_sten                       :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_29                 : 13; /* [29:17],       rsvd,        0x0 */
            uint32_t rfcal_level                    :  2; /* [31:30],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } rfcal_stateen;

    /* 0x24 : SARADC Control Registers */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } saradc_resv;

    /* 0x28 : ZRF Control register 0 */
    union {
        struct {
            uint32_t aupll_sdm_rst_dly              :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t lo_sdm_rst_dly                 :  2; /* [ 3: 2],        r/w,        0x3 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t ppu_lead                       :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t pud_vco_dly                    :  2; /* [11:10],        r/w,        0x1 */
            uint32_t pud_iref_dly                   :  2; /* [13:12],        r/w,        0x1 */
            uint32_t pud_pa_dly                     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t reserved_16_26                 : 11; /* [26:16],       rsvd,        0x0 */
            uint32_t mbg_trim                       :  2; /* [28:27],        r/w,        0x2 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_base_ctrl1;

    /* 0x2C : ZRF Control register 0 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_base_ctrl2;

    /* 0x30 : pucr1 */
    union {
        struct {
            uint32_t pu_sfreg                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t pu_lna                         :  1; /* [    8],        r/w,        0x0 */
            uint32_t pu_rmxgm                       :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_rmx                         :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_rbb                         :  1; /* [   11],        r/w,        0x0 */
            uint32_t pu_adda_ldo                    :  1; /* [   12],        r/w,        0x0 */
            uint32_t adc_clk_en                     :  1; /* [   13],        r/w,        0x0 */
            uint32_t pu_adc                         :  1; /* [   14],        r/w,        0x0 */
            uint32_t pu_op_atest                    :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_pa                          :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_tmx                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_tbb                         :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_dac                         :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_vco                         :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_fbdv                        :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pfd                         :  1; /* [   22],        r/w,        0x0 */
            uint32_t pu_osmx                        :  1; /* [   23],        r/w,        0x0 */
            uint32_t pu_rxbuf                       :  1; /* [   24],        r/w,        0x0 */
            uint32_t pu_txbuf                       :  1; /* [   25],        r/w,        0x0 */
            uint32_t trsw_en                        :  1; /* [   26],        r/w,        0x1 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pu_pkdet                       :  1; /* [   28],        r/w,        0x0 */
            uint32_t pu_rosdac                      :  1; /* [   29],        r/w,        0x0 */
            uint32_t pu_pwrmx                       :  1; /* [   30],        r/w,        0x0 */
            uint32_t pu_tosdac                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr1;

    /* 0x34 : read only from hardware logic */
    union {
        struct {
            uint32_t pu_sfreg_hw                    :  1; /* [    0],          r,        0x1 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t pu_lna_hw                      :  1; /* [    8],          r,        0x1 */
            uint32_t pu_rmxgm_hw                    :  1; /* [    9],          r,        0x1 */
            uint32_t pu_rmx_hw                      :  1; /* [   10],          r,        0x1 */
            uint32_t pu_rbb_hw                      :  1; /* [   11],          r,        0x1 */
            uint32_t pu_adda_ldo_hw                 :  1; /* [   12],          r,        0x1 */
            uint32_t adc_clk_en_hw                  :  1; /* [   13],          r,        0x1 */
            uint32_t pu_adc_hw                      :  1; /* [   14],          r,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pu_pa_hw                       :  1; /* [   16],          r,        0x1 */
            uint32_t pu_tmx_hw                      :  1; /* [   17],          r,        0x1 */
            uint32_t pu_tbb_hw                      :  1; /* [   18],          r,        0x1 */
            uint32_t pu_dac_hw                      :  1; /* [   19],          r,        0x1 */
            uint32_t pu_vco_hw                      :  1; /* [   20],          r,        0x1 */
            uint32_t pu_fbdv_hw                     :  1; /* [   21],          r,        0x1 */
            uint32_t pu_pfd_hw                      :  1; /* [   22],          r,        0x1 */
            uint32_t pu_osmx_hw                     :  1; /* [   23],          r,        0x1 */
            uint32_t pu_rxbuf_hw                    :  1; /* [   24],          r,        0x1 */
            uint32_t pu_txbuf_hw                    :  1; /* [   25],          r,        0x1 */
            uint32_t trsw_en_hw                     :  1; /* [   26],          r,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pu_pkdet_hw                    :  1; /* [   28],          r,        0x1 */
            uint32_t pu_rosdac_hw                   :  1; /* [   29],          r,        0x1 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t pu_tosdac_hw                   :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } pucr1_hw;

    /* 0x38 : pucr2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr2;

    /* 0x3C : pucr2_hw */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr2_hw;

    /* 0x40 : ppu_ctrl_hw */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t ppu_lna_hw                     :  1; /* [    8],          r,        0x1 */
            uint32_t ppu_rmxgm_hw                   :  1; /* [    9],          r,        0x1 */
            uint32_t reserved_10                    :  1; /* [   10],       rsvd,        0x0 */
            uint32_t ppu_rbb_hw                     :  1; /* [   11],          r,        0x1 */
            uint32_t reserved_12_19                 :  8; /* [19:12],       rsvd,        0x0 */
            uint32_t ppu_vco_hw                     :  1; /* [   20],          r,        0x1 */
            uint32_t ppu_fbdv_hw                    :  1; /* [   21],          r,        0x1 */
            uint32_t ppu_pfd_hw                     :  1; /* [   22],          r,        0x1 */
            uint32_t ppu_osmx_hw                    :  1; /* [   23],          r,        0x1 */
            uint32_t ppu_rxbuf_hw                   :  1; /* [   24],          r,        0x1 */
            uint32_t ppu_txbuf_hw                   :  1; /* [   25],          r,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ppu_ctrl_hw;

    /* 0x44 : pud_ctrl_hw */
    union {
        struct {
            uint32_t reserved_0_19                  : 20; /* [19: 0],       rsvd,        0x0 */
            uint32_t pud_vco_hw                     :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pud_ctrl_hw;

    /* 0x48 : gain control1 */
    union {
        struct {
            uint32_t gc_lna                         :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t gc_rmxgm                       :  2; /* [ 5: 4],        r/w,        0x3 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t gc_rbb1                        :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gc_rbb2                        :  3; /* [14:12],        r/w,        0x6 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gc_tmx                         :  3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t gc_tbb                         :  5; /* [24:20],        r/w,        0x8 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t gc_tbb_boost                   :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain1;

    /* 0x4C : trx gain hardware readback */
    union {
        struct {
            uint32_t gc_lna_hw                      :  3; /* [ 2: 0],          r,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t gc_rmxgm_hw                    :  2; /* [ 5: 4],          r,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t gc_rbb1_hw                     :  2; /* [ 9: 8],          r,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gc_rbb2_hw                     :  3; /* [14:12],          r,        0x6 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gc_tmx_hw                      :  3; /* [18:16],          r,        0x8 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t gc_tbb_hw                      :  5; /* [24:20],          r,        0x3 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t gc_tbb_boost_hw                :  2; /* [29:28],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain_hw;

    /* 0x50 : dc test register */
    union {
        struct {
            uint32_t tmux                           :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t dc_tp_en                       :  1; /* [    3],        r/w,        0x0 */
            uint32_t dc_tp_clkpll_en                :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t ten_clkpll                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t ten_clkpll_sfreg               :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t ten_rrf_0                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t ten_rrf_1                      :  1; /* [   13],        r/w,        0x0 */
            uint32_t ten_pa                         :  1; /* [   14],        r/w,        0x0 */
            uint32_t ten_tmx                        :  1; /* [   15],        r/w,        0x0 */
            uint32_t ten_tia                        :  1; /* [   16],        r/w,        0x0 */
            uint32_t ten_bq                         :  1; /* [   17],        r/w,        0x0 */
            uint32_t ten_atest                      :  1; /* [   18],        r/w,        0x0 */
            uint32_t ten_tbb                        :  1; /* [   19],        r/w,        0x0 */
            uint32_t ten_adc                        :  1; /* [   20],        r/w,        0x0 */
            uint32_t ten_dac_i                      :  1; /* [   21],        r/w,        0x0 */
            uint32_t ten_dac_q                      :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ten_vco                        :  1; /* [   24],        r/w,        0x0 */
            uint32_t ten_pfdcp                      :  1; /* [   25],        r/w,        0x0 */
            uint32_t ten_lf                         :  1; /* [   26],        r/w,        0x0 */
            uint32_t ten_lodist                     :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_dc;

    /* 0x54 : digital test register */
    union {
        struct {
            uint32_t dten_clkpll_postdiv_clk        :  1; /* [    0],        r/w,        0x0 */
            uint32_t dten_clkpll_clk96m             :  1; /* [    1],        r/w,        0x0 */
            uint32_t dten_clkpll_clk32m             :  1; /* [    2],        r/w,        0x0 */
            uint32_t dten_clkpll_fsdm               :  1; /* [    3],        r/w,        0x0 */
            uint32_t dten_clkpll_fref               :  1; /* [    4],        r/w,        0x0 */
            uint32_t dten_clkpll_fin                :  1; /* [    5],        r/w,        0x0 */
            uint32_t dten_lo_fsdm                   :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t dten_lo_fref                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t dtest_pull_down                :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_22                 : 13; /* [22:10],       rsvd,        0x0 */
            uint32_t rf_dtest_en                    :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_dig;

    /* 0x58 : ac test register */
    union {
        struct {
            uint32_t atest_op_cc                    :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t atest_dac_en                   :  1; /* [    4],        r/w,        0x0 */
            uint32_t atest_in_trx_sw                :  1; /* [    5],        r/w,        0x0 */
            uint32_t atest_in_en                    :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t atest_gain_r9                  :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t atest_gain_r8                  :  2; /* [11:10],        r/w,        0x0 */
            uint32_t atest_gain_r7                  :  2; /* [13:12],        r/w,        0x0 */
            uint32_t atest_gain_r6                  :  2; /* [15:14],        r/w,        0x0 */
            uint32_t atest_gain_r5                  :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t atest_out_en_q                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t atest_out_en_i                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t atest_in_en_q                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t atest_in_en_i                  :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ten_ac;

    /* 0x5C : pmip_mv2aon */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pmip_mv2aon;

    /* 0x60 : RX normal bias mode registers */
    union {
        struct {
            uint32_t vg11_sel                       :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t vg13_sel                       :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cip;

    /* 0x64 : pa1 */
    union {
        struct {
            uint32_t pa_iaq                         :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t pa_etb_en                      :  1; /* [    3],        r/w,        0x1 */
            uint32_t pa_iet                         :  4; /* [ 7: 4],        r/w,        0x2 */
            uint32_t pa_vbcore                      :  4; /* [11: 8],        r/w,        0xa */
            uint32_t pa_vbcas                       :  3; /* [14:12],        r/w,        0x3 */
            uint32_t pa_half_on                     :  1; /* [   15],        r/w,        0x0 */
            uint32_t pa_ib_fix                      :  1; /* [   16],        r/w,        0x0 */
            uint32_t pa_lz_bias_en                  :  1; /* [   17],        r/w,        0x1 */
            uint32_t pa_pwrmx_osdac                 :  4; /* [21:18],        r/w,        0x4 */
            uint32_t pa_pwrmx_dac_pn_switch         :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pa_pwrmx_bm                    :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pa_att_gc                      :  4; /* [31:28],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } pa1;

    /* 0x68 : RX normal bias mode registers */
    union {
        struct {
            uint32_t reserved_0_2                   :  3; /* [ 2: 0],       rsvd,        0x0 */
            uint32_t pa_etb_en_hw                   :  1; /* [    3],          r,        0x1 */
            uint32_t pa_iet_hw                      :  4; /* [ 7: 4],          r,        0x2 */
            uint32_t pa_vbcore_hw                   :  4; /* [11: 8],          r,        0xa */
            uint32_t pa_vbcas_hw                    :  3; /* [14:12],          r,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t pa_half_on_hw                  :  1; /* [   16],          r,        0x1 */
            uint32_t pa_ib_fix_hw                   :  1; /* [   17],          r,        0x1 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa2;

    /* 0x6C : tmx */
    union {
        struct {
            uint32_t tmx_cs                         :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t tmx_bm_sw                      :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tmx_bm_cas                     :  3; /* [10: 8],        r/w,        0x6 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t tmx_bm_cas_bulk                :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_tsense_en                   :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tmx;

    /* 0x70 : tbb */
    union {
        struct {
            uint32_t tbb_bm_sf                      :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t tbb_bm_cg                      :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tbb_vcm                        :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t tbb_cflt                       :  2; /* [13:12],        r/w,        0x1 */
            uint32_t tbb_iq_bias_short              :  1; /* [   14],        r/w,        0x0 */
            uint32_t tbb_atest_out_en               :  1; /* [   15],        r/w,        0x0 */
            uint32_t tbb_tosdac_q                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tbb_tosdac_i                   :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb;

    /* 0x74 : lna */
    union {
        struct {
            uint32_t lna_bm                         :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t lna_bm_hw                      :  4; /* [ 7: 4],          r,        0x6 */
            uint32_t lna_load_csw                   :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t lna_load_csw_hw                :  4; /* [15:12],          r,        0x6 */
            uint32_t lna_rfb_match                  :  3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t lna_cap_lg                     :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lna_lg_gsel                    :  3; /* [26:24],        r/w,        0x1 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna;

    /* 0x78 : rmxgm */
    union {
        struct {
            uint32_t rmx_bm                         :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rmxgm_bm                       :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rmxgm_10m_mode_en              :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rmxgm;

    /* 0x7C : rbb1 */
    union {
        struct {
            uint32_t rosdac_q                       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_i                       :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_q_hw                    :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_i_hw                    :  6; /* [29:24],          r,       0x20 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t rosdac_range                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb1;

    /* 0x80 : rbb2 */
    union {
        struct {
            uint32_t rbb_cap2_fc_q                  :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rbb_cap2_fc_i                  :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_q                  :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_i                  :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb2;

    /* 0x84 : rbb3 */
    union {
        struct {
            uint32_t rbb_bt_mode_hw                 :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t rbb_bt_mode                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t rbb_bt_fif_tune                :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_deq                        :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t rbb_bm_op                      :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rbb_vcm                        :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t rbb_bq_iqbias_short            :  1; /* [   20],        r/w,        0x1 */
            uint32_t rbb_tia_iqbias_short           :  1; /* [   21],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_bw                         :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t rxiqcal_en                     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t pwr_det_en                     :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb3;

    /* 0x88 : rbb4 */
    union {
        struct {
            uint32_t rbb_pkdet_vth                  :  4; /* [ 3: 0],        r/w,        0x5 */
            uint32_t rbb_pkdet_out_rstn             :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rbb_pkdet_en                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t rbb_pkdet_out_rstn_hw          :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rbb_pkdet_en_hw                :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pkdet_out_raw                  :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t pkdet_out_latch                :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb4;

    /* 0x8C : adda1 */
    union {
        struct {
            uint32_t dac_dvdd_sel                   :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t dac_bias_sel                   :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t dac_clk_sel                    :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t dac_rccalsel                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t dac_clk_sync_inv               :  1; /* [   13],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t adda_ldo_byps                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adda_ldo_dvdd_sel              :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t adda_ldo_dvdd_sel_hw           :  3; /* [26:24],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adda1;

    /* 0x90 : adda2 */
    union {
        struct {
            uint32_t adc_vref_sel                   :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adc_dly_ctl                    :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t adc_dvdd_sel                   :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adc_sar_ascal_en               :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adc_gt_rm                      :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adc_clk_sync_inv               :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t adc_clk_inv                    :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t adc_clk_div_sel                :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adda2;

    /* 0x94  reserved */
    uint8_t RESERVED0x94[12];

    /* 0xA0 : vco1 */
    union {
        struct {
            uint32_t lo_vco_freq_cw                 :  8; /* [ 7: 0],        r/w,       0x80 */
            uint32_t lo_vco_freq_cw_hw              :  8; /* [15: 8],          r,       0x80 */
            uint32_t lo_vco_idac_cw                 :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_idac_cw_hw              :  5; /* [28:24],          r,        0xf */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } vco1;

    /* 0xA4 : vco2 */
    union {
        struct {
            uint32_t lo_vco_vbias_cw                :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t lo_vco_idac_boot               :  1; /* [    4],        r/w,        0x0 */
            uint32_t lo_vco_short_vbias_filter      :  1; /* [    5],        r/w,        0x0 */
            uint32_t lo_vco_short_idac_filter       :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t acal_vref_cw                   :  3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t acal_vco_ud                    :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t acal_inc_en_hw                 :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } vco2;

    /* 0xA8 : vco3 */
    union {
        struct {
            uint32_t fcal_div                       : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t fcal_cnt_op                    : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } vco3;

    /* 0xAC : vco4 */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t fcal_cnt_start                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t fcal_inc_en_hw                 :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t fcal_inc_large_range           :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t fcal_cnt_rdy                   :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t fcal_inc_vctrl_ud              :  2; /* [25:24],          r,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } vco4;

    /* 0xB0 : pfdcp */
    union {
        struct {
            uint32_t lo_cp_sel                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t lo_cp_sel_hw                   :  1; /* [    4],          r,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_cp_startup_en               :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t lo_cp_ota_en                   :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lo_cp_opamp_en                 :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_cp_hiz                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_pfd_rvdd_boost              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t lo_pfd_rst_csd                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t lo_pfd_rst_csd_hw              :  1; /* [   29],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pfdcp;

    /* 0xB4 : lo */
    union {
        struct {
            uint32_t lo_lf_rz_hw                    :  2; /* [ 1: 0],          r,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t lo_lf_r4_hw                    :  2; /* [ 5: 4],          r,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_lf_cz_hw                    :  2; /* [ 9: 8],          r,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_lf_rz                       :  2; /* [13:12],        r/w,        0x2 */
            uint32_t lo_lf_cz                       :  2; /* [15:14],        r/w,        0x2 */
            uint32_t lo_lf_r4                       :  2; /* [17:16],        r/w,        0x2 */
            uint32_t lo_lf_r4_short                 :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t lo_slipped_dn                  :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_slipped_up                  :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo;

    /* 0xB8 : fbdv */
    union {
        struct {
            uint32_t lo_fbdv_halfstep_en_hw         :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t lo_fbdv_halfstep_en            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_fbdv_sel_sample_clk         :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_fbdv_sel_fb_clk             :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lo_fbdv_rst                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_fbdv_rst_hw                 :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } fbdv;

    /* 0xBC : lodist */
    union {
        struct {
            uint32_t lo_osmx_xgm_boost              :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t lo_osmx_en_xgm                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_osmx_fix_cap                :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t lo_osmx_vbuf_stre              :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lo_osmx_capbank_bias           :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lo_osmx_cap                    :  4; /* [23:20],        r/w,        0x8 */
            uint32_t lo_lodist_txbuf_stre           :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t lo_lodist_rxbuf_stre           :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lodist;

    /* 0xC0 : sdm1 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_hw           :  2; /* [ 1: 0],          r,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t lo_sdm_bypass_hw               :  1; /* [    4],          r,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_sdm_dither_sel              :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_sdm_bypass                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lo_sdm_rstb                    :  1; /* [   16],        r/w,        0x1 */
            uint32_t lo_sdm_rstb_hw                 :  1; /* [   17],          r,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lo_sdm_flag                    :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdm1;

    /* 0xC4 : sdm2 */
    union {
        struct {
            uint32_t lo_sdmin                       : 30; /* [29: 0],        r/w, 0x1501c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdm2;

    /* 0xC8 : sdm3 */
    union {
        struct {
            uint32_t lo_sdmin_hw                    : 30; /* [29: 0],          r,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdm3;

    /* 0xcc  reserved */
    uint8_t RESERVED0xcc[32];

    /* 0xEC : rf_resv_reg_0 */
    union {
        struct {
            uint32_t rf_reserved0                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_resv_reg_0;

    /* 0xF0 : rf_resv_reg_1 */
    union {
        struct {
            uint32_t rf_reserved1                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_resv_reg_1;

    /* 0xF4 : rf_resv_reg_2 */
    union {
        struct {
            uint32_t rf_reserved2                   : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_resv_reg_2;

    /* 0xF8 : rrf_gain_index1 */
    union {
        struct {
            uint32_t gain_ctrl0_gc_rmxgm            :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t gain_ctrl0_gc_lna              :  3; /* [ 4: 2],        r/w,        0x0 */
            uint32_t gain_ctrl1_gc_rmxgm            :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t gain_ctrl1_gc_lna              :  3; /* [ 9: 7],        r/w,        0x1 */
            uint32_t gain_ctrl2_gc_rmxgm            :  2; /* [11:10],        r/w,        0x1 */
            uint32_t gain_ctrl2_gc_lna              :  3; /* [14:12],        r/w,        0x2 */
            uint32_t gain_ctrl3_gc_rmxgm            :  2; /* [16:15],        r/w,        0x1 */
            uint32_t gain_ctrl3_gc_lna              :  3; /* [19:17],        r/w,        0x3 */
            uint32_t gain_ctrl4_gc_rmxgm            :  2; /* [21:20],        r/w,        0x1 */
            uint32_t gain_ctrl4_gc_lna              :  3; /* [24:22],        r/w,        0x4 */
            uint32_t gain_ctrl5_gc_rmxgm            :  2; /* [26:25],        r/w,        0x1 */
            uint32_t gain_ctrl5_gc_lna              :  3; /* [29:27],        r/w,        0x5 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf_gain_index1;

    /* 0xFC : rrf_gain_index2 */
    union {
        struct {
            uint32_t gain_ctrl8_gc_rmxgm            :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t gain_ctrl8_gc_lna              :  3; /* [ 4: 2],        r/w,        0x7 */
            uint32_t gain_ctrl7_gc_rmxgm            :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t gain_ctrl7_gc_lna              :  3; /* [ 9: 7],        r/w,        0x7 */
            uint32_t gain_ctrl6_gc_rmxgm            :  2; /* [11:10],        r/w,        0x1 */
            uint32_t gain_ctrl6_gc_lna              :  3; /* [14:12],        r/w,        0x6 */
            uint32_t reserved_15_31                 : 17; /* [31:15],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rrf_gain_index2;

    /* 0x100 : lna_ctrl_hw_mux */
    union {
        struct {
            uint32_t lna_bm_hg                      :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t lna_bm_lg                      :  4; /* [ 7: 4],        r/w,        0x6 */
            uint32_t lna_load_csw_hg                :  4; /* [11: 8],        r/w,        0x6 */
            uint32_t lna_load_csw_lg                :  4; /* [15:12],        r/w,        0x6 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lna_ctrl_hw_mux;

    /* 0x104 : rbb_gain_index1 */
    union {
        struct {
            uint32_t gain_ctrl0_gc_rbb1             :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl0_gc_rbb2             :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl1_gc_rbb1             :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl1_gc_rbb2             :  3; /* [14:12],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl2_gc_rbb1             :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl2_gc_rbb2             :  3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl3_gc_rbb1             :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl3_gc_rbb2             :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index1;

    /* 0x108 : rbb_gain_index2 */
    union {
        struct {
            uint32_t gain_ctrl4_gc_rbb1             :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl4_gc_rbb2             :  3; /* [ 6: 4],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl5_gc_rbb1             :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl5_gc_rbb2             :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl6_gc_rbb1             :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl6_gc_rbb2             :  3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl7_gc_rbb1             :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl7_gc_rbb2             :  3; /* [30:28],        r/w,        0x4 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index2;

    /* 0x10C : rbb_gain_index3 */
    union {
        struct {
            uint32_t gain_ctrl8_gc_rbb1             :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl8_gc_rbb2             :  3; /* [ 6: 4],        r/w,        0x5 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl9_gc_rbb1             :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl9_gc_rbb2             :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl10_gc_rbb1            :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl10_gc_rbb2            :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl11_gc_rbb1            :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl11_gc_rbb2            :  3; /* [30:28],        r/w,        0x5 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index3;

    /* 0x110 : rbb_gain_index4 */
    union {
        struct {
            uint32_t gain_ctrl12_gc_rbb1            :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl12_gc_rbb2            :  3; /* [ 6: 4],        r/w,        0x6 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t gain_ctrl13_gc_rbb1            :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t gain_ctrl13_gc_rbb2            :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t gain_ctrl14_gc_rbb1            :  2; /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t gain_ctrl14_gc_rbb2            :  3; /* [22:20],        r/w,        0x5 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t gain_ctrl15_gc_rbb1            :  2; /* [25:24],        r/w,        0x3 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t gain_ctrl15_gc_rbb2            :  3; /* [30:28],        r/w,        0x6 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index4;

    /* 0x114 : rbb_gain_index5 */
    union {
        struct {
            uint32_t gain_ctrl16_gc_rbb1            :  2; /* [ 1: 0],        r/w,        0x3 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gain_ctrl16_gc_rbb2            :  3; /* [ 6: 4],        r/w,        0x7 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_index5;

    /* 0x118 : tbb_gain_index1 */
    union {
        struct {
            uint32_t gain_ctrl0_gc_tbb              :  5; /* [ 4: 0],        r/w,        0x4 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gain_ctrl0_gc_tmx              :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t gain_ctrl0_dac_bias_sel        :  2; /* [13:12],        r/w,        0x1 */
            uint32_t gain_ctrl0_gc_tbb_boost        :  2; /* [15:14],        r/w,        0x0 */
            uint32_t gain_ctrl1_gc_tbb              :  5; /* [20:16],        r/w,        0x6 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t gain_ctrl1_gc_tmx              :  3; /* [26:24],        r/w,        0x6 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl1_dac_bias_sel        :  2; /* [29:28],        r/w,        0x1 */
            uint32_t gain_ctrl1_gc_tbb_boost        :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb_gain_index1;

    /* 0x11C : tbb_gain_index2 */
    union {
        struct {
            uint32_t gain_ctrl2_gc_tbb              :  5; /* [ 4: 0],        r/w,        0x7 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gain_ctrl2_gc_tmx              :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t gain_ctrl2_dac_bias_sel        :  2; /* [13:12],        r/w,        0x1 */
            uint32_t gain_ctrl2_gc_tbb_boost        :  2; /* [15:14],        r/w,        0x0 */
            uint32_t gain_ctrl3_gc_tbb              :  5; /* [20:16],        r/w,        0x9 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t gain_ctrl3_gc_tmx              :  3; /* [26:24],        r/w,        0x7 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl3_dac_bias_sel        :  2; /* [29:28],        r/w,        0x1 */
            uint32_t gain_ctrl3_gc_tbb_boost        :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb_gain_index2;

    /* 0x120 : tbb_gain_index3 */
    union {
        struct {
            uint32_t gain_ctrl4_gc_tbb              :  5; /* [ 4: 0],        r/w,        0xc */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gain_ctrl4_gc_tmx              :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t gain_ctrl4_dac_bias_sel        :  2; /* [13:12],        r/w,        0x1 */
            uint32_t gain_ctrl4_gc_tbb_boost        :  2; /* [15:14],        r/w,        0x0 */
            uint32_t gain_ctrl5_gc_tbb              :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t gain_ctrl5_gc_tmx              :  3; /* [26:24],        r/w,        0x7 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl5_dac_bias_sel        :  2; /* [29:28],        r/w,        0x1 */
            uint32_t gain_ctrl5_gc_tbb_boost        :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb_gain_index3;

    /* 0x124 : tbb_gain_index4 */
    union {
        struct {
            uint32_t gain_ctrl6_gc_tbb              :  5; /* [ 4: 0],        r/w,       0x13 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gain_ctrl6_gc_tmx              :  3; /* [10: 8],        r/w,        0x7 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t gain_ctrl6_dac_bias_sel        :  2; /* [13:12],        r/w,        0x1 */
            uint32_t gain_ctrl6_gc_tbb_boost        :  2; /* [15:14],        r/w,        0x0 */
            uint32_t gain_ctrl7_gc_tbb              :  5; /* [20:16],        r/w,       0x18 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t gain_ctrl7_gc_tmx              :  3; /* [26:24],        r/w,        0x7 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl7_dac_bias_sel        :  2; /* [29:28],        r/w,        0x1 */
            uint32_t gain_ctrl7_gc_tbb_boost        :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } tbb_gain_index4;

    /* 0x128 : pa_reg_ctrl_hw1 */
    union {
        struct {
            uint32_t reserved_0_11                  : 12; /* [11: 0],       rsvd,        0x0 */
            uint32_t pa_iet_11n                     :  4; /* [15:12],        r/w,        0x2 */
            uint32_t pa_vbcore_11n                  :  4; /* [19:16],        r/w,        0xa */
            uint32_t pa_vbcas_11n                   :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_reg_ctrl_hw1;

    /* 0x12C : pa_reg_ctrl_hw2 */
    union {
        struct {
            uint32_t pa_iet_11g                     :  4; /* [ 3: 0],        r/w,        0x2 */
            uint32_t pa_vbcore_11g                  :  4; /* [ 7: 4],        r/w,        0xa */
            uint32_t pa_vbcas_11g                   :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t pa_iet_11b                     :  4; /* [15:12],        r/w,        0x4 */
            uint32_t pa_vbcore_11b                  :  4; /* [19:16],        r/w,        0xd */
            uint32_t pa_vbcas_11b                   :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_reg_ctrl_hw2;

    /* 0x130 : pa_reg_wifi_ctrl_hw */
    union {
        struct {
            uint32_t pa_half_on_wifi                :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t pa_etb_en_wifi                 :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t pa_ib_fix_wifi                 :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa_reg_wifi_ctrl_hw;

    /* 0x134 : adda_reg_ctrl_hw */
    union {
        struct {
            uint32_t adda_ldo_dvdd_sel_rx           :  3; /* [ 2: 0],        r/w,        0x2 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t adda_ldo_dvdd_sel_tx           :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adda_reg_ctrl_hw;

    /* 0x138 : lo_reg_ctrl_hw1 */
    union {
        struct {
            uint32_t lo_fbdv_halfstep_en_rx         :  1; /* [    0],        r/w,        0x0 */
            uint32_t lo_fbdv_halfstep_en_tx         :  1; /* [    1],        r/w,        0x1 */
            uint32_t lo_cp_sel_rx                   :  1; /* [    2],        r/w,        0x1 */
            uint32_t lo_cp_sel_tx                   :  1; /* [    3],        r/w,        0x1 */
            uint32_t lo_lf_cz_rx                    :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t lo_lf_cz_tx                    :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lo_lf_rz_rx                    :  2; /* [13:12],        r/w,        0x2 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lo_lf_rz_tx                    :  2; /* [17:16],        r/w,        0x2 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t lo_lf_r4_rx                    :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t lo_lf_r4_tx                    :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_reg_ctrl_hw1;

    /* 0x13C : lo_cal_ctrl_hw1 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2404            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2404            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2408            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2408            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw1;

    /* 0x140 : lo_cal_ctrl_hw2 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2412            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2412            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2416            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2416            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw2;

    /* 0x144 : lo_cal_ctrl_hw3 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2420            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2420            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2424            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2424            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw3;

    /* 0x148 : lo_cal_ctrl_hw4 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2428            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2428            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2432            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2432            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw4;

    /* 0x14C : lo_cal_ctrl_hw5 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2436            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2436            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2440            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2440            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw5;

    /* 0x150 : lo_cal_ctrl_hw6 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2444            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2444            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2448            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2448            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw6;

    /* 0x154 : lo_cal_ctrl_hw7 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2452            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2452            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2456            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2456            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw7;

    /* 0x158 : lo_cal_ctrl_hw8 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2460            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2460            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2464            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2464            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw8;

    /* 0x15C : lo_cal_ctrl_hw9 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2468            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2468            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2472            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2472            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw9;

    /* 0x160 : lo_cal_ctrl_hw10 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2476            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2476            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t lo_vco_idac_cw_2480            :  5; /* [20:16],        r/w,        0xf */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2480            :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw10;

    /* 0x164 : lo_cal_ctrl_hw11 */
    union {
        struct {
            uint32_t lo_vco_idac_cw_2484            :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t lo_vco_freq_cw_2484            :  8; /* [15: 8],        r/w,       0x80 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_cal_ctrl_hw11;

    /* 0x168 : rosdac_ctrl_hw1 */
    union {
        struct {
            uint32_t rosdac_i_gc0                   :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_q_gc0                   :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_i_gc1                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_q_gc1                   :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rosdac_ctrl_hw1;

    /* 0x16C : rosdac_ctrl_hw2 */
    union {
        struct {
            uint32_t rosdac_i_gc2                   :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_q_gc2                   :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_i_gc3                   :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_q_gc3                   :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rosdac_ctrl_hw2;

    /* 0x170 : rxiq_ctrl_hw1 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc0           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc0            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw1;

    /* 0x174 : rxiq_ctrl_hw2 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc1           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc1            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw2;

    /* 0x178 : rxiq_ctrl_hw3 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc2           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc2            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw3;

    /* 0x17C : rxiq_ctrl_hw4 */
    union {
        struct {
            uint32_t rx_iq_phase_comp_gc3           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_iq_gain_comp_gc3            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxiq_ctrl_hw4;

    /* 0x180 : tosdac_ctrl_hw1 */
    union {
        struct {
            uint32_t tbb_tosdac_i_gc0               :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc0               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t tbb_tosdac_i_gc1               :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc1               :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw1;

    /* 0x184 : tosdac_ctrl_hw2 */
    union {
        struct {
            uint32_t tbb_tosdac_i_gc2               :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc2               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t tbb_tosdac_i_gc3               :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc3               :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw2;

    /* 0x188 : tosdac_ctrl_hw3 */
    union {
        struct {
            uint32_t tbb_tosdac_i_gc4               :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc4               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t tbb_tosdac_i_gc5               :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc5               :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw3;

    /* 0x18C : tosdac_ctrl_hw4 */
    union {
        struct {
            uint32_t tbb_tosdac_i_gc6               :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc6               :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t tbb_tosdac_i_gc7               :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tbb_tosdac_q_gc7               :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tosdac_ctrl_hw4;

    /* 0x190 : tx_iq_gain_hw0 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc0           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc0            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw0;

    /* 0x194 : tx_iq_gain_hw1 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc1           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc1            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw1;

    /* 0x198 : tx_iq_gain_hw2 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc2           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc2            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw2;

    /* 0x19C : tx_iq_gain_hw3 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc3           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc3            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw3;

    /* 0x1A0 : tx_iq_gain_hw4 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc4           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc4            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw4;

    /* 0x1A4 : tx_iq_gain_hw5 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc5           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc5            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw5;

    /* 0x1A8 : tx_iq_gain_hw6 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc6           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc6            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw6;

    /* 0x1AC : tx_iq_gain_hw7 */
    union {
        struct {
            uint32_t tx_iq_phase_comp_gc7           : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx_iq_gain_comp_gc7            : 11; /* [26:16],        r/w,      0x400 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tx_iq_gain_hw7;

    /* 0x1B0 : lo_sdm_ctrl_hw1 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_wlan_2412    :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2417    :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2422    :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2427    :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2432    :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2437    :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2442    :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2447    :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2452    :  2; /* [17:16],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2457    :  2; /* [19:18],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2462    :  2; /* [21:20],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2467    :  2; /* [23:22],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2472    :  2; /* [25:24],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_wlan_2484    :  2; /* [27:26],        r/w,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw1;

    /* 0x1B4 : lo_sdm_ctrl_hw2 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_ble_2402     :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2404     :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2406     :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2408     :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2410     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2412     :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2414     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2416     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2418     :  2; /* [17:16],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2420     :  2; /* [19:18],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2422     :  2; /* [21:20],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2424     :  2; /* [23:22],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2426     :  2; /* [25:24],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2428     :  2; /* [27:26],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2430     :  2; /* [29:28],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2432     :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw2;

    /* 0x1B8 : lo_sdm_ctrl_hw3 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_ble_2434     :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2436     :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2438     :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2440     :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2442     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2444     :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2446     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2448     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2450     :  2; /* [17:16],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2452     :  2; /* [19:18],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2454     :  2; /* [21:20],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2456     :  2; /* [23:22],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2458     :  2; /* [25:24],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2460     :  2; /* [27:26],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2462     :  2; /* [29:28],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2464     :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw3;

    /* 0x1BC : lo_sdm_ctrl_hw4 */
    union {
        struct {
            uint32_t lo_sdm_dither_sel_ble_2466     :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2468     :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2470     :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2472     :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2474     :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2476     :  2; /* [11:10],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2478     :  2; /* [13:12],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_2480     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t lo_sdm_dither_sel_ble_tx       :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw4;

    /* 0x1C0 : lo_sdm_ctrl_hw5 */
    union {
        struct {
            uint32_t lo_center_freq_mhz             : 12; /* [11: 0],        r/w,      0x97e */
            uint32_t lo_sdm_bypass_mode             :  6; /* [17:12],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw5;

    /* 0x1C4 : lo_sdm_ctrl_hw6 */
    union {
        struct {
            uint32_t lo_sdmin_center                : 29; /* [28: 0],        r/w, 0x14400000 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw6;

    /* 0x1C8 : lo_sdm_ctrl_hw7 */
    union {
        struct {
            uint32_t lo_sdmin_1m                    : 20; /* [19: 0],        r/w,    0x22222 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw7;

    /* 0x1CC : lo_sdm_ctrl_hw8 */
    union {
        struct {
            uint32_t lo_sdmin_if                    : 20; /* [19: 0],        r/w,    0x2aaab */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_sdm_ctrl_hw8;

    /* 0x1D0 : rbb_bw_ctrl_hw */
    union {
        struct {
            uint32_t rbb_bt_mode_ble                :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_bw_ctrl_hw;

    /* 0x1d4  reserved */
    uint8_t RESERVED0x1d4[56];

    /* 0x20C : singen_ctrl0 */
    union {
        struct {
            uint32_t singen_inc_step1               : 10; /* [ 9: 0],        r/w,        0x1 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t singen_inc_step0               : 10; /* [25:16],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t singen_unsign_en               :  1; /* [   28],        r/w,        0x0 */
            uint32_t singen_clkdiv_n                :  2; /* [30:29],        r/w,        0x0 */
            uint32_t singen_en                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl0;

    /* 0x210 : singen_ctrl1 */
    union {
        struct {
            uint32_t singen_clkdiv_q                : 10; /* [ 9: 0],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t singen_mode_q                  :  4; /* [15:12],        r/w,        0x0 */
            uint32_t singen_clkdiv_i                : 10; /* [25:16],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t singen_mode_i                  :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl1;

    /* 0x214 : singen_ctrl2 */
    union {
        struct {
            uint32_t singen_gain_i                  : 11; /* [10: 0],        r/w,      0x3ff */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t singen_start_addr1_i           : 10; /* [21:12],        r/w,        0x0 */
            uint32_t singen_start_addr0_i           : 10; /* [31:22],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl2;

    /* 0x218 : singen_ctrl3 */
    union {
        struct {
            uint32_t singen_gain_q                  : 11; /* [10: 0],        r/w,      0x3ff */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t singen_start_addr1_q           : 10; /* [21:12],        r/w,        0x0 */
            uint32_t singen_start_addr0_q           : 10; /* [31:22],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl3;

    /* 0x21C : singen_ctrl4 */
    union {
        struct {
            uint32_t singen_fix_q                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t singen_fix_en_q                :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t singen_fix_i                   : 12; /* [27:16],        r/w,        0x0 */
            uint32_t singen_fix_en_i                :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } singen_ctrl4;

    /* 0x220 : rfif_dfe_ctrl0 */
    union {
        struct {
            uint32_t rfckg_rxclk_4s_on              :  1; /* [    0],        r/w,        0x0 */
            uint32_t rfckg_txclk_4s_on              :  1; /* [    1],        r/w,        0x0 */
            uint32_t rfckg_adc_afifo_inv            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rfckg_adc_clkout_sel           :  1; /* [    3],        r/w,        0x0 */
            uint32_t rfckg_dac_afifo_inv            :  1; /* [    4],        r/w,        0x1 */
            uint32_t rx_dfe_en_4s                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t rx_dfe_en_4s_en                :  1; /* [    6],        r/w,        0x0 */
            uint32_t tx_dfe_en_4s                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t tx_dfe_en_4s_en                :  1; /* [    8],        r/w,        0x0 */
            uint32_t rx_test_sel                    :  2; /* [10: 9],        r/w,        0x0 */
            uint32_t tx_test_sel                    :  2; /* [12:11],        r/w,        0x0 */
            uint32_t pad_adc_clkout_inv_en          :  1; /* [   13],        r/w,        0x1 */
            uint32_t pad_dac_clkout_inv_en          :  1; /* [   14],        r/w,        0x0 */
            uint32_t rf_ch_ind_ble_4s               :  7; /* [21:15],        r/w,        0x0 */
            uint32_t rf_ch_ind_ble_4s_en            :  1; /* [   22],        r/w,        0x0 */
            uint32_t wifimode_4s                    :  2; /* [24:23],        r/w,        0x0 */
            uint32_t wifimode_4s_en                 :  1; /* [   25],        r/w,        0x0 */
            uint32_t bbmode_4s                      :  1; /* [   26],        r/w,        0x0 */
            uint32_t bbmode_4s_en                   :  1; /* [   27],        r/w,        0x0 */
            uint32_t test_sel                       :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rfif_dfe_ctrl0;

    /* 0x224 : rfif_test_read */
    union {
        struct {
            uint32_t test_read                      : 32; /* [31: 0],          r,     0x1111 */
        }BF;
        uint32_t WORD;
    } rfif_test_read;

    /* 0x228 : rfif_dig_ctrl */
    union {
        struct {
            uint32_t test_from_pad_en               :  1; /* [    0],        r/w,        0x0 */
            uint32_t test_gc_from_pad_en            :  1; /* [    1],        r/w,        0x0 */
            uint32_t rfckg_rxclk_div2_mode          :  1; /* [    2],        r/w,        0x0 */
            uint32_t rfif_int_lo_unlocked_mask      :  1; /* [    3],        r/w,        0x1 */
            uint32_t reserved_4_15                  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t rfif_ppud_cnt2                 :  9; /* [24:16],        r/w,       0x9a */
            uint32_t rfif_ppud_cnt1                 :  5; /* [29:25],        r/w,        0xf */
            uint32_t rfif_ppud_manaual_en           :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rfif_dig_ctrl;

    /* 0x22C : rf_data_temp_0 */
    union {
        struct {
            uint32_t rf_data_temp_0                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_0;

    /* 0x230 : rf_data_temp_1 */
    union {
        struct {
            uint32_t rf_data_temp_1                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_1;

    /* 0x234 : rf_data_temp_2 */
    union {
        struct {
            uint32_t rf_data_temp_2                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_2;

    /* 0x238 : rf_data_temp_3 */
    union {
        struct {
            uint32_t rf_data_temp_3                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_data_temp_3;

    /* 0x23C : rf_sram_ctrl0 */
    union {
        struct {
            uint32_t rf_sram_link_dly               : 16; /* [15: 0],        r/w,       0x20 */
            uint32_t rf_sram_link_mode              :  2; /* [17:16],        r/w,        0x0 */
            uint32_t rf_sram_swap                   :  1; /* [   18],        r/w,        0x0 */
            uint32_t rf_sram_ext_clr                :  1; /* [   19],        r/w,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl0;

    /* 0x240 : rf_sram_ctrl1 */
    union {
        struct {
            uint32_t rf_sram_adc_done               :  1; /* [    0],          r,        0x0 */
            uint32_t rf_sram_adc_en                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_sram_adc_loop_en            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_sram_adc_sts_clr            :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15                  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t rf_sram_adc_done_cnt           : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl1;

    /* 0x244 : rf_sram_ctrl2 */
    union {
        struct {
            uint32_t rf_sram_adc_addr_end           : 16; /* [15: 0],        r/w,      0xfff */
            uint32_t rf_sram_adc_addr_start         : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl2;

    /* 0x248 : rf_sram_ctrl3 */
    union {
        struct {
            uint32_t rf_sram_adc_sts                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl3;

    /* 0x24C : rf_sram_ctrl4 */
    union {
        struct {
            uint32_t rf_sram_dac_done               :  1; /* [    0],          r,        0x0 */
            uint32_t rf_sram_dac_en                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_sram_dac_loop_en            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_sram_dac_sts_clr            :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15                  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t rf_sram_dac_done_cnt           : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl4;

    /* 0x250 : rf_sram_ctrl5 */
    union {
        struct {
            uint32_t rf_sram_dac_addr_end           : 16; /* [15: 0],        r/w,      0xfff */
            uint32_t rf_sram_dac_addr_start         : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl5;

    /* 0x254 : rf_sram_ctrl6 */
    union {
        struct {
            uint32_t rf_sram_dac_sts                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl6;

    /* 0x258 : rf_ical_ctrl0 */
    union {
        struct {
            uint32_t rf_ical_r_cnt_n                : 10; /* [ 9: 0],        r/w,       0x50 */
            uint32_t rf_ical_a_cnt_n                : 10; /* [19:10],        r/w,       0xa0 */
            uint32_t rf_ical_f_cnt_n                : 10; /* [29:20],        r/w,       0x50 */
            uint32_t rf_ical_a_ud_inv_en            :  1; /* [   30],        r/w,        0x0 */
            uint32_t rf_ical_f_ud_inv_en            :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ical_ctrl0;

    /* 0x25C : rf_ical_ctrl1 */
    union {
        struct {
            uint32_t rf_ical_r_avg_n                :  5; /* [ 4: 0],        r/w,       0x1f */
            uint32_t reserved_5_9                   :  5; /* [ 9: 5],       rsvd,        0x0 */
            uint32_t rf_ical_r_os_q                 : 10; /* [19:10],          r,      0x200 */
            uint32_t rf_ical_r_os_i                 : 10; /* [29:20],          r,      0x200 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ical_ctrl1;

    /* 0x260 : rf_ical_ctrl2 */
    union {
        struct {
            uint32_t rf_ical_period_n               : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ical_ctrl2;

    /* 0x264 : rf_fsm_ctrl0 */
    union {
        struct {
            uint32_t rf_ch_ind_wifi                 : 12; /* [11: 0],        r/w,      0x99c */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl0;

    /* 0x268 : rf_fsm_ctrl1 */
    union {
        struct {
            uint32_t rf_fsm_lo_time                 : 16; /* [15: 0],        r/w,      0xc80 */
            uint32_t rf_fsm_lo_rdy                  :  1; /* [   16],          r,        0x0 */
            uint32_t rf_fsm_lo_rdy_rst              :  1; /* [   17],        r/w,        0x0 */
            uint32_t rf_fsm_lo_rdy_4s_1             :  1; /* [   18],        r/w,        0x0 */
            uint32_t rf_fsm_lo_rdy_sbclr            :  1; /* [   19],        r/w,        0x0 */
            uint32_t rf_fsm_pu_pa_dly_n             : 10; /* [29:20],        r/w,       0x50 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl1;

    /* 0x26C : rf_fsm_ctrl2 */
    union {
        struct {
            uint32_t rf_fsm_st_dbg                  :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t rf_fsm_st_dbg_en               :  1; /* [    3],        r/w,        0x0 */
            uint32_t rf_trx_en_ble_4s               :  1; /* [    4],        r/w,        0x0 */
            uint32_t rf_trx_sw_ble_4s               :  1; /* [    5],        r/w,        0x0 */
            uint32_t rf_trx_ble_4s_en               :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7_9                   :  3; /* [ 9: 7],       rsvd,        0x0 */
            uint32_t rf_fsm_dfe_tx_dly_n            : 10; /* [19:10],        r/w,        0xa */
            uint32_t rf_fsm_dfe_rx_dly_n            : 10; /* [29:20],        r/w,       0xa0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fsm_ctrl2;

    /* 0x270 : rf_pkdet_ctrl0 */
    union {
        struct {
            uint32_t pkdet_out_cnt_sts              :  4; /* [ 3: 0],          r,        0x0 */
            uint32_t pkdet_out_cnt_en               :  1; /* [    4],        r/w,        0x0 */
            uint32_t pkdet_out_mode                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_pkdet_ctrl0;

    /* 0x274  reserved */
    uint8_t RESERVED0x274[908];

    /* 0x600 : dfe_ctrl_0 */
    union {
        struct {
            uint32_t tx_iqc_phase                   : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t tx_iqc_phase_en                :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t tx_iqc_gain                    : 11; /* [22:12],        r/w,      0x400 */
            uint32_t tx_iqc_gain_en                 :  1; /* [   23],        r/w,        0x1 */
            uint32_t tx_dvga_gain_qdb               :  7; /* [30:24],        r/w,        0x0 */
            uint32_t tx_dvga_gain_ctrl_hw           :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_0;

    /* 0x604 : dfe_ctrl_1 */
    union {
        struct {
            uint32_t tx_dac_os_i                    : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t tx_dac_os_q                    : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_29                 :  2; /* [29:28],       rsvd,        0x0 */
            uint32_t tx_dac_dat_format              :  1; /* [   30],        r/w,        0x0 */
            uint32_t tx_dac_iq_swap                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_1;

    /* 0x608 : dfe_ctrl_2 */
    union {
        struct {
            uint32_t rx_adc_os_i                    : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_adc_os_q                    : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t rx_adc_dce_flt_en              :  1; /* [   28],        r/w,        0x1 */
            uint32_t rx_adc_low_pow_en              :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_adc_dat_format              :  1; /* [   30],        r/w,        0x0 */
            uint32_t rx_adc_iq_swap                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_2;

    /* 0x60C : dfe_ctrl_3 */
    union {
        struct {
            uint32_t rx_adc_4s_i_val                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t rx_adc_4s_i_en                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t rx_adc_4s_q_val                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t rx_adc_4s_q_en                 :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_3;

    /* 0x610 : dfe_ctrl_4 */
    union {
        struct {
            uint32_t rx_pf_th2                      : 10; /* [ 9: 0],        r/w,        0xf */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rx_pf_th1                      : 10; /* [25:16],        r/w,       0x7f */
            uint32_t reserved_26_29                 :  4; /* [29:26],       rsvd,        0x0 */
            uint32_t rx_pf_q_en                     :  1; /* [   30],        r/w,        0x0 */
            uint32_t rx_pf_i_en                     :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_4;

    /* 0x614 : dfe_ctrl_5 */
    union {
        struct {
            uint32_t rx_iqc_phase                   : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t rx_iqc_phase_en                :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rx_iqc_gain                    : 11; /* [22:12],        r/w,      0x400 */
            uint32_t rx_iqc_gain_en                 :  1; /* [   23],        r/w,        0x1 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_5;

    /* 0x618 : dfe_ctrl_6 */
    union {
        struct {
            uint32_t rx_pm_freqshift_cw             : 20; /* [19: 0],        r/w,        0x0 */
            uint32_t rx_pm_freqshift_en             :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_27                 :  7; /* [27:21],       rsvd,        0x0 */
            uint32_t rx_pm_done                     :  1; /* [   28],          r,        0x0 */
            uint32_t rx_pm_en                       :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_pm_in_sel                   :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_6;

    /* 0x61C : dfe_ctrl_7 */
    union {
        struct {
            uint32_t rx_pm_start_ofs                : 16; /* [15: 0],        r/w,       0x50 */
            uint32_t rx_pm_acc_len                  : 16; /* [31:16],        r/w,      0x400 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_7;

    /* 0x620 : dfe_ctrl_8 */
    union {
        struct {
            uint32_t rx_pm_iqacc_i                  : 25; /* [24: 0],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_8;

    /* 0x624 : dfe_ctrl_9 */
    union {
        struct {
            uint32_t rx_pm_iqacc_q                  : 25; /* [24: 0],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_9;

    /* 0x628 : dfe_ctrl_10 */
    union {
        struct {
            uint32_t dfe_dac_raw_i                  : 11; /* [10: 0],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t dfe_dac_raw_q                  : 11; /* [26:16],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_10;

    /* 0x62C : dfe_ctrl_11 */
    union {
        struct {
            uint32_t dfe_adc_raw_i                  : 10; /* [ 9: 0],          r,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t dfe_adc_raw_q                  : 10; /* [25:16],          r,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_11;

    /* 0x630 : dfe_ctrl_12 */
    union {
        struct {
            uint32_t tx_dvga_gain_qdb_gc0           :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc1           :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc2           :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc3           :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_12;

    /* 0x634 : dfe_ctrl_13 */
    union {
        struct {
            uint32_t tx_dvga_gain_qdb_gc4           :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc5           :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc6           :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc7           :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_13;

    /* 0x638 : dfe_ctrl_14 */
    union {
        struct {
            uint32_t tx_dvga_gain_qdb_gc8           :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc9           :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc10          :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc11          :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_14;

    /* 0x63C : dfe_ctrl_15 */
    union {
        struct {
            uint32_t tx_dvga_gain_qdb_gc12          :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc13          :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc14          :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_gc15          :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_15;

    /* 0x640 : dfe_ctrl_16 */
    union {
        struct {
            uint32_t rf_tbb_ind_gc0                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc1                 :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc2                 :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc3                 :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc4                 :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc5                 :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc6                 :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc7                 :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_16;

    /* 0x644 : dfe_ctrl_17 */
    union {
        struct {
            uint32_t rf_tbb_ind_gc8                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc9                 :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc10                :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc11                :  3; /* [14:12],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc12                :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc13                :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc14                :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t rf_tbb_ind_gc15                :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_17;

    /* 0x648 : dfe_ctrl_18 */
    union {
        struct {
            uint32_t tx_dvga_gain_qdb_ble_gc0       :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_ble_gc1       :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_dvga_gain_qdb_ble_gc2       :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dfe_ctrl_18;

};

typedef volatile struct rf_reg rf_reg_t;


#endif  /* __RF_REG_H__ */
