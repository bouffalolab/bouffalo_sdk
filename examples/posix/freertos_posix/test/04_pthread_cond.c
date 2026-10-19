/**
 * @file 04_pthread_cond.c
 * @brief POSIX pthread_cond (condition variable) test cases
 *
 * Test APIs:
 * - pthread_cond_init() / pthread_cond_destroy()
 * - pthread_cond_wait() / pthread_cond_signal() / pthread_cond_broadcast()
 * - pthread_cond_timedwait()
 */

#include <pthread.h>
#include <time.h>
#include <errno.h>
#include "test_common.h"

/* Test helper variables */
static pthread_mutex_t s_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_cond_var = PTHREAD_COND_INITIALIZER;
static int s_cond_flag = 0;
static int s_cond_counter = 0;

/* Thread function for single wait test */
static void *cond_signal_thread(void *arg)
{
    struct timespec ts = {0, 50000000}; /* 50ms */

    nanosleep(&ts, NULL);

    pthread_mutex_lock(&s_cond_mutex);
    s_cond_flag = 1;
    pthread_cond_signal(&s_cond_var);
    pthread_mutex_unlock(&s_cond_mutex);

    return NULL;
}

/* Thread function for wait test */
static void *cond_wait_thread(void *arg)
{
    pthread_mutex_lock(&s_cond_mutex);
    while (s_cond_flag == 0) {
        pthread_cond_wait(&s_cond_var, &s_cond_mutex);
    }
    s_cond_counter++;
    pthread_mutex_unlock(&s_cond_mutex);

    return NULL;
}

/* Thread function for broadcast test */
static void *cond_broadcast_wait_thread(void *arg)
{
    pthread_mutex_lock(&s_cond_mutex);
    while (s_cond_flag == 0) {
        pthread_cond_wait(&s_cond_var, &s_cond_mutex);
    }
    s_cond_counter++;
    pthread_mutex_unlock(&s_cond_mutex);

    return NULL;
}

/* Thread function for producer/consumer */
#define BUFFER_SIZE 5
static int s_buffer[BUFFER_SIZE];
static int s_buffer_count = 0;
static pthread_mutex_t s_pc_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_producer_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t s_consumer_cond = PTHREAD_COND_INITIALIZER;

static void *producer_thread(void *arg)
{
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&s_pc_mutex);

        /* Wait while buffer is full */
        while (s_buffer_count >= BUFFER_SIZE) {
            pthread_cond_wait(&s_producer_cond, &s_pc_mutex);
        }

        /* Produce item */
        s_buffer[s_buffer_count++] = i;

        /* Signal consumer */
        pthread_cond_signal(&s_consumer_cond);
        pthread_mutex_unlock(&s_pc_mutex);
    }

    return NULL;
}

static void *consumer_thread(void *arg)
{
    int consumed = 0;

    while (consumed < 10) {
        pthread_mutex_lock(&s_pc_mutex);

        /* Wait while buffer is empty */
        while (s_buffer_count <= 0) {
            pthread_cond_wait(&s_consumer_cond, &s_pc_mutex);
        }

        /* Consume item */
        s_buffer_count--;
        consumed++;

        /* Signal producer */
        pthread_cond_signal(&s_producer_cond);
        pthread_mutex_unlock(&s_pc_mutex);
    }

    return NULL;
}

/**
 * T_COND_001: Condition variable initialization and destruction
 * Note: pthread_condattr_init/destroy not implemented, use NULL attr only
 */
static void test_cond_init_destroy(void)
{
    pthread_cond_t cond;
    int ret;

    /* Initialize with default attributes */
    ret = pthread_cond_init(&cond, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_cond_destroy(&cond);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_COND_002: Static initialization PTHREAD_COND_INITIALIZER
 */
static void test_cond_static_init(void)
{
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int ret;

    /* Should be able to use immediately */
    pthread_mutex_lock(&mutex);
    ret = pthread_cond_signal(&cond);
    TEST_ASSERT_EQUAL(0, ret);
    pthread_mutex_unlock(&mutex);

    ret = pthread_cond_destroy(&cond);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutex_destroy(&mutex);
}

/**
 * T_COND_003: Single thread wait/wakeup
 */
static void test_cond_single_wait(void)
{
    pthread_t thread;
    int ret;

    /* Reset test variables */
    s_cond_flag = 0;
    s_cond_counter = 0;

    /* Reinitialize condition variable and mutex */
    pthread_mutex_destroy(&s_cond_mutex);
    pthread_cond_destroy(&s_cond_var);
    s_cond_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_cond_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    /* Create waiter thread */
    ret = pthread_create(&thread, NULL, cond_wait_thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Wait a bit then signal */
    struct timespec ts = {0, 50000000}; /* 50ms */
    nanosleep(&ts, NULL);

    pthread_mutex_lock(&s_cond_mutex);
    s_cond_flag = 1;
    pthread_cond_signal(&s_cond_var);
    pthread_mutex_unlock(&s_cond_mutex);

    /* Wait for thread to complete */
    ret = pthread_join(thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    TEST_ASSERT_EQUAL(1, s_cond_counter);
}

/**
 * T_COND_004: Multiple threads signal wakeup
 */
static void test_cond_multi_signal(void)
{
    pthread_t threads[3];
    pthread_t signal_thread;
    int ret;

    /* Reset test variables */
    s_cond_flag = 0;
    s_cond_counter = 0;

    /* Reinitialize */
    pthread_mutex_destroy(&s_cond_mutex);
    pthread_cond_destroy(&s_cond_var);
    s_cond_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_cond_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    /* Create waiter threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_create(&threads[i], NULL, cond_wait_thread, NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait then signal each one */
    for (int i = 0; i < 3; i++) {
        struct timespec ts = {0, 30000000}; /* 30ms */
        nanosleep(&ts, NULL);

        pthread_mutex_lock(&s_cond_mutex);
        pthread_cond_signal(&s_cond_var);
        pthread_mutex_unlock(&s_cond_mutex);
    }

    /* Set flag to let remaining threads exit */
    pthread_mutex_lock(&s_cond_mutex);
    s_cond_flag = 1;
    pthread_cond_broadcast(&s_cond_var);
    pthread_mutex_unlock(&s_cond_mutex);

    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    TEST_ASSERT(s_cond_counter > 0);
}

/**
 * T_COND_005: Broadcast wakeup
 */
static void test_cond_broadcast(void)
{
    pthread_t threads[3];
    int ret;

    /* Reset test variables */
    s_cond_flag = 0;
    s_cond_counter = 0;

    /* Reinitialize */
    pthread_mutex_destroy(&s_cond_mutex);
    pthread_cond_destroy(&s_cond_var);
    s_cond_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_cond_var = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    /* Create waiter threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_create(&threads[i], NULL, cond_broadcast_wait_thread, NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait then broadcast */
    struct timespec ts = {0, 100000000}; /* 100ms */
    nanosleep(&ts, NULL);

    pthread_mutex_lock(&s_cond_mutex);
    s_cond_flag = 1;
    pthread_cond_broadcast(&s_cond_var);
    pthread_mutex_unlock(&s_cond_mutex);

    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* All 3 threads should have been woken up */
    TEST_ASSERT_EQUAL(3, s_cond_counter);
}

/**
 * T_COND_006: Timed wait
 */
static void test_cond_timedwait(void)
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct timespec ts;
    int ret;

    ret = pthread_cond_init(&cond, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_mutex_init(&mutex, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_mutex_lock(&mutex);

    /* Get current time */
    clock_gettime(CLOCK_REALTIME, &ts);

    /* Add 100ms timeout */
    ts.tv_nsec += 100000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    /* Wait with timeout - should timeout since no signal */
    ret = pthread_cond_timedwait(&cond, &mutex, &ts);
    TEST_ASSERT_EQUAL(ETIMEDOUT, ret);

    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

/**
 * T_COND_007: Producer/Consumer pattern
 */
static void test_cond_producer_consumer(void)
{
    pthread_t producer, consumer;
    int ret;

    /* Reset buffer */
    s_buffer_count = 0;

    /* Reinitialize synchronization objects */
    pthread_mutex_destroy(&s_pc_mutex);
    pthread_cond_destroy(&s_producer_cond);
    pthread_cond_destroy(&s_consumer_cond);
    s_pc_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    s_producer_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    s_consumer_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

    ret = pthread_create(&consumer, NULL, consumer_thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_create(&producer, NULL, producer_thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(producer, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(consumer, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Buffer should be empty after all consumed */
    TEST_ASSERT_EQUAL(0, s_buffer_count);
}

/**
 * Test group entry function - called by main.c
 */
void test_pthread_cond_run(void)
{
    test_cond_init_destroy();
    test_cond_static_init();
    test_cond_single_wait();
    test_cond_multi_signal();
    test_cond_broadcast();
    test_cond_timedwait();
    test_cond_producer_consumer();
}
