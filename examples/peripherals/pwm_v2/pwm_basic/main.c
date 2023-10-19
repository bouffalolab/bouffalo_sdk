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

    /* period = .XCLK / .clk_div / .period = 40MHz( 32MHZ for bl702l) / 40( 32 for bl702l) / 1000 = 1KHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_XCLK,
#if defined(BL702L)
        .clk_div = 32,
#else
        .clk_div = 40,
#endif
        .period = 1000,
    };

    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH0, 100, 500); /* duty = (500-100)/1000 = 40% */
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH0);
    bflb_pwm_v2_start(pwm);

    while (1) {
        printf("pwm basic running\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
