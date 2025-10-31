/**
  ******************************************************************************
  * @file    dma2d_reg.h
  * @version V1.0
  * @date    2025-04-28
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
#ifndef __DMA2D_REG_H__
#define __DMA2D_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define DMA2D_DMA_INTSTATUS_OFFSET   (0x0)  /* DMA_IntStatus */
#define DMA2D_DMA_INTTCSTATUS_OFFSET (0x4)  /* DMA_IntTCStatus */
#define DMA2D_DMA_INTTCCLEAR_OFFSET  (0x8)  /* DMA_IntTCClear */
#define DMA2D_DMA_ENBLDCHNS_OFFSET   (0xC)  /* DMA_EnbldChns */
#define DMA2D_DMA_CONFIG_OFFSET      (0x10) /* DMA_Config */
#define DMA2D_DMA_SYNC_OFFSET        (0x14) /* DMA_Sync */
#define DMA2D_DMA_SOFTBREQ_OFFSET    (0x20) /* DMA_SoftBReq */
#define DMA2D_DMA_SOFTLBREQ_OFFSET   (0x24) /* DMA_SoftLBReq */
#define DMA2D_DMA_SOFTSREQ_OFFSET    (0x28) /* DMA_SoftSReq */
#define DMA2D_DMA_SOFTLSREQ_OFFSET   (0x2C) /* DMA_SoftLSReq */
#define DMA2D_DMA_QOS_OFFSET         (0x38) /* DMA_QOS */
#define DMA2D_CxSRCADDR_OFFSET       (0x00) /* DMA2D_CxSrcAddr */
#define DMA2D_CxDSTADDR_OFFSET       (0x04) /* DMA2D_CxDstAddr */
#define DMA2D_CxLLI_OFFSET           (0x08) /* DMA2D_CxLLI */
#define DMA2D_Cx_BUS_OFFSET          (0x0C) /* DMA2D_Cx_BUS */
#define DMA2D_Cx_SRC_CNT_OFFSET      (0x10) /* DMA2D_Cx_SRC_CNT */
#define DMA2D_Cx_SRC_XIC_OFFSET      (0x14) /* DMA2D_Cx_SRC_XIC */
#define DMA2D_Cx_SRC_YIC_OFFSET      (0x18) /* DMA2D_Cx_SRC_YIC */
#define DMA2D_Cx_DST_CNT_OFFSET      (0x1C) /* DMA2D_Cx_DST_CNT */
#define DMA2D_Cx_DST_XIC_OFFSET      (0x20) /* DMA2D_Cx_DST_XIC */
#define DMA2D_Cx_DST_YIC_OFFSET      (0x24) /* DMA2D_Cx_DST_YIC */
#define DMA2D_Cx_KEY_OFFSET          (0x74) /* DMA2D_Cx_KEY */
#define DMA2D_Cx_KEY_EN_OFFSET       (0x78) /* DMA2D_Cx_KEY_EN */
#define DMA2D_Cx_CFG_OFFSET          (0x7C) /* DMA2D_Cx_CFG */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : DMA_IntStatus */
#define DMA2D_INTSTATUS_SHIFT        (0U)
#define DMA2D_INTSTATUS_MASK         (0xff << DMA2D_INTSTATUS_SHIFT)

/* 0x4 : DMA_IntTCStatus */
#define DMA2D_INTTCSTATUS_SHIFT      (0U)
#define DMA2D_INTTCSTATUS_MASK       (0xff << DMA2D_INTTCSTATUS_SHIFT)

/* 0x8 : DMA_IntTCClear */
#define DMA2D_INTTCCLEAR_SHIFT       (0U)
#define DMA2D_INTTCCLEAR_MASK        (0xff << DMA2D_INTTCCLEAR_SHIFT)

/* 0xC : DMA_EnbldChns */
#define DMA2D_ENABLEDCHANNELS_SHIFT  (0U)
#define DMA2D_ENABLEDCHANNELS_MASK   (0xff << DMA2D_ENABLEDCHANNELS_SHIFT)

/* 0x10 : DMA_Config */
#define DMA2D_E                      (1 << 0U)
#define DMA2D_CHCLKCG_SHIFT          (16U)
#define DMA2D_CHCLKCG_MASK           (0xff << DMA2D_CHCLKCG_SHIFT)

/* 0x14 : DMA_Sync */
#define DMA2D_DMA_SYNC_SHIFT         (0U)
#define DMA2D_DMA_SYNC_MASK          (0xffffffff << DMA2D_DMA_SYNC_SHIFT)

/* 0x20 : DMA_SoftBReq */
#define DMA2D_SOFTBREQ_SHIFT         (0U)
#define DMA2D_SOFTBREQ_MASK          (0xffffffff << DMA2D_SOFTBREQ_SHIFT)

/* 0x24 : DMA_SoftLBReq */
#define DMA2D_SOFTLBREQ_SHIFT        (0U)
#define DMA2D_SOFTLBREQ_MASK         (0xffffffff << DMA2D_SOFTLBREQ_SHIFT)

/* 0x28 : DMA_SoftSReq */
#define DMA2D_SOFTSREQ_SHIFT         (0U)
#define DMA2D_SOFTSREQ_MASK          (0xffffffff << DMA2D_SOFTSREQ_SHIFT)

/* 0x2C : DMA_SoftLSReq */
#define DMA2D_SOFTLSREQ_SHIFT        (0U)
#define DMA2D_SOFTLSREQ_MASK         (0xffffffff << DMA2D_SOFTLSREQ_SHIFT)

/* 0x38 : DMA_QOS */
#define DMA2D_DMA_CH_QOS_SHIFT       (0U)
#define DMA2D_DMA_CH_QOS_MASK        (0x3 << DMA2D_DMA_CH_QOS_SHIFT)

/* 0x00 : DMA2D_CxSrcAddr */
#define DMA2D_SRCADDR_SHIFT          (0U)
#define DMA2D_SRCADDR_MASK           (0xffffffff << DMA2D_SRCADDR_SHIFT)

/* 0x04 : DMA2D_CxDstAddr */
#define DMA2D_DSTADDR_SHIFT          (0U)
#define DMA2D_DSTADDR_MASK           (0xffffffff << DMA2D_DSTADDR_SHIFT)

/* 0x08 : DMA2D_CxLLI */
#define DMA2D_LLI_EN                 (1 << 0U)
#define DMA2D_LLI_ADDR_SHIFT         (2U)
#define DMA2D_LLI_ADDR_MASK          (0x3fffffff << DMA2D_LLI_ADDR_SHIFT)

/* 0x0C : DMA2D_Cx_BUS */
#define DMA2D_TRANSFERSIZE_SHIFT     (0U)
#define DMA2D_TRANSFERSIZE_MASK      (0xfff << DMA2D_TRANSFERSIZE_SHIFT)
#define DMA2D_SRC_BURST_SHIFT        (12U)
#define DMA2D_SRC_BURST_MASK         (0x3 << DMA2D_SRC_BURST_SHIFT)
#define DMA2D_DST_BURST_SHIFT        (15U)
#define DMA2D_DST_BURST_MASK         (0x3 << DMA2D_DST_BURST_SHIFT)
#define DMA2D_SRC_SIZE_SHIFT         (18U)
#define DMA2D_SRC_SIZE_MASK          (0x3 << DMA2D_SRC_SIZE_SHIFT)
#define DMA2D_DST_SIZE_SHIFT         (21U)
#define DMA2D_DST_SIZE_MASK          (0x3 << DMA2D_DST_SIZE_SHIFT)
#define DMA2D_SI                     (1 << 26U)
#define DMA2D_DI                     (1 << 27U)
#define DMA2D_PROT_SHIFT             (28U)
#define DMA2D_PROT_MASK              (0x7 << DMA2D_PROT_SHIFT)
#define DMA2D_I                      (1 << 31U)

/* 0x10 : DMA2D_Cx_SRC_CNT */
#define DMA2D_SRC_X_CNT_SHIFT        (0U)
#define DMA2D_SRC_X_CNT_MASK         (0xffff << DMA2D_SRC_X_CNT_SHIFT)
#define DMA2D_SRC_Y_CNT_SHIFT        (16U)
#define DMA2D_SRC_Y_CNT_MASK         (0xffff << DMA2D_SRC_Y_CNT_SHIFT)

/* 0x14 : DMA2D_Cx_SRC_XIC */
#define DMA2D_SRC_X_INCR_SHIFT       (0U)
#define DMA2D_SRC_X_INCR_MASK        (0xffffffff << DMA2D_SRC_X_INCR_SHIFT)

/* 0x18 : DMA2D_Cx_SRC_YIC */
#define DMA2D_SRC_Y_INCR_SHIFT       (0U)
#define DMA2D_SRC_Y_INCR_MASK        (0xffffffff << DMA2D_SRC_Y_INCR_SHIFT)

/* 0x1C : DMA2D_Cx_DST_CNT */
#define DMA2D_DST_X_CNT_SHIFT        (0U)
#define DMA2D_DST_X_CNT_MASK         (0xffff << DMA2D_DST_X_CNT_SHIFT)

/* 0x20 : DMA2D_Cx_DST_XIC */
#define DMA2D_DST_X_INCR_SHIFT       (0U)
#define DMA2D_DST_X_INCR_MASK        (0xffffffff << DMA2D_DST_X_INCR_SHIFT)

/* 0x24 : DMA2D_Cx_DST_YIC */
#define DMA2D_DST_Y_INCR_SHIFT       (0U)
#define DMA2D_DST_Y_INCR_MASK        (0xffffffff << DMA2D_DST_Y_INCR_SHIFT)

/* 0x74 : DMA2D_Cx_KEY */
#define DMA2D_KEY0_SHIFT             (0U)
#define DMA2D_KEY0_MASK              (0xff << DMA2D_KEY0_SHIFT)
#define DMA2D_KEY1_SHIFT             (8U)
#define DMA2D_KEY1_MASK              (0xff << DMA2D_KEY1_SHIFT)
#define DMA2D_KEY2_SHIFT             (16U)
#define DMA2D_KEY2_MASK              (0xff << DMA2D_KEY2_SHIFT)
#define DMA2D_KEY3_SHIFT             (24U)
#define DMA2D_KEY3_MASK              (0xff << DMA2D_KEY3_SHIFT)

/* 0x78 : DMA2D_Cx_KEY_EN */
#define DMA2D_KEY_EN                 (1 << 0U)
#define DMA2D_KEY_MODE_SHIFT         (1U)
#define DMA2D_KEY_MODE_MASK          (0x3 << DMA2D_KEY_MODE_SHIFT)
#define DMA2D_KEY_STRB_SHIFT         (4U)
#define DMA2D_KEY_STRB_MASK          (0xf << DMA2D_KEY_STRB_SHIFT)

/* 0x7C : DMA2D_Cx_CFG */
#define DMA2D_CH_EN                  (1 << 0U)
#define DMA2D_AHB_B2B                (1 << 14U)
#define DMA2D_FIFO_EMPTY             (1 << 16U)
#define DMA2D_LLICOUNTER_SHIFT       (20U)
#define DMA2D_LLICOUNTER_MASK        (0x3ff << DMA2D_LLICOUNTER_SHIFT)
#define DMA2D_REG_DMA_2D_EN          (1 << 31U)

#endif /* __DMA2D_REG_H__ */
