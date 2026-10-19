/**
 * @file bflbwifid.c
 * @brief BFLB WiFi daemon
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <errno.h>

#include "../include/bflbwifi_log.h"
#include "bflbwifid_config.h"
#include "bflbwifid_instance.h"
#include "bflbwifid_log.h"
#include "bflbwifid_request.h"
#include "bflbwifid_runtime.h"

/* ========== Global Variables ========== */

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
    sa.sa_flags = 0; /* Remove SA_RESTART, allow system calls to be interrupted */

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL); /* Ignore SIGPIPE */
}

/* ========== Main Function ========== */

static void daemonize(void)
{
    pid_t pid;

    /* First fork */
    pid = fork();
    if (pid < 0) {
        BFLBWIFID_LOGE("Failed to fork: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        /* Parent process exits */
        exit(EXIT_SUCCESS);
    }

    /* Child process becomes session leader */
    if (setsid() < 0) {
        BFLBWIFID_LOGE("Failed to setsid: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Second fork */
    pid = fork();
    if (pid < 0) {
        BFLBWIFID_LOGE("Failed to fork: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        /* Parent process exits */
        exit(EXIT_SUCCESS);
    }

    /* Set working directory */
    if (chdir("/") < 0) {
        BFLBWIFID_LOGE("Failed to change directory: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Redirect standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    /* Keep stderr open for log output */
}

int main(int argc, char *argv[])
{
    bflbwifid_config_t config;
    int ret;
    int server_fd = -1;
    int exit_code = EXIT_FAILURE;
    bool wifi_initialized = false;

    /* Print welcome message */
    fprintf(stderr, "BFLB WiFi Daemon v1.0\n");

    bflbwifid_config_init(&config);
    ret = bflbwifid_config_parse_argv(&config, argc, argv);
    if (ret == BFLBWIFID_CONFIG_PARSE_EXIT) {
        return EXIT_SUCCESS;
    }
    if (ret != BFLBWIFID_CONFIG_PARSE_OK) {
        bflbwifid_config_print_usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    /* Setup signal handlers */
    setup_signals();

    /* Daemon mode */
    if (config.daemon_mode) {
        daemonize();
    }

    if (bflbwifid_log_init(config.paths.log_file) != 0) {
        fprintf(stderr, "Warning: Failed to open log file %s: %s\n",
                config.paths.log_file,
                strerror(errno));
    }

    if (bflbwifid_instance_prepare_startup(&config.paths) != 0) {
        goto out;
    }

    if (bflbwifid_instance_write_pid_file(&config.paths) < 0) {
        goto out;
    }

    /* Initialize libbflbwifi */
    BFLBWIFID_LOGI("Initializing BFLB WiFi...");

    /* Set log level to DEBUG for detailed output */
    bflbwifi_log_set_level(LOG_DEBUG);

    bflbwifi_ctrl_config_t ctrl_cfg;

    bflbwifi_ctrl_config_init(&ctrl_cfg);
    if (config.ctrl_backend == BFLBWIFI_CTRL_BACKEND_VCHAN) {
        ret = bflbwifi_ctrl_config_use_vchan(&ctrl_cfg);
    } else {
        ret = bflbwifi_ctrl_config_use_tty(&ctrl_cfg,
                                           config.tty_dev,
                                           BFLBWIFI_CTRL_TTY_BAUDRATE_DEFAULT);
    }
    if (ret != 0) {
        BFLBWIFID_LOGE("Failed to prepare control backend config: %d", ret);
        goto out;
    }

    ret = bflbwifi_init_ex(&ctrl_cfg);
    if (ret != 0) {
        BFLBWIFID_LOGE("Failed to initialize BFLB WiFi: %d", ret);
        goto out;
    }
    wifi_initialized = true;

    server_fd = bflbwifid_instance_create_server(&config.paths);
    if (server_fd < 0) {
        BFLBWIFID_LOGE("Failed to create socket server");
        goto out;
    }

    if (bflbwifid_refresh_runtime_state(true) != 0) {
        BFLBWIFID_LOGE("Failed to initialize runtime state");
        goto out;
    }

    ret = bflbwifid_runtime_start(&g_running);
    if (ret != 0) {
        BFLBWIFID_LOGE("Failed to create daemon monitor thread: %s", strerror(ret));
        goto out;
    }

    BFLBWIFID_LOGI("BFLB WiFi daemon started successfully");
    if (config.ctrl_backend == BFLBWIFI_CTRL_BACKEND_VCHAN) {
        BFLBWIFID_LOGI("Channel: Virtualchannel (netlink)");
    } else {
        BFLBWIFID_LOGI("Channel: TTY device: %s", config.tty_dev);
    }
    BFLBWIFID_LOGI("Daemon paths: socket=%s pid=%s log=%s",
                   config.paths.socket_path,
                   config.paths.pid_file,
                   config.paths.log_file);

    /* Main loop */
    while (g_running) {
        int client_fd;
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);

        /* Set accept timeout (1 second) to periodically check g_running flag */
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        /* Accept client connection */
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_fd < 0) {
            if (errno == EINTR) {
                /* Interrupted by signal */
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* Timeout, continue loop to check g_running */
                continue;
            }
            BFLBWIFID_LOGE("Failed to accept: %s", strerror(errno));
            break;
        }

        /* Handle client request */
        bflbwifid_handle_client(client_fd);

        /* Close client connection */
        close(client_fd);
    }

    BFLBWIFID_LOGI("Main loop exited");
    exit_code = EXIT_SUCCESS;

out:
    g_running = 0;

    bflbwifid_runtime_stop();

    if (wifi_initialized) {
        bflbwifi_deinit();
    }

    bflbwifid_instance_cleanup(&config.paths, &server_fd);
    BFLBWIFID_LOGI("BFLB WiFi daemon stopped");
    bflbwifid_log_deinit();

    return exit_code;
}
