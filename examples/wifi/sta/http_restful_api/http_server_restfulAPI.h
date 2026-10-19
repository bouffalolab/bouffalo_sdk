/**
 * @file http_server.h
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

#ifndef __HTTP_SERVER__H__
#define __HTTP_SERVER__H__

#include <stdint.h>
#define HTTP_SERVER_PORT 80
#define HTTPS_SERVER_PORT 443

// extern SemaphoreHandle_t console_lock;
// extern sys_sem_t s_x_btn_semaphore;
extern char button_clicked_flag;

#define OS_MSG(fmt, ...)                                             \
    do {                                                             \
        if (xSemaphoreTake(console_lock, portMAX_DELAY) == pdTRUE) { \
            printf(fmt, ##__VA_ARGS__);                              \
            xSemaphoreGive(console_lock);                            \
        }                                                            \
    } while (0);

typedef enum {
    HTTP_POST,              //创建
    HTTP_GET,               //读取
    HTTP_PUT,               //更新
    HTTP_PATCH,             //部分更新
    HTTP_DELETE,            //删除
    HTTP_UNKNOWN,
} HtmlRequestMethod_TypeDef;

#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_INTERNAL_SERVER_ERROR 500
#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_CREATED 201
#define HTTP_NO_CONTENT 204
#define HTTP_CONFILCT 409

typedef struct {
    HtmlRequestMethod_TypeDef method;
    const char *url;
    const char *path;
    void(*handler)(struct netconn *conn, const char *url, const char *body);
} HtmlServerRouter_TypeDef;


#define MAX_GPIO_RESOURCES 7
typedef struct {
    uint8_t id;
    uint8_t state;
} RestfulResource_TypeDef;

#define  HTTP_USE_MBEDTLS
void restful_resources_init(void);

void restfulAPI_http_server_init(void);

void get_gpios_handler(struct netconn *conn, const char *url, const char *body);
void delete_gpio_handler(struct netconn *conn, const char *url, const char *body);
void put_gpio_handler(struct netconn *conn, const char *url, const char *body);
void post_gpio_handler(struct netconn *conn, const char *url, const char *body);
void patch_gpio_handler(struct netconn *conn, const char *url, const char *body);
#endif