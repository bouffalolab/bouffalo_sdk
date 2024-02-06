#include "bflb_rtc.h"
#include "hardware/rtc_reg.h"

#if defined(BL602) || defined(BL702) || defined(BL702L)
#define BFLB_RTC_BASE 0x4000F000
#elif defined(BL808) || defined(BL606P) || defined(BL616)
#define BFLB_RTC_BASE 0x2000F000
#elif defined(BL628)
#define BFLB_RTC_BASE 0x2008F000
#endif

void bflb_rtc_disable(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_rtc_disable
    romapi_bflb_rtc_disable(dev);
#else
    uint32_t reg_base;
    uint32_t regval;

    reg_base = BFLB_RTC_BASE;

    /* Clear & Disable RTC counter */
    regval = getreg32(reg_base + HBN_CTL_OFFSET);
    regval &= ~HBN_RTC_ENABLE;
    putreg32(regval, reg_base + HBN_CTL_OFFSET);
#endif
}

void bflb_rtc_set_time(struct bflb_device_s *dev, uint64_t time)
{
#ifdef romapi_bflb_rtc_set_time
    romapi_bflb_rtc_set_time(dev, time);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint64_t rtc_cnt;

    reg_base = BFLB_RTC_BASE;

    /* Clear RTC Control */
    regval = getreg32(reg_base + HBN_CTL_OFFSET);
    regval &= ~HBN_RTC_CTL_MASK;
    putreg32(regval, reg_base + HBN_CTL_OFFSET);

    regval |= HBN_RTC_DLY_OPTION;
    regval |= HBN_RTC_BIT39_0_COMPARE;
    putreg32(regval, reg_base + HBN_CTL_OFFSET);

    /* Tigger RTC val read */
    regval = getreg32(reg_base + HBN_RTC_TIME_H_OFFSET);
    regval |= HBN_RTC_TIME_LATCH;
    putreg32(regval, reg_base + HBN_RTC_TIME_H_OFFSET);
    regval &= ~HBN_RTC_TIME_LATCH;
    putreg32(regval, reg_base + HBN_RTC_TIME_H_OFFSET);

    /* Read RTC val */
    rtc_cnt = getreg32(reg_base + HBN_RTC_TIME_H_OFFSET) & 0xff;
    rtc_cnt <<= 32;
    rtc_cnt |= getreg32(reg_base + HBN_RTC_TIME_L_OFFSET);

    /* calculate RTC Comp time */
    rtc_cnt += time;

    /* Set RTC Comp time  */
    putreg32((uint32_t)rtc_cnt, reg_base + HBN_TIME_L_OFFSET);
    putreg32((uint32_t)(rtc_cnt >> 32) & 0xff, reg_base + HBN_TIME_H_OFFSET);

    /* Enable RTC Counter */
    regval = getreg32(reg_base + HBN_CTL_OFFSET);
    regval |= HBN_RTC_ENABLE;
    putreg32(regval, reg_base + HBN_CTL_OFFSET);
#endif
}

uint64_t bflb_rtc_get_time(struct bflb_device_s *dev)
{
#ifdef romapi_bflb_rtc_get_time
    return romapi_bflb_rtc_get_time(dev);
#else
    uint32_t reg_base;
    uint32_t regval;
    uint64_t time_l;
    uint64_t time_h;

    reg_base = BFLB_RTC_BASE;

    /* Tigger RTC val read */
    regval = getreg32(reg_base + HBN_RTC_TIME_H_OFFSET);
    regval |= HBN_RTC_TIME_LATCH;
    putreg32(regval, reg_base + HBN_RTC_TIME_H_OFFSET);
    regval &= ~HBN_RTC_TIME_LATCH;
    putreg32(regval, reg_base + HBN_RTC_TIME_H_OFFSET);

    /* Read RTC val */
    time_l = getreg32(reg_base + HBN_RTC_TIME_L_OFFSET);
    time_h = getreg32(reg_base + HBN_RTC_TIME_H_OFFSET) & 0xff;

    return (((uint64_t)time_h << 32) | (uint64_t)time_l);
#endif
}

#define SEC_PER_MIN  ((time_t)60)
#define SEC_PER_HOUR ((time_t)60 * SEC_PER_MIN)
#define SEC_PER_DAY  ((time_t)24 * SEC_PER_HOUR)

static const uint16_t g_noleap_daysbeforemonth[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

static inline int clock_isleapyear(int year)
{
    return (year % 400) ? ((year % 100) ? ((year % 4) ? 0 : 1) : 0) : 1;
}

static int clock_daysbeforemonth(int month, uint8_t leapyear)
{
    int retval = g_noleap_daysbeforemonth[month];

    if (month >= 2 && leapyear) {
        retval++;
    }

    return retval;
}

static void clock_utc2calendar(time_t days, int *year, int *month,
                               int *day)
{
    /* There is one leap year every four years, so we can get close with the
     * following:
     */

    int value = days / (4 * 365 + 1); /* Number of 4-years periods since the epoch */
    days -= value * (4 * 365 + 1);    /* Remaining days */
    value <<= 2;                      /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    uint8_t leapyear;
    int tmp;

    for (;;) {
        /* Is this year a leap year (we'll need this later too) */

        leapyear = clock_isleapyear(value + 1970);

        /* Get the number of days in the year */

        tmp = (leapyear ? 366 : 365);

        /* Do we have that many days? */

        if (days >= tmp) {
            /* Yes.. bump up the year */

            value++;
            days -= tmp;
        } else {
            /* Nope... then go handle months */

            break;
        }
    }

    /* At this point, value has the year and days has number days into this year */

    *year = 1970 + value;

    /* Handle the month (zero based) */
    int min = 0;
    int max = 11;

    do {
        /* Get the midpoint */

        value = (min + max) >> 1;

        /* Get the number of days that occurred before the beginning of the month
         * following the midpoint.
         */

        tmp = clock_daysbeforemonth(value + 1, leapyear);

        /* Does the number of days before this month that equal or exceed the
         * number of days we have remaining?
         */

        if (tmp > days) {
            /* Yes.. then the month we want is somewhere from 'min' and to the
             * midpoint, 'value'.  Could it be the midpoint?
             */

            tmp = clock_daysbeforemonth(value, leapyear);

            if (tmp > days) {
                /* No... The one we want is somewhere between min and value-1 */

                max = value - 1;
            } else {
                /* Yes.. 'value' contains the month that we want */

                break;
            }
        } else {
            /* No... The one we want is somwhere between value+1 and max */

            min = value + 1;
        }

        /* If we break out of the loop because min == max, then we want value
         * to be equal to min == max.
         */

        value = min;
    } while (min < max);

    /* The selected month number is in value. Subtract the number of days in the
     * selected month
     */
    days -= clock_daysbeforemonth(value, leapyear);

    /* At this point, value has the month into this year (zero based) and days has
     * number of days into this month (zero based)
     */

    *month = value + 1; /* 1-based */
    *day = days + 1;    /* 1-based */
}

static time_t __clock_calendar2utc(int year, int month, int day)
{
    time_t days;

    /* Years since epoch in units of days (ignoring leap years). */

    days = (year - 1970) * 365;

    /* Add in the extra days for the leap years prior to the current year. */

    days += (year - 1969) >> 2;

    /* Add in the days up to the beginning of this month. */

    days += (time_t)clock_daysbeforemonth(month, clock_isleapyear(year));

    /* Add in the days since the beginning of this month (days are 1-based). */

    days += day - 1;

    /* Then convert the seconds and add in hours, minutes, and seconds */

    return days;
}

static int get_week_day(uint16_t y, uint8_t m, uint8_t d)
{
    int week;

    if (m == 1 || m == 2) {
        m += 12;
        y -= 1;
    }

    week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

    return week + 1;
}

static struct bflb_tm *__gmtime_r(const time_t *timer, struct bflb_tm *result)
{
    time_t epoch;
    time_t jdn;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;

    /* Get the seconds since the EPOCH */

    epoch = *timer;

    /* Convert to days, hours, minutes, and seconds since the EPOCH */

    jdn = epoch / SEC_PER_DAY;
    epoch -= SEC_PER_DAY * jdn;

    hour = epoch / SEC_PER_HOUR;
    epoch -= SEC_PER_HOUR * hour;

    min = epoch / SEC_PER_MIN;
    epoch -= SEC_PER_MIN * min;

    sec = epoch;

    /* Convert the days since the EPOCH to calendar day */
    clock_utc2calendar(jdn, &year, &month, &day);

    /* Then return the struct tm contents */

    result->tm_year = (int)year - 1900; /* Relative to 1900 */
    result->tm_mon = (int)month - 1;    /* zero-based */
    result->tm_mday = (int)day;         /* one-based */
    result->tm_wday = get_week_day(year, month, day);
    result->tm_hour = (int)hour;
    result->tm_min = (int)min;
    result->tm_sec = (int)sec;
    return result;
}

static time_t __mktime(struct bflb_tm *tp)
{
    time_t ret;
    time_t jdn;

    /* Get the EPOCH-relative julian date from the calendar year,
     * month, and date
     */

    jdn = __clock_calendar2utc(tp->tm_year + 1900, tp->tm_mon, tp->tm_mday);

    /* Return the seconds into the julian day. */

    ret = ((jdn * 24 + tp->tm_hour) * 60 + tp->tm_min) * 60 + tp->tm_sec;

    return ret;
}

#define BL_RTC_COUNTER_TO_MS(CNT) ((uint64_t)(CNT)*1000 / 32768) // ((CNT)*(1024-16-8)/32768)
#define BL_RTC_MAX_COUNTER        (0x000000FFFFFFFFFFllu)

uint64_t bl_rtc_get_delta_counter(uint64_t ref_cnt)
{
    uint64_t cnt;

    cnt = bflb_rtc_get_time(NULL);
    ref_cnt &= BL_RTC_MAX_COUNTER;

    if (cnt < ref_cnt) {
        cnt += BL_RTC_MAX_COUNTER + 1;
    }

    return cnt - ref_cnt;
}

uint64_t bl_rtc_get_delta_time_ms(uint64_t ref_cnt)
{
    uint64_t cnt;

    cnt = bl_rtc_get_delta_counter(ref_cnt);

    return BL_RTC_COUNTER_TO_MS(cnt);
}

static volatile struct bflb_tm g_rtc_tm;
static volatile uint64_t s_rtc_ref_cnt = 0;

void bflb_rtc_set_utc_time(const struct bflb_tm *time)
{
    memcpy((void *)&g_rtc_tm, time, sizeof(struct bflb_tm));
    s_rtc_ref_cnt = bflb_rtc_get_time(NULL);
}

void bflb_rtc_get_utc_time(struct bflb_tm *time)
{
    uint64_t time_stamp_ms;

    time_stamp_ms = bl_rtc_get_delta_time_ms(s_rtc_ref_cnt);
    time_stamp_ms = time_stamp_ms / 1000;
    time_stamp_ms += __mktime((struct bflb_tm *)&g_rtc_tm);
    __gmtime_r((const time_t *)&time_stamp_ms, time);
}