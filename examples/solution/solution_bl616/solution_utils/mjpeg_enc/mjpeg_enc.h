#ifndef __MJPEG_ENC_H__
#define __MJPEG_ENC_H__

extern uint32_t g_mjpeg_enc_total_frame_cnt;

int mjpeg_enc_task_init(uint32_t x, uint32_t y, uint8_t quality);

#endif /* __MJPEG_ENC_H__ */
