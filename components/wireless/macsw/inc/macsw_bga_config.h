#ifndef __MACSW_BGA_CONFIG_H__
#define __MACSW_BGA_CONFIG_H__

#include "macsw_default_config.h"

/*
 * Legacy 802.11b/g/a Wi-Fi profile (bands depend on the chip).
 *
 * Disable HT and newer PHY modes together with their aggregation support.
 * Use a smaller best-effort TX descriptor pool for legacy traffic.
 */
#undef CFG_HE
#undef CFG_VHT
#undef CFG_HT
#undef CFG_AGG
#undef CFG_AMSDU
#undef CFG_DEAGG

/* Distance measurement is not needed in the legacy low-RAM profile. */
#undef CFG_FTM_INIT
#undef CFG_FTM_RSP

#undef CFG_TXDESC1
#define CFG_TXDESC1 8

#endif /* __MACSW_BGA_CONFIG_H__ */
