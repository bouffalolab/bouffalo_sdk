#include "bflb_sec_aes.h"
#include "hardware/sec_eng_reg.h"

#define CONFIG_BFLB_AES_USE_BE
#define CONFIG_BFLB_AES_HW_KEY_SEL 1

#define BFLB_PUT_LE32(p) ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | (p[0]))
#define BFLB_PUT_BE32(p) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3]))

void bflb_aes_init(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

#ifdef CONFIG_BFLB_AES_USE_BE
    putreg32(0x1f, reg_base + SEC_ENG_SE_AES_0_ENDIAN_OFFSET);
#else
    putreg32(0x10, reg_base + SEC_ENG_SE_AES_0_ENDIAN_OFFSET);
#endif

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_AES_0_EN;
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

void bflb_aes_deinit(struct bflb_device_s *dev)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    regval &= ~SEC_ENG_SE_AES_0_EN;
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

void bflb_aes_set_mode(struct bflb_device_s *dev, uint8_t mode)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    regval &= ~SEC_ENG_SE_AES_0_BLOCK_MODE_MASK;
    regval |= (mode << SEC_ENG_SE_AES_0_BLOCK_MODE_SHIFT);

    if (mode == AES_MODE_CTR) {
        regval |= SEC_ENG_SE_AES_0_DEC_KEY_SEL;
    } else {
        regval &= ~SEC_ENG_SE_AES_0_DEC_KEY_SEL; /* clear key to set new key */
    }

    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
}

void bflb_aes_setkey(struct bflb_device_s *dev, const uint8_t *key, uint16_t keybits)
{
    uint32_t regval;
    uint32_t reg_base;
    uint8_t mode;
    uint8_t *temp_key = (uint8_t *)key;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    mode = (regval & SEC_ENG_SE_AES_0_BLOCK_MODE_MASK) >> SEC_ENG_SE_AES_0_BLOCK_MODE_SHIFT;

    regval &= ~SEC_ENG_SE_AES_0_MODE_MASK;
    regval &= ~SEC_ENG_SE_AES_0_HW_KEY_EN;
    if (key == NULL) {
        regval |= SEC_ENG_SE_AES_0_HW_KEY_EN;
    }

    if (keybits == 128) {
        regval |= (0 << SEC_ENG_SE_AES_0_MODE_SHIFT);
    } else if (keybits == 192) {
        regval |= (2 << SEC_ENG_SE_AES_0_MODE_SHIFT);
    } else if (keybits == 256) {
        regval |= (1 << SEC_ENG_SE_AES_0_MODE_SHIFT);
    } else {
        regval |= (3 << SEC_ENG_SE_AES_0_MODE_SHIFT);
    }
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    if (key == NULL) {
        regval = getreg32(reg_base + SEC_ENG_SE_AES_0_KEY_SEL_OFFSET);
        regval &= ~SEC_ENG_SE_AES_0_KEY_SEL_MASK;
        regval |= (CONFIG_BFLB_AES_HW_KEY_SEL << SEC_ENG_SE_AES_0_KEY_SEL_SHIFT);
        putreg32(regval, reg_base + SEC_ENG_SE_AES_0_KEY_SEL_OFFSET);

        regval = getreg32(reg_base + SEC_ENG_SE_AES_1_KEY_SEL_OFFSET);
        regval &= ~SEC_ENG_SE_AES_1_KEY_SEL_MASK;
        regval |= (CONFIG_BFLB_AES_HW_KEY_SEL << SEC_ENG_SE_AES_1_KEY_SEL_SHIFT);
        putreg32(regval, reg_base + SEC_ENG_SE_AES_1_KEY_SEL_OFFSET);
    } else {
        putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_0_OFFSET);
        temp_key += 4;
        putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_1_OFFSET);
        temp_key += 4;
        putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_2_OFFSET);
        temp_key += 4;
        putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_3_OFFSET);
        temp_key += 4;

        if (keybits == 192) {
            putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_4_OFFSET);
            temp_key += 4;
            putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_5_OFFSET);
            temp_key += 4;
        } else if ((keybits == 256) || (mode == AES_MODE_XTS)) {
            putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_4_OFFSET);
            temp_key += 4;
            putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_5_OFFSET);
            temp_key += 4;
            putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_6_OFFSET);
            temp_key += 4;
            putreg32(BFLB_PUT_LE32(temp_key), reg_base + SEC_ENG_SE_AES_0_KEY_7_OFFSET);
            temp_key += 4;
        }
    }
}

int bflb_aes_encrypt(struct bflb_device_s *dev,
                     const uint8_t *input,
                     const uint8_t *iv,
                     uint8_t *output,
                     uint32_t len)
{
    uint32_t regval;
    uint32_t reg_base;
    uint8_t mode;
    uint8_t *temp_iv = (uint8_t *)iv;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    if (regval & SEC_ENG_SE_AES_0_BUSY) {
        return -1;
    }
    if (len % 16) {
        return -2;
    }

    regval &= ~SEC_ENG_SE_AES_0_TRIG_1T;
    regval &= ~SEC_ENG_SE_AES_0_IV_SEL; /* Clear aes iv sel to select new iv */
    regval &= ~SEC_ENG_SE_AES_0_DEC_EN; /* Set AES encryption */
    regval &= ~SEC_ENG_SE_AES_0_MSG_LEN_MASK;
    regval |= SEC_ENG_SE_AES_0_INT_CLR_1T;
    regval |= ((len / 16) << SEC_ENG_SE_AES_0_MSG_LEN_SHIFT);
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    mode = (regval & SEC_ENG_SE_AES_0_BLOCK_MODE_MASK) >> SEC_ENG_SE_AES_0_BLOCK_MODE_SHIFT;

    if (mode == AES_MODE_XTS) {
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        temp_iv += 4;
    } else {
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        temp_iv += 4;
    }

    /* Set input and output address */
    putreg32((uint32_t)input, reg_base + SEC_ENG_SE_AES_0_MSA_OFFSET);
    putreg32((uint32_t)output, reg_base + SEC_ENG_SE_AES_0_MDA_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_AES_0_TRIG_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    while (getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET) & SEC_ENG_SE_AES_0_BUSY) {
    }
    return 0;
}

int bflb_aes_decrypt(struct bflb_device_s *dev,
                     const uint8_t *input,
                     const uint8_t *iv,
                     uint8_t *output,
                     uint32_t len)
{
    uint32_t regval;
    uint32_t reg_base;
    uint8_t mode;
    uint8_t *temp_iv = (uint8_t *)iv;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    if (regval & SEC_ENG_SE_AES_0_BUSY) {
        return -1;
    }

    if (len % 16) {
        return -2;
    }

    regval &= ~SEC_ENG_SE_AES_0_TRIG_1T;
    regval &= ~SEC_ENG_SE_AES_0_IV_SEL; /* Clear aes iv sel to select new iv */
    regval |= SEC_ENG_SE_AES_0_DEC_EN;  /* Set AES decryption */
    regval |= SEC_ENG_SE_AES_0_INT_CLR_1T;
    regval &= ~SEC_ENG_SE_AES_0_MSG_LEN_MASK;
    regval |= ((len / 16) << SEC_ENG_SE_AES_0_MSG_LEN_SHIFT);
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    mode = (regval & SEC_ENG_SE_AES_0_BLOCK_MODE_MASK) >> SEC_ENG_SE_AES_0_BLOCK_MODE_SHIFT;

    if (mode == AES_MODE_XTS) {
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_BE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        temp_iv += 4;
    } else {
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_0_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_1_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_2_OFFSET);
        temp_iv += 4;
        putreg32(BFLB_PUT_LE32(temp_iv), reg_base + SEC_ENG_SE_AES_0_IV_3_OFFSET);
        temp_iv += 4;
    }

    /* Set input and output address */
    putreg32((uint32_t)input, reg_base + SEC_ENG_SE_AES_0_MSA_OFFSET);
    putreg32((uint32_t)output, reg_base + SEC_ENG_SE_AES_0_MDA_OFFSET);

    regval = getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_AES_0_TRIG_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET);

    while (getreg32(reg_base + SEC_ENG_SE_AES_0_CTRL_OFFSET) & SEC_ENG_SE_AES_0_BUSY) {
    }
    return 0;
}
