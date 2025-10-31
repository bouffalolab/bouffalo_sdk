#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_mtd.h"
#include "easyflash.h"

uint8_t test_data[] = { "1234567890" };
uint8_t read_buffer[100];

#define WIFI_SSID_KEY   "wifi.ssid"
#define WIFI_PASSWD_KEY "wifi.passwd"
#define TEST_KEY1       "g/hwaddr/mac_aabb"
#define TEST_KEY2       "/root/aa/bbb/"

static EfErrCode env_foreach_cb(const char *name, void *arg) {
  uint32_t *count = (uint32_t *)arg;
  printf("foreach key %d: %s\n", (*count)++, name);
  return EF_NO_ERR;
}

int main(void)
{
    EfErrCode ret;
    board_init();

    /* Partition and boot2 must be use, and we can only operate partition **psm** with easyflash
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
    if (easyflash_init() == EF_NO_ERR) {
        printf("easyflash_init test pass.\n");
    } else {
        printf("errno: %d\r\n", errno);
        printf("easyflash_init test failed.\n");
    }

    memset(read_buffer, 0, sizeof(read_buffer));

    ret = ef_set_and_save_env(WIFI_SSID_KEY, (const char *)"helloworld");
    if (ret != EF_NO_ERR) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = ef_set_and_save_env(WIFI_PASSWD_KEY, (const char *)"helloworld2023");
    if (ret != EF_NO_ERR) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = ef_set_and_save_env(TEST_KEY1, (const char *)"11223344");
    if (ret != EF_NO_ERR) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = ef_set_and_save_env(TEST_KEY2, (const char *)"deadbeef");
    if (ret != EF_NO_ERR) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    ret = ef_save_env();
    if (ret != EF_NO_ERR) {
      printf("test case %d failed.\n", __LINE__);
      while(1);
    }

    char ssid[33];
    char passwd[65];
    char hwaddr[33];

    if (ef_get_env(WIFI_SSID_KEY) != NULL) {
        ret = ef_get_env_blob(WIFI_SSID_KEY, ssid, sizeof(ssid), NULL);
        ssid[ret] = 0;
        printf("ssid:%s, test pass.\r\n", ssid);
    } else {
        printf("test case %d failed.\n", __LINE__);
        while(1);
    }

    if (ef_get_env(WIFI_PASSWD_KEY) != NULL) {
        ret = ef_get_env_blob(WIFI_PASSWD_KEY, passwd, sizeof(passwd), NULL);
        passwd[ret] = 0;
        printf("passwd:%s test pass.\r\n", passwd);
    } else {
        printf("test case %d failed.\n", __LINE__);
        while(1);
    }

    ret = ef_get_env_blob(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL);
    hwaddr[ret] = 0;
    if (ret == 0) {
        printf("read key1 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY1 ":%s, pass\r\n", hwaddr);
    }

    ret = ef_get_env_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 2);
    hwaddr[ret] = 0;
    if (ret == 0) {
        printf("read key1 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY1 "+2:%s, pass\r\n", hwaddr);
    }

    ret = ef_get_env_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 3);
    hwaddr[ret] = 0;
    if (ret == 0) {
        printf("read key1 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY1 "+3:%s, pass\r\n", hwaddr);
    }

    ret = ef_get_env_blob_offset(TEST_KEY1, hwaddr, sizeof(hwaddr), NULL, 100);
    hwaddr[ret] = 0;
    if (ret == 0) {
        printf("test case %d pass.\n", __LINE__);
    } else {
        printf(TEST_KEY1 "+100:%s, failed\r\n", hwaddr);
        while(1);
    }

    ret = ef_get_env_blob("aa/bb", hwaddr, sizeof(hwaddr), NULL);
    hwaddr[ret] = 0;
    if (ret == 0) {
        printf("test non-exists key pass\r\n");
    } else {
        printf("aa/bb:%s, failed!\r\n", hwaddr);
        while(1);
    }

    ret = ef_get_env_blob(TEST_KEY2, hwaddr, sizeof(hwaddr), NULL);
    hwaddr[ret] = 0;
    if (ret == 0) {
        printf("read key2 failed\r\n");
        while(1);
    } else {
        printf(TEST_KEY2 ":%s, pass\r\n", hwaddr);
    }

    printf("foreach all env:\n");
    uint32_t count = 0;
    ef_foreach_env(env_foreach_cb, &count);
    printf("foreach all env: done, total: %d\n", count);
    if (count == 4) {
        printf("ef_foreach_env test pass.\n");
    } else {
        printf("ef_foreach_env test failed.\n");
        while(1);
    }

    ef_print_env();
    printf("clear all kv\r\n");
    /* reset all kv */
    ef_env_set_default();

    ef_print_env();
    count = 0;
    ef_foreach_env(env_foreach_cb, &count);
    if (count == 0) {
        printf("ef_env_set_default test pass.\n");
    } else {
        printf("ef_env_set_default test failed.\n");
        while(1);
    }

    printf("easyflash case success\r\n");
    while (1) {
    }
}
