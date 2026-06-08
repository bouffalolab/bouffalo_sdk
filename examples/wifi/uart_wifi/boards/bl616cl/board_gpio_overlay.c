#include "board_gpio.h"
#include "bflb_gpio.h"
#if defined(CONFIG_LPAPP) && CONFIG_LPAPP
#include <bl616cl_pm.h>
#endif

void board_uartx_gpio_init(void)
{
    struct bflb_device_s *gpio;
#if defined(CONFIG_LPAPP) && CONFIG_LPAPP
    pm_disable_gpio_keep(GPIO_PIN_14);
    pm_disable_gpio_keep(GPIO_PIN_15);
#endif
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_14, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_15, GPIO_UART_FUNC_UART1_RX);
}
