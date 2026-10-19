/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "librws.h"
#include "rws_thread.h"
#include "rws_memory.h"
#include "rws_common.h"

#include <unistd.h>
#include "msp/kernel.h"

struct rws_msp {
	struct rws_thread_struct thread;
	msp_sem_t sem;
};

static void _rws_thread_func_priv(void * some_pointer)
{
    struct rws_msp *t = (struct rws_msp *)some_pointer;
    t->thread.thread_function(t->thread.user_object);
    msp_sem_signal(&t->sem);
    msp_task_exit(0);
}

rws_thread rws_thread_create(rws_thread_funct thread_function, void * user_object)
{
    struct rws_msp *t = NULL;

    if (!thread_function) {
        return NULL;
    }
    t = rws_malloc_zero(sizeof(struct rws_msp));
    t->thread.user_object = user_object;
    t->thread.thread_function = thread_function;
    msp_sem_new(&t->sem, 0);
    msp_task_new("rws-thread", _rws_thread_func_priv, t, 8192);
    return &t->thread;
}

int rws_thread_join(rws_thread thread, void ** retval)
{
    struct rws_msp *t = (struct rws_msp *)thread;
    int ret = msp_sem_wait(&t->sem, MSP_WAIT_FOREVER);
    if (ret == 0) {
        msp_sem_free(&t->sem);
    }
    return ret;
}

void rws_thread_sleep(const unsigned int millisec)
{
    msp_msleep(millisec); // 1s = 1'000'000 microsec.
}

rws_mutex rws_mutex_create_recursive(void)
{
    msp_mutex_t * mutex = (msp_mutex_t *)rws_malloc_zero(sizeof(msp_mutex_t));
    msp_mutex_new(mutex);
    return (rws_mutex)mutex;
}

void rws_mutex_lock(rws_mutex mutex)
{
    if (mutex) {
        msp_mutex_lock((msp_mutex_t *)mutex, MSP_WAIT_FOREVER);
    }
}

void rws_mutex_unlock(rws_mutex mutex)
{
    if (mutex) {
        msp_mutex_unlock((msp_mutex_t *)mutex);
    }
}

void rws_mutex_delete(rws_mutex mutex)
{
    if (mutex) {
        msp_mutex_free((msp_mutex_t *)mutex);
        rws_free(mutex);
    }
}

