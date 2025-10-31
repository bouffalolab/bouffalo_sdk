#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "bl_lp.h"
#include "bl616_pm.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bl616_glb.h"
#include "bl616_glb_gpio.h"
#include "bl616_hbn.h"
#include "bflb_rtc.h"
#include "shell.h"

extern int enable_tickless;
void timerCallback(TimerHandle_t xTimer)
{
    enable_tickless = 0;
    xTimerDelete(xTimer, portMAX_DELAY);

    //if (wifi_mgmr_sta_state_get()) {
    //    wifi_mgmr_sta_ps_exit();
    //}
}

void createAndStartTimer(const char* timerName, TickType_t timerPeriod)
{
    TimerHandle_t timer = xTimerCreate(timerName,
                                       timerPeriod,
                                       pdTRUE,
                                       0,
                                       timerCallback
                                    );

    if (timer == NULL)
    {
        printf("Failed to create timer.\n");
        return;
    }

    if (xTimerStart(timer, 0) != pdPASS)
    {
        printf("Failed to start timer.\n");
        return;
    }
}

void app_pm_enter_pds15(uint32_t timeouts_ms)
{
    if (timeouts_ms) {
        TickType_t timerPeriod = pdMS_TO_TICKS(timeouts_ms);

        char timerName[32];
        snprintf(timerName, sizeof(timerName), "PwrTimer_%u", (unsigned int)timerPeriod);

        createAndStartTimer(timerName, timerPeriod);
        timeouts_ms = 0;
    }
}

int cmd_wakeup_timer(int argc, char **argv)
{
    uint32_t timeouts_ms;

    if ((argc > 1) && (argv[1] != NULL)) {
        printf("%s\r\n", argv[1]);
        timeouts_ms = atoi(argv[1]);
    } else {
        printf("Need timeouts.\r\n");
        return -1;
    }

    app_pm_enter_pds15(timeouts_ms);

    return 0;
}




#define BUFFER_SIZE 1024

typedef struct {
    char server_ip[16];
    uint16_t server_port;
} server_info_t;

void tcp_client_task(void *pvParameters)
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    server_info_t *server_info = (server_info_t *) pvParameters;

    printf("create tcp client task.\r\n");

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Failed to create socket, errno: %d\n", errno);
        vPortFree(server_info);
        vTaskDelete(NULL);
        return;
    }

    // Set server address.
    printf("set server address.\r\n");
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_info->server_port);
    if (inet_aton(server_info->server_ip, &server_addr.sin_addr) == 0) {
        printf("Invalid server IP address\n");
        close(sock);
        vPortFree(server_info);
        vTaskDelete(NULL);
        return;
    }

    // Connect to the server.
    printf("start connect server.\r\n");
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect to server, errno: %d\n", errno);
        close(sock);
        vPortFree(server_info);
        vTaskDelete(NULL);
        return;
    }

    printf("Connected to server %s:%d\n", server_info->server_ip, server_info->server_port);

    while (1) {
        // Receive data sent by the server.
        bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            printf("Failed to receive data, errno: %d\n", errno);
            break;
        } else if (bytes_received == 0) {
            printf("Server closed connection\n");
            break;
        }

        // Process the received data.
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    close(sock);
    vPortFree(server_info);
    printf("Socket closed, task exiting\n");
    vTaskDelete(NULL);
}

void start_tcp_client_task(const char *ip, uint16_t port)
{
    printf("start_tcp_client_task.\r\n");
    server_info_t *server_info = (server_info_t *)pvPortMalloc(sizeof(server_info_t));
    if (server_info == NULL) {
        printf("Failed to allocate memory for server_info\n");
        return;
    }

    printf("copy ip.\r\n");

    strncpy(server_info->server_ip, ip, sizeof(server_info->server_ip) - 1);
    server_info->server_ip[sizeof(server_info->server_ip) - 1] = '\0';  // Ensure that the string is NULL-terminated
    server_info->server_port = port;

    printf("start create.\r\n");
    if (xTaskCreate(tcp_client_task, "tcp_client_task", 2048, server_info, 15, NULL) != pdPASS) {
        printf("Failed to create task\n");
        vPortFree(server_info);
    }
}

static void cmd_tcp_client(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0], atoi(argv[2]));
        return;
    }
    start_tcp_client_task(argv[1], (uint16_t)atoi(argv[2]));
}

SHELL_CMD_EXPORT_ALIAS(cmd_tcp_client, tcp_client, cmd tcp client);
SHELL_CMD_EXPORT_ALIAS(cmd_wakeup_timer, wakeup_timer, wakeup timer);

int ci_pm_test_init(void)
{
    return 0;
}
