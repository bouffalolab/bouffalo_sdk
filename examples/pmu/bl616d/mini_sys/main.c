#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "bflb_timer.h"
#include "shell.h"
#include "board.h"
#include "bl616d_irq.h"
#include "bl616d_glb.h"

static struct bflb_device_s *uartx;
struct bflb_device_s *gpio;
char *g_argv[] = {
    "set_int_mode",
    "0"
};
#define TEST_TIMER_COMP_ID TIMER_COMP_ID_0
struct bflb_device_s *timer2;
struct bflb_device_s *timer0;
volatile int cpu_can_sleep = 0;

void timer2_isr(int irq, void *arg)
{
    bool status = bflb_timer_get_compint_status(timer2, TIMER_COMP_ID_0);
    if (status) {
        bflb_timer_compint_clear(timer2, TIMER_COMP_ID_0);
        printf("timer2 comp0 trigger\r\n");
    }
    status = bflb_timer_get_compint_status(timer2, TIMER_COMP_ID_1);
    if (status) {
        bflb_timer_compint_clear(timer2, TIMER_COMP_ID_1);
        printf("timer2 comp1 trigger\r\n");
    }
    status = bflb_timer_get_compint_status(timer2, TIMER_COMP_ID_2);
    if (status) {
        bflb_timer_compint_clear(timer2, TIMER_COMP_ID_2);
        printf("timer2 comp2 trigger\r\n");
    }
}

void timer2_demo()
{
    /* timer clk = XCLK/(div + 1 )*/
    struct bflb_timer_config_s cfg0;
    cfg0.counter_mode = TIMER_COUNTER_MODE_PROLOAD; /* preload when match occur */
    cfg0.clock_source = TIMER_CLKSRC_BCLK;
    cfg0.clock_div = 39;
    cfg0.trigger_comp_id = TEST_TIMER_COMP_ID;
    cfg0.comp0_val = 1000000;    /* match value 0  */
    cfg0.comp1_val = 0xFFFFFFFF; /* match value 1 */
    cfg0.comp2_val = 0xFFFFFFFF; /* match value 2 */
    cfg0.preload_val = 0;        /* preload value */

    timer2 = bflb_device_get_by_name("timer2");
    // timer1 = bflb_device_get_by_name("timer1");

    /* Timer init with default configuration */
    bflb_timer_init(timer2, &cfg0);

    bflb_irq_attach(timer2->irq_num, timer2_isr, NULL);
    bflb_irq_enable(timer2->irq_num);

    /* Enable timer */
    bflb_timer_start(timer2);
}

void timer0_isr(int irq, void *arg)
{
    bool status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_0);
    if (status) {
        bflb_timer_compint_clear(timer0, TIMER_COMP_ID_0);
        printf("timer0 comp0 trigger\r\n");
    }
    status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_1);
    if (status) {
        bflb_timer_compint_clear(timer0, TIMER_COMP_ID_1);
        printf("timer0 comp1 trigger\r\n");
    }
    status = bflb_timer_get_compint_status(timer0, TIMER_COMP_ID_2);
    if (status) {
        bflb_timer_compint_clear(timer0, TIMER_COMP_ID_2);
        printf("timer0 comp2 trigger\r\n");
    }
}

void timer0_demo()
{
    /* timer clk = XCLK/(div + 1 )*/
    struct bflb_timer_config_s cfg0;
    cfg0.counter_mode = TIMER_COUNTER_MODE_PROLOAD; /* preload when match occur */
    cfg0.clock_source = TIMER_CLKSRC_BCLK;
    cfg0.clock_div = 159;
    cfg0.trigger_comp_id = TEST_TIMER_COMP_ID;
    cfg0.comp0_val = 1000000;    /* match value 0  */
    cfg0.comp1_val = 0xFFFFFFFF; /* match value 1 */
    cfg0.comp2_val = 0xFFFFFFFF; /* match value 2 */
    cfg0.preload_val = 0;        /* preload value */

    timer0 = bflb_device_get_by_name("timer0");
    // timer1 = bflb_device_get_by_name("timer1");

    /* Timer init with default configuration */
    bflb_timer_init(timer0, &cfg0);

    /* unmask timer 0 interrupt */
    /* CAN NOT use API since interrupt register read will return erro value */
    //GLB_Set_NP2MINI_Interrupt_Mask(BL616D_IRQ_TIMER0,0);
    BL_WR_REG(GLB_BASE, GLB_CORE_CFG22, 0xFFFFFFFF);
    BL_WR_REG(GLB_BASE, GLB_CORE_CFG23, 0xFFFFFFEF);

    /* TODO:move start after irq enable */
    /* Enable timer */
    bflb_timer_start(timer0);

    /* attach and enable irq */
    //bflb_irq_attach(BL616D_IRQ_E907_63_0, test_isr, NULL);
    //bflb_irq_enable(BL616D_IRQ_E907_63_0);

    printf("enable timer0 interrupt:%d\r\n", timer0->irq_num);
    bflb_irq_attach(timer0->irq_num, timer0_isr, NULL);
    bflb_irq_enable(timer0->irq_num);
}

int main(void)
{
    int ch;
    uint64_t target_time_stamp = 0;
    board_init();

    /* clear all NP interrupt */
    GLB_Set_NP2MINI_Interrupt_Clear(BL616D_IRQ_E907_63_0);
    bflb_irq_clear_pending(BL616D_IRQ_E907_63_0);
    /* mask all NP interrupt */
    GLB_Set_NP2MINI_Interrupt_Mask(BL616D_IRQ_E907_63_0, 1);

    printf("BL616D Mini Sys Shell\r\n");
    uartx = bflb_device_get_by_name("uart3");
    gpio = bflb_device_get_by_name("gpio");

    shell_init();

    target_time_stamp = bflb_mtimer_get_time_us();
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
        if (bflb_mtimer_get_time_us() >= target_time_stamp) {
            target_time_stamp += 2000 * 1000;
            printf("Hello LP:%lld\r\n", bflb_mtimer_get_time_us());
            printf("np2mini_irq_state: 0x%llx\r\n", GLB_Get_NP2MINI_Interrupt_Status());
        }
        if (cpu_can_sleep) {
            printf("LP CPU can sleep now\r\n");
            cpu_can_sleep = 0;
            /* enter low power mode */
            __WFI();
        }
    }
}

int start_timer(int argc, char **argv)
{
    printf("start timer0 and timer2\r\n");
    timer2_demo();
    timer0_demo();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(start_timer, start_timer, shell start timer0 / 2.);

int wake_up_main_cpu(int argc, char **argv)
{
    printf("wakeup main cpu\r\n");
    PDS_Triger_SW_Wakeup_PDS();
    arch_delay_ms(1);
    PDS_Clear_SW_Wakeup_PDS();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(wake_up_main_cpu, wake_up, wake up main cpu.);

void gpio0_isr(uint8_t pin)
{
    static uint32_t i = 0;
    bflb_gpio_set(gpio, GPIO_PIN_1);
    printf("GPIO_PIN_0 interrupt\r\n");
    if (pin == GPIO_PIN_0) {
        printf("i:%d\r\n", i++);
    }
    bflb_gpio_int_clear(gpio, pin);
}

int set_int_mode(int argc, char **argv)
{
    printf("Set gpio interrupt triggle mode\r\n");

    if ((argc != 2) || (atoi(argv[1]) > 3)) {
        printf("Usage: set_int_mode <value>\r\n"
               "    0: GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE\r\n"
               "    1: GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE\r\n"
               "    2: GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL\r\n"
               "    3: GPIO_INT_TRIG_MODE_SYNC_HIGH_LEVEL\r\n");
        return 1;
    } else {
        printf("Set gpio interrupt triggle mode to %d\r\n", atoi(argv[1]));
    }

    bflb_irq_disable(gpio->irq_num);
    for (int i = 0; i < GPIO_PIN_MAX; i++) {
        bflb_gpio_int_clear(gpio, i);
    }
    BL_WR_REG(GLB_BASE, GLB_CORE_CFG22, 0xFFFFFFFF);
    BL_WR_REG(GLB_BASE, GLB_CORE_CFG23, 0xFFFFEFFF);
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_reset(gpio, GPIO_PIN_1);
    bflb_gpio_int_init(gpio, GPIO_PIN_0, atoi(argv[1]));
    bflb_gpio_irq_attach(GPIO_PIN_0, gpio0_isr);
    bflb_irq_enable(gpio->irq_num);
    cpu_can_sleep = 1;

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(set_int_mode, set_int_mode, shell set_int_triggle_mode.);