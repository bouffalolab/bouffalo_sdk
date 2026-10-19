#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

#define I2C_10BIT_SLAVE_ADDR      0x355
#define I2C_10BIT_TRANSFER_LENGTH 32

static struct bflb_device_s *i2c0;

int main(void)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr[2] = { 0x00, 0x04};
    uint8_t write_data[I2C_10BIT_TRANSFER_LENGTH];

    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

    /* Write buffer init */
    write_data[0] = 0x55;
    write_data[1] = 0x11;
    write_data[2] = 0x22;
    for (size_t i = 3; i < I2C_10BIT_TRANSFER_LENGTH; i++) {
        write_data[i] = i;
    }

    /* Write data */
    msgs[0].addr = I2C_10BIT_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP | I2C_M_TEN;
    msgs[0].buffer = subaddr;
    msgs[0].length = 2;

    msgs[1].addr = I2C_10BIT_SLAVE_ADDR;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data;
    msgs[1].length = I2C_10BIT_TRANSFER_LENGTH;

    bflb_i2c_transfer(i2c0, msgs, 2);
    printf("write over, check slave and wave\r\n");
    
    printf("end\r\n");

    while(1){
    }
}
