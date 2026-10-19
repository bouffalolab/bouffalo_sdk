/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __EQ_ICORE_H__
#define __EQ_ICORE_H__

#include "aef/eq_cls.h"

__BEGIN_DECLS__

typedef struct eq_icore eq_icore_t;

/**
 * @brief  init the icore equalizer
 * @return 0/-1
 */
int eq_icore_init();

/**
 * @brief  alloc a equalizer
 * @param  [in] rate
 * @param  [in] eq_segments : eq filter segment number max
 * @return NULL on error
 */
eq_icore_t* eq_icore_new(uint32_t rate, uint8_t eq_segments);

/**
 * @brief  control whole eq filter enable/disable
 * @param  [in] hdl
 * @param  [in] enable
 * @return 0/-1
 */
int eq_icore_set_enable(eq_icore_t *hdl, uint8_t enable);

/**
 * @brief  set config param to the filter segment
 * @param  [in] hdl
 * @param  [in] segid : id of the filter segment
 * @param  [in] param
 * @return
 */
int eq_icore_set_param(eq_icore_t *hdl, uint8_t segid, const eqfp_t *param);

/**
 * @brief  process samples
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @param  [in] nb_samples : samples per channel
 * @return 0/-1
 */
int eq_icore_process(eq_icore_t *hdl, const int16_t *in, int16_t *out, size_t nb_samples);

/**
 * @brief  free the equalizer
 * @param  [in] hdl
 * @return 0/-1
 */
int eq_icore_free(eq_icore_t *hdl);

__END_DECLS__

#endif /* __EQ_ICORE_H__ */

