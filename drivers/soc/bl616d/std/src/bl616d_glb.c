/**
  ******************************************************************************
  * @file    bl616d_glb.c
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

#include "bl616d_clock.h"
#include "bl616d_glb.h"
#include "bl616d_aon.h"
#include "bl616d_hbn.h"
#include "bl616d_pds.h"

/** @addtogroup  BL616D_Peripheral_Driver
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
    .lfAlphaBase = 0,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 3,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
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
    .lfAlphaBase = 0,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 3,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
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
    .lfAlphaBase = 0,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 3,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
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
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 1,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
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
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 1,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
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
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 1,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
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

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupllBasicCfg_24M_26M = {
    .refdivRatio = 2,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 4,     /*!< 4:450M~550MHz */
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 35, /*!< tt50 = 0.68 */
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupll320MCfg_24M_26M = {
    .refdivRatio = 2,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 1,     /*!< 1:320MHz */
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 35, /*!< tt50 = 0.68 */
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupll640MCfg_24M_26M = {
    .refdivRatio = 2,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 7,     /*!< 1:640MHz */
    .vcoIdacExtra = 2, /*!< cpupll_resv0 */
    .dtcRSel = 1,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 0,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 3,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 40, /*!< tt50 = 0.68 */
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupllBasicCfg_32M_38P4M_40M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 4,     /*!< 4:450M~550MHz */
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 35, /*!< tt50 = 0.68 */
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupll320MCfg_32M_38P4M_40M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 1,     /*!< 1:320MHz */
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 35, /*!< tt50 = 0.68 */
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupll640MCfg_32M_38P4M_40M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 7,     /*!< 1:640MHz */
    .vcoIdacExtra = 2, /*!< cpupll_resv0 */
    .dtcRSel = 1,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 0,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 3,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 40, /*!< tt50 = 0.68 */
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_640M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                             /*!< XTAL is None */
    { &cpupll640MCfg_24M_26M, 0x1AAAA },       /*!< XTAL is 24M */
    { &cpupll640MCfg_32M_38P4M_40M, 0x28000 }, /*!< XTAL is 32M */
    { &cpupll640MCfg_32M_38P4M_40M, 0x21555 }, /*!< XTAL is 38.4M */
    { &cpupll640MCfg_32M_38P4M_40M, 0x20000 }, /*!< XTAL is 40M */
    { &cpupll640MCfg_24M_26M, 0x189D8 },       /*!< XTAL is 26M */
    { &cpupll640MCfg_32M_38P4M_40M, 0x28000 }, /*!< XTAL is RC32M */
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_480M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                              /*!< XTAL is None */
    { &cpupllBasicCfg_24M_26M, 0x14000 },       /*!< XTAL is 24M */
    { &cpupllBasicCfg_32M_38P4M_40M, 0x1E000 }, /*!< XTAL is 32M */
    { &cpupllBasicCfg_32M_38P4M_40M, 0x19000 }, /*!< XTAL is 38.4M */
    { &cpupllBasicCfg_32M_38P4M_40M, 0x18000 }, /*!< XTAL is 40M */
    { &cpupllBasicCfg_24M_26M, 0x12762 },       /*!< XTAL is 26M */
    { &cpupllBasicCfg_32M_38P4M_40M, 0x1E000 }, /*!< XTAL is RC32M */
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_320M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                             /*!< XTAL is None */
    { &cpupll320MCfg_24M_26M, 0xD555 },        /*!< XTAL is 24M */
    { &cpupll320MCfg_32M_38P4M_40M, 0x14000 }, /*!< XTAL is 32M */
    { &cpupll320MCfg_32M_38P4M_40M, 0x10AAA }, /*!< XTAL is 38.4M */
    { &cpupll320MCfg_32M_38P4M_40M, 0x10000 }, /*!< XTAL is 40M */
    { &cpupll320MCfg_24M_26M, 0xC4EC },        /*!< XTAL is 26M */
    { &cpupll320MCfg_32M_38P4M_40M, 0x14000 }, /*!< XTAL is RC32M */
};


/* -------------------------------------------------------------------------- */
/* DSIPLL (e.g. target 1400M VCO) basic configuration for different XTALs     */
/* Field order: refdiv_ratio, vco_speed, vco_idac_extra, tdc_dly_sel, dtc_r_sel,
 * lf_alpha_base, lf_alpha_exp, lf_alpha_fast, lf_beta_base, lf_beta_exp,
 * lf_beta_fast, spd_gain, lms_ext_en, lms_ext_value, bitclk_div              */
/* Provided numeric sequences:
 * 32/38.4/40M XTAL : 4 6 2 0 0 0 2 3 3 1 1 0 0 32 2
 * 24/26M    XTAL   : 2 6 2 0 0 0 2 3 3 1 1 0 0 32 2
 */

const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll1400MCfg_32M_38P4M_40M = {
    .refdiv_ratio   = 4,  /* XTAL pre-div so that PFD stays in valid range */
    .vco_speed      = 6,  /* VCO range index (device-specific table) */
    .vco_idac_extra = 2,  /* Extra bias current trim for VCO */
    .tdc_dly_sel    = 0,  /* TDC delay chain select */
    .dtc_r_sel      = 0,  /* DTC resolution window select */
    .lf_alpha_base  = 0,  /* Loop filter alpha base (integral weight coarse) */
    .lf_alpha_exp   = 2,  /* Alpha exponent scaling (bandwidth control) */
    .lf_alpha_fast  = 3,  /* Fast-lock extra alpha gain */
    .lf_beta_base   = 3,  /* Beta base (proportional weight) */
    .lf_beta_exp    = 1,  /* Beta exponent scaling */
    .lf_beta_fast   = 1,  /* Fast-lock beta boost enable */
    .spd_gain       = 0,  /* TDC speed gain index */
    .lms_ext_en     = 0,  /* Use internal LMS (0) */
    .lms_ext_value  = 32, /* External LMS coefficient (ignored if lms_ext_en=0) */
    .bitclk_div     = 2,  /* Output bit clock divider */
};

const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll1400MCfg_24M_26M = {
    .refdiv_ratio   = 2,  /* XTAL pre-div for lower input XTAL to reach same PFD */
    .vco_speed      = 6,  /* Same VCO range as high XTAL set */
    .vco_idac_extra = 2,  /* Extra bias current trim */
    .tdc_dly_sel    = 0,  /* TDC delay chain select */
    .dtc_r_sel      = 0,  /* DTC resolution window select */
    .lf_alpha_base  = 0,  /* Loop filter alpha base */
    .lf_alpha_exp   = 2,  /* Alpha exponent scaling */
    .lf_alpha_fast  = 3,  /* Fast-lock alpha gain */
    .lf_beta_base   = 3,  /* Beta base */
    .lf_beta_exp    = 1,  /* Beta exponent scaling */
    .lf_beta_fast   = 1,  /* Fast-lock beta enable */
    .spd_gain       = 0,  /* TDC speed gain index */
    .lms_ext_en     = 0,  /* Internal LMS */
    .lms_ext_value  = 32, /* External LMS coefficient placeholder */
    .bitclk_div     = 2,  /* Output bit clock divider */
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_1400M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                              /*!< XTAL is None */
    { &dsipll1400MCfg_24M_26M, 0x3A555 },       /*!< XTAL is 24M */
    { &dsipll1400MCfg_32M_38P4M_40M, 0x57800 }, /*!< XTAL is 32M */
    { &dsipll1400MCfg_32M_38P4M_40M, 0x48EAA }, /*!< XTAL is 38.4M */
    { &dsipll1400MCfg_32M_38P4M_40M, 0x46000 }, /*!< XTAL is 40M */
    { &dsipll1400MCfg_24M_26M, 0x35D89 },       /*!< XTAL is 26M */
    { &dsipll1400MCfg_32M_38P4M_40M, 0x57800 }, /*!< XTAL is RC32M */
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
 *           @arg GLB_PLL_CPUPLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllType)
{
    uint32_t tmpVal;
    uint8_t mcuXclkSel;
    uint8_t mcuRootClkSel;
    volatile uint8_t refClk;

    CHECK_PARAM(IS_GLB_XTAL_TYPE(xtalType));
    CHECK_PARAM((pllType < (GLB_PLL_CPUPLL * 2)));

    if (xtalType == GLB_XTAL_NONE) {
        return ERROR;
    }

    if ((xtalType == GLB_XTAL_RC32M) && (pllType & GLB_PLL_WIFIPLL)) {
        /* wifipll must not select RC32M as ref_clk */
        return ERROR;
    }

    if (xtalType != GLB_XTAL_RC32M) {
        /* power on xtal first */
        AON_Power_On_XTAL();
    }

#if defined(DUAL_CORE)
    uint8_t coreType;
    coreType = GLB_Get_Core_Type();
    if ((GLB_CORE_ID_AP == coreType)) {
#endif
        /* AP */
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

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_CPUPLL_REFCLK_RC32M;
    } else {
        refClk = GLB_CPUPLL_REFCLK_XTAL_SOC;
    }

    /* power on wifipll */
    if (pllType & GLB_PLL_WIFIPLL) {
        GLB_Power_Off_WIFIPLL();
        GLB_WIFIPLL_Ref_Clk_Sel(GLB_WIFIPLL_REFCLK_XTAL_SOC);
        GLB_Power_On_WIFIPLL(&wifipllCfgNormal_960M[xtalType], 0);
    }

    /* power on cpupll */
    if (pllType & GLB_PLL_CPUPLL) {
        GLB_Power_Off_CPUPLL();
        GLB_CPUPLL_Ref_Clk_Sel(refClk);
        GLB_Power_On_CPUPLL(&cpupllCfg_480M[xtalType], 0);
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

    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_PU_WIFIPLL, 0);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

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
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_REFCLK_SEL, refClk);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power on wifipll
 *
 * @param  cfg: GLB WIFIPLL configuration
 * @param  waitStable: wait PLL clock stable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_WIFIPLL(const GLB_WIFIPLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t val = 0;
    uint32_t tmpVal = 0;

    /* Step1:config parameter */
    /* set refdiv_ratio, vco_480M_en vco_speed, dtc_r_sel */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_REFCLK_DIV_RATIO, cfg->basicCfg->refdivRatio);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_VCO_480M_EN, cfg->basicCfg->vco480mEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_VCO_SPEED, cfg->basicCfg->vcoSpeed);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_DTC_R_SEL, cfg->basicCfg->dtcRSel);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* set spd_gain */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_SPD_FCAL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_SPD_GAIN, cfg->basicCfg->spdGain);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_SPD_FCAL_OFFSET, tmpVal);

    /* set alpha_base, alpha_exp, alpha_fast, beta_base, beta_exp, beta_fast */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_LF_VCTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_LF_ALPHA_BASE, cfg->basicCfg->lfAlphaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_LF_ALPHA_EXP, cfg->basicCfg->lfAlphaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_LF_ALPHA_FAST, cfg->basicCfg->lfAlphaFast);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_LF_BETA_BASE, cfg->basicCfg->lfBetaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_LF_BETA_EXP, cfg->basicCfg->lfBetaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_LF_BETA_FAST, cfg->basicCfg->lfBetaFast);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_LF_VCTRL_OFFSET, tmpVal);

    /* set wifipll_sdmin */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_SDMIN_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_SDM_IN, cfg->sdmin);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_SDMIN_OFFSET, tmpVal);

    /* set sdm_bypass */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_PI_SDM_LMS_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_SDM_BYPASS, cfg->basicCfg->sdmBypass);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_PI_SDM_LMS_OFFSET, tmpVal);

    /* set vco_idac_extra */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_RESERVE_OFFSET);
    val = BL_GET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_RESV1);
    val &= ~3;
    val |= cfg->basicCfg->vcoIdacExtra;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_RESV1, val);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_RESERVE_OFFSET, tmpVal);

    /* Step2:config pu */
    /* pu_wifipll=1, wifipll_rstb=1 */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_PU_WIFIPLL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_RSTB, 1);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 2us */
    arch_delay_us(3);

    /* wifipll_rstb=0 */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_RSTB, 0);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 1us */
    arch_delay_us(2);

    /* wifipll_rstb=1 */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_RSTB, 1);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* Step3:enable output clock */
    /* wifipll clock divider enable */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_CLKTREE_DIG_OFFSET);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV1);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV2);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV3);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV4);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV5);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV6);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV8);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV10);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV12);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV20);
    tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_EN_DIV30);
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_CLKTREE_DIG_OFFSET, tmpVal);

    if (waitStable) {
        /* Wait 1.5*180us    */
        arch_delay_us(270);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power off cpupll
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_Off_CPUPLL(void)
{
    uint32_t tmpVal = 0;

    /* cfg0 : pu_cpupll=0 */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_PU_CPUPLL, 0);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB cpupll ref clock select
 *
 * @param  refClk: PLL ref clock select, this parameter can be one of the following values:
 *           @arg GLB_CPUPLL_REFCLK_RC32M
 *           @arg GLB_CPUPLL_REFCLK_XTAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_CPUPLL_Ref_Clk_Sel(uint8_t refClk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CPUPLL_REF_CLK_TYPE(refClk));

    /* cpupll_refclk_sel */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_REFCLK_SEL, refClk);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power on CPUPLL
 *
 * @param  cfg: GLB CPUPLL configuration
 * @param  waitStable: wait CPUPLL clock stable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_CPUPLL(const GLB_CPUPLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t val = 0;
    uint32_t tmpVal = 0;

    /* Step1:config parameter */
    /* set refdiv_ratio, vco_speed, dtc_r_sel */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_REFCLK_DIV_RATIO, cfg->basicCfg->refdivRatio);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_VCO_SPEED, cfg->basicCfg->vcoSpeed);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_DTC_R_SEL, cfg->basicCfg->dtcRSel);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET, tmpVal);

    /* set spd_gain */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SPD_FCAL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SPD_GAIN, cfg->basicCfg->spdGain);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SPD_FCAL_OFFSET, tmpVal);

    /* set alpha_base, alpha_exp, alpha_fast, beta_base, beta_exp, beta_fast */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_LF_VCTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_ALPHA_BASE, cfg->basicCfg->lfAlphaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_ALPHA_EXP, cfg->basicCfg->lfAlphaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_ALPHA_FAST, cfg->basicCfg->lfAlphaFast);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_BETA_BASE, cfg->basicCfg->lfBetaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_BETA_EXP, cfg->basicCfg->lfBetaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_BETA_FAST, cfg->basicCfg->lfBetaFast);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_LF_VCTRL_OFFSET, tmpVal);

    /* set cpupll_sdmin */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SDM_IN, cfg->sdmin);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET, tmpVal);

    /* set lms_ext_en, lms_ext_value */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM_LMS_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LMS_EXT_EN, cfg->basicCfg->lmsExtEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LMS_EXT_VALUE, cfg->basicCfg->lmsExtValue);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SDM_LMS_OFFSET, tmpVal);

    /* set vco_idac_extra */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_RESV_OFFSET);
    val = BL_GET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_RESV0);
    val &= ~6;
    val |= cfg->basicCfg->vcoIdacExtra << 1;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_RESV0, val);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_RESV_OFFSET, tmpVal);

    /* Step2:config pu */
    /* pu_cpupll=1, cpupll_rstb=1 */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_PU_CPUPLL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_RSTB, 1);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 2us */
    arch_delay_us(3);

    /* cpupll_rstb=0 */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_RSTB, 0);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 1us */
    arch_delay_us(2);

    /* cpupll_rstb=1 */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_RSTB, 1);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_ANA_CTRL_OFFSET, tmpVal);

    /* Step3:enable output clock */
    /* cpupll clock divider enable */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_CLKTREE_DIG_OFFSET);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV1);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV2);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV3);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV4);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV5);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV6);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV8);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV10);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV12);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV20);
    tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_EN_DIV30);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_CLKTREE_DIG_OFFSET, tmpVal);

    if (waitStable) {
        /* Wait 1.5*180us    */
        arch_delay_us(270);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power off dsipll
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_Off_DSIPLL(void)
{
    uint32_t tmpVal = 0;

    /* cfg0 : pu_dsipll=0 */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_PU_DSIPLL, 0);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB dsipll ref clock select
 *
 * @param  refClk: PLL ref clock select, this parameter can be one of the following values:
 *           @arg GLB_DSIPLL_REFCLK_RC32M
 *           @arg GLB_DSIPLL_REFCLK_XTAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_DSIPLL_Ref_Clk_Sel(uint8_t refClk)
{
    uint32_t tmpVal = 0;

    /* dsipll_refclk_sel */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_REFCLK_SEL, refClk);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power on DSIPLL
 *
 * @param  cfg: GLB DSIPLL configuration
 * @param  waitStable: wait DSIPLL clock stable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_DSIPLL(const GLB_DSIPLL_Cfg_Type *const cfg, uint8_t waitStable)
{
    uint32_t tmpVal = 0;

    /* Step1:config parameter */
    /* set refdiv_ratio, vco_speed, idac_extra */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_REFCLK_DIV_RATIO, cfg->basicCfg->refdiv_ratio);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_VCO_SPEED, cfg->basicCfg->vco_speed);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_VCO_IDAC_EXTRA, cfg->basicCfg->vco_idac_extra);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* set DSIpll_sdmin */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_SDMIN_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_SDM_IN, cfg->sdmin);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_SDMIN_OFFSET, tmpVal);

    /* set dtc_r_sel */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_DTC_R_SEL, cfg->basicCfg->dtc_r_sel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_TDC_DLY_SEL, cfg->basicCfg->tdc_dly_sel);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* set alpha_base, alpha_exp, alpha_fast, beta_base, beta_exp, beta_fast */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_LF_VCTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LF_ALPHA_BASE, cfg->basicCfg->lf_alpha_base);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LF_ALPHA_EXP, cfg->basicCfg->lf_alpha_exp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LF_ALPHA_FAST, cfg->basicCfg->lf_alpha_fast);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LF_BETA_BASE, cfg->basicCfg->lf_beta_base);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LF_BETA_EXP, cfg->basicCfg->lf_beta_exp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LF_BETA_FAST, cfg->basicCfg->lf_beta_fast);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_LF_VCTRL_OFFSET, tmpVal);

    /* set spd_gain */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_SPD_FCAL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_SPD_GAIN, cfg->basicCfg->spd_gain);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_SPD_FCAL_OFFSET, tmpVal);

    /* set lms_ext_en, lms_ext_value */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_SDM_LMS_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LMS_EXT_EN, cfg->basicCfg->lms_ext_en);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_LMS_EXT_VALUE, cfg->basicCfg->lms_ext_value);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_SDM_LMS_OFFSET, tmpVal);

    /* set lms_ext_en, lms_ext_value */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_BICLK_DIV, cfg->basicCfg->bitclk_div);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET, tmpVal);

    /* Step2:config pu */
    /* pu_DSIpll=1, DSIpll_rstb=1 */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_PU_DSIPLL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_RSTB, 1);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 2us */
    arch_delay_us(3);

    /* dsipll_rstb=0 */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_RSTB, 0);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* delay > 1us */
    arch_delay_us(2);

    /* dsipll_rstb=1 */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_RSTB, 1);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_ANA_CTRL_OFFSET, tmpVal);

    /* Step3:enable output clock */
    /* dsipll clock divider enable */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET);
    tmpVal = BL_SET_REG_BIT(tmpVal, DSIPLL_POSTDIV_EN);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET, tmpVal);

    if (waitStable) {
        /* Wait 1.5*180us    */
        arch_delay_us(270);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB power on DSIPLL and set ref clock
 *
 * @param  xtalType: XTAL type, this parameter can be one of the following values:
 *           @arg GLB_XTAL_NONE
 *           @arg GLB_XTAL_24M
 *           @arg GLB_XTAL_32M
 *           @arg GLB_XTAL_38P4M
 *           @arg GLB_XTAL_40M
 *           @arg GLB_XTAL_26M
 *           @arg GLB_XTAL_RC32M
 *
 * @return SUCCESS or ERROR
 *
 *******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Power_On_DSIPLL_Clk(uint8_t xtalType)
{
    uint8_t refClk;

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_DSIPLL_REFCLK_RC32M;
    } else {
        refClk = GLB_DSIPLL_REFCLK_XTAL_SOC;
    }

    GLB_Power_Off_DSIPLL();
    GLB_DSIPLL_Ref_Clk_Sel(refClk);
    GLB_Power_On_DSIPLL(&dsipllCfg_1400M[xtalType], 1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB set CPUPLL post out
 *
 * @param  enable: Enable or disable
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_CPUPLL_PostOut(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(CCI_BASE, CCI_CPUPLL_CLKTREE_RF);
    tmpVal = BL_CLR_REG_BIT(tmpVal, CCI_CPUPLL_POSTDIV_EN);
    BL_WR_REG(CCI_BASE, CCI_CPUPLL_CLKTREE_RF, tmpVal);

    tmpVal = BL_RD_REG(CCI_BASE, CCI_CPUPLL_CLKTREE_RF);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_POSTDIV, div);
    BL_WR_REG(CCI_BASE, CCI_CPUPLL_CLKTREE_RF, tmpVal);

    tmpVal = BL_RD_REG(CCI_BASE, CCI_CPUPLL_CLKTREE_RF);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, CCI_CPUPLL_POSTDIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, CCI_CPUPLL_POSTDIV_EN);
    }
    BL_WR_REG(CCI_BASE, CCI_CPUPLL_CLKTREE_RF, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB set WIFIPLL post out
 *
 * @param  enable: Enable or disable
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_WIFIPLL_PostOut(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF);
    tmpVal = BL_CLR_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_POSTDIV_EN);
    BL_WR_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF, tmpVal);

    tmpVal = BL_RD_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_POSTDIV, div);
    BL_WR_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF, tmpVal);

    tmpVal = BL_RD_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_POSTDIV_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, RF_ANA1_WIFIPLL_POSTDIV_EN);
    }
    BL_WR_REG(RF_BASE, RF_ANA1_WIFIPLL_CLKTREE_RF, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB enable or disable USB clock
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable)
{
    uint32_t tmpVal = 0;

    /* pu_usbpll_mmdiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG10);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PU_USBPLL_MMDIV);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PU_USBPLL_MMDIV);
    }
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG10, tmpVal);

    /* delay > 2us */
    arch_delay_us(3);

    /* toggle usbpll_rstb */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG10);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_USBPLL_RSTB);
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG10, tmpVal);
    arch_delay_us(2);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG10);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_USBPLL_RSTB);
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG10, tmpVal);
    arch_delay_us(2);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG10);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_USBPLL_RSTB);
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG10, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB enable or disable SSC clock
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable)
{
    uint32_t tmpVal = 0;

    /* pu_usbpll_mmdiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG12);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PU_SSCDIV_MMDIV);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PU_SSCDIV_MMDIV);
    }
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG12, tmpVal);

    /* delay > 2us */
    arch_delay_us(3);

    /* toggle sscdiv_rstb */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG12);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SSCDIV_RSTB);
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG12, tmpVal);
    arch_delay_us(2);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG12);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_SSCDIV_RSTB);
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG12, tmpVal);
    arch_delay_us(2);
    tmpVal = BL_RD_REG(GLB_BASE, GLB_WIFI_PLL_CFG12);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_SSCDIV_RSTB);
    BL_WR_REG(GLB_BASE, GLB_WIFI_PLL_CFG12, tmpVal);

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
        //AON_Set_RC32M_Speed_As_8M(false);
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
        HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    }

    /* config hclk_div=mcuClkDiv */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_MAIN_FCLK_DIV, mcuClkDiv);
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

    *mcuClkDiv = BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SYS_CFG0), GLB_REG_MAIN_FCLK_DIV);
    *mcuPBclkDiv = BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SYS_CFG0), GLB_REG_BCLK_DIV);

    return SUCCESS;
}

#if defined(CPU_MODEL_A0)
/****************************************************************************/ /**
 * @brief  Set mcu System clock
 *
 * @param  clkFreq: mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_MCU_SYS_CLK_RC32M
 *           @arg GLB_MCU_SYS_CLK_XTAL
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV2
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV3
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
    //AON_Set_RC32M_Speed_As_8M(false);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* select pll output clock before select root clock */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_CPUPLL_DIV2:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
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
            //AON_Set_RC32M_Speed_As_8M(false);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV2:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 2);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(1, 1);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 1);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
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
#else
/****************************************************************************/ /**
 * @brief  Set mcu System clock
 *
 * @param  clkFreq: mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_MCU_SYS_CLK_RC32M
 *           @arg GLB_MCU_SYS_CLK_XTAL
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV1
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV3
 *           @arg GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M
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
    //AON_Set_RC32M_Speed_As_8M(false);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* select pll output clock before select root clock */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    switch (clkFreq) {
        case GLB_MCU_SYS_CLK_CPUPLL_DIV1:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 1);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M:
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
            //AON_Set_RC32M_Speed_As_8M(false);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV1:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 3);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 1);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            /* recover xclk */
            HBN_Set_MCU_XCLK_Sel(mcuXclkSel);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M:
            /* For high speed, set DIV first */
            GLB_Set_MCU_System_CLK_Div(0, 2);
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_480M);
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
#endif

#if defined(CPU_MODEL_A0)
/****************************************************************************/ /**
 * @brief  Set wl mcu System clock
 *
 * @param  clkFreq: wl mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_WL_MCU_SYS_CLK_RC32M
 *           @arg GLB_WL_MCU_SYS_CLK_XTAL
 *           @arg GLB_WL_MCU_SYS_CLK_CPUPLL_DIV1
 *           @arg GLB_WL_MCU_SYS_CLK_CPUPLL_DIV2
 *           @arg GLB_WL_MCU_SYS_CLK_CPUPLL_DIV3
 * @param  cpuClkDiv: CPU clock divider
 * @param  BusclkDiv: Bus clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_WL_MCU_System_CLK(uint8_t clkFreq, uint8_t cpuClkDiv, uint8_t BusClkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout;

    CHECK_PARAM(IS_GLB_WL_MCU_SYS_CLK_TYPE(clkFreq));

    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CPU);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_BCLK_DIV, BusClkDiv);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_DIV, cpuClkDiv);
    BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CPU, tmpVal);

    /* bclk act pulse */
    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_BCLK_DIV_ACT_PULSE, 1);
    BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU, tmpVal);
    timeout = 1024;
    do {
        tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_WL_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
    tmpVal |= 0x07;
    switch (clkFreq) {
        case GLB_WL_MCU_SYS_CLK_RC32M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL, 0);
            break;
        case GLB_WL_MCU_SYS_CLK_XTAL:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL, 1);
            break;
        case GLB_WL_MCU_SYS_CLK_CPUPLL_DIV1:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_SEL, 0);
            break;
        case GLB_WL_MCU_SYS_CLK_CPUPLL_DIV2:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_SEL, 1);
            break;
        case GLB_WL_MCU_SYS_CLK_CPUPLL_DIV3:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_SEL, 2);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU, tmpVal);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}
#else
/****************************************************************************/ /**
 * @brief  Set wl mcu System clock
 *
 * @param  clkFreq: wl mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_WL_MCU_SYS_CLK_RC32M
 *           @arg GLB_WL_MCU_SYS_CLK_XTAL
 *           @arg GLB_WL_MCU_SYS_CLK_WIFIPLL_DIV2
 *           @arg GLB_WL_MCU_SYS_CLK_CPUPLL_DIV2
 *           @arg GLB_WL_MCU_SYS_CLK_CPUPLL_DIV3
 * @param  cpuClkDiv: CPU clock divider
 * @param  BusclkDiv: Bus clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_WL_MCU_System_CLK(uint8_t clkFreq, uint8_t cpuClkDiv, uint8_t BusClkDiv)
{
    uint32_t tmpVal;
    uint32_t timeout;

    CHECK_PARAM(IS_GLB_WL_MCU_SYS_CLK_TYPE(clkFreq));

    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CPU);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_BCLK_DIV, BusClkDiv);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_DIV, cpuClkDiv);
    BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CPU, tmpVal);

    /* bclk act pulse */
    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_BCLK_DIV_ACT_PULSE, 1);
    BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU, tmpVal);
    timeout = 1024;
    do {
        tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_WL_STS_BCLK_PROT_DONE);
    } while ((--timeout) && (!tmpVal));
    if (!timeout) {
        return ERROR;
    }

    GLB_CLK_SET_DUMMY_WAIT;

    tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);
    tmpVal |= 0x07;
    switch (clkFreq) {
        case GLB_WL_MCU_SYS_CLK_RC32M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL, 0);
            break;
        case GLB_WL_MCU_SYS_CLK_XTAL:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL, 1);
            break;
        case GLB_WL_MCU_SYS_CLK_WIFIPLL_DIV2:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_SEL, 0);
            break;
        case GLB_WL_MCU_SYS_CLK_CPUPLL_DIV2:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_SEL, 1);
            break;
        case GLB_WL_MCU_SYS_CLK_CPUPLL_DIV3:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_ROOT_CLK_SEL, 1);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_CPU_CLK_SEL, 2);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU, tmpVal);

    GLB_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Select wl x clock source
 *
 * @param  clkSel:
 *           @arg GLB_WL_MCU_XCLK_RC32M
 *           @arg GLB_WL_MCU_XCLK_XTAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_WL_XCLK_Sel(uint8_t clkSel)
{
    uint32_t tmpVal = BL_RD_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU);

    if (clkSel == GLB_WL_MCU_XCLK_RC32M) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL, 0);
        BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU, tmpVal);
    } else if (clkSel == GLB_WL_MCU_XCLK_XTAL) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_WL_REG_XCLK_CLK_SEL, 1);
        BL_WR_REG(GLB_WL_BASE, GLB_WL_CLK_CTRL_CPU, tmpVal);
    } else {
        return ERROR;
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select wl x clock source
 *
 * @param  clkSel:
 *           @arg GLB_WL_MCU_XCLK_RC32M
 *           @arg GLB_WL_MCU_XCLK_XTAL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
void ATTR_CLOCK_SECTION GLB_Set_WIFIPLL_Fine_Tune(void)
{
    uint32_t tmpVal;

    /* WIFIPLL HW CTRL @ 0x200010D4 */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_HW_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX, 0);   /* [12]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_VCO_RSHT_EN_TX_BZ, 0);/* [16]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_PI_BYPASS_RX_BZ, 1);  /* [6]   = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_PI_BYPASS_RX, 1);     /* [2]   = 1 */
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_HW_CTRL_OFFSET, tmpVal);

    /* WIFIPLL PI/SDM/LMS @ 0x200010E4 */
    tmpVal = BL_RD_WORD(RF_BASE + RF_ANA1_WIFIPLL_PI_SDM_LMS_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_PI_BYPASS, 1);        /* [31]  = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_SDM_BYPASS, 0);       /* [24]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_SDM_DITH_FORCE_EN, 0);/* [23]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, RF_ANA1_WIFIPLL_SDM_DITH_EN, 0);      /* [22]  = 0 */
    BL_WR_WORD(RF_BASE + RF_ANA1_WIFIPLL_PI_SDM_LMS_OFFSET, tmpVal);
#if 0
    /* should be set on system init, not here */
    /* RC32M reserved @ 0x2008F994[25] = 1 (within field [31:24]) */
    tmpVal = BL_RD_WORD(AON_BASE + AON_RC32M_CTRL1_AON_OFFSET);
    val |= (1U << (25));
    BL_WR_WORD(AON_BASE + AON_RC32M_CTRL1_AON_OFFSET, tmpVal);
#endif
    /* DCDC12 @ 0x2008F83C: VPFM=1, VC_CLAMP_VTH=3 */
    tmpVal = BL_RD_WORD(AON_BASE + AON_DCDC12_3_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_VPFM_AON, 1);              /* [19:16] = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_VC_CLAMP_VTH_AON, 3);      /* [2:0]   = 3 */
    BL_WR_WORD(AON_BASE + AON_DCDC12_3_OFFSET, tmpVal);

    /* DCDC12 @ 0x2008F830: BM_NM=7, ISENSE_TRIM=3 */
    tmpVal = BL_RD_WORD(AON_BASE + AON_DCDC12_0_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_BM_NM_AON, 7);             /* [6:4]   = 7 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_ISENSE_TRIM_AON, 3);       /* [30:28] = 3 */
    BL_WR_WORD(AON_BASE + AON_DCDC12_0_OFFSET, tmpVal);

    /* DCDC12 @ 0x2008F834: OSC_2M_MODE=1, OSC_EN_INHIBIT_T2=0, OCP_VTH=1, LP_FORCE_EN=0 */
    tmpVal = BL_RD_WORD(AON_BASE + AON_DCDC12_1_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_OSC_2M_MODE_AON, 1);       /* [20]    = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_OSC_EN_INHIBIT_T2_AON, 0); /* [21]    = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_OCP_VTH_AON, 1);           /* [18:16] = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_LP_FORCE_EN_AON, 0);       /* [1]     = 0 */
    BL_WR_WORD(AON_BASE + AON_DCDC12_1_OFFSET, tmpVal);

    /* DCDC12 @ 0x2008F838: RC_SEL=4, SSTART_TIME=1 */
    tmpVal = BL_RD_WORD(AON_BASE + AON_DCDC12_2_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_RC_SEL_AON, 4);            /* [15:12] = 4 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_SSTART_TIME_AON, 1);       /* [29:28] = 1 */
    BL_WR_WORD(AON_BASE + AON_DCDC12_2_OFFSET, tmpVal);

#if 0
    /* should be set on system init, not here */
    /* LDO08AON @ 0x2008F80C: interpret 4'h12 as TRIM=1, SEL=2 */
    tmpVal = BL_RD_WORD(AON_BASE + AON_0_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO08AON_VOUT_SEL_AON, 0xC);      /* [27:24] = 12 */
    BL_WR_WORD(AON_BASE + AON_0_OFFSET, tmpVal);
#endif

    /* CPUPLL tweaks */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_LF_VCTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_MOM_UPDATE_PERIOD, 3);     /* 0x200087D8[1:0] = 3 */
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_LF_VCTRL_OFFSET, tmpVal);

    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SPD_FCAL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_COARSE_GAIN, 3);           /* 0x200087D4[30:29] = 3 */
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SPD_FCAL_OFFSET, tmpVal);
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
            return GLB_CORE_ID_AP;
        case CORE_ID_M1:
            return GLB_CORE_ID_NP;
        case CORE_ID_LP:
            return GLB_CORE_ID_LP;
        default:
            return GLB_CORE_ID_INVALID;
    }

    return GLB_CORE_ID_INVALID;
}

/****************************************************************************/ /**
 * @brief  set CPU reset address
 *
 * @param  coreID: core type, this parameter can be one of the following values:
 *           @arg GLB_CORE_ID_AP
 *           @arg GLB_CORE_ID_NP
 *           @arg GLB_CORE_ID_LP
 * @param  addr: reset address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr)
{
    CHECK_PARAM(IS_GLB_CORE_ID_TYPE(coreID));

    switch (coreID) {
        case GLB_CORE_ID_AP:
            BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG14, addr);
            break;
        case GLB_CORE_ID_NP:
            *((volatile uint32_t *)0x24001144) = addr;
            break;
        case GLB_CORE_ID_LP:
            BL_WR_REG(MINI_MISC, MINI_MISC_MINI_RST_ADDR, addr);
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

    /* set EMI cgen bit */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_CGEN_S1_EXT_EMI_MISC);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set ADC clock
 *
 * @param  enable: enable frequency divider or not
 * @param  clkSel: ADC clock selection, this parameter can be one of the following values:
 *           @arg GLB_ADC_CLK_WIFIPLL_96M
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

#if defined(CPU_MODEL_A0)
/****************************************************************************/ /**
 * @brief  set peripheral DMA cn
 *
 * @param  peri: peripheral, this parameter can be one of the following values:
 *           @arg GLB_PERI_DMA_UART0_RX
 *           @arg GLB_PERI_DMA_UART0_TX
 *           @arg GLB_PERI_DMA_UART1_RX
 *           @arg GLB_PERI_DMA_UART1_TX
 *           @arg GLB_PERI_DMA_UART2_RX
 *           @arg GLB_PERI_DMA_UART2_TX
 *           @arg GLB_PERI_DMA_I2C_0_RX
 *           @arg GLB_PERI_DMA_I2C_0_TX
 *           @arg GLB_PERI_DMA_IRTX_TX
 *           @arg GLB_PERI_DMA_WO
 *           @arg GLB_PERI_DMA_SPI_RX
 *           @arg GLB_PERI_DMA_SPI_TX
 *           @arg GLB_PERI_DMA_AUDAC_TX
 *           @arg GLB_PERI_DMA_I2C_1_RX
 *           @arg GLB_PERI_DMA_I2C_1_TX
 *           @arg GLB_PERI_DMA_I2S_0_RX
 *           @arg GLB_PERI_DMA_I2S_0_TX
 *           @arg GLB_PERI_DMA_ADC0
 *           @arg GLB_PERI_DMA_ADC1
 *           @arg GLB_PERI_DMA_DBI_TX
 *           @arg GLB_PERI_DMA_AUADC_RX
 *           @arg GLB_PERI_DMA_DAC
 *           @arg GLB_PERI_DMA_PEC_SM0_RX
 *           @arg GLB_PERI_DMA_PEC_SM1_RX
 *           @arg GLB_PERI_DMA_PEC_SM2_RX
 *           @arg GLB_PERI_DMA_PEC_SM3_RX
 *           @arg GLB_PERI_DMA_PEC_SM0_TX
 *           @arg GLB_PERI_DMA_PEC_SM1_TX
 *           @arg GLB_PERI_DMA_PEC_SM2_TX
 *           @arg GLB_PERI_DMA_PEC_SM3_TX
 * @param  cn: cn, this parameter can be one of the following values:
 *           @arg GLB_PERI_DMA_CN_SEL_DMA0
 *           @arg GLB_PERI_DMA_CN_SEL_DMA1
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#else
/****************************************************************************/ /**
 * @brief  set peripheral DMA cn
 *
 * @param  peri: peripheral, this parameter can be one of the following values:
 *           @arg GLB_PERI_DMA_UART0_RX
 *           @arg GLB_PERI_DMA_UART0_TX
 *           @arg GLB_PERI_DMA_UART1_RX
 *           @arg GLB_PERI_DMA_UART1_TX
 *           @arg GLB_PERI_DMA_UART2_RX
 *           @arg GLB_PERI_DMA_UART2_TX
 *           @arg GLB_PERI_DMA_I2C_0_RX
 *           @arg GLB_PERI_DMA_I2C_0_TX
 *           @arg GLB_PERI_DMA_IRTX_TX
 *           @arg GLB_PERI_DMA_WO
 *           @arg GLB_PERI_DMA_SPI_RX
 *           @arg GLB_PERI_DMA_SPI_TX
 *           @arg GLB_PERI_DMA_AUDAC_TX
 *           @arg GLB_PERI_DMA_I2C_1_RX
 *           @arg GLB_PERI_DMA_I2C_1_TX
 *           @arg GLB_PERI_DMA_I2S_0_RX
 *           @arg GLB_PERI_DMA_I2S_0_TX
 *           @arg GLB_PERI_DMA_ADC0
 *           @arg GLB_PERI_DMA_ADC1
 *           @arg GLB_PERI_DMA_DBI_TX
 *           @arg GLB_PERI_DMA_AUADC_RX
 *           @arg GLB_PERI_DMA_DSI_TX
 *           @arg GLB_PERI_DMA_DSI_RX
 *           @arg GLB_PERI_DMA_SPI1_RX
 *           @arg GLB_PERI_DMA_SPI1_TX
 *           @arg GLB_PERI_DMA_SPI2_RX
 *           @arg GLB_PERI_DMA_SPI2_TX
 *           @arg GLB_PERI_DMA_PEC_SM0_TX
 *           @arg GLB_PERI_DMA_PEC_SM1_TX
 *           @arg GLB_PERI_DMA_PEC_SM2_TX
 *           @arg GLB_PERI_DMA_PEC_SM3_TX
 *           @arg GLB_PERI_DMA_PEC_SM0_RX
 *           @arg GLB_PERI_DMA_PEC_SM1_RX
 *           @arg GLB_PERI_DMA_PEC_SM2_RX
 *           @arg GLB_PERI_DMA_PEC_SM3_RX
 * @param  cn: cn, this parameter can be one of the following values:
 *           @arg GLB_PERI_DMA_CN_SEL_DMA0
 *           @arg GLB_PERI_DMA_CN_SEL_DMA1
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#endif
BL_Err_Type GLB_Set_Peripheral_DMA_CN(uint8_t peri, uint8_t cn)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PERI_DMA_TYPE(peri));
    CHECK_PARAM(IS_GLB_PERI_DMA_CN_SEL_TYPE(cn));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_DMA_CFG2);
    switch (cn) {
        case GLB_PERI_DMA_CN_SEL_DMA0:
            tmpVal &= ~(1 << peri);
            break;
        case GLB_PERI_DMA_CN_SEL_DMA1:
            tmpVal |= (1 << peri);
            break;
        default:
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_DMA_CFG2, tmpVal);

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
 * @brief  Select ir rx gpio (gpio1~gpio15)
 *
 * @param  gpio: IR gpio selected, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_IR_RX_GPIO_Sel(uint8_t gpio)
{
    uint32_t tmpVal = 0;

    /* Select gpio between gpio1 and gpio15 */
    if (gpio > 0 && gpio < 16) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_IR_CFG1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_IR_RX_GPIO_SEL, gpio);
        BL_WR_REG(GLB_BASE, GLB_IR_CFG1, tmpVal);
    }
    /* Close ir rx */
    else if (gpio == 0) {
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
 *           @arg HBN_UART_CLK_WIFIPLL_160M
 *           @arg HBN_UART_CLK_XCLK
 * @param  div: UART clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0x7));
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
 *           @arg GLB_UART_SIG_FUN_UART2_RTS
 *           @arg GLB_UART_SIG_FUN_UART2_CTS
 *           @arg GLB_UART_SIG_FUN_UART2_TXD
 *           @arg GLB_UART_SIG_FUN_UART2_RXD
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
 *           @arg GLB_SFLASH_CLK_WIFIPLL_48M
 *           @arg GLB_SFLASH_CLK_WIFIPLL_80M
 *           @arg GLB_SFLASH_CLK_BCLK
 *           @arg GLB_SFLASH_CLK_WIFIPLL_96M
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
        case GLB_SFLASH_CLK_WIFIPLL_48M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL2, 0x3);
            break;
        case GLB_SFLASH_CLK_WIFIPLL_80M:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x1);
            break;
        case GLB_SFLASH_CLK_BCLK:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL, 0x2);
            break;
        case GLB_SFLASH_CLK_WIFIPLL_96M:
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
 * @brief  set SPI clock
 *
 * @param  enable: Enable or disable SPI clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_SPI_CLK_WIFIPLL_160M
 *           @arg GLB_SPI_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
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
 * @brief  swap SPI gpio pins sig function
 *
 * @param  group: SPI swap set group, this parameter can be one of the following values:
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35
 *           @arg GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45
 * @param  swap: swap or no swap
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_SPI_Sig_Swap_Set(uint8_t group, uint8_t swap)
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
 * @brief  set PEC clock
 *
 * @param  enable: Enable or disable PEC clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_PEC_CLK_MUXPLL_160M
 *           @arg GLB_PEC_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_PEC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_PEC_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PEC_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_PEC_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PEC_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PEC_CLK_SEL, clkSel);
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
    tmpVal >>= 1;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_PWM1_IO_SEL, ioSel);
    BL_WR_REG(GLB_BASE, GLB_PWM_CFG0, tmpVal);

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
 *           @arg GLB_DIG_CLK_AUPLL
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
 * @brief  set audio ADC clock
 *
 * @param  enable: ENABLE or DISABLE
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((div <= 0x3F));

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_AUDIO_ADC_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_AUDIO_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_AUDIO_ADC_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_AUDIO_CFG0, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set audio SOLO clock
 *
 * @param  enable: ENABLE or DISABLE
 * @param  div: divider
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
 *           @arg GLB_CAM_CLK_TOP_AUPLL_DIV5
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
 * @brief  Select SPI_0 act mode
 *
 * @param  mod: SPI work mode, this parameter can be one of the following values:
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
 *           @arg GLB_AHB_MCU_SW_RSV0
 *           @arg GLB_AHB_MCU_SW_RSV1
 *           @arg GLB_AHB_MCU_SW_WIFI
 *           @arg GLB_AHB_MCU_SW_BTDM
 *           @arg GLB_AHB_MCU_SW_ZIGBEE
 *           @arg GLB_AHB_MCU_SW_BLE2
 *           @arg GLB_AHB_MCU_SW_ZIGBEE2
 *           @arg GLB_AHB_MCU_SW_EMI_MISC
 *           @arg GLB_AHB_MCU_SW_MM_MISC
 *           @arg GLB_AHB_MCU_SW_PSRAM1_CTRL
 *           @arg GLB_AHB_MCU_SW_EXT_USB
 *           @arg GLB_AHB_MCU_SW_EXT_AUPWM
 *           @arg GLB_AHB_MCU_SW_EXT_SDH
 *           @arg GLB_AHB_MCU_SW_EXT_EMAC
 *           @arg GLB_AHB_MCU_SW_D2XA
 *           @arg GLB_AHB_MCU_SW_D2XB
 *           @arg GLB_AHB_MCU_SW_JENC
 *           @arg GLB_AHB_MCU_SW_GLB
 *           @arg GLB_AHB_MCU_SW_MIX
 *           @arg GLB_AHB_MCU_SW_GPIP
 *           @arg GLB_AHB_MCU_SW_SEC_DBG
 *           @arg GLB_AHB_MCU_SW_SEC_ENG
 *           @arg GLB_AHB_MCU_SW_TZ1
 *           @arg GLB_AHB_MCU_SW_RSV38
 *           @arg GLB_AHB_MCU_SW_EFUSE
 *           @arg GLB_AHB_MCU_SW_RSV40
 *           @arg GLB_AHB_MCU_SW_PERI
 *           @arg GLB_AHB_MCU_SW_RSV42
 *           @arg GLB_AHB_MCU_SW_SF
 *           @arg GLB_AHB_MCU_SW_DMA
 *           @arg GLB_AHB_MCU_SW_SDU
 *           @arg GLB_AHB_MCU_SW_PDS
 *           @arg GLB_AHB_MCU_SW_RSV47
 *           @arg GLB_AHB_MCU_SW_UART0
 *           @arg GLB_AHB_MCU_SW_UART1
 *           @arg GLB_AHB_MCU_SW_SPI
 *           @arg GLB_AHB_MCU_SW_I2C0
 *           @arg GLB_AHB_MCU_SW_PWM
 *           @arg GLB_AHB_MCU_SW_TIMER
 *           @arg GLB_AHB_MCU_SW_IR_REMOTE
 *           @arg GLB_AHB_MCU_SW_CHECKSUM
 *           @arg GLB_AHB_MCU_SW_DBI
 *           @arg GLB_AHB_MCU_SW_I2C1
 *           @arg GLB_AHB_MCU_SW_I2S
 *           @arg GLB_AHB_MCU_SW_AUSOLO
 *           @arg GLB_AHB_MCU_SW_RSV61
 *           @arg GLB_AHB_MCU_SW_RSV62
 *           @arg GLB_AHB_MCU_SW_RSV63
 *           @arg GLB_AHB_MCU_SW_PWRON_RST
 *           @arg GLB_AHB_MCU_SW_CPU_RESET
 *           @arg GLB_AHB_MCU_SW_SYS_RESET
 *           @arg GLB_AHB_MCU_SW_CHIP_RESET
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
 *           @arg GLB_DISRST_SF
 *           @arg GLB_DISRST_UART0
 *           @arg GLB_DISRST_UART1
 *           @arg GLB_DISRST_SPI
 *           @arg GLB_DISRST_I2C0
 *           @arg GLB_DISRST_PWM
 *           @arg GLB_DISRST_TIMER
 *           @arg GLB_DISRST_IR_REMOTE
 *           @arg GLB_DISRST_CHECKSUM
 *           @arg GLB_DISRST_DBI
 *           @arg GLB_DISRST_I2C1
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
BL_Err_Type GLB_PER_Clock_Gate(uint64_t ips)
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
                    tmpValCfg0 &= ~(1 << 2);
                    tmpValCfg1 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_0:
                    tmpValCfg0 &= ~(1 << 3);
                    tmpValCfg1 &= ~(1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_1:
                    break;
                case GLB_AHB_CLOCK_IP_DMA_2:
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 &= ~(1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_RF_TOP:
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 &= ~(1 << 2);
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
                    break;
                case GLB_AHB_CLOCK_IP_UART3:
                    break;
                case GLB_AHB_CLOCK_IP_UART4:
                    break;
                case GLB_AHB_CLOCK_IP_SPI:
                    tmpValCfg1 &= ~(1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_I2C:
                    tmpValCfg1 &= ~(1 << 19);
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
                case GLB_AHB_CLOCK_IP_QDEC:
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    break;
                case GLB_AHB_CLOCK_IP_I2S:
                    tmpValCfg1 &= ~(1 << 27);
                    break;
                case GLB_AHB_CLOCK_IP_USB11:
                    break;
                case GLB_AHB_CLOCK_IP_CAM:
                    break;
                case GLB_AHB_CLOCK_IP_MJPEG:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_LP:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_LP:
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
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM0_CTRL:
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM1_CTRL:
                    tmpValCfg2 &= ~(1 << 18);
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
                case GLB_AHB_CLOCK_IP_ZB2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_I2C1:
                    tmpValCfg1 &= ~(1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_MAC_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PLATFORM:
                    break;
                case GLB_AHB_CLOCK_IP_LZ4:
                    break;
                case GLB_AHB_CLOCK_IP_AUPDM:
                    break;
                case GLB_AHB_CLOCK_IP_GAUGE:
                    break;
                case GLB_AHB_CLOCK_IP_DBI:
                    tmpValCfg1 &= ~(1 << 24);
                    break;
                case GLB_AHB_CLOCK_IP_AUSOLO_TOP:
                    tmpValCfg1 &= ~(1 << 28);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_GPIO:
                    tmpValCfg1 &= ~(1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_MM_MISC:
                    tmpValCfg2 &= ~(1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_USB20:
                    tmpValCfg2 &= ~(1 << 19);
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
BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips)
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
                    tmpValCfg0 |= (1 << 2);
                    tmpValCfg1 |= (1 << 3);
                    tmpValCfg1 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_0:
                    tmpValCfg0 |= (1 << 3);
                    tmpValCfg1 |= (1 << 12);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_1:
                    break;
                case GLB_AHB_CLOCK_IP_DMA_2:
                    break;
                case GLB_AHB_CLOCK_IP_CCI:
                    tmpValCfg0 |= (1 << 4);
                    break;
                case GLB_AHB_CLOCK_IP_RF_TOP:
                    break;
                case GLB_AHB_CLOCK_IP_GPIP:
                    tmpValCfg1 |= (1 << 2);
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
                    break;
                case GLB_AHB_CLOCK_IP_UART3:
                    break;
                case GLB_AHB_CLOCK_IP_UART4:
                    break;
                case GLB_AHB_CLOCK_IP_SPI:
                    tmpValCfg1 |= (1 << 18);
                    break;
                case GLB_AHB_CLOCK_IP_I2C:
                    tmpValCfg1 |= (1 << 19);
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
                case GLB_AHB_CLOCK_IP_QDEC:
                    break;
                case GLB_AHB_CLOCK_IP_KYS:
                    break;
                case GLB_AHB_CLOCK_IP_I2S:
                    tmpValCfg1 |= (1 << 27);
                    break;
                case GLB_AHB_CLOCK_IP_USB11:
                    break;
                case GLB_AHB_CLOCK_IP_CAM:
                    break;
                case GLB_AHB_CLOCK_IP_MJPEG:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_BT_BLE_LP:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB_LP:
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
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM0_CTRL:
                    break;
                case GLB_AHB_CLOCK_IP_PSRAM1_CTRL:
                    tmpValCfg2 |= (1 << 18);
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
                case GLB_AHB_CLOCK_IP_ZB2_NORMAL:
                    break;
                case GLB_AHB_CLOCK_IP_ZB2_LP:
                    break;
                case GLB_AHB_CLOCK_IP_I2C1:
                    tmpValCfg1 |= (1 << 25);
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_MAC_PHY:
                    break;
                case GLB_AHB_CLOCK_IP_WIFI_PLATFORM:
                    break;
                case GLB_AHB_CLOCK_IP_LZ4:
                    break;
                case GLB_AHB_CLOCK_IP_AUPDM:
                    break;
                case GLB_AHB_CLOCK_IP_GAUGE:
                    break;
                case GLB_AHB_CLOCK_IP_DBI:
                    tmpValCfg1 |= (1 << 24);
                    break;
                case GLB_AHB_CLOCK_IP_AUSOLO_TOP:
                    tmpValCfg1 |= (1 << 28);
                    break;
                case GLB_AHB_CLOCK_IP_DMA_GPIO:
                    tmpValCfg1 |= (1 << 0);
                    break;
                case GLB_AHB_CLOCK_IP_MM_MISC:
                    tmpValCfg2 |= (1 << 16);
                    break;
                case GLB_AHB_CLOCK_IP_USB20:
                    tmpValCfg2 |= (1 << 19);
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
 *           @arg GLB_PLL_CGEN_ISP_AUPLL_DIV5
 *           @arg GLB_PLL_CGEN_ISP_AUPLL_DIV6
 *           @arg GLB_PLL_CGEN_TOP_AUPLL_DIV5
 *           @arg GLB_PLL_CGEN_TOP_AUPLL_DIV6
 *           @arg GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M
 *           @arg GLB_PLL_CGEN_PSRAMB_AUPLL_DIV1
 *           @arg GLB_PLL_CGEN_TOP_WIFIPLL_240M
 *           @arg GLB_PLL_CGEN_TOP_WIFIPLL_320M
 *           @arg GLB_PLL_CGEN_TOP_AUPLL_DIV2
 *           @arg GLB_PLL_CGEN_TOP_AUPLL_DIV1
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
 *           @arg GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M
 *           @arg GLB_PLL_CGEN_PSRAMB_AUPLL_DIV1
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
 *           @arg GLB_PKA_CLK_WIFIPLL_160M
 *
 * @return SUCCESS or ERROR
 *
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
    AON_Set_RC32M_Speed_As_8M(false);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
    HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_XCLK);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* reset AP/SYS(bus) */
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
        case GLB_CORE_ID_AP:
#endif
            /* Swicth MCU clock to RC32M, no divider */
            AON_Set_RC32M_Speed_As_8M(false);
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
    AON_Set_RC32M_Speed_As_8M(false);
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
            tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LDO18IO_VOUT_SEL, trim.value);
            BL_WR_REG(GLB_BASE, GLB_LDO18IO, tmpVal);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  trim LDO18IO bypass
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Trim_Ldo18ioBypass(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18_bypass", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 1)) {
            tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LDO18IO_BYPASS, trim.value);
            BL_WR_REG(GLB_BASE, GLB_LDO18IO, tmpVal);
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
            tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_LDO18IO_VOUT_TRIM, trim.value);
            BL_WR_REG(GLB_BASE, GLB_LDO18IO, tmpVal);
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

    tmpVal = BL_RD_REG(GLB_BASE, GLB_LDO18IO);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PU_LDO18IO);
    BL_WR_REG(GLB_BASE, GLB_LDO18IO, tmpVal);
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
 *           @arg GLB_WRAM128KB_EM32KB,
 *           @arg GLB_WRAM96KB_EM64KB,
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
        case GLB_WRAM128KB_EM32KB:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03);
            break;
        case GLB_WRAM96KB_EM64KB:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x0F);
            break;
        default:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03);
            break;
    }
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PSRAMB clock
 *
 * @param  enable: Enable or disable
 * @param  clkSel: PSRAMB clock type
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

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PSRAM_CLK_EN);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PSRAM_CLK_SEL, clkSel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PSRAM_CLK_DIV, div);
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG2, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SYS_CFG2);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PSRAM_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PSRAM_CLK_EN);
    }
    BL_WR_REG(GLB_BASE, GLB_SYS_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set PSRAMB clock
 *
 * @param  slave: slave IP, this parameter can be one of the following values:
 *           @arg GLB_SLAVE_GRP_0_ADC
 *           @arg GLB_SLAVE_GRP_0_IR
 *           @arg GLB_SLAVE_GRP_0_I2C
 *           @arg GLB_SLAVE_GRP_0_SPI
 *           @arg GLB_SLAVE_GRP_0_DBI
 *           @arg GLB_SLAVE_GRP_0_AUDIO_AUTO
 *           @arg GLB_SLAVE_GRP_0_AUDIO_ADC
 *           @arg GLB_SLAVE_GRP_0_AUDIO_SOLO
 *           @arg GLB_SLAVE_GRP_0_CAM
 *           @arg GLB_SLAVE_GRP_0_SDH
 *           @arg GLB_SLAVE_GRP_0_PSRAMB
 *           @arg GLB_SLAVE_GRP_0_MAX
 * @param  enable: enable or disable clock
 * @param  clkSel: clock selection
 * @param  div: clock divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Set_Slave_Grp_0_CLK(uint8_t slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div)
{
    uint32_t tmpVal;
    uint32_t regAddr = GLB_BASE + glb_slave_grp_0_table[slave].clkOffSetAddr;
    uint8_t enPos = glb_slave_grp_0_table[slave].clkEnPos;
    uint8_t selPos = glb_slave_grp_0_table[slave].clkSelPos;
    uint8_t divPos = glb_slave_grp_0_table[slave].clkDivPos;
    uint8_t enLen = glb_slave_grp_0_table[slave].clkEnLen;
    uint8_t selLen = glb_slave_grp_0_table[slave].clkSelLen;
    uint8_t divLen = glb_slave_grp_0_table[slave].clkDivLen;

    CHECK_PARAM(IS_GLB_SLAVE_GRP_0_TYPE(slave));

    /* disable clock first */
    if (enLen) {
        tmpVal = BL_RD_WORD(regAddr);
        tmpVal &= ~(((1ULL << enLen) - 1) << enPos);
        BL_WR_WORD(regAddr, tmpVal);
    }

    /* set clock sel or div */
    tmpVal = BL_RD_WORD(regAddr);
    if (selLen) {
        tmpVal &= ~(((1ULL << selLen) - 1) << selPos);
        tmpVal |= (((clkSel.clkSel) & ((1 << selLen) - 1)) << selPos);
    }
    if (divLen) {
        tmpVal &= ~(((1ULL << divLen) - 1) << divPos);
        tmpVal |= ((div & ((1 << divLen) - 1)) << divPos);
    }
    BL_WR_WORD(regAddr, tmpVal);

    /* enable clock */
    if (enLen && enable) {
        tmpVal = BL_RD_WORD(regAddr);
        tmpVal |= (((1ULL << enLen) - 1) << enPos);
        BL_WR_WORD(regAddr, tmpVal);
    }

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
 * @brief  reconfigure CPUPLL clock
 *
 * @param  xtalType: XTAL frequency type, this parameter can be one of the following values:
 *           @arg GLB_XTAL_NONE, should not used as para
 *           @arg GLB_XTAL_24M
 *           @arg GLB_XTAL_32M
 *           @arg GLB_XTAL_38P4M
 *           @arg GLB_XTAL_40M
 *           @arg GLB_XTAL_26M
 *           @arg GLB_XTAL_RC32M
 *           @arg GLB_XTAL_MAX
 * @param  pllCfg: PLL configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_CPUPLL(uint8_t xtalType, const GLB_CPUPLL_Cfg_Type *pllCfgList)
{
    uint8_t refClk;

    if (xtalType == GLB_XTAL_RC32M) {
        refClk = GLB_CPUPLL_REFCLK_RC32M;
    } else {
        refClk = GLB_CPUPLL_REFCLK_XTAL_SOC;
    }

    GLB_Power_Off_CPUPLL();
    GLB_CPUPLL_Ref_Clk_Sel(refClk);
    GLB_Power_On_CPUPLL(&(pllCfgList[xtalType]), 1);

    return SUCCESS;
}

#if defined(CPU_MODEL_A0)
/****************************************************************************/ /**
 * @brief  Set mcu System clock Simple
 *
 * @param  clkFreq: mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_MCU_SYS_CLK_RC32M
 *           @arg GLB_MCU_SYS_CLK_XTAL
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV2
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV3
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
        case GLB_MCU_SYS_CLK_CPUPLL_DIV2:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
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
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_MAIN_FCLK_DIV, mcuClkDiv);
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
            //AON_Set_RC32M_Speed_As_8M(false);
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV2:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
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
#else
/****************************************************************************/ /**
 * @brief  Set mcu System clock Simple
 *
 * @param  clkFreq: mcu system clock type, this parameter can be one of the following values:
 *           @arg GLB_MCU_SYS_CLK_RC32M
 *           @arg GLB_MCU_SYS_CLK_XTAL
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV1
 *           @arg GLB_MCU_SYS_CLK_CPUPLL_DIV3
 *           @arg GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M
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
        case GLB_MCU_SYS_CLK_CPUPLL_DIV1:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 0);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_PLL_SEL, 1);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M:
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
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_MAIN_FCLK_DIV, mcuClkDiv);
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
            //AON_Set_RC32M_Speed_As_8M(false);
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
            break;
        case GLB_MCU_SYS_CLK_XTAL:
            HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV1:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_CPUPLL_DIV3:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_PLL);
            break;
        case GLB_MCU_SYS_CLK_TOP_WIFIPLL_480M:
            /* For high speed, set DIV first */
            /* Set IROM 2T Access 0 since we use RC32M, unuseful now */
            /* MCU_MISC_IROM_2T_Access_Set(0); */
            /* unuseful for mcu, useful for dsp, just for safe */
            GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_TOP_WIFIPLL_480M);
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
#endif

/****************************************************************************/ /**
 * @brief  enable or disable flash xip mode
 *
 * @param  enable: Enable or disable
 *
 * @return NONE
 *
*******************************************************************************/
void GLB_Set_Flash_Xip(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_BMX_CFG6);
    if (enable) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_PROT_REQ_SF);
    } else {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_PROT_REQ_SF);
    }
    BL_WR_REG(GLB_BASE, GLB_BMX_CFG6, tmpVal);
}

/****************************************************************************/ /**
 * @brief  halt mini system
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Halt_Mini_Sys(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_MINI_RESET);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  release mini system reset
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Release_Mini_Sys(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_MINI_RESET);
    BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  mini system ip software reset
 *
 * @param  swrst: reset num, this parameter can be one of the following values:
 *           @arg GLB_MINI_MISC_SW_RST_DMA2
 *           @arg GLB_MINI_MISC_SW_RST_SPI1
 *           @arg GLB_MINI_MISC_SW_RST_UART3
 *           @arg GLB_MINI_MISC_SW_RST_TMR1
 *           @arg GLB_MINI_MISC_SW_RST_I2C2
 *           @arg GLB_MINI_MISC_SW_RST_PWM1
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_MINI_PER_Software_Reset(uint8_t swrst)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_MINI_MISC_SW_RST_TYPE(swrst));

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_RST_0);
    tmpVal &= ~(1 << swrst);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_RST_0, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_RST_0);
    tmpVal |= (1 << swrst);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_RST_0, tmpVal);
    BL_DRV_DUMMY;
    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_RST_0);
    tmpVal &= ~(1 << swrst);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_RST_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MINI SPI1 clock
 *
 * @param  enable: Enable or disable SPI clock
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_SPI_CLK_WIFIPLL_160M
 *           @arg GLB_SPI_CLK_XCLK
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_SPI1_CLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_MINI_SPI1_CLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x1F));

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, MINI_MISC_CR_SPI_CLK_EN);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1, tmpVal);

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_SEL, clkSel);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1, tmpVal);

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, MINI_MISC_CR_SPI_CLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, MINI_MISC_CR_SPI_CLK_EN);
    }
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MINI FCLK
 *
 * @param  enable: Enable or disable mini_fclk
 * @param  clkSel: clock selection, this parameter can be one of the following values:
 *           @arg GLB_MINI_FCLK_XCLK
 *           @arg GLB_MINI_FCLK_WIFIPLL_120M
 *           @arg GLB_MINI_FCLK_WIFIPLL_80M
 *           @arg GLB_MINI_FCLK_WIFIPLL_48M
 * @param  div: divider
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Set_MINI_FCLK(uint8_t enable, uint8_t clkSel, uint8_t div)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_MINI_FCLK_TYPE(clkSel));
    CHECK_PARAM((div <= 0x7F));

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, MINI_MISC_CR_FCLK_EN);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0, tmpVal);

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_FCLK_DIV, div);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_FCLK_SEL, clkSel);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0, tmpVal);

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, MINI_MISC_CR_FCLK_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, MINI_MISC_CR_FCLK_EN);
    }
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  gate pll clock cgen
 *
 * @param  clk: clock cgen, this parameter can be one of the following values:
 *           @arg GLB_MINI_MISC_FCLK
 *           @arg GLB_MINI_MISC_BCLK_SPI1
 *           @arg GLB_MINI_MISC_BCLK_UART3
 *           @arg GLB_MINI_MISC_BCLK_TIM1
 *           @arg GLB_MINI_MISC_BCLK_I2C2
 *           @arg GLB_MINI_MISC_BCLK_PWM1
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_MINI_PER_Clock_Gate(uint32_t clk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_MINI_MISC_CLOCK_TYPE(clk));

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0);
    tmpVal &= ~(1 << clk);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  enable mini system ip's clock
 *
 * @param  clk: clock cgen, this parameter can be one of the following values:
 *           @arg GLB_MINI_MISC_FCLK
 *           @arg GLB_MINI_MISC_BCLK_SPI1
 *           @arg GLB_MINI_MISC_BCLK_UART3
 *           @arg GLB_MINI_MISC_BCLK_TIM1
 *           @arg GLB_MINI_MISC_BCLK_I2C2
 *           @arg GLB_MINI_MISC_BCLK_PWM1
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_MINI_PER_Clock_UnGate(uint32_t clk)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_MINI_MISC_CLOCK_TYPE(clk));

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0);
    tmpVal |= (1 << clk);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select lpcpu jtag
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Select_LPCPU_Jtag(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_E902_JTAG_SEL);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  select npcpu jtag
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Select_NPCPU_Jtag(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_E902_JTAG_SEL);
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  halt cpu
 *
 * @param  coreID: core type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Halt_CPU(uint8_t coreID)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CORE_ID_TYPE(coreID));

    switch (coreID) {
        case GLB_CORE_ID_AP:
            // PDS_Set_MCU0_Clock_Disable();
            arch_delay_us(1);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
            break;
        case GLB_CORE_ID_NP:
            *((volatile uint32_t *)0x24000020) |= (1 << 8);
            break;
        case GLB_CORE_ID_LP:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_CTRL_PICO_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);

            break;
        default:
            return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  release cpu
 *
 * @param  coreID: core type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_Release_CPU(uint8_t coreID)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_GLB_CORE_ID_TYPE(coreID));

    switch (coreID) {
        case GLB_CORE_ID_AP:
            // PDS_Set_MCU0_Clock_Enable();
            // arch_delay_us(1);
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_CPU_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
            break;
        case GLB_CORE_ID_NP:
            *((volatile uint32_t *)0x24000020) &= (~(1 << 8));
            break;
        case GLB_CORE_ID_LP:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_CTRL_PICO_RESET);
            BL_WR_REG(GLB_BASE, GLB_SWRST_CFG2, tmpVal);
            break;
        default:
            return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  enable or disable RC32M force on
 *
 * @param  enable: Enable or disable
 *
 * @return NONE
 *
*******************************************************************************/
void GLB_RC32M_Force_On(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SOC_INFO0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_RC32M_FORCE_ON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32M_FORCE_ON);
    }
    BL_WR_REG(GLB_BASE, GLB_SOC_INFO0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  enable or disable RC32M force off
 *
 * @param  enable: Enable or disable
 *
 * @return NONE
 *
*******************************************************************************/
void GLB_RC32M_Force_Off(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SOC_INFO0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_RC32M_FORCE_OFF);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_RC32M_FORCE_OFF);
    }
    BL_WR_REG(GLB_BASE, GLB_SOC_INFO0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Get np interrupt status
 *
 * @param  None
 *
 * @return interrupt status
 *
*******************************************************************************/
uint64_t GLB_Get_NP2MINI_Interrupt_Status(void)
{
    uint64_t sts;
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CORE_CFG17);
    sts = (uint64_t)tmpVal << 32;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CORE_CFG16);
    sts |= (uint64_t)tmpVal;

    return sts;
}

/****************************************************************************/ /**
 * @brief  clrar np interrupt status
 *
 * @param  irq: IRQ number, range from 16 to 63
 *
 * @return NONE
 *
*******************************************************************************/
void GLB_Set_NP2MINI_Interrupt_Clear(uint8_t irq)
{
    if (GLB_CORE_ID_LP == GLB_Get_Core_Type()) {
        if (irq < (16 + 31)) {
            return;
        } else if (irq == (16 + 31)) {
            BL_WR_REG(GLB_BASE, GLB_CORE_CFG20, 0xFFFFFFFF);
            BL_WR_REG(GLB_BASE, GLB_CORE_CFG21, 0xFFFFFFFF);
            return;
        } else {
            irq = irq - 16 - 32;
        }
    } else {
        if (irq < 16) {
            return;
        } else {
            irq = irq - 16;
        }
    }
    if (irq < 32) {
        BL_WR_REG(GLB_BASE, GLB_CORE_CFG20, 1 << irq);
    } else if (irq < 64) {
        irq = irq - 32;
        BL_WR_REG(GLB_BASE, GLB_CORE_CFG21, 1 << irq);
    } else {
        return;
    }
}

/****************************************************************************/ /**
 * @brief  mask or unmask np2mini interrupt
 *
 * @param  enable: MASK or UNMASK
 *
 * @return NONE
 *
*******************************************************************************/
void GLB_Set_NP2MINI_Interrupt_Mask(uint8_t irq, uint8_t mask)
{
    uint32_t tmpVal;

    if (GLB_CORE_ID_LP == GLB_Get_Core_Type()) {
        if (irq < (16 + 31)) {
            return;
        } else if (irq == (16 + 31)) {
            if (mask) {
                BL_WR_REG(GLB_BASE, GLB_CORE_CFG22, 0xFFFFFFFF);
                BL_WR_REG(GLB_BASE, GLB_CORE_CFG23, 0xFFFFFFFF);
            } else {
                BL_WR_REG(GLB_BASE, GLB_CORE_CFG22, 0);
                BL_WR_REG(GLB_BASE, GLB_CORE_CFG23, 0);
            }
            return;
        } else {
            irq = irq - 16 - 32;
        }
    } else {
        if (irq < 16) {
            return;
        } else {
            irq = irq - 16;
        }
    }
    if (irq < 32) {
        tmpVal = BL_RD_REG(GLB_BASE, GLB_CORE_CFG22);
        if (mask) {
            tmpVal |= (1 << irq);
        } else {
            tmpVal &= ~(1 << irq);
        }
        BL_WR_REG(GLB_BASE, GLB_CORE_CFG22, tmpVal);
    } else if (irq < 64) {
        irq = irq - 32;
        tmpVal = BL_RD_REG(GLB_BASE, GLB_CORE_CFG23);
        if (mask) {
            tmpVal |= (1 << irq);
        } else {
            tmpVal &= ~(1 << irq);
        }
        BL_WR_REG(GLB_BASE, GLB_CORE_CFG23, tmpVal);
    } else {
        return;
    }
}

/****************************************************************************/ /**
 * @brief  Get np2mini interrupt mask
 *
 * @param  NONE
 *
 * @return np2mini interrupt mask value
 *
*******************************************************************************/
uint64_t GLB_Get_NP2MINI_Interrupt_Mask(void)
{
    uint64_t mask;
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CORE_CFG23);
    mask = (uint64_t)tmpVal << 32;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CORE_CFG22);
    mask |= (uint64_t)tmpVal;

    return mask;
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
        gpioPin[0] = 55;
        gpioPin[1] = 56;
        gpioPin[2] = 54;
        gpioPin[3] = 58;
    } else {
        gpioPin[0] = 55;
        gpioPin[1] = 56;
        gpioPin[2] = 57;
        gpioPin[3] = 58;
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
 * @brief  Get LP CPU status
 *
 * @param  None
 *
 * @return LP CPU status
 *
*******************************************************************************/
uint32_t GLB_Get_LP_CPU_Status(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2), GLB_REG_CTRL_PICO_RESET) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Get mini system status
 *
 * @param  None
 *
 * @return mini system status
 *
*******************************************************************************/
uint32_t GLB_Get_Mini_System_Status(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(GLB_BASE, GLB_SWRST_CFG2), GLB_REG_CTRL_MINI_RESET) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Set JTAG DAISY CHAIN
 *
 * @param  enable: 1:enable 0:disable
 *
 * @return NONE
 *
*******************************************************************************/
BL_Err_Type GLB_SET_JTAG_CHAIN(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM_CFG0);
    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WL_JTG_CHAIN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_WL_JTG_CHAIN);
    }
    BL_WR_REG(GLB_BASE, GLB_PARM_CFG0, tmpVal);

    return SUCCESS;
}

/*@} end of group GLB_Public_Functions */

/*@} end of group GLB */

/*@} end of group BL616D_Peripheral_Driver */
