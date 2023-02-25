#include <errno.h>
#include <reent.h>
#include <sys/time.h>

#include "bflb_rtc.h"

static struct bflb_device_s *rtc = NULL;

static uint32_t timestamp_base = 0;
static struct timezone tz = {
    .tz_dsttime = 0,
    .tz_minuteswest = 0
};

struct tms {
    clock_t tms_utime;  /* user time */
    clock_t tms_stime;  /* system time */
    clock_t tms_cutime; /* user time, children */
    clock_t tms_cstime; /* system time, children */
};

/*****************************************************************************
* @brief        get times
* 
* @param[in]    reent       pointer to reentrant struct
* @param[out]   ptms        pointer to tms struct
* 
* @retval clock_t           timeval now
*****************************************************************************/
clock_t _times_r(struct _reent *reent, struct tms *ptms)
{
    clock_t timeval = bflb_mtimer_get_time_us();

    if (ptms) {
        ptms->tms_utime = timeval; /* user time */
        ptms->tms_stime = 0;       /* system time */
        ptms->tms_cutime = 0;      /* user time, children */
        ptms->tms_cstime = 0;      /* system time, children */
    }

    return timeval;
}

/*****************************************************************************
* @brief        
* 
* @param[in]    reent       pointer to reentrant struct
* @param[out]   tp          pointer to timeval struct
* @param[out]   tzvp        pointer to timezone struct
* 
* @retval int               
*****************************************************************************/
int _gettimeofday_r(struct _reent *reent, struct timeval *tp, void *tzvp)
{
    struct timezone *tzp = tzvp;

    if (rtc == NULL) {
        rtc = bflb_device_get_by_name("rtc");
    }

    if (tp) {
        uint64_t ticks = bflb_rtc_get_time(rtc);
        tp->tv_sec = ticks / 32768 + timestamp_base;
        tp->tv_usec = (ticks & 0x7fff) * 31;
    }

    if (tzp) {
        tzp->tz_minuteswest = tz.tz_minuteswest;
        tzp->tz_dsttime = tz.tz_dsttime;
    }

    return 0;
}

/*****************************************************************************
* @brief        
* 
* @param[in]    reent       pointer to reentrant struct
* @param[in]    tp          pointer to timeval struct
* @param[in]    tzp         pointer to timezone struct
* 
* @retval int               
*****************************************************************************/
int _settimeofday_r(struct _reent *reent, const struct timeval *tp, const struct timezone *tzp)
{
    if (rtc == NULL) {
        rtc = bflb_device_get_by_name("rtc");
    }

    if (tp) {
        bflb_rtc_set_time(rtc, tp->tv_usec / 31);
        timestamp_base = tp->tv_sec;
    }

    if (tzp) {
        tz.tz_dsttime = tzp->tz_dsttime;
        tz.tz_minuteswest = tzp->tz_minuteswest;
    }

    return 0;
}

int settimeofday(const struct timeval *tp, const struct timezone *tzp)
{
    return _settimeofday_r(_REENT, tp, tzp);
}
