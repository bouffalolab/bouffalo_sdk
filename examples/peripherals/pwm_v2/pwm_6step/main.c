#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"
#include "math.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_gpio.h"
#include "../foc.h"

#define PWM_TRI_CH_U (PWM_CH0)
#define PWM_TRI_CH_V (PWM_CH1)
#define PWM_TRI_CH_W (PWM_CH2)
#define PWM_PIN_U_H  (GPIO_PIN_24)
#define PWM_PIN_U_L  (GPIO_PIN_25)
#define PWM_PIN_V_H  (GPIO_PIN_26)
#define PWM_PIN_V_L  (GPIO_PIN_27)
#define PWM_PIN_W_H  (GPIO_PIN_28)
#define PWM_PIN_W_L  (GPIO_PIN_29)
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
    if (bflb_gpio_read(gpio, GPIO_PIN_32)) {
        hall |= (1 << 2);
    }
    if (bflb_gpio_read(gpio, GPIO_PIN_33)) {
        hall |= (1 << 1);
    }
    if (bflb_gpio_read(gpio, GPIO_PIN_34)) {
        hall |= (1 << 0);
    }
    return hall;
}

void pwm_isr(int irq, void *arg)
{
    bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_PERIOD);
    *(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFG17_OFFSET) = 0x00400B6A | (1 << 24);
    foc_hall_init(&hall);
    if (hall.angle == ANGLE0) {
        bflb_pwm_v2_channel_positive_stop(pwm, PWM_TRI_CH_U);
        bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_V);
        bflb_gpio_set(gpio, PWM_PIN_W_L);
    } else if (hall.angle == ANGLE60) {
        bflb_gpio_reset(gpio, PWM_PIN_W_L);
        bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_V);
        bflb_gpio_set(gpio, PWM_PIN_U_L);
    } else if (hall.angle == ANGLE120) {
        bflb_pwm_v2_channel_positive_stop(pwm, PWM_TRI_CH_V);
        bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_W);
        bflb_gpio_set(gpio, PWM_PIN_U_L);
    } else if (hall.angle == ANGLE180) {
        bflb_gpio_reset(gpio, PWM_PIN_U_L);
        bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_W);
        bflb_gpio_set(gpio, PWM_PIN_V_L);
    } else if (hall.angle == ANGLE240) {
        bflb_pwm_v2_channel_positive_stop(pwm, PWM_TRI_CH_W);
        bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_U);
        bflb_gpio_set(gpio, PWM_PIN_V_L);
    } else if (hall.angle == ANGLE300) {
        bflb_gpio_reset(gpio, PWM_PIN_V_L);
        bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_U);
        bflb_gpio_set(gpio, PWM_PIN_W_L);
    }
    *(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFG17_OFFSET) = 0x00400B6A;
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
    bflb_gpio_init(gpio, PWM_PIN_U_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_V_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_W_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_U_L, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, PWM_PIN_V_L, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, PWM_PIN_W_L, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0);
    GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_DIFF_END);
    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_TRI_CH_U, &ch_cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_TRI_CH_V, &ch_cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_TRI_CH_W, &ch_cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_U, 0, 1000);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_V, 0, 1000);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_W, 0, 1000);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_TRI_CH_U);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_TRI_CH_V);
    bflb_pwm_v2_channel_positive_stop(pwm, PWM_TRI_CH_W);
    /* config interrupt */
    bflb_pwm_v2_int_enable(pwm, PWM_INTEN_PERIOD, true);
    bflb_irq_attach(pwm->irq_num, pwm_isr, NULL);
    /* start pwm */
    bflb_gpio_set(gpio, PWM_PIN_U_L);
    bflb_gpio_set(gpio, PWM_PIN_V_L);
    bflb_gpio_set(gpio, PWM_PIN_W_L);
    bflb_pwm_v2_start(pwm);
    /* delay for precharg */
    arch_delay_ms(2000);
    bflb_gpio_reset(gpio, PWM_PIN_U_L);
    bflb_gpio_reset(gpio, PWM_PIN_V_L);
    bflb_gpio_reset(gpio, PWM_PIN_W_L);
    bflb_irq_enable(pwm->irq_num);
}

int main(void)
{
    board_init();
    pwm = bflb_device_get_by_name("pwm_v2_0");
    gpio = bflb_device_get_by_name("gpio");
    foc_hall_install_callback_delay_1us(&hall, delay);
    foc_hall_install_callback_read_hall_pin(&hall, hall_get);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_34, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    peri_pwm_init();
    printf("BLDC 6 step example\n");
    while (1) {
        arch_delay_ms(150);
    }
}
