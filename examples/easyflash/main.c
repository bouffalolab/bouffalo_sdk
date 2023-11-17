#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_mtd.h"
#include "easyflash.h"

uint8_t test_data[] = { "1234567890" };
uint8_t read_buffer[100];

#define WIFI_SSID_KEY   "wifi.ssid"
#define WIFI_PASSWD_KEY "wifi.passwd"

int main(void)
{
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
    easyflash_init();
    printf("errno: %d\r\n", errno);

    memset(read_buffer, 0, sizeof(read_buffer));

    printf("ef set env\r\n");
    ef_set_and_save_env(WIFI_SSID_KEY, (const char *)"helloworld");
    ef_set_and_save_env(WIFI_PASSWD_KEY, (const char *)"helloworld2023");

    ef_save_env();

    char ssid[33];
    char passwd[65];
    int ret;

    printf("ef get env\r\n");
    if (ef_get_env(WIFI_SSID_KEY) != NULL) {
        ret = ef_get_env_blob(WIFI_SSID_KEY, ssid, sizeof(ssid), NULL);
        ssid[ret] = 0;
        printf("ssid:%s\r\n", ssid);
    }
    if (ef_get_env(WIFI_PASSWD_KEY) != NULL) {
        ret = ef_get_env_blob(WIFI_PASSWD_KEY, passwd, sizeof(passwd), NULL);
        passwd[ret] = 0;
        printf("passwd:%s\r\n", passwd);
    }

    printf("easyflash case success\r\n");
    while (1) {
    }
}
