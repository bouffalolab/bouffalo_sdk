/**
 * @file http_server_restfulAPI.c
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "bflb_mtimer.h"
#include <stdbool.h>
#include <stdint.h>
#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"
// #include "lwip/inet.h"
// #include "lwip/netif.h"
#include "lwip/tcpbase.h"
#include "lwip/tcp.h"
#include "lwip/apps/fs.h"
#include "lwip/apps/httpd.h"

#include "FreeRTOS.h"
#include "mbedtls/platform.h"
#include "task.h"
#include "bflb_gpio.h"
#include "board.h"
#include "shell.h"
#include "log.h"

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/debug.h>
#include <mbedtls/x509.h>
#include <mbedtls/pk.h>

#include "cJSON.h"

#define USE_TLS 1

// 测试用自签名服务器证书（PEM格式）
const char server_cert_pem[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIID3zCCAsegAwIBAgIUJzKdov3PLxnEOowWmmMg2tc0hX0wDQYJKoZIhvcNAQEL\n"
    "BQAwZTELMAkGA1UEBhMCQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0Jl\n"
    "aWppbmcxEzARBgNVBAoMCk15IENvbXBhbnkxHTAbBgNVBAMMFFJFU1RmdWwgQVBJ\n"
    "IEhUVFAgS0VZMB4XDTI1MTExODAzMDc0NFoXDTI2MTExODAzMDc0NFowazELMAkG\n"
    "A1UEBhMCQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0JlaWppbmcxDzAN\n"
    "BgNVBAoMBk15IE9yZzEOMAwGA1UECwwFSFRUUFMxFzAVBgNVBAMMDjE5Mi4xNjgu\n"
    "MjguMTQxMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA5OTmoSDNXaUy\n"
    "i/Iu+gk+NEgiB+XNnvwSffplinRl5XfrDQ6Rj1PyQaw3V78VxPmHEzpdiK41XTxd\n"
    "f/gZs0Mz9JknPgXYT7Z7Rl4ZqU77XCFg8nz2JFnW69LqmwBOMQsNK3hsG3YXbZ9u\n"
    "O8bHa3/CGV29Q21AJk99dIYi+eNxIjuC4SxsIkDFtDEHWC6Dr36GZ9wUhvKF+wrh\n"
    "tw9rC+uZJImUh4FiFJtn3mjA+b8uSajrJSUqhx6llSb9uGBrjRbMKKgXyXeKZXMo\n"
    "0dNz7i/SYRm6VTeY2kRlW+MvWaz5ZJ3SOnSGh1f1z5d7KrjpYwnBXvv/kpE+2B+x\n"
    "KLh0wHUsiwIDAQABo4GAMH4wCQYDVR0TBAIwADALBgNVHQ8EBAMCBaAwEwYDVR0l\n"
    "BAwwCgYIKwYBBQUHAwEwDwYDVR0RBAgwBocEwKgcjTAdBgNVHQ4EFgQU1lkRJkK6\n"
    "YTjpbTY0GxA1g977EfAwHwYDVR0jBBgwFoAU1eibY3vTY3l/gCDgSWcoRGKNycsw\n"
    "DQYJKoZIhvcNAQELBQADggEBAE4SQhO/cDFJGTHhbXg8lrRVWdYZbtO5asNu82e5\n"
    "XzaMpJq9YL+WnIaRb/gPFZgjvqrcl9WUD3jyfv+L7jOHVJaAUQrj+CEBmCvrp7EW\n"
    "Q4xYAOEQytRmCgf+t2kOQlVzupuYQnE0YXsT/Fc6aRf7tjoVrmkNUkRdxNfpKc+5\n"
    "0HExsa31NAw20Y6KYaAXos9ng28d6hdgJe37aGn0fGdpLUUXpJKr7Y8PtOqXoVC9\n"
    "vo6cqa80iqZu8r25Cm8k2A0+CedEsdygBXqhBpceauUqG2LIpIBz5xuOKF6g8Wkg\n"
    "5LpF4i+zCUvkRVU7Px2QUANpmQumFJJ8zFNJk0ncOluaP7E=\n"
    "-----END CERTIFICATE-----\n";

const char server_private_key_pem[] =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDk5OahIM1dpTKL\n"
    "8i76CT40SCIH5c2e/BJ9+mWKdGXld+sNDpGPU/JBrDdXvxXE+YcTOl2IrjVdPF1/\n"
    "+BmzQzP0mSc+BdhPtntGXhmpTvtcIWDyfPYkWdbr0uqbAE4xCw0reGwbdhdtn247\n"
    "xsdrf8IZXb1DbUAmT310hiL543EiO4LhLGwiQMW0MQdYLoOvfoZn3BSG8oX7CuG3\n"
    "D2sL65kkiZSHgWIUm2feaMD5vy5JqOslJSqHHqWVJv24YGuNFswoqBfJd4plcyjR\n"
    "03PuL9JhGbpVN5jaRGVb4y9ZrPlkndI6dIaHV/XPl3squOljCcFe+/+SkT7YH7Eo\n"
    "uHTAdSyLAgMBAAECggEAaRnT7mN9+QRsP8c+G9BXsDD481BpSJmtMgxiSJ9m1GVq\n"
    "WA3692mP4OjfZpqWt+HtzH3tyd9YII4hzqtQvbfbaPJUCw4rA3sigq6dsNjT4Ttw\n"
    "ZRfM++cGoTUxRWfv/2P4MaAzX66rVkoxFyCQnANHUFdzGr1VCeJRFEqtT7nITnNP\n"
    "c66tDP+8DEIR6WXvfsUwBHD5F7i6uTsiK2fUW3ELzD7K9U720Y6+5poc5/kH9WP7\n"
    "9AaSbnrdx1It3ju7tyjBHfOz1sSovRHqZAkuW9DCQIB2GwMhuhMbk6eqG6O/DwdE\n"
    "eNNaZ0JmIEmj9IRGzshffs88v7nYslRRAc54I7RoAQKBgQD+V8ymUOJFJ7ZqBiPC\n"
    "ItCPk1PGSoXBfEu2HZNZDx30QkN5ts4HqARfsVHTXBdQpmgEeipCISI/sTFIqt+F\n"
    "eS2KcEXcIvNHgRzi1O1l5fcBIng9DD4RWCQfNmFeawkgEODTTOhFQwf+C24vVEk9\n"
    "HXKPyrJyxBy/T0wVN2I2G/xUiwKBgQDmYqg+JSPyhdEgRlva7KnkHiPmdeIj+ZPp\n"
    "nA/zhXH5R8aMKbt7DZCpFK2mAa7KiAtGCnsKysrlsOrji3i5TkmDIkTXvkgATPbt\n"
    "FbaYNoia20Ux5ktofVnKDAJedlei6LmCBYbRJGvB3AI6xE0l+muYvkscTxCMg8tm\n"
    "tEgX9Y2IAQKBgQDgi50mnDQXf8cA9aiuJVRh3VzRrEtIFbc18gh7UOhOFA5EL4rX\n"
    "oZMy9HZcFmqTDka45S3Xsi7tcj8je4/Y47G8DgDvJ4AGecuJyxhHpO4b91WV9+/4\n"
    "GgvR7o2p8O9eeugyRCyvsU0mEOtXJ599MDlLD1XaEp+amqR9+qZfRPfRnwKBgFYO\n"
    "RAnylZrhoVwM7AK6pDsxhvdu0Sne8AEy6kJsdkZHKULx07YDOyXtM2/u8Pt2Fb/z\n"
    "6kH9vgxOKW8VieKt+4egJVbxHLQAC7xDgBEbTauKfC5Hd6NwLznpgnkGe3W+uIx4\n"
    "xAMbN6USKUMKUEb/Y3GKo0PPxL63/CJA9Sz97AABAoGBALrvRz/QdDwF7pCR22Cu\n"
    "MJmPCt55mA2pI3meJdLkx/xbsGGxAlS7jnG3PZxUrqLBvR4wzGBUh/zaowLMfzqi\n"
    "VkopQ9gE50Pv544R3CeoPl5mZlXFwNPYlGQlWJsmlZr0Lacx6vU83t10slaF8p9o\n"
    "ePMYLVge8QbGnMPfA3082opp\n"
    "-----END PRIVATE KEY-----\n";

#include "http_server_restfulAPI.h"

// TLS服务器上下文结构体
struct tls_server_context_t {
    mbedtls_ssl_context ssl_ctx;
    mbedtls_net_context listen_fd;
    mbedtls_net_context client_fd;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt server_cert;
    mbedtls_pk_context server_key;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
};

int tls_http_server_handle_connection(struct tls_server_context_t *ctx);
int tls_http_server_init(struct tls_server_context_t *ctx);
const HtmlServerRouter_TypeDef httpRouter[] = {
    // 静态
    { HTTP_GET, "/", "/index.html", NULL },
    { HTTP_GET, "/index.html", "/index.html", NULL },
    { HTTP_GET, "/css/style.css", "/css/style.css", NULL },
    { HTTP_GET, "/js/main.js", "/js/main.js", NULL },
    { HTTP_UNKNOWN, "/404.html", NULL, NULL },

    // RESTful API
    { HTTP_GET, "/api/v1/gpios", NULL, get_gpios_handler },          // 获取所有GPIO
    { HTTP_POST, "/api/v1/gpios", NULL, post_gpio_handler },         // 创建GPIO
    { HTTP_PUT, "/api/v1/gpios/", NULL, put_gpio_handler },          // 更新GPIO
    { HTTP_DELETE, "/api/v1/gpios/", NULL, delete_gpio_handler },    // 删除GPIO
    {HTTP_PATCH, "/api/v1/gpios/", NULL, patch_gpio_handler }
};

extern struct bflb_device_s *gpio;
static RestfulResource_TypeDef gpio_resources[MAX_GPIO_RESOURCES] = {
    { .id = 0, .state = 0 },    //UNUSED
    { .id = GPIO_PIN_1, .state = 1 },
    { .id = GPIO_PIN_2, .state = 1 },
    { .id = GPIO_PIN_3, .state = 1 },
    { .id = GPIO_PIN_4, .state = 1 },
    { .id = GPIO_PIN_5, .state = 1 },
    { .id = GPIO_PIN_6, .state = 1 },
};

const char *not_found = "<html><body><h1>404 Not Found</h1></body></html>";
const char *internal_server_error = "<html><body><h1>500 Internal Server Error</h1></body></html>";
const char *bad_request = "<html><body><h1>400 Bad Request</h1></body></html>";
const char *unauthorized = "<html><body><h1>401 Unauthorized</h1></body></html>";
const char *forbidden = "<html><body><h1>403 Forbidden</h1></body></html>";


SemaphoreHandle_t console_lock = NULL;
sys_sem_t s_x_btn_semaphore = NULL;
char button_clicked_flag = false;

static struct netconn *http_active_conn = NULL;
static StackType_t http_server_stack[1024];
static StaticTask_t http_server_handle;

// TLS服务器上下文
struct tls_server_context_t tls_ctx;

void send_http_response(struct netconn *conn, int status_code,
                       const char *content_type, const char *body, size_t body_len)
{
    char header[256];
    size_t header_len;
    const char *status_line;

    switch(status_code) {
        case HTTP_OK: status_line = "HTTP/1.1 200 OK"; break;
        case HTTP_CREATED: status_line = "HTTP/1.1 201 Created"; break;
        case HTTP_BAD_REQUEST: status_line = "HTTP/1.1 400 Bad Request"; break;
        case HTTP_UNAUTHORIZED: status_line = "HTTP/1.1 401 Unauthorized"; break;
        case HTTP_NOT_FOUND: status_line = "HTTP/1.1 404 Not Found"; break;
        case HTTP_INTERNAL_SERVER_ERROR: status_line = "HTTP/1.1 500 Internal Server Error"; break;
        default:  status_line = "HTTP/1.1 500 Internal Server Error"; break;
    }

    // HTTP响应头
    header_len = snprintf(header, sizeof(header),
        "%s\r\n"
        "Connection: close\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n"
        "Server: BouffaloHTTP/1.0\r\n"
        "\r\n",
        status_line, content_type, (unsigned long)body_len);

    // 发送头部
#ifndef HTTP_USE_MBEDTLS
    netconn_write(conn, header, header_len, NETCONN_COPY);
    if (body && body_len > 0) {
        netconn_write(conn, body, body_len, NETCONN_NOCOPY);
    }
#else
    mbedtls_ssl_write(&tls_ctx.ssl_ctx, (const unsigned char *)header, header_len);
    if (body && body_len > 0) {
        mbedtls_ssl_write(&tls_ctx.ssl_ctx, (const unsigned char *)body, body_len);
    }
#endif
}

void send_file_with_status(struct netconn *conn, const char *path,int status_code)
{
    struct fs_file file;
    err_t error = ERR_OK;

    error = fs_open(&file, path);

    if (error == ERR_OK) {
        // 根据文件扩展名确定MIME类型
        const char *content_type = "text/html"; // 默认
        if (strstr(path, ".json")) {
            content_type = "application/json";
        } else if (strstr(path, ".css")) {
            content_type = "text/css";
        } else if (strstr(path, ".js")) {
            content_type = "application/javascript";
        }

        // 发送完整的HTTP响应
        send_http_response(conn, status_code, content_type, 
                          (const char*)file.data, file.len);
        fs_close(&file);
    } else {
        // 文件不存在，发送404

        send_http_response(conn, HTTP_NOT_FOUND,
                          "text/html", not_found, strlen(not_found));
        OS_MSG("[ERROR] File not found: %s\n", path);
    }
}

void router_match(struct netconn *conn, const char *url,HtmlRequestMethod_TypeDef method,const char *body)
{
    static uint16_t router_length = sizeof(httpRouter) / sizeof(HtmlServerRouter_TypeDef);
    uint16_t i = 0;
    bool found = false;
    for (i = 0; i < router_length; i++) {
        bool method_match = (httpRouter[i].method == method);
        if (!method_match) {
            continue;
        }

        bool url_match = false;
        size_t route_len = strlen(httpRouter[i].url);
        size_t url_len = strlen(url);
        //特殊处理
        if (i == 0 ) {
            if (strcmp(httpRouter[i].url, url) == 0) {
                url_match = true;
            }
            else {
                continue;
            }
        }

        // end with '/' means prefix match
        if (httpRouter[i].url[route_len - 1] == '/') {
            // 前缀匹配
            if (url_len >= route_len && strncmp(httpRouter[i].url, url, route_len) == 0) {
                url_match = true;
            }
        } else {
            // 完全匹配
            if (strcmp(httpRouter[i].url, url) == 0) {
                url_match = true;
            }
        }

        if (url_match) {
            OS_MSG("[INFO] Found path %s, handling request.\r\n", url);
            found = true;
            if (httpRouter[i].handler != NULL) {
                httpRouter[i].handler(conn, url, body);       //动态调函数
            } else {
                send_file_with_status(conn, httpRouter[i].path, HTTP_OK);       //静态调文件
            }
            break;
        }
    }

    // 发送404页面
    if (!found) {
        OS_MSG("[WARNING] Not found path %s, sending 404 page.\r\n", url);
        send_file_with_status(conn, httpRouter[router_length - 1].path, HTTP_NOT_FOUND);
    }
}



void process_http_request(const char *request, uint16_t length, struct netconn *conn)
{
    char *mem = NULL;
    char *str = NULL;
    char *body = NULL;
    char *url = NULL;
    char *saveptr;
    int error_code = 0;
    OS_MSG("\r\n----------------Recevie request:---------------\r\n %s\r\n-----------------------------------------------\r\n",request);
    mem = malloc(length + 1);  // +1 for null terminator
    if (!mem) {
        error_code = HTTP_INTERNAL_SERVER_ERROR;
        goto error;
    }

    memset(mem, 0, length + 1);
    memcpy(mem, request, length);

    // 简化处理，先处理第一行
    char *first_line = strtok_r(mem, "\r\n", &saveptr);
    if (!first_line) {
        free(mem);
        error_code = HTTP_BAD_REQUEST;
        goto error;
    }

    // 解析方法
    char *method_str = strtok_r(first_line, " ", &saveptr);
    char *url_str = strtok_r(NULL, " ", &saveptr);

    if (!method_str || !url_str) {
        free(mem);
        error_code = HTTP_BAD_REQUEST;
        goto error;
    }

    // 复制URL
    url = strdup(url_str);

    // 查找消息体
    body = strstr(request, "\r\n\r\n");
    if (body) {
        body += 4; // 跳过\r\n\r\n
    }

    // 处理请求
    HtmlRequestMethod_TypeDef method = HTTP_UNKNOWN;
    if (strcmp(method_str, "GET") == 0) {
        method = HTTP_GET;
    } else if (strcmp(method_str, "POST") == 0) {
        method = HTTP_POST;
    } else if (strcmp(method_str, "PUT") == 0) {
        method = HTTP_PUT;
    } else if (strcmp(method_str, "PATCH") == 0) {
        method = HTTP_PATCH;
    } else if (strcmp(method_str, "DELETE") == 0) {
        method = HTTP_DELETE;
    } else {
        free(url);
        free(mem);
        goto error;
    }
    router_match(conn, url,method, body);
    free(url);
    free(mem);
    error:
        if (error_code != 0) {
            const char *error_body = NULL;
            switch (error_code) {
                case HTTP_BAD_REQUEST:
                    error_body = bad_request;
                    break;
                case HTTP_UNAUTHORIZED:
                    error_body = unauthorized;
                    break;
                case HTTP_INTERNAL_SERVER_ERROR:
                default:
                    error_body = internal_server_error;
                    break;
            }
            send_http_response(conn, error_code,
                            "text/html", error_body, strlen(error_body));
        }
}

void http_server_process(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    uint16_t buflen;
    printf("http_server_process\r\n");
    if (netconn_recv(conn, &inbuf) == ERR_OK) {
        if (netconn_err(conn) == ERR_OK) {
            netbuf_data(inbuf, (void **)&buf, &buflen);
            process_http_request(buf, buflen, conn);
        }
        netconn_close(conn);
        netbuf_delete(inbuf);
    }
}

void http_server_thread(void *arg)
{
    LWIP_UNUSED_ARG(arg);
#ifdef  HTTP_USE_MBEDTLS
    tls_http_server_init(&tls_ctx);
    /* bind to port and start listening port, all in mbedtls_net_bind*/
    OS_MSG("http server listen :%d\r\n", HTTPS_SERVER_PORT);
    if (mbedtls_net_bind(&tls_ctx.listen_fd, NULL, "443",MBEDTLS_NET_PROTO_TCP) == ERR_OK) {
        while (1) {
            if (mbedtls_net_accept(&tls_ctx.listen_fd,
                                &tls_ctx.client_fd,
                                NULL,
                                0,
                                NULL) == ERR_OK) {
                OS_MSG("Connected from client\r\n");
                tls_http_server_handle_connection(&tls_ctx);
                // 数据已在 tls_http_server_handle_connection 中通过 TLS 发送
                mbedtls_net_free(&tls_ctx.client_fd);
            }
        }
    }
#else
    struct netconn *conn, *newconn;
    /* create a new tcp connection handle */
    conn = netconn_new(NETCONN_TCP);
    if (conn != NULL) {
        /* bind to port based on protocol (HTTP: 80, HTTPS: 443) */
        if (netconn_bind(conn, IP4_ADDR_ANY, HTTP_SERVER_PORT) == ERR_OK) {
            /* start listening port */
            netconn_listen(conn);
            OS_MSG("http server listen :%d\r\n", HTTP_SERVER_PORT);

            while (1) {
                if (netconn_accept(conn, &newconn) == ERR_OK) {
                    if (newconn == NULL) {
                        OS_MSG("[ERROR]");
                    }

                    http_active_conn = newconn;
                    http_server_process(newconn);

                    http_active_conn = NULL;
                    netconn_delete(newconn);
                }
            }
        }
    }
#endif
}

void restfulAPI_http_server_init(void)
{
    vSemaphoreCreateBinary(console_lock);

    // sys_thread_new("http_server", http_server_thread, NULL, 512, 2);
    // restful_resources_init();
    OS_MSG("Initialized resources\r\n");
    xTaskCreateStatic((void *)http_server_thread, (char *)"http_server", sizeof(http_server_stack) / 4, NULL, osPriorityAboveNormal, http_server_stack, &http_server_handle);
    OS_MSG("Started HTTP server task\r\n");
}


void get_gpios_handler(struct netconn *conn, const char *url, const char *body)
{
    OS_MSG("[DEBUG] get_gpios_handler called with URL: %s\n", url);
    cJSON *root = cJSON_CreateObject();

    // 创建 GPIO 数组
    cJSON *gpio_array = cJSON_CreateArray();

    for (int i = 1; i <= 6; i++) {
        if (gpio_resources[i].id != 0) {
            // 创建一个对象来存储每个 GPIO 的信息
            cJSON *gpio_item = cJSON_CreateObject();
            char str[16] = {0};

            snprintf(str, sizeof(str), "%d", gpio_resources[i].id);
            cJSON_AddStringToObject(gpio_item, "GPIO_ID", str);

            snprintf(str, sizeof(str), "%d", gpio_resources[i].state);
            cJSON_AddStringToObject(gpio_item, "GPIO_STATE", str);

            // 将 GPIO 对象添加到数组中
            cJSON_AddItemToArray(gpio_array, gpio_item);
        }
    }

    // 将数组添加到根对象
    cJSON_AddItemToObject(root, "gpios", gpio_array);

    char *json_string = cJSON_Print(root);
    send_http_response(conn, HTTP_OK, "application/json", json_string, strlen(json_string));

    cJSON_free(json_string);
    cJSON_Delete(root);
}

void post_gpio_handler(struct netconn *conn, const char *url, const char *body)
{
    OS_MSG("[DEBUG] post_gpio_handler called with URL: %s\n", url);
    OS_MSG("[DEBUG] body: %s\n", body ? body : "NULL");

    cJSON *response = cJSON_CreateObject();

    if (!body || strlen(body) == 0) {
        cJSON_AddStringToObject(response, "error", "Request body is required");
        char *json_string = cJSON_PrintUnformatted(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          json_string, strlen(json_string));
        free(json_string);
        cJSON_Delete(response);
        return;
    }

    // 解析JSON参数
    cJSON *json = cJSON_Parse(body);
    cJSON *pin_obj = cJSON_GetObjectItem(json, "pin");
    cJSON *value_obj = cJSON_GetObjectItem(json, "value");
    if (!json || !pin_obj || !value_obj) {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"Invalid JSON\"}", 25);
        return;
    }
    int pin = (int)cJSON_GetNumberValue(pin_obj);
    int value = (int)cJSON_GetNumberValue(value_obj);
    if (pin < 1 || pin > 6) {
        cJSON_Delete(json);
        cJSON_Delete(response);
        OS_MSG("[DEBUG] param error: pin=%d, value=%d\n", pin, value);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"Invalid pin number (1-6)\"}", 41);
        return;
    }

    //操作
    int code = -1;
    OS_MSG("[DEBUG] Creating GPIO: pin=%d, value=%d\n", pin, value);
    if (gpio_resources[pin].id) {
        OS_MSG("[DEBUG] GPIO already exist! pin=%d, value=%d\n", gpio_resources[pin].id, gpio_resources[pin].state);
        cJSON_AddStringToObject(response, "status", "fail");
        cJSON_AddStringToObject(response, "message", "GPIO created failed");
        code = HTTP_OK;
    }else {
        gpio_resources[pin].id = pin;
        gpio_resources[pin].state = value;
        cJSON_AddStringToObject(response, "status", "success");
        cJSON_AddStringToObject(response, "message", "GPIO created successfully");
        code = HTTP_CREATED;
    }
    cJSON_AddNumberToObject(response, "pin", pin);
    cJSON_AddNumberToObject(response, "value", value);
    char *json_string = cJSON_PrintUnformatted(response);
    send_http_response(conn, code, "application/json",
                      json_string, strlen(json_string));

    free(json_string);
    cJSON_Delete(json);
    cJSON_Delete(response);
}

void put_gpio_handler(struct netconn *conn, const char *url, const char *body)
{
    OS_MSG("[DEBUG] put_gpio_handler called with URL: %s\n", url);
    OS_MSG("[DEBUG] body: %s\n", body ? body : "NULL");

    cJSON *response = cJSON_CreateObject();

    // 提取ID
    int pin = -1;
    const char *id_start = strstr(url, "/api/v1/gpios/");
    if (id_start != NULL) {
        id_start += strlen("/api/v1/gpios/");
        pin = atoi(id_start);
        OS_MSG("[DEBUG] Extracted pin ID: %d\n", pin);
    }else {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"GPIO ID is required in URL\"}", 40);
        return;
    }

    if (!body || strlen(body) == 0) {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"Request body is required\"}", 35);
        return;
    }

    // 解析JSON
    cJSON *json = cJSON_Parse(body);
    cJSON *value_obj = cJSON_GetObjectItem(json, "value");
    if (!json || !value_obj) {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"Invalid JSON\"}", 25);
        return;
    }
    int value = (int)cJSON_GetNumberValue(value_obj);
    if (pin < 1 || pin > 6 || value < 0 || value > 1) {
        cJSON_Delete(json);
        cJSON_Delete(response);
        OS_MSG("[DEBUG] Param error : pin=%d, value=%d\n", pin, value);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"Invalid pin number (1-6)\"}", 41);
        return;
    }

    //操作
    int code = -1;
    OS_MSG("[DEBUG] Updating GPIO: pin=%d, value=%d\n", pin, value);
    if (!gpio_resources[pin].id) {
        OS_MSG("[DEBUG] GPIO do not exist! \n");
        cJSON_AddStringToObject(response, "status", "fail");
        cJSON_AddStringToObject(response, "message", "GPIO created failed");
        code = HTTP_NOT_FOUND;
    }else {
        gpio_resources[pin].state = value;
        cJSON_AddStringToObject(response, "status", "success");
        cJSON_AddStringToObject(response, "message", "GPIO updated successfully");
        code = HTTP_OK;
    }

    cJSON_AddNumberToObject(response, "pin", pin);
    cJSON_AddNumberToObject(response, "value", value);
    char *json_string = cJSON_PrintUnformatted(response);
    send_http_response(conn, code, "application/json",
                      json_string, strlen(json_string));

    free(json_string);
    cJSON_Delete(json);
    cJSON_Delete(response);
}

void patch_gpio_handler(struct netconn *conn, const char *url, const char *body)
{
    OS_MSG("[DEBUG] get_gpios_handler called with URL: %s\n", url);
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", "JACK");
    cJSON_AddNumberToObject(root, "age", 25);
    cJSON_AddBoolToObject(root, "married", false);
    char *json_str = cJSON_Print(root);

    OS_MSG("[DEBUG] request body is null, return test json %s", json_str);
    send_http_response(conn, HTTP_OK, "application/json", "", strlen(json_str));

    // 释放内存
    cJSON_Delete(root);
    free(json_str);
}

void delete_gpio_handler(struct netconn *conn, const char *url, const char *body)
{
    OS_MSG("[DEBUG] delete_gpio_handler called with URL: %s\n", url);
    OS_MSG("[DEBUG] body: %s\n", body ? body : "NULL");

    cJSON *response = cJSON_CreateObject();

    // 提取ID
    int pin = -1;
    const char *id_start = strstr(url, "/api/v1/gpios/");
    if (id_start != NULL) {
        id_start += strlen("/api/v1/gpios/");
        pin = atoi(id_start);
        OS_MSG("[DEBUG] Extracted pin ID: %d\n", pin);
    }else {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"GPIO ID is required in URL\"}", 40);
        return;
    }

    if (pin < 0) {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"GPIO ID is required in URL\"}", 40);
        return;
    }
    OS_MSG("[DEBUG] Deleting GPIO: pin=%d\n", pin);

    // 验证参数
    if (pin < 1 || pin > 6) {
        cJSON_Delete(response);
        send_http_response(conn, HTTP_BAD_REQUEST, "application/json",
                          "{\"error\":\"Invalid pin number (1-6)\"}", 41);
        return;
    }
    gpio_resources[pin].id = 0;
    gpio_resources[pin].state = 0;

    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "message", "GPIO deleted successfully");
    cJSON_AddNumberToObject(response, "pin", pin);

    char *json_string = cJSON_PrintUnformatted(response);
    send_http_response(conn, HTTP_OK, "application/json",json_string, strlen(json_string));

    free(json_string);
    cJSON_Delete(response);
}


// void restful_resources_init(void)
// {
//     gpio = bflb_device_get_by_name("gpio");
//     bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
//     bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
//     bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
// }




void http_resetful_app_init(void)
{
    LOG_I("HTTP RESTful API example\r\n");
    restfulAPI_http_server_init();
}

SHELL_CMD_EXPORT_ALIAS(http_resetful_app_init, http_test, wifi http test);

#ifdef HTTP_USE_MBEDTLS
void my_mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str){
    printf("%s:%04d: %s", file, line, str);
}
int tls_http_server_init(struct tls_server_context_t *ctx)
{
    OS_MSG("Start Server-Tls init.\r\n");
    const unsigned char *additional = (const unsigned char *)"H";
    size_t additional_len = 1;
    int rv;
    char buf[50];
    mbedtls_ssl_config *ssl_conf = &ctx->ssl_conf;
    mbedtls_x509_crt *server_cert = &ctx->server_cert;
    mbedtls_pk_context *server_key = &ctx->server_key;
    mbedtls_entropy_context *entropy = &ctx->entropy;
    mbedtls_ctr_drbg_context *ctr_drbg = &ctx->ctr_drbg;
    mbedtls_net_context *listen_fd = &ctx->listen_fd;
    mbedtls_ssl_conf_dbg(ssl_conf, my_mbedtls_debug, stdout);
    mbedtls_debug_set_threshold(4);


    /* 随机数生成器初始化部分 */
    mbedtls_entropy_init(entropy);
    mbedtls_ctr_drbg_init(ctr_drbg);
    rv = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
                                additional, additional_len);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ctr_drbg_seed failed with %x (%s)\n", -rv, buf);
        return -1;
    }
    /* 初始化并解析服务器证书 */
    mbedtls_x509_crt_init(server_cert);
    rv = mbedtls_x509_crt_parse(server_cert,
                                 (unsigned char *)server_cert_pem,
                                 strlen(server_cert_pem)+1);
    if (rv < 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_x509_crt_parse failed with %x (%s)\n", -rv, buf);
        mbedtls_x509_crt_free(server_cert);
        return -1;
    }
    /* 初始化并解析私钥 */
    mbedtls_pk_init(server_key);
    rv = mbedtls_pk_parse_key(server_key,
                               (const unsigned char *)server_private_key_pem,
                               strlen(server_private_key_pem)+1,
                               NULL, 0);
    if (rv < 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_pk_parse_key failed with %x (%s)\n", -rv, buf);
        mbedtls_pk_free(server_key);
        mbedtls_x509_crt_free(server_cert);
        return -1;
    }
    /* 私钥与证书匹配 */
    if (mbedtls_pk_check_pair(&server_cert->pk, server_key) != 0) {
        printf("Private key does not match certificate\n");
        mbedtls_pk_free(server_key);
        mbedtls_x509_crt_free(server_cert);
        return -1;
    }
    /* 初始化SSL/TLS配置 - 使用服务器模式 */
    mbedtls_ssl_config_init(ssl_conf);
    rv = mbedtls_ssl_config_defaults(ssl_conf,
                                     MBEDTLS_SSL_IS_SERVER,
                                     MBEDTLS_SSL_TRANSPORT_STREAM,
                                     MBEDTLS_SSL_PRESET_DEFAULT);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_config_defaults failed with %x (%s)\n", -rv, buf);
        mbedtls_ssl_config_free(ssl_conf);
        mbedtls_pk_free(server_key);
        mbedtls_x509_crt_free(server_cert);
        return -1;
    }

    mbedtls_net_init(listen_fd);

    /* 挂载 随机数生成器 证书私钥 */
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);
    mbedtls_ssl_conf_own_cert(ssl_conf, server_cert, server_key);
    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_NONE);

    printf("TLS server initialized successfully\n");
    return 0;
}

int tls_http_server_handle_connection(struct tls_server_context_t *ctx)
{
    int rv;
    char buf[50];
    mbedtls_ssl_context ssl_ctx;

    /* 初始化 SSL 上下文 */
    mbedtls_ssl_init(&ssl_ctx);


    /* 设置 SSL 配置 */
    rv = mbedtls_ssl_setup(&ssl_ctx, &ctx->ssl_conf);
    if (rv != 0) {
        mbedtls_strerror(rv, buf, sizeof(buf));
        printf("mbedtls_ssl_setup failed with %x (%s)\n", -rv, buf);
        mbedtls_net_free(&ctx->listen_fd);
        return -1;
    }


    mbedtls_ssl_set_bio(&ssl_ctx, &ctx->client_fd,
                        mbedtls_net_send,
                        mbedtls_net_recv,
                        NULL);
    while ((rv = mbedtls_ssl_handshake(&ssl_ctx)) != 0) {
        if (rv != MBEDTLS_ERR_SSL_WANT_READ && rv != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_strerror(rv, buf, sizeof(buf));
            printf("mbedtls_ssl_handshake failed with %x (%s)\n", -rv, buf);
            mbedtls_ssl_free(&ssl_ctx);
            mbedtls_net_free(&ctx->client_fd);
            return -1;
        }
    }

    printf("TLS handshake completed successfully\n");
    ctx->ssl_ctx = ssl_ctx;
    unsigned char request[1024];
    int len = mbedtls_ssl_read(&ssl_ctx, request, sizeof(request) - 1);
    process_http_request((const char *)request,len,NULL);

    mbedtls_ssl_free(&ssl_ctx);
    return 0;
}
#endif