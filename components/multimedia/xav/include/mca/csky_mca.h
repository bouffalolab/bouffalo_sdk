#ifndef __CSKY_MCA_H__
#define __CSKY_MCA_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 8-bit fixed-point numeric type in user-defined format */
typedef int8_t fxp8_t;
/* 16-bit fixed-point numeric type in user-defined format */
typedef int16_t fxp16_t;
/* 24-bit fixed-point numeric type in user-defined format */
typedef int32_t fxp24_t;
/* 32-bit fixed-point numeric type in user-defined format */
typedef int32_t fxp32_t;
/* 64-bit fixed-point numeric type in user-defined format */
typedef int64_t fxp64_t;

/* 8-bit fixed-point numeric type in 1.0.7 format */
typedef fxp8_t q7_t;
/* 16-bit fixed-point numeric type in 1.0.15 format */
typedef fxp16_t q15_t;
/* 32-bit fixed-point numeric type in 1.15.16 format */
typedef fxp32_t q16_t;

// Addition of two 8-bit fixed-point vectors.
void csky_mca_vec_add_vec_fxp8(
    const fxp8_t *input_a,
    const fxp8_t *input_b,
    fxp8_t *output,
    size_t size);

// Addition of two 16-bit fixed-point vectors.
void csky_mca_vec_add_vec_fxp16(
    const fxp16_t *input_a,
    const fxp16_t *input_b,
    fxp16_t *output,
    size_t size);

// Addition of two 32-bit fixed-point vectors.
void csky_mca_vec_add_vec_fxp32(
    const fxp32_t *input_a,
    const fxp32_t *input_b,
    fxp32_t *output,
    size_t size);

// Addition of 8-bit fixed-point vector and scalar.
void csky_mca_vec_add_scalar_fxp8(
    const fxp8_t *input_vector,
    fxp8_t input_scalar,
    fxp8_t *output,
    size_t size);

// Addition of 16-bit fixed-point vector and scalar.
void csky_mca_vec_add_scalar_fxp16(
    const fxp16_t *input_vector,
    fxp16_t input_scalar,
    fxp16_t *output,
    size_t size);

// Addition of 32-bit fixed-point vector and scalar.
void csky_mca_vec_add_scalar_fxp32(
    const fxp32_t *input_vector,
    fxp32_t input_scalar,
    fxp32_t *output,
    size_t size);

/**
 * Multiplication of two 8-bit fixed-point vectors.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_vec_mul_vec_fxp8(
    const fxp8_t *input_a,
    const fxp8_t *input_b,
    fxp8_t *output,
    int8_t output_shift,
    size_t size);

/**
 * Multiplication of two 16-bit fixed-point vectors.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_vec_mul_vec_fxp16(
    const fxp16_t *input_a,
    const fxp16_t *input_b,
    fxp16_t *output,
    int8_t output_shift,
    size_t size);

/**
 * Multiplication of two 32-bit fixed-point vectors.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_vec_mul_vec_fxp32(
    const fxp32_t *input_a,
    const fxp32_t *input_b,
    fxp32_t *output,
    int8_t output_shift,
    size_t size);

/**
 * Multiplication of 8-bit fixed-point vector and scalar.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_vec_mul_scalar_fxp8(
    const fxp8_t *input_vector,
    fxp8_t input_scalar,
    fxp8_t *output,
    int8_t output_shift,
    size_t size);

/**
 * Multiplication of 16-bit fixed-point vector and scalar.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_vec_mul_scalar_fxp16(
    const fxp16_t *input_vector,
    fxp16_t input_scalar,
    fxp16_t *output,
    int8_t output_shift,
    size_t size);

/**
 * Multiplication of 32-bit fixed-point vector and scalar.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_vec_mul_scalar_fxp32(
    const fxp32_t *input_vector,
    fxp32_t input_scalar,
    fxp32_t *output,
    int8_t output_shift,
    size_t size);

/**
 * Multiplies a 8-bit fixed point matrix and a 8-bit fixed point vector and
 * returns a 8-bit fixed point vector.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_mat_mul_vec_fxp8(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    fxp8_t *output,
    int8_t output_shift);

/**
 * Multiplies a 16-bit fixed point matrix and a 16-bit fixed point vector and
 * returns a 16-bit fixed point vector.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_mat_mul_vec_fxp16(
    const fxp16_t *vector,
    const fxp16_t *matrix,
    size_t num_cols,
    size_t num_rows,
    fxp16_t *output,
    int8_t output_shift);

/**
 * Multiplies a 32-bit fixed point matrix and a 32-bit fixed point vector and
 * returns a 32-bit fixed point vector.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_mat_mul_vec_fxp32(
    const fxp32_t *vector,
    const fxp32_t *matrix,
    size_t num_cols,
    size_t num_rows,
    fxp32_t *output,
    int8_t output_shift);

/**
 * Multiplies a 8-bit fixed point matrix and a 8-bit fixed point vector and
 * returns a 32-bit fixed point vector.
 *
 * Note:
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_mat_mul_vec_v8_m8_o32(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    fxp32_t *output,
    int8_t output_shift);

/**
 * 8-bit fully-connected layer function
 *
 * Note:
 *   # num_cols and num_rows must be in range of [1, 1023].
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_fxp8(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp8_t *bias,
    int8_t bias_shift,
    fxp8_t *output,
    int8_t output_shift);

/**
 * 16-bit fully-connected layer function
 *
 * Note:
 *   # num_cols and num_rows must be in range of [1, 1023].
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_fxp16(
    const fxp16_t *vector,
    const fxp16_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp16_t *bias,
    int8_t bias_shift,
    fxp16_t *output,
    int8_t output_shift);

/**
 * Fully-connected layer function
 *   Vector (input ): 8-bit
 *   Matrix (weight): 8-bit
 *   Bias           : 8-bit
 *   Output         : 32-bit
 *
 * Note:
 *   # num_rows must be in range of [1, 1023], while num_cols has no limit.
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_v8_m8_b8_o32(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp8_t *bias,
    int8_t bias_shift,
    fxp32_t *output,
    int8_t output_shift);

/**
 * Fully-connected layer function
 *   Vector (input ): 8-bit
 *   Matrix (weight): 8-bit
 *   Bias           : 16-bit
 *   Output         : 32-bit
 *
 * Note:
 *   # num_rows must be in range of [1, 1023], while num_cols has no limit.
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_v8_m8_b16_o32(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp16_t *bias,
    int8_t bias_shift,
    fxp32_t *output,
    int8_t output_shift);

/**
 * 8-bit fully-connected layer function with ReLU
 *
 * Note:
 *   # num_cols and num_rows must be in range of [1, 1023].
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_with_relu_fxp8(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp8_t *bias,
    int8_t bias_shift,
    fxp8_t *output,
    int8_t output_shift);

/**
 * 16-bit fully-connected layer function with ReLU
 *
 * Note:
 *   # num_cols and num_rows must be in range of [1, 1023].
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_with_relu_fxp16(
    const fxp16_t *vector,
    const fxp16_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp16_t *bias,
    int8_t bias_shift,
    fxp16_t *output,
    int8_t output_shift);

/**
 * Fully-connected layer function with ReLU
 *   Vector (input ): 8-bit
 *   Matrix (weight): 8-bit
 *   Bias           : 8-bit
 *   Output         : 32-bit
 *
 * Note:
 *   # num_rows must be in range of [1, 1023], while num_cols has no limit.
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_with_relu_v8_m8_b8_o32(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp8_t *bias,
    int8_t bias_shift,
    fxp32_t *output,
    int8_t output_shift);

/**
 * Fully-connected layer function with ReLU
 *   Vector (input ): 8-bit
 *   Matrix (weight): 8-bit
 *   Bias           : 16-bit
 *   Output         : 32-bit
 *
 * Note:
 *   # num_rows must be in range of [1, 1023], while num_cols has no limit.
 *   # bias_shift represents the amount of left(+) or right(-) shift for bias.
 *   # output_shift represents the amount of left(-) or right(+) shift for
 *     output.
 */
void csky_mca_fully_connected_with_relu_v8_m8_b16_o32(
    const fxp8_t *vector,
    const fxp8_t *matrix,
    size_t num_cols,
    size_t num_rows,
    const fxp16_t *bias,
    int8_t bias_shift,
    fxp32_t *output,
    int8_t output_shift);

/**
 * 16-bit softmax function
 *
 * Note:
 *   # size must be in range of [1, 1023].
 *   # input is 32-bit q16.
 *   # output is 16-bit q15.
 *   # input will be used as intermediate buffer, so its content will be
 *     over-written.
 */
void csky_mca_softmax_fxp16(
    q16_t *input,
    q15_t *output,
    size_t size);

/**
 * Supported FFT lengths
 */
typedef enum {
    // 512-point FFT
    CSKY_MCA_FFT_LEN_512 = 0x1,
    // 256-point FFT
    CSKY_MCA_FFT_LEN_256 = 0x2,
    // 128-point FFT
    CSKY_MCA_FFT_LEN_128 = 0x4,
    // 64-point FFT
    CSKY_MCA_FFT_LEN_64 = 0x8,
    // 32-point FFT
    CSKY_MCA_FFT_LEN_32 = 0x10,
    // 16-point FFT
    CSKY_MCA_FFT_LEN_16 = 0x20,
} csky_mca_fft_len_t;

/**
 * 32-bit fixed-point real FFT.
 *
 * Note:
 *   # The Q of output is log2(N) less than the Q of input, where N is the
 *     FFT length (fft_len).
 *   # Input size must be in range of [1, fft_len]. If it is less than fft_len,
 *     zeros will be appended to the input.
 *   # Output size is fft_len, with format of:
 *     [R0, R(fft_len/2), R1, I1, ..., R(fft_len/2-1), I(fft_len/2-1)]
 *   # To enable in-place computation, set output same as input.
 */
void csky_mca_rfft_fxp32(
    csky_mca_fft_len_t fft_len,
    const fxp32_t *input,
    size_t input_size,
    fxp32_t *output);

/**
 * 32-bit fixed-point complex FFT.
 *
 * Note:
 *   # The Q of output is (log2(N)+1) less than the Q of input, where N is the
 *     FFT length (fft_len).
 *   # Input size is fft_len * 2, with format of:
 *     [R0, I0, R1, I1, ..., R(fft_len-1), I(fft_len-1)]
 *   # Output size is fft_len * 2, with format of:
 *     [R0, I0, R1, I1, ..., R(fft_len-1), I(fft_len-1)]
 *   # To enable in-place computation, set output same as input.
 */
void csky_mca_cfft_fxp32(
    csky_mca_fft_len_t fft_len,
    const fxp32_t *input,
    fxp32_t *output);

/**
 * 32-bit fixed-point real inverse FFT.
 *
 * Note:
 *   # The Q of output is 1 less than the Q of input.
 *   # Input size is fft_len, with format of:
 *     [R0, R(fft_len/2), R1, I1, ..., R(fft_len/2-1), I(fft_len/2-1)]
 *   # Output size is fft_len, with format of:
 *     [R0, R1, R2, R3, ..., R(fft_len-2), R(fft_len-1)]
 *   # To enable in-place computation, set output same as input.
 */
void csky_mca_rifft_fxp32(
    csky_mca_fft_len_t fft_len,
    const fxp32_t *input,
    fxp32_t *output);

/**
 * 32-bit fixed-point complex FFT.
 *
 * Note:
 *   # The Q of output is 1 less than the Q of input.
 *   # Input size is fft_len * 2, with format of:
 *     [R0, I0, R1, I1, ..., R(fft_len-1), I(fft_len-1)]
 *   # Output size is fft_len * 2, with format of:
 *     [R0, I0, R1, I1, ..., R(fft_len-1), I(fft_len-1)]
 *   # To enable in-place computation, set output same as input.
 */
void csky_mca_cifft_fxp32(
    csky_mca_fft_len_t fft_len,
    const fxp32_t *input,
    fxp32_t *output);

/**
 * 32-bit fixed-point power spectrum.
 *
 * Note:
 *   # The Q of output is (2*(Q_in-log2(N))), where Q_in is Q of input, and N is
 *     the FFT length (fft_len).
 *   # Input size must be in range of [1, fft_len]. If it is less than fft_len,
 *     zeros will be appended to the input.
 *   # Output size is (fft_len/2+1), 64-bit, with format of:
 *     [PS0, PS1, PS2, ..., PS(fftlen/2-1), PS(fftlen/2)]
 *   # To enable in-place computation, set output same as input, and its length
 *     must be at least (N + 2) (32-bit).
 */
void csky_mca_power_spectrum_fxp32(
    csky_mca_fft_len_t fft_len,
    const fxp32_t *input,
    size_t input_size,
    fxp64_t *output);

/*******************************************************************************
 * ASYNCHRONOUS SAMPLING RATE CONVERTER (ASRC)
 *
 * Coefficients are given as:
 *   [b(0,0)    b(0,1)    b(0,2)    ... b(0,nb)    s(0)   ]
 *   [b(1,0)    b(1,1)    b(1,2)    ... b(1,nb)    s(1)   ]
 *   [...       ...       ...       ...            ...    ]
 *   [b(nL-1,0) b(nL-1,1) b(nL-1,2) ... b(nL-1,nb) s(nL-1)]
 * Where nL is list size, and nb is order. The last element in each row is step
 * to skip in input samples for next output sample.
 *
 * ASRC stores the row index (reset to 0 after configuration) of the
 * coefficients after a conversion process is completed. Assume the last stored
 * row index is ridx, outputs are calculated as:
 *   ------
 *   y(0)         =
 *       b(ridx,0)  * x(0) +
 *       b(ridx,1)  * x(1) +
 *       ...               +
 *       b(ridx,nb) * x(nb)
 *   ------
 *   y(1)         =
  *      b(ridx+1,0)  * x(0  + s(ridx)) +
 *       b(ridx+1,1)  * x(1  + s(ridx)) +
 *       ...                            +
 *       b(ridx+1,nb) * x(nb + s(ridx))
 *   ------
 *   ...
 *   ------
 *   y(nL-ridx-1) =
  *      b(nL-1,0)  * x(0  + s(ridx) + ... + s(nL-2)) +
 *       b(nL-1,1)  * x(1  + s(ridx) + ... + s(nL-2)) +
 *       ...                                          +
 *       b(nL-1,nb) * x(nb + s(ridx) + ... + s(nL-2))
 *   ------
 *   y(nL-ridx)   =
  *      b(0,0)     * x(0  + s(ridx) + ... + s(nL-2) + s(nL-1)) +
 *       b(0,1)     * x(1  + s(ridx) + ... + s(nL-2) + s(nL-1)) +
 *       ...                                                    +
 *       b(0,nb)    * x(nb + s(ridx) + ... + s(nL-2) + s(nL-1))
 *   ------
 *   y(nL-ridx+1) =
  *      b(1,0)     * x(0  + s(ridx) + ... + s(nL-2) + s(nL-1) + s(0)) +
 *       b(1,1)     * x(1  + s(ridx) + ... + s(nL-2) + s(nL-1) + s(0)) +
 *       ...                                                           +
 *       b(1,nb)    * x(nb + s(ridx) + ... + s(nL-2) + s(nL-1) + s(0))
 *   ------
 *   ...
 *   ------
 * The ASRC process will be terminated when input samples is not enough for the
 * next output sample, and the coefficients row index for that output sample
 * will be stored so it can be used in the next process.
 *
 * Note:
 *   # Bit-width and precision of output are same to the input.
 *   # For the stereo-channel case, output samples are organized as:
 *     ch1-ch2-ch1-ch2-ch1-ch2-...-ch1-ch2
 *   # For CONTINUOUS ASRC process, consider the following case for convenient:
 *     Index of the last input sample to be used for a completed loop is:
 *         index_a = nb + s(0) + s(1) + ... + s(nL-2)
 *     Index of the first input sample for a new output sample in next loop is:
 *         index_b =      s(0) + s(1) + ... + s(nL-2) + s(nL-1)
 *     Index of the last input sample for a new output sample in next loop is:
 *         index_c = nb + s(0) + s(1) + ... + s(nL-2）＋s(nL-1)
 *
 *     So a completed loop requires the number of input samples (N) satisfy:
 *         N > index_a && N <= index_c
 *
 *     If index_a < index_b, i.e. nb < s(nL-1), the best N is
 *         N_best = index_b
  *    And there is no need to copy any input samples to the next loop.
 *
 *     If index_a >= index_b, i.e. nb >= s(nL-1), the best N is
 *         N_best = {index_a - index_b + 1} + {index_b}
 *     Where the first part {index_a - index_b + 1} is number of old input
 *     samples, and the second part {index_b} is number of new input samples.
 ******************************************************************************/

/**
 * Configures the ASRC with 16-bit input/output in stereo-channel mode.
 *
 * Note:
 *   # coeff is 32-bit (Q1.0.31), with dimension of (list_size, order + 1 + 1).
 *   # order must be in range of [0, 15].
 *   # list_size must be in range [1, 256].
 */
void csky_mca_asrc_stereo_fxp16_config(
    const fxp32_t *coeff,
    size_t order,
    size_t list_size);

/**
 * Configures the ASRC with 24-bit input/output in stereo-channel mode.
 *
 * Note:
 *   # coeff is 32-bit (Q1.0.31), with dimension of (list_size, order + 1 + 1).
 *   # order must be in range of [0, 15].
 *   # list_size must be in range [1, 256].
 */
void csky_mca_asrc_stereo_fxp24_config(
    const fxp32_t *coeff,
    size_t order,
    size_t list_size);

/**
 * Configures the ASRC with 32-bit input/output in stereo-channel mode.
 *
 * Note:
 *   # coeff is 32-bit (Q1.0.31), with dimension of (list_size, order + 1 + 1).
 *   # order must be in range of [0, 15].
 *   # list_size must be in range [1, 256].
 */
void csky_mca_asrc_stereo_fxp32_config(
    const fxp32_t *coeff,
    size_t order,
    size_t list_size);

/**
 * Configures the ASRC with 16-bit input/output in mono-channel mode.
 *
 * Note:
 *   # coeff is 32-bit (Q1.0.31), with dimension of (list_size, order + 1 + 1).
 *   # order must be in range of [0, 15].
 *   # list_size must be in range [1, 256].
 */
void csky_mca_asrc_mono_fxp16_config(
    const fxp32_t *coeff,
    size_t order,
    size_t list_size);

/**
 * Configures the ASRC with 24-bit input/output in mono-channel mode.
 *
 * Note:
 *   # coeff is 32-bit (Q1.0.31), with dimension of (list_size, order + 1 + 1).
 *   # order must be in range of [0, 15].
 *   # list_size must be in range [1, 256].
 */
void csky_mca_asrc_mono_fxp24_config(
    const fxp32_t *coeff,
    size_t order,
    size_t list_size);

/**
 * Configures the ASRC with 32-bit input/output in mono-channel mode.
 *
 * Note:
 *   # coeff is 32-bit (Q1.0.31), with dimension of (list_size, order + 1 + 1).
 *   # order must be in range of [0, 15].
 *   # list_size must be in range [1, 256].
 */
void csky_mca_asrc_mono_fxp32_config(
    const fxp32_t *coeff,
    size_t order,
    size_t list_size);

/**
 * ASRC with 16-bit input/output in stereo-channel mode.
 *
 * Note:
 *   # input_ch1/input_ch2/output are 16-bit (Q1.15.0).
 *   # Size of output in byte is returned.
 */
size_t csky_mca_asrc_stereo_fxp16(
    const fxp16_t *input_ch1,
    const fxp16_t *input_ch2,
    size_t input_size,
    fxp16_t *output);

/**
 * ASRC with 24-bit input/output in stereo-channel mode.
 *
 * Note:
 *   # input_ch1/input_ch2/output are 24-bit (Q1.23.0).
 *   # Size of output in byte is returned.
 */
size_t csky_mca_asrc_stereo_fxp24(
    const fxp24_t *input_ch1,
    const fxp24_t *input_ch2,
    size_t input_size,
    fxp24_t *output);

/**
 * ASRC with 32-bit input/output in stereo-channel mode.
 *
 * Note:
 *   # input_ch1/input_ch2/output are 32-bit (Q1.31.0).
 *   # Size of output in byte is returned.
 */
size_t csky_mca_asrc_stereo_fxp32(
    const fxp32_t *input_ch1,
    const fxp32_t *input_ch2,
    size_t input_size,
    fxp32_t *output);

/**
 * ASRC with 16-bit input/output in mono-channel mode.
 *
 * Note:
 *   # input_ch1/input_ch2/output are 16-bit (Q1.15.0).
 *   # Size of output in byte is returned.
 */
size_t csky_mca_asrc_mono_fxp16(
    const fxp16_t *input,
    size_t input_size,
    fxp16_t *output);

/**
 * ASRC with 24-bit input/output in mono-channel mode.
 *
 * Note:
 *   # input_ch1/input_ch2/output are 24-bit (Q1.23.0).
 *   # Size of output in byte is returned.
 */
size_t csky_mca_asrc_mono_fxp24(
    const fxp24_t *input,
    size_t input_size,
    fxp24_t *output);

/**
 * ASRC with 32-bit input/output in mono-channel mode.
 *
 * Note:
 *   # input_ch1/input_ch2/output are 32-bit (Q1.31.0).
 *   # Size of output in byte is returned.
 */
size_t csky_mca_asrc_mono_fxp32(
    const fxp32_t *input,
    size_t input_size,
    fxp32_t *output);

/*******************************************************************************
 * FIR FILTERS
 *
 * Output is calculated as:
 *   y(n) = b(0)*x(n) + b(1)*x(n-1) + ... + b(nb)*x(n-nb)
 * Where b is filter coefficients, and nb is order. The FIR filter
 * coefficients (symmetric) are stored as follows for fast computation:
 *   [b(nb) ... b(nb-1) b(2) b(1) b(0)]
 ******************************************************************************/

/**
 * Configures FIR filter with 16-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of (order + 1).
 *   # order must be in range of [0, 4095].
 *   # Input/output are 16-bit (Q1.15.0).
 */
void csky_mca_fir_fxp16_coeff32_config(const fxp32_t *coeff, size_t order);

/**
 * Configures FIR filter with 24-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of (order + 1).
 *   # order must be in range of [0, 4095].
 *   # Input/output are 24-bit (Q1.23.0).
 */
void csky_mca_fir_fxp24_coeff32_config(const fxp32_t *coeff, size_t order);

/**
 * Configures FIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of (order + 1).
 *   # order must be in range of [0, 4095].
 *   # Input/output are 32-bit (Q1.31.0).
 */
void csky_mca_fir_fxp32_coeff32_config(const fxp32_t *coeff, size_t order);

/**
 * Configures FIR filter with 16-bit input/ouput and 64-bit coefficients.
 *
 * Note:
 *   # coeff is 64-bit (Q1.15.48), with size of (order + 1).
 *   # order must be in range of [0, 4095].
 *   # Input is 16-bit (Q1.15.0). Output is also 16-bit but its precision can be
 *     configured.
 *   # output_shift must be multiple of 4, and in range of [0 80].
 */
void csky_mca_fir_fxp16_coeff64_config(
    const fxp64_t *coeff,
    size_t order,
    uint8_t output_shift);

/**
 * Configures FIR filter with 24-bit input/ouput and 64-bit coefficients.
 *
 * Note:
 *   # coeff is 64-bit (Q1.15.48), with size of (order + 1).
 *   # order must be in range of [0, 4095].
 *   # Input is 24-bit (Q1.23.0). Output is also 24-bit but its precision can be
 *     configured.
 *   # output_shift must be multiple of 4, and in range of [0 72].
 */
void csky_mca_fir_fxp24_coeff64_config(
    const fxp64_t *coeff,
    size_t order,
    uint8_t output_shift);

/**
 * Configures FIR filter with 32-bit input/ouput and 64-bit coefficients.
 *
 * Note:
 *   # coeff is 64-bit (Q1.15.48), with size of (order + 1).
 *   # order must be in range of [0, 4095].
 *   # Input is 32-bit (Q1.31.0). Output is also 32-bit but its precision can be
 *     configured.
 *   # output_shift must be multiple of 4, and in range of [0 64].
 */
void csky_mca_fir_fxp32_coeff64_config(
    const fxp64_t *coeff,
    size_t order,
    uint8_t output_shift);

/**
 * FIR filter with 16-bit input/output.
 *
 * Note:
 *   # input is 16-bit (Q1.15.0), output also 16-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - order).
 */
void csky_mca_fir_fxp16(
    const fxp16_t *input,
    size_t input_size,
    fxp16_t *output);

/**
 * FIR filter with 24-bit input/output.
 *
 * Note:
 *   # input is 24-bit (Q1.23.0), output also 24-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - order).
 */
void csky_mca_fir_fxp24(
    const fxp24_t *input,
    size_t input_size,
    fxp24_t *output);

/**
 * FIR filter with 32-bit input/output.
 *
 * Note:
 *   # input is 32-bit (Q1.31.0), output also 32-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - order).
 */
void csky_mca_fir_fxp32(
    const fxp32_t *input,
    size_t input_size,
    fxp32_t *output);

/*******************************************************************************
 * 2-ORDER IIR FILTERS
 *
 * Output is calculated as:
 *   y(n) = b(0)*x(n) + b(1)*x(n-1) + b(2)*x(n-2)
 *                    - a(1)*y(n-1) - a(2)*y(n-2)
 * Where b and a are numerator and denominator coefficients respectively. The
 * IIR filter coefficients are stored as follows for fast computation:
 *   [b(2) b(1) b(0) -a(1) -a(2)]
 ******************************************************************************/

/**
 * Configures IIR filter with 16-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 16-bit (Q1.15.0).
 */
void csky_mca_iir_fxp16_coeff32_config(const fxp32_t *coeff);

/**
 * Configures IIR filter with 24-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 24-bit (Q1.23.0).
 */
void csky_mca_iir_fxp24_coeff32_config(const fxp32_t *coeff);

/**
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 */
void csky_mca_iir_fxp32_coeff32_config(const fxp32_t *coeff);

/**
 * Configures IIR filter with 16-bit input/ouput and 64-bit coefficients.
 *
 * Note:
 *   # coeff is 64-bit (Q1.15.48), with size of 5.
 *   # Input is 16-bit (Q1.15.0). Output is also 16-bit but its precision can be
 *     configured.
 *   # output_shift must be multiple of 4, and in range of [0 80].
 */
void csky_mca_iir_fxp16_coeff64_config(
    const fxp64_t *coeff,
    uint8_t output_shift);

/**
 * Configures IIR filter with 24-bit input/ouput and 64-bit coefficients.
 *
 * Note:
 *   # coeff is 64-bit (Q1.15.48), with size of 5.
 *   # Input is 24-bit (Q1.23.0). Output is also 24-bit but its precision can be
 *     configured.
 *   # output_shift must be multiple of 4, and in range of [0 72].
 */
void csky_mca_iir_fxp24_coeff64_config(
    const fxp64_t *coeff,
    uint8_t output_shift);

/**
 * Configures IIR filter with 32-bit input/ouput and 64-bit coefficients.
 *
 * Note:
 *   # coeff is 64-bit (Q1.15.48), with size of 5.
 *   # Input is 32-bit (Q1.31.0). Output is also 32-bit but its precision can be
 *     configured.
 *   # output_shift must be multiple of 4, and in range of [0 64].
 */
void csky_mca_iir_fxp32_coeff64_config(
    const fxp64_t *coeff,
    uint8_t output_shift);

/**
 * IIR filter with 16-bit input/output.
 *
 * Note:
 *   # input is 16-bit (Q1.15.0), output also 16-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - 2).
 *   # yn1 and yn2 are the 1st and 2nd samples from the last output.
 */
void csky_mca_iir_fxp16(
    const fxp16_t *input,
    size_t input_size,
    fxp16_t yn1,
    fxp16_t yn2,
    fxp16_t *output);

/**
 * IIR filter with 24-bit input/output.
 *
 * Note:
 *   # input is 24-bit (Q1.23.0), output also 24-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - 2).
 *   # yn1 and yn2 are the 1st and 2nd samples from the last output.
 */
void csky_mca_iir_fxp24(
    const fxp24_t *input,
    size_t input_size,
    fxp24_t yn1,
    fxp24_t yn2,
    fxp24_t *output);

/**
 * IIR filter with 32-bit input/output.
 *
 * Note:
 *   # input is 32-bit (Q1.31.0), output also 32-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - 2).
 *   # yn1 and yn2 are the 1st and 2nd samples from the last output.
 */
void csky_mca_iir_fxp32(
    const fxp32_t *input,
    size_t input_size,
    fxp32_t yn1,
    fxp32_t yn2,
    fxp32_t *output);

#ifdef __cplusplus
}
#endif

#endif // !__CSKY_MCA_H__
