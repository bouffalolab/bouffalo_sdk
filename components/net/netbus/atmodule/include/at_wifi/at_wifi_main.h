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

#ifdef __cplusplus
}
#endif

#endif/* AT_WIFI_MAIN_H */

