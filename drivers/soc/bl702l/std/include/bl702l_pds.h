/**
  ******************************************************************************
  * @file    bl702l_pds.h
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
#ifndef __BL702L_PDS_H__
#define __BL702L_PDS_H__

#include "pds_reg.h"
#include "bl702l_aon.h"
#include "bl702l_hbn.h"
#include "bl702l_common.h"
#include "bflb_sflash.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PDS
 *  @{
 */

/** @defgroup  PDS_Public_Types
 *  @{
 */

/**
 *  @brief PDS GPIO configuration type definition
 */
typedef struct
{
    uint8_t pd_en;
    uint8_t pu_en;
    uint8_t ie_en;
    uint8_t oe_en;
} PDS_GPIO_Cfg_Type;

/**
 *  @brief PDS RAM configuration type definition
 */
typedef struct
{
    uint32_t PDS_RAM_CFG_0KB_16KB_CPU_RAM_RET1N  : 1;  /*!< [0] 0~16KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_16KB_32KB_CPU_RAM_RET1N : 1;  /*!< [1] 16~32KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_32KB_48KB_CPU_RAM_RET1N : 1;  /*!< [2] 32~48KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_48KB_64KB_CPU_RAM_RET1N : 1;  /*!< [3] 48~64KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_64KB_80KB_CPU_RAM_RET1N : 1;  /*!< [4] 64~80KB cpu_ram RET1N */
    uint32_t PDS_RAM_CFG_RSV_5_7                 : 3;  /*!< [7:5]reserve */
    uint32_t PDS_RAM_CFG_0KB_16KB_CPU_RAM_RET2N  : 1;  /*!< [8] 0~16KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_16KB_32KB_CPU_RAM_RET2N : 1;  /*!< [9] 16~32KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_32KB_48KB_CPU_RAM_RET2N : 1;  /*!< [10] 32~48KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_48KB_64KB_CPU_RAM_RET2N : 1;  /*!< [11] 48~64KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_64KB_80KB_CPU_RAM_RET2N : 1;  /*!< [12] 64~80KB cpu_ram RET2N */
    uint32_t PDS_RAM_CFG_RSV_13_15               : 3;  /*!< [15:13]reserve */
    uint32_t PDS_RAM_CFG_0KB_16KB_CPU_RAM_PGEN   : 1;  /*!< [16] 0~16KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_16KB_32KB_CPU_RAM_PGEN  : 1;  /*!< [17] 16~32KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_32KB_48KB_CPU_RAM_PGEN  : 1;  /*!< [18] 32~48KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_48KB_64KB_CPU_RAM_PGEN  : 1;  /*!< [19] 48~64KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_64KB_80KB_CPU_RAM_PGEN  : 1;  /*!< [20] 64~80KB cpu_ram PGEN */
    uint32_t PDS_RAM_CFG_RSV_21_31               : 11; /*!< [31:21]reserve */
} PDS_RAM_CFG_Type;

/**
 *  @brief PDS configuration type definition
 */
typedef struct
{
    uint32_t pdsStart       : 1; /*!< [0]PDS Start */
    uint32_t sleepForever   : 1; /*!< [1]PDS sleep forever */
    uint32_t xtalForceOff   : 1; /*!< [2]Power off xtal force */
    uint32_t waitRC32mRdy   : 1; /*!< [3]wait XTAL Ready during before PDS Interrupt */
    uint32_t avdd14Off      : 1; /*!< [4]power down avdd14 during PDS */
    uint32_t bgSysOff       : 1; /*!< [5]power down bg_sys during PDS */
    uint32_t glbRstProtect  : 1; /*!< [6]avoid glb_reg reset by any reset */
    uint32_t puFlash        : 1; /*!< [7]turn off Flash Power During PDS */
    uint32_t clkOff         : 1; /*!< [8]gate clock during PDS (each pwr domain has its own control) */
    uint32_t memStby        : 1; /*!< [9]mem_stby during PDS (each power domain can has its own control) */
    uint32_t swPuFlash      : 1; /*!< [10]SW Turn on Flash */
    uint32_t isolation      : 1; /*!< [11]Isolation during PDS (each power domain can has its own control) */
    uint32_t waitXtalRdy    : 1; /*!< [12]wait XTAL Ready during before PDS Interrupt */
    uint32_t pdsPwrOff      : 1; /*!< [13]Power off during PDS (each power domain can has its own control) */
    uint32_t xtalOff        : 1; /*!< [14]xtal power down during PDS */
    uint32_t socEnbForceOn  : 1; /*!< [15]pds_soc_enb always active */
    uint32_t pdsRstSocEn    : 1; /*!< [16]pds_rst controlled by PDS */
    uint32_t pdsRC32mOn     : 1; /*!< [17]RC32M always on or RC32M on/off controlled by PDS state */
    uint32_t pdsLdoVselEn   : 1; /*!< [18]PDS "SLEEP" control LDO voltage enable */
    uint32_t rsv19          : 1; /*!< [19]Reserved */
    uint32_t xtalCntRC32kEn : 1; /*!< [20]HW Auto count RC32K to be XTAL Counter after PDS */
    uint32_t cpu0WfiMask    : 1; /*!< [21]pds start condition mask np_wfi */
    uint32_t ldo11Off       : 1; /*!< [22]power down ldo11 during PDS */
    uint32_t pdsCtlRfSel    : 1; /*!< [23]PDS control RF on/off */
    uint32_t xtalDegEn      : 1; /*!< [24]Enable Digital Deg */
    uint32_t bleWakeupReqEn : 1; /*!< [25]PDS auto send ble_wakeup_req to sleep_timer when exit PDS */
    uint32_t swGpioIsoMod   : 1; /*!< [26]SW force keep GPIO */
    uint32_t pdsClkOff      : 1; /*!< [27]Turn off PDS Clock , Set this bit to 0 to active PDS */
    uint32_t pdsGpioKeep    : 3; /*!< [30:28]enable or disable keep function */
    uint32_t pdsGpioIsoMod  : 1; /*!< [31]HW Keep GPIO at PDS Mode  */
} PDS_CTL_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t rsv0_7          : 8;  /*!< [7:0]reserve */
    uint32_t forceCpuPdsRst  : 1;  /*!< [8]manual force NP pds reset */
    uint32_t rsv9            : 1;  /*!< [9]reserve */
    uint32_t forceBzPdsRst   : 1;  /*!< [10]manual force BZ pds reset */
    uint32_t rsv11_15        : 5;  /*!< [15:11]reserve */
    uint32_t forceCpuGateClk : 1;  /*!< [16]manual force NP clock gated */
    uint32_t rsv17           : 1;  /*!< [17]reserve */
    uint32_t forceBzGateClk  : 1;  /*!< [18]manual force BZ clock gated */
    uint32_t rsv19_31        : 13; /*!< [31:19]reserve */
} PDS_CTL2_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t rsv0             : 1;  /*!< [0]reserve */
    uint32_t forceMiscPwrOff  : 1;  /*!< [1]manual force MISC pwr_off */
    uint32_t rsv2_6           : 5;  /*!< [6:2]reserve */
    uint32_t forceMiscPdsRst  : 1;  /*!< [7]manual force MISC pds_rst */
    uint32_t rsv8_12          : 5;  /*!< [12:8]reserve */
    uint32_t forceMiscGateClk : 1;  /*!< [13]manual force MISC gate_clk */
    uint32_t rsv14_29         : 16; /*!< [29:14]reserve */
    uint32_t MiscIsoEn        : 1;  /*!< [30]make misc isolated at PDS Sleep state */
    uint32_t rsv31            : 1;  /*!< [31]reserve */
} PDS_CTL3_Type;

/**
 *  @brief PDS force configuration type definition
 */
typedef struct
{
    uint32_t rsv0        : 1; /*!< [0]reserve */
    uint32_t cpuRst      : 1; /*!< [1]NP reset at PDS Sleep state */
    uint32_t rsv2        : 1; /*!< [2]reserve */
    uint32_t cpuGateClk  : 1; /*!< [3]NP clock gated at PDS Sleep state */
    uint32_t rsv4_12     : 9; /*!< [12:4]reserve */
    uint32_t BzRst       : 1; /*!< [13]BZ reset at PDS Sleep state */
    uint32_t rsv14       : 1; /*!< [14]reserve */
    uint32_t BzGateClk   : 1; /*!< [15]BZ clock gated at PDS Sleep state */
    uint32_t rsv16_23    : 8; /*!< [23:16]reserve */
    uint32_t MiscPwrOff  : 1; /*!< [24]core_misc Power off at PDS Sleep state */
    uint32_t MiscRst     : 1; /*!< [25]core_misc reset at PDS Sleep state */
    uint32_t rsv26       : 1; /*!< [26]reserve */
    uint32_t MiscGateClk : 1; /*!< [27]core_misc clock gated at PDS Sleep state */
    uint32_t rsv28_31    : 4; /*!< [30:28]reserve */
} PDS_CTL4_Type;

/**
 *  @brief PDS default level configuration type definition
 */
typedef struct
{
    PDS_CTL_Type pdsCtl;   /*!< PDS_CTL configuration */
    PDS_CTL2_Type pdsCtl2; /*!< PDS_CTL2 configuration */
    PDS_CTL3_Type pdsCtl3; /*!< PDS_CTL3 configuration */
    PDS_CTL4_Type pdsCtl4; /*!< PDS_CTL4 configuration */
} PDS_DEFAULT_LV_CFG_Type;

/**
 *  @brief PDS interrupt type definition
 */
typedef enum {
    PDS_INT_WAKEUP = 0,  /*!< PDS wakeup interrupt(assert bit while wakeup, include PDS_Timer/...) */
    PDS_INT_RF_DONE = 2, /*!< PDS RF done interrupt */
    PDS_INT_MAX,         /*!< PDS int max number */
} PDS_INT_Type;

typedef enum {
    PDS_WAKEUP_SRC_HBN_IRQ_OUT = 0, /*!< wakeup trigger by hbn_irq_out[1:0] */
    PDS_WAKEUP_SRC_KYD_WAKEUP,      /*!< wakeup trigger by pds_kyd_wakeup */
    PDS_WAKEUP_SRC_GPIO_IRQ,        /*!< wakeup trigger by gpio_irq */
    PDS_WAKEUP_SRC_PDS_IO_INT,      /*!< wakeup trigger by irrx_int */
    PDS_WAKEUP_SRC_BLE_SLP_IRQ,     /*!< wakeup trigger by ble_slp_irq */
    PDS_WAKEUP_SRC_KYS_INT,         /*!< wakeup trigger by kys_int */
    PDS_WAKEUP_SRC_WDG_TIMEOUT,     /*!< wakeup trigger by pds_watchdog_timeout event */
} PDS_WAKEUP_SRC_Type;
/**
 *  @brief PDS GPIO type definition
 */
typedef enum {
    PDS_GPIO_PIN_0 = 0,
    PDS_GPIO_PIN_1,
    PDS_GPIO_PIN_2,
    PDS_GPIO_PIN_3,
    PDS_GPIO_PIN_7 = 7,
    PDS_GPIO_PIN_8,
    PDS_GPIO_PIN_14 = 14,
    PDS_GPIO_PIN_15,
    PDS_GPIO_PIN_16,
    PDS_GPIO_PIN_17,
    PDS_GPIO_PIN_18,
    PDS_GPIO_PIN_19,
    PDS_GPIO_PIN_20,
    PDS_GPIO_PIN_21,
    PDS_GPIO_PIN_22,
    PDS_GPIO_PIN_23,
    PDS_GPIO_PIN_24,
    PDS_GPIO_PIN_25,
    PDS_GPIO_PIN_26,
    PDS_GPIO_PIN_27,
    PDS_GPIO_PIN_28,
    PDS_GPIO_PIN_29,
    PDS_GPIO_PIN_MAX,
} PDS_GPIO_Type;

typedef enum {
    PDS_KEY_ROW_0 = 0,
    PDS_KEY_ROW_1,
    PDS_KEY_ROW_2,
    PDS_KEY_ROW_3,
    PDS_KEY_ROW_4,
    PDS_KEY_ROW_5,
    PDS_KEY_ROW_6,
    PDS_KEY_ROW_7,
} PDS_KEY_ROW_Type;

typedef enum {
    PDS_KEY_COL_0 = 0,
    PDS_KEY_COL_1,
    PDS_KEY_COL_2,
    PDS_KEY_COL_3,
    PDS_KEY_COL_4,
    PDS_KEY_COL_5,
    PDS_KEY_COL_6,
    PDS_KEY_COL_7,
} PDS_KEY_COL_Type;

/**
 * @brief PDS KYS COL and ROW GPIO Select
 */
typedef enum {
    PDS_KYS_GPIO_PIN_0 = 0,
    PDS_KYS_GPIO_PIN_1,
    PDS_KYS_GPIO_PIN_2,
    PDS_KYS_GPIO_PIN_3,
    PDS_KYS_GPIO_PIN_7 = 7,
    PDS_KYS_GPIO_PIN_8,
    PDS_KYS_GPIO_PIN_9,
    PDS_KYS_GPIO_PIN_10,
    PDS_KYS_GPIO_PIN_11,
    PDS_KYS_GPIO_PIN_12,
    PDS_KYS_GPIO_PIN_13,
    PDS_KYS_GPIO_PIN_14,
    PDS_KYS_GPIO_PIN_15,
    PDS_KYS_GPIO_PIN_16,
    PDS_KYS_GPIO_PIN_17,
    PDS_KYS_GPIO_PIN_18,
    PDS_KYS_GPIO_PIN_19,
    PDS_KYS_GPIO_PIN_20,
    PDS_KYS_GPIO_PIN_21,
    PDS_KYS_GPIO_PIN_22,
    PDS_KYS_GPIO_PIN_23,
    PDS_KYS_GPIO_PIN_24,
    PDS_KYS_GPIO_PIN_25,
    PDS_KYS_GPIO_PIN_26,
    PDS_KYS_GPIO_PIN_27,
    PDS_KYS_GPIO_PIN_28,
    PDS_KYS_GPIO_PIN_29,
    PDS_KYS_GPIO_PIN_30,
    PDS_KYS_GPIO_PIN_31,
    PDS_KYS_GPIO_PIN_MAX,
} PDS_KYS_GPIO_Type;

/**
 * @brief PDS KYD White Key Select
 */
typedef enum {
    PDS_KYD_NO_WHITE_KEY_MODE = 0,
    PDS_KYD_INVERT_DETECT_MODE,
    PDS_KYD_MASK_DETECT_MODE,
} PDS_KYD_WHITE_KEY_MODE_Type;

/**
 *  @brief PDS GPIO group set type
 */
typedef enum {
    PDS_GPIO_GROUP_SET_GPIO0_GPIO3,   /*!< gpio int set 0, gpio0 - gpio3   */
    PDS_GPIO_GROUP_SET_GPIO7,         /*!< gpio int set 1, gpio7           */
    PDS_GPIO_GROUP_SET_GPIO8,         /*!< gpio int set 2, gpio8           */
    PDS_GPIO_GROUP_SET_GPIO14_GPIO15, /*!< gpio int set 3, gpio14 - gpio15 */
    PDS_GPIO_GROUP_SET_GPIO16_GPIO19, /*!< gpio int set 4, gpio16 - gpio19 */
    PDS_GPIO_GROUP_SET_GPIO20_GPIO23, /*!< gpio int set 5, gpio20 - gpio23 */
    PDS_GPIO_GROUP_SET_GPIO24_GPIO27, /*!< gpio int set 6, gpio14 - gpio17 */
    PDS_GPIO_GROUP_SET_GPIO28_GPIO29, /*!< gpio int set 7, gpio28 - gpio29 */
} PDS_GPIO_GROUP_SET_Type;

/**
 *  @brief PDS reset event type definition
 */
typedef enum {
    PDS_RST_EVENT_BUS_RST,        /*!< hreset_n (Bus Reset) */
    PDS_RST_EVENT_HBN_PWR_ON_RST, /*!< pwr_rst_n (hbn power on reset) */
    PDS_RST_EVENT_PDS_RST,        /*!< pds_rst_n (pds reset) */
    PDS_RST_EVENT_MAX,            /*!< PDS rst event max number */
} PDS_RST_EVENT_Type;

/**
 *  @brief PDS RF status type definition
 */
typedef enum {
    PDS_RF_STS_OFF = 0,        /*!< 4'b0000 */
    PDS_RF_STS_PU_MBG = 1,     /*!< 4'b0001 */
    PDS_RF_STS_PU_LDO15RF = 3, /*!< 4'b0011 */
    PDS_RF_STS_PU_SFREG = 7,   /*!< 4'b0111 */
    PDS_RF_STS_BZ_EN_AON = 15, /*!< 4'b1111 */
} PDS_RF_STS_Type;

/**
 *  @brief PDS status type definition
 */
typedef enum {
    PDS_STS_IDLE = 0,            /*!< 4'b0000 */
    PDS_STS_ECG = 8,             /*!< 4'b1000 */
    PDS_STS_ERST = 12,           /*!< 4'b1100 */
    PDS_STS_EISO = 15,           /*!< 4'b1111 */
    PDS_STS_POFF = 7,            /*!< 4'b0111 */
    PDS_STS_PRE_BGON = 3,        /*!< 4'b0011 */
    PDS_STS_PRE_BGON1 = 1,       /*!< 4'b0001 */
    PDS_STS_BGON = 5,            /*!< 4'b0101 */
    PDS_STS_CLK_SW_32M = 4,      /*!< 4'b0100 */
    PDS_STS_PON_DCDC = 6,        /*!< 4'b0110 */
    PDS_STS_PON_LDO11_MISC = 14, /*!< 4'b1110 */
    PDS_STS_PON = 10,            /*!< 4'b1010 */
    PDS_STS_DISO = 2,            /*!< 4'b0010 */
    PDS_STS_DCG = 13,            /*!< 4'b1101 */
    PDS_STS_DRST = 11,           /*!< 4'b1011 */
    PDS_STS_WAIT_EFUSE = 9,      /*!< 4'b1001 */
} PDS_STS_Type;

/**
 *  @brief PDS auto power down configuration type definition
 */
typedef struct
{
    BL_Fun_Type mbgPower;      /*!< PDS auto [31] MBG power */
    BL_Fun_Type ldo18rfPower;  /*!< PDS auto [30] LDO18RF power */
    BL_Fun_Type sfregPower;    /*!< PDS auto [29] SF_REG power */
    BL_Fun_Type pllPower;      /*!< PDS auto [28] PLL power */
    BL_Fun_Type cpu0Power;     /*!< PDS auto [19] NP power */
    BL_Fun_Type rc32mPower;    /*!< PDS auto [17] RC32M power */
    BL_Fun_Type xtalPower;     /*!< PDS auto [14] XTAL power */
    BL_Fun_Type allPower;      /*!< PDS auto [13] all power */
    BL_Fun_Type isoPower;      /*!< PDS auto [11] ISO power */
    BL_Fun_Type bzPower;       /*!< PDS auto [10] BZ power */
    BL_Fun_Type sramDisStanby; /*!< PDS auto [9] SRAM memory stanby disable */
    BL_Fun_Type cgPower;       /*!< PDS auto [8] CG power */
    BL_Fun_Type cpu1Power;     /*!< PDS auto [7] AP power */
    BL_Fun_Type usbPower;      /*!< PDS auto [3] USB power */
} PDS_AUTO_POWER_DOWN_CFG_Type;

/**
 *  @brief PDS auto configuration type definition
 */
typedef struct
{
    uint32_t vddcoreVol;             /*!< PDS auto [27:24] VDDCORE voltage, reference 0x4001F80C[27:24], recommended 0xA */
    BL_Fun_Type vddcoreVolEn;        /*!< PDS auto [18] VDDCORE voltage enable bit */
    BL_Fun_Type cpu0NotNeedWFI;      /*!< PDS auto [21] NP not need WFI to get in PDS mode */
    BL_Fun_Type cpu1NotNeedWFI;      /*!< PDS auto [20] AP not need WFI to get in PDS mode */
    BL_Fun_Type busReset;            /*!< PDS auto [16] bus reset bit, reset after wake up from PDS mode */
    BL_Fun_Type disIrqWakeUp;        /*!< PDS auto [15] disable IRQ request to wake up from PDS mode, except PDS counter IRQ */
    BL_Fun_Type powerOffXtalForever; /*!< PDS auto [2] power off xtal after get in PDS mode, and never power on xtal after wake up */
    BL_Fun_Type sleepForever;        /*!< PDS auto [1] sleep forever after get in PDS mode, need reset system to restart */
} PDS_AUTO_NORMAL_CFG_Type;

/**
 *  @brief PDS force type definition
 */
typedef enum {
    PDS_FORCE_NP,  /*!< PDS force NP */
    PDS_FORCE_RSV, /*!< rsv */
    PDS_FORCE_BZ,  /*!< PDS force BZ */
} PDS_FORCE_Type;

/*@} end of group PDS_Public_Types */

/** @defgroup  PDS_Public_Constants
 *  @{
 */

/** @defgroup  PDS_INT_TYPE
 *  @{
 */
#define IS_PDS_INT_TYPE(type) (((type) == PDS_INT_WAKEUP) || \
                               ((type) == PDS_INT_RF_DONE))

/** @defgroup  PDS_WAKEUP_SRC_Type
 *  @{
 */
#define IS_PDS_WAKEUP_SRC_Type (((type) == PDS_WAKEUP_SRC_HBN_IRQ_OUT) || \
                                ((type) == PDS_WAKEUP_SRC_KYD_WAKEUP) ||  \
                                ((type) == PDS_WAKEUP_SRC_GPIO_IRQ) ||    \
                                ((type) == PDS_WAKEUP_SRC_PDS_IO_INT) ||  \
                                ((type) == PDS_WAKEUP_SRC_BLE_SLP_IRQ) || \
                                ((type) == PDS_WAKEUP_SRC_KYS_INT) ||     \
                                ((type) == PDS_WAKEUP_SRC_WDG_TIMEOUT))

/** @defgroup  PDS_GPIO_Type
 *  @{
 */
#define IS_PDS_GPIO_TYPE(type) (((type) == PDS_GPIO_PIN_0) ||  \
                                ((type) == PDS_GPIO_PIN_1) ||  \
                                ((type) == PDS_GPIO_PIN_2) ||  \
                                ((type) == PDS_GPIO_PIN_3) ||  \
                                ((type) == PDS_GPIO_PIN_7) ||  \
                                ((type) == PDS_GPIO_PIN_8) ||  \
                                ((type) == PDS_GPIO_PIN_14) || \
                                ((type) == PDS_GPIO_PIN_15) || \
                                ((type) == PDS_GPIO_PIN_16) || \
                                ((type) == PDS_GPIO_PIN_17) || \
                                ((type) == PDS_GPIO_PIN_18) || \
                                ((type) == PDS_GPIO_PIN_19) || \
                                ((type) == PDS_GPIO_PIN_20) || \
                                ((type) == PDS_GPIO_PIN_21) || \
                                ((type) == PDS_GPIO_PIN_22) || \
                                ((type) == PDS_GPIO_PIN_23) || \
                                ((type) == PDS_GPIO_PIN_24) || \
                                ((type) == PDS_GPIO_PIN_25) || \
                                ((type) == PDS_GPIO_PIN_26) || \
                                ((type) == PDS_GPIO_PIN_27) || \
                                ((type) == PDS_GPIO_PIN_28) || \
                                ((type) == PDS_GPIO_PIN_29))

/** @defgroup  PDS_KYS_GPIO_Type
 *  @{
 */
#define IS_PDS_KYS_GPIO_TYPE(type) (((type) == PDS_KYS_GPIO_PIN_0) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_1) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_2) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_3) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_7) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_8) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_9) ||  \
                                    ((type) == PDS_KYS_GPIO_PIN_10) || \
                                    ((type) == PDS_KYS_GPIO_PIN_11) || \
                                    ((type) == PDS_KYS_GPIO_PIN_12) || \
                                    ((type) == PDS_KYS_GPIO_PIN_13) || \
                                    ((type) == PDS_KYS_GPIO_PIN_14) || \
                                    ((type) == PDS_KYS_GPIO_PIN_15) || \
                                    ((type) == PDS_KYS_GPIO_PIN_16) || \
                                    ((type) == PDS_KYS_GPIO_PIN_17) || \
                                    ((type) == PDS_KYS_GPIO_PIN_18) || \
                                    ((type) == PDS_KYS_GPIO_PIN_19) || \
                                    ((type) == PDS_KYS_GPIO_PIN_20) || \
                                    ((type) == PDS_KYS_GPIO_PIN_21) || \
                                    ((type) == PDS_KYS_GPIO_PIN_22) || \
                                    ((type) == PDS_KYS_GPIO_PIN_23) || \
                                    ((type) == PDS_KYS_GPIO_PIN_24) || \
                                    ((type) == PDS_KYS_GPIO_PIN_25) || \
                                    ((type) == PDS_KYS_GPIO_PIN_26) || \
                                    ((type) == PDS_KYS_GPIO_PIN_27) || \
                                    ((type) == PDS_KYS_GPIO_PIN_28) || \
                                    ((type) == PDS_KYS_GPIO_PIN_29) || \
                                    ((type) == PDS_KYS_GPIO_PIN_30) || \
                                    ((type) == PDS_KYS_GPIO_PIN_31))

#define IS_PDS_KYS_ROW_TYPE(type) (((type) == PDS_KEY_ROW_0) || \
                                   ((type) == PDS_KEY_ROW_1) || \
                                   ((type) == PDS_KEY_ROW_2) || \
                                   ((type) == PDS_KEY_ROW_3) || \
                                   ((type) == PDS_KEY_ROW_4) || \
                                   ((type) == PDS_KEY_ROW_5) || \
                                   ((type) == PDS_KEY_ROW_6) || \
                                   ((type) == PDS_KEY_ROW_7))

#define IS_PDS_KYS_COL_TYPE(type) (((type) == PDS_KEY_COL_0) || \
                                   ((type) == PDS_KEY_COL_1) || \
                                   ((type) == PDS_KEY_COL_2) || \
                                   ((type) == PDS_KEY_COL_3) || \
                                   ((type) == PDS_KEY_COL_4) || \
                                   ((type) == PDS_KEY_COL_5) || \
                                   ((type) == PDS_KEY_COL_6) || \
                                   ((type) == PDS_KEY_COL_7))

/** @defgroup  PDS_GPIO_GROUP_SET_Type
 *  @{
 */
#define IS_PDS_GPIO_GROUP_SET_Type(type) (((type) == PDS_GPIO_GROUP_SET_GPIO0_GPIO3) ||   \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO7) ||         \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO8) ||         \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO14_GPIO15) || \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO16_GPIO19) || \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO20_GPIO23) || \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO24_GPIO27) || \
                                          ((type) == PDS_GPIO_GROUP_SET_GPIO28_GPIO29))

/**
 *  @brief PDS GPIO interrupt control mode type definition
 */
typedef enum {
    PDS_GPIO_INT_SYNC_FALLING_EDGE = 0,        /*!< GPIO interrupt sync mode, GPIO negedge pulse trigger interrupt */
    PDS_GPIO_INT_SYNC_RISING_EDGE = 1,         /*!< GPIO interrupt sync mode, GPIO posedge pulse trigger interrupt */
    PDS_GPIO_INT_SYNC_LOW_LEVEL = 2,           /*!< GPIO interrupt sync mode, GPIO negedge level trigger interrupt */
    PDS_GPIO_INT_SYNC_HIGH_LEVEL = 3,          /*!< GPIO interrupt sync mode, GPIO posedge level trigger interrupt */
    PDS_GPIO_INT_SYNC_RISING_FALLING_EDGE = 4, /*!< GPIO interrupt sync mode, GPIO posedge negedge pulse trigger interrupt */
    PDS_GPIO_INT_ASYNC_FALLING_EDGE = 8,       /*!< GPIO interrupt async mode, GPIO negedge pulse trigger interrupt */
    PDS_GPIO_INT_ASYNC_RISING_EDGE = 9,        /*!< GPIO interrupt async mode, GPIO posedge pulse trigger interrupt */
    PDS_GPIO_INT_ASYNC_LOW_LEVEL = 10,         /*!< GPIO interrupt async mode, GPIO negedge level trigger interrupt */
    PDS_GPIO_INT_ASYNC_HIGH_LEVEL = 11,        /*!< GPIO interrupt async mode, GPIO posedge level trigger interrupt */
} PDS_GPIO_INT_TRIG_Type;

/** @defgroup  PDS_RST_EVENT_TYPE
 *  @{
 */
#define IS_PDS_RST_EVENT_TYPE(type) (((type) == PDS_RST_EVENT_BUS_RST) ||        \
                                     ((type) == PDS_RST_EVENT_HBN_PWR_ON_RST) || \
                                     ((type) == PDS_RST_EVENT_PDS_RST) ||        \
                                     ((type) == PDS_RST_EVENT_MAX))

/** @defgroup  PDS_PLL_STS_TYPE
 *  @{
 */
#define IS_PDS_PLL_STS_TYPE(type) (((type) == PDS_PLL_STS_OFF) ||   \
                                   ((type) == PDS_PLL_STS_SFREG) || \
                                   ((type) == PDS_PLL_STS_PU) ||    \
                                   ((type) == PDS_PLL_STS_RDY))

/** @defgroup  PDS_RF_STS_TYPE
 *  @{
 */
#define IS_PDS_RF_STS_TYPE(type) (((type) == PDS_RF_STS_OFF) ||        \
                                  ((type) == PDS_RF_STS_PU_MBG) ||     \
                                  ((type) == PDS_RF_STS_PU_LDO15RF) || \
                                  ((type) == PDS_RF_STS_PU_SFREG) ||   \
                                  ((type) == PDS_RF_STS_BZ_EN_AON))

/** @defgroup  PDS_STS_TYPE
 *  @{
 */
#define IS_PDS_STS_TYPE(type) (((type) == PDS_STS_IDLE) ||           \
                               ((type) == PDS_STS_ECG) ||            \
                               ((type) == PDS_STS_ERST) ||           \
                               ((type) == PDS_STS_EISO) ||           \
                               ((type) == PDS_STS_POFF) ||           \
                               ((type) == PDS_STS_PRE_BGON) ||       \
                               ((type) == PDS_STS_PRE_BGON1) ||      \
                               ((type) == PDS_STS_BGON) ||           \
                               ((type) == PDS_STS_CLK_SW_32M) ||     \
                               ((type) == PDS_STS_PON_DCDC) ||       \
                               ((type) == PDS_STS_PON_LDO11_MISC) || \
                               ((type) == PDS_STS_PON) ||            \
                               ((type) == PDS_STS_DISO) ||           \
                               ((type) == PDS_STS_DCG) ||            \
                               ((type) == PDS_STS_DRST) ||           \
                               ((type) == PDS_STS_WAIT_EFUSE))

/** @defgroup  PDS_PLL_XTAL_TYPE
 *  @{
 */
#define IS_PDS_PLL_XTAL_TYPE(type) (((type) == PDS_PLL_XTAL_NONE) || \
                                    ((type) == PDS_PLL_XTAL_32M) ||  \
                                    ((type) == PDS_PLL_XTAL_RC32M))

/** @defgroup  PDS_PLL_CLK_TYPE
 *  @{
 */
#define IS_PDS_PLL_CLK_TYPE(type) (((type) == PDS_PLL_CLK_480M) || \
                                   ((type) == PDS_PLL_CLK_240M) || \
                                   ((type) == PDS_PLL_CLK_192M) || \
                                   ((type) == PDS_PLL_CLK_160M) || \
                                   ((type) == PDS_PLL_CLK_120M) || \
                                   ((type) == PDS_PLL_CLK_96M) ||  \
                                   ((type) == PDS_PLL_CLK_80M) ||  \
                                   ((type) == PDS_PLL_CLK_48M) ||  \
                                   ((type) == PDS_PLL_CLK_32M))

/** @defgroup  PDS_FORCE_TYPE
 *  @{
 */
#define IS_PDS_FORCE_TYPE(type) (((type) == PDS_FORCE_NP) ||  \
                                 ((type) == PDS_FORCE_RSV) || \
                                 ((type) == PDS_FORCE_BZ) ||  \
                                 ((type) == PDS_FORCE_USB))

/** @defgroup  PDS_AUDIO_PLL_TYPE
 *  @{
 */
#define IS_PDS_AUDIO_PLL_TYPE(type) (((type) == AUDIO_PLL_12288000_HZ) || \
                                     ((type) == AUDIO_PLL_11289600_HZ) || \
                                     ((type) == AUDIO_PLL_5644800_HZ) ||  \
                                     ((type) == AUDIO_PLL_24576000_HZ) || \
                                     ((type) == AUDIO_PLL_24000000_HZ) || \
                                     ((type) == AUDIO_PLL_50000000_HZ))

/*@} end of group PDS_Public_Constants */

/** @defgroup  PDS_Public_Macros
 *  @{
 */
#define PDS_LDO_MIN_PU_CNT        (25) /* LDO need 25 cycles to power up */
#define PDS_WARMUP_CNT            (38) /* LDO hw warmup compensation latency cycles */
#define PDS_WARMUP_LATENCY_CNT    (38) /* LDO hw warmup compensation latency cycles */
#define PDS_FORCE_PWR_OFF_OFFSET  (0)
#define PDS_FORCE_ISO_EN_OFFSET   (4)
#define PDS_FORCE_PDS_RST_OFFSET  (8)
#define PDS_FORCE_MEM_STBY_OFFSET (12)
#define PDS_FORCE_GATE_CLK_OFFSET (16)

#define PDS_GPIO_IS_GROUP_ID(pin) (pin >> 2)
/*@} end of group PDS_Public_Macros */

/** @defgroup  PDS_Public_Functions
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type PDS_Int_Callback_Install(PDS_INT_Type intType, intCallback_Type *cbFun);
void PDS_WAKEUP_IRQHandler(void);
#endif
/*----------*/
BL_Err_Type PDS_Set_GPIO_Pad_Cfg(PDS_GPIO_Type pin, PDS_GPIO_Cfg_Type *cfg);
BL_Err_Type PDS_GPIO_Write(PDS_GPIO_GROUP_SET_Type grp, uint32_t val);
BL_Err_Type PDS_Set_GPIO_Pad_IntMask(PDS_GPIO_GROUP_SET_Type grp, BL_Mask_Type intMask);
BL_Err_Type PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_GROUP_SET_Type grp, PDS_GPIO_INT_TRIG_Type trig);
BL_Err_Type PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_GROUP_SET_Type grp);
BL_Err_Type PDS_Set_All_GPIO_IntClear(void);
BL_Sts_Type PDS_Get_GPIO_Pad_IntStatus(PDS_GPIO_Type pin);
/*----------*/
BL_Err_Type PDS_Set_Flash_Pad_Pull_None(uint8_t pinCfg);
BL_Err_Type PDS_Set_Flash_Pad_Pull_None_Fast(uint8_t pinCfg);
/*----------*/
BL_Err_Type PDS_Disable_GPIO_Keep(void);
BL_Err_Type PDS_Enable_PDS_Pad_Keep(uint32_t keepSel);
/*----------*/
BL_Err_Type PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt);
BL_Err_Type PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3);
BL_Err_Type PDS_RAM_Config(PDS_RAM_CFG_Type *ramCfg);
BL_Err_Type PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt);
/*----------*/
BL_Err_Type PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_Type intType, BL_Fun_Type enable);
BL_Sts_Type PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_Type intType);
BL_Err_Type PDS_IntMask(PDS_INT_Type intType, BL_Mask_Type intMask);
BL_Sts_Type PDS_Get_IntStatus(PDS_INT_Type intType);
BL_Err_Type PDS_IntClear(void);
/*----------*/
PDS_RF_STS_Type PDS_Get_PdsRfStstus(void);
PDS_STS_Type PDS_Get_PdsStstus(void);
/*----------*/
BL_Err_Type PDS_Clear_Reset_Event(void);
BL_Sts_Type PDS_Get_Reset_Event(PDS_RST_EVENT_Type event);
/*----------*/
BL_Err_Type PDS_WAKEUP_IRQHandler_Install(void);
BL_Err_Type PDS_Int_Callback_Install(PDS_INT_Type intType, intCallback_Type *cbFun);
/*----------*/
// void PDS_Auto_Time_Config(uint32_t sleepDuration);
void PDS_Auto_Enable(PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, BL_Fun_Type enable);
void PDS_Manual_Force_Turn_Off(PDS_FORCE_Type domain);
void PDS_Manual_Force_Turn_On(PDS_FORCE_Type domain);
/*----------*/
BL_Err_Type PDS_Set_KYD_Matrix_Size(uint8_t col_size, uint8_t row_size);
BL_Err_Type PDS_Set_KYD_Col_Value(uint8_t val);
BL_Err_Type PDS_Set_KYD_Row_Pull(uint8_t en);
BL_Err_Type PDS_Set_KYD_Wakeup_En(uint8_t en);
BL_Err_Type PDS_Clear_KYD_Wakeup(void);
BL_Err_Type PDS_Set_KYS_ROW_IN_GPIO(PDS_KEY_ROW_Type row_x, PDS_KYS_GPIO_Type sel_io);
BL_Err_Type PDS_Set_KYS_COL_OUT_GPIO(PDS_KEY_COL_Type col_x, PDS_KYS_GPIO_Type sel_io);
BL_Err_Type PDS_Get_KYS_Wakeup_ROW_INDEX_GPIO(PDS_KEY_ROW_Type* row_index,PDS_KYS_GPIO_Type* row_gpio);
BL_Err_Type PDS_Get_KYS_Wakeup_COL_INDEX_GPIO(PDS_KEY_COL_Type* col_index,PDS_KYS_GPIO_Type* col_gpio);
BL_Err_Type PDS_Set_KYS_White_Key(uint8_t white_key_i ,PDS_KEY_ROW_Type row_index,PDS_KEY_COL_Type col_index,PDS_KYD_WHITE_KEY_MODE_Type type);
/*----------*/

/*@} end of group PDS_Public_Functions */

/*@} end of group PDS */

/*@} end of group BL702L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_PDS_H__ */
