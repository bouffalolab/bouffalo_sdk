#ifndef __OT_UTILS_BL702L_H__
#define __OT_UTILS_BL702L_H__

#if defined(CONFIG_BL_SDK)
#include <compiler/compiler_ld.h>
#include <bflb_core.h>
#include <bflb_rtc.h>
#else
#include <compiler/common.h>
#include <bl702l.h>
#include <bl702l_glb.h>
#include <bl_rtc.h>
#include <bl_irq.h>
#include <bl_wireless.h>
#endif

#ifdef CFG_OT_USE_ROM_CODE
#include <rom_lmac154_ext.h>
#include <rom_thread_port_ext.h>
#else
#include <lmac154.h>
#include <lmac154_fpt.h>
#include <zb_timer.h>
#endif


#if defined (CFG_OT_USE_ROM_CODE)
int ot_utils_patch_hook(uint32_t * arg, void *hookret) 
{
    uint32_t patch_hook_addr = (uint32_t)__builtin_return_address(0);
    extern void                * otRadioVar_pds_ptr;

    if (0x210373ee == patch_hook_addr) {
        /** if otRadioVar_pds_ptr is NULL, skip function ot_radioUpdateCslTimeByTxDone */
        if (otRadioVar_pds_ptr) {
            return false;
        }
        return true;
    }
//    else if (0x21037276 == patch_hook_addr) {
//        /** ot_radioSend */
//    }
    return 0;
}

void ot_utils_init(void) 
{
    rom_lmac154_hook_init();
    rom_thread_port_hook_init();
}
#endif

#endif
