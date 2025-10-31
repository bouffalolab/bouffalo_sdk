/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ATEMPO_ICORE_H__
#define __ATEMPO_ICORE_H__

#include "atempo/atempo_cls.h"

__BEGIN_DECLS__

typedef struct atempo_icore atempo_icore_t;

/**
 * @brief  init inter-core audio atempoer
 * @return 0/-1
 */
int atempo_icore_init();

/**
 * @brief  alloc a inter-core audio atempoer
 * @param  [in] rate      : audio freq
 * @return NULL on error
 */
atempo_icore_t* atempo_icore_new(uint32_t rate);

/**
 * @brief  write samples to the inter-core audio atempoer
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] nb_samples : samples per channel
 * @return 0/-1
 */
int atempo_icore_write(atempo_icore_t *hdl, const int16_t *in, size_t nb_samples);

/**
 * @brief  read samples from the inter-core audio atempoer
 * @param  [in] hdl
 * @param  [in] out
 * @param  [in] nb_samples : samples per channel
 * @return num of read samples, -1 on error
 */
int atempo_icore_read(atempo_icore_t *hdl, int16_t *out, size_t nb_samples);

/**
 * @brief  set_speed the inter-core audio atempoer
 * @param  [in] hdl
 * @param  [in] speed
 * @return 0/-1
 */
int atempo_icore_set_speed(atempo_icore_t *hdl, float speed);

/**
 * @brief  flush the inter-core audio atempoer
 * @param  [in] hdl
 * @return 0/-1
 */
int atempo_icore_flush(atempo_icore_t *hdl);

/**
 * @brief  free the inter-core audio atempoer
 * @param  [in] hdl
 * @return 0/-1
 */
int atempo_icore_free(atempo_icore_t *hdl);

__END_DECLS__

#endif /* __ATEMPO_ICORE_H__ */

