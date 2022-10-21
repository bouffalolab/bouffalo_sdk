/**
  ******************************************************************************
  * @file    ir_reg.h
  * @version V1.0
  * @date    2022-09-28
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
#ifndef __HARDWARE_IR_H__
#define __HARDWARE_IR_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define IRTX_CONFIG_OFFSET      (0x0)  /* irtx_config */
#define IRTX_INT_STS_OFFSET     (0x4)  /* irtx_int_sts */
#define IRTX_PULSE_WIDTH_OFFSET (0x10) /* irtx_pulse_width */
#define IRTX_PW_0_OFFSET        (0x14) /* irtx_pw_0 */
#define IRTX_PW_1_OFFSET        (0x18) /* irtx_pw_1 */
#define IRRX_CONFIG_OFFSET      (0x40) /* irrx_config */
#define IRRX_INT_STS_OFFSET     (0x44) /* irrx_int_sts */
#define IRRX_PW_CONFIG_OFFSET   (0x48) /* irrx_pw_config */
#define IRRX_DATA_COUNT_OFFSET  (0x50) /* irrx_data_count */
#define IRRX_DATA_WORD0_OFFSET  (0x54) /* irrx_data_word0 */
#define IRRX_DATA_WORD1_OFFSET  (0x58) /* irrx_data_word1 */
#define IR_FIFO_CONFIG_0_OFFSET (0x80) /* ir_fifo_config_0 */
#define IR_FIFO_CONFIG_1_OFFSET (0x84) /* ir_fifo_config_1 */
#define IR_FIFO_WDATA_OFFSET    (0x88) /* ir_fifo_wdata */
#define IR_FIFO_RDATA_OFFSET    (0x8C) /* ir_fifo_rdata */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : irtx_config */
#define IR_CR_IRTX_EN                   (1 << 0U)
#define IR_CR_IRTX_OUT_INV              (1 << 1U)
#define IR_CR_IRTX_MOD_EN               (1 << 2U)
#define IR_CR_IRTX_SWM_EN               (1 << 3U)
#define IR_CR_IRTX_DATA_EN              (1 << 4U)
#define IR_CR_IRTX_LOGIC0_HL_INV        (1 << 5U)
#define IR_CR_IRTX_LOGIC1_HL_INV        (1 << 6U)
#define IR_CR_IRTX_HEAD_EN              (1 << 8U)
#define IR_CR_IRTX_HEAD_HL_INV          (1 << 9U)
#define IR_CR_IRTX_TAIL_EN              (1 << 10U)
#define IR_CR_IRTX_TAIL_HL_INV          (1 << 11U)
#define IR_CR_IRTX_FRM_EN               (1 << 12U)
#define IR_CR_IRTX_FRM_CONT_EN          (1 << 13U)
#define IR_CR_IRTX_FRM_FRAME_SIZE_SHIFT (14U)
#define IR_CR_IRTX_FRM_FRAME_SIZE_MASK  (0x3 << IR_CR_IRTX_FRM_FRAME_SIZE_SHIFT)
#define IR_CR_IRTX_DATA_NUM_SHIFT       (16U)
#define IR_CR_IRTX_DATA_NUM_MASK        (0x7f << IR_CR_IRTX_DATA_NUM_SHIFT)

/* 0x4 : irtx_int_sts */
#define IRTX_END_INT         (1 << 0U)
#define IRTX_FRDY_INT        (1 << 1U)
#define IRTX_FER_INT         (1 << 2U)
#define IR_CR_IRTX_END_MASK  (1 << 8U)
#define IR_CR_IRTX_FRDY_MASK (1 << 9U)
#define IR_CR_IRTX_FER_MASK  (1 << 10U)
#define IR_CR_IRTX_END_CLR   (1 << 16U)
#define IR_CR_IRTX_END_EN    (1 << 24U)
#define IR_CR_IRTX_FRDY_EN   (1 << 25U)
#define IR_CR_IRTX_FER_EN    (1 << 26U)

/* 0x10 : irtx_pulse_width */
#define IR_CR_IRTX_PW_UNIT_SHIFT   (0U)
#define IR_CR_IRTX_PW_UNIT_MASK    (0xfff << IR_CR_IRTX_PW_UNIT_SHIFT)
#define IR_CR_IRTX_MOD_PH0_W_SHIFT (16U)
#define IR_CR_IRTX_MOD_PH0_W_MASK  (0xff << IR_CR_IRTX_MOD_PH0_W_SHIFT)
#define IR_CR_IRTX_MOD_PH1_W_SHIFT (24U)
#define IR_CR_IRTX_MOD_PH1_W_MASK  (0xff << IR_CR_IRTX_MOD_PH1_W_SHIFT)

/* 0x14 : irtx_pw_0 */
#define IR_CR_IRTX_LOGIC0_PH0_W_SHIFT (0U)
#define IR_CR_IRTX_LOGIC0_PH0_W_MASK  (0xff << IR_CR_IRTX_LOGIC0_PH0_W_SHIFT)
#define IR_CR_IRTX_LOGIC0_PH1_W_SHIFT (8U)
#define IR_CR_IRTX_LOGIC0_PH1_W_MASK  (0xff << IR_CR_IRTX_LOGIC0_PH1_W_SHIFT)
#define IR_CR_IRTX_LOGIC1_PH0_W_SHIFT (16U)
#define IR_CR_IRTX_LOGIC1_PH0_W_MASK  (0xff << IR_CR_IRTX_LOGIC1_PH0_W_SHIFT)
#define IR_CR_IRTX_LOGIC1_PH1_W_SHIFT (24U)
#define IR_CR_IRTX_LOGIC1_PH1_W_MASK  (0xff << IR_CR_IRTX_LOGIC1_PH1_W_SHIFT)

/* 0x18 : irtx_pw_1 */
#define IR_CR_IRTX_HEAD_PH0_W_SHIFT (0U)
#define IR_CR_IRTX_HEAD_PH0_W_MASK  (0xff << IR_CR_IRTX_HEAD_PH0_W_SHIFT)
#define IR_CR_IRTX_HEAD_PH1_W_SHIFT (8U)
#define IR_CR_IRTX_HEAD_PH1_W_MASK  (0xff << IR_CR_IRTX_HEAD_PH1_W_SHIFT)
#define IR_CR_IRTX_TAIL_PH0_W_SHIFT (16U)
#define IR_CR_IRTX_TAIL_PH0_W_MASK  (0xff << IR_CR_IRTX_TAIL_PH0_W_SHIFT)
#define IR_CR_IRTX_TAIL_PH1_W_SHIFT (24U)
#define IR_CR_IRTX_TAIL_PH1_W_MASK  (0xff << IR_CR_IRTX_TAIL_PH1_W_SHIFT)

/* 0x40 : irrx_config */
#define IR_CR_IRRX_EN            (1 << 0U)
#define IR_CR_IRRX_IN_INV        (1 << 1U)
#define IR_CR_IRRX_MODE_SHIFT    (2U)
#define IR_CR_IRRX_MODE_MASK     (0x3 << IR_CR_IRRX_MODE_SHIFT)
#define IR_CR_IRRX_DEG_EN        (1 << 4U)
#define IR_CR_IRRX_DEG_CNT_SHIFT (8U)
#define IR_CR_IRRX_DEG_CNT_MASK  (0xf << IR_CR_IRRX_DEG_CNT_SHIFT)

/* 0x44 : irrx_int_sts */
#define IRRX_END_INT         (1 << 0U)
#define IRRX_FRDY_INT        (1 << 1U)
#define IRRX_FER_INT         (1 << 2U)
#define IR_CR_IRRX_END_MASK  (1 << 8U)
#define IR_CR_IRRX_FRDY_MASK (1 << 9U)
#define IR_CR_IRRX_FER_MASK  (1 << 10U)
#define IR_CR_IRRX_END_CLR   (1 << 16U)
#define IR_CR_IRRX_END_EN    (1 << 24U)
#define IR_CR_IRRX_FRDY_EN   (1 << 25U)
#define IR_CR_IRRX_FER_EN    (1 << 26U)

/* 0x48 : irrx_pw_config */
#define IR_CR_IRRX_DATA_TH_SHIFT (0U)
#define IR_CR_IRRX_DATA_TH_MASK  (0xffff << IR_CR_IRRX_DATA_TH_SHIFT)
#define IR_CR_IRRX_END_TH_SHIFT  (16U)
#define IR_CR_IRRX_END_TH_MASK   (0xffff << IR_CR_IRRX_END_TH_SHIFT)

/* 0x50 : irrx_data_count */
#define IR_STS_IRRX_DATA_CNT_SHIFT (0U)
#define IR_STS_IRRX_DATA_CNT_MASK  (0x7f << IR_STS_IRRX_DATA_CNT_SHIFT)

/* 0x54 : irrx_data_word0 */
#define IR_STS_IRRX_DATA_WORD0_SHIFT (0U)
#define IR_STS_IRRX_DATA_WORD0_MASK  (0xffffffff << IR_STS_IRRX_DATA_WORD0_SHIFT)

/* 0x58 : irrx_data_word1 */
#define IR_STS_IRRX_DATA_WORD1_SHIFT (0U)
#define IR_STS_IRRX_DATA_WORD1_MASK  (0xffffffff << IR_STS_IRRX_DATA_WORD1_SHIFT)

/* 0x80 : ir_fifo_config_0 */
#define IRTX_DMA_EN          (1 << 0U)
#define IR_TX_FIFO_CLR       (1 << 2U)
#define IR_RX_FIFO_CLR       (1 << 3U)
#define IR_TX_FIFO_OVERFLOW  (1 << 4U)
#define IR_TX_FIFO_UNDERFLOW (1 << 5U)
#define IR_RX_FIFO_OVERFLOW  (1 << 6U)
#define IR_RX_FIFO_UNDERFLOW (1 << 7U)

/* 0x84 : ir_fifo_config_1 */
#define IR_TX_FIFO_CNT_SHIFT (0U)
#define IR_TX_FIFO_CNT_MASK  (0x7 << IR_TX_FIFO_CNT_SHIFT)
#define IR_RX_FIFO_CNT_SHIFT (8U)
#define IR_RX_FIFO_CNT_MASK  (0x7f << IR_RX_FIFO_CNT_SHIFT)
#define IR_TX_FIFO_TH_SHIFT  (16U)
#define IR_TX_FIFO_TH_MASK   (0x3 << IR_TX_FIFO_TH_SHIFT)
#define IR_RX_FIFO_TH_SHIFT  (24U)
#define IR_RX_FIFO_TH_MASK   (0x3f << IR_RX_FIFO_TH_SHIFT)

/* 0x88 : ir_fifo_wdata */
#define IR_TX_FIFO_WDATA_SHIFT (0U)
#define IR_TX_FIFO_WDATA_MASK  (0xffffffff << IR_TX_FIFO_WDATA_SHIFT)

/* 0x8C : ir_fifo_rdata */
#define IR_RX_FIFO_RDATA_SHIFT (0U)
#define IR_RX_FIFO_RDATA_MASK  (0xffff << IR_RX_FIFO_RDATA_SHIFT)

#endif /* __HARDWARE_IR_H__ */
