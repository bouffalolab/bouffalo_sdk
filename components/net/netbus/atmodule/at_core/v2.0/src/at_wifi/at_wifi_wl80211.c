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
extern int at_wifi_main_init(void);
#include <FreeRTOS.h>
#include <timers.h>
#include "wl80211.h"
#include "wifi_mgmr.h"
#include "supplicant.h"
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
#ifdef CODE_WIFI_ON_GOT_IP6
    } else if (code == CODE_WIFI_ON_GOT_IP6) {
        return AT_WIFI_EVENT_GOTIP6_GL;
#endif
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

static void wifi_mgmr_event_handler(async_input_event_t ev, void *priv)
{
    /* Application-layer observer of the system async event bus. The WL80211
     * -> EV_WIFI translation, netif/DHCP handling and manager processing are
     * owned by the Wi-Fi layer; the AT module only forwards the published
     * events to the AT core. */
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
    /* Application-layer listener only: EV_WL80211 is owned by the Wi-Fi layer
     * and must not be taken over by upper layers. */
    if (async_register_event_filter(EV_WIFI, wifi_mgmr_event_handler, NULL) != 0) {
        abort();
    }

    at_wifi_main_init();
    return 0;
}
