#ifndef BL_P2P_COMPAT_COMMON_H
#define BL_P2P_COMPAT_COMMON_H

#include "../utils/common.h"

static inline int os_reltime_expired(const struct os_reltime *now,
				     const struct os_reltime *ts,
				     os_time_t timeout_secs)
{
	return now->sec > ts->sec + timeout_secs ||
		(now->sec == ts->sec + timeout_secs &&
		 now->usec >= ts->usec);
}

#ifndef BAND_2_4_GHZ
#define BAND_2_4_GHZ BIT(0)
#define BAND_5_GHZ BIT(1)
#define BAND_60_GHZ BIT(2)
#endif

size_t utf8_escape(const char *inp, size_t in_size,
		   char *outp, size_t out_size);

#endif
