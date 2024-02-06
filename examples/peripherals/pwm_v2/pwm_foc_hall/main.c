#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"
#include "math.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_gpio.h"
#include "../foc.h"

#define PWM_TRI_CH_U (PWM_CH2)
#define PWM_TRI_CH_V (PWM_CH3)
#define PWM_TRI_CH_W (PWM_CH0)
#define PWM_PIN_U_H  (GPIO_PIN_28)
#define PWM_PIN_U_L  (GPIO_PIN_29)
#define PWM_PIN_V_H  (GPIO_PIN_30)
#define PWM_PIN_V_L  (GPIO_PIN_31)
#define PWM_PIN_W_H  (GPIO_PIN_32)
#define PWM_PIN_W_L  (GPIO_PIN_33)
#define HALL_PIN_U   (GPIO_PIN_11)
#define HALL_PIN_V   (GPIO_PIN_15)
#define HALL_PIN_W   (GPIO_PIN_18)
struct bflb_device_s *pwm;
struct bflb_device_s *gpio;
volatile int flag = 0;

struct foc_svpwm_s svpwm;
struct foc_hall_s hall;

void svpwm_bindto_bl616(void)
{
    uint16_t center = FOC_PWM_PERIOD_VALUE >> 1;
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_U, center - svpwm.u, center + svpwm.u);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_V, center - svpwm.v, center + svpwm.v);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_W, center - svpwm.w, center + svpwm.w);
}

void pwm_isr(int irq, void *arg)
{
    bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_PERIOD);
    foc_hall_calc_angle(&hall);
    svpwm.angle = hall.angle + 16384; /* add 90 degree */
    foc_svpwm_output(&svpwm);
}

void peri_pwm_init(void)
{
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_PBCLK,
        .clk_div = 1,
        .period = FOC_PWM_PERIOD_VALUE,
    };
    struct bflb_pwm_v2_channel_config_s ch_cfg = {
        .positive_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .negative_polarity = PWM_POLARITY_ACTIVE_HIGH,
        .positive_stop_state = PWM_STATE_INACTIVE,
        .negative_stop_state = PWM_STATE_ACTIVE,
        .positive_brake_state = PWM_STATE_INACTIVE,
        .negative_brake_state = PWM_STATE_ACTIVE,
        .dead_time = FOC_PWM_DEAD_TIME,
    };

    /* config pin */
    bflb_gpio_init(gpio, PWM_PIN_U_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_U_L, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_V_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_V_L, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_W_H, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, PWM_PIN_W_L, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_DIFF_END);
    bflb_pwm_v2_init(pwm, &cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_TRI_CH_U, &ch_cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_TRI_CH_V, &ch_cfg);
    bflb_pwm_v2_channel_init(pwm, PWM_TRI_CH_W, &ch_cfg);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_U, 0, 0);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_V, 0, 0);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_W, 0, 0);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_U);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_TRI_CH_U);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_V);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_TRI_CH_V);
    bflb_pwm_v2_channel_positive_start(pwm, PWM_TRI_CH_W);
    bflb_pwm_v2_channel_negative_start(pwm, PWM_TRI_CH_W);
    /* config interrupt */
    foc_svpwm_install_callback_set_pwm(&svpwm, svpwm_bindto_bl616);
    bflb_pwm_v2_int_enable(pwm, PWM_INTEN_PERIOD, true);
    bflb_irq_attach(pwm->irq_num, pwm_isr, NULL);
    // bflb_irq_enable(pwm->irq_num);
    /* start pwm */
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_SW_BRAKE_ENABLE, true);
    bflb_pwm_v2_start(pwm);
    /* delay for precharg */
    arch_delay_ms(2000);
    bflb_pwm_v2_feature_control(pwm, PWM_CMD_SET_SW_BRAKE_ENABLE, false);
    bflb_irq_enable(pwm->irq_num);
}

void delay(void)
{
    uint32_t i = 100;
    while (i--) {};
}

uint8_t hall_get(void)
{
    uint8_t hall = 0;
    if (bflb_gpio_read(gpio, HALL_PIN_U)) {
        hall |= (1 << 2);
    }
    if (bflb_gpio_read(gpio, HALL_PIN_V)) {
        hall |= (1 << 1);
    }
    if (bflb_gpio_read(gpio, HALL_PIN_W)) {
        hall |= (1 << 0);
    }
    return hall;
}

int main(void)
{
    board_init();
    pwm = bflb_device_get_by_name("pwm_v2_0");
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, HALL_PIN_U, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, HALL_PIN_V, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, HALL_PIN_W, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    foc_hall_install_callback_delay_1us(&hall, delay);
    foc_hall_install_callback_read_hall_pin(&hall, hall_get);
    foc_hall_init(&hall);
    svpwm.angle = hall.angle + 16384; /* add 90 degree */
    foc_svpwm_init(&svpwm);
    peri_pwm_init();
    while (1) {
        uint32_t tick = (uint32_t)hall.carrier_per_sector;
        printf("tick = %d, speed = %d rpm\n", tick, 600000 / 6 / 2 / tick);
        arch_delay_ms(1000);
    }
}
