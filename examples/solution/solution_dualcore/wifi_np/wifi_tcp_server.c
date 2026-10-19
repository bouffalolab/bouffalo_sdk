

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
// clang-format on

#define RECV_DATA_LEN (1024)
#define PING_USAGE                  \
    "wifi_tcp_echo_test [port]\r\n" \
    "\t port: local server listen port\r\n"

shell_sig_func_ptr abort_exec;
uint64_t recv_data_cnt = 0;
int sock_server = -1, connected;
char *recv_data;

static void test_close(int sig)
{
    if (connected >= 0) {
        closesocket(connected);
        connected = -1;
    }
    if (sock_server >= 0) {
        closesocket(sock_server);
    }
    if (recv_data) {
        free(recv_data);
    }
    abort_exec(sig);
    if (recv_data_cnt > 0) {
        printf("recv data=%lld\r\n", recv_data_cnt);
    }
}

static void wifi_test_tcp_server_init(int argc, char **argv)
{
    abort_exec = shell_signal(SHELL_SIGINT, test_close);
    printf("tcp server task start ...\r\n");

    char *port;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;

    if (argc < 2) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get listen port (argv[1] if present) */
    port = argv[1];

    recv_data = (char *)pvPortMalloc(RECV_DATA_LEN);
    if (recv_data == NULL) {
        printf("not memory\r\n");
        return;
    }
    /* create socket */
    if ((sock_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("TCP server create socket error\r\n");
        return;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* bind socket */
    if (bind(sock_server, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("TCP server socket bind fail!\r\n");
        return;
    }

    /* listen port */
    if (listen(sock_server, 5) == -1) {
        printf("TCP server listen error!\r\n ");
        return;
    }

    printf("TCP Server listen start: localhost:%s\r\n", port);
    printf("please connet ...\r\n");

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        connected = accept(sock_server, (struct sockaddr *)&client_addr, &sin_size);
        printf("new client connected from (%s, %d)\r\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int flag = 1;
        setsockopt(connected, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));

        printf("Press CTRL-C to exit.\r\n");
        recv_data_cnt = 0;
        while (1) {
            recv_data_cnt = recv(connected, recv_data, RECV_DATA_LEN, 0);
            if (recv_data_cnt <= 0) {
                break;
            }
            printf("recv %lld len data\r\n", recv_data_cnt);
            if (write(connected, recv_data, recv_data_cnt) < 0) {
                printf("write falied!\r\n");
                break;
            }
            vTaskDelay(500);
        }
        if (connected >= 0) {
            closesocket(connected);
            connected = -1;
        }
        if (sock_server >= 0) {
            closesocket(sock_server);
        }
        if (recv_data) {
            free(recv_data);
        }

        return;
    }
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_tcp_server(int argc, char **argv)
{
    wifi_test_tcp_server_init(argc, argv);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_tcp_server, wifi_tcp_echo_test, wifi tcp test);
#endif
