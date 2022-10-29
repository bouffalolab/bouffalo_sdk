#ifndef _BFLB_L1C_H
#define _BFLB_L1C_H

#include "bflb_core.h"

#if defined(BL616) || defined(BL606P) || defined(BL808) || defined(BL628)
#include "csi_core.h"
#define bflb_l1c_icache_enable()                          csi_icache_enable()
#define bflb_l1c_icache_disable()                         csi_icache_disable()
#define bflb_l1c_icache_invalid_all()                     csi_icache_invalid()
#define bflb_l1c_dcache_enable()                          csi_dcache_enable()
#define bflb_l1c_dcache_disable()                         csi_dcache_disable()
#define bflb_l1c_dcache_clean_all()                       csi_dcache_clean()
#define bflb_l1c_dcache_invalidate_all()                  csi_icache_invalid()
#define bflb_l1c_dcache_clean_invalidate_all()            csi_dcache_clean_invalid()
#define bflb_l1c_dcache_clean_range(addr, len)            csi_dcache_clean_range(addr, len)
#define bflb_l1c_dcache_invalidate_range(addr, len)       csi_dcache_invalid_range(addr, len)
#define bflb_l1c_dcache_clean_invalidate_range(addr, len) csi_dcache_clean_invalid_range(addr, len)
#else
#define bflb_l1c_icache_enable()
#define bflb_l1c_icache_disable()
#define bflb_l1c_icache_invalid_all()
#define bflb_l1c_dcache_enable()
#define bflb_l1c_dcache_disable()
#define bflb_l1c_dcache_clean_all()
#define bflb_l1c_dcache_invalidate_all()
#define bflb_l1c_dcache_clean_invalidate_all()
#define bflb_l1c_dcache_clean_range(addr, len)
#define bflb_l1c_dcache_invalidate_range(addr, len)
#define bflb_l1c_dcache_clean_invalidate_range(addr, len)
#endif

#endif