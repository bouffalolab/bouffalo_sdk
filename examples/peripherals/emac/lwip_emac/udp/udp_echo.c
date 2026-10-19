/**
 * @file udp_echo.c
 * @brief Shell-controlled socket-based UDP echo service.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/sockets.h"

#include "shell.h"
#include "udp_echo.h"

#define DBG_TAG "UDP_ECHO"
#include "log.h"

#define UDP_ECHO_TASK_STACK_DEPTH 512
#define UDP_ECHO_TASK_PRIORITY    osPriorityNormal
#define UDP_ECHO_BUFFER_SIZE      1536
#define UDP_ECHO_RECV_TIMEOUT_MS  200
#define UDP_ECHO_STOP_TIMEOUT_MS  1000

static TaskHandle_t udp_echo_task_handle;
static volatile bool udp_echo_stop_requested;
static uint16_t udp_echo_port = UDP_ECHO_DEFAULT_PORT;
static uint8_t udp_echo_buffer[UDP_ECHO_BUFFER_SIZE];

/**
 * @brief Parse a decimal TCP/UDP port.
 *
 * @retval 0 The port is valid and stored in @p port.
 * @retval -1 The argument is not a valid port in the range 1..65535.
 */
static int udp_echo_parse_port(const char *text, uint16_t *port)
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
 * @brief Run the UDP echo server until a stop is requested.
 */
static void udp_echo_task(void *argument)
{
    struct sockaddr_in local_addr;
    struct timeval timeout = {
        .tv_sec = 0,
        .tv_usec = UDP_ECHO_RECV_TIMEOUT_MS * 1000,
    };
    uint16_t port = (uint16_t)(uintptr_t)argument;
    int socket_fd;
    int reuse = 1;

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd < 0) {
        LOG_E("Create UDP socket failed\r\n");
        goto exit;
    }

    (void)setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    (void)setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        LOG_E("Bind UDP port %u failed\r\n", port);
        closesocket(socket_fd);
        goto exit;
    }

    LOG_I("UDP echo service listening on port %u\r\n", port);

    while (!udp_echo_stop_requested) {
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        ssize_t received;
        ssize_t sent;

        received = recvfrom(socket_fd, udp_echo_buffer, sizeof(udp_echo_buffer), 0,
                            (struct sockaddr *)&peer_addr, &peer_addr_len);
        if (received < 0) {
            continue;
        }

        sent = sendto(socket_fd, udp_echo_buffer, (size_t)received, 0,
                      (struct sockaddr *)&peer_addr, peer_addr_len);
        if (sent != received) {
            LOG_W("Echo UDP datagram failed\r\n");
        }
    }

    closesocket(socket_fd);

exit:
    LOG_I("UDP echo service stopped\r\n");
    udp_echo_task_handle = NULL;
    vTaskDelete(NULL);
}

int udp_echo_start(uint16_t port)
{
    BaseType_t ret;

    if (udp_echo_task_handle != NULL) {
        LOG_W("UDP echo service already running on port %u\r\n", udp_echo_port);
        return -1;
    }

    udp_echo_port = port;
    udp_echo_stop_requested = false;
    ret = xTaskCreate(udp_echo_task, "udp_echo", UDP_ECHO_TASK_STACK_DEPTH,
                      (void *)(uintptr_t)port, UDP_ECHO_TASK_PRIORITY,
                      &udp_echo_task_handle);
    if (ret != pdPASS) {
        udp_echo_task_handle = NULL;
        LOG_E("Create UDP echo task failed\r\n");
        return -1;
    }

    return 0;
}

int udp_echo_stop(void)
{
    TickType_t start_tick;
    TickType_t timeout_ticks = pdMS_TO_TICKS(UDP_ECHO_STOP_TIMEOUT_MS);

    if (udp_echo_task_handle == NULL) {
        LOG_W("UDP echo service is not running\r\n");
        return -1;
    }

    udp_echo_stop_requested = true;
    start_tick = xTaskGetTickCount();
    while (udp_echo_task_handle != NULL) {
        if ((xTaskGetTickCount() - start_tick) >= timeout_ticks) {
            LOG_E("Stop UDP echo service timed out\r\n");
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return 0;
}

static int udp_echo_shell(int argc, char **argv)
{
    uint16_t port = UDP_ECHO_DEFAULT_PORT;

    if ((argc < 2) || (argc > 3)) {
        printf("Usage: udp_echo start [port] | stop\r\n");
        return -1;
    }

    if (strcmp(argv[1], "start") == 0) {
        if ((argc == 3) && (udp_echo_parse_port(argv[2], &port) < 0)) {
            printf("Invalid UDP port: %s\r\n", argv[2]);
            return -1;
        }
        return udp_echo_start(port);
    }

    if ((strcmp(argv[1], "stop") == 0) && (argc == 2)) {
        return udp_echo_stop();
    }

    printf("Usage: udp_echo start [port] | stop\r\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(udp_echo_shell, udp_echo, UDP echo: start [port] or stop);
