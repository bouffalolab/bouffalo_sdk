#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

#include "lfs_kv.h"

int cmd_atef_test(int argc, char **argv)
{
    char buf[65];
    int ret;

    printf("lfs_kv start\r\n");

    memset(buf, 0, sizeof(buf));
    ret = lfs_kv_get_blob("atkeykey", buf, sizeof(buf), NULL);
    buf[64] = 0;
    printf("lfs_kv get, ret:%d, val:%s\r\n", ret, buf);

    ret = lfs_kv_set_blob("atkeykey", (const char *)"valval", strlen("valval"));
    printf("lfs_kv set, ret:%d\r\n", ret);

    memset(buf, 0, sizeof(buf));
    ret = lfs_kv_get_blob("atkeykey", buf, sizeof(buf), NULL);
    buf[64] = 0;
    printf("lfs_kv get, ret:%d, val:%s\r\n", ret, buf);

    printf("lfs_kv end\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_atef_test, atef_test, at lfs_kv test.);

int cmd_atef_set(int argc, char **argv)
{
    int ret;

    if (argc != 3) {
        printf("arg error\r\n");
    }

    ret = lfs_kv_set_blob(argv[1], (const char *)(argv[2]), strlen(argv[2]));
    printf("lfs_kv set key:%s, val:%s, ret:%d\r\n", argv[1], argv[2], ret);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_atef_set, atef_set, at lfs_kv set.);

int cmd_atef_dump(int argc, char **argv)
{
    lfs_kv_print();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_atef_dump, atef_dump, at lfs_kv dump.);

void app_easyflash4_init(void)
{
    bflb_mtd_init();
    lfs_kv_init();
}

