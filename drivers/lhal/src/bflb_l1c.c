#include "bflb_l1c.h"

#if defined(BL616) || defined(BL606P) || defined(BL808) || defined(BL628)
#include "csi_core.h"
void bflb_l1c_icache_enable(void)
{
    csi_icache_enable();
}

void bflb_l1c_icache_disable(void)
{
    csi_icache_disable();
}

ATTR_TCM_SECTION void bflb_l1c_icache_invalid_all(void)
{
    csi_icache_invalid();
}

void bflb_l1c_dcache_enable(void)
{
    csi_dcache_enable();
}

void bflb_l1c_dcache_disable(void)
{
    csi_dcache_disable();
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_all(void)
{
    csi_dcache_clean();
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_all(void)
{
    csi_dcache_invalid();
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_all(void)
{
    csi_dcache_clean_invalid();
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_range(void *addr, uint32_t size)
{
    csi_dcache_clean_range(addr, size);
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size)
{
    csi_dcache_invalid_range(addr, size);
}

ATTR_TCM_SECTION void bflb_l1c_dcache_clean_invalidate_range(void *addr, uint32_t size)
{
    csi_dcache_clean_invalid_range(addr, size);
}
#else

extern void L1C_Cache_Enable_Set(uint8_t wayDisable);
extern void L1C_Cache_Flush(void);

void bflb_l1c_icache_enable(void)
{
}

void bflb_l1c_icache_disable(void)
{
    L1C_Cache_Enable_Set(0x0f);
}

void bflb_l1c_icache_invalid_all(void)
{
}

void bflb_l1c_dcache_enable(void)
{
}

void bflb_l1c_dcache_disable(void)
{
}

void bflb_l1c_dcache_clean_all(void)
{
}

void bflb_l1c_dcache_invalidate_all(void)
{
    L1C_Cache_Flush();
}

void bflb_l1c_dcache_clean_invalidate_all(void)
{
    L1C_Cache_Flush();
}

void bflb_l1c_dcache_clean_range(void *addr, uint32_t size)
{
}

ATTR_TCM_SECTION void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size)
{
    L1C_Cache_Flush();
}

#endif