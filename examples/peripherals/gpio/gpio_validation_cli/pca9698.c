#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bflb_i2c.h"
#include "pca9698.h"

#define PCA9698_REG_INPUT0 0x00
#define PCA9698_REG_OUTPUT0 0x08
#define PCA9698_REG_POLARITY0 0x10
#define PCA9698_REG_CONFIG0 0x18

static int pca9698_write_regs(struct pca9698_device *dev, uint8_t reg, const uint8_t *data, uint8_t len)
{
    struct bflb_i2c_msg_s msg;
    uint8_t buffer[1U + PCA9698_BANK_COUNT];

    if ((dev == NULL) || (dev->i2c == NULL) || (data == NULL) || (len == 0U) || (len > PCA9698_BANK_COUNT)) {
        return -1;
    }

    buffer[0] = reg;
    memcpy(&buffer[1], data, len);

    msg.addr = dev->address;
    msg.flags = I2C_M_WRITE;
    msg.buffer = buffer;
    msg.length = (uint16_t)(len + 1U);
    return bflb_i2c_transfer(dev->i2c, &msg, 1);
}

static int pca9698_read_regs(struct pca9698_device *dev, uint8_t reg, uint8_t *data, uint8_t len)
{
    struct bflb_i2c_msg_s msgs[2];

    if ((dev == NULL) || (dev->i2c == NULL) || (data == NULL) || (len == 0U) || (len > PCA9698_BANK_COUNT)) {
        return -1;
    }

    msgs[0].addr = dev->address;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg;
    msgs[0].length = 1;
    msgs[1].addr = dev->address;
    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = data;
    msgs[1].length = len;
    return bflb_i2c_transfer(dev->i2c, msgs, 2);
}

int pca9698_get_bank_bit(uint8_t pin, uint8_t *bank, uint8_t *bit)
{
    if ((pin >= PCA9698_PIN_COUNT) || (bank == NULL) || (bit == NULL)) {
        return -1;
    }

    *bank = (uint8_t)(pin / 8U);
    *bit = (uint8_t)(pin % 8U);
    return 0;
}

int pca9698_init(struct pca9698_device *dev, struct bflb_device_s *i2c, uint8_t address)
{
    uint8_t zeroes[PCA9698_BANK_COUNT] = { 0 };
    uint8_t inputs[PCA9698_BANK_COUNT];

    if ((dev == NULL) || (i2c == NULL)) {
        return -1;
    }

    memset(dev, 0, sizeof(*dev));
    dev->i2c = i2c;
    dev->address = address;
    memset(dev->output_cache, 0x00, sizeof(dev->output_cache));
    memset(dev->config_cache, 0xFF, sizeof(dev->config_cache));
    memset(inputs, 0xFF, sizeof(inputs));

    if (pca9698_write_regs(dev, PCA9698_REG_POLARITY0, zeroes, PCA9698_BANK_COUNT) != 0) {
        return -1;
    }
    if (pca9698_write_regs(dev, PCA9698_REG_OUTPUT0, dev->output_cache, PCA9698_BANK_COUNT) != 0) {
        return -1;
    }
    if (pca9698_write_regs(dev, PCA9698_REG_CONFIG0, inputs, PCA9698_BANK_COUNT) != 0) {
        return -1;
    }

    return 0;
}

int pca9698_set_direction(struct pca9698_device *dev, uint8_t pin, bool input)
{
    uint8_t bank;
    uint8_t bit;
    uint8_t value;

    if ((dev == NULL) || (pca9698_get_bank_bit(pin, &bank, &bit) != 0)) {
        return -1;
    }

    value = dev->config_cache[bank];
    if (input) {
        value |= (uint8_t)(1U << bit);
    } else {
        value &= (uint8_t)~(1U << bit);
    }

    if (pca9698_write_regs(dev, (uint8_t)(PCA9698_REG_CONFIG0 + bank), &value, 1) != 0) {
        return -1;
    }

    dev->config_cache[bank] = value;
    return 0;
}

int pca9698_write_pin(struct pca9698_device *dev, uint8_t pin, bool high)
{
    uint8_t bank;
    uint8_t bit;
    uint8_t value;

    if ((dev == NULL) || (pca9698_get_bank_bit(pin, &bank, &bit) != 0)) {
        return -1;
    }

    value = dev->output_cache[bank];
    if (high) {
        value |= (uint8_t)(1U << bit);
    } else {
        value &= (uint8_t)~(1U << bit);
    }

    if (pca9698_write_regs(dev, (uint8_t)(PCA9698_REG_OUTPUT0 + bank), &value, 1) != 0) {
        return -1;
    }

    dev->output_cache[bank] = value;
    return 0;
}

int pca9698_read_pin(struct pca9698_device *dev, uint8_t pin, bool *high)
{
    uint8_t bank;
    uint8_t bit;
    uint8_t value;

    if ((dev == NULL) || (high == NULL) || (pca9698_get_bank_bit(pin, &bank, &bit) != 0)) {
        return -1;
    }

    if (pca9698_read_regs(dev, (uint8_t)(PCA9698_REG_INPUT0 + bank), &value, 1) != 0) {
        return -1;
    }

    *high = ((value >> bit) & 0x1U) != 0U;
    return 0;
}

int pca9698_deinit_pin(struct pca9698_device *dev, uint8_t pin)
{
    return pca9698_set_direction(dev, pin, true);
}
