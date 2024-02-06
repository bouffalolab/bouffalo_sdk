#include "foc.h"

static int16_t foc_sin_tab[] = {
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

/***************************************************************************/
/*                               FOC COMMON                                */
/***************************************************************************/
uint8_t foc_calc_sector(uint16_t angle)
{
    if (angle < ANGLE60) {
        return 1;
    } else if (angle < ANGLE120) {
        return 2;
    } else if (angle < ANGLE180) {
        return 3;
    } else if (angle < ANGLE240) {
        return 4;
    } else if (angle < ANGLE300) {
        return 5;
    } else {
        return 6;
    }
}

void foc_calc_sincos(uint16_t angle, int16_t *sin_val, int16_t *cos_val)
{
    uint16_t idx, frac;
    int32_t val1, val2, val;

    idx = angle >> 8;
    frac = angle & 0xFF;
    if (frac == 0) {
        /* angle is just in sin table */
        *sin_val = foc_sin_tab[idx];
        idx += 64; /* cos(degree) = sin(degree + 90d), 65536 / 4 / 256 = 64 */
        idx &= 0xFF;
        *cos_val = foc_sin_tab[idx];
    } else {
        /* line calc, sin */
        val1 = (int32_t)(foc_sin_tab[idx]);
        val2 = (int32_t)(foc_sin_tab[idx + 1]);
        val = val2 - val1;
        val = val * frac;
        val >>= 8;
        val += val1;
        *sin_val = (int16_t)val;
        /* line calc, cos */
        idx += 64; /* cos(degree) = sin(degree + 90d), 65536 / 4 / 256 = 64 */
        idx &= 0xFF;
        val1 = (int32_t)(foc_sin_tab[idx]);
        val2 = (int32_t)(foc_sin_tab[idx + 1]);
        val = val2 - val1;
        val = val * frac;
        val >>= 8;
        val += val1;
        *cos_val = (int16_t)val;
    }
}

/***************************************************************************/
/*                                 SVPWM                                   */
/***************************************************************************/
void foc_svpwm_init(struct foc_svpwm_s *svpwm);
void foc_svpwm_calc_xyz(struct foc_svpwm_s *svpwm);
void foc_svpwm_calc_duty(struct foc_svpwm_s *svpwm, int16_t v1t, int16_t v2t);
void foc_svpwm_calc_pwm_threshold(struct foc_svpwm_s *svpwm);
void foc_svpwm_output(struct foc_svpwm_s *svpwm);
void foc_svpwm_install_callback_set_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void));
void foc_svpwm_install_callback_close_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void));
void foc_svpwm_install_callback_boostcharge_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void));

void foc_svpwm_init(struct foc_svpwm_s *svpwm)
{
    svpwm->precent = 10000;
    svpwm->precent_max = FOC_PWM_PRECENT_MAX;
}

void foc_svpwm_calc_xyz(struct foc_svpwm_s *svpwm)
{
    int32_t sqrt3div2;

    sqrt3div2 = 56755; /* =sqrt(3)/2*65535 */
    sqrt3div2 *= svpwm->cos;
    sqrt3div2 >>= 16;
    svpwm->x = svpwm->sin;
    svpwm->y = (svpwm->sin >> 1) + sqrt3div2;
    svpwm->z = (svpwm->sin >> 1) - sqrt3div2;
}

void foc_svpwm_calc_duty(struct foc_svpwm_s *svpwm, int16_t v1t, int16_t v2t)
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

void foc_svpwm_calc_pwm_threshold(struct foc_svpwm_s *svpwm)
{
    uint16_t t1, t2, t3, tmax;

    switch (svpwm->sector) {
        case 1:
            foc_svpwm_calc_duty(svpwm, 0 - svpwm->z, svpwm->x);
            t1 = svpwm->aon;
            t2 = svpwm->bon;
            t3 = svpwm->con;
            break;
        case 2:
            foc_svpwm_calc_duty(svpwm, svpwm->z, svpwm->y);
            t1 = svpwm->bon;
            t2 = svpwm->aon;
            t3 = svpwm->con;
            break;
        case 3:
            foc_svpwm_calc_duty(svpwm, svpwm->x, 0 - svpwm->y);
            t1 = svpwm->con;
            t2 = svpwm->aon;
            t3 = svpwm->bon;
            break;
        case 4:
            foc_svpwm_calc_duty(svpwm, 0 - svpwm->x, svpwm->z);
            t1 = svpwm->con;
            t2 = svpwm->bon;
            t3 = svpwm->aon;
            break;
        case 5:
            foc_svpwm_calc_duty(svpwm, 0 - svpwm->y, 0 - svpwm->z);
            t1 = svpwm->bon;
            t2 = svpwm->con;
            t3 = svpwm->aon;
            break;
        case 6:
            foc_svpwm_calc_duty(svpwm, svpwm->y, 0 - svpwm->x);
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

void foc_svpwm_output(struct foc_svpwm_s *svpwm)
{
    foc_calc_sincos(svpwm->angle, &(svpwm->sin), &(svpwm->cos));
    foc_svpwm_calc_xyz(svpwm);
    svpwm->sector = foc_calc_sector(svpwm->angle);
    foc_svpwm_calc_pwm_threshold(svpwm);
    svpwm->callback_set_pwm();
}

void foc_svpwm_install_callback_set_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void))
{
    svpwm->callback_set_pwm = callback;
}

void foc_svpwm_install_callback_close_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void))
{
    svpwm->callback_close_pwm = callback;
}

void foc_svpwm_install_callback_boostcharge_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void))
{
    svpwm->callback_boostcharge = callback;
}

/***************************************************************************/
/*                                  HALL                                   */
/***************************************************************************/
void foc_hall_read(struct foc_hall_s *hall);
void foc_hall_init(struct foc_hall_s *hall);
void foc_hall_calc_angle(struct foc_hall_s *hall);
void foc_hall_install_callback_delay_1us(struct foc_hall_s *hall, void (*callback)(void));
void foc_hall_install_callback_read_hall_pin(struct foc_hall_s *hall, uint8_t (*callback)(void));

void foc_hall_read(struct foc_hall_s *hall)
{
    uint8_t t0, t1, t2;

    t0 = hall->callback_read_hall_pin();
    hall->callback_delay_1us();
    t1 = hall->callback_read_hall_pin();
    hall->callback_delay_1us();
    t2 = hall->callback_read_hall_pin();

    if ((t0 == t1) && (t0 == t2)) {
        hall->hall_prestate = hall->hall_state;
        hall->hall_state = t0;
    }
}

void foc_hall_init(struct foc_hall_s *hall)
{
    foc_hall_read(hall);
    hall->carrier_per_sector_running = 0;
    hall->carrier_per_sector = 65535;
    switch (hall->hall_state) {
        case 5:
            hall->angle = ANGLE60;
            break;
        case 4:
            hall->angle = ANGLE120;
            break;
        case 6:
            hall->angle = ANGLE180;
            break;
        case 2:
            hall->angle = ANGLE240;
            break;
        case 3:
            hall->angle = ANGLE300;
            break;
        case 1:
            hall->angle = ANGLE0;
            break;
        default:
            break;
    }
}

void foc_hall_calc_angle(struct foc_hall_s *hall)
{
    foc_hall_read(hall);
    if (hall->hall_prestate == hall->hall_state) {
        if (hall->direction == CW) {
            hall->angle += 10923 / hall->carrier_per_sector; /* 65536 / 6 = 10923degree in every sector */
        } else {
            hall->angle -= 10923 / hall->carrier_per_sector;
        }
        hall->carrier_per_sector_running++;
    } else {
        switch (hall->hall_state) {
            case 5:
                if (hall->hall_prestate == 1) {
                    hall->angle = ANGLE30;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else if (hall->hall_prestate == 4) {
                    hall->angle = ANGLE90;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else {
                    hall->err = 1;
                }
                break;
            case 4:
                if (hall->hall_prestate == 5) {
                    hall->angle = ANGLE90;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else if (hall->hall_prestate == 6) {
                    hall->angle = ANGLE150;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else {
                    hall->err = 1;
                }
                break;
            case 6:
                if (hall->hall_prestate == 4) {
                    hall->angle = ANGLE150;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else if (hall->hall_prestate == 2) {
                    hall->angle = ANGLE210;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else {
                    hall->err = 1;
                }
                break;
            case 2:
                if (hall->hall_prestate == 6) {
                    hall->angle = ANGLE210;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else if (hall->hall_prestate == 3) {
                    hall->angle = ANGLE270;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else {
                    hall->err = 1;
                }
                break;
            case 3:
                if (hall->hall_prestate == 2) {
                    hall->angle = ANGLE270;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else if (hall->hall_prestate == 1) {
                    hall->angle = ANGLE330;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else {
                    hall->err = 1;
                }
                break;
            case 1:
                if (hall->hall_prestate == 3) {
                    hall->angle = ANGLE330;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else if (hall->hall_prestate == 5) {
                    hall->angle = ANGLE30;
                    hall->carrier_per_sector = hall->carrier_per_sector_running;
                    hall->carrier_per_sector_running = 0;
                } else {
                    hall->err = 1;
                }
                break;
            default:
                hall->err = 1;
                break;
        }
    }
}

void foc_hall_install_callback_delay_1us(struct foc_hall_s *hall, void (*callback)(void))
{
    hall->callback_delay_1us = callback;
}

void foc_hall_install_callback_read_hall_pin(struct foc_hall_s *hall, uint8_t (*callback)(void))
{
    hall->callback_read_hall_pin = callback;
}

/***************************************************************************/
/*                                  CURR                                   */
/***************************************************************************/
void foc_curr_calc_abc(struct foc_curr_s *curr);
void foc_curr_calc_clark(struct foc_curr_s *curr);
void foc_curr_calc_park(struct foc_curr_s *curr);
void foc_curr_install_callback_read_adc(struct foc_curr_s *curr);
void foc_curr_filter(struct foc_s *foc);
void foc_state_switch(struct foc_s *foc);

void foc_curr_calc_abc(struct foc_curr_s *curr)
{
    int32_t a, b, c;
#if (FOC_RESISTOR_SOLUTION == 3)
    uint8_t sector;
#endif
    a = (int32_t)curr->offset_u - (int32_t)curr->adc_u;
    b = (int32_t)curr->offset_v - (int32_t)curr->adc_v;
    c = (int32_t)curr->offset_w - (int32_t)curr->adc_w;
    a = (a < -32768) ? (-32768) : a;
    a = (a > 32767) ? 32767 : a;
    b = (b < -32768) ? (-32768) : b;
    b = (b > 32767) ? 32767 : b;
    c = (c < -32768) ? (-32768) : c;
    c = (c > 32767) ? 32767 : c;
#if (FOC_RESISTOR_SOLUTION == 1)
#error not support FOC_RESISTOR_SOLUTION = 1
#elif (FOC_RESISTOR_SOLUTION == 2)
    curr->a = (int16_t)a;
    curr->b = (int16_t)b;
    curr->c = 0;
#elif (FOC_RESISTOR_SOLUTION == 3)
    sector = foc_calc_sector(curr->angle);
    if ((sector == 1) || (sector == 6)) {
        a = 0 - b - c;
    } else if ((sector == 2) || (sector == 3)) {
        b = 0 - a - c;
    } else if ((sector == 4) || (sector == 5)) {
        c = 0 - a - b;
    }
    curr->a = (int16_t)a;
    curr->b = (int16_t)b;
    curr->c = (int16_t)c;
#else
#error not support this FOC_RESISTOR_SOLUTION, please select another
#endif
}

void foc_curr_calc_clark(struct foc_curr_s *curr)
{
    int32_t a, b;

    curr->alpha = curr->a; /* no 3/2, because scale 3/2 */
    a = 37837 * curr->a;   /* 1 / sqrt(3) * 65536 = 37837*/
    b = 37837 * curr->b;   /* b */
    b += b;                /* 2b */
    a += b;                /* a + 2b */
    a >>= 16;
    curr->beta = (int16_t)a;
}

void foc_curr_calc_park(struct foc_curr_s *curr)
{
    int16_t sin, cos;
    int32_t t1, t2;

    foc_calc_sincos(curr->angle, &sin, &cos);
    t1 = curr->beta * sin;
    t2 = curr->alpha * cos;
    t1 = t1 + t2;
    t1 >>= 16;
    curr->d = (int16_t)t1;
    t1 = curr->beta * cos;
    t2 = curr->alpha * sin;
    t1 = t1 - t2;
    t1 >>= 16;
    curr->q = (int16_t)t1;
}

void foc_curr_filter(struct foc_s *foc)
{
    static int32_t sum_u = 0;
    static int32_t sum_v = 0;
    static int32_t sum_w = 0;
    static uint32_t idx = 0;
    static uint32_t count = FOC_TIME_START_DELAY;

    if (count > 0) {
        count--;
        sum_u += foc->curr.adc_u;
        sum_v += foc->curr.adc_v;
        sum_w += foc->curr.adc_w;
        idx++;
        if (idx >= 64) {
            foc->curr.offset_u = sum_u >> 6;
            foc->curr.offset_v = sum_v >> 6;
            foc->curr.offset_w = sum_w >> 6;
            sum_u = 0;
            sum_v = 0;
            sum_w = 0;
            idx = 0;
        }
    }
    foc->curr.offset_u = 21627;
    foc->curr.offset_v = 21627;
    foc->curr.offset_w = 21627;
}

void foc_state_switch(struct foc_s *foc)
{
    if (foc->time_boostcharge > 0) {
        foc->time_boostcharge--;
    }
    switch (foc->state) {
        case FOC_STATE_STOP:
            foc->svpwm.callback_close_pwm();
            break;
        case FOC_STATE_READY:
            foc_hall_init(&foc->hall);
            foc->state = FOC_STATE_BOOSTCHARGE;
            foc->svpwm.callback_boostcharge();
            foc->time_boostcharge = FOC_TIME_BOOSTCHARGE;
            break;
        case FOC_STATE_BOOSTCHARGE:
            if (foc->time_boostcharge == 0) {
                foc->svpwm.callback_close_pwm();
                if (foc->direction == CW) {
                    foc->q_given = 1024;
                } else {
                    foc->q_given = -1024;
                }
                foc->state = FOC_STATE_RUN;
            }
            break;
        case FOC_STATE_RUN:
            foc_hall_calc_angle(&foc->hall);
            break;
        case FOC_STATE_TEST:
            break;
        default:
            break;
    }
}
