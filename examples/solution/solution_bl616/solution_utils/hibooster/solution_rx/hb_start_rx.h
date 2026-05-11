#ifndef __HB_START_RX_H__
#define __HB_START_RX_H__

#include <stdint.h>

extern volatile uint32_t g_hb_rx_total_frame_cnt;

int hb_recv_init(uint16_t local_port,
				 uint8_t peer_ip0,
				 uint8_t peer_ip1,
				 uint8_t peer_ip2,
				 uint8_t peer_ip3,
				 uint16_t peer_port);
int hb_recv_deinit(void);

int hb_recv_start(void);
int hb_recv_stop(void);

#endif
