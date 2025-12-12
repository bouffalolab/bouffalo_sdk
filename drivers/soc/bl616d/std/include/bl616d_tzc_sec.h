/**
  ******************************************************************************
  * @file    bl616d_tzc_sec.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2021 Bouffalo Lab</center></h2>
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
#ifndef __BL616D_TZC_SEC_H__
#define __BL616D_TZC_SEC_H__

#include "tzc_sec_reg.h"
#include "bl616d_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616D_Peripheral_Driver
 *  @{
 */

/** @addtogroup  TZC_SEC
 *  @{
 */

/** @defgroup  TZC_SEC_Public_Types
 *  @{
 */

/**
 *  @brief TZC_SEC master type definition
 */
typedef enum {
    TZC_SEC_MASTER_LP,     /*!< TZC Master:PICO */
    TZC_SEC_MASTER_NP,     /*!< TZC Master:WMCU */
    TZC_SEC_MASTER_USB,    /*!< TZC Master:USB */
    TZC_SEC_MASTER_WIFI1,  /*!< TZC Master:WIFI1 */
    TZC_SEC_MASTER_CCI,    /*!< TZC Master:CCI */
    TZC_SEC_MASTER_SDH,    /*!< TZC Master:SDH */
    TZC_SEC_MASTER_EMAC_A, /*!< TZC Master:EMAC_A */
    TZC_SEC_MASTER_AP,     /*!< TZC Master:AP CPU */
    TZC_SEC_MASTER_RSVD0,  /*!< TZC Master:Reserved */
    TZC_SEC_MASTER_WIFI2,  /*!< TZC Master:WIFI2 */
    TZC_SEC_MASTER_RSVD3,  /*!< TZC Master:Reserved */
    TZC_SEC_MASTER_SDU,    /*!< TZC Master:SDU */
    TZC_SEC_MASTER_MM_GRP1, /*!< TZC Master:MM GRP1 */
    TZC_SEC_MASTER_MM_GRP2, /*!< TZC Master:MM GRP2 */
    TZC_SEC_MASTER_MAX,   /*!< TZC Master max */
} TZC_SEC_Master_Type;


#define TZC_SEC_MASTER_TYPE_DMA0  0
#define TZC_SEC_MASTER_TYPE_DMA1  1
#define TZC_SEC_MASTER_TYPE_DMA2  2
#define TZC_SEC_MASTER_TYPE_2DDMA 3


#define TZC_SEC_DMA_BIT_CH0  (1 << 0)
#define TZC_SEC_DMA_BIT_CH1  (1 << 1)
#define TZC_SEC_DMA_BIT_CH2  (1 << 2)
#define TZC_SEC_DMA_BIT_CH3  (1 << 3)
#define TZC_SEC_DMA_BIT_CH4  (1 << 4)
#define TZC_SEC_DMA_BIT_CH5  (1 << 5)
#define TZC_SEC_DMA_BIT_CH6  (1 << 6)
#define TZC_SEC_DMA_BIT_CH7  (1 << 7)

/**
 *  @brief TZC_SEC slave type definition
 */
typedef enum {
    TZC_SEC_SLAVE_S0_GMAC_A, /*!< TZC Slave:GMAC_A */
    TZC_SEC_SLAVE_S0_DMA,    /*!< TZC Slave:DMA */
    TZC_SEC_SLAVE_S0_GMAC_B, /*!< TZC Slave:GMAC_B */
    TZC_SEC_SLAVE_S0_PWR,    /*!< TZC Slave:PDS/HBN/HBNRAM/TOUCH */
    TZC_SEC_SLAVE_S0_SDH,    /*!< TZC Slave:SDH */
    TZC_SEC_SLAVE_S0_EMAC_A, /*!< TZC Slave:EMAC_A */
    TZC_SEC_SLAVE_S0_SDU,    /*!< TZC Slave:SDU */
    TZC_SEC_SLAVE_S0_USB,    /*!< TZC Slave:USB */

    TZC_SEC_SLAVE_S1_GLB = 8,  /*!< TZC Slave:GLB */
    TZC_SEC_SLAVE_S1_RF,       /*!< TZC Slave:RF */
    TZC_SEC_SLAVE_S1_GPIP,     /*!< TZC Slave:GPIP */
    TZC_SEC_SLAVE_S1_SEC,      /*!< TZC Slave:SEC */
    TZC_SEC_SLAVE_S1_RF2,      /*!< TZC Slave:RF2 */
    TZC_SEC_SLAVE_S1_DMA2D,    /*!< TZC Slave:DMA2D */
    TZC_SEC_SLAVE_S1_TZC1,     /*!< TZC Slave:TZC1 */
    TZC_SEC_SLAVE_S1_TZC2,     /*!< TZC Slave:TZC2 */
    TZC_SEC_SLAVE_S1_CCI,      /*!< TZC Slave:CCI */
    TZC_SEC_SLAVE_S1_MCU_MISC, /*!< TZC Slave:MCU_MISC */
    TZC_SEC_SLAVE_S1_U2_PHY,   /*!< TZC Slave:U2_PHY */
    TZC_SEC_SLAVE_S1_SPI0,     /*!< TZC Slave:SPI0 */
    TZC_SEC_SLAVE_S1_EFUSE,    /*!< TZC Slave:EFUSE */
    TZC_SEC_SLAVE_S1_PSRAM,    /*!< TZC Slave:PSRAM */
    TZC_SEC_SLAVE_S1_CAN0_1,   /*!< TZC Slave:CAN0 and CAN1 */
    TZC_SEC_SLAVE_S1_CAN2_KYS, /*!< TZC Slave:CAN2 and KYS */

    TZC_SEC_SLAVE_S2_MMCU_ATB2AXI = 24, /*!< TZC Slave:MMCU_ATB2AXI */
    TZC_SEC_SLAVE_S2_DMA1,              /*!< TZC Slave:DMA1 */
    TZC_SEC_SLAVE_S2_AUPWM,             /*!< TZC Slave:AUPWM */
    TZC_SEC_SLAVE_S2_SPI2,              /*!< TZC Slave:SPI2 */
    TZC_SEC_SLAVE_S2_RSVD28,            /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_RSVD29,            /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_RSVD30,            /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_RSVD31,            /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_MINI_MISC,         /*!< TZC Slave:MINI_MISC */
    TZC_SEC_SLAVE_S2_PWM1,              /*!< TZC Slave:MINI_PWM */
    TZC_SEC_SLAVE_S2_I2C2,              /*!< TZC Slave:MINI_I2C */
    TZC_SEC_SLAVE_S2_TIMER1,            /*!< TZC Slave:MINI_TIMER */
    TZC_SEC_SLAVE_S2_UART3,             /*!< TZC Slave:MINI_UART */
    TZC_SEC_SLAVE_S2_SPI3,              /*!< TZC Slave:MINI_SPI */
    TZC_SEC_SLAVE_S2_RSVD38,            /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S2_RSVD39,            /*!< TZC Slave:Reserved */

    TZC_SEC_SLAVE_S1A_UART0 = 40, /*!< TZC Slave:UART0 */
    TZC_SEC_SLAVE_S1A_UART1,      /*!< TZC Slave:UART1 */
    TZC_SEC_SLAVE_S1A_UART2,      /*!< TZC Slave:UART2 */
    TZC_SEC_SLAVE_S1A_IPC0,       /*!< TZC Slave:IPC0 */
    TZC_SEC_SLAVE_S1A_I2C0,       /*!< TZC Slave:I2C0 */
    TZC_SEC_SLAVE_S1A_I2C1,       /*!< TZC Slave:I2C1 */
    TZC_SEC_SLAVE_S1A_IPC1,       /*!< TZC Slave:IPC1 */
    TZC_SEC_SLAVE_S1A_AUSOLO,     /*!< TZC Slave:AUSOLO */
    TZC_SEC_SLAVE_S1A_SPI1,       /*!< TZC Slave:SPI1 */
    TZC_SEC_SLAVE_S1A_PWM0,       /*!< TZC Slave:PWM0 */
    TZC_SEC_SLAVE_S1A_TIMER0,     /*!< TZC Slave:TIMER0 */
    TZC_SEC_SLAVE_S1A_IRR,        /*!< TZC Slave:IRR */
    TZC_SEC_SLAVE_S1A_CKS,        /*!< TZC Slave:CKS */
    TZC_SEC_SLAVE_S1A_DBI,        /*!< TZC Slave:DBI */
    TZC_SEC_SLAVE_S1A_I2S,        /*!< TZC Slave:I2S */
    TZC_SEC_SLAVE_S1A_PEC,        /*!< TZC Slave:PEC */

    TZC_SEC_SLAVE_S3_EMAC_B = 56, /*!< TZC Slave:EMAC_B */
    TZC_SEC_SLAVE_S3_BZ,          /*!< TZC Slave:BZ */
    TZC_SEC_SLAVE_S3_WIFI,        /*!< TZC Slave:WIFI */
    TZC_SEC_SLAVE_S3_DMA2,        /*!< TZC Slave:MINI DMA */
    TZC_SEC_SLAVE_S3_RSVD60,      /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S3_RSVD61,      /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S3_RSVD62,      /*!< TZC Slave:Reserved */
    TZC_SEC_SLAVE_S3_RSVD63,      /*!< TZC Slave:Reserved */

    TZC_SEC_SLAVE_MM_S0_MISC = 64,   /*!< TZC Slave:MM_MISC */
    TZC_SEC_SLAVE_MM_S0_DVP2AXI_A,   /*!< TZC Slave:MM_DVP2AXI_A */
    TZC_SEC_SLAVE_MM_S0_DVP2AXI_B,   /*!< TZC Slave:MM_DVP2AXI_B */
    TZC_SEC_SLAVE_MM_S0_MJENC,       /*!< TZC Slave:MM_MJENC */
    TZC_SEC_SLAVE_MM_S0_MJDEC,       /*!< TZC Slave:MM_MJDEC */
    TZC_SEC_SLAVE_MM_S0_DTSRC,       /*!< TZC Slave:MM_DTSRC */
    TZC_SEC_SLAVE_MM_S0_R2B,         /*!< TZC Slave:MM_R2B */
    TZC_SEC_SLAVE_MM_S0_B2R,         /*!< TZC Slave:MM_B2R */
    TZC_SEC_SLAVE_MM_S0_DVP2SRAM_A,  /*!< TZC Slave:MM_DVP2SRAM_A */
    TZC_SEC_SLAVE_MM_S0_DVP2SRAM_B,  /*!< TZC Slave:MM_DVP2SRAM_B */
    TZC_SEC_SLAVE_MM_S0_DISP_OSD_L0, /*!< TZC Slave:MM_DISP_OSD_L0 */
    TZC_SEC_SLAVE_MM_S0_DISP_OSD_L1, /*!< TZC Slave:MM_DISP_OSD_L1 */
    TZC_SEC_SLAVE_MM_S0_DSI,         /*!< TZC Slave:MM_DSI */

    TZC_SEC_SLAVE_MAX, /*!< TZC slave max*/
} TZC_SEC_Slave_Type;

/**
 *  @brief TZC_SEC GLB Ctrl type definition
 */
typedef enum {
    TZC_SEC_GLB_CTRL_POR_RESET,  /*!< TZC GLB Ctrl: Power on reset */
    TZC_SEC_GLB_CTRL_CPU_RESET,  /*!< TZC GLB Ctrl: CPU reset */
    TZC_SEC_GLB_CTRL_SYS_RESET,  /*!< TZC GLB Ctrl: System reset */
    TZC_SEC_GLB_CTRL_CCPU_RESET, /*!< TZC GLB Ctrl: CCPU */
    TZC_SEC_GLB_CTRL_MISC,       /*!< TZC GLB Ctrl: MISC */
    TZC_SEC_GLB_CTRL_SRAM,       /*!< TZC GLB Ctrl: SRAM */
    TZC_SEC_GLB_CTRL_SWRESET,    /*!< TZC GLB Ctrl: Software reset */
    TZC_SEC_GLB_CTRL_BMX,        /*!< TZC GLB Ctrl: BMX */
    TZC_SEC_GLB_CTRL_DBG,        /*!< TZC GLB Ctrl: DBG */
    TZC_SEC_GLB_CTRL_MBIST,      /*!< TZC GLB Ctrl: MBIST */
    TZC_SEC_GLB_CTRL_CLK,        /*!< TZC GLB Ctrl: CLK */
    TZC_SEC_GLB_CTRL_INT,        /*!< TZC GLB Ctrl: Interrupt */
    TZC_SEC_GLB_CTRL_PWR,        /*!< TZC GLB Ctrl: Power */
    TZC_SEC_GLB_CTRL_MAX,
} TZC_SEC_GLB_Ctrl_Type;

/**
 *  @brief TZC_SEC SE Ctrl type definition
 */
typedef enum {
    TZC_SEC_SE_CTRL_SHA,  /*!< TZC SE Ctrl: SHA */
    TZC_SEC_SE_CTRL_AES,  /*!< TZC SE Ctrl: AES */
    TZC_SEC_SE_CTRL_TRNG, /*!< TZC SE Ctrl: TRNG */
    TZC_SEC_SE_CTRL_PKA,  /*!< TZC SE Ctrl: PKA */
    TZC_SEC_SE_CTRL_CDET, /*!< TZC SE Ctrl: CEDT */
    TZC_SEC_SE_CTRL_GMAC, /*!< TZC SE Ctrl: GMAC */
    TZC_SEC_SE_CTRL_MAX,
} TZC_SEC_SE_Ctrl_Type;

/**
 *  @brief TZC_SEC SF Ctrl type definition
 */
typedef enum {
    TZC_SEC_SF_CTRL_CR,  /*!< TZC SF Ctrl: control register */
    TZC_SEC_SF_CTRL_SEC, /*!< TZC SF Ctrl: security register */
    TZC_SEC_SF_CTRL_MAX,
} TZC_SEC_SF_Ctrl_Type;

/**
 *  @brief TZC_SEC SE Ctrl mode definition
 */
typedef enum {
    TZC_SEC_SE_MODE_ARB, /*!< TZC SE Ctrl mode: cpus arbitrate */
    TZC_SEC_SE_MODE_TZC, /*!< TZC SE Ctrl: TZC control as group */
} TZC_SEC_SE_Ctrl_Mode;

/**
 *  @brief TZC_SEC SF Ctrl mode definition
 */
typedef enum {
    TZC_SEC_SF_MODE_ARB, /*!< TZC SF Ctrl mode: cpus arbitrate */
    TZC_SEC_SF_MODE_TZC, /*!< TZC SF Ctrl: TZC control as group */
} TZC_SEC_SF_Ctrl_Mode;

/**
 *  @brief TZC_SEC Auth group
 */
typedef enum {
    TZC_SEC_AUTH_GRP_0, /*!< TZC auth group 0 */
    TZC_SEC_AUTH_GRP_1, /*!< TZC auth group 1 */
} TZC_SEC_Auth_Group;

/**
 *  @brief TZC_SEC Advance Auth group
 */
typedef enum {
    TZC_SEC_ADV_AUTH_GRP_0_DBUS = 0x01, /*!< TZC advance auth group 0 DBUS */
    TZC_SEC_ADV_AUTH_GRP_0_IBUS = 0x02, /*!< TZC advance auth group 0 IBUS */
    TZC_SEC_ADV_AUTH_GRP_1_DBUS = 0x04, /*!< TZC advance auth group 1 DBUS */
    TZC_SEC_ADV_AUTH_GRP_1_IBUS = 0x08, /*!< TZC advance auth group 1 IBUS */
} TZC_SEC_Advance_Auth_Group;

/**
 *  @brief TZC_SEC Mini Ram Advance Auth group
 */
typedef enum {
    TZC_SEC_ADV_AUTH_GRP_MINI_0_IBUS = 0x01, /*!< TZC advance auth group 0 IBUS */
    TZC_SEC_ADV_AUTH_GRP_MINI_0_DBUS = 0x02, /*!< TZC advance auth group 0 DBUS */
    TZC_SEC_ADV_AUTH_GRP_MINI_1_IBUS = 0x04, /*!< TZC advance auth group 1 IBUS */
    TZC_SEC_ADV_AUTH_GRP_MINI_1_DBUS = 0x08, /*!< TZC advance auth group 1 DBUS */
} TZC_SEC_Advance_Auth_Group_Mini;

/**
 *  @brief TZC_SEC Flash Advance Auth group
 */
typedef enum {
    TZC_SEC_ADV_AUTH_GRP_FLASH_0_IBUS = 0x01, /*!< TZC advance auth group 0 IBUS */
    TZC_SEC_ADV_AUTH_GRP_FLASH_0_DBUS = 0x02, /*!< TZC advance auth group 0 DBUS */
    TZC_SEC_ADV_AUTH_GRP_FLASH_1_IBUS = 0x04, /*!< TZC advance auth group 1 IBUS */
    TZC_SEC_ADV_AUTH_GRP_FLASH_1_DBUS = 0x08, /*!< TZC advance auth group 1 DBUS */
} TZC_SEC_Advance_Auth_Group_FLASH;

/*@} end of group TZC_SEC_Public_Types */

/** @defgroup  TZC_SEC_Public_Constants
 *  @{
 */

/*@} end of group TZC_SEC_Public_Constants */

/** @defgroup  TZC_SEC_Public_Macros
 *  @{
 */

#define TZC_SEC_MAX_AUTH_GRP 3
/*@} end of group TZC_SEC_Public_Macros */

/** @defgroup  TZC_SEC_Public_Functions
 *  @{
 */
void Tzc_Sec_Set_Sboot_Done(void);
void Tzc_Sec_Set_Bus_Remap(uint8_t busRmpEn);
void Tzc_Sec_Set_Master_Group(TZC_SEC_Master_Type masterType, uint8_t group);
void Tzc_Sec_Set_Master_DMA_Group(uint8_t dmaType, uint32_t channel_bits, uint8_t group);
void Tzc_Sec_Set_Codec_Group(uint8_t group);
void Tzc_Sec_Set_CPU_Group(uint8_t cpu, uint8_t group);
void Tzc_Sec_Set_Slave_Group(TZC_SEC_Slave_Type slaveType, uint8_t group);
void Tzc_Sec_Set_Glb_Ctrl_Group(TZC_SEC_GLB_Ctrl_Type slaveType, uint8_t group);
void Tzc_Sec_ROM_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_ROM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_OCRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_OCRAM_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_TCM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_TCM_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_WRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_WRAM_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_XRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_XRAM_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_Mini_Ram_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_Mini_Ram_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_Flash_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_Flash_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_Flash_Access_Set_Regionx(uint8_t group);
void Tzc_Sec_PSRAMB_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_PSRAMB_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group);
void Tzc_Sec_PSRAMB_Access_Release(void);
void Tzc_Sec_Set_Se_Ctrl_Mode(TZC_SEC_SE_Ctrl_Mode mode);
void Tzc_Sec_Set_Sf_Ctrl_Mode(TZC_SEC_SF_Ctrl_Mode mode);
void Tzc_Sec_Set_Se_Group(TZC_SEC_SE_Ctrl_Type slaveType, uint8_t group);
void Tzc_Sec_Set_Sf_Group(TZC_SEC_SF_Ctrl_Type slaveType, uint8_t group);
void Tzc_Sec_Set_WTD_Rst_Delay(uint16_t delayValue);
/*@} end of group TZC_SEC_Public_Functions */

/*@} end of group TZC_SEC */

/*@} end of group BL616D_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616D_TZC_SEC_H__ */
