/**
 * @file tcp_client.c
 * @brief Shell-controlled socket-based TCP loopback client.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/errno.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"

#include "shell.h"
#include "tcp_client.h"

#define DBG_TAG "TCP_CLIENT"
#include "log.h"

#define TCP_CLIENT_TASK_STACK_DEPTH   512
#define TCP_CLIENT_TASK_PRIORITY      osPriorityAboveNormal
#define TCP_CLIENT_BUFFER_SIZE        1536
#define TCP_CLIENT_IO_TIMEOUT_MS      200
#define TCP_CLIENT_CONNECT_TIMEOUT_MS 1000
#define TCP_CLIENT_STOP_TIMEOUT_MS    1500

struct tcp_client_config_s {
    ip4_addr_t address;
    uint16_t port;
};

static TaskHandle_t tcp_client_task_handle;
static volatile bool tcp_client_stop_requested;
static struct tcp_client_config_s tcp_client_config;
static uint8_t tcp_client_buffer[TCP_CLIENT_BUFFER_SIZE];

/**
 * @brief Parse a decimal TCP port.
 */
static int tcp_client_parse_port(const char *text, uint16_t *port)
{
    char *end;
    unsigned long value;

    value = strtoul(text, &end, 10);
    if ((end == text) || (*end != '\0') || (value == 0) || (value > UINT16_MAX)) {
        return -1;
    }

    *port = (uint16_t)value;
    return 0;
}

/**
 * @brief Configure a connected socket with bounded I/O timeouts.
 */
static void tcp_client_set_io_timeout(int socket_fd)
{
    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = TCP_CLIENT_IO_TIMEOUT_MS * 1000,
    };

    (void)setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

/**
 * @brief Send one received buffer back completely.
 */
static int tcp_client_send_all(int socket_fd, const uint8_t *data, size_t length)
{
    size_t offset = 0;

    while ((offset < length) && !tcp_client_stop_requested) {
        ssize_t sent = send(socket_fd, data + offset, length - offset, 0);
        if (sent > 0) {
            offset += (size_t)sent;
            continue;
        }
        if ((sent < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            continue;
        }
        return -1;
    }

    return (offset == length) ? 0 : -1;
}

/**
 * @brief Connect a nonblocking socket with one bounded wait.
 */
static int tcp_client_connect(int socket_fd, const struct sockaddr_in *remote_addr)
{
    fd_set write_set;
    struct timeval timeout = {
        .tv_sec = TCP_CLIENT_CONNECT_TIMEOUT_MS / 1000,
        .tv_usec = (TCP_CLIENT_CONNECT_TIMEOUT_MS % 1000) * 1000,
    };
    unsigned long nonblocking = 1;
    int socket_error = 0;
    socklen_t error_length = sizeof(socket_error);
    int ret;

    if (ioctlsocket(socket_fd, FIONBIO, &nonblocking) < 0) {
        return -1;
    }

    ret = connect(socket_fd, (const struct sockaddr *)remote_addr, sizeof(*remote_addr));
    if ((ret < 0) && (errno != EINPROGRESS)) {
        return -1;
    }

    if (ret < 0) {
        FD_ZERO(&write_set);
        FD_SET(socket_fd, &write_set);
        ret = select(socket_fd + 1, NULL, &write_set, NULL, &timeout);
        if ((ret <= 0) || !FD_ISSET(socket_fd, &write_set) ||
            (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &socket_error, &error_length) < 0) ||
            (socket_error != 0)) {
            return -1;
        }
    }

    nonblocking = 0;
    if (ioctlsocket(socket_fd, FIONBIO, &nonblocking) < 0) {
        return -1;
    }

    return 0;
}

/**
 * @brief Connect to a remote endpoint and loop received data back to it.
 */
static void tcp_client_task(void *argument)
{
    const struct tcp_client_config_s *config = (const struct tcp_client_config_s *)argument;
    struct sockaddr_in remote_addr;
    int socket_fd;
    int no_delay = 1;

    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        LOG_E("Create TCP client socket failed\r\n");
        goto exit;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(config->port);
    remote_addr.sin_addr.s_addr = config->address.addr;

    LOG_I("TCP loopback client connecting to %s:%u\r\n", inet_ntoa(remote_addr.sin_addr), config->port);
    if (tcp_client_connect(socket_fd, &remote_addr) < 0) {
        if (!tcp_client_stop_requested) {
            LOG_E("TCP client connect failed\r\n");
        }
        closesocket(socket_fd);
        goto exit;
    }

    tcp_client_set_io_timeout(socket_fd);
    (void)setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay));
    LOG_I("TCP loopback client connected\r\n");

    while (!tcp_client_stop_requested) {
        ssize_t received = recv(socket_fd, tcp_client_buffer, sizeof(tcp_client_buffer), 0);
        if (received > 0) {
            if (tcp_client_send_all(socket_fd, tcp_client_buffer, (size_t)received) < 0) {
                break;
            }
            continue;
        }
        if ((received < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            continue;
        }
        break;
    }

    closesocket(socket_fd);

exit:
    LOG_I("TCP loopback client stopped\r\n");
    tcp_client_task_handle = NULL;
    vTaskDelete(NULL);
}

int tcp_client_start(const ip4_addr_t *address, uint16_t port)
{
    BaseType_t ret;

    if (tcp_client_task_handle != NULL) {
        LOG_W("TCP client is already running\r\n");
        return -1;
    }

    tcp_client_config.address = *address;
    tcp_client_config.port = port;
    tcp_client_stop_requested = false;
    ret = xTaskCreate(tcp_client_task, "tcp_client",
                      TCP_CLIENT_TASK_STACK_DEPTH, &tcp_client_config,
                      TCP_CLIENT_TASK_PRIORITY, &tcp_client_task_handle);
    if (ret != pdPASS) {
        tcp_client_task_handle = NULL;
        LOG_E("Create TCP client task failed\r\n");
        return -1;
    }

    return 0;
}

int tcp_client_stop(void)
{
    TickType_t start_tick;
    TickType_t timeout_ticks = pdMS_TO_TICKS(TCP_CLIENT_STOP_TIMEOUT_MS);

    if (tcp_client_task_handle == NULL) {
        LOG_W("TCP client is not running\r\n");
        return -1;
    }

    tcp_client_stop_requested = true;
    start_tick = xTaskGetTickCount();
    while (tcp_client_task_handle != NULL) {
        if ((xTaskGetTickCount() - start_tick) >= timeout_ticks) {
            LOG_E("Stop TCP client timed out\r\n");
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return 0;
}

static int tcp_client_shell(int argc, char **argv)
{
    ip4_addr_t address;
    uint16_t port = TCP_CLIENT_DEFAULT_PORT;

    if ((argc == 2) && (strcmp(argv[1], "stop") == 0)) {
        return tcp_client_stop();
    }

    if (((argc != 3) && (argc != 4)) || (strcmp(argv[1], "start") != 0)) {
        printf("Usage: tcp_client start <ip> [port] | stop\r\n");
        return -1;
    }

    if (!ip4addr_aton(argv[2], &address)) {
        printf("Invalid IPv4 address: %s\r\n", argv[2]);
        return -1;
    }
    if ((argc == 4) && (tcp_client_parse_port(argv[3], &port) < 0)) {
        printf("Invalid TCP client port: %s\r\n", argv[3]);
        return -1;
    }

    return tcp_client_start(&address, port);
}
SHELL_CMD_EXPORT_ALIAS(tcp_client_shell, tcp_client, TCP loopback client : start IP[port] or stop);
