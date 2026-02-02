/**
 * @file bflbwifid.c
 * @brief BFLB WiFi daemon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

#include "../include/bflbwifi.h"
#include "../include/bflbwifi_log.h"
#include "bflbwifid.h"

/* ========== Global Variables ========== */

static bool g_running = true;
static int g_server_fd = -1;
static char g_tty_dev[128] = "/dev/ttyUSB0";
static int g_log_fd = -1;

/* ========== Log Functions ========== */

static void log_write(const char *level, const char *fmt, ...)
{
    char buf[MAX_BUFFER_SIZE];
    char timestamp[64];
    va_list args;
    time_t now;
    struct tm *tm_info;

    /* Get timestamp */
    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    /* Format log message */
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    /* Output to stderr */
    fprintf(stderr, "[%s] [%s] %s\n", timestamp, level, buf);

    /* Output to log file */
    if (g_log_fd >= 0) {
        char log_buf[MAX_BUFFER_SIZE];
        int len = snprintf(log_buf, sizeof(log_buf), "[%s] [%s] %s\n",
                          timestamp, level, buf);
        if (write(g_log_fd, log_buf, len) < 0) {
            /* Ignore write errors to avoid log errors affecting main program */
        }
    }
}

#define LOGI(fmt, ...) log_write("INFO", fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) log_write("ERROR", fmt, ##__VA_ARGS__)

/* ========== Signal Handling ========== */

static void cleanup_resources(void)
{
    LOGI("Cleaning up resources...");

    /* Close server socket */
    if (g_server_fd >= 0) {
        close(g_server_fd);
        g_server_fd = -1;
    }

    /* Remove Unix socket file */
    if (access(UNIX_SOCKET_PATH, F_OK) == 0) {
        unlink(UNIX_SOCKET_PATH);
    }

    /* Close log file */
    if (g_log_fd >= 0) {
        close(g_log_fd);
        g_log_fd = -1;
    }

    /* Remove PID file */
    if (access(PID_FILE, F_OK) == 0) {
        unlink(PID_FILE);
    }

    LOGI("Cleanup complete");
}

static void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        LOGI("Received signal %d, shutting down...", sig);
        g_running = false;
        cleanup_resources();
    }
}

static void setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  /* Remove SA_RESTART, allow system calls to be interrupted */

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);  /* Ignore SIGPIPE */
}

/* ========== PID File Management ========== */

static int write_pid_file(void)
{
    char buf[32];
    int fd;
    int pid;

    /* Create PID file */
    fd = open(PID_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        LOGE("Failed to create PID file: %s", strerror(errno));
        return -1;
    }

    /* Write PID */
    pid = getpid();
    snprintf(buf, sizeof(buf), "%d\n", pid);
    if (write(fd, buf, strlen(buf)) < 0) {
        LOGE("Failed to write PID file: %s", strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);

    LOGI("PID file created: %s (PID=%d)", PID_FILE, pid);

    return 0;
}

static void remove_pid_file(void)
{
    unlink(PID_FILE);
    LOGI("PID file removed");
}

/* ========== Unix Socket Server ========== */

static int create_unix_socket_server(void)
{
    int server_fd;
    struct sockaddr_un addr;

    /* Create Unix Domain Socket */
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        LOGE("Failed to create socket: %s", strerror(errno));
        return -1;
    }

    /* Set socket options */
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    /* Bind socket address */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, UNIX_SOCKET_PATH, sizeof(addr.sun_path) - 1);

    /* Remove existing socket file */
    unlink(UNIX_SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOGE("Failed to bind socket: %s", strerror(errno));
        close(server_fd);
        return -1;
    }

    /* Set permissions */
    chmod(UNIX_SOCKET_PATH, 0777);

    /* Start listening */
    if (listen(server_fd, 5) < 0) {
        LOGE("Failed to listen: %s", strerror(errno));
        close(server_fd);
        unlink(UNIX_SOCKET_PATH);
        return -1;
    }

    LOGI("Unix socket server started: %s", UNIX_SOCKET_PATH);

    return server_fd;
}

/* ========== Command Handler Functions ========== */

static void handle_connect_ap(int client_fd, const char *ssid, const char *password)
{
    int ret;
    char response[MAX_RESPONSE_LEN];

    LOGI("Connect to AP: %s", ssid);

    /* Prepare connection configuration */
    bflbwifi_sta_config_t config;
    strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    strncpy(config.pwd, password, sizeof(config.pwd) - 1);

    /* Call libbflbwifi API */
    ret = bflbwifi_set_station_dhcpc(0);
    if (ret == 0) {
        LOGI("Force disable sta dhcpclient.");
    }

    /* Call libbflbwifi API */
    ret = bflbwifi_sta_connect(&config, 30000);

    /* Send response */
    if (ret == 0) {
        /* Get connection information */
        bflbwifi_sta_connection_info_t info;
        if (bflbwifi_get_sta_connection_info(&info) == 0) {
            snprintf(response, sizeof(response),
                    "OK CONNECTED\n"
                    "SSID: %s\n"
                    "BSSID: %s\n"
                    "Channel: %d\n"
                    "RSSI: %d dBm\n"
                    "END\n",
                    info.ssid, info.bssid, info.channel, info.rssi);
        } else {
            snprintf(response, sizeof(response),
                    "OK CONNECTED\n"
                    "END\n");
        }
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\n"
                "END\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_disconnect(int client_fd)
{
    int ret;
    char response[MAX_RESPONSE_LEN];

    LOGI("Disconnect");

    ret = bflbwifi_sta_disconnect();

    if (ret == 0) {
        snprintf(response, sizeof(response), "OK DISCONNECTED\nEND\n");
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_scan(int client_fd)
{
    int ret;
    char response[MAX_RESPONSE_LEN];
    bflbwifi_ap_info_t ap_list[32];
    int actual_count;

    LOGI("Scan APs");

    ret = bflbwifi_scan(ap_list, 32, &actual_count, 30000);

    if (ret == 0 || ret == -11) {
        /* Scan successful or buffer too small (but returned partial results) */
        char *ptr = response;
        int remaining = sizeof(response);

        ptr += snprintf(ptr, remaining, "OK SCAN_COMPLETE\n");
        remaining -= (ptr - response);

        for (int i = 0; i < actual_count && remaining > 100; i++) {
            ptr += snprintf(ptr, remaining,
                           "AP[%d]: %s (CH:%d, RSSI:%d, MAC:%s)\n",
                           i + 1, ap_list[i].ssid,
                           ap_list[i].channel, ap_list[i].rssi, ap_list[i].bssid);
            remaining -= (ptr - response);
        }

        snprintf(ptr, remaining, "TOTAL: %d\nEND\n", actual_count);
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_status(int client_fd)
{
    char response[MAX_RESPONSE_LEN];
    bflbwifi_sta_state_t state;

    LOGI("Get status");

    if (bflbwifi_sta_get_state(&state) == 0) {
        const char *state_str;
        switch (state) {
            case WIFI_STATE_IDLE:       state_str = "IDLE"; break;
            case WIFI_STATE_CONNECTING: state_str = "CONNECTING"; break;
            case WIFI_STATE_CONNECTED:  state_str = "CONNECTED"; break;
            case WIFI_STATE_GOTIP:       state_str = "GOTIP"; break;
            case WIFI_STATE_DISCONNECTED: state_str = "DISCONNECTED"; break;
            case WIFI_STATE_ERROR:      state_str = "ERROR"; break;
            default:                    state_str = "UNKNOWN"; break;
        }

        /* Get detailed information */
        bflbwifi_sta_connection_info_t info;
        if (bflbwifi_get_sta_connection_info(&info) == 0 && state == WIFI_STATE_CONNECTED) {
            snprintf(response, sizeof(response),
                    "OK STATUS\n"
                    "State: %s\n"
                    "SSID: %s\n"
                    "BSSID: %s\n"
                    "Channel: %d\n"
                    "RSSI: %d dBm\n"
                    "END\n",
                    state_str, info.ssid, info.bssid,
                    info.channel, info.rssi);
        } else {
            snprintf(response, sizeof(response),
                    "OK STATUS\n"
                    "State: %s\n"
                    "END\n",
                    state_str);
        }
    } else {
        snprintf(response, sizeof(response),
                "ERROR -1 Failed to get status\nEND\n");
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_version(int client_fd)
{
    char response[MAX_RESPONSE_LEN];
    char version[64];

    LOGI("Get version");

    if (bflbwifi_get_version(version, sizeof(version)) == 0) {
        snprintf(response, sizeof(response),
                "OK VERSION\n"
                "Version: %s\n"
                "END\n",
                version);
    } else {
        snprintf(response, sizeof(response),
                "ERROR -1 Failed to get version\nEND\n");
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_set_mode(int client_fd, int mode)
{
    int ret;
    char response[MAX_RESPONSE_LEN];

    LOGI("Set mode: %d", mode);

    ret = bflbwifi_set_mode((bflbwifi_mode_t)mode);

    if (ret == 0) {
        snprintf(response, sizeof(response), "OK MODE_SET\nEND\n");
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_reboot(int client_fd)
{
    int ret;
    char response[MAX_RESPONSE_LEN];

    LOGI("Reboot WiFi module");

    ret = bflbwifi_restart();

    if (ret == 0) {
        snprintf(response, sizeof(response), "OK REBOOTING\nEND\n");
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_ota(int client_fd, const char *filepath)
{
    char response[MAX_RESPONSE_LEN];
    int ret;

    if (!filepath || strlen(filepath) == 0) {
        send(client_fd, "ERROR -4 Missing OTA file path\nEND\n", 37, 0);
        return;
    }

    LOGI("OTA upgrade: %s", filepath);

    /* Execute OTA */
    ret = bflbwifi_ota_upgrade(filepath);

    /* Send response */
    if (ret == 0) {
        snprintf(response, sizeof(response), "OK OTA_COMPLETE\nEND\n");
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_start_ap(int client_fd, const char *ssid, const char *password)
{
    int ret;
    char response[MAX_RESPONSE_LEN];

    if (!ssid || strlen(ssid) == 0) {
        send(client_fd, "ERROR -4 Missing SSID\nEND\n", 30, 0);
        return;
    }

    LOGI("Start AP: %s", ssid);

    /* Prepare AP configuration */
    bflbwifi_ap_config_t config;
    memset(&config, 0, sizeof(config));
    strncpy(config.ssid, ssid, sizeof(config.ssid) - 1);
    if (password) {
        strncpy(config.pwd, password, sizeof(config.pwd) - 1);
    } else {
        config.pwd[0] = '\0';  /* Open network */
    }
    config.channel = 11;       /* Default channel */
    config.enc = WIFI_ENC_WPA2;  /* Default encryption */
    config.max_conn = 4;       /* Default max connections */
    config.ssid_hidden = 0;    /* Not hidden */

    /* Call libbflbwifi API */
    ret = bflbwifi_ap_start(&config, 30000);

    /* Send response */
    if (ret == 0) {
        snprintf(response, sizeof(response),
                "OK AP_STARTED\n"
                "SSID: %s\n"
                "Channel: %d\n"
                "IP: 192.168.4.1\n"
                "DHCP: 192.168.4.50 - 192.168.4.200\n"
                "END\n",
                config.ssid, config.channel);
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void handle_stop_ap(int client_fd)
{
    int ret;
    char response[MAX_RESPONSE_LEN];

    LOGI("Stop AP");

    ret = bflbwifi_ap_stop();

    if (ret == 0) {
        snprintf(response, sizeof(response), "OK AP_STOPPED\nEND\n");
    } else {
        snprintf(response, sizeof(response),
                "ERROR %d %s\nEND\n",
                ret, bflbwifi_strerror(ret));
    }

    send(client_fd, response, strlen(response), 0);
}

static void process_client_command(int client_fd, const char *cmd)
{
    char cmd_copy[MAX_CMD_LEN];
    char *token;
    char *ssid = NULL, *password = NULL;

    strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';

    /* Remove newline */
    char *newline = strchr(cmd_copy, '\n');
    if (newline) *newline = '\0';

    token = strtok(cmd_copy, " ");

    if (token == NULL) {
        send(client_fd, "ERROR -4 Invalid command\nEND\n", 30, 0);
        return;
    }

    /* Check OTA timeout (30 minutes auto-recovery) */
    extern time_t bflbwifi_ota_get_start_time(void);
    if (bflbwifi_is_ota_in_progress()) {
        time_t start_time = bflbwifi_ota_get_start_time();
        if (start_time > 0 && (time(NULL) - start_time > 1800)) {  // 30 minutes
            LOGE("OTA timeout detected, auto-recovery triggered");
            extern void bflbwifi_ota_reset(void);
            bflbwifi_ota_reset();
        }
    }

    /* Check OTA status BEFORE parsing command (reject all except OTA itself) */
    if (bflbwifi_is_ota_in_progress() && strcmp(token, "OTA") != 0) {
        send(client_fd, "ERROR -12 OTA in progress, try later\nEND\n", 43, 0);
        LOGI("Command rejected due to OTA in progress");
        return;
    }

    /* Parse command */
    if (strcmp(token, "CONNECT_AP") == 0) {
        ssid = strtok(NULL, " ");
        password = strtok(NULL, " ");

        if (!ssid || !password) {
            send(client_fd, "ERROR -4 Missing parameters\nEND\n", 35, 0);
            return;
        }

        handle_connect_ap(client_fd, ssid, password);
    }
    else if (strcmp(token, "DISCONNECT") == 0) {
        handle_disconnect(client_fd);
    }
    else if (strcmp(token, "SCAN") == 0) {
        handle_scan(client_fd);
    }
    else if (strcmp(token, "STATUS") == 0) {
        handle_status(client_fd);
    }
    else if (strcmp(token, "VERSION") == 0) {
        handle_version(client_fd);
    }
    else if (strcmp(token, "SET_MODE") == 0) {
        token = strtok(NULL, " ");
        if (!token) {
            send(client_fd, "ERROR -4 Missing mode parameter\nEND\n", 38, 0);
            return;
        }
        int mode = atoi(token);
        handle_set_mode(client_fd, mode);
    }
    else if (strcmp(token, "REBOOT") == 0) {
        handle_reboot(client_fd);
    }
    else if (strcmp(token, "OTA") == 0) {
        char *filepath = strtok(NULL, " ");
        handle_ota(client_fd, filepath);
    }
    else if (strcmp(token, "START_AP") == 0) {
        ssid = strtok(NULL, " ");
        password = strtok(NULL, " ");

        if (!ssid) {
            send(client_fd, "ERROR -4 Missing SSID parameter\nEND\n", 42, 0);
            return;
        }

        handle_start_ap(client_fd, ssid, password);
    }
    else if (strcmp(token, "STOP_AP") == 0) {
        handle_stop_ap(client_fd);
    }
    else if (strcmp(token, "QUIT") == 0) {
        LOGI("Quit command received");
        send(client_fd, "OK QUIT\nEND\n", 13, 0);
        g_running = false;
    }
    else {
        char response[MAX_RESPONSE_LEN];
        snprintf(response, sizeof(response),
                "ERROR -4 Unknown command: %s\nEND\n", token);
        send(client_fd, response, strlen(response), 0);
    }
}

static void handle_client(int client_fd)
{
    char buffer[MAX_BUFFER_SIZE];
    int n;

    /* Receive command */
    n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        return;
    }

    buffer[n] = '\0';

    /* Handle command */
    process_client_command(client_fd, buffer);
}

/* ========== Main Function ========== */

static void daemonize(void)
{
    pid_t pid;

    /* First fork */
    pid = fork();
    if (pid < 0) {
        LOGE("Failed to fork: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        /* Parent process exits */
        exit(EXIT_SUCCESS);
    }

    /* Child process becomes session leader */
    if (setsid() < 0) {
        LOGE("Failed to setsid: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Second fork */
    pid = fork();
    if (pid < 0) {
        LOGE("Failed to fork: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        /* Parent process exits */
        exit(EXIT_SUCCESS);
    }

    /* Set working directory */
    if (chdir("/") < 0) {
        LOGE("Failed to change directory: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Redirect standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    /* Keep stderr open for log output */
}

int main(int argc, char *argv[])
{
    int ret;
    bool daemon_mode = false;

    /* Print welcome message */
    fprintf(stderr, "BFLB WiFi Daemon v1.0\n");

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            strncpy(g_tty_dev, argv[i + 1], sizeof(g_tty_dev) - 1);
            i++;
        } else if (strcmp(argv[i], "-d") == 0) {
            daemon_mode = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            fprintf(stderr, "Usage: %s [options]\n", argv[0]);
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "  -p <dev>   TTY device (default: /dev/ttyUSB0)\n");
            fprintf(stderr, "  -d         Run as daemon\n");
            fprintf(stderr, "  -h         Show this help\n");
            return 0;
        }
    }

    /* Open log file */
    g_log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (g_log_fd < 0) {
        fprintf(stderr, "Warning: Failed to open log file: %s\n", strerror(errno));
    }

    /* Setup signal handlers */
    setup_signals();

    /* Daemon mode */
    if (daemon_mode) {
        daemonize();
    }

    /* Write PID file */
    if (write_pid_file() < 0) {
        return EXIT_FAILURE;
    }

    /* Initialize libbflbwifi */
    LOGI("Initializing BFLB WiFi...");

    /* Set log level to DEBUG for detailed output */
    bflbwifi_log_set_level(LOG_DEBUG);

    ret = bflbwifi_init(g_tty_dev, 115200);
    if (ret != 0) {
        LOGE("Failed to initialize BFLB WiFi: %d", ret);
        remove_pid_file();
        return EXIT_FAILURE;
    }

    /* Create Unix socket server */
    g_server_fd = create_unix_socket_server();
    if (g_server_fd < 0) {
        LOGE("Failed to create socket server");
        bflbwifi_deinit();
        remove_pid_file();
        return EXIT_FAILURE;
    }

    LOGI("BFLB WiFi daemon started successfully");
    LOGI("TTY device: %s", g_tty_dev);

    /* Main loop */
    while (g_running) {
        int client_fd;
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);

        /* Set accept timeout (1 second) to periodically check g_running flag */
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(g_server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        /* Accept client connection */
        client_fd = accept(g_server_fd,
                          (struct sockaddr *)&client_addr,
                          &client_len);

        if (client_fd < 0) {
            if (errno == EINTR) {
                /* Interrupted by signal */
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Timeout, continue loop to check g_running */
                continue;
            }
            LOGE("Failed to accept: %s", strerror(errno));
            break;
        }

        /* Handle client request */
        handle_client(client_fd);

        /* Close client connection */
        close(client_fd);
    }

    LOGI("Main loop exited");

    /* Cleanup WiFi resources (note: socket, PID file, log already cleaned in cleanup_resources) */
    bflbwifi_deinit();

    LOGI("BFLB WiFi daemon stopped");

    return EXIT_SUCCESS;
}
