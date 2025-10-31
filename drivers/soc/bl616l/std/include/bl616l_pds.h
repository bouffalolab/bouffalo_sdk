/**
  ******************************************************************************
  * @file    bl616l_pds.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BL616L_PDS_H__
#define __BL616L_PDS_H__

#include "pds_reg.h"
#include "glb_reg.h"
#include "bl616l_clock.h"
#include "bl616l_aon.h"
#include "bl616l_hbn.h"
#include "bl616l_common.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PDS
 *  @{
 */

/** @defgroup  PDS_Public_Types
 *  @{
 */

/** @defgroup PDS WAKEUP EVENT TYPE
  * @{
  */
#define PDS_WAKEUP_BY_PDS_TIMER               (0)
#define PDS_WAKEUP_BY_HBN_IRQ_OUT             (1)
#define PDS_WAKEUP_BY_GPIO_IRQ                (2)
#define PDS_WAKEUP_BY_PDS_GPIO_INT            (3)
#define PDS_WAKEUP_BY_TOUCH_INT               (4)
#define PDS_WAKEUP_BY_WIFI_WAKEUP             (5)
#define PDS_WAKEUP_BY_DM_SLP_IRQ              (6)
#define PDS_WAKEUP_BY_USBH_WKP_DET            (7)
#define PDS_WAKEUP_BY_USBH_CONN_DET           (8)
#define PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ    (9)
/**
  * @}
  */

/** @defgroup PDS GPIO interrupt control mode type definition
  * @{
  */
#define PDS_GPIO_INT_SYNC_FALLING_EDGE        (0)  /*!< GPIO interrupt sync mode, GPIO negedge pulse trigger interrupt */
#define PDS_GPIO_INT_SYNC_RISING_EDGE         (1)  /*!< GPIO interrupt sync mode, GPIO posedge pulse trigger interrupt */
#define PDS_GPIO_INT_SYNC_LOW_LEVEL           (2)  /*!< Not support. GPIO interrupt sync mode, GPIO negedge level trigger interrupt */
#define PDS_GPIO_INT_SYNC_HIGH_LEVEL          (3)  /*!< GPIO interrupt sync mode, GPIO posedge level trigger interrupt */
#define PDS_GPIO_INT_SYNC_RISING_FALLING_EDGE (4)  /*!< Not support. GPIO interrupt sync mode, GPIO posedge negedge pulse trigger interrupt */
#define PDS_GPIO_INT_ASYNC_FALLING_EDGE       (8)  /*!< GPIO interrupt async mode, GPIO negedge pulse trigger interrupt */
#define PDS_GPIO_INT_ASYNC_RISING_EDGE        (9)  /*!< GPIO interrupt async mode, GPIO posedge pulse trigger interrupt */
#define PDS_GPIO_INT_ASYNC_LOW_LEVEL          (10) /*!< Not support. GPIO interrupt async mode, GPIO negedge level trigger interrupt */
#define PDS_GPIO_INT_ASYNC_HIGH_LEVEL         (11) /*!< GPIO interrupt async mode, GPIO posedge level trigger interrupt */
/**
  * @}
  */

/** @defgroup PDS GPIO INT DET clock type definition
  * @{
  */
#define PDS_GPIO_INT_DET_CLK_F32K             (0) /*!< Select F32K as PDS_GPIO_INT_DET clock */
#define PDS_GPIO_INT_DET_CLK_XTAL             (1) /*!< Select XTAL as PDS_GPIO_INT_DET clock */
#define PDS_GPIO_INT_DET_CLK_RC32M            (2) /*!< Select RC32M as PDS_GPIO_INT_DET clock */
/**
  * @}
  */

/**
 *  @brief PDS configuration type definition
 */
typedef struct
{
    uint32_t pdsStart        : 1; /*!< [0]PDS Start */
    uint32_t sleepForever    : 1; /*!< [1]PDS sleep forever */
    uint32_t xtalForceOff    : 1; /*!< [2]Power off xtal force */
    uint32_t saveWiFiState   : 1; /*!< [3]Save WIFI State Before Enter PDS */
    uint32_t pdsFastClkSel   : 1; /*!< [4]select pds fast clk, 0:rc32m, 1:xtal */
    uint32_t maskIoDetBfPds  : 1; /*!< [5]enable mask pds gpio int det when pds not sleep */
    uint32_t rsvd6_7         : 2; /*!< [7:6]reserved */
    uint32_t clkOff          : 1; /*!< [8]gate clock during PDS (each pwr domain has its own control) */
    uint32_t memStby         : 1; /*!< [9]mem_stby during PDS (each power domain can has its own control) */
    uint32_t glbRstProtect   : 1; /*!< [10]avoid glb_reg reset by any reset */
    uint32_t isolation       : 1; /*!< [11]Isolation during PDS (each power domain can has its own control) */
    uint32_t waitXtalRdy     : 1; /*!< [12]wait XTAL Ready during before PDS Interrupt */
    uint32_t pdsPwrOff       : 1; /*!< [13]Power off during PDS (each power domain can has its own control) */
    uint32_t xtalOff         : 1; /*!< [14]xtal power down during PDS */
    uint32_t socEnbForceOn   : 1; /*!< [15]pds_soc_enb always active */
    uint32_t pdsRstSocEn     : 1; /*!< [16]pds_rst controlled by PDS */
    uint32_t pdsRC32mOn      : 1; /*!< [17]RC32M always on or RC32M on/off controlled by PDS state */
    uint32_t pdsLdoSocVselEn : 1; /*!< [18]PDS "SLEEP" control LDO_SOC voltage enable */
    uint32_t rsvd19_21       : 3; /*!< [21：19]reserved */
    uint32_t wifipllOff      : 1; /*!< [22]PDS control wifipll power off */
    uint32_t rsvd23_24       : 2; /*!< [24:23]reserved */
    uint32_t gpioDetClkSel   : 2; /*!< [26:25]gpio detect clock, 0:fclk, 1:xtal, 2/3:rc32m */
    uint32_t dirAccessWramEn : 1; /*!< [27]cpu and other masters access wram directly，when wifi is power off */
    uint32_t pdsCtlRfSel     : 2; /*!< [29:28]select the way RF controlled by PDS */
    uint32_t pdsUseTbttSlp   : 1; /*!< [30]PDS Auto Sleep When wifi_tbtt_sleep_irq detect to 1 */
    uint32_t rsvd31          : 1; /*!< [31]reserved */
} PDS_CTL_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t cpuPwrOff   : 1; /*!< [0] */
    uint32_t cpuRst      : 1; /*!< [1] */
    uint32_t cpuMemStby  : 1; /*!< [2] */
    uint32_t cpuGateClk  : 1; /*!< [3] */
    uint32_t rsv4_11     : 8; /*!< [11:4]reserve */
    uint32_t wbPwrOff    : 1; /*!< [12] */
    uint32_t wbRst       : 1; /*!< [13] */
    uint32_t wbMemStby   : 1; /*!< [14] */
    uint32_t wbGateClk   : 1; /*!< [15] */
    uint32_t rsv16_19    : 4; /*!< [19:16]reserve */
    uint32_t usbPwrOff   : 1; /*!< [20] */
    uint32_t usbRst      : 1; /*!< [21] */
    uint32_t usbMemStby  : 1; /*!< [22] */
    uint32_t usbGateClk  : 1; /*!< [23] */
    uint32_t miscPwrOff  : 1; /*!< [24] */
    uint32_t miscRst     : 1; /*!< [25] */
    uint32_t miscMemStby : 1; /*!< [26] */
    uint32_t miscGateClk : 1; /*!< [27] */
    uint32_t rsv28_31    : 4; /*!< [31:28]reserve */
} PDS_CTL4_Type;

/** @defgroup PDS interrupt type definition
  * @{
  */
#define PDS_INT_WAKEUP           (0) /*!< PDS wakeup interrupt(assert bit while wakeup, include PDS_Timer/...) */
#define PDS_INT_RF_DONE          (1) /*!< PDS RF done interrupt */
#define PDS_INT_WIFI_TBTT_SLEEP  (2) /*!< PDS wifi tbtt sleep interrupt */
#define PDS_INT_WIFI_TBTT_WAKEUP (3) /*!< PDS wifi tbtt wakeup interrupt */
#define PDS_INT_MAX              (4) /*!< PDS int max number */
/**
  * @}
  */

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t forceCpuPwrOff  : 1;  /*!< [0]manual force NP power off */
    uint32_t rsv1            : 1;  /*!< [1]reserved */
    uint32_t forceWbPwrOff   : 1;  /*!< [2]manual force WB power off */
    uint32_t forceUsbPwrOff  : 1;  /*!< [3]manual force USB power off */
    uint32_t forceCpuIso     : 1;  /*!< [4]manual force NP isolation */
    uint32_t rsv5            : 1;  /*!< [5]reserved */
    uint32_t forceWbIso      : 1;  /*!< [6]manual force WB isolation */
    uint32_t forceUsbIso     : 1;  /*!< [7]manual force USB isolation */
    uint32_t forceCpuPdsRst  : 1;  /*!< [8]manual force NP pds reset */
    uint32_t rsv9            : 1;  /*!< [9]reserved */
    uint32_t forceWbPdsRst   : 1;  /*!< [10]manual force WB pds reset */
    uint32_t forceUsbPdsRst  : 1;  /*!< [11]manual force USB pds reset */
    uint32_t forceCpuMemStby : 1;  /*!< [12]manual force NP memory sleep */
    uint32_t rsv13           : 1;  /*!< [13]reserved */
    uint32_t forceWbMemStby  : 1;  /*!< [14]manual force WB memory sleep */
    uint32_t forceUsbMemStby : 1;  /*!< [15]manual force USB memory sleep */
    uint32_t forceCpuGateClk : 1;  /*!< [16]manual force NP clock gated */
    uint32_t rsv17           : 1;  /*!< [17]reserved */
    uint32_t forceWbGateClk  : 1;  /*!< [18]manual force WB clock gated */
    uint32_t forceUsbGateClk : 1;  /*!< [19]manual force USB clock gated */
    uint32_t rsv20_31        : 12; /*!< [31:20]reserve */
} PDS_CTL2_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t rsv0             : 1;  /*!< [0]reserve */
    uint32_t forceMiscPwrOff  : 1;  /*!< [1]manual force MISC pwr_off */
    uint32_t rsv2_3           : 2;  /*!< [3:2]reserve */
    uint32_t forceMiscIsoEn   : 1;  /*!< [4]manual force MISC iso_en */
    uint32_t rsv5_6           : 2;  /*!< [6:5]reserve */
    uint32_t forceMiscPdsRst  : 1;  /*!< [7]manual force MISC pds_rst */
    uint32_t rsv8_9           : 2;  /*!< [9:8]reserve */
    uint32_t forceMiscMemStby : 1;  /*!< [10]manual force MISC mem_stby */
    uint32_t rsv11_12         : 2;  /*!< [12:11]reserve */
    uint32_t forceMiscGateClk : 1;  /*!< [13]manual force MISC gate_clk */
    uint32_t rsv14_23         : 10; /*!< [23:14]reserve */
    uint32_t cpuIsoEn         : 1;  /*!< [24]make CPU isolated at PDS Sleep state */
    uint32_t rsv25_26         : 2;  /*!< [26:25]reserve */
    uint32_t wbIsoEn          : 1;  /*!< [27]make WB isolated at PDS Sleep state */
    uint32_t rsv28            : 1;  /*!< [28]reserve */
    uint32_t usbIsoEn         : 1;  /*!< [29]make USB isolated at PDS Sleep state */
    uint32_t miscIsoEn        : 1;  /*!< [30]make misc isolated at PDS Sleep state */
    uint32_t rsv31            : 1;  /*!< [31]reserve */
} PDS_CTL3_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t cpuWfiMask : 1;  /*!< [0]pds start condition mask np_wfi */
    uint32_t rsv1_3     : 3;  /*!< [3：1]reserve */
    uint32_t pdsPadOdEn : 4;  /*!< [7:4]GPIO21/22/28/29 5V Tolerant PAD Open Drain Enable */
    uint32_t ctrlUsb33  : 1;  /*!< [8]HW control turn on/off USB 3.3V @USB1.1V Power On/OFF */
    uint32_t rsv9_31    : 23; /*!<[31:9]reserve */
} PDS_CTL5_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t pdsEisoCnt    : 16; /*!< [15:0]number of pds_clk number minus 1 when pds sleep use rc32m */
    uint32_t pdsSelFastClk : 1;  /*!< [16]whether use rc32m\XTAL for pds_clk when pds sleep */
    uint32_t ldoSocLpEn    : 1;  /*!< [17]whether use ldo_soc low power mode when pds sleep */
    uint32_t ldoSysLpEn    : 1;  /*!< [18]whether use ldo13 low power mode when pds sleep */
    uint32_t wakeDisRepu   : 1;  /*!< [19]disable repeat pu again when fast clock is alive */
    uint32_t wakeSelFastClk: 1;  /*!< [20]pds_clk switch to fast_clk firstly when wakeup */
    uint32_t rsv21_31      : 11; /*!< [31:21]reserve */
} PDS_CTL6_Type;

/**
 *  @brief PDS default level configuration type definition
 */
typedef struct
{
    PDS_CTL_Type pdsCtl;   /*!< PDS_CTL configuration */
    PDS_CTL2_Type pdsCtl2; /*!< PDS_CTL2 configuration */
    PDS_CTL3_Type pdsCtl3; /*!< PDS_CTL3 configuration */
    PDS_CTL4_Type pdsCtl4; /*!< PDS_CTL4 configuration */
    PDS_CTL5_Type pdsCtl5; /*!< PDS_CTL5 configuration */
    PDS_CTL6_Type pdsCtl6; /*!< PDS_CTL6 configuration */
} PDS_DEFAULT_LV_CFG_Type;

/**
 *  @brief PDS control RAM1 type definition
 */
typedef struct
{
    uint32_t rsv0_7     : 8; /*!< [7:0] reserve */
    uint32_t ramClkCnt  : 6; /*!< [13:8] HW Option : Assert Extra Clock Counter in  MEM_STBY */
    uint32_t rsv14_15   : 2; /*!< [15:14] reserved */
    uint32_t ramClkCnt2 : 6; /*!< [21:16] HW Option : Assert Extra Clock Counter in MEM_IDLE/LV_MEM_IDLE */
    uint32_t rsv22_23   : 2; /*!< [23:22] reserved */
    uint32_t cpuRamClk  : 1; /*!< [24] PDS Control PD_CORE_CPU SRAM Clock */
    uint32_t rsv25      : 1; /*!< [25] reserved */
    uint32_t wbRamClk   : 1; /*!< [26] PDS Control PD_WB SRAM Clock */
    uint32_t usbRamClk  : 1; /*!< [27] PDS Control PD_usb SRAM Clock */
    uint32_t miscRamClk : 1; /*!< [28] PDS Control PD_CORE_MISC SRAM Clock */
    uint32_t rsv29      : 1; /*!< [29] reserved */
    uint32_t ctlRamClk2 : 1; /*!< [30] To assert extra clock during PDS on sequence */
    uint32_t ctlRamClk  : 1; /*!< [31] Enable PDS Control PD_CORE SRAM Clock */
} PDS_CTRL_RAM1_Type;

/**
 *  @brief PDS control RAM2 type definition
 */
typedef struct
{
    uint32_t wramSlp  : 10; /*!< [9:0] cr_wram_slp */
    uint32_t wramRet  : 10; /*!< [19:10] cr_wram_ret */
    uint32_t rsv20_31 : 12; /*!< [31:20] reserved */
} PDS_CTRL_RAM2_Type;

/**
 *  @brief PDS control RAM3 type definition
 */
typedef struct
{
    uint32_t cr_ocram_ret   : 20; /* [19: 0],        r/w,        0x0 */
    uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
} PDS_CTRL_RAM3_Type;

/**
 *  @brief PDS control RAM4 type definition
 */
typedef struct
{
    uint32_t cr_ocram_slp   : 20; /* [19: 0],        r/w,        0x0 */
    uint32_t reserved_20_31 : 12; /* [31:20],       rsvd,        0x0 */
} PDS_CTRL_RAM4_Type;

typedef struct
{
    PDS_CTRL_RAM1_Type *pds_ram1; /*!< PDS_RAM1 configuration */
    PDS_CTRL_RAM2_Type *pds_ram2; /*!< PDS_RAM2 configuration */
    PDS_CTRL_RAM3_Type *pds_ram3; /*!< PDS_RAM3 configuration */
    PDS_CTRL_RAM4_Type *pds_ram4; /*!< PDS_RAM4 configuration */
} PDS_RAM_CFG_Type;

/**
 *  @brief PDS OCRAM configuration type definition
 */
typedef struct
{
    uint32_t PDS_OCRAM_CFG_0KB_16KB_CPU_RAM_SLP  : 1;  /*!< [0]   0~16KB cpu_ram sleep */
    uint32_t PDS_OCRAM_CFG_16KB_32KB_CPU_RAM_SLP : 1;  /*!< [1]   16~32KB cpu_ram sleep */
    uint32_t PDS_OCRAM_CFG_32KB_48KB_CPU_RAM_SLP : 1;  /*!< [2]   32~48KB cpu_ram sleep */
    uint32_t PDS_OCRAM_CFG_48KB_64KB_CPU_RAM_SLP : 1;  /*!< [3]   48~64KB cpu_ram sleep */
    uint32_t PDS_OCRAM_CFG_0KB_16KB_CPU_RAM_RET  : 1;  /*!< [4]   0~16KB cpu_ram retension */
    uint32_t PDS_OCRAM_CFG_16KB_32KB_CPU_RAM_RET : 1;  /*!< [5]   16~32KB cpu_ram retension */
    uint32_t PDS_OCRAM_CFG_32KB_48KB_CPU_RAM_RET : 1;  /*!< [6]   32~48KB cpu_ram retension */
    uint32_t PDS_OCRAM_CFG_48KB_64KB_CPU_RAM_RET : 1;  /*!< [7]   48~64KB cpu_ram retension */
    uint32_t PDS_OCRAM_CFG_RSV                   : 24; /*!< [31:8]reserve */
} PDS_OCRAM_CFG_Type;

/**
 *  @brief PDS WRAM configuration type definition
 */
typedef struct
{
    uint32_t PDS_WRAM_CFG_0KB_16KB_CPU_RAM_SLP    : 1;  /*!< [0]   0~16KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_16KB_32KB_CPU_RAM_SLP   : 1;  /*!< [1]   16~32KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_32KB_48KB_CPU_RAM_SLP   : 1;  /*!< [2]   32~48KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_48KB_64KB_CPU_RAM_SLP   : 1;  /*!< [3]   48~64KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_64KB_80KB_CPU_RAM_SLP   : 1;  /*!< [4]   64~80KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_80KB_96KB_CPU_RAM_SLP   : 1;  /*!< [5]   80~96KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_96KB_112KB_CPU_RAM_SLP  : 1;  /*!< [6]   96~112KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_112KB_128KB_CPU_RAM_SLP : 1;  /*!< [7]   112~128KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_128KB_144KB_CPU_RAM_SLP : 1;  /*!< [8]   128~144KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_144KB_160KB_CPU_RAM_SLP : 1;  /*!< [9]   144~160KB cpu_ram sleep */
    uint32_t PDS_WRAM_CFG_0KB_16KB_CPU_RAM_RET    : 1;  /*!< [10]   0~16KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_16KB_32KB_CPU_RAM_RET   : 1;  /*!< [11]   16~32KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_32KB_48KB_CPU_RAM_RET   : 1;  /*!< [12]   32~48KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_48KB_64KB_CPU_RAM_RET   : 1;  /*!< [13]   48~64KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_64KB_80KB_CPU_RAM_RET   : 1;  /*!< [14]   64~80KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_80KB_96KB_CPU_RAM_RET   : 1;  /*!< [15]   80~96KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_96KB_112KB_CPU_RAM_RET  : 1;  /*!< [16]   96~112KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_112KB_128KB_CPU_RAM_RET : 1;  /*!< [17]   112~128KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_128KB_144KB_CPU_RAM_RET : 1;  /*!< [18]   128~144KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_144KB_160KB_CPU_RAM_RET : 1;  /*!< [19]   144~160KB cpu_ram retension */
    uint32_t PDS_WRAM_CFG_RSV                     : 22; /*!< [31:20]reserve */
} PDS_WRAM_CFG_Type;

/**
 *  @brief PDS APP configuration type definition
 */
typedef struct
{
    uint8_t pdsLevel;             /*!< PDS level */
    uint8_t turnOffRF;            /*!< Wheather turn off RF */
    uint8_t useXtal32k;           /*!< Wheather use xtal 32K as 32K clock source,otherwise use rc32k */
    uint8_t pdsAonGpioWakeupSrc;  /*!< PDS level 0/1/2/3 mode always on GPIO Wakeup source(HBN wakeup pin) */
    uint8_t pdsAonGpioTrigType;   /*!< PDS level 0/1/2/3 mode always on GPIO Triger type(HBN wakeup pin) */
    uint8_t powerDownFlash;       /*!< Whether power down flash */
    uint8_t turnOffFlashPad;      /*!< Whether turn off embedded flash pad */
    uint8_t ocramRetetion;        /*!< Whether OCRAM Retention */
    uint8_t turnoffPLL;           /*!< Whether trun off PLL */
    uint8_t xtalType;             /*!< XTal type, used when user choose turn off PLL, PDS will turn on when exit PDS mode */
    uint8_t flashContRead;        /*!< Whether enable flash continue read */
    uint32_t sleepTime;           /*!< PDS sleep time */
    spi_flash_cfg_type *flashCfg; /*!< Flash config pointer, used when power down flash */
    uint8_t ldoLevel;             /*!< LDO level */
    void (*preCbFun)(void);       /*!< Pre callback function */
    void (*postCbFun)(void);      /*!< Post callback function */
} PDS_APP_CFG_Type;

/*@} end of group PDS_Public_Types */

/** @defgroup  PDS_Public_Constants
 *  @{
 */
/** @defgroup  PDS_WAKEUP_EVENT_TYPE
 *  @{
 */
#define IS_PDS_WAKEUP_EVENT_TYPE(type) (((type) == PDS_WAKEUP_BY_PDS_TIMER) ||     \
                                        ((type) == PDS_WAKEUP_BY_HBN_IRQ_OUT) ||   \
                                        ((type) == PDS_WAKEUP_BY_GPIO_IRQ) ||      \
                                        ((type) == PDS_WAKEUP_BY_PDS_GPIO_INT) ||  \
                                        ((type) == PDS_WAKEUP_BY_TOUCH_INT) ||     \
                                        ((type) == PDS_WAKEUP_BY_WIFI_WAKEUP) ||   \
                                        ((type) == PDS_WAKEUP_BY_DM_SLP_IRQ) ||    \
                                        ((type) == PDS_WAKEUP_BY_USBH_WKP_DET) ||  \
                                        ((type) == PDS_WAKEUP_BY_USBH_CONN_DET) || \
                                        ((type) == PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ))

/** @defgroup  GLB_GPIO_INT_TRIG_TYPE
 *  @{
 */
#define IS_PDS_GPIO_INT_TRIG_TYPE(type) (((type) == PDS_GPIO_INT_SYNC_FALLING_EDGE) ||        \
                                         ((type) == PDS_GPIO_INT_SYNC_RISING_EDGE) ||         \
                                         ((type) == PDS_GPIO_INT_SYNC_LOW_LEVEL) ||           \
                                         ((type) == PDS_GPIO_INT_SYNC_HIGH_LEVEL) ||          \
                                         ((type) == PDS_GPIO_INT_SYNC_RISING_FALLING_EDGE) || \
                                         ((type) == PDS_GPIO_INT_ASYNC_FALLING_EDGE) ||       \
                                         ((type) == PDS_GPIO_INT_ASYNC_RISING_EDGE) ||        \
                                         ((type) == PDS_GPIO_INT_ASYNC_LOW_LEVEL) ||          \
                                         ((type) == PDS_GPIO_INT_ASYNC_HIGH_LEVEL))

/** @defgroup  PDS_GPIO_INT_DET_CLK_TYPE
 *  @{
 */
#define IS_PDS_GPIO_INT_DET_CLK_TYPE(type) (((type) == PDS_GPIO_INT_DET_CLK_F32K) || \
                                            ((type) == PDS_GPIO_INT_DET_CLK_XCLK) || \
                                            ((type) == PDS_GPIO_INT_DET_CLK_RC32M))

/** @defgroup  PDS_INT_TYPE
 *  @{
 */
#define IS_PDS_INT_TYPE(type) (((type) == PDS_INT_WAKEUP) ||           \
                               ((type) == PDS_INT_RF_DONE) ||          \
                               ((type) == PDS_INT_WIFI_TBTT_SLEEP) ||  \
                               ((type) == PDS_INT_WIFI_TBTT_WAKEUP) || \
                               ((type) == PDS_INT_MAX))

/*@} end of group PDS_Public_Constants */

/** @defgroup  PDS_Public_Macros
 *  @{
 */
#define PDS_LDO_MIN_PU_CNT      (25) /* LDO need 25 cycles to power up */
#define PDS_WARMUP_LATENCY_CNT  (38) /* LDO hw warmup compensation latency cycles */
#define PDS_INT_MASK_BIT_OFFSET (4)

/*@} end of group PDS_Public_Macros */

/** @defgroup  PDS_Public_Functions
 *  @{
 */
BL_Err_Type PDS_Set_GPIO_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask);
BL_Err_Type PDS_Set_GPIO_Pad_IntMode(uint8_t pad, uint8_t trig);
BL_Err_Type PDS_Set_GPIO_Pad_IntClr(uint8_t pad);
BL_Sts_Type PDS_Get_GPIO_Pad_IntStatus(uint8_t pad);
BL_Err_Type PDS_Set_All_GPIO_Pad_IntClr(void);
uint32_t PDS_Get_All_GPIO_Pad_IntStatus(void);
BL_Err_Type PDS_Disable_ALL_GPIO_Keep(void);
BL_Err_Type PDS_Set_PDS_GPIO_INT_DET_CLK_Sel(uint8_t clkSel);
BL_Err_Type PDS_Enable_GPIO_Keep(uint32_t pin);
BL_Err_Type PDS_Enable_WRAM_Access_Direct(void);
BL_Err_Type PDS_Disable_WRAM_Access_Direct(void);
BL_Err_Type PDS_Set_Ldo_Soc_Enter_lp_delay(uint16_t cnt);
BL_Err_Type PDS_Set_Ldo_Soc_Exit_lp_delay(uint16_t cnt);
BL_Err_Type PDS_Set_Ldo_Sys_Enter_lp_delay(uint16_t cnt);
BL_Err_Type PDS_Set_Ldo_Sys_Exit_lp_delay(uint16_t cnt);
BL_Err_Type PDS_Set_MCU0_Clock_Enable(void);
BL_Err_Type PDS_Set_MCU0_Clock_Disable(void);
BL_Err_Type PDS_Pu_PLL_Enable(void);
BL_Err_Type PDS_Pu_PLL_Disable(void);
BL_Err_Type PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt);
BL_Err_Type PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5, PDS_CTL6_Type *cfg6);
BL_Err_Type PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg,
                           PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg);
BL_Err_Type PDS_Set_All_WRAM_Retention(void);
BL_Err_Type PDS_Set_All_OCRAM_Retention(void);
BL_Err_Type PDS_Set_All_WRAM_Sleep(void);
BL_Err_Type PDS_Set_All_OCRAM_Sleep(void);
BL_Err_Type PDS_Disable_Ctrl_Ram(void);
BL_Err_Type PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt);
BL_Err_Type PDS_IntMask(uint8_t intType, BL_Mask_Type intMask);
BL_Sts_Type PDS_Get_IntStatus(uint8_t intType);
BL_Err_Type PDS_IntClear(void);
BL_Err_Type PDS_Set_Wakeup_Src_IntMask(uint32_t WakeupType, BL_Mask_Type intMask);
BL_Sts_Type PDS_Get_Wakeup_Src(uint32_t WakeupType);
uint32_t PDS_Get_All_Wakeup_Src(void);
void PDS_Mask_All_Wakeup_Src(void);
BL_Err_Type PDS_Power_Off_WB(void);
BL_Err_Type PDS_Power_On_WB(void);
BL_Err_Type PDS_Set_USB_Suspend(void);
BL_Err_Type PDS_Set_USB_Resume(void);
BL_Err_Type PDS_Trim_USB20(void);

/*@} end of group PDS_Public_Functions */

/*@} end of group PDS */

/*@} end of group BL616L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616L_PDS_H__ */
