
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

#define HOST_NAME "www.gov.cn"

// clang-format off
static const uint8_t get_buf[] = "GET / HTTP/1.1 \r\nHost: www.gov.cn\r\n\r\n";
uint32_t recv_buf[4 * 1024] = { 0 };
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

#define PING_USAGE                                \
    "wifi_http_test [hostname] [port]\r\n"        \
    "\t hostname: hostname or dest server ip\r\n" \
    "\t port: dest server listen port, defualt port:80\r\n"

static void wifi_test_http_client_init(int argc, char **argv)
{
    abort_exec = shell_signal(SHELL_SIGINT, test_close);
    printf("Http client task start ...\r\n");

    char *host_name;
    char *addr;
    char *port;
    struct sockaddr_in remote_addr;

    if (argc < 2) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get address (argv[1] if present) */
    host_name = argv[1];
#ifdef LWIP_DNS
    ip4_addr_t dns_ip;
    netconn_gethostbyname(host_name, &dns_ip);
    addr = ip_ntoa(&dns_ip);
#endif
    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "80";
    }

    while (1) {
        if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("Http Client create socket error\r\n");
            return;
        }
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(atoi(port));
        remote_addr.sin_addr.s_addr = inet_addr(addr);
        memset(&(remote_addr.sin_zero), 0, sizeof(remote_addr.sin_zero));

        printf("Host:%s, Server ip Address : %s:%s\r\n", host_name, addr, port);

        if (connect(sock_client, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) != 0) {
            printf("Http client connect server falied!\r\n");
            closesocket(sock_client);
            return;
        }

        printf("Http client connect server success!\r\n");
        printf("Press CTRL-C to exit.\r\n");
        memset(recv_buf, 0, sizeof(recv_buf));
        total_cnt = 0;
        write(sock_client, get_buf, sizeof(get_buf));
        while (1) {
            total_cnt = recv(sock_client, (uint8_t *)recv_buf, sizeof(recv_buf), 0);
            if (total_cnt <= 0)
                break;
            printf("%s\r\n", (uint8_t *)recv_buf);
            // vTaskDelay(10000);
        }
        closesocket(sock_client);
        return;
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_http_client(int argc, char **argv)
{
    wifi_test_http_client_init(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_http_client, wifi_http_test, wifi http client test);
#endif
