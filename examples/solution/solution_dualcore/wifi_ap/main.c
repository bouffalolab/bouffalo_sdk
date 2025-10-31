#include "bflb_mtimer.h"
#include "board.h"
#include "bl616d_glb.h"
#include "bflb_uart.h"
#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"

static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);
extern int app_wifi_init(void);

void app_entry(void *param)
{
    app_wifi_init();
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    configASSERT((configMAX_PRIORITIES > 4));
    uart0 = bflb_device_get_by_name("uart0");
    shell_init();
    printf("hello world ap:%d\r\n",GLB_Get_Core_Type());
    shell_init_with_task(uart0);
    
    xTaskCreate(app_entry, (char *)"init", 2048, NULL, 15, NULL);

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