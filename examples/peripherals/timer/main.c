#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include "board.h"

#define TEST_TIMER_COMP_ID TIMER_COMP_ID_0

struct bflb_device_s *timer0;

void timer0_isr(int irq, void *arg)
{
    bool status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_0);
    if (status) {
        printf("timer0 comp0 trigger\r\n");
        bflb_timer_compint_clear(timer0, TIMER_COMP_ID_0);
    }
    status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_1);
    if (status) {
        printf("timer0 comp1 trigger\r\n");
        bflb_timer_compint_clear(timer0, TIMER_COMP_ID_1);
    }
    status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_2);
    if (status) {
        printf("timer0 comp2 trigger\r\n");
        bflb_timer_compint_clear(timer0, TIMER_COMP_ID_2);
    }
}

int main(void)
{
    board_init();

    /* timer clk = XCLK/(div + 1 )*/
    struct bflb_timer_config_s cfg;
    cfg.counter_mode = TIMER_COUNTER_MODE_PROLOAD;
    cfg.clock_source = BFLB_SYSTEM_XCLK;
    cfg.clock_div = 39; /* for bl616/bl808/bl606p is 39, for bl702 is 31 */
    cfg.trigger_comp_id = TEST_TIMER_COMP_ID;
    cfg.comp0_val = 1000000;
    cfg.comp1_val = 2000000;
    cfg.comp2_val = 3000000;
    cfg.preload_val = 0;

    timer0 = bflb_device_get_by_name("timer0");

    bflb_timer_init(timer0, &cfg);
    bflb_irq_attach(timer0->irq_num, timer0_isr, timer0);
    bflb_irq_enable(timer0->irq_num);
    bflb_timer_start(timer0);
    while (1) {
        printf("helloworld\r\n");
        bflb_mtimer_delay_ms(1500);
    }
}
