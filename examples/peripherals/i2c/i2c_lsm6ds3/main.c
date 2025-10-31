#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

#define I2C_LSM6DS3_DEVICE_ADDR (0x6A)
#define I2C_LSM6DS3_BUFFER_SIZE (16)

static struct bflb_device_s *i2c0;
static uint8_t write_data[I2C_LSM6DS3_BUFFER_SIZE];
static uint8_t read_data[I2C_LSM6DS3_BUFFER_SIZE];

int main(void)
{
    struct bflb_i2c_msg_s msgs[2];
    uint8_t subaddr;

    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c0, 400 * 1000);

    /* Read ID */
    subaddr = 0x0F;
    msgs[0].addr = I2C_LSM6DS3_DEVICE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &subaddr;
    msgs[0].length = 1;
    msgs[1].addr = I2C_LSM6DS3_DEVICE_ADDR;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = 1;
    read_data[0] = 0;
    bflb_i2c_transfer(i2c0, msgs, 2);
    /* LSM6DS3TR-C id: 0x6A, LSM6DS33TR id: 0x69 */
    if ((read_data[0] != 0x6A) && (read_data[0] != 0x69)) {
        printf("I2C LSM6DS3 error\r\n");
        return;
    }

    /* config XL and G output by 104Hz */
    subaddr = 0x10;
    write_data[0] = 0x40;
    write_data[1] = 0x40;
    msgs[0].addr = I2C_LSM6DS3_DEVICE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &subaddr;
    msgs[0].length = 1;
    msgs[1].addr = I2C_LSM6DS3_DEVICE_ADDR;
    msgs[1].flags = 0;
    msgs[1].buffer = write_data;
    msgs[1].length = 2;
    bflb_i2c_transfer(i2c0, msgs, 2);

    /* Read output data */
    subaddr = 0x20;
    msgs[0].addr = I2C_LSM6DS3_DEVICE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &subaddr;
    msgs[0].length = 1;
    msgs[1].addr = I2C_LSM6DS3_DEVICE_ADDR;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = read_data;
    msgs[1].length = 14;
    while (1) {
        int16_t *p = (int16_t *)&read_data[0];
        bflb_i2c_transfer(i2c0, msgs, 2);
        printf("LSM6DS3:Temp=%.2f, GX=%d, GY=%d, GZ=%d, AX=%d, AY=%d, AZ=%d\r\n", (float)p[0] / 256.0f + 25.0f, p[1], p[2], p[3], p[4], p[5], p[6]);
        bflb_mtimer_delay_ms(500);
    }

    return 0;
}
