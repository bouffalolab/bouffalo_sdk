#ifndef __MACSW_VELA_BL616CL_CONFIG_H__
#define __MACSW_VELA_BL616CL_CONFIG_H__
#include "macsw_default_config.h"

/*
 * BL616CL openvela profile.
 *
 * With one A-MSDU slot, rxl_hw_buffer1 overflows at TCP RX peak and the
 * MAC drops about 10% of the MPDUs.  Two slots remove the overflow.  The
 * 8-entry reorder window pays for it: the host RX buffers follow
 * (CFG_BARX * CFG_REORD_BUF + 2), so the total RX RAM does not grow.
 */
#undef CFG_RXL_BUFFER1_AMSDU_CNT
#define CFG_RXL_BUFFER1_AMSDU_CNT 2

#undef CFG_REORD_BUF
#define CFG_REORD_BUF 8

#endif /* __MACSW_VELA_BL616CL_CONFIG_H__ */
