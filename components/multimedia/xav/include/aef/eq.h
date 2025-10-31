/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQ_H__
#define __EQ_H__

#include "aef/eq_cls.h"

__BEGIN_DECLS__

/**
 * @brief  regist eq ops
 * @param  [in] ops
 * @return 0/-1
 */
int eqx_ops_register(const struct eqx_ops *ops);

/**
 * @brief  alloc a equalizer
 * @param  [in] rate
 * @param  [in] eq_segments : eq filter segment number max
 * @return NULL on error
 */
eqx_t* eqx_new(uint32_t rate, uint8_t eq_segments);

/**
 * @brief  control whole eq filter enable/disable
 * @param  [in] eq
 * @param  [in] enable
 * @return 0/-1
 */
int eqx_set_enable(eqx_t *eq, uint8_t enable);

/**
 * @brief  set config param to the filter segment
 * @param  [in] eq
 * @param  [in] segid : id of the filter segment
 * @param  [in] param
 * @return 0/-1
 */
int eqx_set_param(eqx_t *eq, uint8_t segid, const eqfp_t *param);

/**
 * @brief  process samples
 * @param  [in] eq
 * @param  [in] in
 * @param  [in] out
 * @param  [in] nb_samples : samples per channel
 * @return 0/-1
 */
int eqx_process(eqx_t *eq, const int16_t *in, int16_t *out, size_t nb_samples);

/**
 * @brief  free the equalizer
 * @param  [in] eq
 * @return 0/-1
 */
int eqx_free(eqx_t *eq);


__END_DECLS__

#endif /* __EQ_H__ */

