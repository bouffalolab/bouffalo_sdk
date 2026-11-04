/* Mbed TLS GCM_ALT: delegate to an independently compiled upstream backend. */
#include "mbedtls/gcm.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "gcm_software_backend.h"

#if !defined(MBEDTLS_GCM_ALT) || !defined(MBEDTLS_GCM_C)
#error "Software GCM requires MBEDTLS_GCM_C and MBEDTLS_GCM_ALT"
#endif
#if defined(MBEDTLS_ARIA_C)
#error "Software GCM backend does not support ARIA configured via user headers"
#endif
#if defined(MBEDTLS_CAMELLIA_C) && !defined(CONFIG_MBEDTLS_CIPHER_CAMELLIA_ENABLED)
#error "Enable Camellia through Kconfig when using software GCM"
#endif

void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
    ctx->MBEDTLS_PRIVATE(backend) = NULL;
}

int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx, mbedtls_cipher_id_t cipher,
                       const unsigned char *key, unsigned int keybits)
{
    if (!ctx || !key) return MBEDTLS_ERR_GCM_BAD_INPUT;
    if (!ctx->MBEDTLS_PRIVATE(backend)) {
        ctx->MBEDTLS_PRIVATE(backend) = mbedtls_calloc(1, gcm_software_backend_context_size());
        if (!ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_CIPHER_ALLOC_FAILED;
        gcm_software_backend_init(ctx->MBEDTLS_PRIVATE(backend));
    }
    return gcm_software_backend_setkey(ctx->MBEDTLS_PRIVATE(backend), (int)cipher, key, keybits);
}

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx, int mode,
                       const unsigned char *iv, size_t iv_len)
{
    if (!ctx || !ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_GCM_BAD_INPUT;
    return gcm_software_backend_starts(ctx->MBEDTLS_PRIVATE(backend), mode, iv, iv_len);
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx, const unsigned char *add, size_t add_len)
{
    if (!ctx || !ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_GCM_BAD_INPUT;
    return gcm_software_backend_update_ad(ctx->MBEDTLS_PRIVATE(backend), add, add_len);
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx, const unsigned char *input, size_t input_length,
                       unsigned char *output, size_t output_size, size_t *output_length)
{
    if (!ctx || !ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_GCM_BAD_INPUT;
    return gcm_software_backend_update(ctx->MBEDTLS_PRIVATE(backend), input, input_length, output, output_size, output_length);
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx, unsigned char *output, size_t output_size,
                       size_t *output_length, unsigned char *tag, size_t tag_len)
{
    if (!ctx || !ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_GCM_BAD_INPUT;
    return gcm_software_backend_finish(ctx->MBEDTLS_PRIVATE(backend), output, output_size, output_length, tag, tag_len);
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx, int mode, size_t length,
                              const unsigned char *iv, size_t iv_len, const unsigned char *add,
                              size_t add_len, const unsigned char *input, unsigned char *output,
                              size_t tag_len, unsigned char *tag)
{
    if (!ctx || !ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_GCM_BAD_INPUT;
    return gcm_software_backend_crypt_and_tag(ctx->MBEDTLS_PRIVATE(backend), mode, length, iv, iv_len, add, add_len,
                                            input, output, tag_len, tag);
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx, size_t length, const unsigned char *iv,
                             size_t iv_len, const unsigned char *add, size_t add_len,
                             const unsigned char *tag, size_t tag_len, const unsigned char *input,
                             unsigned char *output)
{
    if (!ctx || !ctx->MBEDTLS_PRIVATE(backend)) return MBEDTLS_ERR_GCM_BAD_INPUT;
    return gcm_software_backend_auth_decrypt(ctx->MBEDTLS_PRIVATE(backend), length, iv, iv_len, add, add_len,
                                          tag, tag_len, input, output);
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
    if (!ctx) return;
    if (ctx->MBEDTLS_PRIVATE(backend)) {
        gcm_software_backend_free(ctx->MBEDTLS_PRIVATE(backend));
        mbedtls_free(ctx->MBEDTLS_PRIVATE(backend));
    }
    mbedtls_platform_zeroize(ctx, sizeof(*ctx));
}
