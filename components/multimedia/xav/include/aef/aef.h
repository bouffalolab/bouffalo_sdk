/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AEF_H__
#define __AEF_H__

#include "aef/aef_cls.h"

__BEGIN_DECLS__

/**
 * @brief  regist aef ops
 * @param  [in] ops
 * @return 0/-1
 */
int aefx_ops_register(const struct aefx_ops *ops);

/**
 * @brief  alloc a audio effecter
 * @param  [in] rate      : audio freq
 * @param  [in] conf
 * @param  [in] conf_size
 * @param  [in] nsamples_max : max frame len
 * @return NULL on error
 */
aefx_t* aefx_new(uint32_t rate, uint8_t *conf, size_t conf_size, size_t nsamples_max);

/**
 * @brief  process samples
 * @param  [in] aef
 * @param  [in] in
 * @param  [in] out
 * @param  [in] nb_samples : samples per channel
 * @return 0/-1
 */
int aefx_process(aefx_t *aef, const int16_t *in, int16_t *out, size_t nb_samples);

/**
 * @brief  free the audio effecter
 * @param  [in] aef
 * @return 0/-1
 */
int aefx_free(aefx_t *aef);

__END_DECLS__

#endif /* __AEF_H__ */

