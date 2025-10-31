#include "bflb_mtimer.h"
#include "board.h"
#include "bl616d_glb.h"
#include "bflb_uart.h"
#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"

static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);

int main(void)
{
    int ch;
    board_init();
    configASSERT((configMAX_PRIORITIES > 4));
    uart0 = bflb_device_get_by_name("uart0");
    shell_init();
    printf("hello world ap:%d\r\n",GLB_Get_Core_Type());
    shell_init_with_task(uart0);
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