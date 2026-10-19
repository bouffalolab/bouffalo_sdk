//#include <bl_sys/time.h>
#include "list.h"
#include "util.h"
#include "log.h"
#include "rtsp_srv.h"
#include "delay_task.h"


/* Mean it's time to do the delayed task in queue. */
#define TIME_UP 0


static unsigned long long last_sync_time;


static unsigned long long time_now(void)
{
#if 0
    struct timeval tv;
    gettimeofday(&tv, NULL);

    /* 1s = (1 * MILLION)us */
    return tv.tv_sec * MILLION + tv.tv_usec;
#else
    return xTaskGetTickCount()*1000;
#endif
}

/*
 * Synchronize the delay time in the task queue,
 * and check which task is time up.
 */
static void sync_delay_time(void)
{
    int elapsed = 0;            /* Elapsed time since last sync time. */
    unsigned long long now = 0;
    struct delay_task *dtp = 0; /* Used as loop curser. */

    /* The system clock has gone back in time, just reset sync time. */
    now = time_now();
    if (now < last_sync_time) {
        last_sync_time = now;
        return;
    }

    /* Re-calculate the delay time of any task whose time is up. */
    elapsed = (now - last_sync_time >= INT_MAX) ? 0 : (now - last_sync_time);
    list_for_each_entry(dtp, &rtsp_srv.delay_task_queue, entry) {
        if (dtp->delta_time <= elapsed) { /* Time up. */
            elapsed -= dtp->delta_time;
            dtp->delta_time = TIME_UP;
            continue;
        } else {
            dtp->delta_time -= elapsed;
            break;
        }
    }

    last_sync_time = now;
    return;
}

/*
 * Get the minimum delay time in the task queue.
 * The delay time of first task must be the minimun one ^_^
 * Usually used in epoll_wait()'s timeout.
 */
int get_min_delay_time(void)
{
    struct delay_task *dtp = NULL;

    if (!list_empty(&rtsp_srv.delay_task_queue)) {
        dtp = list_first_entry(&rtsp_srv.delay_task_queue, struct delay_task, entry);
        sync_delay_time();
        return dtp->delta_time;
    }
    return -1;
}

static void add_delay_task(struct delay_task *dtp)
{
    struct list_head *pos = NULL;    /* The position which new task is added before. */
    struct delay_task *tmp = NULL;   /* Used as loop curser. */

    /* Synchronize the delay time of tasks to improve accuracy. */
    sync_delay_time();

    /* Find the proper entry of new delay task. */
    pos = &rtsp_srv.delay_task_queue;
    list_for_each_entry(tmp, &rtsp_srv.delay_task_queue, entry) {
        if (tmp->delta_time > dtp->delta_time) {
            tmp->delta_time -= dtp->delta_time;
            pos = &tmp->entry;
            break;
        } else {
            dtp->delta_time -= tmp->delta_time;
        }
    }

    /* Add the new delay task to the queue. */
    list_add_tail(&dtp->entry, pos);
    return;
}

static void del_delay_task(struct delay_task *dtp)
{
    struct delay_task *next_dtp = NULL; /* The task just after @dtp in delay queue. */

    if (!list_empty(&rtsp_srv.delay_task_queue) &&
        !list_is_singular(&rtsp_srv.delay_task_queue)) {
        next_dtp = list_entry(dtp->entry.next, struct delay_task, entry);
        next_dtp->delta_time += dtp->delta_time;
    }

    list_del(&dtp->entry);

    return;
}

struct delay_task *create_delay_task(delay_task_proc_t proc, void *arg, int delay)
{
    struct delay_task *dtp = NULL;

    dtp = mallocz(sizeof(*dtp));
    if (!dtp) {
        printf(ERR "Allocate memory for struct delay_task failed!\n");
        return NULL;
    }
    dtp->arg = arg;
    dtp->proc = proc;
    dtp->delta_time = delay;

    /* Add delayed task to queue. */
    add_delay_task(dtp);

    return dtp;
}

void destroy_delay_task(struct delay_task *dtp)
{
    struct delay_task *tmp = NULL;
    
    /* Delete delayed task from queue. */
    list_for_each_entry(tmp, &rtsp_srv.delay_task_queue, entry) {
        if (tmp == dtp) {
            del_delay_task(dtp);
            freez(dtp);
            return;
        }
    }
    return;
}

void update_delay_time(struct delay_task *dtp, int delay)
{
    del_delay_task(dtp);
    dtp->delta_time = delay;
    add_delay_task(dtp);
    return;
}

void do_delay_task(void)
{
    struct delay_task *dtp = NULL;
    int next_delay = 0;

    /* Do the task in delay queue. */
    if (!list_empty(&rtsp_srv.delay_task_queue)) {
        dtp = list_first_entry(&rtsp_srv.delay_task_queue, struct delay_task, entry);

        if (dtp->delta_time != TIME_UP) {
            sync_delay_time();
        }

        if (dtp->delta_time == TIME_UP) {
            /*
             * The callback function $proc() will return -1 when
             * task is finished or failed, >= 0 when need to be
             * delayed next time.
             * 
             * We destroy the delayed task when task is finished or failed.
             */
            next_delay = dtp->proc(dtp->arg);
            if (next_delay >= 0) {
                update_delay_time(dtp, next_delay);
            } else {
                destroy_delay_task(dtp);
            }
        }
    }
    return;
}

/**
 * Test whether there's any un-done delayed task
 * matched with the @arg in queue.
 * */
void destroy_sess_task(void *arg)
{
    struct delay_task *dtp = NULL;  
    struct delay_task *tmp = NULL;

    list_for_each_entry_safe(dtp, tmp, &rtsp_srv.delay_task_queue, entry) {
        if (dtp->arg == arg) {
            destroy_delay_task(dtp);
        }
    }
    return;
}

void init_delay_task_queue(void)
{
    INIT_LIST_HEAD(&rtsp_srv.delay_task_queue);
    last_sync_time = time_now();
    return;
}

void deinit_delay_task_queue(void)
{
    return;
}
