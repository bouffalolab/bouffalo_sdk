/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <msp/kernel.h>
#include <xutils/debug.h>
#include "msp_port.h"
#include "bflb_gpio.h"

static uint32_t pull_type_to_lhal(uint8_t pull_type) {
    return pull_type ? GPIO_PULLDOWN : GPIO_PULLUP;
}

#if defined CONFIG_CODEC_USE_I2S && CONFIG_CODEC_USE_I2S
__attribute__((weak)) int msp_i2s_device_config(const msp_i2s_device_cfg_t *config)
{
    if (config == NULL || config->sample_rate == 0U) {
        return -1;
    }

    msp_i2s_device_init(config->sample_rate);
    return 0;
}
#endif

int msp_gpio_output_config(uint8_t pin, uint8_t pull_type)
{
    struct bflb_device_s* gpio = bflb_device_get_by_name("gpio");
    uint32_t cfg = GPIO_OUTPUT | pull_type_to_lhal(pull_type) | GPIO_SMT_EN | GPIO_DRV_0;
    bflb_gpio_init(gpio, pin, cfg);

    return 0;
}

int msp_gpio_output_set(uint8_t pin, uint8_t value)
{
    struct bflb_device_s* gpio = bflb_device_get_by_name("gpio");
    if (value) {
        bflb_gpio_set(gpio, pin);
    } else {
        bflb_gpio_reset(gpio, pin);
    }

    return 0;
}
