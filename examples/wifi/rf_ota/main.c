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

#include <stdbool.h>
#include <string.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "wifi_mgmr_ext.h"
#ifndef BL602
#include "fhost_api.h"
#include "wifi_mgmr.h"
#endif

#include "bflb_irq.h"
#include "bflb_mtd.h"
#include "bflb_uart.h"

#include "easyflash.h"
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

#define RF_OTA_WIFI_CONFIG_KEY     "rf_ota/wifi_config"
#define RF_OTA_WIFI_CONFIG_MAGIC   0x52464F54UL
#define RF_OTA_WIFI_CONFIG_VERSION 1UL
#define RF_OTA_WIFI_NOTIFY_READY   (1UL << 0)
#define RF_OTA_WIFI_NOTIFY_CONNECTED (1UL << 1)
#define RF_OTA_WIFI_NOTIFY_FORGET  (1UL << 2)

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct {
    uint32_t magic;
    uint32_t version;
    char ssid[33];
    char password[65];
} rf_ota_wifi_config_t;

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct bflb_device_s *uart0;
static TaskHandle_t wifi_config_task_handle;
static bool wifi_storage_ready;
static bool wifi_autoconnect_enabled;
static bool saved_wifi_config_valid;
static rf_ota_wifi_config_t saved_wifi_config;

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

static bool rf_ota_wifi_config_is_valid(const rf_ota_wifi_config_t *config)
{
    return config->magic == RF_OTA_WIFI_CONFIG_MAGIC &&
           config->version == RF_OTA_WIFI_CONFIG_VERSION &&
           config->ssid[0] != '\0' &&
           strnlen(config->ssid, sizeof(config->ssid)) < sizeof(config->ssid) &&
           strnlen(config->password, sizeof(config->password)) < sizeof(config->password);
}

static void rf_ota_wifi_config_load(void)
{
    rf_ota_wifi_config_t config;
    size_t saved_len = 0;
    size_t read_len;

    memset(&config, 0, sizeof(config));
    read_len = ef_get_env_blob(RF_OTA_WIFI_CONFIG_KEY, &config, sizeof(config), &saved_len);
    if (read_len != sizeof(config) || saved_len != sizeof(config) || !rf_ota_wifi_config_is_valid(&config)) {
        return;
    }

    saved_wifi_config = config;
    saved_wifi_config_valid = true;
}

static int rf_ota_wifi_config_from_connection(rf_ota_wifi_config_t *config)
{
#ifdef BL602
    wifi_mgmr_sta_connect_ind_stat_info_t info;
#else
    wifi_mgmr_connect_ind_stat_info_t info;
#endif
    size_t ssid_len;
    size_t password_len;

    memset(&info, 0, sizeof(info));
#ifdef BL602
    wifi_mgmr_sta_connect_ind_stat_get(&info);
#else
    if (wifi_mgmr_sta_connect_ind_stat_get(&info) != 0) {
        return -1;
    }
#endif

    ssid_len = strnlen(info.ssid, sizeof(info.ssid));
    password_len = strnlen(info.passphr, sizeof(info.passphr));
    if (ssid_len == 0 || ssid_len > sizeof(config->ssid) - 1 ||
        password_len > sizeof(config->password) - 1) {
        return -1;
    }

    memset(config, 0, sizeof(*config));
    config->magic = RF_OTA_WIFI_CONFIG_MAGIC;
    config->version = RF_OTA_WIFI_CONFIG_VERSION;
    memcpy(config->ssid, info.ssid, ssid_len);
    memcpy(config->password, info.passphr, password_len);

    return 0;
}

static void rf_ota_wifi_config_save_current(void)
{
    rf_ota_wifi_config_t config;

    if (!wifi_storage_ready || rf_ota_wifi_config_from_connection(&config) != 0) {
        return;
    }

    if (saved_wifi_config_valid &&
        strcmp(saved_wifi_config.ssid, config.ssid) == 0 &&
        strcmp(saved_wifi_config.password, config.password) == 0) {
        return;
    }

    if (ef_set_env_blob(RF_OTA_WIFI_CONFIG_KEY, &config, sizeof(config)) != EF_NO_ERR) {
        LOG_E("Failed to save WiFi configuration.\r\n");
        return;
    }

    saved_wifi_config = config;
    saved_wifi_config_valid = true;
    LOG_I("WiFi configuration saved.\r\n");
}

static int rf_ota_wifi_connect_saved(void)
{
    char *password = saved_wifi_config.password[0] ? saved_wifi_config.password : NULL;

#ifdef BL602
    wifi_interface_t wifi_interface = wifi_mgmr_sta_enable();

    return wifi_mgmr_sta_connect(wifi_interface, saved_wifi_config.ssid, password, NULL, NULL, 0, 0);
#else
    return wifi_sta_connect(saved_wifi_config.ssid, password, NULL, NULL, 1, 0, 0, 1);
#endif
}

static void rf_ota_wifi_enable_autoconnect(void)
{
    if (wifi_autoconnect_enabled) {
        return;
    }

    if (wifi_mgmr_sta_autoconnect_enable() != 0) {
        LOG_E("Failed to enable WiFi auto reconnect.\r\n");
        return;
    }

    wifi_autoconnect_enabled = true;
}

static void rf_ota_wifi_restore(void)
{
    if (!saved_wifi_config_valid) {
        return;
    }

    rf_ota_wifi_enable_autoconnect();
    LOG_I("Connecting to saved AP: %s\r\n", saved_wifi_config.ssid);
    if (rf_ota_wifi_connect_saved() != 0) {
        LOG_E("Failed to start saved AP connection.\r\n");
    }
}

static void rf_ota_wifi_forget(void)
{
    int disconnect_ret;

    if (!wifi_storage_ready) {
        LOG_E("WiFi configuration storage is unavailable.\r\n");
        return;
    }

    if (wifi_mgmr_sta_autoconnect_disable() != 0) {
        LOG_E("Failed to disable WiFi auto reconnect.\r\n");
        return;
    }

    wifi_autoconnect_enabled = false;
    if (ef_del_env(RF_OTA_WIFI_CONFIG_KEY) != EF_NO_ERR) {
        LOG_E("Failed to delete saved WiFi configuration.\r\n");
        rf_ota_wifi_enable_autoconnect();
        return;
    }

    memset(&saved_wifi_config, 0, sizeof(saved_wifi_config));
    saved_wifi_config_valid = false;

#ifdef BL602
    disconnect_ret = wifi_mgmr_sta_disconnect();
#else
    disconnect_ret = wifi_sta_disconnect();
#endif

    if (disconnect_ret != 0) {
        LOG_E("Saved WiFi configuration deleted, but disconnect failed.\r\n");
        return;
    }

    LOG_I("Saved WiFi configuration deleted and STA disconnected.\r\n");
}

static void rf_ota_wifi_config_task(void *param)
{
    uint32_t notification;

    while (1) {
        xTaskNotifyWait(0, 0xFFFFFFFFUL, &notification, portMAX_DELAY);

        if (notification & RF_OTA_WIFI_NOTIFY_READY) {
            rf_ota_wifi_restore();
        }

        if (notification & RF_OTA_WIFI_NOTIFY_CONNECTED) {
            rf_ota_wifi_enable_autoconnect();
            rf_ota_wifi_config_save_current();
        }

        if (notification & RF_OTA_WIFI_NOTIFY_FORGET) {
            rf_ota_wifi_forget();
        }
    }
}

static void rf_ota_wifi_config_notify(uint32_t notification)
{
    xTaskNotify(wifi_config_task_handle, notification, eSetBits);
}

static void rf_ota_wifi_forget_cmd(int argc, char **argv)
{
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    rf_ota_wifi_config_notify(RF_OTA_WIFI_NOTIFY_FORGET);
}

SHELL_CMD_EXPORT_ALIAS(rf_ota_wifi_forget_cmd, wifi_sta_forget, forget saved AP and disconnect STA);

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
            rf_ota_wifi_config_notify(RF_OTA_WIFI_NOTIFY_READY);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump();
            mm_sec_keydump();
            rf_ota_wifi_config_notify(RF_OTA_WIFI_NOTIFY_CONNECTED);
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
        case CODE_WIFI_ON_AP_CSA_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_CSA_DONE, channel %u\r\n",
                  __func__, (unsigned int)ev->value);
        } break;
        case CODE_WIFI_ON_STA_CSA_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_STA_CSA_DONE, channel %u\r\n",
                  __func__, (unsigned int)ev->value);
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

    bflb_mtd_init();
    if (easyflash_init() == EF_NO_ERR) {
        wifi_storage_ready = true;
        rf_ota_wifi_config_load();
    } else {
        LOG_E("WiFi configuration storage init failed.\r\n");
    }

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    LOG_I("PHY RF init success!\r\n");

    tcpip_init(NULL, NULL);

    if (xTaskCreate(rf_ota_wifi_config_task, "wifi config", 1024, NULL, 9, &wifi_config_task_handle) != pdPASS) {
        LOG_E("Failed to create WiFi config task.\r\n");
        return 0;
    }

    xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
