#ifndef __HB_START_TX_H__
#define __HB_START_TX_H__

#include <stdint.h>

uint32_t soln_hb_sender_get_total_frame_count(void);
void soln_hb_sender_clear_total_frame_count(void);

int soln_hb_sender_init(uint16_t local_port);
int soln_hb_sender_deinit(void);

#endif