#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl702_pm.h"
#include "bl702_hbn.h"
#include "bl702_pds.h"
#include "bflb_gpio.h"

#define TEST_PDS_LEVEL_3 1

int main(void)
{
    board_init();

    pm_pds_mask_all_wakeup_src();
    pm_hbn_mask_all_wakeup_src();

    /* gpio only support one pin from gpio0 to gpio7, and pupd does not support in bl702 */
    pm_set_pds_wakeup_io(GPIO_PIN_0, PDS_AON_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL, 0);
    while (1) {
        printf("enter pds mode\r\n");
        bflb_mtimer_delay_ms(100);

#if TEST_PDS_LEVEL_3
        /* sleep time must set zero to avoid using rtc */
        pm_pds_mode_enter(PM_PDS_LEVEL_3, 0);
        printf("exit pds3 mode\r\n");
#else
        /* sleep time must set zero to avoid using rtc */
        pm_pds_mode_enter(PM_PDS_LEVEL_31, 0);
#endif
    }
}
