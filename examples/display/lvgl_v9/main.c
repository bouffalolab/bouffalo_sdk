/**
 * @file main.c
 * @brief LVGL v9 demos on the default display of the display framework.
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
#ifdef CONFIG_LVGL9_PORT_INDEV
#include "lv_port_indev.h"
#endif
#include "default/display_default.h"

#include "demos/lv_demos.h"

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS)
static TaskHandle_t lvgl_handle;
#endif

/* LVGL tick callback */
uint32_t lv_get_time_ms_cb(void)
{
    return (uint32_t)bflb_mtimer_get_time_ms();
}

#if (LV_USE_LOG)
/* LVGL log callback */
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
    (void)param;

    /* lvgl init */
    lv_init();
    lv_tick_set_cb(lv_get_time_ms_cb);
#if (LV_USE_LOG)
    lv_log_register_print_cb(lv_log_print_cb);
#endif
    if (!lv_port_disp_init()) {
        LOG_E("display port init failed\r\n");
#if defined(CONFIG_FREERTOS)
        vTaskDelete(NULL);
#endif
        return;
    }
#ifdef CONFIG_LVGL9_PORT_INDEV
    lv_port_indev_init();
#endif
    // lv_display_set_rotation(NULL, LV_DISPLAY_ROTATION_90); /* the panel is rotated by the display framework */

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

    LOG_I("LVGL V9 case\r\n");
    LOG_I("LVGL VER: %d.%d.%d\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

#if defined(CONFIG_FREERTOS)
    xTaskCreate(lvgl_main, (char *)"lvgl", 4096, NULL, configMAX_PRIORITIES - 2, &lvgl_handle);
    vTaskStartScheduler();
#else
    lvgl_main(NULL);
#endif
    while (1) {}
}
