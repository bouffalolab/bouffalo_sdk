/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __PCM_CONVERT_H__
#define __PCM_CONVERT_H__

#include "avutil/common.h"
#include "avutil/sf.h"

__BEGIN_DECLS__

/**
 * @brief  s8_ch1_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch1_to_s16_ch2(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  s8_ch1_to_s16_ch1
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch1_to_s16_ch1(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  u8_ch1_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void u8_ch1_to_s16_ch2(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  s8_ch2_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch2_to_s16_ch2(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  s8_ch2_to_s16_ch1
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s8_ch2_to_s16_ch1(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  u8_ch2_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void u8_ch2_to_s16_ch2(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  s16_ch1_to_s16_ch2
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s16_ch1_to_s16_ch2(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  s16_ch2_to_s16_ch1
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] nb_samples : number of input samples available in one channel
 * @return
 */
void s16_ch2_to_s16_ch1(void *dst, const void *src, size_t nb_samples);

/**
 * @brief  u16_le_to_s16_le
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] size : space(src) / sizeof(int16_t)
 * @return
 */
void u16_le_to_s16_le(void *dst, const void *src, size_t size);

/**
 * @brief  u16_be_to_s16_le
 * @param  [in] dst
 * @param  [in] src
 * @param  [in] size : space(src) / sizeof(int16_t)
 * @return
 */
void u16_be_to_s16_le(void *dst, const void *src, size_t size);

__END_DECLS__

#endif /* __PCM_CONVERT_H__ */

