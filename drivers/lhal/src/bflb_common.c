#include "bflb_common.h"
#include "bflb_core.h"

void *ATTR_TCM_SECTION arch_memcpy(void *dst, const void *src, uint32_t n)
{
    const uint8_t *p = src;
    uint8_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
}

uint32_t *ATTR_TCM_SECTION arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n)
{
    const uint32_t *p = src;
    uint32_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
}

void *ATTR_TCM_SECTION arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n)
{
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
}

void *ATTR_TCM_SECTION arch_memset(void *s, uint8_t c, uint32_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n > 0) {
        *p++ = (uint8_t)c;
        --n;
    }

    return s;
}

uint32_t *ATTR_TCM_SECTION arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n)
{
    uint32_t *q = dst;

    while (n--) {
        *q++ = val;
    }

    return dst;
}

int ATTR_TCM_SECTION arch_memcmp(const void *s1, const void *s2, uint32_t n)
{
    const unsigned char *c1 = s1, *c2 = s2;
    int d = 0;

    while (n--) {
        d = (int)*c1++ - (int)*c2++;

        if (d) {
            break;
        }
    }

    return d;
}