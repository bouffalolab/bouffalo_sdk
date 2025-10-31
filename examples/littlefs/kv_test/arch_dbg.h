
#ifndef __ARCH_DBG_H__
#define __ARCH_DBG_H__


#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "arch_time.h"
#include "arch_os.h"

#include <log.h>
#ifdef LOG_INFO
#undef LOG_INFO
#endif

#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif

static inline char * arch_printf_time(void)
{
    struct tm *dtime;
    int gmt_offset = 0;
    time_t cur;
    static char time[256];

    memset(time, 0, sizeof(time));
    cur = arch_os_utc_now();
    dtime = gmtime((const time_t *)&cur);
    snprintf(time, sizeof(time), "%02d:%02d:%02d.%03d ", dtime->tm_hour, dtime->tm_min, dtime->tm_sec, arch_os_ms_now() % 1000);

    return time;
}

#define LOG_INFO_TAG(TAG, _fmt_, ...)                                            \
    do {                                                                         \
        LOG_I("[%s] %s: " _fmt_ "\r\n", arch_printf_time(), TAG, ##__VA_ARGS__); \
    } while (0)

#define LOG_WARN_TAG(TAG, _fmt_, ...)                                                                           \
    do {                                                                                                        \
        LOG_W("[%s] %s: (%s,%d)" _fmt_ "\r\n", arch_printf_time(), TAG, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define LOG_ERROR_TAG(TAG, _fmt_, ...)                                                                          \
    do {                                                                                                        \
        LOG_E("[%s] %s: (%s,%d)" _fmt_ "\r\n", arch_printf_time(), TAG, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define LOG_DEBUG_TAG(TAG, _fmt_, ...)                                           \
    do {                                                                         \
        LOG_D("[%s] %s: " _fmt_ "\r\n", arch_printf_time(), TAG, ##__VA_ARGS__); \
    } while (0)

#endif  /* __APPLN_DBG_H__ */

