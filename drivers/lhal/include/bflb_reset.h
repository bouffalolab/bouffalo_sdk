#ifndef _BFLB_RESET_H
#define _BFLB_RESET_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup RESET
  * @{
  */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  reset peri
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 * @return int
 */
int bflb_peripheral_reset(uint8_t peri);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif