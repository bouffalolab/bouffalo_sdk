#include "bflb_pec_v2.h"
#include "bflb_pec_common.h"

int bflb_pec_memory_size_check(struct bflb_device_s *dev, uint8_t start, uint16_t size, uint8_t *actual_start)
{
    uint8_t mem;

#if defined(BL616L)
    if (dev->sub_idx == 0) {
        mem = start;
        if (mem + size > PEC_MEMORY_SIZE_0) {
            return PEC_STS_MEMORY_LACK;
        }
    } else if (dev->sub_idx == 1) {
        mem = start + PEC_MEMORY_SIZE_0;
        if (mem + size > PEC_MEMORY_SIZE_1) {
            return PEC_STS_MEMORY_LACK;
        }
    } else {
        return PEC_STS_SM_INVALID;
    }
#else
    mem = start;
    if (mem + size > PEC_MEMORY_SIZE) {
        return PEC_STS_MEMORY_LACK;
    }
#endif
    *actual_start = mem;
    bflb_pec_mem_store(dev, start);

    return PEC_STS_OK;
}
