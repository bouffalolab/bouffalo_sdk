#include "bflb_mtimer.h"
#include "bflb_wdg.h"
#include "board.h"

struct bflb_device_s *wdg;
static volatile uint8_t wdg_int_arrived = 0;

int main(void)
{
    board_init();
    printf("Watchdog interrupt test\r\n");

    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = 31;
    wdg_cfg.comp_val = 2000;
    wdg_cfg.mode = WDG_MODE_RESET;

    wdg = bflb_device_get_by_name("watchdog");
    bflb_wdg_init(wdg, &wdg_cfg);

    wdg_int_arrived = 0;
    bflb_wdg_start(wdg);

    /* delay 1s and wdg interrupt should not trigger. */
    bflb_mtimer_delay_ms(1000);
    bflb_wdg_reset_countervalue(wdg);
    if (wdg_int_arrived) {
        printf("Error! Delay 1s, wdg not reset.\r\n");
        bflb_wdg_stop(wdg);
    } else {
        printf("Delay 1s, wdg interrupt not arrive, pass\r\n");
    }

    bflb_wdg_set_countervalue(wdg, 4000);
    bflb_mtimer_delay_ms(2000);

    if (wdg_int_arrived) {
        printf("Error! Delay 2s, wdg not reset.\r\n");
        bflb_wdg_stop(wdg);
    } else {
        printf("Delay 2s, set 4s, wdg interrupt not arrive, pass\r\n");
    }

    printf("Next delay 4s, wdg will reset it.");
    /* delay 2s will trigger wdg interrupt */
    bflb_mtimer_delay_ms(2000);
    bflb_wdg_reset_countervalue(wdg);
    if (wdg_int_arrived) {
        printf("Delay 2s, wdg reset, pass\r\n");
    } else {
        printf("Error! Delay 2s, wdg not reset, count = %d\r\n",
               bflb_wdg_get_countervalue(wdg));
    }
    bflb_wdg_stop(wdg);

    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
