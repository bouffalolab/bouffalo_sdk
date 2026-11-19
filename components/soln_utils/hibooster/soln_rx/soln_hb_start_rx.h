#ifndef __HB_START_RX_H__
#define __HB_START_RX_H__

#include <stdint.h>

/* Successfully received nonempty frames within the supplied buffer capacity. */
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

/* Submit one free remote FBQ buffer without waiting for a slot.
 * Call from task context after releasing a consumer reference.
 * Return 0 if submitted, negative if inactive/full/unavailable.
 * Initialization/deinitialization must not run concurrently with this API. */
int soln_hb_recv_submit_buffer(void);

#endif
