#ifndef __FOC_H__
#define __FOC_H__

#include "stdint.h"
#include "foc_config.h"

#define ANGLE0                (0)
#define ANGLE30               (5461)
#define ANGLE60               (10922)
#define ANGLE90               (16384)
#define ANGLE120              (21845)
#define ANGLE150              (27306)
#define ANGLE180              (32768)
#define ANGLE210              (38229)
#define ANGLE240              (43690)
#define ANGLE270              (49151)
#define ANGLE300              (54613)
#define ANGLE330              (60074)
#define ANGLE360              (65535)

#define FOC_STATE_STOP        (0)
#define FOC_STATE_READY       (1)
#define FOC_STATE_BOOSTCHARGE (2)
#define FOC_STATE_RUN         (3)
#define FOC_STATE_TEST        (4)

#define FOC_PWM_PERIOD_VALUE  (FOC_PWM_SRC_CLK / FOC_PWM_CARRIER_FREQ) /* pwm tick count in one period */
#define FOC_PWM_PRECENT_MAX   (62258)                                  /* max percent of rotate vector modulus, 65535 * 95% */

struct foc_svpwm_s {
    uint16_t angle;                     /* [0, 65536) for [0degree, 360degree) */
    uint8_t sector;                     /* 1.[0, 60d) 2.[60d, 120d) ...... */
    int16_t sin;                        /* sin(angle), [-32767, 32767] for [-1, 1] */
    int16_t cos;                        /* cos(angle), [-32767, 32767] for [-1, 1] */
    int16_t x;                          /* x = sin(angle) */
    int16_t y;                          /* y = 1/2sin(angle) + sqrt(3)/2cos(angle) */
    int16_t z;                          /* y = 1/2sin(angle) - sqrt(3)/2cos(angle) */
    uint16_t precent;                   /* pwm period precent, value is [0, 65535] for [0%, 100%] */
    uint16_t precent_max;               /* restrict precent max value */
    uint16_t aon;                       /* one of tri_phase which pwm duty is max */
    uint16_t bon;                       /* one of tri_phase which pwm duty is middle */
    uint16_t con;                       /* one of tri_phase which pwm duty is min */
    uint16_t u;                         /* u phase value of pwm */
    uint16_t v;                         /* v phase value of pwm */
    uint16_t w;                         /* w phase value of pwm */
    void (*callback_set_pwm)(void);     /* set pwm, it is related to the specific PWM module */
    void (*callback_close_pwm)(void);   /* close pwm, it is related to the specific PWM module */
    void (*callback_boostcharge)(void); /* drive pwm to boost charge, it is related to the specific PWM module */
};

struct foc_hall_s {
    uint16_t angle;                          /* this angle is computed from hall state */
    uint8_t hall_state;                      /* now state of hall */
    uint8_t hall_prestate;                   /* previous state of hall */
    uint8_t direction;                       /* motor ratate direction, computed by hall state */
    uint8_t err;                             /* error state */
    uint16_t carrier_per_sector;             /* carrier period count in one sector */
    uint16_t carrier_per_sector_running;     /* carrier period count in one sector, in sector boundary assign carrier_per_sector */
    void (*callback_delay_1us)(void);        /* delay 1us, it is related to the specific CPU */
    uint8_t (*callback_read_hall_pin)(void); /* read hall pin state, it is related to the specific IC */
};

struct foc_curr_s {
    uint16_t angle;                           /* [0, 65536) for [0degree, 360degree) */
    uint16_t adc_u;                           /* raw ADC value of current of U phase, should unsigned 16bits and left-aligned */
    uint16_t adc_v;                           /* raw ADC value of current of V phase, should unsigned 16bits and left-aligned */
    uint16_t adc_w;                           /* raw ADC value of current of W phase, should unsigned 16bits and left-aligned */
    uint16_t offset_u;                        /* offset(ADC value) of U phase when U current = 0 */
    uint16_t offset_v;                        /* offset(ADC value) of U phase when V current = 0 */
    uint16_t offset_w;                        /* offset(ADC value) of U phase when W current = 0 */
    int16_t a;                                /* current of a axis removed offset */
    int16_t b;                                /* current of b axis removed offset */
    int16_t c;                                /* current of c axis removed offset */
    int16_t alpha;                            /* alpha current after clark transformation */
    int16_t beta;                             /* beta current after clark transformation */
    int16_t d;                                /* d current after park transformation */
    int16_t q;                                /* q current after park transformation */
    uint8_t resistor_solution;                /* resistor solution, available value is 1, 2, 3 */
    void (*callback_read_adc)(int16_t *buff); /* how to read adc device, buff is ordered with: U, V, W, total, bus...... */
};

struct foc_s {
    struct foc_svpwm_s svpwm;
    struct foc_hall_s hall;
    struct foc_curr_s curr;
    uint8_t state;             /* foc work state */
    uint8_t direction;         /* motor rotate direction */
    uint32_t time_boostcharge; /* unit: pwm period, time for lower bridge charge */
    int16_t q_given;
    int16_t d_given;
    void (*callback_disable_irq)(void);
    void (*callback_enable_irq)(void);
};

void foc_svpwm_init(struct foc_svpwm_s *svpwm);
void foc_svpwm_output(struct foc_svpwm_s *svpwm);
void foc_svpwm_install_callback_set_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void));
void foc_svpwm_install_callback_close_pwm(struct foc_svpwm_s *svpwm, void (*callback)(void));

void foc_hall_init(struct foc_hall_s *hall);
void foc_hall_calc_angle(struct foc_hall_s *hall);
void foc_hall_install_callback_delay_1us(struct foc_hall_s *hall, void (*callback)(void));
void foc_hall_install_callback_read_hall_pin(struct foc_hall_s *hall, uint8_t (*callback)(void));

void foc_curr_calc_abc(struct foc_curr_s *curr);
void foc_curr_calc_clark(struct foc_curr_s *curr);
void foc_curr_calc_park(struct foc_curr_s *curr);
void foc_curr_install_callback_read_adc(struct foc_curr_s *curr);
void foc_curr_filter(struct foc_s *foc);
void foc_state_switch(struct foc_s *foc);

void foc_state_switch(struct foc_s *foc);
void foc_timer_counter(struct foc_s *foc);
void foc_start(struct foc_s *foc);

#endif /* __FOC_H__ */
