#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <event_groups.h>
#include <semphr.h>
#include <queue.h>
#include <stream_buffer.h>
#include <mjpeg_booster.h>
#include <mjpeg_sw_buffer.h>
#include <bl_cam_frame.h>

#include <bflb_core.h>

#ifndef SIMU_DATA_TEST
#include "frame_queue_ctrl.h"
#include "frame_queue.h"
#include "bflb_l1c.h"
#endif

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

#ifndef SIMU_DATA_TEST
static uint16_t jpeg_out_queue_rtc_id = FRAME_QUEUE_DEFAULT_ID;

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
#else

void bl_cam_frame_init(void)
{
    return;
}

#endif

#ifdef SIMU_DATA_TEST
#include "static_mjpeg.h"
#endif

#ifndef SIMU_DATA_TEST

static jpeg_frame_t jpeg_frame_info[4] = { 0 };
static int frame_valid[4] = { -1, -1, -1, -1 };
#endif

static int bl_get_jpeg_frame(uint8_t **c_ptr1, uint32_t *c_len1, uint8_t **c_ptr2, uint32_t *c_len2)
{
#ifndef SIMU_DATA_TEST
    int ret = 0;
    static uint32_t get_count = 0;
    uint32_t count = get_count & 0x03;

    RTC_DBG("[rtc] frame:%d, %d, %d, %d\r\n", frame_valid[0], frame_valid[1], frame_valid[2], frame_valid[3]);
    RTC_DBG("[rtc] get: %d\r\n", get_count);

    ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame_info[count], jpeg_out_queue_rtc_id, 1000);

    if (ret < 0) {
        RTC_INFO("[RTC]mjpeg frame pop timeout!\r\n");
        return -1;
    } else {
        RTC_DBG("get jpeg id: %d, addr 0x%X, size %d,\r\n", jpeg_frame_info[count].elem_base.frame_id, jpeg_frame_info[count].elem_base.frame_addr, jpeg_frame_info[count].data_size);

        frame_valid[count] = jpeg_frame_info[count].elem_base.frame_id;

        bflb_l1c_dcache_invalidate_range(jpeg_frame_info[count].elem_base.frame_addr, jpeg_frame_info[count].data_size);

        *c_ptr1 = (uint8_t *)(jpeg_frame_info[count].elem_base.frame_addr);
        *c_len1 = (uint32_t)(jpeg_frame_info[count].data_size);
        *c_ptr2 = NULL;
        *c_len2 = 0;
        get_count++;
    }
#else

    *c_ptr1 = (uint8_t *)color_bar_test_jpg;
    *c_len1 = sizeof(color_bar_test_jpg);
    *c_ptr2 = NULL;
    *c_len2 = 0;

    vTaskDelay(20);

#endif

    return 0;
}

int bl_cam_frame_fifo_get(uint32_t *c_frames, uint8_t **c_ptr1, uint32_t *c_len1, uint8_t **c_ptr2, uint32_t *c_len2, int pic_mode)
{
    int ret = 0;

    *c_frames = 1;
    *c_ptr2 = NULL;
    *c_len2 = 0;
#ifdef CPU_D0
    switch (pic_mode) {
        case H264_MAIN:
            //ret = mm_get_h264_frame(c_ptr1, c_len1);
            //ret = sample_get_h264_frame(c_ptr1, c_len1, c_ptr2, c_len2, 0);
            ret = bl_get_h264_frame(c_ptr1, c_len1, c_ptr2, c_len2);
            break;
        case H264_SUB:
            ret = sample_get_h264_frame(c_ptr1, c_len1, c_ptr2, c_len2, 1);
            /*TODO add H264_SUB stream*/
            break;
        case MJPEG:
            ret = sample_get_jpeg_pic(c_ptr1, c_len1, 2);
            break;
        case RAW:
            sample_start_yuv_pic(1);
            ret = sample_get_raw_pic(c_ptr1, c_len1, 0);
            break;
        case YUV:
            ret = sample_get_yuv_pic(c_ptr1, c_len1, 1);
            break;
        default:
            break;
    }
#else
    switch (pic_mode) {
        case MJPEG:
            ret = bl_get_jpeg_frame(c_ptr1, c_len1, c_ptr2, c_len2);
            break;
        default:
            ret = -1;
            //printf("wrong image channel\r\n");
            break;
    }
#endif
    return ret;
}

int bl_cam_frame_get_info(uint8_t *quality, uint16_t *width, uint16_t *height)
{
    *quality = 50;
    *width = 640;
    *height = 480;
    return 0;
}

int bl_cam_frame_config(uint8_t quality, uint16_t width, uint16_t height)
{
    return 0;
}

int bl_cam_frame_pop(void)
{
#ifndef SIMU_DATA_TEST

    static int pop_count = 0;

    RTC_DBG("[rtc] frame:%d, %d, %d, %d\r\n", frame_valid[0], frame_valid[1], frame_valid[2], frame_valid[3]);
    RTC_DBG("[rtc] free: id-%d %d\r\n", jpeg_frame_info[pop_count & 0x03].elem_base.frame_id, pop_count);

    /* free frame */
    frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame_info[pop_count & 0x03]);
    frame_valid[pop_count & 0x03] = -1;
    pop_count++;

#endif
    return 0;
}

int bl_cam_frame_reset(void)
{
#ifndef SIMU_DATA_TEST

    RTC_DBG("[rtc] reset frame:%d, %d, %d, %d\r\n", frame_valid[0], frame_valid[1], frame_valid[2], frame_valid[3]);

    for (uint16_t i = 0; i < 4; i++) {
        if (frame_valid[i] >= 0) {
            frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame_info[i]);
            frame_valid[i] = -1;
        }
    }

#endif
    return 0;
}
