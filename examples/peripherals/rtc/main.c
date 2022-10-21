#include "bflb_mtimer.h"
#include "bflb_rtc.h"

struct bflb_device_s *rtc;

extern void board_init(void);

int main(void)
{
    board_init();

    rtc = bflb_device_get_by_name("rtc");

    bflb_rtc_set_time(rtc, BFLB_RTC_SEC2TIME(1));
    while (1) {
        printf("time:%lld\r\n", BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)));
        bflb_mtimer_delay_ms(2000);
    }
}
