// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//replacement for gcc built-in functions
#include <stdbool.h>
#include <stdint.h>

#include "bflb_irq.h"

// GCC toolchain will define this pre-processor if "A" extension is supported
#ifndef __riscv_atomic
#define __riscv_atomic 0
#endif

#define HAS_ATOMICS_32 (__riscv_atomic == 1)
#define HAS_ATOMICS_64 ((__riscv_atomic == 1) && (__riscv_xlen == 64))

// Single core SoC: atomics can be implemented using portENTER_CRITICAL_NESTED
// and portEXIT_CRITICAL_NESTED, which disable and enable interrupts.
#define _ATOMIC_ENTER_CRITICAL() bflb_irq_save()

#define _ATOMIC_EXIT_CRITICAL(state) bflb_irq_restore(state)

#ifdef __clang__
// Clang doesn't allow to define "__sync_*" atomics. The workaround is to define function with name "__sync_*_builtin",
// which implements "__sync_*" atomic functionality and use asm directive to set the value of symbol "__sync_*" to the name
// of defined function.

#define CLANG_ATOMIC_SUFFIX(name_) name_##_builtin
#define CLANG_DECLARE_ALIAS(name_)        \
  __asm__(".type " #name_ ", @function\n" \
          ".global " #name_ "\n"          \
          ".equ " #name_ ", " #name_ "_builtin");

#else // __clang__

#define CLANG_ATOMIC_SUFFIX(name_) name_
#define CLANG_DECLARE_ALIAS(name_)

#endif // __clang__

#define ATOMIC_LOAD(n, type)                              \
  type __atomic_load_##n(const type *mem, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();            \
    type ret = *mem;                                      \
    _ATOMIC_EXIT_CRITICAL(state);                         \
    return ret;                                           \
  }

#define ATOMIC_STORE(n, type)                                  \
  void __atomic_store_##n(type *mem, type val, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                 \
    *mem = val;                                                \
    _ATOMIC_EXIT_CRITICAL(state);                              \
  }

#define ATOMIC_EXCHANGE(n, type)                                  \
  type __atomic_exchange_##n(type *mem, type val, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                    \
    type ret = *mem;                                              \
    *mem = val;                                                   \
    _ATOMIC_EXIT_CRITICAL(state);                                 \
    return ret;                                                   \
  }

#define CMP_EXCHANGE(n, type)                                                                                      \
  bool __atomic_compare_exchange_##n(type *mem, type *expect, type desired, bool weak, int success, int failure) { \
    bool ret = false;                                                                                              \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                                                     \
    if (*mem == *expect) {                                                                                         \
      ret = true;                                                                                                  \
      *mem = desired;                                                                                              \
    } else {                                                                                                       \
      *expect = *mem;                                                                                              \
    }                                                                                                              \
    _ATOMIC_EXIT_CRITICAL(state);                                                                                  \
    return ret;                                                                                                    \
  }

#define FETCH_ADD(n, type)                                           \
  type __atomic_fetch_add_##n(type *ptr, type value, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                       \
    type ret = *ptr;                                                 \
    *ptr = *ptr + value;                                             \
    _ATOMIC_EXIT_CRITICAL(state);                                    \
    return ret;                                                      \
  }

#define FETCH_SUB(n, type)                                           \
  type __atomic_fetch_sub_##n(type *ptr, type value, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                       \
    type ret = *ptr;                                                 \
    *ptr = *ptr - value;                                             \
    _ATOMIC_EXIT_CRITICAL(state);                                    \
    return ret;                                                      \
  }

#define FETCH_AND(n, type)                                           \
  type __atomic_fetch_and_##n(type *ptr, type value, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                       \
    type ret = *ptr;                                                 \
    *ptr = *ptr & value;                                             \
    _ATOMIC_EXIT_CRITICAL(state);                                    \
    return ret;                                                      \
  }

#define FETCH_OR(n, type)                                           \
  type __atomic_fetch_or_##n(type *ptr, type value, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                      \
    type ret = *ptr;                                                \
    *ptr = *ptr | value;                                            \
    _ATOMIC_EXIT_CRITICAL(state);                                   \
    return ret;                                                     \
  }

#define FETCH_XOR(n, type)                                           \
  type __atomic_fetch_xor_##n(type *ptr, type value, int memorder) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                       \
    type ret = *ptr;                                                 \
    *ptr = *ptr ^ value;                                             \
    _ATOMIC_EXIT_CRITICAL(state);                                    \
    return ret;                                                      \
  }

#define SYNC_FETCH_OP(op, n, type)                                                \
  type CLANG_ATOMIC_SUFFIX(__sync_fetch_and_##op##_##n)(type * ptr, type value) { \
    return __atomic_fetch_##op##_##n(ptr, value, __ATOMIC_SEQ_CST);               \
  }                                                                               \
  CLANG_DECLARE_ALIAS(__sync_fetch_and_##op##_##n)

#define SYNC_BOOL_CMP_EXCHANGE(n, type)                                                              \
  bool CLANG_ATOMIC_SUFFIX(__sync_bool_compare_and_swap_##n)(type * ptr, type oldval, type newval) { \
    bool ret = false;                                                                                \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                                       \
    if (*ptr == oldval) {                                                                            \
      *ptr = newval;                                                                                 \
      ret = true;                                                                                    \
    }                                                                                                \
    _ATOMIC_EXIT_CRITICAL(state);                                                                    \
    return ret;                                                                                      \
  }                                                                                                  \
  CLANG_DECLARE_ALIAS(__sync_bool_compare_and_swap_##n)

#define SYNC_VAL_CMP_EXCHANGE(n, type)                                                              \
  type CLANG_ATOMIC_SUFFIX(__sync_val_compare_and_swap_##n)(type * ptr, type oldval, type newval) { \
    unsigned state = _ATOMIC_ENTER_CRITICAL();                                                      \
    type ret = *ptr;                                                                                \
    if (*ptr == oldval) {                                                                           \
      *ptr = newval;                                                                                \
    }                                                                                               \
    _ATOMIC_EXIT_CRITICAL(state);                                                                   \
    return ret;                                                                                     \
  }                                                                                                 \
  CLANG_DECLARE_ALIAS(__sync_val_compare_and_swap_##n)

ATOMIC_EXCHANGE(1, uint8_t)
ATOMIC_EXCHANGE(2, uint16_t)
ATOMIC_EXCHANGE(4, uint32_t)

CMP_EXCHANGE(1, uint8_t)
CMP_EXCHANGE(2, uint16_t)
CMP_EXCHANGE(4, uint32_t)

FETCH_ADD(1, uint8_t)
FETCH_ADD(2, uint16_t)
FETCH_ADD(4, uint32_t)

FETCH_SUB(1, uint8_t)
FETCH_SUB(2, uint16_t)
FETCH_SUB(4, uint32_t)

FETCH_AND(1, uint8_t)
FETCH_AND(2, uint16_t)
FETCH_AND(4, uint32_t)

FETCH_OR(1, uint8_t)
FETCH_OR(2, uint16_t)
FETCH_OR(4, uint32_t)

FETCH_XOR(1, uint8_t)
FETCH_XOR(2, uint16_t)
FETCH_XOR(4, uint32_t)

SYNC_FETCH_OP(add, 1, uint8_t)
SYNC_FETCH_OP(add, 2, uint16_t)
SYNC_FETCH_OP(add, 4, uint32_t)

SYNC_FETCH_OP(sub, 1, uint8_t)
SYNC_FETCH_OP(sub, 2, uint16_t)
SYNC_FETCH_OP(sub, 4, uint32_t)

SYNC_FETCH_OP(and, 1, uint8_t)
SYNC_FETCH_OP(and, 2, uint16_t)
SYNC_FETCH_OP(and, 4, uint32_t)

SYNC_FETCH_OP(or, 1, uint8_t)
SYNC_FETCH_OP(or, 2, uint16_t)
SYNC_FETCH_OP(or, 4, uint32_t)

SYNC_FETCH_OP(xor, 1, uint8_t)
SYNC_FETCH_OP(xor, 2, uint16_t)
SYNC_FETCH_OP(xor, 4, uint32_t)

SYNC_BOOL_CMP_EXCHANGE(1, uint8_t)
SYNC_BOOL_CMP_EXCHANGE(2, uint16_t)
SYNC_BOOL_CMP_EXCHANGE(4, uint32_t)

SYNC_VAL_CMP_EXCHANGE(1, uint8_t)
SYNC_VAL_CMP_EXCHANGE(2, uint16_t)
SYNC_VAL_CMP_EXCHANGE(4, uint32_t)

#if !HAS_ATOMICS_64

ATOMIC_LOAD(8, uint64_t)

ATOMIC_STORE(8, uint64_t)

ATOMIC_EXCHANGE(8, uint64_t)

CMP_EXCHANGE(8, uint64_t)

FETCH_ADD(8, uint64_t)

FETCH_SUB(8, uint64_t)

FETCH_AND(8, uint64_t)

FETCH_OR(8, uint64_t)

FETCH_XOR(8, uint64_t)

SYNC_FETCH_OP(add, 8, uint64_t)

SYNC_FETCH_OP(sub, 8, uint64_t)

SYNC_FETCH_OP(and, 8, uint64_t)

SYNC_FETCH_OP(or, 8, uint64_t)

SYNC_FETCH_OP(xor, 8, uint64_t)

SYNC_BOOL_CMP_EXCHANGE(8, uint64_t)

SYNC_VAL_CMP_EXCHANGE(8, uint64_t)

#endif // !HAS_ATOMICS_64
