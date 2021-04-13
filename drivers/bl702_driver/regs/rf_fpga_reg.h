/**
  ******************************************************************************
  * @file    rf_fpga_reg.h
  * @version V1.2
  * @date    2019-09-25
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
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
#ifndef  __RF_FPGA_REG_H__
#define  __RF_FPGA_REG_H__

#include "bl702.h"

/* 0x600 : rf_fpga_ctrl_0 */
#define RF_FPGA_CTRL_0_OFFSET                                   (0x600)
#define RF_FPGA_RF_FSM_ST_CCI_4S                                RF_FPGA_RF_FSM_ST_CCI_4S
#define RF_FPGA_RF_FSM_ST_CCI_4S_POS                            (0U)
#define RF_FPGA_RF_FSM_ST_CCI_4S_LEN                            (5U)
#define RF_FPGA_RF_FSM_ST_CCI_4S_MSK                            (((1U<<RF_FPGA_RF_FSM_ST_CCI_4S_LEN)-1)<<RF_FPGA_RF_FSM_ST_CCI_4S_POS)
#define RF_FPGA_RF_FSM_ST_CCI_4S_UMSK                           (~(((1U<<RF_FPGA_RF_FSM_ST_CCI_4S_LEN)-1)<<RF_FPGA_RF_FSM_ST_CCI_4S_POS))
#define RF_FPGA_RF_FSM_ST_CCI_4S_EN                             RF_FPGA_RF_FSM_ST_CCI_4S_EN
#define RF_FPGA_RF_FSM_ST_CCI_4S_EN_POS                         (5U)
#define RF_FPGA_RF_FSM_ST_CCI_4S_EN_LEN                         (1U)
#define RF_FPGA_RF_FSM_ST_CCI_4S_EN_MSK                         (((1U<<RF_FPGA_RF_FSM_ST_CCI_4S_EN_LEN)-1)<<RF_FPGA_RF_FSM_ST_CCI_4S_EN_POS)
#define RF_FPGA_RF_FSM_ST_CCI_4S_EN_UMSK                        (~(((1U<<RF_FPGA_RF_FSM_ST_CCI_4S_EN_LEN)-1)<<RF_FPGA_RF_FSM_ST_CCI_4S_EN_POS))
#define RF_FPGA_RF_FSM_EN                                       RF_FPGA_RF_FSM_EN
#define RF_FPGA_RF_FSM_EN_POS                                   (16U)
#define RF_FPGA_RF_FSM_EN_LEN                                   (1U)
#define RF_FPGA_RF_FSM_EN_MSK                                   (((1U<<RF_FPGA_RF_FSM_EN_LEN)-1)<<RF_FPGA_RF_FSM_EN_POS)
#define RF_FPGA_RF_FSM_EN_UMSK                                  (~(((1U<<RF_FPGA_RF_FSM_EN_LEN)-1)<<RF_FPGA_RF_FSM_EN_POS))
#define RF_FPGA_RF_FSM_STATE_CCI                                RF_FPGA_RF_FSM_STATE_CCI
#define RF_FPGA_RF_FSM_STATE_CCI_POS                            (23U)
#define RF_FPGA_RF_FSM_STATE_CCI_LEN                            (5U)
#define RF_FPGA_RF_FSM_STATE_CCI_MSK                            (((1U<<RF_FPGA_RF_FSM_STATE_CCI_LEN)-1)<<RF_FPGA_RF_FSM_STATE_CCI_POS)
#define RF_FPGA_RF_FSM_STATE_CCI_UMSK                           (~(((1U<<RF_FPGA_RF_FSM_STATE_CCI_LEN)-1)<<RF_FPGA_RF_FSM_STATE_CCI_POS))
#define RF_FPGA_RF_FSM_PU_RX_EN                                 RF_FPGA_RF_FSM_PU_RX_EN
#define RF_FPGA_RF_FSM_PU_RX_EN_POS                             (28U)
#define RF_FPGA_RF_FSM_PU_RX_EN_LEN                             (1U)
#define RF_FPGA_RF_FSM_PU_RX_EN_MSK                             (((1U<<RF_FPGA_RF_FSM_PU_RX_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_RX_EN_POS)
#define RF_FPGA_RF_FSM_PU_RX_EN_UMSK                            (~(((1U<<RF_FPGA_RF_FSM_PU_RX_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_RX_EN_POS))
#define RF_FPGA_RF_FSM_PU_TX_EN                                 RF_FPGA_RF_FSM_PU_TX_EN
#define RF_FPGA_RF_FSM_PU_TX_EN_POS                             (29U)
#define RF_FPGA_RF_FSM_PU_TX_EN_LEN                             (1U)
#define RF_FPGA_RF_FSM_PU_TX_EN_MSK                             (((1U<<RF_FPGA_RF_FSM_PU_TX_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_TX_EN_POS)
#define RF_FPGA_RF_FSM_PU_TX_EN_UMSK                            (~(((1U<<RF_FPGA_RF_FSM_PU_TX_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_TX_EN_POS))
#define RF_FPGA_RF_FSM_PU_LO_EN                                 RF_FPGA_RF_FSM_PU_LO_EN
#define RF_FPGA_RF_FSM_PU_LO_EN_POS                             (30U)
#define RF_FPGA_RF_FSM_PU_LO_EN_LEN                             (1U)
#define RF_FPGA_RF_FSM_PU_LO_EN_MSK                             (((1U<<RF_FPGA_RF_FSM_PU_LO_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_LO_EN_POS)
#define RF_FPGA_RF_FSM_PU_LO_EN_UMSK                            (~(((1U<<RF_FPGA_RF_FSM_PU_LO_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_LO_EN_POS))
#define RF_FPGA_RF_FSM_PU_SB_EN                                 RF_FPGA_RF_FSM_PU_SB_EN
#define RF_FPGA_RF_FSM_PU_SB_EN_POS                             (31U)
#define RF_FPGA_RF_FSM_PU_SB_EN_LEN                             (1U)
#define RF_FPGA_RF_FSM_PU_SB_EN_MSK                             (((1U<<RF_FPGA_RF_FSM_PU_SB_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_SB_EN_POS)
#define RF_FPGA_RF_FSM_PU_SB_EN_UMSK                            (~(((1U<<RF_FPGA_RF_FSM_PU_SB_EN_LEN)-1)<<RF_FPGA_RF_FSM_PU_SB_EN_POS))

/* 0x604 : rf_fpga_ctrl_1 */
#define RF_FPGA_CTRL_1_OFFSET                                   (0x604)
#define RF_FPGA_RF_FSM_CCI_WR_TIME                              RF_FPGA_RF_FSM_CCI_WR_TIME
#define RF_FPGA_RF_FSM_CCI_WR_TIME_POS                          (0U)
#define RF_FPGA_RF_FSM_CCI_WR_TIME_LEN                          (12U)
#define RF_FPGA_RF_FSM_CCI_WR_TIME_MSK                          (((1U<<RF_FPGA_RF_FSM_CCI_WR_TIME_LEN)-1)<<RF_FPGA_RF_FSM_CCI_WR_TIME_POS)
#define RF_FPGA_RF_FSM_CCI_WR_TIME_UMSK                         (~(((1U<<RF_FPGA_RF_FSM_CCI_WR_TIME_LEN)-1)<<RF_FPGA_RF_FSM_CCI_WR_TIME_POS))
#define RF_FPGA_RF_FSM_LO_TIME                                  RF_FPGA_RF_FSM_LO_TIME
#define RF_FPGA_RF_FSM_LO_TIME_POS                              (16U)
#define RF_FPGA_RF_FSM_LO_TIME_LEN                              (12U)
#define RF_FPGA_RF_FSM_LO_TIME_MSK                              (((1U<<RF_FPGA_RF_FSM_LO_TIME_LEN)-1)<<RF_FPGA_RF_FSM_LO_TIME_POS)
#define RF_FPGA_RF_FSM_LO_TIME_UMSK                             (~(((1U<<RF_FPGA_RF_FSM_LO_TIME_LEN)-1)<<RF_FPGA_RF_FSM_LO_TIME_POS))

/* 0x608 : rf_fpga_ctrl_2 */
#define RF_FPGA_CTRL_2_OFFSET                                   (0x608)
#define RF_FPGA_RF_FSM_DLY_30_TIME                              RF_FPGA_RF_FSM_DLY_30_TIME
#define RF_FPGA_RF_FSM_DLY_30_TIME_POS                          (0U)
#define RF_FPGA_RF_FSM_DLY_30_TIME_LEN                          (12U)
#define RF_FPGA_RF_FSM_DLY_30_TIME_MSK                          (((1U<<RF_FPGA_RF_FSM_DLY_30_TIME_LEN)-1)<<RF_FPGA_RF_FSM_DLY_30_TIME_POS)
#define RF_FPGA_RF_FSM_DLY_30_TIME_UMSK                         (~(((1U<<RF_FPGA_RF_FSM_DLY_30_TIME_LEN)-1)<<RF_FPGA_RF_FSM_DLY_30_TIME_POS))
#define RF_FPGA_RF_FSM_DLY_01_TIME                              RF_FPGA_RF_FSM_DLY_01_TIME
#define RF_FPGA_RF_FSM_DLY_01_TIME_POS                          (16U)
#define RF_FPGA_RF_FSM_DLY_01_TIME_LEN                          (12U)
#define RF_FPGA_RF_FSM_DLY_01_TIME_MSK                          (((1U<<RF_FPGA_RF_FSM_DLY_01_TIME_LEN)-1)<<RF_FPGA_RF_FSM_DLY_01_TIME_POS)
#define RF_FPGA_RF_FSM_DLY_01_TIME_UMSK                         (~(((1U<<RF_FPGA_RF_FSM_DLY_01_TIME_LEN)-1)<<RF_FPGA_RF_FSM_DLY_01_TIME_POS))

/* 0x60C : rf_fpga_ctrl_3 */
#define RF_FPGA_CTRL_3_OFFSET                                   (0x60C)
#define RF_FPGA_RF_FSM_PU_SB                                    RF_FPGA_RF_FSM_PU_SB
#define RF_FPGA_RF_FSM_PU_SB_POS                                (0U)
#define RF_FPGA_RF_FSM_PU_SB_LEN                                (32U)
#define RF_FPGA_RF_FSM_PU_SB_MSK                                (((1U<<RF_FPGA_RF_FSM_PU_SB_LEN)-1)<<RF_FPGA_RF_FSM_PU_SB_POS)
#define RF_FPGA_RF_FSM_PU_SB_UMSK                               (~(((1U<<RF_FPGA_RF_FSM_PU_SB_LEN)-1)<<RF_FPGA_RF_FSM_PU_SB_POS))

/* 0x610 : rf_fpga_ctrl_4 */
#define RF_FPGA_CTRL_4_OFFSET                                   (0x610)
#define RF_FPGA_RF_FSM_PU_LO                                    RF_FPGA_RF_FSM_PU_LO
#define RF_FPGA_RF_FSM_PU_LO_POS                                (0U)
#define RF_FPGA_RF_FSM_PU_LO_LEN                                (32U)
#define RF_FPGA_RF_FSM_PU_LO_MSK                                (((1U<<RF_FPGA_RF_FSM_PU_LO_LEN)-1)<<RF_FPGA_RF_FSM_PU_LO_POS)
#define RF_FPGA_RF_FSM_PU_LO_UMSK                               (~(((1U<<RF_FPGA_RF_FSM_PU_LO_LEN)-1)<<RF_FPGA_RF_FSM_PU_LO_POS))

/* 0x614 : rf_fpga_ctrl_5 */
#define RF_FPGA_CTRL_5_OFFSET                                   (0x614)
#define RF_FPGA_RF_FSM_PU_TX                                    RF_FPGA_RF_FSM_PU_TX
#define RF_FPGA_RF_FSM_PU_TX_POS                                (0U)
#define RF_FPGA_RF_FSM_PU_TX_LEN                                (32U)
#define RF_FPGA_RF_FSM_PU_TX_MSK                                (((1U<<RF_FPGA_RF_FSM_PU_TX_LEN)-1)<<RF_FPGA_RF_FSM_PU_TX_POS)
#define RF_FPGA_RF_FSM_PU_TX_UMSK                               (~(((1U<<RF_FPGA_RF_FSM_PU_TX_LEN)-1)<<RF_FPGA_RF_FSM_PU_TX_POS))

/* 0x618 : rf_fpga_ctrl_6 */
#define RF_FPGA_CTRL_6_OFFSET                                   (0x618)
#define RF_FPGA_RF_FSM_PU_RX                                    RF_FPGA_RF_FSM_PU_RX
#define RF_FPGA_RF_FSM_PU_RX_POS                                (0U)
#define RF_FPGA_RF_FSM_PU_RX_LEN                                (32U)
#define RF_FPGA_RF_FSM_PU_RX_MSK                                (((1U<<RF_FPGA_RF_FSM_PU_RX_LEN)-1)<<RF_FPGA_RF_FSM_PU_RX_POS)
#define RF_FPGA_RF_FSM_PU_RX_UMSK                               (~(((1U<<RF_FPGA_RF_FSM_PU_RX_LEN)-1)<<RF_FPGA_RF_FSM_PU_RX_POS))

/* 0x61C : rf_fpga_ctrl_7 */
#define RF_FPGA_CTRL_7_OFFSET                                   (0x61C)
#define RF_FPGA_IDAC_CW_0                                       RF_FPGA_IDAC_CW_0
#define RF_FPGA_IDAC_CW_0_POS                                   (21U)
#define RF_FPGA_IDAC_CW_0_LEN                                   (5U)
#define RF_FPGA_IDAC_CW_0_MSK                                   (((1U<<RF_FPGA_IDAC_CW_0_LEN)-1)<<RF_FPGA_IDAC_CW_0_POS)
#define RF_FPGA_IDAC_CW_0_UMSK                                  (~(((1U<<RF_FPGA_IDAC_CW_0_LEN)-1)<<RF_FPGA_IDAC_CW_0_POS))

/* 0x620 : rf_fpga_table_0 */
#define RF_FPGA_TABLE_0_OFFSET                                  (0x620)
#define RF_FPGA_SDMIN_0                                         RF_FPGA_SDMIN_0
#define RF_FPGA_SDMIN_0_POS                                     (0U)
#define RF_FPGA_SDMIN_0_LEN                                     (30U)
#define RF_FPGA_SDMIN_0_MSK                                     (((1U<<RF_FPGA_SDMIN_0_LEN)-1)<<RF_FPGA_SDMIN_0_POS)
#define RF_FPGA_SDMIN_0_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_0_LEN)-1)<<RF_FPGA_SDMIN_0_POS))

/* 0x624 : rf_fpga_table_1 */
#define RF_FPGA_TABLE_1_OFFSET                                  (0x624)
#define RF_FPGA_SDMIN_1                                         RF_FPGA_SDMIN_1
#define RF_FPGA_SDMIN_1_POS                                     (0U)
#define RF_FPGA_SDMIN_1_LEN                                     (30U)
#define RF_FPGA_SDMIN_1_MSK                                     (((1U<<RF_FPGA_SDMIN_1_LEN)-1)<<RF_FPGA_SDMIN_1_POS)
#define RF_FPGA_SDMIN_1_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_1_LEN)-1)<<RF_FPGA_SDMIN_1_POS))

/* 0x628 : rf_fpga_table_2 */
#define RF_FPGA_TABLE_2_OFFSET                                  (0x628)
#define RF_FPGA_SDMIN_2                                         RF_FPGA_SDMIN_2
#define RF_FPGA_SDMIN_2_POS                                     (0U)
#define RF_FPGA_SDMIN_2_LEN                                     (30U)
#define RF_FPGA_SDMIN_2_MSK                                     (((1U<<RF_FPGA_SDMIN_2_LEN)-1)<<RF_FPGA_SDMIN_2_POS)
#define RF_FPGA_SDMIN_2_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_2_LEN)-1)<<RF_FPGA_SDMIN_2_POS))

/* 0x62C : rf_fpga_table_3 */
#define RF_FPGA_TABLE_3_OFFSET                                  (0x62C)
#define RF_FPGA_SDMIN_3                                         RF_FPGA_SDMIN_3
#define RF_FPGA_SDMIN_3_POS                                     (0U)
#define RF_FPGA_SDMIN_3_LEN                                     (30U)
#define RF_FPGA_SDMIN_3_MSK                                     (((1U<<RF_FPGA_SDMIN_3_LEN)-1)<<RF_FPGA_SDMIN_3_POS)
#define RF_FPGA_SDMIN_3_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_3_LEN)-1)<<RF_FPGA_SDMIN_3_POS))

/* 0x630 : rf_fpga_table_4 */
#define RF_FPGA_TABLE_4_OFFSET                                  (0x630)
#define RF_FPGA_SDMIN_4                                         RF_FPGA_SDMIN_4
#define RF_FPGA_SDMIN_4_POS                                     (0U)
#define RF_FPGA_SDMIN_4_LEN                                     (30U)
#define RF_FPGA_SDMIN_4_MSK                                     (((1U<<RF_FPGA_SDMIN_4_LEN)-1)<<RF_FPGA_SDMIN_4_POS)
#define RF_FPGA_SDMIN_4_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_4_LEN)-1)<<RF_FPGA_SDMIN_4_POS))

/* 0x634 : rf_fpga_table_5 */
#define RF_FPGA_TABLE_5_OFFSET                                  (0x634)
#define RF_FPGA_SDMIN_5                                         RF_FPGA_SDMIN_5
#define RF_FPGA_SDMIN_5_POS                                     (0U)
#define RF_FPGA_SDMIN_5_LEN                                     (30U)
#define RF_FPGA_SDMIN_5_MSK                                     (((1U<<RF_FPGA_SDMIN_5_LEN)-1)<<RF_FPGA_SDMIN_5_POS)
#define RF_FPGA_SDMIN_5_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_5_LEN)-1)<<RF_FPGA_SDMIN_5_POS))

/* 0x638 : rf_fpga_table_6 */
#define RF_FPGA_TABLE_6_OFFSET                                  (0x638)
#define RF_FPGA_SDMIN_6                                         RF_FPGA_SDMIN_6
#define RF_FPGA_SDMIN_6_POS                                     (0U)
#define RF_FPGA_SDMIN_6_LEN                                     (30U)
#define RF_FPGA_SDMIN_6_MSK                                     (((1U<<RF_FPGA_SDMIN_6_LEN)-1)<<RF_FPGA_SDMIN_6_POS)
#define RF_FPGA_SDMIN_6_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_6_LEN)-1)<<RF_FPGA_SDMIN_6_POS))

/* 0x63C : rf_fpga_table_7 */
#define RF_FPGA_TABLE_7_OFFSET                                  (0x63C)
#define RF_FPGA_SDMIN_7                                         RF_FPGA_SDMIN_7
#define RF_FPGA_SDMIN_7_POS                                     (0U)
#define RF_FPGA_SDMIN_7_LEN                                     (30U)
#define RF_FPGA_SDMIN_7_MSK                                     (((1U<<RF_FPGA_SDMIN_7_LEN)-1)<<RF_FPGA_SDMIN_7_POS)
#define RF_FPGA_SDMIN_7_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_7_LEN)-1)<<RF_FPGA_SDMIN_7_POS))

/* 0x640 : rf_fpga_table_8 */
#define RF_FPGA_TABLE_8_OFFSET                                  (0x640)
#define RF_FPGA_SDMIN_8                                         RF_FPGA_SDMIN_8
#define RF_FPGA_SDMIN_8_POS                                     (0U)
#define RF_FPGA_SDMIN_8_LEN                                     (30U)
#define RF_FPGA_SDMIN_8_MSK                                     (((1U<<RF_FPGA_SDMIN_8_LEN)-1)<<RF_FPGA_SDMIN_8_POS)
#define RF_FPGA_SDMIN_8_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_8_LEN)-1)<<RF_FPGA_SDMIN_8_POS))

/* 0x644 : rf_fpga_table_9 */
#define RF_FPGA_TABLE_9_OFFSET                                  (0x644)
#define RF_FPGA_SDMIN_9                                         RF_FPGA_SDMIN_9
#define RF_FPGA_SDMIN_9_POS                                     (0U)
#define RF_FPGA_SDMIN_9_LEN                                     (30U)
#define RF_FPGA_SDMIN_9_MSK                                     (((1U<<RF_FPGA_SDMIN_9_LEN)-1)<<RF_FPGA_SDMIN_9_POS)
#define RF_FPGA_SDMIN_9_UMSK                                    (~(((1U<<RF_FPGA_SDMIN_9_LEN)-1)<<RF_FPGA_SDMIN_9_POS))

/* 0x648 : rf_fpga_table_10 */
#define RF_FPGA_TABLE_10_OFFSET                                 (0x648)
#define RF_FPGA_SDMIN_10                                        RF_FPGA_SDMIN_10
#define RF_FPGA_SDMIN_10_POS                                    (0U)
#define RF_FPGA_SDMIN_10_LEN                                    (30U)
#define RF_FPGA_SDMIN_10_MSK                                    (((1U<<RF_FPGA_SDMIN_10_LEN)-1)<<RF_FPGA_SDMIN_10_POS)
#define RF_FPGA_SDMIN_10_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_10_LEN)-1)<<RF_FPGA_SDMIN_10_POS))

/* 0x64C : rf_fpga_table_11 */
#define RF_FPGA_TABLE_11_OFFSET                                 (0x64C)
#define RF_FPGA_SDMIN_11                                        RF_FPGA_SDMIN_11
#define RF_FPGA_SDMIN_11_POS                                    (0U)
#define RF_FPGA_SDMIN_11_LEN                                    (30U)
#define RF_FPGA_SDMIN_11_MSK                                    (((1U<<RF_FPGA_SDMIN_11_LEN)-1)<<RF_FPGA_SDMIN_11_POS)
#define RF_FPGA_SDMIN_11_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_11_LEN)-1)<<RF_FPGA_SDMIN_11_POS))

/* 0x650 : rf_fpga_table_12 */
#define RF_FPGA_TABLE_12_OFFSET                                 (0x650)
#define RF_FPGA_SDMIN_12                                        RF_FPGA_SDMIN_12
#define RF_FPGA_SDMIN_12_POS                                    (0U)
#define RF_FPGA_SDMIN_12_LEN                                    (30U)
#define RF_FPGA_SDMIN_12_MSK                                    (((1U<<RF_FPGA_SDMIN_12_LEN)-1)<<RF_FPGA_SDMIN_12_POS)
#define RF_FPGA_SDMIN_12_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_12_LEN)-1)<<RF_FPGA_SDMIN_12_POS))

/* 0x654 : rf_fpga_table_13 */
#define RF_FPGA_TABLE_13_OFFSET                                 (0x654)
#define RF_FPGA_SDMIN_13                                        RF_FPGA_SDMIN_13
#define RF_FPGA_SDMIN_13_POS                                    (0U)
#define RF_FPGA_SDMIN_13_LEN                                    (30U)
#define RF_FPGA_SDMIN_13_MSK                                    (((1U<<RF_FPGA_SDMIN_13_LEN)-1)<<RF_FPGA_SDMIN_13_POS)
#define RF_FPGA_SDMIN_13_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_13_LEN)-1)<<RF_FPGA_SDMIN_13_POS))

/* 0x658 : rf_fpga_table_14 */
#define RF_FPGA_TABLE_14_OFFSET                                 (0x658)
#define RF_FPGA_SDMIN_14                                        RF_FPGA_SDMIN_14
#define RF_FPGA_SDMIN_14_POS                                    (0U)
#define RF_FPGA_SDMIN_14_LEN                                    (30U)
#define RF_FPGA_SDMIN_14_MSK                                    (((1U<<RF_FPGA_SDMIN_14_LEN)-1)<<RF_FPGA_SDMIN_14_POS)
#define RF_FPGA_SDMIN_14_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_14_LEN)-1)<<RF_FPGA_SDMIN_14_POS))

/* 0x65C : rf_fpga_table_15 */
#define RF_FPGA_TABLE_15_OFFSET                                 (0x65C)
#define RF_FPGA_SDMIN_15                                        RF_FPGA_SDMIN_15
#define RF_FPGA_SDMIN_15_POS                                    (0U)
#define RF_FPGA_SDMIN_15_LEN                                    (30U)
#define RF_FPGA_SDMIN_15_MSK                                    (((1U<<RF_FPGA_SDMIN_15_LEN)-1)<<RF_FPGA_SDMIN_15_POS)
#define RF_FPGA_SDMIN_15_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_15_LEN)-1)<<RF_FPGA_SDMIN_15_POS))

/* 0x660 : rf_fpga_table_16 */
#define RF_FPGA_TABLE_16_OFFSET                                 (0x660)
#define RF_FPGA_SDMIN_16                                        RF_FPGA_SDMIN_16
#define RF_FPGA_SDMIN_16_POS                                    (0U)
#define RF_FPGA_SDMIN_16_LEN                                    (30U)
#define RF_FPGA_SDMIN_16_MSK                                    (((1U<<RF_FPGA_SDMIN_16_LEN)-1)<<RF_FPGA_SDMIN_16_POS)
#define RF_FPGA_SDMIN_16_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_16_LEN)-1)<<RF_FPGA_SDMIN_16_POS))

/* 0x664 : rf_fpga_table_17 */
#define RF_FPGA_TABLE_17_OFFSET                                 (0x664)
#define RF_FPGA_SDMIN_17                                        RF_FPGA_SDMIN_17
#define RF_FPGA_SDMIN_17_POS                                    (0U)
#define RF_FPGA_SDMIN_17_LEN                                    (30U)
#define RF_FPGA_SDMIN_17_MSK                                    (((1U<<RF_FPGA_SDMIN_17_LEN)-1)<<RF_FPGA_SDMIN_17_POS)
#define RF_FPGA_SDMIN_17_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_17_LEN)-1)<<RF_FPGA_SDMIN_17_POS))

/* 0x668 : rf_fpga_table_18 */
#define RF_FPGA_TABLE_18_OFFSET                                 (0x668)
#define RF_FPGA_SDMIN_18                                        RF_FPGA_SDMIN_18
#define RF_FPGA_SDMIN_18_POS                                    (0U)
#define RF_FPGA_SDMIN_18_LEN                                    (30U)
#define RF_FPGA_SDMIN_18_MSK                                    (((1U<<RF_FPGA_SDMIN_18_LEN)-1)<<RF_FPGA_SDMIN_18_POS)
#define RF_FPGA_SDMIN_18_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_18_LEN)-1)<<RF_FPGA_SDMIN_18_POS))

/* 0x66C : rf_fpga_table_19 */
#define RF_FPGA_TABLE_19_OFFSET                                 (0x66C)
#define RF_FPGA_SDMIN_19                                        RF_FPGA_SDMIN_19
#define RF_FPGA_SDMIN_19_POS                                    (0U)
#define RF_FPGA_SDMIN_19_LEN                                    (30U)
#define RF_FPGA_SDMIN_19_MSK                                    (((1U<<RF_FPGA_SDMIN_19_LEN)-1)<<RF_FPGA_SDMIN_19_POS)
#define RF_FPGA_SDMIN_19_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_19_LEN)-1)<<RF_FPGA_SDMIN_19_POS))

/* 0x670 : rf_fpga_table_20 */
#define RF_FPGA_TABLE_20_OFFSET                                 (0x670)
#define RF_FPGA_SDMIN_20                                        RF_FPGA_SDMIN_20
#define RF_FPGA_SDMIN_20_POS                                    (0U)
#define RF_FPGA_SDMIN_20_LEN                                    (30U)
#define RF_FPGA_SDMIN_20_MSK                                    (((1U<<RF_FPGA_SDMIN_20_LEN)-1)<<RF_FPGA_SDMIN_20_POS)
#define RF_FPGA_SDMIN_20_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_20_LEN)-1)<<RF_FPGA_SDMIN_20_POS))

/* 0x674 : rf_fpga_table_21 */
#define RF_FPGA_TABLE_21_OFFSET                                 (0x674)
#define RF_FPGA_SDMIN_21                                        RF_FPGA_SDMIN_21
#define RF_FPGA_SDMIN_21_POS                                    (0U)
#define RF_FPGA_SDMIN_21_LEN                                    (30U)
#define RF_FPGA_SDMIN_21_MSK                                    (((1U<<RF_FPGA_SDMIN_21_LEN)-1)<<RF_FPGA_SDMIN_21_POS)
#define RF_FPGA_SDMIN_21_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_21_LEN)-1)<<RF_FPGA_SDMIN_21_POS))

/* 0x678 : rf_fpga_table_22 */
#define RF_FPGA_TABLE_22_OFFSET                                 (0x678)
#define RF_FPGA_SDMIN_22                                        RF_FPGA_SDMIN_22
#define RF_FPGA_SDMIN_22_POS                                    (0U)
#define RF_FPGA_SDMIN_22_LEN                                    (30U)
#define RF_FPGA_SDMIN_22_MSK                                    (((1U<<RF_FPGA_SDMIN_22_LEN)-1)<<RF_FPGA_SDMIN_22_POS)
#define RF_FPGA_SDMIN_22_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_22_LEN)-1)<<RF_FPGA_SDMIN_22_POS))

/* 0x67C : rf_fpga_table_23 */
#define RF_FPGA_TABLE_23_OFFSET                                 (0x67C)
#define RF_FPGA_SDMIN_23                                        RF_FPGA_SDMIN_23
#define RF_FPGA_SDMIN_23_POS                                    (0U)
#define RF_FPGA_SDMIN_23_LEN                                    (30U)
#define RF_FPGA_SDMIN_23_MSK                                    (((1U<<RF_FPGA_SDMIN_23_LEN)-1)<<RF_FPGA_SDMIN_23_POS)
#define RF_FPGA_SDMIN_23_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_23_LEN)-1)<<RF_FPGA_SDMIN_23_POS))

/* 0x680 : rf_fpga_table_24 */
#define RF_FPGA_TABLE_24_OFFSET                                 (0x680)
#define RF_FPGA_SDMIN_24                                        RF_FPGA_SDMIN_24
#define RF_FPGA_SDMIN_24_POS                                    (0U)
#define RF_FPGA_SDMIN_24_LEN                                    (30U)
#define RF_FPGA_SDMIN_24_MSK                                    (((1U<<RF_FPGA_SDMIN_24_LEN)-1)<<RF_FPGA_SDMIN_24_POS)
#define RF_FPGA_SDMIN_24_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_24_LEN)-1)<<RF_FPGA_SDMIN_24_POS))

/* 0x684 : rf_fpga_table_25 */
#define RF_FPGA_TABLE_25_OFFSET                                 (0x684)
#define RF_FPGA_SDMIN_25                                        RF_FPGA_SDMIN_25
#define RF_FPGA_SDMIN_25_POS                                    (0U)
#define RF_FPGA_SDMIN_25_LEN                                    (30U)
#define RF_FPGA_SDMIN_25_MSK                                    (((1U<<RF_FPGA_SDMIN_25_LEN)-1)<<RF_FPGA_SDMIN_25_POS)
#define RF_FPGA_SDMIN_25_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_25_LEN)-1)<<RF_FPGA_SDMIN_25_POS))

/* 0x688 : rf_fpga_table_26 */
#define RF_FPGA_TABLE_26_OFFSET                                 (0x688)
#define RF_FPGA_SDMIN_26                                        RF_FPGA_SDMIN_26
#define RF_FPGA_SDMIN_26_POS                                    (0U)
#define RF_FPGA_SDMIN_26_LEN                                    (30U)
#define RF_FPGA_SDMIN_26_MSK                                    (((1U<<RF_FPGA_SDMIN_26_LEN)-1)<<RF_FPGA_SDMIN_26_POS)
#define RF_FPGA_SDMIN_26_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_26_LEN)-1)<<RF_FPGA_SDMIN_26_POS))

/* 0x68C : rf_fpga_table_27 */
#define RF_FPGA_TABLE_27_OFFSET                                 (0x68C)
#define RF_FPGA_SDMIN_27                                        RF_FPGA_SDMIN_27
#define RF_FPGA_SDMIN_27_POS                                    (0U)
#define RF_FPGA_SDMIN_27_LEN                                    (30U)
#define RF_FPGA_SDMIN_27_MSK                                    (((1U<<RF_FPGA_SDMIN_27_LEN)-1)<<RF_FPGA_SDMIN_27_POS)
#define RF_FPGA_SDMIN_27_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_27_LEN)-1)<<RF_FPGA_SDMIN_27_POS))

/* 0x690 : rf_fpga_table_28 */
#define RF_FPGA_TABLE_28_OFFSET                                 (0x690)
#define RF_FPGA_SDMIN_28                                        RF_FPGA_SDMIN_28
#define RF_FPGA_SDMIN_28_POS                                    (0U)
#define RF_FPGA_SDMIN_28_LEN                                    (30U)
#define RF_FPGA_SDMIN_28_MSK                                    (((1U<<RF_FPGA_SDMIN_28_LEN)-1)<<RF_FPGA_SDMIN_28_POS)
#define RF_FPGA_SDMIN_28_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_28_LEN)-1)<<RF_FPGA_SDMIN_28_POS))

/* 0x694 : rf_fpga_table_29 */
#define RF_FPGA_TABLE_29_OFFSET                                 (0x694)
#define RF_FPGA_SDMIN_29                                        RF_FPGA_SDMIN_29
#define RF_FPGA_SDMIN_29_POS                                    (0U)
#define RF_FPGA_SDMIN_29_LEN                                    (30U)
#define RF_FPGA_SDMIN_29_MSK                                    (((1U<<RF_FPGA_SDMIN_29_LEN)-1)<<RF_FPGA_SDMIN_29_POS)
#define RF_FPGA_SDMIN_29_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_29_LEN)-1)<<RF_FPGA_SDMIN_29_POS))

/* 0x698 : rf_fpga_table_30 */
#define RF_FPGA_TABLE_30_OFFSET                                 (0x698)
#define RF_FPGA_SDMIN_30                                        RF_FPGA_SDMIN_30
#define RF_FPGA_SDMIN_30_POS                                    (0U)
#define RF_FPGA_SDMIN_30_LEN                                    (30U)
#define RF_FPGA_SDMIN_30_MSK                                    (((1U<<RF_FPGA_SDMIN_30_LEN)-1)<<RF_FPGA_SDMIN_30_POS)
#define RF_FPGA_SDMIN_30_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_30_LEN)-1)<<RF_FPGA_SDMIN_30_POS))

/* 0x69C : rf_fpga_table_31 */
#define RF_FPGA_TABLE_31_OFFSET                                 (0x69C)
#define RF_FPGA_SDMIN_31                                        RF_FPGA_SDMIN_31
#define RF_FPGA_SDMIN_31_POS                                    (0U)
#define RF_FPGA_SDMIN_31_LEN                                    (30U)
#define RF_FPGA_SDMIN_31_MSK                                    (((1U<<RF_FPGA_SDMIN_31_LEN)-1)<<RF_FPGA_SDMIN_31_POS)
#define RF_FPGA_SDMIN_31_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_31_LEN)-1)<<RF_FPGA_SDMIN_31_POS))

/* 0x6A0 : rf_fpga_table_32 */
#define RF_FPGA_TABLE_32_OFFSET                                 (0x6A0)
#define RF_FPGA_SDMIN_32                                        RF_FPGA_SDMIN_32
#define RF_FPGA_SDMIN_32_POS                                    (0U)
#define RF_FPGA_SDMIN_32_LEN                                    (30U)
#define RF_FPGA_SDMIN_32_MSK                                    (((1U<<RF_FPGA_SDMIN_32_LEN)-1)<<RF_FPGA_SDMIN_32_POS)
#define RF_FPGA_SDMIN_32_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_32_LEN)-1)<<RF_FPGA_SDMIN_32_POS))

/* 0x6A4 : rf_fpga_table_33 */
#define RF_FPGA_TABLE_33_OFFSET                                 (0x6A4)
#define RF_FPGA_SDMIN_33                                        RF_FPGA_SDMIN_33
#define RF_FPGA_SDMIN_33_POS                                    (0U)
#define RF_FPGA_SDMIN_33_LEN                                    (30U)
#define RF_FPGA_SDMIN_33_MSK                                    (((1U<<RF_FPGA_SDMIN_33_LEN)-1)<<RF_FPGA_SDMIN_33_POS)
#define RF_FPGA_SDMIN_33_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_33_LEN)-1)<<RF_FPGA_SDMIN_33_POS))

/* 0x6A8 : rf_fpga_table_34 */
#define RF_FPGA_TABLE_34_OFFSET                                 (0x6A8)
#define RF_FPGA_SDMIN_34                                        RF_FPGA_SDMIN_34
#define RF_FPGA_SDMIN_34_POS                                    (0U)
#define RF_FPGA_SDMIN_34_LEN                                    (30U)
#define RF_FPGA_SDMIN_34_MSK                                    (((1U<<RF_FPGA_SDMIN_34_LEN)-1)<<RF_FPGA_SDMIN_34_POS)
#define RF_FPGA_SDMIN_34_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_34_LEN)-1)<<RF_FPGA_SDMIN_34_POS))

/* 0x6AC : rf_fpga_table_35 */
#define RF_FPGA_TABLE_35_OFFSET                                 (0x6AC)
#define RF_FPGA_SDMIN_35                                        RF_FPGA_SDMIN_35
#define RF_FPGA_SDMIN_35_POS                                    (0U)
#define RF_FPGA_SDMIN_35_LEN                                    (30U)
#define RF_FPGA_SDMIN_35_MSK                                    (((1U<<RF_FPGA_SDMIN_35_LEN)-1)<<RF_FPGA_SDMIN_35_POS)
#define RF_FPGA_SDMIN_35_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_35_LEN)-1)<<RF_FPGA_SDMIN_35_POS))

/* 0x6B0 : rf_fpga_table_36 */
#define RF_FPGA_TABLE_36_OFFSET                                 (0x6B0)
#define RF_FPGA_SDMIN_36                                        RF_FPGA_SDMIN_36
#define RF_FPGA_SDMIN_36_POS                                    (0U)
#define RF_FPGA_SDMIN_36_LEN                                    (30U)
#define RF_FPGA_SDMIN_36_MSK                                    (((1U<<RF_FPGA_SDMIN_36_LEN)-1)<<RF_FPGA_SDMIN_36_POS)
#define RF_FPGA_SDMIN_36_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_36_LEN)-1)<<RF_FPGA_SDMIN_36_POS))

/* 0x6B4 : rf_fpga_table_37 */
#define RF_FPGA_TABLE_37_OFFSET                                 (0x6B4)
#define RF_FPGA_SDMIN_37                                        RF_FPGA_SDMIN_37
#define RF_FPGA_SDMIN_37_POS                                    (0U)
#define RF_FPGA_SDMIN_37_LEN                                    (30U)
#define RF_FPGA_SDMIN_37_MSK                                    (((1U<<RF_FPGA_SDMIN_37_LEN)-1)<<RF_FPGA_SDMIN_37_POS)
#define RF_FPGA_SDMIN_37_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_37_LEN)-1)<<RF_FPGA_SDMIN_37_POS))

/* 0x6B8 : rf_fpga_table_38 */
#define RF_FPGA_TABLE_38_OFFSET                                 (0x6B8)
#define RF_FPGA_SDMIN_38                                        RF_FPGA_SDMIN_38
#define RF_FPGA_SDMIN_38_POS                                    (0U)
#define RF_FPGA_SDMIN_38_LEN                                    (30U)
#define RF_FPGA_SDMIN_38_MSK                                    (((1U<<RF_FPGA_SDMIN_38_LEN)-1)<<RF_FPGA_SDMIN_38_POS)
#define RF_FPGA_SDMIN_38_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_38_LEN)-1)<<RF_FPGA_SDMIN_38_POS))

/* 0x6BC : rf_fpga_table_39 */
#define RF_FPGA_TABLE_39_OFFSET                                 (0x6BC)
#define RF_FPGA_SDMIN_39                                        RF_FPGA_SDMIN_39
#define RF_FPGA_SDMIN_39_POS                                    (0U)
#define RF_FPGA_SDMIN_39_LEN                                    (30U)
#define RF_FPGA_SDMIN_39_MSK                                    (((1U<<RF_FPGA_SDMIN_39_LEN)-1)<<RF_FPGA_SDMIN_39_POS)
#define RF_FPGA_SDMIN_39_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_39_LEN)-1)<<RF_FPGA_SDMIN_39_POS))

/* 0x6C0 : rf_fpga_table_40 */
#define RF_FPGA_TABLE_40_OFFSET                                 (0x6C0)
#define RF_FPGA_SDMIN_40                                        RF_FPGA_SDMIN_40
#define RF_FPGA_SDMIN_40_POS                                    (0U)
#define RF_FPGA_SDMIN_40_LEN                                    (30U)
#define RF_FPGA_SDMIN_40_MSK                                    (((1U<<RF_FPGA_SDMIN_40_LEN)-1)<<RF_FPGA_SDMIN_40_POS)
#define RF_FPGA_SDMIN_40_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_40_LEN)-1)<<RF_FPGA_SDMIN_40_POS))

/* 0x6C4 : rf_fpga_table_41 */
#define RF_FPGA_TABLE_41_OFFSET                                 (0x6C4)
#define RF_FPGA_SDMIN_41                                        RF_FPGA_SDMIN_41
#define RF_FPGA_SDMIN_41_POS                                    (0U)
#define RF_FPGA_SDMIN_41_LEN                                    (30U)
#define RF_FPGA_SDMIN_41_MSK                                    (((1U<<RF_FPGA_SDMIN_41_LEN)-1)<<RF_FPGA_SDMIN_41_POS)
#define RF_FPGA_SDMIN_41_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_41_LEN)-1)<<RF_FPGA_SDMIN_41_POS))

/* 0x6C8 : rf_fpga_table_42 */
#define RF_FPGA_TABLE_42_OFFSET                                 (0x6C8)
#define RF_FPGA_SDMIN_42                                        RF_FPGA_SDMIN_42
#define RF_FPGA_SDMIN_42_POS                                    (0U)
#define RF_FPGA_SDMIN_42_LEN                                    (30U)
#define RF_FPGA_SDMIN_42_MSK                                    (((1U<<RF_FPGA_SDMIN_42_LEN)-1)<<RF_FPGA_SDMIN_42_POS)
#define RF_FPGA_SDMIN_42_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_42_LEN)-1)<<RF_FPGA_SDMIN_42_POS))

/* 0x6CC : rf_fpga_table_43 */
#define RF_FPGA_TABLE_43_OFFSET                                 (0x6CC)
#define RF_FPGA_SDMIN_43                                        RF_FPGA_SDMIN_43
#define RF_FPGA_SDMIN_43_POS                                    (0U)
#define RF_FPGA_SDMIN_43_LEN                                    (30U)
#define RF_FPGA_SDMIN_43_MSK                                    (((1U<<RF_FPGA_SDMIN_43_LEN)-1)<<RF_FPGA_SDMIN_43_POS)
#define RF_FPGA_SDMIN_43_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_43_LEN)-1)<<RF_FPGA_SDMIN_43_POS))

/* 0x6D0 : rf_fpga_table_44 */
#define RF_FPGA_TABLE_44_OFFSET                                 (0x6D0)
#define RF_FPGA_SDMIN_44                                        RF_FPGA_SDMIN_44
#define RF_FPGA_SDMIN_44_POS                                    (0U)
#define RF_FPGA_SDMIN_44_LEN                                    (30U)
#define RF_FPGA_SDMIN_44_MSK                                    (((1U<<RF_FPGA_SDMIN_44_LEN)-1)<<RF_FPGA_SDMIN_44_POS)
#define RF_FPGA_SDMIN_44_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_44_LEN)-1)<<RF_FPGA_SDMIN_44_POS))

/* 0x6D4 : rf_fpga_table_45 */
#define RF_FPGA_TABLE_45_OFFSET                                 (0x6D4)
#define RF_FPGA_SDMIN_45                                        RF_FPGA_SDMIN_45
#define RF_FPGA_SDMIN_45_POS                                    (0U)
#define RF_FPGA_SDMIN_45_LEN                                    (30U)
#define RF_FPGA_SDMIN_45_MSK                                    (((1U<<RF_FPGA_SDMIN_45_LEN)-1)<<RF_FPGA_SDMIN_45_POS)
#define RF_FPGA_SDMIN_45_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_45_LEN)-1)<<RF_FPGA_SDMIN_45_POS))

/* 0x6D8 : rf_fpga_table_46 */
#define RF_FPGA_TABLE_46_OFFSET                                 (0x6D8)
#define RF_FPGA_SDMIN_46                                        RF_FPGA_SDMIN_46
#define RF_FPGA_SDMIN_46_POS                                    (0U)
#define RF_FPGA_SDMIN_46_LEN                                    (30U)
#define RF_FPGA_SDMIN_46_MSK                                    (((1U<<RF_FPGA_SDMIN_46_LEN)-1)<<RF_FPGA_SDMIN_46_POS)
#define RF_FPGA_SDMIN_46_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_46_LEN)-1)<<RF_FPGA_SDMIN_46_POS))

/* 0x6DC : rf_fpga_table_47 */
#define RF_FPGA_TABLE_47_OFFSET                                 (0x6DC)
#define RF_FPGA_SDMIN_47                                        RF_FPGA_SDMIN_47
#define RF_FPGA_SDMIN_47_POS                                    (0U)
#define RF_FPGA_SDMIN_47_LEN                                    (30U)
#define RF_FPGA_SDMIN_47_MSK                                    (((1U<<RF_FPGA_SDMIN_47_LEN)-1)<<RF_FPGA_SDMIN_47_POS)
#define RF_FPGA_SDMIN_47_UMSK                                   (~(((1U<<RF_FPGA_SDMIN_47_LEN)-1)<<RF_FPGA_SDMIN_47_POS))

/* 0x6E0 : rf_fpga_table_48 */
#define RF_FPGA_TABLE_48_OFFSET                                 (0x6E0)
#define RF_FPGA_FREQ_CW_3                                       RF_FPGA_FREQ_CW_3
#define RF_FPGA_FREQ_CW_3_POS                                   (0U)
#define RF_FPGA_FREQ_CW_3_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_3_MSK                                   (((1U<<RF_FPGA_FREQ_CW_3_LEN)-1)<<RF_FPGA_FREQ_CW_3_POS)
#define RF_FPGA_FREQ_CW_3_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_3_LEN)-1)<<RF_FPGA_FREQ_CW_3_POS))
#define RF_FPGA_FREQ_CW_2                                       RF_FPGA_FREQ_CW_2
#define RF_FPGA_FREQ_CW_2_POS                                   (8U)
#define RF_FPGA_FREQ_CW_2_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_2_MSK                                   (((1U<<RF_FPGA_FREQ_CW_2_LEN)-1)<<RF_FPGA_FREQ_CW_2_POS)
#define RF_FPGA_FREQ_CW_2_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_2_LEN)-1)<<RF_FPGA_FREQ_CW_2_POS))
#define RF_FPGA_FREQ_CW_1                                       RF_FPGA_FREQ_CW_1
#define RF_FPGA_FREQ_CW_1_POS                                   (16U)
#define RF_FPGA_FREQ_CW_1_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_1_MSK                                   (((1U<<RF_FPGA_FREQ_CW_1_LEN)-1)<<RF_FPGA_FREQ_CW_1_POS)
#define RF_FPGA_FREQ_CW_1_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_1_LEN)-1)<<RF_FPGA_FREQ_CW_1_POS))
#define RF_FPGA_FREQ_CW_0                                       RF_FPGA_FREQ_CW_0
#define RF_FPGA_FREQ_CW_0_POS                                   (24U)
#define RF_FPGA_FREQ_CW_0_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_0_MSK                                   (((1U<<RF_FPGA_FREQ_CW_0_LEN)-1)<<RF_FPGA_FREQ_CW_0_POS)
#define RF_FPGA_FREQ_CW_0_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_0_LEN)-1)<<RF_FPGA_FREQ_CW_0_POS))

/* 0x6E4 : rf_fpga_table_49 */
#define RF_FPGA_TABLE_49_OFFSET                                 (0x6E4)
#define RF_FPGA_FREQ_CW_7                                       RF_FPGA_FREQ_CW_7
#define RF_FPGA_FREQ_CW_7_POS                                   (0U)
#define RF_FPGA_FREQ_CW_7_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_7_MSK                                   (((1U<<RF_FPGA_FREQ_CW_7_LEN)-1)<<RF_FPGA_FREQ_CW_7_POS)
#define RF_FPGA_FREQ_CW_7_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_7_LEN)-1)<<RF_FPGA_FREQ_CW_7_POS))
#define RF_FPGA_FREQ_CW_6                                       RF_FPGA_FREQ_CW_6
#define RF_FPGA_FREQ_CW_6_POS                                   (8U)
#define RF_FPGA_FREQ_CW_6_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_6_MSK                                   (((1U<<RF_FPGA_FREQ_CW_6_LEN)-1)<<RF_FPGA_FREQ_CW_6_POS)
#define RF_FPGA_FREQ_CW_6_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_6_LEN)-1)<<RF_FPGA_FREQ_CW_6_POS))
#define RF_FPGA_FREQ_CW_5                                       RF_FPGA_FREQ_CW_5
#define RF_FPGA_FREQ_CW_5_POS                                   (16U)
#define RF_FPGA_FREQ_CW_5_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_5_MSK                                   (((1U<<RF_FPGA_FREQ_CW_5_LEN)-1)<<RF_FPGA_FREQ_CW_5_POS)
#define RF_FPGA_FREQ_CW_5_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_5_LEN)-1)<<RF_FPGA_FREQ_CW_5_POS))
#define RF_FPGA_FREQ_CW_4                                       RF_FPGA_FREQ_CW_4
#define RF_FPGA_FREQ_CW_4_POS                                   (24U)
#define RF_FPGA_FREQ_CW_4_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_4_MSK                                   (((1U<<RF_FPGA_FREQ_CW_4_LEN)-1)<<RF_FPGA_FREQ_CW_4_POS)
#define RF_FPGA_FREQ_CW_4_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_4_LEN)-1)<<RF_FPGA_FREQ_CW_4_POS))

/* 0x6E8 : rf_fpga_table_50 */
#define RF_FPGA_TABLE_50_OFFSET                                 (0x6E8)
#define RF_FPGA_FREQ_CW_11                                      RF_FPGA_FREQ_CW_11
#define RF_FPGA_FREQ_CW_11_POS                                  (0U)
#define RF_FPGA_FREQ_CW_11_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_11_MSK                                  (((1U<<RF_FPGA_FREQ_CW_11_LEN)-1)<<RF_FPGA_FREQ_CW_11_POS)
#define RF_FPGA_FREQ_CW_11_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_11_LEN)-1)<<RF_FPGA_FREQ_CW_11_POS))
#define RF_FPGA_FREQ_CW_10                                      RF_FPGA_FREQ_CW_10
#define RF_FPGA_FREQ_CW_10_POS                                  (8U)
#define RF_FPGA_FREQ_CW_10_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_10_MSK                                  (((1U<<RF_FPGA_FREQ_CW_10_LEN)-1)<<RF_FPGA_FREQ_CW_10_POS)
#define RF_FPGA_FREQ_CW_10_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_10_LEN)-1)<<RF_FPGA_FREQ_CW_10_POS))
#define RF_FPGA_FREQ_CW_9                                       RF_FPGA_FREQ_CW_9
#define RF_FPGA_FREQ_CW_9_POS                                   (16U)
#define RF_FPGA_FREQ_CW_9_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_9_MSK                                   (((1U<<RF_FPGA_FREQ_CW_9_LEN)-1)<<RF_FPGA_FREQ_CW_9_POS)
#define RF_FPGA_FREQ_CW_9_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_9_LEN)-1)<<RF_FPGA_FREQ_CW_9_POS))
#define RF_FPGA_FREQ_CW_8                                       RF_FPGA_FREQ_CW_8
#define RF_FPGA_FREQ_CW_8_POS                                   (24U)
#define RF_FPGA_FREQ_CW_8_LEN                                   (7U)
#define RF_FPGA_FREQ_CW_8_MSK                                   (((1U<<RF_FPGA_FREQ_CW_8_LEN)-1)<<RF_FPGA_FREQ_CW_8_POS)
#define RF_FPGA_FREQ_CW_8_UMSK                                  (~(((1U<<RF_FPGA_FREQ_CW_8_LEN)-1)<<RF_FPGA_FREQ_CW_8_POS))

/* 0x6EC : rf_fpga_table_51 */
#define RF_FPGA_TABLE_51_OFFSET                                 (0x6EC)
#define RF_FPGA_FREQ_CW_15                                      RF_FPGA_FREQ_CW_15
#define RF_FPGA_FREQ_CW_15_POS                                  (0U)
#define RF_FPGA_FREQ_CW_15_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_15_MSK                                  (((1U<<RF_FPGA_FREQ_CW_15_LEN)-1)<<RF_FPGA_FREQ_CW_15_POS)
#define RF_FPGA_FREQ_CW_15_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_15_LEN)-1)<<RF_FPGA_FREQ_CW_15_POS))
#define RF_FPGA_FREQ_CW_14                                      RF_FPGA_FREQ_CW_14
#define RF_FPGA_FREQ_CW_14_POS                                  (8U)
#define RF_FPGA_FREQ_CW_14_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_14_MSK                                  (((1U<<RF_FPGA_FREQ_CW_14_LEN)-1)<<RF_FPGA_FREQ_CW_14_POS)
#define RF_FPGA_FREQ_CW_14_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_14_LEN)-1)<<RF_FPGA_FREQ_CW_14_POS))
#define RF_FPGA_FREQ_CW_13                                      RF_FPGA_FREQ_CW_13
#define RF_FPGA_FREQ_CW_13_POS                                  (16U)
#define RF_FPGA_FREQ_CW_13_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_13_MSK                                  (((1U<<RF_FPGA_FREQ_CW_13_LEN)-1)<<RF_FPGA_FREQ_CW_13_POS)
#define RF_FPGA_FREQ_CW_13_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_13_LEN)-1)<<RF_FPGA_FREQ_CW_13_POS))
#define RF_FPGA_FREQ_CW_12                                      RF_FPGA_FREQ_CW_12
#define RF_FPGA_FREQ_CW_12_POS                                  (24U)
#define RF_FPGA_FREQ_CW_12_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_12_MSK                                  (((1U<<RF_FPGA_FREQ_CW_12_LEN)-1)<<RF_FPGA_FREQ_CW_12_POS)
#define RF_FPGA_FREQ_CW_12_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_12_LEN)-1)<<RF_FPGA_FREQ_CW_12_POS))

/* 0x6F0 : rf_fpga_table_52 */
#define RF_FPGA_TABLE_52_OFFSET                                 (0x6F0)
#define RF_FPGA_FREQ_CW_19                                      RF_FPGA_FREQ_CW_19
#define RF_FPGA_FREQ_CW_19_POS                                  (0U)
#define RF_FPGA_FREQ_CW_19_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_19_MSK                                  (((1U<<RF_FPGA_FREQ_CW_19_LEN)-1)<<RF_FPGA_FREQ_CW_19_POS)
#define RF_FPGA_FREQ_CW_19_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_19_LEN)-1)<<RF_FPGA_FREQ_CW_19_POS))
#define RF_FPGA_FREQ_CW_18                                      RF_FPGA_FREQ_CW_18
#define RF_FPGA_FREQ_CW_18_POS                                  (8U)
#define RF_FPGA_FREQ_CW_18_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_18_MSK                                  (((1U<<RF_FPGA_FREQ_CW_18_LEN)-1)<<RF_FPGA_FREQ_CW_18_POS)
#define RF_FPGA_FREQ_CW_18_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_18_LEN)-1)<<RF_FPGA_FREQ_CW_18_POS))
#define RF_FPGA_FREQ_CW_17                                      RF_FPGA_FREQ_CW_17
#define RF_FPGA_FREQ_CW_17_POS                                  (16U)
#define RF_FPGA_FREQ_CW_17_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_17_MSK                                  (((1U<<RF_FPGA_FREQ_CW_17_LEN)-1)<<RF_FPGA_FREQ_CW_17_POS)
#define RF_FPGA_FREQ_CW_17_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_17_LEN)-1)<<RF_FPGA_FREQ_CW_17_POS))
#define RF_FPGA_FREQ_CW_16                                      RF_FPGA_FREQ_CW_16
#define RF_FPGA_FREQ_CW_16_POS                                  (24U)
#define RF_FPGA_FREQ_CW_16_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_16_MSK                                  (((1U<<RF_FPGA_FREQ_CW_16_LEN)-1)<<RF_FPGA_FREQ_CW_16_POS)
#define RF_FPGA_FREQ_CW_16_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_16_LEN)-1)<<RF_FPGA_FREQ_CW_16_POS))

/* 0x6F4 : rf_fpga_table_53 */
#define RF_FPGA_TABLE_53_OFFSET                                 (0x6F4)
#define RF_FPGA_FREQ_CW_23                                      RF_FPGA_FREQ_CW_23
#define RF_FPGA_FREQ_CW_23_POS                                  (0U)
#define RF_FPGA_FREQ_CW_23_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_23_MSK                                  (((1U<<RF_FPGA_FREQ_CW_23_LEN)-1)<<RF_FPGA_FREQ_CW_23_POS)
#define RF_FPGA_FREQ_CW_23_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_23_LEN)-1)<<RF_FPGA_FREQ_CW_23_POS))
#define RF_FPGA_FREQ_CW_22                                      RF_FPGA_FREQ_CW_22
#define RF_FPGA_FREQ_CW_22_POS                                  (8U)
#define RF_FPGA_FREQ_CW_22_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_22_MSK                                  (((1U<<RF_FPGA_FREQ_CW_22_LEN)-1)<<RF_FPGA_FREQ_CW_22_POS)
#define RF_FPGA_FREQ_CW_22_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_22_LEN)-1)<<RF_FPGA_FREQ_CW_22_POS))
#define RF_FPGA_FREQ_CW_21                                      RF_FPGA_FREQ_CW_21
#define RF_FPGA_FREQ_CW_21_POS                                  (16U)
#define RF_FPGA_FREQ_CW_21_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_21_MSK                                  (((1U<<RF_FPGA_FREQ_CW_21_LEN)-1)<<RF_FPGA_FREQ_CW_21_POS)
#define RF_FPGA_FREQ_CW_21_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_21_LEN)-1)<<RF_FPGA_FREQ_CW_21_POS))
#define RF_FPGA_FREQ_CW_20                                      RF_FPGA_FREQ_CW_20
#define RF_FPGA_FREQ_CW_20_POS                                  (24U)
#define RF_FPGA_FREQ_CW_20_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_20_MSK                                  (((1U<<RF_FPGA_FREQ_CW_20_LEN)-1)<<RF_FPGA_FREQ_CW_20_POS)
#define RF_FPGA_FREQ_CW_20_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_20_LEN)-1)<<RF_FPGA_FREQ_CW_20_POS))

/* 0x6F8 : rf_fpga_table_54 */
#define RF_FPGA_TABLE_54_OFFSET                                 (0x6F8)
#define RF_FPGA_FREQ_CW_27                                      RF_FPGA_FREQ_CW_27
#define RF_FPGA_FREQ_CW_27_POS                                  (0U)
#define RF_FPGA_FREQ_CW_27_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_27_MSK                                  (((1U<<RF_FPGA_FREQ_CW_27_LEN)-1)<<RF_FPGA_FREQ_CW_27_POS)
#define RF_FPGA_FREQ_CW_27_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_27_LEN)-1)<<RF_FPGA_FREQ_CW_27_POS))
#define RF_FPGA_FREQ_CW_26                                      RF_FPGA_FREQ_CW_26
#define RF_FPGA_FREQ_CW_26_POS                                  (8U)
#define RF_FPGA_FREQ_CW_26_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_26_MSK                                  (((1U<<RF_FPGA_FREQ_CW_26_LEN)-1)<<RF_FPGA_FREQ_CW_26_POS)
#define RF_FPGA_FREQ_CW_26_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_26_LEN)-1)<<RF_FPGA_FREQ_CW_26_POS))
#define RF_FPGA_FREQ_CW_25                                      RF_FPGA_FREQ_CW_25
#define RF_FPGA_FREQ_CW_25_POS                                  (16U)
#define RF_FPGA_FREQ_CW_25_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_25_MSK                                  (((1U<<RF_FPGA_FREQ_CW_25_LEN)-1)<<RF_FPGA_FREQ_CW_25_POS)
#define RF_FPGA_FREQ_CW_25_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_25_LEN)-1)<<RF_FPGA_FREQ_CW_25_POS))
#define RF_FPGA_FREQ_CW_24                                      RF_FPGA_FREQ_CW_24
#define RF_FPGA_FREQ_CW_24_POS                                  (24U)
#define RF_FPGA_FREQ_CW_24_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_24_MSK                                  (((1U<<RF_FPGA_FREQ_CW_24_LEN)-1)<<RF_FPGA_FREQ_CW_24_POS)
#define RF_FPGA_FREQ_CW_24_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_24_LEN)-1)<<RF_FPGA_FREQ_CW_24_POS))

/* 0x6FC : rf_fpga_table_55 */
#define RF_FPGA_TABLE_55_OFFSET                                 (0x6FC)
#define RF_FPGA_FREQ_CW_31                                      RF_FPGA_FREQ_CW_31
#define RF_FPGA_FREQ_CW_31_POS                                  (0U)
#define RF_FPGA_FREQ_CW_31_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_31_MSK                                  (((1U<<RF_FPGA_FREQ_CW_31_LEN)-1)<<RF_FPGA_FREQ_CW_31_POS)
#define RF_FPGA_FREQ_CW_31_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_31_LEN)-1)<<RF_FPGA_FREQ_CW_31_POS))
#define RF_FPGA_FREQ_CW_30                                      RF_FPGA_FREQ_CW_30
#define RF_FPGA_FREQ_CW_30_POS                                  (8U)
#define RF_FPGA_FREQ_CW_30_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_30_MSK                                  (((1U<<RF_FPGA_FREQ_CW_30_LEN)-1)<<RF_FPGA_FREQ_CW_30_POS)
#define RF_FPGA_FREQ_CW_30_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_30_LEN)-1)<<RF_FPGA_FREQ_CW_30_POS))
#define RF_FPGA_FREQ_CW_29                                      RF_FPGA_FREQ_CW_29
#define RF_FPGA_FREQ_CW_29_POS                                  (16U)
#define RF_FPGA_FREQ_CW_29_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_29_MSK                                  (((1U<<RF_FPGA_FREQ_CW_29_LEN)-1)<<RF_FPGA_FREQ_CW_29_POS)
#define RF_FPGA_FREQ_CW_29_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_29_LEN)-1)<<RF_FPGA_FREQ_CW_29_POS))
#define RF_FPGA_FREQ_CW_28                                      RF_FPGA_FREQ_CW_28
#define RF_FPGA_FREQ_CW_28_POS                                  (24U)
#define RF_FPGA_FREQ_CW_28_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_28_MSK                                  (((1U<<RF_FPGA_FREQ_CW_28_LEN)-1)<<RF_FPGA_FREQ_CW_28_POS)
#define RF_FPGA_FREQ_CW_28_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_28_LEN)-1)<<RF_FPGA_FREQ_CW_28_POS))

/* 0x700 : rf_fpga_table_56 */
#define RF_FPGA_TABLE_56_OFFSET                                 (0x700)
#define RF_FPGA_FREQ_CW_35                                      RF_FPGA_FREQ_CW_35
#define RF_FPGA_FREQ_CW_35_POS                                  (0U)
#define RF_FPGA_FREQ_CW_35_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_35_MSK                                  (((1U<<RF_FPGA_FREQ_CW_35_LEN)-1)<<RF_FPGA_FREQ_CW_35_POS)
#define RF_FPGA_FREQ_CW_35_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_35_LEN)-1)<<RF_FPGA_FREQ_CW_35_POS))
#define RF_FPGA_FREQ_CW_34                                      RF_FPGA_FREQ_CW_34
#define RF_FPGA_FREQ_CW_34_POS                                  (8U)
#define RF_FPGA_FREQ_CW_34_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_34_MSK                                  (((1U<<RF_FPGA_FREQ_CW_34_LEN)-1)<<RF_FPGA_FREQ_CW_34_POS)
#define RF_FPGA_FREQ_CW_34_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_34_LEN)-1)<<RF_FPGA_FREQ_CW_34_POS))
#define RF_FPGA_FREQ_CW_33                                      RF_FPGA_FREQ_CW_33
#define RF_FPGA_FREQ_CW_33_POS                                  (16U)
#define RF_FPGA_FREQ_CW_33_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_33_MSK                                  (((1U<<RF_FPGA_FREQ_CW_33_LEN)-1)<<RF_FPGA_FREQ_CW_33_POS)
#define RF_FPGA_FREQ_CW_33_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_33_LEN)-1)<<RF_FPGA_FREQ_CW_33_POS))
#define RF_FPGA_FREQ_CW_32                                      RF_FPGA_FREQ_CW_32
#define RF_FPGA_FREQ_CW_32_POS                                  (24U)
#define RF_FPGA_FREQ_CW_32_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_32_MSK                                  (((1U<<RF_FPGA_FREQ_CW_32_LEN)-1)<<RF_FPGA_FREQ_CW_32_POS)
#define RF_FPGA_FREQ_CW_32_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_32_LEN)-1)<<RF_FPGA_FREQ_CW_32_POS))

/* 0x704 : rf_fpga_table_57 */
#define RF_FPGA_TABLE_57_OFFSET                                 (0x704)
#define RF_FPGA_FREQ_CW_39                                      RF_FPGA_FREQ_CW_39
#define RF_FPGA_FREQ_CW_39_POS                                  (0U)
#define RF_FPGA_FREQ_CW_39_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_39_MSK                                  (((1U<<RF_FPGA_FREQ_CW_39_LEN)-1)<<RF_FPGA_FREQ_CW_39_POS)
#define RF_FPGA_FREQ_CW_39_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_39_LEN)-1)<<RF_FPGA_FREQ_CW_39_POS))
#define RF_FPGA_FREQ_CW_38                                      RF_FPGA_FREQ_CW_38
#define RF_FPGA_FREQ_CW_38_POS                                  (8U)
#define RF_FPGA_FREQ_CW_38_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_38_MSK                                  (((1U<<RF_FPGA_FREQ_CW_38_LEN)-1)<<RF_FPGA_FREQ_CW_38_POS)
#define RF_FPGA_FREQ_CW_38_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_38_LEN)-1)<<RF_FPGA_FREQ_CW_38_POS))
#define RF_FPGA_FREQ_CW_37                                      RF_FPGA_FREQ_CW_37
#define RF_FPGA_FREQ_CW_37_POS                                  (16U)
#define RF_FPGA_FREQ_CW_37_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_37_MSK                                  (((1U<<RF_FPGA_FREQ_CW_37_LEN)-1)<<RF_FPGA_FREQ_CW_37_POS)
#define RF_FPGA_FREQ_CW_37_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_37_LEN)-1)<<RF_FPGA_FREQ_CW_37_POS))
#define RF_FPGA_FREQ_CW_36                                      RF_FPGA_FREQ_CW_36
#define RF_FPGA_FREQ_CW_36_POS                                  (24U)
#define RF_FPGA_FREQ_CW_36_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_36_MSK                                  (((1U<<RF_FPGA_FREQ_CW_36_LEN)-1)<<RF_FPGA_FREQ_CW_36_POS)
#define RF_FPGA_FREQ_CW_36_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_36_LEN)-1)<<RF_FPGA_FREQ_CW_36_POS))

/* 0x708 : rf_fpga_table_58 */
#define RF_FPGA_TABLE_58_OFFSET                                 (0x708)
#define RF_FPGA_FREQ_CW_43                                      RF_FPGA_FREQ_CW_43
#define RF_FPGA_FREQ_CW_43_POS                                  (0U)
#define RF_FPGA_FREQ_CW_43_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_43_MSK                                  (((1U<<RF_FPGA_FREQ_CW_43_LEN)-1)<<RF_FPGA_FREQ_CW_43_POS)
#define RF_FPGA_FREQ_CW_43_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_43_LEN)-1)<<RF_FPGA_FREQ_CW_43_POS))
#define RF_FPGA_FREQ_CW_42                                      RF_FPGA_FREQ_CW_42
#define RF_FPGA_FREQ_CW_42_POS                                  (8U)
#define RF_FPGA_FREQ_CW_42_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_42_MSK                                  (((1U<<RF_FPGA_FREQ_CW_42_LEN)-1)<<RF_FPGA_FREQ_CW_42_POS)
#define RF_FPGA_FREQ_CW_42_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_42_LEN)-1)<<RF_FPGA_FREQ_CW_42_POS))
#define RF_FPGA_FREQ_CW_41                                      RF_FPGA_FREQ_CW_41
#define RF_FPGA_FREQ_CW_41_POS                                  (16U)
#define RF_FPGA_FREQ_CW_41_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_41_MSK                                  (((1U<<RF_FPGA_FREQ_CW_41_LEN)-1)<<RF_FPGA_FREQ_CW_41_POS)
#define RF_FPGA_FREQ_CW_41_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_41_LEN)-1)<<RF_FPGA_FREQ_CW_41_POS))
#define RF_FPGA_FREQ_CW_40                                      RF_FPGA_FREQ_CW_40
#define RF_FPGA_FREQ_CW_40_POS                                  (24U)
#define RF_FPGA_FREQ_CW_40_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_40_MSK                                  (((1U<<RF_FPGA_FREQ_CW_40_LEN)-1)<<RF_FPGA_FREQ_CW_40_POS)
#define RF_FPGA_FREQ_CW_40_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_40_LEN)-1)<<RF_FPGA_FREQ_CW_40_POS))

/* 0x70C : rf_fpga_table_59 */
#define RF_FPGA_TABLE_59_OFFSET                                 (0x70C)
#define RF_FPGA_FREQ_CW_47                                      RF_FPGA_FREQ_CW_47
#define RF_FPGA_FREQ_CW_47_POS                                  (0U)
#define RF_FPGA_FREQ_CW_47_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_47_MSK                                  (((1U<<RF_FPGA_FREQ_CW_47_LEN)-1)<<RF_FPGA_FREQ_CW_47_POS)
#define RF_FPGA_FREQ_CW_47_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_47_LEN)-1)<<RF_FPGA_FREQ_CW_47_POS))
#define RF_FPGA_FREQ_CW_46                                      RF_FPGA_FREQ_CW_46
#define RF_FPGA_FREQ_CW_46_POS                                  (8U)
#define RF_FPGA_FREQ_CW_46_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_46_MSK                                  (((1U<<RF_FPGA_FREQ_CW_46_LEN)-1)<<RF_FPGA_FREQ_CW_46_POS)
#define RF_FPGA_FREQ_CW_46_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_46_LEN)-1)<<RF_FPGA_FREQ_CW_46_POS))
#define RF_FPGA_FREQ_CW_45                                      RF_FPGA_FREQ_CW_45
#define RF_FPGA_FREQ_CW_45_POS                                  (16U)
#define RF_FPGA_FREQ_CW_45_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_45_MSK                                  (((1U<<RF_FPGA_FREQ_CW_45_LEN)-1)<<RF_FPGA_FREQ_CW_45_POS)
#define RF_FPGA_FREQ_CW_45_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_45_LEN)-1)<<RF_FPGA_FREQ_CW_45_POS))
#define RF_FPGA_FREQ_CW_44                                      RF_FPGA_FREQ_CW_44
#define RF_FPGA_FREQ_CW_44_POS                                  (24U)
#define RF_FPGA_FREQ_CW_44_LEN                                  (7U)
#define RF_FPGA_FREQ_CW_44_MSK                                  (((1U<<RF_FPGA_FREQ_CW_44_LEN)-1)<<RF_FPGA_FREQ_CW_44_POS)
#define RF_FPGA_FREQ_CW_44_UMSK                                 (~(((1U<<RF_FPGA_FREQ_CW_44_LEN)-1)<<RF_FPGA_FREQ_CW_44_POS))

/* 0x710 : rf_fpga_ctrl_8 */
#define RF_FPGA_CTRL_8_OFFSET                                   (0x710)
#define RF_FPGA_CMD_ENABLE                                      RF_FPGA_CMD_ENABLE
#define RF_FPGA_CMD_ENABLE_POS                                  (0U)
#define RF_FPGA_CMD_ENABLE_LEN                                  (16U)
#define RF_FPGA_CMD_ENABLE_MSK                                  (((1U<<RF_FPGA_CMD_ENABLE_LEN)-1)<<RF_FPGA_CMD_ENABLE_POS)
#define RF_FPGA_CMD_ENABLE_UMSK                                 (~(((1U<<RF_FPGA_CMD_ENABLE_LEN)-1)<<RF_FPGA_CMD_ENABLE_POS))

/* 0x714 : rf_fpga_table_60 */
#define RF_FPGA_TABLE_60_OFFSET                                 (0x714)
#define RF_FPGA_FREQ_CW_RX_3                                    RF_FPGA_FREQ_CW_RX_3
#define RF_FPGA_FREQ_CW_RX_3_POS                                (0U)
#define RF_FPGA_FREQ_CW_RX_3_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_3_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_3_LEN)-1)<<RF_FPGA_FREQ_CW_RX_3_POS)
#define RF_FPGA_FREQ_CW_RX_3_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_3_LEN)-1)<<RF_FPGA_FREQ_CW_RX_3_POS))
#define RF_FPGA_FREQ_CW_RX_2                                    RF_FPGA_FREQ_CW_RX_2
#define RF_FPGA_FREQ_CW_RX_2_POS                                (8U)
#define RF_FPGA_FREQ_CW_RX_2_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_2_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_2_LEN)-1)<<RF_FPGA_FREQ_CW_RX_2_POS)
#define RF_FPGA_FREQ_CW_RX_2_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_2_LEN)-1)<<RF_FPGA_FREQ_CW_RX_2_POS))
#define RF_FPGA_FREQ_CW_RX_1                                    RF_FPGA_FREQ_CW_RX_1
#define RF_FPGA_FREQ_CW_RX_1_POS                                (16U)
#define RF_FPGA_FREQ_CW_RX_1_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_1_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_1_LEN)-1)<<RF_FPGA_FREQ_CW_RX_1_POS)
#define RF_FPGA_FREQ_CW_RX_1_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_1_LEN)-1)<<RF_FPGA_FREQ_CW_RX_1_POS))
#define RF_FPGA_FREQ_CW_RX_0                                    RF_FPGA_FREQ_CW_RX_0
#define RF_FPGA_FREQ_CW_RX_0_POS                                (24U)
#define RF_FPGA_FREQ_CW_RX_0_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_0_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_0_LEN)-1)<<RF_FPGA_FREQ_CW_RX_0_POS)
#define RF_FPGA_FREQ_CW_RX_0_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_0_LEN)-1)<<RF_FPGA_FREQ_CW_RX_0_POS))

/* 0x718 : rf_fpga_table_61 */
#define RF_FPGA_TABLE_61_OFFSET                                 (0x718)
#define RF_FPGA_FREQ_CW_RX_7                                    RF_FPGA_FREQ_CW_RX_7
#define RF_FPGA_FREQ_CW_RX_7_POS                                (0U)
#define RF_FPGA_FREQ_CW_RX_7_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_7_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_7_LEN)-1)<<RF_FPGA_FREQ_CW_RX_7_POS)
#define RF_FPGA_FREQ_CW_RX_7_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_7_LEN)-1)<<RF_FPGA_FREQ_CW_RX_7_POS))
#define RF_FPGA_FREQ_CW_RX_6                                    RF_FPGA_FREQ_CW_RX_6
#define RF_FPGA_FREQ_CW_RX_6_POS                                (8U)
#define RF_FPGA_FREQ_CW_RX_6_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_6_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_6_LEN)-1)<<RF_FPGA_FREQ_CW_RX_6_POS)
#define RF_FPGA_FREQ_CW_RX_6_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_6_LEN)-1)<<RF_FPGA_FREQ_CW_RX_6_POS))
#define RF_FPGA_FREQ_CW_RX_5                                    RF_FPGA_FREQ_CW_RX_5
#define RF_FPGA_FREQ_CW_RX_5_POS                                (16U)
#define RF_FPGA_FREQ_CW_RX_5_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_5_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_5_LEN)-1)<<RF_FPGA_FREQ_CW_RX_5_POS)
#define RF_FPGA_FREQ_CW_RX_5_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_5_LEN)-1)<<RF_FPGA_FREQ_CW_RX_5_POS))
#define RF_FPGA_FREQ_CW_RX_4                                    RF_FPGA_FREQ_CW_RX_4
#define RF_FPGA_FREQ_CW_RX_4_POS                                (24U)
#define RF_FPGA_FREQ_CW_RX_4_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_4_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_4_LEN)-1)<<RF_FPGA_FREQ_CW_RX_4_POS)
#define RF_FPGA_FREQ_CW_RX_4_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_4_LEN)-1)<<RF_FPGA_FREQ_CW_RX_4_POS))

/* 0x71C : rf_fpga_table_62 */
#define RF_FPGA_TABLE_62_OFFSET                                 (0x71C)
#define RF_FPGA_FREQ_CW_RX_11                                   RF_FPGA_FREQ_CW_RX_11
#define RF_FPGA_FREQ_CW_RX_11_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_11_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_11_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_11_LEN)-1)<<RF_FPGA_FREQ_CW_RX_11_POS)
#define RF_FPGA_FREQ_CW_RX_11_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_11_LEN)-1)<<RF_FPGA_FREQ_CW_RX_11_POS))
#define RF_FPGA_FREQ_CW_RX_10                                   RF_FPGA_FREQ_CW_RX_10
#define RF_FPGA_FREQ_CW_RX_10_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_10_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_10_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_10_LEN)-1)<<RF_FPGA_FREQ_CW_RX_10_POS)
#define RF_FPGA_FREQ_CW_RX_10_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_10_LEN)-1)<<RF_FPGA_FREQ_CW_RX_10_POS))
#define RF_FPGA_FREQ_CW_RX_9                                    RF_FPGA_FREQ_CW_RX_9
#define RF_FPGA_FREQ_CW_RX_9_POS                                (16U)
#define RF_FPGA_FREQ_CW_RX_9_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_9_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_9_LEN)-1)<<RF_FPGA_FREQ_CW_RX_9_POS)
#define RF_FPGA_FREQ_CW_RX_9_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_9_LEN)-1)<<RF_FPGA_FREQ_CW_RX_9_POS))
#define RF_FPGA_FREQ_CW_RX_8                                    RF_FPGA_FREQ_CW_RX_8
#define RF_FPGA_FREQ_CW_RX_8_POS                                (24U)
#define RF_FPGA_FREQ_CW_RX_8_LEN                                (7U)
#define RF_FPGA_FREQ_CW_RX_8_MSK                                (((1U<<RF_FPGA_FREQ_CW_RX_8_LEN)-1)<<RF_FPGA_FREQ_CW_RX_8_POS)
#define RF_FPGA_FREQ_CW_RX_8_UMSK                               (~(((1U<<RF_FPGA_FREQ_CW_RX_8_LEN)-1)<<RF_FPGA_FREQ_CW_RX_8_POS))

/* 0x720 : rf_fpga_table_63 */
#define RF_FPGA_TABLE_63_OFFSET                                 (0x720)
#define RF_FPGA_FREQ_CW_RX_15                                   RF_FPGA_FREQ_CW_RX_15
#define RF_FPGA_FREQ_CW_RX_15_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_15_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_15_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_15_LEN)-1)<<RF_FPGA_FREQ_CW_RX_15_POS)
#define RF_FPGA_FREQ_CW_RX_15_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_15_LEN)-1)<<RF_FPGA_FREQ_CW_RX_15_POS))
#define RF_FPGA_FREQ_CW_RX_14                                   RF_FPGA_FREQ_CW_RX_14
#define RF_FPGA_FREQ_CW_RX_14_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_14_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_14_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_14_LEN)-1)<<RF_FPGA_FREQ_CW_RX_14_POS)
#define RF_FPGA_FREQ_CW_RX_14_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_14_LEN)-1)<<RF_FPGA_FREQ_CW_RX_14_POS))
#define RF_FPGA_FREQ_CW_RX_13                                   RF_FPGA_FREQ_CW_RX_13
#define RF_FPGA_FREQ_CW_RX_13_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_13_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_13_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_13_LEN)-1)<<RF_FPGA_FREQ_CW_RX_13_POS)
#define RF_FPGA_FREQ_CW_RX_13_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_13_LEN)-1)<<RF_FPGA_FREQ_CW_RX_13_POS))
#define RF_FPGA_FREQ_CW_RX_12                                   RF_FPGA_FREQ_CW_RX_12
#define RF_FPGA_FREQ_CW_RX_12_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_12_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_12_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_12_LEN)-1)<<RF_FPGA_FREQ_CW_RX_12_POS)
#define RF_FPGA_FREQ_CW_RX_12_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_12_LEN)-1)<<RF_FPGA_FREQ_CW_RX_12_POS))

/* 0x724 : rf_fpga_table_64 */
#define RF_FPGA_TABLE_64_OFFSET                                 (0x724)
#define RF_FPGA_FREQ_CW_RX_19                                   RF_FPGA_FREQ_CW_RX_19
#define RF_FPGA_FREQ_CW_RX_19_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_19_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_19_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_19_LEN)-1)<<RF_FPGA_FREQ_CW_RX_19_POS)
#define RF_FPGA_FREQ_CW_RX_19_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_19_LEN)-1)<<RF_FPGA_FREQ_CW_RX_19_POS))
#define RF_FPGA_FREQ_CW_RX_18                                   RF_FPGA_FREQ_CW_RX_18
#define RF_FPGA_FREQ_CW_RX_18_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_18_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_18_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_18_LEN)-1)<<RF_FPGA_FREQ_CW_RX_18_POS)
#define RF_FPGA_FREQ_CW_RX_18_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_18_LEN)-1)<<RF_FPGA_FREQ_CW_RX_18_POS))
#define RF_FPGA_FREQ_CW_RX_17                                   RF_FPGA_FREQ_CW_RX_17
#define RF_FPGA_FREQ_CW_RX_17_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_17_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_17_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_17_LEN)-1)<<RF_FPGA_FREQ_CW_RX_17_POS)
#define RF_FPGA_FREQ_CW_RX_17_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_17_LEN)-1)<<RF_FPGA_FREQ_CW_RX_17_POS))
#define RF_FPGA_FREQ_CW_RX_16                                   RF_FPGA_FREQ_CW_RX_16
#define RF_FPGA_FREQ_CW_RX_16_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_16_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_16_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_16_LEN)-1)<<RF_FPGA_FREQ_CW_RX_16_POS)
#define RF_FPGA_FREQ_CW_RX_16_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_16_LEN)-1)<<RF_FPGA_FREQ_CW_RX_16_POS))

/* 0x728 : rf_fpga_table_65 */
#define RF_FPGA_TABLE_65_OFFSET                                 (0x728)
#define RF_FPGA_FREQ_CW_RX_23                                   RF_FPGA_FREQ_CW_RX_23
#define RF_FPGA_FREQ_CW_RX_23_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_23_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_23_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_23_LEN)-1)<<RF_FPGA_FREQ_CW_RX_23_POS)
#define RF_FPGA_FREQ_CW_RX_23_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_23_LEN)-1)<<RF_FPGA_FREQ_CW_RX_23_POS))
#define RF_FPGA_FREQ_CW_RX_22                                   RF_FPGA_FREQ_CW_RX_22
#define RF_FPGA_FREQ_CW_RX_22_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_22_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_22_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_22_LEN)-1)<<RF_FPGA_FREQ_CW_RX_22_POS)
#define RF_FPGA_FREQ_CW_RX_22_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_22_LEN)-1)<<RF_FPGA_FREQ_CW_RX_22_POS))
#define RF_FPGA_FREQ_CW_RX_21                                   RF_FPGA_FREQ_CW_RX_21
#define RF_FPGA_FREQ_CW_RX_21_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_21_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_21_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_21_LEN)-1)<<RF_FPGA_FREQ_CW_RX_21_POS)
#define RF_FPGA_FREQ_CW_RX_21_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_21_LEN)-1)<<RF_FPGA_FREQ_CW_RX_21_POS))
#define RF_FPGA_FREQ_CW_RX_20                                   RF_FPGA_FREQ_CW_RX_20
#define RF_FPGA_FREQ_CW_RX_20_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_20_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_20_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_20_LEN)-1)<<RF_FPGA_FREQ_CW_RX_20_POS)
#define RF_FPGA_FREQ_CW_RX_20_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_20_LEN)-1)<<RF_FPGA_FREQ_CW_RX_20_POS))

/* 0x72C : rf_fpga_table_66 */
#define RF_FPGA_TABLE_66_OFFSET                                 (0x72C)
#define RF_FPGA_FREQ_CW_RX_27                                   RF_FPGA_FREQ_CW_RX_27
#define RF_FPGA_FREQ_CW_RX_27_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_27_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_27_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_27_LEN)-1)<<RF_FPGA_FREQ_CW_RX_27_POS)
#define RF_FPGA_FREQ_CW_RX_27_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_27_LEN)-1)<<RF_FPGA_FREQ_CW_RX_27_POS))
#define RF_FPGA_FREQ_CW_RX_26                                   RF_FPGA_FREQ_CW_RX_26
#define RF_FPGA_FREQ_CW_RX_26_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_26_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_26_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_26_LEN)-1)<<RF_FPGA_FREQ_CW_RX_26_POS)
#define RF_FPGA_FREQ_CW_RX_26_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_26_LEN)-1)<<RF_FPGA_FREQ_CW_RX_26_POS))
#define RF_FPGA_FREQ_CW_RX_25                                   RF_FPGA_FREQ_CW_RX_25
#define RF_FPGA_FREQ_CW_RX_25_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_25_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_25_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_25_LEN)-1)<<RF_FPGA_FREQ_CW_RX_25_POS)
#define RF_FPGA_FREQ_CW_RX_25_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_25_LEN)-1)<<RF_FPGA_FREQ_CW_RX_25_POS))
#define RF_FPGA_FREQ_CW_RX_24                                   RF_FPGA_FREQ_CW_RX_24
#define RF_FPGA_FREQ_CW_RX_24_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_24_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_24_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_24_LEN)-1)<<RF_FPGA_FREQ_CW_RX_24_POS)
#define RF_FPGA_FREQ_CW_RX_24_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_24_LEN)-1)<<RF_FPGA_FREQ_CW_RX_24_POS))

/* 0x730 : rf_fpga_table_67 */
#define RF_FPGA_TABLE_67_OFFSET                                 (0x730)
#define RF_FPGA_FREQ_CW_RX_31                                   RF_FPGA_FREQ_CW_RX_31
#define RF_FPGA_FREQ_CW_RX_31_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_31_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_31_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_31_LEN)-1)<<RF_FPGA_FREQ_CW_RX_31_POS)
#define RF_FPGA_FREQ_CW_RX_31_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_31_LEN)-1)<<RF_FPGA_FREQ_CW_RX_31_POS))
#define RF_FPGA_FREQ_CW_RX_30                                   RF_FPGA_FREQ_CW_RX_30
#define RF_FPGA_FREQ_CW_RX_30_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_30_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_30_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_30_LEN)-1)<<RF_FPGA_FREQ_CW_RX_30_POS)
#define RF_FPGA_FREQ_CW_RX_30_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_30_LEN)-1)<<RF_FPGA_FREQ_CW_RX_30_POS))
#define RF_FPGA_FREQ_CW_RX_29                                   RF_FPGA_FREQ_CW_RX_29
#define RF_FPGA_FREQ_CW_RX_29_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_29_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_29_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_29_LEN)-1)<<RF_FPGA_FREQ_CW_RX_29_POS)
#define RF_FPGA_FREQ_CW_RX_29_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_29_LEN)-1)<<RF_FPGA_FREQ_CW_RX_29_POS))
#define RF_FPGA_FREQ_CW_RX_28                                   RF_FPGA_FREQ_CW_RX_28
#define RF_FPGA_FREQ_CW_RX_28_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_28_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_28_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_28_LEN)-1)<<RF_FPGA_FREQ_CW_RX_28_POS)
#define RF_FPGA_FREQ_CW_RX_28_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_28_LEN)-1)<<RF_FPGA_FREQ_CW_RX_28_POS))

/* 0x734 : rf_fpga_table_68 */
#define RF_FPGA_TABLE_68_OFFSET                                 (0x734)
#define RF_FPGA_FREQ_CW_RX_35                                   RF_FPGA_FREQ_CW_RX_35
#define RF_FPGA_FREQ_CW_RX_35_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_35_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_35_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_35_LEN)-1)<<RF_FPGA_FREQ_CW_RX_35_POS)
#define RF_FPGA_FREQ_CW_RX_35_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_35_LEN)-1)<<RF_FPGA_FREQ_CW_RX_35_POS))
#define RF_FPGA_FREQ_CW_RX_34                                   RF_FPGA_FREQ_CW_RX_34
#define RF_FPGA_FREQ_CW_RX_34_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_34_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_34_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_34_LEN)-1)<<RF_FPGA_FREQ_CW_RX_34_POS)
#define RF_FPGA_FREQ_CW_RX_34_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_34_LEN)-1)<<RF_FPGA_FREQ_CW_RX_34_POS))
#define RF_FPGA_FREQ_CW_RX_33                                   RF_FPGA_FREQ_CW_RX_33
#define RF_FPGA_FREQ_CW_RX_33_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_33_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_33_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_33_LEN)-1)<<RF_FPGA_FREQ_CW_RX_33_POS)
#define RF_FPGA_FREQ_CW_RX_33_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_33_LEN)-1)<<RF_FPGA_FREQ_CW_RX_33_POS))
#define RF_FPGA_FREQ_CW_RX_32                                   RF_FPGA_FREQ_CW_RX_32
#define RF_FPGA_FREQ_CW_RX_32_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_32_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_32_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_32_LEN)-1)<<RF_FPGA_FREQ_CW_RX_32_POS)
#define RF_FPGA_FREQ_CW_RX_32_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_32_LEN)-1)<<RF_FPGA_FREQ_CW_RX_32_POS))

/* 0x738 : rf_fpga_table_69 */
#define RF_FPGA_TABLE_69_OFFSET                                 (0x738)
#define RF_FPGA_FREQ_CW_RX_39                                   RF_FPGA_FREQ_CW_RX_39
#define RF_FPGA_FREQ_CW_RX_39_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_39_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_39_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_39_LEN)-1)<<RF_FPGA_FREQ_CW_RX_39_POS)
#define RF_FPGA_FREQ_CW_RX_39_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_39_LEN)-1)<<RF_FPGA_FREQ_CW_RX_39_POS))
#define RF_FPGA_FREQ_CW_RX_38                                   RF_FPGA_FREQ_CW_RX_38
#define RF_FPGA_FREQ_CW_RX_38_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_38_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_38_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_38_LEN)-1)<<RF_FPGA_FREQ_CW_RX_38_POS)
#define RF_FPGA_FREQ_CW_RX_38_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_38_LEN)-1)<<RF_FPGA_FREQ_CW_RX_38_POS))
#define RF_FPGA_FREQ_CW_RX_37                                   RF_FPGA_FREQ_CW_RX_37
#define RF_FPGA_FREQ_CW_RX_37_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_37_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_37_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_37_LEN)-1)<<RF_FPGA_FREQ_CW_RX_37_POS)
#define RF_FPGA_FREQ_CW_RX_37_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_37_LEN)-1)<<RF_FPGA_FREQ_CW_RX_37_POS))
#define RF_FPGA_FREQ_CW_RX_36                                   RF_FPGA_FREQ_CW_RX_36
#define RF_FPGA_FREQ_CW_RX_36_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_36_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_36_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_36_LEN)-1)<<RF_FPGA_FREQ_CW_RX_36_POS)
#define RF_FPGA_FREQ_CW_RX_36_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_36_LEN)-1)<<RF_FPGA_FREQ_CW_RX_36_POS))

/* 0x73C : rf_fpga_table_70 */
#define RF_FPGA_TABLE_70_OFFSET                                 (0x73C)
#define RF_FPGA_FREQ_CW_RX_43                                   RF_FPGA_FREQ_CW_RX_43
#define RF_FPGA_FREQ_CW_RX_43_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_43_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_43_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_43_LEN)-1)<<RF_FPGA_FREQ_CW_RX_43_POS)
#define RF_FPGA_FREQ_CW_RX_43_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_43_LEN)-1)<<RF_FPGA_FREQ_CW_RX_43_POS))
#define RF_FPGA_FREQ_CW_RX_42                                   RF_FPGA_FREQ_CW_RX_42
#define RF_FPGA_FREQ_CW_RX_42_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_42_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_42_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_42_LEN)-1)<<RF_FPGA_FREQ_CW_RX_42_POS)
#define RF_FPGA_FREQ_CW_RX_42_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_42_LEN)-1)<<RF_FPGA_FREQ_CW_RX_42_POS))
#define RF_FPGA_FREQ_CW_RX_41                                   RF_FPGA_FREQ_CW_RX_41
#define RF_FPGA_FREQ_CW_RX_41_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_41_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_41_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_41_LEN)-1)<<RF_FPGA_FREQ_CW_RX_41_POS)
#define RF_FPGA_FREQ_CW_RX_41_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_41_LEN)-1)<<RF_FPGA_FREQ_CW_RX_41_POS))
#define RF_FPGA_FREQ_CW_RX_40                                   RF_FPGA_FREQ_CW_RX_40
#define RF_FPGA_FREQ_CW_RX_40_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_40_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_40_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_40_LEN)-1)<<RF_FPGA_FREQ_CW_RX_40_POS)
#define RF_FPGA_FREQ_CW_RX_40_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_40_LEN)-1)<<RF_FPGA_FREQ_CW_RX_40_POS))

/* 0x740 : rf_fpga_table_71 */
#define RF_FPGA_TABLE_71_OFFSET                                 (0x740)
#define RF_FPGA_FREQ_CW_RX_47                                   RF_FPGA_FREQ_CW_RX_47
#define RF_FPGA_FREQ_CW_RX_47_POS                               (0U)
#define RF_FPGA_FREQ_CW_RX_47_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_47_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_47_LEN)-1)<<RF_FPGA_FREQ_CW_RX_47_POS)
#define RF_FPGA_FREQ_CW_RX_47_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_47_LEN)-1)<<RF_FPGA_FREQ_CW_RX_47_POS))
#define RF_FPGA_FREQ_CW_RX_46                                   RF_FPGA_FREQ_CW_RX_46
#define RF_FPGA_FREQ_CW_RX_46_POS                               (8U)
#define RF_FPGA_FREQ_CW_RX_46_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_46_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_46_LEN)-1)<<RF_FPGA_FREQ_CW_RX_46_POS)
#define RF_FPGA_FREQ_CW_RX_46_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_46_LEN)-1)<<RF_FPGA_FREQ_CW_RX_46_POS))
#define RF_FPGA_FREQ_CW_RX_45                                   RF_FPGA_FREQ_CW_RX_45
#define RF_FPGA_FREQ_CW_RX_45_POS                               (16U)
#define RF_FPGA_FREQ_CW_RX_45_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_45_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_45_LEN)-1)<<RF_FPGA_FREQ_CW_RX_45_POS)
#define RF_FPGA_FREQ_CW_RX_45_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_45_LEN)-1)<<RF_FPGA_FREQ_CW_RX_45_POS))
#define RF_FPGA_FREQ_CW_RX_44                                   RF_FPGA_FREQ_CW_RX_44
#define RF_FPGA_FREQ_CW_RX_44_POS                               (24U)
#define RF_FPGA_FREQ_CW_RX_44_LEN                               (7U)
#define RF_FPGA_FREQ_CW_RX_44_MSK                               (((1U<<RF_FPGA_FREQ_CW_RX_44_LEN)-1)<<RF_FPGA_FREQ_CW_RX_44_POS)
#define RF_FPGA_FREQ_CW_RX_44_UMSK                              (~(((1U<<RF_FPGA_FREQ_CW_RX_44_LEN)-1)<<RF_FPGA_FREQ_CW_RX_44_POS))


struct  rf_fpga_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[1536];

    /* 0x600 : rf_fpga_ctrl_0 */
    union {
        struct {
            uint32_t rf_fsm_st_cci_4s               :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t rf_fsm_st_cci_4s_en            :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t rf_fsm_en                      :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_22                 :  6; /* [22:17],       rsvd,        0x0 */
            uint32_t rf_fsm_state_cci               :  5; /* [27:23],          r,        0x0 */
            uint32_t rf_fsm_pu_rx_en                :  1; /* [   28],        r/w,        0x1 */
            uint32_t rf_fsm_pu_tx_en                :  1; /* [   29],        r/w,        0x1 */
            uint32_t rf_fsm_pu_lo_en                :  1; /* [   30],        r/w,        0x1 */
            uint32_t rf_fsm_pu_sb_en                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_0;

    /* 0x604 : rf_fpga_ctrl_1 */
    union {
        struct {
            uint32_t rf_fsm_cci_wr_time             : 12; /* [11: 0],        r/w,       0x4a */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rf_fsm_lo_time                 : 12; /* [27:16],        r/w,      0x640 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_1;

    /* 0x608 : rf_fpga_ctrl_2 */
    union {
        struct {
            uint32_t rf_fsm_dly_30_time             : 12; /* [11: 0],        r/w,      0x294 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rf_fsm_dly_01_time             : 12; /* [27:16],        r/w,       0x20 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_2;

    /* 0x60C : rf_fpga_ctrl_3 */
    union {
        struct {
            uint32_t rf_fsm_pu_sb                   : 32; /* [31: 0],        r/w, 0xd0000077L */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_3;

    /* 0x610 : rf_fpga_ctrl_4 */
    union {
        struct {
            uint32_t rf_fsm_pu_lo                   : 32; /* [31: 0],        r/w, 0xd3f00077L */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_4;

    /* 0x614 : rf_fpga_ctrl_5 */
    union {
        struct {
            uint32_t rf_fsm_pu_tx                   : 32; /* [31: 0],        r/w, 0xfbff0077L */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_5;

    /* 0x618 : rf_fpga_ctrl_6 */
    union {
        struct {
            uint32_t rf_fsm_pu_rx                   : 32; /* [31: 0],        r/w, 0xc7f07ff7L */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_6;

    /* 0x61C : rf_fpga_ctrl_7 */
    union {
        struct {
            uint32_t reserved_0_20                  : 21; /* [20: 0],       rsvd,        0x0 */
            uint32_t idac_cw_0                      :  5; /* [25:21],        r/w,        0xf */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_7;

    /* 0x620 : rf_fpga_table_0 */
    union {
        struct {
            uint32_t sdmin_0                        : 30; /* [29: 0],        r/w, 0x14d9c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_0;

    /* 0x624 : rf_fpga_table_1 */
    union {
        struct {
            uint32_t sdmin_1                        : 30; /* [29: 0],        r/w, 0x14de38e3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_1;

    /* 0x628 : rf_fpga_table_2 */
    union {
        struct {
            uint32_t sdmin_2                        : 30; /* [29: 0],        r/w, 0x14e2aaaa */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_2;

    /* 0x62C : rf_fpga_table_3 */
    union {
        struct {
            uint32_t sdmin_3                        : 30; /* [29: 0],        r/w, 0x14e71c71 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_3;

    /* 0x630 : rf_fpga_table_4 */
    union {
        struct {
            uint32_t sdmin_4                        : 30; /* [29: 0],        r/w, 0x14eb8e38 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_4;

    /* 0x634 : rf_fpga_table_5 */
    union {
        struct {
            uint32_t sdmin_5                        : 30; /* [29: 0],        r/w, 0x14f00000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_5;

    /* 0x638 : rf_fpga_table_6 */
    union {
        struct {
            uint32_t sdmin_6                        : 30; /* [29: 0],        r/w, 0x14f471c7 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_6;

    /* 0x63C : rf_fpga_table_7 */
    union {
        struct {
            uint32_t sdmin_7                        : 30; /* [29: 0],        r/w, 0x14f8e38e */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_7;

    /* 0x640 : rf_fpga_table_8 */
    union {
        struct {
            uint32_t sdmin_8                        : 30; /* [29: 0],        r/w, 0x14fd5555 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_8;

    /* 0x644 : rf_fpga_table_9 */
    union {
        struct {
            uint32_t sdmin_9                        : 30; /* [29: 0],        r/w, 0x1501c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_9;

    /* 0x648 : rf_fpga_table_10 */
    union {
        struct {
            uint32_t sdmin_10                       : 30; /* [29: 0],        r/w, 0x150638e3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_10;

    /* 0x64C : rf_fpga_table_11 */
    union {
        struct {
            uint32_t sdmin_11                       : 30; /* [29: 0],        r/w, 0x150aaaaa */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_11;

    /* 0x650 : rf_fpga_table_12 */
    union {
        struct {
            uint32_t sdmin_12                       : 30; /* [29: 0],        r/w, 0x150f1c71 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_12;

    /* 0x654 : rf_fpga_table_13 */
    union {
        struct {
            uint32_t sdmin_13                       : 30; /* [29: 0],        r/w, 0x15138e38 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_13;

    /* 0x658 : rf_fpga_table_14 */
    union {
        struct {
            uint32_t sdmin_14                       : 30; /* [29: 0],        r/w, 0x15180000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_14;

    /* 0x65C : rf_fpga_table_15 */
    union {
        struct {
            uint32_t sdmin_15                       : 30; /* [29: 0],        r/w, 0x151c71c7 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_15;

    /* 0x660 : rf_fpga_table_16 */
    union {
        struct {
            uint32_t sdmin_16                       : 30; /* [29: 0],        r/w, 0x1520e38e */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_16;

    /* 0x664 : rf_fpga_table_17 */
    union {
        struct {
            uint32_t sdmin_17                       : 30; /* [29: 0],        r/w, 0x15255555 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_17;

    /* 0x668 : rf_fpga_table_18 */
    union {
        struct {
            uint32_t sdmin_18                       : 30; /* [29: 0],        r/w, 0x1529c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_18;

    /* 0x66C : rf_fpga_table_19 */
    union {
        struct {
            uint32_t sdmin_19                       : 30; /* [29: 0],        r/w, 0x152e38e3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_19;

    /* 0x670 : rf_fpga_table_20 */
    union {
        struct {
            uint32_t sdmin_20                       : 30; /* [29: 0],        r/w, 0x1532aaaa */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_20;

    /* 0x674 : rf_fpga_table_21 */
    union {
        struct {
            uint32_t sdmin_21                       : 30; /* [29: 0],        r/w, 0x15371c71 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_21;

    /* 0x678 : rf_fpga_table_22 */
    union {
        struct {
            uint32_t sdmin_22                       : 30; /* [29: 0],        r/w, 0x153b8e38 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_22;

    /* 0x67C : rf_fpga_table_23 */
    union {
        struct {
            uint32_t sdmin_23                       : 30; /* [29: 0],        r/w, 0x15400000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_23;

    /* 0x680 : rf_fpga_table_24 */
    union {
        struct {
            uint32_t sdmin_24                       : 30; /* [29: 0],        r/w, 0x154471c7 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_24;

    /* 0x684 : rf_fpga_table_25 */
    union {
        struct {
            uint32_t sdmin_25                       : 30; /* [29: 0],        r/w, 0x1548e38e */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_25;

    /* 0x688 : rf_fpga_table_26 */
    union {
        struct {
            uint32_t sdmin_26                       : 30; /* [29: 0],        r/w, 0x154d5555 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_26;

    /* 0x68C : rf_fpga_table_27 */
    union {
        struct {
            uint32_t sdmin_27                       : 30; /* [29: 0],        r/w, 0x1551c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_27;

    /* 0x690 : rf_fpga_table_28 */
    union {
        struct {
            uint32_t sdmin_28                       : 30; /* [29: 0],        r/w, 0x155638e3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_28;

    /* 0x694 : rf_fpga_table_29 */
    union {
        struct {
            uint32_t sdmin_29                       : 30; /* [29: 0],        r/w, 0x155aaaaa */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_29;

    /* 0x698 : rf_fpga_table_30 */
    union {
        struct {
            uint32_t sdmin_30                       : 30; /* [29: 0],        r/w, 0x155f1c71 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_30;

    /* 0x69C : rf_fpga_table_31 */
    union {
        struct {
            uint32_t sdmin_31                       : 30; /* [29: 0],        r/w, 0x15638e38 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_31;

    /* 0x6A0 : rf_fpga_table_32 */
    union {
        struct {
            uint32_t sdmin_32                       : 30; /* [29: 0],        r/w, 0x15680000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_32;

    /* 0x6A4 : rf_fpga_table_33 */
    union {
        struct {
            uint32_t sdmin_33                       : 30; /* [29: 0],        r/w, 0x156c71c7 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_33;

    /* 0x6A8 : rf_fpga_table_34 */
    union {
        struct {
            uint32_t sdmin_34                       : 30; /* [29: 0],        r/w, 0x1570e38e */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_34;

    /* 0x6AC : rf_fpga_table_35 */
    union {
        struct {
            uint32_t sdmin_35                       : 30; /* [29: 0],        r/w, 0x15755555 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_35;

    /* 0x6B0 : rf_fpga_table_36 */
    union {
        struct {
            uint32_t sdmin_36                       : 30; /* [29: 0],        r/w, 0x1579c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_36;

    /* 0x6B4 : rf_fpga_table_37 */
    union {
        struct {
            uint32_t sdmin_37                       : 30; /* [29: 0],        r/w, 0x157e38e3 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_37;

    /* 0x6B8 : rf_fpga_table_38 */
    union {
        struct {
            uint32_t sdmin_38                       : 30; /* [29: 0],        r/w, 0x1582aaaa */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_38;

    /* 0x6BC : rf_fpga_table_39 */
    union {
        struct {
            uint32_t sdmin_39                       : 30; /* [29: 0],        r/w, 0x15871c71 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_39;

    /* 0x6C0 : rf_fpga_table_40 */
    union {
        struct {
            uint32_t sdmin_40                       : 30; /* [29: 0],        r/w, 0x14e071c7 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_40;

    /* 0x6C4 : rf_fpga_table_41 */
    union {
        struct {
            uint32_t sdmin_41                       : 30; /* [29: 0],        r/w, 0x14f6aaaa */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_41;

    /* 0x6C8 : rf_fpga_table_42 */
    union {
        struct {
            uint32_t sdmin_42                       : 30; /* [29: 0],        r/w, 0x150ce38e */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_42;

    /* 0x6CC : rf_fpga_table_43 */
    union {
        struct {
            uint32_t sdmin_43                       : 30; /* [29: 0],        r/w, 0x15231c71 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_43;

    /* 0x6D0 : rf_fpga_table_44 */
    union {
        struct {
            uint32_t sdmin_44                       : 30; /* [29: 0],        r/w, 0x15395555 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_44;

    /* 0x6D4 : rf_fpga_table_45 */
    union {
        struct {
            uint32_t sdmin_45                       : 30; /* [29: 0],        r/w, 0x154f8e38 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_45;

    /* 0x6D8 : rf_fpga_table_46 */
    union {
        struct {
            uint32_t sdmin_46                       : 30; /* [29: 0],        r/w, 0x1565c71c */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_46;

    /* 0x6DC : rf_fpga_table_47 */
    union {
        struct {
            uint32_t sdmin_47                       : 30; /* [29: 0],        r/w, 0x157c0000 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_47;

    /* 0x6E0 : rf_fpga_table_48 */
    union {
        struct {
            uint32_t freq_cw_3                      :  7; /* [ 6: 0],        r/w,       0x29 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_2                      :  7; /* [14: 8],        r/w,       0x29 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_1                      :  7; /* [22:16],        r/w,       0x29 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_0                      :  7; /* [30:24],        r/w,       0x2a */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_48;

    /* 0x6E4 : rf_fpga_table_49 */
    union {
        struct {
            uint32_t freq_cw_7                      :  7; /* [ 6: 0],        r/w,       0x28 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_6                      :  7; /* [14: 8],        r/w,       0x28 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_5                      :  7; /* [22:16],        r/w,       0x28 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_4                      :  7; /* [30:24],        r/w,       0x29 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_49;

    /* 0x6E8 : rf_fpga_table_50 */
    union {
        struct {
            uint32_t freq_cw_11                     :  7; /* [ 6: 0],        r/w,       0x27 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_10                     :  7; /* [14: 8],        r/w,       0x27 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_9                      :  7; /* [22:16],        r/w,       0x27 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_8                      :  7; /* [30:24],        r/w,       0x27 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_50;

    /* 0x6EC : rf_fpga_table_51 */
    union {
        struct {
            uint32_t freq_cw_15                     :  7; /* [ 6: 0],        r/w,       0x25 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_14                     :  7; /* [14: 8],        r/w,       0x26 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_13                     :  7; /* [22:16],        r/w,       0x26 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_12                     :  7; /* [30:24],        r/w,       0x26 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_51;

    /* 0x6F0 : rf_fpga_table_52 */
    union {
        struct {
            uint32_t freq_cw_19                     :  7; /* [ 6: 0],        r/w,       0x24 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_18                     :  7; /* [14: 8],        r/w,       0x25 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_17                     :  7; /* [22:16],        r/w,       0x25 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_16                     :  7; /* [30:24],        r/w,       0x25 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_52;

    /* 0x6F4 : rf_fpga_table_53 */
    union {
        struct {
            uint32_t freq_cw_23                     :  7; /* [ 6: 0],        r/w,       0x23 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_22                     :  7; /* [14: 8],        r/w,       0x24 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_21                     :  7; /* [22:16],        r/w,       0x24 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_20                     :  7; /* [30:24],        r/w,       0x24 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_53;

    /* 0x6F8 : rf_fpga_table_54 */
    union {
        struct {
            uint32_t freq_cw_27                     :  7; /* [ 6: 0],        r/w,       0x22 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_26                     :  7; /* [14: 8],        r/w,       0x22 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_25                     :  7; /* [22:16],        r/w,       0x23 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_24                     :  7; /* [30:24],        r/w,       0x23 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_54;

    /* 0x6FC : rf_fpga_table_55 */
    union {
        struct {
            uint32_t freq_cw_31                     :  7; /* [ 6: 0],        r/w,       0x21 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_30                     :  7; /* [14: 8],        r/w,       0x21 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_29                     :  7; /* [22:16],        r/w,       0x22 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_28                     :  7; /* [30:24],        r/w,       0x22 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_55;

    /* 0x700 : rf_fpga_table_56 */
    union {
        struct {
            uint32_t freq_cw_35                     :  7; /* [ 6: 0],        r/w,       0x20 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_34                     :  7; /* [14: 8],        r/w,       0x20 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_33                     :  7; /* [22:16],        r/w,       0x21 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_32                     :  7; /* [30:24],        r/w,       0x21 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_56;

    /* 0x704 : rf_fpga_table_57 */
    union {
        struct {
            uint32_t freq_cw_39                     :  7; /* [ 6: 0],        r/w,       0x1f */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_38                     :  7; /* [14: 8],        r/w,       0x1f */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_37                     :  7; /* [22:16],        r/w,       0x20 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_36                     :  7; /* [30:24],        r/w,       0x20 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_57;

    /* 0x708 : rf_fpga_table_58 */
    union {
        struct {
            uint32_t freq_cw_43                     :  7; /* [ 6: 0],        r/w,       0x25 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_42                     :  7; /* [14: 8],        r/w,       0x26 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_41                     :  7; /* [22:16],        r/w,       0x28 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_40                     :  7; /* [30:24],        r/w,       0x29 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_58;

    /* 0x70C : rf_fpga_table_59 */
    union {
        struct {
            uint32_t freq_cw_47                     :  7; /* [ 6: 0],        r/w,       0x20 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_46                     :  7; /* [14: 8],        r/w,       0x21 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_45                     :  7; /* [22:16],        r/w,       0x22 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_44                     :  7; /* [30:24],        r/w,       0x24 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_59;

    /* 0x710 : rf_fpga_ctrl_8 */
    union {
        struct {
            uint32_t cmd_enable                     : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_ctrl_8;

    /* 0x714 : rf_fpga_table_60 */
    union {
        struct {
            uint32_t freq_cw_rx_3                   :  7; /* [ 6: 0],        r/w,       0x29 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_2                   :  7; /* [14: 8],        r/w,       0x29 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_1                   :  7; /* [22:16],        r/w,       0x29 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_0                   :  7; /* [30:24],        r/w,       0x2a */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_60;

    /* 0x718 : rf_fpga_table_61 */
    union {
        struct {
            uint32_t freq_cw_rx_7                   :  7; /* [ 6: 0],        r/w,       0x28 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_6                   :  7; /* [14: 8],        r/w,       0x28 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_5                   :  7; /* [22:16],        r/w,       0x28 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_4                   :  7; /* [30:24],        r/w,       0x29 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_61;

    /* 0x71C : rf_fpga_table_62 */
    union {
        struct {
            uint32_t freq_cw_rx_11                  :  7; /* [ 6: 0],        r/w,       0x27 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_10                  :  7; /* [14: 8],        r/w,       0x27 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_9                   :  7; /* [22:16],        r/w,       0x27 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_8                   :  7; /* [30:24],        r/w,       0x27 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_62;

    /* 0x720 : rf_fpga_table_63 */
    union {
        struct {
            uint32_t freq_cw_rx_15                  :  7; /* [ 6: 0],        r/w,       0x25 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_14                  :  7; /* [14: 8],        r/w,       0x26 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_13                  :  7; /* [22:16],        r/w,       0x26 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_12                  :  7; /* [30:24],        r/w,       0x26 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_63;

    /* 0x724 : rf_fpga_table_64 */
    union {
        struct {
            uint32_t freq_cw_rx_19                  :  7; /* [ 6: 0],        r/w,       0x24 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_18                  :  7; /* [14: 8],        r/w,       0x25 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_17                  :  7; /* [22:16],        r/w,       0x25 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_16                  :  7; /* [30:24],        r/w,       0x25 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_64;

    /* 0x728 : rf_fpga_table_65 */
    union {
        struct {
            uint32_t freq_cw_rx_23                  :  7; /* [ 6: 0],        r/w,       0x23 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_22                  :  7; /* [14: 8],        r/w,       0x24 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_21                  :  7; /* [22:16],        r/w,       0x24 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_20                  :  7; /* [30:24],        r/w,       0x24 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_65;

    /* 0x72C : rf_fpga_table_66 */
    union {
        struct {
            uint32_t freq_cw_rx_27                  :  7; /* [ 6: 0],        r/w,       0x22 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_26                  :  7; /* [14: 8],        r/w,       0x22 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_25                  :  7; /* [22:16],        r/w,       0x23 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_24                  :  7; /* [30:24],        r/w,       0x23 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_66;

    /* 0x730 : rf_fpga_table_67 */
    union {
        struct {
            uint32_t freq_cw_rx_31                  :  7; /* [ 6: 0],        r/w,       0x21 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_30                  :  7; /* [14: 8],        r/w,       0x21 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_29                  :  7; /* [22:16],        r/w,       0x22 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_28                  :  7; /* [30:24],        r/w,       0x22 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_67;

    /* 0x734 : rf_fpga_table_68 */
    union {
        struct {
            uint32_t freq_cw_rx_35                  :  7; /* [ 6: 0],        r/w,       0x20 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_34                  :  7; /* [14: 8],        r/w,       0x20 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_33                  :  7; /* [22:16],        r/w,       0x21 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_32                  :  7; /* [30:24],        r/w,       0x21 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_68;

    /* 0x738 : rf_fpga_table_69 */
    union {
        struct {
            uint32_t freq_cw_rx_39                  :  7; /* [ 6: 0],        r/w,       0x1f */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_38                  :  7; /* [14: 8],        r/w,       0x1f */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_37                  :  7; /* [22:16],        r/w,       0x20 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_36                  :  7; /* [30:24],        r/w,       0x20 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_69;

    /* 0x73C : rf_fpga_table_70 */
    union {
        struct {
            uint32_t freq_cw_rx_43                  :  7; /* [ 6: 0],        r/w,       0x25 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_42                  :  7; /* [14: 8],        r/w,       0x26 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_41                  :  7; /* [22:16],        r/w,       0x28 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_40                  :  7; /* [30:24],        r/w,       0x29 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_70;

    /* 0x740 : rf_fpga_table_71 */
    union {
        struct {
            uint32_t freq_cw_rx_47                  :  7; /* [ 6: 0],        r/w,       0x20 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t freq_cw_rx_46                  :  7; /* [14: 8],        r/w,       0x21 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t freq_cw_rx_45                  :  7; /* [22:16],        r/w,       0x22 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t freq_cw_rx_44                  :  7; /* [30:24],        r/w,       0x24 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_fpga_table_71;

};

typedef volatile struct rf_fpga_reg rf_fpga_reg_t;


#endif  /* __RF_FPGA_REG_H__ */
