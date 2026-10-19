#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "shell.h"
#include "bflb_uart.h"

struct bflb_device_s *gpio;
struct bflb_device_s *uart0;
static uint8_t gpio_int_pin = GPIO_PIN_0;

void gpio0_isr(uint8_t pin)
{
    if (pin == gpio_int_pin) {
        printf("Interrupt Trigger!\r\n");
    }
}

int gpio_int_test(int argc, char **argv)
{
    printf("Set gpio interrupt triggle mode\r\n");

    if ((argc != 3) || (atoi(argv[1]) >= GPIO_PIN_MAX) || (atoi(argv[2]) > 3)) {
        printf("Usage: gpio_int_test <gpio> <mode>\r\n"
               "    0: GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE\r\n"
               "    1: GPIO_INT_TRIG_MODE_SYNC_RISING_EDGE\r\n"
               "    2: GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL\r\n"
               "    3: GPIO_INT_TRIG_MODE_SYNC_HIGH_LEVEL\r\n");
        return 1;
    }

    printf("Set gpio%d interrupt triggle mode to %d\r\n", atoi(argv[1]), atoi(argv[2]));

    bflb_irq_disable(gpio->irq_num);
    gpio_int_pin = atoi(argv[1]);
    bflb_gpio_init(gpio, atoi(argv[1]), GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_int_init(gpio, atoi(argv[1]), atoi(argv[2]));
    bflb_gpio_irq_attach(atoi(argv[1]), gpio0_isr);
    bflb_irq_enable(gpio->irq_num);

    return 0;
}

int main(void)
{
    int ch;
    board_init();

    gpio = bflb_device_get_by_name("gpio");
    uart0 = bflb_device_get_by_name("uart0");
    printf("gpio interrupt\r\n");

    shell_init();

    while (1) {
        while ((ch = bflb_uart_getchar(uart0)) != -1) {
            shell_handler(ch);
        }
    }
}
SHELL_CMD_EXPORT_ALIAS(gpio_int_test, gpio_int_test, shell gpio_int_test.);
