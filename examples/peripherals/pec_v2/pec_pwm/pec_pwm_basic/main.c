#include "board.h"
#include "bflb_pec_v2_instance.h"

struct bflb_device_s *pec_pwm0;
struct bflb_device_s *pec_pwm1;
#if !defined(BL616L)
struct bflb_device_s *pec_pwm2;
struct bflb_device_s *pec_pwm3;
#endif

struct bflb_pec_pwm_s pwm0_cfg = {
    .mem = 0,               /*!< memory address of first instruction */
    .div = 0,               /*!< divisor, N = div + 1 */
    .pin = PEC_PWM_CH0_PIN, /*!< PWM output assert pin index */
    .inverse = 0,           /*!< toggle pin level */
};

struct bflb_pec_pwm_s pwm1_cfg = {
    .mem = 0,               /*!< memory address of first instruction */
    .div = 1,               /*!< divisor, N = div + 1 */
    .pin = PEC_PWM_CH1_PIN, /*!< PWM output assert pin index */
    .inverse = 1,           /*!< toggle pin level */
};

#if !defined(BL616L)
struct bflb_pec_pwm_s pwm2_cfg = {
    .mem = 0,               /*!< memory address of first instruction */
    .div = 2,               /*!< divisor, N = div + 1 */
    .pin = PEC_PWM_CH2_PIN, /*!< PWM output assert pin index */
    .inverse = 0,           /*!< toggle pin level */
};

struct bflb_pec_pwm_s pwm3_cfg = {
    .mem = 0,               /*!< memory address of first instruction */
    .div = 3,               /*!< divisor, N = div + 1 */
    .pin = PEC_PWM_CH3_PIN, /*!< PWM output assert pin index */
    .inverse = 1,           /*!< toggle pin level */
};
#endif

int main(void)
{
    board_init();
    board_pec_pwm_gpio_init();
    pec_pwm0 = bflb_device_get_by_name("pec_sm0");
    printf("pec_pwm0 = 0x%08lX\r\n", pec_pwm0);
    pec_pwm1 = bflb_device_get_by_name("pec_sm1");
    printf("pec_pwm1 = 0x%08lX\r\n", pec_pwm1);
#if !defined(BL616L)
    pec_pwm2 = bflb_device_get_by_name("pec_sm2");
    printf("pec_pwm2 = 0x%08lX\r\n", pec_pwm2);
    pec_pwm3 = bflb_device_get_by_name("pec_sm3");
    printf("pec_pwm3 = 0x%08lX\r\n", pec_pwm3);
#endif

    bflb_pec_pwm_init(pec_pwm0, &pwm0_cfg);
    bflb_pec_pwm_init(pec_pwm1, &pwm1_cfg);
#if !defined(BL616L)
    bflb_pec_pwm_init(pec_pwm2, &pwm2_cfg);
    bflb_pec_pwm_init(pec_pwm3, &pwm3_cfg);
#endif
    bflb_pec_pwm_start(pec_pwm0, 8, 13);
    bflb_pec_pwm_start(pec_pwm1, 5, 4);
#if !defined(BL616L)
    bflb_pec_pwm_start(pec_pwm2, 18, 27);
    bflb_pec_pwm_start(pec_pwm3, 20, 10);
#endif
    printf("pec_pwm start\r\n");
    while (1) {
        static uint32_t cnt = 0;
        bflb_mtimer_delay_ms(1000);
        printf("cnt = %ld\r\n", cnt++);
    }
}
