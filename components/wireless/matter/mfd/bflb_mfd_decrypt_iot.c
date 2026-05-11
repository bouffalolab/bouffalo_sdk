#if defined BL602
#include "bflb_mfd_bl602.h"
#elif defined BL702
#include "bflb_mfd_bl702.h"
#elif defined BL702L
#include "bflb_mfd_bl702l.h"
#endif

static bool bflb_mfd_aes_crypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv, bool encrypt)
{
    SEC_Eng_AES_Ctx aesCtx;
    uint8_t keySel = 1;

    if (0 == len) {
        return false;
    }

    memcpy(pout, p, len);
    SEC_ENG_REQUEST_AES_ACCESS();

    Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID0);

    Sec_Eng_AES_Init(&aesCtx, SEC_ENG_AES_ID0, SEC_ENG_AES_CBC, SEC_ENG_AES_KEY_128BITS,
        encrypt ? SEC_ENG_AES_ENCRYPTION : SEC_ENG_AES_DECRYPTION);
    Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0, SEC_ENG_AES_KEY_HW, &keySel, (const uint8_t *)pIv);

    Sec_Eng_AES_Crypt(&aesCtx, SEC_ENG_AES_ID0, pout, len, pout);
    Sec_Eng_AES_Finish(SEC_ENG_AES_ID0);

    SEC_ENG_RELEASE_AES_ACCESS();

    return true;
}

bool bflb_mfd_decrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv)
{
    return bflb_mfd_aes_crypt(p, len, pout, pIv, false);
}

bool bflb_mfd_encrypt(uint8_t *p, uint32_t len, uint8_t *pout, uint32_t *pIv)
{
    return bflb_mfd_aes_crypt(p, len, pout, pIv, true);
}
