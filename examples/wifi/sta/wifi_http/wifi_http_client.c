
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
#include "https_client.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif 

static int payload_cb(int sock, struct http_request *req, void *user_data)
{
    const char *content[] = {
        "foobar",
        "chunked",
        "last"
    };
    char tmp[64];
    int i, pos = 0;

    for (i = 0; i < ARRAY_SIZE(content); i++) {
        pos += snprintf(tmp + pos, sizeof(tmp) - pos,
                "%x\r\n%s\r\n",
                (unsigned int)strlen(content[i]),
                content[i]);
    }

    pos += snprintf(tmp + pos, sizeof(tmp) - pos, "0\r\n\r\n");

    (void)zsock_send(sock, tmp, pos, 0);

    return pos;
}

static int log_output(void *ptr, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++) {
        putchar(((char *)ptr)[i]);
    }
    return i;
}

static void response_cb(struct http_response *rsp,
            enum http_final_call final_data,
            void *user_data)
{
    log_output(rsp->recv_buf, rsp->data_len);
    if (final_data == HTTP_DATA_MORE) {
        //printf("Partial data received (%zd bytes)\r\n", rsp->data_len);
    } else if (final_data == HTTP_DATA_FINAL) {
        //printf("All the data received (%zd bytes)\r\n", rsp->data_len);
    }
}

#define PING_USAGE                                \
    "wifi_http_test [url]\r\n"        \
    "\t url: url or dest server ip\r\n" \

static void wifi_test_http_client_init(int argc, char **argv)
{
    int ret;
    char *url;
    struct https_client_request req;

    if (argc < 2) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get address (argv[1] if present) */
    url = argv[1];

    memset(&req, 0, sizeof(req));
    req.method = HTTP_GET;
    req.url = url;
    req.protocol = "HTTP/1.1";
    req.response = response_cb;

    ret = https_client_request(&req, 3*1000, "IPv4 GET");
    if (ret < 0) {
        printf("http_client get request fail ret:%d\r\n", ret);
    } else {
        printf("Http client GET request server success\r\n");
    }

    const char *headers[] = {
        "Transfer-Encoding: chunked\r\n",
        NULL
    };

    memset(&req, 0, sizeof(req));

    req.method = HTTP_POST;
    req.url = url;
    req.protocol = "HTTP/1.1";
    req.payload_cb = payload_cb;
    req.header_fields = headers;
    req.response = response_cb;

    ret = https_client_request(&req, 3*1000, "IPv4 POST");
    if (ret < 0) {
        printf("http_client post request fail ret:%d\r\n", ret);
    } else {
        printf("Http client POST request server success\r\n");
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
