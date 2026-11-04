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

/* External declarations for each test group.  Single-TARGET builds link one
 * test translation unit; weak declarations keep the common shell runner
 * linkable in that mode. */
#if defined(__GNUC__)
#define POSIX_TEST_WEAK __attribute__((weak))
#else
#define POSIX_TEST_WEAK
#endif
extern void test_pthread_run(void) POSIX_TEST_WEAK;
extern void test_pthread_attr_run(void) POSIX_TEST_WEAK;
extern void test_pthread_mutex_run(void) POSIX_TEST_WEAK;
extern void test_pthread_cond_run(void) POSIX_TEST_WEAK;
extern void test_pthread_barrier_run(void) POSIX_TEST_WEAK;
extern void test_semaphore_run(void) POSIX_TEST_WEAK;
extern void test_mqueue_run(void) POSIX_TEST_WEAK;
extern void test_clock_timer_run(void) POSIX_TEST_WEAK;
extern void test_sched_unistd_run(void) POSIX_TEST_WEAK;
extern void test_cpp_mutex_run(void) POSIX_TEST_WEAK;

typedef struct {
    const char *name;
    const char *title;
    void (*run)(void);
} posix_test_group_t;

static const posix_test_group_t s_test_groups[] = {
    { "pthread", "1. pthread tests", test_pthread_run },
    { "pthread_attr", "2. pthread_attr tests", test_pthread_attr_run },
    { "mutex", "3. pthread_mutex tests", test_pthread_mutex_run },
    { "cond", "4. pthread_cond tests", test_pthread_cond_run },
    { "barrier", "5. pthread_barrier tests", test_pthread_barrier_run },
    { "sem", "6. semaphore tests", test_semaphore_run },
    { "mqueue", "7. mqueue tests", test_mqueue_run },
    { "clock", "8. clock/timer tests", test_clock_timer_run },
    { "sched", "9. sched/unistd tests", test_sched_unistd_run },
    { "cpp_mutex", "10. C++ std::mutex tests", test_cpp_mutex_run },
};

/* Run all tests */
static void run_all_tests(void)
{
    printf("\r\n===== POSIX API Test Suite Start =====\r\n\r\n");

    /* Reset counters */
    g_tests_run = 0;
    g_tests_passed = 0;
    g_tests_failed = 0;
    g_tests_skipped = 0;

    for (size_t i = 0; i < sizeof(s_test_groups) / sizeof(s_test_groups[0]); i++) {
        if (s_test_groups[i].run != NULL) {
            printf("\r\n--- %s ---\r\n", s_test_groups[i].title);
            s_test_groups[i].run();
        }
    }

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
        printf("Groups: pthread, pthread_attr, mutex, cond, barrier, sem, mqueue, clock, sched, cpp_mutex\r\n");
        return;
    }

    g_tests_run = 0;
    g_tests_passed = 0;
    g_tests_failed = 0;
    g_tests_skipped = 0;

    for (size_t i = 0; i < sizeof(s_test_groups) / sizeof(s_test_groups[0]); i++) {
        if (strcmp(argv[1], s_test_groups[i].name) == 0) {
            if (s_test_groups[i].run == NULL) {
                printf("Test group was not built: %s\r\n", argv[1]);
                return;
            }
            s_test_groups[i].run();
            print_test_summary();
            return;
        }
    }
    printf("Unknown test group: %s\r\n", argv[1]);
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
