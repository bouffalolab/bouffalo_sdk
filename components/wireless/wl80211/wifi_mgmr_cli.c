#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "wl80211.h"
#include "supplicant_api.h"
#include "wl80211_platform.h"
#include "wifi_mgmr.h"
#include "macsw.h" // for export_XXX

#include "utils_getopt.h"

#define MAC_ADDR_LIST(m) (m)[0], (m)[1], (m)[2], (m)[3], (m)[4], (m)[5]

int wifi_connect_cmd(int argc, char **argv)
{
    int ret;
    int opt;
    getopt_env_t getopt_env = { 0 };
    wifi_mgmr_sta_connect_params_t conn_param = { 0 };
    uint8_t channel_index = 0xff;

    if (argc < 2) {
        goto _ERROUT;
    }

    conn_param.use_dhcp = 1;
    utils_getopt_init(&getopt_env, 0);

    /* Parsing parameters */
    while ((opt = utils_getopt(&getopt_env, argc, argv, "b:f:c:D")) >= 0) {
        switch (opt) {
            case 'b':
                if (wifi_mgmr_mac_str_to_addr(getopt_env.optarg, NULL) < 0) {
                    wl80211_printf("connect bssid Invalid: %s\r\n", getopt_env.optarg);
                    goto _ERROUT;
                }
                memcpy(conn_param.bssid_str, getopt_env.optarg, MGMR_BSSID_LEN);
                wl80211_printf("connect bssid: %s\r\n", conn_param.bssid_str);
                break;

            case 'f':
                conn_param.pmf_cfg = atoi(getopt_env.optarg);
                if (conn_param.pmf_cfg > 2) {
                    wl80211_printf("connect pmf_cfg Invalid: %d\r\n", conn_param.pmf_cfg);
                    goto _ERROUT;
                }
                wl80211_printf("connect pmf_cfg: %d\r\n", conn_param.pmf_cfg);
                break;

            case 'c':
                channel_index = atoi(getopt_env.optarg);
                conn_param.freq1 = wl80211_channel_to_freq(channel_index);
                if (conn_param.freq1 > 0) {
                    wl80211_printf("connect channel_index: %d(%dMhz)\r\n", channel_index, conn_param.freq1);
                } else {
                    wl80211_printf("connect channel_index Invalid: %d\r\n", channel_index);
                    goto _ERROUT;
                }
                break;

            case 'D':
                conn_param.use_dhcp = 0;
                break;

            default:
                wl80211_printf("connect unknow option: %c\r\n", getopt_env.optopt);
                goto _ERROUT;
        }
    }

    /* ssid */
    if (argc > getopt_env.optind) {
        strncpy(conn_param.ssid, argv[getopt_env.optind], MGMR_SSID_LEN);
        wl80211_printf("connect SSID: %s\r\n", conn_param.ssid);
    } else {
        wl80211_printf("connect Expected SSID\r\n");
        goto _ERROUT;
    }

    /* password */
    if (argc > getopt_env.optind + 1) {
        strncpy(conn_param.key, argv[getopt_env.optind + 1], MGMR_KEY_LEN);
        wl80211_printf("connect password: %s\r\n", conn_param.key);
    } else {
        conn_param.key[0] = '\0';
        wl80211_printf("connect: No password\r\n");
    }

    /* connect start */
    ret = wifi_mgmr_sta_connect(&conn_param);
    if (ret < 0) {
        wl80211_printf("sta_connect FAILD: %d\r\n", ret);
    }
    return 0;

_ERROUT:
    wl80211_printf("[USAGE]: %s [-D] [-b <bssid>] [-c <ch_idx>] <ssid> [password]\r\n", argv[0]);
    return 0;
}

int wifi_disconnect_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_disconnect();

    return 0;
}

void wifi_scan_cmd(int argc, char **argv)
{
    int opt, ret;
    getopt_env_t getopt_env;
    int keep = 0;

    wifi_mgmr_scan_params_t config;
    memset(&config, 0, sizeof(wifi_mgmr_scan_params_t));

    utils_getopt_init(&getopt_env, 0);

    while ((opt = utils_getopt(&getopt_env, argc, argv, "s:c:b:t:p:k")) >= 0) {
        switch (opt) {
            case 's': {
                memcpy(config.ssid_array, getopt_env.optarg, MGMR_SSID_LEN);
                config.ssid_length = strlen(getopt_env.optarg);
                wl80211_printf("ssid: %s len: %d\r\n", config.ssid_array, config.ssid_length);
            } break;
            case 'c': {
                utils_parse_number_adv(getopt_env.optarg, ',', config.channels, MAX_FIXED_CHANNELS_LIMIT, 10,
                                       &config.channels_cnt);
                wl80211_printf("scan channels: ");
                for (int i = 0; i < config.channels_cnt; i++) {
                    wl80211_printf("%d ", config.channels[i]);
                }
                wl80211_printf("\r\n");
            } break;
            case 'b': {
                config.bssid_set_flag = 1;
                utils_parse_number(getopt_env.optarg, ':', config.bssid, 6, 16);
                wl80211_printf("bssid: %s, mac:%02X:%02X:%02X:%02X:%02X:%02X\r\n", getopt_env.optarg,
                               MAC_ADDR_LIST(config.bssid));
            } break;
            case 't': {
                config.duration = atoi(getopt_env.optarg);
            } break;
            case 'p': {
                config.probe_cnt = atoi(getopt_env.optarg);
            } break;
            case 'k': {
                keep = 1;
            } break;
            default:
                wl80211_printf("unknown option: %c\r\n", getopt_env.optopt);
                break;
        }
    }

    if (keep == 0) {
        wifi_mgmr_sta_scanlist_free();
    }

    ret = wifi_mgmr_sta_scan(&config);
    if (ret < 0) {
        wl80211_printf("scan failed\r\n");
        return;
    }
}

void wifi_info_cmd(int argc, char **argv)
{
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_ind_stat = { 0 };

    wl80211_printf("\r\n");
    wl80211_printf("STA Information:\r\n");
    wl80211_printf("-------------------------------------\r\n");
    wifi_mgmr_sta_connect_ind_stat_get(&wifi_mgmr_ind_stat);
    wl80211_printf("-------------------------------------\r\n");
}

void wifi_sta_autoconnect_enable_cmd(int argc, char **argv)
{
    int ret = wifi_mgmr_sta_autoconnect_enable();
    if (ret == 0) {
        wl80211_printf("Enable Auto Reconnect\r\n");
    } else {
        wl80211_printf("Failed to enable auto reconnect, ret = %d\r\n", ret);
    }
}

void wifi_sta_autoconnect_disable_cmd(int argc, char **argv)
{
    int ret = wifi_mgmr_sta_autoconnect_disable();
    if (ret == 0) {
        wl80211_printf("Disable Auto Reconnect\r\n");
    } else {
        wl80211_printf("Failed to disable auto reconnect, ret = %d\r\n", ret);
    }
}

void wifi_country_code_get_cmd(int argc, char **argv)
{
    int ret = 0;
    char country_code[3] = { 0 };

    if (argc != 1) {
        wl80211_printf("Usage: wifi_country_code_get\r\n");
        return;
    }

    // Call API to get country code
    ret = wifi_mgmr_get_country_code(country_code);
    if (ret == 0) {
        wl80211_printf("Current country code: %s\r\n", country_code);
    } else {
        wl80211_printf("Error: Failed to get country code, ret = %d\r\n", ret);
    }
}

void wifi_country_code_set_cmd(int argc, char **argv)
{
    int ret = 0;
    char country_code[3] = { 0 };

    if (argc != 2) {
        wl80211_printf("Usage: wifi_country_code_set [country_code]\r\n");
        wl80211_printf("Example: wifi_country_code_set CN\r\n");
        return;
    }

    // Check country code length
    if (strlen(argv[1]) != 2) {
        wl80211_printf("Error: Country code must be 2 characters\r\n");
        return;
    }

    // Copy country code
    strncpy(country_code, argv[1], 2);
    country_code[2] = '\0';

    // Call API to set country code
    ret = wifi_mgmr_set_country_code(country_code);
    if (ret == 0) {
        wl80211_printf("Success: Country code set to %s\r\n", country_code);
    } else {
        wl80211_printf("Error: Failed to set country code, ret = %d\r\n", ret);
    }
}

void wifi_ap_start_cmd(int argc, char **argv)
{
    int opt;
    getopt_env_t getopt_env;
    struct wl80211_ap_settings ap_setting;
    int channel = 11;
    const char *ssid = NULL;
    const char *password = NULL;

    utils_getopt_init(&getopt_env, 0);

    while ((opt = utils_getopt(&getopt_env, argc, argv, "s:k:c:")) >= 0) {
        switch (opt) {
            case 's':
                ssid = getopt_env.optarg;
                break;
            case 'k':
                password = getopt_env.optarg;
                break;
            case 'c': {
                channel = atoi(getopt_env.optarg);
                if (channel < 1 || channel > 14) {
                    wl80211_printf("Error: Invalid channel %d (1-14)\r\n", channel);
                    return;
                }
                /* Will be used in ap_setting.center_freq1 */
                break;
            }
            default:
                wl80211_printf("unknown option: %c\r\n", getopt_env.optopt);
                wl80211_printf("Usage: wifi_ap_start -s <ssid> [-k <password>] [-c <channel>]\r\n");
                return;
        }
    }

    if (ssid == NULL) {
        wl80211_printf("Error: SSID is required\r\n");
        wl80211_printf("Usage: wifi_ap_start -s <ssid> [-k <password>] [-c <channel>]\r\n");
        return;
    }

    memset(&ap_setting, 0, sizeof(ap_setting));

    /* Set SSID */
    strncpy((char *)ap_setting.ssid, ssid, 32);

    /* Set password if provided */
    if (password != NULL) {
        strncpy((char *)ap_setting.password, password, 64);
        ap_setting.auth_type = WL80211_AUTHTYPE_OPEN_SYSTEM; /* WPA2-PSK with password */
        wl80211_printf("Starting AP: SSID=%s, Password=***\r\n", ssid);
    } else {
        ap_setting.auth_type = WL80211_AUTHTYPE_OPEN_SYSTEM; /* Open system */
        wl80211_printf("Starting AP: SSID=%s, Open\r\n", ssid);
    }

    /* Set default AP parameters */
    ap_setting.beacon_interval = 0;
    ap_setting.center_freq1 = wl80211_channel_to_freq(channel); /* Default channel 11 */
    ap_setting.channel_width = WL80211_CHAN_WIDTH_20;
    ap_setting.max_power = 0x14;

    if (wl80211_ap_start(&ap_setting) != 0) {
        wl80211_printf("Error: Failed to start AP\r\n");
    }
}

void wifi_ap_stop_cmd(int argc, char **argv)
{
    if (wl80211_ap_stop() != 0) {
        wl80211_printf("Error: Failed to stop AP\r\n");
    } else {
        wl80211_printf("AP stopped\r\n");
    }
}

void wifi_sta_list_cmd(int argc, char **argv)
{
    extern struct wl80211_global_state wl80211_glb;
    int sta_count = 0;
    int i;

    if (!wl80211_glb.ap_en) {
        wl80211_printf("AP mode is not enabled\r\n");
        return;
    }

    wl80211_printf("\r\n");
    wl80211_printf("AP Station List:\r\n");
    wl80211_printf("========================================\r\n");

    for (i = 0; i < 4; i++) {
        if (wl80211_glb.aid_list[i].used) {
            wl80211_printf("STA %d:\r\n", sta_count + 1);
            wl80211_printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", wl80211_glb.aid_list[i].mac[0],
                           wl80211_glb.aid_list[i].mac[1], wl80211_glb.aid_list[i].mac[2],
                           wl80211_glb.aid_list[i].mac[3], wl80211_glb.aid_list[i].mac[4],
                           wl80211_glb.aid_list[i].mac[5]);
            wl80211_printf("  AID: %d\r\n", i + 1);
            wl80211_printf("  sta_idx: %d\r\n", wl80211_glb.aid_list[i].sta_idx);
            sta_count++;
        }
    }

    if (sta_count == 0) {
        wl80211_printf("No stations connected\r\n");
    } else {
        wl80211_printf("Total: %d station(s)\r\n", sta_count);
    }
    wl80211_printf("========================================\r\n");
}

void wifi_sta_ps_enter_cmd(int argc, char **argv)
{
    if (!wifi_mgmr_sta_state_get()) {
        wl80211_printf("Error: Not connected, cannot enter PS mode\r\n");
        return;
    }

    wifi_mgmr_sta_ps_enter();
    wl80211_printf("WiFi PS mode enabled\r\n");
}

void wifi_sta_ps_exit_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_ps_exit();
    wl80211_printf("WiFi PS mode disabled\r\n");
}

typedef struct rf_pwr_table {
    int8_t pwr_11b[4];
    int8_t pwr_11g[8];
    int8_t pwr_11n_ht20[8];
    int8_t pwr_11n_ht40[8];
    int8_t pwr_11ac_vht20[10];
    int8_t pwr_11ac_vht40[10];
    int8_t reserved[10];
    int8_t pwr_11ax_he20[12];
    int8_t pwr_11ax_he40[12];
    int8_t reserved2[12];
    int8_t reserved3[12];
} rf_pwr_table_t;

static void _print_sta_pwr(uint8_t numb, int8_t *power_table)
{
    uint8_t i;
    for (i = 0; i < numb; i++) {
        printf("%3u ", power_table[i]);
    }
    printf("\r\n");
}

void wifi_sta_info_cmd(int argc, char **argv)
{
    uint32_t ip = 0, mask = 0, gw = 0, dns = 0;
    int rssi = 0;
    rf_pwr_table_t power_table;
    char country_code[3] = { 0 };
    uint8_t channel = 0;

    memset(power_table.pwr_11b, 0, sizeof(rf_pwr_table_t));

    wifi_mgmr_get_country_code(country_code);
    wifi_mgmr_sta_ip_get(&ip, &mask, &gw, &dns);
    wifi_mgmr_sta_rssi_get(&rssi);

#ifndef BL618DG
    extern void bl_tpc_power_table_get(int8_t *power_table);
    bl_tpc_power_table_get((int8_t *)&power_table.pwr_11b);
#endif
    printf("================================================================\r\n");
#ifdef CONFIG_ANTDIV_STATIC
    printf("ANT :    %d\r\n", antenna_hal_get_current_antenna());
#endif
    printf("RSSI:    %ddbm\r\n", rssi);
    printf("IP  :    %d.%d.%d.%d \r\n", (ip) & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
    printf("MASK:    %d.%d.%d.%d \r\n", (mask) & 0xFF, (mask >> 8) & 0xFF, (mask >> 16) & 0xFF, (mask >> 24) & 0xFF);
    printf("GW  :    %d.%d.%d.%d \r\n", (gw) & 0xFF, (gw >> 8) & 0xFF, (gw >> 16) & 0xFF, (gw >> 24) & 0xFF);
    printf("DNS :    %d.%d.%d.%d \r\n", (dns) & 0xFF, (dns >> 8) & 0xFF, (dns >> 16) & 0xFF, (dns >> 24) & 0xFF);
    printf("Country: %s \r\n", country_code);
    if (wifi_mgmr_sta_channel_get(&channel))
        printf("Channel: not connected\r\n");
    else
        printf("Channel: %d \r\n", channel);
#if 0
#ifdef CFG_IPV6
    struct netif *nif;

    NETIF_FOREACH(nif)
    {
        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            const ip6_addr_t *ip6addr = netif_ip6_addr(nif, i);
            if (!ip6_addr_isany(ip6addr)) {
                fhost_printf("IPv6:    %s, state %x\r\n", ip6addr_ntoa(ip6addr), netif_ip6_addr_state(nif, i));
            }
        }
    }
#endif
#endif
    printf("Power Table (dbm):\r\n");
    printf("-----------------------------------------------------------\r\n");
    printf("  11b:         ");
    _print_sta_pwr(4, power_table.pwr_11b);
    printf("  11g:         ");
    _print_sta_pwr(8, power_table.pwr_11g);
    printf("  11n(ht20):   ");
    _print_sta_pwr(8, power_table.pwr_11n_ht20);
    printf("  11n(ht40):   ");
    _print_sta_pwr(8, power_table.pwr_11n_ht40);
    printf("  11ac(vht20): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht20);
    printf("  11ac(vht40): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht40);
#if 0
    printf("  11ac(vht80): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht80);
#endif
    printf("  11ax(he20):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he20);
    printf("  11ax(he40):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he40);
#if 0
    printf("  11ax(he80):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he80);
    printf("  11ax(he160): ");
    _print_sta_pwr(12, power_table.pwr_11ax_he160);
#endif
    printf("  last rx rate: format:%s,mcs:%u \r\n", export_stats_get_rx_format(), export_stats_get_rx_mcs());
    printf("  last tx rate: format:%s,mcs:%u \r\n", export_stats_get_tx_format(), export_stats_get_tx_mcs());
    printf("================================================================\r\n");
}

void wifi_keyram_cmd(int argc, char **argv) {
    void mm_sec_keydump(void);
    mm_sec_keydump();
}

/* CLI command registration moved to wl80211_platform.c (platform-specific) */
