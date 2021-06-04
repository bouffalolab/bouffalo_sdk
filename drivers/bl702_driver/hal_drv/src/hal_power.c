/**
 * @file hal_power.c
 * @brief 
 * 
 * Copyright (c) 2021 Bouffalolab team
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 * 
 */

#include "misc.h"
#include "bl702_sflash.h"
#include "bl702_sec_eng.h"
#include "bl702_ef_ctrl.h"
#include "bl702_glb.h"
#include "bl702_pds.h"
#include "bl702_clock.h"
#include "power_config.h"
#include "hal_power.h"

/**
 * @brief gating peripheral clock for power saving
 * 
 * @param enable 
 * @param clockType 
 * @return int 
 */
int lp_set_clock_gate(uint8_t enable , Clock_Gate_Type clockType){

    if(clockType == CLOCK_AHB_ALL){

        for (uint8_t i = CLOCK_AHB_UART0_GATE; i < CLOCK_AHB_ALL; i++)
        {
            GLB_AHB_Slave1_Clock_Gate(enable, i);
        }        

        return 0;
    }

    return GLB_AHB_Slave1_Clock_Gate(enable,clockType);
}

/**
 * @brief Set the gpio hz object
 * 
 *  set all gpio to high z for power saving
 * 
 */
void lp_set_all_gpio_hz(void)
{
    GLB_GPIO_Cfg_Type gpio_cfg = {
        .gpioPin = GLB_GPIO_PIN_14,
        .gpioFun = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_UP,
        .drive = 0,
        .smtCtrl = 1};
    for (GLB_GPIO_Type pin = GLB_GPIO_PIN_0; pin < GLB_GPIO_PIN_MAX; pin++)
    {
        GLB_GPIO_Init(&gpio_cfg);
        GLB_GPIO_Set_HZ(gpio_cfg.gpioPin);
    }
}

void lp_power_off_dll(void){
    GLB_Power_Off_DLL();
}
    

/**
 * @brief enter cpu into mode
 * 
 *      wfi mode will suspend CPU for power saving, if any interrupt happned CPU 
 *      will continue to run.
 * 
 * @return int 
 */
int lp_enter_wfi(void){

    HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_1P10V);

    __WFI();

    return 0;
}
/** @defgroup  Hal_Power_Global_Variables
 * 
 *  @brief PDS level config 
 *  @{
 */
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel0 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel1 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel2 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel3 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 0,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 0,
        .cpuRst                  = 0,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel4 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel5 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 3,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 0,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 0,
        .BzRst                   = 0,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel6 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 0,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 0,
        .UsbRst                  = 0,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel7 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 1,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 0,
        .MiscRst                 = 0,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel31 = {
    .pdsCtl = {
        .pdsStart                = 1,
        .sleepForever            = 0,
        .xtalForceOff            = 0,
        .saveWifiState           = 0,
        .dcdc18Off               = 1,
        .bgSysOff                = 1,
        .gpioIePuPd              = 1,
        .puFlash                 = 0,
        .clkOff                  = 1,
        .memStby                 = 1,
        .swPuFlash               = 1,
        .isolation               = 1,
        .waitXtalRdy             = 0,
        .pdsPwrOff               = 1,
        .xtalOff                 = 0,
        .socEnbForceOn           = 0,
        .pdsRstSocEn             = 0,
        .pdsRC32mOn              = 0,
        .pdsLdoVselEn            = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask             = 0,
        .ldo11Off                = 1,
        .pdsForceRamClkEn        = 0,
        .pdsLdoVol               = 0xA,
        .pdsCtlRfSel             = 2,
        .pdsCtlPllSel            = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff          = 0,
        .forceBzPwrOff           = 0,
        .forceUsbPwrOff          = 0,
        .forceCpuIsoEn           = 0,
        .forceBzIsoEn            = 0,
        .forceUsbIsoEn           = 0,
        .forceCpuPdsRst          = 0,
        .forceBzPdsRst           = 0,
        .forceUsbPdsRst          = 0,
        .forceCpuMemStby         = 0,
        .forceBzMemStby          = 0,
        .forceUsbMemStby         = 0,
        .forceCpuGateClk         = 0,
        .forceBzGateClk          = 0,
        .forceUsbGateClk         = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff         = 0,
        .forceBlePwrOff          = 0,
        .forceBleIsoEn           = 0,
        .forceMiscPdsRst         = 0,
        .forceBlePdsRst          = 0,
        .forceMiscMemStby        = 0,
        .forceBleMemStby         = 0,
        .forceMiscGateClk        = 0,
        .forceBleGateClk         = 0,
        .CpuIsoEn                = 1,
        .BzIsoEn                 = 1,
        .BleIsoEn                = 1,
        .UsbIsoEn                = 1,
        .MiscIsoEn               = 1,
    },
    .pdsCtl4 = {
        .cpuPwrOff               = 1,
        .cpuRst                  = 1,
        .cpuMemStby              = 1,
        .cpuGateClk              = 1,
        .BzPwrOff                = 1,
        .BzRst                   = 1,
        .BzMemStby               = 1,
        .BzGateClk               = 1,
        .BlePwrOff               = 1,
        .BleRst                  = 1,
        .BleMemStby              = 1,
        .BleGateClk              = 1,
        .UsbPwrOff               = 1,
        .UsbRst                  = 1,
        .UsbMemStby              = 1,
        .UsbGateClk              = 1,
        .MiscPwrOff              = 1,
        .MiscRst                 = 1,
        .MiscMemStby             = 1,
        .MiscGateClk             = 1,
        .MiscAnaPwrOff           = 1,
        .MiscDigPwrOff           = 1,
    }
};
static SPI_Flash_Cfg_Type flashCfg_Gd_Q80E_Q16E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static void (*glb_preCbFun)(void);
static void (*glb_postCbFun)(void);
/*@} end of group Hal_Power_Global_Variables */

/****************************************************************************//**
 * @brief  PDS update flash_ctrl setting
 *
 * @param  fastClock: fast clock
 *
 * @return None
 *
*******************************************************************************/
static ATTR_TCM_SECTION void PDS_Update_Flash_Ctrl_Setting(uint8_t fastClock)
{
    if(fastClock){
        GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_96M, 0);
    }else{
        GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0);
    }
    SF_Ctrl_Set_Clock_Delay(fastClock);
}

/****************************************************************************//**
 * @brief  PDS power on flash pad gpio
 *
 * @param  flashPinCfg: flash pin config
 *
 * @return None
 *
*******************************************************************************/
static ATTR_TCM_SECTION void PDS_Power_On_Flash_Pad(uint8_t flashPinCfg)
{
    switch(flashPinCfg){
        case 0:
            HBN_Set_Pad_23_28_Pullnone();
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_23,PDS_PAD_CFG_PULL_NONE);
            PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_24,PDS_PAD_CFG_PULL_NONE);
            PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_25,PDS_PAD_CFG_PULL_NONE);
            PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_26,PDS_PAD_CFG_PULL_NONE);
            PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_27,PDS_PAD_CFG_PULL_NONE);
            PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_28,PDS_PAD_CFG_PULL_NONE);
            break;
        default :
            break;
    }
    
    /* Init flash gpio */
    SF_Cfg_Init_Flash_Gpio(flashPinCfg,1);
}

/****************************************************************************//**
 * @brief  PDS power on flash
 *
 * @param  cfg: PDS app config
 *
 * @return None
 *
*******************************************************************************/
static ATTR_TCM_SECTION void PDS_Power_On_Flash(PDS_APP_CFG_Type *cfg)
{
    /* Init flash gpio */
    //SF_Cfg_Init_Flash_Gpio(0,1);

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);

    /* Restore flash */
    SFlash_Restore_From_Powerdown(cfg->flashCfg,cfg->flashContRead);
}

/****************************************************************************//**
 * @brief  power down sleep mode test function
 *
 * @param  cfg: PDS app config
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION PDS_Mode_Enter(PDS_APP_CFG_Type *cfg)
{
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = NULL;
    uint32_t tmpVal=0,tmpVal2=0;
    
    if(cfg->useXtal32k){
        HBN_32K_Sel(HBN_32K_XTAL);
    }else{
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }
    
    /* turn_off_sec_ring */
    SEC_Eng_Turn_Off_Sec_Ring();        /* E_ITEM_04 */
    
    /* disable sec_eng to reduce current which enabled by bflb_platform_init(0) */
    Sec_Eng_Trng_Disable();
    
    /* pre callback */
    if(cfg->preCbFun){
        cfg->preCbFun();
    }
    
    /* always disable HBN pin pull up/down for reduce PDS0/1/2/3/7 current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(DISABLE);
    
    /* mask/unmask HBN pin wakeup, 0x4000F014[4:3] */
    HBN_Pin_WakeUp_Mask(~(cfg->pdsAonGpioWakeupSrc));
    HBN_Aon_Pad_IeSmt_Cfg(cfg->pdsAonGpioWakeupSrc);
    if(cfg->pdsAonGpioWakeupSrc!=0){
        HBN_GPIO_INT_Enable(cfg->pdsAonGpioTrigType);
    }
    
    /* always disable and mask aon_pad_GPIO9, mask/unmask and ie_enable/ie_disable */
    tmpVal=BL_RD_REG(HBN_BASE,HBN_IRQ_MODE);
    tmpVal2=BL_GET_REG_BITS_VAL(tmpVal,HBN_PIN_WAKEUP_MASK);
    tmpVal2|=(1<<0);
    tmpVal=BL_SET_REG_BITS_VAL(tmpVal,HBN_PIN_WAKEUP_MASK,tmpVal2);
    tmpVal2=BL_GET_REG_BITS_VAL(tmpVal,HBN_REG_AON_PAD_IE_SMT);
    tmpVal2&=~(1<<0);
    tmpVal=BL_SET_REG_BITS_VAL(tmpVal,HBN_REG_AON_PAD_IE_SMT,tmpVal2);
    BL_WR_REG(HBN_BASE,HBN_IRQ_MODE,tmpVal);
    
    /* To make it simple and safe*/
    __disable_irq();
    
    if(cfg->powerDownFlash){
        HBN_Power_Down_Flash(cfg->flashCfg);
        GLB_Select_External_Flash();
    }
    
    HBN_Set_Ldo11_All_Vout(cfg->ldoLevel);
    
    if(cfg->turnOffXtal32m){
        HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);
        AON_Power_Off_XTAL();
    }
    
    if(cfg->turnOffDll){
        GLB_Power_Off_DLL();
    }
    
    /* power off PLL to reduce current  */
    if(cfg->turnoffPLL){
        GLB_Set_System_CLK(GLB_DLL_XTAL_NONE,GLB_SYS_CLK_RC32M);
        PDS_Update_Flash_Ctrl_Setting(0);
        PDS_Power_Off_PLL();
    }
    
    /* pds0-pds7 : ldo11rt_iload_sel=3 */
    /* pds31     : ldo11rt_iload_sel=1 */
    if((cfg->pdsLevel>=0)&&(cfg->pdsLevel<=7)){
        HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_25_250UA);
    }else if(cfg->pdsLevel==31){
        HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_10_100UA);
    }else{
        /* pdsLevel error */
    }
    
    switch(cfg->pdsLevel){
        case 0:
            pPdsCfg = &pdsCfgLevel0;
            break;
        case 1:
            pPdsCfg = &pdsCfgLevel1;
            break;
        case 2:
            pPdsCfg = &pdsCfgLevel2;
            break;
        case 3:
            pPdsCfg = &pdsCfgLevel3;
            break;
        case 4:
            pPdsCfg = &pdsCfgLevel4;
            break;
        case 5:
            pPdsCfg = &pdsCfgLevel5;
            break;
        case 6:
            pPdsCfg = &pdsCfgLevel6;
            break;
        case 7:
            pPdsCfg = &pdsCfgLevel7;
            break;
        case 31:
            pPdsCfg = &pdsCfgLevel31;
            break;
        default :
            /* pdsLevel error */
            while(1){
                BL702_Delay_MS(500);
            }
            break;
    }
    
    if(cfg->turnOffFlashPad){
        switch(cfg->flashPinCfg){
            case 0:
                /* turn_off_ext_flash_pin, GPIO23 - GPIO28 */
                for(uint32_t pin=23;pin<=28;pin++){
                    GLB_GPIO_Set_HZ(pin);
                }
                HBN_Set_Pad_23_28_Pullup();
                break;
            case 1:
                /* turn_off_internal_flash_pin, GPIO23 - GPIO28 */
                for(uint32_t pin=23;pin<=28;pin++){
                    GLB_GPIO_Set_HZ(pin);
                }
                break;
            case 2:
                /* turn_off_internal_flash_pin, GPIO23 - GPIO28 */
                for(uint32_t pin=23;pin<=28;pin++){
                    GLB_GPIO_Set_HZ(pin);
                }
                break;
            case 3:
                /* turn_off_ext_flash_pin, GPIO17 - GPIO22 */
                for(uint32_t pin=17;pin<=22;pin++){
                    GLB_GPIO_Set_HZ(pin);
                }
                PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_23,PDS_PAD_CFG_PULL_UP);
                PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_24,PDS_PAD_CFG_PULL_UP);
                PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_25,PDS_PAD_CFG_PULL_UP);
                PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_26,PDS_PAD_CFG_PULL_UP);
                PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_27,PDS_PAD_CFG_PULL_UP);
                PDS_Set_Pad_Config(PDS_PAD_PIN_GPIO_28,PDS_PAD_CFG_PULL_UP);
                break;
            default :
                break;
        }
        pPdsCfg->pdsCtl.puFlash = 1;       /* power switch setting, set 1 for internal flash, has no effect on ext flash */
    }
    
    if(cfg->pdsHoldGpio){
        pPdsCfg->pdsCtl.gpioIePuPd = 1;
    }else{
        pPdsCfg->pdsCtl.gpioIePuPd = 0;
    }
    
    pPdsCfg->pdsCtl.pdsLdoVol = cfg->ldoLevel;
    pPdsCfg->pdsCtl.pdsLdoVselEn = 1;
    
    if(!(cfg->turnOffRF)){
        pPdsCfg->pdsCtl.pdsCtlRfSel = 0;
    }

    /* config  ldo11soc_sstart_delay_aon =2 , cr_pds_pd_ldo11=0 to speedup ldo11soc_rdy_aon */
    AON_Set_LDO11_SOC_Sstart_Delay(0x2);

    PDS_Default_Level_Config(pPdsCfg,cfg->sleepTime);
    __WFI();        /* if(.wfiMask==0){CPU won't power down until PDS module had seen __wfi} */

    /* Wake up from PDS */
    /* post callback, mainly for GPIO or peripherals on/off */
    if(cfg->postCbFun){
        cfg->postCbFun();
    }

    if(cfg->turnoffPLL){
        GLB_Set_System_CLK(cfg->xtalType,GLB_SYS_CLK_DLL144M);
        PDS_Update_Flash_Ctrl_Setting(1);
    }

    if(cfg->turnOffFlashPad){
        PDS_Power_On_Flash_Pad(cfg->flashPinCfg);
    }

    if(cfg->powerDownFlash){
        PDS_Power_On_Flash(cfg);                    /* call GLB_Select_Internal_Flash() in this API */
    }

    /* always turn_on_sec_ring, may cost more current */
    SEC_Eng_Turn_On_Sec_Ring();     /* E_ITEM_04 */
    
    /* always enable sec_eng, may cost more current */
    Sec_Eng_Trng_Enable();
    
    HBN_Aon_Pad_IeSmt_Cfg(ENABLE);
    
    __enable_irq();
}

/**
 * @brief callback function before entry into pds 
 * 
 */
static void ATTR_TCM_SECTION PDS_Pre_CallBack(void)
{
    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_NONE,
        .drive = 0,
        .smtCtrl = 0,
    };

    /* turn off gpio except for CCI, GPIO0 - GPIO22 */
    for (GLB_GPIO_Type pin = GLB_GPIO_PIN_0; pin < GLB_GPIO_PIN_MAX; pin++)
    {

        GLB_CCI_Use_IO_0_1_2_7(DISABLE);
        GLB_CCI_Use_Jtag_Pin(DISABLE);

        /* flash pin GPIO17~GPIO22 */
        if ((pin >= GLB_GPIO_PIN_17) && (pin <= GLB_GPIO_PIN_22))
        {
            continue;
        }
        /* flash pin GPIO23~GPIO28 */
        if ((pin >= GLB_GPIO_PIN_23) && (pin <= GLB_GPIO_PIN_28))
        {
            continue;
        }
        /* DBG_TX pin */
        if (pin == GLB_GPIO_PIN_14)
        {
            continue;
        }
        gpioCfg.gpioPin = pin;
        GLB_GPIO_Init(&gpioCfg);
        GLB_GPIO_Set_HZ(gpioCfg.gpioPin);
    }

    if(glb_preCbFun != NULL)
        glb_preCbFun();
}

/**
 * @brief callback function after wake up form pds if cpu do not reset
 * 
 */
static void ATTR_TCM_SECTION PDS_Post_CallBack(void)
{
    HBN_Set_Ldo11_Rt_Vout(HBN_LDO_LEVEL_1P00V);

    if(glb_postCbFun != NULL)
        glb_postCbFun();
}
/**
 * @brief enter cpu into pds mode
 * 
 *      pds(power down sleep) mode will set CPU into sleep mode.
 *      cpu's behavior after wakeup depend on psd level 
 *      
 *      see flow table if cpu off , cpu will reset after wakeup
 * 
 *                      PD_CORE     PD_CORE_MISC_DIG    PD_CORE_MISC_ANA    PD_CORE_CPU     PD_BZ   PD_USB 
 *          PDS0          ON               ON                  ON               ON            ON      ON   
 *          PDS1          ON               ON                  ON               ON            ON      OFF  
 *          PDS2          ON               ON                  ON               ON            OFF     ON   
 *          PDS3          ON               ON                  ON               ON            OFF     OFF  
 *          PDS4          ON               ON                  ON               OFF           ON      ON   
 *          PDS5          ON               ON                  ON               OFF           ON      OFF  
 *          PDS6          ON               ON                  ON               OFF           OFF     ON   
 *          PDS7          ON               ON                  ON               OFF           OFF     OFF  
 *          PDS31         ON               OFF                 OFF              OFF           OFF     OFF  
 * 
 * 
 * now default pds level is pds31 
 * @return int 
 */
int lp_enter_pds(uint32_t sleep_time ,void (*preCbFun)(void),void (*postCbFun)(void))
{
    
    PDS_APP_CFG_Type pdsCfg = {
        .pdsLevel = 31,                                                   /*!< PDS level */
        .turnOffXtal32m = DEFAULT_LP_PDS_TURN_OFF_XTAL_32M,               /*!< Wheather turn off XTAL32M */
        .turnOffDll = DEFAULT_LP_PDS_TURN_OFF_DLL,                        /*!< Wheather turn off DLL */
        .turnOffRF = DEFAULT_LP_PDS_TURN_OFF_RF,                          /*!< Wheather turn off RF */
        .useXtal32k = DEFAULT_LP_PDS_USE_XTAL_32K,                        /*!< Wheather use xtal 32K as 32K clock source,otherwise use rc32k */
        .pdsAonGpioWakeupSrc = DEFAULT_LP_PDS_AON_GPIO_WAKE_UP_SRC,       /*!< PDS level 0-7,31 mode aon GPIO Wakeup source(HBN wakeup pin) */
        .pdsAonGpioTrigType = DEFAULT_LP_PDS_AON_GPIO_TRIG_TYPE,          /*!< PDS level 0-7,31 mode aon GPIO Triger type(HBN wakeup pin) */
        .powerDownFlash = DEFAULT_LP_PDS_FLASH_POWER_DOWN,                /*!< Whether power down flash */
        .pdsHoldGpio = DEFAULT_LP_PDS_HOLD_GPIO,                          /*!< Whether pds hold gpio level */
        .turnOffFlashPad = DEFAULT_LP_PDS_TURN_OFF_FLASH_PAD,             /*!< Whether turn off ext flash pad(GPIO17-GPIO22, GPIO23-GPIO28) */
        .flashPinCfg = 0,    
        .turnoffPLL=1,                                                  /*!< Whether trun off PLL */                                             /*!< 0 ext flash 23-28, 1 internal flash 23-28, 2 internal flash 23-28, 3 ext flash 17-22 */
        .xtalType = DEAULT_LP_XTAL_TYPE,                                  /*!< XTal type, used when user choose turn off PLL, PDS will turn on when exit PDS mode */
        .flashContRead = 0,                                               /*!< Whether enable flash continue read */
        .sleepTime = 10 * 32768,                                          /*!< PDS sleep time */
        .flashCfg = &flashCfg_Gd_Q80E_Q16E,                               /*!< Flash config pointer, used when power down flash */
        .ldoLevel = DEFAULT_LP_LDO_LEVEL,                                 /*!< LDO level */
        .preCbFun = PDS_Pre_CallBack,                                     /*!< Pre callback function */
        .postCbFun = PDS_Post_CallBack,                                   /*!< Post callback function */
    };

    BL702_Delay_MS(2000);

    glb_preCbFun  = preCbFun;
    glb_postCbFun = postCbFun;

    pdsCfg.sleepTime = sleep_time;

    HBN_Set_Ldo11_Rt_Vout(HBN_LDO_LEVEL_1P00V);

    PDS_WAKEUP_IRQHandler_Install();

    /* enable PDS interrupt to wakeup CPU (PDS1:CPU not powerdown, CPU __WFI) */
    NVIC_EnableIRQ(PDS_WAKEUP_IRQn);

    /* clear and mask PDS int */
    PDS_IntMask(PDS_INT_WAKEUP, UNMASK);
    PDS_IntMask(PDS_INT_RF_DONE, MASK);
    PDS_IntMask(PDS_INT_PLL_DONE, MASK);
    PDS_IntClear();
    
    BL702_Delay_MS(100);

    PDS_Mode_Enter(&pdsCfg);

    while(1)
    {
        //MSG("pds wakeup and running.\r\n");
        BL702_Delay_MS(1000);
    }

    return 0;
}

/****************************************************************************//**
 * @brief  Make buf word
 *
 * @param  data: Data
 *
 * @return Word data
 *
*******************************************************************************/
static uint32_t put_word_buf(uint8_t data)
{
    return (data<<24)+(data<<16)+(data<<8)+data;
}

/****************************************************************************//**
 * @brief  HBN Check RAM Data
 *
 * @param  accessDelay: delay time for access
 *
 * @return None
 *
*******************************************************************************/
static void HBN_Check_RAM(uint32_t accessDelay)
{
    uint32_t i;
    uint8_t hbnWakeup=0;
    uint32_t *hbnVal=(uint32_t *)HBN_RAM_BASE;
    uint32_t *hbnCount=(uint32_t *)(HBN_RAM_BASE+HBN_RAM_SIZE-4);

    MSG("HBN wakeup Case.Check reset to check case success\r\n");
    if(HBN_STATUS_WAKEUP_FLAG==HBN_Get_Status_Flag()){
        MSG("System wakeup from HBN\r\n");
        hbnWakeup=1;
        MSG("HBN RTC Wakeup, HBN RAM Buff: %d\r\n", *hbnCount);

        for(i=0; i<HBN_RAM_SIZE/4-1; i++){
            if(*(hbnVal+i) != put_word_buf(*hbnCount&0xFF)){
                MSG("Check fail, i: %d, hbn value: %d, hbn count: %d\r\n", i, *(hbnVal+i), *hbnCount);
                BL_CASE_FAIL;
            }
        }
        *hbnCount+=1;
        for(i=0; i<HBN_RAM_SIZE/4-1; i++){
            *(hbnVal+i) = put_word_buf(*hbnCount&0xFF);
        }
    }

    /* Delay for SWD access */
    MSG("Delay for SWD access\r\n");
    for(uint8_t i=0;i<accessDelay;i++){
        BL702_Delay_MS(1000);
    }

    if(0==hbnWakeup){
        HBN_Clear_RTC_Counter();
        *hbnCount=0;
        for(i=0; i<HBN_RAM_SIZE/4-1; i++){
            *(hbnVal+i) = put_word_buf(*hbnCount&0xFF);
        }
    }
}

/**
 * @brief enter cpu into hbn mode
 * 
 * @param sleepTime sleep time
 * @return int 
 * 
 * @note now default hbn level is hbn1
 */
int lp_enter_hbn(uint32_t sleepTime)
{
    HBN_APP_CFG_Type cfg = {
        .useXtal32k = 0,                                        /*!< Wheather use xtal 32K as 32K clock source,otherwise use rc32k */
        .sleepTime = sleepTime,                                /*!< HBN sleep time */
        .gpioWakeupSrc = DEFAULT_LP_PDS_AON_GPIO_WAKE_UP_SRC,   /*!< GPIO Wakeup source */
        .gpioTrigType = DEFAULT_LP_PDS_AON_GPIO_TRIG_TYPE,      /*!< GPIO Triger type */
        .flashCfg = NULL,                                       /*!< Flash config pointer, used when power down flash */
        .flashPinCfg = 0,                                       /*!< 0 ext flash 23-28, 1 internal flash 23-28, 2 internal flash 23-28, 3 ext flash 17-22 */
        .hbnLevel = 1,                                          /*!< HBN level */
        .ldoLevel = HBN_LDO_LEVEL_0P90V,                        /*!< LDO level */
    };

    HBN_Out0_IRQHandler_Install();
    HBN_Out1_IRQHandler_Install();
    
    //HBN_Check_RAM(0);

    /* disable aon_pad(gpio7) will cause uart0_rx irq_req, __disable_irq() for all irq_req */
    __disable_irq();
    
    HBN_Mode_Enter(&cfg);
    while (1)
    {
        BL702_Delay_MS(1000);
    }

    return 0;
}