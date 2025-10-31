#ifndef __MJPEG_H__
#define __MJPEG_H__

extern uint32_t g_mjpeg_total_frame_cnt;

int mjpeg_compress_task_init(uint32_t x, uint32_t y, uint8_t quality);

#endif /* __DVP_MJPEG_H__ */