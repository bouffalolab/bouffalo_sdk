#include "bl_lp.h"
#include "bl616d_glb.h"
#include "bl616d_hbn.h"
#include "bl616d_psram.h"
#include "bl_sys.h"

#define RST_REASON         HBN_SYS_RESET_REASON     // use 4 Bytes
#define RST_REASON_CHK     HBN_SYS_RESET_REASON_CHK // use 4 Bytes
#define RST_REASON_CHK_VAL (0xBF1BA55A)

static BL_RST_REASON_E s_rst_reason = BL_RST_POWER_ON;

static char *RST_REASON_ARRAY[] = {
    "BL_RST_POWER_ON",
    "BL_RST_WATCHDOG",
    "BL_RST_PDS_WAKEUP",
    "BL_RST_HBN_WAKEUP",
    "BL_RST_SOFTWARE"
};

BL_RST_REASON_E bl_sys_rstinfo_get(void)
{
    return s_rst_reason;
}

char *bl_sys_rstinfo_getstring(void)
{
    return RST_REASON_ARRAY[s_rst_reason];
}

int bl_sys_rstinfo_set(BL_RST_REASON_E val)
{
    RST_REASON = (uint32_t)val;
    RST_REASON_CHK = (uint32_t)val ^ RST_REASON_CHK_VAL;

    return 0;
}

void bl_sys_rstinfo_init(void)
{
    uint32_t reason = RST_REASON;
    uint32_t reason_chk = RST_REASON_CHK;

    if (SET == AON_Wdg_Status_Get()) {
        printf("Watchdog reset\r\n");
        AON_Wdg_Status_Clear();
        bl_sys_rstinfo_set(BL_RST_WATCHDOG);
    } else if (SET == PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
        printf("PDS wakeup\r\n");
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_TIMER)) {
            printf("--PDS timer wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_HBN_IRQ_OUT)) {
            printf("--HBN IRQ out wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_GPIO_IRQ)) {
            printf("--GPIO IRQ wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_GPIO_INT)) {
            printf("--PDS GPIO INT wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_IRRX_INT)) {
            printf("--IRRX INT wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_WIFI_WAKEUP)) {
            printf("--WIFI wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_DM_SLP_IRQ)) {
            printf("--DM SLP IRQ wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_USB_SDU)) {
            printf("--USB SDU wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_KEYSCAN)) {
            printf("--Keyscan wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ)) {
            printf("--WIFI TBTT wakeup\r\n");
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_SW_CONFIG)) {
            printf("--SW config wakeup\r\n");
        }
        bl_sys_rstinfo_set(BL_RST_PDS_WAKEUP);
    } else if (0 != HBN_Get_All_INT_State() && (1 << HBN_INT_BOD) != HBN_Get_All_INT_State()) {
        printf("HBN wakeup:%08x\r\n", HBN_Get_All_INT_State());
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO0)) {
            printf("--GPIO0 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO1)) {
            printf("--GPIO1 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO2)) {
            printf("--GPIO2 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO3)) {
            printf("--GPIO3 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO4)) {
            printf("--GPIO4 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO5)) {
            printf("--GPIO5 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO6)) {
            printf("--GPIO6 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_GPIO7)) {
            printf("--GPIO7 wakeup\r\n");
        }
        if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
            printf("--RTC wakeup\r\n");
        }
        bl_sys_rstinfo_set(BL_RST_HBN_WAKEUP);
    } else {
        if ((reason ^ RST_REASON_CHK_VAL) == reason_chk) {
            s_rst_reason = reason;
            if (BL_RST_SOFTWARE == s_rst_reason) {
                printf("Software reset\r\n");
            } else if (BL_RST_WATCHDOG == s_rst_reason) {
                printf("Watchdog reset\r\n");
            } else if (BL_RST_PDS_WAKEUP == s_rst_reason) {
                printf("PDS wakeup reset\r\n");
            } else if (BL_RST_HBN_WAKEUP == s_rst_reason) {
                printf("HBN wakeup reset\r\n");
            } else {
                printf("Unknown reset\r\n");
            }
        } else {
            printf("Power on reset\r\n");
            bl_sys_rstinfo_set(BL_RST_POWER_ON);
        }
    }
}

int bl_sys_rstinfo_getsting(char *info)
{
    memcpy(info, (char *)RST_REASON_ARRAY[s_rst_reason], strlen(RST_REASON_ARRAY[s_rst_reason]));
    *(info + strlen(RST_REASON_ARRAY[s_rst_reason])) = '\0';
    return 0;
}

int bl_sys_em_config(void)
{
    /* not support em config */

    return 0;
}

int bl_sys_reset_por(void)
{
    AON_Power_Off_Xtal_32K();
    HBN_32K_Sel(0); // f32k select rc32k
    bl_sys_rstinfo_set(BL_RST_SOFTWARE);
    __disable_irq();

    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_BTDM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_ZIGBEE);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_BLE2);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_ZIGBEE2);

    arch_delay_ms(10);

    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }

    return 0;
}

void bl_sys_reset_system(void)
{
    bl_sys_rstinfo_set(BL_RST_SOFTWARE);
    __disable_irq();
    GLB_SW_System_Reset();
    while (1) {
        /*empty dead loop*/
    }
}