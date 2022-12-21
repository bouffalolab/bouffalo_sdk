#ifndef __BL616_COMMON_S_H__
#define __BL616_COMMON_S_H__

#include "bl616.h"
#include "bl616_common.h"
#include "bflb_platform.h"

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  COMMON_S
 *  @{
 */

/** @defgroup  COMMON_S_Public_Types
 *  @{
 */

/*@} end of group COMMON_S_Public_Types */

/** @defgroup  COMMON_S_Public_Constants
 *  @{
 */

/** @defgroup DRIVER_INT_PERIPH
 *  @{
 */

/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK
 *  @{
 */

/*@} end of group COMMON_S_Public_Constants */

/** @defgroup  COMMON_S_Public_Macros
 *  @{
 */

/**
 * @brief error_number Type definition
 */
typedef enum {
    COPY_SUCCESS = 0,
    PARAM_ERROR = 1,
    BUFFER_OVERLAP = 2,
    SIZE_MISMATCHING = 3,
    DST_OVERFLOW = 4,
}errno_t;

/**
 *  @brief Interrupt callback function type
 */

/*@} end of group DRIVER_Public_Macro */

/** @defgroup DRIVER_Public_FunctionDeclaration
 *  @brief DRIVER functions declaration
 *  @{
 */

errno_t memcpy_s(void *dst, size_t dstSize, const void *src, size_t count);

/*@} end of group DRIVER_COMMON_S  */

#endif /* __BL616_COMMON_S_H__ */
