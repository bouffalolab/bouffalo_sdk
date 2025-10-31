/*
 * Copyright (c) 2025 Bouffalolab
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#include "src/headers/mlog.h"
#include "http/client.h"
#include "https_client.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

#define DEFAULT_HTTP_PORT  (80)
#define DEFAULT_HTTPS_PORT (443)

struct http_client {
    int                     socket;
    char                   *host;               /*!< Domain or IP as string */
    char                   *scheme;
    int                    port;                /*!< Port to connect, default depend on http_client_transport_t (80 or 443) */
    char                   *username;           /*!< Using for Http authentication */
    char                   *password;           /*!< Using for Http authentication */
    char                   *path;
    char                   *query;
};

static int connect_https(struct http_client *client,
                         const char *ca_pem, size_t ca_len,
                         const char *client_cert_pem, size_t client_cert_len,
                         const char *client_key_pem, size_t client_key_len)
{
    https_wrapper_handle_t https = NULL;
    http_wrapper_ssl_param_t ssl_param = {0};

    if (strcasecmp(client->scheme, "http") == 0) {
        https = https_wrapper_connect(client->host, client->port, NULL);
    } else if (strcasecmp(client->scheme, "https") == 0) {
        ssl_param.ca_cert = ca_pem;
        ssl_param.ca_cert_len = ca_len;

        ssl_param.own_cert = client_cert_pem;
        ssl_param.own_cert_len = client_cert_len;

        ssl_param.private_cert = client_key_pem;
        ssl_param.private_cert_len = client_key_len;

        https = https_wrapper_connect(client->host, client->port, &ssl_param);
    }
    return (int)https;
}

static void http_client_clean(struct http_client *client)
{
    if (client->path) {
        free(client->path);
        client->path = NULL;
    }
    if (client->username) {
        free(client->username);
        client->username = NULL;
    }
    if (client->password) {
        free(client->password);
        client->password = NULL;
    }
    if (client->scheme) {
        free(client->scheme);
        client->scheme = NULL;
    }
    if (client->host) {
        free(client->host);
        client->host = NULL;
    }
    if (client->query) {
        free(client->query);
        client->query = NULL;
    }
}

static int http_client_set_url(struct http_client *client, const char *url)
{
    int ret;
    struct http_parser_url purl;
    bool is_absolute_url;
    char *user_info = NULL;

    if (client == NULL || url == NULL) {
        LOG_ERR("client or url must not NULL\r\n");
        return -EINVAL;
    }

    http_parser_url_init(&purl);

    ret = http_parser_parse_url(url, strlen(url), 0, &purl);

    if (ret != 0) {
        LOG_ERR("Error parse url %s\r\n", url);
        return -EINVAL;
    }

    if (client->host) {
        free(client->host);
        client->host = NULL;
    }

    is_absolute_url = (bool)purl.field_data[UF_HOST].len;
    if (is_absolute_url) {

        client->host = malloc(purl.field_data[UF_HOST].len + 1);
        if (!client->host) {
            goto __fail1;
        }
        strncpy(client->host, url + purl.field_data[UF_HOST].off, purl.field_data[UF_HOST].len);
        client->host[purl.field_data[UF_HOST].len] = '\0';
        LOG_DBG("host:%s\r\n", client->host);
    }

    if (client->scheme) {
        free(client->scheme);
        client->scheme = NULL;
    }
    if (purl.field_data[UF_SCHEMA].len) {

        client->scheme = malloc(purl.field_data[UF_SCHEMA].len + 1);
        if (!client->scheme) {
            goto __fail2;
        }
        strncpy(client->scheme, url + purl.field_data[UF_SCHEMA].off, purl.field_data[UF_SCHEMA].len);
        client->scheme[purl.field_data[UF_SCHEMA].len] = '\0';

        if (strcasecmp(client->scheme, "http") == 0) {
            client->port = DEFAULT_HTTP_PORT;
        } else if (strcasecmp(client->scheme, "https") == 0) {
            client->port = DEFAULT_HTTPS_PORT;
        }
        LOG_DBG("scheme:%s\r\n", client->scheme);
    }

    if (purl.field_data[UF_PORT].len) {
        client->port = strtol((const char*)(url + purl.field_data[UF_PORT].off), NULL, 10);
    }

    if (purl.field_data[UF_USERINFO].len) {

        user_info = malloc(purl.field_data[UF_USERINFO].len + 1);
        if (!user_info) {
            goto __fail3;
        }
        strncpy(user_info, url + purl.field_data[UF_USERINFO].off, purl.field_data[UF_USERINFO].len);
        user_info[purl.field_data[UF_USERINFO].len] = '\0';

        char *username = user_info;
        char *password = strchr(user_info, ':');
        if (password) {
            *password = 0;
            password ++;
            client->password = strdup(password);
            if (!client->password) {
                goto __fail3;
            }
        }
        client->username = strdup(username);
        if (!client->username) {
            free(client->password);
            client->password = NULL;
            goto __fail3;
        }
        LOG_DBG("username:%s password:%s\r\n", username, password);
        free(user_info);
    } else if (is_absolute_url) {
        free(client->username);
        free(client->password);
        client->username = NULL;
        client->password = NULL;
    }

    if (purl.field_data[UF_PATH].len) {

        client->path = malloc(purl.field_data[UF_PATH].len + 1);
        if (!client->path) {
            goto __fail4;
        }
        strncpy(client->path, url + purl.field_data[UF_PATH].off, purl.field_data[UF_PATH].len);
        client->path[purl.field_data[UF_PATH].len] = '\0';
    } else {
        client->path = strdup("/");
    }
    LOG_DBG("path:%s", client->path);

    if (purl.field_data[UF_QUERY].len) {

        client->query = malloc(purl.field_data[UF_QUERY].len + 1);
        if (!client->query) {
            goto __fail5;
        }
        strncpy(client->query, url + purl.field_data[UF_QUERY].off, purl.field_data[UF_QUERY].len);
        client->query[purl.field_data[UF_QUERY].len] = '\0';
    } else if (client->query) {
        free(client->query);
        client->query = NULL;
    }

    return 0;

__fail5:
    free(client->path);
    client->path = NULL;
__fail4:
    free(client->username);
    free(client->password);
    client->username = NULL;
    client->password = NULL;
__fail3:
    free(client->scheme);
    client->scheme = NULL;
__fail2:
    free(client->host);
    client->host = NULL;
__fail1:
    LOG_ERR("ENOMEM\r\n");
    return -ENOMEM;
}

int https_client_request(const struct https_client_request *request, uint32_t timeout, void *user_data)
{
    int ret;
    int buffer_size;
    struct http_request req = {0};
    struct http_client client = {0};

    if (!request) {
        return -EINVAL;
    }
    memset(&req, 0, sizeof(req));

    ret = http_client_set_url(&client, request->url);
    if (ret < 0) {
        LOG_ERR("Failed to set URL: %d\r\n", ret);
        goto __end;
    }

    client.socket = connect_https(&client,
                                  request->ca_pem, request->ca_len,
                                  request->client_cert_pem, request->client_cert_len,
                                  request->client_key_pem, request->client_key_len);
    if (!client.socket) {
        ret = -ECONNREFUSED;
        LOG_ERR("https connect failed\r\n");
        goto __end;
    }

    buffer_size = (request->buffer_size) ? request->buffer_size : HTTP_CLIENT_DEFAULT_RX_BUFFER_SIZE;
    req.recv_buf = malloc(buffer_size);
    if (!req.recv_buf) {
        LOG_ERR("MEM ERR\r\n");
        ret = -ENOMEM;
        goto __end;
    }
    req.recv_buf_len = buffer_size;

    req.method = request->method;
    req.response = request->response;
    req.http_cb = request->http_cb;
    req.url = client.path;
    req.protocol = request->protocol;
    req.header_fields = request->header_fields;
    req.content_type_value = request->content_type_value;
    req.host = client.host;
    req.payload_cb = request->payload_cb;
    req.payload = request->payload;
    req.payload_len = request->payload_len;
    req.optional_headers_cb = request->optional_headers_cb;
    req.optional_headers = request->optional_headers;

    ret = http_client_req(client.socket, &req, timeout, user_data);
    if (ret < 0) {
        LOG_ERR("http_client_req fail ret:%d\r\n", ret);
    }

    https_wrapper_destroy((https_wrapper_handle_t)client.socket);

__end:
    if (req.recv_buf) {
        free(req.recv_buf);
    }
    http_client_clean(&client);

    return ret;
}

