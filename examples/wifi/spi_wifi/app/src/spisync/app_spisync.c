#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "mem.h"

#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define SPISLAVE_STACK_SIZE    (2048)
#define TASK_PRIORITY_SPISLAVE (16)

void spislave_demo_task(void *arg)
{
    extern void sramsync_init_cmd(char *buf, int len, int argc, char **argv);
    //extern void ramsync_loop_cmd(int argc, char **argv);

    vTaskDelay(pdMS_TO_TICKS(100));
    LOG_I("Starting tpsync3 ...\r\n");

    sspisync_init_cmd(NULL, 0, 0, NULL);
    vTaskDelay(100);
    //ramsync_loop_cmd(0, NULL);

    vTaskDelete(NULL);
}

int cmd_spisync_test(int argc, char **argv)
{
    xTaskCreate(spislave_demo_task, (char *)"spislv", SPISLAVE_STACK_SIZE/4, NULL, TASK_PRIORITY_SPISLAVE, NULL);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spisync_test, spisync_test, spisync test.);

void app_spisync_init(void)
{
}

