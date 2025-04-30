#include "bflb_common.h"
#include "bflb_core.h"

#include "bflb_ef_ctrl.h"
#include "bflb_flash.h"

#define BFLB_COMMON_UINT32_BIT_LEN  (32)
#define BFLB_COMMON_UINT64_BIT_LEN  (64)
#define BFLB_COMMON_UINT96_BIT_LEN  (96)
#define BFLB_COMMON_UINT128_BIT_LEN (128)

#if defined(BL616)
#define ANTI_ROLLBACK_ENABLE_OFFSET        (0x7C)
#define ANTI_ROLLBACK_ENABLE_MASK          (0x01)
#define ANTI_ROLLBACK_ENABLE_POS           (12)
#define ANTI_ROLLBACK_BOOT2_VERSION_OFFSET (0x170)
#define ANTI_ROLLBACK_APP_VERSION_OFFSET   (0x180)
#elif defined(BL702L)
#define ANTI_ROLLBACK_ENABLE_OFFSET        (0x74)
#define ANTI_ROLLBACK_ENABLE_MASK          (0x01)
#define ANTI_ROLLBACK_ENABLE_POS           (13)
#define ANTI_ROLLBACK_BOOT2_VERSION_OFFSET (0x4C)
#define ANTI_ROLLBACK_APP_VERSION_OFFSET   (0x04)
#endif

#if defined(BL702L)
#define BFLB_BOOT2_IMG_OFFSET 4 * 1024
#else
#define BFLB_BOOT2_IMG_OFFSET 8 * 1024
#endif

#if defined(BL616) || defined(BL702L)
#if !defined(CONFIG_BOOT2)
#define COMPILE_TIME __DATE__ " " __TIME__
static const char ver_name[4] __attribute__ ((section(".verinfo"))) = "app";
static const char git_commit[41] __attribute__ ((section(".verinfo"))) = "";
static const char time_info[30] __attribute__ ((section(".verinfo"))) = COMPILE_TIME;

const blverinf_t app_ver __attribute__ ((section(".blverinf"))) = {
    .anti_rollback = CONFIG_APP_ANTI_ROLLBACK_VER,
    .x = APP_VER_X,
    .y = APP_VER_Y,
    .z = APP_VER_Z,
    .name = (uint32_t)ver_name,
    .build_time = (uint32_t)time_info,
    .commit_id = (uint32_t)git_commit,
    .rsvd0 = 0,
    .rsvd1 = 0,
};
#endif
#endif

__WEAK int ATTR_TCM_SECTION arch_strcmp(const char *str1, const char *str2) {
#ifdef romapi_arch_strcmp
    return romapi_arch_strcmp(str1, str2);
#else
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
#endif
}

__WEAK void *ATTR_TCM_SECTION arch_memcpy(void *dst, const void *src, uint32_t n)
{
#ifdef romapi_arch_memcpy
    return romapi_arch_memcpy(dst, src, n);
#else
    const uint8_t *p = src;
    uint8_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
#endif
}

__WEAK uint32_t *ATTR_TCM_SECTION arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n)
{
#ifdef romapi_arch_memcpy4
    return romapi_arch_memcpy4(dst, src, n);
#else
    const uint32_t *p = src;
    uint32_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
#endif
}

__WEAK void *ATTR_TCM_SECTION arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n)
{
#ifdef romapi_arch_memcpy_fast
    return romapi_arch_memcpy_fast(pdst, psrc, n);
#else
    uint32_t left, done, i = 0;
    uint8_t *dst = (uint8_t *)pdst;
    uint8_t *src = (uint8_t *)psrc;

    if (((uint32_t)(uintptr_t)dst & 0x3) == 0 && ((uint32_t)(uintptr_t)src & 0x3) == 0) {
        arch_memcpy4((uint32_t *)dst, (const uint32_t *)src, n >> 2);
        left = n % 4;
        done = n - left;

        while (i < left) {
            dst[done + i] = src[done + i];
            i++;
        }
    } else {
        arch_memcpy(dst, src, n);
    }

    return dst;
#endif
}

__WEAK void *ATTR_TCM_SECTION arch_memset(void *s, uint8_t c, uint32_t n)
{
#ifdef romapi_arch_memset
    return romapi_arch_memset(s, c, n);
#else
    uint8_t *p = (uint8_t *)s;

    while (n > 0) {
        *p++ = (uint8_t)c;
        --n;
    }

    return s;
#endif
}

__WEAK uint32_t *ATTR_TCM_SECTION arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n)
{
#ifdef romapi_arch_memset4
    return romapi_arch_memset4(dst, val, n);
#else
    uint32_t *q = dst;

    while (n--) {
        *q++ = val;
    }

    return dst;
#endif
}

__WEAK int ATTR_TCM_SECTION arch_memcmp(const void *s1, const void *s2, uint32_t n)
{
#ifdef romapi_arch_memcmp
    return romapi_arch_memcmp(s1, s2, n);
#else
    const unsigned char *c1 = s1, *c2 = s2;
    int d = 0;

    while (n--) {
        d = (int)*c1++ - (int)*c2++;

        if (d) {
            break;
        }
    }

    return d;
#endif
}

#if defined(BL616)
bool bflb_check_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if ((a & 0xFFFF0000UL) >= 0x62FC0000UL) {
        return true;
    }
    return false;
}

void *bflb_get_no_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if (!bflb_check_cache_addr(addr)) {
        return (void *)addr;
    }

    // OCRAM
    if ((a & 0xF0000000UL) == 0x60000000UL) {
        return (void *)((a & ~0xF0000000UL) | 0x20000000UL);
    }
    // pSRAM
    if ((a & 0xF0000000UL) == 0xA0000000UL) {
        return (void *)((a & ~0xF0000000UL) | 0x10000000UL);
    }

    return NULL;
}
#elif defined(BL808) && defined(CPU_M0)
bool bflb_check_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if ((a & 0xFFFF0000UL) >= 0x62020000UL) {
        return true;
    }
    return false;
}
void *bflb_get_no_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if (!bflb_check_cache_addr(addr)) {
        return (void *)addr;
    }

    // OCRAM
    if ((a & 0xF0000000UL) == 0x60000000UL) {
        return (void *)((a & ~0xF0000000UL) | 0x20000000UL);
    }

    return NULL;
}
#endif

#if (defined(BL616) || defined(BL808)) && !defined(CPU_LP)
bool bflb_check_cache_addr_aligned(uintptr_t addr)
{
    if (addr & (BFLB_CACHE_LINE_SIZE - 1)) {
        return false;
    } else {
        return true;
    }
}
#endif

// ---------------- POPULAR POLYNOMIALS ----------------
// CCITT:      x^16 + x^12 + x^5 + x^0                 (0x1021,init 0x0000)
// CRC-16:     x^16 + x^15 + x^2 + x^0                 (0x8005,init 0xFFFF)
// we use 0x8005 here and

const uint8_t chCRCHTalbe[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

const uint8_t chCRCLTalbe[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

uint16_t bflb_soft_crc16(void *in, uint32_t len)
{
#ifdef romapi_bflb_soft_crc16
    return romapi_bflb_soft_crc16(in, len);
#else
    uint8_t chCRCHi = 0xFF;
    uint8_t chCRCLo = 0xFF;
    uint16_t wIndex;
    uint8_t *data = (uint8_t *)in;

    while (len--) {
        wIndex = chCRCLo ^ *data++;
        chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
        chCRCHi = chCRCLTalbe[wIndex];
    }

    return ((chCRCHi << 8) | chCRCLo);
#endif
}

/******************************************************************************
* Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
* Poly:    0x4C11DB7
* Init:    0xFFFFFFF
* Refin:   True
* Refout:  True
* Xorout:  0xFFFFFFF
* Alias:   CRC_32/ADCCP
* Use:     WinRAR,ect.
*****************************************************************************/
uint32_t ATTR_TCM_SECTION bflb_soft_crc32_ex(uint32_t initial, void *in, uint32_t len)
{
#ifdef romapi_bflb_soft_crc32_ex
    return romapi_bflb_soft_crc32_ex(initial, in, len);
#else
    uint8_t i;
    uint32_t crc = ~initial; // Initial value
    uint8_t *data = (uint8_t *)in;

    while (len--) {
        crc ^= *data++; // crc ^= *data; data++;
        for (i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320; // 0xEDB88320= reverse 0x04C11DB7
            } else {
                crc = (crc >> 1);
            }
        }
    }
    return ~crc;
#endif
}

__WEAK uint32_t ATTR_TCM_SECTION bflb_soft_crc32(void *in, uint32_t len)
{
#ifdef romapi_bflb_soft_crc32
    return romapi_bflb_soft_crc32(in, len);
#else
    return bflb_soft_crc32_ex(0, in, len);
#endif
}

__WEAK int32_t bflb_get_app_version_from_efuse(uint8_t *version)
{
#ifdef romapi_bflb_get_app_version_from_efuse
    return romapi_bflb_get_app_version_from_efuse(version);
#else
#if defined(BL616) || defined(BL702L)
    uint32_t otp_ef_boot2_anti_rollback_en = 0;
    uint32_t tmpVal;

    if (NULL == version) {
        return 1;
    }

    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_ENABLE_OFFSET, &tmpVal, 1, 1);
    /* get anti-rollback enable bit */
    otp_ef_boot2_anti_rollback_en = ((tmpVal >> ANTI_ROLLBACK_ENABLE_POS) & ANTI_ROLLBACK_ENABLE_MASK);

    if (0 == otp_ef_boot2_anti_rollback_en) {
        return 1;
    }

#if defined(BL616)
    uint32_t version_low_low = 0;
    uint32_t version_low = 0;
    uint32_t version_high = 0;
    uint32_t version_high_high = 0;
#if defined(BL616)
    uint32_t value[8];
#endif
    /* read efuse value */
    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_APP_VERSION_OFFSET, value, sizeof(value) / 4, 1);
#if defined(BL616)
    /* get real version from efuse */
    version_low_low = value[0] | value[4];
    version_low = value[1] | value[5];
    version_high = value[2] | value[6];
    version_high_high = value[3] | value[7];
#endif

    /* version_real[127:96] case */
    if (version_high_high) {
        *version = BFLB_COMMON_UINT128_BIT_LEN - __builtin_clz(version_high_high);
        return 0;
    }

    /* version_real[95:64] case */
    if (version_high) {
        *version = BFLB_COMMON_UINT96_BIT_LEN - __builtin_clz(version_high);
        return 0;
    }

    /* version_real[63:32] case */
    if (version_low) {
        *version = BFLB_COMMON_UINT64_BIT_LEN - __builtin_clz(version_low);
        return 0;
    }

    /* version_real[31:0] case */
    if (version_low_low) {
        *version = BFLB_COMMON_UINT32_BIT_LEN - __builtin_clz(version_low_low);
        return 0;
    }
#elif defined(BL702L)
    uint32_t value[2];
    uint32_t version_low = 0;
    uint32_t version_high = 0;

    /* read efuse value */
    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_APP_VERSION_OFFSET, value, sizeof(value) / 4, 1);

    /* get real version from efuse */
    version_low = value[0];
    version_high = value[1];

    /* version_real[63:32] case */
    if (version_high) {
        *version = BFLB_COMMON_UINT64_BIT_LEN - __builtin_clz(version_high);
        return 0;
    }

    /* version_real[31:0] case */
    if (version_low) {
        *version = BFLB_COMMON_UINT32_BIT_LEN - __builtin_clz(version_low);
        return 0;
    }
#endif
    *version = 0;
    return 0;
#else
    return 1;
#endif
#endif
}

__WEAK int32_t bflb_set_app_version_to_efuse(uint8_t version)
{
#ifdef romapi_bflb_set_app_version_to_efuse
    return romapi_bflb_set_app_version_to_efuse(version);
#else
#if defined(BL616) || defined(BL702L)
    uint8_t version_old;
    uint32_t tmpVal;

    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_ENABLE_OFFSET, &tmpVal, 1, 1);
    /* Set anti-rollback enable bit */
    tmpVal |= (1 << ANTI_ROLLBACK_ENABLE_POS);
    bflb_ef_ctrl_write_direct(NULL, ANTI_ROLLBACK_ENABLE_OFFSET, &tmpVal, 0x1, 1);

    if (bflb_get_app_version_from_efuse(&version_old) != 0) {
        return 1;
    }

    if (version_old > version) {
        return 1;
    } else if (version_old == version) {
        return 0;
    }

#if defined(BL616)
    uint32_t version_low_low = 0;
    uint32_t version_low = 0;
    uint32_t version_high = 0;
    uint32_t version_high_high = 0;
#if defined(BL616)
    uint32_t value[8];
#endif

    if (version <= BFLB_COMMON_UINT32_BIT_LEN) {
        version_low_low = (1 << (version - 1));
    } else if (version <= BFLB_COMMON_UINT64_BIT_LEN) {
        version_low = (1 << (version - BFLB_COMMON_UINT32_BIT_LEN - 1));
    } else if (version <= BFLB_COMMON_UINT96_BIT_LEN) {
        version_high = (1 << (version - BFLB_COMMON_UINT64_BIT_LEN - 1));
    } else if (version <= BFLB_COMMON_UINT128_BIT_LEN) {
        version_high_high = (1 << (version - BFLB_COMMON_UINT96_BIT_LEN - 1));
    }
#if defined(BL616)
    value[0] = value[4] = version_low_low;
    value[1] = value[5] = version_low;
    value[2] = value[6] = version_high;
    value[3] = value[7] = version_high_high;
#endif
    /* write efuse value */
    bflb_ef_ctrl_write_direct(NULL, ANTI_ROLLBACK_APP_VERSION_OFFSET, value, sizeof(value) / 4, 1);
#elif defined(BL702L)
    uint32_t value[2];
    uint32_t version_low = 0;
    uint32_t version_high = 0;

    if (version > BFLB_COMMON_UINT64_BIT_LEN) {
        return 1;
    }

    if (version > BFLB_COMMON_UINT32_BIT_LEN) {
        version_high = (1 << (version - BFLB_COMMON_UINT32_BIT_LEN - 1));
    } else {
        version_low = (1 << (version - 1));
    }

    value[0] = version_low;
    value[1] = version_high;

    /* write efuse value */
    bflb_ef_ctrl_write_direct(NULL, ANTI_ROLLBACK_APP_VERSION_OFFSET, value, sizeof(value) / 4, 1);
#endif
    return 0;
#else
    return 1;
#endif
#endif
}

__WEAK int32_t bflb_get_boot2_version_from_efuse(uint8_t *version)
{
#ifdef romapi_bflb_get_boot2_version_from_efuse
    return romapi_bflb_get_boot2_version_from_efuse(version);
#else
#if defined(BL616) || defined(BL702L)
    uint32_t otp_ef_boot2_anti_rollback_en = 0;
    uint32_t tmpVal;

    if (NULL == version) {
        return 1;
    }

    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_ENABLE_OFFSET, &tmpVal, 1, 1);
    /* get anti-rollback enable bit */
    otp_ef_boot2_anti_rollback_en = ((tmpVal >> ANTI_ROLLBACK_ENABLE_POS) & ANTI_ROLLBACK_ENABLE_MASK);

    if (0 == otp_ef_boot2_anti_rollback_en) {
        return 1;
    }

#if defined(BL616)
    uint32_t version_high = 0;
    uint32_t version_low = 0;
#if defined(BL616)
    uint32_t value[4];
#endif

    /* read efuse value */
    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_BOOT2_VERSION_OFFSET, value, sizeof(value) / 4, 1);
#if defined(BL616)
    /* get real version from efuse */
    version_low = value[0] | value[2];
    version_high = value[1] | value[3];
#endif

    /* version_real[63:32] case */
    if (version_high) {
        *version = BFLB_COMMON_UINT64_BIT_LEN - __builtin_clz(version_high);
        return 0;
    }

    /* version_real[31:0] case */
    if (version_low) {
        *version = BFLB_COMMON_UINT32_BIT_LEN - __builtin_clz(version_low);
        return 0;
    }

#elif defined(BL702L)
    uint32_t value[1];

    /* read efuse value */
    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_BOOT2_VERSION_OFFSET, value, sizeof(value) / 4, 1);

    /* version_real[31:0] case */
    if (value[0]) {
        *version = BFLB_COMMON_UINT32_BIT_LEN - __builtin_clz(value[0]);
        return 0;
    }
#endif
    *version = 0;
    return 0;
#else
    return 1;
#endif
#endif
}

__WEAK int32_t bflb_set_boot2_version_to_efuse(uint8_t version)
{
#ifdef romapi_bflb_set_boot2_version_to_efuse
    return romapi_bflb_set_boot2_version_to_efuse(version);
#else
#if defined(BL616) || defined(BL702L)
    uint8_t version_old;
    uint32_t tmpVal;

    bflb_ef_ctrl_read_direct(NULL, ANTI_ROLLBACK_ENABLE_OFFSET, &tmpVal, 1, 1);
    /* Set anti-rollback enable bit */
    tmpVal |= (1 << ANTI_ROLLBACK_ENABLE_POS);
    bflb_ef_ctrl_write_direct(NULL, ANTI_ROLLBACK_ENABLE_OFFSET, &tmpVal, 0x1, 1);

    if (bflb_get_boot2_version_from_efuse(&version_old) != 0) {
        return 1;
    }

    if (version_old > version) {
        return 1;
    } else if (version_old == version) {
        return 0;
    }

#if defined(BL616)
    uint32_t version_high = 0;
    uint32_t version_low = 0;
#if defined(BL616)
    uint32_t value[4];
#endif

    if (version <= BFLB_COMMON_UINT32_BIT_LEN) {
        version_low = (1 << (version - 1));
    } else if (version <= BFLB_COMMON_UINT64_BIT_LEN) {
        version_high = (1 << (version - BFLB_COMMON_UINT32_BIT_LEN - 1));
    }
#if defined(BL616)
    value[0] = value[2] = version_low;
    value[1] = value[3] = version_high;
#endif
    /* write efuse value */
    bflb_ef_ctrl_write_direct(NULL, ANTI_ROLLBACK_BOOT2_VERSION_OFFSET, value, sizeof(value) / 4, 1);

#elif defined(BL702L)
    uint32_t value[1];

    if (version > BFLB_COMMON_UINT32_BIT_LEN) {
        return 1;
    }

    value[0] = (1 << (version - 1));

    /* write efuse value */
    bflb_ef_ctrl_write_direct(NULL, ANTI_ROLLBACK_BOOT2_VERSION_OFFSET, value, sizeof(value) / 4, 1);
#endif
    return 0;
#else
    return 1;
#endif
#endif
}

__WEAK int32_t bflb_get_boot2_info_from_flash(blverinf_t *version)
{
    uint32_t addr = 0xC08;
    bflb_flash_read(addr + BFLB_BOOT2_IMG_OFFSET, (uint8_t *)version, sizeof(blverinf_t));

    return 0;
}
