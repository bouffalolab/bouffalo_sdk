#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "blcrypto_sec_pka.h"

struct bflb_device_s *bflb_bignum_pka;

int bflb_crypto_aes_init(bflb_crypto_sec_aes_t *aes);
int bflb_crypto_aes_set_key(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *key, size_t key_len);
int bflb_crypto_aes_transform_blocks(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *input, uint16_t n_blk, uint8_t *output);
int bflb_crypto_aes_transform(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *input, uint8_t *output);
int bflb_crypto_aes_acquire_hw();
int bflb_crypto_aes_release_hw();
static void sec_eng_aes_enable_link(SEC_ENG_AES_ID_Type aesNo);
static BL_Err_Type sec_eng_aes_link_work(SEC_ENG_AES_ID_Type aesNo, uint32_t linkAddr, const uint8_t *in, uint32_t len, uint8_t *out);

int bflb_crypto_aes_init(bflb_crypto_sec_aes_t *aes)
{
    if (!aes) {
        return -1;
    }
    memset(aes, 0, sizeof(*aes));
    aes->link_cfg.aesIntClr = 1;
    aes->link_cfg.aesBlockMode = SEC_ENG_AES_ECB;
    return 0;
}

int bflb_crypto_aes_set_key(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *key, size_t key_len)
{
    if (!(aes && key && (key_len == 16 || key_len == 24 || key_len == 32))) {
        return -1;
    }
    (void)op;
    aes->link_cfg.aesDecKeySel = SEC_ENG_AES_USE_NEW;
    SEC_ENG_AES_Key_Type key_type;
    if (key_len == 16) {
        key_type = SEC_ENG_AES_KEY_128BITS;
    } else if (key_len == 24) {
        key_type = SEC_ENG_AES_KEY_192BITS;
    } else {
        key_type = SEC_ENG_AES_KEY_256BITS;
    }
    aes->link_cfg.aesMode = key_type;
    memcpy(&aes->link_cfg.aesKey0, key, key_len);
    return 0;
}

int bflb_crypto_aes_transform_blocks(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *input, uint16_t n_blk, uint8_t *output)
{
    int ret;
    size_t bytes;
    if (!(aes && input && output)) {
        return -1;
    }
    if (n_blk == 0) {
        return 0;
    }
    bytes = n_blk << 4;
    if (op == BL_AES_ENCRYPT) {
        aes->link_cfg.aesDecEn = SEC_ENG_AES_ENCRYPTION;
    } else {
        aes->link_cfg.aesDecEn = SEC_ENG_AES_DECRYPTION;
    }
    sec_eng_aes_enable_link(AES_ID);
    ret=sec_eng_aes_link_work(AES_ID,(uint32_t)&aes->link_cfg,input,bytes,output);
    sec_eng_aes_enable_link(AES_ID);
    return !(ret==SUCCESS);
}

int bflb_crypto_aes_transform(bflb_crypto_sec_aes_t *aes, bflb_crypto_sec_aes_op_t op, const uint8_t *input, uint8_t *output)
{
    return bflb_crypto_aes_transform_blocks(aes, op, input, 1, output);
}

int bflb_crypto_aes_acquire_hw()
{
    taskENTER_CRITICAL();
    return 0;
}

int bflb_crypto_aes_release_hw()
{
    taskEXIT_CRITICAL();
    return 0;
}

static void sec_eng_aes_enable_link(SEC_ENG_AES_ID_Type aesNo)
{
    struct bflb_device_s *aes_dev;
    CHECK_PARAM(IS_SEC_ENG_AES_ID_TYPE(aesNo));
    aes_dev = bflb_device_get_by_name("sec_aes");
    bflb_aes_link_init(aes_dev);
    uint32_t reg_base = aes_dev->reg_base;
    putreg32(0x03, reg_base + SEC_ENG_SE_AES_0_CTRL_PROT_OFFSET);
}

static BL_Err_Type sec_eng_aes_link_work(SEC_ENG_AES_ID_Type aesNo, uint32_t linkAddr, const uint8_t *in, uint32_t len, uint8_t *out)
{
    struct bflb_device_s *aes_dev;
    int ret;
    CHECK_PARAM(IS_SEC_ENG_AES_ID_TYPE(aesNo));
    aes_dev = bflb_device_get_by_name("sec_aes");
    ret = bflb_aes_link_update(aes_dev, linkAddr, in, out, len);
    if (ret == 0) {
        return SUCCESS;
    } else if (ret == -ETIMEDOUT) {
        return TIMEOUT;
    } else {
        return ERROR;
    }
}
