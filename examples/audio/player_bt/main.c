/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#define APP_MAIN_STACK_SIZE          (4*1024)
#define APP_MAIN_TASK_PRIORITY       (1)

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);
extern void app_main_entry(void *arg);

int main(void)
{
    TaskHandle_t xHandle;
    board_init();

    // configASSERT((configMAX_PRIORITIES > 4));

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    xTaskCreate(app_main_entry, (char *)"app_main", APP_MAIN_STACK_SIZE, NULL, APP_MAIN_TASK_PRIORITY, &xHandle);

    vTaskStartScheduler();

    while (1) {
    }
}

