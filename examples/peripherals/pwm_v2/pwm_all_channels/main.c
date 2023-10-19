#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_device_s *pwm;

int main(void)
{
    board_init();
    board_pwm_gpio_init();

    pwm = bflb_device_get_by_name("pwm_v2_0");

    /* period = .PBCLK / .clk_div / .period = 80MHz( 64MHZ for bl702l) / 80 / 1000 = 1KHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
#if defined(BL702L)
        .clk_div = 64,
#else
        .clk_div = 80,
#endif
        .period = 1000,
    };

    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH0, 100, 500); /* duty = (500-100)/1000 = 40% */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH1, 200, 400); /* duty = (400-200)/1000 = 20% */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 100, 999); /* duty = (999-100)/1000 = 89.9% */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH3, 0, 500); /* duty = (500-0)/1000 = 50% */
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH0);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH0);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH1);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH1);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH2);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH2);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH3);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH3);
    bflb_pwm_v2_start(pwm);

    while (1) {
        printf("pwm all_channel running\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
