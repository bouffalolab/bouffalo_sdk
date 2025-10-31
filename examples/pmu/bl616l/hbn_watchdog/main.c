#include "bflb_mtimer.h"
#include "board.h"
#include "bl616l_glb.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_aon.h"
#include "bl616l_pm.h"

#define SLEEP_TIME   (32768 * 10)
#define WATCHDOG_TME (32768 * 1)

uint32_t hbn_level = PM_HBN_LEVEL_1;
uint32_t wakeup_count;

int main(void)
{
    board_init();
    HBN_32K_Sel(HBN_32K_RC);

    if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
        GLB_RESET_RECORD_Type rst_reason;
        HBN_Clear_IRQ(HBN_INT_RTC);
        GLB_Get_Reset_Reason(&rst_reason);
        GLB_Clr_Reset_Reason();
        printf("hbn rtc wakeup, reset reason is 0b'%b\r\n",rst_reason);
    } else {
        GLB_Clr_Reset_Reason();
    }

    printf("enter hbn_%u mode\r\n", hbn_level);
    bflb_mtimer_delay_us(200);

    HBN_Wdg_Init(HBN_WDG_MODE_RESET, WATCHDOG_TME);
    HBN_Wdg_Start();

    pm_hbn_mode_enter(hbn_level, SLEEP_TIME);

    while (1) {
    }
}
