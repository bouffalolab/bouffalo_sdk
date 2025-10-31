/**
  ******************************************************************************
  * @file    at_wifi_cmd.c
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
#include <semphr.h>

#include <lwip/tcpip.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/udp.h>
#include <lwip/altcp.h>
#include <lwip/altcp_tcp.h>
#include <lwip/altcp_tls.h>
#include <lwip/err.h>
#include <lwip/netif.h>

#include "at_main.h"
#include "at_core.h"
#include "at_wifi_config.h"
#include "at_wifi_main.h"

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "fhost.h"
#include "at_net_main.h"
#include "at_net_config.h"

#define AT_WIFI_CMD_PRINTF printf

static uint8_t g_scan_filter_mac_flag = 0;
static uint8_t g_scan_filter_ssid_flag = 0;
static uint8_t g_scan_filter_mac[6];
static char g_scan_filter_ssid[33];
static uint8_t g_scan_filter_channel = 0;
static uint32_t g_scan_tick = 0;

static int get_mac_from_string(char *string, uint8_t mac[6])
{
    int i, j = 0;
    char mac_string[13];

    if (strlen(string) != 17) {
        return -1;
    }

    memset(mac_string, 0, sizeof(mac_string));
    for (i = 0; i < strlen(string); i++) {
        if (i % 3 == 2) {
            if (string[i] != ':') {
                return -1;
            }
        } else {
            mac_string[j++] = string[i];
        }
    }

    if (utils_hex2bin((const char *)mac_string, strlen(mac_string), mac, 6) > 0) {
        return 0;
    }

    return -1;
}

static int at_query_cmd_wifisp(int argc, const char **argv)
{
    at_response_string("+WIFISP:%d\r\n", !at_wifi_config->wlan_disable);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_wifisp(int argc, const char **argv)
{
    int enable = 0;

    AT_CMD_PARSE_NUMBER(0, &enable);

    if (enable == 0) {
        if (!wifi_mgmr_sta_state_get() && !wifi_mgmr_ap_state_get()) {
            wifi_mgmr_rf_pwr_off();
        } else {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_NOT_ALLOWED);
        }
    } else if (enable == 1) {
        wifi_mgmr_rf_pwr_on();
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_wifi_config->wlan_disable = !enable;

    return AT_RESULT_CODE_OK;
}

static int get_ip_from_string(char *string, uint32_t *ip)
{
    uint32_t ipaddr;

    ipaddr = ipaddr_addr(string);
    if (ipaddr == IPADDR_NONE || ipaddr == IPADDR_ANY) {
        return -1;
    }

    *ip = ipaddr;
    return 0;
}

static int at_query_cmd_cwmode(int argc, const char **argv)
{
    at_response_string("+CWMODE:%d\r\n", at_wifi_config->wifi_mode);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwmode(int argc, const char **argv)
{
    int mode = 0;
    int auto_connect_valid =0, auto_connect;

    AT_CMD_PARSE_NUMBER(0, &mode);
    AT_CMD_PARSE_OPT_NUMBER(1, &auto_connect, auto_connect_valid);

    if (mode < WIFI_DISABLE || mode > WIFI_AP_STA_MODE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (auto_connect_valid) {
        if (auto_connect != WIFI_AUTOCONN_DISABLE && auto_connect != WIFI_AUTOCONN_ENABLE) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        at_wifi_config->switch_mode_auto_conn = auto_connect;
    } else {
        at_wifi_config->switch_mode_auto_conn = WIFI_AUTOCONN_ENABLE;
    }

    if (at_wifi_config->wifi_mode != mode) {
        at_wifi_config->wifi_mode = mode;
        if (at->store) {
            at_wifi_config_save(AT_CONFIG_KEY_WIFI_MODE);
        }
        at_wifi_set_mode();
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwstate(int argc, const char **argv)
{
    wifi_mgmr_connect_ind_stat_info_t info = {0};
    int state = at_wifi_state_get();
    ip4_addr_t ipaddr = {0};
    wifi_sta_ip4_addr_get(&ipaddr.addr, NULL, NULL, NULL);

    wifi_mgmr_sta_connect_ind_stat_get(&info);

    if (state == FHOST_STA_CONNECTED && !ip4_addr_isany(&ipaddr)) {
        at_response_string("+CWSTATE:%d,\"%s\"\r\n", 2, info.ssid);
        return AT_RESULT_CODE_OK;
    }
    if (at_wifi_config->reconnect_state ||
        at_wifi_config->connecting_state ||
        state == FHOST_STA_CONNECTING ||
        state == FHOST_STA_4WAY_HANDSHAKE ||
        state == FHOST_STA_GROUP_HANDSHAKE) {
        at_response_string("+CWSTATE:%d,\"%s\"\r\n", 3, at_wifi_config->sta_info.ssid);
        return AT_RESULT_CODE_OK;
    }

    if (state == FHOST_STA_DISCONNECTED) {
        if ((at_wifi_config->wifi_mode != WIFI_STATION_MODE) && (at_wifi_config->wifi_mode != WIFI_AP_STA_MODE)) {
            at_response_string("+CWSTATE:%d,\"\"\r\n", 0);
        } else {
            at_response_string("+CWSTATE:%d,\"\"\r\n", 4);
        }
    } else if (state == FHOST_STA_CONNECTED) {
        at_response_string("+CWSTATE:%d,\"%s\"\r\n", 1, info.ssid);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwjap(int argc, const char **argv)
{
    int rssi = 0;
    ip4_addr_t ipaddr = {0};
    wifi_mgmr_connect_ind_stat_info_t info = {0};

    int state = at_wifi_state_get();
    wifi_sta_ip4_addr_get(&ipaddr.addr, NULL, NULL, NULL);

    if (state == FHOST_STA_CONNECTED) {
        wifi_mgmr_sta_connect_ind_stat_get(&info);
        wifi_mgmr_sta_rssi_get(&rssi);
        at_response_string("+CWJAP:\"%s\",\"%02x:%02x:%02x:%02x:%02x:%02x\",%d,%d,%d\r\n",
            info.ssid,
            info.bssid[0], info.bssid[1], info.bssid[2], info.bssid[3], info.bssid[4], info.bssid[5],
            info.channel,
            rssi,
            at_wifi_config->sta_info.wep_en);
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwjap(int argc, const char **argv)
{
    char ssid[33];
    char password[65];
    char bssidString[20];
    int bssid_valid = 0;
    uint8_t bssid[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    int wep_enable_valid = 0, wep_enable = 0;
    int listen_interval_valid = 0, listen_interval = 3;
    int scan_mode_valid = 0, scan_mode = 1;
    int jap_timeout_valid = 0, jap_timeout = 15;
    int pmf_valid = 0, pmf = 0;

    AT_CMD_PARSE_STRING(0, ssid, sizeof(ssid));
    AT_CMD_PARSE_STRING(1, password, sizeof(password));
    AT_CMD_PARSE_OPT_STRING(2, bssidString, sizeof(bssidString), bssid_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &wep_enable, wep_enable_valid);
    AT_CMD_PARSE_OPT_NUMBER(4, &listen_interval, listen_interval_valid);
    AT_CMD_PARSE_OPT_NUMBER(5, &scan_mode, scan_mode_valid);
    AT_CMD_PARSE_OPT_NUMBER(6, &jap_timeout, jap_timeout_valid);
    AT_CMD_PARSE_OPT_NUMBER(7, &pmf, pmf_valid);

    if (bssid_valid && (get_mac_from_string(bssidString, bssid) != 0)) {
        printf("err bssid_valid\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }
    if (wep_enable_valid && (wep_enable != 0 && wep_enable != 1)) {
        printf("err pci_enable_valid\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (listen_interval_valid && (listen_interval < 1 || listen_interval > 100)) {
        printf("err listen_interval_valid\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (scan_mode_valid && (scan_mode != 0 && scan_mode != 1)) {
        printf("err scan_mode_valid\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (jap_timeout_valid && (jap_timeout < 3 || jap_timeout > 600)) {
        printf("err jap_timeout_valid\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (pmf_valid && (pmf < 0 || pmf > 3)) {
        printf("err pmf_valid \r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if ((at_wifi_config->wifi_mode != WIFI_STATION_MODE) && (at_wifi_config->wifi_mode != WIFI_AP_STA_MODE)) {
        printf("err at_wifi_config\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    strlcpy(at_wifi_config->sta_info.ssid, ssid, sizeof(at_wifi_config->sta_info.ssid));
    strlcpy(at_wifi_config->sta_info.psk, password, sizeof(at_wifi_config->sta_info.psk));
    if (bssid_valid) {
        memcpy(at_wifi_config->sta_info.bssid, bssid, sizeof(at_wifi_config->sta_info.bssid));
    } else {
        memset(at_wifi_config->sta_info.bssid, 0, sizeof(at_wifi_config->sta_info.bssid));
    }
    at_wifi_config->sta_info.wep_en = (uint8_t)wep_enable;
    at_wifi_config->sta_info.listen_interval = listen_interval;
    at_wifi_config->sta_info.scan_mode = scan_mode;
    at_wifi_config->sta_info.jap_timeout = jap_timeout;
    at_wifi_config->sta_info.pmf = pmf;
    wifi_mgmr_psk_cal(password, ssid, strlen(ssid), at_wifi_config->sta_info.pmk);
    at_wifi_config->sta_info.freq = 0;
    at_wifi_config->sta_info.store = 0;

    at_wifi_sta_set_reconnect();

    at_wifi_sta_connect(at_wifi_config->sta_info.jap_timeout*1000);

    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_cwjap(int argc, const char **argv)
{
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwreconncfg(int argc, const char **argv)
{
    at_response_string("+CWRECONNCFG:%d,%d\r\n", at_wifi_config->reconn_cfg.interval_second, at_wifi_config->reconn_cfg.repeat_count);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwreconncfg(int argc, const char **argv)
{
    int interval_second, repeat_count;

    AT_CMD_PARSE_NUMBER(0, &interval_second);
    AT_CMD_PARSE_NUMBER(1, &repeat_count);

    if ((interval_second >= 0 && interval_second <= 7200) && (repeat_count >= 0 && repeat_count <= 1000)) {
        at_wifi_config->reconn_cfg.interval_second = (uint16_t)interval_second;
        at_wifi_config->reconn_cfg.repeat_count = (uint16_t)repeat_count;
        if (at->store) {
            at_wifi_config_save(AT_CONFIG_KEY_WIFI_RECONN_CFG);
        }
        at_wifi_sta_set_reconnect();
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwlapopt(int argc, const char **argv)
{
    at_response_string("+CWLAPOPT:%d,%d,%d,%d,%d\r\n",
                      at_wifi_config->scan_option.sort_enable,
                      at_wifi_config->scan_option.print_mask,
                      at_wifi_config->scan_option.rssi_filter,
                      at_wifi_config->scan_option.authmode_mask,
                      at_wifi_config->scan_option.max_count);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwlapopt(int argc, const char **argv)
{
    int sort_enable, print_mask;
    int rssi_filter_valid = 0, rssi_filter;
    int max_count_valid = 0, max_count;
    int authmode_mask_valid = 0, authmode_mask;

    AT_CMD_PARSE_NUMBER(0, &sort_enable);
    AT_CMD_PARSE_NUMBER(1, &print_mask);
    AT_CMD_PARSE_OPT_NUMBER(2, &rssi_filter, rssi_filter_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &authmode_mask, authmode_mask_valid);
    AT_CMD_PARSE_OPT_NUMBER(4, &max_count, max_count_valid);

    if (sort_enable != 0 && sort_enable != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (print_mask < 0 || print_mask > 0x7FF) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (max_count_valid && (max_count <= 0 || max_count > 50)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (rssi_filter_valid && (rssi_filter < -100 || rssi_filter > 40)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (authmode_mask_valid && (authmode_mask < 0 || authmode_mask > 0xFF)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_wifi_config->scan_option.sort_enable = sort_enable;
    at_wifi_config->scan_option.print_mask = print_mask;
    if (rssi_filter_valid) {
        at_wifi_config->scan_option.rssi_filter = rssi_filter;
    }
    if (authmode_mask_valid) {
        at_wifi_config->scan_option.authmode_mask = authmode_mask;
    }
    if (max_count_valid) {
        at_wifi_config->scan_option.max_count = max_count;
    }

    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_LAPOPT);
    }
    return AT_RESULT_CODE_OK;
}

void at_scan_done_event_tigger(void)
{
    wifi_mgmr_sta_scanlist();
    at_scan_dump(rtos_now(0) - g_scan_tick);
    g_scan_tick = 0;
}

static int at_scan_wifi(uint8_t *channels, uint16_t channel_num, uint8_t mac[6], const char *ssid, uint8_t scan_mode, uint32_t scan_time)
{
    int ret = 0;
    static wifi_mgmr_scan_params_t scan_cfg;

    memset(&scan_cfg, 0, sizeof(scan_cfg));

    if (scan_time == 0) {
        scan_time = 220;
    }
    scan_cfg.passive = scan_mode;
    scan_cfg.channels_cnt = channel_num;
    scan_cfg.duration = scan_time;
    if (channels) {
        memcpy(scan_cfg.channels, channels, channel_num);
    }
    if (ssid) {
        scan_cfg.ssid_length = strlen(ssid);
        strlcpy(scan_cfg.ssid_array, ssid, sizeof(scan_cfg.ssid_array));
    }
    if (mac) {
        scan_cfg.bssid_set_flag = 1;
        memcpy(scan_cfg.bssid, mac, sizeof(scan_cfg.bssid));
    }
#if CONFIG_ATMODULE_ANTENNA_CTL
    if (antenna_hal_is_static_div_enabled()) {
        ant_scan_task_start(&scan_cfg, NULL);
    } else
#endif
    {
        wifi_mgmr_sta_scan(&scan_cfg);
    }

    if (!g_scan_tick) {
        g_scan_tick = rtos_now(0);
    }

    return ret;
}

static void at_scan_sort(int num, int8_t *rssi, uint8_t *index)
{
    int i, j;
    int8_t rssi_temp;
    uint8_t index_temp;

    for (i = 0; i < num; i++) {
        for (j=0; j<num - i -1; j++) {
            if (rssi[j] < rssi[j + 1]) {
                rssi_temp = rssi[j + 1];
                rssi[j + 1] = rssi[j];
                rssi[j] = rssi_temp;

                index_temp = index[j + 1];
                index[j + 1] = index[j];
                index[j] = index_temp;
            }
        }
    }
}

static int at_scan_get_ecn(uint8_t auth)
{
    if (auth == WIFI_EVENT_BEACON_IND_AUTH_OPEN) {
        return 0;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WEP) {
        return 1;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK) {
        return 2;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK) {
        return 3;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK) {
        return 4;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT) {
        return 5;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE) {
        return 6;
    } else if (auth == WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE) {
        return 7;
    } else {
        return 8;
    }

}

static int at_scan_get_cipher(uint8_t cipher)
{
    if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_NONE) {
        return 0;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_WEP) {
        return 1;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_AES) {
        return 4;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP) {
        return 3;
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES) {
        return 5;
    } else {
        return 7;
    }
}

static int _wifi_mgmr_scan_item_is_timeout(wifi_mgmr_t *mgmr, wifi_mgmr_scan_item_t *item, uint32_t timeout)
{
    return ((unsigned int)rtos_now(0) - (unsigned int)item->timestamp_lastseen) >= (timeout + 1000) ? 1 : 0;
}
void at_scan_dump(uint32_t timeout)
{
#if 1
    int i;
    wifi_mgmr_scan_item_t *scan;
    int array_num = 0;
    uint8_t index_array[WIFI_MGMR_SCAN_ITEMS_MAX];
    int8_t rssi_array[WIFI_MGMR_SCAN_ITEMS_MAX];
    int8_t rssi_filter = at_wifi_config->scan_option.rssi_filter;
    uint16_t print_mask = at_wifi_config->scan_option.print_mask;
    uint16_t authmode_mask = at_wifi_config->scan_option.authmode_mask;
    char outbuf[128];

    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        scan = &wifiMgmr.scan_items[i];
        if (scan->is_used && (!_wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &(wifiMgmr.scan_items[i]), timeout))) {
            /* ssid filter */
            if (g_scan_filter_ssid_flag && strlen(g_scan_filter_ssid) > 0) {
                if (strcmp(g_scan_filter_ssid, scan->ssid)) {
                    continue;
                }
            }
            /* mac filter */
            if (g_scan_filter_mac_flag) {
                if (memcmp(g_scan_filter_mac, scan->bssid, 6)) {
                    continue;
                }
            }
            /* channel filter */
            if (g_scan_filter_channel != 0) {
                if (g_scan_filter_channel != scan->channel) {
                    continue;
                }
            }
            /* rssi and autmode filter */
            if (scan->rssi >= rssi_filter && BIT_ISSET(authmode_mask, at_scan_get_ecn(scan->auth))) {
                index_array[array_num] = i;
                rssi_array[array_num] = scan->rssi;
                array_num++;
            }
        }
    }
    if (at_wifi_config->scan_option.sort_enable) {
        at_scan_sort(array_num, rssi_array, index_array);
    }

    if (at_wifi_config->scan_option.max_count && (array_num > at_wifi_config->scan_option.max_count)) {
        array_num = at_wifi_config->scan_option.max_count;
    }

    for (i = 0; i < array_num; i++) {
        scan = &wifiMgmr.scan_items[index_array[i]];

        memset(outbuf, 0, sizeof(outbuf));
        snprintf(outbuf, sizeof(outbuf), "+CWLAP:(");
        if (BIT_ISSET(print_mask, 0)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", at_scan_get_ecn(scan->auth));
        }
        if (BIT_ISSET(print_mask, 1)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "\"%s\",", scan->ssid);
        }
        if (BIT_ISSET(print_mask, 2)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", scan->rssi);
        }
        if (BIT_ISSET(print_mask, 3)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "\"%02x:%02x:%02x:%02x:%02x:%02x\",",
                    scan->bssid[0],
                    scan->bssid[1],
                    scan->bssid[2],
                    scan->bssid[3],
                    scan->bssid[4],
                    scan->bssid[5]);
        }
        if (BIT_ISSET(print_mask, 4)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", scan->channel);
        }
        if (BIT_ISSET(print_mask, 5)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", -1);
        }
        if (BIT_ISSET(print_mask, 6)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", -1);
        }
        if (BIT_ISSET(print_mask, 7)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", at_scan_get_cipher(scan->cipher));
        }
#if 0
        if (BIT_ISSET(print_mask, 8)) {
            snprintf(outbuf + strlen(outbuf), "%d,", at_scan_get_cipher(scan->group_cipher));
        }
#endif
        if (BIT_ISSET(print_mask, 9)) {
            int mode = 0;
            if (scan->mode&WIFI_MODE_802_11B) {
                mode |= 0x01;
            }
            if (scan->mode&WIFI_MODE_802_11G) {
                mode |= 0x02;
            }
            if (scan->mode&WIFI_MODE_802_11N_2_4) {
                mode |= 0x04;
            }
            if (scan->mode&WIFI_MODE_802_11AX_2_4) {
                mode |= 0x08;
            }
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", mode);
        }
        if (BIT_ISSET(print_mask, 10)) {
            snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), "%d,", scan->wps);
        }
        if (outbuf[strlen(outbuf)-1] == ',') {
            outbuf[strlen(outbuf)-1] = '\0';
        }
        snprintf(outbuf + strlen(outbuf), sizeof(outbuf) - strlen(outbuf), ")\r\n");
        AT_CMD_RESPONSE(outbuf);
    }
#endif
}

static int at_setup_cmd_cwlap(int argc, const char **argv)
{
    int ssid_valid = 0;
    char ssid[33];
    int mac_valid = 0;
    char mac[18];
    char mac_addr[6];
    int channel_valid = 0, channel;
    int scan_type_valid = 0, scan_type;
    int scan_time_min_valid = 0, scan_time_min;
    int scan_time_max_valid = 0, scan_time_max;

    int ret;
    uint8_t scan_channels = 0;
    uint8_t scan_mode = 0;
    uint32_t scan_time = 0;

    AT_CMD_PARSE_OPT_NUMBER(0, &scan_type, scan_type_valid);
    AT_CMD_PARSE_OPT_STRING(1, ssid, sizeof(ssid), ssid_valid);
    AT_CMD_PARSE_OPT_STRING(2, mac, sizeof(mac), mac_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &channel, channel_valid);
    AT_CMD_PARSE_OPT_NUMBER(4, &scan_time_min, scan_time_min_valid);
    AT_CMD_PARSE_OPT_NUMBER(5, &scan_time_max, scan_time_max_valid);

    if (ssid_valid) {
        strlcpy(g_scan_filter_ssid, ssid, sizeof(g_scan_filter_ssid));
        g_scan_filter_ssid_flag = 1;
    }
    if (mac_valid) {
        if (get_mac_from_string(mac, g_scan_filter_mac) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
        memcpy(mac_addr, g_scan_filter_mac, sizeof(mac_addr));
        g_scan_filter_mac_flag = 1;
    }
    if (channel_valid) {
        scan_channels = channel;
        g_scan_filter_channel = channel;
    }
    if (scan_type_valid) {
        AT_WIFI_CMD_PRINTF("scan_type = %d\r\n", scan_type);
        if (scan_type != 0 && scan_type != 1) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        scan_mode = scan_type;
    }
    if (scan_time_min_valid) {
        AT_WIFI_CMD_PRINTF("scan_time_min = %d\r\n", scan_time_min);
        if (scan_time_min < 0 || scan_time_min > 1500) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (scan_time_min > 0) {
            scan_time = scan_time_min;
        }
    }
    if (scan_time_max_valid) {
        AT_WIFI_CMD_PRINTF("scan_time_max = %d\r\n", scan_time_max);
        if (scan_time_max < 0 || scan_time_max > 1500) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (scan_time_max == 0) {
            if (scan_mode == 0) {
                scan_time = 120;
            } else {
                scan_time = 360;
            }
        } else {
            if (scan_time == 0) {
                scan_time = scan_time_max;
            } else {
                scan_time = (scan_time + scan_time_max)/2;
            }
        }
    }
    AT_WIFI_CMD_PRINTF("scan_mode = %d, scan_time = %d\r\n", scan_mode, scan_time);

    if (scan_channels != 0) {
        ret = at_scan_wifi(&scan_channels, 1, mac_valid?mac_addr:NULL, ssid_valid?ssid:NULL, scan_mode, scan_time);
    } else {
        ret = at_scan_wifi(NULL, 0, mac_valid?mac_addr:NULL, ssid_valid?ssid:NULL, scan_mode, scan_time);
    }
    if (ret != 0) {
        g_scan_filter_mac_flag = 0;
        g_scan_filter_ssid_flag = 0;
        g_scan_filter_channel = 0;
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    //at_scan_dump();
    g_scan_filter_mac_flag = 0;
    g_scan_filter_ssid_flag = 0;
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_cwlap(int argc, const char **argv)
{
    if (at_scan_wifi(NULL, 0, NULL, NULL, 0, 0) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    //at_scan_dump();
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_cwqap(int argc, const char **argv)
{
    if ((at_wifi_config->wifi_mode != WIFI_STATION_MODE) && (at_wifi_config->wifi_mode != WIFI_AP_STA_MODE)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    at_wifi_sta_disconnect();
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwqap(int argc, const char **argv)
{
    int restore = 0;
    int restore_valid = 0;

    AT_CMD_PARSE_OPT_NUMBER(0, &restore, restore_valid);

    if ((at_wifi_config->wifi_mode != WIFI_STATION_MODE) && (at_wifi_config->wifi_mode != WIFI_AP_STA_MODE)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    if (restore_valid && restore) {
        memset(&at_wifi_config->sta_info.ssid, 0, sizeof(at_wifi_config->sta_info.ssid));
        ef_del_env(AT_CONFIG_KEY_WIFI_STA_INFO);
    }

    at_wifi_sta_disconnect();
    return AT_RESULT_CODE_OK;
}

#if 1
static int at_query_cmd_cwsap(int argc, const char **argv)
{
    if (at_wifi_config->wifi_mode != WIFI_SOFTAP_MODE && at_wifi_config->wifi_mode != WIFI_AP_STA_MODE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    at_response_string("+CWSAP:\"%s\",\"%s\",%d,%d,%d,%d\r\n",
            at_wifi_config->ap_info.ssid,
            at_wifi_config->ap_info.pwd,
            at_wifi_config->ap_info.channel,
            at_wifi_config->ap_info.ecn,
            at_wifi_config->ap_info.max_conn,
            at_wifi_config->ap_info.ssid_hidden);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwsap(int argc, const char **argv)
{
    char ssid[33];
    char pwd[65];
    int channel, ecn;
    int max_conn_valid = 0, max_conn;
    int ssid_hidden_valid = 0, ssid_hidden;

    AT_CMD_PARSE_STRING(0, ssid, sizeof(ssid));
    AT_CMD_PARSE_STRING(1, pwd, sizeof(pwd));
    AT_CMD_PARSE_NUMBER(2, &channel);
    AT_CMD_PARSE_NUMBER(3, &ecn);
    AT_CMD_PARSE_OPT_NUMBER(4, &max_conn, max_conn_valid);
    AT_CMD_PARSE_OPT_NUMBER(5, &ssid_hidden, ssid_hidden_valid);

    if (at_wifi_config->wifi_mode != WIFI_SOFTAP_MODE && at_wifi_config->wifi_mode != WIFI_AP_STA_MODE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    if (channel < 1 || channel > 13) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (!(ecn == AT_WIFI_ENC_OPEN || (ecn >= AT_WIFI_ENC_WPA_PSK && ecn <= AT_WIFI_ENC_WPA3_PSK)) || (ecn >= AT_WIFI_ENC_WPA_PSK  && ecn <= AT_WIFI_ENC_WPA3_PSK && strlen(pwd) < 8)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (max_conn_valid && (max_conn < 1 || max_conn > CFG_STA_MAX)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (ssid_hidden_valid && (ssid_hidden != 0 && ssid_hidden != 1)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    strlcpy(at_wifi_config->ap_info.ssid, ssid, sizeof(at_wifi_config->ap_info.ssid));
    strlcpy(at_wifi_config->ap_info.pwd, pwd, sizeof(at_wifi_config->ap_info.pwd));
    at_wifi_config->ap_info.channel = channel;
    at_wifi_config->ap_info.ecn = ecn;

    if (max_conn_valid) {
        at_wifi_config->ap_info.max_conn = max_conn;
    }
    if (ssid_hidden_valid) {
        at_wifi_config->ap_info.ssid_hidden = ssid_hidden;
    }
    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_AP_INFO);
    }

    at_wifi_ap_start();
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_cwlif(int argc, const char **argv)
{
    int i;
    struct wifi_sta_basic_info sta_info;
    char ip[20];

    //wifi_mgmr_ap_sta_cnt_get(&sta_cnt);
    for(i = 0; i < CFG_STA_MAX; i++) {
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if(!sta_info.is_used || (sta_info.sta_idx == 0xef)) {
            continue;
        }

        at_wifi_ap_get_sta_ip(sta_info.sta_mac, ip, sizeof(ip), 1);
        at_response_string("+CWLIF:%s,%02x:%02x:%02x:%02x:%02x:%02x\r\n", ip,
                sta_info.sta_mac[0],
                sta_info.sta_mac[1],
                sta_info.sta_mac[2],
                sta_info.sta_mac[3],
                sta_info.sta_mac[4],
                sta_info.sta_mac[5]);
    }
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwqif(int argc, const char **argv)
{
    char mac_string[18];
    uint8_t mac[6];
    int i, sta_index = -1;
    struct wifi_sta_basic_info sta_info;

    if (at_wifi_config->wifi_mode != WIFI_SOFTAP_MODE && at_wifi_config->wifi_mode != WIFI_AP_STA_MODE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    AT_CMD_PARSE_STRING(0, mac_string, sizeof(mac_string));
    if (get_mac_from_string(mac_string, mac) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }

    for(i = 0; i < CFG_STA_MAX; i++) {
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if(!sta_info.is_used || (sta_info.sta_idx == 0xef))
            continue;

        if (memcmp(mac, sta_info.sta_mac, 6) == 0) {
            sta_index = i;
            break;
        }
    }

    if (sta_index == -1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }
    wifi_mgmr_ap_sta_delete(sta_index);
    vTaskDelay(200);
    return AT_RESULT_CODE_OK;
}

static int at_exe_cmd_cwqif(int argc, const char **argv)
{
    int i;
    struct wifi_sta_basic_info sta_info;
    uint8_t sta_cnt;

    if (at_wifi_config->wifi_mode != WIFI_SOFTAP_MODE && at_wifi_config->wifi_mode != WIFI_AP_STA_MODE) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }
    for(i = 0; i < CFG_STA_MAX; i++) {
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if(!sta_info.is_used || (sta_info.sta_idx == 0xef)) {
            continue;
        }

        wifi_mgmr_ap_sta_delete(sta_info.sta_idx);
    }
    vTaskDelay(200);
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwdhcp(int argc, const char **argv)
{
    at_response_string("+CWDHCP:%d", at_wifi_config->dhcp_state);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwdhcp(int argc, const char **argv)
{
    int operate, mode;

    AT_CMD_PARSE_NUMBER(0, &operate);
    AT_CMD_PARSE_NUMBER(1, &mode);

    if ((operate == 0 || operate == 1) && (mode >= 1 && mode <= 3)) {
        if (mode & 0x01) {
            at_wifi_config->dhcp_state.bit.sta_dhcp = operate;
        }
        if (mode & 0x02) {
            at_wifi_config->dhcp_state.bit.ap_dhcp = operate;
        }
        if (at->store) {
            at_wifi_config_save(AT_CONFIG_KEY_WIFI_DHCP_STATE);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwdhcps(int argc, const char **argv)
{
    ip4_addr_t startaddr, stopaddr;
    char start_ip[16];
    char stop_ip[16];

    startaddr.addr = (at_wifi_config->ap_ip.ip & at_wifi_config->ap_ip.netmask) | (at_wifi_config->dhcp_server.start << 24);
    stopaddr.addr = (at_wifi_config->ap_ip.ip & at_wifi_config->ap_ip.netmask) | (at_wifi_config->dhcp_server.end << 24);
    strlcpy(start_ip, ip4addr_ntoa(&startaddr), sizeof(start_ip));
    strlcpy(stop_ip, ip4addr_ntoa(&stopaddr), sizeof(stop_ip));
    at_response_string("+CWDHCPS:%d,%s,%s", at_wifi_config->dhcp_server.lease_time, start_ip, stop_ip);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwdhcps(int argc, const char **argv)
{
    int enable;
    int lease_time;
    char start_ip[16];
    char stop_ip[16];
    int lease_time_valid = 0;
    int start_ip_valid = 0;
    int stop_ip_valid = 0;
    uint32_t startaddr, stopaddr;

    AT_CMD_PARSE_NUMBER(0, &enable);
    AT_CMD_PARSE_OPT_NUMBER(1, &lease_time, lease_time_valid);
    AT_CMD_PARSE_OPT_STRING(2, start_ip, sizeof(start_ip),  start_ip_valid);
    AT_CMD_PARSE_OPT_STRING(3, stop_ip, sizeof(stop_ip), stop_ip_valid);

    if ((enable == 0) && (lease_time_valid + start_ip_valid + stop_ip_valid) > 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if ((enable == 1) && (lease_time_valid + start_ip_valid + stop_ip_valid) < 3) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (enable == 0) {
        if (lease_time_valid + start_ip_valid + stop_ip_valid > 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        at_wifi_config->dhcp_server.lease_time = 120;
        at_wifi_config->dhcp_server.start = 2;
        at_wifi_config->dhcp_server.end = 101;
    } else if (enable == 1) {
        if (lease_time_valid + start_ip_valid + stop_ip_valid < 3) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (lease_time < 1 || lease_time > 2880) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (get_ip_from_string(start_ip, &startaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
        }
        if (get_ip_from_string(stop_ip, &stopaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_TYPE_MISMATCH);
        }
        if ((startaddr & at_wifi_config->ap_ip.netmask) != (at_wifi_config->ap_ip.ip & at_wifi_config->ap_ip.netmask)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
        }
        if ((stopaddr & at_wifi_config->ap_ip.netmask) != (at_wifi_config->ap_ip.ip & at_wifi_config->ap_ip.netmask)) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
        }

        at_wifi_config->dhcp_server.lease_time = lease_time;
        at_wifi_config->dhcp_server.start = (startaddr & (~at_wifi_config->ap_ip.netmask)) >> 24;
        at_wifi_config->dhcp_server.end = (stopaddr & (~at_wifi_config->ap_ip.netmask)) >> 24;
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_DHCP_SERVER);
    }
    at_wifi_ap_set_dhcp_range(at_wifi_config->dhcp_server.start, at_wifi_config->dhcp_server.end);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwautoconn(int argc, const char **argv)
{
    at_response_string("+CWAUTOCONN:%d", at_wifi_config->auto_conn);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwautoconn(int argc, const char **argv)
{
    int auto_conn = 0;

    AT_CMD_PARSE_NUMBER(0, &auto_conn);

    if (auto_conn == WIFI_AUTOCONN_DISABLE || auto_conn == WIFI_AUTOCONN_ENABLE) {
        at_wifi_config->auto_conn = (uint8_t)auto_conn;
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_AUTO_CONN);
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipstamac(int argc, const char **argv)
{
    at_response_string("+CIPSTAMAC:\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
            at_wifi_config->sta_mac.addr[0],
            at_wifi_config->sta_mac.addr[1],
            at_wifi_config->sta_mac.addr[2],
            at_wifi_config->sta_mac.addr[3],
            at_wifi_config->sta_mac.addr[4],
            at_wifi_config->sta_mac.addr[5]);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipstamac(int argc, const char **argv)
{
    char mac_string[18];
    uint8_t mac[6];

    AT_CMD_PARSE_STRING(0, mac_string, sizeof(mac_string));
    if (get_mac_from_string(mac_string, mac) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }

    if (mac[0]&0x01) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (mac[0] == 0x00 && mac[1] == 0x00 && mac[2] == 0x00 && mac[3] == 0x00 && mac[4] == 0x00 && mac[5] == 0x00) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF && mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    memcpy(at_wifi_config->sta_mac.addr, mac, 6);
    memcpy(at_wifi_config->ap_mac.addr, mac, 6);
#ifdef CONFIG_UNSINGLE_MAC_ADDR
    at_wifi_config->ap_mac.addr[0] ^= 0x2;
#endif
    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_MAC);
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_AP_MAC);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipapmac(int argc, const char **argv)
{
    at_response_string("+CIPAPMAC:\"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
            at_wifi_config->ap_mac.addr[0],
            at_wifi_config->ap_mac.addr[1],
            at_wifi_config->ap_mac.addr[2],
            at_wifi_config->ap_mac.addr[3],
            at_wifi_config->ap_mac.addr[4],
            at_wifi_config->ap_mac.addr[5]);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipapmac(int argc, const char **argv)
{
    char mac_string[18];
    uint8_t mac[6];

    AT_CMD_PARSE_STRING(0, mac_string, sizeof(mac_string));
    if (get_mac_from_string(mac_string, mac) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }

    if (mac[0]&0x01) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (mac[0] == 0x00 && mac[1] == 0x00 && mac[2] == 0x00 && mac[3] == 0x00 && mac[4] == 0x00 && mac[5] == 0x00) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    if (mac[0] == 0xFF && mac[1] == 0xFF && mac[2] == 0xFF && mac[3] == 0xFF && mac[4] == 0xFF && mac[5] == 0xFF) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    memcpy(at_wifi_config->ap_mac.addr, mac, 6);
    memcpy(at_wifi_config->sta_mac.addr, mac, 6);
#ifdef CONFIG_UNSINGLE_MAC_ADDR
    at_wifi_config->sta_mac.addr[0] ^= 0x2;
#endif
    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_AP_MAC);
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_MAC);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipsta(int argc, const char **argv)
{
    ip4_addr_t ipaddr = {0}, gwaddr = {0}, maskaddr = {0}, dns = {0};

    wifi_sta_ip4_addr_get(&ipaddr.addr, &maskaddr.addr, &gwaddr.addr, &dns.addr);
    at_response_string("+CIPSTA:%s:\"%s\"\r\n", "ip", ip4addr_ntoa(&ipaddr));
    at_response_string("+CIPSTA:%s:\"%s\"\r\n", "gateway", ip4addr_ntoa(&gwaddr));
    at_response_string("+CIPSTA:%s:\"%s\"\r\n", "netmask", ip4addr_ntoa(&maskaddr));
#if CONFIG_ATMODULE_NETWORK && LWIP_IPV6
    if (at_net_config->ipv6_enable) {
        struct netif *nif = (struct netif *)fhost_to_net_if(MGMR_VIF_STA);

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            const ip6_addr_t * ip6addr = netif_ip6_addr(nif, i);
            if (ip6_addr_isvalid(netif_ip6_addr_state(nif, i)) && ip6_addr_islinklocal(ip6addr)) {
                at_response_string("+CIPSTA:%s:\"%s\"\r\n", "ip6ll", ipaddr_ntoa(ip6addr));
            }
            if (ip6_addr_isvalid(netif_ip6_addr_state(nif, i)) && ip6_addr_isglobal(ip6addr)) {
                at_response_string("+CIPSTA:%s:\"%s\"\r\n", "ip6gl", ipaddr_ntoa(ip6addr));
            }
        }
    }

#endif
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipsta(int argc, const char **argv)
{
    char ip[16];
    char gateway[16];
    char netmask[16];
    int gateway_valid = 0;
    int netmask_valid = 0;
    uint32_t ipaddr, dnsaddr, gwaddr, maskaddr = IP_SET_ADDR(255, 255, 255, 0);
    int state;

    AT_CMD_PARSE_STRING(0, ip, sizeof(ip));
    AT_CMD_PARSE_OPT_STRING(1, gateway, sizeof(gateway), gateway_valid);
    AT_CMD_PARSE_OPT_STRING(2, netmask, sizeof(netmask), netmask_valid);

    if (get_ip_from_string(ip, &ipaddr) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }
    if (gateway_valid) {
        if (get_ip_from_string(gateway, &gwaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    } else {
        gwaddr = (ipaddr&maskaddr)|(0x01<<24);
    }
    if (netmask_valid) {
        if (get_ip_from_string(netmask, &maskaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    }

    wifi_sta_ip4_addr_get(NULL, NULL, NULL, &dnsaddr);

    at_wifi_config->sta_ip.ip = ipaddr;
    at_wifi_config->sta_ip.gateway = gwaddr;
    at_wifi_config->sta_ip.netmask = maskaddr;
    at_wifi_config->dhcp_state.bit.sta_dhcp = 0;
    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_IP);
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_DHCP_STATE);
    }

    //wifi_mgmr_state_get(&state);
    //if (state == WIFI_STATE_CONNECTED_IP_GETTING || state == WIFI_STATE_WITH_AP_CONNECTED_IP_GOT
    //    || state == WIFI_STATE_WITH_AP_CONNECTED_IP_GETTING || state == WIFI_STATE_WITH_AP_CONNECTED_IP_GOT) {
        wifi_mgmr_sta_ip_set(at_wifi_config->sta_ip.ip, at_wifi_config->sta_ip.netmask, at_wifi_config->sta_ip.gateway, 0);
    //    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);
    //}
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cipap(int argc, const char **argv)
{
    ip4_addr_t ipaddr, gwaddr, maskaddr;

    ipaddr.addr = at_wifi_config->ap_ip.ip;
    gwaddr.addr = at_wifi_config->ap_ip.gateway;
    maskaddr.addr = at_wifi_config->ap_ip.netmask;

    at_response_string("+CIPAP:%s:\"%s\"\r\n", "ip", ip4addr_ntoa(&ipaddr));
    at_response_string("+CIPAP:%s:\"%s\"\r\n", "gateway", ip4addr_ntoa(&gwaddr));
    at_response_string("+CIPAP:%s:\"%s\"\r\n", "netmask", ip4addr_ntoa(&maskaddr));
#if CONFIG_ATMODULE_NETWORK && LWIP_IPV6
    if (at_net_config->ipv6_enable) {
        struct netif *nif = (struct netif *)fhost_to_net_if(MGMR_VIF_AP);

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            const ip6_addr_t * ip6addr = netif_ip6_addr(nif, i);
            if (ip6_addr_isvalid(netif_ip6_addr_state(nif, i)) && ip6_addr_islinklocal(ip6addr)) {
                at_response_string("+CIPAP:%s:\"%s\"\r\n", "ip6ll", ipaddr_ntoa(ip6addr));
            }
        }
    }

#endif
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cipap(int argc, const char **argv)
{
    char ip[16];
    char gateway[16];
    char netmask[16];
    int gateway_valid = 0;
    int netmask_valid = 0;
    uint32_t ipaddr, gwaddr, maskaddr;

    AT_CMD_PARSE_STRING(0, ip, sizeof(ip));
    AT_CMD_PARSE_OPT_STRING(1, gateway, sizeof(gateway), gateway_valid);
    AT_CMD_PARSE_OPT_STRING(2, netmask, sizeof(netmask), netmask_valid);

    if (get_ip_from_string(ip, &ipaddr) != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
    }
    if (gateway_valid) {
        if (get_ip_from_string(gateway, &gwaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    } else {
        gwaddr = ipaddr;
    }
    if (netmask_valid) {
        if (get_ip_from_string(netmask, &maskaddr) != 0) {
            return AT_RESULT_WITH_SUB_CODE(AT_SUB_OP_ADDR_ERROR);
        }
    } else {
        maskaddr = IP_SET_ADDR(255, 255, 255, 0);
    }

    at_wifi_config->ap_ip.ip = ipaddr;
    at_wifi_config->ap_ip.gateway = gwaddr;
    at_wifi_config->ap_ip.netmask = maskaddr;
    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_AP_IP);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwapproto(int argc, const char **argv)
{
    at_response_string("+CWAPPROTO:%d", at_wifi_config->ap_proto.byte);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwapproto(int argc, const char **argv)
{
    int proto = 0;

    AT_CMD_PARSE_NUMBER(0, &proto);

    if (proto >= 0 && proto <= 0xf) {
        at_wifi_config->ap_proto.byte = (uint8_t)proto;
        at_wifi_mode_set(1, at_wifi_config->ap_proto);
        at_wifi_config->ap_proto = at_wifi_mode_get(1);
        if (at->store) {
            at_wifi_config_save(AT_CONFIG_KEY_WIFI_AP_PROTO);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwstaproto(int argc, const char **argv)
{
    at_response_string("+CWSTAPROTO:%d", at_wifi_config->sta_proto.byte);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwstaproto(int argc, const char **argv)
{
    int proto = 0;

    AT_CMD_PARSE_NUMBER(0, &proto);

    if (proto >= 0 && proto <= 0xf) {
        at_wifi_config->sta_proto.byte = (uint8_t)proto;
        at_wifi_mode_set(0, at_wifi_config->sta_proto);
        at_wifi_config->sta_proto = at_wifi_mode_get(0);
        if (at->store) {
            at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_PROTO);
        }
    } else {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    return AT_RESULT_CODE_OK;
}

int time_info_get(char *buffer, uint32_t buffer_len)
{
    struct timespec timespec = {0,0};
    struct tm *tm_info;
    int offset, len = 0;
#if CONFIG_ANTENNA_NETWORK
    at_net_sntp_gettime(&timespec);
#endif
    tm_info = localtime(&timespec.tv_sec);

    offset = strftime(buffer, buffer_len, "%Y-%m-%d:%H:%M:%S", tm_info);
    len += offset;

    len += snprintf(buffer + offset, buffer_len - offset, ".%03ld ", timespec.tv_nsec/1000000);
    return len;
}

static int g_min_pkg_len = 0, g_max_pkg_len = 0;
static void cb_sniffer(struct bl_frame_info *info, void *arg)
{
    int n, offset = 0;
    uint32_t buffer_len;
    uint8_t *buffer = NULL;

    if (info->payload != NULL) {
        struct mac_hdr *hdr = (struct mac_hdr *)info->payload;
        (void)hdr;
        //TRACE_APP(INF, "%pM %pM %pM %fc SN:%d length = %d", TR_MAC(hdr->addr1.array),
        //          TR_MAC(hdr->addr2.array), TR_MAC(hdr->addr3.array), hdr->fctl, hdr->seq >> 4,
        //          info->length);
        //fhost_printf("SN:%d length = %d\r\n", hdr->seq >> 4, info->length);
        if (g_min_pkg_len && info->length < g_min_pkg_len) {
            return;
        }

        if (g_max_pkg_len && info->length > g_max_pkg_len) {
            return;
        }
        buffer_len = info->length * 2 + 30;
        buffer = pvPortMalloc(buffer_len + 64);
        if (!buffer) {
            return;
        }

        n = time_info_get(buffer, 64);
        if (n > 0) {
            offset += n;
        }

        //n = snprintf(buffer, buffer_len, "\r\n+CWMONITOR,%d,%d:", phy_freq_to_channel(0, info->freq), info->length);

        for (int i = 0; i < info->length; i++) {
            n = snprintf(buffer + offset, buffer_len - offset, "%02x", info->payload[i]);
            if (n > 0) {
                offset += n;
            }
        }
        n = snprintf(buffer + offset, buffer_len - offset, "\r\n");
        if (n > 0) {
            offset += n;
        }

        AT_CMD_DATA_SEND(buffer, strlen(buffer));
        vPortFree(buffer);
    }
}

static int at_setup_cmd_cwmonitor(int argc, const char **argv)
{
    int channel = 0;
    int enable = 0;
    int min_pkg_len, max_pkg_len;
    int channel_valid = 0;
    int min_pkg_len_valid = 0;
    int max_pkg_len_valid = 0;

    AT_CMD_PARSE_NUMBER(0, &enable);
    AT_CMD_PARSE_OPT_NUMBER(1, &channel, channel_valid);
    AT_CMD_PARSE_OPT_NUMBER(2, &min_pkg_len, min_pkg_len_valid);
    AT_CMD_PARSE_OPT_NUMBER(3, &max_pkg_len, max_pkg_len_valid);

    if (enable < 0 || enable > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (enable == 0) {
        at_wifi_sniffer_stop();
        return AT_RESULT_CODE_OK;
    }

    if (!channel_valid) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_NUM_MISMATCH);
    }

    if (wifi_mgmr_channel_valid_check(channel)) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (min_pkg_len_valid) {
        g_min_pkg_len = min_pkg_len;
    } else {
        g_min_pkg_len = 0;
    }

    if (max_pkg_len_valid) {
        g_max_pkg_len = max_pkg_len;
    } else {
        g_max_pkg_len = 0;
    }

    at_wifi_sniffer_start();
    at_wifi_sniffer_set_channel(channel, cb_sniffer, NULL);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_wps(int argc, const char **argv)
{
    int enable = 0;
    int auth = 0;
    int auth_valid = 0;
    AT_CMD_PARSE_NUMBER(0, &enable);
    AT_CMD_PARSE_OPT_NUMBER(1, &auth, auth_valid);

    if ((at_wifi_config->wifi_mode != WIFI_STATION_MODE) && (at_wifi_config->wifi_mode != WIFI_AP_STA_MODE)) {
        printf("err at_wifi_config\r\n");
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_OP_ERROR);
    }

    if (enable != 0 && enable != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (enable == 0) {
        wifi_mgmr_sta_disconnect();
        return AT_RESULT_CODE_OK;
    }
    if (!auth_valid) {
        auth = 0;
    }

    int ret = wifi_mgmr_sta_wps_pbc(auth);
    if (ret != 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_CMD_EXEC_FAIL);
    }

    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_mdns(int argc, const char **argv)
{
    return AT_RESULT_WITH_SUB_CODE(AT_SUB_UNSUPPORT_CMD);
}

static int at_query_cmd_cwhostname(int argc, const char **argv)
{
#if LWIP_NETIF_HOSTNAME 
#include <lwip/netif.h>
    struct netif *netif = (struct netif *)net_if_find_from_name("wl1");

    at_response_string("+CWHOSTNAME:%s\r\n", netif_get_hostname(netif));
#endif
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwhostname(int argc, const char **argv)
{
    struct netif *netif;
    char hostname[28];

    AT_CMD_PARSE_STRING(0, hostname, sizeof(hostname));

    strlcpy(at_wifi_config->hostname, hostname, sizeof(at_wifi_config->hostname));
    at_wifi_hostname_set(at_wifi_config->hostname);

    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_HOSTNAME);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwcountry(int argc, const char **argv)
{
    char country_code_string[5] = {0};
    wifi_mgmr_get_country_code(country_code_string);
    at_response_string("+CWCOUNTRY:%d,\"%s\"\r\n",
            at_wifi_config->wifi_country.country_policy,
            country_code_string);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwcountry(int argc, const char **argv)
{
    char code[5] = {0};
    char *country_code_string[WIFI_COUNTRY_CODE_MAX] = AT_WIFI_COUNTRY_CODE;
    int country_policy;
    int country_code;

    AT_CMD_PARSE_NUMBER(0, &country_policy);
    AT_CMD_PARSE_STRING(1, code, sizeof(code));

    if (country_policy != 0 && country_policy != 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    for (country_code=0; country_code<WIFI_COUNTRY_CODE_MAX; country_code++) {
        if (strcasecmp(code, country_code_string[country_code]) == 0) {
            break;
        }
    }
    if (country_code >= WIFI_COUNTRY_CODE_MAX) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    at_wifi_config->wifi_country.country_policy = (uint8_t)country_policy;
    at_wifi_config->wifi_country.country_code = (uint8_t)country_code;
    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_COUNTRY_CODE);
    }
    wifi_mgmr_set_country_code(code);
    return AT_RESULT_CODE_OK;
}
#endif

static int at_setup_cmd_cwevt(int argc, const char **argv)
{
    int enable;
    AT_CMD_PARSE_NUMBER(0, &enable);
    if (enable < 0 || enable > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    at_wifi_config->wevt_enable = enable;
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwevt(int argc, const char **argv)
{
    int enable;
    at_response_string("+CWEVT:%d\r\n", at_wifi_config->wevt_enable);
    return AT_RESULT_CODE_OK;
}

#if CONFIG_ATMODULE_ANTENNA_CTL
static int at_setup_cmd_cwantenable(int argc, const char **argv)
{
    int dynamic_enable, static_enable;
    int pin_is_vaild = 0, pin = 0;

    AT_CMD_PARSE_NUMBER(0, &dynamic_enable);
    AT_CMD_PARSE_NUMBER(1, &static_enable);
    AT_CMD_PARSE_OPT_NUMBER(2, &pin, pin_is_vaild);

    if (dynamic_enable < 0 || dynamic_enable > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if (static_enable < 0 || static_enable > 1) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }

    if ((dynamic_enable || static_enable) && pin_is_vaild == 0) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    at_wifi_config->ant_div.static_ant_div_enable = static_enable;
    at_wifi_config->ant_div.dynamic_ant_div_enable = dynamic_enable;
    at_wifi_config->ant_div.ant_div_pin = pin;

    if (at->store) {
        at_wifi_config_save(AT_CONFIG_KEY_WIFI_ANTDIV);
    }
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwantenable(int argc, const char **argv)
{
    at_response_string("+CWANTENABLE:%d,%d,%d\r\n", antenna_hal_is_dynamic_div_enabled(), antenna_hal_is_static_div_enabled(), at_wifi_config->ant_div.ant_div_pin);
    return AT_RESULT_CODE_OK;
}

static int at_setup_cmd_cwant(int argc, const char **argv)
{
    int ant_id;
    AT_CMD_PARSE_NUMBER(0, &ant_id);

    if (ant_id < 0 || ant_id > antenna_hal_get_antenna_count()) {
        return AT_RESULT_WITH_SUB_CODE(AT_SUB_PARA_VALUE_INVALID);
    }
    antenna_hal_switch_antenna(ant_id);

    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwant(int argc, const char **argv)
{
    at_response_string("+CWANT:%d\r\n", antenna_hal_get_current_antenna());
    return AT_RESULT_CODE_OK;
}

static int at_query_cmd_cwantnum(int argc, const char **argv)
{
    at_response_string("+CWANTNUM:%d\r\n", antenna_hal_get_antenna_count());

    return AT_RESULT_CODE_OK;
}
#endif

static int at_query_cmd_cwnetmode(int argc, const char **argv)
{
    // 0: RCP mode
    // 1: NCP mode
    at_response_string("+CWNETMODE:%d\r\n", at_port_netmode_get());
    return AT_RESULT_CODE_OK;
}

static const at_cmd_struct at_wifi_cmd[] = {
    //{"+WIFISP",       NULL, at_query_cmd_wifisp,      at_setup_cmd_wifisp,       NULL,                    1, 1},
    {"+CWMODE",       NULL, at_query_cmd_cwmode,      at_setup_cmd_cwmode,       NULL,                    1, 2},
    {"+CWSTATE",      NULL, at_query_cmd_cwstate,     NULL,                      NULL,                    0, 0},
    {"+CWJAP",        NULL, at_query_cmd_cwjap,       at_setup_cmd_cwjap,        at_exe_cmd_cwjap,        2, 7},
    {"+CWRECONNCFG",  NULL, at_query_cmd_cwreconncfg, at_setup_cmd_cwreconncfg,  NULL,                    2, 2},
    {"+CWLAPOPT",     NULL, at_query_cmd_cwlapopt,    at_setup_cmd_cwlapopt,     NULL,                    2, 5},
    {"+CWLAP",        NULL, NULL,                     at_setup_cmd_cwlap,        at_exe_cmd_cwlap,        1, 6},
    {"+CWQAP",        NULL, NULL,                     at_setup_cmd_cwqap,        at_exe_cmd_cwqap,        0, 1},
    {"+CWAUTOCONN",   NULL, at_query_cmd_cwautoconn,  at_setup_cmd_cwautoconn,   NULL,                    1, 1},
    {"+CWSTAPROTO",   NULL, at_query_cmd_cwstaproto,  at_setup_cmd_cwstaproto,   NULL,                    1, 1},
    {"+CIPSTAMAC",    NULL, at_query_cmd_cipstamac,   at_setup_cmd_cipstamac,    NULL,                    1, 1},
    {"+CIPSTA",       NULL, at_query_cmd_cipsta,      at_setup_cmd_cipsta,       NULL,                    1, 3},
    {"+CWMONITOR",    NULL, NULL,                     at_setup_cmd_cwmonitor,    NULL,                    1, 4},
    //{"+WPS",          NULL, NULL,                     at_setup_cmd_wps,          NULL,                    1, 2},
#if (!CONFIG_RCP_ENABLE)
    {"+CWSAP",        NULL, at_query_cmd_cwsap,       at_setup_cmd_cwsap,        NULL,                    4, 6},
    {"+CWLIF",        NULL, NULL,                     NULL,                      at_exe_cmd_cwlif,        0, 0},
    {"+CWQIF",        NULL, NULL,                     at_setup_cmd_cwqif,        at_exe_cmd_cwqif,        1, 1},
    {"+CWAPPROTO",    NULL, at_query_cmd_cwapproto,   at_setup_cmd_cwapproto,    NULL,                    1, 1},
    {"+CIPAPMAC",     NULL, at_query_cmd_cipapmac,    at_setup_cmd_cipapmac,     NULL,                    1, 1},
    {"+CIPAP",        NULL, at_query_cmd_cipap,       at_setup_cmd_cipap,        NULL,                    1, 3},
    {"+CWDHCP",       NULL, at_query_cmd_cwdhcp,      at_setup_cmd_cwdhcp,       NULL,                    2, 2},
    {"+CWDHCPS",      NULL, at_query_cmd_cwdhcps,     at_setup_cmd_cwdhcps,      NULL,                    1, 4},
    {"+MDNS",         NULL, NULL,                     at_setup_cmd_mdns,         NULL,                    1, 4},
    {"+CWHOSTNAME",   NULL, at_query_cmd_cwhostname,  at_setup_cmd_cwhostname,   NULL,                    1, 1},
#endif
    {"+CWCOUNTRY",    NULL, at_query_cmd_cwcountry,   at_setup_cmd_cwcountry,    NULL,                    2, 2},
    {"+CWEVT",        NULL, at_query_cmd_cwevt,       at_setup_cmd_cwevt,        NULL,                    1, 1},
    {"+CWNETMODE",    NULL, at_query_cmd_cwnetmode,   NULL,                      NULL,                    1, 1},
#if CONFIG_ATMODULE_ANTENNA_CTL
    {"+CWANTENABLE",  NULL, at_query_cmd_cwantenable, at_setup_cmd_cwantenable,  NULL,                    2, 3},
    {"+CWANTNUM",     NULL, at_query_cmd_cwantnum,    NULL,                      NULL,                    0, 0},
    {"+CWANT",        NULL, at_query_cmd_cwant,       at_setup_cmd_cwant,        NULL,                    1, 1},
#endif
};

bool at_wifi_cmd_regist(void)
{
    at_wifi_config_init();

#if CONFIG_ATMODULE_ANTENNA_CTL
    board_antenna_init(at_wifi_config->ant_div.dynamic_ant_div_enable, at_wifi_config->ant_div.static_ant_div_enable, at_wifi_config->ant_div.ant_div_pin);
#endif
    at_wifi_start();

    at_register_function(at_wifi_config_default, at_wifi_stop);

    if (at_cmd_register(at_wifi_cmd, sizeof(at_wifi_cmd) / sizeof(at_wifi_cmd[0])) != 0) {
        return false;
    }

    return true;
}
