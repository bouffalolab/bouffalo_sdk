/**
  ******************************************************************************
  * @file    aon_reg.h
  * @version V1.0
  * @date    2024-10-17
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

#include "bl616l.h"

/* 0x800 : aon */
#define AON_OFFSET                               (0x800)

/* 0x808 : aon_misc */
#define AON_MISC_OFFSET                          (0x808)
#define AON_SW_SOC_EN_AON                        AON_SW_SOC_EN_AON
#define AON_SW_SOC_EN_AON_POS                    (0U)
#define AON_SW_SOC_EN_AON_LEN                    (1U)
#define AON_SW_SOC_EN_AON_MSK                    (((1U << AON_SW_SOC_EN_AON_LEN) - 1) << AON_SW_SOC_EN_AON_POS)
#define AON_SW_SOC_EN_AON_UMSK                   (~(((1U << AON_SW_SOC_EN_AON_LEN) - 1) << AON_SW_SOC_EN_AON_POS))
#define AON_SW_WB_EN_AON                         AON_SW_WB_EN_AON
#define AON_SW_WB_EN_AON_POS                     (1U)
#define AON_SW_WB_EN_AON_LEN                     (1U)
#define AON_SW_WB_EN_AON_MSK                     (((1U << AON_SW_WB_EN_AON_LEN) - 1) << AON_SW_WB_EN_AON_POS)
#define AON_SW_WB_EN_AON_UMSK                    (~(((1U << AON_SW_WB_EN_AON_LEN) - 1) << AON_SW_WB_EN_AON_POS))

/* 0x80C : bg_sys_top */
#define AON_BG_SYS_TOP_OFFSET                    (0x80C)

/* 0x810 : Digital Control */
#define AON_1_OFFSET                             (0x810)
#define AON_PU_PMIP_IREF_AON                     AON_PU_PMIP_IREF_AON
#define AON_PU_PMIP_IREF_AON_POS                 (10U)
#define AON_PU_PMIP_IREF_AON_LEN                 (1U)
#define AON_PU_PMIP_IREF_AON_MSK                 (((1U << AON_PU_PMIP_IREF_AON_LEN) - 1) << AON_PU_PMIP_IREF_AON_POS)
#define AON_PU_PMIP_IREF_AON_UMSK                (~(((1U << AON_PU_PMIP_IREF_AON_LEN) - 1) << AON_PU_PMIP_IREF_AON_POS))
#define AON_RESV                                 AON_RESV
#define AON_RESV_POS                             (12U)
#define AON_RESV_LEN                             (8U)
#define AON_RESV_MSK                             (((1U << AON_RESV_LEN) - 1) << AON_RESV_POS)
#define AON_RESV_UMSK                            (~(((1U << AON_RESV_LEN) - 1) << AON_RESV_POS))

/* 0x814 : Digital Control */
#define AON_LDO_SOC_OFFSET                       (0x814)
#define AON_LDO_SOC_CC_AON                       AON_LDO_SOC_CC_AON
#define AON_LDO_SOC_CC_AON_POS                   (4U)
#define AON_LDO_SOC_CC_AON_LEN                   (3U)
#define AON_LDO_SOC_CC_AON_MSK                   (((1U << AON_LDO_SOC_CC_AON_LEN) - 1) << AON_LDO_SOC_CC_AON_POS)
#define AON_LDO_SOC_CC_AON_UMSK                  (~(((1U << AON_LDO_SOC_CC_AON_LEN) - 1) << AON_LDO_SOC_CC_AON_POS))
#define AON_LDO_SOC_BM_AON                       AON_LDO_SOC_BM_AON
#define AON_LDO_SOC_BM_AON_POS                   (8U)
#define AON_LDO_SOC_BM_AON_LEN                   (2U)
#define AON_LDO_SOC_BM_AON_MSK                   (((1U << AON_LDO_SOC_BM_AON_LEN) - 1) << AON_LDO_SOC_BM_AON_POS)
#define AON_LDO_SOC_BM_AON_UMSK                  (~(((1U << AON_LDO_SOC_BM_AON_LEN) - 1) << AON_LDO_SOC_BM_AON_POS))
#define AON_LDO_SOC_POWER_GOOD_AON               AON_LDO_SOC_POWER_GOOD_AON
#define AON_LDO_SOC_POWER_GOOD_AON_POS           (20U)
#define AON_LDO_SOC_POWER_GOOD_AON_LEN           (1U)
#define AON_LDO_SOC_POWER_GOOD_AON_MSK           (((1U << AON_LDO_SOC_POWER_GOOD_AON_LEN) - 1) << AON_LDO_SOC_POWER_GOOD_AON_POS)
#define AON_LDO_SOC_POWER_GOOD_AON_UMSK          (~(((1U << AON_LDO_SOC_POWER_GOOD_AON_LEN) - 1) << AON_LDO_SOC_POWER_GOOD_AON_POS))
#define AON_LDO_SOC_RDY_AON                      AON_LDO_SOC_RDY_AON
#define AON_LDO_SOC_RDY_AON_POS                  (21U)
#define AON_LDO_SOC_RDY_AON_LEN                  (1U)
#define AON_LDO_SOC_RDY_AON_MSK                  (((1U << AON_LDO_SOC_RDY_AON_LEN) - 1) << AON_LDO_SOC_RDY_AON_POS)
#define AON_LDO_SOC_RDY_AON_UMSK                 (~(((1U << AON_LDO_SOC_RDY_AON_LEN) - 1) << AON_LDO_SOC_RDY_AON_POS))
#define AON_LDO_SOC_OTA_BM_AON                   AON_LDO_SOC_OTA_BM_AON
#define AON_LDO_SOC_OTA_BM_AON_POS               (24U)
#define AON_LDO_SOC_OTA_BM_AON_LEN               (2U)
#define AON_LDO_SOC_OTA_BM_AON_MSK               (((1U << AON_LDO_SOC_OTA_BM_AON_LEN) - 1) << AON_LDO_SOC_OTA_BM_AON_POS)
#define AON_LDO_SOC_OTA_BM_AON_UMSK              (~(((1U << AON_LDO_SOC_OTA_BM_AON_LEN) - 1) << AON_LDO_SOC_OTA_BM_AON_POS))

/* 0x818 : Digital Control */
#define AON_LDO13SYS_OFFSET                      (0x818)
#define AON_LDO13SYS_CC_AON                      AON_LDO13SYS_CC_AON
#define AON_LDO13SYS_CC_AON_POS                  (4U)
#define AON_LDO13SYS_CC_AON_LEN                  (3U)
#define AON_LDO13SYS_CC_AON_MSK                  (((1U << AON_LDO13SYS_CC_AON_LEN) - 1) << AON_LDO13SYS_CC_AON_POS)
#define AON_LDO13SYS_CC_AON_UMSK                 (~(((1U << AON_LDO13SYS_CC_AON_LEN) - 1) << AON_LDO13SYS_CC_AON_POS))
#define AON_LDO13SYS_BM_AON                      AON_LDO13SYS_BM_AON
#define AON_LDO13SYS_BM_AON_POS                  (8U)
#define AON_LDO13SYS_BM_AON_LEN                  (2U)
#define AON_LDO13SYS_BM_AON_MSK                  (((1U << AON_LDO13SYS_BM_AON_LEN) - 1) << AON_LDO13SYS_BM_AON_POS)
#define AON_LDO13SYS_BM_AON_UMSK                 (~(((1U << AON_LDO13SYS_BM_AON_LEN) - 1) << AON_LDO13SYS_BM_AON_POS))
#define AON_LDO13SYS_OCP_OUT_AON                 AON_LDO13SYS_OCP_OUT_AON
#define AON_LDO13SYS_OCP_OUT_AON_POS             (20U)
#define AON_LDO13SYS_OCP_OUT_AON_LEN             (1U)
#define AON_LDO13SYS_OCP_OUT_AON_MSK             (((1U << AON_LDO13SYS_OCP_OUT_AON_LEN) - 1) << AON_LDO13SYS_OCP_OUT_AON_POS)
#define AON_LDO13SYS_OCP_OUT_AON_UMSK            (~(((1U << AON_LDO13SYS_OCP_OUT_AON_LEN) - 1) << AON_LDO13SYS_OCP_OUT_AON_POS))
#define AON_LDO13SYS_OCP_EN_AON                  AON_LDO13SYS_OCP_EN_AON
#define AON_LDO13SYS_OCP_EN_AON_POS              (21U)
#define AON_LDO13SYS_OCP_EN_AON_LEN              (1U)
#define AON_LDO13SYS_OCP_EN_AON_MSK              (((1U << AON_LDO13SYS_OCP_EN_AON_LEN) - 1) << AON_LDO13SYS_OCP_EN_AON_POS)
#define AON_LDO13SYS_OCP_EN_AON_UMSK             (~(((1U << AON_LDO13SYS_OCP_EN_AON_LEN) - 1) << AON_LDO13SYS_OCP_EN_AON_POS))
#define AON_LDO13SYS_OCP_RST_AON                 AON_LDO13SYS_OCP_RST_AON
#define AON_LDO13SYS_OCP_RST_AON_POS             (22U)
#define AON_LDO13SYS_OCP_RST_AON_LEN             (1U)
#define AON_LDO13SYS_OCP_RST_AON_MSK             (((1U << AON_LDO13SYS_OCP_RST_AON_LEN) - 1) << AON_LDO13SYS_OCP_RST_AON_POS)
#define AON_LDO13SYS_OCP_RST_AON_UMSK            (~(((1U << AON_LDO13SYS_OCP_RST_AON_LEN) - 1) << AON_LDO13SYS_OCP_RST_AON_POS))
#define AON_LDO13SYS_OCP_TH_AON                  AON_LDO13SYS_OCP_TH_AON
#define AON_LDO13SYS_OCP_TH_AON_POS              (24U)
#define AON_LDO13SYS_OCP_TH_AON_LEN              (2U)
#define AON_LDO13SYS_OCP_TH_AON_MSK              (((1U << AON_LDO13SYS_OCP_TH_AON_LEN) - 1) << AON_LDO13SYS_OCP_TH_AON_POS)
#define AON_LDO13SYS_OCP_TH_AON_UMSK             (~(((1U << AON_LDO13SYS_OCP_TH_AON_LEN) - 1) << AON_LDO13SYS_OCP_TH_AON_POS))

/* 0x81C : Digital Control */
#define AON_LDO18IO_OFFSET                       (0x81C)
#define AON_LDO18IO_BYPASS_EN_AON                AON_LDO18IO_BYPASS_EN_AON
#define AON_LDO18IO_BYPASS_EN_AON_POS            (2U)
#define AON_LDO18IO_BYPASS_EN_AON_LEN            (1U)
#define AON_LDO18IO_BYPASS_EN_AON_MSK            (((1U << AON_LDO18IO_BYPASS_EN_AON_LEN) - 1) << AON_LDO18IO_BYPASS_EN_AON_POS)
#define AON_LDO18IO_BYPASS_EN_AON_UMSK           (~(((1U << AON_LDO18IO_BYPASS_EN_AON_LEN) - 1) << AON_LDO18IO_BYPASS_EN_AON_POS))
#define AON_LDO18IO_CC_AON                       AON_LDO18IO_CC_AON
#define AON_LDO18IO_CC_AON_POS                   (4U)
#define AON_LDO18IO_CC_AON_LEN                   (3U)
#define AON_LDO18IO_CC_AON_MSK                   (((1U << AON_LDO18IO_CC_AON_LEN) - 1) << AON_LDO18IO_CC_AON_POS)
#define AON_LDO18IO_CC_AON_UMSK                  (~(((1U << AON_LDO18IO_CC_AON_LEN) - 1) << AON_LDO18IO_CC_AON_POS))
#define AON_LDO18IO_BM_AON                       AON_LDO18IO_BM_AON
#define AON_LDO18IO_BM_AON_POS                   (8U)
#define AON_LDO18IO_BM_AON_LEN                   (2U)
#define AON_LDO18IO_BM_AON_MSK                   (((1U << AON_LDO18IO_BM_AON_LEN) - 1) << AON_LDO18IO_BM_AON_POS)
#define AON_LDO18IO_BM_AON_UMSK                  (~(((1U << AON_LDO18IO_BM_AON_LEN) - 1) << AON_LDO18IO_BM_AON_POS))
#define AON_LDO18IO_VOUT_SEL_AON                 AON_LDO18IO_VOUT_SEL_AON
#define AON_LDO18IO_VOUT_SEL_AON_POS             (12U)
#define AON_LDO18IO_VOUT_SEL_AON_LEN             (4U)
#define AON_LDO18IO_VOUT_SEL_AON_MSK             (((1U << AON_LDO18IO_VOUT_SEL_AON_LEN) - 1) << AON_LDO18IO_VOUT_SEL_AON_POS)
#define AON_LDO18IO_VOUT_SEL_AON_UMSK            (~(((1U << AON_LDO18IO_VOUT_SEL_AON_LEN) - 1) << AON_LDO18IO_VOUT_SEL_AON_POS))
#define AON_LDO18IO_VOUT_TRIM_AON                AON_LDO18IO_VOUT_TRIM_AON
#define AON_LDO18IO_VOUT_TRIM_AON_POS            (16U)
#define AON_LDO18IO_VOUT_TRIM_AON_LEN            (4U)
#define AON_LDO18IO_VOUT_TRIM_AON_MSK            (((1U << AON_LDO18IO_VOUT_TRIM_AON_LEN) - 1) << AON_LDO18IO_VOUT_TRIM_AON_POS)
#define AON_LDO18IO_VOUT_TRIM_AON_UMSK           (~(((1U << AON_LDO18IO_VOUT_TRIM_AON_LEN) - 1) << AON_LDO18IO_VOUT_TRIM_AON_POS))
#define AON_LDO18IO_OCP_OUT_AON                  AON_LDO18IO_OCP_OUT_AON
#define AON_LDO18IO_OCP_OUT_AON_POS              (20U)
#define AON_LDO18IO_OCP_OUT_AON_LEN              (1U)
#define AON_LDO18IO_OCP_OUT_AON_MSK              (((1U << AON_LDO18IO_OCP_OUT_AON_LEN) - 1) << AON_LDO18IO_OCP_OUT_AON_POS)
#define AON_LDO18IO_OCP_OUT_AON_UMSK             (~(((1U << AON_LDO18IO_OCP_OUT_AON_LEN) - 1) << AON_LDO18IO_OCP_OUT_AON_POS))
#define AON_LDO18IO_OCP_EN_AON                   AON_LDO18IO_OCP_EN_AON
#define AON_LDO18IO_OCP_EN_AON_POS               (21U)
#define AON_LDO18IO_OCP_EN_AON_LEN               (1U)
#define AON_LDO18IO_OCP_EN_AON_MSK               (((1U << AON_LDO18IO_OCP_EN_AON_LEN) - 1) << AON_LDO18IO_OCP_EN_AON_POS)
#define AON_LDO18IO_OCP_EN_AON_UMSK              (~(((1U << AON_LDO18IO_OCP_EN_AON_LEN) - 1) << AON_LDO18IO_OCP_EN_AON_POS))
#define AON_LDO18IO_OCP_RST_AON                  AON_LDO18IO_OCP_RST_AON
#define AON_LDO18IO_OCP_RST_AON_POS              (22U)
#define AON_LDO18IO_OCP_RST_AON_LEN              (1U)
#define AON_LDO18IO_OCP_RST_AON_MSK              (((1U << AON_LDO18IO_OCP_RST_AON_LEN) - 1) << AON_LDO18IO_OCP_RST_AON_POS)
#define AON_LDO18IO_OCP_RST_AON_UMSK             (~(((1U << AON_LDO18IO_OCP_RST_AON_LEN) - 1) << AON_LDO18IO_OCP_RST_AON_POS))
#define AON_LDO18IO_OCP_TH_AON                   AON_LDO18IO_OCP_TH_AON
#define AON_LDO18IO_OCP_TH_AON_POS               (24U)
#define AON_LDO18IO_OCP_TH_AON_LEN               (2U)
#define AON_LDO18IO_OCP_TH_AON_MSK               (((1U << AON_LDO18IO_OCP_TH_AON_LEN) - 1) << AON_LDO18IO_OCP_TH_AON_POS)
#define AON_LDO18IO_OCP_TH_AON_UMSK              (~(((1U << AON_LDO18IO_OCP_TH_AON_LEN) - 1) << AON_LDO18IO_OCP_TH_AON_POS))
#define AON_LDO18IO_SSTART_SEL_AON               AON_LDO18IO_SSTART_SEL_AON
#define AON_LDO18IO_SSTART_SEL_AON_POS           (26U)
#define AON_LDO18IO_SSTART_SEL_AON_LEN           (2U)
#define AON_LDO18IO_SSTART_SEL_AON_MSK           (((1U << AON_LDO18IO_SSTART_SEL_AON_LEN) - 1) << AON_LDO18IO_SSTART_SEL_AON_POS)
#define AON_LDO18IO_SSTART_SEL_AON_UMSK          (~(((1U << AON_LDO18IO_SSTART_SEL_AON_LEN) - 1) << AON_LDO18IO_SSTART_SEL_AON_POS))

/* 0x820 : Digital Control */
#define AON_PSW_PMIP_RESV_OFFSET                 (0x820)
#define AON_PU_PSW18_FLASH_AON                   AON_PU_PSW18_FLASH_AON
#define AON_PU_PSW18_FLASH_AON_POS               (17U)
#define AON_PU_PSW18_FLASH_AON_LEN               (1U)
#define AON_PU_PSW18_FLASH_AON_MSK               (((1U << AON_PU_PSW18_FLASH_AON_LEN) - 1) << AON_PU_PSW18_FLASH_AON_POS)
#define AON_PU_PSW18_FLASH_AON_UMSK              (~(((1U << AON_PU_PSW18_FLASH_AON_LEN) - 1) << AON_PU_PSW18_FLASH_AON_POS))
#define AON_PU_PSW18_PSRAM_AON                   AON_PU_PSW18_PSRAM_AON
#define AON_PU_PSW18_PSRAM_AON_POS               (18U)
#define AON_PU_PSW18_PSRAM_AON_LEN               (1U)
#define AON_PU_PSW18_PSRAM_AON_MSK               (((1U << AON_PU_PSW18_PSRAM_AON_LEN) - 1) << AON_PU_PSW18_PSRAM_AON_POS)
#define AON_PU_PSW18_PSRAM_AON_UMSK              (~(((1U << AON_PU_PSW18_PSRAM_AON_LEN) - 1) << AON_PU_PSW18_PSRAM_AON_POS))
#define AON_PU_PSW18_EFUSE_AON                   AON_PU_PSW18_EFUSE_AON
#define AON_PU_PSW18_EFUSE_AON_POS               (19U)
#define AON_PU_PSW18_EFUSE_AON_LEN               (1U)
#define AON_PU_PSW18_EFUSE_AON_MSK               (((1U << AON_PU_PSW18_EFUSE_AON_LEN) - 1) << AON_PU_PSW18_EFUSE_AON_POS)
#define AON_PU_PSW18_EFUSE_AON_UMSK              (~(((1U << AON_PU_PSW18_EFUSE_AON_LEN) - 1) << AON_PU_PSW18_EFUSE_AON_POS))
#define AON_PMIP_RESV_AON                        AON_PMIP_RESV_AON
#define AON_PMIP_RESV_AON_POS                    (20U)
#define AON_PMIP_RESV_AON_LEN                    (8U)
#define AON_PMIP_RESV_AON_MSK                    (((1U << AON_PMIP_RESV_AON_LEN) - 1) << AON_PMIP_RESV_AON_POS)
#define AON_PMIP_RESV_AON_UMSK                   (~(((1U << AON_PMIP_RESV_AON_LEN) - 1) << AON_PMIP_RESV_AON_POS))

/* 0x824 : Digital Control */
#define AON_TEN_OFFSET                           (0x824)
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
#define AON_PMIP_DC_TP_INT_OUT_EN_AON            AON_PMIP_DC_TP_INT_OUT_EN_AON
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_POS        (5U)
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_LEN        (1U)
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_MSK        (((1U << AON_PMIP_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_PMIP_DC_TP_INT_OUT_EN_AON_POS)
#define AON_PMIP_DC_TP_INT_OUT_EN_AON_UMSK       (~(((1U << AON_PMIP_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_PMIP_DC_TP_INT_OUT_EN_AON_POS))
#define AON_DC_TP_OUT_EN_AON                     AON_DC_TP_OUT_EN_AON
#define AON_DC_TP_OUT_EN_AON_POS                 (6U)
#define AON_DC_TP_OUT_EN_AON_LEN                 (1U)
#define AON_DC_TP_OUT_EN_AON_MSK                 (((1U << AON_DC_TP_OUT_EN_AON_LEN) - 1) << AON_DC_TP_OUT_EN_AON_POS)
#define AON_DC_TP_OUT_EN_AON_UMSK                (~(((1U << AON_DC_TP_OUT_EN_AON_LEN) - 1) << AON_DC_TP_OUT_EN_AON_POS))
#define AON_DC_TP_INT_OUT_EN_AON                 AON_DC_TP_INT_OUT_EN_AON
#define AON_DC_TP_INT_OUT_EN_AON_POS             (7U)
#define AON_DC_TP_INT_OUT_EN_AON_LEN             (1U)
#define AON_DC_TP_INT_OUT_EN_AON_MSK             (((1U << AON_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_DC_TP_INT_OUT_EN_AON_POS)
#define AON_DC_TP_INT_OUT_EN_AON_UMSK            (~(((1U << AON_DC_TP_INT_OUT_EN_AON_LEN) - 1) << AON_DC_TP_INT_OUT_EN_AON_POS))
#define AON_TEN_AON                              AON_TEN_AON
#define AON_TEN_AON_POS                          (8U)
#define AON_TEN_AON_LEN                          (1U)
#define AON_TEN_AON_MSK                          (((1U << AON_TEN_AON_LEN) - 1) << AON_TEN_AON_POS)
#define AON_TEN_AON_UMSK                         (~(((1U << AON_TEN_AON_LEN) - 1) << AON_TEN_AON_POS))
#define AON_TEN_LDO13SYS_AON                     AON_TEN_LDO13SYS_AON
#define AON_TEN_LDO13SYS_AON_POS                 (9U)
#define AON_TEN_LDO13SYS_AON_LEN                 (1U)
#define AON_TEN_LDO13SYS_AON_MSK                 (((1U << AON_TEN_LDO13SYS_AON_LEN) - 1) << AON_TEN_LDO13SYS_AON_POS)
#define AON_TEN_LDO13SYS_AON_UMSK                (~(((1U << AON_TEN_LDO13SYS_AON_LEN) - 1) << AON_TEN_LDO13SYS_AON_POS))
#define AON_TEN_LDO18IO_AON                      AON_TEN_LDO18IO_AON
#define AON_TEN_LDO18IO_AON_POS                  (10U)
#define AON_TEN_LDO18IO_AON_LEN                  (1U)
#define AON_TEN_LDO18IO_AON_MSK                  (((1U << AON_TEN_LDO18IO_AON_LEN) - 1) << AON_TEN_LDO18IO_AON_POS)
#define AON_TEN_LDO18IO_AON_UMSK                 (~(((1U << AON_TEN_LDO18IO_AON_LEN) - 1) << AON_TEN_LDO18IO_AON_POS))
#define AON_TEN_LDO_SOC_AON                      AON_TEN_LDO_SOC_AON
#define AON_TEN_LDO_SOC_AON_POS                  (13U)
#define AON_TEN_LDO_SOC_AON_LEN                  (1U)
#define AON_TEN_LDO_SOC_AON_MSK                  (((1U << AON_TEN_LDO_SOC_AON_LEN) - 1) << AON_TEN_LDO_SOC_AON_POS)
#define AON_TEN_LDO_SOC_AON_UMSK                 (~(((1U << AON_TEN_LDO_SOC_AON_LEN) - 1) << AON_TEN_LDO_SOC_AON_POS))

/* 0x828 :  */
#define AON_XTAL_PU_OFFSET                       (0x828)
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
#define AON_XTAL_RDY                             AON_XTAL_RDY
#define AON_XTAL_RDY_POS                         (24U)
#define AON_XTAL_RDY_LEN                         (1U)
#define AON_XTAL_RDY_MSK                         (((1U << AON_XTAL_RDY_LEN) - 1) << AON_XTAL_RDY_POS)
#define AON_XTAL_RDY_UMSK                        (~(((1U << AON_XTAL_RDY_LEN) - 1) << AON_XTAL_RDY_POS))

/* 0x82C :  */
#define AON_XTAL_LDO_OFFSET                      (0x82C)
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
#define AON_XTAL_VGATE_FILTER_AON                AON_XTAL_VGATE_FILTER_AON
#define AON_XTAL_VGATE_FILTER_AON_POS            (12U)
#define AON_XTAL_VGATE_FILTER_AON_LEN            (2U)
#define AON_XTAL_VGATE_FILTER_AON_MSK            (((1U << AON_XTAL_VGATE_FILTER_AON_LEN) - 1) << AON_XTAL_VGATE_FILTER_AON_POS)
#define AON_XTAL_VGATE_FILTER_AON_UMSK           (~(((1U << AON_XTAL_VGATE_FILTER_AON_LEN) - 1) << AON_XTAL_VGATE_FILTER_AON_POS))

/* 0x830 :  */
#define AON_XTAL_CORE0_OFFSET                    (0x830)
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

/* 0x834 :  */
#define AON_XTAL_CORE1_OFFSET                    (0x834)
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
#define AON_XTAL_BUFF_VG_SEL_AON                 AON_XTAL_BUFF_VG_SEL_AON
#define AON_XTAL_BUFF_VG_SEL_AON_POS             (6U)
#define AON_XTAL_BUFF_VG_SEL_AON_LEN             (2U)
#define AON_XTAL_BUFF_VG_SEL_AON_MSK             (((1U << AON_XTAL_BUFF_VG_SEL_AON_LEN) - 1) << AON_XTAL_BUFF_VG_SEL_AON_POS)
#define AON_XTAL_BUFF_VG_SEL_AON_UMSK            (~(((1U << AON_XTAL_BUFF_VG_SEL_AON_LEN) - 1) << AON_XTAL_BUFF_VG_SEL_AON_POS))
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON      AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_POS  (8U)
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_LEN  (1U)
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_MSK  (((1U << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_POS)
#define AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_UMSK (~(((1U << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_LEN) - 1) << AON_XTAL_CAPCODE_EXTRA_OUT_FAST_AON_POS))
#define AON_XTAL_SOCBUF_SEL_AON                  AON_XTAL_SOCBUF_SEL_AON
#define AON_XTAL_SOCBUF_SEL_AON_POS              (9U)
#define AON_XTAL_SOCBUF_SEL_AON_LEN              (1U)
#define AON_XTAL_SOCBUF_SEL_AON_MSK              (((1U << AON_XTAL_SOCBUF_SEL_AON_LEN) - 1) << AON_XTAL_SOCBUF_SEL_AON_POS)
#define AON_XTAL_SOCBUF_SEL_AON_UMSK             (~(((1U << AON_XTAL_SOCBUF_SEL_AON_LEN) - 1) << AON_XTAL_SOCBUF_SEL_AON_POS))
#define AON_XTAL_LP_STUP_SEL_AON                 AON_XTAL_LP_STUP_SEL_AON
#define AON_XTAL_LP_STUP_SEL_AON_POS             (10U)
#define AON_XTAL_LP_STUP_SEL_AON_LEN             (2U)
#define AON_XTAL_LP_STUP_SEL_AON_MSK             (((1U << AON_XTAL_LP_STUP_SEL_AON_LEN) - 1) << AON_XTAL_LP_STUP_SEL_AON_POS)
#define AON_XTAL_LP_STUP_SEL_AON_UMSK            (~(((1U << AON_XTAL_LP_STUP_SEL_AON_LEN) - 1) << AON_XTAL_LP_STUP_SEL_AON_POS))
#define AON_XTAL_LP_GM_BOOST_AON                 AON_XTAL_LP_GM_BOOST_AON
#define AON_XTAL_LP_GM_BOOST_AON_POS             (12U)
#define AON_XTAL_LP_GM_BOOST_AON_LEN             (2U)
#define AON_XTAL_LP_GM_BOOST_AON_MSK             (((1U << AON_XTAL_LP_GM_BOOST_AON_LEN) - 1) << AON_XTAL_LP_GM_BOOST_AON_POS)
#define AON_XTAL_LP_GM_BOOST_AON_UMSK            (~(((1U << AON_XTAL_LP_GM_BOOST_AON_LEN) - 1) << AON_XTAL_LP_GM_BOOST_AON_POS))
#define AON_XTAL_LP_EN_AON                       AON_XTAL_LP_EN_AON
#define AON_XTAL_LP_EN_AON_POS                   (14U)
#define AON_XTAL_LP_EN_AON_LEN                   (1U)
#define AON_XTAL_LP_EN_AON_MSK                   (((1U << AON_XTAL_LP_EN_AON_LEN) - 1) << AON_XTAL_LP_EN_AON_POS)
#define AON_XTAL_LP_EN_AON_UMSK                  (~(((1U << AON_XTAL_LP_EN_AON_LEN) - 1) << AON_XTAL_LP_EN_AON_POS))
#define AON_PU_XTAL_LP_AON                       AON_PU_XTAL_LP_AON
#define AON_PU_XTAL_LP_AON_POS                   (15U)
#define AON_PU_XTAL_LP_AON_LEN                   (1U)
#define AON_PU_XTAL_LP_AON_MSK                   (((1U << AON_PU_XTAL_LP_AON_LEN) - 1) << AON_PU_XTAL_LP_AON_POS)
#define AON_PU_XTAL_LP_AON_UMSK                  (~(((1U << AON_PU_XTAL_LP_AON_LEN) - 1) << AON_PU_XTAL_LP_AON_POS))
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

/* 0x838 :  */
#define AON_XTAL_CORE2_OFFSET                    (0x838)
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

/* 0x83C :  */
#define AON_XTAL_BUFFER_OFFSET                   (0x83C)
#define AON_XTAL_LOCK_SEL_X2_AON                 AON_XTAL_LOCK_SEL_X2_AON
#define AON_XTAL_LOCK_SEL_X2_AON_POS             (0U)
#define AON_XTAL_LOCK_SEL_X2_AON_LEN             (1U)
#define AON_XTAL_LOCK_SEL_X2_AON_MSK             (((1U << AON_XTAL_LOCK_SEL_X2_AON_LEN) - 1) << AON_XTAL_LOCK_SEL_X2_AON_POS)
#define AON_XTAL_LOCK_SEL_X2_AON_UMSK            (~(((1U << AON_XTAL_LOCK_SEL_X2_AON_LEN) - 1) << AON_XTAL_LOCK_SEL_X2_AON_POS))
#define AON_XTAL_LOCK_SEL_DIV2_AON               AON_XTAL_LOCK_SEL_DIV2_AON
#define AON_XTAL_LOCK_SEL_DIV2_AON_POS           (4U)
#define AON_XTAL_LOCK_SEL_DIV2_AON_LEN           (1U)
#define AON_XTAL_LOCK_SEL_DIV2_AON_MSK           (((1U << AON_XTAL_LOCK_SEL_DIV2_AON_LEN) - 1) << AON_XTAL_LOCK_SEL_DIV2_AON_POS)
#define AON_XTAL_LOCK_SEL_DIV2_AON_UMSK          (~(((1U << AON_XTAL_LOCK_SEL_DIV2_AON_LEN) - 1) << AON_XTAL_LOCK_SEL_DIV2_AON_POS))
#define AON_XTAL_LOCK_INV_AON                    AON_XTAL_LOCK_INV_AON
#define AON_XTAL_LOCK_INV_AON_POS                (5U)
#define AON_XTAL_LOCK_INV_AON_LEN                (1U)
#define AON_XTAL_LOCK_INV_AON_MSK                (((1U << AON_XTAL_LOCK_INV_AON_LEN) - 1) << AON_XTAL_LOCK_INV_AON_POS)
#define AON_XTAL_LOCK_INV_AON_UMSK               (~(((1U << AON_XTAL_LOCK_INV_AON_LEN) - 1) << AON_XTAL_LOCK_INV_AON_POS))
#define AON_XTAL_PD_HP_BUF_ENB_AON               AON_XTAL_PD_HP_BUF_ENB_AON
#define AON_XTAL_PD_HP_BUF_ENB_AON_POS           (6U)
#define AON_XTAL_PD_HP_BUF_ENB_AON_LEN           (1U)
#define AON_XTAL_PD_HP_BUF_ENB_AON_MSK           (((1U << AON_XTAL_PD_HP_BUF_ENB_AON_LEN) - 1) << AON_XTAL_PD_HP_BUF_ENB_AON_POS)
#define AON_XTAL_PD_HP_BUF_ENB_AON_UMSK          (~(((1U << AON_XTAL_PD_HP_BUF_ENB_AON_LEN) - 1) << AON_XTAL_PD_HP_BUF_ENB_AON_POS))
#define AON_XTAL_LO_BUF_LATCH_EN_AON             AON_XTAL_LO_BUF_LATCH_EN_AON
#define AON_XTAL_LO_BUF_LATCH_EN_AON_POS         (8U)
#define AON_XTAL_LO_BUF_LATCH_EN_AON_LEN         (1U)
#define AON_XTAL_LO_BUF_LATCH_EN_AON_MSK         (((1U << AON_XTAL_LO_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_LO_BUF_LATCH_EN_AON_POS)
#define AON_XTAL_LO_BUF_LATCH_EN_AON_UMSK        (~(((1U << AON_XTAL_LO_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_LO_BUF_LATCH_EN_AON_POS))
#define AON_XTAL_LO_BUF_EN_AON                   AON_XTAL_LO_BUF_EN_AON
#define AON_XTAL_LO_BUF_EN_AON_POS               (12U)
#define AON_XTAL_LO_BUF_EN_AON_LEN               (1U)
#define AON_XTAL_LO_BUF_EN_AON_MSK               (((1U << AON_XTAL_LO_BUF_EN_AON_LEN) - 1) << AON_XTAL_LO_BUF_EN_AON_POS)
#define AON_XTAL_LO_BUF_EN_AON_UMSK              (~(((1U << AON_XTAL_LO_BUF_EN_AON_LEN) - 1) << AON_XTAL_LO_BUF_EN_AON_POS))
#define AON_XTAL_USB2_BUF_DRV_SEL_AON            AON_XTAL_USB2_BUF_DRV_SEL_AON
#define AON_XTAL_USB2_BUF_DRV_SEL_AON_POS        (16U)
#define AON_XTAL_USB2_BUF_DRV_SEL_AON_LEN        (1U)
#define AON_XTAL_USB2_BUF_DRV_SEL_AON_MSK        (((1U << AON_XTAL_USB2_BUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_USB2_BUF_DRV_SEL_AON_POS)
#define AON_XTAL_USB2_BUF_DRV_SEL_AON_UMSK       (~(((1U << AON_XTAL_USB2_BUF_DRV_SEL_AON_LEN) - 1) << AON_XTAL_USB2_BUF_DRV_SEL_AON_POS))
#define AON_XTAL_USB2_BUF_LATCH_EN_AON           AON_XTAL_USB2_BUF_LATCH_EN_AON
#define AON_XTAL_USB2_BUF_LATCH_EN_AON_POS       (20U)
#define AON_XTAL_USB2_BUF_LATCH_EN_AON_LEN       (1U)
#define AON_XTAL_USB2_BUF_LATCH_EN_AON_MSK       (((1U << AON_XTAL_USB2_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_LATCH_EN_AON_POS)
#define AON_XTAL_USB2_BUF_LATCH_EN_AON_UMSK      (~(((1U << AON_XTAL_USB2_BUF_LATCH_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_LATCH_EN_AON_POS))
#define AON_XTAL_USB2_BUF_DUM_EN_AON             AON_XTAL_USB2_BUF_DUM_EN_AON
#define AON_XTAL_USB2_BUF_DUM_EN_AON_POS         (24U)
#define AON_XTAL_USB2_BUF_DUM_EN_AON_LEN         (1U)
#define AON_XTAL_USB2_BUF_DUM_EN_AON_MSK         (((1U << AON_XTAL_USB2_BUF_DUM_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_DUM_EN_AON_POS)
#define AON_XTAL_USB2_BUF_DUM_EN_AON_UMSK        (~(((1U << AON_XTAL_USB2_BUF_DUM_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_DUM_EN_AON_POS))
#define AON_XTAL_USB2_BUF_EN_AON                 AON_XTAL_USB2_BUF_EN_AON
#define AON_XTAL_USB2_BUF_EN_AON_POS             (28U)
#define AON_XTAL_USB2_BUF_EN_AON_LEN             (1U)
#define AON_XTAL_USB2_BUF_EN_AON_MSK             (((1U << AON_XTAL_USB2_BUF_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_EN_AON_POS)
#define AON_XTAL_USB2_BUF_EN_AON_UMSK            (~(((1U << AON_XTAL_USB2_BUF_EN_AON_LEN) - 1) << AON_XTAL_USB2_BUF_EN_AON_POS))
#define AON_XTAL_EXT_BUF_EN_AON                  AON_XTAL_EXT_BUF_EN_AON
#define AON_XTAL_EXT_BUF_EN_AON_POS              (31U)
#define AON_XTAL_EXT_BUF_EN_AON_LEN              (1U)
#define AON_XTAL_EXT_BUF_EN_AON_MSK              (((1U << AON_XTAL_EXT_BUF_EN_AON_LEN) - 1) << AON_XTAL_EXT_BUF_EN_AON_POS)
#define AON_XTAL_EXT_BUF_EN_AON_UMSK             (~(((1U << AON_XTAL_EXT_BUF_EN_AON_LEN) - 1) << AON_XTAL_EXT_BUF_EN_AON_POS))

/* 0x840 :  */
#define AON_XTAL_DIG_OFFSET                      (0x840)
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

/* 0x844 :  */
#define AON_XTAL_TEST_OFFSET                     (0x844)
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

/* 0x848 : tsen */
#define AON_TSEN_OFFSET                          (0x848)
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

/* 0x84C : psw_irrcv */
#define AON_PSW_IRRCV_OFFSET                     (0x84C)
#define AON_PU_IR_PSW_AON                        AON_PU_IR_PSW_AON
#define AON_PU_IR_PSW_AON_POS                    (0U)
#define AON_PU_IR_PSW_AON_LEN                    (1U)
#define AON_PU_IR_PSW_AON_MSK                    (((1U << AON_PU_IR_PSW_AON_LEN) - 1) << AON_PU_IR_PSW_AON_POS)
#define AON_PU_IR_PSW_AON_UMSK                   (~(((1U << AON_PU_IR_PSW_AON_LEN) - 1) << AON_PU_IR_PSW_AON_POS))

/* 0x850 : vddcore_psw */
#define AON_VDDCORE_PSW_OFFSET                   (0x850)

/* 0x854 : psw_misc */
#define AON_PSW_MISC_OFFSET                      (0x854)
#define AON_PU_PSW_IRRCV_AON                     AON_PU_PSW_IRRCV_AON
#define AON_PU_PSW_IRRCV_AON_POS                 (0U)
#define AON_PU_PSW_IRRCV_AON_LEN                 (1U)
#define AON_PU_PSW_IRRCV_AON_MSK                 (((1U << AON_PU_PSW_IRRCV_AON_LEN) - 1) << AON_PU_PSW_IRRCV_AON_POS)
#define AON_PU_PSW_IRRCV_AON_UMSK                (~(((1U << AON_PU_PSW_IRRCV_AON_LEN) - 1) << AON_PU_PSW_IRRCV_AON_POS))
#define AON_EN_POR33_AON                         AON_EN_POR33_AON
#define AON_EN_POR33_AON_POS                     (20U)
#define AON_EN_POR33_AON_LEN                     (1U)
#define AON_EN_POR33_AON_MSK                     (((1U << AON_EN_POR33_AON_LEN) - 1) << AON_EN_POR33_AON_POS)
#define AON_EN_POR33_AON_UMSK                    (~(((1U << AON_EN_POR33_AON_LEN) - 1) << AON_EN_POR33_AON_POS))

/* 0x93C : hbncore_resv0 */
#define AON_HBNCORE_RESV0_OFFSET                 (0x93C)
#define AON_HBNCORE_RESV0_DATA                   AON_HBNCORE_RESV0_DATA
#define AON_HBNCORE_RESV0_DATA_POS               (0U)
#define AON_HBNCORE_RESV0_DATA_LEN               (32U)
#define AON_HBNCORE_RESV0_DATA_MSK               (((1U << AON_HBNCORE_RESV0_DATA_LEN) - 1) << AON_HBNCORE_RESV0_DATA_POS)
#define AON_HBNCORE_RESV0_DATA_UMSK              (~(((1U << AON_HBNCORE_RESV0_DATA_LEN) - 1) << AON_HBNCORE_RESV0_DATA_POS))

/* 0x940 : hbncore_resv1 */
#define AON_HBNCORE_RESV1_OFFSET                 (0x940)
#define AON_HBNCORE_RESV1_DATA                   AON_HBNCORE_RESV1_DATA
#define AON_HBNCORE_RESV1_DATA_POS               (0U)
#define AON_HBNCORE_RESV1_DATA_LEN               (32U)
#define AON_HBNCORE_RESV1_DATA_MSK               (((1U << AON_HBNCORE_RESV1_DATA_LEN) - 1) << AON_HBNCORE_RESV1_DATA_POS)
#define AON_HBNCORE_RESV1_DATA_UMSK              (~(((1U << AON_HBNCORE_RESV1_DATA_LEN) - 1) << AON_HBNCORE_RESV1_DATA_POS))

struct aon_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[2048];

    /* 0x800 : aon */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon;

    /* 0x804  reserved */
    uint8_t RESERVED0x804[4];

    /* 0x808 : aon_misc */
    union {
        struct {
            uint32_t sw_soc_en_aon : 1;  /* [    0],        r/w,        0x1 */
            uint32_t sw_wb_en_aon  : 1;  /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_31 : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon_misc;

    /* 0x80C : bg_sys_top */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } bg_sys_top;

    /* 0x810 : Digital Control */
    union {
        struct {
            uint32_t reserved_0_9     : 10; /* [ 9: 0],       rsvd,        0x0 */
            uint32_t pu_pmip_iref_aon : 1;  /* [   10],        r/w,        0x1 */
            uint32_t reserved_11      : 1;  /* [   11],       rsvd,        0x0 */
            uint32_t aon_resv         : 8;  /* [19:12],        r/w,       0x55 */
            uint32_t reserved_20_31   : 12; /* [31:20],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } aon_1;

    /* 0x814 : Digital Control */
    union {
        struct {
            uint32_t reserved_0_3           : 4;  /* [ 3: 0],       rsvd,        0x0 */
            uint32_t ldo_soc_cc_aon         : 3;  /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7             : 1;  /* [    7],       rsvd,        0x0 */
            uint32_t ldo_soc_bm_aon         : 2;  /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_19         : 10; /* [19:10],       rsvd,        0x0 */
            uint32_t ldo_soc_power_good_aon : 1;  /* [   20],          r,        0x0 */
            uint32_t ldo_soc_rdy_aon        : 1;  /* [   21],          r,        0x0 */
            uint32_t reserved_22_23         : 2;  /* [23:22],       rsvd,        0x0 */
            uint32_t ldo_soc_ota_bm_aon     : 2;  /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_31         : 6;  /* [31:26],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ldo_soc;

    /* 0x818 : Digital Control */
    union {
        struct {
            uint32_t reserved_0_3         : 4;  /* [ 3: 0],       rsvd,        0x0 */
            uint32_t ldo13sys_cc_aon      : 3;  /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7           : 1;  /* [    7],       rsvd,        0x0 */
            uint32_t ldo13sys_bm_aon      : 2;  /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_19       : 10; /* [19:10],       rsvd,        0x0 */
            uint32_t ldo13sys_ocp_out_aon : 1;  /* [   20],          r,        0x0 */
            uint32_t ldo13sys_ocp_en_aon  : 1;  /* [   21],        r/w,        0x1 */
            uint32_t ldo13sys_ocp_rst_aon : 1;  /* [   22],        r/w,        0x0 */
            uint32_t reserved_23          : 1;  /* [   23],       rsvd,        0x0 */
            uint32_t ldo13sys_ocp_th_aon  : 2;  /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_31       : 6;  /* [31:26],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ldo13sys;

    /* 0x81C : Digital Control */
    union {
        struct {
            uint32_t reserved_0_1           : 2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t ldo18io_bypass_en_aon  : 1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3             : 1; /* [    3],       rsvd,        0x0 */
            uint32_t ldo18io_cc_aon         : 3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7             : 1; /* [    7],       rsvd,        0x0 */
            uint32_t ldo18io_bm_aon         : 2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_11         : 2; /* [11:10],       rsvd,        0x0 */
            uint32_t ldo18io_vout_sel_aon   : 4; /* [15:12],        r/w,        0x3 */
            uint32_t ldo18io_vout_trim_aon  : 4; /* [19:16],        r/w,        0x7 */
            uint32_t ldo18io_ocp_out_aon    : 1; /* [   20],          r,        0x0 */
            uint32_t ldo18io_ocp_en_aon     : 1; /* [   21],        r/w,        0x1 */
            uint32_t ldo18io_ocp_rst_aon    : 1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23            : 1; /* [   23],       rsvd,        0x0 */
            uint32_t ldo18io_ocp_th_aon     : 2; /* [25:24],        r/w,        0x1 */
            uint32_t ldo18io_sstart_sel_aon : 2; /* [27:26],        r/w,        0x0 */
            uint32_t reserved_28_31         : 4; /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ldo18io;

    /* 0x820 : Digital Control */
    union {
        struct {
            uint32_t reserved_0_16      : 17; /* [16: 0],       rsvd,        0x0 */
            uint32_t pu_psw18_flash_aon : 1;  /* [   17],        r/w,        0x1 */
            uint32_t pu_psw18_psram_aon : 1;  /* [   18],        r/w,        0x1 */
            uint32_t pu_psw18_efuse_aon : 1;  /* [   19],        r/w,        0x0 */
            uint32_t pmip_resv_aon      : 8;  /* [27:20],        r/w,       0x55 */
            uint32_t reserved_28_31     : 4;  /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } psw_pmip_resv;

    /* 0x824 : Digital Control */
    union {
        struct {
            uint32_t tmux_aon                  : 3;  /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                : 1;  /* [    3],       rsvd,        0x0 */
            uint32_t pmip_dc_tp_out_en_aon     : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pmip_dc_tp_int_out_en_aon : 1;  /* [    5],        r/w,        0x0 */
            uint32_t aon_dc_tp_out_en_aon      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t aon_dc_tp_int_out_en_aon  : 1;  /* [    7],        r/w,        0x0 */
            uint32_t ten_aon                   : 1;  /* [    8],        r/w,        0x0 */
            uint32_t ten_ldo13sys_aon          : 1;  /* [    9],        r/w,        0x0 */
            uint32_t ten_ldo18io_aon           : 1;  /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_12            : 2;  /* [12:11],       rsvd,        0x0 */
            uint32_t ten_ldo_soc_aon           : 1;  /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_31            : 18; /* [31:14],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ten;

    /* 0x828 :  */
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
            uint32_t pud_dly_xtal_aon : 3; /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19      : 1; /* [   19],       rsvd,        0x0 */
            uint32_t xtal_reserve_aon : 4; /* [23:20],        r/w,        0xc */
            uint32_t xtal_rdy         : 1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31   : 7; /* [31:25],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_pu;

    /* 0x82C :  */
    union {
        struct {
            uint32_t xtal_buf_power_boost_aon  : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3              : 3;  /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_core_power_boost_aon : 1;  /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_7              : 3;  /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_ldo_bypass_aon       : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11             : 3;  /* [11: 9],       rsvd,        0x0 */
            uint32_t xtal_vgate_filter_aon     : 2;  /* [13:12],        r/w,        0x3 */
            uint32_t reserved_14_31            : 18; /* [31:14],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_ldo;

    /* 0x830 :  */
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
            uint32_t xtal_capcode_in_aon        : 8; /* [23:16],        r/w,       0xb4 */
            uint32_t xtal_capcode_out_aon       : 8; /* [31:24],        r/w,       0xb4 */
        } BF;
        uint32_t WORD;
    } xtal_core0;

    /* 0x834 :  */
    union {
        struct {
            uint32_t xtal_capcode_unbalance_aon      : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                    : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_capcode_extra_in_fast_aon  : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5                      : 1; /* [    5],       rsvd,        0x0 */
            uint32_t xtal_buff_vg_sel_aon            : 2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t xtal_capcode_extra_out_fast_aon : 1; /* [    8],        r/w,        0x0 */
            uint32_t xtal_socbuf_sel_aon             : 1; /* [    9],        r/w,        0x0 */
            uint32_t xtal_lp_stup_sel_aon            : 2; /* [11:10],        r/w,        0x1 */
            uint32_t xtal_lp_gm_boost_aon            : 2; /* [13:12],        r/w,        0x1 */
            uint32_t xtal_lp_en_aon                  : 1; /* [   14],        r/w,        0x0 */
            uint32_t pu_xtal_lp_aon                  : 1; /* [   15],        r/w,        0x0 */
            uint32_t xtal_capcode_in_fast_aon        : 8; /* [23:16],        r/w,        0x0 */
            uint32_t xtal_capcode_out_fast_aon       : 8; /* [31:24],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_core1;

    /* 0x838 :  */
    union {
        struct {
            uint32_t xtal_capcode_unbalance_fast_aon : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                    : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_ext_sel_aon                : 1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                    : 3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t xtal_gm_boost_fast_aon          : 2; /* [ 9: 8],        r/w,        0x3 */
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

    /* 0x83C :  */
    union {
        struct {
            uint32_t xtal_lock_sel_x2_aon       : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3               : 3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t xtal_lock_sel_div2_aon     : 1; /* [    4],        r/w,        0x0 */
            uint32_t xtal_lock_inv_aon          : 1; /* [    5],        r/w,        0x0 */
            uint32_t xtal_pd_hp_buf_enb_aon     : 1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                 : 1; /* [    7],       rsvd,        0x0 */
            uint32_t xtal_lo_buf_latch_en_aon   : 1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11              : 3; /* [11: 9],       rsvd,        0x0 */
            uint32_t xtal_lo_buf_en_aon         : 1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_15             : 3; /* [15:13],       rsvd,        0x0 */
            uint32_t xtal_usb2_buf_drv_sel_aon  : 1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19             : 3; /* [19:17],       rsvd,        0x0 */
            uint32_t xtal_usb2_buf_latch_en_aon : 1; /* [   20],        r/w,        0x1 */
            uint32_t reserved_21_23             : 3; /* [23:21],       rsvd,        0x0 */
            uint32_t xtal_usb2_buf_dum_en_aon   : 1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27             : 3; /* [27:25],       rsvd,        0x0 */
            uint32_t xtal_usb2_buf_en_aon       : 1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_30             : 2; /* [30:29],       rsvd,        0x0 */
            uint32_t xtal_ext_buf_en_aon        : 1; /* [   31],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_buffer;

    /* 0x840 :  */
    union {
        struct {
            uint32_t xtal_rdy_int_sel_aon : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3         : 2;  /* [ 3: 2],       rsvd,        0x0 */
            uint32_t xtal_rdy_sel_aon     : 2;  /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_31        : 26; /* [31: 6],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } xtal_dig;

    /* 0x844 :  */
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

    /* 0x848 : tsen */
    union {
        struct {
            uint32_t tsen_refcode_corner : 12; /* [11: 0],        r/w,      0x8ff */
            uint32_t reserved_12_15      : 4;  /* [15:12],       rsvd,        0x0 */
            uint32_t tsen_refcode_rfcal  : 12; /* [27:16],        r/w,      0x8ff */
            uint32_t reserved_28_31      : 4;  /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } tsen;

    /* 0x84C : psw_irrcv */
    union {
        struct {
            uint32_t pu_ir_psw_aon : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } psw_irrcv;

    /* 0x850 : vddcore_psw */
    union {
        struct {
            uint32_t reserved_0_31 : 32; /* [31: 0],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } vddcore_psw;

    /* 0x854 : psw_misc */
    union {
        struct {
            uint32_t pu_psw_irrcv_aon : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_19    : 19; /* [19: 1],       rsvd,        0x0 */
            uint32_t en_por33_aon     : 1;  /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31   : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } psw_misc;

    /* 0x858  reserved */
    uint8_t RESERVED0x858[228];

    /* 0x93C : hbncore_resv0 */
    union {
        struct {
            uint32_t hbncore_resv0_data : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } hbncore_resv0;

    /* 0x940 : hbncore_resv1 */
    union {
        struct {
            uint32_t hbncore_resv1_data : 32; /* [31: 0],        r/w, 0xffffffff */
        } BF;
        uint32_t WORD;
    } hbncore_resv1;
};

typedef volatile struct aon_reg aon_reg_t;

#endif /* __AON_REG_H__ */
