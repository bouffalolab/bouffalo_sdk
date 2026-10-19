#include "bl_lp.h"
#include "hbn_reg.h"
#include "bl616cl_aon.h"
#include "bl616cl_glb.h"
#include "bl616cl_hbn.h"
#include <stdint.h>

#define LP_ABS(a)     (((a) > 0) ? (a) : (-(a)))
#define LP_DIFF(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))
#define LP_RC32K_R_CODE_MASK      0x3ffU
#define LP_RC32K_HALF_MSB_FLAG    (1U << 10)

static uint32_t bl_lp_rc32k_encode_state(uint32_t code)
{
    uint32_t state = code & LP_RC32K_R_CODE_MASK;

#if CONFIG_RC32K_CAL_ENABLE_HALF_MSB
    if (HBN_Get_RC32K_Half_MSB()) {
        state |= LP_RC32K_HALF_MSB_FLAG;
    }
#endif

    /* LP state stores the raw 10-bit R code plus the AON half-MSB selector in bit 10. */
    return state;
}

/* Calibrate RC32K and update the beacon timestamp */
int lpfw_recal_rc32k(lp_fw_rc32k_trim_t *p_rc32k_trim_param, uint64_t beacon_timestamp_now_us,
                     uint64_t rtc_timestamp_now_us, uint32_t mode, int clock_ready_check)
{
    uint64_t rtc_timestamp_last_us, beacon_timestamp_last_us;
    int64_t rtc_us, beacon_us;
    int diff_us, diff_ppm;
    int ret = 0;

    /* Check if rc32k_trim_parameter is valid */
    if (!p_rc32k_trim_param) {
        return -5;
    }

    /* get last timestamp */
    rtc_timestamp_last_us = p_rc32k_trim_param->last_rc32trim_stamp_rtc_us;
    beacon_timestamp_last_us = p_rc32k_trim_param->last_rc32trim_stamp_beacon_us;

    if (clock_ready_check && (bl_lp_get_32k_clock_ready() == 0)) {
        /* wait 32k_clock ready */
        ret = -1;
        p_rc32k_trim_param->last_rc32trim_stamp_valid = 0;
        bl_lp_set_32k_trim_ready(0);
        return ret;
    }

    if (p_rc32k_trim_param->last_rc32trim_stamp_valid == 0) {
        ret = -2;
        /* update timestamp */
        goto update_tsf;
    }

    if (rtc_timestamp_now_us < rtc_timestamp_last_us || beacon_timestamp_now_us < beacon_timestamp_last_us) {
        /* The timestamp is abnormal. Abort calibration */
        ret = -3;
        /* update timestamp */
        if (mode == BEACON_STAMP_APP) {
            goto update_tsf;
        } else {
            return ret;
        }
    }

    if (p_rc32k_trim_param->rc32k_auto_cal_en == 0) {
        ret = 7;
        /* update timestamp */
        goto update_tsf;
    }

    /* Obtaining Duration */
    rtc_us = rtc_timestamp_now_us - rtc_timestamp_last_us;
    beacon_us = beacon_timestamp_now_us - beacon_timestamp_last_us;
    /* Time difference */
    diff_us = rtc_us - beacon_us;
    diff_ppm = (int64_t)diff_us * 1000 * 1000 / beacon_us;

    if (LP_ABS(diff_ppm) > 10 * 1000) {
        /* The error is too large. Abort calibration */
        ret = -4;
        if (mode == BEACON_STAMP_APP) {
            goto update_tsf;
        } else {
            return ret;
        }
    }

    if (rtc_us > 5 * 1000 * 1000 || beacon_us > 5 * 1000 * 1000) {
        /* The time span is too large. Abort calibration */
        ret = 5;
        /* update timestamp */
        goto update_tsf;
    }

    if (rtc_us < 250 * 1000 || beacon_us < 250 * 1000) {
        /* Time is too short, accuracy is too low, abandon calibration */
        ret = 6;
        /* not update timestamp !!! */
        return ret;
    }

    if (clock_ready_check && (bl_lp_get_32k_trim_ready() == 0)) {
        /*  */
        ret = 8;
        if (LP_DIFF(p_rc32k_trim_param->rtc32k_error_ppm, diff_ppm) < 1500) {
            /* error ppm diff < 1500, trim ready, (allow dtim sleep) */
            bl_lp_set_32k_trim_ready(1);
            ret = 9;
        }
    }

    if (BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_GLB), HBN_F32K_SEL)) {
        /* rtc use xtal32k, only soft recal */
        p_rc32k_trim_param->rtc32k_error_ppm = (diff_ppm * 1 + p_rc32k_trim_param->rtc32k_error_ppm * 7) / 8;

        /* update timestamp */
        goto update_tsf;

    } else {
        /* rtc32k error (ppm) */
        p_rc32k_trim_param->rtc32k_error_ppm = (diff_ppm * 2 + p_rc32k_trim_param->rtc32k_error_ppm * 6) / 8;

        if (p_rc32k_trim_param->rtc32k_error_ppm > 500 || p_rc32k_trim_param->rtc32k_error_ppm < -500) {
            uint32_t rc32k_code = HBN_Get_RC32K_R_Code();

            if (p_rc32k_trim_param->rtc32k_error_ppm > 500) {
                rc32k_code += 1;
                p_rc32k_trim_param->rtc32k_error_ppm -= 200;

            } else if (p_rc32k_trim_param->rtc32k_error_ppm < -500) {
                rc32k_code -= 1;
                p_rc32k_trim_param->rtc32k_error_ppm += 200;
            }

            HBN_Set_RC32K_R_Code(rc32k_code);

            p_rc32k_trim_param->rc32k_fr_ext = bl_lp_rc32k_encode_state(rc32k_code);
        }

        /* update timestamp */
        goto update_tsf;
    }

update_tsf:
    /* update timestamp */
    p_rc32k_trim_param->last_rc32trim_stamp_rtc_us = rtc_timestamp_now_us;
    p_rc32k_trim_param->last_rc32trim_stamp_beacon_us = beacon_timestamp_now_us;
    p_rc32k_trim_param->last_rc32trim_stamp_valid = mode;
    return ret;
}


void bl_lp_rc32k_save_code(uint32_t code)
{
    iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext = bl_lp_rc32k_encode_state(code);
}

void bl_lp_rc32k_restore_code(uint32_t state)
{
#if CONFIG_RC32K_CAL_ENABLE_HALF_MSB
    HBN_Set_RC32K_Half_MSB((state & LP_RC32K_HALF_MSB_FLAG) != 0U);
#else
    HBN_Set_RC32K_Half_MSB(0);
#endif

    /* The selector must be restored before HBN receives only the raw 10-bit R code. */
    HBN_Set_RC32K_R_Code(state & LP_RC32K_R_CODE_MASK);
}
int bl_lp_rtc_rc32k_coarse_adj(uint32_t expect_time, uint32_t rc32k_actual_time)
{
    printf("func: %s not implement yet!\n", __func__);
    // abort();
    return 0;
}

int bl_lp_set_32k_clock_ready(uint8_t ready_val)
{
    iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready = ready_val;
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = 0;

    return ready_val;
}

int bl_lp_get_32k_clock_ready(void)
{
    return iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready;
}

int bl_lp_set_32k_trim_ready(uint8_t ready_val)
{
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = ready_val;

    return ready_val;
}

int bl_lp_get_32k_trim_ready(void)
{
    if (iot2lp_para->rc32k_trim_parameter->rc32k_auto_cal_en == 0) {
        return 1;
    } else {
        return iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready;
    }
}
