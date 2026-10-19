/* SPDX-License-Identifier: Apache-2.0 */

#include "utils/includes.h"
#include "utils/common.h"
#include "sha1.h"

#include <mbedtls/md.h>
#include <mbedtls/pkcs5.h>

int pbkdf2_sha1(const char *passphrase, const char *ssid, size_t ssid_len,
                int iterations, uint8_t *buf, size_t buflen)
{
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *info;
    int ret = -1;

    if (iterations <= 0 || buflen > UINT32_MAX) {
        return -1;
    }

    mbedtls_md_init(&ctx);
    info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    if (info != NULL && mbedtls_md_setup(&ctx, info, 1) == 0) {
        ret = mbedtls_pkcs5_pbkdf2_hmac(&ctx,
                (const unsigned char *)passphrase, os_strlen(passphrase),
                (const unsigned char *)ssid, ssid_len, iterations, buflen, buf);
    }
    mbedtls_md_free(&ctx);
    return ret == 0 ? 0 : -1;
}
