/**
 * @file 02_pthread_attr.c
 * @brief POSIX pthread_attr (thread attributes) test cases
 *
 * Test APIs:
 * - pthread_attr_init() / pthread_attr_destroy()
 * - pthread_attr_getstacksize() / pthread_attr_setstacksize()
 * - pthread_attr_getdetachstate() / pthread_attr_setdetachstate()
 * - pthread_attr_getschedparam() / pthread_attr_setschedparam()
 * - pthread_attr_setschedpolicy()
 */

#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include "test_common.h"

/* Thread function for attribute tests */
static void *attr_test_thread_func(void *arg)
{
    return (void *)1;
}

/**
 * T_ATT_001: Attribute initialization and destruction
 */
static void test_attr_init_destroy(void)
{
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_attr_destroy(&attr);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_ATT_002: Set/Get stack size
 * Note: PTHREAD_STACK_MIN=8192 is enforced, smaller values return EINVAL
 */
static void test_attr_stacksize(void)
{
    pthread_attr_t attr;
    size_t stacksize;
    size_t new_stacksize;
    int ret;

    ret = pthread_attr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get default stack size */
    ret = pthread_attr_getstacksize(&attr, &stacksize);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT(stacksize > 0);

    /* Set stack size to valid value (>= PTHREAD_STACK_MIN) */
    new_stacksize = 8192;  /* Must be >= PTHREAD_STACK_MIN */
    ret = pthread_attr_setstacksize(&attr, new_stacksize);
    TEST_ASSERT_EQUAL(0, ret);

    /* Verify new stack size */
    ret = pthread_attr_getstacksize(&attr, &stacksize);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(new_stacksize, stacksize);

    /* Setting stack size below minimum should return EINVAL */
    ret = pthread_attr_setstacksize(&attr, 1024);
    TEST_ASSERT_EQUAL(EINVAL, ret);

    pthread_attr_destroy(&attr);
}

/**
 * T_ATT_003: Set/Get detach state
 */
static void test_attr_detachstate(void)
{
    pthread_attr_t attr;
    int detachstate;
    int ret;

    ret = pthread_attr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get default detach state (should be JOINABLE) */
    ret = pthread_attr_getdetachstate(&attr, &detachstate);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PTHREAD_CREATE_JOINABLE, detachstate);

    /* Set to DETACHED state */
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    TEST_ASSERT_EQUAL(0, ret);

    /* Verify new detach state */
    ret = pthread_attr_getdetachstate(&attr, &detachstate);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PTHREAD_CREATE_DETACHED, detachstate);

    /* Set back to JOINABLE */
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    TEST_ASSERT_EQUAL(0, ret);

    ret = pthread_attr_getdetachstate(&attr, &detachstate);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PTHREAD_CREATE_JOINABLE, detachstate);

    pthread_attr_destroy(&attr);
}

/**
 * T_ATT_004: Create thread with custom attributes
 * Note: PTHREAD_STACK_MIN=8192 is enforced
 */
static void test_attr_create_thread(void)
{
    pthread_attr_t attr;
    pthread_t thread;
    size_t custom_stacksize = 8192;  /* Must be >= PTHREAD_STACK_MIN */
    int ret;
    void *retval = NULL;

    ret = pthread_attr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Set custom stack size */
    ret = pthread_attr_setstacksize(&attr, custom_stacksize);
    TEST_ASSERT_EQUAL(0, ret);

    /* Create thread with custom attributes */
    ret = pthread_create(&thread, &attr, attr_test_thread_func, NULL);
    TEST_ASSERT_EQUAL(0, ret);

    /* Wait for thread completion */
    ret = pthread_join(thread, &retval);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL((long)1, (long)retval);

    pthread_attr_destroy(&attr);
}

/**
 * T_ATT_005: Set/Get scheduling parameters
 */
static void test_attr_schedparam(void)
{
    pthread_attr_t attr;
    struct sched_param param;
    struct sched_param get_param;
    int ret;

    ret = pthread_attr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Get default scheduling parameters */
    ret = pthread_attr_getschedparam(&attr, &get_param);
    TEST_ASSERT_EQUAL(0, ret);

    /* Set new scheduling priority */
    param.sched_priority = 2;
    ret = pthread_attr_setschedparam(&attr, &param);
    TEST_ASSERT_EQUAL(0, ret);

    /* Verify new priority */
    ret = pthread_attr_getschedparam(&attr, &get_param);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(2, get_param.sched_priority);

    pthread_attr_destroy(&attr);
}

/**
 * T_ATT_006: Set scheduling policy
 * Note: Only SCHED_OTHER is supported by this SDK
 */
static void test_attr_schedpolicy(void)
{
    pthread_attr_t attr;
    int ret;

    ret = pthread_attr_init(&attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Set to SCHED_OTHER (only supported policy) */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
    TEST_ASSERT_EQUAL(0, ret);

    pthread_attr_destroy(&attr);
}

/**
 * Test group entry function - called by main.c
 */
void test_pthread_attr_run(void)
{
    test_attr_init_destroy();
    test_attr_stacksize();
    test_attr_detachstate();
    test_attr_create_thread();
    test_attr_schedparam();
    test_attr_schedpolicy();
}
