#include "bflb_mtimer.h"
#include "bflb_rtc.h"
#include "board.h"

struct bflb_device_s *rtc;
struct bflb_tm g_time;

int main(void)
{
    board_init();

    rtc = bflb_device_get_by_name("rtc");

    bflb_rtc_set_time(rtc, 0);

    /* 2023-11-27, 10:2:1, Monday */
    g_time.tm_sec = 1;
    g_time.tm_min = 2;
    g_time.tm_hour = 10;
    g_time.tm_wday = 1;
    g_time.tm_mday = 27;
    g_time.tm_mon = 10;
    g_time.tm_year = 2023 - 1900;
    bflb_rtc_set_utc_time(&g_time);
    while (1) {
        printf("time:%lld\r\n", BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)));
        bflb_rtc_get_utc_time(&g_time);
        printf("utc time:%u-%u-%u, %u:%u:%u, wday:%u\r\n",
               g_time.tm_year + 1900, g_time.tm_mon + 1, g_time.tm_mday,
               g_time.tm_hour, g_time.tm_min, g_time.tm_sec,
               g_time.tm_wday);
        bflb_mtimer_delay_ms(5000);
    }
}
