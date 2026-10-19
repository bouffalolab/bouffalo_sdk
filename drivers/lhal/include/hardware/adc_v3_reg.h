/**
  ******************************************************************************
  * @file    adc_v3_reg.h
  * @version V1.0
  * @date    2024-10-16
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
#ifndef  __HARDWARE_ADC_V3_H__
#define  __HARDWARE_ADC_V3_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

/* gpip base */
#define GPIP_GPADC_CONFIG_OFFSET                                (0x0)/* gpadc_config */
#define GPIP_GPADC_DMA_INT_STS_OFFSET                           (0x4)/* gpadc_dma_int_sts */
#define GPIP_GPADC_DMA_INT_MASK_OFFSET                          (0x8)/* gpadc_dma_int_mask */
#define GPIP_GPADC_DMA_INT_CLEAR_OFFSET                         (0xC)/* gpadc_dma_int_clear */
#define GPIP_GPADC_REGULAR_FIFO_OFFSET                          (0x10)/* gpadc_regular_fifo */
#define GPIP_GPADC_INJECTION_FIFO0_OFFSET                       (0x14)/* gpadc_injection_fifo0 */
#define GPIP_GPADC_INJECTION_FIFO1_OFFSET                       (0x18)/* gpadc_injection_fifo1 */
#define GPIP_GPADC_INJECTION_FIFO2_OFFSET                       (0x1C)/* gpadc_injection_fifo2 */
#define GPIP_GPADC_INJECTION_FIFO3_OFFSET                       (0x20)/* gpadc_injection_fifo3 */
#define GPIP_GPADC_PRE_PROC_OFFSET                              (0x24)/* gpadc_pre_proc */
#define GPIP_GPADC_WD_CTRL_0_OFFSET                             (0x28)/* gpadc_wd_ctrl_0 */
#define GPIP_GPADC_WD_CTRL_1_OFFSET                             (0x2C)/* gpadc_wd_ctrl_1 */
#define GPIP_GPADC2_FIFO_ENABLE_OFFSET                          (0x30)/* gpadc2_fifo_enable */
#define GPIP_GPADC_FIFO2_CONFIG_OFFSET                          (0x34)/* gpadc_fifo2_config */
#define GPIP_GPADC2_DMA_INT_STS_OFFSET                          (0x38)/* gpadc2_dma_int_sts */
#define GPIP_GPADC2_DMA_INT_MASK_OFFSET                         (0x3C)/* gpadc2_dma_int_mask */
#define GPIP_GPADC2_DMA_INT_CLEAR_OFFSET                        (0x40)/* gpadc2_dma_int_clear */
#define GPIP_GPADC_REGULAR_FIFO2_OFFSET                         (0x44)/* gpadc_regular_fifo2 */
#define GPIP_GPADC_INJECTION_FIFO4_OFFSET                       (0x48)/* gpadc_injection_fifo4 */
#define GPIP_GPADC_INJECTION_FIFO5_OFFSET                       (0x4C)/* gpadc_injection_fifo5 */
#define GPIP_GPADC_INJECTION_FIFO6_OFFSET                       (0x50)/* gpadc_injection_fifo6 */
#define GPIP_GPADC_INJECTION_FIFO7_OFFSET                       (0x54)/* gpadc_injection_fifo7 */
#define GPIP_GPDAC_CONFIG_OFFSET                                (0x80)/* gpdac_config */
#define GPIP_GPDAC_DMA_CONFIG_OFFSET                            (0x84)/* gpdac_dma_config */
#define GPIP_GPDAC_DMA_WDATA_OFFSET                             (0x88)/* gpdac_dma_wdata */
#define GPIP_GPDAC_TX_FIFO_STATUS_OFFSET                        (0x8C)/* gpdac_tx_fifo_status */

/* aon base */
#define AON_GPADC_REG_CMD1_OFFSET                               (0x90C)/* gpadc_reg_cmd1 */
#define AON_GPADC_REG_CMD2_OFFSET                               (0x910)/* gpadc_reg_cmd2 */
#define AON_GPADC1_REG_CONFIG1_OFFSET                           (0x914)/* gpadc1_reg_config1 */
#define AON_GPADC1_REG_CONFIG2_OFFSET                           (0x918)/* gpadc1_reg_config2 */
#define AON_GPADC1_REG_CTRL_0_OFFSET                            (0x91C)/* gpadc1_reg_ctrl_0 */
#define AON_GPADC1_REG_OS_CAL_DATA_OFFSET                       (0x920)/* gpadc1_reg_os_cal_data */
#define AON_GPADC1_REG_SCAN_0_OFFSET                            (0x924)/* gpadc1_reg_scan_0 */
#define AON_GPADC1_REG_SCAN_1_OFFSET                            (0x928)/* gpadc1_reg_scan_1 */
#define AON_GPADC1_REG_SCAN_2_OFFSET                            (0x92C)/* gpadc1_reg_scan_2 */
#define AON_GPADC1_REG_SCAN_3_OFFSET                            (0x930)/* gpadc1_reg_scan_3 */
#define AON_GPADC1_REG_IJCT_EXT_0_OFFSET                        (0x934)/* gpadc1_reg_ijct_ext_0 */
#define AON_GPADC1_REG_IJCT_INT_0_OFFSET                        (0x938)/* gpadc1_reg_ijct_int_0 */
#define AON_GPADC1_REG_IJCT_INT_1_OFFSET                        (0x93C)/* gpadc1_reg_ijct_int_1 */
#define AON_GPADC1_REG_RESULT_OFFSET                            (0x940)/* gpadc1_reg_result */
#define AON_GPADC1_REG_RAW_DATA_OFFSET                          (0x944)/* gpadc1_reg_raw_data */
#define AON_GPADC1_REG_SATUR_OFFSET                             (0x948)/* gpadc1_reg_satur */
#define AON_GPADC2_REG_CONFIG1_OFFSET                           (0x94C)/* gpadc2_reg_config1 */
#define AON_GPADC2_REG_CONFIG2_OFFSET                           (0x950)/* gpadc2_reg_config2 */
#define AON_GPADC2_REG_CTRL_0_OFFSET                            (0x954)/* gpadc2_reg_ctrl_0 */
#define AON_GPADC2_REG_OS_CAL_DATA_OFFSET                       (0x958)/* gpadc2_reg_os_cal_data */
#define AON_GPADC2_REG_SCAN_0_OFFSET                            (0x95C)/* gpadc2_reg_scan_0 */
#define AON_GPADC2_REG_SCAN_1_OFFSET                            (0x960)/* gpadc2_reg_scan_1 */
#define AON_GPADC2_REG_SCAN_2_OFFSET                            (0x964)/* gpadc2_reg_scan_2 */
#define AON_GPADC2_REG_SCAN_3_OFFSET                            (0x968)/* gpadc2_reg_scan_3 */
#define AON_GPADC2_REG_IJCT_EXT_0_OFFSET                        (0x96C)/* gpadc2_reg_ijct_ext_0 */
#define AON_GPADC2_REG_RESULT_OFFSET                            (0x970)/* gpadc2_reg_result */
#define AON_GPADC2_REG_RAW_DATA_OFFSET                          (0x974)/* gpadc2_reg_raw_data */
#define AON_GPADC2_REG_SATUR_OFFSET                             (0x978)/* gpadc2_reg_satur */
#define AON_GPADC_TEN_OFFSET                                    (0x97C)/* gpadc_ten */

#define AON_GPADC2_OFFSET_COMPENSATION                          (0x008)/* compensate gpadc2_reg_satur */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : gpadc_config */
#define GPIP_GPADC_FIFO_DMA_EN                                  (1<<0U)
#define GPIP_GPADC_FIFO_CLR                                     (1<<1U)
#define GPIP_GPADC_FIFO_THL_SHIFT                               (2U)
#define GPIP_GPADC_FIFO_THL_MASK                                (0x3<<GPIP_GPADC_FIFO_THL_SHIFT)
#define GPIP_GPADC_FIFO_DATA_COUNT_SHIFT                        (4U)
#define GPIP_GPADC_FIFO_DATA_COUNT_MASK                         (0x3f<<GPIP_GPADC_FIFO_DATA_COUNT_SHIFT)
#define GPIP_GPADC_FIFO_NE                                      (1<<10U)
#define GPIP_GPADC_FIFO_FULL                                    (1<<11U)
#define GPIP_GPADC_IJCT_FIFO0_CLR                               (1<<12U)
#define GPIP_GPADC_IJCT_FIFO1_CLR                               (1<<13U)
#define GPIP_GPADC_IJCT_FIFO2_CLR                               (1<<14U)
#define GPIP_GPADC_IJCT_FIFO3_CLR                               (1<<15U)
#define GPIP_GPADC_IJCT_FIFO0_DATA_COUNT_SHIFT                  (16U)
#define GPIP_GPADC_IJCT_FIFO0_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO0_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO0_NE                                (1<<18U)
#define GPIP_GPADC_IJCT_FIFO0_FULL                              (1<<19U)
#define GPIP_GPADC_IJCT_FIFO1_DATA_COUNT_SHIFT                  (20U)
#define GPIP_GPADC_IJCT_FIFO1_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO1_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO1_NE                                (1<<22U)
#define GPIP_GPADC_IJCT_FIFO1_FULL                              (1<<23U)
#define GPIP_GPADC_IJCT_FIFO2_DATA_COUNT_SHIFT                  (24U)
#define GPIP_GPADC_IJCT_FIFO2_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO2_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO2_NE                                (1<<26U)
#define GPIP_GPADC_IJCT_FIFO2_FULL                              (1<<27U)
#define GPIP_GPADC_IJCT_FIFO3_DATA_COUNT_SHIFT                  (28U)
#define GPIP_GPADC_IJCT_FIFO3_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO3_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO3_NE                                (1<<30U)
#define GPIP_GPADC_IJCT_FIFO3_FULL                              (1<<31U)

/* 0x4 : gpadc_dma_int_sts */
#define GPIP_GPADC_FIFO_RDY                                     (1<<0U)
#define GPIP_GPADC_FIFO_OVERRUN                                 (1<<1U)
#define GPIP_GPADC_FIFO_UNDERRUN                                (1<<2U)
#define GPIP_GPADC_IJCT_FIFO0_RDY                               (1<<3U)
#define GPIP_GPADC_IJCT_FIFO0_OVERRUN                           (1<<4U)
#define GPIP_GPADC_IJCT_FIFO0_UNDERRUN                          (1<<5U)
#define GPIP_GPADC_IJCT_FIFO1_RDY                               (1<<6U)
#define GPIP_GPADC_IJCT_FIFO1_OVERRUN                           (1<<7U)
#define GPIP_GPADC_IJCT_FIFO1_UNDERRUN                          (1<<8U)
#define GPIP_GPADC_IJCT_FIFO2_RDY                               (1<<9U)
#define GPIP_GPADC_IJCT_FIFO2_OVERRUN                           (1<<10U)
#define GPIP_GPADC_IJCT_FIFO2_UNDERRUN                          (1<<11U)
#define GPIP_GPADC_IJCT_FIFO3_RDY                               (1<<12U)
#define GPIP_GPADC_IJCT_FIFO3_OVERRUN                           (1<<13U)
#define GPIP_GPADC_IJCT_FIFO3_UNDERRUN                          (1<<14U)
#define GPIP_GPADC1_SCAN_DONE                                   (1<<15U)
#define GPIP_GPADC1_IJCT_DONE                                   (1<<16U)
#define GPIP_GPADC2_SCAN_DONE                                   (1<<17U)
#define GPIP_GPADC2_IJCT_DONE                                   (1<<18U)
#define GPIP_GPADC_WD_INT                                       (1<<19U)

/* 0x8 : gpadc_dma_int_mask */
#define GPIP_GPADC_FIFO_RDY_MASK                                (1<<0U)
#define GPIP_GPADC_FIFO_OVERRUN_MASK                            (1<<1U)
#define GPIP_GPADC_FIFO_UNDERRUN_MASK                           (1<<2U)
#define GPIP_GPADC_IJCT_FIFO0_RDY_MASK                          (1<<3U)
#define GPIP_GPADC_IJCT_FIFO0_OVERRUN_MASK                      (1<<4U)
#define GPIP_GPADC_IJCT_FIFO0_UNDERRUN_MASK                     (1<<5U)
#define GPIP_GPADC_IJCT_FIFO1_RDY_MASK                          (1<<6U)
#define GPIP_GPADC_IJCT_FIFO1_OVERRUN_MASK                      (1<<7U)
#define GPIP_GPADC_IJCT_FIFO1_UNDERRUN_MASK                     (1<<8U)
#define GPIP_GPADC_IJCT_FIFO2_RDY_MASK                          (1<<9U)
#define GPIP_GPADC_IJCT_FIFO2_OVERRUN_MASK                      (1<<10U)
#define GPIP_GPADC_IJCT_FIFO2_UNDERRUN_MASK                     (1<<11U)
#define GPIP_GPADC_IJCT_FIFO3_RDY_MASK                          (1<<12U)
#define GPIP_GPADC_IJCT_FIFO3_OVERRUN_MASK                      (1<<13U)
#define GPIP_GPADC_IJCT_FIFO3_UNDERRUN_MASK                     (1<<14U)
#define GPIP_GPADC1_SCAN_DONE_MASK                              (1<<15U)
#define GPIP_GPADC1_IJCT_DONE_MASK                              (1<<16U)
#define GPIP_GPADC2_SCAN_DONE_MASK                              (1<<17U)
#define GPIP_GPADC2_IJCT_DONE_MASK                              (1<<18U)
#define GPIP_GPADC_WD_INT_MASK                                  (1<<19U)

/* 0xC : gpadc_dma_int_clear */
#define GPIP_GPADC_FIFO_OVERRUN_CLR                             (1<<1U)
#define GPIP_GPADC_FIFO_UNDERRUN_CLR                            (1<<2U)
#define GPIP_GPADC_IJCT_FIFO0_OVERRUN_CLR                       (1<<4U)
#define GPIP_GPADC_IJCT_FIFO0_UNDERRUN_CLR                      (1<<5U)
#define GPIP_GPADC_IJCT_FIFO1_OVERRUN_CLR                       (1<<7U)
#define GPIP_GPADC_IJCT_FIFO1_UNDERRUN_CLR                      (1<<8U)
#define GPIP_GPADC_IJCT_FIFO2_OVERRUN_CLR                       (1<<10U)
#define GPIP_GPADC_IJCT_FIFO2_UNDERRUN_CLR                      (1<<11U)
#define GPIP_GPADC_IJCT_FIFO3_OVERRUN_CLR                       (1<<13U)
#define GPIP_GPADC_IJCT_FIFO3_UNDERRUN_CLR                      (1<<14U)
#define GPIP_GPADC1_SCAN_DONE_CLR                               (1<<15U)
#define GPIP_GPADC1_IJCT_DONE_CLR                               (1<<16U)
#define GPIP_GPADC2_SCAN_DONE_CLR                               (1<<17U)
#define GPIP_GPADC2_IJCT_DONE_CLR                               (1<<18U)
#define GPIP_GPADC_WD_INT_CLR                                   (1<<19U)

/* 0x10 : gpadc_regular_fifo */
#define GPIP_GPADC_RGLR_FIFO_RDATA_SHIFT                        (0U)
#define GPIP_GPADC_RGLR_FIFO_RDATA_MASK                         (0x3fffffff<<GPIP_GPADC_RGLR_FIFO_RDATA_SHIFT)

/* 0x14 : gpadc_injection_fifo0 */
#define GPIP_GPADC_IJCT_FIFO0_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO0_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO0_RDATA_SHIFT)

/* 0x18 : gpadc_injection_fifo1 */
#define GPIP_GPADC_IJCT_FIFO1_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO1_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO1_RDATA_SHIFT)

/* 0x1C : gpadc_injection_fifo2 */
#define GPIP_GPADC_IJCT_FIFO2_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO2_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO2_RDATA_SHIFT)

/* 0x20 : gpadc_injection_fifo3 */
#define GPIP_GPADC_IJCT_FIFO3_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO3_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO3_RDATA_SHIFT)

/* 0x24 : gpadc_pre_proc */
#define GPIP_GPADC_RGLR_DATA_SHIFT_SHIFT                        (0U)
#define GPIP_GPADC_RGLR_DATA_SHIFT_MASK                         (0xf<<GPIP_GPADC_RGLR_DATA_SHIFT_SHIFT)
#define GPIP_GPADC_IJCT_DATA_SHIFT_SHIFT                        (4U)
#define GPIP_GPADC_IJCT_DATA_SHIFT_MASK                         (0xf<<GPIP_GPADC_IJCT_DATA_SHIFT_SHIFT)

/* 0x28 : gpadc_wd_ctrl_0 */
#define GPIP_GPADC_WD_EN                                        (1<<0U)
#define GPIP_GPADC_WD_CH_SEL_SHIFT                              (1U)
#define GPIP_GPADC_WD_CH_SEL_MASK                               (0x3f<<GPIP_GPADC_WD_CH_SEL_SHIFT)
#define GPIP_GPADC_WD_CNTR_TH_SHIFT                             (8U)
#define GPIP_GPADC_WD_CNTR_TH_MASK                              (0x1f<<GPIP_GPADC_WD_CNTR_TH_SHIFT)
#define GPIP_GPADC_WD_CNTR_SHIFT                                (16U)
#define GPIP_GPADC_WD_CNTR_MASK                                 (0x1f<<GPIP_GPADC_WD_CNTR_SHIFT)

/* 0x2C : gpadc_wd_ctrl_1 */
#define GPIP_GPADC_WD_LOWER_TH_SHIFT                            (0U)
#define GPIP_GPADC_WD_LOWER_TH_MASK                             (0xffff<<GPIP_GPADC_WD_LOWER_TH_SHIFT)
#define GPIP_GPADC_WD_UPPER_TH_SHIFT                            (16U)
#define GPIP_GPADC_WD_UPPER_TH_MASK                             (0xffff<<GPIP_GPADC_WD_UPPER_TH_SHIFT)

/* 0x30 : gpadc2_fifo_enable */
#define GPIP_GPADC2_FIFO_EN                                     (1<<31U)

/* 0x34 : gpadc_fifo2_config */
#define GPIP_GPADC_FIFO2_DMA_EN                                 (1<<0U)
#define GPIP_GPADC_FIFO2_CLR                                    (1<<1U)
#define GPIP_GPADC_FIFO2_THL_SHIFT                              (2U)
#define GPIP_GPADC_FIFO2_THL_MASK                               (0x3<<GPIP_GPADC_FIFO2_THL_SHIFT)
#define GPIP_GPADC_FIFO2_DATA_COUNT_SHIFT                       (4U)
#define GPIP_GPADC_FIFO2_DATA_COUNT_MASK                        (0x3f<<GPIP_GPADC_FIFO2_DATA_COUNT_SHIFT)
#define GPIP_GPADC_FIFO2_NE                                     (1<<10U)
#define GPIP_GPADC_FIFO2_FULL                                   (1<<11U)
#define GPIP_GPADC_IJCT_FIFO4_CLR                               (1<<12U)
#define GPIP_GPADC_IJCT_FIFO5_CLR                               (1<<13U)
#define GPIP_GPADC_IJCT_FIFO6_CLR                               (1<<14U)
#define GPIP_GPADC_IJCT_FIFO7_CLR                               (1<<15U)
#define GPIP_GPADC_IJCT_FIFO4_DATA_COUNT_SHIFT                  (16U)
#define GPIP_GPADC_IJCT_FIFO4_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO4_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO4_NE                                (1<<18U)
#define GPIP_GPADC_IJCT_FIFO4_FULL                              (1<<19U)
#define GPIP_GPADC_IJCT_FIFO5_DATA_COUNT_SHIFT                  (20U)
#define GPIP_GPADC_IJCT_FIFO5_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO5_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO5_NE                                (1<<22U)
#define GPIP_GPADC_IJCT_FIFO5_FULL                              (1<<23U)
#define GPIP_GPADC_IJCT_FIFO6_DATA_COUNT_SHIFT                  (24U)
#define GPIP_GPADC_IJCT_FIFO6_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO6_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO6_NE                                (1<<26U)
#define GPIP_GPADC_IJCT_FIFO6_FULL                              (1<<27U)
#define GPIP_GPADC_IJCT_FIFO7_DATA_COUNT_SHIFT                  (28U)
#define GPIP_GPADC_IJCT_FIFO7_DATA_COUNT_MASK                   (0x3<<GPIP_GPADC_IJCT_FIFO7_DATA_COUNT_SHIFT)
#define GPIP_GPADC_IJCT_FIFO7_NE                                (1<<30U)
#define GPIP_GPADC_IJCT_FIFO7_FULL                              (1<<31U)

/* 0x38 : gpadc2_dma_int_sts */
#define GPIP_GPADC_FIFO2_RDY                                    (1<<0U)
#define GPIP_GPADC_FIFO2_OVERRUN                                (1<<1U)
#define GPIP_GPADC_FIFO2_UNDERRUN                               (1<<2U)
#define GPIP_GPADC_IJCT_FIFO4_RDY                               (1<<3U)
#define GPIP_GPADC_IJCT_FIFO4_OVERRUN                           (1<<4U)
#define GPIP_GPADC_IJCT_FIFO4_UNDERRUN                          (1<<5U)
#define GPIP_GPADC_IJCT_FIFO5_RDY                               (1<<6U)
#define GPIP_GPADC_IJCT_FIFO5_OVERRUN                           (1<<7U)
#define GPIP_GPADC_IJCT_FIFO5_UNDERRUN                          (1<<8U)
#define GPIP_GPADC_IJCT_FIFO6_RDY                               (1<<9U)
#define GPIP_GPADC_IJCT_FIFO6_OVERRUN                           (1<<10U)
#define GPIP_GPADC_IJCT_FIFO6_UNDERRUN                          (1<<11U)
#define GPIP_GPADC_IJCT_FIFO7_RDY                               (1<<12U)
#define GPIP_GPADC_IJCT_FIFO7_OVERRUN                           (1<<13U)
#define GPIP_GPADC_IJCT_FIFO7_UNDERRUN                          (1<<14U)

/* 0x3C : gpadc2_dma_int_mask */
#define GPIP_GPADC_FIFO2_RDY_MASK                               (1<<0U)
#define GPIP_GPADC_FIFO2_OVERRUN_MASK                           (1<<1U)
#define GPIP_GPADC_FIFO2_UNDERRUN_MASK                          (1<<2U)
#define GPIP_GPADC_IJCT_FIFO4_RDY_MASK                          (1<<3U)
#define GPIP_GPADC_IJCT_FIFO4_OVERRUN_MASK                      (1<<4U)
#define GPIP_GPADC_IJCT_FIFO4_UNDERRUN_MASK                     (1<<5U)
#define GPIP_GPADC_IJCT_FIFO5_RDY_MASK                          (1<<6U)
#define GPIP_GPADC_IJCT_FIFO5_OVERRUN_MASK                      (1<<7U)
#define GPIP_GPADC_IJCT_FIFO5_UNDERRUN_MASK                     (1<<8U)
#define GPIP_GPADC_IJCT_FIFO6_RDY_MASK                          (1<<9U)
#define GPIP_GPADC_IJCT_FIFO6_OVERRUN_MASK                      (1<<10U)
#define GPIP_GPADC_IJCT_FIFO6_UNDERRUN_MASK                     (1<<11U)
#define GPIP_GPADC_IJCT_FIFO7_RDY_MASK                          (1<<12U)
#define GPIP_GPADC_IJCT_FIFO7_OVERRUN_MASK                      (1<<13U)
#define GPIP_GPADC_IJCT_FIFO7_UNDERRUN_MASK                     (1<<14U)

/* 0x40 : gpadc2_dma_int_clear */
#define GPIP_GPADC_FIFO2_OVERRUN_CLR                            (1<<1U)
#define GPIP_GPADC_FIFO2_UNDERRUN_CLR                           (1<<2U)
#define GPIP_GPADC_IJCT_FIFO4_OVERRUN_CLR                       (1<<4U)
#define GPIP_GPADC_IJCT_FIFO4_UNDERRUN_CLR                      (1<<5U)
#define GPIP_GPADC_IJCT_FIFO5_OVERRUN_CLR                       (1<<7U)
#define GPIP_GPADC_IJCT_FIFO5_UNDERRUN_CLR                      (1<<8U)
#define GPIP_GPADC_IJCT_FIFO6_OVERRUN_CLR                       (1<<10U)
#define GPIP_GPADC_IJCT_FIFO6_UNDERRUN_CLR                      (1<<11U)
#define GPIP_GPADC_IJCT_FIFO7_OVERRUN_CLR                       (1<<13U)
#define GPIP_GPADC_IJCT_FIFO7_UNDERRUN_CLR                      (1<<14U)

/* 0x44 : gpadc_regular_fifo2 */
#define GPIP_GPADC_RGLR_FIFO2_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_RGLR_FIFO2_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_RGLR_FIFO2_RDATA_SHIFT)

/* 0x48 : gpadc_injection_fifo4 */
#define GPIP_GPADC_IJCT_FIFO4_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO4_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO4_RDATA_SHIFT)

/* 0x4C : gpadc_injection_fifo5 */
#define GPIP_GPADC_IJCT_FIFO5_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO5_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO5_RDATA_SHIFT)

/* 0x50 : gpadc_injection_fifo6 */
#define GPIP_GPADC_IJCT_FIFO6_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO6_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO6_RDATA_SHIFT)

/* 0x54 : gpadc_injection_fifo7 */
#define GPIP_GPADC_IJCT_FIFO7_RDATA_SHIFT                       (0U)
#define GPIP_GPADC_IJCT_FIFO7_RDATA_MASK                        (0x3fffffff<<GPIP_GPADC_IJCT_FIFO7_RDATA_SHIFT)

/* 0x90C : gpadc_reg_cmd1 */
#define AON_GPADC_TS_EN                                         (1<<0U)
#define AON_GPADC_TSVBE_LOW                                     (1<<1U)
#define AON_GPADC_VBAT_EN                                       (1<<2U)
#define AON_GPADC_BIAS_SEL                                      (1<<3U)
#define AON_GPADC_INT_NEG_SEL_SHIFT                             (4U)
#define AON_GPADC_INT_NEG_SEL_MASK                              (0x1f<<AON_GPADC_INT_NEG_SEL_SHIFT)
#define AON_GPADC_INT_POS_SEL_SHIFT                             (12U)
#define AON_GPADC_INT_POS_SEL_MASK                              (0x1f<<AON_GPADC_INT_POS_SEL_SHIFT)
#define AON_GPADC_CHIP_SEN_PU                                   (1<<20U)
#define AON_GPADC_CHIP_SEN_SEL_SHIFT                            (21U)
#define AON_GPADC_CHIP_SEN_SEL_MASK                             (0xf<<AON_GPADC_CHIP_SEN_SEL_SHIFT)
#define AON_GPADC_DVDD_SEL_SHIFT                                (25U)
#define AON_GPADC_DVDD_SEL_MASK                                 (0x3<<AON_GPADC_DVDD_SEL_SHIFT)
#define AON_GPADC_TSEXT_SEL                                     (1<<27U)
#define AON_GPADC_VREF_EX_CH_SEL_SHIFT                          (28U)
#define AON_GPADC_VREF_EX_CH_SEL_MASK                           (0xf<<AON_GPADC_VREF_EX_CH_SEL_SHIFT)

/* 0x910 : gpadc_reg_cmd2 */
#define AON_GPADC_MODE_SHIFT                                    (0U)
#define AON_GPADC_MODE_MASK                                     (0x3<<AON_GPADC_MODE_SHIFT)
#define AON_GPADC_CLK_ANA_DLY_SHIFT                             (8U)
#define AON_GPADC_CLK_ANA_DLY_MASK                              (0xf<<AON_GPADC_CLK_ANA_DLY_SHIFT)
#define AON_GPADC_CLK_ANA_DLY_EN                                (1<<12U)
#define AON_GPADC_CLK_ANA_INV                                   (1<<13U)
#define AON_GPADC_CLK_DIV_RATIO_SHIFT                           (16U)
#define AON_GPADC_CLK_DIV_RATIO_MASK                            (0x7<<AON_GPADC_CLK_DIV_RATIO_SHIFT)

/* 0x914 : gpadc1_reg_config1 */
#define AON_GPADC1_VREF_EX_SEL                                  (1<<0U)
#define AON_GPADC1_DIFF_MODE                                    (1<<1U)
#define AON_GPADC1_NEG_GND                                      (1<<2U)
#define AON_GPADC1_PGA_VCMI_EN                                  (1<<3U)
#define AON_GPADC1_VREF_TRIM_SHIFT                              (4U)
#define AON_GPADC1_VREF_TRIM_MASK                               (0x3f<<AON_GPADC1_VREF_TRIM_SHIFT)
#define AON_GPADC1_CHOP_MODE_SHIFT                              (12U)
#define AON_GPADC1_CHOP_MODE_MASK                               (0x3<<AON_GPADC1_CHOP_MODE_SHIFT)
#define AON_GPADC1_PGA_EN                                       (1<<16U)
#define AON_GPADC1_PGA_GAIN_SHIFT                               (20U)
#define AON_GPADC1_PGA_GAIN_MASK                                (0x7<<AON_GPADC1_PGA_GAIN_SHIFT)
#define AON_GPADC1_PGA_VCM_SHIFT                                (24U)
#define AON_GPADC1_PGA_VCM_MASK                                 (0x3<<AON_GPADC1_PGA_VCM_SHIFT)

/* 0x918 : gpadc1_reg_config2 */
#define AON_GPADC1_GLOBAL_EN                                    (1<<0U)
#define AON_GPADC1_CONV_START                                   (1<<1U)
#define AON_GPADC1_SOFT_RST                                     (1<<2U)
#define AON_GPADC1_DWA_EN                                       (1<<4U)
#define AON_GPADC1_DITHER_EN                                    (1<<8U)
#define AON_GPADC1_DLY_SEL_SHIFT                                (12U)
#define AON_GPADC1_DLY_SEL_MASK                                 (0x7<<AON_GPADC1_DLY_SEL_SHIFT)
#define AON_GPADC1_EXT_POS_SEL_SHIFT                            (16U)
#define AON_GPADC1_EXT_POS_SEL_MASK                             (0xf<<AON_GPADC1_EXT_POS_SEL_SHIFT)
#define AON_GPADC1_EXT_NEG_SEL_SHIFT                            (20U)
#define AON_GPADC1_EXT_NEG_SEL_MASK                             (0xf<<AON_GPADC1_EXT_NEG_SEL_SHIFT)

/* 0x91C : gpadc1_reg_ctrl_0 */
#define AON_GPADC1_CONT_CONV_EN                                 (1<<0U)
#define AON_GPADC1_RES_SEL_SHIFT                                (1U)
#define AON_GPADC1_RES_SEL_MASK                                 (0x7<<AON_GPADC1_RES_SEL_SHIFT)
#define AON_GPADC1_IJCT_START                                   (1<<8U)
#define AON_GPADC1_IJCT_LENGTH_SHIFT                            (9U)
#define AON_GPADC1_IJCT_LENGTH_MASK                             (0x3<<AON_GPADC1_IJCT_LENGTH_SHIFT)
#define AON_GPADC1_PWM_TRG_EN                                   (1<<12U)
#define AON_GPADC1_SCAN_LENGTH_SHIFT                            (16U)
#define AON_GPADC1_SCAN_LENGTH_MASK                             (0xf<<AON_GPADC1_SCAN_LENGTH_SHIFT)
#define AON_GPADC1_SCAN_EN                                      (1<<20U)
#define AON_GPADC1_CONV_DELAY_SHIFT                             (24U)
#define AON_GPADC1_CONV_DELAY_MASK                              (0xff<<AON_GPADC1_CONV_DELAY_SHIFT)

/* 0x920 : gpadc1_reg_os_cal_data */
#define AON_GPADC1_OS_CAL_DATA_SHIFT                            (0U)
#define AON_GPADC1_OS_CAL_DATA_MASK                             (0xffff<<AON_GPADC1_OS_CAL_DATA_SHIFT)

/* 0x924 : gpadc1_reg_scan_0 */
#define AON_GPADC1_SCAN_POS_0_SHIFT                             (0U)
#define AON_GPADC1_SCAN_POS_0_MASK                              (0xf<<AON_GPADC1_SCAN_POS_0_SHIFT)
#define AON_GPADC1_SCAN_POS_1_SHIFT                             (4U)
#define AON_GPADC1_SCAN_POS_1_MASK                              (0xf<<AON_GPADC1_SCAN_POS_1_SHIFT)
#define AON_GPADC1_SCAN_POS_2_SHIFT                             (8U)
#define AON_GPADC1_SCAN_POS_2_MASK                              (0xf<<AON_GPADC1_SCAN_POS_2_SHIFT)
#define AON_GPADC1_SCAN_POS_3_SHIFT                             (12U)
#define AON_GPADC1_SCAN_POS_3_MASK                              (0xf<<AON_GPADC1_SCAN_POS_3_SHIFT)
#define AON_GPADC1_SCAN_POS_4_SHIFT                             (16U)
#define AON_GPADC1_SCAN_POS_4_MASK                              (0xf<<AON_GPADC1_SCAN_POS_4_SHIFT)
#define AON_GPADC1_SCAN_POS_5_SHIFT                             (20U)
#define AON_GPADC1_SCAN_POS_5_MASK                              (0xf<<AON_GPADC1_SCAN_POS_5_SHIFT)
#define AON_GPADC1_SCAN_POS_6_SHIFT                             (24U)
#define AON_GPADC1_SCAN_POS_6_MASK                              (0xf<<AON_GPADC1_SCAN_POS_6_SHIFT)
#define AON_GPADC1_SCAN_POS_7_SHIFT                             (28U)
#define AON_GPADC1_SCAN_POS_7_MASK                              (0xf<<AON_GPADC1_SCAN_POS_7_SHIFT)

/* 0x928 : gpadc1_reg_scan_1 */
#define AON_GPADC1_SCAN_POS_8_SHIFT                             (0U)
#define AON_GPADC1_SCAN_POS_8_MASK                              (0xf<<AON_GPADC1_SCAN_POS_8_SHIFT)
#define AON_GPADC1_SCAN_POS_9_SHIFT                             (4U)
#define AON_GPADC1_SCAN_POS_9_MASK                              (0xf<<AON_GPADC1_SCAN_POS_9_SHIFT)
#define AON_GPADC1_SCAN_POS_10_SHIFT                            (8U)
#define AON_GPADC1_SCAN_POS_10_MASK                             (0xf<<AON_GPADC1_SCAN_POS_10_SHIFT)
#define AON_GPADC1_SCAN_POS_11_SHIFT                            (12U)
#define AON_GPADC1_SCAN_POS_11_MASK                             (0xf<<AON_GPADC1_SCAN_POS_11_SHIFT)
#define AON_GPADC1_SCAN_POS_12_SHIFT                            (16U)
#define AON_GPADC1_SCAN_POS_12_MASK                             (0xf<<AON_GPADC1_SCAN_POS_12_SHIFT)
#define AON_GPADC1_SCAN_POS_13_SHIFT                            (20U)
#define AON_GPADC1_SCAN_POS_13_MASK                             (0xf<<AON_GPADC1_SCAN_POS_13_SHIFT)
#define AON_GPADC1_SCAN_POS_14_SHIFT                            (24U)
#define AON_GPADC1_SCAN_POS_14_MASK                             (0xf<<AON_GPADC1_SCAN_POS_14_SHIFT)
#define AON_GPADC1_SCAN_POS_15_SHIFT                            (28U)
#define AON_GPADC1_SCAN_POS_15_MASK                             (0xf<<AON_GPADC1_SCAN_POS_15_SHIFT)

/* 0x92C : gpadc1_reg_scan_2 */
#define AON_GPADC1_SCAN_NEG_0_SHIFT                             (0U)
#define AON_GPADC1_SCAN_NEG_0_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_0_SHIFT)
#define AON_GPADC1_SCAN_NEG_1_SHIFT                             (4U)
#define AON_GPADC1_SCAN_NEG_1_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_1_SHIFT)
#define AON_GPADC1_SCAN_NEG_2_SHIFT                             (8U)
#define AON_GPADC1_SCAN_NEG_2_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_2_SHIFT)
#define AON_GPADC1_SCAN_NEG_3_SHIFT                             (12U)
#define AON_GPADC1_SCAN_NEG_3_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_3_SHIFT)
#define AON_GPADC1_SCAN_NEG_4_SHIFT                             (16U)
#define AON_GPADC1_SCAN_NEG_4_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_4_SHIFT)
#define AON_GPADC1_SCAN_NEG_5_SHIFT                             (20U)
#define AON_GPADC1_SCAN_NEG_5_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_5_SHIFT)
#define AON_GPADC1_SCAN_NEG_6_SHIFT                             (24U)
#define AON_GPADC1_SCAN_NEG_6_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_6_SHIFT)
#define AON_GPADC1_SCAN_NEG_7_SHIFT                             (28U)
#define AON_GPADC1_SCAN_NEG_7_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_7_SHIFT)

/* 0x930 : gpadc1_reg_scan_3 */
#define AON_GPADC1_SCAN_NEG_8_SHIFT                             (0U)
#define AON_GPADC1_SCAN_NEG_8_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_8_SHIFT)
#define AON_GPADC1_SCAN_NEG_9_SHIFT                             (4U)
#define AON_GPADC1_SCAN_NEG_9_MASK                              (0xf<<AON_GPADC1_SCAN_NEG_9_SHIFT)
#define AON_GPADC1_SCAN_NEG_10_SHIFT                            (8U)
#define AON_GPADC1_SCAN_NEG_10_MASK                             (0xf<<AON_GPADC1_SCAN_NEG_10_SHIFT)
#define AON_GPADC1_SCAN_NEG_11_SHIFT                            (12U)
#define AON_GPADC1_SCAN_NEG_11_MASK                             (0xf<<AON_GPADC1_SCAN_NEG_11_SHIFT)
#define AON_GPADC1_SCAN_NEG_12_SHIFT                            (16U)
#define AON_GPADC1_SCAN_NEG_12_MASK                             (0xf<<AON_GPADC1_SCAN_NEG_12_SHIFT)
#define AON_GPADC1_SCAN_NEG_13_SHIFT                            (20U)
#define AON_GPADC1_SCAN_NEG_13_MASK                             (0xf<<AON_GPADC1_SCAN_NEG_13_SHIFT)
#define AON_GPADC1_SCAN_NEG_14_SHIFT                            (24U)
#define AON_GPADC1_SCAN_NEG_14_MASK                             (0xf<<AON_GPADC1_SCAN_NEG_14_SHIFT)
#define AON_GPADC1_SCAN_NEG_15_SHIFT                            (28U)
#define AON_GPADC1_SCAN_NEG_15_MASK                             (0xf<<AON_GPADC1_SCAN_NEG_15_SHIFT)

/* 0x934 : gpadc1_reg_ijct_ext_0 */
#define AON_GPADC1_IJCT_EXT_POS_0_SHIFT                         (0U)
#define AON_GPADC1_IJCT_EXT_POS_0_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_POS_0_SHIFT)
#define AON_GPADC1_IJCT_EXT_POS_1_SHIFT                         (4U)
#define AON_GPADC1_IJCT_EXT_POS_1_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_POS_1_SHIFT)
#define AON_GPADC1_IJCT_EXT_POS_2_SHIFT                         (8U)
#define AON_GPADC1_IJCT_EXT_POS_2_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_POS_2_SHIFT)
#define AON_GPADC1_IJCT_EXT_POS_3_SHIFT                         (12U)
#define AON_GPADC1_IJCT_EXT_POS_3_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_POS_3_SHIFT)
#define AON_GPADC1_IJCT_EXT_NEG_0_SHIFT                         (16U)
#define AON_GPADC1_IJCT_EXT_NEG_0_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_NEG_0_SHIFT)
#define AON_GPADC1_IJCT_EXT_NEG_1_SHIFT                         (20U)
#define AON_GPADC1_IJCT_EXT_NEG_1_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_NEG_1_SHIFT)
#define AON_GPADC1_IJCT_EXT_NEG_2_SHIFT                         (24U)
#define AON_GPADC1_IJCT_EXT_NEG_2_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_NEG_2_SHIFT)
#define AON_GPADC1_IJCT_EXT_NEG_3_SHIFT                         (28U)
#define AON_GPADC1_IJCT_EXT_NEG_3_MASK                          (0xf<<AON_GPADC1_IJCT_EXT_NEG_3_SHIFT)

/* 0x938 : gpadc1_reg_ijct_int_0 */
#define AON_GPADC1_IJCT_INT_POS_0_SHIFT                         (0U)
#define AON_GPADC1_IJCT_INT_POS_0_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_POS_0_SHIFT)
#define AON_GPADC1_IJCT_INT_POS_1_SHIFT                         (8U)
#define AON_GPADC1_IJCT_INT_POS_1_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_POS_1_SHIFT)
#define AON_GPADC1_IJCT_INT_POS_2_SHIFT                         (16U)
#define AON_GPADC1_IJCT_INT_POS_2_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_POS_2_SHIFT)
#define AON_GPADC1_IJCT_INT_POS_3_SHIFT                         (24U)
#define AON_GPADC1_IJCT_INT_POS_3_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_POS_3_SHIFT)

/* 0x93C : gpadc1_reg_ijct_int_1 */
#define AON_GPADC1_IJCT_INT_NEG_0_SHIFT                         (0U)
#define AON_GPADC1_IJCT_INT_NEG_0_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_NEG_0_SHIFT)
#define AON_GPADC1_IJCT_INT_NEG_1_SHIFT                         (8U)
#define AON_GPADC1_IJCT_INT_NEG_1_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_NEG_1_SHIFT)
#define AON_GPADC1_IJCT_INT_NEG_2_SHIFT                         (16U)
#define AON_GPADC1_IJCT_INT_NEG_2_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_NEG_2_SHIFT)
#define AON_GPADC1_IJCT_INT_NEG_3_SHIFT                         (24U)
#define AON_GPADC1_IJCT_INT_NEG_3_MASK                          (0x1f<<AON_GPADC1_IJCT_INT_NEG_3_SHIFT)

/* 0x940 : gpadc1_reg_result */
#define AON_GPADC1_DATA_OUT_SHIFT                               (0U)
#define AON_GPADC1_DATA_OUT_MASK                                (0x1fffffff<<AON_GPADC1_DATA_OUT_SHIFT)
#define AON_GPADC1_DATA_RDY                                     (1<<31U)

/* 0x944 : gpadc1_reg_raw_data */
#define AON_GPADC1_RAW_DATA_SHIFT                               (0U)
#define AON_GPADC1_RAW_DATA_MASK                                (0xfff<<AON_GPADC1_RAW_DATA_SHIFT)
#define AON_GPADC1_RESERVED_SHIFT                               (16U)
#define AON_GPADC1_RESERVED_MASK                                (0xffff<<AON_GPADC1_RESERVED_SHIFT)

/* 0x948 : gpadc1_reg_satur */
#define AON_GPADC1_NEG_SATUR                                    (1<<0U)
#define AON_GPADC1_POS_SATUR                                    (1<<1U)
#define AON_GPADC1_NEG_SATUR_CLR                                (1<<4U)
#define AON_GPADC1_POS_SATUR_CLR                                (1<<5U)
#define AON_GPADC1_NEG_SATUR_MASK                               (1<<8U)
#define AON_GPADC1_POS_SATUR_MASK                               (1<<9U)

/* 0x94C : gpadc2_reg_config1 */
#define AON_GPADC2_VREF_EX_SEL                                  (1<<0U)
#define AON_GPADC2_DIFF_MODE                                    (1<<1U)
#define AON_GPADC2_NEG_GND                                      (1<<2U)
#define AON_GPADC2_PGA_VCMI_EN                                  (1<<3U)
#define AON_GPADC2_VREF_TRIM_SHIFT                              (4U)
#define AON_GPADC2_VREF_TRIM_MASK                               (0x3f<<AON_GPADC2_VREF_TRIM_SHIFT)
#define AON_GPADC2_CHOP_MODE_SHIFT                              (12U)
#define AON_GPADC2_CHOP_MODE_MASK                               (0x3<<AON_GPADC2_CHOP_MODE_SHIFT)
#define AON_GPADC2_PGA_EN                                       (1<<16U)
#define AON_GPADC2_PGA_GAIN_SHIFT                               (20U)
#define AON_GPADC2_PGA_GAIN_MASK                                (0x7<<AON_GPADC2_PGA_GAIN_SHIFT)
#define AON_GPADC2_PGA_VCM_SHIFT                                (24U)
#define AON_GPADC2_PGA_VCM_MASK                                 (0x3<<AON_GPADC2_PGA_VCM_SHIFT)

/* 0x950 : gpadc2_reg_config2 */
#define AON_GPADC2_GLOBAL_EN                                    (1<<0U)
#define AON_GPADC2_CONV_START                                   (1<<1U)
#define AON_GPADC2_SOFT_RST                                     (1<<2U)
#define AON_GPADC2_DWA_EN                                       (1<<4U)
#define AON_GPADC2_DITHER_EN                                    (1<<8U)
#define AON_GPADC2_DLY_SEL_SHIFT                                (12U)
#define AON_GPADC2_DLY_SEL_MASK                                 (0x7<<AON_GPADC2_DLY_SEL_SHIFT)
#define AON_GPADC2_EXT_POS_SEL_SHIFT                            (16U)
#define AON_GPADC2_EXT_POS_SEL_MASK                             (0xf<<AON_GPADC2_EXT_POS_SEL_SHIFT)
#define AON_GPADC2_EXT_NEG_SEL_SHIFT                            (20U)
#define AON_GPADC2_EXT_NEG_SEL_MASK                             (0xf<<AON_GPADC2_EXT_NEG_SEL_SHIFT)

/* 0x954 : gpadc2_reg_ctrl_0 */
#define AON_GPADC2_CONT_CONV_EN                                 (1<<0U)
#define AON_GPADC2_RES_SEL_SHIFT                                (1U)
#define AON_GPADC2_RES_SEL_MASK                                 (0x7<<AON_GPADC2_RES_SEL_SHIFT)
#define AON_GPADC2_IJCT_START                                   (1<<8U)
#define AON_GPADC2_IJCT_LENGTH_SHIFT                            (9U)
#define AON_GPADC2_IJCT_LENGTH_MASK                             (0x3<<AON_GPADC2_IJCT_LENGTH_SHIFT)
#define AON_GPADC2_PWM_TRG_EN                                   (1<<12U)
#define AON_GPADC2_SCAN_LENGTH_SHIFT                            (16U)
#define AON_GPADC2_SCAN_LENGTH_MASK                             (0xf<<AON_GPADC2_SCAN_LENGTH_SHIFT)
#define AON_GPADC2_SCAN_EN                                      (1<<20U)
#define AON_GPADC2_CONV_DELAY_SHIFT                             (24U)
#define AON_GPADC2_CONV_DELAY_MASK                              (0xff<<AON_GPADC2_CONV_DELAY_SHIFT)

/* 0x958 : gpadc2_reg_os_cal_data */
#define AON_GPADC2_OS_CAL_DATA_SHIFT                            (0U)
#define AON_GPADC2_OS_CAL_DATA_MASK                             (0xffff<<AON_GPADC2_OS_CAL_DATA_SHIFT)

/* 0x95C : gpadc2_reg_scan_0 */
#define AON_GPADC2_SCAN_POS_0_SHIFT                             (0U)
#define AON_GPADC2_SCAN_POS_0_MASK                              (0xf<<AON_GPADC2_SCAN_POS_0_SHIFT)
#define AON_GPADC2_SCAN_POS_1_SHIFT                             (4U)
#define AON_GPADC2_SCAN_POS_1_MASK                              (0xf<<AON_GPADC2_SCAN_POS_1_SHIFT)
#define AON_GPADC2_SCAN_POS_2_SHIFT                             (8U)
#define AON_GPADC2_SCAN_POS_2_MASK                              (0xf<<AON_GPADC2_SCAN_POS_2_SHIFT)
#define AON_GPADC2_SCAN_POS_3_SHIFT                             (12U)
#define AON_GPADC2_SCAN_POS_3_MASK                              (0xf<<AON_GPADC2_SCAN_POS_3_SHIFT)
#define AON_GPADC2_SCAN_POS_4_SHIFT                             (16U)
#define AON_GPADC2_SCAN_POS_4_MASK                              (0xf<<AON_GPADC2_SCAN_POS_4_SHIFT)
#define AON_GPADC2_SCAN_POS_5_SHIFT                             (20U)
#define AON_GPADC2_SCAN_POS_5_MASK                              (0xf<<AON_GPADC2_SCAN_POS_5_SHIFT)
#define AON_GPADC2_SCAN_POS_6_SHIFT                             (24U)
#define AON_GPADC2_SCAN_POS_6_MASK                              (0xf<<AON_GPADC2_SCAN_POS_6_SHIFT)
#define AON_GPADC2_SCAN_POS_7_SHIFT                             (28U)
#define AON_GPADC2_SCAN_POS_7_MASK                              (0xf<<AON_GPADC2_SCAN_POS_7_SHIFT)

/* 0x960 : gpadc2_reg_scan_1 */
#define AON_GPADC2_SCAN_POS_8_SHIFT                             (0U)
#define AON_GPADC2_SCAN_POS_8_MASK                              (0xf<<AON_GPADC2_SCAN_POS_8_SHIFT)
#define AON_GPADC2_SCAN_POS_9_SHIFT                             (4U)
#define AON_GPADC2_SCAN_POS_9_MASK                              (0xf<<AON_GPADC2_SCAN_POS_9_SHIFT)
#define AON_GPADC2_SCAN_POS_10_SHIFT                            (8U)
#define AON_GPADC2_SCAN_POS_10_MASK                             (0xf<<AON_GPADC2_SCAN_POS_10_SHIFT)
#define AON_GPADC2_SCAN_POS_11_SHIFT                            (12U)
#define AON_GPADC2_SCAN_POS_11_MASK                             (0xf<<AON_GPADC2_SCAN_POS_11_SHIFT)
#define AON_GPADC2_SCAN_POS_12_SHIFT                            (16U)
#define AON_GPADC2_SCAN_POS_12_MASK                             (0xf<<AON_GPADC2_SCAN_POS_12_SHIFT)
#define AON_GPADC2_SCAN_POS_13_SHIFT                            (20U)
#define AON_GPADC2_SCAN_POS_13_MASK                             (0xf<<AON_GPADC2_SCAN_POS_13_SHIFT)
#define AON_GPADC2_SCAN_POS_14_SHIFT                            (24U)
#define AON_GPADC2_SCAN_POS_14_MASK                             (0xf<<AON_GPADC2_SCAN_POS_14_SHIFT)
#define AON_GPADC2_SCAN_POS_15_SHIFT                            (28U)
#define AON_GPADC2_SCAN_POS_15_MASK                             (0xf<<AON_GPADC2_SCAN_POS_15_SHIFT)

/* 0x964 : gpadc2_reg_scan_2 */
#define AON_GPADC2_SCAN_NEG_0_SHIFT                             (0U)
#define AON_GPADC2_SCAN_NEG_0_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_0_SHIFT)
#define AON_GPADC2_SCAN_NEG_1_SHIFT                             (4U)
#define AON_GPADC2_SCAN_NEG_1_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_1_SHIFT)
#define AON_GPADC2_SCAN_NEG_2_SHIFT                             (8U)
#define AON_GPADC2_SCAN_NEG_2_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_2_SHIFT)
#define AON_GPADC2_SCAN_NEG_3_SHIFT                             (12U)
#define AON_GPADC2_SCAN_NEG_3_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_3_SHIFT)
#define AON_GPADC2_SCAN_NEG_4_SHIFT                             (16U)
#define AON_GPADC2_SCAN_NEG_4_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_4_SHIFT)
#define AON_GPADC2_SCAN_NEG_5_SHIFT                             (20U)
#define AON_GPADC2_SCAN_NEG_5_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_5_SHIFT)
#define AON_GPADC2_SCAN_NEG_6_SHIFT                             (24U)
#define AON_GPADC2_SCAN_NEG_6_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_6_SHIFT)
#define AON_GPADC2_SCAN_NEG_7_SHIFT                             (28U)
#define AON_GPADC2_SCAN_NEG_7_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_7_SHIFT)

/* 0x968 : gpadc2_reg_scan_3 */
#define AON_GPADC2_SCAN_NEG_8_SHIFT                             (0U)
#define AON_GPADC2_SCAN_NEG_8_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_8_SHIFT)
#define AON_GPADC2_SCAN_NEG_9_SHIFT                             (4U)
#define AON_GPADC2_SCAN_NEG_9_MASK                              (0xf<<AON_GPADC2_SCAN_NEG_9_SHIFT)
#define AON_GPADC2_SCAN_NEG_10_SHIFT                            (8U)
#define AON_GPADC2_SCAN_NEG_10_MASK                             (0xf<<AON_GPADC2_SCAN_NEG_10_SHIFT)
#define AON_GPADC2_SCAN_NEG_11_SHIFT                            (12U)
#define AON_GPADC2_SCAN_NEG_11_MASK                             (0xf<<AON_GPADC2_SCAN_NEG_11_SHIFT)
#define AON_GPADC2_SCAN_NEG_12_SHIFT                            (16U)
#define AON_GPADC2_SCAN_NEG_12_MASK                             (0xf<<AON_GPADC2_SCAN_NEG_12_SHIFT)
#define AON_GPADC2_SCAN_NEG_13_SHIFT                            (20U)
#define AON_GPADC2_SCAN_NEG_13_MASK                             (0xf<<AON_GPADC2_SCAN_NEG_13_SHIFT)
#define AON_GPADC2_SCAN_NEG_14_SHIFT                            (24U)
#define AON_GPADC2_SCAN_NEG_14_MASK                             (0xf<<AON_GPADC2_SCAN_NEG_14_SHIFT)
#define AON_GPADC2_SCAN_NEG_15_SHIFT                            (28U)
#define AON_GPADC2_SCAN_NEG_15_MASK                             (0xf<<AON_GPADC2_SCAN_NEG_15_SHIFT)

/* 0x96C : gpadc2_reg_ijct_ext_0 */
#define AON_GPADC2_IJCT_EXT_POS_0_SHIFT                         (0U)
#define AON_GPADC2_IJCT_EXT_POS_0_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_POS_0_SHIFT)
#define AON_GPADC2_IJCT_EXT_POS_1_SHIFT                         (4U)
#define AON_GPADC2_IJCT_EXT_POS_1_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_POS_1_SHIFT)
#define AON_GPADC2_IJCT_EXT_POS_2_SHIFT                         (8U)
#define AON_GPADC2_IJCT_EXT_POS_2_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_POS_2_SHIFT)
#define AON_GPADC2_IJCT_EXT_POS_3_SHIFT                         (12U)
#define AON_GPADC2_IJCT_EXT_POS_3_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_POS_3_SHIFT)
#define AON_GPADC2_IJCT_EXT_NEG_0_SHIFT                         (16U)
#define AON_GPADC2_IJCT_EXT_NEG_0_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_NEG_0_SHIFT)
#define AON_GPADC2_IJCT_EXT_NEG_1_SHIFT                         (20U)
#define AON_GPADC2_IJCT_EXT_NEG_1_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_NEG_1_SHIFT)
#define AON_GPADC2_IJCT_EXT_NEG_2_SHIFT                         (24U)
#define AON_GPADC2_IJCT_EXT_NEG_2_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_NEG_2_SHIFT)
#define AON_GPADC2_IJCT_EXT_NEG_3_SHIFT                         (28U)
#define AON_GPADC2_IJCT_EXT_NEG_3_MASK                          (0xf<<AON_GPADC2_IJCT_EXT_NEG_3_SHIFT)

/* 0x970 : gpadc2_reg_result */
#define AON_GPADC2_DATA_OUT_SHIFT                               (0U)
#define AON_GPADC2_DATA_OUT_MASK                                (0x1fffffff<<AON_GPADC2_DATA_OUT_SHIFT)
#define AON_GPADC2_DATA_RDY                                     (1<<31U)

/* 0x974 : gpadc2_reg_raw_data */
#define AON_GPADC2_RAW_DATA_SHIFT                               (0U)
#define AON_GPADC2_RAW_DATA_MASK                                (0xfff<<AON_GPADC2_RAW_DATA_SHIFT)
#define AON_GPADC2_RESERVED_SHIFT                               (16U)
#define AON_GPADC2_RESERVED_MASK                                (0xffff<<AON_GPADC2_RESERVED_SHIFT)

/* 0x978 : gpadc2_reg_satur */
#define AON_GPADC2_NEG_SATUR                                    (1<<0U)
#define AON_GPADC2_POS_SATUR                                    (1<<1U)
#define AON_GPADC2_NEG_SATUR_CLR                                (1<<4U)
#define AON_GPADC2_POS_SATUR_CLR                                (1<<5U)
#define AON_GPADC2_NEG_SATUR_MASK                               (1<<8U)
#define AON_GPADC2_POS_SATUR_MASK                               (1<<9U)

/* 0x97C : gpadc_ten */
#define AON_TEN_GPADC0                                          (1<<0U)
#define AON_TEN_GPADC1                                          (1<<1U)
#define AON_TEN_GPADC2                                          (1<<2U)

#endif  /* __HARDWARE_ADC_V3_H__ */
