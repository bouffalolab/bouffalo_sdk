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

#include "lcd.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "task.h"
#endif

#include "lvgl.h"
#include "lv_port_disp.h"
#ifdef CONFIG_LVGL9_PORT_INDEV
#include "lv_port_indev.h"
#endif

#include "demos/lv_demos.h"

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS)
static TaskHandle_t lvgl_handle;
#endif

/* lvgl tick cb */
uint32_t lv_get_time_ms_cb(void)
{
    return (uint32_t)bflb_mtimer_get_time_ms();
}

#if (LV_USE_LOG)
/* lvgl log cb */
void lv_log_print_cb(lv_log_level_t level, const char *buf)
{
    switch (level) {
        case LV_LOG_LEVEL_TRACE:
        case LV_LOG_LEVEL_INFO:
            LOG_RI("[LVGL] %s", buf);
            break;

        case LV_LOG_LEVEL_WARN:
        case LV_LOG_LEVEL_USER:
            LOG_RW("[LVGL] %s", buf);
            break;

        case LV_LOG_LEVEL_ERROR:
            LOG_RE("[LVGL] %s", buf);
            break;

        default:
            LOG_RW("[LVGL] %s", buf);
            break;
    }
}
#endif

void lvgl_main(void *param)
{
    /* lvgl init */
    lv_init();
    /* tick cb */
    lv_tick_set_cb(lv_get_time_ms_cb);
#if (LV_USE_LOG)
    /* log cb */
    lv_log_register_print_cb(lv_log_print_cb);
#endif
    /* lcd init */
    lv_port_disp_init();
#ifdef CONFIG_LVGL9_PORT_INDEV
    lv_port_indev_init();
#endif
    // lv_disp_set_rotation(NULL, LV_DISP_ROTATION_90);

    lv_demo_benchmark();
    // lv_demo_flex_layout();
    // lv_demo_keypad_encoder();
    // lv_demo_multilang();
    // lv_demo_music();
    // lv_demo_render(LV_DEMO_RENDER_SCENE_FILL, LV_OPA_50);
    // lv_demo_scroll();
    // lv_demo_stress();
    // lv_demo_transform();
    // lv_demo_widgets();

    lv_task_handler();

    LOG_I("lvgl success\r\n");

    while (1) {
        lv_task_handler();
        bflb_mtimer_delay_us(10);
    }
}

int main(void)
{
    board_init();

    LOG_I("LVGL V9 case\r\n");
    LOG_I("LVGL VER: %d.%d.%d\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

#if defined(CONFIG_FREERTOS)
    xTaskCreate(lvgl_main, (char *)"test_task", 2048, NULL, configMAX_PRIORITIES - 2, &lvgl_handle);
    vTaskStartScheduler();
#else
    lvgl_main(NULL);
#endif
}
