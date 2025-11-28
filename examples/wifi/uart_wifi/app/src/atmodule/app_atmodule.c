#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <shell.h>
#include <netbus_uart.h>
#include "at_main.h"
#include "bflb_dma.h"

#ifdef CONFIG_ATMODULE_UART
const netbus_uart_config_t uart_config = {
    .name = "uart1",
    .speed = 2000000,
    .databits = 8,
    .stopbits = 1,
    .parity = 0,
    .flow_control =0,
};
netbus_uart_ctx_t at_uart;
#endif

#ifdef CONFIG_ATMODULE_TTY
netbus_tty_ctx_t at_tty;
#endif

void app_atmodule_init(void)
{
#ifdef CONFIG_ATMODULE_UART
    netbus_uart_init(&at_uart, &uart_config, 1024, 1024);
#endif
#ifdef CONFIG_ATMODULE_TTY
    netbus_tty_init(&at_tty, 1024, 1024);
#endif

    at_module_init();
}

