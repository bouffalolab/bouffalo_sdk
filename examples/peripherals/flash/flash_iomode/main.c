#include "bflb_flash.h"
#include "board.h"

const char *iomode_string[] = { "NIO", "DO", "QO", "DIO", "QIO" };

#define FLASH_DATA_LEN 4096

uint8_t write_buf[FLASH_DATA_LEN];
uint8_t read_buf[FLASH_DATA_LEN];

uint64_t start_time;

int main(void)
{
    board_init();

    for (uint16_t i = 0; i < FLASH_DATA_LEN; i++) {
        write_buf[i] = i & 0xff;
    }

    for (uint8_t iomode = FLASH_IOMODE_NIO; iomode <= FLASH_IOMODE_QIO; iomode++) {
        printf("select flash iomode:%s\r\n", iomode_string[iomode]);
        bflb_flash_set_iomode(i);

        start_time = bflb_mtimer_get_time_ms();

        /* erase 0x00010000 4k flash */
        bflb_flash_erase(0x00010000, 4096);

        /* write 0x00010000 flash data */
        bflb_flash_write(0x00010000, write_buf, sizeof(write_buf));

        memset(read_buf, 0, FLASH_DATA_LEN);

        /* read 0x00010000 flash data */
        bflb_flash_read(0x00010000, read_buf, sizeof(read_buf));

        printf("cost time:%lld ms\r\n", (bflb_mtimer_get_time_ms() - start_time));

        for (i = 0; i < FLASH_DATA_LEN; i++) {
            if (read_buf[i] != (i & 0xff)) {
                printf("flash test fail at %d, expect:%d but with %d\r\n", i, (i & 0xff), read_buf[i]);
                while (1) {
                }
            }
        }
    }

    printf("flash test success\r\n");
    while (1) {
    }
}