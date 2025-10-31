#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616d_glb.h"
#include "bl616d_pds.h"
#include "bl616d_hbn.h"
#include "bl616d_aon.h"
#include "bl616d_pm.h"

int main(void)
{
    uint32_t tmpVal = 0;
    uint32_t num = 0;

    board_init();

    HBN_32K_Sel(HBN_32K_RC);
    // AON_Power_On_Xtal_32K();
    // HBN_32K_Sel(HBN_32K_XTAL);

    HBN_Enable_RTC_Counter();

    /* wdt state */
    if (SET == AON_Wdg_Status_Get()) {
        printf("wdt status is 1, will clear status\r\n");
        AON_Wdg_Status_Clear();

        if (RESET == AON_Wdg_Status_Get()) {
            printf("clear status success\r\n");
        } else {
            printf("[E]clear status fail\r\n");
        }
    }

    tmpVal = BL_RD_REG(AON_BASE, HBN_RSV0);
    num = tmpVal;
    BL_WR_REG(AON_BASE, HBN_RSV0, (num + 1));

    if (num == 0) {
        /* wdt init */
        AON_Wdg_Init(WDG_RESET_TIME_1S76);

        AON_Wdg_Start();

        arch_delay_us(20);
    }

    AON_Wdg_Kick();

    PDS_Mask_All_Wakeup_Src();
    HBN_Pin_WakeUp_Mask(0xF);

    pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
}
