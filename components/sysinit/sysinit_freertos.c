/*
 * FreeRTOS integration for the SYSINIT scheduler barrier.
 *
 * The pre-scheduler SYSINIT entries run in System_Post_Init(). This file
 * registers the separator entry: it creates the sysinit continuation task,
 * then starts the scheduler and never returns. The continuation task runs the
 * post-scheduler SYSINIT entries in order: async events, the synchronous
 * TCP/IP and Wi-Fi bring-up, and finally the SI_SUB_APP record that enters
 * the application main().
 */

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "sysinit.h"

#ifdef CONFIG_SYSINIT

#include "FreeRTOS.h"
#include "task.h"

#ifndef CONFIG_SYSINIT_TASK_STACK_DEPTH
#define CONFIG_SYSINIT_TASK_STACK_DEPTH 2048
#endif

/* Called from the SI_SUB_APP record instead of the startup assembly, because
 * the scheduler is already running when main() is reached. */
extern int main(void);

#if IS_ENABLED(CONFIG_COMPAT_V2)
/* Legacy main() implementations still call vTaskStartScheduler() at the end.
 * This translation unit is linked with -Wl,--wrap,vTaskStartScheduler, so that
 * call is redirected to __wrap_vTaskStartScheduler(): after the SDK has started
 * the scheduler it prints the deprecation warning and longjmps back to the APP
 * record, skipping the rest of main() (usually while(1)); SYSINIT then
 * continues with the remaining records. The initial start from the SI_SUB_SCHED
 * barrier happens before the continuation task exists and is forwarded to the
 * real vTaskStartScheduler(). */
extern void __real_vTaskStartScheduler(void);

static jmp_buf sysinit_app_jmp;
static volatile int sysinit_app_jmp_armed;
static volatile int sysinit_scheduler_started;

void __wrap_vTaskStartScheduler(void)
{
    if (!sysinit_scheduler_started) {
        __real_vTaskStartScheduler();
        return;
    }

    printf("[W] deprecated: vTaskStartScheduler() is handled by SYSINIT, "
           "will be removed in Bouffalo SDK v3\r\n");

    if (sysinit_app_jmp_armed) {
        sysinit_app_jmp_armed = 0;
        longjmp(sysinit_app_jmp, 1);
    }
}
#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */

static void sysinit_task(void *arg)
{
    (void)arg;

#if IS_ENABLED(CONFIG_COMPAT_V2)
    sysinit_scheduler_started = 1;
#endif

    /* Run every post-scheduler entry: EVENTS, the NET/WIRELESS bring-up and
     * finally SI_SUB_APP which enters the application main(). */
    sysinit_run_post_schedule();

    vTaskDelete(NULL);
}

/* main() is an ordinary SYSINIT record: the async event records (SI_SUB_EVENTS)
 * and the synchronous TCP/IP and Wi-Fi bring-up (SI_SUB_NET / SI_SUB_WIRELESS)
 * have all completed before it runs. */
static void sysinit_app_main(const void *arg)
{
    (void)arg;

#if IS_ENABLED(CONFIG_COMPAT_V2)
    sysinit_app_jmp_armed = 1;
    if (setjmp(sysinit_app_jmp) == 0) {
        /* main() is still the application entry: it registers application
         * events, creates business tasks and returns. */
        (void)main();
    }

    /* Normal return, or longjmp from the legacy vTaskStartScheduler() call:
     * continue with the remaining SYSINIT records. */
    sysinit_app_jmp_armed = 0;
#else
    /* main() is still the application entry: it registers application events,
     * creates business tasks and returns. */
    (void)main();
#endif
}
SYSINIT(app_main, SI_SUB_APP, SI_ORDER_MIDDLE, sysinit_app_main, NULL);

static void sysinit_start_scheduler(const void *arg)
{
    (void)arg;

    /* Run the continuation at the highest usable priority so that tasks created
     * by SYSINIT entries and by main() do not preempt the initialization flow. */
    if (xTaskCreate(sysinit_task, "sysinit", (configSTACK_DEPTH_TYPE)CONFIG_SYSINIT_TASK_STACK_DEPTH, NULL,
                    configMAX_PRIORITIES - 1, NULL) != pdPASS) {
        abort();
    }

#if IS_ENABLED(CONFIG_COMPAT_V2)
    /* Forward to the real entry; the --wrap redirection only intercepts the
     * legacy application call, this initial start must actually run. */
    __real_vTaskStartScheduler();
#else
    vTaskStartScheduler();
#endif

    /* vTaskStartScheduler() does not return when the scheduler starts. */
    abort();
}
SYSINIT(sysinit_sched, SI_SUB_SCHED, SI_ORDER_FIRST, sysinit_start_scheduler, NULL);

#endif /* CONFIG_SYSINIT */
