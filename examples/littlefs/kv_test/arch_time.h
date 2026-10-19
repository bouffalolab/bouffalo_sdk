#ifndef __ARCH_TIME_H__
#define __ARCH_TIME_H__

#include <stdint.h>

uint32_t arch_os_ms_now(void);
void     arch_os_ms_sleep(uint32_t ms);
uint32_t arch_os_ms_elapsed(uint32_t last_ms);
uint32_t arch_os_time_now(void);
uint64_t arch_os_time_system(void);
int      arch_os_utc_set(uint32_t utc);
uint32_t arch_os_utc_now(void);

#endif  /* __ARCH_TIME_H__ */
