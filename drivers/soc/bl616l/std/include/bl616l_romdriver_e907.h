/**
  ******************************************************************************
  * @file    bl616d_romdriver_e907.h
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
#ifndef __BL616D_ROMDRIVER_E907_H__
#define __BL616D_ROMDRIVER_E907_H__

#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>
#include "bl616l_aon.h"
#include "bl616l_clock.h"
#include "bl616l_common.h"
#include "bl616l_ef_cfg.h"
#include "bl616l_glb.h"
#include "bl616l_hbn.h"
#include "bl616l_pds.h"


#include "bflb_efuse.h"

/** @addtogroup  bl616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  ROMDRIVER
 *  @{
 */

/** @defgroup  ROMDRIVER_Public_Types
 *  @{
 */


#define ROM_API_INDEX_MAX   (432)

typedef enum {
    ROM_API_INDEX_VERSION      = 0,
    ROM_API_INDEX_RSVD_0       = 1,
    ROM_API_INDEX_RSVD_1       = 2,
    ROM_API_INDEX_RSVD_LAST    = 3,

    ROM_API_INDEX_AON_Power_On_RF                                   = 4,
    ROM_API_INDEX_AON_Power_Off_RF                                  = 5,
    ROM_API_INDEX_AON_Power_On_XTAL                                 = 6,
    ROM_API_INDEX_AON_Set_Xtal_CapCode                              = 7,
    ROM_API_INDEX_AON_Get_Xtal_CapCode                              = 8,
    ROM_API_INDEX_AON_Power_Off_XTAL                                = 9,
    ROM_API_INDEX_AON_XTAL_Enter_LowPower                           = 10,
    ROM_API_INDEX_AON_XTAL_Exit_LowPower                            = 11,
    ROM_API_INDEX_AON_LDO18_IO_Switch_Efuse                         = 12,
    ROM_API_INDEX_AON_LDO18_IO_Switch_Psram                         = 13,
    ROM_API_INDEX_AON_LDO18_IO_Switch_Flash                         = 14,
    ROM_API_INDEX_AON_Set_LDO18_IO_Sstart_Delay                     = 15,
    ROM_API_INDEX_AON_SW_Set_Ldo18_IO_Vout                          = 16,
    ROM_API_INDEX_AON_SW_Trim_Ldo18_IO_Vout                         = 17,
    ROM_API_INDEX_AON_Trim_Ldo18_IO_Vout                            = 18,
    ROM_API_INDEX_Clock_System_Clock_Get                            = 19,
    ROM_API_INDEX_Clock_Get_PSRAMB_Clk                              = 20,
    ROM_API_INDEX_Clock_Audio_ADC_Clock_Get                         = 21,
    ROM_API_INDEX_Clock_PWM_Clk_Get                                 = 22,
    ROM_API_INDEX_Clock_Peripheral_Clock_Get                        = 23,
    ROM_API_INDEX_SystemCoreClockGet                                = 24,
    ROM_API_INDEX_CPU_Set_MTimer_RST                                = 25,
    ROM_API_INDEX_CPU_Reset_MTimer                                  = 26,
    ROM_API_INDEX_CPU_Set_MTimer_CLK                                = 27,
    ROM_API_INDEX_CPU_Get_MTimer_Source_Clock                       = 28,
    ROM_API_INDEX_CPU_Get_MTimer_Clock                              = 29,
    ROM_API_INDEX_CPU_Get_MTimer_Counter                            = 30,
    ROM_API_INDEX_CPU_Get_CPU_Cycle                                 = 31,
    ROM_API_INDEX_CPU_Get_MTimer_US                                 = 32,
    ROM_API_INDEX_CPU_Get_MTimer_MS                                 = 33,
    ROM_API_INDEX_CPU_MTimer_Delay_US                               = 34,
    ROM_API_INDEX_CPU_MTimer_Delay_MS                               = 35,
    ROM_API_INDEX_ASM_Delay_Us                                      = 36,
    ROM_API_INDEX_arch_delay_us                                     = 37,
    ROM_API_INDEX_arch_delay_ms                                     = 38,
    ROM_API_INDEX_bflb_ef_ctrl_get_common_trim_list                 = 39,
    ROM_API_INDEX_bflb_efuse_get_device_info                        = 40,
    ROM_API_INDEX_bflb_efuse_read_mac_address                       = 41,
    ROM_API_INDEX_bflb_efuse_get_chipid                             = 42,
    ROM_API_INDEX_bflb_efuse_is_mac_address_slot_empty              = 43,
    ROM_API_INDEX_bflb_efuse_read_mac_address_opt                   = 44,
    ROM_API_INDEX_bflb_efuse_get_adc_trim                           = 45,
    ROM_API_INDEX_bflb_efuse_get_adc_tsen_trim                      = 46,
    ROM_API_INDEX_bflb_efuse_read_secure_boot                       = 47,
    ROM_API_INDEX_bflb_efuse_enable_aes                             = 48,
    ROM_API_INDEX_bflb_efuse_rw_lock_aes_key                        = 49,
    ROM_API_INDEX_bflb_efuse_rw_lock_dbg_key                        = 50,
    ROM_API_INDEX_bflb_efuse_write_lock_pk_hash                     = 51,
    ROM_API_INDEX_bflb_efuse_write_lock_usb_pid_vid                 = 52,
    ROM_API_INDEX_GLB_Set_MCU_Muxpll_160M_Sel                       = 53,
    ROM_API_INDEX_GLB_Get_MCU_Muxpll_160M_Sel                       = 54,
    ROM_API_INDEX_GLB_Set_MCU_Muxpll_80M_Sel                        = 55,
    ROM_API_INDEX_GLB_Get_MCU_Muxpll_80M_Sel                        = 56,
    ROM_API_INDEX_GLB_Set_ISP_Muxpll_80M_Sel                        = 57,
    ROM_API_INDEX_GLB_Get_ISP_Muxpll_80M_Sel                        = 58,
    ROM_API_INDEX_GLB_Power_On_XTAL_And_PLL_CLK                     = 59,
    ROM_API_INDEX_GLB_Power_Off_WIFIPLL                             = 60,
    ROM_API_INDEX_GLB_WIFIPLL_Ref_Clk_Sel                           = 61,
    ROM_API_INDEX_GLB_Power_On_WIFIPLL                              = 62,
    ROM_API_INDEX_GLB_Set_MCU_System_CLK_Div                        = 63,
    ROM_API_INDEX_GLB_Get_MCU_System_CLK_Div                        = 64,
    ROM_API_INDEX_GLB_Set_MCU_System_CLK                            = 65,
    ROM_API_INDEX_GLB_Get_Core_Type                                 = 66,
    ROM_API_INDEX_GLB_Set_CPU_Reset_Address                         = 67,
    ROM_API_INDEX_GLB_Clr_EMI_Reset_Gate                            = 68,
    ROM_API_INDEX_GLB_Set_ADC_CLK                                   = 69,
    ROM_API_INDEX_GLB_Set_DMA_CLK                                   = 70,
    ROM_API_INDEX_GLB_Set_IR_CLK                                    = 71,
    ROM_API_INDEX_GLB_IR_RX_GPIO_Sel                                = 72,
    ROM_API_INDEX_GLB_Set_UART_CLK                                  = 73,
    ROM_API_INDEX_GLB_UART_Fun_Sel                                  = 74,
    ROM_API_INDEX_GLB_Set_SF_CLK                                    = 75,
    ROM_API_INDEX_GLB_Set_I2C_CLK                                   = 76,
    ROM_API_INDEX_GLB_Set_I2S_CLK                                   = 77,
    ROM_API_INDEX_GLB_Set_SPI0_CLK                                  = 78,
    ROM_API_INDEX_GLB_Set_SPI1_CLK                                  = 79,
    ROM_API_INDEX_GLB_SPI0_Sig_Swap_Set                             = 80,
    ROM_API_INDEX_GLB_SPI1_Sig_Swap_Set                             = 81,
    ROM_API_INDEX_GLB_Set_PWM1_IO_Sel                               = 82,
    ROM_API_INDEX_GLB_Set_PWM_CLK                                   = 83,
    ROM_API_INDEX_GLB_Set_PDM_IO_Sel                                = 84,
    ROM_API_INDEX_GLB_Set_DBI_CLK                                   = 85,
    ROM_API_INDEX_GLB_Set_DIG_CLK_Sel                               = 86,
    ROM_API_INDEX_GLB_Set_DIG_512K_CLK                              = 87,
    ROM_API_INDEX_GLB_Set_DIG_32K_CLK                               = 88,
    ROM_API_INDEX_GLB_Platform_Wakeup_PDS_Enable                    = 89,
    ROM_API_INDEX_GLB_Sel_MCU_TMR_GPIO_Clock                        = 90,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out0_Sel                       = 91,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out1_Sel                       = 92,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out2_Sel                       = 93,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out3_Sel                       = 94,
    ROM_API_INDEX_GLB_Set_Audio_AUTO_CLK                            = 95,
    ROM_API_INDEX_GLB_Set_Audio_SOLO_CLK                            = 96,
    ROM_API_INDEX_GLB_Set_ISP_CORE_CLK                              = 97,
    ROM_API_INDEX_GLB_Invert_ETH_RX_CLK                             = 98,
    ROM_API_INDEX_GLB_Invert_ETH_TX_CLK                             = 99,
    ROM_API_INDEX_GLB_Invert_ETH_REF_O_CLK                          = 100,
    ROM_API_INDEX_GLB_Set_ETH_REF_O_CLK_Sel                         = 101,
    ROM_API_INDEX_GLB_Set_CAM_CLK                                   = 102,
    ROM_API_INDEX_GLB_Set_SDH_CLK                                   = 103,
    ROM_API_INDEX_GLB_Set_PEC_CLK                                   = 104,
    ROM_API_INDEX_GLB_Set_AHB2QSPI_CLK                              = 105,
    ROM_API_INDEX_GLB_Config_SDIO_Host_Cmd0_Reset                   = 106,
    ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_System                 = 107,
    ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_SDU                    = 108,
    ROM_API_INDEX_GLB_Config_SDIO_Host_Interrupt_CPU                = 109,
    ROM_API_INDEX_GLB_UART_Sig_Swap_Set                             = 110,
    ROM_API_INDEX_GLB_Swap_MCU_SPI_0_MOSI_With_MISO                 = 111,
    ROM_API_INDEX_GLB_Swap_MCU_SPI_1_MOSI_With_MISO                 = 112,
    ROM_API_INDEX_GLB_Set_MCU_SPI_0_ACT_MOD_Sel                     = 113,
    ROM_API_INDEX_GLB_Set_MCU_SPI_1_ACT_MOD_Sel                     = 114,
    ROM_API_INDEX_GLB_Set_SFlash_IO_PARM                            = 115,
    ROM_API_INDEX_GLB_Set_SFlash2_IO_PARM                           = 116,
    ROM_API_INDEX_GLB_Set_Psram_Pad_For_Flash                       = 117,
    ROM_API_INDEX_GLB_Clr_Reset_Reason                              = 118,
    ROM_API_INDEX_GLB_Get_Reset_Reason                              = 119,
    ROM_API_INDEX_GLB_AHB_MCU_Software_Reset                        = 120,
    ROM_API_INDEX_GLB_Disrst_Set                                    = 121,
    ROM_API_INDEX_GLB_PER_Clock_Gate                                = 122,
    ROM_API_INDEX_GLB_PER_Clock_UnGate                              = 123,
    ROM_API_INDEX_GLB_PLL_CGEN_Clock_Gate                           = 124,
    ROM_API_INDEX_GLB_PLL_CGEN_Clock_UnGate                         = 125,
    ROM_API_INDEX_GLB_Set_PKA_CLK_Sel                               = 126,
    ROM_API_INDEX_GLB_MCU_SW_System_Reset                           = 127,
    ROM_API_INDEX_GLB_SW_System_Reset                               = 128,
    ROM_API_INDEX_GLB_SW_CPU_Reset                                  = 129,
    ROM_API_INDEX_GLB_SW_POR_Reset                                  = 130,
    ROM_API_INDEX_GLB_Set_Auto_Calc_Xtal_Type                       = 131,
    ROM_API_INDEX_GLB_Get_Auto_Calc_Xtal_Type                       = 132,
    ROM_API_INDEX_GLB_Set_Flash_Id_Value                            = 133,
    ROM_API_INDEX_GLB_Get_Flash_Id_Value                            = 134,
    ROM_API_INDEX_GLB_Trim_Ldo18ioVoutSel                           = 135,
    ROM_API_INDEX_GLB_Trim_Ldo18ioVoutTrim                          = 136,
    ROM_API_INDEX_GLB_Power_Down_Ldo18ioVout                        = 137,
    ROM_API_INDEX_GLB_Power_Up_Ldo18ioVout                          = 138,
    ROM_API_INDEX_GLB_Set_SRAM_RET                                  = 139,
    ROM_API_INDEX_GLB_Get_SRAM_RET                                  = 140,
    ROM_API_INDEX_GLB_Set_SRAM_SLP                                  = 141,
    ROM_API_INDEX_GLB_Get_SRAM_SLP                                  = 142,
    ROM_API_INDEX_GLB_Set_SRAM_PARM                                 = 143,
    ROM_API_INDEX_GLB_Get_SRAM_PARM                                 = 144,
    ROM_API_INDEX_GLB_Set_EM_Sel                                    = 145,
    ROM_API_INDEX_GLB_Set_PSRAMB_CLK_Sel                            = 146,
    ROM_API_INDEX_GLB_Config_WIFIPLL                                = 147,
    ROM_API_INDEX_GLB_Simple_Set_MCU_System_CLK                     = 148,
    ROM_API_INDEX_GLB_Embedded_Flash_Pad_Enable                     = 149,
    ROM_API_INDEX_GLB_SF_Write_Protect_Enable                       = 150,
    ROM_API_INDEX_GLB_SF_Write_Protect_Disable                      = 151,
    ROM_API_INDEX_GLB_SF_Write_Protect_Set                          = 152,
    ROM_API_INDEX_GLB_Write_SW_Reserved_Register                    = 153,
    ROM_API_INDEX_GLB_Read_SW_Reserved_Register                     = 154,
    ROM_API_INDEX_GLB_Bus_Decoder_Err_Enable                        = 155,
    ROM_API_INDEX_GLB_Bus_Decoder_Err_Disable                       = 156,
    ROM_API_INDEX_HBN_Ctrl_Ldo_Aon_Vout_in_HBN                      = 157,
    ROM_API_INDEX_HBN_Level_Select                                  = 158,
    ROM_API_INDEX_HBN_Power_Down_Flash                              = 159,
    ROM_API_INDEX_HBN_Reset                                         = 160,
    ROM_API_INDEX_HBN_Get_POR_OUT_State                             = 161,
    ROM_API_INDEX_HBN_Get_BOD_OUT_State                             = 162,
    ROM_API_INDEX_HBN_Set_BOD_Config                                = 163,
    ROM_API_INDEX_HBN_Set_BOD_Cfg                                   = 164,
    ROM_API_INDEX_HBN_Get_Reset_Event                               = 165,
    ROM_API_INDEX_HBN_Clr_Reset_Event                               = 166,
    ROM_API_INDEX_HBN_SW_Set_Ldo_Aon_Vout                           = 167,
    ROM_API_INDEX_HBN_SW_Trim_Ldo_Aon_Vout                          = 168,
    ROM_API_INDEX_HBN_Set_RTC_CLK_Sel                               = 169,
    ROM_API_INDEX_HBN_Get_RTC_CLK_Sel                               = 170,
    ROM_API_INDEX_HBN_Set_Xtal_Divied_CLK                           = 171,
    ROM_API_INDEX_HBN_32K_Sel                                       = 172,
    ROM_API_INDEX_HBN_Set_UART_CLK_Sel                              = 173,
    ROM_API_INDEX_HBN_Set_GPIO_INT_DET_CLK_Sel                      = 174,
    ROM_API_INDEX_HBN_Get_MCU_XCLK_Sel                              = 175,
    ROM_API_INDEX_HBN_Set_MCU_XCLK_Sel                              = 176,
    ROM_API_INDEX_HBN_Get_MCU_Root_CLK_Sel                          = 177,
    ROM_API_INDEX_HBN_Set_MCU_Root_CLK_Sel                          = 178,
    ROM_API_INDEX_HBN_Set_HRAM_slp                                  = 179,
    ROM_API_INDEX_HBN_Set_HRAM_Ret                                  = 180,
    ROM_API_INDEX_HBN_SW_Set_Ldo_Soc_Vout                           = 181,
    ROM_API_INDEX_HBN_SW_Trim_Ldo_Soc_Vout                          = 182,
    ROM_API_INDEX_HBN_Set_Ldo_Soc_Vout_in_Lowpower                  = 183,
    ROM_API_INDEX_HBN_Trim_Ldo_Soc_Vout_in_Lowpower                 = 184,
    ROM_API_INDEX_HBN_Ctrl_Ldo_Soc_Mode_by_HW                       = 185,
    ROM_API_INDEX_HBN_Set_Ldo_Soc_Mode                              = 186,
    ROM_API_INDEX_HBN_SW_Set_Ldo_Sys_Vout                           = 187,
    ROM_API_INDEX_HBN_SW_Trim_Ldo_Sys_Vout                          = 188,
    ROM_API_INDEX_HBN_Set_Ldo_Sys_Vout_in_Lowpower                  = 189,
    ROM_API_INDEX_HBN_Ctrl_Ldo_Sys_Mode_by_HW                       = 190,
    ROM_API_INDEX_HBN_Set_Ldo_Sys_Mode                              = 191,
    ROM_API_INDEX_HBN_Power_Off_LDO18_IO                            = 192,
    ROM_API_INDEX_HBN_Power_On_LDO18_IO                             = 193,
    ROM_API_INDEX_HBN_Set_Xtal_32K_Inverter_Amplify_Strength        = 194,
    ROM_API_INDEX_HBN_Set_Xtal_32K_Regulator                        = 195,
    ROM_API_INDEX_HBN_Start_Xtal_32K                                = 196,
    ROM_API_INDEX_HBN_Clear_Xtal_32K_Fast_Startup                   = 197,
    ROM_API_INDEX_HBN_Get_Xtal_32K_Ready_State                      = 198,
    ROM_API_INDEX_HBN_Power_On_Xtal_32K                             = 199,
    ROM_API_INDEX_HBN_Power_Off_Xtal_32K                            = 200,
    ROM_API_INDEX_HBN_Keep_On_RC32K                                 = 201,
    ROM_API_INDEX_HBN_Power_Off_RC32K                               = 202,
    ROM_API_INDEX_HBN_Trim_RC32K                                    = 203,
    ROM_API_INDEX_HBN_Trim_Ldo_Soc_Vout                             = 204,
    ROM_API_INDEX_HBN_Trim_Ldo_Aon_Vout                             = 205,
    ROM_API_INDEX_HBN_Trim_Ldo_Sys_Vout                             = 206,
    ROM_API_INDEX_HBN_Set_RC32M_Speed                               = 207,
    ROM_API_INDEX_HBN_Get_RC32M_Speed                               = 208,
    ROM_API_INDEX_HBN_Trim_RC32M                                    = 209,
    ROM_API_INDEX_HBN_Get_Status_Flag                               = 210,
    ROM_API_INDEX_HBN_Set_Status_Flag                               = 211,
    ROM_API_INDEX_HBN_Get_Wakeup_Addr                               = 212,
    ROM_API_INDEX_HBN_Set_Wakeup_Addr                               = 213,
    ROM_API_INDEX_HBN_Get_Core_Unhalt_Config                        = 214,
    ROM_API_INDEX_HBN_Set_Core_Reboot_Config                        = 215,
    ROM_API_INDEX_HBN_Get_User_Boot_Config                          = 216,
    ROM_API_INDEX_HBN_Set_User_Boot_Config                          = 217,
    ROM_API_INDEX_HBN_Get_Ldo18io_Power_Delay_Config                = 218,
    ROM_API_INDEX_HBN_Set_Ldo18io_Power_Delay_Config                = 219,
    ROM_API_INDEX_HBN_Set_Xtal_Type                                 = 220,
    ROM_API_INDEX_HBN_Get_Xtal_Type                                 = 221,
    ROM_API_INDEX_HBN_Get_Xtal_Value                                = 222,
    ROM_API_INDEX_HBN_Set_Flash_Power_Delay                         = 223,
    ROM_API_INDEX_HBN_Get_Flash_Power_Delay                         = 224,
    ROM_API_INDEX_HBN_Clear_RTC_Counter                             = 225,
    ROM_API_INDEX_HBN_Enable_RTC_Counter                            = 226,
    ROM_API_INDEX_HBN_Set_RTC_Timer                                 = 227,
    ROM_API_INDEX_HBN_Get_RTC_Timer_Val                             = 228,
    ROM_API_INDEX_HBN_Clear_RTC_INT                                 = 229,
    ROM_API_INDEX_HBN_GPIO_INT_Enable                               = 230,
    ROM_API_INDEX_HBN_GPIO_INT_Disable                              = 231,
    ROM_API_INDEX_HBN_Get_INT_State                                 = 232,
    ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode                           = 233,
    ROM_API_INDEX_HBN_Clear_IRQ                                     = 234,
    ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg                                  = 235,
    ROM_API_INDEX_HBN_Set_Aon_Pad_IntMode                           = 236,
    ROM_API_INDEX_HBN_Pin_WakeUp_Mask                               = 237,
    ROM_API_INDEX_HBN_Set_Aon_Pad_IntMask                           = 238,
    ROM_API_INDEX_HBN_Aon_Pad_Ctrl                                  = 239,
    ROM_API_INDEX_HBN_Aon_Pad_Cfg                                   = 240,
    ROM_API_INDEX_HBN_Aon_Pad_Cfg_Set                               = 241,
    ROM_API_INDEX_HBN_Enable_BOD_IRQ                                = 242,
    ROM_API_INDEX_HBN_Disable_BOD_IRQ                               = 243,
    ROM_API_INDEX_HBN_Aon_Pad_WakeUpCfg                             = 244,
    ROM_API_INDEX_HBN_Wdg_Init                                      = 245,
    ROM_API_INDEX_HBN_Wdg_Start                                     = 246,
    ROM_API_INDEX_HBN_Wdg_Stop                                      = 247,
    ROM_API_INDEX_HBN_Wdg_Get_Countervalue                          = 248,
    ROM_API_INDEX_HBN_Wdg_Set_Countervalue                          = 249,
    ROM_API_INDEX_HBN_Wdg_Reset_Countervalue                        = 250,
    ROM_API_INDEX_HBN_Wdg_Compint_Clear                             = 251,
    ROM_API_INDEX_HBN_Get_Version                                   = 252,
    ROM_API_INDEX_HBN_Set_Version                                   = 253,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask                          = 254,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode                          = 255,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr                           = 256,
    ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus                        = 257,
    ROM_API_INDEX_PDS_Set_All_GPIO_Pad_IntClr                       = 258,
    ROM_API_INDEX_PDS_Get_All_GPIO_Pad_IntStatus                    = 259,
    ROM_API_INDEX_PDS_Disable_ALL_GPIO_Keep                         = 260,
    ROM_API_INDEX_PDS_Enable_GPIO_Keep                              = 261,
    ROM_API_INDEX_PDS_Set_PDS_GPIO_INT_DET_CLK_Sel                  = 262,
    ROM_API_INDEX_PDS_Enable_WRAM_Access_Direct                     = 263,
    ROM_API_INDEX_PDS_Disable_WRAM_Access_Direct                    = 264,
    ROM_API_INDEX_PDS_Set_Ldo_Soc_Enter_lp_delay                    = 265,
    ROM_API_INDEX_PDS_Set_Ldo_Soc_Exit_lp_delay                     = 266,
    ROM_API_INDEX_PDS_Set_Ldo_Sys_Enter_lp_delay                    = 267,
    ROM_API_INDEX_PDS_Set_Ldo_Sys_Exit_lp_delay                     = 268,
    ROM_API_INDEX_PDS_Set_MCU0_Clock_Enable                         = 269,
    ROM_API_INDEX_PDS_Set_MCU0_Clock_Disable                        = 270,
    ROM_API_INDEX_PDS_Pu_PLL_Enable                                 = 271,
    ROM_API_INDEX_PDS_Pu_PLL_Disable                                = 272,
    ROM_API_INDEX_PDS_Enable                                        = 273,
    ROM_API_INDEX_PDS_Force_Config                                  = 274,
    ROM_API_INDEX_PDS_RAM_Config                                    = 275,
    ROM_API_INDEX_PDS_Set_All_WRAM_Retention                        = 276,
    ROM_API_INDEX_PDS_Set_All_OCRAM_Retention                       = 277,
    ROM_API_INDEX_PDS_Set_All_WRAM_Sleep                            = 278,
    ROM_API_INDEX_PDS_Set_All_OCRAM_Sleep                           = 279,
    ROM_API_INDEX_PDS_Disable_Ctrl_Ram                              = 280,
    ROM_API_INDEX_PDS_Default_Level_Config                          = 281,
    ROM_API_INDEX_PDS_IntMask                                       = 282,
    ROM_API_INDEX_PDS_Get_IntStatus                                 = 283,
    ROM_API_INDEX_PDS_IntClear                                      = 284,
    ROM_API_INDEX_PDS_Set_Wakeup_Src_IntMask                        = 285,
    ROM_API_INDEX_PDS_Get_Wakeup_Src                                = 286,
    ROM_API_INDEX_PDS_Get_All_Wakeup_Src                            = 287,
    ROM_API_INDEX_PDS_Mask_All_Wakeup_Src                           = 288,
    ROM_API_INDEX_PDS_Power_Off_WB                                  = 289,
    ROM_API_INDEX_PDS_Power_On_WB                                   = 290,
    ROM_API_INDEX_PDS_Set_USB_Suspend                               = 291,
    ROM_API_INDEX_PDS_Set_USB_Resume                                = 292,
    ROM_API_INDEX_PDS_Trim_USB20                                    = 293,
    ROM_API_INDEX_FUNC_EMPTY_START,

    ROM_API_INDEX_FUNC_EMPTY_END    = (ROM_API_INDEX_MAX - 1)

} ROM_API_INDEX_e;

/*@} end of group ROMDRIVER_Public_Types */

/** @defgroup  ROMDRIVER_Public_Constants
 *  @{
 */

/*@} end of group ROMDRIVER_Public_Constants */

/** @defgroup  ROMDRIVER_Public_Macros
 *  @{
 */


#ifndef STD_ROMAPI_TABLE
#define STD_ROMAPI_TABLE      (0x90019000)
#endif

#define STD_ROM_APITABLE      ((uint32_t *)STD_ROMAPI_TABLE)


#define RomDriver_AON_Power_On_RF \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_RF])

#define RomDriver_AON_Power_Off_RF \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_RF])

#define RomDriver_AON_Power_On_XTAL \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_XTAL])

#define RomDriver_AON_Set_Xtal_CapCode \
    ((BL_Err_Type (*)(uint8_t capIn, uint8_t capOut)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_CapCode])

#define RomDriver_AON_Get_Xtal_CapCode \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Get_Xtal_CapCode])

#define RomDriver_AON_Power_Off_XTAL \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_XTAL])

#define RomDriver_AON_XTAL_Enter_LowPower \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_XTAL_Enter_LowPower])

#define RomDriver_AON_XTAL_Exit_LowPower \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_XTAL_Exit_LowPower])

#define RomDriver_AON_LDO18_IO_Switch_Efuse \
    ((BL_Err_Type (*)(uint8_t en)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_LDO18_IO_Switch_Efuse])

#define RomDriver_AON_LDO18_IO_Switch_Psram \
    ((BL_Err_Type (*)(uint8_t en)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_LDO18_IO_Switch_Psram])

#define RomDriver_AON_LDO18_IO_Switch_Flash \
    ((BL_Err_Type (*)(uint8_t en)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_LDO18_IO_Switch_Flash])

#define RomDriver_AON_Set_LDO18_IO_Sstart_Delay \
    ((BL_Err_Type (*)(uint8_t delay)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_LDO18_IO_Sstart_Delay])

#define RomDriver_AON_SW_Set_Ldo18_IO_Vout \
    ((BL_Err_Type (*)(uint8_t ldoLevel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_SW_Set_Ldo18_IO_Vout])

#define RomDriver_AON_SW_Trim_Ldo18_IO_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_SW_Trim_Ldo18_IO_Vout])

#define RomDriver_AON_Trim_Ldo18_IO_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo18_IO_Vout])

#define RomDriver_Clock_System_Clock_Get \
    ((uint32_t (*)(BL_System_Clock_Type type)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Clock_System_Clock_Get])

#define RomDriver_Clock_Get_PSRAMB_Clk \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Clock_Get_PSRAMB_Clk])

#define RomDriver_Clock_Audio_ADC_Clock_Get \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Clock_Audio_ADC_Clock_Get])

#define RomDriver_Clock_PWM_Clk_Get \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Clock_PWM_Clk_Get])

#define RomDriver_Clock_Peripheral_Clock_Get \
    ((uint32_t (*)(BL_Peripheral_Type type)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Clock_Peripheral_Clock_Get])

#define RomDriver_SystemCoreClockGet \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_SystemCoreClockGet])

#define RomDriver_CPU_Set_MTimer_RST \
    ((BL_Err_Type (*)(uint8_t rstEn)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Set_MTimer_RST])

#define RomDriver_CPU_Reset_MTimer \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Reset_MTimer])

#define RomDriver_CPU_Set_MTimer_CLK \
    ((BL_Err_Type (*)(uint8_t enable, BL_MTimer_Source_Clock_Type mTimerSourceClockType, uint16_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Set_MTimer_CLK])

#define RomDriver_CPU_Get_MTimer_Source_Clock \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_Source_Clock])

#define RomDriver_CPU_Get_MTimer_Clock \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_Clock])

#define RomDriver_CPU_Get_MTimer_Counter \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_Counter])

#define RomDriver_CPU_Get_CPU_Cycle \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Get_CPU_Cycle])

#define RomDriver_CPU_Get_MTimer_US \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_US])

#define RomDriver_CPU_Get_MTimer_MS \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_MS])

#define RomDriver_CPU_MTimer_Delay_US \
    ((BL_Err_Type (*)(uint32_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_MTimer_Delay_US])

#define RomDriver_CPU_MTimer_Delay_MS \
    ((BL_Err_Type (*)(uint32_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_CPU_MTimer_Delay_MS])

#define RomDriver_ASM_Delay_Us \
    ((void (*)(uint32_t core, uint32_t cnt, uint32_t loopT)) \
    STD_ROM_APITABLE[ROM_API_INDEX_ASM_Delay_Us])

#define RomDriver_arch_delay_us \
    ((void (*)(uint32_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_arch_delay_us])

#define RomDriver_arch_delay_ms \
    ((void (*)(uint32_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_arch_delay_ms])

#define RomDriver_bflb_ef_ctrl_get_common_trim_list \
    ((uint32_t (*)(const bflb_ef_ctrl_com_trim_cfg_t **ptrim_list)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_ef_ctrl_get_common_trim_list])

#define RomDriver_bflb_efuse_get_device_info \
    ((void (*)(bflb_efuse_device_info_type *device_info)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_get_device_info])

#define RomDriver_bflb_efuse_read_mac_address \
    ((int (*)(uint8_t mac[6], uint8_t reload)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_read_mac_address])

#define RomDriver_bflb_efuse_get_chipid \
    ((int (*)(uint8_t chipid[8])) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_get_chipid])

#define RomDriver_bflb_efuse_is_mac_address_slot_empty \
    ((uint8_t (*)(uint8_t slot, uint8_t reload)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_is_mac_address_slot_empty])

#define RomDriver_bflb_efuse_read_mac_address_opt \
    ((int (*)(uint8_t slot, uint8_t mac[6], uint8_t reload)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_read_mac_address_opt])

#define RomDriver_bflb_efuse_get_adc_trim \
    ((float (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_get_adc_trim])

#define RomDriver_bflb_efuse_get_adc_tsen_trim \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_get_adc_tsen_trim])

#define RomDriver_bflb_efuse_read_secure_boot \
    ((void (*)(uint8_t *sign, uint8_t *aes)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_read_secure_boot])

#define RomDriver_bflb_efuse_enable_aes \
    ((int (*)(uint8_t aes_type, uint8_t xts_mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_enable_aes])

#define RomDriver_bflb_efuse_rw_lock_aes_key \
    ((int (*)(uint8_t key_index, uint8_t rd_lock, uint8_t wr_lock)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_rw_lock_aes_key])

#define RomDriver_bflb_efuse_rw_lock_dbg_key \
    ((int (*)(uint8_t rd_lock, uint8_t wr_lock)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_rw_lock_dbg_key])

#define RomDriver_bflb_efuse_write_lock_pk_hash \
    ((int (*)(uint32_t pkhash_len)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_write_lock_pk_hash])

#define RomDriver_bflb_efuse_write_lock_usb_pid_vid \
    ((int (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_efuse_write_lock_usb_pid_vid])

#define RomDriver_GLB_Set_MCU_Muxpll_160M_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_Muxpll_160M_Sel])

#define RomDriver_GLB_Get_MCU_Muxpll_160M_Sel \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_MCU_Muxpll_160M_Sel])

#define RomDriver_GLB_Set_MCU_Muxpll_80M_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_Muxpll_80M_Sel])

#define RomDriver_GLB_Get_MCU_Muxpll_80M_Sel \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_MCU_Muxpll_80M_Sel])

#define RomDriver_GLB_Set_ISP_Muxpll_80M_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_ISP_Muxpll_80M_Sel])

#define RomDriver_GLB_Get_ISP_Muxpll_80M_Sel \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_ISP_Muxpll_80M_Sel])

#define RomDriver_GLB_Power_On_XTAL_And_PLL_CLK \
    ((BL_Err_Type (*)(uint8_t xtalType, uint8_t pllType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_On_XTAL_And_PLL_CLK])

#define RomDriver_GLB_Power_Off_WIFIPLL \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_Off_WIFIPLL])

#define RomDriver_GLB_WIFIPLL_Ref_Clk_Sel \
    ((BL_Err_Type (*)(uint8_t refClk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_WIFIPLL_Ref_Clk_Sel])

#define RomDriver_GLB_Power_On_WIFIPLL \
    ((BL_Err_Type (*)(const GLB_WIFIPLL_Cfg_Type *const cfg, uint8_t waitStable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_On_WIFIPLL])

#define RomDriver_GLB_Set_MCU_System_CLK_Div \
    ((BL_Err_Type (*)(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_System_CLK_Div])

#define RomDriver_GLB_Get_MCU_System_CLK_Div \
    ((BL_Err_Type (*)(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_MCU_System_CLK_Div])

#define RomDriver_GLB_Set_MCU_System_CLK \
    ((BL_Err_Type (*)(uint8_t clkFreq)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_System_CLK])

#define RomDriver_GLB_Get_Core_Type \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_Core_Type])

#define RomDriver_GLB_Set_CPU_Reset_Address \
    ((BL_Err_Type (*)(uint8_t coreID, uint32_t addr)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_CPU_Reset_Address])

#define RomDriver_GLB_Clr_EMI_Reset_Gate \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Clr_EMI_Reset_Gate])

#define RomDriver_GLB_Set_ADC_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_ADC_CLK])

#define RomDriver_GLB_Set_DMA_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_DMA_CLK])

#define RomDriver_GLB_Set_IR_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_IR_CLK])

#define RomDriver_GLB_IR_RX_GPIO_Sel \
    ((BL_Err_Type (*)(uint8_t gpio)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_IR_RX_GPIO_Sel])

#define RomDriver_GLB_Set_UART_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_UART_CLK])

#define RomDriver_GLB_UART_Fun_Sel \
    ((BL_Err_Type (*)(uint8_t sig, uint8_t fun)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_UART_Fun_Sel])

#define RomDriver_GLB_Set_SF_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SF_CLK])

#define RomDriver_GLB_Set_I2C_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_I2C_CLK])

#define RomDriver_GLB_Set_I2S_CLK \
    ((BL_Err_Type (*)(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_I2S_CLK])

#define RomDriver_GLB_Set_SPI0_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI0_CLK])

#define RomDriver_GLB_Set_SPI1_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI1_CLK])

#define RomDriver_GLB_SPI0_Sig_Swap_Set \
    ((BL_Err_Type (*)(uint8_t group, uint8_t swap)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SPI0_Sig_Swap_Set])

#define RomDriver_GLB_SPI1_Sig_Swap_Set \
    ((BL_Err_Type (*)(uint8_t group, uint8_t swap)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SPI1_Sig_Swap_Set])

#define RomDriver_GLB_Set_PWM1_IO_Sel \
    ((BL_Err_Type (*)(uint8_t ioSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PWM1_IO_Sel])

#define RomDriver_GLB_Set_PWM_CLK \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PWM_CLK])

#define RomDriver_GLB_Set_PDM_IO_Sel \
    ((BL_Err_Type (*)(uint8_t ioSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PDM_IO_Sel])

#define RomDriver_GLB_Set_DBI_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_DBI_CLK])

#define RomDriver_GLB_Set_DIG_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_DIG_CLK_Sel])

#define RomDriver_GLB_Set_DIG_512K_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t compensationEn, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_DIG_512K_CLK])

#define RomDriver_GLB_Set_DIG_32K_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t compensationEn, uint16_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_DIG_32K_CLK])

#define RomDriver_GLB_Platform_Wakeup_PDS_Enable \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Platform_Wakeup_PDS_Enable])

#define RomDriver_GLB_Sel_MCU_TMR_GPIO_Clock \
    ((BL_Err_Type (*)(uint8_t gpioPin)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Sel_MCU_TMR_GPIO_Clock])

#define RomDriver_GLB_Set_Chip_Clock_Out0_Sel \
    ((BL_Err_Type (*)(uint8_t clkOutType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Clock_Out0_Sel])

#define RomDriver_GLB_Set_Chip_Clock_Out1_Sel \
    ((BL_Err_Type (*)(uint8_t clkOutType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Clock_Out1_Sel])

#define RomDriver_GLB_Set_Chip_Clock_Out2_Sel \
    ((BL_Err_Type (*)(uint8_t clkOutType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Clock_Out2_Sel])

#define RomDriver_GLB_Set_Chip_Clock_Out3_Sel \
    ((BL_Err_Type (*)(uint8_t clkOutType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Clock_Out3_Sel])

#define RomDriver_GLB_Set_Audio_AUTO_CLK \
    ((BL_Err_Type (*)(uint8_t divEn)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Audio_AUTO_CLK])

#define RomDriver_GLB_Set_Audio_SOLO_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Audio_SOLO_CLK])

#define RomDriver_GLB_Set_ISP_CORE_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_ISP_CORE_CLK])

#define RomDriver_GLB_Invert_ETH_RX_CLK \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Invert_ETH_RX_CLK])

#define RomDriver_GLB_Invert_ETH_TX_CLK \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Invert_ETH_TX_CLK])

#define RomDriver_GLB_Invert_ETH_REF_O_CLK \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Invert_ETH_REF_O_CLK])

#define RomDriver_GLB_Set_ETH_REF_O_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_ETH_REF_O_CLK_Sel])

#define RomDriver_GLB_Set_CAM_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_CAM_CLK])

#define RomDriver_GLB_Set_SDH_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SDH_CLK])

#define RomDriver_GLB_Set_PEC_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PEC_CLK])

#define RomDriver_GLB_Set_AHB2QSPI_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_AHB2QSPI_CLK])

#define RomDriver_GLB_Config_SDIO_Host_Cmd0_Reset \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_SDIO_Host_Cmd0_Reset])

#define RomDriver_GLB_Config_SDIO_Host_Reset_System \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_System])

#define RomDriver_GLB_Config_SDIO_Host_Reset_SDU \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_SDU])

#define RomDriver_GLB_Config_SDIO_Host_Interrupt_CPU \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_SDIO_Host_Interrupt_CPU])

#define RomDriver_GLB_UART_Sig_Swap_Set \
    ((BL_Err_Type (*)(uint8_t group, uint8_t swap)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_UART_Sig_Swap_Set])

#define RomDriver_GLB_Swap_MCU_SPI_0_MOSI_With_MISO \
    ((BL_Err_Type (*)(BL_Fun_Type newState)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Swap_MCU_SPI_0_MOSI_With_MISO])

#define RomDriver_GLB_Swap_MCU_SPI_1_MOSI_With_MISO \
    ((BL_Err_Type (*)(BL_Fun_Type newState)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Swap_MCU_SPI_1_MOSI_With_MISO])

#define RomDriver_GLB_Set_MCU_SPI_0_ACT_MOD_Sel \
    ((BL_Err_Type (*)(uint8_t mod)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_SPI_0_ACT_MOD_Sel])

#define RomDriver_GLB_Set_MCU_SPI_1_ACT_MOD_Sel \
    ((BL_Err_Type (*)(uint8_t mod)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_SPI_1_ACT_MOD_Sel])

#define RomDriver_GLB_Set_SFlash_IO_PARM \
    ((BL_Err_Type (*)(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SFlash_IO_PARM])

#define RomDriver_GLB_Set_SFlash2_IO_PARM \
    ((BL_Err_Type (*)(uint8_t swapIo3Io0)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SFlash2_IO_PARM])

#define RomDriver_GLB_Set_Psram_Pad_For_Flash \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Psram_Pad_For_Flash])

#define RomDriver_GLB_Clr_Reset_Reason \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Clr_Reset_Reason])

#define RomDriver_GLB_Get_Reset_Reason \
    ((BL_Err_Type (*)(GLB_RESET_RECORD_Type *reason)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_Reset_Reason])

#define RomDriver_GLB_AHB_MCU_Software_Reset \
    ((BL_Err_Type (*)(uint8_t swrst)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_AHB_MCU_Software_Reset])

#define RomDriver_GLB_Disrst_Set \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t disrst)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Disrst_Set])

#define RomDriver_GLB_PER_Clock_Gate \
    ((BL_Err_Type (*)(uint64_t ips)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_PER_Clock_Gate])

#define RomDriver_GLB_PER_Clock_UnGate \
    ((BL_Err_Type (*)(uint64_t ips)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_PER_Clock_UnGate])

#define RomDriver_GLB_PLL_CGEN_Clock_Gate \
    ((BL_Err_Type (*)(uint8_t clk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_PLL_CGEN_Clock_Gate])

#define RomDriver_GLB_PLL_CGEN_Clock_UnGate \
    ((BL_Err_Type (*)(uint8_t clk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_PLL_CGEN_Clock_UnGate])

#define RomDriver_GLB_Set_PKA_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PKA_CLK_Sel])

#define RomDriver_GLB_MCU_SW_System_Reset \
    ((BL_Err_Type (*)(uint8_t sysPart)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_MCU_SW_System_Reset])

#define RomDriver_GLB_SW_System_Reset \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SW_System_Reset])

#define RomDriver_GLB_SW_CPU_Reset \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SW_CPU_Reset])

#define RomDriver_GLB_SW_POR_Reset \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SW_POR_Reset])

#define RomDriver_GLB_Set_Auto_Calc_Xtal_Type \
    ((BL_Err_Type (*)(uint8_t calcXtalType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Auto_Calc_Xtal_Type])

#define RomDriver_GLB_Get_Auto_Calc_Xtal_Type \
    ((BL_Err_Type (*)(uint8_t *calcXtalType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_Auto_Calc_Xtal_Type])

#define RomDriver_GLB_Set_Flash_Id_Value \
    ((BL_Err_Type (*)(uint32_t idValue)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Flash_Id_Value])

#define RomDriver_GLB_Get_Flash_Id_Value \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_Flash_Id_Value])

#define RomDriver_GLB_Trim_Ldo18ioVoutSel \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Trim_Ldo18ioVoutSel])

#define RomDriver_GLB_Trim_Ldo18ioVoutTrim \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Trim_Ldo18ioVoutTrim])

#define RomDriver_GLB_Power_Down_Ldo18ioVout \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_Down_Ldo18ioVout])

#define RomDriver_GLB_Power_Up_Ldo18ioVout \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_Up_Ldo18ioVout])

#define RomDriver_GLB_Set_SRAM_RET \
    ((BL_Err_Type (*)(uint32_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SRAM_RET])

#define RomDriver_GLB_Get_SRAM_RET \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_SRAM_RET])

#define RomDriver_GLB_Set_SRAM_SLP \
    ((BL_Err_Type (*)(uint32_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SRAM_SLP])

#define RomDriver_GLB_Get_SRAM_SLP \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_SRAM_SLP])

#define RomDriver_GLB_Set_SRAM_PARM \
    ((BL_Err_Type (*)(uint32_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SRAM_PARM])

#define RomDriver_GLB_Get_SRAM_PARM \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_SRAM_PARM])

#define RomDriver_GLB_Set_EM_Sel \
    ((BL_Err_Type (*)(uint8_t emType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_EM_Sel])

#define RomDriver_GLB_Set_PSRAMB_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PSRAMB_CLK_Sel])

#define RomDriver_GLB_Config_WIFIPLL \
    ((BL_Err_Type (*)(uint8_t xtalType, const GLB_WIFIPLL_Cfg_Type *pllCfgList)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_WIFIPLL])

#define RomDriver_GLB_Simple_Set_MCU_System_CLK \
    ((BL_Err_Type (*)(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Simple_Set_MCU_System_CLK])

#define RomDriver_GLB_Embedded_Flash_Pad_Enable \
    ((BL_Err_Type (*)(uint8_t swapIo2Cs)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Embedded_Flash_Pad_Enable])

#define RomDriver_GLB_SF_Write_Protect_Enable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SF_Write_Protect_Enable])

#define RomDriver_GLB_SF_Write_Protect_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SF_Write_Protect_Disable])

#define RomDriver_GLB_SF_Write_Protect_Set \
    ((BL_Err_Type (*)(uint8_t region, uint8_t cmd, uint32_t address, uint32_t length)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SF_Write_Protect_Set])

#define RomDriver_GLB_Write_SW_Reserved_Register \
    ((void (*)(uint8_t index, uint32_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Write_SW_Reserved_Register])

#define RomDriver_GLB_Read_SW_Reserved_Register \
    ((uint32_t (*)(uint8_t index)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Read_SW_Reserved_Register])

#define RomDriver_GLB_Bus_Decoder_Err_Enable \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Bus_Decoder_Err_Enable])

#define RomDriver_GLB_Bus_Decoder_Err_Disable \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Bus_Decoder_Err_Disable])

#define RomDriver_HBN_Ctrl_Ldo_Aon_Vout_in_HBN \
    ((BL_Err_Type (*)(uint8_t ldo_level, uint8_t ldo_trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Ctrl_Ldo_Aon_Vout_in_HBN])

#define RomDriver_HBN_Level_Select \
    ((BL_Err_Type (*)(uint8_t hbn_level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Level_Select])

#define RomDriver_HBN_Power_Down_Flash \
    ((void (*)(spi_flash_cfg_type *flashCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_Down_Flash])

#define RomDriver_HBN_Reset \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Reset])

#define RomDriver_HBN_Get_POR_OUT_State \
    ((BL_Sts_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_POR_OUT_State])

#define RomDriver_HBN_Get_BOD_OUT_State \
    ((BL_Sts_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_BOD_OUT_State])

#define RomDriver_HBN_Set_BOD_Config \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t threshold, uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_BOD_Config])

#define RomDriver_HBN_Set_BOD_Cfg \
    ((BL_Err_Type (*)(HBN_BOD_CFG_Type *cfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_BOD_Cfg])

#define RomDriver_HBN_Get_Reset_Event \
    ((void (*)(uint8_t *event)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Reset_Event])

#define RomDriver_HBN_Clr_Reset_Event \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clr_Reset_Event])

#define RomDriver_HBN_SW_Set_Ldo_Aon_Vout \
    ((BL_Err_Type (*)(uint8_t ldoLevel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_SW_Set_Ldo_Aon_Vout])

#define RomDriver_HBN_SW_Trim_Ldo_Aon_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_SW_Trim_Ldo_Aon_Vout])

#define RomDriver_HBN_Set_RTC_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_RTC_CLK_Sel])

#define RomDriver_HBN_Get_RTC_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t *clkType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_RTC_CLK_Sel])

#define RomDriver_HBN_Set_Xtal_Divied_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Xtal_Divied_CLK])

#define RomDriver_HBN_32K_Sel \
    ((BL_Err_Type (*)(uint8_t clkType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_32K_Sel])

#define RomDriver_HBN_Set_UART_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_UART_CLK_Sel])

#define RomDriver_HBN_Set_GPIO_INT_DET_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_GPIO_INT_DET_CLK_Sel])

#define RomDriver_HBN_Get_MCU_XCLK_Sel \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_MCU_XCLK_Sel])

#define RomDriver_HBN_Set_MCU_XCLK_Sel \
    ((BL_Err_Type (*)(uint8_t xclk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_MCU_XCLK_Sel])

#define RomDriver_HBN_Get_MCU_Root_CLK_Sel \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_MCU_Root_CLK_Sel])

#define RomDriver_HBN_Set_MCU_Root_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t rootClk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_MCU_Root_CLK_Sel])

#define RomDriver_HBN_Set_HRAM_slp \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_HRAM_slp])

#define RomDriver_HBN_Set_HRAM_Ret \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_HRAM_Ret])

#define RomDriver_HBN_SW_Set_Ldo_Soc_Vout \
    ((BL_Err_Type (*)(uint8_t ldoLevel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_SW_Set_Ldo_Soc_Vout])

#define RomDriver_HBN_SW_Trim_Ldo_Soc_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_SW_Trim_Ldo_Soc_Vout])

#define RomDriver_HBN_Set_Ldo_Soc_Vout_in_Lowpower \
    ((BL_Err_Type (*)(uint8_t ldoLevel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo_Soc_Vout_in_Lowpower])

#define RomDriver_HBN_Trim_Ldo_Soc_Vout_in_Lowpower \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_Ldo_Soc_Vout_in_Lowpower])

#define RomDriver_HBN_Ctrl_Ldo_Soc_Mode_by_HW \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Ctrl_Ldo_Soc_Mode_by_HW])

#define RomDriver_HBN_Set_Ldo_Soc_Mode \
    ((BL_Err_Type (*)(uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo_Soc_Mode])

#define RomDriver_HBN_SW_Set_Ldo_Sys_Vout \
    ((BL_Err_Type (*)(uint8_t ldolevel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_SW_Set_Ldo_Sys_Vout])

#define RomDriver_HBN_SW_Trim_Ldo_Sys_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_SW_Trim_Ldo_Sys_Vout])

#define RomDriver_HBN_Set_Ldo_Sys_Vout_in_Lowpower \
    ((BL_Err_Type (*)(uint8_t ldolevel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo_Sys_Vout_in_Lowpower])

#define RomDriver_HBN_Ctrl_Ldo_Sys_Mode_by_HW \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Ctrl_Ldo_Sys_Mode_by_HW])

#define RomDriver_HBN_Set_Ldo_Sys_Mode \
    ((BL_Err_Type (*)(uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo_Sys_Mode])

#define RomDriver_HBN_Power_Off_LDO18_IO \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_LDO18_IO])

#define RomDriver_HBN_Power_On_LDO18_IO \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_On_LDO18_IO])

#define RomDriver_HBN_Set_Xtal_32K_Inverter_Amplify_Strength \
    ((BL_Err_Type (*)(uint8_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Xtal_32K_Inverter_Amplify_Strength])

#define RomDriver_HBN_Set_Xtal_32K_Regulator \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Xtal_32K_Regulator])

#define RomDriver_HBN_Start_Xtal_32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Start_Xtal_32K])

#define RomDriver_HBN_Clear_Xtal_32K_Fast_Startup \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clear_Xtal_32K_Fast_Startup])

#define RomDriver_HBN_Get_Xtal_32K_Ready_State \
    ((BL_Sts_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Xtal_32K_Ready_State])

#define RomDriver_HBN_Power_On_Xtal_32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_On_Xtal_32K])

#define RomDriver_HBN_Power_Off_Xtal_32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_Xtal_32K])

#define RomDriver_HBN_Keep_On_RC32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Keep_On_RC32K])

#define RomDriver_HBN_Power_Off_RC32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_RC32K])

#define RomDriver_HBN_Trim_RC32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_RC32K])

#define RomDriver_HBN_Trim_Ldo_Soc_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_Ldo_Soc_Vout])

#define RomDriver_HBN_Trim_Ldo_Aon_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_Ldo_Aon_Vout])

#define RomDriver_HBN_Trim_Ldo_Sys_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_Ldo_Sys_Vout])

#define RomDriver_HBN_Set_RC32M_Speed \
    ((BL_Err_Type (*)(uint8_t rc32m_speed)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_RC32M_Speed])

#define RomDriver_HBN_Get_RC32M_Speed \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_RC32M_Speed])

#define RomDriver_HBN_Trim_RC32M \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_RC32M])

#define RomDriver_HBN_Get_Status_Flag \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Status_Flag])

#define RomDriver_HBN_Set_Status_Flag \
    ((BL_Err_Type (*)(uint32_t flag)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Status_Flag])

#define RomDriver_HBN_Get_Wakeup_Addr \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Wakeup_Addr])

#define RomDriver_HBN_Set_Wakeup_Addr \
    ((BL_Err_Type (*)(uint32_t addr)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Wakeup_Addr])

#define RomDriver_HBN_Get_Core_Unhalt_Config \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Core_Unhalt_Config])

#define RomDriver_HBN_Set_Core_Reboot_Config \
    ((BL_Err_Type (*)(uint8_t core, uint8_t hcfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Core_Reboot_Config])

#define RomDriver_HBN_Get_User_Boot_Config \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_User_Boot_Config])

#define RomDriver_HBN_Set_User_Boot_Config \
    ((BL_Err_Type (*)(uint8_t ubCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_User_Boot_Config])

#define RomDriver_HBN_Get_Ldo18io_Power_Delay_Config \
    ((BL_Err_Type (*)(uint16_t *pwrOffDly, uint16_t *pwrOnDly)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Ldo18io_Power_Delay_Config])

#define RomDriver_HBN_Set_Ldo18io_Power_Delay_Config \
    ((BL_Err_Type (*)(uint16_t pwrOffDly, uint16_t pwrOnDly)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo18io_Power_Delay_Config])

#define RomDriver_HBN_Set_Xtal_Type \
    ((BL_Err_Type (*)(uint8_t xtalType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Xtal_Type])

#define RomDriver_HBN_Get_Xtal_Type \
    ((BL_Err_Type (*)(uint8_t *xtalType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Xtal_Type])

#define RomDriver_HBN_Get_Xtal_Value \
    ((BL_Err_Type (*)(uint32_t *xtalVal)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Xtal_Value])

#define RomDriver_HBN_Set_Flash_Power_Delay \
    ((BL_Err_Type (*)(uint8_t flashPwrDly)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Flash_Power_Delay])

#define RomDriver_HBN_Get_Flash_Power_Delay \
    ((BL_Err_Type (*)(uint8_t *flashPwrDly)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Flash_Power_Delay])

#define RomDriver_HBN_Clear_RTC_Counter \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clear_RTC_Counter])

#define RomDriver_HBN_Enable_RTC_Counter \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Enable_RTC_Counter])

#define RomDriver_HBN_Set_RTC_Timer \
    ((BL_Err_Type (*)(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_RTC_Timer])

#define RomDriver_HBN_Get_RTC_Timer_Val \
    ((BL_Err_Type (*)(uint32_t *valLow, uint32_t *valHigh)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_RTC_Timer_Val])

#define RomDriver_HBN_Clear_RTC_INT \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clear_RTC_INT])

#define RomDriver_HBN_GPIO_INT_Enable \
    ((BL_Err_Type (*)(uint8_t gpioIntTrigType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_INT_Enable])

#define RomDriver_HBN_GPIO_INT_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_INT_Disable])

#define RomDriver_HBN_Get_INT_State \
    ((BL_Sts_Type (*)(uint8_t irqType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_INT_State])

#define RomDriver_HBN_Get_Pin_Wakeup_Mode \
    ((uint8_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode])

#define RomDriver_HBN_Clear_IRQ \
    ((BL_Err_Type (*)(uint8_t irqType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clear_IRQ])

#define RomDriver_HBN_Hw_Pu_Pd_Cfg \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg])

#define RomDriver_HBN_Set_Aon_Pad_IntMode \
    ((BL_Err_Type (*)(uint8_t gpioIntTrigType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Aon_Pad_IntMode])

#define RomDriver_HBN_Pin_WakeUp_Mask \
    ((BL_Err_Type (*)(uint8_t maskVal)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Pin_WakeUp_Mask])

#define RomDriver_HBN_Set_Aon_Pad_IntMask \
    ((BL_Err_Type (*)(uint8_t pad, BL_Mask_Type intMask)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Aon_Pad_IntMask])

#define RomDriver_HBN_Aon_Pad_Ctrl \
    ((BL_Err_Type (*)(uint32_t aonPadCtl1, uint32_t aonPadCtl2)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_Ctrl])

#define RomDriver_HBN_Aon_Pad_Cfg \
    ((BL_Err_Type (*)(uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_Cfg])

#define RomDriver_HBN_Aon_Pad_Cfg_Set \
    ((BL_Err_Type (*)(uint8_t aonPadHwCtrlEn, uint8_t aonGpio)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_Cfg_Set])

#define RomDriver_HBN_Enable_BOD_IRQ \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Enable_BOD_IRQ])

#define RomDriver_HBN_Disable_BOD_IRQ \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Disable_BOD_IRQ])

#define RomDriver_HBN_Aon_Pad_WakeUpCfg \
    ((BL_Err_Type (*)(BL_Fun_Type puPdEn, uint8_t trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_WakeUpCfg])

#define RomDriver_HBN_Wdg_Init \
    ((void (*)(uint8_t mode, uint16_t comp_val)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Init])

#define RomDriver_HBN_Wdg_Start \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Start])

#define RomDriver_HBN_Wdg_Stop \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Stop])

#define RomDriver_HBN_Wdg_Get_Countervalue \
    ((uint16_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Get_Countervalue])

#define RomDriver_HBN_Wdg_Set_Countervalue \
    ((void (*)(uint16_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Set_Countervalue])

#define RomDriver_HBN_Wdg_Reset_Countervalue \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Reset_Countervalue])

#define RomDriver_HBN_Wdg_Compint_Clear \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Wdg_Compint_Clear])

#define RomDriver_HBN_Get_Version \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Version])

#define RomDriver_HBN_Set_Version \
    ((void (*)(uint32_t version)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Version])

#define RomDriver_PDS_Set_GPIO_Pad_IntMask \
    ((BL_Err_Type (*)(uint8_t pad, BL_Mask_Type intMask)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask])

#define RomDriver_PDS_Set_GPIO_Pad_IntMode \
    ((BL_Err_Type (*)(uint8_t pad, uint8_t trig)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode])

#define RomDriver_PDS_Set_GPIO_Pad_IntClr \
    ((BL_Err_Type (*)(uint8_t pad)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr])

#define RomDriver_PDS_Get_GPIO_Pad_IntStatus \
    ((BL_Sts_Type (*)(uint8_t pad)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus])

#define RomDriver_PDS_Set_All_GPIO_Pad_IntClr \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_GPIO_Pad_IntClr])

#define RomDriver_PDS_Get_All_GPIO_Pad_IntStatus \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_All_GPIO_Pad_IntStatus])

#define RomDriver_PDS_Disable_ALL_GPIO_Keep \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Disable_ALL_GPIO_Keep])

#define RomDriver_PDS_Enable_GPIO_Keep \
    ((BL_Err_Type (*)(uint32_t pin)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Enable_GPIO_Keep])

#define RomDriver_PDS_Set_PDS_GPIO_INT_DET_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_PDS_GPIO_INT_DET_CLK_Sel])

#define RomDriver_PDS_Enable_WRAM_Access_Direct \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Enable_WRAM_Access_Direct])

#define RomDriver_PDS_Disable_WRAM_Access_Direct \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Disable_WRAM_Access_Direct])

#define RomDriver_PDS_Set_Ldo_Soc_Enter_lp_delay \
    ((BL_Err_Type (*)(uint16_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Ldo_Soc_Enter_lp_delay])

#define RomDriver_PDS_Set_Ldo_Soc_Exit_lp_delay \
    ((BL_Err_Type (*)(uint16_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Ldo_Soc_Exit_lp_delay])

#define RomDriver_PDS_Set_Ldo_Sys_Enter_lp_delay \
    ((BL_Err_Type (*)(uint16_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Ldo_Sys_Enter_lp_delay])

#define RomDriver_PDS_Set_Ldo_Sys_Exit_lp_delay \
    ((BL_Err_Type (*)(uint16_t cnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Ldo_Sys_Exit_lp_delay])

#define RomDriver_PDS_Set_MCU0_Clock_Enable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU0_Clock_Enable])

#define RomDriver_PDS_Set_MCU0_Clock_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU0_Clock_Disable])

#define RomDriver_PDS_Pu_PLL_Enable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Pu_PLL_Enable])

#define RomDriver_PDS_Pu_PLL_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Pu_PLL_Disable])

#define RomDriver_PDS_Enable \
    ((BL_Err_Type (*)(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Enable])

#define RomDriver_PDS_Force_Config \
    ((BL_Err_Type (*)(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5, PDS_CTL6_Type *cfg6)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Force_Config])

#define RomDriver_PDS_RAM_Config \
    ((BL_Err_Type (*)(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg, PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_RAM_Config])

#define RomDriver_PDS_Set_All_WRAM_Retention \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_WRAM_Retention])

#define RomDriver_PDS_Set_All_OCRAM_Retention \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_OCRAM_Retention])

#define RomDriver_PDS_Set_All_WRAM_Sleep \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_WRAM_Sleep])

#define RomDriver_PDS_Set_All_OCRAM_Sleep \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_OCRAM_Sleep])

#define RomDriver_PDS_Disable_Ctrl_Ram \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Disable_Ctrl_Ram])

#define RomDriver_PDS_Default_Level_Config \
    ((BL_Err_Type (*)(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Default_Level_Config])

#define RomDriver_PDS_IntMask \
    ((BL_Err_Type (*)(uint8_t intType, BL_Mask_Type intMask)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_IntMask])

#define RomDriver_PDS_Get_IntStatus \
    ((BL_Sts_Type (*)(uint8_t intType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_IntStatus])

#define RomDriver_PDS_IntClear \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_IntClear])

#define RomDriver_PDS_Set_Wakeup_Src_IntMask \
    ((BL_Err_Type (*)(uint32_t WakeupType, BL_Mask_Type intMask)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Wakeup_Src_IntMask])

#define RomDriver_PDS_Get_Wakeup_Src \
    ((BL_Sts_Type (*)(uint32_t WakeupType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_Wakeup_Src])

#define RomDriver_PDS_Get_All_Wakeup_Src \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_All_Wakeup_Src])

#define RomDriver_PDS_Mask_All_Wakeup_Src \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Mask_All_Wakeup_Src])

#define RomDriver_PDS_Power_Off_WB \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Power_Off_WB])

#define RomDriver_PDS_Power_On_WB \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Power_On_WB])

#define RomDriver_PDS_Set_USB_Suspend \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_USB_Suspend])

#define RomDriver_PDS_Set_USB_Resume \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_USB_Resume])

#define RomDriver_PDS_Trim_USB20 \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Trim_USB20])


/*@} end of group ROMDRIVER_Public_Macros */

/** @defgroup  ROMDRIVER_Public_Functions
 *  @{
 */

/*@} end of group ROMDRIVER_Public_Functions */

/*@} end of group ROMDRIVER */

/*@} end of group bl616_Peripheral_Driver */

#endif /* __bl616_ROMDRIVER_H__ */
