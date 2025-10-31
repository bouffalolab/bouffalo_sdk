#ifndef __DVP_MJPEG_H__
#define __DVP_MJPEG_H__

#define DVP_JPEG_BLOCK_NUM      (2)
#define DVP_JPEG_W              (CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH)

#define DVP_JPEG_ROW_NUM        (8 * DVP_JPEG_BLOCK_NUM)
#define DVP_JPEG_LINE_BUFF_SIZE (DVP_JPEG_W * DVP_JPEG_ROW_NUM * 2)

extern uint32_t g_dvp_mjpeg_total_frame_cnt;

int dvp_mjpeg_task_init(uint8_t quality);

#endif /* __DVP_MJPEG_H__ */