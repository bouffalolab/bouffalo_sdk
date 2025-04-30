#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "bl616_aon.h"
#include "bl616_pm.h"
#include "bl616_lp.h"

bl_lp_hbn_fw_cfg_t bl_lp_hbn_fw_cfg = {
    .hbn_sleep_cnt = 0,
    .hbn_level = PM_HBN_LEVEL_0,
};

int main(void)
{
    board_init();

    static bl_lp_io_cfg_t lp_wake_io_cfg = {
        /* input enable, use @ref BL_LP_IO_INPUT_EN */
        .io_0_15_ie = BL_LP_IO_INPUT_DISABLE,
        .io_16_ie = BL_LP_IO_INPUT_DISABLE,
        .io_17_ie = BL_LP_IO_INPUT_DISABLE,
        .io_18_ie = BL_LP_IO_INPUT_ENABLE,
        .io_19_ie = BL_LP_IO_INPUT_ENABLE,
        .io_20_34_ie = BL_LP_IO_INPUT_DISABLE,
        /* trigger mode */
        .io_0_7_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,          /* use @ref BL_LP_PDS_IO_TRIG */
        .io_8_15_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,         /* use @ref BL_LP_PDS_IO_TRIG */
        .io_16_19_aon_trig_mode = BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE, /* aon io, use @ref BL_LP_AON_IO_TRIG, full mode support */
        .io_20_27_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,        /* use @ref BL_LP_PDS_IO_TRIG */
        .io_28_34_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,        /* use @ref BL_LP_PDS_IO_TRIG */
        /* resistors */
        .io_0_15_res = BL_LP_IO_RES_NONE,
        .io_16_res = BL_LP_IO_RES_NONE,
        .io_17_res = BL_LP_IO_RES_NONE,
        .io_18_res = BL_LP_IO_RES_PULL_UP,
        .io_19_res = BL_LP_IO_RES_PULL_UP,
        .io_20_34_res = BL_LP_IO_RES_NONE,
        /* wake up unmask */
        .io_wakeup_unmask = 0,
    };

    /* wake up unmask */
    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 18); /* gpio 18 */
    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 19); /* gpio 19 */

    bl_lp_io_wakeup_cfg(&lp_wake_io_cfg);

    /* disable feed external watchdog during boot2 */
    bl_lp_hbn_init(0, 0, 0, 0);

    printf("enter hbn mode\r\n");
    bflb_mtimer_delay_ms(100);

    bl_lp_hbn_enter(&bl_lp_hbn_fw_cfg);

    while (1) {
    }
}
