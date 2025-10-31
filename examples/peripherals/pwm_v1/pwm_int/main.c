#include "bflb_mtimer.h"
#include "bflb_pwm_v1.h"
#include "bflb_clock.h"
#include "board.h"

#define REPT_CNT 5

struct bflb_device_s *pwm;

void pwm_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_pwm_v1_get_intstatus(pwm);

    if (intstatus & PWM_INTSTS_REPT_CH0) {
        bflb_pwm_v1_int_clear(pwm, PWM_INTCLR_REPT_CH0);
        static int times = 0;
        printf("CH0 interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_REPT_CH1) {
        bflb_pwm_v1_int_clear(pwm, PWM_INTCLR_REPT_CH1);
        static int times = 0;
        printf("CH1 interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_REPT_CH2) {
        bflb_pwm_v1_int_clear(pwm, PWM_INTCLR_REPT_CH2);
        static int times = 0;
        printf("CH2 interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_REPT_CH3) {
        bflb_pwm_v1_int_clear(pwm, PWM_INTCLR_REPT_CH3);
        static int times = 0;
        printf("CH3 interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_REPT_CH4) {
        bflb_pwm_v1_int_clear(pwm, PWM_INTCLR_REPT_CH4);
        static int times = 0;
        printf("CH4 interrupt, %d times\r\n", ++times);
    }
}

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

    for (uint8_t i = 0; i < PWM_V1_CH_MAX; i++) {
        bflb_pwm_v1_channel_init(pwm, i, &cfg);
        bflb_pwm_v1_channel_set_threshold(pwm, i, 100, 500 + i * 100); /* duty = ((500 + i *100)-100)/1000  */

        bflb_pwm_v1_int_enable(pwm, i, true);
        bflb_irq_attach(pwm->irq_num, pwm_isr, NULL);
        bflb_irq_enable(pwm->irq_num);
        bflb_pwm_v1_feature_control(pwm, i, PWM_CMD_SET_REPT_COUNT, REPT_CNT);
        bflb_pwm_v1_start(pwm, i);
    }

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
