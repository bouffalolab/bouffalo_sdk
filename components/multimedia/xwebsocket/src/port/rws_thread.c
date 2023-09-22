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

#define _DEFAULT_APP_PRI 10

static void * rws_thread_func_priv(void * some_pointer)
{
    rws_thread t = (rws_thread)some_pointer;
    t->thread_function(t->user_object);

    return NULL;
}

rws_thread rws_thread_create(rws_thread_funct thread_function, void * user_object)
{
    rws_thread t = NULL;
    int res = -1;
    pthread_attr_t attr;

    if (!thread_function) {
        return NULL;
    }
    t = (rws_thread)rws_malloc_zero(sizeof(struct rws_thread_struct));
    t->user_object = user_object;
    t->thread_function = thread_function;
    if (pthread_attr_init(&attr) == 0) {
        if (pthread_attr_setstacksize(&attr, 8 * 1024) == 0) {
            //if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0) {
                //pthread_attr_t attr;
                pthread_attr_init(&attr);
                struct sched_param sched;
                pthread_attr_getschedparam(&attr,&sched);
                sched.sched_priority = _DEFAULT_APP_PRI + 1;
                pthread_attr_setschedparam(&attr,&sched);
                pthread_attr_setstacksize(&attr, 8192);
                if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) {
                    res = pthread_create(&t->thread, &attr, &rws_thread_func_priv, (void *)t);
                    //TODO: Pthread setname don't support.
                    //pthread_setname_np(t->thread, "GnWebsoc");
                }
            //}
        }

        pthread_attr_destroy(&attr);
    }
    CHECK_RET_WITH_RET(res == 0, NULL);
    return t;
}

int rws_thread_join(rws_thread thread, void ** retval)
{
    return pthread_join(thread->thread, retval);
}

void rws_thread_sleep(const unsigned int millisec)
{
    usleep(millisec * 1000); // 1s = 1'000'000 microsec.
}

rws_mutex rws_mutex_create_recursive(void)
{
    pthread_mutex_t * mutex = (pthread_mutex_t *)rws_malloc_zero(sizeof(pthread_mutex_t));
    int res = -1;
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) == 0) {
        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0) {
            res = pthread_mutex_init(mutex, &attr);
        }
        pthread_mutexattr_destroy(&attr);
    }
    return mutex;
}

void rws_mutex_lock(rws_mutex mutex)
{
    if (mutex) {
        pthread_mutex_lock((pthread_mutex_t *)mutex);
    }
}

void rws_mutex_unlock(rws_mutex mutex)
{
    if (mutex) {
        pthread_mutex_unlock((pthread_mutex_t *)mutex);
    }
}

void rws_mutex_delete(rws_mutex mutex)
{
    if (mutex) {
        pthread_mutex_destroy((pthread_mutex_t *)mutex);
        rws_free(mutex);
    }
}

