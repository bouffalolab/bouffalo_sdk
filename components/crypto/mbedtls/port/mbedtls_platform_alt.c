#include "mbedtls/platform_time.h"
#include <bflb_mtimer.h>

mbedtls_ms_time_t mbedtls_ms_time(void)
{
    return bflb_mtimer_get_time_ms();
}
