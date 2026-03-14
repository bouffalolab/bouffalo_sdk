#include "bl_lp.h"
#include "hbn_reg.h"
#include "bl616cl_glb.h"
#include <stdint.h>

#define LP_ABS(a)     (((a) > 0) ? (a) : (-(a)))
#define LP_DIFF(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

/* Calibrate RC32K and update the beacon timestamp */
int lpfw_recal_rc32k(lp_fw_rc32k_trim_t *p_rc32k_trim_param, uint64_t beacon_timestamp_now_us,
                     uint64_t rtc_timestamp_now_us, uint32_t mode, int clock_ready_check)
{
    printf("func: %s not implement yet!\n", __func__);
    abort();
    return 0;
}

int bl_lp_rtc_rc32k_coarse_adj(uint32_t expect_time, uint32_t rc32k_actual_time)
{
    printf("func: %s not implement yet!\n", __func__);
    abort();
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