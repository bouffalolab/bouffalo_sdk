/**
 * @file main.c
 * @brief LVGL v8 demos on the default display of the display framework.
 *
 * The screen is defined by Kconfig (see defconfig and README.md); lv_port_disp_init() takes it with
 * display_default_get(), brings it up and registers it with LVGL. Pick the demo below.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "board.h"
#include "bflb_mtimer.h"

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "task.h"
#endif

#include "lvgl.h"
#include "lv_port_disp_default.h"
#ifdef CONFIG_LVGL8_PORT_INDEV
#include "lv_port_indev.h"
#endif
#include "default/display_default.h"

#include "demos/lv_demos.h"

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS)
static TaskHandle_t lvgl_handle;
#endif

#if (LV_USE_LOG)
/* LVGL log callback */
void lv_log_print_g_cb(const char *buf)
{
    LOG_RI("[LVGL]%s", buf);
}
#endif

void lvgl_main(void *param)
{
    (void)param;

    /* lvgl init */
#if (LV_USE_LOG)
    lv_log_register_print_cb(lv_log_print_g_cb);
#endif
    lv_init();
    if (!lv_port_disp_init()) {
        LOG_E("display port init failed\r\n");
#if defined(CONFIG_FREERTOS)
        vTaskDelete(NULL);
#endif
        return;
    }
#ifdef CONFIG_LVGL8_PORT_INDEV
    lv_port_indev_init();
#endif
    /* Software rotation needs CONFIG_LVGL8_DISP_DEFAULT_SW_ROTATE */
    // lv_disp_set_rotation(NULL, LV_DISP_ROT_90);

    lv_demo_benchmark();
    // lv_demo_keypad_encoder();
    // lv_demo_music();
    // lv_demo_stress();
    // lv_demo_widgets();

    lv_task_handler();

    LOG_I("lvgl success\r\n");

    while (1) {
        lv_task_handler();
#if defined(CONFIG_FREERTOS)
        vTaskDelay(pdMS_TO_TICKS(5));
#else
        bflb_mtimer_delay_ms(5);
#endif
    }
}

int main(void)
{
    board_init();

    LOG_I("LVGL V8 case\r\n");
    LOG_I("LVGL VER: %d.%d.%d\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

#if defined(CONFIG_FREERTOS)
    xTaskCreate(lvgl_main, (char *)"lvgl", 4096, NULL, configMAX_PRIORITIES - 2, &lvgl_handle);
    vTaskStartScheduler();
#else
    lvgl_main(NULL);
#endif
    while (1) {}
}
