/**
  ******************************************************************************
  * @file    touch_v2_reg.h
  * @version V1.0
  * @date    2025-09-10
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
#ifndef  __TOUCH_V2_REG_H__
#define  __TOUCH_V2_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define TOUCH_CTRL_0_OFFSET                                     (0x600)/* touch_ctrl_0 */
#define TOUCH_CTRL_1_OFFSET                                     (0x604)/* touch_ctrl_1 */
#define TOUCH_INT_STS_OFFSET                                    (0x608)/* touch_int_sts */
#define TOUCH_CHL_CFG_OFFSET                                    (0x60c)/* touch_chl_cfg */
#define TOUCH_CTRL_2_OFFSET                                     (0x610)/* touch_ctrl_2 */
#define TOUCH_FORCE_DAT_01_OFFSET                               (0x614)/* touch_force_dat_01 */
#define TOUCH_FORCE_DAT_23_OFFSET                               (0x618)/* touch_force_dat_23 */
#define TOUCH_FORCE_DAT_45_OFFSET                               (0x61c)/* touch_force_dat_45 */
#define TOUCH_FORCE_DAT_67_OFFSET                               (0x620)/* touch_force_dat_67 */
#define TOUCH_FORCE_DAT_89_OFFSET                               (0x624)/* touch_force_dat_89 */
#define TOUCH_FORCE_DAT_AB_OFFSET                               (0x628)/* touch_force_dat_ab */
#define TOUCH_FORCE_DAT_CD_OFFSET                               (0x62c)/* touch_force_dat_cd */
#define TOUCH_FORCE_DAT_EF_OFFSET                               (0x630)/* touch_force_dat_ef */
#define TOUCH_HYS_DAT_0_7_OFFSET                                (0x634)/* touch_hys_dat_0_7 */
#define TOUCH_HYS_DAT_8_F_OFFSET                                (0x638)/* touch_hys_dat_8_f */
#define TOUCH_VTH_DAT_0_3_OFFSET                                (0x63c)/* touch_vth_dat_0_3 */
#define TOUCH_VTH_DAT_4_7_OFFSET                                (0x640)/* touch_vth_dat_4_7 */
#define TOUCH_VTH_DAT_8_B_OFFSET                                (0x644)/* touch_vth_dat_8_b */
#define TOUCH_VTH_DAT_C_F_OFFSET                                (0x648)/* touch_vth_dat_c_f */
#define TOUCH_RAW_DAT_01_OFFSET                                 (0x64c)/* touch_raw_dat_01 */
#define TOUCH_RAW_DAT_23_OFFSET                                 (0x650)/* touch_raw_dat_23 */
#define TOUCH_RAW_DAT_45_OFFSET                                 (0x654)/* touch_raw_dat_45 */
#define TOUCH_RAW_DAT_67_OFFSET                                 (0x658)/* touch_raw_dat_67 */
#define TOUCH_RAW_DAT_89_OFFSET                                 (0x65c)/* touch_raw_dat_89 */
#define TOUCH_RAW_DAT_AB_OFFSET                                 (0x660)/* touch_raw_dat_ab */
#define TOUCH_RAW_DAT_CD_OFFSET                                 (0x664)/* touch_raw_dat_cd */
#define TOUCH_RAW_DAT_EF_OFFSET                                 (0x668)/* touch_raw_dat_ef */
#define TOUCH_FLT_DAT_01_OFFSET                                 (0x66c)/* touch_flt_dat_01 */
#define TOUCH_FLT_DAT_23_OFFSET                                 (0x670)/* touch_flt_dat_23 */
#define TOUCH_FLT_DAT_45_OFFSET                                 (0x674)/* touch_flt_dat_45 */
#define TOUCH_FLT_DAT_67_OFFSET                                 (0x678)/* touch_flt_dat_67 */
#define TOUCH_FLT_DAT_89_OFFSET                                 (0x67c)/* touch_flt_dat_89 */
#define TOUCH_FLT_DAT_AB_OFFSET                                 (0x680)/* touch_flt_dat_ab */
#define TOUCH_FLT_DAT_CD_OFFSET                                 (0x684)/* touch_flt_dat_cd */
#define TOUCH_FLT_DAT_EF_OFFSET                                 (0x688)/* touch_flt_dat_ef */
#define TOUCH_LTA_DAT_01_OFFSET                                 (0x68c)/* touch_lta_dat_01 */
#define TOUCH_LTA_DAT_23_OFFSET                                 (0x690)/* touch_lta_dat_23 */
#define TOUCH_LTA_DAT_45_OFFSET                                 (0x694)/* touch_lta_dat_45 */
#define TOUCH_LTA_DAT_67_OFFSET                                 (0x698)/* touch_lta_dat_67 */
#define TOUCH_LTA_DAT_89_OFFSET                                 (0x69c)/* touch_lta_dat_89 */
#define TOUCH_LTA_DAT_AB_OFFSET                                 (0x6a0)/* touch_lta_dat_ab */
#define TOUCH_LTA_DAT_CD_OFFSET                                 (0x6a4)/* touch_lta_dat_cd */
#define TOUCH_LTA_DAT_EF_OFFSET                                 (0x6a8)/* touch_lta_dat_ef */
#define TOUCH_INT_CLR_OFFSET                                    (0x6aC)/* touch_int_clr */
#define TOUCH_HOP_FREQ_CFG_OFFSET                               (0x6b0)/* touch_hop_freq_cfg */
#define TOUCH_DET_DUR_OFFSET                                    (0x6b4)/* touch_det_dur */

/* Register Bitfield definitions *****************************************************/

/* 0x600 : touch_ctrl_0 */
#define TOUCH_CLK_SEL                                           (1<<0U)
#define TOUCH_CLK_DIV_RATIO_SHIFT                               (1U)
#define TOUCH_CLK_DIV_RATIO_MASK                                (0x7<<TOUCH_CLK_DIV_RATIO_SHIFT)
#define TOUCH_EN                                                (1<<4U)
#define TOUCH_SCN_EN                                            (1<<5U)
#define TOUCH_32M_CLK_SEL                                       (1<<6U)
#define TOUCH_SCN_LST_CHL_CNT_SHIFT                             (7U)
#define TOUCH_SCN_LST_CHL_CNT_MASK                              (0x3f<<TOUCH_SCN_LST_CHL_CNT_SHIFT)
#define TOUCH_TEN_TOUCH                                         (1<<13U)
#define TOUCH_DC_TP_INT_OUT_EN_TOUCH                            (1<<17U)
#define TOUCH_DC_TP_EN_TOUCH                                    (1<<18U)
#define TOUCH_HYS_SEL                                           (1<<19U)
#define TOUCH_VLDO_SEL_SHIFT                                    (20U)
#define TOUCH_VLDO_SEL_MASK                                     (0xf<<TOUCH_VLDO_SEL_SHIFT)
#define TOUCH_VREF_SEL_SHIFT                                    (24U)
#define TOUCH_VREF_SEL_MASK                                     (0xf<<TOUCH_VREF_SEL_SHIFT)

/* 0x604 : touch_ctrl_1 */
#define TOUCH_DET_INT_MASK_SHIFT                                (0U)
#define TOUCH_DET_INT_MASK_MASK                                 (0xffff<<TOUCH_DET_INT_MASK_SHIFT)
#define TOUCH_CHL_SCN_LST_INT_MASK                              (1<<16U)
#define TOUCH_HOP_FREQ_TH_INT_MASK_SHIFT                        (17U)
#define TOUCH_HOP_FREQ_TH_INT_MASK_MASK                         (0x3<<TOUCH_HOP_FREQ_TH_INT_MASK_SHIFT)
#define TOUCH_DATA_HYS_EN                                       (1<<20U)
#define TOUCH_FORCE_VAL_EN                                      (1<<21U)
#define TOUCH_CHANNEL_CAL_EN                                    (1<<22U)
#define TOUCH_LTA_ORDER_SHIFT                                   (23U)
#define TOUCH_LTA_ORDER_MASK                                    (0x3<<TOUCH_LTA_ORDER_SHIFT)
#define TOUCH_LTA_EN                                            (1<<25U)
#define TOUCH_LTA_SEL                                           (1<<26U)
#define TOUCH_FLT_ORDER_SHIFT                                   (27U)
#define TOUCH_FLT_ORDER_MASK                                    (0x3<<TOUCH_FLT_ORDER_SHIFT)
#define TOUCH_FLT_EN                                            (1<<29U)
#define TOUCH_FLT_SEL                                           (1<<30U)
#define TOUCH_SWRST                                             (1<<31U)

/* 0x608 : touch_int_sts */
#define TOUCH_DET_INT_SHIFT                                     (0U)
#define TOUCH_DET_INT_MASK                                      (0xffff<<TOUCH_DET_INT_SHIFT)
#define TOUCH_CHL_SCN_LST_INT                                   (1<<16U)
#define TOUCH_HOP_FREQ_TH_INT_SHIFT                             (17U)
#define TOUCH_HOP_FREQ_TH_INT_MASK                              (0x3<<TOUCH_HOP_FREQ_TH_INT_SHIFT)

/* 0x60c : touch_chl_cfg */
#define TOUCH_LDO_SETTLE_CYC_SHIFT                              (0U)
#define TOUCH_LDO_SETTLE_CYC_MASK                               (0x7<<TOUCH_LDO_SETTLE_CYC_SHIFT)
#define TOUCH_CONT_MODE_EN                                      (1<<3U)
#define TOUCH_ULP2_EN                                           (1<<6U)
#define TOUCH_AS_CHL_SEL                                        (1<<7U)
#define TOUCH_AS_CH_SEL_SHIFT                                   (8U)
#define TOUCH_AS_CH_SEL_MASK                                    (0xff<<TOUCH_AS_CH_SEL_SHIFT)
#define TOUCH_CH_RX_EN_SHIFT                                    (16U)
#define TOUCH_CH_RX_EN_MASK                                     (0xff<<TOUCH_CH_RX_EN_SHIFT)
#define TOUCH_CH_TX_EN_SHIFT                                    (24U)
#define TOUCH_CH_TX_EN_MASK                                     (0xff<<TOUCH_CH_TX_EN_SHIFT)

/* 0x610 : touch_ctrl_2 */
#define TOUCH_SLEEP_CYCLE_SHIFT                                 (0U)
#define TOUCH_SLEEP_CYCLE_MASK                                  (0x7fffff<<TOUCH_SLEEP_CYCLE_SHIFT)
#define TOUCH_PCHARGE_LOW_SHIFT                                 (23U)
#define TOUCH_PCHARGE_LOW_MASK                                  (0x7<<TOUCH_PCHARGE_LOW_SHIFT)
#define TOUCH_PCHARGE_HIGH_SHIFT                                (26U)
#define TOUCH_PCHARGE_HIGH_MASK                                 (0x7<<TOUCH_PCHARGE_HIGH_SHIFT)
#define TOUCH_DET_KEEP                                          (1<<29U)
#define TOUCH_DET_TOUCH_ON                                      (1<<30U)

/* 0x614 : touch_force_dat_01 */
#define TOUCH_FORCE_DATA_0_SHIFT                                (0U)
#define TOUCH_FORCE_DATA_0_MASK                                 (0xffff<<TOUCH_FORCE_DATA_0_SHIFT)
#define TOUCH_FORCE_DATA_1_SHIFT                                (16U)
#define TOUCH_FORCE_DATA_1_MASK                                 (0xffff<<TOUCH_FORCE_DATA_1_SHIFT)

/* 0x618 : touch_force_dat_23 */
#define TOUCH_FORCE_DATA_2_SHIFT                                (0U)
#define TOUCH_FORCE_DATA_2_MASK                                 (0xffff<<TOUCH_FORCE_DATA_2_SHIFT)
#define TOUCH_FORCE_DATA_3_SHIFT                                (16U)
#define TOUCH_FORCE_DATA_3_MASK                                 (0xffff<<TOUCH_FORCE_DATA_3_SHIFT)

/* 0x61c : touch_force_dat_45 */
#define TOUCH_FORCE_DATA_4_SHIFT                                (0U)
#define TOUCH_FORCE_DATA_4_MASK                                 (0xffff<<TOUCH_FORCE_DATA_4_SHIFT)
#define TOUCH_FORCE_DATA_5_SHIFT                                (16U)
#define TOUCH_FORCE_DATA_5_MASK                                 (0xffff<<TOUCH_FORCE_DATA_5_SHIFT)

/* 0x620 : touch_force_dat_67 */
#define TOUCH_FORCE_DATA_6_SHIFT                                (0U)
#define TOUCH_FORCE_DATA_6_MASK                                 (0xffff<<TOUCH_FORCE_DATA_6_SHIFT)
#define TOUCH_FORCE_DATA_7_SHIFT                                (16U)
#define TOUCH_FORCE_DATA_7_MASK                                 (0xffff<<TOUCH_FORCE_DATA_7_SHIFT)

/* 0x624 : touch_force_dat_89 */
#define TOUCH_FORCE_DATA_8_SHIFT                                (0U)
#define TOUCH_FORCE_DATA_8_MASK                                 (0xffff<<TOUCH_FORCE_DATA_8_SHIFT)
#define TOUCH_FORCE_DATA_9_SHIFT                                (16U)
#define TOUCH_FORCE_DATA_9_MASK                                 (0xffff<<TOUCH_FORCE_DATA_9_SHIFT)

/* 0x628 : touch_force_dat_ab */
#define TOUCH_FORCE_DATA_10_SHIFT                               (0U)
#define TOUCH_FORCE_DATA_10_MASK                                (0xffff<<TOUCH_FORCE_DATA_10_SHIFT)
#define TOUCH_FORCE_DATA_11_SHIFT                               (16U)
#define TOUCH_FORCE_DATA_11_MASK                                (0xffff<<TOUCH_FORCE_DATA_11_SHIFT)

/* 0x62c : touch_force_dat_cd */
#define TOUCH_FORCE_DATA_12_SHIFT                               (0U)
#define TOUCH_FORCE_DATA_12_MASK                                (0xffff<<TOUCH_FORCE_DATA_12_SHIFT)
#define TOUCH_FORCE_DATA_13_SHIFT                               (16U)
#define TOUCH_FORCE_DATA_13_MASK                                (0xffff<<TOUCH_FORCE_DATA_13_SHIFT)

/* 0x630 : touch_force_dat_ef */
#define TOUCH_FORCE_DATA_14_SHIFT                               (0U)
#define TOUCH_FORCE_DATA_14_MASK                                (0xffff<<TOUCH_FORCE_DATA_14_SHIFT)
#define TOUCH_FORCE_DATA_15_SHIFT                               (16U)
#define TOUCH_FORCE_DATA_15_MASK                                (0xffff<<TOUCH_FORCE_DATA_15_SHIFT)

/* 0x634 : touch_hys_dat_0_7 */
#define TOUCH_HYS_DATA_0_SHIFT                                  (0U)
#define TOUCH_HYS_DATA_0_MASK                                   (0xf<<TOUCH_HYS_DATA_0_SHIFT)
#define TOUCH_HYS_DATA_1_SHIFT                                  (4U)
#define TOUCH_HYS_DATA_1_MASK                                   (0xf<<TOUCH_HYS_DATA_1_SHIFT)
#define TOUCH_HYS_DATA_2_SHIFT                                  (8U)
#define TOUCH_HYS_DATA_2_MASK                                   (0xf<<TOUCH_HYS_DATA_2_SHIFT)
#define TOUCH_HYS_DATA_3_SHIFT                                  (12U)
#define TOUCH_HYS_DATA_3_MASK                                   (0xf<<TOUCH_HYS_DATA_3_SHIFT)
#define TOUCH_HYS_DATA_4_SHIFT                                  (16U)
#define TOUCH_HYS_DATA_4_MASK                                   (0xf<<TOUCH_HYS_DATA_4_SHIFT)
#define TOUCH_HYS_DATA_5_SHIFT                                  (20U)
#define TOUCH_HYS_DATA_5_MASK                                   (0xf<<TOUCH_HYS_DATA_5_SHIFT)
#define TOUCH_HYS_DATA_6_SHIFT                                  (24U)
#define TOUCH_HYS_DATA_6_MASK                                   (0xf<<TOUCH_HYS_DATA_6_SHIFT)
#define TOUCH_HYS_DATA_7_SHIFT                                  (28U)
#define TOUCH_HYS_DATA_7_MASK                                   (0xf<<TOUCH_HYS_DATA_7_SHIFT)

/* 0x638 : touch_hys_dat_8_f */
#define TOUCH_HYS_DATA_8_SHIFT                                  (0U)
#define TOUCH_HYS_DATA_8_MASK                                   (0xf<<TOUCH_HYS_DATA_8_SHIFT)
#define TOUCH_HYS_DATA_9_SHIFT                                  (4U)
#define TOUCH_HYS_DATA_9_MASK                                   (0xf<<TOUCH_HYS_DATA_9_SHIFT)
#define TOUCH_HYS_DATA_10_SHIFT                                 (8U)
#define TOUCH_HYS_DATA_10_MASK                                  (0xf<<TOUCH_HYS_DATA_10_SHIFT)
#define TOUCH_HYS_DATA_11_SHIFT                                 (12U)
#define TOUCH_HYS_DATA_11_MASK                                  (0xf<<TOUCH_HYS_DATA_11_SHIFT)
#define TOUCH_HYS_DATA_12_SHIFT                                 (16U)
#define TOUCH_HYS_DATA_12_MASK                                  (0xf<<TOUCH_HYS_DATA_12_SHIFT)
#define TOUCH_HYS_DATA_13_SHIFT                                 (20U)
#define TOUCH_HYS_DATA_13_MASK                                  (0xf<<TOUCH_HYS_DATA_13_SHIFT)
#define TOUCH_HYS_DATA_14_SHIFT                                 (24U)
#define TOUCH_HYS_DATA_14_MASK                                  (0xf<<TOUCH_HYS_DATA_14_SHIFT)
#define TOUCH_HYS_DATA_15_SHIFT                                 (28U)
#define TOUCH_HYS_DATA_15_MASK                                  (0xf<<TOUCH_HYS_DATA_15_SHIFT)

/* 0x63c : touch_vth_dat_0_3 */
#define TOUCH_VTH_DATA_0_SHIFT                                  (0U)
#define TOUCH_VTH_DATA_0_MASK                                   (0xff<<TOUCH_VTH_DATA_0_SHIFT)
#define TOUCH_VTH_DATA_1_SHIFT                                  (8U)
#define TOUCH_VTH_DATA_1_MASK                                   (0xff<<TOUCH_VTH_DATA_1_SHIFT)
#define TOUCH_VTH_DATA_2_SHIFT                                  (16U)
#define TOUCH_VTH_DATA_2_MASK                                   (0xff<<TOUCH_VTH_DATA_2_SHIFT)
#define TOUCH_VTH_DATA_3_SHIFT                                  (24U)
#define TOUCH_VTH_DATA_3_MASK                                   (0xff<<TOUCH_VTH_DATA_3_SHIFT)

/* 0x640 : touch_vth_dat_4_7 */
#define TOUCH_VTH_DATA_4_SHIFT                                  (0U)
#define TOUCH_VTH_DATA_4_MASK                                   (0xff<<TOUCH_VTH_DATA_4_SHIFT)
#define TOUCH_VTH_DATA_5_SHIFT                                  (8U)
#define TOUCH_VTH_DATA_5_MASK                                   (0xff<<TOUCH_VTH_DATA_5_SHIFT)
#define TOUCH_VTH_DATA_6_SHIFT                                  (16U)
#define TOUCH_VTH_DATA_6_MASK                                   (0xff<<TOUCH_VTH_DATA_6_SHIFT)
#define TOUCH_VTH_DATA_7_SHIFT                                  (24U)
#define TOUCH_VTH_DATA_7_MASK                                   (0xff<<TOUCH_VTH_DATA_7_SHIFT)

/* 0x644 : touch_vth_dat_8_b */
#define TOUCH_VTH_DATA_8_SHIFT                                  (0U)
#define TOUCH_VTH_DATA_8_MASK                                   (0xff<<TOUCH_VTH_DATA_8_SHIFT)
#define TOUCH_VTH_DATA_9_SHIFT                                  (8U)
#define TOUCH_VTH_DATA_9_MASK                                   (0xff<<TOUCH_VTH_DATA_9_SHIFT)
#define TOUCH_VTH_DATA_10_SHIFT                                 (16U)
#define TOUCH_VTH_DATA_10_MASK                                  (0xff<<TOUCH_VTH_DATA_10_SHIFT)
#define TOUCH_VTH_DATA_11_SHIFT                                 (24U)
#define TOUCH_VTH_DATA_11_MASK                                  (0xff<<TOUCH_VTH_DATA_11_SHIFT)

/* 0x648 : touch_vth_dat_c_f */
#define TOUCH_VTH_DATA_12_SHIFT                                 (0U)
#define TOUCH_VTH_DATA_12_MASK                                  (0xff<<TOUCH_VTH_DATA_12_SHIFT)
#define TOUCH_VTH_DATA_13_SHIFT                                 (8U)
#define TOUCH_VTH_DATA_13_MASK                                  (0xff<<TOUCH_VTH_DATA_13_SHIFT)
#define TOUCH_VTH_DATA_14_SHIFT                                 (16U)
#define TOUCH_VTH_DATA_14_MASK                                  (0xff<<TOUCH_VTH_DATA_14_SHIFT)
#define TOUCH_VTH_DATA_15_SHIFT                                 (24U)
#define TOUCH_VTH_DATA_15_MASK                                  (0xff<<TOUCH_VTH_DATA_15_SHIFT)

/* 0x64c : touch_raw_dat_01 */
#define TOUCH_RAW_DATA_0_SHIFT                                  (0U)
#define TOUCH_RAW_DATA_0_MASK                                   (0xffff<<TOUCH_RAW_DATA_0_SHIFT)
#define TOUCH_RAW_DATA_1_SHIFT                                  (16U)
#define TOUCH_RAW_DATA_1_MASK                                   (0xffff<<TOUCH_RAW_DATA_1_SHIFT)

/* 0x650 : touch_raw_dat_23 */
#define TOUCH_RAW_DATA_2_SHIFT                                  (0U)
#define TOUCH_RAW_DATA_2_MASK                                   (0xffff<<TOUCH_RAW_DATA_2_SHIFT)
#define TOUCH_RAW_DATA_3_SHIFT                                  (16U)
#define TOUCH_RAW_DATA_3_MASK                                   (0xffff<<TOUCH_RAW_DATA_3_SHIFT)

/* 0x654 : touch_raw_dat_45 */
#define TOUCH_RAW_DATA_4_SHIFT                                  (0U)
#define TOUCH_RAW_DATA_4_MASK                                   (0xffff<<TOUCH_RAW_DATA_4_SHIFT)
#define TOUCH_RAW_DATA_5_SHIFT                                  (16U)
#define TOUCH_RAW_DATA_5_MASK                                   (0xffff<<TOUCH_RAW_DATA_5_SHIFT)

/* 0x658 : touch_raw_dat_67 */
#define TOUCH_RAW_DATA_6_SHIFT                                  (0U)
#define TOUCH_RAW_DATA_6_MASK                                   (0xffff<<TOUCH_RAW_DATA_6_SHIFT)
#define TOUCH_RAW_DATA_7_SHIFT                                  (16U)
#define TOUCH_RAW_DATA_7_MASK                                   (0xffff<<TOUCH_RAW_DATA_7_SHIFT)

/* 0x65c : touch_raw_dat_89 */
#define TOUCH_RAW_DATA_8_SHIFT                                  (0U)
#define TOUCH_RAW_DATA_8_MASK                                   (0xffff<<TOUCH_RAW_DATA_8_SHIFT)
#define TOUCH_RAW_DATA_9_SHIFT                                  (16U)
#define TOUCH_RAW_DATA_9_MASK                                   (0xffff<<TOUCH_RAW_DATA_9_SHIFT)

/* 0x660 : touch_raw_dat_ab */
#define TOUCH_RAW_DATA_10_SHIFT                                 (0U)
#define TOUCH_RAW_DATA_10_MASK                                  (0xffff<<TOUCH_RAW_DATA_10_SHIFT)
#define TOUCH_RAW_DATA_11_SHIFT                                 (16U)
#define TOUCH_RAW_DATA_11_MASK                                  (0xffff<<TOUCH_RAW_DATA_11_SHIFT)

/* 0x664 : touch_raw_dat_cd */
#define TOUCH_RAW_DATA_12_SHIFT                                 (0U)
#define TOUCH_RAW_DATA_12_MASK                                  (0xffff<<TOUCH_RAW_DATA_12_SHIFT)
#define TOUCH_RAW_DATA_13_SHIFT                                 (16U)
#define TOUCH_RAW_DATA_13_MASK                                  (0xffff<<TOUCH_RAW_DATA_13_SHIFT)

/* 0x668 : touch_raw_dat_ef */
#define TOUCH_RAW_DATA_14_SHIFT                                 (0U)
#define TOUCH_RAW_DATA_14_MASK                                  (0xffff<<TOUCH_RAW_DATA_14_SHIFT)
#define TOUCH_RAW_DATA_15_SHIFT                                 (16U)
#define TOUCH_RAW_DATA_15_MASK                                  (0xffff<<TOUCH_RAW_DATA_15_SHIFT)

/* 0x66c : touch_flt_dat_01 */
#define TOUCH_FLT_DATA_0_SHIFT                                  (0U)
#define TOUCH_FLT_DATA_0_MASK                                   (0xffff<<TOUCH_FLT_DATA_0_SHIFT)
#define TOUCH_FLT_DATA_1_SHIFT                                  (16U)
#define TOUCH_FLT_DATA_1_MASK                                   (0xffff<<TOUCH_FLT_DATA_1_SHIFT)

/* 0x670 : touch_flt_dat_23 */
#define TOUCH_FLT_DATA_2_SHIFT                                  (0U)
#define TOUCH_FLT_DATA_2_MASK                                   (0xffff<<TOUCH_FLT_DATA_2_SHIFT)
#define TOUCH_FLT_DATA_3_SHIFT                                  (16U)
#define TOUCH_FLT_DATA_3_MASK                                   (0xffff<<TOUCH_FLT_DATA_3_SHIFT)

/* 0x674 : touch_flt_dat_45 */
#define TOUCH_FLT_DATA_4_SHIFT                                  (0U)
#define TOUCH_FLT_DATA_4_MASK                                   (0xffff<<TOUCH_FLT_DATA_4_SHIFT)
#define TOUCH_FLT_DATA_5_SHIFT                                  (16U)
#define TOUCH_FLT_DATA_5_MASK                                   (0xffff<<TOUCH_FLT_DATA_5_SHIFT)

/* 0x678 : touch_flt_dat_67 */
#define TOUCH_FLT_DATA_6_SHIFT                                  (0U)
#define TOUCH_FLT_DATA_6_MASK                                   (0xffff<<TOUCH_FLT_DATA_6_SHIFT)
#define TOUCH_FLT_DATA_7_SHIFT                                  (16U)
#define TOUCH_FLT_DATA_7_MASK                                   (0xffff<<TOUCH_FLT_DATA_7_SHIFT)

/* 0x67c : touch_flt_dat_89 */
#define TOUCH_FLT_DATA_8_SHIFT                                  (0U)
#define TOUCH_FLT_DATA_8_MASK                                   (0xffff<<TOUCH_FLT_DATA_8_SHIFT)
#define TOUCH_FLT_DATA_9_SHIFT                                  (16U)
#define TOUCH_FLT_DATA_9_MASK                                   (0xffff<<TOUCH_FLT_DATA_9_SHIFT)

/* 0x680 : touch_flt_dat_ab */
#define TOUCH_FLT_DATA_10_SHIFT                                 (0U)
#define TOUCH_FLT_DATA_10_MASK                                  (0xffff<<TOUCH_FLT_DATA_10_SHIFT)
#define TOUCH_FLT_DATA_11_SHIFT                                 (16U)
#define TOUCH_FLT_DATA_11_MASK                                  (0xffff<<TOUCH_FLT_DATA_11_SHIFT)

/* 0x684 : touch_flt_dat_cd */
#define TOUCH_FLT_DATA_12_SHIFT                                 (0U)
#define TOUCH_FLT_DATA_12_MASK                                  (0xffff<<TOUCH_FLT_DATA_12_SHIFT)
#define TOUCH_FLT_DATA_13_SHIFT                                 (16U)
#define TOUCH_FLT_DATA_13_MASK                                  (0xffff<<TOUCH_FLT_DATA_13_SHIFT)

/* 0x688 : touch_flt_dat_ef */
#define TOUCH_FLT_DATA_14_SHIFT                                 (0U)
#define TOUCH_FLT_DATA_14_MASK                                  (0xffff<<TOUCH_FLT_DATA_14_SHIFT)
#define TOUCH_FLT_DATA_15_SHIFT                                 (16U)
#define TOUCH_FLT_DATA_15_MASK                                  (0xffff<<TOUCH_FLT_DATA_15_SHIFT)

/* 0x68c : touch_lta_dat_01 */
#define TOUCH_LTA_DATA_0_SHIFT                                  (0U)
#define TOUCH_LTA_DATA_0_MASK                                   (0xffff<<TOUCH_LTA_DATA_0_SHIFT)
#define TOUCH_LTA_DATA_1_SHIFT                                  (16U)
#define TOUCH_LTA_DATA_1_MASK                                   (0xffff<<TOUCH_LTA_DATA_1_SHIFT)

/* 0x690 : touch_lta_dat_23 */
#define TOUCH_LTA_DATA_2_SHIFT                                  (0U)
#define TOUCH_LTA_DATA_2_MASK                                   (0xffff<<TOUCH_LTA_DATA_2_SHIFT)
#define TOUCH_LTA_DATA_3_SHIFT                                  (16U)
#define TOUCH_LTA_DATA_3_MASK                                   (0xffff<<TOUCH_LTA_DATA_3_SHIFT)

/* 0x694 : touch_lta_dat_45 */
#define TOUCH_LTA_DATA_4_SHIFT                                  (0U)
#define TOUCH_LTA_DATA_4_MASK                                   (0xffff<<TOUCH_LTA_DATA_4_SHIFT)
#define TOUCH_LTA_DATA_5_SHIFT                                  (16U)
#define TOUCH_LTA_DATA_5_MASK                                   (0xffff<<TOUCH_LTA_DATA_5_SHIFT)

/* 0x698 : touch_lta_dat_67 */
#define TOUCH_LTA_DATA_6_SHIFT                                  (0U)
#define TOUCH_LTA_DATA_6_MASK                                   (0xffff<<TOUCH_LTA_DATA_6_SHIFT)
#define TOUCH_LTA_DATA_7_SHIFT                                  (16U)
#define TOUCH_LTA_DATA_7_MASK                                   (0xffff<<TOUCH_LTA_DATA_7_SHIFT)

/* 0x69c : touch_lta_dat_89 */
#define TOUCH_LTA_DATA_8_SHIFT                                  (0U)
#define TOUCH_LTA_DATA_8_MASK                                   (0xffff<<TOUCH_LTA_DATA_8_SHIFT)
#define TOUCH_LTA_DATA_9_SHIFT                                  (16U)
#define TOUCH_LTA_DATA_9_MASK                                   (0xffff<<TOUCH_LTA_DATA_9_SHIFT)

/* 0x6a0 : touch_lta_dat_ab */
#define TOUCH_LTA_DATA_10_SHIFT                                 (0U)
#define TOUCH_LTA_DATA_10_MASK                                  (0xffff<<TOUCH_LTA_DATA_10_SHIFT)
#define TOUCH_LTA_DATA_11_SHIFT                                 (16U)
#define TOUCH_LTA_DATA_11_MASK                                  (0xffff<<TOUCH_LTA_DATA_11_SHIFT)

/* 0x6a4 : touch_lta_dat_cd */
#define TOUCH_LTA_DATA_12_SHIFT                                 (0U)
#define TOUCH_LTA_DATA_12_MASK                                  (0xffff<<TOUCH_LTA_DATA_12_SHIFT)
#define TOUCH_LTA_DATA_13_SHIFT                                 (16U)
#define TOUCH_LTA_DATA_13_MASK                                  (0xffff<<TOUCH_LTA_DATA_13_SHIFT)

/* 0x6a8 : touch_lta_dat_ef */
#define TOUCH_LTA_DATA_14_SHIFT                                 (0U)
#define TOUCH_LTA_DATA_14_MASK                                  (0xffff<<TOUCH_LTA_DATA_14_SHIFT)
#define TOUCH_LTA_DATA_15_SHIFT                                 (16U)
#define TOUCH_LTA_DATA_15_MASK                                  (0xffff<<TOUCH_LTA_DATA_15_SHIFT)

/* 0x6aC : touch_int_clr */
#define TOUCH_DET_INT_CLR_SHIFT                                 (0U)
#define TOUCH_DET_INT_CLR_MASK                                  (0xffff<<TOUCH_DET_INT_CLR_SHIFT)
#define TOUCH_CHL_SCN_LST_INT_CLR                               (1<<16U)
#define TOUCH_HOP_FREQ_TH_INT_CLR_SHIFT                         (17U)
#define TOUCH_HOP_FREQ_TH_INT_CLR_MASK                          (0x3<<TOUCH_HOP_FREQ_TH_INT_CLR_SHIFT)

/* 0x6b0 : touch_hop_freq_cfg */
#define TOUCH_RESERVED_SHIFT                                    (0U)
#define TOUCH_RESERVED_MASK                                     (0xff<<TOUCH_RESERVED_SHIFT)
#define TOUCH_HOP_FREQ_TH_SHIFT                                 (8U)
#define TOUCH_HOP_FREQ_TH_MASK                                  (0xff<<TOUCH_HOP_FREQ_TH_SHIFT)
#define TOUCH_HOP_FREQ_EN                                       (1<<19U)
#define TOUCH_HOP_FREQ_DIV0_SHIFT                               (20U)
#define TOUCH_HOP_FREQ_DIV0_MASK                                (0xf<<TOUCH_HOP_FREQ_DIV0_SHIFT)
#define TOUCH_HOP_FREQ_DIV1_SHIFT                               (24U)
#define TOUCH_HOP_FREQ_DIV1_MASK                                (0xf<<TOUCH_HOP_FREQ_DIV1_SHIFT)
#define TOUCH_HOP_FREQ_DIV2_SHIFT                               (28U)
#define TOUCH_HOP_FREQ_DIV2_MASK                                (0xf<<TOUCH_HOP_FREQ_DIV2_SHIFT)

/* 0x6b4 : touch_det_dur */
#define TOUCH_DET_DUR_SHIFT                                     (0U)
#define TOUCH_DET_DUR_MASK                                      (0xffff<<TOUCH_DET_DUR_SHIFT)


#endif  /* __TOUCH_V2_REG_H__ */
