/**
 * @file main.c
 * @brief SD-card JPEG video (background) + LVGL v9 UI (OSD overlay).
 *
 * The whole chain bring-up is done by lcd_init()
 * via the bsp/common/lcd DSI panel driver selected in lcd_conf_user.h, so this
 * project has no dependency on chiptest's dsi_common. dpi_manager only owns the
 * DPI background + OSD overlay + MJDEC decode.
 *
 * Tasks:
 *   - filesystem_reader_task : read /sd/<res>/CCC/pNNNN.jpg into a 2-buffer queue
 *   - image_switch_task      : MJDEC-decode frames into ping-pong YUV, DPI shows them
 *   - lvgl_task              : SquareLine UI rendered into a transparent ARGB8888
 *                              OSD overlay, hardware-composited over the video
 */

#include "board.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_osd.h"
#include <FreeRTOS.h>
#include "task.h"

#include "lcd.h"
#include "dpi_manager.h"
#include "filesystem_reader.h"

#include "lvgl.h"
#include "lv_demos.h"
#include "lv_port_disp.h"

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
#include "bl618dg_glb.h" /* GLB_Set_Display_CLK for the DPI pixel clock */
#else
// use benchmark for now
// #include "ui.h" /* SquareLine UI; CMake picks UICODE480/UICODE720 per DSI panel */
#endif

#define DBG_TAG "MAIN"
#include "log.h"

/* task stacks (words); LVGL needs a large stack (see lvgl-on-bl618dg-dsi-720p memory) */
#define FS_TASK_STACK    1536 /* holds FIL(~580B)+FILINFO(~300B)+path buffers */
#define IMG_TASK_STACK   2048
#define LVGL_TASK_STACK  4096 /* 16KB */

/* The LVGL draw buffers (the OSD overlay canvases) and the display flush/swap
 * path are owned by the framework port (components/.../lv_port_disp_rgb.c). It
 * calls lcd_init() -> the DSI panel bring-up, registers the flush callback that
 * drives lcd_screen_switch() (-> mipi_dsi_v2 OSD swap), and rotates the triple
 * buffers from the OSD SEOF interrupt. This file only owns the video pipeline. */

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

/* Full-screen video: the SquareLine UI runs on a transparent screen background
 * so the DSI video (DPI background layer) shows through everywhere, with the
 * widgets floating on top as an opaque HUD on the OSD overlay. */

/* Panel hardware scan-out frame rate: the LCD framework fires a CYCLE callback
 * from the OSD SEOF interrupt once per scanned frame, so counting it gives the
 * true physical refresh rate (= pixel_clock / (Htotal * Vtotal)), independent of
 * how fast LVGL renders or the video decodes. The ISR only increments; lvgl_task
 * prints the rate once a second. */
static volatile uint32_t scan_frame_count = 0;

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

    /* Framework display port: brings up the whole DSI display side via lcd_init()
     * (panel link + DPI background + OSD0 overlay + OSD interrupt, all in
     * mipi_dsi_v2), creates the LVGL display with its triple draw buffers, and
     * wires flush -> lcd_screen_switch() -> OSD swap (SEOF-interrupt synced). */
    lv_port_disp_init();

    /* Count SEOF scan-out frames to report the panel's true hardware refresh rate. */
    lcd_frame_callback_register(FRAME_INT_TYPE_CYCLE, panel_scan_cycle_cb);

    lv_demo_benchmark();
    // ui_init();

    /* Make the screen background transparent so the video shows through, leaving
     * the widgets as an opaque HUD. Set AFTER ui_init()'s lv_screen_load(). */
    lv_obj_set_style_bg_opa(lv_screen_active(), 100, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(lv_layer_top(), 100, LV_PART_MAIN);

    // struct bflb_device_s *osd0 = bflb_device_get_by_name("osd0");
    // bflb_osd_blend_set_global_a(osd0, true, 0x50);

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

int main(void)
{
    board_init();

#if (LCD_INTERFACE_TYPE == LCD_INTERFACE_DPI)
    /* DPI parallel-RGB bring-up. lcd_init() (-> bl_mipi_dpi_v2_init in OSD-layer
     * mode, run later by lv_port_disp_init()) does NOT mux the RGB pins or set the
     * pixel clock itself, so do it here before any task starts (same as
     * lvgl_v8_with_osd's main). board_dpi_gpio_init() muxes the RGB data/sync pins;
     * pins 0..3 are the remaining data lines it does not cover. */
    {
        struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
        board_dpi_gpio_init();
        bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_DPI | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        /* DPI pixel clock */
        GLB_Set_Display_CLK(1, GLB_DP_CLK_WIFIPLL_96M, 1);
        
    }
    LOG_I("DPI(standard RGB) + LVGL OSD: SD video background + LVGL benchmark overlay\r\n");
#else
    LOG_I("DSI(LCD framework) + LVGL OSD: SD video background + LVGL benchmark overlay\r\n");
#endif
    LOG_I("PSRAM physical size: %lu MB (AP budget %lu MB)\r\n",
          (unsigned long)(board_psram_size_get() / (1024 * 1024)),
          (unsigned long)(CONFIG_PSRAM_FOR_AP_SIZE / (1024 * 1024)));

    /* Video pipeline HW (MJDEC + DMA2D + YUV background framebuffers). The DSI
     * display side is brought up later by lv_port_disp_init() in lvgl_task. The
     * video task blocks on the JPEG queue until fs_reader produces a frame, so
     * the DPI background framebuffer is only switched in after the display is up
     * (same task ordering as lvgl_v8_with_osd). */
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

    vTaskStartScheduler();

    while (1) {
    }
}
