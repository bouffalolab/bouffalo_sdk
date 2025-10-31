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
    LOG_I("Starting cmd_sample_test ...\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sample_test, sample, sample test.);

void app_sample_init(void)
{
}

