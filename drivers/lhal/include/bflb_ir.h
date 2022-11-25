#ifndef _BFLB_IR_H
#define _BFLB_IR_H

#include "bflb_core.h"

#if !defined(BL616)
/** @defgroup IR TX mode definition
  * @{
  */
#define IR_TX_NEC       0
#define IR_TX_RC5       1
#define IR_TX_SWM       2
#define IR_TX_CUSTOMIZE 3
/**
  * @}
  */

/** @defgroup IR TX FIFO valid width definition
  * @{
  */
#if !defined(BL602) && !defined(BL702)
#define IR_TX_FIFO_WIDTH_8BIT  0
#define IR_TX_FIFO_WIDTH_16BIT 1
#define IR_TX_FIFO_WIDTH_24BIT 2
#endif
#define IR_TX_FIFO_WIDTH_32BIT 3
/**
  * @}
  */

/** @defgroup IR TX interrupt type definition
  * @{
  */
#define IR_TX_INT_END  (1 << 0)
#if !defined(BL602) && !defined(BL702)
#define IR_TX_INT_FIFO (1 << 1)
#define IR_TX_INT_FER  (1 << 2)
#endif
/**
  * @}
  */
#endif

#if !defined(BL702L)
/** @defgroup IR RX mode definition
  * @{
  */
#define IR_RX_NEC 0
#define IR_RX_RC5 1
#define IR_RX_SWM 2
/**
  * @}
  */

/** @defgroup IR RX interrupt type definition
  * @{
  */
#define IR_RX_INT_END  (1 << 0)
#if !defined(BL602) && !defined(BL702)
#define IR_RX_INT_FIFO (1 << 1)
#define IR_RX_INT_FER  (1 << 2)
#endif
/**
  * @}
  */
#endif

#if !defined(BL616)
/**
 * @brief IR TX configuration structure
 *
 * @param tx_mode              TX mode select,NEC protocol/RC-5 protocol/software mode/customize
 * @param data_bits            Bit count of data phase (don't care if tx freerun mode is enabled)
 * @param tail_inverse         Enable or disable signal of tail pulse inverse (don't care if SWM is enabled)
 * @param tail_enable          Enable or disable signal of tail pulse (don't care if SWM is enabled)
 * @param head_inverse         Enable or disable signal of head pulse inverse (don't care if SWM is enabled)
 * @param head_enable          Enable or disable signal of head pulse (don't care if SWM is enabled)
 * @param logic1_inverse       Enable or disable signal of logic 1 pulse inverse (don't care if SWM is enabled)
 * @param logic0_inverse       Enable or disable signal of logic 0 pulse inverse (don't care if SWM is enabled)
 * @param data_enable          Enable or disable signal of data pulse (don't care if SWM is enabled)
 * @param swm_enable           Enable or disable software mode(SWM)
 * @param output_modulation    Enable or disable signal of output modulation
 * @param output_inverse       Enable or disable signal of output inverse,0:output stays at low during idle state,1:stay at high
 * @param freerun_enable       Enable or disable tx freerun mode (don't care if SWM is enabled)
 * @param continue_enable      Disable:idle time between frames = (tailPulseWidth_0+tailPulseWidth_1)*pulseWidthUnit,Enable:no idle time between frames
 * @param fifo_width           IR frame size(also the valid width for each fifo entry)
 * @param fifo_threshold       TX FIFO threshold
 * @param logic0_pulse_width_1 Pulse width of logic 0 pulse phase 1 (don't care if SWM is enabled)
 * @param logic0_pulse_width_0 Pulse width of logic 0 pulse phase 0 (don't care if SWM is enabled)
 * @param logic1_pulse_width_1 Pulse width of logic 1 pulse phase 1 (don't care if SWM is enabled)
 * @param logic1_pulse_width_0 Pulse width of logic 1 pulse phase 0 (don't care if SWM is enabled)
 * @param head_pulse_width_1   Pulse width of head pulse phase 1 (don't care if SWM is enabled)
 * @param head_pulse_width_0   Pulse width of head pulse phase 0 (don't care if SWM is enabled)
 * @param tail_pulse_width_1   Pulse width of tail pulse phase 1 (don't care if SWM is enabled)
 * @param tail_pulse_width_0   Pulse width of tail pulse phase 0 (don't care if SWM is enabled)
 * @param modu_width_1         Modulation phase 1 width
 * @param modu_width_0         Modulation phase 0 width
 * @param pulse_width_unit     Pulse width unit
 */
struct bflb_ir_tx_config_s {
    uint8_t tx_mode;
    uint8_t data_bits;
    uint8_t tail_inverse;
    uint8_t tail_enable;
    uint8_t head_inverse;
    uint8_t head_enable;
    uint8_t logic1_inverse;
    uint8_t logic0_inverse;
    uint8_t data_enable;
    uint8_t swm_enable;
    uint8_t output_modulation;
    uint8_t output_inverse;
    uint8_t freerun_enable;
    uint8_t continue_enable;
    uint8_t fifo_width;
    uint8_t fifo_threshold;
    uint8_t logic0_pulse_width_1;
    uint8_t logic0_pulse_width_0;
    uint8_t logic1_pulse_width_1;
    uint8_t logic1_pulse_width_0;
    uint8_t head_pulse_width_1;
    uint8_t head_pulse_width_0;
    uint8_t tail_pulse_width_1;
    uint8_t tail_pulse_width_0;
    uint8_t modu_width_1;
    uint8_t modu_width_0;
    uint16_t pulse_width_unit;
};
#endif

#if !defined(BL702L)
/**
 * @brief IR RX configuration structure
 *
 * @param rx_mode         RX mode select,NEC protocol/RC-5 protocol/software mode
 * @param input_inverse   Enable or disable signal of input inverse
 * @param deglitch_enable Enable or disable signal of rx input de-glitch function
 * @param deglitch_cnt    De-glitch function cycle count
 * @param  data_threshold Pulse width threshold for logic 0/1 detection (don't care if SWM is enabled)
 * @param  end_threshold  Pulse width threshold to trigger end condition
 * @param  fifo_threshold RX FIFO threshold
 */
struct bflb_ir_rx_config_s {
    uint8_t rx_mode;
    uint8_t input_inverse;
    uint8_t deglitch_enable;
    uint8_t deglitch_cnt;
    uint16_t data_threshold;
    uint16_t end_threshold;
    uint16_t fifo_threshold;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(BL616)
void bflb_ir_tx_init(struct bflb_device_s *dev, const struct bflb_ir_tx_config_s *config);
void bflb_ir_send(struct bflb_device_s *dev, uint32_t *data, uint32_t length);
void bflb_ir_swm_send(struct bflb_device_s *dev, uint16_t *data, uint8_t length);
void bflb_ir_tx_enable(struct bflb_device_s *dev, bool enable);
void bflb_ir_txint_mask(struct bflb_device_s *dev, uint8_t int_type, bool mask);
void bflb_ir_txint_clear(struct bflb_device_s *dev);
uint32_t bflb_ir_txint_status(struct bflb_device_s *dev);
#if !defined(BL602) && !defined(BL702)
void bflb_ir_link_txdma(struct bflb_device_s *dev, bool enable);
uint8_t bflb_ir_txfifo_cnt(struct bflb_device_s *dev);
void bflb_ir_txfifo_clear(struct bflb_device_s *dev);
#endif
#endif

#if !defined(BL702L)
void bflb_ir_rx_init(struct bflb_device_s *dev, const struct bflb_ir_rx_config_s *config);
uint8_t bflb_ir_receive(struct bflb_device_s *dev, uint64_t *data);
uint8_t bflb_ir_swm_receive(struct bflb_device_s *dev, uint16_t *data, uint8_t length);
void bflb_ir_rx_enable(struct bflb_device_s *dev, bool enable);
void bflb_ir_rxint_mask(struct bflb_device_s *dev, uint8_t int_type, bool mask);
void bflb_ir_rxint_clear(struct bflb_device_s *dev);
uint32_t bflb_ir_rxint_status(struct bflb_device_s *dev);
uint8_t bflb_ir_rxfifo_cnt(struct bflb_device_s *dev);
void bflb_ir_rxfifo_clear(struct bflb_device_s *dev);
#endif

void bflb_ir_feature_control();

#ifdef __cplusplus
}
#endif

#endif
