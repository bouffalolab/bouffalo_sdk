#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <shell.h>
#include <netbus_tty.h>
#include "at_main.h"
#include "bflb_dma.h"

netbus_tty_ctx_t at_tty;

void app_atmodule_init(void)
{
    netbus_tty_init(&at_tty, 1024, 1024);

    at_module_init();
}

