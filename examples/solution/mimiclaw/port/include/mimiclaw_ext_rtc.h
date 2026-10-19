#ifndef MIMICLAW_EXT_RTC_H
#define MIMICLAW_EXT_RTC_H

#include <stdbool.h>

#include "bflb_rtc.h"

bool mimiclaw_ext_rtc_is_available(void);
bool mimiclaw_ext_rtc_read_utc(struct bflb_tm *tm);
bool mimiclaw_ext_rtc_write_utc(const struct bflb_tm *tm);

#endif
