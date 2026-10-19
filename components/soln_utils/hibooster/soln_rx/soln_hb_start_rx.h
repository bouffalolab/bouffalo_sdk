#ifndef __HB_START_RX_H__
#define __HB_START_RX_H__

#include <stdint.h>

uint32_t soln_hb_recv_get_total_frame_count(void);
void soln_hb_recv_clear_total_frame_count(void);

int soln_hb_recv_init(uint16_t local_port,
				 uint8_t peer_ip0,
				 uint8_t peer_ip1,
				 uint8_t peer_ip2,
				 uint8_t peer_ip3,
				 uint16_t peer_port);
int soln_hb_recv_deinit(void);

int soln_hb_recv_start(void);
int soln_hb_recv_stop(void);

#endif
