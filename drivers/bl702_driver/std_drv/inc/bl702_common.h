#ifndef __BL702_COMMON_H__
#define __BL702_COMMON_H__

#include "bl702.h"
#include "bflb_platform.h"

/** @addtogroup  BL606_Peripheral_Driver
 *  @{
 */

/** @addtogroup  COMMON
 *  @{
 */

/** @defgroup  COMMON_Public_Types
 *  @{
 */

/*@} end of group COMMON_Public_Types */

/** @defgroup  COMMON_Public_Constants
 *  @{
 */
 
/** @defgroup DRIVER_INT_PERIPH
 *  @{
 */
#define IS_INT_PERIPH(INT_PERIPH)       ((INT_PERIPH) < IRQn_LAST)

/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK     
 *  @{
 */
#define IS_BL_MASK_TYPE(type)          (((type) == MASK) || ((type) == UNMASK))

/*@} end of group COMMON_Public_Constants */

/** @defgroup  COMMON_Public_Macros
 *  @{
 */

/*@} end of group DRIVER_Public_Macro */

/** @defgroup DRIVER_Public_FunctionDeclaration
 *  @brief DRIVER functions declaration
 *  @{
 */

#define ARCH_Delay_US                               BL702_Delay_US
#define ARCH_Delay_MS                               BL702_Delay_MS

void Interrupt_Handler_Register(IRQn_Type irq,pFunc interruptFun);
void ASM_Delay_Us(uint32_t core,uint32_t cnt);
void BL702_Delay_US(uint32_t cnt);
void BL702_Delay_MS(uint32_t cnt);
/*@} end of group DRIVER_COMMON  */

#endif /* __BL702_COMMON_H__ */


