/*
 *  FIPS-197 compliant AES implementation
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#include "common.h"

#if defined(MBEDTLS_AES_C)

#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#if defined(MBEDTLS_PADLOCK_C)
#include "mbedtls/padlock.h"
#endif
#if defined(MBEDTLS_AESNI_C)
#include "mbedtls/aesni.h"
#endif

#include "mbedtls/platform.h"
#include "sec_mutex.h"

/* Parameter validation macros based on platform_util.h */
#define AES_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA )
#define AES_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE( cond )

#if defined(MBEDTLS_PADLOCK_C) &&                      \
    ( defined(MBEDTLS_HAVE_X86) || defined(MBEDTLS_PADLOCK_ALIGN16) )
static int aes_padlock_ace = -1;
#endif

static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t ecb_enc_buf[16];
static ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t ecb_dec_buf[16];

static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_aes_link_s link_ctx_temp;

void mbedtls_aes_init( mbedtls_aes_context *ctx )
{
    AES_VALIDATE( ctx != NULL );

    struct bflb_device_s *aes;

    aes = bflb_device_get_by_name("aes");

    memset( ctx, 0, sizeof( mbedtls_aes_context ) );
    ctx->aes = aes;

    bflb_group0_request_aes_access(aes);
    bflb_aes_link_init(aes);
}

void mbedtls_aes_free( mbedtls_aes_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_aes_context ) );
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void mbedtls_aes_xts_init( mbedtls_aes_xts_context *ctx )
{
    AES_VALIDATE( ctx != NULL );

    mbedtls_aes_init( &ctx->crypt );
    mbedtls_aes_init( &ctx->tweak );
}

void mbedtls_aes_xts_free( mbedtls_aes_xts_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_aes_free( &ctx->crypt );
    mbedtls_aes_free( &ctx->tweak );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

/*
 * AES key schedule (encryption)
 */
int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ctx->link_ctx.aes_dec_en = 0;

    if(keybits == 128)
    {
        ctx->link_ctx.aes_key = AES_LINK_KEY_128BITS;
    }
    else if(keybits == 192)
    {
        ctx->link_ctx.aes_key = AES_LINK_KEY_192BITS;
    }
    else if(keybits == 256)
    {
        ctx->link_ctx.aes_key = AES_LINK_KEY_256BITS;
    }
    else
    {
        return -1;
    }

    memcpy(&ctx->link_ctx.aes_key0, key, keybits / 8);
    return( 0 );
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ctx->link_ctx.aes_dec_en = 1;

    if(keybits == 128)
    {
        ctx->link_ctx.aes_key = AES_LINK_KEY_128BITS;
    }
    else if(keybits == 192)
    {
        ctx->link_ctx.aes_key = AES_LINK_KEY_192BITS;
    }
    else if(keybits == 256)
    {
        ctx->link_ctx.aes_key = AES_LINK_KEY_256BITS;
    }
    else
    {
        return -1;
    }

    memcpy(&ctx->link_ctx.aes_key0, key, keybits / 8);
    return( 0 );
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
static int mbedtls_aes_xts_decode_keys( const unsigned char *key,
                                        unsigned int keybits,
                                        const unsigned char **key1,
                                        unsigned int *key1bits,
                                        const unsigned char **key2,
                                        unsigned int *key2bits )
{
    const unsigned int half_keybits = keybits / 2;
    const unsigned int half_keybytes = half_keybits / 8;

    switch( keybits )
    {
        case 256: break;
        case 512: break;
        default : return( MBEDTLS_ERR_AES_INVALID_KEY_LENGTH );
    }

    *key1bits = half_keybits;
    *key2bits = half_keybits;
    *key1 = &key[0];
    *key2 = &key[half_keybytes];

    return 0;
}

int mbedtls_aes_xts_setkey_enc( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ret = mbedtls_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for the encryption mode. */
    ret = mbedtls_aes_setkey_enc( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for encryption. */
    return mbedtls_aes_setkey_enc( &ctx->crypt, key1, key1bits );
}

int mbedtls_aes_xts_setkey_dec( mbedtls_aes_xts_context *ctx,
                                const unsigned char *key,
                                unsigned int keybits)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const unsigned char *key1, *key2;
    unsigned int key1bits, key2bits;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( key != NULL );

    ret = mbedtls_aes_xts_decode_keys( key, keybits, &key1, &key1bits,
                                       &key2, &key2bits );
    if( ret != 0 )
        return( ret );

    /* Set the tweak key. Always set tweak key for encryption. */
    ret = mbedtls_aes_setkey_enc( &ctx->tweak, key2, key2bits );
    if( ret != 0 )
        return( ret );

    /* Set crypt key for decryption. */
    return mbedtls_aes_setkey_dec( &ctx->crypt, key1, key1bits );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#define AES_FROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)                 \
    do                                                      \
    {                                                       \
        (X0) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y0 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y1 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y2 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y3 ) );     \
                                                            \
        (X1) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y1 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y2 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y3 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y0 ) );     \
                                                            \
        (X2) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y2 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y3 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y0 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y1 ) );     \
                                                            \
        (X3) = *RK++ ^ AES_FT0( MBEDTLS_BYTE_0( Y3 ) ) ^    \
                       AES_FT1( MBEDTLS_BYTE_1( Y0 ) ) ^    \
                       AES_FT2( MBEDTLS_BYTE_2( Y1 ) ) ^    \
                       AES_FT3( MBEDTLS_BYTE_3( Y2 ) );     \
    } while( 0 )

#define AES_RROUND(X0,X1,X2,X3,Y0,Y1,Y2,Y3)                 \
    do                                                      \
    {                                                       \
        (X0) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y0 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y3 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y2 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y1 ) );     \
                                                            \
        (X1) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y1 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y0 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y3 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y2 ) );     \
                                                            \
        (X2) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y2 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y1 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y0 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y3 ) );     \
                                                            \
        (X3) = *RK++ ^ AES_RT0( MBEDTLS_BYTE_0( Y3 ) ) ^    \
                       AES_RT1( MBEDTLS_BYTE_1( Y2 ) ) ^    \
                       AES_RT2( MBEDTLS_BYTE_2( Y1 ) ) ^    \
                       AES_RT3( MBEDTLS_BYTE_3( Y0 ) );     \
    } while( 0 )

/*
 * AES-ECB block encryption
 */
int mbedtls_internal_aes_encrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    ctx->link_ctx.aes_mode = AES_MODE_ECB;
    memset(&ctx->link_ctx.aes_iv0, 0, 16);

    bflb_sec_aes_mutex_take();
    memcpy(ecb_enc_buf, input, 16);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_aes_link_s));
    bflb_aes_link_update(ctx->aes, (uint32_t)&link_ctx_temp, ecb_enc_buf, ecb_dec_buf, 16);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_aes_link_s));
    memcpy(output, ecb_dec_buf, 16);
    bflb_sec_aes_mutex_give();

    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_aes_encrypt( mbedtls_aes_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16] )
{
    MBEDTLS_IGNORE_RETURN( mbedtls_internal_aes_encrypt( ctx, input, output ) );
}
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

/*
 * AES-ECB block decryption
 */
int mbedtls_internal_aes_decrypt( mbedtls_aes_context *ctx,
                                  const unsigned char input[16],
                                  unsigned char output[16] )
{
    ctx->link_ctx.aes_mode = AES_MODE_ECB;
    memset(&ctx->link_ctx.aes_iv0, 0, 16);

    bflb_sec_aes_mutex_take();
    memcpy(ecb_enc_buf, input, 16);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_aes_link_s));
    bflb_aes_link_update(ctx->aes, (uint32_t)&link_ctx_temp, ecb_enc_buf, ecb_dec_buf, 16);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_aes_link_s));
    memcpy(output, ecb_dec_buf, 16);
    bflb_sec_aes_mutex_give();

    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_aes_decrypt( mbedtls_aes_context *ctx,
                          const unsigned char input[16],
                          unsigned char output[16] )
{
    MBEDTLS_IGNORE_RETURN( mbedtls_internal_aes_decrypt( ctx, input, output ) );
}
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                           int mode,
                           const unsigned char input[16],
                           unsigned char output[16] )
{
    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );

#if defined(MBEDTLS_AESNI_C) && defined(MBEDTLS_HAVE_X86_64)
    if( mbedtls_aesni_has_support( MBEDTLS_AESNI_AES ) )
        return( mbedtls_aesni_crypt_ecb( ctx, mode, input, output ) );
#endif

#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_HAVE_X86)
    if( aes_padlock_ace )
    {
        if( mbedtls_padlock_xcryptecb( ctx, mode, input, output ) == 0 )
            return( 0 );

        // If padlock data misaligned, we just fall back to
        // unaccelerated mode
        //
    }
#endif

    if( mode == MBEDTLS_AES_ENCRYPT )
        return( mbedtls_internal_aes_encrypt( ctx, input, output ) );
    else
        return( mbedtls_internal_aes_decrypt( ctx, input, output ) );
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    if( length % 16 )
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

#if defined(MBEDTLS_PADLOCK_C) && defined(MBEDTLS_HAVE_X86)
    if( aes_padlock_ace )
    {
        if( mbedtls_padlock_xcryptcbc( ctx, mode, length, iv, input, output ) == 0 )
            return( 0 );

        // If padlock data misaligned, we just fall back to
        // unaccelerated mode
        //
    }
#endif
    ctx->link_ctx.aes_mode = AES_MODE_CBC;
    memcpy(&ctx->link_ctx.aes_iv0, iv, 16);

    bflb_l1c_dcache_clean_invalidate_range((void *)input, length);
    bflb_l1c_dcache_clean_invalidate_range((void *)output, length);

    bflb_sec_aes_mutex_take();
    if( mode == MBEDTLS_AES_DECRYPT )
    {
        memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_aes_link_s));
        bflb_aes_link_update(ctx->aes, (uint32_t)&link_ctx_temp, input, output, length);
        memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_aes_link_s));
    }
    else
    {
        memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_aes_link_s));
        bflb_aes_link_update(ctx->aes, (uint32_t)&link_ctx_temp, input, output, length);
        memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_aes_link_s));
    }
    bflb_sec_aes_mutex_give();
    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_XTS)

typedef unsigned char mbedtls_be128[16];

/*
 * GF(2^128) multiplication function
 *
 * This function multiplies a field element by x in the polynomial field
 * representation. It uses 64-bit word operations to gain speed but compensates
 * for machine endianness and hence works correctly on both big and little
 * endian machines.
 */
static void mbedtls_gf128mul_x_ble( unsigned char r[16],
                                    const unsigned char x[16] )
{
    uint64_t a, b, ra, rb;

    a = MBEDTLS_GET_UINT64_LE( x, 0 );
    b = MBEDTLS_GET_UINT64_LE( x, 8 );

    ra = ( a << 1 )  ^ 0x0087 >> ( 8 - ( ( b >> 63 ) << 3 ) );
    rb = ( a >> 63 ) | ( b << 1 );

    MBEDTLS_PUT_UINT64_LE( ra, r, 0 );
    MBEDTLS_PUT_UINT64_LE( rb, r, 8 );
}

/*
 * AES-XTS buffer encryption/decryption
 */
int mbedtls_aes_crypt_xts( mbedtls_aes_xts_context *ctx,
                           int mode,
                           size_t length,
                           const unsigned char data_unit[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t blocks = length / 16;
    size_t leftover = length % 16;
    unsigned char tweak[16];
    unsigned char prev_tweak[16];
    unsigned char tmp[16];

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( data_unit != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    /* Data units must be at least 16 bytes long. */
    if( length < 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* NIST SP 800-38E disallows data units larger than 2**20 blocks. */
    if( length > ( 1 << 20 ) * 16 )
        return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;

    /* Compute the tweak. */
    ret = mbedtls_aes_crypt_ecb( &ctx->tweak, MBEDTLS_AES_ENCRYPT,
                                 data_unit, tweak );
    if( ret != 0 )
        return( ret );

    while( blocks-- )
    {
        size_t i;

        if( leftover && ( mode == MBEDTLS_AES_DECRYPT ) && blocks == 0 )
        {
            /* We are on the last block in a decrypt operation that has
             * leftover bytes, so we need to use the next tweak for this block,
             * and this tweak for the leftover bytes. Save the current tweak for
             * the leftovers and then update the current tweak for use on this,
             * the last full block. */
            memcpy( prev_tweak, tweak, sizeof( tweak ) );
            mbedtls_gf128mul_x_ble( tweak, tweak );
        }

        for( i = 0; i < 16; i++ )
            tmp[i] = input[i] ^ tweak[i];

        ret = mbedtls_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if( ret != 0 )
            return( ret );

        for( i = 0; i < 16; i++ )
            output[i] = tmp[i] ^ tweak[i];

        /* Update the tweak for the next block. */
        mbedtls_gf128mul_x_ble( tweak, tweak );

        output += 16;
        input += 16;
    }

    if( leftover )
    {
        /* If we are on the leftover bytes in a decrypt operation, we need to
         * use the previous tweak for these bytes (as saved in prev_tweak). */
        unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? prev_tweak : tweak;

        /* We are now on the final part of the data unit, which doesn't divide
         * evenly by 16. It's time for ciphertext stealing. */
        size_t i;
        unsigned char *prev_output = output - 16;

        /* Copy ciphertext bytes from the previous block to our output for each
         * byte of ciphertext we won't steal. At the same time, copy the
         * remainder of the input for this final round (since the loop bounds
         * are the same). */
        for( i = 0; i < leftover; i++ )
        {
            output[i] = prev_output[i];
            tmp[i] = input[i] ^ t[i];
        }

        /* Copy ciphertext bytes from the previous block for input in this
         * round. */
        for( ; i < 16; i++ )
            tmp[i] = prev_output[i] ^ t[i];

        ret = mbedtls_aes_crypt_ecb( &ctx->crypt, mode, tmp, tmp );
        if( ret != 0 )
            return ret;

        /* Write the result back to the previous block, overriding the previous
         * output we copied. */
        for( i = 0; i < 16; i++ )
            prev_output[i] = tmp[i] ^ t[i];
    }

    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    int c;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t n;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    if( mode == MBEDTLS_AES_DECRYPT )
    {
        while( length-- )
        {
            if( n == 0 )
            {
                ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
                if( ret != 0 )
                    goto exit;
            }

            c = *input++;
            *output++ = (unsigned char)( c ^ iv[n] );
            iv[n] = (unsigned char) c;

            n = ( n + 1 ) & 0x0F;
        }
    }
    else
    {
        while( length-- )
        {
            if( n == 0 )
            {
                ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
                if( ret != 0 )
                    goto exit;
            }

            iv[n] = *output++ = (unsigned char)( iv[n] ^ *input++ );

            n = ( n + 1 ) & 0x0F;
        }
    }

    *iv_off = n;
    ret = 0;

exit:
    return( ret );
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                            int mode,
                            size_t length,
                            unsigned char iv[16],
                            const unsigned char *input,
                            unsigned char *output )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char c;
    unsigned char ov[17];

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( mode == MBEDTLS_AES_ENCRYPT ||
                      mode == MBEDTLS_AES_DECRYPT );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );
    while( length-- )
    {
        memcpy( ov, iv, 16 );
        ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
        if( ret != 0 )
            goto exit;

        if( mode == MBEDTLS_AES_DECRYPT )
            ov[16] = *input;

        c = *output++ = (unsigned char)( iv[0] ^ *input++ );

        if( mode == MBEDTLS_AES_ENCRYPT )
            ov[16] = c;

        memcpy( iv, ov + 1, 16 );
    }
    ret = 0;

exit:
    return( ret );
}
#endif /* MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb( mbedtls_aes_context *ctx,
                           size_t length,
                           size_t *iv_off,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output )
{
    int ret = 0;
    size_t n;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( iv_off != NULL );
    AES_VALIDATE_RET( iv != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    n = *iv_off;

    if( n > 15 )
        return( MBEDTLS_ERR_AES_BAD_INPUT_DATA );

    while( length-- )
    {
        if( n == 0 )
        {
            ret = mbedtls_aes_crypt_ecb( ctx, MBEDTLS_AES_ENCRYPT, iv, iv );
            if( ret != 0 )
                goto exit;
        }
        *output++ =  *input++ ^ iv[n];

        n = ( n + 1 ) & 0x0F;
    }

    *iv_off = n;

exit:
    return( ret );
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    AES_VALIDATE_RET( ctx != NULL );
    AES_VALIDATE_RET( nc_off != NULL );
    AES_VALIDATE_RET( nonce_counter != NULL );
    AES_VALIDATE_RET( stream_block != NULL );
    AES_VALIDATE_RET( input != NULL );
    AES_VALIDATE_RET( output != NULL );

    if( length % 16 )
        return( MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH );

    ctx->link_ctx.aes_mode = AES_MODE_CTR;
    memcpy(&ctx->link_ctx.aes_iv0, nonce_counter, 16);

    bflb_l1c_dcache_clean_invalidate_range((void *)input, length);
    bflb_l1c_dcache_clean_invalidate_range((void *)output, length);

    bflb_sec_aes_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_aes_link_s));
    bflb_aes_link_update(ctx->aes, (uint32_t)&link_ctx_temp, input, output, length);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_aes_link_s));
    bflb_sec_aes_mutex_give();
    return( 0 );
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#endif /* MBEDTLS_AES_C */
