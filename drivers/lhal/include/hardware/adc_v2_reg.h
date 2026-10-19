/**
  ******************************************************************************
  * @file    adc_v2_reg.h
  * @version V1.0
  * @date    2024-11-06
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
#ifndef  __HARDWARE_ADC_V2_H__
#define  __HARDWARE_ADC_V2_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

/* gpip base */
#define GPIP_GPADC_CONFIG_OFFSET                                (0x0)/* gpadc_config */
#define GPIP_GPADC_DMA_RDATA_OFFSET                             (0x4)/* gpadc_dma_rdata */

/* glb base */
#define GLB_GPADC_REG_CMD1_OFFSET                               (0x858)/* Digital Control */
#define GLB_GPADC1_REG_CMD1_OFFSET                              (0x85C)/* Digital Control */
#define GLB_GPADC1_REG_CONFIG1_OFFSET                           (0x860)/*  */
#define GLB_GPADC1_REG_CONFIG2_OFFSET                           (0x864)/*  */
#define GLB_GPADC1_REG_STATUS_OFFSET                            (0x868)/*  */
#define GLB_GPADC1_REG_RAW_RESULT_OFFSET                        (0x86C)/*  */
#define GLB_GPADC_TEN_OFFSET                                    (0x870)/*  */
#define GLB_GPADC_REG_SCN_POS1_OFFSET                           (0x874)/*  */
#define GLB_GPADC_REG_SCN_POS2_OFFSET                           (0x878)/*  */
#define GLB_GPADC_REG_SCN_NEG1_OFFSET                           (0x87C)/*  */
#define GLB_GPADC_REG_SCN_NEG2_OFFSET                           (0x880)/*  */
#define GLB_GPADC_REG_INSERT_POS1_OFFSET                        (0x884)/*  */
#define GLB_GPADC_REG_INSERT_POS2_OFFSET                        (0x888)/*  */
#define GLB_GPADC_REG_INSERT_NEG1_OFFSET                        (0x88C)/*  */
#define GLB_GPADC_REG_INSERT_NEG2_OFFSET                        (0x890)/*  */
#define GLB_GPADC1_REG_INSERT_RAW_RESULT_OFFSET                 (0x894)/*  */
#define GLB_GPADC_INT_INSERT_NEG_OFFSET                         (0x898)/*  */
#define GLB_GPADC_INT_INSERT_POS_OFFSET                         (0x89c)/* gpadc_int_insert_pos */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : gpadc_config */
#define GPIP_GPADC_DMA_EN                                       (1<<0U)
#define GPIP_GPADC_FIFO_CLR                                     (1<<1U)
#define GPIP_GPADC_FIFO_NE                                      (1<<2U)
#define GPIP_GPADC_FIFO_FULL                                    (1<<3U)
#define GPIP_GPADC_RDY                                          (1<<4U)
#define GPIP_GPADC_FIFO_OVERRUN                                 (1<<5U)
#define GPIP_GPADC_FIFO_UNDERRUN                                (1<<6U)
#define GPIP_GPADC_RDY_CLR                                      (1<<8U)
#define GPIP_GPADC_FIFO_OVERRUN_CLR                             (1<<9U)
#define GPIP_GPADC_FIFO_UNDERRUN_CLR                            (1<<10U)
#define GPIP_GPADC_RDY_MASK                                     (1<<12U)
#define GPIP_GPADC_FIFO_OVERRUN_MASK                            (1<<13U)
#define GPIP_GPADC_FIFO_UNDERRUN_MASK                           (1<<14U)
#define GPIP_GPADC_FIFO_DATA_COUNT_SHIFT                        (16U)
#define GPIP_GPADC_FIFO_DATA_COUNT_MASK                         (0x3f<<GPIP_GPADC_FIFO_DATA_COUNT_SHIFT)
#define GPIP_GPADC_FIFO_THL_SHIFT                               (22U)
#define GPIP_GPADC_FIFO_THL_MASK                                (0x3<<GPIP_GPADC_FIFO_THL_SHIFT)
#define GPIP_GPADC_INT_RDY_NUM_SHIFT                            (24U)
#define GPIP_GPADC_INT_RDY_NUM_MASK                             (0x3f<<GPIP_GPADC_INT_RDY_NUM_SHIFT)

/* 0x4 : gpadc_dma_rdata */
#define GPIP_GPADC_DMA_RDATA_SHIFT                              (0U)
#define GPIP_GPADC_DMA_RDATA_MASK                               (0x3ffffff<<GPIP_GPADC_DMA_RDATA_SHIFT)

/* 0x858 : Digital Control */
#define GLB_GPADC_TS_EN                                         (1<<0U)
#define GLB_GPADC_TSVBE_LOW                                     (1<<1U)
#define GLB_GPADC_VBAT_EN                                       (1<<2U)
#define GLB_GPADC_BIAS_SEL                                      (1<<3U)
#define GLB_GPADC_INT_NEG_SEL_SHIFT                             (4U)
#define GLB_GPADC_INT_NEG_SEL_MASK                              (0x1f<<GLB_GPADC_INT_NEG_SEL_SHIFT)
#define GLB_GPADC_INT_POS_SEL_SHIFT                             (12U)
#define GLB_GPADC_INT_POS_SEL_MASK                              (0x1f<<GLB_GPADC_INT_POS_SEL_SHIFT)
#define GLB_GPADC_CHIP_SEN_PU                                   (1<<20U)
#define GLB_GPADC_CHIP_SEN_SEL_SHIFT                            (21U)
#define GLB_GPADC_CHIP_SEN_SEL_MASK                             (0xf<<GLB_GPADC_CHIP_SEN_SEL_SHIFT)
#define GLB_GPADC_DVDD_SEL_SHIFT                                (25U)
#define GLB_GPADC_DVDD_SEL_MASK                                 (0x3<<GLB_GPADC_DVDD_SEL_SHIFT)
#define GLB_GPADC_TSEXT_SEL                                     (1<<27U)
#define GLB_GPADC_VREF_EX_CH_SEL_SHIFT                          (28U)
#define GLB_GPADC_VREF_EX_CH_SEL_MASK                           (0xf<<GLB_GPADC_VREF_EX_CH_SEL_SHIFT)

/* 0x85C : Digital Control */
#define GLB_GPADC1_EXT_POS_SEL_SHIFT                            (0U)
#define GLB_GPADC1_EXT_POS_SEL_MASK                             (0x1f<<GLB_GPADC1_EXT_POS_SEL_SHIFT)
#define GLB_GPADC1_EXT_NEG_SEL_SHIFT                            (5U)
#define GLB_GPADC1_EXT_NEG_SEL_MASK                             (0x1f<<GLB_GPADC1_EXT_NEG_SEL_SHIFT)

/* 0x860 :  */
#define GLB_GPADC1_VREF_EX_SEL                                  (1<<0U)
#define GLB_GPADC1_DIFF_MODE                                    (1<<1U)
#define GLB_GPADC1_NEG_GND                                      (1<<2U)
#define GLB_GPADC1_PGA_VCMI_EN                                  (1<<3U)
#define GLB_GPADC1_VREF_TRIM_SHIFT                              (4U)
#define GLB_GPADC1_VREF_TRIM_MASK                               (0x3f<<GLB_GPADC1_VREF_TRIM_SHIFT)
#define GLB_GPADC1_PWM_TRG_EN                                   (1<<10U)
#define GLB_GPADC1_CHOP_MODE_SHIFT                              (12U)
#define GLB_GPADC1_CHOP_MODE_MASK                               (0x3<<GLB_GPADC1_CHOP_MODE_SHIFT)
#define GLB_GPADC1_PGA_EN                                       (1<<16U)
#define GLB_GPADC1_PGA_GAIN_SHIFT                               (17U)
#define GLB_GPADC1_PGA_GAIN_MASK                                (0x7<<GLB_GPADC1_PGA_GAIN_SHIFT)
#define GLB_GPADC1_PGA_VCM_SHIFT                                (20U)
#define GLB_GPADC1_PGA_VCM_MASK                                 (0x3<<GLB_GPADC1_PGA_VCM_SHIFT)
#define GLB_GPADC1_INT_INSERT_LEN_SHIFT                         (23U)
#define GLB_GPADC1_INT_INSERT_LEN_MASK                          (0x7<<GLB_GPADC1_INT_INSERT_LEN_SHIFT)
#define GLB_GPADC1_EXT_INSERT_LEN_SHIFT                         (26U)
#define GLB_GPADC1_EXT_INSERT_LEN_MASK                          (0xf<<GLB_GPADC1_EXT_INSERT_LEN_SHIFT)
#define GLB_GPADC1_INT_EXT_INSERT_SEL                           (1<<30U)
#define GLB_GPADC1_INSERT                                       (1<<31U)

/* 0x864 :  */
#define GLB_GPADC1_GLOBAL_EN                                    (1<<0U)
#define GLB_GPADC1_CONV_START                                   (1<<1U)
#define GLB_GPADC1_SOFT_RST                                     (1<<2U)
#define GLB_GPADC1_DWA_EN                                       (1<<3U)
#define GLB_GPADC1_DITHER_EN                                    (1<<4U)
#define GLB_GPADC1_DLY_SEL_SHIFT                                (5U)
#define GLB_GPADC1_DLY_SEL_MASK                                 (0x7<<GLB_GPADC1_DLY_SEL_SHIFT)
#define GLB_GPADC1_RNG                                          (1<<8U)
#define GLB_GPADC1_CONT_CONV_EN                                 (1<<9U)
#define GLB_GPADC1_SCN_EN                                       (1<<10U)
#define GLB_GPADC1_SCN_LOOP_EN                                  (1<<11U)
#define GLB_GPADC1_SCN_LEN_SHIFT                                (12U)
#define GLB_GPADC1_SCN_LEN_MASK                                 (0xf<<GLB_GPADC1_SCN_LEN_SHIFT)
#define GLB_GPADC1_CHL_SEL_NUM_SHIFT                            (16U)
#define GLB_GPADC1_CHL_SEL_NUM_MASK                             (0xff<<GLB_GPADC1_CHL_SEL_NUM_SHIFT)
#define GLB_GPADC1_RES_SEL_SHIFT                                (24U)
#define GLB_GPADC1_RES_SEL_MASK                                 (0x7<<GLB_GPADC1_RES_SEL_SHIFT)

/* 0x868 :  */
#define GLB_GPADC1_RESERVED_SHIFT                               (16U)
#define GLB_GPADC1_RESERVED_MASK                                (0xffff<<GLB_GPADC1_RESERVED_SHIFT)

/* 0x86C :  */
#define GLB_GPADC1_RAW_DATA_SHIFT                               (0U)
#define GLB_GPADC1_RAW_DATA_MASK                                (0xffff<<GLB_GPADC1_RAW_DATA_SHIFT)
#define GLB_GPADC1_EXT_NEG_CHL_SHIFT                            (16U)
#define GLB_GPADC1_EXT_NEG_CHL_MASK                             (0x1f<<GLB_GPADC1_EXT_NEG_CHL_SHIFT)
#define GLB_GPADC1_EXT_POS_CHL_SHIFT                            (21U)
#define GLB_GPADC1_EXT_POS_CHL_MASK                             (0x1f<<GLB_GPADC1_EXT_POS_CHL_SHIFT)
#define GLB_GPADC1_DATA_RDY                                     (1<<26U)

/* 0x870 :  */
#define GLB_TEN_GPADC0                                          (1<<0U)
#define GLB_TEN_GPADC1                                          (1<<1U)
#define GLB_TEN_GPADC2                                          (1<<2U)

/* 0x874 :  */
#define GLB_GPADC_SCAN_POS_0_SHIFT                              (0U)
#define GLB_GPADC_SCAN_POS_0_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_0_SHIFT)
#define GLB_GPADC_SCAN_POS_1_SHIFT                              (5U)
#define GLB_GPADC_SCAN_POS_1_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_1_SHIFT)
#define GLB_GPADC_SCAN_POS_2_SHIFT                              (10U)
#define GLB_GPADC_SCAN_POS_2_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_2_SHIFT)
#define GLB_GPADC_SCAN_POS_3_SHIFT                              (15U)
#define GLB_GPADC_SCAN_POS_3_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_3_SHIFT)
#define GLB_GPADC_SCAN_POS_4_SHIFT                              (20U)
#define GLB_GPADC_SCAN_POS_4_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_4_SHIFT)
#define GLB_GPADC_SCAN_POS_5_SHIFT                              (25U)
#define GLB_GPADC_SCAN_POS_5_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_5_SHIFT)

/* 0x878 :  */
#define GLB_GPADC_SCAN_POS_6_SHIFT                              (0U)
#define GLB_GPADC_SCAN_POS_6_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_6_SHIFT)
#define GLB_GPADC_SCAN_POS_7_SHIFT                              (5U)
#define GLB_GPADC_SCAN_POS_7_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_7_SHIFT)
#define GLB_GPADC_SCAN_POS_8_SHIFT                              (10U)
#define GLB_GPADC_SCAN_POS_8_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_8_SHIFT)
#define GLB_GPADC_SCAN_POS_9_SHIFT                              (15U)
#define GLB_GPADC_SCAN_POS_9_MASK                               (0x1f<<GLB_GPADC_SCAN_POS_9_SHIFT)
#define GLB_GPADC_SCAN_POS_10_SHIFT                             (20U)
#define GLB_GPADC_SCAN_POS_10_MASK                              (0x1f<<GLB_GPADC_SCAN_POS_10_SHIFT)
#define GLB_GPADC_SCAN_POS_11_SHIFT                             (25U)
#define GLB_GPADC_SCAN_POS_11_MASK                              (0x1f<<GLB_GPADC_SCAN_POS_11_SHIFT)

/* 0x87C :  */
#define GLB_GPADC_SCAN_NEG_0_SHIFT                              (0U)
#define GLB_GPADC_SCAN_NEG_0_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_0_SHIFT)
#define GLB_GPADC_SCAN_NEG_1_SHIFT                              (5U)
#define GLB_GPADC_SCAN_NEG_1_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_1_SHIFT)
#define GLB_GPADC_SCAN_NEG_2_SHIFT                              (10U)
#define GLB_GPADC_SCAN_NEG_2_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_2_SHIFT)
#define GLB_GPADC_SCAN_NEG_3_SHIFT                              (15U)
#define GLB_GPADC_SCAN_NEG_3_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_3_SHIFT)
#define GLB_GPADC_SCAN_NEG_4_SHIFT                              (20U)
#define GLB_GPADC_SCAN_NEG_4_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_4_SHIFT)
#define GLB_GPADC_SCAN_NEG_5_SHIFT                              (25U)
#define GLB_GPADC_SCAN_NEG_5_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_5_SHIFT)

/* 0x880 :  */
#define GLB_GPADC_SCAN_NEG_6_SHIFT                              (0U)
#define GLB_GPADC_SCAN_NEG_6_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_6_SHIFT)
#define GLB_GPADC_SCAN_NEG_7_SHIFT                              (5U)
#define GLB_GPADC_SCAN_NEG_7_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_7_SHIFT)
#define GLB_GPADC_SCAN_NEG_8_SHIFT                              (10U)
#define GLB_GPADC_SCAN_NEG_8_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_8_SHIFT)
#define GLB_GPADC_SCAN_NEG_9_SHIFT                              (15U)
#define GLB_GPADC_SCAN_NEG_9_MASK                               (0x1f<<GLB_GPADC_SCAN_NEG_9_SHIFT)
#define GLB_GPADC_SCAN_NEG_10_SHIFT                             (20U)
#define GLB_GPADC_SCAN_NEG_10_MASK                              (0x1f<<GLB_GPADC_SCAN_NEG_10_SHIFT)
#define GLB_GPADC_SCAN_NEG_11_SHIFT                             (25U)
#define GLB_GPADC_SCAN_NEG_11_MASK                              (0x1f<<GLB_GPADC_SCAN_NEG_11_SHIFT)

/* 0x884 :  */
#define GLB_GPADC_INSERT_POS_0_SHIFT                            (0U)
#define GLB_GPADC_INSERT_POS_0_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_0_SHIFT)
#define GLB_GPADC_INSERT_POS_1_SHIFT                            (5U)
#define GLB_GPADC_INSERT_POS_1_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_1_SHIFT)
#define GLB_GPADC_INSERT_POS_2_SHIFT                            (10U)
#define GLB_GPADC_INSERT_POS_2_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_2_SHIFT)
#define GLB_GPADC_INSERT_POS_3_SHIFT                            (15U)
#define GLB_GPADC_INSERT_POS_3_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_3_SHIFT)
#define GLB_GPADC_INSERT_POS_4_SHIFT                            (20U)
#define GLB_GPADC_INSERT_POS_4_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_4_SHIFT)
#define GLB_GPADC_INSERT_POS_5_SHIFT                            (25U)
#define GLB_GPADC_INSERT_POS_5_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_5_SHIFT)

/* 0x888 :  */
#define GLB_GPADC_INSERT_POS_6_SHIFT                            (0U)
#define GLB_GPADC_INSERT_POS_6_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_6_SHIFT)
#define GLB_GPADC_INSERT_POS_7_SHIFT                            (5U)
#define GLB_GPADC_INSERT_POS_7_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_7_SHIFT)
#define GLB_GPADC_INSERT_POS_8_SHIFT                            (10U)
#define GLB_GPADC_INSERT_POS_8_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_8_SHIFT)
#define GLB_GPADC_INSERT_POS_9_SHIFT                            (15U)
#define GLB_GPADC_INSERT_POS_9_MASK                             (0x1f<<GLB_GPADC_INSERT_POS_9_SHIFT)
#define GLB_GPADC_INSERT_POS_10_SHIFT                           (20U)
#define GLB_GPADC_INSERT_POS_10_MASK                            (0x1f<<GLB_GPADC_INSERT_POS_10_SHIFT)
#define GLB_GPADC_INSERT_POS_11_SHIFT                           (25U)
#define GLB_GPADC_INSERT_POS_11_MASK                            (0x1f<<GLB_GPADC_INSERT_POS_11_SHIFT)

/* 0x88C :  */
#define GLB_GPADC_INSERT_NEG_0_SHIFT                            (0U)
#define GLB_GPADC_INSERT_NEG_0_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_0_SHIFT)
#define GLB_GPADC_INSERT_NEG_1_SHIFT                            (5U)
#define GLB_GPADC_INSERT_NEG_1_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_1_SHIFT)
#define GLB_GPADC_INSERT_NEG_2_SHIFT                            (10U)
#define GLB_GPADC_INSERT_NEG_2_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_2_SHIFT)
#define GLB_GPADC_INSERT_NEG_3_SHIFT                            (15U)
#define GLB_GPADC_INSERT_NEG_3_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_3_SHIFT)
#define GLB_GPADC_INSERT_NEG_4_SHIFT                            (20U)
#define GLB_GPADC_INSERT_NEG_4_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_4_SHIFT)
#define GLB_GPADC_INSERT_NEG_5_SHIFT                            (25U)
#define GLB_GPADC_INSERT_NEG_5_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_5_SHIFT)

/* 0x890 :  */
#define GLB_GPADC_INSERT_NEG_6_SHIFT                            (0U)
#define GLB_GPADC_INSERT_NEG_6_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_6_SHIFT)
#define GLB_GPADC_INSERT_NEG_7_SHIFT                            (5U)
#define GLB_GPADC_INSERT_NEG_7_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_7_SHIFT)
#define GLB_GPADC_INSERT_NEG_8_SHIFT                            (10U)
#define GLB_GPADC_INSERT_NEG_8_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_8_SHIFT)
#define GLB_GPADC_INSERT_NEG_9_SHIFT                            (15U)
#define GLB_GPADC_INSERT_NEG_9_MASK                             (0x1f<<GLB_GPADC_INSERT_NEG_9_SHIFT)
#define GLB_GPADC_INSERT_NEG_10_SHIFT                           (20U)
#define GLB_GPADC_INSERT_NEG_10_MASK                            (0x1f<<GLB_GPADC_INSERT_NEG_10_SHIFT)
#define GLB_GPADC_INSERT_NEG_11_SHIFT                           (25U)
#define GLB_GPADC_INSERT_NEG_11_MASK                            (0x1f<<GLB_GPADC_INSERT_NEG_11_SHIFT)

/* 0x894 :  */
#define GLB_GPADC1_INSERT_RAW_DATA_SHIFT                        (0U)
#define GLB_GPADC1_INSERT_RAW_DATA_MASK                         (0xffff<<GLB_GPADC1_INSERT_RAW_DATA_SHIFT)
#define GLB_GPADC1_INSERT_NEG_CHL_SHIFT                         (16U)
#define GLB_GPADC1_INSERT_NEG_CHL_MASK                          (0x1f<<GLB_GPADC1_INSERT_NEG_CHL_SHIFT)
#define GLB_GPADC1_INSERT_POS_CHL_SHIFT                         (21U)
#define GLB_GPADC1_INSERT_POS_CHL_MASK                          (0x1f<<GLB_GPADC1_INSERT_POS_CHL_SHIFT)
#define GLB_GPADC1_INSERT_DATA_RDY                              (1<<26U)

/* 0x898 :  */
#define GLB_GPADC_INT_INSERT_NEG_0_SHIFT                        (0U)
#define GLB_GPADC_INT_INSERT_NEG_0_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_NEG_0_SHIFT)
#define GLB_GPADC_INT_INSERT_NEG_1_SHIFT                        (5U)
#define GLB_GPADC_INT_INSERT_NEG_1_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_NEG_1_SHIFT)
#define GLB_GPADC_INT_INSERT_NEG_2_SHIFT                        (10U)
#define GLB_GPADC_INT_INSERT_NEG_2_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_NEG_2_SHIFT)
#define GLB_GPADC_INT_INSERT_NEG_3_SHIFT                        (15U)
#define GLB_GPADC_INT_INSERT_NEG_3_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_NEG_3_SHIFT)

/* 0x89c : gpadc_int_insert_pos */
#define GLB_GPADC_INT_INSERT_POS_0_SHIFT                        (0U)
#define GLB_GPADC_INT_INSERT_POS_0_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_POS_0_SHIFT)
#define GLB_GPADC_INT_INSERT_POS_1_SHIFT                        (5U)
#define GLB_GPADC_INT_INSERT_POS_1_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_POS_1_SHIFT)
#define GLB_GPADC_INT_INSERT_POS_2_SHIFT                        (10U)
#define GLB_GPADC_INT_INSERT_POS_2_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_POS_2_SHIFT)
#define GLB_GPADC_INT_INSERT_POS_3_SHIFT                        (15U)
#define GLB_GPADC_INT_INSERT_POS_3_MASK                         (0x1f<<GLB_GPADC_INT_INSERT_POS_3_SHIFT)

#endif  /* __HARDWARE_ADC_V2_H__ */
