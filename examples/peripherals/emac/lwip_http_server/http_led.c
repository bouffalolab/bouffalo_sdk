/**
 * @file http_led.c
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
#include "bflb_gpio.h"
#include "board.h"
#include "log.h"

#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"

#include "http_server.h"
#include "http_led.h"

#define LED_OFF  0
#define LED_ON   1
#define LED_GPIO GPIO_PIN_18 /* do not used flash or uart io */
struct bflb_device_s *gpio;

void bsp_led_config(void)
{
    printf("led gpio init\r\n");
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, LED_GPIO, (0xB << GPIO_FUNC_SHIFT) | GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_reset(gpio, LED_GPIO);
}

void led_on(void)
{
    bflb_gpio_set(gpio, LED_GPIO);
}

void led_off(void)
{
    bflb_gpio_reset(gpio, LED_GPIO);
}

void led_task(void *pvParameters)
{
    const TickType_t x_delay = pdMS_TO_TICKS(50);

    bsp_led_config();

    s_x_btn_semaphore = xSemaphoreCreateBinary();

    while (1) {
        if (xSemaphoreTake(s_x_btn_semaphore, x_delay) == pdTRUE) {
            if (button_clicked_flag) {
                printf("***************************************\r\n");
                printf("LED ON\r\n");
                printf("***************************************\r\n");
                led_on();
            } else {
                led_off();
                printf("***************************************\r\n");
                printf("LED OFF\r\n");
                printf("***************************************\r\n");
            }
        }
    }
}
