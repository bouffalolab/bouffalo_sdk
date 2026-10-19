/**
 * @file 05_pthread_barrier.c
 * @brief POSIX pthread_barrier (barrier) test cases
 *
 * Test APIs:
 * - pthread_barrier_init() / pthread_barrier_destroy()
 * - pthread_barrier_wait()
 */

#include <pthread.h>
#include <errno.h>
#include "test_common.h"

/* Test helper variables */
static pthread_barrier_t s_barrier;
static int s_barrier_counter = 0;
static pthread_mutex_t s_barrier_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread function for barrier test */
static void *barrier_thread_func(void *arg)
{
    /* Increment counter before barrier */
    pthread_mutex_lock(&s_barrier_mutex);
    s_barrier_counter++;
    pthread_mutex_unlock(&s_barrier_mutex);

    /* Wait at barrier */
    int ret = pthread_barrier_wait(&s_barrier);

    /* One thread should get PTHREAD_BARRIER_SERIAL_THREAD */
    /* Others should get 0 */
    TEST_ASSERT(ret == 0 || ret == PTHREAD_BARRIER_SERIAL_THREAD);

    return NULL;
}

/* Thread function for barrier count test */
static int s_arrival_order[4];
static int s_arrival_index = 0;

static void *barrier_order_thread_func(void *arg)
{
    int id = *(int *)arg;

    /* Record arrival order */
    pthread_mutex_lock(&s_barrier_mutex);
    s_arrival_order[s_arrival_index++] = id;
    pthread_mutex_unlock(&s_barrier_mutex);

    /* Wait at barrier */
    pthread_barrier_wait(&s_barrier);

    return NULL;
}

/**
 * T_BAR_001: Barrier initialization and destruction
 * Note: pthread_barrierattr_init/destroy not implemented, use NULL attr only
 */
static void test_barrier_init_destroy(void)
{
    pthread_barrier_t barrier;
    int ret;

    /* Initialize with default attributes */
    ret = pthread_barrier_init(&barrier, NULL, 2);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_barrier_destroy(&barrier);
    TEST_ASSERT_EQUAL(0, ret);

    /* Initialize with different count */
    ret = pthread_barrier_init(&barrier, NULL, 3);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_barrier_destroy(&barrier);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_BAR_002: Two thread barrier synchronization
 */
static void test_barrier_two_threads(void)
{
    pthread_t thread1, thread2;
    int ret;

    /* Reset counter */
    s_barrier_counter = 0;

    /* Initialize barrier for 2 threads */
    ret = pthread_barrier_init(&s_barrier, NULL, 2);
    TEST_ASSERT_EQUAL(0, ret);

    /* Create threads */
    ret = pthread_create(&thread1, NULL, barrier_thread_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_create(&thread2, NULL, barrier_thread_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Wait for threads */
    ret = pthread_join(thread1, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread2, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Counter should be 2 */
    TEST_ASSERT_EQUAL(2, s_barrier_counter);

    pthread_barrier_destroy(&s_barrier);
}

/**
 * T_BAR_003: Multi-thread barrier synchronization
 */
static void test_barrier_multi_threads(void)
{
    pthread_t threads[4];
    int ret;

    /* Reset counter */
    s_barrier_counter = 0;

    /* Initialize barrier for 4 threads */
    ret = pthread_barrier_init(&s_barrier, NULL, 4);
    TEST_ASSERT_EQUAL(0, ret);

    /* Create threads */
    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&threads[i], NULL, barrier_thread_func, NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait for threads */
    for (int i = 0; i < 4; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Counter should be 4 */
    TEST_ASSERT_EQUAL(4, s_barrier_counter);

    pthread_barrier_destroy(&s_barrier);
}

/**
 * T_BAR_004: Barrier count correctness
 */
static void test_barrier_count_correctness(void)
{
    pthread_t threads[4];
    int thread_ids[4] = {0, 1, 2, 3};
    int ret;

    /* Reset variables */
    s_arrival_index = 0;
    for (int i = 0; i < 4; i++) {
        s_arrival_order[i] = -1;
    }

    /* Initialize barrier for 4 threads */
    ret = pthread_barrier_init(&s_barrier, NULL, 4);
    TEST_ASSERT_EQUAL(0, ret);

    /* Create threads with IDs */
    for (int i = 0; i < 4; i++) {
        ret = pthread_create(&threads[i], NULL, barrier_order_thread_func, &thread_ids[i]);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait for threads */
    for (int i = 0; i < 4; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* All 4 threads should have arrived (recorded in order array) */
    int arrived_count = 0;
    for (int i = 0; i < 4; i++) {
        if (s_arrival_order[i] >= 0) {
            arrived_count++;
        }
    }
    TEST_ASSERT_EQUAL(4, arrived_count);

    pthread_barrier_destroy(&s_barrier);
}

/**
 * Test group entry function - called by main.c
 */
void test_pthread_barrier_run(void)
{
    test_barrier_init_destroy();
    test_barrier_two_threads();
    test_barrier_multi_threads();
    test_barrier_count_correctness();
}
