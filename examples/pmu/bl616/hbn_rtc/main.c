#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616_common.h"
#include "bl616_glb.h"
#include "bl616_pds.h"
#include "bl616_hbn.h"
#include "bl616_aon.h"
#include "bl616_pm.h"
#include "bflb_uart.h"
#if (PM_PDS_LDO_LEVEL_DEFAULT == 8)
#include "bl_lp.h"

bl_lp_hbn_fw_cfg_t hbn_test_cfg={
    .hbn_sleep_cnt=32768*10,
    .hbn_level=PM_HBN_LEVEL_0,
};
#endif

int main(void)
{
    board_init();

    printf("enter hbn mode\r\n");
    bflb_mtimer_delay_ms(100);

#if (PM_PDS_LDO_LEVEL_DEFAULT == 8)
    printf("hbn_pattern:0x%08x\r\n",*((volatile uint32_t *)0x20010300));

    hal_pm_ldo11_use_ext_dcdc();

    bl_lp_hbn_init(0,0,1,0);
    bl_lp_hbn_enter(&hbn_test_cfg);
#else
    /* Wake up every 10 seconds by hb0 */
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, 32768*10);
#endif
    while (1) {
    }
}
