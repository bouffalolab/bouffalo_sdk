
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "shell.h"
#include "utils_getopt.h"
#include "bflb_mtimer.h"

#define TCP_CLIENT_BLOCK_TIMEOUT 0
#define TCP_CLIENT_NOT_DEL_SELF  0

struct arg_param {
    int argc;
    char **argv;
};

// clang-format off
static const uint8_t write_buf[128] = "wifi tcp client test, helloworld!\r\n";
static const uint8_t read_buf[128];
// clang-format on

static volatile int wifi_tcp_client_exit;
static shell_sig_func_ptr abort_exec;
static uint64_t total_rx_cnt;
static uint64_t total_tx_cnt;

static void sig_close(int sig)
{
    wifi_tcp_client_exit = 1;

    if (abort_exec) {
        abort_exec(sig);
    }
}

#define PING_USAGE                    \
    "wifi_tcp_test [ip] [port]\r\n"   \
    "\t ip: dest ip or server ip\r\n" \
    "\t port: dest server listen port\r\n"

static void wifi_tcp_client_rx(void *sock_client)
{
    printf("tcp client rx task start ...\r\n");

    int sock = *(int*)sock_client;
    int ret;
    int timeout_cnt = 0;

    /* read */
    while (1) {
        ret = read(sock, read_buf, sizeof(read_buf));

        if (ret >= 0) {
            total_rx_cnt += ret;
            timeout_cnt = 0;
        } else if (ret == ERR_TIMEOUT) {
            if (++timeout_cnt > 3) {
                printf("read failed, timeout_cnt: %d\n\r", timeout_cnt);
                break;
            }
        } else {
            printf("read failed, ret: %d, errno: %d\n\r", ret, errno);
            break;
        }

        vTaskDelay(200);
    }

    /* wait to be deleted */
    #if TCP_CLIENT_NOT_DEL_SELF
    printf("tcp client rx task exiting...\r\n");
    while (1) {
        vTaskDelay(200);
    }
    #else
    vTaskDelete(NULL);
    #endif

    printf("tcp client rx task exit!\r\n");
}

static void wifi_tcp_client_tx(void *sock_client)
{
    printf("tcp client tx task start ...\r\n");

    int sock = *(int*)sock_client;
    int ret;
    int timeout_cnt = 0;

    /* write */
    while (1) {
        ret = write(sock, write_buf, sizeof(write_buf));

        if (ret >= 0) {
            total_tx_cnt += sizeof(write_buf);
            timeout_cnt = 0;
        } else if (ret == ERR_TIMEOUT) {
            if (++timeout_cnt > 3) {
                printf("write failed, timeout_cnt: %d\n\r", timeout_cnt);
                break;
            }
        } else {
            printf("write failed, ret: %d, errno: %d\n\r", ret, errno);
            break;
        }

        vTaskDelay(200);
    }

    /* wait to be deleted */
    #if TCP_CLIENT_NOT_DEL_SELF
    printf("tcp client tx task exiting...\r\n");
    while (1) {
        vTaskDelay(200);
    }
    #else
    vTaskDelete(NULL);
    #endif

    printf("tcp client tx task exit!\r\n");
}

static void wifi_tcp_client_init(void *input_arg)
{
    printf("tcp client task start ...\r\n");

    char *addr;
    char *port;
    int mode;
    int sock_client = -1;
    struct sockaddr_in remote_addr;
    TaskHandle_t px_tcpclient_rx_task = NULL;
    TaskHandle_t px_tcpclient_tx_task = NULL;
    struct arg_param* arg = (struct arg_param*)input_arg;

    /* check arg */
    if (arg->argc < 3) {
        printf("%s", PING_USAGE);
        goto __exit;
    }

    /* get address (argv[1] if present) */
    addr = arg->argv[1];
    /* get port number (argv[2] if present) */
    port = arg->argv[2];

    /* create socket */
    if ((sock_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("TCP Client create socket error\r\n");
        goto __exit;
    }

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(atoi(port));
    remote_addr.sin_addr.s_addr = inet_addr(addr);
    memset(&(remote_addr.sin_zero), 0, sizeof(remote_addr.sin_zero));
    printf("Server ip Address : %s:%s\r\n", addr, port);

    /* connect socket */
    if (connect(sock_client, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) != 0) {
        printf("TCP client connect server falied!\r\n");
        goto __exit;
    }
    printf("TCP client connect server success!\r\n");

    #if TCP_CLIENT_BLOCK_TIMEOUT
    /* blocking timeout */
    int timeout_ms = 4000;
    #if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
    int opt_on = timeout_ms;
    #else
    struct timeval opt_on = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms - (opt_on.tv_sec * 1000)) * 1000,
    };
    #endif
    setsockopt(sock_client, SOL_SOCKET, SO_SNDTIMEO, (void *)&opt_on, sizeof(opt_on));
    #endif

    total_rx_cnt = 0;
    total_tx_cnt = 0;
    wifi_tcp_client_exit = 0;
    abort_exec = shell_signal(SHELL_SIGINT, sig_close);
    printf("Press CTRL-C to exit before next Shell CMD.\r\n");

    /* fork recv and send task, dont care fail */
    xTaskCreate(wifi_tcp_client_rx, "tcp_client_rx", 512, (void *)&sock_client, 20, &px_tcpclient_rx_task);
    xTaskCreate(wifi_tcp_client_tx, "tcp_client_tx", 512, (void *)&sock_client, 20, &px_tcpclient_tx_task);

    while (!wifi_tcp_client_exit) {
        vTaskDelay(500);
    }

__exit:
    /* exit procedure */
    if (sock_client >= 0) {
        printf("closesocket!\r\n");
        closesocket(sock_client);
    }

    if (px_tcpclient_rx_task || px_tcpclient_tx_task) {
        vTaskDelay(500);
    }

    #if TCP_CLIENT_NOT_DEL_SELF
    if (px_tcpclient_rx_task) {
        vTaskDelete(px_tcpclient_rx_task);
    }
    if (px_tcpclient_tx_task) {
        vTaskDelete(px_tcpclient_tx_task);
    }
    #endif

    if (total_rx_cnt || total_tx_cnt) {
        printf("Total recv data=%lld\r\n", total_rx_cnt);
        printf("Total send data=%lld\r\n", total_tx_cnt);
    }

    printf("tcp_client exit!\r\n");
    vTaskDelete(NULL);
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_tcp_client(int argc, char **argv)
{
    struct arg_param arg = {argc, argv};

    if (pdPASS != xTaskCreate(wifi_tcp_client_init, "tcp_client", 512, (void *)&arg, 15, NULL)) {
        return -1;
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_tcp_client, wifi_tcp_test, wifi tcp test);
#endif
