/**
  ******************************************************************************
  * @file    at_wifi_wl80211.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "at_wifi_main.h"
#include <FreeRTOS.h>
#include <timers.h>
#include "wl80211.h"
#include "wifi_mgmr.h"
#include "supplicant_api.h"
#include "bl_wpa.h"
#include "async_event.h"
#include <lwip/etharp.h>
#include <lwip/mem.h>
#include <lwip/netifapi.h>
#include <lwip/pbuf.h>
#include <lwip/tcpip.h>
#include "lwip/dns.h"

extern struct netif vif2netif[WL80211_VIF_MAX];

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

static void _wifi_event(void *private_data, uint32_t code)
{
    at_wifi_event_notify(private_data, code);
}

static void wifi_event_start(uint32_t code)
{
    BaseType_t xReturn;

    xReturn = xTimerPendFunctionCall(_wifi_event, NULL, code, 0);
    configASSERT(xReturn == pdPASS);
}

static void evt_handler_wrapper(void (*handler)(void))
{
    assert(handler != NULL);
    handler();

    vTaskDelete(NULL);
}

static void start_dhcp_tsk(void)
{
    extern void _wifi_mgmr_sta_start_dhcpc(void);

    if (at_wifi_config->dhcp_state.bit.sta_dhcp) {
        _wifi_mgmr_sta_start_dhcpc();
    } else {
        //_wifi_mgmr_sta_start_dhcpc();// fixme
    }
}

static void disconnect_tsk(void)
{
#if 0
    _wifi_mgmr_sta_stop_dhcpc();
    if (!at_wifi_config->switch_mode_auto_conn && wl80211_glb.last_connect_params) {
        wl80211_printf("Autoconnect: Reconnecting to %s\r\n", wl80211_glb.last_connect_params->ssid);
        wl80211_sta_connect(wl80211_glb.last_connect_params);
    }
#endif
}

static void start_dhcpd_tsk(void)
{
    extern void _wifi_mgmr_ap_start_dhcpd(void);
    _wifi_mgmr_ap_start_dhcpd();
}

static void stop_dhcpd_tsk(void)
{
    void _wifi_mgmr_ap_stop_dhcpd(void);
    _wifi_mgmr_ap_stop_dhcpd();
}

static void wl80211_event_handler(async_input_event_t ev, void *priv)
{
    void (*handler)(void) = NULL;
    uint16_t stack_size = 265;

    switch (ev->code) {
        case WL80211_EVT_STA_CONNECTED:
            async_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTED, 0);
            handler = start_dhcp_tsk;
            break;

        case WL80211_EVT_SCAN_DONE:
            async_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, 0);
            //handler = dump_scan_result;
            break;

        case WL80211_EVT_STA_DISCONNECTED:
            //connect_ind_dump((ev->value >> 16) & 0xFF, ev->value & 0xFF);
            async_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, 0);
            handler = disconnect_tsk;
            break;

        case WL80211_EVT_AP_STARTED:
            async_post_event(EV_WIFI, CODE_WIFI_ON_AP_STARTED, 0);
            handler = start_dhcpd_tsk;
            break;

        case WL80211_EVT_AP_STOPPED:
            async_post_event(EV_WIFI, CODE_WIFI_ON_AP_STOPPED, 0);
            handler = stop_dhcpd_tsk;
            break;
    }

    if (handler != NULL) {
        xTaskCreate((void *)evt_handler_wrapper, "evt", stack_size, handler, 20, NULL);
    }
}

static void wifi_mgmr_event_handler(async_input_event_t ev, void *priv)
{
    wifi_event_start(at_wifi_event_code_get(ev->code));
}

struct netif *at_wifi_netif_get(uint8_t vif_idx)
{
    if (vif_idx == AT_WIFI_VIF_STA) {
        return &(vif2netif[WL80211_VIF_STA]);
    } else {
        return &(vif2netif[WL80211_VIF_AP]);
    }
}

int at_wifi_start(void)
{
    async_register_event_filter(EV_WL80211, wl80211_event_handler, NULL);
    async_register_event_filter(EV_WIFI, wifi_mgmr_event_handler, NULL);

    at_wifi_main_init();
    return 0;
}

