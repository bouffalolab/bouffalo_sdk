#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_mtd.h"
#include "easyflash.h"

uint8_t test_data[] = { "1234567890" };
uint8_t read_buffer[100];

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

    memset(read_buffer, 0, sizeof(read_buffer));
    ef_port_erase(0x0, 4096);

    ef_port_write(0x0, test_data, sizeof(test_data));
    ef_port_read(0x0, read_buffer, sizeof(test_data));

    if (memcmp(read_buffer, test_data, sizeof(test_data))) {
        printf("easyflash fail\r\n");
        while (1) {
        }
    }
    printf("write data: %s\r\n", read_buffer);
    printf("easyflash ok\r\n");
    while (1) {
    }
}
