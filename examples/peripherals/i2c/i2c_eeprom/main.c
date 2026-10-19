#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

#define EEPROM_TRANSFER_LENGTH 32
#define EEPROM_SELECT_PAGE0    (0 << 5)

static struct bflb_device_s *i2c0;

int main(void)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr[2] = { 0x00, EEPROM_SELECT_PAGE0};
    uint8_t write_data[256];
    uint8_t read_data[256];

    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

    /* Write and read buffer init */
    for (size_t i = 0; i < 256; i++) {
        write_data[i] = i;
        read_data[i] = 0;
    }

    /* Write page 0 */
    msgs[0].addr = 0x50;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = 0x50;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;

    bflb_i2c_transfer(i2c0, msgs, 2);
    printf("write over\r\n");
    bflb_mtimer_delay_ms(100);

    /* Read page 0 */
    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = EEPROM_TRANSFER_LENGTH;
    bflb_i2c_transfer(i2c0, msgs, 2);
    printf("read over\r\n");
    bflb_mtimer_delay_ms(100);

    /* Check read data */
    for (uint8_t i = 0; i < EEPROM_TRANSFER_LENGTH; i++) {
        if (write_data[i] != read_data[i]) {
            printf("check fail, %d write: %02x, read: %02x\r\n", i, write_data[i], read_data[i]);
        }
    }
    printf("check over\r\n");
    printf("end\r\n");

    while(1){
    }
}
