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

#include "soln_fbq.h"

#include "soln_mjpeg_dec.h"

#ifdef CONFIG_SOLN_VID_JPEG_DEC_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_VID_JPEG_DEC_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_MJPEG_DEC"
#include "log.h"

#if !defined(BL616CL)
#error "This example code is only for BL616CL"
#endif

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

static uint32_t s_mjpeg_dec_total_frame_count = 0;

uint32_t soln_mjpeg_dec_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_mjpeg_dec_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_mjpeg_dec_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_mjpeg_dec_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void mjpeg_dec_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_mjpeg_dec_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

static TaskHandle_t mjpeg_dec_task_hd = NULL;
static struct bflb_device_s *mjpeg_dec_dev = NULL;
static volatile uint32_t mjpeg_dec_intstatus = 0;

static uint16_t remote_jpeg_output_dec_id = CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_ID;

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
        LOG_E("decode frame addr invalid\r\n");
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

#if defined(BL616CL)
    /* burst 2 and delay 0xA0A0, 100ms @1920x1080@RGB888 */
    /* axi burst */
    uint32_t reg_val = getreg32(mjpeg_dec_dev->reg_base + 0);
    reg_val &= ~(0x3 << 4);
    reg_val &= ~(0x3 << 6);
    reg_val |= (2 << 4);
    reg_val |= (2 << 6);
    putreg32(reg_val, mjpeg_dec_dev->reg_base + 0);
    /* axi burst delay */
    putreg32(0xA0A0, mjpeg_dec_dev->reg_base + 0x40);
#endif

    bflb_mjdec_start(mjpeg_dec_dev);

    int mjdec_status = (int)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(MJPEG_DEC_WAIT_TIMEOUT_MS));
    if (mjdec_status != MJPEG_DEC_FRAME_DONE) {
        LOG_E("decode failed, notify=%d intstatus=0x%08lx\r\n",
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
    fbq_elem_t *remote_jpeg_frame;
    fbq_elem_t *remote_raw_frame;

    while (1) {
        /* pop mjpeg frame */
        ret = fbq_pop(soln_fbq_vid_jpeg_remote(), &remote_jpeg_frame, remote_jpeg_output_dec_id, 1000);
        if (ret != FBQ_OK) {
            LOG_W("remote JPEG pop timeout %d, continue wait... \r\n", ret);
            continue;
        }
        LOG_D("remote JPEG pop: id %d, addr 0x%08X, size %d\r\n", remote_jpeg_frame->id, remote_jpeg_frame->data, remote_jpeg_frame->size);

        if ((remote_jpeg_frame->size == 0U) ||
            (remote_jpeg_frame->size > remote_jpeg_frame->capacity)) {
            LOG_E("invalid jpeg size %lu\r\n", (unsigned long)remote_jpeg_frame->size);
            fbq_free(remote_jpeg_frame);
            continue;
        }

        /* parse jpeg size */
        ret = parse_jpeg_size((const uint8_t *)remote_jpeg_frame->data,
                      remote_jpeg_frame->size, &width, &height);
        if (ret < 0) {
            LOG_E("parse jpeg size failed: %d\r\n", ret);
            fbq_free(remote_jpeg_frame);
            continue;
        }
        output_size = (uint32_t)width * (uint32_t)height * (uint32_t)g_output_cfg.pixel_size;

        /* allco img_raw frame buff */
        ret = fbq_alloc(soln_fbq_vid_raw_remote(), &remote_raw_frame, 20);
        if (ret != FBQ_OK) {
            fbq_free(remote_jpeg_frame);
            LOG_D("remote RAW alloc timeout %d\r\n", ret);
            continue;
        }
        remote_raw_frame->type_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_JPEG_DEC;

        /* check output size */
        if ((output_size == 0U) || (output_size > remote_raw_frame->capacity)) {
            LOG_E("output size overflow, need=%lu, frame=%lu\r\n",
                  (unsigned long)output_size,
                (unsigned long)remote_raw_frame->capacity);
            fbq_free(remote_jpeg_frame);
            fbq_free(remote_raw_frame);
            continue;
        }

        bflb_l1c_dcache_clean_range(remote_jpeg_frame->data, remote_jpeg_frame->size);

        /* mjpeg dec */
        ret = mjpeg_dec_decode_frame(remote_jpeg_frame->data,
                         remote_raw_frame->data,
                                     g_output_cfg.mjdec_format);

        fbq_free(remote_jpeg_frame);

        if (ret < 0) {
            LOG_E("mjpeg decode failed: %d\r\n", ret);
        } else {
            mjpeg_dec_record_frame();
            bflb_l1c_dcache_invalidate_range(remote_raw_frame->data, output_size);

            soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext(remote_raw_frame);
            raw_ext->x_start = 0U;
            raw_ext->y_start = 0U;
            raw_ext->x_end = (width > 0U) ? (width - 1U) : 0U;
            raw_ext->y_end = (height > 0U) ? (height - 1U) : 0U;
            raw_ext->format = g_output_cfg.img_raw_format;
            remote_raw_frame->size = output_size;

            (void)fbq_push_mask(soln_fbq_vid_raw_remote(), remote_raw_frame, FBQ_OUTPUT_ALL, 0);
        }

        fbq_free(remote_raw_frame);
    }
}

int soln_mjpeg_dec_task_init(void)
{
    LOG_I("soln_mjpeg_dec_task_init\r\n");

    mjpeg_dec_dev = bflb_device_get_by_name("mjdec");
    if (mjpeg_dec_dev == NULL) {
        LOG_E("get mjdec device failed\r\n");
        return -1;
    }
    bflb_irq_attach(mjpeg_dec_dev->irq_num, mjpeg_dec_isr, NULL);
    bflb_irq_enable(mjpeg_dec_dev->irq_num);

    /* Create the decoder output subscription on the remote JPEG queue. */
    remote_jpeg_output_dec_id = CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_ID;
    if (fbq_output_open(soln_fbq_vid_jpeg_remote(), &remote_jpeg_output_dec_id,
                        CONFIG_SOLN_FBQ_VID_JPEG_REMOTE_DEC_DEPTH,
                        SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_HB_REC) != FBQ_OK) {
        LOG_E("remote JPEG decoder output create failed\r\n");
        return -1;
    } else {
        LOG_I("remote JPEG decoder output ID: %d\r\n", remote_jpeg_output_dec_id);
    }

    if (CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT == 1) {
        g_output_cfg.name = "RGB565";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_RGB565;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_RGB565;
        g_output_cfg.pixel_size = 2;
    } else if (CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT == 2) {
        g_output_cfg.name = "RGB888";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_RGB888;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_RGB888;
        g_output_cfg.pixel_size = 3;
    } else if (CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT == 3) {
        g_output_cfg.name = "NRGB8888";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_NRGB8888;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_NRGB8888;
        g_output_cfg.pixel_size = 4;
    } else if (CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT == 4) {
        g_output_cfg.name = "YUYV";
        g_output_cfg.mjdec_format = MJDEC_FORMAT_YUYV;
        g_output_cfg.img_raw_format = IMG_RAW_FRAME_FORMAT_YUYV;
        g_output_cfg.pixel_size = 2;
    } else {
        LOG_E("invalid output format %d\r\n", CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT);
        return -1;
    }
    LOG_I("mjpeg_dec output format: %s\r\n", g_output_cfg.name);

    /* create mjpeg dec task */
    xTaskCreate(mjpeg_dec_task, (char *)"mjpeg_dec_task", 384, NULL, 25, &mjpeg_dec_task_hd);

    return 0;
}
