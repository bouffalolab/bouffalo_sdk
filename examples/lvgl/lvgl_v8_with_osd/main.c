/**
 * @file main.c
 * @brief LVGL with MJDEC image switching 
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
#include "bflb_mtimer.h"
#include "bl618dg_glb.h"
#include "board.h"
#include "bflb_l1c.h"
#include "bflb_wdg.h"
#include <assert.h>

#include "lcd.h"
#include "dpi_manager.h"
#include "filesystem_reader.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "task.h"
#include "semphr.h"
#endif

#include "lvgl.h"
#include "lv_port_disp.h"
#ifdef CONFIG_LVGL8_PORT_INDEV
#include "lv_port_indev.h"
#endif

#include "demos/lv_demos.h"

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS)
static TaskHandle_t lvgl_handle;
static TaskHandle_t image_switch_handle;

SemaphoreHandle_t xSemaphore;
#endif

struct bflb_device_s *wdg = NULL;

/* YUV buffers for decoded output */
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) uint8_t yuv_buffer_0[LCD_WIDTH * LCD_HEIGHT * 2];
ATTR_NOINIT_PSRAM_SECTION __attribute__((aligned(32))) uint8_t yuv_buffer_1[LCD_WIDTH * LCD_HEIGHT * 2];
uint8_t *yuv_images[] = { yuv_buffer_0, yuv_buffer_1 };

/* lvgl log cb */
void lv_log_print_g_cb(const char *buf)
{
    LOG_RI("[LVGL]%s", buf);
}

static void board_jtag_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_JTAG_M0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_FUNC_JTAG_M0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_JTAG_M0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_FUNC_JTAG_M0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

extern uint8_t dpi_mjdec_isr_enable_flag;

void lvgl_main(void *param)
{
    /* lvgl init */
    lv_log_register_print_cb(lv_log_print_g_cb);
    lv_init();
    lv_port_disp_init();
#ifdef CONFIG_LVGL8_PORT_INDEV
    lv_port_indev_init();
#endif
    // lv_disp_set_rotation(NULL, LV_DISP_ROT_90);

    lv_demo_benchmark();
    // lv_demo_keypad_encoder();
    // lv_demo_music();
    // lv_demo_stress();
    // lv_demo_widgets();

    /* enable osd */
    lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_TRANSP);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, LV_PART_MAIN);

    lv_task_handler();

    LOG_I("lvgl success\r\n");

    while (1) {
        lv_task_handler();

        vTaskDelay(2);
    }
}

int main(void)
{
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");

    board_init();
    board_dpi_gpio_init();
    bflb_gpio_init(gpio, GPIO_PIN_45, GPIO_FUNC_GPIO | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_init(gpio, GPIO_PIN_46, GPIO_FUNC_GPIO | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_init(gpio, GPIO_PIN_47, GPIO_FUNC_GPIO | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_init(gpio, GPIO_PIN_48, GPIO_FUNC_GPIO | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN);
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    board_jtag_gpio_init();
    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = 31;
    wdg_cfg.comp_val = 5000;
    wdg_cfg.mode = WDG_MODE_RESET;

    wdg = bflb_device_get_by_name("watchdog0");
    assert(wdg != NULL);
    bflb_wdg_init(wdg, &wdg_cfg);

    bflb_wdg_start(wdg);

    GLB_Set_Display_CLK(1, GLB_DP_CLK_WIFIPLL_96M, 3);
    /* Initialize DPI manager */
    dpi_manager_init();

    LOG_I("LVGL V8 case\r\n");
    LOG_I("LVGL VER: %d.%d.%d\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

#if defined(CONFIG_FREERTOS)
    /* Initialize filesystem reader queues */
    if (filesystem_reader_init() < 0) {
        LOG_E("Failed to initialize filesystem reader\r\n");
    }

    xSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(lvgl_main, (char *)"lvgl_task", 2048, NULL, configMAX_PRIORITIES - 3, &lvgl_handle);
    xTaskCreate(image_switch_task, (char *)"img_switch", 2048, NULL, configMAX_PRIORITIES - 2, &image_switch_handle);
    xTaskCreate(filesystem_reader_task, (char *)"fs_reader", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
    vTaskStartScheduler();
#else
    lvgl_main(NULL);
#endif
}
