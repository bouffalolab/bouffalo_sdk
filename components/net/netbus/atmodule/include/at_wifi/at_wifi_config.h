/**
  ******************************************************************************
  * @file    at_wifi_config.h
  * @version V1.0
  * @date
  * @brief   This file is part of AT command framework
  ******************************************************************************
  */

#ifndef AT_WIFI_CONFIG_H
#define AT_WIFI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define AT_CONFIG_KEY_WIFI_MODE             "WIFIMODE"
#define AT_CONFIG_KEY_WIFI_STA_INFO         "WIFISTA"
#define AT_CONFIG_KEY_WIFI_RECONN_CFG       "RECONN"
#define AT_CONFIG_KEY_WIFI_AP_INFO          "WIFIAP"
#define AT_CONFIG_KEY_WIFI_DHCP_STATE       "DHCP"
#define AT_CONFIG_KEY_WIFI_DHCP_SERVER      "DHCPSERVER"
#define AT_CONFIG_KEY_WIFI_AUTO_CONN        "WIFIAUTOCONN"
#define AT_CONFIG_KEY_WIFI_AP_MAC           "WIFIAPMAC"
#define AT_CONFIG_KEY_WIFI_STA_MAC          "WIFISTAMAC"
#define AT_CONFIG_KEY_WIFI_AP_PROTO         "WIFIAPPROTO"
#define AT_CONFIG_KEY_WIFI_STA_PROTO        "WIFISTAPROTO"
#define AT_CONFIG_KEY_WIFI_AP_IP            "WIFIAPIP"
#define AT_CONFIG_KEY_WIFI_STA_IP           "WIFISTAIP"
#define AT_CONFIG_KEY_WIFI_COUNTRY_CODE     "WIFICOUNTRY"
#define AT_CONFIG_KEY_WIFI_HOSTNAME         "WIFIHOSTNAME"
#define AT_CONFIG_KEY_WIFI_LAPOPT           "WIFILAPOPT"
#define AT_CONFIG_KEY_WIFI_ANTDIV           "ANTDIV"
#define AT_CONFIG_KEY_WIFI_NETMODE          "NETMODE"

#define AT_WIFI_COUNTRY_CODE                {"CN", "JP", "US", "EU", "00"}

#define IP_SET_ADDR(a,b,c,d) (((uint32_t)((a) & 0xff)) | \
                                            ((uint32_t)((b) & 0xff) << 8) | \
                                            ((uint32_t)((c) & 0xff) << 16) | \
                                            ((uint32_t)((d) & 0xff) << 24))

typedef enum {
    WIFI_DISABLE = 0,
    WIFI_STATION_MODE,
    WIFI_SOFTAP_MODE,
    WIFI_AP_STA_MODE,
} wifi_work_mode;

typedef struct {
    uint8_t addr[6];
} wifi_mac_addr;

typedef struct {
    char ssid[33];
    char psk[65];
    char pmk[66];
    uint16_t freq;
    uint8_t bssid[6];
    uint8_t listen_interval;
    uint8_t scan_mode;
    uint8_t pmf;
    uint8_t wep_en;
    uint8_t store;
    uint16_t jap_timeout;
} wifi_sta_info;

typedef struct {
    char ssid[33];
    char pwd[65];
    uint8_t channel;
    uint8_t ecn;
    uint8_t max_conn;
    uint8_t ssid_hidden;
} wifi_ap_info;

typedef struct {
    uint8_t sort_enable;
    int8_t rssi_filter;
    uint8_t max_count;
    uint16_t print_mask;
    uint16_t authmode_mask;
} wifi_scan_option;

typedef struct {
    uint16_t interval_second;
    uint16_t repeat_count;
} wifi_sta_reconnect;

typedef union {
    struct
    {
        uint8_t sta_dhcp:1;
        uint8_t ap_dhcp:1;
    } bit;
    uint8_t byte;
} wifi_dhcp_state;

typedef struct {
    uint16_t lease_time;
    uint8_t start;
    uint8_t end;
} wifi_dhcp_server;

typedef enum {
    WIFI_AUTOCONN_DISABLE = 0,
    WIFI_AUTOCONN_ENABLE,
} wifi_auto_conn;

typedef union {
    struct
    {
        uint8_t b_mode:1;
        uint8_t g_mode:1;
        uint8_t n_mode:1;
        uint8_t ax_mode:1;
    } bit;
    uint8_t byte;
} wifi_proto;

typedef struct {
    uint32_t ip;
    uint32_t gateway;
    uint32_t netmask;
} wifi_ip;

typedef enum {
    WIFI_COUNTRY_CODE_CN = 0,
    WIFI_COUNTRY_CODE_JP,
    WIFI_COUNTRY_CODE_US,
    WIFI_COUNTRY_CODE_EU,
    WIFI_COUNTRY_CODE_WORLD,
    WIFI_COUNTRY_CODE_MAX
} country_code_type;

typedef struct {
    uint8_t country_policy;
    uint8_t country_code;
} wifi_country_code;

typedef struct {
    uint8_t  static_ant_div_enable;
    uint8_t  dynamic_ant_div_enable;
    uint8_t  ant_div_pin;
} wifi_ant_div;


typedef struct {
    wifi_work_mode wifi_mode;
    wifi_auto_conn switch_mode_auto_conn;
    wifi_mac_addr ap_mac;
    wifi_mac_addr sta_mac;
    wifi_sta_info sta_info;
    wifi_sta_reconnect reconn_cfg;
    wifi_scan_option scan_option;
    wifi_ap_info ap_info;
    wifi_dhcp_state dhcp_state;
    wifi_dhcp_server dhcp_server;
    wifi_auto_conn auto_conn;
    wifi_proto ap_proto;
    wifi_proto sta_proto;
    wifi_ip ap_ip;
    wifi_ip sta_ip;
    wifi_country_code wifi_country;
    char hostname[28];
    uint8_t  reconnect_state;
    uint8_t  connecting_state;
    uint8_t  wlan_disable;
    uint8_t  wevt_enable;
    wifi_ant_div ant_div;
    uint8_t  netmode;
}wifi_config;

extern wifi_config *at_wifi_config;

int at_wifi_config_init(void);

int at_wifi_config_save(const char *key);

int at_wifi_config_default(void);

#ifdef __cplusplus
}
#endif

#endif/* AT_WIFI_CONFIG_H */

