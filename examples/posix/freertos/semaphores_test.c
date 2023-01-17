
/*
 * Copyright (c) 2002, Intel Corporation. All rights reserved.
 * Created by:  crystal.xiong REMOVE-THIS AT intel DOT com
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this
 * source tree.
 *
 * This is a test about producer and consumer. Producer sends data
 * to a buffer. Consumer keeps reading data from the buffer.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/fcntl.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "FreeRTOS_POSIX/semaphore.h"

#define BUF_SIZE	5
#define Max_Num		10

typedef struct {
	int buffer[BUF_SIZE];
	sem_t occupied;
	sem_t empty;
	sem_t lock;
} buf_t;

static int in, out;

static void producer(buf_t * buf)
{
	int data;
	int i;

	for (i = 0; i < Max_Num; i++) {
		if (-1 == sem_wait(&buf->occupied)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
		if (-1 == sem_wait(&buf->lock)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
		data = 100 * i;
		buf->buffer[in] = data;
		printf("producer has added %d to the buffer[%d] \r\n", data, in);
		in = (in + 1) % BUF_SIZE;
		if (-1 == sem_post(&buf->lock)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
		if (-1 == sem_post(&buf->empty)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
	}
	pthread_exit(NULL);
}

static void consumer(buf_t * buf)
{
	int data;
	int i;

	for (i = 0; i < Max_Num; i++) {
		if (-1 == sem_wait(&buf->empty)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
		if (-1 == sem_wait(&buf->lock)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
		data = buf->buffer[out];
		printf("consumer has taken %d from buffer[%d] \r\n", data, out);
		out = (out + 1) % BUF_SIZE;
		if (-1 == sem_post(&buf->lock)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
		if (-1 == sem_post(&buf->occupied)) {
			printf("sem_wait didn't return success \r\n");
			pthread_exit((void *)1);
		}
	}
	pthread_exit(0);
}

int semaphores_test(void)
{
	int shared = 1;
	int occupied_value = BUF_SIZE;
	int empty_value = 0;
	int lock_value = 1;
	buf_t buf;
	pthread_t con, pro;

	if (-1 == sem_init(&buf.occupied, shared, occupied_value)) {
		printf("sem_init didn't return success \r\n");
		return -1;
	}
	if (-1 == sem_init(&buf.empty, shared, empty_value)) {
		printf("sem_init didn't return success \r\n");
		return -1;
	}
	if (-1 == sem_init(&buf.lock, shared, lock_value)) {
		printf("sem_init didn't return success \r\n");
		return -1;
	}
	in = out = 0;

	pthread_create(&con, NULL, (void *)consumer, &buf);
	pthread_create(&pro, NULL, (void *)producer, &buf);
	pthread_join(con, NULL);
	pthread_join(pro, NULL);
	sem_destroy(&buf.occupied);
	sem_destroy(&buf.empty);

	printf("sema PASS\r\r\n");
	return 0;
}
