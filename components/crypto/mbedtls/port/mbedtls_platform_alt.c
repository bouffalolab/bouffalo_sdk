#include "mbedtls/platform_time.h"
#include <qcc74x_mtimer.h>

mbedtls_ms_time_t mbedtls_ms_time(void)
{
    return qcc74x_mtimer_get_time_ms();
}
