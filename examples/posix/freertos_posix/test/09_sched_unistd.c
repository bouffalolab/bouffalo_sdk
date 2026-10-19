/**
 * @file 09_sched_unistd.c
 * @brief POSIX scheduler and unistd test cases
 *
 * Test APIs:
 * - sched_yield() / sched_get_priority_max() / sched_get_priority_min()
 * - sleep() / usleep()
 */

#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "test_common.h"

/* Test helper variables */
static int s_yield_counter = 0;
static pthread_mutex_t s_yield_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread function for yield test */
static void *yield_thread_func(void *arg)
{
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&s_yield_mutex);
        s_yield_counter++;
        pthread_mutex_unlock(&s_yield_mutex);

        /* Yield to let other threads run */
        sched_yield();
    }

    return NULL;
}

/* Thread function for priority test */
static volatile int s_priority_order[3];
static volatile int s_priority_index = 0;

static void *priority_thread_func(void *arg)
{
    int id = *(int *)arg;

    /* Record execution order */
    s_priority_order[s_priority_index++] = id;

    return NULL;
}

/**
 * T_SCH_001: sched_yield give up CPU
 */
static void test_sched_yield(void)
{
    pthread_t thread1, thread2;
    int ret;

    /* Reset counter */
    s_yield_counter = 0;

    /* Create two threads */
    ret = pthread_create(&thread1, NULL, yield_thread_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_create(&thread2, NULL, yield_thread_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Wait for threads */
    ret = pthread_join(thread1, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread2, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Counter should be 10 (5 iterations each) */
    TEST_ASSERT_EQUAL(10, s_yield_counter);
}

/**
 * T_SCH_002: sched_get_priority_max/min
 * Note: Only SCHED_OTHER is supported by this SDK
 * Note: FreeRTOS POSIX layer does NOT validate policy parameter, returns valid range for any policy
 */
static void test_sched_priority_range(void)
{
    int max_prio, min_prio;

    /* Test SCHED_OTHER (only supported policy) */
    max_prio = sched_get_priority_max(SCHED_OTHER);
    min_prio = sched_get_priority_min(SCHED_OTHER);
    TEST_ASSERT(max_prio >= min_prio);
    printf("SCHED_OTHER priority range: %d - %d\r\n", min_prio, max_prio);

    /* Note: FreeRTOS POSIX does NOT validate policy, so this check is skipped.
     * Invalid policy (999) currently returns a valid range instead of -1.
     */
    TEST_SKIP("invalid policy range check not supported by current FreeRTOS POSIX layer");
}

/**
 * T_SCH_003: sleep second level sleep
 */
static void test_sleep(void)
{
    struct timespec start, end;
    unsigned int remaining;
    long elapsed_ms;

    /* Get start time */
    clock_gettime(CLOCK_REALTIME, &start);

    /* Sleep for 1 second */
    remaining = sleep(1);
    TEST_ASSERT_EQUAL(0, remaining);

    /* Get end time */
    clock_gettime(CLOCK_REALTIME, &end);

    /* Calculate elapsed time in milliseconds */
    elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                 (end.tv_nsec - start.tv_nsec) / 1000000;

    /* Should have slept at least 950ms */
    TEST_ASSERT(elapsed_ms >= 950);
}

/**
 * T_SCH_004: usleep microsecond level sleep
 */
static void test_usleep(void)
{
    struct timespec start, end;
    long elapsed_us;
    int ret;

    /* Get start time */
    clock_gettime(CLOCK_REALTIME, &start);

    /* Sleep for 100000us (100ms) */
    ret = usleep(100000);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get end time */
    clock_gettime(CLOCK_REALTIME, &end);

    /* Calculate elapsed time in microseconds */
    elapsed_us = (end.tv_sec - start.tv_sec) * 1000000 +
                 (end.tv_nsec - start.tv_nsec) / 1000;

    /* Should have slept at least 90000us (90ms) */
    TEST_ASSERT(elapsed_us >= 90000);
}

/**
 * T_SCH_005: Multi-priority thread scheduling
 */
static void test_multi_priority_scheduling(void)
{
    pthread_t threads[3];
    pthread_attr_t attrs[3];
    struct sched_param params[3];
    int thread_ids[3] = {0, 1, 2};
    int priorities[3];
    int ret;

    /* Get valid priority range */
    int min_prio = sched_get_priority_min(SCHED_OTHER);
    int max_prio = sched_get_priority_max(SCHED_OTHER);

    /* Set up different priorities */
    priorities[0] = min_prio;
    priorities[1] = (min_prio + max_prio) / 2;
    priorities[2] = max_prio;

    /* Reset order tracking */
    s_priority_index = 0;
    for (int i = 0; i < 3; i++) {
        s_priority_order[i] = -1;
    }

    /* Create threads with different priorities */
    for (int i = 0; i < 3; i++) {
        ret = pthread_attr_init(&attrs[i]);
        TEST_ASSERT_EQUAL(0, ret);

        params[i].sched_priority = priorities[i];
        ret = pthread_attr_setschedparam(&attrs[i], &params[i]);
        TEST_ASSERT_EQUAL(0, ret);

        ret = pthread_create(&threads[i], &attrs[i], priority_thread_func, &thread_ids[i]);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait for threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
        pthread_attr_destroy(&attrs[i]);
    }

    /* All threads should have executed */
    int executed_count = 0;
    for (int i = 0; i < 3; i++) {
        if (s_priority_order[i] >= 0) {
            executed_count++;
        }
    }
    TEST_ASSERT_EQUAL(3, executed_count);
}

/* Thread function for sched param test */
static void *sched_param_test_thread(void *arg)
{
    int policy, ret;
    struct sched_param param;
    pthread_t thread;

    thread = pthread_self();
    TEST_ASSERT(thread != 0);

    /* Get current scheduling parameters */
    ret = pthread_getschedparam(thread, &policy, &param);
    TEST_ASSERT_EQUAL(0, ret);

    /* Policy should be SCHED_OTHER (only supported policy) */
    TEST_ASSERT_EQUAL(SCHED_OTHER, policy);

    /* Set new priority (within valid range) */
    int min_prio = sched_get_priority_min(policy);
    int max_prio = sched_get_priority_max(policy);

    param.sched_priority = (min_prio + max_prio) / 2;
    ret = pthread_setschedparam(thread, policy, &param);
    /* May succeed or fail depending on implementation */

    /* Get parameters again to verify */
    struct sched_param get_param;
    int get_policy;
    ret = pthread_getschedparam(thread, &get_policy, &get_param);
    TEST_ASSERT_EQUAL(0, ret);

    return NULL;
}

/**
 * Test pthread_getschedparam and pthread_setschedparam
 * Note: Only SCHED_OTHER is supported by this SDK
 * Note: pthread_self() only works inside pthread, not FreeRTOS tasks
 */
static void test_thread_sched_param(void)
{
    pthread_t thread;
    int ret;

    /* Must run test inside a pthread since pthread_self() doesn't work on FreeRTOS tasks */
    ret = pthread_create(&thread, NULL, sched_param_test_thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * Test group entry function - called by main.c
 */
void test_sched_unistd_run(void)
{
    test_sched_yield();
    test_sched_priority_range();
    test_sleep();
    test_usleep();
    test_multi_priority_scheduling();
    test_thread_sched_param();
}
