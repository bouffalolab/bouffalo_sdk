/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __MSP_PORT_H__
#define __MSP_PORT_H__

#include <stdint.h>

typedef struct msp_dma_ch_cfg {
    uint8_t dma_tx_ctrl_id;
    uint8_t dma_tx_ch_id;
    uint8_t dma_rx_ctrl_id;
    uint8_t dma_rx_ch_id;
    uint8_t dma_tx_dstPeriph;
    uint8_t dma_rx_srcPeriph;
} msp_dma_ch_cfg_t;

#if !CONFIG_CODEC_USE_I2S
typedef struct msp_codec_pin_cfg {
    uint8_t input_negative_pin;
    uint8_t input_positive_pin;
    uint8_t output_negative_pin;
    uint8_t output_positive_pin;
} msp_codec_pin_cfg_t;
#endif
// pa config
void msp_codec_pa_init_pre();
void msp_codec_pa_init();
void msp_codec_pa_before_opendac();
void msp_codec_pa_after_opendac();
void msp_codec_pa_before_closedac();
void msp_codec_pa_after_closedac();
msp_dma_ch_cfg_t msp_dma_channel_config(void);
int msp_gpio_output_config(uint8_t pin, uint8_t pull_type);
int msp_gpio_output_set(uint8_t pin, uint8_t value);

#if CONFIG_CODEC_USE_I2S
void msp_i2s_tx_enable(void);
void msp_i2s_rx_enable(void);
void msp_i2s_device_init(uint32_t sample_rate);
void msp_i2s_port_init(void);
#else
msp_codec_pin_cfg_t msp_codec_pin_config(void);
#endif
#endif
