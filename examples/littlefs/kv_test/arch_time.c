#include "FreeRTOS.h"

#include "arch_time.h"

/* Time */

uint64_t arch_os_time_system(void)
{
    uint64_t curr;

    curr = xTaskGetTickCount();

    return curr;
}

uint32_t arch_os_ms_now(void)
{
    return (uint32_t)arch_os_time_system();
}

uint32_t arch_os_time_now(void)
{
    return arch_os_ms_now() / 1000;
}

#if 0
uint32_t arch_os_utc_now(void)
{
  struct timespec ts;

  clock_gettime(CLOCK_REALTIME, &ts);

  return (uint32_t)ts.tv_sec;
}

int arch_os_utc_set(uint32_t utc)
{
  struct timespec ts;

  ts.tv_sec = utc;
  ts.tv_nsec = 0;

  return clock_settime(CLOCK_REALTIME, &ts);
}
#else
/*
	UTC Mechanism:
    Maintain a local UTC clock based on the system time and UTC offset.
    Setting the UTC time will calibrate the local clock.

*/
static uint32_t utc_offset = 0;

uint32_t arch_os_utc_now(void)
{
    return utc_offset + arch_os_time_now();
}

//Calibrate the system with UTC standard time.
// Return system time offset (positive value means system time is ahead of UTC time).
int arch_os_utc_set(uint32_t utc)
{
    int ret = 0;
    uint32_t time_now = arch_os_time_now();
    struct timespec ts;

    if (utc_offset != 0) { // Not the first time reading standard time.

        ret = utc_offset + time_now - utc; // Difference between local time and standard time.

        // Temporarily do not adjust the slope, keep the simple strategy.
        //arch_os_time_tune(utc - utc_offset);	// Adjust the system time with the standard time interval.

        utc_offset = utc - time_now; // Set the standard time again.
    } else
        utc_offset = utc - time_now; // Set the standard time for the first time.

    /* set CLOCK_REALTIME */

    vTaskStepTick(time_now);

    return ret;
}
#endif

uint32_t arch_os_ms_elapsed(uint32_t last_ms)
{
    uint32_t now = arch_os_ms_now();

    if (last_ms < now)
        return now - last_ms;
    else
        return 0xFFFFFFFF - last_ms + now + 1;
}

void arch_os_ms_sleep(uint32_t ms)
{
    vTaskDelay(ms);
}
