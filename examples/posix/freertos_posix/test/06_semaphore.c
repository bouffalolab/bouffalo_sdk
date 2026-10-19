/**
 * @file 06_semaphore.c
 * @brief POSIX semaphore test cases
 *
 * Test APIs:
 * - sem_init() / sem_destroy()
 * - sem_wait() / sem_trywait() / sem_timedwait()
 * - sem_post() / sem_getvalue()
 *
 * Note: pshared parameter is ignored, all semaphores only support thread sharing.
 * Test with pshared=0 only.
 */

#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "test_common.h"

/* Test helper variables */
static sem_t s_test_sem;
static int s_sem_counter = 0;
static pthread_mutex_t s_counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread function for semaphore wait */
static void *sem_wait_thread(void *arg)
{
    sem_wait(&s_test_sem);

    pthread_mutex_lock(&s_counter_mutex);
    s_sem_counter++;
    pthread_mutex_unlock(&s_counter_mutex);

    return NULL;
}

/* Thread function for semaphore post */
static void *sem_post_thread(void *arg)
{
    struct timespec ts = {0, 50000000}; /* 50ms */
    nanosleep(&ts, NULL);

    sem_post(&s_test_sem);

    return NULL;
}

/**
 * T_SEM_001: Semaphore initialization and destruction (pshared=0)
 */
static void test_sem_init_destroy(void)
{
    sem_t sem;
    int ret;

    /* Initialize with pshared=0 (thread sharing only) */
    ret = sem_init(&sem, 0, 0);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_destroy(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    /* Initialize with initial value 1 */
    ret = sem_init(&sem, 0, 1);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_destroy(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    /* Initialize with initial value 5 */
    ret = sem_init(&sem, 0, 5);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_destroy(&sem);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_SEM_002: Binary semaphore basic operations
 */
static void test_sem_binary(void)
{
    sem_t sem;
    int ret;
    int value;

    /* Initialize binary semaphore with value 0 */
    ret = sem_init(&sem, 0, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Value should be 0 */
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(0, value);

    /* Post (V operation) */
    ret = sem_post(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    /* Value should be 1 */
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, value);

    /* Wait (P operation) */
    ret = sem_wait(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    /* Value should be 0 */
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(0, value);

    /* Post again */
    ret = sem_post(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    sem_destroy(&sem);
}

/**
 * T_SEM_003: Counting semaphore test
 */
static void test_sem_counting(void)
{
    sem_t sem;
    int ret;
    int value;

    /* Initialize counting semaphore with value 3 */
    ret = sem_init(&sem, 0, 3);
    TEST_ASSERT_EQUAL(0, ret);

    /* Value should be 3 */
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(3, value);

    /* Wait once */
    ret = sem_wait(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(2, value);

    /* Wait again */
    ret = sem_wait(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, value);

    /* Post twice */
    ret = sem_post(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_post(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(3, value);

    sem_destroy(&sem);
}

/**
 * T_SEM_004: trywait non-blocking attempt
 */
static void test_sem_trywait(void)
{
    sem_t sem;
    int ret;

    /* Initialize with value 1 */
    ret = sem_init(&sem, 0, 1);
    TEST_ASSERT_EQUAL(0, ret);

    /* trywait should succeed */
    ret = sem_trywait(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    /* trywait should fail with EAGAIN (returns -1, sets errno) */
    ret = sem_trywait(&sem);
    TEST_ASSERT_EQUAL(-1, ret);
    TEST_ASSERT_EQUAL(EAGAIN, errno);

    /* Post and trywait again */
    ret = sem_post(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    ret = sem_trywait(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    sem_destroy(&sem);
}

/**
 * T_SEM_005: Timed wait
 */
static void test_sem_timedwait(void)
{
    sem_t sem;
    struct timespec ts;
    int ret;

    /* Initialize with value 0 */
    ret = sem_init(&sem, 0, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get current time */
    clock_gettime(CLOCK_REALTIME, &ts);

    /* Add 100ms timeout */
    ts.tv_nsec += 100000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* timedwait should timeout (returns -1, sets errno to ETIMEDOUT) */
    ret = sem_timedwait(&sem, &ts);
    TEST_ASSERT_EQUAL(-1, ret);
    TEST_ASSERT_EQUAL(ETIMEDOUT, errno);

    /* Post and try timedwait again */
    ret = sem_post(&sem);
    TEST_ASSERT_EQUAL(0, ret);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 100000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* timedwait should succeed immediately */
    ret = sem_timedwait(&sem, &ts);
    TEST_ASSERT_EQUAL(0, ret);

    sem_destroy(&sem);
}

/**
 * T_SEM_006: sem_getvalue get value
 */
static void test_sem_getvalue(void)
{
    sem_t sem;
    int ret;
    int value;

    /* Initialize with value 5 */
    ret = sem_init(&sem, 0, 5);
    TEST_ASSERT_EQUAL(0, ret);

    /* Check initial value */
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(5, value);

    /* Post and check */
    sem_post(&sem);
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(6, value);

    /* Wait and check */
    sem_wait(&sem);
    sem_wait(&sem);
    ret = sem_getvalue(&sem, &value);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(4, value);

    sem_destroy(&sem);
}

/**
 * T_SEM_007: Multi-thread semaphore synchronization
 */
static void test_sem_multi_thread(void)
{
    pthread_t waiter_threads[3];
    pthread_t poster_thread;
    int ret;

    /* Reset counter */
    s_sem_counter = 0;

    /* Initialize semaphore with value 0 */
    ret = sem_init(&s_test_sem, 0, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Create waiter threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_create(&waiter_threads[i], NULL, sem_wait_thread, NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait a bit for threads to block on semaphore */
    struct timespec ts = {0, 100000000}; /* 100ms */
    nanosleep(&ts, NULL);

    /* Post 3 times to release all waiters */
    for (int i = 0; i < 3; i++) {
        sem_post(&s_test_sem);
    }

    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_join(waiter_threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Counter should be 3 */
    TEST_ASSERT_EQUAL(3, s_sem_counter);

    sem_destroy(&s_test_sem);
}

/**
 * Test group entry function - called by main.c
 */
void test_semaphore_run(void)
{
    test_sem_init_destroy();
    test_sem_binary();
    test_sem_counting();
    test_sem_trywait();
    test_sem_timedwait();
    test_sem_getvalue();
    test_sem_multi_thread();
}
