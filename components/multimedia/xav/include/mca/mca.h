/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MCA_H__
#define __MCA_H__

#include "mca/mca_cls.h"

__BEGIN_DECLS__

enum {
    MCA_TYPE_UNKNOWN,
    MCA_TYPE_IIR,
    MCA_TYPE_FIR,       // TODO
    MCA_TYPE_MCA,       // TODO:
};

/**
 * @brief  regist mca ops
 * @param  [in] ops
 * @return 0/-1
 */
int mcax_ops_register(const struct mcax_ops *ops);

/**
 * @brief  alloc a mca
 * @param  [in] type : MCA_TYPE_XXX
 * @return NULL on error
 */
mcax_t* mcax_new(int type);

/**
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 * @return -1/0
 */
int mcax_iir_fxp32_coeff32_config(mcax_t *mca, const fxp32_t *coeff);

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
int mcax_iir_fxp32(mcax_t *mca, const fxp32_t *input, size_t input_size, fxp32_t yn1, fxp32_t yn2,
                   fxp32_t *output);

/**
 * @brief  free the mca
 * @param  [in] mca
 * @return 0/-1
 */
int mcax_free(mcax_t *mca);

__END_DECLS__

#endif /* __MCA_H__ */

