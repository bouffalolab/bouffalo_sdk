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

int cmd_netifd_nano(int argc, char *argv[]);
int wifi_mgmr_ap_start_cmd(int argc, char *argv[]);
static void cmd_demo(void)
{
    {
        int argc = 9;
        char *argv[]={"wifi_ap_start","-s", "ap_bridge", "-k", "12345678", "-g", "0", "-d", "0"};
        wifi_mgmr_ap_start_cmd(argc, argv);
    }
    vTaskDelay(200);
    {
        int argc = 5;
        char *argv[]={"netifd", "bridge", "create", "-b", "br4"};
        cmd_netifd_nano(argc, argv);
    }
    vTaskDelay(20);
    {
        int argc = 4;
        char *argv[]={"netifd", "up", "-i", "br4"};
        cmd_netifd_nano(argc, argv);
    }
    vTaskDelay(20);
    {
        int argc = 7;
        char *argv[]={"netifd", "bridge", "add", "-b", "br4", "-i", "wl3"};
        cmd_netifd_nano(argc, argv);
    }
    vTaskDelay(20);
    {
        int argc = 7;
        char *argv[]={"netifd", "bridge", "add", "-b", "br4", "-i", "ex1"};
        cmd_netifd_nano(argc, argv);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_demo, demo, demo test.);

void app_sample_init(void)
{
}

