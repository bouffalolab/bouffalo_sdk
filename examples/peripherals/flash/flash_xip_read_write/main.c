#include "bflb_flash.h"
#include "bflb_l1c.h"
#include "board.h"

#define FLASH_TEST_SIZE 1 * 1024 * 1024

int main(void)
{
    board_init();

    uint8_t write_buf[256];
    uint8_t *read_buf;

    for (uint16_t i = 0; i < 256; i++) {
        write_buf[i] = i;
    }

    for (uint32_t i = 1; i < (FLASH_TEST_SIZE / 1024); i++) {
        printf("test addr:%08x\r\n", (0x00010000 + (i - 1) * 1024));
        /* erase 0x00010000 4k flash */
        bflb_flash_erase(0x00010000 + (i - 1) * 1024, i * 1024);

        /* read 0x00010000 flash data */
        read_buf = (uint8_t *)(FLASH_XIP_BASE - 0x2000 + 0x00010000 + (i - 1) * 1024);
        bflb_l1c_dcache_invalidate_range(read_buf, 256);
        for (uint16_t j = 0; j < 256; j++) {
            if (read_buf[j] != 0xff) {
                printf("flash test1 fail at %d, expect:%d but with %d\r\n", j, 0xff, read_buf[j]);
                while (1) {
                }
            }
        }
        /* write 0x00010000 flash data */
        bflb_flash_write(0x00010000 + (i - 1) * 1024, write_buf, sizeof(write_buf));

        /* read 0x00010000 flash data */
        read_buf = (uint8_t *)(FLASH_XIP_BASE - 0x2000 + 0x00010000 + (i - 1) * 1024);
        bflb_l1c_dcache_invalidate_range(read_buf, 256);
        for (uint16_t j = 0; j < 256; j++) {
            if (read_buf[j] != write_buf[j]) {
                printf("flash test2 fail at %d, expect:%d but with %d\r\n", j, write_buf[j], read_buf[j]);
                while (1) {
                }
            }
        }
    }

    printf("flash test success\r\n");
    while (1) {
    }
}