/**
  ******************************************************************************
  * @file    aon_reg.h
  * @version V1.0
  * @date    2022-06-29
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
#ifndef  __AON_REG_H__
#define  __AON_REG_H__

#include "bl702l.h"

/* 0x800 : aon */
#define AON_OFFSET                                              (0x800)
#define AON_RESV                                                AON_RESV
#define AON_RESV_POS                                            (0U)
#define AON_RESV_LEN                                            (8U)
#define AON_RESV_MSK                                            (((1U<<AON_RESV_LEN)-1)<<AON_RESV_POS)
#define AON_RESV_UMSK                                           (~(((1U<<AON_RESV_LEN)-1)<<AON_RESV_POS))
#define AON_PU_AON_DC_TBUF                                      AON_PU_AON_DC_TBUF
#define AON_PU_AON_DC_TBUF_POS                                  (12U)
#define AON_PU_AON_DC_TBUF_LEN                                  (1U)
#define AON_PU_AON_DC_TBUF_MSK                                  (((1U<<AON_PU_AON_DC_TBUF_LEN)-1)<<AON_PU_AON_DC_TBUF_POS)
#define AON_PU_AON_DC_TBUF_UMSK                                 (~(((1U<<AON_PU_AON_DC_TBUF_LEN)-1)<<AON_PU_AON_DC_TBUF_POS))
#define AON_LDO11_RT_PULLDOWN                                   AON_LDO11_RT_PULLDOWN
#define AON_LDO11_RT_PULLDOWN_POS                               (20U)
#define AON_LDO11_RT_PULLDOWN_LEN                               (1U)
#define AON_LDO11_RT_PULLDOWN_MSK                               (((1U<<AON_LDO11_RT_PULLDOWN_LEN)-1)<<AON_LDO11_RT_PULLDOWN_POS)
#define AON_LDO11_RT_PULLDOWN_UMSK                              (~(((1U<<AON_LDO11_RT_PULLDOWN_LEN)-1)<<AON_LDO11_RT_PULLDOWN_POS))
#define AON_LDO11_RT_PULLDOWN_SEL                               AON_LDO11_RT_PULLDOWN_SEL
#define AON_LDO11_RT_PULLDOWN_SEL_POS                           (21U)
#define AON_LDO11_RT_PULLDOWN_SEL_LEN                           (1U)
#define AON_LDO11_RT_PULLDOWN_SEL_MSK                           (((1U<<AON_LDO11_RT_PULLDOWN_SEL_LEN)-1)<<AON_LDO11_RT_PULLDOWN_SEL_POS)
#define AON_LDO11_RT_PULLDOWN_SEL_UMSK                          (~(((1U<<AON_LDO11_RT_PULLDOWN_SEL_LEN)-1)<<AON_LDO11_RT_PULLDOWN_SEL_POS))
#define AON_SW_PU_LDO11_RT                                      AON_SW_PU_LDO11_RT
#define AON_SW_PU_LDO11_RT_POS                                  (22U)
#define AON_SW_PU_LDO11_RT_LEN                                  (1U)
#define AON_SW_PU_LDO11_RT_MSK                                  (((1U<<AON_SW_PU_LDO11_RT_LEN)-1)<<AON_SW_PU_LDO11_RT_POS)
#define AON_SW_PU_LDO11_RT_UMSK                                 (~(((1U<<AON_SW_PU_LDO11_RT_LEN)-1)<<AON_SW_PU_LDO11_RT_POS))

/* 0x804 : aon_common */
#define AON_COMMON_OFFSET                                       (0x804)
#define AON_TMUX_AON                                            AON_TMUX_AON
#define AON_TMUX_AON_POS                                        (0U)
#define AON_TMUX_AON_LEN                                        (3U)
#define AON_TMUX_AON_MSK                                        (((1U<<AON_TMUX_AON_LEN)-1)<<AON_TMUX_AON_POS)
#define AON_TMUX_AON_UMSK                                       (~(((1U<<AON_TMUX_AON_LEN)-1)<<AON_TMUX_AON_POS))
#define AON_TEN_AON                                             AON_TEN_AON
#define AON_TEN_AON_POS                                         (4U)
#define AON_TEN_AON_LEN                                         (1U)
#define AON_TEN_AON_MSK                                         (((1U<<AON_TEN_AON_LEN)-1)<<AON_TEN_AON_POS)
#define AON_TEN_AON_UMSK                                        (~(((1U<<AON_TEN_AON_LEN)-1)<<AON_TEN_AON_POS))
#define AON_DTEN_XTAL32K                                        AON_DTEN_XTAL32K
#define AON_DTEN_XTAL32K_POS                                    (5U)
#define AON_DTEN_XTAL32K_LEN                                    (1U)
#define AON_DTEN_XTAL32K_MSK                                    (((1U<<AON_DTEN_XTAL32K_LEN)-1)<<AON_DTEN_XTAL32K_POS)
#define AON_DTEN_XTAL32K_UMSK                                   (~(((1U<<AON_DTEN_XTAL32K_LEN)-1)<<AON_DTEN_XTAL32K_POS))
#define AON_TEN_XTAL32K                                         AON_TEN_XTAL32K
#define AON_TEN_XTAL32K_POS                                     (6U)
#define AON_TEN_XTAL32K_LEN                                     (1U)
#define AON_TEN_XTAL32K_MSK                                     (((1U<<AON_TEN_XTAL32K_LEN)-1)<<AON_TEN_XTAL32K_POS)
#define AON_TEN_XTAL32K_UMSK                                    (~(((1U<<AON_TEN_XTAL32K_LEN)-1)<<AON_TEN_XTAL32K_POS))
#define AON_PMIP_DC_TP_OUT_EN_AON                               AON_PMIP_DC_TP_OUT_EN_AON
#define AON_PMIP_DC_TP_OUT_EN_AON_POS                           (7U)
#define AON_PMIP_DC_TP_OUT_EN_AON_LEN                           (1U)
#define AON_PMIP_DC_TP_OUT_EN_AON_MSK                           (((1U<<AON_PMIP_DC_TP_OUT_EN_AON_LEN)-1)<<AON_PMIP_DC_TP_OUT_EN_AON_POS)
#define AON_PMIP_DC_TP_OUT_EN_AON_UMSK                          (~(((1U<<AON_PMIP_DC_TP_OUT_EN_AON_LEN)-1)<<AON_PMIP_DC_TP_OUT_EN_AON_POS))
#define AON_TEN_LDO11SOC_AON                                    AON_TEN_LDO11SOC_AON
#define AON_TEN_LDO11SOC_AON_POS                                (9U)
#define AON_TEN_LDO11SOC_AON_LEN                                (1U)
#define AON_TEN_LDO11SOC_AON_MSK                                (((1U<<AON_TEN_LDO11SOC_AON_LEN)-1)<<AON_TEN_LDO11SOC_AON_POS)
#define AON_TEN_LDO11SOC_AON_UMSK                               (~(((1U<<AON_TEN_LDO11SOC_AON_LEN)-1)<<AON_TEN_LDO11SOC_AON_POS))
#define AON_TEN_DCDC_0_AON                                      AON_TEN_DCDC_0_AON
#define AON_TEN_DCDC_0_AON_POS                                  (10U)
#define AON_TEN_DCDC_0_AON_LEN                                  (1U)
#define AON_TEN_DCDC_0_AON_MSK                                  (((1U<<AON_TEN_DCDC_0_AON_LEN)-1)<<AON_TEN_DCDC_0_AON_POS)
#define AON_TEN_DCDC_0_AON_UMSK                                 (~(((1U<<AON_TEN_DCDC_0_AON_LEN)-1)<<AON_TEN_DCDC_0_AON_POS))
#define AON_TEN_DCDC_1_AON                                      AON_TEN_DCDC_1_AON
#define AON_TEN_DCDC_1_AON_POS                                  (11U)
#define AON_TEN_DCDC_1_AON_LEN                                  (1U)
#define AON_TEN_DCDC_1_AON_MSK                                  (((1U<<AON_TEN_DCDC_1_AON_LEN)-1)<<AON_TEN_DCDC_1_AON_POS)
#define AON_TEN_DCDC_1_AON_UMSK                                 (~(((1U<<AON_TEN_DCDC_1_AON_LEN)-1)<<AON_TEN_DCDC_1_AON_POS))
#define AON_TEN_BG_SYS_AON                                      AON_TEN_BG_SYS_AON
#define AON_TEN_BG_SYS_AON_POS                                  (12U)
#define AON_TEN_BG_SYS_AON_LEN                                  (1U)
#define AON_TEN_BG_SYS_AON_MSK                                  (((1U<<AON_TEN_BG_SYS_AON_LEN)-1)<<AON_TEN_BG_SYS_AON_POS)
#define AON_TEN_BG_SYS_AON_UMSK                                 (~(((1U<<AON_TEN_BG_SYS_AON_LEN)-1)<<AON_TEN_BG_SYS_AON_POS))
#define AON_TEN_LDO14_AON                                       AON_TEN_LDO14_AON
#define AON_TEN_LDO14_AON_POS                                   (13U)
#define AON_TEN_LDO14_AON_LEN                                   (1U)
#define AON_TEN_LDO14_AON_MSK                                   (((1U<<AON_TEN_LDO14_AON_LEN)-1)<<AON_TEN_LDO14_AON_POS)
#define AON_TEN_LDO14_AON_UMSK                                  (~(((1U<<AON_TEN_LDO14_AON_LEN)-1)<<AON_TEN_LDO14_AON_POS))
#define AON_TEN_XTAL_HF_RC32M_AON                               AON_TEN_XTAL_HF_RC32M_AON
#define AON_TEN_XTAL_HF_RC32M_AON_POS                           (16U)
#define AON_TEN_XTAL_HF_RC32M_AON_LEN                           (1U)
#define AON_TEN_XTAL_HF_RC32M_AON_MSK                           (((1U<<AON_TEN_XTAL_HF_RC32M_AON_LEN)-1)<<AON_TEN_XTAL_HF_RC32M_AON_POS)
#define AON_TEN_XTAL_HF_RC32M_AON_UMSK                          (~(((1U<<AON_TEN_XTAL_HF_RC32M_AON_LEN)-1)<<AON_TEN_XTAL_HF_RC32M_AON_POS))
#define AON_TEN_XTAL_AON                                        AON_TEN_XTAL_AON
#define AON_TEN_XTAL_AON_POS                                    (17U)
#define AON_TEN_XTAL_AON_LEN                                    (1U)
#define AON_TEN_XTAL_AON_MSK                                    (((1U<<AON_TEN_XTAL_AON_LEN)-1)<<AON_TEN_XTAL_AON_POS)
#define AON_TEN_XTAL_AON_UMSK                                   (~(((1U<<AON_TEN_XTAL_AON_LEN)-1)<<AON_TEN_XTAL_AON_POS))
#define AON_DTEN_XTAL_AON                                       AON_DTEN_XTAL_AON
#define AON_DTEN_XTAL_AON_POS                                   (18U)
#define AON_DTEN_XTAL_AON_LEN                                   (1U)
#define AON_DTEN_XTAL_AON_MSK                                   (((1U<<AON_DTEN_XTAL_AON_LEN)-1)<<AON_DTEN_XTAL_AON_POS)
#define AON_DTEN_XTAL_AON_UMSK                                  (~(((1U<<AON_DTEN_XTAL_AON_LEN)-1)<<AON_DTEN_XTAL_AON_POS))
#define AON_AVDD14_RDY_AON                                      AON_AVDD14_RDY_AON
#define AON_AVDD14_RDY_AON_POS                                  (31U)
#define AON_AVDD14_RDY_AON_LEN                                  (1U)
#define AON_AVDD14_RDY_AON_MSK                                  (((1U<<AON_AVDD14_RDY_AON_LEN)-1)<<AON_AVDD14_RDY_AON_POS)
#define AON_AVDD14_RDY_AON_UMSK                                 (~(((1U<<AON_AVDD14_RDY_AON_LEN)-1)<<AON_AVDD14_RDY_AON_POS))

/* 0x808 : aon_misc */
#define AON_MISC_OFFSET                                         (0x808)
#define AON_SW_SOC_EN_AON                                       AON_SW_SOC_EN_AON
#define AON_SW_SOC_EN_AON_POS                                   (0U)
#define AON_SW_SOC_EN_AON_LEN                                   (1U)
#define AON_SW_SOC_EN_AON_MSK                                   (((1U<<AON_SW_SOC_EN_AON_LEN)-1)<<AON_SW_SOC_EN_AON_POS)
#define AON_SW_SOC_EN_AON_UMSK                                  (~(((1U<<AON_SW_SOC_EN_AON_LEN)-1)<<AON_SW_SOC_EN_AON_POS))

/* 0x810 : bg_sys_top */
#define AON_BG_SYS_TOP_OFFSET                                   (0x810)
#define AON_PMIP_RESV_AON                                       AON_PMIP_RESV_AON
#define AON_PMIP_RESV_AON_POS                                   (0U)
#define AON_PMIP_RESV_AON_LEN                                   (8U)
#define AON_PMIP_RESV_AON_MSK                                   (((1U<<AON_PMIP_RESV_AON_LEN)-1)<<AON_PMIP_RESV_AON_POS)
#define AON_PMIP_RESV_AON_UMSK                                  (~(((1U<<AON_PMIP_RESV_AON_LEN)-1)<<AON_PMIP_RESV_AON_POS))
#define AON_PU_BG_SYS_AON                                       AON_PU_BG_SYS_AON
#define AON_PU_BG_SYS_AON_POS                                   (8U)
#define AON_PU_BG_SYS_AON_LEN                                   (1U)
#define AON_PU_BG_SYS_AON_MSK                                   (((1U<<AON_PU_BG_SYS_AON_LEN)-1)<<AON_PU_BG_SYS_AON_POS)
#define AON_PU_BG_SYS_AON_UMSK                                  (~(((1U<<AON_PU_BG_SYS_AON_LEN)-1)<<AON_PU_BG_SYS_AON_POS))
#define AON_BG_SYS_START_CTRL_AON                               AON_BG_SYS_START_CTRL_AON
#define AON_BG_SYS_START_CTRL_AON_POS                           (12U)
#define AON_BG_SYS_START_CTRL_AON_LEN                           (1U)
#define AON_BG_SYS_START_CTRL_AON_MSK                           (((1U<<AON_BG_SYS_START_CTRL_AON_LEN)-1)<<AON_BG_SYS_START_CTRL_AON_POS)
#define AON_BG_SYS_START_CTRL_AON_UMSK                          (~(((1U<<AON_BG_SYS_START_CTRL_AON_LEN)-1)<<AON_BG_SYS_START_CTRL_AON_POS))

/* 0x814 : dcdc18_top_0 */
#define AON_DCDC18_TOP_0_OFFSET                                 (0x814)
#define AON_DCDC_VOUT_SEL_AON                                   AON_DCDC_VOUT_SEL_AON
#define AON_DCDC_VOUT_SEL_AON_POS                               (1U)
#define AON_DCDC_VOUT_SEL_AON_LEN                               (5U)
#define AON_DCDC_VOUT_SEL_AON_MSK                               (((1U<<AON_DCDC_VOUT_SEL_AON_LEN)-1)<<AON_DCDC_VOUT_SEL_AON_POS)
#define AON_DCDC_VOUT_SEL_AON_UMSK                              (~(((1U<<AON_DCDC_VOUT_SEL_AON_LEN)-1)<<AON_DCDC_VOUT_SEL_AON_POS))
#define AON_DCDC_VPFM_AON                                       AON_DCDC_VPFM_AON
#define AON_DCDC_VPFM_AON_POS                                   (8U)
#define AON_DCDC_VPFM_AON_LEN                                   (4U)
#define AON_DCDC_VPFM_AON_MSK                                   (((1U<<AON_DCDC_VPFM_AON_LEN)-1)<<AON_DCDC_VPFM_AON_POS)
#define AON_DCDC_VPFM_AON_UMSK                                  (~(((1U<<AON_DCDC_VPFM_AON_LEN)-1)<<AON_DCDC_VPFM_AON_POS))
#define AON_DCDC_OSC_2M_MODE_AON                                AON_DCDC_OSC_2M_MODE_AON
#define AON_DCDC_OSC_2M_MODE_AON_POS                            (12U)
#define AON_DCDC_OSC_2M_MODE_AON_LEN                            (1U)
#define AON_DCDC_OSC_2M_MODE_AON_MSK                            (((1U<<AON_DCDC_OSC_2M_MODE_AON_LEN)-1)<<AON_DCDC_OSC_2M_MODE_AON_POS)
#define AON_DCDC_OSC_2M_MODE_AON_UMSK                           (~(((1U<<AON_DCDC_OSC_2M_MODE_AON_LEN)-1)<<AON_DCDC_OSC_2M_MODE_AON_POS))
#define AON_DCDC_OSC_FREQ_TRIM_AON                              AON_DCDC_OSC_FREQ_TRIM_AON
#define AON_DCDC_OSC_FREQ_TRIM_AON_POS                          (16U)
#define AON_DCDC_OSC_FREQ_TRIM_AON_LEN                          (4U)
#define AON_DCDC_OSC_FREQ_TRIM_AON_MSK                          (((1U<<AON_DCDC_OSC_FREQ_TRIM_AON_LEN)-1)<<AON_DCDC_OSC_FREQ_TRIM_AON_POS)
#define AON_DCDC_OSC_FREQ_TRIM_AON_UMSK                         (~(((1U<<AON_DCDC_OSC_FREQ_TRIM_AON_LEN)-1)<<AON_DCDC_OSC_FREQ_TRIM_AON_POS))
#define AON_DCDC_SLOPE_CURR_SEL_AON                             AON_DCDC_SLOPE_CURR_SEL_AON
#define AON_DCDC_SLOPE_CURR_SEL_AON_POS                         (20U)
#define AON_DCDC_SLOPE_CURR_SEL_AON_LEN                         (5U)
#define AON_DCDC_SLOPE_CURR_SEL_AON_MSK                         (((1U<<AON_DCDC_SLOPE_CURR_SEL_AON_LEN)-1)<<AON_DCDC_SLOPE_CURR_SEL_AON_POS)
#define AON_DCDC_SLOPE_CURR_SEL_AON_UMSK                        (~(((1U<<AON_DCDC_SLOPE_CURR_SEL_AON_LEN)-1)<<AON_DCDC_SLOPE_CURR_SEL_AON_POS))
#define AON_DCDC_EN_STOP_OSC_AON                                AON_DCDC_EN_STOP_OSC_AON
#define AON_DCDC_EN_STOP_OSC_AON_POS                            (25U)
#define AON_DCDC_EN_STOP_OSC_AON_LEN                            (1U)
#define AON_DCDC_EN_STOP_OSC_AON_MSK                            (((1U<<AON_DCDC_EN_STOP_OSC_AON_LEN)-1)<<AON_DCDC_EN_STOP_OSC_AON_POS)
#define AON_DCDC_EN_STOP_OSC_AON_UMSK                           (~(((1U<<AON_DCDC_EN_STOP_OSC_AON_LEN)-1)<<AON_DCDC_EN_STOP_OSC_AON_POS))
#define AON_DCDC_EN_SLOW_OSC_AON                                AON_DCDC_EN_SLOW_OSC_AON
#define AON_DCDC_EN_SLOW_OSC_AON_POS                            (26U)
#define AON_DCDC_EN_SLOW_OSC_AON_LEN                            (1U)
#define AON_DCDC_EN_SLOW_OSC_AON_MSK                            (((1U<<AON_DCDC_EN_SLOW_OSC_AON_LEN)-1)<<AON_DCDC_EN_SLOW_OSC_AON_POS)
#define AON_DCDC_EN_SLOW_OSC_AON_UMSK                           (~(((1U<<AON_DCDC_EN_SLOW_OSC_AON_LEN)-1)<<AON_DCDC_EN_SLOW_OSC_AON_POS))
#define AON_DCDC_OSC_INHIBIT_T2_AON                             AON_DCDC_OSC_INHIBIT_T2_AON
#define AON_DCDC_OSC_INHIBIT_T2_AON_POS                         (27U)
#define AON_DCDC_OSC_INHIBIT_T2_AON_LEN                         (1U)
#define AON_DCDC_OSC_INHIBIT_T2_AON_MSK                         (((1U<<AON_DCDC_OSC_INHIBIT_T2_AON_LEN)-1)<<AON_DCDC_OSC_INHIBIT_T2_AON_POS)
#define AON_DCDC_OSC_INHIBIT_T2_AON_UMSK                        (~(((1U<<AON_DCDC_OSC_INHIBIT_T2_AON_LEN)-1)<<AON_DCDC_OSC_INHIBIT_T2_AON_POS))
#define AON_DCDC_SSTART_TIME_AON                                AON_DCDC_SSTART_TIME_AON
#define AON_DCDC_SSTART_TIME_AON_POS                            (28U)
#define AON_DCDC_SSTART_TIME_AON_LEN                            (2U)
#define AON_DCDC_SSTART_TIME_AON_MSK                            (((1U<<AON_DCDC_SSTART_TIME_AON_LEN)-1)<<AON_DCDC_SSTART_TIME_AON_POS)
#define AON_DCDC_SSTART_TIME_AON_UMSK                           (~(((1U<<AON_DCDC_SSTART_TIME_AON_LEN)-1)<<AON_DCDC_SSTART_TIME_AON_POS))
#define AON_DCDC_DIS_AON                                        AON_DCDC_DIS_AON
#define AON_DCDC_DIS_AON_POS                                    (30U)
#define AON_DCDC_DIS_AON_LEN                                    (1U)
#define AON_DCDC_DIS_AON_MSK                                    (((1U<<AON_DCDC_DIS_AON_LEN)-1)<<AON_DCDC_DIS_AON_POS)
#define AON_DCDC_DIS_AON_UMSK                                   (~(((1U<<AON_DCDC_DIS_AON_LEN)-1)<<AON_DCDC_DIS_AON_POS))
#define AON_DCDC_RDY_AON                                        AON_DCDC_RDY_AON
#define AON_DCDC_RDY_AON_POS                                    (31U)
#define AON_DCDC_RDY_AON_LEN                                    (1U)
#define AON_DCDC_RDY_AON_MSK                                    (((1U<<AON_DCDC_RDY_AON_LEN)-1)<<AON_DCDC_RDY_AON_POS)
#define AON_DCDC_RDY_AON_UMSK                                   (~(((1U<<AON_DCDC_RDY_AON_LEN)-1)<<AON_DCDC_RDY_AON_POS))

/* 0x818 : dcdc18_top_1 */
#define AON_DCDC18_TOP_1_OFFSET                                 (0x818)
#define AON_DCDC_FORCE_EN_CS_ZVS_AON                            AON_DCDC_FORCE_EN_CS_ZVS_AON
#define AON_DCDC_FORCE_EN_CS_ZVS_AON_POS                        (0U)
#define AON_DCDC_FORCE_EN_CS_ZVS_AON_LEN                        (1U)
#define AON_DCDC_FORCE_EN_CS_ZVS_AON_MSK                        (((1U<<AON_DCDC_FORCE_EN_CS_ZVS_AON_LEN)-1)<<AON_DCDC_FORCE_EN_CS_ZVS_AON_POS)
#define AON_DCDC_FORCE_EN_CS_ZVS_AON_UMSK                       (~(((1U<<AON_DCDC_FORCE_EN_CS_ZVS_AON_LEN)-1)<<AON_DCDC_FORCE_EN_CS_ZVS_AON_POS))
#define AON_DCDC_CS_DELAY_AON                                   AON_DCDC_CS_DELAY_AON
#define AON_DCDC_CS_DELAY_AON_POS                               (1U)
#define AON_DCDC_CS_DELAY_AON_LEN                               (3U)
#define AON_DCDC_CS_DELAY_AON_MSK                               (((1U<<AON_DCDC_CS_DELAY_AON_LEN)-1)<<AON_DCDC_CS_DELAY_AON_POS)
#define AON_DCDC_CS_DELAY_AON_UMSK                              (~(((1U<<AON_DCDC_CS_DELAY_AON_LEN)-1)<<AON_DCDC_CS_DELAY_AON_POS))
#define AON_DCDC_ZVS_TD_OPT_AON                                 AON_DCDC_ZVS_TD_OPT_AON
#define AON_DCDC_ZVS_TD_OPT_AON_POS                             (4U)
#define AON_DCDC_ZVS_TD_OPT_AON_LEN                             (3U)
#define AON_DCDC_ZVS_TD_OPT_AON_MSK                             (((1U<<AON_DCDC_ZVS_TD_OPT_AON_LEN)-1)<<AON_DCDC_ZVS_TD_OPT_AON_POS)
#define AON_DCDC_ZVS_TD_OPT_AON_UMSK                            (~(((1U<<AON_DCDC_ZVS_TD_OPT_AON_LEN)-1)<<AON_DCDC_ZVS_TD_OPT_AON_POS))
#define AON_DCDC_NONOVERLAP_TD_AON                              AON_DCDC_NONOVERLAP_TD_AON
#define AON_DCDC_NONOVERLAP_TD_AON_POS                          (8U)
#define AON_DCDC_NONOVERLAP_TD_AON_LEN                          (5U)
#define AON_DCDC_NONOVERLAP_TD_AON_MSK                          (((1U<<AON_DCDC_NONOVERLAP_TD_AON_LEN)-1)<<AON_DCDC_NONOVERLAP_TD_AON_POS)
#define AON_DCDC_NONOVERLAP_TD_AON_UMSK                         (~(((1U<<AON_DCDC_NONOVERLAP_TD_AON_LEN)-1)<<AON_DCDC_NONOVERLAP_TD_AON_POS))
#define AON_DCDC_RC_SEL_AON                                     AON_DCDC_RC_SEL_AON
#define AON_DCDC_RC_SEL_AON_POS                                 (16U)
#define AON_DCDC_RC_SEL_AON_LEN                                 (4U)
#define AON_DCDC_RC_SEL_AON_MSK                                 (((1U<<AON_DCDC_RC_SEL_AON_LEN)-1)<<AON_DCDC_RC_SEL_AON_POS)
#define AON_DCDC_RC_SEL_AON_UMSK                                (~(((1U<<AON_DCDC_RC_SEL_AON_LEN)-1)<<AON_DCDC_RC_SEL_AON_POS))
#define AON_DCDC_CHF_SEL_AON                                    AON_DCDC_CHF_SEL_AON
#define AON_DCDC_CHF_SEL_AON_POS                                (20U)
#define AON_DCDC_CHF_SEL_AON_LEN                                (4U)
#define AON_DCDC_CHF_SEL_AON_MSK                                (((1U<<AON_DCDC_CHF_SEL_AON_LEN)-1)<<AON_DCDC_CHF_SEL_AON_POS)
#define AON_DCDC_CHF_SEL_AON_UMSK                               (~(((1U<<AON_DCDC_CHF_SEL_AON_LEN)-1)<<AON_DCDC_CHF_SEL_AON_POS))
#define AON_DCDC_CFB_SEL_AON                                    AON_DCDC_CFB_SEL_AON
#define AON_DCDC_CFB_SEL_AON_POS                                (24U)
#define AON_DCDC_CFB_SEL_AON_LEN                                (4U)
#define AON_DCDC_CFB_SEL_AON_MSK                                (((1U<<AON_DCDC_CFB_SEL_AON_LEN)-1)<<AON_DCDC_CFB_SEL_AON_POS)
#define AON_DCDC_CFB_SEL_AON_UMSK                               (~(((1U<<AON_DCDC_CFB_SEL_AON_LEN)-1)<<AON_DCDC_CFB_SEL_AON_POS))
#define AON_DCDC_EN_ANTIRING_AON                                AON_DCDC_EN_ANTIRING_AON
#define AON_DCDC_EN_ANTIRING_AON_POS                            (28U)
#define AON_DCDC_EN_ANTIRING_AON_LEN                            (1U)
#define AON_DCDC_EN_ANTIRING_AON_MSK                            (((1U<<AON_DCDC_EN_ANTIRING_AON_LEN)-1)<<AON_DCDC_EN_ANTIRING_AON_POS)
#define AON_DCDC_EN_ANTIRING_AON_UMSK                           (~(((1U<<AON_DCDC_EN_ANTIRING_AON_LEN)-1)<<AON_DCDC_EN_ANTIRING_AON_POS))
#define AON_DCDC_PULLDOWN_AON                                   AON_DCDC_PULLDOWN_AON
#define AON_DCDC_PULLDOWN_AON_POS                               (29U)
#define AON_DCDC_PULLDOWN_AON_LEN                               (1U)
#define AON_DCDC_PULLDOWN_AON_MSK                               (((1U<<AON_DCDC_PULLDOWN_AON_LEN)-1)<<AON_DCDC_PULLDOWN_AON_POS)
#define AON_DCDC_PULLDOWN_AON_UMSK                              (~(((1U<<AON_DCDC_PULLDOWN_AON_LEN)-1)<<AON_DCDC_PULLDOWN_AON_POS))

/* 0x81C : dcdc_top_2 */
#define AON_DCDC_TOP_2_OFFSET                                   (0x81C)
#define AON_DCDC_VOUT_TRIM_AON                                  AON_DCDC_VOUT_TRIM_AON
#define AON_DCDC_VOUT_TRIM_AON_POS                              (0U)
#define AON_DCDC_VOUT_TRIM_AON_LEN                              (4U)
#define AON_DCDC_VOUT_TRIM_AON_MSK                              (((1U<<AON_DCDC_VOUT_TRIM_AON_LEN)-1)<<AON_DCDC_VOUT_TRIM_AON_POS)
#define AON_DCDC_VOUT_TRIM_AON_UMSK                             (~(((1U<<AON_DCDC_VOUT_TRIM_AON_LEN)-1)<<AON_DCDC_VOUT_TRIM_AON_POS))
#define AON_DCDC_OSC_SS_RSTN_AON                                AON_DCDC_OSC_SS_RSTN_AON
#define AON_DCDC_OSC_SS_RSTN_AON_POS                            (4U)
#define AON_DCDC_OSC_SS_RSTN_AON_LEN                            (1U)
#define AON_DCDC_OSC_SS_RSTN_AON_MSK                            (((1U<<AON_DCDC_OSC_SS_RSTN_AON_LEN)-1)<<AON_DCDC_OSC_SS_RSTN_AON_POS)
#define AON_DCDC_OSC_SS_RSTN_AON_UMSK                           (~(((1U<<AON_DCDC_OSC_SS_RSTN_AON_LEN)-1)<<AON_DCDC_OSC_SS_RSTN_AON_POS))
#define AON_DCDC_OSC_SS_EN_AON                                  AON_DCDC_OSC_SS_EN_AON
#define AON_DCDC_OSC_SS_EN_AON_POS                              (5U)
#define AON_DCDC_OSC_SS_EN_AON_LEN                              (1U)
#define AON_DCDC_OSC_SS_EN_AON_MSK                              (((1U<<AON_DCDC_OSC_SS_EN_AON_LEN)-1)<<AON_DCDC_OSC_SS_EN_AON_POS)
#define AON_DCDC_OSC_SS_EN_AON_UMSK                             (~(((1U<<AON_DCDC_OSC_SS_EN_AON_LEN)-1)<<AON_DCDC_OSC_SS_EN_AON_POS))
#define AON_DCDC_OSC_SS_PERIOD_AON                              AON_DCDC_OSC_SS_PERIOD_AON
#define AON_DCDC_OSC_SS_PERIOD_AON_POS                          (8U)
#define AON_DCDC_OSC_SS_PERIOD_AON_LEN                          (2U)
#define AON_DCDC_OSC_SS_PERIOD_AON_MSK                          (((1U<<AON_DCDC_OSC_SS_PERIOD_AON_LEN)-1)<<AON_DCDC_OSC_SS_PERIOD_AON_POS)
#define AON_DCDC_OSC_SS_PERIOD_AON_UMSK                         (~(((1U<<AON_DCDC_OSC_SS_PERIOD_AON_LEN)-1)<<AON_DCDC_OSC_SS_PERIOD_AON_POS))
#define AON_DCDC_OSC_SS_FDEV_AON                                AON_DCDC_OSC_SS_FDEV_AON
#define AON_DCDC_OSC_SS_FDEV_AON_POS                            (10U)
#define AON_DCDC_OSC_SS_FDEV_AON_LEN                            (2U)
#define AON_DCDC_OSC_SS_FDEV_AON_MSK                            (((1U<<AON_DCDC_OSC_SS_FDEV_AON_LEN)-1)<<AON_DCDC_OSC_SS_FDEV_AON_POS)
#define AON_DCDC_OSC_SS_FDEV_AON_UMSK                           (~(((1U<<AON_DCDC_OSC_SS_FDEV_AON_LEN)-1)<<AON_DCDC_OSC_SS_FDEV_AON_POS))
#define AON_DCDC_COMP_GM_SEL_AON                                AON_DCDC_COMP_GM_SEL_AON
#define AON_DCDC_COMP_GM_SEL_AON_POS                            (12U)
#define AON_DCDC_COMP_GM_SEL_AON_LEN                            (3U)
#define AON_DCDC_COMP_GM_SEL_AON_MSK                            (((1U<<AON_DCDC_COMP_GM_SEL_AON_LEN)-1)<<AON_DCDC_COMP_GM_SEL_AON_POS)
#define AON_DCDC_COMP_GM_SEL_AON_UMSK                           (~(((1U<<AON_DCDC_COMP_GM_SEL_AON_LEN)-1)<<AON_DCDC_COMP_GM_SEL_AON_POS))
#define AON_DCDC_ISENSE_TRIM_AON                                AON_DCDC_ISENSE_TRIM_AON
#define AON_DCDC_ISENSE_TRIM_AON_POS                            (16U)
#define AON_DCDC_ISENSE_TRIM_AON_LEN                            (3U)
#define AON_DCDC_ISENSE_TRIM_AON_MSK                            (((1U<<AON_DCDC_ISENSE_TRIM_AON_LEN)-1)<<AON_DCDC_ISENSE_TRIM_AON_POS)
#define AON_DCDC_ISENSE_TRIM_AON_UMSK                           (~(((1U<<AON_DCDC_ISENSE_TRIM_AON_LEN)-1)<<AON_DCDC_ISENSE_TRIM_AON_POS))
#define AON_DCDC_VC_CLAMP_VTH_AON                               AON_DCDC_VC_CLAMP_VTH_AON
#define AON_DCDC_VC_CLAMP_VTH_AON_POS                           (20U)
#define AON_DCDC_VC_CLAMP_VTH_AON_LEN                           (3U)
#define AON_DCDC_VC_CLAMP_VTH_AON_MSK                           (((1U<<AON_DCDC_VC_CLAMP_VTH_AON_LEN)-1)<<AON_DCDC_VC_CLAMP_VTH_AON_POS)
#define AON_DCDC_VC_CLAMP_VTH_AON_UMSK                          (~(((1U<<AON_DCDC_VC_CLAMP_VTH_AON_LEN)-1)<<AON_DCDC_VC_CLAMP_VTH_AON_POS))
#define AON_DCDC_OCP_VTH_AON                                    AON_DCDC_OCP_VTH_AON
#define AON_DCDC_OCP_VTH_AON_POS                                (24U)
#define AON_DCDC_OCP_VTH_AON_LEN                                (3U)
#define AON_DCDC_OCP_VTH_AON_MSK                                (((1U<<AON_DCDC_OCP_VTH_AON_LEN)-1)<<AON_DCDC_OCP_VTH_AON_POS)
#define AON_DCDC_OCP_VTH_AON_UMSK                               (~(((1U<<AON_DCDC_OCP_VTH_AON_LEN)-1)<<AON_DCDC_OCP_VTH_AON_POS))
#define AON_DCDC_OCP_RST_AON                                    AON_DCDC_OCP_RST_AON
#define AON_DCDC_OCP_RST_AON_POS                                (27U)
#define AON_DCDC_OCP_RST_AON_LEN                                (1U)
#define AON_DCDC_OCP_RST_AON_MSK                                (((1U<<AON_DCDC_OCP_RST_AON_LEN)-1)<<AON_DCDC_OCP_RST_AON_POS)
#define AON_DCDC_OCP_RST_AON_UMSK                               (~(((1U<<AON_DCDC_OCP_RST_AON_LEN)-1)<<AON_DCDC_OCP_RST_AON_POS))
#define AON_DCDC_OCP_OUT_AON                                    AON_DCDC_OCP_OUT_AON
#define AON_DCDC_OCP_OUT_AON_POS                                (28U)
#define AON_DCDC_OCP_OUT_AON_LEN                                (1U)
#define AON_DCDC_OCP_OUT_AON_MSK                                (((1U<<AON_DCDC_OCP_OUT_AON_LEN)-1)<<AON_DCDC_OCP_OUT_AON_POS)
#define AON_DCDC_OCP_OUT_AON_UMSK                               (~(((1U<<AON_DCDC_OCP_OUT_AON_LEN)-1)<<AON_DCDC_OCP_OUT_AON_POS))
#define AON_DCDC_DRV_SR_AON                                     AON_DCDC_DRV_SR_AON
#define AON_DCDC_DRV_SR_AON_POS                                 (29U)
#define AON_DCDC_DRV_SR_AON_LEN                                 (2U)
#define AON_DCDC_DRV_SR_AON_MSK                                 (((1U<<AON_DCDC_DRV_SR_AON_LEN)-1)<<AON_DCDC_DRV_SR_AON_POS)
#define AON_DCDC_DRV_SR_AON_UMSK                                (~(((1U<<AON_DCDC_DRV_SR_AON_LEN)-1)<<AON_DCDC_DRV_SR_AON_POS))

/* 0x820 : ldo11soc_top */
#define AON_LDO11SOC_TOP_OFFSET                                 (0x820)
#define AON_LDO11SOC_VOUT_TRIM_AON                              AON_LDO11SOC_VOUT_TRIM_AON
#define AON_LDO11SOC_VOUT_TRIM_AON_POS                          (0U)
#define AON_LDO11SOC_VOUT_TRIM_AON_LEN                          (4U)
#define AON_LDO11SOC_VOUT_TRIM_AON_MSK                          (((1U<<AON_LDO11SOC_VOUT_TRIM_AON_LEN)-1)<<AON_LDO11SOC_VOUT_TRIM_AON_POS)
#define AON_LDO11SOC_VOUT_TRIM_AON_UMSK                         (~(((1U<<AON_LDO11SOC_VOUT_TRIM_AON_LEN)-1)<<AON_LDO11SOC_VOUT_TRIM_AON_POS))
#define AON_PU_LDO11SOC_AON                                     AON_PU_LDO11SOC_AON
#define AON_PU_LDO11SOC_AON_POS                                 (4U)
#define AON_PU_LDO11SOC_AON_LEN                                 (1U)
#define AON_PU_LDO11SOC_AON_MSK                                 (((1U<<AON_PU_LDO11SOC_AON_LEN)-1)<<AON_PU_LDO11SOC_AON_POS)
#define AON_PU_LDO11SOC_AON_UMSK                                (~(((1U<<AON_PU_LDO11SOC_AON_LEN)-1)<<AON_PU_LDO11SOC_AON_POS))
#define AON_LDO11SOC_SSTART_SEL_AON                             AON_LDO11SOC_SSTART_SEL_AON
#define AON_LDO11SOC_SSTART_SEL_AON_POS                         (5U)
#define AON_LDO11SOC_SSTART_SEL_AON_LEN                         (1U)
#define AON_LDO11SOC_SSTART_SEL_AON_MSK                         (((1U<<AON_LDO11SOC_SSTART_SEL_AON_LEN)-1)<<AON_LDO11SOC_SSTART_SEL_AON_POS)
#define AON_LDO11SOC_SSTART_SEL_AON_UMSK                        (~(((1U<<AON_LDO11SOC_SSTART_SEL_AON_LEN)-1)<<AON_LDO11SOC_SSTART_SEL_AON_POS))
#define AON_LDO11SOC_SSTART_DELAY_AON                           AON_LDO11SOC_SSTART_DELAY_AON
#define AON_LDO11SOC_SSTART_DELAY_AON_POS                       (8U)
#define AON_LDO11SOC_SSTART_DELAY_AON_LEN                       (2U)
#define AON_LDO11SOC_SSTART_DELAY_AON_MSK                       (((1U<<AON_LDO11SOC_SSTART_DELAY_AON_LEN)-1)<<AON_LDO11SOC_SSTART_DELAY_AON_POS)
#define AON_LDO11SOC_SSTART_DELAY_AON_UMSK                      (~(((1U<<AON_LDO11SOC_SSTART_DELAY_AON_LEN)-1)<<AON_LDO11SOC_SSTART_DELAY_AON_POS))
#define AON_LDO11SOC_PULLDOWN_AON                               AON_LDO11SOC_PULLDOWN_AON
#define AON_LDO11SOC_PULLDOWN_AON_POS                           (10U)
#define AON_LDO11SOC_PULLDOWN_AON_LEN                           (1U)
#define AON_LDO11SOC_PULLDOWN_AON_MSK                           (((1U<<AON_LDO11SOC_PULLDOWN_AON_LEN)-1)<<AON_LDO11SOC_PULLDOWN_AON_POS)
#define AON_LDO11SOC_PULLDOWN_AON_UMSK                          (~(((1U<<AON_LDO11SOC_PULLDOWN_AON_LEN)-1)<<AON_LDO11SOC_PULLDOWN_AON_POS))
#define AON_LDO11SOC_PULLDOWN_SEL_AON                           AON_LDO11SOC_PULLDOWN_SEL_AON
#define AON_LDO11SOC_PULLDOWN_SEL_AON_POS                       (11U)
#define AON_LDO11SOC_PULLDOWN_SEL_AON_LEN                       (1U)
#define AON_LDO11SOC_PULLDOWN_SEL_AON_MSK                       (((1U<<AON_LDO11SOC_PULLDOWN_SEL_AON_LEN)-1)<<AON_LDO11SOC_PULLDOWN_SEL_AON_POS)
#define AON_LDO11SOC_PULLDOWN_SEL_AON_UMSK                      (~(((1U<<AON_LDO11SOC_PULLDOWN_SEL_AON_LEN)-1)<<AON_LDO11SOC_PULLDOWN_SEL_AON_POS))
#define AON_LDO11SOC_VTH_SEL_AON                                AON_LDO11SOC_VTH_SEL_AON
#define AON_LDO11SOC_VTH_SEL_AON_POS                            (12U)
#define AON_LDO11SOC_VTH_SEL_AON_LEN                            (2U)
#define AON_LDO11SOC_VTH_SEL_AON_MSK                            (((1U<<AON_LDO11SOC_VTH_SEL_AON_LEN)-1)<<AON_LDO11SOC_VTH_SEL_AON_POS)
#define AON_LDO11SOC_VTH_SEL_AON_UMSK                           (~(((1U<<AON_LDO11SOC_VTH_SEL_AON_LEN)-1)<<AON_LDO11SOC_VTH_SEL_AON_POS))
#define AON_LDO11SOC_CC_AON                                     AON_LDO11SOC_CC_AON
#define AON_LDO11SOC_CC_AON_POS                                 (24U)
#define AON_LDO11SOC_CC_AON_LEN                                 (2U)
#define AON_LDO11SOC_CC_AON_MSK                                 (((1U<<AON_LDO11SOC_CC_AON_LEN)-1)<<AON_LDO11SOC_CC_AON_POS)
#define AON_LDO11SOC_CC_AON_UMSK                                (~(((1U<<AON_LDO11SOC_CC_AON_LEN)-1)<<AON_LDO11SOC_CC_AON_POS))
#define AON_LDO11SOC_RDY_AON                                    AON_LDO11SOC_RDY_AON
#define AON_LDO11SOC_RDY_AON_POS                                (28U)
#define AON_LDO11SOC_RDY_AON_LEN                                (1U)
#define AON_LDO11SOC_RDY_AON_MSK                                (((1U<<AON_LDO11SOC_RDY_AON_LEN)-1)<<AON_LDO11SOC_RDY_AON_POS)
#define AON_LDO11SOC_RDY_AON_UMSK                               (~(((1U<<AON_LDO11SOC_RDY_AON_LEN)-1)<<AON_LDO11SOC_RDY_AON_POS))
#define AON_LDO11SOC_POWER_GOOD_AON                             AON_LDO11SOC_POWER_GOOD_AON
#define AON_LDO11SOC_POWER_GOOD_AON_POS                         (29U)
#define AON_LDO11SOC_POWER_GOOD_AON_LEN                         (1U)
#define AON_LDO11SOC_POWER_GOOD_AON_MSK                         (((1U<<AON_LDO11SOC_POWER_GOOD_AON_LEN)-1)<<AON_LDO11SOC_POWER_GOOD_AON_POS)
#define AON_LDO11SOC_POWER_GOOD_AON_UMSK                        (~(((1U<<AON_LDO11SOC_POWER_GOOD_AON_LEN)-1)<<AON_LDO11SOC_POWER_GOOD_AON_POS))

/* 0x824 : ldo14_top */
#define AON_LDO14_TOP_OFFSET                                    (0x824)
#define AON_LDO14_OCP_TH_AON                                    AON_LDO14_OCP_TH_AON
#define AON_LDO14_OCP_TH_AON_POS                                (1U)
#define AON_LDO14_OCP_TH_AON_LEN                                (3U)
#define AON_LDO14_OCP_TH_AON_MSK                                (((1U<<AON_LDO14_OCP_TH_AON_LEN)-1)<<AON_LDO14_OCP_TH_AON_POS)
#define AON_LDO14_OCP_TH_AON_UMSK                               (~(((1U<<AON_LDO14_OCP_TH_AON_LEN)-1)<<AON_LDO14_OCP_TH_AON_POS))
#define AON_LDO14_OCP_EN_AON                                    AON_LDO14_OCP_EN_AON
#define AON_LDO14_OCP_EN_AON_POS                                (4U)
#define AON_LDO14_OCP_EN_AON_LEN                                (1U)
#define AON_LDO14_OCP_EN_AON_MSK                                (((1U<<AON_LDO14_OCP_EN_AON_LEN)-1)<<AON_LDO14_OCP_EN_AON_POS)
#define AON_LDO14_OCP_EN_AON_UMSK                               (~(((1U<<AON_LDO14_OCP_EN_AON_LEN)-1)<<AON_LDO14_OCP_EN_AON_POS))
#define AON_LDO14_OCP_OUT_AON                                   AON_LDO14_OCP_OUT_AON
#define AON_LDO14_OCP_OUT_AON_POS                               (5U)
#define AON_LDO14_OCP_OUT_AON_LEN                               (1U)
#define AON_LDO14_OCP_OUT_AON_MSK                               (((1U<<AON_LDO14_OCP_OUT_AON_LEN)-1)<<AON_LDO14_OCP_OUT_AON_POS)
#define AON_LDO14_OCP_OUT_AON_UMSK                              (~(((1U<<AON_LDO14_OCP_OUT_AON_LEN)-1)<<AON_LDO14_OCP_OUT_AON_POS))
#define AON_LDO14_BM_AON                                        AON_LDO14_BM_AON
#define AON_LDO14_BM_AON_POS                                    (6U)
#define AON_LDO14_BM_AON_LEN                                    (2U)
#define AON_LDO14_BM_AON_MSK                                    (((1U<<AON_LDO14_BM_AON_LEN)-1)<<AON_LDO14_BM_AON_POS)
#define AON_LDO14_BM_AON_UMSK                                   (~(((1U<<AON_LDO14_BM_AON_LEN)-1)<<AON_LDO14_BM_AON_POS))
#define AON_LDO14_SSTART_SEL_AON                                AON_LDO14_SSTART_SEL_AON
#define AON_LDO14_SSTART_SEL_AON_POS                            (8U)
#define AON_LDO14_SSTART_SEL_AON_LEN                            (1U)
#define AON_LDO14_SSTART_SEL_AON_MSK                            (((1U<<AON_LDO14_SSTART_SEL_AON_LEN)-1)<<AON_LDO14_SSTART_SEL_AON_POS)
#define AON_LDO14_SSTART_SEL_AON_UMSK                           (~(((1U<<AON_LDO14_SSTART_SEL_AON_LEN)-1)<<AON_LDO14_SSTART_SEL_AON_POS))
#define AON_LDO14_SSTART_DELAY_AON                              AON_LDO14_SSTART_DELAY_AON
#define AON_LDO14_SSTART_DELAY_AON_POS                          (9U)
#define AON_LDO14_SSTART_DELAY_AON_LEN                          (2U)
#define AON_LDO14_SSTART_DELAY_AON_MSK                          (((1U<<AON_LDO14_SSTART_DELAY_AON_LEN)-1)<<AON_LDO14_SSTART_DELAY_AON_POS)
#define AON_LDO14_SSTART_DELAY_AON_UMSK                         (~(((1U<<AON_LDO14_SSTART_DELAY_AON_LEN)-1)<<AON_LDO14_SSTART_DELAY_AON_POS))
#define AON_LDO14_RDY_AON                                       AON_LDO14_RDY_AON
#define AON_LDO14_RDY_AON_POS                                   (11U)
#define AON_LDO14_RDY_AON_LEN                                   (1U)
#define AON_LDO14_RDY_AON_MSK                                   (((1U<<AON_LDO14_RDY_AON_LEN)-1)<<AON_LDO14_RDY_AON_POS)
#define AON_LDO14_RDY_AON_UMSK                                  (~(((1U<<AON_LDO14_RDY_AON_LEN)-1)<<AON_LDO14_RDY_AON_POS))
#define AON_LDO14_PULLDOWN_AON                                  AON_LDO14_PULLDOWN_AON
#define AON_LDO14_PULLDOWN_AON_POS                              (12U)
#define AON_LDO14_PULLDOWN_AON_LEN                              (1U)
#define AON_LDO14_PULLDOWN_AON_MSK                              (((1U<<AON_LDO14_PULLDOWN_AON_LEN)-1)<<AON_LDO14_PULLDOWN_AON_POS)
#define AON_LDO14_PULLDOWN_AON_UMSK                             (~(((1U<<AON_LDO14_PULLDOWN_AON_LEN)-1)<<AON_LDO14_PULLDOWN_AON_POS))
#define AON_LDO14_PULLDOWN_SEL_AON                              AON_LDO14_PULLDOWN_SEL_AON
#define AON_LDO14_PULLDOWN_SEL_AON_POS                          (13U)
#define AON_LDO14_PULLDOWN_SEL_AON_LEN                          (1U)
#define AON_LDO14_PULLDOWN_SEL_AON_MSK                          (((1U<<AON_LDO14_PULLDOWN_SEL_AON_LEN)-1)<<AON_LDO14_PULLDOWN_SEL_AON_POS)
#define AON_LDO14_PULLDOWN_SEL_AON_UMSK                         (~(((1U<<AON_LDO14_PULLDOWN_SEL_AON_LEN)-1)<<AON_LDO14_PULLDOWN_SEL_AON_POS))
#define AON_LDO14_VOUT_SEL_AON                                  AON_LDO14_VOUT_SEL_AON
#define AON_LDO14_VOUT_SEL_AON_POS                              (16U)
#define AON_LDO14_VOUT_SEL_AON_LEN                              (3U)
#define AON_LDO14_VOUT_SEL_AON_MSK                              (((1U<<AON_LDO14_VOUT_SEL_AON_LEN)-1)<<AON_LDO14_VOUT_SEL_AON_POS)
#define AON_LDO14_VOUT_SEL_AON_UMSK                             (~(((1U<<AON_LDO14_VOUT_SEL_AON_LEN)-1)<<AON_LDO14_VOUT_SEL_AON_POS))
#define AON_LDO14_CC_AON                                        AON_LDO14_CC_AON
#define AON_LDO14_CC_AON_POS                                    (24U)
#define AON_LDO14_CC_AON_LEN                                    (2U)
#define AON_LDO14_CC_AON_MSK                                    (((1U<<AON_LDO14_CC_AON_LEN)-1)<<AON_LDO14_CC_AON_POS)
#define AON_LDO14_CC_AON_UMSK                                   (~(((1U<<AON_LDO14_CC_AON_LEN)-1)<<AON_LDO14_CC_AON_POS))
#define AON_LDO14_BYPASS_AON                                    AON_LDO14_BYPASS_AON
#define AON_LDO14_BYPASS_AON_POS                                (27U)
#define AON_LDO14_BYPASS_AON_LEN                                (1U)
#define AON_LDO14_BYPASS_AON_MSK                                (((1U<<AON_LDO14_BYPASS_AON_LEN)-1)<<AON_LDO14_BYPASS_AON_POS)
#define AON_LDO14_BYPASS_AON_UMSK                               (~(((1U<<AON_LDO14_BYPASS_AON_LEN)-1)<<AON_LDO14_BYPASS_AON_POS))
#define AON_LDO14_VOUT_TRIM_AON                                 AON_LDO14_VOUT_TRIM_AON
#define AON_LDO14_VOUT_TRIM_AON_POS                             (28U)
#define AON_LDO14_VOUT_TRIM_AON_LEN                             (4U)
#define AON_LDO14_VOUT_TRIM_AON_MSK                             (((1U<<AON_LDO14_VOUT_TRIM_AON_LEN)-1)<<AON_LDO14_VOUT_TRIM_AON_POS)
#define AON_LDO14_VOUT_TRIM_AON_UMSK                            (~(((1U<<AON_LDO14_VOUT_TRIM_AON_LEN)-1)<<AON_LDO14_VOUT_TRIM_AON_POS))

/* 0x828 : psw_irrcv */
#define AON_PSW_IRRCV_OFFSET                                    (0x828)
#define AON_PU_IR_PSW_AON                                       AON_PU_IR_PSW_AON
#define AON_PU_IR_PSW_AON_POS                                   (0U)
#define AON_PU_IR_PSW_AON_LEN                                   (1U)
#define AON_PU_IR_PSW_AON_MSK                                   (((1U<<AON_PU_IR_PSW_AON_LEN)-1)<<AON_PU_IR_PSW_AON_POS)
#define AON_PU_IR_PSW_AON_UMSK                                  (~(((1U<<AON_PU_IR_PSW_AON_LEN)-1)<<AON_PU_IR_PSW_AON_POS))

/* 0x880 : rf_top_aon */
#define AON_RF_TOP_AON_OFFSET                                   (0x880)
#define AON_PU_MBG_AON                                          AON_PU_MBG_AON
#define AON_PU_MBG_AON_POS                                      (0U)
#define AON_PU_MBG_AON_LEN                                      (1U)
#define AON_PU_MBG_AON_MSK                                      (((1U<<AON_PU_MBG_AON_LEN)-1)<<AON_PU_MBG_AON_POS)
#define AON_PU_MBG_AON_UMSK                                     (~(((1U<<AON_PU_MBG_AON_LEN)-1)<<AON_PU_MBG_AON_POS))
#define AON_SW_BZ_EN_AON                                        AON_SW_BZ_EN_AON
#define AON_SW_BZ_EN_AON_POS                                    (1U)
#define AON_SW_BZ_EN_AON_LEN                                    (1U)
#define AON_SW_BZ_EN_AON_MSK                                    (((1U<<AON_SW_BZ_EN_AON_LEN)-1)<<AON_SW_BZ_EN_AON_POS)
#define AON_SW_BZ_EN_AON_UMSK                                   (~(((1U<<AON_SW_BZ_EN_AON_LEN)-1)<<AON_SW_BZ_EN_AON_POS))
#define AON_PU_SFREG_AON                                        AON_PU_SFREG_AON
#define AON_PU_SFREG_AON_POS                                    (2U)
#define AON_PU_SFREG_AON_LEN                                    (1U)
#define AON_PU_SFREG_AON_MSK                                    (((1U<<AON_PU_SFREG_AON_LEN)-1)<<AON_PU_SFREG_AON_POS)
#define AON_PU_SFREG_AON_UMSK                                   (~(((1U<<AON_PU_SFREG_AON_LEN)-1)<<AON_PU_SFREG_AON_POS))
#define AON_PUD_XTAL_AON                                        AON_PUD_XTAL_AON
#define AON_PUD_XTAL_AON_POS                                    (3U)
#define AON_PUD_XTAL_AON_LEN                                    (1U)
#define AON_PUD_XTAL_AON_MSK                                    (((1U<<AON_PUD_XTAL_AON_LEN)-1)<<AON_PUD_XTAL_AON_POS)
#define AON_PUD_XTAL_AON_UMSK                                   (~(((1U<<AON_PUD_XTAL_AON_LEN)-1)<<AON_PUD_XTAL_AON_POS))
#define AON_PU_XTAL_AON                                         AON_PU_XTAL_AON
#define AON_PU_XTAL_AON_POS                                     (5U)
#define AON_PU_XTAL_AON_LEN                                     (1U)
#define AON_PU_XTAL_AON_MSK                                     (((1U<<AON_PU_XTAL_AON_LEN)-1)<<AON_PU_XTAL_AON_POS)
#define AON_PU_XTAL_AON_UMSK                                    (~(((1U<<AON_PU_XTAL_AON_LEN)-1)<<AON_PU_XTAL_AON_POS))
#define AON_PU_XTAL_HF_RC32M_AON                                AON_PU_XTAL_HF_RC32M_AON
#define AON_PU_XTAL_HF_RC32M_AON_POS                            (6U)
#define AON_PU_XTAL_HF_RC32M_AON_LEN                            (1U)
#define AON_PU_XTAL_HF_RC32M_AON_MSK                            (((1U<<AON_PU_XTAL_HF_RC32M_AON_LEN)-1)<<AON_PU_XTAL_HF_RC32M_AON_POS)
#define AON_PU_XTAL_HF_RC32M_AON_UMSK                           (~(((1U<<AON_PU_XTAL_HF_RC32M_AON_LEN)-1)<<AON_PU_XTAL_HF_RC32M_AON_POS))
#define AON_PUD_XTAL_HF_RC32M_AON                               AON_PUD_XTAL_HF_RC32M_AON
#define AON_PUD_XTAL_HF_RC32M_AON_POS                           (7U)
#define AON_PUD_XTAL_HF_RC32M_AON_LEN                           (1U)
#define AON_PUD_XTAL_HF_RC32M_AON_MSK                           (((1U<<AON_PUD_XTAL_HF_RC32M_AON_LEN)-1)<<AON_PUD_XTAL_HF_RC32M_AON_POS)
#define AON_PUD_XTAL_HF_RC32M_AON_UMSK                          (~(((1U<<AON_PUD_XTAL_HF_RC32M_AON_LEN)-1)<<AON_PUD_XTAL_HF_RC32M_AON_POS))

/* 0x884 : xtal_cfg */
#define AON_XTAL_CFG_OFFSET                                     (0x884)
#define AON_XTAL_BK_AON                                         AON_XTAL_BK_AON
#define AON_XTAL_BK_AON_POS                                     (0U)
#define AON_XTAL_BK_AON_LEN                                     (2U)
#define AON_XTAL_BK_AON_MSK                                     (((1U<<AON_XTAL_BK_AON_LEN)-1)<<AON_XTAL_BK_AON_POS)
#define AON_XTAL_BK_AON_UMSK                                    (~(((1U<<AON_XTAL_BK_AON_LEN)-1)<<AON_XTAL_BK_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_AON                              AON_XTAL_CAPCODE_EXTRA_AON
#define AON_XTAL_CAPCODE_EXTRA_AON_POS                          (2U)
#define AON_XTAL_CAPCODE_EXTRA_AON_LEN                          (1U)
#define AON_XTAL_CAPCODE_EXTRA_AON_MSK                          (((1U<<AON_XTAL_CAPCODE_EXTRA_AON_LEN)-1)<<AON_XTAL_CAPCODE_EXTRA_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_AON_UMSK                         (~(((1U<<AON_XTAL_CAPCODE_EXTRA_AON_LEN)-1)<<AON_XTAL_CAPCODE_EXTRA_AON_POS))
#define AON_XTAL_EXT_SEL_AON                                    AON_XTAL_EXT_SEL_AON
#define AON_XTAL_EXT_SEL_AON_POS                                (3U)
#define AON_XTAL_EXT_SEL_AON_LEN                                (1U)
#define AON_XTAL_EXT_SEL_AON_MSK                                (((1U<<AON_XTAL_EXT_SEL_AON_LEN)-1)<<AON_XTAL_EXT_SEL_AON_POS)
#define AON_XTAL_EXT_SEL_AON_UMSK                               (~(((1U<<AON_XTAL_EXT_SEL_AON_LEN)-1)<<AON_XTAL_EXT_SEL_AON_POS))
#define AON_XTAL_BUF_EN_AON                                     AON_XTAL_BUF_EN_AON
#define AON_XTAL_BUF_EN_AON_POS                                 (4U)
#define AON_XTAL_BUF_EN_AON_LEN                                 (4U)
#define AON_XTAL_BUF_EN_AON_MSK                                 (((1U<<AON_XTAL_BUF_EN_AON_LEN)-1)<<AON_XTAL_BUF_EN_AON_POS)
#define AON_XTAL_BUF_EN_AON_UMSK                                (~(((1U<<AON_XTAL_BUF_EN_AON_LEN)-1)<<AON_XTAL_BUF_EN_AON_POS))
#define AON_XTAL_BUF_HP_AON                                     AON_XTAL_BUF_HP_AON
#define AON_XTAL_BUF_HP_AON_POS                                 (8U)
#define AON_XTAL_BUF_HP_AON_LEN                                 (4U)
#define AON_XTAL_BUF_HP_AON_MSK                                 (((1U<<AON_XTAL_BUF_HP_AON_LEN)-1)<<AON_XTAL_BUF_HP_AON_POS)
#define AON_XTAL_BUF_HP_AON_UMSK                                (~(((1U<<AON_XTAL_BUF_HP_AON_LEN)-1)<<AON_XTAL_BUF_HP_AON_POS))
#define AON_XTAL_SLEEP_AON                                      AON_XTAL_SLEEP_AON
#define AON_XTAL_SLEEP_AON_POS                                  (13U)
#define AON_XTAL_SLEEP_AON_LEN                                  (1U)
#define AON_XTAL_SLEEP_AON_MSK                                  (((1U<<AON_XTAL_SLEEP_AON_LEN)-1)<<AON_XTAL_SLEEP_AON_POS)
#define AON_XTAL_SLEEP_AON_UMSK                                 (~(((1U<<AON_XTAL_SLEEP_AON_LEN)-1)<<AON_XTAL_SLEEP_AON_POS))
#define AON_XTAL_HF_RC32M_CODE_AON                              AON_XTAL_HF_RC32M_CODE_AON
#define AON_XTAL_HF_RC32M_CODE_AON_POS                          (16U)
#define AON_XTAL_HF_RC32M_CODE_AON_LEN                          (8U)
#define AON_XTAL_HF_RC32M_CODE_AON_MSK                          (((1U<<AON_XTAL_HF_RC32M_CODE_AON_LEN)-1)<<AON_XTAL_HF_RC32M_CODE_AON_POS)
#define AON_XTAL_HF_RC32M_CODE_AON_UMSK                         (~(((1U<<AON_XTAL_HF_RC32M_CODE_AON_LEN)-1)<<AON_XTAL_HF_RC32M_CODE_AON_POS))
#define AON_XTAL_GM_BOOST_AON                                   AON_XTAL_GM_BOOST_AON
#define AON_XTAL_GM_BOOST_AON_POS                               (28U)
#define AON_XTAL_GM_BOOST_AON_LEN                               (2U)
#define AON_XTAL_GM_BOOST_AON_MSK                               (((1U<<AON_XTAL_GM_BOOST_AON_LEN)-1)<<AON_XTAL_GM_BOOST_AON_POS)
#define AON_XTAL_GM_BOOST_AON_UMSK                              (~(((1U<<AON_XTAL_GM_BOOST_AON_LEN)-1)<<AON_XTAL_GM_BOOST_AON_POS))
#define AON_XTAL_RDY_SEL_AON                                    AON_XTAL_RDY_SEL_AON
#define AON_XTAL_RDY_SEL_AON_POS                                (30U)
#define AON_XTAL_RDY_SEL_AON_LEN                                (2U)
#define AON_XTAL_RDY_SEL_AON_MSK                                (((1U<<AON_XTAL_RDY_SEL_AON_LEN)-1)<<AON_XTAL_RDY_SEL_AON_POS)
#define AON_XTAL_RDY_SEL_AON_UMSK                               (~(((1U<<AON_XTAL_RDY_SEL_AON_LEN)-1)<<AON_XTAL_RDY_SEL_AON_POS))

/* 0x888 : xtal_cfg2 */
#define AON_XTAL_CFG2_OFFSET                                    (0x888)
#define AON_XTAL_HF_CAPCODE_OUT_AON                             AON_XTAL_HF_CAPCODE_OUT_AON
#define AON_XTAL_HF_CAPCODE_OUT_AON_POS                         (0U)
#define AON_XTAL_HF_CAPCODE_OUT_AON_LEN                         (8U)
#define AON_XTAL_HF_CAPCODE_OUT_AON_MSK                         (((1U<<AON_XTAL_HF_CAPCODE_OUT_AON_LEN)-1)<<AON_XTAL_HF_CAPCODE_OUT_AON_POS)
#define AON_XTAL_HF_CAPCODE_OUT_AON_UMSK                        (~(((1U<<AON_XTAL_HF_CAPCODE_OUT_AON_LEN)-1)<<AON_XTAL_HF_CAPCODE_OUT_AON_POS))
#define AON_XTAL_HF_CAPCODE_IN_AON                              AON_XTAL_HF_CAPCODE_IN_AON
#define AON_XTAL_HF_CAPCODE_IN_AON_POS                          (8U)
#define AON_XTAL_HF_CAPCODE_IN_AON_LEN                          (8U)
#define AON_XTAL_HF_CAPCODE_IN_AON_MSK                          (((1U<<AON_XTAL_HF_CAPCODE_IN_AON_LEN)-1)<<AON_XTAL_HF_CAPCODE_IN_AON_POS)
#define AON_XTAL_HF_CAPCODE_IN_AON_UMSK                         (~(((1U<<AON_XTAL_HF_CAPCODE_IN_AON_LEN)-1)<<AON_XTAL_HF_CAPCODE_IN_AON_POS))
#define AON_XTAL_HF_SHORT_FILTER_AON                            AON_XTAL_HF_SHORT_FILTER_AON
#define AON_XTAL_HF_SHORT_FILTER_AON_POS                        (16U)
#define AON_XTAL_HF_SHORT_FILTER_AON_LEN                        (1U)
#define AON_XTAL_HF_SHORT_FILTER_AON_MSK                        (((1U<<AON_XTAL_HF_SHORT_FILTER_AON_LEN)-1)<<AON_XTAL_HF_SHORT_FILTER_AON_POS)
#define AON_XTAL_HF_SHORT_FILTER_AON_UMSK                       (~(((1U<<AON_XTAL_HF_SHORT_FILTER_AON_LEN)-1)<<AON_XTAL_HF_SHORT_FILTER_AON_POS))
#define AON_XTAL_HF_INJECT_EN_AON                               AON_XTAL_HF_INJECT_EN_AON
#define AON_XTAL_HF_INJECT_EN_AON_POS                           (20U)
#define AON_XTAL_HF_INJECT_EN_AON_LEN                           (1U)
#define AON_XTAL_HF_INJECT_EN_AON_MSK                           (((1U<<AON_XTAL_HF_INJECT_EN_AON_LEN)-1)<<AON_XTAL_HF_INJECT_EN_AON_POS)
#define AON_XTAL_HF_INJECT_EN_AON_UMSK                          (~(((1U<<AON_XTAL_HF_INJECT_EN_AON_LEN)-1)<<AON_XTAL_HF_INJECT_EN_AON_POS))
#define AON_XTAL_HF_RC32M_RDY_SEL_AON                           AON_XTAL_HF_RC32M_RDY_SEL_AON
#define AON_XTAL_HF_RC32M_RDY_SEL_AON_POS                       (22U)
#define AON_XTAL_HF_RC32M_RDY_SEL_AON_LEN                       (2U)
#define AON_XTAL_HF_RC32M_RDY_SEL_AON_MSK                       (((1U<<AON_XTAL_HF_RC32M_RDY_SEL_AON_LEN)-1)<<AON_XTAL_HF_RC32M_RDY_SEL_AON_POS)
#define AON_XTAL_HF_RC32M_RDY_SEL_AON_UMSK                      (~(((1U<<AON_XTAL_HF_RC32M_RDY_SEL_AON_LEN)-1)<<AON_XTAL_HF_RC32M_RDY_SEL_AON_POS))
#define AON_XTAL_HF_RC32M_RESV_AON                              AON_XTAL_HF_RC32M_RESV_AON
#define AON_XTAL_HF_RC32M_RESV_AON_POS                          (24U)
#define AON_XTAL_HF_RC32M_RESV_AON_LEN                          (8U)
#define AON_XTAL_HF_RC32M_RESV_AON_MSK                          (((1U<<AON_XTAL_HF_RC32M_RESV_AON_LEN)-1)<<AON_XTAL_HF_RC32M_RESV_AON_POS)
#define AON_XTAL_HF_RC32M_RESV_AON_UMSK                         (~(((1U<<AON_XTAL_HF_RC32M_RESV_AON_LEN)-1)<<AON_XTAL_HF_RC32M_RESV_AON_POS))

/* 0x88C : tsen */
#define AON_TSEN_OFFSET                                         (0x88C)
#define AON_TSEN_REFCODE_CORNER                                 AON_TSEN_REFCODE_CORNER
#define AON_TSEN_REFCODE_CORNER_POS                             (0U)
#define AON_TSEN_REFCODE_CORNER_LEN                             (12U)
#define AON_TSEN_REFCODE_CORNER_MSK                             (((1U<<AON_TSEN_REFCODE_CORNER_LEN)-1)<<AON_TSEN_REFCODE_CORNER_POS)
#define AON_TSEN_REFCODE_CORNER_UMSK                            (~(((1U<<AON_TSEN_REFCODE_CORNER_LEN)-1)<<AON_TSEN_REFCODE_CORNER_POS))
#define AON_TSEN_REFCODE_RFCAL                                  AON_TSEN_REFCODE_RFCAL
#define AON_TSEN_REFCODE_RFCAL_POS                              (16U)
#define AON_TSEN_REFCODE_RFCAL_LEN                              (12U)
#define AON_TSEN_REFCODE_RFCAL_MSK                              (((1U<<AON_TSEN_REFCODE_RFCAL_LEN)-1)<<AON_TSEN_REFCODE_RFCAL_POS)
#define AON_TSEN_REFCODE_RFCAL_UMSK                             (~(((1U<<AON_TSEN_REFCODE_RFCAL_LEN)-1)<<AON_TSEN_REFCODE_RFCAL_POS))
#define AON_XTAL_RDY                                            AON_XTAL_RDY
#define AON_XTAL_RDY_POS                                        (28U)
#define AON_XTAL_RDY_LEN                                        (1U)
#define AON_XTAL_RDY_MSK                                        (((1U<<AON_XTAL_RDY_LEN)-1)<<AON_XTAL_RDY_POS)
#define AON_XTAL_RDY_UMSK                                       (~(((1U<<AON_XTAL_RDY_LEN)-1)<<AON_XTAL_RDY_POS))
#define AON_XTAL_INN_CFG_EN_AON                                 AON_XTAL_INN_CFG_EN_AON
#define AON_XTAL_INN_CFG_EN_AON_POS                             (29U)
#define AON_XTAL_INN_CFG_EN_AON_LEN                             (1U)
#define AON_XTAL_INN_CFG_EN_AON_MSK                             (((1U<<AON_XTAL_INN_CFG_EN_AON_LEN)-1)<<AON_XTAL_INN_CFG_EN_AON_POS)
#define AON_XTAL_INN_CFG_EN_AON_UMSK                            (~(((1U<<AON_XTAL_INN_CFG_EN_AON_LEN)-1)<<AON_XTAL_INN_CFG_EN_AON_POS))
#define AON_XTAL_RDY_INT_SEL_AON                                AON_XTAL_RDY_INT_SEL_AON
#define AON_XTAL_RDY_INT_SEL_AON_POS                            (30U)
#define AON_XTAL_RDY_INT_SEL_AON_LEN                            (2U)
#define AON_XTAL_RDY_INT_SEL_AON_MSK                            (((1U<<AON_XTAL_RDY_INT_SEL_AON_LEN)-1)<<AON_XTAL_RDY_INT_SEL_AON_POS)
#define AON_XTAL_RDY_INT_SEL_AON_UMSK                           (~(((1U<<AON_XTAL_RDY_INT_SEL_AON_LEN)-1)<<AON_XTAL_RDY_INT_SEL_AON_POS))

/* 0x900 : acomp0_ctrl */
#define AON_ACOMP0_CTRL_OFFSET                                  (0x900)
#define AON_ACOMP0_EN                                           AON_ACOMP0_EN
#define AON_ACOMP0_EN_POS                                       (0U)
#define AON_ACOMP0_EN_LEN                                       (1U)
#define AON_ACOMP0_EN_MSK                                       (((1U<<AON_ACOMP0_EN_LEN)-1)<<AON_ACOMP0_EN_POS)
#define AON_ACOMP0_EN_UMSK                                      (~(((1U<<AON_ACOMP0_EN_LEN)-1)<<AON_ACOMP0_EN_POS))
#define AON_ACOMP0_HYST_SELN                                    AON_ACOMP0_HYST_SELN
#define AON_ACOMP0_HYST_SELN_POS                                (4U)
#define AON_ACOMP0_HYST_SELN_LEN                                (3U)
#define AON_ACOMP0_HYST_SELN_MSK                                (((1U<<AON_ACOMP0_HYST_SELN_LEN)-1)<<AON_ACOMP0_HYST_SELN_POS)
#define AON_ACOMP0_HYST_SELN_UMSK                               (~(((1U<<AON_ACOMP0_HYST_SELN_LEN)-1)<<AON_ACOMP0_HYST_SELN_POS))
#define AON_ACOMP0_HYST_SELP                                    AON_ACOMP0_HYST_SELP
#define AON_ACOMP0_HYST_SELP_POS                                (7U)
#define AON_ACOMP0_HYST_SELP_LEN                                (3U)
#define AON_ACOMP0_HYST_SELP_MSK                                (((1U<<AON_ACOMP0_HYST_SELP_LEN)-1)<<AON_ACOMP0_HYST_SELP_POS)
#define AON_ACOMP0_HYST_SELP_UMSK                               (~(((1U<<AON_ACOMP0_HYST_SELP_LEN)-1)<<AON_ACOMP0_HYST_SELP_POS))
#define AON_ACOMP0_BIAS_PROG                                    AON_ACOMP0_BIAS_PROG
#define AON_ACOMP0_BIAS_PROG_POS                                (10U)
#define AON_ACOMP0_BIAS_PROG_LEN                                (2U)
#define AON_ACOMP0_BIAS_PROG_MSK                                (((1U<<AON_ACOMP0_BIAS_PROG_LEN)-1)<<AON_ACOMP0_BIAS_PROG_POS)
#define AON_ACOMP0_BIAS_PROG_UMSK                               (~(((1U<<AON_ACOMP0_BIAS_PROG_LEN)-1)<<AON_ACOMP0_BIAS_PROG_POS))
#define AON_ACOMP0_LEVEL_SEL                                    AON_ACOMP0_LEVEL_SEL
#define AON_ACOMP0_LEVEL_SEL_POS                                (12U)
#define AON_ACOMP0_LEVEL_SEL_LEN                                (6U)
#define AON_ACOMP0_LEVEL_SEL_MSK                                (((1U<<AON_ACOMP0_LEVEL_SEL_LEN)-1)<<AON_ACOMP0_LEVEL_SEL_POS)
#define AON_ACOMP0_LEVEL_SEL_UMSK                               (~(((1U<<AON_ACOMP0_LEVEL_SEL_LEN)-1)<<AON_ACOMP0_LEVEL_SEL_POS))
#define AON_ACOMP0_NEG_SEL                                      AON_ACOMP0_NEG_SEL
#define AON_ACOMP0_NEG_SEL_POS                                  (18U)
#define AON_ACOMP0_NEG_SEL_LEN                                  (4U)
#define AON_ACOMP0_NEG_SEL_MSK                                  (((1U<<AON_ACOMP0_NEG_SEL_LEN)-1)<<AON_ACOMP0_NEG_SEL_POS)
#define AON_ACOMP0_NEG_SEL_UMSK                                 (~(((1U<<AON_ACOMP0_NEG_SEL_LEN)-1)<<AON_ACOMP0_NEG_SEL_POS))
#define AON_ACOMP0_POS_SEL                                      AON_ACOMP0_POS_SEL
#define AON_ACOMP0_POS_SEL_POS                                  (22U)
#define AON_ACOMP0_POS_SEL_LEN                                  (4U)
#define AON_ACOMP0_POS_SEL_MSK                                  (((1U<<AON_ACOMP0_POS_SEL_LEN)-1)<<AON_ACOMP0_POS_SEL_POS)
#define AON_ACOMP0_POS_SEL_UMSK                                 (~(((1U<<AON_ACOMP0_POS_SEL_LEN)-1)<<AON_ACOMP0_POS_SEL_POS))
#define AON_ACOMP0_MUXEN                                        AON_ACOMP0_MUXEN
#define AON_ACOMP0_MUXEN_POS                                    (26U)
#define AON_ACOMP0_MUXEN_LEN                                    (1U)
#define AON_ACOMP0_MUXEN_MSK                                    (((1U<<AON_ACOMP0_MUXEN_LEN)-1)<<AON_ACOMP0_MUXEN_POS)
#define AON_ACOMP0_MUXEN_UMSK                                   (~(((1U<<AON_ACOMP0_MUXEN_LEN)-1)<<AON_ACOMP0_MUXEN_POS))

/* 0x904 : acomp1_ctrl */
#define AON_ACOMP1_CTRL_OFFSET                                  (0x904)
#define AON_ACOMP1_EN                                           AON_ACOMP1_EN
#define AON_ACOMP1_EN_POS                                       (0U)
#define AON_ACOMP1_EN_LEN                                       (1U)
#define AON_ACOMP1_EN_MSK                                       (((1U<<AON_ACOMP1_EN_LEN)-1)<<AON_ACOMP1_EN_POS)
#define AON_ACOMP1_EN_UMSK                                      (~(((1U<<AON_ACOMP1_EN_LEN)-1)<<AON_ACOMP1_EN_POS))
#define AON_ACOMP1_HYST_SELN                                    AON_ACOMP1_HYST_SELN
#define AON_ACOMP1_HYST_SELN_POS                                (4U)
#define AON_ACOMP1_HYST_SELN_LEN                                (3U)
#define AON_ACOMP1_HYST_SELN_MSK                                (((1U<<AON_ACOMP1_HYST_SELN_LEN)-1)<<AON_ACOMP1_HYST_SELN_POS)
#define AON_ACOMP1_HYST_SELN_UMSK                               (~(((1U<<AON_ACOMP1_HYST_SELN_LEN)-1)<<AON_ACOMP1_HYST_SELN_POS))
#define AON_ACOMP1_HYST_SELP                                    AON_ACOMP1_HYST_SELP
#define AON_ACOMP1_HYST_SELP_POS                                (7U)
#define AON_ACOMP1_HYST_SELP_LEN                                (3U)
#define AON_ACOMP1_HYST_SELP_MSK                                (((1U<<AON_ACOMP1_HYST_SELP_LEN)-1)<<AON_ACOMP1_HYST_SELP_POS)
#define AON_ACOMP1_HYST_SELP_UMSK                               (~(((1U<<AON_ACOMP1_HYST_SELP_LEN)-1)<<AON_ACOMP1_HYST_SELP_POS))
#define AON_ACOMP1_BIAS_PROG                                    AON_ACOMP1_BIAS_PROG
#define AON_ACOMP1_BIAS_PROG_POS                                (10U)
#define AON_ACOMP1_BIAS_PROG_LEN                                (2U)
#define AON_ACOMP1_BIAS_PROG_MSK                                (((1U<<AON_ACOMP1_BIAS_PROG_LEN)-1)<<AON_ACOMP1_BIAS_PROG_POS)
#define AON_ACOMP1_BIAS_PROG_UMSK                               (~(((1U<<AON_ACOMP1_BIAS_PROG_LEN)-1)<<AON_ACOMP1_BIAS_PROG_POS))
#define AON_ACOMP1_LEVEL_SEL                                    AON_ACOMP1_LEVEL_SEL
#define AON_ACOMP1_LEVEL_SEL_POS                                (12U)
#define AON_ACOMP1_LEVEL_SEL_LEN                                (6U)
#define AON_ACOMP1_LEVEL_SEL_MSK                                (((1U<<AON_ACOMP1_LEVEL_SEL_LEN)-1)<<AON_ACOMP1_LEVEL_SEL_POS)
#define AON_ACOMP1_LEVEL_SEL_UMSK                               (~(((1U<<AON_ACOMP1_LEVEL_SEL_LEN)-1)<<AON_ACOMP1_LEVEL_SEL_POS))
#define AON_ACOMP1_NEG_SEL                                      AON_ACOMP1_NEG_SEL
#define AON_ACOMP1_NEG_SEL_POS                                  (18U)
#define AON_ACOMP1_NEG_SEL_LEN                                  (4U)
#define AON_ACOMP1_NEG_SEL_MSK                                  (((1U<<AON_ACOMP1_NEG_SEL_LEN)-1)<<AON_ACOMP1_NEG_SEL_POS)
#define AON_ACOMP1_NEG_SEL_UMSK                                 (~(((1U<<AON_ACOMP1_NEG_SEL_LEN)-1)<<AON_ACOMP1_NEG_SEL_POS))
#define AON_ACOMP1_POS_SEL                                      AON_ACOMP1_POS_SEL
#define AON_ACOMP1_POS_SEL_POS                                  (22U)
#define AON_ACOMP1_POS_SEL_LEN                                  (4U)
#define AON_ACOMP1_POS_SEL_MSK                                  (((1U<<AON_ACOMP1_POS_SEL_LEN)-1)<<AON_ACOMP1_POS_SEL_POS)
#define AON_ACOMP1_POS_SEL_UMSK                                 (~(((1U<<AON_ACOMP1_POS_SEL_LEN)-1)<<AON_ACOMP1_POS_SEL_POS))
#define AON_ACOMP1_MUXEN                                        AON_ACOMP1_MUXEN
#define AON_ACOMP1_MUXEN_POS                                    (26U)
#define AON_ACOMP1_MUXEN_LEN                                    (1U)
#define AON_ACOMP1_MUXEN_MSK                                    (((1U<<AON_ACOMP1_MUXEN_LEN)-1)<<AON_ACOMP1_MUXEN_POS)
#define AON_ACOMP1_MUXEN_UMSK                                   (~(((1U<<AON_ACOMP1_MUXEN_LEN)-1)<<AON_ACOMP1_MUXEN_POS))

/* 0x908 : acomp_ctrl */
#define AON_ACOMP_CTRL_OFFSET                                   (0x908)
#define AON_ACOMP1_RSTN_ANA                                     AON_ACOMP1_RSTN_ANA
#define AON_ACOMP1_RSTN_ANA_POS                                 (0U)
#define AON_ACOMP1_RSTN_ANA_LEN                                 (1U)
#define AON_ACOMP1_RSTN_ANA_MSK                                 (((1U<<AON_ACOMP1_RSTN_ANA_LEN)-1)<<AON_ACOMP1_RSTN_ANA_POS)
#define AON_ACOMP1_RSTN_ANA_UMSK                                (~(((1U<<AON_ACOMP1_RSTN_ANA_LEN)-1)<<AON_ACOMP1_RSTN_ANA_POS))
#define AON_ACOMP0_RSTN_ANA                                     AON_ACOMP0_RSTN_ANA
#define AON_ACOMP0_RSTN_ANA_POS                                 (1U)
#define AON_ACOMP0_RSTN_ANA_LEN                                 (1U)
#define AON_ACOMP0_RSTN_ANA_MSK                                 (((1U<<AON_ACOMP0_RSTN_ANA_LEN)-1)<<AON_ACOMP0_RSTN_ANA_POS)
#define AON_ACOMP0_RSTN_ANA_UMSK                                (~(((1U<<AON_ACOMP0_RSTN_ANA_LEN)-1)<<AON_ACOMP0_RSTN_ANA_POS))
#define AON_ACOMP1_TEST_EN                                      AON_ACOMP1_TEST_EN
#define AON_ACOMP1_TEST_EN_POS                                  (8U)
#define AON_ACOMP1_TEST_EN_LEN                                  (1U)
#define AON_ACOMP1_TEST_EN_MSK                                  (((1U<<AON_ACOMP1_TEST_EN_LEN)-1)<<AON_ACOMP1_TEST_EN_POS)
#define AON_ACOMP1_TEST_EN_UMSK                                 (~(((1U<<AON_ACOMP1_TEST_EN_LEN)-1)<<AON_ACOMP1_TEST_EN_POS))
#define AON_ACOMP0_TEST_EN                                      AON_ACOMP0_TEST_EN
#define AON_ACOMP0_TEST_EN_POS                                  (9U)
#define AON_ACOMP0_TEST_EN_LEN                                  (1U)
#define AON_ACOMP0_TEST_EN_MSK                                  (((1U<<AON_ACOMP0_TEST_EN_LEN)-1)<<AON_ACOMP0_TEST_EN_POS)
#define AON_ACOMP0_TEST_EN_UMSK                                 (~(((1U<<AON_ACOMP0_TEST_EN_LEN)-1)<<AON_ACOMP0_TEST_EN_POS))
#define AON_ACOMP1_TEST_SEL                                     AON_ACOMP1_TEST_SEL
#define AON_ACOMP1_TEST_SEL_POS                                 (10U)
#define AON_ACOMP1_TEST_SEL_LEN                                 (2U)
#define AON_ACOMP1_TEST_SEL_MSK                                 (((1U<<AON_ACOMP1_TEST_SEL_LEN)-1)<<AON_ACOMP1_TEST_SEL_POS)
#define AON_ACOMP1_TEST_SEL_UMSK                                (~(((1U<<AON_ACOMP1_TEST_SEL_LEN)-1)<<AON_ACOMP1_TEST_SEL_POS))
#define AON_ACOMP0_TEST_SEL                                     AON_ACOMP0_TEST_SEL
#define AON_ACOMP0_TEST_SEL_POS                                 (12U)
#define AON_ACOMP0_TEST_SEL_LEN                                 (2U)
#define AON_ACOMP0_TEST_SEL_MSK                                 (((1U<<AON_ACOMP0_TEST_SEL_LEN)-1)<<AON_ACOMP0_TEST_SEL_POS)
#define AON_ACOMP0_TEST_SEL_UMSK                                (~(((1U<<AON_ACOMP0_TEST_SEL_LEN)-1)<<AON_ACOMP0_TEST_SEL_POS))
#define AON_ACOMP1_OUT_RAW                                      AON_ACOMP1_OUT_RAW
#define AON_ACOMP1_OUT_RAW_POS                                  (17U)
#define AON_ACOMP1_OUT_RAW_LEN                                  (1U)
#define AON_ACOMP1_OUT_RAW_MSK                                  (((1U<<AON_ACOMP1_OUT_RAW_LEN)-1)<<AON_ACOMP1_OUT_RAW_POS)
#define AON_ACOMP1_OUT_RAW_UMSK                                 (~(((1U<<AON_ACOMP1_OUT_RAW_LEN)-1)<<AON_ACOMP1_OUT_RAW_POS))
#define AON_ACOMP0_OUT_RAW                                      AON_ACOMP0_OUT_RAW
#define AON_ACOMP0_OUT_RAW_POS                                  (19U)
#define AON_ACOMP0_OUT_RAW_LEN                                  (1U)
#define AON_ACOMP0_OUT_RAW_MSK                                  (((1U<<AON_ACOMP0_OUT_RAW_LEN)-1)<<AON_ACOMP0_OUT_RAW_POS)
#define AON_ACOMP0_OUT_RAW_UMSK                                 (~(((1U<<AON_ACOMP0_OUT_RAW_LEN)-1)<<AON_ACOMP0_OUT_RAW_POS))
#define AON_ACOMP_VREF_SEL                                      AON_ACOMP_VREF_SEL
#define AON_ACOMP_VREF_SEL_POS                                  (24U)
#define AON_ACOMP_VREF_SEL_LEN                                  (6U)
#define AON_ACOMP_VREF_SEL_MSK                                  (((1U<<AON_ACOMP_VREF_SEL_LEN)-1)<<AON_ACOMP_VREF_SEL_POS)
#define AON_ACOMP_VREF_SEL_UMSK                                 (~(((1U<<AON_ACOMP_VREF_SEL_LEN)-1)<<AON_ACOMP_VREF_SEL_POS))
#define AON_ACOMP_RESERVED                                      AON_ACOMP_RESERVED
#define AON_ACOMP_RESERVED_POS                                  (30U)
#define AON_ACOMP_RESERVED_LEN                                  (2U)
#define AON_ACOMP_RESERVED_MSK                                  (((1U<<AON_ACOMP_RESERVED_LEN)-1)<<AON_ACOMP_RESERVED_POS)
#define AON_ACOMP_RESERVED_UMSK                                 (~(((1U<<AON_ACOMP_RESERVED_LEN)-1)<<AON_ACOMP_RESERVED_POS))

/* 0x93C : hbncore_resv0 */
#define AON_HBNCORE_RESV0_OFFSET                                (0x93C)
#define AON_HBNCORE_RESV0_DATA                                  AON_HBNCORE_RESV0_DATA
#define AON_HBNCORE_RESV0_DATA_POS                              (0U)
#define AON_HBNCORE_RESV0_DATA_LEN                              (32U)
#define AON_HBNCORE_RESV0_DATA_MSK                              (((1U<<AON_HBNCORE_RESV0_DATA_LEN)-1)<<AON_HBNCORE_RESV0_DATA_POS)
#define AON_HBNCORE_RESV0_DATA_UMSK                             (~(((1U<<AON_HBNCORE_RESV0_DATA_LEN)-1)<<AON_HBNCORE_RESV0_DATA_POS))

/* 0x940 : hbncore_resv1 */
#define AON_HBNCORE_RESV1_OFFSET                                (0x940)
#define AON_HBNCORE_RESV1_DATA                                  AON_HBNCORE_RESV1_DATA
#define AON_HBNCORE_RESV1_DATA_POS                              (0U)
#define AON_HBNCORE_RESV1_DATA_LEN                              (32U)
#define AON_HBNCORE_RESV1_DATA_MSK                              (((1U<<AON_HBNCORE_RESV1_DATA_LEN)-1)<<AON_HBNCORE_RESV1_DATA_POS)
#define AON_HBNCORE_RESV1_DATA_UMSK                             (~(((1U<<AON_HBNCORE_RESV1_DATA_LEN)-1)<<AON_HBNCORE_RESV1_DATA_POS))


struct  aon_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[2048];

    /* 0x800 : aon */
    union {
        struct {
            uint32_t aon_resv                       :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_11                  :  4; /* [11: 8],       rsvd,        0x0 */
            uint32_t pu_aon_dc_tbuf                 :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_19                 :  7; /* [19:13],       rsvd,        0x0 */
            uint32_t ldo11_rt_pulldown              :  1; /* [   20],        r/w,        0x0 */
            uint32_t ldo11_rt_pulldown_sel          :  1; /* [   21],        r/w,        0x1 */
            uint32_t sw_pu_ldo11_rt                 :  1; /* [   22],        r/w,        0x1 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aon;

    /* 0x804 : aon_common */
    union {
        struct {
            uint32_t tmux_aon                       :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t ten_aon                        :  1; /* [    4],        r/w,        0x0 */
            uint32_t dten_xtal32k                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t ten_xtal32k                    :  1; /* [    6],        r/w,        0x0 */
            uint32_t pmip_dc_tp_out_en_aon          :  1; /* [    7],        r/w,        0x0 */
            uint32_t reserved_8                     :  1; /* [    8],       rsvd,        0x0 */
            uint32_t ten_ldo11soc_aon               :  1; /* [    9],        r/w,        0x0 */
            uint32_t ten_dcdc_0_aon                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t ten_dcdc_1_aon                 :  1; /* [   11],        r/w,        0x0 */
            uint32_t ten_bg_sys_aon                 :  1; /* [   12],        r/w,        0x0 */
            uint32_t ten_ldo14_aon                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ten_xtal_hf_rc32m_aon          :  1; /* [   16],        r/w,        0x0 */
            uint32_t ten_xtal_aon                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t dten_xtal_aon                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_30                 : 12; /* [30:19],       rsvd,        0x0 */
            uint32_t avdd14_rdy_aon                 :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } aon_common;

    /* 0x808 : aon_misc */
    union {
        struct {
            uint32_t sw_soc_en_aon                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aon_misc;

    /* 0x80c  reserved */
    uint8_t RESERVED0x80c[4];

    /* 0x810 : bg_sys_top */
    union {
        struct {
            uint32_t pmip_resv_aon                  :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t pu_bg_sys_aon                  :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t bg_sys_start_ctrl_aon          :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } bg_sys_top;

    /* 0x814 : dcdc18_top_0 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t dcdc_vout_sel_aon              :  5; /* [ 5: 1],        r/w,        0xb */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t dcdc_vpfm_aon                  :  4; /* [11: 8],        r/w,        0x7 */
            uint32_t dcdc_osc_2m_mode_aon           :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t dcdc_osc_freq_trim_aon         :  4; /* [19:16],        r/w,        0x8 */
            uint32_t dcdc_slope_curr_sel_aon        :  5; /* [24:20],        r/w,        0x8 */
            uint32_t dcdc_en_stop_osc_aon           :  1; /* [   25],        r/w,        0x1 */
            uint32_t dcdc_en_slow_osc_aon           :  1; /* [   26],        r/w,        0x0 */
            uint32_t dcdc_osc_inhibit_t2_aon        :  1; /* [   27],        r/w,        0x0 */
            uint32_t dcdc_sstart_time_aon           :  2; /* [29:28],        r/w,        0x1 */
            uint32_t dcdc_dis_aon                   :  1; /* [   30],        r/w,        0x0 */
            uint32_t dcdc_rdy_aon                   :  1; /* [   31],          r,        0x1 */
        }BF;
        uint32_t WORD;
    } dcdc18_top_0;

    /* 0x818 : dcdc18_top_1 */
    union {
        struct {
            uint32_t dcdc_force_en_cs_zvs_aon       :  1; /* [    0],        r/w,        0x0 */
            uint32_t dcdc_cs_delay_aon              :  3; /* [ 3: 1],        r/w,        0x4 */
            uint32_t dcdc_zvs_td_opt_aon            :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t dcdc_nonoverlap_td_aon         :  5; /* [12: 8],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t dcdc_rc_sel_aon                :  4; /* [19:16],        r/w,        0x8 */
            uint32_t dcdc_chf_sel_aon               :  4; /* [23:20],        r/w,        0x1 */
            uint32_t dcdc_cfb_sel_aon               :  4; /* [27:24],        r/w,        0x8 */
            uint32_t dcdc_en_antiring_aon           :  1; /* [   28],        r/w,        0x1 */
            uint32_t dcdc_pulldown_aon              :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dcdc18_top_1;

    /* 0x81C : dcdc_top_2 */
    union {
        struct {
            uint32_t dcdc_vout_trim_aon             :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t dcdc_osc_ss_rstn_aon           :  1; /* [    4],        r/w,        0x0 */
            uint32_t dcdc_osc_ss_en_aon             :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t dcdc_osc_ss_period_aon         :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t dcdc_osc_ss_fdev_aon           :  2; /* [11:10],        r/w,        0x0 */
            uint32_t dcdc_comp_gm_sel_aon           :  3; /* [14:12],        r/w,        0x4 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t dcdc_isense_trim_aon           :  3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t dcdc_vc_clamp_vth_aon          :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t dcdc_ocp_vth_aon               :  3; /* [26:24],        r/w,        0x4 */
            uint32_t dcdc_ocp_rst_aon               :  1; /* [   27],        r/w,        0x0 */
            uint32_t dcdc_ocp_out_aon               :  1; /* [   28],          r,        0x0 */
            uint32_t dcdc_drv_sr_aon                :  2; /* [30:29],        r/w,        0x2 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dcdc_top_2;

    /* 0x820 : ldo11soc_top */
    union {
        struct {
            uint32_t ldo11soc_vout_trim_aon         :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t pu_ldo11soc_aon                :  1; /* [    4],        r/w,        0x1 */
            uint32_t ldo11soc_sstart_sel_aon        :  1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t ldo11soc_sstart_delay_aon      :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t ldo11soc_pulldown_aon          :  1; /* [   10],        r/w,        0x0 */
            uint32_t ldo11soc_pulldown_sel_aon      :  1; /* [   11],        r/w,        0x1 */
            uint32_t ldo11soc_vth_sel_aon           :  2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_23                 : 10; /* [23:14],       rsvd,        0x0 */
            uint32_t ldo11soc_cc_aon                :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t ldo11soc_rdy_aon               :  1; /* [   28],          r,        0x1 */
            uint32_t ldo11soc_power_good_aon        :  1; /* [   29],          r,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ldo11soc_top;

    /* 0x824 : ldo14_top */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t ldo14_ocp_th_aon               :  3; /* [ 3: 1],        r/w,        0x5 */
            uint32_t ldo14_ocp_en_aon               :  1; /* [    4],        r/w,        0x1 */
            uint32_t ldo14_ocp_out_aon              :  1; /* [    5],          r,        0x0 */
            uint32_t ldo14_bm_aon                   :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t ldo14_sstart_sel_aon           :  1; /* [    8],        r/w,        0x1 */
            uint32_t ldo14_sstart_delay_aon         :  2; /* [10: 9],        r/w,        0x1 */
            uint32_t ldo14_rdy_aon                  :  1; /* [   11],          r,        0x0 */
            uint32_t ldo14_pulldown_aon             :  1; /* [   12],        r/w,        0x0 */
            uint32_t ldo14_pulldown_sel_aon         :  1; /* [   13],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ldo14_vout_sel_aon             :  3; /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19_23                 :  5; /* [23:19],       rsvd,        0x0 */
            uint32_t ldo14_cc_aon                   :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26                    :  1; /* [   26],       rsvd,        0x0 */
            uint32_t ldo14_bypass_aon               :  1; /* [   27],        r/w,        0x0 */
            uint32_t ldo14_vout_trim_aon            :  4; /* [31:28],        r/w,        0x8 */
        }BF;
        uint32_t WORD;
    } ldo14_top;

    /* 0x828 : psw_irrcv */
    union {
        struct {
            uint32_t pu_ir_psw_aon                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } psw_irrcv;

    /* 0x82c  reserved */
    uint8_t RESERVED0x82c[84];

    /* 0x880 : rf_top_aon */
    union {
        struct {
            uint32_t pu_mbg_aon                     :  1; /* [    0],        r/w,        0x0 */
            uint32_t sw_bz_en_aon                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t pu_sfreg_aon                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t pud_xtal_aon                   :  1; /* [    3],          r,        0x0 */
            uint32_t reserved_4                     :  1; /* [    4],       rsvd,        0x0 */
            uint32_t pu_xtal_aon                    :  1; /* [    5],        r/w,        0x1 */
            uint32_t pu_xtal_hf_rc32m_aon           :  1; /* [    6],        r/w,        0x1 */
            uint32_t pud_xtal_hf_rc32m_aon          :  1; /* [    7],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rf_top_aon;

    /* 0x884 : xtal_cfg */
    union {
        struct {
            uint32_t xtal_bk_aon                    :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t xtal_capcode_extra_aon         :  1; /* [    2],        r/w,        0x0 */
            uint32_t xtal_ext_sel_aon               :  1; /* [    3],        r/w,        0x0 */
            uint32_t xtal_buf_en_aon                :  4; /* [ 7: 4],        r/w,        0xf */
            uint32_t xtal_buf_hp_aon                :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reserved_12                    :  1; /* [   12],       rsvd,        0x0 */
            uint32_t xtal_sleep_aon                 :  1; /* [   13],        r/w,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t xtal_hf_rc32m_code_aon         :  8; /* [23:16],        r/w,       0x80 */
            uint32_t reserved_24_27                 :  4; /* [27:24],       rsvd,        0x0 */
            uint32_t xtal_gm_boost_aon              :  2; /* [29:28],        r/w,        0x3 */
            uint32_t xtal_rdy_sel_aon               :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } xtal_cfg;

    /* 0x888 : xtal_cfg2 */
    union {
        struct {
            uint32_t xtal_hf_capcode_out_aon        :  8; /* [ 7: 0],        r/w,       0x40 */
            uint32_t xtal_hf_capcode_in_aon         :  8; /* [15: 8],        r/w,       0x40 */
            uint32_t xtal_hf_short_filter_aon       :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t xtal_hf_inject_en_aon          :  1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21                    :  1; /* [   21],       rsvd,        0x0 */
            uint32_t xtal_hf_rc32m_rdy_sel_aon      :  2; /* [23:22],        r/w,        0x1 */
            uint32_t xtal_hf_rc32m_resv_aon         :  8; /* [31:24],        r/w,       0x10 */
        }BF;
        uint32_t WORD;
    } xtal_cfg2;

    /* 0x88C : tsen */
    union {
        struct {
            uint32_t tsen_refcode_corner            : 12; /* [11: 0],        r/w,      0x8ff */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t tsen_refcode_rfcal             : 12; /* [27:16],        r/w,      0x8ff */
            uint32_t xtal_rdy                       :  1; /* [   28],          r,        0x1 */
            uint32_t xtal_inn_cfg_en_aon            :  1; /* [   29],        r/w,        0x1 */
            uint32_t xtal_rdy_int_sel_aon           :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } tsen;

    /* 0x890  reserved */
    uint8_t RESERVED0x890[112];

    /* 0x900 : acomp0_ctrl */
    union {
        struct {
            uint32_t acomp0_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t acomp0_hyst_seln               :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t acomp0_hyst_selp               :  3; /* [ 9: 7],        r/w,        0x0 */
            uint32_t acomp0_bias_prog               :  2; /* [11:10],        r/w,        0x0 */
            uint32_t acomp0_level_sel               :  6; /* [17:12],        r/w,        0x0 */
            uint32_t acomp0_neg_sel                 :  4; /* [21:18],        r/w,        0x0 */
            uint32_t acomp0_pos_sel                 :  4; /* [25:22],        r/w,        0x0 */
            uint32_t acomp0_muxen                   :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } acomp0_ctrl;

    /* 0x904 : acomp1_ctrl */
    union {
        struct {
            uint32_t acomp1_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t acomp1_hyst_seln               :  3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t acomp1_hyst_selp               :  3; /* [ 9: 7],        r/w,        0x0 */
            uint32_t acomp1_bias_prog               :  2; /* [11:10],        r/w,        0x0 */
            uint32_t acomp1_level_sel               :  6; /* [17:12],        r/w,        0x0 */
            uint32_t acomp1_neg_sel                 :  4; /* [21:18],        r/w,        0x0 */
            uint32_t acomp1_pos_sel                 :  4; /* [25:22],        r/w,        0x0 */
            uint32_t acomp1_muxen                   :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } acomp1_ctrl;

    /* 0x908 : acomp_ctrl */
    union {
        struct {
            uint32_t acomp1_rstn_ana                :  1; /* [    0],        r/w,        0x1 */
            uint32_t acomp0_rstn_ana                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t acomp1_test_en                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t acomp0_test_en                 :  1; /* [    9],        r/w,        0x0 */
            uint32_t acomp1_test_sel                :  2; /* [11:10],        r/w,        0x0 */
            uint32_t acomp0_test_sel                :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_16                 :  3; /* [16:14],       rsvd,        0x0 */
            uint32_t acomp1_out_raw                 :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t acomp0_out_raw                 :  1; /* [   19],          r,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t acomp_vref_sel                 :  6; /* [29:24],        r/w,        0x0 */
            uint32_t acomp_reserved                 :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } acomp_ctrl;

    /* 0x90c  reserved */
    uint8_t RESERVED0x90c[48];

    /* 0x93C : hbncore_resv0 */
    union {
        struct {
            uint32_t hbncore_resv0_data             : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } hbncore_resv0;

    /* 0x940 : hbncore_resv1 */
    union {
        struct {
            uint32_t hbncore_resv1_data             : 32; /* [31: 0],        r/w, 0xffffffff */
        }BF;
        uint32_t WORD;
    } hbncore_resv1;

};

typedef volatile struct aon_reg aon_reg_t;


#endif  /* __AON_REG_H__ */
