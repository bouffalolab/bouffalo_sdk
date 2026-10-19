#include <assert.h>
#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "board.h"

#include "bflb_mtd.h"
#include "coredump.h"

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

int main(void)
{
    bflb_mtd_info_t info;
    bflb_mtd_handle_t handle;
    int ret;

    board_init();

    bflb_mtd_init();
    ret = bflb_mtd_open("core", &handle, BFLB_MTD_OPEN_FLAG_BUSADDR);
    if (ret < 0) {
        puts("No valid coredump partition found\r\n");
    }
    memset(&info, 0, sizeof(info));
    bflb_mtd_info(handle, &info);
    printf("Found Valid coredump partition, XIP Addr %08x, flash addr %08x, size %d\r\n",
           info.xip_addr,
           info.offset,
           info.size);
    core_partition_init(info.offset, info.size);

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    vTaskStartScheduler();

    while (1) {
    }
}

int shell_crash(int argc, char **argv)
{
    printf("shell crash\r\n");
    //asm ("ebreak");
    assert(0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_crash, crash, trigger crash.);
