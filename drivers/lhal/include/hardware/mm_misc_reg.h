/**
  ******************************************************************************
  * @file    mm_misc_reg.h
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
#ifndef  __MM_MISC_REG_H__
#define  __MM_MISC_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define MM_MISC_CTRL_OFFSET                                     (0x0)/* mm_misc_ctrl */
#define MM_MISC_DVP_AS_REGA_OFFSET                              (0x4)/* dvp_as_regA */
#define MM_MISC_DVP_AS_REGB_OFFSET                              (0x8)/* dvp_as_regB */
#define MM_MISC_DVP_AS_REGC_OFFSET                              (0xC)/* dvp_as_regC */
#define MM_MISC_DVP_MUX_SEL_REG_OFFSET                          (0x10)/* dvp_mux_sel_reg */
#define MM_MISC_DVP_MUX_SEL_REG2_OFFSET                         (0x14)/* dvp_mux_sel_reg2 */
#define MM_MISC_DVP_BT656_OFFSET                                (0x18)/* dvp_bt656 */
#define MM_MISC_MM_BUS_OFFSET                                   (0x20)/* mm_bus */
#define MM_MISC_OFFSET                                          (0x40)/* mm_misc */
#define MM_MISC_RST_0_OFFSET                                    (0x100)/* mm_misc_rst_0 */
#define MM_MISC_CLK_0_OFFSET                                    (0x120)/* mm_misc_clk_0 */
#define MM_MISC_DISP_Y2R_CONFIG_0_OFFSET                        (0x200)/* disp_y2r_config_0 */
#define MM_MISC_DISP_Y2R_CONFIG_1_OFFSET                        (0x204)/* disp_y2r_config_1 */
#define MM_MISC_DISP_Y2R_CONFIG_2_OFFSET                        (0x208)/* disp_y2r_config_2 */
#define MM_MISC_DISP_Y2R_CONFIG_3_OFFSET                        (0x20C)/* disp_y2r_config_3 */
#define MM_MISC_DISP_Y2R_CONFIG_4_OFFSET                        (0x210)/* disp_y2r_config_4 */
#define MM_MISC_DISP_Y2R_CONFIG_5_OFFSET                        (0x214)/* disp_y2r_config_5 */
#define MM_MISC_DISP_Y2R_CONFIG_6_OFFSET                        (0x218)/* disp_y2r_config_6 */
#define MM_MISC_DISP_Y2R_CONFIG_7_OFFSET                        (0x21C)/* disp_y2r_config_7 */
#define MM_MISC_DISP_R2Y_CONFIG_0_OFFSET                        (0x240)/* disp_r2y_config_0 */
#define MM_MISC_DISP_R2Y_CONFIG_1_OFFSET                        (0x244)/* disp_r2y_config_1 */
#define MM_MISC_DISP_R2Y_CONFIG_2_OFFSET                        (0x248)/* disp_r2y_config_2 */
#define MM_MISC_DISP_R2Y_CONFIG_3_OFFSET                        (0x24C)/* disp_r2y_config_3 */
#define MM_MISC_DISP_R2Y_CONFIG_4_OFFSET                        (0x250)/* disp_r2y_config_4 */
#define MM_MISC_DISP_R2Y_CONFIG_5_OFFSET                        (0x254)/* disp_r2y_config_5 */
#define MM_MISC_DISP_R2Y_CONFIG_6_OFFSET                        (0x258)/* disp_r2y_config_6 */
#define MM_MISC_DISP_R2Y_CONFIG_7_OFFSET                        (0x25C)/* disp_r2y_config_7 */
#define MM_MISC_DISP_CONFIG_OFFSET                              (0x300)/* disp_config */
#define MM_MISC_DISP_DPI_CONFIG_OFFSET                          (0x304)/* disp_dpi_config */
#define MM_MISC_CROP_ENABLE_OFFSET                              (0x310)/* crop_enable */
#define MM_MISC_DP_CROP_HSYNC_OFFSET                            (0x314)/* dp_crop_hsync */
#define MM_MISC_DP_CROP_VSYNC_OFFSET                            (0x318)/* dp_crop_vsync */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : mm_misc_ctrl */
#define MM_MISC_REG_FORCE_VSYNC_SH                              (1<<0U)
#define MM_MISC_CR_DVP_S2P_HS_INV                               (1<<1U)
#define MM_MISC_CR_DVP_S2P_VS_INV                               (1<<2U)
#define MM_MISC_CR_DVP_S2P_DA_ORDER                             (1<<3U)
#define MM_MISC_CR_DVP_S2P_EN                                   (1<<4U)
#define MM_MISC_REG_SNSR_MODE                                   (1<<5U)
#define MM_MISC_REG_FORCE_PCLK_ON_SHIFT                         (16U)
#define MM_MISC_REG_FORCE_PCLK_ON_MASK                          (0xffff<<MM_MISC_REG_FORCE_PCLK_ON_SHIFT)

/* 0x4 : dvp_as_regA */
#define MM_MISC_CR_DVP_AS_EN                                    (1<<0U)
#define MM_MISC_CR_DVP_HSYNC_INV                                (1<<1U)
#define MM_MISC_CR_DVP_VSYNC_INV                                (1<<2U)
#define MM_MISC_CR_TG_EN_AS                                     (1<<3U)
#define MM_MISC_CR_DVP_FIFO_TH_SHIFT                            (4U)
#define MM_MISC_CR_DVP_FIFO_TH_MASK                             (0xfff<<MM_MISC_CR_DVP_FIFO_TH_SHIFT)
#define MM_MISC_CR_TOTAL_HPIXELS_AS_SHIFT                       (16U)
#define MM_MISC_CR_TOTAL_HPIXELS_AS_MASK                        (0xffff<<MM_MISC_CR_TOTAL_HPIXELS_AS_SHIFT)

/* 0x8 : dvp_as_regB */
#define MM_MISC_CR_ACT_PIXEL_WIDTH_AS_SHIFT                     (0U)
#define MM_MISC_CR_ACT_PIXEL_WIDTH_AS_MASK                      (0xffff<<MM_MISC_CR_ACT_PIXEL_WIDTH_AS_SHIFT)
#define MM_MISC_CR_ACT_START_PIXEL_AS_SHIFT                     (16U)
#define MM_MISC_CR_ACT_START_PIXEL_AS_MASK                      (0xffff<<MM_MISC_CR_ACT_START_PIXEL_AS_SHIFT)

/* 0xC : dvp_as_regC */
#define MM_MISC_CR_ACT_LINE_HEIGHT_AS_SHIFT                     (0U)
#define MM_MISC_CR_ACT_LINE_HEIGHT_AS_MASK                      (0xffff<<MM_MISC_CR_ACT_LINE_HEIGHT_AS_SHIFT)
#define MM_MISC_CR_ACT_START_LINE_AS_SHIFT                      (16U)
#define MM_MISC_CR_ACT_START_LINE_AS_MASK                       (0xffff<<MM_MISC_CR_ACT_START_LINE_AS_SHIFT)

/* 0x10 : dvp_mux_sel_reg */
#define MM_MISC_REG_D2SA_IN_SEL_SHIFT                           (0U)
#define MM_MISC_REG_D2SA_IN_SEL_MASK                            (0x3<<MM_MISC_REG_D2SA_IN_SEL_SHIFT)
#define MM_MISC_REG_D2SB_IN_SEL_SHIFT                           (4U)
#define MM_MISC_REG_D2SB_IN_SEL_MASK                            (0x3<<MM_MISC_REG_D2SB_IN_SEL_SHIFT)
#define MM_MISC_REG_D2XA_IN_SEL_SHIFT                           (24U)
#define MM_MISC_REG_D2XA_IN_SEL_MASK                            (0x3<<MM_MISC_REG_D2XA_IN_SEL_SHIFT)
#define MM_MISC_REG_D2XB_IN_SEL_SHIFT                           (28U)
#define MM_MISC_REG_D2XB_IN_SEL_MASK                            (0x3<<MM_MISC_REG_D2XB_IN_SEL_SHIFT)

/* 0x14 : dvp_mux_sel_reg2 */
#define MM_MISC_RG_DISP_OSD_SEL_SHIFT                           (0U)
#define MM_MISC_RG_DISP_OSD_SEL_MASK                            (0x3<<MM_MISC_RG_DISP_OSD_SEL_SHIFT)
#define MM_MISC_RG_DISP_MUXO_SEL_SHIFT                          (4U)
#define MM_MISC_RG_DISP_MUXO_SEL_MASK                           (0x3<<MM_MISC_RG_DISP_MUXO_SEL_SHIFT)

/* 0x18 : dvp_bt656 */
#define MM_MISC_CR_DVP_BT656_EN                                 (1<<0U)
#define MM_MISC_CR_DVP_BT656_CLK_INV                            (1<<1U)
#define MM_MISC_CR_DVP_BT656_YC_INV                             (1<<2U)

/* 0x20 : mm_bus */
#define MM_MISC_CR_W_THRE_MM2EXT_SHIFT                          (0U)
#define MM_MISC_CR_W_THRE_MM2EXT_MASK                           (0x3<<MM_MISC_CR_W_THRE_MM2EXT_SHIFT)

/* 0x40 : mm_misc */
#define MM_MISC_CR_R2B_Y_SEL_SHIFT                              (0U)
#define MM_MISC_CR_R2B_Y_SEL_MASK                               (0x7<<MM_MISC_CR_R2B_Y_SEL_SHIFT)
#define MM_MISC_CR_R2B_C_SEL_SHIFT                              (4U)
#define MM_MISC_CR_R2B_C_SEL_MASK                               (0x7<<MM_MISC_CR_R2B_C_SEL_SHIFT)
#define MM_MISC_CR_BLOCK_SRAM_EN                                (1<<31U)

/* 0x100 : mm_misc_rst_0 */
#define MM_MISC_SWRST_MM_MISC                                   (1<<0U)
#define MM_MISC_SWRST_D2XA                                      (1<<1U)
#define MM_MISC_SWRST_D2XB                                      (1<<2U)
#define MM_MISC_SWRST_JENC                                      (1<<3U)
#define MM_MISC_SWRST_JDEC                                      (1<<4U)
#define MM_MISC_SWRST_DTSRC                                     (1<<5U)
#define MM_MISC_SWRST_R2B                                       (1<<6U)
#define MM_MISC_SWRST_B2R                                       (1<<7U)
#define MM_MISC_SWRST_D2SA                                      (1<<8U)
#define MM_MISC_SWRST_D2SB                                      (1<<9U)
#define MM_MISC_SWRST_OSDA                                      (1<<10U)
#define MM_MISC_SWRST_DISP                                      (1<<11U)

/* 0x120 : mm_misc_clk_0 */
#define MM_MISC_CGEN_MM                                         (1<<0U)

/* 0x200 : disp_y2r_config_0 */
#define MM_MISC_CR_DISP_Y2R_PRE_0_SHIFT                         (0U)
#define MM_MISC_CR_DISP_Y2R_PRE_0_MASK                          (0x1ff<<MM_MISC_CR_DISP_Y2R_PRE_0_SHIFT)
#define MM_MISC_CR_DISP_Y2R_POS_0_SHIFT                         (16U)
#define MM_MISC_CR_DISP_Y2R_POS_0_MASK                          (0x1ff<<MM_MISC_CR_DISP_Y2R_POS_0_SHIFT)
#define MM_MISC_CR_DISP_Y2R_EN                                  (1<<27U)

/* 0x204 : disp_y2r_config_1 */
#define MM_MISC_CR_DISP_Y2R_PRE_1_SHIFT                         (0U)
#define MM_MISC_CR_DISP_Y2R_PRE_1_MASK                          (0x1ff<<MM_MISC_CR_DISP_Y2R_PRE_1_SHIFT)
#define MM_MISC_CR_DISP_Y2R_POS_1_SHIFT                         (16U)
#define MM_MISC_CR_DISP_Y2R_POS_1_MASK                          (0x1ff<<MM_MISC_CR_DISP_Y2R_POS_1_SHIFT)

/* 0x208 : disp_y2r_config_2 */
#define MM_MISC_CR_DISP_Y2R_PRE_2_SHIFT                         (0U)
#define MM_MISC_CR_DISP_Y2R_PRE_2_MASK                          (0x1ff<<MM_MISC_CR_DISP_Y2R_PRE_2_SHIFT)
#define MM_MISC_CR_DISP_Y2R_POS_2_SHIFT                         (16U)
#define MM_MISC_CR_DISP_Y2R_POS_2_MASK                          (0x1ff<<MM_MISC_CR_DISP_Y2R_POS_2_SHIFT)

/* 0x20C : disp_y2r_config_3 */
#define MM_MISC_CR_DISP_Y2R_MTX_00_SHIFT                        (0U)
#define MM_MISC_CR_DISP_Y2R_MTX_00_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_00_SHIFT)
#define MM_MISC_CR_DISP_Y2R_MTX_01_SHIFT                        (16U)
#define MM_MISC_CR_DISP_Y2R_MTX_01_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_01_SHIFT)

/* 0x210 : disp_y2r_config_4 */
#define MM_MISC_CR_DISP_Y2R_MTX_02_SHIFT                        (0U)
#define MM_MISC_CR_DISP_Y2R_MTX_02_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_02_SHIFT)
#define MM_MISC_CR_DISP_Y2R_MTX_10_SHIFT                        (16U)
#define MM_MISC_CR_DISP_Y2R_MTX_10_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_10_SHIFT)

/* 0x214 : disp_y2r_config_5 */
#define MM_MISC_CR_DISP_Y2R_MTX_11_SHIFT                        (0U)
#define MM_MISC_CR_DISP_Y2R_MTX_11_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_11_SHIFT)
#define MM_MISC_CR_DISP_Y2R_MTX_12_SHIFT                        (16U)
#define MM_MISC_CR_DISP_Y2R_MTX_12_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_12_SHIFT)

/* 0x218 : disp_y2r_config_6 */
#define MM_MISC_CR_DISP_Y2R_MTX_20_SHIFT                        (0U)
#define MM_MISC_CR_DISP_Y2R_MTX_20_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_20_SHIFT)
#define MM_MISC_CR_DISP_Y2R_MTX_21_SHIFT                        (16U)
#define MM_MISC_CR_DISP_Y2R_MTX_21_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_21_SHIFT)

/* 0x21C : disp_y2r_config_7 */
#define MM_MISC_CR_DISP_Y2R_MTX_22_SHIFT                        (0U)
#define MM_MISC_CR_DISP_Y2R_MTX_22_MASK                         (0xfff<<MM_MISC_CR_DISP_Y2R_MTX_22_SHIFT)

/* 0x240 : disp_r2y_config_0 */
#define MM_MISC_CR_DISP_R2Y_PRE_0_SHIFT                         (0U)
#define MM_MISC_CR_DISP_R2Y_PRE_0_MASK                          (0x1ff<<MM_MISC_CR_DISP_R2Y_PRE_0_SHIFT)
#define MM_MISC_CR_DISP_R2Y_POS_0_SHIFT                         (16U)
#define MM_MISC_CR_DISP_R2Y_POS_0_MASK                          (0x1ff<<MM_MISC_CR_DISP_R2Y_POS_0_SHIFT)
#define MM_MISC_CR_DISP_R2Y_EN                                  (1<<27U)

/* 0x244 : disp_r2y_config_1 */
#define MM_MISC_CR_DISP_R2Y_PRE_1_SHIFT                         (0U)
#define MM_MISC_CR_DISP_R2Y_PRE_1_MASK                          (0x1ff<<MM_MISC_CR_DISP_R2Y_PRE_1_SHIFT)
#define MM_MISC_CR_DISP_R2Y_POS_1_SHIFT                         (16U)
#define MM_MISC_CR_DISP_R2Y_POS_1_MASK                          (0x1ff<<MM_MISC_CR_DISP_R2Y_POS_1_SHIFT)

/* 0x248 : disp_r2y_config_2 */
#define MM_MISC_CR_DISP_R2Y_PRE_2_SHIFT                         (0U)
#define MM_MISC_CR_DISP_R2Y_PRE_2_MASK                          (0x1ff<<MM_MISC_CR_DISP_R2Y_PRE_2_SHIFT)
#define MM_MISC_CR_DISP_R2Y_POS_2_SHIFT                         (16U)
#define MM_MISC_CR_DISP_R2Y_POS_2_MASK                          (0x1ff<<MM_MISC_CR_DISP_R2Y_POS_2_SHIFT)

/* 0x24C : disp_r2y_config_3 */
#define MM_MISC_CR_DISP_R2Y_MTX_00_SHIFT                        (0U)
#define MM_MISC_CR_DISP_R2Y_MTX_00_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_00_SHIFT)
#define MM_MISC_CR_DISP_R2Y_MTX_01_SHIFT                        (16U)
#define MM_MISC_CR_DISP_R2Y_MTX_01_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_01_SHIFT)

/* 0x250 : disp_r2y_config_4 */
#define MM_MISC_CR_DISP_R2Y_MTX_02_SHIFT                        (0U)
#define MM_MISC_CR_DISP_R2Y_MTX_02_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_02_SHIFT)
#define MM_MISC_CR_DISP_R2Y_MTX_10_SHIFT                        (16U)
#define MM_MISC_CR_DISP_R2Y_MTX_10_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_10_SHIFT)

/* 0x254 : disp_r2y_config_5 */
#define MM_MISC_CR_DISP_R2Y_MTX_11_SHIFT                        (0U)
#define MM_MISC_CR_DISP_R2Y_MTX_11_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_11_SHIFT)
#define MM_MISC_CR_DISP_R2Y_MTX_12_SHIFT                        (16U)
#define MM_MISC_CR_DISP_R2Y_MTX_12_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_12_SHIFT)

/* 0x258 : disp_r2y_config_6 */
#define MM_MISC_CR_DISP_R2Y_MTX_20_SHIFT                        (0U)
#define MM_MISC_CR_DISP_R2Y_MTX_20_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_20_SHIFT)
#define MM_MISC_CR_DISP_R2Y_MTX_21_SHIFT                        (16U)
#define MM_MISC_CR_DISP_R2Y_MTX_21_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_21_SHIFT)

/* 0x25C : disp_r2y_config_7 */
#define MM_MISC_CR_DISP_R2Y_MTX_22_SHIFT                        (0U)
#define MM_MISC_CR_DISP_R2Y_MTX_22_MASK                         (0xfff<<MM_MISC_CR_DISP_R2Y_MTX_22_SHIFT)

/* 0x300 : disp_config */
#define MM_MISC_RG_DISP_BT1120_EN                               (1<<0U)
#define MM_MISC_RG_DISP_DPI_EN                                  (1<<1U)
#define MM_MISC_RG_DISP_HDMI_EN                                 (1<<2U)
#define MM_MISC_RG_DISP_16                                      (1<<3U)
#define MM_MISC_RG_DISP_DPI_ICC_SHIFT                           (4U)
#define MM_MISC_RG_DISP_DPI_ICC_MASK                            (0x7<<MM_MISC_RG_DISP_DPI_ICC_SHIFT)

/* 0x304 : disp_dpi_config */
#define MM_MISC_RG_DISP_DPI_HS_W_SHIFT                          (0U)
#define MM_MISC_RG_DISP_DPI_HS_W_MASK                           (0xff<<MM_MISC_RG_DISP_DPI_HS_W_SHIFT)
#define MM_MISC_RG_DISP_DPI_HFP_W_SHIFT                         (8U)
#define MM_MISC_RG_DISP_DPI_HFP_W_MASK                          (0xff<<MM_MISC_RG_DISP_DPI_HFP_W_SHIFT)
#define MM_MISC_RG_DISP_DPI_VS_W_SHIFT                          (16U)
#define MM_MISC_RG_DISP_DPI_VS_W_MASK                           (0xff<<MM_MISC_RG_DISP_DPI_VS_W_SHIFT)
#define MM_MISC_RG_DISP_DPI_VFP_W_SHIFT                         (24U)
#define MM_MISC_RG_DISP_DPI_VFP_W_MASK                          (0xff<<MM_MISC_RG_DISP_DPI_VFP_W_SHIFT)

/* 0x310 : crop_enable */
#define MM_MISC_REG_DP_CROP_ENABLE                              (1<<8U)

/* 0x314 : dp_crop_hsync */
#define MM_MISC_REG_DP_CROP_HSYNC_START_SHIFT                   (0U)
#define MM_MISC_REG_DP_CROP_HSYNC_START_MASK                    (0xffff<<MM_MISC_REG_DP_CROP_HSYNC_START_SHIFT)
#define MM_MISC_REG_DP_CROP_HSYNC_END_SHIFT                     (16U)
#define MM_MISC_REG_DP_CROP_HSYNC_END_MASK                      (0xffff<<MM_MISC_REG_DP_CROP_HSYNC_END_SHIFT)

/* 0x318 : dp_crop_vsync */
#define MM_MISC_REG_DP_CROP_VSYNC_START_SHIFT                   (0U)
#define MM_MISC_REG_DP_CROP_VSYNC_START_MASK                    (0xffff<<MM_MISC_REG_DP_CROP_VSYNC_START_SHIFT)
#define MM_MISC_REG_DP_CROP_VSYNC_END_SHIFT                     (16U)
#define MM_MISC_REG_DP_CROP_VSYNC_END_MASK                      (0xffff<<MM_MISC_REG_DP_CROP_VSYNC_END_SHIFT)


#endif  /* __MM_MISC_REG_H__ */
