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

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "bflb_uart.h"

#include "bl_sys.h"
#include "board.h"

#if IS_ENABLED(CONFIG_SHELL)
#include "shell.h"
#endif

#if IS_ENABLED(CONFIG_CHERRYUSB_HOST)
#include "usbh_core.h"
#endif

#include "solution.h"

#define DBG_TAG "MAIN"
#include "log.h"

static void main_init_task(void *param)
{
    (void)param;

#if IS_ENABLED(CONFIG_SHELL)
    extern void shell_init_with_task(struct bflb_device_s * shell);
    /* shell init */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    printf("\r\nShell ready\r\n");
#endif

#if IS_ENABLED(CONFIG_WIFI6)
    /* wifi init */
    extern void wifi_start_firmware_task(void *param);
    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);
    vTaskDelay(200); /* Wait for network ready */
#endif

#if IS_ENABLED(CONFIG_CHERRYUSB_HOST)
    /* USB host init */
    printf("Starting usb host task...\r\n");
    struct bflb_device_s *usb_dev = bflb_device_get_by_name("usb_v2");
    usbh_initialize(0, usb_dev->reg_base);
#endif

    /* solution util init */
    solution_init();

#if IS_ENABLED(CONFIG_SOLUTION_FUNC_HIBOOSTER_RX)
    /* hibooster rx start */
    int hb_recv_init(uint16_t local_port, uint8_t peer_ip0, uint8_t peer_ip1, uint8_t peer_ip2, uint8_t peer_ip3, uint16_t peer_port);
    extern int hb_recv_start(void);
    hb_recv_init(9000, 192, 168, 169, 1, 8800);
    hb_recv_start();
#endif

    LOG_D("main init done\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    /* board init */
    board_init();

#if IS_ENABLED(BL616) || IS_ENABLED(BL616L)
    /* Improve CPU performance */
    bl_cpu_sysmap_init(true, true);
#endif

    xTaskCreate(main_init_task, (char *)"main_init", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();

    LOG_E("Scheduler start failed!\r\n");
    return -1;
}
