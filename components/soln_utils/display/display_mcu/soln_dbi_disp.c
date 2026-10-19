#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "bflb_gpio.h"
#include "bflb_dbi.h"
#include "bflb_sf_ctrl.h"
#include "hardware/dbi_reg.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "lcd.h"

#include "soln_fbq.h"

#include "soln_dbi_disp.h"
#include "soln_display_flush.h"

#include "soln_solution.h"

#ifdef CONFIG_SOLN_DISP_DBI_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_DISP_DBI_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_DBI"
#include "log.h"

#if !defined(LCD_DBI_INTERFACE_TYPE)
#error "The example code is only for LCD with DBI interface, please check your LCD config in menuconfig"
#endif

static fbq_ctrl_t *img_raw_frame_ctrl_disp = NULL;
static uint16_t img_raw_queue_id_disp;
static uint16_t img_raw_queue_depth_disp;

static TaskHandle_t dbi_disp_handle;

static uint32_t s_dbi_disp_total_frame_count = 0;

uint32_t soln_dbi_disp_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_dbi_disp_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_dbi_disp_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_dbi_disp_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void dbi_disp_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_dbi_disp_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

static ATTR_TCM_SECTION void dbi_disp_lcd_task(void *pvParameters)
{
    int ret;
    fbq_elem_t *disp_src_frame;
    char str_buff_total[128];

    lcd_draw_str_ascii16(10, 10, LCD_COLOR_RGB(0xff, 0x80, 0x80), LCD_COLOR_RGB(0, 0, 0), (void *)"Wireless picture transmission test @BouffaloLab", 100);
    lcd_draw_str_ascii16(10, 30, LCD_COLOR_RGB(0xa0, 0xa0, 0xff), LCD_COLOR_RGB(0, 0, 0), (void *)"lcd init ok...", 100);
    lcd_draw_str_ascii16(10, 50, LCD_COLOR_RGB(0, 0xff, 0), LCD_COLOR_RGB(0, 0, 0), (void *)"wait disp frame src...", 100);

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
            goto update_fps;
        }
        LOG_D("img_raw pop: id %d, addr 0x%08X,\r\n", disp_src_frame->id, disp_src_frame->data);

        const soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext_const(disp_src_frame);
        uint16_t width = raw_ext->x_end - raw_ext->x_start + 1;
        uint16_t height = raw_ext->y_end - raw_ext->y_start + 1;

#if defined(BL616CL)
        if (width <= (lcd_max_x + 1) && height <= (lcd_max_y + 1)) {
            /* direct flush, center */
            soln_disp_flush_dma_blocking((lcd_max_x - width) / 2, (lcd_max_y - height) / 2,
                                         (lcd_max_x - width) / 2 + width - 1, (lcd_max_y - height) / 2 + height - 1,
                                         disp_src_frame->data, raw_ext->format);
        } else if ((width == 640) && (height == 480) &&
                   (lcd_max_x + 1 >= 480) && (lcd_max_y + 1 >= 320) &&
                   (raw_ext->format == IMG_RAW_FRAME_FORMAT_YUYV)) {
            /* special case for 640x480 yuv422 */
            soln_yuyv422_640x480_to_yuv444_480x320_to_dbi_fast(disp_src_frame->data, 0, height);
        } else {
            LOG_E("unsupported frame for display, width %d, height %d, format %d\r\n", width, height, raw_ext->format);
        }
#elif defined(BL616)
        if (width <= (lcd_max_x + 1) && height <= (lcd_max_y + 1) &&
            (raw_ext->format == IMG_RAW_FRAME_FORMAT_RGB565 ||
             raw_ext->format == IMG_RAW_FRAME_FORMAT_RGB888 ||
             raw_ext->format == IMG_RAW_FRAME_FORMAT_NRGB8888 ||
             raw_ext->format == IMG_RAW_FRAME_FORMAT_YUV)) {
            /* direct flush with dma, center */
            soln_disp_flush_dma_blocking((lcd_max_x - width) / 2, (lcd_max_y - height) / 2,
                                         (lcd_max_x - width) / 2 + width - 1, (lcd_max_y - height) / 2 + height - 1,
                                         disp_src_frame->data, raw_ext->format);
        } else if (width <= (lcd_max_x + 1) && height <= (lcd_max_y + 1) &&
                   (raw_ext->format == IMG_RAW_FRAME_FORMAT_YUYV)) {
            /* direct flush with cpu convert, center */
            soln_yuyv422_to_yuv444_to_dbi_fast((lcd_max_x - width) / 2, (lcd_max_y - height) / 2,
                                               (lcd_max_x - width) / 2 + width - 1, (lcd_max_y - height) / 2 + height - 1,
                                               disp_src_frame->data);

        } else if ((width == 640) && (height == 480) &&
                   (lcd_max_x + 1 >= 480) && (lcd_max_y + 1 >= 320) &&
                   (raw_ext->format == IMG_RAW_FRAME_FORMAT_YUYV)) {
            /* special case for 640x480 yuv422 */
            soln_yuyv422_640x480_to_yuv444_480x320_to_dbi_fast(disp_src_frame->data, 0, height);
        } else {
            LOG_E("unsupported frame for display, width %d, height %d, format %d\r\n", width, height, raw_ext->format);
        }
#else
        LOG_E("unsupported platform for display\r\n");
#endif

        /* free frame */
        LOG_D("img_raw free: id %d, \r\n", disp_src_frame->id);
        fbq_free(disp_src_frame);

        dbi_disp_record_frame();

    update_fps:

#if LCD_INFO_DISP_ENABLE
        ret = soln_fps_str_get(str_buff_total, sizeof(str_buff_total));
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

int soln_dbi_disp_task_init(void)
{
    uint32_t img_raw_accept_mask;

    LOG_I("dbi_disp_init\r\n");

    /* lcd init */
    soln_disp_dbi_init();

    /* set lcd dir, option */
    lcd_set_dir(1, 0);

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
        LOG_E("img_raw frame display out queue create failed\r\n");
        return -1;
    } else {
        LOG_I("img_raw frame display out queue ID: %d\r\n", img_raw_queue_id_disp);
    }

    /* dbi display task */
    xTaskCreate(dbi_disp_lcd_task, (char *)"disp_dbi_task", 512, NULL, 10, &dbi_disp_handle);

    return 0;
}
