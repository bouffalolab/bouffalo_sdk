/**
  ******************************************************************************
  * @file    mjdec_reg.h
  * @version V1.0
  * @date    2024-09-12
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
#ifndef  __HARDWARE_MJDEC_REG_H__
#define  __HARDWARE_MJDEC_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define MJDEC_JDEC_CONTROL_1_OFFSET                             (0x0)/* jdec_control_1 */
#define MJDEC_JDEC_YY_FRAME_ADDR_OFFSET                         (0x08)/* jdec_yy_frame_addr */
#define MJDEC_JDEC_UV_FRAME_ADDR_OFFSET                         (0x0C)/* jdec_uv_frame_addr */
#define MJDEC_JDEC_CONTROL_3_OFFSET                             (0x1C)/* jdec_control_3 */
#define MJDEC_JDEC_INT_CLR_OFFSET                               (0x20)/* jdec_int_clr */
#define MJDEC_JDEC_FRAM_PUSH_OFFSET                             (0x24)/* jdec_fram_push */
#define MJDEC_JDEC_FRAM_STS_OFFSET                              (0x28)/* jdec_fram_sts */
#define MJDEC_JDEC_FRAME_SIZE_OFFSET                            (0x2C)/* jdec_frame_size */
#define MJDEC_JDEC_HEADER_SKIP_OFFSET                           (0x30)/* jdec_header_skip */
#define MJDEC_JP_ADDR0_OFFSET                                   (0x40)/* jp_addr0 */
#define MJDEC_JP_ADDR1_OFFSET                                   (0x44)/* jp_addr1 */
#define MJDEC_JP_ADDR2_OFFSET                                   (0x48)/* jp_addr2 */
#define MJDEC_JP_ADDR3_OFFSET                                   (0x4C)/* jp_addr3 */
#define MJDEC_JP_ADDR4_OFFSET                                   (0x50)/* jp_addr4 */
#define MJDEC_JP_ADDR5_OFFSET                                   (0x54)/* jp_addr5 */
#define MJDEC_JP_ADDR6_OFFSET                                   (0x58)/* jp_addr6 */
#define MJDEC_JP_ADDR7_OFFSET                                   (0x5C)/* jp_addr7 */
#define MJDEC_JP_ADDR_8_OFFSET                                  (0x60)/* jp_addr_8 */
#define MJDEC_JP_ADDR_9_OFFSET                                  (0x64)/* jp_addr_9 */
#define MJDEC_JP_ADDR_A_OFFSET                                  (0x68)/* jp_addr_a */
#define MJDEC_JP_ADDR_B_OFFSET                                  (0x6C)/* jp_addr_b */
#define MJDEC_JP_ADDR_C_OFFSET                                  (0x70)/* jp_addr_c */
#define MJDEC_JP_ADDR_D_OFFSET                                  (0x74)/* jp_addr_d */
#define MJDEC_JP_ADDR_E_OFFSET                                  (0x78)/* jp_addr_e */
#define MJDEC_JP_ADDR_F_OFFSET                                  (0x7C)/* jp_addr_f */
#define MJDEC_YY_Q0_OFFSET                                      (0x80)/* mjdec_yy_q0 */
#define MJDEC_YY_Q1_OFFSET                                      (0x84)/* mjdec_yy_q1 */
#define MJDEC_YY_Q2_OFFSET                                      (0x88)/* mjdec_yy_q2 */
#define MJDEC_YY_Q3_OFFSET                                      (0x8C)/* mjdec_yy_q3 */
#define MJDEC_YY_Q4_OFFSET                                      (0x90)/* mjdec_yy_q4 */
#define MJDEC_YY_Q5_OFFSET                                      (0x94)/* mjdec_yy_q5 */
#define MJDEC_YY_Q6_OFFSET                                      (0x98)/* mjdec_yy_q6 */
#define MJDEC_YY_Q7_OFFSET                                      (0x9C)/* mjdec_yy_q7 */
#define MJDEC_YY_Q8_OFFSET                                      (0xA0)/* mjdec_yy_q8 */
#define MJDEC_YY_Q9_OFFSET                                      (0xA4)/* mjdec_yy_q9 */
#define MJDEC_YY_QA_OFFSET                                      (0xA8)/* mjdec_yy_qa */
#define MJDEC_YY_QB_OFFSET                                      (0xAC)/* mjdec_yy_qb */
#define MJDEC_YY_QC_OFFSET                                      (0xB0)/* mjdec_yy_qc */
#define MJDEC_YY_QD_OFFSET                                      (0xB4)/* mjdec_yy_qd */
#define MJDEC_YY_QE_OFFSET                                      (0xB8)/* mjdec_yy_qe */
#define MJDEC_YY_QF_OFFSET                                      (0xBC)/* mjdec_yy_qf */
#define MJDEC_UV_Q0_OFFSET                                      (0xC0)/* mjdec_uv_q0 */
#define MJDEC_UV_Q1_OFFSET                                      (0xC4)/* mjdec_uv_q1 */
#define MJDEC_UV_Q2_OFFSET                                      (0xC8)/* mjdec_uv_q2 */
#define MJDEC_UV_Q3_OFFSET                                      (0xCC)/* mjdec_uv_q3 */
#define MJDEC_UV_Q4_OFFSET                                      (0xD0)/* mjdec_uv_q4 */
#define MJDEC_UV_Q5_OFFSET                                      (0xD4)/* mjdec_uv_q5 */
#define MJDEC_UV_Q6_OFFSET                                      (0xD8)/* mjdec_uv_q6 */
#define MJDEC_UV_Q7_OFFSET                                      (0xDC)/* mjdec_uv_q7 */
#define MJDEC_UV_Q8_OFFSET                                      (0xE0)/* mjdec_uv_q8 */
#define MJDEC_UV_Q9_OFFSET                                      (0xE4)/* mjdec_uv_q9 */
#define MJDEC_UV_QA_OFFSET                                      (0xE8)/* mjdec_uv_qa */
#define MJDEC_UV_QB_OFFSET                                      (0xEC)/* mjdec_uv_qb */
#define MJDEC_UV_QC_OFFSET                                      (0xF0)/* mjdec_uv_qc */
#define MJDEC_UV_QD_OFFSET                                      (0xF4)/* mjdec_uv_qd */
#define MJDEC_UV_QE_OFFSET                                      (0xF8)/* mjdec_uv_qe */
#define MJDEC_UV_QF_OFFSET                                      (0xFC)/* mjdec_uv_qf */
#define MJDEC_YY_DC_CFG0_OFFSET                                 (0x100)/* mjdec_yy_dc_cfg0 */
#define MJDEC_YY_DC_CFG1_OFFSET                                 (0x104)/* mjdec_yy_dc_cfg1 */
#define MJDEC_YY_DC_CFG2_OFFSET                                 (0x108)/* mjdec_yy_dc_cfg2 */
#define MJDEC_YY_DC_CFG3_OFFSET                                 (0x10C)/* mjdec_yy_dc_cfg3 */
#define MJDEC_YY_AC_CFG0_OFFSET                                 (0x120)/* mjdec_yy_ac_cfg0 */
#define MJDEC_YY_AC_CFG1_OFFSET                                 (0x124)/* mjdec_yy_ac_cfg1 */
#define MJDEC_YY_AC_CFG2_OFFSET                                 (0x128)/* mjdec_yy_ac_cfg2 */
#define MJDEC_YY_AC_CFG3_OFFSET                                 (0x12C)/* mjdec_yy_ac_cfg3 */
#define MJDEC_YY_AC_CFG4_OFFSET                                 (0x130)/* mjdec_yy_ac_cfg4 */
#define MJDEC_YY_AC_CFG5_OFFSET                                 (0x134)/* mjdec_yy_ac_cfg5 */
#define MJDEC_YY_AC_CFG6_OFFSET                                 (0x138)/* mjdec_yy_ac_cfg6 */
#define MJDEC_YY_AC_CFG7_OFFSET                                 (0x13C)/* mjdec_yy_ac_cfg7 */
#define MJDEC_YY_AC_CFG8_OFFSET                                 (0x140)/* mjdec_yy_ac_cfg8 */
#define MJDEC_YY_AC_CFG9_OFFSET                                 (0x144)/* mjdec_yy_ac_cfg9 */
#define MJDEC_YY_AC_CFGA_OFFSET                                 (0x148)/* mjdec_yy_ac_cfgA */
#define MJDEC_YY_AC_CFGB_OFFSET                                 (0x14C)/* mjdec_yy_ac_cfgB */
#define MJDEC_YY_AC_CFGC_OFFSET                                 (0x150)/* mjdec_yy_ac_cfgC */
#define MJDEC_YY_AC_CFGD_OFFSET                                 (0x154)/* mjdec_yy_ac_cfgD */
#define MJDEC_YY_AC_CFGE_OFFSET                                 (0x158)/* mjdec_yy_ac_cfgE */
#define MJDEC_YY_AC_CFGF_OFFSET                                 (0x15C)/* mjdec_yy_ac_cfgF */
#define MJDEC_YY_AC_CFG10_OFFSET                                (0x160)/* mjdec_yy_ac_cfg10 */
#define MJDEC_YY_AC_CFG11_OFFSET                                (0x164)/* mjdec_yy_ac_cfg11 */
#define MJDEC_YY_AC_CFG12_OFFSET                                (0x168)/* mjdec_yy_ac_cfg12 */
#define MJDEC_YY_AC_CFG13_OFFSET                                (0x16C)/* mjdec_yy_ac_cfg13 */
#define MJDEC_YY_AC_CFG14_OFFSET                                (0x170)/* mjdec_yy_ac_cfg14 */
#define MJDEC_YY_AC_CFG15_OFFSET                                (0x174)/* mjdec_yy_ac_cfg15 */
#define MJDEC_YY_AC_CFG16_OFFSET                                (0x178)/* mjdec_yy_ac_cfg16 */
#define MJDEC_YY_AC_CFG17_OFFSET                                (0x17C)/* mjdec_yy_ac_cfg17 */
#define MJDEC_YY_AC_CFG18_OFFSET                                (0x180)/* mjdec_yy_ac_cfg18 */
#define MJDEC_YY_AC_CFG19_OFFSET                                (0x184)/* mjdec_yy_ac_cfg19 */
#define MJDEC_YY_AC_CFG1A_OFFSET                                (0x188)/* mjdec_yy_ac_cfg1A */
#define MJDEC_YY_AC_CFG1B_OFFSET                                (0x18C)/* mjdec_yy_ac_cfg1B */
#define MJDEC_YY_AC_CFG1C_OFFSET                                (0x190)/* mjdec_yy_ac_cfg1C */
#define MJDEC_YY_AC_CFG1D_OFFSET                                (0x194)/* mjdec_yy_ac_cfg1D */
#define MJDEC_YY_AC_CFG1E_OFFSET                                (0x198)/* mjdec_yy_ac_cfg1E */
#define MJDEC_YY_AC_CFG1F_OFFSET                                (0x19C)/* mjdec_yy_ac_cfg1F */
#define MJDEC_YY_AC_CFG20_OFFSET                                (0x1A0)/* mjdec_yy_ac_cfg20 */
#define MJDEC_YY_AC_CFG21_OFFSET                                (0x1A4)/* mjdec_yy_ac_cfg21 */
#define MJDEC_YY_AC_CFG22_OFFSET                                (0x1A8)/* mjdec_yy_ac_cfg22 */
#define MJDEC_YY_AC_CFG23_OFFSET                                (0x1AC)/* mjdec_yy_ac_cfg23 */
#define MJDEC_YY_AC_CFG24_OFFSET                                (0x1B0)/* mjdec_yy_ac_cfg24 */
#define MJDEC_YY_AC_CFG25_OFFSET                                (0x1B4)/* mjdec_yy_ac_cfg25 */
#define MJDEC_YY_AC_CFG26_OFFSET                                (0x1B8)/* mjdec_yy_ac_cfg26 */
#define MJDEC_YY_AC_CFG27_OFFSET                                (0x1BC)/* mjdec_yy_ac_cfg27 */
#define MJDEC_YY_AC_CFG28_OFFSET                                (0x1C0)/* mjdec_yy_ac_cfg28 */
#define MJDEC_YY_AC_CFG29_OFFSET                                (0x1C4)/* mjdec_yy_ac_cfg29 */
#define MJDEC_YY_AC_CFG2A_OFFSET                                (0x1C8)/* mjdec_yy_ac_cfg2A */
#define MJDEC_YY_AC_CFG2B_OFFSET                                (0x1CC)/* mjdec_yy_ac_cfg2B */
#define MJDEC_YY_AC_CFG2C_OFFSET                                (0x1D0)/* mjdec_yy_ac_cfg2C */
#define MJDEC_UV_DC_CFG0_OFFSET                                 (0x200)/* mjdec_uv_dc_cfg0 */
#define MJDEC_UV_DC_CFG1_OFFSET                                 (0x204)/* mjdec_uv_dc_cfg1 */
#define MJDEC_UV_DC_CFG2_OFFSET                                 (0x208)/* mjdec_uv_dc_cfg2 */
#define MJDEC_UV_DC_CFG3_OFFSET                                 (0x20C)/* mjdec_uv_dc_cfg3 */
#define MJDEC_UV_AC_CFG0_OFFSET                                 (0x220)/* mjdec_uv_ac_cfg0 */
#define MJDEC_UV_AC_CFG1_OFFSET                                 (0x224)/* mjdec_uv_ac_cfg1 */
#define MJDEC_UV_AC_CFG2_OFFSET                                 (0x228)/* mjdec_uv_ac_cfg2 */
#define MJDEC_UV_AC_CFG3_OFFSET                                 (0x22C)/* mjdec_uv_ac_cfg3 */
#define MJDEC_UV_AC_CFG4_OFFSET                                 (0x230)/* mjdec_uv_ac_cfg4 */
#define MJDEC_UV_AC_CFG5_OFFSET                                 (0x234)/* mjdec_uv_ac_cfg5 */
#define MJDEC_UV_AC_CFG6_OFFSET                                 (0x238)/* mjdec_uv_ac_cfg6 */
#define MJDEC_UV_AC_CFG7_OFFSET                                 (0x23C)/* mjdec_uv_ac_cfg7 */
#define MJDEC_UV_AC_CFG8_OFFSET                                 (0x240)/* mjdec_uv_ac_cfg8 */
#define MJDEC_UV_AC_CFG9_OFFSET                                 (0x244)/* mjdec_uv_ac_cfg9 */
#define MJDEC_UV_AC_CFGA_OFFSET                                 (0x248)/* mjdec_uv_ac_cfgA */
#define MJDEC_UV_AC_CFGB_OFFSET                                 (0x24C)/* mjdec_uv_ac_cfgB */
#define MJDEC_UV_AC_CFGC_OFFSET                                 (0x250)/* mjdec_uv_ac_cfgC */
#define MJDEC_UV_AC_CFGD_OFFSET                                 (0x254)/* mjdec_uv_ac_cfgD */
#define MJDEC_UV_AC_CFGE_OFFSET                                 (0x258)/* mjdec_uv_ac_cfgE */
#define MJDEC_UV_AC_CFGF_OFFSET                                 (0x25C)/* mjdec_uv_ac_cfgF */
#define MJDEC_UV_AC_CFG10_OFFSET                                (0x260)/* mjdec_uv_ac_cfg10 */
#define MJDEC_UV_AC_CFG11_OFFSET                                (0x264)/* mjdec_uv_ac_cfg11 */
#define MJDEC_UV_AC_CFG12_OFFSET                                (0x268)/* mjdec_uv_ac_cfg12 */
#define MJDEC_UV_AC_CFG13_OFFSET                                (0x26C)/* mjdec_uv_ac_cfg13 */
#define MJDEC_UV_AC_CFG14_OFFSET                                (0x270)/* mjdec_uv_ac_cfg14 */
#define MJDEC_UV_AC_CFG15_OFFSET                                (0x274)/* mjdec_uv_ac_cfg15 */
#define MJDEC_UV_AC_CFG16_OFFSET                                (0x278)/* mjdec_uv_ac_cfg16 */
#define MJDEC_UV_AC_CFG17_OFFSET                                (0x27C)/* mjdec_uv_ac_cfg17 */
#define MJDEC_UV_AC_CFG18_OFFSET                                (0x280)/* mjdec_uv_ac_cfg18 */
#define MJDEC_UV_AC_CFG19_OFFSET                                (0x284)/* mjdec_uv_ac_cfg19 */
#define MJDEC_UV_AC_CFG1A_OFFSET                                (0x288)/* mjdec_uv_ac_cfg1A */
#define MJDEC_UV_AC_CFG1B_OFFSET                                (0x28C)/* mjdec_uv_ac_cfg1B */
#define MJDEC_UV_AC_CFG1C_OFFSET                                (0x290)/* mjdec_uv_ac_cfg1C */
#define MJDEC_UV_AC_CFG1D_OFFSET                                (0x294)/* mjdec_uv_ac_cfg1D */
#define MJDEC_UV_AC_CFG1E_OFFSET                                (0x298)/* mjdec_uv_ac_cfg1E */
#define MJDEC_UV_AC_CFG1F_OFFSET                                (0x29C)/* mjdec_uv_ac_cfg1F */
#define MJDEC_UV_AC_CFG20_OFFSET                                (0x2A0)/* mjdec_uv_ac_cfg20 */
#define MJDEC_UV_AC_CFG21_OFFSET                                (0x2A4)/* mjdec_uv_ac_cfg21 */
#define MJDEC_UV_AC_CFG22_OFFSET                                (0x2A8)/* mjdec_uv_ac_cfg22 */
#define MJDEC_UV_AC_CFG23_OFFSET                                (0x2AC)/* mjdec_uv_ac_cfg23 */
#define MJDEC_UV_AC_CFG24_OFFSET                                (0x2B0)/* mjdec_uv_ac_cfg24 */
#define MJDEC_UV_AC_CFG25_OFFSET                                (0x2B4)/* mjdec_uv_ac_cfg25 */
#define MJDEC_UV_AC_CFG26_OFFSET                                (0x2B8)/* mjdec_uv_ac_cfg26 */
#define MJDEC_UV_AC_CFG27_OFFSET                                (0x2BC)/* mjdec_uv_ac_cfg27 */
#define MJDEC_UV_AC_CFG28_OFFSET                                (0x2C0)/* mjdec_uv_ac_cfg28 */
#define MJDEC_UV_AC_CFG29_OFFSET                                (0x2C4)/* mjdec_uv_ac_cfg29 */
#define MJDEC_UV_AC_CFG2A_OFFSET                                (0x2C8)/* mjdec_uv_ac_cfg2A */
#define MJDEC_UV_AC_CFG2B_OFFSET                                (0x2CC)/* mjdec_uv_ac_cfg2B */
#define MJDEC_UV_AC_CFG2C_OFFSET                                (0x2D0)/* mjdec_uv_ac_cfg2C */
#define MJDEC_DEBUG_OFFSET                                      (0xFF0)/* mjdec_debug */
#define MJDEC_DUMMY_REG_OFFSET                                  (0xFFC)/* mjdec_dummy_reg */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : jdec_control_1 */
#define MJDEC_REG_MJ_DEC_ENABLE                                 (1<<0U)
#define MJDEC_REG_SWAP_MODE                                     (1<<1U)
#define MJDEC_REG_U_EVEN                                        (1<<2U)
#define MJDEC_REG_YUV_MODE_SHIFT                                (3U)
#define MJDEC_REG_YUV_MODE_MASK                                 (0x3<<MJDEC_REG_YUV_MODE_SHIFT)
#define MJDEC_REG_LAST_HF_WBLK_DMY                              (1<<5U)
#define MJDEC_REG_LAST_HF_HBLK_DMY                              (1<<6U)
#define MJDEC_REG_JSTR_HW_EN                                    (1<<7U)
#define MJDEC_REG_W_XLEN_SHIFT                                  (8U)
#define MJDEC_REG_W_XLEN_MASK                                   (0x7<<MJDEC_REG_W_XLEN_SHIFT)
#define MJDEC_REG_LAST_HF_BLK_DMY                               (1<<11U)
#define MJDEC_REG_R_XLEN_SHIFT                                  (12U)
#define MJDEC_REG_R_XLEN_MASK                                   (0x7<<MJDEC_REG_R_XLEN_SHIFT)
#define MJDEC_REG_Q_MODE_SHIFT                                  (16U)
#define MJDEC_REG_Q_MODE_MASK                                   (0x7f<<MJDEC_REG_Q_MODE_SHIFT)
#define MJDEC_REG_BLK_MODE_EN                                   (1<<31U)

/* 0x08 : jdec_yy_frame_addr */
#define MJDEC_REG_YY_ADDR_START_SHIFT                           (0U)
#define MJDEC_REG_YY_ADDR_START_MASK                            (0xffffffff<<MJDEC_REG_YY_ADDR_START_SHIFT)

/* 0x0C : jdec_uv_frame_addr */
#define MJDEC_REG_UV_ADDR_START_SHIFT                           (0U)
#define MJDEC_REG_UV_ADDR_START_MASK                            (0xffffffff<<MJDEC_REG_UV_ADDR_START_SHIFT)

/* 0x1C : jdec_control_3 */
#define MJDEC_REG_INT_NORMAL_EN                                 (1<<0U)
#define MJDEC_REG_INT_IDLE_EN                                   (1<<1U)
#define MJDEC_STS_NORMAL_INT                                    (1<<4U)
#define MJDEC_STS_IDLE_INT                                      (1<<5U)
#define MJDEC_MJPEG_IDLE                                        (1<<8U)
#define MJDEC_MJPEG_FUNC                                        (1<<9U)
#define MJDEC_MJPEG_FLSH                                        (1<<11U)
#define MJDEC_AXI_READ_IDLE                                     (1<<14U)
#define MJDEC_AXI_WRITE_IDLE                                    (1<<15U)
#define MJDEC_REG_FRAME_CNT_TRGR_INT_SHIFT                      (16U)
#define MJDEC_REG_FRAME_CNT_TRGR_INT_MASK                       (0x1f<<MJDEC_REG_FRAME_CNT_TRGR_INT_SHIFT)
#define MJDEC_FRAME_VALID_CNT_SHIFT                             (24U)
#define MJDEC_FRAME_VALID_CNT_MASK                              (0xff<<MJDEC_FRAME_VALID_CNT_SHIFT)

/* 0x20 : jdec_int_clr */
#define MJDEC_REG_INT_CLR                                       (1<<0U)
#define MJDEC_REG_YUV_FRAME_POP                                 (1<<1U)
#define MJDEC_REG_HUF_CAL_PULSE                                 (1<<2U)
#define MJDEC_STS_HUF_CAL_DONE                                  (1<<18U)

/* 0x24 : jdec_fram_push */
#define MJDEC_REG_JP_PUSH                                       (1<<0U)
#define MJDEC_REG_JP_ADDR_SHIFT                                 (2U)
#define MJDEC_REG_JP_ADDR_MASK                                  (0x3fffffff<<MJDEC_REG_JP_ADDR_SHIFT)

/* 0x28 : jdec_fram_sts */
#define MJDEC_JP_FRAME_CNT_SHIFT                                (0U)
#define MJDEC_JP_FRAME_CNT_MASK                                 (0x1f<<MJDEC_JP_FRAME_CNT_SHIFT)

/* 0x2C : jdec_frame_size */
#define MJDEC_REG_FRAME_WBLK_SHIFT                              (0U)
#define MJDEC_REG_FRAME_WBLK_MASK                               (0xfff<<MJDEC_REG_FRAME_WBLK_SHIFT)
#define MJDEC_REG_FRAME_HBLK_SHIFT                              (16U)
#define MJDEC_REG_FRAME_HBLK_MASK                               (0xfff<<MJDEC_REG_FRAME_HBLK_SHIFT)

/* 0x30 : jdec_header_skip */
#define MJDEC_REG_HDER_SKIP_BYTE_SHIFT                          (0U)
#define MJDEC_REG_HDER_SKIP_BYTE_MASK                           (0xffff<<MJDEC_REG_HDER_SKIP_BYTE_SHIFT)
#define MJDEC_REG_HDER_SKIP                                     (1<<16U)

/* 0x40 : jp_addr0 */
#define MJDEC_JP_ADDR_0_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_0_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_0_SHIFT)

/* 0x44 : jp_addr1 */
#define MJDEC_JP_ADDR_1_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_1_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_1_SHIFT)

/* 0x48 : jp_addr2 */
#define MJDEC_JP_ADDR_2_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_2_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_2_SHIFT)

/* 0x4C : jp_addr3 */
#define MJDEC_JP_ADDR_3_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_3_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_3_SHIFT)

/* 0x50 : jp_addr4 */
#define MJDEC_JP_ADDR_4_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_4_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_4_SHIFT)

/* 0x54 : jp_addr5 */
#define MJDEC_JP_ADDR_5_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_5_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_5_SHIFT)

/* 0x58 : jp_addr6 */
#define MJDEC_JP_ADDR_6_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_6_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_6_SHIFT)

/* 0x5C : jp_addr7 */
#define MJDEC_JP_ADDR_7_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_7_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_7_SHIFT)

/* 0x60 : jp_addr_8 */
#define MJDEC_JP_ADDR_8_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_8_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_8_SHIFT)

/* 0x64 : jp_addr_9 */
#define MJDEC_JP_ADDR_9_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_9_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_9_SHIFT)

/* 0x68 : jp_addr_a */
#define MJDEC_JP_ADDR_A_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_A_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_A_SHIFT)

/* 0x6C : jp_addr_b */
#define MJDEC_JP_ADDR_B_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_B_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_B_SHIFT)

/* 0x70 : jp_addr_c */
#define MJDEC_JP_ADDR_C_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_C_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_C_SHIFT)

/* 0x74 : jp_addr_d */
#define MJDEC_JP_ADDR_D_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_D_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_D_SHIFT)

/* 0x78 : jp_addr_e */
#define MJDEC_JP_ADDR_E_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_E_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_E_SHIFT)

/* 0x7C : jp_addr_f */
#define MJDEC_JP_ADDR_F_SHIFT                                   (0U)
#define MJDEC_JP_ADDR_F_MASK                                    (0xffffffff<<MJDEC_JP_ADDR_F_SHIFT)

/* 0x80 : mjdec_yy_q0 */
#define MJDEC_REG_YY_Q_00_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_00_MASK                                  (0xff<<MJDEC_REG_YY_Q_00_SHIFT)
#define MJDEC_REG_YY_Q_01_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_01_MASK                                  (0xff<<MJDEC_REG_YY_Q_01_SHIFT)
#define MJDEC_REG_YY_Q_02_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_02_MASK                                  (0xff<<MJDEC_REG_YY_Q_02_SHIFT)
#define MJDEC_REG_YY_Q_03_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_03_MASK                                  (0xff<<MJDEC_REG_YY_Q_03_SHIFT)

/* 0x84 : mjdec_yy_q1 */
#define MJDEC_REG_YY_Q_04_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_04_MASK                                  (0xff<<MJDEC_REG_YY_Q_04_SHIFT)
#define MJDEC_REG_YY_Q_05_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_05_MASK                                  (0xff<<MJDEC_REG_YY_Q_05_SHIFT)
#define MJDEC_REG_YY_Q_06_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_06_MASK                                  (0xff<<MJDEC_REG_YY_Q_06_SHIFT)
#define MJDEC_REG_YY_Q_07_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_07_MASK                                  (0xff<<MJDEC_REG_YY_Q_07_SHIFT)

/* 0x88 : mjdec_yy_q2 */
#define MJDEC_REG_YY_Q_08_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_08_MASK                                  (0xff<<MJDEC_REG_YY_Q_08_SHIFT)
#define MJDEC_REG_YY_Q_09_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_09_MASK                                  (0xff<<MJDEC_REG_YY_Q_09_SHIFT)
#define MJDEC_REG_YY_Q_0A_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_0A_MASK                                  (0xff<<MJDEC_REG_YY_Q_0A_SHIFT)
#define MJDEC_REG_YY_Q_0B_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_0B_MASK                                  (0xff<<MJDEC_REG_YY_Q_0B_SHIFT)

/* 0x8C : mjdec_yy_q3 */
#define MJDEC_REG_YY_Q_0C_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_0C_MASK                                  (0xff<<MJDEC_REG_YY_Q_0C_SHIFT)
#define MJDEC_REG_YY_Q_0D_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_0D_MASK                                  (0xff<<MJDEC_REG_YY_Q_0D_SHIFT)
#define MJDEC_REG_YY_Q_0E_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_0E_MASK                                  (0xff<<MJDEC_REG_YY_Q_0E_SHIFT)
#define MJDEC_REG_YY_Q_0F_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_0F_MASK                                  (0xff<<MJDEC_REG_YY_Q_0F_SHIFT)

/* 0x90 : mjdec_yy_q4 */
#define MJDEC_REG_YY_Q_10_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_10_MASK                                  (0xff<<MJDEC_REG_YY_Q_10_SHIFT)
#define MJDEC_REG_YY_Q_11_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_11_MASK                                  (0xff<<MJDEC_REG_YY_Q_11_SHIFT)
#define MJDEC_REG_YY_Q_12_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_12_MASK                                  (0xff<<MJDEC_REG_YY_Q_12_SHIFT)
#define MJDEC_REG_YY_Q_13_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_13_MASK                                  (0xff<<MJDEC_REG_YY_Q_13_SHIFT)

/* 0x94 : mjdec_yy_q5 */
#define MJDEC_REG_YY_Q_14_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_14_MASK                                  (0xff<<MJDEC_REG_YY_Q_14_SHIFT)
#define MJDEC_REG_YY_Q_15_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_15_MASK                                  (0xff<<MJDEC_REG_YY_Q_15_SHIFT)
#define MJDEC_REG_YY_Q_16_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_16_MASK                                  (0xff<<MJDEC_REG_YY_Q_16_SHIFT)
#define MJDEC_REG_YY_Q_17_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_17_MASK                                  (0xff<<MJDEC_REG_YY_Q_17_SHIFT)

/* 0x98 : mjdec_yy_q6 */
#define MJDEC_REG_YY_Q_18_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_18_MASK                                  (0xff<<MJDEC_REG_YY_Q_18_SHIFT)
#define MJDEC_REG_YY_Q_19_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_19_MASK                                  (0xff<<MJDEC_REG_YY_Q_19_SHIFT)
#define MJDEC_REG_YY_Q_1A_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_1A_MASK                                  (0xff<<MJDEC_REG_YY_Q_1A_SHIFT)
#define MJDEC_REG_YY_Q_1B_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_1B_MASK                                  (0xff<<MJDEC_REG_YY_Q_1B_SHIFT)

/* 0x9C : mjdec_yy_q7 */
#define MJDEC_REG_YY_Q_1C_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_1C_MASK                                  (0xff<<MJDEC_REG_YY_Q_1C_SHIFT)
#define MJDEC_REG_YY_Q_1D_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_1D_MASK                                  (0xff<<MJDEC_REG_YY_Q_1D_SHIFT)
#define MJDEC_REG_YY_Q_1E_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_1E_MASK                                  (0xff<<MJDEC_REG_YY_Q_1E_SHIFT)
#define MJDEC_REG_YY_Q_1F_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_1F_MASK                                  (0xff<<MJDEC_REG_YY_Q_1F_SHIFT)

/* 0xA0 : mjdec_yy_q8 */
#define MJDEC_REG_YY_Q_20_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_20_MASK                                  (0xff<<MJDEC_REG_YY_Q_20_SHIFT)
#define MJDEC_REG_YY_Q_21_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_21_MASK                                  (0xff<<MJDEC_REG_YY_Q_21_SHIFT)
#define MJDEC_REG_YY_Q_22_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_22_MASK                                  (0xff<<MJDEC_REG_YY_Q_22_SHIFT)
#define MJDEC_REG_YY_Q_23_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_23_MASK                                  (0xff<<MJDEC_REG_YY_Q_23_SHIFT)

/* 0xA4 : mjdec_yy_q9 */
#define MJDEC_REG_YY_Q_24_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_24_MASK                                  (0xff<<MJDEC_REG_YY_Q_24_SHIFT)
#define MJDEC_REG_YY_Q_25_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_25_MASK                                  (0xff<<MJDEC_REG_YY_Q_25_SHIFT)
#define MJDEC_REG_YY_Q_26_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_26_MASK                                  (0xff<<MJDEC_REG_YY_Q_26_SHIFT)
#define MJDEC_REG_YY_Q_27_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_27_MASK                                  (0xff<<MJDEC_REG_YY_Q_27_SHIFT)

/* 0xA8 : mjdec_yy_qa */
#define MJDEC_REG_YY_Q_28_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_28_MASK                                  (0xff<<MJDEC_REG_YY_Q_28_SHIFT)
#define MJDEC_REG_YY_Q_29_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_29_MASK                                  (0xff<<MJDEC_REG_YY_Q_29_SHIFT)
#define MJDEC_REG_YY_Q_2A_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_2A_MASK                                  (0xff<<MJDEC_REG_YY_Q_2A_SHIFT)
#define MJDEC_REG_YY_Q_2B_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_2B_MASK                                  (0xff<<MJDEC_REG_YY_Q_2B_SHIFT)

/* 0xAC : mjdec_yy_qb */
#define MJDEC_REG_YY_Q_2C_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_2C_MASK                                  (0xff<<MJDEC_REG_YY_Q_2C_SHIFT)
#define MJDEC_REG_YY_Q_2D_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_2D_MASK                                  (0xff<<MJDEC_REG_YY_Q_2D_SHIFT)
#define MJDEC_REG_YY_Q_2E_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_2E_MASK                                  (0xff<<MJDEC_REG_YY_Q_2E_SHIFT)
#define MJDEC_REG_YY_Q_2F_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_2F_MASK                                  (0xff<<MJDEC_REG_YY_Q_2F_SHIFT)

/* 0xB0 : mjdec_yy_qc */
#define MJDEC_REG_YY_Q_30_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_30_MASK                                  (0xff<<MJDEC_REG_YY_Q_30_SHIFT)
#define MJDEC_REG_YY_Q_31_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_31_MASK                                  (0xff<<MJDEC_REG_YY_Q_31_SHIFT)
#define MJDEC_REG_YY_Q_32_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_32_MASK                                  (0xff<<MJDEC_REG_YY_Q_32_SHIFT)
#define MJDEC_REG_YY_Q_33_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_33_MASK                                  (0xff<<MJDEC_REG_YY_Q_33_SHIFT)

/* 0xB4 : mjdec_yy_qd */
#define MJDEC_REG_YY_Q_34_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_34_MASK                                  (0xff<<MJDEC_REG_YY_Q_34_SHIFT)
#define MJDEC_REG_YY_Q_35_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_35_MASK                                  (0xff<<MJDEC_REG_YY_Q_35_SHIFT)
#define MJDEC_REG_YY_Q_36_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_36_MASK                                  (0xff<<MJDEC_REG_YY_Q_36_SHIFT)
#define MJDEC_REG_YY_Q_37_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_37_MASK                                  (0xff<<MJDEC_REG_YY_Q_37_SHIFT)

/* 0xB8 : mjdec_yy_qe */
#define MJDEC_REG_YY_Q_38_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_38_MASK                                  (0xff<<MJDEC_REG_YY_Q_38_SHIFT)
#define MJDEC_REG_YY_Q_39_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_39_MASK                                  (0xff<<MJDEC_REG_YY_Q_39_SHIFT)
#define MJDEC_REG_YY_Q_3A_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_3A_MASK                                  (0xff<<MJDEC_REG_YY_Q_3A_SHIFT)
#define MJDEC_REG_YY_Q_3B_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_3B_MASK                                  (0xff<<MJDEC_REG_YY_Q_3B_SHIFT)

/* 0xBC : mjdec_yy_qf */
#define MJDEC_REG_YY_Q_3C_SHIFT                                 (0U)
#define MJDEC_REG_YY_Q_3C_MASK                                  (0xff<<MJDEC_REG_YY_Q_3C_SHIFT)
#define MJDEC_REG_YY_Q_3D_SHIFT                                 (8U)
#define MJDEC_REG_YY_Q_3D_MASK                                  (0xff<<MJDEC_REG_YY_Q_3D_SHIFT)
#define MJDEC_REG_YY_Q_3E_SHIFT                                 (16U)
#define MJDEC_REG_YY_Q_3E_MASK                                  (0xff<<MJDEC_REG_YY_Q_3E_SHIFT)
#define MJDEC_REG_YY_Q_3F_SHIFT                                 (24U)
#define MJDEC_REG_YY_Q_3F_MASK                                  (0xff<<MJDEC_REG_YY_Q_3F_SHIFT)

/* 0xC0 : mjdec_uv_q0 */
#define MJDEC_REG_UV_Q_00_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_00_MASK                                  (0xff<<MJDEC_REG_UV_Q_00_SHIFT)
#define MJDEC_REG_UV_Q_01_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_01_MASK                                  (0xff<<MJDEC_REG_UV_Q_01_SHIFT)
#define MJDEC_REG_UV_Q_02_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_02_MASK                                  (0xff<<MJDEC_REG_UV_Q_02_SHIFT)
#define MJDEC_REG_UV_Q_03_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_03_MASK                                  (0xff<<MJDEC_REG_UV_Q_03_SHIFT)

/* 0xC4 : mjdec_uv_q1 */
#define MJDEC_REG_UV_Q_04_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_04_MASK                                  (0xff<<MJDEC_REG_UV_Q_04_SHIFT)
#define MJDEC_REG_UV_Q_05_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_05_MASK                                  (0xff<<MJDEC_REG_UV_Q_05_SHIFT)
#define MJDEC_REG_UV_Q_06_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_06_MASK                                  (0xff<<MJDEC_REG_UV_Q_06_SHIFT)
#define MJDEC_REG_UV_Q_07_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_07_MASK                                  (0xff<<MJDEC_REG_UV_Q_07_SHIFT)

/* 0xC8 : mjdec_uv_q2 */
#define MJDEC_REG_UV_Q_08_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_08_MASK                                  (0xff<<MJDEC_REG_UV_Q_08_SHIFT)
#define MJDEC_REG_UV_Q_09_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_09_MASK                                  (0xff<<MJDEC_REG_UV_Q_09_SHIFT)
#define MJDEC_REG_UV_Q_0A_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_0A_MASK                                  (0xff<<MJDEC_REG_UV_Q_0A_SHIFT)
#define MJDEC_REG_UV_Q_0B_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_0B_MASK                                  (0xff<<MJDEC_REG_UV_Q_0B_SHIFT)

/* 0xCC : mjdec_uv_q3 */
#define MJDEC_REG_UV_Q_0C_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_0C_MASK                                  (0xff<<MJDEC_REG_UV_Q_0C_SHIFT)
#define MJDEC_REG_UV_Q_0D_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_0D_MASK                                  (0xff<<MJDEC_REG_UV_Q_0D_SHIFT)
#define MJDEC_REG_UV_Q_0E_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_0E_MASK                                  (0xff<<MJDEC_REG_UV_Q_0E_SHIFT)
#define MJDEC_REG_UV_Q_0F_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_0F_MASK                                  (0xff<<MJDEC_REG_UV_Q_0F_SHIFT)

/* 0xD0 : mjdec_uv_q4 */
#define MJDEC_REG_UV_Q_10_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_10_MASK                                  (0xff<<MJDEC_REG_UV_Q_10_SHIFT)
#define MJDEC_REG_UV_Q_11_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_11_MASK                                  (0xff<<MJDEC_REG_UV_Q_11_SHIFT)
#define MJDEC_REG_UV_Q_12_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_12_MASK                                  (0xff<<MJDEC_REG_UV_Q_12_SHIFT)
#define MJDEC_REG_UV_Q_13_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_13_MASK                                  (0xff<<MJDEC_REG_UV_Q_13_SHIFT)

/* 0xD4 : mjdec_uv_q5 */
#define MJDEC_REG_UV_Q_14_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_14_MASK                                  (0xff<<MJDEC_REG_UV_Q_14_SHIFT)
#define MJDEC_REG_UV_Q_15_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_15_MASK                                  (0xff<<MJDEC_REG_UV_Q_15_SHIFT)
#define MJDEC_REG_UV_Q_16_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_16_MASK                                  (0xff<<MJDEC_REG_UV_Q_16_SHIFT)
#define MJDEC_REG_UV_Q_17_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_17_MASK                                  (0xff<<MJDEC_REG_UV_Q_17_SHIFT)

/* 0xD8 : mjdec_uv_q6 */
#define MJDEC_REG_UV_Q_18_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_18_MASK                                  (0xff<<MJDEC_REG_UV_Q_18_SHIFT)
#define MJDEC_REG_UV_Q_19_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_19_MASK                                  (0xff<<MJDEC_REG_UV_Q_19_SHIFT)
#define MJDEC_REG_UV_Q_1A_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_1A_MASK                                  (0xff<<MJDEC_REG_UV_Q_1A_SHIFT)
#define MJDEC_REG_UV_Q_1B_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_1B_MASK                                  (0xff<<MJDEC_REG_UV_Q_1B_SHIFT)

/* 0xDC : mjdec_uv_q7 */
#define MJDEC_REG_UV_Q_1C_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_1C_MASK                                  (0xff<<MJDEC_REG_UV_Q_1C_SHIFT)
#define MJDEC_REG_UV_Q_1D_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_1D_MASK                                  (0xff<<MJDEC_REG_UV_Q_1D_SHIFT)
#define MJDEC_REG_UV_Q_1E_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_1E_MASK                                  (0xff<<MJDEC_REG_UV_Q_1E_SHIFT)
#define MJDEC_REG_UV_Q_1F_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_1F_MASK                                  (0xff<<MJDEC_REG_UV_Q_1F_SHIFT)

/* 0xE0 : mjdec_uv_q8 */
#define MJDEC_REG_UV_Q_20_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_20_MASK                                  (0xff<<MJDEC_REG_UV_Q_20_SHIFT)
#define MJDEC_REG_UV_Q_21_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_21_MASK                                  (0xff<<MJDEC_REG_UV_Q_21_SHIFT)
#define MJDEC_REG_UV_Q_22_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_22_MASK                                  (0xff<<MJDEC_REG_UV_Q_22_SHIFT)
#define MJDEC_REG_UV_Q_23_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_23_MASK                                  (0xff<<MJDEC_REG_UV_Q_23_SHIFT)

/* 0xE4 : mjdec_uv_q9 */
#define MJDEC_REG_UV_Q_24_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_24_MASK                                  (0xff<<MJDEC_REG_UV_Q_24_SHIFT)
#define MJDEC_REG_UV_Q_25_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_25_MASK                                  (0xff<<MJDEC_REG_UV_Q_25_SHIFT)
#define MJDEC_REG_UV_Q_26_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_26_MASK                                  (0xff<<MJDEC_REG_UV_Q_26_SHIFT)
#define MJDEC_REG_UV_Q_27_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_27_MASK                                  (0xff<<MJDEC_REG_UV_Q_27_SHIFT)

/* 0xE8 : mjdec_uv_qa */
#define MJDEC_REG_UV_Q_28_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_28_MASK                                  (0xff<<MJDEC_REG_UV_Q_28_SHIFT)
#define MJDEC_REG_UV_Q_29_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_29_MASK                                  (0xff<<MJDEC_REG_UV_Q_29_SHIFT)
#define MJDEC_REG_UV_Q_2A_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_2A_MASK                                  (0xff<<MJDEC_REG_UV_Q_2A_SHIFT)
#define MJDEC_REG_UV_Q_2B_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_2B_MASK                                  (0xff<<MJDEC_REG_UV_Q_2B_SHIFT)

/* 0xEC : mjdec_uv_qb */
#define MJDEC_REG_UV_Q_2C_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_2C_MASK                                  (0xff<<MJDEC_REG_UV_Q_2C_SHIFT)
#define MJDEC_REG_UV_Q_2D_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_2D_MASK                                  (0xff<<MJDEC_REG_UV_Q_2D_SHIFT)
#define MJDEC_REG_UV_Q_2E_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_2E_MASK                                  (0xff<<MJDEC_REG_UV_Q_2E_SHIFT)
#define MJDEC_REG_UV_Q_2F_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_2F_MASK                                  (0xff<<MJDEC_REG_UV_Q_2F_SHIFT)

/* 0xF0 : mjdec_uv_qc */
#define MJDEC_REG_UV_Q_30_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_30_MASK                                  (0xff<<MJDEC_REG_UV_Q_30_SHIFT)
#define MJDEC_REG_UV_Q_31_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_31_MASK                                  (0xff<<MJDEC_REG_UV_Q_31_SHIFT)
#define MJDEC_REG_UV_Q_32_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_32_MASK                                  (0xff<<MJDEC_REG_UV_Q_32_SHIFT)
#define MJDEC_REG_UV_Q_33_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_33_MASK                                  (0xff<<MJDEC_REG_UV_Q_33_SHIFT)

/* 0xF4 : mjdec_uv_qd */
#define MJDEC_REG_UV_Q_34_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_34_MASK                                  (0xff<<MJDEC_REG_UV_Q_34_SHIFT)
#define MJDEC_REG_UV_Q_35_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_35_MASK                                  (0xff<<MJDEC_REG_UV_Q_35_SHIFT)
#define MJDEC_REG_UV_Q_36_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_36_MASK                                  (0xff<<MJDEC_REG_UV_Q_36_SHIFT)
#define MJDEC_REG_UV_Q_37_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_37_MASK                                  (0xff<<MJDEC_REG_UV_Q_37_SHIFT)

/* 0xF8 : mjdec_uv_qe */
#define MJDEC_REG_UV_Q_38_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_38_MASK                                  (0xff<<MJDEC_REG_UV_Q_38_SHIFT)
#define MJDEC_REG_UV_Q_39_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_39_MASK                                  (0xff<<MJDEC_REG_UV_Q_39_SHIFT)
#define MJDEC_REG_UV_Q_3A_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_3A_MASK                                  (0xff<<MJDEC_REG_UV_Q_3A_SHIFT)
#define MJDEC_REG_UV_Q_3B_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_3B_MASK                                  (0xff<<MJDEC_REG_UV_Q_3B_SHIFT)

/* 0xFC : mjdec_uv_qf */
#define MJDEC_REG_UV_Q_3C_SHIFT                                 (0U)
#define MJDEC_REG_UV_Q_3C_MASK                                  (0xff<<MJDEC_REG_UV_Q_3C_SHIFT)
#define MJDEC_REG_UV_Q_3D_SHIFT                                 (8U)
#define MJDEC_REG_UV_Q_3D_MASK                                  (0xff<<MJDEC_REG_UV_Q_3D_SHIFT)
#define MJDEC_REG_UV_Q_3E_SHIFT                                 (16U)
#define MJDEC_REG_UV_Q_3E_MASK                                  (0xff<<MJDEC_REG_UV_Q_3E_SHIFT)
#define MJDEC_REG_UV_Q_3F_SHIFT                                 (24U)
#define MJDEC_REG_UV_Q_3F_MASK                                  (0xff<<MJDEC_REG_UV_Q_3F_SHIFT)

/* 0x100 : mjdec_yy_dc_cfg0 */
#define MJDEC_REG_YY_DC_LEN_CNT_01_SHIFT                        (0U)
#define MJDEC_REG_YY_DC_LEN_CNT_01_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_01_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_02_SHIFT                        (4U)
#define MJDEC_REG_YY_DC_LEN_CNT_02_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_02_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_03_SHIFT                        (8U)
#define MJDEC_REG_YY_DC_LEN_CNT_03_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_03_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_04_SHIFT                        (12U)
#define MJDEC_REG_YY_DC_LEN_CNT_04_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_04_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_05_SHIFT                        (16U)
#define MJDEC_REG_YY_DC_LEN_CNT_05_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_05_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_06_SHIFT                        (20U)
#define MJDEC_REG_YY_DC_LEN_CNT_06_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_06_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_07_SHIFT                        (24U)
#define MJDEC_REG_YY_DC_LEN_CNT_07_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_07_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_08_SHIFT                        (28U)
#define MJDEC_REG_YY_DC_LEN_CNT_08_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_08_SHIFT)

/* 0x104 : mjdec_yy_dc_cfg1 */
#define MJDEC_REG_YY_DC_LEN_CNT_09_SHIFT                        (0U)
#define MJDEC_REG_YY_DC_LEN_CNT_09_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_09_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_0A_SHIFT                        (4U)
#define MJDEC_REG_YY_DC_LEN_CNT_0A_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_0A_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_0B_SHIFT                        (8U)
#define MJDEC_REG_YY_DC_LEN_CNT_0B_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_0B_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_0C_SHIFT                        (12U)
#define MJDEC_REG_YY_DC_LEN_CNT_0C_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_0C_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_0D_SHIFT                        (16U)
#define MJDEC_REG_YY_DC_LEN_CNT_0D_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_0D_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_0E_SHIFT                        (20U)
#define MJDEC_REG_YY_DC_LEN_CNT_0E_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_0E_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_0F_SHIFT                        (24U)
#define MJDEC_REG_YY_DC_LEN_CNT_0F_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_0F_SHIFT)
#define MJDEC_REG_YY_DC_LEN_CNT_10_SHIFT                        (28U)
#define MJDEC_REG_YY_DC_LEN_CNT_10_MASK                         (0xf<<MJDEC_REG_YY_DC_LEN_CNT_10_SHIFT)

/* 0x108 : mjdec_yy_dc_cfg2 */
#define MJDEC_REG_YY_DC_VAL_00_SHIFT                            (0U)
#define MJDEC_REG_YY_DC_VAL_00_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_00_SHIFT)
#define MJDEC_REG_YY_DC_VAL_01_SHIFT                            (4U)
#define MJDEC_REG_YY_DC_VAL_01_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_01_SHIFT)
#define MJDEC_REG_YY_DC_VAL_02_SHIFT                            (8U)
#define MJDEC_REG_YY_DC_VAL_02_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_02_SHIFT)
#define MJDEC_REG_YY_DC_VAL_03_SHIFT                            (12U)
#define MJDEC_REG_YY_DC_VAL_03_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_03_SHIFT)
#define MJDEC_REG_YY_DC_VAL_04_SHIFT                            (16U)
#define MJDEC_REG_YY_DC_VAL_04_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_04_SHIFT)
#define MJDEC_REG_YY_DC_VAL_05_SHIFT                            (20U)
#define MJDEC_REG_YY_DC_VAL_05_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_05_SHIFT)
#define MJDEC_REG_YY_DC_VAL_06_SHIFT                            (24U)
#define MJDEC_REG_YY_DC_VAL_06_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_06_SHIFT)
#define MJDEC_REG_YY_DC_VAL_07_SHIFT                            (28U)
#define MJDEC_REG_YY_DC_VAL_07_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_07_SHIFT)

/* 0x10C : mjdec_yy_dc_cfg3 */
#define MJDEC_REG_YY_DC_VAL_08_SHIFT                            (0U)
#define MJDEC_REG_YY_DC_VAL_08_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_08_SHIFT)
#define MJDEC_REG_YY_DC_VAL_09_SHIFT                            (4U)
#define MJDEC_REG_YY_DC_VAL_09_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_09_SHIFT)
#define MJDEC_REG_YY_DC_VAL_0A_SHIFT                            (8U)
#define MJDEC_REG_YY_DC_VAL_0A_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_0A_SHIFT)
#define MJDEC_REG_YY_DC_VAL_0B_SHIFT                            (12U)
#define MJDEC_REG_YY_DC_VAL_0B_MASK                             (0xf<<MJDEC_REG_YY_DC_VAL_0B_SHIFT)

/* 0x120 : mjdec_yy_ac_cfg0 */
#define MJDEC_REG_YY_AC_LEN_CNT_01_SHIFT                        (0U)
#define MJDEC_REG_YY_AC_LEN_CNT_01_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_01_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_02_SHIFT                        (8U)
#define MJDEC_REG_YY_AC_LEN_CNT_02_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_02_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_03_SHIFT                        (16U)
#define MJDEC_REG_YY_AC_LEN_CNT_03_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_03_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_04_SHIFT                        (24U)
#define MJDEC_REG_YY_AC_LEN_CNT_04_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_04_SHIFT)

/* 0x124 : mjdec_yy_ac_cfg1 */
#define MJDEC_REG_YY_AC_LEN_CNT_05_SHIFT                        (0U)
#define MJDEC_REG_YY_AC_LEN_CNT_05_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_05_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_06_SHIFT                        (8U)
#define MJDEC_REG_YY_AC_LEN_CNT_06_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_06_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_07_SHIFT                        (16U)
#define MJDEC_REG_YY_AC_LEN_CNT_07_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_07_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_08_SHIFT                        (24U)
#define MJDEC_REG_YY_AC_LEN_CNT_08_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_08_SHIFT)

/* 0x128 : mjdec_yy_ac_cfg2 */
#define MJDEC_REG_YY_AC_LEN_CNT_09_SHIFT                        (0U)
#define MJDEC_REG_YY_AC_LEN_CNT_09_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_09_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_0A_SHIFT                        (8U)
#define MJDEC_REG_YY_AC_LEN_CNT_0A_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_0A_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_0B_SHIFT                        (16U)
#define MJDEC_REG_YY_AC_LEN_CNT_0B_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_0B_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_0C_SHIFT                        (24U)
#define MJDEC_REG_YY_AC_LEN_CNT_0C_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_0C_SHIFT)

/* 0x12C : mjdec_yy_ac_cfg3 */
#define MJDEC_REG_YY_AC_LEN_CNT_0D_SHIFT                        (0U)
#define MJDEC_REG_YY_AC_LEN_CNT_0D_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_0D_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_0E_SHIFT                        (8U)
#define MJDEC_REG_YY_AC_LEN_CNT_0E_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_0E_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_0F_SHIFT                        (16U)
#define MJDEC_REG_YY_AC_LEN_CNT_0F_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_0F_SHIFT)
#define MJDEC_REG_YY_AC_LEN_CNT_10_SHIFT                        (24U)
#define MJDEC_REG_YY_AC_LEN_CNT_10_MASK                         (0xff<<MJDEC_REG_YY_AC_LEN_CNT_10_SHIFT)

/* 0x130 : mjdec_yy_ac_cfg4 */
#define MJDEC_REG_YY_AC_VAL_01_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_01_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_01_SHIFT)
#define MJDEC_REG_YY_AC_VAL_02_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_02_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_02_SHIFT)
#define MJDEC_REG_YY_AC_VAL_03_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_03_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_03_SHIFT)
#define MJDEC_REG_YY_AC_VAL_04_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_04_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_04_SHIFT)

/* 0x134 : mjdec_yy_ac_cfg5 */
#define MJDEC_REG_YY_AC_VAL_05_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_05_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_05_SHIFT)
#define MJDEC_REG_YY_AC_VAL_06_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_06_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_06_SHIFT)
#define MJDEC_REG_YY_AC_VAL_07_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_07_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_07_SHIFT)
#define MJDEC_REG_YY_AC_VAL_08_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_08_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_08_SHIFT)

/* 0x138 : mjdec_yy_ac_cfg6 */
#define MJDEC_REG_YY_AC_VAL_09_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_09_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_09_SHIFT)
#define MJDEC_REG_YY_AC_VAL_0A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_0A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_0A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_0B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_0B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_0B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_0C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_0C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_0C_SHIFT)

/* 0x13C : mjdec_yy_ac_cfg7 */
#define MJDEC_REG_YY_AC_VAL_0D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_0D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_0D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_0E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_0E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_0E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_0F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_0F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_0F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_10_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_10_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_10_SHIFT)

/* 0x140 : mjdec_yy_ac_cfg8 */
#define MJDEC_REG_YY_AC_VAL_11_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_11_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_11_SHIFT)
#define MJDEC_REG_YY_AC_VAL_12_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_12_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_12_SHIFT)
#define MJDEC_REG_YY_AC_VAL_13_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_13_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_13_SHIFT)
#define MJDEC_REG_YY_AC_VAL_14_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_14_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_14_SHIFT)

/* 0x144 : mjdec_yy_ac_cfg9 */
#define MJDEC_REG_YY_AC_VAL_15_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_15_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_15_SHIFT)
#define MJDEC_REG_YY_AC_VAL_16_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_16_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_16_SHIFT)
#define MJDEC_REG_YY_AC_VAL_17_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_17_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_17_SHIFT)
#define MJDEC_REG_YY_AC_VAL_18_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_18_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_18_SHIFT)

/* 0x148 : mjdec_yy_ac_cfgA */
#define MJDEC_REG_YY_AC_VAL_19_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_19_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_19_SHIFT)
#define MJDEC_REG_YY_AC_VAL_1A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_1A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_1A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_1B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_1B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_1B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_1C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_1C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_1C_SHIFT)

/* 0x14C : mjdec_yy_ac_cfgB */
#define MJDEC_REG_YY_AC_VAL_1D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_1D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_1D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_1E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_1E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_1E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_1F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_1F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_1F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_20_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_20_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_20_SHIFT)

/* 0x150 : mjdec_yy_ac_cfgC */
#define MJDEC_REG_YY_AC_VAL_21_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_21_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_21_SHIFT)
#define MJDEC_REG_YY_AC_VAL_22_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_22_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_22_SHIFT)
#define MJDEC_REG_YY_AC_VAL_23_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_23_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_23_SHIFT)
#define MJDEC_REG_YY_AC_VAL_24_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_24_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_24_SHIFT)

/* 0x154 : mjdec_yy_ac_cfgD */
#define MJDEC_REG_YY_AC_VAL_25_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_25_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_25_SHIFT)
#define MJDEC_REG_YY_AC_VAL_26_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_26_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_26_SHIFT)
#define MJDEC_REG_YY_AC_VAL_27_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_27_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_27_SHIFT)
#define MJDEC_REG_YY_AC_VAL_28_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_28_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_28_SHIFT)

/* 0x158 : mjdec_yy_ac_cfgE */
#define MJDEC_REG_YY_AC_VAL_29_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_29_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_29_SHIFT)
#define MJDEC_REG_YY_AC_VAL_2A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_2A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_2A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_2B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_2B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_2B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_2C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_2C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_2C_SHIFT)

/* 0x15C : mjdec_yy_ac_cfgF */
#define MJDEC_REG_YY_AC_VAL_2D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_2D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_2D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_2E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_2E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_2E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_2F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_2F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_2F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_30_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_30_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_30_SHIFT)

/* 0x160 : mjdec_yy_ac_cfg10 */
#define MJDEC_REG_YY_AC_VAL_31_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_31_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_31_SHIFT)
#define MJDEC_REG_YY_AC_VAL_32_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_32_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_32_SHIFT)
#define MJDEC_REG_YY_AC_VAL_33_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_33_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_33_SHIFT)
#define MJDEC_REG_YY_AC_VAL_34_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_34_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_34_SHIFT)

/* 0x164 : mjdec_yy_ac_cfg11 */
#define MJDEC_REG_YY_AC_VAL_35_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_35_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_35_SHIFT)
#define MJDEC_REG_YY_AC_VAL_36_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_36_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_36_SHIFT)
#define MJDEC_REG_YY_AC_VAL_37_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_37_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_37_SHIFT)
#define MJDEC_REG_YY_AC_VAL_38_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_38_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_38_SHIFT)

/* 0x168 : mjdec_yy_ac_cfg12 */
#define MJDEC_REG_YY_AC_VAL_39_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_39_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_39_SHIFT)
#define MJDEC_REG_YY_AC_VAL_3A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_3A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_3A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_3B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_3B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_3B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_3C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_3C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_3C_SHIFT)

/* 0x16C : mjdec_yy_ac_cfg13 */
#define MJDEC_REG_YY_AC_VAL_3D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_3D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_3D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_3E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_3E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_3E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_3F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_3F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_3F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_40_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_40_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_40_SHIFT)

/* 0x170 : mjdec_yy_ac_cfg14 */
#define MJDEC_REG_YY_AC_VAL_41_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_41_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_41_SHIFT)
#define MJDEC_REG_YY_AC_VAL_42_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_42_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_42_SHIFT)
#define MJDEC_REG_YY_AC_VAL_43_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_43_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_43_SHIFT)
#define MJDEC_REG_YY_AC_VAL_44_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_44_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_44_SHIFT)

/* 0x174 : mjdec_yy_ac_cfg15 */
#define MJDEC_REG_YY_AC_VAL_45_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_45_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_45_SHIFT)
#define MJDEC_REG_YY_AC_VAL_46_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_46_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_46_SHIFT)
#define MJDEC_REG_YY_AC_VAL_47_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_47_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_47_SHIFT)
#define MJDEC_REG_YY_AC_VAL_48_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_48_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_48_SHIFT)

/* 0x178 : mjdec_yy_ac_cfg16 */
#define MJDEC_REG_YY_AC_VAL_49_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_49_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_49_SHIFT)
#define MJDEC_REG_YY_AC_VAL_4A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_4A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_4A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_4B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_4B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_4B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_4C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_4C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_4C_SHIFT)

/* 0x17C : mjdec_yy_ac_cfg17 */
#define MJDEC_REG_YY_AC_VAL_4D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_4D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_4D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_4E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_4E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_4E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_4F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_4F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_4F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_50_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_50_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_50_SHIFT)

/* 0x180 : mjdec_yy_ac_cfg18 */
#define MJDEC_REG_YY_AC_VAL_51_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_51_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_51_SHIFT)
#define MJDEC_REG_YY_AC_VAL_52_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_52_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_52_SHIFT)
#define MJDEC_REG_YY_AC_VAL_53_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_53_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_53_SHIFT)
#define MJDEC_REG_YY_AC_VAL_54_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_54_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_54_SHIFT)

/* 0x184 : mjdec_yy_ac_cfg19 */
#define MJDEC_REG_YY_AC_VAL_55_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_55_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_55_SHIFT)
#define MJDEC_REG_YY_AC_VAL_56_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_56_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_56_SHIFT)
#define MJDEC_REG_YY_AC_VAL_57_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_57_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_57_SHIFT)
#define MJDEC_REG_YY_AC_VAL_58_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_58_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_58_SHIFT)

/* 0x188 : mjdec_yy_ac_cfg1A */
#define MJDEC_REG_YY_AC_VAL_59_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_59_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_59_SHIFT)
#define MJDEC_REG_YY_AC_VAL_5A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_5A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_5A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_5B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_5B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_5B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_5C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_5C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_5C_SHIFT)

/* 0x18C : mjdec_yy_ac_cfg1B */
#define MJDEC_REG_YY_AC_VAL_5D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_5D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_5D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_5E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_5E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_5E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_5F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_5F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_5F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_60_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_60_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_60_SHIFT)

/* 0x190 : mjdec_yy_ac_cfg1C */
#define MJDEC_REG_YY_AC_VAL_61_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_61_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_61_SHIFT)
#define MJDEC_REG_YY_AC_VAL_62_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_62_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_62_SHIFT)
#define MJDEC_REG_YY_AC_VAL_63_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_63_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_63_SHIFT)
#define MJDEC_REG_YY_AC_VAL_64_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_64_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_64_SHIFT)

/* 0x194 : mjdec_yy_ac_cfg1D */
#define MJDEC_REG_YY_AC_VAL_65_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_65_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_65_SHIFT)
#define MJDEC_REG_YY_AC_VAL_66_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_66_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_66_SHIFT)
#define MJDEC_REG_YY_AC_VAL_67_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_67_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_67_SHIFT)
#define MJDEC_REG_YY_AC_VAL_68_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_68_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_68_SHIFT)

/* 0x198 : mjdec_yy_ac_cfg1E */
#define MJDEC_REG_YY_AC_VAL_69_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_69_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_69_SHIFT)
#define MJDEC_REG_YY_AC_VAL_6A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_6A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_6A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_6B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_6B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_6B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_6C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_6C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_6C_SHIFT)

/* 0x19C : mjdec_yy_ac_cfg1F */
#define MJDEC_REG_YY_AC_VAL_6D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_6D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_6D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_6E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_6E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_6E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_6F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_6F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_6F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_70_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_70_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_70_SHIFT)

/* 0x1A0 : mjdec_yy_ac_cfg20 */
#define MJDEC_REG_YY_AC_VAL_71_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_71_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_71_SHIFT)
#define MJDEC_REG_YY_AC_VAL_72_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_72_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_72_SHIFT)
#define MJDEC_REG_YY_AC_VAL_73_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_73_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_73_SHIFT)
#define MJDEC_REG_YY_AC_VAL_74_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_74_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_74_SHIFT)

/* 0x1A4 : mjdec_yy_ac_cfg21 */
#define MJDEC_REG_YY_AC_VAL_75_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_75_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_75_SHIFT)
#define MJDEC_REG_YY_AC_VAL_76_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_76_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_76_SHIFT)
#define MJDEC_REG_YY_AC_VAL_77_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_77_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_77_SHIFT)
#define MJDEC_REG_YY_AC_VAL_78_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_78_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_78_SHIFT)

/* 0x1A8 : mjdec_yy_ac_cfg22 */
#define MJDEC_REG_YY_AC_VAL_79_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_79_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_79_SHIFT)
#define MJDEC_REG_YY_AC_VAL_7A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_7A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_7A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_7B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_7B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_7B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_7C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_7C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_7C_SHIFT)

/* 0x1AC : mjdec_yy_ac_cfg23 */
#define MJDEC_REG_YY_AC_VAL_7D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_7D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_7D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_7E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_7E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_7E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_7F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_7F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_7F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_80_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_80_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_80_SHIFT)

/* 0x1B0 : mjdec_yy_ac_cfg24 */
#define MJDEC_REG_YY_AC_VAL_81_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_81_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_81_SHIFT)
#define MJDEC_REG_YY_AC_VAL_82_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_82_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_82_SHIFT)
#define MJDEC_REG_YY_AC_VAL_83_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_83_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_83_SHIFT)
#define MJDEC_REG_YY_AC_VAL_84_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_84_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_84_SHIFT)

/* 0x1B4 : mjdec_yy_ac_cfg25 */
#define MJDEC_REG_YY_AC_VAL_85_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_85_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_85_SHIFT)
#define MJDEC_REG_YY_AC_VAL_86_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_86_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_86_SHIFT)
#define MJDEC_REG_YY_AC_VAL_87_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_87_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_87_SHIFT)
#define MJDEC_REG_YY_AC_VAL_88_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_88_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_88_SHIFT)

/* 0x1B8 : mjdec_yy_ac_cfg26 */
#define MJDEC_REG_YY_AC_VAL_89_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_89_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_89_SHIFT)
#define MJDEC_REG_YY_AC_VAL_8A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_8A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_8A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_8B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_8B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_8B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_8C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_8C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_8C_SHIFT)

/* 0x1BC : mjdec_yy_ac_cfg27 */
#define MJDEC_REG_YY_AC_VAL_8D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_8D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_8D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_8E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_8E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_8E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_8F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_8F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_8F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_90_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_90_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_90_SHIFT)

/* 0x1C0 : mjdec_yy_ac_cfg28 */
#define MJDEC_REG_YY_AC_VAL_91_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_91_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_91_SHIFT)
#define MJDEC_REG_YY_AC_VAL_92_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_92_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_92_SHIFT)
#define MJDEC_REG_YY_AC_VAL_93_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_93_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_93_SHIFT)
#define MJDEC_REG_YY_AC_VAL_94_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_94_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_94_SHIFT)

/* 0x1C4 : mjdec_yy_ac_cfg29 */
#define MJDEC_REG_YY_AC_VAL_95_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_95_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_95_SHIFT)
#define MJDEC_REG_YY_AC_VAL_96_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_96_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_96_SHIFT)
#define MJDEC_REG_YY_AC_VAL_97_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_97_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_97_SHIFT)
#define MJDEC_REG_YY_AC_VAL_98_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_98_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_98_SHIFT)

/* 0x1C8 : mjdec_yy_ac_cfg2A */
#define MJDEC_REG_YY_AC_VAL_99_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_99_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_99_SHIFT)
#define MJDEC_REG_YY_AC_VAL_9A_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_9A_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_9A_SHIFT)
#define MJDEC_REG_YY_AC_VAL_9B_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_9B_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_9B_SHIFT)
#define MJDEC_REG_YY_AC_VAL_9C_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_9C_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_9C_SHIFT)

/* 0x1CC : mjdec_yy_ac_cfg2B */
#define MJDEC_REG_YY_AC_VAL_9D_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_9D_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_9D_SHIFT)
#define MJDEC_REG_YY_AC_VAL_9E_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_9E_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_9E_SHIFT)
#define MJDEC_REG_YY_AC_VAL_9F_SHIFT                            (16U)
#define MJDEC_REG_YY_AC_VAL_9F_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_9F_SHIFT)
#define MJDEC_REG_YY_AC_VAL_A0_SHIFT                            (24U)
#define MJDEC_REG_YY_AC_VAL_A0_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_A0_SHIFT)

/* 0x1D0 : mjdec_yy_ac_cfg2C */
#define MJDEC_REG_YY_AC_VAL_A1_SHIFT                            (0U)
#define MJDEC_REG_YY_AC_VAL_A1_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_A1_SHIFT)
#define MJDEC_REG_YY_AC_VAL_A2_SHIFT                            (8U)
#define MJDEC_REG_YY_AC_VAL_A2_MASK                             (0xff<<MJDEC_REG_YY_AC_VAL_A2_SHIFT)

/* 0x200 : mjdec_uv_dc_cfg0 */
#define MJDEC_REG_UV_DC_LEN_CNT_01_SHIFT                        (0U)
#define MJDEC_REG_UV_DC_LEN_CNT_01_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_01_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_02_SHIFT                        (4U)
#define MJDEC_REG_UV_DC_LEN_CNT_02_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_02_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_03_SHIFT                        (8U)
#define MJDEC_REG_UV_DC_LEN_CNT_03_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_03_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_04_SHIFT                        (12U)
#define MJDEC_REG_UV_DC_LEN_CNT_04_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_04_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_05_SHIFT                        (16U)
#define MJDEC_REG_UV_DC_LEN_CNT_05_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_05_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_06_SHIFT                        (20U)
#define MJDEC_REG_UV_DC_LEN_CNT_06_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_06_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_07_SHIFT                        (24U)
#define MJDEC_REG_UV_DC_LEN_CNT_07_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_07_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_08_SHIFT                        (28U)
#define MJDEC_REG_UV_DC_LEN_CNT_08_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_08_SHIFT)

/* 0x204 : mjdec_uv_dc_cfg1 */
#define MJDEC_REG_UV_DC_LEN_CNT_09_SHIFT                        (0U)
#define MJDEC_REG_UV_DC_LEN_CNT_09_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_09_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_0A_SHIFT                        (4U)
#define MJDEC_REG_UV_DC_LEN_CNT_0A_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_0A_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_0B_SHIFT                        (8U)
#define MJDEC_REG_UV_DC_LEN_CNT_0B_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_0B_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_0C_SHIFT                        (12U)
#define MJDEC_REG_UV_DC_LEN_CNT_0C_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_0C_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_0D_SHIFT                        (16U)
#define MJDEC_REG_UV_DC_LEN_CNT_0D_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_0D_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_0E_SHIFT                        (20U)
#define MJDEC_REG_UV_DC_LEN_CNT_0E_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_0E_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_0F_SHIFT                        (24U)
#define MJDEC_REG_UV_DC_LEN_CNT_0F_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_0F_SHIFT)
#define MJDEC_REG_UV_DC_LEN_CNT_10_SHIFT                        (28U)
#define MJDEC_REG_UV_DC_LEN_CNT_10_MASK                         (0xf<<MJDEC_REG_UV_DC_LEN_CNT_10_SHIFT)

/* 0x208 : mjdec_uv_dc_cfg2 */
#define MJDEC_REG_UV_DC_VAL_00_SHIFT                            (0U)
#define MJDEC_REG_UV_DC_VAL_00_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_00_SHIFT)
#define MJDEC_REG_UV_DC_VAL_01_SHIFT                            (4U)
#define MJDEC_REG_UV_DC_VAL_01_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_01_SHIFT)
#define MJDEC_REG_UV_DC_VAL_02_SHIFT                            (8U)
#define MJDEC_REG_UV_DC_VAL_02_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_02_SHIFT)
#define MJDEC_REG_UV_DC_VAL_03_SHIFT                            (12U)
#define MJDEC_REG_UV_DC_VAL_03_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_03_SHIFT)
#define MJDEC_REG_UV_DC_VAL_04_SHIFT                            (16U)
#define MJDEC_REG_UV_DC_VAL_04_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_04_SHIFT)
#define MJDEC_REG_UV_DC_VAL_05_SHIFT                            (20U)
#define MJDEC_REG_UV_DC_VAL_05_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_05_SHIFT)
#define MJDEC_REG_UV_DC_VAL_06_SHIFT                            (24U)
#define MJDEC_REG_UV_DC_VAL_06_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_06_SHIFT)
#define MJDEC_REG_UV_DC_VAL_07_SHIFT                            (28U)
#define MJDEC_REG_UV_DC_VAL_07_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_07_SHIFT)

/* 0x20C : mjdec_uv_dc_cfg3 */
#define MJDEC_REG_UV_DC_VAL_08_SHIFT                            (0U)
#define MJDEC_REG_UV_DC_VAL_08_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_08_SHIFT)
#define MJDEC_REG_UV_DC_VAL_09_SHIFT                            (4U)
#define MJDEC_REG_UV_DC_VAL_09_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_09_SHIFT)
#define MJDEC_REG_UV_DC_VAL_0A_SHIFT                            (8U)
#define MJDEC_REG_UV_DC_VAL_0A_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_0A_SHIFT)
#define MJDEC_REG_UV_DC_VAL_0B_SHIFT                            (12U)
#define MJDEC_REG_UV_DC_VAL_0B_MASK                             (0xf<<MJDEC_REG_UV_DC_VAL_0B_SHIFT)

/* 0x220 : mjdec_uv_ac_cfg0 */
#define MJDEC_REG_UV_AC_LEN_CNT_01_SHIFT                        (0U)
#define MJDEC_REG_UV_AC_LEN_CNT_01_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_01_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_02_SHIFT                        (8U)
#define MJDEC_REG_UV_AC_LEN_CNT_02_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_02_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_03_SHIFT                        (16U)
#define MJDEC_REG_UV_AC_LEN_CNT_03_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_03_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_04_SHIFT                        (24U)
#define MJDEC_REG_UV_AC_LEN_CNT_04_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_04_SHIFT)

/* 0x224 : mjdec_uv_ac_cfg1 */
#define MJDEC_REG_UV_AC_LEN_CNT_05_SHIFT                        (0U)
#define MJDEC_REG_UV_AC_LEN_CNT_05_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_05_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_06_SHIFT                        (8U)
#define MJDEC_REG_UV_AC_LEN_CNT_06_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_06_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_07_SHIFT                        (16U)
#define MJDEC_REG_UV_AC_LEN_CNT_07_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_07_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_08_SHIFT                        (24U)
#define MJDEC_REG_UV_AC_LEN_CNT_08_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_08_SHIFT)

/* 0x228 : mjdec_uv_ac_cfg2 */
#define MJDEC_REG_UV_AC_LEN_CNT_09_SHIFT                        (0U)
#define MJDEC_REG_UV_AC_LEN_CNT_09_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_09_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_0A_SHIFT                        (8U)
#define MJDEC_REG_UV_AC_LEN_CNT_0A_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_0A_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_0B_SHIFT                        (16U)
#define MJDEC_REG_UV_AC_LEN_CNT_0B_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_0B_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_0C_SHIFT                        (24U)
#define MJDEC_REG_UV_AC_LEN_CNT_0C_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_0C_SHIFT)

/* 0x22C : mjdec_uv_ac_cfg3 */
#define MJDEC_REG_UV_AC_LEN_CNT_0D_SHIFT                        (0U)
#define MJDEC_REG_UV_AC_LEN_CNT_0D_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_0D_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_0E_SHIFT                        (8U)
#define MJDEC_REG_UV_AC_LEN_CNT_0E_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_0E_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_0F_SHIFT                        (16U)
#define MJDEC_REG_UV_AC_LEN_CNT_0F_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_0F_SHIFT)
#define MJDEC_REG_UV_AC_LEN_CNT_10_SHIFT                        (24U)
#define MJDEC_REG_UV_AC_LEN_CNT_10_MASK                         (0xff<<MJDEC_REG_UV_AC_LEN_CNT_10_SHIFT)

/* 0x230 : mjdec_uv_ac_cfg4 */
#define MJDEC_REG_UV_AC_VAL_01_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_01_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_01_SHIFT)
#define MJDEC_REG_UV_AC_VAL_02_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_02_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_02_SHIFT)
#define MJDEC_REG_UV_AC_VAL_03_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_03_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_03_SHIFT)
#define MJDEC_REG_UV_AC_VAL_04_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_04_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_04_SHIFT)

/* 0x234 : mjdec_uv_ac_cfg5 */
#define MJDEC_REG_UV_AC_VAL_05_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_05_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_05_SHIFT)
#define MJDEC_REG_UV_AC_VAL_06_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_06_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_06_SHIFT)
#define MJDEC_REG_UV_AC_VAL_07_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_07_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_07_SHIFT)
#define MJDEC_REG_UV_AC_VAL_08_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_08_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_08_SHIFT)

/* 0x238 : mjdec_uv_ac_cfg6 */
#define MJDEC_REG_UV_AC_VAL_09_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_09_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_09_SHIFT)
#define MJDEC_REG_UV_AC_VAL_0A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_0A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_0A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_0B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_0B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_0B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_0C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_0C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_0C_SHIFT)

/* 0x23C : mjdec_uv_ac_cfg7 */
#define MJDEC_REG_UV_AC_VAL_0D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_0D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_0D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_0E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_0E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_0E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_0F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_0F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_0F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_10_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_10_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_10_SHIFT)

/* 0x240 : mjdec_uv_ac_cfg8 */
#define MJDEC_REG_UV_AC_VAL_11_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_11_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_11_SHIFT)
#define MJDEC_REG_UV_AC_VAL_12_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_12_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_12_SHIFT)
#define MJDEC_REG_UV_AC_VAL_13_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_13_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_13_SHIFT)
#define MJDEC_REG_UV_AC_VAL_14_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_14_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_14_SHIFT)

/* 0x244 : mjdec_uv_ac_cfg9 */
#define MJDEC_REG_UV_AC_VAL_15_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_15_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_15_SHIFT)
#define MJDEC_REG_UV_AC_VAL_16_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_16_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_16_SHIFT)
#define MJDEC_REG_UV_AC_VAL_17_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_17_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_17_SHIFT)
#define MJDEC_REG_UV_AC_VAL_18_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_18_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_18_SHIFT)

/* 0x248 : mjdec_uv_ac_cfgA */
#define MJDEC_REG_UV_AC_VAL_19_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_19_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_19_SHIFT)
#define MJDEC_REG_UV_AC_VAL_1A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_1A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_1A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_1B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_1B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_1B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_1C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_1C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_1C_SHIFT)

/* 0x24C : mjdec_uv_ac_cfgB */
#define MJDEC_REG_UV_AC_VAL_1D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_1D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_1D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_1E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_1E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_1E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_1F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_1F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_1F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_20_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_20_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_20_SHIFT)

/* 0x250 : mjdec_uv_ac_cfgC */
#define MJDEC_REG_UV_AC_VAL_21_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_21_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_21_SHIFT)
#define MJDEC_REG_UV_AC_VAL_22_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_22_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_22_SHIFT)
#define MJDEC_REG_UV_AC_VAL_23_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_23_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_23_SHIFT)
#define MJDEC_REG_UV_AC_VAL_24_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_24_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_24_SHIFT)

/* 0x254 : mjdec_uv_ac_cfgD */
#define MJDEC_REG_UV_AC_VAL_25_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_25_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_25_SHIFT)
#define MJDEC_REG_UV_AC_VAL_26_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_26_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_26_SHIFT)
#define MJDEC_REG_UV_AC_VAL_27_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_27_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_27_SHIFT)
#define MJDEC_REG_UV_AC_VAL_28_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_28_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_28_SHIFT)

/* 0x258 : mjdec_uv_ac_cfgE */
#define MJDEC_REG_UV_AC_VAL_29_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_29_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_29_SHIFT)
#define MJDEC_REG_UV_AC_VAL_2A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_2A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_2A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_2B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_2B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_2B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_2C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_2C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_2C_SHIFT)

/* 0x25C : mjdec_uv_ac_cfgF */
#define MJDEC_REG_UV_AC_VAL_2D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_2D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_2D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_2E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_2E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_2E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_2F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_2F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_2F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_30_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_30_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_30_SHIFT)

/* 0x260 : mjdec_uv_ac_cfg10 */
#define MJDEC_REG_UV_AC_VAL_31_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_31_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_31_SHIFT)
#define MJDEC_REG_UV_AC_VAL_32_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_32_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_32_SHIFT)
#define MJDEC_REG_UV_AC_VAL_33_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_33_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_33_SHIFT)
#define MJDEC_REG_UV_AC_VAL_34_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_34_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_34_SHIFT)

/* 0x264 : mjdec_uv_ac_cfg11 */
#define MJDEC_REG_UV_AC_VAL_35_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_35_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_35_SHIFT)
#define MJDEC_REG_UV_AC_VAL_36_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_36_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_36_SHIFT)
#define MJDEC_REG_UV_AC_VAL_37_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_37_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_37_SHIFT)
#define MJDEC_REG_UV_AC_VAL_38_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_38_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_38_SHIFT)

/* 0x268 : mjdec_uv_ac_cfg12 */
#define MJDEC_REG_UV_AC_VAL_39_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_39_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_39_SHIFT)
#define MJDEC_REG_UV_AC_VAL_3A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_3A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_3A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_3B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_3B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_3B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_3C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_3C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_3C_SHIFT)

/* 0x26C : mjdec_uv_ac_cfg13 */
#define MJDEC_REG_UV_AC_VAL_3D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_3D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_3D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_3E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_3E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_3E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_3F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_3F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_3F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_40_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_40_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_40_SHIFT)

/* 0x270 : mjdec_uv_ac_cfg14 */
#define MJDEC_REG_UV_AC_VAL_41_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_41_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_41_SHIFT)
#define MJDEC_REG_UV_AC_VAL_42_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_42_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_42_SHIFT)
#define MJDEC_REG_UV_AC_VAL_43_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_43_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_43_SHIFT)
#define MJDEC_REG_UV_AC_VAL_44_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_44_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_44_SHIFT)

/* 0x274 : mjdec_uv_ac_cfg15 */
#define MJDEC_REG_UV_AC_VAL_45_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_45_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_45_SHIFT)
#define MJDEC_REG_UV_AC_VAL_46_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_46_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_46_SHIFT)
#define MJDEC_REG_UV_AC_VAL_47_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_47_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_47_SHIFT)
#define MJDEC_REG_UV_AC_VAL_48_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_48_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_48_SHIFT)

/* 0x278 : mjdec_uv_ac_cfg16 */
#define MJDEC_REG_UV_AC_VAL_49_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_49_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_49_SHIFT)
#define MJDEC_REG_UV_AC_VAL_4A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_4A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_4A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_4B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_4B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_4B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_4C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_4C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_4C_SHIFT)

/* 0x27C : mjdec_uv_ac_cfg17 */
#define MJDEC_REG_UV_AC_VAL_4D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_4D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_4D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_4E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_4E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_4E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_4F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_4F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_4F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_50_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_50_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_50_SHIFT)

/* 0x280 : mjdec_uv_ac_cfg18 */
#define MJDEC_REG_UV_AC_VAL_51_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_51_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_51_SHIFT)
#define MJDEC_REG_UV_AC_VAL_52_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_52_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_52_SHIFT)
#define MJDEC_REG_UV_AC_VAL_53_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_53_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_53_SHIFT)
#define MJDEC_REG_UV_AC_VAL_54_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_54_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_54_SHIFT)

/* 0x284 : mjdec_uv_ac_cfg19 */
#define MJDEC_REG_UV_AC_VAL_55_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_55_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_55_SHIFT)
#define MJDEC_REG_UV_AC_VAL_56_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_56_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_56_SHIFT)
#define MJDEC_REG_UV_AC_VAL_57_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_57_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_57_SHIFT)
#define MJDEC_REG_UV_AC_VAL_58_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_58_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_58_SHIFT)

/* 0x288 : mjdec_uv_ac_cfg1A */
#define MJDEC_REG_UV_AC_VAL_59_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_59_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_59_SHIFT)
#define MJDEC_REG_UV_AC_VAL_5A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_5A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_5A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_5B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_5B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_5B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_5C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_5C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_5C_SHIFT)

/* 0x28C : mjdec_uv_ac_cfg1B */
#define MJDEC_REG_UV_AC_VAL_5D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_5D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_5D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_5E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_5E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_5E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_5F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_5F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_5F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_60_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_60_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_60_SHIFT)

/* 0x290 : mjdec_uv_ac_cfg1C */
#define MJDEC_REG_UV_AC_VAL_61_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_61_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_61_SHIFT)
#define MJDEC_REG_UV_AC_VAL_62_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_62_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_62_SHIFT)
#define MJDEC_REG_UV_AC_VAL_63_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_63_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_63_SHIFT)
#define MJDEC_REG_UV_AC_VAL_64_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_64_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_64_SHIFT)

/* 0x294 : mjdec_uv_ac_cfg1D */
#define MJDEC_REG_UV_AC_VAL_65_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_65_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_65_SHIFT)
#define MJDEC_REG_UV_AC_VAL_66_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_66_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_66_SHIFT)
#define MJDEC_REG_UV_AC_VAL_67_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_67_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_67_SHIFT)
#define MJDEC_REG_UV_AC_VAL_68_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_68_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_68_SHIFT)

/* 0x298 : mjdec_uv_ac_cfg1E */
#define MJDEC_REG_UV_AC_VAL_69_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_69_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_69_SHIFT)
#define MJDEC_REG_UV_AC_VAL_6A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_6A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_6A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_6B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_6B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_6B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_6C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_6C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_6C_SHIFT)

/* 0x29C : mjdec_uv_ac_cfg1F */
#define MJDEC_REG_UV_AC_VAL_6D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_6D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_6D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_6E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_6E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_6E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_6F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_6F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_6F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_70_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_70_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_70_SHIFT)

/* 0x2A0 : mjdec_uv_ac_cfg20 */
#define MJDEC_REG_UV_AC_VAL_71_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_71_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_71_SHIFT)
#define MJDEC_REG_UV_AC_VAL_72_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_72_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_72_SHIFT)
#define MJDEC_REG_UV_AC_VAL_73_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_73_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_73_SHIFT)
#define MJDEC_REG_UV_AC_VAL_74_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_74_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_74_SHIFT)

/* 0x2A4 : mjdec_uv_ac_cfg21 */
#define MJDEC_REG_UV_AC_VAL_75_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_75_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_75_SHIFT)
#define MJDEC_REG_UV_AC_VAL_76_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_76_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_76_SHIFT)
#define MJDEC_REG_UV_AC_VAL_77_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_77_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_77_SHIFT)
#define MJDEC_REG_UV_AC_VAL_78_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_78_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_78_SHIFT)

/* 0x2A8 : mjdec_uv_ac_cfg22 */
#define MJDEC_REG_UV_AC_VAL_79_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_79_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_79_SHIFT)
#define MJDEC_REG_UV_AC_VAL_7A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_7A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_7A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_7B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_7B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_7B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_7C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_7C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_7C_SHIFT)

/* 0x2AC : mjdec_uv_ac_cfg23 */
#define MJDEC_REG_UV_AC_VAL_7D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_7D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_7D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_7E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_7E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_7E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_7F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_7F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_7F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_80_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_80_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_80_SHIFT)

/* 0x2B0 : mjdec_uv_ac_cfg24 */
#define MJDEC_REG_UV_AC_VAL_81_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_81_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_81_SHIFT)
#define MJDEC_REG_UV_AC_VAL_82_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_82_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_82_SHIFT)
#define MJDEC_REG_UV_AC_VAL_83_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_83_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_83_SHIFT)
#define MJDEC_REG_UV_AC_VAL_84_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_84_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_84_SHIFT)

/* 0x2B4 : mjdec_uv_ac_cfg25 */
#define MJDEC_REG_UV_AC_VAL_85_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_85_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_85_SHIFT)
#define MJDEC_REG_UV_AC_VAL_86_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_86_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_86_SHIFT)
#define MJDEC_REG_UV_AC_VAL_87_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_87_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_87_SHIFT)
#define MJDEC_REG_UV_AC_VAL_88_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_88_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_88_SHIFT)

/* 0x2B8 : mjdec_uv_ac_cfg26 */
#define MJDEC_REG_UV_AC_VAL_89_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_89_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_89_SHIFT)
#define MJDEC_REG_UV_AC_VAL_8A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_8A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_8A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_8B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_8B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_8B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_8C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_8C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_8C_SHIFT)

/* 0x2BC : mjdec_uv_ac_cfg27 */
#define MJDEC_REG_UV_AC_VAL_8D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_8D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_8D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_8E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_8E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_8E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_8F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_8F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_8F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_90_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_90_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_90_SHIFT)

/* 0x2C0 : mjdec_uv_ac_cfg28 */
#define MJDEC_REG_UV_AC_VAL_91_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_91_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_91_SHIFT)
#define MJDEC_REG_UV_AC_VAL_92_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_92_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_92_SHIFT)
#define MJDEC_REG_UV_AC_VAL_93_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_93_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_93_SHIFT)
#define MJDEC_REG_UV_AC_VAL_94_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_94_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_94_SHIFT)

/* 0x2C4 : mjdec_uv_ac_cfg29 */
#define MJDEC_REG_UV_AC_VAL_95_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_95_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_95_SHIFT)
#define MJDEC_REG_UV_AC_VAL_96_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_96_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_96_SHIFT)
#define MJDEC_REG_UV_AC_VAL_97_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_97_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_97_SHIFT)
#define MJDEC_REG_UV_AC_VAL_98_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_98_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_98_SHIFT)

/* 0x2C8 : mjdec_uv_ac_cfg2A */
#define MJDEC_REG_UV_AC_VAL_99_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_99_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_99_SHIFT)
#define MJDEC_REG_UV_AC_VAL_9A_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_9A_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_9A_SHIFT)
#define MJDEC_REG_UV_AC_VAL_9B_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_9B_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_9B_SHIFT)
#define MJDEC_REG_UV_AC_VAL_9C_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_9C_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_9C_SHIFT)

/* 0x2CC : mjdec_uv_ac_cfg2B */
#define MJDEC_REG_UV_AC_VAL_9D_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_9D_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_9D_SHIFT)
#define MJDEC_REG_UV_AC_VAL_9E_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_9E_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_9E_SHIFT)
#define MJDEC_REG_UV_AC_VAL_9F_SHIFT                            (16U)
#define MJDEC_REG_UV_AC_VAL_9F_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_9F_SHIFT)
#define MJDEC_REG_UV_AC_VAL_A0_SHIFT                            (24U)
#define MJDEC_REG_UV_AC_VAL_A0_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_A0_SHIFT)

/* 0x2D0 : mjdec_uv_ac_cfg2C */
#define MJDEC_REG_UV_AC_VAL_A1_SHIFT                            (0U)
#define MJDEC_REG_UV_AC_VAL_A1_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_A1_SHIFT)
#define MJDEC_REG_UV_AC_VAL_A2_SHIFT                            (8U)
#define MJDEC_REG_UV_AC_VAL_A2_MASK                             (0xff<<MJDEC_REG_UV_AC_VAL_A2_SHIFT)

/* 0xFF0 : mjdec_debug */
#define MJDEC_REG_MJDEC_DBG_EN                                  (1<<0U)
#define MJDEC_REG_MJDEC_DBG_SEL_SHIFT                           (4U)
#define MJDEC_REG_MJDEC_DBG_SEL_MASK                            (0xf<<MJDEC_REG_MJDEC_DBG_SEL_SHIFT)

/* 0xFFC : mjdec_dummy_reg */
#define MJDEC_JDEC_DUMMY_REG_SHIFT                              (0U)
#define MJDEC_JDEC_DUMMY_REG_MASK                               (0xffffffff<<MJDEC_JDEC_DUMMY_REG_SHIFT)


#endif  /* __HARDWARE_MJDEC_REG_H__ */
