#include <bl702l.h>
#include <hardware/timer_reg.h>
#include <bl702l_glb.h>
#include <bl702l_hbn.h>
#include <bl_sys.h>

static char * RST_REASON_ARRAY[BL_RST_BOD + 1] = {
    [BL_RST_POWER_OFF]              = "BL_RST_POWER_OFF",
    [BL_RST_HARDWARE_WATCHDOG]      = "BL_RST_HARDWARE_WATCHDOG",
    [BL_RST_FATAL_EXCEPTION]        = "BL_RST_FATAL_EXCEPTION",
    [BL_RST_SOFTWARE_WATCHDOG]      = "BL_RST_SOFTWARE_WATCHDOG",
    [BL_RST_SOFTWARE]               = "BL_RST_SOFTWARE",
    [BL_RST_HBN]                    = "BL_RST_HBN",
    [BL_RST_BOD]                    = "BL_RST_BOD",
};

static BL_RST_REASON_E s_rst_reason = BL_RST_POWER_OFF;

BL_RST_REASON_E bl_sys_rstinfo_get(void)
{
    return s_rst_reason;
}

char * bl_sys_rstinfo_getstring(void)
{
    return RST_REASON_ARRAY[s_rst_reason];
}

void bl_sys_rstinfo_init(void)
{
    uint32_t wdt_wsr = 0;
    uint8_t wdt_rst;
    uint8_t hbn_rst;
    uint8_t pds_rst;

    wdt_wsr = BL_RD_REG(TIMER_BASE, TIMER_WSR);
    wdt_rst = wdt_wsr & TIMER_WTS;
    hbn_rst = BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE,HBN_GLB),HBN_RESET_EVENT);
    pds_rst = BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE,PDS_INT),PDS_RESET_EVENT);

    // clear reset status
    wdt_wsr &= ~TIMER_WTS;
    BL_WR_REG(TIMER_BASE, TIMER_WSR, wdt_wsr);
    HBN_Clear_Reset_Event();
    PDS_Clear_Reset_Event();

    if (wdt_rst) {
        s_rst_reason = BL_RST_HARDWARE_WATCHDOG;
    } else if(hbn_rst == 0x0D) {
        s_rst_reason = BL_RST_BOD;
    } else if(hbn_rst == 0x04) {
        s_rst_reason = BL_RST_HBN;
    } else if(pds_rst == 0x07) {
        s_rst_reason = BL_RST_POWER_OFF;
    } else {
        s_rst_reason = BL_RST_SOFTWARE;
    }
}

int bl_sys_reset_por(void)
{
    __disable_irq();
    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }

    return 0;
}

void bl_sys_reset_system(void)
{
    __disable_irq();
    GLB_SW_System_Reset();
    while (1) {
        /*empty dead loop*/
    }
}
