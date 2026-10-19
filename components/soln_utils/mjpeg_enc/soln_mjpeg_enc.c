#include "bflb_mtimer.h"
#include "bflb_irq.h"
#include "bflb_mjpeg.h"
#include "bflb_l1c.h"

#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <queue.h>

#include "soln_fbq.h"

#include "soln_mjpeg_enc.h"
#include "soln_jpeg_head.h"

#ifdef CONFIG_SOLN_VID_JPEG_ENC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_VID_JPEG_ENC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_MJPEG_ENC"
#include "log.h"

#if !defined(BL616) && !defined(BL616CL)
#error "This example code is only for BL616 and BL616CL"
#endif

#define JPEG_FRAME_COMP (2)
#define JPEG_OVER_SIZE  (3)

static uint32_t s_mjpeg_enc_total_frame_count = 0;

uint32_t soln_mjpeg_enc_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_mjpeg_enc_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_mjpeg_enc_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_mjpeg_enc_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void mjpeg_enc_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_mjpeg_enc_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

/***** mjpeg enc ctrl *****/
static TaskHandle_t mjpeg_enc_task_hd;

static struct bflb_device_s *mjpeg_enc_dev;
static struct bflb_mjpeg_config_s mjpeg_enc_cfg;

static uint16_t local_raw_output_enc_id;

/* mjpeg isr */
ATTR_TCM_SECTION static void mjpeg_enc_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_mjpeg_get_intstatus(mjpeg_enc_dev);

    LOG_D("mjpeg_isr 0x%08X\r\n", intstatus);

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
static void mjpeg_enc_init(uint32_t x, uint32_t y, uint8_t quality)
{
    uint32_t jpg_head_buf[800 / 4] = { 0 };
    uint32_t jpg_head_len;

    LOG_I("mjpeg_enc_init\r\n");

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
    jpg_head_len = soln_jpeg_head_create(YUV_MODE_422, quality, x, y, (uint8_t *)jpg_head_buf);
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
    fbq_elem_t *img_raw_frame;
    fbq_elem_t *mjpeg_enc_frame;

    void *yuv_src, *jpeg_dest;
    uint32_t jpeg_buff_size;

    uint8_t *jpeg_pic;
    uint32_t jpeg_len;

    /* clear */
    // ulTaskNotifyTake(pdTRUE, 0);

    while (1) {
        /* pop img_raw frame */
        ret = fbq_pop(soln_fbq_vid_raw_local(), &img_raw_frame, local_raw_output_enc_id, 1000);
        if (ret != FBQ_OK) {
            LOG_W("img_raw pop timeout %d, continue wait... \r\n", ret);
            continue;
        }
        LOG_D("img_raw pop: id %d, addr 0x%08X,\r\n", img_raw_frame->id, img_raw_frame->data);

        const soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext_const(img_raw_frame);
        if (raw_ext->format != IMG_RAW_FRAME_FORMAT_YUYV) {
            LOG_E("img_raw format %d is not supported by mjpeg_enc\r\n", raw_ext->format);
            fbq_free(img_raw_frame);
            continue;
        }

        /* allco frame buff */
        ret = fbq_alloc(soln_fbq_vid_jpeg_local(), &mjpeg_enc_frame, 20);
        if (ret != FBQ_OK) {
            fbq_free(img_raw_frame);
            LOG_D("jpeg buff alloc timeout %d, continue wait... \r\n", ret);
            continue;
        }
        mjpeg_enc_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_JPEG_ENC;

        /* get info */
        yuv_src = img_raw_frame->data;
        jpeg_dest = mjpeg_enc_frame->data;
        jpeg_buff_size = mjpeg_enc_frame->capacity;

        LOG_D("jpeg alloc: id %d, addr 0x%08X, size %d\r\n", mjpeg_enc_frame->id, jpeg_dest, jpeg_buff_size);

        /* start mjpeg enc */
        bflb_mjpeg_stop(mjpeg_enc_dev);
        bflb_mjpeg_update_input_output_buff(mjpeg_enc_dev, yuv_src, NULL, jpeg_dest, jpeg_buff_size);
        bflb_mjpeg_sw_run(mjpeg_enc_dev, 1);

        /* waiting jpeg isr */
        int mjpeg_enc_status = ulTaskNotifyTake(pdTRUE, 100);

        /* free the img_raw frame */
        LOG_D("img_raw free: id %d\r\n", img_raw_frame->id);
        fbq_free(img_raw_frame);

        if (mjpeg_enc_status == JPEG_FRAME_COMP) {
            /* save info */
            jpeg_len = bflb_mjpeg_get_frame_info(mjpeg_enc_dev, &jpeg_pic);
            bflb_mjpeg_pop_one_frame(mjpeg_enc_dev);
            bflb_mjpeg_stop(mjpeg_enc_dev);

            /* check address */
            if ((void *)jpeg_pic != jpeg_dest) {
                LOG_E("mjpeg output address error! 0x%08X -> 0x%08X \r\n", (uint32_t)jpeg_pic, (uint32_t)jpeg_dest);
            } else if (jpeg_len <= jpeg_buff_size) {
                LOG_D("jpeg size %d\r\n", jpeg_len);
                bflb_l1c_dcache_invalidate_range(jpeg_dest, jpeg_len);
                mjpeg_enc_frame->size = jpeg_len;
                (void)fbq_push_mask(soln_fbq_vid_jpeg_local(), mjpeg_enc_frame, FBQ_OUTPUT_ALL, 0);
            } else {
                LOG_E("got mjpeg frame, but jpeg over size\r\n");
            }
        } else {
            if (mjpeg_enc_status == JPEG_OVER_SIZE) {
                LOG_W("mjpeg over size, frame drop\r\n");
            } else {
                LOG_E("mjpeg unknown error %d\r\n", mjpeg_enc_status);
            }
        }

        fbq_free(mjpeg_enc_frame);

        mjpeg_enc_record_frame();
    }
}

/* mjpeg enc task init */
int soln_mjpeg_enc_task_init(uint32_t x, uint32_t y, uint8_t quality)
{
    LOG_I("soln_mjpeg_enc_task_init\r\n");

    /* Create the encoder output subscription on the local RAW queue. */
    local_raw_output_enc_id = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_ENC_ID;
    if (fbq_output_open(soln_fbq_vid_raw_local(), &local_raw_output_enc_id,
                        CONFIG_SOLN_FBQ_VID_RAW_LOCAL_ENC_DEPTH,
                        SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAPTURE_ALL) != FBQ_OK) {
        LOG_E("local RAW encoder output create failed\r\n");
        return -1;
    } else {
        LOG_I("local RAW encoder output ID: %d\r\n", local_raw_output_enc_id);
    }

    mjpeg_enc_init(x, y, quality);

    /* create mjpeg enc task */
    xTaskCreate(mjpeg_enc_task, (char *)"mjpeg_enc_task", 384, NULL, 25, &mjpeg_enc_task_hd);

    return 0;
}
