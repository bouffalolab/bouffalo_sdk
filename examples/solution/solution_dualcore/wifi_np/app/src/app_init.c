#include <stdint.h>

#if defined(CONFIG_FREERTOS)
#include <FreeRTOS.h>
#include "task.h"
#endif

#include "lvgl.h"
#include "lv_port_disp.h"
#ifdef CONFIG_LVGL8_PORT_INDEV
#include "lv_port_indev.h"
#endif
#include <shell.h>

#define DBG_TAG "MAIN"
#include "log.h"

#if defined(CONFIG_FREERTOS)
static TaskHandle_t lvgl_handle;
#endif

/* lvgl log cb */
static void lv_log_print_g_cb(const char *buf)
{
    LOG_RI("[LVGL]%s", buf);
}

static void lvgl_main(void *param)
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

    lv_task_handler();

    LOG_I("lvgl success\r\n");

    while (1) {
        lv_task_handler();
        //bflb_mtimer_delay_us(10);
    }
}

static void coremark_main(void *param)
{
    core_mark();

    vTaskDelete(NULL);
}

static int cmd_coremark(int argc, char **argv)
{
    xTaskCreate(lvgl_main, (char *)"lvgl_task", 2048, NULL, 27, &lvgl_handle);
    xTaskCreate(coremark_main, (char *)"coremark_task", 2048, NULL, 27, NULL);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_coremark, coremark, coremark test);


extern void app_sample_init(void);


int app_init(void)
{
    app_sample_init();
    app_wifi_init();
    return 0;
}

