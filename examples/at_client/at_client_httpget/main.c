/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
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
#include "bflb_platform.h"
#include "at.h"

#define UART_RECV_BUF_SZIE 64
uint8_t recv_buf[UART_RECV_BUF_SZIE];

struct at_client static_client;

static const char *http_get = "GET https://api.seniverse.com/v3/weather/now.json?key=SCeYO8heOo9MPuTxY&location=beijing&language=zh-Hans&unit=c\r\n";


int main(void)
{
    int ret = 0;
    uint32_t recv_len = 0;
    uint8_t recv_buffer[1024] = { 0 };

    bflb_platform_init(0);

    at_client_init();

    memset(&static_client, 0, sizeof(struct at_client));

    static_client.recv_line_buf = recv_buf;
    static_client.recv_bufsz = UART_RECV_BUF_SZIE;

    static_client.resp_succ = "OK";
    static_client.resp_err = NULL;
    static_client.timeout = 200;

    ret = at_exe_cmd(&static_client, "AT");

    //static_client.timeout = 1000;
    //ret = at_exe_cmd(&static_client, "AT+RST");
    ret = at_exe_cmd(&static_client, "AT+CWMODE=1");
    ret = at_exe_cmd(&static_client, "AT+CIPRECVCFG=1");

    static_client.resp_succ = "wifi connected";
    static_client.resp_err = NULL;
    static_client.timeout = 5000;

    while (at_exe_cmd(&static_client, "AT+CWJAP=\"%s\",\"%s\"", "OPPO Reno4 5G", "12345678") < 0) {
    }

    MSG("wifi connected success\r\n");
    bflb_platform_delay_ms(1000);

    static_client.resp_succ = "CONNECTED";
    static_client.resp_err = "ERROR: Connect fail";
    static_client.timeout = 5000;

    while (at_exe_cmd(&static_client, "AT+CIPSTART=0,\"TCP\",\"%s\",%d", "116.62.81.138", 80) < 0) {
    }

    MSG("tcp connected\r\n");
    bflb_platform_delay_ms(1000);

    at_write_recv((uint8_t *)http_get, 114, recv_buffer, &recv_len, 5000);
    at_dump_hex(recv_buffer, recv_len);

    while (1) {
        bflb_platform_delay_ms(1000);
    }
}