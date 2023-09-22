/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <FreeRTOS.h>
#include "task.h"

#include <app_player.h>
#include "bl616_glb.h"

void vAssertCalled(void)
{
    printf("vAssertCalled\r\n");
    __disable_irq();

    while (1);
}

void wifi_event_handler(uint32_t code)
{
}

void app_main_entry(void *arg)
{
    /* romsfs init mount use media factory*/
    romfs_mount(0x378000);

    /* Init player */
    app_player_init();

    extern int app_play_local_music();
    app_play_local_music();

    vTaskDelete(NULL);
}

