#include "bflb_mtimer.h"
#include "bflb_wdg.h"
#include "board.h"

struct bflb_device_s *wdt;
static volatile uint8_t wdt_int_arrived = 0;

void wdt_isr(int irq, void *arg)
{
    wdt_int_arrived = 1;
    bflb_wdg_compint_clear(wdt);
}

int main(void)
{
    board_init();
    printf("Timer watchdog interrupt test\r\n");

    struct bflb_wdg_config_s wdt_cfg;
    wdt_cfg.clock_source = BFLB_SYSTEM_32K_CLK;
    wdt_cfg.clock_div = 0;
    wdt_cfg.comp_val = 64000;

    wdt = bflb_device_get_by_name("watchdog");
    bflb_wdg_init(wdt, &wdt_cfg);
    bflb_irq_attach(wdt->irq_num, wdt_isr, wdt);
    bflb_irq_enable(wdt->irq_num);

    wdt_int_arrived = 0;
    bflb_wdg_start(wdt);

    /* delay 1s and wdt interrupt should not trigger. */
    bflb_mtimer_delay_ms(1000);
    bflb_wdg_reset_countervalue(wdt);
    if (wdt_int_arrived) {
        printf("Error! Delay 1s, WDT interrupt should not arrive\r\n");
        bflb_wdg_stop(wdt);
    }

    /* delay 2s will trigger WDT interrupt */
    bflb_mtimer_delay_ms(2000);
    bflb_wdg_reset_countervalue(wdt);
    if (wdt_int_arrived) {
        printf("Delay 2s, WDT interrupt arrived\r\n");
    } else {
        printf("Error! Delay 2s, WDT interrupt should arrive\r\n");
        printf("get wdt cnt = %d\r\n", bflb_wdg_get_countervalue(wdt));
    }

    bflb_wdg_stop(wdt);

    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
