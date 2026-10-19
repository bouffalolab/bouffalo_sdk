#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include <string.h>
#include <core_rv32.h>
#include <lwip/errno.h>
#include "librtspsrv.h"

#include "soln_fbq.h"

#ifdef CONFIG_SOLN_RTSP_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_RTSP_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_RTSP"
#include "log.h"

static uint16_t local_jpeg_output_net_id = FBQ_OUTPUT_AUTO;
static bool local_jpeg_output_open;

static void bl_cam_frame_init(void)
{
    LOG_I("bl_cam_frame_init\r\n");

    if (!local_jpeg_output_open) {
        local_jpeg_output_net_id = CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_ID;
        if (fbq_output_open(soln_fbq_vid_jpeg_local(), &local_jpeg_output_net_id,
                            CONFIG_SOLN_FBQ_VID_JPEG_LOCAL_NET_DEPTH,
                            SOLUTION_FBQ_TYPE_MASK_IMG_JPEG_LOCAL_ALL) != FBQ_OK) {
            local_jpeg_output_net_id = FBQ_OUTPUT_AUTO;
            LOG_E("jpeg frame wifi_rtc out queue create failed\r\n");
            return;
        } else {
            local_jpeg_output_open = true;
            LOG_I("local JPEG RTSP output ID: %d\r\n", local_jpeg_output_net_id);
        }
    }
}

static void rtsp_client_event_handler(int action)
{
    switch (action) {
        case RTSP_STRM_REPORT_CLIENT_EXIT:
            LOG_I("RTSP disconnect\r\n");
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
        fbq_elem_t *jpeg_frame_info;
        LOG_D("get frame: %d\r\n", get_count);

        ret = fbq_pop(soln_fbq_vid_jpeg_local(), &jpeg_frame_info, local_jpeg_output_net_id, 1000);

        if (ret != FBQ_OK) {
            LOG_W("mjpeg frame pop timeout\r\n");
            return -1;
        } else {
            LOG_D("get jpeg id: %d, addr 0x%X, size %d,\r\n", jpeg_frame_info->id, jpeg_frame_info->data, jpeg_frame_info->size);

            memcpy(frm_info->frm_buf, (uint8_t *)jpeg_frame_info->data, jpeg_frame_info->size);
            frm_info->frm_sz = jpeg_frame_info->size;
            get_count++;
            frm_info->frm_type = FRM_TYPE_M;
            frm_info->timestamp = xTaskGetTickCount(); // frame timestamp unit: 100ns
                                                       /* free frame */
            fbq_free(jpeg_frame_info);

            return 1;
        }
    }

    return -1;
}

void soln_strm_rtsp_start(void)
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
