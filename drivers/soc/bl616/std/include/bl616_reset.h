/**
  ******************************************************************************
  * @file    bl616_reset.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  */
#ifndef __BL616_RESET_H__
#define __BL616_RESET_H__

#include "bflb_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  RESET
 *  @{
 */

/** @defgroup  RESET_Public_Functions
 *  @{
 */


/**
 * @brief  reset peri by id
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 * @return int
 */
int bflb_peripheral_reset_by_id(uint8_t peri);


/*@} end of group RESET_Public_Functions */

/*@} end of group RESET */

/*@} end of group BL616_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616_RESET_H__ */
