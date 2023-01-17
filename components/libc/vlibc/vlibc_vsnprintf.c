/**
 * @author (c) Eyal Rozenberg <eyalroz1@gmx.com>
 *             2021-2022, Haifa, Palestine/Israel
 * @author (c) Marco Paland (info@paland.com)
 *             2014-2019, PALANDesign Hannover, Germany
 *
 * @note Others have made smaller contributions to this file: see the
 * contributors page at https://github.com/eyalroz/printf/graphs/contributors
 * or ask one of the authors. The original code for exponential specifiers was
 * contributed by Martijn Jasperse <m.jasperse@gmail.com>.
 *
 * @brief Small stand-alone implementation of the printf family of functions
 * (`(v)printf`, `(v)s(n)printf` etc., geared towards use on embedded systems with
 * a very limited resources.
 *
 * @note the implementations are thread-safe; re-entrant; use no functions from
 * the standard library; and do not dynamically allocate any memory.
 *
 * @license The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "vlibc_stdio.h"

#define _VLIBC_IO_HAVE_WRITE ((unsigned char)(0x04))
#define _VLIBC_IO_HAVE_READ  ((unsigned char)(0x08))
#define _VLIBC_IO_VBUF_MASK  ((unsigned char)(0x60))
#define _VLIBC_IO_VBUF_FBF   ((unsigned char)(0x40))
#define _VLIBC_IO_VBUF_LBF   ((unsigned char)(0x20))
#define _VLIBC_IO_VBUF_NBF   ((unsigned char)(0x00))
#define _VLIBC_IO_AUTO_BUFF  ((unsigned char)(0x80))

#define _VLIBC_TYPEIS_IO     ((int)(0x01))
#define _VLIBC_TYPEIS_FILE   ((int)(0x02))

#ifdef CONFIG_VLIBC_DEBUG
#define CHECK_FILE(_stream, __ret)       \
    do {                                 \
        if ((void *)(_stream) == NULL) { \
            errno = EINVAL;              \
            return __ret;                \
        }                                \
    } while (0)
#else
#define CHECK_FILE(_stream, __ret) \
    do {                           \
    } while (0)
#endif

#define vlibc_file(_stream) ((vlibc_file_t *)(_stream))

#define IF_IO_DEV(_stream) \
    if (((vlibc_file(_stream)->magic) & _VLIBC_MAGIC_MASK) == _VLIBC_IO_MAGIC_CODE)

#define IF_FILE(_stream) \
    if (((vlibc_file(_stream)->magic) & _VLIBC_MAGIC_MASK) == _VLIBC_FILE_MAGIC_CODE)

#ifdef CONFIG_VLIBC_FATFS
extern int FRESULT_to_errno[20];
#endif

/** @addtogroup config ********************************************************
 * @{
 */

// 'ntoa' conversion buffer size, this must be big enough to hold one converted
// numeric number including padded zeros (dynamically created on stack)
#ifndef PRINTF_INTEGER_BUFFER_SIZE
#define PRINTF_INTEGER_BUFFER_SIZE 32
#endif

// size of the fixed (on-stack) buffer for printing individual decimal numbers.
// this must be big enough to hold one converted floating-point value including
// padded zeros.
#ifndef PRINTF_DECIMAL_BUFFER_SIZE
#define PRINTF_DECIMAL_BUFFER_SIZE 32
#endif

// Support for the decimal notation floating point conversion specifiers (%f, %F)
#ifndef CONFIG_VLIBC_FLOAT
#define CONFIG_VLIBC_FLOAT 1
#endif

// Support for the exponential notation floating point conversion specifiers (%e, %g, %E, %G)
#ifndef CONFIG_VLIBC_FLOAT_EX
#define CONFIG_VLIBC_FLOAT_EX 0
#endif

// Support for the length write-back specifier (%n)
#ifndef CONFIG_VLIBC_WRITEBACK
#define CONFIG_VLIBC_WRITEBACK 0
#endif

// Default precision for the floating point conversion specifiers (the C standard sets this at 6)
#ifndef VLIBC_DEFAULT_FLOAT_PRECISION
#define VLIBC_DEFAULT_FLOAT_PRECISION 6
#endif

// According to the C languages standard, printf() and related functions must be able to print any
// integral number in floating-point notation, regardless of length, when using the %f specifier -
// possibly hundreds of characters, potentially overflowing your buffers. In this implementation,
// all values beyond this threshold are switched to exponential notation.
#ifndef VLIBC_MAX_INTEGRAL_DIGITS_FOR_DECIMAL
#define VLIBC_MAX_INTEGRAL_DIGITS_FOR_DECIMAL 9
#endif

// Support for the long long integral types (with the ll, z and t length modifiers for specifiers
// %d,%i,%o,%x,%X,%u, and with the %p specifier). Note: 'L' (long double) is not supported.
#ifndef CONFIG_VLIBC_LONG_LONG
#define CONFIG_VLIBC_LONG_LONG 0
#endif

// The number of terms in a Taylor series expansion of log_10(x) to
// use for approximation - including the power-zero term (i.e. the
// value at the point of expansion).
#ifndef PRINTF_LOG10_TAYLOR_TERMS
#define PRINTF_LOG10_TAYLOR_TERMS 4
#endif

#if PRINTF_LOG10_TAYLOR_TERMS <= 1
#error "At least one non-constant Taylor expansion is necessary for the log10() calculation"
#endif

#define PRINTF_PREFER_DECIMAL                  false
#define PRINTF_PREFER_EXPONENTIAL              true

/******************************************************************************
 * @}
 */

// The following will convert the number-of-digits into an exponential-notation literal
#define PRINTF_CONCATENATE(s1, s2)             s1##s2
#define PRINTF_EXPAND_THEN_CONCATENATE(s1, s2) PRINTF_CONCATENATE(s1, s2)
#define PRINTF_FLOAT_NOTATION_THRESHOLD        PRINTF_EXPAND_THEN_CONCATENATE(1e, VLIBC_MAX_INTEGRAL_DIGITS_FOR_DECIMAL)

// internal flag definitions
#define FLAGS_ZEROPAD                          (1U << 0U)
#define FLAGS_LEFT                             (1U << 1U)
#define FLAGS_PLUS                             (1U << 2U)
#define FLAGS_SPACE                            (1U << 3U)
#define FLAGS_HASH                             (1U << 4U)
#define FLAGS_UPPERCASE                        (1U << 5U)
#define FLAGS_CHAR                             (1U << 6U)
#define FLAGS_SHORT                            (1U << 7U)
#define FLAGS_INT                              (1U << 8U)
#define FLAGS_LONG                             (1U << 9U)
#define FLAGS_LONG_LONG                        (1U << 10U)
#define FLAGS_PRECISION                        (1U << 11U)
#define FLAGS_ADAPT_EXP                        (1U << 12U)
#define FLAGS_POINTER                          (1U << 13U)
// Note: Similar, but not identical, effect as FLAGS_HASH
#define FLAGS_SIGNED                           (1U << 14U)

typedef unsigned int printf_flags_t;

#define BASE_BINARY  2
#define BASE_OCTAL   8
#define BASE_DECIMAL 10
#define BASE_HEX     16

typedef uint8_t numeric_base_t;

#if CONFIG_VLIBC_LONG_LONG
typedef unsigned long long printf_unsigned_value_t;
typedef long long printf_signed_value_t;
#else
typedef unsigned long printf_unsigned_value_t;
typedef long printf_signed_value_t;
#endif

#define PRINTF_MAX_POSSIBLE_BUFFER_SIZE INT_MAX
// If we were to nitpick, this would actually be INT_MAX + 1,
// since INT_MAX is the maximum return value, which excludes the
// trailing '\0'.

#if (CONFIG_VLIBC_FLOAT || CONFIG_VLIBC_FLOAT_EX)
#include <float.h>
#if FLT_RADIX != 2
#error "Non-binary-radix floating-point types are unsupported."
#endif

#if DBL_MANT_DIG == 24

#define DOUBLE_SIZE_IN_BITS 32
typedef uint32_t double_uint_t;
#define DOUBLE_EXPONENT_MASK                0xFFU
#define DOUBLE_BASE_EXPONENT                127
#define DOUBLE_MAX_SUBNORMAL_EXPONENT_OF_10 -38
#define DOUBLE_MAX_SUBNORMAL_POWER_OF_10    1e-38

#elif DBL_MANT_DIG == 53

#define DOUBLE_SIZE_IN_BITS                 64
typedef uint64_t double_uint_t;
#define DOUBLE_EXPONENT_MASK                0x7FFU
#define DOUBLE_BASE_EXPONENT                1023
#define DOUBLE_MAX_SUBNORMAL_EXPONENT_OF_10 -308
#define DOUBLE_MAX_SUBNORMAL_POWER_OF_10    1e-308

#else
#error "Unsupported double type configuration"
#endif
#define DOUBLE_STORED_MANTISSA_BITS (DBL_MANT_DIG - 1)

typedef union {
    double_uint_t U;
    double F;
} double_with_bit_access;

// This is unnecessary in C99, since compound initializers can be used,
// but:
// 1. Some compilers are finicky about this;
// 2. Some people may want to convert this to C89;
// 3. If you try to use it as C++, only C++20 supports compound literals
static inline double_with_bit_access get_bit_access(double x)
{
    double_with_bit_access dwba;
    dwba.F = x;
    return dwba;
}

static inline int get_sign_bit(double x)
{
    // The sign is stored in the highest bit
    return (int)(get_bit_access(x).U >> (DOUBLE_SIZE_IN_BITS - 1));
}

static inline int get_exp2(double_with_bit_access x)
{
    // The exponent in an IEEE-754 floating-point number occupies a contiguous
    // sequence of bits (e.g. 52..62 for 64-bit doubles), but with a non-trivial representation: An
    // unsigned offset from some negative value (with the extremal offset values reserved for
    // special use).
    return (int)((x.U >> DOUBLE_STORED_MANTISSA_BITS) & DOUBLE_EXPONENT_MASK) - DOUBLE_BASE_EXPONENT;
}
#define PRINTF_ABS(_x) ((_x) > 0 ? (_x) : -(_x))

#endif // (CONFIG_VLIBC_FLOAT || CONFIG_VLIBC_FLOAT_EX)

// Note in particular the behavior here on LONG_MIN or LLONG_MIN; it is valid
// and well-defined, but if you're not careful you can easily trigger undefined
// behavior with -LONG_MIN or -LLONG_MIN
#define ABS_FOR_PRINTING(_x) ((printf_unsigned_value_t)((_x) > 0 ? (_x) : -((printf_signed_value_t)_x)))

// wrapper (used as buffer) for output function type
//
// One of the following must hold:
// 1. max is 0
// 2. buffer is non-null
// 3. stream is non-null
//
// ... otherwise bad things will happen.
typedef struct {
    VLIBC_FILE *stream;
    char *buffer;
    size_t pos;
    size_t max;
} output_gadget_t;

/** @addtogroup putc functions
 * @{
 */

static inline void putc_via_buffer(output_gadget_t *output, char c)
{
    size_t write_pos = output->pos++;
    if (write_pos >= output->max) {
        return;
    }

    output->buffer[write_pos] = c;
}

static inline void putc_via_file(output_gadget_t *output, char c)
{
    size_t write_pos = output->max++;
    output->pos++;

    if (write_pos >= VLIBC_FBUFSIZ) {
        output->max = 0;
        write_pos = 0;
#ifdef CONFIG_VLIBC_FATFS
        FRESULT fresult;
        size_t bytes;
        fresult = f_write(output->stream->file, output->buffer, VLIBC_FBUFSIZ, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }
        }
#else

#endif
    }

    output->buffer[write_pos] = c;
    return;
}

static inline void putc_via_iodev_nbf(output_gadget_t *output, char c)
{
    output->pos++;
    if (__vlibc_io_mem2dev(output->stream->io, &c, 1) == 0) {
        errno = EINVAL;
    }
}

static inline void putc_via_iodev_lbf(output_gadget_t *output, char c)
{
    register char *bg = output->stream->io->bg;
    register char *wp = output->stream->io->wp;
    register char *ed = output->stream->io->ed;

    output->pos++;
    *wp++ = c;

    if ((c == '\n') || (wp == ed)) {
        if (__vlibc_io_mem2dev(output->stream->io, bg, wp - bg) == 0) {
            errno = EINVAL;
            goto done;
        }
        wp = bg;
    }

done:
    output->stream->io->wp = wp;
}

static inline void putc_via_iodev_fbf(output_gadget_t *output, char c)
{
    register char *bg = output->stream->io->bg;
    register char *wp = output->stream->io->wp;
    register char *ed = output->stream->io->ed;

    output->pos++;
    *wp++ = c;

    if (wp == ed) {
        if (__vlibc_io_mem2dev(output->stream->io, bg, ed - bg) == 0) {
            errno = EINVAL;
            goto done;
        }
        wp = bg;
    }

done:
    output->stream->io->wp = wp;
}

/**
 * @}
 */

/** @addtogroup internal std c functions
 * @{
 */

// internal secure strlen
// @return The length of the string (excluding the terminating 0) limited by 'maxsize'
// @note strlen uses size_t, but wes only use this function with size_t
// variables - hence the signature.
static inline size_t strnlen_s_(const char *str, size_t maxsize)
{
    const char *s;
    for (s = str; *s && maxsize--; ++s)
        ;
    return (size_t)(s - str);
}

// internal test if char is a digit (0-9)
// @return true if char is a digit
static inline bool is_digit_(char ch)
{
    return (ch >= '0') && (ch <= '9');
}

// internal ASCII string to size_t conversion
static size_t atou_(const char **str)
{
    size_t i = 0U;
    while (is_digit_(**str)) {
        i = i * 10U + (size_t)(*((*str)++) - '0');
    }
    return i;
}

/**
 * @}
 */

// output the specified string in reverse, taking care of any zero-padding
static void out_rev_(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), const char *buf, size_t len, size_t width, printf_flags_t flags)
{
    const size_t start_pos = output->pos;

    // pad spaces up to given width
    if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
        for (size_t i = len; i < width; i++) {
            putc_function(output, ' ');
        }
    }

    // reverse string
    while (len) {
        putc_function(output, buf[--len]);
    }

    // append pad spaces up to given width
    if (flags & FLAGS_LEFT) {
        while (output->pos - start_pos < width) {
            putc_function(output, ' ');
        }
    }
}

// Invoked by print_integer after the actual number has been printed, performing necessary
// work on the number's prefix (as the number is initially printed in reverse order)
static void print_integer_finalization(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), char *buf, size_t len, bool negative, numeric_base_t base, size_t precision, size_t width, printf_flags_t flags)
{
    size_t unpadded_len = len;

    // pad with leading zeros
    {
        if (!(flags & FLAGS_LEFT)) {
            if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
                width--;
            }
            while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
                buf[len++] = '0';
            }
        }

        while ((len < precision) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
            buf[len++] = '0';
        }

        if (base == BASE_OCTAL && (len > unpadded_len)) {
            // Since we've written some zeros, we've satisfied the alternative format leading space requirement
            flags &= ~FLAGS_HASH;
        }
    }

    // handle hash
    if (flags & (FLAGS_HASH | FLAGS_POINTER)) {
        if (!(flags & FLAGS_PRECISION) && len && ((len == precision) || (len == width))) {
            // Let's take back some padding digits to fit in what will eventually
            // be the format-specific prefix
            if (unpadded_len < len) {
                len--; // This should suffice for BASE_OCTAL
            }
            if (len && (base == BASE_HEX || base == BASE_BINARY) && (unpadded_len < len)) {
                len--; // ... and an extra one for 0x or 0b
            }
        }
        if ((base == BASE_HEX) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
            buf[len++] = 'x';
        } else if ((base == BASE_HEX) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
            buf[len++] = 'X';
        } else if ((base == BASE_BINARY) && (len < PRINTF_INTEGER_BUFFER_SIZE)) {
            buf[len++] = 'b';
        }
        if (len < PRINTF_INTEGER_BUFFER_SIZE) {
            buf[len++] = '0';
        }
    }

    if (len < PRINTF_INTEGER_BUFFER_SIZE) {
        if (negative) {
            buf[len++] = '-';
        } else if (flags & FLAGS_PLUS) {
            buf[len++] = '+'; // ignore the space if the '+' exists
        } else if (flags & FLAGS_SPACE) {
            buf[len++] = ' ';
        }
    }

    out_rev_(output, putc_function, buf, len, width, flags);
}

// An internal itoa-like function
static void print_integer(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), printf_unsigned_value_t value, bool negative, numeric_base_t base, size_t precision, size_t width, printf_flags_t flags)
{
    char buf[PRINTF_INTEGER_BUFFER_SIZE];
    size_t len = 0U;

    if (!value) {
        if (!(flags & FLAGS_PRECISION)) {
            buf[len++] = '0';
            flags &= ~FLAGS_HASH;
            // We drop this flag this since either the alternative and regular modes of the specifier
            // don't differ on 0 values, or (in the case of octal) we've already provided the special
            // handling for this mode.
        } else if (base == BASE_HEX) {
            flags &= ~FLAGS_HASH;
            // We drop this flag this since either the alternative and regular modes of the specifier
            // don't differ on 0 values
        }
    } else {
        do {
            const char digit = (char)(value % base);
            buf[len++] = (char)(digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit - 10);
            value /= base;
        } while (value && (len < PRINTF_INTEGER_BUFFER_SIZE));
    }

    print_integer_finalization(output, putc_function, buf, len, negative, base, precision, width, flags);
}

#if (CONFIG_VLIBC_FLOAT || CONFIG_VLIBC_FLOAT_EX)

// Stores a fixed-precision representation of a double relative
// to a fixed precision (which cannot be determined by examining this structure)
struct double_components {
    int_fast64_t integral;
    int_fast64_t fractional;
    // ... truncation of the actual fractional part of the double value, scaled
    // by the precision value
    bool is_negative;
};

#define NUM_DECIMAL_DIGITS_IN_INT64_T      18
#define PRINTF_MAX_PRECOMPUTED_POWER_OF_10 NUM_DECIMAL_DIGITS_IN_INT64_T
static const double powers_of_10[NUM_DECIMAL_DIGITS_IN_INT64_T] = {
    1e00, 1e01, 1e02, 1e03, 1e04, 1e05, 1e06, 1e07, 1e08,
    1e09, 1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17
};

#define PRINTF_MAX_SUPPORTED_PRECISION NUM_DECIMAL_DIGITS_IN_INT64_T - 1

// Break up a double number - which is known to be a finite non-negative number -
// into its base-10 parts: integral - before the decimal point, and fractional - after it.
// Taken the precision into account, but does not change it even internally.
static struct double_components get_components(double number, size_t precision)
{
    struct double_components number_;
    number_.is_negative = get_sign_bit(number);
    double abs_number = (number_.is_negative) ? -number : number;
    number_.integral = (int_fast64_t)abs_number;
    double remainder = (abs_number - (double)number_.integral) * powers_of_10[precision];
    number_.fractional = (int_fast64_t)remainder;

    remainder -= (double)number_.fractional;

    if (remainder > 0.5) {
        ++number_.fractional;
        // handle rollover, e.g. case 0.99 with precision 1 is 1.0
        if ((double)number_.fractional >= powers_of_10[precision]) {
            number_.fractional = 0;
            ++number_.integral;
        }
    } else if ((remainder == 0.5) && ((number_.fractional == 0U) || (number_.fractional & 1U))) {
        // if halfway, round up if odd OR if last digit is 0
        ++number_.fractional;
    }

    if (precision == 0U) {
        remainder = abs_number - (double)number_.integral;
        if ((!(remainder < 0.5) || (remainder > 0.5)) && (number_.integral & 1)) {
            // exactly 0.5 and ODD, then round up
            // 1.5 -> 2, but 2.5 -> 2
            ++number_.integral;
        }
    }
    return number_;
}

#if CONFIG_VLIBC_FLOAT_EX
struct scaling_factor {
    double raw_factor;
    bool multiply; // if true, need to multiply by raw_factor; otherwise need to divide by it
};

static double apply_scaling(double num, struct scaling_factor normalization)
{
    return normalization.multiply ? num * normalization.raw_factor : num / normalization.raw_factor;
}

static double unapply_scaling(double normalized, struct scaling_factor normalization)
{
    return normalization.multiply ? normalized / normalization.raw_factor : normalized * normalization.raw_factor;
}

static struct scaling_factor update_normalization(struct scaling_factor sf, double extra_multiplicative_factor)
{
    struct scaling_factor result;
    if (sf.multiply) {
        result.multiply = true;
        result.raw_factor = sf.raw_factor * extra_multiplicative_factor;
    } else {
        int factor_exp2 = get_exp2(get_bit_access(sf.raw_factor));
        int extra_factor_exp2 = get_exp2(get_bit_access(extra_multiplicative_factor));

        // Divide the larger-exponent raw raw_factor by the smaller
        if (PRINTF_ABS(factor_exp2) > PRINTF_ABS(extra_factor_exp2)) {
            result.multiply = false;
            result.raw_factor = sf.raw_factor / extra_multiplicative_factor;
        } else {
            result.multiply = true;
            result.raw_factor = extra_multiplicative_factor / sf.raw_factor;
        }
    }
    return result;
}

static struct double_components get_normalized_components(bool negative, size_t precision, double non_normalized, struct scaling_factor normalization, int floored_exp10)
{
    struct double_components components;
    components.is_negative = negative;
    double scaled = apply_scaling(non_normalized, normalization);

    bool close_to_representation_extremum = ((-floored_exp10 + (int)precision) >= DBL_MAX_10_EXP - 1);
    if (close_to_representation_extremum) {
        // We can't have a normalization factor which also accounts for the precision, i.e. moves
        // some decimal digits into the mantissa, since it's unrepresentable, or nearly unrepresentable.
        // So, we'll give up early on getting extra precision...
        return get_components(negative ? -scaled : scaled, precision);
    }
    components.integral = (int_fast64_t)scaled;
    double remainder = non_normalized - unapply_scaling((double)components.integral, normalization);
    double prec_power_of_10 = powers_of_10[precision];
    struct scaling_factor account_for_precision = update_normalization(normalization, prec_power_of_10);
    double scaled_remainder = apply_scaling(remainder, account_for_precision);
    double rounding_threshold = 0.5;

    components.fractional = (int_fast64_t)scaled_remainder; // when precision == 0, the assigned value should be 0
    scaled_remainder -= (double)components.fractional;      //when precision == 0, this will not change scaled_remainder

    components.fractional += (scaled_remainder >= rounding_threshold);
    if (scaled_remainder == rounding_threshold) {
        // banker's rounding: Round towards the even number (making the mean error 0)
        components.fractional &= ~((int_fast64_t)0x1);
    }
    // handle rollover, e.g. the case of 0.99 with precision 1 becoming (0,100),
    // and must then be corrected into (1, 0).
    // Note: for precision = 0, this will "translate" the rounding effect from
    // the fractional part to the integral part where it should actually be
    // felt (as prec_power_of_10 is 1)
    if ((double)components.fractional >= prec_power_of_10) {
        components.fractional = 0;
        ++components.integral;
    }
    return components;
}
#endif // CONFIG_VLIBC_FLOAT_EX

static void print_broken_up_decimal(
    struct double_components number_, output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), size_t precision,
    size_t width, printf_flags_t flags, char *buf, size_t len)
{
    if (precision != 0U) {
        // do fractional part, as an unsigned number

        size_t count = precision;

        // %g/%G mandates we skip the trailing 0 digits...
        if ((flags & FLAGS_ADAPT_EXP) && !(flags & FLAGS_HASH) && (number_.fractional > 0)) {
            while (true) {
                int_fast64_t digit = number_.fractional % 10U;
                if (digit != 0) {
                    break;
                }
                --count;
                number_.fractional /= 10U;
            }
            // ... and even the decimal point if there are no
            // non-zero fractional part digits (see below)
        }

        if (number_.fractional > 0 || !(flags & FLAGS_ADAPT_EXP) || (flags & FLAGS_HASH)) {
            while (len < PRINTF_DECIMAL_BUFFER_SIZE) {
                --count;
                buf[len++] = (char)('0' + number_.fractional % 10U);
                if (!(number_.fractional /= 10U)) {
                    break;
                }
            }
            // add extra 0s
            while ((len < PRINTF_DECIMAL_BUFFER_SIZE) && (count > 0U)) {
                buf[len++] = '0';
                --count;
            }
            if (len < PRINTF_DECIMAL_BUFFER_SIZE) {
                buf[len++] = '.';
            }
        }
    } else {
        if ((flags & FLAGS_HASH) && (len < PRINTF_DECIMAL_BUFFER_SIZE)) {
            buf[len++] = '.';
        }
    }

    // Write the integer part of the number (it comes after the fractional
    // since the character order is reversed)
    while (len < PRINTF_DECIMAL_BUFFER_SIZE) {
        buf[len++] = (char)('0' + (number_.integral % 10));
        if (!(number_.integral /= 10)) {
            break;
        }
    }

    // pad leading zeros
    if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
        if (width && (number_.is_negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
            width--;
        }
        while ((len < width) && (len < PRINTF_DECIMAL_BUFFER_SIZE)) {
            buf[len++] = '0';
        }
    }

    if (len < PRINTF_DECIMAL_BUFFER_SIZE) {
        if (number_.is_negative) {
            buf[len++] = '-';
        } else if (flags & FLAGS_PLUS) {
            buf[len++] = '+'; // ignore the space if the '+' exists
        } else if (flags & FLAGS_SPACE) {
            buf[len++] = ' ';
        }
    }

    out_rev_(output, putc_function, buf, len, width, flags);
}

// internal ftoa for fixed decimal floating point
static void print_decimal_number(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), double number, size_t precision, size_t width, printf_flags_t flags, char *buf, size_t len)
{
    struct double_components value_ = get_components(number, precision);
    print_broken_up_decimal(value_, output, putc_function, precision, width, flags, buf, len);
}

#if CONFIG_VLIBC_FLOAT_EX

// A floor function - but one which only works for numbers whose
// floor value is representable by an int.
static int bastardized_floor(double x)
{
    if (x >= 0) {
        return (int)x;
    }
    int n = (int)x;
    return (((double)n) == x) ? n : n - 1;
}

// Computes the base-10 logarithm of the input number - which must be an actual
// positive number (not infinity or NaN, nor a sub-normal)
static double log10_of_positive(double positive_number)
{
    // The implementation follows David Gay (https://www.ampl.com/netlib/fp/dtoa.c).
    //
    // Since log_10 ( M * 2^x ) = log_10(M) + x , we can separate the components of
    // our input number, and need only solve log_10(M) for M between 1 and 2 (as
    // the base-2 mantissa is always 1-point-something). In that limited range, a
    // Taylor series expansion of log10(x) should serve us well enough; and we'll
    // take the mid-point, 1.5, as the point of expansion.

    double_with_bit_access dwba = get_bit_access(positive_number);
    // based on the algorithm by David Gay (https://www.ampl.com/netlib/fp/dtoa.c)
    int exp2 = get_exp2(dwba);
    // drop the exponent, so dwba.F comes into the range [1,2)
    dwba.U = (dwba.U & (((double_uint_t)(1) << DOUBLE_STORED_MANTISSA_BITS) - 1U)) |
             ((double_uint_t)DOUBLE_BASE_EXPONENT << DOUBLE_STORED_MANTISSA_BITS);
    double z = (dwba.F - 1.5);
    return (
        // Taylor expansion around 1.5:
        0.1760912590556812420       // Expansion term 0: ln(1.5)            / ln(10)
        + z * 0.2895296546021678851 // Expansion term 1: (M - 1.5)   * 2/3  / ln(10)
#if PRINTF_LOG10_TAYLOR_TERMS > 2
        - z * z * 0.0965098848673892950 // Expansion term 2: (M - 1.5)^2 * 2/9  / ln(10)
#if PRINTF_LOG10_TAYLOR_TERMS > 3
        + z * z * z * 0.0428932821632841311 // Expansion term 2: (M - 1.5)^3 * 8/81 / ln(10)
#endif
#endif
        // exact log_2 of the exponent x, with logarithm base change
        + exp2 * 0.30102999566398119521 // = exp2 * log_10(2) = exp2 * ln(2)/ln(10)
    );
}

static double pow10_of_int(int floored_exp10)
{
    // A crude hack for avoiding undesired behavior with barely-normal or slightly-subnormal values.
    if (floored_exp10 == DOUBLE_MAX_SUBNORMAL_EXPONENT_OF_10) {
        return DOUBLE_MAX_SUBNORMAL_POWER_OF_10;
    }
    // Compute 10^(floored_exp10) but (try to) make sure that doesn't overflow
    double_with_bit_access dwba;
    int exp2 = bastardized_floor(floored_exp10 * 3.321928094887362 + 0.5);
    const double z = floored_exp10 * 2.302585092994046 - exp2 * 0.6931471805599453;
    const double z2 = z * z;
    dwba.U = ((double_uint_t)(exp2) + DOUBLE_BASE_EXPONENT) << DOUBLE_STORED_MANTISSA_BITS;
    // compute exp(z) using continued fractions,
    // see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
    dwba.F *= 1 + 2 * z / (2 - z + (z2 / (6 + (z2 / (10 + z2 / 14)))));
    return dwba.F;
}

static void print_exponential_number(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), double number, size_t precision, size_t width, printf_flags_t flags, char *buf, size_t len)
{
    const bool negative = get_sign_bit(number);
    // This number will decrease gradually (by factors of 10) as we "extract" the exponent out of it
    double abs_number = negative ? -number : number;

    int floored_exp10;
    bool abs_exp10_covered_by_powers_table;
    struct scaling_factor normalization;

    // Determine the decimal exponent
    if (abs_number == 0.0) {
        // TODO: This is a special-case for 0.0 (and -0.0); but proper handling is required for denormals more generally.
        floored_exp10 = 0; // ... and no need to set a normalization factor or check the powers table
    } else {
        double exp10 = log10_of_positive(abs_number);
        floored_exp10 = bastardized_floor(exp10);
        double p10 = pow10_of_int(floored_exp10);
        // correct for rounding errors
        if (abs_number < p10) {
            floored_exp10--;
            p10 /= 10;
        }
        abs_exp10_covered_by_powers_table = PRINTF_ABS(floored_exp10) < PRINTF_MAX_PRECOMPUTED_POWER_OF_10;
        normalization.raw_factor = abs_exp10_covered_by_powers_table ? powers_of_10[PRINTF_ABS(floored_exp10)] : p10;
    }

    // We now begin accounting for the widths of the two parts of our printed field:
    // the decimal part after decimal exponent extraction, and the base-10 exponent part.
    // For both of these, the value of 0 has a special meaning, but not the same one:
    // a 0 exponent-part width means "don't print the exponent"; a 0 decimal-part width
    // means "use as many characters as necessary".

    bool fall_back_to_decimal_only_mode = false;
    if (flags & FLAGS_ADAPT_EXP) {
        int required_significant_digits = (precision == 0) ? 1 : (int)precision;
        // Should we want to fall-back to "%f" mode, and only print the decimal part?
        fall_back_to_decimal_only_mode = (floored_exp10 >= -4 && floored_exp10 < required_significant_digits);
        // Now, let's adjust the precision
        // This also decided how we adjust the precision value - as in "%g" mode,
        // "precision" is the number of _significant digits_, and this is when we "translate"
        // the precision value to an actual number of decimal digits.
        int precision_ = fall_back_to_decimal_only_mode ?
                             (int)precision - 1 - floored_exp10 :
                             (int)precision - 1; // the presence of the exponent ensures only one significant digit comes before the decimal point
        precision = (precision_ > 0 ? (unsigned)precision_ : 0U);
        flags |= FLAGS_PRECISION; // make sure print_broken_up_decimal respects our choice above
    }

    normalization.multiply = (floored_exp10 < 0 && abs_exp10_covered_by_powers_table);
    bool should_skip_normalization = (fall_back_to_decimal_only_mode || floored_exp10 == 0);
    struct double_components decimal_part_components =
        should_skip_normalization ?
            get_components(negative ? -abs_number : abs_number, precision) :
            get_normalized_components(negative, precision, abs_number, normalization, floored_exp10);

    // Account for roll-over, e.g. rounding from 9.99 to 100.0 - which effects
    // the exponent and may require additional tweaking of the parts
    if (fall_back_to_decimal_only_mode) {
        if ((flags & FLAGS_ADAPT_EXP) && floored_exp10 >= -1 && decimal_part_components.integral == powers_of_10[floored_exp10 + 1]) {
            floored_exp10++; // Not strictly necessary, since floored_exp10 is no longer really used
            precision--;
            // ... and it should already be the case that decimal_part_components.fractional == 0
        }
        // TODO: What about rollover strictly within the fractional part?
    } else {
        if (decimal_part_components.integral >= 10) {
            floored_exp10++;
            decimal_part_components.integral = 1;
            decimal_part_components.fractional = 0;
        }
    }

    // the floored_exp10 format is "E%+03d" and largest possible floored_exp10 value for a 64-bit double
    // is "307" (for 2^1023), so we set aside 4-5 characters overall
    size_t exp10_part_width = fall_back_to_decimal_only_mode ? 0U : (PRINTF_ABS(floored_exp10) < 100) ? 4U :
                                                                                                        5U;

    size_t decimal_part_width =
        ((flags & FLAGS_LEFT) && exp10_part_width) ?
            // We're padding on the right, so the width constraint is the exponent part's
            // problem, not the decimal part's, so we'll use as many characters as we need:
            0U :
            // We're padding on the left; so the width constraint is the decimal part's
            // problem. Well, can both the decimal part and the exponent part fit within our overall width?
            ((width > exp10_part_width) ?
                 // Yes, so we limit our decimal part's width.
                 // (Note this is trivially valid even if we've fallen back to "%f" mode)
                 width - exp10_part_width :
                 // No; we just give up on any restriction on the decimal part and use as many
                 // characters as we need
                 0U);

    const size_t printed_exponential_start_pos = output->pos;
    print_broken_up_decimal(decimal_part_components, output, putc_function, precision, decimal_part_width, flags, buf, len);

    if (!fall_back_to_decimal_only_mode) {
        putc_function(output, (flags & FLAGS_UPPERCASE) ? 'E' : 'e');
        print_integer(output,
                      putc_function,
                      ABS_FOR_PRINTING(floored_exp10),
                      floored_exp10 < 0, 10, 0, exp10_part_width - 1,
                      FLAGS_ZEROPAD | FLAGS_PLUS);
        if (flags & FLAGS_LEFT) {
            // We need to right-pad with spaces to meet the width requirement
            while (output->pos - printed_exponential_start_pos < width) {
                putc_function(output, ' ');
            }
        }
    }
}
#endif // CONFIG_VLIBC_FLOAT_EX

static void print_floating_point(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), double value, size_t precision, size_t width, printf_flags_t flags, bool prefer_exponential)
{
    char buf[PRINTF_DECIMAL_BUFFER_SIZE];
    size_t len = 0U;

    // test for special values
    if (value != value) {
        out_rev_(output, putc_function, "nan", 3, width, flags);
        return;
    }
    if (value < -DBL_MAX) {
        out_rev_(output, putc_function, "fni-", 4, width, flags);
        return;
    }
    if (value > DBL_MAX) {
        out_rev_(output, putc_function, (flags & FLAGS_PLUS) ? "fni+" : "fni", (flags & FLAGS_PLUS) ? 4U : 3U, width, flags);
        return;
    }

    if (!prefer_exponential &&
        ((value > PRINTF_FLOAT_NOTATION_THRESHOLD) || (value < -PRINTF_FLOAT_NOTATION_THRESHOLD))) {
        // The required behavior of standard printf is to print _every_ integral-part digit -- which could mean
        // printing hundreds of characters, overflowing any fixed internal buffer and necessitating a more complicated
        // implementation.
#if CONFIG_VLIBC_FLOAT_EX
        print_exponential_number(output, putc_function, value, precision, width, flags, buf, len);
#endif
        return;
    }

    // set default precision, if not set explicitly
    if (!(flags & FLAGS_PRECISION)) {
        precision = VLIBC_DEFAULT_FLOAT_PRECISION;
    }

    // limit precision so that our integer holding the fractional part does not overflow
    while ((len < PRINTF_DECIMAL_BUFFER_SIZE) && (precision > PRINTF_MAX_SUPPORTED_PRECISION)) {
        buf[len++] = '0'; // This respects the precision in terms of result length only
        precision--;
    }

#if CONFIG_VLIBC_FLOAT_EX
    if (prefer_exponential)
        print_exponential_number(output, putc_function, value, precision, width, flags, buf, len);
    else
#endif
        print_decimal_number(output, putc_function, value, precision, width, flags, buf, len);
}

#endif // (CONFIG_VLIBC_FLOAT || CONFIG_VLIBC_FLOAT_EX)

// Advances the format pointer past the flags, and returns the parsed flags
// due to the characters passed
static printf_flags_t parse_flags(const char **format)
{
    printf_flags_t flags = 0U;
    do {
        switch (**format) {
            case '0':
                flags |= FLAGS_ZEROPAD;
                (*format)++;
                break;
            case '-':
                flags |= FLAGS_LEFT;
                (*format)++;
                break;
            case '+':
                flags |= FLAGS_PLUS;
                (*format)++;
                break;
            case ' ':
                flags |= FLAGS_SPACE;
                (*format)++;
                break;
            case '#':
                flags |= FLAGS_HASH;
                (*format)++;
                break;
            default:
                return flags;
        }
    } while (true);
}

// internal vsnprintf - used for implementing _all library functions
// Note: We don't like the C standard's parameter names, so using more informative parameter names
// here instead.
static int _vsnprintf(output_gadget_t *output, void (*putc_function)(output_gadget_t *, char), const char *format, va_list args)
{
    // Note: The library only calls _vsnprintf() with output->pos being 0. However, it is
    // possible to call this function with a non-zero pos value for some "remedial printing".

    while (*format) {
        // format specifier?  %[flags][width][.precision][length]
        if (*format != '%') {
            putc_function(output, *format);
            format++;
            continue;
        } else {
            format++;
        }

        printf_flags_t flags = parse_flags(&format);

        // evaluate width field
        size_t width = 0U;
        if (is_digit_(*format)) {
            width = (size_t)atou_(&format);
        } else if (*format == '*') {
            const int w = va_arg(args, int);
            if (w < 0) {
                flags |= FLAGS_LEFT; // reverse padding
                width = (size_t)-w;
            } else {
                width = (size_t)w;
            }
            format++;
        }

        // evaluate precision field
        size_t precision = 0U;
        if (*format == '.') {
            flags |= FLAGS_PRECISION;
            format++;
            if (is_digit_(*format)) {
                precision = (size_t)atou_(&format);
            } else if (*format == '*') {
                const int precision_ = va_arg(args, int);
                precision = precision_ > 0 ? (size_t)precision_ : 0U;
                format++;
            }
        }

        // evaluate length field
        switch (*format) {
            case 'l':
                flags |= FLAGS_LONG;
                format++;
                if (*format == 'l') {
                    flags |= FLAGS_LONG_LONG;
                    format++;
                }
                break;
            case 'h':
                flags |= FLAGS_SHORT;
                format++;
                if (*format == 'h') {
                    flags |= FLAGS_CHAR;
                    format++;
                }
                break;
            case 't':
                flags |= (sizeof(ptrdiff_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            case 'j':
                flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            case 'z':
                flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            default:
                break;
        }

        // evaluate specifier
        switch (*format) {
            case 'd':
            case 'i':
            case 'u':
            case 'x':
            case 'X':
            case 'o':
            case 'b': {
                if (*format == 'd' || *format == 'i') {
                    flags |= FLAGS_SIGNED;
                }

                numeric_base_t base;
                if (*format == 'x' || *format == 'X') {
                    base = BASE_HEX;
                } else if (*format == 'o') {
                    base = BASE_OCTAL;
                } else if (*format == 'b') {
                    base = BASE_BINARY;
                } else {
                    base = BASE_DECIMAL;
                    flags &= ~FLAGS_HASH; // decimal integers have no alternative presentation
                }

                if (*format == 'X') {
                    flags |= FLAGS_UPPERCASE;
                }

                format++;
                // ignore '0' flag when precision is given
                if (flags & FLAGS_PRECISION) {
                    flags &= ~FLAGS_ZEROPAD;
                }

                if (flags & FLAGS_SIGNED) {
                    // A signed specifier: d, i or possibly I + bit size if enabled

                    if (flags & FLAGS_LONG_LONG) {
#if CONFIG_VLIBC_LONG_LONG
                        const long long value = va_arg(args, long long);
                        print_integer(output, putc_function, ABS_FOR_PRINTING(value), value < 0, base, precision, width, flags);
#endif
                    } else if (flags & FLAGS_LONG) {
                        const long value = va_arg(args, long);
                        print_integer(output, putc_function, ABS_FOR_PRINTING(value), value < 0, base, precision, width, flags);
                    } else {
                        // We never try to interpret the argument as something potentially-smaller than int,
                        // due to integer promotion rules: Even if the user passed a short int, short unsigned
                        // etc. - these will come in after promotion, as int's (or unsigned for the case of
                        // short unsigned when it has the same size as int)
                        const int value =
                            (flags & FLAGS_CHAR)  ? (signed char)va_arg(args, int) :
                            (flags & FLAGS_SHORT) ? (short int)va_arg(args, int) :
                                                    va_arg(args, int);
                        print_integer(output, putc_function, ABS_FOR_PRINTING(value), value < 0, base, precision, width, flags);
                    }
                } else {
                    // An unsigned specifier: u, x, X, o, b

                    flags &= ~(FLAGS_PLUS | FLAGS_SPACE);

                    if (flags & FLAGS_LONG_LONG) {
#if CONFIG_VLIBC_LONG_LONG
                        print_integer(output, putc_function, (printf_unsigned_value_t)va_arg(args, unsigned long long), false, base, precision, width, flags);
#endif
                    } else if (flags & FLAGS_LONG) {
                        print_integer(output, putc_function, (printf_unsigned_value_t)va_arg(args, unsigned long), false, base, precision, width, flags);
                    } else {
                        const unsigned int value =
                            (flags & FLAGS_CHAR)  ? (unsigned char)va_arg(args, unsigned int) :
                            (flags & FLAGS_SHORT) ? (unsigned short int)va_arg(args, unsigned int) :
                                                    va_arg(args, unsigned int);
                        print_integer(output, putc_function, (printf_unsigned_value_t)value, false, base, precision, width, flags);
                    }
                }
                break;
            }

#if CONFIG_VLIBC_FLOAT
            case 'f':
            case 'F':
                if (*format == 'F')
                    flags |= FLAGS_UPPERCASE;
                print_floating_point(output, putc_function, va_arg(args, double), precision, width, flags, PRINTF_PREFER_DECIMAL);
                format++;
                break;
#endif

#if CONFIG_VLIBC_FLOAT_EX
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                if ((*format == 'g') || (*format == 'G'))
                    flags |= FLAGS_ADAPT_EXP;
                if ((*format == 'E') || (*format == 'G'))
                    flags |= FLAGS_UPPERCASE;
                print_floating_point(output, putc_function, va_arg(args, double), precision, width, flags, PRINTF_PREFER_EXPONENTIAL);
                format++;
                break;
#endif

            case 'c': {
                size_t l = 1U;
                // pre padding
                if (!(flags & FLAGS_LEFT)) {
                    while (l++ < width) {
                        putc_function(output, ' ');
                    }
                }
                // char output
                putc_function(output, (char)va_arg(args, int));
                // post padding
                if (flags & FLAGS_LEFT) {
                    while (l++ < width) {
                        putc_function(output, ' ');
                    }
                }
                format++;
                break;
            }

            case 's': {
                const char *p = va_arg(args, char *);
                if (p == NULL) {
                    out_rev_(output, putc_function, ")llun(", 6, width, flags);
                } else {
                    size_t l = strnlen_s_(p, precision ? precision : PRINTF_MAX_POSSIBLE_BUFFER_SIZE);
                    // pre padding
                    if (flags & FLAGS_PRECISION) {
                        l = (l < precision ? l : precision);
                    }
                    if (!(flags & FLAGS_LEFT)) {
                        while (l++ < width) {
                            putc_function(output, ' ');
                        }
                    }
                    // string output
                    while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision)) {
                        putc_function(output, *(p++));
                        --precision;
                    }
                    // post padding
                    if (flags & FLAGS_LEFT) {
                        while (l++ < width) {
                            putc_function(output, ' ');
                        }
                    }
                }
                format++;
                break;
            }

            case 'p': {
                width = sizeof(void *) * 2U + 2; // 2 hex chars per byte + the "0x" prefix
                flags |= FLAGS_ZEROPAD | FLAGS_POINTER;
                uintptr_t value = (uintptr_t)va_arg(args, void *);
                (value == (uintptr_t)NULL) ?
                    out_rev_(output, putc_function, ")lin(", 5, width, flags) :
                    print_integer(output, putc_function, (printf_unsigned_value_t)value, false, BASE_HEX, precision, width, flags);
                format++;
                break;
            }

            case '%':
                putc_function(output, '%');
                format++;
                break;

                // Many people prefer to disable support for %n, as it lets the caller
                // engineer a write to an arbitrary location, of a value the caller
                // effectively controls - which could be a security concern in some cases.
#if CONFIG_VLIBC_WRITEBACK
            case 'n': {
                if (flags & FLAGS_CHAR)
                    *(va_arg(args, char *)) = (char)output->pos;
                else if (flags & FLAGS_SHORT)
                    *(va_arg(args, short *)) = (short)output->pos;
                else if (flags & FLAGS_LONG)
                    *(va_arg(args, long *)) = (long)output->pos;
#if CONFIG_VLIBC_LONG_LONG
                else if (flags & FLAGS_LONG_LONG)
                    *(va_arg(args, long long *)) = (long long int)output->pos;
#endif // CONFIG_VLIBC_LONG_LONG
                else
                    *(va_arg(args, int *)) = (int)output->pos;
                format++;
                break;
            }
#endif // CONFIG_VLIBC_WRITEBACK

            default:
                putc_function(output, *format);
                format++;
                break;
        }
    }

    // termination append \0
    if ((output->stream == NULL) && (output->buffer != NULL) && (output->max > 0)) {
        size_t null_char_pos = output->pos < output->max ? output->pos : output->max - 1;
        output->buffer[null_char_pos] = '\0';
    }

    // return written chars without terminating \0
    return (int)output->pos;
}

/** @addtogroup api **********************************************************
 * @{
 */

/**
 *   @brief
 *   @param  stream
 *   @param  format
 *   @param  arg
 *   @return int
 */
// int vfprintf(FILE *__restrict, const char *__restrict, __VALIST) __attribute__((alias("vlibc_vfprintf")));
int vlibc_vfprintf(VLIBC_FILE *stream, const char *format, va_list arg)
{
    CHECK_FILE(stream, EOF);
    CHECK_FILE(format, EOF);

    output_gadget_t gadget;
    gadget.stream = stream;
    gadget.buffer = NULL;
    gadget.pos = 0;
    gadget.max = PRINTF_MAX_POSSIBLE_BUFFER_SIZE;

    IF_IO_DEV(stream)
    {
        switch (stream->io->vbuf) {
            case _IOFBF:
                if (stream->io->wp == stream->io->ed) {
                    if (__vlibc_io_mem2dev(stream->io, stream->io->bg, stream->io->ed - stream->io->bg) == 0) {
                        errno = EINVAL;
                    }
                    stream->io->wp = stream->io->bg;
                }
                return _vsnprintf(&gadget, putc_via_iodev_fbf, format, arg);
            case _IOLBF:
                if (stream->io->wp == stream->io->ed) {
                    if (__vlibc_io_mem2dev(stream->io, stream->io->bg, stream->io->ed - stream->io->bg) == 0) {
                        errno = EINVAL;
                    }
                    stream->io->wp = stream->io->bg;
                }
                return _vsnprintf(&gadget, putc_via_iodev_lbf, format, arg);
            case _IONBF:
                return _vsnprintf(&gadget, putc_via_iodev_nbf, format, arg);
            default:
                errno = EINVAL;
                return EOF;
        }
    }
    else IF_FILE(stream)
    {
#ifdef CONFIG_VLIBC_FATFS
        char buffer[VLIBC_FBUFSIZ];
        int ret;
        FRESULT fresult;
        size_t bytes;

        gadget.buffer = buffer;
        gadget.max = 0;

        ret = _vsnprintf(&gadget, putc_via_file, format, arg);

        fresult = f_write(stream->file, buffer, gadget.max, &bytes);
        if (fresult != FR_OK) {
            if (fresult <= FR_INVALID_PARAMETER) {
                errno = FRESULT_to_errno[fresult];
            }
        }

        return ret;
#else
        return EOF;
#endif
    }
    else
    {
        errno = EINVAL;
        return EOF;
    }
}

/**
 *   @brief
 *   @param  format
 *   @param  arg
 *   @return int
 */
// int vprintf(const char *, __VALIST) __attribute__((alias("vlibc_vprintf")));
int vlibc_vprintf(const char *format, va_list arg)
{
    return vlibc_vfprintf(vlibc_stdout, format, arg);
}

/**
 *   @brief
 *   @param  str
 *   @param  size
 *   @param  format
 *   @param  arg
 *   @return int
 */
int vsnprintf(char *__restrict, size_t, const char *__restrict, __VALIST) __attribute__((alias("vlibc_vsnprintf")));
int vlibc_vsnprintf(char *str, size_t size, const char *format, va_list arg)
{
    CHECK_FILE(str, EOF);
    CHECK_FILE(format, EOF);

    output_gadget_t gadget;
    gadget.stream = NULL;
    gadget.buffer = str;
    gadget.pos = 0;
    gadget.max = size;
    return _vsnprintf(&gadget, putc_via_buffer, format, arg);
}

/**
 *   @brief
 *   @param  str
 *   @param  format
 *   @param  arg
 *   @return int
 */
// int vsprintf(char *__restrict, const char *__restrict, __VALIST) __attribute__((alias("vlibc_vsprintf")));
int vlibc_vsprintf(char *str, const char *format, va_list arg)
{
    return vlibc_vsnprintf(str, PRINTF_MAX_POSSIBLE_BUFFER_SIZE, format, arg);
}

/**
 *   @brief
 *   @param  stream
 *   @param  format
 *   @param  ...
 *   @return int
 */
// int fprintf(FILE *__restrict, const char *__restrict, ...) __attribute__((alias("vlibc_fprintf")));
int vlibc_fprintf(VLIBC_FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int ret = vlibc_vfprintf(stream, format, args);
    va_end(args);
    return ret;
}

/**
 *   @brief
 *   @param  format
 *   @param  ...
 *   @return int
 */
//int printf(const char *__restrict, ...) __attribute__((alias("vlibc_printf")));
int vlibc_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int ret = vlibc_vprintf(format, args);
    va_end(args);
    return ret;
}

/**
 *   @brief
 *   @param  str
 *   @param  format
 *   @param  ...
 *   @return int
 */
// int sprintf(char *__restrict, const char *__restrict, ...) __attribute__((alias("vlibc_sprintf")));
int vlibc_sprintf(char *str, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int ret = vlibc_vsprintf(str, format, args);
    va_end(args);
    return ret;
}

/**
 *   @brief
 *   @param  str
 *   @param  size
 *   @param  format
 *   @param  ...
 *   @return int
 */
// int snprintf(char *__restrict, size_t, const char *__restrict, ...) __attribute__((alias("vlibc_snprintf")));
int vlibc_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    const int ret = vlibc_vsnprintf(str, size, format, args);
    va_end(args);
    return ret;
}

/**
 * @}
 */
