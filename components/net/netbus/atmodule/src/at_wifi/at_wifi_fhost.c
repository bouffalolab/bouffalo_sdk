/**
  ******************************************************************************
  * @file    at_wifi_fhost.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <timers.h>
#include "wifi_mgmr.h"
#include "at_wifi_main.h"

int wifi_mgmr_sta_mac_set(uint8_t mac[6])
{
    if (!mac) {
        return -1;
    }
    memcpy(wifiMgmr.wlan_sta.mac, mac, 6);

    return 0;
}

int wifi_mgmr_ap_mac_set(uint8_t mac[6])
{
    if (!mac) {
        return -1;
    }
    memcpy(wifiMgmr.wlan_ap.mac, mac, 6);
    return 0;
}

static void wifi_background_init(uint8_t *ap_mac, uint8_t *sta_mac, uint8_t country_code)
{
    uint8_t mac[6];

    /* init ap mac address */
    wifi_mgmr_ap_mac_get(mac);
    if (memcmp(mac, ap_mac, 6)) {
        wifi_mgmr_ap_mac_set(ap_mac);
    }

    /* init sta mac address */
    wifi_mgmr_sta_mac_get(mac);
    if (memcmp(mac, sta_mac, 6)) {
        wifi_mgmr_sta_mac_set(sta_mac);
    }
}

static int at_wifi_event_code_get(uint32_t code)
{
    if (code == CODE_WIFI_ON_MGMR_DONE) {
        return AT_WIFI_EVENT_INIT_DONE;
    } else if (code == CODE_WIFI_ON_CONNECTED) {
        return AT_WIFI_EVENT_CONNECTED;
    } else if (code == CODE_WIFI_ON_GOT_IP) {
        return AT_WIFI_EVENT_GOTIP;
    } else if (code == CODE_WIFI_ON_DISCONNECT) {
        return AT_WIFI_EVENT_DISCONNECTED;
    } else if (code == CODE_WIFI_ON_SCAN_DONE) {
        return AT_WIFI_EVENT_SCAN_DONE;
    } else if (code == CODE_WIFI_ON_AP_STARTED) {
        return AT_WIFI_EVENT_AP_STARTED;
    } else if (code == CODE_WIFI_ON_AP_STOPPED) {
        return AT_WIFI_EVENT_AP_STOP;
    } else if (code == CODE_WIFI_ON_AP_STA_ADD) {
        return AT_WIFI_EVENT_AP_STA_ADD;
    } else if (code == CODE_WIFI_ON_AP_STA_DEL) {
        return AT_WIFI_EVENT_AP_STA_DEL;
    }
    return 0;
}

static void _wifi_fhost_event_cb(void *private_data, uint32_t code)
{
    static wifi_conf_t conf = {
        .country_code = "00",
    };

    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            char *country_code_string[WIFI_COUNTRY_CODE_MAX] = AT_WIFI_COUNTRY_CODE;
            strlcpy(wifiMgmr.country_code, country_code_string[at_wifi_config->wifi_country.country_code], sizeof(wifiMgmr.country_code));
            wifi_background_init(   at_wifi_config->ap_mac.addr,
                                    at_wifi_config->sta_mac.addr,
                                    at_wifi_config->wifi_country.country_code);
            wifi_mgmr_init(&conf);
        }
        break;
        default: {
            at_wifi_event_notify(private_data, at_wifi_event_code_get(code));
        }
        break;
    }
}

struct netif *at_wifi_netif_get(uint8_t vif_idx)
{
    return fhost_to_net_if(vif_idx);
}


int wifi_mgmr_sta_scanlist_free(void)
{
    return 0;
}

static void wifi_event_start(uint32_t code)
{
    BaseType_t xReturn;

    xReturn = xTimerPendFunctionCall(_wifi_fhost_event_cb, NULL, code, 0);
    configASSERT(xReturn == pdPASS);
}

void wifi_event_handler(uint32_t code)
{
    wifi_event_start(code);
}

int at_wifi_start(void)
{
    vTaskDelay(100);

    /* Start Wifi_FW */
    wifi_task_create();
    vTaskDelay(500);
    fhost_init();

    return 0;
}

