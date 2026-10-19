/**
 * @file bflb_log_default.h
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

#ifndef _BFLB_LOG_CONF_H
#define _BFLB_LOG_CONF_H

#include "bflb_log.h"

/*!< Enable LOG; disabling it will exclude LOG content from compilation. */
/*!< log enable */
#define BFLB_LOG_ENABLE

/*!< The globally enabled LOG level; LOG content lower than this level will not be included in the compilation. */
/*!< log enable level */
#define BFLB_LOG_LEVEL_ENABLE BFLB_LOG_LEVEL_TRACE

/*!< The LOG level configured for the default LOG logger; LOGs lower than this level will not be recorded */
/*!< The LOG logger level can be dynamically adjusted to modify the recorded content */
/*!< default log record level */
#define BFLB_LOG_LEVEL_DEFAULT BFLB_LOG_LEVEL_TRACE

/*!< The LOG level configured for the default LOG outputter; LOGs lower than this level will not be output  */
/*!< The LOG outputter level can be dynamically adjusted to modify the content output by a specific outputter */
/*!< default direct print level */
#define BFLB_LOG_DIRECT_LEVEL_DEFAULT BFLB_LOG_LEVEL_TRACE

/*!< enable timestamp to time */
#define BFLB_LOG_TIMESTAMP_ENABLE

/*!< Enable the BUFFER outputter; it will have no effect if the configuration is incomplete. */
/*!< enable buffer directed output */
#define BFLB_LOG_DIRECT_BUFFER_ENABLE

/*!< enable stream directed output */
#define BFLB_LOG_DIRECT_STREAM_ENABLE

/*!< enable file directed output */
#define BFLB_LOG_DIRECT_FILE_ENABLE

/*!< Enable the time-split file outputter */
/*!< enable file time directed output */
#define BFLB_LOG_DIRECT_FILE_TIME_ENABLE

/*!< Enable the file outputter split by file size */
/*!< enable file size directed output */
#define BFLB_LOG_DIRECT_FILE_SIZE_ENABLE

/*!< enable short file name */
#define BFLB_LOG_SHORT_FILENAME

/*!< enable file name record, flash use high */
#define BFLB_LOG_FILENAME_ENABLE

/*!< enable function name record, flash use medium */
#define BFLB_LOG_FUNCTION_ENABLE

/*!< enable line record, flash use low */
#define BFLB_LOG_LINE_ENABLE

/*!< Line buffer size, using stack space, please ensure sufficient stack space */
/*!< If the line buffer size is insufficient, a long LOG output may be incomplete */
/*!< line buffer size (in stack) */
/*!< flush use 4xline buffer size in stack */
/*!< log   use 2xline buffer size in stack */
/*!< pay attention to prevent stack overflow */
#define BFLB_LOG_LINE_BUFFER_SIZE 256

/*!< file size rotate min size */
#define BFLB_LOG_FILE_SIZE_MIN (1024)

/*!< file time rotate min interval */
#define BFLB_LOG_FILE_INTERVAL_MIN (60)

/*!< Logging function of the default logger configuration  */
/*!< The logger configuration can be dynamically modified to adjust the logging function and improve speed  */
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
#define BFLB_LOG_FLAG_DEFAULT (    \
    (0xff & BFLB_LOG_FLAG_LEVEL) | \
    (0xff & BFLB_LOG_FLAG_TAG) |   \
    (0xff & BFLB_LOG_FLAG_FUNC) |  \
    (0xff & BFLB_LOG_FLAG_LINE) |  \
    (0xff & BFLB_LOG_FLAG_FILE) |  \
    (0xff & BFLB_LOG_FLAG_CLK) |   \
    (0xff & BFLB_LOG_FLAG_TIME) |  \
    (0x00 & BFLB_LOG_FLAG_THREAD))

/*!< color config */
#define BFLB_LOG_COLOR_FATAL BFLB_LOG_COLOR_FG_MAGENTA BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#define BFLB_LOG_COLOR_ERROR BFLB_LOG_COLOR_FG_RED BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#define BFLB_LOG_COLOR_WARN  BFLB_LOG_COLOR_FG_YELLOW BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
// #define BFLB_LOG_COLOR_INFO  BFLB_LOG_COLOR_FG_GREEN BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#define BFLB_LOG_COLOR_INFO  BFLB_LOG_COLOR_FG_NONE BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_RESET
#define BFLB_LOG_COLOR_DEBUG BFLB_LOG_COLOR_FG_WHITE BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#define BFLB_LOG_COLOR_TRACE BFLB_LOG_COLOR_FG_WHITE BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_FAINT

/*!< simple layout */
#if 1
#define BFLB_LOG_SIMPLE_LAYOUT_STRING(_color, _level, _tag, _tm, _msg) \
    "%s"                                                            \
    "[%s][%10lu][%d-%02d-%02d %02d:%02d:%02d]"                      \
    "[%s:%s:%d]"                                                    \
    "<%s> %s",                                                      \
        (_color),                                                   \
        (_level),                                                   \
        ((_msg)->clkl),                                             \
        (_tm)->year, (_tm)->mon, (_tm)->mday,                       \
        (_tm)->hour, (_tm)->min, (_tm)->sec,                        \
        ((_msg)->file), ((_msg)->func), ((_msg)->line),             \
        (_tag),                                                     \
        ((_msg)->string)
#else
#define BFLB_LOG_SIMPLE_LAYOUT_STRING(_color, _level, _tag, _tm, _msg) \
    "%s"                                                            \
    "[%s][%10lu][%d-%02d-%02d %02d:%02d:%02d]"                      \
    "<%s> %s",                                                      \
        (_color),                                                   \
        (_level),                                                   \
        ((_msg)->clkl),                                             \
        (_tm)->year, (_tm)->mon, (_tm)->mday,                       \
        (_tm)->hour, (_tm)->min, (_tm)->sec,                        \
        (_tag),                                                     \
        ((_msg)->string)
#endif

/*!< level string config */
#define BFLB_LOG_LEVEL_FATAL_STRING "FATL"
#define BFLB_LOG_LEVEL_ERROR_STRING "ERRO"
#define BFLB_LOG_LEVEL_WARN_STRING  "WARN"
#define BFLB_LOG_LEVEL_INFO_STRING  "INFO"
#define BFLB_LOG_LEVEL_DEBUG_STRING "DBUG"
#define BFLB_LOG_LEVEL_TRACE_STRING "TRAC"

#endif
