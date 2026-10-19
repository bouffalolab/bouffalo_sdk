/*
 * SM3 Encryption alogrithm
 * GM/T 0004-2012 Chinese National Standard refers to: http://www.oscca.gov.cn/ 
 * Thanks to MbedTLS.
 * Thanks to author: goldboar (goldboar@163.com).
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SM3_C)

#include "mbedtls/sm3.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf      printf
#endif /* MBEDTLS_PLATFORM_C */

#if !defined(MBEDTLS_SM3_ALT)

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

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

void mbedtls_sm3_init(mbedtls_sm3_context *ctx)
{
    memset(ctx, 0, sizeof(mbedtls_sm3_context));
}

void mbedtls_sm3_free( mbedtls_sm3_context *ctx )
{
    if(ctx == NULL)
        return;

    mbedtls_zeroize(ctx, sizeof(mbedtls_sm3_context));
}

void mbedtls_sm3_clone(mbedtls_sm3_context *dst,
        const mbedtls_sm3_context *src)
{
    *dst = *src;
}

/*
 * SM3 context setup
 */
void mbedtls_sm3_starts(mbedtls_sm3_context *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x7380166F;
    ctx->state[1] = 0x4914B2B9;
    ctx->state[2] = 0x172442D7;
    ctx->state[3] = 0xDA8A0600;
    ctx->state[4] = 0xA96F30BC;
    ctx->state[5] = 0x163138AA;
    ctx->state[6] = 0xE38DEE4D;
    ctx->state[7] = 0xB0FB0E4E;
}

#if !defined(MBEDTLS_SM3_PROCESS_ALT)
void mbedtls_sm3_process(mbedtls_sm3_context *ctx,
        const unsigned char data[64])
{
    uint32_t SS1, SS2, TT1, TT2, W[68], W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t T[64];
    uint32_t Temp1, Temp2, Temp3, Temp4, Temp5;
    int j;

    for (j = 0; j < 16; j++)
        T[j] = 0x79CC4519;
    for (j = 16; j < 64; j++)
        T[j] = 0x7A879D8A;

    GET_UINT32_BE( W[ 0], data,  0 );
    GET_UINT32_BE( W[ 1], data,  4 );
    GET_UINT32_BE( W[ 2], data,  8 );
    GET_UINT32_BE( W[ 3], data, 12 );
    GET_UINT32_BE( W[ 4], data, 16 );
    GET_UINT32_BE( W[ 5], data, 20 );
    GET_UINT32_BE( W[ 6], data, 24 );
    GET_UINT32_BE( W[ 7], data, 28 );
    GET_UINT32_BE( W[ 8], data, 32 );
    GET_UINT32_BE( W[ 9], data, 36 );
    GET_UINT32_BE( W[10], data, 40 );
    GET_UINT32_BE( W[11], data, 44 );
    GET_UINT32_BE( W[12], data, 48 );
    GET_UINT32_BE( W[13], data, 52 );
    GET_UINT32_BE( W[14], data, 56 );
    GET_UINT32_BE( W[15], data, 60 );

#define FF0(x,y,z) ( (x) ^ (y) ^ (z))
#define FF1(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

#define GG0(x,y,z) ( (x) ^ (y) ^ (z))
#define GG1(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )


#define  SHL(x,n) (((x) & 0xFFFFFFFF) << (n % 32))
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - (n % 32))))

#define P0(x) ((x) ^  ROTL((x),9) ^ ROTL((x),17))
#define P1(x) ((x) ^  ROTL((x),15) ^ ROTL((x),23))

    for (j = 16; j < 68; j++) {
        //W[j] = P1( W[j-16] ^ W[j-9] ^ ROTL(W[j-3],15)) ^ ROTL(W[j - 13],7 ) ^ W[j-6];
        //Why thd release's result is different with the debug's ?
        //Below is okay. Interesting, Perhaps VC6 has a bug of Optimizaiton.

        Temp1 = W[j - 16] ^ W[j - 9];
        Temp2 = ROTL(W[j - 3], 15);
        Temp3 = Temp1 ^ Temp2;
        Temp4 = P1(Temp3);
        Temp5 =  ROTL(W[j - 13], 7 ) ^ W[j - 6];
        W[j] = Temp4 ^ Temp5;
    }
    for (j = 0; j < 64; j++) {
        W1[j] = W[j] ^ W[j+4];
    }
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];
    for (j = 0; j < 16; j++) {
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(T[j], j)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF0(A, B, C) + D + SS2 + W1[j];
        TT2 = GG0(E, F, G) + H + SS1 + W[j];
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }

    for (j = 16; j < 64; j++) {
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(T[j], j)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF1(A, B, C) + D + SS2 + W1[j];
        TT2 = GG1(E, F, G) + H + SS1 + W[j];
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }

    ctx->state[0] ^= A;
    ctx->state[1] ^= B;
    ctx->state[2] ^= C;
    ctx->state[3] ^= D;
    ctx->state[4] ^= E;
    ctx->state[5] ^= F;
    ctx->state[6] ^= G;
    ctx->state[7] ^= H;
}
#endif /* !MBEDTLS_SM3_PROCESS_ALT */

/*
 * SM3 process buffer
 */
void mbedtls_sm3_update(mbedtls_sm3_context *ctx,
        const unsigned char *input, int ilen)
{
    int fill;
    uint32_t left;

    if (ilen <= 0)
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < (uint32_t) ilen)
        ctx->total[1]++;

    if (left && ilen >= fill) {
        memcpy((void *) (ctx->buffer + left), (void *) input, fill);
        mbedtls_sm3_process(ctx, ctx->buffer);
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while (ilen >= 64) {
        mbedtls_sm3_process(ctx, input);
        input += 64;
        ilen  -= 64;
    }

    if (ilen > 0) {
        memcpy((void *) (ctx->buffer + left), (void *) input, ilen);
    }
}

static const unsigned char sm3_padding[64] = {
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SM3 final digest
 */
void mbedtls_sm3_finish(mbedtls_sm3_context *ctx, unsigned char output[32])
{
    uint32_t last, padn;
    uint32_t high, low;
    unsigned char msglen[8];

    high = (ctx->total[0] >> 29)
         | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    PUT_UINT32_BE(high, msglen, 0);
    PUT_UINT32_BE(low,  msglen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    mbedtls_sm3_update(ctx, (unsigned char *) sm3_padding, padn);
    mbedtls_sm3_update(ctx, msglen, 8);

    PUT_UINT32_BE(ctx->state[0], output,  0);
    PUT_UINT32_BE(ctx->state[1], output,  4);
    PUT_UINT32_BE(ctx->state[2], output,  8);
    PUT_UINT32_BE(ctx->state[3], output, 12);
    PUT_UINT32_BE(ctx->state[4], output, 16);
    PUT_UINT32_BE(ctx->state[5], output, 20);
    PUT_UINT32_BE(ctx->state[6], output, 24);
    PUT_UINT32_BE(ctx->state[7], output, 28);
}

#endif /* !MBEDTLS_SM3_ALT */

/*
 * output = SM3(input buffer)
 */
void mbedtls_sm3(const unsigned char *input, int ilen,
        unsigned char output[32])
{
    mbedtls_sm3_context ctx;

    mbedtls_sm3_starts(&ctx);
    mbedtls_sm3_update(&ctx, input, ilen);
    mbedtls_sm3_finish(&ctx, output);

    memset(&ctx, 0, sizeof(mbedtls_sm3_context));
}

#if defined(MBEDTLS_SM3_FILE) && defined(MBEDTLS_FS_IO)
/*
 * output = SM3(file contents)
 */
int mbedtls_sm3_file(const char *path, unsigned char output[32])
{
    FILE *f;
    size_t n;
    mbedtls_sm3_context ctx;
    unsigned char buf[1024];

    if ((f = fopen(path, "rb")) == NULL)
        return (1);

    mbedtls_sm3_starts(&ctx);

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        mbedtls_sm3_update(&ctx, buf, (int)n);

    mbedtls_sm3_finish(&ctx, output);

    memset(&ctx, 0, sizeof(mbedtls_sm3_context));

    if (ferror(f) != 0) {
        fclose(f);
        return (2);
    }

    fclose(f);
    return (0);
}
#endif /* MBEDTLS_SM3_FILE && MBEDTLS_FS_IO */

#if defined(MBEDTLS_SM3_HMAC_C)
#if !defined(MBEDTLS_SM3_HMAC_ALT)

/*
 * SM3 HMAC context setup
 */
void mbedtls_sm3_hmac_starts(mbedtls_sm3_context *ctx,
        unsigned char *key, int keylen)
{
    int i;
    unsigned char sum[32];

    if (keylen > 64) {
        mbedtls_sm3(key, keylen, sum);
        keylen = 32;
        // keylen = (is224) ? 28 : 32;
        key = sum;
    }

    memset(ctx->ipad, 0x36, 64);
    memset(ctx->opad, 0x5C, 64);

    for (i = 0; i < keylen; i++) {
        ctx->ipad[i] = (unsigned char)(ctx->ipad[i] ^ key[i]);
        ctx->opad[i] = (unsigned char)(ctx->opad[i] ^ key[i]);
    }

    mbedtls_sm3_starts(ctx);
    mbedtls_sm3_update(ctx, ctx->ipad, 64);

    memset(sum, 0, sizeof(sum));
}

/*
 * SM3 HMAC process buffer
 */
void mbedtls_sm3_hmac_update(mbedtls_sm3_context *ctx,
        unsigned char *input, int ilen)
{
    mbedtls_sm3_update(ctx, input, ilen);
}

/*
 * SM3 HMAC final digest
 */
void mbedtls_sm3_hmac_finish(mbedtls_sm3_context *ctx, unsigned char output[32])
{
    int hlen;
    unsigned char tmpbuf[32];

    // is224 = ctx->is224;
    hlen =  32;

    mbedtls_sm3_finish(ctx, tmpbuf);
    mbedtls_sm3_starts(ctx);
    mbedtls_sm3_update(ctx, ctx->opad, 64);
    mbedtls_sm3_update(ctx, tmpbuf, hlen);
    mbedtls_sm3_finish(ctx, output);

    memset(tmpbuf, 0, sizeof(tmpbuf));
}

#endif /* !MBEDTLS_SM3_HMAC_ALT */

/*
 * output = HMAC-SM#(hmac key, input buffer)
 */
void mbedtls_sm3_hmac(unsigned char *key, int keylen,
        unsigned char *input, int ilen,
        unsigned char output[32])
{
    mbedtls_sm3_context ctx;

    mbedtls_sm3_hmac_starts(&ctx, key, keylen);
    mbedtls_sm3_hmac_update(&ctx, input, ilen);
    mbedtls_sm3_hmac_finish(&ctx, output);

    memset(&ctx, 0, sizeof(mbedtls_sm3_context));
}

#endif /* MBEDTLS_SM3_HMAC_C */

#if defined(MBEDTLS_SELF_TEST)

/*
 * SM3 test vectors from: GM/T 0004-2012 Chinese National Standard
 */
static const unsigned char sm3_test_buf[2][65] = {
    { "abc" },
    { "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd" },
};

static const int sm3_test_buflen[2] = {
    3, 64,
};

static const unsigned char sm3_test_sum[2][32] = {
    {   0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
        0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
        0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
        0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0, },
    {   0xde, 0xbe, 0x9f, 0xf9, 0x22, 0x75, 0xb8, 0xa1,
        0x38, 0x60, 0x48, 0x89, 0xc1, 0x8e, 0x5a, 0x4d,
        0x6f, 0xdb, 0x70, 0xe5, 0x38, 0x7e, 0x57, 0x65,
        0x29, 0x3d, 0xcb, 0xa3, 0x9c, 0x0c, 0x57, 0x32, },
};

int mbedtls_sm3_self_test(int verbose)
{
    int i;
    int ret = 0;
    unsigned char sm3sum[32];
    mbedtls_sm3_context ctx;

    mbedtls_sm3_init(&ctx);

    for (i = 0; i < 2; i++) {
        if (verbose != 0) {
            mbedtls_printf("  SM3 test #%d: ", i + 1);
        }
        mbedtls_sm3_starts(&ctx);
        mbedtls_sm3_update(&ctx, sm3_test_buf[i], sm3_test_buflen[i]);
        mbedtls_sm3_finish(&ctx, sm3sum);

        if (memcmp(sm3sum, sm3_test_sum[i], 32) != 0) {
            int j;
            mbedtls_printf("\n sum:");
            for (j = 0; j < 32; j++) {
                mbedtls_printf(" %02x", sm3sum[j] & 0xff);
            }
            mbedtls_printf("\n chk:");
            for (j = 0; j < 32; j++) {
                mbedtls_printf(" %02x", sm3_test_sum[i][j] & 0xff);
            }
            mbedtls_printf("\n");
            if (verbose != 0) {
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
exit:
    mbedtls_sm3_free(&ctx);
    return (ret);
}
#endif /* MBEDTLS_SELF_TEST */

#endif /* MBEDTLS_SM3_C */