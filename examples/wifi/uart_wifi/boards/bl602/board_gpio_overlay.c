#include "board_gpio_overlay.h"
#include "bflb_gpio.h"
#include "bl602_glb.h"

void board_uartx_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_1, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_12, GPIO_UART_FUNC_UART1_RX);
}
