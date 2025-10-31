#include "bflb_clock.h"
#if defined(BL702)
#include "bl702_clock.h"
#elif defined(BL702L)
#include "bl702l_clock.h"
#elif defined(BL606P)
#include "bl606p_clock.h"
#elif defined(BL808)
#include "bl808_clock.h"
#elif defined(BL616)
#include "bl616_clock.h"
#elif defined(BL616L)
#include "bl616l_clock.h"
#elif defined(BL616D)
#include "bl616d_clock.h"
#endif

/****************************************************************************/ /**
 * @brief  get peri clock
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 *
 * @return int
 *
*******************************************************************************/
uint32_t bflb_peripheral_clock_get(uint8_t peri)
{
    return bflb_peripheral_clock_get_by_id(peri);
}

/****************************************************************************/ /**
 * @brief  enable/disable peri clock
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 *
 * @param  enable: Boolean value to enable or disable the clock
 * @return int
 *
*******************************************************************************/
int ATTR_CLOCK_SECTION bflb_peripheral_clock_control(uint8_t peri, bool enable)
{
    return bflb_peripheral_clock_control_by_id(peri, enable);
}

/****************************************************************************/ /**
 * @brief  get peri clock status
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 *
 * @return int
 *
*******************************************************************************/
int bflb_peripheral_clock_status_get(uint8_t peri)
{
    return bflb_peripheral_clock_status_get_by_id(peri);
}

