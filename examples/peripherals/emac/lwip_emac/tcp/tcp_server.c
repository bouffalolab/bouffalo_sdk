/**
 * @file tcp_server.c
 * @brief Shell-controlled socket-based TCP loopback server.
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
#include "tcp_server.h"

#define DBG_TAG "TCP_SERVER"
#include "log.h"

#define TCP_SERVER_TASK_STACK_DEPTH 512
#define TCP_SERVER_TASK_PRIORITY    osPriorityAboveNormal
#define TCP_SERVER_BACKLOG          4
#define TCP_SERVER_BUFFER_SIZE      1536
#define TCP_SERVER_IO_TIMEOUT_MS    200
#define TCP_SERVER_STOP_TIMEOUT_MS  1000

static TaskHandle_t tcp_server_task_handle;
static volatile bool tcp_server_stop_requested;
static uint16_t tcp_server_port = TCP_SERVER_DEFAULT_PORT;
static uint8_t tcp_server_buffer[TCP_SERVER_BUFFER_SIZE];

/**
 * @brief Parse a decimal TCP port.
 */
static int tcp_server_parse_port(const char *text, uint16_t *port)
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
static void tcp_server_set_io_timeout(int socket_fd)
{
    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = TCP_SERVER_IO_TIMEOUT_MS * 1000,
    };

    (void)setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

/**
 * @brief Send one received buffer back completely.
 */
static int tcp_server_send_all(int socket_fd, const uint8_t *data, size_t length)
{
    size_t offset = 0;

    while ((offset < length) && !tcp_server_stop_requested) {
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
 * @brief Run the independent TCP loopback server.
 */
static void tcp_server_task(void *argument)
{
    struct sockaddr_in local_addr;
    struct sockaddr_in peer_addr;
    struct timeval accept_timeout = {
        .tv_sec = 0,
        .tv_usec = TCP_SERVER_IO_TIMEOUT_MS * 1000,
    };
    socklen_t peer_addr_len;
    uint16_t port = (uint16_t)(uintptr_t)argument;
    int listen_fd;
    int connection_fd;
    int reuse = 1;
    int no_delay = 1;

    listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_fd < 0) {
        LOG_E("Create TCP server socket failed\r\n");
        goto exit;
    }

    (void)setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    (void)setsockopt(listen_fd, SOL_SOCKET, SO_RCVTIMEO, &accept_timeout, sizeof(accept_timeout));

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        LOG_E("Bind TCP server port %u failed\r\n", port);
        closesocket(listen_fd);
        goto exit;
    }

    if (listen(listen_fd, TCP_SERVER_BACKLOG) < 0) {
        LOG_E("Listen on TCP server port %u failed\r\n", port);
        closesocket(listen_fd);
        goto exit;
    }

    LOG_I("TCP loopback server listening on port %u\r\n", port);

wait_connection:
    while (!tcp_server_stop_requested) {
        peer_addr_len = sizeof(peer_addr);
        connection_fd = accept(listen_fd, (struct sockaddr *)&peer_addr, &peer_addr_len);
        if (connection_fd < 0) {
            continue;
        }

        LOG_I("TCP server accepted %s:%u\r\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
        tcp_server_set_io_timeout(connection_fd);
        (void)setsockopt(connection_fd, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay));
        goto data_loop;
    }
    closesocket(listen_fd);
    LOG_I("TCP loopback server stopped\r\n");
    goto exit;

data_loop:
    while (!tcp_server_stop_requested) {
        ssize_t received = recv(connection_fd, tcp_server_buffer, sizeof(tcp_server_buffer), 0);
        if (received > 0) {
            if (tcp_server_send_all(connection_fd, tcp_server_buffer, (size_t)received) < 0) {
                break;
            }
            continue;
        }
        if ((received < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            continue;
        }
        break;
    }
    closesocket(connection_fd);
    LOG_I("TCP server peer disconnected\r\n");
    goto wait_connection;

exit:
    tcp_server_task_handle = NULL;
    vTaskDelete(NULL);
}

int tcp_server_start(uint16_t port)
{
    BaseType_t ret;

    if (tcp_server_task_handle != NULL) {
        LOG_W("TCP server already running on port %u\r\n", tcp_server_port);
        return -1;
    }

    tcp_server_port = port;
    tcp_server_stop_requested = false;
    ret = xTaskCreate(tcp_server_task, "tcp_server",
                      TCP_SERVER_TASK_STACK_DEPTH, (void *)(uintptr_t)port,
                      TCP_SERVER_TASK_PRIORITY, &tcp_server_task_handle);
    if (ret != pdPASS) {
        tcp_server_task_handle = NULL;
        LOG_E("Create TCP server task failed\r\n");
        return -1;
    }

    return 0;
}

int tcp_server_stop(void)
{
    TickType_t start_tick;
    TickType_t timeout_ticks = pdMS_TO_TICKS(TCP_SERVER_STOP_TIMEOUT_MS);

    if (tcp_server_task_handle == NULL) {
        LOG_W("TCP server is not running\r\n");
        return -1;
    }

    tcp_server_stop_requested = true;
    start_tick = xTaskGetTickCount();
    while (tcp_server_task_handle != NULL) {
        if ((xTaskGetTickCount() - start_tick) >= timeout_ticks) {
            LOG_E("Stop TCP server timed out\r\n");
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return 0;
}

static int tcp_server_shell(int argc, char **argv)
{
    uint16_t port = TCP_SERVER_DEFAULT_PORT;

    if ((argc < 2) || (argc > 3)) {
        printf("Usage: tcp_server start [port] | stop\r\n");
        return -1;
    }

    if (strcmp(argv[1], "start") == 0) {
        if ((argc == 3) && (tcp_server_parse_port(argv[2], &port) < 0)) {
            printf("Invalid TCP server port: %s\r\n", argv[2]);
            return -1;
        }
        return tcp_server_start(port);
    }

    if ((strcmp(argv[1], "stop") == 0) && (argc == 2)) {
        return tcp_server_stop();
    }

    printf("Usage: tcp_server start [port] | stop\r\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(tcp_server_shell, tcp_server, TCP loopback server : start[port] or stop);
