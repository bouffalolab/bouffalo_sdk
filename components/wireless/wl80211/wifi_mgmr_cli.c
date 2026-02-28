#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "wl80211.h"
#include "supplicant_api.h"
#include "wl80211_platform.h"
#include "wifi_mgmr.h"

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

    utils_getopt_init(&getopt_env, 0);

    /* Parsing parameters */
    while ((opt = utils_getopt(&getopt_env, argc, argv, "b:f:c:")) >= 0) {
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
    wl80211_printf("[USAGE]: %s [-b <bssid>] [-c <ch_idx>] <ssid> [password]\r\n", argv[0]);
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

    wifi_mgmr_scan_params_t config;
    memset(&config, 0, sizeof(wifi_mgmr_scan_params_t));

    utils_getopt_init(&getopt_env, 0);

    while ((opt = utils_getopt(&getopt_env, argc, argv, "s:c:b:t:p:")) >= 0) {
        switch (opt) {
        case 's': {
            memcpy(config.ssid_array, getopt_env.optarg, MGMR_SSID_LEN);
            config.ssid_length = strlen(getopt_env.optarg);
            wl80211_printf("ssid: %s len: %d\r\n", config.ssid_array, config.ssid_length);
        }
        break;
        case 'c': {
            utils_parse_number_adv(getopt_env.optarg, ',', config.channels, MAX_FIXED_CHANNELS_LIMIT, 10, &config.channels_cnt);
            wl80211_printf("scan channels: ");
            for (int i = 0; i < config.channels_cnt; i++) {
                wl80211_printf("%d ", config.channels[i]);
            }
            wl80211_printf("\r\n");
        }
        break;
        case 'b': {
            config.bssid_set_flag = 1;
            utils_parse_number(getopt_env.optarg, ':', config.bssid, 6, 16);
            wl80211_printf("bssid: %s, mac:%02X:%02X:%02X:%02X:%02X:%02X\r\n", getopt_env.optarg,
                    MAC_ADDR_LIST(config.bssid));
        }
        break;
        case 't': {
            config.duration = atoi(getopt_env.optarg);
        }
        break;
        case 'p': {
            config.probe_cnt = atoi(getopt_env.optarg);
        }
        break;
        default:
            wl80211_printf("unknown option: %c\r\n", getopt_env.optopt);
            break;
        }
    }

    ret = wifi_mgmr_sta_scan(&config);
    if (ret < 0) {
        wl80211_printf("scan failed\r\n");
        return;
    }
}

void wifi_info_cmd(int argc, char **argv)
{
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_ind_stat = {0};

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

void wifi_country_code_set_cmd(int argc, char **argv)
{
    int ret = 0;
    char country_code[3] = {0};

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
            int channel = atoi(getopt_env.optarg);
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
    ap_setting.beacon_interval = 100;
    ap_setting.center_freq1 = wl80211_channel_to_freq(11); /* Default channel 11 */
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
            wl80211_printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                          wl80211_glb.aid_list[i].mac[0],
                          wl80211_glb.aid_list[i].mac[1],
                          wl80211_glb.aid_list[i].mac[2],
                          wl80211_glb.aid_list[i].mac[3],
                          wl80211_glb.aid_list[i].mac[4],
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

/* CLI command registration moved to wl80211_platform.c (platform-specific) */
