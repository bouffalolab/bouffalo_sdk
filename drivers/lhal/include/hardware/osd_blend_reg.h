/**
  ******************************************************************************
  * @file    osd_blend_reg.h
  * @version V1.0
  * @date    2024-12-13
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
#ifndef __OSD_BLEND_REG_H__
#define __OSD_BLEND_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define OSD_BLEND_OBND_LAYER_XCONFIG_OFFSET     (0x0)  /* obnd_layer_xconfig */
#define OSD_BLEND_OBND_LAYER_YCONFIG_OFFSET     (0x4)  /* obnd_layer_yconfig */
#define OSD_BLEND_OBND_MEM_CONFIG0_OFFSET       (0x8)  /* obnd_mem_config0 */
#define OSD_BLEND_OBND_LAYER_CONFIG0_OFFSET     (0x14) /* obnd_layer_config0 */
#define OSD_BLEND_OBND_LAYER_CONFIG1_OFFSET     (0x18) /* obnd_layer_config1 */
#define OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET     (0x1C) /* obnd_layer_config2 */
#define OSD_BLEND_OBND_LAYER_CONFIG3_OFFSET     (0x20) /* obnd_layer_config3 */
#define OSD_BLEND_OBND_LAYER_CONFIG4_OFFSET     (0x24) /* obnd_layer_config4 */
#define OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET     (0x28) /* obnd_layer_config5 */
#define OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET     (0x2C) /* obnd_layer_config6 */
#define OSD_BLEND_OBND_LAYER_CONFIG7_OFFSET     (0x30) /* obnd_layer_config7 */
#define OSD_BLEND_OBND_LAYER_CONFIG8_OFFSET     (0x34) /* obnd_layer_config8 */
#define OSD_BLEND_OBND_ERROR_OFFSET             (0x40) /* obnd_error */
#define OSD_BLEND_OBND_SH_OFFSET                (0x44) /* obnd_sh */
#define OSD_BLEND_OBND_MEM_CONFIG1_OFFSET       (0x60) /* obnd_mem_config1 */
#define OSD_BLEND_OBND_MEM_CONFIG2_OFFSET       (0x64) /* obnd_mem_config2 */
#define OSD_BLEND_OBND_MEM_CONFIG3_OFFSET       (0x68) /* obnd_mem_config3 */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : obnd_layer_xconfig */
#define OSD_BLEND_OBND_X_MIN_SHIFT              (0U)
#define OSD_BLEND_OBND_X_MIN_MASK               (0xfff << OSD_BLEND_OBND_X_MIN_SHIFT)
#define OSD_BLEND_OBND_X_MAX_SHIFT              (16U)
#define OSD_BLEND_OBND_X_MAX_MASK               (0xfff << OSD_BLEND_OBND_X_MAX_SHIFT)

/* 0x4 : obnd_layer_yconfig */
#define OSD_BLEND_OBND_Y_MIN_SHIFT              (0U)
#define OSD_BLEND_OBND_Y_MIN_MASK               (0xfff << OSD_BLEND_OBND_Y_MIN_SHIFT)
#define OSD_BLEND_OBND_Y_MAX_SHIFT              (16U)
#define OSD_BLEND_OBND_Y_MAX_MASK               (0xfff << OSD_BLEND_OBND_Y_MAX_SHIFT)

/* 0x8 : obnd_mem_config0 */
#define OSD_BLEND_OBND_FORCE_SH                 (1 << 0U)
#define OSD_BLEND_OBND_LAYER_EN                 (1 << 15U)

/* 0x14 : obnd_layer_config0 */
#define OSD_BLEND_OBND_COLOR_FORMAT_SHIFT       (0U)
#define OSD_BLEND_OBND_COLOR_FORMAT_MASK        (0x1f << OSD_BLEND_OBND_COLOR_FORMAT_SHIFT)
#define OSD_BLEND_OBND_ORDER_A_SHIFT            (8U)
#define OSD_BLEND_OBND_ORDER_A_MASK             (0x3 << OSD_BLEND_OBND_ORDER_A_SHIFT)
#define OSD_BLEND_OBND_ORDER_RV_SHIFT           (10U)
#define OSD_BLEND_OBND_ORDER_RV_MASK            (0x3 << OSD_BLEND_OBND_ORDER_RV_SHIFT)
#define OSD_BLEND_OBND_ORDER_GY_SHIFT           (12U)
#define OSD_BLEND_OBND_ORDER_GY_MASK            (0x3 << OSD_BLEND_OBND_ORDER_GY_SHIFT)
#define OSD_BLEND_OBND_ORDER_BU_SHIFT           (14U)
#define OSD_BLEND_OBND_ORDER_BU_MASK            (0x3 << OSD_BLEND_OBND_ORDER_BU_SHIFT)
#define OSD_BLEND_OBND_GLOBAL_A_EN              (1 << 16U)
#define OSD_BLEND_OBND_GLOBAL_A_SHIFT           (24U)
#define OSD_BLEND_OBND_GLOBAL_A_MASK            (0xff << OSD_BLEND_OBND_GLOBAL_A_SHIFT)

/* 0x18 : obnd_layer_config1 */
#define OSD_BLEND_OBND_GLOBAL_COLOR_EN          (1 << 0U)
#define OSD_BLEND_OBND_GLOBAL_RV_SHIFT          (8U)
#define OSD_BLEND_OBND_GLOBAL_RV_MASK           (0xff << OSD_BLEND_OBND_GLOBAL_RV_SHIFT)
#define OSD_BLEND_OBND_GLOBAL_GY_SHIFT          (16U)
#define OSD_BLEND_OBND_GLOBAL_GY_MASK           (0xff << OSD_BLEND_OBND_GLOBAL_GY_SHIFT)
#define OSD_BLEND_OBND_GLOBAL_BU_SHIFT          (24U)
#define OSD_BLEND_OBND_GLOBAL_BU_MASK           (0xff << OSD_BLEND_OBND_GLOBAL_BU_SHIFT)

/* 0x1C : obnd_layer_config2 */
#define OSD_BLEND_OBND_KEY_PALETTE_EN           (1 << 0U)
#define OSD_BLEND_OBND_KEY_INDEX_INV            (1 << 1U)
#define OSD_BLEND_OBND_KEY_INDEX_MIN_SHIFT      (8U)
#define OSD_BLEND_OBND_KEY_INDEX_MIN_MASK       (0xff << OSD_BLEND_OBND_KEY_INDEX_MIN_SHIFT)
#define OSD_BLEND_OBND_KEY_INDEX_MAX_SHIFT      (16U)
#define OSD_BLEND_OBND_KEY_INDEX_MAX_MASK       (0xff << OSD_BLEND_OBND_KEY_INDEX_MAX_SHIFT)
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX_SHIFT  (24U)
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX_MASK   (0xff << OSD_BLEND_OBND_KEY_REPLACE_INDEX_SHIFT)

/* 0x20 : obnd_layer_config3 */
#define OSD_BLEND_OBND_KEY_A_MIN_SHIFT          (0U)
#define OSD_BLEND_OBND_KEY_A_MIN_MASK           (0xff << OSD_BLEND_OBND_KEY_A_MIN_SHIFT)
#define OSD_BLEND_OBND_KEY_A_MAX_SHIFT          (8U)
#define OSD_BLEND_OBND_KEY_A_MAX_MASK           (0xff << OSD_BLEND_OBND_KEY_A_MAX_SHIFT)
#define OSD_BLEND_OBND_KEY_RV_MIN_SHIFT         (16U)
#define OSD_BLEND_OBND_KEY_RV_MIN_MASK          (0xff << OSD_BLEND_OBND_KEY_RV_MIN_SHIFT)
#define OSD_BLEND_OBND_KEY_RV_MAX_SHIFT         (24U)
#define OSD_BLEND_OBND_KEY_RV_MAX_MASK          (0xff << OSD_BLEND_OBND_KEY_RV_MAX_SHIFT)

/* 0x24 : obnd_layer_config4 */
#define OSD_BLEND_OBND_KEY_GY_MIN_SHIFT         (0U)
#define OSD_BLEND_OBND_KEY_GY_MIN_MASK          (0xff << OSD_BLEND_OBND_KEY_GY_MIN_SHIFT)
#define OSD_BLEND_OBND_KEY_GY_MAX_SHIFT         (8U)
#define OSD_BLEND_OBND_KEY_GY_MAX_MASK          (0xff << OSD_BLEND_OBND_KEY_GY_MAX_SHIFT)
#define OSD_BLEND_OBND_KEY_BU_MIN_SHIFT         (16U)
#define OSD_BLEND_OBND_KEY_BU_MIN_MASK          (0xff << OSD_BLEND_OBND_KEY_BU_MIN_SHIFT)
#define OSD_BLEND_OBND_KEY_BU_MAX_SHIFT         (24U)
#define OSD_BLEND_OBND_KEY_BU_MAX_MASK          (0xff << OSD_BLEND_OBND_KEY_BU_MAX_SHIFT)

/* 0x28 : obnd_layer_config5 */
#define OSD_BLEND_OBND_KEY_REPLACE_A_SHIFT      (0U)
#define OSD_BLEND_OBND_KEY_REPLACE_A_MASK       (0xff << OSD_BLEND_OBND_KEY_REPLACE_A_SHIFT)
#define OSD_BLEND_OBND_KEY_REPLACE_RV_SHIFT     (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_RV_MASK      (0xff << OSD_BLEND_OBND_KEY_REPLACE_RV_SHIFT)
#define OSD_BLEND_OBND_KEY_REPLACE_GY_SHIFT     (16U)
#define OSD_BLEND_OBND_KEY_REPLACE_GY_MASK      (0xff << OSD_BLEND_OBND_KEY_REPLACE_GY_SHIFT)
#define OSD_BLEND_OBND_KEY_REPLACE_BU_SHIFT     (24U)
#define OSD_BLEND_OBND_KEY_REPLACE_BU_MASK      (0xff << OSD_BLEND_OBND_KEY_REPLACE_BU_SHIFT)

/* 0x2C : obnd_layer_config6 */
#define OSD_BLEND_OBND_KEY_COLOR_EN             (1 << 0U)
#define OSD_BLEND_OBND_KEY_A_INV                (1 << 1U)
#define OSD_BLEND_OBND_KEY_RV_INV               (1 << 2U)
#define OSD_BLEND_OBND_KEY_GY_INV               (1 << 3U)
#define OSD_BLEND_OBND_KEY_BU_INV               (1 << 4U)
#define OSD_BLEND_OBND_UPDATE_TRIGGER           (1 << 5U)
#define OSD_BLEND_OBND_UPDATE_INDEX_SHIFT       (8U)
#define OSD_BLEND_OBND_UPDATE_INDEX_MASK        (0xff << OSD_BLEND_OBND_UPDATE_INDEX_SHIFT)

/* 0x30 : obnd_layer_config7 */
#define OSD_BLEND_OBND_UPDATE_COLOR_SHIFT       (0U)
#define OSD_BLEND_OBND_UPDATE_COLOR_MASK        (0xffffffff << OSD_BLEND_OBND_UPDATE_COLOR_SHIFT)

/* 0x34 : obnd_layer_config8 */
#define OSD_BLEND_OBND_ALPHA_0_SHIFT            (0U)
#define OSD_BLEND_OBND_ALPHA_0_MASK             (0xff << OSD_BLEND_OBND_ALPHA_0_SHIFT)
#define OSD_BLEND_OBND_ALPHA_1_SHIFT            (8U)
#define OSD_BLEND_OBND_ALPHA_1_MASK             (0xff << OSD_BLEND_OBND_ALPHA_1_SHIFT)

/* 0x40 : obnd_error */
#define OSD_BLEND_REG_RFIFO_DRAIN_CLR_W         (1 << 0U)
#define OSD_BLEND_REG_RFIFO_DRAIN_MASK_W        (1 << 1U)
#define OSD_BLEND_STS_RFIFO_DRAIN_R             (1 << 16U)

/* 0x44 : obnd_sh */
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_SHIFT  (0U)
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_MASK   (0xffffffff << OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_SHIFT)

/* 0x60 : obnd_mem_config1 */
#define OSD_BLEND_REG_ADDR_START_SHIFT          (0U)
#define OSD_BLEND_REG_ADDR_START_MASK           (0xffffffff << OSD_BLEND_REG_ADDR_START_SHIFT)

/* 0x64 : obnd_mem_config2 */
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_SHIFT (0U)
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_MASK  (0x3fff << OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_SHIFT)
#define OSD_BLEND_REG_STRIDE_BYTE_X8_SHIFT      (16U)
#define OSD_BLEND_REG_STRIDE_BYTE_X8_MASK       (0x3fff << OSD_BLEND_REG_STRIDE_BYTE_X8_SHIFT)

/* 0x68 : obnd_mem_config3 */
#define OSD_BLEND_REG_FRAME_HEIGHT_SHIFT        (0U)
#define OSD_BLEND_REG_FRAME_HEIGHT_MASK         (0x3fff << OSD_BLEND_REG_FRAME_HEIGHT_SHIFT)
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT_SHIFT   (16U)
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT_MASK    (0x3f << OSD_BLEND_REG_LINE_HEAD_FIX_BIT_SHIFT)
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT_SHIFT   (24U)
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT_MASK    (0x3f << OSD_BLEND_REG_LINE_TAIL_FIX_BIT_SHIFT)

#endif /* __OSD_BLEND_REG_H__ */
