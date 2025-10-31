#include "bl616l_pm.h"
#include "bflb_flash.h"
#include "bl616l_clock.h"
#include "bflb_rtc.h"
#include "bl616l_glb.h"
#include "bl616l_hbn.h"
#include "bl616l_pds.h"

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

#ifndef PM_PDS_CLK_DEFAULT_SEL
#define PM_PDS_CLK_DEFAULT_SEL PM_PDS_CLK_F32K
#endif

#ifndef PM_PDS_LDO_SOC_LOWPOWER_ENABLE
#define PM_PDS_LDO_SOC_LOWPOWER_ENABLE 1
#endif

#ifndef PM_PDS_LDO_SYS_LOWPOWER_ENABLE
#define PM_PDS_LDO_SYS_LOWPOWER_ENABLE 1
#endif

#ifndef PM_PDS_LDO_SOC_LEVEL_DEFAULT
#define PM_PDS_LDO_SOC_LEVEL_DEFAULT HBN_LDO_SOC_LEVEL_0P800V
#endif

#ifndef PM_PDS_LDO_SYS_LEVEL_DEFAULT
#define PM_PDS_LDO_SYS_LEVEL_DEFAULT HBN_LDO_SYS_LEVEL_1P300V
#endif

#ifndef PM_PDS_SET_ALL_RAM_RET_EN
#define PM_PDS_SET_ALL_RAM_RET_EN 1
#endif

#ifndef PM_HBN_FLASH_POWER_OFF
#define PM_HBN_FLASH_POWER_OFF 1
#endif

#ifndef PM_PDS_LDO18IO_POWER_DOWN
#define PM_PDS_LDO18IO_POWER_DOWN 0
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
uint8_t cacheWayDisable = 0;

/* PSRAM IO Configuration, will get from glb register */
uint32_t psramIoCfg = 0;

/* Flash offset value, will get from sf_ctrl register */
uint32_t flash_offset = 0;

spi_flash_cfg_type *flash_cfg;

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel1 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .pdsFastClkSel = 0,
        .maskIoDetBfPds = 0,
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
        .pdsLdoSocVselEn = 0,
        .wifipllOff = 1,
        .gpioDetClkSel = 0,
        .dirAccessWramEn = 0,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
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
        .ctrlUsb33 = 0,
    },
    .pdsCtl6 = {
        .pdsEisoCnt = 0,
        .pdsSelFastClk = 0,
        .ldoSocLpEn = 1,
        .ldoSysLpEn = 1,
        .wakeDisRepu = 0,
        .wakeSelFastClk = 0,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel2 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .pdsFastClkSel = 0,
        .maskIoDetBfPds = 0,
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
        .pdsLdoSocVselEn = 0,
        .wifipllOff = 1,
        .gpioDetClkSel = 0,
        .dirAccessWramEn = 0,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
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
        .cpuMemStby = 1,
        .cpuGateClk = 1,
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
        .ctrlUsb33 = 1,
    },
    .pdsCtl6 = {
        .pdsEisoCnt = 0,
        .pdsSelFastClk = 0,
        .ldoSocLpEn = 1,
        .ldoSysLpEn = 1,
        .wakeDisRepu = 0,
        .wakeSelFastClk = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel3 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .pdsFastClkSel = 0,
        .maskIoDetBfPds = 0,
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
        .pdsLdoSocVselEn = 0,
        .wifipllOff = 1,
        .gpioDetClkSel = 0,
        .dirAccessWramEn = 0,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
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
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 1,
        .wbGateClk = 1,
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
        .ctrlUsb33 = 1,
    },
    .pdsCtl6 = {
        .pdsEisoCnt = 0,
        .pdsSelFastClk = 0,
        .ldoSocLpEn = 1,
        .ldoSysLpEn = 1,
        .wakeDisRepu = 0,
        .wakeSelFastClk = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel7 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .pdsFastClkSel = 0,
        .maskIoDetBfPds = 0,
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
        .pdsLdoSocVselEn = 0,
        .wifipllOff = 1,
        .gpioDetClkSel = 0,
        .dirAccessWramEn = 0,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
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
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 0,
        .wbGateClk = 0,
        .usbPwrOff = 1,
        .usbRst = 1,
        .usbMemStby = 1,
        .usbGateClk = 1,
        .miscPwrOff = 0,
        .miscRst = 0,
        .miscMemStby = 0,
        .miscGateClk = 0,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .ctrlUsb33 = 1,
    },
    .pdsCtl6 = {
        .pdsEisoCnt = 0,
        .pdsSelFastClk = 0,
        .ldoSocLpEn = 1,
        .ldoSysLpEn = 1,
        .wakeDisRepu = 0,
        .wakeSelFastClk = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel11 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .pdsFastClkSel = 0,
        .maskIoDetBfPds = 0,
        .clkOff = 1,
        .memStby = 1,
        .glbRstProtect = 0,
        .isolation = 1,
        .waitXtalRdy = 1,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 0,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoSocVselEn = 0,
        .wifipllOff = 1,
        .gpioDetClkSel = 0,
        .dirAccessWramEn = 0,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
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
        .miscIsoEn = 1,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 1,
        .wbGateClk = 1,
        .usbPwrOff = 0,
        .usbRst = 0,
        .usbMemStby = 0,
        .usbGateClk = 0,
        .miscPwrOff = 1,
        .miscRst = 1,
        .miscMemStby = 1,
        .miscGateClk = 1,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .ctrlUsb33 = 1,
    },
    .pdsCtl6 = {
        .pdsEisoCnt = 0,
        .pdsSelFastClk = 0,
        .ldoSocLpEn = 1,
        .ldoSysLpEn = 1,
        .wakeDisRepu = 0,
        .wakeSelFastClk = 0,
    }
};

static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel15 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWiFiState = 0,
        .pdsFastClkSel = 0,
        .maskIoDetBfPds = 0,
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
        .pdsLdoSocVselEn = 0,
        .wifipllOff = 1,
        .gpioDetClkSel = 0,
        .dirAccessWramEn = 0,
        .pdsCtlRfSel = 3,
        .pdsUseTbttSlp = 0,
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
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .wbPwrOff = 1,
        .wbRst = 1,
        .wbMemStby = 1,
        .wbGateClk = 1,
        .usbPwrOff = 1,
        .usbRst = 1,
        .usbMemStby = 1,
        .usbGateClk = 1,
        .miscPwrOff = 1,
        .miscRst = 1,
        .miscMemStby = 1,
        .miscGateClk = 1,
    },
    .pdsCtl5 = {
        .cpuWfiMask = 0,
        .pdsPadOdEn = 0,
        .ctrlUsb33 = 1,
    },
    .pdsCtl6 = {
        .pdsEisoCnt = 0,
        .pdsSelFastClk = 0,
        .ldoSocLpEn = 1,
        .ldoSysLpEn = 1,
        .wakeDisRepu = 0,
        .wakeSelFastClk = 0,
    }
};

/****************************************************************************/ /**
 * @brief  Disable GPIO Keep,include PDS_IO and HBN_IO
 *
 * @param  pin: gpio number
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION pm_disable_gpio_keep(uint32_t pin)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    if (pin <= GPIO_PIN_5) {
        HBN_AON_PAD_CFG_Type aonPadCfg;
        aonPadCfg.ctrlEn = 0;
        aonPadCfg.ie = 0;
        aonPadCfg.oe = 0;
        aonPadCfg.pullUp = 0;
        aonPadCfg.pullDown = 0;
        aonPadCfg.outValue = 0;

        HBN_Aon_Pad_Cfg(pin, &aonPadCfg);
    } else if (pin < GPIO_PIN_MAX) {
        pos = pin - 6;
        /* PDS_IO latch disable */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_LAT_EN);
        tmpVal &= ~(1 << pos);
        BL_WR_REG(PDS_BASE, PDS_GPIO_LAT_EN, tmpVal);
    } else {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable PDS Wakeup Source
 *
 * @param  WakeupType: PDS Wakeup Source Type, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_TOUCH_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USBH_WKP_DET
 *           @arg PDS_WAKEUP_BY_USBH_CONN_DET
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
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
 *           @arg PDS_WAKEUP_BY_TOUCH_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USBH_WKP_DET
 *           @arg PDS_WAKEUP_BY_USBH_CONN_DET
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
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
 *           @arg PDS_WAKEUP_BY_TOUCH_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USBH_WKP_DET
 *           @arg PDS_WAKEUP_BY_USBH_CONN_DET
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
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
    struct bflb_device_s *gpio;

    if (pin >= GPIO_PIN_MAX) {
        return ERROR;
    }

    gpio = bflb_device_get_by_name("gpio");

    if (pin < 6) {
        if (ie) {
            bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN);
        } else {
            bflb_gpio_init(gpio, pin, GPIO_FLOAT | GPIO_SMT_EN);
        }

        HBN_AON_PAD_CFG_Type aonPadCfg;
        aonPadCfg.ctrlEn = 1;
        aonPadCfg.ie = ie;
        aonPadCfg.oe = 0;
        aonPadCfg.pullUp = pu;
        aonPadCfg.pullDown = pd;

        HBN_Aon_Pad_Cfg(pin, &aonPadCfg);
    } else {
        if (ie) {
            if (pu) {
                bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN);
            } else if (pd) {
                bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_PULLDOWN | GPIO_SMT_EN);
            } else {
                bflb_gpio_init(gpio, pin, GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN);
            }
        } else {
            bflb_gpio_init(gpio, pin, GPIO_ANALOG | GPIO_FLOAT | GPIO_DRV_0);
        }
        PDS_Enable_GPIO_Keep(pin);
    }

    return SUCCESS;
}

int pm_lowpower_gpio_cfg(lp_gpio_cfg_Type *gpio_cfg)
{
    uint8_t io = 0;
    uint8_t ie = 0;
    uint8_t pu = 0;
    uint8_t pd = 0;
    uint8_t unmask = 0;
    uint8_t trig_mode = 0;

    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        io = i;
        ie = (gpio_cfg->io_ie >> i) & 0x1;
        pu = (gpio_cfg->io_pu >> i) & 0x1;
        pd = (gpio_cfg->io_pd >> i) & 0x1;
        unmask = (gpio_cfg->io_wakeup_unmask >> i) & 0x1;

        if (io <= 5) {
            trig_mode = gpio_cfg->io_0_5_trig_mode;
        } else if (io < GPIO_PIN_MAX) {
            trig_mode = gpio_cfg->io_6_36_trig_mode[io - 6];
        } else {
            return -1;
        }

        if (unmask) {
            if (!ie) {
                return -1;
            }
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
 * @brief  set gpio pad int mask type in lowpower mode
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~36
 *
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type pm_set_gpio_int_mask(int pin, int int_mask)
{
    if (pin >= GPIO_PIN_MAX) {
        return ERROR;
    }

    if (pin < 6) {
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
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~36
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
    if (pin >= GPIO_PIN_MAX) {
        return ERROR;
    }

    if (pin < 6) {
        HBN_Set_Aon_Pad_IntMask(pin, int_mask);
        HBN_Set_Aon_Pad_IntMode(trig_mode);

        if (int_mask == UNMASK) {
            HBN_Hw_Pu_Pd_Cfg(DISABLE);
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_HBN_IRQ_OUT, UNMASK);
        }
    } else {
        PDS_Set_GPIO_Pad_IntMask(pin, int_mask);
        PDS_Set_GPIO_Pad_IntMode(pin, trig_mode);

        if (int_mask == UNMASK) {
            PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_PDS_GPIO_INT, UNMASK);
        }
    }
    return SUCCESS;
}

void pm_pds_mode_enter(uint32_t pds_level, uint32_t sleep_time)
{
    if ((sleep_time != 0) && (sleep_time <= PDS_WARMUP_LATENCY_CNT)) {
        return;
    }

    PM_PWR_Cfg ldosys_cfg = {
        .lp_mode_en = PM_PDS_LDO_SYS_LOWPOWER_ENABLE,
        .voltage_level = PM_PDS_LDO_SYS_LEVEL_DEFAULT,
    };

    PM_PWR_Cfg ldosoc_cfg = {
        .lp_mode_en = PM_PDS_LDO_SOC_LOWPOWER_ENABLE,
        .voltage_level = PM_PDS_LDO_SOC_LEVEL_DEFAULT,
    };

    PM_PDS_CFG_Type cfg = {
        .pdsLevel = pds_level,                          /*!< PDS level */
        .turnOffRF = PM_PDS_RF_POWER_OFF,               /*!< Wheather turn off RF */
        .powerDownFlash = PM_PDS_FLASH_POWER_OFF,       /*!< Whether power down flash */
        .ramRetEn = PM_PDS_SET_ALL_RAM_RET_EN,          /*!< Whether OCRAM Retention */
        .turnoffRC32M = ENABLE,                         /*!< Whether trun off RC32M */
        .turnoffXtal = ENABLE,                          /*!< Whether trun off XTAL */
        .turnoffWifiPLL = PM_PDS_PLL_POWER_OFF,         /*!< Whether trun off WiFi PLL */
        .pdsClkType = PM_PDS_CLK_DEFAULT_SEL,           /*!< pds_clk type */
        .pdsGpioDetClkType = PDS_GPIO_INT_DET_CLK_F32K, /*!< pds gpio det clk type */
        .ldo_sys_cfg = ldosys_cfg,                      /*!< Power Config of ldo_sys */
        .ldo_soc_cfg = ldosoc_cfg,                      /*!< Power Config of ldo_soc */
        .ldo_sys_lp_en_cnt = 0,                         /*!< delay_cnt before ldo_sys enable lowpower mode */
        .ldo_sys_exit_lp_cnt = 0,                       /*!< delay_cnt before ldo_sys exit lowpower mode */
        .ldo_soc_lp_en_cnt = 1,                         /*!< delay_cnt before ldo_soc enable lowpower mode */
        .ldo_soc_exit_lp_cnt = 1,                       /*!< delay_cnt before ldo_soc exit lowpower mode */
        .sleepTime = sleep_time,                        /*!< PDS sleep time */
        .flashCfg = NULL,                               /*!< Flash config pointer, used when power down flash */
        .preCbFun = NULL,                               /*!< Pre callback function */
        .postCbFun = NULL,                              /*!< Post callback function */
    };

    if ((cfg.pdsClkType == PM_PDS_CLK_RC32M) || (cfg.pdsClkType == PM_PDS_CLK_RC16M) || (cfg.pdsClkType == PM_PDS_CLK_RC8M)) {
        cfg.pdsGpioDetClkType = PDS_GPIO_INT_DET_CLK_RC32M;
        cfg.turnoffRC32M = DISABLE;
    } else if ((cfg.pdsClkType == PM_PDS_CLK_XTAL) || (cfg.pdsClkType == PM_PDS_CLK_XTAL_LP)) {
        cfg.pdsGpioDetClkType = PDS_GPIO_INT_DET_CLK_XTAL;
        cfg.turnoffXtal = DISABLE;
    } else {
        cfg.pdsGpioDetClkType = PDS_GPIO_INT_DET_CLK_F32K;
    }

    if (PM_PDS_LDO18IO_POWER_DOWN || cfg.powerDownFlash) {
        uint32_t flash_cfg_len;
        bflb_flash_get_cfg((uint8_t **)&cfg.flashCfg, &flash_cfg_len);
    }

    pm_pds_enable((uint32_t *)&cfg);
}

void ATTR_TCM_SECTION pm_pds_enable(uint32_t *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t sf_pin_select = 0;
    PM_PDS_CFG_Type *p = (PM_PDS_CFG_Type *)cfg;
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = NULL;
    uint32_t irq_save = 0;
    uint8_t rc32m_speed = HBN_RCM_32M;

    if ((p->sleepTime != 0) && (p->sleepTime <= PDS_WARMUP_LATENCY_CNT)) {
        return;
    }

    /* To make it simple and safe*/
    irq_save = csi_irq_save();

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
    /***********************************************************/

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
        case PM_PDS_LEVEL_11:
            pPdsCfg = &pdsCfgLevel11;
            break;
        case PM_PDS_LEVEL_15:
            pPdsCfg = &pdsCfgLevel15;
            break;
        default:
            return;
    }

    if (ENABLE == p->ldo_soc_cfg.lp_mode_en) {
        pPdsCfg->pdsCtl6.ldoSocLpEn = 1;
        pPdsCfg->pdsCtl.pdsLdoSocVselEn = 1;
        HBN_Set_Ldo_Soc_Vout_in_Lowpower(p->ldo_soc_cfg.voltage_level);
        HBN_Ctrl_Ldo_Soc_Mode_by_HW(ENABLE);
        PDS_Set_Ldo_Soc_Enter_lp_delay(1);
        PDS_Set_Ldo_Soc_Exit_lp_delay(1);
    } else {
        pPdsCfg->pdsCtl6.ldoSocLpEn = 0;
    }

    if (ENABLE == p->ldo_sys_cfg.lp_mode_en) {
        pPdsCfg->pdsCtl6.ldoSysLpEn = 1;
        HBN_Set_Ldo_Sys_Vout_in_Lowpower(p->ldo_sys_cfg.voltage_level);
        HBN_Ctrl_Ldo_Sys_Mode_by_HW(ENABLE);
        PDS_Set_Ldo_Sys_Enter_lp_delay(0);
        PDS_Set_Ldo_Sys_Exit_lp_delay(0);
    } else {
        pPdsCfg->pdsCtl6.ldoSysLpEn = 0;
    }

    if (PM_PDS_LDO18IO_POWER_DOWN || p->powerDownFlash) {
        /* get sw uasge 0 */
        // EF_Ctrl_Read_Sw_Usage(0, (uint32_t *)&tmpVal);
        tmpVal = BL_RD_WORD(0x20056000 + 0x74);
        /* get flash type */
        sf_pin_select = (tmpVal >> 5) & 0x3f;
        HBN_Power_Down_Flash((spi_flash_cfg_type *)p->flashCfg);

        /* latch flash io pad */
        if (sf_pin_select & (1 << 2)) {
            for (uint8_t i = GPIO_PIN_6; i <= GPIO_PIN_11; i++) {
                PDS_Enable_GPIO_Keep(i);
            }
        } else if (sf_pin_select & (1 << 3)) {
            for (uint8_t i = GPIO_PIN_24; i <= GPIO_PIN_29; i++) {
                PDS_Enable_GPIO_Keep(i);
            }
        } else {
        }
    }

#if PM_PDS_LDO18IO_POWER_DOWN //TO DO
    /* must power down ldo18io, Otherwise, the current is abnormal */
    GLB_Power_Down_Ldo18ioVout();
#endif

    if (p->pdsClkType == PM_PDS_CLK_F32K) {
        pPdsCfg->pdsCtl6.wakeSelFastClk = 1;
        pPdsCfg->pdsCtl6.wakeDisRepu = 0;
        pPdsCfg->pdsCtl.pdsFastClkSel = 0;
    } else if ((p->pdsClkType == PM_PDS_CLK_RC32M) || (p->pdsClkType == PM_PDS_CLK_RC16M) || (p->pdsClkType == PM_PDS_CLK_RC8M)) {
        /* RC32M/RC16M/RC8M as pds clk */
        pPdsCfg->pdsCtl6.pdsEisoCnt = 1;
        pPdsCfg->pdsCtl6.pdsSelFastClk = 1;
        pPdsCfg->pdsCtl6.wakeSelFastClk = 1;
        pPdsCfg->pdsCtl6.wakeDisRepu = 1;
    } else if ((p->pdsClkType == PM_PDS_CLK_XTAL) || (p->pdsClkType == PM_PDS_CLK_XTAL_LP)) {
        /* XTAL as pds clk */
        pPdsCfg->pdsCtl6.pdsEisoCnt = 1;
        pPdsCfg->pdsCtl6.pdsSelFastClk = 1;
        pPdsCfg->pdsCtl6.wakeSelFastClk = 1;
        pPdsCfg->pdsCtl6.wakeDisRepu = 1;
        pPdsCfg->pdsCtl.pdsFastClkSel = 1;
        pPdsCfg->pdsCtl.xtalOff = 0;
        pPdsCfg->pdsCtl.socEnbForceOn = 0;
    } else {
    }

    pPdsCfg->pdsCtl.gpioDetClkSel = p->pdsGpioDetClkType;

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);
    PDS_Pu_PLL_Enable();
    if (p->turnoffWifiPLL) {
        GLB_Power_Off_WIFIPLL();
    } else {
        pPdsCfg->pdsCtl.wifipllOff = 0;
    }

    if (p->turnoffRC32M == ENABLE) {
        pPdsCfg->pdsCtl.pdsRC32mOn = 0;
    } else {
        pPdsCfg->pdsCtl.pdsRC32mOn = 1;
        rc32m_speed = HBN_Get_RC32M_Speed();
        if ((p->pdsClkType == PM_PDS_CLK_RC8M) && (rc32m_speed != HBN_RCM_8M)) {
            HBN_Set_RC32M_Speed(HBN_RCM_8M);
        } else if ((p->pdsClkType == PM_PDS_CLK_RC16M) && (rc32m_speed != HBN_RCM_16M)) {
            HBN_Set_RC32M_Speed(HBN_RCM_16M);
        } else if ((p->pdsClkType == PM_PDS_CLK_RC32M) && (rc32m_speed != HBN_RCM_32M)) {
            HBN_Set_RC32M_Speed(HBN_RCM_32M);
        }
    }

    if (p->turnoffXtal == DISABLE) {
        pPdsCfg->pdsCtl.xtalOff = 0;
        if (p->pdsClkType == PM_PDS_CLK_XTAL_LP) {
            AON_XTAL_Enter_LowPower();
        }
    } else {
        pPdsCfg->pdsCtl.xtalOff = 1;
        if (p->pdsLevel == PM_PDS_LEVEL_15) {
            AON_Power_Off_XTAL();
        }
    }

    if (!(p->turnOffRF)) {
        pPdsCfg->pdsCtl.pdsCtlRfSel = 0;
    }

    if (p->ramRetEn) {
        pPdsCfg->pdsCtl4.miscMemStby = 1;
        pPdsCfg->pdsCtl4.wbMemStby = 1;
        PDS_Set_All_WRAM_Retention();
        PDS_Set_All_OCRAM_Retention();
    } else {
        PDS_Disable_Ctrl_Ram();
    }

    PDS_Default_Level_Config(pPdsCfg, p->sleepTime);

    __WFI();

    /******************************* Wakeup Flow *******************************/
    if (p->turnoffRC32M == DISABLE) {
        if ((rc32m_speed == HBN_RCM_8M) && (p->pdsClkType != PM_PDS_CLK_RC8M)) {
            HBN_Set_RC32M_Speed(HBN_RCM_8M);
        } else if ((rc32m_speed == HBN_RCM_16M) && (p->pdsClkType != PM_PDS_CLK_RC16M)) {
            HBN_Set_RC32M_Speed(HBN_RCM_16M);
        } else if ((rc32m_speed == HBN_RCM_32M) && (p->pdsClkType != PM_PDS_CLK_RC32M)) {
            HBN_Set_RC32M_Speed(HBN_RCM_32M);
        }
    }
    if (p->turnoffXtal == DISABLE) {
        if (p->pdsClkType == PM_PDS_CLK_XTAL_LP) {
            AON_XTAL_Exit_LowPower();
        }
    }

#if PM_PDS_LDO18IO_POWER_DOWN
    GLB_Power_Up_Ldo18ioVout();
#endif
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);

    if (PM_PDS_LDO18IO_POWER_DOWN || p->powerDownFlash) {
        if (p->pdsLevel < PM_PDS_LEVEL_2) {
            /* Init flash gpio */
            /* clear latch flash io pad */
            if (sf_pin_select & (1 << 2)) {
                for (uint8_t i = GPIO_PIN_6; i <= GPIO_PIN_11; i++) {
                    pm_disable_gpio_keep(i);
                }
            } else if (sf_pin_select & (1 << 3)) {
                for (uint8_t i = GPIO_PIN_24; i <= GPIO_PIN_29; i++) {
                    pm_disable_gpio_keep(i);
                }
            } else {
            }

            bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
            bflb_sflash_restore_from_powerdown((spi_flash_cfg_type *)p->flashCfg, 0, SF_CTRL_FLASH_BANK0);
        }
    }

    /* enable global interrupts */
    csi_irq_restore(irq_save);
    /*************************************************************************/
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
void ATTR_TCM_SECTION pm_hbn_mode_enter(uint32_t hbn_level,
                                        uint64_t sleep_time)
{
    /* To make it simple and safe*/
    __ASM volatile("csrc mstatus, 8");

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

    if (hbn_level >= PM_HBN_LEVEL_2) {
        HBN_Power_Off_RC32K();
    } else {
        HBN_Keep_On_RC32K();
    }

    /* MCU CLK SELECT RC32M */
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

#if PM_HBN_FLASH_POWER_OFF
    uint32_t flash_cfg_len;

    /* get flash config */
    bflb_flash_get_cfg((uint8_t **)&flash_cfg, &flash_cfg_len);

    HBN_Power_Down_Flash(flash_cfg);
#endif
    HBN_Power_Off_LDO18_IO();

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    HBN_Level_Select(hbn_level);

    while (1) {
        arch_delay_ms(100);
        // GLB_SW_POR_Reset();
    }
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
    bflb_irq_attach(HBN_OUT0_IRQn, (irq_callback)HBN_OUT0_IRQ, NULL);
    bflb_irq_enable(HBN_OUT0_IRQn);
}

void pm_hbn_out1_irq_register(void)
{
    bflb_irq_attach(HBN_OUT1_IRQn, (irq_callback)HBN_OUT1_IRQ, NULL);
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
    } else {
        HBN_Clear_RTC_INT();
        HBN_Clear_IRQ(HBN_INT_RTC);
        pm_irq_callback(PM_HBN_RTC_WAKEUP_EVENT);
    }
}

void ATTR_TCM_SECTION HBN_OUT1_IRQ(void)
{
    /* PIR */
    if (SET == HBN_Get_INT_State(HBN_INT_PIR)) {
        HBN_Clear_IRQ(HBN_INT_PIR);
    }
    /* BOR */
    else if (SET == HBN_Get_INT_State(HBN_INT_BOD)) {
        HBN_Clear_IRQ(HBN_INT_BOD);
        pm_irq_callback(PM_HBN_BOD_WAKEUP_EVENT);
    }
}

__WEAK void pm_irq_callback(uint32_t event)
{
    // default weak implementation
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
        uint32_t all_event = PDS_Get_All_Wakeup_Src();
        for (uint32_t event = 0; event < PM_PDS_WAKEUP_EVENT_MAX - PM_PDS_WAKEUP_EVENT_MIN; event++) {
            if ((1 << event) & all_event) {
                pm_irq_callback(PM_PDS_WAKEUP_EVENT_MIN + event);
            }
            if ((event + PM_PDS_WAKEUP_EVENT_MIN) == PM_PDS_GPIO6_36_WAKEUP_EVENT) {
                PDS_Set_All_GPIO_Pad_IntClr();
            }
        }
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
