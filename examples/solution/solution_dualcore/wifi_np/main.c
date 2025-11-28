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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "board.h"
#include "shell.h"

#include <app_init.h>

#define DBG_TAG "MAIN"
#include "log.h"

#define CNFIG_WIFI_ENABLE 1

/****************************************************************************
 * Private Types
 ****************************************************************************/

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static struct bflb_device_s *uart1;

void app_wifi_entry(void *param)
{
    printf("%s entry.\r\n", __FUNCTION__);

    app_init();

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    uart1 = bflb_device_get_by_name("uart1");
    shell_init_with_task(uart1);

    xTaskCreate(app_wifi_entry, (char *)"init", 2048, NULL, 16, NULL);

    vTaskStartScheduler();
    while (1) {
        // never here.
    }
}
