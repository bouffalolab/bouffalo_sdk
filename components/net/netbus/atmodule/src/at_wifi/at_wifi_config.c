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
#include "lfs_kv.h"
//#endif
#include "at_config.h"
#include "at_wifi_config.h"
#include "at_wifi_main.h"
#include "at_port.h"
#include "at_pal.h"
#include "at_wifi_mgmr.h"
#include "at_utils_crypto.h"

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
    at_wifi_mgmr_ap_mac_get(at_wifi_config->ap_mac.addr);
    at_wifi_mgmr_sta_mac_get(at_wifi_config->sta_mac.addr);
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_MAC, &at_wifi_config->ap_mac.addr, sizeof(wifi_mac_addr))) {
        at_wifi_mgmr_ap_mac_get(at_wifi_config->ap_mac.addr);
    } else {
        at_wifi_mgmr_ap_mac_set(at_wifi_config->ap_mac.addr);
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_MAC, &at_wifi_config->sta_mac.addr, sizeof(wifi_mac_addr))) {
        at_wifi_mgmr_sta_mac_get(at_wifi_config->sta_mac.addr);
    } else {
        //TODO: AP/STA MAC neet to be different?
        at_wifi_mgmr_sta_mac_set(at_wifi_config->sta_mac.addr);
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_MODE, &at_wifi_config->wifi_mode, sizeof(wifi_work_mode))) {
        at_wifi_config->wifi_mode = WIFI_DISABLE;
    }
#endif
    at_wifi_config->switch_mode_auto_conn = WIFI_AUTOCONN_ENABLE;

    at_utils_crypto_aes_key_init();

    at_wifi_init_credential_cache();

#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_INFO, &at_wifi_config->sta_info, sizeof(wifi_sta_info))) {
#endif
        memset(&at_wifi_config->sta_info, 0, sizeof(wifi_sta_info));
        at_wifi_config->sta_info.jap_timeout = 15;
        at_wifi_config->sta_info.pmf = 1;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }

    if (strlen(at_wifi_config->sta_info.psk) > 0
        && find_credential_slot(at_wifi_config->sta_info.ssid, NULL)==-1) {
        int slot = save_credential_to_cache(at_wifi_config->sta_info.ssid, at_wifi_config->sta_info.psk);
        if (slot >= 0) {
            memcpy(at_wifi_config->sta_info.pwd_encrypted,
                   at_wifi_config->credential_cache[slot].pwd_encrypted,
                   sizeof(at_wifi_config->sta_info.pwd_encrypted));
            memcpy(at_wifi_config->sta_info.iv,
                   at_wifi_config->credential_cache[slot].iv,
                   sizeof(at_wifi_config->sta_info.iv));
            at_config_write(AT_CONFIG_KEY_WIFI_STA_INFO, &at_wifi_config->sta_info, sizeof(wifi_sta_info));
        }
    }

    if (!at_config_read(AT_CONFIG_KEY_WIFI_RECONN_CFG, &at_wifi_config->reconn_cfg, sizeof(wifi_sta_reconnect))) {
#endif
        at_wifi_config->reconn_cfg.interval_second = 0;
        at_wifi_config->reconn_cfg.repeat_count = 0;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_LAPOPT, &at_wifi_config->scan_option, sizeof(at_wifi_config->scan_option))) {
#endif
        at_wifi_config->scan_option.sort_enable = 1;
        at_wifi_config->scan_option.max_count = WIFI_MGMR_SCAN_ITEMS_MAX;
        at_wifi_config->scan_option.rssi_filter = -100;
        at_wifi_config->scan_option.print_mask = 0x7FF;
        at_wifi_config->scan_option.authmode_mask = 0xFF;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
#endif
    at_wifi_config->wevt_enable = 1;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_INFO, &at_wifi_config->ap_info, sizeof(wifi_ap_info))) {
#endif
        snprintf(at_wifi_config->ap_info.ssid, sizeof(at_wifi_config->ap_info.ssid), "AP_%02X%02X%02X", at_wifi_config->ap_mac.addr[3], at_wifi_config->ap_mac.addr[4], at_wifi_config->ap_mac.addr[5]);
        strlcpy(at_wifi_config->ap_info.pwd, "", sizeof(at_wifi_config->ap_info.pwd));
        at_wifi_config->ap_info.channel = 1;
        at_wifi_config->ap_info.ecn = 0;
        at_wifi_config->ap_info.max_conn = 10;
        at_wifi_config->ap_info.ssid_hidden = 0;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }

    memset(&at_wifi_config->ap_credential_cache, 0, sizeof(wifi_secure_credential_t));
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_SEC_CRED, &at_wifi_config->ap_credential_cache, sizeof(wifi_secure_credential_t))
        && strlen(at_wifi_config->ap_info.ssid) > 0 && strlen(at_wifi_config->ap_info.pwd) > 0) {
        at_utils_crypto_get_random_iv(at_wifi_config->ap_credential_cache.iv);
        at_utils_crypto_aes_cbc_encrypt(at_wifi_config->ap_credential_cache.iv,
                                        64,
                                        (const uint8_t *)at_wifi_config->ap_info.pwd,
                                        at_wifi_config->ap_credential_cache.pwd_encrypted);
        memcpy(at_wifi_config->ap_info.pwd_encrypted,
               at_wifi_config->ap_credential_cache.pwd_encrypted,
               sizeof(at_wifi_config->ap_info.pwd_encrypted));
        at_config_write(AT_CONFIG_KEY_WIFI_AP_INFO, &at_wifi_config->ap_info, sizeof(wifi_ap_info));
        at_config_write(AT_CONFIG_KEY_WIFI_AP_SEC_CRED, &at_wifi_config->ap_credential_cache, sizeof(wifi_secure_credential_t));
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_DHCP_STATE, &at_wifi_config->dhcp_state, sizeof(wifi_dhcp_state))) {
#endif
        at_wifi_config->dhcp_state.bit.sta_dhcp = 1;
        at_wifi_config->dhcp_state.bit.ap_dhcp = 1;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_DHCP_SERVER, &at_wifi_config->dhcp_server, sizeof(wifi_dhcp_server))) {
#endif
        at_wifi_config->dhcp_server.lease_time = 120;
        at_wifi_config->dhcp_server.start = 2;
        at_wifi_config->dhcp_server.end = 101;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AUTO_CONN, &at_wifi_config->auto_conn, sizeof(wifi_auto_conn))) {
#endif
        at_wifi_config->auto_conn = WIFI_AUTOCONN_ENABLE;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_PROTO, &at_wifi_config->ap_proto, sizeof(wifi_proto))) {
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_PROTO, &at_wifi_config->sta_proto, sizeof(wifi_proto))) {
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_AP_IP, &at_wifi_config->ap_ip, sizeof(wifi_ip))) {
#endif
        at_wifi_config->ap_ip.ip = IP_SET_ADDR(192, 168, 4 , 1);
        at_wifi_config->ap_ip.gateway = IP_SET_ADDR(192, 168, 4 , 1);
        at_wifi_config->ap_ip.netmask = IP_SET_ADDR(255, 255, 255, 0);
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_STA_IP, &at_wifi_config->sta_ip, sizeof(wifi_ip))) {
#endif
        at_wifi_config->sta_ip.ip = IP_SET_ADDR(0, 0, 0, 0);
        at_wifi_config->sta_ip.gateway = IP_SET_ADDR(0, 0, 0, 0);
        at_wifi_config->sta_ip.netmask = IP_SET_ADDR(0, 0, 0, 0);
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_COUNTRY_CODE, &at_wifi_config->wifi_country, sizeof(wifi_country_code))) {
#endif
        at_wifi_config->wifi_country.country_policy = 1;
        at_wifi_config->wifi_country.country_code = WIFI_COUNTRY_CODE_WORLD;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_HOSTNAME, at_wifi_config->hostname, sizeof(at_wifi_config->hostname))) {
#endif
        strlcpy(at_wifi_config->hostname, "Wlan", sizeof(at_wifi_config->hostname));
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_ANTDIV, &at_wifi_config->ant_div, sizeof(at_wifi_config->ant_div))) {
#endif
        at_wifi_config->ant_div.static_ant_div_enable = 0;
        at_wifi_config->ant_div.dynamic_ant_div_enable = 0;
        at_wifi_config->ant_div.ant_div_pin = 0;
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
    if (!at_config_read(AT_CONFIG_KEY_WIFI_NETMODE, &at_wifi_config->netmode, sizeof(at_wifi_config->netmode))) {
#endif
        at_wifi_config->netmode = at_port_netmode_get();
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
    }
#endif
    return 0;
}

int at_wifi_config_save(const char *key)
{
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
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
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_SEC_CRED) == 0)
        return at_config_write(key, at_wifi_config->credential_cache, sizeof(at_wifi_config->credential_cache));
    else if (strcmp(key, AT_CONFIG_KEY_WIFI_AP_SEC_CRED) == 0)
        return at_config_write(key, &at_wifi_config->ap_credential_cache, sizeof(wifi_secure_credential_t));
#endif

    return -1;
}

int at_wifi_config_default(void)
{
#if defined(CONFIG_ATMODULE_CONFIG_STORAGE) && (CONFIG_ATMODULE_CONFIG_STORAGE)
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
    at_config_delete(AT_CONFIG_KEY_WIFI_NETMODE);
    at_config_delete(AT_CONFIG_KEY_WIFI_SEC_CRED);
    at_config_delete(AT_CONFIG_KEY_WIFI_AP_SEC_CRED);
#endif

    return 0;
}

void at_wifi_init_credential_cache(void)
{
    memset(at_wifi_config->credential_cache, 0, sizeof(at_wifi_config->credential_cache));

    at_config_read(AT_CONFIG_KEY_WIFI_SEC_CRED, at_wifi_config->credential_cache,
                   sizeof(at_wifi_config->credential_cache));
}

int find_credential_slot(const char *ssid, int *free_index)
{
    if (free_index) {
        *free_index = -1;
    }

    for (int i = 0; i < MAX_SECURE_CRED_COUNT; i++) {
        if (at_wifi_config->credential_cache[i].ssid[0] == 0 && free_index && *free_index == -1) {
            *free_index = i;
        } else if (strcmp(at_wifi_config->credential_cache[i].ssid, ssid) == 0) {
            return i;
        }
    }

    return -1;
}

int credential_update(void)
{
    int slot, free_slot;
    slot = find_credential_slot(at_wifi_config->sta_info.ssid, &free_slot);

    if (slot == -1) {
        slot = free_slot;
    }

    if (slot == -1) {
        return -1;
    }

    strlcpy(at_wifi_config->credential_cache[slot].ssid,
            at_wifi_config->sta_info.ssid,
            sizeof(at_wifi_config->credential_cache[slot].ssid));

    memcpy(at_wifi_config->credential_cache[slot].pwd_encrypted,
           at_wifi_config->sta_info.pwd_encrypted,
           sizeof(at_wifi_config->credential_cache[slot].pwd_encrypted));

    memcpy(at_wifi_config->credential_cache[slot].iv,
           at_wifi_config->sta_info.iv,
           sizeof(at_wifi_config->credential_cache[slot].iv));

    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_INFO);
        at_config_write(AT_CONFIG_KEY_WIFI_SEC_CRED, at_wifi_config->credential_cache,
                        sizeof(at_wifi_config->credential_cache));
    }
    return 0;
}

int save_credential_to_cache(const char *ssid, const char *password)
{
    int slot, free_slot;
    char pwd[65] = {0};

    slot = find_credential_slot(ssid, &free_slot);
    if (slot == -1) {
        slot = free_slot;
    }
    if (slot == -1) {
        return -1;
    }

    memset(&at_wifi_config->credential_cache[slot], 0, sizeof(wifi_secure_credential_t));
    strlcpy(at_wifi_config->credential_cache[slot].ssid, ssid, sizeof(at_wifi_config->credential_cache[slot].ssid));

    strlcpy(pwd, password, sizeof(pwd));

    at_utils_crypto_get_random_iv(at_wifi_config->credential_cache[slot].iv);
    at_utils_crypto_aes_cbc_encrypt(at_wifi_config->credential_cache[slot].iv,
                                    64,
                                    (const uint8_t *)pwd,
                                    at_wifi_config->credential_cache[slot].pwd_encrypted);

    memset(pwd, 0, sizeof(pwd));

    if (at->store) {
        at_config_write(AT_CONFIG_KEY_WIFI_SEC_CRED, at_wifi_config->credential_cache,
                        sizeof(at_wifi_config->credential_cache));
    }
    return slot;
}

int clear_cached_credential(const char *ssid)
{
    int slot;

    slot = find_credential_slot(ssid, NULL);
    if (slot == -1) {
        return -1;
    }

    memset(&at_wifi_config->credential_cache[slot], 0, sizeof(wifi_secure_credential_t));
    at_config_write(AT_CONFIG_KEY_WIFI_SEC_CRED, at_wifi_config->credential_cache,
                    sizeof(at_wifi_config->credential_cache));

    return 0;
}

int delete_credential_from_cache(const char *ssid)
{
    int ret;

    ret = clear_cached_credential(ssid);
    if (ret == -1) {
        return -1;
    }

    if (strcmp(at_wifi_config->sta_info.ssid, ssid) == 0) {
        memset(&at_wifi_config->sta_info, 0, sizeof(wifi_sta_info));
        at_config_write(AT_CONFIG_KEY_WIFI_STA_INFO, &at_wifi_config->sta_info, sizeof(wifi_sta_info));
    }

    return 0;
}
