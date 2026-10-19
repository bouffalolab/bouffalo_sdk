#include <stdio.h>
#include <string.h>
#include "board.h"
#include "shell.h"
#include "bflb_core.h"
#include <FreeRTOS.h>
#include "task.h"
#include "test_common.h"

/* Forward declaration for shell_init_with_task (not declared in shell.h) */
extern void shell_init_with_task(struct bflb_device_s *uart);

/* Stack overflow hook - called when FreeRTOS detects stack overflow */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("\r\n[STACK OVERFLOW] Task: %s\r\n", pcTaskName);
    while (1) {
        /* Halt to allow debugging */
    }
}

/* Global test counters */
int g_tests_run = 0;
int g_tests_passed = 0;
int g_tests_failed = 0;
int g_tests_skipped = 0;

/* External declarations for each test group */
extern void test_pthread_run(void);
extern void test_pthread_attr_run(void);
extern void test_pthread_mutex_run(void);
extern void test_pthread_cond_run(void);
extern void test_pthread_barrier_run(void);
extern void test_semaphore_run(void);
extern void test_mqueue_run(void);
extern void test_clock_timer_run(void);
extern void test_sched_unistd_run(void);

/* Run all tests */
static void run_all_tests(void)
{
    printf("\r\n===== POSIX API Test Suite Start =====\r\n\r\n");

    /* Reset counters */
    g_tests_run = 0;
    g_tests_passed = 0;
    g_tests_failed = 0;
    g_tests_skipped = 0;

    printf("--- 1. pthread tests ---\r\n");
    test_pthread_run();

    printf("\r\n--- 2. pthread_attr tests ---\r\n");
    test_pthread_attr_run();

    printf("\r\n--- 3. pthread_mutex tests ---\r\n");
    test_pthread_mutex_run();

    printf("\r\n--- 4. pthread_cond tests ---\r\n");
    test_pthread_cond_run();

    printf("\r\n--- 5. pthread_barrier tests ---\r\n");
    test_pthread_barrier_run();

    printf("\r\n--- 6. semaphore tests ---\r\n");
    test_semaphore_run();

    printf("\r\n--- 7. mqueue tests ---\r\n");
    test_mqueue_run();

    printf("\r\n--- 8. clock/timer tests ---\r\n");
    test_clock_timer_run();

    printf("\r\n--- 9. sched/unistd tests ---\r\n");
    test_sched_unistd_run();

    print_test_summary();
}

/* Shell command: run all tests */
static void cmd_posix_test(int argc, char **argv)
{
    /* Run tests directly in shell task context */
    run_all_tests();
}
SHELL_CMD_EXPORT_ALIAS(cmd_posix_test, posix_test, Run all POSIX tests);

/* Shell command: run single test group */
static void cmd_posix_test_single(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: posix_test_single <group>\r\n");
        printf("Groups: pthread, pthread_attr, mutex, cond, barrier, sem, mqueue, clock, sched\r\n");
        return;
    }

    g_tests_run = 0;
    g_tests_passed = 0;
    g_tests_failed = 0;
    g_tests_skipped = 0;

    if (strcmp(argv[1], "pthread") == 0) {
        test_pthread_run();
    } else if (strcmp(argv[1], "pthread_attr") == 0) {
        test_pthread_attr_run();
    } else if (strcmp(argv[1], "mutex") == 0) {
        test_pthread_mutex_run();
    } else if (strcmp(argv[1], "cond") == 0) {
        test_pthread_cond_run();
    } else if (strcmp(argv[1], "barrier") == 0) {
        test_pthread_barrier_run();
    } else if (strcmp(argv[1], "sem") == 0) {
        test_semaphore_run();
    } else if (strcmp(argv[1], "mqueue") == 0) {
        test_mqueue_run();
    } else if (strcmp(argv[1], "clock") == 0) {
        test_clock_timer_run();
    } else if (strcmp(argv[1], "sched") == 0) {
        test_sched_unistd_run();
    } else {
        printf("Unknown test group: %s\r\n", argv[1]);
        return;
    }

    print_test_summary();
}
SHELL_CMD_EXPORT_ALIAS(cmd_posix_test_single, posix_test_single, Run single POSIX test group);

int main(void)
{
    board_init();

    printf("\r\n========================================\r\n");
    printf("  FreeRTOS POSIX Test Application\r\n");
    printf("========================================\r\n");
    printf("\r\nShell commands:\r\n");
    printf("  posix_test         - Run all tests\r\n");
    printf("  posix_test_single  - Run single test group\r\n");
    printf("\r\n");

    /* Initialize shell task */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    vTaskStartScheduler();   /* starts scheduler, never returns */

    while (1) {
        /* Should never reach here */
    }
}
