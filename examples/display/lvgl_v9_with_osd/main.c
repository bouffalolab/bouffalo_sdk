/**
 * @file main.c
 * @brief JPEG video background + LVGL v9 UI (OSD overlay).
 *
 * Display bring-up belongs to the default video LVGL port and app_display;
 * dpi_manager owns only the video background + MJDEC decode. Tasks:
 *   - filesystem_reader_task : default mode, read /sd/<res>/CCC/pNNNN.jpg into a queue
 *   - usb_reader_task        : USB mode, de-frame DATA ACM JPEG stream into a queue
 *   - image_switch_task      : MJDEC-decode frames into ping-pong YUV, shown as background
 *   - lvgl_task              : adaptive UI on a transparent OSD overlay, composited on top
 */

#include "board.h"
#include "bflb_mtimer.h"
#include <FreeRTOS.h>
#include "task.h"

#ifndef CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#define CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO 0
#endif
#ifndef CONFIG_LVGL_V9_WITH_OSD_OTA
#define CONFIG_LVGL_V9_WITH_OSD_OTA 0
#endif
#define APP_NETWORK_ENABLED (CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO || CONFIG_LVGL_V9_WITH_OSD_OTA)

#if APP_NETWORK_ENABLED
#include "rfparam_adapter.h"
#include "lwip/tcpip.h"
#include "app_wifi.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif
#endif
#if CONFIG_LVGL_V9_WITH_OSD_OTA
#include "app_ota.h"
#endif
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#include <string.h>
#endif

#include "app_display.h"
#include "dpi_manager.h"
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#include "usb_reader.h"
#include "app_usb_composite.h"
#include "nethub.h"
#include "nethub_vchan.h"
#else
#include "filesystem_reader.h"
#endif

#include "lvgl.h"
#include "ui.h"
#include "lv_port_disp_default.h"

#define DBG_TAG "MAIN"
#include "log.h"

/* task stacks (words); LVGL needs a large stack (see lvgl-on-bl618dg-dsi-720p memory) */
#define IMG_TASK_STACK   2048
#define LVGL_TASK_STACK  4096 /* 16KB */
#define FS_TASK_STACK            1536 /* FIL + FILINFO + path buffers */
#define APP_INIT_TASK_STACK      4096
#if APP_NETWORK_ENABLED
#define APP_INIT_TASK_PRIORITY   26
/* Keep display work below Wi-Fi, shell and OTA so large frames cannot starve them. */
#define LVGL_TASK_PRIORITY       5
#define IMG_TASK_PRIORITY        6
#define FS_READER_TASK_PRIORITY  7
#else
#define APP_INIT_TASK_PRIORITY   (configMAX_PRIORITIES - 1)
#define LVGL_TASK_PRIORITY       (configMAX_PRIORITIES - 3)
#define IMG_TASK_PRIORITY        (configMAX_PRIORITIES - 2)
#define FS_READER_TASK_PRIORITY  (configMAX_PRIORITIES - 1)
#endif
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#define USB_TASK_STACK           2048
#define USB_READER_TASK_PRIORITY FS_READER_TASK_PRIORITY
#endif

/* The default video port owns the ARGB8888 OSD buffers and frame retirement.
 * app_display owns the NV12 Base layer and its scan-boundary counter. */
static TaskHandle_t app_init_handle;

static uint32_t lv_tick_cb(void)
{
    return (uint32_t)bflb_mtimer_get_time_ms();
}

#if LV_USE_LOG
static void lv_log_cb(lv_log_level_t level, const char *buf)
{
    (void)level;
    printf("[LVGL] %s", buf);
}
#endif

/* The adaptive UI runs on a transparent screen background so the video (background layer)
 * shows through, with the widgets floating on top as an opaque HUD on the OSD overlay. */

#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
static int cmd_acm_ping_receive_cb(void *arg, uint8_t *data, uint16_t len)
{
    static const uint8_t ping[] = "PING";
    static const uint8_t pong[] = "PONG";
    int ret;

    (void)arg;

    if (len != (sizeof(ping) - 1U) || memcmp(data, ping, sizeof(ping) - 1U) != 0) {
        return NETHUB_OK;
    }

    ret = nethub_vchan_at_send(pong, (uint16_t)(sizeof(pong) - 1U));
    if (ret != NETHUB_OK) {
        LOG_W("CMD ACM PONG send failed: %d\r\n", ret);
    }

    return NETHUB_OK;
}

static void cmd_acm_ping_init(void)
{
    int ret = nethub_vchan_at_recv_register(cmd_acm_ping_receive_cb, NULL);

    if (ret != NETHUB_OK) {
        LOG_W("CMD ACM ping responder init failed: %d\r\n", ret);
    } else {
        LOG_I("CMD ACM ping responder ready\r\n");
    }
}
#endif

#if CONFIG_LVGL_V9_WITH_OSD_OTA
static void ota_status_timer_cb(lv_timer_t *timer)
{
    static app_ota_phase_t last_phase = APP_OTA_IDLE;
    static uint32_t last_percent = UINT32_MAX;
    app_ota_phase_t phase = g_ota_status.phase;
    uint32_t received = g_ota_status.received;
    uint32_t total = g_ota_status.total;

    (void)timer;
    uint32_t percent = total ? (uint32_t)((uint64_t)received * 100 / total) : 0;
    if (phase != last_phase || (g_ota_status.busy && percent != last_percent)) {
        char text[96];
        snprintf(text, sizeof(text), "OTA %s %lu%%", app_ota_phase_name(phase), (unsigned long)percent);
        ui_set_ota_status(text);
        last_phase = phase;
        last_percent = percent;
    }
}
#endif

static void lvgl_task(void *param)
{
    (void)param;

    LOG_I("LVGL VER: %d.%d.%d\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    lv_init();
    lv_tick_set_cb(lv_tick_cb);
#if LV_USE_LOG
    lv_log_register_print_cb(lv_log_cb);
#endif

    /* Initialize the default video display before attaching the NV12 Base layer.
     * Do not start the producer/decoder if either initialization fails. */
    if (!lv_port_disp_init() || app_display_init() != 0) {
        LOG_E("Display initialization failed; playback not started\r\n");
        xTaskNotify(app_init_handle, 2, eSetValueWithOverwrite);
        vTaskDelete(NULL);
        return;
    }

    ui_init();
#if CONFIG_LVGL_V9_WITH_OSD_OTA
    if (!lv_timer_create(ota_status_timer_cb, 100, NULL)) {
        LOG_E("OTA UI timer allocation failed\r\n");
        xTaskNotify(app_init_handle, 2, eSetValueWithOverwrite);
        vTaskDelete(NULL);
        return;
    }
#endif

    /* Make the screen background transparent so the video shows through, leaving
     * the widgets as an opaque HUD. */
    lv_obj_set_style_bg_opa(lv_screen_active(), 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lv_layer_top(), 0, LV_PART_MAIN);

    /* Backlight and display clocks/GPIO belong to the default display port. */
    xTaskNotify(app_init_handle, 1, eSetValueWithOverwrite);

    uint32_t scan_t0 = (uint32_t)bflb_mtimer_get_time_ms();
    uint32_t scan_last = app_display_get_scan_frame_count();
    while (1) {
        lv_task_handler();
        uint32_t now = (uint32_t)bflb_mtimer_get_time_ms();
        if (now - scan_t0 >= 1000) {
            uint32_t cnt = app_display_get_scan_frame_count();
            LOG_I("panel scan-out: %lu fps\r\n", (unsigned long)((cnt - scan_last) * 1000 / (now - scan_t0)));
            scan_last = cnt;
            scan_t0 = now;
        }

        vTaskDelay(1);
    }
}

static void app_init_task(void *param)
{
    TaskHandle_t lvgl_handle = NULL;
    TaskHandle_t image_handle = NULL;
    TaskHandle_t reader_handle = NULL;
    uint32_t display_ready = 0;

    (void)param;
    app_init_handle = xTaskGetCurrentTaskHandle();

#if APP_NETWORK_ENABLED
    int rf_ready = rfparam_init(0, NULL, 0);
    if (rf_ready != 0) {
        LOG_E("RF parameter initialization failed: %d\r\n", rf_ready);
    }
    tcpip_init(NULL, NULL);
#endif
    if (dpi_manager_init() != 0) {
        LOG_E("dpi_manager_init failed\r\n");
        vTaskDelete(NULL);
        return;
    }
#if CONFIG_LVGL_V9_WITH_OSD_OTA
    video_ota_configure_participants(0);
#endif
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
    if (usb_reader_init() != 0 || app_usb_composite_configure() != 0) {
        LOG_E("USB video initialization failed\r\n");
        vTaskDelete(NULL);
        return;
    }
    app_wifi_rx_filter_init();
    if (nethub_bootstrap() != NETHUB_OK) {
        LOG_E("NetHub initialization failed\r\n");
        vTaskDelete(NULL);
        return;
    }
    cmd_acm_ping_init();
#else
    if (filesystem_reader_init() != 0) {
        LOG_E("filesystem_reader_init failed\r\n");
        vTaskDelete(NULL);
        return;
    }
#endif

    /* Network services must not depend on a successful display bring-up. */
#if APP_NETWORK_ENABLED
    if (rf_ready == 0) {
        if (xTaskCreate(wifi_start_firmware_task, "wifi init", 1024, NULL, 10, NULL) != pdPASS) {
            LOG_E("Wi-Fi startup task allocation failed\r\n");
        }
    }
#endif
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
    /* Preserve ECM/CMD ACM enumeration even if the display cannot start. */
    if (app_usb_composite_start() != 0) {
        LOG_E("USB composite start failed\r\n");
        vTaskDelete(NULL);
        return;
    }
#endif

    if (xTaskCreate(lvgl_task, "lvgl", LVGL_TASK_STACK, NULL, LVGL_TASK_PRIORITY, &lvgl_handle) != pdPASS) {
        LOG_E("LVGL task allocation failed\r\n");
        vTaskDelete(NULL);
        return;
    }
    xTaskNotifyWait(0, UINT32_MAX, &display_ready, portMAX_DELAY);
    if (display_ready != 1) {
        vTaskDelete(NULL);
        return;
    }
#if CONFIG_LVGL_V9_WITH_OSD_OTA
    video_ota_configure_participants(VIDEO_OTA_PRODUCER | VIDEO_OTA_DECODER);
#endif
    if (xTaskCreate(image_switch_task, "img_switch", IMG_TASK_STACK, NULL, IMG_TASK_PRIORITY, &image_handle) != pdPASS) {
        goto task_fail;
    }
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
    if (xTaskCreate(usb_reader_task, "usb_reader", USB_TASK_STACK, NULL, USB_READER_TASK_PRIORITY, &reader_handle) != pdPASS) {
        goto task_fail;
    }
#else
    if (xTaskCreate(filesystem_reader_task, "fs_reader", FS_TASK_STACK, NULL, FS_READER_TASK_PRIORITY, &reader_handle) != pdPASS) {
        goto task_fail;
    }
#endif

    vTaskDelete(NULL);
    return;

task_fail:
    LOG_E("Video startup failed; playback stopped\r\n");
    if (reader_handle) {
        vTaskDelete(reader_handle);
    }
    if (image_handle) {
        vTaskDelete(image_handle);
    }
#if CONFIG_LVGL_V9_WITH_OSD_OTA
    video_ota_configure_participants(0);
#endif
    vTaskDelete(lvgl_handle);
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
    /* USB DP/DM are GPIO40/41 on BL618DG; mux them before CherryUSB starts. */
    board_usb_gpio_init();
#endif

#if APP_NETWORK_ENABLED && defined(CONFIG_SHELL)
    shell_init_with_task(bflb_device_get_by_name("uart0"));
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_DSI)
    LOG_I("DSI(default display) + LVGL OSD: %s video background + LVGL UI overlay\r\n",
          CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO ? "USB-ACM" : "SD");
#else
    LOG_I("DPI(standard RGB) + LVGL OSD: %s video background + LVGL UI overlay\r\n",
          CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO ? "USB-ACM" : "SD");
#endif
    LOG_I("PSRAM physical size: %lu MB (AP budget %lu MB)\r\n",
          (unsigned long)(board_psram_size_get() / (1024 * 1024)),
          (unsigned long)(CONFIG_PSRAM_FOR_AP_SIZE / (1024 * 1024)));
    /* app_init_task retains the original network/reader setup. It waits for the
     * LVGL port and app_display before starting either video task; the decoder
     * also uses app_display_wait_ready() as its display readiness barrier. */

    if (xTaskCreate(app_init_task, "app_init", APP_INIT_TASK_STACK, NULL, APP_INIT_TASK_PRIORITY, NULL) != pdPASS) {
        LOG_E("Application init task allocation failed\r\n");
        while (1) {
        }
    }

    vTaskStartScheduler();

    while (1) {
    }
}