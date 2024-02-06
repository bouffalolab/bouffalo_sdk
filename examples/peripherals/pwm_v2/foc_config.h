#ifndef __FOC_CONFIG_H__
#define __FOC_CONFIG_H__

#define FOC_PWM_SRC_CLK       (80 * 1000 * 1000) /* 80MHz */
#define FOC_PWM_CARRIER_FREQ  (10 * 1000)        /* 10KHz */
#define FOC_PWM_DEAD_TIME     (0x90)             /* usually 2us, depend on how IC calculate dead time */

#define FOC_RESISTOR_SOLUTION (2) /* resistor solution, available value is 1, 2, 3 */

#define FOC_TIME_BOOSTCHARGE  (20000) /* unit: pwm period, time for lower bridge charge */
#define FOC_TIME_START_DELAY  (10000) /* unit: pwm period, time for sample static current offset */

#define CW                    (0)
#define CCW                   (1)

#endif /* __FOC_CONFIG_H__ */
