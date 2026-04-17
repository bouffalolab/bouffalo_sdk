#ifndef __HB_START_TX_H__
#define __HB_START_TX_H__

extern volatile uint32_t g_hb_tx_total_frame_cnt;

int hb_sender_init(uint16_t local_port);
int hb_sender_deinit(void);

#endif