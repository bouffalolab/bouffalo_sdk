/*
 * SHA1-based key derivation function (PBKDF2) for IEEE 802.11i
 * Copyright (c) 2003-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "sha1.h"

#include <mbedtls/platform.h>

#ifdef CONFIG_MBEDTLS_V3
#include <mbedtls/compat-2.x.h>
#endif

#if defined(MBEDTLS_SHA1_C)
#include <string.h>
#include <mbedtls/platform_util.h>
#include <mbedtls/error.h>
#include <mbedtls/sha1.h>

/*
 * HMAC(K, M) = H( ( K' ^ opad ) || H( ( K' ^ ipad ) || M ) )
 */
static int mbedtls_hmac_iter_init( const unsigned char *key, size_t key_len,
                            unsigned char *buf, size_t buf_len)
{
    int i, ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    uint32_t *pw, *pw2;

    memset(buf, 0, buf_len);
    uint8_t *buf1 = (uint8_t *)(( (uint32_t)buf + 31 ) & ~( (uint32_t)31 ));
    if( buf_len < buf1 - buf + 256 )
        goto exit;

    if(key_len > 64)
        goto exit;

    uint8_t *buf2 = buf1 + 128;

    memcpy(buf1, key, key_len);
    memcpy(buf2, key, key_len);
    pw = (uint32_t *)buf1;
    pw2 = (uint32_t *)buf2;
    for (i = 0; i < 16; ++i) {
        *pw++ ^= 0x36363636;
        *pw2++ ^= 0x5c5c5c5c;
    }

#if defined(MBEDTLS_SHA1_ALT)
    mbedtls_sha1_once_padded_init();
#endif

exit:

    return( ret );
}

static void pad_buf(uint8_t *buf, size_t input_len)
{
    uint8_t *p = buf + input_len;
    *p++ = 0x80;
    size_t pad_len = 128 - input_len - 1 - 4;
    memset(p, 0, pad_len);
    p += pad_len;
    size_t input_len_bits = input_len << 3;
    {
        p[0] = (uint8_t)(input_len_bits >> 24);
        p[1] = (uint8_t)(input_len_bits >> 16);
        p[2] = (uint8_t)(input_len_bits >> 8);
        p[3] = (uint8_t)(input_len_bits);
    }
}

static int mbedtls_hmac_iter_ret( unsigned char *buf, size_t buf_len,
                           unsigned char *msg, size_t msg_len,
                           unsigned char output[20], int padded)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    uint8_t *buf1 = (uint8_t *)(( (uint32_t)buf + 31 ) & ~( (uint32_t)31 ));
    uint8_t *buf2 = buf1 + 128;

    if( buf_len < buf1 - buf + 256 )
        goto exit;

    if(msg_len > 64 - 1 - 4)
        goto exit;
    else
        memcpy(buf1 + 64, msg, msg_len);

    if(!padded) {
        pad_buf(buf1, 64 + msg_len);
        pad_buf(buf2, 64 + 20);
    }

#if !defined(MBEDTLS_SHA1_ALT)

    mbedtls_sha1_context ctx;

    mbedtls_sha1_init( &ctx );

    if( ( ret = mbedtls_sha1_starts_ret( &ctx ) ) != 0 )
        goto exit;

    if( ( ret = mbedtls_sha1_update_ret( &ctx, buf1, 64 + msg_len ) ) != 0 )
        goto exit;

    if( ( ret = mbedtls_sha1_finish_ret( &ctx, buf2 + 64 ) ) != 0 )
        goto exit;

    mbedtls_sha1_init( &ctx );

    if( ( ret = mbedtls_sha1_starts_ret( &ctx ) ) != 0 )
        goto exit;

    if( ( ret = mbedtls_sha1_update_ret( &ctx, buf2, 64 + 20 ) ) != 0 )
        goto exit;

    if( ( ret = mbedtls_sha1_finish_ret( &ctx, output ) ) != 0 )
        goto exit;
#else

    if( ( ret = mbedtls_sha1_once_padded( buf1, buf2 + 64, 2) ) != 0 )
        goto exit;

    if( ( ret = mbedtls_sha1_once_padded( buf2, output, 2 ) ) != 0 )
        goto exit;
#endif

exit:

    return( ret );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_hmac( const unsigned char *key, size_t key_len,
                   unsigned char *msg, size_t msg_len,
                   unsigned char output[20] )
{
    uint32_t buf[(256 + 28) / 4];

    mbedtls_hmac_iter_init( key, key_len, (unsigned char *)buf, sizeof(buf) );
    mbedtls_hmac_iter_ret( (unsigned char *)buf, sizeof(buf), msg, msg_len, output, 0 );
}
#endif

#endif /* MBEDTLS_HMAC_C */

#if defined(MBEDTLS_SHA1_C)
static int pbkdf2_sha1_f(const char *passphrase, const u8 *ssid,
             size_t ssid_len, int iterations, unsigned int count,
             u8 *digest)
{
    uint32_t buf[(256 + 28) / 4]; // 28 is for padding 32 bytes align, 256 is inner and outer of sha1 2 blocks
    uint8_t *p, msg[32 + 4], output[SHA1_MAC_LEN]; // 32 is max ssid length, 4 is count
    int i, j;

    mbedtls_hmac_iter_init((const unsigned char *)passphrase, os_strlen(passphrase), (unsigned char *)buf, sizeof(buf));

    memcpy(msg, ssid, ssid_len);
    p = msg + ssid_len;
    p[0] = (count >> 24) & 0xff;
    p[1] = (count >> 16) & 0xff;
    p[2] = (count >> 8) & 0xff;
    p[3] = count & 0xff;
    mbedtls_hmac_iter_ret((unsigned char *)buf, sizeof(buf), msg, ssid_len + 4, output, 0);

    os_memcpy(digest, output, SHA1_MAC_LEN);

    int padded = 0;
    for (i = 1; i < iterations; i++) {
        mbedtls_hmac_iter_ret((unsigned char *)buf, sizeof(buf), output, SHA1_MAC_LEN, output, padded);
        padded = 1; // after first iteration, buf already padded.

        for (j = 0; j < SHA1_MAC_LEN; j++)
            digest[j] ^= output[j];
    }
    return 0;
}
#else
static int pbkdf2_sha1_f(const char *passphrase, const u8 *ssid,
        size_t ssid_len, int iterations, unsigned int count,
        u8 *digest)
{
    unsigned char tmp[SHA1_MAC_LEN], tmp2[SHA1_MAC_LEN];
    int i, j;
    unsigned char count_buf[4];
    const u8 *addr[2];
    size_t len[2];
    size_t passphrase_len = os_strlen(passphrase);

    addr[0] = ssid;
    len[0] = ssid_len;
    addr[1] = count_buf;
    len[1] = 4;

    /* F(P, S, c, i) = U1 xor U2 xor ... Uc
    * U1 = PRF(P, S || i)
    * U2 = PRF(P, U1)
    * Uc = PRF(P, Uc-1)
    */

    count_buf[0] = (count >> 24) & 0xff;
    count_buf[1] = (count >> 16) & 0xff;
    count_buf[2] = (count >> 8) & 0xff;
    count_buf[3] = count & 0xff;
    if (hmac_sha1_vector((u8 *) passphrase, passphrase_len, 2, addr, len, tmp))
        return -1;
    os_memcpy(digest, tmp, SHA1_MAC_LEN);

    for (i = 1; i < iterations; i++) {
        if (hmac_sha1((u8 *) passphrase, passphrase_len, tmp, SHA1_MAC_LEN, tmp2))
            return -1;
        os_memcpy(tmp, tmp2, SHA1_MAC_LEN);
        for (j = 0; j < SHA1_MAC_LEN; j++)
            digest[j] ^= tmp2[j];
    }

    return 0;
}
#endif


/**
 * pbkdf2_sha1 - SHA1-based key derivation function (PBKDF2) for IEEE 802.11i
 * @passphrase: ASCII passphrase
 * @ssid: SSID
 * @ssid_len: SSID length in bytes
 * @iterations: Number of iterations to run
 * @buf: Buffer for the generated key
 * @buflen: Length of the buffer in bytes
 * Returns: 0 on success, -1 of failure
 *
 * This function is used to derive PSK for WPA-PSK. For this protocol,
 * iterations is set to 4096 and buflen to 32. This function is described in
 * IEEE Std 802.11-2004, Clause H.4. The main construction is from PKCS#5 v2.0.
 */
int pbkdf2_sha1(const char *passphrase, const u8 *ssid, size_t ssid_len,
                int iterations, u8 *buf, size_t buflen)
{
    unsigned int count = 0;
    unsigned char *pos = buf;
    size_t left = buflen, plen;
    unsigned char digest[SHA1_MAC_LEN];

    while (left > 0) {
        count++;
        if (pbkdf2_sha1_f(passphrase, ssid, ssid_len, iterations, count, digest))
            return -1;
        plen = left > SHA1_MAC_LEN ? SHA1_MAC_LEN : left;
        os_memcpy(pos, digest, plen);
        pos += plen;
        left -= plen;
    }
    return 0;
}
