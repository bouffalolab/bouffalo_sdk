#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <shell.h>
#include <netbus_tty.h>
#include "at_main.h"
#include "bflb_dma.h"

void app_atmodule_init(void)
{
    at_module_init();
}

