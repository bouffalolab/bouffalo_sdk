#ifndef __MJPEG_DEC_H__
#define __MJPEG_DEC_H__

#include <stdint.h>

/*
 * MJPEG decode output format:
 * 1: RGB565
 * 2: RGB888
 * 3: NRGB8888
 * 4: YUYV
 * 5: YUV (not supported yet)
 */
#ifndef CONFIG_MJPEG_DEC_OUTPUT_FORMAT
#define CONFIG_MJPEG_DEC_OUTPUT_FORMAT (2)
#endif

extern uint32_t g_mjpeg_dec_total_frame_cnt;

int mjpeg_dec_task_init(void);

#endif /* __MJPEG_DEC_H__ */
