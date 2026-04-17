#if defined(BL616CL)
#include <assert.h>
#include "bflb_core.h"
#include "bl616cl_hbn.h"

/* BL616CL specific definitions */
#define RC32K_CAP_SEL_REG       HBN_BASE
#define RC32K_CAP_SEL_FIELD     HBN_RC32K_CAP_SEL_AON
#define RC32K_CODE_FR_CAL_FIELD HBN_RC32K_CODE_FR_CAL_AON

#elif defined(BL618DG)
#include <assert.h>
#include "bflb_core.h"
#include "bl618dg_hbn.h"

/* BL618DG uses AON_BASE for CAP_SEL and HBN_BASE for R_CODE */
#define RC32K_CAP_SEL_REG       AON_BASE
#define RC32K_CODE_FR_EXT_REG   HBN_BASE

#endif

#if defined(BL616CL) || defined(BL618DG)

int32_t rc32k_accuracy_ppm_calculate(uint32_t expect_time, uint32_t rc32k_actual_time)
{
    int diff_val, diff_ppm;

    if (expect_time == 0 || rc32k_actual_time == 0) {
        return 1000 * 1000;
    }

    diff_val = rc32k_actual_time - expect_time;
    diff_ppm = (int64_t)diff_val * 1000 * 1000 / expect_time;

    return diff_ppm;
}

bool rc32k_check_accuracy(int32_t actual_ppm, int32_t threshold_ppm)
{
    bool ret = false;
    uint32_t abs_ppm = 0;

    if (actual_ppm < 0) {
        abs_ppm = -actual_ppm;
    } else {
        abs_ppm = actual_ppm;
    }

    if (abs_ppm <= threshold_ppm) {
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

uint8_t rc32k_step_adjust_cal_code(bool direction)
{
    uint32_t tmpVal;
    uint32_t r_code = 0;

#if defined(BL616CL)
    /* fine calibration code for R */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_0);
    r_code = BL_GET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_CAL_AON);

    if (((r_code == 0x3FF) && (direction == false)) ||
        ((r_code == 0x0) && (direction == true))) {
        return -1;
    }

    if (direction == true) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_CAL_AON, r_code - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_CAL_AON, r_code + 1);
    }
    BL_WR_REG(HBN_BASE, HBN_RC32K_0, tmpVal);
#elif defined(BL618DG)
    /* BL618DG uses HBN_Set_RC32K_R_Code / HBN_Get_RC32K_R_Code */
    r_code = HBN_Get_RC32K_R_Code();

    if (((r_code == 0x3FF) && (direction == false)) ||
        ((r_code == 0x0) && (direction == true))) {
        return -1;
    }

    if (direction == true) {
        HBN_Set_RC32K_R_Code(r_code - 1);
    } else {
        HBN_Set_RC32K_R_Code(r_code + 1);
    }
#endif

    return 0;
}

void rc32k_get_trim_code(uint32_t* c_code, uint32_t* r_code)
{
    uint32_t tmpVal;

#if defined(BL616CL)
    /* coarse calibration code for C */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_0);
    *c_code = BL_GET_REG_BITS_VAL(tmpVal, HBN_RC32K_CAP_SEL_AON);
    *r_code = BL_GET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_CAL_AON);
#elif defined(BL618DG)
    /* BL618DG: C code from AON, R code from HBN */
    tmpVal = BL_RD_REG(AON_BASE, AON_RC32K_0);
    *c_code = BL_GET_REG_BITS_VAL(tmpVal, AON_RC32K_CAP_SEL_AON);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_CTRL0);
    *r_code = BL_GET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_EXT);
#endif
}

#endif /* BL616CL || BL618DG */
