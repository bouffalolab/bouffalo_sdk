#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <bl808.h>
#include <bl_irq.h>
#include "cli.h"

#define MSIP_BASE (0xE0000000UL)
static unsigned long n_payload;

static void proc_test1_entry(void *p)
{
    while (1) {
        vTaskDelay(1);
    }
    vTaskDelete(NULL);
}

static void ps_thead_test1_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        puts("ps-thead-test-1 n_payload, run numbers of task as env of ps-thead\r\n");
        return;
    }
    n_payload = (unsigned long)strtol(argv[1], NULL, 0);
    StackType_t *proc_stack_ptr = pvPortMalloc(200 * n_payload * sizeof(StackType_t));
    StaticTask_t *task_ptr = pvPortMalloc(n_payload * sizeof(StaticTask_t));
    if (!proc_stack_ptr || !task_ptr) {
        vPortFree(proc_stack_ptr);
        vPortFree(task_ptr);
        puts("malloc error\r\n");
        return;
    }
    for (int i = n_payload - 1; i >= 0; i--)
        xTaskCreateStatic(proc_test1_entry, (char *)"ps_thead_test1", 200, NULL, 14, proc_stack_ptr + i * 200, &task_ptr[i]);
}

static void proc_test2_entry(void *p)
{
    int s, i = n_payload * 1000 * 1000;
    while (1) {
        while (i--)
            s++;
        vTaskDelay(10);
        i = n_payload * 1000 * 1000;
    }
    vTaskDelete(NULL);
}

static void ps_thead_test2_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        puts("ps-thead-test-2 n_payload, run n million of `++`\r\n");
        return;
    }
    n_payload = (unsigned long)strtol(argv[1], NULL, 0);
    StackType_t *proc_stack_ptr = pvPortMalloc(100 * sizeof(StackType_t));
    StaticTask_t *task_ptr = pvPortMalloc(1 * sizeof(StaticTask_t));
    if (!proc_stack_ptr || !task_ptr) {
        vPortFree(proc_stack_ptr);
        vPortFree(task_ptr);
        puts("malloc error\r\n");
        return;
    }
    xTaskCreateStatic(proc_test2_entry, (char *)"ps_thead_test2", 100, NULL, 14, proc_stack_ptr, task_ptr);
}

static void proc_test3_entry(void *p)
{
    while (1) {
        // trigger irq
        *(char *)MSIP_BASE = 1;
        vTaskDelay(10);
    }
    vTaskDelete(NULL);
}

static int soft_irq_count;
static void soft_irq_handler(void)
{
    if (*(char *)MSIP_BASE == 1)
        *(char *)MSIP_BASE = 0;

    int i = n_payload * 1000 * 1000;
    while (i--) {
        soft_irq_count++;
    }
}

static void ps_thead_test3_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        puts("ps-thead-test-3 n_payload, run n million of `++` in irq\r\n");
        return;
    }
    n_payload = (unsigned long)strtol(argv[1], NULL, 0);
    StackType_t *proc_stack_ptr = pvPortMalloc(100 * sizeof(StackType_t));
    StaticTask_t *task_ptr = pvPortMalloc(1 * sizeof(StaticTask_t));
    if (!proc_stack_ptr || !task_ptr) {
        vPortFree(proc_stack_ptr);
        vPortFree(task_ptr);
        puts("malloc error\r\n");
        return;
    }
    bl_irq_register(MSOFT_IRQn, soft_irq_handler);
    bl_irq_enable(MSOFT_IRQn);
    xTaskCreateStatic(proc_test3_entry, (char *)"ps_thead_test3", 100, NULL, 14, proc_stack_ptr, task_ptr);
}

static void proc_test4_entry(void *p)
{
    while (1) {
        // trigger irq
        *(volatile char *)MSIP_BASE = 1;
    }
    vTaskDelete(NULL);
}

static void test4_irq_handler(void)
{
    int i = 100;
    if (*(char *)MSIP_BASE == 1)
        *(char *)MSIP_BASE = 0;
    while (i--)
        soft_irq_count++;
}

static void ps_thead_test4_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 1) {
        puts("ps-thead-test-4 n_irqs, run nearly 100% irq\r\n");
        return;
    }
    StackType_t *proc_stack_ptr = pvPortMalloc(100 * sizeof(StackType_t));
    StaticTask_t *task_ptr = pvPortMalloc(1 * sizeof(StaticTask_t));
    if (!proc_stack_ptr || !task_ptr) {
        vPortFree(proc_stack_ptr);
        vPortFree(task_ptr);
        puts("malloc error\r\n");
        return;
    }
    bl_irq_register(MSOFT_IRQn, test4_irq_handler);
    bl_irq_enable(MSOFT_IRQn);
    xTaskCreateStatic(proc_test4_entry, (char *)"ps_thead_test4", 100, NULL, 14, proc_stack_ptr, task_ptr);
}

const struct cli_command ps_thead_test_cmds[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "ps_extend_test_1", "run n tasks as env of ps-thead", ps_thead_test1_cmd },
    { "ps_extend_test_2", "run nM payload as env of ps-thead", ps_thead_test2_cmd },
    { "ps_extend_test_3", "run nM payload in irq as env of ps-thead", ps_thead_test3_cmd },
    { "ps_extend_test_4", "trigger nearly 100% irqs as env of ps-thead", ps_thead_test4_cmd },
};

int freertos_helper_cli_test_init(void)
{
    return 0;
}
