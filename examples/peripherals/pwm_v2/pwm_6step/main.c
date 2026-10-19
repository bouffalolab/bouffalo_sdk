#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "bflb_gpio.h"
#include "board.h"
#include "math.h"
#include "../foc.h"

struct bflb_device_s *pwm;
struct bflb_device_s *gpio;

struct foc_hall_s hall;

void delay(void)
{
    uint32_t i = 100;
    while (i--) {};
}

uint8_t hall_get(void)
{
    uint8_t hall = 0;
    if (bflb_gpio_read(gpio, APP_HALL_PIN_U)) {
        hall |= (1 << 2);
    }
    if (bflb_gpio_read(gpio, APP_HALL_PIN_V)) {
        hall |= (1 << 1);
    }
    if (bflb_gpio_read(gpio, APP_HALL_PIN_W)) {
        hall |= (1 << 0);
    }
    return hall;
}

void pwm_isr(int irq, void *arg)
{
    bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_PERIOD);
    bflb_gpio_set(gpio, APP_TEST_PIN);
    foc_hall_init(&hall);
    if (hall.angle == ANGLE0) {
        bflb_pwm_v2_channel_positive_stop(pwm, APP_PWM_TRI_CH_U);
        bflb_pwm_v2_channel_positive_start(pwm, APP_PWM_TRI_CH_V);
        bflb_gpio_set(gpio, APP_PWM_PIN_W_L);
    } else if (hall.angle == ANGLE60) {
        bflb_gpio_reset(gpio, APP_PWM_PIN_W_L);
        bflb_pwm_v2_channel_positive_start(pwm, APP_PWM_TRI_CH_V);
        bflb_gpio_set(gpio, APP_PWM_PIN_U_L);
    } else if (hall.angle == ANGLE120) {
        bflb_pwm_v2_channel_positive_stop(pwm, APP_PWM_TRI_CH_V);
        bflb_pwm_v2_channel_positive_start(pwm, APP_PWM_TRI_CH_W);
        bflb_gpio_set(gpio, APP_PWM_PIN_U_L);
    } else if (hall.angle == ANGLE180) {
        bflb_gpio_reset(gpio, APP_PWM_PIN_U_L);
        bflb_pwm_v2_channel_positive_start(pwm, APP_PWM_TRI_CH_W);
        bflb_gpio_set(gpio, APP_PWM_PIN_V_L);
    } else if (hall.angle == ANGLE240) {
        bflb_pwm_v2_channel_positive_stop(pwm, APP_PWM_TRI_CH_W);
        bflb_pwm_v2_channel_positive_start(pwm, APP_PWM_TRI_CH_U);
        bflb_gpio_set(gpio, APP_PWM_PIN_V_L);
    } else if (hall.angle == ANGLE300) {
        bflb_gpio_reset(gpio, APP_PWM_PIN_V_L);
        bflb_pwm_v2_channel_positive_start(pwm, APP_PWM_TRI_CH_U);
        bflb_gpio_set(gpio, APP_PWM_PIN_W_L);
    }
    bflb_gpio_reset(gpio, APP_TEST_PIN);
}

void peri_pwm_init(void)
{
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 1,
        .period = 80 * 1000 / 10,
    };
    struct bflb_pwm_v2_channel_config_s ch_cfg = {
        .positive_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .negative_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .positive_stop_state = PWM_STATE_INACTIVE,
        .negative_stop_state = PWM_STATE_INACTIVE,
        .positive_brake_state = PWM_STATE_INACTIVE,
        .negative_brake_state = PWM_STATE_ACTIVE,
        .dead_time = 0,
    };

    /* config pin */
    bflb_gpio_init(gpio, APP_PWM_PIN_U_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, APP_PWM_PIN_V_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, APP_PWM_PIN_W_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, APP_PWM_PIN_U_L, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, APP_PWM_PIN_V_L, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, APP_PWM_PIN_W_L, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_IO_SEL, PWM_IO_SEL_DIFF_END);
    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_init(pwm, APP_PWM_TRI_CH_U, &ch_cfg);
    bflb_pwm_v2_channel_init(pwm, APP_PWM_TRI_CH_V, &ch_cfg);
    bflb_pwm_v2_channel_init(pwm, APP_PWM_TRI_CH_W, &ch_cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, APP_PWM_TRI_CH_U, 0, 1000);
    bflb_pwm_v2_channel_set_threshold(pwm, APP_PWM_TRI_CH_V, 0, 1000);
    bflb_pwm_v2_channel_set_threshold(pwm, APP_PWM_TRI_CH_W, 0, 1000);
    bflb_pwm_v2_channel_positive_stop(pwm, APP_PWM_TRI_CH_U);
    bflb_pwm_v2_channel_positive_stop(pwm, APP_PWM_TRI_CH_V);
    bflb_pwm_v2_channel_positive_stop(pwm, APP_PWM_TRI_CH_W);
    /* config interrupt */
    bflb_pwm_v2_int_enable(pwm, PWM_INTEN_PERIOD, true);
    bflb_irq_attach(pwm->irq_num, pwm_isr, NULL);
    /* start pwm */
    bflb_gpio_set(gpio, APP_PWM_PIN_U_L);
    bflb_gpio_set(gpio, APP_PWM_PIN_V_L);
    bflb_gpio_set(gpio, APP_PWM_PIN_W_L);
    bflb_pwm_v2_start(pwm);
    /* delay for precharg */
    arch_delay_ms(2000);
    bflb_gpio_reset(gpio, APP_PWM_PIN_U_L);
    bflb_gpio_reset(gpio, APP_PWM_PIN_V_L);
    bflb_gpio_reset(gpio, APP_PWM_PIN_W_L);
    bflb_irq_enable(pwm->irq_num);
}

int main(void)
{
    board_init();
    pwm = bflb_device_get_by_name("pwm_v2_0");
    gpio = bflb_device_get_by_name("gpio");
    foc_hall_install_callback_delay_1us(&hall, delay);
    foc_hall_install_callback_read_hall_pin(&hall, hall_get);
    bflb_gpio_init(gpio, APP_HALL_PIN_U, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, APP_HALL_PIN_V, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, APP_HALL_PIN_W, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    peri_pwm_init();
    printf("BLDC 6 step example\n");
    while (1) {
        arch_delay_ms(150);
    }
}
