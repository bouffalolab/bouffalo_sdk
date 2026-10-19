/**
  ******************************************************************************
  * @file    bl702l_romdriver.c
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
  
#include "bl702l_romdriver.h"
#include <string.h>

/** @addtogroup  BL702L_Periph_Driver
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
    0x07020001,
    0x00000000,
    0x00000000,
    0x00000000,


    [ROM_API_INDEX_AON_Power_On_MBG]                       = (uint32_t)AON_Power_On_MBG,
    [ROM_API_INDEX_AON_Power_Off_MBG]                      = (uint32_t)AON_Power_Off_MBG,
    [ROM_API_INDEX_AON_Power_On_XTAL]                      = (uint32_t)AON_Power_On_XTAL,
    [ROM_API_INDEX_AON_Set_Xtal_CapCode]                   = (uint32_t)AON_Set_Xtal_CapCode,
    [ROM_API_INDEX_AON_Get_Xtal_CapCode]                   = (uint32_t)AON_Get_Xtal_CapCode,
    [ROM_API_INDEX_AON_Set_Xtal_CapCode_Extra]             = (uint32_t)AON_Set_Xtal_CapCode_Extra,
    [ROM_API_INDEX_AON_Power_Off_XTAL]                     = (uint32_t)AON_Power_Off_XTAL,
    [ROM_API_INDEX_AON_Power_On_BG]                        = (uint32_t)AON_Power_On_BG,
    [ROM_API_INDEX_AON_Power_Off_BG]                       = (uint32_t)AON_Power_Off_BG,
    [ROM_API_INDEX_AON_Power_On_LDO11_SOC]                 = (uint32_t)AON_Power_On_LDO11_SOC,
    [ROM_API_INDEX_AON_Power_Off_LDO11_SOC]                = (uint32_t)AON_Power_Off_LDO11_SOC,
    [ROM_API_INDEX_AON_Power_On_SFReg]                     = (uint32_t)AON_Power_On_SFReg,
    [ROM_API_INDEX_AON_Power_Off_SFReg]                    = (uint32_t)AON_Power_Off_SFReg,
    [ROM_API_INDEX_AON_Set_LDO11_SOC_Sstart_Delay]         = (uint32_t)AON_Set_LDO11_SOC_Sstart_Delay,
    [ROM_API_INDEX_AON_Set_DCDC14_Top_0]                   = (uint32_t)AON_Set_DCDC14_Top_0,
    [ROM_API_INDEX_AON_Trim_Ldo11socVoutTrim]              = (uint32_t)AON_Trim_Ldo11socVoutTrim,
    [ROM_API_INDEX_AON_Trim_Ldo14VoutTrim]                 = (uint32_t)AON_Trim_Ldo14VoutTrim,
    [ROM_API_INDEX_AON_Trim_Dcdc14VoutTrim]                = (uint32_t)AON_Trim_Dcdc14VoutTrim,

    [ROM_API_INDEX_Clock_System_Clock_Get]                 = (uint32_t)Clock_System_Clock_Get,
    [ROM_API_INDEX_Clock_Peripheral_Clock_Get]             = (uint32_t)Clock_Peripheral_Clock_Get,

    [ROM_API_INDEX_SystemCoreClockGet]                     = (uint32_t)SystemCoreClockGet,

    [ROM_API_INDEX_CPU_Get_MTimer_Clock]                   = (uint32_t)CPU_Get_MTimer_Clock,
    [ROM_API_INDEX_CPU_Get_MTimer_Counter]                 = (uint32_t)CPU_Get_MTimer_Counter,
    [ROM_API_INDEX_CPU_Get_CPU_Cycle]                      = (uint32_t)CPU_Get_CPU_Cycle,
    [ROM_API_INDEX_CPU_Get_MTimer_US]                      = (uint32_t)CPU_Get_MTimer_US,
    [ROM_API_INDEX_CPU_Get_MTimer_MS]                      = (uint32_t)CPU_Get_MTimer_MS,
    [ROM_API_INDEX_CPU_MTimer_Delay_US]                    = (uint32_t)CPU_MTimer_Delay_US,
    [ROM_API_INDEX_CPU_MTimer_Delay_MS]                    = (uint32_t)CPU_MTimer_Delay_MS,

    [ROM_API_INDEX_BL702L_Delay_US]                        = (uint32_t)BL702L_Delay_US,
    [ROM_API_INDEX_BL702L_Delay_MS]                        = (uint32_t)BL702L_Delay_MS,

    [ROM_API_INDEX_EF_Ctrl_Get_Trim_Parity]                = (uint32_t)EF_Ctrl_Get_Trim_Parity,
    [ROM_API_INDEX_EF_Ctrl_Read_Common_Trim]               = (uint32_t)EF_Ctrl_Read_Common_Trim,
    [ROM_API_INDEX_EF_Ctrl_Write_Common_Trim]              = (uint32_t)EF_Ctrl_Write_Common_Trim,
    [ROM_API_INDEX_EF_Ctrl_Is_MAC_Address_Slot_Empty]      = (uint32_t)EF_Ctrl_Is_MAC_Address_Slot_Empty,
    [ROM_API_INDEX_EF_Ctrl_Write_MAC_Address_Opt]          = (uint32_t)EF_Ctrl_Write_MAC_Address_Opt,
    [ROM_API_INDEX_EF_Ctrl_Read_MAC_Address_Opt]           = (uint32_t)EF_Ctrl_Read_MAC_Address_Opt,
    [ROM_API_INDEX_EF_Ctrl_Sw_AHB_Clk_0]                   = (uint32_t)EF_Ctrl_Sw_AHB_Clk_0,
    [ROM_API_INDEX_EF_Ctrl_Program_Efuse_0]                = (uint32_t)EF_Ctrl_Program_Efuse_0,
    [ROM_API_INDEX_EF_Ctrl_Load_Efuse_R0]                  = (uint32_t)EF_Ctrl_Load_Efuse_R0,
    [ROM_API_INDEX_EF_Ctrl_Busy]                           = (uint32_t)EF_Ctrl_Busy,
    [ROM_API_INDEX_EF_Ctrl_AutoLoad_Done]                  = (uint32_t)EF_Ctrl_AutoLoad_Done,
    [ROM_API_INDEX_EF_Ctrl_Write_Dbg_Pwd]                  = (uint32_t)EF_Ctrl_Write_Dbg_Pwd,
    [ROM_API_INDEX_EF_Ctrl_Read_Dbg_Pwd]                   = (uint32_t)EF_Ctrl_Read_Dbg_Pwd,
    [ROM_API_INDEX_EF_Ctrl_Readlock_Dbg_Pwd]               = (uint32_t)EF_Ctrl_Readlock_Dbg_Pwd,
    [ROM_API_INDEX_EF_Ctrl_Writelock_Dbg_Pwd]              = (uint32_t)EF_Ctrl_Writelock_Dbg_Pwd,
    [ROM_API_INDEX_EF_Ctrl_Write_Secure_Cfg]               = (uint32_t)EF_Ctrl_Write_Secure_Cfg,
    [ROM_API_INDEX_EF_Ctrl_Read_Secure_Cfg]                = (uint32_t)EF_Ctrl_Read_Secure_Cfg,
    [ROM_API_INDEX_EF_Ctrl_Write_Secure_Boot]              = (uint32_t)EF_Ctrl_Write_Secure_Boot,
    [ROM_API_INDEX_EF_Ctrl_Read_Secure_Boot]               = (uint32_t)EF_Ctrl_Read_Secure_Boot,
    [ROM_API_INDEX_EF_Ctrl_Write_Sw_Usage]                 = (uint32_t)EF_Ctrl_Write_Sw_Usage,
    [ROM_API_INDEX_EF_Ctrl_Read_Sw_Usage]                  = (uint32_t)EF_Ctrl_Read_Sw_Usage,
    [ROM_API_INDEX_EF_Ctrl_Writelock_Sw_Usage]             = (uint32_t)EF_Ctrl_Writelock_Sw_Usage,
    [ROM_API_INDEX_EF_Ctrl_Write_MAC_Address]              = (uint32_t)EF_Ctrl_Write_MAC_Address,
    [ROM_API_INDEX_EF_Ctrl_Get_Byte_Zero_Cnt]              = (uint32_t)EF_Ctrl_Get_Byte_Zero_Cnt,
    [ROM_API_INDEX_EF_Ctrl_Is_All_Bits_Zero]               = (uint32_t)EF_Ctrl_Is_All_Bits_Zero,
    [ROM_API_INDEX_EF_Ctrl_Read_MAC_Address]               = (uint32_t)EF_Ctrl_Read_MAC_Address,
    [ROM_API_INDEX_EF_Ctrl_Writelock_MAC_Address]          = (uint32_t)EF_Ctrl_Writelock_MAC_Address,
    [ROM_API_INDEX_EF_Ctrl_Read_Chip_ID]                   = (uint32_t)EF_Ctrl_Read_Chip_ID,
    [ROM_API_INDEX_EF_Ctrl_Read_Device_Info]               = (uint32_t)EF_Ctrl_Read_Device_Info,
    [ROM_API_INDEX_EF_Ctrl_Is_CapCode_Empty]               = (uint32_t)EF_Ctrl_Is_CapCode_Empty,
    [ROM_API_INDEX_EF_Ctrl_Write_CapCode_Opt]              = (uint32_t)EF_Ctrl_Write_CapCode_Opt,
    [ROM_API_INDEX_EF_Ctrl_Read_CapCode_Opt]               = (uint32_t)EF_Ctrl_Read_CapCode_Opt,
    [ROM_API_INDEX_EF_Ctrl_Is_PowerOffset_Slot_Empty]      = (uint32_t)EF_Ctrl_Is_PowerOffset_Slot_Empty,
    [ROM_API_INDEX_EF_Ctrl_Write_PowerOffset_Opt]          = (uint32_t)EF_Ctrl_Write_PowerOffset_Opt,
    [ROM_API_INDEX_EF_Ctrl_Read_PowerOffset_Opt]           = (uint32_t)EF_Ctrl_Read_PowerOffset_Opt,
    [ROM_API_INDEX_EF_Ctrl_Write_AES_Key]                  = (uint32_t)EF_Ctrl_Write_AES_Key,
    [ROM_API_INDEX_EF_Ctrl_Read_AES_Key]                   = (uint32_t)EF_Ctrl_Read_AES_Key,
    [ROM_API_INDEX_EF_Ctrl_Writelock_AES_Key]              = (uint32_t)EF_Ctrl_Writelock_AES_Key,
    [ROM_API_INDEX_EF_Ctrl_Readlock_AES_Key]               = (uint32_t)EF_Ctrl_Readlock_AES_Key,
    [ROM_API_INDEX_EF_Ctrl_Program_Direct_R0]              = (uint32_t)EF_Ctrl_Program_Direct_R0,
    [ROM_API_INDEX_EF_Ctrl_Read_Direct_R0]                 = (uint32_t)EF_Ctrl_Read_Direct_R0,
    [ROM_API_INDEX_EF_Ctrl_Clear]                          = (uint32_t)EF_Ctrl_Clear,
    [ROM_API_INDEX_EF_Ctrl_Crc_Enable]                     = (uint32_t)EF_Ctrl_Crc_Enable,
    [ROM_API_INDEX_EF_Ctrl_Crc_Is_Busy]                    = (uint32_t)EF_Ctrl_Crc_Is_Busy,
    [ROM_API_INDEX_EF_Ctrl_Crc_Set_Golden]                 = (uint32_t)EF_Ctrl_Crc_Set_Golden,
    [ROM_API_INDEX_EF_Ctrl_Crc_Result]                     = (uint32_t)EF_Ctrl_Crc_Result,

    [ROM_API_INDEX_GLB_Get_Root_CLK_Sel]                   = (uint32_t)GLB_Get_Root_CLK_Sel,
    [ROM_API_INDEX_GLB_Set_System_CLK_Div]                 = (uint32_t)GLB_Set_System_CLK_Div,
    [ROM_API_INDEX_GLB_Get_BCLK_Div]                       = (uint32_t)GLB_Get_BCLK_Div,
    [ROM_API_INDEX_GLB_Get_HCLK_Div]                       = (uint32_t)GLB_Get_HCLK_Div,
    [ROM_API_INDEX_GLB_Set_System_CLK]                     = (uint32_t)GLB_Set_System_CLK,

    [ROM_API_INDEX_System_Core_Clock_Update_From_RC32M]    = (uint32_t)System_Core_Clock_Update_From_RC32M,

    [ROM_API_INDEX_GLB_Set_MAC154_ZIGBEE_CLK]              = (uint32_t)GLB_Set_MAC154_ZIGBEE_CLK,
    [ROM_API_INDEX_GLB_Set_BLE_CLK]                        = (uint32_t)GLB_Set_BLE_CLK,
    [ROM_API_INDEX_GLB_Set_AUDIO_CLK]                      = (uint32_t)GLB_Set_AUDIO_CLK,
    [ROM_API_INDEX_GLB_Set_KYS_CLK]                        = (uint32_t)GLB_Set_KYS_CLK,
    [ROM_API_INDEX_GLB_Set_DMA_CLK]                        = (uint32_t)GLB_Set_DMA_CLK,
    [ROM_API_INDEX_GLB_Set_IR_CLK]                         = (uint32_t)GLB_Set_IR_CLK,
    [ROM_API_INDEX_GLB_Set_SF_CLK]                         = (uint32_t)GLB_Set_SF_CLK,
    [ROM_API_INDEX_GLB_Set_UART_CLK]                       = (uint32_t)GLB_Set_UART_CLK,
    [ROM_API_INDEX_GLB_Sel_TMR_GPIO_Clock]                 = (uint32_t)GLB_Sel_TMR_GPIO_Clock,
    [ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Sel]             = (uint32_t)GLB_Set_Chip_Out_0_CLK_Sel,
    [ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Sel]             = (uint32_t)GLB_Set_Chip_Out_1_CLK_Sel,
    [ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Enable]          = (uint32_t)GLB_Set_Chip_Out_0_CLK_Enable,
    [ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Enable]          = (uint32_t)GLB_Set_Chip_Out_1_CLK_Enable,
    [ROM_API_INDEX_GLB_Set_I2C_CLK]                        = (uint32_t)GLB_Set_I2C_CLK,
    [ROM_API_INDEX_GLB_Set_SPI_CLK]                        = (uint32_t)GLB_Set_SPI_CLK,
    [ROM_API_INDEX_GLB_Set_PKA_CLK_Sel]                    = (uint32_t)GLB_Set_PKA_CLK_Sel,
    [ROM_API_INDEX_GLB_SW_System_Reset]                    = (uint32_t)GLB_SW_System_Reset,
    [ROM_API_INDEX_GLB_SW_CPU_Reset]                       = (uint32_t)GLB_SW_CPU_Reset,
    [ROM_API_INDEX_GLB_SW_POR_Reset]                       = (uint32_t)GLB_SW_POR_Reset,
    [ROM_API_INDEX_GLB_AHB_MCU_Software_Reset]             = (uint32_t)GLB_AHB_MCU_Software_Reset,
    [ROM_API_INDEX_GLB_Disrst_Set]                         = (uint32_t)GLB_Disrst_Set,
    [ROM_API_INDEX_GLB_MAC154_ZIGBEE_Reset]                = (uint32_t)GLB_MAC154_ZIGBEE_Reset,
    [ROM_API_INDEX_GLB_BLE_Reset]                          = (uint32_t)GLB_BLE_Reset,
    [ROM_API_INDEX_GLB_PER_Clock_Gate]                     = (uint32_t)GLB_PER_Clock_Gate,
    [ROM_API_INDEX_GLB_PER_Clock_UnGate]                   = (uint32_t)GLB_PER_Clock_UnGate,
    [ROM_API_INDEX_GLB_Set_RTC_Mode]                       = (uint32_t)GLB_Set_RTC_Mode,
    [ROM_API_INDEX_GLB_BMX_Init]                           = (uint32_t)GLB_BMX_Init,
    [ROM_API_INDEX_GLB_BMX_Addr_Monitor_Enable]            = (uint32_t)GLB_BMX_Addr_Monitor_Enable,
    [ROM_API_INDEX_GLB_BMX_Addr_Monitor_Disable]           = (uint32_t)GLB_BMX_Addr_Monitor_Disable,
    [ROM_API_INDEX_GLB_BMX_BusErrResponse_Enable]          = (uint32_t)GLB_BMX_BusErrResponse_Enable,
    [ROM_API_INDEX_GLB_BMX_BusErrResponse_Disable]         = (uint32_t)GLB_BMX_BusErrResponse_Disable,
    [ROM_API_INDEX_GLB_BMX_Get_Status]                     = (uint32_t)GLB_BMX_Get_Status,
    [ROM_API_INDEX_GLB_BMX_Get_Err_Addr]                   = (uint32_t)GLB_BMX_Get_Err_Addr,
    [ROM_API_INDEX_GLB_BMX_BusErrClr_Set]                  = (uint32_t)GLB_BMX_BusErrClr_Set,
    [ROM_API_INDEX_GLB_Set_SRAM_PARM]                      = (uint32_t)GLB_Set_SRAM_PARM,
    [ROM_API_INDEX_GLB_Get_SRAM_PARM]                      = (uint32_t)GLB_Get_SRAM_PARM,
    [ROM_API_INDEX_GLB_Set_OCRAM_PARM]                     = (uint32_t)GLB_Set_OCRAM_PARM,
    [ROM_API_INDEX_GLB_Get_OCRAM_PARM]                     = (uint32_t)GLB_Get_OCRAM_PARM,
    [ROM_API_INDEX_GLB_Set_EM_Sel]                         = (uint32_t)GLB_Set_EM_Sel,
    [ROM_API_INDEX_GLB_Set_Kys_Drv_Col]                    = (uint32_t)GLB_Set_Kys_Drv_Col,
    [ROM_API_INDEX_GLB_GPIO_O_Latch_Mode_Set]              = (uint32_t)GLB_GPIO_O_Latch_Mode_Set,
    [ROM_API_INDEX_GLB_JTAG_Sig_Swap_Set]                  = (uint32_t)GLB_JTAG_Sig_Swap_Set,
    [ROM_API_INDEX_GLB_CCI_Use_IO_0_1_2_7]                 = (uint32_t)GLB_CCI_Use_IO_0_1_2_7,
    [ROM_API_INDEX_GLB_CCI_Use_Jtag_Pin]                   = (uint32_t)GLB_CCI_Use_Jtag_Pin,
    [ROM_API_INDEX_GLB_Swap_SPI_0_MOSI_With_MISO]          = (uint32_t)GLB_Swap_SPI_0_MOSI_With_MISO,
    [ROM_API_INDEX_GLB_Set_SPI_0_ACT_MOD_Sel]              = (uint32_t)GLB_Set_SPI_0_ACT_MOD_Sel,
    [ROM_API_INDEX_GLB_Set_Flash_Scenario]                 = (uint32_t)GLB_Set_Flash_Scenario,
    [ROM_API_INDEX_GLB_Set_Embedded_FLash_IO_PARM]         = (uint32_t)GLB_Set_Embedded_FLash_IO_PARM,
    [ROM_API_INDEX_GLB_Set_MTimer_CLK]                     = (uint32_t)GLB_Set_MTimer_CLK,
    [ROM_API_INDEX_GLB_Set_ADC_CLK]                        = (uint32_t)GLB_Set_ADC_CLK,
    [ROM_API_INDEX_GLB_Set_DIG_32K_CLK]                    = (uint32_t)GLB_Set_DIG_32K_CLK,
    [ROM_API_INDEX_GLB_SW_BLE_WAKEUP_REQ_Set]              = (uint32_t)GLB_SW_BLE_WAKEUP_REQ_Set,
    [ROM_API_INDEX_GLB_UART_Fun_Sel]                       = (uint32_t)GLB_UART_Fun_Sel,
    [ROM_API_INDEX_GLB_Power_Off_DLL]                      = (uint32_t)GLB_Power_Off_DLL,
    [ROM_API_INDEX_GLB_Power_On_DLL]                       = (uint32_t)GLB_Power_On_DLL,
    [ROM_API_INDEX_GLB_Enable_DLL_All_Clks]                = (uint32_t)GLB_Enable_DLL_All_Clks,
    [ROM_API_INDEX_GLB_Enable_DLL_Clk]                     = (uint32_t)GLB_Enable_DLL_Clk,
    [ROM_API_INDEX_GLB_Disable_DLL_All_Clks]               = (uint32_t)GLB_Disable_DLL_All_Clks,
    [ROM_API_INDEX_GLB_Disable_DLL_Clk]                    = (uint32_t)GLB_Disable_DLL_Clk,
    [ROM_API_INDEX_GLB_Set_Flash_Id_Value]                 = (uint32_t)GLB_Set_Flash_Id_Value,
    [ROM_API_INDEX_GLB_Get_Flash_Id_Value]                 = (uint32_t)GLB_Get_Flash_Id_Value,
    [ROM_API_INDEX_GLB_Trim_RC32M]                         = (uint32_t)GLB_Trim_RC32M,
    [ROM_API_INDEX_GLB_Set_Xtal_Cnt32k_Process]            = (uint32_t)GLB_Set_Xtal_Cnt32k_Process,
    [ROM_API_INDEX_GLB_Clear_Xtal_Cnt32k_Done]             = (uint32_t)GLB_Clear_Xtal_Cnt32k_Done,
    [ROM_API_INDEX_GLB_RC32K_Deg_Start]                    = (uint32_t)GLB_RC32K_Deg_Start,
    [ROM_API_INDEX_GLB_RC32K_Deg_End]                      = (uint32_t)GLB_RC32K_Deg_End,
    [ROM_API_INDEX_GLB_RC32K_Deg_Enable]                   = (uint32_t)GLB_RC32K_Deg_Enable,
    [ROM_API_INDEX_GLB_Xtal_Deg_Cnt_Limit_Set]             = (uint32_t)GLB_Xtal_Deg_Cnt_Limit_Set,
    [ROM_API_INDEX_GLB_IR_LED_Driver_Enable]               = (uint32_t)GLB_IR_LED_Driver_Enable,
    [ROM_API_INDEX_GLB_IR_LED_Driver_Disable]              = (uint32_t)GLB_IR_LED_Driver_Disable,
    [ROM_API_INDEX_GLB_IR_LED_Driver_Output_Enable]        = (uint32_t)GLB_IR_LED_Driver_Output_Enable,
    [ROM_API_INDEX_GLB_IR_LED_Driver_Output_Disable]       = (uint32_t)GLB_IR_LED_Driver_Output_Disable,
    [ROM_API_INDEX_GLB_IR_LED_Driver_Ibias]                = (uint32_t)GLB_IR_LED_Driver_Ibias,
    [ROM_API_INDEX_GLB_GPIO_Init]                          = (uint32_t)GLB_GPIO_Init,
    [ROM_API_INDEX_GLB_GPIO_Func_Init]                     = (uint32_t)GLB_GPIO_Func_Init,
    [ROM_API_INDEX_GLB_GPIO_OUTPUT_Mode_Set]               = (uint32_t)GLB_GPIO_OUTPUT_Mode_Set,
    [ROM_API_INDEX_GLB_GPIO_INPUT_Enable]                  = (uint32_t)GLB_GPIO_INPUT_Enable,
    [ROM_API_INDEX_GLB_GPIO_INPUT_Disable]                 = (uint32_t)GLB_GPIO_INPUT_Disable,
    [ROM_API_INDEX_GLB_GPIO_OUTPUT_Enable]                 = (uint32_t)GLB_GPIO_OUTPUT_Enable,
    [ROM_API_INDEX_GLB_GPIO_OUTPUT_Disable]                = (uint32_t)GLB_GPIO_OUTPUT_Disable,
    [ROM_API_INDEX_GLB_GPIO_Set_HZ]                        = (uint32_t)GLB_GPIO_Set_HZ,
    [ROM_API_INDEX_GLB_GPIO_Get_Fun]                       = (uint32_t)GLB_GPIO_Get_Fun,
    [ROM_API_INDEX_GLB_GPIO_Write]                         = (uint32_t)GLB_GPIO_Write,
    [ROM_API_INDEX_GLB_GPIO_Read]                          = (uint32_t)GLB_GPIO_Read,
    [ROM_API_INDEX_GLB_GPIO_Set]                           = (uint32_t)GLB_GPIO_Set,
    [ROM_API_INDEX_GLB_GPIO_Clr]                           = (uint32_t)GLB_GPIO_Clr,
    [ROM_API_INDEX_GLB_GPIO_IntMask]                       = (uint32_t)GLB_GPIO_IntMask,
    [ROM_API_INDEX_GLB_Clr_GPIO_IntStatus]                 = (uint32_t)GLB_Clr_GPIO_IntStatus,
    [ROM_API_INDEX_GLB_Get_GPIO_IntStatus]                 = (uint32_t)GLB_Get_GPIO_IntStatus,
    [ROM_API_INDEX_GLB_GPIO_Int_Init]                      = (uint32_t)GLB_GPIO_Int_Init,

    [ROM_API_INDEX_HBN_Mode_Enter]                         = (uint32_t)HBN_Mode_Enter,
    [ROM_API_INDEX_HBN_GPIO_Wakeup_Set]                    = (uint32_t)HBN_GPIO_Wakeup_Set,
    [ROM_API_INDEX_HBN_Power_Down_Flash]                   = (uint32_t)HBN_Power_Down_Flash,
    [ROM_API_INDEX_HBN_Enable]                             = (uint32_t)HBN_Enable,
    [ROM_API_INDEX_HBN_Reset]                              = (uint32_t)HBN_Reset,
    [ROM_API_INDEX_HBN_App_Reset]                          = (uint32_t)HBN_App_Reset,
    [ROM_API_INDEX_HBN_Disable]                            = (uint32_t)HBN_Disable,
    [ROM_API_INDEX_HBN_Get_BOR_OUT_State]                  = (uint32_t)HBN_Get_BOR_OUT_State,
    [ROM_API_INDEX_HBN_Set_BOR_Config]                     = (uint32_t)HBN_Set_BOR_Config,
    [ROM_API_INDEX_HBN_Set_Ldo11_Aon_Vout]                 = (uint32_t)HBN_Set_Ldo11_Aon_Vout,
    [ROM_API_INDEX_HBN_Set_Ldo11_Soc_Vout]                 = (uint32_t)HBN_Set_Ldo11_Soc_Vout,
    [ROM_API_INDEX_HBN_Set_Ldo11_All_Vout]                 = (uint32_t)HBN_Set_Ldo11_All_Vout,
    [ROM_API_INDEX_HBN_32K_Sel]                            = (uint32_t)HBN_32K_Sel,
    [ROM_API_INDEX_HBN_Set_UART_CLK_Sel]                   = (uint32_t)HBN_Set_UART_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_XCLK_CLK_Sel]                   = (uint32_t)HBN_Set_XCLK_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_ROOT_CLK_Sel]                   = (uint32_t)HBN_Set_ROOT_CLK_Sel,
    [ROM_API_INDEX_HBN_Set_HRAM_slp]                       = (uint32_t)HBN_Set_HRAM_slp,
    [ROM_API_INDEX_HBN_Set_HRAM_Ret]                       = (uint32_t)HBN_Set_HRAM_Ret,
    [ROM_API_INDEX_HBN_Power_On_Xtal_32K]                  = (uint32_t)HBN_Power_On_Xtal_32K,
    [ROM_API_INDEX_HBN_Power_Off_Xtal_32K]                 = (uint32_t)HBN_Power_Off_Xtal_32K,
    [ROM_API_INDEX_HBN_Power_On_RC32K]                     = (uint32_t)HBN_Power_On_RC32K,
    [ROM_API_INDEX_HBN_Power_Off_RC32K]                    = (uint32_t)HBN_Power_Off_RC32K,
    [ROM_API_INDEX_HBN_Trim_RC32K]                         = (uint32_t)HBN_Trim_RC32K,
    [ROM_API_INDEX_HBN_Get_Status_Flag]                    = (uint32_t)HBN_Get_Status_Flag,
    [ROM_API_INDEX_HBN_Set_Status_Flag]                    = (uint32_t)HBN_Set_Status_Flag,
    [ROM_API_INDEX_HBN_Get_Wakeup_Addr]                    = (uint32_t)HBN_Get_Wakeup_Addr,
    [ROM_API_INDEX_HBN_Set_Wakeup_Addr]                    = (uint32_t)HBN_Set_Wakeup_Addr,
    [ROM_API_INDEX_HBN_Get_User_Boot_Config]               = (uint32_t)HBN_Get_User_Boot_Config,
    [ROM_API_INDEX_HBN_Set_User_Boot_Config]               = (uint32_t)HBN_Set_User_Boot_Config,
    [ROM_API_INDEX_HBN_Clear_RTC_Counter]                  = (uint32_t)HBN_Clear_RTC_Counter,
    [ROM_API_INDEX_HBN_Enable_RTC_Counter]                 = (uint32_t)HBN_Enable_RTC_Counter,
    [ROM_API_INDEX_HBN_Set_RTC_Timer]                      = (uint32_t)HBN_Set_RTC_Timer,
    [ROM_API_INDEX_HBN_Get_RTC_Timer_Val]                  = (uint32_t)HBN_Get_RTC_Timer_Val,
    [ROM_API_INDEX_HBN_Clear_RTC_INT]                      = (uint32_t)HBN_Clear_RTC_INT,
    [ROM_API_INDEX_HBN_GPIO_INT_Enable]                    = (uint32_t)HBN_GPIO_INT_Enable,
    [ROM_API_INDEX_HBN_GPIO_INT_Disable]                   = (uint32_t)HBN_GPIO_INT_Disable,
    [ROM_API_INDEX_HBN_Get_INT_State]                      = (uint32_t)HBN_Get_INT_State,
    [ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode]                = (uint32_t)HBN_Get_Pin_Wakeup_Mode,
    [ROM_API_INDEX_HBN_Clear_IRQ]                          = (uint32_t)HBN_Clear_IRQ,
    [ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg]                       = (uint32_t)HBN_Hw_Pu_Pd_Cfg,
    [ROM_API_INDEX_HBN_Comm_Pad_Wakeup_En_Cfg]             = (uint32_t)HBN_Comm_Pad_Wakeup_En_Cfg,
    [ROM_API_INDEX_HBN_Aon_Pad_IeSmt_Cfg]                  = (uint32_t)HBN_Aon_Pad_IeSmt_Cfg,
    [ROM_API_INDEX_HBN_Pin_WakeUp_Mask]                    = (uint32_t)HBN_Pin_WakeUp_Mask,
    [ROM_API_INDEX_HBN_Enable_AComp0_IRQ]                  = (uint32_t)HBN_Enable_AComp0_IRQ,
    [ROM_API_INDEX_HBN_Disable_AComp0_IRQ]                 = (uint32_t)HBN_Disable_AComp0_IRQ,
    [ROM_API_INDEX_HBN_Enable_AComp1_IRQ]                  = (uint32_t)HBN_Enable_AComp1_IRQ,
    [ROM_API_INDEX_HBN_Disable_AComp1_IRQ]                 = (uint32_t)HBN_Disable_AComp1_IRQ,
    [ROM_API_INDEX_HBN_Enable_BOR_IRQ]                     = (uint32_t)HBN_Enable_BOR_IRQ,
    [ROM_API_INDEX_HBN_Disable_BOR_IRQ]                    = (uint32_t)HBN_Disable_BOR_IRQ,
    [ROM_API_INDEX_HBN_Get_Reset_Event]                    = (uint32_t)HBN_Get_Reset_Event,
    [ROM_API_INDEX_HBN_Clear_Reset_Event]                  = (uint32_t)HBN_Clear_Reset_Event,
    [ROM_API_INDEX_HBN_GPIO_Dbg_Pull_Cfg]                  = (uint32_t)HBN_GPIO_Dbg_Pull_Cfg,
    [ROM_API_INDEX_HBN_Set_BOR_Cfg]                        = (uint32_t)HBN_Set_BOR_Cfg,

    [ROM_API_INDEX_L1C_Cache_Write_Set]                    = (uint32_t)L1C_Cache_Write_Set,
    [ROM_API_INDEX_L1C_Cache_Enable_Set]                   = (uint32_t)L1C_Cache_Enable_Set,
    [ROM_API_INDEX_L1C_Cache_Flush]                        = (uint32_t)L1C_Cache_Flush,
    [ROM_API_INDEX_L1C_Cache_Hit_Count_Get]                = (uint32_t)L1C_Cache_Hit_Count_Get,
    [ROM_API_INDEX_L1C_Cache_Miss_Count_Get]               = (uint32_t)L1C_Cache_Miss_Count_Get,
    [ROM_API_INDEX_L1C_Cache_Read_Disable]                 = (uint32_t)L1C_Cache_Read_Disable,
    [ROM_API_INDEX_L1C_Set_Wrap]                           = (uint32_t)L1C_Set_Wrap,
    [ROM_API_INDEX_L1C_Set_Way_Disable]                    = (uint32_t)L1C_Set_Way_Disable,
    [ROM_API_INDEX_L1C_IROM_2T_Access_Set]                 = (uint32_t)L1C_IROM_2T_Access_Set,
    [ROM_API_INDEX_L1C_BMX_Init]                           = (uint32_t)L1C_BMX_Init,
    [ROM_API_INDEX_L1C_BMX_Addr_Monitor_Enable]            = (uint32_t)L1C_BMX_Addr_Monitor_Enable,
    [ROM_API_INDEX_L1C_BMX_Addr_Monitor_Disable]           = (uint32_t)L1C_BMX_Addr_Monitor_Disable,
    [ROM_API_INDEX_L1C_BMX_BusErrResponse_Enable]          = (uint32_t)L1C_BMX_BusErrResponse_Enable,
    [ROM_API_INDEX_L1C_BMX_BusErrResponse_Disable]         = (uint32_t)L1C_BMX_BusErrResponse_Disable,
    [ROM_API_INDEX_L1C_BMX_Get_Status]                     = (uint32_t)L1C_BMX_Get_Status,
    [ROM_API_INDEX_L1C_BMX_Get_Err_Addr]                   = (uint32_t)L1C_BMX_Get_Err_Addr,

    [ROM_API_INDEX_PDS_Set_GPIO_Pad_Cfg]                   = (uint32_t)PDS_Set_GPIO_Pad_Cfg,
    [ROM_API_INDEX_PDS_GPIO_Write]                         = (uint32_t)PDS_GPIO_Write,
    [ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask]               = (uint32_t)PDS_Set_GPIO_Pad_IntMask,
    [ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode]               = (uint32_t)PDS_Set_GPIO_Pad_IntMode,
    [ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr]                = (uint32_t)PDS_Set_GPIO_Pad_IntClr,
    [ROM_API_INDEX_PDS_Set_All_GPIO_IntClear]              = (uint32_t)PDS_Set_All_GPIO_IntClear,
    [ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus]             = (uint32_t)PDS_Get_GPIO_Pad_IntStatus,
    [ROM_API_INDEX_PDS_Disable_GPIO_Keep]                  = (uint32_t)PDS_Disable_GPIO_Keep,
    [ROM_API_INDEX_PDS_Enable]                             = (uint32_t)PDS_Enable,
    [ROM_API_INDEX_PDS_Force_Config]                       = (uint32_t)PDS_Force_Config,
    [ROM_API_INDEX_PDS_RAM_Config]                         = (uint32_t)PDS_RAM_Config,
    [ROM_API_INDEX_PDS_Default_Level_Config]               = (uint32_t)PDS_Default_Level_Config,
    [ROM_API_INDEX_PDS_Wakeup_Src_En]                      = (uint32_t)PDS_Wakeup_Src_En,
    [ROM_API_INDEX_PDS_Get_Wakeup_Src]                     = (uint32_t)PDS_Get_Wakeup_Src,
    [ROM_API_INDEX_PDS_IntMask]                            = (uint32_t)PDS_IntMask,
    [ROM_API_INDEX_PDS_Get_IntStatus]                      = (uint32_t)PDS_Get_IntStatus,
    [ROM_API_INDEX_PDS_IntClear]                           = (uint32_t)PDS_IntClear,
    [ROM_API_INDEX_PDS_Get_PdsRfStstus]                    = (uint32_t)PDS_Get_PdsRfStstus,
    [ROM_API_INDEX_PDS_Get_PdsStstus]                      = (uint32_t)PDS_Get_PdsStstus,
    [ROM_API_INDEX_PDS_Clear_Reset_Event]                  = (uint32_t)PDS_Clear_Reset_Event,
    [ROM_API_INDEX_PDS_Get_Reset_Event]                    = (uint32_t)PDS_Get_Reset_Event,
    [ROM_API_INDEX_PDS_Auto_Enable]                        = (uint32_t)PDS_Auto_Enable,
    [ROM_API_INDEX_PDS_Manual_Force_Turn_Off]              = (uint32_t)PDS_Manual_Force_Turn_Off,
    [ROM_API_INDEX_PDS_Manual_Force_Turn_On]               = (uint32_t)PDS_Manual_Force_Turn_On,
    [ROM_API_INDEX_PDS_Set_KYD_Matrix_Size]                = (uint32_t)PDS_Set_KYD_Matrix_Size,
    [ROM_API_INDEX_PDS_Set_KYD_Col_Value]                  = (uint32_t)PDS_Set_KYD_Col_Value,
    [ROM_API_INDEX_PDS_Set_KYD_Row_Pull]                   = (uint32_t)PDS_Set_KYD_Row_Pull,
    [ROM_API_INDEX_PDS_Set_KYD_Wakeup_En]                  = (uint32_t)PDS_Set_KYD_Wakeup_En,

    [ROM_API_INDEX_SF_Cfg_Init_Internal_Flash_Gpio]        = (uint32_t)SF_Cfg_Init_Internal_Flash_Gpio,
    [ROM_API_INDEX_SF_Cfg_Init_Ext_Flash_Gpio]             = (uint32_t)SF_Cfg_Init_Ext_Flash_Gpio,
    [ROM_API_INDEX_SF_Cfg_Deinit_Ext_Flash_Gpio]           = (uint32_t)SF_Cfg_Deinit_Ext_Flash_Gpio,
    [ROM_API_INDEX_SF_Cfg_Get_Flash_Cfg_Need_Lock]         = (uint32_t)SF_Cfg_Get_Flash_Cfg_Need_Lock,
    [ROM_API_INDEX_SF_Cfg_Init_Flash_Gpio]                 = (uint32_t)SF_Cfg_Init_Flash_Gpio,
    [ROM_API_INDEX_SF_Cfg_Flash_Identify]                  = (uint32_t)SF_Cfg_Flash_Identify,
    [ROM_API_INDEX_SF_Ctrl_Enable]                         = (uint32_t)SF_Ctrl_Enable,
    [ROM_API_INDEX_SF_Ctrl_Psram_Init]                     = (uint32_t)SF_Ctrl_Psram_Init,
    [ROM_API_INDEX_SF_Ctrl_Get_Clock_Delay]                = (uint32_t)SF_Ctrl_Get_Clock_Delay,
    [ROM_API_INDEX_SF_Ctrl_Set_Clock_Delay]                = (uint32_t)SF_Ctrl_Set_Clock_Delay,
    [ROM_API_INDEX_SF_Ctrl_Cmds_Set]                       = (uint32_t)SF_Ctrl_Cmds_Set,
    [ROM_API_INDEX_SF_Ctrl_Burst_Toggle_Set]               = (uint32_t)SF_Ctrl_Burst_Toggle_Set,
    [ROM_API_INDEX_SF_Ctrl_Select_Pad]                     = (uint32_t)SF_Ctrl_Select_Pad,
    [ROM_API_INDEX_SF_Ctrl_Select_Bank]                    = (uint32_t)SF_Ctrl_Select_Bank,
    [ROM_API_INDEX_SF_Ctrl_Set_Owner]                      = (uint32_t)SF_Ctrl_Set_Owner,
    [ROM_API_INDEX_SF_Ctrl_Disable]                        = (uint32_t)SF_Ctrl_Disable,
    [ROM_API_INDEX_SF_Ctrl_AES_Enable_BE]                  = (uint32_t)SF_Ctrl_AES_Enable_BE,
    [ROM_API_INDEX_SF_Ctrl_AES_Enable_LE]                  = (uint32_t)SF_Ctrl_AES_Enable_LE,
    [ROM_API_INDEX_SF_Ctrl_AES_Set_Region]                 = (uint32_t)SF_Ctrl_AES_Set_Region,
    [ROM_API_INDEX_SF_Ctrl_AES_Set_Key]                    = (uint32_t)SF_Ctrl_AES_Set_Key,
    [ROM_API_INDEX_SF_Ctrl_AES_Set_Key_BE]                 = (uint32_t)SF_Ctrl_AES_Set_Key_BE,
    [ROM_API_INDEX_SF_Ctrl_AES_Set_IV]                     = (uint32_t)SF_Ctrl_AES_Set_IV,
    [ROM_API_INDEX_SF_Ctrl_AES_Set_IV_BE]                  = (uint32_t)SF_Ctrl_AES_Set_IV_BE,
    [ROM_API_INDEX_SF_Ctrl_AES_Enable]                     = (uint32_t)SF_Ctrl_AES_Enable,
    [ROM_API_INDEX_SF_Ctrl_AES_Disable]                    = (uint32_t)SF_Ctrl_AES_Disable,
    [ROM_API_INDEX_SF_Ctrl_Is_AES_Enable]                  = (uint32_t)SF_Ctrl_Is_AES_Enable,
    [ROM_API_INDEX_SF_Ctrl_Set_Flash_Image_Offset]         = (uint32_t)SF_Ctrl_Set_Flash_Image_Offset,
    [ROM_API_INDEX_SF_Ctrl_Get_Flash_Image_Offset]         = (uint32_t)SF_Ctrl_Get_Flash_Image_Offset,
    [ROM_API_INDEX_SF_Ctrl_SendCmd]                        = (uint32_t)SF_Ctrl_SendCmd,
    [ROM_API_INDEX_SF_Ctrl_Flash_Read_Icache_Set]          = (uint32_t)SF_Ctrl_Flash_Read_Icache_Set,
    [ROM_API_INDEX_SF_Ctrl_Psram_Write_Icache_Set]         = (uint32_t)SF_Ctrl_Psram_Write_Icache_Set,
    [ROM_API_INDEX_SF_Ctrl_Psram_Read_Icache_Set]          = (uint32_t)SF_Ctrl_Psram_Read_Icache_Set,
    [ROM_API_INDEX_SF_Ctrl_GetBusyState]                   = (uint32_t)SF_Ctrl_GetBusyState,

    [ROM_API_INDEX_SFlash_Init]                            = (uint32_t)SFlash_Init,
    [ROM_API_INDEX_SFlash_SetSPIMode]                      = (uint32_t)SFlash_SetSPIMode,
    [ROM_API_INDEX_SFlash_Read_Reg]                        = (uint32_t)SFlash_Read_Reg,
    [ROM_API_INDEX_SFlash_Write_Reg]                       = (uint32_t)SFlash_Write_Reg,
    [ROM_API_INDEX_SFlash_Read_Reg_With_Cmd]               = (uint32_t)SFlash_Read_Reg_With_Cmd,
    [ROM_API_INDEX_SFlash_Write_Reg_With_Cmd]              = (uint32_t)SFlash_Write_Reg_With_Cmd,
    [ROM_API_INDEX_SFlash_Clear_Status_Register]           = (uint32_t)SFlash_Clear_Status_Register,
    [ROM_API_INDEX_SFlash_Busy]                            = (uint32_t)SFlash_Busy,
    [ROM_API_INDEX_SFlash_Write_Enable]                    = (uint32_t)SFlash_Write_Enable,
    [ROM_API_INDEX_SFlash_Qspi_Enable]                     = (uint32_t)SFlash_Qspi_Enable,
    [ROM_API_INDEX_SFlash_Volatile_Reg_Write_Enable]       = (uint32_t)SFlash_Volatile_Reg_Write_Enable,
    [ROM_API_INDEX_SFlash_Chip_Erase]                      = (uint32_t)SFlash_Chip_Erase,
    [ROM_API_INDEX_SFlash_Sector_Erase]                    = (uint32_t)SFlash_Sector_Erase,
    [ROM_API_INDEX_SFlash_Blk32_Erase]                     = (uint32_t)SFlash_Blk32_Erase,
    [ROM_API_INDEX_SFlash_Blk64_Erase]                     = (uint32_t)SFlash_Blk64_Erase,
    [ROM_API_INDEX_SFlash_Erase]                           = (uint32_t)SFlash_Erase,
    [ROM_API_INDEX_SFlash_Program]                         = (uint32_t)SFlash_Program,
    [ROM_API_INDEX_SFlash_GetUniqueId]                     = (uint32_t)SFlash_GetUniqueId,
    [ROM_API_INDEX_SFlash_GetJedecId]                      = (uint32_t)SFlash_GetJedecId,
    [ROM_API_INDEX_SFlash_GetDeviceId]                     = (uint32_t)SFlash_GetDeviceId,
    [ROM_API_INDEX_SFlash_Powerdown]                       = (uint32_t)SFlash_Powerdown,
    [ROM_API_INDEX_SFlash_Releae_Powerdown]                = (uint32_t)SFlash_Releae_Powerdown,
    [ROM_API_INDEX_SFlash_Restore_From_Powerdown]          = (uint32_t)SFlash_Restore_From_Powerdown,
    [ROM_API_INDEX_SFlash_SetBurstWrap]                    = (uint32_t)SFlash_SetBurstWrap,
    [ROM_API_INDEX_SFlash_DisableBurstWrap]                = (uint32_t)SFlash_DisableBurstWrap,
    [ROM_API_INDEX_SFlash_Software_Reset]                  = (uint32_t)SFlash_Software_Reset,
    [ROM_API_INDEX_SFlash_Reset_Continue_Read]             = (uint32_t)SFlash_Reset_Continue_Read,
    [ROM_API_INDEX_SFlash_Set_IDbus_Cfg]                   = (uint32_t)SFlash_Set_IDbus_Cfg,
    [ROM_API_INDEX_SFlash_IDbus_Read_Enable]               = (uint32_t)SFlash_IDbus_Read_Enable,
    [ROM_API_INDEX_SFlash_Cache_Read_Enable]               = (uint32_t)SFlash_Cache_Read_Enable,
    [ROM_API_INDEX_SFlash_Cache_Read_Disable]              = (uint32_t)SFlash_Cache_Read_Disable,
    [ROM_API_INDEX_SFlash_Read]                            = (uint32_t)SFlash_Read,

    [ROM_API_INDEX_UART_SetBaudRate]                       = (uint32_t)UART_SetBaudRate,
    [ROM_API_INDEX_UART_Init]                              = (uint32_t)UART_Init,
    [ROM_API_INDEX_UART_DeInit]                            = (uint32_t)UART_DeInit,
    [ROM_API_INDEX_UART_FifoConfig]                        = (uint32_t)UART_FifoConfig,
    [ROM_API_INDEX_UART_Enable]                            = (uint32_t)UART_Enable,
    [ROM_API_INDEX_UART_Disable]                           = (uint32_t)UART_Disable,
    [ROM_API_INDEX_UART_SetTxDataLength]                   = (uint32_t)UART_SetTxDataLength,
    [ROM_API_INDEX_UART_SetRxDataLength]                   = (uint32_t)UART_SetRxDataLength,
    [ROM_API_INDEX_UART_SetRxTimeoutValue]                 = (uint32_t)UART_SetRxTimeoutValue,
    [ROM_API_INDEX_UART_SetRxByteCount]                    = (uint32_t)UART_SetRxByteCount,
    [ROM_API_INDEX_UART_SetDeglitchCount]                  = (uint32_t)UART_SetDeglitchCount,
    [ROM_API_INDEX_UART_ApplyAbrResult]                    = (uint32_t)UART_ApplyAbrResult,
    [ROM_API_INDEX_UART_SetRtsValue]                       = (uint32_t)UART_SetRtsValue,
    [ROM_API_INDEX_UART_ClrRtsValue]                       = (uint32_t)UART_ClrRtsValue,
    [ROM_API_INDEX_UART_SetTxValue]                        = (uint32_t)UART_SetTxValue,
    [ROM_API_INDEX_UART_ClrTxValue]                        = (uint32_t)UART_ClrTxValue,
    [ROM_API_INDEX_UART_TxFreeRun]                         = (uint32_t)UART_TxFreeRun,
    [ROM_API_INDEX_UART_AutoBaudDetection]                 = (uint32_t)UART_AutoBaudDetection,
    [ROM_API_INDEX_UART_SetAllowableError0X55]             = (uint32_t)UART_SetAllowableError0X55,
    [ROM_API_INDEX_UART_GetBitWidth0X55]                   = (uint32_t)UART_GetBitWidth0X55,
    [ROM_API_INDEX_UART_SetRS485]                          = (uint32_t)UART_SetRS485,
    [ROM_API_INDEX_UART_TxFifoClear]                       = (uint32_t)UART_TxFifoClear,
    [ROM_API_INDEX_UART_RxFifoClear]                       = (uint32_t)UART_RxFifoClear,
    [ROM_API_INDEX_UART_IntMask]                           = (uint32_t)UART_IntMask,
    [ROM_API_INDEX_UART_IntClear]                          = (uint32_t)UART_IntClear,
    [ROM_API_INDEX_UART_SendData]                          = (uint32_t)UART_SendData,
    [ROM_API_INDEX_UART_SendDataBlock]                     = (uint32_t)UART_SendDataBlock,
    [ROM_API_INDEX_UART_ReceiveData]                       = (uint32_t)UART_ReceiveData,
    [ROM_API_INDEX_UART_GetAutoBaudCount]                  = (uint32_t)UART_GetAutoBaudCount,
    [ROM_API_INDEX_UART_GetRxByteCount]                    = (uint32_t)UART_GetRxByteCount,
    [ROM_API_INDEX_UART_GetTxFifoCount]                    = (uint32_t)UART_GetTxFifoCount,
    [ROM_API_INDEX_UART_GetRxFifoCount]                    = (uint32_t)UART_GetRxFifoCount,
    [ROM_API_INDEX_UART_GetIntStatus]                      = (uint32_t)UART_GetIntStatus,
    [ROM_API_INDEX_UART_GetTxBusBusyStatus]                = (uint32_t)UART_GetTxBusBusyStatus,
    [ROM_API_INDEX_UART_GetRxBusBusyStatus]                = (uint32_t)UART_GetRxBusBusyStatus,
    [ROM_API_INDEX_UART_GetOverflowStatus]                 = (uint32_t)UART_GetOverflowStatus,

    [ROM_API_INDEX_XIP_SFlash_Opt_Enter]                   = (uint32_t)XIP_SFlash_Opt_Enter,
    [ROM_API_INDEX_XIP_SFlash_Opt_Exit]                    = (uint32_t)XIP_SFlash_Opt_Exit,
    [ROM_API_INDEX_XIP_SFlash_State_Save]                  = (uint32_t)XIP_SFlash_State_Save,
    [ROM_API_INDEX_XIP_SFlash_State_Restore]               = (uint32_t)XIP_SFlash_State_Restore,
    [ROM_API_INDEX_XIP_SFlash_Erase_Need_Lock]             = (uint32_t)XIP_SFlash_Erase_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_Write_Need_Lock]             = (uint32_t)XIP_SFlash_Write_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_Read_Need_Lock]              = (uint32_t)XIP_SFlash_Read_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_GetJedecId_Need_Lock]        = (uint32_t)XIP_SFlash_GetJedecId_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_GetDeviceId_Need_Lock]       = (uint32_t)XIP_SFlash_GetDeviceId_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_GetUniqueId_Need_Lock]       = (uint32_t)XIP_SFlash_GetUniqueId_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_Read_Via_Cache_Need_Lock]    = (uint32_t)XIP_SFlash_Read_Via_Cache_Need_Lock,
    [ROM_API_INDEX_XIP_SFlash_Clear_Status_Register_Need_Lock]= (uint32_t)XIP_SFlash_Clear_Status_Register_Need_Lock,

    [ROM_API_INDEX_IR_TxInit]                              = (uint32_t)IR_TxInit,
    [ROM_API_INDEX_IR_TxPulseWidthConfig]                  = (uint32_t)IR_TxPulseWidthConfig,
    [ROM_API_INDEX_IR_FifoConfig]                          = (uint32_t)IR_FifoConfig,
    [ROM_API_INDEX_IR_DeInit]                              = (uint32_t)IR_DeInit,
    [ROM_API_INDEX_IR_Enable]                              = (uint32_t)IR_Enable,
    [ROM_API_INDEX_IR_Disable]                             = (uint32_t)IR_Disable,
    [ROM_API_INDEX_IR_TxSWM]                               = (uint32_t)IR_TxSWM,
    [ROM_API_INDEX_IR_TxFifoClear]                         = (uint32_t)IR_TxFifoClear,
    [ROM_API_INDEX_IR_SendData]                            = (uint32_t)IR_SendData,
    [ROM_API_INDEX_IR_SWMSendData]                         = (uint32_t)IR_SWMSendData,
    [ROM_API_INDEX_IR_SendCommand]                         = (uint32_t)IR_SendCommand,
    [ROM_API_INDEX_IR_SWMSendCommand]                      = (uint32_t)IR_SWMSendCommand,
    [ROM_API_INDEX_IR_SendNEC]                             = (uint32_t)IR_SendNEC,
    [ROM_API_INDEX_IR_IntMask]                             = (uint32_t)IR_IntMask,
    [ROM_API_INDEX_IR_ClrIntStatus]                        = (uint32_t)IR_ClrIntStatus,
    [ROM_API_INDEX_IR_GetIntStatus]                        = (uint32_t)IR_GetIntStatus,
    [ROM_API_INDEX_IR_GetFifoStatus]                       = (uint32_t)IR_GetFifoStatus,
    [ROM_API_INDEX_IR_GetTxFifoCount]                      = (uint32_t)IR_GetTxFifoCount,
    [ROM_API_INDEX_IR_LEDInit]                             = (uint32_t)IR_LEDInit,
    [ROM_API_INDEX_IR_LEDSend]                             = (uint32_t)IR_LEDSend,

    [ROM_API_INDEX_KYS_Init]                               = (uint32_t)KYS_Init,
    [ROM_API_INDEX_KYS_Enable]                             = (uint32_t)KYS_Enable,
    [ROM_API_INDEX_KYS_Disable]                            = (uint32_t)KYS_Disable,
    [ROM_API_INDEX_KYS_IntMask]                            = (uint32_t)KYS_IntMask,
    [ROM_API_INDEX_KYS_IntClear]                           = (uint32_t)KYS_IntClear,
    [ROM_API_INDEX_KYS_GetIntStatus]                       = (uint32_t)KYS_GetIntStatus,
    [ROM_API_INDEX_KYS_Get_FIFO_Idx]                       = (uint32_t)KYS_Get_FIFO_Idx,
    [ROM_API_INDEX_KYS_ReadKeyfifo]                        = (uint32_t)KYS_ReadKeyfifo,

    [ROM_API_INDEX_Psram_Init]                             = (uint32_t)Psram_Init,
    [ROM_API_INDEX_Psram_ReadReg]                          = (uint32_t)Psram_ReadReg,
    [ROM_API_INDEX_Psram_WriteReg]                         = (uint32_t)Psram_WriteReg,
    [ROM_API_INDEX_Psram_SetDriveStrength]                 = (uint32_t)Psram_SetDriveStrength,
    [ROM_API_INDEX_Psram_SetBurstWrap]                     = (uint32_t)Psram_SetBurstWrap,
    [ROM_API_INDEX_Psram_ReadId]                           = (uint32_t)Psram_ReadId,
    [ROM_API_INDEX_Psram_EnterQuadMode]                    = (uint32_t)Psram_EnterQuadMode,
    [ROM_API_INDEX_Psram_ExitQuadMode]                     = (uint32_t)Psram_ExitQuadMode,
    [ROM_API_INDEX_Psram_ToggleBurstLength]                = (uint32_t)Psram_ToggleBurstLength,
    [ROM_API_INDEX_Psram_SoftwareReset]                    = (uint32_t)Psram_SoftwareReset,
    [ROM_API_INDEX_Psram_Set_IDbus_Cfg]                    = (uint32_t)Psram_Set_IDbus_Cfg,
    [ROM_API_INDEX_Psram_Cache_Write_Set]                  = (uint32_t)Psram_Cache_Write_Set,
    [ROM_API_INDEX_Psram_Write]                            = (uint32_t)Psram_Write,
    [ROM_API_INDEX_Psram_Read]                             = (uint32_t)Psram_Read,

    [ROM_API_INDEX_TIMER_GetCompValue]                     = (uint32_t)TIMER_GetCompValue,
    [ROM_API_INDEX_TIMER_SetCompValue]                     = (uint32_t)TIMER_SetCompValue,
    [ROM_API_INDEX_TIMER_CompValueEffectImmediately]       = (uint32_t)TIMER_CompValueEffectImmediately,
    [ROM_API_INDEX_TIMER_GetCounterValue]                  = (uint32_t)TIMER_GetCounterValue,
    [ROM_API_INDEX_TIMER_ResetCounterValue]                = (uint32_t)TIMER_ResetCounterValue,
    [ROM_API_INDEX_TIMER_GetMatchStatus]                   = (uint32_t)TIMER_GetMatchStatus,
    [ROM_API_INDEX_TIMER_GetPreloadValue]                  = (uint32_t)TIMER_GetPreloadValue,
    [ROM_API_INDEX_TIMER_SetPreloadValue]                  = (uint32_t)TIMER_SetPreloadValue,
    [ROM_API_INDEX_TIMER_SetPreloadTrigSrc]                = (uint32_t)TIMER_SetPreloadTrigSrc,
    [ROM_API_INDEX_TIMER_SetCountMode]                     = (uint32_t)TIMER_SetCountMode,
    [ROM_API_INDEX_TIMER_ClearIntStatus]                   = (uint32_t)TIMER_ClearIntStatus,
    [ROM_API_INDEX_TIMER_Init]                             = (uint32_t)TIMER_Init,
    [ROM_API_INDEX_TIMER_DeInit]                           = (uint32_t)TIMER_DeInit,
    [ROM_API_INDEX_TIMER_Enable]                           = (uint32_t)TIMER_Enable,
    [ROM_API_INDEX_TIMER_Disable]                          = (uint32_t)TIMER_Disable,
    [ROM_API_INDEX_TIMER_IntMask]                          = (uint32_t)TIMER_IntMask,
    [ROM_API_INDEX_TIMER_GPIOSetPolarity]                  = (uint32_t)TIMER_GPIOSetPolarity,
    [ROM_API_INDEX_TIMER_CH0_SetMeasurePulseWidth]         = (uint32_t)TIMER_CH0_SetMeasurePulseWidth,
    [ROM_API_INDEX_TIMER_CH0_GetMeasurePulseWidth]         = (uint32_t)TIMER_CH0_GetMeasurePulseWidth,
    [ROM_API_INDEX_TIMER_ForceClockDivision]               = (uint32_t)TIMER_ForceClockDivision,

    [ROM_API_INDEX_WDT_Set_Clock]                          = (uint32_t)WDT_Set_Clock,
    [ROM_API_INDEX_WDT_GetMatchValue]                      = (uint32_t)WDT_GetMatchValue,
    [ROM_API_INDEX_WDT_SetCompValue]                       = (uint32_t)WDT_SetCompValue,
    [ROM_API_INDEX_WDT_CompValueEffectImmediately]         = (uint32_t)WDT_CompValueEffectImmediately,
    [ROM_API_INDEX_WDT_GetCounterValue]                    = (uint32_t)WDT_GetCounterValue,
    [ROM_API_INDEX_WDT_ResetCounterValue]                  = (uint32_t)WDT_ResetCounterValue,
    [ROM_API_INDEX_WDT_GetResetStatus]                     = (uint32_t)WDT_GetResetStatus,
    [ROM_API_INDEX_WDT_ClearResetStatus]                   = (uint32_t)WDT_ClearResetStatus,
    [ROM_API_INDEX_WDT_Enable]                             = (uint32_t)WDT_Enable,
    [ROM_API_INDEX_WDT_Disable]                            = (uint32_t)WDT_Disable,
    [ROM_API_INDEX_WDT_ForceClockDivision]                 = (uint32_t)WDT_ForceClockDivision,
    [ROM_API_INDEX_WDT_IntMask]                            = (uint32_t)WDT_IntMask,
    [ROM_API_INDEX_WDT_GPIOSetPolarity]                    = (uint32_t)WDT_GPIOSetPolarity,

    [ROM_API_INDEX_arch_memcpy]                            = (uint32_t)arch_memcpy,
    [ROM_API_INDEX_arch_memcpy4]                           = (uint32_t)arch_memcpy4,
    [ROM_API_INDEX_arch_memcpy_fast]                       = (uint32_t)arch_memcpy_fast,
    [ROM_API_INDEX_arch_memset]                            = (uint32_t)arch_memset,
    [ROM_API_INDEX_arch_memset4]                           = (uint32_t)arch_memset4,
    [ROM_API_INDEX_arch_memcmp]                            = (uint32_t)arch_memcmp,

    [ROM_API_INDEX_memcopy_to_fifo]                        = (uint32_t)memcopy_to_fifo,

    [ROM_API_INDEX_fifocopy_to_mem]                        = (uint32_t)fifocopy_to_mem,

    [ROM_API_INDEX_BFLB_Soft_CRC32_Ex]                     = (uint32_t)BFLB_Soft_CRC32_Ex,
    [ROM_API_INDEX_BFLB_Soft_CRC32]                        = (uint32_t)BFLB_Soft_CRC32,

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

/*@} end of group BL702L_Periph_Driver */


