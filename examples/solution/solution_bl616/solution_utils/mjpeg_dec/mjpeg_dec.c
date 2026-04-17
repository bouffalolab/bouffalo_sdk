#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bflb_clock.h"
#include "bflb_irq.h"
#include "bflb_l1c.h"
#include "bflb_mjdec_v2.h"
#include "bflb_name.h"
#include "bflb_peri.h"

#include <FreeRTOS.h>
#include <task.h>

#include "frame_queue.h"
#include "frame_queue_ctrl.h"

#include "mjpeg_dec.h"

#if !defined(BL616CL)
#error "This example code is only for BL616CL"
#endif

#if (0)
#define JPEG_DBG(a, ...) printf("[MJDEC dbg]:" a, ##__VA_ARGS__)
#else
#define JPEG_DBG(a, ...)
#endif

#if (1)
#define JPEG_INFO(a, ...) printf("[MJDEC info]:" a, ##__VA_ARGS__)
#else
#define JPEG_INFO(a, ...)
#endif

#define JPEG_ERR(a, ...)          printf("[MJDEC err]:" a, ##__VA_ARGS__)

#define MJPEG_DEC_FRAME_DONE      (1)
#define MJPEG_DEC_FRAME_ERROR     (2)
#define MJPEG_DEC_WAIT_TIMEOUT_MS (200)

typedef struct {
    uint8_t mjdec_format;
    img_raw_frame_format_t img_raw_format;
    uint8_t pixel_size;
    const char *name;
} mjpeg_dec_output_cfg_t;

static mjpeg_dec_output_cfg_t g_output_cfg = { 0 };

uint32_t g_mjpeg_dec_total_frame_cnt = 0;

static TaskHandle_t mjpeg_dec_task_hd = NULL;
static struct bflb_device_s *mjpeg_dec_dev = NULL;
static volatile uint32_t mjpeg_dec_intstatus = 0;

static uint16_t mjpeg_in_out_queue_mjpeg_dec_id = MJPEG_IN_FRAME_STREAM_DEC_ID;

static void mjpeg_dec_isr(int irq, void *arg)
{
    (void)irq;
    (void)arg;

    if (mjpeg_dec_dev == NULL) {
        return;
    }

    uint32_t intstatus = bflb_mjdec_get_intstatus(mjpeg_dec_dev);
    bflb_mjdec_int_clear(mjpeg_dec_dev, intstatus);

    mjpeg_dec_intstatus |= intstatus;

    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    if (intstatus & MJDEC_INT_ONE_FRAME) {
        xTaskNotifyFromISR(mjpeg_dec_task_hd, MJPEG_DEC_FRAME_DONE, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
    } else {
        xTaskNotifyFromISR(mjpeg_dec_task_hd, MJPEG_DEC_FRAME_ERROR, eSetValueWithOverwrite, &pxHigherPriorityTaskWoken);
    }
    if (pxHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

static int parse_jpeg_size(const uint8_t *data, size_t len, uint16_t *w, uint16_t *h)
{
    if ((data == NULL) || (w == NULL) || (h == NULL)) {
        return -1;
    }

    if (len < 64U) {
        return -1;
    }

    if ((data[0] != 0xFFU) || (data[1] != 0xD8U) || (data[2] != 0xFFU)) {
        return -1;
    }

    size_t i = 2U;
    while (i + 3U < len) {
        if (data[i] != 0xFFU) {
            i++;
            continue;
        }

        while ((i < len) && (data[i] == 0xFFU)) {
            i++;
        }
        if (i >= len) {
            break;
        }

        uint8_t marker = data[i++];
        if ((marker == 0xD8U) || (marker == 0xD9U) || (marker == 0x01U)) {
            continue;
        }
        if ((marker >= 0xD0U) && (marker <= 0xD7U)) {
            continue;
        }

        if (i + 1U >= len) {
            break;
        }

        uint16_t seg_len = ((uint16_t)data[i] << 8) | data[i + 1U];
        if (seg_len < 2U) {
            return -1;
        }

        if ((marker == 0xC0U) || (marker == 0xC1U) || (marker == 0xC2U)) {
            if (i + 7U >= len) {
                return -1;
            }

            *h = ((uint16_t)data[i + 3U] << 8) | data[i + 4U];
            *w = ((uint16_t)data[i + 5U] << 8) | data[i + 6U];

            return (*w > 0U) && (*h > 0U) ? 0 : -1;
        }

        i += seg_len;
    }

    return -1;
}

static int mjpeg_dec_decode_frame(void *jpeg_src_addr, void *raw_dst_addr, uint8_t output_format)
{
    if ((jpeg_src_addr == NULL) || (raw_dst_addr == NULL)) {
        JPEG_ERR("decode frame addr invalid\r\n");
        return -1;
    }

    struct bflb_mjdec_config_s cfg = {
        .format = output_format,
        .alpha = 0xff,
        .has_header = true,
        .parse_header = true,
        .input_bufaddr = (uint32_t)(uintptr_t)jpeg_src_addr,
        .output_bufaddr0 = (uint32_t)(uintptr_t)raw_dst_addr,
        .output_bufaddr1 = 0,
        .row_of_interrupt = 0,
        .row_of_pause = 0,
        .row_of_addr_loop = 0,
    };

    xTaskNotifyStateClear(mjpeg_dec_task_hd);
    mjpeg_dec_intstatus = 0U;

    bflb_mjdec_deinit(mjpeg_dec_dev);
    bflb_mjdec_init(mjpeg_dec_dev, &cfg);
    bflb_mjdec_int_clear(mjpeg_dec_dev, MJDEC_INT_ALL);
    bflb_mjdec_int_enable(mjpeg_dec_dev, MJDEC_INT_ALL, true);
    bflb_mjdec_start(mjpeg_dec_dev);

    int mjdec_status = (int)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(MJPEG_DEC_WAIT_TIMEOUT_MS));
    if (mjdec_status != MJPEG_DEC_FRAME_DONE) {
        JPEG_ERR("decode failed, notify=%d intstatus=0x%08lx\r\n",
                 mjdec_status, (unsigned long)mjpeg_dec_intstatus);
        return -1;
    }

    return 0;
}

static void mjpeg_dec_task(void *pvParameters)
{
    (void)pvParameters;

    int ret;
    uint16_t width, height;
    uint32_t output_size;
    jpeg_frame_t mjpeg_in_frame;
    pimg_raw_frame_t img_raw_in_frame;

    while (1) {
        /* pop mjpeg frame */
        ret = frame_queue_output_pop(g_mjpeg_in_frame_ctrl, (frame_elem_t *)&mjpeg_in_frame, mjpeg_in_out_queue_mjpeg_dec_id, 1000);
        if (ret < 0) {
            JPEG_INFO("mjpeg_in pop timeout %d, continue wait... \r\n", ret);
            continue;
        }
        JPEG_DBG("mjpeg_in pop: id %d, addr 0x%08X, size %d\r\n", mjpeg_in_frame.elem_base.frame_id, mjpeg_in_frame.elem_base.frame_addr, mjpeg_in_frame.data_size);

        if ((mjpeg_in_frame.data_size == 0U) ||
            (mjpeg_in_frame.data_size > mjpeg_in_frame.elem_base.frame_size)) {
            JPEG_ERR("invalid jpeg size %lu\r\n", (unsigned long)mjpeg_in_frame.data_size);
            frame_queue_output_free(g_mjpeg_in_frame_ctrl, (frame_elem_t *)&mjpeg_in_frame);
            continue;
        }

        /* parse jpeg size */
        ret = parse_jpeg_size((const uint8_t *)mjpeg_in_frame.elem_base.frame_addr,
                              mjpeg_in_frame.data_size, &width, &height);
        if (ret < 0) {
            JPEG_ERR("parse jpeg size failed: %d\r\n", ret);
            frame_queue_output_free(g_mjpeg_in_frame_ctrl, (frame_elem_t *)&mjpeg_in_frame);
            continue;
        }
        output_size = (uint32_t)width * (uint32_t)height * (uint32_t)g_output_cfg.pixel_size;

        /* allco img_raw frame buff */
        ret = frame_queue_alloc_lock(g_img_raw_in_frame_ctrl, (frame_elem_t *)&img_raw_in_frame, 5);
        if (ret < 0) {
            frame_queue_output_free(g_mjpeg_in_frame_ctrl, (frame_elem_t *)&mjpeg_in_frame);
            JPEG_DBG("img_raw_in alloc timeout %d\r\n", ret);
            continue;
        }

        /* check output size */
        if ((output_size == 0U) || (output_size > img_raw_in_frame.elem_base.frame_size)) {
            JPEG_ERR("output size overflow, need=%lu, frame=%lu\r\n",
                     (unsigned long)output_size,
                     (unsigned long)img_raw_in_frame.elem_base.frame_size);
            frame_queue_output_free(g_mjpeg_in_frame_ctrl, (frame_elem_t *)&mjpeg_in_frame);
            frame_queue_free_unlock(g_img_raw_in_frame_ctrl, (frame_elem_t *)&img_raw_in_frame);
            continue;
        }

        bflb_l1c_dcache_clean_range(mjpeg_in_frame.elem_base.frame_addr, mjpeg_in_frame.data_size);

        /* mjpeg dec */
        ret = mjpeg_dec_decode_frame(mjpeg_in_frame.elem_base.frame_addr,
                                     img_raw_in_frame.elem_base.frame_addr,
                                     g_output_cfg.mjdec_format);

        /* free the mjpeg frame */
        frame_queue_output_free(g_mjpeg_in_frame_ctrl, (frame_elem_t *)&mjpeg_in_frame);

        if (ret < 0) {
            JPEG_ERR("mjpeg decode failed: %d\r\n", ret);
        } else {
            g_mjpeg_dec_total_frame_cnt++;
            bflb_l1c_dcache_invalidate_range(img_raw_in_frame.elem_base.frame_addr, output_size);

            img_raw_in_frame.x_start = 0U;
            img_raw_in_frame.y_start = 0U;
            img_raw_in_frame.x_end = (width > 0U) ? (width - 1U) : 0U;
            img_raw_in_frame.y_end = (height > 0U) ? (height - 1U) : 0U;
            img_raw_in_frame.format = g_output_cfg.img_raw_format;

            for (uint16_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                frame_queue_push(g_img_raw_in_frame_ctrl, (frame_elem_t *)&img_raw_in_frame, i, 0);
            }
        }

        frame_queue_free_unlock(g_img_raw_in_frame_ctrl, (frame_elem_t *)&img_raw_in_frame);
    }
}

int mjpeg_dec_task_init(void)
{
    JPEG_INFO("mjpeg_dec_task_init\r\n");

    mjpeg_dec_dev = bflb_device_get_by_name("mjdec");
    if (mjpeg_dec_dev == NULL) {
        JPEG_ERR("get mjdec device failed\r\n");
        return -1;
    }
    bflb_irq_attach(mjpeg_dec_dev->irq_num, mjpeg_dec_isr, NULL);
    bflb_irq_enable(mjpeg_dec_dev->irq_num);

    /* create mjpeg dec out queue */
    mjpeg_in_out_queue_mjpeg_dec_id = MJPEG_IN_FRAME_STREAM_DEC_ID;
    if (frame_queue_output_create(g_mjpeg_in_frame_ctrl, &mjpeg_in_out_queue_mjpeg_dec_id, MJPEG_IN_FRAME_STREAM_DEC_DEPTH) < 0) {
        JPEG_ERR("mjpeg_in frame mjpeg_dec out queue create failed\r\n");
        return -1;
    } else {
        JPEG_INFO("mjpeg_in frame mjpeg_dec out queue ID: %d\r\n", mjpeg_in_out_queue_mjpeg_dec_id);
    }

     if (CONFIG_MJPEG_DEC_OUTPUT_FORMAT == 1) {
        g_output_cfg.name = "RGB565";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_RGB565;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_RGB565;
        g_output_cfg.pixel_size = 2;
    } else if (CONFIG_MJPEG_DEC_OUTPUT_FORMAT == 2) {
        g_output_cfg.name = "RGB888";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_BGR888;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_RGB888;
        g_output_cfg.pixel_size = 3;
    } else if (CONFIG_MJPEG_DEC_OUTPUT_FORMAT == 3) {
        g_output_cfg.name = "NRGB8888";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_NBGR8888;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_NRGB8888;
        g_output_cfg.pixel_size = 4;
    } else if (CONFIG_MJPEG_DEC_OUTPUT_FORMAT == 4) {
        g_output_cfg.name = "YUYV";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_YUYV;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_YUYV;
        g_output_cfg.pixel_size = 2;
    } else {
        JPEG_ERR("invalid output format %d\r\n", CONFIG_MJPEG_DEC_OUTPUT_FORMAT);
        return -1;
    }
    JPEG_INFO("mjpeg_dec output format: %s\r\n", g_output_cfg.name);

    /* create mjpeg dec task */
    xTaskCreate(mjpeg_dec_task, (char *)"mjpeg_dec_task", 384, NULL, 25, &mjpeg_dec_task_hd);

    return 0;
}
