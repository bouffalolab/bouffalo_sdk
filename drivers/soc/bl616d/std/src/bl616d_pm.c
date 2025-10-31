#include "bl616d_pm.h"
#include "bflb_flash.h"
#include "bl616d_clock.h"
#include "bflb_rtc.h"
#include "bl616d_glb.h"
#include "bl616d_hbn.h"
#include "bl616d_pds.h"
#include "bflb_gpio.h"

#ifndef PM_PDS_GPIO_KEEP_EN
#define PM_PDS_GPIO_KEEP_EN 0
#endif

#ifndef PM_HBN_GPIO_KEEP_EN
#define PM_HBN_GPIO_KEEP_EN 0
#endif

#ifndef PM_PDS_FLASH_POWER_OFF
#define PM_PDS_FLASH_POWER_OFF 1
#endif

#ifndef PM_PDS_PLL_POWER_OFF
#define PM_PDS_PLL_POWER_OFF 1
#endif

#ifndef PM_PDS_RF_POWER_OFF
#define PM_PDS_RF_POWER_OFF 1
#endif

#ifndef PM_PDS_LDO_SOC_LEVEL_DEFAULT
#define PM_PDS_LDO_SOC_LEVEL_DEFAULT AON_LDO_SOC_LEVEL_0P900V
#endif

#ifndef PM_PDS_DCDC_SYS_LEVEL_DEFAULT
#define PM_PDS_DCDC_SYS_LEVEL_DEFAULT AON_DCDC_SYS_LEVEL_1P200V
#endif

#ifndef PM_PDS_LDO18_AON_LEVEL_DEFAULT
#define PM_PDS_LDO18_AON_LEVEL_DEFAULT AON_LDO18_AON_LEVEL_1P80V
#endif

#ifndef PM_PDS_SET_ALL_RAM_RET_EN
#define PM_PDS_SET_ALL_RAM_RET_EN 1
#endif

#ifndef PM_HBN_FLASH_POWER_OFF
#define PM_HBN_FLASH_POWER_OFF 1
#endif

#ifndef PM_HBN_LDO_LEVEL_DEFAULT
#define PM_HBN_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_1P10V
#endif

#ifndef PM_PDS_LDO18IO_POWER_DOWN
#define PM_PDS_LDO18IO_POWER_DOWN 0
#endif

#ifndef PM_PDS_RC32M_POWER_DOWN
#define PM_PDS_RC32M_POWER_DOWN 1
#endif

static char *trig_mode_desc[] = {
    "sync falling edge trigger",
    "sync rising edge trigger",
    "sync low level trigger",
    "sync high level trigger",
    "sync rising & falling edge trigger",
    "sync rising & falling edge trigger",
    "sync rising & falling edge trigger",
    "sync rising & falling edge trigger",
    "async falling edge trigger",
    "async rising edge trigger",
    "async low level trigger",
    "async high level trigger"
};

/* Cache Way Disable, will get from l1c register */
// uint8_t cacheWayDisable = 0;

/* PSRAM IO Configuration, will get from glb register */
// uint32_t psramIoCfg = 0;

/* Flash offset value, will get from sf_ctrl register */
// uint32_t flash_offset = 0;

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel1 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .ldoSocOff = 0,
        .bgSysOff = 0,
        .ctrlGpioIePuPd = 0,
        .dcdc12Off = 0,
        .clkOff = 1,
        .memStby = 1,
        .glbRstProtect = 0,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 1,
        .socEnbForceOn = 0,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .cpupllOff = 1,
        // .pdsLdoSocVselEn = 0,
        // .usbpllOff = 1,
        // .aupllOff = 1,
        .wifipllOff = 1,
        // .pdsLdoSocVsel = 0x8,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
        .pdsGpioIsoMod = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceWbPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIso = 0,
        .forceWbIso = 0,
        .forceUsbIso = 0,
        .forceCpuPdsRst = 0,
        .forceWbPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceWbMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceWbGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceMiscIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceMiscMemStby = 0,
        .forceMiscGateClk = 0,
        .cpuIsoEn = 0,
        .wbIsoEn = 1,
        .usbIsoEn = 0,
        .miscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 0,
        .cpuRst = 0,
        .cpuMemStby = 0,
        .cpuGateClk = 0,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 0,
        .wbGateClk = 0,
        .usbPwrOff = 0,
        .usbRst = 0,
        .usbMemStby = 0,
        .usbGateClk = 0,
        .miscPwrOff = 0,
        .miscRst = 0,
        .miscMemStby = 0,
        .miscGateClk = 0,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .cpuM1WfiMask = 1,
        .lpcpuWfiMask = 1,
        .ctrlUsb33 = 0,
        .ldo18ioOff = 1,
        .pdsGpioKeep = 0,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel2 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .ldoSocOff = 0,
        .bgSysOff = 0,
        .ctrlGpioIePuPd = 0,
        .dcdc12Off = 0,
        .clkOff = 1,
        .memStby = 1,
        .glbRstProtect = 0,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 1,
        .socEnbForceOn = 0,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .cpupllOff = 1,
        // .pdsLdoSocVselEn = 0,
        // .usbpllOff = 1,
        // .aupllOff = 1,
        .wifipllOff = 1,
        // .pdsLdoSocVsel = 0x8,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
        .pdsGpioIsoMod = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceWbPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIso = 0,
        .forceWbIso = 0,
        .forceUsbIso = 0,
        .forceCpuPdsRst = 0,
        .forceWbPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceWbMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceWbGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceMiscIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceMiscMemStby = 0,
        .forceMiscGateClk = 0,
        .cpuIsoEn = 1,
        .wbIsoEn = 0,
        .usbIsoEn = 0,
        .miscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 0,
        .cpuGateClk = 0,
        .wbPwrOff = 0,
        .wbRst = 0,
        .wbMemStby = 0,
        .wbGateClk = 0,
        .usbPwrOff = 0,
        .usbRst = 0,
        .usbMemStby = 0,
        .usbGateClk = 0,
        .miscPwrOff = 0,
        .miscRst = 0,
        .miscMemStby = 0,
        .miscGateClk = 0,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .cpuM1WfiMask = 1,
        .lpcpuWfiMask = 1,
        .ctrlUsb33 = 1,
        .ldo18ioOff = 1,
        .pdsGpioKeep = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel3 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .ldoSocOff = 0,
        .bgSysOff = 0,
        .ctrlGpioIePuPd = 0,
        .dcdc12Off = 0,
        .clkOff = 1,
        .memStby = 1,
        .glbRstProtect = 0,
        .isolation = 1,
        .waitXtalRdy = 1,
        .pdsPwrOff = 1,
        .xtalOff = 1,
        .socEnbForceOn = 0,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .cpupllOff = 1,
        // .pdsLdoSocVselEn = 0,
        // .usbpllOff = 1,
        // .aupllOff = 1,
        .wifipllOff = 1,
        // .pdsLdoSocVsel = 0x8,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
        .pdsGpioIsoMod = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceWbPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIso = 0,
        .forceWbIso = 0,
        .forceUsbIso = 0,
        .forceCpuPdsRst = 0,
        .forceWbPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceWbMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceWbGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceMiscIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceMiscMemStby = 0,
        .forceMiscGateClk = 0,
        .cpuIsoEn = 1,
        .wbIsoEn = 1,
        .usbIsoEn = 0,
        .miscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 0,
        .cpuGateClk = 0,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 0,
        .wbGateClk = 0,
        .usbPwrOff = 0,
        .usbRst = 0,
        .usbMemStby = 0,
        .usbGateClk = 0,
        .miscPwrOff = 0,
        .miscRst = 0,
        .miscMemStby = 0,
        .miscGateClk = 0,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .cpuM1WfiMask = 1,
        .lpcpuWfiMask = 1,
        .ctrlUsb33 = 1,
        .ldo18ioOff = 1,
        .pdsGpioKeep = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel7 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .ldoSocOff = 0,
        .bgSysOff = 0,
        .ctrlGpioIePuPd = 0,
        .dcdc12Off = 0,
        .clkOff = 1,
        .memStby = 1,
        .glbRstProtect = 0,
        .isolation = 1,
        .waitXtalRdy = 1,
        .pdsPwrOff = 1,
        .xtalOff = 1,
        .socEnbForceOn = 0,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .cpupllOff = 1,
        // .pdsLdoSocVselEn = 0,
        // .usbpllOff = 1,
        // .aupllOff = 1,
        .wifipllOff = 1,
        // .pdsLdoSocVsel = 0x8,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
        .pdsGpioIsoMod = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceWbPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIso = 0,
        .forceWbIso = 0,
        .forceUsbIso = 0,
        .forceCpuPdsRst = 0,
        .forceWbPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceWbMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceWbGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceMiscIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceMiscMemStby = 0,
        .forceMiscGateClk = 0,
        .cpuIsoEn = 1,
        .wbIsoEn = 1,
        .usbIsoEn = 1,
        .miscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 0,
        .cpuGateClk = 0,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 0,
        .wbGateClk = 0,
        .usbPwrOff = 1,
        .usbRst = 1,
        .usbMemStby = 0,
        .usbGateClk = 0,
        .miscPwrOff = 0,
        .miscRst = 0,
        .miscMemStby = 0,
        .miscGateClk = 0,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .cpuM1WfiMask = 1,
        .lpcpuWfiMask = 1,
        .ctrlUsb33 = 1,
        .ldo18ioOff = 1,
        .pdsGpioKeep = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel15 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .ldoSocOff = 0,
        .bgSysOff = 0,
        .ctrlGpioIePuPd = 0,
        .dcdc12Off = 0,
        .clkOff = 1,
        .memStby = 1,
        .glbRstProtect = 0,
        .isolation = 1,
        .waitXtalRdy = 1,
        .pdsPwrOff = 1,
        .xtalOff = 1,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 1,
        .pdsRC32mOn = 0,
        .cpupllOff = 1,
        // .pdsLdoSocVselEn = 0,
        // .usbpllOff = 1,
        // .aupllOff = 1,
        .wifipllOff = 1,
        // .pdsLdoSocVsel = 0x8,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
        .pdsGpioIsoMod = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceWbPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIso = 0,
        .forceWbIso = 0,
        .forceUsbIso = 0,
        .forceCpuPdsRst = 0,
        .forceWbPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceWbMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceWbGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceMiscIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceMiscMemStby = 0,
        .forceMiscGateClk = 0,
        .cpuIsoEn = 1,
        .wbIsoEn = 1,
        .usbIsoEn = 1,
        .miscIsoEn = 1,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 0,
        .cpuGateClk = 0,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 0,
        .wbGateClk = 0,
        .usbPwrOff = 1,
        .usbRst = 1,
        .usbMemStby = 0,
        .usbGateClk = 0,
        .miscPwrOff = 1,
        .miscRst = 1,
        .miscMemStby = 0,
        .miscGateClk = 0,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .cpuM1WfiMask = 1,
        .lpcpuWfiMask = 1,
        .ctrlUsb33 = 1,
        .ldo18ioOff = 1,
        .pdsGpioKeep = 0,
    }
};

/****************************************************************************/ /**
 * @brief  Enable PDS Wakeup Source
 *
 * @param  WakeupType: PDS Wakeup Source Type, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_IRRX_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USB_SDU
 *           @arg PDS_WAKEUP_BY_KEYSCAN
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 *           @arg PDS_WAKEUP_BY_SW_CONFIG
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type pm_pds_wakeup_src_en(uint32_t WakeupType)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;
    uint32_t pds_wakeup_src_en_bits = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);

    pds_wakeup_src_en_bits = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);
    pds_wakeup_src_en_bits |= ((uint32_t)1 << WakeupType);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN, pds_wakeup_src_en_bits);

    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS Wakeup Source
 *
 * @param  WakeupType: PDS Wakeup Source Type, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_IRRX_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USB_SDU
 *           @arg PDS_WAKEUP_BY_KEYSCAN
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 *           @arg PDS_WAKEUP_BY_SW_CONFIG
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type pm_pds_wakeup_src_disable(uint32_t WakeupType)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;
    uint32_t pds_wakeup_src_en_bits = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);

    pds_wakeup_src_en_bits = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);
    pds_wakeup_src_en_bits &= ~((uint32_t)1 << WakeupType);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN, pds_wakeup_src_en_bits);

    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  Get PDS Wakeup Source
 *
 * @return wakeup_event
 *
*******************************************************************************/
uint32_t pm_pds_get_all_wakeup_src(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  Detect PDS Wakeup Source
 *
 * @param  WakeupType: PDS Wakeup Source Type, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_IRRX_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USB_SDU
 *           @arg PDS_WAKEUP_BY_KEYSCAN
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 *           @arg PDS_WAKEUP_BY_SW_CONFIG
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type pm_pds_get_wakeup_src(uint32_t WakeupType)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);
    tmpVal &= ((uint32_t)1 << WakeupType);

    return tmpVal ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Mask All PDS Wakeup Source
 *
*******************************************************************************/
void pm_pds_mask_all_wakeup_src(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_RF_DONE_INT_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set gpio pad pull type in lowpower mode
 *
 * @param  pin: gpio
 *
 * @param  pu: power up
 * @param  pd: power down
 * @param  ie: Active IE (interrupt)
 *
 * @return SUCCESS or ERROR
 *
 * @note   Pu and Pd not depend on IE
 *
*******************************************************************************/
BL_Err_Type pm_set_gpio_pu_pd_ie(int pin, int pu, int pd, int ie)
{
    if (pin <= 7) {
        HBN_AON_PAD_CFG_Type aonPadCfg;
        aonPadCfg.ctrlEn = 1;
        aonPadCfg.ie = ie;
        aonPadCfg.oe = 0;
        aonPadCfg.pullUp = pu;
        aonPadCfg.pullDown = pd;
        HBN_Aon_Pad_Cfg(pin, &aonPadCfg);
    } else {
        PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(pin, pu, pd, ie);
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int mask type in lowpower mode
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type pm_set_gpio_int_mask(int pin, int int_mask)
{
    if (pin <= 7) {
        HBN_Set_Aon_Pad_IntMask(pin, int_mask);

        if (int_mask == UNMASK) {
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
        }
    } else {
        PDS_Set_GPIO_Pad_IntMask(pin, int_mask);

        if (int_mask == UNMASK) {
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_GPIO_INT, UNMASK);
        }
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad trigger mode and int mask type in lowpower mode
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @param  trig: trig type, this parameter can be one of the following values:
 *           @arg PM_IO_TRIG_SYNC_FALLING_EDGE
 *           @arg PM_IO_TRIG_SYNC_RISING_EDGE
 *           @arg PM_IO_TRIG_SYNC_LOW_LEVEL
 *           @arg PM_IO_TRIG_SYNC_HIGH_LEVEL
 *           @arg PM_IO_TRIG_SYNC_RISING_FALLING_EDGE
 *           @arg PM_IO_TRIG_ASYNC_FALLING_EDGE
 *           @arg PM_IO_TRIG_ASYNC_RISING_EDGE
 *           @arg PM_IO_TRIG_ASYNC_LOW_LEVEL
 *           @arg PM_IO_TRIG_ASYNC_HIGH_LEVEL
 *           @arg PM_PDS_IO_TRIG_NONE
 *
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type pm_set_gpio_trig_mode_int_mask(int pin, int trig_mode, int int_mask)
{
    if (pin <= 7) {
        HBN_Set_Aon_Pad_IntMask(pin, int_mask);
        HBN_Set_Aon_Pad_IntMode(trig_mode);

        if (int_mask == UNMASK) {
            HBN_Hw_Pu_Pd_Cfg(DISABLE);
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
        }
    } else if (pin < GPIO_PIN_MAX) {
        PDS_Set_GPIO_Pad_IntMask(pin, int_mask);
        PDS_Set_GPIO_Pad_IntMode(pin, trig_mode);
        if (int_mask == UNMASK) {
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_GPIO_INT, UNMASK);
        }
    } else {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio config in lowpower mode
 *
 * @param  gpio_cfg: gpio config type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int pm_lowpower_gpio_cfg(lp_gpio_cfg_type *gpio_cfg)
{
    uint8_t io = 0;
    uint8_t ie = 0;
    uint8_t pu = 0;
    uint8_t pd = 0;
    uint8_t unmask = 0;
    uint8_t trig_mode = 0;

    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        io = i;
        ie = (gpio_cfg->io_ie >> i) & 0x1;
        pu = (gpio_cfg->io_pu >> i) & 0x1;
        pd = (gpio_cfg->io_pd >> i) & 0x1;
        unmask = (gpio_cfg->io_wakeup_unmask >> i) & 0x1;

        if (io <= 7) {
            trig_mode = gpio_cfg->io_0_7_trig_mode;
        } else if ((io >= 8) && (io <= 15)) {
            trig_mode = gpio_cfg->io_8_15_trig_mode;
        } else if ((io >= 16) && (io <= 23)) {
            trig_mode = gpio_cfg->io_16_23_trig_mode;
        } else if ((io >= 24) && (io <= 31)) {
            trig_mode = gpio_cfg->io_24_31_trig_mode;
        } else if ((io >= 32) && (io <= 39)) {
            trig_mode = gpio_cfg->io_32_39_trig_mode;
        } else if ((io >= 40) && (io <= 47)) {
            trig_mode = gpio_cfg->io_40_47_trig_mode;
        } else if (io < GPIO_PIN_MAX) {
            trig_mode = gpio_cfg->io_48_52_trig_mode;
        } else {
            return -1;
        }

        if (unmask) {
            if (!ie) {
                return -1;
            }
            bflb_gpio_init(gpio, io, GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);

            pm_set_gpio_pu_pd_ie(io, pu, pd, ie);
            pm_set_gpio_trig_mode_int_mask(io, trig_mode, UNMASK);
        } else {
            pm_set_gpio_pu_pd_ie(io, pu, pd, ie);
            pm_set_gpio_int_mask(io, MASK);
        }
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  get trigger mode description
 *
 * @param  gpioindex_cfg: gpio trigger mode index
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
char *pm_get_trig_mode_desc(int index)
{
    if (index < 0 || index >= sizeof(trig_mode_desc) / sizeof(trig_mode_desc[0])) {
        return "Invalid trigger mode index";
    }
    return trig_mode_desc[index];
}

/****************************************************************************/ /**
 * @brief  pm pds enable
 *
 * @param  cfg:
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION pm_pds_enable(uint32_t *cfg)
{
    //uint32_t sf_pin_select = 0;
    PM_PDS_CFG_Type *p = (PM_PDS_CFG_Type *)cfg;
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = NULL;
    uintptr_t irq_flag;

    /* To make it simple and safe*/
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
    irq_flag = csi_irq_save();
#else
    irq_flag = __RV_CSR_READ(CSR_MSTATUS);
    __disable_irq();
#endif

    if (ENABLE == p->ldo_soc_cfg.lp_mode_en) {
        AON_Ctrl_Ldo_Soc_Mode_by_HW(ENABLE);
        AON_Set_Ldo_Soc_Vout_in_Lowpower(p->ldo_soc_cfg.voltage_level);
    } else {
        AON_Set_Ldo_Soc_Mode(AON_LDO_SOC_NORMAL_MODE);
        AON_Set_Ldo_Soc_Vout(p->ldo_soc_cfg.voltage_level);
    }

    if (ENABLE == p->dcdc_sys_cfg.lp_mode_en) {
        AON_Ctrl_Dcdc_Sys_Mode_by_HW(ENABLE);
        AON_Set_Dcdc_Sys_Vout_in_Lowpower(p->dcdc_sys_cfg.voltage_level);
    } else {
        AON_Set_Dcdc_Sys_Mode(AON_DCDC_SYS_NORMAL_MODE);
        AON_Set_Dcdc_Sys_Vout(p->dcdc_sys_cfg.voltage_level);
    }

    if (ENABLE == p->ldo18_aon_cfg.lp_mode_en) {
        AON_Ctrl_Ldo18_Aon_Mode_by_HW(ENABLE);
        AON_Set_Ldo18_Aon_Vout_in_Lowpower(p->ldo18_aon_cfg.voltage_level);
    } else {
        AON_Set_Ldo18_Aon_Mode(AON_LDO18_AON_NORMAL_MODE);
        AON_Set_Ldo18_Aon_Vout(p->ldo18_aon_cfg.voltage_level);
    }
    /************************ PDS INT SET ***********************/
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    if (p->sleepTime) {
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_TIMER, UNMASK); // unmask pds sleep time wakeup
    } else {
        PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_TIMER, MASK); // mask pds sleep time wakeup
    }

    /* clear all interrupt status,except pds_gpio_int */
    PDS_IntClear();

    /* enable PDS interrupt to wakeup CPU (PDS1:CPU not powerdown, CPU __WFI)*/
    /* if enter pds1, please User call pm_pds_irq_register function before pm_pds_mode_enter */
    // pm_pds_irq_register();

    switch (p->pdsLevel) {
        case PM_PDS_LEVEL_1:
            pPdsCfg = &pdsCfgLevel1;
            break;
        case PM_PDS_LEVEL_2:
            pPdsCfg = &pdsCfgLevel2;
            break;
        case PM_PDS_LEVEL_3:
            pPdsCfg = &pdsCfgLevel3;
            break;
        case PM_PDS_LEVEL_7:
            pPdsCfg = &pdsCfgLevel7;
            break;
        case PM_PDS_LEVEL_15:
            pPdsCfg = &pdsCfgLevel15;
            break;
        default:
            return;
    }

    if (p->powerDownFlash) {
        /* get sw uasge 0,get flash type */
        //sf_pin_select = (BL_RD_WORD(0x2000C000 + 0x60) >>14) &0x3f;
        HBN_Power_Down_Flash((spi_flash_cfg_type *)p->flashCfg);

        /* power off flash */
        AON_Set_Ldo18_AON_Power_Switch_For_FLASH(0);
        /* Turn Off Flash/PSRAM PAD IE */
        /* psram io config */
    }
    /* power off psram */
    AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(0);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

    /* power off mbg and sfreg */
    AON_Power_Off_MBG();
    AON_Power_Off_SFReg();

    PDS_Pu_PLL_Enable();

    if (p->turnoffWifiPLL) {
        GLB_Power_Off_WIFIPLL();
    } else {
        pPdsCfg->pdsCtl.wifipllOff = 0;
    }

    if (p->turnoffCpuPLL) {
        GLB_Power_Off_CPUPLL();
    } else {
        pPdsCfg->pdsCtl.cpupllOff = 0;
    }

    if (p->turnoffRC32M) {
        pPdsCfg->pdsCtl.pdsRC32mOn = 0;
    } else {
        pPdsCfg->pdsCtl.pdsRC32mOn = 1;
    }

    if (p->ioKeepEn) {
        pPdsCfg->pdsCtl.pdsGpioIsoMod = 1;
        pPdsCfg->pdsCtl5.pdsGpioKeep = 7;
    }

    if (!(p->turnOffRF)) {
        pPdsCfg->pdsCtl.pdsCtlRfSel = 0;
    }

    if ((0 != BL_RD_REG(PDS_BASE, PDS_GPIO_IE_0)) ||
        (0 != BL_RD_REG(PDS_BASE, PDS_GPIO_IE_1)) ||
        (0 != BL_RD_REG(PDS_BASE, PDS_GPIO_PU_0)) ||
        (0 != BL_RD_REG(PDS_BASE, PDS_GPIO_PU_1)) ||
        (0 != BL_RD_REG(PDS_BASE, PDS_GPIO_PD_0)) ||
        (0 != BL_RD_REG(PDS_BASE, PDS_GPIO_PD_1))) {
        pPdsCfg->pdsCtl.ctrlGpioIePuPd = 1;
    }

    if (p->ramRetEn) {
        PDS_Set_All_WRAM_Retention();
        PDS_Set_All_OCRAM_Retention();
    }

    PDS_Default_Level_Config(pPdsCfg, p->sleepTime);

    __WFI(); /* if(.wfiMask==0){CPU won't power down until PDS module had seen __wfi} */

    AON_Set_Ldo18_AON_Power_Switch_For_FLASH(1);
    AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(1);
    AON_Power_On_MBG();
    AON_Power_On_SFReg();

    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL | GLB_PLL_CPUPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);

    if (p->powerDownFlash) {
        if (p->pdsLevel < PM_PDS_LEVEL_2) {
            /* Init flash gpio, remove, for CPU wakeup case, GLB is not powered off */
            //bflb_sf_cfg_init_flash_gpio((uint8_t)sf_pin_select, 1);

            bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
            bflb_sflash_restore_from_powerdown((spi_flash_cfg_type *)p->flashCfg, 0, SF_CTRL_FLASH_BANK0);
        }
    }

    bflb_irq_restore(irq_flag);
}

/**
 * @brief power management in pds(power down sleep) mode
 *
 * cpu's behavior after wakeup depend on psd level,see flow table if cpu off ,
 * cpu will reset after wakeup
 *
 *    PD_CORE   PD_CORE_MISC_DIG    PD_CORE_MISC_ANA   PD_CORE_CPU PD_BZ PD_USB
 *PDS0    ON           ON                   ON            ON        ON      ON
 *PDS1    ON           ON                   ON            ON        ON      OFF
 *PDS2    ON           ON                   ON            ON        OFF     ON
 *PDS3    ON           ON                   ON            ON        OFF     OFF
 *PDS4    ON           ON                   ON            OFF       ON      ON
 *PDS5    ON           ON                   ON            OFF       ON      OFF
 *PDS6    ON           ON                   ON            OFF       OFF     ON
 *PDS7    ON           ON                   ON            OFF       OFF     OFF
 */
void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time)
{
    if ((sleep_time != 0) && (sleep_time <= PDS_WARMUP_LATENCY_CNT)) {
        return;
    }

    PM_PWR_Cfg ldo18aon_cfg = {
        .lp_mode_en = 1,
        .voltage_level = PM_PDS_LDO18_AON_LEVEL_DEFAULT,
    };

    PM_PWR_Cfg dcdcsys_cfg = {
        .lp_mode_en = 1,
        .voltage_level = PM_PDS_DCDC_SYS_LEVEL_DEFAULT,
    };

    PM_PWR_Cfg ldosoc_cfg = {
        .lp_mode_en = 1,
        .voltage_level = PM_PDS_LDO_SOC_LEVEL_DEFAULT,
    };

    PM_PDS_CFG_Type cfg = {
        .pdsLevel = pds_level,                    /*!< PDS level */
        .ldo18_aon_cfg = ldo18aon_cfg,            /*!< Power Config of ldo18_aon */
        .dcdc_sys_cfg = dcdcsys_cfg,              /*!< Power Config of dcdc_sys */
        .ldo_soc_cfg = ldosoc_cfg,                /*!< Power Config of ldo_soc */
        .turnOffRF = PM_PDS_RF_POWER_OFF,         /*!< Wheather turn off RF */
        .powerDownFlash = PM_PDS_FLASH_POWER_OFF, /*!< Whether power down flash */
        .ramRetEn = PM_PDS_SET_ALL_RAM_RET_EN,    /*!< Whether OCRAM Retention */
        .turnoffRC32M = PM_PDS_RC32M_POWER_DOWN,  /*!< Whether trun off RC32M */
        .turnoffWifiPLL = PM_PDS_PLL_POWER_OFF,   /*!< Whether trun off WiFi PLL */
        .turnoffCpuPLL = PM_PDS_PLL_POWER_OFF,    /*!< Whether trun off AUDIO PLL */
        .ioKeepEn = PM_PDS_GPIO_KEEP_EN,          /*!< PDS io keep select */
        .sleepTime = sleep_time,                  /*!< PDS sleep time */
        .flashCfg = NULL,                         /*!< Flash config pointer, used when power down flash */
        .preCbFun = NULL,                         /*!< Pre callback function */
        .postCbFun = NULL,                        /*!< Post callback function */
    };

    if (cfg.powerDownFlash) {
        uint32_t flash_cfg_len;
        bflb_flash_get_cfg((uint8_t **)&cfg.flashCfg, &flash_cfg_len);
    }

    pm_pds_enable((uint32_t *)&cfg);
}

/**
 * @brief
 *
 * power management in hbn(hibernation) mode
 * cpu will reset after wakeup
 *
 * HBN_LEVEL PD_AON PD_AON_HBNRTC PD_AON_HBNCORE PD_CORE&...
 * HBN0        ON       ON            ON            OFF
 * HBN1        ON       ON            OFF           OFF
 * HBN2        ON       OFF           OFF           OFF
 *
 * @param hbn_level
 */
void ATTR_TCM_SECTION pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level,
                                        uint64_t sleep_time)
{
    uintptr_t irq_flag;

    /* To make it simple and safe*/
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
    irq_flag = csi_irq_save();
#else
    irq_flag = __RV_CSR_READ(CSR_MSTATUS);
    __disable_irq();
#endif

    irq_flag = irq_flag;

    bflb_irq_clear_pending(HBN_OUT0_IRQn);
    bflb_irq_clear_pending(HBN_OUT1_IRQn);

    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    if (sleep_time) {
        struct bflb_device_s *rtc_dev = NULL;
        bflb_rtc_set_time(rtc_dev, sleep_time); // sleep time,unit is cycle
    } else {
        /* clear RTC timer interrupt */
        HBN_Clear_RTC_INT();
        /* commpare value set max value */
        HBN_Set_RTC_Timer(0, 0xFFFFFFFF, 0xFF, 0);
    }

    /* MCU CLK SELECT RC32M */
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

#if PM_HBN_FLASH_POWER_OFF
    spi_flash_cfg_type *flash_cfg;
    uint32_t flash_cfg_len;

    /* get flash config */
    bflb_flash_get_cfg((uint8_t **)&flash_cfg, &flash_cfg_len);

    HBN_Power_Down_Flash(flash_cfg);
#endif

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    HBN_Level_Select(hbn_level);

    while (1) {
        arch_delay_ms(100);
        GLB_SW_POR_Reset();
    }
}

void pm_set_wakeup_callback(void (*wakeup_callback)(void))
{
    BL_WR_REG(HBN_BASE, HBN_RSV1, (uint32_t)wakeup_callback);
    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);
}

void pm_set_boot2_app_jump_para(uint32_t para)
{
    BL_WR_REG(HBN_BASE, HBN_RSV1, para);
    /* Set APP jump flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, APP_JUMP_ENTER_FLAG);
}

void pm_hbn_out0_irq_register(void)
{
    bflb_irq_attach(HBN_OUT0_IRQn, (irq_callback)HBN_OUT0_IRQn, NULL);
    bflb_irq_enable(HBN_OUT0_IRQn);
}

void pm_hbn_out1_irq_register(void)
{
    bflb_irq_attach(HBN_OUT1_IRQn, (irq_callback)HBN_OUT1_IRQn, NULL);
    bflb_irq_enable(HBN_OUT1_IRQn);
}

void HBN_OUT0_IRQ(void)
{
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO0)) {
        HBN_Clear_IRQ(HBN_INT_GPIO0);
        pm_irq_callback(PM_HBN_GPIO0_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO1)) {
        HBN_Clear_IRQ(HBN_INT_GPIO1);
        pm_irq_callback(PM_HBN_GPIO1_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO2)) {
        HBN_Clear_IRQ(HBN_INT_GPIO2);
        pm_irq_callback(PM_HBN_GPIO2_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO3)) {
        HBN_Clear_IRQ(HBN_INT_GPIO3);
        pm_irq_callback(PM_HBN_GPIO3_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO4)) {
        HBN_Clear_IRQ(HBN_INT_GPIO4);
        pm_irq_callback(PM_HBN_GPIO4_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO5)) {
        HBN_Clear_IRQ(HBN_INT_GPIO5);
        pm_irq_callback(PM_HBN_GPIO5_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO6)) {
        HBN_Clear_IRQ(HBN_INT_GPIO6);
        pm_irq_callback(PM_HBN_GPIO6_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO7)) {
        HBN_Clear_IRQ(HBN_INT_GPIO7);
        pm_irq_callback(PM_HBN_GPIO7_WAKEUP_EVENT);
    } else {
        HBN_Clear_RTC_INT();
        HBN_Clear_IRQ(HBN_INT_RTC);
        pm_irq_callback(PM_HBN_RTC_WAKEUP_EVENT);
    }
}

void ATTR_TCM_SECTION HBN_OUT1_IRQ(void)
{
    if (SET == HBN_Get_INT_State(HBN_INT_BOD)) {
        HBN_Clear_IRQ(HBN_INT_BOD);
        pm_irq_callback(PM_HBN_BOD_WAKEUP_EVENT);
    }
}

__WEAK void pm_irq_callback(enum pm_event_type event)
{
}

void pm_pds_irq_register(void)
{
    bflb_irq_attach(PDS_WAKEUP_IRQn, (irq_callback)PDS_WAKEUP_IRQ, NULL);
    bflb_irq_enable(PDS_WAKEUP_IRQn);
}

void ATTR_TCM_SECTION PDS_WAKEUP_IRQ(void)
{
    /* WAKE INT */
    if (SET == PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
    }
    /* RF DONE */
    else if (SET == PDS_Get_IntStatus(PDS_INT_RF_DONE)) {
    }
    /* WIFI_TBTT_SLEEP */
    else if (SET == PDS_Get_IntStatus(PDS_INT_WIFI_TBTT_SLEEP)) {
    }
    /* WIFI_TBTT_WAKEUP */
    else if (SET == PDS_Get_IntStatus(PDS_INT_WIFI_TBTT_WAKEUP)) {
    } else {
    }

    PDS_IntClear();
}
