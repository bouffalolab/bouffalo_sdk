/*
 *  FIPS-180-1 compliant SHA-1 implementation
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
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

#include "common.h"

#if defined(MBEDTLS_SHA1_C)

#include "mbedtls/sha1.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include <string.h>

#include "mbedtls/platform.h"
#include "sec_mutex.h"

#define SHA1_VALIDATE_RET(cond)                             \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA1_BAD_INPUT_DATA )

#define SHA1_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )

static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha1_link_ctx_s link_ctx_temp;

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    SHA1_VALIDATE( ctx != NULL );

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name("sha");

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha1_context ) );
    ctx->sha = sha;

    bflb_group0_request_sha_access(sha);
    bflb_sha_link_init(sha);
}

void mbedtls_sha1_free( mbedtls_sha1_context *ctx )
{
    if( ctx == NULL )
        return;
    
    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha1_context ) );
}

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src )
{
    SHA1_VALIDATE( dst != NULL );
    SHA1_VALIDATE( src != NULL );

    *dst = *src;
}

/*
 * SHA-1 context setup
 */
int mbedtls_sha1_starts_ret( mbedtls_sha1_context *ctx )
{
    SHA1_VALIDATE_RET( ctx != NULL );

    if(ctx->sha == NULL)
    {
        ctx->sha = bflb_device_get_by_name("sha");
    }

    bflb_sec_sha_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sha1_link_start(ctx->sha, &link_ctx_temp);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    mbedtls_sha1_starts_ret( ctx );
}
#endif

/*
 * SHA-1 process buffer
 */
int mbedtls_sha1_update_ret( mbedtls_sha1_context *ctx,
                             const unsigned char *input,
                             size_t ilen )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    SHA1_VALIDATE_RET( ctx != NULL );
    SHA1_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    bflb_l1c_dcache_clean_range((void *)input, ilen);
    bflb_sec_sha_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sha1_link_update(ctx->sha, &link_ctx_temp, input, ilen);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_update( mbedtls_sha1_context *ctx,
                          const unsigned char *input,
                          size_t ilen )
{
    mbedtls_sha1_update_ret( ctx, input, ilen );
}
#endif

/*
 * SHA-1 final digest
 */
int mbedtls_sha1_finish_ret( mbedtls_sha1_context *ctx,
                             unsigned char output[20] )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    SHA1_VALIDATE_RET( ctx != NULL );
    SHA1_VALIDATE_RET( (unsigned char *)output != NULL );

    bflb_sec_sha_mutex_take();
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sha1_link_finish(ctx->sha, &link_ctx_temp, output);
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
void mbedtls_sha1_finish( mbedtls_sha1_context *ctx,
                          unsigned char output[20] )
{
    mbedtls_sha1_finish_ret( ctx, output );
}
#endif

#endif /* MBEDTLS_SHA1_C */
