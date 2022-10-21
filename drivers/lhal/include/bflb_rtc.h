#ifndef _BFLB_RTC_H
#define _BFLB_RTC_H

#include "bflb_core.h"

#define BFLB_RTC_SEC2TIME(s)    (s * 32768)
#define BFLB_RTC_TIME2SEC(time) (time / 32768)

#ifdef __cplusplus
extern "C" {
#endif

void bflb_rtc_set_time(struct bflb_device_s *dev, uint64_t time);
uint64_t bflb_rtc_get_time(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

#endif