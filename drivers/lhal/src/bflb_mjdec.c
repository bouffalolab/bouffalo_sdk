#include "bflb_mjdec.h"
#include "hardware/mjdec_reg.h"

__UNUSED static const uint8_t q_table_50_yy[8][8] = {
    {16, 11, 10, 16,  24,  40,  51,  61,},
    {12, 12, 14, 19,  26,  58,  60,  55,},
    {14, 13, 16, 24,  40,  57,  69,  56,},
    {14, 17, 22, 29,  51,  87,  80,  62,},
    {18, 22, 37, 56,  68, 109, 103,  77,},
    {24, 35, 55, 64,  81, 104, 113,  92,},
    {49, 64, 78, 87, 103, 121, 120, 101,},
    {72, 92, 95, 98, 112, 100, 103,  99,},
};

__UNUSED static const uint8_t q_table_50_uv[8][8] = {
    {17, 18, 24, 47, 99, 99, 99, 99,},
    {18, 21, 26, 66, 99, 99, 99, 99,},
    {24, 26, 56, 99, 99, 99, 99, 99,},
    {47, 66, 99, 99, 99, 99, 99, 99,},
    {99, 99, 99, 99, 99, 99, 99, 99,},
    {99, 99, 99, 99, 99, 99, 99, 99,},
    {99, 99, 99, 99, 99, 99, 99, 99,},
    {99, 99, 99, 99, 99, 99, 99, 99,},
};

__UNUSED static const uint8_t q_zig_zag[64] =
{
    0,   1,  5,  6, 14, 15, 27, 28,
    2,   4,  7, 13, 16, 26, 29, 42,
    3,   8, 12, 17, 25, 30, 41, 43,
    9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63,
};

__UNUSED static const uint8_t q_zig_zag_reverse[64] =
{
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

__UNUSED static void bflb_mjdec_set_framesize(struct bflb_device_s *dev, uint16_t x, uint16_t y)
{
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = 0;
    regval |= (x << MJDEC_REG_FRAME_WBLK_SHIFT);
    regval |= (y << MJDEC_REG_FRAME_HBLK_SHIFT);
    putreg32(regval, reg_base + MJDEC_JDEC_FRAME_SIZE_OFFSET);
}

#if defined(BL616D)
void bflb_mjdec_set_dqt_from_spec(struct bflb_device_s *dev, uint8_t (*yy)[8], uint8_t (*uv)[8])
{
#ifdef romapi_bflb_mjdec_set_dqt_from_spec
    romapi_bflb_mjdec_set_dqt_from_spec(dev, yy, uv);
#else
    uint32_t regval_0, regval_1;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    if (yy) {
        for (uint8_t x = 0; x < 8; x++) {
            regval_0 = 0;
            regval_1 = 0;
            for (uint8_t y = 0; y < 4; y++) {
                regval_0 |= ((uint32_t)yy[y][x] << (8 * y));
                regval_1 |= ((uint32_t)yy[y + 4][x] << (8 * y));
            }
            putreg32(regval_0, reg_base + MJDEC_YY_Q0_OFFSET + x * 8);
            putreg32(regval_1, reg_base + MJDEC_YY_Q1_OFFSET + x * 8);
        }
    }
    if (uv) {
        for (uint8_t x = 0; x < 8; x++) {
            regval_0 = 0;
            regval_1 = 0;
            for (uint8_t y = 0; y < 4; y++) {
                regval_0 |= ((uint32_t)uv[y][x] << (8 * y));
                regval_1 |= ((uint32_t)uv[y + 4][x] << (8 * y));
            }
            putreg32(regval_0, reg_base + MJDEC_UV_Q0_OFFSET + x * 8);
            putreg32(regval_1, reg_base + MJDEC_UV_Q1_OFFSET + x * 8);
        }
    }
#endif
}

void bflb_mjdec_set_dqt_from_zigzag(struct bflb_device_s *dev, uint8_t *yy, uint8_t *uv)
{
#ifdef romapi_bflb_mjdec_set_dqt_from_zigzag
    romapi_bflb_mjdec_set_dqt_from_zigzag(dev, yy, uv);
#else
    uint8_t yy_tab[8][8];
    uint8_t uv_tab[8][8];
    uint8_t idx;

    if (yy) {
        for (uint8_t i = 0; i < 64; i++) {
            idx = q_zig_zag_reverse[i];
            yy_tab[idx / 8][idx % 8] = yy[i];
        }
    }
    if (uv) {
        for (uint8_t i = 0; i < 64; i++) {
            idx = q_zig_zag_reverse[i];
            uv_tab[idx / 8][idx % 8] = uv[i];
        }
    }
    bflb_mjdec_set_dqt_from_spec(dev, (yy ? yy_tab : NULL), (uv ? uv_tab : NULL));
#endif
}

uint8_t bflb_mjdec_set_dqt_from_header(struct bflb_device_s *dev, uint8_t *header)
{
#ifdef romapi_bflb_mjdec_set_dqt_from_header
    return romapi_bflb_mjdec_set_dqt_from_header(dev, header);
#else
    uint8_t *yy_tab = NULL;
    uint8_t *uv_tab = NULL;
    uint32_t len;
    uint8_t *p;
    uint64_t start_time;

    if (header == NULL) {
        return MJDEC_ERR_POINTER_NULL;
    }
    p = header;
    if (p[0] != 0xFF || p[1] != 0xD8) {
        /* not jpeg header */
        return MJDEC_ERR_HEADER_SOI;
    }
    p += 2;
    start_time = bflb_mtimer_get_time_ms();
    while (1) {
        if (p[0] != 0xFF) {
            /* JPEG header format error */
            return MJDEC_ERR_HEADER_MARKER;
        }
        p++;
        len = ((p[1] << 8) | p[2]);
        if (p[0] == 0xDB) {
            if (len == 0x43) {
                if (p[3] == 0) {
                    yy_tab = p + 4;
                } else if (p[3] == 1) {
                    uv_tab = p + 4;
                } else {
                    /* do not support this type of DQT */
                    return MJDEC_ERR_DQT_TYPE;
                }
            } else {
                /* do not support this length of DQT */
                return MJDEC_ERR_DQT_LENGTH;
            }
        } else if ((p[0] == 0xDA) || (p[0] == 0xD9) || (p[0] == 0xFF) || (p[0] == 0x00)) {
            /* start of scan or EOI or other invalid marker */
            break;
        }
        p = p + len + 1;
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return MJDEC_ERR_HEADER_TIMEOUT;
        }
    }
    bflb_mjdec_set_dqt_from_zigzag(dev, yy_tab, uv_tab);
    return MJDEC_OK;
#endif
}

void bflb_mjdec_set_dqt_from_quality(struct bflb_device_s *dev, uint8_t quality)
{
#ifdef romapi_bflb_mjdec_set_dqt_from_quality
    romapi_bflb_mjdec_set_dqt_from_quality(dev, quality);
#else
    uint8_t yy_tab[8][8];
    uint8_t uv_tab[8][8];
    uint32_t scale_factor;
    uint32_t val;

    if (quality == 0) {
        quality = 1;
    } else if (quality > 100) {
        quality = 100;
    }

    if (quality < 50) {
        scale_factor = 5000000 / quality;
    } else {
        scale_factor = 200000 - quality * 2000;
    }

    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            val = (q_table_50_yy[y][x] * scale_factor + 50000) / 100000;
            if (val == 0) {
                yy_tab[y][x] = 1;
            } else if (val > 0xFF) {
                yy_tab[y][x] = 0xFF;
            } else {
                yy_tab[y][x] = val;
            }
            val = (q_table_50_uv[y][x] * scale_factor + 50000) / 100000;
            if (val == 0) {
                uv_tab[y][x] = 1;
            } else if (val > 0xFF) {
                uv_tab[y][x] = 0xFF;
            } else {
                uv_tab[y][x] = val;
            }
        }
    }
    bflb_mjdec_set_dqt_from_spec(dev, yy_tab, uv_tab);
#endif
}

uint8_t bflb_mjdec_set_dht_from_spec(struct bflb_device_s *dev, struct bflb_mjdec_dht_s *dht)
{
#ifdef romapi_bflb_mjdec_set_dht_from_spec
    return romapi_bflb_mjdec_set_dht_from_spec(dev, dht);
#else
    uint32_t regval;
    uint32_t reg_base;
    uint32_t val;
    uint16_t i, j;
    uint8_t flag = 0;
    uint64_t start_time;

    reg_base = dev->reg_base;

    /* YY DC component */
    if (dht->yy_dc_cnt && dht->yy_dc_val) {
        if ((dht->yy_dc_bytes == 0) || (dht->yy_dc_bytes > 12)) {
            return MJDEC_ERR_DHT_YY_DC_BYTES;
        }
        /* configure YY DC component bit count */
        for (i = 0; i < 2; i++) {
            regval = 0;
            for (j = 0; j < 8; j++) {
                val = dht->yy_dc_cnt[i * 8 + j];
                if (val >= 16) {
                    return MJDEC_ERR_DHT_YY_DC_CNT;
                }
                regval |= (val << (4 * j));
            }
            putreg32(regval, reg_base + MJDEC_YY_DC_CFG0_OFFSET + i * 4);
        }
        /* configure YY DC component value */
        for (i = 0; i < dht->yy_dc_bytes / 8; i++) {
            regval = 0;
            for (j = 0; j < 8; j++) {
                val = dht->yy_dc_val[i * 8 + j];
                if (val >= 16) {
                    return MJDEC_ERR_DHT_YY_DC_VAL;
                }
                regval |= (val << (4 * j));
            }
            putreg32(regval, reg_base + MJDEC_YY_DC_CFG2_OFFSET + i * 4);
        }
        if (dht->yy_dc_bytes % 8) {
            regval = getreg32(reg_base + MJDEC_YY_DC_CFG2_OFFSET + i * 4);
            for (j = 0; j < dht->yy_dc_bytes % 8; j++) {
                val = dht->yy_dc_val[i * 8 + j];
                if (val >= 16) {
                    return MJDEC_ERR_DHT_YY_DC_VAL;
                }
                regval &= ~(0xF << (4 * j));
                regval |= (val << (4 * j));
            }
            putreg32(regval, reg_base + MJDEC_YY_DC_CFG2_OFFSET + i * 4);
        }
        flag |= (1 << 0);
    }
    /* YY AC component */
    if (dht->yy_ac_cnt && dht->yy_ac_val) {
        if ((dht->yy_ac_bytes == 0) || (dht->yy_ac_bytes > 0xA2)) {
            return MJDEC_ERR_DHT_YY_AC_BYTES;
        }
        /* configure YY AC component bit count */
        for (i = 0; i < 4; i++) {
            regval = 0;
            for (j = 0; j < 4; j++) {
                val = dht->yy_ac_cnt[i * 4 + j];
                regval |= (val << (8 * j));
            }
            putreg32(regval, reg_base + MJDEC_YY_AC_CFG0_OFFSET + i * 4);
        }
        /* configure YY AC component value */
        for (i = 0; i < dht->yy_ac_bytes / 4; i++) {
            regval = 0;
            for (j = 0; j < 4; j++) {
                val = dht->yy_ac_val[i * 4 + j];
                regval |= (val << (8 * j));
            }
            putreg32(regval, reg_base + MJDEC_YY_AC_CFG4_OFFSET + i * 4);
        }
        if (dht->yy_ac_bytes % 4) {
            regval = getreg32(reg_base + MJDEC_YY_AC_CFG4_OFFSET + i * 4);
            for (j = 0; j < dht->yy_ac_bytes % 4; j++) {
                val = dht->yy_ac_val[i * 4 + j];
                regval &= ~(0xFF << (8 * j));
                regval |= (val << (8 * j));
            }
            putreg32(regval, reg_base + MJDEC_YY_AC_CFG4_OFFSET + i * 4);
        }
        flag |= (1 << 1);
    }
    /* UV DC component */
    if (dht->uv_dc_cnt && dht->uv_dc_val) {
        if ((dht->uv_dc_bytes == 0) || (dht->uv_dc_bytes > 12)) {
            return MJDEC_ERR_DHT_UV_DC_BYTES;
        }
        /* configure UV DC component bit count */
        for (i = 0; i < 2; i++) {
            regval = 0;
            for (j = 0; j < 8; j++) {
                val = dht->uv_dc_cnt[i * 8 + j];
                if (val >= 16) {
                    return MJDEC_ERR_DHT_UV_DC_CNT;
                }
                regval |= (val << (4 * j));
            }
            putreg32(regval, reg_base + MJDEC_UV_DC_CFG0_OFFSET + i * 4);
        }
        /* configure uv DC component value */
        for (i = 0; i < dht->uv_dc_bytes / 8; i++) {
            regval = 0;
            for (j = 0; j < 8; j++) {
                val = dht->uv_dc_val[i * 8 + j];
                if (val >= 16) {
                    return MJDEC_ERR_DHT_UV_DC_VAL;
                }
                regval |= (val << (4 * j));
            }
            putreg32(regval, reg_base + MJDEC_UV_DC_CFG2_OFFSET + i * 4);
        }
        if (dht->uv_dc_bytes % 8) {
            regval = getreg32(reg_base + MJDEC_UV_DC_CFG2_OFFSET + i * 4);
            for (j = 0; j < dht->uv_dc_bytes % 8; j++) {
                val = dht->uv_dc_val[i * 8 + j];
                if (val >= 16) {
                    return MJDEC_ERR_DHT_UV_DC_VAL;
                }
                regval &= ~(0xF << (4 * j));
                regval |= (val << (4 * j));
            }
            putreg32(regval, reg_base + MJDEC_UV_DC_CFG2_OFFSET + i * 4);
        }
        flag |= (1 << 2);
    }
    /* UV AC component */
    if (dht->uv_ac_cnt && dht->uv_ac_val) {
        if ((dht->uv_ac_bytes == 0) || (dht->uv_ac_bytes > 0xA2)) {
            return MJDEC_ERR_DHT_UV_AC_BYTES;
        }
        /* configure UV AC component bit count */
        for (i = 0; i < 4; i++) {
            regval = 0;
            for (j = 0; j < 4; j++) {
                val = dht->uv_ac_cnt[i * 4 + j];
                regval |= (val << (8 * j));
            }
            putreg32(regval, reg_base + MJDEC_UV_AC_CFG0_OFFSET + i * 4);
        }
        /* configure UV AC component value */
        for (i = 0; i < dht->uv_ac_bytes / 4; i++) {
            regval = 0;
            for (j = 0; j < 4; j++) {
                val = dht->uv_ac_val[i * 4 + j];
                regval |= (val << (8 * j));
            }
            putreg32(regval, reg_base + MJDEC_UV_AC_CFG4_OFFSET + i * 4);
        }
        if (dht->uv_ac_bytes % 4) {
            regval = getreg32(reg_base + MJDEC_UV_AC_CFG4_OFFSET + i * 4);
            for (j = 0; j < dht->uv_ac_bytes % 4; j++) {
                val = dht->uv_ac_val[i * 4 + j];
                regval &= ~(0xFF << (8 * j));
                regval |= (val << (8 * j));
            }
            putreg32(regval, reg_base + MJDEC_UV_AC_CFG4_OFFSET + i * 4);
        }
        flag |= (1 << 3);
    }
    /* check validity of DHT */
    if ((flag != 0x3) && (flag != 0xF)) {
        return MJDEC_ERR_DHT_TYPE;
    }
    /* trigger DHT calc */
    putreg32(MJDEC_REG_HUF_CAL_PULSE, reg_base + MJDEC_JDEC_INT_CLR_OFFSET);
    start_time = bflb_mtimer_get_time_ms();
    while (1) {
        regval = getreg32(dev->reg_base + MJDEC_JDEC_INT_CLR_OFFSET);
        regval &= MJDEC_STS_HUF_CAL_DONE;
        if (regval) {
            break;
        }
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return MJDEC_ERR_DHT_TRIG;
        }
    }

    return MJDEC_OK;
#endif
}

uint8_t bflb_mjdec_set_dht_from_header(struct bflb_device_s *dev, uint8_t *header)
{
#ifdef romapi_bflb_mjdec_set_dht_from_header
    return romapi_bflb_mjdec_set_dht_from_header(dev, header);
#else
    struct bflb_mjdec_dht_s dht;
    uint32_t len;
    uint8_t *p;
    uint64_t start_time;

    if (header == NULL) {
        return MJDEC_ERR_POINTER_NULL;
    }
    dht.yy_dc_cnt = NULL;
    dht.yy_dc_val = NULL;
    dht.yy_ac_cnt = NULL;
    dht.yy_ac_val = NULL;
    dht.uv_dc_cnt = NULL;
    dht.uv_dc_val = NULL;
    dht.uv_ac_cnt = NULL;
    dht.uv_ac_val = NULL;
    dht.yy_dc_bytes = 0;
    dht.yy_ac_bytes = 0; 
    dht.uv_dc_bytes = 0;
    dht.uv_ac_bytes = 0;
    p = header;
    if (p[0] != 0xFF || p[1] != 0xD8) {
        /* not jpeg header */
        return MJDEC_ERR_HEADER_SOI;
    }
    p += 2;
    start_time = bflb_mtimer_get_time_ms();
    while (1) {
        if (p[0] != 0xFF) {
            /* JPEG header format error */
            return MJDEC_ERR_HEADER_MARKER;
        }
        p++;
        len = ((p[1] << 8) | p[2]);
        if (p[0] == 0xC4) {
            if (p[3] == 0x00) {
                dht.yy_dc_cnt = p + 4;
                dht.yy_dc_val = dht.yy_dc_cnt + 16;
                dht.yy_dc_bytes = len - 3 - 16;
            } else if (p[3] == 0x10) {
                dht.yy_ac_cnt = p + 4;
                dht.yy_ac_val = dht.yy_ac_cnt + 16;
                dht.yy_ac_bytes = len - 3 - 16;
            } else if (p[3] == 0x01) {
                dht.uv_dc_cnt = p + 4;
                dht.uv_dc_val = dht.uv_dc_cnt + 16;
                dht.uv_dc_bytes = len - 3 - 16;
            } else if (p[3] == 0x11) {
                dht.uv_ac_cnt = p + 4;
                dht.uv_ac_val = dht.uv_ac_cnt + 16;
                dht.uv_ac_bytes = len - 3 - 16;
            } else {
                return MJDEC_ERR_DHT_TYPE;
            }
        } else if ((p[0] == 0xDA) || (p[0] == 0xD9) || (p[0] == 0xFF) || (p[0] == 0x00)) {
            /* start of scan or EOI or other invalid marker */
            break;
        }
        p = p + len + 1;
        if ((bflb_mtimer_get_time_ms() - start_time) > 100) {
            return MJDEC_ERR_HEADER_TIMEOUT;
        }
    }
    return bflb_mjdec_set_dht_from_spec(dev, &dht);
#endif
}

#endif

#if defined(BL808) || defined(BL606P)
void bflb_mjdec_set_dqt_from_quality(struct bflb_device_s *dev, uint8_t quality)
{
    LHAL_PARAM_ASSERT(IS_MJDEC_QUALITY(quality));

#ifdef romapi_bflb_mjdec_set_dqt_from_quality
    romapi_bflb_mjdec_set_dqt_from_quality(dev, quality);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
    regval &= ~MJDEC_REG_Q_MODE_MASK;
    regval |= (quality << MJDEC_REG_Q_MODE_SHIFT);
    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
#endif
}
#endif

void bflb_mjdec_init(struct bflb_device_s *dev, const struct bflb_mjdec_config_s *config)
{
    LHAL_PARAM_ASSERT(dev);
    LHAL_PARAM_ASSERT(IS_MJDEC_FORMAT(config->format));
    LHAL_PARAM_ASSERT(IS_MJDEC_RESOLUTION(config->resolution_x));
    LHAL_PARAM_ASSERT(IS_MJDEC_RESOLUTION(config->resolution_y));
    LHAL_PARAM_ASSERT(IS_MJDEC_ADDR(config->output_bufaddr0));
    LHAL_PARAM_ASSERT(IS_MJDEC_ADDR(config->output_bufaddr1));

#ifdef romapi_bflb_mjdec_init
    romapi_bflb_mjdec_init(dev, config);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
    regval &= ~MJDEC_REG_MJ_DEC_ENABLE;
    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);

    regval = 0;
    regval |= (3 << MJDEC_REG_R_XLEN_SHIFT); /* AXI read burst count 16 increment */
    regval |= (3 << MJDEC_REG_W_XLEN_SHIFT); /* AXI write burst count 16 increment */
    if (config->swap_enable) {
        regval |= MJDEC_REG_SWAP_MODE;
    } else {
        regval &= ~MJDEC_REG_SWAP_MODE;
    }
    switch (config->format) {
        case MJDEC_FORMAT_YUV422SP_NV16:
            regval |= (2 << MJDEC_REG_YUV_MODE_SHIFT);
            regval |= MJDEC_REG_U_EVEN;
            bflb_mjdec_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            if (config->resolution_x % 16) {
                regval |= MJDEC_REG_LAST_HF_WBLK_DMY;
            }
            break;
        case MJDEC_FORMAT_YUV422SP_NV61:
            regval |= (2 << MJDEC_REG_YUV_MODE_SHIFT);
            regval &= ~MJDEC_REG_U_EVEN;
            bflb_mjdec_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 7) >> 3);
            if (config->resolution_x % 16) {
                regval |= MJDEC_REG_LAST_HF_WBLK_DMY;
            }
            break;
        case MJDEC_FORMAT_YUV420SP_NV12:
            regval |= (0 << MJDEC_REG_YUV_MODE_SHIFT);
            regval |= MJDEC_REG_U_EVEN;
            bflb_mjdec_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 15) >> 4);
            if (config->resolution_x % 16) {
                regval |= MJDEC_REG_LAST_HF_WBLK_DMY;
            }
            if (config->resolution_y % 16) {
                regval |= MJDEC_REG_LAST_HF_HBLK_DMY;
            }
            break;
        case MJDEC_FORMAT_YUV420SP_NV21:
            regval |= (0 << MJDEC_REG_YUV_MODE_SHIFT);
            regval &= ~MJDEC_REG_U_EVEN;
            bflb_mjdec_set_framesize(dev, (config->resolution_x + 15) >> 4, (config->resolution_y + 15) >> 4);
            if (config->resolution_x % 16) {
                regval |= MJDEC_REG_LAST_HF_WBLK_DMY;
            }
            if (config->resolution_y % 16) {
                regval |= MJDEC_REG_LAST_HF_HBLK_DMY;
            }
            break;
        case MJDEC_FORMAT_GRAY:
            regval |= (1 << MJDEC_REG_YUV_MODE_SHIFT);
            bflb_mjdec_set_framesize(dev, (config->resolution_x + 7) >> 3, (config->resolution_y + 7) >> 3);
            break;
    default:
        break;
    }
    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);

    putreg32(config->output_bufaddr0, reg_base + MJDEC_JDEC_YY_FRAME_ADDR_OFFSET);
    putreg32(config->output_bufaddr1, reg_base + MJDEC_JDEC_UV_FRAME_ADDR_OFFSET);

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_3_OFFSET);
    regval &= ~MJDEC_REG_INT_NORMAL_EN;
    regval &= ~MJDEC_REG_INT_IDLE_EN;
    regval &= ~MJDEC_REG_FRAME_CNT_TRGR_INT_MASK;
    regval |= (1 << MJDEC_REG_FRAME_CNT_TRGR_INT_SHIFT); /* trigger int with one frame */
    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_3_OFFSET);

    regval = getreg32(reg_base + MJDEC_JDEC_HEADER_SKIP_OFFSET);
    regval &= ~MJDEC_REG_HDER_SKIP_BYTE_MASK;
    if (config->head_size == 0) {
        regval &= ~MJDEC_REG_HDER_SKIP;
    } else {
        regval |= MJDEC_REG_HDER_SKIP;
        regval |= (config->head_size << MJDEC_REG_HDER_SKIP_BYTE_SHIFT);
    }
    putreg32(regval, reg_base + MJDEC_JDEC_HEADER_SKIP_OFFSET);

    /* Clear interrupt */
    putreg32(MJDEC_REG_INT_CLR, reg_base + MJDEC_JDEC_INT_CLR_OFFSET);
#endif
}

void bflb_mjdec_start(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_start
    romapi_bflb_mjdec_start(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
    regval |= MJDEC_REG_MJ_DEC_ENABLE;
    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
#endif
}

void bflb_mjdec_stop(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_stop
    romapi_bflb_mjdec_stop(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
    regval &= ~MJDEC_REG_MJ_DEC_ENABLE;
    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_1_OFFSET);
#endif
}

void bflb_mjdec_push_jpeg(struct bflb_device_s *dev, void *frame)
{
    LHAL_PARAM_ASSERT(IS_MJDEC_ADDR(frame));

#ifdef romapi_bflb_mjdec_push_frame
    romapi_bbflb_mjdec_push_frame(dev, frame);
#else
    putreg32((uint32_t)frame | MJDEC_REG_JP_PUSH, dev->reg_base + MJDEC_JDEC_FRAM_PUSH_OFFSET);
#endif
}

void bflb_mjdec_tcint_mask(struct bflb_device_s *dev, bool mask)
{
#ifdef romapi_bflb_mjdec_tcint_mask
    romapi_bflb_mjdec_tcint_mask(dev, mask);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_3_OFFSET);

    if (mask) {
        regval &= ~MJDEC_REG_INT_NORMAL_EN;
    } else {
        regval |= MJDEC_REG_INT_NORMAL_EN;
    }

    putreg32(regval, reg_base + MJDEC_JDEC_CONTROL_3_OFFSET);
#endif
}

uint32_t bflb_mjdec_get_intstatus(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_get_intstatus
    return romapi_bflb_mjdec_get_intstatus(dev);
#else
    uint32_t regval;
    uint32_t reg_base;

    reg_base = dev->reg_base;

    regval = getreg32(reg_base + MJDEC_JDEC_CONTROL_3_OFFSET);
    regval &= MJDEC_INTSTS_ONE_FRAME;

    return regval;
#endif
}

void bflb_mjdec_int_clear(struct bflb_device_s *dev, uint32_t int_clear)
{
#ifdef romapi_bflb_mjdec_int_clear
    romapi_bflb_mjdec_int_clear(dev, int_clear);
#else
    uint32_t reg_base;
    reg_base = dev->reg_base;

    putreg32(int_clear, reg_base + MJDEC_JDEC_INT_CLR_OFFSET);
#endif
}

uint8_t bflb_mjdec_get_frame_count(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_get_frame_count
    return romapi_bflb_mjdec_get_frame_count(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    return ((getreg32(reg_base + MJDEC_JDEC_CONTROL_3_OFFSET) & MJDEC_FRAME_VALID_CNT_MASK) >> MJDEC_FRAME_VALID_CNT_SHIFT);
#endif
}

void bflb_mjdec_pop_one_frame(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_mjdec_pop_one_frame
    romapi_bflb_mjdec_pop_one_frame(dev);
#else
    uint32_t reg_base;

    reg_base = dev->reg_base;

    putreg32(MJDEC_REG_YUV_FRAME_POP, reg_base + MJDEC_JDEC_INT_CLR_OFFSET);
#endif
}

int bflb_mjdec_feature_control(struct bflb_device_s *dev, int cmd, size_t arg)
{
#ifdef romapi_bflb_mjdec_feature_control
    return romapi_bflb_mjdec_feature_control(dev, cmd, arg);
#else
    int ret = 0;
    __UNUSED uint32_t regval;
    __UNUSED uint32_t reg_base;

    reg_base = dev->reg_base;

    switch (cmd) {
        default:
            ret = -EPERM;
            break;
    }

    return ret;
#endif
}
