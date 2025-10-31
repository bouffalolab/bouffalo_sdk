/**
  ******************************************************************************
  * @file    i2c_reg.h
  * @version V1.0
  * @date    2022-06-16
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
#ifndef __HARDWARE_I2C_H__
#define __HARDWARE_I2C_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define I2C_CONFIG_OFFSET        (0x0)  /* i2c_config */
#define I2C_INT_STS_OFFSET       (0x4)  /* i2c_int_sts */
#if defined(BL616L)
#define I2C_CONFIG_1_OFFSET      (0x8)  /* i2c_config_1 */
#else
#define I2C_SUB_ADDR_OFFSET      (0x8)  /* i2c_sub_addr */
#endif
#define I2C_BUS_BUSY_OFFSET      (0xC)  /* i2c_bus_busy */
#define I2C_PRD_START_OFFSET     (0x10) /* i2c_prd_start */
#define I2C_PRD_STOP_OFFSET      (0x14) /* i2c_prd_stop */
#define I2C_PRD_DATA_OFFSET      (0x18) /* i2c_prd_data */
#if defined(BL616D)
#define I2C_PKT_LEN_OFFSET       (0x1C) /* i2c_pkt_len */
#endif
#if defined(BL616L)
#define I2C_SUB_ADDR_OFFSET       (0x1C)  /* i2c_sub_addr */
#define I2C_SUB_ADDR_3_0_OFFSET   (0x1C)  /* i2c_sub_addr_3_0 */
#define I2C_SUB_ADDR_7_4_OFFSET   (0x20)  /* i2c_sub_addr_7_4 */
#define I2C_SUB_ADDR_11_8_OFFSET  (0x24)  /* i2c_sub_addr_11_8 */
#define I2C_SUB_ADDR_15_12_OFFSET (0x28)  /* i2c_sub_addr_15_12 */
#endif
#define I2C_FIFO_CONFIG_0_OFFSET (0x80) /* i2c_fifo_config_0 */
#define I2C_FIFO_CONFIG_1_OFFSET (0x84) /* i2c_fifo_config_1 */
#define I2C_FIFO_WDATA_OFFSET    (0x88) /* i2c_fifo_wdata */
#define I2C_FIFO_RDATA_OFFSET    (0x8C) /* i2c_fifo_rdata */
#if defined(BL616L)
#define I2C_SW_USAGE_OFFSET      (0x90)/* i2c_sw_usage */
#define I2C_HW_VERSION_OFFSET    (0x90)/* i2c_hw_version */
#endif

/* Register Bitfield definitions *****************************************************/

/* 0x0 : i2c_config */
#define I2C_CR_I2C_M_EN              (1 << 0U)
#define I2C_CR_I2C_PKT_DIR           (1 << 1U)
#define I2C_CR_I2C_DEG_EN            (1 << 2U)
#define I2C_CR_I2C_SCL_SYNC_EN       (1 << 3U)
#if !defined(BL616L)
#define I2C_CR_I2C_SUB_ADDR_EN       (1 << 4U)
#define I2C_CR_I2C_SUB_ADDR_BC_SHIFT (5U)
#define I2C_CR_I2C_SUB_ADDR_BC_MASK  (0x3 << I2C_CR_I2C_SUB_ADDR_BC_SHIFT)
#endif
#if defined(BL616) || defined(BL616D) || defined(BL606P) || defined(BL808) || defined(BL628) || defined(BL702L)
#define I2C_CR_I2C_10B_ADDR_EN    (1 << 7U)
#define I2C_CR_I2C_SLV_ADDR_SHIFT (8U)
#define I2C_CR_I2C_SLV_ADDR_MASK  (0x3ff << I2C_CR_I2C_SLV_ADDR_SHIFT)
#if !defined(BL616D)
#define I2C_CR_I2C_PKT_LEN_SHIFT  (20U)
#define I2C_CR_I2C_PKT_LEN_MASK   (0xff << I2C_CR_I2C_PKT_LEN_SHIFT)
#endif
#elif defined(BL616L)
#define I2C_CR_I2C_10B_ADDR_EN    (1 << 7U)
#define I2C_CR_I2C_SLV_ADDR_SHIFT (8U)
#define I2C_CR_I2C_SLV_ADDR_MASK  (0x3ff << I2C_CR_I2C_SLV_ADDR_SHIFT)
#define I2C_CR_I2C_PKT_LEN_SHIFT  (18U)
#define I2C_CR_I2C_PKT_LEN_MASK   (0x3ff << I2C_CR_I2C_PKT_LEN_SHIFT)
#else
#define I2C_CR_I2C_SLV_ADDR_SHIFT (8U)
#define I2C_CR_I2C_SLV_ADDR_MASK  (0x7f << I2C_CR_I2C_SLV_ADDR_SHIFT)
#define I2C_CR_I2C_PKT_LEN_SHIFT  (16U)
#define I2C_CR_I2C_PKT_LEN_MASK   (0xff << I2C_CR_I2C_PKT_LEN_SHIFT)
#endif

#define I2C_CR_I2C_DEG_CNT_SHIFT (28U)
#define I2C_CR_I2C_DEG_CNT_MASK  (0xf << I2C_CR_I2C_DEG_CNT_SHIFT)

/* 0x4 : i2c_int_sts */
#define I2C_END_INT         (1 << 0U)
#define I2C_TXF_INT         (1 << 1U)
#define I2C_RXF_INT         (1 << 2U)
#define I2C_NAK_INT         (1 << 3U)
#define I2C_ARB_INT         (1 << 4U)
#define I2C_FER_INT         (1 << 5U)
#if defined(BL616L)
#define I2C_TO_INT          (1 << 6U)
#define I2C_CR_I2C_TO_INT   (1 << 7U)
#endif
#define I2C_CR_I2C_END_MASK (1 << 8U)
#define I2C_CR_I2C_TXF_MASK (1 << 9U)
#define I2C_CR_I2C_RXF_MASK (1 << 10U)
#define I2C_CR_I2C_NAK_MASK (1 << 11U)
#define I2C_CR_I2C_ARB_MASK (1 << 12U)
#define I2C_CR_I2C_FER_MASK (1 << 13U)
#if defined(BL616L)
#define I2C_CR_I2C_TO_MASK  (1 << 14U)
#endif
#define I2C_CR_I2C_END_CLR  (1 << 16U)
#if defined(BL616L)
#define I2C_CR_I2C_TO_CLR   (1 << 18U)
#endif
#define I2C_CR_I2C_NAK_CLR  (1 << 19U)
#define I2C_CR_I2C_ARB_CLR  (1 << 20U)
#define I2C_CR_I2C_END_EN   (1 << 24U)
#define I2C_CR_I2C_TXF_EN   (1 << 25U)
#define I2C_CR_I2C_RXF_EN   (1 << 26U)
#define I2C_CR_I2C_NAK_EN   (1 << 27U)
#define I2C_CR_I2C_ARB_EN   (1 << 28U)
#define I2C_CR_I2C_FER_EN   (1 << 29U)
#if defined(BL616L)
#define I2C_CR_I2C_TO_EN    (1 << 30U)
#endif

#if defined(BL616L)
/* 0x8 : i2c_config_1 */
#define I2C_CR_I2C_SUB_ADDR_EN       (1 << 0U)
#define I2C_CR_I2C_SUB_ADDR_BC_SHIFT (1U)
#define I2C_CR_I2C_SUB_ADDR_BC_MASK  (0xf << I2C_CR_I2C_SUB_ADDR_BC_SHIFT)
#else
/* 0x8 : i2c_sub_addr */
#define I2C_CR_I2C_SUB_ADDR_B0_SHIFT (0U)
#define I2C_CR_I2C_SUB_ADDR_B0_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B0_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B1_SHIFT (8U)
#define I2C_CR_I2C_SUB_ADDR_B1_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B1_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B2_SHIFT (16U)
#define I2C_CR_I2C_SUB_ADDR_B2_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B2_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B3_SHIFT (24U)
#define I2C_CR_I2C_SUB_ADDR_B3_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B3_SHIFT)
#endif

/* 0xC : i2c_bus_busy */
#define I2C_STS_I2C_BUS_BUSY    (1 << 0U)
#define I2C_CR_I2C_BUS_BUSY_CLR (1 << 1U)

/* 0x10 : i2c_prd_start */
#define I2C_CR_I2C_PRD_S_PH_0_SHIFT (0U)
#define I2C_CR_I2C_PRD_S_PH_0_MASK  (0xff << I2C_CR_I2C_PRD_S_PH_0_SHIFT)
#define I2C_CR_I2C_PRD_S_PH_1_SHIFT (8U)
#define I2C_CR_I2C_PRD_S_PH_1_MASK  (0xff << I2C_CR_I2C_PRD_S_PH_1_SHIFT)
#define I2C_CR_I2C_PRD_S_PH_2_SHIFT (16U)
#define I2C_CR_I2C_PRD_S_PH_2_MASK  (0xff << I2C_CR_I2C_PRD_S_PH_2_SHIFT)
#define I2C_CR_I2C_PRD_S_PH_3_SHIFT (24U)
#define I2C_CR_I2C_PRD_S_PH_3_MASK  (0xff << I2C_CR_I2C_PRD_S_PH_3_SHIFT)

/* 0x14 : i2c_prd_stop */
#define I2C_CR_I2C_PRD_P_PH_0_SHIFT (0U)
#define I2C_CR_I2C_PRD_P_PH_0_MASK  (0xff << I2C_CR_I2C_PRD_P_PH_0_SHIFT)
#define I2C_CR_I2C_PRD_P_PH_1_SHIFT (8U)
#define I2C_CR_I2C_PRD_P_PH_1_MASK  (0xff << I2C_CR_I2C_PRD_P_PH_1_SHIFT)
#define I2C_CR_I2C_PRD_P_PH_2_SHIFT (16U)
#define I2C_CR_I2C_PRD_P_PH_2_MASK  (0xff << I2C_CR_I2C_PRD_P_PH_2_SHIFT)
#define I2C_CR_I2C_PRD_P_PH_3_SHIFT (24U)
#define I2C_CR_I2C_PRD_P_PH_3_MASK  (0xff << I2C_CR_I2C_PRD_P_PH_3_SHIFT)

/* 0x18 : i2c_prd_data */
#define I2C_CR_I2C_PRD_D_PH_0_SHIFT (0U)
#define I2C_CR_I2C_PRD_D_PH_0_MASK  (0xff << I2C_CR_I2C_PRD_D_PH_0_SHIFT)
#define I2C_CR_I2C_PRD_D_PH_1_SHIFT (8U)
#define I2C_CR_I2C_PRD_D_PH_1_MASK  (0xff << I2C_CR_I2C_PRD_D_PH_1_SHIFT)
#define I2C_CR_I2C_PRD_D_PH_2_SHIFT (16U)
#define I2C_CR_I2C_PRD_D_PH_2_MASK  (0xff << I2C_CR_I2C_PRD_D_PH_2_SHIFT)
#define I2C_CR_I2C_PRD_D_PH_3_SHIFT (24U)
#define I2C_CR_I2C_PRD_D_PH_3_MASK  (0xff << I2C_CR_I2C_PRD_D_PH_3_SHIFT)

#if defined(BL616D)
/* 0x1C : i2c_pkt_len */
#define I2C_CR_I2C_PKT_LEN_SHIFT (0U)
#define I2C_CR_I2C_PKT_LEN_MASK  (0xfff << I2C_CR_I2C_PKT_LEN_SHIFT)
#endif

#if defined(BL616L)
/* 0x1C : i2c_sub_addr_3_0 */
#define I2C_CR_I2C_SUB_ADDR_B0_SHIFT (0U)
#define I2C_CR_I2C_SUB_ADDR_B0_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B0_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B1_SHIFT (8U)
#define I2C_CR_I2C_SUB_ADDR_B1_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B1_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B2_SHIFT (16U)
#define I2C_CR_I2C_SUB_ADDR_B2_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B2_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B3_SHIFT (24U)
#define I2C_CR_I2C_SUB_ADDR_B3_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B3_SHIFT)

/* 0x20 : i2c_sub_addr_7_4 */
#define I2C_CR_I2C_SUB_ADDR_B4_SHIFT (0U)
#define I2C_CR_I2C_SUB_ADDR_B4_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B4_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B5_SHIFT (8U)
#define I2C_CR_I2C_SUB_ADDR_B5_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B5_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B6_SHIFT (16U)
#define I2C_CR_I2C_SUB_ADDR_B6_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B6_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B7_SHIFT (24U)
#define I2C_CR_I2C_SUB_ADDR_B7_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B7_SHIFT)

/* 0x24 : i2c_sub_addr_11_8 */
#define I2C_CR_I2C_SUB_ADDR_B8_SHIFT (0U)
#define I2C_CR_I2C_SUB_ADDR_B8_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B8_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B9_SHIFT (8U)
#define I2C_CR_I2C_SUB_ADDR_B9_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B9_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B10_SHIFT (16U)
#define I2C_CR_I2C_SUB_ADDR_B10_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B10_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B11_SHIFT (24U)
#define I2C_CR_I2C_SUB_ADDR_B11_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B11_SHIFT)

/* 0x28 : i2c_sub_addr_15_12 */
#define I2C_CR_I2C_SUB_ADDR_B12_SHIFT (0U)
#define I2C_CR_I2C_SUB_ADDR_B12_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B12_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B13_SHIFT (8U)
#define I2C_CR_I2C_SUB_ADDR_B13_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B13_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B14_SHIFT (16U)
#define I2C_CR_I2C_SUB_ADDR_B14_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B14_SHIFT)
#define I2C_CR_I2C_SUB_ADDR_B15_SHIFT (24U)
#define I2C_CR_I2C_SUB_ADDR_B15_MASK  (0xff << I2C_CR_I2C_SUB_ADDR_B15_SHIFT)
#endif

/* 0x80 : i2c_fifo_config_0 */
#define I2C_DMA_TX_EN         (1 << 0U)
#define I2C_DMA_RX_EN         (1 << 1U)
#define I2C_TX_FIFO_CLR       (1 << 2U)
#define I2C_RX_FIFO_CLR       (1 << 3U)
#define I2C_TX_FIFO_OVERFLOW  (1 << 4U)
#define I2C_TX_FIFO_UNDERFLOW (1 << 5U)
#define I2C_RX_FIFO_OVERFLOW  (1 << 6U)
#define I2C_RX_FIFO_UNDERFLOW (1 << 7U)
#if defined(BL616L)
#define I2C_CR_I2C_M_TO_POP_VALUE_SHIFT (12U)
#define I2C_CR_I2C_M_TO_POP_VALUE_MASK  (0xfffff << I2C_CR_I2C_M_TO_POP_VALUE_SHIFT)
#endif

/* 0x84 : i2c_fifo_config_1 */
#define I2C_TX_FIFO_CNT_SHIFT (0U)
#define I2C_TX_FIFO_CNT_MASK  (0x3 << I2C_TX_FIFO_CNT_SHIFT)
#define I2C_RX_FIFO_CNT_SHIFT (8U)
#define I2C_RX_FIFO_CNT_MASK  (0x3 << I2C_RX_FIFO_CNT_SHIFT)
#define I2C_TX_FIFO_TH        (1 << 16U)
#define I2C_RX_FIFO_TH        (1 << 24U)

/* 0x88 : i2c_fifo_wdata */
#define I2C_FIFO_WDATA_SHIFT (0U)
#define I2C_FIFO_WDATA_MASK  (0xffffffff << I2C_FIFO_WDATA_SHIFT)

/* 0x8C : i2c_fifo_rdata */
#define I2C_FIFO_RDATA_SHIFT (0U)
#define I2C_FIFO_RDATA_MASK  (0xffffffff << I2C_FIFO_RDATA_SHIFT)

#if defined(BL616L)
/* 0x90 : i2c_sw_usage and i2c_hw_version */
#define I2C_SW_USAGE_SHIFT   (0U)
#define I2C_SW_USAGE_MASK    (0xffffff << I2C_SW_USAGE_SHIFT)
#define I2C_HW_VERSION_SHIFT (24U)
#define I2C_HW_VERSION_MASK  (0xff << I2C_HW_VERSION_SHIFT)
#endif

#endif /* __HARDWARE_I2C_H__ */
