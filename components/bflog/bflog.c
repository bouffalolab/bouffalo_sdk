/**
 * @file bflog.c
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

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "bflog.h"
#include "bflog_dlist.h"

/** @addtogroup std_func
 * @{
 */

#ifndef bflog_fopen
#define bflog_fopen fopen
#endif

#ifndef bflog_fclose
#define bflog_fclose fclose
#endif

#ifndef bflog_fwrite
#define bflog_fwrite fwrite
#endif

#ifndef bflog_ftell
#define bflog_ftell ftell
#endif

#ifndef bflog_fflush
#define bflog_fflush fflush
#endif

#ifndef bflog_memcpy
#define bflog_memcpy memcpy
#endif

#ifndef bflog_snprintf
#define bflog_snprintf snprintf
#endif

#ifndef bflog_vsnprintf
#define bflog_vsnprintf vsnprintf
#endif

#ifndef bflog_remove
#define bflog_remove remove
#endif

#ifndef bflog_rename
#define bflog_rename rename
#endif

/**
 * @}
 */

/** @addtogroup BFLOG_LOCAL_MARCO
 * @{
 */

#ifdef BFLOG_DEBUG_ENABLE
#define _BFLOG_CHECK(_expr, _ret) \
    if (!(_expr))                 \
    return _ret
#else
#define _BFLOG_CHECK(_expr, _ret) ((void)0)
#endif

#define _msg_t(_ptr) ((struct _bflog_msg *)(_ptr))

#define _direct_t(_ptr)           ((bflog_direct_t *)(_ptr))
#define _direct_buffer_t(_ptr)    ((bflog_direct_buffer_t *)(_ptr))
#define _direct_stream_t(_ptr)    ((bflog_direct_stream_t *)(_ptr))
#define _direct_file_t(_ptr)      ((bflog_direct_file_t *)(_ptr))
#define _direct_file_time_t(_ptr) ((bflog_direct_file_time_t *)(_ptr))
#define _direct_file_size_t(_ptr) ((bflog_direct_file_size_t *)(_ptr))

#define _layout_simple_t(_ptr) ((bflog_layout_simple_t *)(_ptr))
#define _layout_format_t(_ptr) ((bflog_layout_format_t *)(_ptr))
#define _layout_yaml_t(_ptr)   ((bflog_layout_yaml_t *)(_ptr))

/*!< default log record flag */
#ifndef BFLOG_FLAG_DEFAULT
#define BFLOG_FLAG_DEFAULT ( \
    BFLOG_FLAG_LEVEL |       \
    BFLOG_FLAG_TAG |         \
    BFLOG_FLAG_FUNC |        \
    BFLOG_FLAG_LINE |        \
    BFLOG_FLAG_FILE |        \
    BFLOG_FLAG_CLK |         \
    BFLOG_FLAG_TIME |        \
    BFLOG_FLAG_THREAD)
#endif

/*!< line buffer size (in stack) */
/*!< flush use 4xline buffer size in stack */
/*!< log   use 2xline buffer size in stack */
/*!< pay attention to prevent stack overflow */
#ifndef BFLOG_LINE_BUFFER_SIZE
#define BFLOG_LINE_BUFFER_SIZE 256
#endif

/*!< default log record level */
#ifndef BFLOG_LEVEL_DEFAULT
#define BFLOG_LEVEL_DEFAULT BFLOG_LEVEL_INFO
#endif

/*!< default direct print level */
#ifndef BFLOG_DIRECT_LEVEL_DEFAULT
#define BFLOG_DIRECT_LEVEL_DEFAULT BFLOG_LEVEL_INFO
#endif

/*!< file size rotate min size */
#ifndef BFLOG_FILE_SIZE_MIN
#define BFLOG_FILE_SIZE_MIN (128 * 1024)
#endif

/*!< file time rotate min interval */
#ifndef BFLOG_FILE_INTERVAL_MIN
#define BFLOG_FILE_INTERVAL_MIN (10 * 60)
#endif

/**
 * @}
 */

static char *bflog_color_strings[6] = {
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_FATAL BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_ERROR BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_WARN BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_INFO BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_DEBUG BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_TRACE BFLOG_COLOR_END
};

static char *bflog_level_strings[6] = {
    BFLOG_LEVEL_FATAL_STRING,
    BFLOG_LEVEL_ERROR_STRING,
    BFLOG_LEVEL_WARN_STRING,
    BFLOG_LEVEL_INFO_STRING,
    BFLOG_LEVEL_DEBUG_STRING,
    BFLOG_LEVEL_TRACE_STRING
};

static char *bflog_dummy_string = "";

/**
 *   @brief         dummy function
 */
static void dummy(void)
{
}

/** @addtogroup BFLOG_TIMESTAMP
 * @{
 */

#ifdef BFLOG_TIMESTAMP_ENABLE

static const uint8_t month_day[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const uint8_t leap_month_day[12] = {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/**
 *   @brief         check leap year
 *   @param  year                   year
 *   @return uint8_t 1 leap year, 0 noleap year
 */
static uint8_t check_leap_year(uint16_t year)
{
    if (year % 4) {
        return 0;
    } else {
        if ((year % 100 == 0) && (year % 400 != 0)) {
            return 0;
        } else {
            return 1;
        }
    }
}

/**
 *   @brief         calculate week
 *   @param  time                   time
 *   @return uint8_t
 */
static void cal_weekday(bflog_tm_t *time)
{
    uint32_t y, m, d, w;

    y = time->year;
    m = time->mon;
    d = time->mday;

    if ((m == 1) || (m == 2)) {
        m += 12;
        y--;
    }

    w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) % 7;

    time->wday = (uint8_t)w;
}

/**
 *   @brief         timestamp to time
 *   @param  timestamp              timestamp
 *   @param  time                   time
 */
void bflog_unix2time(uint32_t timestamp, bflog_tm_t *time)
{
#define BFLOG_FOUR_YEAR_DAY      ((365 << 2) + 1)
#define BFLOG_SEC_NUM_PER_DAY    (24 * 60 * 60)
#define BFLOG_SEC_NUM_PER_HOUR   (60 * 60)
#define BFLOG_SEC_NUM_PER_MINUTE (60)

#ifndef BFLOG_TIMEZONE
#define BFLOG_TIMEZONE 8
#endif

    uint32_t totle_day_num;
    uint32_t current_sec_num;

    uint16_t remain_day;

    uint16_t temp_year;

    const uint8_t *p = NULL;

    totle_day_num = timestamp / BFLOG_SEC_NUM_PER_DAY;
    current_sec_num = timestamp % BFLOG_SEC_NUM_PER_DAY;

    time->hour = current_sec_num / BFLOG_SEC_NUM_PER_HOUR;
    time->min = (current_sec_num % BFLOG_SEC_NUM_PER_HOUR) / BFLOG_SEC_NUM_PER_MINUTE;
    time->sec = (current_sec_num % BFLOG_SEC_NUM_PER_HOUR) % BFLOG_SEC_NUM_PER_MINUTE;

    time->hour += BFLOG_TIMEZONE;

    if (time->hour > 23) {
        time->hour -= 24;
        totle_day_num++;
    }

    time->year = 1970 + (totle_day_num / BFLOG_FOUR_YEAR_DAY) * 4;
    remain_day = totle_day_num % BFLOG_FOUR_YEAR_DAY;

    temp_year = check_leap_year(time->year) ? 366 : 365;
    while (remain_day >= temp_year) {
        time->year++;
        remain_day -= temp_year;
        temp_year = check_leap_year(time->year) ? 366 : 365;
    }

    p = check_leap_year(time->year) ? leap_month_day : month_day;
    remain_day++;
    time->mon = 0;
    while (remain_day > *(p + time->mon)) {
        remain_day -= *(p + time->mon);
        time->mon++;
    }

    time->mon++;
    time->mday = remain_day;

    time->wday = time->mday + 2 * time->mon + 3 * (time->mon + 1) / 5 +
                 time->year + time->year / 4 - time->year / 100 + time->year / 400;

    cal_weekday(time);
}

#endif
/**
 * @}
 */

/** @addtogroup BFLOG_LINE_QUEUE
 * @{
 */

/**
 *   @brief         free oldest message in queue
 *   @param  log                    recorder
 *   @param  free                   space to be released
 *   @return uint16_t               space actually released
 */
static void queue_free(bflog_t *log, uint16_t free)
{
    char *pool = (char *)(log->queue.pool);
    uint16_t rpos = log->queue.rpos;
    uint16_t space = 0;

    while (free > space) {
        /*!< oldest message size */
        uint16_t size = _msg_t(pool + rpos)->size;

        if (rpos + size >= log->queue.size) {
            rpos = rpos + size - log->queue.size;
        } else {
            rpos += size;
        }

        space += size;
    }

    log->queue.rpos = rpos;
    log->queue.free += space;
}

/**
 *   @brief         enqueue
 *   @param  log                    recorder
 *   @param  msg                    store message area
 */
static void queue_put(bflog_t *log, struct _bflog_msg *msg)
{
    char *pool = (char *)(log->queue.pool);
    uint16_t remain = log->queue.size - log->queue.wpos;

    /*!< max message size */
    if (msg->size > log->queue.size) {
        msg->size = log->queue.size;
    }

    /*!< free the oldest msg to store the latest msg */
    if (log->queue.free < msg->size) {
        queue_free(log, msg->size - log->queue.free);
    }

    /*!< ring loop */
    if (msg->size >= remain) {
        /*!< store to ring tail */
        bflog_memcpy(pool + log->queue.wpos, msg, remain);

        log->queue.wpos = msg->size - remain;

        /*!< store to ring head */
        bflog_memcpy(pool, (char *)msg + remain, log->queue.wpos);
    } else {
        bflog_memcpy(pool + log->queue.wpos, msg, msg->size);

        log->queue.wpos += msg->size;
    }

    log->queue.free -= msg->size;
}

/**
 *   @brief         dequeue
 *   @param  log                    recorder
 *   @param  msg                    load message area
 */
static void queue_get(bflog_t *log, struct _bflog_msg *msg)
{
    char *pool = (char *)(log->queue.pool);
    uint16_t remain;
    uint16_t size;

    if (log->queue.free == log->queue.size) {
        msg->zero = 0xbd;
        return;
    }

    remain = log->queue.size - log->queue.rpos;
    size = _msg_t(pool + log->queue.rpos)->size;

    /*!< ring loop */
    if (size >= remain) {
        /*!< load from ring tail */
        bflog_memcpy(msg, pool + log->queue.rpos, remain);

        log->queue.rpos = size - remain;

        /*!< load from ring head */
        bflog_memcpy((char *)msg + remain, pool, log->queue.rpos);
    } else {
        bflog_memcpy(msg, pool + log->queue.rpos, size);
        log->queue.rpos += size;
    }

    log->queue.free += size;

    ((char *)msg)[msg->size] = '\0';
}

/**
 * @}
 */

/** @addtogroup BFLOG_BASE_API
 * @{
 */

/**
 *   @brief         create recorder, thread unsafe
 *   @param  log                    recorder
 *   @param  pool                   queue pool
 *   @param  size                   pool size
 *   @return int
 */
int bflog_create(bflog_t *log, void *pool, uint16_t size, uint8_t mode)
{
    _BFLOG_CHECK(log != NULL, -1);
    _BFLOG_CHECK(pool != NULL, -1);
    _BFLOG_CHECK(size > 0, -1);

    bflog_dlist_init(&(log->direct));

    log->status = BFLOG_STATUS_READY;
    log->flags = BFLOG_FLAG_DEFAULT;
    log->level = BFLOG_LEVEL_DEFAULT;
    log->mode = mode;

    log->queue.free = size;
    log->queue.wpos = 0;
    log->queue.rpos = 0;
    log->queue.size = size;
    log->queue.pool = pool;

    log->enter_critical = dummy;
    log->exit_critical = dummy;
    log->flush_notice = dummy;

    return 0;
}

/**
 *   @brief         delete recorder, thread unsafe
 *   @param  log                    recorder
 *   @return int
 */
int bflog_delete_s(bflog_t *log)
{
    _BFLOG_CHECK(log, -1);

    struct _bflog_list *node;
    struct _bflog_list *next;
    void *direct;

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:

            break;
        default:
            return 0;
    }

    log->enter_critical();

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:

            break;
        default:
            log->exit_critical();
            return -1;
    }

    BFLOG_DLIST_FOREACH_NEXT_S(node, next, &(log->direct))
    {
        direct = BFLOG_DLIST_ENTRY(node, bflog_direct_t, list);

        if (_direct_t(direct)->status == BFLOG_DIRECT_STATUS_RUNNING) {
            _direct_t(direct)->status = BFLOG_DIRECT_STATUS_READY;
        }

        bflog_dlist_remove(node);
    }

    log->status = BFLOG_STATUS_ILLEGAL;

    log->exit_critical();

    return 0;
}

/**
 *   @brief         append directed output, thread safe
 *   @param  log                    recorder
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_append_s(bflog_t *log, bflog_direct_t *direct)
{
    _BFLOG_CHECK(log != NULL, -1);
    _BFLOG_CHECK(direct != NULL, -1);

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    log->enter_critical();

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            log->exit_critical();
            return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_READY) {
        log->exit_critical();
        return -1;
    }
    if (!bflog_dlist_is_empty(&(direct->list))) {
        log->exit_critical();
        return -1;
    }

    bflog_dlist_insert_before(&(log->direct), &(direct->list));
    direct->status = BFLOG_DIRECT_STATUS_SUSPEND;

    log->exit_critical();

    return 0;
}

/**
 *   @brief         delete directed output, thread safe
 *   @param  log                    recorder
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_remove_s(bflog_t *log, bflog_direct_t *direct)
{
    _BFLOG_CHECK(log != NULL, -1);
    _BFLOG_CHECK(direct != NULL, -1);

    struct _bflog_list *node;

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    log->enter_critical();

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            log->exit_critical();
            return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_SUSPEND) {
        log->exit_critical();
        return -1;
    }

    if (bflog_dlist_is_empty(&(direct->list))) {
        log->exit_critical();
        return -1;
    }

    BFLOG_DLIST_FOREACH_NEXT(node, &(log->direct))
    {
        if (BFLOG_DLIST_ENTRY(node, bflog_direct_t, list) == direct) {
            bflog_dlist_remove(&(direct->list));
            direct->status = BFLOG_DIRECT_STATUS_READY;

            log->exit_critical();
            return 0;
        }
    }

    log->exit_critical();
    return -1;
}

/**
 *   @brief         suspend recorder, thread safe
 *   @param  log                    recorder
 *   @return int
 */
int bflog_suspend_s(bflog_t *log)
{
    _BFLOG_CHECK(log != NULL, -1);

    switch (log->status) {
        case BFLOG_STATUS_SUSPEND:
            return 0;
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            return -1;
    }

    log->enter_critical();

    switch (log->status) {
        case BFLOG_STATUS_SUSPEND:
            log->exit_critical();
            return 0;
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            log->exit_critical();
            return -1;
    }

    log->status = BFLOG_STATUS_SUSPEND;

    log->exit_critical();

    return 0;
}

/**
 *   @brief         resume recorder, thread safe
 *   @param  log                   recorder
 *   @return int
 */
int bflog_resume_s(bflog_t *log)
{
    _BFLOG_CHECK(log != NULL, -1);

    switch (log->status) {
        case BFLOG_STATUS_RUNNING:
            return 0;
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    log->enter_critical();

    switch (log->status) {
        case BFLOG_STATUS_RUNNING:
            log->exit_critical();
            return 0;
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            log->exit_critical();
            return -1;
    }

    log->status = BFLOG_STATUS_RUNNING;

    log->exit_critical();

    return 0;
}

/**
 *   @brief         config recorder, thread safe
 *   @param  log                    recorder
 *   @param  command                config command
 *   @param  param                  config param
 *   @return int
 */
int bflog_control_s(bflog_t *log, uint32_t command, uint32_t param)
{
    _BFLOG_CHECK(log != NULL, -1);

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    log->enter_critical();

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            log->exit_critical();
            return -1;
    }

    switch (command) {
        case BFLOG_CMD_ILLEGAL:
            return -1;
        case BFLOG_CMD_FLAG:
            log->flags = param & 0xff;
            break;
        case BFLOG_CMD_LEVEL:
            log->level = param & 0xff;
            break;
        case BFLOG_CMD_QUEUE_POOL:
            log->queue.pool = (void *)param;
            break;
        case BFLOG_CMD_QUEUE_SIZE:
            log->queue.size = param & 0xffff;
            break;
        case BFLOG_CMD_QUEUE_RST:
            log->queue.wpos = 0;
            log->queue.rpos = 0;
            log->queue.free = log->queue.size;
            break;
        case BFLOG_CMD_ENTER_CRITICAL:
            log->enter_critical = (void *)param;
            break;
        case BFLOG_CMD_EXIT_CRITICAL: {
            void (*exit_critical)(void) = log->exit_critical;
            log->exit_critical = (void *)param;
            exit_critical();
        }
            return 0;
        case BFLOG_CMD_FLUSH_NOTICE:
            log->flush_notice = (void *)param;
            break;
        case BFLOG_CMD_MODE:
            log->mode = param & 0xff;
            break;
        default:
            log->exit_critical();
            return -1;
    }

    log->exit_critical();

    return 0;
}

/**
 *   @brief         record log msg, thread safe
 *                  tag, file, func only recorded pointer
 *   @param  log                    recorder
 *   @param  level                  level threshold
 *   @param  tag                    const tag string
 *   @param  file                   const file name
 *   @param  func                   const func name
 *   @param  line                   file line
 *   @param  format                 format string
 *   @param  ...                    format params
 *   @return int
 */
void bflog_s(bflog_t *log, uint8_t level, const char *const tag, const char *const file, const char *const func, const long line, const char *format, ...)
{
    _BFLOG_CHECK(log != NULL, );
    _BFLOG_CHECK(format != NULL, );

    int ret;
    va_list args;
    uint8_t msg[BFLOG_LINE_BUFFER_SIZE + sizeof(struct _bflog_msg)];
    uint16_t size = sizeof(struct _bflog_msg);

    /*!< working only during running and suspend */
    switch (log->status) {
        case BFLOG_STATUS_SUSPEND:
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            return;
    }

    /*!< ignore high level */
    if (level > log->level) {
        return;
    }

    /*!< record clock tick */
    if (log->flags & BFLOG_FLAG_CLK) {
        _msg_t(msg)->clk = bflog_clock();
    } else {
        _msg_t(msg)->clk = 0;
    }

    /*!< record timestamp */
    if (log->flags & BFLOG_FLAG_TIME) {
        _msg_t(msg)->time = bflog_time();
    } else {
        _msg_t(msg)->time = 0;
    }

    /*!< record level */
    _msg_t(msg)->level = level;

    /*!< record tag, only record pointer */
    if (log->flags & BFLOG_FLAG_TAG) {
        _msg_t(msg)->tag = tag;
    } else {
        _msg_t(msg)->tag = bflog_dummy_string;
    }

    /*!< record func, only record pointer */
    if (log->flags & BFLOG_FLAG_FUNC) {
        _msg_t(msg)->func = func;
    } else {
        _msg_t(msg)->func = bflog_dummy_string;
    }

    /*!< record line */
    if (log->flags & BFLOG_FLAG_LINE) {
        _msg_t(msg)->line = line;
    } else {
        _msg_t(msg)->line = 0;
    }

    /*!< record file, only record pointer */
    if (log->flags & BFLOG_FLAG_FILE) {
        _msg_t(msg)->file = file;
    } else {
        _msg_t(msg)->file = bflog_dummy_string;
    }

    /*!< record thread, only record pointer */
    if (log->flags & BFLOG_FLAG_THREAD) {
        _msg_t(msg)->thread = bflog_thread();
    } else {
        _msg_t(msg)->thread = bflog_dummy_string;
    }

    /*!< set zero */
    _msg_t(msg)->zero = 0;

    /*!< print string to msg->string */
    va_start(args, format);
    ret = bflog_vsnprintf(_msg_t(msg)->string, BFLOG_LINE_BUFFER_SIZE, format, args);
    va_end(args);

    /*!< check true size */
    if ((ret >= 0) && (ret <= BFLOG_LINE_BUFFER_SIZE)) {
        size += ret;
    } else {
        size += BFLOG_LINE_BUFFER_SIZE;
    }

    /*!< align 4 byte */
    if (size & 0x3) {
        _msg_t(msg)->size = (size & ~0x3) + 0x4;
    } else {
        _msg_t(msg)->size = size;
    }

    log->enter_critical();
    /*!< working only during running and suspend */
    switch (log->status) {
        case BFLOG_STATUS_SUSPEND:
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            log->exit_critical();
            return;
    }

    queue_put(log, (void *)(&msg));

    if (log->mode & BFLOG_MODE_ASYNC) {
        log->flush_notice();
        log->exit_critical();
        return;
    }

    log->exit_critical();

    bflog_flush_s(log);
}

/**
 *   @brief         flush all msg in queue, thread safe
 *   @param  log                    recorder
 *   @return int
 */
void bflog_flush_s(bflog_t *log)
{
    _BFLOG_CHECK(log != NULL, );

    uint8_t msg[BFLOG_LINE_BUFFER_SIZE + sizeof(struct _bflog_msg)];
    char buf[BFLOG_LINE_BUFFER_SIZE * 2];
    struct _bflog_list *node;
    void *direct;

    /*!< working only during running */
    switch (log->status) {
        case BFLOG_STATUS_SUSPEND:
            return;
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            return;
    }

    do {
        /*!< reset msg string */
        _msg_t(msg)->string[0] = '\0';

        log->enter_critical();

        /*!< working only during running */
        switch (log->status) {
            case BFLOG_STATUS_SUSPEND:
                log->exit_critical();
                return;
            case BFLOG_STATUS_RUNNING:
                break;
            default:
                log->exit_critical();
                return;
        }

        /*!< get msg(on stack) from queue(on ram) */
        queue_get(log, (void *)(&msg));

        log->exit_critical();

        if (_msg_t(msg)->zero != 0) {
            if (_msg_t(msg)->zero == 0xbd) {
                /*!< no msg */
                return;
            } else {
                /*!< error */
                return;
            }
        }

        /*!< color */
        char *color;
        color = bflog_color_strings[_msg_t(msg)->level];

        /*!< level */
        char *level;
        if (log->flags & BFLOG_FLAG_LEVEL) {
            level = bflog_level_strings[_msg_t(msg)->level];
        } else {
            level = bflog_dummy_string;
        }

#ifdef BFLOG_TIMESTAMP_ENABLE
        /*!< time */
        bflog_tm_t tm;

        bflog_unix2time(_msg_t(msg)->time, &tm);
#endif

        log->enter_critical();
        /*!< foreach direct, execute layout to buf(on stack), then output buf(on stack) */
        BFLOG_DLIST_FOREACH_NEXT(node, &(log->direct))
        {
            log->exit_critical();

            int size;
            direct = BFLOG_DLIST_ENTRY(node, bflog_direct_t, list);

            /*!< check direct status */
            if (_direct_t(direct)->status != BFLOG_DIRECT_STATUS_RUNNING) {
                continue;
            }

            /*!< level filter */
            if (_msg_t(msg)->level > _direct_t(direct)->level) {
                continue;
            }

            /*!< nolayout */
            if (_direct_t(direct)->layout == NULL) {
                goto simple_layout;
            }

            /*!< layout */
            switch (_direct_t(direct)->layout->type) {
                case BFLOG_LAYOUT_TYPE_FORMAT:
                    if (_direct_t(direct)->color) {
#ifdef BFLOG_TIMESTAMP_ENABLE
                        size = _layout_format_t(_direct_t(direct)->layout)->snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, color, level, &tm, _msg_t(msg));
#else
                        size = _layout_format_t(_direct_t(direct)->layout)->snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, color, level, NULL, _msg_t(msg));
#endif

                    } else {
#ifdef BFLOG_TIMESTAMP_ENABLE
                        size = _layout_format_t(_direct_t(direct)->layout)->snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, bflog_dummy_string, level, &tm, _msg_t(msg));
#else
                        size = _layout_format_t(_direct_t(direct)->layout)->snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, bflog_dummy_string, level, NULL, _msg_t(msg));
#endif
                    }
                    goto output;

                    /*!< TODO Layout yaml format */
                case BFLOG_LAYOUT_TYPE_YAML:
                case BFLOG_LAYOUT_TYPE_SIMPLE:
                default:
                    goto simple_layout;
            }

        simple_layout:

            if (_direct_t(direct)->color) {
                /*!< default and simple color format */
#ifdef BFLOG_TIMESTAMP_ENABLE
                size = bflog_snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, BFLOG_SIMPLE_LAYOUT_STRING(color, level, &tm, _msg_t(msg)));
#else
                size = bflog_snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, BFLOG_SIMPLE_LAYOUT_STRING(color, level, NULL, _msg_t(msg)));
#endif

            } else {
                /*!< default and simple no color format */
#ifdef BFLOG_TIMESTAMP_ENABLE
                size = bflog_snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, BFLOG_SIMPLE_LAYOUT_STRING(bflog_dummy_string, level, &tm, _msg_t(msg)));
#else
                size = bflog_snprintf(buf, BFLOG_LINE_BUFFER_SIZE * 2, BFLOG_SIMPLE_LAYOUT_STRING(bflog_dummy_string, level, NULL, _msg_t(msg)));
#endif
            }

        output:
            /*!< check true size */
            if ((size < 0) || (size > BFLOG_LINE_BUFFER_SIZE * 2)) {
                size = BFLOG_LINE_BUFFER_SIZE * 2;
            }

            /*!< call write */
            _direct_t(direct)->write(direct, buf, size);

            log->enter_critical();
        }

        log->exit_critical();
    } while (1);

    return;
}

/**
 * @}
 */

/** @addtogroup BFLOG_DIRECT_API
 * @{
 */

/**
 *   @brief         create directed output, thread unsafe
 *   @param  direct                 directed output
 *   @param  type                   directed output type
 *   @return int
 */
int bflog_direct_create(bflog_direct_t *direct, uint8_t type, uint8_t color)
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(
        type != BFLOG_DIRECT_TYPE_BUFFER ||
            type != BFLOG_DIRECT_TYPE_STREAM ||
            type != BFLOG_DIRECT_TYPE_FILE ||
            type != BFLOG_DIRECT_TYPE_FILE_TIME ||
            type != BFLOG_DIRECT_TYPE_FILE_SIZE,
        -1);

    bflog_dlist_init(&(direct->list));

    direct->write = NULL;
    direct->layout = NULL;
    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->level = BFLOG_DIRECT_LEVEL_DEFAULT;
    direct->type = type;
    direct->color = color;

    return 0;
}

/**
 *   @brief         delete directed output, thread unsafe
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_direct_delete(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);

    switch (direct->status) {
        case BFLOG_DIRECT_STATUS_INIT:
            break;
        default:
            return -1;
    }

    bflog_dlist_remove(&(direct->list));

    direct->write = NULL;
    direct->status = BFLOG_DIRECT_STATUS_ILLEGAL;
    direct->type = BFLOG_DIRECT_TYPE_ILLEGAL;

    return 0;
}

/**
 *   @brief         suspend directed output, thread safe
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_direct_suspend_s(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);

    switch (direct->status) {
        case BFLOG_DIRECT_STATUS_RUNNING:
        case BFLOG_DIRECT_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    direct->status = BFLOG_STATUS_SUSPEND;

    return 0;
}

/**
 *   @brief         resume directed output, thread safe
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_direct_resume_s(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);

    switch (direct->status) {
        case BFLOG_DIRECT_STATUS_RUNNING:
        case BFLOG_DIRECT_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    direct->status = BFLOG_STATUS_RUNNING;

    return 0;
}

/**
 *   @brief         link to a layout, thread unsafe
 *   @param  direct                 directed output
 *   @param  layout                 layout
 *   @return int
 */
int bflog_direct_link(bflog_direct_t *direct, bflog_layout_t *layout)
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(layout != NULL, -1);

    if (direct->status != BFLOG_DIRECT_STATUS_READY) {
        return -1;
    }

    if (layout->status != BFLOG_LAYOUT_STATUS_READY) {
        return -1;
    }

    direct->layout = layout;

    return 0;
}

/**
 *   @brief         config direct
 *   @param  direct
 *   @param  command
 *   @param  param
 *   @return int
 */
int bflog_direct_control(bflog_direct_t *direct, uint32_t command, uint32_t param)
{
    _BFLOG_CHECK(direct != NULL, -1);

    switch (direct->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    switch (command) {
        case BFLOG_DIRECT_CMD_LEVEL:
            direct->level = param & 0xff;
            break;

        case BFLOG_DIRECT_CMD_COLOR:
            direct->color = param != 0;
            break;

        default:
            return -1;
    }

    return 0;
}

/**
 * @}
 */

/** @addtogroup BFLOG_DIRECT_TYPE_API
 * @{
 */

/** @addtogroup BFLOG_DIRECT_BUFFER_API
 * @{
 */
#ifdef BFLOG_DIRECT_BUFFER_ENABLE
#if 0
static void bflog_driect_write_buffer(bflog_direct_t *direct, void *ptr, uint16_t size)
{
    /*!< TODO */
    return;
}

int bflog_direct_init_buffer(bflog_direct_t *direct, void *buffer, void *size)
{
    /*!< TODO */
    (void)bflog_driect_write_buffer;
    return 0;
}

int bflog_direct_deinit_buffer(bflog_direct_t *direct)
{
    /*!< TODO */
    return 0;
}
#endif
#endif
/**
 * @}
 */

/** @addtogroup BFLOG_DIRECT_STREAM_API
 * @{
 */
#ifdef BFLOG_DIRECT_STREAM_ENABLE
/**
 *   @brief         write data to stream
 *   @param  log                    recorder
 *   @param  ptr                    data pointer
 *   @param  size                   data size
 *   @return int
 */
static void bflog_direct_write_stream(bflog_direct_t *direct, void *ptr, uint16_t size)
{
    _direct_stream_t(direct)->stream_output(ptr, size);
}

/**
 *   @brief         init stream type direct
 *   @param  direct                 directed output
 *   @param  stream_output          stream output function pointer
 *   @return int
 */
int bflog_direct_init_stream(bflog_direct_t *direct, uint16_t (*stream_output)(void *, uint16_t))
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(stream_output != NULL, -1);

    if (direct->type != BFLOG_DIRECT_TYPE_STREAM) {
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        return -1;
    }

    _direct_stream_t(direct)->stream_output = stream_output;
    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_stream;

    return 0;
}

/**
 *   @brief         deinit stream type direct
 *   @param  direct                 directed output
 *   @return int 
 */
int bflog_direct_deinit_stream(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);
    /*!< TODO */
    return 0;
}

#endif

/**
 * @}
 */

/** @addtogroup BFLOG_DIRECT_FILE_API
 * @{
 */
#ifdef BFLOG_DIRECT_FILE_ENABLE
/**
 *   @brief         write data to file
 *   @param  direct                 directed output
 *   @param  ptr                    data ptr
 *   @param  size                   data size
 */
static void bflog_direct_write_file(bflog_direct_t *direct, void *ptr, uint16_t size)
{
    _BFLOG_CHECK(direct != NULL, );
    _BFLOG_CHECK(ptr != NULL, );

    if (_direct_file_t(direct)->fp == NULL) {
        return;
    }

    bflog_fwrite(ptr, 1, size, _direct_file_t(direct)->fp);
    bflog_fflush(_direct_file_t(direct)->fp);
}

/**
 *   @brief         
 *   @param  direct                 
 *   @param  path                   
 *   @param  lock                   
 *   @param  unlock                 
 *   @return int 
 */
int bflog_direct_init_file(bflog_direct_t *direct, const char *path, void (*lock)(void), void (*unlock)(void))
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(path != NULL, -1);

    char fullpath[256];

    if ((lock == NULL) || (unlock == NULL)) {
        lock = dummy;
        unlock = dummy;
    }

    lock();

    if (direct->type != BFLOG_DIRECT_TYPE_FILE) {
        unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        unlock();
        return -1;
    }

    size_t pathsize = strlen(path);
    bflog_memcpy(fullpath, path, pathsize);
    bflog_snprintf(fullpath + pathsize, 16, ".log");

    _direct_file_t(direct)->fp = bflog_fopen(fullpath, "a+");
    if (_direct_file_t(direct)->fp == NULL) {
        unlock();
        return -1;
    }

    _direct_file_t(direct)->path = path;
    _direct_file_t(direct)->lock = lock;
    _direct_file_t(direct)->unlock = unlock;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_file;

    unlock();

    return 0;
}

/**
 *   @brief
 *   @param  direct
 *   @return int
 */
int bflog_direct_deinit_file(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);

    if (direct->type != BFLOG_DIRECT_TYPE_FILE) {
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_READY) {
        return -1;
    }

    _direct_file_t(direct)->lock();

    if (bflog_fclose(_direct_file_t(direct)->fp)) {
        _direct_file_t(direct)->unlock();
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;
    _direct_file_t(direct)->fp = NULL;
    _direct_file_t(direct)->path = NULL;
    _direct_file_t(direct)->lock = NULL;

    void (*unlock)(void) = _direct_file_t(direct)->unlock;
    _direct_file_t(direct)->unlock = NULL;
    unlock();

    return 0;
}

#endif
/**
 * @}
 */

/** @addtogroup BFLOG_DIRECT_FILE_TIME_API
 * @{
 */

#if (defined(BFLOG_DIRECT_FILE_SIZE_ENABLE) || defined(BFLOG_DIRECT_FILE_TIME_ENABLE))

/**
 *   @brief         file rotate
 *   @param  direct                 file_size/time directed output
 */
static void file_rotate(bflog_direct_file_size_t *direct)
{
    char oldpath[256];
    char newpath[256];

    void *fp;

    size_t pathsize = strlen(direct->path);
    bflog_memcpy(oldpath, direct->path, pathsize);
    bflog_memcpy(newpath, direct->path, pathsize);

    if (direct->fp) {
        bflog_fclose(direct->fp);
    }

    for (int i = direct->keep - 1; i >= 0; i--) {
        bflog_snprintf(oldpath + pathsize, 16, i ? "_%d.log" : ".log", i - 1);
        bflog_snprintf(newpath + pathsize, 16, "_%d.log", i);

        fp = bflog_fopen(newpath, "r");
        if (fp != NULL) {
            bflog_fclose(fp);
            bflog_remove(newpath);
        }

        fp = bflog_fopen(oldpath, "r");
        if (fp != NULL) {
            bflog_fclose(fp);
            bflog_rename(oldpath, newpath);
        }
    }

    direct->fp = bflog_fopen(oldpath, "a+");
}

#endif

#ifdef BFLOG_DIRECT_FILE_TIME_ENABLE

/**
 *   @brief         write data to file
 *   @param  direct
 *   @param  ptr
 *   @param  size
 */
static void bflog_direct_write_file_time(bflog_direct_t *direct, void *ptr, uint16_t size)
{
    _BFLOG_CHECK(direct != NULL, );
    _BFLOG_CHECK(ptr != NULL, );

    uint32_t timestamp;

    if (_direct_file_time_t(direct)->fp == NULL) {
        return;
    }

    timestamp = bflog_time();

    if (timestamp > _direct_file_time_t(direct)->timestamp + _direct_file_time_t(direct)->interval) {
        file_rotate((void *)direct);
        _direct_file_time_t(direct)->timestamp = timestamp;
    }

    bflog_fwrite(ptr, 1, size, _direct_file_size_t(direct)->fp);
    bflog_fflush(_direct_file_size_t(direct)->fp);
}

/**
 *   @brief         
 *   @param  direct                 directed output
 *   @param  path                   file path
 *   @param  interval               file rotate interval
 *   @param  keep                   max keep file count
 *   @param  lock                   lock   function
 *   @param  unlock                 unlock function
 *   @return int 
 */
int bflog_direct_init_file_time(bflog_direct_t *direct, const char *path, uint32_t interval, uint32_t keep, void (*lock)(void), void (*unlock)(void))
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(path != NULL, -1);
    _BFLOG_CHECK(interval >= BFLOG_FILE_INTERVAL_MIN, -1);
    _BFLOG_CHECK(keep > 0, -1);

    if (interval < BFLOG_FILE_INTERVAL_MIN) {
        interval = BFLOG_FILE_INTERVAL_MIN;
    }

    char fullpath[256];

    if ((lock == NULL) || (unlock == NULL)) {
        lock = dummy;
        unlock = dummy;
    }

    lock();

    if (direct->type != BFLOG_DIRECT_TYPE_FILE_TIME) {
        unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        unlock();
        return -1;
    }

    size_t pathsize = strlen(path);
    bflog_memcpy(fullpath, path, pathsize);
    bflog_snprintf(fullpath + pathsize, 16, ".log");

    _direct_file_time_t(direct)->fp = bflog_fopen(fullpath, "a+");
    if (_direct_file_time_t(direct)->fp == NULL) {
        unlock();
        return -1;
    }

    _direct_file_time_t(direct)->timestamp = bflog_time();
    _direct_file_time_t(direct)->keep = keep;
    _direct_file_time_t(direct)->interval = interval;
    _direct_file_time_t(direct)->path = path;
    _direct_file_time_t(direct)->lock = lock;
    _direct_file_time_t(direct)->unlock = unlock;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_file_time;

    unlock();

    return 0;
}

/**
 *   @brief
 *   @param  direct
 *   @return int
 */
int bflog_direct_deinit_file_time(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);

    if (direct->type != BFLOG_DIRECT_TYPE_FILE_TIME) {
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_READY) {
        return -1;
    }

    _direct_file_time_t(direct)->lock();

    if (bflog_fclose(_direct_file_time_t(direct)->fp)) {
        _direct_file_time_t(direct)->unlock();
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;
    _direct_file_time_t(direct)->fp = NULL;
    _direct_file_time_t(direct)->path = NULL;
    _direct_file_time_t(direct)->lock = NULL;
    _direct_file_time_t(direct)->keep = 0;

    void (*unlock)(void) = _direct_file_time_t(direct)->unlock;
    _direct_file_time_t(direct)->unlock = NULL;
    unlock();

    return 0;
}

#endif

/**
 * @}
 */

/** @addtogroup BFLOG_DIRECT_FILE_SIZE_API
 * @{
 */

#ifdef BFLOG_DIRECT_FILE_SIZE_ENABLE

/**
 *   @brief         write data to file
 *   @param  direct
 *   @param  ptr
 *   @param  size
 */
static void bflog_direct_write_file_size(bflog_direct_t *direct, void *ptr, uint16_t size)
{
    _BFLOG_CHECK(direct != NULL, );
    _BFLOG_CHECK(ptr != NULL, );

    if (_direct_file_size_t(direct)->fp == NULL) {
        return;
    }

    /*!< fseek(_direct_file_size_t(direct)->fp, 0L, SEEK_END); */
    size_t fsize = bflog_ftell(_direct_file_size_t(direct)->fp);

    if (fsize > _direct_file_size_t(direct)->size) {
        file_rotate((void *)direct);
    }

    bflog_fwrite(ptr, 1, size, _direct_file_size_t(direct)->fp);
    bflog_fflush(_direct_file_size_t(direct)->fp);
}

/**
 *   @brief         
 *   @param  direct                 
 *   @param  path                   
 *   @param  size                   
 *   @param  keep                   
 *   @param  lock                   
 *   @param  unlock                 
 *   @return int 
 */
int bflog_direct_init_file_size(bflog_direct_t *direct, const char *path, uint32_t size, uint32_t keep, void (*lock)(void), void (*unlock)(void))
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(path != NULL, -1);
    _BFLOG_CHECK(keep > 0, -1);

    if (size < BFLOG_FILE_SIZE_MIN) {
        size = BFLOG_FILE_SIZE_MIN;
    }

    char fullpath[256];

    if ((lock == NULL) || (unlock == NULL)) {
        lock = dummy;
        unlock = dummy;
    }

    lock();

    if (direct->type != BFLOG_DIRECT_TYPE_FILE_SIZE) {
        unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        unlock();
        return -1;
    }

    size_t pathsize = strlen(path);
    bflog_memcpy(fullpath, path, pathsize);
    bflog_snprintf(fullpath + pathsize, 16, ".log");

    _direct_file_size_t(direct)->fp = bflog_fopen(fullpath, "a+");
    if (_direct_file_size_t(direct)->fp == NULL) {
        unlock();
        return -1;
    }

    _direct_file_size_t(direct)->keep = keep;
    _direct_file_size_t(direct)->size = size;
    _direct_file_size_t(direct)->path = path;
    _direct_file_size_t(direct)->lock = lock;
    _direct_file_size_t(direct)->unlock = unlock;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_file_size;

    unlock();

    return 0;
}

/**
 *   @brief
 *   @param  direct
 *   @return int
 */
int bflog_direct_deinit_file_size(bflog_direct_t *direct)
{
    _BFLOG_CHECK(direct != NULL, -1);

    if (direct->type != BFLOG_DIRECT_TYPE_FILE_SIZE) {
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_READY) {
        return -1;
    }

    _direct_file_size_t(direct)->lock();

    if (bflog_fclose(_direct_file_size_t(direct)->fp)) {
        _direct_file_size_t(direct)->unlock();
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;
    _direct_file_size_t(direct)->fp = NULL;
    _direct_file_size_t(direct)->path = NULL;
    _direct_file_size_t(direct)->lock = NULL;
    _direct_file_size_t(direct)->keep = 0;

    void (*unlock)(void) = _direct_file_size_t(direct)->unlock;
    _direct_file_size_t(direct)->unlock = NULL;
    unlock();

    return 0;
}

#endif

/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup BFLOG_LAYOUT_API
 * @{
 */

/**
 *   @brief         create layout, thread unsafe
 *   @param  layout                 layout
 *   @param  type                   layout type
 *   @param  color                  color enable
 *   @return int
 */
int bflog_layout_create(bflog_layout_t *layout, uint8_t type)
{
    _BFLOG_CHECK(layout != NULL, -1);
    _BFLOG_CHECK(
        type != BFLOG_LAYOUT_TYPE_SIMPLE ||
            type != BFLOG_LAYOUT_TYPE_FORMAT ||
            type != BFLOG_LAYOUT_TYPE_YAML,
        -1);

    switch (type) {
        case BFLOG_LAYOUT_TYPE_SIMPLE:
            layout->status = BFLOG_LAYOUT_STATUS_READY;
            break;
        case BFLOG_LAYOUT_TYPE_FORMAT:
            layout->status = BFLOG_LAYOUT_STATUS_INIT;
            break;
        case BFLOG_LAYOUT_TYPE_YAML:

            break;
    }

    layout->type = type;

    return 0;
}

/**
 *   @brief         delete layout, thread unsafe
 *   @param  layout                 layout
 *   @return int
 */
int bflog_layout_delete(bflog_layout_t *layout)
{
    _BFLOG_CHECK(layout != NULL, -1);

    layout->status = BFLOG_LAYOUT_STATUS_ILLEGAL;

    return 0;
}

/**
 *   @brief         set formatting of layout format, thread unsafe
 *   @param  layout                 layout
 *   @param  snprintf               format
 *   @return int
 */
int bflog_layout_format(bflog_layout_t *layout, int (*u_snprintf)(void *ptr, uint16_t size, char *color, char *level, bflog_tm_t *tm, struct _bflog_msg *msg))
{
    _BFLOG_CHECK(layout != NULL, -1);
    _BFLOG_CHECK(u_snprintf != NULL, -1);

    if (layout->type != BFLOG_LAYOUT_TYPE_FORMAT) {
        return -1;
    }

    if (layout->status != BFLOG_LAYOUT_STATUS_INIT) {
        return -1;
    }

    _layout_format_t(layout)->snprintf = u_snprintf;
    layout->status = BFLOG_LAYOUT_STATUS_READY;

    return 0;
}

/**
 * @}
 */
