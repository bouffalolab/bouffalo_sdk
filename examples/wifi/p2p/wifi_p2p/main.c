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

#include <stdarg.h>
#include <stdio.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "wifi_mgmr_ext.h"
#include "fhost_api.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "rfparam_adapter.h"
#include "async_event.h"
#include "mm.h"
#include "board.h"
#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

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
extern void wifi_event_handler(async_input_event_t ev, void *priv);
#ifdef BL602
extern void wifi_task_create(void);
extern int fhost_init(void);
extern int wifi_mgmr_task_start(void);
#endif

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Functions
 ****************************************************************************/

void wifi_start_firmware_task(void *param)
{
    LOG_I("Starting wifi ...\r\n");

    async_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    wifi_task_create();

    LOG_I("Starting fhost ...\r\n");
#ifndef BL602
    fhost_init();
#endif

    vTaskDelete(NULL);
}

static const wifi_mgmr_p2p_event_t *wifi_p2p_event_detail(async_input_event_t ev)
{
    if (!ev || ev->size < sizeof(wifi_mgmr_p2p_event_t)) {
        return NULL;
    }

    return (const wifi_mgmr_p2p_event_t *)ev;
}

static void wifi_p2p_event_append(char *buf, size_t buf_len, size_t *used,
                                  const char *format, ...)
{
    va_list args;
    int len;

    if (!buf || !used || !format || *used >= buf_len) {
        return;
    }

    va_start(args, format);
    len = vsnprintf(buf + *used, buf_len - *used, format, args);
    va_end(args);
    if (len < 0) {
        return;
    }
    if ((size_t)len >= buf_len - *used) {
        *used = buf_len - 1;
    } else {
        *used += (size_t)len;
    }
}

static void wifi_p2p_event_log(const char *name, async_input_event_t ev)
{
    const wifi_mgmr_p2p_event_t *detail = wifi_p2p_event_detail(ev);
    char fields[512];
    size_t used = 0;

    if (!detail) {
        LOG_I("[APP] [EVT] [P2P] %s vif=%lu\r\n", name, ev->value);
        return;
    }

    fields[0] = '\0';
    wifi_p2p_event_append(fields, sizeof(fields), &used, " vif=%lu",
                          ev->value);
    if (detail->peer_addr[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " peer=%s",
                              detail->peer_addr);
    }
    if (detail->peer_iface_addr[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " peer_if=%s",
                              detail->peer_iface_addr);
    }
    if (detail->ifname[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " if=%s",
                              detail->ifname);
    }
    if (detail->role_name[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " role=%s",
                              detail->role_name);
    }
    if (detail->ssid[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " ssid=\"%s\"",
                              detail->ssid);
    }
    if (detail->has_freq) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " freq=%u",
                              detail->freq);
    }
    if (detail->has_status) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " status=%d",
                              detail->status);
    }
    if (detail->has_persistent_id) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " persistent_id=%d", detail->persistent_id);
    }
    if (detail->has_persistent && detail->persistent) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " [PERSISTENT]");
    }
    if (detail->go_dev_addr[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " go_dev_addr=%s",
                              detail->go_dev_addr);
    }
    if (detail->bssid[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " bssid=%s",
                              detail->bssid);
    }
    if (detail->device_name[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " name=\"%s\"",
                              detail->device_name);
    }
    if (detail->pin[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " pin=%s",
                              detail->pin);
    }
    if (detail->reason[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " reason=\"%s\"", detail->reason);
    }
    if (detail->wps_method[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " wps_method=%s",
                              detail->wps_method);
    }
    if (detail->has_config_methods) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " config_methods=0x%x",
                              detail->config_methods);
    }
    if (detail->has_dev_capab) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " dev_capab=0x%x", detail->dev_capab);
    }
    if (detail->has_group_capab) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " group_capab=0x%x", detail->group_capab);
    }
    if (detail->has_level) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " level=%d",
                              detail->level);
    }
    if (detail->has_go_intent) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " go_intent=%u",
                              detail->go_intent);
    }
    if (detail->has_dev_passwd_id) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " dev_pw_id=%u",
                              detail->dev_passwd_id);
    }
    if (detail->raw[0]) {
        wifi_p2p_event_append(fields, sizeof(fields), &used, " raw=\"%s\"",
                              detail->raw);
    }
    if (detail->raw_truncated) {
        wifi_p2p_event_append(fields, sizeof(fields), &used,
                              " raw_truncated=1");
    }

    LOG_I("[APP] [EVT] [P2P] %s%s\r\n", name, fields);
}

void wifi_event_handler(async_input_event_t ev, void *priv)
{
    uint32_t code = ev->code;

    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_task_start();
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_SCAN_DONE_CONNECTING: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE_CONNECTING\r\n", __func__);
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            LOG_I("[SYS] Memory left is %d Bytes\r\n", kfree_size(0));
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
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
        case CODE_WIFI_ON_P2P_DEVICE_FOUND: {
            wifi_p2p_event_log("DEVICE_FOUND", ev);
        } break;
        case CODE_WIFI_ON_P2P_DEVICE_LOST: {
            wifi_p2p_event_log("DEVICE_LOST", ev);
        } break;
        case CODE_WIFI_ON_P2P_FIND_STOPPED: {
            wifi_p2p_event_log("FIND_STOPPED", ev);
        } break;
        case CODE_WIFI_ON_P2P_GO_NEG_REQUEST: {
            /* Application policy hook: inspect the typed event, then call
             * wifi_mgmr_p2p_connect(auth=1) only after explicit approval. */
            wifi_p2p_event_log("GO_NEG_REQUEST", ev);
        } break;
        case CODE_WIFI_ON_P2P_GO_NEG_SUCCESS: {
            wifi_p2p_event_log("GO_NEG_SUCCESS", ev);
        } break;
        case CODE_WIFI_ON_P2P_GO_NEG_FAILURE: {
            wifi_p2p_event_log("GO_NEG_FAILURE", ev);
        } break;
        case CODE_WIFI_ON_P2P_GROUP_FORMATION_SUCCESS: {
            wifi_p2p_event_log("GROUP_FORMATION_SUCCESS", ev);
        } break;
        case CODE_WIFI_ON_P2P_GROUP_FORMATION_FAILURE: {
            wifi_p2p_event_log("GROUP_FORMATION_FAILURE", ev);
        } break;
        case CODE_WIFI_ON_P2P_GROUP_STARTED: {
            wifi_p2p_event_log("GROUP_STARTED", ev);
        } break;
        case CODE_WIFI_ON_P2P_GROUP_REMOVED: {
            wifi_p2p_event_log("GROUP_REMOVED", ev);
        } break;
        case CODE_WIFI_ON_P2P_GOT_IP: {
            LOG_I("[APP] [EVT] [P2P] GOT_IP vif=%lu\r\n", ev->value);
        } break;
        case CODE_WIFI_ON_P2P_PROV_DISC_SHOW_PIN: {
            wifi_p2p_event_log("PROV_DISC_SHOW_PIN", ev);
        } break;
        case CODE_WIFI_ON_P2P_PROV_DISC_ENTER_PIN: {
            wifi_p2p_event_log("PROV_DISC_ENTER_PIN", ev);
        } break;
        case CODE_WIFI_ON_P2P_PROV_DISC_PBC_REQ: {
            wifi_p2p_event_log("PROV_DISC_PBC_REQ", ev);
        } break;
        case CODE_WIFI_ON_P2P_PROV_DISC_PBC_RESP: {
            wifi_p2p_event_log("PROV_DISC_PBC_RESP", ev);
        } break;
        case CODE_WIFI_ON_P2P_PROV_DISC_FAILURE: {
            wifi_p2p_event_log("PROV_DISC_FAILURE", ev);
        } break;
        case CODE_WIFI_ON_P2P_INVITATION_RECEIVED: {
            wifi_p2p_event_log("INVITATION_RECEIVED", ev);
        } break;
        case CODE_WIFI_ON_P2P_INVITATION_ACCEPTED: {
            wifi_p2p_event_log("INVITATION_ACCEPTED", ev);
        } break;
        case CODE_WIFI_ON_P2P_INVITATION_RESULT: {
            wifi_p2p_event_log("INVITATION_RESULT", ev);
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
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

    tcpip_init(NULL, NULL);

    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
