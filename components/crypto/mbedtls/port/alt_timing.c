
#include <sys/time.h>
#include "mbedtls/timing.h"
#include <stdio.h>

#ifndef CONFIG_MBEDTLS_V3
#define MBEDTLS_PRIVATE(member) member
#endif

struct _hr_time
{
    struct timeval start;
};

#ifndef CONFIG_NEWLIB
#include <bflb_rtc.h>
static struct bflb_device_s *rtc = NULL;
static uint32_t timestamp_base = 0;
static struct timezone tz = {
    .tz_dsttime = 0,
    .tz_minuteswest = 0
};

int _gettimeofday_r(struct _reent *reent, struct timeval *tp, void *tzvp)
{
    struct timezone *tzp = tzvp;

    if (rtc == NULL) {
        rtc = bflb_device_get_by_name("rtc");
    }

    if (tp) {
        uint64_t ticks = bflb_rtc_get_time(rtc);
        tp->tv_sec = ticks / 32768 + timestamp_base;
        tp->tv_usec = (ticks & 0x7fff) * 31;
    }

    if (tzp) {
        tzp->tz_minuteswest = tz.tz_minuteswest;
        tzp->tz_dsttime = tz.tz_dsttime;
    }

    return 0;
}
#endif

unsigned long mbedtls_timing_get_timer( struct mbedtls_timing_hr_time *val, int reset )
{
    struct _hr_time *t = (struct _hr_time *) val;

    if( reset )
    {
        gettimeofday( &t->start, NULL );
        return( 0 );
    }
    else
    {
        unsigned long delta;
        struct timeval now;
        gettimeofday( &now, NULL );
        delta = ( now.tv_sec  - t->start.tv_sec  ) * 1000ul
              + ( now.tv_usec - t->start.tv_usec ) / 1000;
        return( delta );
    }
}

/*
 * Set delays to watch
 */
void mbedtls_timing_set_delay( void *data, uint32_t int_ms, uint32_t fin_ms )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

    ctx->MBEDTLS_PRIVATE(int_ms) = int_ms;
    ctx->MBEDTLS_PRIVATE(fin_ms) = fin_ms;

    if( fin_ms != 0 )
        (void) mbedtls_timing_get_timer( &ctx->MBEDTLS_PRIVATE(timer), 1 );
}

/*
 * Get number of delays expired
 */
int mbedtls_timing_get_delay( void *data )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
    unsigned long elapsed_ms;

    if( ctx->MBEDTLS_PRIVATE(fin_ms) == 0 )
        return( -1 );

    elapsed_ms = mbedtls_timing_get_timer( &ctx->MBEDTLS_PRIVATE(timer), 0 );

    if( elapsed_ms >= ctx->MBEDTLS_PRIVATE(fin_ms) ){
        return( 2 );
    }

    if( elapsed_ms >= ctx->MBEDTLS_PRIVATE(int_ms) )
        return( 1 );

    return( 0 );
}

/*
 * Get the final delay.
 */
uint32_t mbedtls_timing_get_final_delay( const mbedtls_timing_delay_context *data )
{
    return( data->MBEDTLS_PRIVATE(fin_ms) );
}
