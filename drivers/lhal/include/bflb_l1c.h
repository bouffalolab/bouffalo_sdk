#ifndef _BFLB_L1C_H
#define _BFLB_L1C_H

#include "bflb_core.h"

void bflb_l1c_icache_enable(void);
void bflb_l1c_icache_disable(void);
void bflb_l1c_icache_invalid_all(void);
void bflb_l1c_dcache_enable(void);
void bflb_l1c_dcache_disable(void);
void bflb_l1c_dcache_clean_all(void);
void bflb_l1c_dcache_invalidate_all(void);
void bflb_l1c_dcache_clean_invalidate_all(void);
void bflb_l1c_dcache_clean_range(void *addr, uint32_t size);
void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size);
void bflb_l1c_dcache_clean_invalidate_range(void *addr, uint32_t size);

#endif