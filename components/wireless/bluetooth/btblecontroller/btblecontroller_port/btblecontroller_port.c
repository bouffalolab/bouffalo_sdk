#include "btblecontroller_port.h"
#include "bflb_irq.h"
#include "bflb_efuse.h"

#if defined(BL616)
#include "bl616_glb.h"
#endif

#if defined(BL702L)
#include "bl702l_glb.h"
#endif

#if defined(BL616)
#include "wl_api.h"
#define WL_API_RMEM_ADDR    0x20010600
#endif

__attribute__((weak)) void btblecontroller_ble_irq_init(void *handler)
{
    bflb_irq_clear_pending(BLE_IRQn);
    bflb_irq_attach(BLE_IRQn, (irq_callback)handler, NULL);
    bflb_irq_enable(BLE_IRQn);
}

__attribute__((weak)) void btblecontroller_bt_irq_init(void *handler)
{
    bflb_irq_clear_pending(BT_IRQn);
    bflb_irq_attach(BT_IRQn, (irq_callback)handler, NULL);
    bflb_irq_enable(BT_IRQn);
}

__attribute__((weak)) void btblecontroller_dm_irq_init(void *handler)
{
    bflb_irq_clear_pending(DM_IRQn);
    bflb_irq_attach(DM_IRQn, (irq_callback)handler, NULL);
    bflb_irq_enable(DM_IRQn);
}

__attribute__((weak)) void btblecontroller_ble_irq_enable(uint8_t enable)
{
    if(enable)
    {
        bflb_irq_enable(BLE_IRQn); 
    }
    else
    {
        bflb_irq_disable(BLE_IRQn); 
    }
}

__attribute__((weak)) void btblecontroller_bt_irq_enable(uint8_t enable)
{
    if(enable)
    {
        bflb_irq_enable(BT_IRQn);
    }
    else
    {
        bflb_irq_disable(BT_IRQn);
    }
}

__attribute__((weak)) void btblecontroller_dm_irq_enable(uint8_t enable)
{
    if(enable)
    {
        bflb_irq_enable(DM_IRQn); 
    }
    else
    {
        bflb_irq_disable(DM_IRQn); 
    }
}

__attribute__((weak)) void btblecontroller_enable_ble_clk(uint8_t enable)
{
    #if defined(BL702L) || defined(BL702)
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
    uint8_t tmp[8];
    bflb_efuse_get_chipid(tmp);

    mac[0] = tmp[0];
    mac[1] = tmp[1];
    mac[2] = tmp[2];
    mac[3] = tmp[3];
    mac[4] = tmp[4];
    mac[5] = tmp[5];
    return 0;
}
