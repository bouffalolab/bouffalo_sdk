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

#ifndef _BFLOG_CONF_H
#define _BFLOG_CONF_H

#include "bflog.h"

/*!< 启用LOG, 禁用将使LOG内容不参与编译 */
/*!< log enable */
#define BFLOG_ENABLE

/*!< 全局启用的LOG等级, 小于此等级的LOG内容不参与编译 */
/*!< log enable level */
#define BFLOG_LEVEL_ENABLE         BFLOG_LEVEL_TRACE

/*!< 默认LOG记录器配置的LOG等级, 小于此等级的LOG将不被记录 */
/*!< 可以动态调节LOG记录器等级来调整记录内容 */
/*!< default log record level */
#define BFLOG_LEVEL_DEFAULT        BFLOG_LEVEL_TRACE

/*!< 默认LOG输出器配置的LOG等级, 小于此等级的LOG将不被输出 */
/*!< 可以动态调节LOG输出器等级来调整某个输出器输出的内容 */
/*!< default direct print level */
#define BFLOG_DIRECT_LEVEL_DEFAULT BFLOG_LEVEL_TRACE

/*!< 使能时间戳 */
/*!< enable timestamp to time */
#define BFLOG_TIMESTAMP_ENABLE

/*!< 使能BUFFER输出器, 未完成配置无效果 */
/*!< enable buffer directed output */
#define BFLOG_DIRECT_BUFFER_ENABLE

/*!< 使能流输出器 */
/*!< enable stream directed output */
#define BFLOG_DIRECT_STREAM_ENABLE

/*!< 使能文件输出器 */
/*!< enable file directed output */
#define BFLOG_DIRECT_FILE_ENABLE

/*!< 使能按时间分割的文件输出器 */
/*!< enable file time directed output */
#define BFLOG_DIRECT_FILE_TIME_ENABLE

/*!< 使能按文件大小分割的文件输出器 */
/*!< enable file size directed output */
#define BFLOG_DIRECT_FILE_SIZE_ENABLE

/*!< 使能短文件名 */
/*!< enable short file name */
#define BFLOG_SHORT_FILENAME

/*!< 使能文件名记录, 占用flash高 */
/*!< enable file name record, flash use high */
#define BFLOG_FILENAME_ENABLE

/*!< 使能函数名记录, 占用flash中等 */
/*!< enable function name record, flash use medium */
#define BFLOG_FUNCTION_ENABLE

/*!< 使能文件行数记录, 占用flash低 */
/*!< enable line record, flash use low */
#define BFLOG_LINE_ENABLE

/*!< 行缓冲大小, 使用的是栈上空间, 请确保栈空间足够 */
/*!< 行缓冲设置大小不足时, 一次长LOG输出可能不完整 */
/*!< line buffer size (in stack) */
/*!< flush use 3xline buffer size in stack */
/*!< log   use 1xline buffer size in stack */
/*!< pay attention to prevent stack overflow */
#define BFLOG_LINE_BUFFER_SIZE  256

/*!< 最小文件尺寸分割大小 */
/*!< file size rotate min size */
#define BFLOG_FILE_SIZE_MIN     (1024)

/*!< 最小时间分割大小 */
/*!< file time rotate min interval */
#define BFLOG_FILE_INTERVAL_MIN (60)

/*!< 默认记录器配置的记录功能 */
/*!< 可以动态修改记录器配置调节记录功能, 提高速度 */
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
#define BFLOG_FLAG_DEFAULT      ( \
    (0xff & BFLOG_FLAG_LEVEL) |   \
    (0xff & BFLOG_FLAG_TAG) |     \
    (0xff & BFLOG_FLAG_FUNC) |    \
    (0xff & BFLOG_FLAG_LINE) |    \
    (0xff & BFLOG_FLAG_FILE) |    \
    (0xff & BFLOG_FLAG_CLK) |     \
    (0xff & BFLOG_FLAG_TIME) |    \
    (0x00 & BFLOG_FLAG_THREAD))

/*!< 不同日志等级颜色配置 */
/*!< color config */
#define BFLOG_COLOR_FATAL BFLOG_COLOR_FG_MAGENTA BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_ERROR BFLOG_COLOR_FG_RED BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_WARN  BFLOG_COLOR_FG_YELLOW BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_INFO  BFLOG_COLOR_FG_NONE BFLOG_COLOR_BG_NONE BFLOG_SGR_RESET
#define BFLOG_COLOR_DEBUG BFLOG_COLOR_FG_WHITE BFLOG_COLOR_BG_NONE BFLOG_SGR_NORMAL
#define BFLOG_COLOR_TRACE BFLOG_COLOR_FG_WHITE BFLOG_COLOR_BG_NONE BFLOG_SGR_FAINT

/*!< 简易排版的格式 */
/*!< simple layout */
#if 0
#define BFLOG_SIMPLE_LAYOUT_STRING(_color, _level, _tag, _tm, _msg) \
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
#define BFLOG_SIMPLE_LAYOUT_STRING(_color, _level, _tag, _tm, _msg) \
    "%s"                                                            \
    "[%c:%10lu]"                                                    \
    "[%s:%d]"                                                       \
    "%10s> %s",                                                     \
        (_color),                                                   \
        (_level[0]),                                                \
        ((_msg)->clkl),                                             \
        ((_msg)->file), ((_msg)->line),                             \
        (_tag),                                                     \
        ((_msg)->string)
#endif

/*!< 不同日志等级提示信息配置 */
/*!< level string config */
#define BFLOG_LEVEL_FATAL_STRING "FATL"
#define BFLOG_LEVEL_ERROR_STRING "ERRO"
#define BFLOG_LEVEL_WARN_STRING  "WARN"
#define BFLOG_LEVEL_INFO_STRING  "INFO"
#define BFLOG_LEVEL_DEBUG_STRING "DBUG"
#define BFLOG_LEVEL_TRACE_STRING "TRAC"

#endif
