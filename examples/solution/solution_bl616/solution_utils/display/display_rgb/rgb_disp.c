#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "lcd.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "rgb_disp.h"
#include "rgb_draw.h"

#if (LCD_INTERFACE_TYPE != LCD_INTERFACE_DPI)
#error "The example code is only for LCD with RGB interface, please check your LCD config in menuconfig"
#endif

#if (0)
#define DISP_DBG(a, ...) printf("[DISP dbg]:" a, ##__VA_ARGS__)
#else
#define DISP_DBG(a, ...)
#endif

#if (1)
#define DISP_INFO(a, ...) printf("[DISP info]:" a, ##__VA_ARGS__)
#else
#define DISP_INFO(a, ...)
#endif

#define DISP_ERR(a, ...) printf("[DISP err]:" a, ##__VA_ARGS__)

#ifdef CONFIG_PSRAM
#define RGB_FB_ATTR __ALIGNED(64) ATTR_NOINIT_PSRAM_SECTION
#else
#define RGB_FB_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#endif

/* */
#define RGB_DISP_3_BUFFER_EN 1

static frame_queue_ctrl_t *img_raw_frame_ctrl_disp = NULL;
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

uint32_t g_rgb_disp_total_frame_cnt = 0;

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
    pimg_raw_frame_t disp_src_frame;

    (void)pvParameters;

    vTaskDelay(100);

    /* drop the first frame */
    ret = frame_queue_output_pop(img_raw_frame_ctrl_disp, (frame_elem_t *)&disp_src_frame, img_raw_queue_id_disp, 0);
    if (ret >= 0) {
        frame_queue_output_free(img_raw_frame_ctrl_disp, (frame_elem_t *)&disp_src_frame);
    }

    while (1) {
        /* get frame */
        ret = frame_queue_output_pop(img_raw_frame_ctrl_disp, (frame_elem_t *)&disp_src_frame, img_raw_queue_id_disp, 1000);
        if (ret < 0) {
            DISP_INFO("img_raw pop timeout: %d, continue wait...\r\n", ret);
            continue;
        }
        DISP_DBG("img_raw pop: id %d, addr 0x%08X,\r\n", disp_src_frame.elem_base.frame_id, disp_src_frame.elem_base.frame_addr);

        disp_fb = rgb_disp_get_draw_buffer();

        uint16_t width = disp_src_frame.x_end - disp_src_frame.x_start + 1;
        uint16_t height = disp_src_frame.y_end - disp_src_frame.y_start + 1;
        bool match_flag = false;

        /* TODO:  draw */
        if (LCD_COLOR_DEPTH == 32 && disp_src_frame.format == IMG_RAW_FRAME_FORMAT_NRGB8888) {
            if (width == 640 && height == 480) {
                if (LCD_W == 480 && LCD_H == 272) {
                    nrgb888_640x480_to_nrgb8888_480x272(disp_src_frame.elem_base.frame_addr, disp_fb);
                    match_flag = true;
                }
            }
        } else if (LCD_COLOR_DEPTH == 32 && disp_src_frame.format == IMG_RAW_FRAME_FORMAT_RGB888) {
            /* */
            if (width == 640 && height == 480) {
                if (LCD_W == 480 && LCD_H == 272) {
                    rgb888_640x480_to_nrgb8888_480x272(disp_src_frame.elem_base.frame_addr, disp_fb);
                    match_flag = true;
                }
            }
        }

        if (match_flag == false) {
            DISP_ERR("unsupported frame for display, width %d, height %d, format %d\r\n", width, height, disp_src_frame.format);
        }

        /* free source frame after draw/switch handling */
        frame_queue_output_free(img_raw_frame_ctrl_disp, (frame_elem_t *)&disp_src_frame);

        /* draw overlay on the frame buffer */
        rgb_draw_overlay((lcd_color_t *)disp_fb);

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

        g_rgb_disp_total_frame_cnt++;
    }
}

int rgb_disp_task_init(void)
{
    int ret;

    DISP_INFO("rgb_disp_init\r\n");

    /* lcd init */
    lcd_clear(rgb_screen_buf[0], LCD_COLOR_RGB(0xff, 0x00, 0x00));
    lcd_init(rgb_screen_buf[0]);

    ret = lcd_frame_callback_register(FRAME_INT_TYPE_SWAP, rgb_screen_frame_callback);
    if (ret < 0) {
        DISP_ERR("lcd frame callback register failed: %d\r\n", ret);
        return -1;
    }

#if IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_IN)
    img_raw_frame_ctrl_disp = g_img_raw_in_frame_ctrl;
    img_raw_queue_id_disp = IMG_RAW_IN_FRAME_STREAM_DISP_ID;
    img_raw_queue_depth_disp = IMG_RAW_IN_FRAME_STREAM_DISP_DEPTH;
#elif IS_ENABLED(CONFIG_SOLUTION_QUEUE_IMG_RAW_OUT)
    img_raw_frame_ctrl_disp = g_img_raw_out_frame_ctrl;
    img_raw_queue_id_disp = IMG_RAW_OUT_FRAME_STREAM_DISP_ID;
    img_raw_queue_depth_disp = IMG_RAW_OUT_FRAME_STREAM_DISP_DEPTH;
#else
#error "please config img_raw queue for display"
#endif

    /* create img_raw output queue */
    if (frame_queue_output_create(img_raw_frame_ctrl_disp, &img_raw_queue_id_disp, img_raw_queue_depth_disp) < 0) {
        DISP_ERR("img_raw frame rgb display out queue create failed\r\n");
        return -1;
    } else {
        DISP_INFO("img_raw frame rgb display out queue ID: %d\r\n", img_raw_queue_id_disp);
    }

    rgb_disp_semaphore = xSemaphoreCreateCounting(1, (RGB_DISP_3_BUFFER_EN ? 1 : 0));

    /* rgb display task */
    xTaskCreate(rgb_disp_lcd_task, (char *)"disp_rgb_task", 1024, NULL, 10, &rgb_disp_handle);

    return 0;
}
