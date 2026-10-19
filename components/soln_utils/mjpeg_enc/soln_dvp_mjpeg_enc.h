#ifndef __DVP_MJPEG_ENC_H__
#define __DVP_MJPEG_ENC_H__

#include <stdint.h>

#define DVP_JPEG_BLOCK_NUM      (2)
#define DVP_JPEG_W              (CONFIG_SOLN_VID_DEFAULT_WIDTH)

#define DVP_JPEG_ROW_NUM        (8 * DVP_JPEG_BLOCK_NUM)
#define DVP_JPEG_LINE_BUFF_SIZE (DVP_JPEG_W * DVP_JPEG_ROW_NUM * 2)

uint32_t soln_dvp_mjpeg_enc_get_total_frame_count(void);
void soln_dvp_mjpeg_enc_clear_total_frame_count(void);

int soln_dvp_mjpeg_enc_task_init(uint8_t quality);

#endif /* __DVP_MJPEG_ENC_H__ */
