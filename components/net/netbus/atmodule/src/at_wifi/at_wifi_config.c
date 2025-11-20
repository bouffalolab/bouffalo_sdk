/**
  ******************************************************************************
  * @file    at_wifi_config.c
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
//#ifdef EASYFLASH_ENABLE
#include <easyflash.h>
//#endif
#include <wifi_mgmr.h>
#include "at_config.h"
#include "at_wifi_config.h"
#include "at_wifi_main.h"
#include "at_pal.h"

wifi_config *at_wifi_config = NULL;

int at_wifi_config_init(void)
{
    at_wifi_config = (wifi_config *)at_malloc(sizeof(wifi_config));
    if (at_wifi_config == NULL) {
        printf("[WIFI_CONFIG] Error: memory allocation failed\r\n");
        return -1;
    }

    memset(at_wifi_config, 0, sizeof(wifi_config));
    at_wifi_config->dhcp_state.bit.sta_dhcp = 1;
    at_wifi_config->dhcp_state.bit.ap_dhcp = 1;

    at_wifi_config->scan_option.sort_enable = 1;
    at_wifi_config->scan_option.max_count = WIFI_MGMR_SCAN_ITEMS_MAX;
    at_wifi_config->scan_option.rssi_filter = -100;
    at_wifi_config->scan_option.print_mask = 0x7FF;
    at_wifi_config->scan_option.authmode_mask = 0xFF;

    at_wifi_config->netmode = 1;
    at_wifi_config->wevt_enable = 1;
    wifi_mgmr_ap_mac_get(at_wifi_config->ap_mac.addr);
    wifi_mgmr_sta_mac_get(at_wifi_config->sta_mac.addr);
#if CONFIG_ATMODULE_CONFIG_STORAGE
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_MAC, &at_wifi_config->ap_mac.addr, sizeof(wifi_mac_addr))) {
        wifi_mgmr_ap_mac_get(at_wifi_config->ap_mac.addr);
    } else {
        wifi_mgmr_mac_set(&at_wifi_config->ap_mac.addr);
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_MAC, &at_wifi_config->sta_mac.addr, sizeof(wifi_mac_addr))) {
        wifi_mgmr_sta_mac_get(at_wifi_config->sta_mac.addr);
    } else {
        //TODO: AP/STA MAC neet to be different?
        wifi_mgmr_mac_set(&at_wifi_config->sta_mac.addr);
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_MODE, &at_wifi_config->wifi_mode, sizeof(wifi_work_mode))) {
        at_wifi_config->wifi_mode = WIFI_DISABLE;
    }
    at_wifi_config->switch_mode_auto_conn = WIFI_AUTOCONN_ENABLE;
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_INFO, &at_wifi_config->sta_info, sizeof(wifi_sta_info))) {
        memset(&at_wifi_config->sta_info, 0, sizeof(wifi_sta_info));
        at_wifi_config->sta_info.jap_timeout = 15;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_RECONN_CFG, &at_wifi_config->reconn_cfg, sizeof(wifi_sta_reconnect))) {
        at_wifi_config->reconn_cfg.interval_second = 0;
        at_wifi_config->reconn_cfg.repeat_count = 0;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_LAPOPT, &at_wifi_config->scan_option, sizeof(at_wifi_config->scan_option))) {
        at_wifi_config->scan_option.sort_enable = 1;
        at_wifi_config->scan_option.max_count = WIFI_MGMR_SCAN_ITEMS_MAX;
        at_wifi_config->scan_option.rssi_filter = -100;
        at_wifi_config->scan_option.print_mask = 0x7FF;
        at_wifi_config->scan_option.authmode_mask = 0xFF;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_INFO, &at_wifi_config->ap_info, sizeof(wifi_ap_info))) {
        snprintf(at_wifi_config->ap_info.ssid, sizeof(at_wifi_config->ap_info.ssid), "AP_%02X%02X%02X", at_wifi_config->ap_mac.addr[3], at_wifi_config->ap_mac.addr[4], at_wifi_config->ap_mac.addr[5]);
        strlcpy(at_wifi_config->ap_info.pwd, "", sizeof(at_wifi_config->ap_info.pwd));
        at_wifi_config->ap_info.channel = 1;
        at_wifi_config->ap_info.ecn = 0;
        at_wifi_config->ap_info.max_conn = 10;
        at_wifi_config->ap_info.ssid_hidden = 0;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_DHCP_STATE, &at_wifi_config->dhcp_state, sizeof(wifi_dhcp_state))) {
        at_wifi_config->dhcp_state.bit.sta_dhcp = 1;
        at_wifi_config->dhcp_state.bit.ap_dhcp = 1;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_DHCP_SERVER, &at_wifi_config->dhcp_server, sizeof(wifi_dhcp_server))) {
        at_wifi_config->dhcp_server.lease_time = 120;
        at_wifi_config->dhcp_server.start = 2;
        at_wifi_config->dhcp_server.end = 101;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AUTO_CONN, &at_wifi_config->auto_conn, sizeof(wifi_auto_conn))) {
        at_wifi_config->auto_conn = WIFI_AUTOCONN_ENABLE;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_PROTO, &at_wifi_config->ap_proto, sizeof(wifi_proto))) {
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_PROTO, &at_wifi_config->sta_proto, sizeof(wifi_proto))) {
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_IP, &at_wifi_config->ap_ip, sizeof(wifi_ip))) {
        at_wifi_config->ap_ip.ip = IP_SET_ADDR(192, 168, 4 , 1);
        at_wifi_config->ap_ip.gateway = IP_SET_ADDR(192, 168, 4 , 1);
        at_wifi_config->ap_ip.netmask = IP_SET_ADDR(255, 255, 255, 0);
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_IP, &at_wifi_config->sta_ip, sizeof(wifi_ip))) {
        at_wifi_config->sta_ip.ip = IP_SET_ADDR(0, 0, 0, 0);
        at_wifi_config->sta_ip.gateway = IP_SET_ADDR(0, 0, 0, 0);
        at_wifi_config->sta_ip.netmask = IP_SET_ADDR(0, 0, 0, 0);
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_COUNTRY_CODE, &at_wifi_config->wifi_country, sizeof(wifi_country_code))) {
        at_wifi_config->wifi_country.country_policy = 1;
        at_wifi_config->wifi_country.country_code = WIFI_COUNTRY_CODE_WORLD;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_HOSTNAME, at_wifi_config->hostname, sizeof(at_wifi_config->hostname))) {
        strlcpy(at_wifi_config->hostname, "Wlan", sizeof(at_wifi_config->hostname));
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_ANTDIV, &at_wifi_config->ant_div, sizeof(at_wifi_config->ant_div))) {
        at_wifi_config->ant_div.static_ant_div_enable = 0;
        at_wifi_config->ant_div.dynamic_ant_div_enable = 0;
        at_wifi_config->ant_div.ant_div_pin = 0;
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_NETMODE, &at_wifi_config->netmode, sizeof(at_wifi_config->netmode))) {
        at_wifi_config->netmode = at_port_netmode_get();
    }
#endif
    return 0;
}

int at_wifi_config_save(const char *key)
{
#if CONFIG_ATMODULE_CONFIG_STORAGE
    if (!key || !at_wifi_config) {
        printf("[WIFI_CONFIG] Error: null key or config\r\n");
        return -1;
    }

    if (strcmp(key, AT_CONFIG_KEY_WIFI_AP_MAC) == 0)
        return at_config_write(key, &at_wifi_config->ap_mac.addr, sizeof(wifi_mac_addr));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_STA_MAC) == 0)
        return at_config_write(key, &at_wifi_config->sta_mac.addr, sizeof(wifi_mac_addr));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_MODE) == 0)
        return at_config_write(key, &at_wifi_config->wifi_mode, sizeof(wifi_work_mode));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_STA_INFO) == 0)
        return at_config_write(key, &at_wifi_config->sta_info, sizeof(wifi_sta_info));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_RECONN_CFG) == 0)
        return at_config_write(key, &at_wifi_config->reconn_cfg, sizeof(wifi_sta_reconnect));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_AP_INFO) == 0)
        return at_config_write(key, &at_wifi_config->ap_info, sizeof(wifi_ap_info));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_DHCP_STATE) == 0)
        return at_config_write(key, &at_wifi_config->dhcp_state, sizeof(wifi_dhcp_state));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_DHCP_SERVER) == 0)
        return at_config_write(key, &at_wifi_config->dhcp_server, sizeof(wifi_dhcp_server));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_AUTO_CONN) == 0)
        return at_config_write(key, &at_wifi_config->auto_conn, sizeof(wifi_auto_conn));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_AP_PROTO) == 0)
        return at_config_write(key, &at_wifi_config->ap_proto, sizeof(wifi_proto));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_STA_PROTO) == 0)
        return at_config_write(key, &at_wifi_config->sta_proto, sizeof(wifi_proto));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_AP_IP) == 0)
        return at_config_write(key, &at_wifi_config->ap_ip, sizeof(wifi_ip));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_STA_IP) == 0)
        return at_config_write(key, &at_wifi_config->sta_ip, sizeof(wifi_ip));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_COUNTRY_CODE) == 0)
        return at_config_write(key, &at_wifi_config->wifi_country, sizeof(wifi_country_code));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_HOSTNAME) == 0)
        return at_config_write(key, at_wifi_config->hostname, sizeof(at_wifi_config->hostname));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_LAPOPT) == 0)
        return at_config_write(key, &at_wifi_config->scan_option, sizeof(at_wifi_config->scan_option));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_ANTDIV) == 0)
        return at_config_write(key, &at_wifi_config->ant_div, sizeof(at_wifi_config->ant_div));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_NETMODE) == 0)
        return at_config_write(key, &at_wifi_config->netmode, sizeof(at_wifi_config->netmode));
#endif
        return -1;
}

int at_wifi_config_default(void)
{
#if CONFIG_ATMODULE_CONFIG_STORAGE
    at_config_delete(AT_CONFIG_KEY_WIFI_AP_MAC);
    at_config_delete(AT_CONFIG_KEY_WIFI_STA_MAC);
    at_config_delete(AT_CONFIG_KEY_WIFI_MODE);
    at_config_delete(AT_CONFIG_KEY_WIFI_STA_INFO);
    at_config_delete(AT_CONFIG_KEY_WIFI_RECONN_CFG);
    at_config_delete(AT_CONFIG_KEY_WIFI_AP_INFO);
    at_config_delete(AT_CONFIG_KEY_WIFI_DHCP_STATE);
    at_config_delete(AT_CONFIG_KEY_WIFI_DHCP_SERVER);
    at_config_delete(AT_CONFIG_KEY_WIFI_AUTO_CONN);
    at_config_delete(AT_CONFIG_KEY_WIFI_AP_PROTO);
    at_config_delete(AT_CONFIG_KEY_WIFI_STA_PROTO);
    at_config_delete(AT_CONFIG_KEY_WIFI_AP_IP);
    at_config_delete(AT_CONFIG_KEY_WIFI_STA_IP);
    at_config_delete(AT_CONFIG_KEY_WIFI_COUNTRY_CODE);
    at_config_delete(AT_CONFIG_KEY_WIFI_HOSTNAME);
    at_config_delete(AT_CONFIG_KEY_WIFI_LAPOPT);
    at_config_delete(AT_CONFIG_KEY_WIFI_ANTDIV);
#endif
    return 0;
}

