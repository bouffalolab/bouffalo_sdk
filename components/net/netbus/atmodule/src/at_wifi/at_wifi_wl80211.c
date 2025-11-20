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

#include <lwip/etharp.h>
#include <lwip/mem.h>
#include <lwip/netifapi.h>
#include <lwip/pbuf.h>
#include <lwip/tcpip.h>
#include "lwip/dns.h"

extern struct netif eth_wl80211_netif;

static void ip_got_cb(struct netif *netif) {
  char *state, *connected;

  if (wifi_mgmr_sta_state_get()) {
    state = "UP";
    connected = ",CONNECTED";
    at_wifi_event_notify(NULL, AT_WIFI_EVENT_GOTIP);
  } else {
    state = "DOWN";
    connected = "";
  }

  printf("Memory left is %d Bytes\r\n", kfree_size());
  printf(
      "[%d]  %c%c: MAC=%02x:%02x:%02x:%02x:%02x:%02x ip=%d.%d.%d.%d/%d %s%s\n",
      netif->num, netif->name[0], netif->name[1], netif->hwaddr[0],
      netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4],
      netif->hwaddr[5], netif->ip_addr.addr & 0xff,
      (netif->ip_addr.addr >> 8) & 0xff, (netif->ip_addr.addr >> 16) & 0xff,
      (netif->ip_addr.addr >> 24) & 0xff,
      32 - __builtin_clz(netif->netmask.addr), state, connected);
}

static void start_dhcp_tsk(void) {
  printf("wl80211 linkup, start dhcp...\n");
  netif_set_status_callback(&eth_wl80211_netif, ip_got_cb);
  if (netifapi_dhcp_start(&eth_wl80211_netif) == ERR_OK) {
    printf("start dhcp success.\n");
  } else {
    printf("start dhcp fail.\n");
  }
}

static void disconnect_tsk(void) {
  printf("wl80211 disconnected, stop dhcp...\n");
  netif_set_status_callback(&eth_wl80211_netif, ip_got_cb);
  if (netifapi_dhcp_stop(&eth_wl80211_netif) == ERR_OK) {
    printf("stop dhcp success.\n");
  } else {
    printf("stop dhcp fail.\n");
  }
}

static int at_wifi_event_code_get(uint32_t code)
{
    if (code == WL80211_EVT_CONNECTED) {
        return AT_WIFI_EVENT_CONNECTED;
    } else if (code == WL80211_EVT_DISCONNECTED) {
        return AT_WIFI_EVENT_DISCONNECTED;
    } else if (code == WL80211_EVT_SCAN_DONE) {
        return AT_WIFI_EVENT_SCAN_DONE;
    }
    return 0;
}

static void _wifi_event(void *private_data, uint32_t code)
{
    if (code == AT_WIFI_EVENT_CONNECTED) {
        start_dhcp_tsk();
    } else if (code == AT_WIFI_EVENT_DISCONNECTED) {
        disconnect_tsk();
    }
    at_wifi_event_notify(private_data, code);
}

static void wifi_event_start(uint32_t code)
{
    BaseType_t xReturn;

    xReturn = xTimerPendFunctionCall(_wifi_event, NULL, code, 0);
    configASSERT(xReturn == pdPASS);
}

struct netif *at_wifi_netif_get(uint8_t vif_idx)
{
    return (vif_idx == AT_WIFI_VIF_STA) ? &eth_wl80211_netif : NULL;
}

void wl80211_event_handler(uint32_t code1, uint32_t code2)
{
    wifi_event_start(at_wifi_event_code_get(code1));
}

int at_wifi_start(void)
{
    /* Start Wifi_FW */
    wifi_task_create();

    wl80211_init();

    bl_supplicant_init();

    wpa_cbs->wpa_sta_init();

    return 0;
}

