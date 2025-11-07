#include "btblecontroller_port.h"
#if defined(CONFIG_IOT_SDK)
#include "bl_irq.h"
#else
#include "bflb_irq.h"
#include "bflb_efuse.h"
#endif

#if defined(BL616)
#include "bl616_glb.h"
#include "wl_api.h"
#include "bl616_pds.h"
#include "bl616_mfg_media.h"
#if defined(CONFIG_IOT_SDK)
#include "bl_efuse.h"
#include "bflb_efuse.h"
#endif
#define WL_API_RMEM_ADDR    0x20010600
#endif

#if defined(BL616D)
#include "bl616d_glb.h"
#include "wl_api.h"
#include "bl616d_pds.h"
#define WL_API_RMEM_ADDR    0x20010600
#endif

#if defined(BL616L)
#include "bl616l_glb.h"
#include "bl616l_pds.h"
#include "wl_api.h"
#endif

#if defined(BL808)
#include "bl808_glb.h"
#endif 

#if defined(BL702L)
#include "bl702l_glb.h"
#endif

#if defined(CONFIG_IOT_SDK)
extern void bl_irq_pending_clear(unsigned int source);
#endif

__attribute__((weak)) void btblecontroller_ble_irq_init(void *handler)
{
    #if defined(CONFIG_IOT_SDK)
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
    #if defined(CONFIG_IOT_SDK)
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
    #if defined(CONFIG_IOT_SDK)
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
    #if defined(CONFIG_IOT_SDK)
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
    #if defined(CONFIG_IOT_SDK)
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
    #if defined(CONFIG_IOT_SDK)
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
    #if defined(BL616) || defined(BL616D)
    struct wl_cfg_t *wl_cfg;

    #if WL_API_RMEM_EN
    wl_cfg = wl_cfg_get((uint8_t *)WL_API_RMEM_ADDR);
    wl_cfg->mode = WL_API_MODE_BZ;
    wl_lp_init((uint8_t*)WL_API_RMEM_ADDR,2412);
    #else
    wl_cfg = wl_cfg_get();
    wl_cfg->mode = WL_API_MODE_BZ;
    wl_lp_init(2412);
    #endif

    #elif defined(BL616L)
    //616L_todo, Not find definition of struct wl_cfg_t in bl616L fpga
    #endif
}

__attribute__((weak)) int btblecontroller_efuse_read_mac(uint8_t mac[6])
{
    int status = 0;
    uint8_t tmp[8] = {0};

    #if defined(CONFIG_IOT_SDK)
    #if defined(BL702L)
    extern int bl_wireless_mac_addr_get(uint8_t mac[8]);
    bl_wireless_mac_addr_get(tmp);
    #else
    extern int bl_efuse_read_mac(uint8_t mac[6]);
    status = bl_efuse_read_mac(tmp);
    #endif
    #else
    #if defined(BL616) || defined(BL616D)
    status = mfg_media_read_macaddr_with_lock(tmp, 1);
    #elif defined(BL616L)
    //BL616L_todo, BL616D_todo
    return 1;
    #endif
    #endif //(CONFIG_IOT_SDK)
    mac[0] = tmp[0];
    mac[1] = tmp[1];
    mac[2] = tmp[2];
    mac[3] = tmp[3];
    mac[4] = tmp[4];
    mac[5] = tmp[5];
    return status;
}

#if defined(BL616) || defined(BL616D)
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

__attribute__((weak)) uint8_t btblecontrolller_get_chip_version()
{
    extern void bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info);
    bflb_efuse_device_info_type device_info;
    bflb_efuse_get_device_info(&device_info);
    return device_info.version;
}
#endif

__attribute__((weak)) int btblecontroller_printf(const char *fmt, ...)
{
    #if defined(CONFIG_IOT_SDK)
    extern void vprint(const char *fmt, va_list argp);
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
    #endif

    return 0;
}


#if defined(BL702L) || defined(BL616) || defined(BL616D)
__attribute__((weak)) void btblecontroller_sys_reset(void)
{
    __disable_irq();
    GLB_SW_POR_Reset();
}
#endif

#if defined(CONFIG_BT_MFG_HCI_CMD) || defined(CONFIG_BLE_MFG_HCI_CMD)
__attribute__((weak)) int btblecontroller_putchar(int c)
{
     #if defined(CONFIG_IOT_SDK)
     extern int bl_putchar(int c);
     return bl_putchar(c);
     #else
     extern int putchar(int c);
     return putchar(c);
     #endif
}
#endif

__attribute__((weak)) void btblecontroller_puts(const char *str)
{
    extern int puts(const char *s);
    puts(str);
}


