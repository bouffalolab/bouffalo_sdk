#include "bflb_cks.h"
#include "hardware/cks_reg.h"

void bflb_cks_reset(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_cks_reset
    romapi_bflb_cks_reset(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + CKS_CONFIG_OFFSET);
    regval |= CKS_CR_CKS_CLR;
    putreg32(regval, reg_base + CKS_CONFIG_OFFSET);
#endif
}

void bflb_cks_set_endian(struct bflb_device_s *dev, uint8_t endian)
{
#ifdef romapi_bflb_cks_set_endian
    romapi_bflb_cks_set_endian(dev, endian);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(endian << 1, reg_base + CKS_CONFIG_OFFSET);
#endif
}

uint16_t bflb_cks_compute(struct bflb_device_s *dev, uint8_t *data, uint32_t length)
{
#ifdef romapi_bflb_cks_compute
    return romapi_bflb_cks_compute(dev, data, length);
#else
    uint32_t reg_base;
    uint32_t i;

    reg_base = dev->reg_base;
    for (i = 0; i < length; i++) {
        putreg32(data[i], reg_base + CKS_DATA_IN_OFFSET);
    }

    return ((uint16_t)(getreg32(reg_base + CKS_OUT_OFFSET) & 0xffff));
#endif
}
