/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <FreeRTOS.h>
#include "task.h"

#include "bl616_glb.h"
#include <msp_port.h>

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
    // romfs_mount(0x378000);

    /* Init player */
    extern int app_minialsa_init(void);
    app_minialsa_init();

#if CONFIG_CODEC_USE_I2S
    msp_i2s_port_init();
#endif

    vTaskDelete(NULL);
}
