/*
 * SM4 Encryption alogrithm (SMS4 algorithm)
 * GM/T 0002-2012 Chinese National Standard refers to: http://www.oscca.gov.cn/ 
 * Thanks to MbedTLS.
 * Thanks to author: goldboar (goldboar@163.com).
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SM4_C)

#include "mbedtls/sm4.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf      printf
#endif /* MBEDTLS_PLATFORM_C */

#if !defined(MBEDTLS_SM4_ALT)

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

#define SWAP(a, b) { unsigned long t = a; a = b; b = t; t = 0; }

void mbedtls_sm4_init(mbedtls_sm4_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sm4_context));
}
void mbedtls_sm4_free(mbedtls_sm4_context *ctx)
{
    if(ctx == NULL) {
        return;
    }
    mbedtls_zeroize(ctx, sizeof(mbedtls_sm4_context));
}

#if !defined(MBEDTLS_SM4_SETKEY_ALT) || !defined(MBEDTLS_SM4_CRYPT_ECB_ALT)
/* rotate shift left marco definition */
#define  SHL(x, n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x, n) (SHL((x), n) | ((x) >> (32 - n)))

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
 * Expanded SM4 S-boxes
 * Sbox table: 8bits input convert to 8 bits output.
 */
static const unsigned char sbox_table[256] =
{
    0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7,
    0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
    0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3,
    0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
    0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a,
    0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
    0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95,
    0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
    0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba,
    0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
    0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b,
    0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
    0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2,
    0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
    0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52,
    0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
    0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5,
    0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
    0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55,
    0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
    0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60,
    0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
    0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f,
    0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
    0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f,
    0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
    0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd,
    0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
    0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e,
    0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
    0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20,
    0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48,
};

/**
 * \brief       look up in SboxTable and get the related value.
 * \param  [in] inch: 0x00~0xFF (8 bits unsigned value).
 */
static unsigned char sm4_sbox(unsigned char inch)
{
    unsigned char *tbl = (unsigned char *)sbox_table;
    unsigned char val = (unsigned char)(tbl[inch]);
    return val;
}
#endif /* !MBEDTLS_SM4_SETKEY_ALT || !MBEDTLS_SM4_CRYPT_ECB_ALT */

#if !defined(MBEDTLS_SM4_SETKEY_ALT)
/* Fixed parameter */
static const uint32_t ck[32] =
{
    0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
    0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
    0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
    0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
    0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
    0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
    0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
    0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279,
};

/* System parameter */
static const uint32_t fk[4] = 
{
    0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc,
};

/**
 * \brief       Calculating round encryption key.
 * \param  [in] a: a is a 32 bits unsigned value;
 * \return      sk[i]: i { 0, 1, 2, 3, ... 31, }.
 */
static uint32_t sm4_calc_rk(uint32_t ka)
{
    uint32_t bb = 0;
    uint32_t rk = 0;
    unsigned char a[4];
    unsigned char b[4];
    PUT_UINT32_BE(ka, a, 0);
    b[0] = sm4_sbox(a[0]);
    b[1] = sm4_sbox(a[1]);
    b[2] = sm4_sbox(a[2]);
    b[3] = sm4_sbox(a[3]);
    GET_UINT32_BE(bb, b, 0)
    rk = bb ^ (ROTL(bb, 13)) ^ (ROTL(bb, 23));
    return rk;
}

static void mbedtls_sm4_setkey(uint32_t sk[32],
        const unsigned char key[MBEDTLS_SM4_KEY_SIZE])
{
    unsigned long mk[4];
    unsigned long k[36];
    unsigned long i = 0;

    GET_UINT32_BE(mk[0], key, 0);
    GET_UINT32_BE(mk[1], key, 4);
    GET_UINT32_BE(mk[2], key, 8);
    GET_UINT32_BE(mk[3], key, 12);
    k[0] = mk[0] ^ fk[0];
    k[1] = mk[1] ^ fk[1];
    k[2] = mk[2] ^ fk[2];
    k[3] = mk[3] ^ fk[3];
    for (; i < 32; i++) {
        k[i + 4] = k[i] ^ (sm4_calc_rk(k[i + 1] ^ k[i + 2] ^ k[i + 3] ^ ck[i]));
        sk[i] = k[i + 4];
    }
}

/*
 * SM4 key schedule (128-bit, encryption)
 */
int mbedtls_sm4_setkey_enc(mbedtls_sm4_context *ctx,
        const unsigned char key[MBEDTLS_SM4_KEY_SIZE])
{
    mbedtls_sm4_setkey(ctx->sk, key);
    return 0;
}

/*
 * SM4 key schedule (128-bit, decryption)
 */
int mbedtls_sm4_setkey_dec(mbedtls_sm4_context *ctx,
        const unsigned char key[MBEDTLS_SM4_KEY_SIZE])
{
    int i;
    mbedtls_sm4_setkey(ctx->sk, key);
    for (i = 0; i < MBEDTLS_SM4_KEY_SIZE; i++) {
        SWAP(ctx->sk[i], ctx->sk[31 - i]);
    }
    return 0;
}
#endif /* !MBEDTLS_SM4_SETKEY_ALT */

#if !defined(MBEDTLS_SM4_CRYPT_ECB_ALT)
/**
 * \brief       "T algorithm" == "L algorithm" + "t algorithm".
 * \param  [in] a: a is a 32 bits unsigned value;
 * \return      c: c is calculated with line algorithm "L" and nonline
 *              algorithm "t"
 */
static uint32_t sm4_lt(unsigned long ka)
{
    unsigned long bb = 0;
    unsigned long c = 0;
    unsigned char a[4];
    unsigned char b[4];
    PUT_UINT32_BE(ka, a, 0);
    b[0] = sm4_sbox(a[0]);
    b[1] = sm4_sbox(a[1]);
    b[2] = sm4_sbox(a[2]);
    b[3] = sm4_sbox(a[3]);
    GET_UINT32_BE(bb, b, 0);
    c = bb ^ (ROTL(bb, 2)) ^ (ROTL(bb, 10)) ^ (ROTL(bb, 18)) ^ (ROTL(bb, 24));
    return c;
}

/**
 * \brief       Calculating and getting encryption/decryption contents.
 * \param  [in] x0: original contents;
 * \param  [in] x1: original contents;
 * \param  [in] x2: original contents;
 * \param  [in] x3: original contents;
 * \param  [in] rk: encryption/decryption key;
 * return the contents of encryption/decryption contents.
 */
static uint32_t sm4_f(uint32_t x0,
                      uint32_t x1,
                      uint32_t x2,
                      uint32_t x3,
                      uint32_t rk)
{
    return (x0 ^ sm4_lt(x1 ^ x2 ^ x3 ^ rk));
}

/*
 * SM4 standard one round processing
 */
static void sm4_one_round(uint32_t sk[32],
        const unsigned char input[MBEDTLS_SM4_KEY_SIZE],
        unsigned char output[MBEDTLS_SM4_KEY_SIZE])
{
    uint32_t i = 0;
    uint32_t b[36];

    memset(b, 0, sizeof(b));
    GET_UINT32_BE(b[0], input, 0);
    GET_UINT32_BE(b[1], input, 4);
    GET_UINT32_BE(b[2], input, 8);
    GET_UINT32_BE(b[3], input, 12);
    while (i < 32) {
        b[i + 4] = sm4_f(b[i], b[i + 1], b[i + 2], b[i + 3], sk[i]);
        i++;
    }
    PUT_UINT32_BE(b[35], output, 0);
    PUT_UINT32_BE(b[34], output, 4);
    PUT_UINT32_BE(b[33], output, 8);
    PUT_UINT32_BE(b[32], output, 12);
}

int mbedtls_sm4_crypt_ecb(mbedtls_sm4_context *ctx, int mode,
        const unsigned char input[MBEDTLS_SM4_KEY_SIZE],
        unsigned char output[MBEDTLS_SM4_KEY_SIZE])
{
    ((void) mode);
    sm4_one_round(ctx->sk, input, output);
    return 0;
}
#endif /* !MBEDTLS_SM4_CRYPT_ECB_ALT */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * SM4-CBC buffer encryption/decryption
 */
int mbedtls_sm4_crypt_cbc(mbedtls_sm4_context *ctx, int mode, size_t length,
        unsigned char iv[MBEDTLS_SM4_KEY_SIZE],
        const unsigned char *input, unsigned char *output)
{
    int i;
    unsigned char temp[MBEDTLS_SM4_KEY_SIZE];

    if (length % MBEDTLS_SM4_KEY_SIZE) {
        return (MBEDTLS_ERR_SM4_INVALID_INPUT_LENGTH);
    }

    if (mode == MBEDTLS_SM4_ENCRYPT) {
        while (length > 0) {
            for (i = 0; i < MBEDTLS_SM4_KEY_SIZE; i++) {
                output[i] = (unsigned char)(input[i] ^ iv[i]);
            }

            mbedtls_sm4_crypt_ecb(ctx, mode, output, output);
            memcpy(iv, output, MBEDTLS_SM4_KEY_SIZE);

            input  += MBEDTLS_SM4_KEY_SIZE;
            output += MBEDTLS_SM4_KEY_SIZE;
            length -= MBEDTLS_SM4_KEY_SIZE;
        }
    }
    else { /* MBEDTLS_SM4_DECRYPT */
        while (length > 0) {
            memcpy(temp, input, MBEDTLS_SM4_KEY_SIZE);
            mbedtls_sm4_crypt_ecb(ctx, mode, input, output);

            for (i = 0; i < MBEDTLS_SM4_KEY_SIZE; i++) {
                output[i] = (unsigned char)(output[i] ^ iv[i]);
            }
            memcpy(iv, temp, MBEDTLS_SM4_KEY_SIZE);

            input  += MBEDTLS_SM4_KEY_SIZE;
            output += MBEDTLS_SM4_KEY_SIZE;
            length -= MBEDTLS_SM4_KEY_SIZE;
        }
    }

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#endif /* !MBEDTLS_SM4_ALT */

#if defined(MBEDTLS_SELF_TEST)

/*
 * SM4 test vectors from: GM/T 0002-2012 Chinese National Standard
 */
static const unsigned char sm4_test_buf[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
};

static const unsigned char sm4_test_key[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
};

static const unsigned char sm4_test_ecb_enc[2][16] = {
    {   0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e,
        0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46, },
    {   0x59, 0x52, 0x98, 0xc7, 0xc6, 0xfd, 0x27, 0x1f,
        0x04, 0x02, 0xf8, 0x04, 0xc3, 0x3d, 0x3f, 0x66, },
};

int mbedtls_sm4_self_test(int verbose)
{
    int ret = 0, i, j, k, u, v;
    mbedtls_sm4_context ctx;
    unsigned char buf[16];
    unsigned char key[16];
    const unsigned char *srctext = NULL, *desttext = NULL;

    memcpy(key, sm4_test_key, 16);
    mbedtls_sm4_init(&ctx);
    for (i = 0; i < 4; i++) {
        u = i >> 1;
        v = i  & 1;

        k = 1;
        if (u == 1) {
            k = 1000000;
        }
        if (v == MBEDTLS_SM4_DECRYPT) {
            srctext = sm4_test_ecb_enc[u];
            desttext = sm4_test_buf;
            mbedtls_sm4_setkey_dec(&ctx, key);
        }
        else {
            srctext = sm4_test_buf;
            desttext = sm4_test_ecb_enc[u];
            mbedtls_sm4_setkey_enc(&ctx, key);
        }
        if (verbose) {
            mbedtls_printf("  SM4-ECB #%d (%s):", k,
                    (v == MBEDTLS_SM4_DECRYPT) ? "dec" : "enc");
        }

        memcpy(buf, srctext, 16);
        for (j = 0; j < k; j++) {
            mbedtls_sm4_crypt_ecb(&ctx, v, buf, buf);
        }
        if (memcmp(buf, desttext, 16)) {
            if (verbose) {
                mbedtls_printf("failed\n");
            }
            ret = 1;
            goto exit;
        }
        if (verbose != 0) {
            mbedtls_printf("passed\n");
        }
    }
    if (verbose != 0) {
        mbedtls_printf("\n");
    }

    ret = 0;
exit:
    mbedtls_sm4_free(&ctx);

    return (ret);
}
#endif /* MBEDTLS_SELF_TEST */

#endif /* MBEDTLS_SM4_C */