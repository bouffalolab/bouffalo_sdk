#include "bflb_mtimer.h"
#include "board.h"
#include "bl618dg_glb.h"
#include "bflb_uart.h"
#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"

#ifdef CONFIG_RPMSG_SERVICE
#include "ipc_throughput_test.h"
#endif

static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);

#ifdef CONFIG_RPMSG_SERVICE
#define AUTO_TEST_TASK_STACK_SIZE  512U
#define AUTO_TEST_TASK_PRIORITY    7U

const bool ipc_throughput_crc_enabled = true;

static void ipc_throughput_auto_test_task(void *param)
{
    char *test_10s_argv[] = { "ipc_throughput_test", "1400", "10" };
    char *test_49h_argv[] = { "ipc_throughput_test", "1400", "172800" };

    (void)param;

    while (!ipc_throughput_is_ready()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    (void)cmd_ipc_throughput_test(3, test_10s_argv);
    vTaskDelay(pdMS_TO_TICKS((2U * DEFAULT_DURATION_SEC +
                              2U * PHASE_GAP_SEC) * 1000U));
    (void)cmd_ipc_throughput_test(3, test_49h_argv);
    vTaskDelete(NULL);
}
#endif

int main(void)
{
    board_init();
    configASSERT((configMAX_PRIORITIES > 4));
    uart0 = bflb_device_get_by_name("uart0");
    shell_init();
    printf("hello world ap:%d\r\n",GLB_Get_Core_Type());
    shell_init_with_task(uart0);
#ifdef CONFIG_RPMSG_SERVICE
    int ret = ipc_throughput_init();
    if (ret < 0) {
        printf("[AP] IPC throughput init failed: %d\r\n", ret);
        return ret;
    }
    if (xTaskCreate(ipc_throughput_auto_test_task, "ipc_auto",
                    AUTO_TEST_TASK_STACK_SIZE, NULL,
                    AUTO_TEST_TASK_PRIORITY, NULL) != pdPASS) {
        printf("[AP] IPC throughput auto task create failed\r\n");
        return -1;
    }
#else
    int ipc_master(void);
    ipc_master();
#endif
    vTaskStartScheduler();
    while (1) {
    }
}

int shell_test(int argc, char **argv)
{
    printf("shell test\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_test, test, shell test.);

int shell_cpu_nop(int argc, char **argv)
{
    vTaskEnterCritical();
    uint64_t start_nop = bflb_mtimer_get_time_us();
    printf("before nop loop:%llu\r\n",start_nop);
    __asm__ volatile(
        "li t0, 1000000\n"
        "1: nop\n"
        "addi t0, t0, -1\n"
        "bnez t0, 1b\n"
        ::: "t0");
    uint64_t end_nop = bflb_mtimer_get_time_us();
    printf("after nop loop:%llu\r\n",end_nop);
    vTaskExitCritical();
    printf("nop loop time:%llu\r\n",end_nop - start_nop);
    printf("cpu freq:%llu mhz\r\n",3000000 / (end_nop - start_nop));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_cpu_nop, cpu_nop, shell cpu_nop.);
