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

/* clang-format off */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "bl_fw_api.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "rfparam_adapter.h"

#include "board.h"
#include "shell.h"

#include "wl80211.h"
#include "async_event.h"

#include <lwip/etharp.h>
#include <lwip/netdb.h>
#include <lwip/netifapi.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
/* clang-format on */

#define DBG_TAG "MAIN"
#include "log.h"

/* for get mac address */
#if defined(BL616)
#include <bl616_mfg_media.h>
#elif defined(BL616L)
#include <bl616l_mfg_media.h>
#endif

#include "wifi_mgmr.h"

struct bflb_device_s *gpio;

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/
void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    wifi_task_create();

    tcpip_init(NULL, NULL);

    wl80211_init();

    wifi_mgmr_init();

    extern uint8_t _heap_wifi_size[];
    printf("heap_wifi_size: %p\r\n", _heap_wifi_size);

    vTaskDelete(NULL);
}

/* async event handler */
static void async_event_handler(void *arg1, uint32_t arg2)
{
    vTaskSuspendAll();
    async_event_loop();
    xTaskResumeAll();
}

static void async_event_loop_wake(void)
{
    BaseType_t xReturn;
    TickType_t wait = portMAX_DELAY;

    if (xTimerGetTimerDaemonTaskHandle() == xTaskGetCurrentTaskHandle()) {
        wait = 0;
    }

    xReturn = xTimerPendFunctionCall(async_event_handler, (void *)NULL, NULL, wait);
    configASSERT(xReturn == pdPASS);
}

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    async_event_init(async_event_loop_wake);

    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    vTaskStartScheduler();

    while (1) {}
}

int macsw_scan_cmd(int argc, char **argv)
{
    if (wl80211_scan(NULL)) {
        printf("wl80211_scan failed\n");
    }
    return 0;
}

int macsw_connect_cmd(int argc, char **argv)
{
    const char *ssid = "zrrong", *passwd = "12345678";
    switch (argc) {
        case 3:
            passwd = argv[2];
        case 2:
            ssid = argv[1];
            break;
        default:
            break;
    }

    printf("connect wifi: %s/%s\n", ssid, passwd);
    struct wl80211_connect_params param;
    memset(&param, 0, sizeof(param));

    strcpy((char *)param.password, passwd);

    memcpy(param.ssid, ssid, 32);
    //memcpy(param.bssid, "\x64\x64\x4a\x82\x73\x74", 6);
    //param.channel = 11;

    if (wl80211_sta_connect(&param)) {
        printf("start scan failed\n");
    }
    return 0;
}

int macsw_start_ap_cmd(int argc, char **argv)
{
    const char *ssid = "BL616-AP";
    const char *passwd = NULL;
    struct wl80211_ap_settings ap_setting;

    if (argc >= 2) {
        ssid = argv[1];
    }
    if (argc >= 3) {
        passwd = argv[2];
    }

    memset(&ap_setting, 0, sizeof(ap_setting));
    strncpy((char *)ap_setting.ssid, ssid, 32);
    if (passwd != NULL) {
        strncpy((char *)ap_setting.password, passwd, 64);
    }

    ap_setting.auth_type = WL80211_AUTHTYPE_OPEN_SYSTEM;
    ap_setting.beacon_interval = 100;
    ap_setting.center_freq1 = wl80211_channel_to_freq(11);
    ap_setting.channel_width = WL80211_CHAN_WIDTH_20;
    ap_setting.max_power = 0x14;

    printf("start ap: %s, password: %s\n", ssid, (passwd == NULL ? "null" : passwd));

    if (wl80211_ap_start(&ap_setting)) {
        printf("ap failed to start!\n");
    }
    return 0;
}

int macsw_stop_ap_cmd(int argc, char **argv)
{
    if (wl80211_ap_stop()) {
        printf("ap failed to stop!\n");
    }
    return 0;
}

static void wifi_scan_cmd(int argc, char **argv)
{
    if (wl80211_scan(NULL)) {
        printf("wl80211_scan failed\n");
    }
    return;
}

static void wl80211_monitor_rx(void *ctx, void *pkt, size_t len, size_t mac_hdr_len, int rssi) {
    printf("%s:%d\n", __func__, __LINE__);
}

static void wifi_monitor_start_cmd(int argc, char **argv)
{
    struct wl80211_monitor_settings mon_settings;
    mon_settings.channel_width = WL80211_CHAN_WIDTH_20;
    mon_settings.center_freq1 = 2412;
    mon_settings.recv = wl80211_monitor_rx;

    if (wl80211_monitor_start(&mon_settings)) {
        printf("wl80211 monitor start failed\n");
    }
    return;
}

static void wifi_monitor_stop_cmd(int argc, char **argv)
{
    if (wl80211_monitor_stop()) {
        printf("wl80211 monitor stop failed\n");
    }
    return;
}

SHELL_CMD_EXPORT_ALIAS(macsw_connect_cmd, macsw_connect, macsw scan.);
SHELL_CMD_EXPORT_ALIAS(macsw_scan_cmd, macsw_scan, macsw scan.);
SHELL_CMD_EXPORT_ALIAS(macsw_start_ap_cmd, macsw_start_ap, macsw start ap.);
SHELL_CMD_EXPORT_ALIAS(macsw_stop_ap_cmd, macsw_stop_ap, macsw stop ap.);

SHELL_CMD_EXPORT_ALIAS(wifi_monitor_start_cmd, monitor_start, monitor start.);
SHELL_CMD_EXPORT_ALIAS(wifi_monitor_stop_cmd, monitor_stop, monitor stop.);

SHELL_CMD_EXPORT_ALIAS(wifi_scan_cmd, wifi_scan, wifi scan);
