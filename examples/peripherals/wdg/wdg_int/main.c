#include "bflb_mtimer.h"
#include "bflb_wdg.h"
#include "board.h"

struct bflb_device_s *wdg;
static volatile uint8_t wdg_int_arrived = 0;

void wdg_isr(int irq, void *arg)
{
    bflb_wdg_compint_clear(wdg);
    wdg_int_arrived = 1;
}

int main(void)
{
    board_init();
    printf("Watchdog interrupt test\r\n");

    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = 0;
    wdg_cfg.comp_val = 64000;
    wdg_cfg.mode = WDG_MODE_INTERRUPT;

    wdg = bflb_device_get_by_name("watchdog");
    bflb_wdg_init(wdg, &wdg_cfg);
    bflb_irq_attach(wdg->irq_num, wdg_isr, wdg);
    bflb_irq_enable(wdg->irq_num);

    wdg_int_arrived = 0;
    bflb_wdg_start(wdg);

    /* delay 1s and wdg interrupt should not trigger. */
    bflb_mtimer_delay_ms(1000);
    bflb_wdg_reset_countervalue(wdg);
    if (wdg_int_arrived) {
        printf("Error! Delay 1s, wdg interrupt should not arrive\r\n");
        bflb_wdg_stop(wdg);
    } else {
        printf("Delay 1s, wdg interrupt not arrive, pass\r\n");
    }

    /* delay 2s will trigger wdg interrupt */
    bflb_mtimer_delay_ms(2000);
    bflb_wdg_reset_countervalue(wdg);
    if (wdg_int_arrived) {
        printf("Delay 2s, wdg interrupt arrived, pass\r\n");
    } else {
        printf("Error! Delay 2s, wdg interrupt not arrived, count = %d\r\n",
               bflb_wdg_get_countervalue(wdg));
    }
    bflb_wdg_stop(wdg);

    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
