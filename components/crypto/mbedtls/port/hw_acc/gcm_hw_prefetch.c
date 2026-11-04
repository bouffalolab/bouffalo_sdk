/*
 * GCM-only adapter for batching consecutive AES counter blocks.  Include the
 * checked-out mbedTLS implementation so the SDK does not carry a fork of it.
 */
#include "common.h"
#include "mbedtls/gcm.h"
#include "mbedtls/cipher.h"

#if defined(MBEDTLS_GCM_C) && defined(MBEDTLS_AES_ALT) && \
    !defined(MBEDTLS_GCM_ALT) && !defined(MBEDTLS_BLOCK_CIPHER_C) && \
    !defined(MBEDTLS_USE_PSA_CRYPTO)
#include "gcm_hw_prefetch.h"

static int gcm_cipher_update(mbedtls_cipher_context_t *ctx,
                             const unsigned char *input, size_t length,
                             unsigned char *output, size_t *output_length);

/* Only the GCM implementation's cipher calls use this adapter. */
#define mbedtls_cipher_update gcm_cipher_update
#include "../../mbedtls_v3/library/gcm.c"
#undef mbedtls_cipher_update

static int gcm_cipher_update(mbedtls_cipher_context_t *ctx,
                             const unsigned char *input, size_t length,
                             unsigned char *output, size_t *output_length)
{
    mbedtls_cipher_type_t type = mbedtls_cipher_get_type(ctx);

    if (length == 16 && input && output && output_length &&
        mbedtls_cipher_get_operation(ctx) == MBEDTLS_ENCRYPT &&
        (type == MBEDTLS_CIPHER_AES_128_ECB ||
         type == MBEDTLS_CIPHER_AES_192_ECB ||
         type == MBEDTLS_CIPHER_AES_256_ECB)) {
        mbedtls_aes_context *aes =
            (mbedtls_aes_context *)ctx->MBEDTLS_PRIVATE(cipher_ctx);

        /* These modes can reuse a hardware key independently of the context. */
        if (aes && !aes->link_ctx.aes_hwkey_en &&
            !aes->link_ctx.aes_newkey_dis) {
            int ret = gcm_hw_prefetch_encrypt_block(aes, input, output);

            if (ret == 0) *output_length = 16;
            return ret;
        }
    }
    return mbedtls_cipher_update(ctx, input, length, output, output_length);
}
#else
/* Configurations using another GCM or block-cipher backend remain unchanged. */
#include "../../mbedtls_v3/library/gcm.c"
#endif
