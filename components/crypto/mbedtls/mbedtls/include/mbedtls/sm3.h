#ifndef MBEDTLS_SM3_H
#define MBEDTLS_SM3_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include <stdint.h>
#include <string.h>

#if !defined(MBEDTLS_SM3_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          SM3 context structure
 */
typedef struct {
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
}
mbedtls_sm3_context;

/**
 * \brief          Initialize SM3 context
 *
 * \param ctx      SM3 context to be initialized
 */
void mbedtls_sm3_init(mbedtls_sm3_context *ctx);

/**
 * \brief          Clear SM3 context
 *
 * \param ctx      SM3 context to be cleared
 */
void mbedtls_sm3_free(mbedtls_sm3_context *ctx);

/**
 * \brief          Clone (the state of) a SM3 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void mbedtls_sm3_clone(mbedtls_sm3_context *dst,
        const mbedtls_sm3_context *src);

/**
 * \brief          SM3 context setup
 *
 * \param ctx      context to be initialized
 */
void mbedtls_sm3_starts(mbedtls_sm3_context *ctx);

/**
 * \brief          SM3 process buffer
 *
 * \param ctx      SM3 context
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 */
void mbedtls_sm3_update(mbedtls_sm3_context *ctx,
        const unsigned char *input, int ilen);

/**
 * \brief          SM3 final digest
 *
 * \param ctx      SM3 context
 * \param output   SM3 checksum result
 */
void mbedtls_sm3_finish(mbedtls_sm3_context *ctx, unsigned char output[32]);

/* Internal use */
void mbedtls_sm3_process(mbedtls_sm3_context *ctx,
        const unsigned char data[64]);

/**
 * \brief          Output = SM3( input buffer )
 *
 * \param input    buffer holding the data
 * \param ilen     length of the input data
 * \param output   SM3 checksum result
 */
void mbedtls_sm3(const unsigned char *input, int ilen,
        unsigned char output[32]);

#ifdef __cplusplus
}
#endif

#else  /* MBEDTLS_SM3_ALT */
#include "sm3_alt.h"
#endif /* MBEDTLS_SM3_ALT */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if the test failed
 */
int mbedtls_sm3_self_test(int verbose);

#ifdef __cplusplus
}
#endif

#endif /* sm3.h */