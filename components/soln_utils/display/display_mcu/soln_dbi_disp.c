#include "bflb_core.h"
#include "bflb_irq.h"

#include <FreeRTOS.h>
#include <task.h>

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
    const display_info_t *info = soln_disp_dbi_info();

    (void)pvParameters;
    soln_disp_draw_text(10, 10, 0xffff8080, 0xff000000, "Wireless picture transmission test @BouffaloLab");
    soln_disp_draw_text(10, 30, 0xffa0a0ff, 0xff000000, "display init ok...");
    soln_disp_draw_text(10, 50, 0xff00ff00, 0xff000000, "wait disp frame src...");

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

        ret = soln_disp_flush_frame(disp_src_frame);
        if (ret) {
            LOG_E("display frame failed: %d\r\n", ret);
        } else {
            dbi_disp_record_frame();
        }

        /* free frame */
        LOG_D("img_raw free: id %d, \r\n", disp_src_frame->id);
        fbq_free(disp_src_frame);

    update_fps:

#if LCD_INFO_DISP_ENABLE
        ret = soln_fps_str_get(str_buff_total, sizeof(str_buff_total));
        if (ret < 0) {
            snprintf(str_buff_total, sizeof(str_buff_total), "FPS: get failed");
        }
        uint16_t bottom = ret >= 0 && (ret + 1u) * 8u >= info->width ? 40u : 20u;
        if (info->height > bottom) {
            soln_disp_draw_text(4, info->height - bottom, 0xffff8080, 0xff404040, str_buff_total);
        }
#endif
    }
}

int soln_dbi_disp_task_init(void)
{
    uint32_t img_raw_accept_mask;
    int ret;

    LOG_I("dbi_disp_init\r\n");

    ret = soln_disp_dbi_init();
    if (ret) {
        LOG_E("display init failed: %d\r\n", ret);
        return ret;
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
        LOG_E("img_raw frame display out queue create failed\r\n");
        return -1;
    } else {
        LOG_I("img_raw frame display out queue ID: %d\r\n", img_raw_queue_id_disp);
    }

    /* dbi display task */
    if (xTaskCreate(dbi_disp_lcd_task, (char *)"disp_dbi_task", 512, NULL, 10, &dbi_disp_handle) != pdPASS) {
        fbq_output_close(img_raw_frame_ctrl_disp, img_raw_queue_id_disp);
        return -ENOMEM;
    }

    return 0;
}
