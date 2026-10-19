#ifndef __MJPEG_ENC_H__
#define __MJPEG_ENC_H__

#include <stdint.h>

uint32_t soln_mjpeg_enc_get_total_frame_count(void);
void soln_mjpeg_enc_clear_total_frame_count(void);

int soln_mjpeg_enc_task_init(uint32_t x, uint32_t y, uint8_t quality);

#endif /* __MJPEG_ENC_H__ */
