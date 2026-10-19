#include "bflb_reset.h"
#if defined(BL702)
#include "bl702_reset.h"
#elif defined(BL702L)
#include "bl702l_reset.h"
#elif defined(BL616)
#include "bl616_reset.h"
#elif defined(BL616CL)
#include "bl616cl_reset.h"
#elif defined(BL618DG)
#include "bl618dg_reset.h"
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
