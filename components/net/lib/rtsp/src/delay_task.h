#ifndef __DELAY_TASK_H__
#define __DELAY_TASK_H__


#define THOUSAND    1000
#define MILLION     1000000


typedef int (*delay_task_proc_t)(void *);


/*
 * Delay task to be done in the future.
 */
struct delay_task {
    struct list_head entry;  /* Entry of task_queue. */
    int delta_time;          /* Delay time relative to previous delayed task. */
    void *arg;               /* Argument for the callback function. */
    delay_task_proc_t proc;  /* Callback of the task. */
};


void do_delay_task(void);
int get_min_delay_time(void);
void update_delay_time(struct delay_task *dtp, int delay);
struct delay_task *create_delay_task(delay_task_proc_t proc, void *arg, int delay);
void destroy_delay_task(struct delay_task *dtp);
void destroy_sess_task(void *arg);
void init_delay_task_queue(void);
void deinit_delay_task_queue(void);


#endif /* __DELAY_TASK_H__ */

