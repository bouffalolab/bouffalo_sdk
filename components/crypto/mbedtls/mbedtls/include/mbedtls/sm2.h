#ifndef MBEDTLS_SM2_H
#define MBEDTLS_SM2_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <string.h>

#include "mbedtls/ecp.h"
#include "mbedtls/md.h"

#define MBEDTLS_ERR_SM2_BAD_INPUT_DATA      -0x4800 /*!< Bad input parameters to function. */
#define MBEDTLS_ERR_SM2_ALLOC_FAILED        -0x4880 /*!< Memory allocation failed. */
#define MBEDTLS_ERR_SM2_KDF_FAILED          -0x4900 /*!< KDF got empty result. */
#define MBEDTLS_ERR_SM2_DECRYPT_BAD_HASH    -0x4980 /*!< Bad C3 in SM2 decrypt */
#define MBEDTLS_ERR_SM2_RANDOM_FAILED       -0x4A00 /*!< Generation of random value, such as (ephemeral) key, failed. */
#define MBEDTLS_ERR_SM2_BAD_SIGNATURE       -0x4A80 /*!< Invalid signature */

#define MBEDTLS_SM2_SPECIFIC_MD_ALGORITHM   MBEDTLS_MD_SM3
#define MBEDTLS_SM2_GMT09_DEFAULT_ID        "1234567812345678"

/**
 *  Enum for the point conversion form as defined in X9.62 (ECDSA)
 *  for the encoding of a elliptic curve point (x,y) */
typedef enum {
    /** the point is encoded as z||x, where the octet z specifies
     *          *  which solution of the quadratic equation y is  */
    MBEDTLS_ECP_POINT_CONVERSION_COMPRESSED = 2,
    /** the point is encoded as z||x||y, where z is the octet 0x02  */
    MBEDTLS_ECP_POINT_CONVERSION_UNCOMPRESSED = 4,
    /** the point is encoded as z||x||y, where the octet z specifies
     *          *  which solution of the quadratic equation y is  */
    MBEDTLS_ECP_POINT_CONVERSION_HYBRID = 6
} mbedtls_ecp_point_conversion_form_t;

#if !defined(MBEDTLS_SM2_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SM2 context structure
 */
typedef mbedtls_ecp_keypair mbedtls_sm2_context;

/**
 * \brief           Perform SM2 encryption
 *
 * \param ctx       SM2 context
 * \param md_alg    Algorithm that was used to hash the message
 * \param input     the plaintext to be encrypted
 * \param ilen      the plaintext length
 * \param output    buffer that will hold the plaintext
 * \param olen      will contain the plaintext length
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful, or a error code
 */
int mbedtls_sm2_encrypt(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *input, size_t ilen,
        unsigned char *output, size_t *olen,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);

/**
 * \brief           Perform SM2 decryption
 *
 * \param ctx       SM2 context
 * \param md_alg    Algorithm that was used to hash the message
 * \param input     encrypted data
 * \param ilen      the encrypted data length
 * \param output    buffer that will hold the plaintext
 * \param olen      will contain the plaintext length
 *
 * \return          0 if successful, or a error code
 */
int mbedtls_sm2_decrypt(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *input, size_t ilen,
        unsigned char *output, size_t *olen);

/**
 * \brief           Compute SM2 signature of a previously hashed message
 *
 * \param ctx       SM2 context
 * \param md_alg    Algorithm that was used to hash the message
 * \param hash      Message digest
 * \param sig       Buffer holding the signature data
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful, or a error code
 */
int mbedtls_sm2_sign(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *hash, unsigned char *sig,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);

/**
 * \brief           Verify SM2 signature of a previously hashed message
 *
 * \param ctx       SM2 context
 * \param md_alg    Algorithm that was used to hash the message
 * \param hash      Message digest
 * \param sig       Signature to verify
 *
 * \return          0 if successful, or a error code
 */
int mbedtls_sm2_verify(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *hash, const unsigned char *sig);

/**
 * \brief           Hash Z with ID and public key
 *
 * \param ctx       SM2 context
 * \param md_alg    Algorithm that was used to hash the message
 * \param id        User ID, character string.
 *                  GM/T 0009-2012 Public Key Infrastructure Application
 *                      Technology SM2 Cryptography Specification: Section 10:
 *                      In the case Without specific agreement, the default ID
 *                      value is: "1234567812345678" (16 byte)
 * \param idlen     User ID string length
 * \param z         Buffer that will hold the Z
 *
 * \return          0 if successful, or a error code
 */
int mbedtls_sm2_hash_z(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const char *id, size_t idlen, unsigned char *z);

/**
 * \brief           Hash E with Z and message
 *
 * \param md_alg    Algorithm that was used to hash the message
 * \param z         Z computed by mbedtls_sm2_hash_z
 * \param input     buffer holding the data
 * \param ilen      length of the input data
 * \param e         Buffer that will hold the E
 *
 * \return          0 if successful, or a error code
 */
int mbedtls_sm2_hash_e(mbedtls_md_type_t md_alg, const unsigned char *z,
        const unsigned char *input, size_t ilen, unsigned char *e);

/**
 * \brief           Generate an SM2 keypair on the given curve
 *
 * \param ctx       SM2 context
 * \param gid       Group (elliptic curve) to use. One of the various
 *                  MBEDTLS_ECP_DP_XXX macros depending on configuration.
 * \param f_rng     RNG function
 * \param p_rng     RNG parameter
 *
 * \return          0 if successful, or a error code.
 */
int mbedtls_sm2_genkey(mbedtls_sm2_context *ctx, mbedtls_ecp_group_id gid,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);

/**
 * \brief           Set an SM2 context from an EC key pair
 *
 * \param ctx       SM2 context to set
 * \param key       EC key to use
 *
 * \return          0 on success, or a MBEDTLS_ERR_ECP_XXX code.
 */
int mbedtls_sm2_from_keypair(mbedtls_sm2_context *ctx,
        const mbedtls_ecp_keypair *key);

/**
 * \brief           Initialize context
 *
 * \param ctx       Context to be initialized
 */
void mbedtls_sm2_init(mbedtls_sm2_context *ctx);

/**
 * \brief           Free context
 *
 * \param ctx       Context to free
 */
void mbedtls_sm2_free(mbedtls_sm2_context *ctx);

#ifdef __cplusplus
}
#endif

#else  /* MBEDTLS_SM2_ALT */
#include "sm2_alt.h"
#endif /* MBEDTLS_SM2_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Checkup routine
 *
 * \return          0 if successful, or 1 if the test failed
 */
int mbedtls_sm2_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* sm2.h */