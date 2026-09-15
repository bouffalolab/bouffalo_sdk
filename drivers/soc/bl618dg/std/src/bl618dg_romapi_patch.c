/**
  ******************************************************************************
  * @file    bl618dg_romapi_patch.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Bouffalo Lab</center></h2>
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

#include "bl618dg_romapi_patch.h"
#include "bl618dg_romdriver_e907.h"
#include "bl618dg_aon.h"
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "bl618dg_glb.h"
#include "bflb_ef_ctrl.h"

#define  PSRAM_X8_CTRL_WAIT_TIMEOUT 1000

#if defined(CPU_MODEL_A0)
#define EFUSE_TRIM_PSRAM_OFFSET      0xB4
#define EFUSE_TRIM_LDO_RC32M_OFFSET  0xB8
#define EFUSE_TRIM_XTAL_OFFSET       0xBC
#else
#define EFUSE_TRIM_PSRAM_OFFSET      0x1F0
#define EFUSE_TRIM_LDO_RC32M_OFFSET  0x1F4
#define EFUSE_TRIM_XTAL_OFFSET       0x1F8
#endif

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

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupllBasicCfg_32M_40M_52M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 4,     /*!< 4:450M~550MHz */
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
#if defined(CPU_MODEL_A0)
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
#else
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
#endif
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 35, /*!< tt50 = 0.68 */
#if !defined(CPU_MODEL_A0)
    .momUpdatePeriod = 3,
    .coarseGain = 3,
    .resv0_5_4 = 1,
#endif
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupll320MCfg_32M_40M_52M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 1,     /*!< 1:320MHz */
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
    .dtcRSel = 3,      /*!< dtc resolution select, 0: 52p~91p, 1:65p~113p, 2:74p~129p, 3: 102p~180p */
    .lfAlphaBase = 1,  /*!< change the alpha by 2^alpha_base_sel, could be 0, 1 */
    .lfAlphaExp = 2,   /*!< change the alpha by 2^(2*alpha_exp_sel),could be 0, 1, 2, 3, 4, 5 */
    .lfAlphaFast = 3,  /*!< in fast lock state, alpha can be enlarge by 2^(alpha_fast_sel), could be 0, 1, 2, 3 */
#if defined(CPU_MODEL_A0)
    .lfBetaBase = 0,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
#else
    .lfBetaBase = 3,   /*!< could be 0.5, 0.625, 0.75, 0.875 */
#endif
    .lfBetaExp = 2,    /*!< change the beta by 2^beta_exp_sel, could be 0, 1, 2, 3, 4, 5 */
    .lfBetaFast = 1,   /*!< in fast lock state, could be 0, 1, enlarge beta by 2^beta_exp_sel */
    .spdGain = 2,      /*!< 0,1,2,3 for 2/2^6, 3/2^6, 4/2^6, 5/2^6, it is the TDC gain */
    .lmsExtEn = 0,     /*!< 1'b1 enable external lms coe, and disable lms block */
    .lmsExtValue = 35, /*!< tt50 = 0.68 */
#if !defined(CPU_MODEL_A0)
    .momUpdatePeriod = 3,
    .coarseGain = 3,
    .resv0_5_4 = 1,
#endif
};

const GLB_CPUPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION cpupll640MCfg_32M_40M_52M = {
    .refdivRatio = 4,  /*!< refclk divide ratio, Fref=Fxtal/cpupll_refclk_div_ratio */
    .vcoSpeed = 7,     /*!< 1:640MHz */
#if defined(CPU_MODEL_A0)
    .vcoIdacExtra = 2, /*!< cpupll_resv0 */
#else
    .vcoIdacExtra = 0, /*!< cpupll_resv0 */
#endif
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
#if !defined(CPU_MODEL_A0)
    .momUpdatePeriod = 3,
    .coarseGain = 3,
    .resv0_5_4 = 1,
#endif
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_638p976M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                           /*!< XTAL is None */
    { &cpupll640MCfg_24M_26M, 0x1A9FB },     /*!< XTAL is 24M */
    { &cpupll640MCfg_32M_40M_52M, 0x27EF9 }, /*!< XTAL is 32M */
    { &cpupll640MCfg_32M_40M_52M, 0x18937 }, /*!< XTAL is 52M */
    { &cpupll640MCfg_32M_40M_52M, 0x1FF2E }, /*!< XTAL is 40M */
    { &cpupll640MCfg_24M_26M, 0x18937 },     /*!< XTAL is 26M */
    { &cpupll640MCfg_32M_40M_52M, 0x27EF9 }, /*!< XTAL is RC32M */
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_640M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                           /*!< XTAL is None */
    { &cpupll640MCfg_24M_26M, 0x1AAAA },     /*!< XTAL is 24M */
    { &cpupll640MCfg_32M_40M_52M, 0x28000 }, /*!< XTAL is 32M */
    { &cpupll640MCfg_32M_40M_52M, 0x189D8 }, /*!< XTAL is 52M */
    { &cpupll640MCfg_32M_40M_52M, 0x20000 }, /*!< XTAL is 40M */
    { &cpupll640MCfg_24M_26M, 0x189D8 },     /*!< XTAL is 26M */
    { &cpupll640MCfg_32M_40M_52M, 0x28000 }, /*!< XTAL is RC32M */
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_480M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                            /*!< XTAL is None */
    { &cpupllBasicCfg_24M_26M, 0x14000 },     /*!< XTAL is 24M */
    { &cpupllBasicCfg_32M_40M_52M, 0x1E000 }, /*!< XTAL is 32M */
    { &cpupllBasicCfg_32M_40M_52M, 0x12762 }, /*!< XTAL is 52M */
    { &cpupllBasicCfg_32M_40M_52M, 0x18000 }, /*!< XTAL is 40M */
    { &cpupllBasicCfg_24M_26M, 0x12762 },     /*!< XTAL is 26M */
    { &cpupllBasicCfg_32M_40M_52M, 0x1E000 }, /*!< XTAL is RC32M */
};

const GLB_CPUPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION cpupllCfg_320M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                           /*!< XTAL is None */
    { &cpupll320MCfg_24M_26M, 0xD555 },      /*!< XTAL is 24M */
    { &cpupll320MCfg_32M_40M_52M, 0x14000 }, /*!< XTAL is 32M */
    { &cpupll320MCfg_32M_40M_52M, 0xC4EC },  /*!< XTAL is 52M */
    { &cpupll320MCfg_32M_40M_52M, 0x10000 }, /*!< XTAL is 40M */
    { &cpupll320MCfg_24M_26M, 0xC4EC },      /*!< XTAL is 26M */
    { &cpupll320MCfg_32M_40M_52M, 0x14000 }, /*!< XTAL is RC32M */
};

const GLB_DSIPLL_CFG_BASIC_Type ATTR_CLOCK_CONST_SECTION dsipll1400MCfg_32M_40M_52M = {
    .refdiv_ratio   = 2,  /* XTAL pre-div so that PFD stays in valid range */
    .vco_speed      = 6,  /* VCO range index (device-specific table) */
    .vco_idac_extra = 2,  /* Extra bias current trim for VCO */
    .tdc_dly_sel    = 0,  /* TDC delay chain select */
    .dtc_r_sel      = 0,  /* DTC resolution window select */
    .lf_alpha_base  = 1,  /* Loop filter alpha base (integral weight coarse) */
    .lf_alpha_exp   = 2,  /* Alpha exponent scaling (bandwidth control) */
    .lf_alpha_fast  = 1,  /* Fast-lock extra alpha gain */
    .lf_beta_base   = 0,  /* Beta base (proportional weight) */
    .lf_beta_exp    = 3,  /* Beta exponent scaling */
    .lf_beta_fast   = 0,  /* Fast-lock beta boost enable */
    .spd_gain       = 2,  /* TDC speed gain index */
    .lms_ext_en     = 0,  /* Use internal LMS (0) */
    .lms_ext_value  = 32, /* External LMS coefficient (ignored if lms_ext_en=0) */
    .bitclk_div     = 2,  /* Output bit clock divider */
    .resv0_1_0      = 1,
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
    .resv0_1_0      = 1,
};

const GLB_DSIPLL_Cfg_Type ATTR_CLOCK_CONST_SECTION dsipllCfg_1400M[GLB_XTAL_MAX] = {
    { NULL, 0x0 },                            /*!< XTAL is None */
    { &dsipll1400MCfg_24M_26M, 0x3A555 },     /*!< XTAL is 24M */
    { &dsipll1400MCfg_32M_40M_52M, 0x57800 }, /*!< XTAL is 32M */
    { &dsipll1400MCfg_32M_40M_52M, 0x1AEC4 }, /*!< XTAL is 52M */
    { &dsipll1400MCfg_32M_40M_52M, 0x23000 }, /*!< XTAL is 40M */
    { &dsipll1400MCfg_24M_26M, 0x35D89 },     /*!< XTAL is 26M */
    { &dsipll1400MCfg_32M_40M_52M, 0x57800 }, /*!< XTAL is RC32M */
};

static void PSram_Ctrl_Request(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_CONFIG_REQ, 1);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);

    do {
        tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            break;
        }
    } while (!BL_IS_REG_BIT_SET(tmpVal, PSRAM_REG_CONFIG_GNT));
}

static void PSram_Ctrl_Release(PSRAM_ID_Type PSRAM_ID)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_CONFIG_REQ, 0);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);
}

BL_Err_Type PSram_Ctrl_Winbond_Hybrid_Sleep_Set(PSRAM_ID_Type PSRAM_ID, PSRAM_Hybrid_Sleep_Mode sleepMode)
{
    uint32_t tmpVal = 0;
    uint32_t psram_base = PSRAM_CTRL_BASE + (0x1000 * PSRAM_ID);
    uint32_t time_out = 0;

    CHECK_PARAM(IS_PSRAM_ID_TYPE(PSRAM_ID));
    CHECK_PARAM(IS_PSRAM_HYBRID_SLEEP_MODE(sleepMode));

    PSram_Ctrl_Request(PSRAM_ID);

    tmpVal = BL_RD_REG(psram_base, PSRAM_WINBOND_PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_WB_HYBRID_SLP, sleepMode);
    BL_WR_REG(psram_base, PSRAM_WINBOND_PSRAM_CONFIGURE, tmpVal);

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_WB_REG_SEL, PSRAM_WINBOND_REG_CR1);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);

    tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PSRAM_REG_CONFIG_W_PUSLE, 1);
    BL_WR_REG(psram_base, PSRAM_CONFIGURE, tmpVal);

    do {
        tmpVal = BL_RD_REG(psram_base, PSRAM_CONFIGURE);
        if (time_out++ > PSRAM_X8_CTRL_WAIT_TIMEOUT) {
            PSram_Ctrl_Release(PSRAM_ID);
            return TIMEOUT;
        }
    } while (!BL_IS_REG_BIT_SET(tmpVal, PSRAM_STS_CONFIG_W_DONE));

    PSram_Ctrl_Release(PSRAM_ID);

    return SUCCESS;
}

BL_Err_Type PSram_Ctrl_Winbond_Enter_Hybrid_Sleep(PSRAM_ID_Type PSRAM_ID)
{
    return PSram_Ctrl_Winbond_Hybrid_Sleep_Set(PSRAM_ID, PSRAM_HYBRID_SLEEP_ENABLE);
}

BL_Err_Type PSram_Ctrl_Winbond_Exit_Hybrid_Sleep(PSRAM_ID_Type PSRAM_ID)
{
    return PSram_Ctrl_Winbond_Hybrid_Sleep_Set(PSRAM_ID, PSRAM_HYBRID_SLEEP_DISABLE);
}

static const bflb_ef_ctrl_com_trim_cfg_t trim_list[] = {
    {
        .name = "auadc_gain_ext",
        .en_addr = 0x6C * 8 + 24,
        .parity_addr = 0x6C * 8 + 25,
        .value_addr = 0x6C * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "auadc_offset",
        .en_addr = 0x70 * 8 + 24,
        .parity_addr = 0x70 * 8 + 25,
        .value_addr = 0x70 * 8 + 0,
        .value_len = 24,
    },
    {
        .name = "usb20",
        .en_addr = 0x74 * 8 + 30,
        .parity_addr = 0x74 * 8 + 31,
        .value_addr = 0x74 * 8 + 23,
        .value_len = 7,
    },
    {
        .name = "dcdc12_vout",
        .en_addr = 0x74 * 8 + 21,
        .parity_addr = 0x74 * 8 + 22,
        .value_addr = 0x74 * 8 + 17,
        .value_len = 4,
    },
    {
        .name = "rc32k",
        .en_addr = 0x74 * 8 + 15,
        .parity_addr = 0x74 * 8 + 16,
        .value_addr = 0x74 * 8 + 11,
        .value_len = 4,
    },
    {
        .name = "rcal_iptat",
        .en_addr = 0x74 * 8 + 9,
        .parity_addr = 0x74 * 8 + 10,
        .value_addr = 0x74 * 8 + 4,
        .value_len = 5,
    },
    {
        .name = "gpadc2_offset",
        .en_addr = 0x78 * 8 + 16,
        .parity_addr = 0x78 * 8 + 17,
        .value_addr = 0x78 * 8 + 0,
        .value_len = 16,
    },
    {
        .name = "psram_trim",
        .en_addr = EFUSE_TRIM_PSRAM_OFFSET * 8 + 12,
        .parity_addr = EFUSE_TRIM_PSRAM_OFFSET * 8 + 11,
        .value_addr = EFUSE_TRIM_PSRAM_OFFSET * 8 + 0,
        .value_len = 11,
    },
    {
        .name = "ldo09",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 30,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 31,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 26,
        .value_len = 4,
    },
    {
        .name = "ldo08",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 24,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 25,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 20,
        .value_len = 4,
    },
    {
        .name = "ldo18",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 18,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 19,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 14,
        .value_len = 4,
    },
    {
        .name = "rc32m",
        .en_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 12,
        .parity_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 13,
        .value_addr = EFUSE_TRIM_LDO_RC32M_OFFSET * 8 + 4,
        .value_len = 8,
    },
    {
        .name = "xtal0",
        .en_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 30,
        .parity_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 31,
        .value_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 22,
        .value_len = 8,
    },
    {
        .name = "xtal1",
        .en_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 20,
        .parity_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 21,
        .value_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 12,
        .value_len = 8,
    },
    {
        .name = "xtal2",
        .en_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 10,
        .parity_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 11,
        .value_addr = EFUSE_TRIM_XTAL_OFFSET * 8 + 2,
        .value_len = 8,
    },
    {
        .name = "tsen",
        .en_addr = 0xD0 * 8 + 30,
        .parity_addr = 0xD0 * 8 + 31,
        .value_addr = 0xD0 * 8 + 18,
        .value_len = 12,
    },
    {
        .name = "gpadc_gain",
        .en_addr = 0xD0 * 8 + 16,
        .parity_addr = 0xD0 * 8 + 17,
        .value_addr = 0xD0 * 8 + 4,
        .value_len = 12,
    },
    {
        .name = "gpadc_vref",
        .en_addr = 0xD4 * 8 + 30,
        .parity_addr = 0xD4 * 8 + 31,
        .value_addr = 0xD4 * 8 + 24,
        .value_len = 6,
    },
    {
        .name = "gpadc_offset",
        .en_addr = 0xD4 * 8 + 16,
        .parity_addr = 0xD4 * 8 + 17,
        .value_addr = 0xD4 * 8 + 0,
        .value_len = 16,
    },
    {
        .name = "gpadc2_gain",
        .en_addr = 0xD8 * 8 + 30,
        .parity_addr = 0xD8 * 8 + 31,
        .value_addr = 0xD8 * 8 + 18,
        .value_len = 12,
    },
    {
        .name = "gpadc2_vref",
        .en_addr = 0xD8 * 8 + 16,
        .parity_addr = 0xD8 * 8 + 17,
        .value_addr = 0xD8 * 8 + 10,
        .value_len = 6,
    },
    {
        .name = "rcal_icx",
        .en_addr = 0xD8 * 8 + 8,
        .parity_addr = 0xD8 * 8 + 9,
        .value_addr = 0xD8 * 8 + 2,
        .value_len = 6,
    },
    {
        .name = "auadc_gain_int",
        .en_addr = 0xDC * 8 + 24,
        .parity_addr = 0xDC * 8 + 25,
        .value_addr = 0xDC * 8 + 0,
        .value_len = 24,
    }
};

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo09_Soc_Slow_Pulldown(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);

    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO09SOC_SLOW_PLD_AON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO09SOC_SLOW_PLD_AON);
    }

    BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Ctrl_Ldo18_Aon_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
#if defined(CPU_MODEL_A0)
    tmpVal ^= (0x7 << 4);
#endif
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO18AON_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO18AON_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Ctrl_Dcdc_Sys_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
#if defined(CPU_MODEL_A0)
    tmpVal ^= (0x7 << 4);
#endif
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_DCDC12_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_DCDC12_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Ctrl_Ldo_Soc_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
#if defined(CPU_MODEL_A0)
    tmpVal ^= (0x7 << 4);
#endif
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO09SOC_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO09SOC_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo18_Aon_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO18_AON_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
#if defined(CPU_MODEL_A0)
    tmpVal ^= (0x7 << 4);
#endif

    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO18AON_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_LP_EN_AON, mode);

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Dcdc_Sys_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_DCDC_SYS_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
#if defined(CPU_MODEL_A0)
    tmpVal ^= (0x7 << 4);
#endif

    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_DCDC12_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_LP_EN_AON, mode);

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo_Soc_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO_SOC_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
#if defined(CPU_MODEL_A0)
    tmpVal ^= (0x7 << 4);
#endif

    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO09SOC_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_LP_EN_AON, mode);

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Enable_Dcdc09(uint8_t gpio)
{
    uint32_t tmpVal;
    uint32_t pwrReqMask;
    uint32_t pwrReqEn;

    CHECK_PARAM((gpio <= 7));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_2);

    pwrReqMask = BL_GET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_MASK);
    pwrReqEn = BL_GET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_EN);

    pwrReqMask &= ~(1U << gpio);
    pwrReqEn |= (1U << gpio);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_MASK, pwrReqMask);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_PWR_REQ_EN, pwrReqEn);

    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_2, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo09_Aon_Force_Off(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_2);

    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_LDO09_AON_4S0);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_LDO09_AON_4S0);
    }

    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_2, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_RC32K_Half_MSB(uint8_t enable)
{
    const uint32_t half_msb_mask = 1UL << (AON_RESV_AON_POS + 1U);
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_1);
    if (enable) {
        tmpVal |= half_msb_mask;
    } else {
        tmpVal &= ~half_msb_mask;
    }
    BL_WR_REG(AON_BASE, AON_1, tmpVal);

    return SUCCESS;
}

uint8_t ATTR_CLOCK_SECTION HBN_Get_RC32K_Half_MSB(void)
{
    const uint32_t half_msb_mask = 1UL << (AON_RESV_AON_POS + 1U);

    return (BL_RD_REG(AON_BASE, AON_1) & half_msb_mask) != 0U;
}

uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg_t **ptrim_list)
{
    *ptrim_list = &trim_list[0];
    return sizeof(trim_list) / sizeof(trim_list[0]);
}

/****************************************************************************/ /**
 * @brief  Efuse write optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  program: Whether program
 *
 * @return 0 or -1
 *
*******************************************************************************/
int bflb_efuse_write_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval;
    uint32_t i = 0, cnt;

    if (slot >= 3) {
        return -1;
    }

    /* Change to local order */
    for (i = 0; i < 3; i++) {
        tmpval = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmpval;
    }

    /* The low 32 bits */
    tmpval = BL_RDWD_FRM_BYTEP(maclow);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, 0x14, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, 0x110, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, 0x118, &tmpval, 1, program);
    }

    /* The high 16 bits */
    tmpval = machigh[0] + (machigh[1] << 8);
    cnt = 0;

    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    tmpval |= ((cnt & 0x3f) << 16);

    if (slot == 0) {
        bflb_ef_ctrl_write_direct(NULL, 0x18, &tmpval, 1, program);
    } else if (slot == 1) {
        bflb_ef_ctrl_write_direct(NULL, 0x114, &tmpval, 1, program);
    } else if (slot == 2) {
        bflb_ef_ctrl_write_direct(NULL, 0x11C, &tmpval, 1, program);
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Trim LDO_SOC According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo_Soc_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo09", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo08_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo08_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo08", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO08AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo18_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo18_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Trim_RC32M(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 8)) {
            tmpVal = BL_RD_REG(PDS_BASE, PDS_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_RC32M_EXT_CODE_EN);
            BL_WR_REG(PDS_BASE, PDS_RC32M_CTRL0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL2_AON);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_RC32M_CODE_FR_EXT2, trim.value);
            BL_WR_REG(AON_BASE, AON_RC32M_CTRL2_AON, tmpVal);
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL2_AON);
            tmpVal = BL_SET_REG_BIT(tmpVal, AON_RC32M_EXT_CODE_SEL);
            BL_WR_REG(AON_BASE, AON_RC32M_CTRL2_AON, tmpVal);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Set the RC32K-to-XTAL counter averaging window
 *
 * @param  cycle: XTAL_CNT_32K.reg_total_32k_cycle[22:20]
 *                0: 4 cycles, 1: 8 cycles, 2: 16 cycles, 3: 32 cycles,
 *                4~7: 64 cycles
 *
 * @return SUCCESS
 *
 * @note   XTAL_CNT_32K is at PDS_BASE + 0x50 (0x4000E050). This field selects
 *         how many 32K cycles are averaged when converting RC32K to XTAL cycles.
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_32K_Cycle(uint32_t cycle)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_REG_TOTAL_32K_CYCLE, cycle);
    BL_WR_REG(PDS_BASE, PDS_XTAL_CNT_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable hardware RC32K counting after PDS
 *
 * @return SUCCESS
 *
 * @note   Sets XTAL_CNT_32K.cr_pds_xtal_cnt_rc32k_en[24] at
 *         PDS_BASE + 0x50 (0x4000E050). When enabled, hardware automatically
 *         counts RC32K against the XTAL counter after PDS.
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Xtal_Cnt_32K_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_XTAL_CNT_RC32K_EN);
    BL_WR_REG(PDS_BASE, PDS_XTAL_CNT_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable hardware RC32K counting after PDS
 *
 * @return SUCCESS
 *
 * @note   Clears XTAL_CNT_32K.cr_pds_xtal_cnt_rc32k_en[24] at
 *         PDS_BASE + 0x50 (0x4000E050).
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Xtal_Cnt_32K_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_XTAL_CNT_RC32K_EN);
    BL_WR_REG(PDS_BASE, PDS_XTAL_CNT_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get RC32K-to-XTAL counter done status
 *
 * @return SET if XTAL_CNT_32K.xtal_cnt_32k_done[30] is set, otherwise RESET
 *
 * @note   The done status is read from PDS_BASE + 0x50 (0x4000E050)[30] and is
 *         also connected to IRQ[33]. Clear it through GLB_XTAL_DEG_32K[28]
 *         (0x40000B80[28]); the clear bit was moved out of this PDS register.
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Xtal_Cnt_32K_Is_Done(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_XTAL_CNT_32K_DONE);

    return tmpVal ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Get the RC32K-to-XTAL counter result
 *
 * @param  count: average 32K cycle count in XTAL cycles,
 *                XTAL_CNT_32K.ro_xtal_cnt_32k_cnt[18:6]
 * @param  res: residue in units of 1/64 XTAL cycle,
 *              XTAL_CNT_32K.ro_xtal_cnt_32k_res[5:0]
 *
 * @return SUCCESS
 *
 * @note   Result fields are read from PDS_BASE + 0x50 (0x4000E050). The residue
 *         corresponds to the averaging window selected by reg_total_32k_cycle.
 *
*******************************************************************************/
BL_Err_Type PDS_Xtal_Cnt_32K_Get_Result(uint32_t *count, uint32_t *res)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
    *count = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_XTAL_CNT_32K_CNT);
    *res = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_XTAL_CNT_32K_RES);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select top miscellaneous XTAL source
 *
 * @param  sel: GLB_XTAL_DEG_32K.top_misc_xtal_sel[9:8]
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type GLB_Set_Top_Misc_Xtal(uint8_t sel)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_TOP_MISC_XTAL_SEL, sel);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Trigger an RC32K-to-XTAL counter process
 *
 * @return SUCCESS
 *
 * @note   The software trigger and done-clear controls were moved from
 *         PDS XTAL_CNT_32K to GLB_XTAL_DEG_32K (0x40000B80):
 *         [31] xtal_cnt_32k_sw_trig_ps is W1P and sets
 *              PDS XTAL_CNT_32K.xtal_cnt_32k_process[29];
 *         [28] clr_xtal_cnt_32k_done is W1P and clears
 *              PDS XTAL_CNT_32K.xtal_cnt_32k_done[30];
 *         [27] xtal_cnt_32k_cgen gates the counter clock.
 *
*******************************************************************************/
BL_Err_Type GLB_Trigger_Xtal_Cnt_32K_Process(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal |= GLB_XTAL_CNT_32K_CGEN_MSK;
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    /* Clear the previous PDS XTAL_CNT_32K.done status through GLB bit[28]. */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal |= GLB_CLR_XTAL_CNT_32K_DONE_MSK;
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    /* Start a new count process; GLB bit[31] drives PDS process bit[29]. */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    tmpVal |= GLB_XTAL_CNT_32K_SW_TRIG_PS_MSK;
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  swap SPI1 MOSI with MISO
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Swap_MCU_SPI_1_MOSI_With_MISO(uint8_t enable)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_HW_RSV3);
    if (enable) {
        tmpVal |= (1 << 30);
    } else {
        tmpVal &= ~(1 << 30);
    }
    BL_WR_REG(GLB_BASE, GLB_HW_RSV3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc vref trim
 *
 * @param [in] dev ADC device handle
 *
 * @return vref trim value
 *
*******************************************************************************/
uint32_t bflb_efuse_get_adc_vref_trim(struct bflb_device_s *dev)
{
    bflb_ef_ctrl_com_trim_t trim;
    /* use hardware default value, also the median value. */
    uint32_t default_value = 32;

    if (dev->idx == 0) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc_vref", &trim, 1);
    } else if (dev->idx == 1) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc2_vref", &trim, 1);
    } else {
        return default_value;
    }

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            default_value = trim.value;
        }
    }
    return default_value;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc offset trim
 *
 * @param [in] dev ADC device handle
 *
 * @return offset trim value
 *
*******************************************************************************/
int32_t bflb_efuse_get_adc_offset_trim(struct bflb_device_s *dev)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t default_value = 0;

    if (dev->idx == 0) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc_offset", &trim, 1);
    } else if (dev->idx == 1) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc2_offset", &trim, 1);
    } else {
        return default_value;
    }

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            /* trim.value is a 9-bits signed value */
            if (trim.value & 0x8000) {
                default_value = trim.value | 0xFFFF0000;
            } else {
                default_value = trim.value & 0xFFFF;
            }
        }
    }
    return default_value;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc gain trim
 *
 * @param [in] dev ADC device handle
 *
 * @return coe
 *
*******************************************************************************/
float bflb_efuse_get_adc_gain_trim(struct bflb_device_s *dev)
{
    bflb_ef_ctrl_com_trim_t trim;
    uint32_t tmp;

    float coe = 1.0f;

    if (dev->idx == 0) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc_gain", &trim, 1);
    } else if (dev->idx == 1) {
        bflb_ef_ctrl_read_common_trim(NULL, "gpadc2_gain", &trim, 1);
    } else {
        return coe;
    }

    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            tmp = trim.value;

            if (tmp & 0x800) {
                tmp = ~tmp;
                tmp += 1;
                tmp = tmp & 0xfff;
                coe = (1.0f + ((float)tmp / 2048.0f));
            } else {
                coe = (1.0f - ((float)tmp / 2048.0f));
            }
        }
    }
    return coe;
}

/****************************************************************************/ /**
 * @brief  Efuse read adc tsen trim
 *
 * @param  None
 *
 * @return int
 *
*******************************************************************************/
uint32_t bflb_efuse_get_adc_tsen_trim(void)
{
    bflb_ef_ctrl_com_trim_t trim;

    bflb_ef_ctrl_read_common_trim(NULL, "tsen", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            return trim.value;
        }
    }

    return 2300;
}

/****************************************************************************/ /**
 * @brief  Efuse set AES key read/write lock
 *
 * @param  key_index: AES key index
 * @param  rd_lock: read lock enable
 * @param  wr_lock: write lock enable
 *
 * @return 0 for success, -1 for unsupported key index
 *
*******************************************************************************/
int bflb_efuse_rw_lock_aes_key(uint8_t key_index, uint8_t rd_lock, uint8_t wr_lock)
{
    uint32_t tmpval = 0;

    if(0 == key_index){
        if(wr_lock){
            tmpval |= (1 << 17);
        }
        if(rd_lock){
            tmpval |= (1 << 27);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(1 == key_index){
        if(wr_lock){
            tmpval |= (1 << 18);
        }
        if(rd_lock){
            tmpval |= (1 << 28);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(2 == key_index){
         if(wr_lock){
            tmpval |= (1 << 19);
        }
        if(rd_lock){
            tmpval |= (1 << 29);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(3 == key_index){
         if(wr_lock){
            tmpval |= (1 << 20);
        }
        if(rd_lock){
            tmpval |= (1 << 30);
        }
        bflb_ef_ctrl_write_direct(NULL, 0x7C, &tmpval, 1, 1);
    }else if(4 == key_index){
        if(wr_lock){
            tmpval |= (1 << 15);
        }
        if(rd_lock){
            tmpval |= (1 << 25);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(5 == key_index){
         if(wr_lock){
            tmpval |= (1 << 16);
        }
        if(rd_lock){
            tmpval |= (1 << 26);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(6 == key_index){
         if(wr_lock){
            tmpval |= (1 << 17);
        }
        if(rd_lock){
            tmpval |= (1 << 27);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(7 == key_index){
         if(wr_lock){
            tmpval |= (1 << 18);
        }
        if(rd_lock){
            tmpval |= (1 << 28);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else if(10 == key_index){
         if(wr_lock){
            tmpval |= (1 << 21);
        }
        if(rd_lock){
            tmpval |= (1 << 31);
        }
        bflb_ef_ctrl_write_direct(NULL, 0xFC, &tmpval, 1, 1);
    }else{
        return -1;
    }
    return 0;
}

void Tzc_Sec_Set_CPU_Group(uint8_t cpu, uint8_t group)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    tmpVal = BL_RD_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID);
    tmpVal2 = BL_RD_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID_LOCK);

    if (cpu == GLB_CORE_ID_AP) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_CPU_TZMID, group);
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, TZC_SEC_TZC_CPU_TZMID_LOCK, 1);
    } else if (cpu == GLB_CORE_ID_NP) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_WMCU_TZMID, group);
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, TZC_SEC_TZC_WMCU_TZMID_LOCK, 1);
    } else if (cpu == GLB_CORE_ID_LP) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_PICO_TZMID, group);
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, TZC_SEC_TZC_PICO_TZMID_LOCK, 1);
    }

    BL_WR_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID, tmpVal);
    BL_WR_REG(TZ1_BASE, TZC_SEC_TZC_BMX_TZMID_LOCK, tmpVal2);
}

static inline uint32_t Clock_SPI3_Clk_Mux_Output(uint8_t sel)
{
    uint32_t tmpVal;
    uint8_t muxpll_160m_sel;

    if (sel == 0) {
        /* muxpll 160MHz */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG_CLK_CFG1);
        muxpll_160m_sel = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_TOP_MUXPLL_160M_SEL);

        if (muxpll_160m_sel == 0) {
            return 160 * 1000 * 1000;
        } else {
            return 0;
        }
    } else {
        /* mini xclk */
        return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MINI_XCLK);
    }
}

/****************************************************************************/ /**

 * @brief  Get Peripheral1 Clock
 *
 * @param  type: Peripheral1 clock type
 *
 * @return Peripheral1 clock value
 *
*******************************************************************************/
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type)
{
    uint32_t clock = 0;
    uint32_t div = 0;

    if (type == BL_PERIPHERAL_CLOCK_SPI3) {
        uint32_t tmpVal;
        uint8_t clk_sel;

        tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_CLOCK_1);
        clk_sel = BL_GET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_SEL);
        div = BL_GET_REG_BITS_VAL(tmpVal, MINI_MISC_CR_SPI_CLK_DIV);
        clock = Clock_SPI3_Clk_Mux_Output(clk_sel);
        
        return clock / (div + 1);
    } else {
        return RomDriver_Clock_Peripheral_Clock_Get(type);
    }
}

uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx)
{
    if (type == BFLB_DEVICE_TYPE_SPI && idx == 3) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SPI3);
    } else {
        return RomDriver_bflb_clk_get_peripheral_clock(type, idx);
    }
}

/****************************************************************************/ /**
 * @brief      delay us
 *
 * @param[in]  core:  systemcoreclock
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
#ifdef ARCH_RISCV
__WEAK
void ATTR_TCM_SECTION ASM_Delay_Us(uint32_t core, uint32_t cnt, uint32_t loopT)
{
    volatile uint32_t divVal = loopT;
    volatile uint32_t speed = 0;
    volatile uint32_t cycNum = 0;

    /* 1M=100K*10, so multiple is 10 */
    /* loop function take 4 instructions, so instructionNum is 4 */
    /* divVal = multiple*instructionNum */

    if (core >= 1 * 1000 * 1000) {
        /* CPU clock >= 1MHz */
        speed = core / (1000 * 1000);
        cycNum = speed * cnt;
        cycNum = cycNum / divVal;
        /* cycNum >= 0 */
    } else {
        /* CPU clock < 1MHz */
        speed = core / 1000;
        cycNum = speed * cnt;
        cycNum = cycNum / 1000;
        cycNum = cycNum / divVal;
        /* cycNum >= 0 */
    }

    if (!cycNum) {
        return;
    }

    __asm__ __volatile__(
        "mv       a4,%0\n\t"
        "li       a5,0x0\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        ".align 4\n\t"
        "1  :\n"
#if defined(CPU_MODEL_A0)
        "beq      a5,a4,2f\n\t"
        "addi     a5,a5,0x1\n\t"
        "j        1b\n\t"
#else
        "addi     a5,a5,0x1\n\t"
        "bne      a5,a4,1b\n\t"
#endif
        "nop\n\t"
        "nop\n\t"
        "2   :\n\t"
        "nop\n"
        :             /* output */
        : "r"(cycNum) /* input */
        : "a4", "a5"  /* destruct description */
    );
}
#endif

/****************************************************************************/ /**
 * @brief      delay us
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
__WEAK
void ATTR_TCM_SECTION arch_delay_us(uint32_t cnt)
{
    uint32_t coreFreq;
#if defined(CPU_MODEL_A0)
    uint32_t loopTick = 3;
#else
    uint32_t loopTick = 1;
#endif
#if defined(__riscv_xthead) || defined(__riscv_xtheadc)
#if ((__ICACHE_PRESENT == 1U) && (__DCACHE_PRESENT == 1U))
    uint32_t iCacheEn;
    uint32_t dCacheEn;

    /* requirement: icache enable && dcache enable */
    /* otherwise the latency depends on the code address */
    iCacheEn = (__get_MHCR() & CACHE_MHCR_IE_Msk) >> CACHE_MHCR_IE_Pos;
    dCacheEn = (__get_MHCR() & CACHE_MHCR_DE_Msk) >> CACHE_MHCR_DE_Pos;

#if defined(CPU_MODEL_A0)
    if (iCacheEn && dCacheEn) {
        loopTick = 3;
    } else if (iCacheEn && !dCacheEn) {
        loopTick = 4;
    } else if (!iCacheEn && !dCacheEn) {
        loopTick = 24;
    } else {
        /* not recommend */
    }
#else
    if (iCacheEn && dCacheEn) {
        loopTick = 1;
    } else if (iCacheEn && !dCacheEn) {
        loopTick = 4;
    } else if (!iCacheEn && !dCacheEn) {
        loopTick = 24;
    } else {
        /* not recommend */
    }
#endif
#endif
#endif

#if defined(DUAL_CORE)
    GLB_CORE_ID_Type coreID;
    coreID = GLB_Get_Core_Type();
    switch (coreID) {
        case GLB_CORE_ID_AP:
#endif
            coreFreq = Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
#if defined(DUAL_CORE)
            break;
        default:
            coreFreq = 32 * 1000 * 1000;
            break;
    }
#endif

    coreFreq = coreFreq ? coreFreq : (32 * 1000 * 1000);

    ASM_Delay_Us(coreFreq, cnt, loopTick);
}

/****************************************************************************/ /**
 * @brief      delay ms
 *
 * @param[in]  cnt:  delay cnt ms
 *
 * @return none
 *
 *******************************************************************************/
__WEAK
void ATTR_TCM_SECTION arch_delay_ms(uint32_t cnt)
{
    uint32_t i = 0;
    uint32_t count = 0;

    if (cnt >= 1024) {
        /* delay (n*1024) ms */
        for (i = 0; i < (cnt / 1024); i++) {
            arch_delay_us(1024 * 1000);
        }
    }

    count = cnt & 0x3FF;

    if (count) {
        /* delay (1-1023)ms */
        arch_delay_us(count * 1000);
    }
}

/****************************************************************************/ /**
 * @brief  GLB set CPUPLL to 632.2176M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_CPUPLL_To_632P2176M_By_Sdmin(void)
{
    uint32_t tmpVal, sdmin;
    uint8_t xtalType;

    if (HBN_Get_Xtal_Type(&xtalType) == SUCCESS) {
        switch (xtalType) {
            case GLB_XTAL_24M: sdmin = 0x1A57A; break;
            case GLB_XTAL_32M: sdmin = 0x27837; break;
            case GLB_XTAL_52M: sdmin = 0x1850E; break;
            case GLB_XTAL_40M: sdmin = 0x1F9C6; break;
            case GLB_XTAL_26M: sdmin = 0x1850E; break;
            case GLB_XTAL_RC32M: sdmin = 0x27837; break;
            default: sdmin = 0x1F9C6; break;
        }
    } else {
        sdmin = 0x1F9C6;
    }

    /* set cpupll_sdmin */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SDM_IN, sdmin);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GLB set CPUPLL to 638.976M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION GLB_Config_CPUPLL_To_638P976M_By_Sdmin(void)
{
    uint32_t tmpVal, sdmin;
    uint8_t xtalType;

    if (HBN_Get_Xtal_Type(&xtalType) == SUCCESS) {
        switch (xtalType) {
            case GLB_XTAL_24M: sdmin = 0x1A9FB; break;
            case GLB_XTAL_32M: sdmin = 0x27EF9; break;
            case GLB_XTAL_52M: sdmin = 0x18937; break;
            case GLB_XTAL_40M: sdmin = 0x1FF2E; break;
            case GLB_XTAL_26M: sdmin = 0x18937; break;
            case GLB_XTAL_RC32M: sdmin = 0x27EF9; break;
            default: sdmin = 0x1FF2E; break;
        }
    } else {
        sdmin = 0x1FF2E;
    }

    /* set cpupll_sdmin */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_SDM_IN, sdmin);
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SDM1_OFFSET, tmpVal);

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
#if !defined(CPU_MODEL_A0)
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_COARSE_GAIN, cfg->basicCfg->coarseGain);
#endif
    BL_WR_WORD(CCI_BASE + CCI_CPUPLL_SPD_FCAL_OFFSET, tmpVal);

    /* set alpha_base, alpha_exp, alpha_fast, beta_base, beta_exp, beta_fast */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_CPUPLL_LF_VCTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_ALPHA_BASE, cfg->basicCfg->lfAlphaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_ALPHA_EXP, cfg->basicCfg->lfAlphaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_ALPHA_FAST, cfg->basicCfg->lfAlphaFast);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_BETA_BASE, cfg->basicCfg->lfBetaBase);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_BETA_EXP, cfg->basicCfg->lfBetaExp);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_LF_BETA_FAST, cfg->basicCfg->lfBetaFast);
#if !defined(CPU_MODEL_A0)
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_CPUPLL_MOM_UPDATE_PERIOD, cfg->basicCfg->momUpdatePeriod);
#endif
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
#if !defined(CPU_MODEL_A0)
    val &= ~30;
    val |= cfg->basicCfg->resv0_5_4 << 4;
#endif
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
    uint32_t val = 0;
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

    /* set bitclk_div */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_BICLK_DIV, cfg->basicCfg->bitclk_div);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET, tmpVal);

    /* set resv0 */
    tmpVal = BL_RD_WORD(DSI_PLL_BASE + DSIPLL_RESERVE_OFFSET);
    val = BL_GET_REG_BITS_VAL(tmpVal, DSIPLL_RESV0);
    val &= ~0x3;
    val |= cfg->basicCfg->resv0_1_0 << 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DSIPLL_RESV0, val);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_RESERVE_OFFSET, tmpVal);

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
    tmpVal = BL_SET_REG_BIT(tmpVal, DSI_MIPLPLL_CLK_EN);
    BL_WR_WORD(DSI_PLL_BASE + DSIPLL_CLKTREE_OFFSET, tmpVal);

    if (waitStable) {
        /* Wait 1.5*180us    */
        arch_delay_us(270);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All WRAM(320KB) Retention
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_WRAM_Retention(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0xFFF);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Whether MAC address slot is empty
 *
 * @param  slot: MAC address slot
 * @param  reload: whether  reload to check
 *
 * @return 0 for all slots full,1 for others
 *
*******************************************************************************/
uint8_t bflb_efuse_is_mac_address_slot_empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff, tmp2 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x14, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x18, &tmp2, 1, reload);
    } else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, 0x110, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x114, &tmp2, 1, reload);
    } else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, 0x118, &tmp1, 1, reload);
        bflb_ef_ctrl_read_direct(NULL, 0x11C, &tmp2, 1, reload);
    }

    part1Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp1, 0, 32));
    part2Empty = (bflb_ef_ctrl_is_all_bits_zero(tmp2, 0, 22));

    return (part1Empty && part2Empty);
}

/****************************************************************************/ /**
 * @brief  Efuse read optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[6]: MAC address buffer
 * @param  reload: Whether reload
 *
 * @return 0 or -1
 *
*******************************************************************************/
int bflb_efuse_read_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpval = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return -1;
    }

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x14, &tmpval, 1, reload);
    }else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, 0x110, &tmpval, 1, reload);
    }else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, 0x118, &tmpval, 1, reload);
    }
    BL_WRWD_TO_BYTEP(maclow, tmpval);

    if (slot == 0) {
        bflb_ef_ctrl_read_direct(NULL, 0x18, &tmpval, 1, reload);
    }else if (slot == 1) {
        bflb_ef_ctrl_read_direct(NULL, 0x114, &tmpval, 1, reload);
    }else if (slot == 2) {
        bflb_ef_ctrl_read_direct(NULL, 0x11C, &tmpval, 1, reload);
    }

    machigh[0] = tmpval & 0xff;
    machigh[1] = (tmpval >> 8) & 0xff;

    /* Check parity */
    for (i = 0; i < 6; i++) {
        cnt += bflb_ef_ctrl_get_byte_zero_cnt(mac[i]);
    }

    if ((cnt & 0x3f) == ((tmpval >> 16) & 0x3f)) {
        /* Change to network order */
        for (i = 0; i < 3; i++) {
            tmpval = mac[i];
            mac[i] = mac[5 - i];
            mac[5 - i] = tmpval;
        }
        return 0;
    } else {
        return -1;
    }
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

#if defined(CPU_MODEL_A0)
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
#endif
#if 0
    /* WIFIPLL HW CTRL @ 0x200010D4 */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_WIFIPLL_HW_CTRL_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_VCO_RSHT_EN_TX_BZ, 0);/* [16]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_PI_BYPASS_RX_BZ, 1);  /* [6]   = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_PI_BYPASS_RX, 1);     /* [2]   = 1 */
    BL_WR_WORD(CCI_BASE + CCI_WIFIPLL_HW_CTRL_OFFSET, tmpVal);

    /* WIFIPLL PI/SDM/LMS @ 0x200010E4 */
    tmpVal = BL_RD_WORD(CCI_BASE + CCI_WIFIPLL_PI_SDM_LMS_OFFSET);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_PI_BYPASS, 1);        /* [31]  = 1 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_SDM_BYPASS, 0);       /* [24]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_SDM_DITH_FORCE_EN, 0);/* [23]  = 0 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CCI_WIFIPLL_SDM_DITH_EN, 0);      /* [22]  = 0 */
    BL_WR_WORD(CCI_BASE + CCI_WIFIPLL_PI_SDM_LMS_OFFSET, tmpVal);
#endif
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