#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

int main(void)
{
    board_init();
    board_i2c0_gpio_init();

    struct bflb_device_s *i2c0;
    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr[2] = { 0x00, 0x00 };

    uint8_t write_data[8];
    uint8_t read_data[8];

    for (size_t i = 0; i < 8; i++) {
        write_data[i] = i;
    }

    msgs[0].addr = 0x50;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = 0x50;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data;
    msgs[1].length = 8;

    bflb_i2c_transfer(i2c0, msgs, 2);
    printf("write over\r\n");
    bflb_mtimer_delay_ms(200);

    msgs[1].addr = 0x50;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = 8;
    bflb_i2c_transfer(i2c0, msgs, 2);
    printf("read over\r\n");
    for (uint8_t i = 0; i < 8; i++) {
        if (write_data[i] != read_data[i]) {
            printf("check fail\r\n");
            while (1) {
            }
        }
    }
    while (1) {
        printf("helloworld\r\n");
        bflb_mtimer_delay_ms(2000);
    }
}
