/**
  ******************************************************************************
  * @file    bflb_pec_reg.h
  * @version V1.0
  * @date    2024-02-26
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
#ifndef  __BFLB_PEC_REG_H__
#define  __BFLB_PEC_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* general control register offsets *********************************************************/
#define PEC_ORIGIN_OFFSET                                       (0x0)/* PEC origin address register */
#define PEC_RESET_OFFSET                                        (0x4)/* SM reset and enable register */
#define PEC_TXF_OFFSET                                          (0x10)/* 0x10~0x1C is orderly for TXF0~3 */
#define PEC_RXF_OFFSET                                          (0x20)/* 0x20~0x2C is orderly for RXF0~3 */
#define PEC_IPC_OFFSET                                          (0x30)/* IPC status register */
#define PEC_IPC_CLR_OFFSET                                      (0x30)/* IPC clear register */
#define PEC_IPC_SET_OFFSET                                      (0x34)/* IPC set register */
#define PEC_INPUT_SYNC_BYPASS_OFFSET                            (0x38)/* input 2-ff sync bypass register */
#define PEC_MEM_OFFSET                                          (0x400)/* Instruction memory register */

/* state machine register offsets *********************************************************/
#define PEC_SM_CLKDIV_OFFSET                                    (0x40)/* Clock divider register for state machine N. Frequency = clock freq / (CLKDIV + 1) */
#define PEC_SM_CFG0_OFFSET                                      (0x44)/* State machine config0 register */
#define PEC_SM_CFG1_OFFSET                                      (0x48)/* State machine config1 register */
#define PEC_SM_INSTR_OFFSET                                     (0x4C)/* State machine instruction register */
#define PEC_SM_SETPIN_OFFSET                                    (0x50)/* State machine set-pin group control register */
#define PEC_SM_OUTPIN_OFFSET                                    (0x54)/* State machine out-pin group control register */
#define PEC_SM_INPIN_OFFSET                                     (0x58)/* State machine in-pin group control register */
#define PEC_SM_FIFO_CTRL_OFFSET                                 (0x5C)/* State machine fifo threshold control and level status register */
#define PEC_SM_OFFSET                                           (0x20)

/* pad register offsets *********************************************************/
#define PEC_PAD_O_INV_OFFSET                                    (0xC0)/* Pad output inverse contro register */
#define PEC_PAD_OE_INV_OFFSET                                   (0xC8)/* Pad output enable inverse contro register */
#define PEC_PAD_I_INV_OFFSET                                    (0xD0)/* Pad input inverse contro register */
#define PEC_PAD_O_CLR_OFFSET                                    (0xD8)/* Pad output clear register, write '1' clear, write '0' has no effect */
#define PEC_PAD_O_SET_OFFSET                                    (0xE0)/* Pad output set register, write '1' set, write '0' has no effect */
#define PEC_PAD_OE_CLR_OFFSET                                   (0xE8)/* Pad output enable clear register, write '1' clear, write '0' has no effect */
#define PEC_PAD_OE_SET_OFFSET                                   (0xF0)/* Pad output enable set register, write '1' set, write '0' has no effect */
#define PEC_PAD_O_OFFSET                                        (0xD8)/* Pad output state register, only active when read */
#define PEC_PAD_OE_OFFSET                                       (0xE8)/* Pad output enable state register, only active when read */

/* interrupt register offsets *********************************************************/
#define PEC_INT_IPC_EN_OFFSET                                   (0XF8)/* IPC every bit enable interrupt */
#define PEC_INT_RAW_OFFSET                                      (0xFC)/* Raw interrupt status */
#define PEC_IRQ0_INTE_OFFSET                                    (0x100)/* Interrupt Enable for irq0 */
#define PEC_IRQ0_INTS_OFFSET                                    (0x104)/* Interrupt status after masking with PEC_IRQ0_INTE_OFFSET */
#define PEC_IRQ1_INTE_OFFSET                                    (0x108)/* Interrupt Enable for irq1 */
#define PEC_IRQ1_INTS_OFFSET                                    (0x10C)/* Interrupt status after masking with PEC_IRQ1_INTE_OFFSET */

/* general register offsets *********************************************************/
#define PEC_SM_R0_OFFSET                                        (0x110)/* state machine r0 register */
#define PEC_SM_R0L_RS_OFFSET                                    (0x114)/* state machine r0[15:0] set & reset register */
#define PEC_SM_R0H_RS_OFFSET                                    (0x118)/* state machine r0[31:16] set & reset register */
#define PEC_SM_R1_OFFSET                                        (0x11C)/* state machine r1 register */
#define PEC_SM_R1L_RS_OFFSET                                    (0x120)/* state machine r1[15:0] set & reset register */
#define PEC_SM_R1H_RS_OFFSET                                    (0x124)/* state machine r1[31:16] set & reset register */
#define PEC_SM_R2_OFFSET                                        (0x128)/* state machine r2 register */
#define PEC_SM_R2L_RS_OFFSET                                    (0x12C)/* state machine r2[15:0] set & reset register */
#define PEC_SM_R2H_RS_OFFSET                                    (0x130)/* state machine r2[31:16] set & reset register */
#define PEC_SM_R3_OFFSET                                        (0x134)/* state machine r3 register */
#define PEC_SM_R3L_RS_OFFSET                                    (0x138)/* state machine r3[15:0] set & reset register */
#define PEC_SM_R3H_RS_OFFSET                                    (0x13C)/* state machine r3[31:16] set & reset register */
#define PEC_SM_R4_OFFSET                                        (0x140)/* state machine r4 register */
#define PEC_SM_R4L_RS_OFFSET                                    (0x144)/* state machine r4[15:0] set & reset register */
#define PEC_SM_R4H_RS_OFFSET                                    (0x148)/* state machine r4[31:16] set & reset register */
#define PEC_SM_R5_OFFSET                                        (0x14C)/* state machine r5 register */
#define PEC_SM_R5L_RS_OFFSET                                    (0x150)/* state machine r5[15:0] set & reset register */
#define PEC_SM_R5H_RS_OFFSET                                    (0x154)/* state machine r5[31:16] set & reset register */
#define PEC_SM_REG_OFFSET                                       (0x48)
#define PEC_SM_REG_REG_OFFSET                                   (0xC)

/* reserved register offsets *********************************************************/
#define PEC_HW_RESV_OFFSET                                      (0x3F0)/* Reserved register for hardware */
#define PEC_SW_RESV_OFFSET                                      (0x3F4)/* Reserved register for software */

#if defined(BL616L)
#define PEC_DEGLITCH_0_OFFSET                                   (0x230)/* deglitch config register for group 0 */
#define PEC_DEGLITCH_1_OFFSET                                   (0x234)/* deglitch config register for group 1 */
#define PEC_DEGLITCH_2_OFFSET                                   (0x238)/* deglitch config register for group 2 */
#define PEC_DEGLITCH_3_OFFSET                                   (0x23C)/* deglitch config register for group 3 */
#define PEC_MEM_CFG_OFFSET                                      (0x3F8)/* config instruction memory access privilege */
#endif

/* Register Bitfield definitions *****************************************************/

/* 0x0 : PEC origin address register */
#define PEC_CR_SM_ORIGIN_SHIFT                                  (0U)
#define PEC_CR_SM_ORIGIN_MASK                                   (0xff<<PEC_CR_SM_ORIGIN_SHIFT)
#define PEC_ORIGIN_SM_OFFSET                                    (8U)

/* 0x4 : SM reset and enable register */
#define PEC_CR_SM_EN                                            (1<<0U)
#define PEC_CR_SM_RESET                                         (1<<1U)
#define PEC_CR_SM_CLKDIV_RESET                                  (1<<2U)
#define PEC_RESET_SM_OFFSET                                     (8U)

/* 0x40 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV + 1) */
#define PEC_CR_SM_CLKDIV_SHIFT                                  (0U)
#define PEC_CR_SM_CLKDIV_MASK                                   (0xffff<<PEC_CR_SM_CLKDIV_SHIFT)

/* 0x44 : State machine config0 register */
#define PEC_CR_SM_WRAP_BOTTOM_SHIFT                            (0U)
#define PEC_CR_SM_WRAP_BOTTOM_MASK                             (0xff<<PEC_CR_SM_WRAP_BOTTOM_SHIFT)
#define PEC_CR_SM_WRAP_TOP_SHIFT                               (8U)
#define PEC_CR_SM_WRAP_TOP_MASK                                (0xff<<PEC_CR_SM_WRAP_TOP_SHIFT)
#define PEC_CR_SM_AUTOPUSH                                     (1<<16U)
#define PEC_CR_SM_AUTOPULL                                     (1<<17U)
#define PEC_CR_SM_IN_SHIFTDIR                                  (1<<18U)
#define PEC_CR_SM_OUT_SHIFTDIR                                 (1<<19U)
#define PEC_CR_SM_TXF_CLR                                      (1<<25U)
#define PEC_CR_SM_RXF_CLR                                      (1<<26U)
#define PEC_CR_SM_FIFO_RX_JOIN_TX                              (1<<27U)
#define PEC_CR_SM_FIFO_TX_JOIN_RX                              (1<<28U)
#define PEC_CR_SM_SIDE_PINDIR                                  (1<<29U)
#define PEC_CR_SM_SIDE_EN                                      (1<<30U)
#define PEC_ST_SM_EXEC_STALLED                                 (1<<31U)

/* 0x48 : State machine config1 register */
#define PEC_ST_SM_ADDR_SHIFT                                   (0U)
#define PEC_ST_SM_ADDR_MASK                                    (0xff<<PEC_ST_SM_ADDR_SHIFT)
#define PEC_CR_SM_PUSH_THRESH_SHIFT                            (8U)
#define PEC_CR_SM_PUSH_THRESH_MASK                             (0x1f<<PEC_CR_SM_PUSH_THRESH_SHIFT)
#define PEC_CR_SM_PULL_THRESH_SHIFT                            (13U)
#define PEC_CR_SM_PULL_THRESH_MASK                             (0x1f<<PEC_CR_SM_PULL_THRESH_SHIFT)
#define PEC_CR_SM_SIDE_BASE_SHIFT                              (18U)
#define PEC_CR_SM_SIDE_BASE_MASK                               (0x3f<<PEC_CR_SM_SIDE_BASE_SHIFT)
#define PEC_CR_SM_SIDE_COUNT_SHIFT                             (24U)
#define PEC_CR_SM_SIDE_COUNT_MASK                              (0x7<<PEC_CR_SM_SIDE_COUNT_SHIFT)
#define PEC_CR_SM_DMA_TX_EN                                    (1<<27U)
#define PEC_CR_SM_DMA_RX_EN                                    (1<<28U)

/* 0x50 : State machine set-pin group control register */
#define PEC_CR_SM_SET_BASE_0_SHIFT                             (0U)
#define PEC_CR_SM_SET_BASE_0_MASK                              (0x3f<<PEC_CR_SM_SET_BASE_0_SHIFT)
#define PEC_CR_SM_SET_COUNT_0_SHIFT                            (6U)
#define PEC_CR_SM_SET_COUNT_0_MASK                             (0x3<<PEC_CR_SM_SET_COUNT_0_SHIFT)
#define PEC_CR_SM_SET_BASE_1_SHIFT                             (8U)
#define PEC_CR_SM_SET_BASE_1_MASK                              (0x3f<<PEC_CR_SM_SET_BASE_1_SHIFT)
#define PEC_CR_SM_SET_COUNT_1_SHIFT                            (14U)
#define PEC_CR_SM_SET_COUNT_1_MASK                             (0x3<<PEC_CR_SM_SET_COUNT_1_SHIFT)
#define PEC_CR_SM_SET_BASE_2_SHIFT                             (16U)
#define PEC_CR_SM_SET_BASE_2_MASK                              (0x3f<<PEC_CR_SM_SET_BASE_2_SHIFT)
#define PEC_CR_SM_SET_COUNT_2_SHIFT                            (22U)
#define PEC_CR_SM_SET_COUNT_2_MASK                             (0x3<<PEC_CR_SM_SET_COUNT_2_SHIFT)
#define PEC_CR_SM_SET_BASE_3_SHIFT                             (24U)
#define PEC_CR_SM_SET_BASE_3_MASK                              (0x3f<<PEC_CR_SM_SET_BASE_3_SHIFT)
#define PEC_CR_SM_SET_COUNT_3_SHIFT                            (30U)
#define PEC_CR_SM_SET_COUNT_3_MASK                             (0x3<<PEC_CR_SM_SET_COUNT_3_SHIFT)

/* 0x54 : State machine out-pin group control register */
#define PEC_CR_SM_OUT_BASE_0_SHIFT                             (0U)
#define PEC_CR_SM_OUT_BASE_0_MASK                              (0x3f<<PEC_CR_SM_OUT_BASE_0_SHIFT)
#define PEC_CR_SM_OUT_COUNT_0_SHIFT                            (6U)
#define PEC_CR_SM_OUT_COUNT_0_MASK                             (0xf<<PEC_CR_SM_OUT_COUNT_0_SHIFT)
#define PEC_CR_SM_OUT_BASE_1_SHIFT                             (10U)
#define PEC_CR_SM_OUT_BASE_1_MASK                              (0x3f<<PEC_CR_SM_OUT_BASE_1_SHIFT)
#define PEC_CR_SM_OUT_COUNT_1_SHIFT                            (16U)
#define PEC_CR_SM_OUT_COUNT_1_MASK                             (0x1f<<PEC_CR_SM_OUT_COUNT_1_SHIFT)
#define PEC_CR_SM_OUT_BASE_2_SHIFT                             (21U)
#define PEC_CR_SM_OUT_BASE_2_MASK                              (0x3f<<PEC_CR_SM_OUT_BASE_2_SHIFT)
#define PEC_CR_SM_OUT_COUNT_2_SHIFT                            (27U)
#define PEC_CR_SM_OUT_COUNT_2_MASK                             (0x1f<<PEC_CR_SM_OUT_COUNT_2_SHIFT)

/* 0x58 : State machine in-pin group control register */
#define PEC_CR_SM_IN_BASE_0_SHIFT                              (0U)
#define PEC_CR_SM_IN_BASE_0_MASK                               (0x3f<<PEC_CR_SM_IN_BASE_0_SHIFT)
#define PEC_CR_SM_IN_BASE_1_SHIFT                              (6U)
#define PEC_CR_SM_IN_BASE_1_MASK                               (0x3f<<PEC_CR_SM_IN_BASE_1_SHIFT)
#define PEC_CR_SM_IN_BASE_2_SHIFT                              (12U)
#define PEC_CR_SM_IN_BASE_2_MASK                               (0x3f<<PEC_CR_SM_IN_BASE_2_SHIFT)
#define PEC_CR_SM_IN_BASE_3_SHIFT                              (18U)
#define PEC_CR_SM_IN_BASE_3_MASK                               (0x3f<<PEC_CR_SM_IN_BASE_3_SHIFT)
#define PEC_CR_SM_IN_BASE_4_SHIFT                              (24U)
#define PEC_CR_SM_IN_BASE_4_MASK                               (0x3f<<PEC_CR_SM_IN_BASE_4_SHIFT)

/* 0x5C ：State machine fifo threshold control and level status register */
#if defined(BL616L)
#define PEC_ST_SM_TX_LEVEL_SHIFT                               (0U)
#define PEC_ST_SM_TX_LEVEL_MASK                                (0x7f<<PEC_ST_SM_TX_LEVEL_SHIFT)
#define PEC_ST_SM_RX_LEVEL_SHIFT                               (8U)
#define PEC_ST_SM_RX_LEVEL_MASK                                (0x7f<<PEC_ST_SM_RX_LEVEL_SHIFT)
#define PEC_CR_SM_TXF_THR_SHIFT                                (16U)
#define PEC_CR_SM_TXF_THR_MASK                                 (0x7f<<PEC_CR_SM_TXF_THR_SHIFT)
#define PEC_CR_SM_RXF_THR_SHIFT                                (24U)
#define PEC_CR_SM_RXF_THR_MASK                                 (0x7f<<PEC_CR_SM_RXF_THR_SHIFT)
#elif defined(BL616D)
#define PEC_ST_SM_TX_LEVEL_SHIFT                               (0U)
#define PEC_ST_SM_TX_LEVEL_MASK                                (0x3f<<PEC_ST_SM_TX_LEVEL_SHIFT)
#define PEC_ST_SM_RX_LEVEL_SHIFT                               (6U)
#define PEC_ST_SM_RX_LEVEL_MASK                                (0x3f<<PEC_ST_SM_RX_LEVEL_SHIFT)
#define PEC_CR_SM_TXF_THR_SHIFT                                (12U)
#define PEC_CR_SM_TXF_THR_MASK                                 (0x1f<<PEC_CR_SM_TXF_THR_SHIFT)
#define PEC_CR_SM_RXF_THR_SHIFT                                (17U)
#define PEC_CR_SM_RXF_THR_MASK                                 (0x1f<<PEC_CR_SM_RXF_THR_SHIFT)
#endif

/* 0xFC : Raw interrupt status */
#define PEC_ST_INTR_SM_RXF_RDY                                 (1<<0U)
#define PEC_ST_INTR_SM_TXF_RDY                                 (1<<4U)

#if defined(BL616L)
/* 0x3F8 : Config instruction memory access privilege */
#define PEC_MEM_SM0_BUS_ACCESS                                 (1<<0U)
#define PEC_MEM_SM1_BUS_ACCESS                                 (1<<1U)
#endif

#endif  /* __BFLB_PEC_REG_H__ */
