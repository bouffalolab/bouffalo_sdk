/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdlib.h>
#include "async_event.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "wifi_mgmr_ext.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "rfparam_adapter.h"

#include "app_uartwifi.h"
#include "app_pm.h"

#include "board.h"
#include "shell.h"

#define INIT_STACK_SIZE    (2048)
#define TASK_PRIORITY_INIT (16)

static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);
extern void wifi_event_handler(async_input_event_t ev, void *priv);

void app_init_entry(void *param)
{
    if (app_uartwifi_init() != 0) {
        abort();
    }

    vTaskDelete(NULL);
}

int main(void)
{
    if (async_register_event_filter(EV_WIFI, wifi_event_handler, NULL) != 0) {
        abort();
    }

    if (xTaskCreate(app_init_entry, (char *)"init", INIT_STACK_SIZE, NULL, TASK_PRIORITY_INIT, NULL) != pdPASS) {
        abort();
    }

#ifdef CONFIG_LPAPP
    app_pm_init();
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    board_uartx_gpio_init();
    vTaskStartScheduler();

    while (1) {
    }
}

