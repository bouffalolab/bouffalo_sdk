#include <stdio.h>

#include "bflb_irq.h"

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "lcd.h"

#include "soln_fbq.h"

#include "soln_rgb_disp.h"
#include "soln_rgb_draw.h"

#ifdef CONFIG_SOLN_DISP_RGB_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_DISP_RGB_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_RGB"
#include "log.h"

#if (LCD_INTERFACE_TYPE != LCD_INTERFACE_DPI)
#error "The example code is only for LCD with RGB interface, please check your LCD config in menuconfig"
#endif

#ifdef CONFIG_PSRAM
#define RGB_FB_ATTR __ALIGNED(64) ATTR_NOINIT_PSRAM_SECTION
#else
#define RGB_FB_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#endif

/* */
#define RGB_DISP_3_BUFFER_EN 1

static fbq_ctrl_t *img_raw_frame_ctrl_disp = NULL;
static uint16_t img_raw_queue_id_disp;
static uint16_t img_raw_queue_depth_disp;

static TaskHandle_t rgb_disp_handle;
static SemaphoreHandle_t rgb_disp_semaphore;

#if RGB_DISP_3_BUFFER_EN
static lcd_color_t RGB_FB_ATTR rgb_screen_buf[3][LCD_W * LCD_H];
#else
static lcd_color_t RGB_FB_ATTR rgb_screen_buf[2][LCD_W * LCD_H];
#endif
static uint8_t rgb_buf_idx = 0;

static uint32_t s_rgb_disp_total_frame_count = 0;

uint32_t soln_rgb_disp_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_rgb_disp_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_rgb_disp_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_rgb_disp_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void rgb_disp_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_rgb_disp_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

static void rgb_screen_frame_callback(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(rgb_disp_semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static lcd_color_t *rgb_disp_get_draw_buffer(void)
{
    rgb_buf_idx = (rgb_buf_idx + 1) % (RGB_DISP_3_BUFFER_EN ? 3 : 2);

    return rgb_screen_buf[rgb_buf_idx];
}

static void rgb_disp_flush_wait(void)
{
    /* wait for next frame interrupt to ensure the current frame is displayed */
    xSemaphoreTake(rgb_disp_semaphore, portMAX_DELAY);
}

static ATTR_TCM_SECTION void rgb_disp_lcd_task(void *pvParameters)
{
    int ret;
    void *disp_fb;
    fbq_elem_t *disp_src_frame;

    (void)pvParameters;

    vTaskDelay(100);

    /* drop the first frame */
    ret = fbq_pop(img_raw_frame_ctrl_disp, &disp_src_frame, img_raw_queue_id_disp, 0);
    if (ret == FBQ_OK) {
        fbq_free(disp_src_frame);
    }

    while (1) {
        /* get frame */
        ret = fbq_pop(img_raw_frame_ctrl_disp, &disp_src_frame, img_raw_queue_id_disp, 1000);
        if (ret != FBQ_OK) {
            LOG_W("img_raw pop timeout: %d, continue wait...\r\n", ret);
            continue;
        }
        LOG_D("img_raw pop: id %d, addr 0x%08X,\r\n", disp_src_frame->id, disp_src_frame->data);

        disp_fb = rgb_disp_get_draw_buffer();

        const soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext_const(disp_src_frame);
        uint16_t width = raw_ext->x_end - raw_ext->x_start + 1;
        uint16_t height = raw_ext->y_end - raw_ext->y_start + 1;
        bool match_flag = false;

        /* TODO:  draw */
        if (LCD_COLOR_DEPTH == 32 && raw_ext->format == IMG_RAW_FRAME_FORMAT_NRGB8888) {
            if (width == 640 && height == 480) {
                if (LCD_W == 480 && LCD_H == 272) {
                    soln_nrgb888_640x480_to_nrgb8888_480x272(disp_src_frame->data, disp_fb);
                    match_flag = true;
                }
            }
        } else if (LCD_COLOR_DEPTH == 32 && raw_ext->format == IMG_RAW_FRAME_FORMAT_RGB888) {
            /* */
            if (width == 640 && height == 480) {
                if (LCD_W == 480 && LCD_H == 272) {
                    soln_rgb888_640x480_to_nrgb8888_480x272(disp_src_frame->data, disp_fb);
                    match_flag = true;
                }
            }
        }

        if (match_flag == false) {
            LOG_E("unsupported frame for display, width %d, height %d, format %d\r\n", width, height, raw_ext->format);
        }

        /* free source frame after draw/switch handling */
        fbq_free(disp_src_frame);

        /* draw overlay on the frame buffer */
        soln_rgb_draw_overlay((lcd_color_t *)disp_fb);

#if RGB_DISP_3_BUFFER_EN
        /* wait for the drawn frame to be displayed */
        rgb_disp_flush_wait();
        /* switch to display the drawn frame buffer */
        lcd_screen_switch((lcd_color_t *)disp_fb);
#else
        /* switch to display the drawn frame buffer */
        lcd_screen_switch((lcd_color_t *)disp_fb);
        /* wait for the drawn frame to be displayed */
        rgb_disp_flush_wait();
#endif

        rgb_disp_record_frame();
    }
}

int soln_rgb_disp_task_init(void)
{
    int ret;
    uint32_t img_raw_accept_mask;

    LOG_I("rgb_disp_init\r\n");

    /* lcd init */
    lcd_clear(rgb_screen_buf[0], LCD_COLOR_RGB(0xff, 0x00, 0x00));
    lcd_init(rgb_screen_buf[0]);

    ret = lcd_frame_callback_register(FRAME_INT_TYPE_SWAP, rgb_screen_frame_callback);
    if (ret < 0) {
        LOG_E("lcd frame callback register failed: %d\r\n", ret);
        return -1;
    }

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
    img_raw_frame_ctrl_disp = soln_fbq_vid_raw_remote();
    img_raw_queue_id_disp = CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_ID;
    img_raw_queue_depth_disp = CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_DEPTH;
    img_raw_accept_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_JPEG_DEC;
#elif IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
    img_raw_frame_ctrl_disp = soln_fbq_vid_raw_local();
    img_raw_queue_id_disp = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_ID;
    img_raw_queue_depth_disp = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_DEPTH;
    img_raw_accept_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAPTURE_ALL;
#else
#error "please config img_raw queue for display"
#endif

    /* Create the display FBQ output subscription. */
    if (fbq_output_open(img_raw_frame_ctrl_disp, &img_raw_queue_id_disp,
                        img_raw_queue_depth_disp,
                        img_raw_accept_mask) != FBQ_OK) {
        LOG_E("img_raw frame rgb display out queue create failed\r\n");
        return -1;
    } else {
        LOG_I("img_raw frame rgb display out queue ID: %d\r\n", img_raw_queue_id_disp);
    }

    rgb_disp_semaphore = xSemaphoreCreateCounting(1, (RGB_DISP_3_BUFFER_EN ? 1 : 0));

    /* rgb display task */
    xTaskCreate(rgb_disp_lcd_task, (char *)"disp_rgb_task", 1024, NULL, 10, &rgb_disp_handle);

    return 0;
}
