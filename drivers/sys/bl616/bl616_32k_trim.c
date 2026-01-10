#include "bl_lp.h"
#include "hbn_reg.h"
#include "bl616_glb.h"
#include <stdint.h>

#define LP_ABS(a)     (((a) > 0) ? (a) : (-(a)))
#define LP_DIFF(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

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

    /* Use local pointer for easier access */
    p_rc32k_trim_param = iot2lp_para->rc32k_trim_parameter;

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

    if (p_rc32k_trim_param->rc32k_auto_cal_en == 0) {
        ret = 7;
        /* update timestamp */
        goto update_tsf;
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

        if (p_rc32k_trim_param->rtc32k_error_ppm > 3000 || p_rc32k_trim_param->rtc32k_error_ppm < -3000) {
            /* get rc32k recal code */
            uint32_t rc32k_reg = *((volatile uint32_t *)0x2000F200);
            uint32_t rc32k_code = rc32k_reg >> 22;

            if (p_rc32k_trim_param->rtc32k_error_ppm > 3000) {
                rc32k_code += 1;
                p_rc32k_trim_param->rtc32k_error_ppm -= 1800;

            } else if (p_rc32k_trim_param->rtc32k_error_ppm < -3000) {
                rc32k_code -= 1;
                p_rc32k_trim_param->rtc32k_error_ppm += 1800;
            }

            /* set rc32k code */
            rc32k_reg &= ~0xffc00000;
            rc32k_reg |= rc32k_code << 22;
            *((volatile uint32_t *)0x2000F200) = rc32k_reg;

            /* save the code */
            p_rc32k_trim_param->rc32k_fr_ext = rc32k_code;
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

int bl_lp_rtc_rc32k_coarse_adj(uint32_t expect_time, uint32_t rc32k_actual_time)
{
    int diff_val, diff_ppm, diff_code;

    if (iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready) {
        return 0;
    }

    if (expect_time == 0 || rc32k_actual_time == 0) {
        return 1000 * 1000;
    }

    diff_val = rc32k_actual_time - expect_time;
    diff_ppm = (int64_t)diff_val * 1000 * 1000 / expect_time;
    if (diff_ppm < 0) {
        diff_code = (diff_ppm / 2000 - 1) * 2 / 3;
    } else {
        diff_code = (diff_ppm / 2000 + 1) * 2 / 3;
    }

    if (diff_ppm > 800 * 1000 || diff_ppm < -800 * 1000) {
        /* The error is too large. Abort calibration */
        return 1000 * 1000;
    }

    /* get rc32k recal code */
    uint32_t rc32k_reg = *((volatile uint32_t *)0x2000F200);
    uint32_t rc32k_code = rc32k_reg >> 22;

    // BL_LP_LOG("rc32k coarse_adj, code:%d, ppm:%d, diff_code:%d\r\n", rc32k_code, diff_ppm, diff_code);

    if (diff_code != 0) {
        rc32k_code += diff_code;

        /* set rc32k code */
        rc32k_reg &= ~0xffc00000;
        rc32k_reg |= rc32k_code << 22;
        *((volatile uint32_t *)0x2000F200) = rc32k_reg;
    }

    /* save the code */
    iot2lp_para->rc32k_trim_parameter->rc32k_fr_ext = rc32k_code;

    return diff_ppm;
}

static void modify_bit(uint32_t *reg_addr, uint8_t bit_position, uint8_t bit_value)
{
    if (bit_value) {
        *reg_addr |= (1U << bit_position);
    } else {
        *reg_addr &= ~(1U << bit_position);
    }
}

static void xtal32k_input(void)
{
    modify_bit((uint32_t *)0x2000F204, 19, 0x1);
    modify_bit((uint32_t *)0x2000F204, 21, 0x1);
    modify_bit((uint32_t *)0x2000F204, 2, 0x1);
    modify_bit((uint32_t *)0x2000F204, 10, 0);
    modify_bit((uint32_t *)0x2000F204, 9, 0);
    modify_bit((uint32_t *)0x2000F204, 17, 0x0);
}

void bl_lp_power_on_xtal32k(uint8_t type)
{
    GLB_GPIO_Cfg_Type gpioCfg = { .gpioPin = GLB_GPIO_PIN_0,
                                  .gpioFun = GPIO_FUN_ANALOG,
                                  .gpioMode = GPIO_MODE_ANALOG,
                                  .pullType = GPIO_PULL_NONE,
                                  .drive = 1,
                                  .smtCtrl = 1 };
    if (type) {
        gpioCfg.gpioPin = 16;
        GLB_GPIO_Init(&gpioCfg);
    }
    gpioCfg.gpioPin = 17;
    GLB_GPIO_Init(&gpioCfg);
    BL_WR_WORD(0x2000F204, 0x210288);

    if (type) {
        /* power on */
        HBN_Set_Xtal_32K_Inverter_Amplify_Strength(3);
        HBN_Power_On_Xtal_32K();
    } else {
        HBN_Power_On_Xtal_32K();
        xtal32k_input();
    }
}

void bl_lp_rtc_use_xtal32K()
{
    GLB_GPIO_Cfg_Type gpioCfg = { .gpioPin = GLB_GPIO_PIN_0,
                                  .gpioFun = GPIO_FUN_ANALOG,
                                  .gpioMode = GPIO_MODE_ANALOG,
                                  .pullType = GPIO_PULL_NONE,
                                  .drive = 1,
                                  .smtCtrl = 1 };

    gpioCfg.gpioPin = 16;
    GLB_GPIO_Init(&gpioCfg);
    gpioCfg.gpioPin = 17;
    GLB_GPIO_Init(&gpioCfg);

    HBN_Power_On_Xtal_32K();
    HBN_32K_Sel(1);
    /* GPIO17 no pull */
    *((volatile uint32_t *)0x2000F014) &= ~(1 << 16);
}

void bl_lp_rtc_use_rc32k()
{
    HBN_32K_Sel(0);
    HBN_Power_Off_Xtal_32K();
}

int bl_lp_set_32k_clock_ready(uint8_t ready_val)
{
    iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready = ready_val;
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = 0;

    // BL_LP_LOG("bl_lp set 32k ready %d\r\n", ready_val);

    return ready_val;
}

int bl_lp_get_32k_clock_ready()
{
    return iot2lp_para->rc32k_trim_parameter->rc32k_clock_ready;
}

int bl_lp_set_32k_trim_ready(uint8_t ready_val)
{
    iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready = ready_val;

    // BL_LP_LOG("bl_lp set 32k trim ready %d\r\n", ready_val);

    return ready_val;
}

int bl_lp_get_32k_trim_ready()
{
    return iot2lp_para->rc32k_trim_parameter->rc32k_trim_ready;
}