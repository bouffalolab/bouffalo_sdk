#include "bflb_mtimer.h"
#include "bflb_mjpeg.h"
#include "bflb_l1c.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#include "mjpeg.h"
#include "jpeg_head.h"

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

uint32_t g_mjpeg_total_frame_cnt = 0;

/***** mjpeg ctrl *****/
static TaskHandle_t mjpeg_compress_task_hd;

static struct bflb_device_s *mjpeg;
static struct bflb_mjpeg_config_s config;

static uint16_t yuyv_out_queue_jpeg_id;

/* mjpeg isr */
ATTR_TCM_SECTION static void mjpeg_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg);

    JPEG_DBG("mjpeg_isr 0x%08X\r\n", intstatus);

    if (intstatus & MJPEG_INTSTS_ONE_FRAME) {
        /* clear frame isr */
        bflb_mjpeg_int_clear(mjpeg, MJPEG_INTCLR_ONE_FRAME);
        /* clear over size isr */
        bflb_mjpeg_int_clear(mjpeg, 1 << 10);

        /* notify to task */
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(mjpeg_compress_task_hd, JPEG_FRAME_COMP, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    } else if (intstatus & (1 << 6)) {
        /* over size error */
        /* clear over size isr */
        bflb_mjpeg_int_clear(mjpeg, 1 << 10);

        /* notify to task */
        BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(mjpeg_compress_task_hd, JPEG_OVER_SIZE, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
        if (pxHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        }
    }
}

/* mjpeg init */
void mjpeg_init(uint32_t x, uint32_t y, uint8_t quality)
{
    uint32_t jpg_head_buf[800 / 4] = { 0 };
    uint32_t jpg_head_len;

    JPEG_INFO("mjpeg_init\r\n");

    mjpeg = bflb_device_get_by_name("mjpeg");

    config.format = MJPEG_FORMAT_YUV422_YUYV;
    config.quality = quality;
    config.rows = y;
    config.resolution_x = x;
    config.resolution_y = y;
    config.input_bufaddr0 = (uint32_t)NULL;
    config.input_bufaddr1 = (uint32_t)NULL;
    config.output_bufaddr = (uint32_t)NULL;
    config.output_bufsize = 0;
    config.input_yy_table = NULL; /* use default table */
    config.input_uv_table = NULL; /* use default table */

    bflb_mjpeg_init(mjpeg, &config);
    jpg_head_len = JpegHeadCreate(YUV_MODE_422, quality, x, y, (uint8_t *)jpg_head_buf);
    bflb_mjpeg_fill_jpeg_header_tail(mjpeg, (uint8_t *)jpg_head_buf, jpg_head_len);

    bflb_mjpeg_tcint_mask(mjpeg, false);
    uint32_t ret_temp = getreg32(mjpeg->reg_base + 0x1C);
    putreg32((ret_temp | (1 << 2)), (mjpeg->reg_base + 0x1C)); /* enable over_size int */
    bflb_irq_attach(mjpeg->irq_num, mjpeg_isr, NULL);
    bflb_irq_enable(mjpeg->irq_num);
}

/* jpeg compress task */
static void mjpeg_compress_task(void *pvParameters)
{
    int ret;

    static pyuyv_frame_t yuyv_frame;
    static jpeg_frame_t mjpeg_compress_frame;

    void *yuv_src, *jpeg_dest;
    uint32_t jpeg_buff_size;

    uint8_t *jpeg_pic;
    uint32_t jpeg_len;

    /* clear */
    // ulTaskNotifyTake(pdTRUE, 0);

    while (1) {
        /* pop yuyv frame */
        ret = frame_queue_output_pop(g_yuyv_frame_ctrl, (frame_elem_t *)&yuyv_frame, yuyv_out_queue_jpeg_id, 1000);
        if (ret < 0) {
            JPEG_INFO("yuyv pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        JPEG_DBG("yuyv pop: id %d, addr 0x%08X,\r\n", yuyv_frame.elem_base.frame_id, yuyv_frame.elem_base.frame_addr);

        /* allco frame buff */
        ret = frame_queue_alloc_lock(g_jpeg_frame_ctrl, (frame_elem_t *)&mjpeg_compress_frame, 0);
        if (ret < 0) {
            frame_queue_output_free(g_yuyv_frame_ctrl, (frame_elem_t *)&yuyv_frame);
            JPEG_DBG("jpeg buff alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }

        /* get info */
        yuv_src = yuyv_frame.elem_base.frame_addr;
        jpeg_dest = mjpeg_compress_frame.elem_base.frame_addr;
        jpeg_buff_size = mjpeg_compress_frame.elem_base.frame_size;

        JPEG_DBG("jpeg alloc: id %d, addr 0x%08X, size %d\r\n", mjpeg_compress_frame.elem_base.frame_id, jpeg_dest, jpeg_buff_size);

        /* start mjpeg compress */
        bflb_mjpeg_stop(mjpeg);
        bflb_mjpeg_update_input_output_buff(mjpeg, yuv_src, NULL, jpeg_dest, jpeg_buff_size);
        bflb_mjpeg_sw_run(mjpeg, 1);

        /* waiting jpeg isr */
        int mjpeg_status = ulTaskNotifyTake(pdTRUE, 100);

        /* free the yuyv frame */
        frame_queue_output_free(g_yuyv_frame_ctrl, (frame_elem_t *)&yuyv_frame);
        JPEG_DBG("yuyv free: id %d\r\n", yuyv_frame.elem_base.frame_id);

        if (mjpeg_status == JPEG_FRAME_COMP) {
            /* save info */
            jpeg_len = bflb_mjpeg_get_frame_info(mjpeg, &jpeg_pic);
            bflb_mjpeg_pop_one_frame(mjpeg);
            bflb_mjpeg_stop(mjpeg);

            /* check address */
            if ((void *)jpeg_pic != jpeg_dest) {
                JPEG_ERR("mjpeg output address error! 0x%08X -> 0x%08X \r\n", (uint32_t)jpeg_pic, (uint32_t)jpeg_dest);
                return;
            }

            JPEG_DBG("jpeg size %d\r\n", jpeg_len);

            if (jpeg_len <= jpeg_buff_size) {
                bflb_l1c_dcache_invalidate_range(jpeg_dest, jpeg_len);
                /* update frame len */
                mjpeg_compress_frame.data_size = jpeg_len;
                /* try send to out_queue */
                for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                    ret = frame_queue_push(g_jpeg_frame_ctrl, (frame_elem_t *)&mjpeg_compress_frame, i, 0);
                }
            } else {
                JPEG_ERR("got mjpeg frame ,but jpeg over size\r\n");
            }
        } else {
            if (mjpeg_status == JPEG_OVER_SIZE) {
                JPEG_INFO("mjpeg over size, frame drop\r\n");
            } else {
                JPEG_ERR("mjpeg unkown error %d\r\n", mjpeg_status);
            }
        }

        /* unlock jpeg */
        frame_queue_free_unlock(g_jpeg_frame_ctrl, (frame_elem_t *)&mjpeg_compress_frame);

        g_mjpeg_total_frame_cnt++;
    }
}

/* jpeg task init */
int mjpeg_compress_task_init(uint32_t x, uint32_t y, uint8_t quality)
{
    JPEG_INFO("mjpeg_compress_task_init\r\n");

    /* create yuyv output queue */
    yuyv_out_queue_jpeg_id = YUYV_FRAME_STREAM_JPEG_ID;
    if (frame_queue_output_create(g_yuyv_frame_ctrl, &yuyv_out_queue_jpeg_id, YUYV_FRAME_STREAM_JPEG_DEPTH) < 0) {
        JPEG_ERR("yuyv frame jpeg out queue create failed\r\n");
        return -1;
    } else {
        JPEG_INFO("yuyv frame jpeg out queue ID: %d\r\n", yuyv_out_queue_jpeg_id);
    }

    mjpeg_init(x, y, quality);

    /* creat record  */
    xTaskCreate(mjpeg_compress_task, (char *)"mjpeg_compress_task", 512, NULL, 19, &mjpeg_compress_task_hd);

    return 0;
}