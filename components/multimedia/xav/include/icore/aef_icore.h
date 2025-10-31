/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AEF_ICORE_H__
#define __AEF_ICORE_H__

#include "aef/aef_cls.h"

__BEGIN_DECLS__

typedef struct aef_icore aef_icore_t;

/**
 * @brief  init inter-core audio effecter
 * @return 0/-1
 */
int aef_icore_init();

/**
 * @brief  alloc a inter-core audio effecter
 * @param  [in] rate      : audio freq
 * @param  [in] conf
 * @param  [in] conf_size
 * @param  [in] nsamples_max : max frame len
 * @return NULL on error
 */
aef_icore_t* aef_icore_new(uint32_t rate, uint8_t *conf, size_t conf_size, size_t nsamples_max);

/**
 * @brief  process samples
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @param  [in] nb_samples : samples per channel
 * @return 0/-1
 */
int aef_icore_process(aef_icore_t *hdl, const int16_t *in, int16_t *out, size_t nb_samples);

/**
 * @brief  free the inter-core audio effecter
 * @param  [in] hdl
 * @return 0/-1
 */
int aef_icore_free(aef_icore_t *hdl);

__END_DECLS__

#endif /* __AEF_ICORE_H__ */

