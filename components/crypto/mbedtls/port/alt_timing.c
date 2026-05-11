#include <sys/time.h>
#include "bflb_mtimer.h"
#include "mbedtls/timing.h"
#include "mbedtls/platform.h"
#include <stdio.h>

#ifdef CONFIG_MBEDTLS_V2
#define MBEDTLS_PRIVATE(member) member
#endif

struct _hr_time
{
    uint64_t start_us;
};

volatile int mbedtls_timing_alarmed = 0;
static uint64_t timing_alarm_deadline_us = 0;

static void mbedtls_timing_update_alarm( void )
{
    if( timing_alarm_deadline_us != 0 &&
        bflb_mtimer_get_time_us() >= timing_alarm_deadline_us )
    {
        mbedtls_timing_alarmed = 1;
        timing_alarm_deadline_us = 0;
    }
}

#ifndef BL618DG
#ifndef CONFIG_NEWLIB
#include <bflb_rtc.h>
static struct bflb_device_s *rtc = NULL;
static uint32_t timestamp_base = 0;
static struct timezone tz = {
    .tz_dsttime = 0,
    .tz_minuteswest = 0
};

int _gettimeofday_r(void *nop, struct timeval *tp, void *tzvp)
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
#endif

unsigned long mbedtls_timing_get_timer( struct mbedtls_timing_hr_time *val, int reset )
{
    struct _hr_time *t = (struct _hr_time *) val;
    uint64_t now_us;

    mbedtls_timing_update_alarm();
    now_us = bflb_mtimer_get_time_us();

    if( reset )
    {
        t->start_us = now_us;
        return( 0 );
    }
    else
    {
        return( (unsigned long) ( ( now_us - t->start_us ) / 1000ULL ) );
    }
}

unsigned long mbedtls_timing_hardclock( void )
{
    mbedtls_timing_update_alarm();
    return( (unsigned long) bflb_mtimer_get_time_us() );
}

void mbedtls_set_alarm( int seconds )
{
    if( seconds <= 0 )
    {
        mbedtls_timing_alarmed = 1;
        timing_alarm_deadline_us = 0;
        return;
    }

    mbedtls_timing_alarmed = 0;
    timing_alarm_deadline_us = bflb_mtimer_get_time_us() +
                               ( (uint64_t) seconds * 1000000ULL );
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

    mbedtls_timing_update_alarm();

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

#if defined(CONFIG_MBEDTLS_V2) && defined(MBEDTLS_SELF_TEST)
/*
 * Busy-waits for the given number of milliseconds.
 * Used for testing mbedtls_timing_hardclock.
 */
static void busy_msleep( unsigned long msec )
{
    struct mbedtls_timing_hr_time hires;
    unsigned long i = 0;
    volatile unsigned long j;

    (void) mbedtls_timing_get_timer( &hires, 1 );

    while( mbedtls_timing_get_timer( &hires, 0 ) < msec )
        i++;

    j = i;
    (void) j;
}

#define TIMING_SELFTEST_FAIL                                        \
    do                                                              \
    {                                                               \
        if( verbose != 0 )                                          \
            mbedtls_printf( "failed at line %d\n", __LINE__ );      \
        return( 1 );                                                \
    } while( 0 )

int mbedtls_timing_self_test( int verbose )
{
    unsigned long cycles = 0, ratio = 0;
    unsigned long millisecs = 0, secs = 0;
    int hardfail = 0;
    struct mbedtls_timing_hr_time hires;
    uint32_t a = 0, b = 0;
    mbedtls_timing_delay_context ctx;

    if( verbose != 0 )
        mbedtls_printf( "  TIMING tests note: will take some time!\n" );

    if( verbose != 0 )
        mbedtls_printf( "  TIMING test #1 (set_alarm / get_timer): " );

    secs = 1;
    (void) mbedtls_timing_get_timer( &hires, 1 );
    mbedtls_set_alarm( (int) secs );
    while( !mbedtls_timing_alarmed )
        mbedtls_timing_update_alarm();

    millisecs = mbedtls_timing_get_timer( &hires, 0 );
    if( millisecs < 800 * secs || millisecs > 1200 * secs + 300 )
        TIMING_SELFTEST_FAIL;

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );

    if( verbose != 0 )
        mbedtls_printf( "  TIMING test #2 (set/get_delay        ): " );

    a = 800;
    b = 400;
    mbedtls_timing_set_delay( &ctx, a, a + b );

    busy_msleep( a - a / 4 );
    if( mbedtls_timing_get_delay( &ctx ) != 0 )
        TIMING_SELFTEST_FAIL;

    busy_msleep( a / 4 + b / 4 );
    if( mbedtls_timing_get_delay( &ctx ) != 1 )
        TIMING_SELFTEST_FAIL;

    busy_msleep( b );
    if( mbedtls_timing_get_delay( &ctx ) != 2 )
        TIMING_SELFTEST_FAIL;

    mbedtls_timing_set_delay( &ctx, 0, 0 );
    busy_msleep( 200 );
    if( mbedtls_timing_get_delay( &ctx ) != -1 )
        TIMING_SELFTEST_FAIL;

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );

    if( verbose != 0 )
        mbedtls_printf( "  TIMING test #3 (hardclock / get_timer): " );

hard_test:
    if( hardfail > 1 )
    {
        if( verbose != 0 )
            mbedtls_printf( "failed (ignored)\n" );
        goto hard_test_done;
    }

    millisecs = 1;
    cycles = mbedtls_timing_hardclock();
    busy_msleep( millisecs );
    cycles = mbedtls_timing_hardclock() - cycles;
    ratio = cycles / millisecs;

    for( millisecs = 2; millisecs <= 4; millisecs++ )
    {
        cycles = mbedtls_timing_hardclock();
        busy_msleep( millisecs );
        cycles = mbedtls_timing_hardclock() - cycles;

        if( cycles / millisecs < ratio - ratio / 5 ||
            cycles / millisecs > ratio + ratio / 5 )
        {
            hardfail++;
            goto hard_test;
        }
    }

    if( verbose != 0 )
        mbedtls_printf( "passed\n" );

hard_test_done:
    if( verbose != 0 )
        mbedtls_printf( "\n" );

    return( 0 );
}
#endif
