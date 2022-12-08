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
 * Note:
 *
 * This Case is currently based on BL706_AVB development board with ili9341 controller screen.
 * If you use other hardware platforms, please modify the pin Settings by yourself
 *
 * Your should Short connect func1 and func2 jump cap on 706_AVB Board,and Plug the screen into the HD13 interface
 * Then the hardware for lvgl case testing is available.
 *
 *
 */
#include "board.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"

#include "lv_conf.h"
#include "lvgl.h"

#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "demos/lv_demos.h"

#include "lcd.h"

/* lvgl log cb */
void lv_log_print_g_cb(const char *buf)
{
    printf("[LVGL] %s", buf);
}

/**
 * @brief spi gpio init
 *
 * Please modify lcd pin: cs/dc/clk/mosi with your own board. Default spi mapping is for BL808.
 * Default lcd type is st7789v with 240 * 280
 */
static void gpio_init(void)
{
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");

    /* lcd cs, see in bsp/common/lcd/spi/xxx_spi.h */
    // bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* lcd dc, see in bsp/common/lcd/spi/xxx_spi.h */
    // bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* lcd spi mosi */
    bflb_gpio_init(gpio, GPIO_PIN_25, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* lcd spi clock */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* backlight pin */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_set(gpio, GPIO_PIN_11);
    /* reset pin */
    bflb_gpio_init(gpio, GPIO_PIN_24, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_set(gpio, GPIO_PIN_24);
}

int main(void)
{
    board_init();
    gpio_init();

    printf("lvgl case\r\n");

    /* lvgl init */
    lv_log_register_print_cb(lv_log_print_g_cb);
    lv_init();
    lv_port_disp_init();
    // lv_port_indev_init();

    lv_demo_benchmark();
    // lv_demo_stress();

    lv_task_handler();

    printf("lvgl success\r\n");

    while (1) {
        lv_task_handler();
    }
}
