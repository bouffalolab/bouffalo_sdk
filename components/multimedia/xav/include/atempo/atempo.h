/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ATEMPO_H__
#define __ATEMPO_H__

#include "atempo/atempo_cls.h"

__BEGIN_DECLS__

/**
 * @brief  regist atempo ops
 * @param  [in] ops
 * @return 0/-1
 */
int atempo_ops_register(const struct atempo_ops *ops);

/**
 * @brief  alloc a audio atempoer
 * @param  [in] rate      : audio freq
 * @return NULL on error
 */
atempo_t* atempo_new(uint32_t rate);

/**
 * @brief  write samples to atempoer
 * @param  [in] atempo
 * @param  [in] in
 * @param  [in] isamples
 * @return 0/-1
 */
int atempo_write(atempo_t *atempo, const int16_t *in, size_t isamples);

/**
 * @brief  read samples from atempoer
 * @param  [in] atempo
 * @param  [in] out
 * @param  [in] osamples
 * @return num of read samples, -1 on error
 */
int atempo_read(atempo_t *atempo, int16_t *out, size_t osamples);

/**
 * @brief  set speed of the audio atempoer
 * @param  [in] atempo
 * @param  [in] speed
 * @return 0/-1
 */
int atempo_set_speed(atempo_t *atempo, float speed);

/**
 * @brief  flush the audio atempoer
 * @param  [in] atempo
 * @return 0/-1
 */
int atempo_flush(atempo_t *atempo);

/**
 * @brief  free the audio atempoer
 * @param  [in] atempo
 * @return 0/-1
 */
int atempo_free(atempo_t *atempo);

__END_DECLS__

#endif /* __ATEMPO_H__ */

