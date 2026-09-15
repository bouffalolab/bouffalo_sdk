/*
 *  FIPS-180-2 compliant SHA-256 implementation
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
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include "common.h"

#if defined(MBEDTLS_SHA256_C)

#include "mbedtls/sha256.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#include <string.h>

#include "mbedtls/platform.h"
#include "bflb_sec_mutex.h"

#ifdef BL618DG
#include "bl618dg_glb.h"
#endif

#ifdef CONFIG_MBEDTLS_V2
#define SHA256_VALIDATE_RET(cond)                           \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA )
#define SHA256_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )
#else
#define SHA256_VALIDATE_RET(cond)                         \
    do                                                    \
    {                                                     \
        if( !( cond ) )                                   \
            return( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA );  \
    } while( 0 )
#define SHA256_VALIDATE(cond)                             \
    do                                                    \
    {                                                     \
        if( !( cond ) )                                   \
            return;                                       \
    } while( 0 )
#endif

static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_link_ctx_s link_ctx_temp;

#if defined(BL602)
#define BL602_DTCM_END 0x02020000UL

/* BL602 SHA link mode cannot reliably update its descriptor in DTCM. */
static struct bflb_sha_link_s working_link_cfg
    __attribute__((section(".wifi_ram"), aligned(4)));

static int sha256_link_cfg_is_in_tcm(const struct bflb_sha_link_s *link_cfg)
{
    uintptr_t addr = (uintptr_t)link_cfg & 0x0FFFFFFFUL;

    return addr < BL602_DTCM_END;
}

static void sha256_link_cfg_load(struct bflb_sha256_link_ctx_s *ctx)
{
    if (sha256_link_cfg_is_in_tcm(&ctx->link_cfg)) {
        memcpy(&working_link_cfg, &ctx->link_cfg, sizeof(working_link_cfg));
        ctx->link_addr = (uint32_t)(uintptr_t)&working_link_cfg;
    }
}

static void sha256_link_cfg_store(struct bflb_sha256_link_ctx_s *ctx)
{
    if (ctx->link_addr == (uint32_t)(uintptr_t)&working_link_cfg) {
        memcpy(&ctx->link_cfg, &working_link_cfg, sizeof(working_link_cfg));
    }
}
#else
static void sha256_link_cfg_load(struct bflb_sha256_link_ctx_s *ctx)
{
    (void)ctx;
}

static void sha256_link_cfg_store(struct bflb_sha256_link_ctx_s *ctx)
{
    (void)ctx;
}
#endif

void mbedtls_sha256_init( mbedtls_sha256_context *ctx )
{
    SHA256_VALIDATE( ctx != NULL );

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
    ctx->sha = sha;

    bflb_sec_sha_mutex_take();
#ifdef BL618DG
    if (GLB_Get_Core_Type() == GLB_CORE_ID_NP) {
        bflb_group1_request_sha_access(sha);
    } else
#endif
    {
        bflb_group0_request_sha_access(sha);
    }
    bflb_sha_link_init(sha);
    bflb_sec_sha_mutex_give();
}

void mbedtls_sha256_free( mbedtls_sha256_context *ctx )
{
    if( ctx == NULL )
        return;

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha256_context ) );
}

void mbedtls_sha256_clone( mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src )
{
    SHA256_VALIDATE( dst != NULL );
    SHA256_VALIDATE( src != NULL );

    *dst = *src;
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts_ret( mbedtls_sha256_context *ctx, int is224 )
{
    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( is224 == 0 || is224 == 1 );

    if(ctx->sha == NULL)
    {
        ctx->sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);
    }

    bflb_sec_sha_mutex_take();
    bflb_sha_link_init(ctx->sha);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha256_link_ctx_s));
    bflb_sha256_link_start(ctx->sha, &link_ctx_temp, is224);
    sha256_link_cfg_load(&link_ctx_temp);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_sha256_link_ctx_s));
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#ifdef CONFIG_MBEDTLS_V2
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx,
                            int is224 )
{
    mbedtls_sha256_starts_ret( ctx, is224 );
}
#else
int mbedtls_sha256_starts( mbedtls_sha256_context *ctx,
                            int is224 )
{
    return mbedtls_sha256_starts_ret( ctx, is224 );
}

#endif
#endif

/*
 * SHA-256 process buffer
 */
int mbedtls_sha256_update_ret( mbedtls_sha256_context *ctx,
                               const unsigned char *input,
                               size_t ilen )
{
    int ret;

    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    bflb_l1c_dcache_clean_range((void *)input, ilen);
    bflb_sec_sha_mutex_take();
    bflb_sha_link_init(ctx->sha);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha256_link_ctx_s));
    sha256_link_cfg_load(&link_ctx_temp);
    ret = bflb_sha256_link_update(ctx->sha, &link_ctx_temp, input, ilen);
    sha256_link_cfg_store(&link_ctx_temp);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_sha256_link_ctx_s));
    bflb_sec_sha_mutex_give();

    if( ret != 0 )
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );

    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#ifdef CONFIG_MBEDTLS_V2
void mbedtls_sha256_update( mbedtls_sha256_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    mbedtls_sha256_update_ret( ctx, input, ilen );
}
#else
int mbedtls_sha256_update( mbedtls_sha256_context *ctx,
                            const unsigned char *input,
                            size_t ilen )
{
    return mbedtls_sha256_update_ret( ctx, input, ilen );
}
#endif
#endif

/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish_ret( mbedtls_sha256_context *ctx,
                               unsigned char output[32] )
{
    //int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    SHA256_VALIDATE_RET( ctx != NULL );
    SHA256_VALIDATE_RET( (unsigned char *)output != NULL );

    bflb_sec_sha_mutex_take();
    bflb_sha_link_init(ctx->sha);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha256_link_ctx_s));
    sha256_link_cfg_load(&link_ctx_temp);
    bflb_sha256_link_finish(ctx->sha, &link_ctx_temp, output);
    sha256_link_cfg_store(&link_ctx_temp);
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#ifdef CONFIG_MBEDTLS_V2
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx,
                            unsigned char output[32] )
{
    mbedtls_sha256_finish_ret( ctx, output );
}
#else
int mbedtls_sha256_finish( mbedtls_sha256_context *ctx,
                            unsigned char *output )
{
    return mbedtls_sha256_finish_ret( ctx, output );
}
#endif
#endif

#endif /* MBEDTLS_SHA256_C */
