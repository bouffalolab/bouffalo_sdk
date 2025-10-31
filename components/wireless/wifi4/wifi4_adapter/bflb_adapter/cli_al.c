#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "utils_getopt.h"
#include "utils_hex.h"
#include "net_iperf_al_priv.h"
#include "net_iperf_al.h"
#ifdef IOT_SDK_ADAPTER
#include "utils_string.h"
#include "cli.h"
#include "utils_hexdump.h"
#include "bflb_os_system.h"
#endif

#ifdef CFG_IPV6
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#endif
#include "wifi_mgmr_cli.h"
#include "wifi_mgmr_ext.h"

#ifdef IOT_SDK_ADAPTER
#define SHELL_CMD_EXPORT_ALIAS(func, name, desc)                                                                \
    static void func##_adapter (char *buf, int len, int argc, char **argv)                                    \
    {                                                                                                           \
        func(argc, argv);                                                                                       \
                                                                                                                \
    }                                                                                                           \
    const static struct cli_command name##cli[] STATIC_CLI_CMD_ATTRIBUTE = {                                    \
                                                                            {#name, #desc , func##_adapter}   \
    }
#else
#include "shell.h"
#endif

void utils_al_parse_number_adv(const char *str, char sep, uint8_t *buf, int buflen, int base, int *count)
{
    utils_parse_number_adv(str, sep, buf, buflen, base, count);
}

void utils_al_parse_number(const char *str, char sep, uint8_t *buf, int buflen, int base)
{
    utils_parse_number(str, sep, buf, buflen, base);
}

int utils_al_getopt_init(getopt_env_t *env, int opterr)
{
    return utils_getopt_init(env, opterr);
}

int utils_al_getopt(getopt_env_t *env, int argc, char * const argv[], const char *optstring)
{
    return utils_getopt(env, argc, argv, optstring);
}

char *utils_al_bin2hex(char *dst, const void *src, size_t count)
{
    return utils_bin2hex(dst, src, count);
}

int fhost_ipc_parse_ip4(char *str, uint32_t *ip, uint32_t *mask)
{
    char *token;
    uint32_t a, i, j;

    #define check_is_num(_str)  for (j = 0 ; j < strlen(_str); j++) {  \
            if (_str[j] < '0' || _str[j] > '9')                        \
                return -1;                                             \
        }

    // Check if mask is present
    token = strchr(str, '/');
    if (token && mask) {
        *token++ = '\0';
        check_is_num(token);
        a = atoi(token);
        if (a == 0 || a > 32)
            return -1;
        *mask = (1<<a) - 1;
    }
    else if (mask)
    {
        *mask = 0xffffffff;
    }

    // parse the ip part
    *ip = 0;
    for (i = 0; i < 4; i ++)
    {
        if (i < 3)
        {
            token = strchr(str, '.');
            if (!token)
                return -1;
            *token++ = '\0';
        }
        check_is_num(str);
        a = atoi(str);
        if (a > 255)
            return -1;
        str = token;
        *ip += (a << (i * 8));
    }

    return 0;
}

void lwip_cmd(int argc, char **argv)
{
void stats_display(void);
    stats_display();
}

void cmd_iperf(int argc, char **argv)
{
    char conv, *token, *substr;
    struct fhost_iperf_settings iperf_settings;
    bool client_server_set = 0;
    int opt;
    getopt_env_t getopt_env;
    rtos_task_handle iperf_handle;

    if(argc >= 2 && !strcmp("stop", argv[1])) {
        if((iperf_handle = fhost_iperf_msg_handle_get()) != NULL) {
            fhost_iperf_sigkill_handler(iperf_handle);
            return;
        }
        printf("havn't start iperf \r\n");
        return;
    }

    fhost_iperf_settings_init(&iperf_settings);
    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "b:w:c:f:i:l:n:p:sut:S:T:XI:h")) != -1) {
        switch (opt) {
        case 'b': // UDP bandwidth
        case 'w': // TCP window size
        {
            char *decimal_str;
            int decimal = 0;
            uint64_t value;

            token = getopt_env.optarg;
            decimal_str = strchr(token, '.');
            if (decimal_str) {
                int fact = 100;
                decimal_str++;
                while (*decimal_str >= '0' && *decimal_str <= '9') {
                    decimal += (*decimal_str - '0') * fact;
                    if (fact == 1)
                        break;
                    fact = fact / 10;
                    decimal_str++;
                }
            }

            value = atoi(token);
            conv = token[strlen(token) - 1];

            // convert according to [Gg Mm Kk]
            switch (conv) {
            case 'G':
            case 'g':
                value *= 1000000000;
                value += decimal * 1000000;
                break;
            case 'M':
            case 'm':
                value *= 1000000;
                value += decimal * 1000;
                break;
            case 'K':
            case 'k':
                value *= 1000;
                value += decimal;
                break;
            default:
                break;
            }

            if (opt == 'b') {
                iperf_settings.udprate = value;
                iperf_settings.flags.is_udp = true;
                iperf_settings.flags.is_bw_set = true;
                // if -l has already been processed, is_buf_len_set is true so don't overwrite that value.
                if (!iperf_settings.flags.is_buf_len_set)
                    iperf_settings.buf_len = FHOST_IPERF_DEFAULT_UDPBUFLEN;
            }
            else {
                // TCP window is ignored for now
            }
            break;
        }
        case 'c': // Client mode with server host to connect to
        {
            if (client_server_set)
                goto help;

            iperf_settings.flags.is_server = 0;
            client_server_set = true;

            if (fhost_ipc_parse_ip4(getopt_env.optarg, &iperf_settings.host_ip, NULL)) {
                printf("invalid IP address %s\n", getopt_env.optarg);
                return;  //FHOST_IPC_ERROR;
            }
            break;
        }
        case 'f': // format to print in
        {
            iperf_settings.format = getopt_env.optarg[0];
            break;
        }
        case 'i': // Interval between periodic reports
        {
            uint32_t interval = 0;
            substr = strchr(getopt_env.optarg, '.');

            if (substr) {
                *substr++ = '\0';
                interval += atoi(substr);
            }

            interval += atoi(getopt_env.optarg) * 10;
            if (interval < 5) {
                printf("interval must be greater than or "
                            "equal to 0.5. Interval set to 0.5\n");
                interval = 5;
            }

            iperf_settings.interval.sec = interval / 10;
            iperf_settings.interval.usec = 100000 * (interval - (iperf_settings.interval.sec * 10));
            iperf_settings.flags.show_int_stats = true;
            break;
        }
        case 'l': //Length of each buffer
        {
            uint32_t udp_min_size = sizeof(struct iperf_UDP_datagram);

            iperf_settings.buf_len = atoi( getopt_env.optarg );
            iperf_settings.flags.is_buf_len_set = true;
            if (iperf_settings.flags.is_udp && iperf_settings.buf_len < udp_min_size) {
                iperf_settings.buf_len = udp_min_size;
                printf("buffer length must be greater than or "
                            "equal to %d in UDP\n", udp_min_size);
            }
            break;
        }
        case 'n': // amount mode (instead of time mode)
        {
            iperf_settings.flags.is_time_mode = false;
            iperf_settings.amount = atoi( getopt_env.optarg );
            break;
        }
        case 'p': //server port
        {
            iperf_settings.port = atoi( getopt_env.optarg );
            break;
        }
        case 's': // server mode
        {
            if (client_server_set)
                goto help;
            iperf_settings.flags.is_server = 1;
            client_server_set = true;
            break;
        }
        case 't': // time mode (instead of amount mode)
        {
            iperf_settings.flags.is_time_mode = true;
            iperf_settings.amount = 0;
            substr = strchr(getopt_env.optarg, '.');
            if (substr) {
                *substr++ = '\0';
                iperf_settings.amount += atoi(substr);
            }

            iperf_settings.amount += atoi(getopt_env.optarg) * 10;
            break;
        }
        case 'u': // UDP instead of TCP
        {
            // if -b has already been processed, UDP rate will be non-zero, so don't overwrite that value
            if (!iperf_settings.flags.is_udp) {
                iperf_settings.flags.is_udp = true;
                iperf_settings.udprate = FHOST_IPERF_DEFAULT_UDPRATE;
            }

            // if -l has already been processed, is_buf_len_set is true, so don't overwrite that value.
            if (!iperf_settings.flags.is_buf_len_set) {
                iperf_settings.buf_len = FHOST_IPERF_DEFAULT_UDPBUFLEN;
            }
            break;
        }
        case 'S': // IP type-of-service
        {
            // the zero base allows the user to specify
            // hexadecimals: "0x#"
            // octals: "0#"
            // decimal numbers: "#"
            iperf_settings.tos = strtol( getopt_env.optarg, NULL, 0 );
            break;
        }
        case 'T': // TTL
        {
            iperf_settings.ttl = atoi(getopt_env.optarg);
            break;
        }
        case 'X': // Peer version detect
        {
            iperf_settings.flags.is_peer_ver = true;
            break;
        }
        case 'I': // vif num
        {
            uint16_t vif_num;
            vif_num = atoi(getopt_env.optarg);
            if (vif_num == MGMR_VIF_STA) {
                iperf_settings.vif_num = MGMR_VIF_STA;
            } else if (vif_num == MGMR_VIF_AP) {
                iperf_settings.vif_num = MGMR_VIF_AP;
            } else {
                printf(iperf_long_help);
                return; //FHOST_IPC_ERROR;
            }
            break;
        }
        case 'h': // Long Help
        {
        help:
            printf(iperf_long_help);
            return;
        }
        default:
        {
            goto help;
        }
        }
    }

    if (!client_server_set)
        goto help;

    fhost_iperf_start(&iperf_settings);
}

SHELL_CMD_EXPORT_ALIAS(cmd_rf_dump, rf_dump, rf dump);
SHELL_CMD_EXPORT_ALIAS(lwip_cmd, lwip, show stats);
SHELL_CMD_EXPORT_ALIAS(cmd_iperf, iperf, iperf test throughput);

SHELL_CMD_EXPORT_ALIAS(wifi_bcnint_set, wifi_ap_bcnint_set, wifi ap bcnin set);
SHELL_CMD_EXPORT_ALIAS(wifi_capcode_cmd, wifi_capcode, wifi capcode );
SHELL_CMD_EXPORT_ALIAS(wifi_scan_cmd, wifi_scan, wifi scan );
SHELL_CMD_EXPORT_ALIAS(wifi_scan_filter_cmd, wifi_scan_filter, wifi scan );
SHELL_CMD_EXPORT_ALIAS(wifi_mon_cmd, wifi_mon, wifi monitor);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_raw_send, wifi_raw_send, wifi raw send test );
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ip_info, wifi_sta_info, wifi sta info );
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ip_set_cmd, wifi_sta_ip_set, wifi STA IP config [ip] [mask] [gw] [dns1] [dns2]);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ip_unset_cmd, wifi_sta_ip_unset, wifi STA IP config unset);
SHELL_CMD_EXPORT_ALIAS(wifi_disconnect_cmd, wifi_sta_disconnect, wifi station disconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_connect_cmd, wifi_sta_connect, wifi station connect);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_get_state_cmd, wifi_sta_get_state, wifi sta get state);
SHELL_CMD_EXPORT_ALIAS(wifi_enable_autoreconnect_cmd, wifi_sta_autoconnect_enable, wifi station enable auto reconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_disable_autoreconnect_cmd, wifi_sta_autoconnect_disable, wifi station disable auto reconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_rc_fixed_enable, rc_fix_en, wifi rate control fixed rate enable);
SHELL_CMD_EXPORT_ALIAS(wifi_rc_fixed_disable, rc_fix_dis, wifi rate control fixed rate diable);
SHELL_CMD_EXPORT_ALIAS(wifi_power_saving_on_cmd, wifi_sta_ps_on, wifi power saving mode ON);
SHELL_CMD_EXPORT_ALIAS(wifi_power_saving_off_cmd, wifi_sta_ps_off, wifi power saving mode OFF);
SHELL_CMD_EXPORT_ALIAS(wifi_power_saving_set, wifi_sta_ps_set, set wifi ps mode active time);
SHELL_CMD_EXPORT_ALIAS(wifi_power_saving_get, wifi_sta_ps_get, get wifi ps mode);
SHELL_CMD_EXPORT_ALIAS(wifi_denoise_enable_cmd, wifi_sta_denoise_enable, wifi denoise);
SHELL_CMD_EXPORT_ALIAS(wifi_denoise_disable_cmd, wifi_sta_denoise_disable, wifi denoise);
SHELL_CMD_EXPORT_ALIAS(wifi_sniffer_on_cmd, wifi_sniffer_on, wifi sniffer mode on);
SHELL_CMD_EXPORT_ALIAS(wifi_sniffer_off_cmd, wifi_sniffer_off, wifi sniffer mode off);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_start, wifi_ap_start, start Ap mode [channel] [max_sta_supported]);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_stop, wifi_ap_stop, stop Ap mode);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_chan_switch, wifi_ap_chan_switch, switch AP channel);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_conf_max_sta, wifi_ap_conf_max_sta, config Ap max sta);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_dump, wifi_dump, dump fw statistic);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_cfg, wifi_cfg, wifi cfg cmd);
SHELL_CMD_EXPORT_ALIAS(cmd_dump_reset, wifi_pkt, wifi dump needed);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_coex_rf_force_on, wifi_coex_rf_force_on, wifi coex RF forece on);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_coex_rf_force_off, wifi_coex_rf_force_off, wifi coex RF forece off);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_coex_pti_force_on, wifi_coex_pti_force_on, wifi coex PTI forece on);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_coex_pti_force_off, wifi_coex_pti_force_off, wifi coex PTI forece off);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_coex_pta_set, wifi_coex_pta_set, wifi coex PTA set);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_sta_list_get_cmd, wifi_sta_list, get sta list in AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_sta_delete_cmd, wifi_sta_del, delete one sta in AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_edca_dump_cmd, wifi_edca_dump, dump EDCA data);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_state_get, wifi_state, get wifi_state);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_power_table_update, wifi_update_power, Power table test command);

int wifi_mgmr_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}
