#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include "board.h"
#define TEST_TIMER_COMP_ID TIMER_COMP_ID_2

struct bflb_device_s *timer0;
struct bflb_device_s *timer1;

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

void timer1_isr(int irq, void *arg)
{
    bool status = bflb_timer_get_compint_status(timer1, TIMER_COMP_ID_0);
    if (status) {
        printf("timer1 comp0 trigger\r\n");
        bflb_timer_compint_clear(timer1, TIMER_COMP_ID_0);
    }
    status = bflb_timer_get_compint_status(timer1, TIMER_COMP_ID_1);
    if (status) {
        printf("timer1 comp1 trigger\r\n");
        bflb_timer_compint_clear(timer1, TIMER_COMP_ID_1);
    }
    status = bflb_timer_get_compint_status(timer1, TIMER_COMP_ID_2);
    if (status) {
        printf("timer1 comp2 trigger\r\n");
        bflb_timer_compint_clear(timer1, TIMER_COMP_ID_2);
    }
}

int main(void)
{
    board_init();
    printf("Timer basic test\n");

    /* timer clk = XCLK/(div + 1 )*/
    struct bflb_timer_config_s cfg0;
    cfg0.counter_mode = TIMER_COUNTER_MODE_PROLOAD; /* preload when match occur */
    cfg0.clock_source = TIMER_CLKSRC_XTAL;
    cfg0.clock_div = 0; /* for bl616/bl808/bl606p is 39, for bl702 is 31 */
    cfg0.trigger_comp_id = TEST_TIMER_COMP_ID;
    cfg0.comp0_val = 230000; /* match value 0  */
    cfg0.comp1_val = 260000; /* match value 1 */
    cfg0.comp2_val = 290000; /* match value 2 */
    cfg0.preload_val = 0;    /* preload value */

    struct bflb_timer_config_s cfg1;
    cfg1.counter_mode = TIMER_COUNTER_MODE_PROLOAD;
    cfg1.clock_source = TIMER_CLKSRC_XTAL;
    cfg1.clock_div = 0; /* for bl616/bl808/bl606p is 39, for bl702 is 31 */
    cfg1.trigger_comp_id = TEST_TIMER_COMP_ID;
    cfg1.comp0_val = 330000; /* match value 0  */
    cfg1.comp1_val = 360000; /* match value 1 */
    cfg1.comp2_val = 390000; /* match value 2 */
    cfg1.preload_val = 0;    /* preload value */

    timer0 = bflb_device_get_by_name("timer0");
    timer1 = bflb_device_get_by_name("timer1");

    /* Timer init with default configuration */
    bflb_timer_init(timer0, &cfg0);
    bflb_timer_init(timer1, &cfg1);

    bflb_irq_attach(timer0->irq_num, timer0_isr, timer0);
    bflb_irq_attach(timer1->irq_num, timer1_isr, timer1);
    bflb_irq_enable(timer0->irq_num);
    bflb_irq_enable(timer1->irq_num);

    /* Enable timer */
    bflb_timer_start(timer0);
    bflb_timer_start(timer1);

    printf("case success.\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
