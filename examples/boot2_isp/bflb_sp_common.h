/**
  ******************************************************************************
  * @file    bflb_sp_common.h
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BFLB_SP_COMMON_H__
#define __BFLB_SP_COMMON_H__

#include "stdint.h"

#ifndef BFLB_SP_FAULT_INJECTION_ENABLE
#define BFLB_SP_FAULT_INJECTION_ENABLE 0
#endif

#if defined(BL702L)
#define BFLB_BOOT2_XZ_MALLOC_BUF_SIZE 40 * 1024
#else
#define BFLB_BOOT2_XZ_MALLOC_BUF_SIZE 80 * 1024
#endif


#if BFLB_SP_FAULT_INJECTION_ENABLE

#define FIH_POSITIVE_VALUE    0x5555AAAA
#define FIH_NEGATIVE_VALUE    0xAAAA5555
#define _FIH_MASK_VALUE       0xA5C35A3C

typedef struct {
    volatile int32_t val;
    volatile int32_t msk;
} fih_int;

typedef fih_int fih_ret;

#define FIH_INT_INIT(x)       ((fih_ret){ (x), (x) ^ _FIH_MASK_VALUE })
#define FIH_SUCCESS           FIH_INT_INIT(FIH_POSITIVE_VALUE)
#define FIH_FAILURE           FIH_INT_INIT(FIH_NEGATIVE_VALUE)

void fih_panic_loop(void);
int32_t fih_int_decode(fih_ret value);
fih_ret fih_ret_encode_status(int32_t ret);
int fih_eq_encoded(fih_ret left, fih_ret right);
int fih_not_eq_encoded(fih_ret left, fih_ret right);
void fih_set_encoded(fih_ret *slot, fih_ret value);
void fih_call_prepare_ret(fih_ret *slot);
fih_ret fih_ret_validate(fih_ret value);

#ifndef FIH_EQ
#define FIH_EQ(x, y) \
    fih_eq_encoded((x), (y))
#endif

#ifndef FIH_NOT_EQ
#define FIH_NOT_EQ(x, y) \
    fih_not_eq_encoded((x), (y))
#endif

#ifndef FIH_SET
#define FIH_SET(x, y)                         \
    do {                                      \
        fih_set_encoded(&(x), (y));           \
    } while (0)
#endif

#ifndef FIH_DECLARE
#define FIH_DECLARE(var, val)                 \
    fih_ret var;                              \
    FIH_SET(var, val)
#endif

#ifndef FIH_CALL
#define FIH_CALL(f, ret, ...)                 \
    do {                                      \
        fih_call_prepare_ret(&(ret));         \
        FIH_SET((ret), f(__VA_ARGS__));       \
    } while (0)
#endif

#ifndef FIH_RET
#define FIH_RET(ret)                          \
    do {                                      \
        return fih_ret_validate((ret));       \
    } while (0)
#endif

#ifndef FIH_PANIC
#define FIH_PANIC fih_panic_loop()
#endif

#else

typedef int32_t fih_ret;

#define FIH_INT_INIT(x)       (x)
#define FIH_SUCCESS           0
#define FIH_FAILURE           (-1)

void fih_panic_loop(void);
int32_t fih_int_decode(fih_ret value);
fih_ret fih_ret_encode_status(int32_t ret);

#ifndef FIH_EQ
#define FIH_EQ(x, y) ((x) == (y))
#endif

#ifndef FIH_NOT_EQ
#define FIH_NOT_EQ(x, y) ((x) != (y))
#endif

#ifndef FIH_SET
#define FIH_SET(x, y)                         \
    do {                                      \
        (x) = (y);                            \
    } while (0)
#endif

#ifndef FIH_DECLARE
#define FIH_DECLARE(var, val) fih_ret var = (val)
#endif

#ifndef FIH_CALL
#define FIH_CALL(f, ret, ...)                 \
    do {                                      \
        (ret) = f(__VA_ARGS__);               \
    } while (0)
#endif

#ifndef FIH_RET
#define FIH_RET(ret)        \
    do {                    \
        return (ret);       \
    } while (0)
#endif

#ifndef FIH_PANIC
#define FIH_PANIC fih_panic_loop()
#endif

#endif

void bflb_sp_dump_data(void *datain, int len);
void bflb_sp_boot2_jump_entry(void);
int32_t bflb_sp_mediaboot_pre_jump(void);
uint8_t bflb_sp_boot2_get_feature_flag(void);

extern uint32_t g_anti_rollback_flag[3];
extern uint32_t g_anti_ef_en,g_anti_ef_app_ver;
extern uint8_t g_malloc_buf[BFLB_BOOT2_XZ_MALLOC_BUF_SIZE];

#endif /* __BFLB_SP_COMMON_H__ */
