#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bl_crt_bundle.h"

/* mbedTLS 2 exposes these members directly; 3 uses MBEDTLS_PRIVATE(). */
#ifndef MBEDTLS_PRIVATE
#define MBEDTLS_PRIVATE(member) member
#endif

#define DBG_TAG "BL_CERT"
#include "log.h"

#define BUNDLE_HEADER_OFFSET 2
#define CRT_HEADER_OFFSET 4

#ifndef CONFIG_CRT_BUNDLE_MAX_CERTS
#define CONFIG_CRT_BUNDLE_MAX_CERTS 200
#endif

static mbedtls_x509_crt s_dummy_crt;
static unsigned int s_bundle_references;

typedef struct crt_bundle_t {
    const uint8_t **crts;
    uint16_t num_certs;
    size_t x509_crt_bundle_len;
} crt_bundle_t;

static crt_bundle_t s_crt_bundle;

static int bl_crt_check_signature(mbedtls_x509_crt *child, const uint8_t *pub_key_buf, size_t pub_key_len);


static int bl_crt_check_signature(mbedtls_x509_crt *child, const uint8_t *pub_key_buf, size_t pub_key_len)
{
    int ret = 0;
    mbedtls_x509_crt parent;
    const mbedtls_md_info_t *md_info;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];

    mbedtls_x509_crt_init(&parent);

    if ( (ret = mbedtls_pk_parse_public_key(&parent.pk, pub_key_buf, pub_key_len) ) != 0) {
        LOG_E("Failed to parse public key, error: 0x%X", ret);
        goto cleanup;
    }

    if (!mbedtls_pk_can_do(&parent.pk, child->MBEDTLS_PRIVATE(sig_pk))) {
        LOG_E("Key type mismatch");
        ret = -1;
        goto cleanup;
    }

    md_info = mbedtls_md_info_from_type(child->MBEDTLS_PRIVATE(sig_md));
    if ( (ret = mbedtls_md( md_info, child->tbs.p, child->tbs.len, hash )) != 0 ) {
        LOG_E("Hash computation failed: 0x%X", ret);
        goto cleanup;
    }

    if ( (ret = mbedtls_pk_verify_ext( child->MBEDTLS_PRIVATE(sig_pk), child->MBEDTLS_PRIVATE(sig_opts), &parent.pk,
                                       child->MBEDTLS_PRIVATE(sig_md), hash, mbedtls_md_get_size( md_info ),
                                       child->MBEDTLS_PRIVATE(sig).p, child->MBEDTLS_PRIVATE(sig).len )) != 0 ) {

        LOG_E("Signature verification error: 0x%X", ret);
        goto cleanup;
    }
cleanup:
    mbedtls_x509_crt_free(&parent);

    return ret;
}

static int bl_crt_verify_one(mbedtls_x509_crt *child)
{
    if (child == NULL || child->issuer_raw.p == NULL || child->issuer_raw.len == 0) {
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    for (uint16_t i = 0; i < s_crt_bundle.num_certs; i++) {
        size_t name_len = ((size_t)s_crt_bundle.crts[i][0] << 8) | s_crt_bundle.crts[i][1];
        const uint8_t *crt_name = s_crt_bundle.crts[i] + CRT_HEADER_OFFSET;

        /* Names are sorted, but their lengths differ. A prefix compare sends
         * binary search to the wrong side, so match the full issuer name. */
        if (name_len != child->issuer_raw.len ||
            memcmp(child->issuer_raw.p, crt_name, name_len) != 0) {
            continue;
        }
        size_t key_len = ((size_t)s_crt_bundle.crts[i][2] << 8) | s_crt_bundle.crts[i][3];
        return bl_crt_check_signature(child, crt_name + name_len, key_len);
    }
    return MBEDTLS_ERR_X509_FATAL_ERROR;
}

int bl_crt_verify_callback(void *buf, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    (void)buf;

    uint32_t flags_filtered = *flags & ~(MBEDTLS_X509_BADCERT_BAD_MD);

    if (flags_filtered != MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
        return 0;
    }


    if (s_crt_bundle.crts == NULL) {
        LOG_E("Certificate bundle empty");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    LOG_D("Bundle contains %u certs", s_crt_bundle.num_certs);

    /* The peer chain may be leaf -> intermediate -> root. The bundle holds
     * root subjects, so keep walking until a bundled root signs a cert. */
    for (mbedtls_x509_crt *child = crt; child != NULL; child = child->next) {
        if (bl_crt_verify_one(child) == 0) {
            LOG_I("Cert verification passed");
            *flags = 0;
            return 0;
        }
    }

    LOG_E("Cert verification failed depth=%d flags=0x%x issuer_len=%u",
          depth, (unsigned)*flags,
          (crt != NULL && crt->issuer_raw.p != NULL) ? (unsigned)crt->issuer_raw.len : 0);
    return MBEDTLS_ERR_X509_FATAL_ERROR;
}


static int bl_crt_bundle_init(const uint8_t *x509_bundle, size_t bundle_size)
{
    if (bundle_size < BUNDLE_HEADER_OFFSET + CRT_HEADER_OFFSET) {
        LOG_E("Bundle size too small");
        return -1;
    }

    uint16_t num_certs = (x509_bundle[0] << 8) | x509_bundle[1];
    if (num_certs > CONFIG_CRT_BUNDLE_MAX_CERTS) {
        LOG_E("Cert count %u exceeds maximum %u",
                      num_certs, CONFIG_CRT_BUNDLE_MAX_CERTS);
        return -1;
    }

    const uint8_t **crts = calloc(num_certs, sizeof(x509_bundle));
    if (crts == NULL) {
        LOG_E("Memory alloc failed");
        return -2;
    }

    const uint8_t *cur_crt;
    const uint8_t *bundle_end = x509_bundle + bundle_size;
    cur_crt = x509_bundle + BUNDLE_HEADER_OFFSET;

    for (int i = 0; i < num_certs; i++) {
        crts[i] = cur_crt;
        if (cur_crt + CRT_HEADER_OFFSET > bundle_end) {
            LOG_E("Bundle data corrupted");
            free(crts);
            return -1;
        }
        size_t name_len = cur_crt[0] << 8 | cur_crt[1];
        size_t key_len = cur_crt[2] << 8 | cur_crt[3];
        cur_crt = cur_crt + CRT_HEADER_OFFSET + name_len + key_len;
    }

    if (cur_crt > bundle_end) {
        LOG_E("Bundle data overflow");
        free(crts);
        return -1;
    }

    free(s_crt_bundle.crts);
    s_crt_bundle.num_certs = num_certs;
    s_crt_bundle.crts = crts;
    return 0;
}

int bl_crt_bundle_attach(void *conf)
{
    int ret = 0;
    if (s_crt_bundle.crts == NULL) {
        const uint8_t *bundle_start;
        const uint8_t *bundle_end;

        if (bl_crt_bundle_get_addr(&bundle_start, &bundle_end) == 0) {
            LOG_I("Bundle loaded: addr=%p, len=%zu",
                  bundle_start, bundle_end - bundle_start);
            ret = bl_crt_bundle_init(bundle_start, bundle_end - bundle_start);
        } else {
            LOG_E("Bundle not found in binary image");
            ret = -1;
        }
    }

    if (ret != 0) {
        LOG_E("Bundle attachment error");
        return ret;
    }

    if (conf) {
        mbedtls_ssl_config *ssl_conf = (mbedtls_ssl_config *)conf;
        mbedtls_x509_crt_init(&s_dummy_crt);
        mbedtls_ssl_conf_ca_chain(ssl_conf, &s_dummy_crt, NULL);
        mbedtls_ssl_conf_verify(ssl_conf, bl_crt_verify_callback, NULL);
        s_bundle_references++;
    }

    return ret;
}

void bl_crt_bundle_detach(mbedtls_ssl_config *conf)
{
    if (conf) {
        mbedtls_ssl_conf_ca_chain(conf, NULL, NULL);
        mbedtls_ssl_conf_verify(conf, NULL, NULL);
        if (s_bundle_references > 0) {
            s_bundle_references--;
        }
    }
    if (s_bundle_references == 0) {
        free(s_crt_bundle.crts);
        s_crt_bundle.crts = NULL;
        s_crt_bundle.num_certs = 0;
    }
}

int bl_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size)
{
    if (s_bundle_references != 0) {
        LOG_E("Cannot replace certificate bundle while SSL configs use it");
        return -1;
    }
    return bl_crt_bundle_init(x509_bundle, bundle_size);
}

int bl_crt_bundle_is_available(void)
{
    const uint8_t *bundle_start;
    const uint8_t *bundle_end;

    return (bl_crt_bundle_get_addr(&bundle_start, &bundle_end) == 0) ? 1 : 0;
}
