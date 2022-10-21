/**
 * @file bflog_default.h
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

#ifndef _BFLOG_USER_H
#define _BFLOG_USER_H

#include "vlibc_stdio.h"
#include "bflog.h"

/*!< param check enable */
#define BFLOG_DEBUG_ENABLE

/*!< log enable */
#define BFLOG_ENABLE

/*!< log enable level */
#define BFLOG_LEVEL_ENABLE BFLOG_LEVEL_TRACE

/*!< default log record level */
#define BFLOG_LEVEL_DEFAULT BFLOG_LEVEL_TRACE

/*!< default direct print level */
#define BFLOG_DIRECT_LEVEL_DEFAULT BFLOG_LEVEL_TRACE

/*!< enable timestamp to time */
#define BFLOG_TIMESTAMP_ENABLE

/*!< enable buffer directed output */
#define BFLOG_DIRECT_BUFFER_ENABLE

/*!< enable stream directed output */
#define BFLOG_DIRECT_STREAM_ENABLE

/*!< enable file directed output */
#define BFLOG_DIRECT_FILE_ENABLE

/*!< enable file time directed output */
#define BFLOG_DIRECT_FILE_TIME_ENABLE

/*!< enable file size directed output */
#define BFLOG_DIRECT_FILE_SIZE_ENABLE

/*!< enable short file name */
#define BFLOG_SHORT_FILENAME

/*!< enable file name record, flash use high */
#define BFLOG_FILENAME_ENABLE

/*!< enable function name record, flash use medium */
#define BFLOG_FUNCTION_ENABLE

/*!< enable line record, flash use low */
#define BFLOG_LINE_ENABLE

/*!< line buffer size (in stack) */
/*!< flush use 4xline buffer size in stack */
/*!< log   use 2xline buffer size in stack */
/*!< pay attention to prevent stack overflow */
#define BFLOG_LINE_BUFFER_SIZE 256

/*!< file size rotate min size */
#define BFLOG_FILE_SIZE_MIN (1024)

/*!< file time rotate min interval */
#define BFLOG_FILE_INTERVAL_MIN (60)

/*!< default record flag config */
/*!< |  item | time occupancy | */
/*!< | level |            low | */
/*!< |   tag |            low | */
/*!< |  func |         medium | */
/*!< |  line |            low | */
/*!< |  file |      very high | */
/*!< | clock |         medium | */
/*!< |  time |           high | */
/*!< |thread |         medium | */
#define BFLOG_FLAG_DEFAULT (    \
    (0xff & BFLOG_FLAG_LEVEL) | \
    (0xff & BFLOG_FLAG_TAG) |   \
    (0xff & BFLOG_FLAG_FUNC) |  \
    (0xff & BFLOG_FLAG_LINE) |  \
    (0xff & BFLOG_FLAG_FILE) |  \
    (0xff & BFLOG_FLAG_CLK) |   \
    (0xff & BFLOG_FLAG_TIME) |  \
    (0x00 & BFLOG_FLAG_THREAD))

/*!< color config */
#define BFLOG_COLOR_FATAL BFLOG_COLOR_FG_MAGENTA BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_ERROR BFLOG_COLOR_FG_RED BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_WARN  BFLOG_COLOR_FG_YELLOW BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_INFO  BFLOG_COLOR_FG_GREEN BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_DEBUG BFLOG_COLOR_FG_WHITE BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_TRACE BFLOG_COLOR_FG_WHITE BFLOG_COLOR_BG_NONE BFLOG_SGR_FAINT

/*!< simple layout */
#define LAYOUT_USE 1

#if (LAYOUT_USE == 0)
#define BFLOG_SIMPLE_LAYOUT_STRING(_color, _level, _tm, _msg) \
    "%s"                                                      \
    "[%s][%10lu][%d-%02d-%02d %02d:%02d:%02d]"                \
    "[%s:%s:%d]"                                              \
    "<%s> %s",                                                \
        (_color),                                             \
        (_level),                                             \
        ((_msg)->clkl),                                       \
        (_tm)->year, (_tm)->mon, (_tm)->mday,                 \
        (_tm)->hour, (_tm)->min, (_tm)->sec,                  \
        ((_msg)->file), ((_msg)->func), ((_msg)->line),       \
        ((_msg)->tag),                                        \
        ((_msg)->string)
#elif (LAYOUT_USE == 1)
#define BFLOG_SIMPLE_LAYOUT_STRING(_color, _level, _tm, _msg) \
    "%s"                                                      \
    "[%s][%10lu][%d-%02d-%02d %02d:%02d:%02d]"                \
    "<%s> %s",                                                \
        (_color),                                             \
        (_level),                                             \
        ((_msg)->clkl),                                       \
        (_tm)->year, (_tm)->mon, (_tm)->mday,                 \
        (_tm)->hour, (_tm)->min, (_tm)->sec,                  \
        ((_msg)->tag),                                        \
        ((_msg)->string)
#else
#define BFLOG_SIMPLE_LAYOUT_STRING(_color, _level, _tm, _msg) \
    "%s"                                                      \
    "[%s][%lu]"                                               \
    " %s",                                                    \
        (_color),                                             \
        (_level),                                             \
        ((_msg)->clkl),                                       \
        ((_msg)->string)
#endif

/*!< level string config */
#define BFLOG_LEVEL_FATAL_STRING "FATL"
#define BFLOG_LEVEL_ERROR_STRING "ERRO"
#define BFLOG_LEVEL_WARN_STRING  "WARN"
#define BFLOG_LEVEL_INFO_STRING  "INFO"
#define BFLOG_LEVEL_DEBUG_STRING "DBUG"
#define BFLOG_LEVEL_TRACE_STRING "TRAC"

#define bflog_fopen     vlibc_fopen
#define bflog_fclose    vlibc_fclose
#define bflog_fwrite    vlibc_fwrite
#define bflog_ftell     vlibc_ftell
#define bflog_fflush    vlibc_fflush
#define bflog_snprintf  vlibc_snprintf
#define bflog_vsnprintf vlibc_vsnprintf
#define bflog_remove    vlibc_remove
#define bflog_rename    vlibc_rename

#define bflog_memcpy memcpy

#endif
