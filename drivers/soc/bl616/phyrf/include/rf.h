#ifndef _RF_H_
#define _RF_H_

#include <stdint.h>

/**
 ****************************************************************************************
 * @brief Radio initialization function.
 * This function is called at platform reset.
 ****************************************************************************************
 */
void rf_init(uint32_t xtalfreq_hz);


/**
 ****************************************************************************************
 * @brief Set RF channel and bandwidth.
 * This function is called to change LO and RBB settings of RF
 ****************************************************************************************
 */
void rf_set_channel(uint8_t bandwidth, uint16_t channel_freq);

/**
 ****************************************************************************************
 * @brief Dump RF registers.
 * This function is called when a fault is occurred
 ****************************************************************************************
 */
void rf_dump_status(void);

/**
 ****************************************************************************************
 * @brief RF interrupt service routine.
 * This function is called when a RF interrupt is asserted to the interrupt controller
 ****************************************************************************************
 */
void rf_lo_isr(void);

/**
 ****************************************************************************************
 * @brief RF interrupt service routine.
 * This function is called when a RF interrupt is asserted to the interrupt controller
 ****************************************************************************************
 */
void rf_clkpll_isr(void);

#endif /* _RF_H_ */
