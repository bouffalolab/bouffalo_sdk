/**
  ******************************************************************************
  * @file    at_wifi_main.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_WIFI_MAIN_H
#define AT_WIFI_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at_wifi_config.h"
#include "async_event.h"

#ifndef WIFI_MGMR_SCAN_ITEMS_MAX
#define WIFI_MGMR_SCAN_ITEMS_MAX 50
#endif
#define  AT_WIFI_EVENT_INIT_DONE       1
#define  AT_WIFI_EVENT_CONNECTED       2
#define  AT_WIFI_EVENT_DISCONNECTED    3
#define  AT_WIFI_EVENT_GOTIP           4
#define  AT_WIFI_EVENT_SCAN_DONE       5
#define  AT_WIFI_EVENT_AP_STARTED      6
#define  AT_WIFI_EVENT_AP_STOP         7
#define  AT_WIFI_EVENT_AP_STA_ADD      8
#define  AT_WIFI_EVENT_AP_STA_DEL      9

#define AT_WIFI_VIF_STA      0
#define AT_WIFI_VIF_AP       1

#define AT_WIFI_ENC_OPEN     0
#define AT_WIFI_ENC_WPA_PSK  2
#define AT_WIFI_ENC_WPA2_PSK 3
#define AT_WIFI_ENC_WPA3_PSK 4

enum at_wifi_state {
    AT_WIFI_STATE_STA_DISCONNECTED = 0,
    AT_WIFI_STATE_STA_SCANNING,
    AT_WIFI_STATE_STA_CONNECTING,
    AT_WIFI_STATE_STA_4WAY_HANDSHAKE,
    AT_WIFI_STATE_STA_GROUP_HANDSHAKE,
    AT_WIFI_STATE_STA_CONNECTED,
};

/// WiFi Mode
typedef enum {
    /// 802.ll b
    AT_WIFI_MODE_802_11B       = 0x01,
    /// 802.11 a
    AT_WIFI_MODE_802_11A       = 0x02,
    /// 802.11 g
    AT_WIFI_MODE_802_11G       = 0x04,
    /// 802.11n at 2.4GHz
    AT_WIFI_MODE_802_11N_2_4   = 0x08,
    /// 802.11n at 5GHz
    AT_WIFI_MODE_802_11N_5     = 0x10,
    /// 802.11ac at 5GHz
    AT_WIFI_MODE_802_11AC_5    = 0x20,
    /// 802.11ac at 2.4GHz
    AT_WIFI_MODE_802_11AC_2_4  = 0x40,
    /// 802.11ax at 2.4GHz
    AT_WIFI_MODE_802_11AX_2_4  = 0x80,
    /// 802.11ax at 5GHz
    AT_WIFI_MODE_802_11AX_5    = 0x100,
} at_wifi_mode_t;

int at_wifi_start(void);

int at_wifi_stop(void);

int at_wifi_set_mode(void);

int at_wifi_sta_connect(int timeout);

int at_wifi_sta_disconnect(void);

int at_wifi_sta_set_reconnect(void);

int at_wifi_ap_start(void);

int at_wifi_ap_stop(void);

int at_wifi_ap_get_sta_ip(uint8_t *mac, char *ip, uint32_t ipbuf_size, int check_flag);

int at_wifi_ap_set_dhcp_range(int start, int end);

int at_wifi_sniffer_start(void);

int at_wifi_sniffer_set_channel(int channel, void *cb, void *arg);

int at_wifi_sniffer_stop(void);

int at_wifi_state_get(void);

int at_wifi_hostname_set(char *hostname);

int at_wifi_mode_set(uint8_t ap_or_sta, wifi_proto proto);

wifi_proto at_wifi_mode_get(uint8_t ap_or_sta);

void at_wifi_event_notify(void *private_data, uint32_t code);

#ifdef __cplusplus
}
#endif

#endif/* AT_WIFI_MAIN_H */

