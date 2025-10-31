#include "bflb_mtimer.h"
#include "bflb_pwm_v1.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_device_s *pwm;

int main(void)
{
    board_init();
    board_pwm_gpio_init();

    pwm = bflb_device_get_by_name("pwm_v1");

    /* period = .XCLK / .clk_div / .period = 32MHz / 32 / 1000 = 1KHz */
    struct bflb_pwm_v1_channel_config_s cfg = {
        .clk_source = BFLB_SYSTEM_XCLK,
        .clk_div = 32,
        .period = 1000,
    };

    bflb_pwm_v1_channel_init(pwm, PWM_CH0, &cfg);
    bflb_pwm_v1_channel_set_threshold(pwm, PWM_CH0, 100, 500); /* duty = (500 -100)/1000 = 40% */
    bflb_pwm_v1_start(pwm, PWM_CH0);

    while (1) {
        printf("pwm basic running\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
