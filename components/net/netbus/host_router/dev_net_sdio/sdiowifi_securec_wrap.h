
#ifndef __SDIOWIFI_SECUREC_WRAP_H__
#define __SDIOWIFI_SECUREC_WRAP_H__

#ifdef SAFE_FUCNTION_WITH_BUFFER_LENGTH
#include <securec_wrap.h>
#else
#define MEMSET_SAFE(d, d_len, c, s_len) memset(d, c, d_len)
#define MEMCPY_SAFE(d, d_len, s, s_len) memcpy(d, s, d_len)
#endif

#endif 

