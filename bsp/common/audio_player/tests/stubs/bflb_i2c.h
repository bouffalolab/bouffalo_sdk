#ifndef TEST_BFLB_I2C_H
#define TEST_BFLB_I2C_H
#include <stdint.h>
struct bflb_device_s { int unused; };
struct bflb_i2c_msg_s {
    uint16_t addr, flags;
    uint8_t *buffer;
    uint16_t length;
};
#define I2C_M_NOSTOP 0x02
#define I2C_M_READ 0x01
int bflb_i2c_transfer(struct bflb_device_s *, struct bflb_i2c_msg_s *, int);
#endif
