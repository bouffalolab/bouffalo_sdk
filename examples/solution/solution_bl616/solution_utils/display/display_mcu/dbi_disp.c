#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_dbi.h"
#include "bflb_sf_ctrl.h"
#include "hardware/dbi_reg.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "lcd.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "dbi_disp.h"
#include "display_flush.h"

#include "solution.h"

#if !defined(LCD_DBI_INTERFACE_TYPE)
#error "The example code is only for LCD with DBI interface, please check your LCD config in menuconfig"
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

static frame_queue_ctrl_t *img_raw_frame_ctrl_disp = NULL;
static uint16_t img_raw_queue_id_disp;
static uint16_t img_raw_queue_depth_disp;

static TaskHandle_t dbi_disp_handle;

uint32_t g_dbi_disp_total_frame_cnt = 0;

static ATTR_TCM_SECTION void dbi_disp_lcd_task(void *pvParameters)
{
    int ret;
    pimg_raw_frame_t disp_src_frame;
    char str_buff_total[128];

    lcd_draw_str_ascii16(10, 10, LCD_COLOR_RGB(0xff, 0x80, 0x80), LCD_COLOR_RGB(0, 0, 0), (void *)"Wireless picture transmission test @BouffaloLab", 100);
    lcd_draw_str_ascii16(10, 30, LCD_COLOR_RGB(0xa0, 0xa0, 0xff), LCD_COLOR_RGB(0, 0, 0), (void *)"lcd init ok...", 100);
    lcd_draw_str_ascii16(10, 50, LCD_COLOR_RGB(0, 0xff, 0), LCD_COLOR_RGB(0, 0, 0), (void *)"wait disp frame src...", 100);

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
            goto update_fps;
        }
        DISP_DBG("img_raw pop: id %d, addr 0x%08X,\r\n", disp_src_frame.elem_base.frame_id, disp_src_frame.elem_base.frame_addr);

        uint16_t width = disp_src_frame.x_end - disp_src_frame.x_start + 1;
        uint16_t height = disp_src_frame.y_end - disp_src_frame.y_start + 1;

#if defined(BL616CL)
        if (width <= (lcd_max_x + 1) && height <= (lcd_max_y + 1)) {
            /* direct flush, center */
            disp_flush_dma_blocking((lcd_max_x - width) / 2, (lcd_max_y - height) / 2,
                                    (lcd_max_x - width) / 2 + width - 1, (lcd_max_y - height) / 2 + height - 1,
                                    disp_src_frame.elem_base.frame_addr, disp_src_frame.format);
        } else if ((width == 640) && (height == 480) &&
                   (lcd_max_x + 1 >= 480) && (lcd_max_y + 1 >= 320) &&
                   (disp_src_frame.format == IMG_RAW_FRAME_FORMAT_YUYV)) {
            /* special case for 640x480 yuv422 */
            yuyv422_640x480_to_yuv444_480x320_to_dbi_fast(disp_src_frame.elem_base.frame_addr, 0, height);
        } else {
            DISP_ERR("unsupported frame for display, width %d, height %d, format %d\r\n", width, height, disp_src_frame.format);
        }
#elif defined(BL616)
        if (width <= (lcd_max_x + 1) && height <= (lcd_max_y + 1) &&
            (disp_src_frame.format == IMG_RAW_FRAME_FORMAT_RGB565 ||
             disp_src_frame.format == IMG_RAW_FRAME_FORMAT_RGB888 ||
             disp_src_frame.format == IMG_RAW_FRAME_FORMAT_NRGB8888 ||
             disp_src_frame.format == IMG_RAW_FRAME_FORMAT_YUV)) {
            /* direct flush with dma, center */
            disp_flush_dma_blocking((lcd_max_x - width) / 2, (lcd_max_y - height) / 2,
                                    (lcd_max_x - width) / 2 + width - 1, (lcd_max_y - height) / 2 + height - 1,
                                    disp_src_frame.elem_base.frame_addr, disp_src_frame.format);
        } else if (width <= (lcd_max_x + 1) && height <= (lcd_max_y + 1) &&
                   (disp_src_frame.format == IMG_RAW_FRAME_FORMAT_YUYV)) {
            /* direct flush with cpu convert, center */
            yuyv422_to_yuv444_to_dbi_fast((lcd_max_x - width) / 2, (lcd_max_y - height) / 2,
                                          (lcd_max_x - width) / 2 + width - 1, (lcd_max_y - height) / 2 + height - 1,
                                          disp_src_frame.elem_base.frame_addr);

        } else if ((width == 640) && (height == 480) &&
                   (lcd_max_x + 1 >= 480) && (lcd_max_y + 1 >= 320) &&
                   (disp_src_frame.format == IMG_RAW_FRAME_FORMAT_YUYV)) {
            /* special case for 640x480 yuv422 */
            yuyv422_640x480_to_yuv444_480x320_to_dbi_fast(disp_src_frame.elem_base.frame_addr, 0, height);
        } else {
            DISP_ERR("unsupported frame for display, width %d, height %d, format %d\r\n", width, height, disp_src_frame.format);
        }
#else
        DISP_ERR("unsupported platform for display\r\n");
#endif

        /* free frame */
        frame_queue_output_free(img_raw_frame_ctrl_disp, (frame_elem_t *)&disp_src_frame);
        DISP_DBG("img_raw free: id %d, \r\n", disp_src_frame.elem_base.frame_id);

        g_dbi_disp_total_frame_cnt++;

    update_fps:

#if LCD_INFO_DISP_ENABLE
        ret = solution_fps_str_get(str_buff_total, sizeof(str_buff_total));
        if (ret < 0) {
            snprintf(str_buff_total, sizeof(str_buff_total), "FPS: get failed");
        } else if ((ret + 1) * 8 < lcd_max_x) {
            lcd_draw_str_ascii16(4, lcd_max_y - 20, LCD_COLOR_RGB(0xff, 0x80, 0x80), LCD_COLOR_RGB(0x40, 0x40, 0x40), (void *)str_buff_total, ret);
        } else {
            lcd_draw_str_ascii16(4, lcd_max_y - 40, LCD_COLOR_RGB(0xff, 0x80, 0x80), LCD_COLOR_RGB(0x40, 0x40, 0x40), (void *)str_buff_total, ret);
        }
#endif
    }
}

int dbi_disp_task_init(void)
{
    DISP_INFO("dbi_disp_init\r\n");

    /* lcd init */
    disp_dbi_init();

    /* set lcd dir, option */
    lcd_set_dir(1, 0);

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
        DISP_ERR("img_raw frame display out queue create failed\r\n");
        return -1;
    } else {
        DISP_INFO("img_raw frame display out queue ID: %d\r\n", img_raw_queue_id_disp);
    }

    /* dbi display task */
    xTaskCreate(dbi_disp_lcd_task, (char *)"disp_dbi_task", 512, NULL, 10, &dbi_disp_handle);

    return 0;
}
