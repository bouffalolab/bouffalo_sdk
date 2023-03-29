
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

// clang-format off
static const uint8_t send_buf[128] = "wifi tcp client test, helloworld!\r\n";
// clang-format on

shell_sig_func_ptr abort_exec;
uint64_t total_cnt = 0;
int sock_client = -1;

static void test_close(int sig)
{
    if (sock_client) {
        closesocket(sock_client);
    }
    abort_exec(sig);
    if (total_cnt > 0) {
        printf("Total send data=%lld\r\n", total_cnt);
    }
}

#define PING_USAGE                    \
    "wifi_tcp_test [ip] [port]\r\n"   \
    "\t ip: dest ip or server ip\r\n" \
    "\t port: dest server listen port\r\n"

static void wifi_test_tcp_client_init(int argc, char **argv)
{
    abort_exec = shell_signal(SHELL_SIGINT, test_close);
    printf("tcp client task start ...\r\n");

    char *addr;
    char *port;
    struct sockaddr_in remote_addr;

    if (argc < 3) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get address (argv[1] if present) */
    addr = argv[1];

    /* get port number (argv[2] if present) */
    port = argv[2];

    while (1) {
        if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("TCP Client create socket error\r\n");
            return;
        }
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(atoi(port));
        remote_addr.sin_addr.s_addr = inet_addr(addr);
        memset(&(remote_addr.sin_zero), 0, sizeof(remote_addr.sin_zero));

        printf("Server ip Address : %s:%s\r\n", addr, port);

        if (connect(sock_client, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) != 0) {
            printf("TCP client connect server falied!\r\n");
            closesocket(sock_client);
            return;
        }

        printf("TCP client connect server success!\r\n");
        printf("Press CTRL-C to exit.\r\n");
        total_cnt = 0;
        while (1) {
            if (write(sock_client, send_buf, sizeof(send_buf)) < 0) {
                printf("write falied!\r\n");
                break;
            }
            total_cnt += sizeof(send_buf);
            vTaskDelay(500);
        }
        closesocket(sock_client);
        return;
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_tcp_client(int argc, char **argv)
{
    wifi_test_tcp_client_init(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_tcp_client, wifi_tcp_test, wifi tcp test);
#endif
