#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

#include "bflbwifid_config.h"
#include "bflbwifid_instance.h"
#include "bflbwifid_service.h"
#include "nethub_log.h"
#include "nethub_wifi.h"
#include "nethub_wifi_service.h"

static volatile sig_atomic_t g_running = 1;

static void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        g_running = 0;
    }
}

static void setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, NULL);
}

static void daemonize_process(void)
{
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (chdir("/") != 0) {
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
}

int main(int argc, char *argv[])
{
    bflbwifid_config_t config;
    int server_fd = -1;
    int exit_code = EXIT_FAILURE;
    int ret;
    bool service_started = false;

    bflbwifid_config_init(&config);
    ret = bflbwifid_config_parse_argv(&config, argc, argv);
    if (ret == BFLBWIFID_CONFIG_PARSE_EXIT) {
        return EXIT_SUCCESS;
    }
    if (ret != BFLBWIFID_CONFIG_PARSE_OK) {
        bflbwifid_config_print_usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "BFLB WiFi Daemon v2.0\n");

    setup_signals();
    if (config.daemon_mode) {
        daemonize_process();
    }

    if (nethub_log_init(config.paths.log_file) != 0) {
        fprintf(stderr, "Warning: failed to open log file %s: %s\n", config.paths.log_file, strerror(errno));
    }

    if (bflbwifid_instance_prepare_startup(&config.paths) != 0 ||
        bflbwifid_instance_write_pid_file(&config.paths) != 0) {
        goto out;
    }

    ret = nethub_wifi_service_init();
    if (ret != 0) {
        NETHUB_LOGE("Failed to initialize WiFi service: %d (%s)", ret, nethub_wifi_strerror(ret));
        if (!config.daemon_mode) {
            fprintf(stderr, "Failed to initialize WiFi service: %d (%s)\n", ret, nethub_wifi_strerror(ret));
        }
        goto out;
    }
    service_started = true;

    server_fd = bflbwifid_instance_create_server(&config.paths);
    if (server_fd < 0) {
        goto out;
    }

    NETHUB_LOGI("bflbwifid started: socket=%s pid=%s log=%s",
                config.paths.socket_path,
                config.paths.pid_file,
                config.paths.log_file);
    if (!config.daemon_mode) {
        fprintf(stderr, "bflbwifid started: socket=%s\n", config.paths.socket_path);
    }

    while (g_running) {
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);
        struct timeval tv;
        int client_fd;

        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            NETHUB_LOGE("accept failed: %s", strerror(errno));
            break;
        }

        bflbwifid_handle_client(client_fd);
        close(client_fd);
    }

    exit_code = EXIT_SUCCESS;

out:
    g_running = 0;
    if (service_started) {
        nethub_wifi_service_deinit();
    }
    bflbwifid_instance_cleanup(&config.paths, &server_fd);
    NETHUB_LOGI("bflbwifid stopped");
    nethub_log_deinit();
    return exit_code;
}
