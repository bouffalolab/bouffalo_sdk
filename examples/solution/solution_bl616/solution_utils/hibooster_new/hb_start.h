#ifndef __HB_START_H__
#define __HB_START_H__

extern volatile uint32_t g_hibooster_total_frame_cnt;

int hb_sender_init(uint16_t local_port);
int hb_sender_deinit(void);

#endif