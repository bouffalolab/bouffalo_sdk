#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include "bflb_sec_aes.h"

/**
 * \brief The AES context-type definition.
 */
typedef struct mbedtls_aes_context
{
    struct bflb_device_s *aes;
    struct bflb_aes_link_s link_ctx;
}
mbedtls_aes_context __attribute__((aligned(32)));

#if defined(MBEDTLS_CIPHER_MODE_XTS)
/**
 * \brief The AES XTS context-type definition.
 */
typedef struct mbedtls_aes_xts_context
{
    mbedtls_aes_context crypt; /*!< The AES context to use for AES block
                                        encryption or decryption. */
    mbedtls_aes_context tweak; /*!< The AES context used for tweak
                                        computation. */
} mbedtls_aes_xts_context;
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#endif /* aes_alt.h */
