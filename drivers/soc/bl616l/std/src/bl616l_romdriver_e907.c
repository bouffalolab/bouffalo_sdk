/**
  ******************************************************************************
  * @file    bl616d_romdriver_e907.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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
  
#include "stdint.h"
#include "bl616l_romdriver_e907.h"


/** @addtogroup  BL616_Periph_Driver
 *  @{
 */

/** @defgroup ROMDRIVER
 *  @brief ROMDRIVER common functions
 *  @{
 */

/** @defgroup ROMDRIVER_Private_Type
 *  @{
 */

/*@} end of group ROMDRIVER_Private_Type*/

/** @defgroup ROMDRIVER_Private_Defines
 *  @{
 */

/*@} end of group ROMDRIVER_Private_Defines */

/** @defgroup ROMDRIVER_Private_Variables
 *  @{
 */                               

/*@} end of group ROMDRIVER_Private_Variables */

/** @defgroup ROMDRIVER_Global_Variables
 *  @{
 */ 
 


uint32_t const romDriverTable[]={
    0x0616A002,
    0x00000000,
    0x00000000,
    0x00000000,

    [ROM_API_INDEX_AON_Power_On_RF]                                            = (uint32_t)AON_Power_On_RF,
    [ROM_API_INDEX_AON_Power_Off_RF]                                           = (uint32_t)AON_Power_Off_RF,
    [ROM_API_INDEX_AON_Power_On_XTAL]                                          = (uint32_t)AON_Power_On_XTAL,
    [ROM_API_INDEX_AON_Set_Xtal_CapCode]                                       = (uint32_t)AON_Set_Xtal_CapCode,
    [ROM_API_INDEX_AON_Get_Xtal_CapCode]                                       = (uint32_t)AON_Get_Xtal_CapCode,
    [ROM_API_INDEX_AON_Power_Off_XTAL]                                         = (uint32_t)AON_Power_Off_XTAL,
    [ROM_API_INDEX_AON_XTAL_Enter_LowPower]                                    = (uint32_t)AON_XTAL_Enter_LowPower,
    [ROM_API_INDEX_AON_XTAL_Exit_LowPower]                                     = (uint32_t)AON_XTAL_Exit_LowPower,
    [ROM_API_INDEX_AON_LDO18_IO_Switch_Efuse]                                  = (uint32_t)AON_LDO18_IO_Switch_Efuse,
    [ROM_API_INDEX_AON_LDO18_IO_Switch_Psram]                                  = (uint32_t)AON_LDO18_IO_Switch_Psram,
    [ROM_API_INDEX_AON_LDO18_IO_Switch_Flash]                                  = (uint32_t)AON_LDO18_IO_Switch_Flash,
    [ROM_API_INDEX_AON_Set_LDO18_IO_Sstart_Delay]                              = (uint32_t)AON_Set_LDO18_IO_Sstart_Delay,
    [ROM_API_INDEX_AON_SW_Set_Ldo18_IO_Vout]                                   = (uint32_t)AON_SW_Set_Ldo18_IO_Vout,
    [ROM_API_INDEX_AON_SW_Trim_Ldo18_IO_Vout]                                  = (uint32_t)AON_SW_Trim_Ldo18_IO_Vout,
    [ROM_API_INDEX_AON_Trim_Ldo18_IO_Vout]                                     = (uint32_t)AON_Trim_Ldo18_IO_Vout,
    [ROM_API_INDEX_Clock_System_Clock_Get]                                     = (uint32_t)Clock_System_Clock_Get,
    [ROM_API_INDEX_Clock_Get_PSRAMB_Clk]                                       = (uint32_t)Clock_Get_PSRAMB_Clk,
    [ROM_API_INDEX_Clock_Audio_ADC_Clock_Get]                                  = (uint32_t)Clock_Audio_ADC_Clock_Get,
    [ROM_API_INDEX_Clock_PWM_Clk_Get]                                          = (uint32_t)Clock_PWM_Clk_Get,
    [ROM_API_INDEX_Clock_Peripheral_Clock_Get]                                 = (uint32_t)Clock_Peripheral_Clock_Get,
    [ROM_API_INDEX_SystemCoreClockGet]                                         = (uint32_t)SystemCoreClockGet,
    [ROM_API_INDEX_CPU_Set_MTimer_RST]                                         = (uint32_t)CPU_Set_MTimer_RST,
    [ROM_API_INDEX_CPU_Reset_MTimer]                                           = (uint32_t)CPU_Reset_MTimer,
    [ROM_API_INDEX_CPU_Set_MTimer_CLK]                                         = (uint32_t)CPU_Set_MTimer_CLK,
    [ROM_API_INDEX_CPU_Get_MTimer_Source_Clock]                                = (uint32_t)CPU_Get_MTimer_Source_Clock,
    [ROM_API_INDEX_CPU_Get_MTimer_Clock]                                       = (uint32_t)CPU_Get_MTimer_Clock,
    [ROM_API_INDEX_CPU_Get_MTimer_Counter]                                     = (uint32_t)CPU_Get_MTimer_Counter,
    [ROM_API_INDEX_CPU_Get_CPU_Cycle]                                          = (uint32_t)CPU_Get_CPU_Cycle,
    [ROM_API_INDEX_CPU_Get_MTimer_US]                                          = (uint32_t)CPU_Get_MTimer_US,
    [ROM_API_INDEX_CPU_Get_MTimer_MS]                                          = (uint32_t)CPU_Get_MTimer_MS,
    [ROM_API_INDEX_CPU_MTimer_Delay_US]                                        = (uint32_t)CPU_MTimer_Delay_US,
    [ROM_API_INDEX_CPU_MTimer_Delay_MS]                                        = (uint32_t)CPU_MTimer_Delay_MS,
    [ROM_API_INDEX_ASM_Delay_Us]                                               = (uint32_t)ASM_Delay_Us,
    [ROM_API_INDEX_arch_delay_us]                                              = (uint32_t)arch_delay_us,
    [ROM_API_INDEX_arch_delay_ms]                                              = (uint32_t)arch_delay_ms,
    [ROM_API_INDEX_bflb_ef_ctrl_get_common_trim_list]                          = (uint32_t)bflb_ef_ctrl_get_common_trim_list,
    [ROM_API_INDEX_bflb_efuse_get_device_info]                                 = (uint32_t)bflb_efuse_get_device_info,
    [ROM_API_INDEX_bflb_efuse_read_mac_address]                                = (uint32_t)bflb_efuse_read_mac_address,
    [ROM_API_INDEX_bflb_efuse_get_chipid]                                      = (uint32_t)bflb_efuse_get_chipid,
    [ROM_API_INDEX_bflb_efuse_is_mac_address_slot_empty]                       = (uint32_t)bflb_efuse_is_mac_address_slot_empty,
    [ROM_API_INDEX_bflb_efuse_read_mac_address_opt]                            = (uint32_t)bflb_efuse_read_mac_address_opt,
    [ROM_API_INDEX_bflb_efuse_get_adc_trim]                                    = (uint32_t)bflb_efuse_get_adc_trim,
    [ROM_API_INDEX_bflb_efuse_get_adc_tsen_trim]                               = (uint32_t)bflb_efuse_get_adc_tsen_trim,
    [ROM_API_INDEX_bflb_efuse_read_secure_boot]                                = (uint32_t)bflb_efuse_read_secure_boot,
    [ROM_API_INDEX_bflb_efuse_enable_aes]                                      = (uint32_t)bflb_efuse_enable_aes,
    [ROM_API_INDEX_bflb_efuse_rw_lock_aes_key]                                 = (uint32_t)bflb_efuse_rw_lock_aes_key,
    [ROM_API_INDEX_bflb_efuse_rw_lock_dbg_key]                                 = (uint32_t)bflb_efuse_rw_lock_dbg_key,
    [ROM_API_INDEX_bflb_efuse_write_lock_pk_hash]                              = (uint32_t)bflb_efuse_write_lock_pk_hash,
    [ROM_API_INDEX_bflb_efuse_write_lock_usb_pid_vid]                          = (uint32_t)bflb_efuse_write_lock_usb_pid_vid,
    [ROM_API_INDEX_GLB_Set_MCU_Muxpll_160M_Sel]                                = (uint32_t)GLB_Set_MCU_Muxpll_160M_Sel,
    [ROM_API_INDEX_GLB_Get_MCU_Muxpll_160M_Sel]                                = (uint32_t)GLB_Get_MCU_Muxpll_160M_Sel,
    [ROM_API_INDEX_GLB_Set_MCU_Muxpll_80M_Sel]                                 = (uint32_t)GLB_Set_MCU_Muxpll_80M_Sel,
    [ROM_API_INDEX_GLB_Get_MCU_Muxpll_80M_Sel]                                 = (uint32_t)GLB_Get_MCU_Muxpll_80M_Sel,
    [ROM_API_INDEX_GLB_Set_ISP_Muxpll_80M_Sel]                                 = (uint32_t)GLB_Set_ISP_Muxpll_80M_Sel,
    [ROM_API_INDEX_GLB_Get_ISP_Muxpll_80M_Sel]                                 = (uint32_t)GLB_Get_ISP_Muxpll_80M_Sel,
    [ROM_API_INDEX_GLB_Power_On_XTAL_And_PLL_CLK]                              = (uint32_t)GLB_Power_On_XTAL_And_PLL_CLK,
    [ROM_API_INDEX_GLB_Power_Off_WIFIPLL]                                      = (uint32_t)GLB_Power_Off_WIFIPLL,
    [ROM_API_INDEX_GLB_WIFIPLL_Ref_Clk_Sel]                                    = (uint32_t)GLB_WIFIPLL_Ref_Clk_Sel,
    [ROM_API_INDEX_GLB_Power_On_WIFIPLL]                                       = (uint32_t)GLB_Power_On_WIFIPLL,
    [ROM_API_INDEX_GLB_Set_MCU_System_CLK_Div]                                 = (uint32_t)GLB_Set_MCU_System_CLK_Div,
    [ROM_API_INDEX_GLB_Get_MCU_System_CLK_Div]                                 = (uint32_t)GLB_Get_MCU_System_CLK_Div,
    [ROM_API_INDEX_GLB_Set_MCU_System_CLK]                                     = (uint32_t)GLB_Set_MCU_System_CLK,
    [ROM_API_INDEX_GLB_Get_Core_Type]                                          = (uint32_t)GLB_Get_Core_Type,
    [ROM_API_INDEX_GLB_Set_CPU_Reset_Address]                                  = (uint32_t)GLB_Set_CPU_Reset_Address,
    [ROM_API_INDEX_GLB_Clr_EMI_Reset_Gate]                                     = (uint32_t)GLB_Clr_EMI_Reset_Gate,
    [ROM_API_INDEX_GLB_Set_ADC_CLK]                                            = (uint32_t)GLB_Set_ADC_CLK,
    [ROM_API_INDEX_GLB_Set_DMA_CLK]                                            = (uint32_t)GLB_Set_DMA_CLK,
    [ROM_API_INDEX_GLB_Set_IR_CLK]                                             = (uint32_t)GLB_Set_IR_CLK,
    [ROM_API_INDEX_GLB_IR_RX_GPIO_Sel]                                         = (uint32_t)GLB_IR_RX_GPIO_Sel,
    [ROM_API_INDEX_GLB_Set_UART_CLK]                                           = (uint32_t)GLB_Set_UART_CLK,
    [ROM_API_INDEX_GLB_UART_Fun_Sel]                                           = (uint32_t)GLB_UART_Fun_Sel,
    [ROM_API_INDEX_GLB_Set_SF_CLK]                                             = (uint32_t)GLB_Set_SF_CLK,
    [ROM_API_INDEX_GLB_Set_I2C_CLK]                                            = (uint32_t)GLB_Set_I2C_CLK,
    [ROM_API_INDEX_GLB_Set_I2S_CLK]                                            = (uint32_t)GLB_Set_I2S_CLK,
    [ROM_API_INDEX_GLB_Set_SPI0_CLK]                                           = (uint32_t)GLB_Set_SPI0_CLK,
    [ROM_API_INDEX_GLB_Set_SPI1_CLK]                                           = (uint32_t)GLB_Set_SPI1_CLK,
    [ROM_API_INDEX_GLB_SPI0_Sig_Swap_Set]                                      = (uint32_t)GLB_SPI0_Sig_Swap_Set,
    [ROM_API_INDEX_GLB_SPI1_Sig_Swap_Set]                                      = (uint32_t)GLB_SPI1_Sig_Swap_Set,
    [ROM_API_INDEX_GLB_Set_PWM1_IO_Sel]                                        = (uint32_t)GLB_Set_PWM1_IO_Sel,
    [ROM_API_INDEX_GLB_Set_PWM_CLK]                                            = (uint32_t)GLB_Set_PWM_CLK,
    [ROM_API_INDEX_GLB_Set_PDM_IO_Sel]                                         = (uint32_t)GLB_Set_PDM_IO_Sel,
    [ROM_API_INDEX_GLB_Set_DBI_CLK]                                            = (uint32_t)GLB_Set_DBI_CLK,
    [ROM_API_INDEX_GLB_Set_DIG_CLK_Sel]                                        = (uint32_t)GLB_Set_DIG_CLK_Sel,
    [ROM_API_INDEX_GLB_Set_DIG_512K_CLK]                                       = (uint32_t)GLB_Set_DIG_512K_CLK,
    [ROM_API_INDEX_GLB_Set_DIG_32K_CLK]                                        = (uint32_t)GLB_Set_DIG_32K_CLK,
    [ROM_API_INDEX_GLB_Platform_Wakeup_PDS_Enable]                             = (uint32_t)GLB_Platform_Wakeup_PDS_Enable,
    [ROM_API_INDEX_GLB_Sel_MCU_TMR_GPIO_Clock]                                 = (uint32_t)GLB_Sel_MCU_TMR_GPIO_Clock,
    [ROM_API_INDEX_GLB_Set_Chip_Clock_Out0_Sel]                                = (uint32_t)GLB_Set_Chip_Clock_Out0_Sel,
    [ROM_API_INDEX_GLB_Set_Chip_Clock_Out1_Sel]                                = (uint32_t)GLB_Set_Chip_Clock_Out1_Sel,
    [ROM_API_INDEX_GLB_Set_Chip_Clock_Out2_Sel]                                = (uint32_t)GLB_Set_Chip_Clock_Out2_Sel,
    [ROM_API_INDEX_GLB_Set_Chip_Clock_Out3_Sel]                                = (uint32_t)GLB_Set_Chip_Clock_Out3_Sel,
    [ROM_API_INDEX_GLB_Set_Audio_AUTO_CLK]                                     = (uint32_t)GLB_Set_Audio_AUTO_CLK,
    [ROM_API_INDEX_GLB_Set_Audio_SOLO_CLK]                                     = (uint32_t)GLB_Set_Audio_SOLO_CLK,
    [ROM_API_INDEX_GLB_Set_ISP_CORE_CLK]                                       = (uint32_t)GLB_Set_ISP_CORE_CLK,
    [ROM_API_INDEX_GLB_Invert_ETH_RX_CLK]                                      = (uint32_t)GLB_Invert_ETH_RX_CLK,
    [ROM_API_INDEX_GLB_Invert_ETH_TX_CLK]                                      = (uint32_t)GLB_Invert_ETH_TX_CLK,
    [ROM_API_INDEX_GLB_Invert_ETH_REF_O_CLK]                                   = (uint32_t)GLB_Invert_ETH_REF_O_CLK,
    [ROM_API_INDEX_GLB_Set_ETH_REF_O_CLK_Sel]                                  = (uint32_t)GLB_Set_ETH_REF_O_CLK_Sel,
    [ROM_API_INDEX_GLB_Set_CAM_CLK]                                            = (uint32_t)GLB_Set_CAM_CLK,
    [ROM_API_INDEX_GLB_Set_SDH_CLK]                                            = (uint32_t)GLB_Set_SDH_CLK,
    [ROM_API_INDEX_GLB_Set_PEC_CLK]                                            = (uint32_t)GLB_Set_PEC_CLK,
    [ROM_API_INDEX_GLB_Set_AHB2QSPI_CLK]                                       = (uint32_t)GLB_Set_AHB2QSPI_CLK,
    [ROM_API_INDEX_GLB_Config_SDIO_Host_Cmd0_Reset]                            = (uint32_t)GLB_Config_SDIO_Host_Cmd0_Reset,
    [ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_System]                          = (uint32_t)GLB_Config_SDIO_Host_Reset_System,
    [ROM_API_INDEX_GLB_Config_SDIO_Host_Reset_SDU]                             = (uint32_t)GLB_Config_SDIO_Host_Reset_SDU,
    [ROM_API_INDEX_GLB_Config_SDIO_Host_Interrupt_CPU]                         = (uint32_t)GLB_Config_SDIO_Host_Interrupt_CPU,
    [ROM_API_INDEX_GLB_UART_Sig_Swap_Set]                                      = (uint32_t)GLB_UART_Sig_Swap_Set,
    [ROM_API_INDEX_GLB_Swap_MCU_SPI_0_MOSI_With_MISO]                          = (uint32_t)GLB_Swap_MCU_SPI_0_MOSI_With_MISO,
    [ROM_API_INDEX_GLB_Swap_MCU_SPI_1_MOSI_With_MISO]                          = (uint32_t)GLB_Swap_MCU_SPI_1_MOSI_With_MISO,
    [ROM_API_INDEX_GLB_Set_MCU_SPI_0_ACT_MOD_Sel]                              = (uint32_t)GLB_Set_MCU_SPI_0_ACT_MOD_Sel,
    [ROM_API_INDEX_GLB_Set_MCU_SPI_1_ACT_MOD_Sel]                              = (uint32_t)GLB_Set_MCU_SPI_1_ACT_MOD_Sel,
    [ROM_API_INDEX_GLB_Set_SFlash_IO_PARM]                                     = (uint32_t)GLB_Set_SFlash_IO_PARM,
    [ROM_API_INDEX_GLB_Set_SFlash2_IO_PARM]                                    = (uint32_t)GLB_Set_SFlash2_IO_PARM,
    [ROM_API_INDEX_GLB_Set_Psram_Pad_For_Flash]                                = (uint32_t)GLB_Set_Psram_Pad_For_Flash,
    [ROM_API_INDEX_GLB_Clr_Reset_Reason]                                       = (uint32_t)GLB_Clr_Reset_Reason,
    [ROM_API_INDEX_GLB_Get_Reset_Reason]                                       = (uint32_t)GLB_Get_Reset_Reason,
    [ROM_API_INDEX_GLB_AHB_MCU_Software_Reset]                                 = (uint32_t)GLB_AHB_MCU_Software_Reset,
    [ROM_API_INDEX_GLB_Disrst_Set]                                             = (uint32_t)GLB_Disrst_Set,
    [ROM_API_INDEX_GLB_PER_Clock_Gate]                                         = (uint32_t)GLB_PER_Clock_Gate,
    [ROM_API_INDEX_GLB_PER_Clock_UnGate]                                       = (uint32_t)GLB_PER_Clock_UnGate,
    [ROM_API_INDEX_GLB_PLL_CGEN_Clock_Gate]                                    = (uint32_t)GLB_PLL_CGEN_Clock_Gate,
    [ROM_API_INDEX_GLB_PLL_CGEN_Clock_UnGate]                                  = (uint32_t)GLB_PLL_CGEN_Clock_UnGate,
    [ROM_API_INDEX_GLB_Set_PKA_CLK_Sel]                                        = (uint32_t)GLB_Set_PKA_CLK_Sel,
    [ROM_API_INDEX_GLB_MCU_SW_System_Reset]                                    = (uint32_t)GLB_MCU_SW_System_Reset,
    [ROM_API_INDEX_GLB_SW_System_Reset]                                        = (uint32_t)GLB_SW_System_Reset,
    [ROM_API_INDEX_GLB_SW_CPU_Reset]                                           = (uint32_t)GLB_SW_CPU_Reset,
    [ROM_API_INDEX_GLB_SW_POR_Reset]                                           = (uint32_t)GLB_SW_POR_Reset,
    [ROM_API_INDEX_GLB_Set_Auto_Calc_Xtal_Type]                                = (uint32_t)GLB_Set_Auto_Calc_Xtal_Type,
    [ROM_API_INDEX_GLB_Get_Auto_Calc_Xtal_Type]                                = (uint32_t)GLB_Get_Auto_Calc_Xtal_Type,
    [ROM_API_INDEX_GLB_Set_Flash_Id_Value]                                     = (uint32_t)GLB_Set_Flash_Id_Value,
    [ROM_API_INDEX_GLB_Get_Flash_Id_Value]                                     = (uint32_t)GLB_Get_Flash_Id_Value,
    [ROM_API_INDEX_GLB_Trim_Ldo18ioVoutSel]                                    = (uint32_t)GLB_Trim_Ldo18ioVoutSel,
    [ROM_API_INDEX_GLB_Trim_Ldo18ioVoutTrim]                                   = (uint32_t)GLB_Trim_Ldo18ioVoutTrim,
    [ROM_API_INDEX_GLB_Power_Down_Ldo18ioVout]                                 = (uint32_t)GLB_Power_Down_Ldo18ioVout,
    [ROM_API_INDEX_GLB_Power_Up_Ldo18ioVout]                                   = (uint32_t)GLB_Power_Up_Ldo18ioVout,
    [ROM_API_INDEX_GLB_Set_SRAM_RET]                                           = (uint32_t)GLB_Set_SRAM_RET,
    [ROM_API_INDEX_GLB_Get_SRAM_RET]                                           = (uint32_t)GLB_Get_SRAM_RET,
    [ROM_API_INDEX_GLB_Set_SRAM_SLP]                                           = (uint32_t)GLB_Set_SRAM_SLP,
    [ROM_API_INDEX_GLB_Get_SRAM_SLP]                                           = (uint32_t)GLB_Get_SRAM_SLP,
    [ROM_API_INDEX_GLB_Set_SRAM_PARM]                                          = (uint32_t)GLB_Set_SRAM_PARM,
    [ROM_API_INDEX_GLB_Get_SRAM_PARM]                                          = (uint32_t)GLB_Get_SRAM_PARM,
    [ROM_API_INDEX_GLB_Set_EM_Sel]                                             = (uint32_t)GLB_Set_EM_Sel,
    [ROM_API_INDEX_GLB_Set_PSRAMB_CLK_Sel]                                     = (uint32_t)GLB_Set_PSRAMB_CLK_Sel,
    [ROM_API_INDEX_GLB_Config_WIFIPLL]                                         = (uint32_t)GLB_Config_WIFIPLL,
    [ROM_API_INDEX_GLB_Simple_Set_MCU_System_CLK]                              = (uint32_t)GLB_Simple_Set_MCU_System_CLK,
    [ROM_API_INDEX_GLB_Embedded_Flash_Pad_Enable]                              = (uint32_t)GLB_Embedded_Flash_Pad_Enable,
    [ROM_API_INDEX_GLB_SF_Write_Protect_Enable]                                = (uint32_t)GLB_SF_Write_Protect_Enable,
    [ROM_API_INDEX_GLB_SF_Write_Protect_Disable]                               = (uint32_t)GLB_SF_Write_Protect_Disable,
    [ROM_API_INDEX_GLB_SF_Write_Protect_Set]                                   = (uint32_t)GLB_SF_Write_Protect_Set,
    [ROM_API_INDEX_GLB_Write_SW_Reserved_Register]                             = (uint32_t)GLB_Write_SW_Reserved_Register,
    [ROM_API_INDEX_GLB_Read_SW_Reserved_Register]                              = (uint32_t)GLB_Read_SW_Reserved_Register,
    [ROM_API_INDEX_GLB_Bus_Decoder_Err_Enable]                                 = (uint32_t)GLB_Bus_Decoder_Err_Enable,
    [ROM_API_INDEX_GLB_Bus_Decoder_Err_Disable]                                = (uint32_t)GLB_Bus_Decoder_Err_Disable,
    [ROM_API_INDEX_HBN_Ctrl_Ldo_Aon_Vout_in_HBN]                               = (uint32_t)HBN_Ctrl_Ldo_Aon_Vout_in_HBN,
    [ROM_API_INDEX_HBN_Level_Select]                                           = (uint32_t)HBN_Level_Select,
    [ROM_API_INDEX_HBN_Power_Down_Flash]                                       = (uint32_t)HBN_Power_Down_Flash,
    [ROM_API_INDEX_HBN_Reset]                                                  = (uint32_t)HBN_Reset,
    [ROM_API_INDEX_HBN_Get_POR_OUT_State]                                      = (uint32_t)HBN_Get_POR_OUT_State,
    [ROM_API_INDEX_HBN_Get_BOD_OUT_State]                                      = (uint32_t)HBN_Get_BOD_OUT_State,
    [ROM_API_INDEX_HBN_Set_BOD_Config]                                         = (uint32_t)HBN_Set_BOD_Config,
    [ROM_API_INDEX_HBN_Set_BOD_Cfg]                                            = (uint32_t)HBN_Set_BOD_Cfg,
    [ROM_API_INDEX_HBN_Get_Reset_Event]                                        = (uint32_t)HBN_Get_Reset_Event,
    [ROM_API_INDEX_HBN_Clr_Reset_Event]                                        = (uint32_t)HBN_Clr_Reset_Event,
    [ROM_API_INDEX_HBN_SW_Set_Ldo_Aon_Vout]                                    = (uint32_t)HBN_SW_Set_Ldo_Aon_Vout,
    [ROM_API_INDEX_HBN_SW_Trim_Ldo_Aon_Vout]                                   = (uint32_t)HBN_SW_Trim_Ldo_Aon_Vout,
    [ROM_API_INDEX_HBN_Set_RTC_CLK_Sel]                                        = (uint32_t)HBN_Set_RTC_CLK_Sel,
    [ROM_API_INDEX_HBN_Get_RTC_CLK_Sel]                                        = (uint32_t)HBN_Get_RTC_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_Xtal_Divied_CLK]                                    = (uint32_t)HBN_Set_Xtal_Divied_CLK,
    [ROM_API_INDEX_HBN_32K_Sel]                                                = (uint32_t)HBN_32K_Sel,
    [ROM_API_INDEX_HBN_Set_UART_CLK_Sel]                                       = (uint32_t)HBN_Set_UART_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_GPIO_INT_DET_CLK_Sel]                               = (uint32_t)HBN_Set_GPIO_INT_DET_CLK_Sel,
    [ROM_API_INDEX_HBN_Get_MCU_XCLK_Sel]                                       = (uint32_t)HBN_Get_MCU_XCLK_Sel,
    [ROM_API_INDEX_HBN_Set_MCU_XCLK_Sel]                                       = (uint32_t)HBN_Set_MCU_XCLK_Sel,
    [ROM_API_INDEX_HBN_Get_MCU_Root_CLK_Sel]                                   = (uint32_t)HBN_Get_MCU_Root_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_MCU_Root_CLK_Sel]                                   = (uint32_t)HBN_Set_MCU_Root_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_HRAM_slp]                                           = (uint32_t)HBN_Set_HRAM_slp,
    [ROM_API_INDEX_HBN_Set_HRAM_Ret]                                           = (uint32_t)HBN_Set_HRAM_Ret,
    [ROM_API_INDEX_HBN_SW_Set_Ldo_Soc_Vout]                                    = (uint32_t)HBN_SW_Set_Ldo_Soc_Vout,
    [ROM_API_INDEX_HBN_SW_Trim_Ldo_Soc_Vout]                                   = (uint32_t)HBN_SW_Trim_Ldo_Soc_Vout,
    [ROM_API_INDEX_HBN_Set_Ldo_Soc_Vout_in_Lowpower]                           = (uint32_t)HBN_Set_Ldo_Soc_Vout_in_Lowpower,
    [ROM_API_INDEX_HBN_Trim_Ldo_Soc_Vout_in_Lowpower]                          = (uint32_t)HBN_Trim_Ldo_Soc_Vout_in_Lowpower,
    [ROM_API_INDEX_HBN_Ctrl_Ldo_Soc_Mode_by_HW]                                = (uint32_t)HBN_Ctrl_Ldo_Soc_Mode_by_HW,
    [ROM_API_INDEX_HBN_Set_Ldo_Soc_Mode]                                       = (uint32_t)HBN_Set_Ldo_Soc_Mode,
    [ROM_API_INDEX_HBN_SW_Set_Ldo_Sys_Vout]                                    = (uint32_t)HBN_SW_Set_Ldo_Sys_Vout,
    [ROM_API_INDEX_HBN_SW_Trim_Ldo_Sys_Vout]                                   = (uint32_t)HBN_SW_Trim_Ldo_Sys_Vout,
    [ROM_API_INDEX_HBN_Set_Ldo_Sys_Vout_in_Lowpower]                           = (uint32_t)HBN_Set_Ldo_Sys_Vout_in_Lowpower,
    [ROM_API_INDEX_HBN_Ctrl_Ldo_Sys_Mode_by_HW]                                = (uint32_t)HBN_Ctrl_Ldo_Sys_Mode_by_HW,
    [ROM_API_INDEX_HBN_Set_Ldo_Sys_Mode]                                       = (uint32_t)HBN_Set_Ldo_Sys_Mode,
    [ROM_API_INDEX_HBN_Power_Off_LDO18_IO]                                     = (uint32_t)HBN_Power_Off_LDO18_IO,
    [ROM_API_INDEX_HBN_Power_On_LDO18_IO]                                      = (uint32_t)HBN_Power_On_LDO18_IO,
    [ROM_API_INDEX_HBN_Set_Xtal_32K_Inverter_Amplify_Strength]                 = (uint32_t)HBN_Set_Xtal_32K_Inverter_Amplify_Strength,
    [ROM_API_INDEX_HBN_Set_Xtal_32K_Regulator]                                 = (uint32_t)HBN_Set_Xtal_32K_Regulator,
    [ROM_API_INDEX_HBN_Start_Xtal_32K]                                         = (uint32_t)HBN_Start_Xtal_32K,
    [ROM_API_INDEX_HBN_Clear_Xtal_32K_Fast_Startup]                            = (uint32_t)HBN_Clear_Xtal_32K_Fast_Startup,
    [ROM_API_INDEX_HBN_Get_Xtal_32K_Ready_State]                               = (uint32_t)HBN_Get_Xtal_32K_Ready_State,
    [ROM_API_INDEX_HBN_Power_On_Xtal_32K]                                      = (uint32_t)HBN_Power_On_Xtal_32K,
    [ROM_API_INDEX_HBN_Power_Off_Xtal_32K]                                     = (uint32_t)HBN_Power_Off_Xtal_32K,
    [ROM_API_INDEX_HBN_Keep_On_RC32K]                                          = (uint32_t)HBN_Keep_On_RC32K,
    [ROM_API_INDEX_HBN_Power_Off_RC32K]                                        = (uint32_t)HBN_Power_Off_RC32K,
    [ROM_API_INDEX_HBN_Trim_RC32K]                                             = (uint32_t)HBN_Trim_RC32K,
    [ROM_API_INDEX_HBN_Trim_Ldo_Soc_Vout]                                      = (uint32_t)HBN_Trim_Ldo_Soc_Vout,
    [ROM_API_INDEX_HBN_Trim_Ldo_Aon_Vout]                                      = (uint32_t)HBN_Trim_Ldo_Aon_Vout,
    [ROM_API_INDEX_HBN_Trim_Ldo_Sys_Vout]                                      = (uint32_t)HBN_Trim_Ldo_Sys_Vout,
    [ROM_API_INDEX_HBN_Set_RC32M_Speed]                                        = (uint32_t)HBN_Set_RC32M_Speed,
    [ROM_API_INDEX_HBN_Get_RC32M_Speed]                                        = (uint32_t)HBN_Get_RC32M_Speed,
    [ROM_API_INDEX_HBN_Trim_RC32M]                                             = (uint32_t)HBN_Trim_RC32M,
    [ROM_API_INDEX_HBN_Get_Status_Flag]                                        = (uint32_t)HBN_Get_Status_Flag,
    [ROM_API_INDEX_HBN_Set_Status_Flag]                                        = (uint32_t)HBN_Set_Status_Flag,
    [ROM_API_INDEX_HBN_Get_Wakeup_Addr]                                        = (uint32_t)HBN_Get_Wakeup_Addr,
    [ROM_API_INDEX_HBN_Set_Wakeup_Addr]                                        = (uint32_t)HBN_Set_Wakeup_Addr,
    [ROM_API_INDEX_HBN_Get_Core_Unhalt_Config]                                 = (uint32_t)HBN_Get_Core_Unhalt_Config,
    [ROM_API_INDEX_HBN_Set_Core_Reboot_Config]                                 = (uint32_t)HBN_Set_Core_Reboot_Config,
    [ROM_API_INDEX_HBN_Get_User_Boot_Config]                                   = (uint32_t)HBN_Get_User_Boot_Config,
    [ROM_API_INDEX_HBN_Set_User_Boot_Config]                                   = (uint32_t)HBN_Set_User_Boot_Config,
    [ROM_API_INDEX_HBN_Get_Ldo18io_Power_Delay_Config]                         = (uint32_t)HBN_Get_Ldo18io_Power_Delay_Config,
    [ROM_API_INDEX_HBN_Set_Ldo18io_Power_Delay_Config]                         = (uint32_t)HBN_Set_Ldo18io_Power_Delay_Config,
    [ROM_API_INDEX_HBN_Set_Xtal_Type]                                          = (uint32_t)HBN_Set_Xtal_Type,
    [ROM_API_INDEX_HBN_Get_Xtal_Type]                                          = (uint32_t)HBN_Get_Xtal_Type,
    [ROM_API_INDEX_HBN_Get_Xtal_Value]                                         = (uint32_t)HBN_Get_Xtal_Value,
    [ROM_API_INDEX_HBN_Set_Flash_Power_Delay]                                  = (uint32_t)HBN_Set_Flash_Power_Delay,
    [ROM_API_INDEX_HBN_Get_Flash_Power_Delay]                                  = (uint32_t)HBN_Get_Flash_Power_Delay,
    [ROM_API_INDEX_HBN_Clear_RTC_Counter]                                      = (uint32_t)HBN_Clear_RTC_Counter,
    [ROM_API_INDEX_HBN_Enable_RTC_Counter]                                     = (uint32_t)HBN_Enable_RTC_Counter,
    [ROM_API_INDEX_HBN_Set_RTC_Timer]                                          = (uint32_t)HBN_Set_RTC_Timer,
    [ROM_API_INDEX_HBN_Get_RTC_Timer_Val]                                      = (uint32_t)HBN_Get_RTC_Timer_Val,
    [ROM_API_INDEX_HBN_Clear_RTC_INT]                                          = (uint32_t)HBN_Clear_RTC_INT,
    [ROM_API_INDEX_HBN_GPIO_INT_Enable]                                        = (uint32_t)HBN_GPIO_INT_Enable,
    [ROM_API_INDEX_HBN_GPIO_INT_Disable]                                       = (uint32_t)HBN_GPIO_INT_Disable,
    [ROM_API_INDEX_HBN_Get_INT_State]                                          = (uint32_t)HBN_Get_INT_State,
    [ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode]                                    = (uint32_t)HBN_Get_Pin_Wakeup_Mode,
    [ROM_API_INDEX_HBN_Clear_IRQ]                                              = (uint32_t)HBN_Clear_IRQ,
    [ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg]                                           = (uint32_t)HBN_Hw_Pu_Pd_Cfg,
    [ROM_API_INDEX_HBN_Set_Aon_Pad_IntMode]                                    = (uint32_t)HBN_Set_Aon_Pad_IntMode,
    [ROM_API_INDEX_HBN_Pin_WakeUp_Mask]                                        = (uint32_t)HBN_Pin_WakeUp_Mask,
    [ROM_API_INDEX_HBN_Set_Aon_Pad_IntMask]                                    = (uint32_t)HBN_Set_Aon_Pad_IntMask,
    [ROM_API_INDEX_HBN_Aon_Pad_Ctrl]                                           = (uint32_t)HBN_Aon_Pad_Ctrl,
    [ROM_API_INDEX_HBN_Aon_Pad_Cfg]                                            = (uint32_t)HBN_Aon_Pad_Cfg,
    [ROM_API_INDEX_HBN_Aon_Pad_Cfg_Set]                                        = (uint32_t)HBN_Aon_Pad_Cfg_Set,
    [ROM_API_INDEX_HBN_Enable_BOD_IRQ]                                         = (uint32_t)HBN_Enable_BOD_IRQ,
    [ROM_API_INDEX_HBN_Disable_BOD_IRQ]                                        = (uint32_t)HBN_Disable_BOD_IRQ,
    [ROM_API_INDEX_HBN_Aon_Pad_WakeUpCfg]                                      = (uint32_t)HBN_Aon_Pad_WakeUpCfg,
    [ROM_API_INDEX_HBN_Wdg_Init]                                               = (uint32_t)HBN_Wdg_Init,
    [ROM_API_INDEX_HBN_Wdg_Start]                                              = (uint32_t)HBN_Wdg_Start,
    [ROM_API_INDEX_HBN_Wdg_Stop]                                               = (uint32_t)HBN_Wdg_Stop,
    [ROM_API_INDEX_HBN_Wdg_Get_Countervalue]                                   = (uint32_t)HBN_Wdg_Get_Countervalue,
    [ROM_API_INDEX_HBN_Wdg_Set_Countervalue]                                   = (uint32_t)HBN_Wdg_Set_Countervalue,
    [ROM_API_INDEX_HBN_Wdg_Reset_Countervalue]                                 = (uint32_t)HBN_Wdg_Reset_Countervalue,
    [ROM_API_INDEX_HBN_Wdg_Compint_Clear]                                      = (uint32_t)HBN_Wdg_Compint_Clear,
    [ROM_API_INDEX_HBN_Get_Version]                                            = (uint32_t)HBN_Get_Version,
    [ROM_API_INDEX_HBN_Set_Version]                                            = (uint32_t)HBN_Set_Version,
    [ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask]                                   = (uint32_t)PDS_Set_GPIO_Pad_IntMask,
    [ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode]                                   = (uint32_t)PDS_Set_GPIO_Pad_IntMode,
    [ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr]                                    = (uint32_t)PDS_Set_GPIO_Pad_IntClr,
    [ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus]                                 = (uint32_t)PDS_Get_GPIO_Pad_IntStatus,
    [ROM_API_INDEX_PDS_Set_All_GPIO_Pad_IntClr]                                = (uint32_t)PDS_Set_All_GPIO_Pad_IntClr,
    [ROM_API_INDEX_PDS_Get_All_GPIO_Pad_IntStatus]                             = (uint32_t)PDS_Get_All_GPIO_Pad_IntStatus,
    [ROM_API_INDEX_PDS_Disable_ALL_GPIO_Keep]                                  = (uint32_t)PDS_Disable_ALL_GPIO_Keep,
    [ROM_API_INDEX_PDS_Enable_GPIO_Keep]                                       = (uint32_t)PDS_Enable_GPIO_Keep,
    [ROM_API_INDEX_PDS_Set_PDS_GPIO_INT_DET_CLK_Sel]                           = (uint32_t)PDS_Set_PDS_GPIO_INT_DET_CLK_Sel,
    [ROM_API_INDEX_PDS_Enable_WRAM_Access_Direct]                              = (uint32_t)PDS_Enable_WRAM_Access_Direct,
    [ROM_API_INDEX_PDS_Disable_WRAM_Access_Direct]                             = (uint32_t)PDS_Disable_WRAM_Access_Direct,
    [ROM_API_INDEX_PDS_Set_Ldo_Soc_Enter_lp_delay]                             = (uint32_t)PDS_Set_Ldo_Soc_Enter_lp_delay,
    [ROM_API_INDEX_PDS_Set_Ldo_Soc_Exit_lp_delay]                              = (uint32_t)PDS_Set_Ldo_Soc_Exit_lp_delay,
    [ROM_API_INDEX_PDS_Set_Ldo_Sys_Enter_lp_delay]                             = (uint32_t)PDS_Set_Ldo_Sys_Enter_lp_delay,
    [ROM_API_INDEX_PDS_Set_Ldo_Sys_Exit_lp_delay]                              = (uint32_t)PDS_Set_Ldo_Sys_Exit_lp_delay,
    [ROM_API_INDEX_PDS_Set_MCU0_Clock_Enable]                                  = (uint32_t)PDS_Set_MCU0_Clock_Enable,
    [ROM_API_INDEX_PDS_Set_MCU0_Clock_Disable]                                 = (uint32_t)PDS_Set_MCU0_Clock_Disable,
    [ROM_API_INDEX_PDS_Pu_PLL_Enable]                                          = (uint32_t)PDS_Pu_PLL_Enable,
    [ROM_API_INDEX_PDS_Pu_PLL_Disable]                                         = (uint32_t)PDS_Pu_PLL_Disable,
    [ROM_API_INDEX_PDS_Enable]                                                 = (uint32_t)PDS_Enable,
    [ROM_API_INDEX_PDS_Force_Config]                                           = (uint32_t)PDS_Force_Config,
    [ROM_API_INDEX_PDS_RAM_Config]                                             = (uint32_t)PDS_RAM_Config,
    [ROM_API_INDEX_PDS_Set_All_WRAM_Retention]                                 = (uint32_t)PDS_Set_All_WRAM_Retention,
    [ROM_API_INDEX_PDS_Set_All_OCRAM_Retention]                                = (uint32_t)PDS_Set_All_OCRAM_Retention,
    [ROM_API_INDEX_PDS_Set_All_WRAM_Sleep]                                     = (uint32_t)PDS_Set_All_WRAM_Sleep,
    [ROM_API_INDEX_PDS_Set_All_OCRAM_Sleep]                                    = (uint32_t)PDS_Set_All_OCRAM_Sleep,
    [ROM_API_INDEX_PDS_Disable_Ctrl_Ram]                                       = (uint32_t)PDS_Disable_Ctrl_Ram,
    [ROM_API_INDEX_PDS_Default_Level_Config]                                   = (uint32_t)PDS_Default_Level_Config,
    [ROM_API_INDEX_PDS_IntMask]                                                = (uint32_t)PDS_IntMask,
    [ROM_API_INDEX_PDS_Get_IntStatus]                                          = (uint32_t)PDS_Get_IntStatus,
    [ROM_API_INDEX_PDS_IntClear]                                               = (uint32_t)PDS_IntClear,
    [ROM_API_INDEX_PDS_Set_Wakeup_Src_IntMask]                                 = (uint32_t)PDS_Set_Wakeup_Src_IntMask,
    [ROM_API_INDEX_PDS_Get_Wakeup_Src]                                         = (uint32_t)PDS_Get_Wakeup_Src,
    [ROM_API_INDEX_PDS_Get_All_Wakeup_Src]                                     = (uint32_t)PDS_Get_All_Wakeup_Src,
    [ROM_API_INDEX_PDS_Mask_All_Wakeup_Src]                                    = (uint32_t)PDS_Mask_All_Wakeup_Src,
    [ROM_API_INDEX_PDS_Power_Off_WB]                                           = (uint32_t)PDS_Power_Off_WB,
    [ROM_API_INDEX_PDS_Power_On_WB]                                            = (uint32_t)PDS_Power_On_WB,
    [ROM_API_INDEX_PDS_Set_USB_Suspend]                                        = (uint32_t)PDS_Set_USB_Suspend,
    [ROM_API_INDEX_PDS_Set_USB_Resume]                                         = (uint32_t)PDS_Set_USB_Resume,
    [ROM_API_INDEX_PDS_Trim_USB20]                                             = (uint32_t)PDS_Trim_USB20,

    [ROM_API_INDEX_FUNC_EMPTY_START ... ROM_API_INDEX_FUNC_EMPTY_END] = 0xdeedbeef,
};


/*@} end of group ROMDRIVER_Global_Variables */

/** @defgroup ROMDRIVER_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group ROMDRIVER_Private_FunctionDeclaration */

/** @defgroup ROMDRIVER_Private_Functions
 *  @{
 */

/*@} end of group ROMDRIVER_Private_Functions */

/** @defgroup ROMDRIVER_Public_Functions
 *  @{
 */

/*@} end of group ROMDRIVER_Public_Functions */

/*@} end of group ROMDRIVER_COMMON */

/*@} end of group BL616_Periph_Driver */
