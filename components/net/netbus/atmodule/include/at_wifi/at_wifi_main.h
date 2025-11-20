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

