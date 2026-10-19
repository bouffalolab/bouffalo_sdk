/**
 * @file 01_pthread.c
 * @brief POSIX pthread (thread management) test cases
 *
 * Test APIs:
 * - pthread_create() / pthread_exit() / pthread_join() / pthread_detach()
 * - pthread_self() / pthread_equal()
 *
 * Note: pthread_cancel is NOT implemented (returns ENOSYS), so NOT tested.
 */

#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include "test_common.h"

/* Test helper variables */
static int s_thread_executed = 0;
static int s_thread_arg_received = 0;
static void *s_thread_return_value = NULL;

/* T_PTH_001: Simple thread function */
static void *thread_simple_func(void *arg)
{
    s_thread_executed = 1;
    return NULL;
}

/* T_PTH_003: Thread function with return value */
static void *thread_return_func(void *arg)
{
    return (void *)12345;
}

/* T_PTH_008: Thread function with argument */
static void *thread_arg_func(void *arg)
{
    int *value = (int *)arg;
    if (value != NULL) {
        s_thread_arg_received = *value;
    }
    return NULL;
}

/* T_PTH_007: Thread function with pthread_exit */
static void *thread_exit_func(void *arg)
{
    s_thread_executed = 1;
    pthread_exit((void *)999);
    return NULL; /* Should not reach here */
}

/* T_PTH_002: Counter thread for multiple threads test */
static void *thread_counter_func(void *arg)
{
    int *counter = (int *)arg;
    (*counter)++;
    return NULL;
}

/* T_PTH_004: Detached thread function */
static void *thread_detach_func(void *arg)
{
    s_thread_executed = 1;
    return NULL;
}

/**
 * T_PTH_001: Create single thread and wait for completion
 */
static void test_pthread_create_join(void)
{
    pthread_t thread;
    int ret;
    void *retval = NULL;

    s_thread_executed = 0;

    ret = pthread_create(&thread, NULL, thread_simple_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread, &retval);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, s_thread_executed);
    TEST_ASSERT(retval == NULL);
}

/**
 * T_PTH_002: Create multiple threads and execute concurrently
 */
static void test_pthread_multiple_threads(void)
{
    pthread_t threads[3];
    int ret;
    int counter = 0;

    for (int i = 0; i < 3; i++) {
        ret = pthread_create(&threads[i], NULL, thread_counter_func, &counter);
        TEST_ASSERT_EQUAL(0, ret);
    }

    for (int i = 0; i < 3; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Counter should be 3 after all threads complete */
    TEST_ASSERT_EQUAL(3, counter);
}

/**
 * T_PTH_003: Thread return value passing
 */
static void test_pthread_return_value(void)
{
    pthread_t thread;
    int ret;
    void *retval = NULL;

    ret = pthread_create(&thread, NULL, thread_return_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread, &retval);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL((long)12345, (long)retval);
}

/**
 * T_PTH_004: pthread_detach detached thread
 */
static void test_pthread_detach(void)
{
    pthread_t thread;
    int ret;

    s_thread_executed = 0;

    ret = pthread_create(&thread, NULL, thread_detach_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_detach(thread);
    TEST_ASSERT_EQUAL(0, ret);

    /* Give detached thread time to execute */
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000; /* 100ms */
    nanosleep(&ts, NULL);

    TEST_ASSERT_EQUAL(1, s_thread_executed);
}

/* Helper thread for pthread_self test */
static pthread_t s_recorded_thread_id;

static void *thread_self_record_func(void *arg)
{
    s_recorded_thread_id = pthread_self();
    s_thread_executed = 1;
    return NULL;
}

/**
 * T_PTH_005: pthread_self get current thread ID
 * Note: pthread_self() returns 0 for FreeRTOS tasks not created via pthread_create.
 * We test pthread_self from within a pthread thread instead.
 */
static void test_pthread_self(void)
{
    pthread_t thread;
    int ret;

    s_thread_executed = 0;

    /* pthread_self from a pthread thread should return valid ID */
    ret = pthread_create(&thread, NULL, thread_self_record_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, s_thread_executed);

    /* Recorded thread ID should be non-zero */
    TEST_ASSERT(s_recorded_thread_id != 0);
}

/**
 * T_PTH_006: pthread_equal compare thread IDs
 * Note: We test pthread_equal by comparing IDs while threads are still alive.
 * Thread IDs may be reused after a thread terminates, so we must compare
 * while both threads exist.
 */
static void test_pthread_equal(void)
{
    pthread_t thread1, thread2;
    int result;
    int ret;

    /* Create two threads WITHOUT joining them first */
    ret = pthread_create(&thread1, NULL, thread_simple_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_create(&thread2, NULL, thread_simple_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Compare thread IDs while both threads are alive */
    /* Same thread ID should be equal */
    result = pthread_equal(thread1, thread1);
    TEST_ASSERT(result != 0);

    /* Different thread IDs should not be equal */
    result = pthread_equal(thread1, thread2);
    TEST_ASSERT_EQUAL(0, result);

    /* Now join both threads */
    ret = pthread_join(thread1, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread2, NULL);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_PTH_007: pthread_exit normal exit
 */
static void test_pthread_exit(void)
{
    pthread_t thread;
    int ret;
    void *retval = NULL;

    s_thread_executed = 0;

    ret = pthread_create(&thread, NULL, thread_exit_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread, &retval);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, s_thread_executed);
    TEST_ASSERT_EQUAL((long)999, (long)retval);
}

/**
 * T_PTH_008: Thread argument passing
 */
static void test_pthread_arg_passing(void)
{
    pthread_t thread;
    int ret;
    int arg_value = 42;

    s_thread_arg_received = 0;

    ret = pthread_create(&thread, NULL, thread_arg_func, &arg_value);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_join(thread, NULL);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(42, s_thread_arg_received);
}

/**
 * T_PTH_009: Invalid parameter error handling
 * SKIPPED: FreeRTOS POSIX implementation does not handle NULL pointers gracefully.
 * Passing NULL to pthread_create causes Store/AMO access fault.
 * This is a known limitation of the FreeRTOS+POSIX layer.
 */
static void test_pthread_invalid_params(void)
{
    TEST_SKIP("NULL pointer handling not supported (causes crash)");
}

/* errno thread-safe test helper */
static int s_errno_test_passed[3];

static void *errno_thread_func(void *arg)
{
    int id = *(int *)arg;
    int expected_errno = (id + 1) * 10;  /* Each thread sets different errno: 10, 20, 30 */

    /* Set this thread's errno */
    errno = expected_errno;

    /* Yield to let other threads run (they will set different errno values) */
    sched_yield();
    sched_yield();
    sched_yield();

    /* Check if errno is still this thread's value (not overwritten by others) */
    s_errno_test_passed[id] = (errno == expected_errno) ? 1 : 0;

    if (errno != expected_errno) {
        printf("[ERRNO] Thread %d: expected=%d, actual=%d\r\n",
               id, expected_errno, errno);
    }

    return NULL;
}

/**
 * T_PTH_010: errno is thread-safe
 * Verify that each thread has its own errno value.
 * FreeRTOS implements this via __errno() returning &FreeRTOS_errno,
 * which is saved/restored on context switch.
 */
static void test_errno_thread_safe(void)
{
    pthread_t threads[3];
    int thread_ids[3] = {0, 1, 2};
    int ret;

    /* Reset */
    for (int i = 0; i < 3; i++) {
        s_errno_test_passed[i] = 0;
    }

    /* Create threads that each set different errno values */
    for (int i = 0; i < 3; i++) {
        ret = pthread_create(&threads[i], NULL, errno_thread_func, &thread_ids[i]);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Wait for all threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_join(threads[i], NULL);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* All threads should have seen their own errno value */
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL(1, s_errno_test_passed[i]);
    }
}

/**
 * Test group entry function - called by main.c
 */
void test_pthread_run(void)
{
    test_pthread_create_join();
    test_pthread_multiple_threads();
    test_pthread_return_value();
    test_pthread_detach();
    test_pthread_self();
    test_pthread_equal();
    test_pthread_exit();
    test_pthread_arg_passing();
    test_pthread_invalid_params();
    test_errno_thread_safe();
}
