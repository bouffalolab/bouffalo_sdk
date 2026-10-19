/**
 * @file 03_pthread_mutex.c
 * @brief POSIX pthread_mutex (mutex) test cases
 *
 * Test APIs:
 * - pthread_mutex_init() / pthread_mutex_destroy()
 * - pthread_mutex_lock() / pthread_mutex_unlock() / pthread_mutex_trylock()
 * - pthread_mutex_timedlock()
 * - pthread_mutexattr_* attribute series
 */

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "test_common.h"

/* Test helper variables */
static int s_shared_counter = 0;
static pthread_mutex_t s_test_mutex;

/* Thread function for multi-threaded test */
static void *mutex_counter_thread(void *arg)
{
    int iterations = *(int *)arg;

    for (int i = 0; i < iterations; i++) {
        pthread_mutex_lock(&s_test_mutex);
        s_shared_counter++;
        pthread_mutex_unlock(&s_test_mutex);
    }

    return NULL;
}

/* Thread function for trylock test */
static void *trylock_thread(void *arg)
{
    int ret;

    ret = pthread_mutex_trylock(&s_test_mutex);
    if (ret == EBUSY) {
        return (void *)1; /* Mutex was locked, expected */
    }
    pthread_mutex_unlock(&s_test_mutex);
    return (void *)0; /* Mutex was not locked */
}

/**
 * T_MTX_001: Mutex initialization and destruction
 */
static void test_mutex_init_destroy(void)
{
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    int ret;

    /* Initialize with default attributes */
    ret = pthread_mutex_init(&mutex, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_destroy(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* Initialize with custom attributes */
    ret = pthread_mutexattr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_init(&mutex, &attr);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_destroy(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutexattr_destroy(&attr);
}

/**
 * T_MTX_002: Static initialization PTHREAD_MUTEX_INITIALIZER
 */
static void test_mutex_static_init(void)
{
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int ret;

    /* Should be able to lock/unlock immediately */
    ret = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* Destroy static initialized mutex */
    ret = pthread_mutex_destroy(&mutex);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_MTX_003: Basic lock/unlock operations
 */
static void test_mutex_lock_unlock(void)
{
    pthread_mutex_t mutex;
    int ret;

    ret = pthread_mutex_init(&mutex, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Lock */
    ret = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* Unlock */
    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* Lock again */
    ret = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* Unlock again */
    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutex_destroy(&mutex);
}

/**
 * T_MTX_004: trylock non-blocking attempt
 */
static void test_mutex_trylock(void)
{
    pthread_mutex_t mutex;
    int ret;

    ret = pthread_mutex_init(&mutex, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* trylock on unlocked mutex should succeed */
    ret = pthread_mutex_trylock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* trylock on locked mutex should return EBUSY */
    ret = pthread_mutex_trylock(&mutex);
    TEST_ASSERT_EQUAL(EBUSY, ret);

    /* Unlock */
    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* trylock should succeed again */
    ret = pthread_mutex_trylock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
}

/**
 * T_MTX_005: Multi-threaded competition protecting shared resource
 */
static void test_mutex_multi_thread(void)
{
    pthread_t threads[4];
    int iterations = 100;
    int ret;

    s_shared_counter = 0;

    ret = pthread_mutex_init(&s_test_mutex, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Create multiple threads */
    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&threads[i], NULL, mutex_counter_thread, &iterations);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait for all threads */
    for (int i = 0; i < 4; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Counter should be 4 * 100 = 400 */
    TEST_ASSERT_EQUAL(400, s_shared_counter);

    pthread_mutex_destroy(&s_test_mutex);
}

/**
 * T_MTX_006: Recursive mutex test
 */
static void test_mutex_recursive(void)
{
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    int ret;

    ret = pthread_mutexattr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_init(&mutex, &attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Recursive lock multiple times */
    ret = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_lock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    /* Unlock same number of times */
    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_unlock(&mutex);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
}

/**
 * T_MTX_007: Timed lock
 */
static void test_mutex_timedlock(void)
{
    struct timespec ts;
    int ret;

    /* Use s_test_mutex so trylock_thread can access it */
    ret = pthread_mutex_init(&s_test_mutex, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get current time */
    clock_gettime(CLOCK_REALTIME, &ts);

    /* Add 100ms timeout */
    ts.tv_nsec += 100000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* timedlock on unlocked mutex should succeed immediately */
    ret = pthread_mutex_timedlock(&s_test_mutex, &ts);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutex_unlock(&s_test_mutex);

    /* Test timeout scenario with another thread */
    pthread_mutex_lock(&s_test_mutex);

    pthread_t thread;
    ret = pthread_create(&thread, NULL, trylock_thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    void *thread_ret;
    pthread_join(thread, &thread_ret);
    /* Thread should have gotten EBUSY */
    TEST_ASSERT_EQUAL(1, (long)thread_ret);

    pthread_mutex_unlock(&s_test_mutex);
    pthread_mutex_destroy(&s_test_mutex);
}

/**
 * T_MTX_008: Mutex attribute set/get
 */
static void test_mutex_attr(void)
{
    pthread_mutexattr_t attr;
    int type;
    int ret;

    ret = pthread_mutexattr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Set and get NORMAL type */
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutexattr_gettype(&attr, &type);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PTHREAD_MUTEX_NORMAL, type);

    /* Set and get RECURSIVE type */
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutexattr_gettype(&attr, &type);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PTHREAD_MUTEX_RECURSIVE, type);

    /* Set and get ERRORCHECK type */
    ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutexattr_gettype(&attr, &type);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PTHREAD_MUTEX_ERRORCHECK, type);

    pthread_mutexattr_destroy(&attr);
}

/**
 * Test group entry function - called by main.c
 */
void test_pthread_mutex_run(void)
{
    test_mutex_init_destroy();
    test_mutex_static_init();
    test_mutex_lock_unlock();
    test_mutex_trylock();
    test_mutex_multi_thread();
    test_mutex_recursive();
    test_mutex_timedlock();
    test_mutex_attr();
}
