/**
 * @file gcc.h
 * @brief 
 * 
 * Copyright (c) 2021 Bouffalolab team
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 * 
 */
#ifndef _GCC_H
#define _GCC_H

/*------------------ RealView Compiler -----------------*/
#if defined(__GNUC__)

#define __ASM__             __asm
#define __VOLATILE__        volatile

#define __INLINE__          inline
#define __STATIC__          static
#define __STATIC_INLINE__   static inline

#ifndef  likely
#define likely(x)           __builtin_expect(!!(x), 1)
#endif
#ifndef likely
#define unlikely(x)         __builtin_expect(!!(x), 0)
#endif
#define __UNUSED__          __attribute__((__unused__))
#define __USED__            __attribute__((__used__))
#define __PACKED__          __attribute__((packed))
#define __ALIGNED__(x)      __attribute__((aligned(x)))
#define __PURE__            __attribute__((__pure__))
#define __CONST__           __attribute__((__const__))
#define __NO_RETURN__       __attribute__((__noreturn__))
#define __NAKED__           __attribute__((naked))
#define __WEAK__            __attribute__((weak))
#define SECTION(x)          __attribute__((section(x)))
#define __API__             __declspec(dllexport)

#endif

#ifdef __GNUC__

#define WARN_UNUSED_RET __attribute__ ((warn_unused_result))
#define WEAK __attribute__ ((weak))

#ifndef PACK_START
#define PACK_START
#endif
#ifndef PACK_END
#define PACK_END __attribute__((packed))
#endif
#define NORETURN __attribute__ ((noreturn))

/* alignment value should be a power of 2 */
#define ALIGN(num, align)  MASK(num, (typeof(num))align - 1)

#define ALIGN_2(num)               ALIGN(num, 2)
#define ALIGN_4(num)               ALIGN(num, 4)
#define ALIGN_8(num)               ALIGN(num, 8)
#define ALIGN_16(num)              ALIGN(num, 16)
#define ALIGN_32(num)              ALIGN(num, 32)

#else /* __GNUC__ */

#define WARN_UNUSED_RET
#define WEAK __weak

#define PACK_START __packed
#define PACK_END
#define NORETURN

#endif /* __GNUC__ */

#endif