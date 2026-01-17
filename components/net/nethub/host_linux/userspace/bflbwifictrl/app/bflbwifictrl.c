/**
 * @file bflbwifictrl.c
 * @brief BFLB WiFi CLI control tool
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <limits.h>

#ifndef R_OK
#define R_OK 4  /* Test for read permission */
#endif

#include "bflbwifid.h"

/* ========== Global Variables ========== */

static char g_socket_path[128] = UNIX_SOCKET_PATH;

/* ========== Helper Functions ========== */

static void print_usage(const char *prog_name)
{
    fprintf(stderr, "BFLB WiFi Control Tool v1.0\n");
    fprintf(stderr, "Usage: %s [options] <command> [arguments]\n", prog_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -s <path>   Unix socket path (default: %s)\n", UNIX_SOCKET_PATH);
    fprintf(stderr, "  -h          Show this help\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Commands:\n");
    fprintf(stderr, "  connect_ap <ssid> [password]   Connect to AP (open network if no password)\n");
    fprintf(stderr, "  disconnect                     Disconnect from AP\n");
    fprintf(stderr, "  scan                           Scan for APs\n");
    fprintf(stderr, "  status                         Show WiFi status\n");
    fprintf(stderr, "  version                        Show version\n");
    fprintf(stderr, "  reboot                         Reboot WiFi module\n");
    fprintf(stderr, "  ota <file.bin>                 OTA firmware upgrade\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s connect_ap myssid\n", prog_name);
    fprintf(stderr, "  %s connect_ap myssid mypassword\n", prog_name);
    fprintf(stderr, "  %s scan\n", prog_name);
    fprintf(stderr, "  %s status\n", prog_name);
}

static int connect_to_daemon(void)
{
    int fd;
    struct sockaddr_un addr;

    /* Check socket path length */
    if (strlen(g_socket_path) >= sizeof(addr.sun_path)) {
        fprintf(stderr, "Error: Socket path too long: %s\n", g_socket_path);
        return -1;
    }

    /* Create Unix Domain Socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "Error: Failed to create socket: %s\n", strerror(errno));
        return -1;
    }

    /* Connect to daemon */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, g_socket_path, strlen(g_socket_path) + 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Error: Failed to connect to daemon: %s\n", strerror(errno));
        fprintf(stderr, "Make sure bflbwifid is running\n");
        close(fd);
        return -1;
    }

    return fd;
}

static int send_command_and_wait(const char *cmd, char *response, size_t response_len)
{
    int fd;
    ssize_t n;
    size_t total = 0;

    /* Connect to daemon */
    fd = connect_to_daemon();
    if (fd < 0) {
        return -1;
    }

    /* Send command */
    n = send(fd, cmd, strlen(cmd), 0);
    if (n < 0) {
        fprintf(stderr, "Error: Failed to send command: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Receive response */
    while (total < response_len - 1) {
        n = recv(fd, response + total, response_len - total - 1, 0);
        if (n <= 0) {
            break;
        }
        total += (size_t)n;

        /* Check if complete response is received (END marker) */
        if (strstr(response, "\nEND\n") != NULL) {
            break;
        }
    }

    response[total] = '\0';
    close(fd);

    return (int)total;
}

/* ========== Command Handler Functions ========== */

static int cmd_connect_ap(int argc, char *argv[])
{
    char cmd[MAX_CMD_LEN];
    char response[MAX_RESPONSE_LEN];

    if (argc < 3) {
        fprintf(stderr, "Error: Missing SSID\n");
        fprintf(stderr, "Usage: %s connect_ap <ssid> [password]\n", argv[0]);
        return -1;
    }

    const char *ssid = argv[2];
    const char *password = (argc >= 4) ? argv[3] : "";

    printf("Connecting to '%s'...\n", ssid);

    /* Construct command */
    snprintf(cmd, sizeof(cmd), "CONNECT_AP %s %s\n", ssid, password);

    /* Send command */
    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    /* Parse and display response */
    printf("%s", response);

    /* Check if successful */
    if (strncmp(response, "OK", 2) == 0) {
        return 0;
    } else {
        return -1;
    }
}

static int cmd_disconnect(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    char cmd[] = "DISCONNECT\n";
    char response[MAX_RESPONSE_LEN];

    printf("Disconnecting...\n");

    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("%s", response);

    return (strncmp(response, "OK", 2) == 0) ? 0 : -1;
}

static int cmd_scan(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    char cmd[] = "SCAN\n";
    char response[MAX_RESPONSE_LEN];

    printf("Scanning for APs...\n");

    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("%s", response);

    return (strncmp(response, "OK", 2) == 0) ? 0 : -1;
}

static int cmd_status(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    char cmd[] = "STATUS\n";
    char response[MAX_RESPONSE_LEN];

    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("%s", response);

    return (strncmp(response, "OK", 2) == 0) ? 0 : -1;
}

static int cmd_version(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    char cmd[] = "VERSION\n";
    char response[MAX_RESPONSE_LEN];

    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("%s", response);

    return (strncmp(response, "OK", 2) == 0) ? 0 : -1;
}

static int cmd_reboot(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    char cmd[] = "REBOOT\n";
    char response[MAX_RESPONSE_LEN];

    printf("Rebooting WiFi module...\n");

    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    printf("%s", response);

    return (strncmp(response, "OK", 2) == 0) ? 0 : -1;
}

static int cmd_ota(int argc, char *argv[])
{
    char cmd[MAX_CMD_LEN];
    char response[MAX_RESPONSE_LEN];
    char filepath_abs[PATH_MAX];
    const char *filepath;

    if (argc < 3) {
        fprintf(stderr, "Error: Missing OTA file path\n");
        fprintf(stderr, "Usage: %s ota <ota.bin>\n", argv[0]);
        return -1;
    }

    filepath = argv[2];

    /* Convert relative path to absolute path */
    if (filepath[0] != '/') {
        if (realpath(filepath, filepath_abs) == NULL) {
            fprintf(stderr, "Error: Cannot resolve path: %s\n", filepath);
            return -1;
        }
        filepath = filepath_abs;
    }

    /* Check file readability */
    if (access(filepath, R_OK) != 0) {
        fprintf(stderr, "Error: OTA file not readable: %s\n", filepath);
        return -1;
    }

    /* Check command length */
    if (strlen(filepath) + 6 > MAX_CMD_LEN) {  // "OTA " + filepath + "\n"
        fprintf(stderr, "Error: File path too long\n");
        return -1;
    }

    printf("Starting OTA upgrade: %s\n", filepath);

    /* Construct command */
    snprintf(cmd, sizeof(cmd), "OTA %s\n", filepath);

    /* Send command */
    if (send_command_and_wait(cmd, response, sizeof(response)) < 0) {
        return -1;
    }

    /* Parse and display response */
    printf("%s", response);

    /* Check if successful */
    if (strncmp(response, "OK", 2) == 0) {
        return 0;
    } else {
        return -1;
    }
}

/* ========== Main Function ========== */

int main(int argc, char *argv[])
{
    int ret;
    int opt;

    /* Parse options */
    while ((opt = getopt(argc, argv, "s:h")) != -1) {
        switch (opt) {
            case 's':
                strncpy(g_socket_path, optarg, sizeof(g_socket_path) - 1);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    /* Check if command is specified */
    if (optind >= argc) {
        fprintf(stderr, "Error: No command specified\n\n");
        print_usage(argv[0]);
        return 1;
    }

    /* Execute command */
    const char *command = argv[optind];

    if (strcmp(command, "connect_ap") == 0) {
        ret = cmd_connect_ap(argc, argv);
    }
    else if (strcmp(command, "disconnect") == 0) {
        ret = cmd_disconnect(argc, argv);
    }
    else if (strcmp(command, "scan") == 0) {
        ret = cmd_scan(argc, argv);
    }
    else if (strcmp(command, "status") == 0) {
        ret = cmd_status(argc, argv);
    }
    else if (strcmp(command, "version") == 0) {
        ret = cmd_version(argc, argv);
    }
    else if (strcmp(command, "reboot") == 0) {
        ret = cmd_reboot(argc, argv);
    }
    else if (strcmp(command, "ota") == 0) {
        ret = cmd_ota(argc, argv);
    }
    else {
        fprintf(stderr, "Error: Unknown command: %s\n\n", command);
        print_usage(argv[0]);
        return 1;
    }

    return (ret == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
