/**
  ******************************************************************************
  * @file    dvp_tsrc_reg.h
  * @version V1.0
  * @date    2025-05-13
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
#ifndef __DVP_TSRC_REG_H__
#define __DVP_TSRC_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define DVP_TSRC_CONFIG_OFFSET                 (0x0)  /* config */
#define DVP_TSRC_FRAME_SIZE_H_OFFSET           (0x4)  /* frame_size_h */
#define DVP_TSRC_FRAME_SIZE_V_OFFSET           (0x8)  /* frame_size_v */
#define DVP_TSRC_FRAME_SIZE_CEA_861_OFFSET     (0xC)  /* frame_size_cea_861 */
#define DVP_TSRC_PIX_DATA_RANGE_OFFSET         (0x10) /* pix_data_range */
#define DVP_TSRC_PIX_DATA_STEP_OFFSET          (0x14) /* pix_data_step */
#define DVP_TSRC_AXI_PUSH_MODE_OFFSET          (0x18) /* axi_push_mode */
#define DVP_TSRC_AXI2DVP_SETTING_OFFSET        (0x20) /* axi2dvp_setting */
#define DVP_TSRC_AXI2DVP_START_ADDR_BY_OFFSET  (0x24) /* axi2dvp_start_addr_by */
#define DVP_TSRC_AXI2DVP_FRAME_INFO1_OFFSET    (0x28) /* axi2dvp_frame_info1 */
#define DVP_TSRC_AXI2DVP_SWAP_ADDR_BY_OFFSET   (0x30) /* axi2dvp_swap_addr_by */
#define DVP_TSRC_AXI2DVP_PREFETCH_OFFSET       (0x34) /* axi2dvp_prefetch */
#define DVP_TSRC_AXI2DVP_FRAME_INFO2_OFFSET    (0x38) /* axi2dvp_frame_info2 */
#define DVP_TSRC_AXI2DVP_START_ADDR_UV_OFFSET  (0x40) /* axi2dvp_start_addr_uv */
#define DVP_TSRC_AXI2DVP_SWAP_ADDR_UV_OFFSET   (0x44) /* axi2dvp_swap_addr_uv */
#define DVP_TSRC_AXI2DVP_STATUS_OFFSET         (0x50) /* axi2dvp_status */
#define DVP_TSRC_DTSRC_POST_TG0_OFFSET         (0x80) /* dtsrc_post_tg0 */
#define DVP_TSRC_DTSRC_POST_TG1_OFFSET         (0x84) /* dtsrc_post_tg1 */
#define DVP_TSRC_DTSRC_POST_TG2_OFFSET         (0x88) /* dtsrc_post_tg2 */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : config */
#define DVP_TSRC_CR_ENABLE                     (1 << 0U)
#define DVP_TSRC_CR_AXI_EN                     (1 << 1U)
#define DVP_TSRC_CR_MODE_CEA_861               (1 << 2U)
#define DVP_TSRC_CR_AXI_PUSH_MODE              (1 << 3U)
#define DVP_TSRC_CR_AXI_SWAP_MODE              (1 << 7U)
#define DVP_TSRC_CR_AXI_SWAP_IDX_SEL_SHIFT     (8U)
#define DVP_TSRC_CR_AXI_SWAP_IDX_SEL_MASK      (0xf << DVP_TSRC_CR_AXI_SWAP_IDX_SEL_SHIFT)
#define DVP_TSRC_CR_AXI_SWAP_IDX_SWM           (1 << 12U)
#define DVP_TSRC_CR_AXI_SWAP_IDX_SWV           (1 << 13U)
#define DVP_TSRC_CR_AXI_DVP_DATA_MODE_SHIFT    (16U)
#define DVP_TSRC_CR_AXI_DVP_DATA_MODE_MASK     (0x7 << DVP_TSRC_CR_AXI_DVP_DATA_MODE_SHIFT)
#define DVP_TSRC_CR_AXI_B0_SEL_SHIFT           (20U)
#define DVP_TSRC_CR_AXI_B0_SEL_MASK            (0x3 << DVP_TSRC_CR_AXI_B0_SEL_SHIFT)
#define DVP_TSRC_CR_AXI_B1_SEL_SHIFT           (22U)
#define DVP_TSRC_CR_AXI_B1_SEL_MASK            (0x3 << DVP_TSRC_CR_AXI_B1_SEL_SHIFT)
#define DVP_TSRC_CR_AXI_B2_SEL_SHIFT           (24U)
#define DVP_TSRC_CR_AXI_B2_SEL_MASK            (0x3 << DVP_TSRC_CR_AXI_B2_SEL_SHIFT)

/* 0x4 : frame_size_h */
#define DVP_TSRC_CR_TOTAL_H_SHIFT              (0U)
#define DVP_TSRC_CR_TOTAL_H_MASK               (0xffff << DVP_TSRC_CR_TOTAL_H_SHIFT)
#define DVP_TSRC_CR_BLANK_H_SHIFT              (16U)
#define DVP_TSRC_CR_BLANK_H_MASK               (0xffff << DVP_TSRC_CR_BLANK_H_SHIFT)

/* 0x8 : frame_size_v */
#define DVP_TSRC_CR_TOTAL_V_SHIFT              (0U)
#define DVP_TSRC_CR_TOTAL_V_MASK               (0xffff << DVP_TSRC_CR_TOTAL_V_SHIFT)
#define DVP_TSRC_CR_BLANK_V_SHIFT              (16U)
#define DVP_TSRC_CR_BLANK_V_MASK               (0xffff << DVP_TSRC_CR_BLANK_V_SHIFT)

/* 0xC : frame_size_cea_861 */
#define DVP_TSRC_CR_H_DURATION_SHIFT           (0U)
#define DVP_TSRC_CR_H_DURATION_MASK            (0xff << DVP_TSRC_CR_H_DURATION_SHIFT)
#define DVP_TSRC_CR_H_PLACEMENT_SHIFT          (8U)
#define DVP_TSRC_CR_H_PLACEMENT_MASK           (0xff << DVP_TSRC_CR_H_PLACEMENT_SHIFT)
#define DVP_TSRC_CR_V_DURATION_SHIFT           (16U)
#define DVP_TSRC_CR_V_DURATION_MASK            (0xff << DVP_TSRC_CR_V_DURATION_SHIFT)
#define DVP_TSRC_CR_V_PLACEMENT_SHIFT          (24U)
#define DVP_TSRC_CR_V_PLACEMENT_MASK           (0xff << DVP_TSRC_CR_V_PLACEMENT_SHIFT)

/* 0x10 : pix_data_range */
#define DVP_TSRC_CR_DATA_MIN_SHIFT             (0U)
#define DVP_TSRC_CR_DATA_MIN_MASK              (0xffff << DVP_TSRC_CR_DATA_MIN_SHIFT)
#define DVP_TSRC_CR_DATA_MAX_SHIFT             (16U)
#define DVP_TSRC_CR_DATA_MAX_MASK              (0xffff << DVP_TSRC_CR_DATA_MAX_SHIFT)

/* 0x14 : pix_data_step */
#define DVP_TSRC_CR_DATA_STEP_SHIFT            (0U)
#define DVP_TSRC_CR_DATA_STEP_MASK             (0xff << DVP_TSRC_CR_DATA_STEP_SHIFT)

/* 0x18 : axi_push_mode */
#define DVP_TSRC_CR_SW_SHD                     (1 << 0U)
#define DVP_TSRC_CR_AXI_FRAME_PUSH             (1 << 3U)
#define DVP_TSRC_CR_AXI_DRAIN_ERR_CLR          (1 << 4U)
#define DVP_TSRC_ST_AXI_FRAME_REMAIN_CNT_SHIFT (8U)
#define DVP_TSRC_ST_AXI_FRAME_REMAIN_CNT_MASK  (0x1f << DVP_TSRC_ST_AXI_FRAME_REMAIN_CNT_SHIFT)

/* 0x20 : axi2dvp_setting */
#define DVP_TSRC_CR_AXI_XLEN_SHIFT             (0U)
#define DVP_TSRC_CR_AXI_XLEN_MASK              (0x7 << DVP_TSRC_CR_AXI_XLEN_SHIFT)
#define DVP_TSRC_CR_AXI_422_SP_MODE            (1 << 7U)
#define DVP_TSRC_CR_AXI_420_SP_MODE            (1 << 8U)
#define DVP_TSRC_CR_AXI_420_UD_SEL             (1 << 9U)
#define DVP_TSRC_CR_QOS_SW_MODE                (1 << 10U)
#define DVP_TSRC_CR_QOS_SW                     (1 << 11U)

/* 0x24 : axi2dvp_start_addr_by */
#define DVP_TSRC_CR_AXI_START_ADDR_BY_SHIFT    (0U)
#define DVP_TSRC_CR_AXI_START_ADDR_BY_MASK     (0xffffffff << DVP_TSRC_CR_AXI_START_ADDR_BY_SHIFT)

/* 0x28 : axi2dvp_frame_info1 */
#define DVP_TSRC_CR_FRAME_WIDTH_X8_SHIFT       (0U)
#define DVP_TSRC_CR_FRAME_WIDTH_X8_MASK        (0x3fff << DVP_TSRC_CR_FRAME_WIDTH_X8_SHIFT)
#define DVP_TSRC_CR_FRAME_HEIGHT_SHIFT         (16U)
#define DVP_TSRC_CR_FRAME_HEIGHT_MASK          (0x3fff << DVP_TSRC_CR_FRAME_HEIGHT_SHIFT)

/* 0x30 : axi2dvp_swap_addr_by */
#define DVP_TSRC_CR_AXI_SWAP_ADDR_BY_SHIFT     (0U)
#define DVP_TSRC_CR_AXI_SWAP_ADDR_BY_MASK      (0xffffffff << DVP_TSRC_CR_AXI_SWAP_ADDR_BY_SHIFT)

/* 0x34 : axi2dvp_prefetch */
#define DVP_TSRC_CR_PREFETCH_V_SHIFT           (0U)
#define DVP_TSRC_CR_PREFETCH_V_MASK            (0xffff << DVP_TSRC_CR_PREFETCH_V_SHIFT)

/* 0x38 : axi2dvp_frame_info2 */
#define DVP_TSRC_CR_STRIDE_WIDTH_X8_SHIFT      (0U)
#define DVP_TSRC_CR_STRIDE_WIDTH_X8_MASK       (0x3fff << DVP_TSRC_CR_STRIDE_WIDTH_X8_SHIFT)

/* 0x40 : axi2dvp_start_addr_uv */
#define DVP_TSRC_CR_AXI_ADDR_START_UV_SHIFT    (0U)
#define DVP_TSRC_CR_AXI_ADDR_START_UV_MASK     (0xffffffff << DVP_TSRC_CR_AXI_ADDR_START_UV_SHIFT)

/* 0x44 : axi2dvp_swap_addr_uv */
#define DVP_TSRC_CR_AXI_ADDR_SWAP_UV_SHIFT     (0U)
#define DVP_TSRC_CR_AXI_ADDR_SWAP_UV_MASK      (0xffffffff << DVP_TSRC_CR_AXI_ADDR_SWAP_UV_SHIFT)

/* 0x50 : axi2dvp_status */
#define DVP_TSRC_ST_AXI_DRAIN_ERROR_BY         (1 << 0U)
#define DVP_TSRC_ST_AXI_DRAIN_ERROR_UV         (1 << 1U)
#define DVP_TSRC_ST_AXI_STATE_IDLE_BY          (1 << 8U)
#define DVP_TSRC_ST_AXI_STATE_WTST_BY          (1 << 9U)
#define DVP_TSRC_ST_AXI_STATE_FUNC_BY          (1 << 10U)
#define DVP_TSRC_ST_AXI_STATE_FLSH_BY          (1 << 11U)
#define DVP_TSRC_ST_AXI_STATE_IDLE_UV          (1 << 12U)
#define DVP_TSRC_ST_AXI_STATE_WTST_UV          (1 << 13U)
#define DVP_TSRC_ST_AXI_STATE_FUNC_UV          (1 << 14U)
#define DVP_TSRC_ST_AXI_STATE_FLSH_UV          (1 << 15U)

/* 0x80 : dtsrc_post_tg0 */
#define DVP_TSRC_CR_TOTAL_HPIXELS_SHIFT        (0U)
#define DVP_TSRC_CR_TOTAL_HPIXELS_MASK         (0xffff << DVP_TSRC_CR_TOTAL_HPIXELS_SHIFT)
#define DVP_TSRC_CR_TG_EN                      (1 << 16U)

/* 0x84 : dtsrc_post_tg1 */
#define DVP_TSRC_CR_ACT_PIXEL_WIDTH_SHIFT      (0U)
#define DVP_TSRC_CR_ACT_PIXEL_WIDTH_MASK       (0xffff << DVP_TSRC_CR_ACT_PIXEL_WIDTH_SHIFT)
#define DVP_TSRC_CR_ACT_START_PIXEL_SHIFT      (16U)
#define DVP_TSRC_CR_ACT_START_PIXEL_MASK       (0xffff << DVP_TSRC_CR_ACT_START_PIXEL_SHIFT)

/* 0x88 : dtsrc_post_tg2 */
#define DVP_TSRC_CR_ACT_LINE_HEIGHT_SHIFT      (0U)
#define DVP_TSRC_CR_ACT_LINE_HEIGHT_MASK       (0xffff << DVP_TSRC_CR_ACT_LINE_HEIGHT_SHIFT)
#define DVP_TSRC_CR_ACT_START_LINE_SHIFT       (16U)
#define DVP_TSRC_CR_ACT_START_LINE_MASK        (0xffff << DVP_TSRC_CR_ACT_START_LINE_SHIFT)

#endif /* __DVP_TSRC_REG_H__ */
