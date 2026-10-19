#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl702_pm.h"
#include "bl702_hbn.h"

#define TEST_PDS_LEVEL_3 1

int main(void)
{
    board_init();

    pm_pds_mask_all_wakeup_src();
    pm_hbn_mask_all_wakeup_src();

    while (1) {
        printf("enter pds mode\r\n");
        bflb_mtimer_delay_ms(100);

#if TEST_PDS_LEVEL_3
        pm_pds_mode_enter(PM_PDS_LEVEL_3, 32768 * 1);
        printf("exit pds3 mode\r\n");
#else
        pm_pds_mode_enter(PM_PDS_LEVEL_31, 32768 * 1);
#endif
    }
}
