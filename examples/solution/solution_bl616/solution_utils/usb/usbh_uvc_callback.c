#include "usbh_uvc_uac.h"

#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>

#include "bflb_mtimer.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#if (0)
#define UVC_INFO(a, ...) printf("[UVC info]:" a, ##__VA_ARGS__)
#else
#define UVC_INFO(a, ...)
#endif

#define UVC_ERR(a, ...) printf("[UVC err]:" a, ##__VA_ARGS__)

uint32_t g_uvc_total_frame_cnt = 0;

struct bflb_device_s *usbh_video_get_dma_ch(void)
{
    return bflb_device_get_by_name("dma0_ch2");
}

#if (CONFIG_SOLUTION_FUNC_UVC_JPEG)

static jpeg_frame_t jpeg_frame = { 0 };

ATTR_TCM_SECTION void usbh_video_one_frame_callback_mjpeg(struct usbh_videostreaming *stream)
{
    int ret;

    if (g_uvc_frame_buff != NULL) {
        /* check */
        if (g_uvc_frame_buff != jpeg_frame.elem_base.frame_addr) {
            printf("uvc unkown jpeg_buff\r\n");
            while (1) {
            };
        }

        /* jpeg frame push */
        if (stream->bufoffset && stream->bufoffset <= jpeg_frame.elem_base.frame_size) {
            jpeg_frame.data_size = stream->bufoffset;
            /* try send to out_queue */
            for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                ret = frame_queue_push(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, i, 0);
            }
        } else {
            printf("uvc jpeg size over: %d -> %d\r\n", jpeg_frame.elem_base.frame_size, stream->bufoffset);
            while (1) {
            };
        }

        /* unlock jpeg */
        frame_queue_free_unlock(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);
    }

    /* alloc new jpeg frame */
    ret = frame_queue_alloc_lock(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, 0);
    if (ret < 0) {
        g_uvc_frame_buff = NULL;
    } else {
        g_uvc_frame_buff = jpeg_frame.elem_base.frame_addr;
        stream->buflen = jpeg_frame.elem_base.frame_size;
    }

    g_uvc_total_frame_cnt++;
}

void usbh_video_run_callback(void)
{
    /* UVC yuyv mode */
    printf("Starting UVC JPEG mode\r\n");
    usbh_video_set_config(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH, CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT, USBH_VIDEO_FORMAT_MJPEG, 1);
    usbh_video_test();
}

void usbh_video_stop_callback(void)
{
    if (g_uvc_frame_buff != NULL) {
        frame_queue_free_unlock(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);
        g_uvc_frame_buff = NULL;
    }

    printf("Stop UVC...\r\n");
}

#endif

#if (CONFIG_SOLUTION_FUNC_UVC_YUYV)

static pyuyv_frame_t pbuff_frame = { 0 };

ATTR_TCM_SECTION void usbh_video_one_frame_callback_yuyv(struct usbh_videostreaming *stream)
{
    int ret;

    if (g_uvc_frame_buff != NULL) {
        if (g_uvc_frame_buff != pbuff_frame.elem_base.frame_addr) {
            UVC_ERR("uvc unkown pbuff\r\n");
            while (1) {
            };
        }

        /* picture push */
        pbuff_frame.x_start = 0;
        pbuff_frame.y_start = 0;
        pbuff_frame.x_end = g_video_width - 1;
        pbuff_frame.y_end = g_video_height - 1;

        for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
            ret = frame_queue_push(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame, i, 0);
            UVC_INFO("uav[%d] %d\r\n", i, ret);
        }

        /* unlock */
        frame_queue_free_unlock(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame);
    }

    /* alloc new */
    ret = frame_queue_alloc_lock(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame, 0);

    if (ret < 0) {
        UVC_INFO("alloc failed\r\n");
        g_uvc_frame_buff = NULL;
    } else {
        UVC_INFO("alloc id %d, addr 0x%08X\r\n", pbuff_frame.elem_base.frame_id, (uint32_t)pbuff_frame.elem_base.frame_addr);
        g_uvc_frame_buff = pbuff_frame.elem_base.frame_addr;
        stream->buflen = pbuff_frame.elem_base.frame_size;
    }

    g_uvc_total_frame_cnt++;
}

void usbh_video_run_callback(void)
{
    /* UVC yuyv mode */
    printf("Starting UVC YUYV mode\r\n");
    usbh_video_set_config(CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH, CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT, USBH_VIDEO_FORMAT_UNCOMPRESSED, 1);
    usbh_video_test();
}

void usbh_video_stop_callback(void)
{
    if (g_uvc_frame_buff != NULL) {
        frame_queue_free_unlock(g_yuyv_frame_ctrl, (frame_elem_t *)&pbuff_frame);
        g_uvc_frame_buff = NULL;
    }

    printf("Stop UVC...\r\n");
}

#endif
