#include "bflb_flash.h"
#include "board.h"

#define FLASH_RW_TOTAL_SIZE (10 * 4096)
#define FLASH_RW_START_ADDR 0x10000

static uint8_t write_buf[4096];
static uint8_t read_buf[4096];

int main(void)
{
    uint32_t i, j;
    uint32_t flash_addr;

    board_init();

    for (i = 0; i < sizeof(write_buf); i++) {
        write_buf[i] = i;
    }

    for (uint32_t offset = 0; offset < FLASH_RW_TOTAL_SIZE; offset += sizeof(write_buf)) {
        flash_addr = FLASH_RW_START_ADDR + offset;
        printf("erase addr:%08x\r\n", flash_addr);

        /* erase flash */
        bflb_flash_erase(flash_addr, sizeof(write_buf));

        memset(read_buf, 0, sizeof(read_buf));
        /* read flash data */
        bflb_flash_read(flash_addr, read_buf, sizeof(read_buf));

        for (j = 0; j < sizeof(read_buf); j++) {
            if (read_buf[j] != 0xff) {
                printf("flash erase fail at %d, expect:%d but get %d\r\n", j, 0xff, read_buf[j]);
                while (1) {
                }
            }
        }

        /* write flash data */
        bflb_flash_write(flash_addr, write_buf, sizeof(write_buf));

        memset(read_buf, 0, sizeof(read_buf));
        /* read flash data */
        bflb_flash_read(flash_addr, read_buf, sizeof(read_buf));

        for (j = 0; j < sizeof(read_buf); j++) {
            if (read_buf[j] != write_buf[j]) {
                printf("flash read fail at %d, expect:%d but get %d\r\n", j, write_buf[j], read_buf[j]);
                while (1) {
                }
            }
        }
        printf("check success\r\n");
    }

    printf("flash read write success\r\n");

    while (1) {
    }
}