
/*
 * Copyright (c) 2002, Intel Corporation. All rights reserved.
 * Created by:  crystal.xiong REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this
 * source tree.
 *
 * 1. Two threads sending/receiving on different message queue.
 * 2. Set different Priority to the messages in the message queue, to
 * see whether the highest priority is received first.
 */

// #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fcntl.h"
#include "pthread.h"
#include "mqueue.h"

#define MQ_NAME_1       "/testmsg1"
#define MQ_NAME_2       "/testmsg2"
#define MSG_SIZE	128
#define MAX_MSG		3

extern int FreeRTOS_errno;
static const char *s_msg_ptr[] = { "msg test 1", "msg test 2", "msg test 3" };

static char r_msg_ptr_1[MAX_MSG][MSG_SIZE];
static char r_msg_ptr_2[MAX_MSG][MSG_SIZE];
static pthread_t send1, send2, rev1, rev2;

static void send_1(void *mq)
{
	int i;
	mqd_t mq1 = *(mqd_t *) mq;

	printf("Enter into send_1 \r\n");
	for (i = 0; i < MAX_MSG; i++) {
		if (-1 == mq_send(mq1, s_msg_ptr[i], MSG_SIZE, i)) {
			printf("mq_send doesn't return success \r\n");
			pthread_exit((void *)1);
		}
		printf("[%d] send '%s' in thread send_1. \r\n", i + 1,
		       s_msg_ptr[i]);
	}
	pthread_exit(NULL);

}

static void send_2(void *mq)
{
	int i;
	mqd_t mq2 = *(mqd_t *) mq;

	printf("Enter into send_2 \r\n");
	for (i = 0; i < MAX_MSG; i++) {
		if (-1 == mq_send(mq2, s_msg_ptr[i], MSG_SIZE, i)) {
			printf("mq_send doesn't return success \r\n");
			pthread_exit((void *)1);
		}
		printf("[%d] send '%s' in thread send_2. \r\n", i + 1,
		       s_msg_ptr[i]);
	}
	pthread_exit(NULL);
}

static void receive_1(void *mq)
{
	int i;
	mqd_t mq1 = *(mqd_t *) mq;

	printf("Enter into receive_1 \r\n");
	for (i = 0; i < MAX_MSG; i++) {
retry:
		if (-1 == mq_receive(mq1, r_msg_ptr_1[i], MSG_SIZE, NULL)) {
	                printf("errno: %d \r\n", FreeRTOS_errno);
			if (FreeRTOS_errno == 11)
				goto retry;
			printf("mq_receive doesn't return success \r\n");
			pthread_exit((void *)1);
		}
		printf("[%d] receive '%s' in thread receive_1. \r\n", i + 1,
		       r_msg_ptr_1[i]);
	}
	pthread_exit(NULL);
}

static void receive_2(void *mq)
{
	int i;
	mqd_t mq2 = *(mqd_t *) mq;

	printf("Enter into receive_2 \r\n");
	for (i = 0; i < MAX_MSG; i++) {
retry:
		if (-1 == mq_receive(mq2, r_msg_ptr_2[i], MSG_SIZE, NULL)) {
			printf("errno: %d \r\n", FreeRTOS_errno);
                        if (FreeRTOS_errno == 11)
                                goto retry;
			printf("mq_receive doesn't return success \r\n");
			pthread_exit((void *)1);
		}
		printf("[%d] receive '%s' in thread receive_2. \r\n", i + 1,
		       r_msg_ptr_2[i]);
	}
	pthread_exit(NULL);
}

int mqueues_test(void)
{

	mqd_t mq1 = 0, mq2 = 0;
	struct mq_attr mqstat;
	int oflag = O_CREAT | O_NONBLOCK | O_RDWR;

	memset(&mqstat, 0, sizeof(mqstat));
	mqstat.mq_maxmsg = MAX_MSG;
	mqstat.mq_msgsize = MSG_SIZE;
	mqstat.mq_flags = 0;

	if ((mq1 = mq_open(MQ_NAME_1, oflag, 0777, &mqstat)) == (mqd_t)-1) {
		printf("mq_open doesn't return success \r\n");
		return -1;
	}
	if ((mq2 = mq_open(MQ_NAME_2, oflag, 0777, &mqstat)) == (mqd_t)-1) {
		printf("mq_open doesn't return success \r\n");
		return -1;
	}
	pthread_create(&send1, NULL, (void *)send_1, (void *)&mq1);
	pthread_create(&send2, NULL, (void *)send_2, (void *)&mq2);
	pthread_create(&rev1, NULL, (void *)receive_1, (void *)&mq1);
	pthread_create(&rev2, NULL, (void *)receive_2, (void *)&mq2);
	pthread_join(send1, NULL);
	pthread_join(send2, NULL);
	pthread_join(rev1, NULL);
	pthread_join(rev2, NULL);

	mq_close(mq1);
	mq_close(mq2);
	mq_unlink(MQ_NAME_1);
	mq_unlink(MQ_NAME_2);
	printf("mq PASS \r\n");
	return 0;
}
