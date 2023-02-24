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
 *
 */
#include "board.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"

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

int main(void)
{
    board_init();

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
        bflb_mtimer_delay_ms(1);
    }
}
