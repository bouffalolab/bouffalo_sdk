#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include <string.h>
#include <core_rv32.h>
#include <lwip/errno.h>
#include "librtspsrv.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#if (0)
#define RTC_DBG(a, ...) printf("[RTC dbg]:" a, ##__VA_ARGS__)
#else
#define RTC_DBG(a, ...)
#endif

#if (1)
#define RTC_INFO(a, ...) printf("[RTC info]:" a, ##__VA_ARGS__)
#else
#define RTC_INFO(a, ...)
#endif

#define RTC_ERR(a, ...) printf("[RTC err]:" a, ##__VA_ARGS__)

static uint16_t jpeg_out_queue_rtc_id = FRAME_QUEUE_DEFAULT_ID;

static jpeg_frame_t jpeg_frame_info;

void bl_cam_frame_init(void)
{
    RTC_INFO("bl_cam_frame_init\r\n");

    if (jpeg_out_queue_rtc_id != MJPEG_FRAME_STREAM_WIFI_RTC_ID) {
        jpeg_out_queue_rtc_id = MJPEG_FRAME_STREAM_WIFI_RTC_ID;
        if (frame_queue_output_create(g_jpeg_frame_ctrl, &jpeg_out_queue_rtc_id, MJPEG_FRAME_STREAM_WIFI_RTC_DEPTH) < 0) {
            jpeg_out_queue_rtc_id = FRAME_QUEUE_DEFAULT_ID;
            RTC_ERR("jpeg frame wifi_rtc out queue create failed\r\n");
            return;
        } else {
            RTC_INFO("jpeg frame wifi_rtc out queue ID: %d\r\n", jpeg_out_queue_rtc_id);
        }
    }
}

static void rtsp_client_event_handler(int action)
{
    switch (action) {
        case RTSP_STRM_REPORT_CLIENT_EXIT:
            printf("RTSP disconnect\r\n");
            break;
        default:
            break;
    }
}

static int get_frm_cb(struct strm_info *strm_info, struct frm_info *frm_info)
{


    if (rtsp_get_video_en()) {
        int ret = 0;
        static uint32_t get_count = 0;
        RTC_DBG("[rtc] get: %d\r\n", get_count);

        ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame_info, jpeg_out_queue_rtc_id, 1000);

        if (ret < 0) {
            RTC_INFO("[RTC]mjpeg frame pop timeout!\r\n");
            return -1;
        } else {
            RTC_DBG("get jpeg id: %d, addr 0x%X, size %d,\r\n", jpeg_frame_info.elem_base.frame_id, jpeg_frame_info.elem_base.frame_addr, jpeg_frame_info.data_size);

            bflb_l1c_dcache_invalidate_range(jpeg_frame_info.elem_base.frame_addr, jpeg_frame_info.data_size);

            memcpy(frm_info->frm_buf, (uint8_t *)jpeg_frame_info.elem_base.frame_addr, jpeg_frame_info.data_size);
            frm_info->frm_sz = jpeg_frame_info.data_size;
            get_count++;
            frm_info->frm_type = FRM_TYPE_M;
            frm_info->timestamp = xTaskGetTickCount(); // frame timestamp unit: 100ns
             /* free frame */
            frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame_info);

            return 1;
        }
    }

    return -1;
}

void strm_rtsp_start(void)
{
    bl_cam_frame_init();
    rtsp_set_video_en(1);
    rtsp_set_audio_en(0);
    set_strm_cb(get_frm_cb);
    rtsp_set_videoFmt(RTSP_VIDEOFMT_MJPEG);
    rtsp_set_strm_report_cb(rtsp_client_event_handler);
    rtsp_set_video_fps(25);
    rtsp_init_lib();
}
