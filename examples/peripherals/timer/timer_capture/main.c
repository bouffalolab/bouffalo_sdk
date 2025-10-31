#include "bflb_mtimer.h"
#include "bflb_timer.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_pwm_v2.h"
#include "bflb_uart.h"
#include "board.h"
#include "shell.h"
#include "hardware/timer_reg.h"
#if defined(BL616)
#include "bflb_acomp.h"
#include "bl616_hbn.h"
#include "bl616_aon.h"
#endif

struct bflb_device_s *timer0;
struct bflb_device_s *uart0;
struct bflb_device_s *pwm0;
struct bflb_device_s *gpio;

#define PWM_OUTPUT_PIN                            GPIO_PIN_0
#define TIMER_CAPTURE_PIN                         GPIO_PIN_1

#define GPIO_PULSE_CREATE_PIN                     GPIO_PIN_3
/* for bl616: GPIO20 ADC CH0, for bl616d: SWGPIO20*/
#define GPIO_FOR_INT_PIN                          GPIO_PIN_20
#define TEST_FOR_INT_DELAY_PIN                    GPIO_PIN_27

#define PULSE_RECORD_COUNT_MAX                    16

#define TIMER_CAPTURE_EVENT_GPIO_INT_FALLING_EDGE 1
#define TIMER_CAPTURE_EVENT_GPIO_INT_RISING_EDGE  2

struct timer_capture_event_info {
    uint8_t event;
    uint32_t lat1;
    uint32_t lat2;
    uint32_t counter;
};

struct timer_capture_event_info event_infos[PULSE_RECORD_COUNT_MAX] = { 0 };
static uint32_t toatl_timer_capture_event = 0;

void timer_capture_get_event_info(struct timer_capture_event_info *event_info)
{
    uint32_t lat1 = 0;
    uint32_t lat2 = 0;

    do {
        lat1 = getreg32(timer0->reg_base + TIMER_GPIO_LAT1_OFFSET);
        lat2 = getreg32(timer0->reg_base + TIMER_GPIO_LAT2_OFFSET);
    } while (0);

    event_info->lat1 = lat1;
    event_info->lat2 = lat2;
    event_info->counter = bflb_timer_get_countervalue(timer0);
}

#if defined(BL616)
static void hbn_acomp_irq_handler_cb(void)
{
    if (HBN_Get_INT_State(HBN_INT_ACOMP0)) {
        struct timer_capture_event_info *event_info = &event_infos[toatl_timer_capture_event % PULSE_RECORD_COUNT_MAX];
        timer_capture_get_event_info(event_info);

        if (bflb_acomp_get_result(AON_ACOMP0_ID) == 1) {
            event_info->event = TIMER_CAPTURE_EVENT_GPIO_INT_RISING_EDGE;
        } else {
            event_info->event = TIMER_CAPTURE_EVENT_GPIO_INT_FALLING_EDGE;
            bflb_gpio_set(gpio, TEST_FOR_INT_DELAY_PIN);
            if (event_info->lat2 - event_info->lat1 < 500 * 40) {
                bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 250);
            } else if (event_info->lat2 - event_info->lat1 < 1000 * 40) {
                bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 500);
            } else if (event_info->lat2 - event_info->lat1 < 1500 * 40) {
                bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 750);
            } else {
                bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 1000);
            }
        }
        HBN_Clear_IRQ(HBN_INT_ACOMP0);
        toatl_timer_capture_event++;
    }
    bflb_gpio_reset(gpio, TEST_FOR_INT_DELAY_PIN);
}

void acomp_init()
{
    /* ADC_CH0 */
    bflb_gpio_init(gpio, GPIO_FOR_INT_PIN, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    struct bflb_acomp_config_s acomp_cfg = {
        .mux_en = ENABLE,
        .pos_chan_sel = AON_ACOMP_CHAN_ADC0,
        .neg_chan_sel = AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1,
        .vio_sel = DEFAULT_ACOMP_VREF_1V65,
        .scaling_factor = AON_ACOMP_SCALING_FACTOR_1,
        .bias_prog = AON_ACOMP_BIAS_POWER_MODE1,
        .hysteresis_pos_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
        .hysteresis_neg_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
    };

    bflb_acomp_init(AON_ACOMP0_ID, &acomp_cfg);
    bflb_acomp_enable(AON_ACOMP0_ID);
    HBN_Clear_IRQ(HBN_INT_ACOMP0);

    HBN_Enable_AComp_IRQ(AON_ACOMP0_ID, HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE);

    bflb_irq_attach(HBN_OUT1_IRQn, (void *)hbn_acomp_irq_handler_cb, NULL);
    bflb_irq_enable(HBN_OUT1_IRQn);
}
#elif defined(BL616D)
void gpio_isr(uint8_t pin)
{
    struct timer_capture_event_info *event_info = &event_infos[toatl_timer_capture_event % PULSE_RECORD_COUNT_MAX];
    toatl_timer_capture_event++;

    if (bflb_gpio_read(gpio, GPIO_FOR_INT_PIN) == 1) {
        event_info->event = TIMER_CAPTURE_EVENT_GPIO_INT_RISING_EDGE;
    } else {
        event_info->event = TIMER_CAPTURE_EVENT_GPIO_INT_FALLING_EDGE;
        bflb_gpio_set(gpio, TEST_FOR_INT_DELAY_PIN);
    }

    timer_capture_get_event_info(event_info);
    if (event_info->lat2 - event_info->lat1 < 500 * 40) {
        bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 250);
    } else if (event_info->lat2 - event_info->lat1 < 1000 * 40) {
        bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 500);
    } else if (event_info->lat2 - event_info->lat1 < 1500 * 40) {
        bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 750);
    } else {
        bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 1000);
    }
    bflb_gpio_reset(gpio, TEST_FOR_INT_DELAY_PIN);
}

void gpio_init()
{
    /* Capture external pulse inputs via ISR */
    bflb_gpio_init(gpio, GPIO_FOR_INT_PIN, GPIO_INPUT | GPIO_PULLDOWN | GPIO_SMT_EN);
    bflb_gpio_int_init(gpio, GPIO_FOR_INT_PIN, GPIO_INT_TRIG_MODE_SYNC_FALLING_RISING_EDGE);
    bflb_gpio_irq_attach(GPIO_FOR_INT_PIN, gpio_isr);
    bflb_irq_enable(gpio->irq_num);
}
#endif

void pulse_init()
{
    gpio = bflb_device_get_by_name("gpio");
    bflb_irq_disable(gpio->irq_num);
    /* External trigger pulse */
    bflb_gpio_init(gpio, GPIO_PULSE_CREATE_PIN, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_3);
    bflb_gpio_reset(gpio, GPIO_PULSE_CREATE_PIN);
    
    bflb_gpio_init(gpio, TEST_FOR_INT_DELAY_PIN, GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_3);
    bflb_gpio_reset(gpio, TEST_FOR_INT_DELAY_PIN);
}

int set_pulse_width(int argc, char **argv)
{
    uint32_t high_level_us;
    printf("Make a pulse\r\n");

    if ((argc != 2)) {
        printf("Usage: set_pulse_width <high-level time (us)>\r\n"
               "example: set_pulse_width 1000\r\n");
        return 1;
    }

    high_level_us = atoi(argv[1]);
    printf("Pulse delay %dus\r\n", high_level_us);

    bflb_gpio_set(gpio, GPIO_PULSE_CREATE_PIN);
    bflb_mtimer_delay_us(high_level_us);
    bflb_gpio_reset(gpio, GPIO_PULSE_CREATE_PIN);

    return 0;
}

static void dump_one_event(int index)
{
    int j = index % PULSE_RECORD_COUNT_MAX;
    if (event_infos[j].event == TIMER_CAPTURE_EVENT_GPIO_INT_RISING_EDGE) {
        printf("Event %02d:  Rising, Timestamp: 0x%08x, Counter: 0x%08x\r\n",
               index, event_infos[j].lat1, event_infos[j].counter);
    } else if (event_infos[j].event == TIMER_CAPTURE_EVENT_GPIO_INT_FALLING_EDGE) {
        printf("Event %02d: Falling, Timestamp: 0x%08x, Counter: 0x%08x\r\n",
               index, event_infos[j].lat2, event_infos[j].counter);
    } else {
        printf("Event %02d: Unknown, Timestamp: 0x0, Counter: 0x0\r\n", index);
    }
}

int dump_event_info(int argc, char **argv)
{
    int i;
    printf("Dump event info\r\n");
    printf("Total capture events: %lu\r\n", toatl_timer_capture_event);
    if (toatl_timer_capture_event <= PULSE_RECORD_COUNT_MAX) {
        printf("Latest %d event info:\r\n", toatl_timer_capture_event);
        for (i = 0; i < toatl_timer_capture_event; i++) {
            dump_one_event(i);
        }
    } else {
        printf("Latest %d event info:\r\n", PULSE_RECORD_COUNT_MAX);
        for (i = toatl_timer_capture_event - PULSE_RECORD_COUNT_MAX; i < toatl_timer_capture_event; i++) {
            dump_one_event(i);
        }
    }
    return 0;
}

void pwm_init()
{
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, PWM_OUTPUT_PIN, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    pwm0 = bflb_device_get_by_name("pwm_v2_0");

    /* period = .XCLK / .clk_div / .period = 40MHz / 40 / 1000 = 1KHz */
    struct bflb_pwm_v2_config_s cfg = {
        .clk_source = BFLB_SYSTEM_XCLK,
        .clk_div = 40,
        .period = 1000,
    };
    /* select pwm io differential end */
    bflb_pwm_v2_init(pwm0, &cfg);
    bflb_pwm_v2_channel_set_threshold(pwm0, PWM_CH0, 0, 250); /* duty = 25% */
    bflb_pwm_v2_channel_positive_start(pwm0, PWM_CH0);
    bflb_pwm_v2_start(pwm0);
}

void timer_capture_init()
{
    struct bflb_timer_config_s timer_cfg;
    timer_cfg.counter_mode = TIMER_COUNTER_MODE_UP;
    timer_cfg.clock_source = TIMER_CLKSRC_XTAL;
    timer_cfg.clock_div = 0;
    timer_cfg.trigger_comp_id = TIMER_COMP_ID_0;
    timer_cfg.comp0_val = 0xFFFFFFFF;
    timer_cfg.comp1_val = 0xFFFFFFFF;
    timer_cfg.comp2_val = 0xFFFFFFFF;
    timer_cfg.preload_val = 0;

    timer0 = bflb_device_get_by_name("timer0");
    struct bflb_timer_capture_config_s capture_cfg = {
        TIMER_CAPTURE_PIN,
        TIMER_GPIO_PULSE_POLARITY_POSITIVE,
    };

    bflb_timer_capture_init(timer0, &capture_cfg);
    bflb_timer_init(timer0, &timer_cfg);
    bflb_timer_start(timer0);
}

int main(void)
{
    int ch;
    board_init();
    printf("Timer capture test\n");
    uart0 = bflb_device_get_by_name("uart0");

    pwm_init();
    pulse_init();
#if defined(BL616)
    acomp_init();
#elif defined(BL616D)
    gpio_init();
#endif
    timer_capture_init();

    shell_init();

    while (1) {
        while ((ch = bflb_uart_getchar(uart0)) != -1) {
            shell_handler(ch);
        }
    }
}

SHELL_CMD_EXPORT_ALIAS(set_pulse_width, set_pulse_width, shell set_pulse_width.);
SHELL_CMD_EXPORT_ALIAS(dump_event_info, dump_event_info, shell dump_event_info.);