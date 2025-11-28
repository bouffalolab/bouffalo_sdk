/**
  ******************************************************************************
  * @file    at_wifi_main.c
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
#include <task.h>
#include <queue.h>

#include <lwip/tcpip.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/udp.h>
#include <lwip/altcp.h>
#include <lwip/altcp_tcp.h>
#include <lwip/altcp_tls.h>
#include <lwip/err.h>
#include <lwip/netif.h>
#include <lwip/netifapi.h>

#include "bl_fw_api.h"
#include <wifi_mgmr.h>
#include <wifi_mgmr_ext.h>
//#include <wifi_mgmr_cli.h>
#if (CONFIG_FHOST)
#include "fhost.h"
#endif
#include "at_main.h"
#include "at_core.h"
#include "at_port.h"
#include "at_base_config.h"
#include "at_wifi_config.h"
#include "at_wifi_main.h"
#include "at_net_main.h"
#if CONFIG_ATMODULE_NETWORK
#include "at_net_config.h"
#endif
#define DBG_TAG "MAIN"
#include "log.h"

#define AT_WIFI_MAIN_PRINTF AT_CMD_PRINTF

#define AT_WIFI_SUPPORT_STORE_CHANNEL

#define AT_WIFI_MAX_STA_NUM (CFG_STA_MAX)

static void wifi_ap_update_sta_ip(uint8_t mac[6], uint32_t ip);

//static wifi_interface_t g_wifi_sta_interface = NULL;
//static wifi_interface_t g_wifi_ap_interface = NULL;
static int g_wifi_sta_is_connected = 0;
static int g_wifi_sta_use_dhcp = 0;
static int g_wifi_sta_disconnect_reason = 0;
static int g_wifi_sniffer_is_start = 0;
static int g_wifi_ap_is_start = 0;
static int g_wifi_reconnect_disable = 0;

/** Mac address length  */
#define DHCP_MAX_HLEN               6
struct dhcp_client_node
{
    struct dhcp_client_node *next;
    uint8_t chaddr[DHCP_MAX_HLEN];
    ip4_addr_t ipaddr;
    uint32_t lease_end;
};

struct wifi_ap_sta_info
{
    long long valid_time;
    uint8_t mac[6];
    uint32_t ip;
    uint8_t sta_index;
};

struct wifi_ap_sta_info g_wifi_ap_sta_info[AT_WIFI_MAX_STA_NUM];

static int wifi_ap_get_sta_ip(uint8_t mac[6], uint32_t *ip);

/* todo: wifi_mgmr_ext.c */
int wifi_mgmr_sta_disconnect(void)
{
    // Defensive: check WiFi state if possible
    return wifi_sta_disconnect();
}

static void wifiopt_sta_disconnect(int force)
{
    g_wifi_reconnect_disable = force;
    wifi_mgmr_sta_autoconnect_disable();
    wifi_mgmr_sta_disconnect();
    vTaskDelay(500);
}

int at_wifi_sta_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns)
{
    struct netif *netif = at_wifi_netif_get(0);
    if (!wifi_mgmr_sta_state_get()) {
      return -1;
    }

    if (addr) {
        *addr = ip4_addr_get_u32(&netif->ip_addr);
    }
    if (mask) {
        *mask = ip4_addr_get_u32(&netif->netmask);
    }
    if (gw) {
        *gw   = ip4_addr_get_u32(&netif->gw);
    }
    if (dns) {
        ip_addr_t *dns_addr = dns_getserver(0);
        *dns  = ip4_addr_get_u32(dns_addr);
    }

    return 0;
}

void wifiopt_sta_connect(void)
{

#if CONFIG_ATMODULE_NETWORK
    ip_addr_t ip_zero;
    ip_addr_set_zero(&ip_zero);


    dns_setserver(0, &ip_zero);
    dns_setserver(1, &ip_zero);
    dns_setserver(2, &ip_zero);
#endif
    if (!at_wifi_config) {
        AT_WIFI_MAIN_PRINTF("[WIFI_MAIN] Error: at_wifi_config is NULL\r\n");
        return;
    }
    char *ssid = at_wifi_config->sta_info.ssid;
    char *psk = at_wifi_config->sta_info.psk;

    int pmf_cfg = 1;

    char *pmk = at_wifi_config->sta_info.pmk;
    uint16_t freq = at_wifi_config->sta_info.freq;
    uint8_t *bssid = at_wifi_config->sta_info.bssid;
    uint16_t listen_interval = (uint16_t)at_wifi_config->sta_info.listen_interval;
    uint32_t flags = 0;
    char *hostname = at_wifi_config->hostname;
    int dhcp_en = (int)at_wifi_config->dhcp_state.bit.sta_dhcp;
    uint32_t ip = at_wifi_config->sta_ip.ip;
    uint32_t mask = at_wifi_config->sta_ip.netmask;
    uint32_t gateway = at_wifi_config->sta_ip.gateway;
    const uint8_t invalid_mac[6] = {0, 0, 0, 0, 0, 0};
    char bssid_str[20] = {0};
    //struct ap_connect_adv ext_param;

    if (strlen(ssid) <= 0) {
        AT_WIFI_MAIN_PRINTF("[WIFI_MAIN] Error: ssid is NULL or empty\r\n");
        return;
    }

    if (memcmp(bssid, invalid_mac, sizeof(invalid_mac)) == 0) {
        bssid = NULL;
    } else {
        snprintf(bssid_str, sizeof(bssid_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        bssid = bssid_str;
    }

    if (!dhcp_en) {
        wifi_mgmr_sta_ip_set(ip, mask, gateway, 0);
    }

#if (CONFIG_IPV6 && CONFIG_ATMODULE_NETWORK)
    if (at_net_config->ipv6_enable) {
        wifi_sta_ipv6_enable(1);
    }
#endif
    at_wifi_hostname_set(hostname);
    at_wifi_config->connecting_state = 1;

    if (at_wifi_config->wevt_enable) {
        at_response_string("+CW:CONNECTING\r\n");
    }
#if CONFIG_ATMODULE_ANTENNA_CTL
    if (antenna_hal_is_static_div_enabled()) {
        return wifi_sta_antenna_connect(ssid, psk, bssid, at_wifi_config->sta_info.wep_en?"WEP":NULL, pmf_cfg, freq, freq, dhcp_en);
    }
#endif
    wifi_sta_connect(ssid, psk, bssid, at_wifi_config->sta_info.wep_en?"WEP":NULL, pmf_cfg, freq, freq, dhcp_en);
}

static int wifiopt_ap_stop(int force)
{
#if (!CONFIG_ATMODULE_NANO)
    g_wifi_ap_is_start = 0;
    dhcpd_stop_with_netif(at_wifi_netif_get(AT_WIFI_VIF_AP));
    wifi_mgmr_ap_stop();
#endif
    return 0;
}

#if (!CONFIG_ATMODULE_NANO)

/* todo: */

int at_wifi_state_get(void)
{
    return fhost_get_vif_state(MGMR_VIF_STA);
}

static void reconnect_event(void *arg)
{
    TaskHandle_t *ptask = (TaskHandle_t *)arg;
    uint32_t interval;
    uint16_t repeat_count;
    int state;

    vTaskDelay(pdMS_TO_TICKS(2000));
    for (repeat_count = 0; repeat_count < at_wifi_config->reconn_cfg.repeat_count;) {

        interval = (at_wifi_config->reconn_cfg.interval_second > 0 ) ? at_wifi_config->reconn_cfg.interval_second * 1000 : 5000;
        vTaskDelay(pdMS_TO_TICKS(interval));

        state = at_wifi_state_get();
        if ((at_wifi_config->wifi_mode != WIFI_STATION_MODE && at_wifi_config->wifi_mode != WIFI_AP_STA_MODE)  ||
            state == FHOST_STA_CONNECTED ||
            (!at_wifi_config->reconn_cfg.repeat_count)) {
            break;
        }
        if (g_wifi_reconnect_disable) {
            g_wifi_reconnect_disable = 0;
            break;
        }
        AT_WIFI_MAIN_PRINTF("xxxxxxxxxxxxx %d %d\r\n", state, wifi_mgmr_sta_state_get());
        if (state == FHOST_STA_DISCONNECTED) {
            repeat_count++;
            printf("reconnect_event interval:%d cfg.repeat_count:%d repeat_count:%d\r\n", interval, at_wifi_config->reconn_cfg.repeat_count, repeat_count);
            at_wifi_config->reconnect_state = 1;
            wifiopt_sta_connect();
        }
    }
    at_wifi_config->reconnect_state = 0;
    *ptask = NULL;
    vTaskDelete(NULL);
}

static void wifi_sta_enable_reconnect(int enable)
{
    static TaskHandle_t task = NULL;

    if ((at_wifi_config->reconn_cfg.interval_second > 0) && (at_wifi_config->reconn_cfg.repeat_count > 0)) {
        g_wifi_reconnect_disable = 0;
        if (!task) {
            xTaskCreate(reconnect_event, (char*)"reconnect", 256, &task, 15, &task);
        }
    }
}

static void _wifi_ap_status_callback(struct netif *netif)
{
    uint32_t ipaddr;
    if (wifi_ap_get_sta_ip((uint8_t *)netif->hwaddr, &ipaddr) == 0) {
        if (ipaddr == ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr))) {
            AT_WIFI_MAIN_PRINTF("skip %s\r\n", ipaddr_ntoa(netif_ip4_addr(netif)));
            return;
        }
    }

    wifi_ap_update_sta_ip((uint8_t *)netif->hwaddr, ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)));

    if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
        ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
       ) {
        if (at_wifi_config->wevt_enable) {
            at_response_string("+CW:DIST_STA_IP \"%02x:%02x:%02x:%02x:%02x:%02x\",\"%s\"\r\n",
                    netif->hwaddr[0],
                    netif->hwaddr[1],
                    netif->hwaddr[2],
                    netif->hwaddr[3],
                    netif->hwaddr[4],
                    netif->hwaddr[5],
                    ip4addr_ntoa(&netif->ip_addr));
        }
    }
}

static int wifi_ap_start(void)
{
    wifi_mgmr_ap_params_t config = {0};

    if (at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
        int sta_channel;
        if (wifi_mgmr_sta_channel_get(&sta_channel) == 0) {
            at_wifi_config->ap_info.channel = sta_channel;
            AT_WIFI_MAIN_PRINTF("Set AP channel to %d\r\n", sta_channel);
        }
    }

    config.ssid = at_wifi_config->ap_info.ssid;
    config.key = at_wifi_config->ap_info.pwd;
    config.hidden_ssid = at_wifi_config->ap_info.ssid_hidden;
    config.channel = at_wifi_config->ap_info.channel;
    config.use_dhcpd = at_wifi_config->dhcp_state.bit.ap_dhcp;
    config.start = at_wifi_config->dhcp_server.start;
    config.limit = at_wifi_config->dhcp_server.end - at_wifi_config->dhcp_server.start;
    config.ap_ipaddr = at_wifi_config->ap_ip.ip;
    config.ap_mask = at_wifi_config->ap_ip.netmask;
    if (at_wifi_config->ap_info.ecn == AT_WIFI_ENC_OPEN) {
        config.akm = NULL;
    } else if (at_wifi_config->ap_info.ecn == AT_WIFI_ENC_WPA_PSK) {
        config.akm = "WPA";
    } else if (at_wifi_config->ap_info.ecn == AT_WIFI_ENC_WPA2_PSK) {
        config.akm = "WPA2";
    } else if (at_wifi_config->ap_info.ecn == AT_WIFI_ENC_WPA3_PSK) {
        config.akm = "WPA3";
    }

    struct netif *netif = at_wifi_netif_get(AT_WIFI_VIF_AP);

    int ret = wifi_mgmr_ap_start(&config);
    if (ret != 0) {
        return ret;
    }
    wifi_mgmr_conf_max_sta(at_wifi_config->ap_info.max_conn);
    vTaskDelay(100);
    //dhcpd_status_callback_set(netif, _wifi_ap_status_callback);
    /* Set STA netif as default netif */
    netifapi_netif_set_default((struct netif *)at_wifi_netif_get(AT_WIFI_VIF_STA));
    g_wifi_ap_is_start = 1;

    return 0;
}

static int wifi_ap_get_sta_info_index(uint8_t mac[6])
{
    int i, index = 0;
    long long oldest_time = 0;

    /* find exist record */
    for (i = 0; i < AT_WIFI_MAX_STA_NUM; i++) {
        if (g_wifi_ap_sta_info[i].valid_time != 0 && memcmp(g_wifi_ap_sta_info[i].mac, mac, 6) == 0) {
            return i;
        }
    }

    /* find empty record */
    for (i = 0; i < AT_WIFI_MAX_STA_NUM; i++) {
        if (g_wifi_ap_sta_info[i].valid_time == 0) {
            return i;
        }
    }

    /* find oldest record */
    oldest_time = at_current_ms_get();
    for (i = 0; i < AT_WIFI_MAX_STA_NUM; i++) {
        if (g_wifi_ap_sta_info[i].valid_time < oldest_time) {
            oldest_time = g_wifi_ap_sta_info[i].valid_time;
            index = i;
        }
    }
    return index;
}

static void wifi_ap_update_sta_ip(uint8_t mac[6], uint32_t ip)
{
    int index = wifi_ap_get_sta_info_index(mac);

    if (index < 0 || index >= AT_WIFI_MAX_STA_NUM) {
        AT_WIFI_MAIN_PRINTF("[WIFI_MAIN] Error: invalid index %d\r\n", index);
        return;
    }

    g_wifi_ap_sta_info[index].valid_time = at_current_ms_get();
    memcpy(g_wifi_ap_sta_info[index].mac, mac, 6);
    g_wifi_ap_sta_info[index].ip = ip;
}

static int wifi_ap_update_sta_index()
{
    int idx = -1;
    struct wifi_sta_basic_info sta_info = {0};

    for(int i = 0; i < CFG_STA_MAX && i < AT_WIFI_MAX_STA_NUM; i++) {
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if(!sta_info.is_used || (sta_info.sta_idx == 0xef)) {
            continue;
        }
        if (memcmp(g_wifi_ap_sta_info[i].mac, sta_info.sta_mac, 6) != 0) {
            idx = i;
            g_wifi_ap_sta_info[i].valid_time = at_current_ms_get();
            memcpy(g_wifi_ap_sta_info[i].mac, sta_info.sta_mac, 6);
            g_wifi_ap_sta_info[i].sta_index = sta_info.sta_idx;
            AT_WIFI_MAIN_PRINTF("update sta add idx:%d\r\n", idx);
            break;
        }
    }
    return idx;
}

static void wifi_ap_delete_sta_info(uint8_t mac[6])
{
    struct wifi_sta_basic_info sta_info = {0};

    for(int i = 0; i < CFG_STA_MAX && i < AT_WIFI_MAX_STA_NUM; i++) {
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if(!sta_info.is_used || (sta_info.sta_idx == 0xef)) {
            if (g_wifi_ap_sta_info[i].valid_time) {
                memcpy(mac, g_wifi_ap_sta_info[i].mac, 6);
                memset(&g_wifi_ap_sta_info[i], 0, sizeof(g_wifi_ap_sta_info[i]));
                AT_WIFI_MAIN_PRINTF("delete sta idx:%d\r\n", i);
                break;
            }
        }
    }
}

static int wifi_ap_get_sta_ip(uint8_t mac[6], uint32_t *ip)
{
    int i;

    for (i = 0; i < AT_WIFI_MAX_STA_NUM; i++) {
        if (g_wifi_ap_sta_info[i].valid_time != 0 && memcmp(g_wifi_ap_sta_info[i].mac, mac, 6) == 0) {
            *ip = g_wifi_ap_sta_info[i].ip;
            return 0;
        }
    }
    return -1;
}

static void wifi_sniffer_data_recv(void *env, uint8_t *pkt, int pkt_len)
{
}

int at_wifi_ap_set_dhcp_range(int start, int end)
{
    return 0;
}

int at_wifi_mode_set(uint8_t ap_or_sta, wifi_proto proto)
{
    int mode = 0;

    if (proto.bit.b_mode) {
        mode |= WIFI_MODE_802_11B;
    }

    if (proto.bit.g_mode) {
        mode |= WIFI_MODE_802_11B;
        mode |= WIFI_MODE_802_11G;
    }

    if (proto.bit.n_mode) {
        mode |= WIFI_MODE_802_11B;
        mode |= WIFI_MODE_802_11G;
        mode |= WIFI_MODE_802_11N_2_4;
    }

    if (proto.bit.ax_mode) {
        mode |= WIFI_MODE_802_11B;
        mode |= WIFI_MODE_802_11G;
        mode |= WIFI_MODE_802_11N_2_4;
        mode |= WIFI_MODE_802_11AX_2_4;
    }

    if (wifi_mgmr_set_mode(ap_or_sta, mode) != 0) {
        AT_WIFI_MAIN_PRINTF("set mode error\r\n");
    }
    return 0;
}

wifi_proto at_wifi_mode_get(uint8_t ap_or_sta)
{
    wifi_proto proto = {0};
    int mode;
    mode = wifi_mgmr_get_mode(ap_or_sta);

    if (WIFI_MODE_802_11B & mode) {
        proto.bit.b_mode = 1;
    }

    if (WIFI_MODE_802_11G & mode) {
        proto.bit.g_mode = 1;
    }

    if (WIFI_MODE_802_11N_2_4 & mode) {
        proto.bit.n_mode = 1;
    }

    if (WIFI_MODE_802_11AX_2_4 & mode) {
        proto.bit.ax_mode = 1;
    }
    return proto;
}

int at_wifi_sniffer_start(void)
{
    if (g_wifi_sniffer_is_start) {
        return -1;
    }

    wifiopt_ap_stop(0);
    wifiopt_sta_disconnect(1);

    g_wifi_sniffer_is_start = 1;
    return 0;
}

int at_wifi_sniffer_set_channel(int channel, void *cb, void *arg)
{
    wifi_mgmr_sniffer_item_t sniffer_item;

    if (!g_wifi_sniffer_is_start) {
        return -1;
    }
    memset(&sniffer_item, 0, sizeof(sniffer_item));

    sniffer_item.itf = "wl1";
    sniffer_item.prim20_freq = phy_channel_to_freq(0, channel);
    sniffer_item.cb = cb;
    sniffer_item.cb_arg = arg;

    AT_WIFI_MAIN_PRINTF("set channel %d\r\n", channel);
    wifi_mgmr_sniffer_enable(sniffer_item);
    return 0;
}

int at_wifi_sniffer_stop(void)
{
    wifi_mgmr_sniffer_item_t sniffer_item;

    if (!g_wifi_sniffer_is_start) {
        return -1;
    }
    memset(&sniffer_item, 0, sizeof(sniffer_item));
    sniffer_item.itf = "wl1";

    wifi_mgmr_sniffer_disable(sniffer_item);

    g_wifi_sniffer_is_start = 0;
    return 0;
}
#endif

static int reason_code_get(int code)
{
    return code;
}

void at_wifi_event_notify(void *private_data, uint32_t code)
{
    switch (code) {
        case AT_WIFI_EVENT_INIT_DONE: {
            LOG_I("[APP] [EVT], CODE_WIFI_ON_MGMR_DONE\r\n");

#if (!CONFIG_ATMODULE_NANO)
            if (at_wifi_config->sta_proto.byte) {
                at_wifi_mode_set(0, at_wifi_config->sta_proto);
            } else {
                at_wifi_config->sta_proto = at_wifi_mode_get(0);
            }
            if (at_wifi_config->ap_proto.byte) {
                at_wifi_mode_set(1, at_wifi_config->ap_proto);
            } else {
                at_wifi_config->ap_proto = at_wifi_mode_get(1);
            }
#endif
            at_port_netmode_set(at_wifi_config->netmode);
            if (at_wifi_config->netmode == 0) {
                at_wifi_config->dhcp_state.bit.sta_dhcp = 0;
            }
            if (at_wifi_config->wifi_mode == WIFI_SOFTAP_MODE || at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
                at_wifi_ap_start();
            }

            if (at_wifi_config->wifi_mode == WIFI_STATION_MODE || at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
                if (at_wifi_config->auto_conn == WIFI_AUTOCONN_ENABLE) {
                    wifiopt_sta_connect();
                }
            }
        }
        break;

        case AT_WIFI_EVENT_DISCONNECTED: {
#if CONFIG_ATMODULE_TTY
            transportsdio_linkdown();
#endif
            if (g_wifi_sta_is_connected) {
                if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
                    ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
                   ) {
                    if (at_wifi_config->wevt_enable) {
                        at_response_string("+CW:DISCONNECTED\r\n");
                    }
                }
                g_wifi_sta_is_connected = 0;

                if (!g_wifi_reconnect_disable) {
                    at_wifi_sta_set_reconnect();
                }
            }
            if (at_wifi_config->wevt_enable) {
                at_response_string("+CW:ERROR,%d\r\n", reason_code_get(wifi_mgmr_sta_info_status_code_get()));
            }
            g_wifi_sta_disconnect_reason = wifi_mgmr_sta_info_status_code_get();// bl_fw_api.h eg: WLAN_FW_BEACON_LOSS
            at_wifi_config->connecting_state = 0;
        }
        break;
        case AT_WIFI_EVENT_SCAN_DONE: {
            extern void at_scan_done_event_tigger(void);
            at_scan_done_event_tigger();

            if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
                ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
               ) {
                if (at_wifi_config->wevt_enable) {
                    at_response_string("+CW:SCAN_DONE\r\n");
                }
            }
        }
        break;

        case AT_WIFI_EVENT_CONNECTED: {
#if CONFIG_ATMODULE_TTY
            transportsdio_linkup();
#endif
            if (!g_wifi_sta_is_connected) {
                if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
                    ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
                   ) {
                    if (at_wifi_config->wevt_enable) {
                        at_response_string("+CW:CONNECTED\r\n");
                    }
                }
                g_wifi_sta_is_connected = 1;
                if (!at_wifi_config->sta_info.store) {
                    at_wifi_config->sta_info.store = 1;
                    if (at->store) {
                        at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_INFO);
                    }
                }

                if (g_wifi_ap_is_start && at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
                    at_wifi_ap_start();
                }
                at_wifi_config->connecting_state = 0;
                //if (g_wifi_sta_use_dhcp == 0)
                //    aos_post_event(EV_WIFI, CODE_WIFI_ON_GOT_IP, 0);

#if 0
#ifdef AT_WIFI_SUPPORT_STORE_CHANNEL
                wifi_mgmr_sta_connect_ind_stat_info_t stat;
                wifi_mgmr_sta_connect_ind_stat_get(&stat);
                if (at_wifi_config->sta_info.freq != stat.chan_freq) {
                    at_wifi_config->sta_info.freq = stat.chan_freq;
                    if (at->store) {
                        at_wifi_config_save(AT_CONFIG_KEY_WIFI_STA_INFO);
                    }
                }
#endif
#endif
            }
        }
        break;

        case AT_WIFI_EVENT_GOTIP: {
            if (g_wifi_sta_is_connected) {
                if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
                    ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
                   ) {
                    if (at_wifi_config->wevt_enable) {
                        at_response_string("+CW:GOTIP\r\n");

#ifdef CONFIG_ATMODULE_BASE
                        if (at_base_config->sysmsg_cfg.syslog){
                            ip4_addr_t ipaddr = {0}, gwaddr = {0}, maskaddr = {0}, dns = {0};
                            at_wifi_sta_ip4_addr_get(&ipaddr.addr, &maskaddr.addr, &gwaddr.addr, &dns.addr);

                            at_response_string("IP:\"%s\"\r\n", ip4addr_ntoa(&ipaddr));
                            at_response_string("Gateway:\"%s\"\r\n", ip4addr_ntoa(&gwaddr));
                            at_response_string("Mask:\"%s\"\r\n", ip4addr_ntoa(&maskaddr));
                            at_response_string("DNS:\"%s\"\r\n", ip4addr_ntoa(&dns));
                        }
#endif
                    }
                }

                g_wifi_sta_disconnect_reason = 0;
            }
#if CONFIG_ATMODULE_NETWORK
            at_net_dns_load();
#endif
        }
        break;

#if (!CONFIG_ATMODULE_NANO)
        case AT_WIFI_EVENT_AP_STARTED: {
        }
        break;

        case AT_WIFI_EVENT_AP_STOP: {
        }
        break;

        case AT_WIFI_EVENT_AP_STA_ADD: {
            struct wifi_sta_basic_info sta_info;
            int idx = wifi_ap_update_sta_index();
            wifi_mgmr_ap_sta_info_get(&sta_info, idx);
            if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
                ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
               ) {
                if (at_wifi_config->wevt_enable) {
                    at_response_string("+CW:STA_CONNECTED \"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
                            sta_info.sta_mac[0],
                            sta_info.sta_mac[1],
                            sta_info.sta_mac[2],
                            sta_info.sta_mac[3],
                            sta_info.sta_mac[4],
                            sta_info.sta_mac[5]);
                }
            }
        }
        break;

        case AT_WIFI_EVENT_AP_STA_DEL: {
            uint8_t sta_mac[6] = {0, 0, 0, 0, 0, 0};
            wifi_ap_delete_sta_info(sta_mac);
            if (at_get_work_mode() != AT_WORK_MODE_THROUGHPUT
#ifdef CONFIG_ATMODULE_BASE
                ||  at_base_config->sysmsg_cfg.bit.link_state_msg
#endif
               ) {
                if (at_wifi_config->wevt_enable) {
                    at_response_string("+CW:STA_DISCONNECTED \"%02x:%02x:%02x:%02x:%02x:%02x\"\r\n",
                            sta_mac[0],
                            sta_mac[1],
                            sta_mac[2],
                            sta_mac[3],
                            sta_mac[4],
                            sta_mac[5]);
                }
            }
        }
#endif
        break;
        default: {
            /*nothing*/
        }
    }
}

static bool wifi_is_connected(void)
{
    return wifi_mgmr_sta_state_get();
}

int at_wifi_set_mode(void)
{
    if(at_wifi_config->wifi_mode == WIFI_STATION_MODE) {
        wifiopt_ap_stop(0);
        if (at_wifi_config->switch_mode_auto_conn == WIFI_AUTOCONN_ENABLE) {
            at_wifi_sta_set_reconnect();

            if (!wifi_is_connected()) {
                wifiopt_sta_connect();
            }
        }
    } else if(at_wifi_config->wifi_mode == WIFI_SOFTAP_MODE) {
        wifiopt_sta_disconnect(0);
        if (!wifi_mgmr_ap_state_get()) {
            at_wifi_ap_start();
        }
    } else if(at_wifi_config->wifi_mode == WIFI_AP_STA_MODE) {
        if (!wifi_mgmr_ap_state_get()) {
            at_wifi_ap_start();
        }
        if (at_wifi_config->switch_mode_auto_conn == WIFI_AUTOCONN_ENABLE) {
            if (!wifi_is_connected()) {
                wifiopt_sta_connect();
            }
        }
    } else if (at_wifi_config->wifi_mode == WIFI_DISABLE) {
        wifiopt_ap_stop(0);
        wifiopt_sta_disconnect(0);
    }

    return 0;
}

int at_wifi_sta_connect(int timeout)
{
    wifiopt_sta_disconnect(0);
    wifiopt_sta_connect();
    return 0;
}

int at_wifi_sta_disconnect(void)
{
    wifiopt_sta_disconnect(1);
    return 0;
}

int at_wifi_sta_set_reconnect(void)
{
#if (!CONFIG_ATMODULE_NANO)
    wifi_sta_enable_reconnect(1);
#endif
    return 0;
}

int at_wifi_ap_start(void)
{
#if (!CONFIG_ATMODULE_NANO)
    wifiopt_ap_stop(0);
    return wifi_ap_start();
#endif
}

int at_wifi_ap_stop(void)
{
    return 0;
}

int at_wifi_ap_get_sta_ip(uint8_t *mac, char *ip, uint32_t ipbuf_size, int check_flag)
{
    uint32_t ip_addr;
    ip4_addr_t ipaddr;

    if (wifi_ap_get_sta_ip(mac, &ip_addr) != 0) {
        strlcpy(ip, "", ipbuf_size);
        return -1;
    }

    if (check_flag) {
        uint32_t start_ip, end_ip;
        start_ip = (at_wifi_config->ap_ip.ip & at_wifi_config->ap_ip.netmask) | (at_wifi_config->dhcp_server.start << 24);
        end_ip = (at_wifi_config->ap_ip.ip & at_wifi_config->ap_ip.netmask) | (at_wifi_config->dhcp_server.end << 24);
        if (ntohl(ip_addr) < ntohl(start_ip) || ntohl(ip_addr) > ntohl(end_ip))
            ip_addr = 0;
    }

    ipaddr.addr = ip_addr;
    strlcpy(ip, ip4addr_ntoa(&ipaddr), ipbuf_size);
    return 0;
}

int at_wifi_stop(void)
{
    wifiopt_ap_stop(1);
    wifiopt_sta_disconnect(1);

    return 0;
}

int at_wifi_hostname_set(char *hostname)
{
#if CONFIG_ATMODULE_NETWORK &&LWIP_NETIF_HOSTNAME
    struct netif *netif = (struct netif *)net_if_find_from_name("wl1");
    netif_set_hostname(netif, hostname);
#endif
}
