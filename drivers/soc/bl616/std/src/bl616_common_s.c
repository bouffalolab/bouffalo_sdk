#include "bl616_common_s.h"
#include <string.h>
#include "bflb_platform.h"
#include "bl616_glb.h"
#include "bl616_dma.h"
#include "bl616_i2s.h"
#include "bl616_i2c.h"
#include "bl616_uart.h"
#include "bl616_clock.h"

/** @addtogroup  BL616_Periph_Driver
 *  @{
 */

/** @defgroup DRIVER_COMMON_S DRIVER_COMMON_S
 *  @brief Digger driver common_s functions
 *  @{
 */

/** @defgroup DRIVER_Private_Type
 *  @{
 */

/*@} end of group DRIVER_Private_Type*/

/** @defgroup DRIVER_Private_Defines
 *  @{
 */

/*@} end of group DRIVER_Private_Defines */

/** @defgroup DRIVER_Private_Variables
 *  @{
 */

/*@} end of group DRIVER_Private_Variables */

/** @defgroup DRIVER_Global_Variables
 *  @{
 */

/*@} end of group DRIVER_Global_Variables */

/** @defgroup DRIVER_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DRIVER_Private_FunctionDeclaration */

/** @defgroup DRIVER_Private_Functions
 *  @{
 */

/*@} end of group DRIVER_Private_Functions */

/** @defgroup DRIVER_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Char memcpy safe
 *
 * @param  dst: Destination
 * @param  dstSize: Destination size
 * @param  src: Source
 * @param  count: Count of char
 *
 * @return Destination pointer
 *
 *******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
static inline uint8_t ATTR_TCM_SECTION is_mem_in_ram(uint32_t addr, size_t size)
{
    /* bl616 all ram     */
    /* 0x20FC0000 + 480K */
    /* 0x60FC0000 + 480K */
    /* 0x22FC0000 + 480K */
    /* 0x62FC0000 + 480K */
    if((((BL616_ALLRAM_BASE<=addr)&&((addr+size)<=BL616_ALLRAM_END))||
        ((BL616_ALLRAM_CACHEABLE_BASE<=addr)&&((addr+size)<=BL616_ALLRAM_CACHEABLE_END))||
        ((BL616_ALLRAM_BUSREMAP_BASE<=addr)&&((addr+size)<=BL616_ALLRAM_BUSREMAP_END))||
        ((BL616_ALLRAM_BUSREMAP_CACHEABLE_BASE<=addr)&&((addr+size)<=BL616_ALLRAM_BUSREMAP_CACHEABLE_END)))){
        return 1;
    }
    
    return 0;
}
#endif

/****************************************************************************/ /**
 * @brief  Char memcpy safe
 *
 * @param  dst: Destination
 * @param  dstSize: Destination size
 * @param  src: Source
 * @param  count: Count of char
 *
 * @return Destination pointer
 *
 *******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
errno_t ATTR_TCM_SECTION memcpy_s(void *dst, size_t dstSize, const void *src, size_t count)
{
    /* param NULL */
    if((NULL==dst)||(NULL==src)){
        return PARAM_ERROR;
    }
    
    /* memory overlap */
    if(!((((uint32_t)src + count)<(uint32_t)dst)||((uint32_t)src>((uint32_t)dst + dstSize)))){
        return BUFFER_OVERLAP;
    }
    
    /* size mismatch */
    if(count > dstSize){
        return SIZE_MISMATCHING;
    }
    
    /* dst overflow */
    if(!is_mem_in_ram((uint32_t)dst, dstSize)){
        return DST_OVERFLOW;
    }
    
    memcpy(dst, src, count);
    
    return COPY_SUCCESS;
}
#endif

/*@} end of group DRIVER_Public_Functions */

/*@} end of group DRIVER_COMMON */

/*@} end of group BL616_Periph_Driver */
