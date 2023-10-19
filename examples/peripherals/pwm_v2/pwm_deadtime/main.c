#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

struct bflb_device_s *pwm;

uint8_t deadtime[PWM_V2_CH_MAX] = {
    0x00 + 0,  /* 0,   bit[7:5]=0xx: dt = [7:0]*1,       range(0~127),    step(1) */
    0x80 + 36, /* 200, bit[7:5]=10x: dt = (64+[5:0])*2,  range(128~254),  step(2) */
    0xC0 + 18, /* 400, bit[7:5]=110: dt = (32+[4:0])*8,  range(256~504),  step(8) */
    0xE0 + 18  /* 800, bit[7:5]=111: dt = (32+[4:0])*16, range(512~1008), step(16) */
};

int main(void)
{
    board_init();
    board_pwm_gpio_init();

    pwm = bflb_device_get_by_name("pwm_v2_0");

    /* period = .PBCLK / .clk_div / .period = 80MHz / 40( 32 for bl702l) / 2000 = 1KHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
#if defined(BL702L)
        .clk_div = 32,
#else
        .clk_div = 40,
#endif
        .period = 2000,
    };

    struct bflb_pwm_v2_channel_config_s ch_cfg = {
        .positive_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .negative_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .positive_stop_state = PWM_STATE_INACTIVE,
        .negative_stop_state = PWM_STATE_ACTIVE,
        .positive_brake_state = PWM_STATE_INACTIVE,
        .negative_brake_state = PWM_STATE_INACTIVE,
        .dead_time = 0,
    };

    bflb_pwm_v2_init(pwm, &cfg);
    /* positive raise@(0+0) fall@1000, negative fall@0 raise@(1000+0) */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH0, 0, 1000);
    /* positive raise@(0+200) fall@1000, negative fall@0 raise@(1000+200) */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH1, 0, 1000);
    /* positive raise@(0+400) fall@1000, negative fall@0 raise@(1000+400) */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 0, 1000);
    /* positive raise@(0+800) fall@1000, negative fall@0 raise@(1000+800) */
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH3, 0, 1000);
    for (uint8_t ch = PWM_CH0; ch < PWM_V2_CH_MAX; ch++) {
        ch_cfg.dead_time = deadtime[ch];
        bflb_pwm_v2_channel_init(pwm, ch, &ch_cfg);
    }
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
        printf("pwm deadtime running\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
