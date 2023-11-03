/**
  ******************************************************************************
  * @file    bl702l_hbn.h
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
#ifndef __BL702L_HBN_H__
#define __BL702L_HBN_H__

#include "hbn_reg.h"
#include "bl702l_aon.h"
#include "bl702l_common.h"
#include "bflb_sflash.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
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
    BL_Fun_Type lowIntEn;  /*!< Low will trigger interrupt */
    BL_Fun_Type highIntEn; /*!< High will trigger interrupt */
} HBN_PIR_INT_CFG_Type;

/**
 *  @brief HBN PIR low pass filter type definition
 */
typedef enum {
    HBN_PIR_LPF_DIV1, /*!< HBN PIR lpf div 1 */
    HBN_PIR_LPF_DIV2, /*!< HBN PIR lpf div 2 */
} HBN_PIR_LPF_Type;

/**
 *  @brief HBN PIR high pass filter type definition
 */
typedef enum {
    HBN_PIR_HPF_METHOD0, /*!< HBN PIR hpf calc method 0, 1-z^-1 */
    HBN_PIR_HPF_METHOD1, /*!< HBN PIR hpf calc method 1, 1-z^-2 */
    HBN_PIR_HPF_METHOD2, /*!< HBN PIR hpf calc method 2, 1-z^-3 */
} HBN_PIR_HPF_Type;

/**
 *  @brief HBN BOR threshold type definition
 */
typedef enum {
    HBN_BOR_THRES_1P80V, /*!< BOR threshold 1.80V */
    HBN_BOR_THRES_1P85V, /*!< BOR threshold 1.85V */
    HBN_BOR_THRES_1P90V, /*!< BOR threshold 1.90V */
    HBN_BOR_THRES_1P95V, /*!< BOR threshold 1.95V */
    HBN_BOR_THRES_2P00V, /*!< BOR threshold 2.00V */
    HBN_BOR_THRES_2P05V, /*!< BOR threshold 2.05V */
    HBN_BOR_THRES_2P10V, /*!< BOR threshold 2.10V */
    HBN_BOR_THRES_2P15V, /*!< BOR threshold 2.15V */
} HBN_BOR_THRES_Type;

/**
 *  @brief HBN BOR mode type definition
 */
typedef enum {
    HBN_BOR_MODE_POR_INDEPENDENT, /*!< POR is independent of BOR */
    HBN_BOR_MODE_POR_RELEVANT,    /*!< POR is relevant to BOR */
} HBN_BOR_MODE_Type;

/**
 *  @brief HBN 32K clock type definition
 */
typedef enum {
    HBN_32K_RC = 0,  /*!< HBN use rc 32k */
    HBN_32K_XTAL,    /*!< HBN use xtal 32k */
    HBN_32K_DIG = 3, /*!< HBN use dig 32k */
} HBN_32K_CLK_Type;

/**
 *  @brief HBN xclk clock type definition
 */
typedef enum {
    HBN_XCLK_CLK_RC32M, /*!< use RC32M as xclk clock */
    HBN_XCLK_CLK_XTAL,  /*!< use XTAL as xclk clock */
} HBN_XCLK_CLK_Type;

/**
 *  @brief HBN root clock type definition
 */
typedef enum {
    HBN_ROOT_CLK_RC32M, /*!< use RC32M as root clock */
    HBN_ROOT_CLK_XTAL,  /*!< use XTAL as root clock */
    HBN_ROOT_CLK_DLL,   /*!< use DLL as root clock */
} HBN_ROOT_CLK_Type;

/**
 *  @brief HBN UART clock type definition
 */
typedef enum {
    HBN_UART_CLK_FCLK = 0, /*!< Select FCLK as UART clock */
    HBN_UART_CLK_64M,      /*!< Select 64M as UART clock */
    HBN_UART_CLK_XCLK,     /*!< Select XCLK as UART clock */
} HBN_UART_CLK_Type;

/**
 *  @brief HBN RTC interrupt delay type definition
 */
typedef enum {
    HBN_RTC_INT_DELAY_32T = 0, /*!< HBN RTC interrupt delay 32T */
    HBN_RTC_INT_DELAY_0T = 1,  /*!< HBN RTC interrupt delay 0T */
} HBN_RTC_INT_Delay_Type;

/**
 *  @brief HBN aon pad type definition
 */
typedef enum {
    HBN_AON_PAD_GPIO9 = 0,  /*!< HBN aon pad type: GPIO16 */
    HBN_AON_PAD_GPIO10 = 1, /*!< HBN aon pad type: GPIO17 */
    HBN_AON_PAD_GPIO11 = 2, /*!< HBN aon pad type: GPIO18 */
    HBN_AON_PAD_GPIO12 = 3, /*!< HBN aon pad type: GPIO19 */
    HBN_AON_PAD_GPIO13 = 4, /*!< HBN aon pad type: GPIO19 */
    HBN_AON_PAD_GPIO30 = 5, /*!< HBN aon pad type: GPIO19 */
    HBN_AON_PAD_GPIO31 = 6, /*!< HBN aon pad type: GPIO19 */
} HBN_AON_PAD_Type;

/**
 *  @brief HBN interrupt type definition
 */
typedef enum {
    HBN_INT_GPIO9 = 0,   /*!< HBN interrupt type: GPIO9 */
    HBN_INT_GPIO10 = 1,  /*!< HBN interrupt type: GPIO10 */
    HBN_INT_GPIO11 = 2,  /*!< HBN interrupt type: GPIO11 */
    HBN_INT_GPIO12 = 3,  /*!< HBN interrupt type: GPIO12 */
    HBN_INT_GPIO13 = 4,  /*!< HBN interrupt type: GPIO13 */
    HBN_INT_GPIO30 = 5,  /*!< HBN interrupt type: GPIO30 */
    HBN_INT_GPIO31 = 6,  /*!< HBN interrupt type: GPIO31 */
    HBN_INT_GPIO8 = 7,   /*!< HBN interrupt type: GPIO8 */
    HBN_INT_GPIO14 = 8,  /*!< HBN interrupt type: GPIO14 */
    HBN_INT_GPIO22 = 9,  /*!< HBN interrupt type: GPIO22 */
    HBN_INT_RTC = 16,    /*!< HBN interrupt type: RTC */
    HBN_INT_BOD = 18,    /*!< HBN interrupt type: BOR */
    HBN_INT_ACOMP0 = 20, /*!< HBN interrupt type: ACOMP0 */
    HBN_INT_ACOMP1 = 22, /*!< HBN interrupt type: ACOMP1 */
} HBN_INT_Type;

/**
 *  @brief HBN acomp interrupt type definition
 */
typedef enum {
    HBN_ACOMP_INT_EDGE_POSEDGE = 1,         /*!< HBN acomp interrupt edge posedge */
    HBN_ACOMP_INT_EDGE_NEGEDGE = 2,         /*!< HBN acomp interrupt edge negedge */
    HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE = 3, /*!< HBN acomp interrupt edge posedge and negedge */
} HBN_ACOMP_INT_EDGE_Type;

/**
 *  @brief HBN reset event type definition
 */
typedef enum {
    HBN_RST_EVENT_POR_OUT,   /*!< por_out event */
    HBN_RST_EVENT_EXT_RST_N, /*!< ext_rst_n event */
    HBN_RST_EVENT_SW_RST,    /*!< sw_rst event */
    HBN_RST_EVENT_PWR_RST_N, /*!< pwr_rst_n event */
    HBN_RST_EVENT_BOR_OUT,   /*!< bor_out_ event */
} HBN_RST_EVENT_Type;

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

/**
 *  @brief HBN GPIO interrupt trigger type definition
 */
typedef enum {
    HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE,  /*!< HBN GPIO INT trigger type: sync falling edge trigger */
    HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE,   /*!< HBN GPIO INT trigger type: sync rising edge trigger */
    HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL,     /*!< HBN GPIO INT trigger type: sync low level trigger */
    HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL,    /*!< HBN GPIO INT trigger type: sync high level trigger */
    HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE, /*!< HBN GPIO INT trigger type: async falling edge trigger */
    HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE,  /*!< HBN GPIO INT trigger type: async rising edge trigger */
    HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL,    /*!< HBN GPIO INT trigger type: async low level trigger */
    HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL,   /*!< HBN GPIO INT trigger type: async high level trigger */
} HBN_GPIO_INT_Trigger_Type;

/**
 *  @brief HBN OUT0 interrupt type definition
 */
typedef enum {
    HBN_OUT0_INT_GPIO9 = 0,  /*!< HBN out 0 interrupt type: GPIO9 */
    HBN_OUT0_INT_GPIO10 = 1, /*!< HBN out 0 interrupt type: GPIO10 */
    HBN_OUT0_INT_GPIO11 = 2, /*!< HBN out 0 interrupt type: GPIO11 */
    HBN_OUT0_INT_GPIO12 = 3, /*!< HBN out 0 interrupt type: GPIO12 */
    HBN_OUT0_INT_GPIO13 = 4, /*!< HBN out 0 interrupt type: GPIO13 */
    HBN_OUT0_INT_GPIO30 = 5, /*!< HBN out 0 interrupt type: GPIO30 */
    HBN_OUT0_INT_GPIO31 = 6, /*!< HBN out 0 interrupt type: GPIO31 */
    HBN_OUT0_INT_GPIO8 = 7,  /*!< HBN out 0 interrupt type: GPIO8 */
    HBN_OUT0_INT_GPIO14 = 8, /*!< HBN out 0 interrupt type: GPIO14 */
    HBN_OUT0_INT_GPIO22 = 9, /*!< HBN out 0 interrupt type: GPIO22 */
    HBN_OUT0_INT_RTC = 16,   /*!< HBN out 0 interrupt type: RTC */
    HBN_OUT0_INT_MAX,        /*!< HBN out 0 max num */
} HBN_OUT0_INT_Type;

/**
 *  @brief HBN OUT0 interrupt type definition
 */
typedef enum {
    HBN_OUT1_INT_BOD,    /*!< HBN out 1 interrupt type: BOD */
    HBN_OUT1_INT_ACOMP0, /*!< HBN out 1 interrupt type: ACOMP0 */
    HBN_OUT1_INT_ACOMP1, /*!< HBN out 1 interrupt type: ACOMP1 */
    HBN_OUT1_INT_MAX,    /*!< HBN out 1 max num */
} HBN_OUT1_INT_Type;

/**
 *  @brief HBN LDO level type definition
 */
typedef enum {
    HBN_LDO11_LEVEL_0P60V = 0,  /*!< VDD11_AON voltage 0.60V */
    HBN_LDO11_LEVEL_0P65V = 1,  /*!< VDD11_AON voltage 0.65V */
    HBN_LDO11_LEVEL_0P70V = 2,  /*!< VDD11_AON voltage 0.70V */
    HBN_LDO11_LEVEL_0P75V = 3,  /*!< VDD11_AON voltage 0.75V */
    HBN_LDO11_LEVEL_0P80V = 4,  /*!< VDD11_AON voltage 0.80V */
    HBN_LDO11_LEVEL_0P85V = 5,  /*!< VDD11_AON voltage 0.85V */
    HBN_LDO11_LEVEL_0P90V = 6,  /*!< VDD11_AON voltage 0.90V */
    HBN_LDO11_LEVEL_0P95V = 7,  /*!< VDD11_AON voltage 0.95V */
    HBN_LDO11_LEVEL_1P00V = 8,  /*!< VDD11_AON voltage 1.00V */
    HBN_LDO11_LEVEL_1P05V = 9,  /*!< VDD11_AON voltage 1.05V */
    HBN_LDO11_LEVEL_1P10V = 10, /*!< VDD11_AON voltage 1.10V */
} HBN_LDO11_LEVEL_Type;

/**
 *  @brief HBN LDO level type definition
 */
typedef enum {
    HBN_LDO_LEVEL_0P60V = 0,  /*!< HBN LDO voltage 0.60V */
    HBN_LDO_LEVEL_0P65V = 1,  /*!< HBN LDO voltage 0.65V */
    HBN_LDO_LEVEL_0P70V = 2,  /*!< HBN LDO voltage 0.70V */
    HBN_LDO_LEVEL_0P75V = 3,  /*!< HBN LDO voltage 0.75V */
    HBN_LDO_LEVEL_0P80V = 4,  /*!< HBN LDO voltage 0.80V */
    HBN_LDO_LEVEL_0P85V = 5,  /*!< HBN LDO voltage 0.85V */
    HBN_LDO_LEVEL_0P90V = 6,  /*!< HBN LDO voltage 0.90V */
    HBN_LDO_LEVEL_0P95V = 7,  /*!< HBN LDO voltage 0.95V */
    HBN_LDO_LEVEL_1P00V = 8,  /*!< HBN LDO voltage 1.00V */
    HBN_LDO_LEVEL_1P05V = 9,  /*!< HBN LDO voltage 1.05V */
    HBN_LDO_LEVEL_1P10V = 10, /*!< HBN LDO voltage 1.10V */
    HBN_LDO_LEVEL_1P15V = 11, /*!< HBN LDO voltage 1.15V */
    HBN_LDO_LEVEL_1P20V = 12, /*!< HBN LDO voltage 1.20V */
    HBN_LDO_LEVEL_1P25V = 13, /*!< HBN LDO voltage 1.25V */
    HBN_LDO_LEVEL_1P30V = 14, /*!< HBN LDO voltage 1.30V */
    HBN_LDO_LEVEL_1P35V = 15, /*!< HBN LDO voltage 1.35V */
} HBN_LDO_LEVEL_Type;

/**
 *  @brief HBN LDO11RT drive strength type definition
 */
typedef enum {
    HBN_LDO11RT_DRIVE_STRENGTH_5_50UA = 0,   /*!< HBN LDO11RT drive strength 0: 5uA to 50uA */
    HBN_LDO11RT_DRIVE_STRENGTH_10_100UA = 1, /*!< HBN LDO11RT drive strength 1: 10uA to 100uA */
    HBN_LDO11RT_DRIVE_STRENGTH_15_150UA = 2, /*!< HBN LDO11RT drive strength 2: 15uA to 150uA */
    HBN_LDO11RT_DRIVE_STRENGTH_25_250UA = 3, /*!< HBN LDO11RT drive strength 3: 25uA to 250uA */
} HBN_LDO11RT_DRIVE_STRENGTH_Type;

/**
 *  @brief HBN level type definition
 */
typedef enum {
    HBN_LEVEL_0, /*!< HBN pd_core */
    HBN_LEVEL_1, /*!< HBN pd_aon_hbncore + pd_core */
    HBN_LEVEL_2, /*!< HBN pd_aon_hbnrtc + pd_aon_hbncore + pd_core */
} HBN_LEVEL_Type;

/**
 *  @brief HBN BOR configuration type definition
 */
typedef struct
{
    uint8_t enableBor;      /*!< Enable BOR or not */
    uint8_t enableBorInt;   /*!< Enable BOR interrupt or not */
    uint8_t borThreshold;   /*!< BOR threshold */
    uint8_t enablePorInBor; /*!< Enable POR when BOR occure or not */
} HBN_BOR_CFG_Type;

/**
 *  @brief HBN APP configuration type definition
 */
typedef struct
{
    uint8_t useXtal32k;           /*!< Whether use xtal 32K as 32K clock source,otherwise use rc32k */
    uint32_t sleepTime;           /*!< HBN sleep time */
    uint8_t hw_pu_pd_en;          /*!< Pull up or pull down enable in the hbn mode */
    spi_flash_cfg_type *flashCfg; /*!< Flash config pointer, used when power down flash */
    HBN_LEVEL_Type hbnLevel;      /*!< HBN level */
    HBN_LDO_LEVEL_Type ldoLevel;  /*!< LDO level */
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

/** @defgroup  HBN_BOR_THRES_TYPE
 *  @{
 */
#define IS_HBN_BOR_THRES_TYPE(type) (((type) == HBN_BOR_THRES_1P80V) ||\
                                     ((type) == HBN_BOR_THRES_1P85V) ||\
                                     ((type) == HBN_BOR_THRES_1P90V) ||\
                                     ((type) == HBN_BOR_THRES_1P95V) ||\
                                     ((type) == HBN_BOR_THRES_2P00V) ||\
                                     ((type) == HBN_BOR_THRES_2P05V) ||\
                                     ((type) == HBN_BOR_THRES_2P10V) ||\
                                     ((type) == HBN_BOR_THRES_2P15V))

/** @defgroup  HBN_BOR_MODE_TYPE
 *  @{
 */
#define IS_HBN_BOR_MODE_TYPE(type) (((type) == HBN_BOR_MODE_POR_INDEPENDENT) || \
                                    ((type) == HBN_BOR_MODE_POR_RELEVANT))

/** @defgroup  HBN_32K_CLK_TYPE
 *  @{
 */
#define IS_HBN_32K_CLK_TYPE(type) (((type) == HBN_32K_RC) ||   \
                                   ((type) == HBN_32K_XTAL) || \
                                   ((type) == HBN_32K_DIG))

/** @defgroup  HBN_XCLK_CLK_TYPE
 *  @{
 */
#define IS_HBN_XCLK_CLK_TYPE(type) (((type) == HBN_XCLK_CLK_RC32M) || \
                                    ((type) == HBN_XCLK_CLK_XTAL))

/** @defgroup  HBN_ROOT_CLK_TYPE
 *  @{
 */
#define IS_HBN_ROOT_CLK_TYPE(type) (((type) == HBN_ROOT_CLK_RC32M) || \
                                    ((type) == HBN_ROOT_CLK_XTAL) ||  \
                                    ((type) == HBN_ROOT_CLK_DLL))

/** @defgroup  HBN_UART_CLK_TYPE
 *  @{
 */
#define IS_HBN_UART_CLK_TYPE(type) (((type) == HBN_UART_CLK_FCLK) || \
                                    ((type) == HBN_UART_CLK_64M) ||  \
                                    ((type) == HBN_UART_CLK_XCLK))

/** @defgroup  HBN_RTC_INT_DELAY_TYPE
 *  @{
 */
#define IS_HBN_RTC_INT_DELAY_TYPE(type) (((type) == HBN_RTC_INT_DELAY_32T) || \
                                         ((type) == HBN_RTC_INT_DELAY_0T))

/** @defgroup  HBN_INT_TYPE
 *  @{
 */
#define IS_HBN_INT_TYPE(type) (((type) == HBN_INT_GPIO9) ||  \
                               ((type) == HBN_INT_GPIO10) || \
                               ((type) == HBN_INT_GPIO11) || \
                               ((type) == HBN_INT_GPIO12) || \
                               ((type) == HBN_INT_GPIO13) || \
                               ((type) == HBN_INT_RTC) ||    \
                               ((type) == HBN_INT_PIR) ||    \
                               ((type) == HBN_INT_BOR) ||    \
                               ((type) == HBN_INT_ACOMP0) || \
                               ((type) == HBN_INT_ACOMP1))

/** @defgroup  HBN_ACOMP_INT_EDGE_TYPE
 *  @{
 */
#define IS_HBN_ACOMP_INT_EDGE_TYPE(type) (((type) == HBN_ACOMP_INT_EDGE_POSEDGE) || \
                                          ((type) == HBN_ACOMP_INT_EDGE_NEGEDGE) || \
                                          ((type) == HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE))
/** @defgroup  HBN_RST_EVENT_TYPE
 *  @{
 */
#define IS_HBN_RST_EVENT_TYPE(type) (((type) == HBN_RST_EVENT_POR_OUT) ||   \
                                     ((type) == HBN_RST_EVENT_EXT_RST_N) || \
                                     ((type) == HBN_RST_EVENT_SW_RST) ||    \
                                     ((type) == HBN_RST_EVENT_PWR_RST_N) || \
                                     ((type) == HBN_RST_EVENT_BOR_OUT))

/** @defgroup  HBN_GPIO_INT_TRIGGER_TYPE
 *  @{
 */
#define IS_HBN_GPIO_INT_TRIGGER_TYPE(type) (((type) == HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE) ||  \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE) ||   \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL) ||     \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL) ||    \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE) || \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE) ||  \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL) ||    \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL))


/** @defgroup  HBN_AON_PAD_TYPE
 *  @{
 */
#define IS_HBN_AON_PAD_TYPE(type) (((type) == HBN_AON_PAD_GPIO9) || \
                                   ((type) == HBN_AON_PAD_GPIO10) || \
                                   ((type) == HBN_AON_PAD_GPIO11) || \
                                   ((type) == HBN_AON_PAD_GPIO12) || \
                                   ((type) == HBN_AON_PAD_GPIO13) || \
                                   ((type) == HBN_AON_PAD_GPIO30) || \
                                   ((type) == HBN_AON_PAD_GPIO31))

/** @defgroup  HBN_OUT0_INT_TYPE
 *  @{
 */
#define IS_HBN_OUT0_INT_TYPE(type) (((type) == HBN_OUT0_INT_GPIO9) ||  \
                                    ((type) == HBN_OUT0_INT_GPIO10) || \
                                    ((type) == HBN_OUT0_INT_GPIO11) || \
                                    ((type) == HBN_OUT0_INT_GPIO12) || \
                                    ((type) == HBN_OUT0_INT_GPIO13) || \
                                    ((type) == HBN_OUT0_INT_RTC) ||    \
                                    ((type) == HBN_OUT0_MAX))

/** @defgroup  HBN_OUT1_INT_TYPE
 *  @{
 */
#define IS_HBN_OUT1_INT_TYPE(type) (((type) == HBN_OUT1_INT_PIR) ||    \
                                    ((type) == HBN_OUT1_INT_BOR) ||    \
                                    ((type) == HBN_OUT1_INT_ACOMP0) || \
                                    ((type) == HBN_OUT1_INT_ACOMP1) || \
                                    ((type) == HBN_OUT1_MAX))

/** @defgroup  HBN_LDO11_LEVEL_Type
 *  @{
 */
#define IS_HBN_LDO11_LEVEL_TYPE(type) (((type) == HBN_LDO11_LEVEL_0P60V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P65V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P70V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P75V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P80V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P85V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P90V) || \
                                       ((type) == HBN_LDO11_LEVEL_0P95V) || \
                                       ((type) == HBN_LDO11_LEVEL_1P00V) || \
                                       ((type) == HBN_LDO11_LEVEL_1P05V) || \
                                       ((type) == HBN_LDO11_LEVEL_1P10V))

/** @defgroup  HBN_LDO_LEVEL_TYPE
 *  @{
 */
#define IS_HBN_LDO_LEVEL_TYPE(type) (((type) == HBN_LDO_LEVEL_0P60V) || \
                                     ((type) == HBN_LDO_LEVEL_0P65V) || \
                                     ((type) == HBN_LDO_LEVEL_0P70V) || \
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

/** @defgroup  HBN_LDO11RT_DRIVE_STRENGTH_TYPE
 *  @{
 */
#define IS_HBN_LDO11RT_DRIVE_STRENGTH_TYPE(type) (((type) == HBN_LDO11RT_DRIVE_STRENGTH_5_50UA) ||   \
                                                  ((type) == HBN_LDO11RT_DRIVE_STRENGTH_10_100UA) || \
                                                  ((type) == HBN_LDO11RT_DRIVE_STRENGTH_15_150UA) || \
                                                  ((type) == HBN_LDO11RT_DRIVE_STRENGTH_25_250UA))

/** @defgroup  HBN_LEVEL_TYPE
 *  @{
 */
#define IS_HBN_LEVEL_TYPE(type) (((type) == HBN_LEVEL_0) || \
                                 ((type) == HBN_LEVEL_1) || \
                                 ((type) == HBN_LEVEL_2))

/*@} end of group HBN_Public_Constants */

/** @defgroup  HBN_Public_Macros
 *  @{
 */
#define HBN_RAM_SIZE           (4 * 1024)
#define HBN_RTC_COMP_BIT0_39   0x01
#define HBN_RTC_COMP_BIT0_23   0x02
#define HBN_RTC_COMP_BIT13_39  0x04
#define HBN_STATUS_ENTER_FLAG  0x4e424845
#define HBN_STATUS_WAKEUP_FLAG 0x4e424857
#define HBN_RELEASE_CORE_FLAG  (0x4)
#define HBN_WAKEUP_GPIO_NONE   0x00
#define HBN_WAKEUP_GPIO_9      0x01
#define HBN_WAKEUP_GPIO_10     0x02
#define HBN_WAKEUP_GPIO_11     0x04
#define HBN_WAKEUP_GPIO_12     0x08
#define HBN_WAKEUP_GPIO_13     0x10
#define HBN_WAKEUP_GPIO_30     0x20
#define HBN_WAKEUP_GPIO_31     0x40
#define HBN_WAKEUP_GPIO_8      0x80
#define HBN_WAKEUP_GPIO_14     0x100
#define HBN_WAKEUP_GPIO_22     0x200
#define HBN_WAKEUP_GPIO_ALL    0x3FF

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
#define HBN_XTAL_TYPE            HBN_XTAL_TYPE
#define HBN_XTAL_TYPE_POS        (0U)
#define HBN_XTAL_TYPE_LEN        (4U)
#define HBN_XTAL_TYPE_MSK        (((1U << HBN_XTAL_TYPE_LEN) - 1) << HBN_XTAL_TYPE_POS)
#define HBN_XTAL_TYPE_UMSK       (~(((1U << HBN_XTAL_TYPE_LEN) - 1) << HBN_XTAL_TYPE_POS))
#define HBN_XTAL_STS             HBN_XTAL_STS
#define HBN_XTAL_STS_POS         (4U)
#define HBN_XTAL_STS_LEN         (4U)
#define HBN_XTAL_STS_MSK         (((1U << HBN_XTAL_STS_LEN) - 1) << HBN_XTAL_STS_POS)
#define HBN_XTAL_STS_UMSK        (~(((1U << HBN_XTAL_STS_LEN) - 1) << HBN_XTAL_STS_POS))
#define HBN_FLASH_POWER_DLY      HBN_FLASH_POWER_DLY
#define HBN_FLASH_POWER_DLY_POS  (8U)
#define HBN_FLASH_POWER_DLY_LEN  (8U)
#define HBN_FLASH_POWER_DLY_MSK  (((1U << HBN_FLASH_POWER_DLY_LEN) - 1) << HBN_FLASH_POWER_DLY_POS)
#define HBN_FLASH_POWER_DLY_UMSK (~(((1U << HBN_FLASH_POWER_DLY_LEN) - 1) << HBN_FLASH_POWER_DLY_POS))
#define HBN_FLASH_POWER_STS      HBN_FLASH_POWER_STS
#define HBN_FLASH_POWER_STS_POS  (16U)
#define HBN_FLASH_POWER_STS_LEN  (4U)
#define HBN_FLASH_POWER_STS_MSK  (((1U << HBN_FLASH_POWER_STS_LEN) - 1) << HBN_FLASH_POWER_STS_POS)
#define HBN_FLASH_POWER_STS_UMSK (~(((1U << HBN_FLASH_POWER_STS_LEN) - 1) << HBN_FLASH_POWER_STS_POS))
#define PDS_GPIO_KEEP_PIN        PDS_GPIO_KEEP_PIN
#define PDS_GPIO_KEEP_PIN_POS    (20U)
#define PDS_GPIO_KEEP_PIN_LEN    (4U)
#define PDS_GPIO_KEEP_PIN_MSK    (((1U << PDS_GPIO_KEEP_PIN_LEN) - 1) << PDS_GPIO_KEEP_PIN_POS)
#define PDS_GPIO_KEEP_PIN_UMSK   (~(((1U << PDS_GPIO_KEEP_PIN_LEN) - 1) << PDS_GPIO_KEEP_PIN_POS))
#define HBN_GPIO_KEEP_PIN        HBN_GPIO_KEEP_PIN
#define HBN_GPIO_KEEP_PIN_POS    (24U)
#define HBN_GPIO_KEEP_PIN_LEN    (4U)
#define HBN_GPIO_KEEP_PIN_MSK    (((1U << HBN_GPIO_KEEP_PIN_LEN) - 1) << HBN_GPIO_KEEP_PIN_POS)
#define HBN_GPIO_KEEP_PIN_UMSK   (~(((1U << HBN_GPIO_KEEP_PIN_LEN) - 1) << HBN_GPIO_KEEP_PIN_POS))
#define PDS_GPIO_KEEP_STS        PDS_GPIO_KEEP_STS
#define PDS_GPIO_KEEP_STS_POS    (28U)
#define PDS_GPIO_KEEP_STS_LEN    (2U)
#define PDS_GPIO_KEEP_STS_MSK    (((1U << PDS_GPIO_KEEP_STS_LEN) - 1) << PDS_GPIO_KEEP_STS_POS)
#define PDS_GPIO_KEEP_STS_UMSK   (~(((1U << PDS_GPIO_KEEP_STS_LEN) - 1) << PDS_GPIO_KEEP_STS_POS))
#define HBN_GPIO_KEEP_STS        HBN_GPIO_KEEP_STS
#define HBN_GPIO_KEEP_STS_POS    (30U)
#define HBN_GPIO_KEEP_STS_LEN    (2U)
#define HBN_GPIO_KEEP_STS_MSK    (((1U << HBN_GPIO_KEEP_STS_LEN) - 1) << HBN_GPIO_KEEP_STS_POS)
#define HBN_GPIO_KEEP_STS_UMSK   (~(((1U << HBN_GPIO_KEEP_STS_LEN) - 1) << HBN_GPIO_KEEP_STS_POS))

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
BL_Err_Type HBN_Reset(void);
BL_Err_Type HBN_App_Reset(uint8_t npXtalType, uint8_t bclkDiv, uint8_t apXtalType, uint8_t fclkDiv);
BL_Err_Type HBN_Disable(void);
/*----------*/
BL_Sts_Type HBN_Get_BOR_OUT_State(void);
BL_Err_Type HBN_Set_BOR_Config(uint8_t enable, HBN_BOR_THRES_Type threshold, HBN_BOR_MODE_Type mode);
/*----------*/
BL_Err_Type HBN_PDS_Set_Ldo11_Vout(HBN_LDO11_LEVEL_Type ldoLevel);
BL_Err_Type HBN_Set_Ldo11_Aon_Vout(HBN_LDO_LEVEL_Type ldoLevel);
BL_Err_Type HBN_Set_Ldo11_Soc_Vout(HBN_LDO_LEVEL_Type ldoLevel);
BL_Err_Type HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_Type ldoLevel);
/*----------*/
BL_Err_Type HBN_32K_Sel(HBN_32K_CLK_Type clkType);
BL_Err_Type HBN_Set_UART_CLK_Sel(HBN_UART_CLK_Type clkSel);
BL_Err_Type HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_Type xClk);
BL_Err_Type HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_Type rootClk);
/*----------*/
BL_Err_Type HBN_Set_HRAM_slp(void);
BL_Err_Type HBN_Set_HRAM_Ret(void);
/*----------*/
uint32_t HBN_Get_Status_Flag(void);
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag);
uint32_t HBN_Get_Wakeup_Addr(void);
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr);
uint8_t HBN_Get_User_Boot_Config(void);
BL_Err_Type HBN_Set_User_Boot_Config(uint8_t ubCfg);
/*----------*/
BL_Err_Type HBN_Clear_RTC_Counter(void);
BL_Err_Type HBN_Enable_RTC_Counter(void);
BL_Err_Type HBN_Set_RTC_Timer(HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode);
BL_Err_Type HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh);
BL_Err_Type HBN_Clear_RTC_INT(void);
/*----------*/
BL_Err_Type HBN_GPIO_INT_Enable(HBN_GPIO_INT_Trigger_Type gpioIntTrigType);
BL_Err_Type HBN_GPIO_INT_Disable(void);
BL_Sts_Type HBN_Get_INT_State(HBN_INT_Type irqType);
uint8_t HBN_Get_Pin_Wakeup_Mode(void);
BL_Err_Type HBN_Clear_IRQ(HBN_INT_Type irqType);
BL_Err_Type HBN_Hw_Pu_Pd_Cfg(uint8_t enable);
BL_Err_Type HBN_Comm_Pad_Wakeup_En_Cfg(uint8_t padCfg);
BL_Err_Type HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg);
BL_Err_Type HBN_Enable_PDS_Pad_Keep(uint32_t keepSel);
BL_Err_Type HBN_Pin_WakeUp_Mask(uint16_t maskVal);
void HBN_GPIO_Wakeup_Set(uint16_t gpio_wakeup_src, HBN_GPIO_INT_Trigger_Type gpio_trig_type);
BL_Err_Type HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, HBN_AON_PAD_Type aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg);
/*----------*/
BL_Err_Type HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge);
BL_Err_Type HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge);
/*----------*/
BL_Err_Type HBN_Enable_BOR_IRQ(void);
BL_Err_Type HBN_Disable_BOR_IRQ(void);
/*----------*/
BL_Sts_Type HBN_Get_Reset_Event(HBN_RST_EVENT_Type event);
BL_Err_Type HBN_Clear_Reset_Event(void);
/*----------*/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type HBN_Out0_IRQHandler_Install(void);
BL_Err_Type HBN_Out0_Callback_Install(HBN_OUT0_INT_Type intType, intCallback_Type *cbFun);
BL_Err_Type HBN_Out1_IRQHandler_Install(void);
BL_Err_Type HBN_Out1_Callback_Install(HBN_OUT1_INT_Type intType, intCallback_Type *cbFun);
#endif
/*----------*/
BL_Err_Type HBN_GPIO_Dbg_Pull_Cfg(BL_Fun_Type pupdEn, BL_Fun_Type dlyEn,
                                  uint8_t dlySec, HBN_INT_Type gpioIrq, BL_Mask_Type gpioMask);
/*----------*/
BL_Err_Type HBN_Power_On_Xtal_32K(void);
BL_Err_Type HBN_Power_Off_Xtal_32K(void);
BL_Err_Type HBN_Power_On_RC32K(void);
BL_Err_Type HBN_Power_Off_RC32K(void);
BL_Err_Type HBN_Trim_RC32K(void);
BL_Err_Type HBN_Set_BOR_Cfg(HBN_BOR_CFG_Type *cfg);
/*----------*/
void HBN_Enable(HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel);
/*----------*/

/*@} end of group HBN_Public_Functions */

/*@} end of group HBN */

/*@} end of group BL702L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_HBN_H__ */
