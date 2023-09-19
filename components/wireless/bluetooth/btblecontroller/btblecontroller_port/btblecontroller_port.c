#include "btblecontroller_port.h"
#if defined(CFG_IOT_SDK)
#include "bl_irq.h"
#else
#include "bflb_irq.h"
#include "bflb_efuse.h"
#endif

#if defined(BL616)
#include "bl616_glb.h"
#include "wl_api.h"
#include "bl616_pds.h"
#if defined(CFG_IOT_SDK)
#include "bl_efuse.h"
#endif
#define WL_API_RMEM_ADDR    0x20010600
#endif

#if defined(BL808)
#include "bl808_glb.h"
#endif 

#if defined(BL702L)
#include "bl702l_glb.h"
#endif

#if defined(CFG_IOT_SDK)
extern void bl_irq_pending_clear(unsigned int source);
#endif

__attribute__((weak)) void btblecontroller_ble_irq_init(void *handler)
{
    #if defined(CFG_IOT_SDK)
    bl_irq_pending_clear(BLE_IRQn);
    bl_irq_register(BLE_IRQn, handler);
    bl_irq_enable(BLE_IRQn);
    #else
    bflb_irq_clear_pending(BLE_IRQn);
    bflb_irq_attach(BLE_IRQn, (irq_callback)handler, NULL);
    bflb_irq_enable(BLE_IRQn);
    #endif
}

#if !defined(BL702L)
__attribute__((weak)) void btblecontroller_bt_irq_init(void *handler)
{
    #if defined(CFG_IOT_SDK)
    bl_irq_pending_clear(BT_IRQn);
    bl_irq_register(BT_IRQn, handler);
    bl_irq_enable(BT_IRQn);
    #else
    bflb_irq_clear_pending(BT_IRQn);
    bflb_irq_attach(BT_IRQn, (irq_callback)handler, NULL);
    bflb_irq_enable(BT_IRQn);
    #endif
}

__attribute__((weak)) void btblecontroller_dm_irq_init(void *handler)
{
    #if defined(CFG_IOT_SDK)
    bl_irq_pending_clear(DM_IRQn);
    bl_irq_register(DM_IRQn, handler);
    bl_irq_enable(DM_IRQn);
    #else
    bflb_irq_clear_pending(DM_IRQn);
    bflb_irq_attach(DM_IRQn, (irq_callback)handler, NULL);
    bflb_irq_enable(DM_IRQn);
    #endif
}
#endif

__attribute__((weak)) void btblecontroller_ble_irq_enable(uint8_t enable)
{
    #if defined(CFG_IOT_SDK)
    if(enable)
    {
 
        bl_irq_enable(BLE_IRQn); 
    }
    else
    {
        bl_irq_disable(BLE_IRQn); 
    }
    #else
    if(enable)
    {
 
        bflb_irq_enable(BLE_IRQn); 
    }
    else
    {
        bflb_irq_disable(BLE_IRQn); 
    }
    #endif
}

#if !defined(BL702L)
__attribute__((weak)) void btblecontroller_bt_irq_enable(uint8_t enable)
{
    #if defined(CFG_IOT_SDK)
    if(enable)
    {    
        bl_irq_enable(BT_IRQn); 
    }
    else
    {
        bl_irq_disable(BT_IRQn); 
    }
    #else

    if(enable)
    {
        bflb_irq_enable(BT_IRQn);
    }
    else
    {
        bflb_irq_disable(BT_IRQn);
    }
    #endif
}

__attribute__((weak)) void btblecontroller_dm_irq_enable(uint8_t enable)
{
    #if defined(CFG_IOT_SDK)
    if(enable)
    {    
        bl_irq_enable(DM_IRQn); 
    }
    else
    {
        bl_irq_disable(DM_IRQn); 
    }
    #else
    if(enable)
    {
        bflb_irq_enable(DM_IRQn); 
    }
    else
    {
        bflb_irq_disable(DM_IRQn); 
    }
    #endif
}
#endif

__attribute__((weak)) void btblecontroller_enable_ble_clk(uint8_t enable)
{
    #if defined(BL702L)
    GLB_Set_BLE_CLK(enable);
    #endif
}

__attribute__((weak)) void btblecontroller_rf_restore()
{
  #if defined(BL616)
  struct wl_cfg_t *wl_cfg;

  wl_cfg = wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
  wl_cfg->mode = WL_API_MODE_BZ;
  wl_lp_init((uint8_t*)WL_API_RMEM_ADDR,2412);
  #endif
}

__attribute__((weak)) int btblecontroller_efuse_read_mac(uint8_t mac[6])
{
    int status = 0;
    uint8_t tmp[8];

    #if defined(CFG_IOT_SDK)
    #if defined(BL702L)
    extern int bl_wireless_mac_addr_get(uint8_t mac[8]);
    bl_wireless_mac_addr_get(tmp);
    #else
    extern int bl_efuse_read_mac(uint8_t mac[6]);
    status = bl_efuse_read_mac(tmp);
    #endif
    #else
    bflb_efuse_get_chipid(tmp);
    #endif
    mac[0] = tmp[0];
    mac[1] = tmp[1];
    mac[2] = tmp[2];
    mac[3] = tmp[3];
    mac[4] = tmp[4];
    mac[5] = tmp[5];
    return status;
}

#if defined(BL616)
__attribute__((weak)) void btblecontroller_software_btdm_reset()
{
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_BTDM);
}

__attribute__((weak)) void btblecontroller_software_pds_reset()
{
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_PDS);
}

__attribute__((weak)) void btblecontroller_pds_trim_rc32m()
{
    PDS_Trim_RC32M();
}
#endif

