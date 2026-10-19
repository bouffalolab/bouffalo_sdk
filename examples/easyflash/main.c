#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_mtd.h"
#include "lfs_kv.h"

uint8_t test_data[] = { "1234567890" };
uint8_t read_buffer[100];

#define WIFI_SSID_KEY   "wifi.ssid"
#define WIFI_PASSWD_KEY "wifi.passwd"
#define TEST_KEY1       "g/hwaddr/mac_aabb"
#define TEST_KEY2       "/root/aa/bbb/"

static lfs_kv_err_t env_foreach_cb(const char *name, void *arg) {
  uint32_t *count = (uint32_t *)arg;
  printf("foreach key %d: %s\n", (*count)++, name);
  return LFS_KV_OK;
}

int main(void)
{
    lfs_kv_err_t ret;
    size_t read_len;
    board_init();

    /* Partition and boot2 must be use, and we can only operate partition **psm** with lfs_kv (littlefs)
     *
     * partition_cfg with psm:
     *
        [[pt_entry]]
        type = 3
        name = "PSM"
        device = 0
        address0 = 0x3E9000
        size0 = 0x8000
        address1 = 0
        size1 = 0
        # compressed image must set len,normal image can left it to 0
        len = 0
        # If header is 1, it will add the header.
        header = 0
        # If header is 1 and security is 1, It will be encrypted.
        security= 0

    */
    bflb_mtd_init();
    if (lfs_kv_init() == LFS_KV_OK) {
        printf("lfs_kv_init test pass.\n");
    } else {
        printf("errno: %d\r\n", errno);
        printf("lfs_kv_init test failed.\n");
    }

    memset(read_buffer, 0, sizeof(read_buffer));

    ret = lfs_kv_set_blob(WIFI_SSID_KEY, (const char *)"helloworld", strlen("helloworld") + 1);
    if (ret != LFS_KV_OK) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = lfs_kv_set_blob(WIFI_PASSWD_KEY, (const char *)"helloworld2023", strlen("helloworld2023") + 1);
    if (ret != LFS_KV_OK) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = lfs_kv_set_blob(TEST_KEY1, (const char *)"11223344", strlen("11223344") + 1);
    if (ret != LFS_KV_OK) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = lfs_kv_set_blob(TEST_KEY2, (const char *)"deadbeef", strlen("deadbeef") + 1);
    if (ret != LFS_KV_OK) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    char ssid[33];
    char passwd[65];
    char hwaddr[33];

    read_len = lfs_kv_get_blob(WIFI_SSID_KEY, ssid, sizeof(ssid), NULL);
    if (read_len > 0) {
        ssid[read_len] = 0;
        printf("ssid:%s, test pass.\r\n", ssid);
    } else {
        printf("test case %d failed.\n", __LINE__);
        while(1);
    }

    read_len = lfs_kv_get_blob(WIFI_PASSWD_KEY, passwd, sizeof(passwd), NULL);
    if (read_len > 0) {
        passwd[read_len] = 0;
        printf("passwd:%s test pass.\r\n", passwd);
    } else {
        printf("test case %d failed.\n", __LINE__);
        while(1);
    }

    read_len = lfs_kv_get_blob(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL);
    hwaddr[read_len] = 0;
    if (read_len == 0) {
        printf("read key1 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY1 ":%s, pass\r\n", hwaddr);
    }

    read_len = lfs_kv_get_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 2);
    hwaddr[read_len] = 0;
    if (read_len == 0) {
        printf("read key1 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY1 "+2:%s, pass\r\n", hwaddr);
    }

    read_len = lfs_kv_get_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 3);
    hwaddr[read_len] = 0;
    if (read_len == 0) {
        printf("read key1 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY1 "+3:%s, pass\r\n", hwaddr);
    }

    read_len = lfs_kv_get_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 100);
    hwaddr[read_len] = 0;
    if (read_len == 0) {
        printf("test case %d pass.\n", __LINE__);
    } else {
        printf(TEST_KEY1 "+100:%s, failed\r\n", hwaddr);
        while(1);
    }

    read_len = lfs_kv_get_blob("aa/bb", hwaddr, sizeof(hwaddr), NULL);
    hwaddr[read_len] = 0;
    if (read_len == 0) {
        printf("test non-exists key pass\r\n");
    } else {
        printf("aa/bb:%s, failed!\r\n", hwaddr);
        while(1);
    }

    read_len = lfs_kv_get_blob(TEST_KEY2, hwaddr, sizeof(hwaddr), NULL);
    hwaddr[read_len] = 0;
    if (read_len == 0) {
        printf("read key2 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY2 ":%s, pass\r\n", hwaddr);
    }

    printf("foreach all kv:\n");
    uint32_t count = 0;
    lfs_kv_foreach(env_foreach_cb, &count);
    printf("foreach all kv: done, total: %d\n", count);
    if (count == 4) {
        printf("lfs_kv_foreach test pass.\n");
    } else {
        printf("lfs_kv_foreach test failed.\n");
        while(1);
    }

    lfs_kv_print();
    printf("clear all kv\r\n");
    /* reset all kv */
    lfs_kv_clear();

    lfs_kv_print();
    count = 0;
    lfs_kv_foreach(env_foreach_cb, &count);
    if (count == 0) {
        printf("lfs_kv_clear test pass.\n");
    } else {
        printf("lfs_kv_clear test failed.\n");
        while(1);
    }

    printf("lfs_kv case success\r\n");
    while (1) {
    }
}
