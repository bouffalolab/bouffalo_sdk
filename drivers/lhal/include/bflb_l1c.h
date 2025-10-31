#ifndef _BFLB_L1C_H
#define _BFLB_L1C_H

#include "stdint.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup L1C
  * @{
  */

#if (defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL606P) || defined(BL808) || defined(BL628)) && !defined(CPU_LP)
#if ((defined(BL808) || defined(BL606P)) && defined(CPU_D0))
#define BFLB_CACHE_LINE_SIZE 64
#else
#define BFLB_CACHE_LINE_SIZE 32
#endif
#endif

/**
 * @brief
 *
 */
void bflb_l1c_icache_enable(void);

/**
 * @brief
 *
 */
void bflb_l1c_icache_disable(void);

/**
 * @brief
 *
 */
void bflb_l1c_icache_invalid_all(void);

/**
 * @brief
 *
 * @param [in] addr
 * @param [in] size
 */
void bflb_l1c_icache_invalid_range(void *addr, uint32_t size);

/**
 * @brief
 *
 */
void bflb_l1c_dcache_enable(void);

/**
 * @brief
 *
 */
void bflb_l1c_dcache_disable(void);

/**
 * @brief
 *
 */
void bflb_l1c_dcache_clean_all(void);

/**
 * @brief
 *
 */
void bflb_l1c_dcache_invalidate_all(void);

/**
 * @brief
 *
 */
void bflb_l1c_dcache_clean_invalidate_all(void);

/**
 * @brief
 *
 * @param [in] addr
 * @param [in] size
 */
void bflb_l1c_dcache_clean_range(void *addr, uint32_t size);

/**
 * @brief
 *
 * @param [in] addr
 * @param [in] size
 */
void bflb_l1c_dcache_invalidate_range(void *addr, uint32_t size);

/**
 * @brief
 *
 * @param [in] addr
 * @param [in] size
 */
void bflb_l1c_dcache_clean_invalidate_range(void *addr, uint32_t size);

void bflb_l1c_hit_count_get(uint32_t *hit_count_low, uint32_t *hit_count_high);
uint32_t bflb_l1c_miss_count_get(void);
void bflb_l1c_cache_write_set(uint8_t wt_en, uint8_t wb_en, uint8_t wa_en);

/**
  * @}
  */

/**
  * @}
  */

#endif