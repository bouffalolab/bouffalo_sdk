#ifndef GPIO_VALIDATION_CLI_PCA9698_H
#define GPIO_VALIDATION_CLI_PCA9698_H

#include <stdbool.h>
#include <stdint.h>

#include "bflb_core.h"

#define PCA9698_BANK_COUNT 5U
#define PCA9698_PIN_COUNT 40U

struct pca9698_device {
    struct bflb_device_s *i2c;
    uint8_t address;
    uint8_t output_cache[PCA9698_BANK_COUNT];
    uint8_t config_cache[PCA9698_BANK_COUNT];
};

int pca9698_init(struct pca9698_device *dev, struct bflb_device_s *i2c, uint8_t address);
int pca9698_set_direction(struct pca9698_device *dev, uint8_t pin, bool input);
int pca9698_write_pin(struct pca9698_device *dev, uint8_t pin, bool high);
int pca9698_read_pin(struct pca9698_device *dev, uint8_t pin, bool *high);
int pca9698_deinit_pin(struct pca9698_device *dev, uint8_t pin);
int pca9698_get_bank_bit(uint8_t pin, uint8_t *bank, uint8_t *bit);

#endif
