#ifndef _BFLB_COMMON_H
#define _BFLB_COMMON_H

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 *
 * @param [in] str1
 * @param [in] str2
 * @return int
 */
int arch_strcmp(const char *str1, const char *str2);

/**
 * @brief
 *
 * @param [in] dst
 * @param [in] src
 * @param [in] n
 * @return void*
 */
void *arch_memcpy(void *dst, const void *src, uint32_t n);

/**
 * @brief
 *
 * @param [in] s
 * @param [in] c
 * @param [in] n
 * @return void*
 */
void *arch_memset(void *s, uint8_t c, uint32_t n);

/**
 * @brief
 *
 * @param [in] s1
 * @param [in] s2
 * @param [in] n
 * @return int
 */
int arch_memcmp(const void *s1, const void *s2, uint32_t n);

/**
 * @brief
 *
 * @param [in] dst
 * @param [in] src
 * @param [in] n
 * @return uint32_t*
 */
uint32_t *arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n);

/**
 * @brief
 *
 * @param [in] pdst
 * @param [in] psrc
 * @param [in] n
 * @return void*
 */
void *arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n);

/**
 * @brief
 *
 * @param [in] dst
 * @param [in] val
 * @param [in] n
 * @return uint32_t*
 */
uint32_t *arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n);

/**
 * @brief
 *
 * @param [in] cnt
 */
void arch_delay_us(uint32_t cnt);

/**
 * @brief
 *
 * @param [in] cnt
 */
void arch_delay_ms(uint32_t cnt);

/**
 * @brief
 *
 * @param [in] in
 * @param [in] len
 * @return [in] uint16_t
 */
uint16_t bflb_soft_crc16(void *in, uint32_t len);

/**
 * @brief
 *
 * @param [in] initial
 * @param [in] in
 * @param [in] len
 * @return [in] uint32_t
 */
uint32_t  bflb_soft_crc32_ex(uint32_t initial, void *in, uint32_t len);

/**
 * @brief
 *
 * @param [in] in
 * @param [in] len
 * @return [in] uint32_t
 */
uint32_t bflb_soft_crc32(void *in, uint32_t len);

#if defined(BL602) || defined(BL702) || defined(BL702L)
#define bflb_check_cache_addr(addr)  (false)
#define bflb_get_no_cache_addr(addr) (addr)
#else
bool bflb_check_cache_addr(const void *addr);

void *bflb_get_no_cache_addr(const void *addr);

#endif

#if (defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL606P) || defined(BL808) || defined(BL628)) && !defined(CPU_LP)
bool bflb_check_cache_addr_aligned(uintptr_t addr);
#else
#define bflb_check_cache_addr_aligned(addr)  (true)
#endif

typedef struct
{
    uint8_t anti_rollback; /* anti-rollback version */
    uint8_t x;             /* custom version x */
    uint8_t y;             /* custom version y */
    uint8_t z;             /* custom version z */
    uint32_t name;         /* app name pointer */
    uint32_t build_time;   /* app build time pointer */
    uint32_t commit_id;    /* app build commit id pointer */
    uint32_t rsvd0;        /* rsvd0 */
    uint32_t rsvd1;        /* rsvd1 */
} bflb_verinf_t;

int32_t bflb_get_app_version_from_efuse(uint8_t *version);
int32_t bflb_set_app_version_to_efuse(uint8_t version);
int32_t bflb_get_boot2_version_from_efuse(uint8_t *version);
int32_t bflb_set_boot2_version_to_efuse(uint8_t version);
int32_t bflb_get_boot2_info_from_flash(bflb_verinf_t *version);

static inline __attribute__((always_inline)) void bflb_lhal_assert_func(const char *file, uint32_t line, const char *function, const char *string)
{
    //printf("Assertion failed:%s\r\nfunction:%s\r\nfile:%s\r\nline:%d\r\n", string, function, file, line);
    __asm__ volatile ("ebreak");
    while (1)
        ;
}

#ifdef __cplusplus
}
#endif

#endif
