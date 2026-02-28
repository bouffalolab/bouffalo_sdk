/**
  ******************************************************************************
  * @file    bl618dg_glb2.c
  * @brief   Auxiliary GLB tweaks (do not modify original files)
  ******************************************************************************
  */

#include "bl618dg_clock.h"
#include "bl618dg_glb.h"
#include "bl618dg_aon.h"
#include "bl618dg_hbn.h"
#include "bl618dg_pds.h"
#include "../include/hardware/bl618dg.h"
#include "../include/hardware/rf_ana1_reg.h"
#include "../include/hardware/aon_reg.h"
#include "../include/hardware/cci_reg.h"

/* Implement fine tune routine per user table. */
void ATTR_CLOCK_SECTION glb_wifipll_fine_tune_set(void)
{
    uint32_t tmpVal;
    uint32_t val;

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
