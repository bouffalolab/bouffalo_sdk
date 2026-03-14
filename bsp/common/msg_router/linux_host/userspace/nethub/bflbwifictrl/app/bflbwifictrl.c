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
#define R_OK 4 /* Test for read permission */
#endif

#include "bflbwifi_ipc.h"
#include "bflbwifi_command.h"
#include "bflbwifid_paths.h"

/* ========== Global Variables ========== */

static char g_socket_path[sizeof(((struct sockaddr_un *)0)->sun_path)] = BFLBWIFID_DEFAULT_SOCKET_PATH;
static const char *g_prog_name = "bflbwifictrl";

/* ========== Helper Functions ========== */

#define DAEMON_CONNECT_RETRY_COUNT      20
#define DAEMON_CONNECT_RETRY_DELAY_US   100000
#define STATUS_REPLY_RETRY_COUNT        20
#define STATUS_REPLY_RETRY_DELAY_US     100000

static void print_usage(const char *prog_name)
{
    fprintf(stderr, "BFLB WiFi Control Tool v1.0\n");
    fprintf(stderr, "Usage: %s [options] <command> [arguments]\n", prog_name);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -s <path>   Unix socket path (default: %s)\n", BFLBWIFID_DEFAULT_SOCKET_PATH);
    fprintf(stderr, "  -h          Show this help\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Commands:\n");
    for (size_t i = 0; i < bflbwifi_command_count(); i++) {
        const bflbwifi_command_desc_t *command = bflbwifi_command_at(i);
        if (!command) {
            continue;
        }

        if (command->usage_args[0] != '\0') {
            fprintf(stderr, "  %-28s %s\n",
                    command->cli_name,
                    command->summary);
            fprintf(stderr, "    usage: %s %s %s\n", prog_name, command->cli_name, command->usage_args);
        } else {
            fprintf(stderr, "  %-28s %s\n", command->cli_name, command->summary);
        }
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s connect_ap myssid\n", prog_name);
    fprintf(stderr, "  %s connect_ap myssid mypassword\n", prog_name);
    fprintf(stderr, "  %s scan\n", prog_name);
    fprintf(stderr, "  %s status\n", prog_name);
    fprintf(stderr, "  %s start_ap myap mypassword\n", prog_name);
}

static int set_socket_path(const char *path)
{
    if (!path || path[0] == '\0') {
        fprintf(stderr, "Error: Socket path cannot be empty\n");
        return -1;
    }

    if (strlen(path) >= sizeof(g_socket_path)) {
        fprintf(stderr, "Error: Socket path too long: %s\n", path);
        return -1;
    }

    memcpy(g_socket_path, path, strlen(path) + 1);
    return 0;
}

static int connect_to_daemon(void)
{
    int fd;
    int last_errno = 0;
    struct sockaddr_un addr;

    /* Check socket path length */
    if (strlen(g_socket_path) >= sizeof(addr.sun_path)) {
        fprintf(stderr, "Error: Socket path too long: %s\n", g_socket_path);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, g_socket_path, strlen(g_socket_path) + 1);

    for (int attempt = 0; attempt < DAEMON_CONNECT_RETRY_COUNT; attempt++) {
        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (fd < 0) {
            fprintf(stderr, "Error: Failed to create socket: %s\n", strerror(errno));
            return -1;
        }

        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
            return fd;
        }

        last_errno = errno;
        close(fd);

        if (last_errno != ENOENT && last_errno != ECONNREFUSED) {
            break;
        }

        usleep(DAEMON_CONNECT_RETRY_DELAY_US);
    }

    fprintf(stderr, "Error: Failed to connect to daemon: %s\n", strerror(last_errno));
    fprintf(stderr, "Make sure bflbwifid is running\n");
    return -1;
}

static int send_request_and_wait(int argc, const char *const argv[], bflbwifi_ipc_response_t *response)
{
    int fd;

    fd = connect_to_daemon();
    if (fd < 0) {
        return -1;
    }

    if (bflbwifi_ipc_send_request(fd, argc, argv) != 0) {
        fprintf(stderr, "Error: Failed to send IPC request\n");
        close(fd);
        return -1;
    }

    if (bflbwifi_ipc_recv_response(fd, response) != 0) {
        fprintf(stderr, "Error: Failed to receive IPC response\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int send_request_and_wait_for_status(int argc, const char *const argv[], bflbwifi_ipc_response_t *response)
{
    for (int attempt = 0; attempt < STATUS_REPLY_RETRY_COUNT; attempt++) {
        if (send_request_and_wait(argc, argv, response) == 0) {
            if (response->payload_len > 0 && response->payload && response->payload[0] != '\0') {
                return 0;
            }
            bflbwifi_ipc_response_deinit(response);
        }

        if (attempt + 1 < STATUS_REPLY_RETRY_COUNT) {
            usleep(STATUS_REPLY_RETRY_DELAY_US);
        }
    }

    return -1;
}

static int print_response_and_return(const bflbwifi_ipc_response_t *response)
{
    if (!response) {
        return -1;
    }

    if (response->payload) {
        printf("%s", response->payload);
    }

    return (response->status == 0) ? 0 : -1;
}

static void print_command_usage(const bflbwifi_command_desc_t *command)
{
    if (!command) {
        return;
    }

    if (command->usage_args[0] != '\0') {
        fprintf(stderr, "Usage: %s %s %s\n", g_prog_name, command->cli_name, command->usage_args);
    } else {
        fprintf(stderr, "Usage: %s %s\n", g_prog_name, command->cli_name);
    }
}

/* ========== Command Handler Functions ========== */

static int cmd_connect_ap(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    bflbwifi_ipc_response_t response;
    const char *request_args[BFLBWIFI_IPC_MAX_ARGS];
    int request_argc = 0;

    const char *ssid = argv[1];
    const char *password = (argc >= 3) ? argv[2] : "";

    printf("Connecting to '%s'...\n", ssid);

    request_args[request_argc++] = command->ipc_name;
    request_args[request_argc++] = ssid;
    if (strlen(password) > 0) {
        request_args[request_argc++] = password;
    }

    if (send_request_and_wait(request_argc, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_disconnect(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bflbwifi_ipc_response_t response;
    const char *request_args[] = {command->ipc_name};

    printf("Disconnecting...\n");

    if (send_request_and_wait(1, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_scan(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bflbwifi_ipc_response_t response;
    const char *request_args[] = {command->ipc_name};

    printf("Scanning for APs...\n");

    if (send_request_and_wait(1, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_status(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bflbwifi_ipc_response_t response;
    const char *request_args[] = {command->ipc_name};

    if (send_request_and_wait_for_status(1, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_version(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bflbwifi_ipc_response_t response;
    const char *request_args[] = {command->ipc_name};

    if (send_request_and_wait(1, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_reboot(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bflbwifi_ipc_response_t response;
    const char *request_args[] = {command->ipc_name};

    printf("Rebooting WiFi module...\n");

    if (send_request_and_wait(1, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_ota(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    bflbwifi_ipc_response_t response;
    char filepath_abs[PATH_MAX];
    const char *filepath;
    const char *request_args[BFLBWIFI_IPC_MAX_ARGS];

    (void)argc;

    filepath = argv[1];

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

    if (strlen(filepath) + 1 > MAX_BUFFER_SIZE) {
        fprintf(stderr, "Error: File path too long\n");
        return -1;
    }

    printf("Starting OTA upgrade: %s\n", filepath);

    request_args[0] = command->ipc_name;
    request_args[1] = filepath;
    if (send_request_and_wait(2, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_start_ap(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    bflbwifi_ipc_response_t response;
    const char *request_args[BFLBWIFI_IPC_MAX_ARGS];
    int request_argc = 0;

    const char *ssid = argv[1];
    const char *password = (argc >= 3) ? argv[2] : "";

    printf("Starting SoftAP '%s'...\n", ssid);

    request_args[request_argc++] = command->ipc_name;
    request_args[request_argc++] = ssid;
    if (strlen(password) > 0) {
        request_args[request_argc++] = password;
    }

    if (send_request_and_wait(request_argc, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

static int cmd_stop_ap(const bflbwifi_command_desc_t *command, int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    bflbwifi_ipc_response_t response;
    const char *request_args[] = {command->ipc_name};

    printf("Stopping SoftAP...\n");

    if (send_request_and_wait(1, request_args, &response) < 0) {
        return -1;
    }

    int ret = print_response_and_return(&response);
    bflbwifi_ipc_response_deinit(&response);
    return ret;
}

/* ========== Main Function ========== */

int main(int argc, char *argv[])
{
    int ret;
    int opt;
    int cmd_argc;
    char **cmd_argv;
    const bflbwifi_command_desc_t *command_desc;

    g_prog_name = argv[0];

    /* Parse options */
    while ((opt = getopt(argc, argv, "s:h")) != -1) {
        switch (opt) {
            case 's':
                if (set_socket_path(optarg) != 0) {
                    return 1;
                }
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
    cmd_argc = argc - optind;
    cmd_argv = &argv[optind];

    command_desc = bflbwifi_command_find_cli(cmd_argv[0]);
    if (!command_desc) {
        fprintf(stderr, "Error: Unknown command: %s\n\n", cmd_argv[0]);
        print_usage(g_prog_name);
        return 1;
    }

    if (bflbwifi_command_validate_argc(command_desc, cmd_argc - 1) != 0) {
        fprintf(stderr, "Error: Invalid arguments for command '%s'\n", command_desc->cli_name);
        print_command_usage(command_desc);
        return 1;
    }

    switch (command_desc->id) {
        case BFLBWIFI_COMMAND_CONNECT_AP:
            ret = cmd_connect_ap(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_DISCONNECT:
            ret = cmd_disconnect(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_SCAN:
            ret = cmd_scan(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_STATUS:
            ret = cmd_status(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_VERSION:
            ret = cmd_version(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_REBOOT:
            ret = cmd_reboot(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_OTA:
            ret = cmd_ota(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_START_AP:
            ret = cmd_start_ap(command_desc, cmd_argc, cmd_argv);
            break;
        case BFLBWIFI_COMMAND_STOP_AP:
            ret = cmd_stop_ap(command_desc, cmd_argc, cmd_argv);
            break;
        default:
            fprintf(stderr, "Error: Unsupported command id: %d\n", command_desc->id);
            return 1;
    }

    return (ret == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
