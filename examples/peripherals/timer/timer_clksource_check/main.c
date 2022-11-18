#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include "board.h"

#if !defined(BL702L)
uint8_t timer_clk_src_type[] = {
    TIMER_CLKSRC_BCLK,
    TIMER_CLKSRC_32K,
    TIMER_CLKSRC_1K,
    TIMER_CLKSRC_XTAL,
};
#else
uint8_t timer_clk_src_type[] = {
    TIMER_CLKSRC_32K,
    TIMER_CLKSRC_1K,
    TIMER_CLKSRC_XTAL,
};
#endif

int main(void)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t cnt = 0;

    board_init();
    printf("Timer clksource check\n");

    struct bflb_device_s *timer0;
    timer0 = bflb_device_get_by_name("timer0");

    /* timer clk = clock_source/(div + 1)*/
    struct bflb_timer_config_s cfg;
    cfg.counter_mode = TIMER_COUNTER_MODE_PROLOAD;
    cfg.clock_source = TIMER_CLKSRC_NO;
    cfg.clock_div = 9;
    cfg.trigger_comp_id = TIMER_COMP_ID_0;
    cfg.comp0_val = 0xFFFFFFFF;
    cfg.comp1_val = 0xFFFFFFFF;
    cfg.comp2_val = 0xFFFFFFFF;
    cfg.preload_val = 0;

    for (i = 0; i < sizeof(timer_clk_src_type) / sizeof(timer_clk_src_type[0]); i++) {
        cnt = 0;

        if (timer_clk_src_type[i] == TIMER_CLKSRC_XTAL) {
            printf("Timer Src Clk is XTAL\r\n");
        } else if (timer_clk_src_type[i] == TIMER_CLKSRC_32K) {
            printf("Timer Src Clk is 32K\r\n");
        } else if (timer_clk_src_type[i] == TIMER_CLKSRC_1K) {
            printf("Timer Src Clk is 1K\r\n");
        }
#if !defined(BL702L)
        else if (timer_clk_src_type[i] == TIMER_CLKSRC_BCLK) {
            printf("Timer Src Clk is BCLK\r\n");
        }
#endif
        else {
            printf("Other clock, not test.\r\n");
            continue;
        }

        cfg.clock_source = timer_clk_src_type[i];
        bflb_timer_init(timer0, &cfg);

        for (j = 0; j < 10; j++) {
            bflb_timer_start(timer0);
            bflb_mtimer_delay_ms(1000);
            cnt = bflb_timer_get_countervalue(timer0);
            bflb_timer_stop(timer0);
            bflb_mtimer_delay_ms(10);
            printf("delay 1000ms, Test %lu times, cnt: %lu\r\n", j, cnt);
        }
        bflb_timer_deinit(timer0);
    }
    printf("case success.\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
