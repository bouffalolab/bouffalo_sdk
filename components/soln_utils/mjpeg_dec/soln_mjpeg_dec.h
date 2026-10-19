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
#ifndef CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT
#define CONFIG_SOLN_VID_JPEG_DEC_OUTPUT_FORMAT (2)
#endif

uint32_t soln_mjpeg_dec_get_total_frame_count(void);
void soln_mjpeg_dec_clear_total_frame_count(void);

int soln_mjpeg_dec_task_init(void);

#endif /* __MJPEG_DEC_H__ */
