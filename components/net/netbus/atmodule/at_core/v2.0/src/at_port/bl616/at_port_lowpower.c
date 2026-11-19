#include "at_main.h"
#include "at_port.h"
#include "bl616_pm.h"
#ifdef CONFIG_LPAPP
#include "bl616_glb_gpio.h"
#include "bl_lp.h"
#endif

void at_port_deep_sleep(uint32_t sleep_time_ms)
{
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, (uint64_t)sleep_time_ms * 32768 / 1000);
}

#ifdef CONFIG_LPAPP
int at_port_sleep_wakeup_config(int gpio, int level)
{
    /* The LP driver retains this configuration pointer. */
    static bl_lp_io_cfg_t wake_io_cfg;

    if (GLB_GPIO_Pad_LeadOut_Sts(gpio) != SET ||
        (level == 0 && (gpio < 16 || gpio > 19))) {
        return -1;
    }

    wake_io_cfg = (bl_lp_io_cfg_t) {
        .io_0_15_ie = BL_LP_IO_INPUT_ENABLE,
        .io_16_ie = BL_LP_IO_INPUT_ENABLE,
        .io_17_ie = BL_LP_IO_INPUT_ENABLE,
        .io_18_ie = BL_LP_IO_INPUT_ENABLE,
        .io_19_ie = BL_LP_IO_INPUT_ENABLE,
        .io_20_34_ie = BL_LP_IO_INPUT_ENABLE,
        .io_wakeup_unmask = (uint64_t)1 << gpio,
        .io_0_7_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL,
        .io_8_15_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL,
        .io_16_19_aon_trig_mode = level ? BL_LP_AON_IO_TRIG_SYNC_HIGH_LEVEL : BL_LP_AON_IO_TRIG_SYNC_LOW_LEVEL,
        .io_20_27_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL,
        .io_28_34_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL,
        .io_0_15_res = BL_LP_IO_RES_NONE,
        .io_16_res = BL_LP_IO_RES_NONE,
        .io_17_res = BL_LP_IO_RES_NONE,
        .io_18_res = BL_LP_IO_RES_NONE,
        .io_19_res = BL_LP_IO_RES_NONE,
        .io_20_34_res = BL_LP_IO_RES_NONE,
    };

    return bl_lp_io_wakeup_cfg(&wake_io_cfg);
}
#endif
