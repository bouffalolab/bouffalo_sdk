#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "nethub_cmd.h"
#include "nethub_wifi.h"

static const char *g_prog_name = "bflbwifictrl";
static const char *g_socket_path = NETHUB_WIFI_DEFAULT_SOCKET_PATH;

static void print_usage(FILE *stream)
{
    size_t i;

    fprintf(stream, "BFLB WiFi Control Tool v2.0\n");
    fprintf(stream, "Usage: %s [options] <command> [arguments]\n", g_prog_name);
    fprintf(stream, "Options:\n");
    fprintf(stream, "  -s <path>   Unix socket path (default: %s)\n", NETHUB_WIFI_DEFAULT_SOCKET_PATH);
    fprintf(stream, "  -h          Show this help\n");
    fprintf(stream, "Commands:\n");
    for (i = 0; i < nethub_cmd_count(); i++) {
        const nethub_cmd_desc_t *cmd = nethub_cmd_at(i);
        if (!cmd) {
            continue;
        }
        if (cmd->usage_args[0] != '\0') {
            fprintf(stream, "  %-12s %-18s %s\n", cmd->cli_name, cmd->usage_args, cmd->summary);
        } else {
            fprintf(stream, "  %-31s %s\n", cmd->cli_name, cmd->summary);
        }
    }
}

static int copy_arg(char *dst, size_t dst_size, const char *src, const char *name)
{
    size_t len = src ? strlen(src) : 0u;

    if (dst_size == 0) {
        return -1;
    }
    if (len >= dst_size) {
        fprintf(stderr, "Error: %s too long\n", name ? name : "argument");
        return -1;
    }
    if (src && len > 0) {
        memcpy(dst, src, len);
    }
    dst[len] = '\0';
    return 0;
}

static int print_result(int ret)
{
    if (ret == 0) {
        return 0;
    }

    fprintf(stderr, "ERROR %d: %s\n", ret, nethub_wifi_strerror(ret));
    return 1;
}

static int cmd_status(nethub_wifi_client_t *client)
{
    nethub_wifi_status_t status;
    int ret = nethub_wifi_get_status(client, &status);

    if (ret != 0) {
        return print_result(ret);
    }

    printf("OK STATUS\n");
    printf("Link: %s\n", nethub_wifi_link_state_name(status.link_state));
    printf("State: %s\n", nethub_wifi_sta_state_name(status.sta_state));
    if (status.ssid[0] != '\0') {
        printf("SSID: %s\n", status.ssid);
        printf("BSSID: %s\n", status.bssid);
        printf("Channel: %u\n", status.channel);
        printf("RSSI: %d dBm\n", status.rssi);
    }
    if (status.ip[0] != '\0') {
        printf("IP: %s\n", status.ip);
        printf("Gateway: %s\n", status.gateway);
        printf("Netmask: %s\n", status.netmask);
        printf("DNS: %s\n", status.dns);
    }
    printf("END\n");
    return 0;
}

static int cmd_version(nethub_wifi_client_t *client)
{
    char version[NETHUB_WIFI_VERSION_MAX_LEN];
    int ret = nethub_wifi_get_version(client, version, sizeof(version));

    if (ret != 0) {
        return print_result(ret);
    }
    printf("OK VERSION\n%s\nEND\n", version);
    return 0;
}

static int cmd_scan(nethub_wifi_client_t *client)
{
    nethub_wifi_ap_info_t aps[NETHUB_WIFI_SCAN_MAX_APS];
    int actual = 0;
    int ret = nethub_wifi_scan(client, aps, NETHUB_WIFI_SCAN_MAX_APS, &actual, 30000);

    if (ret != 0 && ret != NETHUB_WIFI_ERR_BUFFER_TOO_SMALL) {
        return print_result(ret);
    }

    printf("OK SCAN_COMPLETE\n");
    for (int i = 0; i < actual && i < NETHUB_WIFI_SCAN_MAX_APS; i++) {
        printf("AP[%d]: %s (CH:%u, RSSI:%d, MAC:%s)\n",
               i + 1,
               aps[i].ssid,
               aps[i].channel,
               aps[i].rssi,
               aps[i].bssid);
    }
    printf("TOTAL: %d\nEND\n", actual);
    return ret == 0 ? 0 : 1;
}

static int cmd_connect(nethub_wifi_client_t *client, int argc, char *argv[])
{
    nethub_wifi_sta_config_t cfg;
    int ret;

    memset(&cfg, 0, sizeof(cfg));
    if (copy_arg(cfg.ssid, sizeof(cfg.ssid), argv[1], "SSID") != 0) {
        return 1;
    }
    if (argc > 2) {
        if (copy_arg(cfg.password, sizeof(cfg.password), argv[2], "password") != 0) {
            return 1;
        }
    }

    printf("Connecting to '%s'...\n", cfg.ssid);
    ret = nethub_wifi_sta_connect(client, &cfg, 30000);
    if (ret != 0) {
        return print_result(ret);
    }
    printf("OK CONNECTED\nEND\n");
    return 0;
}

static int cmd_start_ap(nethub_wifi_client_t *client, int argc, char *argv[])
{
    nethub_wifi_ap_config_t cfg;
    int ret;

    memset(&cfg, 0, sizeof(cfg));
    if (copy_arg(cfg.ssid, sizeof(cfg.ssid), argv[1], "SSID") != 0) {
        return 1;
    }
    if (argc > 2) {
        if (copy_arg(cfg.password, sizeof(cfg.password), argv[2], "password") != 0) {
            return 1;
        }
    }
    cfg.channel = 11;
    cfg.enc = cfg.password[0] ? NETHUB_WIFI_ENC_WPA2 : NETHUB_WIFI_ENC_OPEN;
    cfg.max_conn = 4;

    printf("Starting SoftAP '%s'...\n", cfg.ssid);
    ret = nethub_wifi_ap_start(client, &cfg, 30000);
    if (ret != 0) {
        return print_result(ret);
    }
    printf("OK AP_STARTED\nSSID: %s\nChannel: %u\nEND\n", cfg.ssid, cfg.channel);
    return 0;
}

static int cmd_ota(nethub_wifi_client_t *client, int argc, char *argv[])
{
    char resolved_path[PATH_MAX];
    const char *path = argv[1];
    int ret;

    (void)argc;
    if (!realpath(path, resolved_path)) {
        fprintf(stderr, "Error: firmware image not found: %s\n", path);
        return 1;
    }

    printf("Updating firmware: %s\n", resolved_path);
    ret = nethub_wifi_ota_update(client, resolved_path, 300000);
    if (ret != 0) {
        return print_result(ret);
    }
    printf("OK OTA_DONE\nDevice is rebooting; wait for link recovery before the next command.\nEND\n");
    return 0;
}

int main(int argc, char *argv[])
{
    nethub_wifi_client_t *client = NULL;
    const nethub_cmd_desc_t *cmd;
    int opt;
    int cmd_argc;
    char **cmd_argv;
    int ret;

    g_prog_name = argv[0];
    while ((opt = getopt(argc, argv, "s:h")) != -1) {
        switch (opt) {
            case 's':
                g_socket_path = optarg;
                break;
            case 'h':
                print_usage(stdout);
                return 0;
            default:
                print_usage(stderr);
                return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: No command specified\n");
        print_usage(stderr);
        return 1;
    }

    cmd_argc = argc - optind;
    cmd_argv = &argv[optind];
    cmd = nethub_cmd_find_cli(cmd_argv[0]);
    if (!cmd) {
        fprintf(stderr, "Error: Unknown command: %s\n", cmd_argv[0]);
        print_usage(stderr);
        return 1;
    }
    if (nethub_cmd_validate_argc(cmd, cmd_argc - 1) != 0) {
        fprintf(stderr, "Error: Invalid arguments for command '%s'\n", cmd->cli_name);
        if (cmd->usage_args[0] != '\0') {
            fprintf(stderr, "Usage: %s %s %s\n", g_prog_name, cmd->cli_name, cmd->usage_args);
        }
        return 1;
    }

    ret = nethub_wifi_client_open(&client, g_socket_path);
    if (ret != 0) {
        return print_result(ret);
    }

    switch (cmd->id) {
        case NETHUB_CMD_CONNECT_AP:
            ret = cmd_connect(client, cmd_argc, cmd_argv);
            break;
        case NETHUB_CMD_DISCONNECT:
            printf("Disconnecting...\n");
            ret = print_result(nethub_wifi_sta_disconnect(client));
            if (ret == 0) {
                printf("OK DISCONNECTED\nEND\n");
            }
            break;
        case NETHUB_CMD_SCAN:
            printf("Scanning for APs...\n");
            ret = cmd_scan(client);
            break;
        case NETHUB_CMD_STATUS:
            ret = cmd_status(client);
            break;
        case NETHUB_CMD_VERSION:
            ret = cmd_version(client);
            break;
        case NETHUB_CMD_REBOOT:
            printf("Rebooting WiFi module...\n");
            ret = print_result(nethub_wifi_reboot(client));
            if (ret == 0) {
                printf("OK REBOOTING\nEND\n");
            }
            break;
        case NETHUB_CMD_START_AP:
            ret = cmd_start_ap(client, cmd_argc, cmd_argv);
            break;
        case NETHUB_CMD_STOP_AP:
            printf("Stopping SoftAP...\n");
            ret = print_result(nethub_wifi_ap_stop(client));
            if (ret == 0) {
                printf("OK AP_STOPPED\nEND\n");
            }
            break;
        case NETHUB_CMD_OTA:
            ret = cmd_ota(client, cmd_argc, cmd_argv);
            break;
        default:
            ret = 1;
            break;
    }

    nethub_wifi_client_close(client);
    return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
