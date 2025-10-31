#include <blcrypto_suite/blcrypto_suite_top_config.h>
#include "blcrypto_suite/blcrypto_suite_aes.h"
#include "blcrypto_sec_pka.h"

#if USE_HWCRYPTO
#include <string.h>
#include <FreeRTOS.h>

struct bflb_crypto_suite_aes {
    bflb_crypto_sec_aes_t ctx;
};

struct bflb_crypto_suite_aes *bflb_crypto_suite_aes_init()
{
    struct bflb_crypto_suite_aes *aes;

    aes = pvPortMalloc(sizeof(*aes));
    if (aes == NULL) {
        return NULL;
    }
    bflb_crypto_aes_init((bflb_crypto_sec_aes_t *)aes);

    return aes;
}

void bflb_crypto_suite_aes_deinit(struct bflb_crypto_suite_aes **aes)
{
    if (!(aes != NULL && *aes != NULL)) {
        return;
    }

    vPortFree(*aes);
    *aes = NULL;
}

int bflb_crypto_suite_aes_set_key(struct bflb_crypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *key, int bits)
{
    if (!aes) {
        return -1;
    }
    (void)mode;
    bflb_crypto_aes_set_key((bflb_crypto_sec_aes_t *)aes, BL_AES_ENCRYPT, key, bits / 8);

    return 0;
}

int bflb_crypto_suite_aes_crypt(struct bflb_crypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *in, uint8_t *out)
{
    bflb_crypto_sec_aes_op_t op;
    if (!aes) {
        return -1;
    }
    if (mode == AES_ENC) {
        op = BL_AES_ENCRYPT;
    } else {
        op = BL_AES_DECRYPT;
    }
    bflb_crypto_aes_acquire_hw();
    bflb_crypto_aes_transform((bflb_crypto_sec_aes_t *)aes, op, in, out);
    bflb_crypto_aes_release_hw();

    return 0;
}
#else
#include <FreeRTOS.h>
#include <mbedtls/aes.h>

struct bflb_crypto_suite_aes_internal {
    mbedtls_aes_context aes;
};

// only ECB is supported
struct bflb_crypto_suite_aes *bflb_crypto_suite_aes_init()
{
    struct bflb_crypto_suite_aes_internal *aes_internal;

    aes_internal = pvPortMalloc(sizeof(struct bflb_crypto_suite_aes_internal));
    if (aes_internal == NULL) {
        return NULL;
    }

    mbedtls_aes_init(&aes_internal->aes);
    return (struct bflb_crypto_suite_aes *)aes_internal;
}

void bflb_crypto_suite_aes_deinit(struct bflb_crypto_suite_aes **aes)
{
    if (!(aes != NULL && *aes != NULL)) {
        return;
    }

    vPortFree(*aes);
    *aes = NULL;
}

int bflb_crypto_suite_aes_set_key(struct bflb_crypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *key, int bits)
{
    if (!aes) {
        return -1;
    }
    struct bflb_crypto_suite_aes_internal *aes_internal = (struct bflb_crypto_suite_aes_internal *)aes;

    if (mode == AES_ENC) {
        mbedtls_aes_setkey_enc(&aes_internal->aes, key, bits);
    } else {
        mbedtls_aes_setkey_dec(&aes_internal->aes, key, bits);
    }
    return 0;
}

int bflb_crypto_suite_aes_crypt(struct bflb_crypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *in, uint8_t *out)
{
    if (!aes) {
        return -1;
    }
    struct bflb_crypto_suite_aes_internal *aes_internal = (struct bflb_crypto_suite_aes_internal *)aes;

    if (mode == AES_ENC) {
        mbedtls_aes_crypt_ecb(&aes_internal->aes, MBEDTLS_AES_ENCRYPT, in, out);
    } else {
        mbedtls_aes_crypt_ecb(&aes_internal->aes, MBEDTLS_AES_DECRYPT, in, out);
    }
    return 0;
}
#endif
