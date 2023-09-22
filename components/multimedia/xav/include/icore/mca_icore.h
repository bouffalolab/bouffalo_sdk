/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCA_ICORE_H__
#define __MCA_ICORE_H__

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>
#include "mca/csky_mca.h"

__BEGIN_DECLS__

typedef struct mca_icore mca_icore_t;

/**
 * @brief  init the icore inter-core mca
 * @return 0/-1
 */
int mca_icore_init();

/**
 * @brief  alloc a inter-core mca
 * @param  [in] type : MCA_TYPE_XXX
 * @return NULL on error
 */
mca_icore_t* mca_icore_new(int type);

/**
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 * @return -1/0
 */
int mca_icore_iir_fxp32_coeff32_config(mca_icore_t *hdl, const fxp32_t *coeff);

/**
 * IIR filter with 32-bit input/output.
 *
 * Note:
 *   # input is 32-bit (Q1.31.0), output also 32-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - 2).
 *   # yn1 and yn2 are the 1st and 2nd samples from the last output.
 * @return -1/0
 */
int mca_icore_iir_fxp32(mca_icore_t *hdl, const fxp32_t *input, size_t input_size, fxp32_t yn1, fxp32_t yn2,
                        fxp32_t *output);

/**
 * @brief  free the inter-core mca
 * @param  [in] hdl
 * @return 0/-1
 */
int mca_icore_free(mca_icore_t *hdl);

__END_DECLS__

#endif /* __MCA_ICORE_H__ */

