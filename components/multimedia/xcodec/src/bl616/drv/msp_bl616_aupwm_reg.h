/**
  ******************************************************************************
  * @file    msp_aupwm_reg.h
  * @version V1.0
  * @date    2021-10-14
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
#ifndef  __MSP_BL616_AUPWM_REG_H__
#define  __MSP_BL616_AUPWM_REG_H__

#include "msp_bl616_platform.h"

/* 0x000 : aupwm_0 */
#define AUPWM_0_OFFSET                                          (0x000)
#define AUPWM_DAC_0_EN                                          AUPWM_DAC_0_EN
#define AUPWM_DAC_0_EN_POS                                      (0U)
#define AUPWM_DAC_0_EN_LEN                                      (1U)
#define AUPWM_DAC_0_EN_MSK                                      (((1U<<AUPWM_DAC_0_EN_LEN)-1)<<AUPWM_DAC_0_EN_POS)
#define AUPWM_DAC_0_EN_UMSK                                     (~(((1U<<AUPWM_DAC_0_EN_LEN)-1)<<AUPWM_DAC_0_EN_POS))
#define AUPWM_DAC_ITF_EN                                        AUPWM_DAC_ITF_EN
#define AUPWM_DAC_ITF_EN_POS                                    (1U)
#define AUPWM_DAC_ITF_EN_LEN                                    (1U)
#define AUPWM_DAC_ITF_EN_MSK                                    (((1U<<AUPWM_DAC_ITF_EN_LEN)-1)<<AUPWM_DAC_ITF_EN_POS)
#define AUPWM_DAC_ITF_EN_UMSK                                   (~(((1U<<AUPWM_DAC_ITF_EN_LEN)-1)<<AUPWM_DAC_ITF_EN_POS))
#define AUPWM_CKG_ENA                                           AUPWM_CKG_ENA
#define AUPWM_CKG_ENA_POS                                       (27U)
#define AUPWM_CKG_ENA_LEN                                       (1U)
#define AUPWM_CKG_ENA_MSK                                       (((1U<<AUPWM_CKG_ENA_LEN)-1)<<AUPWM_CKG_ENA_POS)
#define AUPWM_CKG_ENA_UMSK                                      (~(((1U<<AUPWM_CKG_ENA_LEN)-1)<<AUPWM_CKG_ENA_POS))
#define AUPWM_AU_PWM_MODE                                       AUPWM_AU_PWM_MODE
#define AUPWM_AU_PWM_MODE_POS                                   (28U)
#define AUPWM_AU_PWM_MODE_LEN                                   (4U)
#define AUPWM_AU_PWM_MODE_MSK                                   (((1U<<AUPWM_AU_PWM_MODE_LEN)-1)<<AUPWM_AU_PWM_MODE_POS)
#define AUPWM_AU_PWM_MODE_UMSK                                  (~(((1U<<AUPWM_AU_PWM_MODE_LEN)-1)<<AUPWM_AU_PWM_MODE_POS))

/* 0x4 : aupwm_status */
#define AUPWM_STATUS_OFFSET                                     (0x4)
#define AUPWM_DAC_H0_BUSY                                       AUPWM_DAC_H0_BUSY
#define AUPWM_DAC_H0_BUSY_POS                                   (12U)
#define AUPWM_DAC_H0_BUSY_LEN                                   (1U)
#define AUPWM_DAC_H0_BUSY_MSK                                   (((1U<<AUPWM_DAC_H0_BUSY_LEN)-1)<<AUPWM_DAC_H0_BUSY_POS)
#define AUPWM_DAC_H0_BUSY_UMSK                                  (~(((1U<<AUPWM_DAC_H0_BUSY_LEN)-1)<<AUPWM_DAC_H0_BUSY_POS))
#define AUPWM_DAC_H0_MUTE_DONE                                  AUPWM_DAC_H0_MUTE_DONE
#define AUPWM_DAC_H0_MUTE_DONE_POS                              (13U)
#define AUPWM_DAC_H0_MUTE_DONE_LEN                              (1U)
#define AUPWM_DAC_H0_MUTE_DONE_MSK                              (((1U<<AUPWM_DAC_H0_MUTE_DONE_LEN)-1)<<AUPWM_DAC_H0_MUTE_DONE_POS)
#define AUPWM_DAC_H0_MUTE_DONE_UMSK                             (~(((1U<<AUPWM_DAC_H0_MUTE_DONE_LEN)-1)<<AUPWM_DAC_H0_MUTE_DONE_POS))
#define AUPWM_DAC_S0_INT                                        AUPWM_DAC_S0_INT
#define AUPWM_DAC_S0_INT_POS                                    (16U)
#define AUPWM_DAC_S0_INT_LEN                                    (1U)
#define AUPWM_DAC_S0_INT_MSK                                    (((1U<<AUPWM_DAC_S0_INT_LEN)-1)<<AUPWM_DAC_S0_INT_POS)
#define AUPWM_DAC_S0_INT_UMSK                                   (~(((1U<<AUPWM_DAC_S0_INT_LEN)-1)<<AUPWM_DAC_S0_INT_POS))
#define AUPWM_DAC_S0_INT_CLR                                    AUPWM_DAC_S0_INT_CLR
#define AUPWM_DAC_S0_INT_CLR_POS                                (17U)
#define AUPWM_DAC_S0_INT_CLR_LEN                                (1U)
#define AUPWM_DAC_S0_INT_CLR_MSK                                (((1U<<AUPWM_DAC_S0_INT_CLR_LEN)-1)<<AUPWM_DAC_S0_INT_CLR_POS)
#define AUPWM_DAC_S0_INT_CLR_UMSK                               (~(((1U<<AUPWM_DAC_S0_INT_CLR_LEN)-1)<<AUPWM_DAC_S0_INT_CLR_POS))
#define AUPWM_ZD_AMUTE                                          AUPWM_ZD_AMUTE
#define AUPWM_ZD_AMUTE_POS                                      (23U)
#define AUPWM_ZD_AMUTE_LEN                                      (1U)
#define AUPWM_ZD_AMUTE_MSK                                      (((1U<<AUPWM_ZD_AMUTE_LEN)-1)<<AUPWM_ZD_AMUTE_POS)
#define AUPWM_ZD_AMUTE_UMSK                                     (~(((1U<<AUPWM_ZD_AMUTE_LEN)-1)<<AUPWM_ZD_AMUTE_POS))
#define AUPWM_AUDIO_INT_ALL                                     AUPWM_AUDIO_INT_ALL
#define AUPWM_AUDIO_INT_ALL_POS                                 (24U)
#define AUPWM_AUDIO_INT_ALL_LEN                                 (1U)
#define AUPWM_AUDIO_INT_ALL_MSK                                 (((1U<<AUPWM_AUDIO_INT_ALL_LEN)-1)<<AUPWM_AUDIO_INT_ALL_POS)
#define AUPWM_AUDIO_INT_ALL_UMSK                                (~(((1U<<AUPWM_AUDIO_INT_ALL_LEN)-1)<<AUPWM_AUDIO_INT_ALL_POS))

/* 0x8 : aupwm_s0 */
#define AUPWM_S0_OFFSET                                         (0x8)
#define AUPWM_DAC_S0_CTRL_RMP_RATE                              AUPWM_DAC_S0_CTRL_RMP_RATE
#define AUPWM_DAC_S0_CTRL_RMP_RATE_POS                          (2U)
#define AUPWM_DAC_S0_CTRL_RMP_RATE_LEN                          (4U)
#define AUPWM_DAC_S0_CTRL_RMP_RATE_MSK                          (((1U<<AUPWM_DAC_S0_CTRL_RMP_RATE_LEN)-1)<<AUPWM_DAC_S0_CTRL_RMP_RATE_POS)
#define AUPWM_DAC_S0_CTRL_RMP_RATE_UMSK                         (~(((1U<<AUPWM_DAC_S0_CTRL_RMP_RATE_LEN)-1)<<AUPWM_DAC_S0_CTRL_RMP_RATE_POS))
#define AUPWM_DAC_S0_CTRL_ZCD_RATE                              AUPWM_DAC_S0_CTRL_ZCD_RATE
#define AUPWM_DAC_S0_CTRL_ZCD_RATE_POS                          (6U)
#define AUPWM_DAC_S0_CTRL_ZCD_RATE_LEN                          (4U)
#define AUPWM_DAC_S0_CTRL_ZCD_RATE_MSK                          (((1U<<AUPWM_DAC_S0_CTRL_ZCD_RATE_LEN)-1)<<AUPWM_DAC_S0_CTRL_ZCD_RATE_POS)
#define AUPWM_DAC_S0_CTRL_ZCD_RATE_UMSK                         (~(((1U<<AUPWM_DAC_S0_CTRL_ZCD_RATE_LEN)-1)<<AUPWM_DAC_S0_CTRL_ZCD_RATE_POS))
#define AUPWM_DAC_S0_CTRL_MODE                                  AUPWM_DAC_S0_CTRL_MODE
#define AUPWM_DAC_S0_CTRL_MODE_POS                              (10U)
#define AUPWM_DAC_S0_CTRL_MODE_LEN                              (2U)
#define AUPWM_DAC_S0_CTRL_MODE_MSK                              (((1U<<AUPWM_DAC_S0_CTRL_MODE_LEN)-1)<<AUPWM_DAC_S0_CTRL_MODE_POS)
#define AUPWM_DAC_S0_CTRL_MODE_UMSK                             (~(((1U<<AUPWM_DAC_S0_CTRL_MODE_LEN)-1)<<AUPWM_DAC_S0_CTRL_MODE_POS))
#define AUPWM_DAC_S0_VOLUME_UPDATE                              AUPWM_DAC_S0_VOLUME_UPDATE
#define AUPWM_DAC_S0_VOLUME_UPDATE_POS                          (12U)
#define AUPWM_DAC_S0_VOLUME_UPDATE_LEN                          (1U)
#define AUPWM_DAC_S0_VOLUME_UPDATE_MSK                          (((1U<<AUPWM_DAC_S0_VOLUME_UPDATE_LEN)-1)<<AUPWM_DAC_S0_VOLUME_UPDATE_POS)
#define AUPWM_DAC_S0_VOLUME_UPDATE_UMSK                         (~(((1U<<AUPWM_DAC_S0_VOLUME_UPDATE_LEN)-1)<<AUPWM_DAC_S0_VOLUME_UPDATE_POS))
#define AUPWM_DAC_S0_VOLUME                                     AUPWM_DAC_S0_VOLUME
#define AUPWM_DAC_S0_VOLUME_POS                                 (13U)
#define AUPWM_DAC_S0_VOLUME_LEN                                 (9U)
#define AUPWM_DAC_S0_VOLUME_MSK                                 (((1U<<AUPWM_DAC_S0_VOLUME_LEN)-1)<<AUPWM_DAC_S0_VOLUME_POS)
#define AUPWM_DAC_S0_VOLUME_UMSK                                (~(((1U<<AUPWM_DAC_S0_VOLUME_LEN)-1)<<AUPWM_DAC_S0_VOLUME_POS))
#define AUPWM_DAC_S0_MUTE_RMPUP_RATE                            AUPWM_DAC_S0_MUTE_RMPUP_RATE
#define AUPWM_DAC_S0_MUTE_RMPUP_RATE_POS                        (22U)
#define AUPWM_DAC_S0_MUTE_RMPUP_RATE_LEN                        (4U)
#define AUPWM_DAC_S0_MUTE_RMPUP_RATE_MSK                        (((1U<<AUPWM_DAC_S0_MUTE_RMPUP_RATE_LEN)-1)<<AUPWM_DAC_S0_MUTE_RMPUP_RATE_POS)
#define AUPWM_DAC_S0_MUTE_RMPUP_RATE_UMSK                       (~(((1U<<AUPWM_DAC_S0_MUTE_RMPUP_RATE_LEN)-1)<<AUPWM_DAC_S0_MUTE_RMPUP_RATE_POS))
#define AUPWM_DAC_S0_MUTE_RMPDN_RATE                            AUPWM_DAC_S0_MUTE_RMPDN_RATE
#define AUPWM_DAC_S0_MUTE_RMPDN_RATE_POS                        (26U)
#define AUPWM_DAC_S0_MUTE_RMPDN_RATE_LEN                        (4U)
#define AUPWM_DAC_S0_MUTE_RMPDN_RATE_MSK                        (((1U<<AUPWM_DAC_S0_MUTE_RMPDN_RATE_LEN)-1)<<AUPWM_DAC_S0_MUTE_RMPDN_RATE_POS)
#define AUPWM_DAC_S0_MUTE_RMPDN_RATE_UMSK                       (~(((1U<<AUPWM_DAC_S0_MUTE_RMPDN_RATE_LEN)-1)<<AUPWM_DAC_S0_MUTE_RMPDN_RATE_POS))
#define AUPWM_DAC_S0_MUTE_SOFTMODE                              AUPWM_DAC_S0_MUTE_SOFTMODE
#define AUPWM_DAC_S0_MUTE_SOFTMODE_POS                          (30U)
#define AUPWM_DAC_S0_MUTE_SOFTMODE_LEN                          (1U)
#define AUPWM_DAC_S0_MUTE_SOFTMODE_MSK                          (((1U<<AUPWM_DAC_S0_MUTE_SOFTMODE_LEN)-1)<<AUPWM_DAC_S0_MUTE_SOFTMODE_POS)
#define AUPWM_DAC_S0_MUTE_SOFTMODE_UMSK                         (~(((1U<<AUPWM_DAC_S0_MUTE_SOFTMODE_LEN)-1)<<AUPWM_DAC_S0_MUTE_SOFTMODE_POS))
#define AUPWM_DAC_S0_MUTE                                       AUPWM_DAC_S0_MUTE
#define AUPWM_DAC_S0_MUTE_POS                                   (31U)
#define AUPWM_DAC_S0_MUTE_LEN                                   (1U)
#define AUPWM_DAC_S0_MUTE_MSK                                   (((1U<<AUPWM_DAC_S0_MUTE_LEN)-1)<<AUPWM_DAC_S0_MUTE_POS)
#define AUPWM_DAC_S0_MUTE_UMSK                                  (~(((1U<<AUPWM_DAC_S0_MUTE_LEN)-1)<<AUPWM_DAC_S0_MUTE_POS))

/* 0xC : aupwm_s0_misc */
#define AUPWM_S0_MISC_OFFSET                                    (0xC)
#define AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT                           AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT
#define AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_POS                       (28U)
#define AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_LEN                       (4U)
#define AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_MSK                       (((1U<<AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_LEN)-1)<<AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_POS)
#define AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_UMSK                      (~(((1U<<AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_LEN)-1)<<AUPWM_DAC_S0_CTRL_ZCD_TIMEOUT_POS))

/* 0x10 : aupwm_zd_0 */
#define AUPWM_ZD_0_OFFSET                                       (0x10)
#define AUPWM_ZD_TIME                                           AUPWM_ZD_TIME
#define AUPWM_ZD_TIME_POS                                       (0U)
#define AUPWM_ZD_TIME_LEN                                       (15U)
#define AUPWM_ZD_TIME_MSK                                       (((1U<<AUPWM_ZD_TIME_LEN)-1)<<AUPWM_ZD_TIME_POS)
#define AUPWM_ZD_TIME_UMSK                                      (~(((1U<<AUPWM_ZD_TIME_LEN)-1)<<AUPWM_ZD_TIME_POS))
#define AUPWM_ZD_EN                                             AUPWM_ZD_EN
#define AUPWM_ZD_EN_POS                                         (16U)
#define AUPWM_ZD_EN_LEN                                         (1U)
#define AUPWM_ZD_EN_MSK                                         (((1U<<AUPWM_ZD_EN_LEN)-1)<<AUPWM_ZD_EN_POS)
#define AUPWM_ZD_EN_UMSK                                        (~(((1U<<AUPWM_ZD_EN_LEN)-1)<<AUPWM_ZD_EN_POS))

/* 0x14 : aupwm_1 */
#define AUPWM_1_OFFSET                                          (0x14)
#define AUPWM_DAC_MIX_SEL                                       AUPWM_DAC_MIX_SEL
#define AUPWM_DAC_MIX_SEL_POS                                   (0U)
#define AUPWM_DAC_MIX_SEL_LEN                                   (2U)
#define AUPWM_DAC_MIX_SEL_MSK                                   (((1U<<AUPWM_DAC_MIX_SEL_LEN)-1)<<AUPWM_DAC_MIX_SEL_POS)
#define AUPWM_DAC_MIX_SEL_UMSK                                  (~(((1U<<AUPWM_DAC_MIX_SEL_LEN)-1)<<AUPWM_DAC_MIX_SEL_POS))
#define AUPWM_DAC_DSM_OUT_FMT                                   AUPWM_DAC_DSM_OUT_FMT
#define AUPWM_DAC_DSM_OUT_FMT_POS                               (4U)
#define AUPWM_DAC_DSM_OUT_FMT_LEN                               (1U)
#define AUPWM_DAC_DSM_OUT_FMT_MSK                               (((1U<<AUPWM_DAC_DSM_OUT_FMT_LEN)-1)<<AUPWM_DAC_DSM_OUT_FMT_POS)
#define AUPWM_DAC_DSM_OUT_FMT_UMSK                              (~(((1U<<AUPWM_DAC_DSM_OUT_FMT_LEN)-1)<<AUPWM_DAC_DSM_OUT_FMT_POS))
#define AUPWM_DAC_DSM_ORDER                                     AUPWM_DAC_DSM_ORDER
#define AUPWM_DAC_DSM_ORDER_POS                                 (5U)
#define AUPWM_DAC_DSM_ORDER_LEN                                 (2U)
#define AUPWM_DAC_DSM_ORDER_MSK                                 (((1U<<AUPWM_DAC_DSM_ORDER_LEN)-1)<<AUPWM_DAC_DSM_ORDER_POS)
#define AUPWM_DAC_DSM_ORDER_UMSK                                (~(((1U<<AUPWM_DAC_DSM_ORDER_LEN)-1)<<AUPWM_DAC_DSM_ORDER_POS))
#define AUPWM_DAC_DSM_SCALING_MODE                              AUPWM_DAC_DSM_SCALING_MODE
#define AUPWM_DAC_DSM_SCALING_MODE_POS                          (7U)
#define AUPWM_DAC_DSM_SCALING_MODE_LEN                          (2U)
#define AUPWM_DAC_DSM_SCALING_MODE_MSK                          (((1U<<AUPWM_DAC_DSM_SCALING_MODE_LEN)-1)<<AUPWM_DAC_DSM_SCALING_MODE_POS)
#define AUPWM_DAC_DSM_SCALING_MODE_UMSK                         (~(((1U<<AUPWM_DAC_DSM_SCALING_MODE_LEN)-1)<<AUPWM_DAC_DSM_SCALING_MODE_POS))
#define AUPWM_DAC_DSM_SCALING_EN                                AUPWM_DAC_DSM_SCALING_EN
#define AUPWM_DAC_DSM_SCALING_EN_POS                            (10U)
#define AUPWM_DAC_DSM_SCALING_EN_LEN                            (1U)
#define AUPWM_DAC_DSM_SCALING_EN_MSK                            (((1U<<AUPWM_DAC_DSM_SCALING_EN_LEN)-1)<<AUPWM_DAC_DSM_SCALING_EN_POS)
#define AUPWM_DAC_DSM_SCALING_EN_UMSK                           (~(((1U<<AUPWM_DAC_DSM_SCALING_EN_LEN)-1)<<AUPWM_DAC_DSM_SCALING_EN_POS))
#define AUPWM_DAC_DSM_DITHER_AMP                                AUPWM_DAC_DSM_DITHER_AMP
#define AUPWM_DAC_DSM_DITHER_AMP_POS                            (11U)
#define AUPWM_DAC_DSM_DITHER_AMP_LEN                            (3U)
#define AUPWM_DAC_DSM_DITHER_AMP_MSK                            (((1U<<AUPWM_DAC_DSM_DITHER_AMP_LEN)-1)<<AUPWM_DAC_DSM_DITHER_AMP_POS)
#define AUPWM_DAC_DSM_DITHER_AMP_UMSK                           (~(((1U<<AUPWM_DAC_DSM_DITHER_AMP_LEN)-1)<<AUPWM_DAC_DSM_DITHER_AMP_POS))
#define AUPWM_DAC_DSM_DITHER_EN                                 AUPWM_DAC_DSM_DITHER_EN
#define AUPWM_DAC_DSM_DITHER_EN_POS                             (14U)
#define AUPWM_DAC_DSM_DITHER_EN_LEN                             (1U)
#define AUPWM_DAC_DSM_DITHER_EN_MSK                             (((1U<<AUPWM_DAC_DSM_DITHER_EN_LEN)-1)<<AUPWM_DAC_DSM_DITHER_EN_POS)
#define AUPWM_DAC_DSM_DITHER_EN_UMSK                            (~(((1U<<AUPWM_DAC_DSM_DITHER_EN_LEN)-1)<<AUPWM_DAC_DSM_DITHER_EN_POS))
#define AUPWM_DAC_DSM_DITHER_PRBS_MODE                          AUPWM_DAC_DSM_DITHER_PRBS_MODE
#define AUPWM_DAC_DSM_DITHER_PRBS_MODE_POS                      (15U)
#define AUPWM_DAC_DSM_DITHER_PRBS_MODE_LEN                      (2U)
#define AUPWM_DAC_DSM_DITHER_PRBS_MODE_MSK                      (((1U<<AUPWM_DAC_DSM_DITHER_PRBS_MODE_LEN)-1)<<AUPWM_DAC_DSM_DITHER_PRBS_MODE_POS)
#define AUPWM_DAC_DSM_DITHER_PRBS_MODE_UMSK                     (~(((1U<<AUPWM_DAC_DSM_DITHER_PRBS_MODE_LEN)-1)<<AUPWM_DAC_DSM_DITHER_PRBS_MODE_POS))

/* 0x18 : aupwm_rsvd */
#define AUPWM_RSVD_OFFSET                                       (0x18)
#define AUPWM_AU_PWM_RESERVED                                   AUPWM_AU_PWM_RESERVED
#define AUPWM_AU_PWM_RESERVED_POS                               (0U)
#define AUPWM_AU_PWM_RESERVED_LEN                               (32U)
#define AUPWM_AU_PWM_RESERVED_MSK                               (((1U<<AUPWM_AU_PWM_RESERVED_LEN)-1)<<AUPWM_AU_PWM_RESERVED_POS)
#define AUPWM_AU_PWM_RESERVED_UMSK                              (~(((1U<<AUPWM_AU_PWM_RESERVED_LEN)-1)<<AUPWM_AU_PWM_RESERVED_POS))

/* 0x1C : aupwm_test_0 */
#define AUPWM_TEST_0_OFFSET                                     (0x1C)
#define AUPWM_DAC_IN_0                                          AUPWM_DAC_IN_0
#define AUPWM_DAC_IN_0_POS                                      (0U)
#define AUPWM_DAC_IN_0_LEN                                      (16U)
#define AUPWM_DAC_IN_0_MSK                                      (((1U<<AUPWM_DAC_IN_0_LEN)-1)<<AUPWM_DAC_IN_0_POS)
#define AUPWM_DAC_IN_0_UMSK                                     (~(((1U<<AUPWM_DAC_IN_0_LEN)-1)<<AUPWM_DAC_IN_0_POS))
#define AUPWM_DAC_DPGA_0                                        AUPWM_DAC_DPGA_0
#define AUPWM_DAC_DPGA_0_POS                                    (16U)
#define AUPWM_DAC_DPGA_0_LEN                                    (16U)
#define AUPWM_DAC_DPGA_0_MSK                                    (((1U<<AUPWM_DAC_DPGA_0_LEN)-1)<<AUPWM_DAC_DPGA_0_POS)
#define AUPWM_DAC_DPGA_0_UMSK                                   (~(((1U<<AUPWM_DAC_DPGA_0_LEN)-1)<<AUPWM_DAC_DPGA_0_POS))

/* 0x20 : aupwm_test_1 */
#define AUPWM_TEST_1_OFFSET                                     (0x20)
#define AUPWM_DAC_FIR_0                                         AUPWM_DAC_FIR_0
#define AUPWM_DAC_FIR_0_POS                                     (0U)
#define AUPWM_DAC_FIR_0_LEN                                     (17U)
#define AUPWM_DAC_FIR_0_MSK                                     (((1U<<AUPWM_DAC_FIR_0_LEN)-1)<<AUPWM_DAC_FIR_0_POS)
#define AUPWM_DAC_FIR_0_UMSK                                    (~(((1U<<AUPWM_DAC_FIR_0_LEN)-1)<<AUPWM_DAC_FIR_0_POS))

/* 0x24 : aupwm_test_2 */
#define AUPWM_TEST_2_OFFSET                                     (0x24)
#define AUPWM_DAC_SINC_0                                        AUPWM_DAC_SINC_0
#define AUPWM_DAC_SINC_0_POS                                    (0U)
#define AUPWM_DAC_SINC_0_LEN                                    (16U)
#define AUPWM_DAC_SINC_0_MSK                                    (((1U<<AUPWM_DAC_SINC_0_LEN)-1)<<AUPWM_DAC_SINC_0_POS)
#define AUPWM_DAC_SINC_0_UMSK                                   (~(((1U<<AUPWM_DAC_SINC_0_LEN)-1)<<AUPWM_DAC_SINC_0_POS))

/* 0x28 : aupwm_test_3 */
#define AUPWM_TEST_3_OFFSET                                     (0x28)
#define AUPWM_AU_PWM_TEST_READ                                  AUPWM_AU_PWM_TEST_READ
#define AUPWM_AU_PWM_TEST_READ_POS                              (0U)
#define AUPWM_AU_PWM_TEST_READ_LEN                              (32U)
#define AUPWM_AU_PWM_TEST_READ_MSK                              (((1U<<AUPWM_AU_PWM_TEST_READ_LEN)-1)<<AUPWM_AU_PWM_TEST_READ_POS)
#define AUPWM_AU_PWM_TEST_READ_UMSK                             (~(((1U<<AUPWM_AU_PWM_TEST_READ_LEN)-1)<<AUPWM_AU_PWM_TEST_READ_POS))

/* 0x8C : aupwm_fifo_ctrl */
#define AUPWM_FIFO_CTRL_OFFSET                                  (0x8C)
#define AUPWM_TX_FIFO_FLUSH                                     AUPWM_TX_FIFO_FLUSH
#define AUPWM_TX_FIFO_FLUSH_POS                                 (0U)
#define AUPWM_TX_FIFO_FLUSH_LEN                                 (1U)
#define AUPWM_TX_FIFO_FLUSH_MSK                                 (((1U<<AUPWM_TX_FIFO_FLUSH_LEN)-1)<<AUPWM_TX_FIFO_FLUSH_POS)
#define AUPWM_TX_FIFO_FLUSH_UMSK                                (~(((1U<<AUPWM_TX_FIFO_FLUSH_LEN)-1)<<AUPWM_TX_FIFO_FLUSH_POS))
#define AUPWM_TXO_INT_EN                                        AUPWM_TXO_INT_EN
#define AUPWM_TXO_INT_EN_POS                                    (1U)
#define AUPWM_TXO_INT_EN_LEN                                    (1U)
#define AUPWM_TXO_INT_EN_MSK                                    (((1U<<AUPWM_TXO_INT_EN_LEN)-1)<<AUPWM_TXO_INT_EN_POS)
#define AUPWM_TXO_INT_EN_UMSK                                   (~(((1U<<AUPWM_TXO_INT_EN_LEN)-1)<<AUPWM_TXO_INT_EN_POS))
#define AUPWM_TXU_INT_EN                                        AUPWM_TXU_INT_EN
#define AUPWM_TXU_INT_EN_POS                                    (2U)
#define AUPWM_TXU_INT_EN_LEN                                    (1U)
#define AUPWM_TXU_INT_EN_MSK                                    (((1U<<AUPWM_TXU_INT_EN_LEN)-1)<<AUPWM_TXU_INT_EN_POS)
#define AUPWM_TXU_INT_EN_UMSK                                   (~(((1U<<AUPWM_TXU_INT_EN_LEN)-1)<<AUPWM_TXU_INT_EN_POS))
#define AUPWM_TXA_INT_EN                                        AUPWM_TXA_INT_EN
#define AUPWM_TXA_INT_EN_POS                                    (3U)
#define AUPWM_TXA_INT_EN_LEN                                    (1U)
#define AUPWM_TXA_INT_EN_MSK                                    (((1U<<AUPWM_TXA_INT_EN_LEN)-1)<<AUPWM_TXA_INT_EN_POS)
#define AUPWM_TXA_INT_EN_UMSK                                   (~(((1U<<AUPWM_TXA_INT_EN_LEN)-1)<<AUPWM_TXA_INT_EN_POS))
#define AUPWM_TX_DRQ_EN                                         AUPWM_TX_DRQ_EN
#define AUPWM_TX_DRQ_EN_POS                                     (4U)
#define AUPWM_TX_DRQ_EN_LEN                                     (1U)
#define AUPWM_TX_DRQ_EN_MSK                                     (((1U<<AUPWM_TX_DRQ_EN_LEN)-1)<<AUPWM_TX_DRQ_EN_POS)
#define AUPWM_TX_DRQ_EN_UMSK                                    (~(((1U<<AUPWM_TX_DRQ_EN_LEN)-1)<<AUPWM_TX_DRQ_EN_POS))
#define AUPWM_TX_CH_EN                                          AUPWM_TX_CH_EN
#define AUPWM_TX_CH_EN_POS                                      (8U)
#define AUPWM_TX_CH_EN_LEN                                      (2U)
#define AUPWM_TX_CH_EN_MSK                                      (((1U<<AUPWM_TX_CH_EN_LEN)-1)<<AUPWM_TX_CH_EN_POS)
#define AUPWM_TX_CH_EN_UMSK                                     (~(((1U<<AUPWM_TX_CH_EN_LEN)-1)<<AUPWM_TX_CH_EN_POS))
#define AUPWM_TX_DRQ_CNT                                        AUPWM_TX_DRQ_CNT
#define AUPWM_TX_DRQ_CNT_POS                                    (14U)
#define AUPWM_TX_DRQ_CNT_LEN                                    (2U)
#define AUPWM_TX_DRQ_CNT_MSK                                    (((1U<<AUPWM_TX_DRQ_CNT_LEN)-1)<<AUPWM_TX_DRQ_CNT_POS)
#define AUPWM_TX_DRQ_CNT_UMSK                                   (~(((1U<<AUPWM_TX_DRQ_CNT_LEN)-1)<<AUPWM_TX_DRQ_CNT_POS))
#define AUPWM_TX_TRG_LEVEL                                      AUPWM_TX_TRG_LEVEL
#define AUPWM_TX_TRG_LEVEL_POS                                  (16U)
#define AUPWM_TX_TRG_LEVEL_LEN                                  (6U)
#define AUPWM_TX_TRG_LEVEL_MSK                                  (((1U<<AUPWM_TX_TRG_LEVEL_LEN)-1)<<AUPWM_TX_TRG_LEVEL_POS)
#define AUPWM_TX_TRG_LEVEL_UMSK                                 (~(((1U<<AUPWM_TX_TRG_LEVEL_LEN)-1)<<AUPWM_TX_TRG_LEVEL_POS))
#define AUPWM_TX_DATA_MODE                                      AUPWM_TX_DATA_MODE
#define AUPWM_TX_DATA_MODE_POS                                  (24U)
#define AUPWM_TX_DATA_MODE_LEN                                  (2U)
#define AUPWM_TX_DATA_MODE_MSK                                  (((1U<<AUPWM_TX_DATA_MODE_LEN)-1)<<AUPWM_TX_DATA_MODE_POS)
#define AUPWM_TX_DATA_MODE_UMSK                                 (~(((1U<<AUPWM_TX_DATA_MODE_LEN)-1)<<AUPWM_TX_DATA_MODE_POS))

/* 0x90 : aupwm_fifo_status */
#define AUPWM_FIFO_STATUS_OFFSET                                (0x90)
#define AUPWM_TXO_INT                                           AUPWM_TXO_INT
#define AUPWM_TXO_INT_POS                                       (1U)
#define AUPWM_TXO_INT_LEN                                       (1U)
#define AUPWM_TXO_INT_MSK                                       (((1U<<AUPWM_TXO_INT_LEN)-1)<<AUPWM_TXO_INT_POS)
#define AUPWM_TXO_INT_UMSK                                      (~(((1U<<AUPWM_TXO_INT_LEN)-1)<<AUPWM_TXO_INT_POS))
#define AUPWM_TXU_INT                                           AUPWM_TXU_INT
#define AUPWM_TXU_INT_POS                                       (2U)
#define AUPWM_TXU_INT_LEN                                       (1U)
#define AUPWM_TXU_INT_MSK                                       (((1U<<AUPWM_TXU_INT_LEN)-1)<<AUPWM_TXU_INT_POS)
#define AUPWM_TXU_INT_UMSK                                      (~(((1U<<AUPWM_TXU_INT_LEN)-1)<<AUPWM_TXU_INT_POS))
#define AUPWM_TXA_INT                                           AUPWM_TXA_INT
#define AUPWM_TXA_INT_POS                                       (4U)
#define AUPWM_TXA_INT_LEN                                       (1U)
#define AUPWM_TXA_INT_MSK                                       (((1U<<AUPWM_TXA_INT_LEN)-1)<<AUPWM_TXA_INT_POS)
#define AUPWM_TXA_INT_UMSK                                      (~(((1U<<AUPWM_TXA_INT_LEN)-1)<<AUPWM_TXA_INT_POS))
#define AUPWM_TXA_CNT                                           AUPWM_TXA_CNT
#define AUPWM_TXA_CNT_POS                                       (16U)
#define AUPWM_TXA_CNT_LEN                                       (6U)
#define AUPWM_TXA_CNT_MSK                                       (((1U<<AUPWM_TXA_CNT_LEN)-1)<<AUPWM_TXA_CNT_POS)
#define AUPWM_TXA_CNT_UMSK                                      (~(((1U<<AUPWM_TXA_CNT_LEN)-1)<<AUPWM_TXA_CNT_POS))
#define AUPWM_TXA                                               AUPWM_TXA
#define AUPWM_TXA_POS                                           (24U)
#define AUPWM_TXA_LEN                                           (1U)
#define AUPWM_TXA_MSK                                           (((1U<<AUPWM_TXA_LEN)-1)<<AUPWM_TXA_POS)
#define AUPWM_TXA_UMSK                                          (~(((1U<<AUPWM_TXA_LEN)-1)<<AUPWM_TXA_POS))

/* 0x94 : aupwm_fifo_data */
#define AUPWM_FIFO_DATA_OFFSET                                  (0x94)
#define AUPWM_TX_DATA                                           AUPWM_TX_DATA
#define AUPWM_TX_DATA_POS                                       (0U)
#define AUPWM_TX_DATA_LEN                                       (32U)
#define AUPWM_TX_DATA_MSK                                       (((1U<<AUPWM_TX_DATA_LEN)-1)<<AUPWM_TX_DATA_POS)
#define AUPWM_TX_DATA_UMSK                                      (~(((1U<<AUPWM_TX_DATA_LEN)-1)<<AUPWM_TX_DATA_POS))


struct  aupwm_reg {
    /* 0x000 : aupwm_0 */
    union {
        struct {
            uint32_t dac_0_en                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t dac_itf_en                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_26                  : 25; /* [26: 2],       rsvd,        0x0 */
            uint32_t ckg_ena                        :  1; /* [   27],        r/w,        0x1 */
            uint32_t au_pwm_mode                    :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_0;

    /* 0x4 : aupwm_status */
    union {
        struct {
            uint32_t reserved_0_11                  : 12; /* [11: 0],       rsvd,        0x0 */
            uint32_t dac_h0_busy                    :  1; /* [   12],          r,        0x0 */
            uint32_t dac_h0_mute_done               :  1; /* [   13],          r,        0x1 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t dac_s0_int                     :  1; /* [   16],          r,        0x0 */
            uint32_t dac_s0_int_clr                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_22                 :  5; /* [22:18],       rsvd,        0x0 */
            uint32_t zd_amute                       :  1; /* [   23],          r,        0x0 */
            uint32_t audio_int_all                  :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_status;

    /* 0x8 : aupwm_s0 */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t dac_s0_ctrl_rmp_rate           :  4; /* [ 5: 2],        r/w,        0x6 */
            uint32_t dac_s0_ctrl_zcd_rate           :  4; /* [ 9: 6],        r/w,        0x2 */
            uint32_t dac_s0_ctrl_mode               :  2; /* [11:10],        r/w,        0x2 */
            uint32_t dac_s0_volume_update           :  1; /* [   12],        r/w,        0x0 */
            uint32_t dac_s0_volume                  :  9; /* [21:13],        r/w,        0x0 */
            uint32_t dac_s0_mute_rmpup_rate         :  4; /* [25:22],        r/w,        0x0 */
            uint32_t dac_s0_mute_rmpdn_rate         :  4; /* [29:26],        r/w,        0x6 */
            uint32_t dac_s0_mute_softmode           :  1; /* [   30],        r/w,        0x1 */
            uint32_t dac_s0_mute                    :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_s0;

    /* 0xC : aupwm_s0_misc */
    union {
        struct {
            uint32_t reserved_0_27                  : 28; /* [27: 0],       rsvd,        0x0 */
            uint32_t dac_s0_ctrl_zcd_timeout        :  4; /* [31:28],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } aupwm_s0_misc;

    /* 0x10 : aupwm_zd_0 */
    union {
        struct {
            uint32_t zd_time                        : 15; /* [14: 0],        r/w,      0x200 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t zd_en                          :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_zd_0;

    /* 0x14 : aupwm_1 */
    union {
        struct {
            uint32_t dac_mix_sel                    :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t dac_dsm_out_fmt                :  1; /* [    4],        r/w,        0x0 */
            uint32_t dac_dsm_order                  :  2; /* [ 6: 5],        r/w,        0x0 */
            uint32_t dac_dsm_scaling_mode           :  2; /* [ 8: 7],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t dac_dsm_scaling_en             :  1; /* [   10],        r/w,        0x1 */
            uint32_t dac_dsm_dither_amp             :  3; /* [13:11],        r/w,        0x0 */
            uint32_t dac_dsm_dither_en              :  1; /* [   14],        r/w,        0x1 */
            uint32_t dac_dsm_dither_prbs_mode       :  2; /* [16:15],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_1;

    /* 0x18 : aupwm_rsvd */
    union {
        struct {
            uint32_t au_pwm_reserved                : 32; /* [31: 0],        r/w, 0xffff0000 */
        }BF;
        uint32_t WORD;
    } aupwm_rsvd;

    /* 0x1C : aupwm_test_0 */
    union {
        struct {
            uint32_t dac_in_0                       : 16; /* [15: 0],          r,        0x0 */
            uint32_t dac_dpga_0                     : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_test_0;

    /* 0x20 : aupwm_test_1 */
    union {
        struct {
            uint32_t dac_fir_0                      : 17; /* [16: 0],          r,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_test_1;

    /* 0x24 : aupwm_test_2 */
    union {
        struct {
            uint32_t dac_sinc_0                     : 16; /* [15: 0],          r,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_test_2;

    /* 0x28 : aupwm_test_3 */
    union {
        struct {
            uint32_t au_pwm_test_read               : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_test_3;

    /* 0x2c  reserved */
    uint8_t RESERVED0x2c[96];

    /* 0x8C : aupwm_fifo_ctrl */
    union {
        struct {
            uint32_t tx_fifo_flush                  :  1; /* [    0],        w1c,        0x0 */
            uint32_t txo_int_en                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t txu_int_en                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t txa_int_en                     :  1; /* [    3],        r/w,        0x0 */
            uint32_t tx_drq_en                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t tx_ch_en                       :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_13                 :  4; /* [13:10],       rsvd,        0x0 */
            uint32_t tx_drq_cnt                     :  2; /* [15:14],        r/w,        0x0 */
            uint32_t tx_trg_level                   :  6; /* [21:16],        r/w,        0xf */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t tx_data_mode                   :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_fifo_ctrl;

    /* 0x90 : aupwm_fifo_status */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t txo_int                        :  1; /* [    1],          r,        0x0 */
            uint32_t txu_int                        :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t txa_int                        :  1; /* [    4],          r,        0x0 */
            uint32_t reserved_5_15                  : 11; /* [15: 5],       rsvd,        0x0 */
            uint32_t txa_cnt                        :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t txa                            :  1; /* [   24],          r,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_fifo_status;

    /* 0x94 : aupwm_fifo_data */
    union {
        struct {
            uint32_t tx_data                        : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } aupwm_fifo_data;

};

typedef volatile struct aupwm_reg aupwm_reg_t;


#endif  /* __AUPWM_REG_H__ */
