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

/** @addtogroup std_func
 * @{
 */

#ifndef bflogc_fopen
#define bflogc_fopen fopen
#endif

#ifndef bflogc_fclose
#define bflogc_fclose fclose
#endif

#ifndef bflogc_fwrite
#define bflogc_fwrite fwrite
#endif

#ifndef bflogc_ftell
#define bflogc_ftell ftell
#endif

#ifndef bflogc_fflush
#define bflogc_fflush fflush
#endif

#ifndef bflogc_memcpy
#define bflogc_memcpy memcpy
#endif

#ifndef bflogc_snprintf
#define bflogc_snprintf snprintf
#endif

#ifndef bflogc_vsnprintf
#define bflogc_vsnprintf vsnprintf
#endif

#ifndef bflogc_remove
#define bflogc_remove remove
#endif

#ifndef bflogc_rename
#define bflogc_rename rename
#endif

#ifndef bflogc_strcmp
#define bflogc_strcmp strcmp
#endif

/**
 * @}
 */

/** @addtogroup BFLOG_LOCAL_MARCO
 * @{
 */

#ifdef CONFIG_BFLOG_DEBUG
#define _BFLOG_CHECK(_expr, _ret) \
    if (!(_expr))                 \
    return _ret
#else
#define _BFLOG_CHECK(_expr, _ret) ((void)0)
#endif

#define _bflog_t(_ptr)            ((bflog_t *)(_ptr))

#define _msg_t(_ptr)              ((struct _bflog_msg *)(_ptr))
#define _tag_t(_ptr)              ((struct _bflog_tag *)(_ptr))

#define _direct_t(_ptr)           ((bflog_direct_t *)(_ptr))
#define _direct_buffer_t(_ptr)    ((bflog_direct_buffer_t *)(_ptr))
#define _direct_stream_t(_ptr)    ((bflog_direct_stream_t *)(_ptr))
#define _direct_file_t(_ptr)      ((bflog_direct_file_t *)(_ptr))
#define _direct_file_time_t(_ptr) ((bflog_direct_file_time_t *)(_ptr))
#define _direct_file_size_t(_ptr) ((bflog_direct_file_size_t *)(_ptr))

#define _layout_simple_t(_ptr)    ((bflog_layout_simple_t *)(_ptr))
#define _layout_format_t(_ptr)    ((bflog_layout_format_t *)(_ptr))
#define _layout_yaml_t(_ptr)      ((bflog_layout_yaml_t *)(_ptr))

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
#if defined(CONFIG_LOG_NCOLOR) && CONFIG_LOG_NCOLOR
static char *bflog_color_strings[6] = {
    "",
    "",
    "",
    "",
    "",
    ""
};
#else
static char *bflog_color_strings[6] = {
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_FATAL BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_ERROR BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_WARN BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_INFO BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_DEBUG BFLOG_COLOR_END,
    BFLOG_COLOR_START BFLOG_COLOR_RESET BFLOG_COLOR_TRACE BFLOG_COLOR_END
};
#endif

static char *bflog_level_strings[6] = {
    BFLOG_LEVEL_FATAL_STRING,
    BFLOG_LEVEL_ERROR_STRING,
    BFLOG_LEVEL_WARN_STRING,
    BFLOG_LEVEL_INFO_STRING,
    BFLOG_LEVEL_DEBUG_STRING,
    BFLOG_LEVEL_TRACE_STRING
};

static uint32_t global_filter = 0xffffffff;

static char *bflog_dummy_string = "";

extern struct _bflog_tag __bflog_tags_start__;
extern struct _bflog_tag __bflog_tags_end__;

/**
 *   @brief         dummy function
 */
static int dummy(void)
{
    return 0;
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

#define bflog_dlist_init(_plist) \
    (_plist)->next = (_plist);   \
    (_plist)->prev = (_plist)

#define bflog_dlist_insert_after(_plist, _pnode) \
    (_plist)->next->prev = (_pnode);             \
    (_pnode)->next = (_plist)->next;             \
    (_plist)->next = (_pnode);                   \
    (_pnode)->prev = (_plist)

#define bflog_dlist_insert_before(_plist, _pnode) \
    (_plist)->prev->next = (_pnode);              \
    (_pnode)->prev = (_plist)->prev;              \
    (_plist)->prev = (_pnode);                    \
    (_pnode)->next = (_plist)

#define bflog_dlist_remove(_pnode)         \
    (_pnode)->next->prev = (_pnode)->prev; \
    (_pnode)->prev->next = (_pnode)->next; \
    (_pnode)->next = (_pnode);             \
    (_pnode)->prev = (_pnode)

#define bflog_dlist_move_head(_plist, _pnode) \
    bflog_dlist_remove(_pnode);               \
    bflog_dlist_insert_after(_plist, _pnode)

#define bflog_dlist_move_tail(_plist, _pnode) \
    bflog_dlist_remove(_pnode);               \
    bflog_dlist_insert_before(_plist, _pnode)

#define bflog_dlist_is_empty(_plist) \
    ((void *)((_plist)->next) == (void *)(_plist))

#define BFLOG_DLIST_CONTAINER_OF(_ptr, _type, _member) \
    ((_type *)((char *)(_ptr) - (unsigned int)(&((_type *)0)->_member)))

#define BFLOG_DLIST_NEW(_list_name) \
    struct _bflog_list _list_name = { &(_list_name), &(_list_name) }

#define BFLOG_DLIST_ENTRY(_pnode, _type, _member) \
    BFLOG_DLIST_CONTAINER_OF(_pnode, _type, _member)

#define BFLOG_DLIST_ENTRY_FIRST(_pnode, _type, _member) \
    BFLOG_DLIST_CONTAINER_OF((_pnode)-next, _type, _member)

#define BFLOG_DLIST_ENTRY_FIRST_ORNULL(_pnode, _type, _member) \
    (bflog_dlist_is_empty(_pnode) ? NULL : BFLOG_DLIST_ENTRY_FIRST(_pnode, _type, _member))

#define BFLOG_DLIST_FOREACH(_pos, _head)            for ((_pos) = (_head)->next; (_pos) != (_head); (_pos) = (_pos)->next)

#define BFLOG_DLIST_FOREACH_NEXT                    BFLOG_DLIST_FOREACH

#define BFLOG_DLIST_FOREACH_PREV(_pos, _head)       for ((_pos) = (_head)->prev; (_pos) != (_head); (_pos) = (_pos)->prev)

#define BFLOG_DLIST_FOREACH_S(_pos, _n, _head)      for ((_pos) = (_head)->next, (_n) = (_pos)->next; (_pos) != (_head); (_pos) = (_n), (_n) = (_pos)->next)

#define BFLOG_DLIST_FOREACH_NEXT_S                  BFLOG_DLIST_FOREACH_S

#define BFLOG_DLIST_FOREACH_PREV_S(_pos, _n, _head) for ((_pos) = (_head)->prev, (_n) = (_pos)->prev; (_pos) != (_head); (_pos) = (_n), (_n) = (_pos)->prev)

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
        bflogc_memcpy(pool + log->queue.wpos, msg, remain);

        log->queue.wpos = msg->size - remain;

        /*!< store to ring head */
        bflogc_memcpy(pool, (char *)msg + remain, log->queue.wpos);
    } else {
        bflogc_memcpy(pool + log->queue.wpos, msg, msg->size);

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
        bflogc_memcpy(msg, pool + log->queue.rpos, remain);

        log->queue.rpos = size - remain;

        /*!< load from ring head */
        bflogc_memcpy((char *)msg + remain, pool, log->queue.rpos);
    } else {
        bflogc_memcpy(msg, pool + log->queue.rpos, size);
        log->queue.rpos += size;
    }

    log->queue.free += size;

    ((char *)msg)[msg->size] = '\0';
}

/**
 * @}
 */

/** @addtogroup BFLOG_FILTER
 * @{
 */

/**
 *   @brief         find the last bit set
 *   @param  word                   word
 *   @return int                    bit index
 */
static int bflog_fls(uint32_t word)
{
    int bit = 32;

    if (!word) {
        bit -= 1;
    }
    if (!(word & 0xffff0000)) {
        word <<= 16;
        bit -= 16;
    }
    if (!(word & 0xff000000)) {
        word <<= 8;
        bit -= 8;
    }
    if (!(word & 0xf0000000)) {
        word <<= 4;
        bit -= 4;
    }
    if (!(word & 0xc0000000)) {
        word <<= 2;
        bit -= 2;
    }
    if (!(word & 0x80000000)) {
        word <<= 1;
        bit -= 1;
    }

    return bit;
}

/**
 *   @brief         alloc a filter
 *   @return uint32_t  (0 on faild) filter bit
 */
static uint32_t bflog_filter_alloc(void)
{
    uint8_t shift = bflog_fls(global_filter) - 1;

    if (shift >= 0) {
        global_filter = global_filter & ~(0x1 << shift);
        return 0x1 << shift;
    } else {
        return 0;
    }
}

/**
 *   @brief         free a filter
 *   @param  filter                 filter bit
 */
static void bflog_filter_free(uint32_t filter)
{
    global_filter = global_filter | filter;
}

/**
 *   @brief         config filter
 *   @param  filter                 filter bit
 *   @param  tag_string             tag string pointer
 *   @return int
 */
static int bflog_filter_set(uint32_t filter, void *tag_string, uint8_t enable)
{
    struct _bflog_tag *ps = &__bflog_tags_start__;
    struct _bflog_tag *pe = &__bflog_tags_end__;

    while (ps < pe) {
        if ((tag_string == ps->tag) || (0 == bflogc_strcmp(tag_string, ps->tag))) {
            if (enable) {
                ps->en |= filter;
            } else {
                ps->en &= ~filter;
            }
            return 0;
        }
        ps += 1;
    }

    return -1;
}

/**
 * @}
 */

/** @addtogroup BFLOG_BASE_API
 * @{
 */

int bflog_global_filter(void *tag_string, uint8_t enable)
{
    _BFLOG_CHECK(tag_string != NULL, -1);

    return bflog_filter_set(0xffffffff, tag_string, enable);
}

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

    uint32_t filter = bflog_filter_alloc();
    if (filter > 0) {
        log->filter = filter;
    } else {
        return -1;
    }

    return 0;
}

/**
 *   @brief         delete recorder, thread unsafe
 *   @param  log                    recorder
 *   @return int
 */
int bflog_delete(bflog_t *log)
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

    if (log->enter_critical()) {
        return -1;
    }

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

    bflog_filter_free(log->filter);

    log->exit_critical();

    return 0;
}

/**
 *   @brief         append directed output, thread safe
 *   @param  log                    recorder
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_append(bflog_t *log, bflog_direct_t *direct)
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

    if (log->enter_critical()) {
        return -1;
    }

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
int bflog_remove(bflog_t *log, bflog_direct_t *direct)
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

    if (log->enter_critical()) {
        return -1;
    }

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
int bflog_suspend(bflog_t *log)
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

    if (log->enter_critical()) {
        return -1;
    }

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
int bflog_resume(bflog_t *log)
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

    if (log->enter_critical()) {
        return -1;
    }

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
int bflog_control(bflog_t *log, uint32_t command, uint32_t param)
{
    _BFLOG_CHECK(log != NULL, -1);

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        case BFLOG_STATUS_RUNNING:
            if (command != BFLOG_CMD_LEVEL) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    switch (command) {
        case BFLOG_CMD_ENTER_CRITICAL:
            if ((void *)param == NULL) {
                log->enter_critical = dummy;
            } else {
                log->enter_critical = (void *)param;
            }
            return 0;
        case BFLOG_CMD_EXIT_CRITICAL: {
            if ((void *)param == NULL) {
                log->exit_critical = dummy;
            } else {
                log->exit_critical = (void *)param;
            }
        }
            return 0;
        default:
            break;
    }

    if (log->enter_critical()) {
        return -1;
    }

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_SUSPEND:
            break;
        case BFLOG_STATUS_RUNNING:
            if (command != BFLOG_CMD_LEVEL) {
                return -1;
            }
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
            log->level = param & BFLOG_LEVEL_MASK;
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
        case BFLOG_CMD_FLUSH_NOTICE:
            if ((void *)param == NULL) {
                log->flush_notice = dummy;
            } else {
                log->flush_notice = (void *)param;
            }
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
 *   @brief         config recorder filter, thread safe
 *   @param  log                    recorder
 *   @param  tag_string             tag string
 *   @param  enable                 tag enable
 *   @return int
 */
int bflog_filter(bflog_t *log, void *tag_string, uint8_t enable)
{
    _BFLOG_CHECK(log != NULL, -1);
    _BFLOG_CHECK(tag_string != NULL, -1);

    int ret;

    switch (log->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_RUNNING:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    if (log->enter_critical()) {
        return -1;
    }

    ret = bflog_filter_set(log->filter, tag_string, enable);

    if (log->exit_critical()) {
        return -1;
    }

    return ret;
}

/**
 *   @brief         record log msg, thread safe
 *                  tag, file, func only recorded pointer
 *   @param  log                    recorder
 *   @param  level                  level threshold
 *   @param  tag                    tag
 *   @param  file                   const file name
 *   @param  func                   const func name
 *   @param  line                   file line
 *   @param  format                 format string
 *   @param  ...                    format params
 *   @return int
 */
int bflog(void *log, uint8_t level, void *tag, const char *const file, const char *const func, const long line, const char *format, ...)
{
    _BFLOG_CHECK(log != NULL, -1);
    _BFLOG_CHECK(format != NULL, -1);

    int ret;
    va_list args;
    uint8_t msg[BFLOG_LINE_BUFFER_SIZE + sizeof(struct _bflog_msg)];
    uint16_t size = sizeof(struct _bflog_msg);

    /*!< working only during running and suspend */
    switch (_bflog_t(log)->status) {
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            return -1;
    }

    /*!< level filter */
    if ((level & BFLOG_LEVEL_MASK) > _bflog_t(log)->level) {
        return 0;
    }

    /*!< if advanced tag, use tag filter */
    if (((uintptr_t)(&__bflog_tags_start__) <= (uintptr_t)(tag)) &&
        ((uintptr_t)(tag) < (uintptr_t)(&__bflog_tags_end__))) {
        /*!< tag filter */
        if ((tag != NULL) &&
            ((_tag_t(tag)->en & _bflog_t(log)->filter) != _bflog_t(log)->filter)) {
            return 0;
        }
    }

    /*!< record clock tick */
    if (_bflog_t(log)->flags & BFLOG_FLAG_CLK) {
        _msg_t(msg)->clk = bflog_clock();
    } else {
        _msg_t(msg)->clk = 0;
    }

    /*!< record timestamp */
    if (_bflog_t(log)->flags & BFLOG_FLAG_TIME) {
        _msg_t(msg)->time = bflog_time();
    } else {
        _msg_t(msg)->time = 0;
    }

    /*!< record level */
    _msg_t(msg)->level = level;

    /*!< record tag, only record pointer */
    if (_bflog_t(log)->flags & BFLOG_FLAG_TAG) {
        _msg_t(msg)->tag = tag;
    } else {
        _msg_t(msg)->tag = NULL;
    }

    /*!< record func, only record pointer */
    if (_bflog_t(log)->flags & BFLOG_FLAG_FUNC) {
        _msg_t(msg)->func = func;
    } else {
        _msg_t(msg)->func = bflog_dummy_string;
    }

    /*!< record line */
    if (_bflog_t(log)->flags & BFLOG_FLAG_LINE) {
        _msg_t(msg)->line = line;
    } else {
        _msg_t(msg)->line = 0;
    }

    /*!< record file, only record pointer */
    if (_bflog_t(log)->flags & BFLOG_FLAG_FILE) {
        _msg_t(msg)->file = file;
    } else {
        _msg_t(msg)->file = bflog_dummy_string;
    }

    /*!< record thread, only record pointer */
    if (_bflog_t(log)->flags & BFLOG_FLAG_THREAD) {
        _msg_t(msg)->thread = bflog_thread();
    } else {
        _msg_t(msg)->thread = bflog_dummy_string;
    }

    /*!< set zero */
    _msg_t(msg)->zero = 0;

    /*!< print string to msg->string */
    va_start(args, format);
    ret = bflogc_vsnprintf(_msg_t(msg)->string, BFLOG_LINE_BUFFER_SIZE, format, args);
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

    if (_bflog_t(log)->enter_critical()) {
        return -1;
    }
    /*!< working only during running and suspend */
    switch (_bflog_t(log)->status) {
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            _bflog_t(log)->exit_critical();
            return -1;
    }

    queue_put(_bflog_t(log), (void *)(&msg));

    if (_bflog_t(log)->mode & BFLOG_MODE_ASYNC) {
        _bflog_t(log)->flush_notice();
        _bflog_t(log)->exit_critical();
        return 0;
    }

    _bflog_t(log)->exit_critical();

    return bflog_flush(_bflog_t(log));
}

/**
 *   @brief         flush all msg in queue, thread safe
 *   @param  log                    recorder
 *   @return int
 */
int bflog_flush(void *log)
{
    _BFLOG_CHECK(log != NULL, -1);

    uint8_t msg[BFLOG_LINE_BUFFER_SIZE + sizeof(struct _bflog_msg)];
    char buf[BFLOG_LINE_BUFFER_SIZE * 2];
    struct _bflog_list *node;
    void *direct;
    uint32_t filter = _bflog_t(log)->filter;

    /*!< working only during running */
    switch (_bflog_t(log)->status) {
        case BFLOG_STATUS_SUSPEND:
            return -1;
        case BFLOG_STATUS_RUNNING:
            break;
        default:
            return -1;
    }

    do {
        /*!< reset msg string */
        _msg_t(msg)->string[0] = '\0';

        if (_bflog_t(log)->enter_critical()) {
            return -1;
        }

        /*!< working only during running */
        switch (_bflog_t(log)->status) {
            case BFLOG_STATUS_SUSPEND:
                _bflog_t(log)->exit_critical();
                return -1;
            case BFLOG_STATUS_RUNNING:
                break;
            default:
                _bflog_t(log)->exit_critical();
                return -1;
        }

        /*!< get msg(on stack) from queue(on ram) */
        queue_get(_bflog_t(log), (void *)(&msg));

        _bflog_t(log)->exit_critical();

        if (_msg_t(msg)->zero != 0) {
            if (_msg_t(msg)->zero == 0xbd) {
                /*!< no msg */
                return 0;
            } else {
                /*!< error */
                return -1;
            }
        }

        /*!< color */
        char *color;
        color = bflog_color_strings[_msg_t(msg)->level & BFLOG_LEVEL_MASK];

        /*!< level */
        char *level;
        if (_bflog_t(log)->flags & BFLOG_FLAG_LEVEL) {
            level = bflog_level_strings[_msg_t(msg)->level & BFLOG_LEVEL_MASK];
        } else {
            level = bflog_dummy_string;
        }

#ifdef BFLOG_TIMESTAMP_ENABLE
        /*!< time */
        bflog_tm_t tm;

        bflog_unix2time(_msg_t(msg)->time, &tm);
#endif

        /*!< check if advanced tag */
        uint8_t advanced_tag;
        if (((uintptr_t)(&__bflog_tags_start__) <= (uintptr_t)(_msg_t(msg)->tag)) &&
            ((uintptr_t)(_msg_t(msg)->tag) < (uintptr_t)(&__bflog_tags_end__))) {
            advanced_tag = 1;
        } else {
            advanced_tag = 0;
        }

        _bflog_t(log)->enter_critical();
        /*!< foreach direct, execute layout to buf(on stack), then output buf(on stack) */
        BFLOG_DLIST_FOREACH_NEXT(node, &(_bflog_t(log)->direct))
        {
            _bflog_t(log)->exit_critical();

            int size;
            direct = BFLOG_DLIST_ENTRY(node, bflog_direct_t, list);

            /*!< check direct status */
            if (_direct_t(direct)->status != BFLOG_DIRECT_STATUS_RUNNING) {
                continue;
            }

            /*!< level filter */
            if ((_msg_t(msg)->level & BFLOG_LEVEL_MASK) > _direct_t(direct)->level) {
                continue;
            }

            if (advanced_tag) {
                /*!< tag filter */
                if ((_msg_t(msg)->tag != NULL) &&
                    ((_tag_t(_msg_t(msg)->tag)->en & filter) != filter)) {
                    continue;
                }
            }

            char *tag;
            if ((_msg_t(msg)->tag == NULL)) {
                tag = bflog_dummy_string;
            } else {
                tag = advanced_tag ? _tag_t(_msg_t(msg)->tag)->tag : _msg_t(msg)->tag;
            }

            /*!< raw output */
            if (_msg_t(msg)->level & BFLOG_LEVEL_RAW) {
                size = bflogc_snprintf(
                    buf,
                    BFLOG_LINE_BUFFER_SIZE * 2,
                    "%s", _msg_t(msg)->string);
                goto output;
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
                        size = _layout_format_t(_direct_t(direct)->layout)
                                   ->snprintf(
                                       buf,
                                       BFLOG_LINE_BUFFER_SIZE * 2,
                                       color,
                                       level,
                                       tag,
                                       &tm,
                                       _msg_t(msg));
#else
                        size = _layout_format_t(_direct_t(direct)->layout)
                                   ->snprintf(
                                       buf,
                                       BFLOG_LINE_BUFFER_SIZE * 2,
                                       color,
                                       level,
                                       tag,
                                       NULL,
                                       _msg_t(msg));
#endif
                    } else {
#ifdef BFLOG_TIMESTAMP_ENABLE
                        size = _layout_format_t(_direct_t(direct)->layout)
                                   ->snprintf(
                                       buf,
                                       BFLOG_LINE_BUFFER_SIZE * 2,
                                       bflog_dummy_string,
                                       level,
                                       tag,
                                       &tm,
                                       _msg_t(msg));
#else
                        size = _layout_format_t(_direct_t(direct)->layout)
                                   ->snprintf(
                                       buf,
                                       BFLOG_LINE_BUFFER_SIZE * 2,
                                       bflog_dummy_string,
                                       level,
                                       tag,
                                       NULL,
                                       _msg_t(msg));
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
                size = bflogc_snprintf(
                    buf,
                    BFLOG_LINE_BUFFER_SIZE * 2,
                    BFLOG_SIMPLE_LAYOUT_STRING(
                        color,
                        level,
                        tag,
                        &tm,
                        _msg_t(msg)));
#else
                size = bflogc_snprintf(
                    buf,
                    BFLOG_LINE_BUFFER_SIZE * 2,
                    BFLOG_SIMPLE_LAYOUT_STRING(
                        color,
                        level,
                        tag,
                        NULL,
                        _msg_t(msg)));
#endif

            } else {
                /*!< default and simple no color format */
#ifdef BFLOG_TIMESTAMP_ENABLE
                size = bflogc_snprintf(
                    buf,
                    BFLOG_LINE_BUFFER_SIZE * 2,
                    BFLOG_SIMPLE_LAYOUT_STRING(
                        bflog_dummy_string,
                        level,
                        tag,
                        &tm,
                        _msg_t(msg)));
#else
                size = bflogc_snprintf(
                    buf,
                    BFLOG_LINE_BUFFER_SIZE * 2,
                    BFLOG_SIMPLE_LAYOUT_STRING(
                        bflog_dummy_string,
                        level,
                        tag,
                        NULL,
                        _msg_t(msg)));
#endif
            }

        output:
            /*!< check true size */
            if ((size < 0) || (size > BFLOG_LINE_BUFFER_SIZE * 2)) {
                size = BFLOG_LINE_BUFFER_SIZE * 2;
            }

            if (_direct_t(direct)->lock()) {
                /*!< drop log message */
            } else {
                /*!< call write */
                _direct_t(direct)->write(direct, buf, size);

                _direct_t(direct)->unlock();
            }

            _bflog_t(log)->enter_critical();
        }

        _bflog_t(log)->exit_critical();
    } while (1);

    return -1;
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
int bflog_direct_create(bflog_direct_t *direct, uint8_t type, uint8_t color, void(*lock), void(*unlock))
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

    if ((lock == NULL) || (unlock == NULL)) {
        direct->lock = dummy;
        direct->unlock = dummy;
    } else {
        direct->lock = lock;
        direct->unlock = unlock;
    }

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

    if (direct->lock()) {
        return -1;
    }

    switch (direct->status) {
        case BFLOG_DIRECT_STATUS_INIT:
            break;
        default:
            direct->unlock();
            return -1;
    }

    bflog_dlist_remove(&(direct->list));

    direct->write = NULL;
    direct->status = BFLOG_DIRECT_STATUS_ILLEGAL;
    direct->type = BFLOG_DIRECT_TYPE_ILLEGAL;

    direct->lock = dummy;

    int (*unlock)(void) = direct->unlock;
    direct->unlock = dummy;

    bflog_filter_free(direct->filter);

    unlock();

    return 0;
}

/**
 *   @brief         suspend directed output, thread safe
 *   @param  direct                 directed output
 *   @return int
 */
int bflog_direct_suspend(bflog_direct_t *direct)
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
int bflog_direct_resume(bflog_direct_t *direct)
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

    if ((direct->status != BFLOG_DIRECT_STATUS_READY) &&
        (direct->status != BFLOG_DIRECT_STATUS_SUSPEND)) {
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
        case BFLOG_STATUS_RUNNING:
            if (command != BFLOG_DIRECT_CMD_LEVEL) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    switch (command) {
        case BFLOG_DIRECT_CMD_LEVEL:
            direct->level = param & BFLOG_LEVEL_MASK;
            break;

        case BFLOG_DIRECT_CMD_COLOR:
            direct->color = param != 0;
            break;

        case BFLOG_DIRECT_CMD_LOCK:
            if ((void *)param == NULL) {
                direct->lock = dummy;
            } else {
                direct->lock = (void *)param;
            }
            break;

        case BFLOG_DIRECT_CMD_UNLOCK:
            if ((void *)param == NULL) {
                direct->unlock = dummy;
            } else {
                direct->unlock = (void *)param;
            }
            break;

        default:
            return -1;
    }

    return 0;
}

/**
 *   @brief         config direct filter, thread safe
 *   @param  direct                 direct
 *   @param  tag_string             tag string
 *   @param  enable                 tag enable
 *   @return int
 */
int bflog_direct_filter(bflog_direct_t *direct, void *tag_string, uint8_t enable)
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(tag_string != NULL, -1);

    int ret;

    switch (direct->status) {
        case BFLOG_STATUS_READY:
        case BFLOG_STATUS_RUNNING:
        case BFLOG_STATUS_SUSPEND:
            break;
        default:
            return -1;
    }

    ret = bflog_filter_set(direct->filter, tag_string, enable);

    return ret;
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

int bflog_direct_init_buffer_s(bflog_direct_t *direct, void *buffer, void *size)
{
    /*!< TODO */
    (void)bflog_driect_write_buffer;
    return 0;
}

int bflog_direct_deinit_buffer_s(bflog_direct_t *direct)
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
 *   @brief         init stream type direct, thread safe
 *   @param  direct                 directed output
 *   @param  stream_output          stream output function pointer
 *   @return int
 */
int bflog_direct_init_stream(bflog_direct_t *direct, uint16_t (*stream_output)(void *, uint16_t))
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(stream_output != NULL, -1);

    if (direct->lock()) {
        return -1;
    }

    if (direct->type != BFLOG_DIRECT_TYPE_STREAM) {
        direct->unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        direct->unlock();
        return -1;
    }

    _direct_stream_t(direct)->stream_output = stream_output;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_stream;

    direct->unlock();

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

    if (direct->type != BFLOG_DIRECT_TYPE_STREAM) {
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_READY) {
        return -1;
    }

    if (direct->lock()) {
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;

    _direct_stream_t(direct)->stream_output = NULL;

    direct->unlock();

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

    bflogc_fwrite(ptr, 1, size, _direct_file_t(direct)->fp);
    bflogc_fflush(_direct_file_t(direct)->fp);
}

/**
 *   @brief         
 *   @param  direct                 
 *   @param  path                                 
 *   @return int 
 */
int bflog_direct_init_file(bflog_direct_t *direct, const char *path)
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(path != NULL, -1);

    char fullpath[256];

    if (direct->lock()) {
        return -1;
    }

    if (direct->type != BFLOG_DIRECT_TYPE_FILE) {
        direct->unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        direct->unlock();
        return -1;
    }

    size_t pathsize = strlen(path);
    bflogc_memcpy(fullpath, path, pathsize);
    bflogc_snprintf(fullpath + pathsize, 16, ".log");

    _direct_file_t(direct)->fp = bflogc_fopen(fullpath, "a+");
    if (_direct_file_t(direct)->fp == NULL) {
        direct->unlock();
        return -1;
    }

    _direct_file_t(direct)->path = path;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_file;

    direct->unlock();

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

    if (direct->lock()) {
        return -1;
    }

    if (bflogc_fclose(_direct_file_t(direct)->fp)) {
        direct->unlock();
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;

    _direct_file_t(direct)->fp = NULL;
    _direct_file_t(direct)->path = NULL;

    direct->unlock();

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
    bflogc_memcpy(oldpath, direct->path, pathsize);
    bflogc_memcpy(newpath, direct->path, pathsize);

    if (direct->fp) {
        bflogc_fclose(direct->fp);
    }

    for (int i = direct->keep - 1; i >= 0; i--) {
        bflogc_snprintf(oldpath + pathsize, 16, i ? "_%d.log" : ".log", i - 1);
        bflogc_snprintf(newpath + pathsize, 16, "_%d.log", i);

        fp = bflogc_fopen(newpath, "r");
        if (fp != NULL) {
            bflogc_fclose(fp);
            bflogc_remove(newpath);
        }

        fp = bflogc_fopen(oldpath, "r");
        if (fp != NULL) {
            bflogc_fclose(fp);
            bflogc_rename(oldpath, newpath);
        }
    }

    direct->fp = bflogc_fopen(oldpath, "a+");
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

    bflogc_fwrite(ptr, 1, size, _direct_file_size_t(direct)->fp);
    bflogc_fflush(_direct_file_size_t(direct)->fp);
}

/**
 *   @brief         
 *   @param  direct                 directed output
 *   @param  path                   file path
 *   @param  interval               file rotate interval
 *   @param  keep                   max keep file count
 *   @return int 
 */
int bflog_direct_init_file_time(bflog_direct_t *direct, const char *path, uint32_t interval, uint32_t keep)
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(path != NULL, -1);
    _BFLOG_CHECK(interval >= BFLOG_FILE_INTERVAL_MIN, -1);
    _BFLOG_CHECK(keep > 0, -1);

    if (interval < BFLOG_FILE_INTERVAL_MIN) {
        interval = BFLOG_FILE_INTERVAL_MIN;
    }

    char fullpath[256];

    if (direct->lock()) {
        return -1;
    }

    if (direct->type != BFLOG_DIRECT_TYPE_FILE_TIME) {
        direct->unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        direct->unlock();
        return -1;
    }

    size_t pathsize = strlen(path);
    bflogc_memcpy(fullpath, path, pathsize);
    bflogc_snprintf(fullpath + pathsize, 16, ".log");

    _direct_file_time_t(direct)->fp = bflogc_fopen(fullpath, "a+");
    if (_direct_file_time_t(direct)->fp == NULL) {
        direct->unlock();
        return -1;
    }

    _direct_file_time_t(direct)->timestamp = bflog_time();
    _direct_file_time_t(direct)->keep = keep;
    _direct_file_time_t(direct)->interval = interval;
    _direct_file_time_t(direct)->path = path;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_file_time;

    direct->unlock();

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

    if (direct->lock()) {
        return -1;
    }

    if (bflogc_fclose(_direct_file_time_t(direct)->fp)) {
        direct->unlock();
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;
    _direct_file_time_t(direct)->fp = NULL;
    _direct_file_time_t(direct)->path = NULL;
    _direct_file_time_t(direct)->keep = 0;

    direct->unlock();

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
    size_t fsize = bflogc_ftell(_direct_file_size_t(direct)->fp);

    if (fsize > _direct_file_size_t(direct)->size) {
        file_rotate((void *)direct);
    }

    bflogc_fwrite(ptr, 1, size, _direct_file_size_t(direct)->fp);
    bflogc_fflush(_direct_file_size_t(direct)->fp);
}

/**
 *   @brief         
 *   @param  direct                 
 *   @param  path                   
 *   @param  size                   
 *   @param  keep                                
 *   @return int 
 */
int bflog_direct_init_file_size(bflog_direct_t *direct, const char *path, uint32_t size, uint32_t keep)
{
    _BFLOG_CHECK(direct != NULL, -1);
    _BFLOG_CHECK(path != NULL, -1);
    _BFLOG_CHECK(keep > 0, -1);

    if (size < BFLOG_FILE_SIZE_MIN) {
        size = BFLOG_FILE_SIZE_MIN;
    }

    char fullpath[256];

    if (direct->lock()) {
        return -1;
    }

    if (direct->type != BFLOG_DIRECT_TYPE_FILE_SIZE) {
        direct->unlock();
        return -1;
    }

    if (direct->status != BFLOG_DIRECT_STATUS_INIT) {
        direct->unlock();
        return -1;
    }

    size_t pathsize = strlen(path);
    bflogc_memcpy(fullpath, path, pathsize);
    bflogc_snprintf(fullpath + pathsize, 16, ".log");

    _direct_file_size_t(direct)->fp = bflogc_fopen(fullpath, "a+");
    if (_direct_file_size_t(direct)->fp == NULL) {
        direct->unlock();
        return -1;
    }

    _direct_file_size_t(direct)->keep = keep;
    _direct_file_size_t(direct)->size = size;
    _direct_file_size_t(direct)->path = path;

    direct->status = BFLOG_DIRECT_STATUS_READY;
    direct->write = bflog_direct_write_file_size;

    direct->unlock();

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

    if (direct->lock()) {
        return -1;
    }

    if (bflogc_fclose(_direct_file_size_t(direct)->fp)) {
        direct->unlock();
        return -1;
    }

    direct->status = BFLOG_DIRECT_STATUS_INIT;
    direct->write = NULL;
    _direct_file_size_t(direct)->fp = NULL;
    _direct_file_size_t(direct)->path = NULL;
    _direct_file_size_t(direct)->keep = 0;

    direct->unlock();

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
int bflog_layout_format(bflog_layout_t *layout, int (*u_snprintf)(void *ptr, uint16_t size, char *color, char *level, char *tag, bflog_tm_t *tm, struct _bflog_msg *msg))
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
