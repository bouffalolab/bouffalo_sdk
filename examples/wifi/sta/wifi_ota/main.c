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

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_flash.h"

#include "bl616_glb.h"
#include "rfparam_adapter.h"

#include "board.h"
#include "shell.h"
#include "partition.h"

#include "bflb_wdg.h"
#include "ring_buffer.h"

#define DBG_TAG "MAIN"
#include "log.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

#define AUTO_CONNECT_WIFI (1)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct bflb_device_s *gpio;
static struct bflb_device_s *uart0;

static TaskHandle_t wifi_fw_task;
#if AUTO_CONNECT_WIFI
struct bflb_device_s *wdg;
static TaskHandle_t ota_task_handle;
#endif
static wifi_conf_t conf = {
    .country_code = "CN",
};

#if AUTO_CONNECT_WIFI
// clang-format off
/* config your wifi ssid and password */
static const uint8_t wifi_sta_connet[] = "wifi_sta_connect H3C_21A0AD\r";
/* config your ota server and port */
static const uint8_t wifi_ota_test[] = "wifi_ota_test 192.168.123.120 3365\r";
/* reboot command */
// static const uint8_t send_buf1[] = "reboot\r";
// clang-format on

extern Ring_Buffer_Type shell_rb;
extern void shell_release_sem(void);

SemaphoreHandle_t sem_wifi_init_done;
SemaphoreHandle_t sem_wifi_connect_done;
SemaphoreHandle_t sem_wifi_disconnect;
#endif

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

int wifi_start_firmware_task(void)
{
    LOG_I("Starting wifi ...\r\n");

    /* enable wifi clock */

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* Enable wifi irq */

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *)"fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_fw_task);

    return 0;
}

void wifi_event_handler(uint32_t code)
{
    uint32_t ret = 0;
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
#if AUTO_CONNECT_WIFI
            ret = xSemaphoreGiveFromISR(sem_wifi_init_done, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
#endif
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
#if AUTO_CONNECT_WIFI
            ret = xSemaphoreGiveFromISR(sem_wifi_connect_done, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
#endif
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
#if AUTO_CONNECT_WIFI
            ret = xSemaphoreGiveFromISR(sem_wifi_disconnect, &xHigherPriorityTaskWoken);
            if (ret == pdPASS) {
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
#endif
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

#if AUTO_CONNECT_WIFI
void ota_task(void *param)
{
    if (xSemaphoreTake(sem_wifi_init_done, portMAX_DELAY) == pdTRUE) {
        Ring_Buffer_Write(&shell_rb, wifi_sta_connet, sizeof(wifi_sta_connet) - 1);
        shell_release_sem();
    }

    if (xSemaphoreTake(sem_wifi_connect_done, portMAX_DELAY) == pdTRUE) {
        Ring_Buffer_Write(&shell_rb, wifi_ota_test, sizeof(wifi_ota_test) - 1);
        shell_release_sem();
    }

    while (1) {
        if (xSemaphoreTake(sem_wifi_disconnect, portMAX_DELAY) == pdTRUE) {
            Ring_Buffer_Write(&shell_rb, wifi_sta_connet, sizeof(wifi_sta_connet) - 1);
            shell_release_sem();
        }
        vTaskDelay(10000);
    }
}
#endif

int main(void)
{
    board_init();

#if AUTO_CONNECT_WIFI
    sem_wifi_init_done = xSemaphoreCreateBinary();
    sem_wifi_connect_done = xSemaphoreCreateBinary();
    sem_wifi_disconnect = xSemaphoreCreateBinary();
#endif

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    tcpip_init(NULL, NULL);
    wifi_start_firmware_task();

#if AUTO_CONNECT_WIFI
    printf("Config Watchdog...\r\n");
    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = 31;
    wdg_cfg.comp_val = 30000;
    wdg_cfg.mode = WDG_MODE_RESET;

    wdg = bflb_device_get_by_name("watchdog");
    bflb_wdg_init(wdg, &wdg_cfg);

    bflb_wdg_start(wdg);
    bflb_wdg_reset_countervalue(wdg);
    printf("Next delay 30s, wdg will reset it.\r\n");

    xTaskCreate(ota_task, "ota_task", 2048, NULL, 15, &ota_task_handle);
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
