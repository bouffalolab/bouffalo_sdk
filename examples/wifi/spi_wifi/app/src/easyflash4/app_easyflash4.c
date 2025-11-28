#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

#include <easyflash.h>

int cmd_atef_test(int argc, char **argv)
{
    char buf[65];
    int ret;

    printf("ef start\r\n");

    memset(buf, 0, sizeof(buf));
    ret = ef_get_env_blob("atkeykey", buf, sizeof(buf), NULL);
    buf[64] = 0;
    printf("ef get, ret:%d, val:%s\r\n", ret, buf);

    ret = ef_set_env_blob("atkeykey", (const char *)"valval", strlen("valval"));
    printf("ef set, ret:%d\r\n", ret);

    memset(buf, 0, sizeof(buf));
    ret = ef_get_env_blob("atkeykey", buf, sizeof(buf), NULL);
    buf[64] = 0;
    printf("ef get, ret:%d, val:%s\r\n", ret, buf);

    printf("ef end\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_atef_test, atef_test, at easyflash test.);

int cmd_atef_set(int argc, char **argv)
{
    int ret;

    if (argc != 3) {
        printf("arg error\r\n");
    }

    ret = ef_set_env_blob(argv[1], (const char *)(argv[2]), strlen(argv[2]));
    printf("ef set key:%s, val:%s, ret:%d\r\n", argv[1], argv[2], ret);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_atef_set, atef_set, at easyflash set.);

int cmd_atef_dump(int argc, char **argv)
{
    ef_print_env();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_atef_dump, atef_dump, at easyflash dump.);

void app_easyflash4_init(void)
{
    bflb_mtd_init();
    easyflash_init();
}


