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
	utc机制:
	基于time机制 和 utc_offset 本地维持一个utc时钟
	设置utc 时间将用于校准time

*/
static uint32_t utc_offset = 0;

uint32_t arch_os_utc_now(void)
{
    return utc_offset + arch_os_time_now();
}

//通过utc标准时间 校准系统，
//返回系统时间偏移 (正数 表示系统时间快过UTC时间值)
int arch_os_utc_set(uint32_t utc)
{
    int ret = 0;
    uint32_t time_now = arch_os_time_now();
    struct timespec ts;

    if (utc_offset != 0) { //非第一次读取标准时间

        ret = utc_offset + time_now - utc; //本地时间 和 标准时间 差

        //暂时不调整斜率，保持简单策略
        //arch_os_time_tune(utc - utc_offset);	//用标准时间间隔 调整系统时间

        utc_offset = utc - time_now; //再一次设置标准时间
    } else
        utc_offset = utc - time_now; //第一次设置标准时间

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
