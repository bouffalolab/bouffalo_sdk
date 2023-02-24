#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_device_s *pwm;

/* period = .32K_CLK / .clk_div / .period = 32768Hz / 32 / 1000 = 1.024Hz */
struct bflb_pwm_v2_config_s cfg = {
    .clk_source = BFLB_SYSTEM_32K_CLK,
    .clk_div = 32,
    .period = 1000,
};

struct bflb_pwm_v2_channel_config_s ch_cfg[PWM_V2_CH_MAX] = { {
                                                               .positive_polarity = PWM_POLARITY_ACTIVE_HIGH,
                                                               .negative_polarity = PWM_POLARITY_ACTIVE_HIGH,
                                                               .positive_stop_state = PWM_STATE_INACTIVE,
                                                               .negative_stop_state = PWM_STATE_ACTIVE,
                                                               .positive_brake_state = PWM_STATE_INACTIVE,
                                                               .negative_brake_state = PWM_STATE_ACTIVE,
                                                               .dead_time = 0,
                                                           },
                                                           {
                                                               .positive_polarity = PWM_POLARITY_ACTIVE_LOW,
                                                               .negative_polarity = PWM_POLARITY_ACTIVE_LOW,
                                                               .positive_stop_state = PWM_STATE_ACTIVE,
                                                               .negative_stop_state = PWM_STATE_INACTIVE,
                                                               .positive_brake_state = PWM_STATE_ACTIVE,
                                                               .negative_brake_state = PWM_STATE_ACTIVE,
                                                               .dead_time = 0,
                                                           },
                                                           {
                                                               .positive_polarity = PWM_POLARITY_ACTIVE_HIGH,
                                                               .negative_polarity = PWM_POLARITY_ACTIVE_LOW,
                                                               .positive_stop_state = PWM_STATE_INACTIVE,
                                                               .negative_stop_state = PWM_STATE_INACTIVE,
                                                               .positive_brake_state = PWM_STATE_INACTIVE,
                                                               .negative_brake_state = PWM_STATE_INACTIVE,
                                                               .dead_time = 0,
                                                           },
                                                           {
                                                               .positive_polarity = PWM_POLARITY_ACTIVE_LOW,
                                                               .negative_polarity = PWM_POLARITY_ACTIVE_HIGH,
                                                               .positive_stop_state = PWM_STATE_ACTIVE,
                                                               .negative_stop_state = PWM_STATE_ACTIVE,
                                                               .positive_brake_state = PWM_STATE_ACTIVE,
                                                               .negative_brake_state = PWM_STATE_INACTIVE,
                                                               .dead_time = 0,
                                                           } };

int main(void)
{
    board_init();
    board_pwm_gpio_init();

    pwm = bflb_device_get_by_name("pwm_v2_0");

    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH0, 0, 500);   /* duty = (500-0)/1000 = 50% */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH1, 200, 400); /* duty = (400-200)/1000 = 20% */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 99, 999);  /* duty = (999-99)/1000 = 90% */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH3, 300, 900); /* duty = (900-300)/1000 = 60% */
    bflb_pwm_v2_channel_init(pwm, PWM_CH0, &ch_cfg[0]);
    bflb_pwm_v2_channel_init(pwm, PWM_CH1, &ch_cfg[1]);
    bflb_pwm_v2_channel_init(pwm, PWM_CH2, &ch_cfg[2]);
    bflb_pwm_v2_channel_init(pwm, PWM_CH3, &ch_cfg[3]);
    printf("pwm config_channel running\r\n");
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH0);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH0);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH1);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH1);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH2);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH2);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_CH3);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_CH3);
    bflb_pwm_v2_start(pwm);
    bflb_mtimer_delay_ms(10 * 1000);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH0);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH0);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH1);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH1);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH2);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH2);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_CH3);
    bflb_pwm_v2_channel_negative_stop(pwm, PWM_CH3);

    while (1) {
        printf("pwm config_channel stop, please check wave\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
