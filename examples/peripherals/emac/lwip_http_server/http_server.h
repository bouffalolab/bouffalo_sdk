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

#define HTTP_SERVER_PORT 80

extern SemaphoreHandle_t console_lock;
extern sys_sem_t s_x_btn_semaphore;
extern char button_clicked_flag;

#define OS_MSG(fmt, ...)                                             \
    do {                                                             \
        if (xSemaphoreTake(console_lock, portMAX_DELAY) == pdTRUE) { \
            printf(fmt, ##__VA_ARGS__);                              \
            xSemaphoreGive(console_lock);                            \
        }                                                            \
    } while (0);

typedef enum {
    HTTP_POST,
    HTTP_GET,
    HTTP_UNKNOWN,
} HtmlRequestMethod_TypeDef;

typedef struct {
    const char *url;
    const char *path;
} HtmlServerRouter_TypeDef;

void http_server_init(void);

void http_server_netconn_init(void);

#endif