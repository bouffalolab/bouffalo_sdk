/* SPDX-License-Identifier: Apache-2.0 */
#ifndef TEST_BFLB_CLOCK_H
#define TEST_BFLB_CLOCK_H

#include <stdint.h>

#define BFLB_DEVICE_TYPE_I2S 9
uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t index);

#endif
