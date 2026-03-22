#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "wl80211.h"
#include "macsw.h"

#include "supplicant_api.h"
#include "wl80211_platform.h"
#include "bl_lp.h"

#ifdef COMPAT_WIFI_MGMR
#include "wifi_mgmr.h"

#if !defined(__NuttX__)
#include "async_event.h"
#endif

/* Autoconnect state */
static int wifi_mgmr_disable_autoconnect = 1; /* Default: disabled */
static uint16_t wifi_mgmr_listen_itv = 0;

#if !defined(__NuttX__)
#define WIFI_MGMR_AP_DHCPD_DEFAULT_START (-1)
#define WIFI_MGMR_AP_DHCPD_DEFAULT_LIMIT (-1)

struct wifi_mgmr_ap_netif_cfg {
    bool pending;
    bool use_ipcfg;
    bool use_dhcpd;
    int start;
    int limit;
    uint32_t ap_ipaddr;
    uint32_t ap_mask;
};

static struct wifi_mgmr_ap_netif_cfg wifi_mgmr_ap_netif_cfg;

extern void *_wifi_mgmr_sta_link_up(void);
extern void *_wifi_mgmr_sta_link_down(void);
extern void _wifi_mgmr_ip_got_dump(void);
extern void _wifi_mgmr_ap_start_dhcpd(bool use_ipcfg, bool use_dhcpd, int start, int limit, uint32_t ap_ipaddr,
                                      uint32_t ap_mask);
extern void _wifi_mgmr_ap_stop_dhcpd(void);

static void auth_cipher_convert(struct wl80211_scan_result_item *result, uint8_t *auth, uint8_t *cipher);

static void wifi_mgmr_ap_netif_cfg_set_defaults(struct wifi_mgmr_ap_netif_cfg *config)
{
    config->use_ipcfg = true;
    config->use_dhcpd = true;
    config->start = WIFI_MGMR_AP_DHCPD_DEFAULT_START;
    config->limit = WIFI_MGMR_AP_DHCPD_DEFAULT_LIMIT;
    config->ap_ipaddr = 0;
    config->ap_mask = 0;
}

static void wifi_mgmr_ap_netif_cfg_clear(void)
{
    memset(&wifi_mgmr_ap_netif_cfg, 0, sizeof(wifi_mgmr_ap_netif_cfg));
    wifi_mgmr_ap_netif_cfg_set_defaults(&wifi_mgmr_ap_netif_cfg);
}

static void wifi_mgmr_ap_netif_cfg_save(const wifi_mgmr_ap_params_t *config)
{
    wifi_mgmr_ap_netif_cfg.pending = true;
    wifi_mgmr_ap_netif_cfg.use_ipcfg = config->use_ipcfg;
    wifi_mgmr_ap_netif_cfg.use_dhcpd = config->use_ipcfg && config->use_dhcpd;
    wifi_mgmr_ap_netif_cfg.ap_ipaddr = config->ap_ipaddr;
    wifi_mgmr_ap_netif_cfg.ap_mask = config->ap_mask;

    if (wifi_mgmr_ap_netif_cfg.use_dhcpd && (config->start <= 0 || config->limit <= 0)) {
        wifi_mgmr_ap_netif_cfg.start = WIFI_MGMR_AP_DHCPD_DEFAULT_START;
        wifi_mgmr_ap_netif_cfg.limit = WIFI_MGMR_AP_DHCPD_DEFAULT_LIMIT;
    } else {
        wifi_mgmr_ap_netif_cfg.start = config->start;
        wifi_mgmr_ap_netif_cfg.limit = config->limit;
    }
}

static void wifi_mgmr_ap_netif_start(void)
{
    if (!wifi_mgmr_ap_netif_cfg.pending) {
        wifi_mgmr_ap_netif_cfg_set_defaults(&wifi_mgmr_ap_netif_cfg);
    }

    _wifi_mgmr_ap_start_dhcpd(wifi_mgmr_ap_netif_cfg.use_ipcfg, wifi_mgmr_ap_netif_cfg.use_dhcpd,
                              wifi_mgmr_ap_netif_cfg.start, wifi_mgmr_ap_netif_cfg.limit,
                              wifi_mgmr_ap_netif_cfg.ap_ipaddr, wifi_mgmr_ap_netif_cfg.ap_mask);

    wifi_mgmr_ap_netif_cfg.pending = false;
}

static void wifi_mgmr_ap_netif_stop(void)
{
    wifi_mgmr_ap_netif_cfg.pending = false;
    _wifi_mgmr_ap_stop_dhcpd();
}

static void connect_ind_dump(uint16_t status_code, uint16_t ieeetypes_code)
{
    wl80211_printf("=================================================================\r\n");
    wl80211_printf("[AT][RX] Connection Status\r\n");
    wl80211_printf("[AT][RX]   status_code %u\r\n", status_code);
    wl80211_printf("[AT][RX]   reason_code %u\r\n", ieeetypes_code);
    wl80211_printf("[AT][RX]   status detail: %s\r\n", wifi_mgmr_get_sm_status_code_str(status_code));
    wl80211_printf("=================================================================\r\n");
}

static void start_dhcp_tsk(void)
{
    _wifi_mgmr_sta_link_up();
}

static void disconnect_tsk(void)
{
    _wifi_mgmr_sta_link_down();
    if (!wifi_mgmr_disable_autoconnect && wl80211_glb.last_connect_params) {
        wl80211_printf("Autoconnect: Reconnecting to %s\r\n", wl80211_glb.last_connect_params->ssid);
        wl80211_sta_connect(wl80211_glb.last_connect_params);
    }
}

static void start_dhcpd_tsk(void)
{
    wifi_mgmr_ap_netif_start();
}

static void stop_dhcpd_tsk(void)
{
    wifi_mgmr_ap_netif_stop();
}
#endif

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
    uint8_t auth, cipher;
    int total = 0;

    /* count APs */
    RB_FOREACH(n, _scan_result_tree, &wl80211_scan_result)
    {
        total++;
    }

    wl80211_printf("cached scan list\r\n");
    wl80211_printf("*********************************************************************"
                   "*******************************\r\n");

    if (total == 0) {
        goto _RET;
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

    for (i = 0; i < total; i++) {
        n = rssi_sorted[i];

        auth_cipher_convert(n, &auth, &cipher);

        wl80211_printf("index[%02d]: channel %3u, bssid %02X:%02X:%02X:%02X:%02X:%02X, "
                       "rssi %3d, ppm abs:rel %3d : %3d, wps %2d, mode %6s, auth %20s, "
                       "cipher:%12s, SSID %s\r\n",
                       i, n->channel, n->bssid[0], n->bssid[1], n->bssid[2], n->bssid[3], n->bssid[4], n->bssid[5],
                       n->rssi, 0, 0, !!(n->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPS),
                       wifi_mgmr_mode_to_str(n->mode), wifi_mgmr_auth_to_str(auth), wifi_mgmr_cipher_to_str(cipher),
                       n->ssid);
    }
    free(rssi_sorted);

_RET:
    wl80211_printf("index[%02d]: empty\r\n", total);
    wl80211_printf("---------------------------------------------------------------------"
                   "-------------------------------\r\n");
}

#if !defined(__NuttX__)
static void wl80211_event_handler(async_input_event_t ev, void *priv)
{
    void (*handler)(void) = NULL;

    switch (ev->code) {
        case WL80211_EVT_STA_CONNECTED:
            connect_ind_dump(0, 0);
            async_post_event(EV_WIFI, CODE_WIFI_ON_CONNECTED, 0);
            handler = start_dhcp_tsk;
            break;

        case WL80211_EVT_SCAN_DONE:
            async_post_event(EV_WIFI, CODE_WIFI_ON_SCAN_DONE, 0);
            handler = dump_scan_result;
            break;

        case WL80211_EVT_STA_DISCONNECTED:
            connect_ind_dump((ev->value >> 16) & 0xFF, ev->value & 0xFF);
            async_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, 0);
            handler = disconnect_tsk;
            break;

        case WL80211_EVT_AP_STARTED:
            async_post_event(EV_WIFI, CODE_WIFI_ON_AP_STARTED, 0);
            handler = start_dhcpd_tsk;
            break;

        case WL80211_EVT_AP_STOPPED:
            async_post_event(EV_WIFI, CODE_WIFI_ON_AP_STOPPED, 0);
            handler = stop_dhcpd_tsk;
            break;
    }

    void rtos_start_evt_task(void (*handler)(void));
    if (handler != NULL) {
        rtos_start_evt_task(handler);
    }
}

static void wifi_mgmr_event_handler(async_input_event_t ev, void *priv)
{
    switch (ev->code) {
        case CODE_WIFI_ON_INIT_DONE: {
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
            wifi_mgmr_sta_scanlist();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
            void mm_sec_keydump(void);
            mm_sec_keydump();
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            _wifi_mgmr_ip_got_dump();
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
#ifdef LP_APP
            bl_pm_event_bit_clear(PSM_EVENT_DISCONNECT);
#endif
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_DISCONNECT\r\n", __func__);
        } break;
        case CODE_WIFI_CMD_RECONNECT: {
        } break;
        case CODE_WIFI_ON_AP_STARTED: {
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_AP_STARTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STOPPED: {
            wl80211_printf("[APP] [EVT] %s, CODE_WIFI_ON_AP_STOPPED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_AP_STA_ADD: {
            wl80211_printf("[APP] [EVT] [AP] [ADD] %lld\r\n", rtos_now(0));
        } break;
        case CODE_WIFI_ON_AP_STA_DEL: {
            wl80211_printf("[APP] [EVT] [AP] [DEL] %lld\r\n", rtos_now(0));
        } break;
        default: {
            wl80211_printf("[APP] [EVT] Unknown code %u \r\n", ev->code);
        }
    }
}
#endif

void wifi_mgmr_init(void)
{
#if !defined(__NuttX__)
    async_register_event_filter(EV_WL80211, wl80211_event_handler, NULL);
    async_register_event_filter(EV_WIFI, wifi_mgmr_event_handler, NULL);

    /* compat legacy wifi mgmr */
    async_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);
    async_post_event(EV_WIFI, CODE_WIFI_ON_MGMR_DONE, 0);
#endif
}

void wifi_mgmr_sta_ps_enter(void)
{
    if (wl80211_sta_set_ps(1)) {
        wl80211_printf("enable ps mode failed!\r\n");
    }
}

void wifi_mgmr_sta_ps_exit(void)
{
    if (wl80211_sta_set_ps(0)) {
        wl80211_printf("exit ps mode failed!\r\n");
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
    struct wl80211_connect_params conn_params;
    int ssid_len, key_len;

    memset(&conn_params, 0, sizeof(struct wl80211_connect_params));
    if (ssid) {
        ssid_len = strlen(ssid);
        if (ssid_len <= 0 || ssid_len > MGMR_SSID_LEN) {
            return -1;
        }
        strlcpy((char *)conn_params.ssid, ssid, 32);
    } else {
        return -1;
    }

    if (key) {
        key_len = strlen(key);
        if ((key_len < 8 && key_len != 5 && key_len != 0) || (key_len > MGMR_KEY_LEN)) {
            return -1;
        }
        strlcpy((char *)conn_params.password, key, 64);
    }

    if (bssid) {
        if (strlen(bssid) > MGMR_BSSID_LEN || parse_mac_addr(bssid, conn_params.bssid) != 0) {
            return -1;
        }
    }

    (void)akm_str;

    if (pmf_cfg < 0 || pmf_cfg > 2) {
        return -1;
    } else {
        conn_params.mfp = pmf_cfg;
    }

    if ((freq1 && wl80211_freq_check(freq1) > 0) || (freq2 && wl80211_freq_check(freq2) > 0) ||
        (freq1 && freq2 && freq1 != freq2)) {
        return -1;
    } else {
        uint16_t freq = freq1 ? freq1 : freq2;
        conn_params.channel = wl80211_freq_to_channel(freq);
    }

    (void)use_dhcp;

    wl80211_sta_connect(&conn_params);
    return 0;
}

int wifi_sta_disconnect(void)
{
    return wl80211_sta_disconnect();
}

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6])
{
    if (wl80211_sta_get_bssid(bssid) < 0) {
        return -1;
    }
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
    int ch;
    ch = wl80211_sta_get_channel();
    if (ch < 0) {
        return -1;
    }
    *channel = ch;

    return 0;
}

int wifi_mgmr_sta_aid_get(void)
{
    return wl80211_sta_get_aid();
}

int wifi_mgmr_sta_state_get(void)
{
    return wl80211_sta_is_connected();
}

void wifi_mgmr_sta_info_dump(wifi_mgmr_connect_ind_stat_info_t *connection_info)
{
    int passphr_len = strlen(connection_info->passphr);
    wl80211_printf("Connected\r\n");
    wl80211_printf("ssid:            %s\r\n", connection_info->ssid);
    // fix show error when using wps
    wl80211_printf("passphr:         %s\r\n", (passphr_len || connection_info->security) ? "********" : "Open");
    wl80211_printf("bssid:           %02X:%02X:%02X:%02X:%02X:%02X\r\n", connection_info->bssid[0],
                   connection_info->bssid[1], connection_info->bssid[2], connection_info->bssid[3],
                   connection_info->bssid[4], connection_info->bssid[5]);
    wl80211_printf("aid:             %d\r\n", connection_info->aid);
    wl80211_printf("channel:         %d\r\n", connection_info->channel);
    wl80211_printf("band:            %s\r\n", (connection_info->chan_band) ? "5G" : "2.4G");
    wl80211_printf("security:        %d\r\n", connection_info->security);
}

int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat)
{
    if (wifi_mgmr_sta_state_get()) {
        char ssid[33];
        char password[65];
        uint8_t bssid[6];
        wl80211_sta_get_ssid(ssid);
        wl80211_sta_get_password(password);
        wl80211_sta_get_bssid(bssid);

        int ssid_len = strlen(ssid);
        if (ssid_len > 0) {
            memcpy(wifi_mgmr_ind_stat->ssid, ssid, ssid_len);
            wifi_mgmr_ind_stat->ssid[ssid_len] = '\0';
        }

        int passphr_len = strlen(password);
        if (passphr_len > 0) {
            memcpy(wifi_mgmr_ind_stat->passphr, password, passphr_len);
            wifi_mgmr_ind_stat->passphr[passphr_len] = '\0';
        }

        memcpy(wifi_mgmr_ind_stat->bssid, bssid, 6);
        wifi_mgmr_ind_stat->aid = wl80211_sta_get_aid();
        wifi_mgmr_ind_stat->channel = wl80211_sta_get_channel();
        wifi_mgmr_ind_stat->chan_band = wl80211_channel_check(wifi_mgmr_ind_stat->channel);
        assert(wifi_mgmr_ind_stat->chan_band != (uint8_t)-1);
        wifi_mgmr_ind_stat->security = wl80211_glb.security; // FIXME

        wifi_mgmr_sta_info_dump(wifi_mgmr_ind_stat);

        return 0;
    } else {
        wl80211_printf("No Connect\r\n");
        return -1;
    }
}

int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config)
{
    struct wl80211_scan_params scan_params;
    memset(&scan_params, 0, sizeof(struct wl80211_scan_params));

    scan_params.ssid_len = config->ssid_length;
    if (config->ssid_length) {
        scan_params.ssid = config->ssid_array;
    }
    if (config->bssid_set_flag) {
        scan_params.bssid = config->bssid;
    }

    if (config->channels_cnt) {
        scan_params.channels_cnt = config->channels_cnt;
        scan_params.channels = config->channels;
    }

    scan_params.probe_cnt = config->probe_cnt;
    scan_params.probe_interval = config->duration;
    if (config->passive) {
        scan_params.flags |= WL80211_SCAN_FLAGS_PASSIVE;
    }

    return wl80211_scan(&scan_params);
}

static void auth_cipher_convert(struct wl80211_scan_result_item *result, uint8_t *auth, uint8_t *cipher)
{
    if (result->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN) {
        if (result->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
            *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK;
            if (result->key_mgmt & WPA_KEY_MGMT_SAE) {
                *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE;
            }
        } else if (result->key_mgmt & WPA_KEY_MGMT_SAE) {
            *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE;
        } else if (result->key_mgmt & (WPA_KEY_MGMT_IEEE8021X | WPA_KEY_MGMT_IEEE8021X_SHA256)) {
            *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT;
        }
    } else if (result->flags & WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA) {
        if (result->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
            *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK;
        } else if (result->key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
            *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT;
        }
    }

#define TEST_WPA_RSN (WL80211_SCAN_AP_RESULT_FLAGS_HAS_RSN | WL80211_SCAN_AP_RESULT_FLAGS_HAS_WPA)
    if ((result->flags & TEST_WPA_RSN) == TEST_WPA_RSN) {
        if (*auth != WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT) {
            *auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK;
        }
    } else if ((result->flags & TEST_WPA_RSN) == 0) {
        *auth = WIFI_EVENT_BEACON_IND_AUTH_WEP;
        *cipher = WIFI_EVENT_BEACON_IND_CIPHER_WEP;
    }
#undef TEST_WPA_RSN

#define TEST_CCMP_TKIP (WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP | WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP)
    if ((result->flags & (TEST_CCMP_TKIP)) == WL80211_SCAN_AP_RESULT_FLAGS_HAS_CCMP) {
        *cipher = WIFI_EVENT_BEACON_IND_CIPHER_AES;
    }
    if ((result->flags & (TEST_CCMP_TKIP)) == WL80211_SCAN_AP_RESULT_FLAGS_HAS_TKIP) {
        *cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP;
    }
    if ((result->flags & (TEST_CCMP_TKIP)) == TEST_CCMP_TKIP) {
        *cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES;
    }
#undef TEST_CCMP_TKIP

    if (result->key_mgmt == 0) {
        *auth = WIFI_EVENT_BEACON_IND_AUTH_OPEN;
        *cipher = WIFI_EVENT_BEACON_IND_CIPHER_NONE;
    }
}

static void wifi_mgmr_sta_scanlist_format(struct wl80211_scan_result_item *result, wifi_mgmr_scan_item_t *item)
{
    uint8_t auth = 0, cipher = 0;

    auth_cipher_convert(result, &auth, &cipher);

    item->mode = result->mode;
    item->timestamp_lastseen = 0;
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

uint32_t wifi_mgmr_sta_scanlist_dump(void *results, uint32_t resultNums)
{
    struct wl80211_scan_result_item *n;
    int total = 0, i;
    wifi_mgmr_scan_item_t *scanResults = (wifi_mgmr_scan_item_t *)results;
    struct wl80211_scan_result_item **rssi_sorted = NULL;

    RB_FOREACH(n, _scan_result_tree, &wl80211_scan_result)
    {
        total++;
    }

    if (total == 0) {
        return 0;
    }

    rssi_sorted = calloc(total, sizeof(struct wl80211_scan_result_item *));
    assert(rssi_sorted);

    i = 0;
    RB_FOREACH(n, _scan_result_tree, &wl80211_scan_result)
    {
        rssi_sorted[i++] = n;
    }

    qsort(rssi_sorted, total, sizeof(void *), cmp_rssi_desc);

    for (i = 0; i < resultNums && i < resultNums; i++) {
        memset(scanResults + i, 0, sizeof(wifi_mgmr_scan_item_t));
        wifi_mgmr_sta_scanlist_format(rssi_sorted[i], scanResults + i);
    }

    free(rssi_sorted);

    return i;
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

int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *config)
{
    int ret;
    struct wl80211_connect_params params;

    memset(&params, 0, sizeof(struct wl80211_connect_params));

    /* freq (channel) */
    params.channel = wl80211_freq_to_channel(config->freq1);
    if (params.channel == (uint8_t)-1) {
        params.channel = 0;
    }

    // FIXME pmf_cfg => mfp convert
    params.mfp = config->pmf_cfg;

    /* ssid */
    memcpy(params.ssid, config->ssid, 32);

    /* password */
    memcpy(params.password, config->key, 64);

    /* bssid */
    uint8_t bssid_addr[6] = { 0 };
    if (strlen(config->bssid_str)) {
        ret = wifi_mgmr_mac_str_to_addr(config->bssid_str, bssid_addr);
        if (ret) {
            return ret;
        }
    }
    memcpy(params.bssid, &bssid_addr, 6);

    /* listen_interval */
    params.listen_interval = wifi_mgmr_listen_itv;

    /* start scan and connect */
    ret = wl80211_sta_connect(&params);
    if (ret) {
        return ret;
    }

    return 0;
}

int wifi_mgmr_sta_disconnect(void)
{
    return wl80211_sta_disconnect();
}

// XXX need this?
int wifi_mgmr_sta_set_listen_itv(uint16_t itv)
{
    wifi_mgmr_listen_itv = itv;
    return 0;
}

uint16_t wifi_mgmr_sta_get_listen_itv(void)
{
    return wifi_mgmr_listen_itv;
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

int wifi_mgmr_sta_autoconnect_enable(void)
{
    wifi_mgmr_disable_autoconnect = 0;
    wl80211_printf("Enable Auto Reconnect\r\n");
    return 0;
}

int wifi_mgmr_sta_autoconnect_disable(void)
{
    wifi_mgmr_disable_autoconnect = 1;
    wl80211_printf("Disable Auto Reconnect\r\n");
    return 0;
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
    const struct ieee80211_dot_d *country;
    country = wl80211_get_country();
    assert(country != NULL);

    if (c24G_cnt != NULL) {
        *c24G_cnt = country->channel24G_num;
    }

    if (c5G_cnt != NULL) {
        *c5G_cnt = country->channel5G_num;
    }

    return 0;
}

int wifi_mgmr_get_channel_list(const char *country_code, uint8_t **c24G_list, uint8_t **c5G_list)
{
    const struct ieee80211_dot_d *country;
    country = wl80211_get_country();
    assert(country != NULL);

    *c24G_list = (uint8_t *)&country->channel24G_chan[0];
    *c5G_list = (uint8_t *)&country->channel5G_chan[0];

    return 0;
}

int wifi_mgmr_set_country_code(char *country_code)
{
    return wl80211_set_country_code(country_code);
}

int wifi_mgmr_get_country_code(char *country_code)
{
    const char *c = wl80211_get_country_code();
    if (!c) {
        return -1;
    }
    memcpy(country_code, c, 3); // XXX
    return 0;
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

int wifi_mgmr_mac_str_to_addr(const char *str, uint8_t addr[])
{
    const char *ptr = str;
    uint32_t i;

    if (!str || strlen(str) < 17)
        return -1;

    for (i = 0; i < 6; i++) {
        char *next;
        long int hex = strtol(ptr, &next, 16);
        if (((unsigned)hex > 255) || (next == ptr) || ((i < 5) && (*next != ':')) || ((i == 5) && (*next != '\0'))) {
            return -1;
        }
        ptr = ++next;

        if (addr) {
            ((uint8_t *)addr)[i] = (uint8_t)hex;
        }
    }

    return 0;
}

int wifi_mgmr_ap_start(const wifi_mgmr_ap_params_t *config)
{
    struct wl80211_ap_settings ap_setting;

    if ((!config) || (!config->ssid)) {
        printf("arg error.\r\n");
        return -1;
    }

    memset(&ap_setting, 0, sizeof(ap_setting));

    /* Set SSID */
    strncpy((char *)ap_setting.ssid, config->ssid, 32);

    /* Set password if provided */
    if (config->key != NULL) {
        strncpy((char *)ap_setting.password, config->key, 64);
        ap_setting.auth_type = WL80211_AUTHTYPE_OPEN_SYSTEM; /* WPA2-PSK with password */
        wl80211_printf("Starting AP: SSID=%s, Password=***\r\n", ap_setting.ssid);
    } else {
        ap_setting.auth_type = WL80211_AUTHTYPE_OPEN_SYSTEM; /* Open system */
        wl80211_printf("Starting AP: SSID=%s, Open\r\n", ap_setting.ssid);
    }

    /* Set AP parameters from config */
    if (config->channel != 0) {
        ap_setting.center_freq1 = wl80211_channel_to_freq(config->channel);
        wl80211_printf("AP Channel: %d\r\n", config->channel);
    } else {
        ap_setting.center_freq1 = wl80211_channel_to_freq(11); /* Default channel 11 */
    }

    ap_setting.channel_width = WL80211_CHAN_WIDTH_20;
    ap_setting.beacon_interval = 100;
    ap_setting.max_power = 0x14;

#if !defined(__NuttX__)
    wifi_mgmr_ap_netif_cfg_save(config);
#endif

    if (wl80211_ap_start(&ap_setting) != 0) {
#if !defined(__NuttX__)
        wifi_mgmr_ap_netif_cfg_clear();
#endif
        wl80211_printf("Error: Failed to start AP\r\n");
        return -1;
    }

    return 0;
}
int wifi_mgmr_ap_stop(void)
{
#if !defined(__NuttX__)
    wifi_mgmr_ap_netif_stop();
#endif
    /* Stop AP */
    if (wl80211_ap_stop() != 0) {
        wl80211_printf("Error: Failed to stop AP\r\n");
    } else {
        wl80211_printf("AP stopped\r\n");
    }

    return 0;
}
#endif /* COMPAT_WIFI_MGMR */
