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
#include <lwip/tcpip.h>
#include "wifi_mgmr_ext.h"
#include "bflb_uart.h"
#include "rfparam_adapter.h"
#include "board.h"
#include "shell.h"
#include "bflb_mtd.h"
#include "easyflash.h"
#include "web_config.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define AP_SSID    "BL602_Config"
#define AP_PASSWD  NULL
#define AP_CHANNEL 1

static wifi_conf_t conf = { .country_code = "CN" };
static struct bflb_device_s *uart0;
static web_config_t g_web_ctx;

extern void shell_init_with_task(struct bflb_device_s *shell);

static void wifi_init_task(void *param)
{
    wifi_task_create();
    fhost_init();
    vTaskDelete(NULL);
}

static void ap_start_task(void *param)
{
    vTaskDelay(pdMS_TO_TICKS(200));
    wifi_mgmr_sta_enable();
    wifi_interface_t wifi_if = wifi_mgmr_ap_enable();
    wifi_mgmr_ap_start(wifi_if, AP_SSID, 0, AP_PASSWD, AP_CHANNEL);
    vTaskDelete(NULL);
}

static void web_init_task(void *param)
{
    web_config_do_scan();
    vTaskDelay(pdMS_TO_TICKS(4500));
    web_config_init(&g_web_ctx);
    LOG_I("Visit http://192.168.169.1\r\n");
    vTaskDelete(NULL);
}

void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE:
            wifi_mgmr_init(&conf);
            break;
        case CODE_WIFI_ON_MGMR_DONE:
            xTaskCreate(ap_start_task, "ap", 1024, NULL, 15, NULL);
            break;
        case CODE_WIFI_ON_GOT_IP:
            web_config_notify_result(1);
            break;
        case CODE_WIFI_ON_AP_STARTED:
            xTaskCreate(web_init_task, "web", 512, NULL, 14, NULL);
            break;
        default:
            break;
    }
}

int main(void)
{
    board_init();
    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);
    bflb_mtd_init();
    easyflash_init();
    
    if (rfparam_init(0, NULL, 0) != 0) {
        LOG_E("RF init failed\r\n");
        return 0;
    }
    
    LOG_I("SoftAP Config: %s\r\n", AP_SSID);
    tcpip_init(NULL, NULL);
    xTaskCreate(wifi_init_task, "wifi", 1024, NULL, 10, NULL);
    vTaskStartScheduler();
    while (1) {}
}
