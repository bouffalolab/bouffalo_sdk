/* Compiled in the private upstream software configuration and namespace. */
#include "mbedtls/gcm.h"
#include "gcm_software_backend.h"

size_t gcm_software_backend_context_size(void)
{
    return sizeof(mbedtls_gcm_context);
}

void gcm_software_backend_init(void *ctx)
{
    mbedtls_gcm_init(ctx);
}

void gcm_software_backend_free(void *ctx)
{
    mbedtls_gcm_free(ctx);
}

int gcm_software_backend_setkey(void *ctx, int cipher,
                                const unsigned char *key, unsigned int bits)
{
    return mbedtls_gcm_setkey(ctx, (mbedtls_cipher_id_t)cipher, key, bits);
}

int gcm_software_backend_starts(void *ctx, int mode,
                                const unsigned char *iv, size_t iv_len)
{
    return mbedtls_gcm_starts(ctx, mode, iv, iv_len);
}

int gcm_software_backend_update_ad(void *ctx, const unsigned char *input, size_t len)
{
    return mbedtls_gcm_update_ad(ctx, input, len);
}

int gcm_software_backend_update(void *ctx, const unsigned char *input, size_t len,
                                unsigned char *output, size_t output_size, size_t *output_len)
{
    return mbedtls_gcm_update(ctx, input, len, output, output_size, output_len);
}

int gcm_software_backend_finish(void *ctx, unsigned char *output, size_t output_size,
                                size_t *output_len, unsigned char *tag, size_t tag_len)
{
    return mbedtls_gcm_finish(ctx, output, output_size, output_len, tag, tag_len);
}

int gcm_software_backend_crypt_and_tag(void *ctx, int mode, size_t len,
                                       const unsigned char *iv, size_t iv_len,
                                       const unsigned char *add, size_t add_len,
                                       const unsigned char *input, unsigned char *output,
                                       size_t tag_len, unsigned char *tag)
{
    return mbedtls_gcm_crypt_and_tag(ctx, mode, len, iv, iv_len, add, add_len,
                                     input, output, tag_len, tag);
}

int gcm_software_backend_auth_decrypt(void *ctx, size_t len,
                                      const unsigned char *iv, size_t iv_len,
                                      const unsigned char *add, size_t add_len,
                                      const unsigned char *tag, size_t tag_len,
                                      const unsigned char *input, unsigned char *output)
{
    return mbedtls_gcm_auth_decrypt(ctx, len, iv, iv_len, add, add_len,
                                    tag, tag_len, input, output);
}
