#include "bflb_mfd_common.h"

static bool bflb_mfd_aes_crypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv, bool encrypt)
{
    struct bflb_device_s *aes;

    aes = bflb_device_get_by_name(BFLB_NAME_SEC_AES);
    if (0 == len || NULL == aes) {
        return false;
    }

    bflb_group0_request_aes_access(aes);

    bflb_aes_init(aes);
    bflb_aes_set_mode(aes, AES_MODE_CBC);
    bflb_aes_setkey(aes, NULL, 128);
    bflb_aes_select_hwkey(aes, 1, 0);

    bflb_l1c_dcache_clean_all();
    bflb_l1c_dcache_disable();
    int iret = encrypt
        ? bflb_aes_encrypt(aes, p, (uint8_t *)pIv, pout, len)
        : bflb_aes_decrypt(aes, p, (uint8_t *)pIv, pout, len);
    bflb_l1c_dcache_enable();

    bflb_aes_deinit(aes);
    bflb_group0_release_aes_access(aes);

    return 0 == iret;
}

bool bflb_mfd_decrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv)
{
    return bflb_mfd_aes_crypt(p, len, pout, pIv, false);
}

bool bflb_mfd_encrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv)
{
    return bflb_mfd_aes_crypt(p, len, pout, pIv, true);
}
