#include "bflb_sec_sha.h"
#include "hardware/sec_eng_reg.h"

#define PUT_UINT32_BE(n, b, i)               \
    {                                        \
        (b)[(i)] = (uint8_t)((n) >> 24);     \
        (b)[(i) + 1] = (uint8_t)((n) >> 16); \
        (b)[(i) + 2] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 3] = (uint8_t)((n));       \
    }
#define PUT_UINT32(n, b, i)                  \
    {                                        \
        (b)[(i) + 3] = (uint8_t)((n) >> 24); \
        (b)[(i) + 2] = (uint8_t)((n) >> 16); \
        (b)[(i) + 1] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 0] = (uint8_t)((n));       \
    }
#define PUT_UINT64_BE(n, b, i)               \
    {                                        \
        (b)[(i)] = (uint8_t)((n) >> 56);     \
        (b)[(i) + 1] = (uint8_t)((n) >> 48); \
        (b)[(i) + 2] = (uint8_t)((n) >> 40); \
        (b)[(i) + 3] = (uint8_t)((n) >> 32); \
        (b)[(i) + 4] = (uint8_t)((n) >> 24); \
        (b)[(i) + 5] = (uint8_t)((n) >> 16); \
        (b)[(i) + 6] = (uint8_t)((n) >> 8);  \
        (b)[(i) + 7] = (uint8_t)((n));       \
    }

void bflb_sha_init(struct bflb_device_s *dev, uint8_t mode)
{
#ifdef romapi_bflb_sha_init
    romapi_bflb_sha_init(dev, mode);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval &= ~SEC_ENG_SE_SHA_0_MODE_EXT_MASK;
    regval &= ~SEC_ENG_SE_SHA_0_MODE_MASK;
    if (mode < 8) {
        regval |= (mode << SEC_ENG_SE_SHA_0_MODE_SHIFT);
    } else {
        regval |= ((mode - 7) << SEC_ENG_SE_SHA_0_MODE_EXT_SHIFT);
    }

    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

void bflb_sha1_start(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx)
{
#ifdef romapi_bflb_sha1_start
    romapi_bflb_sha1_start(dev, ctx);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    arch_memset(ctx, 0, sizeof(struct bflb_sha1_ctx_s));
    ctx->sha_padding[0] = 0x80;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_SHA_0_EN;
    regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

void bflb_sha256_start(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx)
{
#ifdef romapi_bflb_sha256_start
    romapi_bflb_sha256_start(dev, ctx);
#else
    return bflb_sha1_start(dev, (struct bflb_sha1_ctx_s *)ctx);
#endif
}

void bflb_sha512_start(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx)
{
#ifdef romapi_bflb_sha512_start
    romapi_bflb_sha512_start(dev, ctx);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    arch_memset(ctx, 0, sizeof(struct bflb_sha512_ctx_s));
    ctx->sha_padding[0] = 0x80;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_SHA_0_EN;
    regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

int bflb_sha1_update(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, const uint8_t *input, uint32_t len)
{
#ifdef romapi_bflb_sha1_update
    return romapi_bflb_sha1_update(dev, ctx, input, len);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t fill;
    uint32_t left;
    uint64_t start_time;

    if (len == 0) {
        return 0;
    }

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    if (ctx->sha_feed) {
        regval |= SEC_ENG_SE_SHA_0_HASH_SEL;
    } else {
        regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
    }

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += len;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < len) {
        ctx->total[1]++;
    }

    if (left && len >= fill) {
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, fill);
        putreg32((uint32_t)(uintptr_t)ctx->sha_buf, reg_base + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        regval &= ~SEC_ENG_SE_SHA_0_MSG_LEN_MASK;
        regval |= (1 << SEC_ENG_SE_SHA_0_MSG_LEN_SHIFT);
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        ctx->sha_feed = 1;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 64;
    len = len % 64;

    if (fill > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* SHA need set se_sha_sel to 1 to keep the last sha state */
        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        if (ctx->sha_feed) {
            regval |= SEC_ENG_SE_SHA_0_HASH_SEL;
        } else {
            regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
        }
        /* Fill data */
        putreg32((uintptr_t)input, reg_base + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        regval &= ~SEC_ENG_SE_SHA_0_MSG_LEN_MASK;
        regval |= (fill << SEC_ENG_SE_SHA_0_MSG_LEN_SHIFT);
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 64);
        ctx->sha_feed = 1;
    }

    if (len > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* Copy left data into temp buffer */
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, len);
    }

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    return 0;
#endif
}

int bflb_sha1_once_padded(struct bflb_device_s *dev, const uint8_t *input, uint8_t *output, uint32_t nblock)
{
    uint32_t regval;
    uint32_t reg_base;
    uint64_t start_time;

    if (nblock == 0) {
        return 0;
    }

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_SHA_0_EN;
    regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;

    putreg32((uint32_t)(uintptr_t)input, reg_base + SEC_ENG_SE_SHA_0_MSA_OFFSET);
    regval &= ~SEC_ENG_SE_SHA_0_MSG_LEN_MASK;
    regval |= (nblock << SEC_ENG_SE_SHA_0_MSG_LEN_SHIFT);
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_0_OFFSET);
    *output++ = (regval & 0xff);
    *output++ = ((regval >> 8) & 0xff);
    *output++ = ((regval >> 16) & 0xff);
    *output++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_1_OFFSET);
    *output++ = (regval & 0xff);
    *output++ = ((regval >> 8) & 0xff);
    *output++ = ((regval >> 16) & 0xff);
    *output++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_2_OFFSET);
    *output++ = (regval & 0xff);
    *output++ = ((regval >> 8) & 0xff);
    *output++ = ((regval >> 16) & 0xff);
    *output++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_3_OFFSET);
    *output++ = (regval & 0xff);
    *output++ = ((regval >> 8) & 0xff);
    *output++ = ((regval >> 16) & 0xff);
    *output++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_4_OFFSET);
    *output++ = (regval & 0xff);
    *output++ = ((regval >> 8) & 0xff);
    *output++ = ((regval >> 16) & 0xff);
    *output++ = ((regval >> 24) & 0xff);

    return 0;
}

int bflb_sha256_update(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, const uint8_t *input, uint32_t len)
{
#ifdef romapi_bflb_sha256_update
    return romapi_bflb_sha256_update(dev, ctx, input, len);
#else
    return bflb_sha1_update(dev, (struct bflb_sha1_ctx_s *)ctx, input, len);
#endif
}

int bflb_sha512_update(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, const uint8_t *input, uint64_t len)
{
#ifdef romapi_bflb_sha512_update
    return romapi_bflb_sha512_update(dev, ctx, input, len);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t fill;
    uint32_t left;
    uint64_t start_time;

    if (len == 0) {
        return 0;
    }

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    if (ctx->sha_feed) {
        regval |= SEC_ENG_SE_SHA_0_HASH_SEL;
    } else {
        regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
    }

    left = ctx->total[0] & 0x7F;
    fill = 128 - left;

    ctx->total[0] += (uint64_t)len;

    if (ctx->total[0] < (uint64_t)len) {
        ctx->total[1]++;
    }

    if (left && len >= fill) {
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, fill);
        putreg32((uint32_t)(uintptr_t)ctx->sha_buf, reg_base + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        regval &= ~SEC_ENG_SE_SHA_0_MSG_LEN_MASK;
        regval |= (1 << SEC_ENG_SE_SHA_0_MSG_LEN_SHIFT);
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        ctx->sha_feed = 1;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 128;
    len = len % 128;

    if (fill > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* SHA need set se_sha_sel to 1 to keep the last sha state */
        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        if (ctx->sha_feed) {
            regval |= SEC_ENG_SE_SHA_0_HASH_SEL;
        } else {
            regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
        }
        /* Fill data */
        putreg32((uintptr_t)input, reg_base + SEC_ENG_SE_SHA_0_MSA_OFFSET);

        regval &= ~SEC_ENG_SE_SHA_0_MSG_LEN_MASK;
        regval |= (fill << SEC_ENG_SE_SHA_0_MSG_LEN_SHIFT);
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 128);
        ctx->sha_feed = 1;
    }

    if (len > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* Copy left data into temp buffer */
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, len);
    }

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    return 0;
#endif
}

void bflb_sha1_finish(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, uint8_t *output)
{
#ifdef romapi_bflb_sha1_finish
    romapi_bflb_sha1_finish(dev, ctx, output);
#else
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t msgLen[8];
    uint32_t regval;
    uint32_t reg_base;
    uint8_t mode;
    uint8_t *p = (uint8_t *)output;

    reg_base = dev->reg_base;

    high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);

    PUT_UINT32_BE(high, msgLen, 0);
    PUT_UINT32_BE(low, msgLen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    bflb_sha1_update(dev, ctx, (uint8_t *)ctx->sha_padding, padn);
    arch_memcpy_fast(ctx->sha_padding, msgLen, 8);
    bflb_sha1_update(dev, ctx, (uint8_t *)ctx->sha_padding, 8);

    /* Copy SHA value */
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_0_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_1_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_2_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_3_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_4_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);

    mode = (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_MODE_MASK) >> SEC_ENG_SE_SHA_0_MODE_SHIFT;

    if ((mode == SHA_MODE_SHA224) || (mode == SHA_MODE_SHA256)) {
        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_5_OFFSET);
        *p++ = (regval & 0xff);
        *p++ = ((regval >> 8) & 0xff);
        *p++ = ((regval >> 16) & 0xff);
        *p++ = ((regval >> 24) & 0xff);
        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_6_OFFSET);
        *p++ = (regval & 0xff);
        *p++ = ((regval >> 8) & 0xff);
        *p++ = ((regval >> 16) & 0xff);
        *p++ = ((regval >> 24) & 0xff);

        if (mode == SHA_MODE_SHA256) {
            regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_7_OFFSET);
            *p++ = (regval & 0xff);
            *p++ = ((regval >> 8) & 0xff);
            *p++ = ((regval >> 16) & 0xff);
            *p++ = ((regval >> 24) & 0xff);
        }
    }

    /* Disable SHA engine*/
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
    regval &= ~SEC_ENG_SE_SHA_0_EN;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

void bflb_sha256_finish(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, uint8_t *output)
{
#ifdef romapi_bflb_sha256_finish
    romapi_bflb_sha256_finish(dev, ctx, output);
#else
    return bflb_sha1_finish(dev, (struct bflb_sha1_ctx_s *)ctx, output);
#endif
}

void bflb_sha512_finish(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, uint8_t *output)
{
#ifdef romapi_bflb_sha512_finish
    romapi_bflb_sha512_finish(dev, ctx, output);
#else
    uint64_t last, padn;
    uint64_t high, low;
    uint8_t msgLen[16];
    uint8_t mode;
    uint32_t regval;
    uint32_t reg_base;
    uint8_t *p = (uint8_t *)output;

    reg_base = dev->reg_base;

    high = (ctx->total[0] >> 61) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);

    PUT_UINT64_BE(high, msgLen, 0);
    PUT_UINT64_BE(low, msgLen, 8);

    last = ctx->total[0] & 0x7F;
    padn = (last < 112) ? (112 - last) : (240 - last);

    bflb_sha512_update(dev, ctx, (uint8_t *)ctx->sha_padding, padn);
    arch_memcpy_fast(ctx->sha_padding, msgLen, 16);
    bflb_sha512_update(dev, ctx, (uint8_t *)ctx->sha_padding, 16);

    /* Copy SHA value */
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_0_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_0_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_1_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_1_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_2_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_2_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_3_OFFSET);
    *p++ = (regval & 0xff);
    *p++ = ((regval >> 8) & 0xff);
    *p++ = ((regval >> 16) & 0xff);
    *p++ = ((regval >> 24) & 0xff);

    mode = (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_MODE_MASK) >> SEC_ENG_SE_SHA_0_MODE_SHIFT;

    if ((mode == SHA_MODE_SHA512) || (mode == SHA_MODE_SHA384) || (mode == SHA_MODE_SHA512T256)) {
        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_3_OFFSET);
        *p++ = (regval & 0xff);
        *p++ = ((regval >> 8) & 0xff);
        *p++ = ((regval >> 16) & 0xff);
        *p++ = ((regval >> 24) & 0xff);

        if ((mode == SHA_MODE_SHA512) || (mode == SHA_MODE_SHA384)) {
            regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_4_OFFSET);
            *p++ = (regval & 0xff);
            *p++ = ((regval >> 8) & 0xff);
            *p++ = ((regval >> 16) & 0xff);
            *p++ = ((regval >> 24) & 0xff);
            regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_4_OFFSET);
            *p++ = (regval & 0xff);
            *p++ = ((regval >> 8) & 0xff);
            *p++ = ((regval >> 16) & 0xff);
            *p++ = ((regval >> 24) & 0xff);
            regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_5_OFFSET);
            *p++ = (regval & 0xff);
            *p++ = ((regval >> 8) & 0xff);
            *p++ = ((regval >> 16) & 0xff);
            *p++ = ((regval >> 24) & 0xff);
            regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_5_OFFSET);
            *p++ = (regval & 0xff);
            *p++ = ((regval >> 8) & 0xff);
            *p++ = ((regval >> 16) & 0xff);
            *p++ = ((regval >> 24) & 0xff);

            if (mode == SHA_MODE_SHA512) {
                regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_6_OFFSET);
                *p++ = (regval & 0xff);
                *p++ = ((regval >> 8) & 0xff);
                *p++ = ((regval >> 16) & 0xff);
                *p++ = ((regval >> 24) & 0xff);
                regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_6_OFFSET);
                *p++ = (regval & 0xff);
                *p++ = ((regval >> 8) & 0xff);
                *p++ = ((regval >> 16) & 0xff);
                *p++ = ((regval >> 24) & 0xff);
                regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_H_7_OFFSET);
                *p++ = (regval & 0xff);
                *p++ = ((regval >> 8) & 0xff);
                *p++ = ((regval >> 16) & 0xff);
                *p++ = ((regval >> 24) & 0xff);
                regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_HASH_L_7_OFFSET);
                *p++ = (regval & 0xff);
                *p++ = ((regval >> 8) & 0xff);
                *p++ = ((regval >> 16) & 0xff);
                *p++ = ((regval >> 24) & 0xff);
            }
        }
    }

    /* Disable SHA engine*/
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval &= ~SEC_ENG_SE_SHA_0_HASH_SEL;
    regval &= ~SEC_ENG_SE_SHA_0_EN;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

void bflb_sha_link_init(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_sha_link_init
    romapi_bflb_sha_link_init(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_SHA_0_EN;
    regval |= SEC_ENG_SE_SHA_0_LINK_MODE;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

void bflb_sha_link_deinit(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_sha_link_deinit
    romapi_bflb_sha_link_deinit(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval &= ~SEC_ENG_SE_SHA_0_EN;
    regval &= ~SEC_ENG_SE_SHA_0_LINK_MODE;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
#endif
}

void bflb_sha1_link_start(struct bflb_device_s *dev, struct bflb_sha1_link_ctx_s *ctx)
{
#ifdef romapi_bflb_sha1_link_start
    romapi_bflb_sha1_link_start(dev, ctx);
#else
    arch_memset(ctx, 0, sizeof(struct bflb_sha1_link_ctx_s));
    ctx->sha_padding[0] = 0x80;
    ctx->link_addr = (uint32_t)(uintptr_t)&ctx->link_cfg;
    ctx->link_cfg.sha_mode = SHA_MODE_SHA1;
#endif
}

void bflb_sha256_link_start(struct bflb_device_s *dev, struct bflb_sha256_link_ctx_s *ctx, int is224)
{
#ifdef romapi_bflb_sha256_link_start
    romapi_bflb_sha256_link_start(dev, ctx, is224);
#else
    arch_memset(ctx, 0, sizeof(struct bflb_sha1_link_ctx_s));
    ctx->sha_padding[0] = 0x80;
    ctx->link_addr = (uint32_t)(uintptr_t)&ctx->link_cfg;
    if (is224) {
        ctx->link_cfg.sha_mode = SHA_MODE_SHA224;
    } else {
        ctx->link_cfg.sha_mode = SHA_MODE_SHA256;
    }
#endif
}

void bflb_sha512_link_start(struct bflb_device_s *dev, struct bflb_sha512_link_ctx_s *ctx, int is384)
{
#ifdef romapi_bflb_sha512_link_start
    romapi_bflb_sha512_link_start(dev, ctx, is384);
#else
    arch_memset(ctx, 0, sizeof(struct bflb_sha512_link_ctx_s));
    ctx->sha_padding[0] = 0x80;
    ctx->link_addr = (uint32_t)(uintptr_t)&ctx->link_cfg;
    if (is384) {
        ctx->link_cfg.sha_mode = SHA_MODE_SHA384;
    } else {
        ctx->link_cfg.sha_mode = SHA_MODE_SHA512;
    }
#endif
}

int bflb_sha1_link_update(struct bflb_device_s *dev,
                          struct bflb_sha1_link_ctx_s *ctx,
                          const uint8_t *input,
                          uint32_t len)
{
#ifdef romapi_bflb_sha1_link_update
    return romapi_bflb_sha1_link_update(dev, ctx, input, len);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t fill;
    uint32_t left;
    uint64_t start_time;

    if (len == 0) {
        return 0;
    }

    reg_base = dev->reg_base;

    /* Set link address */
    putreg32(ctx->link_addr, reg_base + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += len;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < len) {
        ctx->total[1]++;
    }

    if (left && len >= fill) {
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, fill);
        /* Set data source address */
        *(uint32_t *)(uintptr_t)(ctx->link_addr + 4) = (uint32_t)(uintptr_t)ctx->sha_buf;

        /* Set data length */
        *((uint16_t *)(uintptr_t)ctx->link_addr + 1) = 1;

        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)ctx->link_addr) |= 0x40;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 64;
    len = len % 64;

    if (fill > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* Fill data */
        *(uint32_t *)(uintptr_t)(ctx->link_addr + 4) = (uint32_t)(uintptr_t)input;
        *((uint16_t *)(uintptr_t)ctx->link_addr + 1) = fill;

        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 64);
        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)ctx->link_addr) |= 0x40;
    }

    if (len > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* Copy left data into temp buffer */
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, len);
    }

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    return 0;
#endif
}

int bflb_sha256_link_update(struct bflb_device_s *dev,
                            struct bflb_sha256_link_ctx_s *ctx,
                            const uint8_t *input,
                            uint32_t len)
{
#ifdef romapi_bflb_sha256_link_update
    return romapi_bflb_sha256_link_update(dev, ctx, input, len);
#else
    return bflb_sha1_link_update(dev, (struct bflb_sha1_link_ctx_s *)ctx, input, len);
#endif
}

int bflb_sha512_link_update(struct bflb_device_s *dev,
                            struct bflb_sha512_link_ctx_s *ctx,
                            const uint8_t *input,
                            uint64_t len)
{
#ifdef romapi_bflb_sha512_link_update
    return romapi_bflb_sha512_link_update(dev, ctx, input, len);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t fill;
    uint32_t left;
    uint64_t start_time;

    if (len == 0) {
        return 0;
    }

    reg_base = dev->reg_base;

    /* Set link address */
    putreg32(ctx->link_addr, reg_base + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    left = ctx->total[0] & 0x7F;
    fill = 128 - left;

    ctx->total[0] += len;

    if (ctx->total[0] < len) {
        ctx->total[1]++;
    }

    if (left && len >= fill) {
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, fill);
        /* Set data source address */
        *(uint32_t *)(uintptr_t)(ctx->link_addr + 4) = (uint32_t)(uintptr_t)ctx->sha_buf;

        /* Set data length */
        *((uint16_t *)(uintptr_t)ctx->link_addr + 1) = 1;

        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)ctx->link_addr) |= 0x40;
        input += fill;
        len -= fill;
        left = 0;
    }

    fill = len / 128;
    len = len % 128;

    if (fill > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* Fill data */
        *(uint32_t *)(uintptr_t)(ctx->link_addr + 4) = (uint32_t)(uintptr_t)input;
        *((uint16_t *)(uintptr_t)ctx->link_addr + 1) = fill;

        regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
        regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
        putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

        input += (fill * 128);
        /* Choose accumulating last hash in the next time */
        *((uint32_t *)(uintptr_t)ctx->link_addr) |= 0x40;
    }

    if (len > 0) {
        start_time = bflb_mtimer_get_time_ms();
        while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
            if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
                return -ETIMEDOUT;
            }
        }

        /* Copy left data into temp buffer */
        arch_memcpy_fast((void *)((uint8_t *)ctx->sha_buf + left), input, len);
    }

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }
    return 0;
#endif
}

void bflb_sha1_link_finish(struct bflb_device_s *dev,
                           struct bflb_sha1_link_ctx_s *ctx,
                           uint8_t *output)
{
#ifdef romapi_bflb_sha1_link_finish
    return romapi_bflb_sha1_link_finish(dev, ctx, output);
#else
    uint32_t last, padn;
    uint32_t high, low;
    uint8_t msgLen[8];

    uint32_t reg_base;
    uint32_t sha_mode = (*(uint32_t *)(uintptr_t)ctx->link_addr) >> 2 & 0x7;

    reg_base = dev->reg_base;

    /* Set link address */
    putreg32(ctx->link_addr, reg_base + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);

    PUT_UINT32_BE(high, msgLen, 0);
    PUT_UINT32_BE(low, msgLen, 4);

    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    bflb_sha1_link_update(dev, ctx, (uint8_t *)ctx->sha_padding, padn);
    bflb_sha1_link_update(dev, ctx, msgLen, 8);

    /* Get result according to SHA mode,result is placed in (link address + offset:8) */
    switch (sha_mode) {
        case 0:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 32);
            break;

        case 1:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 28);
            break;

        case 2:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 20);
            break;

        case 3:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 20);
            break;

        default:
            break;
    }

    /* Choose new hash in the next time */
    *((uint32_t *)(uintptr_t)ctx->link_addr) &= ~0x40;
#endif
}

void bflb_sha256_link_finish(struct bflb_device_s *dev,
                             struct bflb_sha256_link_ctx_s *ctx,
                             uint8_t *output)
{
#ifdef romapi_bflb_sha256_link_finish
    return romapi_bflb_sha256_link_finish(dev, ctx, output);
#else
    return bflb_sha1_link_finish(dev, (struct bflb_sha1_link_ctx_s *)ctx, output);
#endif
}

void bflb_sha512_link_finish(struct bflb_device_s *dev,
                             struct bflb_sha512_link_ctx_s *ctx,
                             uint8_t *output)
{
#ifdef romapi_bflb_sha512_link_finish
    romapi_bflb_sha512_link_finish(dev, ctx, output);
#else
    uint64_t last, padn;
    uint64_t high, low;
    uint8_t msgLen[16];

    uint32_t reg_base;
    uint32_t sha_mode = (*(uint32_t *)(uintptr_t)ctx->link_addr) >> 2 & 0x7;

    reg_base = dev->reg_base;

    /* Set link address */
    putreg32(ctx->link_addr, reg_base + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    high = (ctx->total[0] >> 61) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);

    PUT_UINT64_BE(high, msgLen, 0);
    PUT_UINT64_BE(low, msgLen, 8);

    last = ctx->total[0] & 0x7F;
    padn = (last < 112) ? (112 - last) : (240 - last);

    bflb_sha512_link_update(dev, ctx, (uint8_t *)ctx->sha_padding, padn);
    bflb_sha512_link_update(dev, ctx, msgLen, 16);

    /* Get result according to SHA mode,result is placed in (link address + offset:8) */
    switch (sha_mode) {
        case 4:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 64);
            break;

        case 5:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 48);
            break;

        case 6:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 28);
            break;

        case 7:
            arch_memcpy_fast(output, (uint8_t *)(uintptr_t)(ctx->link_addr + 8), 32);
            break;

        default:
            break;
    }

    /* Choose new hash in the next time */
    *((uint32_t *)(uintptr_t)ctx->link_addr) &= ~0x40;
#endif
}

int bflb_group0_request_sha_access(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_group0_request_sha_access
    return romapi_bflb_group0_request_sha_access(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);
    if ((regval & 0x03) == 0x03) {
        putreg32(0x02, reg_base + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);

        regval = getreg32(reg_base + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);
        if ((regval & 0x03) == 0x01) {
            return 0;
        }
    }

    return -1;
#endif
}

int bflb_group1_request_sha_access(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_group1_request_sha_access
    return romapi_bflb_group1_request_sha_access(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);
    if ((regval & 0x03) == 0x03) {
        putreg32(0x04, reg_base + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);

        regval = getreg32(reg_base + SEC_ENG_SE_CTRL_PROT_RD_OFFSET);
        if ((regval & 0x03) == 0x02) {
            return 0;
        }
    }

    return -1;
#endif
}

void bflb_group0_release_sha_access(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_group0_release_sha_access
    romapi_bflb_group0_release_sha_access(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(0x06, reg_base + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);
#endif
}

void bflb_group1_release_sha_access(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_group1_release_sha_access
    romapi_bflb_group1_release_sha_access(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(0x06, reg_base + SEC_ENG_SE_SHA_0_CTRL_PROT_OFFSET);
#endif
}

int bflb_sha_crc32_link_work(struct bflb_device_s *dev, uint32_t addr, const uint8_t *in, uint32_t len, uint8_t *out)
{
#ifdef romapi_bflb_sha_crc32_link_work
    return romapi_bflb_sha_crc32_link_work(dev, addr, in, len, out);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint64_t start_time;

    reg_base = dev->reg_base;
    /* Link address should word align */
    if ((addr & 0x03) != 0) {
        return -EINVAL;
    }

    /* Set link address */
    putreg32(addr, reg_base + SEC_ENG_SE_SHA_0_LINK_OFFSET);

    /* Change source buffer address and destination buffer address */
    *(uint32_t *)(uintptr_t)(addr + 4) = (uint32_t)(uintptr_t)in;

    /* Set data length , 32 bits per block*/
    *((uint16_t *)(uintptr_t)addr + 1) = len * 8 / 32;

    /* Trigger */
    regval = getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);
    regval |= SEC_ENG_SE_SHA_0_TRIG_1T;
    putreg32(regval, reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET);

    start_time = bflb_mtimer_get_time_ms();
    while (getreg32(reg_base + SEC_ENG_SE_SHA_0_CTRL_OFFSET) & SEC_ENG_SE_SHA_0_BUSY) {
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return -ETIMEDOUT;
        }
    }

    /* CRC32 code len is 32 bits */
    arch_memcpy_fast(out, (uint8_t *)(uintptr_t)(addr + 0x10), 4);

    return 0;
#endif
}