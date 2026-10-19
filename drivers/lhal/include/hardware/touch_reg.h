/**
  ******************************************************************************
  * @file    touch_reg.h
  * @version V1.0
  * @date    2025-04-17
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
#ifndef  __TOUCH_REG_H__
#define  __TOUCH_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define TOUCH1_OFFSET                                           (0xA00)/* touch channel, clock, ana config1 */
#define TOUCH2_OFFSET                                           (0xA04)/* touch channel, clock, ana config2 */
#define TOUCH3_OFFSET                                           (0xA08)/* touch data process */
#define TOUCH_SLEEP_TIME_OFFSET                                 (0xA0C)/* Touch_sleep_time */
#define TOUCH_DATA_HYSTERSIS_OFFSET                             (0xA10)/* touch_data_hystersis */
#define TOUCH_CHANNEL_FORCE_DATA_0_OFFSET                       (0xA14)/* Channel_force_data_0 */
#define TOUCH_CHANNEL_FORCE_DATA_1_OFFSET                       (0xA18)/* Channel_force_data_1 */
#define TOUCH_CHANNEL_FORCE_DATA_2_OFFSET                       (0xA1C)/* Channel_force_data_2 */
#define TOUCH_CHANNEL_FORCE_DATA_3_OFFSET                       (0xA20)/* Channel_force_data_3 */
#define TOUCH_CHANNEL_FORCE_DATA_4_OFFSET                       (0xA24)/* Channel_force_data_4 */
#define TOUCH_CHANNEL_FORCE_DATA_5_OFFSET                       (0xA28)/* Channel_force_data_5 */
#define TOUCH_CHANNEL_VTH_DATA_0_OFFSET                         (0xA2C)/* Channel_vth_data_0 */
#define TOUCH_CHANNEL_VTH_DATA_1_OFFSET                         (0xA30)/* Channel_vth_data_1 */
#define TOUCH_CHANNEL_VTH_DATA_2_OFFSET                         (0xA34)/* Channel_vth_data_2 */
#define TOUCH_CHANNEL_RAW_DATA_0_OFFSET                         (0xA38)/* Channel_raw_data_0 */
#define TOUCH_CHANNEL_RAW_DATA_1_OFFSET                         (0xA3C)/* Channel_raw_data_1 */
#define TOUCH_CHANNEL_RAW_DATA_2_OFFSET                         (0xA40)/* Channel_raw_data_2 */
#define TOUCH_CHANNEL_RAW_DATA_3_OFFSET                         (0xA44)/* Channel_raw_data_3 */
#define TOUCH_CHANNEL_RAW_DATA_4_OFFSET                         (0xA48)/* Channel_raw_data_4 */
#define TOUCH_CHANNEL_RAW_DATA_5_OFFSET                         (0xA4C)/* Channel_raw_data_5 */
#define TOUCH_CHANNEL_RAW_DATA_6_OFFSET                         (0xA50)/* Channel_raw_data_6 */
#define TOUCH_CHANNEL_RAW_DATA_7_OFFSET                         (0xA54)/* Channel_raw_data_7 */
#define TOUCH_CHANNEL_RAW_DATA_8_OFFSET                         (0xA58)/* Channel_raw_data_8 */
#define TOUCH_CHANNEL_RAW_DATA_9_OFFSET                         (0xA5C)/* Channel_raw_data_9 */
#define TOUCH_CHANNEL_RAW_DATA_10_OFFSET                        (0xA60)/* Channel_raw_data_10 */
#define TOUCH_CHANNEL_RAW_DATA_11_OFFSET                        (0xA64)/* Channel_raw_data_11 */
#define TOUCH_CHANNEL_LTA_DATA_0_OFFSET                         (0xA68)/* Channel_LTA_data_0 */
#define TOUCH_CHANNEL_LTA_DATA_1_OFFSET                         (0xA6C)/* Channel_LTA_data_1 */
#define TOUCH_CHANNEL_LTA_DATA_2_OFFSET                         (0xA70)/* Channel_LTA_data_2 */
#define TOUCH_CHANNEL_LTA_DATA_3_OFFSET                         (0xA74)/* Channel_LTA_data_3 */
#define TOUCH_CHANNEL_LTA_DATA_4_OFFSET                         (0xA78)/* Channel_LTA_data_4 */
#define TOUCH_CHANNEL_LTA_DATA_5_OFFSET                         (0xA7C)/* Channel_LTA_data_5 */
#define TOUCH_CHANNEL_LTA_DATA_6_OFFSET                         (0xA80)/* Channel_LTA_data_6 */
#define TOUCH_CHANNEL_LTA_DATA_7_OFFSET                         (0xA84)/* Channel_LTA_data_7 */
#define TOUCH_CHANNEL_LTA_DATA_8_OFFSET                         (0xA88)/* Channel_LTA_data_8 */
#define TOUCH_CHANNEL_LTA_DATA_9_OFFSET                         (0xA8C)/* Channel_LTA_data_9 */
#define TOUCH_CHANNEL_LTA_DATA_10_OFFSET                        (0xA90)/* Channel_LTA_data_10 */
#define TOUCH_CHANNEL_LTA_DATA_11_OFFSET                        (0xA94)/* Channel_LTA_data_11 */
#define TOUCH_CHANNEL_FLT_DATA_0_OFFSET                         (0xA98)/* Channel_FLT_data_0 */
#define TOUCH_CHANNEL_FLT_DATA_1_OFFSET                         (0xA9C)/* Channel_FLT_data_1 */
#define TOUCH_CHANNEL_FLT_DATA_2_OFFSET                         (0xAA0)/* Channel_FLT_data_2 */
#define TOUCH_CHANNEL_FLT_DATA_3_OFFSET                         (0xAA4)/* Channel_FLT_data_3 */
#define TOUCH_CHANNEL_FLT_DATA_4_OFFSET                         (0xAA8)/* Channel_FLT_data_4 */
#define TOUCH_CHANNEL_FLT_DATA_5_OFFSET                         (0xAAC)/* Channel_FLT_data_5 */
#define TOUCH_CHANNEL_FLT_DATA_6_OFFSET                         (0xAB0)/* Channel_FLT_data_6 */
#define TOUCH_CHANNEL_FLT_DATA_7_OFFSET                         (0xAB4)/* Channel_FLT_data_7 */
#define TOUCH_CHANNEL_FLT_DATA_8_OFFSET                         (0xAB8)/* Channel_FLT_data_8 */
#define TOUCH_CHANNEL_FLT_DATA_9_OFFSET                         (0xABC)/* Channel_FLT_data_9 */
#define TOUCH_CHANNEL_FLT_DATA_10_OFFSET                        (0xAC0)/* Channel_FLT_data_10 */
#define TOUCH_CHANNEL_FLT_DATA_11_OFFSET                        (0xAC4)/* Channel_FLT_data_11 */
#define TOUCH_RSVD_OFFSET                                       (0xAC8)/* touch_rsvd */
#define TOUCH_INT_SETTING_OFFSET                                (0xACC)/* touch_int_setting */
#define TOUCH_INT_STATUS_OFFSET                                 (0xAD0)/* touch_int_status */

/* Register Bitfield definitions *****************************************************/

/* 0xA00 : touch channel, clock, ana config1 */
#define TOUCH_VREF_SEL_SHIFT                                    (0U)
#define TOUCH_VREF_SEL_MASK                                     (0x7<<TOUCH_VREF_SEL_SHIFT)
#define TOUCH_VLDO_SEL_SHIFT                                    (3U)
#define TOUCH_VLDO_SEL_MASK                                     (0x7<<TOUCH_VLDO_SEL_SHIFT)
#define TOUCH_COMP_HYS_SEL                                      (1<<6U)
#define TOUCH_CURRENT_SEL                                       (1<<7U)
#define TOUCH_CLK_SEL                                           (1<<16U)
#define TOUCH_CLK_DIV_RATIO_SHIFT                               (17U)
#define TOUCH_CLK_DIV_RATIO_MASK                                (0x7<<TOUCH_CLK_DIV_RATIO_SHIFT)
#define TOUCH_PCHARGE_HIGH_SHIFT                                (20U)
#define TOUCH_PCHARGE_HIGH_MASK                                 (0x7<<TOUCH_PCHARGE_HIGH_SHIFT)
#define TOUCH_PCHARGE_LOW_SHIFT                                 (23U)
#define TOUCH_PCHARGE_LOW_MASK                                  (0x7<<TOUCH_PCHARGE_LOW_SHIFT)
#define TOUCH_CONT_EN                                           (1<<26U)
#define TOUCH_CYCLE_EN                                          (1<<27U)
#define TOUCH_ULP_EN                                            (1<<28U)
#define TOUCH_PU_TOUCH                                          (1<<30U)

/* 0xA04 : touch channel, clock, ana config2 */
#define TOUCH_CHANNEL_SEL_SHIFT                                 (0U)
#define TOUCH_CHANNEL_SEL_MASK                                  (0xf<<TOUCH_CHANNEL_SEL_SHIFT)
#define TOUCH_CHANNEL0_HIGHZ_EN                                 (1<<4U)
#define TOUCH_CHANNEL1_HIGHZ_EN                                 (1<<5U)
#define TOUCH_CHANNEL2_HIGHZ_EN                                 (1<<6U)
#define TOUCH_CHANNEL3_HIGHZ_EN                                 (1<<7U)
#define TOUCH_CHANNEL4_HIGHZ_EN                                 (1<<8U)
#define TOUCH_CHANNEL5_HIGHZ_EN                                 (1<<9U)
#define TOUCH_CHANNEL6_HIGHZ_EN                                 (1<<10U)
#define TOUCH_CHANNEL7_HIGHZ_EN                                 (1<<11U)
#define TOUCH_CHANNEL8_HIGHZ_EN                                 (1<<12U)
#define TOUCH_CHANNEL9_HIGHZ_EN                                 (1<<13U)
#define TOUCH_CHANNEL10_HIGHZ_EN                                (1<<14U)
#define TOUCH_CHANNEL11_HIGHZ_EN                                (1<<15U)

/* 0xA08 : touch data process */
#define TOUCH_CHANNEL_CAL_EN                                    (1<<0U)
#define TOUCH_FORCE_VALUE_EN                                    (1<<1U)
#define TOUCH_DATA_HYS_EN                                       (1<<2U)
#define TOUCH_LTA_EN                                            (1<<4U)
#define TOUCH_LTA_ORDER_SHIFT                                   (5U)
#define TOUCH_LTA_ORDER_MASK                                    (0x7<<TOUCH_LTA_ORDER_SHIFT)
#define TOUCH_FLT_EN                                            (1<<8U)
#define TOUCH_FLT_ORDER_SHIFT                                   (9U)
#define TOUCH_FLT_ORDER_MASK                                    (0x7<<TOUCH_FLT_ORDER_SHIFT)
#define TOUCH_SELF_MUTUAL_SEL                                   (1<<12U)
#define TOUCH_VLDO_CCSEL_SHIFT                                  (13U)
#define TOUCH_VLDO_CCSEL_MASK                                   (0x3<<TOUCH_VLDO_CCSEL_SHIFT)
#define TOUCH_TEN_TOUCH                                         (1<<18U)

/* 0xA0C : Touch_sleep_time */
#define TOUCH_SLEEP_CYCLE_SHIFT                                 (0U)
#define TOUCH_SLEEP_CYCLE_MASK                                  (0x7fffff<<TOUCH_SLEEP_CYCLE_SHIFT)

/* 0xA10 : touch_data_hystersis */
#define TOUCH_DATA_HYS_SHIFT                                    (0U)
#define TOUCH_DATA_HYS_MASK                                     (0x1ff<<TOUCH_DATA_HYS_SHIFT)

/* 0xA14 : Channel_force_data_0 */
#define TOUCH_FORCE_DATA_CH0_SHIFT                              (0U)
#define TOUCH_FORCE_DATA_CH0_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH0_SHIFT)
#define TOUCH_FORCE_DATA_CH1_SHIFT                              (16U)
#define TOUCH_FORCE_DATA_CH1_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH1_SHIFT)

/* 0xA18 : Channel_force_data_1 */
#define TOUCH_FORCE_DATA_CH2_SHIFT                              (0U)
#define TOUCH_FORCE_DATA_CH2_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH2_SHIFT)
#define TOUCH_FORCE_DATA_CH3_SHIFT                              (16U)
#define TOUCH_FORCE_DATA_CH3_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH3_SHIFT)

/* 0xA1C : Channel_force_data_2 */
#define TOUCH_FORCE_DATA_CH4_SHIFT                              (0U)
#define TOUCH_FORCE_DATA_CH4_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH4_SHIFT)
#define TOUCH_FORCE_DATA_CH5_SHIFT                              (16U)
#define TOUCH_FORCE_DATA_CH5_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH5_SHIFT)

/* 0xA20 : Channel_force_data_3 */
#define TOUCH_FORCE_DATA_CH6_SHIFT                              (0U)
#define TOUCH_FORCE_DATA_CH6_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH6_SHIFT)
#define TOUCH_FORCE_DATA_CH7_SHIFT                              (16U)
#define TOUCH_FORCE_DATA_CH7_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH7_SHIFT)

/* 0xA24 : Channel_force_data_4 */
#define TOUCH_FORCE_DATA_CH8_SHIFT                              (0U)
#define TOUCH_FORCE_DATA_CH8_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH8_SHIFT)
#define TOUCH_FORCE_DATA_CH9_SHIFT                              (16U)
#define TOUCH_FORCE_DATA_CH9_MASK                               (0xffff<<TOUCH_FORCE_DATA_CH9_SHIFT)

/* 0xA28 : Channel_force_data_5 */
#define TOUCH_FORCE_DATA_CH10_SHIFT                             (0U)
#define TOUCH_FORCE_DATA_CH10_MASK                              (0xffff<<TOUCH_FORCE_DATA_CH10_SHIFT)
#define TOUCH_FORCE_DATA_CH11_SHIFT                             (16U)
#define TOUCH_FORCE_DATA_CH11_MASK                              (0xffff<<TOUCH_FORCE_DATA_CH11_SHIFT)

/* 0xA2C : Channel_vth_data_0 */
#define TOUCH_VTH_DATA_CH0_SHIFT                                (0U)
#define TOUCH_VTH_DATA_CH0_MASK                                 (0xff<<TOUCH_VTH_DATA_CH0_SHIFT)
#define TOUCH_VTH_DATA_CH1_SHIFT                                (8U)
#define TOUCH_VTH_DATA_CH1_MASK                                 (0xff<<TOUCH_VTH_DATA_CH1_SHIFT)
#define TOUCH_VTH_DATA_CH2_SHIFT                                (16U)
#define TOUCH_VTH_DATA_CH2_MASK                                 (0xff<<TOUCH_VTH_DATA_CH2_SHIFT)
#define TOUCH_VTH_DATA_CH3_SHIFT                                (24U)
#define TOUCH_VTH_DATA_CH3_MASK                                 (0xff<<TOUCH_VTH_DATA_CH3_SHIFT)

/* 0xA30 : Channel_vth_data_1 */
#define TOUCH_VTH_DATA_CH4_SHIFT                                (0U)
#define TOUCH_VTH_DATA_CH4_MASK                                 (0xff<<TOUCH_VTH_DATA_CH4_SHIFT)
#define TOUCH_VTH_DATA_CH5_SHIFT                                (8U)
#define TOUCH_VTH_DATA_CH5_MASK                                 (0xff<<TOUCH_VTH_DATA_CH5_SHIFT)
#define TOUCH_VTH_DATA_CH6_SHIFT                                (16U)
#define TOUCH_VTH_DATA_CH6_MASK                                 (0xff<<TOUCH_VTH_DATA_CH6_SHIFT)
#define TOUCH_VTH_DATA_CH7_SHIFT                                (24U)
#define TOUCH_VTH_DATA_CH7_MASK                                 (0xff<<TOUCH_VTH_DATA_CH7_SHIFT)

/* 0xA34 : Channel_vth_data_2 */
#define TOUCH_VTH_DATA_CH8_SHIFT                                (0U)
#define TOUCH_VTH_DATA_CH8_MASK                                 (0xff<<TOUCH_VTH_DATA_CH8_SHIFT)
#define TOUCH_VTH_DATA_CH9_SHIFT                                (8U)
#define TOUCH_VTH_DATA_CH9_MASK                                 (0xff<<TOUCH_VTH_DATA_CH9_SHIFT)
#define TOUCH_VTH_DATA_CH10_SHIFT                               (16U)
#define TOUCH_VTH_DATA_CH10_MASK                                (0xff<<TOUCH_VTH_DATA_CH10_SHIFT)
#define TOUCH_VTH_DATA_CH11_SHIFT                               (24U)
#define TOUCH_VTH_DATA_CH11_MASK                                (0xff<<TOUCH_VTH_DATA_CH11_SHIFT)

/* 0xA38 : Channel_raw_data_0 */
#define TOUCH_RAW_DATA_CH0_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH0_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH0_SHIFT)

/* 0xA3C : Channel_raw_data_1 */
#define TOUCH_RAW_DATA_CH1_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH1_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH1_SHIFT)

/* 0xA40 : Channel_raw_data_2 */
#define TOUCH_RAW_DATA_CH2_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH2_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH2_SHIFT)

/* 0xA44 : Channel_raw_data_3 */
#define TOUCH_RAW_DATA_CH3_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH3_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH3_SHIFT)

/* 0xA48 : Channel_raw_data_4 */
#define TOUCH_RAW_DATA_CH4_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH4_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH4_SHIFT)

/* 0xA4C : Channel_raw_data_5 */
#define TOUCH_RAW_DATA_CH5_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH5_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH5_SHIFT)

/* 0xA50 : Channel_raw_data_6 */
#define TOUCH_RAW_DATA_CH6_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH6_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH6_SHIFT)

/* 0xA54 : Channel_raw_data_7 */
#define TOUCH_RAW_DATA_CH7_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH7_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH7_SHIFT)

/* 0xA58 : Channel_raw_data_8 */
#define TOUCH_RAW_DATA_CH8_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH8_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH8_SHIFT)

/* 0xA5C : Channel_raw_data_9 */
#define TOUCH_RAW_DATA_CH9_SHIFT                                (0U)
#define TOUCH_RAW_DATA_CH9_MASK                                 (0xffff<<TOUCH_RAW_DATA_CH9_SHIFT)

/* 0xA60 : Channel_raw_data_10 */
#define TOUCH_RAW_DATA_CH10_SHIFT                               (0U)
#define TOUCH_RAW_DATA_CH10_MASK                                (0xffff<<TOUCH_RAW_DATA_CH10_SHIFT)

/* 0xA64 : Channel_raw_data_11 */
#define TOUCH_RAW_DATA_CH11_SHIFT                               (0U)
#define TOUCH_RAW_DATA_CH11_MASK                                (0xffff<<TOUCH_RAW_DATA_CH11_SHIFT)

/* 0xA68 : Channel_LTA_data_0 */
#define TOUCH_LTA_DATA_CH0_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH0_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH0_SHIFT)

/* 0xA6C : Channel_LTA_data_1 */
#define TOUCH_LTA_DATA_CH1_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH1_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH1_SHIFT)

/* 0xA70 : Channel_LTA_data_2 */
#define TOUCH_LTA_DATA_CH2_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH2_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH2_SHIFT)

/* 0xA74 : Channel_LTA_data_3 */
#define TOUCH_LTA_DATA_CH3_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH3_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH3_SHIFT)

/* 0xA78 : Channel_LTA_data_4 */
#define TOUCH_LTA_DATA_CH4_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH4_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH4_SHIFT)

/* 0xA7C : Channel_LTA_data_5 */
#define TOUCH_LTA_DATA_CH5_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH5_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH5_SHIFT)

/* 0xA80 : Channel_LTA_data_6 */
#define TOUCH_LTA_DATA_CH6_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH6_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH6_SHIFT)

/* 0xA84 : Channel_LTA_data_7 */
#define TOUCH_LTA_DATA_CH7_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH7_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH7_SHIFT)

/* 0xA88 : Channel_LTA_data_8 */
#define TOUCH_LTA_DATA_CH8_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH8_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH8_SHIFT)

/* 0xA8C : Channel_LTA_data_9 */
#define TOUCH_LTA_DATA_CH9_SHIFT                                (0U)
#define TOUCH_LTA_DATA_CH9_MASK                                 (0xffff<<TOUCH_LTA_DATA_CH9_SHIFT)

/* 0xA90 : Channel_LTA_data_10 */
#define TOUCH_LTA_DATA_CH10_SHIFT                               (0U)
#define TOUCH_LTA_DATA_CH10_MASK                                (0xffff<<TOUCH_LTA_DATA_CH10_SHIFT)

/* 0xA94 : Channel_LTA_data_11 */
#define TOUCH_LTA_DATA_CH11_SHIFT                               (0U)
#define TOUCH_LTA_DATA_CH11_MASK                                (0xffff<<TOUCH_LTA_DATA_CH11_SHIFT)

/* 0xA98 : Channel_FLT_data_0 */
#define TOUCH_FLT_DATA_CH0_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH0_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH0_SHIFT)

/* 0xA9C : Channel_FLT_data_1 */
#define TOUCH_FLT_DATA_CH1_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH1_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH1_SHIFT)

/* 0xAA0 : Channel_FLT_data_2 */
#define TOUCH_FLT_DATA_CH2_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH2_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH2_SHIFT)

/* 0xAA4 : Channel_FLT_data_3 */
#define TOUCH_FLT_DATA_CH3_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH3_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH3_SHIFT)

/* 0xAA8 : Channel_FLT_data_4 */
#define TOUCH_FLT_DATA_CH4_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH4_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH4_SHIFT)

/* 0xAAC : Channel_FLT_data_5 */
#define TOUCH_FLT_DATA_CH5_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH5_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH5_SHIFT)

/* 0xAB0 : Channel_FLT_data_6 */
#define TOUCH_FLT_DATA_CH6_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH6_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH6_SHIFT)

/* 0xAB4 : Channel_FLT_data_7 */
#define TOUCH_FLT_DATA_CH7_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH7_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH7_SHIFT)

/* 0xAB8 : Channel_FLT_data_8 */
#define TOUCH_FLT_DATA_CH8_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH8_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH8_SHIFT)

/* 0xABC : Channel_FLT_data_9 */
#define TOUCH_FLT_DATA_CH9_SHIFT                                (0U)
#define TOUCH_FLT_DATA_CH9_MASK                                 (0xffff<<TOUCH_FLT_DATA_CH9_SHIFT)

/* 0xAC0 : Channel_FLT_data_10 */
#define TOUCH_FLT_DATA_CH10_SHIFT                               (0U)
#define TOUCH_FLT_DATA_CH10_MASK                                (0xffff<<TOUCH_FLT_DATA_CH10_SHIFT)

/* 0xAC4 : Channel_FLT_data_11 */
#define TOUCH_FLT_DATA_CH11_SHIFT                               (0U)
#define TOUCH_FLT_DATA_CH11_MASK                                (0xffff<<TOUCH_FLT_DATA_CH11_SHIFT)

/* 0xAC8 : touch_rsvd */
#define TOUCH_RESERVED_SHIFT                                    (0U)
#define TOUCH_RESERVED_MASK                                     (0xff<<TOUCH_RESERVED_SHIFT)

/* 0xACC : touch_int_setting */
#define TOUCH_INT_CLR_SHIFT                                     (0U)
#define TOUCH_INT_CLR_MASK                                      (0xfff<<TOUCH_INT_CLR_SHIFT)
#define TOUCH_INT_MASK_SHIFT                                    (16U)
#define TOUCH_INT_MASK_MASK                                     (0xfff<<TOUCH_INT_MASK_SHIFT)
#define TOUCH_INT_EN                                            (1<<31U)

/* 0xAD0 : touch_int_status */
#define TOUCH_INT_STATUS_SHIFT                                  (0U)
#define TOUCH_INT_STATUS_MASK                                   (0xfff<<TOUCH_INT_STATUS_SHIFT)
#define TOUCH_END_FLAG                                          (1<<12U)


#endif  /* __TOUCH_REG_H__ */
