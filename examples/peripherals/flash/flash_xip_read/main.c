#include "bflb_flash.h"
#include "bflb_l1c.h"
#include "board.h"

#define FLASH_RW_TOTAL_SIZE  512 * 1024
#define FLASH_RW_START_ADDR  0x10000

static uint8_t write_buf[1024];

int main(void)
{
    uint8_t *read_buf;
    uint32_t flash_addr;

    board_init();

    for (uint16_t i = 0; i < sizeof(write_buf); i++) {
        write_buf[i] = i;
    }

    for (uint32_t offset = 0; offset < FLASH_RW_TOTAL_SIZE; offset+=sizeof(write_buf)) {

        flash_addr=FLASH_RW_START_ADDR + offset;
        printf("erase addr:%08x\r\n", flash_addr);

        /* erase flash */
        bflb_flash_erase(flash_addr , sizeof(write_buf));

        /* read flash data */
        read_buf = (uint8_t *)(FLASH_XIP_BASE + flash_addr - bflb_flash_get_image_offset());
        bflb_l1c_dcache_invalidate_range(read_buf, sizeof(write_buf));
        for (uint16_t j = 0; j < sizeof(write_buf); j++) {
            if (read_buf[j] != 0xff) {
                printf("flash erase fail at %d, expect:%d but get %d\r\n", j, 0xff, read_buf[j]);
                while (1) {
                }
            }
        }
        /* write flash data */
        bflb_flash_write(flash_addr, write_buf, sizeof(write_buf));

        /* read flash data */
        bflb_l1c_dcache_invalidate_range(read_buf, sizeof(write_buf));
        for (uint16_t j = 0; j < sizeof(write_buf); j++) {
            if (read_buf[j] != write_buf[j]) {
                printf("flash check fail at %d, expect:%d but get %d\r\n", j, write_buf[j], read_buf[j]);
                while (1) {
                }
            }
        }
        printf("check success\r\n");
    }

    printf("flash xip read success\r\n");

    while (1) {
    }
}