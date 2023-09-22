/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __CMCA_H__
#define __CMCA_H__

#include "mca/mca.h"

__BEGIN_DECLS__

/**
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 * @return -1/0
 */
int cmca_iir_fxp32_coeff32_config(const fxp32_t *coeff);

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
int cmca_iir_fxp32(const fxp32_t *input, size_t input_size, fxp32_t yn1, fxp32_t yn2,
                   fxp32_t *output);

__END_DECLS__

#endif /* __CMCA_H__ */

