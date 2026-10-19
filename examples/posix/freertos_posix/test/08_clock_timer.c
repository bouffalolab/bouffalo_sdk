/**
 * @file 08_clock_timer.c
 * @brief POSIX clock and timer test cases
 *
 * Test APIs:
 * - clock_gettime() / clock_settime() / clock_getres()
 * - nanosleep() / clock_nanosleep()
 * - timer_create() / timer_delete() / timer_settime() / timer_gettime()
 *
 * Note:
 * - Most functions ignore clock_id parameter, use CLOCK_REALTIME default
 * - Timer does NOT support SIGEV_SIGNAL, only supports SIGEV_THREAD
 */

#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "test_common.h"

/* Test helper variables */
static int s_timer_callback_count = 0;
static volatile int s_timer_expired = 0;

/* Timer callback function (SIGEV_THREAD mode) */
static void timer_callback(union sigval sv)
{
    s_timer_callback_count++;
    s_timer_expired = 1;
}

/**
 * T_CLK_001: clock_gettime get time (use CLOCK_REALTIME)
 */
static void test_clock_gettime(void)
{
    struct timespec ts;
    int ret;

    /* Get CLOCK_REALTIME */
    ret = clock_gettime(CLOCK_REALTIME, &ts);
    TEST_ASSERT_EQUAL(0, ret);

    /* tv_sec should be positive (since epoch) */
    TEST_ASSERT(ts.tv_sec >= 0);

    /* tv_nsec should be in valid range [0, 999999999] */
    TEST_ASSERT(ts.tv_nsec >= 0);
    TEST_ASSERT(ts.tv_nsec < 1000000000);

    /* Get time again - should be >= previous */
    struct timespec ts2;
    ret = clock_gettime(CLOCK_REALTIME, &ts2);
    TEST_ASSERT_EQUAL(0, ret);

    /* Time should have moved forward or stayed same */
    TEST_ASSERT(ts2.tv_sec > ts.tv_sec ||
                (ts2.tv_sec == ts.tv_sec && ts2.tv_nsec >= ts.tv_nsec));
}

/**
 * T_CLK_002: clock_getres get resolution
 */
static void test_clock_getres(void)
{
    struct timespec res;
    int ret;

    /* Get CLOCK_REALTIME resolution */
    ret = clock_getres(CLOCK_REALTIME, &res);
    TEST_ASSERT_EQUAL(0, ret);

    /* Resolution should be valid */
    TEST_ASSERT(res.tv_sec >= 0);
    TEST_ASSERT(res.tv_nsec >= 0);
    TEST_ASSERT(res.tv_nsec < 1000000000);

    /* Resolution should not be zero (at least 1 nanosecond or 1 tick) */
    TEST_ASSERT(res.tv_sec > 0 || res.tv_nsec > 0);
}

/**
 * T_CLK_003: nanosleep nanosecond level sleep
 */
static void test_nanosleep(void)
{
    struct timespec req, rem;
    struct timespec start, end;
    int ret;

    /* Sleep for 100ms */
    req.tv_sec = 0;
    req.tv_nsec = 100000000; /* 100ms */

    /* Get start time */
    clock_gettime(CLOCK_REALTIME, &start);

    ret = nanosleep(&req, &rem);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get end time */
    clock_gettime(CLOCK_REALTIME, &end);

    /* Calculate elapsed time */
    long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L +
                      (end.tv_nsec - start.tv_nsec);

    /* Should have slept at least 100ms (allow some tolerance) */
    TEST_ASSERT(elapsed_ns >= 90000000); /* At least 90ms */
}

/**
 * T_CLK_004: clock_nanosleep sleep
 */
static void test_clock_nanosleep(void)
{
    struct timespec req, rem;
    struct timespec start, end;
    int ret;

    /* Sleep for 50ms */
    req.tv_sec = 0;
    req.tv_nsec = 50000000; /* 50ms */

    /* Get start time */
    clock_gettime(CLOCK_REALTIME, &start);

    ret = clock_nanosleep(CLOCK_REALTIME, 0, &req, &rem);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get end time */
    clock_gettime(CLOCK_REALTIME, &end);

    /* Calculate elapsed time */
    long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L +
                      (end.tv_nsec - start.tv_nsec);

    /* Should have slept at least 50ms (allow some tolerance) */
    TEST_ASSERT(elapsed_ns >= 45000000); /* At least 45ms */
}

/**
 * T_CLK_005: timer_create create timer (SIGEV_THREAD)
 */
static void test_timer_create(void)
{
    timer_t timerid;
    struct sigevent sev;
    int ret;

    /* Set up signal event for thread notification */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;
    sev.sigev_value.sival_ptr = NULL;

    /* Create timer */
    ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
    TEST_ASSERT_EQUAL(0, ret);

    /* Delete timer */
    ret = timer_delete(timerid);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_CLK_006: timer_settime start timer
 */
static void test_timer_settime(void)
{
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    int ret;

    /* Reset counter */
    s_timer_callback_count = 0;
    s_timer_expired = 0;

    /* Set up signal event */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;

    /* Create timer */
    ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
    TEST_ASSERT_EQUAL(0, ret);

    /* Set timer to expire after 100ms, no interval */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000; /* 100ms */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0; /* One-shot */

    ret = timer_settime(timerid, 0, &its, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Wait for timer to expire */
    struct timespec wait = {0, 200000000}; /* 200ms */
    nanosleep(&wait, NULL);

    /* Callback should have been called */
    TEST_ASSERT_EQUAL(1, s_timer_callback_count);
    TEST_ASSERT_EQUAL(1, s_timer_expired);

    /* Delete timer */
    ret = timer_delete(timerid);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_CLK_007: timer_delete delete timer
 */
static void test_timer_delete(void)
{
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    int ret;

    /* Set up signal event */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;

    /* Create timer */
    ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
    TEST_ASSERT_EQUAL(0, ret);

    /* Arm timer */
    its.it_value.tv_sec = 10; /* 10 seconds */
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    ret = timer_settime(timerid, 0, &its, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Delete timer immediately (before it expires) */
    ret = timer_delete(timerid);
    TEST_ASSERT_EQUAL(0, ret);

    /* Note: FreeRTOS POSIX layer does NOT detect double-delete.
     * Calling timer_delete() on an already-deleted timer causes double-free.
     * Skip this test to avoid crash.
     * g_tests_run++; // Count as skipped
     */
}

/**
 * T_CLK_008: Timer callback function (SIGEV_THREAD mode)
 */
static void test_timer_callback(void)
{
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    int ret;

    /* Reset counter */
    s_timer_callback_count = 0;

    /* Set up signal event with callback */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;
    sev.sigev_value.sival_int = 42;

    /* Create timer */
    ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
    TEST_ASSERT_EQUAL(0, ret);

    /* Set periodic timer: first expire after 50ms, then every 50ms */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 50000000; /* 50ms */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 50000000; /* 50ms interval */

    ret = timer_settime(timerid, 0, &its, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Wait for multiple callbacks */
    struct timespec wait = {0, 180000000}; /* 180ms */
    nanosleep(&wait, NULL);

    /* Should have at least 2 callbacks */
    TEST_ASSERT(s_timer_callback_count >= 2);

    /* Delete timer */
    ret = timer_delete(timerid);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * Test timer_gettime
 */
static void test_timer_gettime(void)
{
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its, get_its;
    int ret;

    /* Set up signal event */
    memset(&sev, 0, sizeof(sev));
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_callback;

    /* Create timer */
    ret = timer_create(CLOCK_REALTIME, &sev, &timerid);
    TEST_ASSERT_EQUAL(0, ret);

    /* Disarmed timer should have zero value */
    ret = timer_gettime(timerid, &get_its);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(0, get_its.it_value.tv_sec);
    TEST_ASSERT_EQUAL(0, get_its.it_value.tv_nsec);

    /* Arm timer */
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 500000000; /* 1.5 seconds */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    ret = timer_settime(timerid, 0, &its, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get remaining time */
    ret = timer_gettime(timerid, &get_its);
    TEST_ASSERT_EQUAL(0, ret);

    /* Remaining time should be close to 1.5 seconds */
    TEST_ASSERT(get_its.it_value.tv_sec == 1 || get_its.it_value.tv_sec == 0);
    TEST_ASSERT(get_its.it_value.tv_nsec > 0 || get_its.it_value.tv_sec > 0);

    /* Delete timer */
    ret = timer_delete(timerid);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * Test group entry function - called by main.c
 */
void test_clock_timer_run(void)
{
    test_clock_gettime();
    test_clock_getres();
    test_nanosleep();
    test_clock_nanosleep();
    test_timer_create();
    test_timer_settime();
    test_timer_delete();
    test_timer_callback();
    test_timer_gettime();
}
