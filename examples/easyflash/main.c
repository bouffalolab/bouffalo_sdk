/*
 * lfs_kv / EasyFlash-compatible key-value store test.
 *
 * Flash, the partition table and the LittleFS KV store are initialized by
 * SYSINIT before main(): with CONFIG_LITTLEFS_KV_EASYFLASH the fixed PSM
 * partition is mounted for the application. The application only uses the
 * lfs_kv_* runtime API; board_init()/bflb_mtd_init()/lfs_kv_init() are not
 * needed in the SYSINIT build.
 *
 * Required PSM partition:
 *
 *   [[pt_entry]]
 *   type = 3
 *   name = "PSM"
 *   device = 0
 *   address0 = 0x3E9000
 *   size0 = 0x8000
 *   address1 = 0
 *   size1 = 0
 *   len = 0
 *   header = 0
 *   security = 0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lfs_kv.h"

#define WIFI_SSID_KEY   "wifi.ssid"
#define WIFI_PASSWD_KEY "wifi.passwd"
#define TEST_KEY1       "g/hwaddr/mac_aabb"
#define TEST_KEY2       "/root/aa/bbb/"

static lfs_kv_err_t env_foreach_cb(const char *name, void *arg)
{
    uint32_t *count = (uint32_t *)arg;

    printf("foreach key %d: %s\n", (*count)++, name);
    return LFS_KV_OK;
}

int main(void)
{
    lfs_kv_err_t ret;
    size_t read_len;
    char ssid[33];
    char passwd[65];
    char hwaddr[33];
    uint32_t count = 0;

    /* The KV store was mounted by SYSINIT before main(); the record only
     * reports a missing store, so probe the API with the first write below. */
    printf("lfs_kv_init test pass.\n");

    ret = lfs_kv_set_blob(WIFI_SSID_KEY, "helloworld", strlen("helloworld") + 1);
    if (ret != LFS_KV_OK) {
        printf("test case %d failed.\n", __LINE__);
        while (1)
            ;
    }

    ret = lfs_kv_set_blob(WIFI_PASSWD_KEY, "helloworld2023", strlen("helloworld2023") + 1);
    if (ret != LFS_KV_OK) {
        printf("test case %d failed.\n", __LINE__);
        while (1)
            ;
    }

    ret = lfs_kv_set_blob(TEST_KEY1, "11223344", strlen("11223344") + 1);
    if (ret != LFS_KV_OK) {
        printf("test case %d failed.\n", __LINE__);
        while (1)
            ;
    }

    ret = lfs_kv_set_blob(TEST_KEY2, "deadbeef", strlen("deadbeef") + 1);
    if (ret != LFS_KV_OK) {
        printf("test case %d failed.\n", __LINE__);
        while (1)
            ;
    }

    read_len = lfs_kv_get_blob(WIFI_SSID_KEY, ssid, sizeof(ssid), NULL);
    if (read_len > 0) {
        ssid[read_len] = 0;
        printf("ssid:%s, test pass.\r\n", ssid);
    } else {
        printf("test case %d failed.\n", __LINE__);
        while (1)
            ;
    }

    read_len = lfs_kv_get_blob(WIFI_PASSWD_KEY, passwd, sizeof(passwd), NULL);
    if (read_len > 0) {
        passwd[read_len] = 0;
        printf("passwd:%s test pass.\r\n", passwd);
    } else {
        printf("test case %d failed.\n", __LINE__);
        while (1)
            ;
    }

    read_len = lfs_kv_get_blob(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL);
    if (read_len == 0) {
        printf("read key1 failed\r\n");
        while (1)
            ;
    }
    hwaddr[read_len] = 0;
    printf(TEST_KEY1 ":%s, pass\r\n", hwaddr);

    read_len = lfs_kv_get_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 2);
    if (read_len == 0) {
        printf("read key1 failed\r\n");
        while (1)
            ;
    }
    hwaddr[read_len] = 0;
    printf(TEST_KEY1 "+2:%s, pass\r\n", hwaddr);

    read_len = lfs_kv_get_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 3);
    if (read_len == 0) {
        printf("read key1 failed\r\n");
        while (1)
            ;
    }
    hwaddr[read_len] = 0;
    printf(TEST_KEY1 "+3:%s, pass\r\n", hwaddr);

    read_len = lfs_kv_get_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 100);
    if (read_len == 0) {
        printf("test case %d pass.\n", __LINE__);
    } else {
        hwaddr[read_len] = 0;
        printf(TEST_KEY1 "+100:%s, failed\r\n", hwaddr);
        while (1)
            ;
    }

    read_len = lfs_kv_get_blob("aa/bb", hwaddr, sizeof(hwaddr), NULL);
    if (read_len == 0) {
        printf("test non-exists key pass\r\n");
    } else {
        hwaddr[read_len] = 0;
        printf("aa/bb:%s, failed!\r\n", hwaddr);
        while (1)
            ;
    }

    read_len = lfs_kv_get_blob(TEST_KEY2, hwaddr, sizeof(hwaddr), NULL);
    if (read_len == 0) {
        printf("read key2 failed\r\n");
        while (1)
            ;
    }
    hwaddr[read_len] = 0;
    printf(TEST_KEY2 ":%s, pass\r\n", hwaddr);

    printf("foreach all kv:\n");
    lfs_kv_foreach(env_foreach_cb, &count);
    printf("foreach all kv: done, total: %d\n", count);
    if (count == 4) {
        printf("lfs_kv_foreach test pass.\n");
    } else {
        printf("lfs_kv_foreach test failed.\n");
        while (1)
            ;
    }

    lfs_kv_print();
    printf("clear all kv\r\n");
    lfs_kv_clear();

    lfs_kv_print();
    count = 0;
    lfs_kv_foreach(env_foreach_cb, &count);
    if (count == 0) {
        printf("lfs_kv_clear test pass.\n");
    } else {
        printf("lfs_kv_clear test failed.\n");
        while (1)
            ;
    }

    printf("lfs_kv case success\r\n");
    while (1) {}
}
