/**
 * @file bflb_timestamp.c
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

#include "bflb_timestamp.h"
#include <stdlib.h>

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
static void cal_weekday(bflb_timestamp_t *time)
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
void bflb_timestamp_utc2time(uint32_t utc, bflb_timestamp_t *time)
{
#define BFLB_TIMESTAMP_FOUR_YEAR_DAY      ((365 << 2) + 1)
#define BFLB_TIMESTAMP_SEC_NUM_PER_DAY    (24 * 60 * 60)
#define BFLB_TIMESTAMP_SEC_NUM_PER_HOUR   (60 * 60)
#define BFLB_TIMESTAMP_SEC_NUM_PER_MINUTE (60)

#ifndef BFLB_TIMESTAMP_TIMEZONE
#define BFLB_TIMESTAMP_TIMEZONE 8
#endif

    uint32_t total_day_num;
    uint32_t current_sec_num;

    uint16_t remain_day;

    uint16_t temp_year;

    const uint8_t *p = NULL;

    total_day_num = utc / BFLB_TIMESTAMP_SEC_NUM_PER_DAY;
    current_sec_num = utc % BFLB_TIMESTAMP_SEC_NUM_PER_DAY;

    time->hour = current_sec_num / BFLB_TIMESTAMP_SEC_NUM_PER_HOUR;
    time->min = (current_sec_num % BFLB_TIMESTAMP_SEC_NUM_PER_HOUR) / BFLB_TIMESTAMP_SEC_NUM_PER_MINUTE;
    time->sec = (current_sec_num % BFLB_TIMESTAMP_SEC_NUM_PER_HOUR) % BFLB_TIMESTAMP_SEC_NUM_PER_MINUTE;

    time->hour += BFLB_TIMESTAMP_TIMEZONE;

    if (time->hour > 23) {
        time->hour -= 24;
        total_day_num++;
    }

    time->year = 1970 + (total_day_num / BFLB_TIMESTAMP_FOUR_YEAR_DAY) * 4;
    remain_day = total_day_num % BFLB_TIMESTAMP_FOUR_YEAR_DAY;

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
