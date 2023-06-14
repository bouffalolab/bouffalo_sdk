/*
 * SM2 Encryption alogrithm
 *
 * References:
 * - GM/T 0003-2012 Chinese National Standard:
 *      Public Key Cryptographic Algorithm SM2 Based on Elliptic Curves
 * - GM/T 0009-2012 SM2 cryptography algorithm application specification
 * - GM/T 0015-2012 Digital certificate format based on SM2 algorithm
 *
 * Thanks to MbedTLS.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SM2_C)

#include "mbedtls/sm2.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_calloc      calloc
#define mbedtls_printf      printf
#define mbedtls_free        free
#endif /* MBEDTLS_PLATFORM_C */

#if !defined(MBEDTLS_SM2_ALT)

#if !defined(MBEDTLS_SM2_CRYPT_ALT) || !defined(MBEDTLS_SM2_SIGN_ALT)
/**
 * Get random r in [1, n-1]
 */
static int sm2_get_rand(mbedtls_sm2_context *ctx, mbedtls_mpi *r,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    size_t blind_tries = 0;
    size_t nlen;
    do {
        nlen = (ctx->grp.nbits + 7) / 8;
        MBEDTLS_MPI_CHK(mbedtls_mpi_fill_random(r, nlen, f_rng, p_rng));
        MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(r, 8 * nlen - ctx->grp.nbits));

        /* See mbedtls_ecp_gen_keypair() */
        if (++blind_tries > 30)
            return (MBEDTLS_ERR_SM2_RANDOM_FAILED);
    } while (mbedtls_mpi_cmp_int(r, 1) < 0 ||
            mbedtls_mpi_cmp_mpi(r, &ctx->grp.N) >= 0);
cleanup:
    return (ret);
}
#endif /* !MBEDTLS_SM2_CRYPT_ALT || !MBEDTLS_SM2_SIGN_ALT */

#if !defined(MBEDTLS_SM2_CRYPT_ALT)
/**
 * SM2 KDF (ISO/IEC 15946-2 3.1.3)
 * (GM/T 0003-2012 - Part 3: Key Exchange Protocol 5.4.3)
 */
static int mbedtls_sm2_pbkdf2(mbedtls_md_context_t *ctx,
        const unsigned char *password, size_t plen,
        const unsigned char *salt, size_t slen,
        unsigned int iteration_count,
        uint32_t key_length, unsigned char *output)
{
    int ret, j;
    unsigned int i;
    unsigned char md1[MBEDTLS_MD_MAX_SIZE];
    unsigned char work[MBEDTLS_MD_MAX_SIZE];
    unsigned char md_size = mbedtls_md_get_size(ctx->md_info);
    size_t use_len;
    unsigned char *out_p = output;
    unsigned char counter[4];

    memset(counter, 0, 4);
    counter[3] = 1;

    if (iteration_count > 0xFFFFFFFF)
        return (MBEDTLS_ERR_SM2_BAD_INPUT_DATA);

    while (key_length) {
        /* U1 ends up in work */
        if ((ret = mbedtls_md_starts(ctx)) != 0)
            return ret;
        if ((ret = mbedtls_md_update(ctx, password, plen)) != 0)
            return ret;
        if ((ret = mbedtls_md_update(ctx, salt, slen)) != 0)
            return ret;
        if ((ret = mbedtls_md_update(ctx, counter, 4)) != 0)
            return ret;
        if ((ret = mbedtls_md_finish(ctx, work)) != 0)
            return ret;

        memcpy(md1, work, md_size);

        for (i = 1; i < iteration_count; i++) {
            /* U2 ends up in md1 */
            if ((ret = mbedtls_md_hmac_starts(ctx, password, plen)) != 0)
                return (ret);
            if ((ret = mbedtls_md_hmac_update(ctx, md1, md_size)) != 0)
                return (ret);
            if ((ret = mbedtls_md_hmac_finish(ctx, md1)) != 0)
                return (ret);

            /* U1 xor U2 */
            for (j = 0; j < md_size; j++)
                work[j] ^= md1[j];
        }

        use_len = (key_length < md_size) ? key_length : md_size;
        memcpy(out_p, work, use_len);

        key_length -= (uint32_t)use_len;
        out_p += use_len;

        for (i = 4; i > 0; i--)
            if (++counter[i - 1] != 0)
                break;
    }

    return (0);
}

int mbedtls_sm2_encrypt(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *input, size_t ilen,
        unsigned char *output, size_t *olen,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = 0;
    size_t i;
    mbedtls_mpi k;
    mbedtls_mpi h;
    mbedtls_ecp_point point;
    mbedtls_md_context_t md_ctx;
    size_t xlen, ylen;
    unsigned char *xym = NULL;

    mbedtls_mpi_init(&k);
    mbedtls_mpi_init(&h);
    mbedtls_md_init(&md_ctx);
    mbedtls_ecp_point_init(&point);
    MBEDTLS_MPI_CHK(mbedtls_md_setup(&md_ctx,
                mbedtls_md_info_from_type(md_alg), 0));

    do {
        /* A1: rand k in [1, n-1] */
        MBEDTLS_MPI_CHK(sm2_get_rand(ctx, &k, f_rng, p_rng));

        /* A2: C1 = [k]G = (x1, y1) */
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&ctx->grp, &point, &k, &ctx->grp.G,
                    NULL, NULL));
        output[0] = MBEDTLS_ECP_POINT_CONVERSION_UNCOMPRESSED;
        *olen = 1;
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.X, output + *olen,
                    mbedtls_mpi_size(&point.X)));
        *olen += mbedtls_mpi_size(&point.X);
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.Y, output + *olen,
                    mbedtls_mpi_size(&point.Y)));
        *olen += mbedtls_mpi_size(&point.Y);

        /* A3: check [h]P != O */
        MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&h, 1));
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&ctx->grp, &point, &h, &ctx->Q,
                    NULL, NULL));
        MBEDTLS_MPI_CHK(mbedtls_ecp_is_zero(&point));

        /* A4: [k]P = (x2, y2) */
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&ctx->grp, &point, &k, &ctx->Q,
                    NULL, NULL));

        /* A5: t = KDF(x2 || y2, klen) */
        xlen = mbedtls_mpi_size(&point.X);
        ylen = mbedtls_mpi_size(&point.Y);
        if ((xym = mbedtls_calloc(1, xlen + ylen + ilen)) == NULL) {
            MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_ALLOC_FAILED);
        }
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.X, xym, xlen));
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.Y, xym + xlen, ylen));
        MBEDTLS_MPI_CHK(mbedtls_sm2_pbkdf2(&md_ctx, xym, xlen + ylen,
                    NULL, 0, 0, ilen, xym + xlen + ylen));
        for (i = 0; i < ilen; i++) {
            if (*(xym + xlen + ylen + i)) {
                break;
            }
        }
        if (i >= xlen + ylen) {
            continue;
        }

        break;
    } while (0);

    /* A6: C2 = M xor t */
    for (i = 0; i < ilen; i++) {
        output[*olen + i] = input[i] ^ *(xym + xlen + ylen + i);
    }
    *olen += ilen;

    /* A7: C3 = Hash(x2 || M || y2) */
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.X, xym, xlen));
    memmove(xym + xlen, input, ilen);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.Y, xym + xlen + ilen, ylen));
    MBEDTLS_MPI_CHK(mbedtls_md(md_ctx.md_info, xym, xlen + ilen + ylen,
                output + *olen));
    *olen += mbedtls_md_get_size(md_ctx.md_info);

cleanup:
    mbedtls_mpi_free(&k);
    mbedtls_mpi_free(&h);
    mbedtls_ecp_point_free(&point);
    if (xym) {
        mbedtls_free(xym);
    }
    mbedtls_md_free(&md_ctx);

    return (ret);
}

int mbedtls_sm2_decrypt(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *input, size_t ilen,
        unsigned char *output, size_t *olen)
{
    int ret = 0;
    size_t i;
    mbedtls_mpi h;
    mbedtls_ecp_point C1;
    mbedtls_ecp_point point;
    mbedtls_md_context_t md_ctx;
    size_t c1len, ptlen, mdlen, xlen, ylen;
    unsigned char *xym = NULL;

    mbedtls_ecp_point_init(&C1);
    mbedtls_ecp_point_init(&point);
    mbedtls_mpi_init(&h);
    mbedtls_md_init(&md_ctx);
    MBEDTLS_MPI_CHK(mbedtls_md_setup(&md_ctx,
                mbedtls_md_info_from_type(md_alg), 0));

    /* B1: get C1 */
    c1len = 1 + (ctx->grp.nbits + 7) / 8 * 2;
    MBEDTLS_MPI_CHK(mbedtls_ecp_point_read_binary(&ctx->grp, &C1,
                input, c1len));

    /* B2: check [h]C1 != O */
    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&h, 1));
    MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&ctx->grp, &point, &h, &C1, NULL, NULL));
    MBEDTLS_MPI_CHK(mbedtls_ecp_is_zero(&point));

    /* B3: [d]C1 = (x2, y2) */
    MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&ctx->grp, &point, &ctx->d, &C1,
                NULL, NULL));

    /* B4: t = KDF(x2 || y2, klen) */
    xlen = mbedtls_mpi_size(&point.X);
    ylen = mbedtls_mpi_size(&point.Y);
    mdlen = mbedtls_md_get_size(md_ctx.md_info);
    ptlen = ilen - c1len - mdlen;
    if ((xym = mbedtls_calloc(1, xlen + ylen + ptlen)) == NULL) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_ALLOC_FAILED);
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.X, xym, xlen));
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.Y, xym + xlen, ylen));
    MBEDTLS_MPI_CHK(mbedtls_sm2_pbkdf2(&md_ctx, xym, xlen + ylen,
                NULL, 0, 0, ptlen, xym + xlen + ylen));
    for (i = 0; i < ptlen; i++) {
        if (*(xym + xlen + ylen + i)) {
            break;
        }
    }
    if (i >= xlen + ylen) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_KDF_FAILED);
    }

    /* B5: M' = C2 xor t */
    for (i = 0; i < ptlen; i++) {
        output[i] = input[c1len + i] ^ *(xym + xlen + ylen + i);
    }
    *olen = ptlen;

    /* B6: check Hash(x2 || M' || y2) == C3 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.X, xym, xlen));
    memmove(xym + xlen, output, *olen);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&point.Y,
                xym + xlen + *olen, ylen));
    MBEDTLS_MPI_CHK(mbedtls_md(md_ctx.md_info, xym, xlen + *olen + ylen, xym));
    if (memcmp(input + c1len + ptlen, xym, mdlen)) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_DECRYPT_BAD_HASH);
    }

cleanup:
    mbedtls_mpi_free(&h);
    mbedtls_ecp_point_free(&point);
    mbedtls_ecp_point_free(&C1);
    if (xym) {
        mbedtls_free(xym);
    }
    mbedtls_md_free(&md_ctx);

    return (ret);
}
#endif /* !MBEDTLS_SM2_CRYPT_ALT */

#if !defined(MBEDTLS_SM2_SIGN_ALT)
int mbedtls_sm2_sign(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *hash, unsigned char *sig,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret = 0;
    mbedtls_mpi k;
    mbedtls_mpi e;
    mbedtls_mpi r;
    mbedtls_mpi s;
    mbedtls_ecp_point point;

    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&k);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_ecp_point_init(&point);

    /**
     * A1: M' = Z || M
     * A2: e = Hash(M')
     * Parameter <hash> is the digest of <M'>, need convert to bignum <e>.
     */
    do {
        /* A3: rand k in [1, n-1] */
        MBEDTLS_MPI_CHK(sm2_get_rand(ctx, &k, f_rng, p_rng));

        /* A4: (x1, y1) = [k]G */
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(&ctx->grp, &point, &k, &ctx->grp.G,
                    NULL, NULL));

        /* A5: r = (e + x1) mod n; if (r == 0 || r + k == n) goto A3; */
        MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&e, hash,
                    mbedtls_md_get_size(mbedtls_md_info_from_type(md_alg))));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&r, &e, &point.X));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&r, &r, &ctx->grp.N));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&s, &r, &k));
        if (mbedtls_mpi_cmp_int(&r, 0) == 0 ||
                mbedtls_mpi_cmp_mpi(&s, &ctx->grp.N) == 0) {
            continue;
        }
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&r, sig,
                    mbedtls_mpi_size(&r)));

        /* A6: s = (((1 + d)^-1) * (k - r * d)) mod n; if (s == 0) goto A3; */
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&r, &r, &ctx->d));
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&r, &k, &r));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&s, &ctx->d, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&s, &s, &ctx->grp.N));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&s, &s, &r));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&s, &s, &ctx->grp.N));
        if (mbedtls_mpi_cmp_int(&s, 0) == 0) {
            continue;
        }

        break;
    } while (1);

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&s, sig + (ctx->grp.nbits + 7) / 8,
                mbedtls_mpi_size(&s)));

cleanup:
    mbedtls_mpi_free(&k);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_ecp_point_free(&point);

    return (ret);
}

int mbedtls_sm2_verify(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const unsigned char *hash, const unsigned char *sig)
{
    int ret = 0;
    mbedtls_mpi r;
    mbedtls_mpi s;
    mbedtls_mpi e;
    mbedtls_mpi t;
    mbedtls_ecp_point point;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&t);
    mbedtls_ecp_point_init(&point);

    /* B1,B2: check r, s in [1, n-1] */
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&r, sig, (ctx->grp.nbits + 7) / 8));
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&s, sig + (ctx->grp.nbits + 7) / 8,
            (ctx->grp.nbits + 7) / 8));
    if (mbedtls_mpi_cmp_int(&r, 1) < 0 ||
            mbedtls_mpi_cmp_mpi(&r, &ctx->grp.N) >= 0 ||
            mbedtls_mpi_cmp_int(&s, 1) < 0 ||
            mbedtls_mpi_cmp_mpi(&s, &ctx->grp.N) >= 0) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_BAD_SIGNATURE - 1);
    }

    /**
     * B3: M' = Z || M
     * B4: e' = Hash(M')
     * Parameter <hash> is the digest of <M'>, need convert to bignum <e>.
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&e, hash,
                mbedtls_md_get_size(mbedtls_md_info_from_type(md_alg))));

    /* B5: t = (r + s) mod n; if (t == 0) return error; */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&t, &r, &s));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&t, &t, &ctx->grp.N));
    if (mbedtls_mpi_cmp_int(&t, 0) == 0) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_BAD_SIGNATURE - 2);
    }

    /* B6: (x1, y1) = [s]G + [t]P */
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(&ctx->grp, &point,
                &s, &ctx->grp.G, &t, &ctx->Q));

    /* B7: R = (e + x1) mod n; if (R == r) Success; else Failed; */
    mbedtls_mpi_free(&t);
    mbedtls_mpi_init(&t);
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&t, &e, &point.X));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&t, &t, &ctx->grp.N));
    if (mbedtls_mpi_cmp_mpi(&t, &r) != 0) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_BAD_SIGNATURE - 3);
    }

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&t);
    mbedtls_ecp_point_free(&point);

    return (ret);
}

int mbedtls_sm2_hash_z(mbedtls_sm2_context *ctx, mbedtls_md_type_t md_alg,
        const char *id, size_t idlen, unsigned char *z)
{
    int ret = 0;
    unsigned char * m = NULL;
    unsigned char * p;
    size_t mlen;
    size_t l;
    const char * def_id = MBEDTLS_SM2_GMT09_DEFAULT_ID;
    size_t def_id_len = strlen(def_id);
    const mbedtls_md_info_t * md_info = NULL;

    if (id != NULL) {
        def_id = (char *)id;
        def_id_len = idlen;
    }
    md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_BAD_INPUT_DATA);
    }
    mlen = 2 + def_id_len
        + mbedtls_mpi_size(&ctx->grp.A) + mbedtls_mpi_size(&ctx->grp.B)
        + mbedtls_mpi_size(&ctx->grp.G.X) + mbedtls_mpi_size(&ctx->grp.G.Y)
        + mbedtls_mpi_size(&ctx->Q.X) + mbedtls_mpi_size(&ctx->Q.Y);
    if ((m = mbedtls_calloc(1, mlen)) == NULL) {
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_ALLOC_FAILED);
    }

    m[0] = (def_id_len >> 5) & 0xFF;
    m[1] = (def_id_len << 3) & 0xFF;
    p = m + 2;
    memmove(p, def_id, def_id_len);
    p += def_id_len;
    l = mbedtls_mpi_size(&ctx->grp.A);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->grp.A, p, l));
    p += l;
    l = mbedtls_mpi_size(&ctx->grp.B);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->grp.B, p, l));
    p += l;
    l = mbedtls_mpi_size(&ctx->grp.G.X);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->grp.G.X, p, l));
    p += l;
    l = mbedtls_mpi_size(&ctx->grp.G.Y);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->grp.G.Y, p, l));
    p += l;
    l = mbedtls_mpi_size(&ctx->Q.X);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->Q.X, p, l));
    p += l;
    l = mbedtls_mpi_size(&ctx->Q.Y);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->Q.Y, p, l));
    p += l;
    MBEDTLS_MPI_CHK(mbedtls_md(md_info, m, p - m, z));

cleanup:
    if (m) {
        mbedtls_free(m);
    }

    return (ret);
}

int mbedtls_sm2_hash_e(mbedtls_md_type_t md_alg, const unsigned char *z,
        const unsigned char *input, size_t ilen, unsigned char *e)
{
    int ret = 0;
    const mbedtls_md_info_t * md_info = NULL;
    mbedtls_md_context_t md_ctx;

    md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL)
        MBEDTLS_MPI_CHK(MBEDTLS_ERR_SM2_BAD_INPUT_DATA);

    mbedtls_md_init(&md_ctx);
    if((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0)
        goto cleanup;

    if ((ret = mbedtls_md_starts(&md_ctx)) != 0)
        return ret;
    if ((ret = mbedtls_md_update(&md_ctx, z, mbedtls_md_get_size(md_info))) != 0)
        return ret;
    if ((ret = mbedtls_md_update(&md_ctx, input, ilen)) != 0)
        return ret;
    if ((ret = mbedtls_md_finish(&md_ctx, e)) != 0)
        return ret;

cleanup:
    mbedtls_md_free(&md_ctx);

    return (ret);
}
#endif /* !MBEDTLS_SM2_SIGN_ALT */

#if !defined(MBEDTLS_SM2_GENKEY_ALT)
/*
 * Generate key pair
 */
int mbedtls_sm2_genkey(mbedtls_sm2_context *ctx, mbedtls_ecp_group_id gid,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    return (mbedtls_ecp_group_load(&ctx->grp, gid) ||
            mbedtls_ecp_gen_keypair(&ctx->grp, &ctx->d, &ctx->Q, f_rng, p_rng));
}
#endif /* !MBEDTLS_SM2_GENKEY_ALT */

int mbedtls_sm2_from_keypair(mbedtls_sm2_context *ctx,
        const mbedtls_ecp_keypair *key)
{
    int ret;

    if( ( ret = mbedtls_ecp_group_copy( &ctx->grp, &key->grp ) ) != 0 ||
        ( ret = mbedtls_mpi_copy( &ctx->d, &key->d ) ) != 0 ||
        ( ret = mbedtls_ecp_copy( &ctx->Q, &key->Q ) ) != 0 )
    {
        mbedtls_sm2_free( ctx );
    }

    return( ret );
}

void mbedtls_sm2_init(mbedtls_sm2_context *ctx)
{
    mbedtls_ecp_keypair_init(ctx);
}

void mbedtls_sm2_free(mbedtls_sm2_context *ctx)
{
    mbedtls_ecp_keypair_free(ctx);
}

#endif /* !MBEDTLS_SM2_ALT */

#if defined(MBEDTLS_SELF_TEST)

/*
 * SM2 test vectors from: GM/T 0003-2012 Chinese National Standard
 */
static const unsigned char sm2_test_plaintext[] = { /* "encryption standard" */
    0x65, 0x6E, 0x63, 0x72, 0x79, 0x70, 0x74, 0x69,
    0x6F, 0x6E, 0x20, 0x73, 0x74, 0x61, 0x6E, 0x64,
    0x61, 0x72, 0x64,
};
static const unsigned char sm2_test_ciphertext[] = {
    0x04, 0x24, 0x5C, 0x26, 0xFB, 0x68, 0xB1, 0xDD,
    0xDD, 0xB1, 0x2C, 0x4B, 0x6B, 0xF9, 0xF2, 0xB6,
    0xD5, 0xFE, 0x60, 0xA3, 0x83, 0xB0, 0xD1, 0x8D,
    0x1C, 0x41, 0x44, 0xAB, 0xF1, 0x7F, 0x62, 0x52,
    0xE7, 0x76, 0xCB, 0x92, 0x64, 0xC2, 0xA7, 0xE8,
    0x8E, 0x52, 0xB1, 0x99, 0x03, 0xFD, 0xC4, 0x73,
    0x78, 0xF6, 0x05, 0xE3, 0x68, 0x11, 0xF5, 0xC0,
    0x74, 0x23, 0xA2, 0x4B, 0x84, 0x40, 0x0F, 0x01,
    0xB8, 0x65, 0x00, 0x53, 0xA8, 0x9B, 0x41, 0xC4,
    0x18, 0xB0, 0xC3, 0xAA, 0xD0, 0x0D, 0x88, 0x6C,
    0x00, 0x28, 0x64, 0x67, 0x9C, 0x3D, 0x73, 0x60,
    0xC3, 0x01, 0x56, 0xFA, 0xB7, 0xC8, 0x0A, 0x02,
    0x76, 0x71, 0x2D, 0xA9, 0xD8, 0x09, 0x4A, 0x63,
    0x4B, 0x76, 0x6D, 0x3A, 0x28, 0x5E, 0x07, 0x48,
    0x06, 0x53, 0x42, 0x6D,
};
static const unsigned char sm2_test1_prik[] = {
    0x16, 0x49, 0xAB, 0x77, 0xA0, 0x06, 0x37, 0xBD,
    0x5E, 0x2E, 0xFE, 0x28, 0x3F, 0xBF, 0x35, 0x35,
    0x34, 0xAA, 0x7F, 0x7C, 0xB8, 0x94, 0x63, 0xF2,
    0x08, 0xDD, 0xBC, 0x29, 0x20, 0xBB, 0x0D, 0xA0,
};
static const unsigned char sm2_test1_pubk[] = {
    0x04,

    0x43, 0x5B, 0x39, 0xCC, 0xA8, 0xF3, 0xB5, 0x08,
    0xC1, 0x48, 0x8A, 0xFC, 0x67, 0xBE, 0x49, 0x1A,
    0x0F, 0x7B, 0xA0, 0x7E, 0x58, 0x1A, 0x0E, 0x48,
    0x49, 0xA5, 0xCF, 0x70, 0x62, 0x8A, 0x7E, 0x0A,

    0x75, 0xDD, 0xBA, 0x78, 0xF1, 0x5F, 0xEE, 0xCB,
    0x4C, 0x78, 0x95, 0xE2, 0xC1, 0xCD, 0xF5, 0xFE,
    0x01, 0xDE, 0xBB, 0x2C, 0xDB, 0xAD, 0xF4, 0x53,
    0x99, 0xCC, 0xF7, 0x7B, 0xBA, 0x07, 0x6A, 0x42,
};
static const unsigned char sm2_test1_rand_fix[] = {
    0x4C, 0x62, 0xEE, 0xFD, 0x6E, 0xCF, 0xC2, 0xB9,
    0x5B, 0x92, 0xFD, 0x6C, 0x3D, 0x95, 0x75, 0x14,
    0x8A, 0xFA, 0x17, 0x42, 0x55, 0x46, 0xD4, 0x90,
    0x18, 0xE5, 0x38, 0x8D, 0x49, 0xDD, 0x7B, 0x4F,
};

static const unsigned char sm2_test_messagetext[] = {   /* message digest */
    0x6D, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x20,
    0x64, 0x69, 0x67, 0x65, 0x73, 0x74,
};
static const unsigned char sm2_test2_rand_fix[] = {
    0x6C, 0xB2, 0x8D, 0x99, 0x38, 0x5C, 0x17, 0x5C,
    0x94, 0xF9, 0x4E, 0x93, 0x48, 0x17, 0x66, 0x3F,
    0xC1, 0x76, 0xD9, 0x25, 0xDD, 0x72, 0xB7, 0x27,
    0x26, 0x0D, 0xBA, 0xAE, 0x1F, 0xB2, 0xF9, 0x6F, 
};
static const unsigned char sm2_test2_prik[] = {
    0x12, 0x8B, 0x2F, 0xA8, 0xBD, 0x43, 0x3C, 0x6C,
    0x06, 0x8C, 0x8D, 0x80, 0x3D, 0xFF, 0x79, 0x79,
    0x2A, 0x51, 0x9A, 0x55, 0x17, 0x1B, 0x1B, 0x65,
    0x0C, 0x23, 0x66, 0x1D, 0x15, 0x89, 0x72, 0x63,
};
static const unsigned char sm2_test2_pubk[] = {
    0x04,

    0x0A, 0xE4, 0xC7, 0x79, 0x8A, 0xA0, 0xF1, 0x19,
    0x47, 0x1B, 0xEE, 0x11, 0x82, 0x5B, 0xE4, 0x62,
    0x02, 0xBB, 0x79, 0xE2, 0xA5, 0x84, 0x44, 0x95,
    0xE9, 0x7C, 0x04, 0xFF, 0x4D, 0xF2, 0x54, 0x8A,

    0x7C, 0x02, 0x40, 0xF8, 0x8F, 0x1C, 0xD4, 0xE1,
    0x63, 0x52, 0xA7, 0x3C, 0x17, 0xB7, 0xF1, 0x6F,
    0x07, 0x35, 0x3E, 0x53, 0xA1, 0x76, 0xD6, 0x84,
    0xA9, 0xFE, 0x0C, 0x6B, 0xB7, 0x98, 0xE8, 0x57,
};
static const char sm2_test_ID[] = {            /* "ALICE123@YAHOO.COM" */
    0x41, 0x4C, 0x49, 0x43, 0x45, 0x31, 0x32, 0x33,
    0x40, 0x59, 0x41, 0x48, 0x4F, 0x4F, 0x2E, 0x43,
    0x4F, 0x4D,
};
static const unsigned char sm2_test_z[] = {
    0xF4, 0xA3, 0x84, 0x89, 0xE3, 0x2B, 0x45, 0xB6,
    0xF8, 0x76, 0xE3, 0xAC, 0x21, 0x68, 0xCA, 0x39,
    0x23, 0x62, 0xDC, 0x8F, 0x23, 0x45, 0x9C, 0x1D,
    0x11, 0x46, 0xFC, 0x3D, 0xBF, 0xB7, 0xBC, 0x9A,
};
static const unsigned char sm2_test_md[] = {
    0xB5, 0x24, 0xF5, 0x52, 0xCD, 0x82, 0xB8, 0xB0,
    0x28, 0x47, 0x6E, 0x00, 0x5C, 0x37, 0x7F, 0xB1,
    0x9A, 0x87, 0xE6, 0xFC, 0x68, 0x2D, 0x48, 0xBB,
    0x5D, 0x42, 0xE3, 0xD9, 0xB9, 0xEF, 0xFE, 0x76,
};
static const unsigned char sm2_test_sign[] = {
    0x40, 0xF1, 0xEC, 0x59, 0xF7, 0x93, 0xD9, 0xF4,
    0x9E, 0x09, 0xDC, 0xEF, 0x49, 0x13, 0x0D, 0x41,
    0x94, 0xF7, 0x9F, 0xB1, 0xEE, 0xD2, 0xCA, 0xA5,
    0x5B, 0xAC, 0xDB, 0x49, 0xC4, 0xE7, 0x55, 0xD1,
    0x6F, 0xC6, 0xDA, 0xC3, 0x2C, 0x5D, 0x5C, 0xF1,
    0x0C, 0x77, 0xDF, 0xB2, 0x0F, 0x7C, 0x2E, 0xB6,
    0x67, 0xA4, 0x57, 0x87, 0x2F, 0xB0, 0x9E, 0xC5,
    0x63, 0x27, 0xA6, 0x7E, 0xC7, 0xDE, 0xEB, 0xE7,
};

static int sm2_set_fix_rng(void * p_rng, unsigned char * buf, size_t size)
{
    memmove(buf, p_rng, size);
    return 0;
}

int mbedtls_sm2_self_test(int verbose)
{
    int ret = 0;
    mbedtls_sm2_context ctx;
    unsigned char output[512];
    size_t olen;

    mbedtls_sm2_init(&ctx);
    if ((ret = mbedtls_ecp_group_load(&ctx.grp,
                    MBEDTLS_ECP_DP_SM2P256T1)) != 0) {
        mbedtls_printf("load group failed\n");
        goto cleanup;
    }

    if( verbose != 0 )
        mbedtls_printf( "  SM2 key validation: " );

    if ((ret = mbedtls_mpi_read_binary(&ctx.d,
                    sm2_test1_prik, sizeof(sm2_test1_prik))) != 0) {
        mbedtls_printf("read private key1 failed\n");
        goto cleanup;
    }
    if ((ret = mbedtls_ecp_point_read_binary(&ctx.grp, &ctx.Q,
            sm2_test1_pubk, sizeof(sm2_test1_pubk))) != 0) {
        mbedtls_printf("read public key1 failed\n");
        goto cleanup;
    }

    if (((ret = mbedtls_ecp_check_pubkey(&ctx.grp, &ctx.Q)) != 0) ||
            (ret = mbedtls_ecp_check_privkey(&ctx.grp, &ctx.d) != 0)) {
        if (verbose != 0) {
            mbedtls_printf("failed\n");
        }
        goto cleanup;
    }

    if (verbose)
        mbedtls_printf("passed\n  SM2 encryption: ");

    if ((ret = mbedtls_sm2_encrypt(&ctx, MBEDTLS_MD_SM3,
                    sm2_test_plaintext, sizeof(sm2_test_plaintext),
                    output, &olen,
                    sm2_set_fix_rng, (void *)sm2_test1_rand_fix)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "failed\n" );
        }
        goto cleanup;
    }
    if (memcmp(output, sm2_test_ciphertext, sizeof(sm2_test_ciphertext)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "check failed\n" );
        }
        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n  SM2 decryption: ");
    }

    if ((ret = mbedtls_sm2_decrypt(&ctx, MBEDTLS_MD_SM3,
                    sm2_test_ciphertext, sizeof(sm2_test_ciphertext),
                    output, &olen)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "failed\n" );
        }
        goto cleanup;
    }
    if (memcmp(output, sm2_test_plaintext, sizeof(sm2_test_plaintext)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "check failed\n" );
        }
        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n  SM2 key validation: ");
    }

    if ((ret = mbedtls_mpi_read_binary(&ctx.d,
                    sm2_test2_prik, sizeof(sm2_test2_prik))) != 0) {
        mbedtls_printf("read private key2 failed\n");
        goto cleanup;
    }
    if ((ret = mbedtls_ecp_point_read_binary(&ctx.grp, &ctx.Q,
                    sm2_test2_pubk, sizeof(sm2_test2_pubk))) != 0) {
        mbedtls_printf("read public key2 failed\n");
        goto cleanup;
    }

    if (((ret = mbedtls_ecp_check_pubkey(&ctx.grp, &ctx.Q)) != 0) ||
            (ret = mbedtls_ecp_check_privkey(&ctx.grp, &ctx.d) != 0)) {
        if (verbose != 0) {
            mbedtls_printf("failed\n");
        }
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n  SM2 Get Z: ");
    }

    if ((ret = mbedtls_sm2_hash_z(&ctx, MBEDTLS_MD_SM3,
                    sm2_test_ID, sizeof(sm2_test_ID), output)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "failed\n" );
        }
        goto cleanup;
    }
    if (memcmp(output, sm2_test_z, sizeof(sm2_test_z)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "check failed\n" );
        }
        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n  SM2 Get hash: ");
    }

    if ((ret = mbedtls_sm2_hash_e(MBEDTLS_MD_SM3,
                    sm2_test_z, sm2_test_messagetext,
                    sizeof(sm2_test_messagetext), output)) != 0) {
        if (verbose != 0) {
            mbedtls_printf("failed\n");
        }
        goto cleanup;
    }
    if (memcmp(output, sm2_test_md, sizeof(sm2_test_md)) != 0) {
        if (verbose != 0) {
            mbedtls_printf("check failed\n");
        }
        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n  SM2 sign: ");
    }

    if ((ret = mbedtls_sm2_sign(&ctx, MBEDTLS_MD_SM3, sm2_test_md, output,
                    sm2_set_fix_rng, (void *)sm2_test2_rand_fix)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "failed\n" );
        }
        goto cleanup;
    }
    if (memcmp(output, sm2_test_sign, sizeof(sm2_test_sign)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "check failed\n" );
        }
        ret = 1;
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n  SM2 verify: ");
    }

    if ((ret = mbedtls_sm2_verify(&ctx, MBEDTLS_MD_SM3,
                    sm2_test_md, sm2_test_sign)) != 0) {
        if (verbose != 0) {
            mbedtls_printf( "failed\n" );
        }
        goto cleanup;
    }

    if (verbose != 0) {
        mbedtls_printf("passed\n");
    }

    if (verbose != 0) {
        mbedtls_printf("\n");
    }

cleanup:
    mbedtls_sm2_free(&ctx);
    return ret;
}
#endif /* MBEDTLS_SELF_TEST */

#endif /* MBEDTLS_SM2_C */