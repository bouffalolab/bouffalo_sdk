/**
 * @file main.c
 * @brief JPEG video background + LVGL v9 UI (OSD overlay).
 *
 * Display bring-up is all in lcd_init() (bsp/common/lcd panel driver per lcd_conf_user.h);
 * dpi_manager owns only the video background + MJDEC decode. Tasks:
 *   - filesystem_reader_task : default mode, read /sd/<res>/CCC/pNNNN.jpg into a queue
 *   - usb_reader_task        : USB mode, de-frame DATA ACM JPEG stream into a queue
 *   - image_switch_task      : MJDEC-decode frames into ping-pong YUV, shown as background
 *   - lvgl_task              : SquareLine UI on a transparent OSD overlay, composited on top
 */

#include "board.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_osd.h"
#include <FreeRTOS.h>
#include "task.h"

#ifndef CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#define CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO 0
#endif

#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#include "rfparam_adapter.h"
#include "lwip/tcpip.h"
#include <string.h>
#endif

#include "lcd.h"
#include "dpi_manager.h"
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#include "usb_reader.h"
#include "app_wifi.h"
#include "app_usb_composite.h"
#include "nethub.h"
#include "nethub_vchan.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#include <stdlib.h>
#endif
#else
#include "filesystem_reader.h"
#endif

#include "lvgl.h"
#include "lv_demos.h"
#include "lv_port_disp.h"

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
#include "bl618dg_glb.h" /* GLB_Set_Display_CLK for the DPI pixel clock */
#endif
#if defined(LVGL_WITH_SQUARELINE_UI) && LVGL_WITH_SQUARELINE_UI
#include "ui.h" /* SquareLine UI; CMake picks the matching UICODE folder per panel */
#endif

#define DBG_TAG "MAIN"
#include "log.h"

/* task stacks (words); LVGL needs a large stack (see lvgl-on-bl618dg-dsi-720p memory) */
#define IMG_TASK_STACK   2048
#define LVGL_TASK_STACK  4096 /* 16KB */
#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
#define USB_TASK_STACK           2048
#define APP_INIT_TASK_STACK      4096
#define APP_INIT_TASK_PRIORITY   26
#define LVGL_TASK_PRIORITY       (configMAX_PRIORITIES - 5)
#define IMG_TASK_PRIORITY        (configMAX_PRIORITIES - 4)
#define USB_READER_TASK_PRIORITY (configMAX_PRIORITIES - 3)
#else
#define FS_TASK_STACK            1536 /* holds FIL(~580B)+FILINFO(~300B)+path buffers */
#endif

#if (DPI_PIXEL_CLOCK_USE_SW_GPIO)
/* Pixel clock via software GPIO (CLKOUT). Only LCD_DPI_STANDARD uses this; other panels
 * drive PCLK from the DPI peripheral's hardware pin (see DPI_PIXEL_CLOCK_USE_SW_GPIO). */
static void dpi_pixel_clock_output(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}
#endif

/* The LVGL draw buffers (OSD overlay) and the flush/swap path live in the framework port
 * (lv_port_disp_rgb.c): it calls lcd_init(), wires flush -> lcd_screen_switch() (OSD swap),
 * and rotates the buffers from the SEOF interrupt. This file only owns the video pipeline. */

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

/* The SquareLine UI runs on a transparent screen background so the video (background layer)
 * shows through, with the widgets floating on top as an opaque HUD on the OSD overlay. */

/* Panel hardware scan-out rate: the framework fires a CYCLE callback per scanned frame (OSD
 * SEOF), so counting it gives the true refresh rate, independent of LVGL/video. The ISR only
 * increments; lvgl_task prints it once a second. */
static volatile uint32_t scan_frame_count = 0;

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

static void panel_scan_cycle_cb(void)
{
    scan_frame_count++;
}

static void lvgl_task(void *param)
{
    (void)param;

    LOG_I("LVGL VER: %d.%d.%d\r\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    lv_init();
    lv_tick_set_cb(lv_tick_cb);
#if LV_USE_LOG
    lv_log_register_print_cb(lv_log_cb);
#endif

    /* Framework display port: brings up the whole display side via lcd_init(), creates the
     * LVGL display with its triple draw buffers, and wires flush -> lcd_screen_switch() (OSD swap). */
    lv_port_disp_init();

#if (DPI_PIXEL_CLOCK_USE_SW_GPIO)
    dpi_pixel_clock_output();
#endif

    /* Count SEOF scan-out frames to report the panel's true hardware refresh rate. */
    lcd_frame_callback_register(FRAME_INT_TYPE_CYCLE, panel_scan_cycle_cb);

#if defined(LVGL_WITH_SQUARELINE_UI) && LVGL_WITH_SQUARELINE_UI
    ui_init();
    // lv_demo_benchmark();
#else
    lv_demo_benchmark();
#endif

    /* Make the screen background transparent so the video shows through, leaving
     * the widgets as an opaque HUD. Set AFTER ui_init()'s lv_screen_load(). */
    lv_obj_set_style_bg_opa(lv_screen_active(), 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lv_layer_top(), 0, LV_PART_MAIN);
    // struct bflb_device_s *osd0 = bflb_device_get_by_name("osd0");
    // bflb_osd_blend_set_global_a(osd0, true, 0x10);

#if defined(LCD_BACKLIGHT_EN) && LCD_BACKLIGHT_EN
    /* Render the first frame before enabling the backlight so power-up never
     * shows an uninitialized/garbage frame. */
    lv_task_handler();
    lv_refr_now(NULL);
    lcd_backlight_toggle(true);
#endif

    uint32_t scan_t0 = (uint32_t)bflb_mtimer_get_time_ms();
    uint32_t scan_last = scan_frame_count;
    while (1) {
        lv_task_handler();
        uint32_t now = (uint32_t)bflb_mtimer_get_time_ms();
        if (now - scan_t0 >= 1000) {
            uint32_t cnt = scan_frame_count;
            LOG_I("panel scan-out: %lu fps\r\n", (unsigned long)((cnt - scan_last) * 1000 / (now - scan_t0)));
            scan_last = cnt;
            scan_t0 = now;
        }

        vTaskDelay(1);
    }
}

#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
static void app_init_task(void *param)
{
    (void)param;

    rfparam_init(0, NULL, 0);
    tcpip_init(NULL, NULL);
    dpi_manager_init();
    usb_reader_init();
    app_usb_composite_configure();
    app_wifi_rx_filter_init();
    nethub_bootstrap();
    cmd_acm_ping_init();
    app_usb_composite_start();

    xTaskCreate(lvgl_task, "lvgl", LVGL_TASK_STACK, NULL, LVGL_TASK_PRIORITY, NULL);
    xTaskCreate(image_switch_task, "img_switch", IMG_TASK_STACK, NULL, IMG_TASK_PRIORITY, NULL);
    xTaskCreate(usb_reader_task, "usb_reader", USB_TASK_STACK, NULL, USB_READER_TASK_PRIORITY, NULL);
    app_wifi_start();

    vTaskDelete(NULL);
}
#endif

int main(void)
{
    board_init();

#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
    /* USB DP/DM are GPIO40/41 on BL618DG; mux them before CherryUSB starts. */
    board_usb_gpio_init();

#ifdef CONFIG_SHELL
    shell_init_with_task(bflb_device_get_by_name("uart0"));
#endif
#endif

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
    /* DPI pixel clock */
    GLB_Set_Display_CLK(1, GLB_DP_CLK_WIFIPLL_160M, 2); //1024x600 div=1
    
    LOG_I("DPI(standard RGB) + LVGL OSD: %s video background + LVGL benchmark overlay\r\n",
          CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO ? "USB-ACM" : "SD");
#else
    LOG_I("DSI(LCD framework) + LVGL OSD: %s video background + LVGL UI overlay\r\n",
          CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO ? "USB-ACM" : "SD");
#endif
    LOG_I("PSRAM physical size: %lu MB (AP budget %lu MB)\r\n",
          (unsigned long)(board_psram_size_get() / (1024 * 1024)),
          (unsigned long)(CONFIG_PSRAM_FOR_AP_SIZE / (1024 * 1024)));
    /* Video pipeline HW (MJDEC + DMA2D + YUV background framebuffers). The DSI
     * display side is brought up later by lv_port_disp_init() in lvgl_task. The
     * video task blocks on the JPEG queue until fs_reader produces a frame, so
     * the DPI background framebuffer is only switched in after the display is up
     * (same task ordering as lvgl_v8_with_osd). */

#if CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO
    xTaskCreate(app_init_task, "app_init", APP_INIT_TASK_STACK, NULL, APP_INIT_TASK_PRIORITY, NULL);
#else
    /* Video pipeline HW (MJDEC + DMA2D + YUV background buffers). The display side comes up
     * later in lvgl_task; the video task blocks on the JPEG queue until fs_reader produces a
     * frame, so the background is only switched in after the display is up. */

    if (dpi_manager_init() != 0) {
        LOG_E("dpi_manager_init failed\r\n");
        while (1) {
        }
    }

    if (filesystem_reader_init() != 0) {
        LOG_E("filesystem_reader_init failed\r\n");
        while (1) {
        }
    }

    xTaskCreate(lvgl_task, "lvgl", LVGL_TASK_STACK, NULL, configMAX_PRIORITIES - 3, NULL);
    xTaskCreate(image_switch_task, "img_switch", IMG_TASK_STACK, NULL, configMAX_PRIORITIES - 2, NULL);
    xTaskCreate(filesystem_reader_task, "fs_reader", FS_TASK_STACK, NULL, configMAX_PRIORITIES - 1, NULL);
#endif

    vTaskStartScheduler();

    while (1) {
    }
}
