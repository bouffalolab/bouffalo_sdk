#ifndef _PHY_CNTRL_H_
#define _PHY_CNTRL_H_
#include "co_int.h"
#include "hal_desc.h"

void bbp_init();
void bbp_reset();
void bbp_check_rssi(struct rx_hd *rhd);
void bbp_update(struct rx_hd *rhd, uint16_t type_subtype);
void set_center_freq(uint16_t center_freq);

#endif
