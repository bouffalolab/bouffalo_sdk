/**
 * @file log.h
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

#ifndef _LOG_H
#define _LOG_H

#include <stdint.h>
#include <string.h>

#ifndef DBG_TAG
#define DBG_TAG ""
#endif

#ifdef CONFIG_BFLOG
#include "bflog.h"
extern bflog_t __bflog_recorder;
extern void *__bflog_recorder_pointer;

#undef BFLOG_TAG
#define BFLOG_TAG DBG_TAG
#else
#define BFLOG_DEFINE_TAG(name, _string, enable)
#define BFLOG_EXTERN_TAG(name)
#define BFLOG_GET_TAG(name) NULL
#define BFLOG_TAG           NULL
#endif

#ifdef CONFIG_LOG_DISABLE

#define LOG_F(...)  ((void)0)
#define LOG_RF(...) ((void)0)
#define LOG_E(...)  ((void)0)
#define LOG_RE(...) ((void)0)
#define LOG_W(...)  ((void)0)
#define LOG_RW(...) ((void)0)
#define LOG_I(...)  ((void)0)
#define LOG_RI(...) ((void)0)
#define LOG_D(...)  ((void)0)
#define LOG_RD(...) ((void)0)
#define LOG_T(...)  ((void)0)
#define LOG_RT(...) ((void)0)
#define LOG_FLUSH() ((void)0)

#else

#ifdef CONFIG_BFLOG

#if (CONFIG_LOG_LEVEL >= 0)
#define LOG_F(...)  BFLOG_F((void *)&__bflog_recorder, __VA_ARGS__)
#define LOG_RF(...) BFLOG_RF((void *)&__bflog_recorder, __VA_ARGS__)
#else
#define LOG_F(...)  ((void)0)
#define LOG_RF(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 1)
#define LOG_E(...)  BFLOG_E((void *)&__bflog_recorder, __VA_ARGS__)
#define LOG_RE(...) BFLOG_RE((void *)&__bflog_recorder, __VA_ARGS__)
#else
#define LOG_E(...)  ((void)0)
#define LOG_RE(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 2)
#define LOG_W(...)  BFLOG_W((void *)&__bflog_recorder, __VA_ARGS__)
#define LOG_RW(...) BFLOG_RW((void *)&__bflog_recorder, __VA_ARGS__)
#else
#define LOG_W(...)  ((void)0)
#define LOG_RW(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 3)
#define LOG_I(...)  BFLOG_I((void *)&__bflog_recorder, __VA_ARGS__)
#define LOG_RI(...) BFLOG_RI((void *)&__bflog_recorder, __VA_ARGS__)
#else
#define LOG_I(...)  ((void)0)
#define LOG_RI(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 4)
#define LOG_D(...)  BFLOG_D((void *)&__bflog_recorder, __VA_ARGS__)
#define LOG_RD(...) BFLOG_RD((void *)&__bflog_recorder, __VA_ARGS__)
#else
#define LOG_D(...)  ((void)0)
#define LOG_RD(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 5)
#define LOG_T(...)  BFLOG_T((void *)&__bflog_recorder, __VA_ARGS__)
#define LOG_RT(...) BFLOG_RT((void *)&__bflog_recorder, __VA_ARGS__)
#else
#define LOG_T(...)  ((void)0)
#define LOG_RT(...) ((void)0)
#endif

#define LOG_FLUSH() bflog_flush(&__bflog_recorder)

#else

#if (CONFIG_LOG_LEVEL >= 0)
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define LOG_F(...) printf("[F][" DBG_TAG "] " __VA_ARGS__)
#else
#define LOG_F(...) printf("\033[0;35m[F][" DBG_TAG "] " __VA_ARGS__)
#endif
#define LOG_RF(...) printf(__VA_ARGS__)
#else
#define LOG_F(...)  ((void)0)
#define LOG_RF(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 1)
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define LOG_E(...) printf("[E][" DBG_TAG "] " __VA_ARGS__)
#else
#define LOG_E(...) printf("\033[0;31m[E][" DBG_TAG "] " __VA_ARGS__)
#endif
#define LOG_RE(...) printf(__VA_ARGS__)
#else
#define LOG_E(...)  ((void)0)
#define LOG_RE(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 2)
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define LOG_W(...) printf("[W][" DBG_TAG "] " __VA_ARGS__)
#else
#define LOG_W(...) printf("\033[0;33m[W][" DBG_TAG "] " __VA_ARGS__)
#endif
#define LOG_RW(...) printf(__VA_ARGS__)
#else
#define LOG_W(...)  ((void)0)
#define LOG_RW(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 3)
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define LOG_I(...) printf("[I][" DBG_TAG "] " __VA_ARGS__)
#else
#define LOG_I(...) printf("\033[0m[I][" DBG_TAG "] " __VA_ARGS__)
#endif
#define LOG_RI(...) printf(__VA_ARGS__)
#else
#define LOG_I(...)  ((void)0)
#define LOG_RI(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 4)
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define LOG_D(...) printf("[D][" DBG_TAG "] " __VA_ARGS__)
#else
#define LOG_D(...) printf("\033[0m[D][" DBG_TAG "] " __VA_ARGS__)
#endif
#define LOG_RD(...) printf(__VA_ARGS__)
#else
#define LOG_D(...)  ((void)0)
#define LOG_RD(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 5)
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define LOG_T(...) printf("[T][" DBG_TAG "] " __VA_ARGS__)
#else
#define LOG_T(...) printf("\033[0m[T][" DBG_TAG "] " __VA_ARGS__)
#endif
#define LOG_RT(...) printf(__VA_ARGS__)
#else
#define LOG_T(...)  ((void)0)
#define LOG_RT(...) ((void)0)
#endif

#define LOG_FLUSH() ((void)0)

#endif

#endif

extern void error_handler(void);

#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
#define ASSERT_EXPR
#define ASSERT_VALUE
#define ASSERT_RESET
#else
#define ASSERT_EXPR  "\033[0;34;1m"
#define ASSERT_VALUE "\033[0;1m"
#define ASSERT_RESET "\033[0m"
#endif

#ifdef CONFIG_ASSERT_DISABLE

#define _CALL_ERROR()                            error_handler()
#define _ASSERT_PARAM_MESSAGE(x, ...)            ((void)(0))
#define _ASSERT_FUNC_MESSAGE(x, ...)             ((void)(x))
#define _ASSERT_ZERO_PARAM_MESSAGE(x, ...)       ((void)(0))
#define _ASSERT_ZERO_FUNC_MESSAGE(x, ...)        ((void)(x))
#define _ASSERT_EQUAL_PARAM_MESSAGE(val, x, ...) ((void)(0))
#define _ASSERT_EQUAL_FUNC_MESSAGE(val, x, ...) \
    ((void)(val));                              \
    ((void)(x))

#else

#define _CALL_ERROR()                       \
    do {                                    \
        printf("(Call Error Handler)\r\n"); \
        error_handler();                    \
    } while (0)

#define _ASSERT_PARAM_MESSAGE(x, ...)                                               \
    do {                                                                            \
        if ((uint32_t)(x) == 0) {                                                   \
            printf(__VA_ARGS__);                                                    \
            printf(" at %s:%d\r\n", __FILE__, __LINE__);                            \
            printf(" " ASSERT_EXPR #x ASSERT_VALUE " = false" ASSERT_RESET "\r\n"); \
            error_handler();                                                        \
        }                                                                           \
    } while (0)

#define _ASSERT_FUNC_MESSAGE(x, ...)                                                \
    do {                                                                            \
        if ((uint32_t)(x) == 0) {                                                   \
            printf(__VA_ARGS__);                                                    \
            printf(" at %s:%d\r\n", __FILE__, __LINE__);                            \
            printf(" " ASSERT_EXPR #x ASSERT_VALUE " = false" ASSERT_RESET "\r\n"); \
            error_handler();                                                        \
        }                                                                           \
    } while (0)

#define _ASSERT_ZERO_PARAM_MESSAGE(x, ...)                                           \
    do {                                                                             \
        int _x = (int)(x);                                                           \
        if (!_x == 0) {                                                              \
            printf(__VA_ARGS__);                                                     \
            printf(" at %s:%d\r\n", __FILE__, __LINE__);                             \
            printf(" " ASSERT_EXPR #x ASSERT_VALUE " = %d" ASSERT_RESET "\r\n", _x); \
            error_handler();                                                         \
        }                                                                            \
    } while (0)

#define _ASSERT_ZERO_FUNC_MESSAGE(x, ...)                                            \
    do {                                                                             \
        int _x = (int)(x);                                                           \
        if (!_x == 0) {                                                              \
            printf(__VA_ARGS__);                                                     \
            printf(" at %s:%d\r\n", __FILE__, __LINE__);                             \
            printf(" " ASSERT_EXPR #x ASSERT_VALUE " = %d" ASSERT_RESET "\r\n", _x); \
            error_handler();                                                         \
        }                                                                            \
    } while (0)

#define _ASSERT_EQUAL_PARAM_MESSAGE(val, x, ...)                                                                                  \
    do {                                                                                                                          \
        int _val = (int)(val);                                                                                                    \
        int _x = (int)(x);                                                                                                        \
        if (_val != _x) {                                                                                                         \
            printf(__VA_ARGS__);                                                                                                  \
            printf(" at %s:%d\r\n", __FILE__, __LINE__);                                                                          \
            printf(" " ASSERT_EXPR #val " == " #x ASSERT_VALUE " but " #val " = %d , " #x " = %d" ASSERT_RESET "\r\n", _val, _x); \
            error_handler();                                                                                                      \
        }                                                                                                                         \
    } while (0)

#define _ASSERT_EQUAL_FUNC_MESSAGE(val, x, ...)                                                                                   \
    do {                                                                                                                          \
        int _val = (int)(val);                                                                                                    \
        int _x = (int)(x);                                                                                                        \
        if (_val != _x) {                                                                                                         \
            printf(__VA_ARGS__);                                                                                                  \
            printf(" at %s:%d\r\n", __FILE__, __LINE__);                                                                          \
            printf(" " ASSERT_EXPR #val " == " #x ASSERT_VALUE " but " #val " = %d , " #x " = %d" ASSERT_RESET "\r\n", _val, _x); \
            error_handler();                                                                                                      \
        }                                                                                                                         \
    } while (0)

#endif

#define _ASSERT_TRUE_PARAM_MESSAGE  _ASSERT_PARAM_MESSAGE
#define _ASSERT_TRUE_FUNC_MESSAGE   _ASSERT_FUNC_MESSAGE
#define _ASSERT_FALSE_PARAM_MESSAGE _ASSERT_ZERO_PARAM_MESSAGE
#define _ASSERT_FALSE_FUNC_MESSAGE  _ASSERT_ZERO_FUNC_MESSAGE

#define _ASSERT_PARAM(x)            _ASSERT_PARAM_MESSAGE(x, "Assertion Failed")
#define _ASSERT_FUNC(x)             _ASSERT_FUNC_MESSAGE(x, "Assertion Failed")
#define _ASSERT_TRUE_PARAM(x)       _ASSERT_TRUE_PARAM_MESSAGE(x, "Assertion TRUE Failed")
#define _ASSERT_TRUE_FUNC(x)        _ASSERT_TRUE_FUNC_MESSAGE(x, "Assertion TRUE Failed")
#define _ASSERT_FALSE_PARAM(x)      _ASSERT_FALSE_PARAM_MESSAGE(x, "Assertion FALSE Failed")
#define _ASSERT_FALSE_FUNC(x)       _ASSERT_FALSE_FUNC_MESSAGE(x, "Assertion FALSE Failed")
#define _ASSERT_ZERO_PARAM(x)       _ASSERT_ZERO_PARAM_MESSAGE(x, "Assertion ZERO Failed")
#define _ASSERT_ZERO_FUNC(x)        _ASSERT_ZERO_FUNC_MESSAGE(x, "Assertion ZERO Failed")
#define _ASSERT_EQUAL_PARAM(val, x) _ASSERT_EQUAL_PARAM_MESSAGE(val, x, "Assertion EQUAL Failed")
#define _ASSERT_EQUAL_FUNC(val, x)  _ASSERT_EQUAL_FUNC_MESSAGE(val, x, "Assertion EQUAL Failed")

#define _STATIC_ASSERT              _Static_assert

extern void log_restart(void);
extern void log_start(void);

#include "bflb_dbg.h"

#endif
