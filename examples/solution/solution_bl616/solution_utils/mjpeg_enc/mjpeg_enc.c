#include "bflb_mtimer.h"
#include "bflb_mjpeg.h"
#include "bflb_l1c.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "mjpeg_enc.h"
#include "jpeg_head.h"

#if !defined(BL616) && !defined(BL616CL)
#error "This example code is only for BL616 and BL616CL"
#endif

#if (0)
#define JPEG_DBG(a, ...) printf("[JPEG dbg]:" a, ##__VA_ARGS__)
#else
#define JPEG_DBG(a, ...)
#endif

#if (1)
#define JPEG_INFO(a, ...) printf("[JPEG info]:" a, ##__VA_ARGS__)
#else
#define JPEG_INFO(a, ...)
#endif

#define JPEG_ERR(a, ...) printf("[JPEG err]:" a, ##__VA_ARGS__)

#define JPEG_FRAME_COMP  (2)
#define JPEG_OVER_SIZE   (3)

uint32_t g_mjpeg_enc_total_frame_cnt = 0;

/***** mjpeg enc ctrl *****/
static TaskHandle_t mjpeg_enc_task_hd;

static struct bflb_device_s *mjpeg_enc_dev;
static struct bflb_mjpeg_config_s mjpeg_enc_cfg;

static uint16_t img_raw_out_queue_mjpeg_enc_id;

/* mjpeg isr */
ATTR_TCM_SECTION static void mjpeg_enc_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg_enc_dev);

    JPEG_DBG("mjpeg_isr 0x%08X\r\n", intstatus);

    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        /* clear frame isr */
        bflb_mjpeg_int_clear(mjpeg_enc_dev, MJPEG_INTCLR_ONE_FRAME);
        /* clear over size isr */
        bflb_mjpeg_int_clear(mjpeg_enc_dev, 1 << 10);

        /* notify to task */
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(mjpeg_enc_task_hd, JPEG_FRAME_COMP, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (intstatus & (1 << 6)) {
        /* over size error */
        /* clear over size isr */
        bflb_mjpeg_int_clear(mjpeg_enc_dev, 1 << 10);

        /* notify to task */
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(mjpeg_enc_task_hd, JPEG_OVER_SIZE, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    }
}

/* mjpeg enc init */
void mjpeg_enc_init(uint32_t x, uint32_t y, uint8_t quality)
{
    uint32_t jpg_head_buf[800 / 4] = { 0 };
    uint32_t jpg_head_len;

    JPEG_INFO("mjpeg_enc_init\r\n");

    mjpeg_enc_dev = bflb_device_get_by_name("mjpeg");

    mjpeg_enc_cfg.format = MJPEG_FORMAT_YUV422_YUYV;
    mjpeg_enc_cfg.quality = quality;
    mjpeg_enc_cfg.rows = y;
    mjpeg_enc_cfg.resolution_x = x;
    mjpeg_enc_cfg.resolution_y = y;
    mjpeg_enc_cfg.input_bufaddr0 = (uint32_t)NULL;
    mjpeg_enc_cfg.input_bufaddr1 = (uint32_t)NULL;
    mjpeg_enc_cfg.output_bufaddr = (uint32_t)NULL;
    mjpeg_enc_cfg.output_bufsize = 0;
    mjpeg_enc_cfg.input_yy_table = NULL; /* use default table */
    mjpeg_enc_cfg.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg_enc_dev, &mjpeg_enc_cfg);
    jpg_head_len = JpegHeadCreate(YUV_MODE_422, quality, x, y, (uint8_t *)jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg_enc_dev, (uint8_t *)jpg_head_buf, jpg_head_len);

    bflb_mjpeg_tcint_mask(mjpeg_enc_dev, false);
    uint32_t ret_temp = getreg32(mjpeg_enc_dev->reg_base + 0x1C);
    putreg32((ret_temp | (1 << 2)), (mjpeg_enc_dev->reg_base + 0x1C)); /* enable over_size int */
    bflb_irq_attach(mjpeg_enc_dev->irq_num, mjpeg_enc_isr, NULL);
    bflb_irq_enable(mjpeg_enc_dev->irq_num);
}

/* mjpeg enc task */
static void mjpeg_enc_task(void *pvParameters)
{
    int ret;

    static pimg_raw_frame_t img_raw_frame;
    static jpeg_frame_t mjpeg_enc_frame;

    void *yuv_src, *jpeg_dest;
    uint32_t jpeg_buff_size;

    uint8_t *jpeg_pic;
    uint32_t jpeg_len;

    /* clear */
    // ulTaskNotifyTake(pdTRUE, 0);

    while (1) {
        /* pop img_raw frame */
        ret = frame_queue_output_pop(g_img_raw_out_frame_ctrl, (frame_elem_t *)&img_raw_frame, img_raw_out_queue_mjpeg_enc_id, 1000);
        if (ret < 0) {
            JPEG_INFO("img_raw pop timeout %d, continue wait... \r\n", ret);
            continue;
        }
        JPEG_DBG("img_raw pop: id %d, addr 0x%08X,\r\n", img_raw_frame.elem_base.frame_id, img_raw_frame.elem_base.frame_addr);

        if (img_raw_frame.format != IMG_RAW_FRAME_FORMAT_YUYV) {
            JPEG_ERR("img_raw format %d is not supported by mjpeg_enc\r\n", img_raw_frame.format);
            frame_queue_output_free(g_img_raw_out_frame_ctrl, (frame_elem_t *)&img_raw_frame);
            continue;
        }

        /* allco frame buff */
        ret = frame_queue_alloc_lock(g_mjpeg_out_frame_ctrl, (frame_elem_t *)&mjpeg_enc_frame, 20);
        if (ret < 0) {
            frame_queue_output_free(g_img_raw_out_frame_ctrl, (frame_elem_t *)&img_raw_frame);
            JPEG_DBG("jpeg buff alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        /* get info */
        yuv_src = img_raw_frame.elem_base.frame_addr;
        jpeg_dest = mjpeg_enc_frame.elem_base.frame_addr;
        jpeg_buff_size = mjpeg_enc_frame.elem_base.frame_size;

        JPEG_DBG("jpeg alloc: id %d, addr 0x%08X, size %d\r\n", mjpeg_enc_frame.elem_base.frame_id, jpeg_dest, jpeg_buff_size);

        /* start mjpeg enc */
        bflb_mjpeg_stop(mjpeg_enc_dev);
        bflb_mjpeg_update_input_output_buff(mjpeg_enc_dev, yuv_src, NULL, jpeg_dest, jpeg_buff_size);
        bflb_mjpeg_sw_run(mjpeg_enc_dev, 1);

        /* waiting jpeg isr */
        int mjpeg_enc_status = ulTaskNotifyTake(pdTRUE, 100);

        /* free the img_raw frame */
        frame_queue_output_free(g_img_raw_out_frame_ctrl, (frame_elem_t *)&img_raw_frame);
        JPEG_DBG("img_raw free: id %d\r\n", img_raw_frame.elem_base.frame_id);

        if (mjpeg_enc_status == JPEG_FRAME_COMP) {
            /* save info */
            jpeg_len = bflb_mjpeg_get_frame_info(mjpeg_enc_dev, &jpeg_pic);
            bflb_mjpeg_pop_one_frame(mjpeg_enc_dev);
            bflb_mjpeg_stop(mjpeg_enc_dev);

            /* check address */
            if ((void *)jpeg_pic != jpeg_dest) {
                JPEG_ERR("mjpeg output address error! 0x%08X -> 0x%08X \r\n", (uint32_t)jpeg_pic, (uint32_t)jpeg_dest);
                return;
            }

            JPEG_DBG("jpeg size %d\r\n", jpeg_len);

            if (jpeg_len <= jpeg_buff_size) {
                bflb_l1c_dcache_invalidate_range(jpeg_dest, jpeg_len);
                /* update frame len */
                mjpeg_enc_frame.data_size = jpeg_len;
                /* try send to out_queue */
                for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                    ret = frame_queue_push(g_mjpeg_out_frame_ctrl, (frame_elem_t *)&mjpeg_enc_frame, i, 0);
                }
            } else {
                JPEG_ERR("got mjpeg frame ,but jpeg over size\r\n");
            }
        } else {
            if (mjpeg_enc_status == JPEG_OVER_SIZE) {
                JPEG_INFO("mjpeg over size, frame drop\r\n");
            } else {
                JPEG_ERR("mjpeg unkown error %d\r\n", mjpeg_enc_status);
            }
        }

        /* unlock jpeg */
        frame_queue_free_unlock(g_mjpeg_out_frame_ctrl, (frame_elem_t *)&mjpeg_enc_frame);

        g_mjpeg_enc_total_frame_cnt++;
    }
}

/* mjpeg enc task init */
int mjpeg_enc_task_init(uint32_t x, uint32_t y, uint8_t quality)
{
    JPEG_INFO("mjpeg_enc_task_init\r\n");

    /* create img_raw output queue */
    img_raw_out_queue_mjpeg_enc_id = IMG_RAW_OUT_FRAME_STREAM_JPEG_ID;
    if (frame_queue_output_create(g_img_raw_out_frame_ctrl, &img_raw_out_queue_mjpeg_enc_id, IMG_RAW_OUT_FRAME_STREAM_JPEG_DEPTH) < 0) {
        JPEG_ERR("img_raw frame mjpeg_enc out queue create failed\r\n");
        return -1;
    } else {
        JPEG_INFO("img_raw frame mjpeg_enc out queue ID: %d\r\n", img_raw_out_queue_mjpeg_enc_id);
    }

    mjpeg_enc_init(x, y, quality);

    /* create mjpeg enc task */
    xTaskCreate(mjpeg_enc_task, (char *)"mjpeg_enc_task", 384, NULL, 25, &mjpeg_enc_task_hd);

    return 0;
}
