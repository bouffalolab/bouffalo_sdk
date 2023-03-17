#ifndef PIKA_HAL_BLMCU_COMMON_H
#define PIKA_HAL_BLMCU_COMMON_H

typedef struct platform_gpio_t {
    struct bflb_device_s* device;
    uint8_t pin;
    uint32_t config;
} platform_gpio_t;

#endif
