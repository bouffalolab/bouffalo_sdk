/**
  ******************************************************************************
  * @file    cam_front_reg.h
  * @version V1.0
  * @date    2022-12-03
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
#ifndef  __HARDWARE_CAM_FRONT_H__
#define  __HARDWARE_CAM_FRONT_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#if defined(BL616D)
#define CAM_FRONT_MM_MISC_CTRL_OFFSET                           (0x0)/* mm_misc_ctrl */
#define CAM_FRONT_CONFIG_OFFSET                                 (0x4)/* config */
#define CAM_FRONT_DVP_AS_REGB_OFFSET                            (0x8)/* dvp_as_regb */
#define CAM_FRONT_DVP_AS_REGC_OFFSET                            (0xc)/* dvp_as_regc */
#define CAM_FRONT_DVP_MUX_SEL_REG_OFFSET                        (0x10)/* dvp_mux_sel_reg */
#define CAM_FRONT_DVP_MUX_SEL_REG2_OFFSET                       (0x14)/* dvp_mux_sel_reg2 */
#define CAM_FRONT_DVP_BT656_OFFSET                              (0x18)/* dvp_bt656 */
#define CAM_FRONT_MM_BUS_OFFSET                                 (0x20)/* mm_bus */
#define CAM_FRONT_MM_MISC_OFFSET                                (0x40)/* mm_misc */
#define CAM_FRONT_MM_MISC_RST_0_OFFSET                          (0x100)/* mm_misc_rst_0 */
#define CAM_FRONT_MM_MISC_CLK_0_OFFSET                          (0x120)/* mm_misc_clk_0 */
#else
#define CAM_FRONT_CONFIG_OFFSET                                 (0x0)/* config */
#define CAM_FRONT_DVP2BUS_SRC_SEL_1_OFFSET                      (0x8)/* dvp2bus_src_sel_1 */
#if defined(BL616) || defined(BL616L)
#define CAM_FRONT_SNSR_CTRL_OFFSET                              (0xC)/* snsr_ctrl */
#define CAM_FRONT_EMI_MISC_OFFSET                               (0x10)/* emi_misc */
#define CAM_FRONT_ISP_ID_YUV_OFFSET                             (0x14)/* isp_id_yuv */
#endif
#if defined(BL808)
#define CAM_FRONT_PIX_DATA_CTRL_OFFSET                          (0x4)/* pix_data_ctrl */
#define CAM_FRONT_DVP2BUS_SRC_SEL_2_OFFSET                      (0x14)/* dvp2bus_src_sel_2 */
#define CAM_FRONT_ISP_ID_YUV_OFFSET                             (0x28)/* isp_id_yuv */
#define CAM_FRONT_ADJA_CTRL_2_OFFSET                            (0x108)/* adjA_ctrl_2 */
#define CAM_FRONT_Y2RA_CONFIG_0_OFFSET                          (0x160)/* y2rA_config_0 */
#define CAM_FRONT_Y2RA_CONFIG_1_OFFSET                          (0x164)/* y2rA_config_1 */
#define CAM_FRONT_Y2RA_CONFIG_2_OFFSET                          (0x168)/* y2rA_config_2 */
#define CAM_FRONT_Y2RA_CONFIG_3_OFFSET                          (0x16C)/* y2rA_config_3 */
#define CAM_FRONT_Y2RA_CONFIG_4_OFFSET                          (0x170)/* y2rA_config_4 */
#define CAM_FRONT_Y2RA_CONFIG_5_OFFSET                          (0x174)/* y2rA_config_5 */
#define CAM_FRONT_Y2RA_CONFIG_6_OFFSET                          (0x178)/* y2rA_config_6 */
#define CAM_FRONT_Y2RA_CONFIG_7_OFFSET                          (0x17C)/* y2rA_config_7 */
#endif
#endif

/* Register Bitfield definitions *****************************************************/

#if defined(BL616D)
/* 0x0 : mm_misc_ctrl */
#define CAM_FRONT_REG_FORCE_VSYNC_SH                            (1<<0U)
#define CAM_FRONT_CR_DVP_S2P_HS_INV                             (1<<1U)
#define CAM_FRONT_CR_DVP_S2P_VS_INV                             (1<<2U)
#define CAM_FRONT_CR_DVP_S2P_DA_ORDER                           (1<<3U)
#define CAM_FRONT_CR_DVP_S2P_EN                                 (1<<4U)
#define CAM_FRONT_REG_SNSR_MODE                                 (1<<5U)
#define CAM_FRONT_REG_FORCE_PCLK_ON_SHIFT                       (16U)
#define CAM_FRONT_REG_FORCE_PCLK_ON_MASK                        (0xffff<<CAM_FRONT_REG_FORCE_PCLK_ON_SHIFT)
#endif

/* 0x0 : config */
#define CAM_FRONT_RG_DVPAS_ENABLE                               (1<<0U)
#define CAM_FRONT_RG_DVPAS_HS_INV                               (1<<1U)
#define CAM_FRONT_RG_DVPAS_VS_INV                               (1<<2U)
#if defined(BL616D)
#define CAM_FRONT_CR_TG_EN_AS                                   (1<<3U)
#define CAM_FRONT_RG_DVPAS_FIFO_TH_SHIFT                        (4U)
#define CAM_FRONT_RG_DVPAS_FIFO_TH_MASK                         (0xfff<<CAM_FRONT_RG_DVPAS_FIFO_TH_SHIFT)
#define CAM_FRONT_RG_TOTAL_HPIXELS_AS_SHIFT                     (16U)
#define CAM_FRONT_RG_TOTAL_HPIXELS_AS_MASK                      (0xffff<<CAM_FRONT_RG_TOTAL_HPIXELS_AS_SHIFT)
#else
#define CAM_FRONT_RG_DVPAS_DA_ORDER                             (1<<3U)
#define CAM_FRONT_RG_DVPAS_FIFO_TH_SHIFT                        (16U)
#define CAM_FRONT_RG_DVPAS_FIFO_TH_MASK                         (0x7ff<<CAM_FRONT_RG_DVPAS_FIFO_TH_SHIFT)
#endif

#if defined(BL616D)
/* 0x8 : dvp_as_regb */
#define CAM_FRONT_CR_ACT_PIXEL_WIDTH_AS_SHIFT                   (0U)
#define CAM_FRONT_CR_ACT_PIXEL_WIDTH_AS_MASK                    (0xffff<<CAM_FRONT_CR_ACT_PIXEL_WIDTH_AS_SHIFT)
#define CAM_FRONT_CR_ACT_START_PIXEL_AS_SHIFT                   (16U)
#define CAM_FRONT_CR_ACT_START_PIXEL_AS_MASK                    (0xffff<<CAM_FRONT_CR_ACT_START_PIXEL_AS_SHIFT)

/* 0xC : dvp_as_regc */
#define CAM_FRONT_CR_ACT_LINE_HEIGHT_AS_SHIFT                   (0U)
#define CAM_FRONT_CR_ACT_LINE_HEIGHT_AS_MASK                    (0xffff<<CAM_FRONT_CR_ACT_LINE_HEIGHT_AS_SHIFT)
#define CAM_FRONT_CR_ACT_START_LINE_AS_SHIFT                    (16U)
#define CAM_FRONT_CR_ACT_START_LINE_AS_MASK                     (0xffff<<CAM_FRONT_CR_ACT_START_LINE_AS_SHIFT)

/* 0x10 : dvp_mux_sel_reg */
#define CAM_FRONT_REG_422TO420A_IN_SEL_SHIFT                    (4U)
#define CAM_FRONT_REG_422TO420A_IN_SEL_MASK                     (0x7<<CAM_FRONT_REG_422TO420A_IN_SEL_SHIFT)
#define CAM_FRONT_REG_420TO422A_IN_SEL_SHIFT                    (12U)
#define CAM_FRONT_REG_420TO422A_IN_SEL_MASK                     (0x3<<CAM_FRONT_REG_420TO422A_IN_SEL_SHIFT)
#define CAM_FRONT_REG_B2RA_IN_SEL_SHIFT                         (16U)
#define CAM_FRONT_REG_B2RA_IN_SEL_MASK                          (0x3<<CAM_FRONT_REG_B2RA_IN_SEL_SHIFT)
#define CAM_FRONT_REG_D2XA_IN_SEL_SHIFT                         (24U)
#define CAM_FRONT_REG_D2XA_IN_SEL_MASK                          (0x7<<CAM_FRONT_REG_D2XA_IN_SEL_SHIFT)
#define CAM_FRONT_REG_D2XB_IN_SEL_SHIFT                         (28U)
#define CAM_FRONT_REG_D2XB_IN_SEL_MASK                          (0x7<<CAM_FRONT_REG_D2XB_IN_SEL_SHIFT)

/* 0x14 : dvp_mux_sel_reg2 */
#define CAM_FRONT_REG_D2SA_IN_SEL_SHIFT                         (0U)
#define CAM_FRONT_REG_D2SA_IN_SEL_MASK                          (0x7<<CAM_FRONT_REG_D2SA_IN_SEL_SHIFT)
#define CAM_FRONT_REG_D2SB_IN_SEL_SHIFT                         (4U)
#define CAM_FRONT_REG_D2SB_IN_SEL_MASK                          (0x7<<CAM_FRONT_REG_D2SB_IN_SEL_SHIFT)
#define CAM_FRONT_REG_OSDA_IN_SEL_SHIFT                         (8U)
#define CAM_FRONT_REG_OSDA_IN_SEL_MASK                          (0x7<<CAM_FRONT_REG_OSDA_IN_SEL_SHIFT)
#define CAM_FRONT_REG_DISP_IN_SEL_SHIFT                         (12U)
#define CAM_FRONT_REG_DISP_IN_SEL_MASK                          (0x7<<CAM_FRONT_REG_DISP_IN_SEL_SHIFT)

/* 0x18 : dvp_bt656 */
#define CAM_FRONT_CR_DVP_BT656_EN                               (1<<0U)
#define CAM_FRONT_CR_DVP_BT656_CLK_INV                          (1<<1U)
#define CAM_FRONT_CR_DVP_BT656_YC_INV                           (1<<2U)

/* 0x20 : mm_bus */
#define CAM_FRONT_CR_W_THRE_MM2EXT_SHIFT                        (0U)
#define CAM_FRONT_CR_W_THRE_MM2EXT_MASK                         (0x3<<CAM_FRONT_CR_W_THRE_MM2EXT_SHIFT)

/* 0x40 : mm_misc */
#define CAM_FRONT_CR_R2B_Y_SEL_SHIFT                            (0U)
#define CAM_FRONT_CR_R2B_Y_SEL_MASK                             (0x7<<CAM_FRONT_CR_R2B_Y_SEL_SHIFT)
#define CAM_FRONT_CR_R2B_C_SEL_SHIFT                            (4U)
#define CAM_FRONT_CR_R2B_C_SEL_MASK                             (0x7<<CAM_FRONT_CR_R2B_C_SEL_SHIFT)
#define CAM_FRONT_CR_BLOCK_SRAM_EN                              (1<<31U)

/* 0x100 : mm_misc_rst_0 */
#define CAM_FRONT_SWRST_MM_MISC                                 (1<<0U)
#define CAM_FRONT_SWRST_D2XA                                    (1<<1U)
#define CAM_FRONT_SWRST_D2XB                                    (1<<2U)
#define CAM_FRONT_SWRST_JENC                                    (1<<3U)
#define CAM_FRONT_SWRST_JDEC                                    (1<<4U)
#define CAM_FRONT_SWRST_DTSRC                                   (1<<5U)
#define CAM_FRONT_SWRST_R2B                                     (1<<6U)
#define CAM_FRONT_SWRST_B2R                                     (1<<7U)
#define CAM_FRONT_SWRST_D2SA                                    (1<<8U)
#define CAM_FRONT_SWRST_D2SB                                    (1<<9U)
#define CAM_FRONT_SWRST_OSDA                                    (1<<10U)
#define CAM_FRONT_SWRST_DISP                                    (1<<11U)

/* 0x120 : mm_misc_clk_0 */
#define CAM_FRONT_CGEN_MM                                       (1<<0U)
#endif

#if defined(BL808)
/* 0x4 : pix_data_ctrl */
#define CAM_FRONT_REG_PIX_DATA_CTRL_SHIFT                       (0U)
#define CAM_FRONT_REG_PIX_DATA_CTRL_MASK                        (0xfff<<CAM_FRONT_REG_PIX_DATA_CTRL_SHIFT)
#define CAM_FRONT_REG_PIX_DATA_SHT_BIT_SHIFT                    (16U)
#define CAM_FRONT_REG_PIX_DATA_SHT_BIT_MASK                     (0xf<<CAM_FRONT_REG_PIX_DATA_SHT_BIT_SHIFT)
#define CAM_FRONT_REG_PIX_DATA_SHT_DIR                          (1<<20U)
#define CAM_FRONT_REG_ISP_DTSRC_SRC                             (1<<31U)

/* 0x8 : dvp2bus_src_sel_1 */
#define CAM_FRONT_RG_D2B_DVP_SEL_A_SHIFT                        (0U)
#define CAM_FRONT_RG_D2B_DVP_SEL_A_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_A_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_A                               (1<<7U)
#define CAM_FRONT_RG_D2B_DVP_SEL_B_SHIFT                        (8U)
#define CAM_FRONT_RG_D2B_DVP_SEL_B_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_B_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_B                               (1<<15U)
#define CAM_FRONT_RG_D2B_DVP_SEL_C_SHIFT                        (16U)
#define CAM_FRONT_RG_D2B_DVP_SEL_C_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_C_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_C                               (1<<23U)
#define CAM_FRONT_RG_D2B_DVP_SEL_D_SHIFT                        (24U)
#define CAM_FRONT_RG_D2B_DVP_SEL_D_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_D_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_D                               (1<<31U)

/* 0x14 : dvp2bus_src_sel_2 */
#define CAM_FRONT_RG_D2B_DVP_SEL_E_SHIFT                        (0U)
#define CAM_FRONT_RG_D2B_DVP_SEL_E_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_E_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_E                               (1<<7U)
#define CAM_FRONT_RG_D2B_DVP_SEL_F_SHIFT                        (8U)
#define CAM_FRONT_RG_D2B_DVP_SEL_F_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_F_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_F                               (1<<15U)
#define CAM_FRONT_RG_D2B_DVP_SEL_G_SHIFT                        (16U)
#define CAM_FRONT_RG_D2B_DVP_SEL_G_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_G_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_G                               (1<<23U)
#define CAM_FRONT_RG_D2B_DVP_SEL_H_SHIFT                        (24U)
#define CAM_FRONT_RG_D2B_DVP_SEL_H_MASK                         (0x3f<<CAM_FRONT_RG_D2B_DVP_SEL_H_SHIFT)
#define CAM_FRONT_RG_D2X_ID_SEL_H                               (1<<31U)

/* 0x160 : y2rA_config_0 */
#define CAM_FRONT_RG_Y2RA_PRE_0_SHIFT                           (0U)
#define CAM_FRONT_RG_Y2RA_PRE_0_MASK                            (0x1ff<<CAM_FRONT_RG_Y2RA_PRE_0_SHIFT)
#define CAM_FRONT_RG_Y2RA_POS_0_SHIFT                           (16U)
#define CAM_FRONT_RG_Y2RA_POS_0_MASK                            (0x1ff<<CAM_FRONT_RG_Y2RA_POS_0_SHIFT)
#define CAM_FRONT_RG_Y2RA_EN                                    (1<<27U)
#define CAM_FRONT_RG_Y2RA_SEL_SHIFT                             (28U)
#define CAM_FRONT_RG_Y2RA_SEL_MASK                              (0xf<<CAM_FRONT_RG_Y2RA_SEL_SHIFT)

/* 0x164 : y2rA_config_1 */
#define CAM_FRONT_RG_Y2RA_PRE_1_SHIFT                           (0U)
#define CAM_FRONT_RG_Y2RA_PRE_1_MASK                            (0x1ff<<CAM_FRONT_RG_Y2RA_PRE_1_SHIFT)
#define CAM_FRONT_RG_Y2RA_POS_1_SHIFT                           (16U)
#define CAM_FRONT_RG_Y2RA_POS_1_MASK                            (0x1ff<<CAM_FRONT_RG_Y2RA_POS_1_SHIFT)

/* 0x168 : y2rA_config_2 */
#define CAM_FRONT_RG_Y2RA_PRE_2_SHIFT                           (0U)
#define CAM_FRONT_RG_Y2RA_PRE_2_MASK                            (0x1ff<<CAM_FRONT_RG_Y2RA_PRE_2_SHIFT)
#define CAM_FRONT_RG_Y2RA_POS_2_SHIFT                           (16U)
#define CAM_FRONT_RG_Y2RA_POS_2_MASK                            (0x1ff<<CAM_FRONT_RG_Y2RA_POS_2_SHIFT)

/* 0x16C : y2rA_config_3 */
#define CAM_FRONT_RG_Y2RA_MTX_00_SHIFT                          (0U)
#define CAM_FRONT_RG_Y2RA_MTX_00_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_00_SHIFT)
#define CAM_FRONT_RG_Y2RA_MTX_01_SHIFT                          (16U)
#define CAM_FRONT_RG_Y2RA_MTX_01_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_01_SHIFT)

/* 0x170 : y2rA_config_4 */
#define CAM_FRONT_RG_Y2RA_MTX_02_SHIFT                          (0U)
#define CAM_FRONT_RG_Y2RA_MTX_02_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_02_SHIFT)
#define CAM_FRONT_RG_Y2RA_MTX_10_SHIFT                          (16U)
#define CAM_FRONT_RG_Y2RA_MTX_10_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_10_SHIFT)

/* 0x174 : y2rA_config_5 */
#define CAM_FRONT_RG_Y2RA_MTX_11_SHIFT                          (0U)
#define CAM_FRONT_RG_Y2RA_MTX_11_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_11_SHIFT)
#define CAM_FRONT_RG_Y2RA_MTX_12_SHIFT                          (16U)
#define CAM_FRONT_RG_Y2RA_MTX_12_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_12_SHIFT)

/* 0x178 : y2rA_config_6 */
#define CAM_FRONT_RG_Y2RA_MTX_20_SHIFT                          (0U)
#define CAM_FRONT_RG_Y2RA_MTX_20_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_20_SHIFT)
#define CAM_FRONT_RG_Y2RA_MTX_21_SHIFT                          (16U)
#define CAM_FRONT_RG_Y2RA_MTX_21_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_21_SHIFT)

/* 0x17C : y2rA_config_7 */
#define CAM_FRONT_RG_Y2RA_MTX_22_SHIFT                          (0U)
#define CAM_FRONT_RG_Y2RA_MTX_22_MASK                           (0xfff<<CAM_FRONT_RG_Y2RA_MTX_22_SHIFT)
#endif

#if defined(BL616) || defined(BL616L)
/* 0x8 : dvp2bus_src_sel_1 */
#define CAM_FRONT_RG_D2X_DVP_SEL                                (1<<0U)

/* 0xC : snsr_ctrl */
#define CAM_FRONT_RG_SNSR_RST                                   (1<<0U)
#define CAM_FRONT_RG_SNSR_PWDN                                  (1<<1U)

/* 0x10 : emi_misc */
#define CAM_FRONT_REG_X_WTHRE_PB_SHIFT                          (0U)
#define CAM_FRONT_REG_X_WTHRE_PB_MASK                           (0x3<<CAM_FRONT_REG_X_WTHRE_PB_SHIFT)
#define CAM_FRONT_REG_SF_HARB_MODE                              (1<<4U)
#endif

#if !defined(BL616D)
/* 0x14 : isp_id_yuv */
#define CAM_FRONT_REG_YUV_IDGEN_RST                             (1<<0U)
#define CAM_FRONT_REG_YUV_IDGEN_EDGE                            (1<<1U)
#define CAM_FRONT_REG_YUV_IDGEN_CNT_INCR_SHIFT                  (16U)
#define CAM_FRONT_REG_YUV_IDGEN_CNT_INCR_MASK                   (0xffff<<CAM_FRONT_REG_YUV_IDGEN_CNT_INCR_SHIFT)
#endif

#endif  /* __CAM_FRONT_REG_H__ */
