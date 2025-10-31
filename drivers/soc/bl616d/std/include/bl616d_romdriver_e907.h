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
#include <compiler/compiler_gcc.h>
#include "bl616d_aon.h"
#include "bl616d_clock.h"
#include "bl616d_common.h"
#include "bl616d_ef_cfg.h"
#include "bl616d_glb.h"
#include "bl616d_hbn.h"
#include "bl616d_pds.h"
#include "bl616d_psram.h"
#include "bl616d_sec_dbg.h"
#include "bl616d_tzc_sec.h"
#include "bl616d_clock.h"

#include "bflb_clock.h"
#include "bflb_peri.h"
#include "bflb_efuse.h"

/** @addtogroup  bl616d_Peripheral_Driver
 *  @{
 */

/** @addtogroup  ROMDRIVER
 *  @{
 */

/** @defgroup  ROMDRIVER_Public_Types
 *  @{
 */


#define ROM_API_INDEX_MAX   (512)

typedef enum {
    ROM_API_INDEX_VERSION      = 0,
    ROM_API_INDEX_RSVD_0       = 1,
    ROM_API_INDEX_RSVD_1       = 2,
    ROM_API_INDEX_RSVD_LAST    = 3,

    ROM_API_INDEX_bflb_clk_get_system_clock                         = 4,
    ROM_API_INDEX_bflb_clk_get_peripheral_clock                     = 5,
    ROM_API_INDEX_bflb_peripheral_clock_get_by_id                   = 6,
    ROM_API_INDEX_bflb_peripheral_clock_control_by_id               = 7,
    ROM_API_INDEX_bflb_peripheral_clock_status_get_by_id            = 8,
    ROM_API_INDEX_AON_Power_On_MBG                                  = 9,
    ROM_API_INDEX_AON_Power_Off_MBG                                 = 10,
    ROM_API_INDEX_AON_Power_On_SFReg                                = 11,
    ROM_API_INDEX_AON_Power_Off_SFReg                               = 12,
    ROM_API_INDEX_AON_Power_On_RF                                   = 13,
    ROM_API_INDEX_AON_Power_Off_RF                                  = 14,
    ROM_API_INDEX_AON_Power_On_XTAL                                 = 15,
    ROM_API_INDEX_AON_Set_Xtal_CapCode                              = 16,
    ROM_API_INDEX_AON_Get_Xtal_CapCode                              = 17,
    ROM_API_INDEX_AON_Power_Off_XTAL                                = 18,
    ROM_API_INDEX_AON_Power_On_BG                                   = 19,
    ROM_API_INDEX_AON_Power_Off_BG                                  = 20,
    ROM_API_INDEX_AON_Power_On_LDO15_RF                             = 21,
    ROM_API_INDEX_AON_Power_Off_LDO15_RF                            = 22,
    ROM_API_INDEX_AON_Output_Float_LDO15_RF                         = 23,
    ROM_API_INDEX_AON_Output_Pulldown_LDO15_RF                      = 24,
    ROM_API_INDEX_AON_Output_LDO15_RF_Ctrl_by_Pd                    = 25,
    ROM_API_INDEX_AON_Set_Xtal_32K_Inverter_Amplify_Strength        = 26,
    ROM_API_INDEX_AON_Set_Xtal_32K_Regulator                        = 27,
    ROM_API_INDEX_AON_Start_Xtal_32K                                = 28,
    ROM_API_INDEX_AON_Clear_Xtal_32K_Fast_Startup                   = 29,
    ROM_API_INDEX_AON_Get_Xtal_32K_Ready_State                      = 30,
    ROM_API_INDEX_AON_Power_On_Xtal_32K                             = 31,
    ROM_API_INDEX_AON_Power_Off_Xtal_32K                            = 32,
    ROM_API_INDEX_AON_Set_Ldo18_Aon_Vout                            = 33,
    ROM_API_INDEX_AON_SW_Trim_Ldo18_Aon_Vout                        = 34,
    ROM_API_INDEX_AON_Set_Ldo08_Aon_Vout                            = 35,
    ROM_API_INDEX_AON_SW_Trim_Ldo08_Aon_Vout                        = 36,
    ROM_API_INDEX_AON_Set_Ldo_Soc_Vout                              = 37,
    ROM_API_INDEX_AON_SW_Trim_Ldo_Soc_Vout                          = 38,
    ROM_API_INDEX_AON_Set_Switch_For_Efuse                          = 39,
    ROM_API_INDEX_AON_Set_Ldo18_AON_Power_Switch_For_FLASH          = 40,
    ROM_API_INDEX_AON_Set_Ldo18_AON_Power_Switch_For_PSRAM          = 41,
    ROM_API_INDEX_AON_Set_Dcdc_Sys_Vout                             = 42,
    ROM_API_INDEX_AON_SW_Trim_Dcdc_Sys_Vout                         = 43,
    ROM_API_INDEX_AON_Set_Ldo18_Aon_Vout_in_Lowpower                = 44,
    ROM_API_INDEX_AON_Set_Dcdc_Sys_Vout_in_Lowpower                 = 45,
    ROM_API_INDEX_AON_Set_Ldo_Soc_Vout_in_Lowpower                  = 46,
    ROM_API_INDEX_AON_Ctrl_Ldo18_Aon_Mode_by_HW                     = 47,
    ROM_API_INDEX_AON_Ctrl_Dcdc_Sys_Mode_by_HW                      = 48,
    ROM_API_INDEX_AON_Ctrl_Ldo_Soc_Mode_by_HW                       = 49,
    ROM_API_INDEX_AON_Set_Ldo18_Aon_Mode                            = 50,
    ROM_API_INDEX_AON_Set_Dcdc_Sys_Mode                             = 51,
    ROM_API_INDEX_AON_Set_Ldo_Soc_Mode                              = 52,
    ROM_API_INDEX_AON_Set_Dcdc_Sys_Sstart_Delay                     = 53,
    ROM_API_INDEX_AON_Get_BOD_OUT_State                             = 54,
    ROM_API_INDEX_AON_Set_BOD_Config                                = 55,
    ROM_API_INDEX_AON_Wdg_Init                                      = 56,
    ROM_API_INDEX_AON_Wdg_Start                                     = 57,
    ROM_API_INDEX_AON_Wdg_Status_Get                                = 58,
    ROM_API_INDEX_AON_Wdg_Status_Clear                              = 59,
    ROM_API_INDEX_AON_Wdg_Kick                                      = 60,
    ROM_API_INDEX_AON_Set_RC32M_Speed_As_8M                         = 61,
    ROM_API_INDEX_AON_Set_RC32M_Code_Fr_Ext                         = 62,
    ROM_API_INDEX_AON_Trim_Ldo_Soc_Vout                             = 63,
    ROM_API_INDEX_AON_Trim_Ldo08_Aon_Vout                           = 64,
    ROM_API_INDEX_AON_Trim_Ldo18_Aon_Vout                           = 65,
    ROM_API_INDEX_HBN_Trim_Dcdc12_Sys_Vout                          = 66,
    ROM_API_INDEX_Clock_System_Clock_Get                            = 67,
    ROM_API_INDEX_Clock_Peripheral_Clock_Get                        = 68,
    ROM_API_INDEX_SystemCoreClockGet                                = 69,
    ROM_API_INDEX_CPU_Set_MTimer_RST                                = 70,
    ROM_API_INDEX_CPU_Reset_MTimer                                  = 71,
    ROM_API_INDEX_CPU_Set_MTimer_CLK                                = 72,
    ROM_API_INDEX_CPU_Get_MTimer_Source_Clock                       = 73,
    ROM_API_INDEX_CPU_Get_MTimer_Clock                              = 74,
    ROM_API_INDEX_CPU_Get_MTimer_Counter                            = 75,
    ROM_API_INDEX_CPU_Get_CPU_Cycle                                 = 76,
    ROM_API_INDEX_CPU_Get_MTimer_US                                 = 77,
    ROM_API_INDEX_CPU_Get_MTimer_MS                                 = 78,
    ROM_API_INDEX_CPU_MTimer_Delay_US                               = 79,
    ROM_API_INDEX_CPU_MTimer_Delay_MS                               = 80,
    ROM_API_INDEX_ASM_Delay_Us                                      = 81,
    ROM_API_INDEX_arch_delay_us                                     = 82,
    ROM_API_INDEX_arch_delay_ms                                     = 83,
    ROM_API_INDEX_bflb_ef_ctrl_get_common_trim_list                 = 84,
    ROM_API_INDEX_bflb_efuse_get_device_info                        = 85,
    ROM_API_INDEX_bflb_efuse_read_mac_address                       = 86,
    ROM_API_INDEX_bflb_efuse_get_chipid                             = 87,
    ROM_API_INDEX_bflb_efuse_get_adc_trim                           = 88,
    ROM_API_INDEX_bflb_efuse_get_adc_tsen_trim                      = 89,
    ROM_API_INDEX_bflb_efuse_read_secure_boot                       = 90,
    ROM_API_INDEX_bflb_efuse_enable_aes                             = 91,
    ROM_API_INDEX_bflb_efuse_rw_lock_aes_key                        = 92,
    ROM_API_INDEX_bflb_efuse_rw_lock_dbg_key                        = 93,
    ROM_API_INDEX_bflb_efuse_write_lock_pk_hash                     = 94,
    ROM_API_INDEX_bflb_efuse_write_lock_usb_pid_vid                 = 95,
    ROM_API_INDEX_GLB_Power_On_XTAL_And_PLL_CLK                     = 96,
    ROM_API_INDEX_GLB_Power_Off_WIFIPLL                             = 97,
    ROM_API_INDEX_GLB_WIFIPLL_Ref_Clk_Sel                           = 98,
    ROM_API_INDEX_GLB_Power_On_WIFIPLL                              = 99,
    ROM_API_INDEX_GLB_Power_Off_CPUPLL                              = 100,
    ROM_API_INDEX_GLB_CPUPLL_Ref_Clk_Sel                            = 101,
    ROM_API_INDEX_GLB_Power_On_CPUPLL                               = 102,
    ROM_API_INDEX_GLB_Set_CPUPLL_PostOut                            = 103,
    ROM_API_INDEX_GLB_Set_USB_CLK_From_WIFIPLL                      = 104,
    ROM_API_INDEX_GLB_Set_SSC_CLK_From_WIFIPLL                      = 105,
    ROM_API_INDEX_GLB_Set_MCU_System_CLK_Div                        = 106,
    ROM_API_INDEX_GLB_Get_MCU_System_CLK_Div                        = 107,
    ROM_API_INDEX_GLB_Set_MCU_System_CLK                            = 108,
    ROM_API_INDEX_GLB_Set_WL_MCU_System_CLK                         = 109,
    ROM_API_INDEX_GLB_Set_WL_XCLK_Sel                               = 110,
    ROM_API_INDEX_GLB_Get_Core_Type                                 = 111,
    ROM_API_INDEX_GLB_Set_CPU_Reset_Address                         = 112,
    ROM_API_INDEX_GLB_Clr_EMI_Reset_Gate                            = 113,
    ROM_API_INDEX_GLB_Set_ADC_CLK                                   = 114,
    ROM_API_INDEX_GLB_Set_DMA_CLK                                   = 115,
    ROM_API_INDEX_GLB_Set_Peripheral_DMA_CN                         = 116,
    ROM_API_INDEX_GLB_Set_IR_CLK                                    = 117,
    ROM_API_INDEX_GLB_IR_RX_GPIO_Sel                                = 118,
    ROM_API_INDEX_GLB_Set_UART_CLK                                  = 119,
    ROM_API_INDEX_GLB_UART_Fun_Sel                                  = 120,
    ROM_API_INDEX_GLB_Set_SF_CLK                                    = 121,
    ROM_API_INDEX_GLB_Set_I2C_CLK                                   = 122,
    ROM_API_INDEX_GLB_Set_I2S_CLK                                   = 123,
    ROM_API_INDEX_GLB_Set_SPI_CLK                                   = 124,
    ROM_API_INDEX_GLB_SPI_Sig_Swap_Set                              = 125,
    ROM_API_INDEX_GLB_Set_PEC_CLK                                   = 126,
    ROM_API_INDEX_GLB_Set_PWM1_IO_Sel                               = 127,
    ROM_API_INDEX_GLB_Set_PDM_IO_Sel                                = 128,
    ROM_API_INDEX_GLB_Set_DBI_CLK                                   = 129,
    ROM_API_INDEX_GLB_Set_DIG_CLK_Sel                               = 130,
    ROM_API_INDEX_GLB_Set_DIG_512K_CLK                              = 131,
    ROM_API_INDEX_GLB_Set_DIG_32K_CLK                               = 132,
    ROM_API_INDEX_GLB_Platform_Wakeup_PDS_Enable                    = 133,
    ROM_API_INDEX_GLB_Sel_MCU_TMR_GPIO_Clock                        = 134,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out0_Sel                       = 135,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out1_Sel                       = 136,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out2_Sel                       = 137,
    ROM_API_INDEX_GLB_Set_Chip_Clock_Out3_Sel                       = 138,
    ROM_API_INDEX_GLB_Set_Audio_AUTO_CLK                            = 139,
    ROM_API_INDEX_GLB_Set_Audio_ADC_CLK                             = 140,
    ROM_API_INDEX_GLB_Set_Audio_SOLO_CLK                            = 141,
    ROM_API_INDEX_GLB_Invert_ETH_RX_CLK                             = 142,
    ROM_API_INDEX_GLB_Invert_ETH_TX_CLK                             = 143,
    ROM_API_INDEX_GLB_Invert_ETH_REF_O_CLK                          = 144,
    ROM_API_INDEX_GLB_Set_ETH_REF_O_CLK_Sel                         = 145,
    ROM_API_INDEX_GLB_Set_CAM_CLK                                   = 146,
    ROM_API_INDEX_GLB_Set_SDH_CLK                                   = 147,
    ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_System                 = 148,
    ROM_API_INDEX_GLB_UART_Sig_Swap_Set                             = 149,
    ROM_API_INDEX_GLB_Swap_MCU_SPI_0_MOSI_With_MISO                 = 150,
    ROM_API_INDEX_GLB_Set_MCU_SPI_0_ACT_MOD_Sel                     = 151,
    ROM_API_INDEX_GLB_Set_SFlash_IO_PARM                            = 152,
    ROM_API_INDEX_GLB_Set_SFlash2_IO_PARM                           = 153,
    ROM_API_INDEX_GLB_Clr_Reset_Reason                              = 154,
    ROM_API_INDEX_GLB_Get_Reset_Reason                              = 155,
    ROM_API_INDEX_GLB_AHB_MCU_Software_Reset                        = 156,
    ROM_API_INDEX_GLB_Disrst_Set                                    = 157,
    ROM_API_INDEX_GLB_PER_Clock_Gate                                = 158,
    ROM_API_INDEX_GLB_PER_Clock_UnGate                              = 159,
    ROM_API_INDEX_GLB_PLL_CGEN_Clock_Gate                           = 160,
    ROM_API_INDEX_GLB_PLL_CGEN_Clock_UnGate                         = 161,
    ROM_API_INDEX_GLB_Set_PKA_CLK_Sel                               = 162,
    ROM_API_INDEX_GLB_MCU_SW_System_Reset                           = 163,
    ROM_API_INDEX_GLB_SW_System_Reset                               = 164,
    ROM_API_INDEX_GLB_SW_CPU_Reset                                  = 165,
    ROM_API_INDEX_GLB_SW_POR_Reset                                  = 166,
    ROM_API_INDEX_GLB_Set_Auto_Calc_Xtal_Type                       = 167,
    ROM_API_INDEX_GLB_Get_Auto_Calc_Xtal_Type                       = 168,
    ROM_API_INDEX_GLB_Set_Flash_Id_Value                            = 169,
    ROM_API_INDEX_GLB_Get_Flash_Id_Value                            = 170,
    ROM_API_INDEX_GLB_Trim_Ldo18ioVoutSel                           = 171,
    ROM_API_INDEX_GLB_Trim_Ldo18ioBypass                            = 172,
    ROM_API_INDEX_GLB_Trim_Ldo18ioVoutTrim                          = 173,
    ROM_API_INDEX_GLB_Power_Down_Ldo18ioVout                        = 174,
    ROM_API_INDEX_GLB_Set_SRAM_RET                                  = 175,
    ROM_API_INDEX_GLB_Get_SRAM_RET                                  = 176,
    ROM_API_INDEX_GLB_Set_SRAM_SLP                                  = 177,
    ROM_API_INDEX_GLB_Get_SRAM_SLP                                  = 178,
    ROM_API_INDEX_GLB_Set_SRAM_PARM                                 = 179,
    ROM_API_INDEX_GLB_Get_SRAM_PARM                                 = 180,
    ROM_API_INDEX_GLB_Set_EM_Sel                                    = 181,
    ROM_API_INDEX_GLB_Set_PSRAMB_CLK_Sel                            = 182,
    ROM_API_INDEX_GLB_Set_Slave_Grp_0_CLK                           = 183,
    ROM_API_INDEX_GLB_Config_WIFIPLL                                = 184,
    ROM_API_INDEX_GLB_Config_CPUPLL                                 = 185,
    ROM_API_INDEX_GLB_Simple_Set_MCU_System_CLK                     = 186,
    ROM_API_INDEX_GLB_Set_Flash_Xip                                 = 187,
    ROM_API_INDEX_GLB_Halt_Mini_Sys                                 = 188,
    ROM_API_INDEX_GLB_Release_Mini_Sys                              = 189,
    ROM_API_INDEX_GLB_MINI_PER_Software_Reset                       = 190,
    ROM_API_INDEX_GLB_Set_SPI1_CLK                                  = 191,
    ROM_API_INDEX_GLB_Set_MINI_FCLK                                 = 192,
    ROM_API_INDEX_GLB_MINI_PER_Clock_Gate                           = 193,
    ROM_API_INDEX_GLB_MINI_PER_Clock_UnGate                         = 194,
    ROM_API_INDEX_GLB_Select_LPCPU_Jtag                             = 195,
    ROM_API_INDEX_GLB_Halt_CPU                                      = 196,
    ROM_API_INDEX_GLB_Release_CPU                                   = 197,
    ROM_API_INDEX_GLB_RC32M_Force_On                                = 198,
    ROM_API_INDEX_GLB_RC32M_Force_Off                               = 199,
    ROM_API_INDEX_GLB_Get_NP2MINI_Interrupt_Status                  = 200,
    ROM_API_INDEX_GLB_Set_NP2MINI_Interrupt_Clear                   = 201,
    ROM_API_INDEX_GLB_Set_NP2MINI_Interrupt_Mask                    = 202,
    ROM_API_INDEX_GLB_Get_NP2MINI_Interrupt_Mask                    = 203,
    ROM_API_INDEX_GLB_Embedded_Flash_Pad_Enable                     = 204,
    ROM_API_INDEX_HBN_Power_Down_Flash                              = 205,
    ROM_API_INDEX_HBN_Level_Select                                  = 206,
    ROM_API_INDEX_HBN_Reset                                         = 207,
    ROM_API_INDEX_HBN_Set_GPADC_CLK_Sel                             = 208,
    ROM_API_INDEX_HBN_PIR_Enable                                    = 209,
    ROM_API_INDEX_HBN_PIR_Disable                                   = 210,
    ROM_API_INDEX_HBN_PIR_INT_Config                                = 211,
    ROM_API_INDEX_HBN_PIR_LPF_Sel                                   = 212,
    ROM_API_INDEX_HBN_PIR_HPF_Sel                                   = 213,
    ROM_API_INDEX_HBN_Set_PIR_Threshold                             = 214,
    ROM_API_INDEX_HBN_Get_PIR_Threshold                             = 215,
    ROM_API_INDEX_HBN_Set_PIR_Interval                              = 216,
    ROM_API_INDEX_HBN_Get_PIR_Interval                              = 217,
    ROM_API_INDEX_HBN_32K_Sel                                       = 218,
    ROM_API_INDEX_HBN_Set_UART_CLK_Sel                              = 219,
    ROM_API_INDEX_HBN_Get_MCU_XCLK_Sel                              = 220,
    ROM_API_INDEX_HBN_Set_MCU_XCLK_Sel                              = 221,
    ROM_API_INDEX_HBN_Get_MCU_Root_CLK_Sel                          = 222,
    ROM_API_INDEX_HBN_Set_MCU_Root_CLK_Sel                          = 223,
    ROM_API_INDEX_HBN_Set_HRAM_slp                                  = 224,
    ROM_API_INDEX_HBN_Set_HRAM_Ret                                  = 225,
    ROM_API_INDEX_HBN_Keep_On_RC32K                                 = 226,
    ROM_API_INDEX_HBN_Power_Off_RC32K                               = 227,
    ROM_API_INDEX_HBN_Trim_RC32K                                    = 228,
    ROM_API_INDEX_HBN_Get_Status_Flag                               = 229,
    ROM_API_INDEX_HBN_Set_Status_Flag                               = 230,
    ROM_API_INDEX_HBN_Get_Wakeup_Addr                               = 231,
    ROM_API_INDEX_HBN_Set_Wakeup_Addr                               = 232,
    ROM_API_INDEX_HBN_Get_Core_Unhalt_Config                        = 233,
    ROM_API_INDEX_HBN_Set_Core_Reboot_Config                        = 234,
    ROM_API_INDEX_HBN_Get_User_Boot_Config                          = 235,
    ROM_API_INDEX_HBN_Set_User_Boot_Config                          = 236,
    ROM_API_INDEX_HBN_Get_Ldo18io_Power_Delay_Config                = 237,
    ROM_API_INDEX_HBN_Set_Ldo18io_Power_Delay_Config                = 238,
    ROM_API_INDEX_HBN_Set_Xtal_Type                                 = 239,
    ROM_API_INDEX_HBN_Get_Xtal_Type                                 = 240,
    ROM_API_INDEX_HBN_Get_Xtal_Value                                = 241,
    ROM_API_INDEX_HBN_Set_Flash_Power_Delay                         = 242,
    ROM_API_INDEX_HBN_Get_Flash_Power_Delay                         = 243,
    ROM_API_INDEX_HBN_Set_Gpio_Keep                                 = 244,
    ROM_API_INDEX_HBN_Get_Gpio_Keep                                 = 245,
    ROM_API_INDEX_HBN_Clear_Gpio_Keep                               = 246,
    ROM_API_INDEX_HBN_Set_PDS_Gpio_Keep                             = 247,
    ROM_API_INDEX_HBN_Get_PDS_Gpio_Keep                             = 248,
    ROM_API_INDEX_HBN_Clear_PDS_Gpio_Keep                           = 249,
    ROM_API_INDEX_HBN_Disable_Gpio_Keep                             = 250,
    ROM_API_INDEX_HBN_Clear_RTC_Counter                             = 251,
    ROM_API_INDEX_HBN_Enable_RTC_Counter                            = 252,
    ROM_API_INDEX_HBN_Set_RTC_Timer                                 = 253,
    ROM_API_INDEX_HBN_Get_RTC_Timer_Val                             = 254,
    ROM_API_INDEX_HBN_Recal_RC32K                                   = 255,
    ROM_API_INDEX_HBN_Clear_RTC_INT                                 = 256,
    ROM_API_INDEX_HBN_GPIO_INT_Enable                               = 257,
    ROM_API_INDEX_HBN_GPIO_INT_Disable                              = 258,
    ROM_API_INDEX_HBN_Get_INT_State                                 = 259,
    ROM_API_INDEX_HBN_Get_All_INT_State                             = 260,
    ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode                           = 261,
    ROM_API_INDEX_HBN_Clear_IRQ                                     = 262,
    ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg                                  = 263,
    ROM_API_INDEX_HBN_Set_Aon_Pad_IntMode                           = 264,
    ROM_API_INDEX_HBN_Pin_WakeUp_Mask                               = 265,
    ROM_API_INDEX_HBN_Set_Aon_Pad_IntMask                           = 266,
    ROM_API_INDEX_HBN_Aon_Pad_Ctrl                                  = 267,
    ROM_API_INDEX_HBN_Aon_Pad_Cfg                                   = 268,
    ROM_API_INDEX_HBN_Aon_Pad_Cfg_Set                               = 269,
    ROM_API_INDEX_HBN_Enable_AComp_IRQ                              = 270,
    ROM_API_INDEX_HBN_Disable_AComp_IRQ                             = 271,
    ROM_API_INDEX_HBN_Enable_BOD_IRQ                                = 272,
    ROM_API_INDEX_HBN_Disable_BOD_IRQ                               = 273,
    ROM_API_INDEX_HBN_Aon_Pad_WakeUpCfg                             = 274,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie                      = 275,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask                          = 276,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode                          = 277,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr                           = 278,
    ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus                        = 279,
    ROM_API_INDEX_PDS_Set_All_GPIO_Pad_IntClr                       = 280,
    ROM_API_INDEX_PDS_Get_All_GPIO_Pad_IntStatus                    = 281,
    ROM_API_INDEX_PDS_Set_Flash_Pad_Pull_None                       = 282,
    ROM_API_INDEX_PDS_Set_Flash_Pad_Pull_None_Fast                  = 283,
    ROM_API_INDEX_PDS_Disable_GPIO_Keep                             = 284,
    ROM_API_INDEX_PDS_Set_MCU0_Clock_Enable                         = 285,
    ROM_API_INDEX_PDS_Set_MCU0_Clock_Disable                        = 286,
    ROM_API_INDEX_PDS_Set_MCU1_Clock_Enable                         = 287,
    ROM_API_INDEX_PDS_Set_MCU1_Clock_Disable                        = 288,
    ROM_API_INDEX_PDS_Set_MCU0_Reset_Address                        = 289,
    ROM_API_INDEX_PDS_Pu_PLL_Enable                                 = 290,
    ROM_API_INDEX_PDS_Pu_PLL_Disable                                = 291,
    ROM_API_INDEX_PDS_Enable                                        = 292,
    ROM_API_INDEX_PDS_Force_Config                                  = 293,
    ROM_API_INDEX_PDS_RAM_Config                                    = 294,
    ROM_API_INDEX_PDS_Set_All_WRAM_Retention                        = 295,
    ROM_API_INDEX_PDS_Set_All_OCRAM_Retention                       = 296,
    ROM_API_INDEX_PDS_Set_All_WRAM_Sleep                            = 297,
    ROM_API_INDEX_PDS_Set_All_OCRAM_Sleep                           = 298,
    ROM_API_INDEX_PDS_Disable_Ctrl_Ram                              = 299,
    ROM_API_INDEX_PDS_Default_Level_Config                          = 300,
    ROM_API_INDEX_PDS_IntMask                                       = 301,
    ROM_API_INDEX_PDS_Get_IntStatus                                 = 302,
    ROM_API_INDEX_PDS_IntClear                                      = 303,
    ROM_API_INDEX_PDS_Set_Wakeup_Src_IntMask                        = 304,
    ROM_API_INDEX_PDS_Get_Wakeup_Src                                = 305,
    ROM_API_INDEX_PDS_Get_All_Wakeup_Src                            = 306,
    ROM_API_INDEX_PDS_Mask_All_Wakeup_Src                           = 307,
    ROM_API_INDEX_PDS_Triger_SW_Wakeup_PDS                          = 308,
    ROM_API_INDEX_PDS_Clear_SW_Wakeup_PDS                           = 309,
    ROM_API_INDEX_PDS_Trim_RC32M                                    = 310,
    ROM_API_INDEX_PDS_Trim_USB20                                    = 311,
    ROM_API_INDEX_PDS_Power_Off_WB                                  = 312,
    ROM_API_INDEX_PDS_Power_On_WB                                   = 313,
    ROM_API_INDEX_PSram_Ctrl_Init                                   = 314,
    ROM_API_INDEX_PSram_Ctrl_Winbond_Read_Reg                       = 315,
    ROM_API_INDEX_PSram_Ctrl_Winbond_Write_Reg                      = 316,
    ROM_API_INDEX_PSram_Ctrl_ApMem_Read_Reg                         = 317,
    ROM_API_INDEX_PSram_Ctrl_ApMem_Write_Reg                        = 318,
    ROM_API_INDEX_PSram_Ctrl_ApMem_Reset                            = 319,
    ROM_API_INDEX_PSram_Ctrl_Winbond_Reset                          = 320,
    ROM_API_INDEX_PSram_Ctrl_CK_Sel                                 = 321,
    ROM_API_INDEX_PSram_Ctrl_Get_Timeout_Flag                       = 322,
    ROM_API_INDEX_PSram_Ctrl_Clear_Timout_Flag                      = 323,
    ROM_API_INDEX_PSram_Ctrl_Debug_Timout                           = 324,
    ROM_API_INDEX_Sec_Dbg_Read_Chip_ID                              = 325,
    ROM_API_INDEX_Sec_Dbg_Set_Dbg_Pwd                               = 326,
    ROM_API_INDEX_Sec_Dbg_Read_Dbg_Mode                             = 327,
    ROM_API_INDEX_Sec_Dbg_Read_Dbg_Enable                           = 328,
    ROM_API_INDEX_Sec_Dbg_Set_Dbg_Trigger                           = 329,
    ROM_API_INDEX_Sec_Dbg_Wait_Ready                                = 330,
    ROM_API_INDEX_Tzc_Sec_Set_Sboot_Done                            = 331,
    ROM_API_INDEX_Tzc_Sec_Set_Bus_Remap                             = 332,
    ROM_API_INDEX_Tzc_Sec_Set_Master_Group                          = 333,
    ROM_API_INDEX_Tzc_Sec_Set_Slave_Group                           = 334,
    ROM_API_INDEX_Tzc_Sec_Set_Codec_Group                           = 335,
    ROM_API_INDEX_Tzc_Sec_Set_Glb_Ctrl_Group                        = 336,
    ROM_API_INDEX_Tzc_Sec_Set_CPU_Group                             = 337,
    ROM_API_INDEX_Tzc_Sec_ROM_Access_Set                            = 338,
    ROM_API_INDEX_Tzc_Sec_ROM_Access_Set_Advance                    = 339,
    ROM_API_INDEX_Tzc_Sec_OCRAM_Access_Set_Advance                  = 340,
    ROM_API_INDEX_Tzc_Sec_OCRAM_Access_Set_Regionx                  = 341,
    ROM_API_INDEX_Tzc_Sec_WRAM_Access_Set_Advance                   = 342,
    ROM_API_INDEX_Tzc_Sec_WRAM_Access_Set_Regionx                   = 343,
    ROM_API_INDEX_Tzc_Sec_HBNRAM_Access_Set                         = 344,
    ROM_API_INDEX_Tzc_Sec_Flash_Access_Set                          = 345,
    ROM_API_INDEX_Tzc_Sec_Flash_Access_Set_Regionx                  = 346,
    ROM_API_INDEX_Tzc_Sec_Flash_Access_Set_Advance                  = 347,
    ROM_API_INDEX_Tzc_Sec_PSRAMB_Access_Set                         = 348,
    ROM_API_INDEX_Tzc_Sec_PSRAMB_Access_Release                     = 349,
    ROM_API_INDEX_Tzc_Sec_PSRAMB_Access_Set_Advance                 = 350,
    ROM_API_INDEX_Tzc_Sec_Set_Se_Ctrl_Mode                          = 351,
    ROM_API_INDEX_Tzc_Sec_Set_Sf_Ctrl_Mode                          = 352,
    ROM_API_INDEX_Tzc_Sec_Set_Se_Group                              = 353,
    ROM_API_INDEX_Tzc_Sec_Set_Sf_Group                              = 354,
    ROM_API_INDEX_Tzc_Sec_Set_WTD_Rst_Delay                         = 355,
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
#define STD_ROMAPI_TABLE      (0x60f83000)
#endif

#define STD_ROM_APITABLE      ((uint32_t *)STD_ROMAPI_TABLE)


#define RomDriver_bflb_clk_get_system_clock \
    ((uint32_t (*)(uint8_t type)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_clk_get_system_clock])

#define RomDriver_bflb_clk_get_peripheral_clock \
    ((uint32_t (*)(uint8_t type, uint8_t idx)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_clk_get_peripheral_clock])

#define RomDriver_bflb_peripheral_clock_get_by_id \
    ((uint32_t (*)(uint8_t peri)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_peripheral_clock_get_by_id])

#define RomDriver_bflb_peripheral_clock_control_by_id \
    ((int (*)(uint8_t peri, bool enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_peripheral_clock_control_by_id])

#define RomDriver_bflb_peripheral_clock_status_get_by_id \
    ((int (*)(uint8_t peri)) \
    STD_ROM_APITABLE[ROM_API_INDEX_bflb_peripheral_clock_status_get_by_id])

#define RomDriver_AON_Power_On_MBG \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_MBG])

#define RomDriver_AON_Power_Off_MBG \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_MBG])

#define RomDriver_AON_Power_On_SFReg \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_SFReg])

#define RomDriver_AON_Power_Off_SFReg \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_SFReg])

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

#define RomDriver_AON_Power_On_BG \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_BG])

#define RomDriver_AON_Power_Off_BG \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_BG])

#define RomDriver_AON_Power_On_LDO15_RF \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_LDO15_RF])

#define RomDriver_AON_Power_Off_LDO15_RF \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_LDO15_RF])

#define RomDriver_AON_Output_Float_LDO15_RF \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Output_Float_LDO15_RF])

#define RomDriver_AON_Output_Pulldown_LDO15_RF \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Output_Pulldown_LDO15_RF])

#define RomDriver_AON_Output_LDO15_RF_Ctrl_by_Pd \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Output_LDO15_RF_Ctrl_by_Pd])

#define RomDriver_AON_Set_Xtal_32K_Inverter_Amplify_Strength \
    ((BL_Err_Type (*)(uint8_t value)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_32K_Inverter_Amplify_Strength])

#define RomDriver_AON_Set_Xtal_32K_Regulator \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_32K_Regulator])

#define RomDriver_AON_Start_Xtal_32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Start_Xtal_32K])

#define RomDriver_AON_Clear_Xtal_32K_Fast_Startup \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Clear_Xtal_32K_Fast_Startup])

#define RomDriver_AON_Get_Xtal_32K_Ready_State \
    ((BL_Sts_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Get_Xtal_32K_Ready_State])

#define RomDriver_AON_Power_On_Xtal_32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_On_Xtal_32K])

#define RomDriver_AON_Power_Off_Xtal_32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_Xtal_32K])

#define RomDriver_AON_Set_Ldo18_Aon_Vout \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo18_Aon_Vout])

#define RomDriver_AON_SW_Trim_Ldo18_Aon_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_SW_Trim_Ldo18_Aon_Vout])

#define RomDriver_AON_Set_Ldo08_Aon_Vout \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo08_Aon_Vout])

#define RomDriver_AON_SW_Trim_Ldo08_Aon_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_SW_Trim_Ldo08_Aon_Vout])

#define RomDriver_AON_Set_Ldo_Soc_Vout \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo_Soc_Vout])

#define RomDriver_AON_SW_Trim_Ldo_Soc_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_SW_Trim_Ldo_Soc_Vout])

#define RomDriver_AON_Set_Switch_For_Efuse \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Switch_For_Efuse])

#define RomDriver_AON_Set_Ldo18_AON_Power_Switch_For_FLASH \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo18_AON_Power_Switch_For_FLASH])

#define RomDriver_AON_Set_Ldo18_AON_Power_Switch_For_PSRAM \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo18_AON_Power_Switch_For_PSRAM])

#define RomDriver_AON_Set_Dcdc_Sys_Vout \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Dcdc_Sys_Vout])

#define RomDriver_AON_SW_Trim_Dcdc_Sys_Vout \
    ((BL_Err_Type (*)(uint8_t trim)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_SW_Trim_Dcdc_Sys_Vout])

#define RomDriver_AON_Set_Ldo18_Aon_Vout_in_Lowpower \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo18_Aon_Vout_in_Lowpower])

#define RomDriver_AON_Set_Dcdc_Sys_Vout_in_Lowpower \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Dcdc_Sys_Vout_in_Lowpower])

#define RomDriver_AON_Set_Ldo_Soc_Vout_in_Lowpower \
    ((BL_Err_Type (*)(uint8_t level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo_Soc_Vout_in_Lowpower])

#define RomDriver_AON_Ctrl_Ldo18_Aon_Mode_by_HW \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Ctrl_Ldo18_Aon_Mode_by_HW])

#define RomDriver_AON_Ctrl_Dcdc_Sys_Mode_by_HW \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Ctrl_Dcdc_Sys_Mode_by_HW])

#define RomDriver_AON_Ctrl_Ldo_Soc_Mode_by_HW \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Ctrl_Ldo_Soc_Mode_by_HW])

#define RomDriver_AON_Set_Ldo18_Aon_Mode \
    ((BL_Err_Type (*)(uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo18_Aon_Mode])

#define RomDriver_AON_Set_Dcdc_Sys_Mode \
    ((BL_Err_Type (*)(uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Dcdc_Sys_Mode])

#define RomDriver_AON_Set_Ldo_Soc_Mode \
    ((BL_Err_Type (*)(uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Ldo_Soc_Mode])

#define RomDriver_AON_Set_Dcdc_Sys_Sstart_Delay \
    ((BL_Err_Type (*)(uint8_t delay)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_Dcdc_Sys_Sstart_Delay])

#define RomDriver_AON_Get_BOD_OUT_State \
    ((BL_Sts_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Get_BOD_OUT_State])

#define RomDriver_AON_Set_BOD_Config \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t threshold, uint8_t mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_BOD_Config])

#define RomDriver_AON_Wdg_Init \
    ((void (*)(uint16_t comp_val)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Wdg_Init])

#define RomDriver_AON_Wdg_Start \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Wdg_Start])

#define RomDriver_AON_Wdg_Status_Get \
    ((BL_Sts_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Wdg_Status_Get])

#define RomDriver_AON_Wdg_Status_Clear \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Wdg_Status_Clear])

#define RomDriver_AON_Wdg_Kick \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Wdg_Kick])

#define RomDriver_AON_Set_RC32M_Speed_As_8M \
    ((void (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_RC32M_Speed_As_8M])

#define RomDriver_AON_Set_RC32M_Code_Fr_Ext \
    ((void (*)(uint8_t code)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Set_RC32M_Code_Fr_Ext])

#define RomDriver_AON_Trim_Ldo_Soc_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo_Soc_Vout])

#define RomDriver_AON_Trim_Ldo08_Aon_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo08_Aon_Vout])

#define RomDriver_AON_Trim_Ldo18_Aon_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo18_Aon_Vout])

#define RomDriver_HBN_Trim_Dcdc12_Sys_Vout \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_Dcdc12_Sys_Vout])

#define RomDriver_Clock_System_Clock_Get \
    ((uint32_t (*)(BL_System_Clock_Type type)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Clock_System_Clock_Get])

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
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clock_src, uint16_t div)) \
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

#define RomDriver_GLB_Power_Off_CPUPLL \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_Off_CPUPLL])

#define RomDriver_GLB_CPUPLL_Ref_Clk_Sel \
    ((BL_Err_Type (*)(uint8_t refClk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_CPUPLL_Ref_Clk_Sel])

#define RomDriver_GLB_Power_On_CPUPLL \
    ((BL_Err_Type (*)(const GLB_CPUPLL_Cfg_Type *const cfg, uint8_t waitStable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_On_CPUPLL])

#define RomDriver_GLB_Set_CPUPLL_PostOut \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_CPUPLL_PostOut])

#define RomDriver_GLB_Set_USB_CLK_From_WIFIPLL \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_USB_CLK_From_WIFIPLL])

#define RomDriver_GLB_Set_SSC_CLK_From_WIFIPLL \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SSC_CLK_From_WIFIPLL])

#define RomDriver_GLB_Set_MCU_System_CLK_Div \
    ((BL_Err_Type (*)(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_System_CLK_Div])

#define RomDriver_GLB_Get_MCU_System_CLK_Div \
    ((BL_Err_Type (*)(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_MCU_System_CLK_Div])

#define RomDriver_GLB_Set_MCU_System_CLK \
    ((BL_Err_Type (*)(uint8_t clkFreq)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_System_CLK])

#define RomDriver_GLB_Set_WL_MCU_System_CLK \
    ((BL_Err_Type (*)(uint8_t clkFreq, uint8_t cpuClkDiv, uint8_t BusClkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_WL_MCU_System_CLK])

#define RomDriver_GLB_Set_WL_XCLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_WL_XCLK_Sel])

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

#define RomDriver_GLB_Set_Peripheral_DMA_CN \
    ((BL_Err_Type (*)(uint8_t peri, uint8_t cn)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Peripheral_DMA_CN])

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

#define RomDriver_GLB_Set_SPI_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI_CLK])

#define RomDriver_GLB_SPI_Sig_Swap_Set \
    ((BL_Err_Type (*)(uint8_t group, uint8_t swap)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_SPI_Sig_Swap_Set])

#define RomDriver_GLB_Set_PEC_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PEC_CLK])

#define RomDriver_GLB_Set_PWM1_IO_Sel \
    ((BL_Err_Type (*)(uint8_t ioSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_PWM1_IO_Sel])

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

#define RomDriver_GLB_Set_Audio_ADC_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Audio_ADC_CLK])

#define RomDriver_GLB_Set_Audio_SOLO_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Audio_SOLO_CLK])

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

#define RomDriver_GLB_Config_SDIO_Host_Reset_System \
    ((BL_Err_Type (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_System])

#define RomDriver_GLB_UART_Sig_Swap_Set \
    ((BL_Err_Type (*)(uint8_t group, uint8_t swap)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_UART_Sig_Swap_Set])

#define RomDriver_GLB_Swap_MCU_SPI_0_MOSI_With_MISO \
    ((BL_Err_Type (*)(BL_Fun_Type newState)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Swap_MCU_SPI_0_MOSI_With_MISO])

#define RomDriver_GLB_Set_MCU_SPI_0_ACT_MOD_Sel \
    ((BL_Err_Type (*)(uint8_t mod)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MCU_SPI_0_ACT_MOD_Sel])

#define RomDriver_GLB_Set_SFlash_IO_PARM \
    ((BL_Err_Type (*)(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SFlash_IO_PARM])

#define RomDriver_GLB_Set_SFlash2_IO_PARM \
    ((BL_Err_Type (*)(uint8_t swapIo3Io0)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SFlash2_IO_PARM])

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

#define RomDriver_GLB_Trim_Ldo18ioBypass \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Trim_Ldo18ioBypass])

#define RomDriver_GLB_Trim_Ldo18ioVoutTrim \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Trim_Ldo18ioVoutTrim])

#define RomDriver_GLB_Power_Down_Ldo18ioVout \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Power_Down_Ldo18ioVout])

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

#define RomDriver_GLB_Set_Slave_Grp_0_CLK \
    ((BL_Err_Type (*)(uint8_t slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Slave_Grp_0_CLK])

#define RomDriver_GLB_Config_WIFIPLL \
    ((BL_Err_Type (*)(uint8_t xtalType, const GLB_WIFIPLL_Cfg_Type *pllCfgList)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_WIFIPLL])

#define RomDriver_GLB_Config_CPUPLL \
    ((BL_Err_Type (*)(uint8_t xtalType, const GLB_CPUPLL_Cfg_Type *pllCfgList)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Config_CPUPLL])

#define RomDriver_GLB_Simple_Set_MCU_System_CLK \
    ((BL_Err_Type (*)(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Simple_Set_MCU_System_CLK])

#define RomDriver_GLB_Set_Flash_Xip \
    ((void (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_Flash_Xip])

#define RomDriver_GLB_Halt_Mini_Sys \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Halt_Mini_Sys])

#define RomDriver_GLB_Release_Mini_Sys \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Release_Mini_Sys])

#define RomDriver_GLB_MINI_PER_Software_Reset \
    ((BL_Err_Type (*)(uint8_t swrst)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_MINI_PER_Software_Reset])

#define RomDriver_GLB_Set_SPI1_CLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI1_CLK])

#define RomDriver_GLB_Set_MINI_FCLK \
    ((BL_Err_Type (*)(uint8_t enable, uint8_t clkSel, uint8_t div)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_MINI_FCLK])

#define RomDriver_GLB_MINI_PER_Clock_Gate \
    ((BL_Err_Type (*)(uint32_t clk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_MINI_PER_Clock_Gate])

#define RomDriver_GLB_MINI_PER_Clock_UnGate \
    ((BL_Err_Type (*)(uint32_t clk)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_MINI_PER_Clock_UnGate])

#define RomDriver_GLB_Select_LPCPU_Jtag \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Select_LPCPU_Jtag])

#define RomDriver_GLB_Halt_CPU \
    ((BL_Err_Type (*)(uint8_t coreID)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Halt_CPU])

#define RomDriver_GLB_Release_CPU \
    ((BL_Err_Type (*)(uint8_t coreID)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Release_CPU])

#define RomDriver_GLB_RC32M_Force_On \
    ((void (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_RC32M_Force_On])

#define RomDriver_GLB_RC32M_Force_Off \
    ((void (*)(uint8_t enable)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_RC32M_Force_Off])

#define RomDriver_GLB_Get_NP2MINI_Interrupt_Status \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_NP2MINI_Interrupt_Status])

#define RomDriver_GLB_Set_NP2MINI_Interrupt_Clear \
    ((void (*)(uint8_t irq)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_NP2MINI_Interrupt_Clear])

#define RomDriver_GLB_Set_NP2MINI_Interrupt_Mask \
    ((void (*)(uint8_t irq, uint8_t mask)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Set_NP2MINI_Interrupt_Mask])

#define RomDriver_GLB_Get_NP2MINI_Interrupt_Mask \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Get_NP2MINI_Interrupt_Mask])

#define RomDriver_GLB_Embedded_Flash_Pad_Enable \
    ((BL_Err_Type (*)(uint8_t swapIo2Cs)) \
    STD_ROM_APITABLE[ROM_API_INDEX_GLB_Embedded_Flash_Pad_Enable])

#define RomDriver_HBN_Power_Down_Flash \
    ((void (*)(spi_flash_cfg_type *flashCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_Down_Flash])

#define RomDriver_HBN_Level_Select \
    ((BL_Err_Type (*)(uint8_t hbn_level)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Level_Select])

#define RomDriver_HBN_Reset \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Reset])

#define RomDriver_HBN_Set_GPADC_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_GPADC_CLK_Sel])

#define RomDriver_HBN_PIR_Enable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_PIR_Enable])

#define RomDriver_HBN_PIR_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_PIR_Disable])

#define RomDriver_HBN_PIR_INT_Config \
    ((BL_Err_Type (*)(HBN_PIR_INT_CFG_Type *pirIntCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_PIR_INT_Config])

#define RomDriver_HBN_PIR_LPF_Sel \
    ((BL_Err_Type (*)(uint8_t lpf)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_PIR_LPF_Sel])

#define RomDriver_HBN_PIR_HPF_Sel \
    ((BL_Err_Type (*)(uint8_t hpf)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_PIR_HPF_Sel])

#define RomDriver_HBN_Set_PIR_Threshold \
    ((BL_Err_Type (*)(uint16_t threshold)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_PIR_Threshold])

#define RomDriver_HBN_Get_PIR_Threshold \
    ((uint16_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_PIR_Threshold])

#define RomDriver_HBN_Set_PIR_Interval \
    ((BL_Err_Type (*)(uint16_t interval)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_PIR_Interval])

#define RomDriver_HBN_Get_PIR_Interval \
    ((uint16_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_PIR_Interval])

#define RomDriver_HBN_32K_Sel \
    ((BL_Err_Type (*)(uint8_t clkType)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_32K_Sel])

#define RomDriver_HBN_Set_UART_CLK_Sel \
    ((BL_Err_Type (*)(uint8_t clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_UART_CLK_Sel])

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

#define RomDriver_HBN_Keep_On_RC32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Keep_On_RC32K])

#define RomDriver_HBN_Power_Off_RC32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_RC32K])

#define RomDriver_HBN_Trim_RC32K \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Trim_RC32K])

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

#define RomDriver_HBN_Set_Gpio_Keep \
    ((BL_Err_Type (*)(uint8_t gpioKeep)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_Gpio_Keep])

#define RomDriver_HBN_Get_Gpio_Keep \
    ((BL_Err_Type (*)(uint8_t *gpioKeep)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_Gpio_Keep])

#define RomDriver_HBN_Clear_Gpio_Keep \
    ((BL_Err_Type (*)(uint8_t gpioKeep)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clear_Gpio_Keep])

#define RomDriver_HBN_Set_PDS_Gpio_Keep \
    ((BL_Err_Type (*)(uint8_t gpioKeep)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Set_PDS_Gpio_Keep])

#define RomDriver_HBN_Get_PDS_Gpio_Keep \
    ((BL_Err_Type (*)(uint8_t *gpioKeep)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_PDS_Gpio_Keep])

#define RomDriver_HBN_Clear_PDS_Gpio_Keep \
    ((BL_Err_Type (*)(uint8_t gpioKeep)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Clear_PDS_Gpio_Keep])

#define RomDriver_HBN_Disable_Gpio_Keep \
    ((BL_Err_Type (*)(int pin)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Disable_Gpio_Keep])

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

#define RomDriver_HBN_Recal_RC32K \
    ((BL_Err_Type (*)(int32_t expected_counter, int32_t actual_counter)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Recal_RC32K])

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

#define RomDriver_HBN_Get_All_INT_State \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Get_All_INT_State])

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

#define RomDriver_HBN_Enable_AComp_IRQ \
    ((BL_Err_Type (*)(uint8_t acompId, uint8_t edge)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Enable_AComp_IRQ])

#define RomDriver_HBN_Disable_AComp_IRQ \
    ((BL_Err_Type (*)(uint8_t acompId, uint8_t edge)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Disable_AComp_IRQ])

#define RomDriver_HBN_Enable_BOD_IRQ \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Enable_BOD_IRQ])

#define RomDriver_HBN_Disable_BOD_IRQ \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Disable_BOD_IRQ])

#define RomDriver_HBN_Aon_Pad_WakeUpCfg \
    ((BL_Err_Type (*)(BL_Fun_Type puPdEn, uint8_t trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec)) \
    STD_ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_WakeUpCfg])

#define RomDriver_PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie \
    ((BL_Err_Type (*)(uint8_t pad, uint8_t pu, uint8_t pd, uint8_t ie)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie])

#define RomDriver_PDS_Set_GPIO_Pad_IntMask \
    ((BL_Err_Type (*)(uint8_t pad, BL_Mask_Type intMask)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask])

#define RomDriver_PDS_Set_GPIO_Pad_IntMode \
    ((BL_Err_Type (*)(uint8_t pin, uint8_t trig)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode])

#define RomDriver_PDS_Set_GPIO_Pad_IntClr \
    ((BL_Err_Type (*)(uint8_t pin)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr])

#define RomDriver_PDS_Get_GPIO_Pad_IntStatus \
    ((BL_Sts_Type (*)(uint8_t pad)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus])

#define RomDriver_PDS_Set_All_GPIO_Pad_IntClr \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_GPIO_Pad_IntClr])

#define RomDriver_PDS_Get_All_GPIO_Pad_IntStatus \
    ((uint64_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Get_All_GPIO_Pad_IntStatus])

#define RomDriver_PDS_Set_Flash_Pad_Pull_None \
    ((BL_Err_Type (*)(uint8_t pinCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Flash_Pad_Pull_None])

#define RomDriver_PDS_Set_Flash_Pad_Pull_None_Fast \
    ((BL_Err_Type (*)(uint8_t pinCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_Flash_Pad_Pull_None_Fast])

#define RomDriver_PDS_Disable_GPIO_Keep \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Disable_GPIO_Keep])

#define RomDriver_PDS_Set_MCU0_Clock_Enable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU0_Clock_Enable])

#define RomDriver_PDS_Set_MCU0_Clock_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU0_Clock_Disable])

#define RomDriver_PDS_Set_MCU1_Clock_Enable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU1_Clock_Enable])

#define RomDriver_PDS_Set_MCU1_Clock_Disable \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU1_Clock_Disable])

#define RomDriver_PDS_Set_MCU0_Reset_Address \
    ((BL_Err_Type (*)(uint32_t addr)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Set_MCU0_Reset_Address])

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
    ((BL_Err_Type (*)(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5)) \
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

#define RomDriver_PDS_Triger_SW_Wakeup_PDS \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Triger_SW_Wakeup_PDS])

#define RomDriver_PDS_Clear_SW_Wakeup_PDS \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Clear_SW_Wakeup_PDS])

#define RomDriver_PDS_Trim_RC32M \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Trim_RC32M])

#define RomDriver_PDS_Trim_USB20 \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Trim_USB20])

#define RomDriver_PDS_Power_Off_WB \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Power_Off_WB])

#define RomDriver_PDS_Power_On_WB \
    ((BL_Err_Type (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PDS_Power_On_WB])

#define RomDriver_PSram_Ctrl_Init \
    ((void (*)(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Cfg_Type *psramCtrlCfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Init])

#define RomDriver_PSram_Ctrl_Winbond_Read_Reg \
    ((BL_Err_Type (*)(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, uint16_t *regVal)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Winbond_Read_Reg])

#define RomDriver_PSram_Ctrl_Winbond_Write_Reg \
    ((BL_Err_Type (*)(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, PSRAM_Winbond_Cfg_Type *reg_cfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Winbond_Write_Reg])

#define RomDriver_PSram_Ctrl_ApMem_Read_Reg \
    ((BL_Err_Type (*)(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_ApMem_Cfg_Reg_Type reg_addr, uint16_t *regVal)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_ApMem_Read_Reg])

#define RomDriver_PSram_Ctrl_ApMem_Write_Reg \
    ((BL_Err_Type (*)(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_ApMem_Cfg_Reg_Type reg_addr, PSRAM_APMemory_Cfg_Type *reg_cfg)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_ApMem_Write_Reg])

#define RomDriver_PSram_Ctrl_ApMem_Reset \
    ((void (*)(PSRAM_ID_Type PSRAM_ID)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_ApMem_Reset])

#define RomDriver_PSram_Ctrl_Winbond_Reset \
    ((void (*)(PSRAM_ID_Type PSRAM_ID)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Winbond_Reset])

#define RomDriver_PSram_Ctrl_CK_Sel \
    ((void (*)(PSRAM_ID_Type PSRAM_ID, PSRAM_Clock_Type clkSel)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_CK_Sel])

#define RomDriver_PSram_Ctrl_Get_Timeout_Flag \
    ((uint8_t (*)(PSRAM_ID_Type PSRAM_ID)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Get_Timeout_Flag])

#define RomDriver_PSram_Ctrl_Clear_Timout_Flag \
    ((void (*)(PSRAM_ID_Type PSRAM_ID)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Clear_Timout_Flag])

#define RomDriver_PSram_Ctrl_Debug_Timout \
    ((void (*)(PSRAM_ID_Type PSRAM_ID, uint8_t enable, uint32_t timeoutThr)) \
    STD_ROM_APITABLE[ROM_API_INDEX_PSram_Ctrl_Debug_Timout])

#define RomDriver_Sec_Dbg_Read_Chip_ID \
    ((void (*)(uint8_t id[8])) \
    STD_ROM_APITABLE[ROM_API_INDEX_Sec_Dbg_Read_Chip_ID])

#define RomDriver_Sec_Dbg_Set_Dbg_Pwd \
    ((void (*)(const uint32_t pwd[4])) \
    STD_ROM_APITABLE[ROM_API_INDEX_Sec_Dbg_Set_Dbg_Pwd])

#define RomDriver_Sec_Dbg_Read_Dbg_Mode \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Sec_Dbg_Read_Dbg_Mode])

#define RomDriver_Sec_Dbg_Read_Dbg_Enable \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Sec_Dbg_Read_Dbg_Enable])

#define RomDriver_Sec_Dbg_Set_Dbg_Trigger \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Sec_Dbg_Set_Dbg_Trigger])

#define RomDriver_Sec_Dbg_Wait_Ready \
    ((uint32_t (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Sec_Dbg_Wait_Ready])

#define RomDriver_Tzc_Sec_Set_Sboot_Done \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Sboot_Done])

#define RomDriver_Tzc_Sec_Set_Bus_Remap \
    ((void (*)(uint8_t busRmpEn)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Bus_Remap])

#define RomDriver_Tzc_Sec_Set_Master_Group \
    ((void (*)(TZC_SEC_Master_Type masterType, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Master_Group])

#define RomDriver_Tzc_Sec_Set_Slave_Group \
    ((void (*)(TZC_SEC_Slave_Type slaveType, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Slave_Group])

#define RomDriver_Tzc_Sec_Set_Codec_Group \
    ((void (*)(uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Codec_Group])

#define RomDriver_Tzc_Sec_Set_Glb_Ctrl_Group \
    ((void (*)(TZC_SEC_GLB_Ctrl_Type slaveType, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Glb_Ctrl_Group])

#define RomDriver_Tzc_Sec_Set_CPU_Group \
    ((void (*)(uint8_t cpu, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_CPU_Group])

#define RomDriver_Tzc_Sec_ROM_Access_Set \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_ROM_Access_Set])

#define RomDriver_Tzc_Sec_ROM_Access_Set_Advance \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_ROM_Access_Set_Advance])

#define RomDriver_Tzc_Sec_OCRAM_Access_Set_Advance \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_OCRAM_Access_Set_Advance])

#define RomDriver_Tzc_Sec_OCRAM_Access_Set_Regionx \
    ((void (*)(uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_OCRAM_Access_Set_Regionx])

#define RomDriver_Tzc_Sec_WRAM_Access_Set_Advance \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_WRAM_Access_Set_Advance])

#define RomDriver_Tzc_Sec_WRAM_Access_Set_Regionx \
    ((void (*)(uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_WRAM_Access_Set_Regionx])

#define RomDriver_Tzc_Sec_HBNRAM_Access_Set \
    ((void (*)(uint32_t startAddr, uint32_t length)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_HBNRAM_Access_Set])

#define RomDriver_Tzc_Sec_Flash_Access_Set \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Flash_Access_Set])

#define RomDriver_Tzc_Sec_Flash_Access_Set_Regionx \
    ((void (*)(uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Flash_Access_Set_Regionx])

#define RomDriver_Tzc_Sec_Flash_Access_Set_Advance \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Flash_Access_Set_Advance])

#define RomDriver_Tzc_Sec_PSRAMB_Access_Set \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_PSRAMB_Access_Set])

#define RomDriver_Tzc_Sec_PSRAMB_Access_Release \
    ((void (*)(void)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_PSRAMB_Access_Release])

#define RomDriver_Tzc_Sec_PSRAMB_Access_Set_Advance \
    ((void (*)(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_PSRAMB_Access_Set_Advance])

#define RomDriver_Tzc_Sec_Set_Se_Ctrl_Mode \
    ((void (*)(TZC_SEC_SE_Ctrl_Mode mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Se_Ctrl_Mode])

#define RomDriver_Tzc_Sec_Set_Sf_Ctrl_Mode \
    ((void (*)(TZC_SEC_SF_Ctrl_Mode mode)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Sf_Ctrl_Mode])

#define RomDriver_Tzc_Sec_Set_Se_Group \
    ((void (*)(TZC_SEC_SE_Ctrl_Type slaveType, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Se_Group])

#define RomDriver_Tzc_Sec_Set_Sf_Group \
    ((void (*)(TZC_SEC_SF_Ctrl_Type slaveType, uint8_t group)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_Sf_Group])

#define RomDriver_Tzc_Sec_Set_WTD_Rst_Delay \
    ((void (*)(uint16_t delayValue)) \
    STD_ROM_APITABLE[ROM_API_INDEX_Tzc_Sec_Set_WTD_Rst_Delay])


/*@} end of group ROMDRIVER_Public_Macros */

/** @defgroup  ROMDRIVER_Public_Functions
 *  @{
 */

/*@} end of group ROMDRIVER_Public_Functions */

/*@} end of group ROMDRIVER */

/*@} end of group bl616d_Peripheral_Driver */

#endif /* __bl616d_ROMDRIVER_H__ */
