#include "bflb_flash.h"
#include "board.h"


#define FLASH_DATA_LEN 4096
#define FLASH_RW_START_ADDR  0x10000

const char *iomode_string[] = { "NIO", "DO", "QO", "DIO", "QIO" };
static uint8_t write_buf[FLASH_DATA_LEN];
static uint8_t read_buf[FLASH_DATA_LEN];
static uint64_t start_time;

int main(void)
{
    uint32_t i;

    board_init();

    for (i = 0; i < sizeof(write_buf); i++) {
        write_buf[i] = i & 0xff;
    }

    for (uint8_t iomode = FLASH_IOMODE_NIO; iomode <= FLASH_IOMODE_QIO; iomode++) {

        printf("select flash iomode:%s\r\n", iomode_string[iomode]);
        bflb_flash_set_iomode(iomode);

        start_time = bflb_mtimer_get_time_ms();

        /* erase  flash */
        bflb_flash_erase(FLASH_RW_START_ADDR, sizeof(write_buf));

        memset(read_buf, 0, sizeof(read_buf));
        /* read flash data */
        bflb_flash_read(FLASH_RW_START_ADDR, read_buf, sizeof(read_buf));

        for (i = 0; i < sizeof(read_buf); i++) {
            if (read_buf[i] != 0xff) {
                printf("flash erase fail at %d, expect:%d but get %d\r\n", i, 0xff, read_buf[i]);
                while (1) {
                }
            }
        }

        /* write flash data */
        bflb_flash_write(FLASH_RW_START_ADDR, write_buf, sizeof(write_buf));

        memset(read_buf, 0, sizeof(read_buf));
        /* read flash data */
        bflb_flash_read(FLASH_RW_START_ADDR, read_buf, sizeof(read_buf));

        printf("cost time:%lld ms\r\n", (bflb_mtimer_get_time_ms() - start_time));

        for (i = 0; i < sizeof(write_buf); i++) {
            if (read_buf[i] != write_buf[i]) {
                printf("flash write fail at %d, expect:%d but get %d\r\n", i, write_buf[i], read_buf[i]);
                while (1) {
                }
            }
        }
        printf("flash data check success\r\n");
    }

    printf("flash io case success\r\n");
    while (1) {
    }
}