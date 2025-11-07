#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "utils_getopt.h"
#include "utils_hex.h"
#ifdef IOT_SDK_ADAPTER
#include "utils_string.h"
#include "cli.h"
#include "utils_hexdump.h"
#include "bl_os_system.h"
#endif
#include "fhost.h"
#include "fhost_ipc.h"
#include "net_iperf_al_priv.h"
#include "net_ping_al_priv.h"
#include "net_tg_al_priv.h"
#include "arpa/inet.h"
#ifdef CFG_IPV6
#include "lwip/tcpip.h"
#include "lwip/ip_addr.h"
#endif
#include "macsw.h"

#include "wifi_mgmr_cli.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#if defined(CFG_CSIDMA_CLI) && CFG_CSIDMA_CLI
#include "csidma_cmd.h"
#endif
#ifdef CONFIG_ANTDIV_STATIC
#include "antenna_al.h"
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

#define TG_USAGE0    \
 "TG command can be used for the traffic generation and reception\r\n"  \
 " First a stream must be configured with the following parameter:\r\n"   \
 " - id (< @ref FHOST_TG_MAX_TRAFFIC_STREAMS)\r\n"    \
 " - profile (@ref profile_id)\r\n"   \
 " - direction (@ref FHOST_TG_DIRECT_SEND, @ref FHOST_TG_DIRECT_RECV)\r\n"    \
 " - remote ip and port\r\n"  \
 " - local ip and port\r\n"   \
 " - rate (in pkt/s, 0 for RX)\r\n"   \
 " - packet size (in bytes, 0 for RX)\r\n"    \
 " - duration (in seconds, 0 for RX)\r\n"
#define TG_USAGE1    \
 " - tos (Type Of Service) (basically 'tid << 5', 0 for RX).\r\n" \
 " Once a stream has been configured, it can be started/stopped\r\n" \
 "   tg config <id> <profile> <direction> <remote_ip> <remote_port> <local_ip> <local_port>\r\n" \
 "             <rate> <pksize> <duration> <tos>\r\n" \
 "   tg start  <id1> [<id2> ... <id8>]\r\n" \
 "   tg stop   <id1> [<id2> ... <id8>]\r\n"

void cmd_tg(int argc, char **argv) {
    char *str = NULL;
    uint32_t i, len = 0;

    // Calculate the total length of the string
    for (i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1; // Include spaces
    }

    // Allocate memory
    str = rtos_malloc(1 + len * sizeof(char));
    if (!str) {
        fhost_printf("cmd_tg alloc failed\r\n");
        return;
    }

    // Concatenate strings
    str[0] = '\0';
    for (i = 1; i < argc; i++) {
        strcat(str, argv[i]);
        if (i < argc - 1) {
            strcat(str, " ");
        }
    }
    //fhost_printf("tg cmd：%s\n", str);

    char *token, *next = str;

    token = fhost_ipc_next_token(&next);
    if (!strcmp("config", token))
    {
        char *id, *prof, *direction, *remote_ip, *remote_port, *local_ip,
        *local_port, *rate, *pksize, *duration, *tos;
        uint32_t stream_id;
        uint32_t rip, lip;
        char fmt[] = "Configure stream %d for %s\n";

        id = fhost_ipc_next_token(&next);
        prof = fhost_ipc_next_token(&next);
        direction = fhost_ipc_next_token(&next);
        remote_ip = fhost_ipc_next_token(&next);
        remote_port = fhost_ipc_next_token(&next);
        local_ip = fhost_ipc_next_token(&next);
        local_port = fhost_ipc_next_token(&next);
        rate = fhost_ipc_next_token(&next);
        pksize = fhost_ipc_next_token(&next);
        duration = fhost_ipc_next_token(&next);
        tos = fhost_ipc_next_token(&next);

        if (!id || !prof || !direction ||
            !remote_ip || fhost_ipc_parse_ip4(remote_ip, &rip, NULL) || !remote_port ||
            !local_ip || fhost_ipc_parse_ip4(local_ip, &lip, NULL) || !local_port ||
            !pksize || !duration || !tos) {
            //show usage
            fhost_printf("invaild args, usage:\r\n %s", TG_USAGE0);
            fhost_printf("%s", TG_USAGE1);
            goto exit;// FHOST_IPC_SHOW_USAGE;
        }

        stream_id = atoi(id);

        if (fhost_tg_config(stream_id, atoi(prof), atoi(direction), rip, atoi(remote_port),
                            lip, atoi(local_port), atoi(rate), atoi(pksize), atoi(duration),
                            atoi(tos)))
        {
            fhost_printf("Configuration error\n");
            goto exit; // FHOST_IPC_ERROR;
        }

        if (atoi(direction) == FHOST_TG_DIRECT_SEND)
        {
            fhost_printf(fmt, stream_id, "sending\n");
        }
        else
        {
            fhost_printf(fmt, stream_id, "receiving\n");
        }
    }
    else if (!strcmp("start", token))
    {
        char *id[FHOST_TG_MAX_TRAFFIC_STREAMS + 1];
        uint32_t stream_id;
        char fmt[40] = "Start stream id : ";
        int i = 0;
        int j = 0;

        id[i] = fhost_ipc_next_token(&next);
        // If there's the first ID, try to collect all the rest
        while (id[i] != NULL && i < FHOST_TG_MAX_TRAFFIC_STREAMS)
        {
            id[++i] = fhost_ipc_next_token(&next);
        }

        if (!id[0])
        {
            fhost_printf("tg start failed %d\r\n", __LINE__);
            goto exit;
        }

        // In this loop, we count all the IDs of streams to start
        for (j = 0; j <= i - 1; j++)
        {
            stream_id = atoi(id[j]);
            if (fhost_tg_start(stream_id))
            {
                fhost_printf("Can't start stream : %d\n", stream_id);
                goto exit;
            }
            strcat(strcat(fmt, id[j]), " ");
            // If not, ASSERT (!hal_machw_time_past(value)) at mm_timer.c:116
            rtos_task_suspend(500);
        }
        strcat(fmt, "\n");
        fhost_printf(fmt);
    }
    else if (!strcmp("stop", token))
    {
        uint32_t stream_id;
        struct fhost_tg_stats *stats = NULL;

        token = fhost_ipc_next_token(&next);
        if (!token)
        {
            fhost_printf("tg stop failed %d \r\n", __LINE__);
            goto exit;
        }

        do
        {
            stream_id = atoi(token);
            stats = fhost_tg_stop(stream_id);

            if (stats == NULL)
            {
                fhost_printf("Can't stop stream : %d\n", stream_id);
                goto exit;
            }

            rtos_task_suspend(1);
            fhost_printf("STOP stream ID : %d\n"
                         "TX frames : %d\n"
                         "RX frames : %d\n"
                         "TX bytes : %d\n"
                         "RX bytes : %d\n",
                         stream_id,
                         stats->tx_frames,
                         stats->rx_frames,
                         stats->tx_bytes,
                         stats->rx_bytes);
            rtos_task_suspend(10);
            fhost_printf("Throughput : %d bps\n"
                         "Expected : %d\n"
                         "Lost : %d\n"
                         "Unordered : %d\n"
                         "RTT : %d us\n",
                         stats->throughput,
                         stats->expected,
                         stats->lost,
                         stats->unordered,
                         stats->rt_time);
        } while((token = fhost_ipc_next_token(&next)));
    }
    else
    {
        //show usage
        fhost_printf("invaild args, usage:\r\n %s", TG_USAGE0);
        fhost_printf("%s", TG_USAGE1);
        goto exit;
    }

exit:
    rtos_free(str);
    return;// FHOST_IPC_SUCCESS;
}

#define FHOST_PING_USAGE \
    "ping <dst_ip> [-s pksize (bytes)] [-r rate (pkt/sec)] [-d duration (sec)] [-Q tos]\r\n"    \
    "ping stop <id1> [<id2> ... <id8>]\r\n"

/**
 ****************************************************************************************
 * @brief Process function for 'ping' command
 *
 * Ping command can be used to test the reachability of a host on an IP network.
 *
   @verbatim
   ping <dst_ip> [-s pksize (bytes)] [-r rate (pkt/sec)] [-d duration (sec)] [-Q tos]
   ping stop <id1> [<id2> ... <id8>]
   @endverbatim
 *
 * Note that -s, -r, -d, -t are options for ping command. We could choose any of them to
 * configure. If not configured, it will set the default values at layer net_tg_al.
 *
 * @param[in] params ping command above
 *
 * @return 0 on success and !=0 if error occurred
 ****************************************************************************************
 */
#define IPC_MSG_ID_NULL 0
void cmd_fhost_ping(int argc, char **argv)
{
    char *str = NULL;
    uint32_t i, len = 0;

    for (i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1;
    }

    str = rtos_malloc(1 + len * sizeof(char));
    if (!str) {
        fhost_printf("cmd_fhost_ping alloc failed\r\n");
        return;
    }

    str[0] = '\0';
    for (i = 1; i < argc; i++) {
        strcat(str, argv[i]);
        if (i < argc - 1) {
            strcat(str, " ");
        }
    }
    fhost_printf("ping cmd：%s\n", str);

    char *token, *next = str;
    uint32_t rip = 0, ret;
    token = fhost_ipc_next_token(&next);
    uint32_t rate = 0, pksize = 0, duration = 0, tos = 0;
    //struct fhost_task_msg_id *task_hdl_msg_ptr;
    bool background = false;
    struct fhost_ping_task_args args;

    if (!strcmp("stop", token))
    {
        uint32_t stream_id;
        struct fhost_ping_stream* ping_stream;

        token = fhost_ipc_next_token(&next);
        if (!token)
            goto usage;

        stream_id = atoi(token);
        ping_stream = fhost_ping_find_stream_profile(stream_id);

        if (ping_stream)
        {
#if 0
            if (ping_stream->background)
            {
                task_hdl_msg_ptr = fhost_search_task_hdl_msg(ping_stream->ping_handle);
                task_hdl_msg_ptr->msg_id = IPC_MSG_ID_NULL;
                ret = FHOST_IPC_NO_RESP;
            }
#endif
            fhost_ping_stop(ping_stream);
        }
        else
        {
            fhost_printf("Invalid stream_id %d", stream_id);
        }
        goto exit;
    }

    do
    {
        // Handle all options of ping command
        if (token[0] == '-')
        {
            switch(token[1])
            {
                case ('s'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    pksize = atoi(token);
                    break;
                case ('r'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    rate = atoi(token);
                    break;
                case ('d'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    duration = atoi(token);
                    break;
                case ('Q'):
                    token = fhost_ipc_next_token(&next);
                    if (!token)
                        goto usage;
                    tos = atoi(token);
                    break;
                case ('G'):
                    //background = true;
                    fhost_printf("Not supported\r\n");
                    break;
                default:
                    goto usage;
            }
        }
        // If it's neither options, nor IP address, then the input is wrong
        else if (fhost_ipc_parse_ip4(token, &rip, NULL))
        {
            fhost_printf("Invalid IP address: %s\n", token);
            goto exit;
        }
    } while ((token = fhost_ipc_next_token(&next)));

    // IP destination should be set by the command
    if (rip == 0)
        goto usage;

    args.rip = rip;
    args.rate = rate;
    args.pksize = pksize;
    args.duration = duration;
    args.tos = tos;
    args.background = background;
    ret = fhost_ipc_start_task(IPC_MSG_ID_NULL, fhost_ping_start, &args, fhost_ping_sigkill_handler);

    if (ret == FHOST_IPC_ERROR)
    {
        fhost_printf("Send ping error\n");
    }
    goto exit;

usage:
    fhost_printf("%s", FHOST_PING_USAGE);

exit:
    rtos_free(str);
    return;
}

#define SET_IPV4_USAGE                    \
    "set_ipv4 [ip] [dns] [gw] [mask]\r\n"   \
    "\t ip: set static ip\r\n" \
    "\t dns: set local host dns\r\n" \
    "\t gw: set local host gate way. default is 255.255.255.0\r\n" \
    "\t mask: set local host mask. default is 255.255.255.0\r\n"

void wifi_sta_static_ipv4(int argc, char **argv)
{
    char *addr, *mask, *gw, *dns;
    if (argc < 3) {
        printf("%s", SET_IPV4_USAGE);
        return;
    }

    /* ip addr */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "127.0.0.1";
    }

    /* ip dns */
    if (argc > 2) {
        dns = argv[2];
    } else {
        dns = "0.0.0.0";
    }

    /* ip gw */
    if (argc > 3) {
        gw = argv[3];
    } else {
        gw = "255.255.255.0";
    }
    /* ip mask */
    if (argc > 4) {
        mask = argv[4];
    } else {
        mask = "255.255.255.0";
    }
    printf("addr:%s,mask:%s,gw:%s,dns:%s\r\n", addr, mask, gw, dns);

    wifi_mgmr_sta_ip_set(inet_addr(addr), inet_addr(mask), inet_addr(gw), inet_addr(dns));

    return;

}

static void _print_sta_pwr(uint8_t numb, int8_t *power_table)
{
    uint8_t i;
    for (i = 0; i<numb; i++) {
        printf("%3u ", power_table[i]);
    }
    printf("\r\n");
}

void wifi_sta_info_cmd(int argc, char **argv)
{
    ip4_addr_t addr, mask, gw, dns;
    int rssi = 0;
    rf_pwr_table_t power_table;
    char country_code[3] = {0};
    int channel = 0;

    memset(&addr, 0, sizeof(ip4_addr_t));
    memset(&mask, 0, sizeof(ip4_addr_t));
    memset(&gw, 0, sizeof(ip4_addr_t));
    memset(&dns, 0, sizeof(ip4_addr_t));
    memset(power_table.pwr_11b, 0, sizeof(rf_pwr_table_t));

    wifi_mgmr_get_country_code(country_code);
    wifi_sta_ip4_addr_get(&addr.addr, &mask.addr, &gw.addr, &dns.addr);
    wifi_mgmr_sta_rssi_get(&rssi);
#ifndef BL616D
    wifi_mgmr_tpc_pwr_get(&power_table);
#endif
    fhost_printf("================================================================\r\n");
    #ifdef CONFIG_ANTDIV_STATIC
    fhost_printf("ANT :    %d\r\n", antenna_hal_get_current_antenna());
    #endif
    fhost_printf("RSSI:    %ddbm\r\n", rssi);
    fhost_printf("IP  :    %s \r\n", ip4addr_ntoa(&addr));
    fhost_printf("MASK:    %s \r\n", ip4addr_ntoa(&mask));
    fhost_printf("GW  :    %s \r\n", ip4addr_ntoa(&gw));
    fhost_printf("DNS :    %s \r\n", ip4addr_ntoa(&dns));
    fhost_printf("Country: %s \r\n", country_code);
    if (wifi_mgmr_sta_channel_get(&channel))
        fhost_printf("Channel: not connected\r\n");
    else
        fhost_printf("Channel: %d \r\n", channel);
    #ifdef CFG_IPV6
    struct netif *nif;

    NETIF_FOREACH(nif) {
        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++)
        {
            const ip6_addr_t * ip6addr = netif_ip6_addr(nif, i);
            if (!ip6_addr_isany(ip6addr))
            {
                fhost_printf("IPv6:    %s, state %x\r\n", ip6addr_ntoa(ip6addr), netif_ip6_addr_state(nif, i));
            }
        }
    }
    #endif
    fhost_printf("Power Table (dbm):\r\n");
    fhost_printf("-----------------------------------------------------------\r\n");
    fhost_printf("  11b:         ");
    _print_sta_pwr(4,  power_table.pwr_11b);
    fhost_printf("  11g:         ");
    _print_sta_pwr(8,  power_table.pwr_11g);
    fhost_printf("  11n(ht20):   ");
    _print_sta_pwr(8,  power_table.pwr_11n_ht20);
    fhost_printf("  11n(ht40):   ");
    _print_sta_pwr(8,  power_table.pwr_11n_ht40);
    fhost_printf("  11ac(vht20): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht20);
    fhost_printf("  11ac(vht40): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht40);
#if 0
    fhost_printf("  11ac(vht80): ");
    _print_sta_pwr(10, power_table.pwr_11ac_vht80);
#endif
    fhost_printf("  11ax(he20):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he20);
    fhost_printf("  11ax(he40):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he40);
#if 0
    fhost_printf("  11ax(he80):  ");
    _print_sta_pwr(12, power_table.pwr_11ax_he80);
    fhost_printf("  11ax(he160): ");
    _print_sta_pwr(12, power_table.pwr_11ax_he160);
#endif
    fhost_printf("  last rx rate: format:%s,mcs:%u \r\n", export_stats_get_rx_format(), export_stats_get_rx_mcs());
    fhost_printf("  last tx rate: format:%s,mcs:%u \r\n", export_stats_get_tx_format(), export_stats_get_tx_mcs());
    fhost_printf("================================================================\r\n");
}

void wifi_mgmr_ap_start_cmd(int argc, char **argv)
{
    getopt_env_t getopt_env;
    int opt;
    wifi_mgmr_ap_params_t config;

    if (argc < 2) {
        goto _ERROUT;
    }

    memset(&config, 0, sizeof(config));

    // default use_ipcfg use_dhcpd
    config.use_ipcfg = true;
    config.use_dhcpd = true;

    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "b:s:k:c:a:t:h:i:g:d:I:S:L:n:w:")) != -1) {
        switch (opt) {
	case 'b':
	    config.type = (uint8_t)atoi(getopt_env.optarg);
	    break;

	case 's':
	    config.ssid = getopt_env.optarg;
	    break;

	case 'k':
	    config.key = getopt_env.optarg;
	    break;

	case 'c':
	    config.channel = (uint8_t)atoi(getopt_env.optarg);
	    break;

	case 'a':
	    config.akm = getopt_env.optarg;
	    break;

    case 't':
	    config.ap_max_inactivity = (uint32_t)atoi(getopt_env.optarg);
	    break;

    case 'h':
	    config.hidden_ssid = (uint32_t)atoi(getopt_env.optarg);
	    break;

    case 'i':
	    config.isolation = (uint32_t)atoi(getopt_env.optarg);
	    break;
	case 'g':
	    config.use_ipcfg = atoi(getopt_env.optarg);
	    break;
	case 'd':
	    config.use_dhcpd = atoi(getopt_env.optarg);
	    break;
	case 'I':
	    config.ap_ipaddr = inet_addr(getopt_env.optarg);
	    config.ap_mask = inet_addr("255.255.255.0");
            if((config.ap_ipaddr & 0xff000000)  != 0x01000000)
                printf("ap mode ipaddr is not x.x.x.1 \r\n");
	    break;

    case 'S':
	    config.start = atoi(getopt_env.optarg);
	    break;

	case 'L':
	    config.limit = atoi(getopt_env.optarg);
	    break;

    case 'n':
        config.bcn_interval = atoi(getopt_env.optarg);
        break;

    case 'w':
        config.disable_wmm = (uint32_t)atoi(getopt_env.optarg);
        break;

	case '?':
	    printf("unknow option: %c \r\n", getopt_env.optopt);
	    goto _ERROUT;

        }
    }

    if (config.ssid == NULL) {
        goto _ERROUT;
    }

    wifi_mgmr_ap_start(&config);

    return;

 _ERROUT:
    printf("[USAGE]: %s -s <ssid> [-k <key>] [-c <channel>] [-a <akm>] [-I <ipv4_addr>] [-S <dhcpd_start>] [-L <dhcpd_limit>] [-n <bcn_interval>]\r\n", argv[0]);
    return;
}

void wifi_mgmr_ap_stop_cmd(int argc, char **argv)
{
    wifi_mgmr_ap_stop();
}

#if WIFI_STATISTIC_ENABLE
void cmd_fw_dbg(int argc, char **argv)
{
    struct cfgmacsw_me_param cmd;
    struct cfgmacsw_me_param_resp resp;
    struct fhost_cntrl_link *fw_dbg;

    uint32_t param_tx_ampdu_retry_cnt_limit;
    uint32_t param_tx_ampdu_protect_enable;
    uint32_t param_tx_ampdu_drop_to_singleton_retrycnt_threshold;

    uint16_t id;
    uint32_t value;

    if ((1 != argc) && (2 != argc) && (4 != argc)) {
        fhost_printf("fw_dbg get|set [id:[%d:%d]] [value]\r\n",
                CFGMACSW_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
                CFGMACSW_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD);
        goto cmd_fw_dbg_err;
    }

#if 0
    fw_dbg = fhost_cntrl_cfgmacsw_link_open();
    if (NULL == fw_dbg) {
        fhost_printf("fw_dbg NULL\r\n");
        return;
    }
#else
    fw_dbg = cntrl_link;
#endif

    memset(&cmd, 0, sizeof(cmd));
    memset(&resp, 0, sizeof(resp));

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGMACSW_ME_PARAM_CMD;
    cmd.hdr.resp_queue = fw_dbg->queue;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGMACSW_ME_PARAM_RESP;

    if ((4 == argc) && (strncmp(argv[1], "set", 3) == 0)) {
        id = atoi(argv[2]);
        if (id > CFGMACSW_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD) {
            goto cmd_fw_dbg_err;
        }
        value = atoi(argv[3]);
        cmd.id  = id;
        cmd.cmd = CFGMACSW_ME_PARAM_CMD_SET;
        memcpy(cmd.data, &value, sizeof(uint32_t));

        fhost_printf("set id:%d, cmd:%d, value:%d\r\n", id, cmd, value);
        if (fhost_cntrl_cfgmacsw_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgmacsw_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
    } else if ((2 == argc) && (strncmp(argv[1], "get", 3) == 0)) {
        // get
        cmd.id  = CFGMACSW_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT;
        cmd.cmd = CFGMACSW_ME_PARAM_CMD_GET;
        if (fhost_cntrl_cfgmacsw_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgmacsw_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
        memcpy(&param_tx_ampdu_retry_cnt_limit, resp.data, sizeof(uint32_t));
        fhost_printf("param_tx_ampdu_retry_cnt_limit                      = %d\r\n", param_tx_ampdu_retry_cnt_limit);

        // get
        cmd.id  = CFGMACSW_ME_PARAM_ID_TX_AMPDU_PROTECT_ENABLE;
        cmd.cmd = CFGMACSW_ME_PARAM_CMD_GET;
        if (fhost_cntrl_cfgmacsw_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgmacsw_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
        memcpy(&param_tx_ampdu_protect_enable, resp.data, sizeof(uint32_t));
        fhost_printf("param_tx_ampdu_protect_enable                       = %d\r\n", param_tx_ampdu_protect_enable);

        // get
        cmd.id  = CFGMACSW_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD;
        cmd.cmd = CFGMACSW_ME_PARAM_CMD_GET;
        if (fhost_cntrl_cfgmacsw_cmd_send(&cmd.hdr, &resp.hdr)) {
            fhost_printf("fhost_cntrl_cfgmacsw_cmd_send error\r\n");
            goto cmd_fw_dbg_err;
        }
        memcpy(&param_tx_ampdu_drop_to_singleton_retrycnt_threshold , resp.data, sizeof(uint32_t));
        fhost_printf("param_tx_ampdu_drop_to_singleton_retrycnt_threshold = %d\r\n", param_tx_ampdu_drop_to_singleton_retrycnt_threshold);

        // fhost_statistic
        fhost_printf("tx_ampdu_retry_limit_count = %d\r\n", fhost_statistic.tx_ampdu_retry_limit_count);
        fhost_printf("tx_ampdu_lft_expired_count = %d\r\n", fhost_statistic.tx_ampdu_lft_expired_count);
        fhost_printf("tx_ampdu_sw_retry          = %d\r\n", fhost_statistic.tx_ampdu_sw_retry);
        fhost_printf("tx_ampdu_unvalid_ba        = %d\r\n", fhost_statistic.tx_ampdu_unvalid_ba);
    } else {
        goto cmd_fw_dbg_err;
    }

#if 0
    fhost_cntrl_cfgmacsw_link_close(fw_dbg);
#endif

    return;
cmd_fw_dbg_err:
    fhost_printf("fw_dbg get|set [id:[%d:%d]] [value]\r\n",
            CFGMACSW_ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
            CFGMACSW_ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD);
}
#endif

#if defined(CFG_REC) && WIFI_STATISTIC_ENABLE
int cmd_fw_dbg_rec(int argc, char **argv)
{
    extern char _ld_bl_static_fw_dbg_entry_start, _ld_bl_static_fw_dbg_entry_end;
    extern char _ld_bl_static_fw_dbg_count_start, _ld_bl_static_fw_dbg_count_end;

    fw_dbg_t *slot;
    uint32_t *p_count;
    int sum1, sum2, i;

    sum1 = ((fw_dbg_t *)&_ld_bl_static_fw_dbg_entry_end -
            (fw_dbg_t *)&_ld_bl_static_fw_dbg_entry_start);
    sum2 = ((uint32_t *)&_ld_bl_static_fw_dbg_count_end -
            (uint32_t *)&_ld_bl_static_fw_dbg_count_start);
    slot = (fw_dbg_t *)&_ld_bl_static_fw_dbg_entry_start;
    p_count = (uint32_t *)&_ld_bl_static_fw_dbg_count_start;

    if (sum1 != sum2) {
        fhost_printf("never here, assert %d,%d\r\n", sum1, sum2);
        return -1;
    }

    for (i = 0; i < sum1; i++) {
        fhost_printf("[idex:%4d] [count:%6d] %s:%d\r\n", i, *p_count, slot->name, slot->line);
        slot += 1;
        p_count += 1;
    }

    return 0;
}
#endif

#ifdef CONFIG_MAT
static int cmd_mat(int argc, char *argv[])
{
    void mat_print_mib(void);
    mat_print_mib();
}
#endif

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

SHELL_CMD_EXPORT_ALIAS(cmd_hello, hello, test cli hello);
SHELL_CMD_EXPORT_ALIAS(cmd_phy, phy, test cli hello);
#ifdef CMD_TXL_CNTRL_PUSH_AC_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_ac_set, ac_set, access_category config);
#endif
#ifdef CFG_FOR_COEXISTENCE_TEST_STOPAP_PATCH
SHELL_CMD_EXPORT_ALIAS(cmd_ap_stop, ap_stop, ap stop);
#endif
SHELL_CMD_EXPORT_ALIAS(wifi_ap_bcn_mode_set_cmd, bcn_mode_set, wifi ap bcn mode set);
SHELL_CMD_EXPORT_ALIAS(wifi_mgmr_ap_acl_set_cmd, ap_acl_set, wifi ap acl set);
SHELL_CMD_EXPORT_ALIAS(wifi_mgmr_ap_acl_enable_cmd, ap_acl_perm_set, wifi ap acl perm set);
SHELL_CMD_EXPORT_ALIAS(wifi_scan_cmd, wifi_scan, wifi scan);
SHELL_CMD_EXPORT_ALIAS(wifi_connect_cmd, wifi_sta_connect, wifi station connect);
SHELL_CMD_EXPORT_ALIAS(wifi_disconnect_cmd, wifi_sta_disconnect, wifi station disconnect);
SHELL_CMD_EXPORT_ALIAS(lwip_cmd, lwip, show stats);
#ifdef CFG_FHOST_MONITOR
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sniffer_on, wifi_sniffer_on, wifi sniffer on);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sniffer_off, wifi_sniffer_off, wifi sniffer off);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_state_get, wifi_state, get wifi state);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_rssi_get, wifi_sta_rssi, get wifi sya rssi);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_channel_get, wifi_sta_channel, get wifi channel);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_ssid_passphr_get, wifi_sta_ssid_passphr_get, get wifi ssid password);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_sta_mac_get, wifi_sta_mac_get, get wifi sta mac);
SHELL_CMD_EXPORT_ALIAS(wifi_enable_autoreconnect_cmd, wifi_sta_autoconnect_enable, wifi station enable auto reconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_disable_autoreconnect_cmd, wifi_sta_autoconnect_disable, wifi station disable auto reconnect);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ps_on_cmd, wifi_sta_ps_on, wifi sta powersave mode on);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ps_off_cmd, wifi_sta_ps_off, wifi sta powersave mode off);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_ps_set_cmd, wifi_sta_ps_set, wifi sta powersave mode set parameters);
SHELL_CMD_EXPORT_ALIAS(wifi_sta_info_cmd, wifi_sta_info, wifi sta info);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_sta_list_get_cmd, wifi_sta_list, get sta list in AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_ap_sta_delete_cmd, wifi_sta_del, delete one sta in AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_mgmr_ap_start_cmd, wifi_ap_start, start AP mode);
SHELL_CMD_EXPORT_ALIAS(wifi_mgmr_ap_stop_cmd, wifi_ap_stop, stop AP mode);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_mac_get, wifi_ap_mac_get, get wifi ap mac);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ap_conf_max_sta, wifi_ap_conf_max_sta, config AP mac sta);
#ifdef CFG_TG
SHELL_CMD_EXPORT_ALIAS(cmd_tg, tg, wifi tg);
#endif
#ifdef CFG_TG
SHELL_CMD_EXPORT_ALIAS(cmd_fhost_ping, fhost_ping, fhost ping);
#endif
#ifdef CONFIG_CLI_WIFI_DUBUG
SHELL_CMD_EXPORT_ALIAS(cmd_wifi, wifi, wifi);
#endif
SHELL_CMD_EXPORT_ALIAS(wifi_sta_static_ipv4, set_ipv4, ipc task set);
#ifdef CFG_IPERF
SHELL_CMD_EXPORT_ALIAS(cmd_iperf, iperf, iperf test throughput);
#endif
#ifdef CFG_RAW_SEND_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_raw_send, wifi_raw_send, wifi raw send test);
#endif
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_wps_pbc, wps_pbc, Start wps pbc for sta);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_stats, wifi_stats, Get/Clear stats for wifi);
SHELL_CMD_EXPORT_ALIAS(cmd_rc, rc, Print the Rate Control Table);
SHELL_CMD_EXPORT_ALIAS(cmd_rate, rate, set g_fw_rate);
SHELL_CMD_EXPORT_ALIAS(cmd_non_pref_chan, non_pref_chan, set non_pref_chan);
SHELL_CMD_EXPORT_ALIAS(cmd_non_pref_chan_notify, non_pref_chan_notify, notify non_pref_chan change);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_mgmr_sta_twt_setup, wifi_mgmr_sta_twt_setup, Setup WiFi Manager STA TWT functionality);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_mgmr_sta_twt_teardown, wifi_mgmr_sta_twt_teardown, Teardown WiFi Manager STA TWT functionality);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_mode_set, wifi_mode_set, set ap/sta mode);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_get_tx_remaining, get_tx_remaining, get tx remaining);
#if WIFI_STATISTIC_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_fw_dbg, fw_dbg, fw debug param);
#endif
#if defined(CFG_REC) && WIFI_STATISTIC_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_fw_dbg_rec, fw_dbg_rec, fw debug param);
#endif
#if defined(CFG_CSIDMA_CLI) && CFG_CSIDMA_CLI
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_start, csidma_start, start csidma);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_stop, csidma_stop, stop csidma);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_ready, csidma_ready, ready csidma);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_debug, csidma_debug, debug csidma);
#ifdef CSIDMA_FTM_ENABLE
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_force_ftm_mac_set, csidma_force_ftm_mac_set, csidma force ftm mac set);
SHELL_CMD_EXPORT_ALIAS(cmd_csidma_force_ftm, csidma_force_ftm, csidma force ftm);
#endif
#endif
#ifdef CONFIG_MAT
SHELL_CMD_EXPORT_ALIAS(cmd_mat, mat, show MAT information);
#endif
#ifdef CONFIG_ANTDIV_STATIC
SHELL_CMD_EXPORT_ALIAS(wifi_antenna_scan_cmd, wifi_antenna_scan, wifi antenna scan);
SHELL_CMD_EXPORT_ALIAS(wifi_antenna_scan_connect_cmd, wifi_antenna_connect, wifi antenna scan connect);
#endif

int bl_wifi6_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}
