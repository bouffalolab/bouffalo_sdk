/**
 * @file 07_mqueue.c
 * @brief POSIX mqueue (message queue) test cases
 *
 * Test APIs:
 * - mq_open() / mq_close() / mq_unlink()
 * - mq_send() / mq_receive()
 * - mq_timedsend() / mq_timedreceive()
 * - mq_getattr()
 *
 * Note: Message priority is NOT supported, msg_prio parameter is ignored.
 */

#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include "test_common.h"

#define TEST_QUEUE_NAME "/test_mq"
#define MAX_MSG_SIZE 128
#define MAX_MSGS 10

/* Test helper variables */
static mqd_t s_test_mq;
static pthread_mutex_t s_mq_mutex = PTHREAD_MUTEX_INITIALIZER;
static int s_received_count = 0;

/* Thread function for receiver */
static void *mq_receiver_thread(void *arg)
{
    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_received;
    unsigned int prio;

    bytes_received = mq_receive(s_test_mq, buffer, MAX_MSG_SIZE, &prio);
    if (bytes_received > 0) {
        pthread_mutex_lock(&s_mq_mutex);
        s_received_count++;
        pthread_mutex_unlock(&s_mq_mutex);
    }

    return NULL;
}

/**
 * T_MQ_001: Message queue create and close
 */
static void test_mq_open_close(void)
{
    mqd_t mq;
    struct mq_attr attr;
    int ret;

    /* Set queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    /* Close queue */
    ret = mq_close(mq);
    TEST_ASSERT_EQUAL(0, ret);

    /* Unlink queue */
    ret = mq_unlink(TEST_QUEUE_NAME);
    TEST_ASSERT_EQUAL(0, ret);
}

/**
 * T_MQ_002: Basic send/receive (no priority test)
 */
static void test_mq_send_receive(void)
{
    mqd_t mq;
    struct mq_attr attr;
    char send_buf[] = "Hello, Message Queue!";
    char recv_buf[MAX_MSG_SIZE];
    ssize_t bytes_received;
    unsigned int prio;
    int ret;

    /* Set queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    /* Send message (priority ignored, use 0) */
    ret = mq_send(mq, send_buf, strlen(send_buf) + 1, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Receive message */
    bytes_received = mq_receive(mq, recv_buf, MAX_MSG_SIZE, &prio);
    TEST_ASSERT(bytes_received > 0);
    TEST_ASSERT_EQUAL_STRING(send_buf, recv_buf);

    /* Close and unlink */
    mq_close(mq);
    mq_unlink(TEST_QUEUE_NAME);
}

/**
 * T_MQ_003: Non-blocking mode O_NONBLOCK
 */
static void test_mq_nonblock(void)
{
    mqd_t mq;
    struct mq_attr attr;
    char buf[MAX_MSG_SIZE];
    ssize_t bytes_received;
    int ret;

    /* Set queue attributes with non-blocking flag */
    attr.mq_flags = O_NONBLOCK;
    attr.mq_maxmsg = 1; /* Only 1 message */
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    /* Receive on empty queue should fail with EAGAIN */
    bytes_received = mq_receive(mq, buf, MAX_MSG_SIZE, NULL);
    TEST_ASSERT_EQUAL(-1, bytes_received);
    TEST_ASSERT_EQUAL(EAGAIN, errno);

    /* Close and unlink */
    mq_close(mq);
    mq_unlink(TEST_QUEUE_NAME);
}

/**
 * T_MQ_004: Timed send/receive
 */
static void test_mq_timed(void)
{
    mqd_t mq;
    struct mq_attr attr;
    char send_buf[] = "Timed Message";
    char recv_buf[MAX_MSG_SIZE];
    struct timespec ts;
    ssize_t bytes_received;
    int ret;

    /* Set queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    /* Send message */
    ret = mq_send(mq, send_buf, strlen(send_buf) + 1, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Timed receive */
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1; /* 1 second timeout */

    bytes_received = mq_timedreceive(mq, recv_buf, MAX_MSG_SIZE, NULL, &ts);
    TEST_ASSERT(bytes_received > 0);
    TEST_ASSERT_EQUAL_STRING(send_buf, recv_buf);

    /* Timed receive on empty queue should timeout */
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 100000000; /* 100ms */
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    bytes_received = mq_timedreceive(mq, recv_buf, MAX_MSG_SIZE, NULL, &ts);
    TEST_ASSERT_EQUAL(-1, bytes_received);
    TEST_ASSERT_EQUAL(ETIMEDOUT, errno);

    /* Close and unlink */
    mq_close(mq);
    mq_unlink(TEST_QUEUE_NAME);
}

/**
 * T_MQ_005: Get queue attributes
 */
static void test_mq_getattr(void)
{
    mqd_t mq;
    struct mq_attr attr;
    struct mq_attr get_attr;
    int ret;

    /* Set queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 5;
    attr.mq_msgsize = 64;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    /* Get attributes */
    ret = mq_getattr(mq, &get_attr);
    TEST_ASSERT_EQUAL(0, ret);

    /* Verify max message settings */
    TEST_ASSERT_EQUAL(5, get_attr.mq_maxmsg);
    TEST_ASSERT_EQUAL(64, get_attr.mq_msgsize);

    /* Close and unlink */
    mq_close(mq);
    mq_unlink(TEST_QUEUE_NAME);
}

/**
 * T_MQ_006: Multi producer/consumer
 */
static void test_mq_multi_producer_consumer(void)
{
    mqd_t mq;
    struct mq_attr attr;
    pthread_t producer, consumer;
    int ret;

    /* Reset counter */
    s_received_count = 0;

    /* Set queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MSGS;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    s_test_mq = mq;

    /* Send multiple messages */
    for (int i = 0; i < 3; i++) {
        char msg[32];
        snprintf(msg, sizeof(msg), "Message %d", i);
        ret = mq_send(mq, msg, strlen(msg) + 1, 0);
        TEST_ASSERT_EQUAL(0, ret);
    }

    /* Create consumer threads */
    for (int i = 0; i < 3; i++) {
        ret = pthread_create(&consumer, NULL, mq_receiver_thread, NULL);
        TEST_ASSERT_EQUAL(0, ret);
        pthread_join(consumer, NULL);
    }

    /* All 3 messages should be received */
    TEST_ASSERT_EQUAL(3, s_received_count);

    /* Close and unlink */
    mq_close(mq);
    mq_unlink(TEST_QUEUE_NAME);
}

/**
 * T_MQ_007: Queue full/empty handling
 */
static void test_mq_full_empty(void)
{
    mqd_t mq;
    struct mq_attr attr;
    char msg[] = "X";
    int ret;

    /* Set queue attributes with small capacity */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 2; /* Only 2 messages */
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    /* Create queue */
    mq = mq_open(TEST_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    TEST_ASSERT(mq != (mqd_t)-1);

    /* Fill queue */
    ret = mq_send(mq, msg, 2, 0);
    TEST_ASSERT_EQUAL(0, ret);

    ret = mq_send(mq, msg, 2, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Try to send to full queue (blocking mode will wait, so we use timed version) */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 50000000; /* 50ms */
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    ret = mq_timedsend(mq, msg, 2, 0, &ts);
    /* Should timeout because queue is full */
    TEST_ASSERT_EQUAL(ETIMEDOUT, errno);

    /* Receive one message */
    char buf[MAX_MSG_SIZE];
    ssize_t bytes = mq_receive(mq, buf, MAX_MSG_SIZE, NULL);
    TEST_ASSERT(bytes > 0);

    /* Now sending should succeed */
    ret = mq_send(mq, msg, 2, 0);
    TEST_ASSERT_EQUAL(0, ret);

    /* Close and unlink */
    mq_close(mq);
    mq_unlink(TEST_QUEUE_NAME);
}

/**
 * Test group entry function - called by main.c
 */
void test_mqueue_run(void)
{
    test_mq_open_close();
    test_mq_send_receive();
    test_mq_nonblock();
    test_mq_timed();
    test_mq_getattr();
    test_mq_multi_producer_consumer();
    test_mq_full_empty();
}
