#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"
#include "bl616d_pm.h"
#include "bl616d_hbn.h"

#define HBN_RTC_WAKEUP_TIME (5 * 1000 * 1000 / 32)
#define PDS_PRINT_DELAY     200

int main(void)
{
    uint32_t tmpVal = 0;
    uint32_t hbn_mode = 0;
    uint32_t rtc_val_high;
    uint32_t rtc_val_low;

    board_init();

    AON_Power_On_Xtal_32K();
    // HBN_32K_Sel(HBN_32K_XTAL);

    HBN_32K_Sel(HBN_32K_RC);

    HBN_Enable_RTC_Counter();

    arch_delay_ms(500);

    HBN_Get_RTC_Timer_Val(&rtc_val_low, &rtc_val_high);
    printf("rtc_val_high = 0x%08lX, rtc_val_low = 0x%08lX\r\n", rtc_val_high, rtc_val_low);

    arch_delay_ms(500);

    HBN_Get_RTC_Timer_Val(&rtc_val_low, &rtc_val_high);
    printf("rtc_val_high = 0x%08lX, rtc_val_low = 0x%08lX\r\n", rtc_val_high, rtc_val_low);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV0);
    hbn_mode = tmpVal;
    BL_WR_REG(AON_BASE, HBN_RSV0, (tmpVal + 1));
    printf("enter hbn%d mode\r\n", hbn_mode % 2);
    arch_delay_us(PDS_PRINT_DELAY);

    pm_hbn_mode_enter(hbn_mode % 2, HBN_RTC_WAKEUP_TIME);

    while (1) {
    }
}
