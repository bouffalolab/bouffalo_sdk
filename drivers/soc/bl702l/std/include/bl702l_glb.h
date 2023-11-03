/**
  ******************************************************************************
  * @file    bl702l_glb.h
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
#ifndef __BL702L_GLB_H__
#define __BL702L_GLB_H__

#include "glb_reg.h"
#include "pds_reg.h"
#include "bl702l_glb_gpio.h"
#include "bl702l_gpio.h"
#include "bl702l_l1c.h"
#include "bl702l_hbn.h"
#include "bl702l_aon.h"
#include "bl702l_ef_cfg.h"
#include "bl702l_pds.h"
#include "bl702l_common.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  GLB
 *  @{
 */

/** @defgroup  GLB_Public_Types
 *  @{
 */

/**
 *  @brief GLB IR_LED type definition
 */
typedef enum {
    GLB_IR_LED0, /*!< IR_LED0 */
    GLB_IR_LED1, /*!< IR_LED1 */
} GLB_IR_LED_Type;

/**
 *  @brief GLB root clock type definition
 */
typedef enum {
    GLB_ROOT_CLK_RC32M, /*!< root clock select RC32M */
    GLB_ROOT_CLK_XTAL,  /*!< root clock select XTAL */
    GLB_ROOT_CLK_DLL,   /*!< root clock select DLL others */
} GLB_ROOT_CLK_Type;

/**
 *  @brief GLB system clock type definition
 */
typedef enum {
    GLB_SYS_CLK_RC32M,     /*!< use RC32M as system clock frequency */
    GLB_SYS_CLK_XTAL,      /*!< use XTAL as system clock */
    GLB_SYS_CLK_DLL25P6M,  /*!< use DLL output 25.6MHz as system clock */
    GLB_SYS_CLK_DLL42P67M, /*!< use DLL output 42.67MHz as system clock */
    GLB_SYS_CLK_DLL64M,    /*!< use DLL output 64MHz as system clock */
    GLB_SYS_CLK_DLL128M,   /*!< use DLL output 128MHz as system clock */
} GLB_SYS_CLK_Type;

/**
 *  @brief GLB AUDIO clock source type definition
 */
typedef enum {
    GLB_AUDIO_CLK_SRC_2P032M, /*!< Select 2.032MHz as AUDIO clock source */
    GLB_AUDIO_CLK_SRC_6P095M, /*!< Select 6.095MHz as AUDIO clock source */
} GLB_AUDIO_CLK_SRC_Type;

/**
 *  @brief GLB KYS clock source type definition
 */
typedef enum {
    GLB_KYS_CLK_SRC_XCLK, /*!< KYS clock source select XCLK */
    GLB_KYS_CLK_SRC_F32K, /*!< KYS clock source select f32k */
} GLB_KYS_CLK_SRC_Type;

/**
 *  @brief GLB DMA clock ID type definition
 */
typedef enum {
    GLB_DMA_CLK_DMA0_CH0, /*!< DMA clock ID:channel 0 */
    GLB_DMA_CLK_DMA0_CH1, /*!< DMA clock ID:channel 1 */
    GLB_DMA_CLK_DMA0_CH2, /*!< DMA clock ID:channel 2 */
    GLB_DMA_CLK_DMA0_CH3, /*!< DMA clock ID:channel 3 */
} GLB_DMA_CLK_ID_Type;

/**
 *  @brief GLB IR clock source type definition
 */
typedef enum {
    GLB_IR_CLK_SRC_XCLK, /*!< IR clock source select XCLK */
} GLB_IR_CLK_SRC_Type;

/**
 *  @brief GLB flash clock type definition
 */
typedef enum {
    GLB_SFLASH_CLK_XCLK,   /*!< Select XCLK as flash clock */
    GLB_SFLASH_CLK_64M,    /*!< Select 64M as flash clock */
    GLB_SFLASH_CLK_BCLK,   /*!< Select BCLK as flash clock */
    GLB_SFLASH_CLK_42P67M, /*!< Select 42.67M as flash clock */
} GLB_SFLASH_CLK_Type;

/**
 *  @brief GLB chip clock out 0 type definition
 */
typedef enum {
    GLB_CHIP_CLK_OUT_0_NONE,   /*!< no chip clock out */
    GLB_CHIP_CLK_OUT_0_2P032M, /*!< DLL div63 2.032MHz clock out */
    GLB_CHIP_CLK_OUT_0_6P095M, /*!< DLL div21 6.095MHz clock out */
    GLB_CHIP_CLK_OUT_0_XCLK,   /*!< clk_xclk */
} GLB_CHIP_CLK_OUT_0_Type;

/**
 *  @brief GLB chip clock out 1 type definition
 */
typedef enum {
    GLB_CHIP_CLK_OUT_1_NONE,     /*!< no chip clock out */
    GLB_CHIP_CLK_OUT_1_F32K,     /*!< f32k clock out */
    GLB_CHIP_CLK_OUT_1_ANA_XTAL, /*!< ana xtal clock out */
    GLB_CHIP_CLK_OUT_1_DIG_XTAL, /*!< dig xtal clock out */
} GLB_CHIP_CLK_OUT_1_Type;

/**
 *  @brief GLB I2C clock source type definition
 */
typedef enum {
    GLB_I2C_CLK_SRC_BCLK, /*!< I2C clock source select BCLK */
    GLB_I2C_CLK_SRC_XCLK, /*!< I2C clock source select XCLK */
} GLB_I2C_CLK_SRC_Type;

/**
 *  @brief GLB SPI clock source type definition
 */
typedef enum {
    GLB_SPI_CLK_SRC_BCLK, /*!< SPI clock source select BCLK */
    GLB_SPI_CLK_SRC_XCLK, /*!< SPI clock source select XCLK */
} GLB_SPI_CLK_SRC_Type;

/**
 *  @brief GLB SPI pad action type definition
 */
typedef enum {
    GLB_SPI_PAD_ACT_AS_SLAVE,  /*!< SPI pad act as slave */
    GLB_SPI_PAD_ACT_AS_MASTER, /*!< SPI pad act as master */
} GLB_SPI_PAD_ACT_AS_Type;

/**
 *  @brief GLB PKA clock source type definition
 */
typedef enum {
    GLB_PKA_CLK_SRC_HCLK,      /*!< PKA clock source select HCLK */
    GLB_PKA_CLK_SRC_DLL128M,   /*!< PKA clock source select DLL128M */
    GLB_PKA_CLK_SRC_DLL64M,    /*!< PKA clock source select DLL64M */
    GLB_PKA_CLK_SRC_DLL42P67M, /*!< PKA clock source select DLL42.67M */
} GLB_PKA_CLK_SRC_Type;

/**
 *  @brief BMX arb mode type definition
 */
typedef enum {
    BMX_ARB_FIX,         /*!< 0->fix */
    BMX_ARB_ROUND_ROBIN, /*!< 2->round-robin */
    BMX_ARB_RANDOM,      /*!< 3->random */
} BMX_ARB_Type;

/**
 *  @brief GLB AHB software type definition
 */
typedef enum {
    GLB_AHB_MCU_SW_M154 = 4,
    GLB_AHB_MCU_SW_BLE = 8,
    GLB_AHB_MCU_SW_GLB = 32,
    GLB_AHB_MCU_SW_MIX = 33,
    GLB_AHB_MCU_SW_GPIP = 34,
    GLB_AHB_MCU_SW_SEC_DBG = 35,
    GLB_AHB_MCU_SW_SEC_ENG = 36,
    GLB_AHB_MCU_SW_TZ1 = 37,
    GLB_AHB_MCU_SW_TZ2 = 38,
    GLB_AHB_MCU_SW_EFUSE = 39,
    GLB_AHB_MCU_SW_CCI = 40,
    GLB_AHB_MCU_SW_L1C = 41,
    GLB_AHB_MCU_SW_SF = 43,
    GLB_AHB_MCU_SW_DMA = 44,
    GLB_AHB_MCU_SW_PDS = 46,
    GLB_AHB_MCU_SW_UART0 = 48,
    GLB_AHB_MCU_SW_SPI = 50,
    GLB_AHB_MCU_SW_I2C = 51,
    GLB_AHB_MCU_SW_PWM = 52,
    GLB_AHB_MCU_SW_TIMER = 53,
    GLB_AHB_MCU_SW_IR_REMOTE = 54,
    GLB_AHB_MCU_SW_CHECKSUM = 55,
    GLB_AHB_MCU_SW_KYS = 57,
    GLB_AHB_MCU_SW_AUSOLO = 61,
    GLB_AHB_MCU_SW_PWRON_RST = 64,
    GLB_AHB_MCU_SW_CPU_RESET = 65,
    GLB_AHB_MCU_SW_SYS_RESET = 66,
} GLB_AHB_MCU_SW_Type;

/**
 *  @brief GLB dis reset type definition
 */
typedef enum {
    GLB_DISRST_MIX = 1,
    GLB_DISRST_GPIP = 2,
    GLB_DISRST_CCI = 8,
    GLB_DISRST_SF = 11,
    GLB_DISRST_DMA = 12,
    GLB_DISRST_UART0 = 16,
    GLB_DISRST_SPI = 18,
    GLB_DISRST_I2C = 19,
    GLB_DISRST_PWM = 20,
    GLB_DISRST_TIMER = 21,
    GLB_DISRST_IR_REMOTE = 22,
    GLB_DISRST_CHECKSUM = 23,
    GLB_DISRST_KYS = 25,
    GLB_DISRST_AUSOLO = 29,
} GLB_DISRST_Type;

/**
 *  @brief BMX configuration structure type definition
 */
typedef struct {
    uint8_t timeoutEn;   /*!< Bus timeout enable: detect slave no reaponse in 1024 cycles */
    BL_Fun_Type errEn;   /*!< Bus error response enable */
    BMX_ARB_Type arbMod; /*!< 0->fix, 2->round-robin, 3->random */
} BMX_Cfg_Type;

/**
 *  @brief RTC mode type definition
 */
typedef enum {
    RTC_MODE_LOWPOWER, /*!< RTC work on low power mode */
    RTC_MODE_NORMAL,   /*!< RTC work on normal mode */
} RTC_MODE_Type;

/**
 *  @brief BMX bus err type definition
 */
typedef enum {
    BMX_BUS_ERR_TRUSTZONE_DECODE, /*!< Bus trustzone decode error */
    BMX_BUS_ERR_ADDR_DECODE,      /*!< Bus addr decode error */
} BMX_BUS_ERR_Type;

/**
 *  @brief BMX bus err interrupt type definition
 */
typedef enum {
    BMX_ERR_INT_ERR, /*!< BMX bus err interrupt */
    BMX_ERR_INT_ALL, /*!< BMX bus err interrupt max num */
} BMX_ERR_INT_Type;

/**
 *  @brief BMX time out interrupt type definition
 */
typedef enum {
    BMX_TO_INT_TIMEOUT, /*!< BMX timeout interrupt */
    BMX_TO_INT_ALL,     /*!< BMX timeout interrupt max num */
} BMX_TO_INT_Type;

/**
 *  @brief GLB EM type definition
 */
typedef enum {
    GLB_EM_0KB = 0x0,  /*!< 0x0 --> 0KB */
    GLB_EM_8KB = 0x3,  /*!< 0x3 --> 8KB */
    GLB_EM_16KB = 0xF, /*!< 0xF --> 16KB */
} GLB_EM_Type;

/**
 *  @brief GLB MTIMER clock type definition
 */
typedef enum {
    GLB_MTIMER_CLK_XCLK, /*!< BUS clock */
    GLB_MTIMER_CLK_F32K, /*!< 32KHz */
} GLB_MTIMER_CLK_Type;

/**
 *  @brief GLB ADC clock source type definition
 */
typedef enum {
    GLB_ADC_CLK_SRC_F32K, /*!< ADC clock source select F32K */
    GLB_ADC_CLK_SRC_XCLK, /*!< ADC clock source select XCLK */
} GLB_ADC_CLK_SRC_Type;

/**
 *  @brief GLB DIG clock source select type definition
 */
typedef enum {
    GLB_DIG_CLK_SRC_XTAL32M, /*!< select XTAL32M as DIG clock source */
    GLB_DIG_CLK_SRC_RC32M,   /*!< select RC32M as DIG clock source */
} GLB_DIG_CLK_SRC_Type;

/**
 *  @brief GLB BT bandwidth type definition
 */
typedef enum {
    GLB_BT_BANDWIDTH_1M, /*!< BT bandwidth 1MHz */
    GLB_BT_BANDWIDTH_2M, /*!< BT bandwidth 2MHz */
} GLB_BT_BANDWIDTH_Type;

/**
 *  @brief GLB UART signal type definition
 */
typedef enum {
    GLB_UART_SIG_0, /*!< UART signal 0 */
    GLB_UART_SIG_1, /*!< UART signal 1 */
    GLB_UART_SIG_2, /*!< UART signal 2 */
    GLB_UART_SIG_3, /*!< UART signal 3 */
} GLB_UART_SIG_Type;

/**
 *  @brief GLB UART signal  function type definition
 */
typedef enum {
    GLB_UART_SIG_FUN_UART0_RTS, /*!< UART funtion: UART 0 RTS */
    GLB_UART_SIG_FUN_UART0_CTS, /*!< UART funtion: UART 0 CTS */
    GLB_UART_SIG_FUN_UART0_TXD, /*!< UART funtion: UART 0 TXD */
    GLB_UART_SIG_FUN_UART0_RXD, /*!< UART funtion: UART 0 RXD */
} GLB_UART_SIG_FUN_Type;

/**
 *  @brief GLB DLL output clock type definition
 */
typedef enum {
    GLB_DLL_CLK_RF,     /*!< DLL output div1 RF clock */
    GLB_DLL_CLK_2P032M, /*!< DLL output 2.032MHz clock */
    GLB_DLL_CLK_6P095M, /*!< DLL output 6.095MHz clock */
    GLB_DLL_CLK_25P6M,  /*!< DLL output 25.6MHz clock */
    GLB_DLL_CLK_42P67M, /*!< DLL output 42.67MHz clock */
    GLB_DLL_CLK_64M,    /*!< DLL output 64MHz clock */
    GLB_DLL_CLK_128M,   /*!< DLL output 128MHz clock */
} GLB_DLL_CLK_Type;

/**
 *  @brief PLL XTAL type definition
 */
typedef enum {
    GLB_DLL_XTAL_NONE,  /*!< XTAL is none */
    GLB_DLL_XTAL_32M,   /*!< XTAL is 32M */
    GLB_DLL_XTAL_RC32M, /*!< XTAL is RC32M */
} GLB_DLL_XTAL_Type;

/**
 *  @brief GLB AHB clock IP type definition
 */
typedef enum {
    GLB_AHB_CLOCK_IP_CPU,
    GLB_AHB_CLOCK_IP_SDU,
    GLB_AHB_CLOCK_IP_SEC,
    GLB_AHB_CLOCK_IP_DMA_0,
    GLB_AHB_CLOCK_IP_DMA_1,
    GLB_AHB_CLOCK_IP_DMA_2,
    GLB_AHB_CLOCK_IP_CCI,
    GLB_AHB_CLOCK_IP_RF_TOP,
    GLB_AHB_CLOCK_IP_GPIP,
    GLB_AHB_CLOCK_IP_TZC,
    GLB_AHB_CLOCK_IP_EF_CTRL,
    GLB_AHB_CLOCK_IP_SF_CTRL,
    GLB_AHB_CLOCK_IP_EMAC,
    GLB_AHB_CLOCK_IP_UART0,
    GLB_AHB_CLOCK_IP_UART1,
    GLB_AHB_CLOCK_IP_UART2,
    GLB_AHB_CLOCK_IP_UART3,
    GLB_AHB_CLOCK_IP_UART4,
    GLB_AHB_CLOCK_IP_SPI,
    GLB_AHB_CLOCK_IP_I2C,
    GLB_AHB_CLOCK_IP_PWM,
    GLB_AHB_CLOCK_IP_TIMER,
    GLB_AHB_CLOCK_IP_IR,
    GLB_AHB_CLOCK_IP_CHECKSUM,
    GLB_AHB_CLOCK_IP_QDEC,
    GLB_AHB_CLOCK_IP_KYS,
    GLB_AHB_CLOCK_IP_I2S,
    GLB_AHB_CLOCK_IP_USB11,
    GLB_AHB_CLOCK_IP_CAM,
    GLB_AHB_CLOCK_IP_MJPEG,
    GLB_AHB_CLOCK_IP_BT_BLE_NORMAL,
    GLB_AHB_CLOCK_IP_BT_BLE_LP,
    GLB_AHB_CLOCK_IP_ZB_NORMAL,
    GLB_AHB_CLOCK_IP_ZB_LP,
    GLB_AHB_CLOCK_IP_WIFI_NORMAL,
    GLB_AHB_CLOCK_IP_WIFI_LP,
    GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL,
    GLB_AHB_CLOCK_IP_BT_BLE_2_LP,
    GLB_AHB_CLOCK_IP_EMI_MISC,
    GLB_AHB_CLOCK_IP_PSRAM0_CTRL,
    GLB_AHB_CLOCK_IP_PSRAM1_CTRL,
    GLB_AHB_CLOCK_IP_USB20_SDU,
    GLB_AHB_CLOCK_IP_MIX2,
    GLB_AHB_CLOCK_IP_AUDIO,
    GLB_AHB_CLOCK_IP_SDH,
    GLB_AHB_CLOCK_IP_ZB2_NORMAL,
    GLB_AHB_CLOCK_IP_ZB2_LP,
    GLB_AHB_CLOCK_IP_I2C1,
    GLB_AHB_CLOCK_IP_WIFI_PHY,
    GLB_AHB_CLOCK_IP_WIFI_MAC_PHY,
    GLB_AHB_CLOCK_IP_WIFI_PLATFORM,
    GLB_AHB_CLOCK_IP_LZ4,
    GLB_AHB_CLOCK_IP_AUPDM,
    GLB_AHB_CLOCK_IP_GAUGE,
    GLB_AHB_CLOCK_IP_DBI,
    GLB_AHB_CLOCK_IP_PEC,
    GLB_AHB_CLOCK_IP_ISO11898,
    GLB_AHB_CLOCK_IP_AUSOLO_TOP,
    GLB_AHB_CLOCK_IP_DMA_GPIO,
    GLB_AHB_CLOCK_IP_MM_MISC,
} GLB_AHB_CLOCK_IP_Type;

/*@} end of group GLB_Public_Types */

/** @defgroup  GLB_Public_Constants
 *  @{
 */

/** @defgroup  GLB_ROOT_CLK_TYPE
 *  @{
 */
#define IS_GLB_IR_LED_TYPE(type) (((type) == GLB_IR_LED0) || \
                                  ((type) == GLB_IR_LED1))

/** @defgroup  GLB_ROOT_CLK_TYPE
 *  @{
 */
#define IS_GLB_ROOT_CLK_TYPE(type) (((type) == GLB_ROOT_CLK_RC32M) || \
                                    ((type) == GLB_ROOT_CLK_XTAL) ||  \
                                    ((type) == GLB_ROOT_CLK_DLL))

/** @defgroup  GLB_SYS_CLK_TYPE
 *  @{
 */
#define IS_GLB_SYS_CLK_TYPE(type) (((type) == GLB_SYS_CLK_RC32M) ||     \
                                   ((type) == GLB_SYS_CLK_XTAL) ||      \
                                   ((type) == GLB_SYS_CLK_DLL25P6M) ||  \
                                   ((type) == GLB_SYS_CLK_DLL42P67M) || \
                                   ((type) == GLB_SYS_CLK_DLL64M) ||    \
                                   ((type) == GLB_SYS_CLK_DLL128M))

/** @defgroup  GLB_AUDIO_CLK_SRC_TYPE
 *  @{
 */
#define IS_GLB_AUDIO_CLK_SRC_TYPE(type) (((type) == GLB_AUDIO_CLK_SRC_2P032M) || \
                                         ((type) == GLB_AUDIO_CLK_SRC_6P095M))

/** @defgroup  GLB_KYS_CLK_SRC_TYPE
 *  @{
 */
#define IS_GLB_KYS_CLK_SRC_TYPE(type) (((type) == GLB_KYS_CLK_SRC_XCLK) || \
                                       ((type) == GLB_KYS_CLK_SRC_F32K))

/** @defgroup  GLB_DMA_CLK_ID_TYPE
 *  @{
 */
#define IS_GLB_DMA_CLK_ID_TYPE(type) (((type) == GLB_DMA_CLK_DMA0_CH0) || \
                                      ((type) == GLB_DMA_CLK_DMA0_CH1) || \
                                      ((type) == GLB_DMA_CLK_DMA0_CH2) || \
                                      ((type) == GLB_DMA_CLK_DMA0_CH3))

/** @defgroup  GLB_IR_CLK_SRC_TYPE
 *  @{
 */
#define IS_GLB_IR_CLK_SRC_TYPE(type) (((type) == GLB_IR_CLK_SRC_XCLK))

/** @defgroup  GLB_I2C_CLK_SRC_TYPE
 *  @{
 */
#define IS_GLB_I2C_CLK_SRC_TYPE(type) (((type) == GLB_I2C_CLK_SRC_BCLK) || \
                                       ((type) == GLB_I2C_CLK_SRC_XCLK))

/** @defgroup  GLB_SPI_CLK_SRC_TYPE
 *  @{
 */
#define IS_GLB_SPI_CLK_SRC_TYPE(type) (((type) == GLB_SPI_CLK_SRC_BCLK) || \
                                       ((type) == GLB_SPI_CLK_SRC_XCLK))

/** @defgroup  GLB_SFLASH_CLK_TYPE
 *  @{
 */
#define IS_GLB_SFLASH_CLK_TYPE(type) (((type) == GLB_SFLASH_CLK_XCLK) || \
                                      ((type) == GLB_SFLASH_CLK_64M) ||  \
                                      ((type) == GLB_SFLASH_CLK_BCLK) || \
                                      ((type) == GLB_SFLASH_CLK_42P67M))

/** @defgroup  GLB_CHIP_CLK_OUT_1_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_OUT_1_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_1_NONE) ||     \
                                          ((type) == GLB_CHIP_CLK_OUT_1_F32K) ||     \
                                          ((type) == GLB_CHIP_CLK_OUT_1_ANA_XTAL) || \
                                          ((type) == GLB_CHIP_CLK_OUT_1_DIG_XTAL))

/** @defgroup  GLB_CHIP_CLK_OUT_0_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_OUT_0_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_0_NONE) ||   \
                                          ((type) == GLB_CHIP_CLK_OUT_0_2P032M) || \
                                          ((type) == GLB_CHIP_CLK_OUT_0_6P095M) || \
                                          ((type) == GLB_CHIP_CLK_OUT_0_XCLK))

/** @defgroup  GLB_SPI_PAD_ACT_AS_TYPE
 *  @{
 */
#define IS_GLB_SPI_PAD_ACT_AS_TYPE(type) (((type) == GLB_SPI_PAD_ACT_AS_SLAVE) || \
                                          ((type) == GLB_SPI_PAD_ACT_AS_MASTER))

/** @defgroup  GLB_PKA_CLK_TYPE
 *  @{
 */
#define IS_GLB_PKA_CLK_SRC_TYPE(type) (((type) == GLB_PKA_CLK_SRC_HCLK) ||    \
                                       ((type) == GLB_PKA_CLK_SRC_DLL128M) || \
                                       ((type) == GLB_PKA_CLK_SRC_DLL64M) ||  \
                                       ((type) == GLB_PKA_CLK_SRC_DLL42P67M))

/** @defgroup  GLB_AHB_SW_TYPE
 *  @{
 */
#define IS_GLB_AHB_MCU_SW_TYPE(type) (((type) == GLB_AHB_MCU_SW_M154) ||      \
                                      ((type) == GLB_AHB_MCU_SW_BLE) ||       \
                                      ((type) == GLB_AHB_MCU_SW_GLB) ||       \
                                      ((type) == GLB_AHB_MCU_SW_MIX) ||       \
                                      ((type) == GLB_AHB_MCU_SW_GPIP) ||      \
                                      ((type) == GLB_AHB_MCU_SW_SEC_DBG) ||   \
                                      ((type) == GLB_AHB_MCU_SW_SEC_ENG) ||   \
                                      ((type) == GLB_AHB_MCU_SW_TZ1) ||       \
                                      ((type) == GLB_AHB_MCU_SW_TZ2) ||       \
                                      ((type) == GLB_AHB_MCU_SW_EFUSE) ||     \
                                      ((type) == GLB_AHB_MCU_SW_CCI) ||       \
                                      ((type) == GLB_AHB_MCU_SW_L1C) ||       \
                                      ((type) == GLB_AHB_MCU_SW_SF) ||        \
                                      ((type) == GLB_AHB_MCU_SW_DMA) ||       \
                                      ((type) == GLB_AHB_MCU_SW_PDS) ||       \
                                      ((type) == GLB_AHB_MCU_SW_UART0) ||     \
                                      ((type) == GLB_AHB_MCU_SW_SPI) ||       \
                                      ((type) == GLB_AHB_MCU_SW_I2C) ||       \
                                      ((type) == GLB_AHB_MCU_SW_PWM) ||       \
                                      ((type) == GLB_AHB_MCU_SW_TIMER) ||     \
                                      ((type) == GLB_AHB_MCU_SW_IR_REMOTE) || \
                                      ((type) == GLB_AHB_MCU_SW_CHECKSUM) ||  \
                                      ((type) == GLB_AHB_MCU_SW_KYS) ||       \
                                      ((type) == GLB_AHB_MCU_SW_AUSOLO) ||    \
                                      ((type) == GLB_AHB_MCU_SW_PWRON_RST) || \
                                      ((type) == GLB_AHB_MCU_SW_CPU_RESET) || \
                                      ((type) == GLB_AHB_MCU_SW_SYS_RESET))

/** @defgroup  GLB DISRST_TYPE
 *  @{
 */
#define IS_GLB_DISRST_TYPE(type) (((type) == GLB_DISRST_MIX) ||       \
                                  ((type) == GLB_DISRST_GPIP) ||      \
                                  ((type) == GLB_DISRST_CCI) ||       \
                                  ((type) == GLB_DISRST_SF) ||        \
                                  ((type) == GLB_DISRST_DMA) ||       \
                                  ((type) == GLB_DISRST_UART0) ||     \
                                  ((type) == GLB_DISRST_SPI) ||       \
                                  ((type) == GLB_DISRST_I2C) ||       \
                                  ((type) == GLB_DISRST_PWM) ||       \
                                  ((type) == GLB_DISRST_TIMER) ||     \
                                  ((type) == GLB_DISRST_IR_REMOTE) || \
                                  ((type) == GLB_DISRST_CHECKSUM) ||  \
                                  ((type) == GLB_DISRST_KYS) ||       \
                                  ((type) == GLB_DISRST_AUSOLO))

/** @defgroup  RTC_MODE_TYPE
 *  @{
 */
#define IS_RTC_MODE_TYPE(type) (((type) == RTC_MODE_LOWPOWER) || \
                                ((type) == RTC_MODE_NORMAL))

/** @defgroup  BMX_ARB_TYPE
 *  @{
 */
#define IS_BMX_ARB_TYPE(type) (((type) == BMX_ARB_FIX) ||         \
                               ((type) == BMX_ARB_ROUND_ROBIN) || \
                               ((type) == BMX_ARB_RANDOM))

/** @defgroup  BMX_BUS_ERR_TYPE
 *  @{
 */
#define IS_BMX_BUS_ERR_TYPE(type) (((type) == BMX_BUS_ERR_TRUSTZONE_DECODE) || \
                                   ((type) == BMX_BUS_ERR_ADDR_DECODE))

/** @defgroup  BMX_ERR_INT_TYPE
 *  @{
 */
#define IS_BMX_ERR_INT_TYPE(type) (((type) == BMX_ERR_INT_ERR) || \
                                   ((type) == BMX_ERR_INT_ALL))

/** @defgroup  BMX_TO_INT_TYPE
 *  @{
 */
#define IS_BMX_TO_INT_TYPE(type) (((type) == BMX_TO_INT_TIMEOUT) || \
                                  ((type) == BMX_TO_INT_ALL))

/** @defgroup  GLB_EM_TYPE
 *  @{
 */
#define IS_GLB_EM_TYPE(type) (((type) == GLB_EM_0KB) || \
                              ((type) == GLB_EM_8KB) || \
                              ((type) == GLB_EM_16KB))

/** @defgroup  GLB_MTIMER_CLK_TYPE
 *  @{
 */
#define IS_GLB_MTIMER_CLK_TYPE(type) (((type) == GLB_MTIMER_CLK_XCLK) || \
                                      ((type) == GLB_MTIMER_CLK_F32K))

/** @defgroup  GLB_ADC_CLK_TYPE
 *  @{
 */
#define IS_GLB_ADC_CLK_SRC_TYPE(type) (((type) == GLB_ADC_CLK_SRC_F32K) || \
                                       ((type) == GLB_ADC_CLK_SRC_XCLK))

/** @defgroup  GLB_DIG_CLK_TYPE
 *  @{
 */
#define IS_GLB_DIG_CLK_SRC_TYPE(type) (((type) == GLB_DIG_CLK_SRC_XTAL32M) || \
                                       ((type) == GLB_DIG_CLK_SRC_RC32M))

/** @defgroup  GLB_BT_BANDWIDTH_TYPE
 *  @{
 */
#define IS_GLB_BT_BANDWIDTH_TYPE(type) (((type) == GLB_BT_BANDWIDTH_1M) || \
                                        ((type) == GLB_BT_BANDWIDTH_2M))

/** @defgroup  GLB_UART_SIG_TYPE
 *  @{
 */
#define IS_GLB_UART_SIG_TYPE(type) (((type) == GLB_UART_SIG_0) || \
                                    ((type) == GLB_UART_SIG_1) || \
                                    ((type) == GLB_UART_SIG_2) || \
                                    ((type) == GLB_UART_SIG_3))

/** @defgroup  GLB_UART_SIG_FUN_TYPE
 *  @{
 */
#define IS_GLB_UART_SIG_FUN_TYPE(type) (((type) == GLB_UART_SIG_FUN_UART0_RTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_CTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_TXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_RXD))

/** @defgroup  GLB_DLL_CLK_TYPE
 *  @{
 */
#define IS_GLB_DLL_CLK_TYPE(type) (((type) == GLB_DLL_CLK_RF) ||     \
                                   ((type) == GLB_DLL_CLK_2P032M) || \
                                   ((type) == GLB_DLL_CLK_6P095M) || \
                                   ((type) == GLB_DLL_CLK_25P6M) ||  \
                                   ((type) == GLB_DLL_CLK_42P67M) || \
                                   ((type) == GLB_DLL_CLK_64M) ||    \
                                   ((type) == GLB_DLL_CLK_128M))

/** @defgroup  GLB_DLL_XTAL_TYPE
 *  @{
 */
#define IS_GLB_DLL_XTAL_TYPE(type) (((type) == GLB_DLL_XTAL_NONE) || \
                                    ((type) == GLB_DLL_XTAL_32M) ||  \
                                    ((type) == GLB_DLL_XTAL_RC32M))

/*@} end of group GLB_Public_Constants */

/** @defgroup  GLB_Public_Macros
 *  @{
 */
#define JTAG_SIG_SWAP_GPIO0_GPIO3   0x01 /* GPIO0-3   E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO4_GPIO7   0x02 /* GPIO4-7   E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO8_GPIO11  0x04 /* GPIO8-11  E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO12_GPIO15 0x08 /* GPIO12-15 E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO16_GPIO19 0x10 /* GPIO16-19 E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_GPIO20_GPIO22 0x20 /* GPIO20-22 E21_TMS/E21_TDI/E21_TCK/E21_TDO -> E21_TCK/E21_TDO/E21_TMS/E21_TDI */
#define JTAG_SIG_SWAP_NONE          0x00 /* GPIO0-22  E21_TMS/E21_TDI/E21_TCK/E21_TDO <- E21_TCK/E21_TDO/E21_TMS/E21_TDI */

#define GLB_AHB_CLOCK_CPU             (1ULL << GLB_AHB_CLOCK_IP_CPU)
#define GLB_AHB_CLOCK_SDU             (1ULL << GLB_AHB_CLOCK_IP_SDU)
#define GLB_AHB_CLOCK_SEC             (1ULL << GLB_AHB_CLOCK_IP_SEC)
#define GLB_AHB_CLOCK_DMA_0           (1ULL << GLB_AHB_CLOCK_IP_DMA_0)
#define GLB_AHB_CLOCK_DMA_1           (1ULL << GLB_AHB_CLOCK_IP_DMA_1)
#define GLB_AHB_CLOCK_DMA_2           (1ULL << GLB_AHB_CLOCK_IP_DMA_2)
#define GLB_AHB_CLOCK_CCI             (1ULL << GLB_AHB_CLOCK_IP_CCI)
#define GLB_AHB_CLOCK_RF_TOP          (1ULL << GLB_AHB_CLOCK_IP_RF_TOP)
#define GLB_AHB_CLOCK_GPIP            (1ULL << GLB_AHB_CLOCK_IP_GPIP)
#define GLB_AHB_CLOCK_TZC             (1ULL << GLB_AHB_CLOCK_IP_TZC)
#define GLB_AHB_CLOCK_EF_CTRL         (1ULL << GLB_AHB_CLOCK_IP_EF_CTRL)
#define GLB_AHB_CLOCK_SF_CTRL         (1ULL << GLB_AHB_CLOCK_IP_SF_CTRL)
#define GLB_AHB_CLOCK_EMAC            (1ULL << GLB_AHB_CLOCK_IP_EMAC)
#define GLB_AHB_CLOCK_UART0           (1ULL << GLB_AHB_CLOCK_IP_UART0)
#define GLB_AHB_CLOCK_UART1           (1ULL << GLB_AHB_CLOCK_IP_UART1)
#define GLB_AHB_CLOCK_UART2           (1ULL << GLB_AHB_CLOCK_IP_UART2)
#define GLB_AHB_CLOCK_UART3           (1ULL << GLB_AHB_CLOCK_IP_UART3)
#define GLB_AHB_CLOCK_UART4           (1ULL << GLB_AHB_CLOCK_IP_UART4)
#define GLB_AHB_CLOCK_SPI             (1ULL << GLB_AHB_CLOCK_IP_SPI)
#define GLB_AHB_CLOCK_I2C             (1ULL << GLB_AHB_CLOCK_IP_I2C)
#define GLB_AHB_CLOCK_PWM             (1ULL << GLB_AHB_CLOCK_IP_PWM)
#define GLB_AHB_CLOCK_TIMER           (1ULL << GLB_AHB_CLOCK_IP_TIMER)
#define GLB_AHB_CLOCK_IR              (1ULL << GLB_AHB_CLOCK_IP_IR)
#define GLB_AHB_CLOCK_CHECKSUM        (1ULL << GLB_AHB_CLOCK_IP_CHECKSUM)
#define GLB_AHB_CLOCK_QDEC            (1ULL << GLB_AHB_CLOCK_IP_QDEC)
#define GLB_AHB_CLOCK_KYS             (1ULL << GLB_AHB_CLOCK_IP_KYS)
#define GLB_AHB_CLOCK_I2S             (1ULL << GLB_AHB_CLOCK_IP_I2S)
#define GLB_AHB_CLOCK_USB11           (1ULL << GLB_AHB_CLOCK_IP_USB11)
#define GLB_AHB_CLOCK_CAM             (1ULL << GLB_AHB_CLOCK_IP_CAM)
#define GLB_AHB_CLOCK_MJPEG           (1ULL << GLB_AHB_CLOCK_IP_MJPEG)
#define GLB_AHB_CLOCK_BT_BLE_NORMAL   (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_NORMAL)
#define GLB_AHB_CLOCK_BT_BLE_LP       (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_LP)
#define GLB_AHB_CLOCK_ZB_NORMAL       (1ULL << GLB_AHB_CLOCK_IP_ZB_NORMAL)
#define GLB_AHB_CLOCK_ZB_LP           (1ULL << GLB_AHB_CLOCK_IP_ZB_LP)
#define GLB_AHB_CLOCK_WIFI_NORMAL     (1ULL << GLB_AHB_CLOCK_IP_WIFI_NORMAL)
#define GLB_AHB_CLOCK_WIFI_LP         (1ULL << GLB_AHB_CLOCK_IP_WIFI_LP)
#define GLB_AHB_CLOCK_BT_BLE_2_NORMAL (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL)
#define GLB_AHB_CLOCK_BT_BLE_2_LP     (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_2_LP)
#define GLB_AHB_CLOCK_EMI_MISC        (1ULL << GLB_AHB_CLOCK_IP_EMI_MISC)
#define GLB_AHB_CLOCK_PSRAM0_CTRL     (1ULL << GLB_AHB_CLOCK_IP_PSRAM0_CTRL)
#define GLB_AHB_CLOCK_PSRAM1_CTRL     (1ULL << GLB_AHB_CLOCK_IP_PSRAM1_CTRL)
#define GLB_AHB_CLOCK_USB20_SDU       (1ULL << GLB_AHB_CLOCK_IP_USB20_SDU)
#define GLB_AHB_CLOCK_MIX2            (1ULL << GLB_AHB_CLOCK_IP_MIX2)
#define GLB_AHB_CLOCK_AUDIO           (1ULL << GLB_AHB_CLOCK_IP_AUDIO)
#define GLB_AHB_CLOCK_SDH             (1ULL << GLB_AHB_CLOCK_IP_SDH)
#define GLB_AHB_CLOCK_ZB_2_NORMAL     (1ULL << GLB_AHB_CLOCK_IP_ZB2_NORMAL)
#define GLB_AHB_CLOCK_ZB_2_LP         (1ULL << GLB_AHB_CLOCK_IP_ZB2_LP)
#define GLB_AHB_CLOCK_I2C1            (1ULL << GLB_AHB_CLOCK_IP_I2C1)
#define GLB_AHB_CLOCK_WIFI_PHY        (1ULL << GLB_AHB_CLOCK_IP_WIFI_PHY)
#define GLB_AHB_CLOCK_WIFI_MAC_PHY    (1ULL << GLB_AHB_CLOCK_IP_WIFI_MAC_PHY)
#define GLB_AHB_CLOCK_WIFI_PLATFORM   (1ULL << GLB_AHB_CLOCK_IP_WIFI_PLATFORM)
#define GLB_AHB_CLOCK_LZ4             (1ULL << GLB_AHB_CLOCK_IP_LZ4)
#define GLB_AHB_CLOCK_AUPDM           (1ULL << GLB_AHB_CLOCK_IP_AUPDM)
#define GLB_AHB_CLOCK_GAUGE           (1ULL << GLB_AHB_CLOCK_IP_GAUGE)
#define GLB_AHB_CLOCK_DBI             (1ULL << GLB_AHB_CLOCK_IP_DBI)
#define GLB_AHB_CLOCK_PEC             (1ULL << GLB_AHB_CLOCK_IP_PEC)
#define GLB_AHB_CLOCK_ISO11898        (1ULL << GLB_AHB_CLOCK_IP_ISO11898)
#define GLB_AHB_CLOCK_AUSOLO_TOP      (1ULL << GLB_AHB_CLOCK_IP_AUSOLO_TOP)
#define GLB_AHB_CLOCK_DMA_GPIO        (1ULL << GLB_AHB_CLOCK_IP_DMA_GPIO)
#define GLB_AHB_CLOCK_MM_MISC         (1ULL << GLB_AHB_CLOCK_IP_MM_MISC)

/*@} end of group GLB_Public_Macros */

/** @defgroup  GLB_Public_Functions
 *  @{
 */
/*----------*/
#ifndef BFLB_USE_HAL_DRIVER
void BMX_ERR_IRQHandler(void);
void BMX_TO_IRQHandler(void);
#endif
/*----------*/
GLB_ROOT_CLK_Type GLB_Get_Root_CLK_Sel(void);
BL_Err_Type GLB_Set_System_CLK_Div(uint8_t hclkDiv, uint8_t bclkDiv);
uint8_t GLB_Get_BCLK_Div(void);
uint8_t GLB_Get_HCLK_Div(void);
BL_Err_Type GLB_Set_System_CLK(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq);
BL_Err_Type System_Core_Clock_Update_From_RC32M(void);
/*----------*/
BL_Err_Type GLB_Set_MAC154_ZIGBEE_CLK(uint8_t enable);
BL_Err_Type GLB_Set_BLE_CLK(uint8_t enable);
BL_Err_Type GLB_Set_AUDIO_CLK(uint8_t clkDivEn, uint8_t autoDivEn, GLB_AUDIO_CLK_SRC_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_KYS_CLK(GLB_KYS_CLK_SRC_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk);
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_SF_CLK(uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div);
BL_Err_Type GLB_Sel_TMR_GPIO_Clock(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_0_Type clkSel);
BL_Err_Type GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_1_Type clkSel);
BL_Err_Type GLB_Set_Chip_Out_CLK_Enable(uint8_t enable, uint8_t pin);
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_SRC_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_SRC_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_SRC_Type clkSel);
/*----------*/
BL_Err_Type GLB_SW_System_Reset(void);
BL_Err_Type GLB_SW_CPU_Reset(void);
BL_Err_Type GLB_SW_POR_Reset(void);
BL_Err_Type GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_Type swrst);
BL_Err_Type GLB_MAC154_ZIGBEE_Reset(void);
BL_Err_Type GLB_BLE_Reset(void);
BL_Err_Type GLB_Disrst_Set(uint8_t enable, GLB_DISRST_Type disrst);
BL_Err_Type GLB_PER_Clock_Gate(uint64_t ips);
BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips);
/*----------*/
BL_Err_Type GLB_Set_RTC_Mode(RTC_MODE_Type rtcMode);
/*----------*/
BL_Err_Type GLB_BMX_Init(BMX_Cfg_Type *BmxCfg);
BL_Err_Type GLB_BMX_Addr_Monitor_Enable(void);
BL_Err_Type GLB_BMX_Addr_Monitor_Disable(void);
BL_Err_Type GLB_BMX_BusErrResponse_Enable(void);
BL_Err_Type GLB_BMX_BusErrResponse_Disable(void);
BL_Sts_Type GLB_BMX_Get_Status(BMX_BUS_ERR_Type errType);
uint32_t GLB_BMX_Get_Err_Addr(void);
BL_Err_Type GLB_BMX_BusErrClr_Set(uint8_t enable);
BL_Err_Type BMX_ERR_INT_Callback_Install(BMX_ERR_INT_Type intType, intCallback_Type *cbFun);
BL_Err_Type BMX_TIMEOUT_INT_Callback_Install(BMX_TO_INT_Type intType, intCallback_Type *cbFun);
/*----------*/
BL_Err_Type GLB_Set_SRAM_PARM(uint32_t value);
uint32_t GLB_Get_SRAM_PARM(void);
BL_Err_Type GLB_Set_OCRAM_PARM(uint32_t value);
uint32_t GLB_Get_OCRAM_PARM(void);
/*----------*/
BL_Err_Type GLB_Set_EM_Sel(GLB_EM_Type emType);
/*----------*/
BL_Err_Type GLB_Set_Kys_Drv_Col(uint8_t enable);
BL_Err_Type GLB_GPIO_O_Latch_Mode_Set(uint8_t enable);
BL_Err_Type GLB_JTAG_Sig_Swap_Set(uint8_t swapSel);
BL_Err_Type GLB_CCI_Use_IO_0_1_2_7(uint8_t enable);
BL_Err_Type GLB_CCI_Use_Jtag_Pin(uint8_t enable);
BL_Err_Type GLB_Swap_SPI_0_MOSI_With_MISO(BL_Fun_Type newState);
BL_Err_Type GLB_Set_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_Type mod);
BL_Err_Type GLB_Set_Flash_Scenario(uint8_t enable);
BL_Err_Type GLB_Set_Embedded_FLash_IO_PARM(uint8_t reverse, uint8_t swapIo3Io0, uint8_t swapIo2Cs);
/*----------*/
BL_Err_Type GLB_Set_MTimer_CLK(uint8_t enable, GLB_MTIMER_CLK_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_SRC_Type clkSel, uint8_t div);
BL_Err_Type GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensation, GLB_DIG_CLK_SRC_Type clkSel, uint16_t div);
BL_Err_Type GLB_SW_BLE_WAKEUP_REQ_Set(uint8_t enable);
/*----------*/
BL_Err_Type GLB_UART_Fun_Sel(GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun);
/*----------*/
BL_Err_Type GLB_Power_Off_DLL(void);
BL_Err_Type GLB_Power_On_DLL(GLB_DLL_XTAL_Type xtalType);
BL_Err_Type GLB_Enable_DLL_All_Clks(void);
BL_Err_Type GLB_Enable_DLL_Clk(GLB_DLL_CLK_Type dllClk);
BL_Err_Type GLB_Disable_DLL_All_Clks(void);
BL_Err_Type GLB_Disable_DLL_Clk(GLB_DLL_CLK_Type dllClk);
/*----------*/
BL_Err_Type GLB_Set_Flash_Id_Value(uint32_t idValue);
uint32_t GLB_Get_Flash_Id_Value(void);
/*----------*/
BL_Err_Type GLB_Trim_RC32M(void);
BL_Err_Type GLB_Set_Xtal_Cnt32k_Process(void);
BL_Err_Type GLB_Clear_Xtal_Cnt32k_Done(void);
BL_Err_Type GLB_RC32K_Deg_Start(void);
BL_Err_Type GLB_RC32K_Deg_End(void);
BL_Err_Type GLB_RC32K_Deg_Enable(uint8_t enable);
BL_Err_Type GLB_Xtal_Deg_Cnt_Limit_Set(uint8_t cnt);
/*----------*/
BL_Err_Type GLB_IR_LED_Driver_Enable(void);
BL_Err_Type GLB_IR_LED_Driver_Disable(void);
BL_Err_Type GLB_IR_LED_Driver_Output_Enable(GLB_IR_LED_Type led);
BL_Err_Type GLB_IR_LED_Driver_Output_Disable(GLB_IR_LED_Type led);
BL_Err_Type GLB_IR_LED_Driver_Ibias(uint8_t ibias);
/*----------*/;

/*@} end of group GLB_Public_Functions */

/*@} end of group GLB */

/*@} end of group BL702L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_GLB_H__ */
