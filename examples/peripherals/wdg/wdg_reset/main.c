#include "bflb_mtimer.h"
#include "bflb_wdg.h"
#include "board.h"
#include <assert.h>

struct bflb_device_s *wdg;

int main(void)
{
    board_init();
    printf("Watchdog reset test\r\n");

    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = 31;
    wdg_cfg.comp_val = 5000;
    wdg_cfg.mode = WDG_MODE_RESET;

    wdg = bflb_device_get_by_name("watchdog0");
    assert(wdg != NULL);
    bflb_wdg_init(wdg, &wdg_cfg);

    bflb_wdg_start(wdg);

    bflb_mtimer_delay_ms(2000);
    bflb_wdg_reset_countervalue(wdg);
    printf("Test 1: Delay 2s under 5s threshold: no reset\r\n");

    bflb_mtimer_delay_ms(4000);
    printf("Test 2: Reset counter and delay 4s, counter value: %d\r\n",
           bflb_wdg_get_countervalue(wdg));

    printf("Test 3: Waiting for watchdog reset after 1s...\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
