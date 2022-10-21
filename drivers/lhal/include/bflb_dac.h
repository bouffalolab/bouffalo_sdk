#ifndef _BFLB_DAC_H
#define _BFLB_DAC_H

#include "bflb_core.h"

/** @defgroup DAC_SAMPLING_FREQ dac sampling frequence definition
  * @{
  */
#define DAC_SAMPLING_FREQ_32K  0
#define DAC_SAMPLING_FREQ_16K  1
#define DAC_SAMPLING_FREQ_8K   3
#define DAC_SAMPLING_FREQ_512K 4
/**
  * @}
  */

/** @defgroup DAC_CHANNEL dac channel select
  * @{
  */
#define DAC_CHANNEL_A (1 << 0)
#define DAC_CHANNEL_B (1 << 1)
/**
  * @}
  */

#ifdef __cplusplus
extern "C" {
#endif

void bflb_dac_init(struct bflb_device_s *dev, uint8_t frequence);
void bflb_dac_deinit(struct bflb_device_s *dev);
void bflb_dac_link_txdma(struct bflb_device_s *dev, bool enable);
void bflb_dac_channel_enable(struct bflb_device_s *dev, uint8_t ch);
void bflb_dac_channel_disable(struct bflb_device_s *dev, uint8_t ch);
void bflb_dac_set_value(struct bflb_device_s *dev, uint8_t ch, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif