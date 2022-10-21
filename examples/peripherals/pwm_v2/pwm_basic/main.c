#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "board.h"

struct bflb_device_s *pwm0;

int main(void)
{
    board_init();
    board_pwm0_gpio_init();

    pwm0 = bflb_device_get_by_name("pwm0");

    /* period = .XCLK / .clk_div / .period = 40MHz / 40 / 1000 = 1KHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_XCLK,
        .clk_div = 40,
        .period = 1000,
    };

    bflb_pwm_v2_init(pwm0, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 100, 500); /* duty = (500-100)/1000 = 40% */
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CH0);
    bflb_pwm_v2_start(pwm0);

    while (1) {
        printf("pwm0 basic running\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
