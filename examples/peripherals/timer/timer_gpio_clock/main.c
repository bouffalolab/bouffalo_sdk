#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include "bflb_gpio.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

#define PWM_CHANNEL_SELECT PWM_CH0
#define PWM_STOP_COUNT     (10000)

struct bflb_device_s *pwm0;
struct bflb_device_s *timer0;
struct bflb_device_s *gpio;
volatile int pwm_is_stopped;

static void pwm_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_pwm_v2_get_intstatus(pwm0);

    if (intstatus & PWM_INTSTS_REPT) {
        bflb_pwm_v2_stop(pwm0);
        bflb_pwm_v2_int_clear(pwm0, PWM_INTSTS_REPT);
        pwm_is_stopped = 1;
    }
}

static void pwm_init()
{
    pwm0 = bflb_device_get_by_name("pwm_v2_0");
    /* period = .XCLK / .clk_div / .period = 40MHz / 2 / 10 = 2MHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_XCLK,
        .clk_div = 2,
        .period = 10,
    };
    /* init pwm */
    bflb_pwm_v2_init(pwm0, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CHANNEL_SELECT, 1, 5); /* duty = (5-1)/10 = 40% */
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CHANNEL_SELECT);
    bflb_pwm_v2_int_enable(pwm0, PWM_INTEN_REPT, true);
    bflb_irq_attach(pwm0->irq_num, pwm_isr, NULL);
    bflb_irq_enable(pwm0->irq_num);
    bflb_pwm_v2_feature_control(pwm0, PWM_CMD_SET_REPT_COUNT, PWM_STOP_COUNT);
    bflb_pwm_v2_feature_control(pwm0, PWM_CMD_SET_STOP_ON_REPT, true);
}

int main(void)
{
    uint32_t i;
    uint32_t cnt = 0;
    
    board_init();
    board_pwm_gpio_init();
    board_timer_gpio_init();

    printf("Timer GPIO as clock source case\n");
    pwm_init();

    struct bflb_timer_config_s cfg;
    cfg.counter_mode = TIMER_COUNTER_MODE_UP;
    cfg.clock_source = TIMER_CLKSRC_GPIO;
    cfg.clock_div = 0;
    cfg.trigger_comp_id = TIMER_COMP_ID_2;
    cfg.comp0_val = 0xFFFFFFFF;
    cfg.comp1_val = 0xFFFFFFFF;
    cfg.comp2_val = 0xFFFFFFFF;
    cfg.preload_val = 0;

    timer0 = bflb_device_get_by_name("timer0");

    /* Timer init with default configuration */
    bflb_timer_init(timer0, &cfg);

    /* Enable timer and get count */
    for(i = 0; i < 10; i++) {
        pwm_is_stopped = 0;
        bflb_timer_start(timer0);
        bflb_pwm_v2_start(pwm0);
        while (pwm_is_stopped == 0);
        cnt = bflb_timer_get_countervalue(timer0);
        // bflb_timer_stop(timer0); /* because timer clock has been stopped now, so bflb_timer_stop is useless */
        if (cnt == (i + 1) * PWM_STOP_COUNT - 2) {
            printf("Test %lu times success, timer count value: %lu\r\n", i, cnt);
        } else if (cnt == 0) {
            printf("Please connect clock source pin to pwm pin!\r\n");
        } else {
            printf("Test %lu times failed, timer count value: %lu, expect value: %lu\r\n", i, cnt, (i + 1) * PWM_STOP_COUNT - 2);
        }
    }

    while (1) {
        bflb_mtimer_delay_ms(1500);
    }
}
