/**
  ******************************************************************************
  * @file    bl616_hbn.h
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
#ifndef __BL616_HBN_H__
#define __BL616_HBN_H__

#include "hbn_reg.h"
#include "bl616_aon.h"
#include "bl616_l1c.h"
#include "bl616_common.h"
#include "bflb_sflash.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  HBN
 *  @{
 */

/** @defgroup  HBN_Public_Types
 *  @{
 */

/**
 *  @brief HBN PIR interrupt configuration type definition
 */
typedef struct
{
    uint8_t lowIntEn;  /*!< Low will trigger interrupt */
    uint8_t highIntEn; /*!< High will trigger interrupt */
} HBN_PIR_INT_CFG_Type;

/** @defgroup HBN PIR low pass filter type definition
  * @{
  */
#define HBN_PIR_LPF_DIV1             (0) /*!< HBN PIR lpf div 1 */
#define HBN_PIR_LPF_DIV2             (1) /*!< HBN PIR lpf div 2 */
/**
  * @}
  */

/** @defgroup HBN PIR high pass filter type definition
  * @{
  */
#define HBN_PIR_HPF_METHOD0          (0) /*!< HBN PIR hpf calc method 0, 1-z^-1 */
#define HBN_PIR_HPF_METHOD1          (1) /*!< HBN PIR hpf calc method 1, 1-z^-2 */
#define HBN_PIR_HPF_METHOD2          (2) /*!< HBN PIR hpf calc method 2, 1-z^-3 */
/**
  * @}
  */

/** @defgroup HBN flash pad pu pd type
  * @{
  */
#define HBN_FLASH_PAD_PULL_NONE      (0) /*!< flash pad pull none when Flash at Deep Power Down Mode */
#define HBN_FLASH_PAD_PULL_UP        (1) /*!< flash pad pull up when Flash at Deep Power Down Mode */
#define HBN_FLASH_PAD_PULL_DOWN      (2) /*!< flash pad pull down when Flash at Deep Power Down Mode */
/**
  * @}
  */

/** @defgroup HBN BOD threshold type definition
  * @{
  */
#define HBN_BOD_THRES_2P05V          (0) /*!< BOD threshold 2.05V */
#define HBN_BOD_THRES_2P10V          (1) /*!< BOD threshold 2.10V */
#define HBN_BOD_THRES_2P15V          (2) /*!< BOD threshold 2.15V */
#define HBN_BOD_THRES_2P20V          (3) /*!< BOD threshold 2.20V */
#define HBN_BOD_THRES_2P25V          (4) /*!< BOD threshold 2.25V */
#define HBN_BOD_THRES_2P30V          (5) /*!< BOD threshold 2.30V */
#define HBN_BOD_THRES_2P35V          (6) /*!< BOD threshold 2.35V */
#define HBN_BOD_THRES_2P40V          (7) /*!< BOD threshold 2.40V */
/**
  * @}
  */

/** @defgroup HBN BOD mode type definition
  * @{
  */
#define HBN_BOD_MODE_POR_INDEPENDENT (0) /*!< POR is independent of BOD */
#define HBN_BOD_MODE_POR_RELEVANT    (1) /*!< POR is relevant to BOD */
/**
  * @}
  */

/** @defgroup HBN 32K clock type definition
  * @{
  */
#define HBN_32K_RC                   (0) /*!< HBN use rc 32k */
#define HBN_32K_XTAL                 (1) /*!< HBN use xtal 32k */
#define HBN_32K_DIG                  (3) /*!< HBN use dig 32k */
/**
  * @}
  */

/** @defgroup HBN xclk clock type definition
  * @{
  */
#define HBN_MCU_XCLK_RC32M           (0) /*!< use RC32M as xclk clock */
#define HBN_MCU_XCLK_XTAL            (1) /*!< use XTAL as xclk clock */
/**
  * @}
  */

/** @defgroup HBN GPADC clock type definition
  * @{
  */
#define HBN_GPADC_CLK_32M            (0) /*!< use gpadc_32m_clk as clock */
#define HBN_GPADC_CLK_F32K           (1) /*!< use f32k_clk as clock */
/**
  * @}
  */

/** @defgroup HBN root clock type definition
  * @{
  */
#define HBN_MCU_ROOT_CLK_XCLK        (0) /*!< use XCLK as root clock */
#define HBN_MCU_ROOT_CLK_PLL         (1) /*!< use PLL as root clock */
/**
  * @}
  */

/** @defgroup HBN UART clock type definition
  * @{
  */
#define HBN_UART_CLK_MCU_BCLK        (0) /*!< Select mcu_pbclk as UART clock */
#define HBN_UART_CLK_MUXPLL_160M     (1) /*!< Select MUXPLL 160M as UART clock */
#define HBN_UART_CLK_XCLK            (2) /*!< Select XCLK as UART clock */
/**
  * @}
  */

/** @defgroup HBN RTC interrupt delay type definition
  * @{
  */
#define HBN_RTC_INT_DELAY_32T        (0) /*!< HBN RTC interrupt delay 32T */
#define HBN_RTC_INT_DELAY_0T         (1) /*!< HBN RTC interrupt delay 0T */
/**
  * @}
  */

/** @defgroup HBN interrupt type definition
  * @{
  */
#define HBN_INT_GPIO16               (0)  /*!< HBN interrupt type: GPIO16 */
#define HBN_INT_GPIO17               (1)  /*!< HBN interrupt type: GPIO17 */
#define HBN_INT_GPIO18               (2)  /*!< HBN interrupt type: GPIO18 */
#define HBN_INT_GPIO19               (3)  /*!< HBN interrupt type: GPIO19 */
#define HBN_INT_RTC                  (16) /*!< HBN interrupt type: RTC */
#define HBN_INT_PIR                  (17) /*!< HBN interrupt type: PIR */
#define HBN_INT_BOD                  (18) /*!< HBN interrupt type: BOD */
#define HBN_INT_ACOMP0               (20) /*!< HBN interrupt type: ACOMP0 */
#define HBN_INT_ACOMP1               (22) /*!< HBN interrupt type: ACOMP1 */
/**
  * @}
  */

/** @defgroup HBN aon pad type definition
  * @{
  */
#define HBN_AON_PAD_GPIO16           (0) /*!< HBN aon pad type: GPIO16 */
#define HBN_AON_PAD_GPIO17           (1) /*!< HBN aon pad type: GPIO17 */
#define HBN_AON_PAD_GPIO18           (2) /*!< HBN aon pad type: GPIO18 */
#define HBN_AON_PAD_GPIO19           (3) /*!< HBN aon pad type: GPIO19 */
/**
  * @}
  */

/**
 *  @brief HBN AON PAD configuration type definition
 */
typedef struct
{
    uint8_t ctrlEn;   /*!< AON PAD Control by AON HW */
    uint8_t ie;       /*!< Always on PAD IE/SMT (if corresponding  AON GPIO controlled by AON HW) */
    uint8_t oe;       /*!< Always on PAD OE (if corresponding  AON GPIO controlled by AON HW)     */
    uint8_t pullUp;   /*!< Always on PAD PU (if corresponding  AON GPIO controlled by AON HW)     */
    uint8_t pullDown; /*!< Always on PAD PD (if corresponding  AON GPIO controlled by AON HW)     */
} HBN_AON_PAD_CFG_Type;

/** @defgroup HBN GPIO interrupt trigger type definition
  * @{
  */
#define HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE        (0x0) /*!< HBN GPIO INT trigger type: sync falling edge trigger */
#define HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE         (0x1) /*!< HBN GPIO INT trigger type: sync rising edge trigger */
#define HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL           (0x2) /*!< HBN GPIO INT trigger type: sync low level trigger */
#define HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL          (0x3) /*!< HBN GPIO INT trigger type: sync high level trigger */
#define HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE (0x4) /*!< HBN GPIO INT trigger type: sync rising falling edge trigger */
#define HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE       (0x8) /*!< HBN GPIO INT trigger type: async falling edge trigger */
#define HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE        (0x9) /*!< HBN GPIO INT trigger type: async rising edge trigger */
#define HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL          (0xA) /*!< HBN GPIO INT trigger type: async low level trigger */
#define HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL         (0xB) /*!< HBN GPIO INT trigger type: async high level trigger */
/**
  * @}
  */

/** @defgroup HBN OUT0 interrupt type definition
  * @{
  */
#define HBN_OUT0_INT_GPIO16                           (0) /*!< HBN out 0 interrupt type: GPIO16 */
#define HBN_OUT0_INT_GPIO17                           (1) /*!< HBN out 0 interrupt type: GPIO17 */
#define HBN_OUT0_INT_GPIO18                           (2) /*!< HBN out 0 interrupt type: GPIO18 */
#define HBN_OUT0_INT_GPIO19                           (3) /*!< HBN out 0 interrupt type: GPIO19 */
#define HBN_OUT0_INT_RTC                              (4) /*!< HBN out 0 interrupt type: RTC */
#define HBN_OUT0_INT_MAX                              (5) /*!< MAX */
/**
  * @}
  */

/** @defgroup HBN OUT1 interrupt type definition
  * @{
  */
#define HBN_OUT1_INT_PIR                              (0) /*!< HBN out 1 interrupt type: PIR */
#define HBN_OUT1_INT_BOD                              (1) /*!< HBN out 1 interrupt type: BOD */
#define HBN_OUT1_INT_ACOMP0                           (2) /*!< HBN out 1 interrupt type: ACOMP0 */
#define HBN_OUT1_INT_ACOMP1                           (3) /*!< HBN out 1 interrupt type: ACOMP1 */
#define HBN_OUT1_INT_MAX                              (4) /*!< MAX */
/**
  * @}
  */

/** @defgroup HBN acomp interrupt type definition
  * @{
  */
#define HBN_ACOMP_INT_EDGE_POSEDGE                    (1) /*!< HBN acomp interrupt edge posedge */
#define HBN_ACOMP_INT_EDGE_NEGEDGE                    (2) /*!< HBN acomp interrupt edge negedge */
#define HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE            (3) /*!< HBN acomp interrupt edge posedge and negedge */
/**
  * @}
  */

/** @defgroup HBN LDO level type definition
  * @{
  */
#define HBN_LDO_LEVEL_0P70V                           (2)  /*!< HBN LDO voltage 0.70V */
#define HBN_LDO_LEVEL_0P75V                           (3)  /*!< HBN LDO voltage 0.75V */
#define HBN_LDO_LEVEL_0P80V                           (4)  /*!< HBN LDO voltage 0.80V */
#define HBN_LDO_LEVEL_0P85V                           (5)  /*!< HBN LDO voltage 0.85V */
#define HBN_LDO_LEVEL_0P90V                           (6)  /*!< HBN LDO voltage 0.90V */
#define HBN_LDO_LEVEL_0P95V                           (7)  /*!< HBN LDO voltage 0.95V */
#define HBN_LDO_LEVEL_1P00V                           (8)  /*!< HBN LDO voltage 1.00V */
#define HBN_LDO_LEVEL_1P05V                           (9)  /*!< HBN LDO voltage 1.05V */
#define HBN_LDO_LEVEL_1P10V                           (10) /*!< HBN LDO voltage 1.10V */
#define HBN_LDO_LEVEL_1P15V                           (11) /*!< HBN LDO voltage 1.15V */
#define HBN_LDO_LEVEL_1P20V                           (12) /*!< HBN LDO voltage 1.20V */
#define HBN_LDO_LEVEL_1P25V                           (13) /*!< HBN LDO voltage 1.25V */
#define HBN_LDO_LEVEL_1P30V                           (14) /*!< HBN LDO voltage 1.30V */
#define HBN_LDO_LEVEL_1P35V                           (15) /*!< HBN LDO voltage 1.35V */
/**
  * @}
  */

/** @defgroup HBN level type definition
  * @{
  */
#define HBN_LEVEL_0                                   (0) /*!< HBN pd_core */
#define HBN_LEVEL_1                                   (1) /*!< HBN pd_aon_hbncore + pd_core */
/**
  * @}
  */

/**
 *  @brief HBN BOD configuration type definition
 */
typedef struct
{
    uint8_t enableBod;      /*!< Enable BOD or not */
    uint8_t enableBodInt;   /*!< Enable BOD interrupt or not */
    uint8_t bodThreshold;   /*!< BOD threshold */
    uint8_t enablePorInBod; /*!< Enable POR when BOD occure or not */
} HBN_BOD_CFG_Type;

/**
 *  @brief HBN APP configuration type definition
 */
typedef struct
{
    uint8_t useXtal32k;           /*!< Whether use xtal 32K as 32K clock source,otherwise use rc32k */
    uint32_t sleepTime;           /*!< HBN sleep time */
    uint8_t gpioWakeupSrc;        /*!< GPIO Wakeup source */
    uint8_t gpioTrigType;         /*!< GPIO Triger type */
    spi_flash_cfg_type *flashCfg; /*!< Flash config pointer, used when power down flash */
    uint8_t hbnLevel;             /*!< HBN level */
    uint8_t ldoLevel;             /*!< LDO level */
    uint8_t dcdcPuSeq;            /*!< power on dcdc sequence */
} HBN_APP_CFG_Type;

/*@} end of group HBN_Public_Types */

/** @defgroup  HBN_Public_Constants
 *  @{
 */

/** @defgroup  HBN_PIR_LPF_TYPE
 *  @{
 */
#define IS_HBN_PIR_LPF_TYPE(type) (((type) == HBN_PIR_LPF_DIV1) || \
                                   ((type) == HBN_PIR_LPF_DIV2))

/** @defgroup  HBN_PIR_HPF_TYPE
 *  @{
 */
#define IS_HBN_PIR_HPF_TYPE(type) (((type) == HBN_PIR_HPF_METHOD0) || \
                                   ((type) == HBN_PIR_HPF_METHOD1) || \
                                   ((type) == HBN_PIR_HPF_METHOD2))

/** @defgroup  HBN_FLASH_PAD_PULL_TYPE
 *  @{
 */
#define IS_HBN_FLASH_PAD_PULL_TYPE(type) (((type) == HBN_FLASH_PAD_PULL_NONE) || \
                                          ((type) == HBN_FLASH_PAD_PULL_UP) ||   \
                                          ((type) == HBN_FLASH_PAD_PULL_DOWN))

/** @defgroup  HBN_BOD_THRES_TYPE
 *  @{
 */
#define IS_HBN_BOD_THRES_TYPE(type) (((type) == HBN_BOD_THRES_2P05V) || \
                                     ((type) == HBN_BOD_THRES_2P10V) || \
                                     ((type) == HBN_BOD_THRES_2P15V) || \
                                     ((type) == HBN_BOD_THRES_2P20V) || \
                                     ((type) == HBN_BOD_THRES_2P25V) || \
                                     ((type) == HBN_BOD_THRES_2P30V) || \
                                     ((type) == HBN_BOD_THRES_2P35V) || \
                                     ((type) == HBN_BOD_THRES_2P40V))

/** @defgroup  HBN_BOD_MODE_TYPE
 *  @{
 */
#define IS_HBN_BOD_MODE_TYPE(type) (((type) == HBN_BOD_MODE_POR_INDEPENDENT) || \
                                    ((type) == HBN_BOD_MODE_POR_RELEVANT))

/** @defgroup  HBN_32K_CLK_TYPE
 *  @{
 */
#define IS_HBN_32K_CLK_TYPE(type) (((type) == HBN_32K_RC) ||   \
                                   ((type) == HBN_32K_XTAL) || \
                                   ((type) == HBN_32K_DIG))

/** @defgroup  HBN_MCU_XCLK_TYPE
 *  @{
 */
#define IS_HBN_MCU_XCLK_TYPE(type) (((type) == HBN_MCU_XCLK_RC32M) || \
                                    ((type) == HBN_MCU_XCLK_XTAL))

/** @defgroup  HBN_GPADC_CLK_TYPE
 *  @{
 */
#define IS_HBN_GPADC_CLK_TYPE(type) (((type) == HBN_GPADC_CLK_32M) || \
                                     ((type) == HBN_GPADC_CLK_F32K))

/** @defgroup  HBN_MCU_ROOT_CLK_TYPE
 *  @{
 */
#define IS_HBN_MCU_ROOT_CLK_TYPE(type) (((type) == HBN_MCU_ROOT_CLK_XCLK) || \
                                        ((type) == HBN_MCU_ROOT_CLK_PLL))

/** @defgroup  HBN_UART_CLK_TYPE
 *  @{
 */
#define IS_HBN_UART_CLK_TYPE(type) (((type) == HBN_UART_CLK_MCU_BCLK) ||    \
                                    ((type) == HBN_UART_CLK_MUXPLL_160M) || \
                                    ((type) == HBN_UART_CLK_XCLK))

/** @defgroup  HBN_RTC_INT_DELAY_TYPE
 *  @{
 */
#define IS_HBN_RTC_INT_DELAY_TYPE(type) (((type) == HBN_RTC_INT_DELAY_32T) || \
                                         ((type) == HBN_RTC_INT_DELAY_0T))

/** @defgroup  HBN_INT_TYPE
 *  @{
 */
#define IS_HBN_INT_TYPE(type) (((type) == HBN_INT_GPIO16) || \
                               ((type) == HBN_INT_GPIO17) || \
                               ((type) == HBN_INT_GPIO18) || \
                               ((type) == HBN_INT_GPIO19) || \
                               ((type) == HBN_INT_RTC) ||    \
                               ((type) == HBN_INT_PIR) ||    \
                               ((type) == HBN_INT_BOD) ||    \
                               ((type) == HBN_INT_ACOMP0) || \
                               ((type) == HBN_INT_ACOMP1))

/** @defgroup  HBN_AON_PAD_TYPE
 *  @{
 */
#define IS_HBN_AON_PAD_TYPE(type) (((type) == HBN_AON_PAD_GPIO16) || \
                                   ((type) == HBN_AON_PAD_GPIO17) || \
                                   ((type) == HBN_AON_PAD_GPIO18) || \
                                   ((type) == HBN_AON_PAD_GPIO19))

/** @defgroup  HBN_ACOMP_INT_EDGE_TYPE
 *  @{
 */
#define IS_HBN_ACOMP_INT_EDGE_TYPE(type) (((type) == HBN_ACOMP_INT_EDGE_POSEDGE) || \
                                          ((type) == HBN_ACOMP_INT_EDGE_NEGEDGE) || \
                                          ((type) == HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE))

/** @defgroup  HBN_GPIO_INT_TRIGGER_TYPE
 *  @{
 */
#define IS_HBN_GPIO_INT_TRIGGER_TYPE(type) (((type) == HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE) ||        \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE) ||         \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL) ||           \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL) ||          \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE) || \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE) ||       \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE) ||        \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL) ||          \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL))

/** @defgroup  HBN_OUT0_INT_TYPE
 *  @{
 */
#define IS_HBN_OUT0_INT_TYPE(type) (((type) == HBN_OUT0_INT_GPIO9) ||  \
                                    ((type) == HBN_OUT0_INT_GPIO10) || \
                                    ((type) == HBN_OUT0_INT_GPIO11) || \
                                    ((type) == HBN_OUT0_INT_GPIO12) || \
                                    ((type) == HBN_OUT0_INT_GPIO13) || \
                                    ((type) == HBN_OUT0_INT_GPIO14) || \
                                    ((type) == HBN_OUT0_INT_GPIO15) || \
                                    ((type) == HBN_OUT0_INT_GPIO40) || \
                                    ((type) == HBN_OUT0_INT_GPIO41) || \
                                    ((type) == HBN_OUT0_INT_RTC) ||    \
                                    ((type) == HBN_OUT0_INT_MAX))

/** @defgroup  HBN_OUT1_INT_TYPE
 *  @{
 */
#define IS_HBN_OUT1_INT_TYPE(type) (((type) == HBN_OUT1_INT_PIR) ||    \
                                    ((type) == HBN_OUT1_INT_BOD) ||    \
                                    ((type) == HBN_OUT1_INT_ACOMP0) || \
                                    ((type) == HBN_OUT1_INT_ACOMP1) || \
                                    ((type) == HBN_OUT1_INT_MAX))

/** @defgroup  HBN_LDO_LEVEL_TYPE
 *  @{
 */
#define IS_HBN_LDO_LEVEL_TYPE(type) (((type) == HBN_LDO_LEVEL_0P70V) || \
                                     ((type) == HBN_LDO_LEVEL_0P75V) || \
                                     ((type) == HBN_LDO_LEVEL_0P80V) || \
                                     ((type) == HBN_LDO_LEVEL_0P85V) || \
                                     ((type) == HBN_LDO_LEVEL_0P90V) || \
                                     ((type) == HBN_LDO_LEVEL_0P95V) || \
                                     ((type) == HBN_LDO_LEVEL_1P00V) || \
                                     ((type) == HBN_LDO_LEVEL_1P05V) || \
                                     ((type) == HBN_LDO_LEVEL_1P10V) || \
                                     ((type) == HBN_LDO_LEVEL_1P15V) || \
                                     ((type) == HBN_LDO_LEVEL_1P20V) || \
                                     ((type) == HBN_LDO_LEVEL_1P25V) || \
                                     ((type) == HBN_LDO_LEVEL_1P30V) || \
                                     ((type) == HBN_LDO_LEVEL_1P35V))

/** @defgroup  HBN_LEVEL_TYPE
 *  @{
 */
#define IS_HBN_LEVEL_TYPE(type) (((type) == HBN_LEVEL_0) || \
                                 ((type) == HBN_LEVEL_1))

/*@} end of group HBN_Public_Constants */

/** @defgroup  HBN_Public_Macros
 *  @{
 */
#define HBN_RAM_SIZE                   (4 * 1024)
#define HBN_RTC_COMP_BIT0_39           0x01
#define HBN_RTC_COMP_BIT0_23           0x02
#define HBN_RTC_COMP_BIT13_39          0x04
#define HBN_STATUS_ENTER_FLAG          0x4e424845
#define HBN_STATUS_WAKEUP_FLAG         0x4e424857
#define APP_JUMP_ENTER_FLAG            0x50504145
#define APP_JUMP_DONE_FLAG             0x50504157
#define HBN_RELEASE_CORE_FLAG          (0x4)
#define HBN_LDO18IO_POWER_DLY_FLAG     (0x52)
#define HBN_XTAL_FLAG_VALUE            (0x8)
#define HBN_FLASH_POWER_DLY_FLAG       (0x6)
#define HBN_GPIO_KEEP_FLAG             (0x2)
#define PDS_GPIO_KEEP_FLAG             (0x2)

/* 0x108 : HBN_RSV2 */
#define HBN_LDO18IO_POWER_ON_DLY       HBN_LDO18IO_POWER_ON_DLY
#define HBN_LDO18IO_POWER_ON_DLY_POS   (0U)
#define HBN_LDO18IO_POWER_ON_DLY_LEN   (11U)
#define HBN_LDO18IO_POWER_ON_DLY_MSK   (((1U << HBN_LDO18IO_POWER_ON_DLY_LEN) - 1) << HBN_LDO18IO_POWER_ON_DLY_POS)
#define HBN_LDO18IO_POWER_ON_DLY_UMSK  (~(((1U << HBN_LDO18IO_POWER_ON_DLY_LEN) - 1) << HBN_LDO18IO_POWER_ON_DLY_POS))
#define HBN_LDO18IO_POWER_OFF_DLY      HBN_LDO18IO_POWER_OFF_DLY
#define HBN_LDO18IO_POWER_OFF_DLY_POS  (11U)
#define HBN_LDO18IO_POWER_OFF_DLY_LEN  (5U)
#define HBN_LDO18IO_POWER_OFF_DLY_MSK  (((1U << HBN_LDO18IO_POWER_OFF_DLY_LEN) - 1) << HBN_LDO18IO_POWER_OFF_DLY_POS)
#define HBN_LDO18IO_POWER_OFF_DLY_UMSK (~(((1U << HBN_LDO18IO_POWER_OFF_DLY_LEN) - 1) << HBN_LDO18IO_POWER_OFF_DLY_POS))
#define HBN_LDO18IO_POWER_DLY_STS      HBN_LDO18IO_POWER_DLY_STS
#define HBN_LDO18IO_POWER_DLY_STS_POS  (16U)
#define HBN_LDO18IO_POWER_DLY_STS_LEN  (8U)
#define HBN_LDO18IO_POWER_DLY_STS_MSK  (((1U << HBN_LDO18IO_POWER_DLY_STS_LEN) - 1) << HBN_LDO18IO_POWER_DLY_STS_POS)
#define HBN_LDO18IO_POWER_DLY_STS_UMSK (~(((1U << HBN_LDO18IO_POWER_DLY_STS_LEN) - 1) << HBN_LDO18IO_POWER_DLY_STS_POS))
#define HBN_CORE_UNHALT                HBN_CORE_UNHALT
#define HBN_CORE_UNHALT_POS            (25U)
#define HBN_CORE_UNHALT_LEN            (1U)
#define HBN_CORE_UNHALT_MSK            (((1U << HBN_CORE_UNHALT_LEN) - 1) << HBN_CORE_UNHALT_POS)
#define HBN_CORE_UNHALT_UMSK           (~(((1U << HBN_CORE_UNHALT_LEN) - 1) << HBN_CORE_UNHALT_POS))
#define HBN_USER_BOOT_SEL              HBN_USER_BOOT_SEL
#define HBN_USER_BOOT_SEL_POS          (26U)
#define HBN_USER_BOOT_SEL_LEN          (2U)
#define HBN_USER_BOOT_SEL_MSK          (((1U << HBN_USER_BOOT_SEL_LEN) - 1) << HBN_USER_BOOT_SEL_POS)
#define HBN_USER_BOOT_SEL_UMSK         (~(((1U << HBN_USER_BOOT_SEL_LEN) - 1) << HBN_USER_BOOT_SEL_POS))
#define HBN_RELEASE_CORE               HBN_RELEASE_CORE
#define HBN_RELEASE_CORE_POS           (28U)
#define HBN_RELEASE_CORE_LEN           (4U)
#define HBN_RELEASE_CORE_MSK           (((1U << HBN_RELEASE_CORE_LEN) - 1) << HBN_RELEASE_CORE_POS)
#define HBN_RELEASE_CORE_UMSK          (~(((1U << HBN_RELEASE_CORE_LEN) - 1) << HBN_RELEASE_CORE_POS))

/* 0x108 : HBN_RSV3 */
#define HBN_XTAL_TYPE                  HBN_XTAL_TYPE
#define HBN_XTAL_TYPE_POS              (0U)
#define HBN_XTAL_TYPE_LEN              (4U)
#define HBN_XTAL_TYPE_MSK              (((1U << HBN_XTAL_TYPE_LEN) - 1) << HBN_XTAL_TYPE_POS)
#define HBN_XTAL_TYPE_UMSK             (~(((1U << HBN_XTAL_TYPE_LEN) - 1) << HBN_XTAL_TYPE_POS))
#define HBN_XTAL_STS                   HBN_XTAL_STS
#define HBN_XTAL_STS_POS               (4U)
#define HBN_XTAL_STS_LEN               (4U)
#define HBN_XTAL_STS_MSK               (((1U << HBN_XTAL_STS_LEN) - 1) << HBN_XTAL_STS_POS)
#define HBN_XTAL_STS_UMSK              (~(((1U << HBN_XTAL_STS_LEN) - 1) << HBN_XTAL_STS_POS))
#define HBN_FLASH_POWER_DLY            HBN_FLASH_POWER_DLY
#define HBN_FLASH_POWER_DLY_POS        (8U)
#define HBN_FLASH_POWER_DLY_LEN        (8U)
#define HBN_FLASH_POWER_DLY_MSK        (((1U << HBN_FLASH_POWER_DLY_LEN) - 1) << HBN_FLASH_POWER_DLY_POS)
#define HBN_FLASH_POWER_DLY_UMSK       (~(((1U << HBN_FLASH_POWER_DLY_LEN) - 1) << HBN_FLASH_POWER_DLY_POS))
#define HBN_FLASH_POWER_STS            HBN_FLASH_POWER_STS
#define HBN_FLASH_POWER_STS_POS        (16U)
#define HBN_FLASH_POWER_STS_LEN        (4U)
#define HBN_FLASH_POWER_STS_MSK        (((1U << HBN_FLASH_POWER_STS_LEN) - 1) << HBN_FLASH_POWER_STS_POS)
#define HBN_FLASH_POWER_STS_UMSK       (~(((1U << HBN_FLASH_POWER_STS_LEN) - 1) << HBN_FLASH_POWER_STS_POS))
#define PDS_GPIO_KEEP_PIN              PDS_GPIO_KEEP_PIN
#define PDS_GPIO_KEEP_PIN_POS          (20U)
#define PDS_GPIO_KEEP_PIN_LEN          (4U)
#define PDS_GPIO_KEEP_PIN_MSK          (((1U << PDS_GPIO_KEEP_PIN_LEN) - 1) << PDS_GPIO_KEEP_PIN_POS)
#define PDS_GPIO_KEEP_PIN_UMSK         (~(((1U << PDS_GPIO_KEEP_PIN_LEN) - 1) << PDS_GPIO_KEEP_PIN_POS))
#define HBN_GPIO_KEEP_PIN              HBN_GPIO_KEEP_PIN
#define HBN_GPIO_KEEP_PIN_POS          (24U)
#define HBN_GPIO_KEEP_PIN_LEN          (4U)
#define HBN_GPIO_KEEP_PIN_MSK          (((1U << HBN_GPIO_KEEP_PIN_LEN) - 1) << HBN_GPIO_KEEP_PIN_POS)
#define HBN_GPIO_KEEP_PIN_UMSK         (~(((1U << HBN_GPIO_KEEP_PIN_LEN) - 1) << HBN_GPIO_KEEP_PIN_POS))
#define PDS_GPIO_KEEP_STS              PDS_GPIO_KEEP_STS
#define PDS_GPIO_KEEP_STS_POS          (28U)
#define PDS_GPIO_KEEP_STS_LEN          (2U)
#define PDS_GPIO_KEEP_STS_MSK          (((1U << PDS_GPIO_KEEP_STS_LEN) - 1) << PDS_GPIO_KEEP_STS_POS)
#define PDS_GPIO_KEEP_STS_UMSK         (~(((1U << PDS_GPIO_KEEP_STS_LEN) - 1) << PDS_GPIO_KEEP_STS_POS))
#define HBN_GPIO_KEEP_STS              HBN_GPIO_KEEP_STS
#define HBN_GPIO_KEEP_STS_POS          (30U)
#define HBN_GPIO_KEEP_STS_LEN          (2U)
#define HBN_GPIO_KEEP_STS_MSK          (((1U << HBN_GPIO_KEEP_STS_LEN) - 1) << HBN_GPIO_KEEP_STS_POS)
#define HBN_GPIO_KEEP_STS_UMSK         (~(((1U << HBN_GPIO_KEEP_STS_LEN) - 1) << HBN_GPIO_KEEP_STS_POS))

/*@} end of group HBN_Public_Macros */

/** @defgroup  HBN_Public_Functions
 *  @{
 */
/*----------*/
#ifndef BFLB_USE_HAL_DRIVER
void HBN_OUT0_IRQHandler(void);
void HBN_OUT1_IRQHandler(void);
#endif
/*----------*/
void HBN_Mode_Enter(HBN_APP_CFG_Type *cfg);
void HBN_Power_Down_Flash(spi_flash_cfg_type *flashCfg);
void HBN_Enable(uint32_t aGPIOIeCfg, uint8_t ldoLevel, uint8_t hbnLevel, uint8_t dcdcPuSeq);
BL_Err_Type HBN_Reset(void);
/*----------*/
BL_Err_Type HBN_Set_GPADC_CLK_Sel(uint8_t clkSel);
BL_Err_Type HBN_PIR_Enable(void);
BL_Err_Type HBN_PIR_Disable(void);
BL_Err_Type HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg);
BL_Err_Type HBN_PIR_LPF_Sel(uint8_t lpf);
BL_Err_Type HBN_PIR_HPF_Sel(uint8_t hpf);
BL_Err_Type HBN_Set_PIR_Threshold(uint16_t threshold);
uint16_t HBN_Get_PIR_Threshold(void);
BL_Err_Type HBN_Set_PIR_Interval(uint16_t interval);
uint16_t HBN_Get_PIR_Interval(void);
/*----------*/
BL_Sts_Type HBN_Get_BOD_OUT_State(void);
BL_Err_Type HBN_Set_BOD_Config(uint8_t enable, uint8_t threshold, uint8_t mode);
/*----------*/
BL_Err_Type HBN_Set_Ldo11_Aon_Vout(uint8_t ldoLevel);
BL_Err_Type HBN_Set_Ldo11_Rt_Vout(uint8_t ldoLevel);
BL_Err_Type HBN_Set_Ldo11_Soc_Vout(uint8_t ldoLevel);
BL_Err_Type HBN_Set_Ldo11_All_Vout(uint8_t ldoLevel);
/*----------*/
BL_Err_Type HBN_32K_Sel(uint8_t clkType);
BL_Err_Type HBN_Set_UART_CLK_Sel(uint8_t clkSel);
uint8_t HBN_Get_MCU_XCLK_Sel(void);
BL_Err_Type HBN_Set_MCU_XCLK_Sel(uint8_t xclk);
uint8_t HBN_Get_MCU_Root_CLK_Sel(void);
BL_Err_Type HBN_Set_MCU_Root_CLK_Sel(uint8_t rootClk);
/*----------*/
BL_Err_Type HBN_Set_HRAM_slp(void);
BL_Err_Type HBN_Set_HRAM_Ret(void);
/*----------*/
uint32_t HBN_Get_Status_Flag(void);
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag);
uint32_t HBN_Get_Wakeup_Addr(void);
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr);
/*----------*/
uint8_t HBN_Get_Core_Unhalt_Config(void);
BL_Err_Type HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg);
uint8_t HBN_Get_User_Boot_Config(void);
BL_Err_Type HBN_Set_User_Boot_Config(uint8_t ubCfg);
BL_Err_Type HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly);
BL_Err_Type HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly);
/*----------*/
BL_Err_Type HBN_Set_Xtal_Type(uint8_t xtalType);
BL_Err_Type HBN_Get_Xtal_Type(uint8_t *xtalType);
BL_Err_Type HBN_Get_Xtal_Value(uint32_t *xtalVal);
BL_Err_Type HBN_Set_Flash_Power_Delay(uint8_t flashPwrDly);
BL_Err_Type HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly);
BL_Err_Type HBN_Set_Gpio_Keep(uint8_t gpioKeep);
BL_Err_Type HBN_Get_Gpio_Keep(uint8_t *gpioKeep);
BL_Err_Type HBN_Clear_Gpio_Keep(uint8_t gpioKeep);
BL_Err_Type HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep);
BL_Err_Type HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep);
BL_Err_Type HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep);

/*----------*/
BL_Err_Type HBN_Clear_RTC_Counter(void);
BL_Err_Type HBN_Enable_RTC_Counter(void);
BL_Err_Type HBN_Set_RTC_Timer(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode);
BL_Err_Type HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh);
BL_Err_Type HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter);
BL_Err_Type HBN_Clear_RTC_IRQ(void);
/*----------*/
BL_Err_Type HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType);
BL_Err_Type HBN_GPIO_INT_Disable(void);
BL_Sts_Type HBN_Get_INT_State(uint8_t irqType);
uint8_t HBN_Get_Pin_Wakeup_Mode(void);
BL_Err_Type HBN_Clear_IRQ(uint8_t irqType);
BL_Err_Type HBN_Hw_Pu_Pd_Cfg(uint8_t enable);
BL_Err_Type HBN_Pin_WakeUp_Mask(uint8_t maskVal);
BL_Err_Type HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2);
BL_Err_Type HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg);
BL_Err_Type HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio);
/*----------*/
BL_Err_Type HBN_Enable_AComp_IRQ(uint8_t acompId, uint8_t edge);
BL_Err_Type HBN_Disable_AComp_IRQ(uint8_t acompId, uint8_t edge);
/*----------*/
BL_Err_Type HBN_Enable_BOD_IRQ(void);
BL_Err_Type HBN_Disable_BOD_IRQ(void);
/*----------*/
BL_Err_Type HBN_Out0_Callback_Install(uint8_t intType, intCallback_Type *cbFun);
BL_Err_Type HBN_Out1_Callback_Install(uint8_t intType, intCallback_Type *cbFun);
/*----------*/
BL_Err_Type HBN_Aon_Pad_WakeUpCfg(BL_Fun_Type puPdEn, uint8_t trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec);
/*----------*/
BL_Err_Type HBN_Set_Xtal_32K_Capbank(uint8_t value);
BL_Err_Type HBN_Set_Xtal_32K_Inverter_Amplify_Strength(uint8_t value);
BL_Err_Type HBN_Set_Xtal_32K_Regulator(uint8_t level);
BL_Err_Type HBN_Power_On_Xtal_32K(void);
BL_Err_Type HBN_Power_Off_Xtal_32K(void);
BL_Err_Type HBN_Keep_On_RC32K(void);
BL_Err_Type HBN_Power_Off_RC32K(void);
BL_Err_Type HBN_Trim_Ldo33VoutTrim(void);
BL_Err_Type HBN_Trim_RC32K(void);
BL_Err_Type HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg);
void HBN_Get_Reset_Event(uint8_t *event);
void HBN_Clr_Reset_Event(void);
BL_Err_Type HBN_Clear_RTC_INT(void);
/*----------*/

/*@} end of group HBN_Public_Functions */

/*@} end of group HBN */

/*@} end of group BL616_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616_HBN_H__ */
