#ifndef MBEDTLS_SM4_H
#define MBEDTLS_SM4_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdint.h>
#include <string.h>

#define MBEDTLS_SM4_ENCRYPT 1
#define MBEDTLS_SM4_DECRYPT 0

#define MBEDTLS_ERR_SM4_INVALID_INPUT_LENGTH        -0x0054

#define MBEDTLS_SM4_KEY_SIZE 16

#if !defined(MBEDTLS_SM4_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           SM4 context structure
 */
typedef struct
{
    uint32_t sk[32];            /*!<  SM4 subkeys       */
}
mbedtls_sm4_context;

/**
 * \brief          Initialize SM4 context
 *
 * \param ctx      SM4 context to be initialized
 */
void mbedtls_sm4_init(mbedtls_sm4_context *ctx);
/**
 * \brief          Clear SM4 context
 *
 * \param ctx      SM4 context to be cleared
 */
void mbedtls_sm4_free(mbedtls_sm4_context *ctx);
/**
 * \brief          SM4 key schedule (encryption)
 *
 * \param ctx      SM4 context to be initialized
 * \param key      <MBEDTLS_SM4_KEY_SIZE> byte secret key
 *
 * \return         0
 */
int mbedtls_sm4_setkey_enc(mbedtls_sm4_context *ctx,
        const unsigned char key[MBEDTLS_SM4_KEY_SIZE]);
/**
 * \brief          SM4 key schedule (decryption)
 *
 * \param ctx      SM4 context to be initialized
 * \param key      <MBEDTLS_SM4_KEY_SIZE> byte secret key
 *
 * \return         0
 */
int mbedtls_sm4_setkey_dec(mbedtls_sm4_context *ctx,
        const unsigned char key[MBEDTLS_SM4_KEY_SIZE]);

/**
 * \brief          SM4-ECB block encryption/decryption
 *
 * \param ctx      SM4 context
 * \param input    <MBEDTLS_SM4_KEY_SIZE> byte input block
 * \param output   <MBEDTLS_SM4_KEY_SIZE> byte output block
 *
 * \return         0 if successful
 */
int mbedtls_sm4_crypt_ecb(mbedtls_sm4_context *ctx, int mode,
        const unsigned char input[MBEDTLS_SM4_KEY_SIZE],
        unsigned char output[MBEDTLS_SM4_KEY_SIZE]);
#if defined(MBEDTLS_CIPHER_MODE_CBC)
/**
 * \brief          SM4-CBC buffer encryption/decryption
 *
 * \param ctx      SM4 context
 * \param mode     MBEDTLS_SM4_ENCRYPT or MBEDTLS_SM4_DECRYPT
 * \param length   length of the input data
 * \param iv       initialization vector (updated after use)
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 *
 * \return         0 if successful, or MBEDTLS_ERR_SM4_INVALID_INPUT_LENGTH
 */
int mbedtls_sm4_crypt_cbc(mbedtls_sm4_context *ctx, int mode, size_t length,
        unsigned char iv[MBEDTLS_SM4_KEY_SIZE],
        const unsigned char *input, unsigned char *output);
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#ifdef __cplusplus
}
#endif

#else  /* MBEDTLS_SM4_ALT */
#include "sm4_alt.h"
#endif /* MBEDTLS_SM4_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int mbedtls_sm4_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* sm4.h */