#include "bflb_sec_trng.h"
#include "hardware/sec_eng_reg.h"

#define BFLB_PUT_LE32TOBYTES(p, val) \
    {                                \
        p[0] = val & 0xff;           \
        p[1] = (val >> 8) & 0xff;    \
        p[2] = (val >> 16) & 0xff;   \
        p[3] = (val >> 24) & 0xff;   \
    }

void bflb_trng_read(struct bflb_device_s *dev, uint8_t data[32])
{
    uint32_t regval;
    uint32_t reg_base;
    uint8_t *p = (uint8_t *)data;

    reg_base = dev->reg_base;

    /* enable trng */
    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval |= SEC_ENG_SE_TRNG_0_EN;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval |= SEC_ENG_SE_TRNG_0_INT_CLR_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* busy will be set to 1 after trigger, the gap is 1T */
    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");

    while (getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET) & SEC_ENG_SE_TRNG_0_BUSY) {
    }

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval |= SEC_ENG_SE_TRNG_0_INT_CLR_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval |= SEC_ENG_SE_TRNG_0_TRIG_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* busy will be set to 1 after trigger, the gap is 1T */
    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");
    __ASM volatile("nop");

    while (getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET) & SEC_ENG_SE_TRNG_0_BUSY) {
    }

    /* copy trng value */
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_0_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_1_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_2_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_3_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_4_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_5_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_6_OFFSET));
    p += 4;
    BFLB_PUT_LE32TOBYTES(p, getreg32(reg_base + SEC_ENG_SE_TRNG_0_DOUT_7_OFFSET));
    p += 4;

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval &= ~SEC_ENG_SE_TRNG_0_TRIG_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval |= SEC_ENG_SE_TRNG_0_DOUT_CLR_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval &= ~SEC_ENG_SE_TRNG_0_DOUT_CLR_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    /* disable trng */
    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval &= ~SEC_ENG_SE_TRNG_0_EN;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
    regval |= SEC_ENG_SE_TRNG_0_INT_CLR_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_TRNG_0_CTRL_0_OFFSET);
}