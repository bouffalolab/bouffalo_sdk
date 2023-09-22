/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __MSP_PORT_H__
#define __MSP_PORT_H__

#include <stdint.h>

typedef struct msp_dma_ch_cfg
{
    uint8_t dma_tx_ctrl_id;
    uint8_t dma_tx_ch_id;
    uint8_t dma_rx_ctrl_id;
    uint8_t dma_rx_ch_id;
    uint8_t dma_tx_dstPeriph;
    uint8_t dma_rx_srcPeriph;
} msp_dma_ch_cfg_t;

#if !CONFIG_CODEC_USE_I2S
typedef struct msp_codec_pin_cfg
{
    uint8_t  input_negative_pin;
    uint8_t  input_positive_pin;
    uint8_t  output_negative_pin;
    uint8_t  output_positive_pin;
} msp_codec_pin_cfg_t;
#endif
// pa config
void msp_codec_pa_init_pre();       // unused
void msp_codec_pa_init();
void msp_codec_pa_before_opendac(); //unused
void msp_codec_pa_after_opendac();
void msp_codec_pa_before_closedac();
void msp_codec_pa_after_closedac(); // unused
msp_dma_ch_cfg_t msp_dma_channel_config(void);
int msp_gpio_set_func(uint8_t pin, uint8_t pull_type, uint8_t gpio_mode, uint8_t pin_func);
int msp_gpio_output_config(uint8_t pin, uint8_t pull_type);
int msp_gpio_output_set(uint8_t pin, uint8_t value);


#if CONFIG_CODEC_USE_I2S
void msp_i2s_tx_enable(void);
void msp_i2s_rx_enable(void);
void msp_i2s_device_init(uint32_t sample_rate);
#else
msp_codec_pin_cfg_t msp_codec_pin_config(void);
#endif
#if 0
typedef void (*msp_dma_irq_t)(void *p_arg, uint32_t flag);

// irq register
void msp_irq_register(int irqnum, void *handler, void *ctx);
void msp_irq_unregister(int irqnum);
void msp_irq_enable(int irqnum);
void msp_irq_disenable(int irqnum);

// mem
void *msp_malloc_align(int size, int align_bytes);
void msp_free_align(void *ptr);
uint64_t msp_gettick_count(void);

//dma
void msp_dma_chan_start(int chan);
void msp_dma_chan_stop(int chan);
void msp_dma_irq_callback_set(int chan, msp_dma_irq_t pfn, void *p_arg);
int msp_dma_chan_request(int arg);
int msp_dma_chan_release(int chan);
#endif
#endif
