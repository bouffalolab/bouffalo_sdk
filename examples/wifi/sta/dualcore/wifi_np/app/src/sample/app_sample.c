#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <mem.h>

#include <shell.h>

#define DBG_TAG "APP_SAMPLE"
#include <log.h>

int cmd_sample_test(int argc, char **argv)
{
    uint8_t *buf = NULL;
    int len;
    int old = 0;

    LOG_I("Starting cmd_sample_test ...\r\n");
    for (len = 100; len < 1000*1024; len+=128) {
        buf = malloc(len);

        if (buf) {
            old = len;
            free(buf);
        } else {
            printf("malloc %d success. but %d failed.\r\n", old, len);
            break;
        }
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sample_test, sample, sample test.);

int cmd_startwifi_test(int argc, char **argv)
{
    int wifi_start_firmware_task(void);
    wifi_start_firmware_task();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_startwifi_test, startwifi, startwifi test.);

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
SHELL_CMD_EXPORT_ALIAS(shell_cpu_nop, cpu_nop, shell cpu_nop);

void app_sample_init(void)
{
}