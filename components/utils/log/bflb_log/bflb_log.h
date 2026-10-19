/**
 * @file bflb_log.h
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
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

#ifndef _BFLB_LOG_H
#define _BFLB_LOG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if __has_include("bflb_log_conf_user.h")
#include "bflb_log_conf_user.h"
#else
#include "bflb_log_conf.h"
#endif

/** @addtogroup BFLB_LOG_CSI Control Sequence Introducer
 * @{
 */
#define BFLB_LOG_CSI_START      "\033["
#define BFLB_LOG_CSI_CUU        "A"
#define BFLB_LOG_CSI_CUD        "B"
#define BFLB_LOG_CSI_CUF        "C"
#define BFLB_LOG_CSI_CUB        "D"
#define BFLB_LOG_CSI_CNL        "E"
#define BFLB_LOG_CSI_CPL        "F"
#define BFLB_LOG_CSI_CHA        "G"
#define BFLB_LOG_CSI_CUP        "H"
#define BFLB_LOG_CSI_ED         "J"
#define BFLB_LOG_CSI_EL         "K"
#define BFLB_LOG_CSI_SU         "S"
#define BFLB_LOG_CSI_SD         "T"
#define BFLB_LOG_CSI_SGR        "m"
/**
 * @}
 */

/** @addtogroup BFLB_LOG_SGR Select Graphic Rendition
 * @{
 */
#define BFLB_LOG_SGR_RESET      "0"
#define BFLB_LOG_SGR_BOLD       "1"
#define BFLB_LOG_SGR_FAINT      "2"
#define BFLB_LOG_SGR_ITALICS    "3"
#define BFLB_LOG_SGR_UNDERLINE  "4"
#define BFLB_LOG_SGR_BLINKS     "5"
#define BFLB_LOG_SGR_BLINKR     "6"
#define BFLB_LOG_SGR_REVERSE    "7"
#define BFLB_LOG_SGR_HIDE       "8"
#define BFLB_LOG_SGR_STRIKE     "9"
#define BFLB_LOG_SGR_NORMAL     "22"
#define BFLB_LOG_SGR_FG_BLACK   "30"
#define BFLB_LOG_SGR_FG_RED     "31"
#define BFLB_LOG_SGR_FG_GREEN   "32"
#define BFLB_LOG_SGR_FG_YELLOW  "33"
#define BFLB_LOG_SGR_FG_BLUE    "34"
#define BFLB_LOG_SGR_FG_MAGENTA "35"
#define BFLB_LOG_SGR_FG_CYAN    "36"
#define BFLB_LOG_SGR_FG_WHITE   "37"
#define BFLB_LOG_SGR_BG_BLACK   "40"
#define BFLB_LOG_SGR_BG_RED     "41"
#define BFLB_LOG_SGR_BG_GREEN   "42"
#define BFLB_LOG_SGR_BG_YELLOW  "43"
#define BFLB_LOG_SGR_BG_BLUE    "44"
#define BFLB_LOG_SGR_BG_MAGENTA "45"
#define BFLB_LOG_SGR_BG_CYAN    "46"
#define BFLB_LOG_SGR_BG_WHITE   "47"
/**
 * @}
 */

/** @addtogroup BFLB_LOG_COLOR
 * @{
 */
#define BFLB_LOG_COLOR_START    BFLB_LOG_CSI_START
#define BFLB_LOG_COLOR_END      BFLB_LOG_CSI_SGR
#define BFLB_LOG_CLOLR_SEP      ";"
#define BFLB_LOG_COLOR_DEFAULT
#define BFLB_LOG_COLOR_RESET BFLB_LOG_SGR_RESET BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_NONE
#define BFLB_LOG_COLOR_FG_BLACK   BFLB_LOG_SGR_FG_BLACK BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_RED     BFLB_LOG_SGR_FG_RED BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_GREEN   BFLB_LOG_SGR_FG_GREEN BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_YELLOW  BFLB_LOG_SGR_FG_YELLOW BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_BLUE    BFLB_LOG_SGR_FG_BLUE BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_MAGENTA BFLB_LOG_SGR_FG_MAGENTA BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_CYAN    BFLB_LOG_SGR_FG_CYAN BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_FG_WHITE   BFLB_LOG_SGR_FG_WHITE BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_NONE
#define BFLB_LOG_COLOR_BG_BLACK   BFLB_LOG_SGR_BG_BLACK BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_RED     BFLB_LOG_SGR_BG_RED BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_GREEN   BFLB_LOG_SGR_BG_GREEN BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_YELLOW  BFLB_LOG_SGR_BG_YELLOW BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_BLUE    BFLB_LOG_SGR_BG_BLUE BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_MAGENTA BFLB_LOG_SGR_BG_MAGENTA BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_CYAN    BFLB_LOG_SGR_BG_CYAN BFLB_LOG_CLOLR_SEP
#define BFLB_LOG_COLOR_BG_WHITE   BFLB_LOG_SGR_BG_WHITE BFLB_LOG_CLOLR_SEP
/**
 * @}
 */

/** @addtogroup BFLB_LOG_COLOR_CONTROL
 * @{
 */
#ifndef BFLB_LOG_COLOR_FATAL
#define BFLB_LOG_COLOR_FATAL BFLB_LOG_COLOR_FG_MAGENTA BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_BLINKS
#endif

#ifndef BFLB_LOG_COLOR_ERROR
#define BFLB_LOG_COLOR_ERROR BFLB_LOG_COLOR_FG_RED BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#endif

#ifndef BFLB_LOG_COLOR_WARN
#define BFLB_LOG_COLOR_WARN BFLB_LOG_COLOR_FG_YELLOW BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#endif

#ifndef BFLB_LOG_COLOR_INFO
#define BFLB_LOG_COLOR_INFO BFLB_LOG_COLOR_FG_NONE BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_RESET
#endif

#ifndef BFLB_LOG_COLOR_DEBUG
#define BFLB_LOG_COLOR_DEBUG BFLB_LOG_COLOR_FG_WHITE BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_NORMAL
#endif

#ifndef BFLB_LOG_COLOR_TRACE
#define BFLB_LOG_COLOR_TRACE BFLB_LOG_COLOR_FG_WHITE BFLB_LOG_COLOR_BG_NONE BFLB_LOG_SGR_FAINT
#endif

/**
 * @}
 */

/** @addtogroup BFLB_LOG_LEVEL_STRING
 * @{
 */
#ifndef BFLB_LOG_LEVEL_FATAL_STRING
#define BFLB_LOG_LEVEL_FATAL_STRING "FATL"
#endif

#ifndef BFLB_LOG_LEVEL_ERROR_STRING
#define BFLB_LOG_LEVEL_ERROR_STRING "ERRO"
#endif

#ifndef BFLB_LOG_LEVEL_WARN_STRING
#define BFLB_LOG_LEVEL_WARN_STRING "WARN"
#endif

#ifndef BFLB_LOG_LEVEL_INFO_STRING
#define BFLB_LOG_LEVEL_INFO_STRING "INFO"
#endif

#ifndef BFLB_LOG_LEVEL_DEBUG_STRING
#define BFLB_LOG_LEVEL_DEBUG_STRING "DBUG"
#endif

#ifndef BFLB_LOG_LEVEL_TRACE_STRING
#define BFLB_LOG_LEVEL_TRACE_STRING "TRAC"
#endif
/**
 * @}
 */

/** @addtogroup BFLB_LOG_LEVEL
 * @{
 */
#define BFLB_LOG_LEVEL_FATAL           0x00 /*!< level fatal, create a panic */
#define BFLB_LOG_LEVEL_ERROR           0x01 /*!< level error                 */
#define BFLB_LOG_LEVEL_WARN            0x02 /*!< level warning               */
#define BFLB_LOG_LEVEL_INFO            0x03 /*!< level information           */
#define BFLB_LOG_LEVEL_DEBUG           0x04 /*!< level debug                 */
#define BFLB_LOG_LEVEL_TRACE           0x05 /*!< level trace information     */
#define BFLB_LOG_LEVEL_MASK            0x7F /*!< level mask */
#define BFLB_LOG_LEVEL_RAW             0x80 /*!< level raw bit */
/**
 * @}
 */

/** @addtogroup BFLB_LOG_FLAG
 * @{
 */
#define BFLB_LOG_FLAG_LEVEL            ((uint8_t)0x01) /*!< supported print level     */
#define BFLB_LOG_FLAG_TAG              ((uint8_t)0x02) /*!< supported record tag      */
#define BFLB_LOG_FLAG_FUNC             ((uint8_t)0x04) /*!< supported record function */
#define BFLB_LOG_FLAG_LINE             ((uint8_t)0x08) /*!< supported record line     */
#define BFLB_LOG_FLAG_FILE             ((uint8_t)0x10) /*!< supported record file     */
#define BFLB_LOG_FLAG_CLK              ((uint8_t)0x20) /*!< supported record clock    */
#define BFLB_LOG_FLAG_TIME             ((uint8_t)0x40) /*!< supported record time     */
#define BFLB_LOG_FLAG_THREAD           ((uint8_t)0x80) /*!< supported record thread   */
/**
 * @}
 */

/** @addtogroup BFLB_LOG_BASE_MARCO
 * @{
 */

/** @addtogroup BFLB_LOG_STATUS
 * @{
 */
#define BFLB_LOG_STATUS_ILLEGAL        ((uint8_t)0)
#define BFLB_LOG_STATUS_READY          ((uint8_t)1)
#define BFLB_LOG_STATUS_RUNNING        ((uint8_t)2)
#define BFLB_LOG_STATUS_SUSPEND        ((uint8_t)3)
/**
 * @}
 */

/** @addtogroup BFLB_LOG_MODE
 * @{
 */
#define BFLB_LOG_MODE_SYNC             ((uint8_t)0x00)
#define BFLB_LOG_MODE_ASYNC            ((uint8_t)0x01)
/**
 * @}
 */

/** @addtogroup BFLB_LOG_COMMAND
 * @{
 */
#define BFLB_LOG_CMD_ILLEGAL           ((uint32_t)0x00)
#define BFLB_LOG_CMD_FLAG              ((uint32_t)0x01)
#define BFLB_LOG_CMD_LEVEL             ((uint32_t)0x02)
#define BFLB_LOG_CMD_QUEUE_POOL        ((uint32_t)0x03)
#define BFLB_LOG_CMD_QUEUE_SIZE        ((uint32_t)0x04)
#define BFLB_LOG_CMD_QUEUE_RST         ((uint32_t)0x05)
#define BFLB_LOG_CMD_ENTER_CRITICAL    ((uint32_t)0x06)
#define BFLB_LOG_CMD_EXIT_CRITICAL     ((uint32_t)0x07)
#define BFLB_LOG_CMD_FLUSH_NOTICE      ((uint32_t)0x08)
#define BFLB_LOG_CMD_MODE              ((uint32_t)0x09)
/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup BFLB_LOG_DIRECT_MARCO
 * @{
 */

/** @addtogroup BFLB_LOG_DIRECT_STATUS
 * @{
 */
#define BFLB_LOG_DIRECT_STATUS_ILLEGAL ((uint8_t)0x00)
#define BFLB_LOG_DIRECT_STATUS_READY   ((uint8_t)0x01)
#define BFLB_LOG_DIRECT_STATUS_RUNNING ((uint8_t)0x02)
#define BFLB_LOG_DIRECT_STATUS_SUSPEND ((uint8_t)0x03)
#define BFLB_LOG_DIRECT_STATUS_INIT    ((uint8_t)0x04)
#define BFLB_LOG_DIRECT_STATUS_IOERR   ((uint8_t)0x05)
/**
 * @}
 */

/** @addtogroup BFLB_LOG_DIRECT_COMMAND
 * @{
 */
#define BFLB_LOG_DIRECT_CMD_ILLEGAL    ((uint32_t)0x00)
#define BFLB_LOG_DIRECT_CMD_LEVEL      ((uint32_t)0x02)
#define BFLB_LOG_DIRECT_CMD_LOCK       ((uint32_t)0x06)
#define BFLB_LOG_DIRECT_CMD_UNLOCK     ((uint32_t)0x07)
#define BFLB_LOG_DIRECT_CMD_COLOR      ((uint32_t)0x0A)
/**
 * @}
 */

/** @addtogroup BFLB_LOG_DIRECT_TYPE
 * @{
 */
#define BFLB_LOG_DIRECT_TYPE_ILLEGAL   ((uint8_t)0x00)
#define BFLB_LOG_DIRECT_TYPE_BUFFER    ((uint8_t)0x01)
#define BFLB_LOG_DIRECT_TYPE_STREAM    ((uint8_t)0x02)
#define BFLB_LOG_DIRECT_TYPE_FILE      ((uint8_t)0x03)
#define BFLB_LOG_DIRECT_TYPE_FILE_TIME ((uint8_t)0x04)
#define BFLB_LOG_DIRECT_TYPE_FILE_SIZE ((uint8_t)0x05)
/**
 * @}
 */

/** @addtogroup BFLB_LOG_DIRECT_COLOR
 * @{
 */
#define BFLB_LOG_DIRECT_COLOR_DISABLE  ((uint8_t)0)
#define BFLB_LOG_DIRECT_COLOR_ENABLE   ((uint8_t)1)
/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup BFLB_LOG_LAYOUT_MARCO
 * @{
 */

/** @addtogroup BFLB_LOG_LAYOUT_STATUS
 * @{
 */
#define BFLB_LOG_LAYOUT_STATUS_ILLEGAL ((uint8_t)0x00)
#define BFLB_LOG_LAYOUT_STATUS_READY   ((uint8_t)0x01)
#define BFLB_LOG_LAYOUT_STATUS_INIT    ((uint8_t)0x04)
/**
 * @}
 */

/** @addtogroup BFLB_LOG_LAYOUT_TYPE
 * @{
 */
#define BFLB_LOG_LAYOUT_TYPE_SIMPLE    ((uint8_t)0)
#define BFLB_LOG_LAYOUT_TYPE_FORMAT    ((uint8_t)1)
#define BFLB_LOG_LAYOUT_TYPE_YAML      ((uint8_t)2)
/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup BFLB_LOG_STRUCT_TIMESTAMP
 * @{
 */

typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t wday;
} bflb_log_tm_t;

/**
 * @}
 */

/** @addtogroup BFLB_LOG_STRUCT
 * @{
 */

/**
 *   @brief         dual list
 */
struct _bflb_log_list {
    struct _bflb_log_list *next;
    struct _bflb_log_list *prev;
};

/**
 *   @brief         tag
 */
struct _bflb_log_tag {
    char *tag;
    /*!< max 32 filter */
    uint32_t en;
};

/**
 *   @brief         message
 */
struct _bflb_log_msg {
    union {
        uint32_t head; /*!< first 4byte */
        struct
        {
            uint16_t size; /*!< msg total size */
            uint8_t level; /*!< msg level */
            uint8_t zero;  /*!< always zero */
        };
    };

    uint32_t time;    /*!< rtc timestamp */
    union {
        uint64_t clk; /*!< cpu clock tick */
        struct
        {
            uint32_t clkl; /*!< low 4byte */
            uint32_t clkh; /*!< high 4byte */
        };
    };

    uint32_t line;      /*!< msg line */
    const char *func;   /*!< msg function, must be static string, only record pointer */
    const char *file;   /*!< msg file,     must be static string, only record pointer */
    void *tag;          /*!< msg tag */
    const char *thread; /*!< msg thread,   must be static string, only record pointer */
    char string[0];     /*!< msg string */
};

/**
 *   @brief         recorder base type
 */
typedef struct
{
    struct _bflb_log_list direct; /*!< dlist */
    int (*enter_critical)(void);
    int (*exit_critical)(void);
    int (*flush_notice)(void);

    uint8_t status;
    uint8_t flags;
    uint8_t level;
    uint8_t mode;

    uint32_t filter;

    struct
    {
        uint16_t free;
        uint16_t size;
        uint16_t wpos;
        uint16_t rpos;
        void *pool;
    } queue;
} bflb_log_t;

#define _BFLB_LOG_STRUCT_LAYOUT_EXTENDS \
    uint8_t status;                  \
    uint8_t _rsvd0;                  \
    uint8_t _rsvd1;                  \
    uint8_t type;

/**
 *   @brief         layout base type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_LAYOUT_EXTENDS
} bflb_log_layout_t;

/**
 *   @brief         layout simple type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_LAYOUT_EXTENDS
} bflb_log_layout_simple_t;

/**
 *   @brief         layout format type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_LAYOUT_EXTENDS
    int (*snprintf)(void *ptr, uint16_t size, char *color, char *level, char *tag, bflb_log_tm_t *tm, struct _bflb_log_msg *msg);
} bflb_log_layout_format_t;

/**
 *   @brief         layout yaml type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_LAYOUT_EXTENDS
} bflb_log_layout_yaml_t;

#define _BFLB_LOG_STRUCT_DIRECT_EXTENDS                                       \
    struct _bflb_log_list list;                                               \
    void (*write)(struct bflb_log_direct * direct, void *ptr, uint16_t size); \
    bflb_log_layout_t *layout;                                                \
    uint8_t status;                                                        \
    uint8_t color;                                                         \
    uint8_t level;                                                         \
    uint8_t type;                                                          \
    uint32_t filter;                                                       \
    int (*lock)(void);                                                     \
    int (*unlock)(void)

/**
 *   @brief         direct base type
 */
typedef struct bflb_log_direct {
    _BFLB_LOG_STRUCT_DIRECT_EXTENDS;
} bflb_log_direct_t;

/**
 *   @brief         direct buffer type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_DIRECT_EXTENDS;
} bflb_log_direct_buffer_t;

/**
 *   @brief         direct stream type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_DIRECT_EXTENDS;
    uint16_t (*stream_output)(void *, uint16_t);
} bflb_log_direct_stream_t;

/**
 *   @brief         direct file type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_DIRECT_EXTENDS;
    void *fp;
    const char *path;
} bflb_log_direct_file_t;

/**
 *   @brief         direct file time division type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_DIRECT_EXTENDS;
    void *fp;         /*!< file pointer now */
    const char *path; /*!< file path */
    uint32_t interval;
    uint32_t keep;
    uint32_t timestamp;
} bflb_log_direct_file_time_t;

/**
 *   @brief         direct file size division type
 */
typedef struct
{
    _BFLB_LOG_STRUCT_DIRECT_EXTENDS;
    void *fp;
    const char *path;
    uint32_t size;
    uint32_t keep;
} bflb_log_direct_file_size_t;

/**
 * @}
 */

extern uint64_t bflb_log_clock(void);
extern uint32_t bflb_log_time(void);
extern char *bflb_log_thread(void);

extern int bflb_log_global_filter(void *tag_string, uint8_t enable);

extern int bflb_log_create(bflb_log_t *log, void *pool, uint16_t size, uint8_t mode);
extern int bflb_log_delete(bflb_log_t *log);
extern int bflb_log_append(bflb_log_t *log, bflb_log_direct_t *direct);
extern int bflb_log_remove(bflb_log_t *log, bflb_log_direct_t *direct);
extern int bflb_log_suspend(bflb_log_t *log);
extern int bflb_log_resume(bflb_log_t *log);
extern int bflb_log_control(bflb_log_t *log, uint32_t command, uint32_t param);
extern int bflb_log_filter(bflb_log_t *log, void *tag_string, uint8_t enable);
extern int bflb_log(void *log, uint8_t level, void *tag, const char *const file, const char *const func, const long line, const char *format, ...);
extern int bflb_log_flush(void *log);

extern int bflb_log_direct_create(bflb_log_direct_t *direct, uint8_t type, uint8_t color, void(*lock), void(*unlock));
extern int bflb_log_direct_delete(bflb_log_direct_t *direct);
extern int bflb_log_direct_suspend(bflb_log_direct_t *direct);
extern int bflb_log_direct_resume(bflb_log_direct_t *direct);
extern int bflb_log_direct_link(bflb_log_direct_t *direct, bflb_log_layout_t *layout);
extern int bflb_log_direct_control(bflb_log_direct_t *direct, uint32_t command, uint32_t param);
extern int bflb_log_direct_filter(bflb_log_direct_t *direct, void *tag_string, uint8_t enable);

extern int bflb_log_direct_init_buffer(bflb_log_direct_t *direct, void *buffer, void *size);
extern int bflb_log_direct_deinit_buffer(bflb_log_direct_t *direct);

extern int bflb_log_direct_init_stream(bflb_log_direct_t *direct, uint16_t (*stream_output)(void *, uint16_t));
extern int bflb_log_direct_deinit_stream(bflb_log_direct_t *direct);

extern int bflb_log_direct_init_file(bflb_log_direct_t *direct, const char *path);
extern int bflb_log_direct_deinit_file(bflb_log_direct_t *direct);

extern int bflb_log_direct_init_file_size(bflb_log_direct_t *direct, const char *path, uint32_t size, uint32_t keep);
extern int bflb_log_direct_deinit_file_size(bflb_log_direct_t *direct);

extern int bflb_log_direct_init_file_time(bflb_log_direct_t *direct, const char *path, uint32_t interval, uint32_t keep);
extern int bflb_log_direct_deinit_file_time(bflb_log_direct_t *direct);

extern int bflb_log_layout_create(bflb_log_layout_t *layout, uint8_t type);
extern int bflb_log_layout_delete(bflb_log_layout_t *layout);
extern int bflb_log_layout_format(bflb_log_layout_t *layout, int (*u_snprintf)(void *ptr, uint16_t size, char *color, char *level, char *tag, bflb_log_tm_t *tm, struct _bflb_log_msg *msg));

#ifdef BFLB_LOG_TIMESTAMP_ENABLE
extern void bflb_log_unix2time(uint32_t timestamp, bflb_log_tm_t *time);
#endif

#ifndef BFLB_LOG_LEVEL_ENABLE
#define BFLB_LOG_LEVEL_ENABLE BFLB_LOG_LEVEL_INFO
#endif

#ifdef _WIN32_DEBUG_PATH
#define __PATH_DELIMITER '\\'
#else
#define __PATH_DELIMITER '/'
#endif

#ifndef __BFLB_LOG_FILENAME__
#ifndef BFLB_LOG_FILENAME_ENABLE
#define __BFLB_LOG_FILENAME__ ""
#else
#ifndef BFLB_LOG_SHORT_FILENAME
#define __BFLB_LOG_FILENAME__ __FILE__
#else
#define __BFLB_LOG_FILENAME__ (strrchr(__FILE__, __PATH_DELIMITER) + 1)
#endif
#endif
#endif

#ifndef BFLB_LOG_FUNCTION_ENABLE
#define __BFLB_LOG_FUNCTION__ ""
#else
#define __BFLB_LOG_FUNCTION__ __FUNCTION__
#endif

#ifndef BFLB_LOG_LINE_ENABLE
#define __BFLB_LOG_LINE__ 0
#else
#define __BFLB_LOG_LINE__ __LINE__
#endif

#define __BFLB_LOG_WRAP(x) #x

/*!< define a tag */
#define BFLB_LOG_DEFINE_TAG(name, _string, enable)                                                                        \
    __attribute__((unused)) struct _bflb_log_tag __bflb_log_tag_##name##__ __attribute__((section(".bflb_log_tags_array"))) = { \
        .tag = _string,                                                                                                \
        .en = enable ? 0xffffffff : 0x00000000                                                                         \
    }

/*!< extern a tag */
#define BFLB_LOG_EXTERN_TAG(name) extern struct _bflb_log_tag __bflb_log_tag_##name##__ __attribute__((section(".bflb_log_tags_array")))

/*!< get tag name */
#define BFLB_LOG_GET_TAG(name)    &__bflb_log_tag_##name##__

/*!< set tag */
#define BFLB_LOG_TAG              NULL

#ifdef BFLB_LOG_ENABLE

#define BFLB_LOG_X(_log, _level, _tag, ...) bflb_log((void *)(_log), (_level), (_tag), __BFLB_LOG_FILENAME__, __BFLB_LOG_FUNCTION__, __BFLB_LOG_LINE__, ##__VA_ARGS__)

#if (BFLB_LOG_LEVEL_ENABLE >= BFLB_LOG_LEVEL_FATAL)
#define BFLB_LOG_F(_log, ...)  BFLB_LOG_X(_log, BFLB_LOG_LEVEL_FATAL, (BFLB_LOG_TAG), ##__VA_ARGS__)
#define BFLB_LOG_RF(_log, ...) BFLB_LOG_X(_log, BFLB_LOG_LEVEL_RAW | BFLB_LOG_LEVEL_FATAL, (BFLB_LOG_TAG), ##__VA_ARGS__)
#else
#define BFLB_LOG_F(_log, ...)  ((void)(_log))
#define BFLB_LOG_RF(_log, ...) ((void)(_log))
#endif

#if (BFLB_LOG_LEVEL_ENABLE >= BFLB_LOG_LEVEL_ERROR)
#define BFLB_LOG_E(_log, ...)  BFLB_LOG_X(_log, BFLB_LOG_LEVEL_ERROR, (BFLB_LOG_TAG), ##__VA_ARGS__)
#define BFLB_LOG_RE(_log, ...) BFLB_LOG_X(_log, BFLB_LOG_LEVEL_RAW | BFLB_LOG_LEVEL_ERROR, (BFLB_LOG_TAG), ##__VA_ARGS__)
#else
#define BFLB_LOG_E(_log, ...)  ((void)(_log))
#define BFLB_LOG_RE(_log, ...) ((void)(_log))
#endif

#if (BFLB_LOG_LEVEL_ENABLE >= BFLB_LOG_LEVEL_WARN)
#define BFLB_LOG_W(_log, ...)  BFLB_LOG_X(_log, BFLB_LOG_LEVEL_WARN, (BFLB_LOG_TAG), ##__VA_ARGS__)
#define BFLB_LOG_RW(_log, ...) BFLB_LOG_X(_log, BFLB_LOG_LEVEL_RAW | BFLB_LOG_LEVEL_WARN, (BFLB_LOG_TAG), ##__VA_ARGS__)
#else
#define BFLB_LOG_W(_log, ...)  ((void)(_log))
#define BFLB_LOG_RW(_log, ...) ((void)(_log))
#endif

#if (BFLB_LOG_LEVEL_ENABLE >= BFLB_LOG_LEVEL_INFO)
#define BFLB_LOG_I(_log, ...)  BFLB_LOG_X(_log, BFLB_LOG_LEVEL_INFO, (BFLB_LOG_TAG), ##__VA_ARGS__)
#define BFLB_LOG_RI(_log, ...) BFLB_LOG_X(_log, BFLB_LOG_LEVEL_RAW | BFLB_LOG_LEVEL_INFO, (BFLB_LOG_TAG), ##__VA_ARGS__)
#else
#define BFLB_LOG_I(_log, ...)  ((void)(_log))
#define BFLB_LOG_RI(_log, ...) ((void)(_log))
#endif

#if (BFLB_LOG_LEVEL_ENABLE >= BFLB_LOG_LEVEL_DEBUG)
#define BFLB_LOG_D(_log, ...)  BFLB_LOG_X(_log, BFLB_LOG_LEVEL_DEBUG, (BFLB_LOG_TAG), ##__VA_ARGS__)
#define BFLB_LOG_RD(_log, ...) BFLB_LOG_X(_log, BFLB_LOG_LEVEL_RAW | BFLB_LOG_LEVEL_DEBUG, (BFLB_LOG_TAG), ##__VA_ARGS__)
#else
#define BFLB_LOG_D(_log, ...)  ((void)(_log))
#define BFLB_LOG_RD(_log, ...) ((void)(_log))
#endif

#if (BFLB_LOG_LEVEL_ENABLE >= BFLB_LOG_LEVEL_TRACE)
#define BFLB_LOG_T(_log, ...)  BFLB_LOG_X(_log, BFLB_LOG_LEVEL_TRACE, (BFLB_LOG_TAG), ##__VA_ARGS__)
#define BFLB_LOG_RT(_log, ...) BFLB_LOG_X(_log, BFLB_LOG_LEVEL_RAW | BFLB_LOG_LEVEL_TRACE, (BFLB_LOG_TAG), ##__VA_ARGS__)
#else
#define BFLB_LOG_T(_log, ...)  ((void)(_log))
#define BFLB_LOG_RT(_log, ...) ((void)(_log))
#endif

#else

#define BFLB_LOG_X(_log, _level, _tag, ...) (void)(_log), (void)(_level), (void)(_tag),

#define BFLB_LOG_F(_log, ...)               ((void)(_log))
#define BFLB_LOG_RF(_log, ...)              ((void)(_log))
#define BFLB_LOG_E(_log, ...)               ((void)(_log))
#define BFLB_LOG_RE(_log, ...)              ((void)(_log))
#define BFLB_LOG_W(_log, ...)               ((void)(_log))
#define BFLB_LOG_RW(_log, ...)              ((void)(_log))
#define BFLB_LOG_I(_log, ...)               ((void)(_log))
#define BFLB_LOG_RI(_log, ...)              ((void)(_log))
#define BFLB_LOG_D(_log, ...)               ((void)(_log))
#define BFLB_LOG_RD(_log, ...)              ((void)(_log))
#define BFLB_LOG_T(_log, ...)               ((void)(_log))
#define BFLB_LOG_RT(_log, ...)              ((void)(_log))

#endif

#endif
