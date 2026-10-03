#include <stdarg.h>
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
#endif

#if defined(BL618DG)
#include "bl618dg_glb.h"
#ifndef CONFIG_DBG_RUN_ON_FPGA
#include "wl_api.h"
#endif
#include "bl618dg_pds.h"
#include "bflb_gpio.h"
#endif

#if defined(BL616CL)
#include "bl616cl_glb.h"
#include "bl616cl_pds.h"
#include "wl_api.h"
#endif

#if defined(BL702L)
#include "bl702l_glb.h"
#if !defined(CONFIG_IOT_SDK)
#include "bl702l_hbn.h"
#include "bl702l_pds.h"
#endif
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
#ifndef CONFIG_DBG_RUN_ON_FPGA
#if (LE_PDS_ENABLE)
    #if defined(BL616) || defined(BL618DG) || defined(BL616CL)
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

    #endif
#endif /* LE_PDS_ENABLE */
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
    #if defined(BL616) || defined(BL618DG) || defined(BL616CL)
    status = mfg_media_read_macaddr_with_lock(tmp, 1);
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

#if defined(BL616) || defined(BL618DG)
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
    #else
    va_list argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
    #endif

    return 0;
}

#if defined(BL702L) || defined(BL616) || defined(BL618DG)
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
     #if !defined(BL618DG)
     extern int putchar(int c);
     #endif
     return putchar(c);
     #endif
}
#endif

__attribute__((weak)) void btblecontroller_puts(const char *str)
{
    extern int puts(const char *s);
    puts(str);
}

__attribute__((weak)) uint64_t btblecontroller_mtimer_get_time_us(void)
{
#if defined(BL702L)
    extern uint64_t bl_timer_now_us64(void);
    return bl_timer_now_us64();
#else
    extern uint64_t bflb_mtimer_get_time_us(void);
    return bflb_mtimer_get_time_us();
#endif
}

#if defined(BL618DG)
__attribute__((weak)) void btblecontroller_gpio_config(uint8_t pin, uint8_t is_high)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, pin, GPIO_FUNC_GPIO | GPIO_OUTPUT);

    if (is_high) {
        bflb_gpio_set(gpio, pin);
    } else {
        bflb_gpio_reset(gpio, pin);
    }
}

/**
 * @brief Trigger RC32K XTAL counter calibration (setup phase)
 * Configures registers to start counting RC32K cycles using XTAL as reference
 */
__attribute__((weak)) void btblecontroller_rc32k_xtal_count_trigger(void)
{
    GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_XCLK);

    GLB_Set_Top_Misc_Xtal(0);
    /* 64 RC32K cycles */
    PDS_Set_32K_Cycle(4);
    PDS_Xtal_Cnt_32K_Enable();

    GLB_Trigger_Xtal_Cnt_32K_Process();
}

/**
 * @brief Wait for RC32K XTAL counter calibration and get result
 * @return xtal_cnt2 value (count result)
 */
__attribute__((weak)) uint16_t btblecontroller_rc32k_xtal_count_wait_result(void)
{
    uint64_t startUs;
    uint32_t waitUs;
    BL_Sts_Type done;
    uint32_t xtal_cnt2 = 0;
    uint32_t xtal_cnt2_res = 0;
    uint32_t xtal_cnt2_x64 = 0;

    startUs = btblecontroller_mtimer_get_time_us();
    do {
        done = PDS_Xtal_Cnt_32K_Is_Done();
        waitUs = (uint32_t)(btblecontroller_mtimer_get_time_us() - startUs);
    } while ((done == RESET) && (waitUs < 10000U));

    /* Read count values */
    PDS_Xtal_Cnt_32K_Get_Result(&xtal_cnt2, &xtal_cnt2_res);
    xtal_cnt2_x64 = (xtal_cnt2 << 6) + xtal_cnt2_res;

    return (uint16_t)xtal_cnt2;
}
#endif

/*
 * Current-SDK shims for BL702L controller code that still calls HOSAL APIs
 * directly; legacy builds get the same symbols from platform/hosal/bl702l_hal.
 */
#if defined(BL702L) && !defined(CONFIG_IOT_SDK)

#ifdef BL702L_Delay_US
#undef BL702L_Delay_US
#endif

#define BL_RTC_MAX_COUNTER  ((1ULL << 40) - 1)

static uint64_t bl702l_rtc_get_counter(void)
{
    uint32_t low;
    uint32_t high;

    HBN_Get_RTC_Timer_Val(&low, &high);
    return ((uint64_t)high << 32) | low;
}

__attribute__((weak)) uint64_t bl_rtc_get_aligned_counter(void)
{
    uint32_t low_previous;
    uint32_t low;
    uint32_t high;

    HBN_Get_RTC_Timer_Val(&low_previous, &high);
    do {
        HBN_Get_RTC_Timer_Val(&low, &high);
    } while (low == low_previous);

    return ((uint64_t)high << 32) | low;
}

__attribute__((weak)) uint64_t bl_rtc_get_delta_counter(uint64_t ref_cnt)
{
    uint64_t cnt = bl702l_rtc_get_counter();

    ref_cnt &= BL_RTC_MAX_COUNTER;
    if (cnt < ref_cnt) {
        cnt += BL_RTC_MAX_COUNTER + 1;
    }
    return cnt - ref_cnt;
}

__attribute__((weak)) void bl_rtc_trigger_xtal_cnt_32k(void)
{
    uint32_t value = BL_RD_REG(HBN_BASE, HBN_GLB);

    if (BL_GET_REG_BITS_VAL(value, HBN_F32K_SEL) == 1) {
        return;
    }

    value = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    value = BL_SET_REG_BIT(value, GLB_CLR_XTAL_CNT_32K_DONE);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, value);

    value = BL_RD_REG(GLB_BASE, GLB_XTAL_DEG_32K);
    value = BL_SET_REG_BIT(value, GLB_XTAL_CNT_32K_SW_TRIG_PS);
    BL_WR_REG(GLB_BASE, GLB_XTAL_DEG_32K, value);
}

__attribute__((weak)) uint16_t bl_rtc_process_xtal_cnt_32k(void)
{
    uint32_t value = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);

    if (BL_GET_REG_BITS_VAL(value, PDS_XTAL_CNT_32K_PROCESS)) {
        do {
            value = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
        } while (!BL_GET_REG_BITS_VAL(value, PDS_XTAL_CNT_32K_DONE));
    }

    return BL_GET_REG_BITS_VAL(value, PDS_RO_XTAL_CNT_32K_CNT);
}

__attribute__((weak)) uint32_t bl_rtc_32k_to_32m(uint32_t cycles)
{
    uint32_t value = BL_RD_REG(PDS_BASE, PDS_XTAL_CNT_32K);
    uint16_t cnt = BL_GET_REG_BITS_VAL(value, PDS_RO_XTAL_CNT_32K_CNT);
    uint16_t res = BL_GET_REG_BITS_VAL(value, PDS_RO_XTAL_CNT_32K_RES);

    return cycles * cnt + cycles * res / 64;
}

__attribute__((weak)) uint64_t bl_timer_now_us64(void)
{
    return bflb_mtimer_get_time_us();
}

__attribute__((weak)) void BL702L_Delay_US(uint32_t cnt)
{
    bflb_mtimer_delay_us(cnt);
}

__attribute__((weak)) int bl_wireless_mac_addr_get(uint8_t mac[8])
{
    mac[0] = 0;
    mac[1] = 0;
    return bflb_efuse_read_mac_address_opt(0, mac + 2, 1);
}

/* Legacy parity: bl_iot_sdk hosal defaults to 0 until the app calls
 * bl_wireless_default_tx_power_set(); a strong app symbol overrides this. */
__attribute__((weak)) int8_t bl_wireless_default_tx_power_get(void)
{
    return 0;
}

#endif /* BL702L && !CONFIG_IOT_SDK */
