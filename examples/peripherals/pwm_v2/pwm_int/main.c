#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"

#define REPT_CNT 3

struct bflb_device_s *pwm;

void pwm_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_pwm_v2_get_intstatus(pwm);

    if (intstatus & PWM_INTSTS_CH0_L) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH0_L);
        static int times = 0;
        printf("CH0_L interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH0_H) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_CH0_H);
        static int times = 0;
        printf("CH0_H interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH1_L) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH1_L);
        static int times = 0;
        printf("CH1_L interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH1_H) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH1_H);
        static int times = 0;
        printf("CH1_H interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH2_L) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH2_L);
        static int times = 0;
        printf("CH2_L interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH2_H) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH2_H);
        static int times = 0;
        printf("CH2_H interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH3_L) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH3_L);
        static int times = 0;
        printf("CH3_L interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_CH3_H) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTCLR_CH3_H);
        static int times = 0;
        printf("CH3_H interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_PERIOD) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_PERIOD);
        static int times = 0;
        printf("period interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_BRAKE) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_BRAKE);
        static int times = 0;
        printf("brake interrupt, %d times\r\n", ++times);
    }
    if (intstatus & PWM_INTSTS_REPT) {
        bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_REPT);
        static int times = 0;
        printf("rept interrupt, %d times\r\n", ++times);
        bflb_pwm_v2_stop(pwm);
    }
}

int main(void)
{
    board_init();
    board_pwm_gpio_init();

    pwm = bflb_device_get_by_name("pwm_v2_0");

    /* period = .PBCLK / .clk_div / .period = 80MHz( 64MHZ for bl702l) / 80( 64 for bl702l) / 1000 = 1kHz */
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
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH0, 100, 700);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH1, 200, 300);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH2, 600, 800);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_CH3, 500, 900);
    bflb_pwm_v2_int_enable(pwm, PWM_INTEN_CH0_L | \
                              PWM_INTEN_CH0_H | \
                              PWM_INTEN_CH1_L | \
                              PWM_INTEN_CH1_H | \
                              PWM_INTEN_CH2_L | \
                              PWM_INTEN_CH2_H | \
                              PWM_INTEN_CH3_L | \
                              PWM_INTEN_CH3_H | \
                              PWM_INTEN_PERIOD | \
                              PWM_INTEN_BRAKE | \
                              PWM_INTEN_REPT, true);
    bflb_irq_attach(pwm->irq_num, pwm_isr, NULL);
    bflb_irq_enable(pwm->irq_num);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_REPT_COUNT, REPT_CNT);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_STOP_ON_REPT, true);
    bflb_pwm_v2_start(pwm);

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
