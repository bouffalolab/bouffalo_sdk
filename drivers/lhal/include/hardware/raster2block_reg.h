/**
  ******************************************************************************
  * @file    raster2block_reg.h
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
#ifndef  __RASTER2BLOCK_REG_H__
#define  __RASTER2BLOCK_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define RASTER2BLOCK_R2B_CFG0_OFFSET                            (0xC00)/* r2b_cfg0 */
#define RASTER2BLOCK_R2B_CFG1_OFFSET                            (0xC04)/* r2b_cfg1 */
#define RASTER2BLOCK_R2B_NENC_CFG0_OFFSET                       (0xC08)/* r2b_nenc_cfg0 */
#define RASTER2BLOCK_R2B_NENC_CFG1_OFFSET                       (0xC0C)/* r2b_nenc_cfg1 */
#define RASTER2BLOCK_R2B_NENC_CFG2_OFFSET                       (0xC10)/* r2b_nenc_cfg2 */
#define RASTER2BLOCK_R2B_NENC_CFG5_OFFSET                       (0xC1C)/* r2b_nenc_cfg5 */
#define RASTER2BLOCK_R2B_NENC_CFG6_OFFSET                       (0xC20)/* r2b_nenc_cfg6 */

/* Register Bitfield definitions *****************************************************/

/* 0xC00 : r2b_cfg0 */
#define RASTER2BLOCK_REG_R2B_EN                                 (1<<0U)
#define RASTER2BLOCK_REG_R2B_ENC_MODE                           (1<<1U)
#define RASTER2BLOCK_REG_BLOCK_UP_DMY                           (1<<2U)
#define RASTER2BLOCK_REG_BLOCK_DW_DMY                           (1<<3U)
#define RASTER2BLOCK_REG_MIRROR_EN                              (1<<4U)
#define RASTER2BLOCK_REG_FLIP_EN                                (1<<5U)
#define RASTER2BLOCK_REG_FRAME_MODE_SHIFT                       (6U)
#define RASTER2BLOCK_REG_FRAME_MODE_MASK                        (0x3<<RASTER2BLOCK_REG_FRAME_MODE_SHIFT)
#define RASTER2BLOCK_REG_C_BLOCK_ORDER                          (1<<8U)
#define RASTER2BLOCK_REG_AW_IDLE_CYCLE_SHIFT                    (16U)
#define RASTER2BLOCK_REG_AW_IDLE_CYCLE_MASK                     (0xff<<RASTER2BLOCK_REG_AW_IDLE_CYCLE_SHIFT)
#define RASTER2BLOCK_REG_OSTD_MAX_SHIFT                         (24U)
#define RASTER2BLOCK_REG_OSTD_MAX_MASK                          (0xff<<RASTER2BLOCK_REG_OSTD_MAX_SHIFT)

/* 0xC04 : r2b_cfg1 */
#define RASTER2BLOCK_REG_FRAME_WBLK_SHIFT                       (0U)
#define RASTER2BLOCK_REG_FRAME_WBLK_MASK                        (0xfff<<RASTER2BLOCK_REG_FRAME_WBLK_SHIFT)
#define RASTER2BLOCK_REG_FRAME_HBLK_SHIFT                       (16U)
#define RASTER2BLOCK_REG_FRAME_HBLK_MASK                        (0xfff<<RASTER2BLOCK_REG_FRAME_HBLK_SHIFT)

/* 0xC08 : r2b_nenc_cfg0 */
#define RASTER2BLOCK_REG_Y_ADDR_START_SHIFT                     (0U)
#define RASTER2BLOCK_REG_Y_ADDR_START_MASK                      (0xffffffff<<RASTER2BLOCK_REG_Y_ADDR_START_SHIFT)

/* 0xC0C : r2b_nenc_cfg1 */
#define RASTER2BLOCK_REG_C_ADDR_START_SHIFT                     (0U)
#define RASTER2BLOCK_REG_C_ADDR_START_MASK                      (0xffffffff<<RASTER2BLOCK_REG_C_ADDR_START_SHIFT)

/* 0xC10 : r2b_nenc_cfg2 */
#define RASTER2BLOCK_REG_Y_MEM_FRAME_CNT_SHIFT                  (0U)
#define RASTER2BLOCK_REG_Y_MEM_FRAME_CNT_MASK                   (0xff<<RASTER2BLOCK_REG_Y_MEM_FRAME_CNT_SHIFT)
#define RASTER2BLOCK_REG_C_MEM_FRAME_CNT_SHIFT                  (8U)
#define RASTER2BLOCK_REG_C_MEM_FRAME_CNT_MASK                   (0xff<<RASTER2BLOCK_REG_C_MEM_FRAME_CNT_SHIFT)

/* 0xC1C : r2b_nenc_cfg5 */
#define RASTER2BLOCK_REG_INT_FRAME_EN                           (1<<0U)
#define RASTER2BLOCK_REG_INT_SRAM_OV_EN                         (1<<1U)
#define RASTER2BLOCK_REG_ENC_INT_NORMAL_EN                      (1<<2U)
#define RASTER2BLOCK_REG_ENC_INT_MEM_EN                         (1<<3U)
#define RASTER2BLOCK_REG_ENC_INT_FRAME_EN                       (1<<4U)
#define RASTER2BLOCK_REG_ENC_INT_IDLE_EN                        (1<<5U)
#define RASTER2BLOCK_REG_ENC_INT_SWAP_EN                        (1<<6U)
#define RASTER2BLOCK_STS_FRAME_INT                              (1<<8U)
#define RASTER2BLOCK_STS_SRAM_OV_INT                            (1<<9U)
#define RASTER2BLOCK_STS_ENC_NORMAL_INT                         (1<<10U)
#define RASTER2BLOCK_STS_ENC_MEM_INT                            (1<<11U)
#define RASTER2BLOCK_STS_ENC_FRAME_INT                          (1<<12U)
#define RASTER2BLOCK_STS_ENC_IDLE_INT                           (1<<13U)
#define RASTER2BLOCK_STS_ENC_SWAP_INT                           (1<<14U)
#define RASTER2BLOCK_REG_FRAME_CNT_TRGR_INT_SHIFT               (16U)
#define RASTER2BLOCK_REG_FRAME_CNT_TRGR_INT_MASK                (0x1f<<RASTER2BLOCK_REG_FRAME_CNT_TRGR_INT_SHIFT)

/* 0xC20 : r2b_nenc_cfg6 */
#define RASTER2BLOCK_REG_INT_FRAME_CLR                          (1<<0U)
#define RASTER2BLOCK_REG_INT_SRAM_OV_CLR                        (1<<1U)
#define RASTER2BLOCK_REG_ENC_INT_NORMAL_CLR                     (1<<2U)
#define RASTER2BLOCK_REG_ENC_INT_MEM_CLR                        (1<<3U)
#define RASTER2BLOCK_REG_ENC_INT_FRAME_CLR                      (1<<4U)
#define RASTER2BLOCK_REG_ENC_INT_IDLE_CLR                       (1<<5U)
#define RASTER2BLOCK_REG_ENC_INT_SWAP_CLR                       (1<<6U)


#endif  /* __RASTER2BLOCK_REG_H__ */
