/**
  ******************************************************************************
  * @file    rf_reg.h
  * @version V1.2
  * @date    2020-04-30
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
#ifndef  __RF_REG_H__
#define  __RF_REG_H__

#include "bl702.h"

/* 0x0 : Silicon revision */
#define RF_REV_OFFSET                                           (0x0)
#define RF_REV                                                  RF_REV
#define RF_REV_POS                                              (0U)
#define RF_REV_LEN                                              (8U)
#define RF_REV_MSK                                              (((1U<<RF_REV_LEN)-1)<<RF_REV_POS)
#define RF_REV_UMSK                                             (~(((1U<<RF_REV_LEN)-1)<<RF_REV_POS))
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

/* 0x4 : dsp_readback */
#define RF_DSP_READBACK_OFFSET                                  (0x4)
#define RF_CH_IND_HW                                            RF_CH_IND_HW
#define RF_CH_IND_HW_POS                                        (2U)
#define RF_CH_IND_HW_LEN                                        (7U)
#define RF_CH_IND_HW_MSK                                        (((1U<<RF_CH_IND_HW_LEN)-1)<<RF_CH_IND_HW_POS)
#define RF_CH_IND_HW_UMSK                                       (~(((1U<<RF_CH_IND_HW_LEN)-1)<<RF_CH_IND_HW_POS))
#define RF_RBB_IND_HW                                           RF_RBB_IND_HW
#define RF_RBB_IND_HW_POS                                       (9U)
#define RF_RBB_IND_HW_LEN                                       (5U)
#define RF_RBB_IND_HW_MSK                                       (((1U<<RF_RBB_IND_HW_LEN)-1)<<RF_RBB_IND_HW_POS)
#define RF_RBB_IND_HW_UMSK                                      (~(((1U<<RF_RBB_IND_HW_LEN)-1)<<RF_RBB_IND_HW_POS))
#define RF_RBB_BW_IND_HW                                        RF_RBB_BW_IND_HW
#define RF_RBB_BW_IND_HW_POS                                    (14U)
#define RF_RBB_BW_IND_HW_LEN                                    (1U)
#define RF_RBB_BW_IND_HW_MSK                                    (((1U<<RF_RBB_BW_IND_HW_LEN)-1)<<RF_RBB_BW_IND_HW_POS)
#define RF_RBB_BW_IND_HW_UMSK                                   (~(((1U<<RF_RBB_BW_IND_HW_LEN)-1)<<RF_RBB_BW_IND_HW_POS))
#define RF_CH_IND                                               RF_CH_IND
#define RF_CH_IND_POS                                           (16U)
#define RF_CH_IND_LEN                                           (7U)
#define RF_CH_IND_MSK                                           (((1U<<RF_CH_IND_LEN)-1)<<RF_CH_IND_POS)
#define RF_CH_IND_UMSK                                          (~(((1U<<RF_CH_IND_LEN)-1)<<RF_CH_IND_POS))
#define RF_CH_IND_CTRL_HW                                       RF_CH_IND_CTRL_HW
#define RF_CH_IND_CTRL_HW_POS                                   (23U)
#define RF_CH_IND_CTRL_HW_LEN                                   (1U)
#define RF_CH_IND_CTRL_HW_MSK                                   (((1U<<RF_CH_IND_CTRL_HW_LEN)-1)<<RF_CH_IND_CTRL_HW_POS)
#define RF_CH_IND_CTRL_HW_UMSK                                  (~(((1U<<RF_CH_IND_CTRL_HW_LEN)-1)<<RF_CH_IND_CTRL_HW_POS))
#define RF_RBB_IND                                              RF_RBB_IND
#define RF_RBB_IND_POS                                          (24U)
#define RF_RBB_IND_LEN                                          (5U)
#define RF_RBB_IND_MSK                                          (((1U<<RF_RBB_IND_LEN)-1)<<RF_RBB_IND_POS)
#define RF_RBB_IND_UMSK                                         (~(((1U<<RF_RBB_IND_LEN)-1)<<RF_RBB_IND_POS))
#define RF_RBB_IND_CTRL_HW                                      RF_RBB_IND_CTRL_HW
#define RF_RBB_IND_CTRL_HW_POS                                  (29U)
#define RF_RBB_IND_CTRL_HW_LEN                                  (1U)
#define RF_RBB_IND_CTRL_HW_MSK                                  (((1U<<RF_RBB_IND_CTRL_HW_LEN)-1)<<RF_RBB_IND_CTRL_HW_POS)
#define RF_RBB_IND_CTRL_HW_UMSK                                 (~(((1U<<RF_RBB_IND_CTRL_HW_LEN)-1)<<RF_RBB_IND_CTRL_HW_POS))
#define RF_RBB_BW_IND                                           RF_RBB_BW_IND
#define RF_RBB_BW_IND_POS                                       (30U)
#define RF_RBB_BW_IND_LEN                                       (1U)
#define RF_RBB_BW_IND_MSK                                       (((1U<<RF_RBB_BW_IND_LEN)-1)<<RF_RBB_BW_IND_POS)
#define RF_RBB_BW_IND_UMSK                                      (~(((1U<<RF_RBB_BW_IND_LEN)-1)<<RF_RBB_BW_IND_POS))
#define RF_RBB_BW_IND_CTRL_HW                                   RF_RBB_BW_IND_CTRL_HW
#define RF_RBB_BW_IND_CTRL_HW_POS                               (31U)
#define RF_RBB_BW_IND_CTRL_HW_LEN                               (1U)
#define RF_RBB_BW_IND_CTRL_HW_MSK                               (((1U<<RF_RBB_BW_IND_CTRL_HW_LEN)-1)<<RF_RBB_BW_IND_CTRL_HW_POS)
#define RF_RBB_BW_IND_CTRL_HW_UMSK                              (~(((1U<<RF_RBB_BW_IND_CTRL_HW_LEN)-1)<<RF_RBB_BW_IND_CTRL_HW_POS))

/* 0x8 : Control logic switch */
#define RF_CTRL_SOURCE_OFFSET                                   (0x8)
#define RF_PU_CTRL_HW                                           RF_PU_CTRL_HW
#define RF_PU_CTRL_HW_POS                                       (0U)
#define RF_PU_CTRL_HW_LEN                                       (1U)
#define RF_PU_CTRL_HW_MSK                                       (((1U<<RF_PU_CTRL_HW_LEN)-1)<<RF_PU_CTRL_HW_POS)
#define RF_PU_CTRL_HW_UMSK                                      (~(((1U<<RF_PU_CTRL_HW_LEN)-1)<<RF_PU_CTRL_HW_POS))
#define RF_GAIN_CTRL_TX_HW                                      RF_GAIN_CTRL_TX_HW
#define RF_GAIN_CTRL_TX_HW_POS                                  (3U)
#define RF_GAIN_CTRL_TX_HW_LEN                                  (1U)
#define RF_GAIN_CTRL_TX_HW_MSK                                  (((1U<<RF_GAIN_CTRL_TX_HW_LEN)-1)<<RF_GAIN_CTRL_TX_HW_POS)
#define RF_GAIN_CTRL_TX_HW_UMSK                                 (~(((1U<<RF_GAIN_CTRL_TX_HW_LEN)-1)<<RF_GAIN_CTRL_TX_HW_POS))
#define RF_GAIN_CTRL_RX_HW                                      RF_GAIN_CTRL_RX_HW
#define RF_GAIN_CTRL_RX_HW_POS                                  (4U)
#define RF_GAIN_CTRL_RX_HW_LEN                                  (1U)
#define RF_GAIN_CTRL_RX_HW_MSK                                  (((1U<<RF_GAIN_CTRL_RX_HW_LEN)-1)<<RF_GAIN_CTRL_RX_HW_POS)
#define RF_GAIN_CTRL_RX_HW_UMSK                                 (~(((1U<<RF_GAIN_CTRL_RX_HW_LEN)-1)<<RF_GAIN_CTRL_RX_HW_POS))
#define RF_ROSDAC_CTRL_HW                                       RF_ROSDAC_CTRL_HW
#define RF_ROSDAC_CTRL_HW_POS                                   (8U)
#define RF_ROSDAC_CTRL_HW_LEN                                   (1U)
#define RF_ROSDAC_CTRL_HW_MSK                                   (((1U<<RF_ROSDAC_CTRL_HW_LEN)-1)<<RF_ROSDAC_CTRL_HW_POS)
#define RF_ROSDAC_CTRL_HW_UMSK                                  (~(((1U<<RF_ROSDAC_CTRL_HW_LEN)-1)<<RF_ROSDAC_CTRL_HW_POS))
#define RF_ROSDAC_CTRL_RCCAL                                    RF_ROSDAC_CTRL_RCCAL
#define RF_ROSDAC_CTRL_RCCAL_POS                                (9U)
#define RF_ROSDAC_CTRL_RCCAL_LEN                                (1U)
#define RF_ROSDAC_CTRL_RCCAL_MSK                                (((1U<<RF_ROSDAC_CTRL_RCCAL_LEN)-1)<<RF_ROSDAC_CTRL_RCCAL_POS)
#define RF_ROSDAC_CTRL_RCCAL_UMSK                               (~(((1U<<RF_ROSDAC_CTRL_RCCAL_LEN)-1)<<RF_ROSDAC_CTRL_RCCAL_POS))
#define RF_KCAL_RATIO_CTRL_HW                                   RF_KCAL_RATIO_CTRL_HW
#define RF_KCAL_RATIO_CTRL_HW_POS                               (12U)
#define RF_KCAL_RATIO_CTRL_HW_LEN                               (1U)
#define RF_KCAL_RATIO_CTRL_HW_MSK                               (((1U<<RF_KCAL_RATIO_CTRL_HW_LEN)-1)<<RF_KCAL_RATIO_CTRL_HW_POS)
#define RF_KCAL_RATIO_CTRL_HW_UMSK                              (~(((1U<<RF_KCAL_RATIO_CTRL_HW_LEN)-1)<<RF_KCAL_RATIO_CTRL_HW_POS))
#define RF_RBB_BW_CTRL_HW                                       RF_RBB_BW_CTRL_HW
#define RF_RBB_BW_CTRL_HW_POS                                   (16U)
#define RF_RBB_BW_CTRL_HW_LEN                                   (1U)
#define RF_RBB_BW_CTRL_HW_MSK                                   (((1U<<RF_RBB_BW_CTRL_HW_LEN)-1)<<RF_RBB_BW_CTRL_HW_POS)
#define RF_RBB_BW_CTRL_HW_UMSK                                  (~(((1U<<RF_RBB_BW_CTRL_HW_LEN)-1)<<RF_RBB_BW_CTRL_HW_POS))
#define RF_LO_FCW_CTRL_HW                                       RF_LO_FCW_CTRL_HW
#define RF_LO_FCW_CTRL_HW_POS                                   (20U)
#define RF_LO_FCW_CTRL_HW_LEN                                   (1U)
#define RF_LO_FCW_CTRL_HW_MSK                                   (((1U<<RF_LO_FCW_CTRL_HW_LEN)-1)<<RF_LO_FCW_CTRL_HW_POS)
#define RF_LO_FCW_CTRL_HW_UMSK                                  (~(((1U<<RF_LO_FCW_CTRL_HW_LEN)-1)<<RF_LO_FCW_CTRL_HW_POS))
#define RF_INC_FCAL_EN_CTRL_HW                                  RF_INC_FCAL_EN_CTRL_HW
#define RF_INC_FCAL_EN_CTRL_HW_POS                              (24U)
#define RF_INC_FCAL_EN_CTRL_HW_LEN                              (1U)
#define RF_INC_FCAL_EN_CTRL_HW_MSK                              (((1U<<RF_INC_FCAL_EN_CTRL_HW_LEN)-1)<<RF_INC_FCAL_EN_CTRL_HW_POS)
#define RF_INC_FCAL_EN_CTRL_HW_UMSK                             (~(((1U<<RF_INC_FCAL_EN_CTRL_HW_LEN)-1)<<RF_INC_FCAL_EN_CTRL_HW_POS))
#define RF_VCO_IDAC_CTRL_HW                                     RF_VCO_IDAC_CTRL_HW
#define RF_VCO_IDAC_CTRL_HW_POS                                 (28U)
#define RF_VCO_IDAC_CTRL_HW_LEN                                 (1U)
#define RF_VCO_IDAC_CTRL_HW_MSK                                 (((1U<<RF_VCO_IDAC_CTRL_HW_LEN)-1)<<RF_VCO_IDAC_CTRL_HW_POS)
#define RF_VCO_IDAC_CTRL_HW_UMSK                                (~(((1U<<RF_VCO_IDAC_CTRL_HW_LEN)-1)<<RF_VCO_IDAC_CTRL_HW_POS))

/* 0xC : rf calibration state enable in full cal list */
#define RF_CAL_STATE_CTRL_OFFSET                                (0xC)
#define RF_FCAL_STATE_EN                                        RF_FCAL_STATE_EN
#define RF_FCAL_STATE_EN_POS                                    (0U)
#define RF_FCAL_STATE_EN_LEN                                    (1U)
#define RF_FCAL_STATE_EN_MSK                                    (((1U<<RF_FCAL_STATE_EN_LEN)-1)<<RF_FCAL_STATE_EN_POS)
#define RF_FCAL_STATE_EN_UMSK                                   (~(((1U<<RF_FCAL_STATE_EN_LEN)-1)<<RF_FCAL_STATE_EN_POS))
#define RF_ACAL_STATE_EN                                        RF_ACAL_STATE_EN
#define RF_ACAL_STATE_EN_POS                                    (1U)
#define RF_ACAL_STATE_EN_LEN                                    (1U)
#define RF_ACAL_STATE_EN_MSK                                    (((1U<<RF_ACAL_STATE_EN_LEN)-1)<<RF_ACAL_STATE_EN_POS)
#define RF_ACAL_STATE_EN_UMSK                                   (~(((1U<<RF_ACAL_STATE_EN_LEN)-1)<<RF_ACAL_STATE_EN_POS))
#define RF_KCAL_STATE_EN                                        RF_KCAL_STATE_EN
#define RF_KCAL_STATE_EN_POS                                    (2U)
#define RF_KCAL_STATE_EN_LEN                                    (1U)
#define RF_KCAL_STATE_EN_MSK                                    (((1U<<RF_KCAL_STATE_EN_LEN)-1)<<RF_KCAL_STATE_EN_POS)
#define RF_KCAL_STATE_EN_UMSK                                   (~(((1U<<RF_KCAL_STATE_EN_LEN)-1)<<RF_KCAL_STATE_EN_POS))
#define RF_ROSCAL_STATE_EN                                      RF_ROSCAL_STATE_EN
#define RF_ROSCAL_STATE_EN_POS                                  (3U)
#define RF_ROSCAL_STATE_EN_LEN                                  (1U)
#define RF_ROSCAL_STATE_EN_MSK                                  (((1U<<RF_ROSCAL_STATE_EN_LEN)-1)<<RF_ROSCAL_STATE_EN_POS)
#define RF_ROSCAL_STATE_EN_UMSK                                 (~(((1U<<RF_ROSCAL_STATE_EN_LEN)-1)<<RF_ROSCAL_STATE_EN_POS))
#define RF_RCCAL_STATE_EN                                       RF_RCCAL_STATE_EN
#define RF_RCCAL_STATE_EN_POS                                   (4U)
#define RF_RCCAL_STATE_EN_LEN                                   (1U)
#define RF_RCCAL_STATE_EN_MSK                                   (((1U<<RF_RCCAL_STATE_EN_LEN)-1)<<RF_RCCAL_STATE_EN_POS)
#define RF_RCCAL_STATE_EN_UMSK                                  (~(((1U<<RF_RCCAL_STATE_EN_LEN)-1)<<RF_RCCAL_STATE_EN_POS))
#define RF_INC_FCAL_STATE_EN                                    RF_INC_FCAL_STATE_EN
#define RF_INC_FCAL_STATE_EN_POS                                (5U)
#define RF_INC_FCAL_STATE_EN_LEN                                (1U)
#define RF_INC_FCAL_STATE_EN_MSK                                (((1U<<RF_INC_FCAL_STATE_EN_LEN)-1)<<RF_INC_FCAL_STATE_EN_POS)
#define RF_INC_FCAL_STATE_EN_UMSK                               (~(((1U<<RF_INC_FCAL_STATE_EN_LEN)-1)<<RF_INC_FCAL_STATE_EN_POS))
#define RF_INC_ACAL_STATE_EN                                    RF_INC_ACAL_STATE_EN
#define RF_INC_ACAL_STATE_EN_POS                                (6U)
#define RF_INC_ACAL_STATE_EN_LEN                                (1U)
#define RF_INC_ACAL_STATE_EN_MSK                                (((1U<<RF_INC_ACAL_STATE_EN_LEN)-1)<<RF_INC_ACAL_STATE_EN_POS)
#define RF_INC_ACAL_STATE_EN_UMSK                               (~(((1U<<RF_INC_ACAL_STATE_EN_LEN)-1)<<RF_INC_ACAL_STATE_EN_POS))
#define RF_INC_ROSCAL_STATE_EN                                  RF_INC_ROSCAL_STATE_EN
#define RF_INC_ROSCAL_STATE_EN_POS                              (7U)
#define RF_INC_ROSCAL_STATE_EN_LEN                              (1U)
#define RF_INC_ROSCAL_STATE_EN_MSK                              (((1U<<RF_INC_ROSCAL_STATE_EN_LEN)-1)<<RF_INC_ROSCAL_STATE_EN_POS)
#define RF_INC_ROSCAL_STATE_EN_UMSK                             (~(((1U<<RF_INC_ROSCAL_STATE_EN_LEN)-1)<<RF_INC_ROSCAL_STATE_EN_POS))

/* 0x10 : Calibration mode register */
#define RF_CAL_SWITCH_CTRL_OFFSET                               (0x10)
#define RF_ACAL_EN                                              RF_ACAL_EN
#define RF_ACAL_EN_POS                                          (0U)
#define RF_ACAL_EN_LEN                                          (1U)
#define RF_ACAL_EN_MSK                                          (((1U<<RF_ACAL_EN_LEN)-1)<<RF_ACAL_EN_POS)
#define RF_ACAL_EN_UMSK                                         (~(((1U<<RF_ACAL_EN_LEN)-1)<<RF_ACAL_EN_POS))
#define RF_KCAL_EN                                              RF_KCAL_EN
#define RF_KCAL_EN_POS                                          (4U)
#define RF_KCAL_EN_LEN                                          (1U)
#define RF_KCAL_EN_MSK                                          (((1U<<RF_KCAL_EN_LEN)-1)<<RF_KCAL_EN_POS)
#define RF_KCAL_EN_UMSK                                         (~(((1U<<RF_KCAL_EN_LEN)-1)<<RF_KCAL_EN_POS))
#define RF_RCCAL_EN                                             RF_RCCAL_EN
#define RF_RCCAL_EN_POS                                         (8U)
#define RF_RCCAL_EN_LEN                                         (1U)
#define RF_RCCAL_EN_MSK                                         (((1U<<RF_RCCAL_EN_LEN)-1)<<RF_RCCAL_EN_POS)
#define RF_RCCAL_EN_UMSK                                        (~(((1U<<RF_RCCAL_EN_LEN)-1)<<RF_RCCAL_EN_POS))
#define RF_INC_ACAL_EN                                          RF_INC_ACAL_EN
#define RF_INC_ACAL_EN_POS                                      (12U)
#define RF_INC_ACAL_EN_LEN                                      (1U)
#define RF_INC_ACAL_EN_MSK                                      (((1U<<RF_INC_ACAL_EN_LEN)-1)<<RF_INC_ACAL_EN_POS)
#define RF_INC_ACAL_EN_UMSK                                     (~(((1U<<RF_INC_ACAL_EN_LEN)-1)<<RF_INC_ACAL_EN_POS))
#define RF_INC_FCAL_EN                                          RF_INC_FCAL_EN
#define RF_INC_FCAL_EN_POS                                      (16U)
#define RF_INC_FCAL_EN_LEN                                      (1U)
#define RF_INC_FCAL_EN_MSK                                      (((1U<<RF_INC_FCAL_EN_LEN)-1)<<RF_INC_FCAL_EN_POS)
#define RF_INC_FCAL_EN_UMSK                                     (~(((1U<<RF_INC_FCAL_EN_LEN)-1)<<RF_INC_FCAL_EN_POS))
#define RF_INC_FCAL_EN_HW                                       RF_INC_FCAL_EN_HW
#define RF_INC_FCAL_EN_HW_POS                                   (17U)
#define RF_INC_FCAL_EN_HW_LEN                                   (1U)
#define RF_INC_FCAL_EN_HW_MSK                                   (((1U<<RF_INC_FCAL_EN_HW_LEN)-1)<<RF_INC_FCAL_EN_HW_POS)
#define RF_INC_FCAL_EN_HW_UMSK                                  (~(((1U<<RF_INC_FCAL_EN_HW_LEN)-1)<<RF_INC_FCAL_EN_HW_POS))

/* 0x14 : rf_cal_status */
#define RF_CAL_STATUS_OFFSET                                    (0x14)
#define RF_ACAL_STATUS                                          RF_ACAL_STATUS
#define RF_ACAL_STATUS_POS                                      (2U)
#define RF_ACAL_STATUS_LEN                                      (2U)
#define RF_ACAL_STATUS_MSK                                      (((1U<<RF_ACAL_STATUS_LEN)-1)<<RF_ACAL_STATUS_POS)
#define RF_ACAL_STATUS_UMSK                                     (~(((1U<<RF_ACAL_STATUS_LEN)-1)<<RF_ACAL_STATUS_POS))
#define RF_KCAL_STATUS                                          RF_KCAL_STATUS
#define RF_KCAL_STATUS_POS                                      (4U)
#define RF_KCAL_STATUS_LEN                                      (2U)
#define RF_KCAL_STATUS_MSK                                      (((1U<<RF_KCAL_STATUS_LEN)-1)<<RF_KCAL_STATUS_POS)
#define RF_KCAL_STATUS_UMSK                                     (~(((1U<<RF_KCAL_STATUS_LEN)-1)<<RF_KCAL_STATUS_POS))
#define RF_ROSCAL_STATUS                                        RF_ROSCAL_STATUS
#define RF_ROSCAL_STATUS_POS                                    (6U)
#define RF_ROSCAL_STATUS_LEN                                    (2U)
#define RF_ROSCAL_STATUS_MSK                                    (((1U<<RF_ROSCAL_STATUS_LEN)-1)<<RF_ROSCAL_STATUS_POS)
#define RF_ROSCAL_STATUS_UMSK                                   (~(((1U<<RF_ROSCAL_STATUS_LEN)-1)<<RF_ROSCAL_STATUS_POS))
#define RF_RCCAL_STATUS                                         RF_RCCAL_STATUS
#define RF_RCCAL_STATUS_POS                                     (8U)
#define RF_RCCAL_STATUS_LEN                                     (2U)
#define RF_RCCAL_STATUS_MSK                                     (((1U<<RF_RCCAL_STATUS_LEN)-1)<<RF_RCCAL_STATUS_POS)
#define RF_RCCAL_STATUS_UMSK                                    (~(((1U<<RF_RCCAL_STATUS_LEN)-1)<<RF_RCCAL_STATUS_POS))
#define RF_DL_RFCAL_TABLE_STATUS                                RF_DL_RFCAL_TABLE_STATUS
#define RF_DL_RFCAL_TABLE_STATUS_POS                            (30U)
#define RF_DL_RFCAL_TABLE_STATUS_LEN                            (2U)
#define RF_DL_RFCAL_TABLE_STATUS_MSK                            (((1U<<RF_DL_RFCAL_TABLE_STATUS_LEN)-1)<<RF_DL_RFCAL_TABLE_STATUS_POS)
#define RF_DL_RFCAL_TABLE_STATUS_UMSK                           (~(((1U<<RF_DL_RFCAL_TABLE_STATUS_LEN)-1)<<RF_DL_RFCAL_TABLE_STATUS_POS))

/* 0x18 : pu_delay_confg */
#define RF_PU_DELAY_CONFG_OFFSET                                (0x18)
#define RF_PPU_LEAD                                             RF_PPU_LEAD
#define RF_PPU_LEAD_POS                                         (0U)
#define RF_PPU_LEAD_LEN                                         (2U)
#define RF_PPU_LEAD_MSK                                         (((1U<<RF_PPU_LEAD_LEN)-1)<<RF_PPU_LEAD_POS)
#define RF_PPU_LEAD_UMSK                                        (~(((1U<<RF_PPU_LEAD_LEN)-1)<<RF_PPU_LEAD_POS))
#define RF_PUD_DELAY                                            RF_PUD_DELAY
#define RF_PUD_DELAY_POS                                        (4U)
#define RF_PUD_DELAY_LEN                                        (2U)
#define RF_PUD_DELAY_MSK                                        (((1U<<RF_PUD_DELAY_LEN)-1)<<RF_PUD_DELAY_POS)
#define RF_PUD_DELAY_UMSK                                       (~(((1U<<RF_PUD_DELAY_LEN)-1)<<RF_PUD_DELAY_POS))
#define RF_LO_RESET_DELAY                                       RF_LO_RESET_DELAY
#define RF_LO_RESET_DELAY_POS                                   (12U)
#define RF_LO_RESET_DELAY_LEN                                   (2U)
#define RF_LO_RESET_DELAY_MSK                                   (((1U<<RF_LO_RESET_DELAY_LEN)-1)<<RF_LO_RESET_DELAY_POS)
#define RF_LO_RESET_DELAY_UMSK                                  (~(((1U<<RF_LO_RESET_DELAY_LEN)-1)<<RF_LO_RESET_DELAY_POS))
#define RF_LO_RESET_WIDTH                                       RF_LO_RESET_WIDTH
#define RF_LO_RESET_WIDTH_POS                                   (16U)
#define RF_LO_RESET_WIDTH_LEN                                   (2U)
#define RF_LO_RESET_WIDTH_MSK                                   (((1U<<RF_LO_RESET_WIDTH_LEN)-1)<<RF_LO_RESET_WIDTH_POS)
#define RF_LO_RESET_WIDTH_UMSK                                  (~(((1U<<RF_LO_RESET_WIDTH_LEN)-1)<<RF_LO_RESET_WIDTH_POS))
#define RF_ADPLL_RESET_WIDTH                                    RF_ADPLL_RESET_WIDTH
#define RF_ADPLL_RESET_WIDTH_POS                                (20U)
#define RF_ADPLL_RESET_WIDTH_LEN                                (2U)
#define RF_ADPLL_RESET_WIDTH_MSK                                (((1U<<RF_ADPLL_RESET_WIDTH_LEN)-1)<<RF_ADPLL_RESET_WIDTH_POS)
#define RF_ADPLL_RESET_WIDTH_UMSK                               (~(((1U<<RF_ADPLL_RESET_WIDTH_LEN)-1)<<RF_ADPLL_RESET_WIDTH_POS))

/* 0x100 : Register control of power up signals */
#define RF_PUCR_REG_OFFSET                                      (0x100)
#define RF_LODIST_TX_EN                                         RF_LODIST_TX_EN
#define RF_LODIST_TX_EN_POS                                     (0U)
#define RF_LODIST_TX_EN_LEN                                     (1U)
#define RF_LODIST_TX_EN_MSK                                     (((1U<<RF_LODIST_TX_EN_LEN)-1)<<RF_LODIST_TX_EN_POS)
#define RF_LODIST_TX_EN_UMSK                                    (~(((1U<<RF_LODIST_TX_EN_LEN)-1)<<RF_LODIST_TX_EN_POS))
#define RF_PU_TXBUF                                             RF_PU_TXBUF
#define RF_PU_TXBUF_POS                                         (1U)
#define RF_PU_TXBUF_LEN                                         (1U)
#define RF_PU_TXBUF_MSK                                         (((1U<<RF_PU_TXBUF_LEN)-1)<<RF_PU_TXBUF_POS)
#define RF_PU_TXBUF_UMSK                                        (~(((1U<<RF_PU_TXBUF_LEN)-1)<<RF_PU_TXBUF_POS))
#define RF_PU_RXBUF                                             RF_PU_RXBUF
#define RF_PU_RXBUF_POS                                         (2U)
#define RF_PU_RXBUF_LEN                                         (1U)
#define RF_PU_RXBUF_MSK                                         (((1U<<RF_PU_RXBUF_LEN)-1)<<RF_PU_RXBUF_POS)
#define RF_PU_RXBUF_UMSK                                        (~(((1U<<RF_PU_RXBUF_LEN)-1)<<RF_PU_RXBUF_POS))
#define RF_PU_DTC                                               RF_PU_DTC
#define RF_PU_DTC_POS                                           (3U)
#define RF_PU_DTC_LEN                                           (1U)
#define RF_PU_DTC_MSK                                           (((1U<<RF_PU_DTC_LEN)-1)<<RF_PU_DTC_POS)
#define RF_PU_DTC_UMSK                                          (~(((1U<<RF_PU_DTC_LEN)-1)<<RF_PU_DTC_POS))
#define RF_PU_ADPLL_SFREG                                       RF_PU_ADPLL_SFREG
#define RF_PU_ADPLL_SFREG_POS                                   (4U)
#define RF_PU_ADPLL_SFREG_LEN                                   (1U)
#define RF_PU_ADPLL_SFREG_MSK                                   (((1U<<RF_PU_ADPLL_SFREG_LEN)-1)<<RF_PU_ADPLL_SFREG_POS)
#define RF_PU_ADPLL_SFREG_UMSK                                  (~(((1U<<RF_PU_ADPLL_SFREG_LEN)-1)<<RF_PU_ADPLL_SFREG_POS))
#define RF_PU_ADPLL_ADC                                         RF_PU_ADPLL_ADC
#define RF_PU_ADPLL_ADC_POS                                     (5U)
#define RF_PU_ADPLL_ADC_LEN                                     (1U)
#define RF_PU_ADPLL_ADC_MSK                                     (((1U<<RF_PU_ADPLL_ADC_LEN)-1)<<RF_PU_ADPLL_ADC_POS)
#define RF_PU_ADPLL_ADC_UMSK                                    (~(((1U<<RF_PU_ADPLL_ADC_LEN)-1)<<RF_PU_ADPLL_ADC_POS))
#define RF_ADPLL_CLK_EN                                         RF_ADPLL_CLK_EN
#define RF_ADPLL_CLK_EN_POS                                     (6U)
#define RF_ADPLL_CLK_EN_LEN                                     (1U)
#define RF_ADPLL_CLK_EN_MSK                                     (((1U<<RF_ADPLL_CLK_EN_LEN)-1)<<RF_ADPLL_CLK_EN_POS)
#define RF_ADPLL_CLK_EN_UMSK                                    (~(((1U<<RF_ADPLL_CLK_EN_LEN)-1)<<RF_ADPLL_CLK_EN_POS))
#define RF_LOTPM_HFP_BYPASS                                     RF_LOTPM_HFP_BYPASS
#define RF_LOTPM_HFP_BYPASS_POS                                 (7U)
#define RF_LOTPM_HFP_BYPASS_LEN                                 (1U)
#define RF_LOTPM_HFP_BYPASS_MSK                                 (((1U<<RF_LOTPM_HFP_BYPASS_LEN)-1)<<RF_LOTPM_HFP_BYPASS_POS)
#define RF_LOTPM_HFP_BYPASS_UMSK                                (~(((1U<<RF_LOTPM_HFP_BYPASS_LEN)-1)<<RF_LOTPM_HFP_BYPASS_POS))
#define RF_LOTPM_LFP_BYPASS                                     RF_LOTPM_LFP_BYPASS
#define RF_LOTPM_LFP_BYPASS_POS                                 (8U)
#define RF_LOTPM_LFP_BYPASS_LEN                                 (1U)
#define RF_LOTPM_LFP_BYPASS_MSK                                 (((1U<<RF_LOTPM_LFP_BYPASS_LEN)-1)<<RF_LOTPM_LFP_BYPASS_POS)
#define RF_LOTPM_LFP_BYPASS_UMSK                                (~(((1U<<RF_LOTPM_LFP_BYPASS_LEN)-1)<<RF_LOTPM_LFP_BYPASS_POS))
#define RF_LOTPM_HFP_CLK_EN                                     RF_LOTPM_HFP_CLK_EN
#define RF_LOTPM_HFP_CLK_EN_POS                                 (9U)
#define RF_LOTPM_HFP_CLK_EN_LEN                                 (1U)
#define RF_LOTPM_HFP_CLK_EN_MSK                                 (((1U<<RF_LOTPM_HFP_CLK_EN_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_POS)
#define RF_LOTPM_HFP_CLK_EN_UMSK                                (~(((1U<<RF_LOTPM_HFP_CLK_EN_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_POS))
#define RF_PU_FBDV_BUF                                          RF_PU_FBDV_BUF
#define RF_PU_FBDV_BUF_POS                                      (10U)
#define RF_PU_FBDV_BUF_LEN                                      (1U)
#define RF_PU_FBDV_BUF_MSK                                      (((1U<<RF_PU_FBDV_BUF_LEN)-1)<<RF_PU_FBDV_BUF_POS)
#define RF_PU_FBDV_BUF_UMSK                                     (~(((1U<<RF_PU_FBDV_BUF_LEN)-1)<<RF_PU_FBDV_BUF_POS))
#define RF_PU_FBDV                                              RF_PU_FBDV
#define RF_PU_FBDV_POS                                          (11U)
#define RF_PU_FBDV_LEN                                          (1U)
#define RF_PU_FBDV_MSK                                          (((1U<<RF_PU_FBDV_LEN)-1)<<RF_PU_FBDV_POS)
#define RF_PU_FBDV_UMSK                                         (~(((1U<<RF_PU_FBDV_LEN)-1)<<RF_PU_FBDV_POS))
#define RF_PU_VCO                                               RF_PU_VCO
#define RF_PU_VCO_POS                                           (12U)
#define RF_PU_VCO_LEN                                           (1U)
#define RF_PU_VCO_MSK                                           (((1U<<RF_PU_VCO_LEN)-1)<<RF_PU_VCO_POS)
#define RF_PU_VCO_UMSK                                          (~(((1U<<RF_PU_VCO_LEN)-1)<<RF_PU_VCO_POS))
#define RF_PU_VCO_LDO                                           RF_PU_VCO_LDO
#define RF_PU_VCO_LDO_POS                                       (13U)
#define RF_PU_VCO_LDO_LEN                                       (1U)
#define RF_PU_VCO_LDO_MSK                                       (((1U<<RF_PU_VCO_LDO_LEN)-1)<<RF_PU_VCO_LDO_POS)
#define RF_PU_VCO_LDO_UMSK                                      (~(((1U<<RF_PU_VCO_LDO_LEN)-1)<<RF_PU_VCO_LDO_POS))
#define RF_PU_LODIST_BODY_BIAS                                  RF_PU_LODIST_BODY_BIAS
#define RF_PU_LODIST_BODY_BIAS_POS                              (14U)
#define RF_PU_LODIST_BODY_BIAS_LEN                              (1U)
#define RF_PU_LODIST_BODY_BIAS_MSK                              (((1U<<RF_PU_LODIST_BODY_BIAS_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_POS)
#define RF_PU_LODIST_BODY_BIAS_UMSK                             (~(((1U<<RF_PU_LODIST_BODY_BIAS_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_POS))
#define RF_RXADC_CLK_EN                                         RF_RXADC_CLK_EN
#define RF_RXADC_CLK_EN_POS                                     (15U)
#define RF_RXADC_CLK_EN_LEN                                     (1U)
#define RF_RXADC_CLK_EN_MSK                                     (((1U<<RF_RXADC_CLK_EN_LEN)-1)<<RF_RXADC_CLK_EN_POS)
#define RF_RXADC_CLK_EN_UMSK                                    (~(((1U<<RF_RXADC_CLK_EN_LEN)-1)<<RF_RXADC_CLK_EN_POS))
#define RF_PU_RXADC                                             RF_PU_RXADC
#define RF_PU_RXADC_POS                                         (16U)
#define RF_PU_RXADC_LEN                                         (1U)
#define RF_PU_RXADC_MSK                                         (((1U<<RF_PU_RXADC_LEN)-1)<<RF_PU_RXADC_POS)
#define RF_PU_RXADC_UMSK                                        (~(((1U<<RF_PU_RXADC_LEN)-1)<<RF_PU_RXADC_POS))
#define RF_PU_ROSDAC                                            RF_PU_ROSDAC
#define RF_PU_ROSDAC_POS                                        (17U)
#define RF_PU_ROSDAC_LEN                                        (1U)
#define RF_PU_ROSDAC_MSK                                        (((1U<<RF_PU_ROSDAC_LEN)-1)<<RF_PU_ROSDAC_POS)
#define RF_PU_ROSDAC_UMSK                                       (~(((1U<<RF_PU_ROSDAC_LEN)-1)<<RF_PU_ROSDAC_POS))
#define RF_PU_RBB_PKDET                                         RF_PU_RBB_PKDET
#define RF_PU_RBB_PKDET_POS                                     (18U)
#define RF_PU_RBB_PKDET_LEN                                     (1U)
#define RF_PU_RBB_PKDET_MSK                                     (((1U<<RF_PU_RBB_PKDET_LEN)-1)<<RF_PU_RBB_PKDET_POS)
#define RF_PU_RBB_PKDET_UMSK                                    (~(((1U<<RF_PU_RBB_PKDET_LEN)-1)<<RF_PU_RBB_PKDET_POS))
#define RF_PU_RBB                                               RF_PU_RBB
#define RF_PU_RBB_POS                                           (19U)
#define RF_PU_RBB_LEN                                           (1U)
#define RF_PU_RBB_MSK                                           (((1U<<RF_PU_RBB_LEN)-1)<<RF_PU_RBB_POS)
#define RF_PU_RBB_UMSK                                          (~(((1U<<RF_PU_RBB_LEN)-1)<<RF_PU_RBB_POS))
#define RF_PU_RMX                                               RF_PU_RMX
#define RF_PU_RMX_POS                                           (20U)
#define RF_PU_RMX_LEN                                           (1U)
#define RF_PU_RMX_MSK                                           (((1U<<RF_PU_RMX_LEN)-1)<<RF_PU_RMX_POS)
#define RF_PU_RMX_UMSK                                          (~(((1U<<RF_PU_RMX_LEN)-1)<<RF_PU_RMX_POS))
#define RF_PU_LNA                                               RF_PU_LNA
#define RF_PU_LNA_POS                                           (21U)
#define RF_PU_LNA_LEN                                           (1U)
#define RF_PU_LNA_MSK                                           (((1U<<RF_PU_LNA_LEN)-1)<<RF_PU_LNA_POS)
#define RF_PU_LNA_UMSK                                          (~(((1U<<RF_PU_LNA_LEN)-1)<<RF_PU_LNA_POS))
#define RF_PU_PA                                                RF_PU_PA
#define RF_PU_PA_POS                                            (22U)
#define RF_PU_PA_LEN                                            (1U)
#define RF_PU_PA_MSK                                            (((1U<<RF_PU_PA_LEN)-1)<<RF_PU_PA_POS)
#define RF_PU_PA_UMSK                                           (~(((1U<<RF_PU_PA_LEN)-1)<<RF_PU_PA_POS))
#define RF_RX_BYPASS_EN                                         RF_RX_BYPASS_EN
#define RF_RX_BYPASS_EN_POS                                     (23U)
#define RF_RX_BYPASS_EN_LEN                                     (1U)
#define RF_RX_BYPASS_EN_MSK                                     (((1U<<RF_RX_BYPASS_EN_LEN)-1)<<RF_RX_BYPASS_EN_POS)
#define RF_RX_BYPASS_EN_UMSK                                    (~(((1U<<RF_RX_BYPASS_EN_LEN)-1)<<RF_RX_BYPASS_EN_POS))
#define RF_PA_SERI_CAP_EN                                       RF_PA_SERI_CAP_EN
#define RF_PA_SERI_CAP_EN_POS                                   (24U)
#define RF_PA_SERI_CAP_EN_LEN                                   (1U)
#define RF_PA_SERI_CAP_EN_MSK                                   (((1U<<RF_PA_SERI_CAP_EN_LEN)-1)<<RF_PA_SERI_CAP_EN_POS)
#define RF_PA_SERI_CAP_EN_UMSK                                  (~(((1U<<RF_PA_SERI_CAP_EN_LEN)-1)<<RF_PA_SERI_CAP_EN_POS))
#define RF_RST_ADPLL                                            RF_RST_ADPLL
#define RF_RST_ADPLL_POS                                        (25U)
#define RF_RST_ADPLL_LEN                                        (1U)
#define RF_RST_ADPLL_MSK                                        (((1U<<RF_RST_ADPLL_LEN)-1)<<RF_RST_ADPLL_POS)
#define RF_RST_ADPLL_UMSK                                       (~(((1U<<RF_RST_ADPLL_LEN)-1)<<RF_RST_ADPLL_POS))
#define RF_RST_LOTPM_HFP                                        RF_RST_LOTPM_HFP
#define RF_RST_LOTPM_HFP_POS                                    (26U)
#define RF_RST_LOTPM_HFP_LEN                                    (1U)
#define RF_RST_LOTPM_HFP_MSK                                    (((1U<<RF_RST_LOTPM_HFP_LEN)-1)<<RF_RST_LOTPM_HFP_POS)
#define RF_RST_LOTPM_HFP_UMSK                                   (~(((1U<<RF_RST_LOTPM_HFP_LEN)-1)<<RF_RST_LOTPM_HFP_POS))
#define RF_RST_FBDV                                             RF_RST_FBDV
#define RF_RST_FBDV_POS                                         (27U)
#define RF_RST_FBDV_LEN                                         (1U)
#define RF_RST_FBDV_MSK                                         (((1U<<RF_RST_FBDV_LEN)-1)<<RF_RST_FBDV_POS)
#define RF_RST_FBDV_UMSK                                        (~(((1U<<RF_RST_FBDV_LEN)-1)<<RF_RST_FBDV_POS))

/* 0x104 : Power up setting in SB state */
#define RF_PUCR_SB_OFFSET                                       (0x104)
#define RF_LODIST_TX_EN_SB                                      RF_LODIST_TX_EN_SB
#define RF_LODIST_TX_EN_SB_POS                                  (0U)
#define RF_LODIST_TX_EN_SB_LEN                                  (1U)
#define RF_LODIST_TX_EN_SB_MSK                                  (((1U<<RF_LODIST_TX_EN_SB_LEN)-1)<<RF_LODIST_TX_EN_SB_POS)
#define RF_LODIST_TX_EN_SB_UMSK                                 (~(((1U<<RF_LODIST_TX_EN_SB_LEN)-1)<<RF_LODIST_TX_EN_SB_POS))
#define RF_PU_TXBUF_SB                                          RF_PU_TXBUF_SB
#define RF_PU_TXBUF_SB_POS                                      (1U)
#define RF_PU_TXBUF_SB_LEN                                      (1U)
#define RF_PU_TXBUF_SB_MSK                                      (((1U<<RF_PU_TXBUF_SB_LEN)-1)<<RF_PU_TXBUF_SB_POS)
#define RF_PU_TXBUF_SB_UMSK                                     (~(((1U<<RF_PU_TXBUF_SB_LEN)-1)<<RF_PU_TXBUF_SB_POS))
#define RF_PU_RXBUF_SB                                          RF_PU_RXBUF_SB
#define RF_PU_RXBUF_SB_POS                                      (2U)
#define RF_PU_RXBUF_SB_LEN                                      (1U)
#define RF_PU_RXBUF_SB_MSK                                      (((1U<<RF_PU_RXBUF_SB_LEN)-1)<<RF_PU_RXBUF_SB_POS)
#define RF_PU_RXBUF_SB_UMSK                                     (~(((1U<<RF_PU_RXBUF_SB_LEN)-1)<<RF_PU_RXBUF_SB_POS))
#define RF_PU_DTC_SB                                            RF_PU_DTC_SB
#define RF_PU_DTC_SB_POS                                        (3U)
#define RF_PU_DTC_SB_LEN                                        (1U)
#define RF_PU_DTC_SB_MSK                                        (((1U<<RF_PU_DTC_SB_LEN)-1)<<RF_PU_DTC_SB_POS)
#define RF_PU_DTC_SB_UMSK                                       (~(((1U<<RF_PU_DTC_SB_LEN)-1)<<RF_PU_DTC_SB_POS))
#define RF_PU_ADPLL_SFREG_SB                                    RF_PU_ADPLL_SFREG_SB
#define RF_PU_ADPLL_SFREG_SB_POS                                (4U)
#define RF_PU_ADPLL_SFREG_SB_LEN                                (1U)
#define RF_PU_ADPLL_SFREG_SB_MSK                                (((1U<<RF_PU_ADPLL_SFREG_SB_LEN)-1)<<RF_PU_ADPLL_SFREG_SB_POS)
#define RF_PU_ADPLL_SFREG_SB_UMSK                               (~(((1U<<RF_PU_ADPLL_SFREG_SB_LEN)-1)<<RF_PU_ADPLL_SFREG_SB_POS))
#define RF_PU_ADPLL_ADC_SB                                      RF_PU_ADPLL_ADC_SB
#define RF_PU_ADPLL_ADC_SB_POS                                  (5U)
#define RF_PU_ADPLL_ADC_SB_LEN                                  (1U)
#define RF_PU_ADPLL_ADC_SB_MSK                                  (((1U<<RF_PU_ADPLL_ADC_SB_LEN)-1)<<RF_PU_ADPLL_ADC_SB_POS)
#define RF_PU_ADPLL_ADC_SB_UMSK                                 (~(((1U<<RF_PU_ADPLL_ADC_SB_LEN)-1)<<RF_PU_ADPLL_ADC_SB_POS))
#define RF_ADPLL_CLK_EN_SB                                      RF_ADPLL_CLK_EN_SB
#define RF_ADPLL_CLK_EN_SB_POS                                  (6U)
#define RF_ADPLL_CLK_EN_SB_LEN                                  (1U)
#define RF_ADPLL_CLK_EN_SB_MSK                                  (((1U<<RF_ADPLL_CLK_EN_SB_LEN)-1)<<RF_ADPLL_CLK_EN_SB_POS)
#define RF_ADPLL_CLK_EN_SB_UMSK                                 (~(((1U<<RF_ADPLL_CLK_EN_SB_LEN)-1)<<RF_ADPLL_CLK_EN_SB_POS))
#define RF_LOTPM_HFP_BYPASS_SB                                  RF_LOTPM_HFP_BYPASS_SB
#define RF_LOTPM_HFP_BYPASS_SB_POS                              (7U)
#define RF_LOTPM_HFP_BYPASS_SB_LEN                              (1U)
#define RF_LOTPM_HFP_BYPASS_SB_MSK                              (((1U<<RF_LOTPM_HFP_BYPASS_SB_LEN)-1)<<RF_LOTPM_HFP_BYPASS_SB_POS)
#define RF_LOTPM_HFP_BYPASS_SB_UMSK                             (~(((1U<<RF_LOTPM_HFP_BYPASS_SB_LEN)-1)<<RF_LOTPM_HFP_BYPASS_SB_POS))
#define RF_LOTPM_LFP_BYPASS_SB                                  RF_LOTPM_LFP_BYPASS_SB
#define RF_LOTPM_LFP_BYPASS_SB_POS                              (8U)
#define RF_LOTPM_LFP_BYPASS_SB_LEN                              (1U)
#define RF_LOTPM_LFP_BYPASS_SB_MSK                              (((1U<<RF_LOTPM_LFP_BYPASS_SB_LEN)-1)<<RF_LOTPM_LFP_BYPASS_SB_POS)
#define RF_LOTPM_LFP_BYPASS_SB_UMSK                             (~(((1U<<RF_LOTPM_LFP_BYPASS_SB_LEN)-1)<<RF_LOTPM_LFP_BYPASS_SB_POS))
#define RF_LOTPM_HFP_CLK_EN_SB                                  RF_LOTPM_HFP_CLK_EN_SB
#define RF_LOTPM_HFP_CLK_EN_SB_POS                              (9U)
#define RF_LOTPM_HFP_CLK_EN_SB_LEN                              (1U)
#define RF_LOTPM_HFP_CLK_EN_SB_MSK                              (((1U<<RF_LOTPM_HFP_CLK_EN_SB_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_SB_POS)
#define RF_LOTPM_HFP_CLK_EN_SB_UMSK                             (~(((1U<<RF_LOTPM_HFP_CLK_EN_SB_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_SB_POS))
#define RF_PU_FBDV_BUF_SB                                       RF_PU_FBDV_BUF_SB
#define RF_PU_FBDV_BUF_SB_POS                                   (10U)
#define RF_PU_FBDV_BUF_SB_LEN                                   (1U)
#define RF_PU_FBDV_BUF_SB_MSK                                   (((1U<<RF_PU_FBDV_BUF_SB_LEN)-1)<<RF_PU_FBDV_BUF_SB_POS)
#define RF_PU_FBDV_BUF_SB_UMSK                                  (~(((1U<<RF_PU_FBDV_BUF_SB_LEN)-1)<<RF_PU_FBDV_BUF_SB_POS))
#define RF_PU_FBDV_SB                                           RF_PU_FBDV_SB
#define RF_PU_FBDV_SB_POS                                       (11U)
#define RF_PU_FBDV_SB_LEN                                       (1U)
#define RF_PU_FBDV_SB_MSK                                       (((1U<<RF_PU_FBDV_SB_LEN)-1)<<RF_PU_FBDV_SB_POS)
#define RF_PU_FBDV_SB_UMSK                                      (~(((1U<<RF_PU_FBDV_SB_LEN)-1)<<RF_PU_FBDV_SB_POS))
#define RF_PU_VCO_SB                                            RF_PU_VCO_SB
#define RF_PU_VCO_SB_POS                                        (12U)
#define RF_PU_VCO_SB_LEN                                        (1U)
#define RF_PU_VCO_SB_MSK                                        (((1U<<RF_PU_VCO_SB_LEN)-1)<<RF_PU_VCO_SB_POS)
#define RF_PU_VCO_SB_UMSK                                       (~(((1U<<RF_PU_VCO_SB_LEN)-1)<<RF_PU_VCO_SB_POS))
#define RF_PU_VCO_LDO_SB                                        RF_PU_VCO_LDO_SB
#define RF_PU_VCO_LDO_SB_POS                                    (13U)
#define RF_PU_VCO_LDO_SB_LEN                                    (1U)
#define RF_PU_VCO_LDO_SB_MSK                                    (((1U<<RF_PU_VCO_LDO_SB_LEN)-1)<<RF_PU_VCO_LDO_SB_POS)
#define RF_PU_VCO_LDO_SB_UMSK                                   (~(((1U<<RF_PU_VCO_LDO_SB_LEN)-1)<<RF_PU_VCO_LDO_SB_POS))
#define RF_PU_LODIST_BODY_BIAS_SB                               RF_PU_LODIST_BODY_BIAS_SB
#define RF_PU_LODIST_BODY_BIAS_SB_POS                           (14U)
#define RF_PU_LODIST_BODY_BIAS_SB_LEN                           (1U)
#define RF_PU_LODIST_BODY_BIAS_SB_MSK                           (((1U<<RF_PU_LODIST_BODY_BIAS_SB_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_SB_POS)
#define RF_PU_LODIST_BODY_BIAS_SB_UMSK                          (~(((1U<<RF_PU_LODIST_BODY_BIAS_SB_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_SB_POS))
#define RF_RXADC_CLK_EN_SB                                      RF_RXADC_CLK_EN_SB
#define RF_RXADC_CLK_EN_SB_POS                                  (15U)
#define RF_RXADC_CLK_EN_SB_LEN                                  (1U)
#define RF_RXADC_CLK_EN_SB_MSK                                  (((1U<<RF_RXADC_CLK_EN_SB_LEN)-1)<<RF_RXADC_CLK_EN_SB_POS)
#define RF_RXADC_CLK_EN_SB_UMSK                                 (~(((1U<<RF_RXADC_CLK_EN_SB_LEN)-1)<<RF_RXADC_CLK_EN_SB_POS))
#define RF_PU_RXADC_SB                                          RF_PU_RXADC_SB
#define RF_PU_RXADC_SB_POS                                      (16U)
#define RF_PU_RXADC_SB_LEN                                      (1U)
#define RF_PU_RXADC_SB_MSK                                      (((1U<<RF_PU_RXADC_SB_LEN)-1)<<RF_PU_RXADC_SB_POS)
#define RF_PU_RXADC_SB_UMSK                                     (~(((1U<<RF_PU_RXADC_SB_LEN)-1)<<RF_PU_RXADC_SB_POS))
#define RF_PU_ROSDAC_SB                                         RF_PU_ROSDAC_SB
#define RF_PU_ROSDAC_SB_POS                                     (17U)
#define RF_PU_ROSDAC_SB_LEN                                     (1U)
#define RF_PU_ROSDAC_SB_MSK                                     (((1U<<RF_PU_ROSDAC_SB_LEN)-1)<<RF_PU_ROSDAC_SB_POS)
#define RF_PU_ROSDAC_SB_UMSK                                    (~(((1U<<RF_PU_ROSDAC_SB_LEN)-1)<<RF_PU_ROSDAC_SB_POS))
#define RF_PU_RBB_PKDET_SB                                      RF_PU_RBB_PKDET_SB
#define RF_PU_RBB_PKDET_SB_POS                                  (18U)
#define RF_PU_RBB_PKDET_SB_LEN                                  (1U)
#define RF_PU_RBB_PKDET_SB_MSK                                  (((1U<<RF_PU_RBB_PKDET_SB_LEN)-1)<<RF_PU_RBB_PKDET_SB_POS)
#define RF_PU_RBB_PKDET_SB_UMSK                                 (~(((1U<<RF_PU_RBB_PKDET_SB_LEN)-1)<<RF_PU_RBB_PKDET_SB_POS))
#define RF_PU_RBB_SB                                            RF_PU_RBB_SB
#define RF_PU_RBB_SB_POS                                        (19U)
#define RF_PU_RBB_SB_LEN                                        (1U)
#define RF_PU_RBB_SB_MSK                                        (((1U<<RF_PU_RBB_SB_LEN)-1)<<RF_PU_RBB_SB_POS)
#define RF_PU_RBB_SB_UMSK                                       (~(((1U<<RF_PU_RBB_SB_LEN)-1)<<RF_PU_RBB_SB_POS))
#define RF_PU_RMX_SB                                            RF_PU_RMX_SB
#define RF_PU_RMX_SB_POS                                        (20U)
#define RF_PU_RMX_SB_LEN                                        (1U)
#define RF_PU_RMX_SB_MSK                                        (((1U<<RF_PU_RMX_SB_LEN)-1)<<RF_PU_RMX_SB_POS)
#define RF_PU_RMX_SB_UMSK                                       (~(((1U<<RF_PU_RMX_SB_LEN)-1)<<RF_PU_RMX_SB_POS))
#define RF_PU_LNA_SB                                            RF_PU_LNA_SB
#define RF_PU_LNA_SB_POS                                        (21U)
#define RF_PU_LNA_SB_LEN                                        (1U)
#define RF_PU_LNA_SB_MSK                                        (((1U<<RF_PU_LNA_SB_LEN)-1)<<RF_PU_LNA_SB_POS)
#define RF_PU_LNA_SB_UMSK                                       (~(((1U<<RF_PU_LNA_SB_LEN)-1)<<RF_PU_LNA_SB_POS))
#define RF_PU_PA_SB                                             RF_PU_PA_SB
#define RF_PU_PA_SB_POS                                         (22U)
#define RF_PU_PA_SB_LEN                                         (1U)
#define RF_PU_PA_SB_MSK                                         (((1U<<RF_PU_PA_SB_LEN)-1)<<RF_PU_PA_SB_POS)
#define RF_PU_PA_SB_UMSK                                        (~(((1U<<RF_PU_PA_SB_LEN)-1)<<RF_PU_PA_SB_POS))
#define RF_RX_BYPASS_EN_SB                                      RF_RX_BYPASS_EN_SB
#define RF_RX_BYPASS_EN_SB_POS                                  (23U)
#define RF_RX_BYPASS_EN_SB_LEN                                  (1U)
#define RF_RX_BYPASS_EN_SB_MSK                                  (((1U<<RF_RX_BYPASS_EN_SB_LEN)-1)<<RF_RX_BYPASS_EN_SB_POS)
#define RF_RX_BYPASS_EN_SB_UMSK                                 (~(((1U<<RF_RX_BYPASS_EN_SB_LEN)-1)<<RF_RX_BYPASS_EN_SB_POS))
#define RF_PA_SERI_CAP_EN_SB                                    RF_PA_SERI_CAP_EN_SB
#define RF_PA_SERI_CAP_EN_SB_POS                                (24U)
#define RF_PA_SERI_CAP_EN_SB_LEN                                (1U)
#define RF_PA_SERI_CAP_EN_SB_MSK                                (((1U<<RF_PA_SERI_CAP_EN_SB_LEN)-1)<<RF_PA_SERI_CAP_EN_SB_POS)
#define RF_PA_SERI_CAP_EN_SB_UMSK                               (~(((1U<<RF_PA_SERI_CAP_EN_SB_LEN)-1)<<RF_PA_SERI_CAP_EN_SB_POS))

/* 0x108 : Power up in LOTX state */
#define RF_PUCR_LOTX_OFFSET                                     (0x108)
#define RF_LODIST_TX_EN_LOTX                                    RF_LODIST_TX_EN_LOTX
#define RF_LODIST_TX_EN_LOTX_POS                                (0U)
#define RF_LODIST_TX_EN_LOTX_LEN                                (1U)
#define RF_LODIST_TX_EN_LOTX_MSK                                (((1U<<RF_LODIST_TX_EN_LOTX_LEN)-1)<<RF_LODIST_TX_EN_LOTX_POS)
#define RF_LODIST_TX_EN_LOTX_UMSK                               (~(((1U<<RF_LODIST_TX_EN_LOTX_LEN)-1)<<RF_LODIST_TX_EN_LOTX_POS))
#define RF_PU_TXBUF_LOTX                                        RF_PU_TXBUF_LOTX
#define RF_PU_TXBUF_LOTX_POS                                    (1U)
#define RF_PU_TXBUF_LOTX_LEN                                    (1U)
#define RF_PU_TXBUF_LOTX_MSK                                    (((1U<<RF_PU_TXBUF_LOTX_LEN)-1)<<RF_PU_TXBUF_LOTX_POS)
#define RF_PU_TXBUF_LOTX_UMSK                                   (~(((1U<<RF_PU_TXBUF_LOTX_LEN)-1)<<RF_PU_TXBUF_LOTX_POS))
#define RF_PU_RXBUF_LOTX                                        RF_PU_RXBUF_LOTX
#define RF_PU_RXBUF_LOTX_POS                                    (2U)
#define RF_PU_RXBUF_LOTX_LEN                                    (1U)
#define RF_PU_RXBUF_LOTX_MSK                                    (((1U<<RF_PU_RXBUF_LOTX_LEN)-1)<<RF_PU_RXBUF_LOTX_POS)
#define RF_PU_RXBUF_LOTX_UMSK                                   (~(((1U<<RF_PU_RXBUF_LOTX_LEN)-1)<<RF_PU_RXBUF_LOTX_POS))
#define RF_PU_DTC_LOTX                                          RF_PU_DTC_LOTX
#define RF_PU_DTC_LOTX_POS                                      (3U)
#define RF_PU_DTC_LOTX_LEN                                      (1U)
#define RF_PU_DTC_LOTX_MSK                                      (((1U<<RF_PU_DTC_LOTX_LEN)-1)<<RF_PU_DTC_LOTX_POS)
#define RF_PU_DTC_LOTX_UMSK                                     (~(((1U<<RF_PU_DTC_LOTX_LEN)-1)<<RF_PU_DTC_LOTX_POS))
#define RF_PU_ADPLL_SFREG_LOTX                                  RF_PU_ADPLL_SFREG_LOTX
#define RF_PU_ADPLL_SFREG_LOTX_POS                              (4U)
#define RF_PU_ADPLL_SFREG_LOTX_LEN                              (1U)
#define RF_PU_ADPLL_SFREG_LOTX_MSK                              (((1U<<RF_PU_ADPLL_SFREG_LOTX_LEN)-1)<<RF_PU_ADPLL_SFREG_LOTX_POS)
#define RF_PU_ADPLL_SFREG_LOTX_UMSK                             (~(((1U<<RF_PU_ADPLL_SFREG_LOTX_LEN)-1)<<RF_PU_ADPLL_SFREG_LOTX_POS))
#define RF_PU_ADPLL_ADC_LOTX                                    RF_PU_ADPLL_ADC_LOTX
#define RF_PU_ADPLL_ADC_LOTX_POS                                (5U)
#define RF_PU_ADPLL_ADC_LOTX_LEN                                (1U)
#define RF_PU_ADPLL_ADC_LOTX_MSK                                (((1U<<RF_PU_ADPLL_ADC_LOTX_LEN)-1)<<RF_PU_ADPLL_ADC_LOTX_POS)
#define RF_PU_ADPLL_ADC_LOTX_UMSK                               (~(((1U<<RF_PU_ADPLL_ADC_LOTX_LEN)-1)<<RF_PU_ADPLL_ADC_LOTX_POS))
#define RF_ADPLL_CLK_EN_LOTX                                    RF_ADPLL_CLK_EN_LOTX
#define RF_ADPLL_CLK_EN_LOTX_POS                                (6U)
#define RF_ADPLL_CLK_EN_LOTX_LEN                                (1U)
#define RF_ADPLL_CLK_EN_LOTX_MSK                                (((1U<<RF_ADPLL_CLK_EN_LOTX_LEN)-1)<<RF_ADPLL_CLK_EN_LOTX_POS)
#define RF_ADPLL_CLK_EN_LOTX_UMSK                               (~(((1U<<RF_ADPLL_CLK_EN_LOTX_LEN)-1)<<RF_ADPLL_CLK_EN_LOTX_POS))
#define RF_LOTPM_HFP_BYPASS_LOTX                                RF_LOTPM_HFP_BYPASS_LOTX
#define RF_LOTPM_HFP_BYPASS_LOTX_POS                            (7U)
#define RF_LOTPM_HFP_BYPASS_LOTX_LEN                            (1U)
#define RF_LOTPM_HFP_BYPASS_LOTX_MSK                            (((1U<<RF_LOTPM_HFP_BYPASS_LOTX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_LOTX_POS)
#define RF_LOTPM_HFP_BYPASS_LOTX_UMSK                           (~(((1U<<RF_LOTPM_HFP_BYPASS_LOTX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_LOTX_POS))
#define RF_LOTPM_LFP_BYPASS_LOTX                                RF_LOTPM_LFP_BYPASS_LOTX
#define RF_LOTPM_LFP_BYPASS_LOTX_POS                            (8U)
#define RF_LOTPM_LFP_BYPASS_LOTX_LEN                            (1U)
#define RF_LOTPM_LFP_BYPASS_LOTX_MSK                            (((1U<<RF_LOTPM_LFP_BYPASS_LOTX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_LOTX_POS)
#define RF_LOTPM_LFP_BYPASS_LOTX_UMSK                           (~(((1U<<RF_LOTPM_LFP_BYPASS_LOTX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_LOTX_POS))
#define RF_LOTPM_HFP_CLK_EN_LOTX                                RF_LOTPM_HFP_CLK_EN_LOTX
#define RF_LOTPM_HFP_CLK_EN_LOTX_POS                            (9U)
#define RF_LOTPM_HFP_CLK_EN_LOTX_LEN                            (1U)
#define RF_LOTPM_HFP_CLK_EN_LOTX_MSK                            (((1U<<RF_LOTPM_HFP_CLK_EN_LOTX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_LOTX_POS)
#define RF_LOTPM_HFP_CLK_EN_LOTX_UMSK                           (~(((1U<<RF_LOTPM_HFP_CLK_EN_LOTX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_LOTX_POS))
#define RF_PU_FBDV_BUF_LOTX                                     RF_PU_FBDV_BUF_LOTX
#define RF_PU_FBDV_BUF_LOTX_POS                                 (10U)
#define RF_PU_FBDV_BUF_LOTX_LEN                                 (1U)
#define RF_PU_FBDV_BUF_LOTX_MSK                                 (((1U<<RF_PU_FBDV_BUF_LOTX_LEN)-1)<<RF_PU_FBDV_BUF_LOTX_POS)
#define RF_PU_FBDV_BUF_LOTX_UMSK                                (~(((1U<<RF_PU_FBDV_BUF_LOTX_LEN)-1)<<RF_PU_FBDV_BUF_LOTX_POS))
#define RF_PU_FBDV_LOTX                                         RF_PU_FBDV_LOTX
#define RF_PU_FBDV_LOTX_POS                                     (11U)
#define RF_PU_FBDV_LOTX_LEN                                     (1U)
#define RF_PU_FBDV_LOTX_MSK                                     (((1U<<RF_PU_FBDV_LOTX_LEN)-1)<<RF_PU_FBDV_LOTX_POS)
#define RF_PU_FBDV_LOTX_UMSK                                    (~(((1U<<RF_PU_FBDV_LOTX_LEN)-1)<<RF_PU_FBDV_LOTX_POS))
#define RF_PU_VCO_LOTX                                          RF_PU_VCO_LOTX
#define RF_PU_VCO_LOTX_POS                                      (12U)
#define RF_PU_VCO_LOTX_LEN                                      (1U)
#define RF_PU_VCO_LOTX_MSK                                      (((1U<<RF_PU_VCO_LOTX_LEN)-1)<<RF_PU_VCO_LOTX_POS)
#define RF_PU_VCO_LOTX_UMSK                                     (~(((1U<<RF_PU_VCO_LOTX_LEN)-1)<<RF_PU_VCO_LOTX_POS))
#define RF_PU_VCO_LDO_LOTX                                      RF_PU_VCO_LDO_LOTX
#define RF_PU_VCO_LDO_LOTX_POS                                  (13U)
#define RF_PU_VCO_LDO_LOTX_LEN                                  (1U)
#define RF_PU_VCO_LDO_LOTX_MSK                                  (((1U<<RF_PU_VCO_LDO_LOTX_LEN)-1)<<RF_PU_VCO_LDO_LOTX_POS)
#define RF_PU_VCO_LDO_LOTX_UMSK                                 (~(((1U<<RF_PU_VCO_LDO_LOTX_LEN)-1)<<RF_PU_VCO_LDO_LOTX_POS))
#define RF_PU_LODIST_BODY_BIAS_LOTX                             RF_PU_LODIST_BODY_BIAS_LOTX
#define RF_PU_LODIST_BODY_BIAS_LOTX_POS                         (14U)
#define RF_PU_LODIST_BODY_BIAS_LOTX_LEN                         (1U)
#define RF_PU_LODIST_BODY_BIAS_LOTX_MSK                         (((1U<<RF_PU_LODIST_BODY_BIAS_LOTX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_LOTX_POS)
#define RF_PU_LODIST_BODY_BIAS_LOTX_UMSK                        (~(((1U<<RF_PU_LODIST_BODY_BIAS_LOTX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_LOTX_POS))
#define RF_RXADC_CLK_EN_LOTX                                    RF_RXADC_CLK_EN_LOTX
#define RF_RXADC_CLK_EN_LOTX_POS                                (15U)
#define RF_RXADC_CLK_EN_LOTX_LEN                                (1U)
#define RF_RXADC_CLK_EN_LOTX_MSK                                (((1U<<RF_RXADC_CLK_EN_LOTX_LEN)-1)<<RF_RXADC_CLK_EN_LOTX_POS)
#define RF_RXADC_CLK_EN_LOTX_UMSK                               (~(((1U<<RF_RXADC_CLK_EN_LOTX_LEN)-1)<<RF_RXADC_CLK_EN_LOTX_POS))
#define RF_PU_RXADC_LOTX                                        RF_PU_RXADC_LOTX
#define RF_PU_RXADC_LOTX_POS                                    (16U)
#define RF_PU_RXADC_LOTX_LEN                                    (1U)
#define RF_PU_RXADC_LOTX_MSK                                    (((1U<<RF_PU_RXADC_LOTX_LEN)-1)<<RF_PU_RXADC_LOTX_POS)
#define RF_PU_RXADC_LOTX_UMSK                                   (~(((1U<<RF_PU_RXADC_LOTX_LEN)-1)<<RF_PU_RXADC_LOTX_POS))
#define RF_PU_ROSDAC_LOTX                                       RF_PU_ROSDAC_LOTX
#define RF_PU_ROSDAC_LOTX_POS                                   (17U)
#define RF_PU_ROSDAC_LOTX_LEN                                   (1U)
#define RF_PU_ROSDAC_LOTX_MSK                                   (((1U<<RF_PU_ROSDAC_LOTX_LEN)-1)<<RF_PU_ROSDAC_LOTX_POS)
#define RF_PU_ROSDAC_LOTX_UMSK                                  (~(((1U<<RF_PU_ROSDAC_LOTX_LEN)-1)<<RF_PU_ROSDAC_LOTX_POS))
#define RF_PU_RBB_PKDET_LOTX                                    RF_PU_RBB_PKDET_LOTX
#define RF_PU_RBB_PKDET_LOTX_POS                                (18U)
#define RF_PU_RBB_PKDET_LOTX_LEN                                (1U)
#define RF_PU_RBB_PKDET_LOTX_MSK                                (((1U<<RF_PU_RBB_PKDET_LOTX_LEN)-1)<<RF_PU_RBB_PKDET_LOTX_POS)
#define RF_PU_RBB_PKDET_LOTX_UMSK                               (~(((1U<<RF_PU_RBB_PKDET_LOTX_LEN)-1)<<RF_PU_RBB_PKDET_LOTX_POS))
#define RF_PU_RBB_LOTX                                          RF_PU_RBB_LOTX
#define RF_PU_RBB_LOTX_POS                                      (19U)
#define RF_PU_RBB_LOTX_LEN                                      (1U)
#define RF_PU_RBB_LOTX_MSK                                      (((1U<<RF_PU_RBB_LOTX_LEN)-1)<<RF_PU_RBB_LOTX_POS)
#define RF_PU_RBB_LOTX_UMSK                                     (~(((1U<<RF_PU_RBB_LOTX_LEN)-1)<<RF_PU_RBB_LOTX_POS))
#define RF_PU_RMX_LOTX                                          RF_PU_RMX_LOTX
#define RF_PU_RMX_LOTX_POS                                      (20U)
#define RF_PU_RMX_LOTX_LEN                                      (1U)
#define RF_PU_RMX_LOTX_MSK                                      (((1U<<RF_PU_RMX_LOTX_LEN)-1)<<RF_PU_RMX_LOTX_POS)
#define RF_PU_RMX_LOTX_UMSK                                     (~(((1U<<RF_PU_RMX_LOTX_LEN)-1)<<RF_PU_RMX_LOTX_POS))
#define RF_PU_LNA_LOTX                                          RF_PU_LNA_LOTX
#define RF_PU_LNA_LOTX_POS                                      (21U)
#define RF_PU_LNA_LOTX_LEN                                      (1U)
#define RF_PU_LNA_LOTX_MSK                                      (((1U<<RF_PU_LNA_LOTX_LEN)-1)<<RF_PU_LNA_LOTX_POS)
#define RF_PU_LNA_LOTX_UMSK                                     (~(((1U<<RF_PU_LNA_LOTX_LEN)-1)<<RF_PU_LNA_LOTX_POS))
#define RF_PU_PA_LOTX                                           RF_PU_PA_LOTX
#define RF_PU_PA_LOTX_POS                                       (22U)
#define RF_PU_PA_LOTX_LEN                                       (1U)
#define RF_PU_PA_LOTX_MSK                                       (((1U<<RF_PU_PA_LOTX_LEN)-1)<<RF_PU_PA_LOTX_POS)
#define RF_PU_PA_LOTX_UMSK                                      (~(((1U<<RF_PU_PA_LOTX_LEN)-1)<<RF_PU_PA_LOTX_POS))
#define RF_RX_BYPASS_EN_LOTX                                    RF_RX_BYPASS_EN_LOTX
#define RF_RX_BYPASS_EN_LOTX_POS                                (23U)
#define RF_RX_BYPASS_EN_LOTX_LEN                                (1U)
#define RF_RX_BYPASS_EN_LOTX_MSK                                (((1U<<RF_RX_BYPASS_EN_LOTX_LEN)-1)<<RF_RX_BYPASS_EN_LOTX_POS)
#define RF_RX_BYPASS_EN_LOTX_UMSK                               (~(((1U<<RF_RX_BYPASS_EN_LOTX_LEN)-1)<<RF_RX_BYPASS_EN_LOTX_POS))
#define RF_PA_SERI_CAP_EN_LOTX                                  RF_PA_SERI_CAP_EN_LOTX
#define RF_PA_SERI_CAP_EN_LOTX_POS                              (24U)
#define RF_PA_SERI_CAP_EN_LOTX_LEN                              (1U)
#define RF_PA_SERI_CAP_EN_LOTX_MSK                              (((1U<<RF_PA_SERI_CAP_EN_LOTX_LEN)-1)<<RF_PA_SERI_CAP_EN_LOTX_POS)
#define RF_PA_SERI_CAP_EN_LOTX_UMSK                             (~(((1U<<RF_PA_SERI_CAP_EN_LOTX_LEN)-1)<<RF_PA_SERI_CAP_EN_LOTX_POS))

/* 0x10C : Power up in LORX state */
#define RF_PUCR_LORX_OFFSET                                     (0x10C)
#define RF_LODIST_TX_EN_LORX                                    RF_LODIST_TX_EN_LORX
#define RF_LODIST_TX_EN_LORX_POS                                (0U)
#define RF_LODIST_TX_EN_LORX_LEN                                (1U)
#define RF_LODIST_TX_EN_LORX_MSK                                (((1U<<RF_LODIST_TX_EN_LORX_LEN)-1)<<RF_LODIST_TX_EN_LORX_POS)
#define RF_LODIST_TX_EN_LORX_UMSK                               (~(((1U<<RF_LODIST_TX_EN_LORX_LEN)-1)<<RF_LODIST_TX_EN_LORX_POS))
#define RF_PU_TXBUF_LORX                                        RF_PU_TXBUF_LORX
#define RF_PU_TXBUF_LORX_POS                                    (1U)
#define RF_PU_TXBUF_LORX_LEN                                    (1U)
#define RF_PU_TXBUF_LORX_MSK                                    (((1U<<RF_PU_TXBUF_LORX_LEN)-1)<<RF_PU_TXBUF_LORX_POS)
#define RF_PU_TXBUF_LORX_UMSK                                   (~(((1U<<RF_PU_TXBUF_LORX_LEN)-1)<<RF_PU_TXBUF_LORX_POS))
#define RF_PU_RXBUF_LORX                                        RF_PU_RXBUF_LORX
#define RF_PU_RXBUF_LORX_POS                                    (2U)
#define RF_PU_RXBUF_LORX_LEN                                    (1U)
#define RF_PU_RXBUF_LORX_MSK                                    (((1U<<RF_PU_RXBUF_LORX_LEN)-1)<<RF_PU_RXBUF_LORX_POS)
#define RF_PU_RXBUF_LORX_UMSK                                   (~(((1U<<RF_PU_RXBUF_LORX_LEN)-1)<<RF_PU_RXBUF_LORX_POS))
#define RF_PU_DTC_LORX                                          RF_PU_DTC_LORX
#define RF_PU_DTC_LORX_POS                                      (3U)
#define RF_PU_DTC_LORX_LEN                                      (1U)
#define RF_PU_DTC_LORX_MSK                                      (((1U<<RF_PU_DTC_LORX_LEN)-1)<<RF_PU_DTC_LORX_POS)
#define RF_PU_DTC_LORX_UMSK                                     (~(((1U<<RF_PU_DTC_LORX_LEN)-1)<<RF_PU_DTC_LORX_POS))
#define RF_PU_ADPLL_SFREG_LORX                                  RF_PU_ADPLL_SFREG_LORX
#define RF_PU_ADPLL_SFREG_LORX_POS                              (4U)
#define RF_PU_ADPLL_SFREG_LORX_LEN                              (1U)
#define RF_PU_ADPLL_SFREG_LORX_MSK                              (((1U<<RF_PU_ADPLL_SFREG_LORX_LEN)-1)<<RF_PU_ADPLL_SFREG_LORX_POS)
#define RF_PU_ADPLL_SFREG_LORX_UMSK                             (~(((1U<<RF_PU_ADPLL_SFREG_LORX_LEN)-1)<<RF_PU_ADPLL_SFREG_LORX_POS))
#define RF_PU_ADPLL_ADC_LORX                                    RF_PU_ADPLL_ADC_LORX
#define RF_PU_ADPLL_ADC_LORX_POS                                (5U)
#define RF_PU_ADPLL_ADC_LORX_LEN                                (1U)
#define RF_PU_ADPLL_ADC_LORX_MSK                                (((1U<<RF_PU_ADPLL_ADC_LORX_LEN)-1)<<RF_PU_ADPLL_ADC_LORX_POS)
#define RF_PU_ADPLL_ADC_LORX_UMSK                               (~(((1U<<RF_PU_ADPLL_ADC_LORX_LEN)-1)<<RF_PU_ADPLL_ADC_LORX_POS))
#define RF_ADPLL_CLK_EN_LORX                                    RF_ADPLL_CLK_EN_LORX
#define RF_ADPLL_CLK_EN_LORX_POS                                (6U)
#define RF_ADPLL_CLK_EN_LORX_LEN                                (1U)
#define RF_ADPLL_CLK_EN_LORX_MSK                                (((1U<<RF_ADPLL_CLK_EN_LORX_LEN)-1)<<RF_ADPLL_CLK_EN_LORX_POS)
#define RF_ADPLL_CLK_EN_LORX_UMSK                               (~(((1U<<RF_ADPLL_CLK_EN_LORX_LEN)-1)<<RF_ADPLL_CLK_EN_LORX_POS))
#define RF_LOTPM_HFP_BYPASS_LORX                                RF_LOTPM_HFP_BYPASS_LORX
#define RF_LOTPM_HFP_BYPASS_LORX_POS                            (7U)
#define RF_LOTPM_HFP_BYPASS_LORX_LEN                            (1U)
#define RF_LOTPM_HFP_BYPASS_LORX_MSK                            (((1U<<RF_LOTPM_HFP_BYPASS_LORX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_LORX_POS)
#define RF_LOTPM_HFP_BYPASS_LORX_UMSK                           (~(((1U<<RF_LOTPM_HFP_BYPASS_LORX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_LORX_POS))
#define RF_LOTPM_LFP_BYPASS_LORX                                RF_LOTPM_LFP_BYPASS_LORX
#define RF_LOTPM_LFP_BYPASS_LORX_POS                            (8U)
#define RF_LOTPM_LFP_BYPASS_LORX_LEN                            (1U)
#define RF_LOTPM_LFP_BYPASS_LORX_MSK                            (((1U<<RF_LOTPM_LFP_BYPASS_LORX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_LORX_POS)
#define RF_LOTPM_LFP_BYPASS_LORX_UMSK                           (~(((1U<<RF_LOTPM_LFP_BYPASS_LORX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_LORX_POS))
#define RF_LOTPM_HFP_CLK_EN_LORX                                RF_LOTPM_HFP_CLK_EN_LORX
#define RF_LOTPM_HFP_CLK_EN_LORX_POS                            (9U)
#define RF_LOTPM_HFP_CLK_EN_LORX_LEN                            (1U)
#define RF_LOTPM_HFP_CLK_EN_LORX_MSK                            (((1U<<RF_LOTPM_HFP_CLK_EN_LORX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_LORX_POS)
#define RF_LOTPM_HFP_CLK_EN_LORX_UMSK                           (~(((1U<<RF_LOTPM_HFP_CLK_EN_LORX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_LORX_POS))
#define RF_PU_FBDV_BUF_LORX                                     RF_PU_FBDV_BUF_LORX
#define RF_PU_FBDV_BUF_LORX_POS                                 (10U)
#define RF_PU_FBDV_BUF_LORX_LEN                                 (1U)
#define RF_PU_FBDV_BUF_LORX_MSK                                 (((1U<<RF_PU_FBDV_BUF_LORX_LEN)-1)<<RF_PU_FBDV_BUF_LORX_POS)
#define RF_PU_FBDV_BUF_LORX_UMSK                                (~(((1U<<RF_PU_FBDV_BUF_LORX_LEN)-1)<<RF_PU_FBDV_BUF_LORX_POS))
#define RF_PU_FBDV_LORX                                         RF_PU_FBDV_LORX
#define RF_PU_FBDV_LORX_POS                                     (11U)
#define RF_PU_FBDV_LORX_LEN                                     (1U)
#define RF_PU_FBDV_LORX_MSK                                     (((1U<<RF_PU_FBDV_LORX_LEN)-1)<<RF_PU_FBDV_LORX_POS)
#define RF_PU_FBDV_LORX_UMSK                                    (~(((1U<<RF_PU_FBDV_LORX_LEN)-1)<<RF_PU_FBDV_LORX_POS))
#define RF_PU_VCO_LORX                                          RF_PU_VCO_LORX
#define RF_PU_VCO_LORX_POS                                      (12U)
#define RF_PU_VCO_LORX_LEN                                      (1U)
#define RF_PU_VCO_LORX_MSK                                      (((1U<<RF_PU_VCO_LORX_LEN)-1)<<RF_PU_VCO_LORX_POS)
#define RF_PU_VCO_LORX_UMSK                                     (~(((1U<<RF_PU_VCO_LORX_LEN)-1)<<RF_PU_VCO_LORX_POS))
#define RF_PU_VCO_LDO_LORX                                      RF_PU_VCO_LDO_LORX
#define RF_PU_VCO_LDO_LORX_POS                                  (13U)
#define RF_PU_VCO_LDO_LORX_LEN                                  (1U)
#define RF_PU_VCO_LDO_LORX_MSK                                  (((1U<<RF_PU_VCO_LDO_LORX_LEN)-1)<<RF_PU_VCO_LDO_LORX_POS)
#define RF_PU_VCO_LDO_LORX_UMSK                                 (~(((1U<<RF_PU_VCO_LDO_LORX_LEN)-1)<<RF_PU_VCO_LDO_LORX_POS))
#define RF_PU_LODIST_BODY_BIAS_LORX                             RF_PU_LODIST_BODY_BIAS_LORX
#define RF_PU_LODIST_BODY_BIAS_LORX_POS                         (14U)
#define RF_PU_LODIST_BODY_BIAS_LORX_LEN                         (1U)
#define RF_PU_LODIST_BODY_BIAS_LORX_MSK                         (((1U<<RF_PU_LODIST_BODY_BIAS_LORX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_LORX_POS)
#define RF_PU_LODIST_BODY_BIAS_LORX_UMSK                        (~(((1U<<RF_PU_LODIST_BODY_BIAS_LORX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_LORX_POS))
#define RF_RXADC_CLK_EN_LORX                                    RF_RXADC_CLK_EN_LORX
#define RF_RXADC_CLK_EN_LORX_POS                                (15U)
#define RF_RXADC_CLK_EN_LORX_LEN                                (1U)
#define RF_RXADC_CLK_EN_LORX_MSK                                (((1U<<RF_RXADC_CLK_EN_LORX_LEN)-1)<<RF_RXADC_CLK_EN_LORX_POS)
#define RF_RXADC_CLK_EN_LORX_UMSK                               (~(((1U<<RF_RXADC_CLK_EN_LORX_LEN)-1)<<RF_RXADC_CLK_EN_LORX_POS))
#define RF_PU_RXADC_LORX                                        RF_PU_RXADC_LORX
#define RF_PU_RXADC_LORX_POS                                    (16U)
#define RF_PU_RXADC_LORX_LEN                                    (1U)
#define RF_PU_RXADC_LORX_MSK                                    (((1U<<RF_PU_RXADC_LORX_LEN)-1)<<RF_PU_RXADC_LORX_POS)
#define RF_PU_RXADC_LORX_UMSK                                   (~(((1U<<RF_PU_RXADC_LORX_LEN)-1)<<RF_PU_RXADC_LORX_POS))
#define RF_PU_ROSDAC_LORX                                       RF_PU_ROSDAC_LORX
#define RF_PU_ROSDAC_LORX_POS                                   (17U)
#define RF_PU_ROSDAC_LORX_LEN                                   (1U)
#define RF_PU_ROSDAC_LORX_MSK                                   (((1U<<RF_PU_ROSDAC_LORX_LEN)-1)<<RF_PU_ROSDAC_LORX_POS)
#define RF_PU_ROSDAC_LORX_UMSK                                  (~(((1U<<RF_PU_ROSDAC_LORX_LEN)-1)<<RF_PU_ROSDAC_LORX_POS))
#define RF_PU_RBB_PKDET_LORX                                    RF_PU_RBB_PKDET_LORX
#define RF_PU_RBB_PKDET_LORX_POS                                (18U)
#define RF_PU_RBB_PKDET_LORX_LEN                                (1U)
#define RF_PU_RBB_PKDET_LORX_MSK                                (((1U<<RF_PU_RBB_PKDET_LORX_LEN)-1)<<RF_PU_RBB_PKDET_LORX_POS)
#define RF_PU_RBB_PKDET_LORX_UMSK                               (~(((1U<<RF_PU_RBB_PKDET_LORX_LEN)-1)<<RF_PU_RBB_PKDET_LORX_POS))
#define RF_PU_RBB_LORX                                          RF_PU_RBB_LORX
#define RF_PU_RBB_LORX_POS                                      (19U)
#define RF_PU_RBB_LORX_LEN                                      (1U)
#define RF_PU_RBB_LORX_MSK                                      (((1U<<RF_PU_RBB_LORX_LEN)-1)<<RF_PU_RBB_LORX_POS)
#define RF_PU_RBB_LORX_UMSK                                     (~(((1U<<RF_PU_RBB_LORX_LEN)-1)<<RF_PU_RBB_LORX_POS))
#define RF_PU_RMX_LORX                                          RF_PU_RMX_LORX
#define RF_PU_RMX_LORX_POS                                      (20U)
#define RF_PU_RMX_LORX_LEN                                      (1U)
#define RF_PU_RMX_LORX_MSK                                      (((1U<<RF_PU_RMX_LORX_LEN)-1)<<RF_PU_RMX_LORX_POS)
#define RF_PU_RMX_LORX_UMSK                                     (~(((1U<<RF_PU_RMX_LORX_LEN)-1)<<RF_PU_RMX_LORX_POS))
#define RF_PU_LNA_LORX                                          RF_PU_LNA_LORX
#define RF_PU_LNA_LORX_POS                                      (21U)
#define RF_PU_LNA_LORX_LEN                                      (1U)
#define RF_PU_LNA_LORX_MSK                                      (((1U<<RF_PU_LNA_LORX_LEN)-1)<<RF_PU_LNA_LORX_POS)
#define RF_PU_LNA_LORX_UMSK                                     (~(((1U<<RF_PU_LNA_LORX_LEN)-1)<<RF_PU_LNA_LORX_POS))
#define RF_PU_PA_LORX                                           RF_PU_PA_LORX
#define RF_PU_PA_LORX_POS                                       (22U)
#define RF_PU_PA_LORX_LEN                                       (1U)
#define RF_PU_PA_LORX_MSK                                       (((1U<<RF_PU_PA_LORX_LEN)-1)<<RF_PU_PA_LORX_POS)
#define RF_PU_PA_LORX_UMSK                                      (~(((1U<<RF_PU_PA_LORX_LEN)-1)<<RF_PU_PA_LORX_POS))
#define RF_RX_BYPASS_EN_LORX                                    RF_RX_BYPASS_EN_LORX
#define RF_RX_BYPASS_EN_LORX_POS                                (23U)
#define RF_RX_BYPASS_EN_LORX_LEN                                (1U)
#define RF_RX_BYPASS_EN_LORX_MSK                                (((1U<<RF_RX_BYPASS_EN_LORX_LEN)-1)<<RF_RX_BYPASS_EN_LORX_POS)
#define RF_RX_BYPASS_EN_LORX_UMSK                               (~(((1U<<RF_RX_BYPASS_EN_LORX_LEN)-1)<<RF_RX_BYPASS_EN_LORX_POS))
#define RF_PA_SERI_CAP_EN_LORX                                  RF_PA_SERI_CAP_EN_LORX
#define RF_PA_SERI_CAP_EN_LORX_POS                              (24U)
#define RF_PA_SERI_CAP_EN_LORX_LEN                              (1U)
#define RF_PA_SERI_CAP_EN_LORX_MSK                              (((1U<<RF_PA_SERI_CAP_EN_LORX_LEN)-1)<<RF_PA_SERI_CAP_EN_LORX_POS)
#define RF_PA_SERI_CAP_EN_LORX_UMSK                             (~(((1U<<RF_PA_SERI_CAP_EN_LORX_LEN)-1)<<RF_PA_SERI_CAP_EN_LORX_POS))

/* 0x110 : Power up in TX state */
#define RF_PUCR_TX_OFFSET                                       (0x110)
#define RF_LODIST_TX_EN_TX                                      RF_LODIST_TX_EN_TX
#define RF_LODIST_TX_EN_TX_POS                                  (0U)
#define RF_LODIST_TX_EN_TX_LEN                                  (1U)
#define RF_LODIST_TX_EN_TX_MSK                                  (((1U<<RF_LODIST_TX_EN_TX_LEN)-1)<<RF_LODIST_TX_EN_TX_POS)
#define RF_LODIST_TX_EN_TX_UMSK                                 (~(((1U<<RF_LODIST_TX_EN_TX_LEN)-1)<<RF_LODIST_TX_EN_TX_POS))
#define RF_PU_TXBUF_TX                                          RF_PU_TXBUF_TX
#define RF_PU_TXBUF_TX_POS                                      (1U)
#define RF_PU_TXBUF_TX_LEN                                      (1U)
#define RF_PU_TXBUF_TX_MSK                                      (((1U<<RF_PU_TXBUF_TX_LEN)-1)<<RF_PU_TXBUF_TX_POS)
#define RF_PU_TXBUF_TX_UMSK                                     (~(((1U<<RF_PU_TXBUF_TX_LEN)-1)<<RF_PU_TXBUF_TX_POS))
#define RF_PU_RXBUF_TX                                          RF_PU_RXBUF_TX
#define RF_PU_RXBUF_TX_POS                                      (2U)
#define RF_PU_RXBUF_TX_LEN                                      (1U)
#define RF_PU_RXBUF_TX_MSK                                      (((1U<<RF_PU_RXBUF_TX_LEN)-1)<<RF_PU_RXBUF_TX_POS)
#define RF_PU_RXBUF_TX_UMSK                                     (~(((1U<<RF_PU_RXBUF_TX_LEN)-1)<<RF_PU_RXBUF_TX_POS))
#define RF_PU_DTC_TX                                            RF_PU_DTC_TX
#define RF_PU_DTC_TX_POS                                        (3U)
#define RF_PU_DTC_TX_LEN                                        (1U)
#define RF_PU_DTC_TX_MSK                                        (((1U<<RF_PU_DTC_TX_LEN)-1)<<RF_PU_DTC_TX_POS)
#define RF_PU_DTC_TX_UMSK                                       (~(((1U<<RF_PU_DTC_TX_LEN)-1)<<RF_PU_DTC_TX_POS))
#define RF_PU_ADPLL_SFREG_TX                                    RF_PU_ADPLL_SFREG_TX
#define RF_PU_ADPLL_SFREG_TX_POS                                (4U)
#define RF_PU_ADPLL_SFREG_TX_LEN                                (1U)
#define RF_PU_ADPLL_SFREG_TX_MSK                                (((1U<<RF_PU_ADPLL_SFREG_TX_LEN)-1)<<RF_PU_ADPLL_SFREG_TX_POS)
#define RF_PU_ADPLL_SFREG_TX_UMSK                               (~(((1U<<RF_PU_ADPLL_SFREG_TX_LEN)-1)<<RF_PU_ADPLL_SFREG_TX_POS))
#define RF_PU_ADPLL_ADC_TX                                      RF_PU_ADPLL_ADC_TX
#define RF_PU_ADPLL_ADC_TX_POS                                  (5U)
#define RF_PU_ADPLL_ADC_TX_LEN                                  (1U)
#define RF_PU_ADPLL_ADC_TX_MSK                                  (((1U<<RF_PU_ADPLL_ADC_TX_LEN)-1)<<RF_PU_ADPLL_ADC_TX_POS)
#define RF_PU_ADPLL_ADC_TX_UMSK                                 (~(((1U<<RF_PU_ADPLL_ADC_TX_LEN)-1)<<RF_PU_ADPLL_ADC_TX_POS))
#define RF_ADPLL_CLK_EN_TX                                      RF_ADPLL_CLK_EN_TX
#define RF_ADPLL_CLK_EN_TX_POS                                  (6U)
#define RF_ADPLL_CLK_EN_TX_LEN                                  (1U)
#define RF_ADPLL_CLK_EN_TX_MSK                                  (((1U<<RF_ADPLL_CLK_EN_TX_LEN)-1)<<RF_ADPLL_CLK_EN_TX_POS)
#define RF_ADPLL_CLK_EN_TX_UMSK                                 (~(((1U<<RF_ADPLL_CLK_EN_TX_LEN)-1)<<RF_ADPLL_CLK_EN_TX_POS))
#define RF_LOTPM_HFP_BYPASS_TX                                  RF_LOTPM_HFP_BYPASS_TX
#define RF_LOTPM_HFP_BYPASS_TX_POS                              (7U)
#define RF_LOTPM_HFP_BYPASS_TX_LEN                              (1U)
#define RF_LOTPM_HFP_BYPASS_TX_MSK                              (((1U<<RF_LOTPM_HFP_BYPASS_TX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_TX_POS)
#define RF_LOTPM_HFP_BYPASS_TX_UMSK                             (~(((1U<<RF_LOTPM_HFP_BYPASS_TX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_TX_POS))
#define RF_LOTPM_LFP_BYPASS_TX                                  RF_LOTPM_LFP_BYPASS_TX
#define RF_LOTPM_LFP_BYPASS_TX_POS                              (8U)
#define RF_LOTPM_LFP_BYPASS_TX_LEN                              (1U)
#define RF_LOTPM_LFP_BYPASS_TX_MSK                              (((1U<<RF_LOTPM_LFP_BYPASS_TX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_TX_POS)
#define RF_LOTPM_LFP_BYPASS_TX_UMSK                             (~(((1U<<RF_LOTPM_LFP_BYPASS_TX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_TX_POS))
#define RF_LOTPM_HFP_CLK_EN_TX                                  RF_LOTPM_HFP_CLK_EN_TX
#define RF_LOTPM_HFP_CLK_EN_TX_POS                              (9U)
#define RF_LOTPM_HFP_CLK_EN_TX_LEN                              (1U)
#define RF_LOTPM_HFP_CLK_EN_TX_MSK                              (((1U<<RF_LOTPM_HFP_CLK_EN_TX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_TX_POS)
#define RF_LOTPM_HFP_CLK_EN_TX_UMSK                             (~(((1U<<RF_LOTPM_HFP_CLK_EN_TX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_TX_POS))
#define RF_PU_FBDV_BUF_TX                                       RF_PU_FBDV_BUF_TX
#define RF_PU_FBDV_BUF_TX_POS                                   (10U)
#define RF_PU_FBDV_BUF_TX_LEN                                   (1U)
#define RF_PU_FBDV_BUF_TX_MSK                                   (((1U<<RF_PU_FBDV_BUF_TX_LEN)-1)<<RF_PU_FBDV_BUF_TX_POS)
#define RF_PU_FBDV_BUF_TX_UMSK                                  (~(((1U<<RF_PU_FBDV_BUF_TX_LEN)-1)<<RF_PU_FBDV_BUF_TX_POS))
#define RF_PU_FBDV_TX                                           RF_PU_FBDV_TX
#define RF_PU_FBDV_TX_POS                                       (11U)
#define RF_PU_FBDV_TX_LEN                                       (1U)
#define RF_PU_FBDV_TX_MSK                                       (((1U<<RF_PU_FBDV_TX_LEN)-1)<<RF_PU_FBDV_TX_POS)
#define RF_PU_FBDV_TX_UMSK                                      (~(((1U<<RF_PU_FBDV_TX_LEN)-1)<<RF_PU_FBDV_TX_POS))
#define RF_PU_VCO_TX                                            RF_PU_VCO_TX
#define RF_PU_VCO_TX_POS                                        (12U)
#define RF_PU_VCO_TX_LEN                                        (1U)
#define RF_PU_VCO_TX_MSK                                        (((1U<<RF_PU_VCO_TX_LEN)-1)<<RF_PU_VCO_TX_POS)
#define RF_PU_VCO_TX_UMSK                                       (~(((1U<<RF_PU_VCO_TX_LEN)-1)<<RF_PU_VCO_TX_POS))
#define RF_PU_VCO_LDO_TX                                        RF_PU_VCO_LDO_TX
#define RF_PU_VCO_LDO_TX_POS                                    (13U)
#define RF_PU_VCO_LDO_TX_LEN                                    (1U)
#define RF_PU_VCO_LDO_TX_MSK                                    (((1U<<RF_PU_VCO_LDO_TX_LEN)-1)<<RF_PU_VCO_LDO_TX_POS)
#define RF_PU_VCO_LDO_TX_UMSK                                   (~(((1U<<RF_PU_VCO_LDO_TX_LEN)-1)<<RF_PU_VCO_LDO_TX_POS))
#define RF_PU_LODIST_BODY_BIAS_TX                               RF_PU_LODIST_BODY_BIAS_TX
#define RF_PU_LODIST_BODY_BIAS_TX_POS                           (14U)
#define RF_PU_LODIST_BODY_BIAS_TX_LEN                           (1U)
#define RF_PU_LODIST_BODY_BIAS_TX_MSK                           (((1U<<RF_PU_LODIST_BODY_BIAS_TX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_TX_POS)
#define RF_PU_LODIST_BODY_BIAS_TX_UMSK                          (~(((1U<<RF_PU_LODIST_BODY_BIAS_TX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_TX_POS))
#define RF_RXADC_CLK_EN_TX                                      RF_RXADC_CLK_EN_TX
#define RF_RXADC_CLK_EN_TX_POS                                  (15U)
#define RF_RXADC_CLK_EN_TX_LEN                                  (1U)
#define RF_RXADC_CLK_EN_TX_MSK                                  (((1U<<RF_RXADC_CLK_EN_TX_LEN)-1)<<RF_RXADC_CLK_EN_TX_POS)
#define RF_RXADC_CLK_EN_TX_UMSK                                 (~(((1U<<RF_RXADC_CLK_EN_TX_LEN)-1)<<RF_RXADC_CLK_EN_TX_POS))
#define RF_PU_RXADC_TX                                          RF_PU_RXADC_TX
#define RF_PU_RXADC_TX_POS                                      (16U)
#define RF_PU_RXADC_TX_LEN                                      (1U)
#define RF_PU_RXADC_TX_MSK                                      (((1U<<RF_PU_RXADC_TX_LEN)-1)<<RF_PU_RXADC_TX_POS)
#define RF_PU_RXADC_TX_UMSK                                     (~(((1U<<RF_PU_RXADC_TX_LEN)-1)<<RF_PU_RXADC_TX_POS))
#define RF_PU_ROSDAC_TX                                         RF_PU_ROSDAC_TX
#define RF_PU_ROSDAC_TX_POS                                     (17U)
#define RF_PU_ROSDAC_TX_LEN                                     (1U)
#define RF_PU_ROSDAC_TX_MSK                                     (((1U<<RF_PU_ROSDAC_TX_LEN)-1)<<RF_PU_ROSDAC_TX_POS)
#define RF_PU_ROSDAC_TX_UMSK                                    (~(((1U<<RF_PU_ROSDAC_TX_LEN)-1)<<RF_PU_ROSDAC_TX_POS))
#define RF_PU_RBB_PKDET_TX                                      RF_PU_RBB_PKDET_TX
#define RF_PU_RBB_PKDET_TX_POS                                  (18U)
#define RF_PU_RBB_PKDET_TX_LEN                                  (1U)
#define RF_PU_RBB_PKDET_TX_MSK                                  (((1U<<RF_PU_RBB_PKDET_TX_LEN)-1)<<RF_PU_RBB_PKDET_TX_POS)
#define RF_PU_RBB_PKDET_TX_UMSK                                 (~(((1U<<RF_PU_RBB_PKDET_TX_LEN)-1)<<RF_PU_RBB_PKDET_TX_POS))
#define RF_PU_RBB_TX                                            RF_PU_RBB_TX
#define RF_PU_RBB_TX_POS                                        (19U)
#define RF_PU_RBB_TX_LEN                                        (1U)
#define RF_PU_RBB_TX_MSK                                        (((1U<<RF_PU_RBB_TX_LEN)-1)<<RF_PU_RBB_TX_POS)
#define RF_PU_RBB_TX_UMSK                                       (~(((1U<<RF_PU_RBB_TX_LEN)-1)<<RF_PU_RBB_TX_POS))
#define RF_PU_RMX_TX                                            RF_PU_RMX_TX
#define RF_PU_RMX_TX_POS                                        (20U)
#define RF_PU_RMX_TX_LEN                                        (1U)
#define RF_PU_RMX_TX_MSK                                        (((1U<<RF_PU_RMX_TX_LEN)-1)<<RF_PU_RMX_TX_POS)
#define RF_PU_RMX_TX_UMSK                                       (~(((1U<<RF_PU_RMX_TX_LEN)-1)<<RF_PU_RMX_TX_POS))
#define RF_PU_LNA_TX                                            RF_PU_LNA_TX
#define RF_PU_LNA_TX_POS                                        (21U)
#define RF_PU_LNA_TX_LEN                                        (1U)
#define RF_PU_LNA_TX_MSK                                        (((1U<<RF_PU_LNA_TX_LEN)-1)<<RF_PU_LNA_TX_POS)
#define RF_PU_LNA_TX_UMSK                                       (~(((1U<<RF_PU_LNA_TX_LEN)-1)<<RF_PU_LNA_TX_POS))
#define RF_PU_PA_TX                                             RF_PU_PA_TX
#define RF_PU_PA_TX_POS                                         (22U)
#define RF_PU_PA_TX_LEN                                         (1U)
#define RF_PU_PA_TX_MSK                                         (((1U<<RF_PU_PA_TX_LEN)-1)<<RF_PU_PA_TX_POS)
#define RF_PU_PA_TX_UMSK                                        (~(((1U<<RF_PU_PA_TX_LEN)-1)<<RF_PU_PA_TX_POS))
#define RF_RX_BYPASS_EN_TX                                      RF_RX_BYPASS_EN_TX
#define RF_RX_BYPASS_EN_TX_POS                                  (23U)
#define RF_RX_BYPASS_EN_TX_LEN                                  (1U)
#define RF_RX_BYPASS_EN_TX_MSK                                  (((1U<<RF_RX_BYPASS_EN_TX_LEN)-1)<<RF_RX_BYPASS_EN_TX_POS)
#define RF_RX_BYPASS_EN_TX_UMSK                                 (~(((1U<<RF_RX_BYPASS_EN_TX_LEN)-1)<<RF_RX_BYPASS_EN_TX_POS))
#define RF_PA_SERI_CAP_EN_TX                                    RF_PA_SERI_CAP_EN_TX
#define RF_PA_SERI_CAP_EN_TX_POS                                (24U)
#define RF_PA_SERI_CAP_EN_TX_LEN                                (1U)
#define RF_PA_SERI_CAP_EN_TX_MSK                                (((1U<<RF_PA_SERI_CAP_EN_TX_LEN)-1)<<RF_PA_SERI_CAP_EN_TX_POS)
#define RF_PA_SERI_CAP_EN_TX_UMSK                               (~(((1U<<RF_PA_SERI_CAP_EN_TX_LEN)-1)<<RF_PA_SERI_CAP_EN_TX_POS))

/* 0x114 : Power up in RX state */
#define RF_PUCR_RX_OFFSET                                       (0x114)
#define RF_LODIST_TX_EN_RX                                      RF_LODIST_TX_EN_RX
#define RF_LODIST_TX_EN_RX_POS                                  (0U)
#define RF_LODIST_TX_EN_RX_LEN                                  (1U)
#define RF_LODIST_TX_EN_RX_MSK                                  (((1U<<RF_LODIST_TX_EN_RX_LEN)-1)<<RF_LODIST_TX_EN_RX_POS)
#define RF_LODIST_TX_EN_RX_UMSK                                 (~(((1U<<RF_LODIST_TX_EN_RX_LEN)-1)<<RF_LODIST_TX_EN_RX_POS))
#define RF_PU_TXBUF_RX                                          RF_PU_TXBUF_RX
#define RF_PU_TXBUF_RX_POS                                      (1U)
#define RF_PU_TXBUF_RX_LEN                                      (1U)
#define RF_PU_TXBUF_RX_MSK                                      (((1U<<RF_PU_TXBUF_RX_LEN)-1)<<RF_PU_TXBUF_RX_POS)
#define RF_PU_TXBUF_RX_UMSK                                     (~(((1U<<RF_PU_TXBUF_RX_LEN)-1)<<RF_PU_TXBUF_RX_POS))
#define RF_PU_RXBUF_RX                                          RF_PU_RXBUF_RX
#define RF_PU_RXBUF_RX_POS                                      (2U)
#define RF_PU_RXBUF_RX_LEN                                      (1U)
#define RF_PU_RXBUF_RX_MSK                                      (((1U<<RF_PU_RXBUF_RX_LEN)-1)<<RF_PU_RXBUF_RX_POS)
#define RF_PU_RXBUF_RX_UMSK                                     (~(((1U<<RF_PU_RXBUF_RX_LEN)-1)<<RF_PU_RXBUF_RX_POS))
#define RF_PU_DTC_RX                                            RF_PU_DTC_RX
#define RF_PU_DTC_RX_POS                                        (3U)
#define RF_PU_DTC_RX_LEN                                        (1U)
#define RF_PU_DTC_RX_MSK                                        (((1U<<RF_PU_DTC_RX_LEN)-1)<<RF_PU_DTC_RX_POS)
#define RF_PU_DTC_RX_UMSK                                       (~(((1U<<RF_PU_DTC_RX_LEN)-1)<<RF_PU_DTC_RX_POS))
#define RF_PU_ADPLL_SFREG_RX                                    RF_PU_ADPLL_SFREG_RX
#define RF_PU_ADPLL_SFREG_RX_POS                                (4U)
#define RF_PU_ADPLL_SFREG_RX_LEN                                (1U)
#define RF_PU_ADPLL_SFREG_RX_MSK                                (((1U<<RF_PU_ADPLL_SFREG_RX_LEN)-1)<<RF_PU_ADPLL_SFREG_RX_POS)
#define RF_PU_ADPLL_SFREG_RX_UMSK                               (~(((1U<<RF_PU_ADPLL_SFREG_RX_LEN)-1)<<RF_PU_ADPLL_SFREG_RX_POS))
#define RF_PU_ADPLL_ADC_RX                                      RF_PU_ADPLL_ADC_RX
#define RF_PU_ADPLL_ADC_RX_POS                                  (5U)
#define RF_PU_ADPLL_ADC_RX_LEN                                  (1U)
#define RF_PU_ADPLL_ADC_RX_MSK                                  (((1U<<RF_PU_ADPLL_ADC_RX_LEN)-1)<<RF_PU_ADPLL_ADC_RX_POS)
#define RF_PU_ADPLL_ADC_RX_UMSK                                 (~(((1U<<RF_PU_ADPLL_ADC_RX_LEN)-1)<<RF_PU_ADPLL_ADC_RX_POS))
#define RF_ADPLL_CLK_EN_RX                                      RF_ADPLL_CLK_EN_RX
#define RF_ADPLL_CLK_EN_RX_POS                                  (6U)
#define RF_ADPLL_CLK_EN_RX_LEN                                  (1U)
#define RF_ADPLL_CLK_EN_RX_MSK                                  (((1U<<RF_ADPLL_CLK_EN_RX_LEN)-1)<<RF_ADPLL_CLK_EN_RX_POS)
#define RF_ADPLL_CLK_EN_RX_UMSK                                 (~(((1U<<RF_ADPLL_CLK_EN_RX_LEN)-1)<<RF_ADPLL_CLK_EN_RX_POS))
#define RF_LOTPM_HFP_BYPASS_RX                                  RF_LOTPM_HFP_BYPASS_RX
#define RF_LOTPM_HFP_BYPASS_RX_POS                              (7U)
#define RF_LOTPM_HFP_BYPASS_RX_LEN                              (1U)
#define RF_LOTPM_HFP_BYPASS_RX_MSK                              (((1U<<RF_LOTPM_HFP_BYPASS_RX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_RX_POS)
#define RF_LOTPM_HFP_BYPASS_RX_UMSK                             (~(((1U<<RF_LOTPM_HFP_BYPASS_RX_LEN)-1)<<RF_LOTPM_HFP_BYPASS_RX_POS))
#define RF_LOTPM_LFP_BYPASS_RX                                  RF_LOTPM_LFP_BYPASS_RX
#define RF_LOTPM_LFP_BYPASS_RX_POS                              (8U)
#define RF_LOTPM_LFP_BYPASS_RX_LEN                              (1U)
#define RF_LOTPM_LFP_BYPASS_RX_MSK                              (((1U<<RF_LOTPM_LFP_BYPASS_RX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_RX_POS)
#define RF_LOTPM_LFP_BYPASS_RX_UMSK                             (~(((1U<<RF_LOTPM_LFP_BYPASS_RX_LEN)-1)<<RF_LOTPM_LFP_BYPASS_RX_POS))
#define RF_LOTPM_HFP_CLK_EN_RX                                  RF_LOTPM_HFP_CLK_EN_RX
#define RF_LOTPM_HFP_CLK_EN_RX_POS                              (9U)
#define RF_LOTPM_HFP_CLK_EN_RX_LEN                              (1U)
#define RF_LOTPM_HFP_CLK_EN_RX_MSK                              (((1U<<RF_LOTPM_HFP_CLK_EN_RX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_RX_POS)
#define RF_LOTPM_HFP_CLK_EN_RX_UMSK                             (~(((1U<<RF_LOTPM_HFP_CLK_EN_RX_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_RX_POS))
#define RF_PU_FBDV_BUF_RX                                       RF_PU_FBDV_BUF_RX
#define RF_PU_FBDV_BUF_RX_POS                                   (10U)
#define RF_PU_FBDV_BUF_RX_LEN                                   (1U)
#define RF_PU_FBDV_BUF_RX_MSK                                   (((1U<<RF_PU_FBDV_BUF_RX_LEN)-1)<<RF_PU_FBDV_BUF_RX_POS)
#define RF_PU_FBDV_BUF_RX_UMSK                                  (~(((1U<<RF_PU_FBDV_BUF_RX_LEN)-1)<<RF_PU_FBDV_BUF_RX_POS))
#define RF_PU_FBDV_RX                                           RF_PU_FBDV_RX
#define RF_PU_FBDV_RX_POS                                       (11U)
#define RF_PU_FBDV_RX_LEN                                       (1U)
#define RF_PU_FBDV_RX_MSK                                       (((1U<<RF_PU_FBDV_RX_LEN)-1)<<RF_PU_FBDV_RX_POS)
#define RF_PU_FBDV_RX_UMSK                                      (~(((1U<<RF_PU_FBDV_RX_LEN)-1)<<RF_PU_FBDV_RX_POS))
#define RF_PU_VCO_RX                                            RF_PU_VCO_RX
#define RF_PU_VCO_RX_POS                                        (12U)
#define RF_PU_VCO_RX_LEN                                        (1U)
#define RF_PU_VCO_RX_MSK                                        (((1U<<RF_PU_VCO_RX_LEN)-1)<<RF_PU_VCO_RX_POS)
#define RF_PU_VCO_RX_UMSK                                       (~(((1U<<RF_PU_VCO_RX_LEN)-1)<<RF_PU_VCO_RX_POS))
#define RF_PU_VCO_LDO_RX                                        RF_PU_VCO_LDO_RX
#define RF_PU_VCO_LDO_RX_POS                                    (13U)
#define RF_PU_VCO_LDO_RX_LEN                                    (1U)
#define RF_PU_VCO_LDO_RX_MSK                                    (((1U<<RF_PU_VCO_LDO_RX_LEN)-1)<<RF_PU_VCO_LDO_RX_POS)
#define RF_PU_VCO_LDO_RX_UMSK                                   (~(((1U<<RF_PU_VCO_LDO_RX_LEN)-1)<<RF_PU_VCO_LDO_RX_POS))
#define RF_PU_LODIST_BODY_BIAS_RX                               RF_PU_LODIST_BODY_BIAS_RX
#define RF_PU_LODIST_BODY_BIAS_RX_POS                           (14U)
#define RF_PU_LODIST_BODY_BIAS_RX_LEN                           (1U)
#define RF_PU_LODIST_BODY_BIAS_RX_MSK                           (((1U<<RF_PU_LODIST_BODY_BIAS_RX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_RX_POS)
#define RF_PU_LODIST_BODY_BIAS_RX_UMSK                          (~(((1U<<RF_PU_LODIST_BODY_BIAS_RX_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_RX_POS))
#define RF_RXADC_CLK_EN_RX                                      RF_RXADC_CLK_EN_RX
#define RF_RXADC_CLK_EN_RX_POS                                  (15U)
#define RF_RXADC_CLK_EN_RX_LEN                                  (1U)
#define RF_RXADC_CLK_EN_RX_MSK                                  (((1U<<RF_RXADC_CLK_EN_RX_LEN)-1)<<RF_RXADC_CLK_EN_RX_POS)
#define RF_RXADC_CLK_EN_RX_UMSK                                 (~(((1U<<RF_RXADC_CLK_EN_RX_LEN)-1)<<RF_RXADC_CLK_EN_RX_POS))
#define RF_PU_RXADC_RX                                          RF_PU_RXADC_RX
#define RF_PU_RXADC_RX_POS                                      (16U)
#define RF_PU_RXADC_RX_LEN                                      (1U)
#define RF_PU_RXADC_RX_MSK                                      (((1U<<RF_PU_RXADC_RX_LEN)-1)<<RF_PU_RXADC_RX_POS)
#define RF_PU_RXADC_RX_UMSK                                     (~(((1U<<RF_PU_RXADC_RX_LEN)-1)<<RF_PU_RXADC_RX_POS))
#define RF_PU_ROSDAC_RX                                         RF_PU_ROSDAC_RX
#define RF_PU_ROSDAC_RX_POS                                     (17U)
#define RF_PU_ROSDAC_RX_LEN                                     (1U)
#define RF_PU_ROSDAC_RX_MSK                                     (((1U<<RF_PU_ROSDAC_RX_LEN)-1)<<RF_PU_ROSDAC_RX_POS)
#define RF_PU_ROSDAC_RX_UMSK                                    (~(((1U<<RF_PU_ROSDAC_RX_LEN)-1)<<RF_PU_ROSDAC_RX_POS))
#define RF_PU_RBB_PKDET_RX                                      RF_PU_RBB_PKDET_RX
#define RF_PU_RBB_PKDET_RX_POS                                  (18U)
#define RF_PU_RBB_PKDET_RX_LEN                                  (1U)
#define RF_PU_RBB_PKDET_RX_MSK                                  (((1U<<RF_PU_RBB_PKDET_RX_LEN)-1)<<RF_PU_RBB_PKDET_RX_POS)
#define RF_PU_RBB_PKDET_RX_UMSK                                 (~(((1U<<RF_PU_RBB_PKDET_RX_LEN)-1)<<RF_PU_RBB_PKDET_RX_POS))
#define RF_PU_RBB_RX                                            RF_PU_RBB_RX
#define RF_PU_RBB_RX_POS                                        (19U)
#define RF_PU_RBB_RX_LEN                                        (1U)
#define RF_PU_RBB_RX_MSK                                        (((1U<<RF_PU_RBB_RX_LEN)-1)<<RF_PU_RBB_RX_POS)
#define RF_PU_RBB_RX_UMSK                                       (~(((1U<<RF_PU_RBB_RX_LEN)-1)<<RF_PU_RBB_RX_POS))
#define RF_PU_RMX_RX                                            RF_PU_RMX_RX
#define RF_PU_RMX_RX_POS                                        (20U)
#define RF_PU_RMX_RX_LEN                                        (1U)
#define RF_PU_RMX_RX_MSK                                        (((1U<<RF_PU_RMX_RX_LEN)-1)<<RF_PU_RMX_RX_POS)
#define RF_PU_RMX_RX_UMSK                                       (~(((1U<<RF_PU_RMX_RX_LEN)-1)<<RF_PU_RMX_RX_POS))
#define RF_PU_LNA_RX                                            RF_PU_LNA_RX
#define RF_PU_LNA_RX_POS                                        (21U)
#define RF_PU_LNA_RX_LEN                                        (1U)
#define RF_PU_LNA_RX_MSK                                        (((1U<<RF_PU_LNA_RX_LEN)-1)<<RF_PU_LNA_RX_POS)
#define RF_PU_LNA_RX_UMSK                                       (~(((1U<<RF_PU_LNA_RX_LEN)-1)<<RF_PU_LNA_RX_POS))
#define RF_PU_PA_RX                                             RF_PU_PA_RX
#define RF_PU_PA_RX_POS                                         (22U)
#define RF_PU_PA_RX_LEN                                         (1U)
#define RF_PU_PA_RX_MSK                                         (((1U<<RF_PU_PA_RX_LEN)-1)<<RF_PU_PA_RX_POS)
#define RF_PU_PA_RX_UMSK                                        (~(((1U<<RF_PU_PA_RX_LEN)-1)<<RF_PU_PA_RX_POS))
#define RF_RX_BYPASS_EN_RX                                      RF_RX_BYPASS_EN_RX
#define RF_RX_BYPASS_EN_RX_POS                                  (23U)
#define RF_RX_BYPASS_EN_RX_LEN                                  (1U)
#define RF_RX_BYPASS_EN_RX_MSK                                  (((1U<<RF_RX_BYPASS_EN_RX_LEN)-1)<<RF_RX_BYPASS_EN_RX_POS)
#define RF_RX_BYPASS_EN_RX_UMSK                                 (~(((1U<<RF_RX_BYPASS_EN_RX_LEN)-1)<<RF_RX_BYPASS_EN_RX_POS))
#define RF_PA_SERI_CAP_EN_RX                                    RF_PA_SERI_CAP_EN_RX
#define RF_PA_SERI_CAP_EN_RX_POS                                (24U)
#define RF_PA_SERI_CAP_EN_RX_LEN                                (1U)
#define RF_PA_SERI_CAP_EN_RX_MSK                                (((1U<<RF_PA_SERI_CAP_EN_RX_LEN)-1)<<RF_PA_SERI_CAP_EN_RX_POS)
#define RF_PA_SERI_CAP_EN_RX_UMSK                               (~(((1U<<RF_PA_SERI_CAP_EN_RX_LEN)-1)<<RF_PA_SERI_CAP_EN_RX_POS))

/* 0x118 : Hardware read back of power up signals */
#define RF_PUCR_HW_OFFSET                                       (0x118)
#define RF_LODIST_TX_EN_HW                                      RF_LODIST_TX_EN_HW
#define RF_LODIST_TX_EN_HW_POS                                  (0U)
#define RF_LODIST_TX_EN_HW_LEN                                  (1U)
#define RF_LODIST_TX_EN_HW_MSK                                  (((1U<<RF_LODIST_TX_EN_HW_LEN)-1)<<RF_LODIST_TX_EN_HW_POS)
#define RF_LODIST_TX_EN_HW_UMSK                                 (~(((1U<<RF_LODIST_TX_EN_HW_LEN)-1)<<RF_LODIST_TX_EN_HW_POS))
#define RF_PU_TXBUF_HW                                          RF_PU_TXBUF_HW
#define RF_PU_TXBUF_HW_POS                                      (1U)
#define RF_PU_TXBUF_HW_LEN                                      (1U)
#define RF_PU_TXBUF_HW_MSK                                      (((1U<<RF_PU_TXBUF_HW_LEN)-1)<<RF_PU_TXBUF_HW_POS)
#define RF_PU_TXBUF_HW_UMSK                                     (~(((1U<<RF_PU_TXBUF_HW_LEN)-1)<<RF_PU_TXBUF_HW_POS))
#define RF_PU_RXBUF_HW                                          RF_PU_RXBUF_HW
#define RF_PU_RXBUF_HW_POS                                      (2U)
#define RF_PU_RXBUF_HW_LEN                                      (1U)
#define RF_PU_RXBUF_HW_MSK                                      (((1U<<RF_PU_RXBUF_HW_LEN)-1)<<RF_PU_RXBUF_HW_POS)
#define RF_PU_RXBUF_HW_UMSK                                     (~(((1U<<RF_PU_RXBUF_HW_LEN)-1)<<RF_PU_RXBUF_HW_POS))
#define RF_PU_DTC_HW                                            RF_PU_DTC_HW
#define RF_PU_DTC_HW_POS                                        (3U)
#define RF_PU_DTC_HW_LEN                                        (1U)
#define RF_PU_DTC_HW_MSK                                        (((1U<<RF_PU_DTC_HW_LEN)-1)<<RF_PU_DTC_HW_POS)
#define RF_PU_DTC_HW_UMSK                                       (~(((1U<<RF_PU_DTC_HW_LEN)-1)<<RF_PU_DTC_HW_POS))
#define RF_PU_ADPLL_SFREG_HW                                    RF_PU_ADPLL_SFREG_HW
#define RF_PU_ADPLL_SFREG_HW_POS                                (4U)
#define RF_PU_ADPLL_SFREG_HW_LEN                                (1U)
#define RF_PU_ADPLL_SFREG_HW_MSK                                (((1U<<RF_PU_ADPLL_SFREG_HW_LEN)-1)<<RF_PU_ADPLL_SFREG_HW_POS)
#define RF_PU_ADPLL_SFREG_HW_UMSK                               (~(((1U<<RF_PU_ADPLL_SFREG_HW_LEN)-1)<<RF_PU_ADPLL_SFREG_HW_POS))
#define RF_PU_ADPLL_ADC_HW                                      RF_PU_ADPLL_ADC_HW
#define RF_PU_ADPLL_ADC_HW_POS                                  (5U)
#define RF_PU_ADPLL_ADC_HW_LEN                                  (1U)
#define RF_PU_ADPLL_ADC_HW_MSK                                  (((1U<<RF_PU_ADPLL_ADC_HW_LEN)-1)<<RF_PU_ADPLL_ADC_HW_POS)
#define RF_PU_ADPLL_ADC_HW_UMSK                                 (~(((1U<<RF_PU_ADPLL_ADC_HW_LEN)-1)<<RF_PU_ADPLL_ADC_HW_POS))
#define RF_ADPLL_CLK_EN_HW                                      RF_ADPLL_CLK_EN_HW
#define RF_ADPLL_CLK_EN_HW_POS                                  (6U)
#define RF_ADPLL_CLK_EN_HW_LEN                                  (1U)
#define RF_ADPLL_CLK_EN_HW_MSK                                  (((1U<<RF_ADPLL_CLK_EN_HW_LEN)-1)<<RF_ADPLL_CLK_EN_HW_POS)
#define RF_ADPLL_CLK_EN_HW_UMSK                                 (~(((1U<<RF_ADPLL_CLK_EN_HW_LEN)-1)<<RF_ADPLL_CLK_EN_HW_POS))
#define RF_LOTPM_HFP_BYPASS_HW                                  RF_LOTPM_HFP_BYPASS_HW
#define RF_LOTPM_HFP_BYPASS_HW_POS                              (7U)
#define RF_LOTPM_HFP_BYPASS_HW_LEN                              (1U)
#define RF_LOTPM_HFP_BYPASS_HW_MSK                              (((1U<<RF_LOTPM_HFP_BYPASS_HW_LEN)-1)<<RF_LOTPM_HFP_BYPASS_HW_POS)
#define RF_LOTPM_HFP_BYPASS_HW_UMSK                             (~(((1U<<RF_LOTPM_HFP_BYPASS_HW_LEN)-1)<<RF_LOTPM_HFP_BYPASS_HW_POS))
#define RF_LOTPM_LFP_BYPASS_HW                                  RF_LOTPM_LFP_BYPASS_HW
#define RF_LOTPM_LFP_BYPASS_HW_POS                              (8U)
#define RF_LOTPM_LFP_BYPASS_HW_LEN                              (1U)
#define RF_LOTPM_LFP_BYPASS_HW_MSK                              (((1U<<RF_LOTPM_LFP_BYPASS_HW_LEN)-1)<<RF_LOTPM_LFP_BYPASS_HW_POS)
#define RF_LOTPM_LFP_BYPASS_HW_UMSK                             (~(((1U<<RF_LOTPM_LFP_BYPASS_HW_LEN)-1)<<RF_LOTPM_LFP_BYPASS_HW_POS))
#define RF_LOTPM_HFP_CLK_EN_HW                                  RF_LOTPM_HFP_CLK_EN_HW
#define RF_LOTPM_HFP_CLK_EN_HW_POS                              (9U)
#define RF_LOTPM_HFP_CLK_EN_HW_LEN                              (1U)
#define RF_LOTPM_HFP_CLK_EN_HW_MSK                              (((1U<<RF_LOTPM_HFP_CLK_EN_HW_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_HW_POS)
#define RF_LOTPM_HFP_CLK_EN_HW_UMSK                             (~(((1U<<RF_LOTPM_HFP_CLK_EN_HW_LEN)-1)<<RF_LOTPM_HFP_CLK_EN_HW_POS))
#define RF_PU_FBDV_BUF_HW                                       RF_PU_FBDV_BUF_HW
#define RF_PU_FBDV_BUF_HW_POS                                   (10U)
#define RF_PU_FBDV_BUF_HW_LEN                                   (1U)
#define RF_PU_FBDV_BUF_HW_MSK                                   (((1U<<RF_PU_FBDV_BUF_HW_LEN)-1)<<RF_PU_FBDV_BUF_HW_POS)
#define RF_PU_FBDV_BUF_HW_UMSK                                  (~(((1U<<RF_PU_FBDV_BUF_HW_LEN)-1)<<RF_PU_FBDV_BUF_HW_POS))
#define RF_PU_FBDV_HW                                           RF_PU_FBDV_HW
#define RF_PU_FBDV_HW_POS                                       (11U)
#define RF_PU_FBDV_HW_LEN                                       (1U)
#define RF_PU_FBDV_HW_MSK                                       (((1U<<RF_PU_FBDV_HW_LEN)-1)<<RF_PU_FBDV_HW_POS)
#define RF_PU_FBDV_HW_UMSK                                      (~(((1U<<RF_PU_FBDV_HW_LEN)-1)<<RF_PU_FBDV_HW_POS))
#define RF_PU_VCO_HW                                            RF_PU_VCO_HW
#define RF_PU_VCO_HW_POS                                        (12U)
#define RF_PU_VCO_HW_LEN                                        (1U)
#define RF_PU_VCO_HW_MSK                                        (((1U<<RF_PU_VCO_HW_LEN)-1)<<RF_PU_VCO_HW_POS)
#define RF_PU_VCO_HW_UMSK                                       (~(((1U<<RF_PU_VCO_HW_LEN)-1)<<RF_PU_VCO_HW_POS))
#define RF_PU_VCO_LDO_HW                                        RF_PU_VCO_LDO_HW
#define RF_PU_VCO_LDO_HW_POS                                    (13U)
#define RF_PU_VCO_LDO_HW_LEN                                    (1U)
#define RF_PU_VCO_LDO_HW_MSK                                    (((1U<<RF_PU_VCO_LDO_HW_LEN)-1)<<RF_PU_VCO_LDO_HW_POS)
#define RF_PU_VCO_LDO_HW_UMSK                                   (~(((1U<<RF_PU_VCO_LDO_HW_LEN)-1)<<RF_PU_VCO_LDO_HW_POS))
#define RF_PU_LODIST_BODY_BIAS_HW                               RF_PU_LODIST_BODY_BIAS_HW
#define RF_PU_LODIST_BODY_BIAS_HW_POS                           (14U)
#define RF_PU_LODIST_BODY_BIAS_HW_LEN                           (1U)
#define RF_PU_LODIST_BODY_BIAS_HW_MSK                           (((1U<<RF_PU_LODIST_BODY_BIAS_HW_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_HW_POS)
#define RF_PU_LODIST_BODY_BIAS_HW_UMSK                          (~(((1U<<RF_PU_LODIST_BODY_BIAS_HW_LEN)-1)<<RF_PU_LODIST_BODY_BIAS_HW_POS))
#define RF_RXADC_CLK_EN_HW                                      RF_RXADC_CLK_EN_HW
#define RF_RXADC_CLK_EN_HW_POS                                  (15U)
#define RF_RXADC_CLK_EN_HW_LEN                                  (1U)
#define RF_RXADC_CLK_EN_HW_MSK                                  (((1U<<RF_RXADC_CLK_EN_HW_LEN)-1)<<RF_RXADC_CLK_EN_HW_POS)
#define RF_RXADC_CLK_EN_HW_UMSK                                 (~(((1U<<RF_RXADC_CLK_EN_HW_LEN)-1)<<RF_RXADC_CLK_EN_HW_POS))
#define RF_PU_RXADC_HW                                          RF_PU_RXADC_HW
#define RF_PU_RXADC_HW_POS                                      (16U)
#define RF_PU_RXADC_HW_LEN                                      (1U)
#define RF_PU_RXADC_HW_MSK                                      (((1U<<RF_PU_RXADC_HW_LEN)-1)<<RF_PU_RXADC_HW_POS)
#define RF_PU_RXADC_HW_UMSK                                     (~(((1U<<RF_PU_RXADC_HW_LEN)-1)<<RF_PU_RXADC_HW_POS))
#define RF_PU_ROSDAC_HW                                         RF_PU_ROSDAC_HW
#define RF_PU_ROSDAC_HW_POS                                     (17U)
#define RF_PU_ROSDAC_HW_LEN                                     (1U)
#define RF_PU_ROSDAC_HW_MSK                                     (((1U<<RF_PU_ROSDAC_HW_LEN)-1)<<RF_PU_ROSDAC_HW_POS)
#define RF_PU_ROSDAC_HW_UMSK                                    (~(((1U<<RF_PU_ROSDAC_HW_LEN)-1)<<RF_PU_ROSDAC_HW_POS))
#define RF_PU_RBB_PKDET_HW                                      RF_PU_RBB_PKDET_HW
#define RF_PU_RBB_PKDET_HW_POS                                  (18U)
#define RF_PU_RBB_PKDET_HW_LEN                                  (1U)
#define RF_PU_RBB_PKDET_HW_MSK                                  (((1U<<RF_PU_RBB_PKDET_HW_LEN)-1)<<RF_PU_RBB_PKDET_HW_POS)
#define RF_PU_RBB_PKDET_HW_UMSK                                 (~(((1U<<RF_PU_RBB_PKDET_HW_LEN)-1)<<RF_PU_RBB_PKDET_HW_POS))
#define RF_PU_RBB_HW                                            RF_PU_RBB_HW
#define RF_PU_RBB_HW_POS                                        (19U)
#define RF_PU_RBB_HW_LEN                                        (1U)
#define RF_PU_RBB_HW_MSK                                        (((1U<<RF_PU_RBB_HW_LEN)-1)<<RF_PU_RBB_HW_POS)
#define RF_PU_RBB_HW_UMSK                                       (~(((1U<<RF_PU_RBB_HW_LEN)-1)<<RF_PU_RBB_HW_POS))
#define RF_PU_RMX_HW                                            RF_PU_RMX_HW
#define RF_PU_RMX_HW_POS                                        (20U)
#define RF_PU_RMX_HW_LEN                                        (1U)
#define RF_PU_RMX_HW_MSK                                        (((1U<<RF_PU_RMX_HW_LEN)-1)<<RF_PU_RMX_HW_POS)
#define RF_PU_RMX_HW_UMSK                                       (~(((1U<<RF_PU_RMX_HW_LEN)-1)<<RF_PU_RMX_HW_POS))
#define RF_PU_LNA_HW                                            RF_PU_LNA_HW
#define RF_PU_LNA_HW_POS                                        (21U)
#define RF_PU_LNA_HW_LEN                                        (1U)
#define RF_PU_LNA_HW_MSK                                        (((1U<<RF_PU_LNA_HW_LEN)-1)<<RF_PU_LNA_HW_POS)
#define RF_PU_LNA_HW_UMSK                                       (~(((1U<<RF_PU_LNA_HW_LEN)-1)<<RF_PU_LNA_HW_POS))
#define RF_PU_PA_HW                                             RF_PU_PA_HW
#define RF_PU_PA_HW_POS                                         (22U)
#define RF_PU_PA_HW_LEN                                         (1U)
#define RF_PU_PA_HW_MSK                                         (((1U<<RF_PU_PA_HW_LEN)-1)<<RF_PU_PA_HW_POS)
#define RF_PU_PA_HW_UMSK                                        (~(((1U<<RF_PU_PA_HW_LEN)-1)<<RF_PU_PA_HW_POS))
#define RF_RX_BYPASS_EN_HW                                      RF_RX_BYPASS_EN_HW
#define RF_RX_BYPASS_EN_HW_POS                                  (23U)
#define RF_RX_BYPASS_EN_HW_LEN                                  (1U)
#define RF_RX_BYPASS_EN_HW_MSK                                  (((1U<<RF_RX_BYPASS_EN_HW_LEN)-1)<<RF_RX_BYPASS_EN_HW_POS)
#define RF_RX_BYPASS_EN_HW_UMSK                                 (~(((1U<<RF_RX_BYPASS_EN_HW_LEN)-1)<<RF_RX_BYPASS_EN_HW_POS))
#define RF_PA_SERI_CAP_EN_HW                                    RF_PA_SERI_CAP_EN_HW
#define RF_PA_SERI_CAP_EN_HW_POS                                (24U)
#define RF_PA_SERI_CAP_EN_HW_LEN                                (1U)
#define RF_PA_SERI_CAP_EN_HW_MSK                                (((1U<<RF_PA_SERI_CAP_EN_HW_LEN)-1)<<RF_PA_SERI_CAP_EN_HW_POS)
#define RF_PA_SERI_CAP_EN_HW_UMSK                               (~(((1U<<RF_PA_SERI_CAP_EN_HW_LEN)-1)<<RF_PA_SERI_CAP_EN_HW_POS))
#define RF_RST_ADPLL_HW                                         RF_RST_ADPLL_HW
#define RF_RST_ADPLL_HW_POS                                     (25U)
#define RF_RST_ADPLL_HW_LEN                                     (1U)
#define RF_RST_ADPLL_HW_MSK                                     (((1U<<RF_RST_ADPLL_HW_LEN)-1)<<RF_RST_ADPLL_HW_POS)
#define RF_RST_ADPLL_HW_UMSK                                    (~(((1U<<RF_RST_ADPLL_HW_LEN)-1)<<RF_RST_ADPLL_HW_POS))
#define RF_RST_LOTPM_HFP_HW                                     RF_RST_LOTPM_HFP_HW
#define RF_RST_LOTPM_HFP_HW_POS                                 (26U)
#define RF_RST_LOTPM_HFP_HW_LEN                                 (1U)
#define RF_RST_LOTPM_HFP_HW_MSK                                 (((1U<<RF_RST_LOTPM_HFP_HW_LEN)-1)<<RF_RST_LOTPM_HFP_HW_POS)
#define RF_RST_LOTPM_HFP_HW_UMSK                                (~(((1U<<RF_RST_LOTPM_HFP_HW_LEN)-1)<<RF_RST_LOTPM_HFP_HW_POS))
#define RF_RST_FBDV_HW                                          RF_RST_FBDV_HW
#define RF_RST_FBDV_HW_POS                                      (27U)
#define RF_RST_FBDV_HW_LEN                                      (1U)
#define RF_RST_FBDV_HW_MSK                                      (((1U<<RF_RST_FBDV_HW_LEN)-1)<<RF_RST_FBDV_HW_POS)
#define RF_RST_FBDV_HW_UMSK                                     (~(((1U<<RF_RST_FBDV_HW_LEN)-1)<<RF_RST_FBDV_HW_POS))

/* 0x11C : non_reg_readback */
#define RF_NON_REG_READBACK_OFFSET                              (0x11C)
#define RF_PPU_TESTBUF_HW                                       RF_PPU_TESTBUF_HW
#define RF_PPU_TESTBUF_HW_POS                                   (5U)
#define RF_PPU_TESTBUF_HW_LEN                                   (1U)
#define RF_PPU_TESTBUF_HW_MSK                                   (((1U<<RF_PPU_TESTBUF_HW_LEN)-1)<<RF_PPU_TESTBUF_HW_POS)
#define RF_PPU_TESTBUF_HW_UMSK                                  (~(((1U<<RF_PPU_TESTBUF_HW_LEN)-1)<<RF_PPU_TESTBUF_HW_POS))
#define RF_PPU_TXBUF_HW                                         RF_PPU_TXBUF_HW
#define RF_PPU_TXBUF_HW_POS                                     (6U)
#define RF_PPU_TXBUF_HW_LEN                                     (1U)
#define RF_PPU_TXBUF_HW_MSK                                     (((1U<<RF_PPU_TXBUF_HW_LEN)-1)<<RF_PPU_TXBUF_HW_POS)
#define RF_PPU_TXBUF_HW_UMSK                                    (~(((1U<<RF_PPU_TXBUF_HW_LEN)-1)<<RF_PPU_TXBUF_HW_POS))
#define RF_PPU_RXBUF_HW                                         RF_PPU_RXBUF_HW
#define RF_PPU_RXBUF_HW_POS                                     (7U)
#define RF_PPU_RXBUF_HW_LEN                                     (1U)
#define RF_PPU_RXBUF_HW_MSK                                     (((1U<<RF_PPU_RXBUF_HW_LEN)-1)<<RF_PPU_RXBUF_HW_POS)
#define RF_PPU_RXBUF_HW_UMSK                                    (~(((1U<<RF_PPU_RXBUF_HW_LEN)-1)<<RF_PPU_RXBUF_HW_POS))
#define RF_PPU_ADPLL_SFREG_HW                                   RF_PPU_ADPLL_SFREG_HW
#define RF_PPU_ADPLL_SFREG_HW_POS                               (8U)
#define RF_PPU_ADPLL_SFREG_HW_LEN                               (1U)
#define RF_PPU_ADPLL_SFREG_HW_MSK                               (((1U<<RF_PPU_ADPLL_SFREG_HW_LEN)-1)<<RF_PPU_ADPLL_SFREG_HW_POS)
#define RF_PPU_ADPLL_SFREG_HW_UMSK                              (~(((1U<<RF_PPU_ADPLL_SFREG_HW_LEN)-1)<<RF_PPU_ADPLL_SFREG_HW_POS))
#define RF_PPU_FBDV_HW                                          RF_PPU_FBDV_HW
#define RF_PPU_FBDV_HW_POS                                      (9U)
#define RF_PPU_FBDV_HW_LEN                                      (1U)
#define RF_PPU_FBDV_HW_MSK                                      (((1U<<RF_PPU_FBDV_HW_LEN)-1)<<RF_PPU_FBDV_HW_POS)
#define RF_PPU_FBDV_HW_UMSK                                     (~(((1U<<RF_PPU_FBDV_HW_LEN)-1)<<RF_PPU_FBDV_HW_POS))
#define RF_PUD_VCO_HW                                           RF_PUD_VCO_HW
#define RF_PUD_VCO_HW_POS                                       (10U)
#define RF_PUD_VCO_HW_LEN                                       (1U)
#define RF_PUD_VCO_HW_MSK                                       (((1U<<RF_PUD_VCO_HW_LEN)-1)<<RF_PUD_VCO_HW_POS)
#define RF_PUD_VCO_HW_UMSK                                      (~(((1U<<RF_PUD_VCO_HW_LEN)-1)<<RF_PUD_VCO_HW_POS))
#define RF_PPU_VCO_HW                                           RF_PPU_VCO_HW
#define RF_PPU_VCO_HW_POS                                       (11U)
#define RF_PPU_VCO_HW_LEN                                       (1U)
#define RF_PPU_VCO_HW_MSK                                       (((1U<<RF_PPU_VCO_HW_LEN)-1)<<RF_PPU_VCO_HW_POS)
#define RF_PPU_VCO_HW_UMSK                                      (~(((1U<<RF_PPU_VCO_HW_LEN)-1)<<RF_PPU_VCO_HW_POS))
#define RF_PPU_VCO_LDO_HW                                       RF_PPU_VCO_LDO_HW
#define RF_PPU_VCO_LDO_HW_POS                                   (12U)
#define RF_PPU_VCO_LDO_HW_LEN                                   (1U)
#define RF_PPU_VCO_LDO_HW_MSK                                   (((1U<<RF_PPU_VCO_LDO_HW_LEN)-1)<<RF_PPU_VCO_LDO_HW_POS)
#define RF_PPU_VCO_LDO_HW_UMSK                                  (~(((1U<<RF_PPU_VCO_LDO_HW_LEN)-1)<<RF_PPU_VCO_LDO_HW_POS))
#define RF_PPU_LODIST_BODY_BIAS_HW                              RF_PPU_LODIST_BODY_BIAS_HW
#define RF_PPU_LODIST_BODY_BIAS_HW_POS                          (13U)
#define RF_PPU_LODIST_BODY_BIAS_HW_LEN                          (1U)
#define RF_PPU_LODIST_BODY_BIAS_HW_MSK                          (((1U<<RF_PPU_LODIST_BODY_BIAS_HW_LEN)-1)<<RF_PPU_LODIST_BODY_BIAS_HW_POS)
#define RF_PPU_LODIST_BODY_BIAS_HW_UMSK                         (~(((1U<<RF_PPU_LODIST_BODY_BIAS_HW_LEN)-1)<<RF_PPU_LODIST_BODY_BIAS_HW_POS))
#define RF_PPU_RBB_HW                                           RF_PPU_RBB_HW
#define RF_PPU_RBB_HW_POS                                       (15U)
#define RF_PPU_RBB_HW_LEN                                       (1U)
#define RF_PPU_RBB_HW_MSK                                       (((1U<<RF_PPU_RBB_HW_LEN)-1)<<RF_PPU_RBB_HW_POS)
#define RF_PPU_RBB_HW_UMSK                                      (~(((1U<<RF_PPU_RBB_HW_LEN)-1)<<RF_PPU_RBB_HW_POS))
#define RF_PPU_LNA_HW                                           RF_PPU_LNA_HW
#define RF_PPU_LNA_HW_POS                                       (16U)
#define RF_PPU_LNA_HW_LEN                                       (1U)
#define RF_PPU_LNA_HW_MSK                                       (((1U<<RF_PPU_LNA_HW_LEN)-1)<<RF_PPU_LNA_HW_POS)
#define RF_PPU_LNA_HW_UMSK                                      (~(((1U<<RF_PPU_LNA_HW_LEN)-1)<<RF_PPU_LNA_HW_POS))

/* 0x120 : Register control of TX/RX gain */
#define RF_TRX_GAIN_BW_OFFSET                                   (0x120)
#define RF_GC_LNA                                               RF_GC_LNA
#define RF_GC_LNA_POS                                           (0U)
#define RF_GC_LNA_LEN                                           (3U)
#define RF_GC_LNA_MSK                                           (((1U<<RF_GC_LNA_LEN)-1)<<RF_GC_LNA_POS)
#define RF_GC_LNA_UMSK                                          (~(((1U<<RF_GC_LNA_LEN)-1)<<RF_GC_LNA_POS))
#define RF_GC_RBB1                                              RF_GC_RBB1
#define RF_GC_RBB1_POS                                          (3U)
#define RF_GC_RBB1_LEN                                          (2U)
#define RF_GC_RBB1_MSK                                          (((1U<<RF_GC_RBB1_LEN)-1)<<RF_GC_RBB1_POS)
#define RF_GC_RBB1_UMSK                                         (~(((1U<<RF_GC_RBB1_LEN)-1)<<RF_GC_RBB1_POS))
#define RF_GC_RBB2                                              RF_GC_RBB2
#define RF_GC_RBB2_POS                                          (5U)
#define RF_GC_RBB2_LEN                                          (3U)
#define RF_GC_RBB2_MSK                                          (((1U<<RF_GC_RBB2_LEN)-1)<<RF_GC_RBB2_POS)
#define RF_GC_RBB2_UMSK                                         (~(((1U<<RF_GC_RBB2_LEN)-1)<<RF_GC_RBB2_POS))
#define RF_RBB_BW                                               RF_RBB_BW
#define RF_RBB_BW_POS                                           (8U)
#define RF_RBB_BW_LEN                                           (1U)
#define RF_RBB_BW_MSK                                           (((1U<<RF_RBB_BW_LEN)-1)<<RF_RBB_BW_POS)
#define RF_RBB_BW_UMSK                                          (~(((1U<<RF_RBB_BW_LEN)-1)<<RF_RBB_BW_POS))
#define RF_PA_REF_DAC                                           RF_PA_REF_DAC
#define RF_PA_REF_DAC_POS                                       (12U)
#define RF_PA_REF_DAC_LEN                                       (5U)
#define RF_PA_REF_DAC_MSK                                       (((1U<<RF_PA_REF_DAC_LEN)-1)<<RF_PA_REF_DAC_POS)
#define RF_PA_REF_DAC_UMSK                                      (~(((1U<<RF_PA_REF_DAC_LEN)-1)<<RF_PA_REF_DAC_POS))
#define RF_PA_INBUF_UNIT                                        RF_PA_INBUF_UNIT
#define RF_PA_INBUF_UNIT_POS                                    (20U)
#define RF_PA_INBUF_UNIT_LEN                                    (3U)
#define RF_PA_INBUF_UNIT_MSK                                    (((1U<<RF_PA_INBUF_UNIT_LEN)-1)<<RF_PA_INBUF_UNIT_POS)
#define RF_PA_INBUF_UNIT_UMSK                                   (~(((1U<<RF_PA_INBUF_UNIT_LEN)-1)<<RF_PA_INBUF_UNIT_POS))

/* 0x124 : Hardware read back of TX/RX gain */
#define RF_TRX_GAIN_BW_HW_OFFSET                                (0x124)
#define RF_GC_LNA_HW                                            RF_GC_LNA_HW
#define RF_GC_LNA_HW_POS                                        (0U)
#define RF_GC_LNA_HW_LEN                                        (3U)
#define RF_GC_LNA_HW_MSK                                        (((1U<<RF_GC_LNA_HW_LEN)-1)<<RF_GC_LNA_HW_POS)
#define RF_GC_LNA_HW_UMSK                                       (~(((1U<<RF_GC_LNA_HW_LEN)-1)<<RF_GC_LNA_HW_POS))
#define RF_GC_RBB1_HW                                           RF_GC_RBB1_HW
#define RF_GC_RBB1_HW_POS                                       (3U)
#define RF_GC_RBB1_HW_LEN                                       (2U)
#define RF_GC_RBB1_HW_MSK                                       (((1U<<RF_GC_RBB1_HW_LEN)-1)<<RF_GC_RBB1_HW_POS)
#define RF_GC_RBB1_HW_UMSK                                      (~(((1U<<RF_GC_RBB1_HW_LEN)-1)<<RF_GC_RBB1_HW_POS))
#define RF_GC_RBB2_HW                                           RF_GC_RBB2_HW
#define RF_GC_RBB2_HW_POS                                       (5U)
#define RF_GC_RBB2_HW_LEN                                       (3U)
#define RF_GC_RBB2_HW_MSK                                       (((1U<<RF_GC_RBB2_HW_LEN)-1)<<RF_GC_RBB2_HW_POS)
#define RF_GC_RBB2_HW_UMSK                                      (~(((1U<<RF_GC_RBB2_HW_LEN)-1)<<RF_GC_RBB2_HW_POS))
#define RF_RBB_BW_HW                                            RF_RBB_BW_HW
#define RF_RBB_BW_HW_POS                                        (8U)
#define RF_RBB_BW_HW_LEN                                        (1U)
#define RF_RBB_BW_HW_MSK                                        (((1U<<RF_RBB_BW_HW_LEN)-1)<<RF_RBB_BW_HW_POS)
#define RF_RBB_BW_HW_UMSK                                       (~(((1U<<RF_RBB_BW_HW_LEN)-1)<<RF_RBB_BW_HW_POS))
#define RF_PA_REF_DAC_HW                                        RF_PA_REF_DAC_HW
#define RF_PA_REF_DAC_HW_POS                                    (12U)
#define RF_PA_REF_DAC_HW_LEN                                    (5U)
#define RF_PA_REF_DAC_HW_MSK                                    (((1U<<RF_PA_REF_DAC_HW_LEN)-1)<<RF_PA_REF_DAC_HW_POS)
#define RF_PA_REF_DAC_HW_UMSK                                   (~(((1U<<RF_PA_REF_DAC_HW_LEN)-1)<<RF_PA_REF_DAC_HW_POS))
#define RF_PA_INBUF_UNIT_HW                                     RF_PA_INBUF_UNIT_HW
#define RF_PA_INBUF_UNIT_HW_POS                                 (20U)
#define RF_PA_INBUF_UNIT_HW_LEN                                 (3U)
#define RF_PA_INBUF_UNIT_HW_MSK                                 (((1U<<RF_PA_INBUF_UNIT_HW_LEN)-1)<<RF_PA_INBUF_UNIT_HW_POS)
#define RF_PA_INBUF_UNIT_HW_UMSK                                (~(((1U<<RF_PA_INBUF_UNIT_HW_LEN)-1)<<RF_PA_INBUF_UNIT_HW_POS))

/* 0x128 : DC Test register 0 */
#define RF_DCTEST_ACTEST_OFFSET                                 (0x128)
#define RF_DC_TP_OUT_EN                                         RF_DC_TP_OUT_EN
#define RF_DC_TP_OUT_EN_POS                                     (4U)
#define RF_DC_TP_OUT_EN_LEN                                     (2U)
#define RF_DC_TP_OUT_EN_MSK                                     (((1U<<RF_DC_TP_OUT_EN_LEN)-1)<<RF_DC_TP_OUT_EN_POS)
#define RF_DC_TP_OUT_EN_UMSK                                    (~(((1U<<RF_DC_TP_OUT_EN_LEN)-1)<<RF_DC_TP_OUT_EN_POS))
#define RF_ATEST_OUT_EN                                         RF_ATEST_OUT_EN
#define RF_ATEST_OUT_EN_POS                                     (6U)
#define RF_ATEST_OUT_EN_LEN                                     (2U)
#define RF_ATEST_OUT_EN_MSK                                     (((1U<<RF_ATEST_OUT_EN_LEN)-1)<<RF_ATEST_OUT_EN_POS)
#define RF_ATEST_OUT_EN_UMSK                                    (~(((1U<<RF_ATEST_OUT_EN_LEN)-1)<<RF_ATEST_OUT_EN_POS))
#define RF_TEN_DTC                                              RF_TEN_DTC
#define RF_TEN_DTC_POS                                          (17U)
#define RF_TEN_DTC_LEN                                          (1U)
#define RF_TEN_DTC_MSK                                          (((1U<<RF_TEN_DTC_LEN)-1)<<RF_TEN_DTC_POS)
#define RF_TEN_DTC_UMSK                                         (~(((1U<<RF_TEN_DTC_LEN)-1)<<RF_TEN_DTC_POS))
#define RF_TEN_RBB                                              RF_TEN_RBB
#define RF_TEN_RBB_POS                                          (18U)
#define RF_TEN_RBB_LEN                                          (1U)
#define RF_TEN_RBB_MSK                                          (((1U<<RF_TEN_RBB_LEN)-1)<<RF_TEN_RBB_POS)
#define RF_TEN_RBB_UMSK                                         (~(((1U<<RF_TEN_RBB_LEN)-1)<<RF_TEN_RBB_POS))
#define RF_TEN_RBB_ACTEST                                       RF_TEN_RBB_ACTEST
#define RF_TEN_RBB_ACTEST_POS                                   (19U)
#define RF_TEN_RBB_ACTEST_LEN                                   (1U)
#define RF_TEN_RBB_ACTEST_MSK                                   (((1U<<RF_TEN_RBB_ACTEST_LEN)-1)<<RF_TEN_RBB_ACTEST_POS)
#define RF_TEN_RBB_ACTEST_UMSK                                  (~(((1U<<RF_TEN_RBB_ACTEST_LEN)-1)<<RF_TEN_RBB_ACTEST_POS))
#define RF_TEN_ADPLL_ADC                                        RF_TEN_ADPLL_ADC
#define RF_TEN_ADPLL_ADC_POS                                    (20U)
#define RF_TEN_ADPLL_ADC_LEN                                    (1U)
#define RF_TEN_ADPLL_ADC_MSK                                    (((1U<<RF_TEN_ADPLL_ADC_LEN)-1)<<RF_TEN_ADPLL_ADC_POS)
#define RF_TEN_ADPLL_ADC_UMSK                                   (~(((1U<<RF_TEN_ADPLL_ADC_LEN)-1)<<RF_TEN_ADPLL_ADC_POS))
#define RF_TEN_VCO                                              RF_TEN_VCO
#define RF_TEN_VCO_POS                                          (21U)
#define RF_TEN_VCO_LEN                                          (1U)
#define RF_TEN_VCO_MSK                                          (((1U<<RF_TEN_VCO_LEN)-1)<<RF_TEN_VCO_POS)
#define RF_TEN_VCO_UMSK                                         (~(((1U<<RF_TEN_VCO_LEN)-1)<<RF_TEN_VCO_POS))
#define RF_TEN_RXADC                                            RF_TEN_RXADC
#define RF_TEN_RXADC_POS                                        (22U)
#define RF_TEN_RXADC_LEN                                        (1U)
#define RF_TEN_RXADC_MSK                                        (((1U<<RF_TEN_RXADC_LEN)-1)<<RF_TEN_RXADC_POS)
#define RF_TEN_RXADC_UMSK                                       (~(((1U<<RF_TEN_RXADC_LEN)-1)<<RF_TEN_RXADC_POS))
#define RF_TEN_RRF1                                             RF_TEN_RRF1
#define RF_TEN_RRF1_POS                                         (23U)
#define RF_TEN_RRF1_LEN                                         (1U)
#define RF_TEN_RRF1_MSK                                         (((1U<<RF_TEN_RRF1_LEN)-1)<<RF_TEN_RRF1_POS)
#define RF_TEN_RRF1_UMSK                                        (~(((1U<<RF_TEN_RRF1_LEN)-1)<<RF_TEN_RRF1_POS))
#define RF_TEN_RRF0                                             RF_TEN_RRF0
#define RF_TEN_RRF0_POS                                         (24U)
#define RF_TEN_RRF0_LEN                                         (1U)
#define RF_TEN_RRF0_MSK                                         (((1U<<RF_TEN_RRF0_LEN)-1)<<RF_TEN_RRF0_POS)
#define RF_TEN_RRF0_UMSK                                        (~(((1U<<RF_TEN_RRF0_LEN)-1)<<RF_TEN_RRF0_POS))
#define RF_TEN_PA_1                                             RF_TEN_PA_1
#define RF_TEN_PA_1_POS                                         (25U)
#define RF_TEN_PA_1_LEN                                         (1U)
#define RF_TEN_PA_1_MSK                                         (((1U<<RF_TEN_PA_1_LEN)-1)<<RF_TEN_PA_1_POS)
#define RF_TEN_PA_1_UMSK                                        (~(((1U<<RF_TEN_PA_1_LEN)-1)<<RF_TEN_PA_1_POS))
#define RF_TEN_PA_0                                             RF_TEN_PA_0
#define RF_TEN_PA_0_POS                                         (26U)
#define RF_TEN_PA_0_LEN                                         (1U)
#define RF_TEN_PA_0_MSK                                         (((1U<<RF_TEN_PA_0_LEN)-1)<<RF_TEN_PA_0_POS)
#define RF_TEN_PA_0_UMSK                                        (~(((1U<<RF_TEN_PA_0_LEN)-1)<<RF_TEN_PA_0_POS))
#define RF_TEN_LODIST                                           RF_TEN_LODIST
#define RF_TEN_LODIST_POS                                       (27U)
#define RF_TEN_LODIST_LEN                                       (1U)
#define RF_TEN_LODIST_MSK                                       (((1U<<RF_TEN_LODIST_LEN)-1)<<RF_TEN_LODIST_POS)
#define RF_TEN_LODIST_UMSK                                      (~(((1U<<RF_TEN_LODIST_LEN)-1)<<RF_TEN_LODIST_POS))
#define RF_TEN_DLL                                              RF_TEN_DLL
#define RF_TEN_DLL_POS                                          (28U)
#define RF_TEN_DLL_LEN                                          (1U)
#define RF_TEN_DLL_MSK                                          (((1U<<RF_TEN_DLL_LEN)-1)<<RF_TEN_DLL_POS)
#define RF_TEN_DLL_UMSK                                         (~(((1U<<RF_TEN_DLL_LEN)-1)<<RF_TEN_DLL_POS))
#define RF_TEN_MBG                                              RF_TEN_MBG
#define RF_TEN_MBG_POS                                          (31U)
#define RF_TEN_MBG_LEN                                          (1U)
#define RF_TEN_MBG_MSK                                          (((1U<<RF_TEN_MBG_LEN)-1)<<RF_TEN_MBG_POS)
#define RF_TEN_MBG_UMSK                                         (~(((1U<<RF_TEN_MBG_LEN)-1)<<RF_TEN_MBG_POS))

/* 0x12C : LO Bias Mode registers */
#define RF_DTEST_OFFSET                                         (0x12C)
#define RF_DTEST_PULLDOWN                                       RF_DTEST_PULLDOWN
#define RF_DTEST_PULLDOWN_POS                                   (0U)
#define RF_DTEST_PULLDOWN_LEN                                   (1U)
#define RF_DTEST_PULLDOWN_MSK                                   (((1U<<RF_DTEST_PULLDOWN_LEN)-1)<<RF_DTEST_PULLDOWN_POS)
#define RF_DTEST_PULLDOWN_UMSK                                  (~(((1U<<RF_DTEST_PULLDOWN_LEN)-1)<<RF_DTEST_PULLDOWN_POS))
#define RF_DTEST_EN_RXADC_Q                                     RF_DTEST_EN_RXADC_Q
#define RF_DTEST_EN_RXADC_Q_POS                                 (23U)
#define RF_DTEST_EN_RXADC_Q_LEN                                 (1U)
#define RF_DTEST_EN_RXADC_Q_MSK                                 (((1U<<RF_DTEST_EN_RXADC_Q_LEN)-1)<<RF_DTEST_EN_RXADC_Q_POS)
#define RF_DTEST_EN_RXADC_Q_UMSK                                (~(((1U<<RF_DTEST_EN_RXADC_Q_LEN)-1)<<RF_DTEST_EN_RXADC_Q_POS))
#define RF_DTEST_EN_RXADC_I                                     RF_DTEST_EN_RXADC_I
#define RF_DTEST_EN_RXADC_I_POS                                 (24U)
#define RF_DTEST_EN_RXADC_I_LEN                                 (1U)
#define RF_DTEST_EN_RXADC_I_MSK                                 (((1U<<RF_DTEST_EN_RXADC_I_LEN)-1)<<RF_DTEST_EN_RXADC_I_POS)
#define RF_DTEST_EN_RXADC_I_UMSK                                (~(((1U<<RF_DTEST_EN_RXADC_I_LEN)-1)<<RF_DTEST_EN_RXADC_I_POS))
#define RF_DTEST_EN_ADPLL_ADC                                   RF_DTEST_EN_ADPLL_ADC
#define RF_DTEST_EN_ADPLL_ADC_POS                               (25U)
#define RF_DTEST_EN_ADPLL_ADC_LEN                               (1U)
#define RF_DTEST_EN_ADPLL_ADC_MSK                               (((1U<<RF_DTEST_EN_ADPLL_ADC_LEN)-1)<<RF_DTEST_EN_ADPLL_ADC_POS)
#define RF_DTEST_EN_ADPLL_ADC_UMSK                              (~(((1U<<RF_DTEST_EN_ADPLL_ADC_LEN)-1)<<RF_DTEST_EN_ADPLL_ADC_POS))
#define RF_DTEST_EN_MOD4                                        RF_DTEST_EN_MOD4
#define RF_DTEST_EN_MOD4_POS                                    (26U)
#define RF_DTEST_EN_MOD4_LEN                                    (1U)
#define RF_DTEST_EN_MOD4_MSK                                    (((1U<<RF_DTEST_EN_MOD4_LEN)-1)<<RF_DTEST_EN_MOD4_POS)
#define RF_DTEST_EN_MOD4_UMSK                                   (~(((1U<<RF_DTEST_EN_MOD4_LEN)-1)<<RF_DTEST_EN_MOD4_POS))
#define RF_DTEST_EN_FREF                                        RF_DTEST_EN_FREF
#define RF_DTEST_EN_FREF_POS                                    (27U)
#define RF_DTEST_EN_FREF_LEN                                    (1U)
#define RF_DTEST_EN_FREF_MSK                                    (((1U<<RF_DTEST_EN_FREF_LEN)-1)<<RF_DTEST_EN_FREF_POS)
#define RF_DTEST_EN_FREF_UMSK                                   (~(((1U<<RF_DTEST_EN_FREF_LEN)-1)<<RF_DTEST_EN_FREF_POS))
#define RF_DTEST_EN_DTC_OUT                                     RF_DTEST_EN_DTC_OUT
#define RF_DTEST_EN_DTC_OUT_POS                                 (28U)
#define RF_DTEST_EN_DTC_OUT_LEN                                 (1U)
#define RF_DTEST_EN_DTC_OUT_MSK                                 (((1U<<RF_DTEST_EN_DTC_OUT_LEN)-1)<<RF_DTEST_EN_DTC_OUT_POS)
#define RF_DTEST_EN_DTC_OUT_UMSK                                (~(((1U<<RF_DTEST_EN_DTC_OUT_LEN)-1)<<RF_DTEST_EN_DTC_OUT_POS))
#define RF_DTEST_EN_DTC_IN                                      RF_DTEST_EN_DTC_IN
#define RF_DTEST_EN_DTC_IN_POS                                  (29U)
#define RF_DTEST_EN_DTC_IN_LEN                                  (1U)
#define RF_DTEST_EN_DTC_IN_MSK                                  (((1U<<RF_DTEST_EN_DTC_IN_LEN)-1)<<RF_DTEST_EN_DTC_IN_POS)
#define RF_DTEST_EN_DTC_IN_UMSK                                 (~(((1U<<RF_DTEST_EN_DTC_IN_LEN)-1)<<RF_DTEST_EN_DTC_IN_POS))

/* 0x130 : adpll_test */
#define RF_ADPLL_TEST_OFFSET                                    (0x130)
#define RF_ADPLL_TEST_OUT                                       RF_ADPLL_TEST_OUT
#define RF_ADPLL_TEST_OUT_POS                                   (0U)
#define RF_ADPLL_TEST_OUT_LEN                                   (16U)
#define RF_ADPLL_TEST_OUT_MSK                                   (((1U<<RF_ADPLL_TEST_OUT_LEN)-1)<<RF_ADPLL_TEST_OUT_POS)
#define RF_ADPLL_TEST_OUT_UMSK                                  (~(((1U<<RF_ADPLL_TEST_OUT_LEN)-1)<<RF_ADPLL_TEST_OUT_POS))
#define RF_ADPLL_TEST_DATA_SEL                                  RF_ADPLL_TEST_DATA_SEL
#define RF_ADPLL_TEST_DATA_SEL_POS                              (16U)
#define RF_ADPLL_TEST_DATA_SEL_LEN                              (3U)
#define RF_ADPLL_TEST_DATA_SEL_MSK                              (((1U<<RF_ADPLL_TEST_DATA_SEL_LEN)-1)<<RF_ADPLL_TEST_DATA_SEL_POS)
#define RF_ADPLL_TEST_DATA_SEL_UMSK                             (~(((1U<<RF_ADPLL_TEST_DATA_SEL_LEN)-1)<<RF_ADPLL_TEST_DATA_SEL_POS))
#define RF_ADPLL_TEST_START_SEL                                 RF_ADPLL_TEST_START_SEL
#define RF_ADPLL_TEST_START_SEL_POS                             (20U)
#define RF_ADPLL_TEST_START_SEL_LEN                             (2U)
#define RF_ADPLL_TEST_START_SEL_MSK                             (((1U<<RF_ADPLL_TEST_START_SEL_LEN)-1)<<RF_ADPLL_TEST_START_SEL_POS)
#define RF_ADPLL_TEST_START_SEL_UMSK                            (~(((1U<<RF_ADPLL_TEST_START_SEL_LEN)-1)<<RF_ADPLL_TEST_START_SEL_POS))
#define RF_ADPLL_TEST_EN                                        RF_ADPLL_TEST_EN
#define RF_ADPLL_TEST_EN_POS                                    (24U)
#define RF_ADPLL_TEST_EN_LEN                                    (1U)
#define RF_ADPLL_TEST_EN_MSK                                    (((1U<<RF_ADPLL_TEST_EN_LEN)-1)<<RF_ADPLL_TEST_EN_POS)
#define RF_ADPLL_TEST_EN_UMSK                                   (~(((1U<<RF_ADPLL_TEST_EN_LEN)-1)<<RF_ADPLL_TEST_EN_POS))
#define RF_ADPLL_TEST_START                                     RF_ADPLL_TEST_START
#define RF_ADPLL_TEST_START_POS                                 (25U)
#define RF_ADPLL_TEST_START_LEN                                 (1U)
#define RF_ADPLL_TEST_START_MSK                                 (((1U<<RF_ADPLL_TEST_START_LEN)-1)<<RF_ADPLL_TEST_START_POS)
#define RF_ADPLL_TEST_START_UMSK                                (~(((1U<<RF_ADPLL_TEST_START_LEN)-1)<<RF_ADPLL_TEST_START_POS))

/* 0x134 : rf_ext_pa */
#define RF_EXT_PA_OFFSET                                        (0x134)
#define RF_EXT_PA_SB                                            RF_EXT_PA_SB
#define RF_EXT_PA_SB_POS                                        (0U)
#define RF_EXT_PA_SB_LEN                                        (5U)
#define RF_EXT_PA_SB_MSK                                        (((1U<<RF_EXT_PA_SB_LEN)-1)<<RF_EXT_PA_SB_POS)
#define RF_EXT_PA_SB_UMSK                                       (~(((1U<<RF_EXT_PA_SB_LEN)-1)<<RF_EXT_PA_SB_POS))
#define RF_EXT_PA_LOTX                                          RF_EXT_PA_LOTX
#define RF_EXT_PA_LOTX_POS                                      (5U)
#define RF_EXT_PA_LOTX_LEN                                      (5U)
#define RF_EXT_PA_LOTX_MSK                                      (((1U<<RF_EXT_PA_LOTX_LEN)-1)<<RF_EXT_PA_LOTX_POS)
#define RF_EXT_PA_LOTX_UMSK                                     (~(((1U<<RF_EXT_PA_LOTX_LEN)-1)<<RF_EXT_PA_LOTX_POS))
#define RF_EXT_PA_TX                                            RF_EXT_PA_TX
#define RF_EXT_PA_TX_POS                                        (10U)
#define RF_EXT_PA_TX_LEN                                        (5U)
#define RF_EXT_PA_TX_MSK                                        (((1U<<RF_EXT_PA_TX_LEN)-1)<<RF_EXT_PA_TX_POS)
#define RF_EXT_PA_TX_UMSK                                       (~(((1U<<RF_EXT_PA_TX_LEN)-1)<<RF_EXT_PA_TX_POS))
#define RF_EXT_PA_LORX                                          RF_EXT_PA_LORX
#define RF_EXT_PA_LORX_POS                                      (15U)
#define RF_EXT_PA_LORX_LEN                                      (5U)
#define RF_EXT_PA_LORX_MSK                                      (((1U<<RF_EXT_PA_LORX_LEN)-1)<<RF_EXT_PA_LORX_POS)
#define RF_EXT_PA_LORX_UMSK                                     (~(((1U<<RF_EXT_PA_LORX_LEN)-1)<<RF_EXT_PA_LORX_POS))
#define RF_EXT_PA_RX                                            RF_EXT_PA_RX
#define RF_EXT_PA_RX_POS                                        (20U)
#define RF_EXT_PA_RX_LEN                                        (5U)
#define RF_EXT_PA_RX_MSK                                        (((1U<<RF_EXT_PA_RX_LEN)-1)<<RF_EXT_PA_RX_POS)
#define RF_EXT_PA_RX_UMSK                                       (~(((1U<<RF_EXT_PA_RX_LEN)-1)<<RF_EXT_PA_RX_POS))

/* 0x200 : cip_ldo15 */
#define RF_CIP_LDO15_OFFSET                                     (0x200)
#define RF_VG11_SEL                                             RF_VG11_SEL
#define RF_VG11_SEL_POS                                         (0U)
#define RF_VG11_SEL_LEN                                         (2U)
#define RF_VG11_SEL_MSK                                         (((1U<<RF_VG11_SEL_LEN)-1)<<RF_VG11_SEL_POS)
#define RF_VG11_SEL_UMSK                                        (~(((1U<<RF_VG11_SEL_LEN)-1)<<RF_VG11_SEL_POS))

/* 0x204 : PA register */
#define RF_PA_OFFSET                                            (0x204)
#define RF_PA_SERI_CS_TX                                        RF_PA_SERI_CS_TX
#define RF_PA_SERI_CS_TX_POS                                    (0U)
#define RF_PA_SERI_CS_TX_LEN                                    (4U)
#define RF_PA_SERI_CS_TX_MSK                                    (((1U<<RF_PA_SERI_CS_TX_LEN)-1)<<RF_PA_SERI_CS_TX_POS)
#define RF_PA_SERI_CS_TX_UMSK                                   (~(((1U<<RF_PA_SERI_CS_TX_LEN)-1)<<RF_PA_SERI_CS_TX_POS))
#define RF_PA_SERI_CS_RX                                        RF_PA_SERI_CS_RX
#define RF_PA_SERI_CS_RX_POS                                    (4U)
#define RF_PA_SERI_CS_RX_LEN                                    (4U)
#define RF_PA_SERI_CS_RX_MSK                                    (((1U<<RF_PA_SERI_CS_RX_LEN)-1)<<RF_PA_SERI_CS_RX_POS)
#define RF_PA_SERI_CS_RX_UMSK                                   (~(((1U<<RF_PA_SERI_CS_RX_LEN)-1)<<RF_PA_SERI_CS_RX_POS))
#define RF_PA_SERI_CS_HW                                        RF_PA_SERI_CS_HW
#define RF_PA_SERI_CS_HW_POS                                    (8U)
#define RF_PA_SERI_CS_HW_LEN                                    (4U)
#define RF_PA_SERI_CS_HW_MSK                                    (((1U<<RF_PA_SERI_CS_HW_LEN)-1)<<RF_PA_SERI_CS_HW_POS)
#define RF_PA_SERI_CS_HW_UMSK                                   (~(((1U<<RF_PA_SERI_CS_HW_LEN)-1)<<RF_PA_SERI_CS_HW_POS))
#define RF_PA_PARA_CS                                           RF_PA_PARA_CS
#define RF_PA_PARA_CS_POS                                       (12U)
#define RF_PA_PARA_CS_LEN                                       (4U)
#define RF_PA_PARA_CS_MSK                                       (((1U<<RF_PA_PARA_CS_LEN)-1)<<RF_PA_PARA_CS_POS)
#define RF_PA_PARA_CS_UMSK                                      (~(((1U<<RF_PA_PARA_CS_LEN)-1)<<RF_PA_PARA_CS_POS))
#define RF_PA_VDD11_SEL                                         RF_PA_VDD11_SEL
#define RF_PA_VDD11_SEL_POS                                     (20U)
#define RF_PA_VDD11_SEL_LEN                                     (3U)
#define RF_PA_VDD11_SEL_MSK                                     (((1U<<RF_PA_VDD11_SEL_LEN)-1)<<RF_PA_VDD11_SEL_POS)
#define RF_PA_VDD11_SEL_UMSK                                    (~(((1U<<RF_PA_VDD11_SEL_LEN)-1)<<RF_PA_VDD11_SEL_POS))
#define RF_PA_LDO_BM                                            RF_PA_LDO_BM
#define RF_PA_LDO_BM_POS                                        (24U)
#define RF_PA_LDO_BM_LEN                                        (3U)
#define RF_PA_LDO_BM_MSK                                        (((1U<<RF_PA_LDO_BM_LEN)-1)<<RF_PA_LDO_BM_POS)
#define RF_PA_LDO_BM_UMSK                                       (~(((1U<<RF_PA_LDO_BM_LEN)-1)<<RF_PA_LDO_BM_POS))
#define RF_PA_LP_EN                                             RF_PA_LP_EN
#define RF_PA_LP_EN_POS                                         (28U)
#define RF_PA_LP_EN_LEN                                         (1U)
#define RF_PA_LP_EN_MSK                                         (((1U<<RF_PA_LP_EN_LEN)-1)<<RF_PA_LP_EN_POS)
#define RF_PA_LP_EN_UMSK                                        (~(((1U<<RF_PA_LP_EN_LEN)-1)<<RF_PA_LP_EN_POS))
#define RF_PA_HP_EN                                             RF_PA_HP_EN
#define RF_PA_HP_EN_POS                                         (29U)
#define RF_PA_HP_EN_LEN                                         (1U)
#define RF_PA_HP_EN_MSK                                         (((1U<<RF_PA_HP_EN_LEN)-1)<<RF_PA_HP_EN_POS)
#define RF_PA_HP_EN_UMSK                                        (~(((1U<<RF_PA_HP_EN_LEN)-1)<<RF_PA_HP_EN_POS))
#define RF_PA_FORCE_SHORT_OPEN                                  RF_PA_FORCE_SHORT_OPEN
#define RF_PA_FORCE_SHORT_OPEN_POS                              (30U)
#define RF_PA_FORCE_SHORT_OPEN_LEN                              (1U)
#define RF_PA_FORCE_SHORT_OPEN_MSK                              (((1U<<RF_PA_FORCE_SHORT_OPEN_LEN)-1)<<RF_PA_FORCE_SHORT_OPEN_POS)
#define RF_PA_FORCE_SHORT_OPEN_UMSK                             (~(((1U<<RF_PA_FORCE_SHORT_OPEN_LEN)-1)<<RF_PA_FORCE_SHORT_OPEN_POS))

/* 0x208 : LNA mixer register */
#define RF_LNA_MX_OFFSET                                        (0x208)
#define RF_RMX_BM                                               RF_RMX_BM
#define RF_RMX_BM_POS                                           (0U)
#define RF_RMX_BM_LEN                                           (3U)
#define RF_RMX_BM_MSK                                           (((1U<<RF_RMX_BM_LEN)-1)<<RF_RMX_BM_POS)
#define RF_RMX_BM_UMSK                                          (~(((1U<<RF_RMX_BM_LEN)-1)<<RF_RMX_BM_POS))
#define RF_LNA_RFB_MATCH                                        RF_LNA_RFB_MATCH
#define RF_LNA_RFB_MATCH_POS                                    (3U)
#define RF_LNA_RFB_MATCH_LEN                                    (3U)
#define RF_LNA_RFB_MATCH_MSK                                    (((1U<<RF_LNA_RFB_MATCH_LEN)-1)<<RF_LNA_RFB_MATCH_POS)
#define RF_LNA_RFB_MATCH_UMSK                                   (~(((1U<<RF_LNA_RFB_MATCH_LEN)-1)<<RF_LNA_RFB_MATCH_POS))
#define RF_LNA_VDD13_SEL                                        RF_LNA_VDD13_SEL
#define RF_LNA_VDD13_SEL_POS                                    (6U)
#define RF_LNA_VDD13_SEL_LEN                                    (2U)
#define RF_LNA_VDD13_SEL_MSK                                    (((1U<<RF_LNA_VDD13_SEL_LEN)-1)<<RF_LNA_VDD13_SEL_POS)
#define RF_LNA_VDD13_SEL_UMSK                                   (~(((1U<<RF_LNA_VDD13_SEL_LEN)-1)<<RF_LNA_VDD13_SEL_POS))
#define RF_LNA_LOAD_CSW                                         RF_LNA_LOAD_CSW
#define RF_LNA_LOAD_CSW_POS                                     (8U)
#define RF_LNA_LOAD_CSW_LEN                                     (4U)
#define RF_LNA_LOAD_CSW_MSK                                     (((1U<<RF_LNA_LOAD_CSW_LEN)-1)<<RF_LNA_LOAD_CSW_POS)
#define RF_LNA_LOAD_CSW_UMSK                                    (~(((1U<<RF_LNA_LOAD_CSW_LEN)-1)<<RF_LNA_LOAD_CSW_POS))
#define RF_LNA_LG_GSEL                                          RF_LNA_LG_GSEL
#define RF_LNA_LG_GSEL_POS                                      (12U)
#define RF_LNA_LG_GSEL_LEN                                      (3U)
#define RF_LNA_LG_GSEL_MSK                                      (((1U<<RF_LNA_LG_GSEL_LEN)-1)<<RF_LNA_LG_GSEL_POS)
#define RF_LNA_LG_GSEL_UMSK                                     (~(((1U<<RF_LNA_LG_GSEL_LEN)-1)<<RF_LNA_LG_GSEL_POS))
#define RF_LNA_CAP_MATCH                                        RF_LNA_CAP_MATCH
#define RF_LNA_CAP_MATCH_POS                                    (15U)
#define RF_LNA_CAP_MATCH_LEN                                    (3U)
#define RF_LNA_CAP_MATCH_MSK                                    (((1U<<RF_LNA_CAP_MATCH_LEN)-1)<<RF_LNA_CAP_MATCH_POS)
#define RF_LNA_CAP_MATCH_UMSK                                   (~(((1U<<RF_LNA_CAP_MATCH_LEN)-1)<<RF_LNA_CAP_MATCH_POS))
#define RF_LNA_CAP_LG                                           RF_LNA_CAP_LG
#define RF_LNA_CAP_LG_POS                                       (18U)
#define RF_LNA_CAP_LG_LEN                                       (2U)
#define RF_LNA_CAP_LG_MSK                                       (((1U<<RF_LNA_CAP_LG_LEN)-1)<<RF_LNA_CAP_LG_POS)
#define RF_LNA_CAP_LG_UMSK                                      (~(((1U<<RF_LNA_CAP_LG_LEN)-1)<<RF_LNA_CAP_LG_POS))
#define RF_LNA_BM_LG                                            RF_LNA_BM_LG
#define RF_LNA_BM_LG_POS                                        (20U)
#define RF_LNA_BM_LG_LEN                                        (4U)
#define RF_LNA_BM_LG_MSK                                        (((1U<<RF_LNA_BM_LG_LEN)-1)<<RF_LNA_BM_LG_POS)
#define RF_LNA_BM_LG_UMSK                                       (~(((1U<<RF_LNA_BM_LG_LEN)-1)<<RF_LNA_BM_LG_POS))
#define RF_LNA_BM_HG                                            RF_LNA_BM_HG
#define RF_LNA_BM_HG_POS                                        (24U)
#define RF_LNA_BM_HG_LEN                                        (4U)
#define RF_LNA_BM_HG_MSK                                        (((1U<<RF_LNA_BM_HG_LEN)-1)<<RF_LNA_BM_HG_POS)
#define RF_LNA_BM_HG_UMSK                                       (~(((1U<<RF_LNA_BM_HG_LEN)-1)<<RF_LNA_BM_HG_POS))
#define RF_LNA_BM_HW                                            RF_LNA_BM_HW
#define RF_LNA_BM_HW_POS                                        (28U)
#define RF_LNA_BM_HW_LEN                                        (4U)
#define RF_LNA_BM_HW_MSK                                        (((1U<<RF_LNA_BM_HW_LEN)-1)<<RF_LNA_BM_HW_POS)
#define RF_LNA_BM_HW_UMSK                                       (~(((1U<<RF_LNA_BM_HW_LEN)-1)<<RF_LNA_BM_HW_POS))

/* 0x20C : rbb_rosdac */
#define RF_RBB_ROSDAC_OFFSET                                    (0x20C)
#define RF_ROSDAC_Q_HW                                          RF_ROSDAC_Q_HW
#define RF_ROSDAC_Q_HW_POS                                      (0U)
#define RF_ROSDAC_Q_HW_LEN                                      (6U)
#define RF_ROSDAC_Q_HW_MSK                                      (((1U<<RF_ROSDAC_Q_HW_LEN)-1)<<RF_ROSDAC_Q_HW_POS)
#define RF_ROSDAC_Q_HW_UMSK                                     (~(((1U<<RF_ROSDAC_Q_HW_LEN)-1)<<RF_ROSDAC_Q_HW_POS))
#define RF_ROSDAC_I_HW                                          RF_ROSDAC_I_HW
#define RF_ROSDAC_I_HW_POS                                      (8U)
#define RF_ROSDAC_I_HW_LEN                                      (6U)
#define RF_ROSDAC_I_HW_MSK                                      (((1U<<RF_ROSDAC_I_HW_LEN)-1)<<RF_ROSDAC_I_HW_POS)
#define RF_ROSDAC_I_HW_UMSK                                     (~(((1U<<RF_ROSDAC_I_HW_LEN)-1)<<RF_ROSDAC_I_HW_POS))
#define RF_ROSDAC_Q                                             RF_ROSDAC_Q
#define RF_ROSDAC_Q_POS                                         (16U)
#define RF_ROSDAC_Q_LEN                                         (6U)
#define RF_ROSDAC_Q_MSK                                         (((1U<<RF_ROSDAC_Q_LEN)-1)<<RF_ROSDAC_Q_POS)
#define RF_ROSDAC_Q_UMSK                                        (~(((1U<<RF_ROSDAC_Q_LEN)-1)<<RF_ROSDAC_Q_POS))
#define RF_ROSDAC_I                                             RF_ROSDAC_I
#define RF_ROSDAC_I_POS                                         (24U)
#define RF_ROSDAC_I_LEN                                         (6U)
#define RF_ROSDAC_I_MSK                                         (((1U<<RF_ROSDAC_I_LEN)-1)<<RF_ROSDAC_I_POS)
#define RF_ROSDAC_I_UMSK                                        (~(((1U<<RF_ROSDAC_I_LEN)-1)<<RF_ROSDAC_I_POS))

/* 0x210 : rbb_cap_1 */
#define RF_RBB_CAP_1_OFFSET                                     (0x210)
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

/* 0x214 : rbb_cap_2 */
#define RF_RBB_CAP_2_OFFSET                                     (0x214)
#define RF_RBB_CAP2_FC_Q_HW                                     RF_RBB_CAP2_FC_Q_HW
#define RF_RBB_CAP2_FC_Q_HW_POS                                 (0U)
#define RF_RBB_CAP2_FC_Q_HW_LEN                                 (6U)
#define RF_RBB_CAP2_FC_Q_HW_MSK                                 (((1U<<RF_RBB_CAP2_FC_Q_HW_LEN)-1)<<RF_RBB_CAP2_FC_Q_HW_POS)
#define RF_RBB_CAP2_FC_Q_HW_UMSK                                (~(((1U<<RF_RBB_CAP2_FC_Q_HW_LEN)-1)<<RF_RBB_CAP2_FC_Q_HW_POS))
#define RF_RBB_CAP2_FC_I_HW                                     RF_RBB_CAP2_FC_I_HW
#define RF_RBB_CAP2_FC_I_HW_POS                                 (8U)
#define RF_RBB_CAP2_FC_I_HW_LEN                                 (6U)
#define RF_RBB_CAP2_FC_I_HW_MSK                                 (((1U<<RF_RBB_CAP2_FC_I_HW_LEN)-1)<<RF_RBB_CAP2_FC_I_HW_POS)
#define RF_RBB_CAP2_FC_I_HW_UMSK                                (~(((1U<<RF_RBB_CAP2_FC_I_HW_LEN)-1)<<RF_RBB_CAP2_FC_I_HW_POS))
#define RF_RBB_CAP1_FC_Q_HW                                     RF_RBB_CAP1_FC_Q_HW
#define RF_RBB_CAP1_FC_Q_HW_POS                                 (16U)
#define RF_RBB_CAP1_FC_Q_HW_LEN                                 (6U)
#define RF_RBB_CAP1_FC_Q_HW_MSK                                 (((1U<<RF_RBB_CAP1_FC_Q_HW_LEN)-1)<<RF_RBB_CAP1_FC_Q_HW_POS)
#define RF_RBB_CAP1_FC_Q_HW_UMSK                                (~(((1U<<RF_RBB_CAP1_FC_Q_HW_LEN)-1)<<RF_RBB_CAP1_FC_Q_HW_POS))
#define RF_RBB_CAP1_FC_I_HW                                     RF_RBB_CAP1_FC_I_HW
#define RF_RBB_CAP1_FC_I_HW_POS                                 (24U)
#define RF_RBB_CAP1_FC_I_HW_LEN                                 (6U)
#define RF_RBB_CAP1_FC_I_HW_MSK                                 (((1U<<RF_RBB_CAP1_FC_I_HW_LEN)-1)<<RF_RBB_CAP1_FC_I_HW_POS)
#define RF_RBB_CAP1_FC_I_HW_UMSK                                (~(((1U<<RF_RBB_CAP1_FC_I_HW_LEN)-1)<<RF_RBB_CAP1_FC_I_HW_POS))

/* 0x218 : rbb_cap_3 */
#define RF_RBB_CAP_3_OFFSET                                     (0x218)
#define RF_RBB_CAP2_FC_Q_BW0                                    RF_RBB_CAP2_FC_Q_BW0
#define RF_RBB_CAP2_FC_Q_BW0_POS                                (0U)
#define RF_RBB_CAP2_FC_Q_BW0_LEN                                (6U)
#define RF_RBB_CAP2_FC_Q_BW0_MSK                                (((1U<<RF_RBB_CAP2_FC_Q_BW0_LEN)-1)<<RF_RBB_CAP2_FC_Q_BW0_POS)
#define RF_RBB_CAP2_FC_Q_BW0_UMSK                               (~(((1U<<RF_RBB_CAP2_FC_Q_BW0_LEN)-1)<<RF_RBB_CAP2_FC_Q_BW0_POS))
#define RF_RBB_CAP2_FC_I_BW0                                    RF_RBB_CAP2_FC_I_BW0
#define RF_RBB_CAP2_FC_I_BW0_POS                                (8U)
#define RF_RBB_CAP2_FC_I_BW0_LEN                                (6U)
#define RF_RBB_CAP2_FC_I_BW0_MSK                                (((1U<<RF_RBB_CAP2_FC_I_BW0_LEN)-1)<<RF_RBB_CAP2_FC_I_BW0_POS)
#define RF_RBB_CAP2_FC_I_BW0_UMSK                               (~(((1U<<RF_RBB_CAP2_FC_I_BW0_LEN)-1)<<RF_RBB_CAP2_FC_I_BW0_POS))
#define RF_RBB_CAP1_FC_Q_BW0                                    RF_RBB_CAP1_FC_Q_BW0
#define RF_RBB_CAP1_FC_Q_BW0_POS                                (16U)
#define RF_RBB_CAP1_FC_Q_BW0_LEN                                (6U)
#define RF_RBB_CAP1_FC_Q_BW0_MSK                                (((1U<<RF_RBB_CAP1_FC_Q_BW0_LEN)-1)<<RF_RBB_CAP1_FC_Q_BW0_POS)
#define RF_RBB_CAP1_FC_Q_BW0_UMSK                               (~(((1U<<RF_RBB_CAP1_FC_Q_BW0_LEN)-1)<<RF_RBB_CAP1_FC_Q_BW0_POS))
#define RF_RBB_CAP1_FC_I_BW0                                    RF_RBB_CAP1_FC_I_BW0
#define RF_RBB_CAP1_FC_I_BW0_POS                                (24U)
#define RF_RBB_CAP1_FC_I_BW0_LEN                                (6U)
#define RF_RBB_CAP1_FC_I_BW0_MSK                                (((1U<<RF_RBB_CAP1_FC_I_BW0_LEN)-1)<<RF_RBB_CAP1_FC_I_BW0_POS)
#define RF_RBB_CAP1_FC_I_BW0_UMSK                               (~(((1U<<RF_RBB_CAP1_FC_I_BW0_LEN)-1)<<RF_RBB_CAP1_FC_I_BW0_POS))

/* 0x21C : rbb_cap4 */
#define RF_RBB_CAP4_OFFSET                                      (0x21C)
#define RF_RBB_CAP2_FC_Q_BW1                                    RF_RBB_CAP2_FC_Q_BW1
#define RF_RBB_CAP2_FC_Q_BW1_POS                                (0U)
#define RF_RBB_CAP2_FC_Q_BW1_LEN                                (6U)
#define RF_RBB_CAP2_FC_Q_BW1_MSK                                (((1U<<RF_RBB_CAP2_FC_Q_BW1_LEN)-1)<<RF_RBB_CAP2_FC_Q_BW1_POS)
#define RF_RBB_CAP2_FC_Q_BW1_UMSK                               (~(((1U<<RF_RBB_CAP2_FC_Q_BW1_LEN)-1)<<RF_RBB_CAP2_FC_Q_BW1_POS))
#define RF_RBB_CAP2_FC_I_BW1                                    RF_RBB_CAP2_FC_I_BW1
#define RF_RBB_CAP2_FC_I_BW1_POS                                (8U)
#define RF_RBB_CAP2_FC_I_BW1_LEN                                (6U)
#define RF_RBB_CAP2_FC_I_BW1_MSK                                (((1U<<RF_RBB_CAP2_FC_I_BW1_LEN)-1)<<RF_RBB_CAP2_FC_I_BW1_POS)
#define RF_RBB_CAP2_FC_I_BW1_UMSK                               (~(((1U<<RF_RBB_CAP2_FC_I_BW1_LEN)-1)<<RF_RBB_CAP2_FC_I_BW1_POS))
#define RF_RBB_CAP1_FC_Q_BW1                                    RF_RBB_CAP1_FC_Q_BW1
#define RF_RBB_CAP1_FC_Q_BW1_POS                                (16U)
#define RF_RBB_CAP1_FC_Q_BW1_LEN                                (6U)
#define RF_RBB_CAP1_FC_Q_BW1_MSK                                (((1U<<RF_RBB_CAP1_FC_Q_BW1_LEN)-1)<<RF_RBB_CAP1_FC_Q_BW1_POS)
#define RF_RBB_CAP1_FC_Q_BW1_UMSK                               (~(((1U<<RF_RBB_CAP1_FC_Q_BW1_LEN)-1)<<RF_RBB_CAP1_FC_Q_BW1_POS))
#define RF_RBB_CAP1_FC_I_BW1                                    RF_RBB_CAP1_FC_I_BW1
#define RF_RBB_CAP1_FC_I_BW1_POS                                (24U)
#define RF_RBB_CAP1_FC_I_BW1_LEN                                (6U)
#define RF_RBB_CAP1_FC_I_BW1_MSK                                (((1U<<RF_RBB_CAP1_FC_I_BW1_LEN)-1)<<RF_RBB_CAP1_FC_I_BW1_POS)
#define RF_RBB_CAP1_FC_I_BW1_UMSK                               (~(((1U<<RF_RBB_CAP1_FC_I_BW1_LEN)-1)<<RF_RBB_CAP1_FC_I_BW1_POS))

/* 0x220 : rbb_rx */
#define RF_RBB_RX_OFFSET                                        (0x220)
#define RF_RBB_RX2_BW1                                          RF_RBB_RX2_BW1
#define RF_RBB_RX2_BW1_POS                                      (0U)
#define RF_RBB_RX2_BW1_LEN                                      (3U)
#define RF_RBB_RX2_BW1_MSK                                      (((1U<<RF_RBB_RX2_BW1_LEN)-1)<<RF_RBB_RX2_BW1_POS)
#define RF_RBB_RX2_BW1_UMSK                                     (~(((1U<<RF_RBB_RX2_BW1_LEN)-1)<<RF_RBB_RX2_BW1_POS))
#define RF_RBB_RX1_BW1                                          RF_RBB_RX1_BW1
#define RF_RBB_RX1_BW1_POS                                      (4U)
#define RF_RBB_RX1_BW1_LEN                                      (3U)
#define RF_RBB_RX1_BW1_MSK                                      (((1U<<RF_RBB_RX1_BW1_LEN)-1)<<RF_RBB_RX1_BW1_POS)
#define RF_RBB_RX1_BW1_UMSK                                     (~(((1U<<RF_RBB_RX1_BW1_LEN)-1)<<RF_RBB_RX1_BW1_POS))
#define RF_RBB_RX2_BW0                                          RF_RBB_RX2_BW0
#define RF_RBB_RX2_BW0_POS                                      (8U)
#define RF_RBB_RX2_BW0_LEN                                      (3U)
#define RF_RBB_RX2_BW0_MSK                                      (((1U<<RF_RBB_RX2_BW0_LEN)-1)<<RF_RBB_RX2_BW0_POS)
#define RF_RBB_RX2_BW0_UMSK                                     (~(((1U<<RF_RBB_RX2_BW0_LEN)-1)<<RF_RBB_RX2_BW0_POS))
#define RF_RBB_RX1_BW0                                          RF_RBB_RX1_BW0
#define RF_RBB_RX1_BW0_POS                                      (12U)
#define RF_RBB_RX1_BW0_LEN                                      (3U)
#define RF_RBB_RX1_BW0_MSK                                      (((1U<<RF_RBB_RX1_BW0_LEN)-1)<<RF_RBB_RX1_BW0_POS)
#define RF_RBB_RX1_BW0_UMSK                                     (~(((1U<<RF_RBB_RX1_BW0_LEN)-1)<<RF_RBB_RX1_BW0_POS))
#define RF_RBB_RX2_HW                                           RF_RBB_RX2_HW
#define RF_RBB_RX2_HW_POS                                       (16U)
#define RF_RBB_RX2_HW_LEN                                       (3U)
#define RF_RBB_RX2_HW_MSK                                       (((1U<<RF_RBB_RX2_HW_LEN)-1)<<RF_RBB_RX2_HW_POS)
#define RF_RBB_RX2_HW_UMSK                                      (~(((1U<<RF_RBB_RX2_HW_LEN)-1)<<RF_RBB_RX2_HW_POS))
#define RF_RBB_RX1_HW                                           RF_RBB_RX1_HW
#define RF_RBB_RX1_HW_POS                                       (20U)
#define RF_RBB_RX1_HW_LEN                                       (3U)
#define RF_RBB_RX1_HW_MSK                                       (((1U<<RF_RBB_RX1_HW_LEN)-1)<<RF_RBB_RX1_HW_POS)
#define RF_RBB_RX1_HW_UMSK                                      (~(((1U<<RF_RBB_RX1_HW_LEN)-1)<<RF_RBB_RX1_HW_POS))
#define RF_RBB_RX2                                              RF_RBB_RX2
#define RF_RBB_RX2_POS                                          (24U)
#define RF_RBB_RX2_LEN                                          (3U)
#define RF_RBB_RX2_MSK                                          (((1U<<RF_RBB_RX2_LEN)-1)<<RF_RBB_RX2_POS)
#define RF_RBB_RX2_UMSK                                         (~(((1U<<RF_RBB_RX2_LEN)-1)<<RF_RBB_RX2_POS))
#define RF_RBB_RX1                                              RF_RBB_RX1
#define RF_RBB_RX1_POS                                          (28U)
#define RF_RBB_RX1_LEN                                          (3U)
#define RF_RBB_RX1_MSK                                          (((1U<<RF_RBB_RX1_LEN)-1)<<RF_RBB_RX1_POS)
#define RF_RBB_RX1_UMSK                                         (~(((1U<<RF_RBB_RX1_LEN)-1)<<RF_RBB_RX1_POS))

/* 0x224 : rbb */
#define RF_RBB_OFFSET                                           (0x224)
#define RF_PKDET_OUT_LATCH                                      RF_PKDET_OUT_LATCH
#define RF_PKDET_OUT_LATCH_POS                                  (0U)
#define RF_PKDET_OUT_LATCH_LEN                                  (1U)
#define RF_PKDET_OUT_LATCH_MSK                                  (((1U<<RF_PKDET_OUT_LATCH_LEN)-1)<<RF_PKDET_OUT_LATCH_POS)
#define RF_PKDET_OUT_LATCH_UMSK                                 (~(((1U<<RF_PKDET_OUT_LATCH_LEN)-1)<<RF_PKDET_OUT_LATCH_POS))
#define RF_PKDET_OUT_RAW                                        RF_PKDET_OUT_RAW
#define RF_PKDET_OUT_RAW_POS                                    (1U)
#define RF_PKDET_OUT_RAW_LEN                                    (1U)
#define RF_PKDET_OUT_RAW_MSK                                    (((1U<<RF_PKDET_OUT_RAW_LEN)-1)<<RF_PKDET_OUT_RAW_POS)
#define RF_PKDET_OUT_RAW_UMSK                                   (~(((1U<<RF_PKDET_OUT_RAW_LEN)-1)<<RF_PKDET_OUT_RAW_POS))
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW                           RF_RBB_PKDET_OUT_RSTN_CTRL_HW
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS                       (4U)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN                       (1U)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_MSK                       (((1U<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS)
#define RF_RBB_PKDET_OUT_RSTN_CTRL_HW_UMSK                      (~(((1U<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_CTRL_HW_POS))
#define RF_RBB_PKDET_OUT_RSTN_HW                                RF_RBB_PKDET_OUT_RSTN_HW
#define RF_RBB_PKDET_OUT_RSTN_HW_POS                            (5U)
#define RF_RBB_PKDET_OUT_RSTN_HW_LEN                            (1U)
#define RF_RBB_PKDET_OUT_RSTN_HW_MSK                            (((1U<<RF_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_HW_POS)
#define RF_RBB_PKDET_OUT_RSTN_HW_UMSK                           (~(((1U<<RF_RBB_PKDET_OUT_RSTN_HW_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_HW_POS))
#define RF_RBB_PKDET_OUT_RSTN                                   RF_RBB_PKDET_OUT_RSTN
#define RF_RBB_PKDET_OUT_RSTN_POS                               (6U)
#define RF_RBB_PKDET_OUT_RSTN_LEN                               (1U)
#define RF_RBB_PKDET_OUT_RSTN_MSK                               (((1U<<RF_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_POS)
#define RF_RBB_PKDET_OUT_RSTN_UMSK                              (~(((1U<<RF_RBB_PKDET_OUT_RSTN_LEN)-1)<<RF_RBB_PKDET_OUT_RSTN_POS))
#define RF_RBB_PKDET_EN_CTRL_HW                                 RF_RBB_PKDET_EN_CTRL_HW
#define RF_RBB_PKDET_EN_CTRL_HW_POS                             (8U)
#define RF_RBB_PKDET_EN_CTRL_HW_LEN                             (1U)
#define RF_RBB_PKDET_EN_CTRL_HW_MSK                             (((1U<<RF_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_EN_CTRL_HW_POS)
#define RF_RBB_PKDET_EN_CTRL_HW_UMSK                            (~(((1U<<RF_RBB_PKDET_EN_CTRL_HW_LEN)-1)<<RF_RBB_PKDET_EN_CTRL_HW_POS))
#define RF_RBB_PKDET_EN_HW                                      RF_RBB_PKDET_EN_HW
#define RF_RBB_PKDET_EN_HW_POS                                  (9U)
#define RF_RBB_PKDET_EN_HW_LEN                                  (1U)
#define RF_RBB_PKDET_EN_HW_MSK                                  (((1U<<RF_RBB_PKDET_EN_HW_LEN)-1)<<RF_RBB_PKDET_EN_HW_POS)
#define RF_RBB_PKDET_EN_HW_UMSK                                 (~(((1U<<RF_RBB_PKDET_EN_HW_LEN)-1)<<RF_RBB_PKDET_EN_HW_POS))
#define RF_RBB_PKDET_EN                                         RF_RBB_PKDET_EN
#define RF_RBB_PKDET_EN_POS                                     (10U)
#define RF_RBB_PKDET_EN_LEN                                     (1U)
#define RF_RBB_PKDET_EN_MSK                                     (((1U<<RF_RBB_PKDET_EN_LEN)-1)<<RF_RBB_PKDET_EN_POS)
#define RF_RBB_PKDET_EN_UMSK                                    (~(((1U<<RF_RBB_PKDET_EN_LEN)-1)<<RF_RBB_PKDET_EN_POS))
#define RF_RBB_PKDET_VTH                                        RF_RBB_PKDET_VTH
#define RF_RBB_PKDET_VTH_POS                                    (12U)
#define RF_RBB_PKDET_VTH_LEN                                    (4U)
#define RF_RBB_PKDET_VTH_MSK                                    (((1U<<RF_RBB_PKDET_VTH_LEN)-1)<<RF_RBB_PKDET_VTH_POS)
#define RF_RBB_PKDET_VTH_UMSK                                   (~(((1U<<RF_RBB_PKDET_VTH_LEN)-1)<<RF_RBB_PKDET_VTH_POS))
#define RF_ROSDAC_RANGE                                         RF_ROSDAC_RANGE
#define RF_ROSDAC_RANGE_POS                                     (16U)
#define RF_ROSDAC_RANGE_LEN                                     (2U)
#define RF_ROSDAC_RANGE_MSK                                     (((1U<<RF_ROSDAC_RANGE_LEN)-1)<<RF_ROSDAC_RANGE_POS)
#define RF_ROSDAC_RANGE_UMSK                                    (~(((1U<<RF_ROSDAC_RANGE_LEN)-1)<<RF_ROSDAC_RANGE_POS))
#define RF_RBB_LPF_EN                                           RF_RBB_LPF_EN
#define RF_RBB_LPF_EN_POS                                       (19U)
#define RF_RBB_LPF_EN_LEN                                       (1U)
#define RF_RBB_LPF_EN_MSK                                       (((1U<<RF_RBB_LPF_EN_LEN)-1)<<RF_RBB_LPF_EN_POS)
#define RF_RBB_LPF_EN_UMSK                                      (~(((1U<<RF_RBB_LPF_EN_LEN)-1)<<RF_RBB_LPF_EN_POS))
#define RF_RBB_DEQ                                              RF_RBB_DEQ
#define RF_RBB_DEQ_POS                                          (20U)
#define RF_RBB_DEQ_LEN                                          (2U)
#define RF_RBB_DEQ_MSK                                          (((1U<<RF_RBB_DEQ_LEN)-1)<<RF_RBB_DEQ_POS)
#define RF_RBB_DEQ_UMSK                                         (~(((1U<<RF_RBB_DEQ_LEN)-1)<<RF_RBB_DEQ_POS))
#define RF_RBB_VCM                                              RF_RBB_VCM
#define RF_RBB_VCM_POS                                          (24U)
#define RF_RBB_VCM_LEN                                          (2U)
#define RF_RBB_VCM_MSK                                          (((1U<<RF_RBB_VCM_LEN)-1)<<RF_RBB_VCM_POS)
#define RF_RBB_VCM_UMSK                                         (~(((1U<<RF_RBB_VCM_LEN)-1)<<RF_RBB_VCM_POS))
#define RF_RBB_BM_OP                                            RF_RBB_BM_OP
#define RF_RBB_BM_OP_POS                                        (28U)
#define RF_RBB_BM_OP_LEN                                        (2U)
#define RF_RBB_BM_OP_MSK                                        (((1U<<RF_RBB_BM_OP_LEN)-1)<<RF_RBB_BM_OP_POS)
#define RF_RBB_BM_OP_UMSK                                       (~(((1U<<RF_RBB_BM_OP_LEN)-1)<<RF_RBB_BM_OP_POS))

/* 0x228 : rxadc */
#define RF_RXADC_OFFSET                                         (0x228)
#define RF_RXADC_OSCAL_EN                                       RF_RXADC_OSCAL_EN
#define RF_RXADC_OSCAL_EN_POS                                   (0U)
#define RF_RXADC_OSCAL_EN_LEN                                   (1U)
#define RF_RXADC_OSCAL_EN_MSK                                   (((1U<<RF_RXADC_OSCAL_EN_LEN)-1)<<RF_RXADC_OSCAL_EN_POS)
#define RF_RXADC_OSCAL_EN_UMSK                                  (~(((1U<<RF_RXADC_OSCAL_EN_LEN)-1)<<RF_RXADC_OSCAL_EN_POS))
#define RF_RXADC_VREF_SEL                                       RF_RXADC_VREF_SEL
#define RF_RXADC_VREF_SEL_POS                                   (4U)
#define RF_RXADC_VREF_SEL_LEN                                   (2U)
#define RF_RXADC_VREF_SEL_MSK                                   (((1U<<RF_RXADC_VREF_SEL_LEN)-1)<<RF_RXADC_VREF_SEL_POS)
#define RF_RXADC_VREF_SEL_UMSK                                  (~(((1U<<RF_RXADC_VREF_SEL_LEN)-1)<<RF_RXADC_VREF_SEL_POS))
#define RF_RXADC_CLK_SYNC_INV                                   RF_RXADC_CLK_SYNC_INV
#define RF_RXADC_CLK_SYNC_INV_POS                               (8U)
#define RF_RXADC_CLK_SYNC_INV_LEN                               (1U)
#define RF_RXADC_CLK_SYNC_INV_MSK                               (((1U<<RF_RXADC_CLK_SYNC_INV_LEN)-1)<<RF_RXADC_CLK_SYNC_INV_POS)
#define RF_RXADC_CLK_SYNC_INV_UMSK                              (~(((1U<<RF_RXADC_CLK_SYNC_INV_LEN)-1)<<RF_RXADC_CLK_SYNC_INV_POS))
#define RF_RXADC_CLK_INV                                        RF_RXADC_CLK_INV
#define RF_RXADC_CLK_INV_POS                                    (12U)
#define RF_RXADC_CLK_INV_LEN                                    (1U)
#define RF_RXADC_CLK_INV_MSK                                    (((1U<<RF_RXADC_CLK_INV_LEN)-1)<<RF_RXADC_CLK_INV_POS)
#define RF_RXADC_CLK_INV_UMSK                                   (~(((1U<<RF_RXADC_CLK_INV_LEN)-1)<<RF_RXADC_CLK_INV_POS))
#define RF_RXADC_CLK_DIV_SEL                                    RF_RXADC_CLK_DIV_SEL
#define RF_RXADC_CLK_DIV_SEL_POS                                (16U)
#define RF_RXADC_CLK_DIV_SEL_LEN                                (1U)
#define RF_RXADC_CLK_DIV_SEL_MSK                                (((1U<<RF_RXADC_CLK_DIV_SEL_LEN)-1)<<RF_RXADC_CLK_DIV_SEL_POS)
#define RF_RXADC_CLK_DIV_SEL_UMSK                               (~(((1U<<RF_RXADC_CLK_DIV_SEL_LEN)-1)<<RF_RXADC_CLK_DIV_SEL_POS))
#define RF_RXADC_GLITCH_REMOVE                                  RF_RXADC_GLITCH_REMOVE
#define RF_RXADC_GLITCH_REMOVE_POS                              (20U)
#define RF_RXADC_GLITCH_REMOVE_LEN                              (1U)
#define RF_RXADC_GLITCH_REMOVE_MSK                              (((1U<<RF_RXADC_GLITCH_REMOVE_LEN)-1)<<RF_RXADC_GLITCH_REMOVE_POS)
#define RF_RXADC_GLITCH_REMOVE_UMSK                             (~(((1U<<RF_RXADC_GLITCH_REMOVE_LEN)-1)<<RF_RXADC_GLITCH_REMOVE_POS))
#define RF_RXADC_DLY_CTRL                                       RF_RXADC_DLY_CTRL
#define RF_RXADC_DLY_CTRL_POS                                   (24U)
#define RF_RXADC_DLY_CTRL_LEN                                   (2U)
#define RF_RXADC_DLY_CTRL_MSK                                   (((1U<<RF_RXADC_DLY_CTRL_LEN)-1)<<RF_RXADC_DLY_CTRL_POS)
#define RF_RXADC_DLY_CTRL_UMSK                                  (~(((1U<<RF_RXADC_DLY_CTRL_LEN)-1)<<RF_RXADC_DLY_CTRL_POS))

/* 0x22C : rxadc_readback */
#define RF_RXADC_READBACK_OFFSET                                (0x22C)
#define RF_RXADC_DOUT_Q                                         RF_RXADC_DOUT_Q
#define RF_RXADC_DOUT_Q_POS                                     (0U)
#define RF_RXADC_DOUT_Q_LEN                                     (9U)
#define RF_RXADC_DOUT_Q_MSK                                     (((1U<<RF_RXADC_DOUT_Q_LEN)-1)<<RF_RXADC_DOUT_Q_POS)
#define RF_RXADC_DOUT_Q_UMSK                                    (~(((1U<<RF_RXADC_DOUT_Q_LEN)-1)<<RF_RXADC_DOUT_Q_POS))
#define RF_RXADC_DOUT_I                                         RF_RXADC_DOUT_I
#define RF_RXADC_DOUT_I_POS                                     (16U)
#define RF_RXADC_DOUT_I_LEN                                     (9U)
#define RF_RXADC_DOUT_I_MSK                                     (((1U<<RF_RXADC_DOUT_I_LEN)-1)<<RF_RXADC_DOUT_I_POS)
#define RF_RXADC_DOUT_I_UMSK                                    (~(((1U<<RF_RXADC_DOUT_I_LEN)-1)<<RF_RXADC_DOUT_I_POS))

/* 0x230 : rf_adc_osdata */
#define RF_ADC_OSDATA_OFFSET                                    (0x230)
#define RF_RXADC_OS_Q                                           RF_RXADC_OS_Q
#define RF_RXADC_OS_Q_POS                                       (0U)
#define RF_RXADC_OS_Q_LEN                                       (9U)
#define RF_RXADC_OS_Q_MSK                                       (((1U<<RF_RXADC_OS_Q_LEN)-1)<<RF_RXADC_OS_Q_POS)
#define RF_RXADC_OS_Q_UMSK                                      (~(((1U<<RF_RXADC_OS_Q_LEN)-1)<<RF_RXADC_OS_Q_POS))
#define RF_RXADC_OS_I                                           RF_RXADC_OS_I
#define RF_RXADC_OS_I_POS                                       (16U)
#define RF_RXADC_OS_I_LEN                                       (9U)
#define RF_RXADC_OS_I_MSK                                       (((1U<<RF_RXADC_OS_I_LEN)-1)<<RF_RXADC_OS_I_POS)
#define RF_RXADC_OS_I_UMSK                                      (~(((1U<<RF_RXADC_OS_I_LEN)-1)<<RF_RXADC_OS_I_POS))

/* 0x234 : testbuf */
#define RF_TESTBUF_OFFSET                                       (0x234)
#define RF_TESTBUF_RIN                                          RF_TESTBUF_RIN
#define RF_TESTBUF_RIN_POS                                      (0U)
#define RF_TESTBUF_RIN_LEN                                      (1U)
#define RF_TESTBUF_RIN_MSK                                      (((1U<<RF_TESTBUF_RIN_LEN)-1)<<RF_TESTBUF_RIN_POS)
#define RF_TESTBUF_RIN_UMSK                                     (~(((1U<<RF_TESTBUF_RIN_LEN)-1)<<RF_TESTBUF_RIN_POS))
#define RF_TESTBUF_RFB                                          RF_TESTBUF_RFB
#define RF_TESTBUF_RFB_POS                                      (4U)
#define RF_TESTBUF_RFB_LEN                                      (1U)
#define RF_TESTBUF_RFB_MSK                                      (((1U<<RF_TESTBUF_RFB_LEN)-1)<<RF_TESTBUF_RFB_POS)
#define RF_TESTBUF_RFB_UMSK                                     (~(((1U<<RF_TESTBUF_RFB_LEN)-1)<<RF_TESTBUF_RFB_POS))
#define RF_TESTBUF_OP_CC                                        RF_TESTBUF_OP_CC
#define RF_TESTBUF_OP_CC_POS                                    (8U)
#define RF_TESTBUF_OP_CC_LEN                                    (4U)
#define RF_TESTBUF_OP_CC_MSK                                    (((1U<<RF_TESTBUF_OP_CC_LEN)-1)<<RF_TESTBUF_OP_CC_POS)
#define RF_TESTBUF_OP_CC_UMSK                                   (~(((1U<<RF_TESTBUF_OP_CC_LEN)-1)<<RF_TESTBUF_OP_CC_POS))
#define RF_TESTBUF_BOOST                                        RF_TESTBUF_BOOST
#define RF_TESTBUF_BOOST_POS                                    (12U)
#define RF_TESTBUF_BOOST_LEN                                    (1U)
#define RF_TESTBUF_BOOST_MSK                                    (((1U<<RF_TESTBUF_BOOST_LEN)-1)<<RF_TESTBUF_BOOST_POS)
#define RF_TESTBUF_BOOST_UMSK                                   (~(((1U<<RF_TESTBUF_BOOST_LEN)-1)<<RF_TESTBUF_BOOST_POS))
#define RF_TESTBUF_BM                                           RF_TESTBUF_BM
#define RF_TESTBUF_BM_POS                                       (16U)
#define RF_TESTBUF_BM_LEN                                       (3U)
#define RF_TESTBUF_BM_MSK                                       (((1U<<RF_TESTBUF_BM_LEN)-1)<<RF_TESTBUF_BM_POS)
#define RF_TESTBUF_BM_UMSK                                      (~(((1U<<RF_TESTBUF_BM_LEN)-1)<<RF_TESTBUF_BM_POS))
#define RF_TESTBUF_VCM                                          RF_TESTBUF_VCM
#define RF_TESTBUF_VCM_POS                                      (20U)
#define RF_TESTBUF_VCM_LEN                                      (2U)
#define RF_TESTBUF_VCM_MSK                                      (((1U<<RF_TESTBUF_VCM_LEN)-1)<<RF_TESTBUF_VCM_POS)
#define RF_TESTBUF_VCM_UMSK                                     (~(((1U<<RF_TESTBUF_VCM_LEN)-1)<<RF_TESTBUF_VCM_POS))
#define RF_PU_TESTBUF                                           RF_PU_TESTBUF
#define RF_PU_TESTBUF_POS                                       (24U)
#define RF_PU_TESTBUF_LEN                                       (1U)
#define RF_PU_TESTBUF_MSK                                       (((1U<<RF_PU_TESTBUF_LEN)-1)<<RF_PU_TESTBUF_POS)
#define RF_PU_TESTBUF_UMSK                                      (~(((1U<<RF_PU_TESTBUF_LEN)-1)<<RF_PU_TESTBUF_POS))

/* 0x238 : vco */
#define RF_VCO_OFFSET                                           (0x238)
#define RF_VCO_SHORT_VBIAS_FILTER                               RF_VCO_SHORT_VBIAS_FILTER
#define RF_VCO_SHORT_VBIAS_FILTER_POS                           (0U)
#define RF_VCO_SHORT_VBIAS_FILTER_LEN                           (1U)
#define RF_VCO_SHORT_VBIAS_FILTER_MSK                           (((1U<<RF_VCO_SHORT_VBIAS_FILTER_LEN)-1)<<RF_VCO_SHORT_VBIAS_FILTER_POS)
#define RF_VCO_SHORT_VBIAS_FILTER_UMSK                          (~(((1U<<RF_VCO_SHORT_VBIAS_FILTER_LEN)-1)<<RF_VCO_SHORT_VBIAS_FILTER_POS))
#define RF_VCO_SHORT_IDAC_FILTER                                RF_VCO_SHORT_IDAC_FILTER
#define RF_VCO_SHORT_IDAC_FILTER_POS                            (1U)
#define RF_VCO_SHORT_IDAC_FILTER_LEN                            (1U)
#define RF_VCO_SHORT_IDAC_FILTER_MSK                            (((1U<<RF_VCO_SHORT_IDAC_FILTER_LEN)-1)<<RF_VCO_SHORT_IDAC_FILTER_POS)
#define RF_VCO_SHORT_IDAC_FILTER_UMSK                           (~(((1U<<RF_VCO_SHORT_IDAC_FILTER_LEN)-1)<<RF_VCO_SHORT_IDAC_FILTER_POS))
#define RF_VCO_MODCAP_SEL                                       RF_VCO_MODCAP_SEL
#define RF_VCO_MODCAP_SEL_POS                                   (2U)
#define RF_VCO_MODCAP_SEL_LEN                                   (2U)
#define RF_VCO_MODCAP_SEL_MSK                                   (((1U<<RF_VCO_MODCAP_SEL_LEN)-1)<<RF_VCO_MODCAP_SEL_POS)
#define RF_VCO_MODCAP_SEL_UMSK                                  (~(((1U<<RF_VCO_MODCAP_SEL_LEN)-1)<<RF_VCO_MODCAP_SEL_POS))
#define RF_VCO_ACAL_VREF                                        RF_VCO_ACAL_VREF
#define RF_VCO_ACAL_VREF_POS                                    (4U)
#define RF_VCO_ACAL_VREF_LEN                                    (3U)
#define RF_VCO_ACAL_VREF_MSK                                    (((1U<<RF_VCO_ACAL_VREF_LEN)-1)<<RF_VCO_ACAL_VREF_POS)
#define RF_VCO_ACAL_VREF_UMSK                                   (~(((1U<<RF_VCO_ACAL_VREF_LEN)-1)<<RF_VCO_ACAL_VREF_POS))
#define RF_VCO_VBIAS                                            RF_VCO_VBIAS
#define RF_VCO_VBIAS_POS                                        (8U)
#define RF_VCO_VBIAS_LEN                                        (2U)
#define RF_VCO_VBIAS_MSK                                        (((1U<<RF_VCO_VBIAS_LEN)-1)<<RF_VCO_VBIAS_POS)
#define RF_VCO_VBIAS_UMSK                                       (~(((1U<<RF_VCO_VBIAS_LEN)-1)<<RF_VCO_VBIAS_POS))
#define RF_VCO_IDAC_BOOST                                       RF_VCO_IDAC_BOOST
#define RF_VCO_IDAC_BOOST_POS                                   (12U)
#define RF_VCO_IDAC_BOOST_LEN                                   (1U)
#define RF_VCO_IDAC_BOOST_MSK                                   (((1U<<RF_VCO_IDAC_BOOST_LEN)-1)<<RF_VCO_IDAC_BOOST_POS)
#define RF_VCO_IDAC_BOOST_UMSK                                  (~(((1U<<RF_VCO_IDAC_BOOST_LEN)-1)<<RF_VCO_IDAC_BOOST_POS))
#define RF_VCO_LDO_VSEL                                         RF_VCO_LDO_VSEL
#define RF_VCO_LDO_VSEL_POS                                     (13U)
#define RF_VCO_LDO_VSEL_LEN                                     (2U)
#define RF_VCO_LDO_VSEL_MSK                                     (((1U<<RF_VCO_LDO_VSEL_LEN)-1)<<RF_VCO_LDO_VSEL_POS)
#define RF_VCO_LDO_VSEL_UMSK                                    (~(((1U<<RF_VCO_LDO_VSEL_LEN)-1)<<RF_VCO_LDO_VSEL_POS))
#define RF_VCO_LDO_BYPASS                                       RF_VCO_LDO_BYPASS
#define RF_VCO_LDO_BYPASS_POS                                   (15U)
#define RF_VCO_LDO_BYPASS_LEN                                   (1U)
#define RF_VCO_LDO_BYPASS_MSK                                   (((1U<<RF_VCO_LDO_BYPASS_LEN)-1)<<RF_VCO_LDO_BYPASS_POS)
#define RF_VCO_LDO_BYPASS_UMSK                                  (~(((1U<<RF_VCO_LDO_BYPASS_LEN)-1)<<RF_VCO_LDO_BYPASS_POS))
#define RF_VCO_IDAC                                             RF_VCO_IDAC
#define RF_VCO_IDAC_POS                                         (16U)
#define RF_VCO_IDAC_LEN                                         (6U)
#define RF_VCO_IDAC_MSK                                         (((1U<<RF_VCO_IDAC_LEN)-1)<<RF_VCO_IDAC_POS)
#define RF_VCO_IDAC_UMSK                                        (~(((1U<<RF_VCO_IDAC_LEN)-1)<<RF_VCO_IDAC_POS))
#define RF_VCO_IDAC_HW                                          RF_VCO_IDAC_HW
#define RF_VCO_IDAC_HW_POS                                      (24U)
#define RF_VCO_IDAC_HW_LEN                                      (6U)
#define RF_VCO_IDAC_HW_MSK                                      (((1U<<RF_VCO_IDAC_HW_LEN)-1)<<RF_VCO_IDAC_HW_POS)
#define RF_VCO_IDAC_HW_UMSK                                     (~(((1U<<RF_VCO_IDAC_HW_LEN)-1)<<RF_VCO_IDAC_HW_POS))
#define RF_VCO_ACAL_UD                                          RF_VCO_ACAL_UD
#define RF_VCO_ACAL_UD_POS                                      (31U)
#define RF_VCO_ACAL_UD_LEN                                      (1U)
#define RF_VCO_ACAL_UD_MSK                                      (((1U<<RF_VCO_ACAL_UD_LEN)-1)<<RF_VCO_ACAL_UD_POS)
#define RF_VCO_ACAL_UD_UMSK                                     (~(((1U<<RF_VCO_ACAL_UD_LEN)-1)<<RF_VCO_ACAL_UD_POS))

/* 0x23C : lodist */
#define RF_LODIST_OFFSET                                        (0x23C)
#define RF_LODIST_TXBUF_SUPPLY_MODE                             RF_LODIST_TXBUF_SUPPLY_MODE
#define RF_LODIST_TXBUF_SUPPLY_MODE_POS                         (0U)
#define RF_LODIST_TXBUF_SUPPLY_MODE_LEN                         (2U)
#define RF_LODIST_TXBUF_SUPPLY_MODE_MSK                         (((1U<<RF_LODIST_TXBUF_SUPPLY_MODE_LEN)-1)<<RF_LODIST_TXBUF_SUPPLY_MODE_POS)
#define RF_LODIST_TXBUF_SUPPLY_MODE_UMSK                        (~(((1U<<RF_LODIST_TXBUF_SUPPLY_MODE_LEN)-1)<<RF_LODIST_TXBUF_SUPPLY_MODE_POS))
#define RF_LODIST_RXBUF_SUPPLY_MODE                             RF_LODIST_RXBUF_SUPPLY_MODE
#define RF_LODIST_RXBUF_SUPPLY_MODE_POS                         (4U)
#define RF_LODIST_RXBUF_SUPPLY_MODE_LEN                         (2U)
#define RF_LODIST_RXBUF_SUPPLY_MODE_MSK                         (((1U<<RF_LODIST_RXBUF_SUPPLY_MODE_LEN)-1)<<RF_LODIST_RXBUF_SUPPLY_MODE_POS)
#define RF_LODIST_RXBUF_SUPPLY_MODE_UMSK                        (~(((1U<<RF_LODIST_RXBUF_SUPPLY_MODE_LEN)-1)<<RF_LODIST_RXBUF_SUPPLY_MODE_POS))
#define RF_LODIST_RXBUF_SUPPLY_BOOST                            RF_LODIST_RXBUF_SUPPLY_BOOST
#define RF_LODIST_RXBUF_SUPPLY_BOOST_POS                        (6U)
#define RF_LODIST_RXBUF_SUPPLY_BOOST_LEN                        (1U)
#define RF_LODIST_RXBUF_SUPPLY_BOOST_MSK                        (((1U<<RF_LODIST_RXBUF_SUPPLY_BOOST_LEN)-1)<<RF_LODIST_RXBUF_SUPPLY_BOOST_POS)
#define RF_LODIST_RXBUF_SUPPLY_BOOST_UMSK                       (~(((1U<<RF_LODIST_RXBUF_SUPPLY_BOOST_LEN)-1)<<RF_LODIST_RXBUF_SUPPLY_BOOST_POS))
#define RF_LODIST_RWELL_BIAS                                    RF_LODIST_RWELL_BIAS
#define RF_LODIST_RWELL_BIAS_POS                                (8U)
#define RF_LODIST_RWELL_BIAS_LEN                                (2U)
#define RF_LODIST_RWELL_BIAS_MSK                                (((1U<<RF_LODIST_RWELL_BIAS_LEN)-1)<<RF_LODIST_RWELL_BIAS_POS)
#define RF_LODIST_RWELL_BIAS_UMSK                               (~(((1U<<RF_LODIST_RWELL_BIAS_LEN)-1)<<RF_LODIST_RWELL_BIAS_POS))
#define RF_LODIST_NWELL_BIAS                                    RF_LODIST_NWELL_BIAS
#define RF_LODIST_NWELL_BIAS_POS                                (12U)
#define RF_LODIST_NWELL_BIAS_LEN                                (2U)
#define RF_LODIST_NWELL_BIAS_MSK                                (((1U<<RF_LODIST_NWELL_BIAS_LEN)-1)<<RF_LODIST_NWELL_BIAS_POS)
#define RF_LODIST_NWELL_BIAS_UMSK                               (~(((1U<<RF_LODIST_NWELL_BIAS_LEN)-1)<<RF_LODIST_NWELL_BIAS_POS))
#define RF_LODIST_75DC_SEL                                      RF_LODIST_75DC_SEL
#define RF_LODIST_75DC_SEL_POS                                  (16U)
#define RF_LODIST_75DC_SEL_LEN                                  (1U)
#define RF_LODIST_75DC_SEL_MSK                                  (((1U<<RF_LODIST_75DC_SEL_LEN)-1)<<RF_LODIST_75DC_SEL_POS)
#define RF_LODIST_75DC_SEL_UMSK                                 (~(((1U<<RF_LODIST_75DC_SEL_LEN)-1)<<RF_LODIST_75DC_SEL_POS))

/* 0x240 : fbdv */
#define RF_FBDV_OFFSET                                          (0x240)
#define RF_FBDV_TPM_CLK_SEL                                     RF_FBDV_TPM_CLK_SEL
#define RF_FBDV_TPM_CLK_SEL_POS                                 (0U)
#define RF_FBDV_TPM_CLK_SEL_LEN                                 (3U)
#define RF_FBDV_TPM_CLK_SEL_MSK                                 (((1U<<RF_FBDV_TPM_CLK_SEL_LEN)-1)<<RF_FBDV_TPM_CLK_SEL_POS)
#define RF_FBDV_TPM_CLK_SEL_UMSK                                (~(((1U<<RF_FBDV_TPM_CLK_SEL_LEN)-1)<<RF_FBDV_TPM_CLK_SEL_POS))
#define RF_FBDV_ADPLL_CLK_SEL                                   RF_FBDV_ADPLL_CLK_SEL
#define RF_FBDV_ADPLL_CLK_SEL_POS                               (4U)
#define RF_FBDV_ADPLL_CLK_SEL_LEN                               (1U)
#define RF_FBDV_ADPLL_CLK_SEL_MSK                               (((1U<<RF_FBDV_ADPLL_CLK_SEL_LEN)-1)<<RF_FBDV_ADPLL_CLK_SEL_POS)
#define RF_FBDV_ADPLL_CLK_SEL_UMSK                              (~(((1U<<RF_FBDV_ADPLL_CLK_SEL_LEN)-1)<<RF_FBDV_ADPLL_CLK_SEL_POS))
#define RF_FBDV_DCO_DITHER_CLK_SEL                              RF_FBDV_DCO_DITHER_CLK_SEL
#define RF_FBDV_DCO_DITHER_CLK_SEL_POS                          (8U)
#define RF_FBDV_DCO_DITHER_CLK_SEL_LEN                          (1U)
#define RF_FBDV_DCO_DITHER_CLK_SEL_MSK                          (((1U<<RF_FBDV_DCO_DITHER_CLK_SEL_LEN)-1)<<RF_FBDV_DCO_DITHER_CLK_SEL_POS)
#define RF_FBDV_DCO_DITHER_CLK_SEL_UMSK                         (~(((1U<<RF_FBDV_DCO_DITHER_CLK_SEL_LEN)-1)<<RF_FBDV_DCO_DITHER_CLK_SEL_POS))
#define RF_FBDV_FB_CLK_SEL                                      RF_FBDV_FB_CLK_SEL
#define RF_FBDV_FB_CLK_SEL_POS                                  (12U)
#define RF_FBDV_FB_CLK_SEL_LEN                                  (1U)
#define RF_FBDV_FB_CLK_SEL_MSK                                  (((1U<<RF_FBDV_FB_CLK_SEL_LEN)-1)<<RF_FBDV_FB_CLK_SEL_POS)
#define RF_FBDV_FB_CLK_SEL_UMSK                                 (~(((1U<<RF_FBDV_FB_CLK_SEL_LEN)-1)<<RF_FBDV_FB_CLK_SEL_POS))
#define RF_FBDV_SAMPLE_CLK_SEL                                  RF_FBDV_SAMPLE_CLK_SEL
#define RF_FBDV_SAMPLE_CLK_SEL_POS                              (16U)
#define RF_FBDV_SAMPLE_CLK_SEL_LEN                              (2U)
#define RF_FBDV_SAMPLE_CLK_SEL_MSK                              (((1U<<RF_FBDV_SAMPLE_CLK_SEL_LEN)-1)<<RF_FBDV_SAMPLE_CLK_SEL_POS)
#define RF_FBDV_SAMPLE_CLK_SEL_UMSK                             (~(((1U<<RF_FBDV_SAMPLE_CLK_SEL_LEN)-1)<<RF_FBDV_SAMPLE_CLK_SEL_POS))
#define RF_FBDV_STG_SEL                                         RF_FBDV_STG_SEL
#define RF_FBDV_STG_SEL_POS                                     (20U)
#define RF_FBDV_STG_SEL_LEN                                     (1U)
#define RF_FBDV_STG_SEL_MSK                                     (((1U<<RF_FBDV_STG_SEL_LEN)-1)<<RF_FBDV_STG_SEL_POS)
#define RF_FBDV_STG_SEL_UMSK                                    (~(((1U<<RF_FBDV_STG_SEL_LEN)-1)<<RF_FBDV_STG_SEL_POS))
#define RF_RST_MMDIV                                            RF_RST_MMDIV
#define RF_RST_MMDIV_POS                                        (24U)
#define RF_RST_MMDIV_LEN                                        (1U)
#define RF_RST_MMDIV_MSK                                        (((1U<<RF_RST_MMDIV_LEN)-1)<<RF_RST_MMDIV_POS)
#define RF_RST_MMDIV_UMSK                                       (~(((1U<<RF_RST_MMDIV_LEN)-1)<<RF_RST_MMDIV_POS))
#define RF_LOTPM_FMASH_CLK_POLARITY                             RF_LOTPM_FMASH_CLK_POLARITY
#define RF_LOTPM_FMASH_CLK_POLARITY_POS                         (28U)
#define RF_LOTPM_FMASH_CLK_POLARITY_LEN                         (1U)
#define RF_LOTPM_FMASH_CLK_POLARITY_MSK                         (((1U<<RF_LOTPM_FMASH_CLK_POLARITY_LEN)-1)<<RF_LOTPM_FMASH_CLK_POLARITY_POS)
#define RF_LOTPM_FMASH_CLK_POLARITY_UMSK                        (~(((1U<<RF_LOTPM_FMASH_CLK_POLARITY_LEN)-1)<<RF_LOTPM_FMASH_CLK_POLARITY_POS))
#define RF_DCO_DITHER_CLK_POLARITY                              RF_DCO_DITHER_CLK_POLARITY
#define RF_DCO_DITHER_CLK_POLARITY_POS                          (29U)
#define RF_DCO_DITHER_CLK_POLARITY_LEN                          (1U)
#define RF_DCO_DITHER_CLK_POLARITY_MSK                          (((1U<<RF_DCO_DITHER_CLK_POLARITY_LEN)-1)<<RF_DCO_DITHER_CLK_POLARITY_POS)
#define RF_DCO_DITHER_CLK_POLARITY_UMSK                         (~(((1U<<RF_DCO_DITHER_CLK_POLARITY_LEN)-1)<<RF_DCO_DITHER_CLK_POLARITY_POS))

/* 0x244 : kcal1 */
#define RF_KCAL1_OFFSET                                         (0x244)
#define RF_KCAL_DIV                                             RF_KCAL_DIV
#define RF_KCAL_DIV_POS                                         (0U)
#define RF_KCAL_DIV_LEN                                         (16U)
#define RF_KCAL_DIV_MSK                                         (((1U<<RF_KCAL_DIV_LEN)-1)<<RF_KCAL_DIV_POS)
#define RF_KCAL_DIV_UMSK                                        (~(((1U<<RF_KCAL_DIV_LEN)-1)<<RF_KCAL_DIV_POS))
#define RF_KCAL_CNT_START                                       RF_KCAL_CNT_START
#define RF_KCAL_CNT_START_POS                                   (16U)
#define RF_KCAL_CNT_START_LEN                                   (1U)
#define RF_KCAL_CNT_START_MSK                                   (((1U<<RF_KCAL_CNT_START_LEN)-1)<<RF_KCAL_CNT_START_POS)
#define RF_KCAL_CNT_START_UMSK                                  (~(((1U<<RF_KCAL_CNT_START_LEN)-1)<<RF_KCAL_CNT_START_POS))
#define RF_KCAL_RATIO                                           RF_KCAL_RATIO
#define RF_KCAL_RATIO_POS                                       (20U)
#define RF_KCAL_RATIO_LEN                                       (10U)
#define RF_KCAL_RATIO_MSK                                       (((1U<<RF_KCAL_RATIO_LEN)-1)<<RF_KCAL_RATIO_POS)
#define RF_KCAL_RATIO_UMSK                                      (~(((1U<<RF_KCAL_RATIO_LEN)-1)<<RF_KCAL_RATIO_POS))

/* 0x248 : kcal2 */
#define RF_KCAL2_OFFSET                                         (0x248)
#define RF_KCAL_CNT_OP                                          RF_KCAL_CNT_OP
#define RF_KCAL_CNT_OP_POS                                      (0U)
#define RF_KCAL_CNT_OP_LEN                                      (16U)
#define RF_KCAL_CNT_OP_MSK                                      (((1U<<RF_KCAL_CNT_OP_LEN)-1)<<RF_KCAL_CNT_OP_POS)
#define RF_KCAL_CNT_OP_UMSK                                     (~(((1U<<RF_KCAL_CNT_OP_LEN)-1)<<RF_KCAL_CNT_OP_POS))
#define RF_KCAL_CNT_RDY                                         RF_KCAL_CNT_RDY
#define RF_KCAL_CNT_RDY_POS                                     (16U)
#define RF_KCAL_CNT_RDY_LEN                                     (1U)
#define RF_KCAL_CNT_RDY_MSK                                     (((1U<<RF_KCAL_CNT_RDY_LEN)-1)<<RF_KCAL_CNT_RDY_POS)
#define RF_KCAL_CNT_RDY_UMSK                                    (~(((1U<<RF_KCAL_CNT_RDY_LEN)-1)<<RF_KCAL_CNT_RDY_POS))
#define RF_KCAL_RATIO_HW                                        RF_KCAL_RATIO_HW
#define RF_KCAL_RATIO_HW_POS                                    (20U)
#define RF_KCAL_RATIO_HW_LEN                                    (10U)
#define RF_KCAL_RATIO_HW_MSK                                    (((1U<<RF_KCAL_RATIO_HW_LEN)-1)<<RF_KCAL_RATIO_HW_POS)
#define RF_KCAL_RATIO_HW_UMSK                                   (~(((1U<<RF_KCAL_RATIO_HW_LEN)-1)<<RF_KCAL_RATIO_HW_POS))

/* 0x24C : adpll_slope_gen */
#define RF_ADPLL_SLOPE_GEN_OFFSET                               (0x24C)
#define RF_ADPLL_SLOPE_GEN_R_SEL                                RF_ADPLL_SLOPE_GEN_R_SEL
#define RF_ADPLL_SLOPE_GEN_R_SEL_POS                            (0U)
#define RF_ADPLL_SLOPE_GEN_R_SEL_LEN                            (3U)
#define RF_ADPLL_SLOPE_GEN_R_SEL_MSK                            (((1U<<RF_ADPLL_SLOPE_GEN_R_SEL_LEN)-1)<<RF_ADPLL_SLOPE_GEN_R_SEL_POS)
#define RF_ADPLL_SLOPE_GEN_R_SEL_UMSK                           (~(((1U<<RF_ADPLL_SLOPE_GEN_R_SEL_LEN)-1)<<RF_ADPLL_SLOPE_GEN_R_SEL_POS))
#define RF_ADPLL_SLOPE_GEN_DC_CORR                              RF_ADPLL_SLOPE_GEN_DC_CORR
#define RF_ADPLL_SLOPE_GEN_DC_CORR_POS                          (4U)
#define RF_ADPLL_SLOPE_GEN_DC_CORR_LEN                          (2U)
#define RF_ADPLL_SLOPE_GEN_DC_CORR_MSK                          (((1U<<RF_ADPLL_SLOPE_GEN_DC_CORR_LEN)-1)<<RF_ADPLL_SLOPE_GEN_DC_CORR_POS)
#define RF_ADPLL_SLOPE_GEN_DC_CORR_UMSK                         (~(((1U<<RF_ADPLL_SLOPE_GEN_DC_CORR_LEN)-1)<<RF_ADPLL_SLOPE_GEN_DC_CORR_POS))
#define RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE                  RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE
#define RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_POS              (6U)
#define RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_LEN              (1U)
#define RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_MSK              (((1U<<RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_LEN)-1)<<RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_POS)
#define RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_UMSK             (~(((1U<<RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_LEN)-1)<<RF_ADPLL_SLOPE_GEN_PULSE_WIDTH_ENHANCE_POS))

/* 0x250 : adpll_adc */
#define RF_ADPLL_ADC_OFFSET                                     (0x250)
#define RF_ADPLL_ADC_VTH_BIAS_MODE                              RF_ADPLL_ADC_VTH_BIAS_MODE
#define RF_ADPLL_ADC_VTH_BIAS_MODE_POS                          (0U)
#define RF_ADPLL_ADC_VTH_BIAS_MODE_LEN                          (1U)
#define RF_ADPLL_ADC_VTH_BIAS_MODE_MSK                          (((1U<<RF_ADPLL_ADC_VTH_BIAS_MODE_LEN)-1)<<RF_ADPLL_ADC_VTH_BIAS_MODE_POS)
#define RF_ADPLL_ADC_VTH_BIAS_MODE_UMSK                         (~(((1U<<RF_ADPLL_ADC_VTH_BIAS_MODE_LEN)-1)<<RF_ADPLL_ADC_VTH_BIAS_MODE_POS))
#define RF_ADPLL_ADC_VTH_EN                                     RF_ADPLL_ADC_VTH_EN
#define RF_ADPLL_ADC_VTH_EN_POS                                 (1U)
#define RF_ADPLL_ADC_VTH_EN_LEN                                 (1U)
#define RF_ADPLL_ADC_VTH_EN_MSK                                 (((1U<<RF_ADPLL_ADC_VTH_EN_LEN)-1)<<RF_ADPLL_ADC_VTH_EN_POS)
#define RF_ADPLL_ADC_VTH_EN_UMSK                                (~(((1U<<RF_ADPLL_ADC_VTH_EN_LEN)-1)<<RF_ADPLL_ADC_VTH_EN_POS))
#define RF_ADPLL_ADC_DATA_SIGN_SEL                              RF_ADPLL_ADC_DATA_SIGN_SEL
#define RF_ADPLL_ADC_DATA_SIGN_SEL_POS                          (2U)
#define RF_ADPLL_ADC_DATA_SIGN_SEL_LEN                          (1U)
#define RF_ADPLL_ADC_DATA_SIGN_SEL_MSK                          (((1U<<RF_ADPLL_ADC_DATA_SIGN_SEL_LEN)-1)<<RF_ADPLL_ADC_DATA_SIGN_SEL_POS)
#define RF_ADPLL_ADC_DATA_SIGN_SEL_UMSK                         (~(((1U<<RF_ADPLL_ADC_DATA_SIGN_SEL_LEN)-1)<<RF_ADPLL_ADC_DATA_SIGN_SEL_POS))
#define RF_ADPLL_ADC_VREF_FINE                                  RF_ADPLL_ADC_VREF_FINE
#define RF_ADPLL_ADC_VREF_FINE_POS                              (4U)
#define RF_ADPLL_ADC_VREF_FINE_LEN                              (2U)
#define RF_ADPLL_ADC_VREF_FINE_MSK                              (((1U<<RF_ADPLL_ADC_VREF_FINE_LEN)-1)<<RF_ADPLL_ADC_VREF_FINE_POS)
#define RF_ADPLL_ADC_VREF_FINE_UMSK                             (~(((1U<<RF_ADPLL_ADC_VREF_FINE_LEN)-1)<<RF_ADPLL_ADC_VREF_FINE_POS))
#define RF_ADPLL_ADC_VREF_COARSE                                RF_ADPLL_ADC_VREF_COARSE
#define RF_ADPLL_ADC_VREF_COARSE_POS                            (8U)
#define RF_ADPLL_ADC_VREF_COARSE_LEN                            (2U)
#define RF_ADPLL_ADC_VREF_COARSE_MSK                            (((1U<<RF_ADPLL_ADC_VREF_COARSE_LEN)-1)<<RF_ADPLL_ADC_VREF_COARSE_POS)
#define RF_ADPLL_ADC_VREF_COARSE_UMSK                           (~(((1U<<RF_ADPLL_ADC_VREF_COARSE_LEN)-1)<<RF_ADPLL_ADC_VREF_COARSE_POS))
#define RF_ADPLL_ADC_OSCAL_EN                                   RF_ADPLL_ADC_OSCAL_EN
#define RF_ADPLL_ADC_OSCAL_EN_POS                               (12U)
#define RF_ADPLL_ADC_OSCAL_EN_LEN                               (1U)
#define RF_ADPLL_ADC_OSCAL_EN_MSK                               (((1U<<RF_ADPLL_ADC_OSCAL_EN_LEN)-1)<<RF_ADPLL_ADC_OSCAL_EN_POS)
#define RF_ADPLL_ADC_OSCAL_EN_UMSK                              (~(((1U<<RF_ADPLL_ADC_OSCAL_EN_LEN)-1)<<RF_ADPLL_ADC_OSCAL_EN_POS))
#define RF_ADPLL_ADC_CLK_SYNC_INV                               RF_ADPLL_ADC_CLK_SYNC_INV
#define RF_ADPLL_ADC_CLK_SYNC_INV_POS                           (16U)
#define RF_ADPLL_ADC_CLK_SYNC_INV_LEN                           (1U)
#define RF_ADPLL_ADC_CLK_SYNC_INV_MSK                           (((1U<<RF_ADPLL_ADC_CLK_SYNC_INV_LEN)-1)<<RF_ADPLL_ADC_CLK_SYNC_INV_POS)
#define RF_ADPLL_ADC_CLK_SYNC_INV_UMSK                          (~(((1U<<RF_ADPLL_ADC_CLK_SYNC_INV_LEN)-1)<<RF_ADPLL_ADC_CLK_SYNC_INV_POS))
#define RF_ADPLL_ADC_CLK_DIV_SEL                                RF_ADPLL_ADC_CLK_DIV_SEL
#define RF_ADPLL_ADC_CLK_DIV_SEL_POS                            (24U)
#define RF_ADPLL_ADC_CLK_DIV_SEL_LEN                            (1U)
#define RF_ADPLL_ADC_CLK_DIV_SEL_MSK                            (((1U<<RF_ADPLL_ADC_CLK_DIV_SEL_LEN)-1)<<RF_ADPLL_ADC_CLK_DIV_SEL_POS)
#define RF_ADPLL_ADC_CLK_DIV_SEL_UMSK                           (~(((1U<<RF_ADPLL_ADC_CLK_DIV_SEL_LEN)-1)<<RF_ADPLL_ADC_CLK_DIV_SEL_POS))
#define RF_ADPLL_ADC_CLK_INV                                    RF_ADPLL_ADC_CLK_INV
#define RF_ADPLL_ADC_CLK_INV_POS                                (28U)
#define RF_ADPLL_ADC_CLK_INV_LEN                                (1U)
#define RF_ADPLL_ADC_CLK_INV_MSK                                (((1U<<RF_ADPLL_ADC_CLK_INV_LEN)-1)<<RF_ADPLL_ADC_CLK_INV_POS)
#define RF_ADPLL_ADC_CLK_INV_UMSK                               (~(((1U<<RF_ADPLL_ADC_CLK_INV_LEN)-1)<<RF_ADPLL_ADC_CLK_INV_POS))
#define RF_ADPLL_ADC_CLK_EN                                     RF_ADPLL_ADC_CLK_EN
#define RF_ADPLL_ADC_CLK_EN_POS                                 (29U)
#define RF_ADPLL_ADC_CLK_EN_LEN                                 (1U)
#define RF_ADPLL_ADC_CLK_EN_MSK                                 (((1U<<RF_ADPLL_ADC_CLK_EN_LEN)-1)<<RF_ADPLL_ADC_CLK_EN_POS)
#define RF_ADPLL_ADC_CLK_EN_UMSK                                (~(((1U<<RF_ADPLL_ADC_CLK_EN_LEN)-1)<<RF_ADPLL_ADC_CLK_EN_POS))

/* 0x254 : adpll_dtc */
#define RF_ADPLL_DTC_OFFSET                                     (0x254)
#define RF_ADPLL_DTC_BYPASS                                     RF_ADPLL_DTC_BYPASS
#define RF_ADPLL_DTC_BYPASS_POS                                 (0U)
#define RF_ADPLL_DTC_BYPASS_LEN                                 (1U)
#define RF_ADPLL_DTC_BYPASS_MSK                                 (((1U<<RF_ADPLL_DTC_BYPASS_LEN)-1)<<RF_ADPLL_DTC_BYPASS_POS)
#define RF_ADPLL_DTC_BYPASS_UMSK                                (~(((1U<<RF_ADPLL_DTC_BYPASS_LEN)-1)<<RF_ADPLL_DTC_BYPASS_POS))
#define RF_ADPLL_DTC_R_SEL                                      RF_ADPLL_DTC_R_SEL
#define RF_ADPLL_DTC_R_SEL_POS                                  (4U)
#define RF_ADPLL_DTC_R_SEL_LEN                                  (3U)
#define RF_ADPLL_DTC_R_SEL_MSK                                  (((1U<<RF_ADPLL_DTC_R_SEL_LEN)-1)<<RF_ADPLL_DTC_R_SEL_POS)
#define RF_ADPLL_DTC_R_SEL_UMSK                                 (~(((1U<<RF_ADPLL_DTC_R_SEL_LEN)-1)<<RF_ADPLL_DTC_R_SEL_POS))
#define RF_ADPLL_DTC_INV_VTH_SEL                                RF_ADPLL_DTC_INV_VTH_SEL
#define RF_ADPLL_DTC_INV_VTH_SEL_POS                            (8U)
#define RF_ADPLL_DTC_INV_VTH_SEL_LEN                            (2U)
#define RF_ADPLL_DTC_INV_VTH_SEL_MSK                            (((1U<<RF_ADPLL_DTC_INV_VTH_SEL_LEN)-1)<<RF_ADPLL_DTC_INV_VTH_SEL_POS)
#define RF_ADPLL_DTC_INV_VTH_SEL_UMSK                           (~(((1U<<RF_ADPLL_DTC_INV_VTH_SEL_LEN)-1)<<RF_ADPLL_DTC_INV_VTH_SEL_POS))

/* 0x258 : lo_fc_config1 */
#define RF_LO_FC_CONFIG1_OFFSET                                 (0x258)
#define RF_LO_FCW                                               RF_LO_FCW
#define RF_LO_FCW_POS                                           (0U)
#define RF_LO_FCW_LEN                                           (25U)
#define RF_LO_FCW_MSK                                           (((1U<<RF_LO_FCW_LEN)-1)<<RF_LO_FCW_POS)
#define RF_LO_FCW_UMSK                                          (~(((1U<<RF_LO_FCW_LEN)-1)<<RF_LO_FCW_POS))

/* 0x25C : lo_fcw_config2 */
#define RF_LO_FCW_CONFIG2_OFFSET                                (0x25C)
#define RF_LO_FCW_HW                                            RF_LO_FCW_HW
#define RF_LO_FCW_HW_POS                                        (0U)
#define RF_LO_FCW_HW_LEN                                        (25U)
#define RF_LO_FCW_HW_MSK                                        (((1U<<RF_LO_FCW_HW_LEN)-1)<<RF_LO_FCW_HW_POS)
#define RF_LO_FCW_HW_UMSK                                       (~(((1U<<RF_LO_FCW_HW_LEN)-1)<<RF_LO_FCW_HW_POS))

/* 0x260 : lo_fcw3 */
#define RF_LO_FCW3_OFFSET                                       (0x260)
#define RF_TX_FREQ_MOD_LP                                       RF_TX_FREQ_MOD_LP
#define RF_TX_FREQ_MOD_LP_POS                                   (0U)
#define RF_TX_FREQ_MOD_LP_LEN                                   (12U)
#define RF_TX_FREQ_MOD_LP_MSK                                   (((1U<<RF_TX_FREQ_MOD_LP_LEN)-1)<<RF_TX_FREQ_MOD_LP_POS)
#define RF_TX_FREQ_MOD_LP_UMSK                                  (~(((1U<<RF_TX_FREQ_MOD_LP_LEN)-1)<<RF_TX_FREQ_MOD_LP_POS))
#define RF_TX_FREQ_MOD_HP                                       RF_TX_FREQ_MOD_HP
#define RF_TX_FREQ_MOD_HP_POS                                   (16U)
#define RF_TX_FREQ_MOD_HP_LEN                                   (10U)
#define RF_TX_FREQ_MOD_HP_MSK                                   (((1U<<RF_TX_FREQ_MOD_HP_LEN)-1)<<RF_TX_FREQ_MOD_HP_POS)
#define RF_TX_FREQ_MOD_HP_UMSK                                  (~(((1U<<RF_TX_FREQ_MOD_HP_LEN)-1)<<RF_TX_FREQ_MOD_HP_POS))

/* 0x264 : lotpm */
#define RF_LOTPM_OFFSET                                         (0x264)
#define RF_LOTPM_LFP_DELAY_SEL                                  RF_LOTPM_LFP_DELAY_SEL
#define RF_LOTPM_LFP_DELAY_SEL_POS                              (0U)
#define RF_LOTPM_LFP_DELAY_SEL_LEN                              (2U)
#define RF_LOTPM_LFP_DELAY_SEL_MSK                              (((1U<<RF_LOTPM_LFP_DELAY_SEL_LEN)-1)<<RF_LOTPM_LFP_DELAY_SEL_POS)
#define RF_LOTPM_LFP_DELAY_SEL_UMSK                             (~(((1U<<RF_LOTPM_LFP_DELAY_SEL_LEN)-1)<<RF_LOTPM_LFP_DELAY_SEL_POS))
#define RF_LOTPM_HFP_DELAY_FMASH                                RF_LOTPM_HFP_DELAY_FMASH
#define RF_LOTPM_HFP_DELAY_FMASH_POS                            (4U)
#define RF_LOTPM_HFP_DELAY_FMASH_LEN                            (4U)
#define RF_LOTPM_HFP_DELAY_FMASH_MSK                            (((1U<<RF_LOTPM_HFP_DELAY_FMASH_LEN)-1)<<RF_LOTPM_HFP_DELAY_FMASH_POS)
#define RF_LOTPM_HFP_DELAY_FMASH_UMSK                           (~(((1U<<RF_LOTPM_HFP_DELAY_FMASH_LEN)-1)<<RF_LOTPM_HFP_DELAY_FMASH_POS))
#define RF_LOTPM_HFP_DELAY_FREF                                 RF_LOTPM_HFP_DELAY_FREF
#define RF_LOTPM_HFP_DELAY_FREF_POS                             (8U)
#define RF_LOTPM_HFP_DELAY_FREF_LEN                             (2U)
#define RF_LOTPM_HFP_DELAY_FREF_MSK                             (((1U<<RF_LOTPM_HFP_DELAY_FREF_LEN)-1)<<RF_LOTPM_HFP_DELAY_FREF_POS)
#define RF_LOTPM_HFP_DELAY_FREF_UMSK                            (~(((1U<<RF_LOTPM_HFP_DELAY_FREF_LEN)-1)<<RF_LOTPM_HFP_DELAY_FREF_POS))
#define RF_LOTPM_HFP_POLARITY                                   RF_LOTPM_HFP_POLARITY
#define RF_LOTPM_HFP_POLARITY_POS                               (12U)
#define RF_LOTPM_HFP_POLARITY_LEN                               (1U)
#define RF_LOTPM_HFP_POLARITY_MSK                               (((1U<<RF_LOTPM_HFP_POLARITY_LEN)-1)<<RF_LOTPM_HFP_POLARITY_POS)
#define RF_LOTPM_HFP_POLARITY_UMSK                              (~(((1U<<RF_LOTPM_HFP_POLARITY_LEN)-1)<<RF_LOTPM_HFP_POLARITY_POS))
#define RF_LOTPM_HFP_MASH1_SEL                                  RF_LOTPM_HFP_MASH1_SEL
#define RF_LOTPM_HFP_MASH1_SEL_POS                              (16U)
#define RF_LOTPM_HFP_MASH1_SEL_LEN                              (1U)
#define RF_LOTPM_HFP_MASH1_SEL_MSK                              (((1U<<RF_LOTPM_HFP_MASH1_SEL_LEN)-1)<<RF_LOTPM_HFP_MASH1_SEL_POS)
#define RF_LOTPM_HFP_MASH1_SEL_UMSK                             (~(((1U<<RF_LOTPM_HFP_MASH1_SEL_LEN)-1)<<RF_LOTPM_HFP_MASH1_SEL_POS))

/* 0x268 : adpll1 */
#define RF_ADPLL1_OFFSET                                        (0x268)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR                         RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_POS                     (0U)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_LEN                     (1U)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_MSK                     (((1U<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_LEN)-1)<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_POS)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_UMSK                    (~(((1U<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_LEN)-1)<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_POS))
#define RF_ADPLL_LO_LOCK_SEL                                    RF_ADPLL_LO_LOCK_SEL
#define RF_ADPLL_LO_LOCK_SEL_POS                                (1U)
#define RF_ADPLL_LO_LOCK_SEL_LEN                                (1U)
#define RF_ADPLL_LO_LOCK_SEL_MSK                                (((1U<<RF_ADPLL_LO_LOCK_SEL_LEN)-1)<<RF_ADPLL_LO_LOCK_SEL_POS)
#define RF_ADPLL_LO_LOCK_SEL_UMSK                               (~(((1U<<RF_ADPLL_LO_LOCK_SEL_LEN)-1)<<RF_ADPLL_LO_LOCK_SEL_POS))
#define RF_ADPLL_TIMEOUT_CNT1_SEL                               RF_ADPLL_TIMEOUT_CNT1_SEL
#define RF_ADPLL_TIMEOUT_CNT1_SEL_POS                           (2U)
#define RF_ADPLL_TIMEOUT_CNT1_SEL_LEN                           (1U)
#define RF_ADPLL_TIMEOUT_CNT1_SEL_MSK                           (((1U<<RF_ADPLL_TIMEOUT_CNT1_SEL_LEN)-1)<<RF_ADPLL_TIMEOUT_CNT1_SEL_POS)
#define RF_ADPLL_TIMEOUT_CNT1_SEL_UMSK                          (~(((1U<<RF_ADPLL_TIMEOUT_CNT1_SEL_LEN)-1)<<RF_ADPLL_TIMEOUT_CNT1_SEL_POS))
#define RF_ADPLL_TIMEOUT_CNT_SEL                                RF_ADPLL_TIMEOUT_CNT_SEL
#define RF_ADPLL_TIMEOUT_CNT_SEL_POS                            (3U)
#define RF_ADPLL_TIMEOUT_CNT_SEL_LEN                            (1U)
#define RF_ADPLL_TIMEOUT_CNT_SEL_MSK                            (((1U<<RF_ADPLL_TIMEOUT_CNT_SEL_LEN)-1)<<RF_ADPLL_TIMEOUT_CNT_SEL_POS)
#define RF_ADPLL_TIMEOUT_CNT_SEL_UMSK                           (~(((1U<<RF_ADPLL_TIMEOUT_CNT_SEL_LEN)-1)<<RF_ADPLL_TIMEOUT_CNT_SEL_POS))
#define RF_ADPLL_MOMHOLD_LMSENB_EXT                             RF_ADPLL_MOMHOLD_LMSENB_EXT
#define RF_ADPLL_MOMHOLD_LMSENB_EXT_POS                         (4U)
#define RF_ADPLL_MOMHOLD_LMSENB_EXT_LEN                         (1U)
#define RF_ADPLL_MOMHOLD_LMSENB_EXT_MSK                         (((1U<<RF_ADPLL_MOMHOLD_LMSENB_EXT_LEN)-1)<<RF_ADPLL_MOMHOLD_LMSENB_EXT_POS)
#define RF_ADPLL_MOMHOLD_LMSENB_EXT_UMSK                        (~(((1U<<RF_ADPLL_MOMHOLD_LMSENB_EXT_LEN)-1)<<RF_ADPLL_MOMHOLD_LMSENB_EXT_POS))
#define RF_ADPLL_RST_COARSE_DET_EXT                             RF_ADPLL_RST_COARSE_DET_EXT
#define RF_ADPLL_RST_COARSE_DET_EXT_POS                         (5U)
#define RF_ADPLL_RST_COARSE_DET_EXT_LEN                         (1U)
#define RF_ADPLL_RST_COARSE_DET_EXT_MSK                         (((1U<<RF_ADPLL_RST_COARSE_DET_EXT_LEN)-1)<<RF_ADPLL_RST_COARSE_DET_EXT_POS)
#define RF_ADPLL_RST_COARSE_DET_EXT_UMSK                        (~(((1U<<RF_ADPLL_RST_COARSE_DET_EXT_LEN)-1)<<RF_ADPLL_RST_COARSE_DET_EXT_POS))
#define RF_ADPLL_RST_SPD_DET_EXT                                RF_ADPLL_RST_SPD_DET_EXT
#define RF_ADPLL_RST_SPD_DET_EXT_POS                            (6U)
#define RF_ADPLL_RST_SPD_DET_EXT_LEN                            (1U)
#define RF_ADPLL_RST_SPD_DET_EXT_MSK                            (((1U<<RF_ADPLL_RST_SPD_DET_EXT_LEN)-1)<<RF_ADPLL_RST_SPD_DET_EXT_POS)
#define RF_ADPLL_RST_SPD_DET_EXT_UMSK                           (~(((1U<<RF_ADPLL_RST_SPD_DET_EXT_LEN)-1)<<RF_ADPLL_RST_SPD_DET_EXT_POS))
#define RF_ADPLL_LOOP_LOCK_EXT                                  RF_ADPLL_LOOP_LOCK_EXT
#define RF_ADPLL_LOOP_LOCK_EXT_POS                              (7U)
#define RF_ADPLL_LOOP_LOCK_EXT_LEN                              (1U)
#define RF_ADPLL_LOOP_LOCK_EXT_MSK                              (((1U<<RF_ADPLL_LOOP_LOCK_EXT_LEN)-1)<<RF_ADPLL_LOOP_LOCK_EXT_POS)
#define RF_ADPLL_LOOP_LOCK_EXT_UMSK                             (~(((1U<<RF_ADPLL_LOOP_LOCK_EXT_LEN)-1)<<RF_ADPLL_LOOP_LOCK_EXT_POS))
#define RF_ADPLL_FCAL_DONE_EXT                                  RF_ADPLL_FCAL_DONE_EXT
#define RF_ADPLL_FCAL_DONE_EXT_POS                              (8U)
#define RF_ADPLL_FCAL_DONE_EXT_LEN                              (1U)
#define RF_ADPLL_FCAL_DONE_EXT_MSK                              (((1U<<RF_ADPLL_FCAL_DONE_EXT_LEN)-1)<<RF_ADPLL_FCAL_DONE_EXT_POS)
#define RF_ADPLL_FCAL_DONE_EXT_UMSK                             (~(((1U<<RF_ADPLL_FCAL_DONE_EXT_LEN)-1)<<RF_ADPLL_FCAL_DONE_EXT_POS))
#define RF_ADPLL_FCAL_START_EXT                                 RF_ADPLL_FCAL_START_EXT
#define RF_ADPLL_FCAL_START_EXT_POS                             (9U)
#define RF_ADPLL_FCAL_START_EXT_LEN                             (1U)
#define RF_ADPLL_FCAL_START_EXT_MSK                             (((1U<<RF_ADPLL_FCAL_START_EXT_LEN)-1)<<RF_ADPLL_FCAL_START_EXT_POS)
#define RF_ADPLL_FCAL_START_EXT_UMSK                            (~(((1U<<RF_ADPLL_FCAL_START_EXT_LEN)-1)<<RF_ADPLL_FCAL_START_EXT_POS))
#define RF_ADPLL_LO_LOCK_DIRECTLY                               RF_ADPLL_LO_LOCK_DIRECTLY
#define RF_ADPLL_LO_LOCK_DIRECTLY_POS                           (10U)
#define RF_ADPLL_LO_LOCK_DIRECTLY_LEN                           (1U)
#define RF_ADPLL_LO_LOCK_DIRECTLY_MSK                           (((1U<<RF_ADPLL_LO_LOCK_DIRECTLY_LEN)-1)<<RF_ADPLL_LO_LOCK_DIRECTLY_POS)
#define RF_ADPLL_LO_LOCK_DIRECTLY_UMSK                          (~(((1U<<RF_ADPLL_LO_LOCK_DIRECTLY_LEN)-1)<<RF_ADPLL_LO_LOCK_DIRECTLY_POS))
#define RF_ADPLL_LO_FSM_EXT                                     RF_ADPLL_LO_FSM_EXT
#define RF_ADPLL_LO_FSM_EXT_POS                                 (11U)
#define RF_ADPLL_LO_FSM_EXT_LEN                                 (1U)
#define RF_ADPLL_LO_FSM_EXT_MSK                                 (((1U<<RF_ADPLL_LO_FSM_EXT_LEN)-1)<<RF_ADPLL_LO_FSM_EXT_POS)
#define RF_ADPLL_LO_FSM_EXT_UMSK                                (~(((1U<<RF_ADPLL_LO_FSM_EXT_LEN)-1)<<RF_ADPLL_LO_FSM_EXT_POS))
#define RF_ADPLL_FSM_EN                                         RF_ADPLL_FSM_EN
#define RF_ADPLL_FSM_EN_POS                                     (12U)
#define RF_ADPLL_FSM_EN_LEN                                     (1U)
#define RF_ADPLL_FSM_EN_MSK                                     (((1U<<RF_ADPLL_FSM_EN_LEN)-1)<<RF_ADPLL_FSM_EN_POS)
#define RF_ADPLL_FSM_EN_UMSK                                    (~(((1U<<RF_ADPLL_FSM_EN_LEN)-1)<<RF_ADPLL_FSM_EN_POS))
#define RF_ADPLL_LOCK_FAIL_EN                                   RF_ADPLL_LOCK_FAIL_EN
#define RF_ADPLL_LOCK_FAIL_EN_POS                               (14U)
#define RF_ADPLL_LOCK_FAIL_EN_LEN                               (1U)
#define RF_ADPLL_LOCK_FAIL_EN_MSK                               (((1U<<RF_ADPLL_LOCK_FAIL_EN_LEN)-1)<<RF_ADPLL_LOCK_FAIL_EN_POS)
#define RF_ADPLL_LOCK_FAIL_EN_UMSK                              (~(((1U<<RF_ADPLL_LOCK_FAIL_EN_LEN)-1)<<RF_ADPLL_LOCK_FAIL_EN_POS))
#define RF_ADPLL_ABNORMAL_DEALED                                RF_ADPLL_ABNORMAL_DEALED
#define RF_ADPLL_ABNORMAL_DEALED_POS                            (15U)
#define RF_ADPLL_ABNORMAL_DEALED_LEN                            (1U)
#define RF_ADPLL_ABNORMAL_DEALED_MSK                            (((1U<<RF_ADPLL_ABNORMAL_DEALED_LEN)-1)<<RF_ADPLL_ABNORMAL_DEALED_POS)
#define RF_ADPLL_ABNORMAL_DEALED_UMSK                           (~(((1U<<RF_ADPLL_ABNORMAL_DEALED_LEN)-1)<<RF_ADPLL_ABNORMAL_DEALED_POS))
#define RF_ADPLL_VCTRL_DET_START_EXT                            RF_ADPLL_VCTRL_DET_START_EXT
#define RF_ADPLL_VCTRL_DET_START_EXT_POS                        (18U)
#define RF_ADPLL_VCTRL_DET_START_EXT_LEN                        (1U)
#define RF_ADPLL_VCTRL_DET_START_EXT_MSK                        (((1U<<RF_ADPLL_VCTRL_DET_START_EXT_LEN)-1)<<RF_ADPLL_VCTRL_DET_START_EXT_POS)
#define RF_ADPLL_VCTRL_DET_START_EXT_UMSK                       (~(((1U<<RF_ADPLL_VCTRL_DET_START_EXT_LEN)-1)<<RF_ADPLL_VCTRL_DET_START_EXT_POS))
#define RF_ADPLL_VCTRL_DET_EN_EXT                               RF_ADPLL_VCTRL_DET_EN_EXT
#define RF_ADPLL_VCTRL_DET_EN_EXT_POS                           (19U)
#define RF_ADPLL_VCTRL_DET_EN_EXT_LEN                           (1U)
#define RF_ADPLL_VCTRL_DET_EN_EXT_MSK                           (((1U<<RF_ADPLL_VCTRL_DET_EN_EXT_LEN)-1)<<RF_ADPLL_VCTRL_DET_EN_EXT_POS)
#define RF_ADPLL_VCTRL_DET_EN_EXT_UMSK                          (~(((1U<<RF_ADPLL_VCTRL_DET_EN_EXT_LEN)-1)<<RF_ADPLL_VCTRL_DET_EN_EXT_POS))
#define RF_ADPLL_MOM_UPDATE_EN_EXT                              RF_ADPLL_MOM_UPDATE_EN_EXT
#define RF_ADPLL_MOM_UPDATE_EN_EXT_POS                          (20U)
#define RF_ADPLL_MOM_UPDATE_EN_EXT_LEN                          (1U)
#define RF_ADPLL_MOM_UPDATE_EN_EXT_MSK                          (((1U<<RF_ADPLL_MOM_UPDATE_EN_EXT_LEN)-1)<<RF_ADPLL_MOM_UPDATE_EN_EXT_POS)
#define RF_ADPLL_MOM_UPDATE_EN_EXT_UMSK                         (~(((1U<<RF_ADPLL_MOM_UPDATE_EN_EXT_LEN)-1)<<RF_ADPLL_MOM_UPDATE_EN_EXT_POS))
#define RF_ADPLL_FREQERR_DET_START_EXT                          RF_ADPLL_FREQERR_DET_START_EXT
#define RF_ADPLL_FREQERR_DET_START_EXT_POS                      (21U)
#define RF_ADPLL_FREQERR_DET_START_EXT_LEN                      (1U)
#define RF_ADPLL_FREQERR_DET_START_EXT_MSK                      (((1U<<RF_ADPLL_FREQERR_DET_START_EXT_LEN)-1)<<RF_ADPLL_FREQERR_DET_START_EXT_POS)
#define RF_ADPLL_FREQERR_DET_START_EXT_UMSK                     (~(((1U<<RF_ADPLL_FREQERR_DET_START_EXT_LEN)-1)<<RF_ADPLL_FREQERR_DET_START_EXT_POS))
#define RF_ADPLL_MOM_SEARCH_EN_EXT                              RF_ADPLL_MOM_SEARCH_EN_EXT
#define RF_ADPLL_MOM_SEARCH_EN_EXT_POS                          (22U)
#define RF_ADPLL_MOM_SEARCH_EN_EXT_LEN                          (1U)
#define RF_ADPLL_MOM_SEARCH_EN_EXT_MSK                          (((1U<<RF_ADPLL_MOM_SEARCH_EN_EXT_LEN)-1)<<RF_ADPLL_MOM_SEARCH_EN_EXT_POS)
#define RF_ADPLL_MOM_SEARCH_EN_EXT_UMSK                         (~(((1U<<RF_ADPLL_MOM_SEARCH_EN_EXT_LEN)-1)<<RF_ADPLL_MOM_SEARCH_EN_EXT_POS))
#define RF_ADPLL_LO_OPEN                                        RF_ADPLL_LO_OPEN
#define RF_ADPLL_LO_OPEN_POS                                    (23U)
#define RF_ADPLL_LO_OPEN_LEN                                    (1U)
#define RF_ADPLL_LO_OPEN_MSK                                    (((1U<<RF_ADPLL_LO_OPEN_LEN)-1)<<RF_ADPLL_LO_OPEN_POS)
#define RF_ADPLL_LO_OPEN_UMSK                                   (~(((1U<<RF_ADPLL_LO_OPEN_LEN)-1)<<RF_ADPLL_LO_OPEN_POS))
#define RF_ADPLL_SFREG_SEL                                      RF_ADPLL_SFREG_SEL
#define RF_ADPLL_SFREG_SEL_POS                                  (24U)
#define RF_ADPLL_SFREG_SEL_LEN                                  (1U)
#define RF_ADPLL_SFREG_SEL_MSK                                  (((1U<<RF_ADPLL_SFREG_SEL_LEN)-1)<<RF_ADPLL_SFREG_SEL_POS)
#define RF_ADPLL_SFREG_SEL_UMSK                                 (~(((1U<<RF_ADPLL_SFREG_SEL_LEN)-1)<<RF_ADPLL_SFREG_SEL_POS))
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL                     RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_POS                 (25U)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_LEN                 (1U)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_MSK                 (((1U<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_LEN)-1)<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_POS)
#define RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_UMSK                (~(((1U<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_LEN)-1)<<RF_ADPLL_LO_UNLOCK_INTRPT_CLEAR_SEL_POS))
#define RF_ADPLL_FORCE_INC_FCAL_EN                              RF_ADPLL_FORCE_INC_FCAL_EN
#define RF_ADPLL_FORCE_INC_FCAL_EN_POS                          (26U)
#define RF_ADPLL_FORCE_INC_FCAL_EN_LEN                          (1U)
#define RF_ADPLL_FORCE_INC_FCAL_EN_MSK                          (((1U<<RF_ADPLL_FORCE_INC_FCAL_EN_LEN)-1)<<RF_ADPLL_FORCE_INC_FCAL_EN_POS)
#define RF_ADPLL_FORCE_INC_FCAL_EN_UMSK                         (~(((1U<<RF_ADPLL_FORCE_INC_FCAL_EN_LEN)-1)<<RF_ADPLL_FORCE_INC_FCAL_EN_POS))

/* 0x26C : adpll_lf_reg */
#define RF_ADPLL_LF_REG_OFFSET                                  (0x26C)
#define RF_ADPLL_LF_VCTRL_RANGE_EXT                             RF_ADPLL_LF_VCTRL_RANGE_EXT
#define RF_ADPLL_LF_VCTRL_RANGE_EXT_POS                         (0U)
#define RF_ADPLL_LF_VCTRL_RANGE_EXT_LEN                         (2U)
#define RF_ADPLL_LF_VCTRL_RANGE_EXT_MSK                         (((1U<<RF_ADPLL_LF_VCTRL_RANGE_EXT_LEN)-1)<<RF_ADPLL_LF_VCTRL_RANGE_EXT_POS)
#define RF_ADPLL_LF_VCTRL_RANGE_EXT_UMSK                        (~(((1U<<RF_ADPLL_LF_VCTRL_RANGE_EXT_LEN)-1)<<RF_ADPLL_LF_VCTRL_RANGE_EXT_POS))
#define RF_ADPLL_LF_LSB_EXT                                     RF_ADPLL_LF_LSB_EXT
#define RF_ADPLL_LF_LSB_EXT_POS                                 (2U)
#define RF_ADPLL_LF_LSB_EXT_LEN                                 (7U)
#define RF_ADPLL_LF_LSB_EXT_MSK                                 (((1U<<RF_ADPLL_LF_LSB_EXT_LEN)-1)<<RF_ADPLL_LF_LSB_EXT_POS)
#define RF_ADPLL_LF_LSB_EXT_UMSK                                (~(((1U<<RF_ADPLL_LF_LSB_EXT_LEN)-1)<<RF_ADPLL_LF_LSB_EXT_POS))
#define RF_ADPLL_LF_AVG_EN                                      RF_ADPLL_LF_AVG_EN
#define RF_ADPLL_LF_AVG_EN_POS                                  (9U)
#define RF_ADPLL_LF_AVG_EN_LEN                                  (1U)
#define RF_ADPLL_LF_AVG_EN_MSK                                  (((1U<<RF_ADPLL_LF_AVG_EN_LEN)-1)<<RF_ADPLL_LF_AVG_EN_POS)
#define RF_ADPLL_LF_AVG_EN_UMSK                                 (~(((1U<<RF_ADPLL_LF_AVG_EN_LEN)-1)<<RF_ADPLL_LF_AVG_EN_POS))
#define RF_ADPLL_LF_F_P3                                        RF_ADPLL_LF_F_P3
#define RF_ADPLL_LF_F_P3_POS                                    (10U)
#define RF_ADPLL_LF_F_P3_LEN                                    (2U)
#define RF_ADPLL_LF_F_P3_MSK                                    (((1U<<RF_ADPLL_LF_F_P3_LEN)-1)<<RF_ADPLL_LF_F_P3_POS)
#define RF_ADPLL_LF_F_P3_UMSK                                   (~(((1U<<RF_ADPLL_LF_F_P3_LEN)-1)<<RF_ADPLL_LF_F_P3_POS))
#define RF_ADPLL_LF_BETA_FAST                                   RF_ADPLL_LF_BETA_FAST
#define RF_ADPLL_LF_BETA_FAST_POS                               (13U)
#define RF_ADPLL_LF_BETA_FAST_LEN                               (1U)
#define RF_ADPLL_LF_BETA_FAST_MSK                               (((1U<<RF_ADPLL_LF_BETA_FAST_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_POS)
#define RF_ADPLL_LF_BETA_FAST_UMSK                              (~(((1U<<RF_ADPLL_LF_BETA_FAST_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_POS))
#define RF_ADPLL_LF_BETA_EXP                                    RF_ADPLL_LF_BETA_EXP
#define RF_ADPLL_LF_BETA_EXP_POS                                (14U)
#define RF_ADPLL_LF_BETA_EXP_LEN                                (3U)
#define RF_ADPLL_LF_BETA_EXP_MSK                                (((1U<<RF_ADPLL_LF_BETA_EXP_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_POS)
#define RF_ADPLL_LF_BETA_EXP_UMSK                               (~(((1U<<RF_ADPLL_LF_BETA_EXP_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_POS))
#define RF_ADPLL_LF_BETA_BASE                                   RF_ADPLL_LF_BETA_BASE
#define RF_ADPLL_LF_BETA_BASE_POS                               (17U)
#define RF_ADPLL_LF_BETA_BASE_LEN                               (2U)
#define RF_ADPLL_LF_BETA_BASE_MSK                               (((1U<<RF_ADPLL_LF_BETA_BASE_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_POS)
#define RF_ADPLL_LF_BETA_BASE_UMSK                              (~(((1U<<RF_ADPLL_LF_BETA_BASE_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_POS))
#define RF_ADPLL_LF_ALPHA_FAST                                  RF_ADPLL_LF_ALPHA_FAST
#define RF_ADPLL_LF_ALPHA_FAST_POS                              (20U)
#define RF_ADPLL_LF_ALPHA_FAST_LEN                              (2U)
#define RF_ADPLL_LF_ALPHA_FAST_MSK                              (((1U<<RF_ADPLL_LF_ALPHA_FAST_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_POS)
#define RF_ADPLL_LF_ALPHA_FAST_UMSK                             (~(((1U<<RF_ADPLL_LF_ALPHA_FAST_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_POS))
#define RF_ADPLL_LF_ALPHA_EXP                                   RF_ADPLL_LF_ALPHA_EXP
#define RF_ADPLL_LF_ALPHA_EXP_POS                               (24U)
#define RF_ADPLL_LF_ALPHA_EXP_LEN                               (3U)
#define RF_ADPLL_LF_ALPHA_EXP_MSK                               (((1U<<RF_ADPLL_LF_ALPHA_EXP_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_POS)
#define RF_ADPLL_LF_ALPHA_EXP_UMSK                              (~(((1U<<RF_ADPLL_LF_ALPHA_EXP_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_POS))
#define RF_ADPLL_LF_ALPHA_BASE                                  RF_ADPLL_LF_ALPHA_BASE
#define RF_ADPLL_LF_ALPHA_BASE_POS                              (27U)
#define RF_ADPLL_LF_ALPHA_BASE_LEN                              (1U)
#define RF_ADPLL_LF_ALPHA_BASE_MSK                              (((1U<<RF_ADPLL_LF_ALPHA_BASE_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_POS)
#define RF_ADPLL_LF_ALPHA_BASE_UMSK                             (~(((1U<<RF_ADPLL_LF_ALPHA_BASE_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_POS))
#define RF_ADPLL_LF_CTRL_HW                                     RF_ADPLL_LF_CTRL_HW
#define RF_ADPLL_LF_CTRL_HW_POS                                 (28U)
#define RF_ADPLL_LF_CTRL_HW_LEN                                 (1U)
#define RF_ADPLL_LF_CTRL_HW_MSK                                 (((1U<<RF_ADPLL_LF_CTRL_HW_LEN)-1)<<RF_ADPLL_LF_CTRL_HW_POS)
#define RF_ADPLL_LF_CTRL_HW_UMSK                                (~(((1U<<RF_ADPLL_LF_CTRL_HW_LEN)-1)<<RF_ADPLL_LF_CTRL_HW_POS))

/* 0x270 : adpll_lf_tx */
#define RF_ADPLL_LF_TX_OFFSET                                   (0x270)
#define RF_ADPLL_LF_F_P3_TX                                     RF_ADPLL_LF_F_P3_TX
#define RF_ADPLL_LF_F_P3_TX_POS                                 (10U)
#define RF_ADPLL_LF_F_P3_TX_LEN                                 (2U)
#define RF_ADPLL_LF_F_P3_TX_MSK                                 (((1U<<RF_ADPLL_LF_F_P3_TX_LEN)-1)<<RF_ADPLL_LF_F_P3_TX_POS)
#define RF_ADPLL_LF_F_P3_TX_UMSK                                (~(((1U<<RF_ADPLL_LF_F_P3_TX_LEN)-1)<<RF_ADPLL_LF_F_P3_TX_POS))
#define RF_ADPLL_LF_BETA_FAST_TX                                RF_ADPLL_LF_BETA_FAST_TX
#define RF_ADPLL_LF_BETA_FAST_TX_POS                            (13U)
#define RF_ADPLL_LF_BETA_FAST_TX_LEN                            (1U)
#define RF_ADPLL_LF_BETA_FAST_TX_MSK                            (((1U<<RF_ADPLL_LF_BETA_FAST_TX_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_TX_POS)
#define RF_ADPLL_LF_BETA_FAST_TX_UMSK                           (~(((1U<<RF_ADPLL_LF_BETA_FAST_TX_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_TX_POS))
#define RF_ADPLL_LF_BETA_EXP_TX                                 RF_ADPLL_LF_BETA_EXP_TX
#define RF_ADPLL_LF_BETA_EXP_TX_POS                             (14U)
#define RF_ADPLL_LF_BETA_EXP_TX_LEN                             (3U)
#define RF_ADPLL_LF_BETA_EXP_TX_MSK                             (((1U<<RF_ADPLL_LF_BETA_EXP_TX_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_TX_POS)
#define RF_ADPLL_LF_BETA_EXP_TX_UMSK                            (~(((1U<<RF_ADPLL_LF_BETA_EXP_TX_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_TX_POS))
#define RF_ADPLL_LF_BETA_BASE_TX                                RF_ADPLL_LF_BETA_BASE_TX
#define RF_ADPLL_LF_BETA_BASE_TX_POS                            (17U)
#define RF_ADPLL_LF_BETA_BASE_TX_LEN                            (2U)
#define RF_ADPLL_LF_BETA_BASE_TX_MSK                            (((1U<<RF_ADPLL_LF_BETA_BASE_TX_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_TX_POS)
#define RF_ADPLL_LF_BETA_BASE_TX_UMSK                           (~(((1U<<RF_ADPLL_LF_BETA_BASE_TX_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_TX_POS))
#define RF_ADPLL_LF_ALPHA_FAST_TX                               RF_ADPLL_LF_ALPHA_FAST_TX
#define RF_ADPLL_LF_ALPHA_FAST_TX_POS                           (20U)
#define RF_ADPLL_LF_ALPHA_FAST_TX_LEN                           (2U)
#define RF_ADPLL_LF_ALPHA_FAST_TX_MSK                           (((1U<<RF_ADPLL_LF_ALPHA_FAST_TX_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_TX_POS)
#define RF_ADPLL_LF_ALPHA_FAST_TX_UMSK                          (~(((1U<<RF_ADPLL_LF_ALPHA_FAST_TX_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_TX_POS))
#define RF_ADPLL_LF_ALPHA_EXP_TX                                RF_ADPLL_LF_ALPHA_EXP_TX
#define RF_ADPLL_LF_ALPHA_EXP_TX_POS                            (24U)
#define RF_ADPLL_LF_ALPHA_EXP_TX_LEN                            (3U)
#define RF_ADPLL_LF_ALPHA_EXP_TX_MSK                            (((1U<<RF_ADPLL_LF_ALPHA_EXP_TX_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_TX_POS)
#define RF_ADPLL_LF_ALPHA_EXP_TX_UMSK                           (~(((1U<<RF_ADPLL_LF_ALPHA_EXP_TX_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_TX_POS))
#define RF_ADPLL_LF_ALPHA_BASE_TX                               RF_ADPLL_LF_ALPHA_BASE_TX
#define RF_ADPLL_LF_ALPHA_BASE_TX_POS                           (27U)
#define RF_ADPLL_LF_ALPHA_BASE_TX_LEN                           (1U)
#define RF_ADPLL_LF_ALPHA_BASE_TX_MSK                           (((1U<<RF_ADPLL_LF_ALPHA_BASE_TX_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_TX_POS)
#define RF_ADPLL_LF_ALPHA_BASE_TX_UMSK                          (~(((1U<<RF_ADPLL_LF_ALPHA_BASE_TX_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_TX_POS))

/* 0x274 : adpll_lf_rx */
#define RF_ADPLL_LF_RX_OFFSET                                   (0x274)
#define RF_ADPLL_LF_F_P3_RX                                     RF_ADPLL_LF_F_P3_RX
#define RF_ADPLL_LF_F_P3_RX_POS                                 (10U)
#define RF_ADPLL_LF_F_P3_RX_LEN                                 (2U)
#define RF_ADPLL_LF_F_P3_RX_MSK                                 (((1U<<RF_ADPLL_LF_F_P3_RX_LEN)-1)<<RF_ADPLL_LF_F_P3_RX_POS)
#define RF_ADPLL_LF_F_P3_RX_UMSK                                (~(((1U<<RF_ADPLL_LF_F_P3_RX_LEN)-1)<<RF_ADPLL_LF_F_P3_RX_POS))
#define RF_ADPLL_LF_BETA_FAST_RX                                RF_ADPLL_LF_BETA_FAST_RX
#define RF_ADPLL_LF_BETA_FAST_RX_POS                            (13U)
#define RF_ADPLL_LF_BETA_FAST_RX_LEN                            (1U)
#define RF_ADPLL_LF_BETA_FAST_RX_MSK                            (((1U<<RF_ADPLL_LF_BETA_FAST_RX_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_RX_POS)
#define RF_ADPLL_LF_BETA_FAST_RX_UMSK                           (~(((1U<<RF_ADPLL_LF_BETA_FAST_RX_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_RX_POS))
#define RF_ADPLL_LF_BETA_EXP_RX                                 RF_ADPLL_LF_BETA_EXP_RX
#define RF_ADPLL_LF_BETA_EXP_RX_POS                             (14U)
#define RF_ADPLL_LF_BETA_EXP_RX_LEN                             (3U)
#define RF_ADPLL_LF_BETA_EXP_RX_MSK                             (((1U<<RF_ADPLL_LF_BETA_EXP_RX_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_RX_POS)
#define RF_ADPLL_LF_BETA_EXP_RX_UMSK                            (~(((1U<<RF_ADPLL_LF_BETA_EXP_RX_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_RX_POS))
#define RF_ADPLL_LF_BETA_BASE_RX                                RF_ADPLL_LF_BETA_BASE_RX
#define RF_ADPLL_LF_BETA_BASE_RX_POS                            (17U)
#define RF_ADPLL_LF_BETA_BASE_RX_LEN                            (2U)
#define RF_ADPLL_LF_BETA_BASE_RX_MSK                            (((1U<<RF_ADPLL_LF_BETA_BASE_RX_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_RX_POS)
#define RF_ADPLL_LF_BETA_BASE_RX_UMSK                           (~(((1U<<RF_ADPLL_LF_BETA_BASE_RX_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_RX_POS))
#define RF_ADPLL_LF_ALPHA_FAST_RX                               RF_ADPLL_LF_ALPHA_FAST_RX
#define RF_ADPLL_LF_ALPHA_FAST_RX_POS                           (20U)
#define RF_ADPLL_LF_ALPHA_FAST_RX_LEN                           (2U)
#define RF_ADPLL_LF_ALPHA_FAST_RX_MSK                           (((1U<<RF_ADPLL_LF_ALPHA_FAST_RX_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_RX_POS)
#define RF_ADPLL_LF_ALPHA_FAST_RX_UMSK                          (~(((1U<<RF_ADPLL_LF_ALPHA_FAST_RX_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_RX_POS))
#define RF_ADPLL_LF_ALPHA_EXP_RX                                RF_ADPLL_LF_ALPHA_EXP_RX
#define RF_ADPLL_LF_ALPHA_EXP_RX_POS                            (24U)
#define RF_ADPLL_LF_ALPHA_EXP_RX_LEN                            (3U)
#define RF_ADPLL_LF_ALPHA_EXP_RX_MSK                            (((1U<<RF_ADPLL_LF_ALPHA_EXP_RX_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_RX_POS)
#define RF_ADPLL_LF_ALPHA_EXP_RX_UMSK                           (~(((1U<<RF_ADPLL_LF_ALPHA_EXP_RX_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_RX_POS))
#define RF_ADPLL_LF_ALPHA_BASE_RX                               RF_ADPLL_LF_ALPHA_BASE_RX
#define RF_ADPLL_LF_ALPHA_BASE_RX_POS                           (27U)
#define RF_ADPLL_LF_ALPHA_BASE_RX_LEN                           (1U)
#define RF_ADPLL_LF_ALPHA_BASE_RX_MSK                           (((1U<<RF_ADPLL_LF_ALPHA_BASE_RX_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_RX_POS)
#define RF_ADPLL_LF_ALPHA_BASE_RX_UMSK                          (~(((1U<<RF_ADPLL_LF_ALPHA_BASE_RX_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_RX_POS))

/* 0x278 : adpll_lf_hw */
#define RF_ADPLL_LF_HW_OFFSET                                   (0x278)
#define RF_ADPLL_LF_F_P3_HW                                     RF_ADPLL_LF_F_P3_HW
#define RF_ADPLL_LF_F_P3_HW_POS                                 (10U)
#define RF_ADPLL_LF_F_P3_HW_LEN                                 (2U)
#define RF_ADPLL_LF_F_P3_HW_MSK                                 (((1U<<RF_ADPLL_LF_F_P3_HW_LEN)-1)<<RF_ADPLL_LF_F_P3_HW_POS)
#define RF_ADPLL_LF_F_P3_HW_UMSK                                (~(((1U<<RF_ADPLL_LF_F_P3_HW_LEN)-1)<<RF_ADPLL_LF_F_P3_HW_POS))
#define RF_ADPLL_LF_BETA_FAST_HW                                RF_ADPLL_LF_BETA_FAST_HW
#define RF_ADPLL_LF_BETA_FAST_HW_POS                            (13U)
#define RF_ADPLL_LF_BETA_FAST_HW_LEN                            (1U)
#define RF_ADPLL_LF_BETA_FAST_HW_MSK                            (((1U<<RF_ADPLL_LF_BETA_FAST_HW_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_HW_POS)
#define RF_ADPLL_LF_BETA_FAST_HW_UMSK                           (~(((1U<<RF_ADPLL_LF_BETA_FAST_HW_LEN)-1)<<RF_ADPLL_LF_BETA_FAST_HW_POS))
#define RF_ADPLL_LF_BETA_EXP_HW                                 RF_ADPLL_LF_BETA_EXP_HW
#define RF_ADPLL_LF_BETA_EXP_HW_POS                             (14U)
#define RF_ADPLL_LF_BETA_EXP_HW_LEN                             (3U)
#define RF_ADPLL_LF_BETA_EXP_HW_MSK                             (((1U<<RF_ADPLL_LF_BETA_EXP_HW_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_HW_POS)
#define RF_ADPLL_LF_BETA_EXP_HW_UMSK                            (~(((1U<<RF_ADPLL_LF_BETA_EXP_HW_LEN)-1)<<RF_ADPLL_LF_BETA_EXP_HW_POS))
#define RF_ADPLL_LF_BETA_BASE_HW                                RF_ADPLL_LF_BETA_BASE_HW
#define RF_ADPLL_LF_BETA_BASE_HW_POS                            (17U)
#define RF_ADPLL_LF_BETA_BASE_HW_LEN                            (2U)
#define RF_ADPLL_LF_BETA_BASE_HW_MSK                            (((1U<<RF_ADPLL_LF_BETA_BASE_HW_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_HW_POS)
#define RF_ADPLL_LF_BETA_BASE_HW_UMSK                           (~(((1U<<RF_ADPLL_LF_BETA_BASE_HW_LEN)-1)<<RF_ADPLL_LF_BETA_BASE_HW_POS))
#define RF_ADPLL_LF_ALPHA_FAST_HW                               RF_ADPLL_LF_ALPHA_FAST_HW
#define RF_ADPLL_LF_ALPHA_FAST_HW_POS                           (20U)
#define RF_ADPLL_LF_ALPHA_FAST_HW_LEN                           (2U)
#define RF_ADPLL_LF_ALPHA_FAST_HW_MSK                           (((1U<<RF_ADPLL_LF_ALPHA_FAST_HW_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_HW_POS)
#define RF_ADPLL_LF_ALPHA_FAST_HW_UMSK                          (~(((1U<<RF_ADPLL_LF_ALPHA_FAST_HW_LEN)-1)<<RF_ADPLL_LF_ALPHA_FAST_HW_POS))
#define RF_ADPLL_LF_ALPHA_EXP_HW                                RF_ADPLL_LF_ALPHA_EXP_HW
#define RF_ADPLL_LF_ALPHA_EXP_HW_POS                            (24U)
#define RF_ADPLL_LF_ALPHA_EXP_HW_LEN                            (3U)
#define RF_ADPLL_LF_ALPHA_EXP_HW_MSK                            (((1U<<RF_ADPLL_LF_ALPHA_EXP_HW_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_HW_POS)
#define RF_ADPLL_LF_ALPHA_EXP_HW_UMSK                           (~(((1U<<RF_ADPLL_LF_ALPHA_EXP_HW_LEN)-1)<<RF_ADPLL_LF_ALPHA_EXP_HW_POS))
#define RF_ADPLL_LF_ALPHA_BASE_HW                               RF_ADPLL_LF_ALPHA_BASE_HW
#define RF_ADPLL_LF_ALPHA_BASE_HW_POS                           (27U)
#define RF_ADPLL_LF_ALPHA_BASE_HW_LEN                           (1U)
#define RF_ADPLL_LF_ALPHA_BASE_HW_MSK                           (((1U<<RF_ADPLL_LF_ALPHA_BASE_HW_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_HW_POS)
#define RF_ADPLL_LF_ALPHA_BASE_HW_UMSK                          (~(((1U<<RF_ADPLL_LF_ALPHA_BASE_HW_LEN)-1)<<RF_ADPLL_LF_ALPHA_BASE_HW_POS))

/* 0x27C : adpll_vctrl */
#define RF_ADPLL_VCTRL_OFFSET                                   (0x27C)
#define RF_SDMOUT_DLY_SEL                                       RF_SDMOUT_DLY_SEL
#define RF_SDMOUT_DLY_SEL_POS                                   (0U)
#define RF_SDMOUT_DLY_SEL_LEN                                   (2U)
#define RF_SDMOUT_DLY_SEL_MSK                                   (((1U<<RF_SDMOUT_DLY_SEL_LEN)-1)<<RF_SDMOUT_DLY_SEL_POS)
#define RF_SDMOUT_DLY_SEL_UMSK                                  (~(((1U<<RF_SDMOUT_DLY_SEL_LEN)-1)<<RF_SDMOUT_DLY_SEL_POS))
#define RF_SDM_BYPASS                                           RF_SDM_BYPASS
#define RF_SDM_BYPASS_POS                                       (4U)
#define RF_SDM_BYPASS_LEN                                       (1U)
#define RF_SDM_BYPASS_MSK                                       (((1U<<RF_SDM_BYPASS_LEN)-1)<<RF_SDM_BYPASS_POS)
#define RF_SDM_BYPASS_UMSK                                      (~(((1U<<RF_SDM_BYPASS_LEN)-1)<<RF_SDM_BYPASS_POS))
#define RF_SDM_DITHER                                           RF_SDM_DITHER
#define RF_SDM_DITHER_POS                                       (8U)
#define RF_SDM_DITHER_LEN                                       (2U)
#define RF_SDM_DITHER_MSK                                       (((1U<<RF_SDM_DITHER_LEN)-1)<<RF_SDM_DITHER_POS)
#define RF_SDM_DITHER_UMSK                                      (~(((1U<<RF_SDM_DITHER_LEN)-1)<<RF_SDM_DITHER_POS))
#define RF_SDM_ORDER                                            RF_SDM_ORDER
#define RF_SDM_ORDER_POS                                        (12U)
#define RF_SDM_ORDER_LEN                                        (1U)
#define RF_SDM_ORDER_MSK                                        (((1U<<RF_SDM_ORDER_LEN)-1)<<RF_SDM_ORDER_POS)
#define RF_SDM_ORDER_UMSK                                       (~(((1U<<RF_SDM_ORDER_LEN)-1)<<RF_SDM_ORDER_POS))
#define RF_ADPLL_CAPCODE_BYPASS                                 RF_ADPLL_CAPCODE_BYPASS
#define RF_ADPLL_CAPCODE_BYPASS_POS                             (14U)
#define RF_ADPLL_CAPCODE_BYPASS_LEN                             (1U)
#define RF_ADPLL_CAPCODE_BYPASS_MSK                             (((1U<<RF_ADPLL_CAPCODE_BYPASS_LEN)-1)<<RF_ADPLL_CAPCODE_BYPASS_POS)
#define RF_ADPLL_CAPCODE_BYPASS_UMSK                            (~(((1U<<RF_ADPLL_CAPCODE_BYPASS_LEN)-1)<<RF_ADPLL_CAPCODE_BYPASS_POS))
#define RF_ADPLL_DCO_MASH_BYPASS                                RF_ADPLL_DCO_MASH_BYPASS
#define RF_ADPLL_DCO_MASH_BYPASS_POS                            (15U)
#define RF_ADPLL_DCO_MASH_BYPASS_LEN                            (1U)
#define RF_ADPLL_DCO_MASH_BYPASS_MSK                            (((1U<<RF_ADPLL_DCO_MASH_BYPASS_LEN)-1)<<RF_ADPLL_DCO_MASH_BYPASS_POS)
#define RF_ADPLL_DCO_MASH_BYPASS_UMSK                           (~(((1U<<RF_ADPLL_DCO_MASH_BYPASS_LEN)-1)<<RF_ADPLL_DCO_MASH_BYPASS_POS))
#define RF_ADPLL_FORCE_MOM_HOLD                                 RF_ADPLL_FORCE_MOM_HOLD
#define RF_ADPLL_FORCE_MOM_HOLD_POS                             (16U)
#define RF_ADPLL_FORCE_MOM_HOLD_LEN                             (1U)
#define RF_ADPLL_FORCE_MOM_HOLD_MSK                             (((1U<<RF_ADPLL_FORCE_MOM_HOLD_LEN)-1)<<RF_ADPLL_FORCE_MOM_HOLD_POS)
#define RF_ADPLL_FORCE_MOM_HOLD_UMSK                            (~(((1U<<RF_ADPLL_FORCE_MOM_HOLD_LEN)-1)<<RF_ADPLL_FORCE_MOM_HOLD_POS))
#define RF_ADPLL_MOM_UPDATE_PERIOD                              RF_ADPLL_MOM_UPDATE_PERIOD
#define RF_ADPLL_MOM_UPDATE_PERIOD_POS                          (20U)
#define RF_ADPLL_MOM_UPDATE_PERIOD_LEN                          (2U)
#define RF_ADPLL_MOM_UPDATE_PERIOD_MSK                          (((1U<<RF_ADPLL_MOM_UPDATE_PERIOD_LEN)-1)<<RF_ADPLL_MOM_UPDATE_PERIOD_POS)
#define RF_ADPLL_MOM_UPDATE_PERIOD_UMSK                         (~(((1U<<RF_ADPLL_MOM_UPDATE_PERIOD_LEN)-1)<<RF_ADPLL_MOM_UPDATE_PERIOD_POS))
#define RF_ADPLL_VCTRL_DET_CONS_EN                              RF_ADPLL_VCTRL_DET_CONS_EN
#define RF_ADPLL_VCTRL_DET_CONS_EN_POS                          (24U)
#define RF_ADPLL_VCTRL_DET_CONS_EN_LEN                          (1U)
#define RF_ADPLL_VCTRL_DET_CONS_EN_MSK                          (((1U<<RF_ADPLL_VCTRL_DET_CONS_EN_LEN)-1)<<RF_ADPLL_VCTRL_DET_CONS_EN_POS)
#define RF_ADPLL_VCTRL_DET_CONS_EN_UMSK                         (~(((1U<<RF_ADPLL_VCTRL_DET_CONS_EN_LEN)-1)<<RF_ADPLL_VCTRL_DET_CONS_EN_POS))
#define RF_ADPLL_VCTRL_MONI_WIN_SEL                             RF_ADPLL_VCTRL_MONI_WIN_SEL
#define RF_ADPLL_VCTRL_MONI_WIN_SEL_POS                         (25U)
#define RF_ADPLL_VCTRL_MONI_WIN_SEL_LEN                         (1U)
#define RF_ADPLL_VCTRL_MONI_WIN_SEL_MSK                         (((1U<<RF_ADPLL_VCTRL_MONI_WIN_SEL_LEN)-1)<<RF_ADPLL_VCTRL_MONI_WIN_SEL_POS)
#define RF_ADPLL_VCTRL_MONI_WIN_SEL_UMSK                        (~(((1U<<RF_ADPLL_VCTRL_MONI_WIN_SEL_LEN)-1)<<RF_ADPLL_VCTRL_MONI_WIN_SEL_POS))
#define RF_ADPLL_VCTRL_LOCK_WIN_SEL                             RF_ADPLL_VCTRL_LOCK_WIN_SEL
#define RF_ADPLL_VCTRL_LOCK_WIN_SEL_POS                         (26U)
#define RF_ADPLL_VCTRL_LOCK_WIN_SEL_LEN                         (1U)
#define RF_ADPLL_VCTRL_LOCK_WIN_SEL_MSK                         (((1U<<RF_ADPLL_VCTRL_LOCK_WIN_SEL_LEN)-1)<<RF_ADPLL_VCTRL_LOCK_WIN_SEL_POS)
#define RF_ADPLL_VCTRL_LOCK_WIN_SEL_UMSK                        (~(((1U<<RF_ADPLL_VCTRL_LOCK_WIN_SEL_LEN)-1)<<RF_ADPLL_VCTRL_LOCK_WIN_SEL_POS))
#define RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN                         RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN
#define RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_POS                     (27U)
#define RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_LEN                     (1U)
#define RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_MSK                     (((1U<<RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_LEN)-1)<<RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_POS)
#define RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_UMSK                    (~(((1U<<RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_LEN)-1)<<RF_ADPLL_VCTRL_RANGE_SEL_EXT_EN_POS))

/* 0x280 : adpll_lms */
#define RF_ADPLL_LMS_OFFSET                                     (0x280)
#define RF_ADPLL_PHA_CANCEL_DELAY                               RF_ADPLL_PHA_CANCEL_DELAY
#define RF_ADPLL_PHA_CANCEL_DELAY_POS                           (0U)
#define RF_ADPLL_PHA_CANCEL_DELAY_LEN                           (2U)
#define RF_ADPLL_PHA_CANCEL_DELAY_MSK                           (((1U<<RF_ADPLL_PHA_CANCEL_DELAY_LEN)-1)<<RF_ADPLL_PHA_CANCEL_DELAY_POS)
#define RF_ADPLL_PHA_CANCEL_DELAY_UMSK                          (~(((1U<<RF_ADPLL_PHA_CANCEL_DELAY_LEN)-1)<<RF_ADPLL_PHA_CANCEL_DELAY_POS))
#define RF_ADPLL_PHA_CANCEL_EN                                  RF_ADPLL_PHA_CANCEL_EN
#define RF_ADPLL_PHA_CANCEL_EN_POS                              (4U)
#define RF_ADPLL_PHA_CANCEL_EN_LEN                              (1U)
#define RF_ADPLL_PHA_CANCEL_EN_MSK                              (((1U<<RF_ADPLL_PHA_CANCEL_EN_LEN)-1)<<RF_ADPLL_PHA_CANCEL_EN_POS)
#define RF_ADPLL_PHA_CANCEL_EN_UMSK                             (~(((1U<<RF_ADPLL_PHA_CANCEL_EN_LEN)-1)<<RF_ADPLL_PHA_CANCEL_EN_POS))
#define RF_ADPLL_LMS_Q_DELAY                                    RF_ADPLL_LMS_Q_DELAY
#define RF_ADPLL_LMS_Q_DELAY_POS                                (8U)
#define RF_ADPLL_LMS_Q_DELAY_LEN                                (2U)
#define RF_ADPLL_LMS_Q_DELAY_MSK                                (((1U<<RF_ADPLL_LMS_Q_DELAY_LEN)-1)<<RF_ADPLL_LMS_Q_DELAY_POS)
#define RF_ADPLL_LMS_Q_DELAY_UMSK                               (~(((1U<<RF_ADPLL_LMS_Q_DELAY_LEN)-1)<<RF_ADPLL_LMS_Q_DELAY_POS))
#define RF_ADPLL_PHA_PRBS_SEL                                   RF_ADPLL_PHA_PRBS_SEL
#define RF_ADPLL_PHA_PRBS_SEL_POS                               (10U)
#define RF_ADPLL_PHA_PRBS_SEL_LEN                               (2U)
#define RF_ADPLL_PHA_PRBS_SEL_MSK                               (((1U<<RF_ADPLL_PHA_PRBS_SEL_LEN)-1)<<RF_ADPLL_PHA_PRBS_SEL_POS)
#define RF_ADPLL_PHA_PRBS_SEL_UMSK                              (~(((1U<<RF_ADPLL_PHA_PRBS_SEL_LEN)-1)<<RF_ADPLL_PHA_PRBS_SEL_POS))
#define RF_ADPLL_LMS_STEP_ENLARGE                               RF_ADPLL_LMS_STEP_ENLARGE
#define RF_ADPLL_LMS_STEP_ENLARGE_POS                           (12U)
#define RF_ADPLL_LMS_STEP_ENLARGE_LEN                           (1U)
#define RF_ADPLL_LMS_STEP_ENLARGE_MSK                           (((1U<<RF_ADPLL_LMS_STEP_ENLARGE_LEN)-1)<<RF_ADPLL_LMS_STEP_ENLARGE_POS)
#define RF_ADPLL_LMS_STEP_ENLARGE_UMSK                          (~(((1U<<RF_ADPLL_LMS_STEP_ENLARGE_LEN)-1)<<RF_ADPLL_LMS_STEP_ENLARGE_POS))
#define RF_ADPLL_PHA_DITHER_EN                                  RF_ADPLL_PHA_DITHER_EN
#define RF_ADPLL_PHA_DITHER_EN_POS                              (13U)
#define RF_ADPLL_PHA_DITHER_EN_LEN                              (1U)
#define RF_ADPLL_PHA_DITHER_EN_MSK                              (((1U<<RF_ADPLL_PHA_DITHER_EN_LEN)-1)<<RF_ADPLL_PHA_DITHER_EN_POS)
#define RF_ADPLL_PHA_DITHER_EN_UMSK                             (~(((1U<<RF_ADPLL_PHA_DITHER_EN_LEN)-1)<<RF_ADPLL_PHA_DITHER_EN_POS))
#define RF_ADPLL_PHA_DEM_EN                                     RF_ADPLL_PHA_DEM_EN
#define RF_ADPLL_PHA_DEM_EN_POS                                 (14U)
#define RF_ADPLL_PHA_DEM_EN_LEN                                 (1U)
#define RF_ADPLL_PHA_DEM_EN_MSK                                 (((1U<<RF_ADPLL_PHA_DEM_EN_LEN)-1)<<RF_ADPLL_PHA_DEM_EN_POS)
#define RF_ADPLL_PHA_DEM_EN_UMSK                                (~(((1U<<RF_ADPLL_PHA_DEM_EN_LEN)-1)<<RF_ADPLL_PHA_DEM_EN_POS))
#define RF_ADPLL_SDM_DITHER_PRBS_EN                             RF_ADPLL_SDM_DITHER_PRBS_EN
#define RF_ADPLL_SDM_DITHER_PRBS_EN_POS                         (15U)
#define RF_ADPLL_SDM_DITHER_PRBS_EN_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_PRBS_EN_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_PRBS_EN_LEN)-1)<<RF_ADPLL_SDM_DITHER_PRBS_EN_POS)
#define RF_ADPLL_SDM_DITHER_PRBS_EN_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_PRBS_EN_LEN)-1)<<RF_ADPLL_SDM_DITHER_PRBS_EN_POS))
#define RF_ADPLL_LMS_STEP                                       RF_ADPLL_LMS_STEP
#define RF_ADPLL_LMS_STEP_POS                                   (16U)
#define RF_ADPLL_LMS_STEP_LEN                                   (2U)
#define RF_ADPLL_LMS_STEP_MSK                                   (((1U<<RF_ADPLL_LMS_STEP_LEN)-1)<<RF_ADPLL_LMS_STEP_POS)
#define RF_ADPLL_LMS_STEP_UMSK                                  (~(((1U<<RF_ADPLL_LMS_STEP_LEN)-1)<<RF_ADPLL_LMS_STEP_POS))
#define RF_ADPLL_SDM_DITHER_EN_CTRL_HW                          RF_ADPLL_SDM_DITHER_EN_CTRL_HW
#define RF_ADPLL_SDM_DITHER_EN_CTRL_HW_POS                      (18U)
#define RF_ADPLL_SDM_DITHER_EN_CTRL_HW_LEN                      (1U)
#define RF_ADPLL_SDM_DITHER_EN_CTRL_HW_MSK                      (((1U<<RF_ADPLL_SDM_DITHER_EN_CTRL_HW_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_CTRL_HW_POS)
#define RF_ADPLL_SDM_DITHER_EN_CTRL_HW_UMSK                     (~(((1U<<RF_ADPLL_SDM_DITHER_EN_CTRL_HW_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_CTRL_HW_POS))
#define RF_ADPLL_SDM_DITHER_EN                                  RF_ADPLL_SDM_DITHER_EN
#define RF_ADPLL_SDM_DITHER_EN_POS                              (19U)
#define RF_ADPLL_SDM_DITHER_EN_LEN                              (1U)
#define RF_ADPLL_SDM_DITHER_EN_MSK                              (((1U<<RF_ADPLL_SDM_DITHER_EN_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_POS)
#define RF_ADPLL_SDM_DITHER_EN_UMSK                             (~(((1U<<RF_ADPLL_SDM_DITHER_EN_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_POS))
#define RF_ADPLL_LMS_EXT_VALUE                                  RF_ADPLL_LMS_EXT_VALUE
#define RF_ADPLL_LMS_EXT_VALUE_POS                              (20U)
#define RF_ADPLL_LMS_EXT_VALUE_LEN                              (9U)
#define RF_ADPLL_LMS_EXT_VALUE_MSK                              (((1U<<RF_ADPLL_LMS_EXT_VALUE_LEN)-1)<<RF_ADPLL_LMS_EXT_VALUE_POS)
#define RF_ADPLL_LMS_EXT_VALUE_UMSK                             (~(((1U<<RF_ADPLL_LMS_EXT_VALUE_LEN)-1)<<RF_ADPLL_LMS_EXT_VALUE_POS))
#define RF_ADPLL_LMS_EXT_VALUE_EN                               RF_ADPLL_LMS_EXT_VALUE_EN
#define RF_ADPLL_LMS_EXT_VALUE_EN_POS                           (29U)
#define RF_ADPLL_LMS_EXT_VALUE_EN_LEN                           (1U)
#define RF_ADPLL_LMS_EXT_VALUE_EN_MSK                           (((1U<<RF_ADPLL_LMS_EXT_VALUE_EN_LEN)-1)<<RF_ADPLL_LMS_EXT_VALUE_EN_POS)
#define RF_ADPLL_LMS_EXT_VALUE_EN_UMSK                          (~(((1U<<RF_ADPLL_LMS_EXT_VALUE_EN_LEN)-1)<<RF_ADPLL_LMS_EXT_VALUE_EN_POS))
#define RF_ADPLL_FREF_DIV2_EN                                   RF_ADPLL_FREF_DIV2_EN
#define RF_ADPLL_FREF_DIV2_EN_POS                               (31U)
#define RF_ADPLL_FREF_DIV2_EN_LEN                               (1U)
#define RF_ADPLL_FREF_DIV2_EN_MSK                               (((1U<<RF_ADPLL_FREF_DIV2_EN_LEN)-1)<<RF_ADPLL_FREF_DIV2_EN_POS)
#define RF_ADPLL_FREF_DIV2_EN_UMSK                              (~(((1U<<RF_ADPLL_FREF_DIV2_EN_LEN)-1)<<RF_ADPLL_FREF_DIV2_EN_POS))

/* 0x284 : adpll_spd */
#define RF_ADPLL_SPD_OFFSET                                     (0x284)
#define RF_ADPLL_SPD_IN_RANGE_DELAY                             RF_ADPLL_SPD_IN_RANGE_DELAY
#define RF_ADPLL_SPD_IN_RANGE_DELAY_POS                         (0U)
#define RF_ADPLL_SPD_IN_RANGE_DELAY_LEN                         (2U)
#define RF_ADPLL_SPD_IN_RANGE_DELAY_MSK                         (((1U<<RF_ADPLL_SPD_IN_RANGE_DELAY_LEN)-1)<<RF_ADPLL_SPD_IN_RANGE_DELAY_POS)
#define RF_ADPLL_SPD_IN_RANGE_DELAY_UMSK                        (~(((1U<<RF_ADPLL_SPD_IN_RANGE_DELAY_LEN)-1)<<RF_ADPLL_SPD_IN_RANGE_DELAY_POS))
#define RF_ADPLL_SPD_OUT_RANGE_DELAY                            RF_ADPLL_SPD_OUT_RANGE_DELAY
#define RF_ADPLL_SPD_OUT_RANGE_DELAY_POS                        (4U)
#define RF_ADPLL_SPD_OUT_RANGE_DELAY_LEN                        (1U)
#define RF_ADPLL_SPD_OUT_RANGE_DELAY_MSK                        (((1U<<RF_ADPLL_SPD_OUT_RANGE_DELAY_LEN)-1)<<RF_ADPLL_SPD_OUT_RANGE_DELAY_POS)
#define RF_ADPLL_SPD_OUT_RANGE_DELAY_UMSK                       (~(((1U<<RF_ADPLL_SPD_OUT_RANGE_DELAY_LEN)-1)<<RF_ADPLL_SPD_OUT_RANGE_DELAY_POS))
#define RF_ADPLL_SPD_IN_RANGE_CONS                              RF_ADPLL_SPD_IN_RANGE_CONS
#define RF_ADPLL_SPD_IN_RANGE_CONS_POS                          (8U)
#define RF_ADPLL_SPD_IN_RANGE_CONS_LEN                          (2U)
#define RF_ADPLL_SPD_IN_RANGE_CONS_MSK                          (((1U<<RF_ADPLL_SPD_IN_RANGE_CONS_LEN)-1)<<RF_ADPLL_SPD_IN_RANGE_CONS_POS)
#define RF_ADPLL_SPD_IN_RANGE_CONS_UMSK                         (~(((1U<<RF_ADPLL_SPD_IN_RANGE_CONS_LEN)-1)<<RF_ADPLL_SPD_IN_RANGE_CONS_POS))
#define RF_ADPLL_COARSE_PATH_TURNOFF                            RF_ADPLL_COARSE_PATH_TURNOFF
#define RF_ADPLL_COARSE_PATH_TURNOFF_POS                        (10U)
#define RF_ADPLL_COARSE_PATH_TURNOFF_LEN                        (2U)
#define RF_ADPLL_COARSE_PATH_TURNOFF_MSK                        (((1U<<RF_ADPLL_COARSE_PATH_TURNOFF_LEN)-1)<<RF_ADPLL_COARSE_PATH_TURNOFF_POS)
#define RF_ADPLL_COARSE_PATH_TURNOFF_UMSK                       (~(((1U<<RF_ADPLL_COARSE_PATH_TURNOFF_LEN)-1)<<RF_ADPLL_COARSE_PATH_TURNOFF_POS))
#define RF_ADPLL_SPD_THRESHOLD                                  RF_ADPLL_SPD_THRESHOLD
#define RF_ADPLL_SPD_THRESHOLD_POS                              (12U)
#define RF_ADPLL_SPD_THRESHOLD_LEN                              (2U)
#define RF_ADPLL_SPD_THRESHOLD_MSK                              (((1U<<RF_ADPLL_SPD_THRESHOLD_LEN)-1)<<RF_ADPLL_SPD_THRESHOLD_POS)
#define RF_ADPLL_SPD_THRESHOLD_UMSK                             (~(((1U<<RF_ADPLL_SPD_THRESHOLD_LEN)-1)<<RF_ADPLL_SPD_THRESHOLD_POS))
#define RF_ADPLL_COARSE_IN_RANGE_CONS                           RF_ADPLL_COARSE_IN_RANGE_CONS
#define RF_ADPLL_COARSE_IN_RANGE_CONS_POS                       (14U)
#define RF_ADPLL_COARSE_IN_RANGE_CONS_LEN                       (2U)
#define RF_ADPLL_COARSE_IN_RANGE_CONS_MSK                       (((1U<<RF_ADPLL_COARSE_IN_RANGE_CONS_LEN)-1)<<RF_ADPLL_COARSE_IN_RANGE_CONS_POS)
#define RF_ADPLL_COARSE_IN_RANGE_CONS_UMSK                      (~(((1U<<RF_ADPLL_COARSE_IN_RANGE_CONS_LEN)-1)<<RF_ADPLL_COARSE_IN_RANGE_CONS_POS))
#define RF_ADPLL_SPD_GAIN                                       RF_ADPLL_SPD_GAIN
#define RF_ADPLL_SPD_GAIN_POS                                   (16U)
#define RF_ADPLL_SPD_GAIN_LEN                                   (2U)
#define RF_ADPLL_SPD_GAIN_MSK                                   (((1U<<RF_ADPLL_SPD_GAIN_LEN)-1)<<RF_ADPLL_SPD_GAIN_POS)
#define RF_ADPLL_SPD_GAIN_UMSK                                  (~(((1U<<RF_ADPLL_SPD_GAIN_LEN)-1)<<RF_ADPLL_SPD_GAIN_POS))
#define RF_ADPLL_COARSE_GAIN                                    RF_ADPLL_COARSE_GAIN
#define RF_ADPLL_COARSE_GAIN_POS                                (18U)
#define RF_ADPLL_COARSE_GAIN_LEN                                (2U)
#define RF_ADPLL_COARSE_GAIN_MSK                                (((1U<<RF_ADPLL_COARSE_GAIN_LEN)-1)<<RF_ADPLL_COARSE_GAIN_POS)
#define RF_ADPLL_COARSE_GAIN_UMSK                               (~(((1U<<RF_ADPLL_COARSE_GAIN_LEN)-1)<<RF_ADPLL_COARSE_GAIN_POS))
#define RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW                     RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW
#define RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_POS                 (20U)
#define RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_LEN                 (1U)
#define RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_MSK                 (((1U<<RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_LEN)-1)<<RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_POS)
#define RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_UMSK                (~(((1U<<RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_LEN)-1)<<RF_ADPLL_FORCE_LF_FAST_MODE_CTRL_HW_POS))
#define RF_ADPLL_FORCE_LF_FAST_MODE                             RF_ADPLL_FORCE_LF_FAST_MODE
#define RF_ADPLL_FORCE_LF_FAST_MODE_POS                         (21U)
#define RF_ADPLL_FORCE_LF_FAST_MODE_LEN                         (1U)
#define RF_ADPLL_FORCE_LF_FAST_MODE_MSK                         (((1U<<RF_ADPLL_FORCE_LF_FAST_MODE_LEN)-1)<<RF_ADPLL_FORCE_LF_FAST_MODE_POS)
#define RF_ADPLL_FORCE_LF_FAST_MODE_UMSK                        (~(((1U<<RF_ADPLL_FORCE_LF_FAST_MODE_LEN)-1)<<RF_ADPLL_FORCE_LF_FAST_MODE_POS))
#define RF_ADPLL_FORCE_LF_FAST_MODE_HW                          RF_ADPLL_FORCE_LF_FAST_MODE_HW
#define RF_ADPLL_FORCE_LF_FAST_MODE_HW_POS                      (22U)
#define RF_ADPLL_FORCE_LF_FAST_MODE_HW_LEN                      (1U)
#define RF_ADPLL_FORCE_LF_FAST_MODE_HW_MSK                      (((1U<<RF_ADPLL_FORCE_LF_FAST_MODE_HW_LEN)-1)<<RF_ADPLL_FORCE_LF_FAST_MODE_HW_POS)
#define RF_ADPLL_FORCE_LF_FAST_MODE_HW_UMSK                     (~(((1U<<RF_ADPLL_FORCE_LF_FAST_MODE_HW_LEN)-1)<<RF_ADPLL_FORCE_LF_FAST_MODE_HW_POS))
#define RF_ADPLL_COARSE_PHAERR_EN                               RF_ADPLL_COARSE_PHAERR_EN
#define RF_ADPLL_COARSE_PHAERR_EN_POS                           (23U)
#define RF_ADPLL_COARSE_PHAERR_EN_LEN                           (1U)
#define RF_ADPLL_COARSE_PHAERR_EN_MSK                           (((1U<<RF_ADPLL_COARSE_PHAERR_EN_LEN)-1)<<RF_ADPLL_COARSE_PHAERR_EN_POS)
#define RF_ADPLL_COARSE_PHAERR_EN_UMSK                          (~(((1U<<RF_ADPLL_COARSE_PHAERR_EN_LEN)-1)<<RF_ADPLL_COARSE_PHAERR_EN_POS))
#define RF_ADPLL_COARSE_PATH_OFFTIME_SEL                        RF_ADPLL_COARSE_PATH_OFFTIME_SEL
#define RF_ADPLL_COARSE_PATH_OFFTIME_SEL_POS                    (24U)
#define RF_ADPLL_COARSE_PATH_OFFTIME_SEL_LEN                    (1U)
#define RF_ADPLL_COARSE_PATH_OFFTIME_SEL_MSK                    (((1U<<RF_ADPLL_COARSE_PATH_OFFTIME_SEL_LEN)-1)<<RF_ADPLL_COARSE_PATH_OFFTIME_SEL_POS)
#define RF_ADPLL_COARSE_PATH_OFFTIME_SEL_UMSK                   (~(((1U<<RF_ADPLL_COARSE_PATH_OFFTIME_SEL_LEN)-1)<<RF_ADPLL_COARSE_PATH_OFFTIME_SEL_POS))
#define RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT                       RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT
#define RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_POS                   (25U)
#define RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_LEN                   (2U)
#define RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_MSK                   (((1U<<RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_LEN)-1)<<RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_POS)
#define RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_UMSK                  (~(((1U<<RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_LEN)-1)<<RF_ADPLL_SPD_OUTRANGE_DLY_SEL_EXT_POS))
#define RF_ADPLL_SPD_LMS_SSTP_WIN_SEL                           RF_ADPLL_SPD_LMS_SSTP_WIN_SEL
#define RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_POS                       (27U)
#define RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_LEN                       (1U)
#define RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_MSK                       (((1U<<RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_LEN)-1)<<RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_POS)
#define RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_UMSK                      (~(((1U<<RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_LEN)-1)<<RF_ADPLL_SPD_LMS_SSTP_WIN_SEL_POS))
#define RF_ADPLL_FORCE_COARSE_PATH_ON                           RF_ADPLL_FORCE_COARSE_PATH_ON
#define RF_ADPLL_FORCE_COARSE_PATH_ON_POS                       (28U)
#define RF_ADPLL_FORCE_COARSE_PATH_ON_LEN                       (1U)
#define RF_ADPLL_FORCE_COARSE_PATH_ON_MSK                       (((1U<<RF_ADPLL_FORCE_COARSE_PATH_ON_LEN)-1)<<RF_ADPLL_FORCE_COARSE_PATH_ON_POS)
#define RF_ADPLL_FORCE_COARSE_PATH_ON_UMSK                      (~(((1U<<RF_ADPLL_FORCE_COARSE_PATH_ON_LEN)-1)<<RF_ADPLL_FORCE_COARSE_PATH_ON_POS))
#define RF_ADPLL_COARSEPHA_DLY_SEL                              RF_ADPLL_COARSEPHA_DLY_SEL
#define RF_ADPLL_COARSEPHA_DLY_SEL_POS                          (29U)
#define RF_ADPLL_COARSEPHA_DLY_SEL_LEN                          (1U)
#define RF_ADPLL_COARSEPHA_DLY_SEL_MSK                          (((1U<<RF_ADPLL_COARSEPHA_DLY_SEL_LEN)-1)<<RF_ADPLL_COARSEPHA_DLY_SEL_POS)
#define RF_ADPLL_COARSEPHA_DLY_SEL_UMSK                         (~(((1U<<RF_ADPLL_COARSEPHA_DLY_SEL_LEN)-1)<<RF_ADPLL_COARSEPHA_DLY_SEL_POS))
#define RF_ADPLL_SPD_IN_RANGE_DELAY_1                           RF_ADPLL_SPD_IN_RANGE_DELAY_1
#define RF_ADPLL_SPD_IN_RANGE_DELAY_1_POS                       (30U)
#define RF_ADPLL_SPD_IN_RANGE_DELAY_1_LEN                       (2U)
#define RF_ADPLL_SPD_IN_RANGE_DELAY_1_MSK                       (((1U<<RF_ADPLL_SPD_IN_RANGE_DELAY_1_LEN)-1)<<RF_ADPLL_SPD_IN_RANGE_DELAY_1_POS)
#define RF_ADPLL_SPD_IN_RANGE_DELAY_1_UMSK                      (~(((1U<<RF_ADPLL_SPD_IN_RANGE_DELAY_1_LEN)-1)<<RF_ADPLL_SPD_IN_RANGE_DELAY_1_POS))

/* 0x288 : fcal */
#define RF_FCAL_OFFSET                                          (0x288)
#define RF_FCAL_DIV_RATIO_ADJ_EN                                RF_FCAL_DIV_RATIO_ADJ_EN
#define RF_FCAL_DIV_RATIO_ADJ_EN_POS                            (0U)
#define RF_FCAL_DIV_RATIO_ADJ_EN_LEN                            (1U)
#define RF_FCAL_DIV_RATIO_ADJ_EN_MSK                            (((1U<<RF_FCAL_DIV_RATIO_ADJ_EN_LEN)-1)<<RF_FCAL_DIV_RATIO_ADJ_EN_POS)
#define RF_FCAL_DIV_RATIO_ADJ_EN_UMSK                           (~(((1U<<RF_FCAL_DIV_RATIO_ADJ_EN_LEN)-1)<<RF_FCAL_DIV_RATIO_ADJ_EN_POS))
#define RF_FCAL_COARSE_PHA_THRESHOLD                            RF_FCAL_COARSE_PHA_THRESHOLD
#define RF_FCAL_COARSE_PHA_THRESHOLD_POS                        (1U)
#define RF_FCAL_COARSE_PHA_THRESHOLD_LEN                        (2U)
#define RF_FCAL_COARSE_PHA_THRESHOLD_MSK                        (((1U<<RF_FCAL_COARSE_PHA_THRESHOLD_LEN)-1)<<RF_FCAL_COARSE_PHA_THRESHOLD_POS)
#define RF_FCAL_COARSE_PHA_THRESHOLD_UMSK                       (~(((1U<<RF_FCAL_COARSE_PHA_THRESHOLD_LEN)-1)<<RF_FCAL_COARSE_PHA_THRESHOLD_POS))
#define RF_FCAL_MOM_TOGGLE_CNT                                  RF_FCAL_MOM_TOGGLE_CNT
#define RF_FCAL_MOM_TOGGLE_CNT_POS                              (3U)
#define RF_FCAL_MOM_TOGGLE_CNT_LEN                              (1U)
#define RF_FCAL_MOM_TOGGLE_CNT_MSK                              (((1U<<RF_FCAL_MOM_TOGGLE_CNT_LEN)-1)<<RF_FCAL_MOM_TOGGLE_CNT_POS)
#define RF_FCAL_MOM_TOGGLE_CNT_UMSK                             (~(((1U<<RF_FCAL_MOM_TOGGLE_CNT_LEN)-1)<<RF_FCAL_MOM_TOGGLE_CNT_POS))
#define RF_FCAL_CLK_PERIOD                                      RF_FCAL_CLK_PERIOD
#define RF_FCAL_CLK_PERIOD_POS                                  (4U)
#define RF_FCAL_CLK_PERIOD_LEN                                  (2U)
#define RF_FCAL_CLK_PERIOD_MSK                                  (((1U<<RF_FCAL_CLK_PERIOD_LEN)-1)<<RF_FCAL_CLK_PERIOD_POS)
#define RF_FCAL_CLK_PERIOD_UMSK                                 (~(((1U<<RF_FCAL_CLK_PERIOD_LEN)-1)<<RF_FCAL_CLK_PERIOD_POS))
#define RF_FCAL_MODE                                            RF_FCAL_MODE
#define RF_FCAL_MODE_POS                                        (6U)
#define RF_FCAL_MODE_LEN                                        (2U)
#define RF_FCAL_MODE_MSK                                        (((1U<<RF_FCAL_MODE_LEN)-1)<<RF_FCAL_MODE_POS)
#define RF_FCAL_MODE_UMSK                                       (~(((1U<<RF_FCAL_MODE_LEN)-1)<<RF_FCAL_MODE_POS))
#define RF_FCAL_MOM_INI_EXT                                     RF_FCAL_MOM_INI_EXT
#define RF_FCAL_MOM_INI_EXT_POS                                 (16U)
#define RF_FCAL_MOM_INI_EXT_LEN                                 (8U)
#define RF_FCAL_MOM_INI_EXT_MSK                                 (((1U<<RF_FCAL_MOM_INI_EXT_LEN)-1)<<RF_FCAL_MOM_INI_EXT_POS)
#define RF_FCAL_MOM_INI_EXT_UMSK                                (~(((1U<<RF_FCAL_MOM_INI_EXT_LEN)-1)<<RF_FCAL_MOM_INI_EXT_POS))

/* 0x28C : adpll_polarity */
#define RF_ADPLL_POLARITY_OFFSET                                (0x28C)
#define RF_ADPLL_LP_MOM_POLARITY                                RF_ADPLL_LP_MOM_POLARITY
#define RF_ADPLL_LP_MOM_POLARITY_POS                            (8U)
#define RF_ADPLL_LP_MOM_POLARITY_LEN                            (1U)
#define RF_ADPLL_LP_MOM_POLARITY_MSK                            (((1U<<RF_ADPLL_LP_MOM_POLARITY_LEN)-1)<<RF_ADPLL_LP_MOM_POLARITY_POS)
#define RF_ADPLL_LP_MOM_POLARITY_UMSK                           (~(((1U<<RF_ADPLL_LP_MOM_POLARITY_LEN)-1)<<RF_ADPLL_LP_MOM_POLARITY_POS))
#define RF_ADPLL_LMS_POLARITY                                   RF_ADPLL_LMS_POLARITY
#define RF_ADPLL_LMS_POLARITY_POS                               (12U)
#define RF_ADPLL_LMS_POLARITY_LEN                               (1U)
#define RF_ADPLL_LMS_POLARITY_MSK                               (((1U<<RF_ADPLL_LMS_POLARITY_LEN)-1)<<RF_ADPLL_LMS_POLARITY_POS)
#define RF_ADPLL_LMS_POLARITY_UMSK                              (~(((1U<<RF_ADPLL_LMS_POLARITY_LEN)-1)<<RF_ADPLL_LMS_POLARITY_POS))
#define RF_ADPLL_FCAL_POLARITY                                  RF_ADPLL_FCAL_POLARITY
#define RF_ADPLL_FCAL_POLARITY_POS                              (16U)
#define RF_ADPLL_FCAL_POLARITY_LEN                              (1U)
#define RF_ADPLL_FCAL_POLARITY_MSK                              (((1U<<RF_ADPLL_FCAL_POLARITY_LEN)-1)<<RF_ADPLL_FCAL_POLARITY_POS)
#define RF_ADPLL_FCAL_POLARITY_UMSK                             (~(((1U<<RF_ADPLL_FCAL_POLARITY_LEN)-1)<<RF_ADPLL_FCAL_POLARITY_POS))
#define RF_ADPLL_LP_POLARITY                                    RF_ADPLL_LP_POLARITY
#define RF_ADPLL_LP_POLARITY_POS                                (20U)
#define RF_ADPLL_LP_POLARITY_LEN                                (1U)
#define RF_ADPLL_LP_POLARITY_MSK                                (((1U<<RF_ADPLL_LP_POLARITY_LEN)-1)<<RF_ADPLL_LP_POLARITY_POS)
#define RF_ADPLL_LP_POLARITY_UMSK                               (~(((1U<<RF_ADPLL_LP_POLARITY_LEN)-1)<<RF_ADPLL_LP_POLARITY_POS))

/* 0x290 : adpll_output */
#define RF_ADPLL_OUTPUT_OFFSET                                  (0x290)
#define RF_ADPLL_UNLOCK_INTRPT                                  RF_ADPLL_UNLOCK_INTRPT
#define RF_ADPLL_UNLOCK_INTRPT_POS                              (0U)
#define RF_ADPLL_UNLOCK_INTRPT_LEN                              (1U)
#define RF_ADPLL_UNLOCK_INTRPT_MSK                              (((1U<<RF_ADPLL_UNLOCK_INTRPT_LEN)-1)<<RF_ADPLL_UNLOCK_INTRPT_POS)
#define RF_ADPLL_UNLOCK_INTRPT_UMSK                             (~(((1U<<RF_ADPLL_UNLOCK_INTRPT_LEN)-1)<<RF_ADPLL_UNLOCK_INTRPT_POS))
#define RF_ADPLL_LO_LOCK                                        RF_ADPLL_LO_LOCK
#define RF_ADPLL_LO_LOCK_POS                                    (1U)
#define RF_ADPLL_LO_LOCK_LEN                                    (1U)
#define RF_ADPLL_LO_LOCK_MSK                                    (((1U<<RF_ADPLL_LO_LOCK_LEN)-1)<<RF_ADPLL_LO_LOCK_POS)
#define RF_ADPLL_LO_LOCK_UMSK                                   (~(((1U<<RF_ADPLL_LO_LOCK_LEN)-1)<<RF_ADPLL_LO_LOCK_POS))
#define RF_ADPLL_FSM_STATE                                      RF_ADPLL_FSM_STATE
#define RF_ADPLL_FSM_STATE_POS                                  (3U)
#define RF_ADPLL_FSM_STATE_LEN                                  (4U)
#define RF_ADPLL_FSM_STATE_MSK                                  (((1U<<RF_ADPLL_FSM_STATE_LEN)-1)<<RF_ADPLL_FSM_STATE_POS)
#define RF_ADPLL_FSM_STATE_UMSK                                 (~(((1U<<RF_ADPLL_FSM_STATE_LEN)-1)<<RF_ADPLL_FSM_STATE_POS))
#define RF_ADPLL_SPD_UNLOCK_SIGN                                RF_ADPLL_SPD_UNLOCK_SIGN
#define RF_ADPLL_SPD_UNLOCK_SIGN_POS                            (7U)
#define RF_ADPLL_SPD_UNLOCK_SIGN_LEN                            (1U)
#define RF_ADPLL_SPD_UNLOCK_SIGN_MSK                            (((1U<<RF_ADPLL_SPD_UNLOCK_SIGN_LEN)-1)<<RF_ADPLL_SPD_UNLOCK_SIGN_POS)
#define RF_ADPLL_SPD_UNLOCK_SIGN_UMSK                           (~(((1U<<RF_ADPLL_SPD_UNLOCK_SIGN_LEN)-1)<<RF_ADPLL_SPD_UNLOCK_SIGN_POS))
#define RF_ADPLL_VCTRL_OUT_RANGE_FSM                            RF_ADPLL_VCTRL_OUT_RANGE_FSM
#define RF_ADPLL_VCTRL_OUT_RANGE_FSM_POS                        (8U)
#define RF_ADPLL_VCTRL_OUT_RANGE_FSM_LEN                        (1U)
#define RF_ADPLL_VCTRL_OUT_RANGE_FSM_MSK                        (((1U<<RF_ADPLL_VCTRL_OUT_RANGE_FSM_LEN)-1)<<RF_ADPLL_VCTRL_OUT_RANGE_FSM_POS)
#define RF_ADPLL_VCTRL_OUT_RANGE_FSM_UMSK                       (~(((1U<<RF_ADPLL_VCTRL_OUT_RANGE_FSM_LEN)-1)<<RF_ADPLL_VCTRL_OUT_RANGE_FSM_POS))
#define RF_ADPLL_MOM_UPDATE_FAIL_FSM                            RF_ADPLL_MOM_UPDATE_FAIL_FSM
#define RF_ADPLL_MOM_UPDATE_FAIL_FSM_POS                        (9U)
#define RF_ADPLL_MOM_UPDATE_FAIL_FSM_LEN                        (1U)
#define RF_ADPLL_MOM_UPDATE_FAIL_FSM_MSK                        (((1U<<RF_ADPLL_MOM_UPDATE_FAIL_FSM_LEN)-1)<<RF_ADPLL_MOM_UPDATE_FAIL_FSM_POS)
#define RF_ADPLL_MOM_UPDATE_FAIL_FSM_UMSK                       (~(((1U<<RF_ADPLL_MOM_UPDATE_FAIL_FSM_LEN)-1)<<RF_ADPLL_MOM_UPDATE_FAIL_FSM_POS))
#define RF_ADPLL_MOM_UPDATE_OU_FSM                              RF_ADPLL_MOM_UPDATE_OU_FSM
#define RF_ADPLL_MOM_UPDATE_OU_FSM_POS                          (10U)
#define RF_ADPLL_MOM_UPDATE_OU_FSM_LEN                          (1U)
#define RF_ADPLL_MOM_UPDATE_OU_FSM_MSK                          (((1U<<RF_ADPLL_MOM_UPDATE_OU_FSM_LEN)-1)<<RF_ADPLL_MOM_UPDATE_OU_FSM_POS)
#define RF_ADPLL_MOM_UPDATE_OU_FSM_UMSK                         (~(((1U<<RF_ADPLL_MOM_UPDATE_OU_FSM_LEN)-1)<<RF_ADPLL_MOM_UPDATE_OU_FSM_POS))
#define RF_ADPLL_SPD_UNLOCK_FSM                                 RF_ADPLL_SPD_UNLOCK_FSM
#define RF_ADPLL_SPD_UNLOCK_FSM_POS                             (11U)
#define RF_ADPLL_SPD_UNLOCK_FSM_LEN                             (1U)
#define RF_ADPLL_SPD_UNLOCK_FSM_MSK                             (((1U<<RF_ADPLL_SPD_UNLOCK_FSM_LEN)-1)<<RF_ADPLL_SPD_UNLOCK_FSM_POS)
#define RF_ADPLL_SPD_UNLOCK_FSM_UMSK                            (~(((1U<<RF_ADPLL_SPD_UNLOCK_FSM_LEN)-1)<<RF_ADPLL_SPD_UNLOCK_FSM_POS))
#define RF_ADPLL_SPD_LOCK_FSM                                   RF_ADPLL_SPD_LOCK_FSM
#define RF_ADPLL_SPD_LOCK_FSM_POS                               (12U)
#define RF_ADPLL_SPD_LOCK_FSM_LEN                               (1U)
#define RF_ADPLL_SPD_LOCK_FSM_MSK                               (((1U<<RF_ADPLL_SPD_LOCK_FSM_LEN)-1)<<RF_ADPLL_SPD_LOCK_FSM_POS)
#define RF_ADPLL_SPD_LOCK_FSM_UMSK                              (~(((1U<<RF_ADPLL_SPD_LOCK_FSM_LEN)-1)<<RF_ADPLL_SPD_LOCK_FSM_POS))
#define RF_ADPLL_FCAL_DONE_FSM                                  RF_ADPLL_FCAL_DONE_FSM
#define RF_ADPLL_FCAL_DONE_FSM_POS                              (13U)
#define RF_ADPLL_FCAL_DONE_FSM_LEN                              (1U)
#define RF_ADPLL_FCAL_DONE_FSM_MSK                              (((1U<<RF_ADPLL_FCAL_DONE_FSM_LEN)-1)<<RF_ADPLL_FCAL_DONE_FSM_POS)
#define RF_ADPLL_FCAL_DONE_FSM_UMSK                             (~(((1U<<RF_ADPLL_FCAL_DONE_FSM_LEN)-1)<<RF_ADPLL_FCAL_DONE_FSM_POS))
#define RF_ADPLL_CAPCODE_OUT_RANGE                              RF_ADPLL_CAPCODE_OUT_RANGE
#define RF_ADPLL_CAPCODE_OUT_RANGE_POS                          (14U)
#define RF_ADPLL_CAPCODE_OUT_RANGE_LEN                          (1U)
#define RF_ADPLL_CAPCODE_OUT_RANGE_MSK                          (((1U<<RF_ADPLL_CAPCODE_OUT_RANGE_LEN)-1)<<RF_ADPLL_CAPCODE_OUT_RANGE_POS)
#define RF_ADPLL_CAPCODE_OUT_RANGE_UMSK                         (~(((1U<<RF_ADPLL_CAPCODE_OUT_RANGE_LEN)-1)<<RF_ADPLL_CAPCODE_OUT_RANGE_POS))
#define RF_ADPLL_MOM_UPDATE_TOTAL_OU                            RF_ADPLL_MOM_UPDATE_TOTAL_OU
#define RF_ADPLL_MOM_UPDATE_TOTAL_OU_POS                        (15U)
#define RF_ADPLL_MOM_UPDATE_TOTAL_OU_LEN                        (2U)
#define RF_ADPLL_MOM_UPDATE_TOTAL_OU_MSK                        (((1U<<RF_ADPLL_MOM_UPDATE_TOTAL_OU_LEN)-1)<<RF_ADPLL_MOM_UPDATE_TOTAL_OU_POS)
#define RF_ADPLL_MOM_UPDATE_TOTAL_OU_UMSK                       (~(((1U<<RF_ADPLL_MOM_UPDATE_TOTAL_OU_LEN)-1)<<RF_ADPLL_MOM_UPDATE_TOTAL_OU_POS))
#define RF_ADPLL_CAPCODE_UD                                     RF_ADPLL_CAPCODE_UD
#define RF_ADPLL_CAPCODE_UD_POS                                 (17U)
#define RF_ADPLL_CAPCODE_UD_LEN                                 (1U)
#define RF_ADPLL_CAPCODE_UD_MSK                                 (((1U<<RF_ADPLL_CAPCODE_UD_LEN)-1)<<RF_ADPLL_CAPCODE_UD_POS)
#define RF_ADPLL_CAPCODE_UD_UMSK                                (~(((1U<<RF_ADPLL_CAPCODE_UD_LEN)-1)<<RF_ADPLL_CAPCODE_UD_POS))
#define RF_ADPLL_VCTRL_DET_DONE                                 RF_ADPLL_VCTRL_DET_DONE
#define RF_ADPLL_VCTRL_DET_DONE_POS                             (18U)
#define RF_ADPLL_VCTRL_DET_DONE_LEN                             (1U)
#define RF_ADPLL_VCTRL_DET_DONE_MSK                             (((1U<<RF_ADPLL_VCTRL_DET_DONE_LEN)-1)<<RF_ADPLL_VCTRL_DET_DONE_POS)
#define RF_ADPLL_VCTRL_DET_DONE_UMSK                            (~(((1U<<RF_ADPLL_VCTRL_DET_DONE_LEN)-1)<<RF_ADPLL_VCTRL_DET_DONE_POS))
#define RF_ADPLL_FREQERR_SIGN                                   RF_ADPLL_FREQERR_SIGN
#define RF_ADPLL_FREQERR_SIGN_POS                               (19U)
#define RF_ADPLL_FREQERR_SIGN_LEN                               (1U)
#define RF_ADPLL_FREQERR_SIGN_MSK                               (((1U<<RF_ADPLL_FREQERR_SIGN_LEN)-1)<<RF_ADPLL_FREQERR_SIGN_POS)
#define RF_ADPLL_FREQERR_SIGN_UMSK                              (~(((1U<<RF_ADPLL_FREQERR_SIGN_LEN)-1)<<RF_ADPLL_FREQERR_SIGN_POS))
#define RF_ADPLL_FREQERR_OU                                     RF_ADPLL_FREQERR_OU
#define RF_ADPLL_FREQERR_OU_POS                                 (20U)
#define RF_ADPLL_FREQERR_OU_LEN                                 (1U)
#define RF_ADPLL_FREQERR_OU_MSK                                 (((1U<<RF_ADPLL_FREQERR_OU_LEN)-1)<<RF_ADPLL_FREQERR_OU_POS)
#define RF_ADPLL_FREQERR_OU_UMSK                                (~(((1U<<RF_ADPLL_FREQERR_OU_LEN)-1)<<RF_ADPLL_FREQERR_OU_POS))
#define RF_ADPLL_FREQERR_DET_DONE                               RF_ADPLL_FREQERR_DET_DONE
#define RF_ADPLL_FREQERR_DET_DONE_POS                           (21U)
#define RF_ADPLL_FREQERR_DET_DONE_LEN                           (1U)
#define RF_ADPLL_FREQERR_DET_DONE_MSK                           (((1U<<RF_ADPLL_FREQERR_DET_DONE_LEN)-1)<<RF_ADPLL_FREQERR_DET_DONE_POS)
#define RF_ADPLL_FREQERR_DET_DONE_UMSK                          (~(((1U<<RF_ADPLL_FREQERR_DET_DONE_LEN)-1)<<RF_ADPLL_FREQERR_DET_DONE_POS))

/* 0x294 : adpll_reserved */
#define RF_ADPLL_RESERVED_OFFSET                                (0x294)
#define RF_ADPLL_RESV1                                          RF_ADPLL_RESV1
#define RF_ADPLL_RESV1_POS                                      (0U)
#define RF_ADPLL_RESV1_LEN                                      (16U)
#define RF_ADPLL_RESV1_MSK                                      (((1U<<RF_ADPLL_RESV1_LEN)-1)<<RF_ADPLL_RESV1_POS)
#define RF_ADPLL_RESV1_UMSK                                     (~(((1U<<RF_ADPLL_RESV1_LEN)-1)<<RF_ADPLL_RESV1_POS))
#define RF_ADPLL_RESV0                                          RF_ADPLL_RESV0
#define RF_ADPLL_RESV0_POS                                      (16U)
#define RF_ADPLL_RESV0_LEN                                      (16U)
#define RF_ADPLL_RESV0_MSK                                      (((1U<<RF_ADPLL_RESV0_LEN)-1)<<RF_ADPLL_RESV0_POS)
#define RF_ADPLL_RESV0_UMSK                                     (~(((1U<<RF_ADPLL_RESV0_LEN)-1)<<RF_ADPLL_RESV0_POS))

/* 0x298 : rf_reserved */
#define RF_RESERVED_OFFSET                                      (0x298)
#define RF_RESV1                                                RF_RESV1
#define RF_RESV1_POS                                            (0U)
#define RF_RESV1_LEN                                            (16U)
#define RF_RESV1_MSK                                            (((1U<<RF_RESV1_LEN)-1)<<RF_RESV1_POS)
#define RF_RESV1_UMSK                                           (~(((1U<<RF_RESV1_LEN)-1)<<RF_RESV1_POS))
#define RF_RESV0                                                RF_RESV0
#define RF_RESV0_POS                                            (16U)
#define RF_RESV0_LEN                                            (16U)
#define RF_RESV0_MSK                                            (((1U<<RF_RESV0_LEN)-1)<<RF_RESV0_POS)
#define RF_RESV0_UMSK                                           (~(((1U<<RF_RESV0_LEN)-1)<<RF_RESV0_POS))

/* 0x29C : rf_reserved_2 */
#define RF_RESERVED_2_OFFSET                                    (0x29C)
#define RF_RESV                                                 RF_RESV
#define RF_RESV_POS                                             (0U)
#define RF_RESV_LEN                                             (32U)
#define RF_RESV_MSK                                             (((1U<<RF_RESV_LEN)-1)<<RF_RESV_POS)
#define RF_RESV_UMSK                                            (~(((1U<<RF_RESV_LEN)-1)<<RF_RESV_POS))

/* 0x300 : rbb_gain_ctrl0 */
#define RF_RBB_GAIN_CTRL0_OFFSET                                (0x300)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW0                              RF_GAIN_CTRL0_ROSDAC_Q_BW0
#define RF_GAIN_CTRL0_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL0_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL0_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL0_ROSDAC_I_BW0                              RF_GAIN_CTRL0_ROSDAC_I_BW0
#define RF_GAIN_CTRL0_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL0_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL0_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL0_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL0_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL0_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL0_ROSDAC_Q_BW1                              RF_GAIN_CTRL0_ROSDAC_Q_BW1
#define RF_GAIN_CTRL0_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL0_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL0_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL0_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL0_ROSDAC_I_BW1                              RF_GAIN_CTRL0_ROSDAC_I_BW1
#define RF_GAIN_CTRL0_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL0_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL0_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL0_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL0_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL0_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL0_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL0_G_RBB2                                    RF_GAIN_CTRL0_G_RBB2
#define RF_GAIN_CTRL0_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL0_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL0_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL0_G_RBB2_LEN)-1)<<RF_GAIN_CTRL0_G_RBB2_POS)
#define RF_GAIN_CTRL0_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL0_G_RBB2_LEN)-1)<<RF_GAIN_CTRL0_G_RBB2_POS))
#define RF_GAIN_CTRL0_G_RBB1                                    RF_GAIN_CTRL0_G_RBB1
#define RF_GAIN_CTRL0_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL0_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL0_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL0_G_RBB1_LEN)-1)<<RF_GAIN_CTRL0_G_RBB1_POS)
#define RF_GAIN_CTRL0_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL0_G_RBB1_LEN)-1)<<RF_GAIN_CTRL0_G_RBB1_POS))

/* 0x304 : rbb_gain_ctrl1 */
#define RF_RBB_GAIN_CTRL1_OFFSET                                (0x304)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW0                              RF_GAIN_CTRL1_ROSDAC_Q_BW0
#define RF_GAIN_CTRL1_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL1_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL1_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL1_ROSDAC_I_BW0                              RF_GAIN_CTRL1_ROSDAC_I_BW0
#define RF_GAIN_CTRL1_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL1_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL1_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL1_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL1_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL1_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL1_ROSDAC_Q_BW1                              RF_GAIN_CTRL1_ROSDAC_Q_BW1
#define RF_GAIN_CTRL1_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL1_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL1_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL1_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL1_ROSDAC_I_BW1                              RF_GAIN_CTRL1_ROSDAC_I_BW1
#define RF_GAIN_CTRL1_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL1_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL1_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL1_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL1_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL1_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL1_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL1_G_RBB2                                    RF_GAIN_CTRL1_G_RBB2
#define RF_GAIN_CTRL1_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL1_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL1_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL1_G_RBB2_LEN)-1)<<RF_GAIN_CTRL1_G_RBB2_POS)
#define RF_GAIN_CTRL1_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL1_G_RBB2_LEN)-1)<<RF_GAIN_CTRL1_G_RBB2_POS))
#define RF_GAIN_CTRL1_G_RBB1                                    RF_GAIN_CTRL1_G_RBB1
#define RF_GAIN_CTRL1_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL1_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL1_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL1_G_RBB1_LEN)-1)<<RF_GAIN_CTRL1_G_RBB1_POS)
#define RF_GAIN_CTRL1_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL1_G_RBB1_LEN)-1)<<RF_GAIN_CTRL1_G_RBB1_POS))

/* 0x308 : rbb_gain_ctrl2 */
#define RF_RBB_GAIN_CTRL2_OFFSET                                (0x308)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW0                              RF_GAIN_CTRL2_ROSDAC_Q_BW0
#define RF_GAIN_CTRL2_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL2_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL2_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL2_ROSDAC_I_BW0                              RF_GAIN_CTRL2_ROSDAC_I_BW0
#define RF_GAIN_CTRL2_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL2_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL2_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL2_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL2_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL2_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL2_ROSDAC_Q_BW1                              RF_GAIN_CTRL2_ROSDAC_Q_BW1
#define RF_GAIN_CTRL2_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL2_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL2_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL2_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL2_ROSDAC_I_BW1                              RF_GAIN_CTRL2_ROSDAC_I_BW1
#define RF_GAIN_CTRL2_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL2_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL2_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL2_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL2_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL2_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL2_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL2_G_RBB2                                    RF_GAIN_CTRL2_G_RBB2
#define RF_GAIN_CTRL2_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL2_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL2_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL2_G_RBB2_LEN)-1)<<RF_GAIN_CTRL2_G_RBB2_POS)
#define RF_GAIN_CTRL2_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL2_G_RBB2_LEN)-1)<<RF_GAIN_CTRL2_G_RBB2_POS))
#define RF_GAIN_CTRL2_G_RBB1                                    RF_GAIN_CTRL2_G_RBB1
#define RF_GAIN_CTRL2_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL2_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL2_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL2_G_RBB1_LEN)-1)<<RF_GAIN_CTRL2_G_RBB1_POS)
#define RF_GAIN_CTRL2_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL2_G_RBB1_LEN)-1)<<RF_GAIN_CTRL2_G_RBB1_POS))

/* 0x30C : rbb_gain_ctrl3 */
#define RF_RBB_GAIN_CTRL3_OFFSET                                (0x30C)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW0                              RF_GAIN_CTRL3_ROSDAC_Q_BW0
#define RF_GAIN_CTRL3_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL3_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL3_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL3_ROSDAC_I_BW0                              RF_GAIN_CTRL3_ROSDAC_I_BW0
#define RF_GAIN_CTRL3_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL3_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL3_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL3_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL3_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL3_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL3_ROSDAC_Q_BW1                              RF_GAIN_CTRL3_ROSDAC_Q_BW1
#define RF_GAIN_CTRL3_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL3_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL3_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL3_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL3_ROSDAC_I_BW1                              RF_GAIN_CTRL3_ROSDAC_I_BW1
#define RF_GAIN_CTRL3_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL3_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL3_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL3_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL3_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL3_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL3_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL3_G_RBB2                                    RF_GAIN_CTRL3_G_RBB2
#define RF_GAIN_CTRL3_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL3_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL3_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL3_G_RBB2_LEN)-1)<<RF_GAIN_CTRL3_G_RBB2_POS)
#define RF_GAIN_CTRL3_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL3_G_RBB2_LEN)-1)<<RF_GAIN_CTRL3_G_RBB2_POS))
#define RF_GAIN_CTRL3_G_RBB1                                    RF_GAIN_CTRL3_G_RBB1
#define RF_GAIN_CTRL3_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL3_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL3_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL3_G_RBB1_LEN)-1)<<RF_GAIN_CTRL3_G_RBB1_POS)
#define RF_GAIN_CTRL3_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL3_G_RBB1_LEN)-1)<<RF_GAIN_CTRL3_G_RBB1_POS))

/* 0x310 : rbb_gain_ctrl4 */
#define RF_RBB_GAIN_CTRL4_OFFSET                                (0x310)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW0                              RF_GAIN_CTRL4_ROSDAC_Q_BW0
#define RF_GAIN_CTRL4_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL4_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL4_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL4_ROSDAC_I_BW0                              RF_GAIN_CTRL4_ROSDAC_I_BW0
#define RF_GAIN_CTRL4_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL4_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL4_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL4_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL4_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL4_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL4_ROSDAC_Q_BW1                              RF_GAIN_CTRL4_ROSDAC_Q_BW1
#define RF_GAIN_CTRL4_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL4_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL4_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL4_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL4_ROSDAC_I_BW1                              RF_GAIN_CTRL4_ROSDAC_I_BW1
#define RF_GAIN_CTRL4_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL4_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL4_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL4_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL4_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL4_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL4_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL4_G_RBB2                                    RF_GAIN_CTRL4_G_RBB2
#define RF_GAIN_CTRL4_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL4_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL4_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL4_G_RBB2_LEN)-1)<<RF_GAIN_CTRL4_G_RBB2_POS)
#define RF_GAIN_CTRL4_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL4_G_RBB2_LEN)-1)<<RF_GAIN_CTRL4_G_RBB2_POS))
#define RF_GAIN_CTRL4_G_RBB1                                    RF_GAIN_CTRL4_G_RBB1
#define RF_GAIN_CTRL4_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL4_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL4_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL4_G_RBB1_LEN)-1)<<RF_GAIN_CTRL4_G_RBB1_POS)
#define RF_GAIN_CTRL4_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL4_G_RBB1_LEN)-1)<<RF_GAIN_CTRL4_G_RBB1_POS))

/* 0x314 : rbb_gain_ctrl5 */
#define RF_RBB_GAIN_CTRL5_OFFSET                                (0x314)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW0                              RF_GAIN_CTRL5_ROSDAC_Q_BW0
#define RF_GAIN_CTRL5_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL5_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL5_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL5_ROSDAC_I_BW0                              RF_GAIN_CTRL5_ROSDAC_I_BW0
#define RF_GAIN_CTRL5_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL5_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL5_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL5_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL5_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL5_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL5_ROSDAC_Q_BW1                              RF_GAIN_CTRL5_ROSDAC_Q_BW1
#define RF_GAIN_CTRL5_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL5_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL5_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL5_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL5_ROSDAC_I_BW1                              RF_GAIN_CTRL5_ROSDAC_I_BW1
#define RF_GAIN_CTRL5_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL5_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL5_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL5_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL5_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL5_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL5_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL5_G_RBB2                                    RF_GAIN_CTRL5_G_RBB2
#define RF_GAIN_CTRL5_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL5_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL5_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL5_G_RBB2_LEN)-1)<<RF_GAIN_CTRL5_G_RBB2_POS)
#define RF_GAIN_CTRL5_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL5_G_RBB2_LEN)-1)<<RF_GAIN_CTRL5_G_RBB2_POS))
#define RF_GAIN_CTRL5_G_RBB1                                    RF_GAIN_CTRL5_G_RBB1
#define RF_GAIN_CTRL5_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL5_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL5_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL5_G_RBB1_LEN)-1)<<RF_GAIN_CTRL5_G_RBB1_POS)
#define RF_GAIN_CTRL5_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL5_G_RBB1_LEN)-1)<<RF_GAIN_CTRL5_G_RBB1_POS))

/* 0x318 : rbb_gain_ctrl6 */
#define RF_RBB_GAIN_CTRL6_OFFSET                                (0x318)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW0                              RF_GAIN_CTRL6_ROSDAC_Q_BW0
#define RF_GAIN_CTRL6_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL6_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL6_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL6_ROSDAC_I_BW0                              RF_GAIN_CTRL6_ROSDAC_I_BW0
#define RF_GAIN_CTRL6_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL6_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL6_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL6_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL6_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL6_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL6_ROSDAC_Q_BW1                              RF_GAIN_CTRL6_ROSDAC_Q_BW1
#define RF_GAIN_CTRL6_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL6_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL6_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL6_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL6_ROSDAC_I_BW1                              RF_GAIN_CTRL6_ROSDAC_I_BW1
#define RF_GAIN_CTRL6_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL6_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL6_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL6_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL6_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL6_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL6_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL6_G_RBB2                                    RF_GAIN_CTRL6_G_RBB2
#define RF_GAIN_CTRL6_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL6_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL6_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL6_G_RBB2_LEN)-1)<<RF_GAIN_CTRL6_G_RBB2_POS)
#define RF_GAIN_CTRL6_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL6_G_RBB2_LEN)-1)<<RF_GAIN_CTRL6_G_RBB2_POS))
#define RF_GAIN_CTRL6_G_RBB1                                    RF_GAIN_CTRL6_G_RBB1
#define RF_GAIN_CTRL6_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL6_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL6_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL6_G_RBB1_LEN)-1)<<RF_GAIN_CTRL6_G_RBB1_POS)
#define RF_GAIN_CTRL6_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL6_G_RBB1_LEN)-1)<<RF_GAIN_CTRL6_G_RBB1_POS))

/* 0x31C : rbb_gain_ctrl7 */
#define RF_RBB_GAIN_CTRL7_OFFSET                                (0x31C)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW0                              RF_GAIN_CTRL7_ROSDAC_Q_BW0
#define RF_GAIN_CTRL7_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL7_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL7_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL7_ROSDAC_I_BW0                              RF_GAIN_CTRL7_ROSDAC_I_BW0
#define RF_GAIN_CTRL7_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL7_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL7_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL7_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL7_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL7_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL7_ROSDAC_Q_BW1                              RF_GAIN_CTRL7_ROSDAC_Q_BW1
#define RF_GAIN_CTRL7_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL7_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL7_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL7_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL7_ROSDAC_I_BW1                              RF_GAIN_CTRL7_ROSDAC_I_BW1
#define RF_GAIN_CTRL7_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL7_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL7_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL7_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL7_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL7_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL7_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL7_G_RBB2                                    RF_GAIN_CTRL7_G_RBB2
#define RF_GAIN_CTRL7_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL7_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL7_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL7_G_RBB2_LEN)-1)<<RF_GAIN_CTRL7_G_RBB2_POS)
#define RF_GAIN_CTRL7_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL7_G_RBB2_LEN)-1)<<RF_GAIN_CTRL7_G_RBB2_POS))
#define RF_GAIN_CTRL7_G_RBB1                                    RF_GAIN_CTRL7_G_RBB1
#define RF_GAIN_CTRL7_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL7_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL7_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL7_G_RBB1_LEN)-1)<<RF_GAIN_CTRL7_G_RBB1_POS)
#define RF_GAIN_CTRL7_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL7_G_RBB1_LEN)-1)<<RF_GAIN_CTRL7_G_RBB1_POS))

/* 0x320 : rbb_gain_ctrl8 */
#define RF_RBB_GAIN_CTRL8_OFFSET                                (0x320)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW0                              RF_GAIN_CTRL8_ROSDAC_Q_BW0
#define RF_GAIN_CTRL8_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL8_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL8_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL8_ROSDAC_I_BW0                              RF_GAIN_CTRL8_ROSDAC_I_BW0
#define RF_GAIN_CTRL8_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL8_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL8_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL8_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL8_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL8_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL8_ROSDAC_Q_BW1                              RF_GAIN_CTRL8_ROSDAC_Q_BW1
#define RF_GAIN_CTRL8_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL8_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL8_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL8_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL8_ROSDAC_I_BW1                              RF_GAIN_CTRL8_ROSDAC_I_BW1
#define RF_GAIN_CTRL8_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL8_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL8_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL8_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL8_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL8_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL8_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL8_G_RBB2                                    RF_GAIN_CTRL8_G_RBB2
#define RF_GAIN_CTRL8_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL8_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL8_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL8_G_RBB2_LEN)-1)<<RF_GAIN_CTRL8_G_RBB2_POS)
#define RF_GAIN_CTRL8_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL8_G_RBB2_LEN)-1)<<RF_GAIN_CTRL8_G_RBB2_POS))
#define RF_GAIN_CTRL8_G_RBB1                                    RF_GAIN_CTRL8_G_RBB1
#define RF_GAIN_CTRL8_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL8_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL8_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL8_G_RBB1_LEN)-1)<<RF_GAIN_CTRL8_G_RBB1_POS)
#define RF_GAIN_CTRL8_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL8_G_RBB1_LEN)-1)<<RF_GAIN_CTRL8_G_RBB1_POS))

/* 0x324 : rbb_gain_ctrl9 */
#define RF_RBB_GAIN_CTRL9_OFFSET                                (0x324)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW0                              RF_GAIN_CTRL9_ROSDAC_Q_BW0
#define RF_GAIN_CTRL9_ROSDAC_Q_BW0_POS                          (0U)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW0_LEN                          (6U)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW0_MSK                          (((1U<<RF_GAIN_CTRL9_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL9_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL9_ROSDAC_I_BW0                              RF_GAIN_CTRL9_ROSDAC_I_BW0
#define RF_GAIN_CTRL9_ROSDAC_I_BW0_POS                          (6U)
#define RF_GAIN_CTRL9_ROSDAC_I_BW0_LEN                          (6U)
#define RF_GAIN_CTRL9_ROSDAC_I_BW0_MSK                          (((1U<<RF_GAIN_CTRL9_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL9_ROSDAC_I_BW0_UMSK                         (~(((1U<<RF_GAIN_CTRL9_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL9_ROSDAC_Q_BW1                              RF_GAIN_CTRL9_ROSDAC_Q_BW1
#define RF_GAIN_CTRL9_ROSDAC_Q_BW1_POS                          (12U)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW1_LEN                          (6U)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW1_MSK                          (((1U<<RF_GAIN_CTRL9_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL9_ROSDAC_Q_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL9_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL9_ROSDAC_I_BW1                              RF_GAIN_CTRL9_ROSDAC_I_BW1
#define RF_GAIN_CTRL9_ROSDAC_I_BW1_POS                          (18U)
#define RF_GAIN_CTRL9_ROSDAC_I_BW1_LEN                          (6U)
#define RF_GAIN_CTRL9_ROSDAC_I_BW1_MSK                          (((1U<<RF_GAIN_CTRL9_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL9_ROSDAC_I_BW1_UMSK                         (~(((1U<<RF_GAIN_CTRL9_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL9_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL9_G_RBB2                                    RF_GAIN_CTRL9_G_RBB2
#define RF_GAIN_CTRL9_G_RBB2_POS                                (24U)
#define RF_GAIN_CTRL9_G_RBB2_LEN                                (3U)
#define RF_GAIN_CTRL9_G_RBB2_MSK                                (((1U<<RF_GAIN_CTRL9_G_RBB2_LEN)-1)<<RF_GAIN_CTRL9_G_RBB2_POS)
#define RF_GAIN_CTRL9_G_RBB2_UMSK                               (~(((1U<<RF_GAIN_CTRL9_G_RBB2_LEN)-1)<<RF_GAIN_CTRL9_G_RBB2_POS))
#define RF_GAIN_CTRL9_G_RBB1                                    RF_GAIN_CTRL9_G_RBB1
#define RF_GAIN_CTRL9_G_RBB1_POS                                (28U)
#define RF_GAIN_CTRL9_G_RBB1_LEN                                (2U)
#define RF_GAIN_CTRL9_G_RBB1_MSK                                (((1U<<RF_GAIN_CTRL9_G_RBB1_LEN)-1)<<RF_GAIN_CTRL9_G_RBB1_POS)
#define RF_GAIN_CTRL9_G_RBB1_UMSK                               (~(((1U<<RF_GAIN_CTRL9_G_RBB1_LEN)-1)<<RF_GAIN_CTRL9_G_RBB1_POS))

/* 0x328 : rbb_gain_ctrl10 */
#define RF_RBB_GAIN_CTRL10_OFFSET                               (0x328)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW0                             RF_GAIN_CTRL10_ROSDAC_Q_BW0
#define RF_GAIN_CTRL10_ROSDAC_Q_BW0_POS                         (0U)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW0_LEN                         (6U)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW0_MSK                         (((1U<<RF_GAIN_CTRL10_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL10_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL10_ROSDAC_I_BW0                             RF_GAIN_CTRL10_ROSDAC_I_BW0
#define RF_GAIN_CTRL10_ROSDAC_I_BW0_POS                         (6U)
#define RF_GAIN_CTRL10_ROSDAC_I_BW0_LEN                         (6U)
#define RF_GAIN_CTRL10_ROSDAC_I_BW0_MSK                         (((1U<<RF_GAIN_CTRL10_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL10_ROSDAC_I_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL10_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL10_ROSDAC_Q_BW1                             RF_GAIN_CTRL10_ROSDAC_Q_BW1
#define RF_GAIN_CTRL10_ROSDAC_Q_BW1_POS                         (12U)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW1_LEN                         (6U)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW1_MSK                         (((1U<<RF_GAIN_CTRL10_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL10_ROSDAC_Q_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL10_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL10_ROSDAC_I_BW1                             RF_GAIN_CTRL10_ROSDAC_I_BW1
#define RF_GAIN_CTRL10_ROSDAC_I_BW1_POS                         (18U)
#define RF_GAIN_CTRL10_ROSDAC_I_BW1_LEN                         (6U)
#define RF_GAIN_CTRL10_ROSDAC_I_BW1_MSK                         (((1U<<RF_GAIN_CTRL10_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL10_ROSDAC_I_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL10_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL10_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL10_G_RBB2                                   RF_GAIN_CTRL10_G_RBB2
#define RF_GAIN_CTRL10_G_RBB2_POS                               (24U)
#define RF_GAIN_CTRL10_G_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL10_G_RBB2_MSK                               (((1U<<RF_GAIN_CTRL10_G_RBB2_LEN)-1)<<RF_GAIN_CTRL10_G_RBB2_POS)
#define RF_GAIN_CTRL10_G_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL10_G_RBB2_LEN)-1)<<RF_GAIN_CTRL10_G_RBB2_POS))
#define RF_GAIN_CTRL10_G_RBB1                                   RF_GAIN_CTRL10_G_RBB1
#define RF_GAIN_CTRL10_G_RBB1_POS                               (28U)
#define RF_GAIN_CTRL10_G_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL10_G_RBB1_MSK                               (((1U<<RF_GAIN_CTRL10_G_RBB1_LEN)-1)<<RF_GAIN_CTRL10_G_RBB1_POS)
#define RF_GAIN_CTRL10_G_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL10_G_RBB1_LEN)-1)<<RF_GAIN_CTRL10_G_RBB1_POS))

/* 0x32C : rbb_gain_ctrl11 */
#define RF_RBB_GAIN_CTRL11_OFFSET                               (0x32C)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW0                             RF_GAIN_CTRL11_ROSDAC_Q_BW0
#define RF_GAIN_CTRL11_ROSDAC_Q_BW0_POS                         (0U)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW0_LEN                         (6U)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW0_MSK                         (((1U<<RF_GAIN_CTRL11_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL11_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL11_ROSDAC_I_BW0                             RF_GAIN_CTRL11_ROSDAC_I_BW0
#define RF_GAIN_CTRL11_ROSDAC_I_BW0_POS                         (6U)
#define RF_GAIN_CTRL11_ROSDAC_I_BW0_LEN                         (6U)
#define RF_GAIN_CTRL11_ROSDAC_I_BW0_MSK                         (((1U<<RF_GAIN_CTRL11_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL11_ROSDAC_I_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL11_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL11_ROSDAC_Q_BW1                             RF_GAIN_CTRL11_ROSDAC_Q_BW1
#define RF_GAIN_CTRL11_ROSDAC_Q_BW1_POS                         (12U)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW1_LEN                         (6U)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW1_MSK                         (((1U<<RF_GAIN_CTRL11_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL11_ROSDAC_Q_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL11_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL11_ROSDAC_I_BW1                             RF_GAIN_CTRL11_ROSDAC_I_BW1
#define RF_GAIN_CTRL11_ROSDAC_I_BW1_POS                         (18U)
#define RF_GAIN_CTRL11_ROSDAC_I_BW1_LEN                         (6U)
#define RF_GAIN_CTRL11_ROSDAC_I_BW1_MSK                         (((1U<<RF_GAIN_CTRL11_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL11_ROSDAC_I_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL11_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL11_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL11_G_RBB2                                   RF_GAIN_CTRL11_G_RBB2
#define RF_GAIN_CTRL11_G_RBB2_POS                               (24U)
#define RF_GAIN_CTRL11_G_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL11_G_RBB2_MSK                               (((1U<<RF_GAIN_CTRL11_G_RBB2_LEN)-1)<<RF_GAIN_CTRL11_G_RBB2_POS)
#define RF_GAIN_CTRL11_G_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL11_G_RBB2_LEN)-1)<<RF_GAIN_CTRL11_G_RBB2_POS))
#define RF_GAIN_CTRL11_G_RBB1                                   RF_GAIN_CTRL11_G_RBB1
#define RF_GAIN_CTRL11_G_RBB1_POS                               (28U)
#define RF_GAIN_CTRL11_G_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL11_G_RBB1_MSK                               (((1U<<RF_GAIN_CTRL11_G_RBB1_LEN)-1)<<RF_GAIN_CTRL11_G_RBB1_POS)
#define RF_GAIN_CTRL11_G_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL11_G_RBB1_LEN)-1)<<RF_GAIN_CTRL11_G_RBB1_POS))

/* 0x330 : rbb_gain_ctrl12 */
#define RF_RBB_GAIN_CTRL12_OFFSET                               (0x330)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW0                             RF_GAIN_CTRL12_ROSDAC_Q_BW0
#define RF_GAIN_CTRL12_ROSDAC_Q_BW0_POS                         (0U)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW0_LEN                         (6U)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW0_MSK                         (((1U<<RF_GAIN_CTRL12_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL12_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL12_ROSDAC_I_BW0                             RF_GAIN_CTRL12_ROSDAC_I_BW0
#define RF_GAIN_CTRL12_ROSDAC_I_BW0_POS                         (6U)
#define RF_GAIN_CTRL12_ROSDAC_I_BW0_LEN                         (6U)
#define RF_GAIN_CTRL12_ROSDAC_I_BW0_MSK                         (((1U<<RF_GAIN_CTRL12_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL12_ROSDAC_I_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL12_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL12_ROSDAC_Q_BW1                             RF_GAIN_CTRL12_ROSDAC_Q_BW1
#define RF_GAIN_CTRL12_ROSDAC_Q_BW1_POS                         (12U)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW1_LEN                         (6U)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW1_MSK                         (((1U<<RF_GAIN_CTRL12_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL12_ROSDAC_Q_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL12_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL12_ROSDAC_I_BW1                             RF_GAIN_CTRL12_ROSDAC_I_BW1
#define RF_GAIN_CTRL12_ROSDAC_I_BW1_POS                         (18U)
#define RF_GAIN_CTRL12_ROSDAC_I_BW1_LEN                         (6U)
#define RF_GAIN_CTRL12_ROSDAC_I_BW1_MSK                         (((1U<<RF_GAIN_CTRL12_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL12_ROSDAC_I_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL12_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL12_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL12_G_RBB2                                   RF_GAIN_CTRL12_G_RBB2
#define RF_GAIN_CTRL12_G_RBB2_POS                               (24U)
#define RF_GAIN_CTRL12_G_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL12_G_RBB2_MSK                               (((1U<<RF_GAIN_CTRL12_G_RBB2_LEN)-1)<<RF_GAIN_CTRL12_G_RBB2_POS)
#define RF_GAIN_CTRL12_G_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL12_G_RBB2_LEN)-1)<<RF_GAIN_CTRL12_G_RBB2_POS))
#define RF_GAIN_CTRL12_G_RBB1                                   RF_GAIN_CTRL12_G_RBB1
#define RF_GAIN_CTRL12_G_RBB1_POS                               (28U)
#define RF_GAIN_CTRL12_G_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL12_G_RBB1_MSK                               (((1U<<RF_GAIN_CTRL12_G_RBB1_LEN)-1)<<RF_GAIN_CTRL12_G_RBB1_POS)
#define RF_GAIN_CTRL12_G_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL12_G_RBB1_LEN)-1)<<RF_GAIN_CTRL12_G_RBB1_POS))

/* 0x334 : rbb_gain_ctrl13 */
#define RF_RBB_GAIN_CTRL13_OFFSET                               (0x334)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW0                             RF_GAIN_CTRL13_ROSDAC_Q_BW0
#define RF_GAIN_CTRL13_ROSDAC_Q_BW0_POS                         (0U)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW0_LEN                         (6U)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW0_MSK                         (((1U<<RF_GAIN_CTRL13_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL13_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL13_ROSDAC_I_BW0                             RF_GAIN_CTRL13_ROSDAC_I_BW0
#define RF_GAIN_CTRL13_ROSDAC_I_BW0_POS                         (6U)
#define RF_GAIN_CTRL13_ROSDAC_I_BW0_LEN                         (6U)
#define RF_GAIN_CTRL13_ROSDAC_I_BW0_MSK                         (((1U<<RF_GAIN_CTRL13_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL13_ROSDAC_I_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL13_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL13_ROSDAC_Q_BW1                             RF_GAIN_CTRL13_ROSDAC_Q_BW1
#define RF_GAIN_CTRL13_ROSDAC_Q_BW1_POS                         (12U)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW1_LEN                         (6U)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW1_MSK                         (((1U<<RF_GAIN_CTRL13_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL13_ROSDAC_Q_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL13_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL13_ROSDAC_I_BW1                             RF_GAIN_CTRL13_ROSDAC_I_BW1
#define RF_GAIN_CTRL13_ROSDAC_I_BW1_POS                         (18U)
#define RF_GAIN_CTRL13_ROSDAC_I_BW1_LEN                         (6U)
#define RF_GAIN_CTRL13_ROSDAC_I_BW1_MSK                         (((1U<<RF_GAIN_CTRL13_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL13_ROSDAC_I_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL13_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL13_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL13_G_RBB2                                   RF_GAIN_CTRL13_G_RBB2
#define RF_GAIN_CTRL13_G_RBB2_POS                               (24U)
#define RF_GAIN_CTRL13_G_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL13_G_RBB2_MSK                               (((1U<<RF_GAIN_CTRL13_G_RBB2_LEN)-1)<<RF_GAIN_CTRL13_G_RBB2_POS)
#define RF_GAIN_CTRL13_G_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL13_G_RBB2_LEN)-1)<<RF_GAIN_CTRL13_G_RBB2_POS))
#define RF_GAIN_CTRL13_G_RBB1                                   RF_GAIN_CTRL13_G_RBB1
#define RF_GAIN_CTRL13_G_RBB1_POS                               (28U)
#define RF_GAIN_CTRL13_G_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL13_G_RBB1_MSK                               (((1U<<RF_GAIN_CTRL13_G_RBB1_LEN)-1)<<RF_GAIN_CTRL13_G_RBB1_POS)
#define RF_GAIN_CTRL13_G_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL13_G_RBB1_LEN)-1)<<RF_GAIN_CTRL13_G_RBB1_POS))

/* 0x338 : rbb_gain_ctrl14 */
#define RF_RBB_GAIN_CTRL14_OFFSET                               (0x338)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW0                             RF_GAIN_CTRL14_ROSDAC_Q_BW0
#define RF_GAIN_CTRL14_ROSDAC_Q_BW0_POS                         (0U)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW0_LEN                         (6U)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW0_MSK                         (((1U<<RF_GAIN_CTRL14_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL14_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL14_ROSDAC_I_BW0                             RF_GAIN_CTRL14_ROSDAC_I_BW0
#define RF_GAIN_CTRL14_ROSDAC_I_BW0_POS                         (6U)
#define RF_GAIN_CTRL14_ROSDAC_I_BW0_LEN                         (6U)
#define RF_GAIN_CTRL14_ROSDAC_I_BW0_MSK                         (((1U<<RF_GAIN_CTRL14_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL14_ROSDAC_I_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL14_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL14_ROSDAC_Q_BW1                             RF_GAIN_CTRL14_ROSDAC_Q_BW1
#define RF_GAIN_CTRL14_ROSDAC_Q_BW1_POS                         (12U)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW1_LEN                         (6U)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW1_MSK                         (((1U<<RF_GAIN_CTRL14_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL14_ROSDAC_Q_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL14_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL14_ROSDAC_I_BW1                             RF_GAIN_CTRL14_ROSDAC_I_BW1
#define RF_GAIN_CTRL14_ROSDAC_I_BW1_POS                         (18U)
#define RF_GAIN_CTRL14_ROSDAC_I_BW1_LEN                         (6U)
#define RF_GAIN_CTRL14_ROSDAC_I_BW1_MSK                         (((1U<<RF_GAIN_CTRL14_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL14_ROSDAC_I_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL14_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL14_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL14_G_RBB2                                   RF_GAIN_CTRL14_G_RBB2
#define RF_GAIN_CTRL14_G_RBB2_POS                               (24U)
#define RF_GAIN_CTRL14_G_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL14_G_RBB2_MSK                               (((1U<<RF_GAIN_CTRL14_G_RBB2_LEN)-1)<<RF_GAIN_CTRL14_G_RBB2_POS)
#define RF_GAIN_CTRL14_G_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL14_G_RBB2_LEN)-1)<<RF_GAIN_CTRL14_G_RBB2_POS))
#define RF_GAIN_CTRL14_G_RBB1                                   RF_GAIN_CTRL14_G_RBB1
#define RF_GAIN_CTRL14_G_RBB1_POS                               (28U)
#define RF_GAIN_CTRL14_G_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL14_G_RBB1_MSK                               (((1U<<RF_GAIN_CTRL14_G_RBB1_LEN)-1)<<RF_GAIN_CTRL14_G_RBB1_POS)
#define RF_GAIN_CTRL14_G_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL14_G_RBB1_LEN)-1)<<RF_GAIN_CTRL14_G_RBB1_POS))

/* 0x33C : rbb_gain_ctrl15 */
#define RF_RBB_GAIN_CTRL15_OFFSET                               (0x33C)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW0                             RF_GAIN_CTRL15_ROSDAC_Q_BW0
#define RF_GAIN_CTRL15_ROSDAC_Q_BW0_POS                         (0U)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW0_LEN                         (6U)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW0_MSK                         (((1U<<RF_GAIN_CTRL15_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_Q_BW0_POS)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL15_ROSDAC_Q_BW0_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_Q_BW0_POS))
#define RF_GAIN_CTRL15_ROSDAC_I_BW0                             RF_GAIN_CTRL15_ROSDAC_I_BW0
#define RF_GAIN_CTRL15_ROSDAC_I_BW0_POS                         (6U)
#define RF_GAIN_CTRL15_ROSDAC_I_BW0_LEN                         (6U)
#define RF_GAIN_CTRL15_ROSDAC_I_BW0_MSK                         (((1U<<RF_GAIN_CTRL15_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_I_BW0_POS)
#define RF_GAIN_CTRL15_ROSDAC_I_BW0_UMSK                        (~(((1U<<RF_GAIN_CTRL15_ROSDAC_I_BW0_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_I_BW0_POS))
#define RF_GAIN_CTRL15_ROSDAC_Q_BW1                             RF_GAIN_CTRL15_ROSDAC_Q_BW1
#define RF_GAIN_CTRL15_ROSDAC_Q_BW1_POS                         (12U)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW1_LEN                         (6U)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW1_MSK                         (((1U<<RF_GAIN_CTRL15_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_Q_BW1_POS)
#define RF_GAIN_CTRL15_ROSDAC_Q_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL15_ROSDAC_Q_BW1_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_Q_BW1_POS))
#define RF_GAIN_CTRL15_ROSDAC_I_BW1                             RF_GAIN_CTRL15_ROSDAC_I_BW1
#define RF_GAIN_CTRL15_ROSDAC_I_BW1_POS                         (18U)
#define RF_GAIN_CTRL15_ROSDAC_I_BW1_LEN                         (6U)
#define RF_GAIN_CTRL15_ROSDAC_I_BW1_MSK                         (((1U<<RF_GAIN_CTRL15_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_I_BW1_POS)
#define RF_GAIN_CTRL15_ROSDAC_I_BW1_UMSK                        (~(((1U<<RF_GAIN_CTRL15_ROSDAC_I_BW1_LEN)-1)<<RF_GAIN_CTRL15_ROSDAC_I_BW1_POS))
#define RF_GAIN_CTRL15_G_RBB2                                   RF_GAIN_CTRL15_G_RBB2
#define RF_GAIN_CTRL15_G_RBB2_POS                               (24U)
#define RF_GAIN_CTRL15_G_RBB2_LEN                               (3U)
#define RF_GAIN_CTRL15_G_RBB2_MSK                               (((1U<<RF_GAIN_CTRL15_G_RBB2_LEN)-1)<<RF_GAIN_CTRL15_G_RBB2_POS)
#define RF_GAIN_CTRL15_G_RBB2_UMSK                              (~(((1U<<RF_GAIN_CTRL15_G_RBB2_LEN)-1)<<RF_GAIN_CTRL15_G_RBB2_POS))
#define RF_GAIN_CTRL15_G_RBB1                                   RF_GAIN_CTRL15_G_RBB1
#define RF_GAIN_CTRL15_G_RBB1_POS                               (28U)
#define RF_GAIN_CTRL15_G_RBB1_LEN                               (2U)
#define RF_GAIN_CTRL15_G_RBB1_MSK                               (((1U<<RF_GAIN_CTRL15_G_RBB1_LEN)-1)<<RF_GAIN_CTRL15_G_RBB1_POS)
#define RF_GAIN_CTRL15_G_RBB1_UMSK                              (~(((1U<<RF_GAIN_CTRL15_G_RBB1_LEN)-1)<<RF_GAIN_CTRL15_G_RBB1_POS))

/* 0x400 : acal_config */
#define RF_ACAL_CONFIG_OFFSET                                   (0x400)
#define RF_VCO_IDAC_LL                                          RF_VCO_IDAC_LL
#define RF_VCO_IDAC_LL_POS                                      (0U)
#define RF_VCO_IDAC_LL_LEN                                      (6U)
#define RF_VCO_IDAC_LL_MSK                                      (((1U<<RF_VCO_IDAC_LL_LEN)-1)<<RF_VCO_IDAC_LL_POS)
#define RF_VCO_IDAC_LL_UMSK                                     (~(((1U<<RF_VCO_IDAC_LL_LEN)-1)<<RF_VCO_IDAC_LL_POS))
#define RF_VCO_IDAC_LH                                          RF_VCO_IDAC_LH
#define RF_VCO_IDAC_LH_POS                                      (8U)
#define RF_VCO_IDAC_LH_LEN                                      (6U)
#define RF_VCO_IDAC_LH_MSK                                      (((1U<<RF_VCO_IDAC_LH_LEN)-1)<<RF_VCO_IDAC_LH_POS)
#define RF_VCO_IDAC_LH_UMSK                                     (~(((1U<<RF_VCO_IDAC_LH_LEN)-1)<<RF_VCO_IDAC_LH_POS))
#define RF_VCO_IDAC_HL                                          RF_VCO_IDAC_HL
#define RF_VCO_IDAC_HL_POS                                      (16U)
#define RF_VCO_IDAC_HL_LEN                                      (6U)
#define RF_VCO_IDAC_HL_MSK                                      (((1U<<RF_VCO_IDAC_HL_LEN)-1)<<RF_VCO_IDAC_HL_POS)
#define RF_VCO_IDAC_HL_UMSK                                     (~(((1U<<RF_VCO_IDAC_HL_LEN)-1)<<RF_VCO_IDAC_HL_POS))
#define RF_VCO_IDAC_HH                                          RF_VCO_IDAC_HH
#define RF_VCO_IDAC_HH_POS                                      (24U)
#define RF_VCO_IDAC_HH_LEN                                      (6U)
#define RF_VCO_IDAC_HH_MSK                                      (((1U<<RF_VCO_IDAC_HH_LEN)-1)<<RF_VCO_IDAC_HH_POS)
#define RF_VCO_IDAC_HH_UMSK                                     (~(((1U<<RF_VCO_IDAC_HH_LEN)-1)<<RF_VCO_IDAC_HH_POS))

/* 0x404 : lo_config_2402 */
#define RF_LO_CONFIG_2402_OFFSET                                (0x404)
#define RF_KCAL_RATIO_2402                                      RF_KCAL_RATIO_2402
#define RF_KCAL_RATIO_2402_POS                                  (0U)
#define RF_KCAL_RATIO_2402_LEN                                  (10U)
#define RF_KCAL_RATIO_2402_MSK                                  (((1U<<RF_KCAL_RATIO_2402_LEN)-1)<<RF_KCAL_RATIO_2402_POS)
#define RF_KCAL_RATIO_2402_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2402_LEN)-1)<<RF_KCAL_RATIO_2402_POS))
#define RF_ADPLL_SDM_DITHER_EN_2402                             RF_ADPLL_SDM_DITHER_EN_2402
#define RF_ADPLL_SDM_DITHER_EN_2402_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2402_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2402_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2402_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2402_POS)
#define RF_ADPLL_SDM_DITHER_EN_2402_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2402_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2402_POS))

/* 0x408 : lo_config_2404 */
#define RF_LO_CONFIG_2404_OFFSET                                (0x408)
#define RF_KCAL_RATIO_2404                                      RF_KCAL_RATIO_2404
#define RF_KCAL_RATIO_2404_POS                                  (0U)
#define RF_KCAL_RATIO_2404_LEN                                  (10U)
#define RF_KCAL_RATIO_2404_MSK                                  (((1U<<RF_KCAL_RATIO_2404_LEN)-1)<<RF_KCAL_RATIO_2404_POS)
#define RF_KCAL_RATIO_2404_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2404_LEN)-1)<<RF_KCAL_RATIO_2404_POS))
#define RF_ADPLL_SDM_DITHER_EN_2404                             RF_ADPLL_SDM_DITHER_EN_2404
#define RF_ADPLL_SDM_DITHER_EN_2404_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2404_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2404_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2404_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2404_POS)
#define RF_ADPLL_SDM_DITHER_EN_2404_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2404_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2404_POS))

/* 0x40C : lo_config_2406 */
#define RF_LO_CONFIG_2406_OFFSET                                (0x40C)
#define RF_KCAL_RATIO_2406                                      RF_KCAL_RATIO_2406
#define RF_KCAL_RATIO_2406_POS                                  (0U)
#define RF_KCAL_RATIO_2406_LEN                                  (10U)
#define RF_KCAL_RATIO_2406_MSK                                  (((1U<<RF_KCAL_RATIO_2406_LEN)-1)<<RF_KCAL_RATIO_2406_POS)
#define RF_KCAL_RATIO_2406_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2406_LEN)-1)<<RF_KCAL_RATIO_2406_POS))
#define RF_ADPLL_SDM_DITHER_EN_2406                             RF_ADPLL_SDM_DITHER_EN_2406
#define RF_ADPLL_SDM_DITHER_EN_2406_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2406_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2406_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2406_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2406_POS)
#define RF_ADPLL_SDM_DITHER_EN_2406_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2406_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2406_POS))

/* 0x410 : lo_config_2408 */
#define RF_LO_CONFIG_2408_OFFSET                                (0x410)
#define RF_KCAL_RATIO_2408                                      RF_KCAL_RATIO_2408
#define RF_KCAL_RATIO_2408_POS                                  (0U)
#define RF_KCAL_RATIO_2408_LEN                                  (10U)
#define RF_KCAL_RATIO_2408_MSK                                  (((1U<<RF_KCAL_RATIO_2408_LEN)-1)<<RF_KCAL_RATIO_2408_POS)
#define RF_KCAL_RATIO_2408_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2408_LEN)-1)<<RF_KCAL_RATIO_2408_POS))
#define RF_ADPLL_SDM_DITHER_EN_2408                             RF_ADPLL_SDM_DITHER_EN_2408
#define RF_ADPLL_SDM_DITHER_EN_2408_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2408_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2408_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2408_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2408_POS)
#define RF_ADPLL_SDM_DITHER_EN_2408_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2408_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2408_POS))

/* 0x414 : lo_config_2410 */
#define RF_LO_CONFIG_2410_OFFSET                                (0x414)
#define RF_KCAL_RATIO_2410                                      RF_KCAL_RATIO_2410
#define RF_KCAL_RATIO_2410_POS                                  (0U)
#define RF_KCAL_RATIO_2410_LEN                                  (10U)
#define RF_KCAL_RATIO_2410_MSK                                  (((1U<<RF_KCAL_RATIO_2410_LEN)-1)<<RF_KCAL_RATIO_2410_POS)
#define RF_KCAL_RATIO_2410_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2410_LEN)-1)<<RF_KCAL_RATIO_2410_POS))
#define RF_ADPLL_SDM_DITHER_EN_2410                             RF_ADPLL_SDM_DITHER_EN_2410
#define RF_ADPLL_SDM_DITHER_EN_2410_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2410_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2410_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2410_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2410_POS)
#define RF_ADPLL_SDM_DITHER_EN_2410_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2410_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2410_POS))

/* 0x418 : lo_config_2412 */
#define RF_LO_CONFIG_2412_OFFSET                                (0x418)
#define RF_KCAL_RATIO_2412                                      RF_KCAL_RATIO_2412
#define RF_KCAL_RATIO_2412_POS                                  (0U)
#define RF_KCAL_RATIO_2412_LEN                                  (10U)
#define RF_KCAL_RATIO_2412_MSK                                  (((1U<<RF_KCAL_RATIO_2412_LEN)-1)<<RF_KCAL_RATIO_2412_POS)
#define RF_KCAL_RATIO_2412_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2412_LEN)-1)<<RF_KCAL_RATIO_2412_POS))
#define RF_ADPLL_SDM_DITHER_EN_2412                             RF_ADPLL_SDM_DITHER_EN_2412
#define RF_ADPLL_SDM_DITHER_EN_2412_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2412_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2412_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2412_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2412_POS)
#define RF_ADPLL_SDM_DITHER_EN_2412_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2412_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2412_POS))

/* 0x41C : lo_config_2414 */
#define RF_LO_CONFIG_2414_OFFSET                                (0x41C)
#define RF_KCAL_RATIO_2414                                      RF_KCAL_RATIO_2414
#define RF_KCAL_RATIO_2414_POS                                  (0U)
#define RF_KCAL_RATIO_2414_LEN                                  (10U)
#define RF_KCAL_RATIO_2414_MSK                                  (((1U<<RF_KCAL_RATIO_2414_LEN)-1)<<RF_KCAL_RATIO_2414_POS)
#define RF_KCAL_RATIO_2414_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2414_LEN)-1)<<RF_KCAL_RATIO_2414_POS))
#define RF_ADPLL_SDM_DITHER_EN_2414                             RF_ADPLL_SDM_DITHER_EN_2414
#define RF_ADPLL_SDM_DITHER_EN_2414_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2414_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2414_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2414_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2414_POS)
#define RF_ADPLL_SDM_DITHER_EN_2414_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2414_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2414_POS))

/* 0x420 : lo_config_2416 */
#define RF_LO_CONFIG_2416_OFFSET                                (0x420)
#define RF_KCAL_RATIO_2416                                      RF_KCAL_RATIO_2416
#define RF_KCAL_RATIO_2416_POS                                  (0U)
#define RF_KCAL_RATIO_2416_LEN                                  (10U)
#define RF_KCAL_RATIO_2416_MSK                                  (((1U<<RF_KCAL_RATIO_2416_LEN)-1)<<RF_KCAL_RATIO_2416_POS)
#define RF_KCAL_RATIO_2416_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2416_LEN)-1)<<RF_KCAL_RATIO_2416_POS))
#define RF_ADPLL_SDM_DITHER_EN_2416                             RF_ADPLL_SDM_DITHER_EN_2416
#define RF_ADPLL_SDM_DITHER_EN_2416_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2416_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2416_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2416_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2416_POS)
#define RF_ADPLL_SDM_DITHER_EN_2416_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2416_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2416_POS))

/* 0x424 : lo_config_2418 */
#define RF_LO_CONFIG_2418_OFFSET                                (0x424)
#define RF_KCAL_RATIO_2418                                      RF_KCAL_RATIO_2418
#define RF_KCAL_RATIO_2418_POS                                  (0U)
#define RF_KCAL_RATIO_2418_LEN                                  (10U)
#define RF_KCAL_RATIO_2418_MSK                                  (((1U<<RF_KCAL_RATIO_2418_LEN)-1)<<RF_KCAL_RATIO_2418_POS)
#define RF_KCAL_RATIO_2418_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2418_LEN)-1)<<RF_KCAL_RATIO_2418_POS))
#define RF_ADPLL_SDM_DITHER_EN_2418                             RF_ADPLL_SDM_DITHER_EN_2418
#define RF_ADPLL_SDM_DITHER_EN_2418_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2418_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2418_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2418_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2418_POS)
#define RF_ADPLL_SDM_DITHER_EN_2418_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2418_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2418_POS))

/* 0x428 : lo_config_2420 */
#define RF_LO_CONFIG_2420_OFFSET                                (0x428)
#define RF_KCAL_RATIO_2420                                      RF_KCAL_RATIO_2420
#define RF_KCAL_RATIO_2420_POS                                  (0U)
#define RF_KCAL_RATIO_2420_LEN                                  (10U)
#define RF_KCAL_RATIO_2420_MSK                                  (((1U<<RF_KCAL_RATIO_2420_LEN)-1)<<RF_KCAL_RATIO_2420_POS)
#define RF_KCAL_RATIO_2420_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2420_LEN)-1)<<RF_KCAL_RATIO_2420_POS))
#define RF_ADPLL_SDM_DITHER_EN_2420                             RF_ADPLL_SDM_DITHER_EN_2420
#define RF_ADPLL_SDM_DITHER_EN_2420_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2420_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2420_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2420_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2420_POS)
#define RF_ADPLL_SDM_DITHER_EN_2420_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2420_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2420_POS))

/* 0x42C : lo_config_2422 */
#define RF_LO_CONFIG_2422_OFFSET                                (0x42C)
#define RF_KCAL_RATIO_2422                                      RF_KCAL_RATIO_2422
#define RF_KCAL_RATIO_2422_POS                                  (0U)
#define RF_KCAL_RATIO_2422_LEN                                  (10U)
#define RF_KCAL_RATIO_2422_MSK                                  (((1U<<RF_KCAL_RATIO_2422_LEN)-1)<<RF_KCAL_RATIO_2422_POS)
#define RF_KCAL_RATIO_2422_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2422_LEN)-1)<<RF_KCAL_RATIO_2422_POS))
#define RF_ADPLL_SDM_DITHER_EN_2422                             RF_ADPLL_SDM_DITHER_EN_2422
#define RF_ADPLL_SDM_DITHER_EN_2422_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2422_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2422_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2422_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2422_POS)
#define RF_ADPLL_SDM_DITHER_EN_2422_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2422_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2422_POS))

/* 0x430 : lo_config_2424 */
#define RF_LO_CONFIG_2424_OFFSET                                (0x430)
#define RF_KCAL_RATIO_2424                                      RF_KCAL_RATIO_2424
#define RF_KCAL_RATIO_2424_POS                                  (0U)
#define RF_KCAL_RATIO_2424_LEN                                  (10U)
#define RF_KCAL_RATIO_2424_MSK                                  (((1U<<RF_KCAL_RATIO_2424_LEN)-1)<<RF_KCAL_RATIO_2424_POS)
#define RF_KCAL_RATIO_2424_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2424_LEN)-1)<<RF_KCAL_RATIO_2424_POS))
#define RF_ADPLL_SDM_DITHER_EN_2424                             RF_ADPLL_SDM_DITHER_EN_2424
#define RF_ADPLL_SDM_DITHER_EN_2424_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2424_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2424_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2424_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2424_POS)
#define RF_ADPLL_SDM_DITHER_EN_2424_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2424_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2424_POS))

/* 0x434 : lo_config_2426 */
#define RF_LO_CONFIG_2426_OFFSET                                (0x434)
#define RF_KCAL_RATIO_2426                                      RF_KCAL_RATIO_2426
#define RF_KCAL_RATIO_2426_POS                                  (0U)
#define RF_KCAL_RATIO_2426_LEN                                  (10U)
#define RF_KCAL_RATIO_2426_MSK                                  (((1U<<RF_KCAL_RATIO_2426_LEN)-1)<<RF_KCAL_RATIO_2426_POS)
#define RF_KCAL_RATIO_2426_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2426_LEN)-1)<<RF_KCAL_RATIO_2426_POS))
#define RF_ADPLL_SDM_DITHER_EN_2426                             RF_ADPLL_SDM_DITHER_EN_2426
#define RF_ADPLL_SDM_DITHER_EN_2426_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2426_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2426_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2426_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2426_POS)
#define RF_ADPLL_SDM_DITHER_EN_2426_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2426_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2426_POS))

/* 0x438 : lo_config_2428 */
#define RF_LO_CONFIG_2428_OFFSET                                (0x438)
#define RF_KCAL_RATIO_2428                                      RF_KCAL_RATIO_2428
#define RF_KCAL_RATIO_2428_POS                                  (0U)
#define RF_KCAL_RATIO_2428_LEN                                  (10U)
#define RF_KCAL_RATIO_2428_MSK                                  (((1U<<RF_KCAL_RATIO_2428_LEN)-1)<<RF_KCAL_RATIO_2428_POS)
#define RF_KCAL_RATIO_2428_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2428_LEN)-1)<<RF_KCAL_RATIO_2428_POS))
#define RF_ADPLL_SDM_DITHER_EN_2428                             RF_ADPLL_SDM_DITHER_EN_2428
#define RF_ADPLL_SDM_DITHER_EN_2428_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2428_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2428_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2428_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2428_POS)
#define RF_ADPLL_SDM_DITHER_EN_2428_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2428_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2428_POS))

/* 0x43C : lo_config_2430 */
#define RF_LO_CONFIG_2430_OFFSET                                (0x43C)
#define RF_KCAL_RATIO_2430                                      RF_KCAL_RATIO_2430
#define RF_KCAL_RATIO_2430_POS                                  (0U)
#define RF_KCAL_RATIO_2430_LEN                                  (10U)
#define RF_KCAL_RATIO_2430_MSK                                  (((1U<<RF_KCAL_RATIO_2430_LEN)-1)<<RF_KCAL_RATIO_2430_POS)
#define RF_KCAL_RATIO_2430_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2430_LEN)-1)<<RF_KCAL_RATIO_2430_POS))
#define RF_ADPLL_SDM_DITHER_EN_2430                             RF_ADPLL_SDM_DITHER_EN_2430
#define RF_ADPLL_SDM_DITHER_EN_2430_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2430_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2430_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2430_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2430_POS)
#define RF_ADPLL_SDM_DITHER_EN_2430_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2430_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2430_POS))

/* 0x440 : lo_config_2432 */
#define RF_LO_CONFIG_2432_OFFSET                                (0x440)
#define RF_KCAL_RATIO_2432                                      RF_KCAL_RATIO_2432
#define RF_KCAL_RATIO_2432_POS                                  (0U)
#define RF_KCAL_RATIO_2432_LEN                                  (10U)
#define RF_KCAL_RATIO_2432_MSK                                  (((1U<<RF_KCAL_RATIO_2432_LEN)-1)<<RF_KCAL_RATIO_2432_POS)
#define RF_KCAL_RATIO_2432_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2432_LEN)-1)<<RF_KCAL_RATIO_2432_POS))
#define RF_ADPLL_SDM_DITHER_EN_2432                             RF_ADPLL_SDM_DITHER_EN_2432
#define RF_ADPLL_SDM_DITHER_EN_2432_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2432_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2432_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2432_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2432_POS)
#define RF_ADPLL_SDM_DITHER_EN_2432_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2432_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2432_POS))

/* 0x444 : lo_config_2434 */
#define RF_LO_CONFIG_2434_OFFSET                                (0x444)
#define RF_KCAL_RATIO_2434                                      RF_KCAL_RATIO_2434
#define RF_KCAL_RATIO_2434_POS                                  (0U)
#define RF_KCAL_RATIO_2434_LEN                                  (10U)
#define RF_KCAL_RATIO_2434_MSK                                  (((1U<<RF_KCAL_RATIO_2434_LEN)-1)<<RF_KCAL_RATIO_2434_POS)
#define RF_KCAL_RATIO_2434_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2434_LEN)-1)<<RF_KCAL_RATIO_2434_POS))
#define RF_ADPLL_SDM_DITHER_EN_2434                             RF_ADPLL_SDM_DITHER_EN_2434
#define RF_ADPLL_SDM_DITHER_EN_2434_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2434_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2434_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2434_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2434_POS)
#define RF_ADPLL_SDM_DITHER_EN_2434_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2434_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2434_POS))

/* 0x448 : lo_config_2436 */
#define RF_LO_CONFIG_2436_OFFSET                                (0x448)
#define RF_KCAL_RATIO_2436                                      RF_KCAL_RATIO_2436
#define RF_KCAL_RATIO_2436_POS                                  (0U)
#define RF_KCAL_RATIO_2436_LEN                                  (10U)
#define RF_KCAL_RATIO_2436_MSK                                  (((1U<<RF_KCAL_RATIO_2436_LEN)-1)<<RF_KCAL_RATIO_2436_POS)
#define RF_KCAL_RATIO_2436_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2436_LEN)-1)<<RF_KCAL_RATIO_2436_POS))
#define RF_ADPLL_SDM_DITHER_EN_2436                             RF_ADPLL_SDM_DITHER_EN_2436
#define RF_ADPLL_SDM_DITHER_EN_2436_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2436_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2436_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2436_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2436_POS)
#define RF_ADPLL_SDM_DITHER_EN_2436_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2436_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2436_POS))

/* 0x44C : lo_config_2438 */
#define RF_LO_CONFIG_2438_OFFSET                                (0x44C)
#define RF_KCAL_RATIO_2438                                      RF_KCAL_RATIO_2438
#define RF_KCAL_RATIO_2438_POS                                  (0U)
#define RF_KCAL_RATIO_2438_LEN                                  (10U)
#define RF_KCAL_RATIO_2438_MSK                                  (((1U<<RF_KCAL_RATIO_2438_LEN)-1)<<RF_KCAL_RATIO_2438_POS)
#define RF_KCAL_RATIO_2438_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2438_LEN)-1)<<RF_KCAL_RATIO_2438_POS))
#define RF_ADPLL_SDM_DITHER_EN_2438                             RF_ADPLL_SDM_DITHER_EN_2438
#define RF_ADPLL_SDM_DITHER_EN_2438_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2438_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2438_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2438_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2438_POS)
#define RF_ADPLL_SDM_DITHER_EN_2438_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2438_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2438_POS))

/* 0x450 : lo_config_2440 */
#define RF_LO_CONFIG_2440_OFFSET                                (0x450)
#define RF_KCAL_RATIO_2440                                      RF_KCAL_RATIO_2440
#define RF_KCAL_RATIO_2440_POS                                  (0U)
#define RF_KCAL_RATIO_2440_LEN                                  (10U)
#define RF_KCAL_RATIO_2440_MSK                                  (((1U<<RF_KCAL_RATIO_2440_LEN)-1)<<RF_KCAL_RATIO_2440_POS)
#define RF_KCAL_RATIO_2440_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2440_LEN)-1)<<RF_KCAL_RATIO_2440_POS))
#define RF_ADPLL_SDM_DITHER_EN_2440                             RF_ADPLL_SDM_DITHER_EN_2440
#define RF_ADPLL_SDM_DITHER_EN_2440_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2440_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2440_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2440_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2440_POS)
#define RF_ADPLL_SDM_DITHER_EN_2440_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2440_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2440_POS))

/* 0x454 : lo_config_2442 */
#define RF_LO_CONFIG_2442_OFFSET                                (0x454)
#define RF_KCAL_RATIO_2442                                      RF_KCAL_RATIO_2442
#define RF_KCAL_RATIO_2442_POS                                  (0U)
#define RF_KCAL_RATIO_2442_LEN                                  (10U)
#define RF_KCAL_RATIO_2442_MSK                                  (((1U<<RF_KCAL_RATIO_2442_LEN)-1)<<RF_KCAL_RATIO_2442_POS)
#define RF_KCAL_RATIO_2442_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2442_LEN)-1)<<RF_KCAL_RATIO_2442_POS))
#define RF_ADPLL_SDM_DITHER_EN_2442                             RF_ADPLL_SDM_DITHER_EN_2442
#define RF_ADPLL_SDM_DITHER_EN_2442_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2442_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2442_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2442_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2442_POS)
#define RF_ADPLL_SDM_DITHER_EN_2442_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2442_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2442_POS))

/* 0x458 : lo_config_2444 */
#define RF_LO_CONFIG_2444_OFFSET                                (0x458)
#define RF_KCAL_RATIO_2444                                      RF_KCAL_RATIO_2444
#define RF_KCAL_RATIO_2444_POS                                  (0U)
#define RF_KCAL_RATIO_2444_LEN                                  (10U)
#define RF_KCAL_RATIO_2444_MSK                                  (((1U<<RF_KCAL_RATIO_2444_LEN)-1)<<RF_KCAL_RATIO_2444_POS)
#define RF_KCAL_RATIO_2444_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2444_LEN)-1)<<RF_KCAL_RATIO_2444_POS))
#define RF_ADPLL_SDM_DITHER_EN_2444                             RF_ADPLL_SDM_DITHER_EN_2444
#define RF_ADPLL_SDM_DITHER_EN_2444_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2444_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2444_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2444_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2444_POS)
#define RF_ADPLL_SDM_DITHER_EN_2444_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2444_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2444_POS))

/* 0x45C : lo_config_2446 */
#define RF_LO_CONFIG_2446_OFFSET                                (0x45C)
#define RF_KCAL_RATIO_2446                                      RF_KCAL_RATIO_2446
#define RF_KCAL_RATIO_2446_POS                                  (0U)
#define RF_KCAL_RATIO_2446_LEN                                  (10U)
#define RF_KCAL_RATIO_2446_MSK                                  (((1U<<RF_KCAL_RATIO_2446_LEN)-1)<<RF_KCAL_RATIO_2446_POS)
#define RF_KCAL_RATIO_2446_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2446_LEN)-1)<<RF_KCAL_RATIO_2446_POS))
#define RF_ADPLL_SDM_DITHER_EN_2446                             RF_ADPLL_SDM_DITHER_EN_2446
#define RF_ADPLL_SDM_DITHER_EN_2446_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2446_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2446_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2446_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2446_POS)
#define RF_ADPLL_SDM_DITHER_EN_2446_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2446_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2446_POS))

/* 0x460 : lo_config_2448 */
#define RF_LO_CONFIG_2448_OFFSET                                (0x460)
#define RF_KCAL_RATIO_2448                                      RF_KCAL_RATIO_2448
#define RF_KCAL_RATIO_2448_POS                                  (0U)
#define RF_KCAL_RATIO_2448_LEN                                  (10U)
#define RF_KCAL_RATIO_2448_MSK                                  (((1U<<RF_KCAL_RATIO_2448_LEN)-1)<<RF_KCAL_RATIO_2448_POS)
#define RF_KCAL_RATIO_2448_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2448_LEN)-1)<<RF_KCAL_RATIO_2448_POS))
#define RF_ADPLL_SDM_DITHER_EN_2448                             RF_ADPLL_SDM_DITHER_EN_2448
#define RF_ADPLL_SDM_DITHER_EN_2448_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2448_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2448_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2448_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2448_POS)
#define RF_ADPLL_SDM_DITHER_EN_2448_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2448_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2448_POS))

/* 0x464 : lo_config_2450 */
#define RF_LO_CONFIG_2450_OFFSET                                (0x464)
#define RF_KCAL_RATIO_2450                                      RF_KCAL_RATIO_2450
#define RF_KCAL_RATIO_2450_POS                                  (0U)
#define RF_KCAL_RATIO_2450_LEN                                  (10U)
#define RF_KCAL_RATIO_2450_MSK                                  (((1U<<RF_KCAL_RATIO_2450_LEN)-1)<<RF_KCAL_RATIO_2450_POS)
#define RF_KCAL_RATIO_2450_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2450_LEN)-1)<<RF_KCAL_RATIO_2450_POS))
#define RF_ADPLL_SDM_DITHER_EN_2450                             RF_ADPLL_SDM_DITHER_EN_2450
#define RF_ADPLL_SDM_DITHER_EN_2450_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2450_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2450_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2450_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2450_POS)
#define RF_ADPLL_SDM_DITHER_EN_2450_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2450_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2450_POS))

/* 0x468 : lo_config_2452 */
#define RF_LO_CONFIG_2452_OFFSET                                (0x468)
#define RF_KCAL_RATIO_2452                                      RF_KCAL_RATIO_2452
#define RF_KCAL_RATIO_2452_POS                                  (0U)
#define RF_KCAL_RATIO_2452_LEN                                  (10U)
#define RF_KCAL_RATIO_2452_MSK                                  (((1U<<RF_KCAL_RATIO_2452_LEN)-1)<<RF_KCAL_RATIO_2452_POS)
#define RF_KCAL_RATIO_2452_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2452_LEN)-1)<<RF_KCAL_RATIO_2452_POS))
#define RF_ADPLL_SDM_DITHER_EN_2452                             RF_ADPLL_SDM_DITHER_EN_2452
#define RF_ADPLL_SDM_DITHER_EN_2452_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2452_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2452_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2452_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2452_POS)
#define RF_ADPLL_SDM_DITHER_EN_2452_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2452_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2452_POS))

/* 0x46C : lo_config_2454 */
#define RF_LO_CONFIG_2454_OFFSET                                (0x46C)
#define RF_KCAL_RATIO_2454                                      RF_KCAL_RATIO_2454
#define RF_KCAL_RATIO_2454_POS                                  (0U)
#define RF_KCAL_RATIO_2454_LEN                                  (10U)
#define RF_KCAL_RATIO_2454_MSK                                  (((1U<<RF_KCAL_RATIO_2454_LEN)-1)<<RF_KCAL_RATIO_2454_POS)
#define RF_KCAL_RATIO_2454_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2454_LEN)-1)<<RF_KCAL_RATIO_2454_POS))
#define RF_ADPLL_SDM_DITHER_EN_2454                             RF_ADPLL_SDM_DITHER_EN_2454
#define RF_ADPLL_SDM_DITHER_EN_2454_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2454_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2454_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2454_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2454_POS)
#define RF_ADPLL_SDM_DITHER_EN_2454_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2454_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2454_POS))

/* 0x470 : lo_config_2456 */
#define RF_LO_CONFIG_2456_OFFSET                                (0x470)
#define RF_KCAL_RATIO_2456                                      RF_KCAL_RATIO_2456
#define RF_KCAL_RATIO_2456_POS                                  (0U)
#define RF_KCAL_RATIO_2456_LEN                                  (10U)
#define RF_KCAL_RATIO_2456_MSK                                  (((1U<<RF_KCAL_RATIO_2456_LEN)-1)<<RF_KCAL_RATIO_2456_POS)
#define RF_KCAL_RATIO_2456_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2456_LEN)-1)<<RF_KCAL_RATIO_2456_POS))
#define RF_ADPLL_SDM_DITHER_EN_2456                             RF_ADPLL_SDM_DITHER_EN_2456
#define RF_ADPLL_SDM_DITHER_EN_2456_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2456_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2456_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2456_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2456_POS)
#define RF_ADPLL_SDM_DITHER_EN_2456_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2456_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2456_POS))

/* 0x474 : lo_config_2458 */
#define RF_LO_CONFIG_2458_OFFSET                                (0x474)
#define RF_KCAL_RATIO_2458                                      RF_KCAL_RATIO_2458
#define RF_KCAL_RATIO_2458_POS                                  (0U)
#define RF_KCAL_RATIO_2458_LEN                                  (10U)
#define RF_KCAL_RATIO_2458_MSK                                  (((1U<<RF_KCAL_RATIO_2458_LEN)-1)<<RF_KCAL_RATIO_2458_POS)
#define RF_KCAL_RATIO_2458_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2458_LEN)-1)<<RF_KCAL_RATIO_2458_POS))
#define RF_ADPLL_SDM_DITHER_EN_2458                             RF_ADPLL_SDM_DITHER_EN_2458
#define RF_ADPLL_SDM_DITHER_EN_2458_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2458_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2458_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2458_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2458_POS)
#define RF_ADPLL_SDM_DITHER_EN_2458_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2458_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2458_POS))

/* 0x478 : lo_config_2460 */
#define RF_LO_CONFIG_2460_OFFSET                                (0x478)
#define RF_KCAL_RATIO_2460                                      RF_KCAL_RATIO_2460
#define RF_KCAL_RATIO_2460_POS                                  (0U)
#define RF_KCAL_RATIO_2460_LEN                                  (10U)
#define RF_KCAL_RATIO_2460_MSK                                  (((1U<<RF_KCAL_RATIO_2460_LEN)-1)<<RF_KCAL_RATIO_2460_POS)
#define RF_KCAL_RATIO_2460_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2460_LEN)-1)<<RF_KCAL_RATIO_2460_POS))
#define RF_ADPLL_SDM_DITHER_EN_2460                             RF_ADPLL_SDM_DITHER_EN_2460
#define RF_ADPLL_SDM_DITHER_EN_2460_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2460_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2460_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2460_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2460_POS)
#define RF_ADPLL_SDM_DITHER_EN_2460_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2460_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2460_POS))

/* 0x47C : lo_config_2462 */
#define RF_LO_CONFIG_2462_OFFSET                                (0x47C)
#define RF_KCAL_RATIO_2462                                      RF_KCAL_RATIO_2462
#define RF_KCAL_RATIO_2462_POS                                  (0U)
#define RF_KCAL_RATIO_2462_LEN                                  (10U)
#define RF_KCAL_RATIO_2462_MSK                                  (((1U<<RF_KCAL_RATIO_2462_LEN)-1)<<RF_KCAL_RATIO_2462_POS)
#define RF_KCAL_RATIO_2462_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2462_LEN)-1)<<RF_KCAL_RATIO_2462_POS))
#define RF_ADPLL_SDM_DITHER_EN_2462                             RF_ADPLL_SDM_DITHER_EN_2462
#define RF_ADPLL_SDM_DITHER_EN_2462_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2462_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2462_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2462_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2462_POS)
#define RF_ADPLL_SDM_DITHER_EN_2462_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2462_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2462_POS))

/* 0x480 : lo_config_2464 */
#define RF_LO_CONFIG_2464_OFFSET                                (0x480)
#define RF_KCAL_RATIO_2464                                      RF_KCAL_RATIO_2464
#define RF_KCAL_RATIO_2464_POS                                  (0U)
#define RF_KCAL_RATIO_2464_LEN                                  (10U)
#define RF_KCAL_RATIO_2464_MSK                                  (((1U<<RF_KCAL_RATIO_2464_LEN)-1)<<RF_KCAL_RATIO_2464_POS)
#define RF_KCAL_RATIO_2464_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2464_LEN)-1)<<RF_KCAL_RATIO_2464_POS))
#define RF_ADPLL_SDM_DITHER_EN_2464                             RF_ADPLL_SDM_DITHER_EN_2464
#define RF_ADPLL_SDM_DITHER_EN_2464_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2464_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2464_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2464_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2464_POS)
#define RF_ADPLL_SDM_DITHER_EN_2464_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2464_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2464_POS))

/* 0x484 : lo_config_2466 */
#define RF_LO_CONFIG_2466_OFFSET                                (0x484)
#define RF_KCAL_RATIO_2466                                      RF_KCAL_RATIO_2466
#define RF_KCAL_RATIO_2466_POS                                  (0U)
#define RF_KCAL_RATIO_2466_LEN                                  (10U)
#define RF_KCAL_RATIO_2466_MSK                                  (((1U<<RF_KCAL_RATIO_2466_LEN)-1)<<RF_KCAL_RATIO_2466_POS)
#define RF_KCAL_RATIO_2466_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2466_LEN)-1)<<RF_KCAL_RATIO_2466_POS))
#define RF_ADPLL_SDM_DITHER_EN_2466                             RF_ADPLL_SDM_DITHER_EN_2466
#define RF_ADPLL_SDM_DITHER_EN_2466_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2466_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2466_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2466_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2466_POS)
#define RF_ADPLL_SDM_DITHER_EN_2466_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2466_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2466_POS))

/* 0x488 : lo_config_2468 */
#define RF_LO_CONFIG_2468_OFFSET                                (0x488)
#define RF_KCAL_RATIO_2468                                      RF_KCAL_RATIO_2468
#define RF_KCAL_RATIO_2468_POS                                  (0U)
#define RF_KCAL_RATIO_2468_LEN                                  (10U)
#define RF_KCAL_RATIO_2468_MSK                                  (((1U<<RF_KCAL_RATIO_2468_LEN)-1)<<RF_KCAL_RATIO_2468_POS)
#define RF_KCAL_RATIO_2468_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2468_LEN)-1)<<RF_KCAL_RATIO_2468_POS))
#define RF_ADPLL_SDM_DITHER_EN_2468                             RF_ADPLL_SDM_DITHER_EN_2468
#define RF_ADPLL_SDM_DITHER_EN_2468_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2468_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2468_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2468_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2468_POS)
#define RF_ADPLL_SDM_DITHER_EN_2468_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2468_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2468_POS))

/* 0x48C : lo_config_2470 */
#define RF_LO_CONFIG_2470_OFFSET                                (0x48C)
#define RF_KCAL_RATIO_2470                                      RF_KCAL_RATIO_2470
#define RF_KCAL_RATIO_2470_POS                                  (0U)
#define RF_KCAL_RATIO_2470_LEN                                  (10U)
#define RF_KCAL_RATIO_2470_MSK                                  (((1U<<RF_KCAL_RATIO_2470_LEN)-1)<<RF_KCAL_RATIO_2470_POS)
#define RF_KCAL_RATIO_2470_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2470_LEN)-1)<<RF_KCAL_RATIO_2470_POS))
#define RF_ADPLL_SDM_DITHER_EN_2470                             RF_ADPLL_SDM_DITHER_EN_2470
#define RF_ADPLL_SDM_DITHER_EN_2470_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2470_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2470_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2470_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2470_POS)
#define RF_ADPLL_SDM_DITHER_EN_2470_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2470_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2470_POS))

/* 0x490 : lo_config_2472 */
#define RF_LO_CONFIG_2472_OFFSET                                (0x490)
#define RF_KCAL_RATIO_2472                                      RF_KCAL_RATIO_2472
#define RF_KCAL_RATIO_2472_POS                                  (0U)
#define RF_KCAL_RATIO_2472_LEN                                  (10U)
#define RF_KCAL_RATIO_2472_MSK                                  (((1U<<RF_KCAL_RATIO_2472_LEN)-1)<<RF_KCAL_RATIO_2472_POS)
#define RF_KCAL_RATIO_2472_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2472_LEN)-1)<<RF_KCAL_RATIO_2472_POS))
#define RF_ADPLL_SDM_DITHER_EN_2472                             RF_ADPLL_SDM_DITHER_EN_2472
#define RF_ADPLL_SDM_DITHER_EN_2472_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2472_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2472_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2472_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2472_POS)
#define RF_ADPLL_SDM_DITHER_EN_2472_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2472_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2472_POS))

/* 0x494 : lo_config_2474 */
#define RF_LO_CONFIG_2474_OFFSET                                (0x494)
#define RF_KCAL_RATIO_2474                                      RF_KCAL_RATIO_2474
#define RF_KCAL_RATIO_2474_POS                                  (0U)
#define RF_KCAL_RATIO_2474_LEN                                  (10U)
#define RF_KCAL_RATIO_2474_MSK                                  (((1U<<RF_KCAL_RATIO_2474_LEN)-1)<<RF_KCAL_RATIO_2474_POS)
#define RF_KCAL_RATIO_2474_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2474_LEN)-1)<<RF_KCAL_RATIO_2474_POS))
#define RF_ADPLL_SDM_DITHER_EN_2474                             RF_ADPLL_SDM_DITHER_EN_2474
#define RF_ADPLL_SDM_DITHER_EN_2474_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2474_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2474_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2474_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2474_POS)
#define RF_ADPLL_SDM_DITHER_EN_2474_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2474_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2474_POS))

/* 0x498 : lo_config_2476 */
#define RF_LO_CONFIG_2476_OFFSET                                (0x498)
#define RF_KCAL_RATIO_2476                                      RF_KCAL_RATIO_2476
#define RF_KCAL_RATIO_2476_POS                                  (0U)
#define RF_KCAL_RATIO_2476_LEN                                  (10U)
#define RF_KCAL_RATIO_2476_MSK                                  (((1U<<RF_KCAL_RATIO_2476_LEN)-1)<<RF_KCAL_RATIO_2476_POS)
#define RF_KCAL_RATIO_2476_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2476_LEN)-1)<<RF_KCAL_RATIO_2476_POS))
#define RF_ADPLL_SDM_DITHER_EN_2476                             RF_ADPLL_SDM_DITHER_EN_2476
#define RF_ADPLL_SDM_DITHER_EN_2476_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2476_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2476_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2476_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2476_POS)
#define RF_ADPLL_SDM_DITHER_EN_2476_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2476_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2476_POS))

/* 0x49C : lo_config_2478 */
#define RF_LO_CONFIG_2478_OFFSET                                (0x49C)
#define RF_KCAL_RATIO_2478                                      RF_KCAL_RATIO_2478
#define RF_KCAL_RATIO_2478_POS                                  (0U)
#define RF_KCAL_RATIO_2478_LEN                                  (10U)
#define RF_KCAL_RATIO_2478_MSK                                  (((1U<<RF_KCAL_RATIO_2478_LEN)-1)<<RF_KCAL_RATIO_2478_POS)
#define RF_KCAL_RATIO_2478_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2478_LEN)-1)<<RF_KCAL_RATIO_2478_POS))
#define RF_ADPLL_SDM_DITHER_EN_2478                             RF_ADPLL_SDM_DITHER_EN_2478
#define RF_ADPLL_SDM_DITHER_EN_2478_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2478_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2478_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2478_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2478_POS)
#define RF_ADPLL_SDM_DITHER_EN_2478_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2478_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2478_POS))

/* 0x4A0 : lo_config_2480 */
#define RF_LO_CONFIG_2480_OFFSET                                (0x4A0)
#define RF_KCAL_RATIO_2480                                      RF_KCAL_RATIO_2480
#define RF_KCAL_RATIO_2480_POS                                  (0U)
#define RF_KCAL_RATIO_2480_LEN                                  (10U)
#define RF_KCAL_RATIO_2480_MSK                                  (((1U<<RF_KCAL_RATIO_2480_LEN)-1)<<RF_KCAL_RATIO_2480_POS)
#define RF_KCAL_RATIO_2480_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2480_LEN)-1)<<RF_KCAL_RATIO_2480_POS))
#define RF_ADPLL_SDM_DITHER_EN_2480                             RF_ADPLL_SDM_DITHER_EN_2480
#define RF_ADPLL_SDM_DITHER_EN_2480_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2480_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2480_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2480_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2480_POS)
#define RF_ADPLL_SDM_DITHER_EN_2480_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2480_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2480_POS))

/* 0x4A4 : lo_config_2405 */
#define RF_LO_CONFIG_2405_OFFSET                                (0x4A4)
#define RF_KCAL_RATIO_2405                                      RF_KCAL_RATIO_2405
#define RF_KCAL_RATIO_2405_POS                                  (0U)
#define RF_KCAL_RATIO_2405_LEN                                  (10U)
#define RF_KCAL_RATIO_2405_MSK                                  (((1U<<RF_KCAL_RATIO_2405_LEN)-1)<<RF_KCAL_RATIO_2405_POS)
#define RF_KCAL_RATIO_2405_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2405_LEN)-1)<<RF_KCAL_RATIO_2405_POS))
#define RF_ADPLL_SDM_DITHER_EN_2405                             RF_ADPLL_SDM_DITHER_EN_2405
#define RF_ADPLL_SDM_DITHER_EN_2405_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2405_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2405_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2405_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2405_POS)
#define RF_ADPLL_SDM_DITHER_EN_2405_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2405_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2405_POS))

/* 0x4A8 : lo_config_2415 */
#define RF_LO_CONFIG_2415_OFFSET                                (0x4A8)
#define RF_KCAL_RATIO_2415                                      RF_KCAL_RATIO_2415
#define RF_KCAL_RATIO_2415_POS                                  (0U)
#define RF_KCAL_RATIO_2415_LEN                                  (10U)
#define RF_KCAL_RATIO_2415_MSK                                  (((1U<<RF_KCAL_RATIO_2415_LEN)-1)<<RF_KCAL_RATIO_2415_POS)
#define RF_KCAL_RATIO_2415_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2415_LEN)-1)<<RF_KCAL_RATIO_2415_POS))
#define RF_ADPLL_SDM_DITHER_EN_2415                             RF_ADPLL_SDM_DITHER_EN_2415
#define RF_ADPLL_SDM_DITHER_EN_2415_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2415_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2415_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2415_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2415_POS)
#define RF_ADPLL_SDM_DITHER_EN_2415_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2415_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2415_POS))

/* 0x4AC : lo_config_2425 */
#define RF_LO_CONFIG_2425_OFFSET                                (0x4AC)
#define RF_KCAL_RATIO_2425                                      RF_KCAL_RATIO_2425
#define RF_KCAL_RATIO_2425_POS                                  (0U)
#define RF_KCAL_RATIO_2425_LEN                                  (10U)
#define RF_KCAL_RATIO_2425_MSK                                  (((1U<<RF_KCAL_RATIO_2425_LEN)-1)<<RF_KCAL_RATIO_2425_POS)
#define RF_KCAL_RATIO_2425_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2425_LEN)-1)<<RF_KCAL_RATIO_2425_POS))
#define RF_ADPLL_SDM_DITHER_EN_2425                             RF_ADPLL_SDM_DITHER_EN_2425
#define RF_ADPLL_SDM_DITHER_EN_2425_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2425_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2425_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2425_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2425_POS)
#define RF_ADPLL_SDM_DITHER_EN_2425_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2425_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2425_POS))

/* 0x4B0 : lo_config_2435 */
#define RF_LO_CONFIG_2435_OFFSET                                (0x4B0)
#define RF_KCAL_RATIO_2435                                      RF_KCAL_RATIO_2435
#define RF_KCAL_RATIO_2435_POS                                  (0U)
#define RF_KCAL_RATIO_2435_LEN                                  (10U)
#define RF_KCAL_RATIO_2435_MSK                                  (((1U<<RF_KCAL_RATIO_2435_LEN)-1)<<RF_KCAL_RATIO_2435_POS)
#define RF_KCAL_RATIO_2435_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2435_LEN)-1)<<RF_KCAL_RATIO_2435_POS))
#define RF_ADPLL_SDM_DITHER_EN_2435                             RF_ADPLL_SDM_DITHER_EN_2435
#define RF_ADPLL_SDM_DITHER_EN_2435_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2435_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2435_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2435_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2435_POS)
#define RF_ADPLL_SDM_DITHER_EN_2435_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2435_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2435_POS))

/* 0x4B4 : lo_config_2445 */
#define RF_LO_CONFIG_2445_OFFSET                                (0x4B4)
#define RF_KCAL_RATIO_2445                                      RF_KCAL_RATIO_2445
#define RF_KCAL_RATIO_2445_POS                                  (0U)
#define RF_KCAL_RATIO_2445_LEN                                  (10U)
#define RF_KCAL_RATIO_2445_MSK                                  (((1U<<RF_KCAL_RATIO_2445_LEN)-1)<<RF_KCAL_RATIO_2445_POS)
#define RF_KCAL_RATIO_2445_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2445_LEN)-1)<<RF_KCAL_RATIO_2445_POS))
#define RF_ADPLL_SDM_DITHER_EN_2445                             RF_ADPLL_SDM_DITHER_EN_2445
#define RF_ADPLL_SDM_DITHER_EN_2445_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2445_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2445_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2445_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2445_POS)
#define RF_ADPLL_SDM_DITHER_EN_2445_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2445_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2445_POS))

/* 0x4B8 : lo_config_2455 */
#define RF_LO_CONFIG_2455_OFFSET                                (0x4B8)
#define RF_KCAL_RATIO_2455                                      RF_KCAL_RATIO_2455
#define RF_KCAL_RATIO_2455_POS                                  (0U)
#define RF_KCAL_RATIO_2455_LEN                                  (10U)
#define RF_KCAL_RATIO_2455_MSK                                  (((1U<<RF_KCAL_RATIO_2455_LEN)-1)<<RF_KCAL_RATIO_2455_POS)
#define RF_KCAL_RATIO_2455_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2455_LEN)-1)<<RF_KCAL_RATIO_2455_POS))
#define RF_ADPLL_SDM_DITHER_EN_2455                             RF_ADPLL_SDM_DITHER_EN_2455
#define RF_ADPLL_SDM_DITHER_EN_2455_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2455_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2455_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2455_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2455_POS)
#define RF_ADPLL_SDM_DITHER_EN_2455_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2455_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2455_POS))

/* 0x4BC : lo_config_2465 */
#define RF_LO_CONFIG_2465_OFFSET                                (0x4BC)
#define RF_KCAL_RATIO_2465                                      RF_KCAL_RATIO_2465
#define RF_KCAL_RATIO_2465_POS                                  (0U)
#define RF_KCAL_RATIO_2465_LEN                                  (10U)
#define RF_KCAL_RATIO_2465_MSK                                  (((1U<<RF_KCAL_RATIO_2465_LEN)-1)<<RF_KCAL_RATIO_2465_POS)
#define RF_KCAL_RATIO_2465_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2465_LEN)-1)<<RF_KCAL_RATIO_2465_POS))
#define RF_ADPLL_SDM_DITHER_EN_2465                             RF_ADPLL_SDM_DITHER_EN_2465
#define RF_ADPLL_SDM_DITHER_EN_2465_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2465_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2465_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2465_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2465_POS)
#define RF_ADPLL_SDM_DITHER_EN_2465_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2465_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2465_POS))

/* 0x4C0 : lo_config_2475 */
#define RF_LO_CONFIG_2475_OFFSET                                (0x4C0)
#define RF_KCAL_RATIO_2475                                      RF_KCAL_RATIO_2475
#define RF_KCAL_RATIO_2475_POS                                  (0U)
#define RF_KCAL_RATIO_2475_LEN                                  (10U)
#define RF_KCAL_RATIO_2475_MSK                                  (((1U<<RF_KCAL_RATIO_2475_LEN)-1)<<RF_KCAL_RATIO_2475_POS)
#define RF_KCAL_RATIO_2475_UMSK                                 (~(((1U<<RF_KCAL_RATIO_2475_LEN)-1)<<RF_KCAL_RATIO_2475_POS))
#define RF_ADPLL_SDM_DITHER_EN_2475                             RF_ADPLL_SDM_DITHER_EN_2475
#define RF_ADPLL_SDM_DITHER_EN_2475_POS                         (12U)
#define RF_ADPLL_SDM_DITHER_EN_2475_LEN                         (1U)
#define RF_ADPLL_SDM_DITHER_EN_2475_MSK                         (((1U<<RF_ADPLL_SDM_DITHER_EN_2475_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2475_POS)
#define RF_ADPLL_SDM_DITHER_EN_2475_UMSK                        (~(((1U<<RF_ADPLL_SDM_DITHER_EN_2475_LEN)-1)<<RF_ADPLL_SDM_DITHER_EN_2475_POS))

/* 0x500 : dg_testbus_0 */
#define RF_DG_TESTBUS_0_OFFSET                                  (0x500)
#define RF_TESTBUS_READ                                         RF_TESTBUS_READ
#define RF_TESTBUS_READ_POS                                     (0U)
#define RF_TESTBUS_READ_LEN                                     (32U)
#define RF_TESTBUS_READ_MSK                                     (((1U<<RF_TESTBUS_READ_LEN)-1)<<RF_TESTBUS_READ_POS)
#define RF_TESTBUS_READ_UMSK                                    (~(((1U<<RF_TESTBUS_READ_LEN)-1)<<RF_TESTBUS_READ_POS))

/* 0x504 : dg_testbus_1 */
#define RF_DG_TESTBUS_1_OFFSET                                  (0x504)
#define RF_TESTBUS_SEL                                          RF_TESTBUS_SEL
#define RF_TESTBUS_SEL_POS                                      (0U)
#define RF_TESTBUS_SEL_LEN                                      (4U)
#define RF_TESTBUS_SEL_MSK                                      (((1U<<RF_TESTBUS_SEL_LEN)-1)<<RF_TESTBUS_SEL_POS)
#define RF_TESTBUS_SEL_UMSK                                     (~(((1U<<RF_TESTBUS_SEL_LEN)-1)<<RF_TESTBUS_SEL_POS))

/* 0x508 : dg_ppud_0 */
#define RF_DG_PPUD_0_OFFSET                                     (0x508)
#define RF_PPUD_CNT2                                            RF_PPUD_CNT2
#define RF_PPUD_CNT2_POS                                        (16U)
#define RF_PPUD_CNT2_LEN                                        (9U)
#define RF_PPUD_CNT2_MSK                                        (((1U<<RF_PPUD_CNT2_LEN)-1)<<RF_PPUD_CNT2_POS)
#define RF_PPUD_CNT2_UMSK                                       (~(((1U<<RF_PPUD_CNT2_LEN)-1)<<RF_PPUD_CNT2_POS))
#define RF_PPUD_CNT1                                            RF_PPUD_CNT1
#define RF_PPUD_CNT1_POS                                        (25U)
#define RF_PPUD_CNT1_LEN                                        (5U)
#define RF_PPUD_CNT1_MSK                                        (((1U<<RF_PPUD_CNT1_LEN)-1)<<RF_PPUD_CNT1_POS)
#define RF_PPUD_CNT1_UMSK                                       (~(((1U<<RF_PPUD_CNT1_LEN)-1)<<RF_PPUD_CNT1_POS))
#define RF_PPUD_MANAUAL_EN                                      RF_PPUD_MANAUAL_EN
#define RF_PPUD_MANAUAL_EN_POS                                  (30U)
#define RF_PPUD_MANAUAL_EN_LEN                                  (1U)
#define RF_PPUD_MANAUAL_EN_MSK                                  (((1U<<RF_PPUD_MANAUAL_EN_LEN)-1)<<RF_PPUD_MANAUAL_EN_POS)
#define RF_PPUD_MANAUAL_EN_UMSK                                 (~(((1U<<RF_PPUD_MANAUAL_EN_LEN)-1)<<RF_PPUD_MANAUAL_EN_POS))

/* 0x50C : rf_top */
#define RF_TOP_OFFSET                                           (0x50C)
#define RFCKG_AFIFO_ADPLL_INV                                   RFCKG_AFIFO_ADPLL_INV
#define RFCKG_AFIFO_ADPLL_INV_POS                               (0U)
#define RFCKG_AFIFO_ADPLL_INV_LEN                               (1U)
#define RFCKG_AFIFO_ADPLL_INV_MSK                               (((1U<<RFCKG_AFIFO_ADPLL_INV_LEN)-1)<<RFCKG_AFIFO_ADPLL_INV_POS)
#define RFCKG_AFIFO_ADPLL_INV_UMSK                              (~(((1U<<RFCKG_AFIFO_ADPLL_INV_LEN)-1)<<RFCKG_AFIFO_ADPLL_INV_POS))
#define RFCKG_AFIFO_RXADC_INV                                   RFCKG_AFIFO_RXADC_INV
#define RFCKG_AFIFO_RXADC_INV_POS                               (1U)
#define RFCKG_AFIFO_RXADC_INV_LEN                               (1U)
#define RFCKG_AFIFO_RXADC_INV_MSK                               (((1U<<RFCKG_AFIFO_RXADC_INV_LEN)-1)<<RFCKG_AFIFO_RXADC_INV_POS)
#define RFCKG_AFIFO_RXADC_INV_UMSK                              (~(((1U<<RFCKG_AFIFO_RXADC_INV_LEN)-1)<<RFCKG_AFIFO_RXADC_INV_POS))
#define RFCKG_AFIFO_TX_INV                                      RFCKG_AFIFO_TX_INV
#define RFCKG_AFIFO_TX_INV_POS                                  (2U)
#define RFCKG_AFIFO_TX_INV_LEN                                  (1U)
#define RFCKG_AFIFO_TX_INV_MSK                                  (((1U<<RFCKG_AFIFO_TX_INV_LEN)-1)<<RFCKG_AFIFO_TX_INV_POS)
#define RFCKG_AFIFO_TX_INV_UMSK                                 (~(((1U<<RFCKG_AFIFO_TX_INV_LEN)-1)<<RFCKG_AFIFO_TX_INV_POS))
#define RF_MAC_LO_TIME_OFFSET                                   RF_MAC_LO_TIME_OFFSET
#define RF_MAC_LO_TIME_OFFSET_POS                               (16U)
#define RF_MAC_LO_TIME_OFFSET_LEN                               (6U)
#define RF_MAC_LO_TIME_OFFSET_MSK                               (((1U<<RF_MAC_LO_TIME_OFFSET_LEN)-1)<<RF_MAC_LO_TIME_OFFSET_POS)
#define RF_MAC_LO_TIME_OFFSET_UMSK                              (~(((1U<<RF_MAC_LO_TIME_OFFSET_LEN)-1)<<RF_MAC_LO_TIME_OFFSET_POS))
#define RF_RX_MODE_HW                                           RF_RX_MODE_HW
#define RF_RX_MODE_HW_POS                                       (22U)
#define RF_RX_MODE_HW_LEN                                       (2U)
#define RF_RX_MODE_HW_MSK                                       (((1U<<RF_RX_MODE_HW_LEN)-1)<<RF_RX_MODE_HW_POS)
#define RF_RX_MODE_HW_UMSK                                      (~(((1U<<RF_RX_MODE_HW_LEN)-1)<<RF_RX_MODE_HW_POS))
#define RF_RX_MODE_4S                                           RF_RX_MODE_4S
#define RF_RX_MODE_4S_POS                                       (24U)
#define RF_RX_MODE_4S_LEN                                       (2U)
#define RF_RX_MODE_4S_MSK                                       (((1U<<RF_RX_MODE_4S_LEN)-1)<<RF_RX_MODE_4S_POS)
#define RF_RX_MODE_4S_UMSK                                      (~(((1U<<RF_RX_MODE_4S_LEN)-1)<<RF_RX_MODE_4S_POS))
#define RF_RX_MODE_4S_EN                                        RF_RX_MODE_4S_EN
#define RF_RX_MODE_4S_EN_POS                                    (26U)
#define RF_RX_MODE_4S_EN_LEN                                    (1U)
#define RF_RX_MODE_4S_EN_MSK                                    (((1U<<RF_RX_MODE_4S_EN_LEN)-1)<<RF_RX_MODE_4S_EN_POS)
#define RF_RX_MODE_4S_EN_UMSK                                   (~(((1U<<RF_RX_MODE_4S_EN_LEN)-1)<<RF_RX_MODE_4S_EN_POS))
#define RF_RX_EN_4S                                             RF_RX_EN_4S
#define RF_RX_EN_4S_POS                                         (28U)
#define RF_RX_EN_4S_LEN                                         (1U)
#define RF_RX_EN_4S_MSK                                         (((1U<<RF_RX_EN_4S_LEN)-1)<<RF_RX_EN_4S_POS)
#define RF_RX_EN_4S_UMSK                                        (~(((1U<<RF_RX_EN_4S_LEN)-1)<<RF_RX_EN_4S_POS))
#define RF_RX_EN_SRC                                            RF_RX_EN_SRC
#define RF_RX_EN_SRC_POS                                        (29U)
#define RF_RX_EN_SRC_LEN                                        (1U)
#define RF_RX_EN_SRC_MSK                                        (((1U<<RF_RX_EN_SRC_LEN)-1)<<RF_RX_EN_SRC_POS)
#define RF_RX_EN_SRC_UMSK                                       (~(((1U<<RF_RX_EN_SRC_LEN)-1)<<RF_RX_EN_SRC_POS))
#define RF_TX_EN_4S                                             RF_TX_EN_4S
#define RF_TX_EN_4S_POS                                         (30U)
#define RF_TX_EN_4S_LEN                                         (1U)
#define RF_TX_EN_4S_MSK                                         (((1U<<RF_TX_EN_4S_LEN)-1)<<RF_TX_EN_4S_POS)
#define RF_TX_EN_4S_UMSK                                        (~(((1U<<RF_TX_EN_4S_LEN)-1)<<RF_TX_EN_4S_POS))
#define RF_TX_EN_SRC                                            RF_TX_EN_SRC
#define RF_TX_EN_SRC_POS                                        (31U)
#define RF_TX_EN_SRC_LEN                                        (1U)
#define RF_TX_EN_SRC_MSK                                        (((1U<<RF_TX_EN_SRC_LEN)-1)<<RF_TX_EN_SRC_POS)
#define RF_TX_EN_SRC_UMSK                                       (~(((1U<<RF_TX_EN_SRC_LEN)-1)<<RF_TX_EN_SRC_POS))

/* 0x510 : rf_fsm */
#define RF_FSM_OFFSET                                           (0x510)
#define RF_FSM_LO_TIME                                          RF_FSM_LO_TIME
#define RF_FSM_LO_TIME_POS                                      (0U)
#define RF_FSM_LO_TIME_LEN                                      (12U)
#define RF_FSM_LO_TIME_MSK                                      (((1U<<RF_FSM_LO_TIME_LEN)-1)<<RF_FSM_LO_TIME_POS)
#define RF_FSM_LO_TIME_UMSK                                     (~(((1U<<RF_FSM_LO_TIME_LEN)-1)<<RF_FSM_LO_TIME_POS))
#define RF_FSM_STATE                                            RF_FSM_STATE
#define RF_FSM_STATE_POS                                        (12U)
#define RF_FSM_STATE_LEN                                        (3U)
#define RF_FSM_STATE_MSK                                        (((1U<<RF_FSM_STATE_LEN)-1)<<RF_FSM_STATE_POS)
#define RF_FSM_STATE_UMSK                                       (~(((1U<<RF_FSM_STATE_LEN)-1)<<RF_FSM_STATE_POS))
#define RF_FSM_ST_4S                                            RF_FSM_ST_4S
#define RF_FSM_ST_4S_POS                                        (16U)
#define RF_FSM_ST_4S_LEN                                        (3U)
#define RF_FSM_ST_4S_MSK                                        (((1U<<RF_FSM_ST_4S_LEN)-1)<<RF_FSM_ST_4S_POS)
#define RF_FSM_ST_4S_UMSK                                       (~(((1U<<RF_FSM_ST_4S_LEN)-1)<<RF_FSM_ST_4S_POS))
#define RF_FSM_ST_4S_EN                                         RF_FSM_ST_4S_EN
#define RF_FSM_ST_4S_EN_POS                                     (20U)
#define RF_FSM_ST_4S_EN_LEN                                     (1U)
#define RF_FSM_ST_4S_EN_MSK                                     (((1U<<RF_FSM_ST_4S_EN_LEN)-1)<<RF_FSM_ST_4S_EN_POS)
#define RF_FSM_ST_4S_EN_UMSK                                    (~(((1U<<RF_FSM_ST_4S_EN_LEN)-1)<<RF_FSM_ST_4S_EN_POS))
#define RF_FSM_EN                                               RF_FSM_EN
#define RF_FSM_EN_POS                                           (21U)
#define RF_FSM_EN_LEN                                           (1U)
#define RF_FSM_EN_MSK                                           (((1U<<RF_FSM_EN_LEN)-1)<<RF_FSM_EN_POS)
#define RF_FSM_EN_UMSK                                          (~(((1U<<RF_FSM_EN_LEN)-1)<<RF_FSM_EN_POS))
#define RF_FSM_RX_AFIFO_4S                                      RF_FSM_RX_AFIFO_4S
#define RF_FSM_RX_AFIFO_4S_POS                                  (22U)
#define RF_FSM_RX_AFIFO_4S_LEN                                  (1U)
#define RF_FSM_RX_AFIFO_4S_MSK                                  (((1U<<RF_FSM_RX_AFIFO_4S_LEN)-1)<<RF_FSM_RX_AFIFO_4S_POS)
#define RF_FSM_RX_AFIFO_4S_UMSK                                 (~(((1U<<RF_FSM_RX_AFIFO_4S_LEN)-1)<<RF_FSM_RX_AFIFO_4S_POS))
#define RF_FSM_RX_AFIFO_4S_EN                                   RF_FSM_RX_AFIFO_4S_EN
#define RF_FSM_RX_AFIFO_4S_EN_POS                               (23U)
#define RF_FSM_RX_AFIFO_4S_EN_LEN                               (1U)
#define RF_FSM_RX_AFIFO_4S_EN_MSK                               (((1U<<RF_FSM_RX_AFIFO_4S_EN_LEN)-1)<<RF_FSM_RX_AFIFO_4S_EN_POS)
#define RF_FSM_RX_AFIFO_4S_EN_UMSK                              (~(((1U<<RF_FSM_RX_AFIFO_4S_EN_LEN)-1)<<RF_FSM_RX_AFIFO_4S_EN_POS))
#define RF_FSM_TX_AFIFO_4S                                      RF_FSM_TX_AFIFO_4S
#define RF_FSM_TX_AFIFO_4S_POS                                  (24U)
#define RF_FSM_TX_AFIFO_4S_LEN                                  (1U)
#define RF_FSM_TX_AFIFO_4S_MSK                                  (((1U<<RF_FSM_TX_AFIFO_4S_LEN)-1)<<RF_FSM_TX_AFIFO_4S_POS)
#define RF_FSM_TX_AFIFO_4S_UMSK                                 (~(((1U<<RF_FSM_TX_AFIFO_4S_LEN)-1)<<RF_FSM_TX_AFIFO_4S_POS))
#define RF_FSM_TX_AFIFO_4S_EN                                   RF_FSM_TX_AFIFO_4S_EN
#define RF_FSM_TX_AFIFO_4S_EN_POS                               (25U)
#define RF_FSM_TX_AFIFO_4S_EN_LEN                               (1U)
#define RF_FSM_TX_AFIFO_4S_EN_MSK                               (((1U<<RF_FSM_TX_AFIFO_4S_EN_LEN)-1)<<RF_FSM_TX_AFIFO_4S_EN_POS)
#define RF_FSM_TX_AFIFO_4S_EN_UMSK                              (~(((1U<<RF_FSM_TX_AFIFO_4S_EN_LEN)-1)<<RF_FSM_TX_AFIFO_4S_EN_POS))
#define RF_FSM_AFIFO_DLY_TIME                                   RF_FSM_AFIFO_DLY_TIME
#define RF_FSM_AFIFO_DLY_TIME_POS                               (26U)
#define RF_FSM_AFIFO_DLY_TIME_LEN                               (6U)
#define RF_FSM_AFIFO_DLY_TIME_MSK                               (((1U<<RF_FSM_AFIFO_DLY_TIME_LEN)-1)<<RF_FSM_AFIFO_DLY_TIME_POS)
#define RF_FSM_AFIFO_DLY_TIME_UMSK                              (~(((1U<<RF_FSM_AFIFO_DLY_TIME_LEN)-1)<<RF_FSM_AFIFO_DLY_TIME_POS))

/* 0x514 : rf_singen_0 */
#define RF_SINGEN_0_OFFSET                                      (0x514)
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

/* 0x518 : rf_singen_1 */
#define RF_SINGEN_1_OFFSET                                      (0x518)
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

/* 0x51C : rf_singen_2 */
#define RF_SINGEN_2_OFFSET                                      (0x51C)
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

/* 0x520 : rf_singen_3 */
#define RF_SINGEN_3_OFFSET                                      (0x520)
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

/* 0x524 : rf_singen_4 */
#define RF_SINGEN_4_OFFSET                                      (0x524)
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

/* 0x528 : rf_sram_ctrl0 */
#define RF_SRAM_CTRL0_OFFSET                                    (0x528)
#define RF_SRAM_DONE                                            RF_SRAM_DONE
#define RF_SRAM_DONE_POS                                        (0U)
#define RF_SRAM_DONE_LEN                                        (1U)
#define RF_SRAM_DONE_MSK                                        (((1U<<RF_SRAM_DONE_LEN)-1)<<RF_SRAM_DONE_POS)
#define RF_SRAM_DONE_UMSK                                       (~(((1U<<RF_SRAM_DONE_LEN)-1)<<RF_SRAM_DONE_POS))
#define RF_SRAM_EN                                              RF_SRAM_EN
#define RF_SRAM_EN_POS                                          (1U)
#define RF_SRAM_EN_LEN                                          (1U)
#define RF_SRAM_EN_MSK                                          (((1U<<RF_SRAM_EN_LEN)-1)<<RF_SRAM_EN_POS)
#define RF_SRAM_EN_UMSK                                         (~(((1U<<RF_SRAM_EN_LEN)-1)<<RF_SRAM_EN_POS))
#define RF_SRAM_LOOP_EN                                         RF_SRAM_LOOP_EN
#define RF_SRAM_LOOP_EN_POS                                     (2U)
#define RF_SRAM_LOOP_EN_LEN                                     (1U)
#define RF_SRAM_LOOP_EN_MSK                                     (((1U<<RF_SRAM_LOOP_EN_LEN)-1)<<RF_SRAM_LOOP_EN_POS)
#define RF_SRAM_LOOP_EN_UMSK                                    (~(((1U<<RF_SRAM_LOOP_EN_LEN)-1)<<RF_SRAM_LOOP_EN_POS))
#define RF_SRAM_STS_CLR                                         RF_SRAM_STS_CLR
#define RF_SRAM_STS_CLR_POS                                     (3U)
#define RF_SRAM_STS_CLR_LEN                                     (1U)
#define RF_SRAM_STS_CLR_MSK                                     (((1U<<RF_SRAM_STS_CLR_LEN)-1)<<RF_SRAM_STS_CLR_POS)
#define RF_SRAM_STS_CLR_UMSK                                    (~(((1U<<RF_SRAM_STS_CLR_LEN)-1)<<RF_SRAM_STS_CLR_POS))
#define RF_SRAM_MODE                                            RF_SRAM_MODE
#define RF_SRAM_MODE_POS                                        (4U)
#define RF_SRAM_MODE_LEN                                        (2U)
#define RF_SRAM_MODE_MSK                                        (((1U<<RF_SRAM_MODE_LEN)-1)<<RF_SRAM_MODE_POS)
#define RF_SRAM_MODE_UMSK                                       (~(((1U<<RF_SRAM_MODE_LEN)-1)<<RF_SRAM_MODE_POS))
#define RF_SRAM_DONE_CNT                                        RF_SRAM_DONE_CNT
#define RF_SRAM_DONE_CNT_POS                                    (16U)
#define RF_SRAM_DONE_CNT_LEN                                    (16U)
#define RF_SRAM_DONE_CNT_MSK                                    (((1U<<RF_SRAM_DONE_CNT_LEN)-1)<<RF_SRAM_DONE_CNT_POS)
#define RF_SRAM_DONE_CNT_UMSK                                   (~(((1U<<RF_SRAM_DONE_CNT_LEN)-1)<<RF_SRAM_DONE_CNT_POS))

/* 0x52C : rf_sram_ctrl1 */
#define RF_SRAM_CTRL1_OFFSET                                    (0x52C)
#define RF_SRAM_ADDR_END                                        RF_SRAM_ADDR_END
#define RF_SRAM_ADDR_END_POS                                    (0U)
#define RF_SRAM_ADDR_END_LEN                                    (16U)
#define RF_SRAM_ADDR_END_MSK                                    (((1U<<RF_SRAM_ADDR_END_LEN)-1)<<RF_SRAM_ADDR_END_POS)
#define RF_SRAM_ADDR_END_UMSK                                   (~(((1U<<RF_SRAM_ADDR_END_LEN)-1)<<RF_SRAM_ADDR_END_POS))
#define RF_SRAM_ADDR_START                                      RF_SRAM_ADDR_START
#define RF_SRAM_ADDR_START_POS                                  (16U)
#define RF_SRAM_ADDR_START_LEN                                  (16U)
#define RF_SRAM_ADDR_START_MSK                                  (((1U<<RF_SRAM_ADDR_START_LEN)-1)<<RF_SRAM_ADDR_START_POS)
#define RF_SRAM_ADDR_START_UMSK                                 (~(((1U<<RF_SRAM_ADDR_START_LEN)-1)<<RF_SRAM_ADDR_START_POS))

/* 0x530 : rf_sram_ctrl2 */
#define RF_SRAM_CTRL2_OFFSET                                    (0x530)
#define RF_SRAM_STS                                             RF_SRAM_STS
#define RF_SRAM_STS_POS                                         (0U)
#define RF_SRAM_STS_LEN                                         (32U)
#define RF_SRAM_STS_MSK                                         (((1U<<RF_SRAM_STS_LEN)-1)<<RF_SRAM_STS_POS)
#define RF_SRAM_STS_UMSK                                        (~(((1U<<RF_SRAM_STS_LEN)-1)<<RF_SRAM_STS_POS))

/* 0x534 : rf_test_mode */
#define RF_TEST_MODE_OFFSET                                     (0x534)
#define RF_DACOUT_HW                                            RF_DACOUT_HW
#define RF_DACOUT_HW_POS                                        (0U)
#define RF_DACOUT_HW_LEN                                        (12U)
#define RF_DACOUT_HW_MSK                                        (((1U<<RF_DACOUT_HW_LEN)-1)<<RF_DACOUT_HW_POS)
#define RF_DACOUT_HW_UMSK                                       (~(((1U<<RF_DACOUT_HW_LEN)-1)<<RF_DACOUT_HW_POS))
#define RF_DACOUT_4S                                            RF_DACOUT_4S
#define RF_DACOUT_4S_POS                                        (16U)
#define RF_DACOUT_4S_LEN                                        (12U)
#define RF_DACOUT_4S_MSK                                        (((1U<<RF_DACOUT_4S_LEN)-1)<<RF_DACOUT_4S_POS)
#define RF_DACOUT_4S_UMSK                                       (~(((1U<<RF_DACOUT_4S_LEN)-1)<<RF_DACOUT_4S_POS))
#define RF_DACOUT_4S_EN                                         RF_DACOUT_4S_EN
#define RF_DACOUT_4S_EN_POS                                     (28U)
#define RF_DACOUT_4S_EN_LEN                                     (1U)
#define RF_DACOUT_4S_EN_MSK                                     (((1U<<RF_DACOUT_4S_EN_LEN)-1)<<RF_DACOUT_4S_EN_POS)
#define RF_DACOUT_4S_EN_UMSK                                    (~(((1U<<RF_DACOUT_4S_EN_LEN)-1)<<RF_DACOUT_4S_EN_POS))
#define RF_DACOUT_4S_SRAM_EN                                    RF_DACOUT_4S_SRAM_EN
#define RF_DACOUT_4S_SRAM_EN_POS                                (29U)
#define RF_DACOUT_4S_SRAM_EN_LEN                                (1U)
#define RF_DACOUT_4S_SRAM_EN_MSK                                (((1U<<RF_DACOUT_4S_SRAM_EN_LEN)-1)<<RF_DACOUT_4S_SRAM_EN_POS)
#define RF_DACOUT_4S_SRAM_EN_UMSK                               (~(((1U<<RF_DACOUT_4S_SRAM_EN_LEN)-1)<<RF_DACOUT_4S_SRAM_EN_POS))
#define RF_TEST_MODE_EN                                         RF_TEST_MODE_EN
#define RF_TEST_MODE_EN_POS                                     (30U)
#define RF_TEST_MODE_EN_LEN                                     (1U)
#define RF_TEST_MODE_EN_MSK                                     (((1U<<RF_TEST_MODE_EN_LEN)-1)<<RF_TEST_MODE_EN_POS)
#define RF_TEST_MODE_EN_UMSK                                    (~(((1U<<RF_TEST_MODE_EN_LEN)-1)<<RF_TEST_MODE_EN_POS))

/* 0x538 : rf_rx_pulse_filter */
#define RF_RX_PULSE_FILTER_OFFSET                               (0x538)
#define RF_PF_TH2                                               RF_PF_TH2
#define RF_PF_TH2_POS                                           (0U)
#define RF_PF_TH2_LEN                                           (9U)
#define RF_PF_TH2_MSK                                           (((1U<<RF_PF_TH2_LEN)-1)<<RF_PF_TH2_POS)
#define RF_PF_TH2_UMSK                                          (~(((1U<<RF_PF_TH2_LEN)-1)<<RF_PF_TH2_POS))
#define RF_PF_TH1                                               RF_PF_TH1
#define RF_PF_TH1_POS                                           (10U)
#define RF_PF_TH1_LEN                                           (9U)
#define RF_PF_TH1_MSK                                           (((1U<<RF_PF_TH1_LEN)-1)<<RF_PF_TH1_POS)
#define RF_PF_TH1_UMSK                                          (~(((1U<<RF_PF_TH1_LEN)-1)<<RF_PF_TH1_POS))
#define RF_PF_EN_Q                                              RF_PF_EN_Q
#define RF_PF_EN_Q_POS                                          (20U)
#define RF_PF_EN_Q_LEN                                          (1U)
#define RF_PF_EN_Q_MSK                                          (((1U<<RF_PF_EN_Q_LEN)-1)<<RF_PF_EN_Q_POS)
#define RF_PF_EN_Q_UMSK                                         (~(((1U<<RF_PF_EN_Q_LEN)-1)<<RF_PF_EN_Q_POS))
#define RF_PF_EN_I                                              RF_PF_EN_I
#define RF_PF_EN_I_POS                                          (21U)
#define RF_PF_EN_I_LEN                                          (1U)
#define RF_PF_EN_I_MSK                                          (((1U<<RF_PF_EN_I_LEN)-1)<<RF_PF_EN_I_POS)
#define RF_PF_EN_I_UMSK                                         (~(((1U<<RF_PF_EN_I_LEN)-1)<<RF_PF_EN_I_POS))


struct  rf_reg {
    /* 0x0 : Silicon revision */
    union {
        struct {
            uint32_t rf_rev                         :  8; /* [ 7: 0],          r,        0x7 */
            uint32_t fw_rev                         :  8; /* [15: 8],          r,        0x0 */
            uint32_t hw_rev                         :  8; /* [23:16],          r,        0x7 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_rev;

    /* 0x4 : dsp_readback */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t ch_ind_hw                      :  7; /* [ 8: 2],          r,        0x0 */
            uint32_t rbb_ind_hw                     :  5; /* [13: 9],          r,        0x0 */
            uint32_t rbb_bw_ind_hw                  :  1; /* [   14],          r,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ch_ind                         :  7; /* [22:16],        r/w,        0x0 */
            uint32_t ch_ind_ctrl_hw                 :  1; /* [   23],        r/w,        0x1 */
            uint32_t rbb_ind                        :  5; /* [28:24],        r/w,        0x0 */
            uint32_t rbb_ind_ctrl_hw                :  1; /* [   29],        r/w,        0x1 */
            uint32_t rbb_bw_ind                     :  1; /* [   30],        r/w,        0x0 */
            uint32_t rbb_bw_ind_ctrl_hw             :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } dsp_readback;

    /* 0x8 : Control logic switch */
    union {
        struct {
            uint32_t pu_ctrl_hw                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_2                   :  2; /* [ 2: 1],       rsvd,        0x0 */
            uint32_t gain_ctrl_tx_hw                :  1; /* [    3],        r/w,        0x1 */
            uint32_t gain_ctrl_rx_hw                :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rosdac_ctrl_hw                 :  1; /* [    8],        r/w,        0x1 */
            uint32_t rosdac_ctrl_rccal              :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t kcal_ratio_ctrl_hw             :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rbb_bw_ctrl_hw                 :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t lo_fcw_ctrl_hw                 :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t inc_fcal_en_ctrl_hw            :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t vco_idac_ctrl_hw               :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ctrl_source;

    /* 0xC : rf calibration state enable in full cal list */
    union {
        struct {
            uint32_t fcal_state_en                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t acal_state_en                  :  1; /* [    1],        r/w,        0x1 */
            uint32_t kcal_state_en                  :  1; /* [    2],        r/w,        0x1 */
            uint32_t roscal_state_en                :  1; /* [    3],        r/w,        0x1 */
            uint32_t rccal_state_en                 :  1; /* [    4],        r/w,        0x1 */
            uint32_t inc_fcal_state_en              :  1; /* [    5],        r/w,        0x1 */
            uint32_t inc_acal_state_en              :  1; /* [    6],        r/w,        0x1 */
            uint32_t inc_roscal_state_en            :  1; /* [    7],        r/w,        0x1 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_cal_state_ctrl;

    /* 0x10 : Calibration mode register */
    union {
        struct {
            uint32_t acal_en                        :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t kcal_en                        :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rccal_en                       :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t inc_acal_en                    :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t inc_fcal_en                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t inc_fcal_en_hw                 :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_cal_switch_ctrl;

    /* 0x14 : rf_cal_status */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t acal_status                    :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t kcal_status                    :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t roscal_status                  :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t rccal_status                   :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_29                 : 20; /* [29:10],       rsvd,        0x0 */
            uint32_t dl_rfcal_table_status          :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_cal_status;

    /* 0x18 : pu_delay_confg */
    union {
        struct {
            uint32_t ppu_lead                       :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t pud_delay                      :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_11                  :  6; /* [11: 6],       rsvd,        0x0 */
            uint32_t lo_reset_delay                 :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lo_reset_width                 :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t adpll_reset_width              :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pu_delay_confg;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[228];

    /* 0x100 : Register control of power up signals */
    union {
        struct {
            uint32_t lodist_tx_en                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t pu_txbuf                       :  1; /* [    1],        r/w,        0x0 */
            uint32_t pu_rxbuf                       :  1; /* [    2],        r/w,        0x0 */
            uint32_t pu_dtc                         :  1; /* [    3],        r/w,        0x0 */
            uint32_t pu_adpll_sfreg                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t pu_adpll_adc                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t adpll_clk_en                   :  1; /* [    6],        r/w,        0x0 */
            uint32_t lotpm_hfp_bypass               :  1; /* [    7],        r/w,        0x1 */
            uint32_t lotpm_lfp_bypass               :  1; /* [    8],        r/w,        0x1 */
            uint32_t lotpm_hfp_clk_en               :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_fbdv_buf                    :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_fbdv                        :  1; /* [   11],        r/w,        0x0 */
            uint32_t pu_vco                         :  1; /* [   12],        r/w,        0x0 */
            uint32_t pu_vco_ldo                     :  1; /* [   13],        r/w,        0x0 */
            uint32_t pu_lodist_body_bias            :  1; /* [   14],        r/w,        0x0 */
            uint32_t rxadc_clk_en                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_rxadc                       :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_rosdac                      :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_rbb_pkdet                   :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_rbb                         :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_rmx                         :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_lna                         :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pa                          :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_bypass_en                   :  1; /* [   23],        r/w,        0x1 */
            uint32_t pa_seri_cap_en                 :  1; /* [   24],        r/w,        0x1 */
            uint32_t rst_adpll                      :  1; /* [   25],        r/w,        0x0 */
            uint32_t rst_lotpm_hfp                  :  1; /* [   26],        r/w,        0x0 */
            uint32_t rst_fbdv                       :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_reg;

    /* 0x104 : Power up setting in SB state */
    union {
        struct {
            uint32_t lodist_tx_en_sb                :  1; /* [    0],        r/w,        0x0 */
            uint32_t pu_txbuf_sb                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t pu_rxbuf_sb                    :  1; /* [    2],        r/w,        0x0 */
            uint32_t pu_dtc_sb                      :  1; /* [    3],        r/w,        0x0 */
            uint32_t pu_adpll_sfreg_sb              :  1; /* [    4],        r/w,        0x0 */
            uint32_t pu_adpll_adc_sb                :  1; /* [    5],        r/w,        0x0 */
            uint32_t adpll_clk_en_sb                :  1; /* [    6],        r/w,        0x0 */
            uint32_t lotpm_hfp_bypass_sb            :  1; /* [    7],        r/w,        0x1 */
            uint32_t lotpm_lfp_bypass_sb            :  1; /* [    8],        r/w,        0x1 */
            uint32_t lotpm_hfp_clk_en_sb            :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_fbdv_buf_sb                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_fbdv_sb                     :  1; /* [   11],        r/w,        0x0 */
            uint32_t pu_vco_sb                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t pu_vco_ldo_sb                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t pu_lodist_body_bias_sb         :  1; /* [   14],        r/w,        0x0 */
            uint32_t rxadc_clk_en_sb                :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_rxadc_sb                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_rosdac_sb                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_rbb_pkdet_sb                :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_rbb_sb                      :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_rmx_sb                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_lna_sb                      :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pa_sb                       :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_bypass_en_sb                :  1; /* [   23],        r/w,        0x1 */
            uint32_t pa_seri_cap_en_sb              :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_sb;

    /* 0x108 : Power up in LOTX state */
    union {
        struct {
            uint32_t lodist_tx_en_lotx              :  1; /* [    0],        r/w,        0x1 */
            uint32_t pu_txbuf_lotx                  :  1; /* [    1],        r/w,        0x1 */
            uint32_t pu_rxbuf_lotx                  :  1; /* [    2],        r/w,        0x0 */
            uint32_t pu_dtc_lotx                    :  1; /* [    3],        r/w,        0x1 */
            uint32_t pu_adpll_sfreg_lotx            :  1; /* [    4],        r/w,        0x1 */
            uint32_t pu_adpll_adc_lotx              :  1; /* [    5],        r/w,        0x1 */
            uint32_t adpll_clk_en_lotx              :  1; /* [    6],        r/w,        0x1 */
            uint32_t lotpm_hfp_bypass_lotx          :  1; /* [    7],        r/w,        0x1 */
            uint32_t lotpm_lfp_bypass_lotx          :  1; /* [    8],        r/w,        0x1 */
            uint32_t lotpm_hfp_clk_en_lotx          :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_fbdv_buf_lotx               :  1; /* [   10],        r/w,        0x1 */
            uint32_t pu_fbdv_lotx                   :  1; /* [   11],        r/w,        0x1 */
            uint32_t pu_vco_lotx                    :  1; /* [   12],        r/w,        0x1 */
            uint32_t pu_vco_ldo_lotx                :  1; /* [   13],        r/w,        0x1 */
            uint32_t pu_lodist_body_bias_lotx       :  1; /* [   14],        r/w,        0x1 */
            uint32_t rxadc_clk_en_lotx              :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_rxadc_lotx                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_rosdac_lotx                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_rbb_pkdet_lotx              :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_rbb_lotx                    :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_rmx_lotx                    :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_lna_lotx                    :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pa_lotx                     :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_bypass_en_lotx              :  1; /* [   23],        r/w,        0x1 */
            uint32_t pa_seri_cap_en_lotx            :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_lotx;

    /* 0x10C : Power up in LORX state */
    union {
        struct {
            uint32_t lodist_tx_en_lorx              :  1; /* [    0],        r/w,        0x0 */
            uint32_t pu_txbuf_lorx                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t pu_rxbuf_lorx                  :  1; /* [    2],        r/w,        0x1 */
            uint32_t pu_dtc_lorx                    :  1; /* [    3],        r/w,        0x1 */
            uint32_t pu_adpll_sfreg_lorx            :  1; /* [    4],        r/w,        0x1 */
            uint32_t pu_adpll_adc_lorx              :  1; /* [    5],        r/w,        0x1 */
            uint32_t adpll_clk_en_lorx              :  1; /* [    6],        r/w,        0x1 */
            uint32_t lotpm_hfp_bypass_lorx          :  1; /* [    7],        r/w,        0x1 */
            uint32_t lotpm_lfp_bypass_lorx          :  1; /* [    8],        r/w,        0x1 */
            uint32_t lotpm_hfp_clk_en_lorx          :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_fbdv_buf_lorx               :  1; /* [   10],        r/w,        0x1 */
            uint32_t pu_fbdv_lorx                   :  1; /* [   11],        r/w,        0x1 */
            uint32_t pu_vco_lorx                    :  1; /* [   12],        r/w,        0x1 */
            uint32_t pu_vco_ldo_lorx                :  1; /* [   13],        r/w,        0x1 */
            uint32_t pu_lodist_body_bias_lorx       :  1; /* [   14],        r/w,        0x1 */
            uint32_t rxadc_clk_en_lorx              :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_rxadc_lorx                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_rosdac_lorx                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_rbb_pkdet_lorx              :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_rbb_lorx                    :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_rmx_lorx                    :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_lna_lorx                    :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pa_lorx                     :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_bypass_en_lorx              :  1; /* [   23],        r/w,        0x1 */
            uint32_t pa_seri_cap_en_lorx            :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_lorx;

    /* 0x110 : Power up in TX state */
    union {
        struct {
            uint32_t lodist_tx_en_tx                :  1; /* [    0],        r/w,        0x1 */
            uint32_t pu_txbuf_tx                    :  1; /* [    1],        r/w,        0x1 */
            uint32_t pu_rxbuf_tx                    :  1; /* [    2],        r/w,        0x0 */
            uint32_t pu_dtc_tx                      :  1; /* [    3],        r/w,        0x1 */
            uint32_t pu_adpll_sfreg_tx              :  1; /* [    4],        r/w,        0x1 */
            uint32_t pu_adpll_adc_tx                :  1; /* [    5],        r/w,        0x1 */
            uint32_t adpll_clk_en_tx                :  1; /* [    6],        r/w,        0x1 */
            uint32_t lotpm_hfp_bypass_tx            :  1; /* [    7],        r/w,        0x0 */
            uint32_t lotpm_lfp_bypass_tx            :  1; /* [    8],        r/w,        0x0 */
            uint32_t lotpm_hfp_clk_en_tx            :  1; /* [    9],        r/w,        0x1 */
            uint32_t pu_fbdv_buf_tx                 :  1; /* [   10],        r/w,        0x1 */
            uint32_t pu_fbdv_tx                     :  1; /* [   11],        r/w,        0x1 */
            uint32_t pu_vco_tx                      :  1; /* [   12],        r/w,        0x1 */
            uint32_t pu_vco_ldo_tx                  :  1; /* [   13],        r/w,        0x1 */
            uint32_t pu_lodist_body_bias_tx         :  1; /* [   14],        r/w,        0x1 */
            uint32_t rxadc_clk_en_tx                :  1; /* [   15],        r/w,        0x0 */
            uint32_t pu_rxadc_tx                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t pu_rosdac_tx                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t pu_rbb_pkdet_tx                :  1; /* [   18],        r/w,        0x0 */
            uint32_t pu_rbb_tx                      :  1; /* [   19],        r/w,        0x0 */
            uint32_t pu_rmx_tx                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t pu_lna_tx                      :  1; /* [   21],        r/w,        0x0 */
            uint32_t pu_pa_tx                       :  1; /* [   22],        r/w,        0x1 */
            uint32_t rx_bypass_en_tx                :  1; /* [   23],        r/w,        0x1 */
            uint32_t pa_seri_cap_en_tx              :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_tx;

    /* 0x114 : Power up in RX state */
    union {
        struct {
            uint32_t lodist_tx_en_rx                :  1; /* [    0],        r/w,        0x0 */
            uint32_t pu_txbuf_rx                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t pu_rxbuf_rx                    :  1; /* [    2],        r/w,        0x1 */
            uint32_t pu_dtc_rx                      :  1; /* [    3],        r/w,        0x1 */
            uint32_t pu_adpll_sfreg_rx              :  1; /* [    4],        r/w,        0x1 */
            uint32_t pu_adpll_adc_rx                :  1; /* [    5],        r/w,        0x1 */
            uint32_t adpll_clk_en_rx                :  1; /* [    6],        r/w,        0x1 */
            uint32_t lotpm_hfp_bypass_rx            :  1; /* [    7],        r/w,        0x1 */
            uint32_t lotpm_lfp_bypass_rx            :  1; /* [    8],        r/w,        0x1 */
            uint32_t lotpm_hfp_clk_en_rx            :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_fbdv_buf_rx                 :  1; /* [   10],        r/w,        0x1 */
            uint32_t pu_fbdv_rx                     :  1; /* [   11],        r/w,        0x1 */
            uint32_t pu_vco_rx                      :  1; /* [   12],        r/w,        0x1 */
            uint32_t pu_vco_ldo_rx                  :  1; /* [   13],        r/w,        0x1 */
            uint32_t pu_lodist_body_bias_rx         :  1; /* [   14],        r/w,        0x1 */
            uint32_t rxadc_clk_en_rx                :  1; /* [   15],        r/w,        0x1 */
            uint32_t pu_rxadc_rx                    :  1; /* [   16],        r/w,        0x1 */
            uint32_t pu_rosdac_rx                   :  1; /* [   17],        r/w,        0x1 */
            uint32_t pu_rbb_pkdet_rx                :  1; /* [   18],        r/w,        0x1 */
            uint32_t pu_rbb_rx                      :  1; /* [   19],        r/w,        0x1 */
            uint32_t pu_rmx_rx                      :  1; /* [   20],        r/w,        0x1 */
            uint32_t pu_lna_rx                      :  1; /* [   21],        r/w,        0x1 */
            uint32_t pu_pa_rx                       :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_bypass_en_rx                :  1; /* [   23],        r/w,        0x0 */
            uint32_t pa_seri_cap_en_rx              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_rx;

    /* 0x118 : Hardware read back of power up signals */
    union {
        struct {
            uint32_t lodist_tx_en_hw                :  1; /* [    0],          r,        0x0 */
            uint32_t pu_txbuf_hw                    :  1; /* [    1],          r,        0x0 */
            uint32_t pu_rxbuf_hw                    :  1; /* [    2],          r,        0x0 */
            uint32_t pu_dtc_hw                      :  1; /* [    3],          r,        0x0 */
            uint32_t pu_adpll_sfreg_hw              :  1; /* [    4],          r,        0x0 */
            uint32_t pu_adpll_adc_hw                :  1; /* [    5],          r,        0x0 */
            uint32_t adpll_clk_en_hw                :  1; /* [    6],          r,        0x0 */
            uint32_t lotpm_hfp_bypass_hw            :  1; /* [    7],          r,        0x0 */
            uint32_t lotpm_lfp_bypass_hw            :  1; /* [    8],          r,        0x0 */
            uint32_t lotpm_hfp_clk_en_hw            :  1; /* [    9],          r,        0x0 */
            uint32_t pu_fbdv_buf_hw                 :  1; /* [   10],          r,        0x0 */
            uint32_t pu_fbdv_hw                     :  1; /* [   11],          r,        0x0 */
            uint32_t pu_vco_hw                      :  1; /* [   12],          r,        0x0 */
            uint32_t pu_vco_ldo_hw                  :  1; /* [   13],          r,        0x0 */
            uint32_t pu_lodist_body_bias_hw         :  1; /* [   14],          r,        0x0 */
            uint32_t rxadc_clk_en_hw                :  1; /* [   15],          r,        0x0 */
            uint32_t pu_rxadc_hw                    :  1; /* [   16],          r,        0x0 */
            uint32_t pu_rosdac_hw                   :  1; /* [   17],          r,        0x0 */
            uint32_t pu_rbb_pkdet_hw                :  1; /* [   18],          r,        0x0 */
            uint32_t pu_rbb_hw                      :  1; /* [   19],          r,        0x0 */
            uint32_t pu_rmx_hw                      :  1; /* [   20],          r,        0x0 */
            uint32_t pu_lna_hw                      :  1; /* [   21],          r,        0x0 */
            uint32_t pu_pa_hw                       :  1; /* [   22],          r,        0x0 */
            uint32_t rx_bypass_en_hw                :  1; /* [   23],          r,        0x1 */
            uint32_t pa_seri_cap_en_hw              :  1; /* [   24],          r,        0x1 */
            uint32_t rst_adpll_hw                   :  1; /* [   25],          r,        0x0 */
            uint32_t rst_lotpm_hfp_hw               :  1; /* [   26],          r,        0x0 */
            uint32_t rst_fbdv_hw                    :  1; /* [   27],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pucr_hw;

    /* 0x11C : non_reg_readback */
    union {
        struct {
            uint32_t reserved_0_4                   :  5; /* [ 4: 0],       rsvd,        0x0 */
            uint32_t ppu_testbuf_hw                 :  1; /* [    5],          r,        0x0 */
            uint32_t ppu_txbuf_hw                   :  1; /* [    6],          r,        0x0 */
            uint32_t ppu_rxbuf_hw                   :  1; /* [    7],          r,        0x0 */
            uint32_t ppu_adpll_sfreg_hw             :  1; /* [    8],          r,        0x0 */
            uint32_t ppu_fbdv_hw                    :  1; /* [    9],          r,        0x0 */
            uint32_t pud_vco_hw                     :  1; /* [   10],          r,        0x0 */
            uint32_t ppu_vco_hw                     :  1; /* [   11],          r,        0x0 */
            uint32_t ppu_vco_ldo_hw                 :  1; /* [   12],          r,        0x0 */
            uint32_t ppu_lodist_body_bias_hw        :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14                    :  1; /* [   14],       rsvd,        0x0 */
            uint32_t ppu_rbb_hw                     :  1; /* [   15],          r,        0x0 */
            uint32_t ppu_lna_hw                     :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } non_reg_readback;

    /* 0x120 : Register control of TX/RX gain */
    union {
        struct {
            uint32_t gc_lna                         :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t gc_rbb1                        :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t gc_rbb2                        :  3; /* [ 7: 5],        r/w,        0x7 */
            uint32_t rbb_bw                         :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pa_ref_dac                     :  5; /* [16:12],        r/w,       0x1c */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pa_inbuf_unit                  :  3; /* [22:20],        r/w,        0x7 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain_bw;

    /* 0x124 : Hardware read back of TX/RX gain */
    union {
        struct {
            uint32_t gc_lna_hw                      :  3; /* [ 2: 0],          r,        0x0 */
            uint32_t gc_rbb1_hw                     :  2; /* [ 4: 3],          r,        0x0 */
            uint32_t gc_rbb2_hw                     :  3; /* [ 7: 5],          r,        0x0 */
            uint32_t rbb_bw_hw                      :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pa_ref_dac_hw                  :  5; /* [16:12],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t pa_inbuf_unit_hw               :  3; /* [22:20],          r,        0x7 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } trx_gain_bw_hw;

    /* 0x128 : DC Test register 0 */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t dc_tp_out_en                   :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t atest_out_en                   :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t reserved_8_16                  :  9; /* [16: 8],       rsvd,        0x0 */
            uint32_t ten_dtc                        :  1; /* [   17],        r/w,        0x0 */
            uint32_t ten_rbb                        :  1; /* [   18],        r/w,        0x0 */
            uint32_t ten_rbb_actest                 :  1; /* [   19],        r/w,        0x0 */
            uint32_t ten_adpll_adc                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t ten_vco                        :  1; /* [   21],        r/w,        0x0 */
            uint32_t ten_rxadc                      :  1; /* [   22],        r/w,        0x0 */
            uint32_t ten_rrf1                       :  1; /* [   23],        r/w,        0x0 */
            uint32_t ten_rrf0                       :  1; /* [   24],        r/w,        0x0 */
            uint32_t ten_pa_1                       :  1; /* [   25],        r/w,        0x0 */
            uint32_t ten_pa_0                       :  1; /* [   26],        r/w,        0x0 */
            uint32_t ten_lodist                     :  1; /* [   27],        r/w,        0x0 */
            uint32_t ten_dll                        :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t ten_mbg                        :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dctest_actest;

    /* 0x12C : LO Bias Mode registers */
    union {
        struct {
            uint32_t dtest_pulldown                 :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_22                  : 22; /* [22: 1],       rsvd,        0x0 */
            uint32_t dtest_en_rxadc_q               :  1; /* [   23],        r/w,        0x0 */
            uint32_t dtest_en_rxadc_i               :  1; /* [   24],        r/w,        0x0 */
            uint32_t dtest_en_adpll_adc             :  1; /* [   25],        r/w,        0x0 */
            uint32_t dtest_en_mod4                  :  1; /* [   26],        r/w,        0x0 */
            uint32_t dtest_en_fref                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t dtest_en_dtc_out               :  1; /* [   28],        r/w,        0x0 */
            uint32_t dtest_en_dtc_in                :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dtest;

    /* 0x130 : adpll_test */
    union {
        struct {
            uint32_t adpll_test_out                 : 16; /* [15: 0],          r,        0x0 */
            uint32_t adpll_test_data_sel            :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t adpll_test_start_sel           :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_test_en                  :  1; /* [   24],        r/w,        0x0 */
            uint32_t adpll_test_start               :  1; /* [   25],          r,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_test;

    /* 0x134 : rf_ext_pa */
    union {
        struct {
            uint32_t rf_ext_pa_sb                   :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t rf_ext_pa_lotx                 :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t rf_ext_pa_tx                   :  5; /* [14:10],        r/w,        0x1 */
            uint32_t rf_ext_pa_lorx                 :  5; /* [19:15],        r/w,        0x0 */
            uint32_t rf_ext_pa_rx                   :  5; /* [24:20],        r/w,        0x2 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_ext_pa;

    /* 0x138  reserved */
    uint8_t RESERVED0x138[200];

    /* 0x200 : cip_ldo15 */
    union {
        struct {
            uint32_t vg11_sel                       :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cip_ldo15;

    /* 0x204 : PA register */
    union {
        struct {
            uint32_t pa_seri_cs_tx                  :  4; /* [ 3: 0],        r/w,        0x6 */
            uint32_t pa_seri_cs_rx                  :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t pa_seri_cs_hw                  :  4; /* [11: 8],          r,        0x6 */
            uint32_t pa_para_cs                     :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reserved_16_19                 :  4; /* [19:16],       rsvd,        0x0 */
            uint32_t pa_vdd11_sel                   :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t pa_ldo_bm                      :  3; /* [26:24],        r/w,        0x6 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t pa_lp_en                       :  1; /* [   28],        r/w,        0x0 */
            uint32_t pa_hp_en                       :  1; /* [   29],        r/w,        0x0 */
            uint32_t pa_force_short_open            :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pa;

    /* 0x208 : LNA mixer register */
    union {
        struct {
            uint32_t rmx_bm                         :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t lna_rfb_match                  :  3; /* [ 5: 3],        r/w,        0x4 */
            uint32_t lna_vdd13_sel                  :  2; /* [ 7: 6],        r/w,        0x2 */
            uint32_t lna_load_csw                   :  4; /* [11: 8],        r/w,        0x7 */
            uint32_t lna_lg_gsel                    :  3; /* [14:12],        r/w,        0x1 */
            uint32_t lna_cap_match                  :  3; /* [17:15],        r/w,        0x0 */
            uint32_t lna_cap_lg                     :  2; /* [19:18],        r/w,        0x0 */
            uint32_t lna_bm_lg                      :  4; /* [23:20],        r/w,        0x5 */
            uint32_t lna_bm_hg                      :  4; /* [27:24],        r/w,        0x5 */
            uint32_t lna_bm_hw                      :  4; /* [31:28],          r,        0x5 */
        }BF;
        uint32_t WORD;
    } lna_mx;

    /* 0x20C : rbb_rosdac */
    union {
        struct {
            uint32_t rosdac_q_hw                    :  6; /* [ 5: 0],          r,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rosdac_i_hw                    :  6; /* [13: 8],          r,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rosdac_q                       :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rosdac_i                       :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_rosdac;

    /* 0x210 : rbb_cap_1 */
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
    } rbb_cap_1;

    /* 0x214 : rbb_cap_2 */
    union {
        struct {
            uint32_t rbb_cap2_fc_q_hw               :  6; /* [ 5: 0],          r,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rbb_cap2_fc_i_hw               :  6; /* [13: 8],          r,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_q_hw               :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_i_hw               :  6; /* [29:24],          r,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_cap_2;

    /* 0x218 : rbb_cap_3 */
    union {
        struct {
            uint32_t rbb_cap2_fc_q_bw0              :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rbb_cap2_fc_i_bw0              :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_q_bw0              :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_i_bw0              :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_cap_3;

    /* 0x21C : rbb_cap4 */
    union {
        struct {
            uint32_t rbb_cap2_fc_q_bw1              :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rbb_cap2_fc_i_bw1              :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_q_bw1              :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_cap1_fc_i_bw1              :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_cap4;

    /* 0x220 : rbb_rx */
    union {
        struct {
            uint32_t rbb_rx2_bw1                    :  3; /* [ 2: 0],        r/w,        0x5 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rbb_rx1_bw1                    :  3; /* [ 6: 4],        r/w,        0x5 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_rx2_bw0                    :  3; /* [10: 8],        r/w,        0x2 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rbb_rx1_bw0                    :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rbb_rx2_hw                     :  3; /* [18:16],          r,        0x2 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t rbb_rx1_hw                     :  3; /* [22:20],          r,        0x2 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rbb_rx2                        :  3; /* [26:24],        r/w,        0x5 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t rbb_rx1                        :  3; /* [30:28],        r/w,        0x5 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_rx;

    /* 0x224 : rbb */
    union {
        struct {
            uint32_t pkdet_out_latch                :  1; /* [    0],          r,        0x0 */
            uint32_t pkdet_out_raw                  :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t rbb_pkdet_out_rstn_ctrl_hw     :  1; /* [    4],        r/w,        0x1 */
            uint32_t rbb_pkdet_out_rstn_hw          :  1; /* [    5],          r,        0x1 */
            uint32_t rbb_pkdet_out_rstn             :  1; /* [    6],        r/w,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rbb_pkdet_en_ctrl_hw           :  1; /* [    8],        r/w,        0x1 */
            uint32_t rbb_pkdet_en_hw                :  1; /* [    9],          r,        0x0 */
            uint32_t rbb_pkdet_en                   :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t rbb_pkdet_vth                  :  4; /* [15:12],        r/w,        0x5 */
            uint32_t rosdac_range                   :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t rbb_lpf_en                     :  1; /* [   19],        r/w,        0x0 */
            uint32_t rbb_deq                        :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rbb_vcm                        :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t rbb_bm_op                      :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb;

    /* 0x228 : rxadc */
    union {
        struct {
            uint32_t rxadc_oscal_en                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t rxadc_vref_sel                 :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t rxadc_clk_sync_inv             :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t rxadc_clk_inv                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rxadc_clk_div_sel              :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rxadc_glitch_remove            :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rxadc_dly_ctrl                 :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxadc;

    /* 0x22C : rxadc_readback */
    union {
        struct {
            uint32_t rxadc_dout_q                   :  9; /* [ 8: 0],          r,      0x100 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t rxadc_dout_i                   :  9; /* [24:16],          r,      0x100 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rxadc_readback;

    /* 0x230 : rf_adc_osdata */
    union {
        struct {
            uint32_t rxadc_os_q                     :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t rxadc_os_i                     :  9; /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_adc_osdata;

    /* 0x234 : testbuf */
    union {
        struct {
            uint32_t testbuf_rin                    :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t testbuf_rfb                    :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t testbuf_op_cc                  :  4; /* [11: 8],        r/w,        0xa */
            uint32_t testbuf_boost                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t testbuf_bm                     :  3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t testbuf_vcm                    :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t pu_testbuf                     :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } testbuf;

    /* 0x238 : vco */
    union {
        struct {
            uint32_t vco_short_vbias_filter         :  1; /* [    0],        r/w,        0x0 */
            uint32_t vco_short_idac_filter          :  1; /* [    1],        r/w,        0x0 */
            uint32_t vco_modcap_sel                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t vco_acal_vref                  :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t vco_vbias                      :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t vco_idac_boost                 :  1; /* [   12],        r/w,        0x0 */
            uint32_t vco_ldo_vsel                   :  2; /* [14:13],        r/w,        0x1 */
            uint32_t vco_ldo_bypass                 :  1; /* [   15],        r/w,        0x0 */
            uint32_t vco_idac                       :  6; /* [21:16],        r/w,       0x18 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t vco_idac_hw                    :  6; /* [29:24],          r,       0x20 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t vco_acal_ud                    :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } vco;

    /* 0x23C : lodist */
    union {
        struct {
            uint32_t lodist_txbuf_supply_mode       :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t lodist_rxbuf_supply_mode       :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t lodist_rxbuf_supply_boost      :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t lodist_rwell_bias              :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lodist_nwell_bias              :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t lodist_75dc_sel                :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lodist;

    /* 0x240 : fbdv */
    union {
        struct {
            uint32_t fbdv_tpm_clk_sel               :  3; /* [ 2: 0],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t fbdv_adpll_clk_sel             :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t fbdv_dco_dither_clk_sel        :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t fbdv_fb_clk_sel                :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t fbdv_sample_clk_sel            :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t fbdv_stg_sel                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rst_mmdiv                      :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t lotpm_fmash_clk_polarity       :  1; /* [   28],        r/w,        0x0 */
            uint32_t dco_dither_clk_polarity        :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } fbdv;

    /* 0x244 : kcal1 */
    union {
        struct {
            uint32_t kcal_div                       : 16; /* [15: 0],        r/w,     0x2bc0 */
            uint32_t kcal_cnt_start                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t kcal_ratio                     : 10; /* [29:20],        r/w,      0x355 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } kcal1;

    /* 0x248 : kcal2 */
    union {
        struct {
            uint32_t kcal_cnt_op                    : 16; /* [15: 0],          r,        0x0 */
            uint32_t kcal_cnt_rdy                   :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t kcal_ratio_hw                  : 10; /* [29:20],          r,      0x355 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } kcal2;

    /* 0x24C : adpll_slope_gen */
    union {
        struct {
            uint32_t adpll_slope_gen_r_sel          :  3; /* [ 2: 0],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t adpll_slope_gen_dc_corr        :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t adpll_slope_gen_pulse_width_enhance :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_slope_gen;

    /* 0x250 : adpll_adc */
    union {
        struct {
            uint32_t adpll_adc_vth_bias_mode        :  1; /* [    0],        r/w,        0x0 */
            uint32_t adpll_adc_vth_en               :  1; /* [    1],        r/w,        0x0 */
            uint32_t adpll_adc_data_sign_sel        :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t adpll_adc_vref_fine            :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t adpll_adc_vref_coarse          :  2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_adc_oscal_en             :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adpll_adc_clk_sync_inv         :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t adpll_adc_clk_div_sel          :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t adpll_adc_clk_inv              :  1; /* [   28],        r/w,        0x0 */
            uint32_t adpll_adc_clk_en               :  1; /* [   29],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_adc;

    /* 0x254 : adpll_dtc */
    union {
        struct {
            uint32_t adpll_dtc_bypass               :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t adpll_dtc_r_sel                :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t adpll_dtc_inv_vth_sel          :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_dtc;

    /* 0x258 : lo_fc_config1 */
    union {
        struct {
            uint32_t lo_fcw                         : 25; /* [24: 0],        r/w,   0x962000 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_fc_config1;

    /* 0x25C : lo_fcw_config2 */
    union {
        struct {
            uint32_t lo_fcw_hw                      : 25; /* [24: 0],          r,   0x9b0000 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_fcw_config2;

    /* 0x260 : lo_fcw3 */
    union {
        struct {
            uint32_t tx_freq_mod_lp                 : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t tx_freq_mod_hp                 : 10; /* [25:16],          r,      0x200 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_fcw3;

    /* 0x264 : lotpm */
    union {
        struct {
            uint32_t lotpm_lfp_delay_sel            :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t lotpm_hfp_delay_fmash          :  4; /* [ 7: 4],        r/w,        0x2 */
            uint32_t lotpm_hfp_delay_fref           :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t lotpm_hfp_polarity             :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t lotpm_hfp_mash1_sel            :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lotpm;

    /* 0x268 : adpll1 */
    union {
        struct {
            uint32_t adpll_lo_unlock_intrpt_clear   :  1; /* [    0],        r/w,        0x0 */
            uint32_t adpll_lo_lock_sel              :  1; /* [    1],        r/w,        0x0 */
            uint32_t adpll_timeout_cnt1_sel         :  1; /* [    2],        r/w,        0x0 */
            uint32_t adpll_timeout_cnt_sel          :  1; /* [    3],        r/w,        0x0 */
            uint32_t adpll_momhold_lmsenb_ext       :  1; /* [    4],        r/w,        0x0 */
            uint32_t adpll_rst_coarse_det_ext       :  1; /* [    5],        r/w,        0x0 */
            uint32_t adpll_rst_spd_det_ext          :  1; /* [    6],        r/w,        0x0 */
            uint32_t adpll_loop_lock_ext            :  1; /* [    7],        r/w,        0x0 */
            uint32_t adpll_fcal_done_ext            :  1; /* [    8],        r/w,        0x0 */
            uint32_t adpll_fcal_start_ext           :  1; /* [    9],        r/w,        0x0 */
            uint32_t adpll_lo_lock_directly         :  1; /* [   10],        r/w,        0x0 */
            uint32_t adpll_lo_fsm_ext               :  1; /* [   11],        r/w,        0x0 */
            uint32_t adpll_fsm_en                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t adpll_lock_fail_en             :  1; /* [   14],        r/w,        0x0 */
            uint32_t adpll_abnormal_dealed          :  1; /* [   15],        r/w,        0x0 */
            uint32_t reserved_16_17                 :  2; /* [17:16],       rsvd,        0x0 */
            uint32_t adpll_vctrl_det_start_ext      :  1; /* [   18],        r/w,        0x0 */
            uint32_t adpll_vctrl_det_en_ext         :  1; /* [   19],        r/w,        0x0 */
            uint32_t adpll_mom_update_en_ext        :  1; /* [   20],        r/w,        0x0 */
            uint32_t adpll_freqerr_det_start_ext    :  1; /* [   21],        r/w,        0x0 */
            uint32_t adpll_mom_search_en_ext        :  1; /* [   22],        r/w,        0x0 */
            uint32_t adpll_lo_open                  :  1; /* [   23],        r/w,        0x0 */
            uint32_t adpll_sfreg_sel                :  1; /* [   24],        r/w,        0x0 */
            uint32_t adpll_lo_unlock_intrpt_clear_sel :  1; /* [   25],        r/w,        0x1 */
            uint32_t adpll_force_inc_fcal_en        :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll1;

    /* 0x26C : adpll_lf_reg */
    union {
        struct {
            uint32_t adpll_lf_vctrl_range_ext       :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t adpll_lf_lsb_ext               :  7; /* [ 8: 2],        r/w,       0x40 */
            uint32_t adpll_lf_avg_en                :  1; /* [    9],        r/w,        0x1 */
            uint32_t adpll_lf_f_p3                  :  2; /* [11:10],        r/w,        0x1 */
            uint32_t reserved_12                    :  1; /* [   12],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast             :  1; /* [   13],        r/w,        0x1 */
            uint32_t adpll_lf_beta_exp              :  3; /* [16:14],        r/w,        0x3 */
            uint32_t adpll_lf_beta_base             :  2; /* [18:17],        r/w,        0x3 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast            :  2; /* [21:20],        r/w,        0x3 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp             :  3; /* [26:24],        r/w,        0x2 */
            uint32_t adpll_lf_alpha_base            :  1; /* [   27],        r/w,        0x1 */
            uint32_t adpll_lf_ctrl_hw               :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_lf_reg;

    /* 0x270 : adpll_lf_tx */
    union {
        struct {
            uint32_t reserved_0_9                   : 10; /* [ 9: 0],       rsvd,        0x0 */
            uint32_t adpll_lf_f_p3_tx               :  2; /* [11:10],        r/w,        0x1 */
            uint32_t reserved_12                    :  1; /* [   12],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast_tx          :  1; /* [   13],        r/w,        0x1 */
            uint32_t adpll_lf_beta_exp_tx           :  3; /* [16:14],        r/w,        0x3 */
            uint32_t adpll_lf_beta_base_tx          :  2; /* [18:17],        r/w,        0x3 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast_tx         :  2; /* [21:20],        r/w,        0x3 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp_tx          :  3; /* [26:24],        r/w,        0x2 */
            uint32_t adpll_lf_alpha_base_tx         :  1; /* [   27],        r/w,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_lf_tx;

    /* 0x274 : adpll_lf_rx */
    union {
        struct {
            uint32_t reserved_0_9                   : 10; /* [ 9: 0],       rsvd,        0x0 */
            uint32_t adpll_lf_f_p3_rx               :  2; /* [11:10],        r/w,        0x1 */
            uint32_t reserved_12                    :  1; /* [   12],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast_rx          :  1; /* [   13],        r/w,        0x1 */
            uint32_t adpll_lf_beta_exp_rx           :  3; /* [16:14],        r/w,        0x3 */
            uint32_t adpll_lf_beta_base_rx          :  2; /* [18:17],        r/w,        0x3 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast_rx         :  2; /* [21:20],        r/w,        0x3 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp_rx          :  3; /* [26:24],        r/w,        0x2 */
            uint32_t adpll_lf_alpha_base_rx         :  1; /* [   27],        r/w,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_lf_rx;

    /* 0x278 : adpll_lf_hw */
    union {
        struct {
            uint32_t reserved_0_9                   : 10; /* [ 9: 0],       rsvd,        0x0 */
            uint32_t adpll_lf_f_p3_hw               :  2; /* [11:10],          r,        0x1 */
            uint32_t reserved_12                    :  1; /* [   12],       rsvd,        0x0 */
            uint32_t adpll_lf_beta_fast_hw          :  1; /* [   13],          r,        0x1 */
            uint32_t adpll_lf_beta_exp_hw           :  3; /* [16:14],          r,        0x3 */
            uint32_t adpll_lf_beta_base_hw          :  2; /* [18:17],          r,        0x3 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_fast_hw         :  2; /* [21:20],          r,        0x3 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_lf_alpha_exp_hw          :  3; /* [26:24],          r,        0x2 */
            uint32_t adpll_lf_alpha_base_hw         :  1; /* [   27],          r,        0x1 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_lf_hw;

    /* 0x27C : adpll_vctrl */
    union {
        struct {
            uint32_t sdmout_dly_sel                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t sdm_bypass                     :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t sdm_dither                     :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t sdm_order                      :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t adpll_capcode_bypass           :  1; /* [   14],        r/w,        0x0 */
            uint32_t adpll_dco_mash_bypass          :  1; /* [   15],        r/w,        0x0 */
            uint32_t adpll_force_mom_hold           :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adpll_mom_update_period        :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t adpll_vctrl_det_cons_en        :  1; /* [   24],        r/w,        0x0 */
            uint32_t adpll_vctrl_moni_win_sel       :  1; /* [   25],        r/w,        0x0 */
            uint32_t adpll_vctrl_lock_win_sel       :  1; /* [   26],        r/w,        0x0 */
            uint32_t adpll_vctrl_range_sel_ext_en   :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_vctrl;

    /* 0x280 : adpll_lms */
    union {
        struct {
            uint32_t adpll_pha_cancel_delay         :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_pha_cancel_en            :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t adpll_lms_q_delay              :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t adpll_pha_prbs_sel             :  2; /* [11:10],        r/w,        0x1 */
            uint32_t adpll_lms_step_enlarge         :  1; /* [   12],        r/w,        0x0 */
            uint32_t adpll_pha_dither_en            :  1; /* [   13],        r/w,        0x0 */
            uint32_t adpll_pha_dem_en               :  1; /* [   14],        r/w,        0x1 */
            uint32_t adpll_sdm_dither_prbs_en       :  1; /* [   15],        r/w,        0x0 */
            uint32_t adpll_lms_step                 :  2; /* [17:16],        r/w,        0x2 */
            uint32_t adpll_sdm_dither_en_ctrl_hw    :  1; /* [   18],        r/w,        0x1 */
            uint32_t adpll_sdm_dither_en            :  1; /* [   19],        r/w,        0x1 */
            uint32_t adpll_lms_ext_value            :  9; /* [28:20],        r/w,      0x140 */
            uint32_t adpll_lms_ext_value_en         :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t adpll_fref_div2_en             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_lms;

    /* 0x284 : adpll_spd */
    union {
        struct {
            uint32_t adpll_spd_in_range_delay       :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t adpll_spd_out_range_delay      :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t adpll_spd_in_range_cons        :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t adpll_coarse_path_turnoff      :  2; /* [11:10],        r/w,        0x2 */
            uint32_t adpll_spd_threshold            :  2; /* [13:12],        r/w,        0x3 */
            uint32_t adpll_coarse_in_range_cons     :  2; /* [15:14],        r/w,        0x3 */
            uint32_t adpll_spd_gain                 :  2; /* [17:16],        r/w,        0x2 */
            uint32_t adpll_coarse_gain              :  2; /* [19:18],        r/w,        0x2 */
            uint32_t adpll_force_lf_fast_mode_ctrl_hw :  1; /* [   20],        r/w,        0x1 */
            uint32_t adpll_force_lf_fast_mode       :  1; /* [   21],        r/w,        0x0 */
            uint32_t adpll_force_lf_fast_mode_hw    :  1; /* [   22],          r,        0x0 */
            uint32_t adpll_coarse_phaerr_en         :  1; /* [   23],        r/w,        0x1 */
            uint32_t adpll_coarse_path_offtime_sel  :  1; /* [   24],        r/w,        0x0 */
            uint32_t adpll_spd_outrange_dly_sel_ext :  2; /* [26:25],        r/w,        0x1 */
            uint32_t adpll_spd_lms_sstp_win_sel     :  1; /* [   27],        r/w,        0x0 */
            uint32_t adpll_force_coarse_path_on     :  1; /* [   28],        r/w,        0x0 */
            uint32_t adpll_coarsepha_dly_sel        :  1; /* [   29],        r/w,        0x0 */
            uint32_t adpll_spd_in_range_delay_1     :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } adpll_spd;

    /* 0x288 : fcal */
    union {
        struct {
            uint32_t fcal_div_ratio_adj_en          :  1; /* [    0],        r/w,        0x1 */
            uint32_t fcal_coarse_pha_threshold      :  2; /* [ 2: 1],        r/w,        0x1 */
            uint32_t fcal_mom_toggle_cnt            :  1; /* [    3],        r/w,        0x0 */
            uint32_t fcal_clk_period                :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t fcal_mode                      :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t reserved_8_15                  :  8; /* [15: 8],       rsvd,        0x0 */
            uint32_t fcal_mom_ini_ext               :  8; /* [23:16],        r/w,       0x80 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } fcal;

    /* 0x28C : adpll_polarity */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t adpll_lp_mom_polarity          :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t adpll_lms_polarity             :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t adpll_fcal_polarity            :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t adpll_lp_polarity              :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_polarity;

    /* 0x290 : adpll_output */
    union {
        struct {
            uint32_t adpll_unlock_intrpt            :  1; /* [    0],          r,        0x0 */
            uint32_t adpll_lo_lock                  :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t adpll_fsm_state                :  4; /* [ 6: 3],          r,        0x0 */
            uint32_t adpll_spd_unlock_sign          :  1; /* [    7],          r,        0x0 */
            uint32_t adpll_vctrl_out_range_fsm      :  1; /* [    8],          r,        0x0 */
            uint32_t adpll_mom_update_fail_fsm      :  1; /* [    9],          r,        0x0 */
            uint32_t adpll_mom_update_ou_fsm        :  1; /* [   10],          r,        0x0 */
            uint32_t adpll_spd_unlock_fsm           :  1; /* [   11],          r,        0x0 */
            uint32_t adpll_spd_lock_fsm             :  1; /* [   12],          r,        0x0 */
            uint32_t adpll_fcal_done_fsm            :  1; /* [   13],          r,        0x0 */
            uint32_t adpll_capcode_out_range        :  1; /* [   14],          r,        0x0 */
            uint32_t adpll_mom_update_total_ou      :  2; /* [16:15],          r,        0x0 */
            uint32_t adpll_capcode_ud               :  1; /* [   17],          r,        0x0 */
            uint32_t adpll_vctrl_det_done           :  1; /* [   18],          r,        0x0 */
            uint32_t adpll_freqerr_sign             :  1; /* [   19],          r,        0x0 */
            uint32_t adpll_freqerr_ou               :  1; /* [   20],          r,        0x0 */
            uint32_t adpll_freqerr_det_done         :  1; /* [   21],          r,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_output;

    /* 0x294 : adpll_reserved */
    union {
        struct {
            uint32_t adpll_resv1                    : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t adpll_resv0                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } adpll_reserved;

    /* 0x298 : rf_reserved */
    union {
        struct {
            uint32_t rf_resv1                       : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t rf_resv0                       : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_reserved;

    /* 0x29C : rf_reserved_2 */
    union {
        struct {
            uint32_t rf_resv                        : 32; /* [31: 0],        r/w, 0x55555555 */
        }BF;
        uint32_t WORD;
    } rf_reserved_2;

    /* 0x2a0  reserved */
    uint8_t RESERVED0x2a0[96];

    /* 0x300 : rbb_gain_ctrl0 */
    union {
        struct {
            uint32_t gain_ctrl0_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl0_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl0_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl0_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl0_g_rbb2              :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl0_g_rbb1              :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl0;

    /* 0x304 : rbb_gain_ctrl1 */
    union {
        struct {
            uint32_t gain_ctrl1_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl1_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl1_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl1_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl1_g_rbb2              :  3; /* [26:24],        r/w,        0x1 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl1_g_rbb1              :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl1;

    /* 0x308 : rbb_gain_ctrl2 */
    union {
        struct {
            uint32_t gain_ctrl2_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl2_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl2_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl2_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl2_g_rbb2              :  3; /* [26:24],        r/w,        0x2 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl2_g_rbb1              :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl2;

    /* 0x30C : rbb_gain_ctrl3 */
    union {
        struct {
            uint32_t gain_ctrl3_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl3_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl3_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl3_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl3_g_rbb2              :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl3_g_rbb1              :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl3;

    /* 0x310 : rbb_gain_ctrl4 */
    union {
        struct {
            uint32_t gain_ctrl4_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl4_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl4_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl4_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl4_g_rbb2              :  3; /* [26:24],        r/w,        0x1 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl4_g_rbb1              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl4;

    /* 0x314 : rbb_gain_ctrl5 */
    union {
        struct {
            uint32_t gain_ctrl5_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl5_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl5_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl5_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl5_g_rbb2              :  3; /* [26:24],        r/w,        0x2 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl5_g_rbb1              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl5;

    /* 0x318 : rbb_gain_ctrl6 */
    union {
        struct {
            uint32_t gain_ctrl6_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl6_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl6_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl6_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl6_g_rbb2              :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl6_g_rbb1              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl6;

    /* 0x31C : rbb_gain_ctrl7 */
    union {
        struct {
            uint32_t gain_ctrl7_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl7_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl7_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl7_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl7_g_rbb2              :  3; /* [26:24],        r/w,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl7_g_rbb1              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl7;

    /* 0x320 : rbb_gain_ctrl8 */
    union {
        struct {
            uint32_t gain_ctrl8_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl8_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl8_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl8_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl8_g_rbb2              :  3; /* [26:24],        r/w,        0x5 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl8_g_rbb1              :  2; /* [29:28],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl8;

    /* 0x324 : rbb_gain_ctrl9 */
    union {
        struct {
            uint32_t gain_ctrl9_rosdac_q_bw0        :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl9_rosdac_i_bw0        :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl9_rosdac_q_bw1        :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl9_rosdac_i_bw1        :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl9_g_rbb2              :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl9_g_rbb1              :  2; /* [29:28],        r/w,        0x2 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl9;

    /* 0x328 : rbb_gain_ctrl10 */
    union {
        struct {
            uint32_t gain_ctrl10_rosdac_q_bw0       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl10_rosdac_i_bw0       :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl10_rosdac_q_bw1       :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl10_rosdac_i_bw1       :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl10_g_rbb2             :  3; /* [26:24],        r/w,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl10_g_rbb1             :  2; /* [29:28],        r/w,        0x2 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl10;

    /* 0x32C : rbb_gain_ctrl11 */
    union {
        struct {
            uint32_t gain_ctrl11_rosdac_q_bw0       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl11_rosdac_i_bw0       :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl11_rosdac_q_bw1       :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl11_rosdac_i_bw1       :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl11_g_rbb2             :  3; /* [26:24],        r/w,        0x5 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl11_g_rbb1             :  2; /* [29:28],        r/w,        0x2 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl11;

    /* 0x330 : rbb_gain_ctrl12 */
    union {
        struct {
            uint32_t gain_ctrl12_rosdac_q_bw0       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl12_rosdac_i_bw0       :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl12_rosdac_q_bw1       :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl12_rosdac_i_bw1       :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl12_g_rbb2             :  3; /* [26:24],        r/w,        0x6 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl12_g_rbb1             :  2; /* [29:28],        r/w,        0x2 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl12;

    /* 0x334 : rbb_gain_ctrl13 */
    union {
        struct {
            uint32_t gain_ctrl13_rosdac_q_bw0       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl13_rosdac_i_bw0       :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl13_rosdac_q_bw1       :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl13_rosdac_i_bw1       :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl13_g_rbb2             :  3; /* [26:24],        r/w,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl13_g_rbb1             :  2; /* [29:28],        r/w,        0x3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl13;

    /* 0x338 : rbb_gain_ctrl14 */
    union {
        struct {
            uint32_t gain_ctrl14_rosdac_q_bw0       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl14_rosdac_i_bw0       :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl14_rosdac_q_bw1       :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl14_rosdac_i_bw1       :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl14_g_rbb2             :  3; /* [26:24],        r/w,        0x5 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl14_g_rbb1             :  2; /* [29:28],        r/w,        0x3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl14;

    /* 0x33C : rbb_gain_ctrl15 */
    union {
        struct {
            uint32_t gain_ctrl15_rosdac_q_bw0       :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t gain_ctrl15_rosdac_i_bw0       :  6; /* [11: 6],        r/w,       0x20 */
            uint32_t gain_ctrl15_rosdac_q_bw1       :  6; /* [17:12],        r/w,       0x20 */
            uint32_t gain_ctrl15_rosdac_i_bw1       :  6; /* [23:18],        r/w,       0x20 */
            uint32_t gain_ctrl15_g_rbb2             :  3; /* [26:24],        r/w,        0x6 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t gain_ctrl15_g_rbb1             :  2; /* [29:28],        r/w,        0x3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rbb_gain_ctrl15;

    /* 0x340  reserved */
    uint8_t RESERVED0x340[192];

    /* 0x400 : acal_config */
    union {
        struct {
            uint32_t vco_idac_ll                    :  6; /* [ 5: 0],        r/w,       0x20 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t vco_idac_lh                    :  6; /* [13: 8],        r/w,       0x20 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t vco_idac_hl                    :  6; /* [21:16],        r/w,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t vco_idac_hh                    :  6; /* [29:24],        r/w,       0x20 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } acal_config;

    /* 0x404 : lo_config_2402 */
    union {
        struct {
            uint32_t kcal_ratio_2402                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2402       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2402;

    /* 0x408 : lo_config_2404 */
    union {
        struct {
            uint32_t kcal_ratio_2404                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2404       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2404;

    /* 0x40C : lo_config_2406 */
    union {
        struct {
            uint32_t kcal_ratio_2406                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2406       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2406;

    /* 0x410 : lo_config_2408 */
    union {
        struct {
            uint32_t kcal_ratio_2408                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2408       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2408;

    /* 0x414 : lo_config_2410 */
    union {
        struct {
            uint32_t kcal_ratio_2410                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2410       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2410;

    /* 0x418 : lo_config_2412 */
    union {
        struct {
            uint32_t kcal_ratio_2412                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2412       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2412;

    /* 0x41C : lo_config_2414 */
    union {
        struct {
            uint32_t kcal_ratio_2414                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2414       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2414;

    /* 0x420 : lo_config_2416 */
    union {
        struct {
            uint32_t kcal_ratio_2416                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2416       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2416;

    /* 0x424 : lo_config_2418 */
    union {
        struct {
            uint32_t kcal_ratio_2418                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2418       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2418;

    /* 0x428 : lo_config_2420 */
    union {
        struct {
            uint32_t kcal_ratio_2420                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2420       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2420;

    /* 0x42C : lo_config_2422 */
    union {
        struct {
            uint32_t kcal_ratio_2422                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2422       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2422;

    /* 0x430 : lo_config_2424 */
    union {
        struct {
            uint32_t kcal_ratio_2424                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2424       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2424;

    /* 0x434 : lo_config_2426 */
    union {
        struct {
            uint32_t kcal_ratio_2426                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2426       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2426;

    /* 0x438 : lo_config_2428 */
    union {
        struct {
            uint32_t kcal_ratio_2428                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2428       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2428;

    /* 0x43C : lo_config_2430 */
    union {
        struct {
            uint32_t kcal_ratio_2430                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2430       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2430;

    /* 0x440 : lo_config_2432 */
    union {
        struct {
            uint32_t kcal_ratio_2432                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2432       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2432;

    /* 0x444 : lo_config_2434 */
    union {
        struct {
            uint32_t kcal_ratio_2434                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2434       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2434;

    /* 0x448 : lo_config_2436 */
    union {
        struct {
            uint32_t kcal_ratio_2436                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2436       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2436;

    /* 0x44C : lo_config_2438 */
    union {
        struct {
            uint32_t kcal_ratio_2438                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2438       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2438;

    /* 0x450 : lo_config_2440 */
    union {
        struct {
            uint32_t kcal_ratio_2440                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2440       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2440;

    /* 0x454 : lo_config_2442 */
    union {
        struct {
            uint32_t kcal_ratio_2442                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2442       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2442;

    /* 0x458 : lo_config_2444 */
    union {
        struct {
            uint32_t kcal_ratio_2444                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2444       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2444;

    /* 0x45C : lo_config_2446 */
    union {
        struct {
            uint32_t kcal_ratio_2446                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2446       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2446;

    /* 0x460 : lo_config_2448 */
    union {
        struct {
            uint32_t kcal_ratio_2448                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2448       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2448;

    /* 0x464 : lo_config_2450 */
    union {
        struct {
            uint32_t kcal_ratio_2450                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2450       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2450;

    /* 0x468 : lo_config_2452 */
    union {
        struct {
            uint32_t kcal_ratio_2452                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2452       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2452;

    /* 0x46C : lo_config_2454 */
    union {
        struct {
            uint32_t kcal_ratio_2454                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2454       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2454;

    /* 0x470 : lo_config_2456 */
    union {
        struct {
            uint32_t kcal_ratio_2456                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2456       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2456;

    /* 0x474 : lo_config_2458 */
    union {
        struct {
            uint32_t kcal_ratio_2458                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2458       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2458;

    /* 0x478 : lo_config_2460 */
    union {
        struct {
            uint32_t kcal_ratio_2460                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2460       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2460;

    /* 0x47C : lo_config_2462 */
    union {
        struct {
            uint32_t kcal_ratio_2462                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2462       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2462;

    /* 0x480 : lo_config_2464 */
    union {
        struct {
            uint32_t kcal_ratio_2464                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2464       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2464;

    /* 0x484 : lo_config_2466 */
    union {
        struct {
            uint32_t kcal_ratio_2466                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2466       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2466;

    /* 0x488 : lo_config_2468 */
    union {
        struct {
            uint32_t kcal_ratio_2468                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2468       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2468;

    /* 0x48C : lo_config_2470 */
    union {
        struct {
            uint32_t kcal_ratio_2470                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2470       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2470;

    /* 0x490 : lo_config_2472 */
    union {
        struct {
            uint32_t kcal_ratio_2472                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2472       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2472;

    /* 0x494 : lo_config_2474 */
    union {
        struct {
            uint32_t kcal_ratio_2474                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2474       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2474;

    /* 0x498 : lo_config_2476 */
    union {
        struct {
            uint32_t kcal_ratio_2476                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2476       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2476;

    /* 0x49C : lo_config_2478 */
    union {
        struct {
            uint32_t kcal_ratio_2478                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2478       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2478;

    /* 0x4A0 : lo_config_2480 */
    union {
        struct {
            uint32_t kcal_ratio_2480                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2480       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2480;

    /* 0x4A4 : lo_config_2405 */
    union {
        struct {
            uint32_t kcal_ratio_2405                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2405       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2405;

    /* 0x4A8 : lo_config_2415 */
    union {
        struct {
            uint32_t kcal_ratio_2415                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2415       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2415;

    /* 0x4AC : lo_config_2425 */
    union {
        struct {
            uint32_t kcal_ratio_2425                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2425       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2425;

    /* 0x4B0 : lo_config_2435 */
    union {
        struct {
            uint32_t kcal_ratio_2435                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2435       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2435;

    /* 0x4B4 : lo_config_2445 */
    union {
        struct {
            uint32_t kcal_ratio_2445                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2445       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2445;

    /* 0x4B8 : lo_config_2455 */
    union {
        struct {
            uint32_t kcal_ratio_2455                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2455       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2455;

    /* 0x4BC : lo_config_2465 */
    union {
        struct {
            uint32_t kcal_ratio_2465                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2465       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2465;

    /* 0x4C0 : lo_config_2475 */
    union {
        struct {
            uint32_t kcal_ratio_2475                : 10; /* [ 9: 0],        r/w,      0x26c */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t adpll_sdm_dither_en_2475       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } lo_config_2475;

    /* 0x4c4  reserved */
    uint8_t RESERVED0x4c4[60];

    /* 0x500 : dg_testbus_0 */
    union {
        struct {
            uint32_t rf_testbus_read                : 32; /* [31: 0],          r,    0xbf707 */
        }BF;
        uint32_t WORD;
    } dg_testbus_0;

    /* 0x504 : dg_testbus_1 */
    union {
        struct {
            uint32_t rf_testbus_sel                 :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dg_testbus_1;

    /* 0x508 : dg_ppud_0 */
    union {
        struct {
            uint32_t reserved_0_15                  : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t ppud_cnt2                      :  9; /* [24:16],        r/w,       0x9a */
            uint32_t ppud_cnt1                      :  5; /* [29:25],        r/w,        0xf */
            uint32_t ppud_manaual_en                :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dg_ppud_0;

    /* 0x50C : rf_top */
    union {
        struct {
            uint32_t rfckg_afifo_adpll_inv          :  1; /* [    0],        r/w,        0x0 */
            uint32_t rfckg_afifo_rxadc_inv          :  1; /* [    1],        r/w,        0x0 */
            uint32_t rfckg_afifo_tx_inv             :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3_15                  : 13; /* [15: 3],       rsvd,        0x0 */
            uint32_t rf_mac_lo_time_offset          :  6; /* [21:16],        r/w,        0x4 */
            uint32_t rf_rx_mode_hw                  :  2; /* [23:22],          r,        0x0 */
            uint32_t rf_rx_mode_4s                  :  2; /* [25:24],        r/w,        0x0 */
            uint32_t rf_rx_mode_4s_en               :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t rf_rx_en_4s                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t rf_rx_en_src                   :  1; /* [   29],        r/w,        0x0 */
            uint32_t rf_tx_en_4s                    :  1; /* [   30],        r/w,        0x0 */
            uint32_t rf_tx_en_src                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_top;

    /* 0x510 : rf_fsm */
    union {
        struct {
            uint32_t rf_fsm_lo_time                 : 12; /* [11: 0],        r/w,      0x640 */
            uint32_t rf_fsm_state                   :  3; /* [14:12],          r,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rf_fsm_st_4s                   :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t rf_fsm_st_4s_en                :  1; /* [   20],        r/w,        0x0 */
            uint32_t rf_fsm_en                      :  1; /* [   21],        r/w,        0x1 */
            uint32_t rf_fsm_rx_afifo_4s             :  1; /* [   22],        r/w,        0x0 */
            uint32_t rf_fsm_rx_afifo_4s_en          :  1; /* [   23],        r/w,        0x0 */
            uint32_t rf_fsm_tx_afifo_4s             :  1; /* [   24],        r/w,        0x0 */
            uint32_t rf_fsm_tx_afifo_4s_en          :  1; /* [   25],        r/w,        0x0 */
            uint32_t rf_fsm_afifo_dly_time          :  6; /* [31:26],        r/w,       0x17 */
        }BF;
        uint32_t WORD;
    } rf_fsm;

    /* 0x514 : rf_singen_0 */
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
    } rf_singen_0;

    /* 0x518 : rf_singen_1 */
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
    } rf_singen_1;

    /* 0x51C : rf_singen_2 */
    union {
        struct {
            uint32_t singen_gain_i                  : 11; /* [10: 0],        r/w,      0x3ff */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t singen_start_addr1_i           : 10; /* [21:12],        r/w,        0x0 */
            uint32_t singen_start_addr0_i           : 10; /* [31:22],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_singen_2;

    /* 0x520 : rf_singen_3 */
    union {
        struct {
            uint32_t singen_gain_q                  : 11; /* [10: 0],        r/w,      0x3ff */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t singen_start_addr1_q           : 10; /* [21:12],        r/w,        0x0 */
            uint32_t singen_start_addr0_q           : 10; /* [31:22],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_singen_3;

    /* 0x524 : rf_singen_4 */
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
    } rf_singen_4;

    /* 0x528 : rf_sram_ctrl0 */
    union {
        struct {
            uint32_t rf_sram_done                   :  1; /* [    0],          r,        0x0 */
            uint32_t rf_sram_en                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t rf_sram_loop_en                :  1; /* [    2],        r/w,        0x0 */
            uint32_t rf_sram_sts_clr                :  1; /* [    3],        r/w,        0x0 */
            uint32_t rf_sram_mode                   :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t rf_sram_done_cnt               : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl0;

    /* 0x52C : rf_sram_ctrl1 */
    union {
        struct {
            uint32_t rf_sram_addr_end               : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t rf_sram_addr_start             : 16; /* [31:16],        r/w,     0x6000 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl1;

    /* 0x530 : rf_sram_ctrl2 */
    union {
        struct {
            uint32_t rf_sram_sts                    : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_sram_ctrl2;

    /* 0x534 : rf_test_mode */
    union {
        struct {
            uint32_t dacout_hw                      : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t dacout_4s                      : 12; /* [27:16],        r/w,        0x0 */
            uint32_t dacout_4s_en                   :  1; /* [   28],        r/w,        0x0 */
            uint32_t dacout_4s_sram_en              :  1; /* [   29],        r/w,        0x0 */
            uint32_t rf_test_mode_en                :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_test_mode;

    /* 0x538 : rf_rx_pulse_filter */
    union {
        struct {
            uint32_t pf_th2                         :  9; /* [ 8: 0],        r/w,        0xf */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t pf_th1                         :  9; /* [18:10],        r/w,       0x7f */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t pf_en_q                        :  1; /* [   20],        r/w,        0x0 */
            uint32_t pf_en_i                        :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_rx_pulse_filter;

};

typedef volatile struct rf_reg rf_reg_t;


#endif  /* __RF_REG_H__ */
