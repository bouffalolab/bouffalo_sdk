/**
  ******************************************************************************
  * @file    cci_reg.h
  * @version V1.0
  * @date    2025-11-26
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
#ifndef  __CCI_REG_H__
#define  __CCI_REG_H__

#include "bl616d.h"

/* 0x0 : cci_cfg */
#define CCI_CFG_OFFSET                                          (0x0)
#define CCI_EN                                                  CCI_EN
#define CCI_EN_POS                                              (0U)
#define CCI_EN_LEN                                              (1U)
#define CCI_EN_MSK                                              (((1U<<CCI_EN_LEN)-1)<<CCI_EN_POS)
#define CCI_EN_UMSK                                             (~(((1U<<CCI_EN_LEN)-1)<<CCI_EN_POS))
#define CCI_SLV_SEL_CCI2                                        CCI_SLV_SEL_CCI2
#define CCI_SLV_SEL_CCI2_POS                                    (1U)
#define CCI_SLV_SEL_CCI2_LEN                                    (1U)
#define CCI_SLV_SEL_CCI2_MSK                                    (((1U<<CCI_SLV_SEL_CCI2_LEN)-1)<<CCI_SLV_SEL_CCI2_POS)
#define CCI_SLV_SEL_CCI2_UMSK                                   (~(((1U<<CCI_SLV_SEL_CCI2_LEN)-1)<<CCI_SLV_SEL_CCI2_POS))
#define CCI_MAS_SEL_CCI2                                        CCI_MAS_SEL_CCI2
#define CCI_MAS_SEL_CCI2_POS                                    (2U)
#define CCI_MAS_SEL_CCI2_LEN                                    (1U)
#define CCI_MAS_SEL_CCI2_MSK                                    (((1U<<CCI_MAS_SEL_CCI2_LEN)-1)<<CCI_MAS_SEL_CCI2_POS)
#define CCI_MAS_SEL_CCI2_UMSK                                   (~(((1U<<CCI_MAS_SEL_CCI2_LEN)-1)<<CCI_MAS_SEL_CCI2_POS))
#define CCI_MAS_HW_MODE                                         CCI_MAS_HW_MODE
#define CCI_MAS_HW_MODE_POS                                     (3U)
#define CCI_MAS_HW_MODE_LEN                                     (1U)
#define CCI_MAS_HW_MODE_MSK                                     (((1U<<CCI_MAS_HW_MODE_LEN)-1)<<CCI_MAS_HW_MODE_POS)
#define CCI_MAS_HW_MODE_UMSK                                    (~(((1U<<CCI_MAS_HW_MODE_LEN)-1)<<CCI_MAS_HW_MODE_POS))
#define CCI_REG_M_CCI_SCLK_EN                                   CCI_REG_M_CCI_SCLK_EN
#define CCI_REG_M_CCI_SCLK_EN_POS                               (4U)
#define CCI_REG_M_CCI_SCLK_EN_LEN                               (1U)
#define CCI_REG_M_CCI_SCLK_EN_MSK                               (((1U<<CCI_REG_M_CCI_SCLK_EN_LEN)-1)<<CCI_REG_M_CCI_SCLK_EN_POS)
#define CCI_REG_M_CCI_SCLK_EN_UMSK                              (~(((1U<<CCI_REG_M_CCI_SCLK_EN_LEN)-1)<<CCI_REG_M_CCI_SCLK_EN_POS))
#define CCI_REG_DIV_M_CCI_SCLK                                  CCI_REG_DIV_M_CCI_SCLK
#define CCI_REG_DIV_M_CCI_SCLK_POS                              (5U)
#define CCI_REG_DIV_M_CCI_SCLK_LEN                              (2U)
#define CCI_REG_DIV_M_CCI_SCLK_MSK                              (((1U<<CCI_REG_DIV_M_CCI_SCLK_LEN)-1)<<CCI_REG_DIV_M_CCI_SCLK_POS)
#define CCI_REG_DIV_M_CCI_SCLK_UMSK                             (~(((1U<<CCI_REG_DIV_M_CCI_SCLK_LEN)-1)<<CCI_REG_DIV_M_CCI_SCLK_POS))
#define CCI_CFG_CCI1_PRE_READ                                   CCI_CFG_CCI1_PRE_READ
#define CCI_CFG_CCI1_PRE_READ_POS                               (7U)
#define CCI_CFG_CCI1_PRE_READ_LEN                               (1U)
#define CCI_CFG_CCI1_PRE_READ_MSK                               (((1U<<CCI_CFG_CCI1_PRE_READ_LEN)-1)<<CCI_CFG_CCI1_PRE_READ_POS)
#define CCI_CFG_CCI1_PRE_READ_UMSK                              (~(((1U<<CCI_CFG_CCI1_PRE_READ_LEN)-1)<<CCI_CFG_CCI1_PRE_READ_POS))
#define CCI_REG_SCCI_CLK_INV                                    CCI_REG_SCCI_CLK_INV
#define CCI_REG_SCCI_CLK_INV_POS                                (8U)
#define CCI_REG_SCCI_CLK_INV_LEN                                (1U)
#define CCI_REG_SCCI_CLK_INV_MSK                                (((1U<<CCI_REG_SCCI_CLK_INV_LEN)-1)<<CCI_REG_SCCI_CLK_INV_POS)
#define CCI_REG_SCCI_CLK_INV_UMSK                               (~(((1U<<CCI_REG_SCCI_CLK_INV_LEN)-1)<<CCI_REG_SCCI_CLK_INV_POS))
#define CCI_REG_MCCI_CLK_INV                                    CCI_REG_MCCI_CLK_INV
#define CCI_REG_MCCI_CLK_INV_POS                                (9U)
#define CCI_REG_MCCI_CLK_INV_LEN                                (1U)
#define CCI_REG_MCCI_CLK_INV_MSK                                (((1U<<CCI_REG_MCCI_CLK_INV_LEN)-1)<<CCI_REG_MCCI_CLK_INV_POS)
#define CCI_REG_MCCI_CLK_INV_UMSK                               (~(((1U<<CCI_REG_MCCI_CLK_INV_LEN)-1)<<CCI_REG_MCCI_CLK_INV_POS))
#define CCI_CFG_MCCI_DLY_R                                      CCI_CFG_MCCI_DLY_R
#define CCI_CFG_MCCI_DLY_R_POS                                  (16U)
#define CCI_CFG_MCCI_DLY_R_LEN                                  (1U)
#define CCI_CFG_MCCI_DLY_R_MSK                                  (((1U<<CCI_CFG_MCCI_DLY_R_LEN)-1)<<CCI_CFG_MCCI_DLY_R_POS)
#define CCI_CFG_MCCI_DLY_R_UMSK                                 (~(((1U<<CCI_CFG_MCCI_DLY_R_LEN)-1)<<CCI_CFG_MCCI_DLY_R_POS))

/* 0x4 : cci_addr */
#define CCI_ADDR_OFFSET                                         (0x4)
#define CCI_APB_CCI_ADDR                                        CCI_APB_CCI_ADDR
#define CCI_APB_CCI_ADDR_POS                                    (0U)
#define CCI_APB_CCI_ADDR_LEN                                    (32U)
#define CCI_APB_CCI_ADDR_MSK                                    (((1U<<CCI_APB_CCI_ADDR_LEN)-1)<<CCI_APB_CCI_ADDR_POS)
#define CCI_APB_CCI_ADDR_UMSK                                   (~(((1U<<CCI_APB_CCI_ADDR_LEN)-1)<<CCI_APB_CCI_ADDR_POS))

/* 0x8 : cci_wdata */
#define CCI_WDATA_OFFSET                                        (0x8)
#define CCI_APB_CCI_WDATA                                       CCI_APB_CCI_WDATA
#define CCI_APB_CCI_WDATA_POS                                   (0U)
#define CCI_APB_CCI_WDATA_LEN                                   (32U)
#define CCI_APB_CCI_WDATA_MSK                                   (((1U<<CCI_APB_CCI_WDATA_LEN)-1)<<CCI_APB_CCI_WDATA_POS)
#define CCI_APB_CCI_WDATA_UMSK                                  (~(((1U<<CCI_APB_CCI_WDATA_LEN)-1)<<CCI_APB_CCI_WDATA_POS))

/* 0xC : cci_rdata */
#define CCI_RDATA_OFFSET                                        (0xC)
#define CCI_APB_CCI_RDATA                                       CCI_APB_CCI_RDATA
#define CCI_APB_CCI_RDATA_POS                                   (0U)
#define CCI_APB_CCI_RDATA_LEN                                   (32U)
#define CCI_APB_CCI_RDATA_MSK                                   (((1U<<CCI_APB_CCI_RDATA_LEN)-1)<<CCI_APB_CCI_RDATA_POS)
#define CCI_APB_CCI_RDATA_UMSK                                  (~(((1U<<CCI_APB_CCI_RDATA_LEN)-1)<<CCI_APB_CCI_RDATA_POS))

/* 0x10 : cci_ctl */
#define CCI_CTL_OFFSET                                          (0x10)
#define CCI_WRITE_FLAG                                          CCI_WRITE_FLAG
#define CCI_WRITE_FLAG_POS                                      (0U)
#define CCI_WRITE_FLAG_LEN                                      (1U)
#define CCI_WRITE_FLAG_MSK                                      (((1U<<CCI_WRITE_FLAG_LEN)-1)<<CCI_WRITE_FLAG_POS)
#define CCI_WRITE_FLAG_UMSK                                     (~(((1U<<CCI_WRITE_FLAG_LEN)-1)<<CCI_WRITE_FLAG_POS))
#define CCI_READ_FLAG                                           CCI_READ_FLAG
#define CCI_READ_FLAG_POS                                       (1U)
#define CCI_READ_FLAG_LEN                                       (1U)
#define CCI_READ_FLAG_MSK                                       (((1U<<CCI_READ_FLAG_LEN)-1)<<CCI_READ_FLAG_POS)
#define CCI_READ_FLAG_UMSK                                      (~(((1U<<CCI_READ_FLAG_LEN)-1)<<CCI_READ_FLAG_POS))
#define CCI_AHB_STATE                                           CCI_AHB_STATE
#define CCI_AHB_STATE_POS                                       (2U)
#define CCI_AHB_STATE_LEN                                       (2U)
#define CCI_AHB_STATE_MSK                                       (((1U<<CCI_AHB_STATE_LEN)-1)<<CCI_AHB_STATE_POS)
#define CCI_AHB_STATE_UMSK                                      (~(((1U<<CCI_AHB_STATE_LEN)-1)<<CCI_AHB_STATE_POS))

/* 0xD0 : wifipll_ana_ctrl */
#define CCI_WIFIPLL_ANA_CTRL_OFFSET                             (0xD0)
#define CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK                        CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK
#define CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_POS                    (0U)
#define CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_LEN                    (2U)
#define CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_MSK                    (((1U<<CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_LEN)-1)<<CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_POS)
#define CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_UMSK                   (~(((1U<<CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_LEN)-1)<<CCI_WIFIPLL_FBDIV_SEL_SAMPLE_CLK_POS))
#define CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK                         CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK
#define CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_POS                     (2U)
#define CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_LEN                     (1U)
#define CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_MSK                     (((1U<<CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_POS)
#define CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_UMSK                    (~(((1U<<CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<CCI_WIFIPLL_FBDIV_SEL_ADPLL_CLK_POS))
#define CCI_WIFIPLL_DTC_R_SEL                                   CCI_WIFIPLL_DTC_R_SEL
#define CCI_WIFIPLL_DTC_R_SEL_POS                               (4U)
#define CCI_WIFIPLL_DTC_R_SEL_LEN                               (2U)
#define CCI_WIFIPLL_DTC_R_SEL_MSK                               (((1U<<CCI_WIFIPLL_DTC_R_SEL_LEN)-1)<<CCI_WIFIPLL_DTC_R_SEL_POS)
#define CCI_WIFIPLL_DTC_R_SEL_UMSK                              (~(((1U<<CCI_WIFIPLL_DTC_R_SEL_LEN)-1)<<CCI_WIFIPLL_DTC_R_SEL_POS))
#define CCI_WIFIPLL_DTC_BYPASS_EN                               CCI_WIFIPLL_DTC_BYPASS_EN
#define CCI_WIFIPLL_DTC_BYPASS_EN_POS                           (6U)
#define CCI_WIFIPLL_DTC_BYPASS_EN_LEN                           (1U)
#define CCI_WIFIPLL_DTC_BYPASS_EN_MSK                           (((1U<<CCI_WIFIPLL_DTC_BYPASS_EN_LEN)-1)<<CCI_WIFIPLL_DTC_BYPASS_EN_POS)
#define CCI_WIFIPLL_DTC_BYPASS_EN_UMSK                          (~(((1U<<CCI_WIFIPLL_DTC_BYPASS_EN_LEN)-1)<<CCI_WIFIPLL_DTC_BYPASS_EN_POS))
#define CCI_WIFIPLL_VCO_SPEED                                   CCI_WIFIPLL_VCO_SPEED
#define CCI_WIFIPLL_VCO_SPEED_POS                               (8U)
#define CCI_WIFIPLL_VCO_SPEED_LEN                               (3U)
#define CCI_WIFIPLL_VCO_SPEED_MSK                               (((1U<<CCI_WIFIPLL_VCO_SPEED_LEN)-1)<<CCI_WIFIPLL_VCO_SPEED_POS)
#define CCI_WIFIPLL_VCO_SPEED_UMSK                              (~(((1U<<CCI_WIFIPLL_VCO_SPEED_LEN)-1)<<CCI_WIFIPLL_VCO_SPEED_POS))
#define CCI_WIFIPLL_VCO_480M_EN                                 CCI_WIFIPLL_VCO_480M_EN
#define CCI_WIFIPLL_VCO_480M_EN_POS                             (11U)
#define CCI_WIFIPLL_VCO_480M_EN_LEN                             (1U)
#define CCI_WIFIPLL_VCO_480M_EN_MSK                             (((1U<<CCI_WIFIPLL_VCO_480M_EN_LEN)-1)<<CCI_WIFIPLL_VCO_480M_EN_POS)
#define CCI_WIFIPLL_VCO_480M_EN_UMSK                            (~(((1U<<CCI_WIFIPLL_VCO_480M_EN_LEN)-1)<<CCI_WIFIPLL_VCO_480M_EN_POS))
#define CCI_WIFIPLL_VCO_LDO_BYPASS                              CCI_WIFIPLL_VCO_LDO_BYPASS
#define CCI_WIFIPLL_VCO_LDO_BYPASS_POS                          (12U)
#define CCI_WIFIPLL_VCO_LDO_BYPASS_LEN                          (1U)
#define CCI_WIFIPLL_VCO_LDO_BYPASS_MSK                          (((1U<<CCI_WIFIPLL_VCO_LDO_BYPASS_LEN)-1)<<CCI_WIFIPLL_VCO_LDO_BYPASS_POS)
#define CCI_WIFIPLL_VCO_LDO_BYPASS_UMSK                         (~(((1U<<CCI_WIFIPLL_VCO_LDO_BYPASS_LEN)-1)<<CCI_WIFIPLL_VCO_LDO_BYPASS_POS))
#define CCI_WIFIPLL_VCO_RSHT_SEL                                CCI_WIFIPLL_VCO_RSHT_SEL
#define CCI_WIFIPLL_VCO_RSHT_SEL_POS                            (13U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_LEN                            (2U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_MSK                            (((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_POS)
#define CCI_WIFIPLL_VCO_RSHT_SEL_UMSK                           (~(((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_POS))
#define CCI_WIFIPLL_DCOMASH_BYPASS                              CCI_WIFIPLL_DCOMASH_BYPASS
#define CCI_WIFIPLL_DCOMASH_BYPASS_POS                          (15U)
#define CCI_WIFIPLL_DCOMASH_BYPASS_LEN                          (1U)
#define CCI_WIFIPLL_DCOMASH_BYPASS_MSK                          (((1U<<CCI_WIFIPLL_DCOMASH_BYPASS_LEN)-1)<<CCI_WIFIPLL_DCOMASH_BYPASS_POS)
#define CCI_WIFIPLL_DCOMASH_BYPASS_UMSK                         (~(((1U<<CCI_WIFIPLL_DCOMASH_BYPASS_LEN)-1)<<CCI_WIFIPLL_DCOMASH_BYPASS_POS))
#define CCI_WIFIPLL_REFCLK_SEL                                  CCI_WIFIPLL_REFCLK_SEL
#define CCI_WIFIPLL_REFCLK_SEL_POS                              (16U)
#define CCI_WIFIPLL_REFCLK_SEL_LEN                              (2U)
#define CCI_WIFIPLL_REFCLK_SEL_MSK                              (((1U<<CCI_WIFIPLL_REFCLK_SEL_LEN)-1)<<CCI_WIFIPLL_REFCLK_SEL_POS)
#define CCI_WIFIPLL_REFCLK_SEL_UMSK                             (~(((1U<<CCI_WIFIPLL_REFCLK_SEL_LEN)-1)<<CCI_WIFIPLL_REFCLK_SEL_POS))
#define CCI_WIFIPLL_VCO_RSHT_SEL_HP                             CCI_WIFIPLL_VCO_RSHT_SEL_HP
#define CCI_WIFIPLL_VCO_RSHT_SEL_HP_POS                         (18U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_HP_LEN                         (2U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_HP_MSK                         (((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_HP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_HP_POS)
#define CCI_WIFIPLL_VCO_RSHT_SEL_HP_UMSK                        (~(((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_HP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_HP_POS))
#define CCI_WIFIPLL_REFCLK_DIV_RATIO                            CCI_WIFIPLL_REFCLK_DIV_RATIO
#define CCI_WIFIPLL_REFCLK_DIV_RATIO_POS                        (20U)
#define CCI_WIFIPLL_REFCLK_DIV_RATIO_LEN                        (4U)
#define CCI_WIFIPLL_REFCLK_DIV_RATIO_MSK                        (((1U<<CCI_WIFIPLL_REFCLK_DIV_RATIO_LEN)-1)<<CCI_WIFIPLL_REFCLK_DIV_RATIO_POS)
#define CCI_WIFIPLL_REFCLK_DIV_RATIO_UMSK                       (~(((1U<<CCI_WIFIPLL_REFCLK_DIV_RATIO_LEN)-1)<<CCI_WIFIPLL_REFCLK_DIV_RATIO_POS))
#define CCI_WIFIPLL_RSTB                                        CCI_WIFIPLL_RSTB
#define CCI_WIFIPLL_RSTB_POS                                    (24U)
#define CCI_WIFIPLL_RSTB_LEN                                    (1U)
#define CCI_WIFIPLL_RSTB_MSK                                    (((1U<<CCI_WIFIPLL_RSTB_LEN)-1)<<CCI_WIFIPLL_RSTB_POS)
#define CCI_WIFIPLL_RSTB_UMSK                                   (~(((1U<<CCI_WIFIPLL_RSTB_LEN)-1)<<CCI_WIFIPLL_RSTB_POS))
#define CCI_PU_WIFIPLL                                          CCI_PU_WIFIPLL
#define CCI_PU_WIFIPLL_POS                                      (28U)
#define CCI_PU_WIFIPLL_LEN                                      (1U)
#define CCI_PU_WIFIPLL_MSK                                      (((1U<<CCI_PU_WIFIPLL_LEN)-1)<<CCI_PU_WIFIPLL_POS)
#define CCI_PU_WIFIPLL_UMSK                                     (~(((1U<<CCI_PU_WIFIPLL_LEN)-1)<<CCI_PU_WIFIPLL_POS))
#define CCI_WIFIPLL_VCO_RSHT_SEL_LP                             CCI_WIFIPLL_VCO_RSHT_SEL_LP
#define CCI_WIFIPLL_VCO_RSHT_SEL_LP_POS                         (30U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_LP_LEN                         (2U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_LP_MSK                         (((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_LP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_LP_POS)
#define CCI_WIFIPLL_VCO_RSHT_SEL_LP_UMSK                        (~(((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_LP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_LP_POS))

/* 0xD4 : wifipll_hw_ctrl */
#define CCI_WIFIPLL_HW_CTRL_OFFSET                              (0xD4)
#define CCI_WIFIPLL_PI_BYPASS_TX                                CCI_WIFIPLL_PI_BYPASS_TX
#define CCI_WIFIPLL_PI_BYPASS_TX_POS                            (0U)
#define CCI_WIFIPLL_PI_BYPASS_TX_LEN                            (1U)
#define CCI_WIFIPLL_PI_BYPASS_TX_MSK                            (((1U<<CCI_WIFIPLL_PI_BYPASS_TX_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_TX_POS)
#define CCI_WIFIPLL_PI_BYPASS_TX_UMSK                           (~(((1U<<CCI_WIFIPLL_PI_BYPASS_TX_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_TX_POS))
#define CCI_WIFIPLL_PI_BYPASS_RX                                CCI_WIFIPLL_PI_BYPASS_RX
#define CCI_WIFIPLL_PI_BYPASS_RX_POS                            (2U)
#define CCI_WIFIPLL_PI_BYPASS_RX_LEN                            (1U)
#define CCI_WIFIPLL_PI_BYPASS_RX_MSK                            (((1U<<CCI_WIFIPLL_PI_BYPASS_RX_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_RX_POS)
#define CCI_WIFIPLL_PI_BYPASS_RX_UMSK                           (~(((1U<<CCI_WIFIPLL_PI_BYPASS_RX_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_RX_POS))
#define CCI_WIFIPLL_PI_BYPASS_TX_BZ                             CCI_WIFIPLL_PI_BYPASS_TX_BZ
#define CCI_WIFIPLL_PI_BYPASS_TX_BZ_POS                         (4U)
#define CCI_WIFIPLL_PI_BYPASS_TX_BZ_LEN                         (1U)
#define CCI_WIFIPLL_PI_BYPASS_TX_BZ_MSK                         (((1U<<CCI_WIFIPLL_PI_BYPASS_TX_BZ_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_TX_BZ_POS)
#define CCI_WIFIPLL_PI_BYPASS_TX_BZ_UMSK                        (~(((1U<<CCI_WIFIPLL_PI_BYPASS_TX_BZ_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_TX_BZ_POS))
#define CCI_WIFIPLL_PI_BYPASS_RX_BZ                             CCI_WIFIPLL_PI_BYPASS_RX_BZ
#define CCI_WIFIPLL_PI_BYPASS_RX_BZ_POS                         (6U)
#define CCI_WIFIPLL_PI_BYPASS_RX_BZ_LEN                         (1U)
#define CCI_WIFIPLL_PI_BYPASS_RX_BZ_MSK                         (((1U<<CCI_WIFIPLL_PI_BYPASS_RX_BZ_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_RX_BZ_POS)
#define CCI_WIFIPLL_PI_BYPASS_RX_BZ_UMSK                        (~(((1U<<CCI_WIFIPLL_PI_BYPASS_RX_BZ_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_RX_BZ_POS))
#define CCI_WIFIPLL_PI_BYPASS_SEL_WIFI                          CCI_WIFIPLL_PI_BYPASS_SEL_WIFI
#define CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_POS                      (8U)
#define CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_LEN                      (1U)
#define CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_MSK                      (((1U<<CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_POS)
#define CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_UMSK                     (~(((1U<<CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_SEL_WIFI_POS))
#define CCI_WIFIPLL_PI_BYPASS_HW                                CCI_WIFIPLL_PI_BYPASS_HW
#define CCI_WIFIPLL_PI_BYPASS_HW_POS                            (9U)
#define CCI_WIFIPLL_PI_BYPASS_HW_LEN                            (1U)
#define CCI_WIFIPLL_PI_BYPASS_HW_MSK                            (((1U<<CCI_WIFIPLL_PI_BYPASS_HW_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_HW_POS)
#define CCI_WIFIPLL_PI_BYPASS_HW_UMSK                           (~(((1U<<CCI_WIFIPLL_PI_BYPASS_HW_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_HW_POS))
#define CCI_WIFIPLL_VCO_RSHT_SEL_HW                             CCI_WIFIPLL_VCO_RSHT_SEL_HW
#define CCI_WIFIPLL_VCO_RSHT_SEL_HW_POS                         (10U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_HW_LEN                         (2U)
#define CCI_WIFIPLL_VCO_RSHT_SEL_HW_MSK                         (((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_HW_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_HW_POS)
#define CCI_WIFIPLL_VCO_RSHT_SEL_HW_UMSK                        (~(((1U<<CCI_WIFIPLL_VCO_RSHT_SEL_HW_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_SEL_HW_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_RX                              CCI_WIFIPLL_VCO_RSHT_EN_RX
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_POS                          (13U)
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_LEN                          (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_MSK                          (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_RX_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_RX_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_UMSK                         (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_RX_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_RX_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ                           CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_POS                       (16U)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_LEN                       (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_MSK                       (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_UMSK                      (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ                           CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_POS                       (17U)
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_LEN                       (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_MSK                       (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_UMSK                      (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_RX_BZ_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI                        CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI
#define CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_POS                    (20U)
#define CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_LEN                    (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_MSK                    (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_UMSK                   (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_SEL_WIFI_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN                                 CCI_WIFIPLL_VCO_RSHT_EN
#define CCI_WIFIPLL_VCO_RSHT_EN_POS                             (21U)
#define CCI_WIFIPLL_VCO_RSHT_EN_LEN                             (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_MSK                             (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_UMSK                            (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_HW                              CCI_WIFIPLL_VCO_RSHT_EN_HW
#define CCI_WIFIPLL_VCO_RSHT_EN_HW_POS                          (22U)
#define CCI_WIFIPLL_VCO_RSHT_EN_HW_LEN                          (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_HW_MSK                          (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_HW_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_HW_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_HW_UMSK                         (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_HW_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_HW_POS))
#define CCI_WIFIPLL_WIFI_EN_CTRL_HW                             CCI_WIFIPLL_WIFI_EN_CTRL_HW
#define CCI_WIFIPLL_WIFI_EN_CTRL_HW_POS                         (24U)
#define CCI_WIFIPLL_WIFI_EN_CTRL_HW_LEN                         (1U)
#define CCI_WIFIPLL_WIFI_EN_CTRL_HW_MSK                         (((1U<<CCI_WIFIPLL_WIFI_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_WIFI_EN_CTRL_HW_POS)
#define CCI_WIFIPLL_WIFI_EN_CTRL_HW_UMSK                        (~(((1U<<CCI_WIFIPLL_WIFI_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_WIFI_EN_CTRL_HW_POS))
#define CCI_WIFIPLL_BZ_EN_CTRL_HW                               CCI_WIFIPLL_BZ_EN_CTRL_HW
#define CCI_WIFIPLL_BZ_EN_CTRL_HW_POS                           (25U)
#define CCI_WIFIPLL_BZ_EN_CTRL_HW_LEN                           (1U)
#define CCI_WIFIPLL_BZ_EN_CTRL_HW_MSK                           (((1U<<CCI_WIFIPLL_BZ_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_BZ_EN_CTRL_HW_POS)
#define CCI_WIFIPLL_BZ_EN_CTRL_HW_UMSK                          (~(((1U<<CCI_WIFIPLL_BZ_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_BZ_EN_CTRL_HW_POS))
#define CCI_WIFIPLL_PI_BYPASS_CTRL_HW                           CCI_WIFIPLL_PI_BYPASS_CTRL_HW
#define CCI_WIFIPLL_PI_BYPASS_CTRL_HW_POS                       (26U)
#define CCI_WIFIPLL_PI_BYPASS_CTRL_HW_LEN                       (1U)
#define CCI_WIFIPLL_PI_BYPASS_CTRL_HW_MSK                       (((1U<<CCI_WIFIPLL_PI_BYPASS_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_CTRL_HW_POS)
#define CCI_WIFIPLL_PI_BYPASS_CTRL_HW_UMSK                      (~(((1U<<CCI_WIFIPLL_PI_BYPASS_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_CTRL_HW_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW                         CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW
#define CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_POS                     (27U)
#define CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_LEN                     (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_MSK                     (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_UMSK                    (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_CTRL_HW_POS))
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW                    CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_POS                (28U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN                (1U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_MSK                (((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_POS)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_UMSK               (~(((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_CTRL_HW_POS))
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI                   CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_POS               (29U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_LEN               (1U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_MSK               (((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_POS)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_UMSK              (~(((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_SEL_WIFI_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_HP                           CCI_WIFIPLL_VCO_RSHT_EN_TX_HP
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_POS                       (30U)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_LEN                       (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_MSK                       (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_UMSK                      (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_TX_HP_POS))
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_LP                           CCI_WIFIPLL_VCO_RSHT_EN_TX_LP
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_POS                       (31U)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_LEN                       (1U)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_MSK                       (((1U<<CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_POS)
#define CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_UMSK                      (~(((1U<<CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_LEN)-1)<<CCI_WIFIPLL_VCO_RSHT_EN_TX_LP_POS))

/* 0xD8 : wifipll_spd_fcal */
#define CCI_WIFIPLL_SPD_FCAL_OFFSET                             (0xD8)
#define CCI_WIFIPLL_FCAL_THRESHOLD                              CCI_WIFIPLL_FCAL_THRESHOLD
#define CCI_WIFIPLL_FCAL_THRESHOLD_POS                          (0U)
#define CCI_WIFIPLL_FCAL_THRESHOLD_LEN                          (1U)
#define CCI_WIFIPLL_FCAL_THRESHOLD_MSK                          (((1U<<CCI_WIFIPLL_FCAL_THRESHOLD_LEN)-1)<<CCI_WIFIPLL_FCAL_THRESHOLD_POS)
#define CCI_WIFIPLL_FCAL_THRESHOLD_UMSK                         (~(((1U<<CCI_WIFIPLL_FCAL_THRESHOLD_LEN)-1)<<CCI_WIFIPLL_FCAL_THRESHOLD_POS))
#define CCI_WIFIPLL_FCAL_PHAERR_LIMIT                           CCI_WIFIPLL_FCAL_PHAERR_LIMIT
#define CCI_WIFIPLL_FCAL_PHAERR_LIMIT_POS                       (1U)
#define CCI_WIFIPLL_FCAL_PHAERR_LIMIT_LEN                       (1U)
#define CCI_WIFIPLL_FCAL_PHAERR_LIMIT_MSK                       (((1U<<CCI_WIFIPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<CCI_WIFIPLL_FCAL_PHAERR_LIMIT_POS)
#define CCI_WIFIPLL_FCAL_PHAERR_LIMIT_UMSK                      (~(((1U<<CCI_WIFIPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<CCI_WIFIPLL_FCAL_PHAERR_LIMIT_POS))
#define CCI_WIFIPLL_FCAL_NDIV_ADJ_EN                            CCI_WIFIPLL_FCAL_NDIV_ADJ_EN
#define CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_POS                        (4U)
#define CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_LEN                        (1U)
#define CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_MSK                        (((1U<<CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_POS)
#define CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_UMSK                       (~(((1U<<CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<CCI_WIFIPLL_FCAL_NDIV_ADJ_EN_POS))
#define CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT                         CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT
#define CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_POS                     (8U)
#define CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_LEN                     (7U)
#define CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_MSK                     (((1U<<CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_LEN)-1)<<CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_POS)
#define CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_UMSK                    (~(((1U<<CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_LEN)-1)<<CCI_WIFIPLL_FCAL_MOMCAP_INI_EXT_POS))
#define CCI_WIFIPLL_FCAL_MODE                                   CCI_WIFIPLL_FCAL_MODE
#define CCI_WIFIPLL_FCAL_MODE_POS                               (16U)
#define CCI_WIFIPLL_FCAL_MODE_LEN                               (2U)
#define CCI_WIFIPLL_FCAL_MODE_MSK                               (((1U<<CCI_WIFIPLL_FCAL_MODE_LEN)-1)<<CCI_WIFIPLL_FCAL_MODE_POS)
#define CCI_WIFIPLL_FCAL_MODE_UMSK                              (~(((1U<<CCI_WIFIPLL_FCAL_MODE_LEN)-1)<<CCI_WIFIPLL_FCAL_MODE_POS))
#define CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN                       CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN
#define CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_POS                   (20U)
#define CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_LEN                   (1U)
#define CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_MSK                   (((1U<<CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_POS)
#define CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_UMSK                  (~(((1U<<CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<CCI_WIFIPLL_FAST_INC_DVAR_COMP_EN_POS))
#define CCI_WIFIPLL_FAST_INC_FCAL_EN                            CCI_WIFIPLL_FAST_INC_FCAL_EN
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_POS                        (21U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_LEN                        (1U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_MSK                        (((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_POS)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_UMSK                       (~(((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_POS))
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_HW                         CCI_WIFIPLL_FAST_INC_FCAL_EN_HW
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_POS                     (22U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_LEN                     (1U)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_MSK                     (((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_POS)
#define CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_UMSK                    (~(((1U<<CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_EN_HW_POS))
#define CCI_WIFIPLL_INC_FCAL_EN                                 CCI_WIFIPLL_INC_FCAL_EN
#define CCI_WIFIPLL_INC_FCAL_EN_POS                             (23U)
#define CCI_WIFIPLL_INC_FCAL_EN_LEN                             (1U)
#define CCI_WIFIPLL_INC_FCAL_EN_MSK                             (((1U<<CCI_WIFIPLL_INC_FCAL_EN_LEN)-1)<<CCI_WIFIPLL_INC_FCAL_EN_POS)
#define CCI_WIFIPLL_INC_FCAL_EN_UMSK                            (~(((1U<<CCI_WIFIPLL_INC_FCAL_EN_LEN)-1)<<CCI_WIFIPLL_INC_FCAL_EN_POS))
#define CCI_WIFIPLL_SPD_UNLOCK_DLY                              CCI_WIFIPLL_SPD_UNLOCK_DLY
#define CCI_WIFIPLL_SPD_UNLOCK_DLY_POS                          (24U)
#define CCI_WIFIPLL_SPD_UNLOCK_DLY_LEN                          (1U)
#define CCI_WIFIPLL_SPD_UNLOCK_DLY_MSK                          (((1U<<CCI_WIFIPLL_SPD_UNLOCK_DLY_LEN)-1)<<CCI_WIFIPLL_SPD_UNLOCK_DLY_POS)
#define CCI_WIFIPLL_SPD_UNLOCK_DLY_UMSK                         (~(((1U<<CCI_WIFIPLL_SPD_UNLOCK_DLY_LEN)-1)<<CCI_WIFIPLL_SPD_UNLOCK_DLY_POS))
#define CCI_WIFIPLL_SPD_GAIN                                    CCI_WIFIPLL_SPD_GAIN
#define CCI_WIFIPLL_SPD_GAIN_POS                                (26U)
#define CCI_WIFIPLL_SPD_GAIN_LEN                                (2U)
#define CCI_WIFIPLL_SPD_GAIN_MSK                                (((1U<<CCI_WIFIPLL_SPD_GAIN_LEN)-1)<<CCI_WIFIPLL_SPD_GAIN_POS)
#define CCI_WIFIPLL_SPD_GAIN_UMSK                               (~(((1U<<CCI_WIFIPLL_SPD_GAIN_LEN)-1)<<CCI_WIFIPLL_SPD_GAIN_POS))
#define CCI_WIFIPLL_COARSE_PATH_FORCE_ON                        CCI_WIFIPLL_COARSE_PATH_FORCE_ON
#define CCI_WIFIPLL_COARSE_PATH_FORCE_ON_POS                    (28U)
#define CCI_WIFIPLL_COARSE_PATH_FORCE_ON_LEN                    (1U)
#define CCI_WIFIPLL_COARSE_PATH_FORCE_ON_MSK                    (((1U<<CCI_WIFIPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<CCI_WIFIPLL_COARSE_PATH_FORCE_ON_POS)
#define CCI_WIFIPLL_COARSE_PATH_FORCE_ON_UMSK                   (~(((1U<<CCI_WIFIPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<CCI_WIFIPLL_COARSE_PATH_FORCE_ON_POS))
#define CCI_WIFIPLL_COARSE_GAIN                                 CCI_WIFIPLL_COARSE_GAIN
#define CCI_WIFIPLL_COARSE_GAIN_POS                             (29U)
#define CCI_WIFIPLL_COARSE_GAIN_LEN                             (2U)
#define CCI_WIFIPLL_COARSE_GAIN_MSK                             (((1U<<CCI_WIFIPLL_COARSE_GAIN_LEN)-1)<<CCI_WIFIPLL_COARSE_GAIN_POS)
#define CCI_WIFIPLL_COARSE_GAIN_UMSK                            (~(((1U<<CCI_WIFIPLL_COARSE_GAIN_LEN)-1)<<CCI_WIFIPLL_COARSE_GAIN_POS))

/* 0xDC : wifipll_lf_vctrl */
#define CCI_WIFIPLL_LF_VCTRL_OFFSET                             (0xDC)
#define CCI_WIFIPLL_MOM_UPDATE_PERIOD                           CCI_WIFIPLL_MOM_UPDATE_PERIOD
#define CCI_WIFIPLL_MOM_UPDATE_PERIOD_POS                       (0U)
#define CCI_WIFIPLL_MOM_UPDATE_PERIOD_LEN                       (2U)
#define CCI_WIFIPLL_MOM_UPDATE_PERIOD_MSK                       (((1U<<CCI_WIFIPLL_MOM_UPDATE_PERIOD_LEN)-1)<<CCI_WIFIPLL_MOM_UPDATE_PERIOD_POS)
#define CCI_WIFIPLL_MOM_UPDATE_PERIOD_UMSK                      (~(((1U<<CCI_WIFIPLL_MOM_UPDATE_PERIOD_LEN)-1)<<CCI_WIFIPLL_MOM_UPDATE_PERIOD_POS))
#define CCI_WIFIPLL_MOM_FORCE_HOLD                              CCI_WIFIPLL_MOM_FORCE_HOLD
#define CCI_WIFIPLL_MOM_FORCE_HOLD_POS                          (3U)
#define CCI_WIFIPLL_MOM_FORCE_HOLD_LEN                          (1U)
#define CCI_WIFIPLL_MOM_FORCE_HOLD_MSK                          (((1U<<CCI_WIFIPLL_MOM_FORCE_HOLD_LEN)-1)<<CCI_WIFIPLL_MOM_FORCE_HOLD_POS)
#define CCI_WIFIPLL_MOM_FORCE_HOLD_UMSK                         (~(((1U<<CCI_WIFIPLL_MOM_FORCE_HOLD_LEN)-1)<<CCI_WIFIPLL_MOM_FORCE_HOLD_POS))
#define CCI_WIFIPLL_MOM_CODE_BYPASS                             CCI_WIFIPLL_MOM_CODE_BYPASS
#define CCI_WIFIPLL_MOM_CODE_BYPASS_POS                         (4U)
#define CCI_WIFIPLL_MOM_CODE_BYPASS_LEN                         (1U)
#define CCI_WIFIPLL_MOM_CODE_BYPASS_MSK                         (((1U<<CCI_WIFIPLL_MOM_CODE_BYPASS_LEN)-1)<<CCI_WIFIPLL_MOM_CODE_BYPASS_POS)
#define CCI_WIFIPLL_MOM_CODE_BYPASS_UMSK                        (~(((1U<<CCI_WIFIPLL_MOM_CODE_BYPASS_LEN)-1)<<CCI_WIFIPLL_MOM_CODE_BYPASS_POS))
#define CCI_WIFIPLL_LO_OPEN                                     CCI_WIFIPLL_LO_OPEN
#define CCI_WIFIPLL_LO_OPEN_POS                                 (8U)
#define CCI_WIFIPLL_LO_OPEN_LEN                                 (1U)
#define CCI_WIFIPLL_LO_OPEN_MSK                                 (((1U<<CCI_WIFIPLL_LO_OPEN_LEN)-1)<<CCI_WIFIPLL_LO_OPEN_POS)
#define CCI_WIFIPLL_LO_OPEN_UMSK                                (~(((1U<<CCI_WIFIPLL_LO_OPEN_LEN)-1)<<CCI_WIFIPLL_LO_OPEN_POS))
#define CCI_WIFIPLL_FORCE_LF_FAST_MODE                          CCI_WIFIPLL_FORCE_LF_FAST_MODE
#define CCI_WIFIPLL_FORCE_LF_FAST_MODE_POS                      (10U)
#define CCI_WIFIPLL_FORCE_LF_FAST_MODE_LEN                      (1U)
#define CCI_WIFIPLL_FORCE_LF_FAST_MODE_MSK                      (((1U<<CCI_WIFIPLL_FORCE_LF_FAST_MODE_LEN)-1)<<CCI_WIFIPLL_FORCE_LF_FAST_MODE_POS)
#define CCI_WIFIPLL_FORCE_LF_FAST_MODE_UMSK                     (~(((1U<<CCI_WIFIPLL_FORCE_LF_FAST_MODE_LEN)-1)<<CCI_WIFIPLL_FORCE_LF_FAST_MODE_POS))
#define CCI_WIFIPLL_LF_LSB_EXT                                  CCI_WIFIPLL_LF_LSB_EXT
#define CCI_WIFIPLL_LF_LSB_EXT_POS                              (12U)
#define CCI_WIFIPLL_LF_LSB_EXT_LEN                              (6U)
#define CCI_WIFIPLL_LF_LSB_EXT_MSK                              (((1U<<CCI_WIFIPLL_LF_LSB_EXT_LEN)-1)<<CCI_WIFIPLL_LF_LSB_EXT_POS)
#define CCI_WIFIPLL_LF_LSB_EXT_UMSK                             (~(((1U<<CCI_WIFIPLL_LF_LSB_EXT_LEN)-1)<<CCI_WIFIPLL_LF_LSB_EXT_POS))
#define CCI_WIFIPLL_LF_BETA_FAST                                CCI_WIFIPLL_LF_BETA_FAST
#define CCI_WIFIPLL_LF_BETA_FAST_POS                            (20U)
#define CCI_WIFIPLL_LF_BETA_FAST_LEN                            (1U)
#define CCI_WIFIPLL_LF_BETA_FAST_MSK                            (((1U<<CCI_WIFIPLL_LF_BETA_FAST_LEN)-1)<<CCI_WIFIPLL_LF_BETA_FAST_POS)
#define CCI_WIFIPLL_LF_BETA_FAST_UMSK                           (~(((1U<<CCI_WIFIPLL_LF_BETA_FAST_LEN)-1)<<CCI_WIFIPLL_LF_BETA_FAST_POS))
#define CCI_WIFIPLL_LF_BETA_EXP                                 CCI_WIFIPLL_LF_BETA_EXP
#define CCI_WIFIPLL_LF_BETA_EXP_POS                             (21U)
#define CCI_WIFIPLL_LF_BETA_EXP_LEN                             (3U)
#define CCI_WIFIPLL_LF_BETA_EXP_MSK                             (((1U<<CCI_WIFIPLL_LF_BETA_EXP_LEN)-1)<<CCI_WIFIPLL_LF_BETA_EXP_POS)
#define CCI_WIFIPLL_LF_BETA_EXP_UMSK                            (~(((1U<<CCI_WIFIPLL_LF_BETA_EXP_LEN)-1)<<CCI_WIFIPLL_LF_BETA_EXP_POS))
#define CCI_WIFIPLL_LF_BETA_BASE                                CCI_WIFIPLL_LF_BETA_BASE
#define CCI_WIFIPLL_LF_BETA_BASE_POS                            (24U)
#define CCI_WIFIPLL_LF_BETA_BASE_LEN                            (2U)
#define CCI_WIFIPLL_LF_BETA_BASE_MSK                            (((1U<<CCI_WIFIPLL_LF_BETA_BASE_LEN)-1)<<CCI_WIFIPLL_LF_BETA_BASE_POS)
#define CCI_WIFIPLL_LF_BETA_BASE_UMSK                           (~(((1U<<CCI_WIFIPLL_LF_BETA_BASE_LEN)-1)<<CCI_WIFIPLL_LF_BETA_BASE_POS))
#define CCI_WIFIPLL_LF_ALPHA_FAST                               CCI_WIFIPLL_LF_ALPHA_FAST
#define CCI_WIFIPLL_LF_ALPHA_FAST_POS                           (26U)
#define CCI_WIFIPLL_LF_ALPHA_FAST_LEN                           (2U)
#define CCI_WIFIPLL_LF_ALPHA_FAST_MSK                           (((1U<<CCI_WIFIPLL_LF_ALPHA_FAST_LEN)-1)<<CCI_WIFIPLL_LF_ALPHA_FAST_POS)
#define CCI_WIFIPLL_LF_ALPHA_FAST_UMSK                          (~(((1U<<CCI_WIFIPLL_LF_ALPHA_FAST_LEN)-1)<<CCI_WIFIPLL_LF_ALPHA_FAST_POS))
#define CCI_WIFIPLL_LF_ALPHA_EXP                                CCI_WIFIPLL_LF_ALPHA_EXP
#define CCI_WIFIPLL_LF_ALPHA_EXP_POS                            (28U)
#define CCI_WIFIPLL_LF_ALPHA_EXP_LEN                            (3U)
#define CCI_WIFIPLL_LF_ALPHA_EXP_MSK                            (((1U<<CCI_WIFIPLL_LF_ALPHA_EXP_LEN)-1)<<CCI_WIFIPLL_LF_ALPHA_EXP_POS)
#define CCI_WIFIPLL_LF_ALPHA_EXP_UMSK                           (~(((1U<<CCI_WIFIPLL_LF_ALPHA_EXP_LEN)-1)<<CCI_WIFIPLL_LF_ALPHA_EXP_POS))
#define CCI_WIFIPLL_LF_ALPHA_BASE                               CCI_WIFIPLL_LF_ALPHA_BASE
#define CCI_WIFIPLL_LF_ALPHA_BASE_POS                           (31U)
#define CCI_WIFIPLL_LF_ALPHA_BASE_LEN                           (1U)
#define CCI_WIFIPLL_LF_ALPHA_BASE_MSK                           (((1U<<CCI_WIFIPLL_LF_ALPHA_BASE_LEN)-1)<<CCI_WIFIPLL_LF_ALPHA_BASE_POS)
#define CCI_WIFIPLL_LF_ALPHA_BASE_UMSK                          (~(((1U<<CCI_WIFIPLL_LF_ALPHA_BASE_LEN)-1)<<CCI_WIFIPLL_LF_ALPHA_BASE_POS))

/* 0xE0 : wifipll_sdmin */
#define CCI_WIFIPLL_SDMIN_OFFSET                                (0xE0)
#define CCI_WIFIPLL_SDM_IN                                      CCI_WIFIPLL_SDM_IN
#define CCI_WIFIPLL_SDM_IN_POS                                  (0U)
#define CCI_WIFIPLL_SDM_IN_LEN                                  (25U)
#define CCI_WIFIPLL_SDM_IN_MSK                                  (((1U<<CCI_WIFIPLL_SDM_IN_LEN)-1)<<CCI_WIFIPLL_SDM_IN_POS)
#define CCI_WIFIPLL_SDM_IN_UMSK                                 (~(((1U<<CCI_WIFIPLL_SDM_IN_LEN)-1)<<CCI_WIFIPLL_SDM_IN_POS))

/* 0xE4 : wifipll_pi_sdm_lms */
#define CCI_WIFIPLL_PI_SDM_LMS_OFFSET                           (0xE4)
#define CCI_WIFIPLL_LMS_EXT_VALUE                               CCI_WIFIPLL_LMS_EXT_VALUE
#define CCI_WIFIPLL_LMS_EXT_VALUE_POS                           (0U)
#define CCI_WIFIPLL_LMS_EXT_VALUE_LEN                           (6U)
#define CCI_WIFIPLL_LMS_EXT_VALUE_MSK                           (((1U<<CCI_WIFIPLL_LMS_EXT_VALUE_LEN)-1)<<CCI_WIFIPLL_LMS_EXT_VALUE_POS)
#define CCI_WIFIPLL_LMS_EXT_VALUE_UMSK                          (~(((1U<<CCI_WIFIPLL_LMS_EXT_VALUE_LEN)-1)<<CCI_WIFIPLL_LMS_EXT_VALUE_POS))
#define CCI_WIFIPLL_LMS_EXT_EN                                  CCI_WIFIPLL_LMS_EXT_EN
#define CCI_WIFIPLL_LMS_EXT_EN_POS                              (7U)
#define CCI_WIFIPLL_LMS_EXT_EN_LEN                              (1U)
#define CCI_WIFIPLL_LMS_EXT_EN_MSK                              (((1U<<CCI_WIFIPLL_LMS_EXT_EN_LEN)-1)<<CCI_WIFIPLL_LMS_EXT_EN_POS)
#define CCI_WIFIPLL_LMS_EXT_EN_UMSK                             (~(((1U<<CCI_WIFIPLL_LMS_EXT_EN_LEN)-1)<<CCI_WIFIPLL_LMS_EXT_EN_POS))
#define CCI_WIFIPLL_LMS_POLARITY                                CCI_WIFIPLL_LMS_POLARITY
#define CCI_WIFIPLL_LMS_POLARITY_POS                            (8U)
#define CCI_WIFIPLL_LMS_POLARITY_LEN                            (1U)
#define CCI_WIFIPLL_LMS_POLARITY_MSK                            (((1U<<CCI_WIFIPLL_LMS_POLARITY_LEN)-1)<<CCI_WIFIPLL_LMS_POLARITY_POS)
#define CCI_WIFIPLL_LMS_POLARITY_UMSK                           (~(((1U<<CCI_WIFIPLL_LMS_POLARITY_LEN)-1)<<CCI_WIFIPLL_LMS_POLARITY_POS))
#define CCI_WIFIPLL_PHA_CANCEL_EN                               CCI_WIFIPLL_PHA_CANCEL_EN
#define CCI_WIFIPLL_PHA_CANCEL_EN_POS                           (12U)
#define CCI_WIFIPLL_PHA_CANCEL_EN_LEN                           (1U)
#define CCI_WIFIPLL_PHA_CANCEL_EN_MSK                           (((1U<<CCI_WIFIPLL_PHA_CANCEL_EN_LEN)-1)<<CCI_WIFIPLL_PHA_CANCEL_EN_POS)
#define CCI_WIFIPLL_PHA_CANCEL_EN_UMSK                          (~(((1U<<CCI_WIFIPLL_PHA_CANCEL_EN_LEN)-1)<<CCI_WIFIPLL_PHA_CANCEL_EN_POS))
#define CCI_WIFIPLL_SDM_REGRST_EN                               CCI_WIFIPLL_SDM_REGRST_EN
#define CCI_WIFIPLL_SDM_REGRST_EN_POS                           (15U)
#define CCI_WIFIPLL_SDM_REGRST_EN_LEN                           (1U)
#define CCI_WIFIPLL_SDM_REGRST_EN_MSK                           (((1U<<CCI_WIFIPLL_SDM_REGRST_EN_LEN)-1)<<CCI_WIFIPLL_SDM_REGRST_EN_POS)
#define CCI_WIFIPLL_SDM_REGRST_EN_UMSK                          (~(((1U<<CCI_WIFIPLL_SDM_REGRST_EN_LEN)-1)<<CCI_WIFIPLL_SDM_REGRST_EN_POS))
#define CCI_WIFIPLL_SDM_OUT_DLY                                 CCI_WIFIPLL_SDM_OUT_DLY
#define CCI_WIFIPLL_SDM_OUT_DLY_POS                             (16U)
#define CCI_WIFIPLL_SDM_OUT_DLY_LEN                             (1U)
#define CCI_WIFIPLL_SDM_OUT_DLY_MSK                             (((1U<<CCI_WIFIPLL_SDM_OUT_DLY_LEN)-1)<<CCI_WIFIPLL_SDM_OUT_DLY_POS)
#define CCI_WIFIPLL_SDM_OUT_DLY_UMSK                            (~(((1U<<CCI_WIFIPLL_SDM_OUT_DLY_LEN)-1)<<CCI_WIFIPLL_SDM_OUT_DLY_POS))
#define CCI_WIFIPLL_SDM_ORDER                                   CCI_WIFIPLL_SDM_ORDER
#define CCI_WIFIPLL_SDM_ORDER_POS                               (17U)
#define CCI_WIFIPLL_SDM_ORDER_LEN                               (1U)
#define CCI_WIFIPLL_SDM_ORDER_MSK                               (((1U<<CCI_WIFIPLL_SDM_ORDER_LEN)-1)<<CCI_WIFIPLL_SDM_ORDER_POS)
#define CCI_WIFIPLL_SDM_ORDER_UMSK                              (~(((1U<<CCI_WIFIPLL_SDM_ORDER_LEN)-1)<<CCI_WIFIPLL_SDM_ORDER_POS))
#define CCI_WIFIPLL_SDM_NOI_DITH                                CCI_WIFIPLL_SDM_NOI_DITH
#define CCI_WIFIPLL_SDM_NOI_DITH_POS                            (18U)
#define CCI_WIFIPLL_SDM_NOI_DITH_LEN                            (1U)
#define CCI_WIFIPLL_SDM_NOI_DITH_MSK                            (((1U<<CCI_WIFIPLL_SDM_NOI_DITH_LEN)-1)<<CCI_WIFIPLL_SDM_NOI_DITH_POS)
#define CCI_WIFIPLL_SDM_NOI_DITH_UMSK                           (~(((1U<<CCI_WIFIPLL_SDM_NOI_DITH_LEN)-1)<<CCI_WIFIPLL_SDM_NOI_DITH_POS))
#define CCI_WIFIPLL_SDM_DITH_PRBS_EN                            CCI_WIFIPLL_SDM_DITH_PRBS_EN
#define CCI_WIFIPLL_SDM_DITH_PRBS_EN_POS                        (19U)
#define CCI_WIFIPLL_SDM_DITH_PRBS_EN_LEN                        (1U)
#define CCI_WIFIPLL_SDM_DITH_PRBS_EN_MSK                        (((1U<<CCI_WIFIPLL_SDM_DITH_PRBS_EN_LEN)-1)<<CCI_WIFIPLL_SDM_DITH_PRBS_EN_POS)
#define CCI_WIFIPLL_SDM_DITH_PRBS_EN_UMSK                       (~(((1U<<CCI_WIFIPLL_SDM_DITH_PRBS_EN_LEN)-1)<<CCI_WIFIPLL_SDM_DITH_PRBS_EN_POS))
#define CCI_WIFIPLL_SDM_SIG_DITH                                CCI_WIFIPLL_SDM_SIG_DITH
#define CCI_WIFIPLL_SDM_SIG_DITH_POS                            (20U)
#define CCI_WIFIPLL_SDM_SIG_DITH_LEN                            (2U)
#define CCI_WIFIPLL_SDM_SIG_DITH_MSK                            (((1U<<CCI_WIFIPLL_SDM_SIG_DITH_LEN)-1)<<CCI_WIFIPLL_SDM_SIG_DITH_POS)
#define CCI_WIFIPLL_SDM_SIG_DITH_UMSK                           (~(((1U<<CCI_WIFIPLL_SDM_SIG_DITH_LEN)-1)<<CCI_WIFIPLL_SDM_SIG_DITH_POS))
#define CCI_WIFIPLL_SDM_DITH_EN                                 CCI_WIFIPLL_SDM_DITH_EN
#define CCI_WIFIPLL_SDM_DITH_EN_POS                             (22U)
#define CCI_WIFIPLL_SDM_DITH_EN_LEN                             (1U)
#define CCI_WIFIPLL_SDM_DITH_EN_MSK                             (((1U<<CCI_WIFIPLL_SDM_DITH_EN_LEN)-1)<<CCI_WIFIPLL_SDM_DITH_EN_POS)
#define CCI_WIFIPLL_SDM_DITH_EN_UMSK                            (~(((1U<<CCI_WIFIPLL_SDM_DITH_EN_LEN)-1)<<CCI_WIFIPLL_SDM_DITH_EN_POS))
#define CCI_WIFIPLL_SDM_DITH_FORCE_EN                           CCI_WIFIPLL_SDM_DITH_FORCE_EN
#define CCI_WIFIPLL_SDM_DITH_FORCE_EN_POS                       (23U)
#define CCI_WIFIPLL_SDM_DITH_FORCE_EN_LEN                       (1U)
#define CCI_WIFIPLL_SDM_DITH_FORCE_EN_MSK                       (((1U<<CCI_WIFIPLL_SDM_DITH_FORCE_EN_LEN)-1)<<CCI_WIFIPLL_SDM_DITH_FORCE_EN_POS)
#define CCI_WIFIPLL_SDM_DITH_FORCE_EN_UMSK                      (~(((1U<<CCI_WIFIPLL_SDM_DITH_FORCE_EN_LEN)-1)<<CCI_WIFIPLL_SDM_DITH_FORCE_EN_POS))
#define CCI_WIFIPLL_SDM_BYPASS                                  CCI_WIFIPLL_SDM_BYPASS
#define CCI_WIFIPLL_SDM_BYPASS_POS                              (24U)
#define CCI_WIFIPLL_SDM_BYPASS_LEN                              (1U)
#define CCI_WIFIPLL_SDM_BYPASS_MSK                              (((1U<<CCI_WIFIPLL_SDM_BYPASS_LEN)-1)<<CCI_WIFIPLL_SDM_BYPASS_POS)
#define CCI_WIFIPLL_SDM_BYPASS_UMSK                             (~(((1U<<CCI_WIFIPLL_SDM_BYPASS_LEN)-1)<<CCI_WIFIPLL_SDM_BYPASS_POS))
#define CCI_WIFIPLL_PI_SPACE_SEL                                CCI_WIFIPLL_PI_SPACE_SEL
#define CCI_WIFIPLL_PI_SPACE_SEL_POS                            (27U)
#define CCI_WIFIPLL_PI_SPACE_SEL_LEN                            (1U)
#define CCI_WIFIPLL_PI_SPACE_SEL_MSK                            (((1U<<CCI_WIFIPLL_PI_SPACE_SEL_LEN)-1)<<CCI_WIFIPLL_PI_SPACE_SEL_POS)
#define CCI_WIFIPLL_PI_SPACE_SEL_UMSK                           (~(((1U<<CCI_WIFIPLL_PI_SPACE_SEL_LEN)-1)<<CCI_WIFIPLL_PI_SPACE_SEL_POS))
#define CCI_WIFIPLL_PI_MOD_SEL                                  CCI_WIFIPLL_PI_MOD_SEL
#define CCI_WIFIPLL_PI_MOD_SEL_POS                              (28U)
#define CCI_WIFIPLL_PI_MOD_SEL_LEN                              (2U)
#define CCI_WIFIPLL_PI_MOD_SEL_MSK                              (((1U<<CCI_WIFIPLL_PI_MOD_SEL_LEN)-1)<<CCI_WIFIPLL_PI_MOD_SEL_POS)
#define CCI_WIFIPLL_PI_MOD_SEL_UMSK                             (~(((1U<<CCI_WIFIPLL_PI_MOD_SEL_LEN)-1)<<CCI_WIFIPLL_PI_MOD_SEL_POS))
#define CCI_WIFIPLL_PI_CLK_SEL                                  CCI_WIFIPLL_PI_CLK_SEL
#define CCI_WIFIPLL_PI_CLK_SEL_POS                              (30U)
#define CCI_WIFIPLL_PI_CLK_SEL_LEN                              (1U)
#define CCI_WIFIPLL_PI_CLK_SEL_MSK                              (((1U<<CCI_WIFIPLL_PI_CLK_SEL_LEN)-1)<<CCI_WIFIPLL_PI_CLK_SEL_POS)
#define CCI_WIFIPLL_PI_CLK_SEL_UMSK                             (~(((1U<<CCI_WIFIPLL_PI_CLK_SEL_LEN)-1)<<CCI_WIFIPLL_PI_CLK_SEL_POS))
#define CCI_WIFIPLL_PI_BYPASS                                   CCI_WIFIPLL_PI_BYPASS
#define CCI_WIFIPLL_PI_BYPASS_POS                               (31U)
#define CCI_WIFIPLL_PI_BYPASS_LEN                               (1U)
#define CCI_WIFIPLL_PI_BYPASS_MSK                               (((1U<<CCI_WIFIPLL_PI_BYPASS_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_POS)
#define CCI_WIFIPLL_PI_BYPASS_UMSK                              (~(((1U<<CCI_WIFIPLL_PI_BYPASS_LEN)-1)<<CCI_WIFIPLL_PI_BYPASS_POS))

/* 0xE8 : wifipll_clktree_rf */
#define CCI_WIFIPLL_CLKTREE_RF_OFFSET                           (0xE8)
#define CCI_WIFIPLL_DL_CTRL_DIV30_BZ                            CCI_WIFIPLL_DL_CTRL_DIV30_BZ
#define CCI_WIFIPLL_DL_CTRL_DIV30_BZ_POS                        (0U)
#define CCI_WIFIPLL_DL_CTRL_DIV30_BZ_LEN                        (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV30_BZ_MSK                        (((1U<<CCI_WIFIPLL_DL_CTRL_DIV30_BZ_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV30_BZ_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV30_BZ_UMSK                       (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV30_BZ_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV30_BZ_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV6_BZ                             CCI_WIFIPLL_DL_CTRL_DIV6_BZ
#define CCI_WIFIPLL_DL_CTRL_DIV6_BZ_POS                         (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV6_BZ_LEN                         (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV6_BZ_MSK                         (((1U<<CCI_WIFIPLL_DL_CTRL_DIV6_BZ_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV6_BZ_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV6_BZ_UMSK                        (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV6_BZ_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV6_BZ_POS))
#define CCI_WIFIPLL_EN_BZ_DIV30                                 CCI_WIFIPLL_EN_BZ_DIV30
#define CCI_WIFIPLL_EN_BZ_DIV30_POS                             (4U)
#define CCI_WIFIPLL_EN_BZ_DIV30_LEN                             (1U)
#define CCI_WIFIPLL_EN_BZ_DIV30_MSK                             (((1U<<CCI_WIFIPLL_EN_BZ_DIV30_LEN)-1)<<CCI_WIFIPLL_EN_BZ_DIV30_POS)
#define CCI_WIFIPLL_EN_BZ_DIV30_UMSK                            (~(((1U<<CCI_WIFIPLL_EN_BZ_DIV30_LEN)-1)<<CCI_WIFIPLL_EN_BZ_DIV30_POS))
#define CCI_WIFIPLL_EN_BZ_DIV6_HW                               CCI_WIFIPLL_EN_BZ_DIV6_HW
#define CCI_WIFIPLL_EN_BZ_DIV6_HW_POS                           (8U)
#define CCI_WIFIPLL_EN_BZ_DIV6_HW_LEN                           (1U)
#define CCI_WIFIPLL_EN_BZ_DIV6_HW_MSK                           (((1U<<CCI_WIFIPLL_EN_BZ_DIV6_HW_LEN)-1)<<CCI_WIFIPLL_EN_BZ_DIV6_HW_POS)
#define CCI_WIFIPLL_EN_BZ_DIV6_HW_UMSK                          (~(((1U<<CCI_WIFIPLL_EN_BZ_DIV6_HW_LEN)-1)<<CCI_WIFIPLL_EN_BZ_DIV6_HW_POS))
#define CCI_WIFIPLL_EN_BZ_DIV6                                  CCI_WIFIPLL_EN_BZ_DIV6
#define CCI_WIFIPLL_EN_BZ_DIV6_POS                              (10U)
#define CCI_WIFIPLL_EN_BZ_DIV6_LEN                              (1U)
#define CCI_WIFIPLL_EN_BZ_DIV6_MSK                              (((1U<<CCI_WIFIPLL_EN_BZ_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_BZ_DIV6_POS)
#define CCI_WIFIPLL_EN_BZ_DIV6_UMSK                             (~(((1U<<CCI_WIFIPLL_EN_BZ_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_BZ_DIV6_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV3_RF                             CCI_WIFIPLL_DL_CTRL_DIV3_RF
#define CCI_WIFIPLL_DL_CTRL_DIV3_RF_POS                         (12U)
#define CCI_WIFIPLL_DL_CTRL_DIV3_RF_LEN                         (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV3_RF_MSK                         (((1U<<CCI_WIFIPLL_DL_CTRL_DIV3_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV3_RF_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV3_RF_UMSK                        (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV3_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV3_RF_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV1_RF                             CCI_WIFIPLL_DL_CTRL_DIV1_RF
#define CCI_WIFIPLL_DL_CTRL_DIV1_RF_POS                         (14U)
#define CCI_WIFIPLL_DL_CTRL_DIV1_RF_LEN                         (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV1_RF_MSK                         (((1U<<CCI_WIFIPLL_DL_CTRL_DIV1_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV1_RF_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV1_RF_UMSK                        (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV1_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV1_RF_POS))
#define CCI_WIFIPLL_SEL_DIV3_DIV6                               CCI_WIFIPLL_SEL_DIV3_DIV6
#define CCI_WIFIPLL_SEL_DIV3_DIV6_POS                           (18U)
#define CCI_WIFIPLL_SEL_DIV3_DIV6_LEN                           (1U)
#define CCI_WIFIPLL_SEL_DIV3_DIV6_MSK                           (((1U<<CCI_WIFIPLL_SEL_DIV3_DIV6_LEN)-1)<<CCI_WIFIPLL_SEL_DIV3_DIV6_POS)
#define CCI_WIFIPLL_SEL_DIV3_DIV6_UMSK                          (~(((1U<<CCI_WIFIPLL_SEL_DIV3_DIV6_LEN)-1)<<CCI_WIFIPLL_SEL_DIV3_DIV6_POS))
#define CCI_WIFIPLL_SEL_DIV1_DIV2                               CCI_WIFIPLL_SEL_DIV1_DIV2
#define CCI_WIFIPLL_SEL_DIV1_DIV2_POS                           (19U)
#define CCI_WIFIPLL_SEL_DIV1_DIV2_LEN                           (1U)
#define CCI_WIFIPLL_SEL_DIV1_DIV2_MSK                           (((1U<<CCI_WIFIPLL_SEL_DIV1_DIV2_LEN)-1)<<CCI_WIFIPLL_SEL_DIV1_DIV2_POS)
#define CCI_WIFIPLL_SEL_DIV1_DIV2_UMSK                          (~(((1U<<CCI_WIFIPLL_SEL_DIV1_DIV2_LEN)-1)<<CCI_WIFIPLL_SEL_DIV1_DIV2_POS))
#define CCI_WIFIPLL_EN_RF_DIV3                                  CCI_WIFIPLL_EN_RF_DIV3
#define CCI_WIFIPLL_EN_RF_DIV3_POS                              (20U)
#define CCI_WIFIPLL_EN_RF_DIV3_LEN                              (1U)
#define CCI_WIFIPLL_EN_RF_DIV3_MSK                              (((1U<<CCI_WIFIPLL_EN_RF_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV3_POS)
#define CCI_WIFIPLL_EN_RF_DIV3_UMSK                             (~(((1U<<CCI_WIFIPLL_EN_RF_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV3_POS))
#define CCI_WIFIPLL_EN_RF_DIV1_HW                               CCI_WIFIPLL_EN_RF_DIV1_HW
#define CCI_WIFIPLL_EN_RF_DIV1_HW_POS                           (21U)
#define CCI_WIFIPLL_EN_RF_DIV1_HW_LEN                           (1U)
#define CCI_WIFIPLL_EN_RF_DIV1_HW_MSK                           (((1U<<CCI_WIFIPLL_EN_RF_DIV1_HW_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV1_HW_POS)
#define CCI_WIFIPLL_EN_RF_DIV1_HW_UMSK                          (~(((1U<<CCI_WIFIPLL_EN_RF_DIV1_HW_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV1_HW_POS))
#define CCI_WIFIPLL_EN_RF_DIV1                                  CCI_WIFIPLL_EN_RF_DIV1
#define CCI_WIFIPLL_EN_RF_DIV1_POS                              (22U)
#define CCI_WIFIPLL_EN_RF_DIV1_LEN                              (1U)
#define CCI_WIFIPLL_EN_RF_DIV1_MSK                              (((1U<<CCI_WIFIPLL_EN_RF_DIV1_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV1_POS)
#define CCI_WIFIPLL_EN_RF_DIV1_UMSK                             (~(((1U<<CCI_WIFIPLL_EN_RF_DIV1_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV1_POS))
#define CCI_WIFIPLL_POSTDIV_EN                                  CCI_WIFIPLL_POSTDIV_EN
#define CCI_WIFIPLL_POSTDIV_EN_POS                              (23U)
#define CCI_WIFIPLL_POSTDIV_EN_LEN                              (1U)
#define CCI_WIFIPLL_POSTDIV_EN_MSK                              (((1U<<CCI_WIFIPLL_POSTDIV_EN_LEN)-1)<<CCI_WIFIPLL_POSTDIV_EN_POS)
#define CCI_WIFIPLL_POSTDIV_EN_UMSK                             (~(((1U<<CCI_WIFIPLL_POSTDIV_EN_LEN)-1)<<CCI_WIFIPLL_POSTDIV_EN_POS))
#define CCI_WIFIPLL_POSTDIV                                     CCI_WIFIPLL_POSTDIV
#define CCI_WIFIPLL_POSTDIV_POS                                 (24U)
#define CCI_WIFIPLL_POSTDIV_LEN                                 (8U)
#define CCI_WIFIPLL_POSTDIV_MSK                                 (((1U<<CCI_WIFIPLL_POSTDIV_LEN)-1)<<CCI_WIFIPLL_POSTDIV_POS)
#define CCI_WIFIPLL_POSTDIV_UMSK                                (~(((1U<<CCI_WIFIPLL_POSTDIV_LEN)-1)<<CCI_WIFIPLL_POSTDIV_POS))

/* 0xEC : wifipll_clktree_dig */
#define CCI_WIFIPLL_CLKTREE_DIG_OFFSET                          (0xEC)
#define CCI_WIFIPLL_DL_CTRL_DIV30                               CCI_WIFIPLL_DL_CTRL_DIV30
#define CCI_WIFIPLL_DL_CTRL_DIV30_POS                           (0U)
#define CCI_WIFIPLL_DL_CTRL_DIV30_LEN                           (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV30_MSK                           (((1U<<CCI_WIFIPLL_DL_CTRL_DIV30_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV30_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV30_UMSK                          (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV30_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV30_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV20                               CCI_WIFIPLL_DL_CTRL_DIV20
#define CCI_WIFIPLL_DL_CTRL_DIV20_POS                           (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV20_LEN                           (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV20_MSK                           (((1U<<CCI_WIFIPLL_DL_CTRL_DIV20_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV20_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV20_UMSK                          (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV20_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV20_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV12                               CCI_WIFIPLL_DL_CTRL_DIV12
#define CCI_WIFIPLL_DL_CTRL_DIV12_POS                           (2U)
#define CCI_WIFIPLL_DL_CTRL_DIV12_LEN                           (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV12_MSK                           (((1U<<CCI_WIFIPLL_DL_CTRL_DIV12_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV12_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV12_UMSK                          (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV12_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV12_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV10                               CCI_WIFIPLL_DL_CTRL_DIV10
#define CCI_WIFIPLL_DL_CTRL_DIV10_POS                           (3U)
#define CCI_WIFIPLL_DL_CTRL_DIV10_LEN                           (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV10_MSK                           (((1U<<CCI_WIFIPLL_DL_CTRL_DIV10_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV10_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV10_UMSK                          (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV10_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV10_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV8                                CCI_WIFIPLL_DL_CTRL_DIV8
#define CCI_WIFIPLL_DL_CTRL_DIV8_POS                            (4U)
#define CCI_WIFIPLL_DL_CTRL_DIV8_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV8_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV8_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV8_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV8_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV8_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV8_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV6                                CCI_WIFIPLL_DL_CTRL_DIV6
#define CCI_WIFIPLL_DL_CTRL_DIV6_POS                            (5U)
#define CCI_WIFIPLL_DL_CTRL_DIV6_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV6_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV6_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV6_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV6_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV6_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV6_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV5                                CCI_WIFIPLL_DL_CTRL_DIV5
#define CCI_WIFIPLL_DL_CTRL_DIV5_POS                            (6U)
#define CCI_WIFIPLL_DL_CTRL_DIV5_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV5_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV5_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV5_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV5_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV5_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV5_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV4                                CCI_WIFIPLL_DL_CTRL_DIV4
#define CCI_WIFIPLL_DL_CTRL_DIV4_POS                            (7U)
#define CCI_WIFIPLL_DL_CTRL_DIV4_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV4_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV4_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV4_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV4_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV4_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV4_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV3                                CCI_WIFIPLL_DL_CTRL_DIV3
#define CCI_WIFIPLL_DL_CTRL_DIV3_POS                            (8U)
#define CCI_WIFIPLL_DL_CTRL_DIV3_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV3_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV3_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV3_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV3_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV3_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV3_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV2                                CCI_WIFIPLL_DL_CTRL_DIV2
#define CCI_WIFIPLL_DL_CTRL_DIV2_POS                            (9U)
#define CCI_WIFIPLL_DL_CTRL_DIV2_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV2_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV2_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV2_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV2_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV2_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV2_POS))
#define CCI_WIFIPLL_DL_CTRL_DIV1                                CCI_WIFIPLL_DL_CTRL_DIV1
#define CCI_WIFIPLL_DL_CTRL_DIV1_POS                            (10U)
#define CCI_WIFIPLL_DL_CTRL_DIV1_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_DIV1_MSK                            (((1U<<CCI_WIFIPLL_DL_CTRL_DIV1_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV1_POS)
#define CCI_WIFIPLL_DL_CTRL_DIV1_UMSK                           (~(((1U<<CCI_WIFIPLL_DL_CTRL_DIV1_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_DIV1_POS))
#define CCI_WIFIPLL_EN_DIV30                                    CCI_WIFIPLL_EN_DIV30
#define CCI_WIFIPLL_EN_DIV30_POS                                (16U)
#define CCI_WIFIPLL_EN_DIV30_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV30_MSK                                (((1U<<CCI_WIFIPLL_EN_DIV30_LEN)-1)<<CCI_WIFIPLL_EN_DIV30_POS)
#define CCI_WIFIPLL_EN_DIV30_UMSK                               (~(((1U<<CCI_WIFIPLL_EN_DIV30_LEN)-1)<<CCI_WIFIPLL_EN_DIV30_POS))
#define CCI_WIFIPLL_EN_DIV20                                    CCI_WIFIPLL_EN_DIV20
#define CCI_WIFIPLL_EN_DIV20_POS                                (17U)
#define CCI_WIFIPLL_EN_DIV20_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV20_MSK                                (((1U<<CCI_WIFIPLL_EN_DIV20_LEN)-1)<<CCI_WIFIPLL_EN_DIV20_POS)
#define CCI_WIFIPLL_EN_DIV20_UMSK                               (~(((1U<<CCI_WIFIPLL_EN_DIV20_LEN)-1)<<CCI_WIFIPLL_EN_DIV20_POS))
#define CCI_WIFIPLL_EN_DIV12                                    CCI_WIFIPLL_EN_DIV12
#define CCI_WIFIPLL_EN_DIV12_POS                                (18U)
#define CCI_WIFIPLL_EN_DIV12_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV12_MSK                                (((1U<<CCI_WIFIPLL_EN_DIV12_LEN)-1)<<CCI_WIFIPLL_EN_DIV12_POS)
#define CCI_WIFIPLL_EN_DIV12_UMSK                               (~(((1U<<CCI_WIFIPLL_EN_DIV12_LEN)-1)<<CCI_WIFIPLL_EN_DIV12_POS))
#define CCI_WIFIPLL_EN_DIV10                                    CCI_WIFIPLL_EN_DIV10
#define CCI_WIFIPLL_EN_DIV10_POS                                (19U)
#define CCI_WIFIPLL_EN_DIV10_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV10_MSK                                (((1U<<CCI_WIFIPLL_EN_DIV10_LEN)-1)<<CCI_WIFIPLL_EN_DIV10_POS)
#define CCI_WIFIPLL_EN_DIV10_UMSK                               (~(((1U<<CCI_WIFIPLL_EN_DIV10_LEN)-1)<<CCI_WIFIPLL_EN_DIV10_POS))
#define CCI_WIFIPLL_EN_DIV8                                     CCI_WIFIPLL_EN_DIV8
#define CCI_WIFIPLL_EN_DIV8_POS                                 (20U)
#define CCI_WIFIPLL_EN_DIV8_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV8_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV8_LEN)-1)<<CCI_WIFIPLL_EN_DIV8_POS)
#define CCI_WIFIPLL_EN_DIV8_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV8_LEN)-1)<<CCI_WIFIPLL_EN_DIV8_POS))
#define CCI_WIFIPLL_EN_DIV6                                     CCI_WIFIPLL_EN_DIV6
#define CCI_WIFIPLL_EN_DIV6_POS                                 (21U)
#define CCI_WIFIPLL_EN_DIV6_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV6_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_DIV6_POS)
#define CCI_WIFIPLL_EN_DIV6_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_DIV6_POS))
#define CCI_WIFIPLL_EN_DIV5                                     CCI_WIFIPLL_EN_DIV5
#define CCI_WIFIPLL_EN_DIV5_POS                                 (22U)
#define CCI_WIFIPLL_EN_DIV5_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV5_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV5_LEN)-1)<<CCI_WIFIPLL_EN_DIV5_POS)
#define CCI_WIFIPLL_EN_DIV5_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV5_LEN)-1)<<CCI_WIFIPLL_EN_DIV5_POS))
#define CCI_WIFIPLL_EN_DIV4                                     CCI_WIFIPLL_EN_DIV4
#define CCI_WIFIPLL_EN_DIV4_POS                                 (23U)
#define CCI_WIFIPLL_EN_DIV4_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV4_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV4_LEN)-1)<<CCI_WIFIPLL_EN_DIV4_POS)
#define CCI_WIFIPLL_EN_DIV4_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV4_LEN)-1)<<CCI_WIFIPLL_EN_DIV4_POS))
#define CCI_WIFIPLL_EN_DIV3                                     CCI_WIFIPLL_EN_DIV3
#define CCI_WIFIPLL_EN_DIV3_POS                                 (24U)
#define CCI_WIFIPLL_EN_DIV3_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV3_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_DIV3_POS)
#define CCI_WIFIPLL_EN_DIV3_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_DIV3_POS))
#define CCI_WIFIPLL_EN_DIV2                                     CCI_WIFIPLL_EN_DIV2
#define CCI_WIFIPLL_EN_DIV2_POS                                 (25U)
#define CCI_WIFIPLL_EN_DIV2_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV2_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV2_LEN)-1)<<CCI_WIFIPLL_EN_DIV2_POS)
#define CCI_WIFIPLL_EN_DIV2_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV2_LEN)-1)<<CCI_WIFIPLL_EN_DIV2_POS))
#define CCI_WIFIPLL_EN_DIV1                                     CCI_WIFIPLL_EN_DIV1
#define CCI_WIFIPLL_EN_DIV1_POS                                 (26U)
#define CCI_WIFIPLL_EN_DIV1_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV1_MSK                                 (((1U<<CCI_WIFIPLL_EN_DIV1_LEN)-1)<<CCI_WIFIPLL_EN_DIV1_POS)
#define CCI_WIFIPLL_EN_DIV1_UMSK                                (~(((1U<<CCI_WIFIPLL_EN_DIV1_LEN)-1)<<CCI_WIFIPLL_EN_DIV1_POS))
#define CCI_WIFIPLL_EN_USB2_DIV48                               CCI_WIFIPLL_EN_USB2_DIV48
#define CCI_WIFIPLL_EN_USB2_DIV48_POS                           (27U)
#define CCI_WIFIPLL_EN_USB2_DIV48_LEN                           (1U)
#define CCI_WIFIPLL_EN_USB2_DIV48_MSK                           (((1U<<CCI_WIFIPLL_EN_USB2_DIV48_LEN)-1)<<CCI_WIFIPLL_EN_USB2_DIV48_POS)
#define CCI_WIFIPLL_EN_USB2_DIV48_UMSK                          (~(((1U<<CCI_WIFIPLL_EN_USB2_DIV48_LEN)-1)<<CCI_WIFIPLL_EN_USB2_DIV48_POS))
#define CCI_WIFIPLL_DL_CTRL_USB2_DIV48                          CCI_WIFIPLL_DL_CTRL_USB2_DIV48
#define CCI_WIFIPLL_DL_CTRL_USB2_DIV48_POS                      (28U)
#define CCI_WIFIPLL_DL_CTRL_USB2_DIV48_LEN                      (1U)
#define CCI_WIFIPLL_DL_CTRL_USB2_DIV48_MSK                      (((1U<<CCI_WIFIPLL_DL_CTRL_USB2_DIV48_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_USB2_DIV48_POS)
#define CCI_WIFIPLL_DL_CTRL_USB2_DIV48_UMSK                     (~(((1U<<CCI_WIFIPLL_DL_CTRL_USB2_DIV48_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_USB2_DIV48_POS))

/* 0xF0 : wifipll_reserve */
#define CCI_WIFIPLL_RESERVE_OFFSET                              (0xF0)
#define CCI_WIFIPLL_RESV0                                       CCI_WIFIPLL_RESV0
#define CCI_WIFIPLL_RESV0_POS                                   (0U)
#define CCI_WIFIPLL_RESV0_LEN                                   (8U)
#define CCI_WIFIPLL_RESV0_MSK                                   (((1U<<CCI_WIFIPLL_RESV0_LEN)-1)<<CCI_WIFIPLL_RESV0_POS)
#define CCI_WIFIPLL_RESV0_UMSK                                  (~(((1U<<CCI_WIFIPLL_RESV0_LEN)-1)<<CCI_WIFIPLL_RESV0_POS))
#define CCI_WIFIPLL_RESV1                                       CCI_WIFIPLL_RESV1
#define CCI_WIFIPLL_RESV1_POS                                   (8U)
#define CCI_WIFIPLL_RESV1_LEN                                   (8U)
#define CCI_WIFIPLL_RESV1_MSK                                   (((1U<<CCI_WIFIPLL_RESV1_LEN)-1)<<CCI_WIFIPLL_RESV1_POS)
#define CCI_WIFIPLL_RESV1_UMSK                                  (~(((1U<<CCI_WIFIPLL_RESV1_LEN)-1)<<CCI_WIFIPLL_RESV1_POS))
#define CCI_WIFIPLL_VCO_FREQ_RECORD                             CCI_WIFIPLL_VCO_FREQ_RECORD
#define CCI_WIFIPLL_VCO_FREQ_RECORD_POS                         (16U)
#define CCI_WIFIPLL_VCO_FREQ_RECORD_LEN                         (16U)
#define CCI_WIFIPLL_VCO_FREQ_RECORD_MSK                         (((1U<<CCI_WIFIPLL_VCO_FREQ_RECORD_LEN)-1)<<CCI_WIFIPLL_VCO_FREQ_RECORD_POS)
#define CCI_WIFIPLL_VCO_FREQ_RECORD_UMSK                        (~(((1U<<CCI_WIFIPLL_VCO_FREQ_RECORD_LEN)-1)<<CCI_WIFIPLL_VCO_FREQ_RECORD_POS))

/* 0xF4 : wifipll_test */
#define CCI_WIFIPLL_TEST_OFFSET                                 (0xF4)
#define CCI_WIFIPLL_TEST_DATA_MUX                               CCI_WIFIPLL_TEST_DATA_MUX
#define CCI_WIFIPLL_TEST_DATA_MUX_POS                           (0U)
#define CCI_WIFIPLL_TEST_DATA_MUX_LEN                           (1U)
#define CCI_WIFIPLL_TEST_DATA_MUX_MSK                           (((1U<<CCI_WIFIPLL_TEST_DATA_MUX_LEN)-1)<<CCI_WIFIPLL_TEST_DATA_MUX_POS)
#define CCI_WIFIPLL_TEST_DATA_MUX_UMSK                          (~(((1U<<CCI_WIFIPLL_TEST_DATA_MUX_LEN)-1)<<CCI_WIFIPLL_TEST_DATA_MUX_POS))
#define CCI_WIFIPLL_TEST_START_MUX                              CCI_WIFIPLL_TEST_START_MUX
#define CCI_WIFIPLL_TEST_START_MUX_POS                          (4U)
#define CCI_WIFIPLL_TEST_START_MUX_LEN                          (1U)
#define CCI_WIFIPLL_TEST_START_MUX_MSK                          (((1U<<CCI_WIFIPLL_TEST_START_MUX_LEN)-1)<<CCI_WIFIPLL_TEST_START_MUX_POS)
#define CCI_WIFIPLL_TEST_START_MUX_UMSK                         (~(((1U<<CCI_WIFIPLL_TEST_START_MUX_LEN)-1)<<CCI_WIFIPLL_TEST_START_MUX_POS))
#define CCI_WIFIPLL_TEST_EN                                     CCI_WIFIPLL_TEST_EN
#define CCI_WIFIPLL_TEST_EN_POS                                 (8U)
#define CCI_WIFIPLL_TEST_EN_LEN                                 (1U)
#define CCI_WIFIPLL_TEST_EN_MSK                                 (((1U<<CCI_WIFIPLL_TEST_EN_LEN)-1)<<CCI_WIFIPLL_TEST_EN_POS)
#define CCI_WIFIPLL_TEST_EN_UMSK                                (~(((1U<<CCI_WIFIPLL_TEST_EN_LEN)-1)<<CCI_WIFIPLL_TEST_EN_POS))
#define CCI_DTEN_WIFIPLL_REFCLK                                 CCI_DTEN_WIFIPLL_REFCLK
#define CCI_DTEN_WIFIPLL_REFCLK_POS                             (10U)
#define CCI_DTEN_WIFIPLL_REFCLK_LEN                             (1U)
#define CCI_DTEN_WIFIPLL_REFCLK_MSK                             (((1U<<CCI_DTEN_WIFIPLL_REFCLK_LEN)-1)<<CCI_DTEN_WIFIPLL_REFCLK_POS)
#define CCI_DTEN_WIFIPLL_REFCLK_UMSK                            (~(((1U<<CCI_DTEN_WIFIPLL_REFCLK_LEN)-1)<<CCI_DTEN_WIFIPLL_REFCLK_POS))
#define CCI_DTEN_WIFIPLL_POSTDIV                                CCI_DTEN_WIFIPLL_POSTDIV
#define CCI_DTEN_WIFIPLL_POSTDIV_POS                            (12U)
#define CCI_DTEN_WIFIPLL_POSTDIV_LEN                            (1U)
#define CCI_DTEN_WIFIPLL_POSTDIV_MSK                            (((1U<<CCI_DTEN_WIFIPLL_POSTDIV_LEN)-1)<<CCI_DTEN_WIFIPLL_POSTDIV_POS)
#define CCI_DTEN_WIFIPLL_POSTDIV_UMSK                           (~(((1U<<CCI_DTEN_WIFIPLL_POSTDIV_LEN)-1)<<CCI_DTEN_WIFIPLL_POSTDIV_POS))
#define CCI_DTEN_WIFIPLL_FBDIV_FSDM                             CCI_DTEN_WIFIPLL_FBDIV_FSDM
#define CCI_DTEN_WIFIPLL_FBDIV_FSDM_POS                         (16U)
#define CCI_DTEN_WIFIPLL_FBDIV_FSDM_LEN                         (1U)
#define CCI_DTEN_WIFIPLL_FBDIV_FSDM_MSK                         (((1U<<CCI_DTEN_WIFIPLL_FBDIV_FSDM_LEN)-1)<<CCI_DTEN_WIFIPLL_FBDIV_FSDM_POS)
#define CCI_DTEN_WIFIPLL_FBDIV_FSDM_UMSK                        (~(((1U<<CCI_DTEN_WIFIPLL_FBDIV_FSDM_LEN)-1)<<CCI_DTEN_WIFIPLL_FBDIV_FSDM_POS))
#define CCI_DTEN_WIFIPLL_FBDIV_FFB                              CCI_DTEN_WIFIPLL_FBDIV_FFB
#define CCI_DTEN_WIFIPLL_FBDIV_FFB_POS                          (20U)
#define CCI_DTEN_WIFIPLL_FBDIV_FFB_LEN                          (1U)
#define CCI_DTEN_WIFIPLL_FBDIV_FFB_MSK                          (((1U<<CCI_DTEN_WIFIPLL_FBDIV_FFB_LEN)-1)<<CCI_DTEN_WIFIPLL_FBDIV_FFB_POS)
#define CCI_DTEN_WIFIPLL_FBDIV_FFB_UMSK                         (~(((1U<<CCI_DTEN_WIFIPLL_FBDIV_FFB_LEN)-1)<<CCI_DTEN_WIFIPLL_FBDIV_FFB_POS))
#define CCI_DTEN_WIFIPLL_CLK32M                                 CCI_DTEN_WIFIPLL_CLK32M
#define CCI_DTEN_WIFIPLL_CLK32M_POS                             (24U)
#define CCI_DTEN_WIFIPLL_CLK32M_LEN                             (1U)
#define CCI_DTEN_WIFIPLL_CLK32M_MSK                             (((1U<<CCI_DTEN_WIFIPLL_CLK32M_LEN)-1)<<CCI_DTEN_WIFIPLL_CLK32M_POS)
#define CCI_DTEN_WIFIPLL_CLK32M_UMSK                            (~(((1U<<CCI_DTEN_WIFIPLL_CLK32M_LEN)-1)<<CCI_DTEN_WIFIPLL_CLK32M_POS))
#define CCI_TEN_WIFIPLL                                         CCI_TEN_WIFIPLL
#define CCI_TEN_WIFIPLL_POS                                     (28U)
#define CCI_TEN_WIFIPLL_LEN                                     (1U)
#define CCI_TEN_WIFIPLL_MSK                                     (((1U<<CCI_TEN_WIFIPLL_LEN)-1)<<CCI_TEN_WIFIPLL_POS)
#define CCI_TEN_WIFIPLL_UMSK                                    (~(((1U<<CCI_TEN_WIFIPLL_LEN)-1)<<CCI_TEN_WIFIPLL_POS))

/* 0xF8 : wifipll_readback */
#define CCI_WIFIPLL_READBACK_OFFSET                             (0xF8)
#define CCI_WIFIPLL_TEST_START                                  CCI_WIFIPLL_TEST_START
#define CCI_WIFIPLL_TEST_START_POS                              (9U)
#define CCI_WIFIPLL_TEST_START_LEN                              (1U)
#define CCI_WIFIPLL_TEST_START_MSK                              (((1U<<CCI_WIFIPLL_TEST_START_LEN)-1)<<CCI_WIFIPLL_TEST_START_POS)
#define CCI_WIFIPLL_TEST_START_UMSK                             (~(((1U<<CCI_WIFIPLL_TEST_START_LEN)-1)<<CCI_WIFIPLL_TEST_START_POS))
#define CCI_WIFIPLL_VCTRL_OUT_RANGE                             CCI_WIFIPLL_VCTRL_OUT_RANGE
#define CCI_WIFIPLL_VCTRL_OUT_RANGE_POS                         (12U)
#define CCI_WIFIPLL_VCTRL_OUT_RANGE_LEN                         (1U)
#define CCI_WIFIPLL_VCTRL_OUT_RANGE_MSK                         (((1U<<CCI_WIFIPLL_VCTRL_OUT_RANGE_LEN)-1)<<CCI_WIFIPLL_VCTRL_OUT_RANGE_POS)
#define CCI_WIFIPLL_VCTRL_OUT_RANGE_UMSK                        (~(((1U<<CCI_WIFIPLL_VCTRL_OUT_RANGE_LEN)-1)<<CCI_WIFIPLL_VCTRL_OUT_RANGE_POS))
#define CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU                     CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU
#define CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_POS                 (16U)
#define CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN                 (2U)
#define CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_MSK                 (((1U<<CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN)-1)<<CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_POS)
#define CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_UMSK                (~(((1U<<CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN)-1)<<CCI_WIFIPLL_SOC_MOM_UPDATE_TOTAL_OU_POS))
#define CCI_WIFIPLL_LO_UNLOCK                                   CCI_WIFIPLL_LO_UNLOCK
#define CCI_WIFIPLL_LO_UNLOCK_POS                               (20U)
#define CCI_WIFIPLL_LO_UNLOCK_LEN                               (1U)
#define CCI_WIFIPLL_LO_UNLOCK_MSK                               (((1U<<CCI_WIFIPLL_LO_UNLOCK_LEN)-1)<<CCI_WIFIPLL_LO_UNLOCK_POS)
#define CCI_WIFIPLL_LO_UNLOCK_UMSK                              (~(((1U<<CCI_WIFIPLL_LO_UNLOCK_LEN)-1)<<CCI_WIFIPLL_LO_UNLOCK_POS))
#define CCI_WIFIPLL_LO_LOCK                                     CCI_WIFIPLL_LO_LOCK
#define CCI_WIFIPLL_LO_LOCK_POS                                 (24U)
#define CCI_WIFIPLL_LO_LOCK_LEN                                 (1U)
#define CCI_WIFIPLL_LO_LOCK_MSK                                 (((1U<<CCI_WIFIPLL_LO_LOCK_LEN)-1)<<CCI_WIFIPLL_LO_LOCK_POS)
#define CCI_WIFIPLL_LO_LOCK_UMSK                                (~(((1U<<CCI_WIFIPLL_LO_LOCK_LEN)-1)<<CCI_WIFIPLL_LO_LOCK_POS))
#define CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW                        CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW
#define CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_POS                    (28U)
#define CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_LEN                    (1U)
#define CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_MSK                    (((1U<<CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_POS)
#define CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_UMSK                   (~(((1U<<CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_LEN)-1)<<CCI_WIFIPLL_FAST_INC_FCAL_OVFLOW_POS))

/* 0xFC : wifipll_adpll_test_out */
#define CCI_WIFIPLL_ADPLL_TEST_OUT_OFFSET                       (0xFC)
#define CCI_WIFIPLL_TEST_OUT                                    CCI_WIFIPLL_TEST_OUT
#define CCI_WIFIPLL_TEST_OUT_POS                                (0U)
#define CCI_WIFIPLL_TEST_OUT_LEN                                (32U)
#define CCI_WIFIPLL_TEST_OUT_MSK                                (((1U<<CCI_WIFIPLL_TEST_OUT_LEN)-1)<<CCI_WIFIPLL_TEST_OUT_POS)
#define CCI_WIFIPLL_TEST_OUT_UMSK                               (~(((1U<<CCI_WIFIPLL_TEST_OUT_LEN)-1)<<CCI_WIFIPLL_TEST_OUT_POS))

/* 0x7D0 : start-up control */
#define CCI_CPUPLL_ANA_CTRL_OFFSET                              (0x7D0)
#define CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK                         CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK
#define CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_POS                     (0U)
#define CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_LEN                     (2U)
#define CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_MSK                     (((1U<<CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_LEN)-1)<<CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_POS)
#define CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_UMSK                    (~(((1U<<CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_LEN)-1)<<CCI_CPUPLL_FBDIV_SEL_SAMPLE_CLK_POS))
#define CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK                          CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK
#define CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_POS                      (2U)
#define CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_LEN                      (1U)
#define CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_MSK                      (((1U<<CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_POS)
#define CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_UMSK                     (~(((1U<<CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_LEN)-1)<<CCI_CPUPLL_FBDIV_SEL_ADPLL_CLK_POS))
#define CCI_CPUPLL_DTC_R_SEL                                    CCI_CPUPLL_DTC_R_SEL
#define CCI_CPUPLL_DTC_R_SEL_POS                                (4U)
#define CCI_CPUPLL_DTC_R_SEL_LEN                                (2U)
#define CCI_CPUPLL_DTC_R_SEL_MSK                                (((1U<<CCI_CPUPLL_DTC_R_SEL_LEN)-1)<<CCI_CPUPLL_DTC_R_SEL_POS)
#define CCI_CPUPLL_DTC_R_SEL_UMSK                               (~(((1U<<CCI_CPUPLL_DTC_R_SEL_LEN)-1)<<CCI_CPUPLL_DTC_R_SEL_POS))
#define CCI_CPUPLL_DTC_BYPASS_EN                                CCI_CPUPLL_DTC_BYPASS_EN
#define CCI_CPUPLL_DTC_BYPASS_EN_POS                            (6U)
#define CCI_CPUPLL_DTC_BYPASS_EN_LEN                            (1U)
#define CCI_CPUPLL_DTC_BYPASS_EN_MSK                            (((1U<<CCI_CPUPLL_DTC_BYPASS_EN_LEN)-1)<<CCI_CPUPLL_DTC_BYPASS_EN_POS)
#define CCI_CPUPLL_DTC_BYPASS_EN_UMSK                           (~(((1U<<CCI_CPUPLL_DTC_BYPASS_EN_LEN)-1)<<CCI_CPUPLL_DTC_BYPASS_EN_POS))
#define CCI_CPUPLL_VCO_SPEED                                    CCI_CPUPLL_VCO_SPEED
#define CCI_CPUPLL_VCO_SPEED_POS                                (8U)
#define CCI_CPUPLL_VCO_SPEED_LEN                                (3U)
#define CCI_CPUPLL_VCO_SPEED_MSK                                (((1U<<CCI_CPUPLL_VCO_SPEED_LEN)-1)<<CCI_CPUPLL_VCO_SPEED_POS)
#define CCI_CPUPLL_VCO_SPEED_UMSK                               (~(((1U<<CCI_CPUPLL_VCO_SPEED_LEN)-1)<<CCI_CPUPLL_VCO_SPEED_POS))
#define CCI_CPUPLL_VCO_LDO_BYPASS                               CCI_CPUPLL_VCO_LDO_BYPASS
#define CCI_CPUPLL_VCO_LDO_BYPASS_POS                           (12U)
#define CCI_CPUPLL_VCO_LDO_BYPASS_LEN                           (1U)
#define CCI_CPUPLL_VCO_LDO_BYPASS_MSK                           (((1U<<CCI_CPUPLL_VCO_LDO_BYPASS_LEN)-1)<<CCI_CPUPLL_VCO_LDO_BYPASS_POS)
#define CCI_CPUPLL_VCO_LDO_BYPASS_UMSK                          (~(((1U<<CCI_CPUPLL_VCO_LDO_BYPASS_LEN)-1)<<CCI_CPUPLL_VCO_LDO_BYPASS_POS))
#define CCI_CPUPLL_REFCLK_SEL                                   CCI_CPUPLL_REFCLK_SEL
#define CCI_CPUPLL_REFCLK_SEL_POS                               (16U)
#define CCI_CPUPLL_REFCLK_SEL_LEN                               (2U)
#define CCI_CPUPLL_REFCLK_SEL_MSK                               (((1U<<CCI_CPUPLL_REFCLK_SEL_LEN)-1)<<CCI_CPUPLL_REFCLK_SEL_POS)
#define CCI_CPUPLL_REFCLK_SEL_UMSK                              (~(((1U<<CCI_CPUPLL_REFCLK_SEL_LEN)-1)<<CCI_CPUPLL_REFCLK_SEL_POS))
#define CCI_CPUPLL_REFCLK_DIV_RATIO                             CCI_CPUPLL_REFCLK_DIV_RATIO
#define CCI_CPUPLL_REFCLK_DIV_RATIO_POS                         (20U)
#define CCI_CPUPLL_REFCLK_DIV_RATIO_LEN                         (4U)
#define CCI_CPUPLL_REFCLK_DIV_RATIO_MSK                         (((1U<<CCI_CPUPLL_REFCLK_DIV_RATIO_LEN)-1)<<CCI_CPUPLL_REFCLK_DIV_RATIO_POS)
#define CCI_CPUPLL_REFCLK_DIV_RATIO_UMSK                        (~(((1U<<CCI_CPUPLL_REFCLK_DIV_RATIO_LEN)-1)<<CCI_CPUPLL_REFCLK_DIV_RATIO_POS))
#define CCI_CPUPLL_RSTB                                         CCI_CPUPLL_RSTB
#define CCI_CPUPLL_RSTB_POS                                     (24U)
#define CCI_CPUPLL_RSTB_LEN                                     (1U)
#define CCI_CPUPLL_RSTB_MSK                                     (((1U<<CCI_CPUPLL_RSTB_LEN)-1)<<CCI_CPUPLL_RSTB_POS)
#define CCI_CPUPLL_RSTB_UMSK                                    (~(((1U<<CCI_CPUPLL_RSTB_LEN)-1)<<CCI_CPUPLL_RSTB_POS))
#define CCI_PU_CPUPLL                                           CCI_PU_CPUPLL
#define CCI_PU_CPUPLL_POS                                       (28U)
#define CCI_PU_CPUPLL_LEN                                       (1U)
#define CCI_PU_CPUPLL_MSK                                       (((1U<<CCI_PU_CPUPLL_LEN)-1)<<CCI_PU_CPUPLL_POS)
#define CCI_PU_CPUPLL_UMSK                                      (~(((1U<<CCI_PU_CPUPLL_LEN)-1)<<CCI_PU_CPUPLL_POS))

/* 0x7D4 : cpupll_spd_fcal */
#define CCI_CPUPLL_SPD_FCAL_OFFSET                              (0x7D4)
#define CCI_CPUPLL_FCAL_THRESHOLD                               CCI_CPUPLL_FCAL_THRESHOLD
#define CCI_CPUPLL_FCAL_THRESHOLD_POS                           (0U)
#define CCI_CPUPLL_FCAL_THRESHOLD_LEN                           (1U)
#define CCI_CPUPLL_FCAL_THRESHOLD_MSK                           (((1U<<CCI_CPUPLL_FCAL_THRESHOLD_LEN)-1)<<CCI_CPUPLL_FCAL_THRESHOLD_POS)
#define CCI_CPUPLL_FCAL_THRESHOLD_UMSK                          (~(((1U<<CCI_CPUPLL_FCAL_THRESHOLD_LEN)-1)<<CCI_CPUPLL_FCAL_THRESHOLD_POS))
#define CCI_CPUPLL_FCAL_PHAERR_LIMIT                            CCI_CPUPLL_FCAL_PHAERR_LIMIT
#define CCI_CPUPLL_FCAL_PHAERR_LIMIT_POS                        (1U)
#define CCI_CPUPLL_FCAL_PHAERR_LIMIT_LEN                        (1U)
#define CCI_CPUPLL_FCAL_PHAERR_LIMIT_MSK                        (((1U<<CCI_CPUPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<CCI_CPUPLL_FCAL_PHAERR_LIMIT_POS)
#define CCI_CPUPLL_FCAL_PHAERR_LIMIT_UMSK                       (~(((1U<<CCI_CPUPLL_FCAL_PHAERR_LIMIT_LEN)-1)<<CCI_CPUPLL_FCAL_PHAERR_LIMIT_POS))
#define CCI_CPUPLL_FCAL_NDIV_ADJ_EN                             CCI_CPUPLL_FCAL_NDIV_ADJ_EN
#define CCI_CPUPLL_FCAL_NDIV_ADJ_EN_POS                         (4U)
#define CCI_CPUPLL_FCAL_NDIV_ADJ_EN_LEN                         (1U)
#define CCI_CPUPLL_FCAL_NDIV_ADJ_EN_MSK                         (((1U<<CCI_CPUPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<CCI_CPUPLL_FCAL_NDIV_ADJ_EN_POS)
#define CCI_CPUPLL_FCAL_NDIV_ADJ_EN_UMSK                        (~(((1U<<CCI_CPUPLL_FCAL_NDIV_ADJ_EN_LEN)-1)<<CCI_CPUPLL_FCAL_NDIV_ADJ_EN_POS))
#define CCI_CPUPLL_FCAL_MOMCAP_INI_EXT                          CCI_CPUPLL_FCAL_MOMCAP_INI_EXT
#define CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_POS                      (8U)
#define CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_LEN                      (7U)
#define CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_MSK                      (((1U<<CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_LEN)-1)<<CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_POS)
#define CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_UMSK                     (~(((1U<<CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_LEN)-1)<<CCI_CPUPLL_FCAL_MOMCAP_INI_EXT_POS))
#define CCI_CPUPLL_FCAL_MODE                                    CCI_CPUPLL_FCAL_MODE
#define CCI_CPUPLL_FCAL_MODE_POS                                (16U)
#define CCI_CPUPLL_FCAL_MODE_LEN                                (2U)
#define CCI_CPUPLL_FCAL_MODE_MSK                                (((1U<<CCI_CPUPLL_FCAL_MODE_LEN)-1)<<CCI_CPUPLL_FCAL_MODE_POS)
#define CCI_CPUPLL_FCAL_MODE_UMSK                               (~(((1U<<CCI_CPUPLL_FCAL_MODE_LEN)-1)<<CCI_CPUPLL_FCAL_MODE_POS))
#define CCI_CPUPLL_FAST_INC_DVAR_COMP_EN                        CCI_CPUPLL_FAST_INC_DVAR_COMP_EN
#define CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_POS                    (20U)
#define CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_LEN                    (1U)
#define CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_MSK                    (((1U<<CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_POS)
#define CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_UMSK                   (~(((1U<<CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_LEN)-1)<<CCI_CPUPLL_FAST_INC_DVAR_COMP_EN_POS))
#define CCI_CPUPLL_FAST_INC_FCAL_EN                             CCI_CPUPLL_FAST_INC_FCAL_EN
#define CCI_CPUPLL_FAST_INC_FCAL_EN_POS                         (21U)
#define CCI_CPUPLL_FAST_INC_FCAL_EN_LEN                         (1U)
#define CCI_CPUPLL_FAST_INC_FCAL_EN_MSK                         (((1U<<CCI_CPUPLL_FAST_INC_FCAL_EN_LEN)-1)<<CCI_CPUPLL_FAST_INC_FCAL_EN_POS)
#define CCI_CPUPLL_FAST_INC_FCAL_EN_UMSK                        (~(((1U<<CCI_CPUPLL_FAST_INC_FCAL_EN_LEN)-1)<<CCI_CPUPLL_FAST_INC_FCAL_EN_POS))
#define CCI_CPUPLL_INC_FCAL_EN                                  CCI_CPUPLL_INC_FCAL_EN
#define CCI_CPUPLL_INC_FCAL_EN_POS                              (23U)
#define CCI_CPUPLL_INC_FCAL_EN_LEN                              (1U)
#define CCI_CPUPLL_INC_FCAL_EN_MSK                              (((1U<<CCI_CPUPLL_INC_FCAL_EN_LEN)-1)<<CCI_CPUPLL_INC_FCAL_EN_POS)
#define CCI_CPUPLL_INC_FCAL_EN_UMSK                             (~(((1U<<CCI_CPUPLL_INC_FCAL_EN_LEN)-1)<<CCI_CPUPLL_INC_FCAL_EN_POS))
#define CCI_CPUPLL_SPD_UNLOCK_DLY                               CCI_CPUPLL_SPD_UNLOCK_DLY
#define CCI_CPUPLL_SPD_UNLOCK_DLY_POS                           (24U)
#define CCI_CPUPLL_SPD_UNLOCK_DLY_LEN                           (1U)
#define CCI_CPUPLL_SPD_UNLOCK_DLY_MSK                           (((1U<<CCI_CPUPLL_SPD_UNLOCK_DLY_LEN)-1)<<CCI_CPUPLL_SPD_UNLOCK_DLY_POS)
#define CCI_CPUPLL_SPD_UNLOCK_DLY_UMSK                          (~(((1U<<CCI_CPUPLL_SPD_UNLOCK_DLY_LEN)-1)<<CCI_CPUPLL_SPD_UNLOCK_DLY_POS))
#define CCI_CPUPLL_SPD_GAIN                                     CCI_CPUPLL_SPD_GAIN
#define CCI_CPUPLL_SPD_GAIN_POS                                 (26U)
#define CCI_CPUPLL_SPD_GAIN_LEN                                 (2U)
#define CCI_CPUPLL_SPD_GAIN_MSK                                 (((1U<<CCI_CPUPLL_SPD_GAIN_LEN)-1)<<CCI_CPUPLL_SPD_GAIN_POS)
#define CCI_CPUPLL_SPD_GAIN_UMSK                                (~(((1U<<CCI_CPUPLL_SPD_GAIN_LEN)-1)<<CCI_CPUPLL_SPD_GAIN_POS))
#define CCI_CPUPLL_COARSE_PATH_FORCE_ON                         CCI_CPUPLL_COARSE_PATH_FORCE_ON
#define CCI_CPUPLL_COARSE_PATH_FORCE_ON_POS                     (28U)
#define CCI_CPUPLL_COARSE_PATH_FORCE_ON_LEN                     (1U)
#define CCI_CPUPLL_COARSE_PATH_FORCE_ON_MSK                     (((1U<<CCI_CPUPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<CCI_CPUPLL_COARSE_PATH_FORCE_ON_POS)
#define CCI_CPUPLL_COARSE_PATH_FORCE_ON_UMSK                    (~(((1U<<CCI_CPUPLL_COARSE_PATH_FORCE_ON_LEN)-1)<<CCI_CPUPLL_COARSE_PATH_FORCE_ON_POS))
#define CCI_CPUPLL_COARSE_GAIN                                  CCI_CPUPLL_COARSE_GAIN
#define CCI_CPUPLL_COARSE_GAIN_POS                              (29U)
#define CCI_CPUPLL_COARSE_GAIN_LEN                              (2U)
#define CCI_CPUPLL_COARSE_GAIN_MSK                              (((1U<<CCI_CPUPLL_COARSE_GAIN_LEN)-1)<<CCI_CPUPLL_COARSE_GAIN_POS)
#define CCI_CPUPLL_COARSE_GAIN_UMSK                             (~(((1U<<CCI_CPUPLL_COARSE_GAIN_LEN)-1)<<CCI_CPUPLL_COARSE_GAIN_POS))

/* 0x7D8 : cpupll_lf_vctrl */
#define CCI_CPUPLL_LF_VCTRL_OFFSET                              (0x7D8)
#define CCI_CPUPLL_MOM_UPDATE_PERIOD                            CCI_CPUPLL_MOM_UPDATE_PERIOD
#define CCI_CPUPLL_MOM_UPDATE_PERIOD_POS                        (0U)
#define CCI_CPUPLL_MOM_UPDATE_PERIOD_LEN                        (2U)
#define CCI_CPUPLL_MOM_UPDATE_PERIOD_MSK                        (((1U<<CCI_CPUPLL_MOM_UPDATE_PERIOD_LEN)-1)<<CCI_CPUPLL_MOM_UPDATE_PERIOD_POS)
#define CCI_CPUPLL_MOM_UPDATE_PERIOD_UMSK                       (~(((1U<<CCI_CPUPLL_MOM_UPDATE_PERIOD_LEN)-1)<<CCI_CPUPLL_MOM_UPDATE_PERIOD_POS))
#define CCI_CPUPLL_MOM_FORCE_HOLD                               CCI_CPUPLL_MOM_FORCE_HOLD
#define CCI_CPUPLL_MOM_FORCE_HOLD_POS                           (3U)
#define CCI_CPUPLL_MOM_FORCE_HOLD_LEN                           (1U)
#define CCI_CPUPLL_MOM_FORCE_HOLD_MSK                           (((1U<<CCI_CPUPLL_MOM_FORCE_HOLD_LEN)-1)<<CCI_CPUPLL_MOM_FORCE_HOLD_POS)
#define CCI_CPUPLL_MOM_FORCE_HOLD_UMSK                          (~(((1U<<CCI_CPUPLL_MOM_FORCE_HOLD_LEN)-1)<<CCI_CPUPLL_MOM_FORCE_HOLD_POS))
#define CCI_CPUPLL_MOM_CODE_BYPASS                              CCI_CPUPLL_MOM_CODE_BYPASS
#define CCI_CPUPLL_MOM_CODE_BYPASS_POS                          (4U)
#define CCI_CPUPLL_MOM_CODE_BYPASS_LEN                          (1U)
#define CCI_CPUPLL_MOM_CODE_BYPASS_MSK                          (((1U<<CCI_CPUPLL_MOM_CODE_BYPASS_LEN)-1)<<CCI_CPUPLL_MOM_CODE_BYPASS_POS)
#define CCI_CPUPLL_MOM_CODE_BYPASS_UMSK                         (~(((1U<<CCI_CPUPLL_MOM_CODE_BYPASS_LEN)-1)<<CCI_CPUPLL_MOM_CODE_BYPASS_POS))
#define CCI_CPUPLL_DCOMASH_BYPASS                               CCI_CPUPLL_DCOMASH_BYPASS
#define CCI_CPUPLL_DCOMASH_BYPASS_POS                           (6U)
#define CCI_CPUPLL_DCOMASH_BYPASS_LEN                           (1U)
#define CCI_CPUPLL_DCOMASH_BYPASS_MSK                           (((1U<<CCI_CPUPLL_DCOMASH_BYPASS_LEN)-1)<<CCI_CPUPLL_DCOMASH_BYPASS_POS)
#define CCI_CPUPLL_DCOMASH_BYPASS_UMSK                          (~(((1U<<CCI_CPUPLL_DCOMASH_BYPASS_LEN)-1)<<CCI_CPUPLL_DCOMASH_BYPASS_POS))
#define CCI_CPUPLL_LO_OPEN                                      CCI_CPUPLL_LO_OPEN
#define CCI_CPUPLL_LO_OPEN_POS                                  (8U)
#define CCI_CPUPLL_LO_OPEN_LEN                                  (1U)
#define CCI_CPUPLL_LO_OPEN_MSK                                  (((1U<<CCI_CPUPLL_LO_OPEN_LEN)-1)<<CCI_CPUPLL_LO_OPEN_POS)
#define CCI_CPUPLL_LO_OPEN_UMSK                                 (~(((1U<<CCI_CPUPLL_LO_OPEN_LEN)-1)<<CCI_CPUPLL_LO_OPEN_POS))
#define CCI_CPUPLL_FORCE_LF_FAST_MODE                           CCI_CPUPLL_FORCE_LF_FAST_MODE
#define CCI_CPUPLL_FORCE_LF_FAST_MODE_POS                       (10U)
#define CCI_CPUPLL_FORCE_LF_FAST_MODE_LEN                       (1U)
#define CCI_CPUPLL_FORCE_LF_FAST_MODE_MSK                       (((1U<<CCI_CPUPLL_FORCE_LF_FAST_MODE_LEN)-1)<<CCI_CPUPLL_FORCE_LF_FAST_MODE_POS)
#define CCI_CPUPLL_FORCE_LF_FAST_MODE_UMSK                      (~(((1U<<CCI_CPUPLL_FORCE_LF_FAST_MODE_LEN)-1)<<CCI_CPUPLL_FORCE_LF_FAST_MODE_POS))
#define CCI_CPUPLL_LF_LSB_EXT                                   CCI_CPUPLL_LF_LSB_EXT
#define CCI_CPUPLL_LF_LSB_EXT_POS                               (12U)
#define CCI_CPUPLL_LF_LSB_EXT_LEN                               (6U)
#define CCI_CPUPLL_LF_LSB_EXT_MSK                               (((1U<<CCI_CPUPLL_LF_LSB_EXT_LEN)-1)<<CCI_CPUPLL_LF_LSB_EXT_POS)
#define CCI_CPUPLL_LF_LSB_EXT_UMSK                              (~(((1U<<CCI_CPUPLL_LF_LSB_EXT_LEN)-1)<<CCI_CPUPLL_LF_LSB_EXT_POS))
#define CCI_CPUPLL_LF_BETA_FAST                                 CCI_CPUPLL_LF_BETA_FAST
#define CCI_CPUPLL_LF_BETA_FAST_POS                             (20U)
#define CCI_CPUPLL_LF_BETA_FAST_LEN                             (1U)
#define CCI_CPUPLL_LF_BETA_FAST_MSK                             (((1U<<CCI_CPUPLL_LF_BETA_FAST_LEN)-1)<<CCI_CPUPLL_LF_BETA_FAST_POS)
#define CCI_CPUPLL_LF_BETA_FAST_UMSK                            (~(((1U<<CCI_CPUPLL_LF_BETA_FAST_LEN)-1)<<CCI_CPUPLL_LF_BETA_FAST_POS))
#define CCI_CPUPLL_LF_BETA_EXP                                  CCI_CPUPLL_LF_BETA_EXP
#define CCI_CPUPLL_LF_BETA_EXP_POS                              (21U)
#define CCI_CPUPLL_LF_BETA_EXP_LEN                              (3U)
#define CCI_CPUPLL_LF_BETA_EXP_MSK                              (((1U<<CCI_CPUPLL_LF_BETA_EXP_LEN)-1)<<CCI_CPUPLL_LF_BETA_EXP_POS)
#define CCI_CPUPLL_LF_BETA_EXP_UMSK                             (~(((1U<<CCI_CPUPLL_LF_BETA_EXP_LEN)-1)<<CCI_CPUPLL_LF_BETA_EXP_POS))
#define CCI_CPUPLL_LF_BETA_BASE                                 CCI_CPUPLL_LF_BETA_BASE
#define CCI_CPUPLL_LF_BETA_BASE_POS                             (24U)
#define CCI_CPUPLL_LF_BETA_BASE_LEN                             (2U)
#define CCI_CPUPLL_LF_BETA_BASE_MSK                             (((1U<<CCI_CPUPLL_LF_BETA_BASE_LEN)-1)<<CCI_CPUPLL_LF_BETA_BASE_POS)
#define CCI_CPUPLL_LF_BETA_BASE_UMSK                            (~(((1U<<CCI_CPUPLL_LF_BETA_BASE_LEN)-1)<<CCI_CPUPLL_LF_BETA_BASE_POS))
#define CCI_CPUPLL_LF_ALPHA_FAST                                CCI_CPUPLL_LF_ALPHA_FAST
#define CCI_CPUPLL_LF_ALPHA_FAST_POS                            (26U)
#define CCI_CPUPLL_LF_ALPHA_FAST_LEN                            (2U)
#define CCI_CPUPLL_LF_ALPHA_FAST_MSK                            (((1U<<CCI_CPUPLL_LF_ALPHA_FAST_LEN)-1)<<CCI_CPUPLL_LF_ALPHA_FAST_POS)
#define CCI_CPUPLL_LF_ALPHA_FAST_UMSK                           (~(((1U<<CCI_CPUPLL_LF_ALPHA_FAST_LEN)-1)<<CCI_CPUPLL_LF_ALPHA_FAST_POS))
#define CCI_CPUPLL_LF_ALPHA_EXP                                 CCI_CPUPLL_LF_ALPHA_EXP
#define CCI_CPUPLL_LF_ALPHA_EXP_POS                             (28U)
#define CCI_CPUPLL_LF_ALPHA_EXP_LEN                             (3U)
#define CCI_CPUPLL_LF_ALPHA_EXP_MSK                             (((1U<<CCI_CPUPLL_LF_ALPHA_EXP_LEN)-1)<<CCI_CPUPLL_LF_ALPHA_EXP_POS)
#define CCI_CPUPLL_LF_ALPHA_EXP_UMSK                            (~(((1U<<CCI_CPUPLL_LF_ALPHA_EXP_LEN)-1)<<CCI_CPUPLL_LF_ALPHA_EXP_POS))
#define CCI_CPUPLL_LF_ALPHA_BASE                                CCI_CPUPLL_LF_ALPHA_BASE
#define CCI_CPUPLL_LF_ALPHA_BASE_POS                            (31U)
#define CCI_CPUPLL_LF_ALPHA_BASE_LEN                            (1U)
#define CCI_CPUPLL_LF_ALPHA_BASE_MSK                            (((1U<<CCI_CPUPLL_LF_ALPHA_BASE_LEN)-1)<<CCI_CPUPLL_LF_ALPHA_BASE_POS)
#define CCI_CPUPLL_LF_ALPHA_BASE_UMSK                           (~(((1U<<CCI_CPUPLL_LF_ALPHA_BASE_LEN)-1)<<CCI_CPUPLL_LF_ALPHA_BASE_POS))

/* 0x7DC : sdm control */
#define CCI_CPUPLL_SDM1_OFFSET                                  (0x7DC)
#define CCI_CPUPLL_SDM_IN                                       CCI_CPUPLL_SDM_IN
#define CCI_CPUPLL_SDM_IN_POS                                   (0U)
#define CCI_CPUPLL_SDM_IN_LEN                                   (19U)
#define CCI_CPUPLL_SDM_IN_MSK                                   (((1U<<CCI_CPUPLL_SDM_IN_LEN)-1)<<CCI_CPUPLL_SDM_IN_POS)
#define CCI_CPUPLL_SDM_IN_UMSK                                  (~(((1U<<CCI_CPUPLL_SDM_IN_LEN)-1)<<CCI_CPUPLL_SDM_IN_POS))

/* 0x7E0 : cpupll_sdm_lms */
#define CCI_CPUPLL_SDM_LMS_OFFSET                               (0x7E0)
#define CCI_CPUPLL_LMS_EXT_VALUE                                CCI_CPUPLL_LMS_EXT_VALUE
#define CCI_CPUPLL_LMS_EXT_VALUE_POS                            (0U)
#define CCI_CPUPLL_LMS_EXT_VALUE_LEN                            (6U)
#define CCI_CPUPLL_LMS_EXT_VALUE_MSK                            (((1U<<CCI_CPUPLL_LMS_EXT_VALUE_LEN)-1)<<CCI_CPUPLL_LMS_EXT_VALUE_POS)
#define CCI_CPUPLL_LMS_EXT_VALUE_UMSK                           (~(((1U<<CCI_CPUPLL_LMS_EXT_VALUE_LEN)-1)<<CCI_CPUPLL_LMS_EXT_VALUE_POS))
#define CCI_CPUPLL_LMS_EXT_EN                                   CCI_CPUPLL_LMS_EXT_EN
#define CCI_CPUPLL_LMS_EXT_EN_POS                               (7U)
#define CCI_CPUPLL_LMS_EXT_EN_LEN                               (1U)
#define CCI_CPUPLL_LMS_EXT_EN_MSK                               (((1U<<CCI_CPUPLL_LMS_EXT_EN_LEN)-1)<<CCI_CPUPLL_LMS_EXT_EN_POS)
#define CCI_CPUPLL_LMS_EXT_EN_UMSK                              (~(((1U<<CCI_CPUPLL_LMS_EXT_EN_LEN)-1)<<CCI_CPUPLL_LMS_EXT_EN_POS))
#define CCI_CPUPLL_LMS_POLARITY                                 CCI_CPUPLL_LMS_POLARITY
#define CCI_CPUPLL_LMS_POLARITY_POS                             (8U)
#define CCI_CPUPLL_LMS_POLARITY_LEN                             (1U)
#define CCI_CPUPLL_LMS_POLARITY_MSK                             (((1U<<CCI_CPUPLL_LMS_POLARITY_LEN)-1)<<CCI_CPUPLL_LMS_POLARITY_POS)
#define CCI_CPUPLL_LMS_POLARITY_UMSK                            (~(((1U<<CCI_CPUPLL_LMS_POLARITY_LEN)-1)<<CCI_CPUPLL_LMS_POLARITY_POS))
#define CCI_CPUPLL_PHA_CANCEL_EN                                CCI_CPUPLL_PHA_CANCEL_EN
#define CCI_CPUPLL_PHA_CANCEL_EN_POS                            (12U)
#define CCI_CPUPLL_PHA_CANCEL_EN_LEN                            (1U)
#define CCI_CPUPLL_PHA_CANCEL_EN_MSK                            (((1U<<CCI_CPUPLL_PHA_CANCEL_EN_LEN)-1)<<CCI_CPUPLL_PHA_CANCEL_EN_POS)
#define CCI_CPUPLL_PHA_CANCEL_EN_UMSK                           (~(((1U<<CCI_CPUPLL_PHA_CANCEL_EN_LEN)-1)<<CCI_CPUPLL_PHA_CANCEL_EN_POS))
#define CCI_CPUPLL_SDM_REGRST_EN                                CCI_CPUPLL_SDM_REGRST_EN
#define CCI_CPUPLL_SDM_REGRST_EN_POS                            (15U)
#define CCI_CPUPLL_SDM_REGRST_EN_LEN                            (1U)
#define CCI_CPUPLL_SDM_REGRST_EN_MSK                            (((1U<<CCI_CPUPLL_SDM_REGRST_EN_LEN)-1)<<CCI_CPUPLL_SDM_REGRST_EN_POS)
#define CCI_CPUPLL_SDM_REGRST_EN_UMSK                           (~(((1U<<CCI_CPUPLL_SDM_REGRST_EN_LEN)-1)<<CCI_CPUPLL_SDM_REGRST_EN_POS))
#define CCI_CPUPLL_SDM_OUT_DLY                                  CCI_CPUPLL_SDM_OUT_DLY
#define CCI_CPUPLL_SDM_OUT_DLY_POS                              (16U)
#define CCI_CPUPLL_SDM_OUT_DLY_LEN                              (1U)
#define CCI_CPUPLL_SDM_OUT_DLY_MSK                              (((1U<<CCI_CPUPLL_SDM_OUT_DLY_LEN)-1)<<CCI_CPUPLL_SDM_OUT_DLY_POS)
#define CCI_CPUPLL_SDM_OUT_DLY_UMSK                             (~(((1U<<CCI_CPUPLL_SDM_OUT_DLY_LEN)-1)<<CCI_CPUPLL_SDM_OUT_DLY_POS))
#define CCI_CPUPLL_SDM_ORDER                                    CCI_CPUPLL_SDM_ORDER
#define CCI_CPUPLL_SDM_ORDER_POS                                (17U)
#define CCI_CPUPLL_SDM_ORDER_LEN                                (1U)
#define CCI_CPUPLL_SDM_ORDER_MSK                                (((1U<<CCI_CPUPLL_SDM_ORDER_LEN)-1)<<CCI_CPUPLL_SDM_ORDER_POS)
#define CCI_CPUPLL_SDM_ORDER_UMSK                               (~(((1U<<CCI_CPUPLL_SDM_ORDER_LEN)-1)<<CCI_CPUPLL_SDM_ORDER_POS))
#define CCI_CPUPLL_SDM_SIG_DITH                                 CCI_CPUPLL_SDM_SIG_DITH
#define CCI_CPUPLL_SDM_SIG_DITH_POS                             (20U)
#define CCI_CPUPLL_SDM_SIG_DITH_LEN                             (2U)
#define CCI_CPUPLL_SDM_SIG_DITH_MSK                             (((1U<<CCI_CPUPLL_SDM_SIG_DITH_LEN)-1)<<CCI_CPUPLL_SDM_SIG_DITH_POS)
#define CCI_CPUPLL_SDM_SIG_DITH_UMSK                            (~(((1U<<CCI_CPUPLL_SDM_SIG_DITH_LEN)-1)<<CCI_CPUPLL_SDM_SIG_DITH_POS))
#define CCI_CPUPLL_SDM_DITH_FORCE_EN                            CCI_CPUPLL_SDM_DITH_FORCE_EN
#define CCI_CPUPLL_SDM_DITH_FORCE_EN_POS                        (23U)
#define CCI_CPUPLL_SDM_DITH_FORCE_EN_LEN                        (1U)
#define CCI_CPUPLL_SDM_DITH_FORCE_EN_MSK                        (((1U<<CCI_CPUPLL_SDM_DITH_FORCE_EN_LEN)-1)<<CCI_CPUPLL_SDM_DITH_FORCE_EN_POS)
#define CCI_CPUPLL_SDM_DITH_FORCE_EN_UMSK                       (~(((1U<<CCI_CPUPLL_SDM_DITH_FORCE_EN_LEN)-1)<<CCI_CPUPLL_SDM_DITH_FORCE_EN_POS))
#define CCI_CPUPLL_SDM_BYPASS                                   CCI_CPUPLL_SDM_BYPASS
#define CCI_CPUPLL_SDM_BYPASS_POS                               (24U)
#define CCI_CPUPLL_SDM_BYPASS_LEN                               (1U)
#define CCI_CPUPLL_SDM_BYPASS_MSK                               (((1U<<CCI_CPUPLL_SDM_BYPASS_LEN)-1)<<CCI_CPUPLL_SDM_BYPASS_POS)
#define CCI_CPUPLL_SDM_BYPASS_UMSK                              (~(((1U<<CCI_CPUPLL_SDM_BYPASS_LEN)-1)<<CCI_CPUPLL_SDM_BYPASS_POS))

/* 0x7E4 : cpupll_clktree_dig */
#define CCI_CPUPLL_CLKTREE_DIG_OFFSET                           (0x7E4)
#define CCI_CPUPLL_DL_CTRL_DIV30                                CCI_CPUPLL_DL_CTRL_DIV30
#define CCI_CPUPLL_DL_CTRL_DIV30_POS                            (0U)
#define CCI_CPUPLL_DL_CTRL_DIV30_LEN                            (1U)
#define CCI_CPUPLL_DL_CTRL_DIV30_MSK                            (((1U<<CCI_CPUPLL_DL_CTRL_DIV30_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV30_POS)
#define CCI_CPUPLL_DL_CTRL_DIV30_UMSK                           (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV30_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV30_POS))
#define CCI_CPUPLL_DL_CTRL_DIV20                                CCI_CPUPLL_DL_CTRL_DIV20
#define CCI_CPUPLL_DL_CTRL_DIV20_POS                            (1U)
#define CCI_CPUPLL_DL_CTRL_DIV20_LEN                            (1U)
#define CCI_CPUPLL_DL_CTRL_DIV20_MSK                            (((1U<<CCI_CPUPLL_DL_CTRL_DIV20_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV20_POS)
#define CCI_CPUPLL_DL_CTRL_DIV20_UMSK                           (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV20_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV20_POS))
#define CCI_CPUPLL_DL_CTRL_DIV12                                CCI_CPUPLL_DL_CTRL_DIV12
#define CCI_CPUPLL_DL_CTRL_DIV12_POS                            (2U)
#define CCI_CPUPLL_DL_CTRL_DIV12_LEN                            (1U)
#define CCI_CPUPLL_DL_CTRL_DIV12_MSK                            (((1U<<CCI_CPUPLL_DL_CTRL_DIV12_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV12_POS)
#define CCI_CPUPLL_DL_CTRL_DIV12_UMSK                           (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV12_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV12_POS))
#define CCI_CPUPLL_DL_CTRL_DIV10                                CCI_CPUPLL_DL_CTRL_DIV10
#define CCI_CPUPLL_DL_CTRL_DIV10_POS                            (3U)
#define CCI_CPUPLL_DL_CTRL_DIV10_LEN                            (1U)
#define CCI_CPUPLL_DL_CTRL_DIV10_MSK                            (((1U<<CCI_CPUPLL_DL_CTRL_DIV10_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV10_POS)
#define CCI_CPUPLL_DL_CTRL_DIV10_UMSK                           (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV10_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV10_POS))
#define CCI_CPUPLL_DL_CTRL_DIV8                                 CCI_CPUPLL_DL_CTRL_DIV8
#define CCI_CPUPLL_DL_CTRL_DIV8_POS                             (4U)
#define CCI_CPUPLL_DL_CTRL_DIV8_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV8_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV8_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV8_POS)
#define CCI_CPUPLL_DL_CTRL_DIV8_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV8_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV8_POS))
#define CCI_CPUPLL_DL_CTRL_DIV6                                 CCI_CPUPLL_DL_CTRL_DIV6
#define CCI_CPUPLL_DL_CTRL_DIV6_POS                             (5U)
#define CCI_CPUPLL_DL_CTRL_DIV6_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV6_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV6_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV6_POS)
#define CCI_CPUPLL_DL_CTRL_DIV6_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV6_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV6_POS))
#define CCI_CPUPLL_DL_CTRL_DIV5                                 CCI_CPUPLL_DL_CTRL_DIV5
#define CCI_CPUPLL_DL_CTRL_DIV5_POS                             (6U)
#define CCI_CPUPLL_DL_CTRL_DIV5_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV5_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV5_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV5_POS)
#define CCI_CPUPLL_DL_CTRL_DIV5_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV5_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV5_POS))
#define CCI_CPUPLL_DL_CTRL_DIV4                                 CCI_CPUPLL_DL_CTRL_DIV4
#define CCI_CPUPLL_DL_CTRL_DIV4_POS                             (7U)
#define CCI_CPUPLL_DL_CTRL_DIV4_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV4_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV4_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV4_POS)
#define CCI_CPUPLL_DL_CTRL_DIV4_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV4_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV4_POS))
#define CCI_CPUPLL_DL_CTRL_DIV3                                 CCI_CPUPLL_DL_CTRL_DIV3
#define CCI_CPUPLL_DL_CTRL_DIV3_POS                             (8U)
#define CCI_CPUPLL_DL_CTRL_DIV3_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV3_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV3_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV3_POS)
#define CCI_CPUPLL_DL_CTRL_DIV3_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV3_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV3_POS))
#define CCI_CPUPLL_DL_CTRL_DIV2                                 CCI_CPUPLL_DL_CTRL_DIV2
#define CCI_CPUPLL_DL_CTRL_DIV2_POS                             (9U)
#define CCI_CPUPLL_DL_CTRL_DIV2_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV2_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV2_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV2_POS)
#define CCI_CPUPLL_DL_CTRL_DIV2_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV2_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV2_POS))
#define CCI_CPUPLL_DL_CTRL_DIV1                                 CCI_CPUPLL_DL_CTRL_DIV1
#define CCI_CPUPLL_DL_CTRL_DIV1_POS                             (10U)
#define CCI_CPUPLL_DL_CTRL_DIV1_LEN                             (1U)
#define CCI_CPUPLL_DL_CTRL_DIV1_MSK                             (((1U<<CCI_CPUPLL_DL_CTRL_DIV1_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV1_POS)
#define CCI_CPUPLL_DL_CTRL_DIV1_UMSK                            (~(((1U<<CCI_CPUPLL_DL_CTRL_DIV1_LEN)-1)<<CCI_CPUPLL_DL_CTRL_DIV1_POS))
#define CCI_CPUPLL_EN_DIV30                                     CCI_CPUPLL_EN_DIV30
#define CCI_CPUPLL_EN_DIV30_POS                                 (16U)
#define CCI_CPUPLL_EN_DIV30_LEN                                 (1U)
#define CCI_CPUPLL_EN_DIV30_MSK                                 (((1U<<CCI_CPUPLL_EN_DIV30_LEN)-1)<<CCI_CPUPLL_EN_DIV30_POS)
#define CCI_CPUPLL_EN_DIV30_UMSK                                (~(((1U<<CCI_CPUPLL_EN_DIV30_LEN)-1)<<CCI_CPUPLL_EN_DIV30_POS))
#define CCI_CPUPLL_EN_DIV20                                     CCI_CPUPLL_EN_DIV20
#define CCI_CPUPLL_EN_DIV20_POS                                 (17U)
#define CCI_CPUPLL_EN_DIV20_LEN                                 (1U)
#define CCI_CPUPLL_EN_DIV20_MSK                                 (((1U<<CCI_CPUPLL_EN_DIV20_LEN)-1)<<CCI_CPUPLL_EN_DIV20_POS)
#define CCI_CPUPLL_EN_DIV20_UMSK                                (~(((1U<<CCI_CPUPLL_EN_DIV20_LEN)-1)<<CCI_CPUPLL_EN_DIV20_POS))
#define CCI_CPUPLL_EN_DIV12                                     CCI_CPUPLL_EN_DIV12
#define CCI_CPUPLL_EN_DIV12_POS                                 (18U)
#define CCI_CPUPLL_EN_DIV12_LEN                                 (1U)
#define CCI_CPUPLL_EN_DIV12_MSK                                 (((1U<<CCI_CPUPLL_EN_DIV12_LEN)-1)<<CCI_CPUPLL_EN_DIV12_POS)
#define CCI_CPUPLL_EN_DIV12_UMSK                                (~(((1U<<CCI_CPUPLL_EN_DIV12_LEN)-1)<<CCI_CPUPLL_EN_DIV12_POS))
#define CCI_CPUPLL_EN_DIV10                                     CCI_CPUPLL_EN_DIV10
#define CCI_CPUPLL_EN_DIV10_POS                                 (19U)
#define CCI_CPUPLL_EN_DIV10_LEN                                 (1U)
#define CCI_CPUPLL_EN_DIV10_MSK                                 (((1U<<CCI_CPUPLL_EN_DIV10_LEN)-1)<<CCI_CPUPLL_EN_DIV10_POS)
#define CCI_CPUPLL_EN_DIV10_UMSK                                (~(((1U<<CCI_CPUPLL_EN_DIV10_LEN)-1)<<CCI_CPUPLL_EN_DIV10_POS))
#define CCI_CPUPLL_EN_DIV8                                      CCI_CPUPLL_EN_DIV8
#define CCI_CPUPLL_EN_DIV8_POS                                  (20U)
#define CCI_CPUPLL_EN_DIV8_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV8_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV8_LEN)-1)<<CCI_CPUPLL_EN_DIV8_POS)
#define CCI_CPUPLL_EN_DIV8_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV8_LEN)-1)<<CCI_CPUPLL_EN_DIV8_POS))
#define CCI_CPUPLL_EN_DIV6                                      CCI_CPUPLL_EN_DIV6
#define CCI_CPUPLL_EN_DIV6_POS                                  (21U)
#define CCI_CPUPLL_EN_DIV6_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV6_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV6_LEN)-1)<<CCI_CPUPLL_EN_DIV6_POS)
#define CCI_CPUPLL_EN_DIV6_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV6_LEN)-1)<<CCI_CPUPLL_EN_DIV6_POS))
#define CCI_CPUPLL_EN_DIV5                                      CCI_CPUPLL_EN_DIV5
#define CCI_CPUPLL_EN_DIV5_POS                                  (22U)
#define CCI_CPUPLL_EN_DIV5_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV5_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV5_LEN)-1)<<CCI_CPUPLL_EN_DIV5_POS)
#define CCI_CPUPLL_EN_DIV5_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV5_LEN)-1)<<CCI_CPUPLL_EN_DIV5_POS))
#define CCI_CPUPLL_EN_DIV4                                      CCI_CPUPLL_EN_DIV4
#define CCI_CPUPLL_EN_DIV4_POS                                  (23U)
#define CCI_CPUPLL_EN_DIV4_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV4_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV4_LEN)-1)<<CCI_CPUPLL_EN_DIV4_POS)
#define CCI_CPUPLL_EN_DIV4_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV4_LEN)-1)<<CCI_CPUPLL_EN_DIV4_POS))
#define CCI_CPUPLL_EN_DIV3                                      CCI_CPUPLL_EN_DIV3
#define CCI_CPUPLL_EN_DIV3_POS                                  (24U)
#define CCI_CPUPLL_EN_DIV3_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV3_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV3_LEN)-1)<<CCI_CPUPLL_EN_DIV3_POS)
#define CCI_CPUPLL_EN_DIV3_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV3_LEN)-1)<<CCI_CPUPLL_EN_DIV3_POS))
#define CCI_CPUPLL_EN_DIV2                                      CCI_CPUPLL_EN_DIV2
#define CCI_CPUPLL_EN_DIV2_POS                                  (25U)
#define CCI_CPUPLL_EN_DIV2_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV2_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV2_LEN)-1)<<CCI_CPUPLL_EN_DIV2_POS)
#define CCI_CPUPLL_EN_DIV2_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV2_LEN)-1)<<CCI_CPUPLL_EN_DIV2_POS))
#define CCI_CPUPLL_EN_DIV1                                      CCI_CPUPLL_EN_DIV1
#define CCI_CPUPLL_EN_DIV1_POS                                  (26U)
#define CCI_CPUPLL_EN_DIV1_LEN                                  (1U)
#define CCI_CPUPLL_EN_DIV1_MSK                                  (((1U<<CCI_CPUPLL_EN_DIV1_LEN)-1)<<CCI_CPUPLL_EN_DIV1_POS)
#define CCI_CPUPLL_EN_DIV1_UMSK                                 (~(((1U<<CCI_CPUPLL_EN_DIV1_LEN)-1)<<CCI_CPUPLL_EN_DIV1_POS))

/* 0x7E8 : test control */
#define CCI_CPUPLL_RESV_OFFSET                                  (0x7E8)
#define CCI_CPUPLL_RESV0                                        CCI_CPUPLL_RESV0
#define CCI_CPUPLL_RESV0_POS                                    (0U)
#define CCI_CPUPLL_RESV0_LEN                                    (8U)
#define CCI_CPUPLL_RESV0_MSK                                    (((1U<<CCI_CPUPLL_RESV0_LEN)-1)<<CCI_CPUPLL_RESV0_POS)
#define CCI_CPUPLL_RESV0_UMSK                                   (~(((1U<<CCI_CPUPLL_RESV0_LEN)-1)<<CCI_CPUPLL_RESV0_POS))
#define CCI_CPUPLL_RESV1                                        CCI_CPUPLL_RESV1
#define CCI_CPUPLL_RESV1_POS                                    (8U)
#define CCI_CPUPLL_RESV1_LEN                                    (8U)
#define CCI_CPUPLL_RESV1_MSK                                    (((1U<<CCI_CPUPLL_RESV1_LEN)-1)<<CCI_CPUPLL_RESV1_POS)
#define CCI_CPUPLL_RESV1_UMSK                                   (~(((1U<<CCI_CPUPLL_RESV1_LEN)-1)<<CCI_CPUPLL_RESV1_POS))
#define CCI_CPUPLL_VCO_FREQ_RECORD                              CCI_CPUPLL_VCO_FREQ_RECORD
#define CCI_CPUPLL_VCO_FREQ_RECORD_POS                          (16U)
#define CCI_CPUPLL_VCO_FREQ_RECORD_LEN                          (16U)
#define CCI_CPUPLL_VCO_FREQ_RECORD_MSK                          (((1U<<CCI_CPUPLL_VCO_FREQ_RECORD_LEN)-1)<<CCI_CPUPLL_VCO_FREQ_RECORD_POS)
#define CCI_CPUPLL_VCO_FREQ_RECORD_UMSK                         (~(((1U<<CCI_CPUPLL_VCO_FREQ_RECORD_LEN)-1)<<CCI_CPUPLL_VCO_FREQ_RECORD_POS))

/* 0x7EC : cpupll_test */
#define CCI_CPUPLL_TEST_OFFSET                                  (0x7EC)
#define CCI_CPUPLL_TEST_DATA_MUX                                CCI_CPUPLL_TEST_DATA_MUX
#define CCI_CPUPLL_TEST_DATA_MUX_POS                            (0U)
#define CCI_CPUPLL_TEST_DATA_MUX_LEN                            (1U)
#define CCI_CPUPLL_TEST_DATA_MUX_MSK                            (((1U<<CCI_CPUPLL_TEST_DATA_MUX_LEN)-1)<<CCI_CPUPLL_TEST_DATA_MUX_POS)
#define CCI_CPUPLL_TEST_DATA_MUX_UMSK                           (~(((1U<<CCI_CPUPLL_TEST_DATA_MUX_LEN)-1)<<CCI_CPUPLL_TEST_DATA_MUX_POS))
#define CCI_CPUPLL_TEST_START_MUX                               CCI_CPUPLL_TEST_START_MUX
#define CCI_CPUPLL_TEST_START_MUX_POS                           (4U)
#define CCI_CPUPLL_TEST_START_MUX_LEN                           (1U)
#define CCI_CPUPLL_TEST_START_MUX_MSK                           (((1U<<CCI_CPUPLL_TEST_START_MUX_LEN)-1)<<CCI_CPUPLL_TEST_START_MUX_POS)
#define CCI_CPUPLL_TEST_START_MUX_UMSK                          (~(((1U<<CCI_CPUPLL_TEST_START_MUX_LEN)-1)<<CCI_CPUPLL_TEST_START_MUX_POS))
#define CCI_CPUPLL_TEST_EN                                      CCI_CPUPLL_TEST_EN
#define CCI_CPUPLL_TEST_EN_POS                                  (8U)
#define CCI_CPUPLL_TEST_EN_LEN                                  (1U)
#define CCI_CPUPLL_TEST_EN_MSK                                  (((1U<<CCI_CPUPLL_TEST_EN_LEN)-1)<<CCI_CPUPLL_TEST_EN_POS)
#define CCI_CPUPLL_TEST_EN_UMSK                                 (~(((1U<<CCI_CPUPLL_TEST_EN_LEN)-1)<<CCI_CPUPLL_TEST_EN_POS))
#define CCI_DTEN_CPUPLL_REFCLK                                  CCI_DTEN_CPUPLL_REFCLK
#define CCI_DTEN_CPUPLL_REFCLK_POS                              (10U)
#define CCI_DTEN_CPUPLL_REFCLK_LEN                              (1U)
#define CCI_DTEN_CPUPLL_REFCLK_MSK                              (((1U<<CCI_DTEN_CPUPLL_REFCLK_LEN)-1)<<CCI_DTEN_CPUPLL_REFCLK_POS)
#define CCI_DTEN_CPUPLL_REFCLK_UMSK                             (~(((1U<<CCI_DTEN_CPUPLL_REFCLK_LEN)-1)<<CCI_DTEN_CPUPLL_REFCLK_POS))
#define CCI_DTEN_CPUPLL_POSTDIV                                 CCI_DTEN_CPUPLL_POSTDIV
#define CCI_DTEN_CPUPLL_POSTDIV_POS                             (12U)
#define CCI_DTEN_CPUPLL_POSTDIV_LEN                             (1U)
#define CCI_DTEN_CPUPLL_POSTDIV_MSK                             (((1U<<CCI_DTEN_CPUPLL_POSTDIV_LEN)-1)<<CCI_DTEN_CPUPLL_POSTDIV_POS)
#define CCI_DTEN_CPUPLL_POSTDIV_UMSK                            (~(((1U<<CCI_DTEN_CPUPLL_POSTDIV_LEN)-1)<<CCI_DTEN_CPUPLL_POSTDIV_POS))
#define CCI_DTEN_CPUPLL_FBDIV_FSDM                              CCI_DTEN_CPUPLL_FBDIV_FSDM
#define CCI_DTEN_CPUPLL_FBDIV_FSDM_POS                          (16U)
#define CCI_DTEN_CPUPLL_FBDIV_FSDM_LEN                          (1U)
#define CCI_DTEN_CPUPLL_FBDIV_FSDM_MSK                          (((1U<<CCI_DTEN_CPUPLL_FBDIV_FSDM_LEN)-1)<<CCI_DTEN_CPUPLL_FBDIV_FSDM_POS)
#define CCI_DTEN_CPUPLL_FBDIV_FSDM_UMSK                         (~(((1U<<CCI_DTEN_CPUPLL_FBDIV_FSDM_LEN)-1)<<CCI_DTEN_CPUPLL_FBDIV_FSDM_POS))
#define CCI_DTEN_CPUPLL_FBDIV_FFB                               CCI_DTEN_CPUPLL_FBDIV_FFB
#define CCI_DTEN_CPUPLL_FBDIV_FFB_POS                           (20U)
#define CCI_DTEN_CPUPLL_FBDIV_FFB_LEN                           (1U)
#define CCI_DTEN_CPUPLL_FBDIV_FFB_MSK                           (((1U<<CCI_DTEN_CPUPLL_FBDIV_FFB_LEN)-1)<<CCI_DTEN_CPUPLL_FBDIV_FFB_POS)
#define CCI_DTEN_CPUPLL_FBDIV_FFB_UMSK                          (~(((1U<<CCI_DTEN_CPUPLL_FBDIV_FFB_LEN)-1)<<CCI_DTEN_CPUPLL_FBDIV_FFB_POS))
#define CCI_DTEN_CPUPLL_CLK32M                                  CCI_DTEN_CPUPLL_CLK32M
#define CCI_DTEN_CPUPLL_CLK32M_POS                              (24U)
#define CCI_DTEN_CPUPLL_CLK32M_LEN                              (1U)
#define CCI_DTEN_CPUPLL_CLK32M_MSK                              (((1U<<CCI_DTEN_CPUPLL_CLK32M_LEN)-1)<<CCI_DTEN_CPUPLL_CLK32M_POS)
#define CCI_DTEN_CPUPLL_CLK32M_UMSK                             (~(((1U<<CCI_DTEN_CPUPLL_CLK32M_LEN)-1)<<CCI_DTEN_CPUPLL_CLK32M_POS))
#define CCI_TEN_CPUPLL                                          CCI_TEN_CPUPLL
#define CCI_TEN_CPUPLL_POS                                      (28U)
#define CCI_TEN_CPUPLL_LEN                                      (1U)
#define CCI_TEN_CPUPLL_MSK                                      (((1U<<CCI_TEN_CPUPLL_LEN)-1)<<CCI_TEN_CPUPLL_POS)
#define CCI_TEN_CPUPLL_UMSK                                     (~(((1U<<CCI_TEN_CPUPLL_LEN)-1)<<CCI_TEN_CPUPLL_POS))

/* 0x7F0 : cpupll_readback */
#define CCI_CPUPLL_READBACK_OFFSET                              (0x7F0)
#define CCI_CPUPLL_TEST_START                                   CCI_CPUPLL_TEST_START
#define CCI_CPUPLL_TEST_START_POS                               (9U)
#define CCI_CPUPLL_TEST_START_LEN                               (1U)
#define CCI_CPUPLL_TEST_START_MSK                               (((1U<<CCI_CPUPLL_TEST_START_LEN)-1)<<CCI_CPUPLL_TEST_START_POS)
#define CCI_CPUPLL_TEST_START_UMSK                              (~(((1U<<CCI_CPUPLL_TEST_START_LEN)-1)<<CCI_CPUPLL_TEST_START_POS))
#define CCI_CPUPLL_VCTRL_OUT_RANGE                              CCI_CPUPLL_VCTRL_OUT_RANGE
#define CCI_CPUPLL_VCTRL_OUT_RANGE_POS                          (12U)
#define CCI_CPUPLL_VCTRL_OUT_RANGE_LEN                          (1U)
#define CCI_CPUPLL_VCTRL_OUT_RANGE_MSK                          (((1U<<CCI_CPUPLL_VCTRL_OUT_RANGE_LEN)-1)<<CCI_CPUPLL_VCTRL_OUT_RANGE_POS)
#define CCI_CPUPLL_VCTRL_OUT_RANGE_UMSK                         (~(((1U<<CCI_CPUPLL_VCTRL_OUT_RANGE_LEN)-1)<<CCI_CPUPLL_VCTRL_OUT_RANGE_POS))
#define CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU                      CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU
#define CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_POS                  (16U)
#define CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN                  (2U)
#define CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_MSK                  (((1U<<CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN)-1)<<CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_POS)
#define CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_UMSK                 (~(((1U<<CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_LEN)-1)<<CCI_CPUPLL_SOC_MOM_UPDATE_TOTAL_OU_POS))
#define CCI_CPUPLL_LO_UNLOCK                                    CCI_CPUPLL_LO_UNLOCK
#define CCI_CPUPLL_LO_UNLOCK_POS                                (20U)
#define CCI_CPUPLL_LO_UNLOCK_LEN                                (1U)
#define CCI_CPUPLL_LO_UNLOCK_MSK                                (((1U<<CCI_CPUPLL_LO_UNLOCK_LEN)-1)<<CCI_CPUPLL_LO_UNLOCK_POS)
#define CCI_CPUPLL_LO_UNLOCK_UMSK                               (~(((1U<<CCI_CPUPLL_LO_UNLOCK_LEN)-1)<<CCI_CPUPLL_LO_UNLOCK_POS))
#define CCI_CPUPLL_LO_LOCK                                      CCI_CPUPLL_LO_LOCK
#define CCI_CPUPLL_LO_LOCK_POS                                  (24U)
#define CCI_CPUPLL_LO_LOCK_LEN                                  (1U)
#define CCI_CPUPLL_LO_LOCK_MSK                                  (((1U<<CCI_CPUPLL_LO_LOCK_LEN)-1)<<CCI_CPUPLL_LO_LOCK_POS)
#define CCI_CPUPLL_LO_LOCK_UMSK                                 (~(((1U<<CCI_CPUPLL_LO_LOCK_LEN)-1)<<CCI_CPUPLL_LO_LOCK_POS))
#define CCI_CPUPLL_FAST_INC_FCAL_OVFLOW                         CCI_CPUPLL_FAST_INC_FCAL_OVFLOW
#define CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_POS                     (28U)
#define CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_LEN                     (1U)
#define CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_MSK                     (((1U<<CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_LEN)-1)<<CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_POS)
#define CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_UMSK                    (~(((1U<<CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_LEN)-1)<<CCI_CPUPLL_FAST_INC_FCAL_OVFLOW_POS))

/* 0x7F4 : cpupll_adpll_test_out */
#define CCI_CPUPLL_ADPLL_TEST_OUT_OFFSET                        (0x7F4)
#define CCI_CPUPLL_TEST_OUT                                     CCI_CPUPLL_TEST_OUT
#define CCI_CPUPLL_TEST_OUT_POS                                 (0U)
#define CCI_CPUPLL_TEST_OUT_LEN                                 (32U)
#define CCI_CPUPLL_TEST_OUT_MSK                                 (((1U<<CCI_CPUPLL_TEST_OUT_LEN)-1)<<CCI_CPUPLL_TEST_OUT_POS)
#define CCI_CPUPLL_TEST_OUT_UMSK                                (~(((1U<<CCI_CPUPLL_TEST_OUT_LEN)-1)<<CCI_CPUPLL_TEST_OUT_POS))

/* 0x7F8 : cpupll_ssc */
#define CCI_CPUPLL_SSC_OFFSET                                   (0x7F8)
#define CCI_CPUPLL_SSC_START_GATE_EN                            CCI_CPUPLL_SSC_START_GATE_EN
#define CCI_CPUPLL_SSC_START_GATE_EN_POS                        (0U)
#define CCI_CPUPLL_SSC_START_GATE_EN_LEN                        (1U)
#define CCI_CPUPLL_SSC_START_GATE_EN_MSK                        (((1U<<CCI_CPUPLL_SSC_START_GATE_EN_LEN)-1)<<CCI_CPUPLL_SSC_START_GATE_EN_POS)
#define CCI_CPUPLL_SSC_START_GATE_EN_UMSK                       (~(((1U<<CCI_CPUPLL_SSC_START_GATE_EN_LEN)-1)<<CCI_CPUPLL_SSC_START_GATE_EN_POS))
#define CCI_CPUPLL_SSC_GAIN                                     CCI_CPUPLL_SSC_GAIN
#define CCI_CPUPLL_SSC_GAIN_POS                                 (4U)
#define CCI_CPUPLL_SSC_GAIN_LEN                                 (3U)
#define CCI_CPUPLL_SSC_GAIN_MSK                                 (((1U<<CCI_CPUPLL_SSC_GAIN_LEN)-1)<<CCI_CPUPLL_SSC_GAIN_POS)
#define CCI_CPUPLL_SSC_GAIN_UMSK                                (~(((1U<<CCI_CPUPLL_SSC_GAIN_LEN)-1)<<CCI_CPUPLL_SSC_GAIN_POS))
#define CCI_CPUPLL_SSC_CNT                                      CCI_CPUPLL_SSC_CNT
#define CCI_CPUPLL_SSC_CNT_POS                                  (8U)
#define CCI_CPUPLL_SSC_CNT_LEN                                  (8U)
#define CCI_CPUPLL_SSC_CNT_MSK                                  (((1U<<CCI_CPUPLL_SSC_CNT_LEN)-1)<<CCI_CPUPLL_SSC_CNT_POS)
#define CCI_CPUPLL_SSC_CNT_UMSK                                 (~(((1U<<CCI_CPUPLL_SSC_CNT_LEN)-1)<<CCI_CPUPLL_SSC_CNT_POS))
#define CCI_CPUPLL_SSC_EN                                       CCI_CPUPLL_SSC_EN
#define CCI_CPUPLL_SSC_EN_POS                                   (16U)
#define CCI_CPUPLL_SSC_EN_LEN                                   (1U)
#define CCI_CPUPLL_SSC_EN_MSK                                   (((1U<<CCI_CPUPLL_SSC_EN_LEN)-1)<<CCI_CPUPLL_SSC_EN_POS)
#define CCI_CPUPLL_SSC_EN_UMSK                                  (~(((1U<<CCI_CPUPLL_SSC_EN_LEN)-1)<<CCI_CPUPLL_SSC_EN_POS))

/* 0x7FC : cpupll_clktree_rf */
#define CCI_CPUPLL_CLKTREE_RF_OFFSET                            (0x7FC)
#define CCI_CPUPLL_POSTDIV_EN                                   CCI_CPUPLL_POSTDIV_EN
#define CCI_CPUPLL_POSTDIV_EN_POS                               (23U)
#define CCI_CPUPLL_POSTDIV_EN_LEN                               (1U)
#define CCI_CPUPLL_POSTDIV_EN_MSK                               (((1U<<CCI_CPUPLL_POSTDIV_EN_LEN)-1)<<CCI_CPUPLL_POSTDIV_EN_POS)
#define CCI_CPUPLL_POSTDIV_EN_UMSK                              (~(((1U<<CCI_CPUPLL_POSTDIV_EN_LEN)-1)<<CCI_CPUPLL_POSTDIV_EN_POS))
#define CCI_CPUPLL_POSTDIV                                      CCI_CPUPLL_POSTDIV
#define CCI_CPUPLL_POSTDIV_POS                                  (24U)
#define CCI_CPUPLL_POSTDIV_LEN                                  (8U)
#define CCI_CPUPLL_POSTDIV_MSK                                  (((1U<<CCI_CPUPLL_POSTDIV_LEN)-1)<<CCI_CPUPLL_POSTDIV_POS)
#define CCI_CPUPLL_POSTDIV_UMSK                                 (~(((1U<<CCI_CPUPLL_POSTDIV_LEN)-1)<<CCI_CPUPLL_POSTDIV_POS))


struct  cci_reg {
    /* 0x0 : cci_cfg */
    union {
        struct {
            uint32_t cci_en                         :  1; /* [    0],        r/w,        0x1 */
            uint32_t cci_slv_sel_cci2               :  1; /* [    1],        r/w,        0x0 */
            uint32_t cci_mas_sel_cci2               :  1; /* [    2],        r/w,        0x0 */
            uint32_t cci_mas_hw_mode                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_m_cci_sclk_en              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_div_m_cci_sclk             :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t cfg_cci1_pre_read              :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_scci_clk_inv               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_mcci_clk_inv               :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cfg_mcci_dly_r                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_cfg;

    /* 0x4 : cci_addr */
    union {
        struct {
            uint32_t apb_cci_addr                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_addr;

    /* 0x8 : cci_wdata */
    union {
        struct {
            uint32_t apb_cci_wdata                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_wdata;

    /* 0xC : cci_rdata */
    union {
        struct {
            uint32_t apb_cci_rdata                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_rdata;

    /* 0x10 : cci_ctl */
    union {
        struct {
            uint32_t cci_write_flag                 :  1; /* [    0],          r,        0x0 */
            uint32_t cci_read_flag                  :  1; /* [    1],          r,        0x0 */
            uint32_t ahb_state                      :  2; /* [ 3: 2],          r,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_ctl;

    /* 0x14  reserved */
    uint8_t RESERVED0x14[188];

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
            uint32_t wifipll_vco_rsht_sel_hp        :  2; /* [19:18],        r/w,        0x0 */
            uint32_t wifipll_refclk_div_ratio       :  4; /* [23:20],        r/w,        0x2 */
            uint32_t wifipll_rstb                   :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t pu_wifipll                     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29                    :  1; /* [   29],       rsvd,        0x0 */
            uint32_t wifipll_vco_rsht_sel_lp        :  2; /* [31:30],        r/w,        0x0 */
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
            uint32_t wifipll_vco_rsht_sel_hw        :  2; /* [11:10],          r,        0x0 */
            uint32_t reserved_12                    :  1; /* [   12],       rsvd,        0x0 */
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
            uint32_t wifipll_vco_rsht_en_tx_hp      :  1; /* [   30],        r/w,        0x1 */
            uint32_t wifipll_vco_rsht_en_tx_lp      :  1; /* [   31],        r/w,        0x1 */
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

    /* 0x100  reserved */
    uint8_t RESERVED0x100[1744];

    /* 0x7D0 : start-up control */
    union {
        struct {
            uint32_t cpupll_fbdiv_sel_sample_clk    :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t cpupll_fbdiv_sel_adpll_clk     :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t cpupll_dtc_r_sel               :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t cpupll_dtc_bypass_en           :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t cpupll_vco_speed               :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t cpupll_vco_ldo_bypass          :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cpupll_refclk_sel              :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t cpupll_refclk_div_ratio        :  4; /* [23:20],        r/w,        0x4 */
            uint32_t cpupll_rstb                    :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t pu_cpupll                      :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_ana_ctrl;

    /* 0x7D4 : cpupll_spd_fcal */
    union {
        struct {
            uint32_t cpupll_fcal_threshold          :  1; /* [    0],        r/w,        0x1 */
            uint32_t cpupll_fcal_phaerr_limit       :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t cpupll_fcal_ndiv_adj_en        :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cpupll_fcal_momcap_ini_ext     :  7; /* [14: 8],        r/w,       0x40 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t cpupll_fcal_mode               :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t cpupll_fast_inc_dvar_comp_en   :  1; /* [   20],        r/w,        0x1 */
            uint32_t cpupll_fast_inc_fcal_en        :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t cpupll_inc_fcal_en             :  1; /* [   23],        r/w,        0x0 */
            uint32_t cpupll_spd_unlock_dly          :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25                    :  1; /* [   25],       rsvd,        0x0 */
            uint32_t cpupll_spd_gain                :  2; /* [27:26],        r/w,        0x1 */
            uint32_t cpupll_coarse_path_force_on    :  1; /* [   28],        r/w,        0x0 */
            uint32_t cpupll_coarse_gain             :  2; /* [30:29],        r/w,        0x2 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_spd_fcal;

    /* 0x7D8 : cpupll_lf_vctrl */
    union {
        struct {
            uint32_t cpupll_mom_update_period       :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t cpupll_mom_force_hold          :  1; /* [    3],        r/w,        0x0 */
            uint32_t cpupll_mom_code_bypass         :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5                     :  1; /* [    5],       rsvd,        0x0 */
            uint32_t cpupll_dcomash_bypass          :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t cpupll_lo_open                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cpupll_force_lf_fast_mode      :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t cpupll_lf_lsb_ext              :  6; /* [17:12],        r/w,       0x20 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t cpupll_lf_beta_fast            :  1; /* [   20],        r/w,        0x1 */
            uint32_t cpupll_lf_beta_exp             :  3; /* [23:21],        r/w,        0x2 */
            uint32_t cpupll_lf_beta_base            :  2; /* [25:24],        r/w,        0x3 */
            uint32_t cpupll_lf_alpha_fast           :  2; /* [27:26],        r/w,        0x3 */
            uint32_t cpupll_lf_alpha_exp            :  3; /* [30:28],        r/w,        0x3 */
            uint32_t cpupll_lf_alpha_base           :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_lf_vctrl;

    /* 0x7DC : sdm control */
    union {
        struct {
            uint32_t cpupll_sdm_in                  : 19; /* [18: 0],        r/w,    0x18000 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_sdm1;

    /* 0x7E0 : cpupll_sdm_lms */
    union {
        struct {
            uint32_t cpupll_lms_ext_value           :  6; /* [ 5: 0],        r/w,       0x2b */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t cpupll_lms_ext_en              :  1; /* [    7],        r/w,        0x0 */
            uint32_t cpupll_lms_polarity            :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t cpupll_pha_cancel_en           :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t cpupll_sdm_regrst_en           :  1; /* [   15],        r/w,        0x0 */
            uint32_t cpupll_sdm_out_dly             :  1; /* [   16],        r/w,        0x0 */
            uint32_t cpupll_sdm_order               :  1; /* [   17],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t cpupll_sdm_sig_dith            :  2; /* [21:20],        r/w,        0x2 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t cpupll_sdm_dith_force_en       :  1; /* [   23],        r/w,        0x1 */
            uint32_t cpupll_sdm_bypass              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_sdm_lms;

    /* 0x7E4 : cpupll_clktree_dig */
    union {
        struct {
            uint32_t cpupll_dl_ctrl_div30           :  1; /* [    0],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div20           :  1; /* [    1],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div12           :  1; /* [    2],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div10           :  1; /* [    3],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div8            :  1; /* [    4],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div6            :  1; /* [    5],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div5            :  1; /* [    6],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div4            :  1; /* [    7],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div3            :  1; /* [    8],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div2            :  1; /* [    9],        r/w,        0x0 */
            uint32_t cpupll_dl_ctrl_div1            :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t cpupll_en_div30                :  1; /* [   16],        r/w,        0x1 */
            uint32_t cpupll_en_div20                :  1; /* [   17],        r/w,        0x1 */
            uint32_t cpupll_en_div12                :  1; /* [   18],        r/w,        0x1 */
            uint32_t cpupll_en_div10                :  1; /* [   19],        r/w,        0x1 */
            uint32_t cpupll_en_div8                 :  1; /* [   20],        r/w,        0x1 */
            uint32_t cpupll_en_div6                 :  1; /* [   21],        r/w,        0x1 */
            uint32_t cpupll_en_div5                 :  1; /* [   22],        r/w,        0x1 */
            uint32_t cpupll_en_div4                 :  1; /* [   23],        r/w,        0x1 */
            uint32_t cpupll_en_div3                 :  1; /* [   24],        r/w,        0x1 */
            uint32_t cpupll_en_div2                 :  1; /* [   25],        r/w,        0x1 */
            uint32_t cpupll_en_div1                 :  1; /* [   26],        r/w,        0x1 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_clktree_dig;

    /* 0x7E8 : test control */
    union {
        struct {
            uint32_t cpupll_resv0                   :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t cpupll_resv1                   :  8; /* [15: 8],        r/w,       0xff */
            uint32_t cpupll_vco_freq_record         : 16; /* [31:16],        r/w,      0x3c0 */
        }BF;
        uint32_t WORD;
    } cpupll_resv;

    /* 0x7EC : cpupll_test */
    union {
        struct {
            uint32_t cpupll_test_data_mux           :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t cpupll_test_start_mux          :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t cpupll_test_en                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t dten_cpupll_refclk             :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t dten_cpupll_postdiv            :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t dten_cpupll_fbdiv_fsdm         :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t dten_cpupll_fbdiv_ffb          :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t dten_cpupll_clk32m             :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t ten_cpupll                     :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_test;

    /* 0x7F0 : cpupll_readback */
    union {
        struct {
            uint32_t reserved_0_8                   :  9; /* [ 8: 0],       rsvd,        0x0 */
            uint32_t cpupll_test_start              :  1; /* [    9],          r,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t cpupll_vctrl_out_range         :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cpupll_soc_mom_update_total_ou :  2; /* [17:16],          r,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t cpupll_lo_unlock               :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t cpupll_lo_lock                 :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t cpupll_fast_inc_fcal_ovflow    :  1; /* [   28],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_readback;

    /* 0x7F4 : cpupll_adpll_test_out */
    union {
        struct {
            uint32_t cpupll_test_out                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_adpll_test_out;

    /* 0x7F8 : cpupll_ssc */
    union {
        struct {
            uint32_t cpupll_ssc_start_gate_en       :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t cpupll_ssc_gain                :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t cpupll_ssc_cnt                 :  8; /* [15: 8],        r/w,       0x32 */
            uint32_t cpupll_ssc_en                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpupll_ssc;

    /* 0x7FC : cpupll_clktree_rf */
    union {
        struct {
            uint32_t reserved_0_22                  : 23; /* [22: 0],       rsvd,        0x0 */
            uint32_t cpupll_postdiv_en              :  1; /* [   23],        r/w,        0x1 */
            uint32_t cpupll_postdiv                 :  8; /* [31:24],        r/w,       0x9c */
        }BF;
        uint32_t WORD;
    } cpupll_clktree_rf;

};

typedef volatile struct cci_reg cci_reg_t;


#endif  /* __CCI_REG_H__ */
