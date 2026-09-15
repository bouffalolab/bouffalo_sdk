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
#include "bflb_sec_mutex.h"

#ifdef BL618DG
#include "bl618dg_glb.h"
#endif

#ifdef CONFIG_MBEDTLS_V2
#define SHA1_VALIDATE_RET(cond)                             \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_SHA1_BAD_INPUT_DATA )

#define SHA1_VALIDATE(cond)  MBEDTLS_INTERNAL_VALIDATE( cond )
#else
#define SHA1_VALIDATE_RET(cond)
#define SHA1_VALIDATE(cond)
#endif

static ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha1_link_ctx_s link_ctx_temp;

#if defined(BL602)
#define BL602_DTCM_END 0x02020000UL

/* BL602 SHA link mode cannot reliably update its descriptor in DTCM. */
static struct bflb_sha_link_s working_link_cfg
    __attribute__((section(".wifi_ram"), aligned(4)));

static int sha1_link_cfg_is_in_tcm(const struct bflb_sha_link_s *link_cfg)
{
    uintptr_t addr = (uintptr_t)link_cfg & 0x0FFFFFFFUL;

    return addr < BL602_DTCM_END;
}

static void sha1_link_cfg_load(struct bflb_sha1_link_ctx_s *ctx)
{
    if (sha1_link_cfg_is_in_tcm(&ctx->link_cfg)) {
        memcpy(&working_link_cfg, &ctx->link_cfg, sizeof(working_link_cfg));
        ctx->link_addr = (uint32_t)(uintptr_t)&working_link_cfg;
    }
}

static void sha1_link_cfg_store(struct bflb_sha1_link_ctx_s *ctx)
{
    if (ctx->link_addr == (uint32_t)(uintptr_t)&working_link_cfg) {
        memcpy(&ctx->link_cfg, &working_link_cfg, sizeof(working_link_cfg));
    }
}
#else
static void sha1_link_cfg_load(struct bflb_sha1_link_ctx_s *ctx)
{
    (void)ctx;
}

static void sha1_link_cfg_store(struct bflb_sha1_link_ctx_s *ctx)
{
    (void)ctx;
}
#endif

void mbedtls_sha1_init( mbedtls_sha1_context *ctx )
{
    SHA1_VALIDATE( ctx != NULL );

    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    mbedtls_platform_zeroize( ctx, sizeof( mbedtls_sha1_context ) );
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
        ctx->sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);
    }

    bflb_sec_sha_mutex_take();
    bflb_sha_link_init(ctx->sha);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sha1_link_start(ctx->sha, &link_ctx_temp);
    sha1_link_cfg_load(&link_ctx_temp);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#ifdef CONFIG_MBEDTLS_V2
void mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    mbedtls_sha1_starts_ret( ctx );
}
#else
int mbedtls_sha1_starts( mbedtls_sha1_context *ctx )
{
    return mbedtls_sha1_starts_ret( ctx );
}
#endif
#endif

/*
 * SHA-1 process buffer
 */
int mbedtls_sha1_update_ret( mbedtls_sha1_context *ctx,
                             const unsigned char *input,
                             size_t ilen )
{
    int ret;
    SHA1_VALIDATE_RET( ctx != NULL );
    SHA1_VALIDATE_RET( ilen == 0 || input != NULL );

    if( ilen == 0 )
        return( 0 );

    bflb_l1c_dcache_clean_range((void *)input, ilen);
    bflb_sec_sha_mutex_take();
    bflb_sha_link_init(ctx->sha);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha1_link_ctx_s));
    sha1_link_cfg_load(&link_ctx_temp);
    ret = bflb_sha1_link_update(ctx->sha, &link_ctx_temp, input, ilen);
    sha1_link_cfg_store(&link_ctx_temp);
    memcpy(&ctx->link_ctx, &link_ctx_temp, sizeof(struct bflb_sha1_link_ctx_s));
    bflb_sec_sha_mutex_give();

    if( ret != 0 )
        return( MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED );

    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#ifdef CONFIG_MBEDTLS_V2
void mbedtls_sha1_update( mbedtls_sha1_context *ctx,
                          const unsigned char *input,
                          size_t ilen )
{
    mbedtls_sha1_update_ret( ctx, input, ilen );
}
#else
int mbedtls_sha1_update( mbedtls_sha1_context *ctx,
                          const unsigned char *input,
                          size_t ilen )
{
    return mbedtls_sha1_update_ret( ctx, input, ilen );
}
#endif
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
    bflb_sha_link_init(ctx->sha);
    memcpy(&link_ctx_temp, &ctx->link_ctx, sizeof(struct bflb_sha1_link_ctx_s));
    sha1_link_cfg_load(&link_ctx_temp);
    bflb_sha1_link_finish(ctx->sha, &link_ctx_temp, output);
    sha1_link_cfg_store(&link_ctx_temp);
    bflb_sec_sha_mutex_give();
    return( 0 );
}

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#ifdef CONFIG_MBEDTLS_V2
void mbedtls_sha1_finish( mbedtls_sha1_context *ctx,
                          unsigned char output[20] )
{
    mbedtls_sha1_finish_ret( ctx, output );
}
#else
int mbedtls_sha1_finish( mbedtls_sha1_context *ctx,
                          unsigned char output[20] )
{
    return mbedtls_sha1_finish_ret( ctx, output );
}
#endif
#endif

void mbedtls_sha1_once_padded_init( void )
{
    struct bflb_device_s *sha;

    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_sec_sha_mutex_take();
#ifdef BL618DG
    if (GLB_Get_Core_Type() == GLB_CORE_ID_NP) {
        bflb_group1_request_sha_access(sha);
    } else
#endif
    {
        bflb_group0_request_sha_access(sha);
    }
    bflb_sec_sha_mutex_give();
}

int mbedtls_sha1_once_padded(const unsigned char *input, unsigned char *output, unsigned char nblock)
{
    int ret;
    struct bflb_device_s *sha;
    const uint8_t *input_nc;

    SHA1_VALIDATE_RET( input != NULL );
    SHA1_VALIDATE_RET( output != NULL );

    if (nblock == 0) {
        return 0;
    }

    /* SEC SHA one-shot path requires aligned input for direct memory fetch. */
    if (!bflb_check_cache_addr_aligned((uintptr_t)input)) {
        return MBEDTLS_ERR_SHA1_BAD_INPUT_DATA;
    }

    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_l1c_dcache_clean_range((void *)input, nblock * 64);
    input_nc = (const uint8_t *)bflb_get_no_cache_addr(input);
    bflb_sec_sha_mutex_take();
    bflb_sha_link_deinit(sha);
    bflb_sha_init(sha, SHA_MODE_SHA1);
    ret = bflb_sha1_once_padded(sha, (uint8_t *)input_nc, output, nblock);
    bflb_sec_sha_mutex_give();

    return( ret );
}

#endif /* MBEDTLS_SHA1_C */
