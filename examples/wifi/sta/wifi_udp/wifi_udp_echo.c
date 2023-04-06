
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
uint32_t recv_buf[2 * 1024] = {0};
// clang-format on

shell_sig_func_ptr abort_exec;
uint64_t recv_len = 0;
int sock = -1;

static void test_close(int sig)
{
    if (sock) {
        closesocket(sock);
    }
    abort_exec(sig);
    if (recv_len > 0) {
        printf("Total send data=%lld\r\n", recv_len);
    }
}

#define PING_USAGE             \
    "wifi_udp_echo [port]\r\n" \
    "\t port: local listen port, default port 5001\r\n"

static void wifi_test_udp_echo_init(int argc, char **argv)
{
    abort_exec = shell_signal(SHELL_SIGINT, test_close);
    printf("udp server task start ...\r\n");

    char *port;
    struct sockaddr_in udp_addr, remote_addr;
    socklen_t addr_len;

    if (argc < 2) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get port number (argv[1] if present) */
    if (argc > 1) {
        port = argv[1];
    } else {
        port = "5001";
    }

    memset(&recv_buf[0], 0, sizeof(recv_buf));

    while (1) {
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            printf("udp create socket error\r\n");
            return;
        }
        udp_addr.sin_family = AF_INET;
        udp_addr.sin_port = htons(atoi(port));
        udp_addr.sin_addr.s_addr = INADDR_ANY;
        memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));

        printf("Server ip Address : %s:%s\r\n", udp_addr.sin_addr.s_addr, port);

        if (bind(sock, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) != 0) {
            printf("udp bind falied!\r\n");
            closesocket(sock);
            return;
        }

        printf("udp bind port success!\r\n");
        printf("Press CTRL-C to exit.\r\n");
        recv_len = 0;
        while (1) {
            recv_len = recvfrom(sock, recv_buf, 1024, 0, (struct sockaddr *)&remote_addr, &addr_len);
            printf("recv from %s\r\n", inet_ntoa(remote_addr.sin_addr));
            printf("recv:%s \r\n", recv_buf);
            sendto(sock, recv_buf, recv_len, 0, (struct sockaddr *)&remote_addr, addr_len);
        }
        closesocket(sock);
        return;
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_udp_echo(int argc, char **argv)
{
    wifi_test_udp_echo_init(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_udp_echo, wifi_udp_echo, wifi udp test);
#endif
