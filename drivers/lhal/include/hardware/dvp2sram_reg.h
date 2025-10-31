/**
  ******************************************************************************
  * @file    dvp2sram_reg.h
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
#ifndef  __DVP2SRAM_REG_H__
#define  __DVP2SRAM_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define DVP2SRAM_D2S_CONFIGUE_OFFSET                            (0x0)/* d2s_configue */
#define DVP2SRAM_D2S_SRAM_ADDR_OFFSET                           (0x4)/* d2s_sram_addr */
#define DVP2SRAM_D2S_MEM_BCNT_OFFSET                            (0x8)/* d2s_mem_bcnt */
#define DVP2SRAM_D2S_STATUS_OFFSET                              (0xC)/* d2s_status */
#define DVP2SRAM_D2S_FRAME_BCNT_OFFSET                          (0x10)/* d2s_frame_bcnt */
#define DVP2SRAM_D2S_FRAME_FIFO_POP_OFFSET                      (0x14)/* d2s_frame_fifo_pop */
#define DVP2SRAM_D2S_FRAME_VLD_OFFSET                           (0x18)/* d2s_frame_vld */
#define DVP2SRAM_D2S_FRAME_PERIOD_OFFSET                        (0x1C)/* d2s_frame_period */
#define DVP2SRAM_D2S_W1P_REG_OFFSET                             (0x20)/* d2s_w1p_reg */
#define DVP2SRAM_D2S_DMY_CFG0_OFFSET                            (0x24)/* d2s_dmy_cfg0 */
#define DVP2SRAM_D2S_HSYNC_CROP_OFFSET                          (0x30)/* d2s_hsync_crop */
#define DVP2SRAM_D2S_VSYNC_CROP_OFFSET                          (0x34)/* d2s_vsync_crop */
#define DVP2SRAM_D2S_FRAM_EXM_OFFSET                            (0x38)/* d2s_fram_exm */
#define DVP2SRAM_D2S_DUMMY_REG_OFFSET                           (0xFC)/* d2s_dummy_reg */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : d2s_configue */
#define DVP2SRAM_REG_DVP_ENABLE                                 (1<<0U)
#define DVP2SRAM_REG_FRAM_VLD_POL                               (1<<2U)
#define DVP2SRAM_REG_LINE_VLD_POL                               (1<<3U)
#define DVP2SRAM_REG_DVP_MODE_SHIFT                             (8U)
#define DVP2SRAM_REG_DVP_MODE_MASK                              (0x7<<DVP2SRAM_REG_DVP_MODE_SHIFT)
#define DVP2SRAM_REG_DROP_EN                                    (1<<12U)
#define DVP2SRAM_REG_DROP_EVEN                                  (1<<13U)
#define DVP2SRAM_REG_DVP_DATA_BSEL                              (1<<19U)
#define DVP2SRAM_REG_V_SUBSAMPLE_EN                             (1<<22U)
#define DVP2SRAM_REG_V_SUBSAMPLE_POL                            (1<<23U)
#define DVP2SRAM_REG_DVP_WAIT_CYCLE_SHIFT                       (24U)
#define DVP2SRAM_REG_DVP_WAIT_CYCLE_MASK                        (0xff<<DVP2SRAM_REG_DVP_WAIT_CYCLE_SHIFT)

/* 0x4 : d2s_sram_addr */
#define DVP2SRAM_REG_SRAM_ADDR_FSTART_SHIFT                     (0U)
#define DVP2SRAM_REG_SRAM_ADDR_FSTART_MASK                      (0xffff<<DVP2SRAM_REG_SRAM_ADDR_FSTART_SHIFT)
#define DVP2SRAM_REG_SRAM_ADDR_FEND_SHIFT                       (16U)
#define DVP2SRAM_REG_SRAM_ADDR_FEND_MASK                        (0xffff<<DVP2SRAM_REG_SRAM_ADDR_FEND_SHIFT)

/* 0x8 : d2s_mem_bcnt */
#define DVP2SRAM_REG_LINE_BYTE_CNT_SHIFT                        (0U)
#define DVP2SRAM_REG_LINE_BYTE_CNT_MASK                         (0xfff<<DVP2SRAM_REG_LINE_BYTE_CNT_SHIFT)
#define DVP2SRAM_REG_FRAM_LINE_CNT_SHIFT                        (16U)
#define DVP2SRAM_REG_FRAM_LINE_CNT_MASK                         (0xfff<<DVP2SRAM_REG_FRAM_LINE_CNT_SHIFT)

/* 0xC : d2s_status */
#define DVP2SRAM_REG_INT_HCNT_EN                                (1<<6U)
#define DVP2SRAM_REG_INT_VCNT_EN                                (1<<7U)
#define DVP2SRAM_STS_HCNT_INT                                   (1<<21U)
#define DVP2SRAM_STS_VCNT_INT                                   (1<<22U)
#define DVP2SRAM_ST_BUS_IDLE                                    (1<<24U)
#define DVP2SRAM_ST_BUS_FUNC                                    (1<<25U)
#define DVP2SRAM_ST_BUS_WAIT                                    (1<<26U)
#define DVP2SRAM_ST_BUS_FLSH                                    (1<<27U)
#define DVP2SRAM_ST_DVP_IDLE                                    (1<<29U)

/* 0x10 : d2s_frame_bcnt */
#define DVP2SRAM_REG_FRAME_BYTE_CNT_SHIFT                       (0U)
#define DVP2SRAM_REG_FRAME_BYTE_CNT_MASK                        (0xffffffff<<DVP2SRAM_REG_FRAME_BYTE_CNT_SHIFT)

/* 0x14 : d2s_frame_fifo_pop */
#define DVP2SRAM_REG_EXP_BYTE_CNT_SHIFT                         (0U)
#define DVP2SRAM_REG_EXP_BYTE_CNT_MASK                          (0x1ff<<DVP2SRAM_REG_EXP_BYTE_CNT_SHIFT)

/* 0x18 : d2s_frame_vld */
#define DVP2SRAM_REG_FRAME_N_VLD_SHIFT                          (0U)
#define DVP2SRAM_REG_FRAME_N_VLD_MASK                           (0xffffffff<<DVP2SRAM_REG_FRAME_N_VLD_SHIFT)

/* 0x1C : d2s_frame_period */
#define DVP2SRAM_REG_FRAME_PERIOD_SHIFT                         (0U)
#define DVP2SRAM_REG_FRAME_PERIOD_MASK                          (0x1f<<DVP2SRAM_REG_FRAME_PERIOD_SHIFT)

/* 0x20 : d2s_w1p_reg */
#define DVP2SRAM_REG_INT_HCNT_CLR                               (1<<8U)
#define DVP2SRAM_REG_INT_VCNT_CLR                               (1<<9U)
#define DVP2SRAM_CR_SW_SHD                                      (1<<16U)

/* 0x24 : d2s_dmy_cfg0 */
#define DVP2SRAM_REG_LEFT_BLK_DMY                               (1<<0U)
#define DVP2SRAM_REG_RIGHT_BLK_DMY                              (1<<1U)
#define DVP2SRAM_REG_UP_BLK_DMY                                 (1<<2U)
#define DVP2SRAM_REG_DOWN_BLK_DMY                               (1<<3U)
#define DVP2SRAM_REG_VERTICAL_DMY8                              (1<<4U)

/* 0x30 : d2s_hsync_crop */
#define DVP2SRAM_REG_HSYNC_ACT_END_SHIFT                        (0U)
#define DVP2SRAM_REG_HSYNC_ACT_END_MASK                         (0xffff<<DVP2SRAM_REG_HSYNC_ACT_END_SHIFT)
#define DVP2SRAM_REG_HSYNC_ACT_START_SHIFT                      (16U)
#define DVP2SRAM_REG_HSYNC_ACT_START_MASK                       (0xffff<<DVP2SRAM_REG_HSYNC_ACT_START_SHIFT)

/* 0x34 : d2s_vsync_crop */
#define DVP2SRAM_REG_VSYNC_ACT_END_SHIFT                        (0U)
#define DVP2SRAM_REG_VSYNC_ACT_END_MASK                         (0xffff<<DVP2SRAM_REG_VSYNC_ACT_END_SHIFT)
#define DVP2SRAM_REG_VSYNC_ACT_START_SHIFT                      (16U)
#define DVP2SRAM_REG_VSYNC_ACT_START_MASK                       (0xffff<<DVP2SRAM_REG_VSYNC_ACT_START_SHIFT)

/* 0x38 : d2s_fram_exm */
#define DVP2SRAM_REG_TOTAL_HCNT_SHIFT                           (0U)
#define DVP2SRAM_REG_TOTAL_HCNT_MASK                            (0xffff<<DVP2SRAM_REG_TOTAL_HCNT_SHIFT)
#define DVP2SRAM_REG_TOTAL_VCNT_SHIFT                           (16U)
#define DVP2SRAM_REG_TOTAL_VCNT_MASK                            (0xffff<<DVP2SRAM_REG_TOTAL_VCNT_SHIFT)

/* 0xFC : d2s_dummy_reg */


#endif  /* __DVP2SRAM_REG_H__ */
