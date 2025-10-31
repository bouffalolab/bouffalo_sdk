/**
  ******************************************************************************
  * @file    osd_draw_reg.h
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
#ifndef __OSD_DRAW_REG_H__
#define __OSD_DRAW_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define OSD_DRAW_CONFIG_OFFSET                   (0x0)  /* osd_draw_config */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG0_OFFSET (0x4)  /* draw_partial_yuv_config0 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG0_OFFSET   (0x8)  /* draw_partial_x_config0 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG0_OFFSET   (0xC)  /* draw_partial_y_config0 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG1_OFFSET (0x10) /* draw_partial_yuv_config1 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG1_OFFSET   (0x14) /* draw_partial_x_config1 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG1_OFFSET   (0x18) /* draw_partial_y_config1 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG2_OFFSET (0x1C) /* draw_partial_yuv_config2 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG2_OFFSET   (0x20) /* draw_partial_x_config2 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG2_OFFSET   (0x24) /* draw_partial_y_config2 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG3_OFFSET (0x28) /* draw_partial_yuv_config3 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG3_OFFSET   (0x2C) /* draw_partial_x_config3 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG3_OFFSET   (0x30) /* draw_partial_y_config3 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG4_OFFSET (0x34) /* draw_partial_yuv_config4 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG4_OFFSET   (0x38) /* draw_partial_x_config4 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG4_OFFSET   (0x3C) /* draw_partial_y_config4 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG5_OFFSET (0x40) /* draw_partial_yuv_config5 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG5_OFFSET   (0x44) /* draw_partial_x_config5 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG5_OFFSET   (0x48) /* draw_partial_y_config5 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG6_OFFSET (0x4C) /* draw_partial_yuv_config6 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG6_OFFSET   (0x50) /* draw_partial_x_config6 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG6_OFFSET   (0x54) /* draw_partial_y_config6 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG7_OFFSET (0x58) /* draw_partial_yuv_config7 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG7_OFFSET   (0x5C) /* draw_partial_x_config7 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG7_OFFSET   (0x60) /* draw_partial_y_config7 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG8_OFFSET (0x64) /* draw_partial_yuv_config8 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG8_OFFSET   (0x68) /* draw_partial_x_config8 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG8_OFFSET   (0x6C) /* draw_partial_y_config8 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG9_OFFSET (0x70) /* draw_partial_yuv_config9 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG9_OFFSET   (0x74) /* draw_partial_x_config9 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG9_OFFSET   (0x78) /* draw_partial_y_config9 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIGA_OFFSET (0x7C) /* draw_partial_yuv_configa */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIGA_OFFSET   (0x80) /* draw_partial_x_configa */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIGA_OFFSET   (0x84) /* draw_partial_y_configa */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIGB_OFFSET (0x88) /* draw_partial_yuv_configb */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIGB_OFFSET   (0x8C) /* draw_partial_x_configb */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIGB_OFFSET   (0x90) /* draw_partial_y_configb */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIGC_OFFSET (0x94) /* draw_partial_yuv_configc */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIGC_OFFSET   (0x98) /* draw_partial_x_configc */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIGC_OFFSET   (0x9C) /* draw_partial_y_configc */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIGD_OFFSET (0xA0) /* draw_partial_yuv_configd */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIGD_OFFSET   (0xA4) /* draw_partial_x_configd */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIGD_OFFSET   (0xA8) /* draw_partial_y_configd */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIGE_OFFSET (0xAC) /* draw_partial_yuv_confige */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIGE_OFFSET   (0xB0) /* draw_partial_x_confige */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIGE_OFFSET   (0xB4) /* draw_partial_y_confige */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIGF_OFFSET (0xB8) /* draw_partial_yuv_configf */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIGF_OFFSET   (0xBC) /* draw_partial_x_configf */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIGF_OFFSET   (0xC0) /* draw_partial_y_configf */
#define OSD_DRAW_OSD_BLEND_EN_OFFSET             (0xF0) /* osd_blend_en */
#define OSD_DRAW_OSD_SW_SHD_OFFSET               (0xF4) /* osd_sw_shd */
#define OSD_DRAW_OSD_INT_STS_OFFSET              (0xF8) /* osd_int_sts */
#define OSD_DRAW_OSD_MISC_OFFSET                 (0xFC) /* osd_misc */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : osd_draw_config */
#define OSD_DRAW_REG_DRAW_EN_L_SHIFT             (0U)
#define OSD_DRAW_REG_DRAW_EN_L_MASK              (0xffff << OSD_DRAW_REG_DRAW_EN_L_SHIFT)
#define OSD_DRAW_REG_DRAW_TYPE_L_SHIFT           (16U)
#define OSD_DRAW_REG_DRAW_TYPE_L_MASK            (0xffff << OSD_DRAW_REG_DRAW_TYPE_L_SHIFT)

/* 0x4 : draw_partial_yuv_config0 */
#define OSD_DRAW_REG_DRAW_TH_00_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_00_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_00_SHIFT)
#define OSD_DRAW_REG_DRAW_V_00_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_00_MASK              (0xff << OSD_DRAW_REG_DRAW_V_00_SHIFT)
#define OSD_DRAW_REG_DRAW_U_00_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_00_MASK              (0xff << OSD_DRAW_REG_DRAW_U_00_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_00_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_00_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_00_SHIFT)

/* 0x8 : draw_partial_x_config0 */
#define OSD_DRAW_REG_DRAW_X_MIN_00_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_00_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_00_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_00_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_00_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_00_SHIFT)

/* 0xC : draw_partial_y_config0 */
#define OSD_DRAW_REG_DRAW_Y_MIN_00_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_00_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_00_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_00_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_00_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_00_SHIFT)

/* 0x10 : draw_partial_yuv_config1 */
#define OSD_DRAW_REG_DRAW_TH_01_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_01_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_01_SHIFT)
#define OSD_DRAW_REG_DRAW_V_01_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_01_MASK              (0xff << OSD_DRAW_REG_DRAW_V_01_SHIFT)
#define OSD_DRAW_REG_DRAW_U_01_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_01_MASK              (0xff << OSD_DRAW_REG_DRAW_U_01_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_01_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_01_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_01_SHIFT)

/* 0x14 : draw_partial_x_config1 */
#define OSD_DRAW_REG_DRAW_X_MIN_01_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_01_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_01_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_01_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_01_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_01_SHIFT)

/* 0x18 : draw_partial_y_config1 */
#define OSD_DRAW_REG_DRAW_Y_MIN_01_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_01_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_01_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_01_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_01_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_01_SHIFT)

/* 0x1C : draw_partial_yuv_config2 */
#define OSD_DRAW_REG_DRAW_TH_02_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_02_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_02_SHIFT)
#define OSD_DRAW_REG_DRAW_V_02_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_02_MASK              (0xff << OSD_DRAW_REG_DRAW_V_02_SHIFT)
#define OSD_DRAW_REG_DRAW_U_02_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_02_MASK              (0xff << OSD_DRAW_REG_DRAW_U_02_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_02_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_02_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_02_SHIFT)

/* 0x20 : draw_partial_x_config2 */
#define OSD_DRAW_REG_DRAW_X_MIN_02_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_02_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_02_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_02_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_02_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_02_SHIFT)

/* 0x24 : draw_partial_y_config2 */
#define OSD_DRAW_REG_DRAW_Y_MIN_02_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_02_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_02_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_02_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_02_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_02_SHIFT)

/* 0x28 : draw_partial_yuv_config3 */
#define OSD_DRAW_REG_DRAW_TH_03_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_03_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_03_SHIFT)
#define OSD_DRAW_REG_DRAW_V_03_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_03_MASK              (0xff << OSD_DRAW_REG_DRAW_V_03_SHIFT)
#define OSD_DRAW_REG_DRAW_U_03_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_03_MASK              (0xff << OSD_DRAW_REG_DRAW_U_03_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_03_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_03_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_03_SHIFT)

/* 0x2C : draw_partial_x_config3 */
#define OSD_DRAW_REG_DRAW_X_MIN_03_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_03_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_03_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_03_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_03_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_03_SHIFT)

/* 0x30 : draw_partial_y_config3 */
#define OSD_DRAW_REG_DRAW_Y_MIN_03_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_03_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_03_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_03_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_03_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_03_SHIFT)

/* 0x34 : draw_partial_yuv_config4 */
#define OSD_DRAW_REG_DRAW_TH_04_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_04_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_04_SHIFT)
#define OSD_DRAW_REG_DRAW_V_04_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_04_MASK              (0xff << OSD_DRAW_REG_DRAW_V_04_SHIFT)
#define OSD_DRAW_REG_DRAW_U_04_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_04_MASK              (0xff << OSD_DRAW_REG_DRAW_U_04_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_04_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_04_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_04_SHIFT)

/* 0x38 : draw_partial_x_config4 */
#define OSD_DRAW_REG_DRAW_X_MIN_04_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_04_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_04_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_04_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_04_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_04_SHIFT)

/* 0x3C : draw_partial_y_config4 */
#define OSD_DRAW_REG_DRAW_Y_MIN_04_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_04_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_04_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_04_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_04_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_04_SHIFT)

/* 0x40 : draw_partial_yuv_config5 */
#define OSD_DRAW_REG_DRAW_TH_05_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_05_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_05_SHIFT)
#define OSD_DRAW_REG_DRAW_V_05_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_05_MASK              (0xff << OSD_DRAW_REG_DRAW_V_05_SHIFT)
#define OSD_DRAW_REG_DRAW_U_05_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_05_MASK              (0xff << OSD_DRAW_REG_DRAW_U_05_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_05_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_05_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_05_SHIFT)

/* 0x44 : draw_partial_x_config5 */
#define OSD_DRAW_REG_DRAW_X_MIN_05_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_05_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_05_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_05_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_05_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_05_SHIFT)

/* 0x48 : draw_partial_y_config5 */
#define OSD_DRAW_REG_DRAW_Y_MIN_05_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_05_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_05_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_05_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_05_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_05_SHIFT)

/* 0x4C : draw_partial_yuv_config6 */
#define OSD_DRAW_REG_DRAW_TH_06_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_06_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_06_SHIFT)
#define OSD_DRAW_REG_DRAW_V_06_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_06_MASK              (0xff << OSD_DRAW_REG_DRAW_V_06_SHIFT)
#define OSD_DRAW_REG_DRAW_U_06_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_06_MASK              (0xff << OSD_DRAW_REG_DRAW_U_06_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_06_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_06_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_06_SHIFT)

/* 0x50 : draw_partial_x_config6 */
#define OSD_DRAW_REG_DRAW_X_MIN_06_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_06_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_06_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_06_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_06_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_06_SHIFT)

/* 0x54 : draw_partial_y_config6 */
#define OSD_DRAW_REG_DRAW_Y_MIN_06_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_06_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_06_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_06_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_06_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_06_SHIFT)

/* 0x58 : draw_partial_yuv_config7 */
#define OSD_DRAW_REG_DRAW_TH_07_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_07_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_07_SHIFT)
#define OSD_DRAW_REG_DRAW_V_07_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_07_MASK              (0xff << OSD_DRAW_REG_DRAW_V_07_SHIFT)
#define OSD_DRAW_REG_DRAW_U_07_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_07_MASK              (0xff << OSD_DRAW_REG_DRAW_U_07_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_07_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_07_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_07_SHIFT)

/* 0x5C : draw_partial_x_config7 */
#define OSD_DRAW_REG_DRAW_X_MIN_07_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_07_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_07_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_07_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_07_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_07_SHIFT)

/* 0x60 : draw_partial_y_config7 */
#define OSD_DRAW_REG_DRAW_Y_MIN_07_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_07_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_07_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_07_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_07_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_07_SHIFT)

/* 0x64 : draw_partial_yuv_config8 */
#define OSD_DRAW_REG_DRAW_TH_08_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_08_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_08_SHIFT)
#define OSD_DRAW_REG_DRAW_V_08_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_08_MASK              (0xff << OSD_DRAW_REG_DRAW_V_08_SHIFT)
#define OSD_DRAW_REG_DRAW_U_08_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_08_MASK              (0xff << OSD_DRAW_REG_DRAW_U_08_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_08_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_08_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_08_SHIFT)

/* 0x68 : draw_partial_x_config8 */
#define OSD_DRAW_REG_DRAW_X_MIN_08_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_08_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_08_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_08_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_08_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_08_SHIFT)

/* 0x6C : draw_partial_y_config8 */
#define OSD_DRAW_REG_DRAW_Y_MIN_08_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_08_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_08_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_08_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_08_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_08_SHIFT)

/* 0x70 : draw_partial_yuv_config9 */
#define OSD_DRAW_REG_DRAW_TH_09_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_09_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_09_SHIFT)
#define OSD_DRAW_REG_DRAW_V_09_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_09_MASK              (0xff << OSD_DRAW_REG_DRAW_V_09_SHIFT)
#define OSD_DRAW_REG_DRAW_U_09_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_09_MASK              (0xff << OSD_DRAW_REG_DRAW_U_09_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_09_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_09_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_09_SHIFT)

/* 0x74 : draw_partial_x_config9 */
#define OSD_DRAW_REG_DRAW_X_MIN_09_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_09_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_09_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_09_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_09_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_09_SHIFT)

/* 0x78 : draw_partial_y_config9 */
#define OSD_DRAW_REG_DRAW_Y_MIN_09_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_09_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_09_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_09_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_09_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_09_SHIFT)

/* 0x7C : draw_partial_yuv_configa */
#define OSD_DRAW_REG_DRAW_TH_0A_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_0A_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_0A_SHIFT)
#define OSD_DRAW_REG_DRAW_V_0A_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_0A_MASK              (0xff << OSD_DRAW_REG_DRAW_V_0A_SHIFT)
#define OSD_DRAW_REG_DRAW_U_0A_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_0A_MASK              (0xff << OSD_DRAW_REG_DRAW_U_0A_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_0A_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_0A_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_0A_SHIFT)

/* 0x80 : draw_partial_x_configa */
#define OSD_DRAW_REG_DRAW_X_MIN_0A_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_0A_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_0A_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_0A_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_0A_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_0A_SHIFT)

/* 0x84 : draw_partial_y_configa */
#define OSD_DRAW_REG_DRAW_Y_MIN_0A_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_0A_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_0A_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_0A_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_0A_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_0A_SHIFT)

/* 0x88 : draw_partial_yuv_configb */
#define OSD_DRAW_REG_DRAW_TH_0B_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_0B_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_0B_SHIFT)
#define OSD_DRAW_REG_DRAW_V_0B_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_0B_MASK              (0xff << OSD_DRAW_REG_DRAW_V_0B_SHIFT)
#define OSD_DRAW_REG_DRAW_U_0B_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_0B_MASK              (0xff << OSD_DRAW_REG_DRAW_U_0B_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_0B_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_0B_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_0B_SHIFT)

/* 0x8C : draw_partial_x_configb */
#define OSD_DRAW_REG_DRAW_X_MIN_0B_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_0B_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_0B_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_0B_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_0B_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_0B_SHIFT)

/* 0x90 : draw_partial_y_configb */
#define OSD_DRAW_REG_DRAW_Y_MIN_0B_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_0B_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_0B_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_0B_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_0B_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_0B_SHIFT)

/* 0x94 : draw_partial_yuv_configc */
#define OSD_DRAW_REG_DRAW_TH_0C_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_0C_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_0C_SHIFT)
#define OSD_DRAW_REG_DRAW_V_0C_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_0C_MASK              (0xff << OSD_DRAW_REG_DRAW_V_0C_SHIFT)
#define OSD_DRAW_REG_DRAW_U_0C_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_0C_MASK              (0xff << OSD_DRAW_REG_DRAW_U_0C_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_0C_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_0C_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_0C_SHIFT)

/* 0x98 : draw_partial_x_configc */
#define OSD_DRAW_REG_DRAW_X_MIN_0C_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_0C_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_0C_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_0C_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_0C_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_0C_SHIFT)

/* 0x9C : draw_partial_y_configc */
#define OSD_DRAW_REG_DRAW_Y_MIN_0C_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_0C_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_0C_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_0C_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_0C_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_0C_SHIFT)

/* 0xA0 : draw_partial_yuv_configd */
#define OSD_DRAW_REG_DRAW_TH_0D_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_0D_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_0D_SHIFT)
#define OSD_DRAW_REG_DRAW_V_0D_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_0D_MASK              (0xff << OSD_DRAW_REG_DRAW_V_0D_SHIFT)
#define OSD_DRAW_REG_DRAW_U_0D_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_0D_MASK              (0xff << OSD_DRAW_REG_DRAW_U_0D_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_0D_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_0D_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_0D_SHIFT)

/* 0xA4 : draw_partial_x_configd */
#define OSD_DRAW_REG_DRAW_X_MIN_0D_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_0D_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_0D_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_0D_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_0D_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_0D_SHIFT)

/* 0xA8 : draw_partial_y_configd */
#define OSD_DRAW_REG_DRAW_Y_MIN_0D_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_0D_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_0D_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_0D_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_0D_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_0D_SHIFT)

/* 0xAC : draw_partial_yuv_confige */
#define OSD_DRAW_REG_DRAW_TH_0E_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_0E_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_0E_SHIFT)
#define OSD_DRAW_REG_DRAW_V_0E_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_0E_MASK              (0xff << OSD_DRAW_REG_DRAW_V_0E_SHIFT)
#define OSD_DRAW_REG_DRAW_U_0E_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_0E_MASK              (0xff << OSD_DRAW_REG_DRAW_U_0E_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_0E_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_0E_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_0E_SHIFT)

/* 0xB0 : draw_partial_x_confige */
#define OSD_DRAW_REG_DRAW_X_MIN_0E_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_0E_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_0E_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_0E_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_0E_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_0E_SHIFT)

/* 0xB4 : draw_partial_y_confige */
#define OSD_DRAW_REG_DRAW_Y_MIN_0E_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_0E_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_0E_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_0E_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_0E_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_0E_SHIFT)

/* 0xB8 : draw_partial_yuv_configf */
#define OSD_DRAW_REG_DRAW_TH_0F_SHIFT            (0U)
#define OSD_DRAW_REG_DRAW_TH_0F_MASK             (0xff << OSD_DRAW_REG_DRAW_TH_0F_SHIFT)
#define OSD_DRAW_REG_DRAW_V_0F_SHIFT             (8U)
#define OSD_DRAW_REG_DRAW_V_0F_MASK              (0xff << OSD_DRAW_REG_DRAW_V_0F_SHIFT)
#define OSD_DRAW_REG_DRAW_U_0F_SHIFT             (16U)
#define OSD_DRAW_REG_DRAW_U_0F_MASK              (0xff << OSD_DRAW_REG_DRAW_U_0F_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_0F_SHIFT             (24U)
#define OSD_DRAW_REG_DRAW_Y_0F_MASK              (0xff << OSD_DRAW_REG_DRAW_Y_0F_SHIFT)

/* 0xBC : draw_partial_x_configf */
#define OSD_DRAW_REG_DRAW_X_MIN_0F_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_0F_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MIN_0F_SHIFT)
#define OSD_DRAW_REG_DRAW_X_MAX_0F_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_0F_MASK          (0xfff << OSD_DRAW_REG_DRAW_X_MAX_0F_SHIFT)

/* 0xC0 : draw_partial_y_configf */
#define OSD_DRAW_REG_DRAW_Y_MIN_0F_SHIFT         (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_0F_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MIN_0F_SHIFT)
#define OSD_DRAW_REG_DRAW_Y_MAX_0F_SHIFT         (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_0F_MASK          (0xfff << OSD_DRAW_REG_DRAW_Y_MAX_0F_SHIFT)

/* 0xF0 : osd_blend_en */
#define OSD_DRAW_CR_BLEND_EN                     (1 << 0U)

/* 0xF4 : osd_sw_shd */
#define OSD_DRAW_CR_SW_SHD                       (1 << 0U)

/* 0xF8 : osd_int_sts */
#define OSD_DRAW_REG_OSD_SEOF_INT_CLR_W          (1 << 0U)
#define OSD_DRAW_REG_OSD_SEOF_INT_MASK_W         (1 << 1U)
#define OSD_DRAW_REG_OSD_SEOF_INT_EDGE_W         (1 << 2U)
#define OSD_DRAW_STS_OSD_DRAIN_INT_R             (1 << 16U)
#define OSD_DRAW_STS_OSD_SEOF_INT_R              (1 << 20U)

/* 0xFC : osd_misc */
#define OSD_DRAW_REG_OSD_BUS_STRT_W              (1 << 0U)
#define OSD_DRAW_REG_QOS_SW_MODE_W               (1 << 4U)
#define OSD_DRAW_REG_QOS_SW_W                    (1 << 5U)
#define OSD_DRAW_REG_OSD_OSTD_MAX_W_SHIFT        (8U)
#define OSD_DRAW_REG_OSD_OSTD_MAX_W_MASK         (0x3 << OSD_DRAW_REG_OSD_OSTD_MAX_W_SHIFT)
#define OSD_DRAW_REG_XLEN_W_SHIFT                (12U)
#define OSD_DRAW_REG_XLEN_W_MASK                 (0x7 << OSD_DRAW_REG_XLEN_W_SHIFT)
#define OSD_DRAW_STS_OSD_CMD_IDLE_R              (1 << 16U)
#define OSD_DRAW_STS_OSD_AXI_IDLE_R              (1 << 20U)
#define OSD_DRAW_REG_OSD_PCLK_FORCE_ON_W_SHIFT   (24U)
#define OSD_DRAW_REG_OSD_PCLK_FORCE_ON_W_MASK    (0xff << OSD_DRAW_REG_OSD_PCLK_FORCE_ON_W_SHIFT)

#endif /* __OSD_DRAW_REG_H__ */
