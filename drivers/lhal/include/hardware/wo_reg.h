/**
  ******************************************************************************
  * @file    wo_reg.h
  * @version V1.0
  * @date    2023-08-15
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
#ifndef __HARDWARE_WO_H__
#define __HARDWARE_WO_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#if defined(BL616) || defined(BL606P) || defined(BL808) || defined(BL628)
#define GLB_GPIO_CFG0_OFFSET                                    (0x8C4)/* gpio_cfg0 */
#define GLB_GPIO_CFG142_OFFSET                                  (0xAFC)/* gpio_cfg142 */
#define GLB_GPIO_CFG143_OFFSET                                  (0xB00)/* gpio_cfg143 */
#define GLB_GPIO_CFG144_OFFSET                                  (0xB04)/* gpio_cfg144 */
#endif

/* Register Bitfield definitions *****************************************************/

/* 0x8C4 : gpio_cfg0 */
#define GLB_REG_GPIO_0_IE                                       (1<<0U)
#define GLB_REG_GPIO_0_SMT                                      (1<<1U)
#define GLB_REG_GPIO_0_DRV_SHIFT                                (2U)
#define GLB_REG_GPIO_0_DRV_MASK                                 (0x3<<GLB_REG_GPIO_0_DRV_SHIFT)
#define GLB_REG_GPIO_0_PU                                       (1<<4U)
#define GLB_REG_GPIO_0_PD                                       (1<<5U)
#define GLB_REG_GPIO_0_OE                                       (1<<6U)
#define GLB_REG_GPIO_0_FUNC_SEL_SHIFT                           (8U)
#define GLB_REG_GPIO_0_FUNC_SEL_MASK                            (0x1f<<GLB_REG_GPIO_0_FUNC_SEL_SHIFT)
#define GLB_REG_GPIO_0_INT_MODE_SET_SHIFT                       (16U)
#define GLB_REG_GPIO_0_INT_MODE_SET_MASK                        (0xf<<GLB_REG_GPIO_0_INT_MODE_SET_SHIFT)
#define GLB_REG_GPIO_0_INT_CLR                                  (1<<20U)
#define GLB_GPIO_0_INT_STAT                                     (1<<21U)
#define GLB_REG_GPIO_0_INT_MASK                                 (1<<22U)
#define GLB_REG_GPIO_0_O                                        (1<<24U)
#define GLB_REG_GPIO_0_SET                                      (1<<25U)
#define GLB_REG_GPIO_0_CLR                                      (1<<26U)
#define GLB_REG_GPIO_0_I                                        (1<<28U)
#define GLB_REG_GPIO_0_MODE_SHIFT                               (30U)
#define GLB_REG_GPIO_0_MODE_MASK                                (0x3<<GLB_REG_GPIO_0_MODE_SHIFT)

/* 0xAFC : gpio_cfg142 */
#define GLB_CR_GPIO_TX_EN                                       (1<<0U)
#define GLB_CR_INVERT_CODE0_HIGH                                (1<<1U)
#define GLB_CR_INVERT_CODE1_HIGH                                (1<<2U)
#define GLB_CR_CORE_GPIO_LS1P8_SHIFT                            (3U)
#define GLB_CR_CORE_GPIO_LS1P8_MASK                             (0x7<<GLB_CR_CORE_GPIO_LS1P8_SHIFT)
#define GLB_CR_CODE_TOTAL_TIME_SHIFT                            (7U)
#define GLB_CR_CODE_TOTAL_TIME_MASK                             (0x1ff<<GLB_CR_CODE_TOTAL_TIME_SHIFT)
#define GLB_CR_CODE0_HIGH_TIME_SHIFT                            (16U)
#define GLB_CR_CODE0_HIGH_TIME_MASK                             (0xff<<GLB_CR_CODE0_HIGH_TIME_SHIFT)
#define GLB_CR_CODE1_HIGH_TIME_SHIFT                            (24U)
#define GLB_CR_CODE1_HIGH_TIME_MASK                             (0xff<<GLB_CR_CODE1_HIGH_TIME_SHIFT)

/* 0xB00 : gpio_cfg143 */
#define GLB_CR_GPIO_DMA_TX_EN                                   (1<<0U)
#define GLB_CR_GPIO_DMA_OUT_SEL_LATCH                           (1<<1U)
#define GLB_GPIO_TX_FIFO_CLR                                    (1<<2U)
#define GLB_GPIO_TX_END_CLR                                     (1<<3U)
#define GLB_GPIO_TX_FIFO_OVERFLOW                               (1<<4U)
#define GLB_GPIO_TX_FIFO_UNDERFLOW                              (1<<5U)
#define GLB_CR_GPIO_DMA_PARK_VALUE                              (1<<7U)
#define GLB_GPIO_TX_FIFO_CNT_SHIFT                              (8U)
#define GLB_GPIO_TX_FIFO_CNT_MASK                               (0xff<<GLB_GPIO_TX_FIFO_CNT_SHIFT)
#define GLB_CR_GPIO_TX_FIFO_TH_SHIFT                            (16U)
#define GLB_CR_GPIO_TX_FIFO_TH_MASK                             (0x7f<<GLB_CR_GPIO_TX_FIFO_TH_SHIFT)
#define GLB_CR_GPIO_TX_END_MASK                                 (1<<23U)
#define GLB_CR_GPIO_TX_FIFO_MASK                                (1<<24U)
#define GLB_CR_GPIO_TX_FER_MASK                                 (1<<25U)
#define GLB_R_GPIO_TX_END_INT                                   (1<<26U)
#define GLB_R_GPIO_TX_FIFO_INT                                  (1<<27U)
#define GLB_R_GPIO_TX_FER_INT                                   (1<<28U)
#define GLB_CR_GPIO_TX_END_EN                                   (1<<29U)
#define GLB_CR_GPIO_TX_FIFO_EN                                  (1<<30U)
#define GLB_CR_GPIO_TX_FER_EN                                   (1<<31U)

/* 0xB04 : gpio_cfg144 */
#define GLB_GPIO_TX_DATA_TO_FIFO_SHIFT                          (0U)
#define GLB_GPIO_TX_DATA_TO_FIFO_MASK                           (0xffff<<GLB_GPIO_TX_DATA_TO_FIFO_SHIFT)

#endif /* __HARDWARE_WO_H__ */
