#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "board.h"

struct bflb_device_s *pwm0;

int main(void)
{
    board_init();
    board_pwm0_gpio_init();

    pwm0 = bflb_device_get_by_name("pwm0");

    /* period = .PBCLK / .clk_div / .period = 80MHz / 80 / 1000 = 1KHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 80,
        .period = 1000,
    };

    bflb_pwm_v2_init(pwm0, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 100, 500); /* duty = (500-100)/1000 = 40% */
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH1, 200, 400); /* duty = (400-200)/1000 = 20% */
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH2, 100, 999); /* duty = (999-100)/1000 = 89.9% */
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH3, 0, 500); /* duty = (500-0)/1000 = 50% */
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CH0);
    bflb_pwm_v2_channel_negative_start(pwm0, PWM_CH0);
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CH1);
    bflb_pwm_v2_channel_negative_start(pwm0, PWM_CH1);
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CH2);
    bflb_pwm_v2_channel_negative_start(pwm0, PWM_CH2);
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CH3);
    bflb_pwm_v2_channel_negative_start(pwm0, PWM_CH3);
    bflb_pwm_v2_start(pwm0);

    while (1) {
        printf("pwm0 all_channel running\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
