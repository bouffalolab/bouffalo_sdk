/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __VOL_SCALE_H__
#define __VOL_SCALE_H__

#include "avutil/common.h"

__BEGIN_DECLS__

#define FACTOR_N               (256)

/**
 * @brief  init factor of the vol scale, divided into FACTOR_N(256) equal parts. once only
 * @param  [in] mindb
 * @param  [in] maxdb
 * @return
 */
void vol_scale_init(int mindb, int maxdb);

/**
 * @brief  vol scale
 * @param  [in] in
 * @param  [in] nb_samples
 * @param  [in] out
 * @param  [in] scale index : 0~255
 * @return 0/-1
 */
int vol_scale(const int16_t *in, size_t nb_samples, int16_t *out, uint8_t scale_index);

__END_DECLS__

#endif /* __VOL_SCALE_H__ */

