#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_efuse.h"

bflb_efuse_device_info_type device_info;
uint8_t chip_id[8];
uint8_t sign;
uint8_t aes;

const char *aes_mode[] = {"AES-NO", "AES-128", "AES-192", "AES-256"};

int main(void)
{
    struct bflb_device_s *efuse_dev;

    board_init();
    efuse_dev = bflb_device_get_by_name("ef_ctrl");
    if (NULL == efuse_dev) {
        printf("efuse device driver not found!\r\n");
        while (1);
    }

    bflb_efuse_get_device_info(&device_info);
    bflb_efuse_get_chipid(chip_id);
    bflb_efuse_read_secure_boot(&sign, &aes);

    /* printf device_info */
    printf("DEVICE_INFO:\r\n");
    printf("    PACKAGE: %s\r\n", device_info.package_name);
#if !defined(BL602)
    printf("    PSRAM:   %s\r\n", device_info.psram_info_name);
#endif
    printf("    FLASH:   %s\r\n", device_info.flash_info_name);
#if !defined(BL702) && !defined(BL702L)
    printf("    VERSION: %d\r\n", device_info.version);
#endif
    printf("CHIP_ID:\r\n   ");
    for (int i = 0; i < sizeof(chip_id) / sizeof(chip_id[0]) - 2; i++) {
        printf(" %02X", chip_id[i]);
    }
    printf("\r\n");

    printf("SIGNATURE:\r\n    ");
    if (sign) {
        printf("ENABLE\r\n");
    } else {
        printf("DISABLE\r\n");
    }
    printf("AES_MODE:\r\n    %s\r\n", aes_mode[aes]);

#if defined(BL616)
    /* process corner */
    printf("processor corner = %d, %s\n", device_info.process_corner, device_info.process_corner_name);
#endif

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
