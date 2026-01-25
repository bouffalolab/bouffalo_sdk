#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "wl80211.h"
#include "supplicant_api.h"
#include "wl80211_platform.h"
#include "wifi_mgmr.h"

#if !defined(__NuttX__)
#include "async_event.h"
#endif

#include "utils_getopt.h"
#include "utils_hex.h"

#define DBG_TAG "WIFI_CLI"
#include "log.h"

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
                    LOG_W("connect bssid Invalid: %s\r\n", getopt_env.optarg);
                    goto _ERROUT;
                }
                memcpy(conn_param.bssid_str, getopt_env.optarg, MGMR_BSSID_LEN);
                LOG_I("connect bssid: %s\r\n", conn_param.bssid_str);
                break;

            case 'f':
                conn_param.pmf_cfg = atoi(getopt_env.optarg);
                if (conn_param.pmf_cfg > 2) {
                    LOG_W("connect pmf_cfg Invalid: %d\r\n", conn_param.pmf_cfg);
                    goto _ERROUT;
                }
                LOG_I("connect pmf_cfg: %d\r\n", conn_param.pmf_cfg);
                break;

            case 'c':
                channel_index = atoi(getopt_env.optarg);
                conn_param.freq1 = wl80211_channel_to_freq(channel_index);
                if (conn_param.freq1 > 0) {
                    LOG_I("connect channel_index: %d(%dMhz)\r\n", channel_index, conn_param.freq1);
                } else {
                    LOG_I("connect channel_index Invalid: %d\r\n", channel_index);
                    goto _ERROUT;
                }
                break;

            default:
                LOG_W("connect unknow option: %c\r\n", getopt_env.optopt);
                goto _ERROUT;
        }
    }

    /* ssid */
    if (argc > getopt_env.optind) {
        strncpy(conn_param.ssid, argv[getopt_env.optind], MGMR_SSID_LEN);
        LOG_I("connect SSID: %s\r\n", conn_param.ssid);
    } else {
        LOG_W("connect Expected SSID\r\n");
        goto _ERROUT;
    }

    /* password */
    if (argc > getopt_env.optind + 1) {
        strncpy(conn_param.key, argv[getopt_env.optind + 1], MGMR_KEY_LEN);
        LOG_I("connect password: %s\r\n", conn_param.key);
    } else {
        conn_param.key[0] = '\0';
        LOG_I("connect: No password\r\n");
    }

    /* connect start */
    ret = wifi_mgmr_sta_connect(&conn_param);
    if (ret < 0) {
        LOG_E("sta_connect FAILD: %d\r\n", ret);
    }
    return 0;

_ERROUT:
    LOG_W("[USAGE]: %s [-b <bssid>] [-c <ch_idx>] <ssid> [password]\r\n", argv[0]);
    return 0;
}

int wifi_disconnect_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_disconnect();

    return 0;
}

#if 0
void wifi_scan_cmd(int argc, char **argv)
{
    int opt, ret;
    getopt_env_t getopt_env;

    wifi_mgmr_scan_params_t config;
    memset(&config, 0 , sizeof(wifi_mgmr_scan_params_t));

    utils_al_getopt_init(&getopt_env, 0);

    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "s:c:b:t:p:")) != -1) {
         switch (opt) {
             case 's':
             {
                 memcpy(config.ssid_array, getopt_env.optarg, MAC_SSID_LEN);
                 config.ssid_length = strlen(getopt_env.optarg);
                 fhost_printf("ssid: %s len: %d\r\n", config.ssid_array, config.ssid_length);
             }
             break;
             case 'c':
             {
                 utils_al_parse_number_adv(getopt_env.optarg, ',', config.channels, MAX_FIXED_CHANNELS_LIMIT, 10, &config.channels_cnt);
                 fhost_printf("scan channels: ");
                 for (int i = 0; i < config.channels_cnt; i++) {
                     fhost_printf("%d ", config.channels[i]);
                 }
             }
             break;
             case 'b':
             {
                 config.bssid_set_flag = 1;
                 utils_al_parse_number(getopt_env.optarg, ':', config.bssid, 6, 16);
                 fhost_printf("bssid: %s, mac:%02X:%02X:%02X:%02X:%02X:%02X\r\n", getopt_env.optarg,
                          MAC_ADDR_LIST(config.bssid));
             }
             break;
             case 't':
             {
                 config.duration = atoi(getopt_env.optarg);
             }
             break;
             case 'p':
             {
                 config.probe_cnt = atoi(getopt_env.optarg);
             }
             break;
             default:
             {
                 fhost_printf("unknow option: %c\r\n", getopt_env.optopt);
             }
         }

    }

    if (wl80211_scan(NULL)) {
        printf("scan failed \r\n");
    }
}

static void wifi_scan_cmd(int argc, char **argv)
{
    return;
}
#endif

#ifdef CONFIG_SHELL
#include "shell.h"
SHELL_CMD_EXPORT_ALIAS(wifi_connect_cmd, wifi_sta_connect, wifi station connect);
SHELL_CMD_EXPORT_ALIAS(wifi_disconnect_cmd, wifi_sta_disconnect, wifi station disconnect);
//SHELL_CMD_EXPORT_ALIAS(wifi_scan_cmd, wifi_scan, wifi scan);
#endif
