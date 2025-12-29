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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "board.h"
#include "shell.h"

#include "bflb_core.h"
#include "bflb_mtimer.h"
#include "bflb_uart.h"

#include <app_user.h>

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

void app_init_entry(void *param)
{
    app_user_init();
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    xTaskCreate(app_init_entry, "app init", 2048, NULL, 27, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}

