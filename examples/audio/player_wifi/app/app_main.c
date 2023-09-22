/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <FreeRTOS.h>
#include "task.h"

#include <app_wifi.h>
#include <app_player.h>

#include "rfparam_adapter.h"
#include "bl616_glb.h"

void vAssertCalled(void)
{
    printf("vAssertCalled\r\n");
    __disable_irq();

    while (1);
}

void app_main_entry(void *arg)
{
    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }

    /* Init wifi */
    app_wifi_init();

    /* Init player */
    app_player_init();

    vTaskDelete(NULL);
}

