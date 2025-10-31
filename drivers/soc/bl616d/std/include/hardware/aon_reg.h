/**
  ******************************************************************************
  * @file    aon_reg.h
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
#ifndef __AON_REG_H__
#define __AON_REG_H__

#include "bl616d.h"

/* 0x800 : aon */
#define AON_OFFSET                               (0x800)
#define AON_RESV                                 AON_RESV
#define AON_RESV_POS                             (0U)
#define AON_RESV_LEN                             (8U)
#define AON_RESV_MSK                             (((1U << AON_RESV_LEN) - 1) << AON_RESV_POS)
#define AON_RESV_UMSK                            (~(((1U << AON_RESV_LEN) - 1) << AON_RESV_POS))
#define AON_PU_AON_DC_TBUF                       AON_PU_AON_DC_TBUF
#define AON_PU_AON_DC_TBUF_POS                   (12U)
#define AON_PU_AON_DC_TBUF_LEN                   (1U)
#define AON_PU_AON_DC_TBUF_MSK                   (((1U << AON_PU_AON_DC_TBUF_LEN) - 1) << AON_PU_AON_DC_TBUF_POS)
#define AON_PU_AON_DC_TBUF_UMSK                  (~(((1U << AON_PU_AON_DC_TBUF_LEN) - 1) << AON_PU_AON_DC_TBUF_POS))
#define AON_LDO11_RT_PULLDOWN                    AON_LDO11_RT_PULLDOWN
#define AON_LDO11_RT_PULLDOWN_POS                (20U)
#define AON_LDO11_RT_PULLDOWN_LEN                (1U)
#define AON_LDO11_RT_PULLDOWN_MSK                (((1U << AON_LDO11_RT_PULLDOWN_LEN) - 1) << AON_LDO11_RT_PULLDOWN_POS)
#define AON_LDO11_RT_PULLDOWN_UMSK               (~(((1U << AON_LDO11_RT_PULLDOWN_LEN) - 1) << AON_LDO11_RT_PULLDOWN_POS))
#define AON_LDO11_RT_PULLDOWN_SEL                AON_LDO11_RT_PULLDOWN_SEL
#define AON_LDO11_RT_PULLDOWN_SEL_POS            (21U)
#define AON_LDO11_RT_PULLDOWN_SEL_LEN            (1U)
#define AON_LDO11_RT_PULLDOWN_SEL_MSK            (((1U << AON_LDO11_RT_PULLDOWN_SEL_LEN) - 1) << AON_LDO11_RT_PULLDOWN_SEL_POS)
#define AON_LDO11_RT_PULLDOWN_SEL_UMSK           (~(((1U << AON_LDO11_RT_PULLDOWN_SEL_LEN) - 1) << AON_LDO11_RT_PULLDOWN_SEL_POS))
#define AON_SW_PU_LDO11_RT                       AON_SW_PU_LDO11_RT
#define AON_SW_PU_LDO11_RT_POS                   (22U)
#define AON_SW_PU_LDO11_RT_LEN                   (1U)
#define AON_SW_PU_LDO11_RT_MSK                   (((1U << AON_SW_PU_LDO11_RT_LEN) - 1) << AON_SW_PU_LDO11_RT_POS)
#define AON_SW_PU_LDO11_RT_UMSK                  (~(((1U << AON_SW_PU_LDO11_RT_LEN) - 1) << AON_SW_PU_LDO11_RT_POS))

/* 0x804 : aon_common */
#define AON_COMMON_OFFSET                        (0x804)
#define AON_TEN_VDDCORE_AON                      AON_TEN_VDDCORE_AON
#define AON_TEN_VDDCORE_AON_POS                  (8U)
#define AON_TEN_VDDCORE_AON_LEN                  (1U)
#define AON_TEN_VDDCORE_AON_MSK                  (((1U << AON_TEN_VDDCORE_AON_LEN) - 1) << AON_TEN_VDDCORE_AON_POS)
#define AON_TEN_VDDCORE_AON_UMSK                 (~(((1U << AON_TEN_VDDCORE_AON_LEN) - 1) << AON_TEN_VDDCORE_AON_POS))
#define AON_TEN_LDO11SOC_AON                     AON_TEN_LDO11SOC_AON
#define AON_TEN_LDO11SOC_AON_POS                 (9U)
#define AON_TEN_LDO11SOC_AON_LEN                 (1U)
#define AON_TEN_LDO11SOC_AON_MSK                 (((1U << AON_TEN_LDO11SOC_AON_LEN) - 1) << AON_TEN_LDO11SOC_AON_POS)
#define AON_TEN_LDO11SOC_AON_UMSK                (~(((1U << AON_TEN_LDO11SOC_AON_LEN) - 1) << AON_TEN_LDO11SOC_AON_POS))
#define AON_TEN_DCDC_0_AON                       AON_TEN_DCDC_0_AON
#define AON_TEN_DCDC_0_AON_POS                   (10U)
#define AON_TEN_DCDC_0_AON_LEN                   (1U)
#define AON_TEN_DCDC_0_AON_MSK                   (((1U << AON_TEN_DCDC_0_AON_LEN) - 1) << AON_TEN_DCDC_0_AON_POS)
#define AON_TEN_DCDC_0_AON_UMSK                  (~(((1U << AON_TEN_DCDC_0_AON_LEN) - 1) << AON_TEN_DCDC_0_AON_POS))
#define AON_TEN_DCDC_1_AON                       AON_TEN_DCDC_1_AON
#define AON_TEN_DCDC_1_AON_POS                   (11U)
#define AON_TEN_DCDC_1_AON_LEN                   (1U)
#define AON_TEN_DCDC_1_AON_MSK                   (((1U << AON_TEN_DCDC_1_AON_LEN) - 1) << AON_TEN_DCDC_1_AON_POS)
#define AON_TEN_DCDC_1_AON_UMSK                  (~(((1U << AON_TEN_DCDC_1_AON_LEN) - 1) << AON_TEN_DCDC_1_AON_POS))
#define AON_TEN_BG_SYS_AON                       AON_TEN_BG_SYS_AON
#define AON_TEN_BG_SYS_AON_POS                   (12U)
#define AON_TEN_BG_SYS_AON_LEN                   (1U)
#define AON_TEN_BG_SYS_AON_MSK                   (((1U << AON_TEN_BG_SYS_AON_LEN) - 1) << AON_TEN_BG_SYS_AON_POS)
#define AON_TEN_BG_SYS_AON_UMSK                  (~(((1U << AON_TEN_BG_SYS_AON_LEN) - 1) << AON_TEN_BG_SYS_AON_POS))
#define AON_TEN_LDO15RF_AON                      AON_TEN_LDO15RF_AON
#define AON_TEN_LDO15RF_AON_POS                  (16U)
#define AON_TEN_LDO15RF_AON_LEN                  (1U)
#define AON_TEN_LDO15RF_AON_MSK                  (((1U << AON_TEN_LDO15RF_AON_LEN) - 1) << AON_TEN_LDO15RF_AON_POS)
#define AON_TEN_LDO15RF_AON_UMSK                 (~(((1U << AON_TEN_LDO15RF_AON_LEN) - 1) << AON_TEN_LDO15RF_AON_POS))
#define AON_TEN_CIP_MISC_AON                     AON_TEN_CIP_MISC_AON
#define AON_TEN_CIP_MISC_AON_POS                 (20U)
#define AON_TEN_CIP_MISC_AON_LEN                 (1U)
#define AON_TEN_CIP_MISC_AON_MSK                 (((1U << AON_TEN_CIP_MISC_AON_LEN) - 1) << AON_TEN_CIP_MISC_AON_POS)
#define AON_TEN_CIP_MISC_AON_UMSK                (~(((1U << AON_TEN_CIP_MISC_AON_LEN) - 1) << AON_TEN_CIP_MISC_AON_POS))
#define AON_DTEST_PULLDOWN_AON                   AON_DTEST_PULLDOWN_AON
#define AON_DTEST_PULLDOWN_AON_POS               (22U)
#define AON_DTEST_PULLDOWN_AON_LEN               (1U)
#define AON_DTEST_PULLDOWN_AON_MSK               (((1U << AON_DTEST_PULLDOWN_AON_LEN) - 1) << AON_DTEST_PULLDOWN_AON_POS)
#define AON_DTEST_PULLDOWN_AON_UMSK              (~(((1U << AON_DTEST_PULLDOWN_AON_LEN) - 1) << AON_DTEST_PULLDOWN_AON_POS))

/* 0x808 : Digital Control */
#define AON_MBG_SFREG_OFFSET                     (0x808)
#define AON_PU_MBG_AON                           AON_PU_MBG_AON
#define AON_PU_MBG_AON_POS                       (0U)
#define AON_PU_MBG_AON_LEN                       (1U)
#define AON_PU_MBG_AON_MSK                       (((1U << AON_PU_MBG_AON_LEN) - 1) << AON_PU_MBG_AON_POS)
#define AON_PU_MBG_AON_UMSK                      (~(((1U << AON_PU_MBG_AON_LEN) - 1) << AON_PU_MBG_AON_POS))
#define AON_PU_SFREG_AON                         AON_PU_SFREG_AON
#define AON_PU_SFREG_AON_POS                     (2U)
#define AON_PU_SFREG_AON_LEN                     (1U)
#define AON_PU_SFREG_AON_MSK                     (((1U << AON_PU_SFREG_AON_LEN) - 1) << AON_PU_SFREG_AON_POS)
#define AON_PU_SFREG_AON_UMSK                    (~(((1U << AON_PU_SFREG_AON_LEN) - 1) << AON_PU_SFREG_AON_POS))
#define AON_TEN_MBG_AON                          AON_TEN_MBG_AON
#define AON_TEN_MBG_AON_POS                      (6U)
#define AON_TEN_MBG_AON_LEN                      (1U)
#define AON_TEN_MBG_AON_MSK                      (((1U << AON_TEN_MBG_AON_LEN) - 1) << AON_TEN_MBG_AON_POS)
#define AON_TEN_MBG_AON_UMSK                     (~(((1U << AON_TEN_MBG_AON_LEN) - 1) << AON_TEN_MBG_AON_POS))
#define AON_TEN_CIP_AON                          AON_TEN_CIP_AON
#define AON_TEN_CIP_AON_POS                      (7U)
#define AON_TEN_CIP_AON_LEN                      (1U)
#define AON_TEN_CIP_AON_MSK                      (((1U << AON_TEN_CIP_AON_LEN) - 1) << AON_TEN_CIP_AON_POS)
#define AON_TEN_CIP_AON_UMSK                     (~(((1U << AON_TEN_CIP_AON_LEN) - 1) << AON_TEN_CIP_AON_POS))
#define AON_CIP_RESV_AON                         AON_CIP_RESV_AON
#define AON_CIP_RESV_AON_POS                     (8U)
#define AON_CIP_RESV_AON_LEN                     (8U)
#define AON_CIP_RESV_AON_MSK                     (((1U << AON_CIP_RESV_AON_LEN) - 1) << AON_CIP_RESV_AON_POS)
#define AON_CIP_RESV_AON_UMSK                    (~(((1U << AON_CIP_RESV_AON_LEN) - 1) << AON_CIP_RESV_AON_POS))
#define AON_MBG_PREREG_VOUT_SEL_AON              AON_MBG_PREREG_VOUT_SEL_AON
#define AON_MBG_PREREG_VOUT_SEL_AON_POS          (16U)
#define AON_MBG_PREREG_VOUT_SEL_AON_LEN          (2U)
#define AON_MBG_PREREG_VOUT_SEL_AON_MSK          (((1U << AON_MBG_PREREG_VOUT_SEL_AON_LEN) - 1) << AON_MBG_PREREG_VOUT_SEL_AON_POS)
#define AON_MBG_PREREG_VOUT_SEL_AON_UMSK         (~(((1U << AON_MBG_PREREG_VOUT_SEL_AON_LEN) - 1) << AON_MBG_PREREG_VOUT_SEL_AON_POS))
#define AON_MBG_PREREG_BYPASS_AON                AON_MBG_PREREG_BYPASS_AON
#define AON_MBG_PREREG_BYPASS_AON_POS            (18U)
#define AON_MBG_PREREG_BYPASS_AON_LEN            (1U)
#define AON_MBG_PREREG_BYPASS_AON_MSK            (((1U << AON_MBG_PREREG_BYPASS_AON_LEN) - 1) << AON_MBG_PREREG_BYPASS_AON_POS)
#define AON_MBG_PREREG_BYPASS_AON_UMSK           (~(((1U << AON_MBG_PREREG_BYPASS_AON_LEN) - 1) << AON_MBG_PREREG_BYPASS_AON_POS))
#define AON_MBG_PTAT_OUT_EN_AON                  AON_MBG_PTAT_OUT_EN_AON
#define AON_MBG_PTAT_OUT_EN_AON_POS              (19U)
#define AON_MBG_PTAT_OUT_EN_AON_LEN              (1U)
#define AON_MBG_PTAT_OUT_EN_AON_MSK              (((1U << AON_MBG_PTAT_OUT_EN_AON_LEN) - 1) << AON_MBG_PTAT_OUT_EN_AON_POS)
#define AON_MBG_PTAT_OUT_EN_AON_UMSK             (~(((1U << AON_MBG_PTAT_OUT_EN_AON_LEN) - 1) << AON_MBG_PTAT_OUT_EN_AON_POS))
#define AON_SFREG_VG10_SEL_AON                   AON_SFREG_VG10_SEL_AON
#define AON_SFREG_VG10_SEL_AON_POS               (20U)
#define AON_SFREG_VG10_SEL_AON_LEN               (3U)
#define AON_SFREG_VG10_SEL_AON_MSK               (((1U << AON_SFREG_VG10_SEL_AON_LEN) - 1) << AON_SFREG_VG10_SEL_AON_POS)
#define AON_SFREG_VG10_SEL_AON_UMSK              (~(((1U << AON_SFREG_VG10_SEL_AON_LEN) - 1) << AON_SFREG_VG10_SEL_AON_POS))
#define AON_SFREG_VG09_SEL_AON                   AON_SFREG_VG09_SEL_AON
#define AON_SFREG_VG09_SEL_AON_POS               (24U)
#define AON_SFREG_VG09_SEL_AON_LEN               (3U)
#define AON_SFREG_VG09_SEL_AON_MSK               (((1U << AON_SFREG_VG09_SEL_AON_LEN) - 1) << AON_SFREG_VG09_SEL_AON_POS)
#define AON_SFREG_VG09_SEL_AON_UMSK              (~(((1U << AON_SFREG_VG09_SEL_AON_LEN) - 1) << AON_SFREG_VG09_SEL_AON_POS))

/* 0x80C : aon_0 */
#define AON_0_OFFSET                             (0x80C)
#define AON_PU_BOD_AON                           AON_PU_BOD_AON
#define AON_PU_BOD_AON_POS                       (0U)
#define AON_PU_BOD_AON_LEN                       (1U)
#define AON_PU_BOD_AON_MSK                       (((1U << AON_PU_BOD_AON_LEN) - 1) << AON_PU_BOD_AON_POS)
#define AON_PU_BOD_AON_UMSK                      (~(((1U << AON_PU_BOD_AON_LEN) - 1) << AON_PU_BOD_AON_POS))
#define AON_PU_RC32K_AON                         AON_PU_RC32K_AON
#define AON_PU_RC32K_AON_POS                     (1U)
#define AON_PU_RC32K_AON_LEN                     (1U)
#define AON_PU_RC32K_AON_MSK                     (((1U << AON_PU_RC32K_AON_LEN) - 1) << AON_PU_RC32K_AON_POS)
#define AON_PU_RC32K_AON_UMSK                    (~(((1U << AON_PU_RC32K_AON_LEN) - 1) << AON_PU_RC32K_AON_POS))
#define AON_PU_VCC18D_AON                        AON_PU_VCC18D_AON
#define AON_PU_VCC18D_AON_POS                    (2U)
#define AON_PU_VCC18D_AON_LEN                    (1U)
#define AON_PU_VCC18D_AON_MSK                    (((1U << AON_PU_VCC18D_AON_LEN) - 1) << AON_PU_VCC18D_AON_POS)
#define AON_PU_VCC18D_AON_UMSK                   (~(((1U << AON_PU_VCC18D_AON_LEN) - 1) << AON_PU_VCC18D_AON_POS))
#define AON_PU_XTAL32K_AON                       AON_PU_XTAL32K_AON
#define AON_PU_XTAL32K_AON_POS                   (3U)
#define AON_PU_XTAL32K_AON_LEN                   (1U)
#define AON_PU_XTAL32K_AON_MSK                   (((1U << AON_PU_XTAL32K_AON_LEN) - 1) << AON_PU_XTAL32K_AON_POS)
#define AON_PU_XTAL32K_AON_UMSK                  (~(((1U << AON_PU_XTAL32K_AON_LEN) - 1) << AON_PU_XTAL32K_AON_POS))
#define AON_PU_XTAL32K_BUF_AON                   AON_PU_XTAL32K_BUF_AON
#define AON_PU_XTAL32K_BUF_AON_POS               (4U)
#define AON_PU_XTAL32K_BUF_AON_LEN               (1U)
#define AON_PU_XTAL32K_BUF_AON_MSK               (((1U << AON_PU_XTAL32K_BUF_AON_LEN) - 1) << AON_PU_XTAL32K_BUF_AON_POS)
#define AON_PU_XTAL32K_BUF_AON_UMSK              (~(((1U << AON_PU_XTAL32K_BUF_AON_LEN) - 1) << AON_PU_XTAL32K_BUF_AON_POS))
#define AON_LDO18AON_EXT18_EN_AON                AON_LDO18AON_EXT18_EN_AON
#define AON_LDO18AON_EXT18_EN_AON_POS            (5U)
#define AON_LDO18AON_EXT18_EN_AON_LEN            (1U)
#define AON_LDO18AON_EXT18_EN_AON_MSK            (((1U << AON_LDO18AON_EXT18_EN_AON_LEN) - 1) << AON_LDO18AON_EXT18_EN_AON_POS)
#define AON_LDO18AON_EXT18_EN_AON_UMSK           (~(((1U << AON_LDO18AON_EXT18_EN_AON_LEN) - 1) << AON_LDO18AON_EXT18_EN_AON_POS))
#define AON_LDO18AON_SLOW_PLD_AON                AON_LDO18AON_SLOW_PLD_AON
#define AON_LDO18AON_SLOW_PLD_AON_POS            (7U)
#define AON_LDO18AON_SLOW_PLD_AON_LEN            (1U)
#define AON_LDO18AON_SLOW_PLD_AON_MSK            (((1U << AON_LDO18AON_SLOW_PLD_AON_LEN) - 1) << AON_LDO18AON_SLOW_PLD_AON_POS)
#define AON_LDO18AON_SLOW_PLD_AON_UMSK           (~(((1U << AON_LDO18AON_SLOW_PLD_AON_LEN) - 1) << AON_LDO18AON_SLOW_PLD_AON_POS))
#define AON_LDO18AON_CC_AON                      AON_LDO18AON_CC_AON
#define AON_LDO18AON_CC_AON_POS                  (8U)
#define AON_LDO18AON_CC_AON_LEN                  (3U)
#define AON_LDO18AON_CC_AON_MSK                  (((1U << AON_LDO18AON_CC_AON_LEN) - 1) << AON_LDO18AON_CC_AON_POS)
#define AON_LDO18AON_CC_AON_UMSK                 (~(((1U << AON_LDO18AON_CC_AON_LEN) - 1) << AON_LDO18AON_CC_AON_POS))
#define AON_LDO18AON_RZ_AON                      AON_LDO18AON_RZ_AON
#define AON_LDO18AON_RZ_AON_POS                  (12U)
#define AON_LDO18AON_RZ_AON_LEN                  (2U)
#define AON_LDO18AON_RZ_AON_MSK                  (((1U << AON_LDO18AON_RZ_AON_LEN) - 1) << AON_LDO18AON_RZ_AON_POS)
#define AON_LDO18AON_RZ_AON_UMSK                 (~(((1U << AON_LDO18AON_RZ_AON_LEN) - 1) << AON_LDO18AON_RZ_AON_POS))
#define AON_LDO18AON_VOUT_SEL_AON                AON_LDO18AON_VOUT_SEL_AON
#define AON_LDO18AON_VOUT_SEL_AON_POS            (16U)
#define AON_LDO18AON_VOUT_SEL_AON_LEN            (3U)
#define AON_LDO18AON_VOUT_SEL_AON_MSK            (((1U << AON_LDO18AON_VOUT_SEL_AON_LEN) - 1) << AON_LDO18AON_VOUT_SEL_AON_POS)
#define AON_LDO18AON_VOUT_SEL_AON_UMSK           (~(((1U << AON_LDO18AON_VOUT_SEL_AON_LEN) - 1) << AON_LDO18AON_VOUT_SEL_AON_POS))
#define AON_LDO18AON_VOUT_TRIM_AON               AON_LDO18AON_VOUT_TRIM_AON
#define AON_LDO18AON_VOUT_TRIM_AON_POS           (20U)
#define AON_LDO18AON_VOUT_TRIM_AON_LEN           (4U)
#define AON_LDO18AON_VOUT_TRIM_AON_MSK           (((1U << AON_LDO18AON_VOUT_TRIM_AON_LEN) - 1) << AON_LDO18AON_VOUT_TRIM_AON_POS)
#define AON_LDO18AON_VOUT_TRIM_AON_UMSK          (~(((1U << AON_LDO18AON_VOUT_TRIM_AON_LEN) - 1) << AON_LDO18AON_VOUT_TRIM_AON_POS))
#define AON_LDO08AON_VOUT_SEL_AON                AON_LDO08AON_VOUT_SEL_AON
#define AON_LDO08AON_VOUT_SEL_AON_POS            (24U)
#define AON_LDO08AON_VOUT_SEL_AON_LEN            (4U)
#define AON_LDO08AON_VOUT_SEL_AON_MSK            (((1U << AON_LDO08AON_VOUT_SEL_AON_LEN) - 1) << AON_LDO08AON_VOUT_SEL_AON_POS)
#define AON_LDO08AON_VOUT_SEL_AON_UMSK           (~(((1U << AON_LDO08AON_VOUT_SEL_AON_LEN) - 1) << AON_LDO08AON_VOUT_SEL_AON_POS))
#define AON_LDO08AON_VOUT_TRIM_AON               AON_LDO08AON_VOUT_TRIM_AON
#define AON_LDO08AON_VOUT_TRIM_AON_POS           (28U)
#define AON_LDO08AON_VOUT_TRIM_AON_LEN           (4U)
#define AON_LDO08AON_VOUT_TRIM_AON_MSK           (((1U << AON_LDO08AON_VOUT_TRIM_AON_LEN) - 1) << AON_LDO08AON_VOUT_TRIM_AON_POS)
#define AON_LDO08AON_VOUT_TRIM_AON_UMSK          (~(((1U << AON_LDO08AON_VOUT_TRIM_AON_LEN) - 1) << AON_LDO08AON_VOUT_TRIM_AON_POS))

/* 0x810 : Digital Control */
#define AON_1_OFFSET                             (0x810)
#define AON_BOD_TH_AON                           AON_BOD_TH_AON
#define AON_BOD_TH_AON_POS                       (0U)
#define AON_BOD_TH_AON_LEN                       (3U)
#define AON_BOD_TH_AON_MSK                       (((1U << AON_BOD_TH_AON_LEN) - 1) << AON_BOD_TH_AON_POS)
#define AON_BOD_TH_AON_UMSK                      (~(((1U << AON_BOD_TH_AON_LEN) - 1) << AON_BOD_TH_AON_POS))
#define AON_R_BOD_OUT                            AON_R_BOD_OUT
#define AON_R_BOD_OUT_POS                        (3U)
#define AON_R_BOD_OUT_LEN                        (1U)
#define AON_R_BOD_OUT_MSK                        (((1U << AON_R_BOD_OUT_LEN) - 1) << AON_R_BOD_OUT_POS)
#define AON_R_BOD_OUT_UMSK                       (~(((1U << AON_R_BOD_OUT_LEN) - 1) << AON_R_BOD_OUT_POS))
#define AON_POR_OUT                              AON_POR_OUT
#define AON_POR_OUT_POS                          (4U)
#define AON_POR_OUT_LEN                          (1U)
#define AON_POR_OUT_MSK                          (((1U << AON_POR_OUT_LEN) - 1) << AON_POR_OUT_POS)
#define AON_POR_OUT_UMSK                         (~(((1U << AON_POR_OUT_LEN) - 1) << AON_POR_OUT_POS))
#define AON_HW_PU_BG_SYS_AON                     AON_HW_PU_BG_SYS_AON
#define AON_HW_PU_BG_SYS_AON_POS                 (5U)
#define AON_HW_PU_BG_SYS_AON_LEN                 (1U)
#define AON_HW_PU_BG_SYS_AON_MSK                 (((1U << AON_HW_PU_BG_SYS_AON_LEN) - 1) << AON_HW_PU_BG_SYS_AON_POS)
#define AON_HW_PU_BG_SYS_AON_UMSK                (~(((1U << AON_HW_PU_BG_SYS_AON_LEN) - 1) << AON_HW_PU_BG_SYS_AON_POS))
#define AON_HW_PU_DCDC12_AON                     AON_HW_PU_DCDC12_AON
#define AON_HW_PU_DCDC12_AON_POS                 (6U)
#define AON_HW_PU_DCDC12_AON_LEN                 (1U)
#define AON_HW_PU_DCDC12_AON_MSK                 (((1U << AON_HW_PU_DCDC12_AON_LEN) - 1) << AON_HW_PU_DCDC12_AON_POS)
#define AON_HW_PU_DCDC12_AON_UMSK                (~(((1U << AON_HW_PU_DCDC12_AON_LEN) - 1) << AON_HW_PU_DCDC12_AON_POS))
#define AON_HW_PU_LDO09_AON                      AON_HW_PU_LDO09_AON
#define AON_HW_PU_LDO09_AON_POS                  (7U)
#define AON_HW_PU_LDO09_AON_LEN                  (1U)
#define AON_HW_PU_LDO09_AON_MSK                  (((1U << AON_HW_PU_LDO09_AON_LEN) - 1) << AON_HW_PU_LDO09_AON_POS)
#define AON_HW_PU_LDO09_AON_UMSK                 (~(((1U << AON_HW_PU_LDO09_AON_LEN) - 1) << AON_HW_PU_LDO09_AON_POS))
#define AON_RC32M_PU_PDS                         AON_RC32M_PU_PDS
#define AON_RC32M_PU_PDS_POS                     (8U)
#define AON_RC32M_PU_PDS_LEN                     (1U)
#define AON_RC32M_PU_PDS_MSK                     (((1U << AON_RC32M_PU_PDS_LEN) - 1) << AON_RC32M_PU_PDS_POS)
#define AON_RC32M_PU_PDS_UMSK                    (~(((1U << AON_RC32M_PU_PDS_LEN) - 1) << AON_RC32M_PU_PDS_POS))
#define AON_RESV_AON                             AON_RESV_AON
#define AON_RESV_AON_POS                         (12U)
#define AON_RESV_AON_LEN                         (8U)
#define AON_RESV_AON_MSK                         (((1U << AON_RESV_AON_LEN) - 1) << AON_RESV_AON_POS)
#define AON_RESV_AON_UMSK                        (~(((1U << AON_RESV_AON_LEN) - 1) << AON_RESV_AON_POS))
#define AON_SW_SOC_EN_AON                        AON_SW_SOC_EN_AON
#define AON_SW_SOC_EN_AON_POS                    (20U)
#define AON_SW_SOC_EN_AON_LEN                    (1U)
#define AON_SW_SOC_EN_AON_MSK                    (((1U << AON_SW_SOC_EN_AON_LEN) - 1) << AON_SW_SOC_EN_AON_POS)
#define AON_SW_SOC_EN_AON_UMSK                   (~(((1U << AON_SW_SOC_EN_AON_LEN) - 1) << AON_SW_SOC_EN_AON_POS))
#define AON_SW_WB_EN_AON                         AON_SW_WB_EN_AON
#define AON_SW_WB_EN_AON_POS                     (21U)
#define AON_SW_WB_EN_AON_LEN                     (1U)
#define AON_SW_WB_EN_AON_MSK                     (((1U << AON_SW_WB_EN_AON_LEN) - 1) << AON_SW_WB_EN_AON_POS)
#define AON_SW_WB_EN_AON_UMSK                    (~(((1U << AON_SW_WB_EN_AON_LEN) - 1) << AON_SW_WB_EN_AON_POS))
#define AON_SW_BZ_EN_AON                         AON_SW_BZ_EN_AON
#define AON_SW_BZ_EN_AON_POS                     (22U)
#define AON_SW_BZ_EN_AON_LEN                     (1U)
#define AON_SW_BZ_EN_AON_MSK                     (((1U << AON_SW_BZ_EN_AON_LEN) - 1) << AON_SW_BZ_EN_AON_POS)
#define AON_SW_BZ_EN_AON_UMSK                    (~(((1U << AON_SW_BZ_EN_AON_LEN) - 1) << AON_SW_BZ_EN_AON_POS))

/* 0x814 : Digital Control */
#define AON_RC32K_0_OFFSET                       (0x814)
#define AON_RC32K_CAP_SEL_AON                    AON_RC32K_CAP_SEL_AON
#define AON_RC32K_CAP_SEL_AON_POS                (0U)
#define AON_RC32K_CAP_SEL_AON_LEN                (4U)
#define AON_RC32K_CAP_SEL_AON_MSK                (((1U << AON_RC32K_CAP_SEL_AON_LEN) - 1) << AON_RC32K_CAP_SEL_AON_POS)
#define AON_RC32K_CAP_SEL_AON_UMSK               (~(((1U << AON_RC32K_CAP_SEL_AON_LEN) - 1) << AON_RC32K_CAP_SEL_AON_POS))
#define AON_RC32K_PMIP_CLK_SEL_AON               AON_RC32K_PMIP_CLK_SEL_AON
#define AON_RC32K_PMIP_CLK_SEL_AON_POS           (15U)
#define AON_RC32K_PMIP_CLK_SEL_AON_LEN           (1U)
#define AON_RC32K_PMIP_CLK_SEL_AON_MSK           (((1U << AON_RC32K_PMIP_CLK_SEL_AON_LEN) - 1) << AON_RC32K_PMIP_CLK_SEL_AON_POS)
#define AON_RC32K_PMIP_CLK_SEL_AON_UMSK          (~(((1U << AON_RC32K_PMIP_CLK_SEL_AON_LEN) - 1) << AON_RC32K_PMIP_CLK_SEL_AON_POS))
#define AON_RC32K_TCOMP_CODE_AON                 AON_RC32K_TCOMP_CODE_AON
#define AON_RC32K_TCOMP_CODE_AON_POS             (16U)
#define AON_RC32K_TCOMP_CODE_AON_LEN             (10U)
#define AON_RC32K_TCOMP_CODE_AON_MSK             (((1U << AON_RC32K_TCOMP_CODE_AON_LEN) - 1) << AON_RC32K_TCOMP_CODE_AON_POS)
#define AON_RC32K_TCOMP_CODE_AON_UMSK            (~(((1U << AON_RC32K_TCOMP_CODE_AON_LEN) - 1) << AON_RC32K_TCOMP_CODE_AON_POS))
#define AON_RC32K_TCOMP_EN_AON                   AON_RC32K_TCOMP_EN_AON
#define AON_RC32K_TCOMP_EN_AON_POS               (26U)
#define AON_RC32K_TCOMP_EN_AON_LEN               (1U)
#define AON_RC32K_TCOMP_EN_AON_MSK               (((1U << AON_RC32K_TCOMP_EN_AON_LEN) - 1) << AON_RC32K_TCOMP_EN_AON_POS)
#define AON_RC32K_TCOMP_EN_AON_UMSK              (~(((1U << AON_RC32K_TCOMP_EN_AON_LEN) - 1) << AON_RC32K_TCOMP_EN_AON_POS))
#define AON_RC32K_VREF_DLY_AON                   AON_RC32K_VREF_DLY_AON
#define AON_RC32K_VREF_DLY_AON_POS               (28U)
#define AON_RC32K_VREF_DLY_AON_LEN               (2U)
#define AON_RC32K_VREF_DLY_AON_MSK               (((1U << AON_RC32K_VREF_DLY_AON_LEN) - 1) << AON_RC32K_VREF_DLY_AON_POS)
#define AON_RC32K_VREF_DLY_AON_UMSK              (~(((1U << AON_RC32K_VREF_DLY_AON_LEN) - 1) << AON_RC32K_VREF_DLY_AON_POS))

/* 0x818 : Digital Control */
#define AON_RC32K_1_OFFSET                       (0x818)
#define AON_RC32K_RDY                            AON_RC32K_RDY
#define AON_RC32K_RDY_POS                        (0U)
#define AON_RC32K_RDY_LEN                        (1U)
#define AON_RC32K_RDY_MSK                        (((1U << AON_RC32K_RDY_LEN) - 1) << AON_RC32K_RDY_POS)
#define AON_RC32K_RDY_UMSK                       (~(((1U << AON_RC32K_RDY_LEN) - 1) << AON_RC32K_RDY_POS))
#define AON_RC32K_CODE_VALID                     AON_RC32K_CODE_VALID
#define AON_RC32K_CODE_VALID_POS                 (1U)
#define AON_RC32K_CODE_VALID_LEN                 (1U)
#define AON_RC32K_CODE_VALID_MSK                 (((1U << AON_RC32K_CODE_VALID_LEN) - 1) << AON_RC32K_CODE_VALID_POS)
#define AON_RC32K_CODE_VALID_UMSK                (~(((1U << AON_RC32K_CODE_VALID_LEN) - 1) << AON_RC32K_CODE_VALID_POS))
#define AON_RC32K_IPTAT_BM_AON                   AON_RC32K_IPTAT_BM_AON
#define AON_RC32K_IPTAT_BM_AON_POS               (4U)
#define AON_RC32K_IPTAT_BM_AON_LEN               (3U)
#define AON_RC32K_IPTAT_BM_AON_MSK               (((1U << AON_RC32K_IPTAT_BM_AON_LEN) - 1) << AON_RC32K_IPTAT_BM_AON_POS)
#define AON_RC32K_IPTAT_BM_AON_UMSK              (~(((1U << AON_RC32K_IPTAT_BM_AON_LEN) - 1) << AON_RC32K_IPTAT_BM_AON_POS))
#define AON_PU_RC32K_IPTAT_GEN_AON               AON_PU_RC32K_IPTAT_GEN_AON
#define AON_PU_RC32K_IPTAT_GEN_AON_POS           (7U)
#define AON_PU_RC32K_IPTAT_GEN_AON_LEN           (1U)
#define AON_PU_RC32K_IPTAT_GEN_AON_MSK           (((1U << AON_PU_RC32K_IPTAT_GEN_AON_LEN) - 1) << AON_PU_RC32K_IPTAT_GEN_AON_POS)
#define AON_PU_RC32K_IPTAT_GEN_AON_UMSK          (~(((1U << AON_PU_RC32K_IPTAT_GEN_AON_LEN) - 1) << AON_PU_RC32K_IPTAT_GEN_AON_POS))

/* 0x81C : Digital Control */
#define AON_XTAL32K_OFFSET                       (0x81C)
#define AON_XTAL32K_AC_CAP_SHORT_AON             AON_XTAL32K_AC_CAP_SHORT_AON
#define AON_XTAL32K_AC_CAP_SHORT_AON_POS         (0U)
#define AON_XTAL32K_AC_CAP_SHORT_AON_LEN         (1U)
#define AON_XTAL32K_AC_CAP_SHORT_AON_MSK         (((1U << AON_XTAL32K_AC_CAP_SHORT_AON_LEN) - 1) << AON_XTAL32K_AC_CAP_SHORT_AON_POS)
#define AON_XTAL32K_AC_CAP_SHORT_AON_UMSK        (~(((1U << AON_XTAL32K_AC_CAP_SHORT_AON_LEN) - 1) << AON_XTAL32K_AC_CAP_SHORT_AON_POS))
#define AON_XTAL32K_FAST_STARTUP_AON             AON_XTAL32K_FAST_STARTUP_AON
#define AON_XTAL32K_FAST_STARTUP_AON_POS         (1U)
#define AON_XTAL32K_FAST_STARTUP_AON_LEN         (1U)
#define AON_XTAL32K_FAST_STARTUP_AON_MSK         (((1U << AON_XTAL32K_FAST_STARTUP_AON_LEN) - 1) << AON_XTAL32K_FAST_STARTUP_AON_POS)
#define AON_XTAL32K_FAST_STARTUP_AON_UMSK        (~(((1U << AON_XTAL32K_FAST_STARTUP_AON_LEN) - 1) << AON_XTAL32K_FAST_STARTUP_AON_POS))
#define AON_XTAL32K_AMP_CTRL_AON                 AON_XTAL32K_AMP_CTRL_AON
#define AON_XTAL32K_AMP_CTRL_AON_POS             (2U)
#define AON_XTAL32K_AMP_CTRL_AON_LEN             (2U)
#define AON_XTAL32K_AMP_CTRL_AON_MSK             (((1U << AON_XTAL32K_AMP_CTRL_AON_LEN) - 1) << AON_XTAL32K_AMP_CTRL_AON_POS)
#define AON_XTAL32K_AMP_CTRL_AON_UMSK            (~(((1U << AON_XTAL32K_AMP_CTRL_AON_LEN) - 1) << AON_XTAL32K_AMP_CTRL_AON_POS))
#define AON_XTAL32K_EXT_SEL_AON                  AON_XTAL32K_EXT_SEL_AON
#define AON_XTAL32K_EXT_SEL_AON_POS              (10U)
#define AON_XTAL32K_EXT_SEL_AON_LEN              (1U)
#define AON_XTAL32K_EXT_SEL_AON_MSK              (((1U << AON_XTAL32K_EXT_SEL_AON_LEN) - 1) << AON_XTAL32K_EXT_SEL_AON_POS)
#define AON_XTAL32K_EXT_SEL_AON_UMSK             (~(((1U << AON_XTAL32K_EXT_SEL_AON_LEN) - 1) << AON_XTAL32K_EXT_SEL_AON_POS))
#define AON_XTAL32K_HIZ_EN_AON                   AON_XTAL32K_HIZ_EN_AON
#define AON_XTAL32K_HIZ_EN_AON_POS               (11U)
#define AON_XTAL32K_HIZ_EN_AON_LEN               (1U)
#define AON_XTAL32K_HIZ_EN_AON_MSK               (((1U << AON_XTAL32K_HIZ_EN_AON_LEN) - 1) << AON_XTAL32K_HIZ_EN_AON_POS)
#define AON_XTAL32K_HIZ_EN_AON_UMSK              (~(((1U << AON_XTAL32K_HIZ_EN_AON_LEN) - 1) << AON_XTAL32K_HIZ_EN_AON_POS))
#define AON_XTAL32K_INV_STRE_AON                 AON_XTAL32K_INV_STRE_AON
#define AON_XTAL32K_INV_STRE_AON_POS             (12U)
#define AON_XTAL32K_INV_STRE_AON_LEN             (2U)
#define AON_XTAL32K_INV_STRE_AON_MSK             (((1U << AON_XTAL32K_INV_STRE_AON_LEN) - 1) << AON_XTAL32K_INV_STRE_AON_POS)
#define AON_XTAL32K_INV_STRE_AON_UMSK            (~(((1U << AON_XTAL32K_INV_STRE_AON_LEN) - 1) << AON_XTAL32K_INV_STRE_AON_POS))
#define AON_XTAL32K_OTF_SHORT_AON                AON_XTAL32K_OTF_SHORT_AON
#define AON_XTAL32K_OTF_SHORT_AON_POS            (14U)
#define AON_XTAL32K_OTF_SHORT_AON_LEN            (1U)
#define AON_XTAL32K_OTF_SHORT_AON_MSK            (((1U << AON_XTAL32K_OTF_SHORT_AON_LEN) - 1) << AON_XTAL32K_OTF_SHORT_AON_POS)
#define AON_XTAL32K_OTF_SHORT_AON_UMSK           (~(((1U << AON_XTAL32K_OTF_SHORT_AON_LEN) - 1) << AON_XTAL32K_OTF_SHORT_AON_POS))
#define AON_XTAL32K_OUTBUF_STRE_AON              AON_XTAL32K_OUTBUF_STRE_AON
#define AON_XTAL32K_OUTBUF_STRE_AON_POS          (15U)
#define AON_XTAL32K_OUTBUF_STRE_AON_LEN          (1U)
#define AON_XTAL32K_OUTBUF_STRE_AON_MSK          (((1U << AON_XTAL32K_OUTBUF_STRE_AON_LEN) - 1) << AON_XTAL32K_OUTBUF_STRE_AON_POS)
#define AON_XTAL32K_OUTBUF_STRE_AON_UMSK         (~(((1U << AON_XTAL32K_OUTBUF_STRE_AON_LEN) - 1) << AON_XTAL32K_OUTBUF_STRE_AON_POS))
#define AON_XTAL32K_RDY_RSTB_AON                 AON_XTAL32K_RDY_RSTB_AON
#define AON_XTAL32K_RDY_RSTB_AON_POS             (16U)
#define AON_XTAL32K_RDY_RSTB_AON_LEN             (1U)
#define AON_XTAL32K_RDY_RSTB_AON_MSK             (((1U << AON_XTAL32K_RDY_RSTB_AON_LEN) - 1) << AON_XTAL32K_RDY_RSTB_AON_POS)
#define AON_XTAL32K_RDY_RSTB_AON_UMSK            (~(((1U << AON_XTAL32K_RDY_RSTB_AON_LEN) - 1) << AON_XTAL32K_RDY_RSTB_AON_POS))
#define AON_XTAL32K_GM_PD_CTRL_AON               AON_XTAL32K_GM_PD_CTRL_AON
#define AON_XTAL32K_GM_PD_CTRL_AON_POS           (17U)
#define AON_XTAL32K_GM_PD_CTRL_AON_LEN           (1U)
#define AON_XTAL32K_GM_PD_CTRL_AON_MSK           (((1U << AON_XTAL32K_GM_PD_CTRL_AON_LEN) - 1) << AON_XTAL32K_GM_PD_CTRL_AON_POS)
#define AON_XTAL32K_GM_PD_CTRL_AON_UMSK          (~(((1U << AON_XTAL32K_GM_PD_CTRL_AON_LEN) - 1) << AON_XTAL32K_GM_PD_CTRL_AON_POS))
#define AON_XTAL32K_RDY_SEL_AON                  AON_XTAL32K_RDY_SEL_AON
#define AON_XTAL32K_RDY_SEL_AON_POS              (20U)
#define AON_XTAL32K_RDY_SEL_AON_LEN              (2U)
#define AON_XTAL32K_RDY_SEL_AON_MSK              (((1U << AON_XTAL32K_RDY_SEL_AON_LEN) - 1) << AON_XTAL32K_RDY_SEL_AON_POS)
#define AON_XTAL32K_RDY_SEL_AON_UMSK             (~(((1U << AON_XTAL32K_RDY_SEL_AON_LEN) - 1) << AON_XTAL32K_RDY_SEL_AON_POS))
#define AON_XTAL32K_REG_AON                      AON_XTAL32K_REG_AON
#define AON_XTAL32K_REG_AON_POS                  (24U)
#define AON_XTAL32K_REG_AON_LEN                  (2U)
#define AON_XTAL32K_REG_AON_MSK                  (((1U << AON_XTAL32K_REG_AON_LEN) - 1) << AON_XTAL32K_REG_AON_POS)
#define AON_XTAL32K_REG_AON_UMSK                 (~(((1U << AON_XTAL32K_REG_AON_LEN) - 1) << AON_XTAL32K_REG_AON_POS))
#define AON_XTAL32K_CLK_RDY                      AON_XTAL32K_CLK_RDY
#define AON_XTAL32K_CLK_RDY_POS                  (26U)
#define AON_XTAL32K_CLK_RDY_LEN                  (1U)
#define AON_XTAL32K_CLK_RDY_MSK                  (((1U << AON_XTAL32K_CLK_RDY_LEN) - 1) << AON_XTAL32K_CLK_RDY_POS)
#define AON_XTAL32K_CLK_RDY_UMSK                 (~(((1U << AON_XTAL32K_CLK_RDY_LEN) - 1) << AON_XTAL32K_CLK_RDY_POS))
#define AON_XTAL32K_INV_EXTRA_STRE_AON           AON_XTAL32K_INV_EXTRA_STRE_AON
#define AON_XTAL32K_INV_EXTRA_STRE_AON_POS       (27U)
#define AON_XTAL32K_INV_EXTRA_STRE_AON_LEN       (1U)
#define AON_XTAL32K_INV_EXTRA_STRE_AON_MSK       (((1U << AON_XTAL32K_INV_EXTRA_STRE_AON_LEN) - 1) << AON_XTAL32K_INV_EXTRA_STRE_AON_POS)
#define AON_XTAL32K_INV_EXTRA_STRE_AON_UMSK      (~(((1U << AON_XTAL32K_INV_EXTRA_STRE_AON_LEN) - 1) << AON_XTAL32K_INV_EXTRA_STRE_AON_POS))

/* 0x820 : bg_sys_top */
#define AON_BG_SYS_TOP_OFFSET                    (0x820)
#define AON_PU_BG_SYS_AON                        AON_PU_BG_SYS_AON
#define AON_PU_BG_SYS_AON_POS                    (0U)
#define AON_PU_BG_SYS_AON_LEN                    (1U)
#define AON_PU_BG_SYS_AON_MSK                    (((1U << AON_PU_BG_SYS_AON_LEN) - 1) << AON_PU_BG_SYS_AON_POS)
#define AON_PU_BG_SYS_AON_UMSK                   (~(((1U << AON_PU_BG_SYS_AON_LEN) - 1) << AON_PU_BG_SYS_AON_POS))

/* 0x824 : ldo11soc_and_dctest */
#define AON_LDO11SOC_AND_DCTEST_OFFSET           (0x824)
#define AON_LDO11SOC_RDY_AON                     AON_LDO11SOC_RDY_AON
#define AON_LDO11SOC_RDY_AON_POS                 (28U)
#define AON_LDO11SOC_RDY_AON_LEN                 (1U)
#define AON_LDO11SOC_RDY_AON_MSK                 (((1U << AON_LDO11SOC_RDY_AON_LEN) - 1) << AON_LDO11SOC_RDY_AON_POS)
#define AON_LDO11SOC_RDY_AON_UMSK                (~(((1U << AON_LDO11SOC_RDY_AON_LEN) - 1) << AON_LDO11SOC_RDY_AON_POS))

/* 0x828 : psw_misc */
#define AON_PSW_MISC_OFFSET                      (0x828)
#define AON_USB20_RCAL_COARSE_AON                AON_USB20_RCAL_COARSE_AON
#define AON_USB20_RCAL_COARSE_AON_POS            (4U)
#define AON_USB20_RCAL_COARSE_AON_LEN            (2U)
#define AON_USB20_RCAL_COARSE_AON_MSK            (((1U << AON_USB20_RCAL_COARSE_AON_LEN) - 1) << AON_USB20_RCAL_COARSE_AON_POS)
#define AON_USB20_RCAL_COARSE_AON_UMSK           (~(((1U << AON_USB20_RCAL_COARSE_AON_LEN) - 1) << AON_USB20_RCAL_COARSE_AON_POS))
#define AON_USB20_RCAL_CODE_AON                  AON_USB20_RCAL_CODE_AON
#define AON_USB20_RCAL_CODE_AON_POS              (8U)
#define AON_USB20_RCAL_CODE_AON_LEN              (5U)
#define AON_USB20_RCAL_CODE_AON_MSK              (((1U << AON_USB20_RCAL_CODE_AON_LEN) - 1) << AON_USB20_RCAL_CODE_AON_POS)
#define AON_USB20_RCAL_CODE_AON_UMSK             (~(((1U << AON_USB20_RCAL_CODE_AON_LEN) - 1) << AON_USB20_RCAL_CODE_AON_POS))
#define AON_USB20_RREF_HIZ_AON                   AON_USB20_RREF_HIZ_AON
#define AON_USB20_RREF_HIZ_AON_POS               (16U)
#define AON_USB20_RREF_HIZ_AON_LEN               (1U)
#define AON_USB20_RREF_HIZ_AON_MSK               (((1U << AON_USB20_RREF_HIZ_AON_LEN) - 1) << AON_USB20_RREF_HIZ_AON_POS)
#define AON_USB20_RREF_HIZ_AON_UMSK              (~(((1U << AON_USB20_RREF_HIZ_AON_LEN) - 1) << AON_USB20_RREF_HIZ_AON_POS))
#define AON_USB20_RREF_CAL_IIN_EN_AON            AON_USB20_RREF_CAL_IIN_EN_AON
#define AON_USB20_RREF_CAL_IIN_EN_AON_POS        (17U)
#define AON_USB20_RREF_CAL_IIN_EN_AON_LEN        (1U)
#define AON_USB20_RREF_CAL_IIN_EN_AON_MSK        (((1U << AON_USB20_RREF_CAL_IIN_EN_AON_LEN) - 1) << AON_USB20_RREF_CAL_IIN_EN_AON_POS)
#define AON_USB20_RREF_CAL_IIN_EN_AON_UMSK       (~(((1U << AON_USB20_RREF_CAL_IIN_EN_AON_LEN) - 1) << AON_USB20_RREF_CAL_IIN_EN_AON_POS))
#define AON_USB20_RREF_CAL_VOUT_EN_AON           AON_USB20_RREF_CAL_VOUT_EN_AON
#define AON_USB20_RREF_CAL_VOUT_EN_AON_POS       (18U)
#define AON_USB20_RREF_CAL_VOUT_EN_AON_LEN       (1U)
#define AON_USB20_RREF_CAL_VOUT_EN_AON_MSK       (((1U << AON_USB20_RREF_CAL_VOUT_EN_AON_LEN) - 1) << AON_USB20_RREF_CAL_VOUT_EN_AON_POS)
#define AON_USB20_RREF_CAL_VOUT_EN_AON_UMSK      (~(((1U << AON_USB20_RREF_CAL_VOUT_EN_AON_LEN) - 1) << AON_USB20_RREF_CAL_VOUT_EN_AON_POS))
#define AON_USB20_RREF_EXT_EN_AON                AON_USB20_RREF_EXT_EN_AON
#define AON_USB20_RREF_EXT_EN_AON_POS            (19U)
#define AON_USB20_RREF_EXT_EN_AON_LEN            (1U)
#define AON_USB20_RREF_EXT_EN_AON_MSK            (((1U << AON_USB20_RREF_EXT_EN_AON_LEN) - 1) << AON_USB20_RREF_EXT_EN_AON_POS)
#define AON_USB20_RREF_EXT_EN_AON_UMSK           (~(((1U << AON_USB20_RREF_EXT_EN_AON_LEN) - 1) << AON_USB20_RREF_EXT_EN_AON_POS))

/* 0x82C : Digital Control */
#define AON_LDO09SOC_OFFSET                      (0x82C)
#define AON_LDO09SOC_EXT_AON                     AON_LDO09SOC_EXT_AON
#define AON_LDO09SOC_EXT_AON_POS                 (0U)
#define AON_LDO09SOC_EXT_AON_LEN                 (1U)
#define AON_LDO09SOC_EXT_AON_MSK                 (((1U << AON_LDO09SOC_EXT_AON_LEN) - 1) << AON_LDO09SOC_EXT_AON_POS)
#define AON_LDO09SOC_EXT_AON_UMSK                (~(((1U << AON_LDO09SOC_EXT_AON_LEN) - 1) << AON_LDO09SOC_EXT_AON_POS))
#define AON_LDO09SOC_SLOW_PLD_AON                AON_LDO09SOC_SLOW_PLD_AON
#define AON_LDO09SOC_SLOW_PLD_AON_POS            (2U)
#define AON_LDO09SOC_SLOW_PLD_AON_LEN            (1U)
#define AON_LDO09SOC_SLOW_PLD_AON_MSK            (((1U << AON_LDO09SOC_SLOW_PLD_AON_LEN) - 1) << AON_LDO09SOC_SLOW_PLD_AON_POS)
#define AON_LDO09SOC_SLOW_PLD_AON_UMSK           (~(((1U << AON_LDO09SOC_SLOW_PLD_AON_LEN) - 1) << AON_LDO09SOC_SLOW_PLD_AON_POS))
#define AON_LDO09SOC_CC_AON                      AON_LDO09SOC_CC_AON
#define AON_LDO09SOC_CC_AON_POS                  (4U)
#define AON_LDO09SOC_CC_AON_LEN                  (3U)
#define AON_LDO09SOC_CC_AON_MSK                  (((1U << AON_LDO09SOC_CC_AON_LEN) - 1) << AON_LDO09SOC_CC_AON_POS)
#define AON_LDO09SOC_CC_AON_UMSK                 (~(((1U << AON_LDO09SOC_CC_AON_LEN) - 1) << AON_LDO09SOC_CC_AON_POS))
#define AON_LDO09SOC_RZ_AON                      AON_LDO09SOC_RZ_AON
#define AON_LDO09SOC_RZ_AON_POS                  (8U)
#define AON_LDO09SOC_RZ_AON_LEN                  (2U)
#define AON_LDO09SOC_RZ_AON_MSK                  (((1U << AON_LDO09SOC_RZ_AON_LEN) - 1) << AON_LDO09SOC_RZ_AON_POS)
#define AON_LDO09SOC_RZ_AON_UMSK                 (~(((1U << AON_LDO09SOC_RZ_AON_LEN) - 1) << AON_LDO09SOC_RZ_AON_POS))
#define AON_LDO09SOC_VOUT_SEL_AON                AON_LDO09SOC_VOUT_SEL_AON
#define AON_LDO09SOC_VOUT_SEL_AON_POS            (12U)
#define AON_LDO09SOC_VOUT_SEL_AON_LEN            (4U)
#define AON_LDO09SOC_VOUT_SEL_AON_MSK            (((1U << AON_LDO09SOC_VOUT_SEL_AON_LEN) - 1) << AON_LDO09SOC_VOUT_SEL_AON_POS)
#define AON_LDO09SOC_VOUT_SEL_AON_UMSK           (~(((1U << AON_LDO09SOC_VOUT_SEL_AON_LEN) - 1) << AON_LDO09SOC_VOUT_SEL_AON_POS))
#define AON_LDO09SOC_VOUT_TRIM_AON               AON_LDO09SOC_VOUT_TRIM_AON
#define AON_LDO09SOC_VOUT_TRIM_AON_POS           (16U)
#define AON_LDO09SOC_VOUT_TRIM_AON_LEN           (4U)
#define AON_LDO09SOC_VOUT_TRIM_AON_MSK           (((1U << AON_LDO09SOC_VOUT_TRIM_AON_LEN) - 1) << AON_LDO09SOC_VOUT_TRIM_AON_POS)
#define AON_LDO09SOC_VOUT_TRIM_AON_UMSK          (~(((1U << AON_LDO09SOC_VOUT_TRIM_AON_LEN) - 1) << AON_LDO09SOC_VOUT_TRIM_AON_POS))
#define AON_LDO09SOC_PWR_GOOD                    AON_LDO09SOC_PWR_GOOD
#define AON_LDO09SOC_PWR_GOOD_POS                (20U)
#define AON_LDO09SOC_PWR_GOOD_LEN                (1U)
#define AON_LDO09SOC_PWR_GOOD_MSK                (((1U << AON_LDO09SOC_PWR_GOOD_LEN) - 1) << AON_LDO09SOC_PWR_GOOD_POS)
#define AON_LDO09SOC_PWR_GOOD_UMSK               (~(((1U << AON_LDO09SOC_PWR_GOOD_LEN) - 1) << AON_LDO09SOC_PWR_GOOD_POS))
#define AON_LDO09_RDY_AON                        AON_LDO09_RDY_AON
#define AON_LDO09_RDY_AON_POS                    (21U)
#define AON_LDO09_RDY_AON_LEN                    (1U)
#define AON_LDO09_RDY_AON_MSK                    (((1U << AON_LDO09_RDY_AON_LEN) - 1) << AON_LDO09_RDY_AON_POS)
#define AON_LDO09_RDY_AON_UMSK                   (~(((1U << AON_LDO09_RDY_AON_LEN) - 1) << AON_LDO09_RDY_AON_POS))

/* 0x830 : Digital Control */
#define AON_DCDC12_0_OFFSET                      (0x830)
#define AON_DCDC12_BM_LP_AON                     AON_DCDC12_BM_LP_AON
#define AON_DCDC12_BM_LP_AON_POS                 (0U)
#define AON_DCDC12_BM_LP_AON_LEN                 (3U)
#define AON_DCDC12_BM_LP_AON_MSK                 (((1U << AON_DCDC12_BM_LP_AON_LEN) - 1) << AON_DCDC12_BM_LP_AON_POS)
#define AON_DCDC12_BM_LP_AON_UMSK                (~(((1U << AON_DCDC12_BM_LP_AON_LEN) - 1) << AON_DCDC12_BM_LP_AON_POS))
#define AON_DCDC12_BM_NM_AON                     AON_DCDC12_BM_NM_AON
#define AON_DCDC12_BM_NM_AON_POS                 (4U)
#define AON_DCDC12_BM_NM_AON_LEN                 (3U)
#define AON_DCDC12_BM_NM_AON_MSK                 (((1U << AON_DCDC12_BM_NM_AON_LEN) - 1) << AON_DCDC12_BM_NM_AON_POS)
#define AON_DCDC12_BM_NM_AON_UMSK                (~(((1U << AON_DCDC12_BM_NM_AON_LEN) - 1) << AON_DCDC12_BM_NM_AON_POS))
#define AON_DCDC12_CFB_SEL_AON                   AON_DCDC12_CFB_SEL_AON
#define AON_DCDC12_CFB_SEL_AON_POS               (8U)
#define AON_DCDC12_CFB_SEL_AON_LEN               (4U)
#define AON_DCDC12_CFB_SEL_AON_MSK               (((1U << AON_DCDC12_CFB_SEL_AON_LEN) - 1) << AON_DCDC12_CFB_SEL_AON_POS)
#define AON_DCDC12_CFB_SEL_AON_UMSK              (~(((1U << AON_DCDC12_CFB_SEL_AON_LEN) - 1) << AON_DCDC12_CFB_SEL_AON_POS))
#define AON_DCDC12_CHF_SEL_AON                   AON_DCDC12_CHF_SEL_AON
#define AON_DCDC12_CHF_SEL_AON_POS               (12U)
#define AON_DCDC12_CHF_SEL_AON_LEN               (4U)
#define AON_DCDC12_CHF_SEL_AON_MSK               (((1U << AON_DCDC12_CHF_SEL_AON_LEN) - 1) << AON_DCDC12_CHF_SEL_AON_POS)
#define AON_DCDC12_CHF_SEL_AON_UMSK              (~(((1U << AON_DCDC12_CHF_SEL_AON_LEN) - 1) << AON_DCDC12_CHF_SEL_AON_POS))
#define AON_DCDC12_COMP_GM_SEL_AON               AON_DCDC12_COMP_GM_SEL_AON
#define AON_DCDC12_COMP_GM_SEL_AON_POS           (16U)
#define AON_DCDC12_COMP_GM_SEL_AON_LEN           (3U)
#define AON_DCDC12_COMP_GM_SEL_AON_MSK           (((1U << AON_DCDC12_COMP_GM_SEL_AON_LEN) - 1) << AON_DCDC12_COMP_GM_SEL_AON_POS)
#define AON_DCDC12_COMP_GM_SEL_AON_UMSK          (~(((1U << AON_DCDC12_COMP_GM_SEL_AON_LEN) - 1) << AON_DCDC12_COMP_GM_SEL_AON_POS))
#define AON_DCDC12_CS_DELAY_AON                  AON_DCDC12_CS_DELAY_AON
#define AON_DCDC12_CS_DELAY_AON_POS              (20U)
#define AON_DCDC12_CS_DELAY_AON_LEN              (3U)
#define AON_DCDC12_CS_DELAY_AON_MSK              (((1U << AON_DCDC12_CS_DELAY_AON_LEN) - 1) << AON_DCDC12_CS_DELAY_AON_POS)
#define AON_DCDC12_CS_DELAY_AON_UMSK             (~(((1U << AON_DCDC12_CS_DELAY_AON_LEN) - 1) << AON_DCDC12_CS_DELAY_AON_POS))
#define AON_DCDC12_DOUBLER_32K_AON               AON_DCDC12_DOUBLER_32K_AON
#define AON_DCDC12_DOUBLER_32K_AON_POS           (23U)
#define AON_DCDC12_DOUBLER_32K_AON_LEN           (1U)
#define AON_DCDC12_DOUBLER_32K_AON_MSK           (((1U << AON_DCDC12_DOUBLER_32K_AON_LEN) - 1) << AON_DCDC12_DOUBLER_32K_AON_POS)
#define AON_DCDC12_DOUBLER_32K_AON_UMSK          (~(((1U << AON_DCDC12_DOUBLER_32K_AON_LEN) - 1) << AON_DCDC12_DOUBLER_32K_AON_POS))
#define AON_DCDC12_DRV_SR_AON                    AON_DCDC12_DRV_SR_AON
#define AON_DCDC12_DRV_SR_AON_POS                (24U)
#define AON_DCDC12_DRV_SR_AON_LEN                (2U)
#define AON_DCDC12_DRV_SR_AON_MSK                (((1U << AON_DCDC12_DRV_SR_AON_LEN) - 1) << AON_DCDC12_DRV_SR_AON_POS)
#define AON_DCDC12_DRV_SR_AON_UMSK               (~(((1U << AON_DCDC12_DRV_SR_AON_LEN) - 1) << AON_DCDC12_DRV_SR_AON_POS))
#define AON_DCDC12_EN_ANTIRING_AON               AON_DCDC12_EN_ANTIRING_AON
#define AON_DCDC12_EN_ANTIRING_AON_POS           (26U)
#define AON_DCDC12_EN_ANTIRING_AON_LEN           (1U)
#define AON_DCDC12_EN_ANTIRING_AON_MSK           (((1U << AON_DCDC12_EN_ANTIRING_AON_LEN) - 1) << AON_DCDC12_EN_ANTIRING_AON_POS)
#define AON_DCDC12_EN_ANTIRING_AON_UMSK          (~(((1U << AON_DCDC12_EN_ANTIRING_AON_LEN) - 1) << AON_DCDC12_EN_ANTIRING_AON_POS))
#define AON_DCDC12_ANTIRING_BYP_LP_AON           AON_DCDC12_ANTIRING_BYP_LP_AON
#define AON_DCDC12_ANTIRING_BYP_LP_AON_POS       (27U)
#define AON_DCDC12_ANTIRING_BYP_LP_AON_LEN       (1U)
#define AON_DCDC12_ANTIRING_BYP_LP_AON_MSK       (((1U << AON_DCDC12_ANTIRING_BYP_LP_AON_LEN) - 1) << AON_DCDC12_ANTIRING_BYP_LP_AON_POS)
#define AON_DCDC12_ANTIRING_BYP_LP_AON_UMSK      (~(((1U << AON_DCDC12_ANTIRING_BYP_LP_AON_LEN) - 1) << AON_DCDC12_ANTIRING_BYP_LP_AON_POS))
#define AON_DCDC12_ISENSE_TRIM_AON               AON_DCDC12_ISENSE_TRIM_AON
#define AON_DCDC12_ISENSE_TRIM_AON_POS           (28U)
#define AON_DCDC12_ISENSE_TRIM_AON_LEN           (3U)
#define AON_DCDC12_ISENSE_TRIM_AON_MSK           (((1U << AON_DCDC12_ISENSE_TRIM_AON_LEN) - 1) << AON_DCDC12_ISENSE_TRIM_AON_POS)
#define AON_DCDC12_ISENSE_TRIM_AON_UMSK          (~(((1U << AON_DCDC12_ISENSE_TRIM_AON_LEN) - 1) << AON_DCDC12_ISENSE_TRIM_AON_POS))
#define AON_DCDC12_FORCE_EN_CS_ZVS_AON           AON_DCDC12_FORCE_EN_CS_ZVS_AON
#define AON_DCDC12_FORCE_EN_CS_ZVS_AON_POS       (31U)
#define AON_DCDC12_FORCE_EN_CS_ZVS_AON_LEN       (1U)
#define AON_DCDC12_FORCE_EN_CS_ZVS_AON_MSK       (((1U << AON_DCDC12_FORCE_EN_CS_ZVS_AON_LEN) - 1) << AON_DCDC12_FORCE_EN_CS_ZVS_AON_POS)
#define AON_DCDC12_FORCE_EN_CS_ZVS_AON_UMSK      (~(((1U << AON_DCDC12_FORCE_EN_CS_ZVS_AON_LEN) - 1) << AON_DCDC12_FORCE_EN_CS_ZVS_AON_POS))

/* 0x834 : Digital Control */
#define AON_DCDC12_1_OFFSET                      (0x834)
#define AON_DCDC12_LP_FORCE_EN_AON               AON_DCDC12_LP_FORCE_EN_AON
#define AON_DCDC12_LP_FORCE_EN_AON_POS           (1U)
#define AON_DCDC12_LP_FORCE_EN_AON_LEN           (1U)
#define AON_DCDC12_LP_FORCE_EN_AON_MSK           (((1U << AON_DCDC12_LP_FORCE_EN_AON_LEN) - 1) << AON_DCDC12_LP_FORCE_EN_AON_POS)
#define AON_DCDC12_LP_FORCE_EN_AON_UMSK          (~(((1U << AON_DCDC12_LP_FORCE_EN_AON_LEN) - 1) << AON_DCDC12_LP_FORCE_EN_AON_POS))
#define AON_DCDC12_NONOVERLAP_TD_AON             AON_DCDC12_NONOVERLAP_TD_AON
#define AON_DCDC12_NONOVERLAP_TD_AON_POS         (4U)
#define AON_DCDC12_NONOVERLAP_TD_AON_LEN         (5U)
#define AON_DCDC12_NONOVERLAP_TD_AON_MSK         (((1U << AON_DCDC12_NONOVERLAP_TD_AON_LEN) - 1) << AON_DCDC12_NONOVERLAP_TD_AON_POS)
#define AON_DCDC12_NONOVERLAP_TD_AON_UMSK        (~(((1U << AON_DCDC12_NONOVERLAP_TD_AON_LEN) - 1) << AON_DCDC12_NONOVERLAP_TD_AON_POS))
#define AON_DCDC12_OCP_LP_TW_AON                 AON_DCDC12_OCP_LP_TW_AON
#define AON_DCDC12_OCP_LP_TW_AON_POS             (12U)
#define AON_DCDC12_OCP_LP_TW_AON_LEN             (2U)
#define AON_DCDC12_OCP_LP_TW_AON_MSK             (((1U << AON_DCDC12_OCP_LP_TW_AON_LEN) - 1) << AON_DCDC12_OCP_LP_TW_AON_POS)
#define AON_DCDC12_OCP_LP_TW_AON_UMSK            (~(((1U << AON_DCDC12_OCP_LP_TW_AON_LEN) - 1) << AON_DCDC12_OCP_LP_TW_AON_POS))
#define AON_DCDC12_OCP_OUT_AON                   AON_DCDC12_OCP_OUT_AON
#define AON_DCDC12_OCP_OUT_AON_POS               (14U)
#define AON_DCDC12_OCP_OUT_AON_LEN               (1U)
#define AON_DCDC12_OCP_OUT_AON_MSK               (((1U << AON_DCDC12_OCP_OUT_AON_LEN) - 1) << AON_DCDC12_OCP_OUT_AON_POS)
#define AON_DCDC12_OCP_OUT_AON_UMSK              (~(((1U << AON_DCDC12_OCP_OUT_AON_LEN) - 1) << AON_DCDC12_OCP_OUT_AON_POS))
#define AON_DCDC12_OCP_RST_AON                   AON_DCDC12_OCP_RST_AON
#define AON_DCDC12_OCP_RST_AON_POS               (15U)
#define AON_DCDC12_OCP_RST_AON_LEN               (1U)
#define AON_DCDC12_OCP_RST_AON_MSK               (((1U << AON_DCDC12_OCP_RST_AON_LEN) - 1) << AON_DCDC12_OCP_RST_AON_POS)
#define AON_DCDC12_OCP_RST_AON_UMSK              (~(((1U << AON_DCDC12_OCP_RST_AON_LEN) - 1) << AON_DCDC12_OCP_RST_AON_POS))
#define AON_DCDC12_OCP_VTH_AON                   AON_DCDC12_OCP_VTH_AON
#define AON_DCDC12_OCP_VTH_AON_POS               (16U)
#define AON_DCDC12_OCP_VTH_AON_LEN               (3U)
#define AON_DCDC12_OCP_VTH_AON_MSK               (((1U << AON_DCDC12_OCP_VTH_AON_LEN) - 1) << AON_DCDC12_OCP_VTH_AON_POS)
#define AON_DCDC12_OCP_VTH_AON_UMSK              (~(((1U << AON_DCDC12_OCP_VTH_AON_LEN) - 1) << AON_DCDC12_OCP_VTH_AON_POS))
#define AON_DCDC12_OSC_2M_MODE_AON               AON_DCDC12_OSC_2M_MODE_AON
#define AON_DCDC12_OSC_2M_MODE_AON_POS           (20U)
#define AON_DCDC12_OSC_2M_MODE_AON_LEN           (1U)
#define AON_DCDC12_OSC_2M_MODE_AON_MSK           (((1U << AON_DCDC12_OSC_2M_MODE_AON_LEN) - 1) << AON_DCDC12_OSC_2M_MODE_AON_POS)
#define AON_DCDC12_OSC_2M_MODE_AON_UMSK          (~(((1U << AON_DCDC12_OSC_2M_MODE_AON_LEN) - 1) << AON_DCDC12_OSC_2M_MODE_AON_POS))
#define AON_DCDC12_OSC_EN_INHIBIT_T2_AON         AON_DCDC12_OSC_EN_INHIBIT_T2_AON
#define AON_DCDC12_OSC_EN_INHIBIT_T2_AON_POS     (21U)
#define AON_DCDC12_OSC_EN_INHIBIT_T2_AON_LEN     (1U)
#define AON_DCDC12_OSC_EN_INHIBIT_T2_AON_MSK     (((1U << AON_DCDC12_OSC_EN_INHIBIT_T2_AON_LEN) - 1) << AON_DCDC12_OSC_EN_INHIBIT_T2_AON_POS)
#define AON_DCDC12_OSC_EN_INHIBIT_T2_AON_UMSK    (~(((1U << AON_DCDC12_OSC_EN_INHIBIT_T2_AON_LEN) - 1) << AON_DCDC12_OSC_EN_INHIBIT_T2_AON_POS))
#define AON_DCDC12_OSC_EN_SLOW_AON               AON_DCDC12_OSC_EN_SLOW_AON
#define AON_DCDC12_OSC_EN_SLOW_AON_POS           (22U)
#define AON_DCDC12_OSC_EN_SLOW_AON_LEN           (1U)
#define AON_DCDC12_OSC_EN_SLOW_AON_MSK           (((1U << AON_DCDC12_OSC_EN_SLOW_AON_LEN) - 1) << AON_DCDC12_OSC_EN_SLOW_AON_POS)
#define AON_DCDC12_OSC_EN_SLOW_AON_UMSK          (~(((1U << AON_DCDC12_OSC_EN_SLOW_AON_LEN) - 1) << AON_DCDC12_OSC_EN_SLOW_AON_POS))
#define AON_DCDC12_OSC_EN_STOP_AON               AON_DCDC12_OSC_EN_STOP_AON
#define AON_DCDC12_OSC_EN_STOP_AON_POS           (23U)
#define AON_DCDC12_OSC_EN_STOP_AON_LEN           (1U)
#define AON_DCDC12_OSC_EN_STOP_AON_MSK           (((1U << AON_DCDC12_OSC_EN_STOP_AON_LEN) - 1) << AON_DCDC12_OSC_EN_STOP_AON_POS)
#define AON_DCDC12_OSC_EN_STOP_AON_UMSK          (~(((1U << AON_DCDC12_OSC_EN_STOP_AON_LEN) - 1) << AON_DCDC12_OSC_EN_STOP_AON_POS))
#define AON_DCDC12_OSC_FREQ_TRIM_AON             AON_DCDC12_OSC_FREQ_TRIM_AON
#define AON_DCDC12_OSC_FREQ_TRIM_AON_POS         (24U)
#define AON_DCDC12_OSC_FREQ_TRIM_AON_LEN         (4U)
#define AON_DCDC12_OSC_FREQ_TRIM_AON_MSK         (((1U << AON_DCDC12_OSC_FREQ_TRIM_AON_LEN) - 1) << AON_DCDC12_OSC_FREQ_TRIM_AON_POS)
#define AON_DCDC12_OSC_FREQ_TRIM_AON_UMSK        (~(((1U << AON_DCDC12_OSC_FREQ_TRIM_AON_LEN) - 1) << AON_DCDC12_OSC_FREQ_TRIM_AON_POS))
#define AON_DCDC12_PULLDOWN_AON                  AON_DCDC12_PULLDOWN_AON
#define AON_DCDC12_PULLDOWN_AON_POS              (28U)
#define AON_DCDC12_PULLDOWN_AON_LEN              (1U)
#define AON_DCDC12_PULLDOWN_AON_MSK              (((1U << AON_DCDC12_PULLDOWN_AON_LEN) - 1) << AON_DCDC12_PULLDOWN_AON_POS)
#define AON_DCDC12_PULLDOWN_AON_UMSK             (~(((1U << AON_DCDC12_PULLDOWN_AON_LEN) - 1) << AON_DCDC12_PULLDOWN_AON_POS))

/* 0x838 : Digital Control */
#define AON_DCDC12_2_OFFSET                      (0x838)
#define AON_DCDC12_OSC_SS_FDEV_AON               AON_DCDC12_OSC_SS_FDEV_AON
#define AON_DCDC12_OSC_SS_FDEV_AON_POS           (0U)
#define AON_DCDC12_OSC_SS_FDEV_AON_LEN           (2U)
#define AON_DCDC12_OSC_SS_FDEV_AON_MSK           (((1U << AON_DCDC12_OSC_SS_FDEV_AON_LEN) - 1) << AON_DCDC12_OSC_SS_FDEV_AON_POS)
#define AON_DCDC12_OSC_SS_FDEV_AON_UMSK          (~(((1U << AON_DCDC12_OSC_SS_FDEV_AON_LEN) - 1) << AON_DCDC12_OSC_SS_FDEV_AON_POS))
#define AON_DCDC12_OSC_SS_PERIOD_AON             AON_DCDC12_OSC_SS_PERIOD_AON
#define AON_DCDC12_OSC_SS_PERIOD_AON_POS         (2U)
#define AON_DCDC12_OSC_SS_PERIOD_AON_LEN         (2U)
#define AON_DCDC12_OSC_SS_PERIOD_AON_MSK         (((1U << AON_DCDC12_OSC_SS_PERIOD_AON_LEN) - 1) << AON_DCDC12_OSC_SS_PERIOD_AON_POS)
#define AON_DCDC12_OSC_SS_PERIOD_AON_UMSK        (~(((1U << AON_DCDC12_OSC_SS_PERIOD_AON_LEN) - 1) << AON_DCDC12_OSC_SS_PERIOD_AON_POS))
#define AON_DCDC12_OSC_SS_RSTN_AON               AON_DCDC12_OSC_SS_RSTN_AON
#define AON_DCDC12_OSC_SS_RSTN_AON_POS           (4U)
#define AON_DCDC12_OSC_SS_RSTN_AON_LEN           (1U)
#define AON_DCDC12_OSC_SS_RSTN_AON_MSK           (((1U << AON_DCDC12_OSC_SS_RSTN_AON_LEN) - 1) << AON_DCDC12_OSC_SS_RSTN_AON_POS)
#define AON_DCDC12_OSC_SS_RSTN_AON_UMSK          (~(((1U << AON_DCDC12_OSC_SS_RSTN_AON_LEN) - 1) << AON_DCDC12_OSC_SS_RSTN_AON_POS))
#define AON_DCDC12_OSC_SS_EN_AON                 AON_DCDC12_OSC_SS_EN_AON
#define AON_DCDC12_OSC_SS_EN_AON_POS             (5U)
#define AON_DCDC12_OSC_SS_EN_AON_LEN             (1U)
#define AON_DCDC12_OSC_SS_EN_AON_MSK             (((1U << AON_DCDC12_OSC_SS_EN_AON_LEN) - 1) << AON_DCDC12_OSC_SS_EN_AON_POS)
#define AON_DCDC12_OSC_SS_EN_AON_UMSK            (~(((1U << AON_DCDC12_OSC_SS_EN_AON_LEN) - 1) << AON_DCDC12_OSC_SS_EN_AON_POS))
#define AON_DCDC12_PWM_LP_VTH_AON                AON_DCDC12_PWM_LP_VTH_AON
#define AON_DCDC12_PWM_LP_VTH_AON_POS            (8U)
#define AON_DCDC12_PWM_LP_VTH_AON_LEN            (3U)
#define AON_DCDC12_PWM_LP_VTH_AON_MSK            (((1U << AON_DCDC12_PWM_LP_VTH_AON_LEN) - 1) << AON_DCDC12_PWM_LP_VTH_AON_POS)
#define AON_DCDC12_PWM_LP_VTH_AON_UMSK           (~(((1U << AON_DCDC12_PWM_LP_VTH_AON_LEN) - 1) << AON_DCDC12_PWM_LP_VTH_AON_POS))
#define AON_DCDC12_RC_SEL_AON                    AON_DCDC12_RC_SEL_AON
#define AON_DCDC12_RC_SEL_AON_POS                (12U)
#define AON_DCDC12_RC_SEL_AON_LEN                (4U)
#define AON_DCDC12_RC_SEL_AON_MSK                (((1U << AON_DCDC12_RC_SEL_AON_LEN) - 1) << AON_DCDC12_RC_SEL_AON_POS)
#define AON_DCDC12_RC_SEL_AON_UMSK               (~(((1U << AON_DCDC12_RC_SEL_AON_LEN) - 1) << AON_DCDC12_RC_SEL_AON_POS))
#define AON_DCDC12_RDY_AON                       AON_DCDC12_RDY_AON
#define AON_DCDC12_RDY_AON_POS                   (16U)
#define AON_DCDC12_RDY_AON_LEN                   (1U)
#define AON_DCDC12_RDY_AON_MSK                   (((1U << AON_DCDC12_RDY_AON_LEN) - 1) << AON_DCDC12_RDY_AON_POS)
#define AON_DCDC12_RDY_AON_UMSK                  (~(((1U << AON_DCDC12_RDY_AON_LEN) - 1) << AON_DCDC12_RDY_AON_POS))
#define AON_DCDC12_SLOPE_CURR_SEL_AON            AON_DCDC12_SLOPE_CURR_SEL_AON
#define AON_DCDC12_SLOPE_CURR_SEL_AON_POS        (20U)
#define AON_DCDC12_SLOPE_CURR_SEL_AON_LEN        (5U)
#define AON_DCDC12_SLOPE_CURR_SEL_AON_MSK        (((1U << AON_DCDC12_SLOPE_CURR_SEL_AON_LEN) - 1) << AON_DCDC12_SLOPE_CURR_SEL_AON_POS)
#define AON_DCDC12_SLOPE_CURR_SEL_AON_UMSK       (~(((1U << AON_DCDC12_SLOPE_CURR_SEL_AON_LEN) - 1) << AON_DCDC12_SLOPE_CURR_SEL_AON_POS))
#define AON_DCDC12_SSTART_TIME_AON               AON_DCDC12_SSTART_TIME_AON
#define AON_DCDC12_SSTART_TIME_AON_POS           (28U)
#define AON_DCDC12_SSTART_TIME_AON_LEN           (2U)
#define AON_DCDC12_SSTART_TIME_AON_MSK           (((1U << AON_DCDC12_SSTART_TIME_AON_LEN) - 1) << AON_DCDC12_SSTART_TIME_AON_POS)
#define AON_DCDC12_SSTART_TIME_AON_UMSK          (~(((1U << AON_DCDC12_SSTART_TIME_AON_LEN) - 1) << AON_DCDC12_SSTART_TIME_AON_POS))

/* 0x83C : Digital Control */
#define AON_DCDC12_3_OFFSET                      (0x83C)
#define AON_DCDC12_VC_CLAMP_VTH_AON              AON_DCDC12_VC_CLAMP_VTH_AON
#define AON_DCDC12_VC_CLAMP_VTH_AON_POS          (0U)
#define AON_DCDC12_VC_CLAMP_VTH_AON_LEN          (3U)
#define AON_DCDC12_VC_CLAMP_VTH_AON_MSK          (((1U << AON_DCDC12_VC_CLAMP_VTH_AON_LEN) - 1) << AON_DCDC12_VC_CLAMP_VTH_AON_POS)
#define AON_DCDC12_VC_CLAMP_VTH_AON_UMSK         (~(((1U << AON_DCDC12_VC_CLAMP_VTH_AON_LEN) - 1) << AON_DCDC12_VC_CLAMP_VTH_AON_POS))
#define AON_DCDC12_VOUT_SEL_AON                  AON_DCDC12_VOUT_SEL_AON
#define AON_DCDC12_VOUT_SEL_AON_POS              (4U)
#define AON_DCDC12_VOUT_SEL_AON_LEN              (5U)
#define AON_DCDC12_VOUT_SEL_AON_MSK              (((1U << AON_DCDC12_VOUT_SEL_AON_LEN) - 1) << AON_DCDC12_VOUT_SEL_AON_POS)
#define AON_DCDC12_VOUT_SEL_AON_UMSK             (~(((1U << AON_DCDC12_VOUT_SEL_AON_LEN) - 1) << AON_DCDC12_VOUT_SEL_AON_POS))
#define AON_DCDC12_VOUT_TRIM_AON                 AON_DCDC12_VOUT_TRIM_AON
#define AON_DCDC12_VOUT_TRIM_AON_POS             (12U)
#define AON_DCDC12_VOUT_TRIM_AON_LEN             (4U)
#define AON_DCDC12_VOUT_TRIM_AON_MSK             (((1U << AON_DCDC12_VOUT_TRIM_AON_LEN) - 1) << AON_DCDC12_VOUT_TRIM_AON_POS)
#define AON_DCDC12_VOUT_TRIM_AON_UMSK            (~(((1U << AON_DCDC12_VOUT_TRIM_AON_LEN) - 1) << AON_DCDC12_VOUT_TRIM_AON_POS))
#define AON_DCDC12_VPFM_AON                      AON_DCDC12_VPFM_AON
#define AON_DCDC12_VPFM_AON_POS                  (16U)
#define AON_DCDC12_VPFM_AON_LEN                  (4U)
#define AON_DCDC12_VPFM_AON_MSK                  (((1U << AON_DCDC12_VPFM_AON_LEN) - 1) << AON_DCDC12_VPFM_AON_POS)
#define AON_DCDC12_VPFM_AON_UMSK                 (~(((1U << AON_DCDC12_VPFM_AON_LEN) - 1) << AON_DCDC12_VPFM_AON_POS))
#define AON_DCDC12_ZVS_TD_OPT_AON                AON_DCDC12_ZVS_TD_OPT_AON
#define AON_DCDC12_ZVS_TD_OPT_AON_POS            (20U)
#define AON_DCDC12_ZVS_TD_OPT_AON_LEN            (3U)
#define AON_DCDC12_ZVS_TD_OPT_AON_MSK            (((1U << AON_DCDC12_ZVS_TD_OPT_AON_LEN) - 1) << AON_DCDC12_ZVS_TD_OPT_AON_POS)
#define AON_DCDC12_ZVS_TD_OPT_AON_UMSK           (~(((1U << AON_DCDC12_ZVS_TD_OPT_AON_LEN) - 1) << AON_DCDC12_ZVS_TD_OPT_AON_POS))
#define AON_PMIP_RESV_AON                        AON_PMIP_RESV_AON
#define AON_PMIP_RESV_AON_POS                    (24U)
#define AON_PMIP_RESV_AON_LEN                    (8U)
#define AON_PMIP_RESV_AON_MSK                    (((1U << AON_PMIP_RESV_AON_LEN) - 1) << AON_PMIP_RESV_AON_POS)
#define AON_PMIP_RESV_AON_UMSK                   (~(((1U << AON_PMIP_RESV_AON_LEN) - 1) << AON_PMIP_RESV_AON_POS))

/* 0x840 : Digital Control */
#define AON_PSW_OFFSET                           (0x840)
#define AON_PU_PSW18_PERI_AON                    AON_PU_PSW18_PERI_AON
#define AON_PU_PSW18_PERI_AON_POS                (4U)
#define AON_PU_PSW18_PERI_AON_LEN                (1U)
#define AON_PU_PSW18_PERI_AON_MSK                (((1U << AON_PU_PSW18_PERI_AON_LEN) - 1) << AON_PU_PSW18_PERI_AON_POS)
#define AON_PU_PSW18_PERI_AON_UMSK               (~(((1U << AON_PU_PSW18_PERI_AON_LEN) - 1) << AON_PU_PSW18_PERI_AON_POS))

/* 0x844 : Digital Control */
#define AON_TEN_OFFSET                           (0x844)
#define AON_TMUX_AON                             AON_TMUX_AON
#define AON_TMUX_AON_POS                         (0U)
#define AON_TMUX_AON_LEN                         (3U)
#define AON_TMUX_AON_MSK                         (((1U << AON_TMUX_AON_LEN) - 1) << AON_TMUX_AON_POS)
#define AON_TMUX_AON_UMSK                        (~(((1U << AON_TMUX_AON_LEN) - 1) << AON_TMUX_AON_POS))
#define AON_PMIP_DC_TP_OUT_EN_AON                AON_PMIP_DC_TP_OUT_EN_AON
#define AON_PMIP_DC_TP_OUT_EN_AON_POS            (4U)
#define AON_PMIP_DC_TP_OUT_EN_AON_LEN            (1U)
#define AON_PMIP_DC_TP_OUT_EN_AON_MSK            (((1U << AON_PMIP_DC_TP_OUT_EN_AON_LEN) - 1) << AON_PMIP_DC_TP_OUT_EN_AON_POS)
#define AON_PMIP_DC_TP_OUT_EN_AON_UMSK           (~(((1U << AON_PMIP_DC_TP_OUT_EN_AON_LEN) - 1) << AON_PMIP_DC_TP_OUT_EN_AON_POS))
#define AON_DC_TP_OUT_EN_AON                     AON_DC_TP_OUT_EN_AON
#define AON_DC_TP_OUT_EN_AON_POS                 (6U)
#define AON_DC_TP_OUT_EN_AON_LEN                 (1U)
#define AON_DC_TP_OUT_EN_AON_MSK                 (((1U << AON_DC_TP_OUT_EN_AON_LEN) - 1) << AON_DC_TP_OUT_EN_AON_POS)
#define AON_DC_TP_OUT_EN_AON_UMSK                (~(((1U << AON_DC_TP_OUT_EN_AON_LEN) - 1) << AON_DC_TP_OUT_EN_AON_POS))
#define AON_TEN_AON                              AON_TEN_AON
#define AON_TEN_AON_POS                          (8U)
#define AON_TEN_AON_LEN                          (1U)
#define AON_TEN_AON_MSK                          (((1U << AON_TEN_AON_LEN) - 1) << AON_TEN_AON_POS)
#define AON_TEN_AON_UMSK                         (~(((1U << AON_TEN_AON_LEN) - 1) << AON_TEN_AON_POS))
#define AON_TEN_BGSYS_AON                        AON_TEN_BGSYS_AON
#define AON_TEN_BGSYS_AON_POS                    (9U)
#define AON_TEN_BGSYS_AON_LEN                    (1U)
#define AON_TEN_BGSYS_AON_MSK                    (((1U << AON_TEN_BGSYS_AON_LEN) - 1) << AON_TEN_BGSYS_AON_POS)
#define AON_TEN_BGSYS_AON_UMSK                   (~(((1U << AON_TEN_BGSYS_AON_LEN) - 1) << AON_TEN_BGSYS_AON_POS))
#define AON_TEN_DCDC12_0_AON                     AON_TEN_DCDC12_0_AON
#define AON_TEN_DCDC12_0_AON_POS                 (10U)
#define AON_TEN_DCDC12_0_AON_LEN                 (1U)
#define AON_TEN_DCDC12_0_AON_MSK                 (((1U << AON_TEN_DCDC12_0_AON_LEN) - 1) << AON_TEN_DCDC12_0_AON_POS)
#define AON_TEN_DCDC12_0_AON_UMSK                (~(((1U << AON_TEN_DCDC12_0_AON_LEN) - 1) << AON_TEN_DCDC12_0_AON_POS))
#define AON_TEN_DCDC12_1_AON                     AON_TEN_DCDC12_1_AON
#define AON_TEN_DCDC12_1_AON_POS                 (11U)
#define AON_TEN_DCDC12_1_AON_LEN                 (1U)
#define AON_TEN_DCDC12_1_AON_MSK                 (((1U << AON_TEN_DCDC12_1_AON_LEN) - 1) << AON_TEN_DCDC12_1_AON_POS)
#define AON_TEN_DCDC12_1_AON_UMSK                (~(((1U << AON_TEN_DCDC12_1_AON_LEN) - 1) << AON_TEN_DCDC12_1_AON_POS))
#define AON_TEN_LDO09SOC_AON                     AON_TEN_LDO09SOC_AON
#define AON_TEN_LDO09SOC_AON_POS                 (12U)
#define AON_TEN_LDO09SOC_AON_LEN                 (1U)
#define AON_TEN_LDO09SOC_AON_MSK                 (((1U << AON_TEN_LDO09SOC_AON_LEN) - 1) << AON_TEN_LDO09SOC_AON_POS)
#define AON_TEN_LDO09SOC_AON_UMSK                (~(((1U << AON_TEN_LDO09SOC_AON_LEN) - 1) << AON_TEN_LDO09SOC_AON_POS))
#define AON_TEN_RC32M                            AON_TEN_RC32M
#define AON_TEN_RC32M_POS                        (13U)
#define AON_TEN_RC32M_LEN                        (1U)
#define AON_TEN_RC32M_MSK                        (((1U << AON_TEN_RC32M_LEN) - 1) << AON_TEN_RC32M_POS)
#define AON_TEN_RC32M_UMSK                       (~(((1U << AON_TEN_RC32M_LEN) - 1) << AON_TEN_RC32M_POS))
#define AON_TEN_XTAL32K_AON                      AON_TEN_XTAL32K_AON
#define AON_TEN_XTAL32K_AON_POS                  (16U)
#define AON_TEN_XTAL32K_AON_LEN                  (1U)
#define AON_TEN_XTAL32K_AON_MSK                  (((1U << AON_TEN_XTAL32K_AON_LEN) - 1) << AON_TEN_XTAL32K_AON_POS)
#define AON_TEN_XTAL32K_AON_UMSK                 (~(((1U << AON_TEN_XTAL32K_AON_LEN) - 1) << AON_TEN_XTAL32K_AON_POS))
#define AON_DTEN_XTAL32K_CLK_AON                 AON_DTEN_XTAL32K_CLK_AON
#define AON_DTEN_XTAL32K_CLK_AON_POS             (20U)
#define AON_DTEN_XTAL32K_CLK_AON_LEN             (1U)
#define AON_DTEN_XTAL32K_CLK_AON_MSK             (((1U << AON_DTEN_XTAL32K_CLK_AON_LEN) - 1) << AON_DTEN_XTAL32K_CLK_AON_POS)
#define AON_DTEN_XTAL32K_CLK_AON_UMSK            (~(((1U << AON_DTEN_XTAL32K_CLK_AON_LEN) - 1) << AON_DTEN_XTAL32K_CLK_AON_POS))
#define AON_DTEN_XTAL32K_PU_AON                  AON_DTEN_XTAL32K_PU_AON
#define AON_DTEN_XTAL32K_PU_AON_POS              (21U)
#define AON_DTEN_XTAL32K_PU_AON_LEN              (1U)
#define AON_DTEN_XTAL32K_PU_AON_MSK              (((1U << AON_DTEN_XTAL32K_PU_AON_LEN) - 1) << AON_DTEN_XTAL32K_PU_AON_POS)
#define AON_DTEN_XTAL32K_PU_AON_UMSK             (~(((1U << AON_DTEN_XTAL32K_PU_AON_LEN) - 1) << AON_DTEN_XTAL32K_PU_AON_POS))
#define AON_DTEN_XTAL32K_RDY_AON                 AON_DTEN_XTAL32K_RDY_AON
#define AON_DTEN_XTAL32K_RDY_AON_POS             (22U)
#define AON_DTEN_XTAL32K_RDY_AON_LEN             (1U)
#define AON_DTEN_XTAL32K_RDY_AON_MSK             (((1U << AON_DTEN_XTAL32K_RDY_AON_LEN) - 1) << AON_DTEN_XTAL32K_RDY_AON_POS)
#define AON_DTEN_XTAL32K_RDY_AON_UMSK            (~(((1U << AON_DTEN_XTAL32K_RDY_AON_LEN) - 1) << AON_DTEN_XTAL32K_RDY_AON_POS))
#define AON_PMIP_DC_TP_INT_OUT_EN_AON            AON_PMIP_DC_TP_INT_OUT_EN_AON
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_POS        (24U)
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_LEN        (1U)
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_MSK        (((1U << AON_PMIP_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_PMIP_DC_TP_INT_OUT_EN_AON_POS)
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_UMSK       (~(((1U << AON_PMIP_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_PMIP_DC_TP_INT_OUT_EN_AON_POS))
#define AON_DC_TP_INT_OUT_EN_AON                 AON_DC_TP_INT_OUT_EN_AON
#define AON_DC_TP_INT_OUT_EN_AON_POS             (25U)
#define AON_DC_TP_INT_OUT_EN_AON_LEN             (1U)
#define AON_DC_TP_INT_OUT_EN_AON_MSK             (((1U << AON_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_DC_TP_INT_OUT_EN_AON_POS)
#define AON_DC_TP_INT_OUT_EN_AON_UMSK            (~(((1U << AON_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_DC_TP_INT_OUT_EN_AON_POS))
#define AON_DTEN_RC32K_AON                       AON_DTEN_RC32K_AON
#define AON_DTEN_RC32K_AON_POS                   (26U)
#define AON_DTEN_RC32K_AON_LEN                   (1U)
#define AON_DTEN_RC32K_AON_MSK                   (((1U << AON_DTEN_RC32K_AON_LEN) - 1) << AON_DTEN_RC32K_AON_POS)
#define AON_DTEN_RC32K_AON_UMSK                  (~(((1U << AON_DTEN_RC32K_AON_LEN) - 1) << AON_DTEN_RC32K_AON_POS))
#define AON_DTEN_RC32M                           AON_DTEN_RC32M
#define AON_DTEN_RC32M_POS                       (27U)
#define AON_DTEN_RC32M_LEN                       (1U)
#define AON_DTEN_RC32M_MSK                       (((1U << AON_DTEN_RC32M_LEN) - 1) << AON_DTEN_RC32M_POS)
#define AON_DTEN_RC32M_UMSK                      (~(((1U << AON_DTEN_RC32M_LEN) - 1) << AON_DTEN_RC32M_POS))

/* 0x848 : Digital Control */
#define AON_LP_EN_HW_OFFSET                      (0x848)
#define AON_DCDC12_LP_EN_AON                     AON_DCDC12_LP_EN_AON
#define AON_DCDC12_LP_EN_AON_POS                 (0U)
#define AON_DCDC12_LP_EN_AON_LEN                 (1U)
#define AON_DCDC12_LP_EN_AON_MSK                 (((1U << AON_DCDC12_LP_EN_AON_LEN) - 1) << AON_DCDC12_LP_EN_AON_POS)
#define AON_DCDC12_LP_EN_AON_UMSK                (~(((1U << AON_DCDC12_LP_EN_AON_LEN) - 1) << AON_DCDC12_LP_EN_AON_POS))
#define AON_LDO09SOC_LP_EN_AON                   AON_LDO09SOC_LP_EN_AON
#define AON_LDO09SOC_LP_EN_AON_POS               (1U)
#define AON_LDO09SOC_LP_EN_AON_LEN               (1U)
#define AON_LDO09SOC_LP_EN_AON_MSK               (((1U << AON_LDO09SOC_LP_EN_AON_LEN) - 1) << AON_LDO09SOC_LP_EN_AON_POS)
#define AON_LDO09SOC_LP_EN_AON_UMSK              (~(((1U << AON_LDO09SOC_LP_EN_AON_LEN) - 1) << AON_LDO09SOC_LP_EN_AON_POS))
#define AON_LDO18AON_LP_EN_AON                   AON_LDO18AON_LP_EN_AON
#define AON_LDO18AON_LP_EN_AON_POS               (2U)
#define AON_LDO18AON_LP_EN_AON_LEN               (1U)
#define AON_LDO18AON_LP_EN_AON_MSK               (((1U << AON_LDO18AON_LP_EN_AON_LEN) - 1) << AON_LDO18AON_LP_EN_AON_POS)
#define AON_LDO18AON_LP_EN_AON_UMSK              (~(((1U << AON_LDO18AON_LP_EN_AON_LEN) - 1) << AON_LDO18AON_LP_EN_AON_POS))
#define AON_DCDC12_LP_EN_AON_CTRL_HW             AON_DCDC12_LP_EN_AON_CTRL_HW
#define AON_DCDC12_LP_EN_AON_CTRL_HW_POS         (4U)
#define AON_DCDC12_LP_EN_AON_CTRL_HW_LEN         (1U)
#define AON_DCDC12_LP_EN_AON_CTRL_HW_MSK         (((1U << AON_DCDC12_LP_EN_AON_CTRL_HW_LEN) - 1) << AON_DCDC12_LP_EN_AON_CTRL_HW_POS)
#define AON_DCDC12_LP_EN_AON_CTRL_HW_UMSK        (~(((1U << AON_DCDC12_LP_EN_AON_CTRL_HW_LEN) - 1) << AON_DCDC12_LP_EN_AON_CTRL_HW_POS))
#define AON_LDO09SOC_LP_EN_AON_CTRL_HW           AON_LDO09SOC_LP_EN_AON_CTRL_HW
#define AON_LDO09SOC_LP_EN_AON_CTRL_HW_POS       (5U)
#define AON_LDO09SOC_LP_EN_AON_CTRL_HW_LEN       (1U)
#define AON_LDO09SOC_LP_EN_AON_CTRL_HW_MSK       (((1U << AON_LDO09SOC_LP_EN_AON_CTRL_HW_LEN) - 1) << AON_LDO09SOC_LP_EN_AON_CTRL_HW_POS)
#define AON_LDO09SOC_LP_EN_AON_CTRL_HW_UMSK      (~(((1U << AON_LDO09SOC_LP_EN_AON_CTRL_HW_LEN) - 1) << AON_LDO09SOC_LP_EN_AON_CTRL_HW_POS))
#define AON_LDO18AON_LP_EN_AON_CTRL_HW           AON_LDO18AON_LP_EN_AON_CTRL_HW
#define AON_LDO18AON_LP_EN_AON_CTRL_HW_POS       (6U)
#define AON_LDO18AON_LP_EN_AON_CTRL_HW_LEN       (1U)
#define AON_LDO18AON_LP_EN_AON_CTRL_HW_MSK       (((1U << AON_LDO18AON_LP_EN_AON_CTRL_HW_LEN) - 1) << AON_LDO18AON_LP_EN_AON_CTRL_HW_POS)
#define AON_LDO18AON_LP_EN_AON_CTRL_HW_UMSK      (~(((1U << AON_LDO18AON_LP_EN_AON_CTRL_HW_LEN) - 1) << AON_LDO18AON_LP_EN_AON_CTRL_HW_POS))
#define AON_DCDC12_LP_EN_AON_HW                  AON_DCDC12_LP_EN_AON_HW
#define AON_DCDC12_LP_EN_AON_HW_POS              (8U)
#define AON_DCDC12_LP_EN_AON_HW_LEN              (1U)
#define AON_DCDC12_LP_EN_AON_HW_MSK              (((1U << AON_DCDC12_LP_EN_AON_HW_LEN) - 1) << AON_DCDC12_LP_EN_AON_HW_POS)
#define AON_DCDC12_LP_EN_AON_HW_UMSK             (~(((1U << AON_DCDC12_LP_EN_AON_HW_LEN) - 1) << AON_DCDC12_LP_EN_AON_HW_POS))
#define AON_LDO09SOC_LP_EN_AON_HW                AON_LDO09SOC_LP_EN_AON_HW
#define AON_LDO09SOC_LP_EN_AON_HW_POS            (9U)
#define AON_LDO09SOC_LP_EN_AON_HW_LEN            (1U)
#define AON_LDO09SOC_LP_EN_AON_HW_MSK            (((1U << AON_LDO09SOC_LP_EN_AON_HW_LEN) - 1) << AON_LDO09SOC_LP_EN_AON_HW_POS)
#define AON_LDO09SOC_LP_EN_AON_HW_UMSK           (~(((1U << AON_LDO09SOC_LP_EN_AON_HW_LEN) - 1) << AON_LDO09SOC_LP_EN_AON_HW_POS))
#define AON_LDO18AON_LP_EN_AON_HW                AON_LDO18AON_LP_EN_AON_HW
#define AON_LDO18AON_LP_EN_AON_HW_POS            (10U)
#define AON_LDO18AON_LP_EN_AON_HW_LEN            (1U)
#define AON_LDO18AON_LP_EN_AON_HW_MSK            (((1U << AON_LDO18AON_LP_EN_AON_HW_LEN) - 1) << AON_LDO18AON_LP_EN_AON_HW_POS)
#define AON_LDO18AON_LP_EN_AON_HW_UMSK           (~(((1U << AON_LDO18AON_LP_EN_AON_HW_LEN) - 1) << AON_LDO18AON_LP_EN_AON_HW_POS))

/* 0x84C : Digital Control */
#define AON_LP_EN_CTRL_OFFSET                    (0x84C)
#define AON_LDO09SOC_VOUT_SEL_AON_HW             AON_LDO09SOC_VOUT_SEL_AON_HW
#define AON_LDO09SOC_VOUT_SEL_AON_HW_POS         (0U)
#define AON_LDO09SOC_VOUT_SEL_AON_HW_LEN         (4U)
#define AON_LDO09SOC_VOUT_SEL_AON_HW_MSK         (((1U << AON_LDO09SOC_VOUT_SEL_AON_HW_LEN) - 1) << AON_LDO09SOC_VOUT_SEL_AON_HW_POS)
#define AON_LDO09SOC_VOUT_SEL_AON_HW_UMSK        (~(((1U << AON_LDO09SOC_VOUT_SEL_AON_HW_LEN) - 1) << AON_LDO09SOC_VOUT_SEL_AON_HW_POS))
#define AON_LDO18AON_VOUT_SEL_AON_HW             AON_LDO18AON_VOUT_SEL_AON_HW
#define AON_LDO18AON_VOUT_SEL_AON_HW_POS         (4U)
#define AON_LDO18AON_VOUT_SEL_AON_HW_LEN         (3U)
#define AON_LDO18AON_VOUT_SEL_AON_HW_MSK         (((1U << AON_LDO18AON_VOUT_SEL_AON_HW_LEN) - 1) << AON_LDO18AON_VOUT_SEL_AON_HW_POS)
#define AON_LDO18AON_VOUT_SEL_AON_HW_UMSK        (~(((1U << AON_LDO18AON_VOUT_SEL_AON_HW_LEN) - 1) << AON_LDO18AON_VOUT_SEL_AON_HW_POS))
#define AON_DCDC12_VOUT_SEL_AON_HW               AON_DCDC12_VOUT_SEL_AON_HW
#define AON_DCDC12_VOUT_SEL_AON_HW_POS           (8U)
#define AON_DCDC12_VOUT_SEL_AON_HW_LEN           (5U)
#define AON_DCDC12_VOUT_SEL_AON_HW_MSK           (((1U << AON_DCDC12_VOUT_SEL_AON_HW_LEN) - 1) << AON_DCDC12_VOUT_SEL_AON_HW_POS)
#define AON_DCDC12_VOUT_SEL_AON_HW_UMSK          (~(((1U << AON_DCDC12_VOUT_SEL_AON_HW_LEN) - 1) << AON_DCDC12_VOUT_SEL_AON_HW_POS))
#define AON_LDO09SOC_VOUT_SEL_AON_LP             AON_LDO09SOC_VOUT_SEL_AON_LP
#define AON_LDO09SOC_VOUT_SEL_AON_LP_POS         (16U)
#define AON_LDO09SOC_VOUT_SEL_AON_LP_LEN         (4U)
#define AON_LDO09SOC_VOUT_SEL_AON_LP_MSK         (((1U << AON_LDO09SOC_VOUT_SEL_AON_LP_LEN) - 1) << AON_LDO09SOC_VOUT_SEL_AON_LP_POS)
#define AON_LDO09SOC_VOUT_SEL_AON_LP_UMSK        (~(((1U << AON_LDO09SOC_VOUT_SEL_AON_LP_LEN) - 1) << AON_LDO09SOC_VOUT_SEL_AON_LP_POS))
#define AON_LDO18AON_VOUT_SEL_AON_LP             AON_LDO18AON_VOUT_SEL_AON_LP
#define AON_LDO18AON_VOUT_SEL_AON_LP_POS         (20U)
#define AON_LDO18AON_VOUT_SEL_AON_LP_LEN         (3U)
#define AON_LDO18AON_VOUT_SEL_AON_LP_MSK         (((1U << AON_LDO18AON_VOUT_SEL_AON_LP_LEN) - 1) << AON_LDO18AON_VOUT_SEL_AON_LP_POS)
#define AON_LDO18AON_VOUT_SEL_AON_LP_UMSK        (~(((1U << AON_LDO18AON_VOUT_SEL_AON_LP_LEN) - 1) << AON_LDO18AON_VOUT_SEL_AON_LP_POS))
#define AON_DCDC12_VOUT_SEL_AON_LP               AON_DCDC12_VOUT_SEL_AON_LP
#define AON_DCDC12_VOUT_SEL_AON_LP_POS           (24U)
#define AON_DCDC12_VOUT_SEL_AON_LP_LEN           (5U)
#define AON_DCDC12_VOUT_SEL_AON_LP_MSK           (((1U << AON_DCDC12_VOUT_SEL_AON_LP_LEN) - 1) << AON_DCDC12_VOUT_SEL_AON_LP_POS)
#define AON_DCDC12_VOUT_SEL_AON_LP_UMSK          (~(((1U << AON_DCDC12_VOUT_SEL_AON_LP_LEN) - 1) << AON_DCDC12_VOUT_SEL_AON_LP_POS))

/* 0x880 : rf_top_aon */
#define AON_RF_TOP_AON_OFFSET                    (0x880)
#define AON_PU_LDO15RF_AON                       AON_PU_LDO15RF_AON
#define AON_PU_LDO15RF_AON_POS                   (1U)
#define AON_PU_LDO15RF_AON_LEN                   (1U)
#define AON_PU_LDO15RF_AON_MSK                   (((1U << AON_PU_LDO15RF_AON_LEN) - 1) << AON_PU_LDO15RF_AON_POS)
#define AON_PU_LDO15RF_AON_UMSK                  (~(((1U << AON_PU_LDO15RF_AON_LEN) - 1) << AON_PU_LDO15RF_AON_POS))
#define AON_LDO15RF_SSTART_SEL_AON               AON_LDO15RF_SSTART_SEL_AON
#define AON_LDO15RF_SSTART_SEL_AON_POS           (8U)
#define AON_LDO15RF_SSTART_SEL_AON_LEN           (1U)
#define AON_LDO15RF_SSTART_SEL_AON_MSK           (((1U << AON_LDO15RF_SSTART_SEL_AON_LEN) - 1) << AON_LDO15RF_SSTART_SEL_AON_POS)
#define AON_LDO15RF_SSTART_SEL_AON_UMSK          (~(((1U << AON_LDO15RF_SSTART_SEL_AON_LEN) - 1) << AON_LDO15RF_SSTART_SEL_AON_POS))
#define AON_LDO15RF_SSTART_DELAY_AON             AON_LDO15RF_SSTART_DELAY_AON
#define AON_LDO15RF_SSTART_DELAY_AON_POS         (9U)
#define AON_LDO15RF_SSTART_DELAY_AON_LEN         (2U)
#define AON_LDO15RF_SSTART_DELAY_AON_MSK         (((1U << AON_LDO15RF_SSTART_DELAY_AON_LEN) - 1) << AON_LDO15RF_SSTART_DELAY_AON_POS)
#define AON_LDO15RF_SSTART_DELAY_AON_UMSK        (~(((1U << AON_LDO15RF_SSTART_DELAY_AON_LEN) - 1) << AON_LDO15RF_SSTART_DELAY_AON_POS))
#define AON_LDO15RF_PULLDOWN_AON                 AON_LDO15RF_PULLDOWN_AON
#define AON_LDO15RF_PULLDOWN_AON_POS             (12U)
#define AON_LDO15RF_PULLDOWN_AON_LEN             (1U)
#define AON_LDO15RF_PULLDOWN_AON_MSK             (((1U << AON_LDO15RF_PULLDOWN_AON_LEN) - 1) << AON_LDO15RF_PULLDOWN_AON_POS)
#define AON_LDO15RF_PULLDOWN_AON_UMSK            (~(((1U << AON_LDO15RF_PULLDOWN_AON_LEN) - 1) << AON_LDO15RF_PULLDOWN_AON_POS))
#define AON_LDO15RF_PULLDOWN_SEL_AON             AON_LDO15RF_PULLDOWN_SEL_AON
#define AON_LDO15RF_PULLDOWN_SEL_AON_POS         (13U)
#define AON_LDO15RF_PULLDOWN_SEL_AON_LEN         (1U)
#define AON_LDO15RF_PULLDOWN_SEL_AON_MSK         (((1U << AON_LDO15RF_PULLDOWN_SEL_AON_LEN) - 1) << AON_LDO15RF_PULLDOWN_SEL_AON_POS)
#define AON_LDO15RF_PULLDOWN_SEL_AON_UMSK        (~(((1U << AON_LDO15RF_PULLDOWN_SEL_AON_LEN) - 1) << AON_LDO15RF_PULLDOWN_SEL_AON_POS))
#define AON_LDO15RF_VOUT_SEL_AON                 AON_LDO15RF_VOUT_SEL_AON
#define AON_LDO15RF_VOUT_SEL_AON_POS             (16U)
#define AON_LDO15RF_VOUT_SEL_AON_LEN             (3U)
#define AON_LDO15RF_VOUT_SEL_AON_MSK             (((1U << AON_LDO15RF_VOUT_SEL_AON_LEN) - 1) << AON_LDO15RF_VOUT_SEL_AON_POS)
#define AON_LDO15RF_VOUT_SEL_AON_UMSK            (~(((1U << AON_LDO15RF_VOUT_SEL_AON_LEN) - 1) << AON_LDO15RF_VOUT_SEL_AON_POS))
#define AON_LDO15RF_CC_AON                       AON_LDO15RF_CC_AON
#define AON_LDO15RF_CC_AON_POS                   (24U)
#define AON_LDO15RF_CC_AON_LEN                   (2U)
#define AON_LDO15RF_CC_AON_MSK                   (((1U << AON_LDO15RF_CC_AON_LEN) - 1) << AON_LDO15RF_CC_AON_POS)
#define AON_LDO15RF_CC_AON_UMSK                  (~(((1U << AON_LDO15RF_CC_AON_LEN) - 1) << AON_LDO15RF_CC_AON_POS))
#define AON_LDO15RF_BYPASS_AON                   AON_LDO15RF_BYPASS_AON
#define AON_LDO15RF_BYPASS_AON_POS               (27U)
#define AON_LDO15RF_BYPASS_AON_LEN               (1U)
#define AON_LDO15RF_BYPASS_AON_MSK               (((1U << AON_LDO15RF_BYPASS_AON_LEN) - 1) << AON_LDO15RF_BYPASS_AON_POS)
#define AON_LDO15RF_BYPASS_AON_UMSK              (~(((1U << AON_LDO15RF_BYPASS_AON_LEN) - 1) << AON_LDO15RF_BYPASS_AON_POS))
#define AON_LDO15RF_VOUT_TRIM_AON                AON_LDO15RF_VOUT_TRIM_AON
#define AON_LDO15RF_VOUT_TRIM_AON_POS            (28U)
#define AON_LDO15RF_VOUT_TRIM_AON_LEN            (4U)
#define AON_LDO15RF_VOUT_TRIM_AON_MSK            (((1U << AON_LDO15RF_VOUT_TRIM_AON_LEN) - 1) << AON_LDO15RF_VOUT_TRIM_AON_POS)
#define AON_LDO15RF_VOUT_TRIM_AON_UMSK           (~(((1U << AON_LDO15RF_VOUT_TRIM_AON_LEN) - 1) << AON_LDO15RF_VOUT_TRIM_AON_POS))

/* 0x884 : xtal_cfg */
#define AON_XTAL_CFG_OFFSET                      (0x884)
#define AON_XTAL_BK_AON                          AON_XTAL_BK_AON
#define AON_XTAL_BK_AON_POS                      (0U)
#define AON_XTAL_BK_AON_LEN                      (2U)
#define AON_XTAL_BK_AON_MSK                      (((1U << AON_XTAL_BK_AON_LEN) - 1) << AON_XTAL_BK_AON_POS)
#define AON_XTAL_BK_AON_UMSK                     (~(((1U << AON_XTAL_BK_AON_LEN) - 1) << AON_XTAL_BK_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_AON               AON_XTAL_CAPCODE_EXTRA_AON
#define AON_XTAL_CAPCODE_EXTRA_AON_POS           (2U)
#define AON_XTAL_CAPCODE_EXTRA_AON_LEN           (1U)
#define AON_XTAL_CAPCODE_EXTRA_AON_MSK           (((1U << AON_XTAL_CAPCODE_EXTRA_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_AON_UMSK          (~(((1U << AON_XTAL_CAPCODE_EXTRA_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_AON_POS))
#define AON_XTAL_BUF_EN_AON                      AON_XTAL_BUF_EN_AON
#define AON_XTAL_BUF_EN_AON_POS                  (4U)
#define AON_XTAL_BUF_EN_AON_LEN                  (2U)
#define AON_XTAL_BUF_EN_AON_MSK                  (((1U << AON_XTAL_BUF_EN_AON_LEN) - 1) << AON_XTAL_BUF_EN_AON_POS)
#define AON_XTAL_BUF_EN_AON_UMSK                 (~(((1U << AON_XTAL_BUF_EN_AON_LEN) - 1) << AON_XTAL_BUF_EN_AON_POS))
#define AON_XTAL_ACBUF_EN_AON                    AON_XTAL_ACBUF_EN_AON
#define AON_XTAL_ACBUF_EN_AON_POS                (6U)
#define AON_XTAL_ACBUF_EN_AON_LEN                (1U)
#define AON_XTAL_ACBUF_EN_AON_MSK                (((1U << AON_XTAL_ACBUF_EN_AON_LEN) - 1) << AON_XTAL_ACBUF_EN_AON_POS)
#define AON_XTAL_ACBUF_EN_AON_UMSK               (~(((1U << AON_XTAL_ACBUF_EN_AON_LEN) - 1) << AON_XTAL_ACBUF_EN_AON_POS))
#define AON_XTAL_ACBUF_MODE_AON                  AON_XTAL_ACBUF_MODE_AON
#define AON_XTAL_ACBUF_MODE_AON_POS              (7U)
#define AON_XTAL_ACBUF_MODE_AON_LEN              (1U)
#define AON_XTAL_ACBUF_MODE_AON_MSK              (((1U << AON_XTAL_ACBUF_MODE_AON_LEN) - 1) << AON_XTAL_ACBUF_MODE_AON_POS)
#define AON_XTAL_ACBUF_MODE_AON_UMSK             (~(((1U << AON_XTAL_ACBUF_MODE_AON_LEN) - 1) << AON_XTAL_ACBUF_MODE_AON_POS))
#define AON_XTAL_BUF_HP_AON                      AON_XTAL_BUF_HP_AON
#define AON_XTAL_BUF_HP_AON_POS                  (8U)
#define AON_XTAL_BUF_HP_AON_LEN                  (2U)
#define AON_XTAL_BUF_HP_AON_MSK                  (((1U << AON_XTAL_BUF_HP_AON_LEN) - 1) << AON_XTAL_BUF_HP_AON_POS)
#define AON_XTAL_BUF_HP_AON_UMSK                 (~(((1U << AON_XTAL_BUF_HP_AON_LEN) - 1) << AON_XTAL_BUF_HP_AON_POS))
#define AON_XTAL_FAST_STARTUP_AON                AON_XTAL_FAST_STARTUP_AON
#define AON_XTAL_FAST_STARTUP_AON_POS            (12U)
#define AON_XTAL_FAST_STARTUP_AON_LEN            (1U)
#define AON_XTAL_FAST_STARTUP_AON_MSK            (((1U << AON_XTAL_FAST_STARTUP_AON_LEN) - 1) << AON_XTAL_FAST_STARTUP_AON_POS)
#define AON_XTAL_FAST_STARTUP_AON_UMSK           (~(((1U << AON_XTAL_FAST_STARTUP_AON_LEN) - 1) << AON_XTAL_FAST_STARTUP_AON_POS))
#define AON_XTAL_AMP_CTRL_AON                    AON_XTAL_AMP_CTRL_AON
#define AON_XTAL_AMP_CTRL_AON_POS                (14U)
#define AON_XTAL_AMP_CTRL_AON_LEN                (2U)
#define AON_XTAL_AMP_CTRL_AON_MSK                (((1U << AON_XTAL_AMP_CTRL_AON_LEN) - 1) << AON_XTAL_AMP_CTRL_AON_POS)
#define AON_XTAL_AMP_CTRL_AON_UMSK               (~(((1U << AON_XTAL_AMP_CTRL_AON_LEN) - 1) << AON_XTAL_AMP_CTRL_AON_POS))

/* 0x888 : tsen */
#define AON_TSEN_OFFSET                          (0x888)
#define AON_TSEN_REFCODE_CORNER                  AON_TSEN_REFCODE_CORNER
#define AON_TSEN_REFCODE_CORNER_POS              (0U)
#define AON_TSEN_REFCODE_CORNER_LEN              (12U)
#define AON_TSEN_REFCODE_CORNER_MSK              (((1U << AON_TSEN_REFCODE_CORNER_LEN) - 1) << AON_TSEN_REFCODE_CORNER_POS)
#define AON_TSEN_REFCODE_CORNER_UMSK             (~(((1U << AON_TSEN_REFCODE_CORNER_LEN) - 1) << AON_TSEN_REFCODE_CORNER_POS))
#define AON_TSEN_REFCODE_RFCAL                   AON_TSEN_REFCODE_RFCAL
#define AON_TSEN_REFCODE_RFCAL_POS               (16U)
#define AON_TSEN_REFCODE_RFCAL_LEN               (12U)
#define AON_TSEN_REFCODE_RFCAL_MSK               (((1U << AON_TSEN_REFCODE_RFCAL_LEN) - 1) << AON_TSEN_REFCODE_RFCAL_POS)
#define AON_TSEN_REFCODE_RFCAL_UMSK              (~(((1U << AON_TSEN_REFCODE_RFCAL_LEN) - 1) << AON_TSEN_REFCODE_RFCAL_POS))
#define AON_XTAL_RDY                             AON_XTAL_RDY
#define AON_XTAL_RDY_POS                         (28U)
#define AON_XTAL_RDY_LEN                         (1U)
#define AON_XTAL_RDY_MSK                         (((1U << AON_XTAL_RDY_LEN) - 1) << AON_XTAL_RDY_POS)
#define AON_XTAL_RDY_UMSK                        (~(((1U << AON_XTAL_RDY_LEN) - 1) << AON_XTAL_RDY_POS))

/* 0x890 : aon_wdt */
#define AON_WDT_OFFSET                           (0x890)
#define AON_CR_ANO_WDT_EN                        AON_CR_ANO_WDT_EN
#define AON_CR_ANO_WDT_EN_POS                    (0U)
#define AON_CR_ANO_WDT_EN_LEN                    (1U)
#define AON_CR_ANO_WDT_EN_MSK                    (((1U << AON_CR_ANO_WDT_EN_LEN) - 1) << AON_CR_ANO_WDT_EN_POS)
#define AON_CR_ANO_WDT_EN_UMSK                   (~(((1U << AON_CR_ANO_WDT_EN_LEN) - 1) << AON_CR_ANO_WDT_EN_POS))
#define AON_CR_SW_KICK_POS                       AON_CR_SW_KICK_POS
#define AON_CR_SW_KICK_POS_POS                   (1U)
#define AON_CR_SW_KICK_POS_LEN                   (1U)
#define AON_CR_SW_KICK_POS_MSK                   (((1U << AON_CR_SW_KICK_POS_LEN) - 1) << AON_CR_SW_KICK_POS_POS)
#define AON_CR_SW_KICK_POS_UMSK                  (~(((1U << AON_CR_SW_KICK_POS_LEN) - 1) << AON_CR_SW_KICK_POS_POS))
#define AON_CR_AON_WDT_CLR                       AON_CR_AON_WDT_CLR
#define AON_CR_AON_WDT_CLR_POS                   (2U)
#define AON_CR_AON_WDT_CLR_LEN                   (1U)
#define AON_CR_AON_WDT_CLR_MSK                   (((1U << AON_CR_AON_WDT_CLR_LEN) - 1) << AON_CR_AON_WDT_CLR_POS)
#define AON_CR_AON_WDT_CLR_UMSK                  (~(((1U << AON_CR_AON_WDT_CLR_LEN) - 1) << AON_CR_AON_WDT_CLR_POS))
#define AON_ST_AON_WDT                           AON_ST_AON_WDT
#define AON_ST_AON_WDT_POS                       (7U)
#define AON_ST_AON_WDT_LEN                       (1U)
#define AON_ST_AON_WDT_MSK                       (((1U << AON_ST_AON_WDT_LEN) - 1) << AON_ST_AON_WDT_POS)
#define AON_ST_AON_WDT_UMSK                      (~(((1U << AON_ST_AON_WDT_LEN) - 1) << AON_ST_AON_WDT_POS))
#define AON_CR_CNT_MASK_BIT                      AON_CR_CNT_MASK_BIT
#define AON_CR_CNT_MASK_BIT_POS                  (8U)
#define AON_CR_CNT_MASK_BIT_LEN                  (6U)
#define AON_CR_CNT_MASK_BIT_MSK                  (((1U << AON_CR_CNT_MASK_BIT_LEN) - 1) << AON_CR_CNT_MASK_BIT_POS)
#define AON_CR_CNT_MASK_BIT_UMSK                 (~(((1U << AON_CR_CNT_MASK_BIT_LEN) - 1) << AON_CR_CNT_MASK_BIT_POS))

/* 0x8C4 : xtal_pu */
#define AON_XTAL_PU_OFFSET                       (0x8C4)
#define AON_PU_XTAL_AON                          AON_PU_XTAL_AON
#define AON_PU_XTAL_AON_POS                      (0U)
#define AON_PU_XTAL_AON_LEN                      (1U)
#define AON_PU_XTAL_AON_MSK                      (((1U << AON_PU_XTAL_AON_LEN) - 1) << AON_PU_XTAL_AON_POS)
#define AON_PU_XTAL_AON_UMSK                     (~(((1U << AON_PU_XTAL_AON_LEN) - 1) << AON_PU_XTAL_AON_POS))
#define AON_PU_XTAL_BUF_AON                      AON_PU_XTAL_BUF_AON
#define AON_PU_XTAL_BUF_AON_POS                  (4U)
#define AON_PU_XTAL_BUF_AON_LEN                  (1U)
#define AON_PU_XTAL_BUF_AON_MSK                  (((1U << AON_PU_XTAL_BUF_AON_LEN) - 1) << AON_PU_XTAL_BUF_AON_POS)
#define AON_PU_XTAL_BUF_AON_UMSK                 (~(((1U << AON_PU_XTAL_BUF_AON_LEN) - 1) << AON_PU_XTAL_BUF_AON_POS))
#define AON_PU_XTAL_CORE_AON                     AON_PU_XTAL_CORE_AON
#define AON_PU_XTAL_CORE_AON_POS                 (8U)
#define AON_PU_XTAL_CORE_AON_LEN                 (1U)
#define AON_PU_XTAL_CORE_AON_MSK                 (((1U << AON_PU_XTAL_CORE_AON_LEN) - 1) << AON_PU_XTAL_CORE_AON_POS)
#define AON_PU_XTAL_CORE_AON_UMSK                (~(((1U << AON_PU_XTAL_CORE_AON_LEN) - 1) << AON_PU_XTAL_CORE_AON_POS))
#define AON_PUD_XTAL_AON                         AON_PUD_XTAL_AON
#define AON_PUD_XTAL_AON_POS                     (12U)
#define AON_PUD_XTAL_AON_LEN                     (1U)
#define AON_PUD_XTAL_AON_MSK                     (((1U << AON_PUD_XTAL_AON_LEN) - 1) << AON_PUD_XTAL_AON_POS)
#define AON_PUD_XTAL_AON_UMSK                    (~(((1U << AON_PUD_XTAL_AON_LEN) - 1) << AON_PUD_XTAL_AON_POS))
#define AON_PUD_DLY_XTAL_AON                     AON_PUD_DLY_XTAL_AON
#define AON_PUD_DLY_XTAL_AON_POS                 (16U)
#define AON_PUD_DLY_XTAL_AON_LEN                 (3U)
#define AON_PUD_DLY_XTAL_AON_MSK                 (((1U << AON_PUD_DLY_XTAL_AON_LEN) - 1) << AON_PUD_DLY_XTAL_AON_POS)
#define AON_PUD_DLY_XTAL_AON_UMSK                (~(((1U << AON_PUD_DLY_XTAL_AON_LEN) - 1) << AON_PUD_DLY_XTAL_AON_POS))
#define AON_XTAL_RESERVE_AON                     AON_XTAL_RESERVE_AON
#define AON_XTAL_RESERVE_AON_POS                 (20U)
#define AON_XTAL_RESERVE_AON_LEN                 (4U)
#define AON_XTAL_RESERVE_AON_MSK                 (((1U << AON_XTAL_RESERVE_AON_LEN) - 1) << AON_XTAL_RESERVE_AON_POS)
#define AON_XTAL_RESERVE_AON_UMSK                (~(((1U << AON_XTAL_RESERVE_AON_LEN) - 1) << AON_XTAL_RESERVE_AON_POS))

/* 0x8C8 : xtal_ldo */
#define AON_XTAL_LDO_OFFSET                      (0x8C8)
#define AON_XTAL_BUF_POWER_BOOST_AON             AON_XTAL_BUF_POWER_BOOST_AON
#define AON_XTAL_BUF_POWER_BOOST_AON_POS         (0U)
#define AON_XTAL_BUF_POWER_BOOST_AON_LEN         (1U)
#define AON_XTAL_BUF_POWER_BOOST_AON_MSK         (((1U << AON_XTAL_BUF_POWER_BOOST_AON_LEN) - 1) << AON_XTAL_BUF_POWER_BOOST_AON_POS)
#define AON_XTAL_BUF_POWER_BOOST_AON_UMSK        (~(((1U << AON_XTAL_BUF_POWER_BOOST_AON_LEN) - 1) << AON_XTAL_BUF_POWER_BOOST_AON_POS))
#define AON_XTAL_CORE_POWER_BOOST_AON            AON_XTAL_CORE_POWER_BOOST_AON
#define AON_XTAL_CORE_POWER_BOOST_AON_POS        (4U)
#define AON_XTAL_CORE_POWER_BOOST_AON_LEN        (1U)
#define AON_XTAL_CORE_POWER_BOOST_AON_MSK        (((1U << AON_XTAL_CORE_POWER_BOOST_AON_LEN) - 1) << AON_XTAL_CORE_POWER_BOOST_AON_POS)
#define AON_XTAL_CORE_POWER_BOOST_AON_UMSK       (~(((1U << AON_XTAL_CORE_POWER_BOOST_AON_LEN) - 1) << AON_XTAL_CORE_POWER_BOOST_AON_POS))
#define AON_XTAL_LDO_BYPASS_AON                  AON_XTAL_LDO_BYPASS_AON
#define AON_XTAL_LDO_BYPASS_AON_POS              (8U)
#define AON_XTAL_LDO_BYPASS_AON_LEN              (1U)
#define AON_XTAL_LDO_BYPASS_AON_MSK              (((1U << AON_XTAL_LDO_BYPASS_AON_LEN) - 1) << AON_XTAL_LDO_BYPASS_AON_POS)
#define AON_XTAL_LDO_BYPASS_AON_UMSK             (~(((1U << AON_XTAL_LDO_BYPASS_AON_LEN) - 1) << AON_XTAL_LDO_BYPASS_AON_POS))
#define AON_XTAL_SLEEP_AON                       AON_XTAL_SLEEP_AON
#define AON_XTAL_SLEEP_AON_POS                   (12U)
#define AON_XTAL_SLEEP_AON_LEN                   (1U)
#define AON_XTAL_SLEEP_AON_MSK                   (((1U << AON_XTAL_SLEEP_AON_LEN) - 1) << AON_XTAL_SLEEP_AON_POS)
#define AON_XTAL_SLEEP_AON_UMSK                  (~(((1U << AON_XTAL_SLEEP_AON_LEN) - 1) << AON_XTAL_SLEEP_AON_POS))
#define AON_XTAL_VGATE_FILTER_AON                AON_XTAL_VGATE_FILTER_AON
#define AON_XTAL_VGATE_FILTER_AON_POS            (16U)
#define AON_XTAL_VGATE_FILTER_AON_LEN            (2U)
#define AON_XTAL_VGATE_FILTER_AON_MSK            (((1U << AON_XTAL_VGATE_FILTER_AON_LEN) - 1) << AON_XTAL_VGATE_FILTER_AON_POS)
#define AON_XTAL_VGATE_FILTER_AON_UMSK           (~(((1U << AON_XTAL_VGATE_FILTER_AON_LEN) - 1) << AON_XTAL_VGATE_FILTER_AON_POS))

/* 0x8CC : xtal_core0 */
#define AON_XTAL_CORE0_OFFSET                    (0x8CC)
#define AON_XTAL_INJECTION_EN_AON                AON_XTAL_INJECTION_EN_AON
#define AON_XTAL_INJECTION_EN_AON_POS            (0U)
#define AON_XTAL_INJECTION_EN_AON_LEN            (1U)
#define AON_XTAL_INJECTION_EN_AON_MSK            (((1U << AON_XTAL_INJECTION_EN_AON_LEN) - 1) << AON_XTAL_INJECTION_EN_AON_POS)
#define AON_XTAL_INJECTION_EN_AON_UMSK           (~(((1U << AON_XTAL_INJECTION_EN_AON_LEN) - 1) << AON_XTAL_INJECTION_EN_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_IN_AON            AON_XTAL_CAPCODE_EXTRA_IN_AON
#define AON_XTAL_CAPCODE_EXTRA_IN_AON_POS        (4U)
#define AON_XTAL_CAPCODE_EXTRA_IN_AON_LEN        (1U)
#define AON_XTAL_CAPCODE_EXTRA_IN_AON_MSK        (((1U << AON_XTAL_CAPCODE_EXTRA_IN_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_IN_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_IN_AON_UMSK       (~(((1U << AON_XTAL_CAPCODE_EXTRA_IN_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_IN_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_OUT_AON           AON_XTAL_CAPCODE_EXTRA_OUT_AON
#define AON_XTAL_CAPCODE_EXTRA_OUT_AON_POS       (8U)
#define AON_XTAL_CAPCODE_EXTRA_OUT_AON_LEN       (1U)
#define AON_XTAL_CAPCODE_EXTRA_OUT_AON_MSK       (((1U << AON_XTAL_CAPCODE_EXTRA_OUT_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_OUT_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_OUT_AON_UMSK      (~(((1U << AON_XTAL_CAPCODE_EXTRA_OUT_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_OUT_AON_POS))
#define AON_XTAL_GM_PD_CTRL_AON                  AON_XTAL_GM_PD_CTRL_AON
#define AON_XTAL_GM_PD_CTRL_AON_POS              (12U)
#define AON_XTAL_GM_PD_CTRL_AON_LEN              (1U)
#define AON_XTAL_GM_PD_CTRL_AON_MSK              (((1U << AON_XTAL_GM_PD_CTRL_AON_LEN) - 1) << AON_XTAL_GM_PD_CTRL_AON_POS)
#define AON_XTAL_GM_PD_CTRL_AON_UMSK             (~(((1U << AON_XTAL_GM_PD_CTRL_AON_LEN) - 1) << AON_XTAL_GM_PD_CTRL_AON_POS))
#define AON_XTAL_CAPCODE_IN_AON                  AON_XTAL_CAPCODE_IN_AON
#define AON_XTAL_CAPCODE_IN_AON_POS              (16U)
#define AON_XTAL_CAPCODE_IN_AON_LEN              (8U)
#define AON_XTAL_CAPCODE_IN_AON_MSK              (((1U << AON_XTAL_CAPCODE_IN_AON_LEN) - 1) << AON_XTAL_CAPCODE_IN_AON_POS)
#define AON_XTAL_CAPCODE_IN_AON_UMSK             (~(((1U << AON_XTAL_CAPCODE_IN_AON_LEN) - 1) << AON_XTAL_CAPCODE_IN_AON_POS))
#define AON_XTAL_CAPCODE_OUT_AON                 AON_XTAL_CAPCODE_OUT_AON
#define AON_XTAL_CAPCODE_OUT_AON_POS             (24U)
#define AON_XTAL_CAPCODE_OUT_AON_LEN             (8U)
#define AON_XTAL_CAPCODE_OUT_AON_MSK             (((1U << AON_XTAL_CAPCODE_OUT_AON_LEN) - 1) << AON_XTAL_CAPCODE_OUT_AON_POS)
#define AON_XTAL_CAPCODE_OUT_AON_UMSK            (~(((1U << AON_XTAL_CAPCODE_OUT_AON_LEN) - 1) << AON_XTAL_CAPCODE_OUT_AON_POS))

/* 0x8D0 : xtal_core1 */
#define AON_XTAL_CORE1_OFFSET                    (0x8D0)
#define AON_XTAL_CAPCODE_UNBALANCE_AON           AON_XTAL_CAPCODE_UNBALANCE_AON
#define AON_XTAL_CAPCODE_UNBALANCE_AON_POS       (0U)
#define AON_XTAL_CAPCODE_UNBALANCE_AON_LEN       (1U)
#define AON_XTAL_CAPCODE_UNBALANCE_AON_MSK       (((1U << AON_XTAL_CAPCODE_UNBALANCE_AON_LEN) - 1) << AON_XTAL_CAPCODE_UNBALANCE_AON_POS)
#define AON_XTAL_CAPCODE_UNBALANCE_AON_UMSK      (~(((1U << AON_XTAL_CAPCODE_UNBALANCE_AON_LEN) - 1) << AON_XTAL_CAPCODE_UNBALANCE_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON       AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON
#define AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_POS   (4U)
#define AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_LEN   (1U)
#define AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_MSK   (((1U << AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_UMSK  (~(((1U << AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_IN_FAST_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON      AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_POS  (8U)
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_LEN  (1U)
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_MSK  (((1U << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_UMSK (~(((1U << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_POS))
#define AON_XTAL_CAPCODE_IN_FAST_AON             AON_XTAL_CAPCODE_IN_FAST_AON
#define AON_XTAL_CAPCODE_IN_FAST_AON_POS         (16U)
#define AON_XTAL_CAPCODE_IN_FAST_AON_LEN         (8U)
#define AON_XTAL_CAPCODE_IN_FAST_AON_MSK         (((1U << AON_XTAL_CAPCODE_IN_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_IN_FAST_AON_POS)
#define AON_XTAL_CAPCODE_IN_FAST_AON_UMSK        (~(((1U << AON_XTAL_CAPCODE_IN_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_IN_FAST_AON_POS))
#define AON_XTAL_CAPCODE_OUT_FAST_AON            AON_XTAL_CAPCODE_OUT_FAST_AON
#define AON_XTAL_CAPCODE_OUT_FAST_AON_POS        (24U)
#define AON_XTAL_CAPCODE_OUT_FAST_AON_LEN        (8U)
#define AON_XTAL_CAPCODE_OUT_FAST_AON_MSK        (((1U << AON_XTAL_CAPCODE_OUT_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_OUT_FAST_AON_POS)
#define AON_XTAL_CAPCODE_OUT_FAST_AON_UMSK       (~(((1U << AON_XTAL_CAPCODE_OUT_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_OUT_FAST_AON_POS))

/* 0x8D4 : xtal_core2 */
#define AON_XTAL_CORE2_OFFSET                    (0x8D4)
#define AON_XTAL_CAPCODE_UNBALANCE_FAST_AON      AON_XTAL_CAPCODE_UNBALANCE_FAST_AON
#define AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_POS  (0U)
#define AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_LEN  (1U)
#define AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_MSK  (((1U << AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_POS)
#define AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_UMSK (~(((1U << AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_UNBALANCE_FAST_AON_POS))
#define AON_XTAL_EXT_SEL_AON                     AON_XTAL_EXT_SEL_AON
#define AON_XTAL_EXT_SEL_AON_POS                 (4U)
#define AON_XTAL_EXT_SEL_AON_LEN                 (1U)
#define AON_XTAL_EXT_SEL_AON_MSK                 (((1U << AON_XTAL_EXT_SEL_AON_LEN) - 1) << AON_XTAL_EXT_SEL_AON_POS)
#define AON_XTAL_EXT_SEL_AON_UMSK                (~(((1U << AON_XTAL_EXT_SEL_AON_LEN) - 1) << AON_XTAL_EXT_SEL_AON_POS))
#define AON_XTAL_GM_BOOST_FAST_AON               AON_XTAL_GM_BOOST_FAST_AON
#define AON_XTAL_GM_BOOST_FAST_AON_POS           (8U)
#define AON_XTAL_GM_BOOST_FAST_AON_LEN           (2U)
#define AON_XTAL_GM_BOOST_FAST_AON_MSK           (((1U << AON_XTAL_GM_BOOST_FAST_AON_LEN) - 1) << AON_XTAL_GM_BOOST_FAST_AON_POS)
#define AON_XTAL_GM_BOOST_FAST_AON_UMSK          (~(((1U << AON_XTAL_GM_BOOST_FAST_AON_LEN) - 1) << AON_XTAL_GM_BOOST_FAST_AON_POS))
#define AON_XTAL_GM_BOOST_AON                    AON_XTAL_GM_BOOST_AON
#define AON_XTAL_GM_BOOST_AON_POS                (12U)
#define AON_XTAL_GM_BOOST_AON_LEN                (2U)
#define AON_XTAL_GM_BOOST_AON_MSK                (((1U << AON_XTAL_GM_BOOST_AON_LEN) - 1) << AON_XTAL_GM_BOOST_AON_POS)
#define AON_XTAL_GM_BOOST_AON_UMSK               (~(((1U << AON_XTAL_GM_BOOST_AON_LEN) - 1) << AON_XTAL_GM_BOOST_AON_POS))
#define AON_XTAL_SOCBUF_EN_AON                   AON_XTAL_SOCBUF_EN_AON
#define AON_XTAL_SOCBUF_EN_AON_POS               (16U)
#define AON_XTAL_SOCBUF_EN_AON_LEN               (1U)
#define AON_XTAL_SOCBUF_EN_AON_MSK               (((1U << AON_XTAL_SOCBUF_EN_AON_LEN) - 1) << AON_XTAL_SOCBUF_EN_AON_POS)
#define AON_XTAL_SOCBUF_EN_AON_UMSK              (~(((1U << AON_XTAL_SOCBUF_EN_AON_LEN) - 1) << AON_XTAL_SOCBUF_EN_AON_POS))
#define AON_XTAL_SOCBUF_DRV_SEL_AON              AON_XTAL_SOCBUF_DRV_SEL_AON
#define AON_XTAL_SOCBUF_DRV_SEL_AON_POS          (20U)
#define AON_XTAL_SOCBUF_DRV_SEL_AON_LEN          (1U)
#define AON_XTAL_SOCBUF_DRV_SEL_AON_MSK          (((1U << AON_XTAL_SOCBUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_SOCBUF_DRV_SEL_AON_POS)
#define AON_XTAL_SOCBUF_DRV_SEL_AON_UMSK         (~(((1U << AON_XTAL_SOCBUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_SOCBUF_DRV_SEL_AON_POS))
#define AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON        AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON
#define AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_POS    (21U)
#define AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_LEN    (1U)
#define AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_MSK    (((1U << AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_POS)
#define AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_UMSK   (~(((1U << AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_LATCH_EN_AON_POS))
#define AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON         AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON
#define AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_POS     (22U)
#define AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_LEN     (1U)
#define AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_MSK     (((1U << AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_POS)
#define AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_UMSK    (~(((1U << AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_DRV_SEL_AON_POS))
#define AON_XTAL_WIFIPLL_BUF_DUM_EN_AON          AON_XTAL_WIFIPLL_BUF_DUM_EN_AON
#define AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_POS      (23U)
#define AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_LEN      (1U)
#define AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_MSK      (((1U << AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_POS)
#define AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_UMSK     (~(((1U << AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_DUM_EN_AON_POS))
#define AON_XTAL_WIFIPLL_BUF_EN_AON              AON_XTAL_WIFIPLL_BUF_EN_AON
#define AON_XTAL_WIFIPLL_BUF_EN_AON_POS          (24U)
#define AON_XTAL_WIFIPLL_BUF_EN_AON_LEN          (1U)
#define AON_XTAL_WIFIPLL_BUF_EN_AON_MSK          (((1U << AON_XTAL_WIFIPLL_BUF_EN_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_EN_AON_POS)
#define AON_XTAL_WIFIPLL_BUF_EN_AON_UMSK         (~(((1U << AON_XTAL_WIFIPLL_BUF_EN_AON_LEN) - 1) << AON_XTAL_WIFIPLL_BUF_EN_AON_POS))
#define AON_XTAL_INN_CFG_EN_AON                  AON_XTAL_INN_CFG_EN_AON
#define AON_XTAL_INN_CFG_EN_AON_POS              (28U)
#define AON_XTAL_INN_CFG_EN_AON_LEN              (1U)
#define AON_XTAL_INN_CFG_EN_AON_MSK              (((1U << AON_XTAL_INN_CFG_EN_AON_LEN) - 1) << AON_XTAL_INN_CFG_EN_AON_POS)
#define AON_XTAL_INN_CFG_EN_AON_UMSK             (~(((1U << AON_XTAL_INN_CFG_EN_AON_LEN) - 1) << AON_XTAL_INN_CFG_EN_AON_POS))

/* 0x8D8 : xtal_buffer */
#define AON_XTAL_BUFFER_OFFSET                   (0x8D8)
#define AON_XTAL_TAUR_LO_BUF_EN_AON              AON_XTAL_TAUR_LO_BUF_EN_AON
#define AON_XTAL_TAUR_LO_BUF_EN_AON_POS          (4U)
#define AON_XTAL_TAUR_LO_BUF_EN_AON_LEN          (1U)
#define AON_XTAL_TAUR_LO_BUF_EN_AON_MSK          (((1U << AON_XTAL_TAUR_LO_BUF_EN_AON_LEN) - 1) << AON_XTAL_TAUR_LO_BUF_EN_AON_POS)
#define AON_XTAL_TAUR_LO_BUF_EN_AON_UMSK         (~(((1U << AON_XTAL_TAUR_LO_BUF_EN_AON_LEN) - 1) << AON_XTAL_TAUR_LO_BUF_EN_AON_POS))
#define AON_XTAL_USB2_BUF_EN_AON                 AON_XTAL_USB2_BUF_EN_AON
#define AON_XTAL_USB2_BUF_EN_AON_POS             (8U)
#define AON_XTAL_USB2_BUF_EN_AON_LEN             (1U)
#define AON_XTAL_USB2_BUF_EN_AON_MSK             (((1U << AON_XTAL_USB2_BUF_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_EN_AON_POS)
#define AON_XTAL_USB2_BUF_EN_AON_UMSK            (~(((1U << AON_XTAL_USB2_BUF_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_EN_AON_POS))
#define AON_XTAL_EXTBUF_EN_AON                   AON_XTAL_EXTBUF_EN_AON
#define AON_XTAL_EXTBUF_EN_AON_POS               (12U)
#define AON_XTAL_EXTBUF_EN_AON_LEN               (1U)
#define AON_XTAL_EXTBUF_EN_AON_MSK               (((1U << AON_XTAL_EXTBUF_EN_AON_LEN) - 1) << AON_XTAL_EXTBUF_EN_AON_POS)
#define AON_XTAL_EXTBUF_EN_AON_UMSK              (~(((1U << AON_XTAL_EXTBUF_EN_AON_LEN) - 1) << AON_XTAL_EXTBUF_EN_AON_POS))
#define AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON        AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON
#define AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_POS    (15U)
#define AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_LEN    (1U)
#define AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_MSK    (((1U << AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_POS)
#define AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_UMSK   (~(((1U << AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_TAUR_LO_BUF_LATCH_EN_AON_POS))
#define AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON         AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON
#define AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_POS     (16U)
#define AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_LEN     (1U)
#define AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_MSK     (((1U << AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_POS)
#define AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_UMSK    (~(((1U << AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_DRV_SEL_AON_POS))
#define AON_XTAL_ARIE_LO_BUF_DUM_EN_AON          AON_XTAL_ARIE_LO_BUF_DUM_EN_AON
#define AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_POS      (20U)
#define AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_LEN      (1U)
#define AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_MSK      (((1U << AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_POS)
#define AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_UMSK     (~(((1U << AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_DUM_EN_AON_POS))
#define AON_XTAL_ARIE_LO_BUF_EN_AON              AON_XTAL_ARIE_LO_BUF_EN_AON
#define AON_XTAL_ARIE_LO_BUF_EN_AON_POS          (24U)
#define AON_XTAL_ARIE_LO_BUF_EN_AON_LEN          (1U)
#define AON_XTAL_ARIE_LO_BUF_EN_AON_MSK          (((1U << AON_XTAL_ARIE_LO_BUF_EN_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_EN_AON_POS)
#define AON_XTAL_ARIE_LO_BUF_EN_AON_UMSK         (~(((1U << AON_XTAL_ARIE_LO_BUF_EN_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_EN_AON_POS))
#define AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON        AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON
#define AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_POS    (28U)
#define AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_LEN    (1U)
#define AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_MSK    (((1U << AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_POS)
#define AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_UMSK   (~(((1U << AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_ARIE_LO_BUF_LATCH_EN_AON_POS))

/* 0x8DC : xtal_dig */
#define AON_XTAL_DIG_OFFSET                      (0x8DC)
#define AON_XTAL_RDY_INT_SEL_AON                 AON_XTAL_RDY_INT_SEL_AON
#define AON_XTAL_RDY_INT_SEL_AON_POS             (0U)
#define AON_XTAL_RDY_INT_SEL_AON_LEN             (2U)
#define AON_XTAL_RDY_INT_SEL_AON_MSK             (((1U << AON_XTAL_RDY_INT_SEL_AON_LEN) - 1) << AON_XTAL_RDY_INT_SEL_AON_POS)
#define AON_XTAL_RDY_INT_SEL_AON_UMSK            (~(((1U << AON_XTAL_RDY_INT_SEL_AON_LEN) - 1) << AON_XTAL_RDY_INT_SEL_AON_POS))
#define AON_XTAL_RDY_SEL_AON                     AON_XTAL_RDY_SEL_AON
#define AON_XTAL_RDY_SEL_AON_POS                 (4U)
#define AON_XTAL_RDY_SEL_AON_LEN                 (2U)
#define AON_XTAL_RDY_SEL_AON_MSK                 (((1U << AON_XTAL_RDY_SEL_AON_LEN) - 1) << AON_XTAL_RDY_SEL_AON_POS)
#define AON_XTAL_RDY_SEL_AON_UMSK                (~(((1U << AON_XTAL_RDY_SEL_AON_LEN) - 1) << AON_XTAL_RDY_SEL_AON_POS))

/* 0x8E0 : xtal_test */
#define AON_XTAL_TEST_OFFSET                     (0x8E0)
#define AON_TEN_XTAL_AON                         AON_TEN_XTAL_AON
#define AON_TEN_XTAL_AON_POS                     (0U)
#define AON_TEN_XTAL_AON_LEN                     (1U)
#define AON_TEN_XTAL_AON_MSK                     (((1U << AON_TEN_XTAL_AON_LEN) - 1) << AON_TEN_XTAL_AON_POS)
#define AON_TEN_XTAL_AON_UMSK                    (~(((1U << AON_TEN_XTAL_AON_LEN) - 1) << AON_TEN_XTAL_AON_POS))
#define AON_DTEN_XTAL_AON                        AON_DTEN_XTAL_AON
#define AON_DTEN_XTAL_AON_POS                    (8U)
#define AON_DTEN_XTAL_AON_LEN                    (1U)
#define AON_DTEN_XTAL_AON_MSK                    (((1U << AON_DTEN_XTAL_AON_LEN) - 1) << AON_DTEN_XTAL_AON_POS)
#define AON_DTEN_XTAL_AON_UMSK                   (~(((1U << AON_DTEN_XTAL_AON_LEN) - 1) << AON_DTEN_XTAL_AON_POS))
#define AON_DTEN_XTAL_PU_AON                     AON_DTEN_XTAL_PU_AON
#define AON_DTEN_XTAL_PU_AON_POS                 (12U)
#define AON_DTEN_XTAL_PU_AON_LEN                 (1U)
#define AON_DTEN_XTAL_PU_AON_MSK                 (((1U << AON_DTEN_XTAL_PU_AON_LEN) - 1) << AON_DTEN_XTAL_PU_AON_POS)
#define AON_DTEN_XTAL_PU_AON_UMSK                (~(((1U << AON_DTEN_XTAL_PU_AON_LEN) - 1) << AON_DTEN_XTAL_PU_AON_POS))
#define AON_DTEN_XTAL_RDY_AON                    AON_DTEN_XTAL_RDY_AON
#define AON_DTEN_XTAL_RDY_AON_POS                (16U)
#define AON_DTEN_XTAL_RDY_AON_LEN                (1U)
#define AON_DTEN_XTAL_RDY_AON_MSK                (((1U << AON_DTEN_XTAL_RDY_AON_LEN) - 1) << AON_DTEN_XTAL_RDY_AON_POS)
#define AON_DTEN_XTAL_RDY_AON_UMSK               (~(((1U << AON_DTEN_XTAL_RDY_AON_LEN) - 1) << AON_DTEN_XTAL_RDY_AON_POS))

/* 0x900 : acomp0_ctrl */
#define AON_ACOMP0_CTRL_OFFSET                   (0x900)
#define AON_ACOMP0_EN                            AON_ACOMP0_EN
#define AON_ACOMP0_EN_POS                        (0U)
#define AON_ACOMP0_EN_LEN                        (1U)
#define AON_ACOMP0_EN_MSK                        (((1U << AON_ACOMP0_EN_LEN) - 1) << AON_ACOMP0_EN_POS)
#define AON_ACOMP0_EN_UMSK                       (~(((1U << AON_ACOMP0_EN_LEN) - 1) << AON_ACOMP0_EN_POS))
#define AON_ACOMP0_HYST_SELN                     AON_ACOMP0_HYST_SELN
#define AON_ACOMP0_HYST_SELN_POS                 (4U)
#define AON_ACOMP0_HYST_SELN_LEN                 (3U)
#define AON_ACOMP0_HYST_SELN_MSK                 (((1U << AON_ACOMP0_HYST_SELN_LEN) - 1) << AON_ACOMP0_HYST_SELN_POS)
#define AON_ACOMP0_HYST_SELN_UMSK                (~(((1U << AON_ACOMP0_HYST_SELN_LEN) - 1) << AON_ACOMP0_HYST_SELN_POS))
#define AON_ACOMP0_HYST_SELP                     AON_ACOMP0_HYST_SELP
#define AON_ACOMP0_HYST_SELP_POS                 (7U)
#define AON_ACOMP0_HYST_SELP_LEN                 (3U)
#define AON_ACOMP0_HYST_SELP_MSK                 (((1U << AON_ACOMP0_HYST_SELP_LEN) - 1) << AON_ACOMP0_HYST_SELP_POS)
#define AON_ACOMP0_HYST_SELP_UMSK                (~(((1U << AON_ACOMP0_HYST_SELP_LEN) - 1) << AON_ACOMP0_HYST_SELP_POS))
#define AON_ACOMP0_BIAS_PROG                     AON_ACOMP0_BIAS_PROG
#define AON_ACOMP0_BIAS_PROG_POS                 (10U)
#define AON_ACOMP0_BIAS_PROG_LEN                 (2U)
#define AON_ACOMP0_BIAS_PROG_MSK                 (((1U << AON_ACOMP0_BIAS_PROG_LEN) - 1) << AON_ACOMP0_BIAS_PROG_POS)
#define AON_ACOMP0_BIAS_PROG_UMSK                (~(((1U << AON_ACOMP0_BIAS_PROG_LEN) - 1) << AON_ACOMP0_BIAS_PROG_POS))
#define AON_ACOMP0_LEVEL_SEL                     AON_ACOMP0_LEVEL_SEL
#define AON_ACOMP0_LEVEL_SEL_POS                 (12U)
#define AON_ACOMP0_LEVEL_SEL_LEN                 (6U)
#define AON_ACOMP0_LEVEL_SEL_MSK                 (((1U << AON_ACOMP0_LEVEL_SEL_LEN) - 1) << AON_ACOMP0_LEVEL_SEL_POS)
#define AON_ACOMP0_LEVEL_SEL_UMSK                (~(((1U << AON_ACOMP0_LEVEL_SEL_LEN) - 1) << AON_ACOMP0_LEVEL_SEL_POS))
#define AON_ACOMP0_NEG_SEL                       AON_ACOMP0_NEG_SEL
#define AON_ACOMP0_NEG_SEL_POS                   (18U)
#define AON_ACOMP0_NEG_SEL_LEN                   (4U)
#define AON_ACOMP0_NEG_SEL_MSK                   (((1U << AON_ACOMP0_NEG_SEL_LEN) - 1) << AON_ACOMP0_NEG_SEL_POS)
#define AON_ACOMP0_NEG_SEL_UMSK                  (~(((1U << AON_ACOMP0_NEG_SEL_LEN) - 1) << AON_ACOMP0_NEG_SEL_POS))
#define AON_ACOMP0_POS_SEL                       AON_ACOMP0_POS_SEL
#define AON_ACOMP0_POS_SEL_POS                   (22U)
#define AON_ACOMP0_POS_SEL_LEN                   (4U)
#define AON_ACOMP0_POS_SEL_MSK                   (((1U << AON_ACOMP0_POS_SEL_LEN) - 1) << AON_ACOMP0_POS_SEL_POS)
#define AON_ACOMP0_POS_SEL_UMSK                  (~(((1U << AON_ACOMP0_POS_SEL_LEN) - 1) << AON_ACOMP0_POS_SEL_POS))
#define AON_ACOMP0_MUXEN                         AON_ACOMP0_MUXEN
#define AON_ACOMP0_MUXEN_POS                     (26U)
#define AON_ACOMP0_MUXEN_LEN                     (1U)
#define AON_ACOMP0_MUXEN_MSK                     (((1U << AON_ACOMP0_MUXEN_LEN) - 1) << AON_ACOMP0_MUXEN_POS)
#define AON_ACOMP0_MUXEN_UMSK                    (~(((1U << AON_ACOMP0_MUXEN_LEN) - 1) << AON_ACOMP0_MUXEN_POS))

/* 0x904 : acomp1_ctrl */
#define AON_ACOMP1_CTRL_OFFSET                   (0x904)
#define AON_ACOMP1_EN                            AON_ACOMP1_EN
#define AON_ACOMP1_EN_POS                        (0U)
#define AON_ACOMP1_EN_LEN                        (1U)
#define AON_ACOMP1_EN_MSK                        (((1U << AON_ACOMP1_EN_LEN) - 1) << AON_ACOMP1_EN_POS)
#define AON_ACOMP1_EN_UMSK                       (~(((1U << AON_ACOMP1_EN_LEN) - 1) << AON_ACOMP1_EN_POS))
#define AON_ACOMP1_HYST_SELN                     AON_ACOMP1_HYST_SELN
#define AON_ACOMP1_HYST_SELN_POS                 (4U)
#define AON_ACOMP1_HYST_SELN_LEN                 (3U)
#define AON_ACOMP1_HYST_SELN_MSK                 (((1U << AON_ACOMP1_HYST_SELN_LEN) - 1) << AON_ACOMP1_HYST_SELN_POS)
#define AON_ACOMP1_HYST_SELN_UMSK                (~(((1U << AON_ACOMP1_HYST_SELN_LEN) - 1) << AON_ACOMP1_HYST_SELN_POS))
#define AON_ACOMP1_HYST_SELP                     AON_ACOMP1_HYST_SELP
#define AON_ACOMP1_HYST_SELP_POS                 (7U)
#define AON_ACOMP1_HYST_SELP_LEN                 (3U)
#define AON_ACOMP1_HYST_SELP_MSK                 (((1U << AON_ACOMP1_HYST_SELP_LEN) - 1) << AON_ACOMP1_HYST_SELP_POS)
#define AON_ACOMP1_HYST_SELP_UMSK                (~(((1U << AON_ACOMP1_HYST_SELP_LEN) - 1) << AON_ACOMP1_HYST_SELP_POS))
#define AON_ACOMP1_BIAS_PROG                     AON_ACOMP1_BIAS_PROG
#define AON_ACOMP1_BIAS_PROG_POS                 (10U)
#define AON_ACOMP1_BIAS_PROG_LEN                 (2U)
#define AON_ACOMP1_BIAS_PROG_MSK                 (((1U << AON_ACOMP1_BIAS_PROG_LEN) - 1) << AON_ACOMP1_BIAS_PROG_POS)
#define AON_ACOMP1_BIAS_PROG_UMSK                (~(((1U << AON_ACOMP1_BIAS_PROG_LEN) - 1) << AON_ACOMP1_BIAS_PROG_POS))
#define AON_ACOMP1_LEVEL_SEL                     AON_ACOMP1_LEVEL_SEL
#define AON_ACOMP1_LEVEL_SEL_POS                 (12U)
#define AON_ACOMP1_LEVEL_SEL_LEN                 (6U)
#define AON_ACOMP1_LEVEL_SEL_MSK                 (((1U << AON_ACOMP1_LEVEL_SEL_LEN) - 1) << AON_ACOMP1_LEVEL_SEL_POS)
#define AON_ACOMP1_LEVEL_SEL_UMSK                (~(((1U << AON_ACOMP1_LEVEL_SEL_LEN) - 1) << AON_ACOMP1_LEVEL_SEL_POS))
#define AON_ACOMP1_NEG_SEL                       AON_ACOMP1_NEG_SEL
#define AON_ACOMP1_NEG_SEL_POS                   (18U)
#define AON_ACOMP1_NEG_SEL_LEN                   (4U)
#define AON_ACOMP1_NEG_SEL_MSK                   (((1U << AON_ACOMP1_NEG_SEL_LEN) - 1) << AON_ACOMP1_NEG_SEL_POS)
#define AON_ACOMP1_NEG_SEL_UMSK                  (~(((1U << AON_ACOMP1_NEG_SEL_LEN) - 1) << AON_ACOMP1_NEG_SEL_POS))
#define AON_ACOMP1_POS_SEL                       AON_ACOMP1_POS_SEL
#define AON_ACOMP1_POS_SEL_POS                   (22U)
#define AON_ACOMP1_POS_SEL_LEN                   (4U)
#define AON_ACOMP1_POS_SEL_MSK                   (((1U << AON_ACOMP1_POS_SEL_LEN) - 1) << AON_ACOMP1_POS_SEL_POS)
#define AON_ACOMP1_POS_SEL_UMSK                  (~(((1U << AON_ACOMP1_POS_SEL_LEN) - 1) << AON_ACOMP1_POS_SEL_POS))
#define AON_ACOMP1_MUXEN                         AON_ACOMP1_MUXEN
#define AON_ACOMP1_MUXEN_POS                     (26U)
#define AON_ACOMP1_MUXEN_LEN                     (1U)
#define AON_ACOMP1_MUXEN_MSK                     (((1U << AON_ACOMP1_MUXEN_LEN) - 1) << AON_ACOMP1_MUXEN_POS)
#define AON_ACOMP1_MUXEN_UMSK                    (~(((1U << AON_ACOMP1_MUXEN_LEN) - 1) << AON_ACOMP1_MUXEN_POS))

/* 0x908 : acomp_ctrl */
#define AON_ACOMP_CTRL_OFFSET                    (0x908)
#define AON_ACOMP1_RSTN_ANA                      AON_ACOMP1_RSTN_ANA
#define AON_ACOMP1_RSTN_ANA_POS                  (0U)
#define AON_ACOMP1_RSTN_ANA_LEN                  (1U)
#define AON_ACOMP1_RSTN_ANA_MSK                  (((1U << AON_ACOMP1_RSTN_ANA_LEN) - 1) << AON_ACOMP1_RSTN_ANA_POS)
#define AON_ACOMP1_RSTN_ANA_UMSK                 (~(((1U << AON_ACOMP1_RSTN_ANA_LEN) - 1) << AON_ACOMP1_RSTN_ANA_POS))
#define AON_ACOMP0_RSTN_ANA                      AON_ACOMP0_RSTN_ANA
#define AON_ACOMP0_RSTN_ANA_POS                  (1U)
#define AON_ACOMP0_RSTN_ANA_LEN                  (1U)
#define AON_ACOMP0_RSTN_ANA_MSK                  (((1U << AON_ACOMP0_RSTN_ANA_LEN) - 1) << AON_ACOMP0_RSTN_ANA_POS)
#define AON_ACOMP0_RSTN_ANA_UMSK                 (~(((1U << AON_ACOMP0_RSTN_ANA_LEN) - 1) << AON_ACOMP0_RSTN_ANA_POS))
#define AON_ACOMP1_TEST_EN                       AON_ACOMP1_TEST_EN
#define AON_ACOMP1_TEST_EN_POS                   (8U)
#define AON_ACOMP1_TEST_EN_LEN                   (1U)
#define AON_ACOMP1_TEST_EN_MSK                   (((1U << AON_ACOMP1_TEST_EN_LEN) - 1) << AON_ACOMP1_TEST_EN_POS)
#define AON_ACOMP1_TEST_EN_UMSK                  (~(((1U << AON_ACOMP1_TEST_EN_LEN) - 1) << AON_ACOMP1_TEST_EN_POS))
#define AON_ACOMP0_TEST_EN                       AON_ACOMP0_TEST_EN
#define AON_ACOMP0_TEST_EN_POS                   (9U)
#define AON_ACOMP0_TEST_EN_LEN                   (1U)
#define AON_ACOMP0_TEST_EN_MSK                   (((1U << AON_ACOMP0_TEST_EN_LEN) - 1) << AON_ACOMP0_TEST_EN_POS)
#define AON_ACOMP0_TEST_EN_UMSK                  (~(((1U << AON_ACOMP0_TEST_EN_LEN) - 1) << AON_ACOMP0_TEST_EN_POS))
#define AON_ACOMP1_TEST_SEL                      AON_ACOMP1_TEST_SEL
#define AON_ACOMP1_TEST_SEL_POS                  (10U)
#define AON_ACOMP1_TEST_SEL_LEN                  (2U)
#define AON_ACOMP1_TEST_SEL_MSK                  (((1U << AON_ACOMP1_TEST_SEL_LEN) - 1) << AON_ACOMP1_TEST_SEL_POS)
#define AON_ACOMP1_TEST_SEL_UMSK                 (~(((1U << AON_ACOMP1_TEST_SEL_LEN) - 1) << AON_ACOMP1_TEST_SEL_POS))
#define AON_ACOMP0_TEST_SEL                      AON_ACOMP0_TEST_SEL
#define AON_ACOMP0_TEST_SEL_POS                  (12U)
#define AON_ACOMP0_TEST_SEL_LEN                  (2U)
#define AON_ACOMP0_TEST_SEL_MSK                  (((1U << AON_ACOMP0_TEST_SEL_LEN) - 1) << AON_ACOMP0_TEST_SEL_POS)
#define AON_ACOMP0_TEST_SEL_UMSK                 (~(((1U << AON_ACOMP0_TEST_SEL_LEN) - 1) << AON_ACOMP0_TEST_SEL_POS))
#define AON_ACOMP1_OUT_RAW                       AON_ACOMP1_OUT_RAW
#define AON_ACOMP1_OUT_RAW_POS                   (17U)
#define AON_ACOMP1_OUT_RAW_LEN                   (1U)
#define AON_ACOMP1_OUT_RAW_MSK                   (((1U << AON_ACOMP1_OUT_RAW_LEN) - 1) << AON_ACOMP1_OUT_RAW_POS)
#define AON_ACOMP1_OUT_RAW_UMSK                  (~(((1U << AON_ACOMP1_OUT_RAW_LEN) - 1) << AON_ACOMP1_OUT_RAW_POS))
#define AON_ACOMP0_OUT_RAW                       AON_ACOMP0_OUT_RAW
#define AON_ACOMP0_OUT_RAW_POS                   (19U)
#define AON_ACOMP0_OUT_RAW_LEN                   (1U)
#define AON_ACOMP0_OUT_RAW_MSK                   (((1U << AON_ACOMP0_OUT_RAW_LEN) - 1) << AON_ACOMP0_OUT_RAW_POS)
#define AON_ACOMP0_OUT_RAW_UMSK                  (~(((1U << AON_ACOMP0_OUT_RAW_LEN) - 1) << AON_ACOMP0_OUT_RAW_POS))
#define AON_ACOMP_VREF_SEL                       AON_ACOMP_VREF_SEL
#define AON_ACOMP_VREF_SEL_POS                   (24U)
#define AON_ACOMP_VREF_SEL_LEN                   (6U)
#define AON_ACOMP_VREF_SEL_MSK                   (((1U << AON_ACOMP_VREF_SEL_LEN) - 1) << AON_ACOMP_VREF_SEL_POS)
#define AON_ACOMP_VREF_SEL_UMSK                  (~(((1U << AON_ACOMP_VREF_SEL_LEN) - 1) << AON_ACOMP_VREF_SEL_POS))
#define AON_ACOMP_RESERVED                       AON_ACOMP_RESERVED
#define AON_ACOMP_RESERVED_POS                   (30U)
#define AON_ACOMP_RESERVED_LEN                   (2U)
#define AON_ACOMP_RESERVED_MSK                   (((1U << AON_ACOMP_RESERVED_LEN) - 1) << AON_ACOMP_RESERVED_POS)
#define AON_ACOMP_RESERVED_UMSK                  (~(((1U << AON_ACOMP_RESERVED_LEN) - 1) << AON_ACOMP_RESERVED_POS))

/* 0x90C : gpadc_reg_cmd1 */
#define AON_GPADC_REG_CMD1_OFFSET                (0x90C)
#define AON_GPADC_TS_EN                          AON_GPADC_TS_EN
#define AON_GPADC_TS_EN_POS                      (0U)
#define AON_GPADC_TS_EN_LEN                      (1U)
#define AON_GPADC_TS_EN_MSK                      (((1U << AON_GPADC_TS_EN_LEN) - 1) << AON_GPADC_TS_EN_POS)
#define AON_GPADC_TS_EN_UMSK                     (~(((1U << AON_GPADC_TS_EN_LEN) - 1) << AON_GPADC_TS_EN_POS))
#define AON_GPADC_TSVBE_LOW                      AON_GPADC_TSVBE_LOW
#define AON_GPADC_TSVBE_LOW_POS                  (1U)
#define AON_GPADC_TSVBE_LOW_LEN                  (1U)
#define AON_GPADC_TSVBE_LOW_MSK                  (((1U << AON_GPADC_TSVBE_LOW_LEN) - 1) << AON_GPADC_TSVBE_LOW_POS)
#define AON_GPADC_TSVBE_LOW_UMSK                 (~(((1U << AON_GPADC_TSVBE_LOW_LEN) - 1) << AON_GPADC_TSVBE_LOW_POS))
#define AON_GPADC_VBAT_EN                        AON_GPADC_VBAT_EN
#define AON_GPADC_VBAT_EN_POS                    (2U)
#define AON_GPADC_VBAT_EN_LEN                    (1U)
#define AON_GPADC_VBAT_EN_MSK                    (((1U << AON_GPADC_VBAT_EN_LEN) - 1) << AON_GPADC_VBAT_EN_POS)
#define AON_GPADC_VBAT_EN_UMSK                   (~(((1U << AON_GPADC_VBAT_EN_LEN) - 1) << AON_GPADC_VBAT_EN_POS))
#define AON_GPADC_BIAS_SEL                       AON_GPADC_BIAS_SEL
#define AON_GPADC_BIAS_SEL_POS                   (3U)
#define AON_GPADC_BIAS_SEL_LEN                   (1U)
#define AON_GPADC_BIAS_SEL_MSK                   (((1U << AON_GPADC_BIAS_SEL_LEN) - 1) << AON_GPADC_BIAS_SEL_POS)
#define AON_GPADC_BIAS_SEL_UMSK                  (~(((1U << AON_GPADC_BIAS_SEL_LEN) - 1) << AON_GPADC_BIAS_SEL_POS))
#define AON_GPADC_INT_NEG_SEL                    AON_GPADC_INT_NEG_SEL
#define AON_GPADC_INT_NEG_SEL_POS                (4U)
#define AON_GPADC_INT_NEG_SEL_LEN                (5U)
#define AON_GPADC_INT_NEG_SEL_MSK                (((1U << AON_GPADC_INT_NEG_SEL_LEN) - 1) << AON_GPADC_INT_NEG_SEL_POS)
#define AON_GPADC_INT_NEG_SEL_UMSK               (~(((1U << AON_GPADC_INT_NEG_SEL_LEN) - 1) << AON_GPADC_INT_NEG_SEL_POS))
#define AON_GPADC_INT_POS_SEL                    AON_GPADC_INT_POS_SEL
#define AON_GPADC_INT_POS_SEL_POS                (12U)
#define AON_GPADC_INT_POS_SEL_LEN                (5U)
#define AON_GPADC_INT_POS_SEL_MSK                (((1U << AON_GPADC_INT_POS_SEL_LEN) - 1) << AON_GPADC_INT_POS_SEL_POS)
#define AON_GPADC_INT_POS_SEL_UMSK               (~(((1U << AON_GPADC_INT_POS_SEL_LEN) - 1) << AON_GPADC_INT_POS_SEL_POS))
#define AON_GPADC_CHIP_SEN_PU                    AON_GPADC_CHIP_SEN_PU
#define AON_GPADC_CHIP_SEN_PU_POS                (20U)
#define AON_GPADC_CHIP_SEN_PU_LEN                (1U)
#define AON_GPADC_CHIP_SEN_PU_MSK                (((1U << AON_GPADC_CHIP_SEN_PU_LEN) - 1) << AON_GPADC_CHIP_SEN_PU_POS)
#define AON_GPADC_CHIP_SEN_PU_UMSK               (~(((1U << AON_GPADC_CHIP_SEN_PU_LEN) - 1) << AON_GPADC_CHIP_SEN_PU_POS))
#define AON_GPADC_CHIP_SEN_SEL                   AON_GPADC_CHIP_SEN_SEL
#define AON_GPADC_CHIP_SEN_SEL_POS               (21U)
#define AON_GPADC_CHIP_SEN_SEL_LEN               (4U)
#define AON_GPADC_CHIP_SEN_SEL_MSK               (((1U << AON_GPADC_CHIP_SEN_SEL_LEN) - 1) << AON_GPADC_CHIP_SEN_SEL_POS)
#define AON_GPADC_CHIP_SEN_SEL_UMSK              (~(((1U << AON_GPADC_CHIP_SEN_SEL_LEN) - 1) << AON_GPADC_CHIP_SEN_SEL_POS))
#define AON_GPADC_DVDD_SEL                       AON_GPADC_DVDD_SEL
#define AON_GPADC_DVDD_SEL_POS                   (25U)
#define AON_GPADC_DVDD_SEL_LEN                   (2U)
#define AON_GPADC_DVDD_SEL_MSK                   (((1U << AON_GPADC_DVDD_SEL_LEN) - 1) << AON_GPADC_DVDD_SEL_POS)
#define AON_GPADC_DVDD_SEL_UMSK                  (~(((1U << AON_GPADC_DVDD_SEL_LEN) - 1) << AON_GPADC_DVDD_SEL_POS))
#define AON_GPADC_TSEXT_SEL                      AON_GPADC_TSEXT_SEL
#define AON_GPADC_TSEXT_SEL_POS                  (27U)
#define AON_GPADC_TSEXT_SEL_LEN                  (1U)
#define AON_GPADC_TSEXT_SEL_MSK                  (((1U << AON_GPADC_TSEXT_SEL_LEN) - 1) << AON_GPADC_TSEXT_SEL_POS)
#define AON_GPADC_TSEXT_SEL_UMSK                 (~(((1U << AON_GPADC_TSEXT_SEL_LEN) - 1) << AON_GPADC_TSEXT_SEL_POS))
#define AON_GPADC_VREF_EX_CH_SEL                 AON_GPADC_VREF_EX_CH_SEL
#define AON_GPADC_VREF_EX_CH_SEL_POS             (28U)
#define AON_GPADC_VREF_EX_CH_SEL_LEN             (4U)
#define AON_GPADC_VREF_EX_CH_SEL_MSK             (((1U << AON_GPADC_VREF_EX_CH_SEL_LEN) - 1) << AON_GPADC_VREF_EX_CH_SEL_POS)
#define AON_GPADC_VREF_EX_CH_SEL_UMSK            (~(((1U << AON_GPADC_VREF_EX_CH_SEL_LEN) - 1) << AON_GPADC_VREF_EX_CH_SEL_POS))

/* 0x910 : gpadc_reg_cmd2 */
#define AON_GPADC_REG_CMD2_OFFSET                (0x910)
#define AON_GPADC_MODE                           AON_GPADC_MODE
#define AON_GPADC_MODE_POS                       (0U)
#define AON_GPADC_MODE_LEN                       (2U)
#define AON_GPADC_MODE_MSK                       (((1U << AON_GPADC_MODE_LEN) - 1) << AON_GPADC_MODE_POS)
#define AON_GPADC_MODE_UMSK                      (~(((1U << AON_GPADC_MODE_LEN) - 1) << AON_GPADC_MODE_POS))
#define AON_GPADC_CLK_ANA_DLY                    AON_GPADC_CLK_ANA_DLY
#define AON_GPADC_CLK_ANA_DLY_POS                (8U)
#define AON_GPADC_CLK_ANA_DLY_LEN                (4U)
#define AON_GPADC_CLK_ANA_DLY_MSK                (((1U << AON_GPADC_CLK_ANA_DLY_LEN) - 1) << AON_GPADC_CLK_ANA_DLY_POS)
#define AON_GPADC_CLK_ANA_DLY_UMSK               (~(((1U << AON_GPADC_CLK_ANA_DLY_LEN) - 1) << AON_GPADC_CLK_ANA_DLY_POS))
#define AON_GPADC_CLK_ANA_DLY_EN                 AON_GPADC_CLK_ANA_DLY_EN
#define AON_GPADC_CLK_ANA_DLY_EN_POS             (12U)
#define AON_GPADC_CLK_ANA_DLY_EN_LEN             (1U)
#define AON_GPADC_CLK_ANA_DLY_EN_MSK             (((1U << AON_GPADC_CLK_ANA_DLY_EN_LEN) - 1) << AON_GPADC_CLK_ANA_DLY_EN_POS)
#define AON_GPADC_CLK_ANA_DLY_EN_UMSK            (~(((1U << AON_GPADC_CLK_ANA_DLY_EN_LEN) - 1) << AON_GPADC_CLK_ANA_DLY_EN_POS))
#define AON_GPADC_CLK_ANA_INV                    AON_GPADC_CLK_ANA_INV
#define AON_GPADC_CLK_ANA_INV_POS                (13U)
#define AON_GPADC_CLK_ANA_INV_LEN                (1U)
#define AON_GPADC_CLK_ANA_INV_MSK                (((1U << AON_GPADC_CLK_ANA_INV_LEN) - 1) << AON_GPADC_CLK_ANA_INV_POS)
#define AON_GPADC_CLK_ANA_INV_UMSK               (~(((1U << AON_GPADC_CLK_ANA_INV_LEN) - 1) << AON_GPADC_CLK_ANA_INV_POS))
#define AON_GPADC_CLK_DIV_RATIO                  AON_GPADC_CLK_DIV_RATIO
#define AON_GPADC_CLK_DIV_RATIO_POS              (16U)
#define AON_GPADC_CLK_DIV_RATIO_LEN              (3U)
#define AON_GPADC_CLK_DIV_RATIO_MSK              (((1U << AON_GPADC_CLK_DIV_RATIO_LEN) - 1) << AON_GPADC_CLK_DIV_RATIO_POS)
#define AON_GPADC_CLK_DIV_RATIO_UMSK             (~(((1U << AON_GPADC_CLK_DIV_RATIO_LEN) - 1) << AON_GPADC_CLK_DIV_RATIO_POS))

/* 0x914 : gpadc1_reg_config1 */
#define AON_GPADC1_REG_CONFIG1_OFFSET            (0x914)
#define AON_GPADC1_VREF_EX_SEL                   AON_GPADC1_VREF_EX_SEL
#define AON_GPADC1_VREF_EX_SEL_POS               (0U)
#define AON_GPADC1_VREF_EX_SEL_LEN               (1U)
#define AON_GPADC1_VREF_EX_SEL_MSK               (((1U << AON_GPADC1_VREF_EX_SEL_LEN) - 1) << AON_GPADC1_VREF_EX_SEL_POS)
#define AON_GPADC1_VREF_EX_SEL_UMSK              (~(((1U << AON_GPADC1_VREF_EX_SEL_LEN) - 1) << AON_GPADC1_VREF_EX_SEL_POS))
#define AON_GPADC1_DIFF_MODE                     AON_GPADC1_DIFF_MODE
#define AON_GPADC1_DIFF_MODE_POS                 (1U)
#define AON_GPADC1_DIFF_MODE_LEN                 (1U)
#define AON_GPADC1_DIFF_MODE_MSK                 (((1U << AON_GPADC1_DIFF_MODE_LEN) - 1) << AON_GPADC1_DIFF_MODE_POS)
#define AON_GPADC1_DIFF_MODE_UMSK                (~(((1U << AON_GPADC1_DIFF_MODE_LEN) - 1) << AON_GPADC1_DIFF_MODE_POS))
#define AON_GPADC1_NEG_GND                       AON_GPADC1_NEG_GND
#define AON_GPADC1_NEG_GND_POS                   (2U)
#define AON_GPADC1_NEG_GND_LEN                   (1U)
#define AON_GPADC1_NEG_GND_MSK                   (((1U << AON_GPADC1_NEG_GND_LEN) - 1) << AON_GPADC1_NEG_GND_POS)
#define AON_GPADC1_NEG_GND_UMSK                  (~(((1U << AON_GPADC1_NEG_GND_LEN) - 1) << AON_GPADC1_NEG_GND_POS))
#define AON_GPADC1_PGA_VCMI_EN                   AON_GPADC1_PGA_VCMI_EN
#define AON_GPADC1_PGA_VCMI_EN_POS               (3U)
#define AON_GPADC1_PGA_VCMI_EN_LEN               (1U)
#define AON_GPADC1_PGA_VCMI_EN_MSK               (((1U << AON_GPADC1_PGA_VCMI_EN_LEN) - 1) << AON_GPADC1_PGA_VCMI_EN_POS)
#define AON_GPADC1_PGA_VCMI_EN_UMSK              (~(((1U << AON_GPADC1_PGA_VCMI_EN_LEN) - 1) << AON_GPADC1_PGA_VCMI_EN_POS))
#define AON_GPADC1_VREF_TRIM                     AON_GPADC1_VREF_TRIM
#define AON_GPADC1_VREF_TRIM_POS                 (4U)
#define AON_GPADC1_VREF_TRIM_LEN                 (6U)
#define AON_GPADC1_VREF_TRIM_MSK                 (((1U << AON_GPADC1_VREF_TRIM_LEN) - 1) << AON_GPADC1_VREF_TRIM_POS)
#define AON_GPADC1_VREF_TRIM_UMSK                (~(((1U << AON_GPADC1_VREF_TRIM_LEN) - 1) << AON_GPADC1_VREF_TRIM_POS))
#define AON_GPADC1_CHOP_MODE                     AON_GPADC1_CHOP_MODE
#define AON_GPADC1_CHOP_MODE_POS                 (12U)
#define AON_GPADC1_CHOP_MODE_LEN                 (2U)
#define AON_GPADC1_CHOP_MODE_MSK                 (((1U << AON_GPADC1_CHOP_MODE_LEN) - 1) << AON_GPADC1_CHOP_MODE_POS)
#define AON_GPADC1_CHOP_MODE_UMSK                (~(((1U << AON_GPADC1_CHOP_MODE_LEN) - 1) << AON_GPADC1_CHOP_MODE_POS))
#define AON_GPADC1_PGA_EN                        AON_GPADC1_PGA_EN
#define AON_GPADC1_PGA_EN_POS                    (16U)
#define AON_GPADC1_PGA_EN_LEN                    (1U)
#define AON_GPADC1_PGA_EN_MSK                    (((1U << AON_GPADC1_PGA_EN_LEN) - 1) << AON_GPADC1_PGA_EN_POS)
#define AON_GPADC1_PGA_EN_UMSK                   (~(((1U << AON_GPADC1_PGA_EN_LEN) - 1) << AON_GPADC1_PGA_EN_POS))
#define AON_GPADC1_PGA_GAIN                      AON_GPADC1_PGA_GAIN
#define AON_GPADC1_PGA_GAIN_POS                  (20U)
#define AON_GPADC1_PGA_GAIN_LEN                  (3U)
#define AON_GPADC1_PGA_GAIN_MSK                  (((1U << AON_GPADC1_PGA_GAIN_LEN) - 1) << AON_GPADC1_PGA_GAIN_POS)
#define AON_GPADC1_PGA_GAIN_UMSK                 (~(((1U << AON_GPADC1_PGA_GAIN_LEN) - 1) << AON_GPADC1_PGA_GAIN_POS))
#define AON_GPADC1_PGA_VCM                       AON_GPADC1_PGA_VCM
#define AON_GPADC1_PGA_VCM_POS                   (24U)
#define AON_GPADC1_PGA_VCM_LEN                   (2U)
#define AON_GPADC1_PGA_VCM_MSK                   (((1U << AON_GPADC1_PGA_VCM_LEN) - 1) << AON_GPADC1_PGA_VCM_POS)
#define AON_GPADC1_PGA_VCM_UMSK                  (~(((1U << AON_GPADC1_PGA_VCM_LEN) - 1) << AON_GPADC1_PGA_VCM_POS))

/* 0x918 : gpadc1_reg_config2 */
#define AON_GPADC1_REG_CONFIG2_OFFSET            (0x918)
#define AON_GPADC1_GLOBAL_EN                     AON_GPADC1_GLOBAL_EN
#define AON_GPADC1_GLOBAL_EN_POS                 (0U)
#define AON_GPADC1_GLOBAL_EN_LEN                 (1U)
#define AON_GPADC1_GLOBAL_EN_MSK                 (((1U << AON_GPADC1_GLOBAL_EN_LEN) - 1) << AON_GPADC1_GLOBAL_EN_POS)
#define AON_GPADC1_GLOBAL_EN_UMSK                (~(((1U << AON_GPADC1_GLOBAL_EN_LEN) - 1) << AON_GPADC1_GLOBAL_EN_POS))
#define AON_GPADC1_CONV_START                    AON_GPADC1_CONV_START
#define AON_GPADC1_CONV_START_POS                (1U)
#define AON_GPADC1_CONV_START_LEN                (1U)
#define AON_GPADC1_CONV_START_MSK                (((1U << AON_GPADC1_CONV_START_LEN) - 1) << AON_GPADC1_CONV_START_POS)
#define AON_GPADC1_CONV_START_UMSK               (~(((1U << AON_GPADC1_CONV_START_LEN) - 1) << AON_GPADC1_CONV_START_POS))
#define AON_GPADC1_SOFT_RST                      AON_GPADC1_SOFT_RST
#define AON_GPADC1_SOFT_RST_POS                  (2U)
#define AON_GPADC1_SOFT_RST_LEN                  (1U)
#define AON_GPADC1_SOFT_RST_MSK                  (((1U << AON_GPADC1_SOFT_RST_LEN) - 1) << AON_GPADC1_SOFT_RST_POS)
#define AON_GPADC1_SOFT_RST_UMSK                 (~(((1U << AON_GPADC1_SOFT_RST_LEN) - 1) << AON_GPADC1_SOFT_RST_POS))
#define AON_GPADC1_DWA_EN                        AON_GPADC1_DWA_EN
#define AON_GPADC1_DWA_EN_POS                    (4U)
#define AON_GPADC1_DWA_EN_LEN                    (1U)
#define AON_GPADC1_DWA_EN_MSK                    (((1U << AON_GPADC1_DWA_EN_LEN) - 1) << AON_GPADC1_DWA_EN_POS)
#define AON_GPADC1_DWA_EN_UMSK                   (~(((1U << AON_GPADC1_DWA_EN_LEN) - 1) << AON_GPADC1_DWA_EN_POS))
#define AON_GPADC1_DITHER_EN                     AON_GPADC1_DITHER_EN
#define AON_GPADC1_DITHER_EN_POS                 (8U)
#define AON_GPADC1_DITHER_EN_LEN                 (1U)
#define AON_GPADC1_DITHER_EN_MSK                 (((1U << AON_GPADC1_DITHER_EN_LEN) - 1) << AON_GPADC1_DITHER_EN_POS)
#define AON_GPADC1_DITHER_EN_UMSK                (~(((1U << AON_GPADC1_DITHER_EN_LEN) - 1) << AON_GPADC1_DITHER_EN_POS))
#define AON_GPADC1_DLY_SEL                       AON_GPADC1_DLY_SEL
#define AON_GPADC1_DLY_SEL_POS                   (12U)
#define AON_GPADC1_DLY_SEL_LEN                   (3U)
#define AON_GPADC1_DLY_SEL_MSK                   (((1U << AON_GPADC1_DLY_SEL_LEN) - 1) << AON_GPADC1_DLY_SEL_POS)
#define AON_GPADC1_DLY_SEL_UMSK                  (~(((1U << AON_GPADC1_DLY_SEL_LEN) - 1) << AON_GPADC1_DLY_SEL_POS))
#define AON_GPADC1_EXT_POS_SEL                   AON_GPADC1_EXT_POS_SEL
#define AON_GPADC1_EXT_POS_SEL_POS               (16U)
#define AON_GPADC1_EXT_POS_SEL_LEN               (4U)
#define AON_GPADC1_EXT_POS_SEL_MSK               (((1U << AON_GPADC1_EXT_POS_SEL_LEN) - 1) << AON_GPADC1_EXT_POS_SEL_POS)
#define AON_GPADC1_EXT_POS_SEL_UMSK              (~(((1U << AON_GPADC1_EXT_POS_SEL_LEN) - 1) << AON_GPADC1_EXT_POS_SEL_POS))
#define AON_GPADC1_EXT_NEG_SEL                   AON_GPADC1_EXT_NEG_SEL
#define AON_GPADC1_EXT_NEG_SEL_POS               (20U)
#define AON_GPADC1_EXT_NEG_SEL_LEN               (4U)
#define AON_GPADC1_EXT_NEG_SEL_MSK               (((1U << AON_GPADC1_EXT_NEG_SEL_LEN) - 1) << AON_GPADC1_EXT_NEG_SEL_POS)
#define AON_GPADC1_EXT_NEG_SEL_UMSK              (~(((1U << AON_GPADC1_EXT_NEG_SEL_LEN) - 1) << AON_GPADC1_EXT_NEG_SEL_POS))

/* 0x91C : gpadc1_reg_ctrl_0 */
#define AON_GPADC1_REG_CTRL_0_OFFSET             (0x91C)
#define AON_GPADC1_CONT_CONV_EN                  AON_GPADC1_CONT_CONV_EN
#define AON_GPADC1_CONT_CONV_EN_POS              (0U)
#define AON_GPADC1_CONT_CONV_EN_LEN              (1U)
#define AON_GPADC1_CONT_CONV_EN_MSK              (((1U << AON_GPADC1_CONT_CONV_EN_LEN) - 1) << AON_GPADC1_CONT_CONV_EN_POS)
#define AON_GPADC1_CONT_CONV_EN_UMSK             (~(((1U << AON_GPADC1_CONT_CONV_EN_LEN) - 1) << AON_GPADC1_CONT_CONV_EN_POS))
#define AON_GPADC1_RES_SEL                       AON_GPADC1_RES_SEL
#define AON_GPADC1_RES_SEL_POS                   (1U)
#define AON_GPADC1_RES_SEL_LEN                   (3U)
#define AON_GPADC1_RES_SEL_MSK                   (((1U << AON_GPADC1_RES_SEL_LEN) - 1) << AON_GPADC1_RES_SEL_POS)
#define AON_GPADC1_RES_SEL_UMSK                  (~(((1U << AON_GPADC1_RES_SEL_LEN) - 1) << AON_GPADC1_RES_SEL_POS))
#define AON_GPADC1_CONV_DELAY                    AON_GPADC1_CONV_DELAY
#define AON_GPADC1_CONV_DELAY_POS                (4U)
#define AON_GPADC1_CONV_DELAY_LEN                (4U)
#define AON_GPADC1_CONV_DELAY_MSK                (((1U << AON_GPADC1_CONV_DELAY_LEN) - 1) << AON_GPADC1_CONV_DELAY_POS)
#define AON_GPADC1_CONV_DELAY_UMSK               (~(((1U << AON_GPADC1_CONV_DELAY_LEN) - 1) << AON_GPADC1_CONV_DELAY_POS))
#define AON_GPADC1_IJCT_START                    AON_GPADC1_IJCT_START
#define AON_GPADC1_IJCT_START_POS                (8U)
#define AON_GPADC1_IJCT_START_LEN                (1U)
#define AON_GPADC1_IJCT_START_MSK                (((1U << AON_GPADC1_IJCT_START_LEN) - 1) << AON_GPADC1_IJCT_START_POS)
#define AON_GPADC1_IJCT_START_UMSK               (~(((1U << AON_GPADC1_IJCT_START_LEN) - 1) << AON_GPADC1_IJCT_START_POS))
#define AON_GPADC1_IJCT_LENGTH                   AON_GPADC1_IJCT_LENGTH
#define AON_GPADC1_IJCT_LENGTH_POS               (9U)
#define AON_GPADC1_IJCT_LENGTH_LEN               (2U)
#define AON_GPADC1_IJCT_LENGTH_MSK               (((1U << AON_GPADC1_IJCT_LENGTH_LEN) - 1) << AON_GPADC1_IJCT_LENGTH_POS)
#define AON_GPADC1_IJCT_LENGTH_UMSK              (~(((1U << AON_GPADC1_IJCT_LENGTH_LEN) - 1) << AON_GPADC1_IJCT_LENGTH_POS))
#define AON_GPADC1_PWM_TRG_EN                    AON_GPADC1_PWM_TRG_EN
#define AON_GPADC1_PWM_TRG_EN_POS                (12U)
#define AON_GPADC1_PWM_TRG_EN_LEN                (1U)
#define AON_GPADC1_PWM_TRG_EN_MSK                (((1U << AON_GPADC1_PWM_TRG_EN_LEN) - 1) << AON_GPADC1_PWM_TRG_EN_POS)
#define AON_GPADC1_PWM_TRG_EN_UMSK               (~(((1U << AON_GPADC1_PWM_TRG_EN_LEN) - 1) << AON_GPADC1_PWM_TRG_EN_POS))
#define AON_GPADC1_SCAN_LENGTH                   AON_GPADC1_SCAN_LENGTH
#define AON_GPADC1_SCAN_LENGTH_POS               (16U)
#define AON_GPADC1_SCAN_LENGTH_LEN               (4U)
#define AON_GPADC1_SCAN_LENGTH_MSK               (((1U << AON_GPADC1_SCAN_LENGTH_LEN) - 1) << AON_GPADC1_SCAN_LENGTH_POS)
#define AON_GPADC1_SCAN_LENGTH_UMSK              (~(((1U << AON_GPADC1_SCAN_LENGTH_LEN) - 1) << AON_GPADC1_SCAN_LENGTH_POS))
#define AON_GPADC1_SCAN_EN                       AON_GPADC1_SCAN_EN
#define AON_GPADC1_SCAN_EN_POS                   (20U)
#define AON_GPADC1_SCAN_EN_LEN                   (1U)
#define AON_GPADC1_SCAN_EN_MSK                   (((1U << AON_GPADC1_SCAN_EN_LEN) - 1) << AON_GPADC1_SCAN_EN_POS)
#define AON_GPADC1_SCAN_EN_UMSK                  (~(((1U << AON_GPADC1_SCAN_EN_LEN) - 1) << AON_GPADC1_SCAN_EN_POS))

/* 0x920 : gpadc1_reg_os_cal_data */
#define AON_GPADC1_REG_OS_CAL_DATA_OFFSET        (0x920)
#define AON_GPADC1_OS_CAL_DATA                   AON_GPADC1_OS_CAL_DATA
#define AON_GPADC1_OS_CAL_DATA_POS               (0U)
#define AON_GPADC1_OS_CAL_DATA_LEN               (16U)
#define AON_GPADC1_OS_CAL_DATA_MSK               (((1U << AON_GPADC1_OS_CAL_DATA_LEN) - 1) << AON_GPADC1_OS_CAL_DATA_POS)
#define AON_GPADC1_OS_CAL_DATA_UMSK              (~(((1U << AON_GPADC1_OS_CAL_DATA_LEN) - 1) << AON_GPADC1_OS_CAL_DATA_POS))

/* 0x924 : gpadc1_reg_scan_0 */
#define AON_GPADC1_REG_SCAN_0_OFFSET             (0x924)
#define AON_GPADC1_SCAN_POS_0                    AON_GPADC1_SCAN_POS_0
#define AON_GPADC1_SCAN_POS_0_POS                (0U)
#define AON_GPADC1_SCAN_POS_0_LEN                (4U)
#define AON_GPADC1_SCAN_POS_0_MSK                (((1U << AON_GPADC1_SCAN_POS_0_LEN) - 1) << AON_GPADC1_SCAN_POS_0_POS)
#define AON_GPADC1_SCAN_POS_0_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_0_LEN) - 1) << AON_GPADC1_SCAN_POS_0_POS))
#define AON_GPADC1_SCAN_POS_1                    AON_GPADC1_SCAN_POS_1
#define AON_GPADC1_SCAN_POS_1_POS                (4U)
#define AON_GPADC1_SCAN_POS_1_LEN                (4U)
#define AON_GPADC1_SCAN_POS_1_MSK                (((1U << AON_GPADC1_SCAN_POS_1_LEN) - 1) << AON_GPADC1_SCAN_POS_1_POS)
#define AON_GPADC1_SCAN_POS_1_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_1_LEN) - 1) << AON_GPADC1_SCAN_POS_1_POS))
#define AON_GPADC1_SCAN_POS_2                    AON_GPADC1_SCAN_POS_2
#define AON_GPADC1_SCAN_POS_2_POS                (8U)
#define AON_GPADC1_SCAN_POS_2_LEN                (4U)
#define AON_GPADC1_SCAN_POS_2_MSK                (((1U << AON_GPADC1_SCAN_POS_2_LEN) - 1) << AON_GPADC1_SCAN_POS_2_POS)
#define AON_GPADC1_SCAN_POS_2_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_2_LEN) - 1) << AON_GPADC1_SCAN_POS_2_POS))
#define AON_GPADC1_SCAN_POS_3                    AON_GPADC1_SCAN_POS_3
#define AON_GPADC1_SCAN_POS_3_POS                (12U)
#define AON_GPADC1_SCAN_POS_3_LEN                (4U)
#define AON_GPADC1_SCAN_POS_3_MSK                (((1U << AON_GPADC1_SCAN_POS_3_LEN) - 1) << AON_GPADC1_SCAN_POS_3_POS)
#define AON_GPADC1_SCAN_POS_3_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_3_LEN) - 1) << AON_GPADC1_SCAN_POS_3_POS))
#define AON_GPADC1_SCAN_POS_4                    AON_GPADC1_SCAN_POS_4
#define AON_GPADC1_SCAN_POS_4_POS                (16U)
#define AON_GPADC1_SCAN_POS_4_LEN                (4U)
#define AON_GPADC1_SCAN_POS_4_MSK                (((1U << AON_GPADC1_SCAN_POS_4_LEN) - 1) << AON_GPADC1_SCAN_POS_4_POS)
#define AON_GPADC1_SCAN_POS_4_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_4_LEN) - 1) << AON_GPADC1_SCAN_POS_4_POS))
#define AON_GPADC1_SCAN_POS_5                    AON_GPADC1_SCAN_POS_5
#define AON_GPADC1_SCAN_POS_5_POS                (20U)
#define AON_GPADC1_SCAN_POS_5_LEN                (4U)
#define AON_GPADC1_SCAN_POS_5_MSK                (((1U << AON_GPADC1_SCAN_POS_5_LEN) - 1) << AON_GPADC1_SCAN_POS_5_POS)
#define AON_GPADC1_SCAN_POS_5_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_5_LEN) - 1) << AON_GPADC1_SCAN_POS_5_POS))
#define AON_GPADC1_SCAN_POS_6                    AON_GPADC1_SCAN_POS_6
#define AON_GPADC1_SCAN_POS_6_POS                (24U)
#define AON_GPADC1_SCAN_POS_6_LEN                (4U)
#define AON_GPADC1_SCAN_POS_6_MSK                (((1U << AON_GPADC1_SCAN_POS_6_LEN) - 1) << AON_GPADC1_SCAN_POS_6_POS)
#define AON_GPADC1_SCAN_POS_6_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_6_LEN) - 1) << AON_GPADC1_SCAN_POS_6_POS))
#define AON_GPADC1_SCAN_POS_7                    AON_GPADC1_SCAN_POS_7
#define AON_GPADC1_SCAN_POS_7_POS                (28U)
#define AON_GPADC1_SCAN_POS_7_LEN                (4U)
#define AON_GPADC1_SCAN_POS_7_MSK                (((1U << AON_GPADC1_SCAN_POS_7_LEN) - 1) << AON_GPADC1_SCAN_POS_7_POS)
#define AON_GPADC1_SCAN_POS_7_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_7_LEN) - 1) << AON_GPADC1_SCAN_POS_7_POS))

/* 0x928 : gpadc1_reg_scan_1 */
#define AON_GPADC1_REG_SCAN_1_OFFSET             (0x928)
#define AON_GPADC1_SCAN_POS_8                    AON_GPADC1_SCAN_POS_8
#define AON_GPADC1_SCAN_POS_8_POS                (0U)
#define AON_GPADC1_SCAN_POS_8_LEN                (4U)
#define AON_GPADC1_SCAN_POS_8_MSK                (((1U << AON_GPADC1_SCAN_POS_8_LEN) - 1) << AON_GPADC1_SCAN_POS_8_POS)
#define AON_GPADC1_SCAN_POS_8_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_8_LEN) - 1) << AON_GPADC1_SCAN_POS_8_POS))
#define AON_GPADC1_SCAN_POS_9                    AON_GPADC1_SCAN_POS_9
#define AON_GPADC1_SCAN_POS_9_POS                (4U)
#define AON_GPADC1_SCAN_POS_9_LEN                (4U)
#define AON_GPADC1_SCAN_POS_9_MSK                (((1U << AON_GPADC1_SCAN_POS_9_LEN) - 1) << AON_GPADC1_SCAN_POS_9_POS)
#define AON_GPADC1_SCAN_POS_9_UMSK               (~(((1U << AON_GPADC1_SCAN_POS_9_LEN) - 1) << AON_GPADC1_SCAN_POS_9_POS))
#define AON_GPADC1_SCAN_POS_10                   AON_GPADC1_SCAN_POS_10
#define AON_GPADC1_SCAN_POS_10_POS               (8U)
#define AON_GPADC1_SCAN_POS_10_LEN               (4U)
#define AON_GPADC1_SCAN_POS_10_MSK               (((1U << AON_GPADC1_SCAN_POS_10_LEN) - 1) << AON_GPADC1_SCAN_POS_10_POS)
#define AON_GPADC1_SCAN_POS_10_UMSK              (~(((1U << AON_GPADC1_SCAN_POS_10_LEN) - 1) << AON_GPADC1_SCAN_POS_10_POS))
#define AON_GPADC1_SCAN_POS_11                   AON_GPADC1_SCAN_POS_11
#define AON_GPADC1_SCAN_POS_11_POS               (12U)
#define AON_GPADC1_SCAN_POS_11_LEN               (4U)
#define AON_GPADC1_SCAN_POS_11_MSK               (((1U << AON_GPADC1_SCAN_POS_11_LEN) - 1) << AON_GPADC1_SCAN_POS_11_POS)
#define AON_GPADC1_SCAN_POS_11_UMSK              (~(((1U << AON_GPADC1_SCAN_POS_11_LEN) - 1) << AON_GPADC1_SCAN_POS_11_POS))
#define AON_GPADC1_SCAN_POS_12                   AON_GPADC1_SCAN_POS_12
#define AON_GPADC1_SCAN_POS_12_POS               (16U)
#define AON_GPADC1_SCAN_POS_12_LEN               (4U)
#define AON_GPADC1_SCAN_POS_12_MSK               (((1U << AON_GPADC1_SCAN_POS_12_LEN) - 1) << AON_GPADC1_SCAN_POS_12_POS)
#define AON_GPADC1_SCAN_POS_12_UMSK              (~(((1U << AON_GPADC1_SCAN_POS_12_LEN) - 1) << AON_GPADC1_SCAN_POS_12_POS))
#define AON_GPADC1_SCAN_POS_13                   AON_GPADC1_SCAN_POS_13
#define AON_GPADC1_SCAN_POS_13_POS               (20U)
#define AON_GPADC1_SCAN_POS_13_LEN               (4U)
#define AON_GPADC1_SCAN_POS_13_MSK               (((1U << AON_GPADC1_SCAN_POS_13_LEN) - 1) << AON_GPADC1_SCAN_POS_13_POS)
#define AON_GPADC1_SCAN_POS_13_UMSK              (~(((1U << AON_GPADC1_SCAN_POS_13_LEN) - 1) << AON_GPADC1_SCAN_POS_13_POS))
#define AON_GPADC1_SCAN_POS_14                   AON_GPADC1_SCAN_POS_14
#define AON_GPADC1_SCAN_POS_14_POS               (24U)
#define AON_GPADC1_SCAN_POS_14_LEN               (4U)
#define AON_GPADC1_SCAN_POS_14_MSK               (((1U << AON_GPADC1_SCAN_POS_14_LEN) - 1) << AON_GPADC1_SCAN_POS_14_POS)
#define AON_GPADC1_SCAN_POS_14_UMSK              (~(((1U << AON_GPADC1_SCAN_POS_14_LEN) - 1) << AON_GPADC1_SCAN_POS_14_POS))
#define AON_GPADC1_SCAN_POS_15                   AON_GPADC1_SCAN_POS_15
#define AON_GPADC1_SCAN_POS_15_POS               (28U)
#define AON_GPADC1_SCAN_POS_15_LEN               (4U)
#define AON_GPADC1_SCAN_POS_15_MSK               (((1U << AON_GPADC1_SCAN_POS_15_LEN) - 1) << AON_GPADC1_SCAN_POS_15_POS)
#define AON_GPADC1_SCAN_POS_15_UMSK              (~(((1U << AON_GPADC1_SCAN_POS_15_LEN) - 1) << AON_GPADC1_SCAN_POS_15_POS))

/* 0x92C : gpadc1_reg_scan_2 */
#define AON_GPADC1_REG_SCAN_2_OFFSET             (0x92C)
#define AON_GPADC1_SCAN_NEG_0                    AON_GPADC1_SCAN_NEG_0
#define AON_GPADC1_SCAN_NEG_0_POS                (0U)
#define AON_GPADC1_SCAN_NEG_0_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_0_MSK                (((1U << AON_GPADC1_SCAN_NEG_0_LEN) - 1) << AON_GPADC1_SCAN_NEG_0_POS)
#define AON_GPADC1_SCAN_NEG_0_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_0_LEN) - 1) << AON_GPADC1_SCAN_NEG_0_POS))
#define AON_GPADC1_SCAN_NEG_1                    AON_GPADC1_SCAN_NEG_1
#define AON_GPADC1_SCAN_NEG_1_POS                (4U)
#define AON_GPADC1_SCAN_NEG_1_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_1_MSK                (((1U << AON_GPADC1_SCAN_NEG_1_LEN) - 1) << AON_GPADC1_SCAN_NEG_1_POS)
#define AON_GPADC1_SCAN_NEG_1_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_1_LEN) - 1) << AON_GPADC1_SCAN_NEG_1_POS))
#define AON_GPADC1_SCAN_NEG_2                    AON_GPADC1_SCAN_NEG_2
#define AON_GPADC1_SCAN_NEG_2_POS                (8U)
#define AON_GPADC1_SCAN_NEG_2_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_2_MSK                (((1U << AON_GPADC1_SCAN_NEG_2_LEN) - 1) << AON_GPADC1_SCAN_NEG_2_POS)
#define AON_GPADC1_SCAN_NEG_2_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_2_LEN) - 1) << AON_GPADC1_SCAN_NEG_2_POS))
#define AON_GPADC1_SCAN_NEG_3                    AON_GPADC1_SCAN_NEG_3
#define AON_GPADC1_SCAN_NEG_3_POS                (12U)
#define AON_GPADC1_SCAN_NEG_3_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_3_MSK                (((1U << AON_GPADC1_SCAN_NEG_3_LEN) - 1) << AON_GPADC1_SCAN_NEG_3_POS)
#define AON_GPADC1_SCAN_NEG_3_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_3_LEN) - 1) << AON_GPADC1_SCAN_NEG_3_POS))
#define AON_GPADC1_SCAN_NEG_4                    AON_GPADC1_SCAN_NEG_4
#define AON_GPADC1_SCAN_NEG_4_POS                (16U)
#define AON_GPADC1_SCAN_NEG_4_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_4_MSK                (((1U << AON_GPADC1_SCAN_NEG_4_LEN) - 1) << AON_GPADC1_SCAN_NEG_4_POS)
#define AON_GPADC1_SCAN_NEG_4_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_4_LEN) - 1) << AON_GPADC1_SCAN_NEG_4_POS))
#define AON_GPADC1_SCAN_NEG_5                    AON_GPADC1_SCAN_NEG_5
#define AON_GPADC1_SCAN_NEG_5_POS                (20U)
#define AON_GPADC1_SCAN_NEG_5_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_5_MSK                (((1U << AON_GPADC1_SCAN_NEG_5_LEN) - 1) << AON_GPADC1_SCAN_NEG_5_POS)
#define AON_GPADC1_SCAN_NEG_5_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_5_LEN) - 1) << AON_GPADC1_SCAN_NEG_5_POS))
#define AON_GPADC1_SCAN_NEG_6                    AON_GPADC1_SCAN_NEG_6
#define AON_GPADC1_SCAN_NEG_6_POS                (24U)
#define AON_GPADC1_SCAN_NEG_6_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_6_MSK                (((1U << AON_GPADC1_SCAN_NEG_6_LEN) - 1) << AON_GPADC1_SCAN_NEG_6_POS)
#define AON_GPADC1_SCAN_NEG_6_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_6_LEN) - 1) << AON_GPADC1_SCAN_NEG_6_POS))
#define AON_GPADC1_SCAN_NEG_7                    AON_GPADC1_SCAN_NEG_7
#define AON_GPADC1_SCAN_NEG_7_POS                (28U)
#define AON_GPADC1_SCAN_NEG_7_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_7_MSK                (((1U << AON_GPADC1_SCAN_NEG_7_LEN) - 1) << AON_GPADC1_SCAN_NEG_7_POS)
#define AON_GPADC1_SCAN_NEG_7_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_7_LEN) - 1) << AON_GPADC1_SCAN_NEG_7_POS))

/* 0x930 : gpadc1_reg_scan_3 */
#define AON_GPADC1_REG_SCAN_3_OFFSET             (0x930)
#define AON_GPADC1_SCAN_NEG_8                    AON_GPADC1_SCAN_NEG_8
#define AON_GPADC1_SCAN_NEG_8_POS                (0U)
#define AON_GPADC1_SCAN_NEG_8_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_8_MSK                (((1U << AON_GPADC1_SCAN_NEG_8_LEN) - 1) << AON_GPADC1_SCAN_NEG_8_POS)
#define AON_GPADC1_SCAN_NEG_8_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_8_LEN) - 1) << AON_GPADC1_SCAN_NEG_8_POS))
#define AON_GPADC1_SCAN_NEG_9                    AON_GPADC1_SCAN_NEG_9
#define AON_GPADC1_SCAN_NEG_9_POS                (4U)
#define AON_GPADC1_SCAN_NEG_9_LEN                (4U)
#define AON_GPADC1_SCAN_NEG_9_MSK                (((1U << AON_GPADC1_SCAN_NEG_9_LEN) - 1) << AON_GPADC1_SCAN_NEG_9_POS)
#define AON_GPADC1_SCAN_NEG_9_UMSK               (~(((1U << AON_GPADC1_SCAN_NEG_9_LEN) - 1) << AON_GPADC1_SCAN_NEG_9_POS))
#define AON_GPADC1_SCAN_NEG_10                   AON_GPADC1_SCAN_NEG_10
#define AON_GPADC1_SCAN_NEG_10_POS               (8U)
#define AON_GPADC1_SCAN_NEG_10_LEN               (4U)
#define AON_GPADC1_SCAN_NEG_10_MSK               (((1U << AON_GPADC1_SCAN_NEG_10_LEN) - 1) << AON_GPADC1_SCAN_NEG_10_POS)
#define AON_GPADC1_SCAN_NEG_10_UMSK              (~(((1U << AON_GPADC1_SCAN_NEG_10_LEN) - 1) << AON_GPADC1_SCAN_NEG_10_POS))
#define AON_GPADC1_SCAN_NEG_11                   AON_GPADC1_SCAN_NEG_11
#define AON_GPADC1_SCAN_NEG_11_POS               (12U)
#define AON_GPADC1_SCAN_NEG_11_LEN               (4U)
#define AON_GPADC1_SCAN_NEG_11_MSK               (((1U << AON_GPADC1_SCAN_NEG_11_LEN) - 1) << AON_GPADC1_SCAN_NEG_11_POS)
#define AON_GPADC1_SCAN_NEG_11_UMSK              (~(((1U << AON_GPADC1_SCAN_NEG_11_LEN) - 1) << AON_GPADC1_SCAN_NEG_11_POS))
#define AON_GPADC1_SCAN_NEG_12                   AON_GPADC1_SCAN_NEG_12
#define AON_GPADC1_SCAN_NEG_12_POS               (16U)
#define AON_GPADC1_SCAN_NEG_12_LEN               (4U)
#define AON_GPADC1_SCAN_NEG_12_MSK               (((1U << AON_GPADC1_SCAN_NEG_12_LEN) - 1) << AON_GPADC1_SCAN_NEG_12_POS)
#define AON_GPADC1_SCAN_NEG_12_UMSK              (~(((1U << AON_GPADC1_SCAN_NEG_12_LEN) - 1) << AON_GPADC1_SCAN_NEG_12_POS))
#define AON_GPADC1_SCAN_NEG_13                   AON_GPADC1_SCAN_NEG_13
#define AON_GPADC1_SCAN_NEG_13_POS               (20U)
#define AON_GPADC1_SCAN_NEG_13_LEN               (4U)
#define AON_GPADC1_SCAN_NEG_13_MSK               (((1U << AON_GPADC1_SCAN_NEG_13_LEN) - 1) << AON_GPADC1_SCAN_NEG_13_POS)
#define AON_GPADC1_SCAN_NEG_13_UMSK              (~(((1U << AON_GPADC1_SCAN_NEG_13_LEN) - 1) << AON_GPADC1_SCAN_NEG_13_POS))
#define AON_GPADC1_SCAN_NEG_14                   AON_GPADC1_SCAN_NEG_14
#define AON_GPADC1_SCAN_NEG_14_POS               (24U)
#define AON_GPADC1_SCAN_NEG_14_LEN               (4U)
#define AON_GPADC1_SCAN_NEG_14_MSK               (((1U << AON_GPADC1_SCAN_NEG_14_LEN) - 1) << AON_GPADC1_SCAN_NEG_14_POS)
#define AON_GPADC1_SCAN_NEG_14_UMSK              (~(((1U << AON_GPADC1_SCAN_NEG_14_LEN) - 1) << AON_GPADC1_SCAN_NEG_14_POS))
#define AON_GPADC1_SCAN_NEG_15                   AON_GPADC1_SCAN_NEG_15
#define AON_GPADC1_SCAN_NEG_15_POS               (28U)
#define AON_GPADC1_SCAN_NEG_15_LEN               (4U)
#define AON_GPADC1_SCAN_NEG_15_MSK               (((1U << AON_GPADC1_SCAN_NEG_15_LEN) - 1) << AON_GPADC1_SCAN_NEG_15_POS)
#define AON_GPADC1_SCAN_NEG_15_UMSK              (~(((1U << AON_GPADC1_SCAN_NEG_15_LEN) - 1) << AON_GPADC1_SCAN_NEG_15_POS))

/* 0x934 : gpadc1_reg_ijct_ext_0 */
#define AON_GPADC1_REG_IJCT_EXT_0_OFFSET         (0x934)
#define AON_GPADC1_IJCT_EXT_POS_0                AON_GPADC1_IJCT_EXT_POS_0
#define AON_GPADC1_IJCT_EXT_POS_0_POS            (0U)
#define AON_GPADC1_IJCT_EXT_POS_0_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_POS_0_MSK            (((1U << AON_GPADC1_IJCT_EXT_POS_0_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_0_POS)
#define AON_GPADC1_IJCT_EXT_POS_0_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_POS_0_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_0_POS))
#define AON_GPADC1_IJCT_EXT_POS_1                AON_GPADC1_IJCT_EXT_POS_1
#define AON_GPADC1_IJCT_EXT_POS_1_POS            (4U)
#define AON_GPADC1_IJCT_EXT_POS_1_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_POS_1_MSK            (((1U << AON_GPADC1_IJCT_EXT_POS_1_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_1_POS)
#define AON_GPADC1_IJCT_EXT_POS_1_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_POS_1_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_1_POS))
#define AON_GPADC1_IJCT_EXT_POS_2                AON_GPADC1_IJCT_EXT_POS_2
#define AON_GPADC1_IJCT_EXT_POS_2_POS            (8U)
#define AON_GPADC1_IJCT_EXT_POS_2_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_POS_2_MSK            (((1U << AON_GPADC1_IJCT_EXT_POS_2_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_2_POS)
#define AON_GPADC1_IJCT_EXT_POS_2_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_POS_2_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_2_POS))
#define AON_GPADC1_IJCT_EXT_POS_3                AON_GPADC1_IJCT_EXT_POS_3
#define AON_GPADC1_IJCT_EXT_POS_3_POS            (12U)
#define AON_GPADC1_IJCT_EXT_POS_3_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_POS_3_MSK            (((1U << AON_GPADC1_IJCT_EXT_POS_3_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_3_POS)
#define AON_GPADC1_IJCT_EXT_POS_3_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_POS_3_LEN) - 1) << AON_GPADC1_IJCT_EXT_POS_3_POS))
#define AON_GPADC1_IJCT_EXT_NEG_0                AON_GPADC1_IJCT_EXT_NEG_0
#define AON_GPADC1_IJCT_EXT_NEG_0_POS            (16U)
#define AON_GPADC1_IJCT_EXT_NEG_0_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_NEG_0_MSK            (((1U << AON_GPADC1_IJCT_EXT_NEG_0_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_0_POS)
#define AON_GPADC1_IJCT_EXT_NEG_0_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_NEG_0_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_0_POS))
#define AON_GPADC1_IJCT_EXT_NEG_1                AON_GPADC1_IJCT_EXT_NEG_1
#define AON_GPADC1_IJCT_EXT_NEG_1_POS            (20U)
#define AON_GPADC1_IJCT_EXT_NEG_1_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_NEG_1_MSK            (((1U << AON_GPADC1_IJCT_EXT_NEG_1_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_1_POS)
#define AON_GPADC1_IJCT_EXT_NEG_1_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_NEG_1_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_1_POS))
#define AON_GPADC1_IJCT_EXT_NEG_2                AON_GPADC1_IJCT_EXT_NEG_2
#define AON_GPADC1_IJCT_EXT_NEG_2_POS            (24U)
#define AON_GPADC1_IJCT_EXT_NEG_2_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_NEG_2_MSK            (((1U << AON_GPADC1_IJCT_EXT_NEG_2_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_2_POS)
#define AON_GPADC1_IJCT_EXT_NEG_2_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_NEG_2_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_2_POS))
#define AON_GPADC1_IJCT_EXT_NEG_3                AON_GPADC1_IJCT_EXT_NEG_3
#define AON_GPADC1_IJCT_EXT_NEG_3_POS            (28U)
#define AON_GPADC1_IJCT_EXT_NEG_3_LEN            (4U)
#define AON_GPADC1_IJCT_EXT_NEG_3_MSK            (((1U << AON_GPADC1_IJCT_EXT_NEG_3_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_3_POS)
#define AON_GPADC1_IJCT_EXT_NEG_3_UMSK           (~(((1U << AON_GPADC1_IJCT_EXT_NEG_3_LEN) - 1) << AON_GPADC1_IJCT_EXT_NEG_3_POS))

/* 0x938 : gpadc1_reg_ijct_int_0 */
#define AON_GPADC1_REG_IJCT_INT_0_OFFSET         (0x938)
#define AON_GPADC1_IJCT_INT_POS_0                AON_GPADC1_IJCT_INT_POS_0
#define AON_GPADC1_IJCT_INT_POS_0_POS            (0U)
#define AON_GPADC1_IJCT_INT_POS_0_LEN            (5U)
#define AON_GPADC1_IJCT_INT_POS_0_MSK            (((1U << AON_GPADC1_IJCT_INT_POS_0_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_0_POS)
#define AON_GPADC1_IJCT_INT_POS_0_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_POS_0_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_0_POS))
#define AON_GPADC1_IJCT_INT_POS_1                AON_GPADC1_IJCT_INT_POS_1
#define AON_GPADC1_IJCT_INT_POS_1_POS            (8U)
#define AON_GPADC1_IJCT_INT_POS_1_LEN            (5U)
#define AON_GPADC1_IJCT_INT_POS_1_MSK            (((1U << AON_GPADC1_IJCT_INT_POS_1_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_1_POS)
#define AON_GPADC1_IJCT_INT_POS_1_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_POS_1_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_1_POS))
#define AON_GPADC1_IJCT_INT_POS_2                AON_GPADC1_IJCT_INT_POS_2
#define AON_GPADC1_IJCT_INT_POS_2_POS            (16U)
#define AON_GPADC1_IJCT_INT_POS_2_LEN            (5U)
#define AON_GPADC1_IJCT_INT_POS_2_MSK            (((1U << AON_GPADC1_IJCT_INT_POS_2_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_2_POS)
#define AON_GPADC1_IJCT_INT_POS_2_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_POS_2_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_2_POS))
#define AON_GPADC1_IJCT_INT_POS_3                AON_GPADC1_IJCT_INT_POS_3
#define AON_GPADC1_IJCT_INT_POS_3_POS            (24U)
#define AON_GPADC1_IJCT_INT_POS_3_LEN            (5U)
#define AON_GPADC1_IJCT_INT_POS_3_MSK            (((1U << AON_GPADC1_IJCT_INT_POS_3_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_3_POS)
#define AON_GPADC1_IJCT_INT_POS_3_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_POS_3_LEN) - 1) << AON_GPADC1_IJCT_INT_POS_3_POS))

/* 0x93C : gpadc1_reg_ijct_int_1 */
#define AON_GPADC1_REG_IJCT_INT_1_OFFSET         (0x93C)
#define AON_GPADC1_IJCT_INT_NEG_0                AON_GPADC1_IJCT_INT_NEG_0
#define AON_GPADC1_IJCT_INT_NEG_0_POS            (0U)
#define AON_GPADC1_IJCT_INT_NEG_0_LEN            (5U)
#define AON_GPADC1_IJCT_INT_NEG_0_MSK            (((1U << AON_GPADC1_IJCT_INT_NEG_0_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_0_POS)
#define AON_GPADC1_IJCT_INT_NEG_0_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_NEG_0_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_0_POS))
#define AON_GPADC1_IJCT_INT_NEG_1                AON_GPADC1_IJCT_INT_NEG_1
#define AON_GPADC1_IJCT_INT_NEG_1_POS            (8U)
#define AON_GPADC1_IJCT_INT_NEG_1_LEN            (5U)
#define AON_GPADC1_IJCT_INT_NEG_1_MSK            (((1U << AON_GPADC1_IJCT_INT_NEG_1_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_1_POS)
#define AON_GPADC1_IJCT_INT_NEG_1_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_NEG_1_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_1_POS))
#define AON_GPADC1_IJCT_INT_NEG_2                AON_GPADC1_IJCT_INT_NEG_2
#define AON_GPADC1_IJCT_INT_NEG_2_POS            (16U)
#define AON_GPADC1_IJCT_INT_NEG_2_LEN            (5U)
#define AON_GPADC1_IJCT_INT_NEG_2_MSK            (((1U << AON_GPADC1_IJCT_INT_NEG_2_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_2_POS)
#define AON_GPADC1_IJCT_INT_NEG_2_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_NEG_2_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_2_POS))
#define AON_GPADC1_IJCT_INT_NEG_3                AON_GPADC1_IJCT_INT_NEG_3
#define AON_GPADC1_IJCT_INT_NEG_3_POS            (24U)
#define AON_GPADC1_IJCT_INT_NEG_3_LEN            (5U)
#define AON_GPADC1_IJCT_INT_NEG_3_MSK            (((1U << AON_GPADC1_IJCT_INT_NEG_3_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_3_POS)
#define AON_GPADC1_IJCT_INT_NEG_3_UMSK           (~(((1U << AON_GPADC1_IJCT_INT_NEG_3_LEN) - 1) << AON_GPADC1_IJCT_INT_NEG_3_POS))

/* 0x940 : gpadc1_reg_result */
#define AON_GPADC1_REG_RESULT_OFFSET             (0x940)
#define AON_GPADC1_DATA_OUT                      AON_GPADC1_DATA_OUT
#define AON_GPADC1_DATA_OUT_POS                  (0U)
#define AON_GPADC1_DATA_OUT_LEN                  (29U)
#define AON_GPADC1_DATA_OUT_MSK                  (((1U << AON_GPADC1_DATA_OUT_LEN) - 1) << AON_GPADC1_DATA_OUT_POS)
#define AON_GPADC1_DATA_OUT_UMSK                 (~(((1U << AON_GPADC1_DATA_OUT_LEN) - 1) << AON_GPADC1_DATA_OUT_POS))
#define AON_GPADC1_DATA_RDY                      AON_GPADC1_DATA_RDY
#define AON_GPADC1_DATA_RDY_POS                  (31U)
#define AON_GPADC1_DATA_RDY_LEN                  (1U)
#define AON_GPADC1_DATA_RDY_MSK                  (((1U << AON_GPADC1_DATA_RDY_LEN) - 1) << AON_GPADC1_DATA_RDY_POS)
#define AON_GPADC1_DATA_RDY_UMSK                 (~(((1U << AON_GPADC1_DATA_RDY_LEN) - 1) << AON_GPADC1_DATA_RDY_POS))

/* 0x944 : gpadc1_reg_raw_data */
#define AON_GPADC1_REG_RAW_DATA_OFFSET           (0x944)
#define AON_GPADC1_RAW_DATA                      AON_GPADC1_RAW_DATA
#define AON_GPADC1_RAW_DATA_POS                  (0U)
#define AON_GPADC1_RAW_DATA_LEN                  (12U)
#define AON_GPADC1_RAW_DATA_MSK                  (((1U << AON_GPADC1_RAW_DATA_LEN) - 1) << AON_GPADC1_RAW_DATA_POS)
#define AON_GPADC1_RAW_DATA_UMSK                 (~(((1U << AON_GPADC1_RAW_DATA_LEN) - 1) << AON_GPADC1_RAW_DATA_POS))
#define AON_GPADC1_RESERVED                      AON_GPADC1_RESERVED
#define AON_GPADC1_RESERVED_POS                  (16U)
#define AON_GPADC1_RESERVED_LEN                  (16U)
#define AON_GPADC1_RESERVED_MSK                  (((1U << AON_GPADC1_RESERVED_LEN) - 1) << AON_GPADC1_RESERVED_POS)
#define AON_GPADC1_RESERVED_UMSK                 (~(((1U << AON_GPADC1_RESERVED_LEN) - 1) << AON_GPADC1_RESERVED_POS))

/* 0x948 : gpadc1_reg_satur */
#define AON_GPADC1_REG_SATUR_OFFSET              (0x948)
#define AON_GPADC1_NEG_SATUR                     AON_GPADC1_NEG_SATUR
#define AON_GPADC1_NEG_SATUR_POS                 (0U)
#define AON_GPADC1_NEG_SATUR_LEN                 (1U)
#define AON_GPADC1_NEG_SATUR_MSK                 (((1U << AON_GPADC1_NEG_SATUR_LEN) - 1) << AON_GPADC1_NEG_SATUR_POS)
#define AON_GPADC1_NEG_SATUR_UMSK                (~(((1U << AON_GPADC1_NEG_SATUR_LEN) - 1) << AON_GPADC1_NEG_SATUR_POS))
#define AON_GPADC1_POS_SATUR                     AON_GPADC1_POS_SATUR
#define AON_GPADC1_POS_SATUR_POS                 (1U)
#define AON_GPADC1_POS_SATUR_LEN                 (1U)
#define AON_GPADC1_POS_SATUR_MSK                 (((1U << AON_GPADC1_POS_SATUR_LEN) - 1) << AON_GPADC1_POS_SATUR_POS)
#define AON_GPADC1_POS_SATUR_UMSK                (~(((1U << AON_GPADC1_POS_SATUR_LEN) - 1) << AON_GPADC1_POS_SATUR_POS))
#define AON_GPADC1_NEG_SATUR_CLR                 AON_GPADC1_NEG_SATUR_CLR
#define AON_GPADC1_NEG_SATUR_CLR_POS             (4U)
#define AON_GPADC1_NEG_SATUR_CLR_LEN             (1U)
#define AON_GPADC1_NEG_SATUR_CLR_MSK             (((1U << AON_GPADC1_NEG_SATUR_CLR_LEN) - 1) << AON_GPADC1_NEG_SATUR_CLR_POS)
#define AON_GPADC1_NEG_SATUR_CLR_UMSK            (~(((1U << AON_GPADC1_NEG_SATUR_CLR_LEN) - 1) << AON_GPADC1_NEG_SATUR_CLR_POS))
#define AON_GPADC1_POS_SATUR_CLR                 AON_GPADC1_POS_SATUR_CLR
#define AON_GPADC1_POS_SATUR_CLR_POS             (5U)
#define AON_GPADC1_POS_SATUR_CLR_LEN             (1U)
#define AON_GPADC1_POS_SATUR_CLR_MSK             (((1U << AON_GPADC1_POS_SATUR_CLR_LEN) - 1) << AON_GPADC1_POS_SATUR_CLR_POS)
#define AON_GPADC1_POS_SATUR_CLR_UMSK            (~(((1U << AON_GPADC1_POS_SATUR_CLR_LEN) - 1) << AON_GPADC1_POS_SATUR_CLR_POS))
#define AON_GPADC1_NEG_SATUR_MASK                AON_GPADC1_NEG_SATUR_MASK
#define AON_GPADC1_NEG_SATUR_MASK_POS            (8U)
#define AON_GPADC1_NEG_SATUR_MASK_LEN            (1U)
#define AON_GPADC1_NEG_SATUR_MASK_MSK            (((1U << AON_GPADC1_NEG_SATUR_MASK_LEN) - 1) << AON_GPADC1_NEG_SATUR_MASK_POS)
#define AON_GPADC1_NEG_SATUR_MASK_UMSK           (~(((1U << AON_GPADC1_NEG_SATUR_MASK_LEN) - 1) << AON_GPADC1_NEG_SATUR_MASK_POS))
#define AON_GPADC1_POS_SATUR_MASK                AON_GPADC1_POS_SATUR_MASK
#define AON_GPADC1_POS_SATUR_MASK_POS            (9U)
#define AON_GPADC1_POS_SATUR_MASK_LEN            (1U)
#define AON_GPADC1_POS_SATUR_MASK_MSK            (((1U << AON_GPADC1_POS_SATUR_MASK_LEN) - 1) << AON_GPADC1_POS_SATUR_MASK_POS)
#define AON_GPADC1_POS_SATUR_MASK_UMSK           (~(((1U << AON_GPADC1_POS_SATUR_MASK_LEN) - 1) << AON_GPADC1_POS_SATUR_MASK_POS))

/* 0x94C : gpadc2_reg_config1 */
#define AON_GPADC2_REG_CONFIG1_OFFSET            (0x94C)
#define AON_GPADC2_VREF_EX_SEL                   AON_GPADC2_VREF_EX_SEL
#define AON_GPADC2_VREF_EX_SEL_POS               (0U)
#define AON_GPADC2_VREF_EX_SEL_LEN               (1U)
#define AON_GPADC2_VREF_EX_SEL_MSK               (((1U << AON_GPADC2_VREF_EX_SEL_LEN) - 1) << AON_GPADC2_VREF_EX_SEL_POS)
#define AON_GPADC2_VREF_EX_SEL_UMSK              (~(((1U << AON_GPADC2_VREF_EX_SEL_LEN) - 1) << AON_GPADC2_VREF_EX_SEL_POS))
#define AON_GPADC2_DIFF_MODE                     AON_GPADC2_DIFF_MODE
#define AON_GPADC2_DIFF_MODE_POS                 (1U)
#define AON_GPADC2_DIFF_MODE_LEN                 (1U)
#define AON_GPADC2_DIFF_MODE_MSK                 (((1U << AON_GPADC2_DIFF_MODE_LEN) - 1) << AON_GPADC2_DIFF_MODE_POS)
#define AON_GPADC2_DIFF_MODE_UMSK                (~(((1U << AON_GPADC2_DIFF_MODE_LEN) - 1) << AON_GPADC2_DIFF_MODE_POS))
#define AON_GPADC2_NEG_GND                       AON_GPADC2_NEG_GND
#define AON_GPADC2_NEG_GND_POS                   (2U)
#define AON_GPADC2_NEG_GND_LEN                   (1U)
#define AON_GPADC2_NEG_GND_MSK                   (((1U << AON_GPADC2_NEG_GND_LEN) - 1) << AON_GPADC2_NEG_GND_POS)
#define AON_GPADC2_NEG_GND_UMSK                  (~(((1U << AON_GPADC2_NEG_GND_LEN) - 1) << AON_GPADC2_NEG_GND_POS))
#define AON_GPADC2_PGA_VCMI_EN                   AON_GPADC2_PGA_VCMI_EN
#define AON_GPADC2_PGA_VCMI_EN_POS               (3U)
#define AON_GPADC2_PGA_VCMI_EN_LEN               (1U)
#define AON_GPADC2_PGA_VCMI_EN_MSK               (((1U << AON_GPADC2_PGA_VCMI_EN_LEN) - 1) << AON_GPADC2_PGA_VCMI_EN_POS)
#define AON_GPADC2_PGA_VCMI_EN_UMSK              (~(((1U << AON_GPADC2_PGA_VCMI_EN_LEN) - 1) << AON_GPADC2_PGA_VCMI_EN_POS))
#define AON_GPADC2_VREF_TRIM                     AON_GPADC2_VREF_TRIM
#define AON_GPADC2_VREF_TRIM_POS                 (4U)
#define AON_GPADC2_VREF_TRIM_LEN                 (6U)
#define AON_GPADC2_VREF_TRIM_MSK                 (((1U << AON_GPADC2_VREF_TRIM_LEN) - 1) << AON_GPADC2_VREF_TRIM_POS)
#define AON_GPADC2_VREF_TRIM_UMSK                (~(((1U << AON_GPADC2_VREF_TRIM_LEN) - 1) << AON_GPADC2_VREF_TRIM_POS))
#define AON_GPADC2_CHOP_MODE                     AON_GPADC2_CHOP_MODE
#define AON_GPADC2_CHOP_MODE_POS                 (12U)
#define AON_GPADC2_CHOP_MODE_LEN                 (2U)
#define AON_GPADC2_CHOP_MODE_MSK                 (((1U << AON_GPADC2_CHOP_MODE_LEN) - 1) << AON_GPADC2_CHOP_MODE_POS)
#define AON_GPADC2_CHOP_MODE_UMSK                (~(((1U << AON_GPADC2_CHOP_MODE_LEN) - 1) << AON_GPADC2_CHOP_MODE_POS))
#define AON_GPADC2_PGA_EN                        AON_GPADC2_PGA_EN
#define AON_GPADC2_PGA_EN_POS                    (16U)
#define AON_GPADC2_PGA_EN_LEN                    (1U)
#define AON_GPADC2_PGA_EN_MSK                    (((1U << AON_GPADC2_PGA_EN_LEN) - 1) << AON_GPADC2_PGA_EN_POS)
#define AON_GPADC2_PGA_EN_UMSK                   (~(((1U << AON_GPADC2_PGA_EN_LEN) - 1) << AON_GPADC2_PGA_EN_POS))
#define AON_GPADC2_PGA_GAIN                      AON_GPADC2_PGA_GAIN
#define AON_GPADC2_PGA_GAIN_POS                  (20U)
#define AON_GPADC2_PGA_GAIN_LEN                  (3U)
#define AON_GPADC2_PGA_GAIN_MSK                  (((1U << AON_GPADC2_PGA_GAIN_LEN) - 1) << AON_GPADC2_PGA_GAIN_POS)
#define AON_GPADC2_PGA_GAIN_UMSK                 (~(((1U << AON_GPADC2_PGA_GAIN_LEN) - 1) << AON_GPADC2_PGA_GAIN_POS))
#define AON_GPADC2_PGA_VCM                       AON_GPADC2_PGA_VCM
#define AON_GPADC2_PGA_VCM_POS                   (24U)
#define AON_GPADC2_PGA_VCM_LEN                   (2U)
#define AON_GPADC2_PGA_VCM_MSK                   (((1U << AON_GPADC2_PGA_VCM_LEN) - 1) << AON_GPADC2_PGA_VCM_POS)
#define AON_GPADC2_PGA_VCM_UMSK                  (~(((1U << AON_GPADC2_PGA_VCM_LEN) - 1) << AON_GPADC2_PGA_VCM_POS))

/* 0x950 : gpadc2_reg_config2 */
#define AON_GPADC2_REG_CONFIG2_OFFSET            (0x950)
#define AON_GPADC2_GLOBAL_EN                     AON_GPADC2_GLOBAL_EN
#define AON_GPADC2_GLOBAL_EN_POS                 (0U)
#define AON_GPADC2_GLOBAL_EN_LEN                 (1U)
#define AON_GPADC2_GLOBAL_EN_MSK                 (((1U << AON_GPADC2_GLOBAL_EN_LEN) - 1) << AON_GPADC2_GLOBAL_EN_POS)
#define AON_GPADC2_GLOBAL_EN_UMSK                (~(((1U << AON_GPADC2_GLOBAL_EN_LEN) - 1) << AON_GPADC2_GLOBAL_EN_POS))
#define AON_GPADC2_CONV_START                    AON_GPADC2_CONV_START
#define AON_GPADC2_CONV_START_POS                (1U)
#define AON_GPADC2_CONV_START_LEN                (1U)
#define AON_GPADC2_CONV_START_MSK                (((1U << AON_GPADC2_CONV_START_LEN) - 1) << AON_GPADC2_CONV_START_POS)
#define AON_GPADC2_CONV_START_UMSK               (~(((1U << AON_GPADC2_CONV_START_LEN) - 1) << AON_GPADC2_CONV_START_POS))
#define AON_GPADC2_SOFT_RST                      AON_GPADC2_SOFT_RST
#define AON_GPADC2_SOFT_RST_POS                  (2U)
#define AON_GPADC2_SOFT_RST_LEN                  (1U)
#define AON_GPADC2_SOFT_RST_MSK                  (((1U << AON_GPADC2_SOFT_RST_LEN) - 1) << AON_GPADC2_SOFT_RST_POS)
#define AON_GPADC2_SOFT_RST_UMSK                 (~(((1U << AON_GPADC2_SOFT_RST_LEN) - 1) << AON_GPADC2_SOFT_RST_POS))
#define AON_GPADC2_DWA_EN                        AON_GPADC2_DWA_EN
#define AON_GPADC2_DWA_EN_POS                    (4U)
#define AON_GPADC2_DWA_EN_LEN                    (1U)
#define AON_GPADC2_DWA_EN_MSK                    (((1U << AON_GPADC2_DWA_EN_LEN) - 1) << AON_GPADC2_DWA_EN_POS)
#define AON_GPADC2_DWA_EN_UMSK                   (~(((1U << AON_GPADC2_DWA_EN_LEN) - 1) << AON_GPADC2_DWA_EN_POS))
#define AON_GPADC2_DITHER_EN                     AON_GPADC2_DITHER_EN
#define AON_GPADC2_DITHER_EN_POS                 (8U)
#define AON_GPADC2_DITHER_EN_LEN                 (1U)
#define AON_GPADC2_DITHER_EN_MSK                 (((1U << AON_GPADC2_DITHER_EN_LEN) - 1) << AON_GPADC2_DITHER_EN_POS)
#define AON_GPADC2_DITHER_EN_UMSK                (~(((1U << AON_GPADC2_DITHER_EN_LEN) - 1) << AON_GPADC2_DITHER_EN_POS))
#define AON_GPADC2_DLY_SEL                       AON_GPADC2_DLY_SEL
#define AON_GPADC2_DLY_SEL_POS                   (12U)
#define AON_GPADC2_DLY_SEL_LEN                   (3U)
#define AON_GPADC2_DLY_SEL_MSK                   (((1U << AON_GPADC2_DLY_SEL_LEN) - 1) << AON_GPADC2_DLY_SEL_POS)
#define AON_GPADC2_DLY_SEL_UMSK                  (~(((1U << AON_GPADC2_DLY_SEL_LEN) - 1) << AON_GPADC2_DLY_SEL_POS))
#define AON_GPADC2_EXT_POS_SEL                   AON_GPADC2_EXT_POS_SEL
#define AON_GPADC2_EXT_POS_SEL_POS               (16U)
#define AON_GPADC2_EXT_POS_SEL_LEN               (4U)
#define AON_GPADC2_EXT_POS_SEL_MSK               (((1U << AON_GPADC2_EXT_POS_SEL_LEN) - 1) << AON_GPADC2_EXT_POS_SEL_POS)
#define AON_GPADC2_EXT_POS_SEL_UMSK              (~(((1U << AON_GPADC2_EXT_POS_SEL_LEN) - 1) << AON_GPADC2_EXT_POS_SEL_POS))
#define AON_GPADC2_EXT_NEG_SEL                   AON_GPADC2_EXT_NEG_SEL
#define AON_GPADC2_EXT_NEG_SEL_POS               (20U)
#define AON_GPADC2_EXT_NEG_SEL_LEN               (4U)
#define AON_GPADC2_EXT_NEG_SEL_MSK               (((1U << AON_GPADC2_EXT_NEG_SEL_LEN) - 1) << AON_GPADC2_EXT_NEG_SEL_POS)
#define AON_GPADC2_EXT_NEG_SEL_UMSK              (~(((1U << AON_GPADC2_EXT_NEG_SEL_LEN) - 1) << AON_GPADC2_EXT_NEG_SEL_POS))

/* 0x954 : gpadc2_reg_ctrl_0 */
#define AON_GPADC2_REG_CTRL_0_OFFSET             (0x954)
#define AON_GPADC2_CONT_CONV_EN                  AON_GPADC2_CONT_CONV_EN
#define AON_GPADC2_CONT_CONV_EN_POS              (0U)
#define AON_GPADC2_CONT_CONV_EN_LEN              (1U)
#define AON_GPADC2_CONT_CONV_EN_MSK              (((1U << AON_GPADC2_CONT_CONV_EN_LEN) - 1) << AON_GPADC2_CONT_CONV_EN_POS)
#define AON_GPADC2_CONT_CONV_EN_UMSK             (~(((1U << AON_GPADC2_CONT_CONV_EN_LEN) - 1) << AON_GPADC2_CONT_CONV_EN_POS))
#define AON_GPADC2_RES_SEL                       AON_GPADC2_RES_SEL
#define AON_GPADC2_RES_SEL_POS                   (1U)
#define AON_GPADC2_RES_SEL_LEN                   (3U)
#define AON_GPADC2_RES_SEL_MSK                   (((1U << AON_GPADC2_RES_SEL_LEN) - 1) << AON_GPADC2_RES_SEL_POS)
#define AON_GPADC2_RES_SEL_UMSK                  (~(((1U << AON_GPADC2_RES_SEL_LEN) - 1) << AON_GPADC2_RES_SEL_POS))
#define AON_GPADC2_CONV_DELAY                    AON_GPADC2_CONV_DELAY
#define AON_GPADC2_CONV_DELAY_POS                (4U)
#define AON_GPADC2_CONV_DELAY_LEN                (4U)
#define AON_GPADC2_CONV_DELAY_MSK                (((1U << AON_GPADC2_CONV_DELAY_LEN) - 1) << AON_GPADC2_CONV_DELAY_POS)
#define AON_GPADC2_CONV_DELAY_UMSK               (~(((1U << AON_GPADC2_CONV_DELAY_LEN) - 1) << AON_GPADC2_CONV_DELAY_POS))
#define AON_GPADC2_IJCT_START                    AON_GPADC2_IJCT_START
#define AON_GPADC2_IJCT_START_POS                (8U)
#define AON_GPADC2_IJCT_START_LEN                (1U)
#define AON_GPADC2_IJCT_START_MSK                (((1U << AON_GPADC2_IJCT_START_LEN) - 1) << AON_GPADC2_IJCT_START_POS)
#define AON_GPADC2_IJCT_START_UMSK               (~(((1U << AON_GPADC2_IJCT_START_LEN) - 1) << AON_GPADC2_IJCT_START_POS))
#define AON_GPADC2_IJCT_LENGTH                   AON_GPADC2_IJCT_LENGTH
#define AON_GPADC2_IJCT_LENGTH_POS               (9U)
#define AON_GPADC2_IJCT_LENGTH_LEN               (2U)
#define AON_GPADC2_IJCT_LENGTH_MSK               (((1U << AON_GPADC2_IJCT_LENGTH_LEN) - 1) << AON_GPADC2_IJCT_LENGTH_POS)
#define AON_GPADC2_IJCT_LENGTH_UMSK              (~(((1U << AON_GPADC2_IJCT_LENGTH_LEN) - 1) << AON_GPADC2_IJCT_LENGTH_POS))
#define AON_GPADC2_PWM_TRG_EN                    AON_GPADC2_PWM_TRG_EN
#define AON_GPADC2_PWM_TRG_EN_POS                (12U)
#define AON_GPADC2_PWM_TRG_EN_LEN                (1U)
#define AON_GPADC2_PWM_TRG_EN_MSK                (((1U << AON_GPADC2_PWM_TRG_EN_LEN) - 1) << AON_GPADC2_PWM_TRG_EN_POS)
#define AON_GPADC2_PWM_TRG_EN_UMSK               (~(((1U << AON_GPADC2_PWM_TRG_EN_LEN) - 1) << AON_GPADC2_PWM_TRG_EN_POS))
#define AON_GPADC2_SCAN_LENGTH                   AON_GPADC2_SCAN_LENGTH
#define AON_GPADC2_SCAN_LENGTH_POS               (16U)
#define AON_GPADC2_SCAN_LENGTH_LEN               (4U)
#define AON_GPADC2_SCAN_LENGTH_MSK               (((1U << AON_GPADC2_SCAN_LENGTH_LEN) - 1) << AON_GPADC2_SCAN_LENGTH_POS)
#define AON_GPADC2_SCAN_LENGTH_UMSK              (~(((1U << AON_GPADC2_SCAN_LENGTH_LEN) - 1) << AON_GPADC2_SCAN_LENGTH_POS))
#define AON_GPADC2_SCAN_EN                       AON_GPADC2_SCAN_EN
#define AON_GPADC2_SCAN_EN_POS                   (20U)
#define AON_GPADC2_SCAN_EN_LEN                   (1U)
#define AON_GPADC2_SCAN_EN_MSK                   (((1U << AON_GPADC2_SCAN_EN_LEN) - 1) << AON_GPADC2_SCAN_EN_POS)
#define AON_GPADC2_SCAN_EN_UMSK                  (~(((1U << AON_GPADC2_SCAN_EN_LEN) - 1) << AON_GPADC2_SCAN_EN_POS))

/* 0x958 : gpadc2_reg_os_cal_data */
#define AON_GPADC2_REG_OS_CAL_DATA_OFFSET        (0x958)
#define AON_GPADC2_OS_CAL_DATA                   AON_GPADC2_OS_CAL_DATA
#define AON_GPADC2_OS_CAL_DATA_POS               (0U)
#define AON_GPADC2_OS_CAL_DATA_LEN               (16U)
#define AON_GPADC2_OS_CAL_DATA_MSK               (((1U << AON_GPADC2_OS_CAL_DATA_LEN) - 1) << AON_GPADC2_OS_CAL_DATA_POS)
#define AON_GPADC2_OS_CAL_DATA_UMSK              (~(((1U << AON_GPADC2_OS_CAL_DATA_LEN) - 1) << AON_GPADC2_OS_CAL_DATA_POS))

/* 0x95C : gpadc2_reg_scan_0 */
#define AON_GPADC2_REG_SCAN_0_OFFSET             (0x95C)
#define AON_GPADC2_SCAN_POS_0                    AON_GPADC2_SCAN_POS_0
#define AON_GPADC2_SCAN_POS_0_POS                (0U)
#define AON_GPADC2_SCAN_POS_0_LEN                (4U)
#define AON_GPADC2_SCAN_POS_0_MSK                (((1U << AON_GPADC2_SCAN_POS_0_LEN) - 1) << AON_GPADC2_SCAN_POS_0_POS)
#define AON_GPADC2_SCAN_POS_0_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_0_LEN) - 1) << AON_GPADC2_SCAN_POS_0_POS))
#define AON_GPADC2_SCAN_POS_1                    AON_GPADC2_SCAN_POS_1
#define AON_GPADC2_SCAN_POS_1_POS                (4U)
#define AON_GPADC2_SCAN_POS_1_LEN                (4U)
#define AON_GPADC2_SCAN_POS_1_MSK                (((1U << AON_GPADC2_SCAN_POS_1_LEN) - 1) << AON_GPADC2_SCAN_POS_1_POS)
#define AON_GPADC2_SCAN_POS_1_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_1_LEN) - 1) << AON_GPADC2_SCAN_POS_1_POS))
#define AON_GPADC2_SCAN_POS_2                    AON_GPADC2_SCAN_POS_2
#define AON_GPADC2_SCAN_POS_2_POS                (8U)
#define AON_GPADC2_SCAN_POS_2_LEN                (4U)
#define AON_GPADC2_SCAN_POS_2_MSK                (((1U << AON_GPADC2_SCAN_POS_2_LEN) - 1) << AON_GPADC2_SCAN_POS_2_POS)
#define AON_GPADC2_SCAN_POS_2_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_2_LEN) - 1) << AON_GPADC2_SCAN_POS_2_POS))
#define AON_GPADC2_SCAN_POS_3                    AON_GPADC2_SCAN_POS_3
#define AON_GPADC2_SCAN_POS_3_POS                (12U)
#define AON_GPADC2_SCAN_POS_3_LEN                (4U)
#define AON_GPADC2_SCAN_POS_3_MSK                (((1U << AON_GPADC2_SCAN_POS_3_LEN) - 1) << AON_GPADC2_SCAN_POS_3_POS)
#define AON_GPADC2_SCAN_POS_3_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_3_LEN) - 1) << AON_GPADC2_SCAN_POS_3_POS))
#define AON_GPADC2_SCAN_POS_4                    AON_GPADC2_SCAN_POS_4
#define AON_GPADC2_SCAN_POS_4_POS                (16U)
#define AON_GPADC2_SCAN_POS_4_LEN                (4U)
#define AON_GPADC2_SCAN_POS_4_MSK                (((1U << AON_GPADC2_SCAN_POS_4_LEN) - 1) << AON_GPADC2_SCAN_POS_4_POS)
#define AON_GPADC2_SCAN_POS_4_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_4_LEN) - 1) << AON_GPADC2_SCAN_POS_4_POS))
#define AON_GPADC2_SCAN_POS_5                    AON_GPADC2_SCAN_POS_5
#define AON_GPADC2_SCAN_POS_5_POS                (20U)
#define AON_GPADC2_SCAN_POS_5_LEN                (4U)
#define AON_GPADC2_SCAN_POS_5_MSK                (((1U << AON_GPADC2_SCAN_POS_5_LEN) - 1) << AON_GPADC2_SCAN_POS_5_POS)
#define AON_GPADC2_SCAN_POS_5_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_5_LEN) - 1) << AON_GPADC2_SCAN_POS_5_POS))
#define AON_GPADC2_SCAN_POS_6                    AON_GPADC2_SCAN_POS_6
#define AON_GPADC2_SCAN_POS_6_POS                (24U)
#define AON_GPADC2_SCAN_POS_6_LEN                (4U)
#define AON_GPADC2_SCAN_POS_6_MSK                (((1U << AON_GPADC2_SCAN_POS_6_LEN) - 1) << AON_GPADC2_SCAN_POS_6_POS)
#define AON_GPADC2_SCAN_POS_6_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_6_LEN) - 1) << AON_GPADC2_SCAN_POS_6_POS))
#define AON_GPADC2_SCAN_POS_7                    AON_GPADC2_SCAN_POS_7
#define AON_GPADC2_SCAN_POS_7_POS                (28U)
#define AON_GPADC2_SCAN_POS_7_LEN                (4U)
#define AON_GPADC2_SCAN_POS_7_MSK                (((1U << AON_GPADC2_SCAN_POS_7_LEN) - 1) << AON_GPADC2_SCAN_POS_7_POS)
#define AON_GPADC2_SCAN_POS_7_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_7_LEN) - 1) << AON_GPADC2_SCAN_POS_7_POS))

/* 0x960 : gpadc2_reg_scan_1 */
#define AON_GPADC2_REG_SCAN_1_OFFSET             (0x960)
#define AON_GPADC2_SCAN_POS_8                    AON_GPADC2_SCAN_POS_8
#define AON_GPADC2_SCAN_POS_8_POS                (0U)
#define AON_GPADC2_SCAN_POS_8_LEN                (4U)
#define AON_GPADC2_SCAN_POS_8_MSK                (((1U << AON_GPADC2_SCAN_POS_8_LEN) - 1) << AON_GPADC2_SCAN_POS_8_POS)
#define AON_GPADC2_SCAN_POS_8_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_8_LEN) - 1) << AON_GPADC2_SCAN_POS_8_POS))
#define AON_GPADC2_SCAN_POS_9                    AON_GPADC2_SCAN_POS_9
#define AON_GPADC2_SCAN_POS_9_POS                (4U)
#define AON_GPADC2_SCAN_POS_9_LEN                (4U)
#define AON_GPADC2_SCAN_POS_9_MSK                (((1U << AON_GPADC2_SCAN_POS_9_LEN) - 1) << AON_GPADC2_SCAN_POS_9_POS)
#define AON_GPADC2_SCAN_POS_9_UMSK               (~(((1U << AON_GPADC2_SCAN_POS_9_LEN) - 1) << AON_GPADC2_SCAN_POS_9_POS))
#define AON_GPADC2_SCAN_POS_10                   AON_GPADC2_SCAN_POS_10
#define AON_GPADC2_SCAN_POS_10_POS               (8U)
#define AON_GPADC2_SCAN_POS_10_LEN               (4U)
#define AON_GPADC2_SCAN_POS_10_MSK               (((1U << AON_GPADC2_SCAN_POS_10_LEN) - 1) << AON_GPADC2_SCAN_POS_10_POS)
#define AON_GPADC2_SCAN_POS_10_UMSK              (~(((1U << AON_GPADC2_SCAN_POS_10_LEN) - 1) << AON_GPADC2_SCAN_POS_10_POS))
#define AON_GPADC2_SCAN_POS_11                   AON_GPADC2_SCAN_POS_11
#define AON_GPADC2_SCAN_POS_11_POS               (12U)
#define AON_GPADC2_SCAN_POS_11_LEN               (4U)
#define AON_GPADC2_SCAN_POS_11_MSK               (((1U << AON_GPADC2_SCAN_POS_11_LEN) - 1) << AON_GPADC2_SCAN_POS_11_POS)
#define AON_GPADC2_SCAN_POS_11_UMSK              (~(((1U << AON_GPADC2_SCAN_POS_11_LEN) - 1) << AON_GPADC2_SCAN_POS_11_POS))
#define AON_GPADC2_SCAN_POS_12                   AON_GPADC2_SCAN_POS_12
#define AON_GPADC2_SCAN_POS_12_POS               (16U)
#define AON_GPADC2_SCAN_POS_12_LEN               (4U)
#define AON_GPADC2_SCAN_POS_12_MSK               (((1U << AON_GPADC2_SCAN_POS_12_LEN) - 1) << AON_GPADC2_SCAN_POS_12_POS)
#define AON_GPADC2_SCAN_POS_12_UMSK              (~(((1U << AON_GPADC2_SCAN_POS_12_LEN) - 1) << AON_GPADC2_SCAN_POS_12_POS))
#define AON_GPADC2_SCAN_POS_13                   AON_GPADC2_SCAN_POS_13
#define AON_GPADC2_SCAN_POS_13_POS               (20U)
#define AON_GPADC2_SCAN_POS_13_LEN               (4U)
#define AON_GPADC2_SCAN_POS_13_MSK               (((1U << AON_GPADC2_SCAN_POS_13_LEN) - 1) << AON_GPADC2_SCAN_POS_13_POS)
#define AON_GPADC2_SCAN_POS_13_UMSK              (~(((1U << AON_GPADC2_SCAN_POS_13_LEN) - 1) << AON_GPADC2_SCAN_POS_13_POS))
#define AON_GPADC2_SCAN_POS_14                   AON_GPADC2_SCAN_POS_14
#define AON_GPADC2_SCAN_POS_14_POS               (24U)
#define AON_GPADC2_SCAN_POS_14_LEN               (4U)
#define AON_GPADC2_SCAN_POS_14_MSK               (((1U << AON_GPADC2_SCAN_POS_14_LEN) - 1) << AON_GPADC2_SCAN_POS_14_POS)
#define AON_GPADC2_SCAN_POS_14_UMSK              (~(((1U << AON_GPADC2_SCAN_POS_14_LEN) - 1) << AON_GPADC2_SCAN_POS_14_POS))
#define AON_GPADC2_SCAN_POS_15                   AON_GPADC2_SCAN_POS_15
#define AON_GPADC2_SCAN_POS_15_POS               (28U)
#define AON_GPADC2_SCAN_POS_15_LEN               (4U)
#define AON_GPADC2_SCAN_POS_15_MSK               (((1U << AON_GPADC2_SCAN_POS_15_LEN) - 1) << AON_GPADC2_SCAN_POS_15_POS)
#define AON_GPADC2_SCAN_POS_15_UMSK              (~(((1U << AON_GPADC2_SCAN_POS_15_LEN) - 1) << AON_GPADC2_SCAN_POS_15_POS))

/* 0x964 : gpadc2_reg_scan_2 */
#define AON_GPADC2_REG_SCAN_2_OFFSET             (0x964)
#define AON_GPADC2_SCAN_NEG_0                    AON_GPADC2_SCAN_NEG_0
#define AON_GPADC2_SCAN_NEG_0_POS                (0U)
#define AON_GPADC2_SCAN_NEG_0_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_0_MSK                (((1U << AON_GPADC2_SCAN_NEG_0_LEN) - 1) << AON_GPADC2_SCAN_NEG_0_POS)
#define AON_GPADC2_SCAN_NEG_0_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_0_LEN) - 1) << AON_GPADC2_SCAN_NEG_0_POS))
#define AON_GPADC2_SCAN_NEG_1                    AON_GPADC2_SCAN_NEG_1
#define AON_GPADC2_SCAN_NEG_1_POS                (4U)
#define AON_GPADC2_SCAN_NEG_1_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_1_MSK                (((1U << AON_GPADC2_SCAN_NEG_1_LEN) - 1) << AON_GPADC2_SCAN_NEG_1_POS)
#define AON_GPADC2_SCAN_NEG_1_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_1_LEN) - 1) << AON_GPADC2_SCAN_NEG_1_POS))
#define AON_GPADC2_SCAN_NEG_2                    AON_GPADC2_SCAN_NEG_2
#define AON_GPADC2_SCAN_NEG_2_POS                (8U)
#define AON_GPADC2_SCAN_NEG_2_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_2_MSK                (((1U << AON_GPADC2_SCAN_NEG_2_LEN) - 1) << AON_GPADC2_SCAN_NEG_2_POS)
#define AON_GPADC2_SCAN_NEG_2_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_2_LEN) - 1) << AON_GPADC2_SCAN_NEG_2_POS))
#define AON_GPADC2_SCAN_NEG_3                    AON_GPADC2_SCAN_NEG_3
#define AON_GPADC2_SCAN_NEG_3_POS                (12U)
#define AON_GPADC2_SCAN_NEG_3_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_3_MSK                (((1U << AON_GPADC2_SCAN_NEG_3_LEN) - 1) << AON_GPADC2_SCAN_NEG_3_POS)
#define AON_GPADC2_SCAN_NEG_3_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_3_LEN) - 1) << AON_GPADC2_SCAN_NEG_3_POS))
#define AON_GPADC2_SCAN_NEG_4                    AON_GPADC2_SCAN_NEG_4
#define AON_GPADC2_SCAN_NEG_4_POS                (16U)
#define AON_GPADC2_SCAN_NEG_4_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_4_MSK                (((1U << AON_GPADC2_SCAN_NEG_4_LEN) - 1) << AON_GPADC2_SCAN_NEG_4_POS)
#define AON_GPADC2_SCAN_NEG_4_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_4_LEN) - 1) << AON_GPADC2_SCAN_NEG_4_POS))
#define AON_GPADC2_SCAN_NEG_5                    AON_GPADC2_SCAN_NEG_5
#define AON_GPADC2_SCAN_NEG_5_POS                (20U)
#define AON_GPADC2_SCAN_NEG_5_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_5_MSK                (((1U << AON_GPADC2_SCAN_NEG_5_LEN) - 1) << AON_GPADC2_SCAN_NEG_5_POS)
#define AON_GPADC2_SCAN_NEG_5_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_5_LEN) - 1) << AON_GPADC2_SCAN_NEG_5_POS))
#define AON_GPADC2_SCAN_NEG_6                    AON_GPADC2_SCAN_NEG_6
#define AON_GPADC2_SCAN_NEG_6_POS                (24U)
#define AON_GPADC2_SCAN_NEG_6_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_6_MSK                (((1U << AON_GPADC2_SCAN_NEG_6_LEN) - 1) << AON_GPADC2_SCAN_NEG_6_POS)
#define AON_GPADC2_SCAN_NEG_6_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_6_LEN) - 1) << AON_GPADC2_SCAN_NEG_6_POS))
#define AON_GPADC2_SCAN_NEG_7                    AON_GPADC2_SCAN_NEG_7
#define AON_GPADC2_SCAN_NEG_7_POS                (28U)
#define AON_GPADC2_SCAN_NEG_7_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_7_MSK                (((1U << AON_GPADC2_SCAN_NEG_7_LEN) - 1) << AON_GPADC2_SCAN_NEG_7_POS)
#define AON_GPADC2_SCAN_NEG_7_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_7_LEN) - 1) << AON_GPADC2_SCAN_NEG_7_POS))

/* 0x968 : gpadc2_reg_scan_3 */
#define AON_GPADC2_REG_SCAN_3_OFFSET             (0x968)
#define AON_GPADC2_SCAN_NEG_8                    AON_GPADC2_SCAN_NEG_8
#define AON_GPADC2_SCAN_NEG_8_POS                (0U)
#define AON_GPADC2_SCAN_NEG_8_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_8_MSK                (((1U << AON_GPADC2_SCAN_NEG_8_LEN) - 1) << AON_GPADC2_SCAN_NEG_8_POS)
#define AON_GPADC2_SCAN_NEG_8_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_8_LEN) - 1) << AON_GPADC2_SCAN_NEG_8_POS))
#define AON_GPADC2_SCAN_NEG_9                    AON_GPADC2_SCAN_NEG_9
#define AON_GPADC2_SCAN_NEG_9_POS                (4U)
#define AON_GPADC2_SCAN_NEG_9_LEN                (4U)
#define AON_GPADC2_SCAN_NEG_9_MSK                (((1U << AON_GPADC2_SCAN_NEG_9_LEN) - 1) << AON_GPADC2_SCAN_NEG_9_POS)
#define AON_GPADC2_SCAN_NEG_9_UMSK               (~(((1U << AON_GPADC2_SCAN_NEG_9_LEN) - 1) << AON_GPADC2_SCAN_NEG_9_POS))
#define AON_GPADC2_SCAN_NEG_10                   AON_GPADC2_SCAN_NEG_10
#define AON_GPADC2_SCAN_NEG_10_POS               (8U)
#define AON_GPADC2_SCAN_NEG_10_LEN               (4U)
#define AON_GPADC2_SCAN_NEG_10_MSK               (((1U << AON_GPADC2_SCAN_NEG_10_LEN) - 1) << AON_GPADC2_SCAN_NEG_10_POS)
#define AON_GPADC2_SCAN_NEG_10_UMSK              (~(((1U << AON_GPADC2_SCAN_NEG_10_LEN) - 1) << AON_GPADC2_SCAN_NEG_10_POS))
#define AON_GPADC2_SCAN_NEG_11                   AON_GPADC2_SCAN_NEG_11
#define AON_GPADC2_SCAN_NEG_11_POS               (12U)
#define AON_GPADC2_SCAN_NEG_11_LEN               (4U)
#define AON_GPADC2_SCAN_NEG_11_MSK               (((1U << AON_GPADC2_SCAN_NEG_11_LEN) - 1) << AON_GPADC2_SCAN_NEG_11_POS)
#define AON_GPADC2_SCAN_NEG_11_UMSK              (~(((1U << AON_GPADC2_SCAN_NEG_11_LEN) - 1) << AON_GPADC2_SCAN_NEG_11_POS))
#define AON_GPADC2_SCAN_NEG_12                   AON_GPADC2_SCAN_NEG_12
#define AON_GPADC2_SCAN_NEG_12_POS               (16U)
#define AON_GPADC2_SCAN_NEG_12_LEN               (4U)
#define AON_GPADC2_SCAN_NEG_12_MSK               (((1U << AON_GPADC2_SCAN_NEG_12_LEN) - 1) << AON_GPADC2_SCAN_NEG_12_POS)
#define AON_GPADC2_SCAN_NEG_12_UMSK              (~(((1U << AON_GPADC2_SCAN_NEG_12_LEN) - 1) << AON_GPADC2_SCAN_NEG_12_POS))
#define AON_GPADC2_SCAN_NEG_13                   AON_GPADC2_SCAN_NEG_13
#define AON_GPADC2_SCAN_NEG_13_POS               (20U)
#define AON_GPADC2_SCAN_NEG_13_LEN               (4U)
#define AON_GPADC2_SCAN_NEG_13_MSK               (((1U << AON_GPADC2_SCAN_NEG_13_LEN) - 1) << AON_GPADC2_SCAN_NEG_13_POS)
#define AON_GPADC2_SCAN_NEG_13_UMSK              (~(((1U << AON_GPADC2_SCAN_NEG_13_LEN) - 1) << AON_GPADC2_SCAN_NEG_13_POS))
#define AON_GPADC2_SCAN_NEG_14                   AON_GPADC2_SCAN_NEG_14
#define AON_GPADC2_SCAN_NEG_14_POS               (24U)
#define AON_GPADC2_SCAN_NEG_14_LEN               (4U)
#define AON_GPADC2_SCAN_NEG_14_MSK               (((1U << AON_GPADC2_SCAN_NEG_14_LEN) - 1) << AON_GPADC2_SCAN_NEG_14_POS)
#define AON_GPADC2_SCAN_NEG_14_UMSK              (~(((1U << AON_GPADC2_SCAN_NEG_14_LEN) - 1) << AON_GPADC2_SCAN_NEG_14_POS))
#define AON_GPADC2_SCAN_NEG_15                   AON_GPADC2_SCAN_NEG_15
#define AON_GPADC2_SCAN_NEG_15_POS               (28U)
#define AON_GPADC2_SCAN_NEG_15_LEN               (4U)
#define AON_GPADC2_SCAN_NEG_15_MSK               (((1U << AON_GPADC2_SCAN_NEG_15_LEN) - 1) << AON_GPADC2_SCAN_NEG_15_POS)
#define AON_GPADC2_SCAN_NEG_15_UMSK              (~(((1U << AON_GPADC2_SCAN_NEG_15_LEN) - 1) << AON_GPADC2_SCAN_NEG_15_POS))

/* 0x96C : gpadc2_reg_ijct_ext_0 */
#define AON_GPADC2_REG_IJCT_EXT_0_OFFSET         (0x96C)
#define AON_GPADC2_IJCT_EXT_POS_0                AON_GPADC2_IJCT_EXT_POS_0
#define AON_GPADC2_IJCT_EXT_POS_0_POS            (0U)
#define AON_GPADC2_IJCT_EXT_POS_0_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_POS_0_MSK            (((1U << AON_GPADC2_IJCT_EXT_POS_0_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_0_POS)
#define AON_GPADC2_IJCT_EXT_POS_0_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_POS_0_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_0_POS))
#define AON_GPADC2_IJCT_EXT_POS_1                AON_GPADC2_IJCT_EXT_POS_1
#define AON_GPADC2_IJCT_EXT_POS_1_POS            (4U)
#define AON_GPADC2_IJCT_EXT_POS_1_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_POS_1_MSK            (((1U << AON_GPADC2_IJCT_EXT_POS_1_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_1_POS)
#define AON_GPADC2_IJCT_EXT_POS_1_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_POS_1_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_1_POS))
#define AON_GPADC2_IJCT_EXT_POS_2                AON_GPADC2_IJCT_EXT_POS_2
#define AON_GPADC2_IJCT_EXT_POS_2_POS            (8U)
#define AON_GPADC2_IJCT_EXT_POS_2_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_POS_2_MSK            (((1U << AON_GPADC2_IJCT_EXT_POS_2_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_2_POS)
#define AON_GPADC2_IJCT_EXT_POS_2_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_POS_2_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_2_POS))
#define AON_GPADC2_IJCT_EXT_POS_3                AON_GPADC2_IJCT_EXT_POS_3
#define AON_GPADC2_IJCT_EXT_POS_3_POS            (12U)
#define AON_GPADC2_IJCT_EXT_POS_3_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_POS_3_MSK            (((1U << AON_GPADC2_IJCT_EXT_POS_3_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_3_POS)
#define AON_GPADC2_IJCT_EXT_POS_3_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_POS_3_LEN) - 1) << AON_GPADC2_IJCT_EXT_POS_3_POS))
#define AON_GPADC2_IJCT_EXT_NEG_0                AON_GPADC2_IJCT_EXT_NEG_0
#define AON_GPADC2_IJCT_EXT_NEG_0_POS            (16U)
#define AON_GPADC2_IJCT_EXT_NEG_0_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_NEG_0_MSK            (((1U << AON_GPADC2_IJCT_EXT_NEG_0_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_0_POS)
#define AON_GPADC2_IJCT_EXT_NEG_0_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_NEG_0_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_0_POS))
#define AON_GPADC2_IJCT_EXT_NEG_1                AON_GPADC2_IJCT_EXT_NEG_1
#define AON_GPADC2_IJCT_EXT_NEG_1_POS            (20U)
#define AON_GPADC2_IJCT_EXT_NEG_1_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_NEG_1_MSK            (((1U << AON_GPADC2_IJCT_EXT_NEG_1_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_1_POS)
#define AON_GPADC2_IJCT_EXT_NEG_1_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_NEG_1_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_1_POS))
#define AON_GPADC2_IJCT_EXT_NEG_2                AON_GPADC2_IJCT_EXT_NEG_2
#define AON_GPADC2_IJCT_EXT_NEG_2_POS            (24U)
#define AON_GPADC2_IJCT_EXT_NEG_2_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_NEG_2_MSK            (((1U << AON_GPADC2_IJCT_EXT_NEG_2_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_2_POS)
#define AON_GPADC2_IJCT_EXT_NEG_2_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_NEG_2_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_2_POS))
#define AON_GPADC2_IJCT_EXT_NEG_3                AON_GPADC2_IJCT_EXT_NEG_3
#define AON_GPADC2_IJCT_EXT_NEG_3_POS            (28U)
#define AON_GPADC2_IJCT_EXT_NEG_3_LEN            (4U)
#define AON_GPADC2_IJCT_EXT_NEG_3_MSK            (((1U << AON_GPADC2_IJCT_EXT_NEG_3_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_3_POS)
#define AON_GPADC2_IJCT_EXT_NEG_3_UMSK           (~(((1U << AON_GPADC2_IJCT_EXT_NEG_3_LEN) - 1) << AON_GPADC2_IJCT_EXT_NEG_3_POS))

/* 0x970 : gpadc2_reg_result */
#define AON_GPADC2_REG_RESULT_OFFSET             (0x970)
#define AON_GPADC2_DATA_OUT                      AON_GPADC2_DATA_OUT
#define AON_GPADC2_DATA_OUT_POS                  (0U)
#define AON_GPADC2_DATA_OUT_LEN                  (29U)
#define AON_GPADC2_DATA_OUT_MSK                  (((1U << AON_GPADC2_DATA_OUT_LEN) - 1) << AON_GPADC2_DATA_OUT_POS)
#define AON_GPADC2_DATA_OUT_UMSK                 (~(((1U << AON_GPADC2_DATA_OUT_LEN) - 1) << AON_GPADC2_DATA_OUT_POS))
#define AON_GPADC2_DATA_RDY                      AON_GPADC2_DATA_RDY
#define AON_GPADC2_DATA_RDY_POS                  (31U)
#define AON_GPADC2_DATA_RDY_LEN                  (1U)
#define AON_GPADC2_DATA_RDY_MSK                  (((1U << AON_GPADC2_DATA_RDY_LEN) - 1) << AON_GPADC2_DATA_RDY_POS)
#define AON_GPADC2_DATA_RDY_UMSK                 (~(((1U << AON_GPADC2_DATA_RDY_LEN) - 1) << AON_GPADC2_DATA_RDY_POS))

/* 0x974 : gpadc2_reg_raw_data */
#define AON_GPADC2_REG_RAW_DATA_OFFSET           (0x974)
#define AON_GPADC2_RAW_DATA                      AON_GPADC2_RAW_DATA
#define AON_GPADC2_RAW_DATA_POS                  (0U)
#define AON_GPADC2_RAW_DATA_LEN                  (12U)
#define AON_GPADC2_RAW_DATA_MSK                  (((1U << AON_GPADC2_RAW_DATA_LEN) - 1) << AON_GPADC2_RAW_DATA_POS)
#define AON_GPADC2_RAW_DATA_UMSK                 (~(((1U << AON_GPADC2_RAW_DATA_LEN) - 1) << AON_GPADC2_RAW_DATA_POS))
#define AON_GPADC2_RESERVED                      AON_GPADC2_RESERVED
#define AON_GPADC2_RESERVED_POS                  (16U)
#define AON_GPADC2_RESERVED_LEN                  (16U)
#define AON_GPADC2_RESERVED_MSK                  (((1U << AON_GPADC2_RESERVED_LEN) - 1) << AON_GPADC2_RESERVED_POS)
#define AON_GPADC2_RESERVED_UMSK                 (~(((1U << AON_GPADC2_RESERVED_LEN) - 1) << AON_GPADC2_RESERVED_POS))

/* 0x978 : gpadc2_reg_satur */
#define AON_GPADC2_REG_SATUR_OFFSET              (0x978)
#define AON_GPADC2_NEG_SATUR                     AON_GPADC2_NEG_SATUR
#define AON_GPADC2_NEG_SATUR_POS                 (0U)
#define AON_GPADC2_NEG_SATUR_LEN                 (1U)
#define AON_GPADC2_NEG_SATUR_MSK                 (((1U << AON_GPADC2_NEG_SATUR_LEN) - 1) << AON_GPADC2_NEG_SATUR_POS)
#define AON_GPADC2_NEG_SATUR_UMSK                (~(((1U << AON_GPADC2_NEG_SATUR_LEN) - 1) << AON_GPADC2_NEG_SATUR_POS))
#define AON_GPADC2_POS_SATUR                     AON_GPADC2_POS_SATUR
#define AON_GPADC2_POS_SATUR_POS                 (1U)
#define AON_GPADC2_POS_SATUR_LEN                 (1U)
#define AON_GPADC2_POS_SATUR_MSK                 (((1U << AON_GPADC2_POS_SATUR_LEN) - 1) << AON_GPADC2_POS_SATUR_POS)
#define AON_GPADC2_POS_SATUR_UMSK                (~(((1U << AON_GPADC2_POS_SATUR_LEN) - 1) << AON_GPADC2_POS_SATUR_POS))
#define AON_GPADC2_NEG_SATUR_CLR                 AON_GPADC2_NEG_SATUR_CLR
#define AON_GPADC2_NEG_SATUR_CLR_POS             (4U)
#define AON_GPADC2_NEG_SATUR_CLR_LEN             (1U)
#define AON_GPADC2_NEG_SATUR_CLR_MSK             (((1U << AON_GPADC2_NEG_SATUR_CLR_LEN) - 1) << AON_GPADC2_NEG_SATUR_CLR_POS)
#define AON_GPADC2_NEG_SATUR_CLR_UMSK            (~(((1U << AON_GPADC2_NEG_SATUR_CLR_LEN) - 1) << AON_GPADC2_NEG_SATUR_CLR_POS))
#define AON_GPADC2_POS_SATUR_CLR                 AON_GPADC2_POS_SATUR_CLR
#define AON_GPADC2_POS_SATUR_CLR_POS             (5U)
#define AON_GPADC2_POS_SATUR_CLR_LEN             (1U)
#define AON_GPADC2_POS_SATUR_CLR_MSK             (((1U << AON_GPADC2_POS_SATUR_CLR_LEN) - 1) << AON_GPADC2_POS_SATUR_CLR_POS)
#define AON_GPADC2_POS_SATUR_CLR_UMSK            (~(((1U << AON_GPADC2_POS_SATUR_CLR_LEN) - 1) << AON_GPADC2_POS_SATUR_CLR_POS))
#define AON_GPADC2_NEG_SATUR_MASK                AON_GPADC2_NEG_SATUR_MASK
#define AON_GPADC2_NEG_SATUR_MASK_POS            (8U)
#define AON_GPADC2_NEG_SATUR_MASK_LEN            (1U)
#define AON_GPADC2_NEG_SATUR_MASK_MSK            (((1U << AON_GPADC2_NEG_SATUR_MASK_LEN) - 1) << AON_GPADC2_NEG_SATUR_MASK_POS)
#define AON_GPADC2_NEG_SATUR_MASK_UMSK           (~(((1U << AON_GPADC2_NEG_SATUR_MASK_LEN) - 1) << AON_GPADC2_NEG_SATUR_MASK_POS))
#define AON_GPADC2_POS_SATUR_MASK                AON_GPADC2_POS_SATUR_MASK
#define AON_GPADC2_POS_SATUR_MASK_POS            (9U)
#define AON_GPADC2_POS_SATUR_MASK_LEN            (1U)
#define AON_GPADC2_POS_SATUR_MASK_MSK            (((1U << AON_GPADC2_POS_SATUR_MASK_LEN) - 1) << AON_GPADC2_POS_SATUR_MASK_POS)
#define AON_GPADC2_POS_SATUR_MASK_UMSK           (~(((1U << AON_GPADC2_POS_SATUR_MASK_LEN) - 1) << AON_GPADC2_POS_SATUR_MASK_POS))

/* 0x97C : gpadc_ten */
#define AON_GPADC_TEN_OFFSET                     (0x97C)
#define AON_TEN_GPADC0                           AON_TEN_GPADC0
#define AON_TEN_GPADC0_POS                       (0U)
#define AON_TEN_GPADC0_LEN                       (1U)
#define AON_TEN_GPADC0_MSK                       (((1U << AON_TEN_GPADC0_LEN) - 1) << AON_TEN_GPADC0_POS)
#define AON_TEN_GPADC0_UMSK                      (~(((1U << AON_TEN_GPADC0_LEN) - 1) << AON_TEN_GPADC0_POS))
#define AON_TEN_GPADC1                           AON_TEN_GPADC1
#define AON_TEN_GPADC1_POS                       (1U)
#define AON_TEN_GPADC1_LEN                       (1U)
#define AON_TEN_GPADC1_MSK                       (((1U << AON_TEN_GPADC1_LEN) - 1) << AON_TEN_GPADC1_POS)
#define AON_TEN_GPADC1_UMSK                      (~(((1U << AON_TEN_GPADC1_LEN) - 1) << AON_TEN_GPADC1_POS))
#define AON_TEN_GPADC2                           AON_TEN_GPADC2
#define AON_TEN_GPADC2_POS                       (2U)
#define AON_TEN_GPADC2_LEN                       (1U)
#define AON_TEN_GPADC2_MSK                       (((1U << AON_TEN_GPADC2_LEN) - 1) << AON_TEN_GPADC2_POS)
#define AON_TEN_GPADC2_UMSK                      (~(((1U << AON_TEN_GPADC2_LEN) - 1) << AON_TEN_GPADC2_POS))

/* 0x980 : hbncore_resv0 */
#define AON_HBNCORE_RESV0_OFFSET                 (0x980)
#define AON_HBNCORE_RESV0_DATA                   AON_HBNCORE_RESV0_DATA
#define AON_HBNCORE_RESV0_DATA_POS               (0U)
#define AON_HBNCORE_RESV0_DATA_LEN               (32U)
#define AON_HBNCORE_RESV0_DATA_MSK               (((1U << AON_HBNCORE_RESV0_DATA_LEN) - 1) << AON_HBNCORE_RESV0_DATA_POS)
#define AON_HBNCORE_RESV0_DATA_UMSK              (~(((1U << AON_HBNCORE_RESV0_DATA_LEN) - 1) << AON_HBNCORE_RESV0_DATA_POS))

/* 0x984 : hbncore_resv1 */
#define AON_HBNCORE_RESV1_OFFSET                 (0x984)
#define AON_HBNCORE_RESV1_DATA                   AON_HBNCORE_RESV1_DATA
#define AON_HBNCORE_RESV1_DATA_POS               (0U)
#define AON_HBNCORE_RESV1_DATA_LEN               (32U)
#define AON_HBNCORE_RESV1_DATA_MSK               (((1U << AON_HBNCORE_RESV1_DATA_LEN) - 1) << AON_HBNCORE_RESV1_DATA_POS)
#define AON_HBNCORE_RESV1_DATA_UMSK              (~(((1U << AON_HBNCORE_RESV1_DATA_LEN) - 1) << AON_HBNCORE_RESV1_DATA_POS))

/* 0x990 : rc32m_ctrl0_aon */
#define AON_RC32M_CTRL0_AON_OFFSET               (0x990)
#define AON_RC32M_CODE_FR_EXT                    AON_RC32M_CODE_FR_EXT
#define AON_RC32M_CODE_FR_EXT_POS                (22U)
#define AON_RC32M_CODE_FR_EXT_LEN                (8U)
#define AON_RC32M_CODE_FR_EXT_MSK                (((1U << AON_RC32M_CODE_FR_EXT_LEN) - 1) << AON_RC32M_CODE_FR_EXT_POS)
#define AON_RC32M_CODE_FR_EXT_UMSK               (~(((1U << AON_RC32M_CODE_FR_EXT_LEN) - 1) << AON_RC32M_CODE_FR_EXT_POS))

/* 0x994 : rc32m_ctrl1_aon */
#define AON_RC32M_CTRL1_AON_OFFSET               (0x994)
#define AON_RC32M_RESERVED                       AON_RC32M_RESERVED
#define AON_RC32M_RESERVED_POS                   (24U)
#define AON_RC32M_RESERVED_LEN                   (8U)
#define AON_RC32M_RESERVED_MSK                   (((1U << AON_RC32M_RESERVED_LEN) - 1) << AON_RC32M_RESERVED_POS)
#define AON_RC32M_RESERVED_UMSK                  (~(((1U << AON_RC32M_RESERVED_LEN) - 1) << AON_RC32M_RESERVED_POS))

/* 0x998 : rc32m_ctrl2_aon */
#define AON_RC32M_CTRL2_AON_OFFSET               (0x998)
#define AON_RC32M_CODE_FR_EXT2                   AON_RC32M_CODE_FR_EXT2
#define AON_RC32M_CODE_FR_EXT2_POS               (22U)
#define AON_RC32M_CODE_FR_EXT2_LEN               (8U)
#define AON_RC32M_CODE_FR_EXT2_MSK               (((1U << AON_RC32M_CODE_FR_EXT2_LEN) - 1) << AON_RC32M_CODE_FR_EXT2_POS)
#define AON_RC32M_CODE_FR_EXT2_UMSK              (~(((1U << AON_RC32M_CODE_FR_EXT2_LEN) - 1) << AON_RC32M_CODE_FR_EXT2_POS))
#define AON_RC32M_EXT_CODE_SEL                   AON_RC32M_EXT_CODE_SEL
#define AON_RC32M_EXT_CODE_SEL_POS               (31U)
#define AON_RC32M_EXT_CODE_SEL_LEN               (1U)
#define AON_RC32M_EXT_CODE_SEL_MSK               (((1U << AON_RC32M_EXT_CODE_SEL_LEN) - 1) << AON_RC32M_EXT_CODE_SEL_POS)
#define AON_RC32M_EXT_CODE_SEL_UMSK              (~(((1U << AON_RC32M_EXT_CODE_SEL_LEN) - 1) << AON_RC32M_EXT_CODE_SEL_POS))

/* 0xf00 : hbnram ctrl */
#define AON_TZC_HBNRAM_CTRL_OFFSET               (0xF00)
#define AON_TZC_HBNRAM_R0_EN                     AON_TZC_HBNRAM_R0_EN
#define AON_TZC_HBNRAM_R0_EN_POS                 (16U)
#define AON_TZC_HBNRAM_R0_EN_LEN                 (1U)
#define AON_TZC_HBNRAM_R0_EN_MSK                 (((1U << AON_TZC_HBNRAM_R0_EN_LEN) - 1) << AON_TZC_HBNRAM_R0_EN_POS)
#define AON_TZC_HBNRAM_R0_EN_UMSK                (~(((1U << AON_TZC_HBNRAM_R0_EN_LEN) - 1) << AON_TZC_HBNRAM_R0_EN_POS))
#define AON_TZC_HBNRAM_R0_LOCK                   AON_TZC_HBNRAM_R0_LOCK
#define AON_TZC_HBNRAM_R0_LOCK_POS               (24U)
#define AON_TZC_HBNRAM_R0_LOCK_LEN               (1U)
#define AON_TZC_HBNRAM_R0_LOCK_MSK               (((1U << AON_TZC_HBNRAM_R0_LOCK_LEN) - 1) << AON_TZC_HBNRAM_R0_LOCK_POS)
#define AON_TZC_HBNRAM_R0_LOCK_UMSK              (~(((1U << AON_TZC_HBNRAM_R0_LOCK_LEN) - 1) << AON_TZC_HBNRAM_R0_LOCK_POS))

/* 0xf04 : hbnram r0 */
#define AON_TZC_HBNRAM_R0_OFFSET                 (0xF04)
#define AON_TZC_HBNRAM_R0_END                    AON_TZC_HBNRAM_R0_END
#define AON_TZC_HBNRAM_R0_END_POS                (0U)
#define AON_TZC_HBNRAM_R0_END_LEN                (11U)
#define AON_TZC_HBNRAM_R0_END_MSK                (((1U << AON_TZC_HBNRAM_R0_END_LEN) - 1) << AON_TZC_HBNRAM_R0_END_POS)
#define AON_TZC_HBNRAM_R0_END_UMSK               (~(((1U << AON_TZC_HBNRAM_R0_END_LEN) - 1) << AON_TZC_HBNRAM_R0_END_POS))
#define AON_TZC_HBNRAM_R0_START                  AON_TZC_HBNRAM_R0_START
#define AON_TZC_HBNRAM_R0_START_POS              (16U)
#define AON_TZC_HBNRAM_R0_START_LEN              (11U)
#define AON_TZC_HBNRAM_R0_START_MSK              (((1U << AON_TZC_HBNRAM_R0_START_LEN) - 1) << AON_TZC_HBNRAM_R0_START_POS)
#define AON_TZC_HBNRAM_R0_START_UMSK             (~(((1U << AON_TZC_HBNRAM_R0_START_LEN) - 1) << AON_TZC_HBNRAM_R0_START_POS))

struct aon_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[2048];

    /* 0x800 : aon */
    union {
        struct {
            uint32_t aon_resv              : 8; /* [ 7: 0],        r/w,        0xf */
            uint32_t reserved_8_11         : 4; /* [11: 8],       rsvd,        0x0 */
            uint32_t pu_aon_dc_tbuf        : 1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_19        : 7; /* [19:13],       rsvd,        0x0 */
            uint32_t ldo11_rt_pulldown     : 1; /* [   20],        r/w,        0x0 */
            uint32_t ldo11_rt_pulldown_sel : 1; /* [   21],        r/w,        0x0 */
            uint32_t sw_pu_ldo11_rt        : 1; /* [   22],        r/w,        0x1 */
            uint32_t reserved_23_31        : 9; /* [31:23],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon;

    /* 0x804 : aon_common */
    union {
        struct {
            uint32_t reserved_0_7           : 8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t ten_vddcore_aon        : 1; /* [    8],        r/w,        0x0 */
            uint32_t ten_ldo11soc_aon       : 1; /* [    9],        r/w,        0x0 */
            uint32_t ten_dcdc_0_aon         : 1; /* [   10],        r/w,        0x0 */
            uint32_t ten_dcdc_1_aon         : 1; /* [   11],        r/w,        0x0 */
            uint32_t ten_bg_sys_aon         : 1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15         : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t ten_ldo15rf_aon        : 1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19         : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t ten_cip_misc_aon       : 1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21            : 1; /* [   21],       rsvd,        0x0 */
            uint32_t aon_dtest_pulldown_aon : 1; /* [   22],        r/w,        0x1 */
            uint32_t reserved_23_31         : 9; /* [31:23],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon_common;

    /* 0x808 : Digital Control */
    union {
        struct {
            uint32_t pu_mbg_aon              : 1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1              : 1; /* [    1],       rsvd,        0x0 */
            uint32_t pu_sfreg_aon            : 1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_5            : 3; /* [ 5: 3],       rsvd,        0x0 */
            uint32_t ten_mbg_aon             : 1; /* [    6],        r/w,        0x0 */
            uint32_t ten_cip_aon             : 1; /* [    7],        r/w,        0x0 */
            uint32_t cip_resv_aon            : 8; /* [15: 8],        r/w,       0x55 */
            uint32_t mbg_prereg_vout_sel_aon : 2; /* [17:16],        r/w,        0x1 */
            uint32_t mbg_prereg_bypass_aon   : 1; /* [   18],        r/w,        0x0 */
            uint32_t mbg_ptat_out_en_aon     : 1; /* [   19],        r/w,        0x0 */
            uint32_t sfreg_vg10_sel_aon      : 3; /* [22:20],        r/w,        0x2 */
            uint32_t reserved_23             : 1; /* [   23],       rsvd,        0x0 */
            uint32_t sfreg_vg09_sel_aon      : 3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27_31          : 5; /* [31:27],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mbg_sfreg;

    /* 0x80C : aon_0 */
    union {
        struct {
            uint32_t pu_bod_aon             : 1; /* [    0],        r/w,        0x0 */
            uint32_t pu_rc32k_aon           : 1; /* [    1],          r,        0x1 */
            uint32_t pu_vcc18d_aon          : 1; /* [    2],        r/w,        0x1 */
            uint32_t pu_xtal32k_aon         : 1; /* [    3],        r/w,        0x0 */
            uint32_t pu_xtal32k_buf_aon     : 1; /* [    4],        r/w,        0x1 */
            uint32_t ldo18aon_ext18_en_aon  : 1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6             : 1; /* [    6],       rsvd,        0x0 */
            uint32_t ldo18aon_slow_pld_aon  : 1; /* [    7],        r/w,        0x0 */
            uint32_t ldo18aon_cc_aon        : 3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11            : 1; /* [   11],       rsvd,        0x0 */
            uint32_t ldo18aon_rz_aon        : 2; /* [13:12],        r/w,        0x1 */
            uint32_t reserved_14_15         : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t ldo18aon_vout_sel_aon  : 3; /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19            : 1; /* [   19],       rsvd,        0x0 */
            uint32_t ldo18aon_vout_trim_aon : 4; /* [23:20],        r/w,        0x8 */
            uint32_t ldo08aon_vout_sel_aon  : 4; /* [27:24],        r/w,        0x9 */
            uint32_t ldo08aon_vout_trim_aon : 4; /* [31:28],        r/w,        0x8 */
        } BF;
        uint32_t WORD;
    } aon_0;

    /* 0x810 : Digital Control */
    union {
        struct {
            uint32_t bod_th_aon       : 3; /* [ 2: 0],        r/w,        0x5 */
            uint32_t r_bod_out        : 1; /* [    3],          r,        0x0 */
            uint32_t por_out          : 1; /* [    4],          r,        0x0 */
            uint32_t hw_pu_bg_sys_aon : 1; /* [    5],          r,        0x0 */
            uint32_t hw_pu_dcdc12_aon : 1; /* [    6],          r,        0x0 */
            uint32_t hw_pu_ldo09_aon  : 1; /* [    7],          r,        0x0 */
            uint32_t rc32m_pu_pds     : 1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_11    : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t aon_resv_aon     : 8; /* [19:12],        r/w,       0x55 */
            uint32_t sw_soc_en_aon    : 1; /* [   20],        r/w,        0x1 */
            uint32_t sw_wb_en_aon     : 1; /* [   21],        r/w,        0x1 */
            uint32_t sw_bz_en_aon     : 1; /* [   22],        r/w,        0x1 */
            uint32_t reserved_23_31   : 9; /* [31:23],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon_1;

    /* 0x814 : Digital Control */
    union {
        struct {
            uint32_t rc32k_cap_sel_aon      : 4;  /* [ 3: 0],        r/w,        0x8 */
            uint32_t reserved_4_14          : 11; /* [14: 4],       rsvd,        0x0 */
            uint32_t rc32k_pmip_clk_sel_aon : 1;  /* [   15],        r/w,        0x0 */
            uint32_t rc32k_tcomp_code_aon   : 10; /* [25:16],        r/w,        0x0 */
            uint32_t rc32k_tcomp_en_aon     : 1;  /* [   26],        r/w,        0x0 */
            uint32_t reserved_27            : 1;  /* [   27],       rsvd,        0x0 */
            uint32_t rc32k_vref_dly_aon     : 2;  /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31         : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32k_0;

    /* 0x818 : Digital Control */
    union {
        struct {
            uint32_t rc32k_rdy              : 1;  /* [    0],          r,        0x0 */
            uint32_t rc32k_code_valid       : 1;  /* [    1],          r,        0x0 */
            uint32_t reserved_2_3           : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t rc32k_iptat_bm_aon     : 3;  /* [ 6: 4],        r/w,        0x3 */
            uint32_t pu_rc32k_iptat_gen_aon : 1;  /* [    7],        r/w,        0x1 */
            uint32_t reserved_8_31          : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32k_1;

    /* 0x81C : Digital Control */
    union {
        struct {
            uint32_t xtal32k_ac_cap_short_aon   : 1; /* [    0],        r/w,        0x0 */
            uint32_t xtal32k_fast_startup_aon   : 1; /* [    1],        r/w,        0x0 */
            uint32_t xtal32k_amp_ctrl_aon       : 2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t reserved_4_9               : 6; /* [ 9: 4],       rsvd,        0x0 */
            uint32_t xtal32k_ext_sel_aon        : 1; /* [   10],        r/w,        0x0 */
            uint32_t xtal32k_hiz_en_aon         : 1; /* [   11],        r/w,        0x1 */
            uint32_t xtal32k_inv_stre_aon       : 2; /* [13:12],        r/w,        0x3 */
            uint32_t xtal32k_otf_short_aon      : 1; /* [   14],        r/w,        0x0 */
            uint32_t xtal32k_outbuf_stre_aon    : 1; /* [   15],        r/w,        0x0 */
            uint32_t xtal32k_rdy_rstb_aon       : 1; /* [   16],        r/w,        0x1 */
            uint32_t xtal32k_gm_pd_ctrl_aon     : 1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_19             : 2; /* [19:18],       rsvd,        0x0 */
            uint32_t xtal32k_rdy_sel_aon        : 2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23             : 2; /* [23:22],       rsvd,        0x0 */
            uint32_t xtal32k_reg_aon            : 2; /* [25:24],        r/w,        0x1 */
            uint32_t xtal32k_clk_rdy            : 1; /* [   26],          r,        0x0 */
            uint32_t xtal32k_inv_extra_stre_aon : 1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31             : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal32k;

    /* 0x820 : bg_sys_top */
    union {
        struct {
            uint32_t pu_bg_sys_aon : 1;  /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } bg_sys_top;

    /* 0x824 : ldo11soc_and_dctest */
    union {
        struct {
            uint32_t reserved_0_27    : 28; /* [27: 0],       rsvd,        0x0 */
            uint32_t ldo11soc_rdy_aon : 1;  /* [   28],          r,        0x1 */
            uint32_t reserved_29_31   : 3;  /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ldo11soc_and_dctest;

    /* 0x828 : psw_misc */
    union {
        struct {
            uint32_t reserved_0_3               : 4;  /* [ 3: 0],       rsvd,        0x0 */
            uint32_t usb20_rcal_coarse_aon      : 2;  /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7               : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t usb20_rcal_code_aon        : 5;  /* [12: 8],        r/w,       0x11 */
            uint32_t reserved_13_15             : 3;  /* [15:13],       rsvd,        0x0 */
            uint32_t usb20_rref_hiz_aon         : 1;  /* [   16],        r/w,        0x1 */
            uint32_t usb20_rref_cal_iin_en_aon  : 1;  /* [   17],        r/w,        0x0 */
            uint32_t usb20_rref_cal_vout_en_aon : 1;  /* [   18],        r/w,        0x0 */
            uint32_t usb20_rref_ext_en_aon      : 1;  /* [   19],        r/w,        0x0 */
            uint32_t reserved_20_31             : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } psw_misc;

    /* 0x82C : Digital Control */
    union {
        struct {
            uint32_t ldo09soc_ext_aon       : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1             : 1;  /* [    1],       rsvd,        0x0 */
            uint32_t ldo09soc_slow_pld_aon  : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3             : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t ldo09soc_cc_aon        : 3;  /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7             : 1;  /* [    7],       rsvd,        0x0 */
            uint32_t ldo09soc_rz_aon        : 2;  /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11         : 2;  /* [11:10],       rsvd,        0x0 */
            uint32_t ldo09soc_vout_sel_aon  : 4;  /* [15:12],        r/w,        0x8 */
            uint32_t ldo09soc_vout_trim_aon : 4;  /* [19:16],        r/w,        0x7 */
            uint32_t ldo09soc_pwr_good      : 1;  /* [   20],          r,        0x0 */
            uint32_t ldo09_rdy_aon          : 1;  /* [   21],          r,        0x0 */
            uint32_t reserved_22_31         : 10; /* [31:22],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ldo09soc;

    /* 0x830 : Digital Control */
    union {
        struct {
            uint32_t dcdc12_bm_lp_aon           : 3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                 : 1; /* [    3],       rsvd,        0x0 */
            uint32_t dcdc12_bm_nm_aon           : 3; /* [ 6: 4],        r/w,        0x6 */
            uint32_t reserved_7                 : 1; /* [    7],       rsvd,        0x0 */
            uint32_t dcdc12_cfb_sel_aon         : 4; /* [11: 8],        r/w,        0x8 */
            uint32_t dcdc12_chf_sel_aon         : 4; /* [15:12],        r/w,        0x1 */
            uint32_t dcdc12_comp_gm_sel_aon     : 3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                : 1; /* [   19],       rsvd,        0x0 */
            uint32_t dcdc12_cs_delay_aon        : 3; /* [22:20],        r/w,        0x4 */
            uint32_t dcdc12_doubler_32k_aon     : 1; /* [   23],        r/w,        0x0 */
            uint32_t dcdc12_drv_sr_aon          : 2; /* [25:24],        r/w,        0x2 */
            uint32_t dcdc12_en_antiring_aon     : 1; /* [   26],        r/w,        0x1 */
            uint32_t dcdc12_antiring_byp_lp_aon : 1; /* [   27],        r/w,        0x1 */
            uint32_t dcdc12_isense_trim_aon     : 3; /* [30:28],        r/w,        0x4 */
            uint32_t dcdc12_force_en_cs_zvs_aon : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } dcdc12_0;

    /* 0x834 : Digital Control */
    union {
        struct {
            uint32_t reserved_0                   : 1; /* [    0],       rsvd,        0x0 */
            uint32_t dcdc12_lp_force_en_aon       : 1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                 : 2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t dcdc12_nonoverlap_td_aon     : 5; /* [ 8: 4],        r/w,        0xc */
            uint32_t reserved_9_11                : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t dcdc12_ocp_lp_tw_aon         : 2; /* [13:12],        r/w,        0x0 */
            uint32_t dcdc12_ocp_out_aon           : 1; /* [   14],          r,        0x0 */
            uint32_t dcdc12_ocp_rst_aon           : 1; /* [   15],        r/w,        0x0 */
            uint32_t dcdc12_ocp_vth_aon           : 3; /* [18:16],        r/w,        0x4 */
            uint32_t reserved_19                  : 1; /* [   19],       rsvd,        0x0 */
            uint32_t dcdc12_osc_2m_mode_aon       : 1; /* [   20],        r/w,        0x0 */
            uint32_t dcdc12_osc_en_inhibit_t2_aon : 1; /* [   21],        r/w,        0x1 */
            uint32_t dcdc12_osc_en_slow_aon       : 1; /* [   22],        r/w,        0x0 */
            uint32_t dcdc12_osc_en_stop_aon       : 1; /* [   23],        r/w,        0x1 */
            uint32_t dcdc12_osc_freq_trim_aon     : 4; /* [27:24],        r/w,        0x8 */
            uint32_t dcdc12_pulldown_aon          : 1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31               : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } dcdc12_1;

    /* 0x838 : Digital Control */
    union {
        struct {
            uint32_t dcdc12_osc_ss_fdev_aon    : 2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t dcdc12_osc_ss_period_aon  : 2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t dcdc12_osc_ss_rstn_aon    : 1; /* [    4],        r/w,        0x0 */
            uint32_t dcdc12_osc_ss_en_aon      : 1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7              : 2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t dcdc12_pwm_lp_vth_aon     : 3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11               : 1; /* [   11],       rsvd,        0x0 */
            uint32_t dcdc12_rc_sel_aon         : 4; /* [15:12],        r/w,        0x3 */
            uint32_t dcdc12_rdy_aon            : 1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19            : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t dcdc12_slope_curr_sel_aon : 5; /* [24:20],        r/w,        0x6 */
            uint32_t reserved_25_27            : 3; /* [27:25],       rsvd,        0x0 */
            uint32_t dcdc12_sstart_time_aon    : 2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31            : 2; /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } dcdc12_2;

    /* 0x83C : Digital Control */
    union {
        struct {
            uint32_t dcdc12_vc_clamp_vth_aon : 3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3              : 1; /* [    3],       rsvd,        0x0 */
            uint32_t dcdc12_vout_sel_aon     : 5; /* [ 8: 4],        r/w,       0x14 */
            uint32_t reserved_9_11           : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t dcdc12_vout_trim_aon    : 4; /* [15:12],        r/w,        0x7 */
            uint32_t dcdc12_vpfm_aon         : 4; /* [19:16],        r/w,        0x4 */
            uint32_t dcdc12_zvs_td_opt_aon   : 3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23             : 1; /* [   23],       rsvd,        0x0 */
            uint32_t pmip_resv_aon           : 8; /* [31:24],        r/w,       0x55 */
        } BF;
        uint32_t WORD;
    } dcdc12_3;

    /* 0x840 : Digital Control */
    union {
        struct {
            uint32_t reserved_0_3      : 4;  /* [ 3: 0],       rsvd,        0x0 */
            uint32_t pu_psw18_peri_aon : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31     : 27; /* [31: 5],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } psw;

    /* 0x844 : Digital Control */
    union {
        struct {
            uint32_t tmux_aon                  : 3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                : 1; /* [    3],       rsvd,        0x0 */
            uint32_t pmip_dc_tp_out_en_aon     : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5                : 1; /* [    5],       rsvd,        0x0 */
            uint32_t aon_dc_tp_out_en_aon      : 1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                : 1; /* [    7],       rsvd,        0x0 */
            uint32_t ten_aon                   : 1; /* [    8],        r/w,        0x0 */
            uint32_t ten_bgsys_aon             : 1; /* [    9],        r/w,        0x0 */
            uint32_t ten_dcdc12_0_aon          : 1; /* [   10],        r/w,        0x0 */
            uint32_t ten_dcdc12_1_aon          : 1; /* [   11],        r/w,        0x0 */
            uint32_t ten_ldo09soc_aon          : 1; /* [   12],        r/w,        0x0 */
            uint32_t ten_rc32m                 : 1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15            : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t ten_xtal32k_aon           : 1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19            : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t dten_xtal32k_clk_aon      : 1; /* [   20],        r/w,        0x0 */
            uint32_t dten_xtal32k_pu_aon       : 1; /* [   21],        r/w,        0x0 */
            uint32_t dten_xtal32k_rdy_aon      : 1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23               : 1; /* [   23],       rsvd,        0x0 */
            uint32_t pmip_dc_tp_int_out_en_aon : 1; /* [   24],        r/w,        0x0 */
            uint32_t aon_dc_tp_int_out_en_aon  : 1; /* [   25],        r/w,        0x0 */
            uint32_t dten_rc32k_aon            : 1; /* [   26],        r/w,        0x0 */
            uint32_t dten_rc32m                : 1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31            : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ten;

    /* 0x848 : Digital Control */
    union {
        struct {
            uint32_t dcdc12_lp_en_aon           : 1;  /* [    0],        r/w,        0x0 */
            uint32_t ldo09soc_lp_en_aon         : 1;  /* [    1],        r/w,        0x0 */
            uint32_t ldo18aon_lp_en_aon         : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                 : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t dcdc12_lp_en_aon_ctrl_hw   : 1;  /* [    4],        r/w,        0x1 */
            uint32_t ldo09soc_lp_en_aon_ctrl_hw : 1;  /* [    5],        r/w,        0x1 */
            uint32_t ldo18aon_lp_en_aon_ctrl_hw : 1;  /* [    6],        r/w,        0x1 */
            uint32_t reserved_7                 : 1;  /* [    7],       rsvd,        0x0 */
            uint32_t dcdc12_lp_en_aon_hw        : 1;  /* [    8],          r,        0x0 */
            uint32_t ldo09soc_lp_en_aon_hw      : 1;  /* [    9],          r,        0x0 */
            uint32_t ldo18aon_lp_en_aon_hw      : 1;  /* [   10],          r,        0x0 */
            uint32_t reserved_11_31             : 21; /* [31:11],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } lp_en_hw;

    /* 0x84C : Digital Control */
    union {
        struct {
            uint32_t ldo09soc_vout_sel_aon_hw : 4; /* [ 3: 0],          r,        0x8 */
            uint32_t ldo18aon_vout_sel_aon_hw : 3; /* [ 6: 4],          r,        0x3 */
            uint32_t reserved_7               : 1; /* [    7],       rsvd,        0x0 */
            uint32_t dcdc12_vout_sel_aon_hw   : 5; /* [12: 8],          r,       0x14 */
            uint32_t reserved_13_15           : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t ldo09soc_vout_sel_aon_lp : 4; /* [19:16],        r/w,        0x8 */
            uint32_t ldo18aon_vout_sel_aon_lp : 3; /* [22:20],        r/w,        0x3 */
            uint32_t reserved_23              : 1; /* [   23],       rsvd,        0x0 */
            uint32_t dcdc12_vout_sel_aon_lp   : 5; /* [28:24],        r/w,       0x14 */
            uint32_t reserved_29_31           : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } lp_en_ctrl;

    /* 0x850  reserved */
    uint8_t RESERVED0x850[48];

    /* 0x880 : rf_top_aon */
    union {
        struct {
            uint32_t reserved_0               : 1; /* [    0],       rsvd,        0x0 */
            uint32_t pu_ldo15rf_aon           : 1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_7             : 6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t ldo15rf_sstart_sel_aon   : 1; /* [    8],        r/w,        0x1 */
            uint32_t ldo15rf_sstart_delay_aon : 2; /* [10: 9],        r/w,        0x0 */
            uint32_t reserved_11              : 1; /* [   11],       rsvd,        0x0 */
            uint32_t ldo15rf_pulldown_aon     : 1; /* [   12],        r/w,        0x0 */
            uint32_t ldo15rf_pulldown_sel_aon : 1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15           : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t ldo15rf_vout_sel_aon     : 3; /* [18:16],        r/w,        0x2 */
            uint32_t reserved_19_23           : 5; /* [23:19],       rsvd,        0x0 */
            uint32_t ldo15rf_cc_aon           : 2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26              : 1; /* [   26],       rsvd,        0x0 */
            uint32_t ldo15rf_bypass_aon       : 1; /* [   27],        r/w,        0x0 */
            uint32_t ldo15rf_vout_trim_aon    : 4; /* [31:28],        r/w,        0x7 */
        } BF;
        uint32_t WORD;
    } rf_top_aon;

    /* 0x884 : xtal_cfg */
    union {
        struct {
            uint32_t xtal_bk_aon            : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t xtal_capcode_extra_aon : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3             : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t xtal_buf_en_aon        : 2;  /* [ 5: 4],        r/w,        0x3 */
            uint32_t xtal_acbuf_en_aon      : 1;  /* [    6],        r/w,        0x1 */
            uint32_t xtal_acbuf_mode_aon    : 1;  /* [    7],        r/w,        0x1 */
            uint32_t xtal_buf_hp_aon        : 2;  /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_11         : 2;  /* [11:10],       rsvd,        0x0 */
            uint32_t xtal_fast_startup_aon  : 1;  /* [   12],        r/w,        0x1 */
            uint32_t reserved_13            : 1;  /* [   13],       rsvd,        0x0 */
            uint32_t xtal_amp_ctrl_aon      : 2;  /* [15:14],        r/w,        0x3 */
            uint32_t reserved_16_31         : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_cfg;

    /* 0x888 : tsen */
    union {
        struct {
            uint32_t tsen_refcode_corner : 12; /* [11: 0],        r/w,      0x8ff */
            uint32_t reserved_12_15      : 4;  /* [15:12],       rsvd,        0x0 */
            uint32_t tsen_refcode_rfcal  : 12; /* [27:16],        r/w,      0x8ff */
            uint32_t xtal_rdy            : 1;  /* [   28],          r,        0x1 */
            uint32_t reserved_29_31      : 3;  /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } tsen;

    /* 0x88c  reserved */
    uint8_t RESERVED0x88c[4];

    /* 0x890 : aon_wdt */
    union {
        struct {
            uint32_t cr_ano_wdt_en   : 1;  /* [    0],        r/w,        0x0 */
            uint32_t cr_sw_kick_pos  : 1;  /* [    1],        w1p,        0x0 */
            uint32_t cr_aon_wdt_clr  : 1;  /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_6    : 4;  /* [ 6: 3],       rsvd,        0x0 */
            uint32_t st_aon_wdt      : 1;  /* [    7],          r,        0x0 */
            uint32_t cr_cnt_mask_bit : 6;  /* [13: 8],        r/w,        0x4 */
            uint32_t reserved_14_31  : 18; /* [31:14],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon_wdt;

    /* 0x894  reserved */
    uint8_t RESERVED0x894[48];

    /* 0x8C4 : xtal_pu */
    union {
        struct {
            uint32_t pu_xtal_aon      : 1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3     : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t pu_xtal_buf_aon  : 1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7     : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t pu_xtal_core_aon : 1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11    : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t pud_xtal_aon     : 1; /* [   12],          r,        0x1 */
            uint32_t reserved_13_15   : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t pud_dly_xtal_aon : 3; /* [18:16],        r/w,        0x1 */
            uint32_t reserved_19      : 1; /* [   19],       rsvd,        0x0 */
            uint32_t xtal_reserve_aon : 4; /* [23:20],        r/w,        0x3 */
            uint32_t reserved_24_31   : 8; /* [31:24],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_pu;

    /* 0x8C8 : xtal_ldo */
    union {
        struct {
            uint32_t xtal_buf_power_boost_aon  : 1;  /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3              : 3;  /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_core_power_boost_aon : 1;  /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7              : 3;  /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_ldo_bypass_aon       : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11             : 3;  /* [11: 9],       rsvd,        0x0 */
            uint32_t xtal_sleep_aon            : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15            : 3;  /* [15:13],       rsvd,        0x0 */
            uint32_t xtal_vgate_filter_aon     : 2;  /* [17:16],        r/w,        0x3 */
            uint32_t reserved_18_31            : 14; /* [31:18],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_ldo;

    /* 0x8CC : xtal_core0 */
    union {
        struct {
            uint32_t xtal_injection_en_aon      : 1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3               : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_capcode_extra_in_aon  : 1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7               : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_capcode_extra_out_aon : 1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11              : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t xtal_gm_pd_ctrl_aon        : 1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15             : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t xtal_capcode_in_aon        : 8; /* [23:16],        r/w,       0xc8 */
            uint32_t xtal_capcode_out_aon       : 8; /* [31:24],        r/w,       0xc8 */
        } BF;
        uint32_t WORD;
    } xtal_core0;

    /* 0x8D0 : xtal_core1 */
    union {
        struct {
            uint32_t xtal_capcode_unbalance_aon      : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                    : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_capcode_extra_in_fast_aon  : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                    : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_capcode_extra_out_fast_aon : 1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                   : 7; /* [15: 9],       rsvd,        0x0 */
            uint32_t xtal_capcode_in_fast_aon        : 8; /* [23:16],        r/w,       0x3c */
            uint32_t xtal_capcode_out_fast_aon       : 8; /* [31:24],        r/w,       0x3c */
        } BF;
        uint32_t WORD;
    } xtal_core1;

    /* 0x8D4 : xtal_core2 */
    union {
        struct {
            uint32_t xtal_capcode_unbalance_fast_aon : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                    : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_ext_sel_aon                : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                    : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_gm_boost_fast_aon          : 2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                  : 2; /* [11:10],       rsvd,        0x0 */
            uint32_t xtal_gm_boost_aon               : 2; /* [13:12],        r/w,        0x2 */
            uint32_t reserved_14_15                  : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t xtal_socbuf_en_aon              : 1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                  : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t xtal_socbuf_drv_sel_aon         : 1; /* [   20],        r/w,        0x0 */
            uint32_t xtal_wifipll_buf_latch_en_aon   : 1; /* [   21],        r/w,        0x1 */
            uint32_t xtal_wifipll_buf_drv_sel_aon    : 1; /* [   22],        r/w,        0x1 */
            uint32_t xtal_wifipll_buf_dum_en_aon     : 1; /* [   23],        r/w,        0x1 */
            uint32_t xtal_wifipll_buf_en_aon         : 1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                  : 3; /* [27:25],       rsvd,        0x0 */
            uint32_t xtal_inn_cfg_en_aon             : 1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                  : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_core2;

    /* 0x8D8 : xtal_buffer */
    union {
        struct {
            uint32_t reserved_0_3                  : 4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t xtal_taur_lo_buf_en_aon       : 1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7                  : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_usb2_buf_en_aon          : 1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                 : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t xtal_extbuf_en_aon            : 1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_14                : 2; /* [14:13],       rsvd,        0x0 */
            uint32_t xtal_taur_lo_buf_latch_en_aon : 1; /* [   15],        r/w,        0x1 */
            uint32_t xtal_arie_lo_buf_drv_sel_aon  : 1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_19                : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t xtal_arie_lo_buf_dum_en_aon   : 1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23                : 3; /* [23:21],       rsvd,        0x0 */
            uint32_t xtal_arie_lo_buf_en_aon       : 1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                : 3; /* [27:25],       rsvd,        0x0 */
            uint32_t xtal_arie_lo_buf_latch_en_aon : 1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_buffer;

    /* 0x8DC : xtal_dig */
    union {
        struct {
            uint32_t xtal_rdy_int_sel_aon : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3         : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t xtal_rdy_sel_aon     : 2;  /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_31        : 26; /* [31: 6],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_dig;

    /* 0x8E0 : xtal_test */
    union {
        struct {
            uint32_t ten_xtal_aon      : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7      : 7;  /* [ 7: 1],       rsvd,        0x0 */
            uint32_t dten_xtal_aon     : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11     : 3;  /* [11: 9],       rsvd,        0x0 */
            uint32_t dten_xtal_pu_aon  : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15    : 3;  /* [15:13],       rsvd,        0x0 */
            uint32_t dten_xtal_rdy_aon : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31    : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_test;

    /* 0x8e4  reserved */
    uint8_t RESERVED0x8e4[28];

    /* 0x900 : acomp0_ctrl */
    union {
        struct {
            uint32_t acomp0_en        : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3     : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t acomp0_hyst_seln : 3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t acomp0_hyst_selp : 3; /* [ 9: 7],        r/w,        0x0 */
            uint32_t acomp0_bias_prog : 2; /* [11:10],        r/w,        0x0 */
            uint32_t acomp0_level_sel : 6; /* [17:12],        r/w,        0x0 */
            uint32_t acomp0_neg_sel   : 4; /* [21:18],        r/w,        0x0 */
            uint32_t acomp0_pos_sel   : 4; /* [25:22],        r/w,        0x0 */
            uint32_t acomp0_muxen     : 1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31   : 5; /* [31:27],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } acomp0_ctrl;

    /* 0x904 : acomp1_ctrl */
    union {
        struct {
            uint32_t acomp1_en        : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3     : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t acomp1_hyst_seln : 3; /* [ 6: 4],        r/w,        0x0 */
            uint32_t acomp1_hyst_selp : 3; /* [ 9: 7],        r/w,        0x0 */
            uint32_t acomp1_bias_prog : 2; /* [11:10],        r/w,        0x0 */
            uint32_t acomp1_level_sel : 6; /* [17:12],        r/w,        0x0 */
            uint32_t acomp1_neg_sel   : 4; /* [21:18],        r/w,        0x0 */
            uint32_t acomp1_pos_sel   : 4; /* [25:22],        r/w,        0x0 */
            uint32_t acomp1_muxen     : 1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31   : 5; /* [31:27],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } acomp1_ctrl;

    /* 0x908 : acomp_ctrl */
    union {
        struct {
            uint32_t acomp1_rstn_ana : 1; /* [    0],        r/w,        0x1 */
            uint32_t acomp0_rstn_ana : 1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_7    : 6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t acomp1_test_en  : 1; /* [    8],        r/w,        0x0 */
            uint32_t acomp0_test_en  : 1; /* [    9],        r/w,        0x0 */
            uint32_t acomp1_test_sel : 2; /* [11:10],        r/w,        0x0 */
            uint32_t acomp0_test_sel : 2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_16  : 3; /* [16:14],       rsvd,        0x0 */
            uint32_t acomp1_out_raw  : 1; /* [   17],          r,        0x0 */
            uint32_t reserved_18     : 1; /* [   18],       rsvd,        0x0 */
            uint32_t acomp0_out_raw  : 1; /* [   19],          r,        0x0 */
            uint32_t reserved_20_23  : 4; /* [23:20],       rsvd,        0x0 */
            uint32_t acomp_vref_sel  : 6; /* [29:24],        r/w,        0x0 */
            uint32_t acomp_reserved  : 2; /* [31:30],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } acomp_ctrl;

    /* 0x90C : gpadc_reg_cmd1 */
    union {
        struct {
            uint32_t gpadc_ts_en          : 1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc_tsvbe_low      : 1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc_vbat_en        : 1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc_bias_sel       : 1; /* [    3],        r/w,        0x0 */
            uint32_t gpadc_int_neg_sel    : 5; /* [ 8: 4],        r/w,        0x4 */
            uint32_t reserved_9_11        : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t gpadc_int_pos_sel    : 5; /* [16:12],        r/w,        0x4 */
            uint32_t reserved_17_19       : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t gpadc_chip_sen_pu    : 1; /* [   20],        r/w,        0x0 */
            uint32_t gpadc_chip_sen_sel   : 4; /* [24:21],        r/w,        0x0 */
            uint32_t gpadc_dvdd_sel       : 2; /* [26:25],        r/w,        0x1 */
            uint32_t gpadc_tsext_sel      : 1; /* [   27],        r/w,        0x0 */
            uint32_t gpadc_vref_ex_ch_sel : 4; /* [31:28],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc_reg_cmd1;

    /* 0x910 : gpadc_reg_cmd2 */
    union {
        struct {
            uint32_t gpadc_mode           : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_7         : 6;  /* [ 7: 2],       rsvd,        0x0 */
            uint32_t gpadc_clk_ana_dly    : 4;  /* [11: 8],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_dly_en : 1;  /* [   12],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_inv    : 1;  /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15       : 2;  /* [15:14],       rsvd,        0x0 */
            uint32_t gpadc_clk_div_ratio  : 3;  /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19_31       : 13; /* [31:19],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc_reg_cmd2;

    /* 0x914 : gpadc1_reg_config1 */
    union {
        struct {
            uint32_t gpadc1_vref_ex_sel : 1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc1_diff_mode   : 1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc1_neg_gnd     : 1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc1_pga_vcmi_en : 1; /* [    3],        r/w,        0x0 */
            uint32_t gpadc1_vref_trim   : 6; /* [ 9: 4],        r/w,       0x32 */
            uint32_t reserved_10_11     : 2; /* [11:10],       rsvd,        0x0 */
            uint32_t gpadc1_chop_mode   : 2; /* [13:12],        r/w,        0x2 */
            uint32_t reserved_14_15     : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t gpadc1_pga_en      : 1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19     : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t gpadc1_pga_gain    : 3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23        : 1; /* [   23],       rsvd,        0x0 */
            uint32_t gpadc1_pga_vcm     : 2; /* [25:24],        r/w,        0x3 */
            uint32_t reserved_26_31     : 6; /* [31:26],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_config1;

    /* 0x918 : gpadc1_reg_config2 */
    union {
        struct {
            uint32_t gpadc1_global_en   : 1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc1_conv_start  : 1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc1_soft_rst    : 1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3         : 1; /* [    3],       rsvd,        0x0 */
            uint32_t gpadc1_dwa_en      : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7       : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gpadc1_dither_en   : 1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11      : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t gpadc1_dly_sel     : 3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15        : 1; /* [   15],       rsvd,        0x0 */
            uint32_t gpadc1_ext_pos_sel : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc1_ext_neg_sel : 4; /* [23:20],        r/w,        0xf */
            uint32_t reserved_24_31     : 8; /* [31:24],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_config2;

    /* 0x91C : gpadc1_reg_ctrl_0 */
    union {
        struct {
            uint32_t gpadc1_cont_conv_en : 1;  /* [    0],        r/w,        0x1 */
            uint32_t gpadc1_res_sel      : 3;  /* [ 3: 1],        r/w,        0x0 */
            uint32_t gpadc1_conv_delay   : 4;  /* [ 7: 4],        r/w,        0x4 */
            uint32_t gpadc1_ijct_start   : 1;  /* [    8],        r/w,        0x0 */
            uint32_t gpadc1_ijct_length  : 2;  /* [10: 9],        r/w,        0x0 */
            uint32_t reserved_11         : 1;  /* [   11],       rsvd,        0x0 */
            uint32_t gpadc1_pwm_trg_en   : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15      : 3;  /* [15:13],       rsvd,        0x0 */
            uint32_t gpadc1_scan_length  : 4;  /* [19:16],        r/w,        0x0 */
            uint32_t gpadc1_scan_en      : 1;  /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31      : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_ctrl_0;

    /* 0x920 : gpadc1_reg_os_cal_data */
    union {
        struct {
            uint32_t gpadc1_os_cal_data : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31     : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_os_cal_data;

    /* 0x924 : gpadc1_reg_scan_0 */
    union {
        struct {
            uint32_t gpadc1_scan_pos_0 : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_1 : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_2 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_3 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_4 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_5 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_6 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_7 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_scan_0;

    /* 0x928 : gpadc1_reg_scan_1 */
    union {
        struct {
            uint32_t gpadc1_scan_pos_8  : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_9  : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_10 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_11 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_12 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_13 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_14 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc1_scan_pos_15 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_scan_1;

    /* 0x92C : gpadc1_reg_scan_2 */
    union {
        struct {
            uint32_t gpadc1_scan_neg_0 : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_1 : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_2 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_3 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_4 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_5 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_6 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_7 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_scan_2;

    /* 0x930 : gpadc1_reg_scan_3 */
    union {
        struct {
            uint32_t gpadc1_scan_neg_8  : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_9  : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_10 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_11 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_12 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_13 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_14 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc1_scan_neg_15 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_scan_3;

    /* 0x934 : gpadc1_reg_ijct_ext_0 */
    union {
        struct {
            uint32_t gpadc1_ijct_ext_pos_0 : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_pos_1 : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_pos_2 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_pos_3 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_neg_0 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_neg_1 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_neg_2 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc1_ijct_ext_neg_3 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_ijct_ext_0;

    /* 0x938 : gpadc1_reg_ijct_int_0 */
    union {
        struct {
            uint32_t gpadc1_ijct_int_pos_0 : 5; /* [ 4: 0],        r/w,        0x4 */
            uint32_t reserved_5_7          : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gpadc1_ijct_int_pos_1 : 5; /* [12: 8],        r/w,        0x4 */
            uint32_t reserved_13_15        : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t gpadc1_ijct_int_pos_2 : 5; /* [20:16],        r/w,        0x4 */
            uint32_t reserved_21_23        : 3; /* [23:21],       rsvd,        0x0 */
            uint32_t gpadc1_ijct_int_pos_3 : 5; /* [28:24],        r/w,        0x4 */
            uint32_t reserved_29_31        : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_ijct_int_0;

    /* 0x93C : gpadc1_reg_ijct_int_1 */
    union {
        struct {
            uint32_t gpadc1_ijct_int_neg_0 : 5; /* [ 4: 0],        r/w,        0x4 */
            uint32_t reserved_5_7          : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gpadc1_ijct_int_neg_1 : 5; /* [12: 8],        r/w,        0x4 */
            uint32_t reserved_13_15        : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t gpadc1_ijct_int_neg_2 : 5; /* [20:16],        r/w,        0x4 */
            uint32_t reserved_21_23        : 3; /* [23:21],       rsvd,        0x0 */
            uint32_t gpadc1_ijct_int_neg_3 : 5; /* [28:24],        r/w,        0x4 */
            uint32_t reserved_29_31        : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_ijct_int_1;

    /* 0x940 : gpadc1_reg_result */
    union {
        struct {
            uint32_t gpadc1_data_out : 29; /* [28: 0],          r,   0xff0000 */
            uint32_t reserved_29_30  : 2;  /* [30:29],       rsvd,        0x0 */
            uint32_t gpadc1_data_rdy : 1;  /* [   31],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_result;

    /* 0x944 : gpadc1_reg_raw_data */
    union {
        struct {
            uint32_t gpadc1_raw_data : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_15  : 4;  /* [15:12],       rsvd,        0x0 */
            uint32_t gpadc1_reserved : 16; /* [31:16],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_raw_data;

    /* 0x948 : gpadc1_reg_satur */
    union {
        struct {
            uint32_t gpadc1_neg_satur      : 1;  /* [    0],          r,        0x0 */
            uint32_t gpadc1_pos_satur      : 1;  /* [    1],          r,        0x0 */
            uint32_t reserved_2_3          : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gpadc1_neg_satur_clr  : 1;  /* [    4],        r/w,        0x0 */
            uint32_t gpadc1_pos_satur_clr  : 1;  /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7          : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t gpadc1_neg_satur_mask : 1;  /* [    8],        r/w,        0x0 */
            uint32_t gpadc1_pos_satur_mask : 1;  /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31        : 22; /* [31:10],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc1_reg_satur;

    /* 0x94C : gpadc2_reg_config1 */
    union {
        struct {
            uint32_t gpadc2_vref_ex_sel : 1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc2_diff_mode   : 1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc2_neg_gnd     : 1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc2_pga_vcmi_en : 1; /* [    3],        r/w,        0x0 */
            uint32_t gpadc2_vref_trim   : 6; /* [ 9: 4],        r/w,       0x32 */
            uint32_t reserved_10_11     : 2; /* [11:10],       rsvd,        0x0 */
            uint32_t gpadc2_chop_mode   : 2; /* [13:12],        r/w,        0x2 */
            uint32_t reserved_14_15     : 2; /* [15:14],       rsvd,        0x0 */
            uint32_t gpadc2_pga_en      : 1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19     : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t gpadc2_pga_gain    : 3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23        : 1; /* [   23],       rsvd,        0x0 */
            uint32_t gpadc2_pga_vcm     : 2; /* [25:24],        r/w,        0x3 */
            uint32_t reserved_26_31     : 6; /* [31:26],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_config1;

    /* 0x950 : gpadc2_reg_config2 */
    union {
        struct {
            uint32_t gpadc2_global_en   : 1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc2_conv_start  : 1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc2_soft_rst    : 1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3         : 1; /* [    3],       rsvd,        0x0 */
            uint32_t gpadc2_dwa_en      : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7       : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gpadc2_dither_en   : 1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11      : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t gpadc2_dly_sel     : 3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15        : 1; /* [   15],       rsvd,        0x0 */
            uint32_t gpadc2_ext_pos_sel : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc2_ext_neg_sel : 4; /* [23:20],        r/w,        0xf */
            uint32_t reserved_24_31     : 8; /* [31:24],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_config2;

    /* 0x954 : gpadc2_reg_ctrl_0 */
    union {
        struct {
            uint32_t gpadc2_cont_conv_en : 1;  /* [    0],        r/w,        0x1 */
            uint32_t gpadc2_res_sel      : 3;  /* [ 3: 1],        r/w,        0x0 */
            uint32_t gpadc2_conv_delay   : 4;  /* [ 7: 4],        r/w,        0x4 */
            uint32_t gpadc2_ijct_start   : 1;  /* [    8],        r/w,        0x0 */
            uint32_t gpadc2_ijct_length  : 2;  /* [10: 9],        r/w,        0x0 */
            uint32_t reserved_11         : 1;  /* [   11],       rsvd,        0x0 */
            uint32_t gpadc2_pwm_trg_en   : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15      : 3;  /* [15:13],       rsvd,        0x0 */
            uint32_t gpadc2_scan_length  : 4;  /* [19:16],        r/w,        0x0 */
            uint32_t gpadc2_scan_en      : 1;  /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31      : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_ctrl_0;

    /* 0x958 : gpadc2_reg_os_cal_data */
    union {
        struct {
            uint32_t gpadc2_os_cal_data : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31     : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_os_cal_data;

    /* 0x95C : gpadc2_reg_scan_0 */
    union {
        struct {
            uint32_t gpadc2_scan_pos_0 : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_1 : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_2 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_3 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_4 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_5 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_6 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_7 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_scan_0;

    /* 0x960 : gpadc2_reg_scan_1 */
    union {
        struct {
            uint32_t gpadc2_scan_pos_8  : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_9  : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_10 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_11 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_12 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_13 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_14 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc2_scan_pos_15 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_scan_1;

    /* 0x964 : gpadc2_reg_scan_2 */
    union {
        struct {
            uint32_t gpadc2_scan_neg_0 : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_1 : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_2 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_3 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_4 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_5 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_6 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_7 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_scan_2;

    /* 0x968 : gpadc2_reg_scan_3 */
    union {
        struct {
            uint32_t gpadc2_scan_neg_8  : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_9  : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_10 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_11 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_12 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_13 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_14 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc2_scan_neg_15 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_scan_3;

    /* 0x96C : gpadc2_reg_ijct_ext_0 */
    union {
        struct {
            uint32_t gpadc2_ijct_ext_pos_0 : 4; /* [ 3: 0],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_pos_1 : 4; /* [ 7: 4],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_pos_2 : 4; /* [11: 8],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_pos_3 : 4; /* [15:12],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_neg_0 : 4; /* [19:16],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_neg_1 : 4; /* [23:20],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_neg_2 : 4; /* [27:24],        r/w,        0xf */
            uint32_t gpadc2_ijct_ext_neg_3 : 4; /* [31:28],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_ijct_ext_0;

    /* 0x970 : gpadc2_reg_result */
    union {
        struct {
            uint32_t gpadc2_data_out : 29; /* [28: 0],          r,   0xff0000 */
            uint32_t reserved_29_30  : 2;  /* [30:29],       rsvd,        0x0 */
            uint32_t gpadc2_data_rdy : 1;  /* [   31],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_result;

    /* 0x974 : gpadc2_reg_raw_data */
    union {
        struct {
            uint32_t gpadc2_raw_data : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_15  : 4;  /* [15:12],       rsvd,        0x0 */
            uint32_t gpadc2_reserved : 16; /* [31:16],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_raw_data;

    /* 0x978 : gpadc2_reg_satur */
    union {
        struct {
            uint32_t gpadc2_neg_satur      : 1;  /* [    0],          r,        0x0 */
            uint32_t gpadc2_pos_satur      : 1;  /* [    1],          r,        0x0 */
            uint32_t reserved_2_3          : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gpadc2_neg_satur_clr  : 1;  /* [    4],        r/w,        0x0 */
            uint32_t gpadc2_pos_satur_clr  : 1;  /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7          : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t gpadc2_neg_satur_mask : 1;  /* [    8],        r/w,        0x0 */
            uint32_t gpadc2_pos_satur_mask : 1;  /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31        : 22; /* [31:10],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc2_reg_satur;

    /* 0x97C : gpadc_ten */
    union {
        struct {
            uint32_t ten_gpadc0    : 1;  /* [    0],        r/w,        0x0 */
            uint32_t ten_gpadc1    : 1;  /* [    1],        r/w,        0x0 */
            uint32_t ten_gpadc2    : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } gpadc_ten;

    /* 0x980 : hbncore_resv0 */
    union {
        struct {
            uint32_t hbncore_resv0_data : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } hbncore_resv0;

    /* 0x984 : hbncore_resv1 */
    union {
        struct {
            uint32_t hbncore_resv1_data : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } hbncore_resv1;

    /* 0x988  reserved */
    uint8_t RESERVED0x988[8];

    /* 0x990 : rc32m_ctrl0_aon */
    union {
        struct {
            uint32_t reserved_0_21     : 22; /* [21: 0],       rsvd,        0x0 */
            uint32_t rc32m_code_fr_ext : 8;  /* [29:22],        r/w,       0x7f */
            uint32_t reserved_30_31    : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl0_aon;

    /* 0x994 : rc32m_ctrl1_aon */
    union {
        struct {
            uint32_t reserved_0_23  : 24; /* [23: 0],       rsvd,        0x0 */
            uint32_t rc32m_reserved : 8;  /* [31:24],        r/w,        0xf */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl1_aon;

    /* 0x998 : rc32m_ctrl2_aon */
    union {
        struct {
            uint32_t reserved_0_21      : 22; /* [21: 0],       rsvd,        0x0 */
            uint32_t rc32m_code_fr_ext2 : 8;  /* [29:22],        r/w,       0x7f */
            uint32_t reserved_30        : 1;  /* [   30],       rsvd,        0x0 */
            uint32_t rc32m_ext_code_sel : 1;  /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } rc32m_ctrl2_aon;
};

typedef volatile struct aon_reg aon_reg_t;

#endif /* __AON_REG_H__ */
