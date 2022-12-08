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

#ifdef CONFIG_BFLOG
#include "bflog.h"
extern bflog_t __bflog_recorder;
#endif

#ifndef DBG_TAG
#define DBG_TAG ""
#endif

#ifdef CONFIG_LOG_DISABLE
#define LOG_F(...)  (void(0))
#define LOG_E(...)  (void(0))
#define LOG_W(...)  (void(0))
#define LOG_I(...)  (void(0))
#define LOG_D(...)  (void(0))
#define LOG_T(...)  (void(0))
#define LOG_FLUSH() (void(0))
#else

#ifdef CONFIG_BFLOG

#if (CONFIG_LOG_LEVEL >= 0)
#define LOG_F(...) BFLOG_F((void *)&__bflog_recorder, DBG_TAG, __VA_ARGS__)
#else
#define LOG_F(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 1)
#define LOG_E(...) BFLOG_E((void *)&__bflog_recorder, DBG_TAG, __VA_ARGS__)
#else
#define LOG_E(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 2)
#define LOG_W(...) BFLOG_W((void *)&__bflog_recorder, DBG_TAG, __VA_ARGS__)
#else
#define LOG_W(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 3)
#define LOG_I(...) BFLOG_I((void *)&__bflog_recorder, DBG_TAG, __VA_ARGS__)
#else
#define LOG_I(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 4)
#define LOG_D(...) BFLOG_D((void *)&__bflog_recorder, DBG_TAG, __VA_ARGS__)
#else
#define LOG_D(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 5)
#define LOG_T(...) BFLOG_T((void *)&__bflog_recorder, DBG_TAG, __VA_ARGS__)
#else
#define LOG_T(...) ((void)0)
#endif

#define LOG_FLUSH() bflog_flush_s(&__bflog_recorder)

#else

#if (CONFIG_LOG_LEVEL >= 0)
#define LOG_F(...) printf(__VA_ARGS__)
#else
#define LOG_F(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 1)
#define LOG_E(...) printf(__VA_ARGS__)
#else
#define LOG_E(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 2)
#define LOG_W(...) printf(__VA_ARGS__)
#else
#define LOG_W(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 3)
#define LOG_I(...) printf(__VA_ARGS__)
#else
#define LOG_I(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 4)
#define LOG_D(...) printf(__VA_ARGS__)
#else
#define LOG_D(...) ((void)0)
#endif

#if (CONFIG_LOG_LEVEL >= 5)
#define LOG_T(...) printf(__VA_ARGS__)
#else
#define LOG_T(...) ((void)0)
#endif

#define LOG_FLUSH() ((void)0)

#endif
#endif

extern void error_handler(void);

#ifdef CONFIG_ASSERT_DISABLE

#define _ASSERT_PARAM(x) ((void)(0))
#define _ASSERT_FUNC(x)  ((void)(x))
#define _CALL_ERROR()    error_handler()

#else

#define _CALL_ERROR()                       \
    do {                                    \
        printf("(Call Error Handler)\r\n"); \
        LOG_F("Call Error Handler\r\n");    \
        LOG_FLUSH();                        \
        error_handler();                    \
    } while (0)

#define _ASSERT_PARAM(x)                        \
    if ((uint32_t)(x) == 0) {                   \
        printf("(Assertion Faild)\r\n");        \
        printf("(%s)\r\n", (const char *)(#x)); \
        LOG_F("Assertion Faild\r\n");           \
        LOG_F("%s\r\n", (const char *)(#x));    \
        LOG_FLUSH();                            \
        error_handler();                        \
    }

#define _ASSERT_FUNC(x)                             \
    do {                                            \
        if ((uint32_t)(x) == 0) {                   \
            printf("(Assertion Faild)\r\n");        \
            printf("(%s)\r\n", (const char *)(#x)); \
            LOG_F("Assertion Faild\r\n");           \
            LOG_F("%s\r\n", (const char *)(#x));    \
            LOG_FLUSH();                            \
            error_handler();                        \
        }                                           \
    } while (0)
#endif

#endif
