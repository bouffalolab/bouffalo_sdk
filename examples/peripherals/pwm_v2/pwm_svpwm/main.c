#include "bflb_mtimer.h"
#include "bflb_pwm_v2.h"
#include "bflb_clock.h"
#include "board.h"
#include "math.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_gpio.h"

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
volatile int flag = 0;

#define ANGLE0               0
#define ANGLE30              5461
#define ANGLE60              10922
#define ANGLE90              16384
#define ANGLE120             21845
#define ANGLE150             27306
#define ANGLE180             32768
#define ANGLE210             38229
#define ANGLE240             43690
#define ANGLE270             49151
#define ANGLE300             54613
#define ANGLE330             60074
#define ANGLE360             65535

#define FOC_PWM_SRC_CLK      (80 * 1000 * 1000)                                          /* 80MHz */
#define FOC_PWM_CARRIER_FREQ (10 * 1000)                                                 /* 20KHz */
#define FOC_PWM_PERIOD_VALUE (FOC_PWM_SRC_CLK / FOC_PWM_CARRIER_FREQ)                    /* 4000 count */
#define FOC_PWM_DEAD_TIME    (2 * FOC_PWM_PERIOD_VALUE * FOC_PWM_CARRIER_FREQ / 1000000) /* 2us */
#define FOC_PWM_PRECENT_MAX  (62258)                                                     /* 65535 * 95% */

int16_t foc_sin_tab[] = {
    0, 804, 1608, 2410, 3212, 4011, 4808, 5602,
    6393, 7179, 7962, 8739, 9512, 10278, 11039, 11793,
    12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530,
    18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
    23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790,
    27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
    30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971,
    32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757,
    32767, 32757, 32728, 32678, 32609, 32521, 32412, 32285,
    32137, 31971, 31785, 31580, 31356, 31113, 30852, 30571,
    30273, 29956, 29621, 29268, 28898, 28510, 28105, 27683,
    27245, 26790, 26319, 25832, 25329, 24811, 24279, 23731,
    23170, 22594, 22005, 21403, 20787, 20159, 19519, 18868,
    18204, 17530, 16846, 16151, 15446, 14732, 14010, 13279,
    12539, 11793, 11039, 10278, 9512, 8739, 7962, 7179,
    6393, 5602, 4808, 4011, 3212, 2410, 1608, 804,
    0, -804, -1608, -2410, -3212, -4011, -4808, -5602,
    -6393, -7179, -7962, -8739, -9512, -10278, -11039, -11793,
    -12539, -13279, -14010, -14732, -15446, -16151, -16846, -17530,
    -18204, -18868, -19519, -20159, -20787, -21403, -22005, -22594,
    -23170, -23731, -24279, -24811, -25329, -25832, -26319, -26790,
    -27245, -27683, -28105, -28510, -28898, -29268, -29621, -29956,
    -30273, -30571, -30852, -31113, -31356, -31580, -31785, -31971,
    -32137, -32285, -32412, -32521, -32609, -32678, -32728, -32757,
    -32767, -32757, -32728, -32678, -32609, -32521, -32412, -32285,
    -32137, -31971, -31785, -31580, -31356, -31113, -30852, -30571,
    -30273, -29956, -29621, -29268, -28898, -28510, -28105, -27683,
    -27245, -26790, -26319, -25832, -25329, -24811, -24279, -23731,
    -23170, -22594, -22005, -21403, -20787, -20159, -19519, -18868,
    -18204, -17530, -16846, -16151, -15446, -14732, -14010, -13279,
    -12539, -11793, -11039, -10278, -9512, -8739, -7962, -7179,
    -6393, -5602, -4808, -4011, -3212, -2410, -1608, -804,
    0
};

struct foc_svpwm_s {
    uint16_t angle;       /* [0, 65536) for [0degree, 360degree) */
    uint8_t sector;       /* 1.[0, 60d) 2.[60d, 120d) ...... */
    int16_t sin;          /* sin(angle), [-32767, 32767] for [-1, 1] */
    int16_t cos;          /* cos(angle), [-32767, 32767] for [-1, 1] */
    int16_t x;            /* x = sin(angle) */
    int16_t y;            /* y = 1/2sin(angle) + sqrt(3)/2cos(angle) */
    int16_t z;            /* y = 1/2sin(angle) - sqrt(3)/2cos(angle) */
    uint16_t precent;     /* pwm period precent, value is [0, 65535] for [0%, 100%] */
    uint16_t precent_max; /* restrict precent max value */
    uint16_t aon;         /* one of tri_phase which pwm duty is max */
    uint16_t bon;         /* one of tri_phase which pwm duty is middle */
    uint16_t con;         /* one of tri_phase which pwm duty is min */
    uint16_t u;           /* u phase value of pwm */
    uint16_t v;           /* v phase value of pwm */
    uint16_t w;           /* w phase value of pwm */
};

struct foc_svpwm_s svpwm;

void foc_init(struct foc_svpwm_s *svpwm)
{
    svpwm->precent = 10000;
    svpwm->precent_max = FOC_PWM_PRECENT_MAX;
}

void foc_calc_sincos(struct foc_svpwm_s *svpwm)
{
    uint16_t idx, frac;
    int32_t val1, val2, val;

    idx = svpwm->angle >> 8;
    frac = svpwm->angle & 0xFF;
    if (frac == 0) {
        /* angle is just in sin table */
        svpwm->sin = foc_sin_tab[idx];
        idx += 64; /* cos(degree) = sin(degree + 90d), 65536 / 4 / 256 = 64 */
        idx &= 0xFF;
        svpwm->cos = foc_sin_tab[idx];
    } else {
        /* line calc, sin */
        val1 = (int32_t)(foc_sin_tab[idx]);
        val2 = (int32_t)(foc_sin_tab[idx + 1]);
        val = val2 - val1;
        val = val * frac;
        val >>= 8;
        val += val1;
        svpwm->sin = (int16_t)val;
        /* line calc, cos */
        idx += 64; /* cos(degree) = sin(degree + 90d), 65536 / 4 / 256 = 64 */
        idx &= 0xFF;
        val1 = (int32_t)(foc_sin_tab[idx]);
        val2 = (int32_t)(foc_sin_tab[idx + 1]);
        val = val2 - val1;
        val = val * frac;
        val >>= 8;
        val += val1;
        svpwm->cos = (int16_t)val;
    }
}

void foc_calc_xyx(struct foc_svpwm_s *svpwm)
{
    int32_t sqrt3div2;

    sqrt3div2 = 56755; /* =sqrt(3)/2*65535 */
    sqrt3div2 *= svpwm->cos;
    sqrt3div2 >>= 16;
    svpwm->x = svpwm->sin;
    svpwm->y = (svpwm->sin >> 1) + sqrt3div2;
    svpwm->z = (svpwm->sin >> 1) - sqrt3div2;
}

void foc_calc_sector(struct foc_svpwm_s *svpwm)
{
    uint16_t angle;

    angle = svpwm->angle;
    if (angle < ANGLE60) {
        svpwm->sector = 1;
    } else if (angle < ANGLE120) {
        svpwm->sector = 2;
    } else if (angle < ANGLE180) {
        svpwm->sector = 3;
    } else if (angle < ANGLE240) {
        svpwm->sector = 4;
    } else if (angle < ANGLE300) {
        svpwm->sector = 5;
    } else {
        svpwm->sector = 6;
    }
}

void foc_calc_duty(struct foc_svpwm_s *svpwm, int16_t v1t, int16_t v2t)
{
    uint32_t temp;
    uint32_t v1, v2;

    v1t = (v1t > 0) ? v1t : 0;
    v2t = (v2t > 0) ? v2t : 0;
    if (svpwm->precent > svpwm->precent_max) {
        svpwm->precent = svpwm->precent_max;
    }
    temp = FOC_PWM_PERIOD_VALUE * svpwm->precent;
    temp >>= 16;
    v1 = (uint32_t)v1t; /* v1t / 2 */
    v2 = (uint32_t)v2t; /* v2t / 2 */
    v1 *= temp;
    v2 *= temp;
    v1 >>= 16;
    v2 >>= 16;
    svpwm->con = ((FOC_PWM_PERIOD_VALUE >> 1) - v1 - v2) >> 1; /* (T/2 - T1/2 - T2/2) / 2 */
    svpwm->bon = svpwm->con + v2;
    svpwm->aon = svpwm->bon + v1;
}

void foc_calc_pwm_threshold(struct foc_svpwm_s *svpwm)
{
    uint16_t t1, t2, t3, tmax;

    switch (svpwm->sector) {
        case 1:
            foc_calc_duty(svpwm, 0 - svpwm->z, svpwm->x);
            t1 = svpwm->aon;
            t2 = svpwm->bon;
            t3 = svpwm->con;
            break;
        case 2:
            foc_calc_duty(svpwm, svpwm->z, svpwm->y);
            t1 = svpwm->bon;
            t2 = svpwm->aon;
            t3 = svpwm->con;
            break;
        case 3:
            foc_calc_duty(svpwm, svpwm->x, 0 - svpwm->y);
            t1 = svpwm->con;
            t2 = svpwm->aon;
            t3 = svpwm->bon;
            break;
        case 4:
            foc_calc_duty(svpwm, 0 - svpwm->x, svpwm->z);
            t1 = svpwm->con;
            t2 = svpwm->bon;
            t3 = svpwm->aon;
            break;
        case 5:
            foc_calc_duty(svpwm, 0 - svpwm->y, 0 - svpwm->z);
            t1 = svpwm->bon;
            t2 = svpwm->con;
            t3 = svpwm->aon;
            break;
        case 6:
            foc_calc_duty(svpwm, svpwm->y, 0 - svpwm->x);
            t1 = svpwm->aon;
            t2 = svpwm->con;
            t3 = svpwm->bon;
            break;
        default:
            t1 = 0;
            t2 = 0;
            t3 = 0;
            break;
    }
    t1 = (t1 > FOC_PWM_DEAD_TIME) ? t1 : FOC_PWM_DEAD_TIME;
    t2 = (t2 > FOC_PWM_DEAD_TIME) ? t2 : FOC_PWM_DEAD_TIME;
    t3 = (t3 > FOC_PWM_DEAD_TIME) ? t3 : FOC_PWM_DEAD_TIME;
    tmax = (FOC_PWM_PERIOD_VALUE >> 1) - FOC_PWM_DEAD_TIME;
    t1 = (t1 > tmax) ? tmax : t1;
    t2 = (t2 > tmax) ? tmax : t2;
    t3 = (t3 > tmax) ? tmax : t3;
    svpwm->u = t1;
    svpwm->v = t2;
    svpwm->w = t3;
}

void bl_pwm_out(struct foc_svpwm_s *svpwm)
{
    uint16_t center = FOC_PWM_PERIOD_VALUE >> 1;
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_U, center - svpwm->u, center + svpwm->u);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_V, center - svpwm->v, center + svpwm->v);
    bflb_pwm_v2_channel_set_threshold(pwm, PWM_TRI_CH_W, center - svpwm->w, center + svpwm->w);
}

void pwm_isr(int irq, void *arg)
{
    bflb_pwm_v2_int_clear(pwm, PWM_INTSTS_PERIOD);
    *(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFG17_OFFSET) = 0x00400B6A | (1 << 24);
    // svpwm.angle += 100;
    foc_calc_sincos(&svpwm);
    foc_calc_xyx(&svpwm);
    foc_calc_sector(&svpwm);
    foc_calc_pwm_threshold(&svpwm);
    bl_pwm_out(&svpwm);
    *(volatile uint32_t *)(GLB_BASE + GLB_GPIO_CFG17_OFFSET) = 0x00400B6A;
    if (flag) {
        printf("%d, angle = %5d, sector = %d, sin = %5d, cos = %5d, x = %5d, y = %5d, z = %5d, aon = %5d, bon = %5d, con = %5d, u = %5d, v = %5d, w = %5d\n",
               svpwm.angle / 182,
               svpwm.angle,
               svpwm.sector,
               svpwm.sin,
               svpwm.cos,
               svpwm.x,
               svpwm.y,
               svpwm.z,
               svpwm.aon,
               svpwm.bon,
               svpwm.con,
               svpwm.u,
               svpwm.v,
               svpwm.w);
        flag = 0;
    }
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

    pwm = bflb_device_get_by_name("pwm_v2_0");
    gpio = bflb_device_get_by_name("gpio");

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

int main(void)
{
    board_init();
    foc_init(&svpwm);
    peri_pwm_init();
    // printf("angle = %5d, sector = %d, sin = %5d, cos = %5d, x = %5d, y = %5d, z = %5d, aon = %5d, bon = %5d, con = %5d, u = %5d, v = %5d, w = %5d\n",
    //         svpwm.angle,
    //         svpwm.sector,
    //         svpwm.sin,
    //         svpwm.cos,
    //         svpwm.x,
    //         svpwm.y,
    //         svpwm.z,
    //         svpwm.aon,
    //         svpwm.bon,
    //         svpwm.con,
    //         svpwm.u,
    //         svpwm.v,
    //         svpwm.w);
    bflb_gpio_init(gpio, GPIO_PIN_32, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_33, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_34, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    while (1) {
        bool key100 = bflb_gpio_read(gpio, GPIO_PIN_32);
        bool key10 = bflb_gpio_read(gpio, GPIO_PIN_33);
        bool key1 = bflb_gpio_read(gpio, GPIO_PIN_34);
        static uint16_t angle = 0;
        if (!key100) {
            angle += 18204;
        } else if (!key10) {
            angle += 1820;
        } else if (!key1) {
            angle += 182;
        }
        if (angle != svpwm.angle) {
            svpwm.angle = angle;
            // printf("angel = %d\n", svpwm.angle);
            flag = 1;
        }
        arch_delay_ms(150);
    }
}
