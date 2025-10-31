#include "bflb_reset.h"
#if defined(BL702)
#include "bl702_reset.h"
#elif defined(BL702L)
#include "bl702l_reset.h"
#elif defined(BL606P)
#include "bl606p_reset.h"
#elif defined(BL808)
#include "bl808_reset.h"
#elif defined(BL616)
#include "bl616_reset.h"
#elif defined(BL616L)
#include "bl616l_reset.h"
#elif defined(BL616D)
#include "bl616d_reset.h"
#endif
/****************************************************************************/ /**
 * @brief  reset peri
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 *
 * @return int
 *
*******************************************************************************/
int bflb_peripheral_reset(uint8_t peri)
{
    return bflb_peripheral_reset_by_id(peri);
}