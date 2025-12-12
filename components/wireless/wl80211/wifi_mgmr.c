#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "wl80211.h"
#include "supplicant_api.h"
#include "rtos_al.h"
#include "wifi_mgmr.h"
#include "async_event.h"

#include <lwip/etharp.h>
#include <lwip/netdb.h>
#include <lwip/netifapi.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>

#define DBG_TAG "WIFI_MGMR"
#include "log.h"

extern void *_wifi_mgmr_sta_start_dhcpc(void);
extern void *_wifi_mgmr_sta_stop_dhcpc(void);
extern void *_wifi_mgmr_get_sta_netif(void);

static void connect_ind_dump(uint16_t status_code, uint16_t ieeetypes_code)
{
    printf("=================================================================\r\n");
    printf("[AT][RX] Connection Status\r\n");
    printf("[AT][RX]   status_code %u\r\n", status_code);
    printf("[AT][RX]   reason_code %u\r\n", ieeetypes_code);
    printf("[AT][RX]   status detail: %s\r\n", wifi_mgmr_get_sm_status_code_str(status_code));
    printf("=================================================================\r\n");
}

static void ip_got_cb(struct netif *netif)
{
    char *state, *connected;

    if (wifi_mgmr_sta_state_get()) {
        state = "UP";
        connected = ",CONNECTED";
    } else {
        state = "DOWN";
        connected = "";
    }

    extern size_t kfree_size(void);
    printf("Memory left is %d Bytes\r\n", kfree_size());
    printf("[%d]  %c%c: MAC=%02x:%02x:%02x:%02x:%02x:%02x ip=%d.%d.%d.%d/%d %s%s\n", netif->num, netif->name[0],
           netif->name[1], netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4],
           netif->hwaddr[5], netif->ip_addr.addr & 0xff, (netif->ip_addr.addr >> 8) & 0xff,
           (netif->ip_addr.addr >> 16) & 0xff, (netif->ip_addr.addr >> 24) & 0xff,
           32 - __builtin_clz(netif->netmask.addr), state, connected);
}

static void start_dhcp_tsk(void)
{
    _wifi_mgmr_sta_start_dhcpc();
}

static void disconnect_tsk(void)
{
    _wifi_mgmr_sta_stop_dhcpc();
}

static int cmp_rssi_desc(const void *a, const void *b)
{
    const struct wl80211_scan_result_item *lhs = *(void **)a;
    const struct wl80211_scan_result_item *rhs = *(void **)b;

    return (rhs->rssi > lhs->rssi) - (rhs->rssi < lhs->rssi);
}

static void dump_scan_result(void)
{
    struct wl80211_scan_result_item *n;
    struct wl80211_scan_result_item **rssi_sorted = NULL;
    int i = 0;
    int auth = -1, cipher = -1;
    int total = 0;

    /* count APs */
    RB_FOREACH(n, _scan_result_tree, &wl80211_scan_result)
    {
        total++;
    }

    rssi_sorted = calloc(total, sizeof(struct wl80211_scan_result_item *));
    assert(rssi_sorted);

    // sort rssi
    i = 0;
    RB_FOREACH(n, _scan_result_tree, &wl80211_scan_result)
    {
        rssi_sorted[i++] = n;
    }

    qsort(rssi_sorted, total, sizeof(void *), cmp_rssi_desc);

    printf("cached scan list\r\n");
    printf("*********************************************************************"
           "*******************************\r\n");

    for (i = 0; i < total; i++) {
        n = rssi_sorted[i];

        RB_REMOVE(_scan_result_tree, &wl80211_scan_result, n);

        if (n->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN) {
            if (n->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK;
                if (n->key_mgmt & WPA_KEY_MGMT_SAE) {
                    auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE;
                }
            } else if (n->key_mgmt & WPA_KEY_MGMT_SAE) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE;
            } else if (n->key_mgmt & (WPA_KEY_MGMT_IEEE8021X | WPA_KEY_MGMT_IEEE8021X_SHA256)) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT;
            }
        } else if (n->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA) {
            if (n->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK;
            } else if (n->key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT;
            }
        }

#define TEST_WPA_RSN (WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN | WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA)
        if ((n->flags & TEST_WPA_RSN) == TEST_WPA_RSN) {
            if (auth != WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK;
            }
        } else if ((n->flags & TEST_WPA_RSN) == 0) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WEP;
            cipher = WIFI_EVENT_BEACON_IND_CIPHER_WEP;
        }
#undef TEST_WPA_RSN

#define TEST_CCMP_TKIP (WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP | WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP)
        if ((n->flags & (TEST_CCMP_TKIP)) == WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP) {
            cipher = WIFI_EVENT_BEACON_IND_CIPHER_AES;
        }
        if ((n->flags & (TEST_CCMP_TKIP)) == WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP) {
            cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP;
        }
        if ((n->flags & (TEST_CCMP_TKIP)) == TEST_CCMP_TKIP) {
            cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES;
        }
#undef TEST_CCMP_TKIP

        if (n->key_mgmt == 0) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_OPEN;
            cipher = WIFI_EVENT_BEACON_IND_CIPHER_NONE;
        }

        printf("index[%02d]: channel %3u, bssid %02X:%02X:%02X:%02X:%02X:%02X, "
               "rssi %3d, ppm abs:rel %3d : %3d, wps %2d, mode %6s, auth %20s, "
               "cipher:%12s, SSID %s\r\n",
               i, n->channel, n->bssid[0], n->bssid[1], n->bssid[2], n->bssid[3], n->bssid[4], n->bssid[5], n->rssi, 0,
               0, !!(n->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPS), wifi_mgmr_mode_to_str(n->mode),
               wifi_mgmr_auth_to_str(auth), wifi_mgmr_cipher_to_str(cipher), n->ssid);

        if (n->ssid) {
            free((void *)n->ssid);
        }
        free(n);
    }
    printf("index[%02d]: empty\r\n", total);
    printf("---------------------------------------------------------------------"
           "-------------------------------\r\n");

    free(rssi_sorted);
}

static void evt_handler_wrapper(void (*handler)(void))
{
    assert(handler != NULL);
    handler();

    vTaskDelete(NULL);
}

static void wl80211_event_handler(input_event_t ev, void *priv)
{
    void (*handler)(void) = NULL;
    uint16_t stack_size = 265;

    switch (ev->code) {
        case WL80211_EVT_CONNECTED:
            connect_ind_dump(0, 0);
            async_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTED, 0);
            handler = start_dhcp_tsk;
            break;

        case WL80211_EVT_SCAN_DONE:
            async_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, 0);
            handler = dump_scan_result;
            break;

        case WL80211_EVT_DISCONNECTED:
            connect_ind_dump((ev->value >> 16) & 0xFF, ev->value & 0xFF);
            async_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, 0);
            handler = disconnect_tsk;
            break;
    }

    if (handler != NULL) {
        xTaskCreate((void *)evt_handler_wrapper, "evt", stack_size, handler, 20, NULL);
    }
}

static void wifi_mgmr_event_handler(input_event_t ev, void *priv)
{
    switch (ev->code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump(void);
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            ip_got_cb(_wifi_mgmr_get_sta_netif());
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            LOG_I("[APP] [EVT] [AP] [ADD] %lld\r\n", xTaskGetTickCount());
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            LOG_I("[APP] [EVT] [AP] [DEL] %lld\r\n", xTaskGetTickCount());
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", ev->code);
        }
    }
}

void wifi_mgmr_init(void)
{
    async_register_event_filter(EV_WL80211, wl80211_event_handler, NULL);
    async_register_event_filter(EV_WIFI, wifi_mgmr_event_handler, NULL);

    /* compat legacy wifi mgmr */
    async_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);
    async_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DONE, 0);
}

void wifi_mgmr_sta_ps_enter(void)
{
    if (wl80211_cntrl(WL80211_CTRL_STA_SET_PS_MODE, 1)) {
        printf("enable ps mode failed!\r\n");
        abort();
    }
}

void wifi_mgmr_sta_ps_exit(void)
{
    if (wl80211_cntrl(WL80211_CTRL_STA_SET_PS_MODE, 0)) {
        printf("exit ps mode failed!\r\n");
        abort();
    }
}

static int parse_mac_addr(const char *str, uint8_t *addr)
{
    const char *ptr = str;
    uint32_t i;

    if (!str || strlen(str) < 17 || !addr)
        return -1;

    for (i = 0; i < 6; i++) {
        char *next;
        long int hex = strtol(ptr, &next, 16);
        if (((unsigned)hex > 255) || ((hex == 0) && (next == ptr)) || ((i < 5) && (*next != ':')) ||
            ((i == 5) && (*next != '\0')))
            return -1;

        ((uint8_t *)addr)[i] = (uint8_t)hex;
        ptr = ++next;
    }

    return 0;
}

int wifi_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg,
                     uint16_t freq1, uint16_t freq2, uint8_t use_dhcp)
{
    char conn_ssid[33] = { 0 };
    char conn_password[65] = { 0 };
    uint8_t conn_bssid[6] = { 0 };
    uint8_t conn_pmf_cfg = 0;
    uint16_t conn_freq = 0;
    int ssid_len, key_len;

    if (ssid) {
        ssid_len = strlen(ssid);
        if (ssid_len <= 0 || ssid_len > MGMR_SSID_LEN) {
            return -1;
        }
        strcpy(conn_ssid, ssid);
    } else {
        return -1;
    }

    if (key) {
        key_len = strlen(key);
        if ((key_len < 8 && key_len != 5 && key_len != 0) || (key_len > MGMR_KEY_LEN)) {
            return -1;
        }
        strcpy(conn_password, key);
    } else {
        memset(conn_password, 0, sizeof(conn_password));
    }

    if (bssid) {
        if (strlen(bssid) > MGMR_BSSID_LEN || parse_mac_addr(bssid, conn_bssid) != 0) {
            return -1;
        }
    } else {
        memset(conn_bssid, 0, sizeof(conn_bssid));
    }

    (void)akm_str;

    if (pmf_cfg < 0 || pmf_cfg > 2) {
        return -1;
    } else {
        conn_pmf_cfg = pmf_cfg;
    }

    if ((freq1 && wl80211_freq_valid_check(freq1)) || (freq2 && wl80211_freq_valid_check(freq2)) ||
        (freq1 && freq2 && freq1 != freq2)) {
        return -1;
    } else {
        conn_freq = freq1 ? freq1 : freq2;
    }

    (void)use_dhcp;

    wl80211_cntrl(WL80211_CTRL_STA_SET_SSID, conn_ssid);
    wl80211_cntrl(WL80211_CTRL_STA_SET_PASSWORD, conn_password);
    wl80211_cntrl(WL80211_CTRL_STA_SET_BSSID, conn_bssid);
    wl80211_cntrl(WL80211_CTRL_STA_SET_FREQ, conn_freq);
    wl80211_cntrl(WL80211_CTRL_STA_SET_PMF, conn_pmf_cfg);
    wl80211_cntrl(WL80211_CTRL_STA_CONNECT);

    return 0;
}

int wifi_sta_disconnect(void)
{
    return wl80211_cntrl(WL80211_CTRL_STA_DISCONNECT);
}

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6])
{
    if (wl80211_glb.link_up == 0) {
        return -1;
    }

    assert(bssid != NULL);
    memcpy(bssid, wl80211_glb.bssid, 6);
    return 0;
}

int wifi_mgmr_sta_mac_get(uint8_t mac[6])
{
    assert(mac != NULL);

    return wl80211_cntrl(WL80211_CTRL_STA_GET_MAC, mac);
}

int wifi_mgmr_sta_rssi_get(int *rssi)
{
    if (wl80211_glb.link_up == 0) {
        return -1;
    }
    assert(rssi != NULL);

    return wl80211_cntrl(WL80211_CTRL_STA_GET_RSSI, rssi);
}

int wifi_mgmr_sta_channel_get(uint8_t *channel)
{
    if (wl80211_glb.link_up == 0) {
        return -1;
    }
    assert(channel != NULL);

    *channel = (uint8_t)phy_freq_to_channel(PHY_BAND_2G4, wl80211_glb.freq);
    return 0;
}

int wifi_mgmr_sta_aid_get(void)
{
    if (wl80211_glb.link_up == 0) {
        return -1;
    }

    return wl80211_glb.aid;
}

int wifi_mgmr_sta_state_get(void)
{
    return wl80211_glb.link_up;
}

void wifi_mgmr_sta_info_dump(wifi_mgmr_connect_ind_stat_info_t *connection_info)
{
    int passphr_len = strlen(connection_info->passphr);
    printf("Connected\r\n");
    printf("ssid:            %s\r\n", connection_info->ssid);
    // fix show error when using wps
    printf("passphr:         %s\r\n", (passphr_len || connection_info->security) ? "********" : "Open");
    printf("bssid:           %02X:%02X:%02X:%02X:%02X:%02X\r\n", connection_info->bssid[0], connection_info->bssid[1],
           connection_info->bssid[2], connection_info->bssid[3], connection_info->bssid[4], connection_info->bssid[5]);
    printf("aid:             %d\r\n", connection_info->aid);
    printf("channel:         %d\r\n", connection_info->channel);
    printf("band:            %s\r\n", (connection_info->chan_band) ? "5G" : "2.4G");
    printf("security:        %d\r\n", connection_info->security);
}

int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat)
{
    if (wl80211_glb.link_up) {
        int ssid_len = strlen((char *)wl80211_glb.ssid);
        if (ssid_len > 0) {
            memcpy(wifi_mgmr_ind_stat->ssid, wl80211_glb.ssid, ssid_len);
            wifi_mgmr_ind_stat->ssid[ssid_len] = '\0';
        }

        int passphr_len = strlen((char *)wl80211_glb.password);
        if (passphr_len > 0) {
            memcpy(wifi_mgmr_ind_stat->passphr, wl80211_glb.password, passphr_len);
            wifi_mgmr_ind_stat->passphr[passphr_len] = '\0';
        }
        memcpy(wifi_mgmr_ind_stat->bssid, wl80211_glb.bssid, sizeof(wl80211_glb.bssid));
        wifi_mgmr_ind_stat->aid = wl80211_glb.aid;
        wifi_mgmr_ind_stat->channel = (uint8_t)phy_freq_to_channel(wl80211_glb.chan_band, wl80211_glb.freq);
        wifi_mgmr_ind_stat->chan_band = wl80211_glb.chan_band;
        wifi_mgmr_ind_stat->security = wl80211_glb.security;

        wifi_mgmr_sta_info_dump(wifi_mgmr_ind_stat);

        return 0;
    } else {
        printf("No Connect\r\n");
        return -1;
    }
}

int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config)
{
    struct wl80211_scan_params scan_params;

    memset(&scan_params, 0, sizeof(struct wl80211_scan_params));
    scan_params.ssid_length = config->ssid_length;
    if (config->ssid_length) {
        memcpy(scan_params.ssid, config->ssid_array, config->ssid_length);
    }
    if (config->bssid_set_flag) {
        memcpy(scan_params.bssid, config->bssid, 6);
    } else {
        memset(scan_params.bssid, 0xFF, 6);
    }
    if (config->channels_cnt) {
        scan_params.channels_cnt = config->channels_cnt;
        memcpy(scan_params.channels, config->channels, config->channels_cnt);
    }
    scan_params.probe_cnt = config->probe_cnt;
    scan_params.duration = config->duration;
    scan_params.passive = config->passive;

    return wl80211_cntrl(WL80211_CTRL_SCAN_PARAMS, &scan_params);
}

uint32_t wifi_mgmr_sta_scanlist_nums_get(void)
{
    struct wl80211_scan_result_item *n, *tmp;
    uint32_t cnt = 0;

    RB_FOREACH_SAFE(n, _scan_result_tree, &wl80211_scan_result, tmp)
    {
        cnt++;
    }

    return cnt;
}

static void wifi_mgmr_sta_scanlist_format(struct wl80211_scan_result_item *result, wifi_mgmr_scan_item_t *item)
{
    uint8_t auth = 0, cipher = 0;

    if (result->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN) {
        if (result->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK;
            if (result->key_mgmt & WPA_KEY_MGMT_SAE) {
                auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE;
            }
        } else if (result->key_mgmt & WPA_KEY_MGMT_SAE) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE;
        } else if (result->key_mgmt & (WPA_KEY_MGMT_IEEE8021X | WPA_KEY_MGMT_IEEE8021X_SHA256)) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT;
        }
    } else if (result->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA) {
        if (result->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK;
        } else if (result->key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT;
        }
    }

#define TEST_WPA_RSN (WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN | WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA)
    if ((result->flags & TEST_WPA_RSN) == TEST_WPA_RSN) {
        if (auth != WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT) {
            auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK;
        }
    } else if ((result->flags & TEST_WPA_RSN) == 0) {
        auth = WIFI_EVENT_BEACON_IND_AUTH_WEP;
        cipher = WIFI_EVENT_BEACON_IND_CIPHER_WEP;
    }
#undef TEST_WPA_RSN

#define TEST_CCMP_TKIP (WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP | WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP)
    if ((result->flags & (TEST_CCMP_TKIP)) == WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP) {
        cipher = WIFI_EVENT_BEACON_IND_CIPHER_AES;
    }
    if ((result->flags & (TEST_CCMP_TKIP)) == WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP) {
        cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP;
    }
    if ((result->flags & (TEST_CCMP_TKIP)) == TEST_CCMP_TKIP) {
        cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES;
    }
#undef TEST_CCMP_TKIP

    if (result->key_mgmt == 0) {
        auth = WIFI_EVENT_BEACON_IND_AUTH_OPEN;
        cipher = WIFI_EVENT_BEACON_IND_CIPHER_NONE;
    }

    item->mode = result->mode;
    item->timestamp_lastseen = rtos_now(0);
    if (result->ssid) {
        item->ssid_len = strlen(result->ssid);
        memcpy(item->ssid, result->ssid, strlen(result->ssid));
    }
    item->ssid_tail[0] = '\0';
    item->channel = result->channel;
    item->rssi = result->rssi;
    memcpy(item->bssid, result->bssid, 6);
    item->ppm_abs = 0;
    item->ppm_rel = 0;
    item->auth = auth;
    item->cipher = cipher;
    item->is_used = 1;
    item->wps = (result->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPS) ? 1 : 0;
    item->best_antenna = 0;
}

uint32_t wifi_mgmr_sta_scanlist_dump(void *results, uint32_t resultNums)
{
    struct wl80211_scan_result_item *n, *tmp;
    uint32_t cnt = 0;
    wifi_mgmr_scan_item_t *scanResults = (wifi_mgmr_scan_item_t *)results;

    RB_FOREACH_SAFE(n, _scan_result_tree, &wl80211_scan_result, tmp)
    {
        memset(scanResults + cnt, 0, sizeof(wifi_mgmr_scan_item_t));
        wifi_mgmr_sta_scanlist_format(n, scanResults + cnt);
        cnt++;

        if (cnt >= resultNums) {
            break;
        }
    }

    return cnt;
}

int wifi_mgmr_sta_scanlist_free(void)
{
    struct wl80211_scan_result_item *n, *tmp;

    RB_FOREACH_SAFE(n, _scan_result_tree, &wl80211_scan_result, tmp)
    {
        RB_REMOVE(_scan_result_tree, &wl80211_scan_result, n);

        if (n->ssid) {
            free((void *)n->ssid);
        }
        free(n);
    }

    return 0;
}

int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb)
{
    struct wl80211_scan_result_item *n, *tmp;
    wifi_mgmr_scan_item_t item;

    RB_FOREACH_SAFE(n, _scan_result_tree, &wl80211_scan_result, tmp)
    {
        memset(&item, 0, sizeof(wifi_mgmr_scan_item_t));
        wifi_mgmr_sta_scanlist_format(n, &item);
        cb(env, arg, &item);
    }

    return 0;
}

int wifi_mgmr_ap_state_get(void)
{
    return -1;
}

int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns)
{
    return -1;
}

int wifi_mgmr_ap_mac_get(uint8_t mac[6])
{
    return -1;
}

int wifi_mgmr_sta_scanlist(void)
{
    return -1;
}

int wifi_mgmr_sta_autoconnect_disable(void)
{
    return -1;
}

uint16_t wifi_mgmr_sta_info_status_code_get(void)
{
    return wl80211_glb.status_code;
}

uint16_t wifi_mgmr_sta_info_reason_code_get(void)
{
    return wl80211_glb.reason_code;
}

int wifi_mgmr_get_channel_nums(const char *country_code, uint8_t *c24G_cnt, uint8_t *c5G_cnt)
{
    return wl80211_get_channel_nums(country_code, c24G_cnt, c5G_cnt);
}

int wifi_mgmr_get_channel_list(const char *country_code, uint8_t **c24G_list, uint8_t **c5G_list)
{
    return wl80211_get_channel_list(country_code, c24G_list, c5G_list);
}

int wifi_mgmr_set_country_code(char *country_code)
{
    return wl80211_set_country_code(country_code);
}

int wifi_mgmr_get_country_code(char *country_code)
{
    return wl80211_get_country_code(country_code);
}

char *wifi_mgmr_mode_to_str(uint32_t mode)
{
    wifimode_t allmode[] = {
        { "B", WIFI_MODE_B },
        { "BG", WIFI_MODE_BG },
        { "BGN", WIFI_MODE_BGN },
        { "BGAX", WIFI_MODE_BGAX },
        { "BGNAX", WIFI_MODE_BGNAX },
        { "A", WIFI_MODE_A },
        { "AN", WIFI_MODE_AN },
        // WiFi 5 (802.11ac)
        { "AAC", WIFI_MODE_AAC },
        { "ANAC", WIFI_MODE_ANAC },
        // WiFi 6 (802.11ax) 5G
        { "AAX", WIFI_MODE_AAX },
        { "ANAX", WIFI_MODE_ANAX },
        { "AACAX", WIFI_MODE_AACAX },
        { "ANACAX", WIFI_MODE_ANACAX },
    };
    int mode_count = sizeof(allmode) / sizeof(wifimode_t);

    for (int i = 0; i < mode_count; i++) {
        if (allmode[i].mode == mode) {
            return allmode[i].mode_str;
        }
    }

    return "Unknown";
}

char *wifi_mgmr_auth_to_str(uint8_t auth)
{
    switch (auth) {
        case WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE: {
            return "WPA3-SAE";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE: {
            return "WPA2-PSK/WPA3-SAE";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_OPEN: {
            return "Open";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_WEP: {
            return "WEP";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK: {
            return "WPA-PSK";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK: {
            return "WPA2-PSK";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK: {
            return "WPA2-PSK/WPA-PSK";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT: {
            return "WPAx-Enterprise";
        } break;
        case WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN: {
            return "Unknown";
        } break;
        default: {
            return "Unknown";
        }
    }
}

char *wifi_mgmr_cipher_to_str(uint8_t cipher)
{
    if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_NONE) {
        return "NONE";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_WEP) {
        return "WEP";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_AES) {
        return "AES";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP) {
        return "TKIP";
    } else if (cipher == WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES) {
        return "TKIP/AES";
    } else {
        return "Unknown";
    }
}

static const struct reason_code sm_reason_list[] = {
    { WLAN_FW_SUCCESSFUL, "sm connect ind ok" },
    { WLAN_FW_TX_AUTH_FRAME_ALLOCATE_FAIILURE, "tx auth frame alloc failure" },
    { WLAN_FW_AUTHENTICATION_FAIILURE, "Authentication failure" },
    { WLAN_FW_AUTH_ALGO_FAIILURE, "Auth response but auth algo failure" },
    { WLAN_FW_TX_ASSOC_FRAME_ALLOCATE_FAIILURE, "tx assoc frame alloc failure" },
    { WLAN_FW_ASSOCIATE_FAIILURE, "Association failure" },
    { WLAN_FW_DEAUTH_BY_AP_WHEN_NOT_CONNECTION, "deauth by AP when connecting" },
    { WLAN_FW_DEAUTH_BY_AP_WHEN_CONNECTION, "deauth by AP when connected" },
    { WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE, "Passwd error, 4-way handshake timeout" },
    { WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_TRANSMIT_FAILURE, "Passwd error, tx deauth frame transmit failure" },
    { WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_ALLOCATE_FAIILURE, "Passwd error, tx deauth frame allocate failure" },
    { WLAN_FW_AUTH_OR_ASSOC_RESPONSE_TIMEOUT_FAILURE, "auth or associate frame response timeout failure" },
    { WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL, "SSID error, scan no bssid and channel" },
    { WLAN_FW_CREATE_CHANNEL_CTX_FAILURE_WHEN_JOIN_NETWORK, "create channel context failure when join network" },
    { WLAN_FW_JOIN_NETWORK_FAILURE, "join network failure" },
    { WLAN_FW_ADD_STA_FAILURE, "add sta failure" },
    { WLAN_FW_BEACON_LOSS, "ap beacon loss" },
    { WLAN_FW_NETWORK_SECURITY_NOMATCH, "network security no match" },
    { WLAN_FW_NETWORK_WEPLEN_ERROR, "wep network psk len error" },
    { WLAN_FW_DISCONNECT_BY_USER_WITH_DEAUTH, "user disconnect and send deauth" },
    { WLAN_FW_DISCONNECT_BY_USER_NO_DEAUTH, "user disconnect but no send deauth" },
    { WLAN_FW_DISCONNECT_BY_FW_PS_TX_NULLFRAME_FAILURE, "fw disconnect(tx nullframe failures)" },
    { WLAN_FW_TRAFFIC_LOSS, "fw disconnect(traffic loss)" },
    { WLAN_FW_SWITCH_CHANNEL_FAILURE, "fw switch channel failure" },
    { WLAN_FW_AUTH_OR_ASSOC_RESPONSE_CFM_FAILURE, "auth or associate frame response cfm failure" },
    { WLAN_FW_CONNECT_PARAMS_ERROR, "connect params error" },
};

static const char *_get_status_code_str(const struct reason_code list[], uint16_t len, uint16_t status_code)
{
    int i;

    for (i = 0; i < len; i++) {
        if (list[i].reason_code == status_code) {
            break;
        }
    }
    if (len == i) {
        return "Unknown Code";
    }

    return list[i].action;
}

const char *wifi_mgmr_get_sm_status_code_str(uint16_t status_code)
{
    return _get_status_code_str(sm_reason_list, sizeof(sm_reason_list) / sizeof(sm_reason_list[0]), status_code);
}
