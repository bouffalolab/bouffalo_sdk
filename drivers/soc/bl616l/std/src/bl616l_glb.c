/**
  ******************************************************************************
  * @file    bl616l_glb.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "bl616l_clock.h"
#include "bl616l_glb.h"
#include "bl616l_aon.h"
#include "bl616l_hbn.h"
#include "bl616l_pds.h"

/** @addtogroup  BL616L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  GLB
 *  @{
 */

/** @defgroup  GLB_Private_Macros
 *  @{
 */
#define GLB_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }
#define GLB_CLK_SET_DUMMY_TIMEOUT (0xFF)
#define GLB_FREQ_RC32M            (32000000)

/*@} end of group GLB_Private_Macros */

/** @defgroup  GLB_Private_Types
 *  @{
 */

static BL_Err_Type GLB_Get_And_Clr_First_Set_From_U64(uint64_t *val, uint32_t *bit);

/*@} end of group GLB_Private_Types */

/** @defgroup  GLB_Private_Variables
 *  @{
 */

/* WiFi PLL Config*/
const GLB_WIFIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifipllBasicCfgNormal_32M_38P4M_40M = {
    .refdivRatio = 2,  /*!< refclk divide ratio, Fref=Fxtal/wifipll_refclk_div_ratio */
    .vcoSpeed = 5,     /*!< 5:825M~1.2GHz */
    .vcoIdacExtra = 2, /*!< wifipll_resv1 */
    .vco480mEn = 0,    /*!< 1: wifipll enable low power mode */
    .sdmBypass = 1,    /*!< hardware control, 1'b1 bypass sdm, integer-N pll */
    .dtcRSel = 0,      /*!< dtc resolution select, 0: 36.4p~58.8p, 1:43.57p~70.56p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 1,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
};

const GLB_WIFIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifipllBasicCfgNormal_24M = {
    .refdivRatio = 1,  /*!< refclk divide ratio, Fref=Fxtal/wifipll_refclk_div_ratio */
    .vcoSpeed = 5,     /*!< 5:825M~1.2GHz */
    .vcoIdacExtra = 2, /*!< wifipll_resv1 */
    .vco480mEn = 0,    /*!< 1: wifipll enable low power mode */
    .sdmBypass = 1,    /*!< hardware control, 1'b1 bypass sdm, integer-N pll */
    .dtcRSel = 0,      /*!< dtc resolution select, 0: 36.4p~58.8p, 1:43.57p~70.56p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 1,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
};

const GLB_WIFIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifipllBasicCfgNormal_26M = {
    .refdivRatio = 1,  /*!< refclk divide ratio, Fref=Fxtal/wifipll_refclk_div_ratio */
    .vcoSpeed = 5,     /*!< 5:825M~1.2GHz */
    .vcoIdacExtra = 2, /*!< wifipll_resv1 */
    .vco480mEn = 0,    /*!< 1: wifipll enable low power mode */
    .sdmBypass = 0,    /*!< hardware control, 1'b1 bypass sdm, integer-N pll */
    .dtcRSel = 0,      /*!< dtc resolution select, 0: 36.4p~58.8p, 1:43.57p~70.56p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 1,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
};

const GLB_WIFIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifipllBasicCfgLowpower_32M_38P4M_40M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/wifipll_refclk_div_ratio */
    .vcoSpeed = 3,     /*!< 5:825M~1.2GHz */
    .vcoIdacExtra = 0, /*!< wifipll_resv1 */
    .vco480mEn = 1,    /*!< 1: wifipll enable low power mode */
    .sdmBypass = 1,    /*!< hardware control, 1'b1 bypass sdm, integer-N pll */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 36.4p~58.8p, 1:43.57p~70.56p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 0,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
};

const GLB_WIFIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifipllBasicCfgLowpower_24M = {
    .refdivRatio = 2,  /*!< refclk divide ratio, Fref=Fxtal/wifipll_refclk_div_ratio */
    .vcoSpeed = 3,     /*!< 5:825M~1.2GHz */
    .vcoIdacExtra = 0, /*!< wifipll_resv1 */
    .vco480mEn = 1,    /*!< 1: wifipll enable low power mode */
    .sdmBypass = 1,    /*!< hardware control, 1'b1 bypass sdm, integer-N pll */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 36.4p~58.8p, 1:43.57p~70.56p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 0,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
};

const GLB_WIFIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION wifipllBasicCfgLowpower_26M = {
    .refdivRatio = 2,  /*!< refclk divide ratio, Fref=Fxtal/wifipll_refclk_div_ratio */
    .vcoSpeed = 3,     /*!< 5:825M~1.2GHz */
    .vcoIdacExtra = 0, /*!< wifipll_resv1 */
    .vco480mEn = 1,    /*!< 1: wifipll enable low power mode */
    .sdmBypass = 0,    /*!< hardware control, 1'b1 bypass sdm, integer-N pll */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 36.4p~58.8p, 1:43.57p~70.56p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 0,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
};

const GLB_WIFIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifipllCfgNormal_960M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                                      /*!< XTAL is None */
    { &wifipllBasicCfgNormal_24M, 0x500000 },           /*!< XTAL is 24M */
    { &wifipllBasicCfgNormal_32M_38P4M_40M, 0x780000 }, /*!< XTAL is 32M */
    { &wifipllBasicCfgNormal_32M_38P4M_40M, 0x640000 }, /*!< XTAL is 38.4M */
    { &wifipllBasicCfgNormal_32M_38P4M_40M, 0x600000 }, /*!< XTAL is 40M */
    { &wifipllBasicCfgNormal_26M, 0x49D89D },           /*!< XTAL is 26M */
    { &wifipllBasicCfgNormal_32M_38P4M_40M, 0x780000 }, /*!< XTAL is RC32M */
};

const GLB_WIFIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION wifipllCfgLowpower_960M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                                        /*!< XTAL is None */
    { &wifipllBasicCfgLowpower_24M, 0x500000 },           /*!< XTAL is 24M */
    { &wifipllBasicCfgLowpower_32M_38P4M_40M, 0x780000 }, /*!< XTAL is 32M */
    { &wifipllBasicCfgLowpower_32M_38P4M_40M, 0x640000 }, /*!< XTAL is 38.4M */
    { &wifipllBasicCfgLowpower_32M_38P4M_40M, 0x600000 }, /*!< XTAL is 40M */
    { &wifipllBasicCfgLowpower_26M, 0x49D89D },           /*!< XTAL is 26M */
    { &wifipllBasicCfgLowpower_32M_38P4M_40M, 0x780000 }, /*!< XTAL is RC32M */
};

const GLB_SLAVE_GRP_0_TBL_Type ATTR_CLOCK_CONST_SECTION glb_slave_grp_0_table[GLB_SLAVE_GRP_0_MAX] = {
    { GLB_ADC_CFG0_OFFSET, GLB_GPADC_32M_DIV_EN_POS, GLB_GPADC_32M_CLK_SEL_POS, GLB_GPADC_32M_CLK_DIV_POS, GLB_GPADC_32M_DIV_EN_LEN, GLB_GPADC_32M_CLK_SEL_LEN, GLB_GPADC_32M_CLK_DIV_LEN },
    { GLB_IR_CFG0_OFFSET, GLB_IR_CLK_EN_POS, 0, GLB_IR_CLK_DIV_POS, GLB_IR_CLK_EN_LEN, 0, GLB_IR_CLK_DIV_LEN },
    { GLB_I2C_CFG0_OFFSET, GLB_I2C_CLK_EN_POS, GLB_I2C_CLK_SEL_POS, GLB_I2C_CLK_DIV_POS, GLB_I2C_CLK_EN_LEN, GLB_I2C_CLK_SEL_LEN, GLB_I2C_CLK_DIV_LEN },
    { GLB_SPI_CFG0_OFFSET, GLB_SPI_CLK_EN_POS, GLB_SPI_CLK_SEL_POS, GLB_SPI_CLK_DIV_POS, GLB_SPI_CLK_EN_LEN, GLB_SPI_CLK_SEL_LEN, GLB_SPI_CLK_DIV_LEN },
    { GLB_DBI_CFG0_OFFSET, GLB_DBI_CLK_EN_POS, GLB_DBI_CLK_SEL_POS, GLB_DBI_CLK_DIV_POS, GLB_DBI_CLK_EN_LEN, GLB_DBI_CLK_SEL_LEN, GLB_DBI_CLK_DIV_LEN },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_AUTO_DIV_EN_POS, 0, 0, GLB_REG_AUDIO_AUTO_DIV_EN_LEN, 0, 0 },
    { GLB_AUDIO_CFG0_OFFSET, GLB_REG_AUDIO_ADC_CLK_EN_POS, 0, GLB_REG_AUDIO_ADC_CLK_DIV_POS, GLB_REG_AUDIO_ADC_CLK_EN_LEN, 0, GLB_REG_AUDIO_ADC_CLK_DIV_LEN },
    { GLB_AUDIO_CFG1_OFFSET, GLB_REG_AUDIO_SOLO_CLK_EN_POS, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_POS, GLB_REG_AUDIO_SOLO_CLK_EN_LEN, 0, GLB_REG_AUDIO_SOLO_CLK_DIV_LEN },
    { GLB_CAM_CFG0_OFFSET, GLB_REG_CAM_REF_CLK_EN_POS, GLB_REG_CAM_REF_CLK_SRC_SEL_POS, GLB_REG_CAM_REF_CLK_DIV_POS, GLB_REG_CAM_REF_CLK_EN_LEN, GLB_REG_CAM_REF_CLK_SRC_SEL_LEN, GLB_REG_CAM_REF_CLK_DIV_LEN },
    { GLB_SDH_CFG0_OFFSET, GLB_REG_SDH_CLK_EN_POS, GLB_REG_SDH_CLK_SEL_POS, GLB_REG_SDH_CLK_DIV_POS, GLB_REG_SDH_CLK_EN_LEN, GLB_REG_SDH_CLK_SEL_LEN, GLB_REG_SDH_CLK_DIV_LEN },
    { GLB_PSRAM_CFG0_OFFSET, GLB_REG_PSRAMB_CLK_EN_POS, GLB_REG_PSRAMB_CLK_SEL_POS, GLB_REG_PSRAMB_CLK_DIV_POS, GLB_REG_PSRAMB_CLK_EN_LEN, GLB_REG_PSRAMB_CLK_SEL_LEN, GLB_REG_PSRAMB_CLK_DIV_LEN },
};

/*@} end of group GLB_Private_Variables */

/** @defgroup  GLB_Global_Variables
 *  @{
 */

/*@} end of group GLB_Global_Variables */

/** @defgroup  GLB_Private_Fun_Declaration
 *  @{
 */

/*@} end of group GLB_Private_Fun_Declaration */

/** @defgroup  GLB_Private_Functions
 *  @{
 */

/*@} end of group GLB_Private_Functions */

/** @defgroup  GLB_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  set mcu muxpll 160M selection
 *
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_MCU_MUXPLL_160M_SEL_WIFIPLL_160M
 *           @arg GLB_MCU_MUXPLL_160M_SEL_TOP_WIFIPLL_240M
 *           @arg GLB_MCU_MUXPLL_160M_SEL_WIFIPLL_120M
 *           @arg GLB_MCU_MUXPLL_160M_SEL_WIFIPLL_96M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_MCU_Muxpll_160M_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_MCU_MUXPLL_160M_CLK_SEL_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get mcu muxpll 160M selection
 *
 * @param  None
 *
 * @return 160M mux select value
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION GLB_Get_MCU_Muxpll_160M_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
    return (uint8_t)(BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL));
}

/****************************************************************************/ /**
 * @brief  set top muxpll 80M selection
 *
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_MCU_MUXPLL_80M_SEL_WIFIPLL_80M
 *           @arg GLB_MCU_MUXPLL_80M_SEL_WIFIPLL_96M
 *           @arg GLB_MCU_MUXPLL_80M_SEL_WIFIPLL_48M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_MCU_Muxpll_80M_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_MCU_MUXPLL_80M_CLK_SEL_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_80M_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get mcu muxpll 80M selection
 *
 * @param  None
 *
 * @return 80M mux select value
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION GLB_Get_MCU_Muxpll_80M_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
    return (uint8_t)(BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_80M_SEL));
}

/****************************************************************************/ /**
 * @brief  set isp muxpll 80M selection
 *
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_ISP_MUXPLL_80M_SEL_WIFIPLL_80M_CGEN
 *           @arg GLB_ISP_MUXPLL_80M_SEL_WIFIPLL_120M_CGEN
 *           @arg GLB_ISP_MUXPLL_80M_SEL_WIFIPLL_96M_CGEN
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_ISP_Muxpll_80M_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_ISP_MUXPLL_80M_CLK_SEL_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_ISP_MUXPLL_80M_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get isp muxpll 80M selection
 *
 * @param  None
 *
 * @return 80M mux select value
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION GLB_Get_ISP_Muxpll_80M_Sel(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
    return (uint8_t)(BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_ISP_MUXPLL_80M_SEL));
}

/****************************************************************************/ /**
 * @brief  power on all PLL clock
 *
 * @param  xtalType: XTAL frequency type, this parameter can be one of the following values:
 *           @arg GLB_XTAL_NONE
 *           @arg GLB_XTAL_24M
 *           @arg GLB_XTAL_32M
 *           @arg GLB_XTAL_38P4M
 *           @arg GLB_XTAL_40M
 *           @arg GLB_XTAL_26M
 *           @arg GLB_XTAL_RC32M
 *           @arg GLB_XTAL_MAX
 * @param  pllType: only power on xtal, this parameter can be one of the following values or 'or' of them:
 *           @arg GLB_PLL_NONE
 *           @arg GLB_PLL_WIFIPLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllType)
{
    uint32_t tmpVal;
    uint8_t mcuXclkSel;
    uint8_t mcuRootClkSel;

    CHECK_PARAM(IS_GLB_XTAL_TYPE(xtalType));
    CHECK_PARAM((pllType < (GLB_PLL_WIFIPLL * 2)));

    if (xtalType == GLB_XTAL_NONE) {
        return ERROR;
    }

    if (xtalType != GLB_XTAL_RC32M) {
        /* power on xtal first */
        AON_Power_On_XTAL();
    }

#if defined(DUAL_CORE)
    uint8_t coreType;
    coreType = GLB_Get_Core_Type();
    if ((GLB_CORE_ID_M0 == coreType)) {
#endif
        /* M0 */
        /* Before config XTAL and PLL ,make sure root clk is from RC32M */
        mcuXclkSel = HBN_Get_MCU_XCLK_Sel();
        mcuRootClkSel = HBN_Get_MCU_Root_CLK_Sel();
        if ((mcuXclkSel != HBN_MCU_XCLK_RC32M) || (mcuRootClkSel != HBN_MCU_ROOT_CLK_XCLK)) {
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
        }
        GLB_Set_MCU_System_CLK_Div(0, 0);
#if defined(DUAL_CORE)
    }
#endif

    HBN_Set_Xtal_Type(xtalType);

    if (GLB_PLL_NONE == pllType) {
        GLB_CLK_SET_DUMMY_WAIT;
        return SUCCESS;
    }

    /* power on wifipll */
    if (pllType & GLB_PLL_WIFIPLL) {
        GLB_Power_Off_WIFIPLL();
        GLB_WIFIPLL_Ref_Clk_Sel(GLB_WIFIPLL_REFCLK_XTAL_SOC);
        GLB_Power_On_WIFIPLL(&wifipllCfgNormal_960M[xtalType], 0);
    }

    arch_delay_us(75);

    if (xtalType != GLB_XTAL_RC32M) {
        /* if power on xtal, always set xclk from xtal */
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
    }

    /* enable all PLL clock output */
    /* GLB reg_pll_en = 1, cannot be zero */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_PLL_EN);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG0, tmpVal);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power off wifipll
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_Off_WIFIPLL(void)
{
    uint32_t tmpVal = 0;

    /* cfg0 : pu_wifipll=0 */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_WIFIPLL, 0);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB wifipll ref clock select
 *
 * @param  refClk: PLL ref clock select, this parameter can be one of the following values:
 *           @arg GLB_WIFIPLL_REFCLK_XTAL_SOC
 *           @arg GLB_WIFIPLL_REFCLK_XTAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_WIFIPLL_Ref_Clk_Sel(uint8_t refClk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_WIFIPLL_REF_CLK_TYPE(refClk));

    /* wifipll_refclk_sel */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_REFCLK_SEL, refClk);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power on wifipll
 *
 * @param  cfg: GLB PLL configuration
 * @param  waitStable: wait PLL clock stable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_WIFIPLL(const GLB_WIFIPLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t tmpVal = 0;

    /* Step1:config parameter */
    /* set refdiv_ratio, vco_speed, dtc_r_sel */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_REFCLK_DIV_RATIO, cfg->basicCfg->refdivRatio);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_VCO_480M_EN, cfg->basicCfg->vco480mEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_VCO_SPEED, cfg->basicCfg->vcoSpeed);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_VCO_IDAC_EXTRA, cfg->basicCfg->vcoIdacExtra);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_DTC_R_SEL, cfg->basicCfg->dtcRSel);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* set spd_gain */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_SPD_FCAL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SPD_GAIN, cfg->basicCfg->spdGain);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_SPD_FCAL_OFFSET, tmpVal);

    /* set alpha_base, alpha_exp, alpha_fast, beta_base, beta_exp, beta_fast */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_LF_VCTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_LF_ALPHA_BASE, cfg->basicCfg->lfAlphaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_LF_ALPHA_EXP, cfg->basicCfg->lfAlphaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_LF_ALPHA_FAST, cfg->basicCfg->lfAlphaFast);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_LF_BETA_BASE, cfg->basicCfg->lfBetaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_LF_BETA_EXP, cfg->basicCfg->lfBetaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_LF_BETA_FAST, cfg->basicCfg->lfBetaFast);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_LF_VCTRL_OFFSET, tmpVal);

    /* set wifipll_sdmin */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_SDMIN_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_IN, cfg->sdmin);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_SDMIN_OFFSET, tmpVal);

    /* set sdm_bypass */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_PI_SDM_LMS_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_SDM_BYPASS, cfg->basicCfg->sdmBypass);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_PI_SDM_LMS_OFFSET, tmpVal);

    /* Step2:config pu */
    /* pu_wifipll=1, wifipll_rstb=1 */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_WIFIPLL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_RSTB, 1);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 4us */
    arch_delay_us(6);

    /* wifipll_rstb=0 */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_RSTB, 0);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 1us */
    arch_delay_us(2);

    /* wifipll_rstb=1 */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WIFIPLL_RSTB, 1);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* Step3:enable output clock */
    /* wifipll clock divider enable */
    tmpVal = BL_RD_WORD(GLB_BASE + GLB_WIFIPLL_CLKTREE_DIG_OFFSET);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV1);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV3);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV4);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV5);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV6);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV8);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV10);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV12);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV20);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV30);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_USB2_DIV2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_USB2_DIV48);
    BL_WR_WORD(GLB_BASE + GLB_WIFIPLL_CLKTREE_DIG_OFFSET, tmpVal);

    if (waitStable) {
        /* Wait 1.5*180us    */
        arch_delay_us(270);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set System clock divider
 *
 * @param  mcuClkDiv: HCLK divider
 * @param  mcuPBclkDiv: BCLK divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout = 1024;
    uint8_t xclkSel;
    uint8_t rootClkSel;

    /* get root clock */
    xclkSel = HBN_Get_MCU_XCLK_Sel();
    rootClkSel = HBN_Get_MCU_Root_CLK_Sel();

    if ((xclkSel != HBN_MCU_XCLK_RC32M) || (rootClkSel != HBN_MCU_ROOT_CLK_XCLK)) {
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
        HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    }

    /* config hclk_div=mcuClkDiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG0, tmpVal);

    /* config bclk_div=mcuPBclkDiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG0, tmpVal);
    /* bclk act pulse */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG1, tmpVal);

    timeout = 1024;
    do {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG1);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    /* recover root clock */
    HBN_Set_MCU_XCLK_Sel(xclkSel);
    HBN_Set_MCU_Root_CLK_Sel(rootClkSel);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get System clock divider
 *
 * @param  mcuClkDiv: HCLK divider
 * @param  mcuPBclkDiv: BCLK divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv)
{
    if ((NULL == mcuClkDiv) || (NULL == mcuPBclkDiv)) {
        return ERROR;
    }

    *mcuClkDiv = BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SYS_CFG0), GLB_REG_HCLK_DIV);
    *mcuPBclkDiv = BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SYS_CFG0), GLB_REG_BCLK_DIV);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set mcu System clock
 *
 * @param  clkFreq: mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_MCU_SYS_CLK_RC32M
 *           @arg GLB_MCU_SYS_CLK_XTAL
 *           @arg GLB_MCU_SYS_CLK_WIFIPLL_96M
 *           @arg GLB_MCU_SYS_CLK_WIFIPLL_192M
 *           @arg GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M
 *           @arg GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_MCU_System_CLK(uint8_t clkFreq)
{
    uint32_t tmpVal;
    uint8_t mcuXclkSel;

    CHECK_PARAM(IS_GLB_MCU_SYS_CLK_TYPE(clkFreq));

    /* get xclk&&rootclk clock */
    mcuXclkSel = HBN_Get_MCU_XCLK_Sel();

    /* change root clock to rc32m */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* select pll output clock before select root clock */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_WIFIPLL_96M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_WIFIPLL_192M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 1);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 2);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    /* select root clock */
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_RC32M:
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            break;
        case GLB_MCU_SYS_CLK_WIFIPLL_96M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            // GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_WIPIPLL_96M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_WIFIPLL_192M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 1);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            // GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_192M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 1);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 1);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get core type
 *
 * @param  None
 *
 * @return core type
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION GLB_Get_Core_Type(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_WORD(CORE_ID_ADDRESS);

    switch (tmpVal) {
        case CORE_ID_M0:
            return GLB_CORE_ID_M0;
        default:
            return GLB_CORE_ID_INVALID;
    }

    return GLB_CORE_ID_INVALID;
}

/****************************************************************************/ /**
 * @brief  set CPU reset address
 *
 * @param  coreID: core type, this parameter can be one of the following values:
 *           @arg GLB_CORE_ID_M0
 * @param  addr: reset address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr)
{
    CHECK_PARAM(IS_GLB_CORE_ID_TYPE(coreID));

    switch (coreID) {
        case GLB_CORE_ID_M0:
            BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG14, addr);
            break;
        default:
            return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Clr_EMI_Reset_Gate(void)
{
    uint32_t tmpVal;

    /* clear EMI swrst bit */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SWRST_S1_EXT_EMI_MISC);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set ADC clock
 *
 * @param  enable: enable frequency divider or not
 * @param  clkSel: ADC clock selection, this parameter can be one of the following values:
 *           @arg GLB_ADC_CLK_WIFIPLL_32M
 *           @arg GLB_ADC_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_ADC_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3F));

    /* disable ADC clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ADC_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_GPADC_32M_DIV_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_ADC_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DMA clock
 *
 * @param  enable: Enable or disable
 * @param  clk: DMA clock type, this parameter can be one of the following values:
 *           @arg GLB_DMA0_CLK_CH0
 *           @arg GLB_DMA0_CLK_CH1
 *           @arg GLB_DMA0_CLK_CH2
 *           @arg GLB_DMA0_CLK_CH3
 *           @arg GLB_DMA0_CLK_CH4
 *           @arg GLB_DMA0_CLK_CH5
 *           @arg GLB_DMA0_CLK_CH6
 *           @arg GLB_DMA0_CLK_CH7
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, uint8_t clk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_GLB_DMA_CLK_ID_TYPE(clk));

    /* DMA0 */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DMA_CFG0);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, GLB_DMA_CLK_EN);
    if (enable) {
        tmpVal2 |= (1 << clk);
    } else {
        tmpVal2 &= (~(1 << clk));
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DMA_CLK_EN, tmpVal2);
    BL_WR_REG(GLB_BASE, GLB_DMA_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set IR clock divider
 *
 * @param  enable: enable or disable IR clock
 * @param  clkSel: IR clock type, this parameter can be one of the following values:
 *           @arg GLB_IR_CLK_SRC_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_IR_CLK_SRC_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3F));

    (void)clkSel;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_IR_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_IR_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_IR_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select ir rx gpio (gpio9~gpio23)
 *
 * @param  gpio: IR gpio selected, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_RX_GPIO_Sel(uint8_t gpio)
{
    uint32_t tmpVal = 0;

    /* Select gpio between gpio9 and gpio23 */
    if (gpio > 8 && gpio < 24) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_IR_RX_GPIO_SEL, gpio - 8);
        BL_WR_REG(GLB_BASE, GLB_IR_CFG1, tmpVal);
    }

    /* Close ir rx */
    if (gpio == 0) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_IR_RX_GPIO_SEL, 0);
        BL_WR_REG(GLB_BASE, GLB_IR_CFG1, tmpVal);
    } else {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set UART clock
 *
 * @param  enable: Enable or disable UART clock
 * @param  clkSel: UART clock type, this parameter can be one of the following values:
 *           @arg HBN_UART_CLK_MCU_BCLK
 *           @arg HBN_UART_CLK_MUXPLL_160M
 *           @arg HBN_UART_CLK_XCLK
 * @param  div: UART clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0xFF));
    CHECK_PARAM(IS_HBN_UART_CLK_TYPE(clkSel));

    /* disable UART clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_UART_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG0, tmpVal);

    /* Set div */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_UART_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG0, tmpVal);

    /* Select clock source for uart */
    HBN_Set_UART_CLK_Sel(clkSel);

    /* Set enable or disable */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_UART_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_UART_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_UART_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select UART signal function
 *
 * @param  sig: UART signal, this parameter can be GLB_UART_SIG_xx where xx is 0~11
 * @param  fun: UART function, this parameter can be one of the following values:
 *           @arg GLB_UART_SIG_FUN_UART0_RTS
 *           @arg GLB_UART_SIG_FUN_UART0_CTS
 *           @arg GLB_UART_SIG_FUN_UART0_TXD
 *           @arg GLB_UART_SIG_FUN_UART0_RXD
 *           @arg GLB_UART_SIG_FUN_UART1_RTS
 *           @arg GLB_UART_SIG_FUN_UART1_CTS
 *           @arg GLB_UART_SIG_FUN_UART1_TXD
 *           @arg GLB_UART_SIG_FUN_UART1_RXD
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_UART_Fun_Sel(uint8_t sig, uint8_t fun)
{
    uint32_t sig_pos = 0;
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_UART_SIG_TYPE(sig));
    CHECK_PARAM(IS_GLB_UART_SIG_FUN_TYPE(fun));

    if (sig < GLB_UART_SIG_8) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG1);
        sig_pos = (sig * 4);
        /* Clear original val */
        tmpVal &= (~(0xf << sig_pos));
        /* Set new value */
        tmpVal |= (fun << sig_pos);
        BL_WR_REG(GLB_BASE, GLB_UART_CFG1, tmpVal);
    } else {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_UART_CFG2);
        sig_pos = ((sig - 8) * 4);
        /* Clear original val */
        tmpVal &= (~(0xf << sig_pos));
        /* Set new value */
        tmpVal |= (fun << sig_pos);
        BL_WR_REG(GLB_BASE, GLB_UART_CFG2, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set sflash clock
 *
 * @param  enable: enable or disable sflash clock
 * @param  clkSel: sflash clock type, this parameter can be one of the following values:
 *           @arg GLB_SFLASH_CLK_WIFIPLL_120M
 *           @arg GLB_SFLASH_CLK_XTAL
 *           @arg GLB_SFLASH_CLK_WIFIPLL_96M
 *           @arg GLB_SFLASH_CLK_MUXPLL_80M
 *           @arg GLB_SFLASH_CLK_BCLK
 *           @arg GLB_SFLASH_CLK_WIFIPLL_160M
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_SF_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SFLASH_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x7));

    /* disable SFLASH clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SF_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SF_CFG0, tmpVal);

    /* clock divider */
    /* Select flash clock, all Flash CLKs are divied by PLL_480M */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV, div);
    switch (clkSel) {
        case GLB_SFLASH_CLK_WIFIPLL_120M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL2, 0x0);
            break;
        case GLB_SFLASH_CLK_XTAL:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL2, 0x1);
            break;
        case GLB_SFLASH_CLK_WIFIPLL_96M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL2, 0x3);
            break;
        case GLB_SFLASH_CLK_MUXPLL_80M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x1);
            break;
        case GLB_SFLASH_CLK_BCLK:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x2);
            break;
        case GLB_SFLASH_CLK_WIFIPLL_160M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x3);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_SF_CFG0, tmpVal);

    /* enable or disable flash clock */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SF_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SF_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SF_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set I2C clock
 *
 * @param  enable: Enable or disable I2C clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_I2C_CLK_BCLK
 *           @arg GLB_I2C_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_I2C_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2C_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_I2C_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_I2C_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set I2S clock
 *
 * @param  refClkEn: ref clock ENABLE or DISABLE
 * @param  refClkDiv: divider
 * @param  inRef: di ref clock, this parameter can be one of the following values:
 *           @arg GLB_I2S_DI_SEL_I2S_DI_INPUT
 *           @arg GLB_I2S_DI_SEL_I2S_REF_OUTPUT
 * @param  outRef: do ref clock, this parameter can be one of the following values:
 *           @arg GLB_I2S_DO_SEL_I2S_DO_OUTPT
 *           @arg GLB_I2S_DO_SEL_I2S_REF_OUTPUT
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((refClkDiv <= 0x3F));
    CHECK_PARAM(IS_GLB_I2S_DI_REF_CLK_TYPE(inRef));
    CHECK_PARAM(IS_GLB_I2S_DO_REF_CLK_TYPE(outRef));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_I2S_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_I2S_DO_REF_CLK_SEL, outRef);
    if (refClkEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_I2S_REF_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_I2S_REF_CLK_EN);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_I2S_DI_REF_CLK_SEL, inRef);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_I2S_REF_CLK_DIV, refClkDiv);
    BL_WR_REG(GLB_BASE, GLB_I2S_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set SPI0 clock
 *
 * @param  enable: Enable or disable SPI0 clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_SPI_CLK_MCU_MUXPLL_160M
 *           @arg GLB_SPI_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI0_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SPI_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set SPI1 clock
 *
 * @param  enable: Enable or disable SPI1 clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_SPI_CLK_MCU_MUXPLL_160M
 *           @arg GLB_SPI_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI1_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SPI_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI1_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI1_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI1_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SPI1_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SPI1_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap SPI0 gpio pins sig function
 *
 * @param  group: SPI0 swap set group, this parameter can be one of the following values:
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45
 * @param  swap: swap or no swap
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SPI0_Sig_Swap_Set(uint8_t group, uint8_t swap)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;

    CHECK_PARAM(IS_GLB_SPI_SIG_SWAP_GRP_TYPE(group));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_SWAP_SET);
    if (swap) {
        tmpVal2 |= (1 << group);
    } else {
        tmpVal2 &= ~(1 << group);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI_SWAP_SET, tmpVal2);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap SPI1 gpio pins sig function
 *
 * @param  group: SPI1 swap set group, this parameter can be one of the following values:
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45
 * @param  swap: swap or no swap
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SPI1_Sig_Swap_Set(uint8_t group, uint8_t swap)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;

    CHECK_PARAM(IS_GLB_SPI_SIG_SWAP_GRP_TYPE(group));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SPI_CFG0);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI1_SWAP_SET);
    if (swap) {
        tmpVal2 |= (1 << group);
    } else {
        tmpVal2 &= ~(1 << group);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SPI1_SWAP_SET, tmpVal2);
    BL_WR_REG(GLB_BASE, GLB_SPI_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PWM1 clock
 *
 * @param  ioSel: io select, this parameter can be one of the following values:
 *           @arg GLB_PWM1_IO_SINGLE_END
 *           @arg GLB_PWM1_IO_DIFF_END
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PWM1_IO_Sel(uint8_t ioSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PWM1_IO_SEL_TYPE(ioSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PWM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PWM1_IO_SEL, ioSel);
    BL_WR_REG(GLB_BASE, GLB_PWM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PWM clock
 *
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_PWM_CLK_BCLK
 *           @arg GLB_PWM_CLK_XCLK
 *           @arg GLB_PWM_CLK_F32K
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PWM_CLK(uint8_t clkSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PWM_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0xFFFF));

    /* stop pwm */
    tmpVal = *((volatile uint32_t *)0x2000A440);
    tmpVal |= ((uint32_t)1 << 27);
    *((volatile uint32_t *)0x2000A440) = tmpVal;

    tmpVal = *((volatile uint32_t *)0x2000A440);
    /* set pwm_clk_sel */
    tmpVal &= ~((uint32_t)0x3 << 30);
    tmpVal |= ((uint32_t)clkSel << 30);
    (*((volatile uint32_t *)0x2000A440)) = tmpVal;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PDM clock
 *
 * @param  ioSel: io select, this parameter can be one of the following values:
 *           @arg GLB_PDM_IO_SEL_AUDIO_TOP
 *           @arg GLB_PDM_IO_SEL_AUPDM_TOP
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PDM_IO_Sel(uint8_t ioSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PDM_IO_SEL_TYPE(ioSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PDM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PDM_IO_SEL, ioSel);
    BL_WR_REG(GLB_BASE, GLB_PDM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DBI clock
 *
 * @param  enable: Enable or disable DBI clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_DBI_CLK_MCU_MUXPLL_160M
 *           @arg GLB_DBI_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DBI_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DBI_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DBI_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DBI_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DBI_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DBI_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DBI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DBI_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_DBI_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select DIG clock source
 *
 * @param  clkSel: DIG clock selection, this parameter can be one of the following values:
 *           @arg GLB_DIG_CLK_WIFIPLL_32M
 *           @arg GLB_DIG_CLK_XCLK
 *           @arg GLB_DIG_CLK_WIFIPLL_160M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DIG_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal;
    uint32_t dig512kEn;
    uint32_t dig32kEn;

    CHECK_PARAM(IS_GLB_DIG_CLK_TYPE(clkSel));

    /* disable DIG512K and DIG32K clock first */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    dig512kEn = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_512K_EN);
    dig32kEn = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_EN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_512K_EN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_32K_EN);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    /* repristinate DIG512K and DIG32K clock */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_512K_EN, dig512kEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_EN, dig32kEn);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DIG 512K clock
 *
 * @param  enable: enable or disable DIG 512K clock
 * @param  compensationEn: enable or disable DIG 512K clock compensation
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div)
{
    uint32_t tmpVal;

    CHECK_PARAM((div <= 0x7F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    if (compensationEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DIG_512K_COMP);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_512K_COMP);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_512K_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DIG_512K_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_512K_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set DIG 32K clock
 *
 * @param  enable: enable or disable DIG 32K clock
 * @param  compensationEn: enable or disable DIG 32K clock compensation
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div)
{
    uint32_t tmpVal;

    CHECK_PARAM((div <= 0x7FF));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    if (compensationEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DIG_32K_COMP);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_32K_COMP);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_DIG_32K_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_DIG_32K_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_DIG_32K_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  platform wakeup becomes one of  pds_wakeup source
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Platform_Wakeup_PDS_Enable(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_EN_PLATFORM_WAKEUP);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_EN_PLATFORM_WAKEUP);
    }
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  mcu gpio timer clock select
 *
 * @param  gpioPin: gpio pin number, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Sel_MCU_TMR_GPIO_Clock(uint8_t gpioPin)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    switch (gpioPin & 0x3) {
        case 0: /* inout_sig_0 */
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_EN, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 0);
            break;
        case 1: /* inout_sig_1 */
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_EN, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 1);
            break;
        case 2: /* inout_sig_2 */
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_2_EN, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 2);
            break;
        case 3: /* inout_sig_3 */
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_3_EN, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_GPIO_TMR_CLK_SEL, 3);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out0 select
 *
 * @param  clkOutType: chip clock out0 output type, this parameter can be one of the following values:
 *           @arg GLB_CHIP_CLK_OUT_0_CAM_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_0_I2S_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_0_NONE
 *           @arg GLB_CHIP_CLK_OUT_0_SOLO_IN_128FS
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out0_Sel(uint8_t clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out0 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_0_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out1 select
 *
 * @param  clkOutType: chip clock out1 output type, this parameter can be one of the following values:
 *           @arg GLB_CHIP_CLK_OUT_1_CAM_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_1_I2S_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_1_NONE
 *           @arg GLB_CHIP_CLK_OUT_1_SOLO_IN_128FS
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out1_Sel(uint8_t clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out1 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_1_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out2 select
 *
 * @param  clkOutType: chip clock out2 output type, this parameter can be one of the following values:
 *           @arg GLB_CHIP_CLK_OUT_2_CAM_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_2_I2S_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_2_ANA_XTAL_CLK
 *           @arg GLB_CHIP_CLK_OUT_2_WIFIPLL_32M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out2_Sel(uint8_t clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out2 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_2_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_2_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  chip clock out3 select
 *
 * @param  clkOutType: chip clock out3 output type, this parameter can be one of the following values:
 *           @arg GLB_CHIP_CLK_OUT_3_CAM_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_3_I2S_REF_CLK
 *           @arg GLB_CHIP_CLK_OUT_3_NONE
 *           @arg GLB_CHIP_CLK_OUT_3_WIFIPLL_48M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Chip_Clock_Out3_Sel(uint8_t clkOutType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG2);
    /* set clk out3 output enable */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_3_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CHIP_CLK_OUT_3_SEL, clkOutType);
    BL_WR_REG(GLB_BASE, GLB_DIG_CLK_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set audio auto clock
 *
 * @param  divEn: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Audio_AUTO_CLK(uint8_t divEn)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    if (divEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_AUTO_DIV_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set audio SOLO clock
 *
 * @param  enable: ENABLE or DISABLE
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0xFF));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_SOLO_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG1);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_SOLO_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set isp_core clock
 *
 * @param  enable: Enable or disable isp_core clock
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_ISP_CORE_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0xF));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ISP_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_ISP_CORE_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_ISP_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ISP_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_ISP_CORE_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_ISP_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ISP_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_ISP_CORE_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_ISP_CORE_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_ISP_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  invert eth rx clock
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Invert_ETH_RX_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ETH_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CFG_INV_ETH_RX_CLK);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CFG_INV_ETH_RX_CLK);
    }
    BL_WR_REG(GLB_BASE, GLB_ETH_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  invert eth tx clock
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Invert_ETH_TX_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ETH_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CFG_INV_ETH_TX_CLK);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CFG_INV_ETH_TX_CLK);
    }
    BL_WR_REG(GLB_BASE, GLB_ETH_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  invert eth ref clock
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Invert_ETH_REF_O_CLK(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ETH_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CFG_INV_ETH_REF_CLK_O);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CFG_INV_ETH_REF_CLK_O);
    }
    BL_WR_REG(GLB_BASE, GLB_ETH_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set eth ref clock select
 *
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_ETH_REF_CLK_OUT_OUTSIDE_50M
 *           @arg GLB_ETH_REF_CLK_OUT_INSIDE_50M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_ETH_REF_O_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_ETH_REF_CLK_OUT_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_ETH_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_SEL_ETH_REF_CLK_O, clkSel);
    BL_WR_REG(GLB_BASE, GLB_ETH_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set CAM clock
 *
 * @param  enable: Enable or disable CAM clock
 * @param  clkSel: CAM clock type, this parameter can be one of the following values:
 *           @arg GLB_CAM_CLK_XCLK
 *           @arg GLB_CAM_CLK_WIFIPLL_96M
 *           @arg GLB_CAM_CLK_WIFIPLL_120M
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_CAM_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CAM_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CAM_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CAM_REF_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_CAM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set SDH clock
 *
 * @param  enable: Enable or disable
 * @param  clkSel: SDH clock type, this parameter can be one of the following values:
 *           @arg GLB_SDH_CLK_WIFIPLL_96M
 *           @arg GLB_SDH_CLK_WIFIPLL_120M
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SDH_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_SDH_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x7));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SDH_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SDH_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SDH_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDH_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_SDH_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SDH_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SDH_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PEC clock
 *
 * @param  enable: Enable or disable
 * @param  clkSel: PEC clock type, this parameter can be one of the following values:
 *           @arg GLB_PEC_CLK_MCU_MUXPLL_160M
 *           @arg GLB_PEC_CLK_XCLK
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PEC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PEC_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x31));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set AHB2QSPI clock
 *
 * @param  enable: Enable or disable
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_AHB2QSPI_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 1023));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AHB2QSPI_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_QSPI_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_AHB2QSPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AHB2QSPI_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_QSPI_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_AHB2QSPI_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AHB2QSPI_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_QSPI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_QSPI_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AHB2QSPI_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable or Disable Host cmd0 reset
 *
 * @param  enable: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Config_SDIO_Host_Cmd0_Reset(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDIO_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_HOST_CMD0_RST_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_HOST_CMD0_RST_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SDIO_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable or Disable Host config 0x6[3] to reset System
 *
 * @param  enable: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Config_SDIO_Host_Reset_System(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDIO_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_SYS_RST_SD_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SYS_RST_SD_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SDIO_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable or Disable Host config 0x6[3] to reset SDU sdu_rst_ sd_rst_
 *
 * @param  enable: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Config_SDIO_Host_Reset_SDU(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDIO_CFG0);
    if (enable) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SDU_RST_SD_DIS);
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SD_RST_SD_DIS);
    } else {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_SDU_RST_SD_DIS);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_SD_RST_SD_DIS);
    }
    BL_WR_REG(GLB_BASE, GLB_SDIO_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable or Disable Host config 0x7[4] to interrupt CPU
 *
 * @param  enable: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SDIO_CFG0);
    if (enable) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_SDIO_INT_SYS_DIS);
    } else {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_SDIO_INT_SYS_DIS);
    }
    BL_WR_REG(GLB_BASE, GLB_SDIO_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap UART gpio pins sig function
 *
 * @param  group: UART swap set group, this parameter can be one of the following values:
 *           @arg GLB_UART_SIG_SWAP_GRP_GPIO0_GPIO11
 *           @arg GLB_UART_SIG_SWAP_GRP_GPIO12_GPIO23
 *           @arg GLB_UART_SIG_SWAP_GRP_GPIO24_GPIO35
 *           @arg GLB_UART_SIG_SWAP_GRP_GPIO36_GPIO45
 * @param  swap: swap or no swap
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_UART_Sig_Swap_Set(uint8_t group, uint8_t swap)
{
    uint32_t tmpVal = 0;
    uint32_t tmpVal2 = 0;

    CHECK_PARAM(IS_GLB_UART_SIG_SWAP_GRP_TYPE(group));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, GLB_UART_SWAP_SET);
    if (swap) {
        tmpVal2 |= (1 << group);
    } else {
        tmpVal2 &= ~(1 << group);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_UART_SWAP_SET, tmpVal2);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap SPI0 MOSI with MISO
 *
 * @param  newState: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Swap_MCU_SPI_0_MOSI_With_MISO(BL_Fun_Type newState)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SPI_0_SWAP, newState);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap SPI1 MOSI with MISO
 *
 * @param  newState: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Swap_MCU_SPI_1_MOSI_With_MISO(BL_Fun_Type newState)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SPI_1_SWAP, newState);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select SPI0 act mode
 *
 * @param  mod: SPI0 work mode, this parameter can be one of the following values:
 *           @arg GLB_SPI_PAD_ACT_AS_SLAVE
 *           @arg GLB_SPI_PAD_ACT_AS_MASTER
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_MCU_SPI_0_ACT_MOD_Sel(uint8_t mod)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_SPI_PAD_ACT_AS_TYPE(mod));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SPI_0_MASTER_MODE, mod);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select SPI1 act mode
 *
 * @param  mod: SPI1 work mode, this parameter can be one of the following values:
 *           @arg GLB_SPI_PAD_ACT_AS_SLAVE
 *           @arg GLB_SPI_PAD_ACT_AS_MASTER
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_MCU_SPI_1_ACT_MOD_Sel(uint8_t mod)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_GLB_SPI_PAD_ACT_AS_TYPE(mod));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_SPI_1_MASTER_MODE, mod);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set flash io parameter
 *
 * @param  selEmbedded: Select embedded flash pin
 * @param  swapIo3Io0: Select embedded flash swap io0 with io3
 * @param  swapIo2Cs: Select embedded flash swap cs with io2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    if (selEmbedded) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SEL_EMBEDDED_SFLASH);
        if (swapIo3Io0) {
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWAP_SFLASH_IO_3_IO_0);
        } else {
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SWAP_SFLASH_IO_3_IO_0);
        }
        if (swapIo2Cs) {
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWAP_SFLASH_IO_2_CS);
        } else {
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SWAP_SFLASH_IO_2_CS);
        }
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SEL_EMBEDDED_SFLASH);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWAP_SFLASH_IO_3_IO_0);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWAP_SFLASH_IO_2_CS);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    if (selEmbedded) {
        GLB_Embedded_Flash_Pad_Enable(swapIo2Cs);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set sflash2 io parameter
 *
 * @param  swapIo3Io0: Select embedded flash swap io0 with io3
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    if (swapIo3Io0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SWAP_SFLASH2_IO_3_IO_0);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SWAP_SFLASH2_IO_3_IO_0);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable or disable use psram pad for flash
 *
 * @param  enable: Enable or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Set_Psram_Pad_For_Flash(uint8_t enable)
{
    uint32_t tmpVal;
    uint32_t gpioCfgAddress;
    uint8_t gpioPin[4];
    uint8_t i;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PSRAM_PAD_FOR_FLASH);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PSRAM_PAD_FOR_FLASH);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    if (enable) {
        gpioPin[0] = 56;
        gpioPin[1] = 54;
        gpioPin[2] = 51;
        gpioPin[3] = 47;

        for (i = 0; i < 4; i++) {
            gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin[i] << 2);
            tmpVal = BL_RD_WORD(gpioCfgAddress);
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
            BL_WR_WORD(gpioCfgAddress, tmpVal);
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  clear reset recorder
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Clr_Reset_Reason(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_RESET_STS0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_TOP_RESET_RECORDER);
    BL_WR_REG(GLB_BASE, GLB_RESET_STS0, tmpVal);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_RESET_STS0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CLR_TOP_RESET_RECORDER);
    BL_WR_REG(GLB_BASE, GLB_RESET_STS0, tmpVal);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_RESET_STS0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_CLR_TOP_RESET_RECORDER);
    BL_WR_REG(GLB_BASE, GLB_RESET_STS0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get reset reason
 *
 * @param  reason: store reset reason
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason)
{
    uint32_t tmpVal;

    CHECK_PARAM((NULL != reason));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_RESET_STS0);
    reason->reset_recorder_ana_powb = (tmpVal >> 0) & 0x1;
    reason->reset_recorder_ext_rst_n = (tmpVal >> 1) & 0x1;
    reason->reset_recorder_pds_reset = (tmpVal >> 2) & 0x1;
    reason->reset_recorder_wdt_rst_n = (tmpVal >> 3) & 0x1;
    reason->reset_recorder_cpu_porst_n = (tmpVal >> 4) & 0x1;
    reason->reset_recorder_sys_reset_n = (tmpVal >> 5) & 0x1;
    reason->reset_recorder_cpu_sys_rstreq_n = (tmpVal >> 6) & 0x1;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  software reset
 *
 * @param  swrst: reset num, this parameter can be one of the following values:
 *           @arg GLB_AHB_MCU_SW_WIFI
 *           @arg GLB_AHB_MCU_SW_BTDM
 *           @arg GLB_AHB_MCU_SW_BLE2
 *           @arg GLB_AHB_MCU_SW_EMI_MISC
 *           @arg GLB_AHB_MCU_SW_MM_MISC
 *           @arg GLB_AHB_MCU_SW_PSRAM1_CTRL
 *           @arg GLB_AHB_MCU_SW_AUDAC
 *           @arg GLB_AHB_MCU_SW_SDH
 *           @arg GLB_AHB_MCU_SW_EMAC
 *           @arg GLB_AHB_MCU_SW_JDEC
 *           @arg GLB_AHB_MCU_SW_D2XA
 *           @arg GLB_AHB_MCU_SW_D2XB
 *           @arg GLB_AHB_MCU_SW_JENC
 *           @arg GLB_AHB_MCU_SW_GLB
 *           @arg GLB_AHB_MCU_SW_MIX
 *           @arg GLB_AHB_MCU_SW_GPIP
 *           @arg GLB_AHB_MCU_SW_SEC_DBG
 *           @arg GLB_AHB_MCU_SW_SEC_ENG
 *           @arg GLB_AHB_MCU_SW_TZ1
 *           @arg GLB_AHB_MCU_SW_EFUSE
 *           @arg GLB_AHB_MCU_SW_GPIP2
 *           @arg GLB_AHB_MCU_SW_SF
 *           @arg GLB_AHB_MCU_SW_DMA
 *           @arg GLB_AHB_MCU_SW_SDU
 *           @arg GLB_AHB_MCU_SW_PDS
 *           @arg GLB_AHB_MCU_SW_UART0
 *           @arg GLB_AHB_MCU_SW_UART1
 *           @arg GLB_AHB_MCU_SW_SPI0
 *           @arg GLB_AHB_MCU_SW_I2C0
 *           @arg GLB_AHB_MCU_SW_PWM
 *           @arg GLB_AHB_MCU_SW_TIMER
 *           @arg GLB_AHB_MCU_SW_IR_REMOTE
 *           @arg GLB_AHB_MCU_SW_CHECKSUM
 *           @arg GLB_AHB_MCU_SW_DBI
 *           @arg GLB_AHB_MCU_SW_I2C1
 *           @arg GLB_AHB_MCU_SW_CAN
 *           @arg GLB_AHB_MCU_SW_I2S
 *           @arg GLB_AHB_MCU_SW_AUADC
 *           @arg GLB_AHB_MCU_SW_UART2
 *           @arg GLB_AHB_MCU_SW_SPI1
 *           @arg GLB_AHB_MCU_SW_PWRON_RST
 *           @arg GLB_AHB_MCU_SW_CPU_RESET
 *           @arg GLB_AHB_MCU_SW_SYS_RESET
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_AHB_MCU_Software_Reset(uint8_t swrst)
{
    uint32_t tmpVal = 0;
    uint32_t bit = 0;
    uint32_t regAddr = 0;

    CHECK_PARAM(IS_GLB_AHB_MCU_SW_TYPE(swrst));

    if (swrst < 32) {
        bit = swrst;
        regAddr = GLB_BASE + GLB_SWRST_CFG0_OFFSET;
    } else if (swrst < 64) {
        bit = swrst - 32;
        regAddr = GLB_BASE + GLB_SWRST_CFG1_OFFSET;
    } else if (swrst < 96) {
        bit = swrst - 64;
        regAddr = GLB_BASE + GLB_SWRST_CFG2_OFFSET;
    }

    tmpVal = BL_RD_WORD(regAddr);
    tmpVal &= ~(1 << bit);
    BL_WR_WORD(regAddr, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_WORD(regAddr);
    tmpVal |= (1 << bit);
    BL_WR_WORD(regAddr, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_WORD(regAddr);
    tmpVal &= ~(1 << bit);
    BL_WR_WORD(regAddr, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  dis reset
 *
 * @param  enable: ENABLE or DISABLE
 * @param  disrst: disrst macro, this parameter can be one of the following values:
 *           @arg GLB_DISRST_GPIP
 *           @arg GLB_DISRST_SEC_ENG
 *           @arg GLB_DISRST_CCI
 *           @arg GLB_DISRST_GPIP2
 *           @arg GLB_DISRST_SF
 *           @arg GLB_DISRST_UART0
 *           @arg GLB_DISRST_UART1
 *           @arg GLB_DISRST_SPI0
 *           @arg GLB_DISRST_I2C0
 *           @arg GLB_DISRST_PWM
 *           @arg GLB_DISRST_TIMER
 *           @arg GLB_DISRST_IR_REMOTE
 *           @arg GLB_DISRST_CHECKSUM
 *           @arg GLB_DISRST_DBI
 *           @arg GLB_DISRST_I2C1
 *           @arg GLB_DISRST_I2S
 *           @arg GLB_DISRST_AUADC
 *           @arg GLB_DISRST_UART2
 *           @arg GLB_DISRST_SPI1
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Disrst_Set(uint8_t enable, uint8_t disrst)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_DISRST_TYPE(disrst));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG3);
    if (enable) {
        tmpVal |= (1 << disrst);
    } else {
        tmpVal &= ~(1 << disrst);
    }
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get first 1 from u64, then clear it
 *
 * @param  val: target value
 * @param  bit: first 1 in bit
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type GLB_Get_And_Clr_First_Set_From_U64(uint64_t *val, uint32_t *bit)
{
    if (!*val) {
        return ERROR;
    }

    for (uint8_t i = 0; i < 64; i++) {
        if ((*val) & ((uint64_t)1 << i)) {
            *bit = i;
            (*val) &= ~((uint64_t)1 << i);
            break;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  hold IPs clock
 *
 * @param  ips: GLB_AHB_CLOCK_CPU | GLB_AHB_CLOCK_RSVD | ...... (not GLB_AHB_CLOCK_IP_xxx)
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_PER_Clock_Gate(uint64_t ips)
{
    /* api request from cjy */

    uint32_t tmpValCfg0 = 0;
    uint32_t tmpValCfg1 = 0;
    uint32_t tmpValCfg2 = 0;
    uint32_t bitfield = 0;

    tmpValCfg0 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG0);
    tmpValCfg1 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpValCfg2 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    while (ips) {
        if (SUCCESS == GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            switch (bitfield) {
                case GLB_AHB_CLOCK_IP_CPU:
                    tmpValCfg0 &= ~(1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_RSVD:
                    tmpValCfg0 &= ~(1 << 1);
                    break;
                case GLB_AHB_CLOCK_IP_SEC:
                    tmpValCfg1 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA:
                    tmpValCfg1 &= ~(1 << 12);
                    break;
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_RF_TOP:
                    tmpValCfg1 &= ~(1 << 1);
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 &= ~(1 << 2);
                    tmpValCfg1 &= ~(1 << 15);
                    break;
                case GLB_AHB_CLOCK_IP_TZC:
                    tmpValCfg1 &= ~(1 << 5);
                    break;
                case GLB_AHB_CLOCK_IP_EF_CTRL:
                    tmpValCfg1 &= ~(1 << 7);
                    break;
                case GLB_AHB_CLOCK_IP_SF_CTRL:
                    tmpValCfg1 &= ~(1 << 11);
                    break;
                case GLB_AHB_CLOCK_IP_EMAC:
                    tmpValCfg2 &= ~(1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_UART0:
                    tmpValCfg1 &= ~(1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_UART1:
                    tmpValCfg1 &= ~(1 << 17);
                    break;
                case GLB_AHB_CLOCK_IP_UART2:
                    tmpValCfg1 &= ~(1 << 29);
                    break;
                case GLB_AHB_CLOCK_IP_SPI0:
                    tmpValCfg1 &= ~(1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_SPI1:
                    tmpValCfg1 &= ~(1 << 30);
                    break;
                case GLB_AHB_CLOCK_IP_I2C0:
                    tmpValCfg1 &= ~(1 << 19);
                    break;
                case GLB_AHB_CLOCK_IP_I2C1:
                    tmpValCfg1 &= ~(1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_PWM:
                    tmpValCfg1 &= ~(1 << 20);
                    break;
                case GLB_AHB_CLOCK_IP_TIMER:
                    tmpValCfg1 &= ~(1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_IR:
                    tmpValCfg1 &= ~(1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_CHECKSUM:
                    tmpValCfg1 &= ~(1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    break;
                case GLB_AHB_CLOCK_IP_I2S:
                    tmpValCfg1 &= ~(1 << 27);
                    break;
                case GLB_AHB_CLOCK_IP_CAM:
                    break;
                case GLB_AHB_CLOCK_IP_MJPEG:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_LP:
                    break;
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_NORMAL:
                    tmpValCfg2 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_LP:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_EMI_MISC:
                    tmpValCfg2 &= ~(1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM1_CTRL:
                    tmpValCfg2 &= ~(1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_USB:
                    tmpValCfg1 &= ~(1 << 14);
                    break;
                case GLB_AHB_CLOCK_IP_SDU:
                    tmpValCfg1 &= ~(1 << 13);
                    break;
                case GLB_AHB_CLOCK_IP_MIX2:
                    break;
                case GLB_AHB_CLOCK_IP_AUDIO:
                    tmpValCfg2 &= ~(1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_SDH:
                    tmpValCfg2 &= ~(1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_MAC_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PLATFORM:
                    break;
                case GLB_AHB_CLOCK_IP_LZ4:
                    break;
                case GLB_AHB_CLOCK_IP_DBI:
                    tmpValCfg1 &= ~(1 << 24);
                    break;
                case GLB_AHB_CLOCK_IP_AUADC:
                    tmpValCfg1 &= ~(1 << 28);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_GPIO:
                    tmpValCfg1 &= ~(1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_MM_MISC:
                    tmpValCfg2 &= ~(1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_PEC:
                    tmpValCfg2 &= ~(1 << 25);
                    break;
                default:
                    break;
            }
        }
    }
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpValCfg0);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpValCfg1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpValCfg2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  release IPs clock
 *
 * @param  ips: GLB_AHB_CLOCK_CPU | GLB_AHB_CLOCK_RSVD | ...... (not GLB_AHB_CLOCK_IP_xxx)
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_PER_Clock_UnGate(uint64_t ips)
{
    /* api request from cjy */

    uint32_t tmpValCfg0 = 0;
    uint32_t tmpValCfg1 = 0;
    uint32_t tmpValCfg2 = 0;
    uint32_t bitfield = 0;

    tmpValCfg0 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG0);
    tmpValCfg1 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpValCfg2 = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    while (ips) {
        if (SUCCESS == GLB_Get_And_Clr_First_Set_From_U64(&ips, &bitfield)) {
            switch (bitfield) {
                case GLB_AHB_CLOCK_IP_CPU:
                    tmpValCfg0 |= (1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_RSVD:
                    tmpValCfg0 |= (1 << 1);
                    break;
                case GLB_AHB_CLOCK_IP_SEC:
                    tmpValCfg1 |= (1 << 3);
                    tmpValCfg1 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA:
                    tmpValCfg1 |= (1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_RF_TOP:
                    tmpValCfg1 |= (1 << 1);
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 |= (1 << 2);
                    tmpValCfg1 |= (1 << 15);
                    break;
                case GLB_AHB_CLOCK_IP_TZC:
                    tmpValCfg1 |= (1 << 5);
                    break;
                case GLB_AHB_CLOCK_IP_EF_CTRL:
                    tmpValCfg1 |= (1 << 7);
                    break;
                case GLB_AHB_CLOCK_IP_SF_CTRL:
                    tmpValCfg1 |= (1 << 11);
                    break;
                case GLB_AHB_CLOCK_IP_EMAC:
                    tmpValCfg2 |= (1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_UART0:
                    tmpValCfg1 |= (1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_UART1:
                    tmpValCfg1 |= (1 << 17);
                    break;
                case GLB_AHB_CLOCK_IP_UART2:
                    tmpValCfg1 |= (1 << 29);
                    break;
                case GLB_AHB_CLOCK_IP_SPI0:
                    tmpValCfg1 |= (1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_SPI1:
                    tmpValCfg1 |= (1 << 30);
                    break;
                case GLB_AHB_CLOCK_IP_I2C0:
                    tmpValCfg1 |= (1 << 19);
                    break;
                case GLB_AHB_CLOCK_IP_I2C1:
                    tmpValCfg1 |= (1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_PWM:
                    tmpValCfg1 |= (1 << 20);
                    break;
                case GLB_AHB_CLOCK_IP_TIMER:
                    tmpValCfg1 |= (1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_IR:
                    tmpValCfg1 |= (1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_CHECKSUM:
                    tmpValCfg1 |= (1 << 23);
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    break;
                case GLB_AHB_CLOCK_IP_I2S:
                    tmpValCfg1 |= (1 << 27);
                    break;
                case GLB_AHB_CLOCK_IP_CAM:
                    break;
                case GLB_AHB_CLOCK_IP_MJPEG:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_LP:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_NORMAL:
                    tmpValCfg2 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_LP:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_EMI_MISC:
                    tmpValCfg2 |= (1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM1_CTRL:
                    tmpValCfg2 |= (1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_USB:
                    tmpValCfg1 |= (1 << 14);
                    break;
                case GLB_AHB_CLOCK_IP_SDU:
                    tmpValCfg1 |= (1 << 13);
                    break;
                case GLB_AHB_CLOCK_IP_MIX2:
                    break;
                case GLB_AHB_CLOCK_IP_AUDIO:
                    tmpValCfg2 |= (1 << 21);
                    break;
                case GLB_AHB_CLOCK_IP_SDH:
                    tmpValCfg2 |= (1 << 22);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_MAC_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PLATFORM:
                    break;
                case GLB_AHB_CLOCK_IP_LZ4:
                    break;
                case GLB_AHB_CLOCK_IP_DBI:
                    tmpValCfg1 |= (1 << 24);
                    break;
                case GLB_AHB_CLOCK_IP_AUADC:
                    tmpValCfg1 |= (1 << 28);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_GPIO:
                    tmpValCfg1 |= (1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_MM_MISC:
                    tmpValCfg2 |= (1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_PEC:
                    tmpValCfg2 |= (1 << 25);
                    break;
                default:
                    break;
            }
        }
    }
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpValCfg0);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpValCfg1);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpValCfg2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  gate pll clock cgen
 *
 * @param  clk: pll clock cgen, this parameter can be one of the following values:
 *           @arg GLB_PLL_CGEN_ISP_WIFIPLL_80M
 *           @arg GLB_PLL_CGEN_ISP_WIFIPLL_120M
 *           @arg GLB_PLL_CGEN_ISP_WIFIPLL_96M
 *           @arg GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M
 *           @arg GLB_PLL_CGEN_PSRAMB_WIFIPLL_240M
 *           @arg GLB_PLL_CGEN_TOP_WIFIPLL_240M
 *           @arg GLB_PLL_CGEN_TOP_WIFIPLL_320M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_PLL_CGEN_Clock_Gate(uint8_t clk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PLL_CGEN_TYPE(clk));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG3);
    tmpVal &= (~(1 << clk));
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  gate pll clock cgen
 *
 * @param  clk: pll clock cgen, this parameter can be one of the following values:
 *           @arg GLB_PLL_CGEN_ISP_WIFIPLL_80M
 *           @arg GLB_PLL_CGEN_ISP_WIFIPLL_120M
 *           @arg GLB_PLL_CGEN_ISP_WIFIPLL_96M
 *           @arg GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M
 *           @arg GLB_PLL_CGEN_PSRAMB_WIFIPLL_240M
 *           @arg GLB_PLL_CGEN_TOP_WIFIPLL_240M
 *           @arg GLB_PLL_CGEN_TOP_WIFIPLL_320M
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_PLL_CGEN_Clock_UnGate(uint8_t clk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PLL_CGEN_TYPE(clk));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG3);
    tmpVal |= (1 << clk);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select PKA clock source
 *
 * @param  clkSel: PKA clock selection, this parameter can be one of the following values:
 *           @arg GLB_PKA_CLK_MCU_BCLK
 *           @arg GLB_PKA_CLK_MCU_MUXPLL_160M
 *           @arg GLB_PKA_CLK_WIFIPLL_320M
 *
 * @return SUCCESS or ERROR
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_PKA_CLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PKA_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PKA_CLK_SEL, clkSel);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  mcu system part reset
 *
 * @param  sysPart: mcu reset part, this parameter can be one of the following values:
 *           @arg GLB_MCU_SW_SYSTEM_CTRL_MCU
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_MCU_SW_System_Reset(uint8_t sysPart)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_MCU_SW_SYSTEM_TYPE(sysPart));

    /* reset=0 */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal &= ~(1 << sysPart);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
    BL_DRV_DUMMY;
    /* reset=1 */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal |= (1 << sysPart);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
    BL_DRV_DUMMY;
    /* reset=1 */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal &= ~(1 << sysPart);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Software system reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_SW_System_Reset(void)
{
    uint32_t tmpVal;

    /* Swicth MCU clock to RC32M, no divider */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* reset M0/SYS(bus) */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_SYS_RESET);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Software CPU reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_SW_CPU_Reset(void)
{
    uint32_t tmpVal;

#if defined(DUAL_CORE)
    uint8_t core = GLB_CORE_ID_INVALID;
    /* Do reset */
    core = GLB_Get_Core_Type();
    switch (core) {
        case GLB_CORE_ID_M0:
#endif
            /* Swicth MCU clock to RC32M, no divider */
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
            GLB_Set_MCU_System_CLK_Div(0, 0);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
#if defined(DUAL_CORE)
            break;
        default:
            break;
    }
#endif

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Software power on reset
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_SW_POR_Reset(void)
{
    uint32_t tmpVal;

    /* Swicth MCU clock to RC32M, no divider */
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* Do reset */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_PWRON_RST);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    /* waiting for reset */
    while (1) {
        arch_delay_us(10);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set auto calc xtal type value
 *
 * @param  calcXtalType: auto calc xtal type, this parameter can be one of the following values:
 *           @arg GLB_XTAL_NONE
 *           @arg GLB_XTAL_24M
 *           @arg GLB_XTAL_32M
 *           @arg GLB_XTAL_38P4M
 *           @arg GLB_XTAL_40M
 *           @arg GLB_XTAL_26M
 *           @arg GLB_XTAL_RC32M
 *           @arg GLB_XTAL_MAX
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Auto_Calc_Xtal_Type(uint8_t calcXtalType)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_XTAL_TYPE(calcXtalType));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_HW_RSV0);
    tmpVal = tmpVal & 0xffff0000;
    tmpVal |= GLB_AUTO_CALC_XTAL_FLAG_VALUE;
    tmpVal |= calcXtalType;
    BL_WR_REG(GLB_BASE, GLB_HW_RSV0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set auto calc xtal type value
 *
 * @param  calcXtalType: auto calc xtal type, this parameter can be one of the following values:
 *           @arg GLB_XTAL_NONE
 *           @arg GLB_XTAL_24M
 *           @arg GLB_XTAL_32M
 *           @arg GLB_XTAL_38P4M
 *           @arg GLB_XTAL_40M
 *           @arg GLB_XTAL_26M
 *           @arg GLB_XTAL_RC32M
 *           @arg GLB_XTAL_MAX
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Get_Auto_Calc_Xtal_Type(uint8_t *calcXtalType)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_HW_RSV0);
    if ((tmpVal & GLB_AUTO_CALC_XTAL_FLAG_MASK) == GLB_AUTO_CALC_XTAL_FLAG_VALUE) {
        *calcXtalType = (tmpVal & 0xff);
        return SUCCESS;
    } else {
        *calcXtalType = 0;
        return ERROR;
    }
}

/****************************************************************************/ /**
 * @brief  set flash id value
 *
 * @param  idValue: flash id value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Set_Flash_Id_Value(uint32_t idValue)
{
    BL_WR_REG(GLB_BASE, GLB_HW_RSV1, ((idValue & 0xFFFFFF) | 0x5A000000));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get flash id value
 *
 * @param  None
 *
 * @return flash id
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION GLB_Get_Flash_Id_Value(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_HW_RSV1);
    if ((tmpVal & 0x7F000000) == 0x5A000000) {
        return (tmpVal & 0x00FFFFFF);
    }

    return 0x00000000;
}

/****************************************************************************/ /**
 * @brief  trim LDO18IO vout sel
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Trim_Ldo18ioVoutSel(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18_sel", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_VOUT_SEL_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  trim LDO18IO vout
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Trim_Ldo18ioVoutTrim(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  power down LDO18IO vout
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION GLB_Power_Down_Ldo18ioVout(void)
{
    uint32_t tmpVal = 0;

    /* set pu_ldo18io_aon = 0 */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_LDO18IO_AON);
    BL_WR_REG(HBN_BASE, HBN_LDO_2, tmpVal);
}

/****************************************************************************/ /**
 * @brief  power up LDO18IO vout
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION GLB_Power_Up_Ldo18ioVout(void)
{
    uint32_t tmpVal = 0;

    /* set pu_ldo18io_aon = 0 */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_LDO_2);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_LDO18IO_AON);
    BL_WR_REG(HBN_BASE, HBN_LDO_2, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set sram_ret value
 *
 * @param  value: value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SRAM_RET(uint32_t value)
{
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG0, value);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get sram_ret value
 *
 * @param  None
 *
 * @return value
 *
*******************************************************************************/
uint32_t GLB_Get_SRAM_RET(void)
{
    return BL_RD_REG(GLB_BASE, GLB_SRAM_CFG0);
}

/****************************************************************************/ /**
 * @brief  set sram_slp value
 *
 * @param  value: value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SRAM_SLP(uint32_t value)
{
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG1, value);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get sram_slp value
 *
 * @param  None
 *
 * @return value
 *
*******************************************************************************/
uint32_t GLB_Get_SRAM_SLP(void)
{
    return BL_RD_REG(GLB_BASE, GLB_SRAM_CFG1);
}

/****************************************************************************/ /**
 * @brief  set sram_param value
 *
 * @param  value: value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SRAM_PARM(uint32_t value)
{
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG2, value);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get sram_parm value
 *
 * @param  None
 *
 * @return value
 *
*******************************************************************************/
uint32_t GLB_Get_SRAM_PARM(void)
{
    return BL_RD_REG(GLB_BASE, GLB_SRAM_CFG2);
}

/****************************************************************************/ /**
 * @brief  select EM type
 *
 * @param  emType: EM type, this parameter can be one of the following values:
 *           @arg GLB_WRAM160KB_EM0KB,
 *           @arg GLB_WRAM144KB_EM16KB,
 *           @arg GLB_WRAM128KB_EM32KB,
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_EM_Sel(uint8_t emType)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_EM_TYPE(emType));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SRAM_CFG3);
    switch (emType) {
        case GLB_WRAM160KB_EM0KB:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00);
            break;
        case GLB_WRAM144KB_EM16KB:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x01);
            break;
        case GLB_WRAM128KB_EM32KB:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03);
            break;
        default:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x01);
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PSRAMB clock
 *
 * @param  enable: Enable or disable
 * @param  clkSel: PSRAMB clock type, this parameter can be one of the following values:
 *           @arg GLB_PSRAMB_EMI_WIFIPLL_320M
 *           @arg GLB_PSRAMB_EMI_WIFIPLL_240M
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PSRAMB_PLL_TYPE(clkSel));
    CHECK_PARAM((div <= 0x3));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_PSRAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PSRAMB_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PSRAMB_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_PSRAM_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PSRAM_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_PSRAMB_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_PSRAM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  reconfigure WIFIPLL clock
 *
 * @param  xtalType: XTAL frequency type, this parameter can be one of the following values:
 *           @arg GLB_XTAL_NONE, should not used as para
 *           @arg GLB_XTAL_24M
 *           @arg GLB_XTAL_32M
 *           @arg GLB_XTAL_38P4M
 *           @arg GLB_XTAL_40M
 *           @arg GLB_XTAL_26M
 *           @arg GLB_XTAL_RC32M, should not used as para
 *           @arg GLB_XTAL_MAX
 * @param  pllCfg: PLL configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_WIFIPLL(uint8_t xtalType, const GLB_WIFIPLL_Cfg_Type *pllCfgList)
{
    GLB_Power_Off_WIFIPLL();
    GLB_WIFIPLL_Ref_Clk_Sel(GLB_WIFIPLL_REFCLK_XTAL_SOC);
    GLB_Power_On_WIFIPLL(&(pllCfgList[xtalType]), 1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set mcu System clock Simple
 *
 * @param  clkFreq: mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_MCU_SYS_CLK_RC32M
 *           @arg GLB_MCU_SYS_CLK_XTAL
 *           @arg GLB_MCU_SYS_CLK_WIFIPLL_96M
 *           @arg GLB_MCU_SYS_CLK_WIFIPLL_192M
 *           @arg GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M
 *           @arg GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M
 * @param  mcuClkDiv: HCLK divider
 * @param  mcuPBclkDiv: BCLK divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout;

    CHECK_PARAM(IS_GLB_MCU_SYS_CLK_TYPE(clkFreq));

    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);

    /* select pll output clock before select root clock */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_WIFIPLL_96M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_WIFIPLL_192M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 1);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 2);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 3);
            break;
        default:
            break;
    }
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    /* clock div */
    /* config hclk_div=mcuClkDiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_HCLK_DIV, mcuClkDiv);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG0, tmpVal);
    /* config bclk_div=mcuPBclkDiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV, mcuPBclkDiv);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG0, tmpVal);
    /* bclk act pulse */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_BCLK_DIV_ACT_PULSE, 1);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG1, tmpVal);
    timeout = 1024;
    do {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG1);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    /* select root clock */
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_RC32M:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            break;
        case GLB_MCU_SYS_CLK_WIFIPLL_96M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            // GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_96M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_WIFIPLL_192M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            // GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_192M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_240M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_320M);
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        default:
            break;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Embedded flash set input function enable
 *
 * @param  swapIo2Cs: Select embedded flash swap cs with io2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs)
{
    uint32_t gpioCfgAddress;
    uint32_t tmpVal;
    uint8_t gpioPin[4];
    uint8_t i;

    if (swapIo2Cs) {
        gpioPin[0] = 39;
        gpioPin[1] = 40;
        gpioPin[2] = 38;
        gpioPin[3] = 42;
    } else {
        gpioPin[0] = 39;
        gpioPin[1] = 40;
        gpioPin[2] = 41;
        gpioPin[3] = 42;
    }
    for (i = 0; i < 4; i++) {
        gpioCfgAddress = GLB_BASE + GLB_GPIO_CFG0_OFFSET + (gpioPin[i] << 2);
        tmpVal = BL_RD_WORD(gpioCfgAddress);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
        BL_WR_WORD(gpioCfgAddress, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable sflash write protect
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SF_Write_Protect_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_WR_PROT_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SF_WR_PROT_DIS);
    BL_WR_REG(GLB_BASE, GLB_SF_WR_PROT_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable sflash write protect
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SF_Write_Protect_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_WR_PROT_CFG);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SF_WR_PROT_DIS);
    BL_WR_REG(GLB_BASE, GLB_SF_WR_PROT_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set sflash protect region
 *
 * @param  region: select one region, 0~3 is available
 * @param  cmd: allowed command for selected region
 * @param  addr_begin: begin flash address for selected region
 * @param  addr_end: end flash address for selected region
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SF_Write_Protect_Set(uint8_t region, uint8_t cmd, uint32_t address, uint32_t length)
{
    uint32_t tmpVal;

    if (region > 3) {
        return ERROR;
    }
    /* address must be 256 bytes aligned, begin address align downward, end address align upward */
    /* config address */
    tmpVal = BL_RD_REG(GLB_BASE + region * 4, GLB_SF_SEG0_ADDR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_SEG0_BEGIN, address >> 8);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_SEG0_END, (address + length + 0xFF) >> 8);
    BL_WR_REG(GLB_BASE + region * 4, GLB_SF_SEG0_ADDR_CFG, tmpVal);
    /* config command */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_WR_CMD_CFG);
    tmpVal &= ~(0xFF << (region * 8));
    tmpVal |= (cmd << (region * 8));
    BL_WR_REG(GLB_BASE, GLB_SF_WR_CMD_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Write glb reserved register
 *
 * @param  index: select one register, 0~3 is available
 * @param  value: value to be write to selected register
 *
 * @return NONE
 *
*******************************************************************************/
void GLB_Write_SW_Reserved_Register(uint8_t index, uint32_t value)
{
    if (index > 3) {
        return;
    }
    BL_WR_REG(GLB_BASE + index * 4, GLB_SW_RSV0, value);
}

uint32_t GLB_Read_SW_Reserved_Register(uint8_t index)
{
    if (index > 3) {
        return 0;
    }
    return BL_RD_REG(GLB_BASE + index * 4, GLB_SW_RSV0);
}

/****************************************************************************/ /**
 * @brief  Enable bus decoder error response
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void GLB_Bus_Decoder_Err_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(MCU_MISC_BASE, MCU_MISC_MCU_BUS_CFG0);
    tmpVal |= MCU_MISC_DEC_ERR_RSP_MSK;
    BL_WR_REG(MCU_MISC_BASE, MCU_MISC_MCU_BUS_CFG0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Disable bus decoder error response
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void GLB_Bus_Decoder_Err_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(MCU_MISC_BASE, MCU_MISC_MCU_BUS_CFG0);
    tmpVal &= MCU_MISC_DEC_ERR_RSP_UMSK;
    BL_WR_REG(MCU_MISC_BASE, MCU_MISC_MCU_BUS_CFG0, tmpVal);
}

/*@} end of group GLB_Public_Functions */

/*@} end of group GLB */

/*@} end of group BL616L_Peripheral_Driver */
