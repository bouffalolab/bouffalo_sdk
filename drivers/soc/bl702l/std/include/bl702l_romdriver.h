/**
  ******************************************************************************
  * @file    bl702l_romdriver.h
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
#ifndef __BL702L_ROMDRIVER_H__
#define __BL702L_ROMDRIVER_H__

#include "bl702l_aon.h"
#include "bl702l_clock.h"
#include "bl702l_common.h"
#include "bl702l_ef_cfg.h"
#include "bl702l_ef_ctrl.h"
#include "bl702l_glb.h"
#include "bl702l_glb_gpio.h"
#include "bl702l_hbn.h"
#include "bl702l_l1c.h"
#include "bl702l_pds.h"
// #include "bl702l_uart.h"
#include "bflb_sf_cfg.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sflash.h"
#include "bflb_xip_sflash.h"

// #include "bl702l_ir.h"
// #include "bl702l_kys.h"
// #include "bl702l_psram.h"
// #include "bl702l_timer.h"

// #include "misc.h"
// #include "softcrc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  ROMDRIVER
 *  @{
 */

/** @defgroup  ROMDRIVER_Public_Types
 *  @{
 */

#define ROMAPI_INDEX_SECT_SIZE     (0x800)
#define ROMAPI_INDEX_MAX           (ROMAPI_INDEX_SECT_SIZE / 4 - 1)

typedef enum {
    ROM_API_INDEX_VERSION = 0,
    ROM_API_INDEX_RSVD_0,
    ROM_API_INDEX_RSVD_1,
    ROM_API_INDEX_RSVD_LAST,


    ROM_API_INDEX_AON_Power_On_MBG,
    ROM_API_INDEX_AON_Power_Off_MBG,
    ROM_API_INDEX_AON_Power_On_XTAL,
    ROM_API_INDEX_AON_Set_Xtal_CapCode,
    ROM_API_INDEX_AON_Get_Xtal_CapCode,
    ROM_API_INDEX_AON_Set_Xtal_CapCode_Extra,
    ROM_API_INDEX_AON_Power_Off_XTAL,
    ROM_API_INDEX_AON_Power_On_BG,
    ROM_API_INDEX_AON_Power_Off_BG,
    ROM_API_INDEX_AON_Power_On_LDO11_SOC,
    ROM_API_INDEX_AON_Power_Off_LDO11_SOC,
    ROM_API_INDEX_AON_Power_On_SFReg,
    ROM_API_INDEX_AON_Power_Off_SFReg,
    ROM_API_INDEX_AON_Set_LDO11_SOC_Sstart_Delay,
    ROM_API_INDEX_AON_Set_DCDC14_Top_0,
    ROM_API_INDEX_AON_Trim_Ldo11socVoutTrim,
    ROM_API_INDEX_AON_Trim_Ldo14VoutTrim,
    ROM_API_INDEX_AON_Trim_Dcdc14VoutTrim,

    ROM_API_INDEX_Clock_System_Clock_Get,
    ROM_API_INDEX_Clock_Peripheral_Clock_Get,

    ROM_API_INDEX_SystemCoreClockGet,

    ROM_API_INDEX_CPU_Get_MTimer_Clock,
    ROM_API_INDEX_CPU_Get_MTimer_Counter,
    ROM_API_INDEX_CPU_Get_CPU_Cycle,
    ROM_API_INDEX_CPU_Get_MTimer_US,
    ROM_API_INDEX_CPU_Get_MTimer_MS,
    ROM_API_INDEX_CPU_MTimer_Delay_US,
    ROM_API_INDEX_CPU_MTimer_Delay_MS,

    ROM_API_INDEX_BL702L_Delay_US,
    ROM_API_INDEX_BL702L_Delay_MS,

    ROM_API_INDEX_EF_Ctrl_Get_Trim_Parity,
    ROM_API_INDEX_EF_Ctrl_Read_Common_Trim,
    ROM_API_INDEX_EF_Ctrl_Write_Common_Trim,
    ROM_API_INDEX_EF_Ctrl_Is_MAC_Address_Slot_Empty,
    ROM_API_INDEX_EF_Ctrl_Write_MAC_Address_Opt,
    ROM_API_INDEX_EF_Ctrl_Read_MAC_Address_Opt,
    ROM_API_INDEX_EF_Ctrl_Sw_AHB_Clk_0,
    ROM_API_INDEX_EF_Ctrl_Program_Efuse_0,
    ROM_API_INDEX_EF_Ctrl_Load_Efuse_R0,
    ROM_API_INDEX_EF_Ctrl_Busy,
    ROM_API_INDEX_EF_Ctrl_AutoLoad_Done,
    ROM_API_INDEX_EF_Ctrl_Write_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Read_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Readlock_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Writelock_Dbg_Pwd,
    ROM_API_INDEX_EF_Ctrl_Write_Secure_Cfg,
    ROM_API_INDEX_EF_Ctrl_Read_Secure_Cfg,
    ROM_API_INDEX_EF_Ctrl_Write_Secure_Boot,
    ROM_API_INDEX_EF_Ctrl_Read_Secure_Boot,
    ROM_API_INDEX_EF_Ctrl_Write_Sw_Usage,
    ROM_API_INDEX_EF_Ctrl_Read_Sw_Usage,
    ROM_API_INDEX_EF_Ctrl_Writelock_Sw_Usage,
    ROM_API_INDEX_EF_Ctrl_Write_MAC_Address,
    ROM_API_INDEX_EF_Ctrl_Get_Byte_Zero_Cnt,
    ROM_API_INDEX_EF_Ctrl_Is_All_Bits_Zero,
    ROM_API_INDEX_EF_Ctrl_Read_MAC_Address,
    ROM_API_INDEX_EF_Ctrl_Writelock_MAC_Address,
    ROM_API_INDEX_EF_Ctrl_Read_Chip_ID,
    ROM_API_INDEX_EF_Ctrl_Read_Device_Info,
    ROM_API_INDEX_EF_Ctrl_Is_CapCode_Empty,
    ROM_API_INDEX_EF_Ctrl_Write_CapCode_Opt,
    ROM_API_INDEX_EF_Ctrl_Read_CapCode_Opt,
    ROM_API_INDEX_EF_Ctrl_Is_PowerOffset_Slot_Empty,
    ROM_API_INDEX_EF_Ctrl_Write_PowerOffset_Opt,
    ROM_API_INDEX_EF_Ctrl_Read_PowerOffset_Opt,
    ROM_API_INDEX_EF_Ctrl_Write_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Read_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Writelock_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Readlock_AES_Key,
    ROM_API_INDEX_EF_Ctrl_Program_Direct_R0,
    ROM_API_INDEX_EF_Ctrl_Read_Direct_R0,
    ROM_API_INDEX_EF_Ctrl_Clear,
    ROM_API_INDEX_EF_Ctrl_Crc_Enable,
    ROM_API_INDEX_EF_Ctrl_Crc_Is_Busy,
    ROM_API_INDEX_EF_Ctrl_Crc_Set_Golden,
    ROM_API_INDEX_EF_Ctrl_Crc_Result,

    ROM_API_INDEX_GLB_Get_Root_CLK_Sel,
    ROM_API_INDEX_GLB_Set_System_CLK_Div,
    ROM_API_INDEX_GLB_Get_BCLK_Div,
    ROM_API_INDEX_GLB_Get_HCLK_Div,
    ROM_API_INDEX_GLB_Set_System_CLK,

    ROM_API_INDEX_System_Core_Clock_Update_From_RC32M,

    ROM_API_INDEX_GLB_Set_MAC154_ZIGBEE_CLK,
    ROM_API_INDEX_GLB_Set_BLE_CLK,
    ROM_API_INDEX_GLB_Set_AUDIO_CLK,
    ROM_API_INDEX_GLB_Set_KYS_CLK,
    ROM_API_INDEX_GLB_Set_DMA_CLK,
    ROM_API_INDEX_GLB_Set_IR_CLK,
    ROM_API_INDEX_GLB_Set_SF_CLK,
    ROM_API_INDEX_GLB_Set_UART_CLK,
    ROM_API_INDEX_GLB_Sel_TMR_GPIO_Clock,
    ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Sel,
    ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Sel,
    ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Enable,
    ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Enable,
    ROM_API_INDEX_GLB_Set_I2C_CLK,
    ROM_API_INDEX_GLB_Set_SPI_CLK,
    ROM_API_INDEX_GLB_Set_PKA_CLK_Sel,
    ROM_API_INDEX_GLB_SW_System_Reset,
    ROM_API_INDEX_GLB_SW_CPU_Reset,
    ROM_API_INDEX_GLB_SW_POR_Reset,
    ROM_API_INDEX_GLB_AHB_MCU_Software_Reset,
    ROM_API_INDEX_GLB_Disrst_Set,
    ROM_API_INDEX_GLB_MAC154_ZIGBEE_Reset,
    ROM_API_INDEX_GLB_BLE_Reset,
    ROM_API_INDEX_GLB_PER_Clock_Gate,
    ROM_API_INDEX_GLB_PER_Clock_UnGate,
    ROM_API_INDEX_GLB_Set_RTC_Mode,
    ROM_API_INDEX_GLB_BMX_Init,
    ROM_API_INDEX_GLB_BMX_Addr_Monitor_Enable,
    ROM_API_INDEX_GLB_BMX_Addr_Monitor_Disable,
    ROM_API_INDEX_GLB_BMX_BusErrResponse_Enable,
    ROM_API_INDEX_GLB_BMX_BusErrResponse_Disable,
    ROM_API_INDEX_GLB_BMX_Get_Status,
    ROM_API_INDEX_GLB_BMX_Get_Err_Addr,
    ROM_API_INDEX_GLB_BMX_BusErrClr_Set,
    ROM_API_INDEX_GLB_Set_SRAM_PARM,
    ROM_API_INDEX_GLB_Get_SRAM_PARM,
    ROM_API_INDEX_GLB_Set_OCRAM_PARM,
    ROM_API_INDEX_GLB_Get_OCRAM_PARM,
    ROM_API_INDEX_GLB_Set_EM_Sel,
    ROM_API_INDEX_GLB_Set_Kys_Drv_Col,
    ROM_API_INDEX_GLB_GPIO_O_Latch_Mode_Set,
    ROM_API_INDEX_GLB_JTAG_Sig_Swap_Set,
    ROM_API_INDEX_GLB_CCI_Use_IO_0_1_2_7,
    ROM_API_INDEX_GLB_CCI_Use_Jtag_Pin,
    ROM_API_INDEX_GLB_Swap_SPI_0_MOSI_With_MISO,
    ROM_API_INDEX_GLB_Set_SPI_0_ACT_MOD_Sel,
    ROM_API_INDEX_GLB_Set_Flash_Scenario,
    ROM_API_INDEX_GLB_Set_Embedded_FLash_IO_PARM,
    ROM_API_INDEX_GLB_Set_MTimer_CLK,
    ROM_API_INDEX_GLB_Set_ADC_CLK,
    ROM_API_INDEX_GLB_Set_DIG_32K_CLK,
    ROM_API_INDEX_GLB_SW_BLE_WAKEUP_REQ_Set,
    ROM_API_INDEX_GLB_UART_Fun_Sel,
    ROM_API_INDEX_GLB_Power_Off_DLL,
    ROM_API_INDEX_GLB_Power_On_DLL,
    ROM_API_INDEX_GLB_Enable_DLL_All_Clks,
    ROM_API_INDEX_GLB_Enable_DLL_Clk,
    ROM_API_INDEX_GLB_Disable_DLL_All_Clks,
    ROM_API_INDEX_GLB_Disable_DLL_Clk,
    ROM_API_INDEX_GLB_Set_Flash_Id_Value,
    ROM_API_INDEX_GLB_Get_Flash_Id_Value,
    ROM_API_INDEX_GLB_Trim_RC32M,
    ROM_API_INDEX_GLB_Set_Xtal_Cnt32k_Process,
    ROM_API_INDEX_GLB_Clear_Xtal_Cnt32k_Done,
    ROM_API_INDEX_GLB_RC32K_Deg_Start,
    ROM_API_INDEX_GLB_RC32K_Deg_End,
    ROM_API_INDEX_GLB_RC32K_Deg_Enable,
    ROM_API_INDEX_GLB_Xtal_Deg_Cnt_Limit_Set,
    ROM_API_INDEX_GLB_IR_LED_Driver_Enable,
    ROM_API_INDEX_GLB_IR_LED_Driver_Disable,
    ROM_API_INDEX_GLB_IR_LED_Driver_Output_Enable,
    ROM_API_INDEX_GLB_IR_LED_Driver_Output_Disable,
    ROM_API_INDEX_GLB_IR_LED_Driver_Ibias,
    ROM_API_INDEX_GLB_GPIO_Init,
    ROM_API_INDEX_GLB_GPIO_Func_Init,
    ROM_API_INDEX_GLB_GPIO_OUTPUT_Mode_Set,
    ROM_API_INDEX_GLB_GPIO_INPUT_Enable,
    ROM_API_INDEX_GLB_GPIO_INPUT_Disable,
    ROM_API_INDEX_GLB_GPIO_OUTPUT_Enable,
    ROM_API_INDEX_GLB_GPIO_OUTPUT_Disable,
    ROM_API_INDEX_GLB_GPIO_Set_HZ,
    ROM_API_INDEX_GLB_GPIO_Get_Fun,
    ROM_API_INDEX_GLB_GPIO_Write,
    ROM_API_INDEX_GLB_GPIO_Read,
    ROM_API_INDEX_GLB_GPIO_Set,
    ROM_API_INDEX_GLB_GPIO_Clr,
    ROM_API_INDEX_GLB_GPIO_IntMask,
    ROM_API_INDEX_GLB_Clr_GPIO_IntStatus,
    ROM_API_INDEX_GLB_Get_GPIO_IntStatus,
    ROM_API_INDEX_GLB_GPIO_Int_Init,

    ROM_API_INDEX_HBN_Mode_Enter,
    ROM_API_INDEX_HBN_GPIO_Wakeup_Set,
    ROM_API_INDEX_HBN_Power_Down_Flash,
    ROM_API_INDEX_HBN_Enable,
    ROM_API_INDEX_HBN_Reset,
    ROM_API_INDEX_HBN_App_Reset,
    ROM_API_INDEX_HBN_Disable,
    ROM_API_INDEX_HBN_Get_BOR_OUT_State,
    ROM_API_INDEX_HBN_Set_BOR_Config,
    ROM_API_INDEX_HBN_Set_Ldo11_Aon_Vout,
    ROM_API_INDEX_HBN_Set_Ldo11_Soc_Vout,
    ROM_API_INDEX_HBN_Set_Ldo11_All_Vout,
    ROM_API_INDEX_HBN_32K_Sel,
    ROM_API_INDEX_HBN_Set_UART_CLK_Sel,
    ROM_API_INDEX_HBN_Set_XCLK_CLK_Sel,
    ROM_API_INDEX_HBN_Set_ROOT_CLK_Sel,
    ROM_API_INDEX_HBN_Set_HRAM_slp,
    ROM_API_INDEX_HBN_Set_HRAM_Ret,
    ROM_API_INDEX_HBN_Power_On_Xtal_32K,
    ROM_API_INDEX_HBN_Power_Off_Xtal_32K,
    ROM_API_INDEX_HBN_Power_On_RC32K,
    ROM_API_INDEX_HBN_Power_Off_RC32K,
    ROM_API_INDEX_HBN_Trim_RC32K,
    ROM_API_INDEX_HBN_Get_Status_Flag,
    ROM_API_INDEX_HBN_Set_Status_Flag,
    ROM_API_INDEX_HBN_Get_Wakeup_Addr,
    ROM_API_INDEX_HBN_Set_Wakeup_Addr,
    ROM_API_INDEX_HBN_Get_User_Boot_Config,
    ROM_API_INDEX_HBN_Set_User_Boot_Config,
    ROM_API_INDEX_HBN_Clear_RTC_Counter,
    ROM_API_INDEX_HBN_Enable_RTC_Counter,
    ROM_API_INDEX_HBN_Set_RTC_Timer,
    ROM_API_INDEX_HBN_Get_RTC_Timer_Val,
    ROM_API_INDEX_HBN_Clear_RTC_INT,
    ROM_API_INDEX_HBN_GPIO_INT_Enable,
    ROM_API_INDEX_HBN_GPIO_INT_Disable,
    ROM_API_INDEX_HBN_Get_INT_State,
    ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode,
    ROM_API_INDEX_HBN_Clear_IRQ,
    ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg,
    ROM_API_INDEX_HBN_Comm_Pad_Wakeup_En_Cfg,
    ROM_API_INDEX_HBN_Aon_Pad_IeSmt_Cfg,
    ROM_API_INDEX_HBN_Pin_WakeUp_Mask,
    ROM_API_INDEX_HBN_Enable_AComp0_IRQ,
    ROM_API_INDEX_HBN_Disable_AComp0_IRQ,
    ROM_API_INDEX_HBN_Enable_AComp1_IRQ,
    ROM_API_INDEX_HBN_Disable_AComp1_IRQ,
    ROM_API_INDEX_HBN_Enable_BOR_IRQ,
    ROM_API_INDEX_HBN_Disable_BOR_IRQ,
    ROM_API_INDEX_HBN_Get_Reset_Event,
    ROM_API_INDEX_HBN_Clear_Reset_Event,
    ROM_API_INDEX_HBN_GPIO_Dbg_Pull_Cfg,
    ROM_API_INDEX_HBN_Set_BOR_Cfg,

    ROM_API_INDEX_L1C_Cache_Write_Set,
    ROM_API_INDEX_L1C_Cache_Enable_Set,
    ROM_API_INDEX_L1C_Cache_Flush,
    ROM_API_INDEX_L1C_Cache_Hit_Count_Get,
    ROM_API_INDEX_L1C_Cache_Miss_Count_Get,
    ROM_API_INDEX_L1C_Cache_Read_Disable,
    ROM_API_INDEX_L1C_Set_Wrap,
    ROM_API_INDEX_L1C_Set_Way_Disable,
    ROM_API_INDEX_L1C_IROM_2T_Access_Set,
    ROM_API_INDEX_L1C_BMX_Init,
    ROM_API_INDEX_L1C_BMX_Addr_Monitor_Enable,
    ROM_API_INDEX_L1C_BMX_Addr_Monitor_Disable,
    ROM_API_INDEX_L1C_BMX_BusErrResponse_Enable,
    ROM_API_INDEX_L1C_BMX_BusErrResponse_Disable,
    ROM_API_INDEX_L1C_BMX_Get_Status,
    ROM_API_INDEX_L1C_BMX_Get_Err_Addr,

    ROM_API_INDEX_PDS_Set_GPIO_Pad_Cfg,
    ROM_API_INDEX_PDS_GPIO_Write,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode,
    ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr,
    ROM_API_INDEX_PDS_Set_All_GPIO_IntClear,
    ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus,
    ROM_API_INDEX_PDS_Disable_GPIO_Keep,
    ROM_API_INDEX_PDS_Enable,
    ROM_API_INDEX_PDS_Force_Config,
    ROM_API_INDEX_PDS_RAM_Config,
    ROM_API_INDEX_PDS_Default_Level_Config,
    ROM_API_INDEX_PDS_Wakeup_Src_En,
    ROM_API_INDEX_PDS_Get_Wakeup_Src,
    ROM_API_INDEX_PDS_IntMask,
    ROM_API_INDEX_PDS_Get_IntStatus,
    ROM_API_INDEX_PDS_IntClear,
    ROM_API_INDEX_PDS_Get_PdsRfStstus,
    ROM_API_INDEX_PDS_Get_PdsStstus,
    ROM_API_INDEX_PDS_Clear_Reset_Event,
    ROM_API_INDEX_PDS_Get_Reset_Event,
    ROM_API_INDEX_PDS_Auto_Enable,
    ROM_API_INDEX_PDS_Manual_Force_Turn_Off,
    ROM_API_INDEX_PDS_Manual_Force_Turn_On,
    ROM_API_INDEX_PDS_Set_KYD_Matrix_Size,
    ROM_API_INDEX_PDS_Set_KYD_Col_Value,
    ROM_API_INDEX_PDS_Set_KYD_Row_Pull,
    ROM_API_INDEX_PDS_Set_KYD_Wakeup_En,

    ROM_API_INDEX_SF_Cfg_Init_Internal_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Init_Ext_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Deinit_Ext_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Get_Flash_Cfg_Need_Lock,
    ROM_API_INDEX_SF_Cfg_Init_Flash_Gpio,
    ROM_API_INDEX_SF_Cfg_Flash_Identify,
    ROM_API_INDEX_SF_Ctrl_Enable,
    ROM_API_INDEX_SF_Ctrl_Psram_Init,
    ROM_API_INDEX_SF_Ctrl_Get_Clock_Delay,
    ROM_API_INDEX_SF_Ctrl_Set_Clock_Delay,
    ROM_API_INDEX_SF_Ctrl_Cmds_Set,
    ROM_API_INDEX_SF_Ctrl_Burst_Toggle_Set,
    ROM_API_INDEX_SF_Ctrl_Select_Pad,
    ROM_API_INDEX_SF_Ctrl_Select_Bank,
    ROM_API_INDEX_SF_Ctrl_Set_Owner,
    ROM_API_INDEX_SF_Ctrl_Disable,
    ROM_API_INDEX_SF_Ctrl_AES_Enable_BE,
    ROM_API_INDEX_SF_Ctrl_AES_Enable_LE,
    ROM_API_INDEX_SF_Ctrl_AES_Set_Region,
    ROM_API_INDEX_SF_Ctrl_AES_Set_Key,
    ROM_API_INDEX_SF_Ctrl_AES_Set_Key_BE,
    ROM_API_INDEX_SF_Ctrl_AES_Set_IV,
    ROM_API_INDEX_SF_Ctrl_AES_Set_IV_BE,
    ROM_API_INDEX_SF_Ctrl_AES_Enable,
    ROM_API_INDEX_SF_Ctrl_AES_Disable,
    ROM_API_INDEX_SF_Ctrl_Is_AES_Enable,
    ROM_API_INDEX_SF_Ctrl_Set_Flash_Image_Offset,
    ROM_API_INDEX_SF_Ctrl_Get_Flash_Image_Offset,
    ROM_API_INDEX_SF_Ctrl_SendCmd,
    ROM_API_INDEX_SF_Ctrl_Flash_Read_Icache_Set,
    ROM_API_INDEX_SF_Ctrl_Psram_Write_Icache_Set,
    ROM_API_INDEX_SF_Ctrl_Psram_Read_Icache_Set,
    ROM_API_INDEX_SF_Ctrl_GetBusyState,

    ROM_API_INDEX_SFlash_Init,
    ROM_API_INDEX_SFlash_SetSPIMode,
    ROM_API_INDEX_SFlash_Read_Reg,
    ROM_API_INDEX_SFlash_Write_Reg,
    ROM_API_INDEX_SFlash_Read_Reg_With_Cmd,
    ROM_API_INDEX_SFlash_Write_Reg_With_Cmd,
    ROM_API_INDEX_SFlash_Clear_Status_Register,
    ROM_API_INDEX_SFlash_Busy,
    ROM_API_INDEX_SFlash_Write_Enable,
    ROM_API_INDEX_SFlash_Qspi_Enable,
    ROM_API_INDEX_SFlash_Volatile_Reg_Write_Enable,
    ROM_API_INDEX_SFlash_Chip_Erase,
    ROM_API_INDEX_SFlash_Sector_Erase,
    ROM_API_INDEX_SFlash_Blk32_Erase,
    ROM_API_INDEX_SFlash_Blk64_Erase,
    ROM_API_INDEX_SFlash_Erase,
    ROM_API_INDEX_SFlash_Program,
    ROM_API_INDEX_SFlash_GetUniqueId,
    ROM_API_INDEX_SFlash_GetJedecId,
    ROM_API_INDEX_SFlash_GetDeviceId,
    ROM_API_INDEX_SFlash_Powerdown,
    ROM_API_INDEX_SFlash_Releae_Powerdown,
    ROM_API_INDEX_SFlash_Restore_From_Powerdown,
    ROM_API_INDEX_SFlash_SetBurstWrap,
    ROM_API_INDEX_SFlash_DisableBurstWrap,
    ROM_API_INDEX_SFlash_Software_Reset,
    ROM_API_INDEX_SFlash_Reset_Continue_Read,
    ROM_API_INDEX_SFlash_Set_IDbus_Cfg,
    ROM_API_INDEX_SFlash_IDbus_Read_Enable,
    ROM_API_INDEX_SFlash_Cache_Read_Enable,
    ROM_API_INDEX_SFlash_Cache_Read_Disable,
    ROM_API_INDEX_SFlash_Read,

    ROM_API_INDEX_UART_SetBaudRate,
    ROM_API_INDEX_UART_Init,
    ROM_API_INDEX_UART_DeInit,
    ROM_API_INDEX_UART_FifoConfig,
    ROM_API_INDEX_UART_Enable,
    ROM_API_INDEX_UART_Disable,
    ROM_API_INDEX_UART_SetTxDataLength,
    ROM_API_INDEX_UART_SetRxDataLength,
    ROM_API_INDEX_UART_SetRxTimeoutValue,
    ROM_API_INDEX_UART_SetRxByteCount,
    ROM_API_INDEX_UART_SetDeglitchCount,
    ROM_API_INDEX_UART_ApplyAbrResult,
    ROM_API_INDEX_UART_SetRtsValue,
    ROM_API_INDEX_UART_ClrRtsValue,
    ROM_API_INDEX_UART_SetTxValue,
    ROM_API_INDEX_UART_ClrTxValue,
    ROM_API_INDEX_UART_TxFreeRun,
    ROM_API_INDEX_UART_AutoBaudDetection,
    ROM_API_INDEX_UART_SetAllowableError0X55,
    ROM_API_INDEX_UART_GetBitWidth0X55,
    ROM_API_INDEX_UART_SetRS485,
    ROM_API_INDEX_UART_TxFifoClear,
    ROM_API_INDEX_UART_RxFifoClear,
    ROM_API_INDEX_UART_IntMask,
    ROM_API_INDEX_UART_IntClear,
    ROM_API_INDEX_UART_SendData,
    ROM_API_INDEX_UART_SendDataBlock,
    ROM_API_INDEX_UART_ReceiveData,
    ROM_API_INDEX_UART_GetAutoBaudCount,
    ROM_API_INDEX_UART_GetRxByteCount,
    ROM_API_INDEX_UART_GetTxFifoCount,
    ROM_API_INDEX_UART_GetRxFifoCount,
    ROM_API_INDEX_UART_GetIntStatus,
    ROM_API_INDEX_UART_GetTxBusBusyStatus,
    ROM_API_INDEX_UART_GetRxBusBusyStatus,
    ROM_API_INDEX_UART_GetOverflowStatus,

    ROM_API_INDEX_XIP_SFlash_Opt_Enter,
    ROM_API_INDEX_XIP_SFlash_Opt_Exit,
    ROM_API_INDEX_XIP_SFlash_State_Save,
    ROM_API_INDEX_XIP_SFlash_State_Restore,
    ROM_API_INDEX_XIP_SFlash_Erase_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Write_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Read_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_GetJedecId_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_GetDeviceId_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_GetUniqueId_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Read_Via_Cache_Need_Lock,
    ROM_API_INDEX_XIP_SFlash_Clear_Status_Register_Need_Lock,

    ROM_API_INDEX_IR_TxInit,
    ROM_API_INDEX_IR_TxPulseWidthConfig,
    ROM_API_INDEX_IR_FifoConfig,
    ROM_API_INDEX_IR_DeInit,
    ROM_API_INDEX_IR_Enable,
    ROM_API_INDEX_IR_Disable,
    ROM_API_INDEX_IR_TxSWM,
    ROM_API_INDEX_IR_TxFifoClear,
    ROM_API_INDEX_IR_SendData,
    ROM_API_INDEX_IR_SWMSendData,
    ROM_API_INDEX_IR_SendCommand,
    ROM_API_INDEX_IR_SWMSendCommand,
    ROM_API_INDEX_IR_SendNEC,
    ROM_API_INDEX_IR_IntMask,
    ROM_API_INDEX_IR_ClrIntStatus,
    ROM_API_INDEX_IR_GetIntStatus,
    ROM_API_INDEX_IR_GetFifoStatus,
    ROM_API_INDEX_IR_GetTxFifoCount,
    ROM_API_INDEX_IR_LEDInit,
    ROM_API_INDEX_IR_LEDSend,

    ROM_API_INDEX_KYS_Init,
    ROM_API_INDEX_KYS_Enable,
    ROM_API_INDEX_KYS_Disable,
    ROM_API_INDEX_KYS_IntMask,
    ROM_API_INDEX_KYS_IntClear,
    ROM_API_INDEX_KYS_GetIntStatus,
    ROM_API_INDEX_KYS_Get_FIFO_Idx,
    ROM_API_INDEX_KYS_ReadKeyfifo,

    ROM_API_INDEX_Psram_Init,
    ROM_API_INDEX_Psram_ReadReg,
    ROM_API_INDEX_Psram_WriteReg,
    ROM_API_INDEX_Psram_SetDriveStrength,
    ROM_API_INDEX_Psram_SetBurstWrap,
    ROM_API_INDEX_Psram_ReadId,
    ROM_API_INDEX_Psram_EnterQuadMode,
    ROM_API_INDEX_Psram_ExitQuadMode,
    ROM_API_INDEX_Psram_ToggleBurstLength,
    ROM_API_INDEX_Psram_SoftwareReset,
    ROM_API_INDEX_Psram_Set_IDbus_Cfg,
    ROM_API_INDEX_Psram_Cache_Write_Set,
    ROM_API_INDEX_Psram_Write,
    ROM_API_INDEX_Psram_Read,

    ROM_API_INDEX_TIMER_GetCompValue,
    ROM_API_INDEX_TIMER_SetCompValue,
    ROM_API_INDEX_TIMER_CompValueEffectImmediately,
    ROM_API_INDEX_TIMER_GetCounterValue,
    ROM_API_INDEX_TIMER_ResetCounterValue,
    ROM_API_INDEX_TIMER_GetMatchStatus,
    ROM_API_INDEX_TIMER_GetPreloadValue,
    ROM_API_INDEX_TIMER_SetPreloadValue,
    ROM_API_INDEX_TIMER_SetPreloadTrigSrc,
    ROM_API_INDEX_TIMER_SetCountMode,
    ROM_API_INDEX_TIMER_ClearIntStatus,
    ROM_API_INDEX_TIMER_Init,
    ROM_API_INDEX_TIMER_DeInit,
    ROM_API_INDEX_TIMER_Enable,
    ROM_API_INDEX_TIMER_Disable,
    ROM_API_INDEX_TIMER_IntMask,
    ROM_API_INDEX_TIMER_GPIOSetPolarity,
    ROM_API_INDEX_TIMER_CH0_SetMeasurePulseWidth,
    ROM_API_INDEX_TIMER_CH0_GetMeasurePulseWidth,
    ROM_API_INDEX_TIMER_ForceClockDivision,

    ROM_API_INDEX_WDT_Set_Clock,
    ROM_API_INDEX_WDT_GetMatchValue,
    ROM_API_INDEX_WDT_SetCompValue,
    ROM_API_INDEX_WDT_CompValueEffectImmediately,
    ROM_API_INDEX_WDT_GetCounterValue,
    ROM_API_INDEX_WDT_ResetCounterValue,
    ROM_API_INDEX_WDT_GetResetStatus,
    ROM_API_INDEX_WDT_ClearResetStatus,
    ROM_API_INDEX_WDT_Enable,
    ROM_API_INDEX_WDT_Disable,
    ROM_API_INDEX_WDT_ForceClockDivision,
    ROM_API_INDEX_WDT_IntMask,
    ROM_API_INDEX_WDT_GPIOSetPolarity,

    ROM_API_INDEX_arch_memcpy,
    ROM_API_INDEX_arch_memcpy4,
    ROM_API_INDEX_arch_memcpy_fast,
    ROM_API_INDEX_arch_memset,
    ROM_API_INDEX_arch_memset4,
    ROM_API_INDEX_arch_memcmp,

    ROM_API_INDEX_memcopy_to_fifo,

    ROM_API_INDEX_fifocopy_to_mem,

    ROM_API_INDEX_BFLB_Soft_CRC32_Ex,
    ROM_API_INDEX_BFLB_Soft_CRC32,

    ROM_API_INDEX_FUNC_EMPTY_START,

    ROM_API_INDEX_FUNC_EMPTY_END = ROMAPI_INDEX_MAX

} ROM_API_INDEX_e;

/*@} end of group ROMDRIVER_Public_Types */

/** @defgroup  ROMDRIVER_Public_Constants
 *  @{
 */

/*@} end of group ROMDRIVER_Public_Constants */

/** @defgroup  ROMDRIVER_Public_Macros
 *  @{
 */


#define ROM_APITABLE            ((uint32_t *)0x21010800)


#define RomDriver_AON_Power_On_MBG                        \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_MBG])

#define RomDriver_AON_Power_Off_MBG                       \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_MBG])

#define RomDriver_AON_Power_On_XTAL                       \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_XTAL])

#define RomDriver_AON_Set_Xtal_CapCode                    \
    ((BL_Err_Type (*) (uint8_t capIn, uint8_t capOut))ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_CapCode])

#define RomDriver_AON_Get_Xtal_CapCode                    \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Get_Xtal_CapCode])

#define RomDriver_AON_Set_Xtal_CapCode_Extra              \
    ((BL_Err_Type (*) (uint8_t extra))ROM_APITABLE[ROM_API_INDEX_AON_Set_Xtal_CapCode_Extra])

#define RomDriver_AON_Power_Off_XTAL                      \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_XTAL])

#define RomDriver_AON_Power_On_BG                         \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_BG])

#define RomDriver_AON_Power_Off_BG                        \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_BG])

#define RomDriver_AON_Power_On_LDO11_SOC                  \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_LDO11_SOC])

#define RomDriver_AON_Power_Off_LDO11_SOC                 \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_LDO11_SOC])

#define RomDriver_AON_Power_On_SFReg                      \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_On_SFReg])

#define RomDriver_AON_Power_Off_SFReg                     \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Power_Off_SFReg])

#define RomDriver_AON_Set_LDO11_SOC_Sstart_Delay          \
    ((BL_Err_Type (*) (uint8_t delay))ROM_APITABLE[ROM_API_INDEX_AON_Set_LDO11_SOC_Sstart_Delay])

#define RomDriver_AON_Set_DCDC14_Top_0                    \
    ((BL_Err_Type (*) (uint8_t voutSel, uint8_t vpfm))ROM_APITABLE[ROM_API_INDEX_AON_Set_DCDC14_Top_0])

#define RomDriver_AON_Trim_Ldo11socVoutTrim               \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo11socVoutTrim])

#define RomDriver_AON_Trim_Ldo14VoutTrim                  \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Trim_Ldo14VoutTrim])

#define RomDriver_AON_Trim_Dcdc14VoutTrim                 \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_AON_Trim_Dcdc14VoutTrim])

#define RomDriver_Clock_System_Clock_Get                  \
    ((uint32_t (*) (BL_System_Clock_Type type))ROM_APITABLE[ROM_API_INDEX_Clock_System_Clock_Get])

#define RomDriver_Clock_Peripheral_Clock_Get              \
    ((uint32_t (*) (BL_Peripheral_Type type))ROM_APITABLE[ROM_API_INDEX_Clock_Peripheral_Clock_Get])

#define RomDriver_SystemCoreClockGet                      \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SystemCoreClockGet])

#define RomDriver_CPU_Get_MTimer_Clock                    \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_Clock])

#define RomDriver_CPU_Get_MTimer_Counter                  \
    ((uint64_t (*) (void))ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_Counter])

#define RomDriver_CPU_Get_CPU_Cycle                       \
    ((uint64_t (*) (void))ROM_APITABLE[ROM_API_INDEX_CPU_Get_CPU_Cycle])

#define RomDriver_CPU_Get_MTimer_US                       \
    ((uint64_t (*) (void))ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_US])

#define RomDriver_CPU_Get_MTimer_MS                       \
    ((uint64_t (*) (void))ROM_APITABLE[ROM_API_INDEX_CPU_Get_MTimer_MS])

#define RomDriver_CPU_MTimer_Delay_US                     \
    ((BL_Err_Type (*) (uint32_t cnt))ROM_APITABLE[ROM_API_INDEX_CPU_MTimer_Delay_US])

#define RomDriver_CPU_MTimer_Delay_MS                     \
    ((BL_Err_Type (*) (uint32_t cnt))ROM_APITABLE[ROM_API_INDEX_CPU_MTimer_Delay_MS])

#define RomDriver_BL702L_Delay_US                         \
    ((void (*) (uint32_t cnt))ROM_APITABLE[ROM_API_INDEX_BL702L_Delay_US])

#define RomDriver_BL702L_Delay_MS                         \
    ((void (*) (uint32_t cnt))ROM_APITABLE[ROM_API_INDEX_BL702L_Delay_MS])

#define RomDriver_EF_Ctrl_Get_Trim_Parity                 \
    ((uint8_t (*) (uint32_t val, uint8_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Get_Trim_Parity])

#define RomDriver_EF_Ctrl_Read_Common_Trim                \
    ((void (*) (char *name, Efuse_Common_Trim_Type *trim))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Common_Trim])

#define RomDriver_EF_Ctrl_Write_Common_Trim               \
    ((void (*) (char *name, uint8_t trim_en, uint32_t trim_value))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Common_Trim])

#define RomDriver_EF_Ctrl_Is_MAC_Address_Slot_Empty       \
    ((uint8_t (*) (uint8_t slot, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_MAC_Address_Slot_Empty])

#define RomDriver_EF_Ctrl_Write_MAC_Address_Opt           \
    ((BL_Err_Type (*) (uint8_t slot, uint8_t mac[8], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_MAC_Address_Opt])

#define RomDriver_EF_Ctrl_Read_MAC_Address_Opt            \
    ((BL_Err_Type (*) (uint8_t slot, uint8_t mac[8], uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_MAC_Address_Opt])

#define RomDriver_EF_Ctrl_Sw_AHB_Clk_0                    \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Sw_AHB_Clk_0])

#define RomDriver_EF_Ctrl_Program_Efuse_0                 \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Program_Efuse_0])

#define RomDriver_EF_Ctrl_Load_Efuse_R0                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Load_Efuse_R0])

#define RomDriver_EF_Ctrl_Busy                            \
    ((BL_Sts_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Busy])

#define RomDriver_EF_Ctrl_AutoLoad_Done                   \
    ((BL_Sts_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_AutoLoad_Done])

#define RomDriver_EF_Ctrl_Write_Dbg_Pwd                   \
    ((void (*) (uint32_t passWdLow, uint32_t passWdHigh, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Read_Dbg_Pwd                    \
    ((void (*) (uint32_t *passWdLow, uint32_t *passWdHigh))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Readlock_Dbg_Pwd                \
    ((void (*) (uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Readlock_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Writelock_Dbg_Pwd               \
    ((void (*) (uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_Dbg_Pwd])

#define RomDriver_EF_Ctrl_Write_Secure_Cfg                \
    ((void (*) (EF_Ctrl_Sec_Param_Type *cfg, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Secure_Cfg])

#define RomDriver_EF_Ctrl_Read_Secure_Cfg                 \
    ((void (*) (EF_Ctrl_Sec_Param_Type *cfg))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Secure_Cfg])

#define RomDriver_EF_Ctrl_Write_Secure_Boot               \
    ((void (*) (uint8_t sign[1], uint8_t aes[1], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Secure_Boot])

#define RomDriver_EF_Ctrl_Read_Secure_Boot                \
    ((void (*) (uint8_t sign[1], uint8_t aes[1]))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Secure_Boot])

#define RomDriver_EF_Ctrl_Write_Sw_Usage                  \
    ((void (*) (uint32_t index, uint32_t usage, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_Sw_Usage])

#define RomDriver_EF_Ctrl_Read_Sw_Usage                   \
    ((void (*) (uint32_t index, uint32_t *usage))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Sw_Usage])

#define RomDriver_EF_Ctrl_Writelock_Sw_Usage              \
    ((void (*) (uint32_t index, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_Sw_Usage])

#define RomDriver_EF_Ctrl_Write_MAC_Address               \
    ((void (*) (uint8_t mac[8], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_MAC_Address])

#define RomDriver_EF_Ctrl_Get_Byte_Zero_Cnt               \
    ((uint32_t (*) (uint8_t val))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Get_Byte_Zero_Cnt])

#define RomDriver_EF_Ctrl_Is_All_Bits_Zero                \
    ((uint8_t (*) (uint32_t val, uint8_t start, uint8_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_All_Bits_Zero])

#define RomDriver_EF_Ctrl_Read_MAC_Address                \
    ((BL_Err_Type (*) (uint8_t mac[8]))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_MAC_Address])

#define RomDriver_EF_Ctrl_Writelock_MAC_Address           \
    ((void (*) (uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_MAC_Address])

#define RomDriver_EF_Ctrl_Read_Chip_ID                    \
    ((BL_Err_Type (*) (uint8_t chipID[8]))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Chip_ID])

#define RomDriver_EF_Ctrl_Read_Device_Info                \
    ((void (*) (Efuse_Device_Info_Type *deviceInfo))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Device_Info])

#define RomDriver_EF_Ctrl_Is_CapCode_Empty                \
    ((uint8_t (*) (uint8_t slot, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_CapCode_Empty])

#define RomDriver_EF_Ctrl_Write_CapCode_Opt               \
    ((BL_Err_Type (*) (uint8_t slot, uint8_t code, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_CapCode_Opt])

#define RomDriver_EF_Ctrl_Read_CapCode_Opt                \
    ((BL_Err_Type (*) (uint8_t slot, uint8_t *code, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_CapCode_Opt])

#define RomDriver_EF_Ctrl_Is_PowerOffset_Slot_Empty       \
    ((uint8_t (*) (uint8_t slot, uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Is_PowerOffset_Slot_Empty])

#define RomDriver_EF_Ctrl_Write_PowerOffset_Opt           \
    ((BL_Err_Type (*) (uint8_t slot, int8_t pwrOffset[2], uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_PowerOffset_Opt])

#define RomDriver_EF_Ctrl_Read_PowerOffset_Opt            \
    ((BL_Err_Type (*) (uint8_t slot, int8_t pwrOffset[2], uint8_t reload))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_PowerOffset_Opt])

#define RomDriver_EF_Ctrl_Write_AES_Key                   \
    ((void (*) (uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Write_AES_Key])

#define RomDriver_EF_Ctrl_Read_AES_Key                    \
    ((void (*) (uint8_t index, uint32_t *keyData, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_AES_Key])

#define RomDriver_EF_Ctrl_Writelock_AES_Key               \
    ((void (*) (uint8_t index, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Writelock_AES_Key])

#define RomDriver_EF_Ctrl_Readlock_AES_Key                \
    ((void (*) (uint8_t index, uint8_t program))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Readlock_AES_Key])

#define RomDriver_EF_Ctrl_Program_Direct_R0               \
    ((void (*) (uint32_t index, uint32_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Program_Direct_R0])

#define RomDriver_EF_Ctrl_Read_Direct_R0                  \
    ((void (*) (uint32_t index, uint32_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Read_Direct_R0])

#define RomDriver_EF_Ctrl_Clear                           \
    ((void (*) (uint32_t index, uint32_t len))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Clear])

#define RomDriver_EF_Ctrl_Crc_Enable                      \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Enable])

#define RomDriver_EF_Ctrl_Crc_Is_Busy                     \
    ((BL_Sts_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Is_Busy])

#define RomDriver_EF_Ctrl_Crc_Set_Golden                  \
    ((void (*) (uint32_t goldenValue))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Set_Golden])

#define RomDriver_EF_Ctrl_Crc_Result                      \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_EF_Ctrl_Crc_Result])

#define RomDriver_GLB_Get_Root_CLK_Sel                    \
    ((GLB_ROOT_CLK_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_Root_CLK_Sel])

#define RomDriver_GLB_Set_System_CLK_Div                  \
    ((BL_Err_Type (*) (uint8_t hclkDiv, uint8_t bclkDiv))ROM_APITABLE[ROM_API_INDEX_GLB_Set_System_CLK_Div])

#define RomDriver_GLB_Get_BCLK_Div                        \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_BCLK_Div])

#define RomDriver_GLB_Get_HCLK_Div                        \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_HCLK_Div])

#define RomDriver_GLB_Set_System_CLK                      \
    ((BL_Err_Type (*) (GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq))ROM_APITABLE[ROM_API_INDEX_GLB_Set_System_CLK])

#define RomDriver_System_Core_Clock_Update_From_RC32M     \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_System_Core_Clock_Update_From_RC32M])

#define RomDriver_GLB_Set_MAC154_ZIGBEE_CLK               \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_MAC154_ZIGBEE_CLK])

#define RomDriver_GLB_Set_BLE_CLK                         \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_BLE_CLK])

#define RomDriver_GLB_Set_AUDIO_CLK                       \
    ((BL_Err_Type (*) (uint8_t clkDivEn, uint8_t autoDivEn, GLB_AUDIO_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_AUDIO_CLK])

#define RomDriver_GLB_Set_KYS_CLK                         \
    ((BL_Err_Type (*) (GLB_KYS_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_KYS_CLK])

#define RomDriver_GLB_Set_DMA_CLK                         \
    ((BL_Err_Type (*) (uint8_t enable, GLB_DMA_CLK_ID_Type clk))ROM_APITABLE[ROM_API_INDEX_GLB_Set_DMA_CLK])

#define RomDriver_GLB_Set_IR_CLK                          \
    ((BL_Err_Type (*) (uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_IR_CLK])

#define RomDriver_GLB_Set_SF_CLK                          \
    ((BL_Err_Type (*) (uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SF_CLK])

#define RomDriver_GLB_Set_UART_CLK                        \
    ((BL_Err_Type (*) (uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_UART_CLK])

#define RomDriver_GLB_Sel_TMR_GPIO_Clock                  \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Sel_TMR_GPIO_Clock])

#define RomDriver_GLB_Set_Chip_Out_0_CLK_Sel              \
    ((BL_Err_Type (*) (GLB_CHIP_CLK_OUT_0_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Sel])

#define RomDriver_GLB_Set_Chip_Out_1_CLK_Sel              \
    ((BL_Err_Type (*) (GLB_CHIP_CLK_OUT_1_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Sel])

#define RomDriver_GLB_Set_Chip_Out_0_CLK_Enable           \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_0_CLK_Enable])

#define RomDriver_GLB_Set_Chip_Out_1_CLK_Enable           \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Chip_Out_1_CLK_Enable])

#define RomDriver_GLB_Set_I2C_CLK                         \
    ((BL_Err_Type (*) (uint8_t enable, GLB_I2C_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_I2C_CLK])

#define RomDriver_GLB_Set_SPI_CLK                         \
    ((BL_Err_Type (*) (uint8_t enable, GLB_SPI_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI_CLK])

#define RomDriver_GLB_Set_PKA_CLK_Sel                     \
    ((BL_Err_Type (*) (GLB_PKA_CLK_SRC_Type clkSel))ROM_APITABLE[ROM_API_INDEX_GLB_Set_PKA_CLK_Sel])

#define RomDriver_GLB_SW_System_Reset                     \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_SW_System_Reset])

#define RomDriver_GLB_SW_CPU_Reset                        \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_SW_CPU_Reset])

#define RomDriver_GLB_SW_POR_Reset                        \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_SW_POR_Reset])

#define RomDriver_GLB_AHB_MCU_Software_Reset              \
    ((BL_Err_Type (*) (GLB_AHB_MCU_SW_Type swrst))ROM_APITABLE[ROM_API_INDEX_GLB_AHB_MCU_Software_Reset])

#define RomDriver_GLB_Disrst_Set                          \
    ((BL_Err_Type (*) (uint8_t enable, GLB_DISRST_Type disrst))ROM_APITABLE[ROM_API_INDEX_GLB_Disrst_Set])

#define RomDriver_GLB_MAC154_ZIGBEE_Reset                 \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_MAC154_ZIGBEE_Reset])

#define RomDriver_GLB_BLE_Reset                           \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_BLE_Reset])

#define RomDriver_GLB_PER_Clock_Gate                      \
    ((BL_Err_Type (*) (uint64_t ips))ROM_APITABLE[ROM_API_INDEX_GLB_PER_Clock_Gate])

#define RomDriver_GLB_PER_Clock_UnGate                    \
    ((BL_Err_Type (*) (uint64_t ips))ROM_APITABLE[ROM_API_INDEX_GLB_PER_Clock_UnGate])

#define RomDriver_GLB_Set_RTC_Mode                        \
    ((BL_Err_Type (*) (RTC_MODE_Type rtcMode))ROM_APITABLE[ROM_API_INDEX_GLB_Set_RTC_Mode])

#define RomDriver_GLB_BMX_Init                            \
    ((BL_Err_Type (*) (BMX_Cfg_Type *BmxCfg))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_Init])

#define RomDriver_GLB_BMX_Addr_Monitor_Enable             \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_Addr_Monitor_Enable])

#define RomDriver_GLB_BMX_Addr_Monitor_Disable            \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_Addr_Monitor_Disable])

#define RomDriver_GLB_BMX_BusErrResponse_Enable           \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_BusErrResponse_Enable])

#define RomDriver_GLB_BMX_BusErrResponse_Disable          \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_BusErrResponse_Disable])

#define RomDriver_GLB_BMX_Get_Status                      \
    ((BL_Sts_Type (*) (BMX_BUS_ERR_Type errType))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_Get_Status])

#define RomDriver_GLB_BMX_Get_Err_Addr                    \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_Get_Err_Addr])

#define RomDriver_GLB_BMX_BusErrClr_Set                   \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_BMX_BusErrClr_Set])

#define RomDriver_GLB_Set_SRAM_PARM                       \
    ((BL_Err_Type (*) (uint32_t value))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SRAM_PARM])

#define RomDriver_GLB_Get_SRAM_PARM                       \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_SRAM_PARM])

#define RomDriver_GLB_Set_OCRAM_PARM                      \
    ((BL_Err_Type (*) (uint32_t value))ROM_APITABLE[ROM_API_INDEX_GLB_Set_OCRAM_PARM])

#define RomDriver_GLB_Get_OCRAM_PARM                      \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_OCRAM_PARM])

#define RomDriver_GLB_Set_EM_Sel                          \
    ((BL_Err_Type (*) (GLB_EM_Type emType))ROM_APITABLE[ROM_API_INDEX_GLB_Set_EM_Sel])

#define RomDriver_GLB_Set_Kys_Drv_Col                     \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Kys_Drv_Col])

#define RomDriver_GLB_GPIO_O_Latch_Mode_Set               \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_O_Latch_Mode_Set])

#define RomDriver_GLB_JTAG_Sig_Swap_Set                   \
    ((BL_Err_Type (*) (uint8_t swapSel))ROM_APITABLE[ROM_API_INDEX_GLB_JTAG_Sig_Swap_Set])

#define RomDriver_GLB_CCI_Use_IO_0_1_2_7                  \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_CCI_Use_IO_0_1_2_7])

#define RomDriver_GLB_CCI_Use_Jtag_Pin                    \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_CCI_Use_Jtag_Pin])

#define RomDriver_GLB_Swap_SPI_0_MOSI_With_MISO           \
    ((BL_Err_Type (*) (BL_Fun_Type newState))ROM_APITABLE[ROM_API_INDEX_GLB_Swap_SPI_0_MOSI_With_MISO])

#define RomDriver_GLB_Set_SPI_0_ACT_MOD_Sel               \
    ((BL_Err_Type (*) (GLB_SPI_PAD_ACT_AS_Type mod))ROM_APITABLE[ROM_API_INDEX_GLB_Set_SPI_0_ACT_MOD_Sel])

#define RomDriver_GLB_Set_Flash_Scenario                  \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Flash_Scenario])

#define RomDriver_GLB_Set_Embedded_FLash_IO_PARM          \
    ((BL_Err_Type (*) (uint8_t reverse, uint8_t swapIo3Io0, uint8_t swapIo2Cs))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Embedded_FLash_IO_PARM])

#define RomDriver_GLB_Set_MTimer_CLK                      \
    ((BL_Err_Type (*) (uint8_t enable, GLB_MTIMER_CLK_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_MTimer_CLK])

#define RomDriver_GLB_Set_ADC_CLK                         \
    ((BL_Err_Type (*) (uint8_t enable, GLB_ADC_CLK_SRC_Type clkSel, uint8_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_ADC_CLK])

#define RomDriver_GLB_Set_DIG_32K_CLK                     \
    ((BL_Err_Type (*) (uint8_t enable, uint8_t compensation, GLB_DIG_CLK_SRC_Type clkSel, uint16_t div))ROM_APITABLE[ROM_API_INDEX_GLB_Set_DIG_32K_CLK])

#define RomDriver_GLB_SW_BLE_WAKEUP_REQ_Set               \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_SW_BLE_WAKEUP_REQ_Set])

#define RomDriver_GLB_UART_Fun_Sel                        \
    ((BL_Err_Type (*) (GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun))ROM_APITABLE[ROM_API_INDEX_GLB_UART_Fun_Sel])

#define RomDriver_GLB_Power_Off_DLL                       \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Power_Off_DLL])

#define RomDriver_GLB_Power_On_DLL                        \
    ((BL_Err_Type (*) (GLB_DLL_XTAL_Type xtalType))ROM_APITABLE[ROM_API_INDEX_GLB_Power_On_DLL])

#define RomDriver_GLB_Enable_DLL_All_Clks                 \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Enable_DLL_All_Clks])

#define RomDriver_GLB_Enable_DLL_Clk                      \
    ((BL_Err_Type (*) (GLB_DLL_CLK_Type dllClk))ROM_APITABLE[ROM_API_INDEX_GLB_Enable_DLL_Clk])

#define RomDriver_GLB_Disable_DLL_All_Clks                \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Disable_DLL_All_Clks])

#define RomDriver_GLB_Disable_DLL_Clk                     \
    ((BL_Err_Type (*) (GLB_DLL_CLK_Type dllClk))ROM_APITABLE[ROM_API_INDEX_GLB_Disable_DLL_Clk])

#define RomDriver_GLB_Set_Flash_Id_Value                  \
    ((BL_Err_Type (*) (uint32_t idValue))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Flash_Id_Value])

#define RomDriver_GLB_Get_Flash_Id_Value                  \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Get_Flash_Id_Value])

#define RomDriver_GLB_Trim_RC32M                          \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Trim_RC32M])

#define RomDriver_GLB_Set_Xtal_Cnt32k_Process             \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Set_Xtal_Cnt32k_Process])

#define RomDriver_GLB_Clear_Xtal_Cnt32k_Done              \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_Clear_Xtal_Cnt32k_Done])

#define RomDriver_GLB_RC32K_Deg_Start                     \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_RC32K_Deg_Start])

#define RomDriver_GLB_RC32K_Deg_End                       \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_RC32K_Deg_End])

#define RomDriver_GLB_RC32K_Deg_Enable                    \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_GLB_RC32K_Deg_Enable])

#define RomDriver_GLB_Xtal_Deg_Cnt_Limit_Set              \
    ((BL_Err_Type (*) (uint8_t cnt))ROM_APITABLE[ROM_API_INDEX_GLB_Xtal_Deg_Cnt_Limit_Set])

#define RomDriver_GLB_IR_LED_Driver_Enable                \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_IR_LED_Driver_Enable])

#define RomDriver_GLB_IR_LED_Driver_Disable               \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_GLB_IR_LED_Driver_Disable])

#define RomDriver_GLB_IR_LED_Driver_Output_Enable         \
    ((BL_Err_Type (*) (GLB_IR_LED_Type led))ROM_APITABLE[ROM_API_INDEX_GLB_IR_LED_Driver_Output_Enable])

#define RomDriver_GLB_IR_LED_Driver_Output_Disable        \
    ((BL_Err_Type (*) (GLB_IR_LED_Type led))ROM_APITABLE[ROM_API_INDEX_GLB_IR_LED_Driver_Output_Disable])

#define RomDriver_GLB_IR_LED_Driver_Ibias                 \
    ((BL_Err_Type (*) (uint8_t ibias))ROM_APITABLE[ROM_API_INDEX_GLB_IR_LED_Driver_Ibias])

#define RomDriver_GLB_GPIO_Init                           \
    ((BL_Err_Type (*) (GLB_GPIO_Cfg_Type *cfg))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Init])

#define RomDriver_GLB_GPIO_Func_Init                      \
    ((BL_Err_Type (*) (GLB_GPIO_FUNC_Type gpioFun, GLB_GPIO_Type *pinList, uint8_t cnt))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Func_Init])

#define RomDriver_GLB_GPIO_OUTPUT_Mode_Set                \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin, GLB_GPIO_OUTPUT_MODE_Type mode))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_OUTPUT_Mode_Set])

#define RomDriver_GLB_GPIO_INPUT_Enable                   \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_INPUT_Enable])

#define RomDriver_GLB_GPIO_INPUT_Disable                  \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_INPUT_Disable])

#define RomDriver_GLB_GPIO_OUTPUT_Enable                  \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_OUTPUT_Enable])

#define RomDriver_GLB_GPIO_OUTPUT_Disable                 \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_OUTPUT_Disable])

#define RomDriver_GLB_GPIO_Set_HZ                         \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Set_HZ])

#define RomDriver_GLB_GPIO_Get_Fun                        \
    ((uint8_t (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Get_Fun])

#define RomDriver_GLB_GPIO_Write                          \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin, uint32_t val))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Write])

#define RomDriver_GLB_GPIO_Read                           \
    ((uint32_t (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Read])

#define RomDriver_GLB_GPIO_Set                            \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Set])

#define RomDriver_GLB_GPIO_Clr                            \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Clr])

#define RomDriver_GLB_GPIO_IntMask                        \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_IntMask])

#define RomDriver_GLB_Clr_GPIO_IntStatus                  \
    ((BL_Err_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Clr_GPIO_IntStatus])

#define RomDriver_GLB_Get_GPIO_IntStatus                  \
    ((BL_Sts_Type (*) (GLB_GPIO_Type gpioPin))ROM_APITABLE[ROM_API_INDEX_GLB_Get_GPIO_IntStatus])

#define RomDriver_GLB_GPIO_Int_Init                       \
    ((BL_Err_Type (*) (GLB_GPIO_INT_Cfg_Type *intCfg))ROM_APITABLE[ROM_API_INDEX_GLB_GPIO_Int_Init])

#define RomDriver_HBN_Mode_Enter                          \
    ((void (*) (HBN_APP_CFG_Type *cfg))ROM_APITABLE[ROM_API_INDEX_HBN_Mode_Enter])

#define RomDriver_HBN_GPIO_Wakeup_Set                     \
    ((void (*) (uint16_t gpio_wakeup_src, HBN_GPIO_INT_Trigger_Type gpio_trig_type))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_Wakeup_Set])

#define RomDriver_HBN_Power_Down_Flash                    \
    ((void (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Power_Down_Flash])

#define RomDriver_HBN_Enable                              \
    ((void (*) (HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Enable])

#define RomDriver_HBN_Reset                               \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Reset])

#define RomDriver_HBN_App_Reset                           \
    ((BL_Err_Type (*) (uint8_t npXtalType, uint8_t bclkDiv, uint8_t apXtalType, uint8_t fclkDiv))ROM_APITABLE[ROM_API_INDEX_HBN_App_Reset])

#define RomDriver_HBN_Disable                             \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Disable])

#define RomDriver_HBN_Get_BOR_OUT_State                   \
    ((BL_Sts_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_BOR_OUT_State])

#define RomDriver_HBN_Set_BOR_Config                      \
    ((BL_Err_Type (*) (uint8_t enable, HBN_BOR_THRES_Type threshold, HBN_BOR_MODE_Type mode))ROM_APITABLE[ROM_API_INDEX_HBN_Set_BOR_Config])

#define RomDriver_HBN_Set_Ldo11_Aon_Vout                  \
    ((BL_Err_Type (*) (HBN_LDO_LEVEL_Type ldoLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo11_Aon_Vout])

#define RomDriver_HBN_Set_Ldo11_Soc_Vout                  \
    ((BL_Err_Type (*) (HBN_LDO_LEVEL_Type ldoLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo11_Soc_Vout])

#define RomDriver_HBN_Set_Ldo11_All_Vout                  \
    ((BL_Err_Type (*) (HBN_LDO_LEVEL_Type ldoLevel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Ldo11_All_Vout])

#define RomDriver_HBN_32K_Sel                             \
    ((BL_Err_Type (*) (HBN_32K_CLK_Type clkType))ROM_APITABLE[ROM_API_INDEX_HBN_32K_Sel])

#define RomDriver_HBN_Set_UART_CLK_Sel                    \
    ((BL_Err_Type (*) (HBN_UART_CLK_Type clkSel))ROM_APITABLE[ROM_API_INDEX_HBN_Set_UART_CLK_Sel])

#define RomDriver_HBN_Set_XCLK_CLK_Sel                    \
    ((BL_Err_Type (*) (HBN_XCLK_CLK_Type xClk))ROM_APITABLE[ROM_API_INDEX_HBN_Set_XCLK_CLK_Sel])

#define RomDriver_HBN_Set_ROOT_CLK_Sel                    \
    ((BL_Err_Type (*) (HBN_ROOT_CLK_Type rootClk))ROM_APITABLE[ROM_API_INDEX_HBN_Set_ROOT_CLK_Sel])

#define RomDriver_HBN_Set_HRAM_slp                        \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Set_HRAM_slp])

#define RomDriver_HBN_Set_HRAM_Ret                        \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Set_HRAM_Ret])

#define RomDriver_HBN_Power_On_Xtal_32K                   \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_On_Xtal_32K])

#define RomDriver_HBN_Power_Off_Xtal_32K                  \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_Xtal_32K])

#define RomDriver_HBN_Power_On_RC32K                      \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_On_RC32K])

#define RomDriver_HBN_Power_Off_RC32K                     \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Power_Off_RC32K])

#define RomDriver_HBN_Trim_RC32K                          \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Trim_RC32K])

#define RomDriver_HBN_Get_Status_Flag                     \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Status_Flag])

#define RomDriver_HBN_Set_Status_Flag                     \
    ((BL_Err_Type (*) (uint32_t flag))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Status_Flag])

#define RomDriver_HBN_Get_Wakeup_Addr                     \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Wakeup_Addr])

#define RomDriver_HBN_Set_Wakeup_Addr                     \
    ((BL_Err_Type (*) (uint32_t addr))ROM_APITABLE[ROM_API_INDEX_HBN_Set_Wakeup_Addr])

#define RomDriver_HBN_Get_User_Boot_Config                \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_User_Boot_Config])

#define RomDriver_HBN_Set_User_Boot_Config                \
    ((BL_Err_Type (*) (uint8_t ubCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Set_User_Boot_Config])

#define RomDriver_HBN_Clear_RTC_Counter                   \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_RTC_Counter])

#define RomDriver_HBN_Enable_RTC_Counter                  \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_RTC_Counter])

#define RomDriver_HBN_Set_RTC_Timer                       \
    ((BL_Err_Type (*) (HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode))ROM_APITABLE[ROM_API_INDEX_HBN_Set_RTC_Timer])

#define RomDriver_HBN_Get_RTC_Timer_Val                   \
    ((BL_Err_Type (*) (uint32_t *valLow, uint32_t *valHigh))ROM_APITABLE[ROM_API_INDEX_HBN_Get_RTC_Timer_Val])

#define RomDriver_HBN_Clear_RTC_INT                       \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_RTC_INT])

#define RomDriver_HBN_GPIO_INT_Enable                     \
    ((BL_Err_Type (*) (HBN_GPIO_INT_Trigger_Type gpioIntTrigType))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_INT_Enable])

#define RomDriver_HBN_GPIO_INT_Disable                    \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_INT_Disable])

#define RomDriver_HBN_Get_INT_State                       \
    ((BL_Sts_Type (*) (HBN_INT_Type irqType))ROM_APITABLE[ROM_API_INDEX_HBN_Get_INT_State])

#define RomDriver_HBN_Get_Pin_Wakeup_Mode                 \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Pin_Wakeup_Mode])

#define RomDriver_HBN_Clear_IRQ                           \
    ((BL_Err_Type (*) (HBN_INT_Type irqType))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_IRQ])

#define RomDriver_HBN_Hw_Pu_Pd_Cfg                        \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_HBN_Hw_Pu_Pd_Cfg])

#define RomDriver_HBN_Comm_Pad_Wakeup_En_Cfg              \
    ((BL_Err_Type (*) (uint8_t padCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Comm_Pad_Wakeup_En_Cfg])

#define RomDriver_HBN_Aon_Pad_IeSmt_Cfg                   \
    ((BL_Err_Type (*) (uint8_t padCfg))ROM_APITABLE[ROM_API_INDEX_HBN_Aon_Pad_IeSmt_Cfg])

#define RomDriver_HBN_Pin_WakeUp_Mask                     \
    ((BL_Err_Type (*) (uint16_t maskVal))ROM_APITABLE[ROM_API_INDEX_HBN_Pin_WakeUp_Mask])

#define RomDriver_HBN_Enable_AComp0_IRQ                   \
    ((BL_Err_Type (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_AComp0_IRQ])

#define RomDriver_HBN_Disable_AComp0_IRQ                  \
    ((BL_Err_Type (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Disable_AComp0_IRQ])

#define RomDriver_HBN_Enable_AComp1_IRQ                   \
    ((BL_Err_Type (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_AComp1_IRQ])

#define RomDriver_HBN_Disable_AComp1_IRQ                  \
    ((BL_Err_Type (*) (HBN_ACOMP_INT_EDGE_Type edge))ROM_APITABLE[ROM_API_INDEX_HBN_Disable_AComp1_IRQ])

#define RomDriver_HBN_Enable_BOR_IRQ                      \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Enable_BOR_IRQ])

#define RomDriver_HBN_Disable_BOR_IRQ                     \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Disable_BOR_IRQ])

#define RomDriver_HBN_Get_Reset_Event                     \
    ((BL_Sts_Type (*) (HBN_RST_EVENT_Type event))ROM_APITABLE[ROM_API_INDEX_HBN_Get_Reset_Event])

#define RomDriver_HBN_Clear_Reset_Event                   \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_HBN_Clear_Reset_Event])

#define RomDriver_HBN_GPIO_Dbg_Pull_Cfg                   \
    ((BL_Err_Type (*) (BL_Fun_Type pupdEn, BL_Fun_Type dlyEn, uint8_t dlySec, HBN_INT_Type gpioIrq, BL_Mask_Type gpioMask))ROM_APITABLE[ROM_API_INDEX_HBN_GPIO_Dbg_Pull_Cfg])

#define RomDriver_HBN_Set_BOR_Cfg                         \
    ((BL_Err_Type (*) (HBN_BOR_CFG_Type *cfg))ROM_APITABLE[ROM_API_INDEX_HBN_Set_BOR_Cfg])

#define RomDriver_L1C_Cache_Write_Set                     \
    ((void (*) (BL_Fun_Type wtEn, BL_Fun_Type wbEn, BL_Fun_Type waEn))ROM_APITABLE[ROM_API_INDEX_L1C_Cache_Write_Set])

#define RomDriver_L1C_Cache_Enable_Set                    \
    ((BL_Err_Type (*) (uint8_t wayDisable))ROM_APITABLE[ROM_API_INDEX_L1C_Cache_Enable_Set])

#define RomDriver_L1C_Cache_Flush                         \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_Cache_Flush])

#define RomDriver_L1C_Cache_Hit_Count_Get                 \
    ((void (*) (uint32_t *hitCountLow, uint32_t *hitCountHigh))ROM_APITABLE[ROM_API_INDEX_L1C_Cache_Hit_Count_Get])

#define RomDriver_L1C_Cache_Miss_Count_Get                \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_Cache_Miss_Count_Get])

#define RomDriver_L1C_Cache_Read_Disable                  \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_Cache_Read_Disable])

#define RomDriver_L1C_Set_Wrap                            \
    ((BL_Err_Type (*) (BL_Fun_Type wrap))ROM_APITABLE[ROM_API_INDEX_L1C_Set_Wrap])

#define RomDriver_L1C_Set_Way_Disable                     \
    ((BL_Err_Type (*) (uint8_t disableVal))ROM_APITABLE[ROM_API_INDEX_L1C_Set_Way_Disable])

#define RomDriver_L1C_IROM_2T_Access_Set                  \
    ((BL_Err_Type (*) (uint8_t enable))ROM_APITABLE[ROM_API_INDEX_L1C_IROM_2T_Access_Set])

#define RomDriver_L1C_BMX_Init                            \
    ((BL_Err_Type (*) (L1C_BMX_Cfg_Type *l1cBmxCfg))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_Init])

#define RomDriver_L1C_BMX_Addr_Monitor_Enable             \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_Addr_Monitor_Enable])

#define RomDriver_L1C_BMX_Addr_Monitor_Disable            \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_Addr_Monitor_Disable])

#define RomDriver_L1C_BMX_BusErrResponse_Enable           \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_BusErrResponse_Enable])

#define RomDriver_L1C_BMX_BusErrResponse_Disable          \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_BusErrResponse_Disable])

#define RomDriver_L1C_BMX_Get_Status                      \
    ((BL_Sts_Type (*) (L1C_BMX_BUS_ERR_Type errType))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_Get_Status])

#define RomDriver_L1C_BMX_Get_Err_Addr                    \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_L1C_BMX_Get_Err_Addr])

#define RomDriver_PDS_Set_GPIO_Pad_Cfg                    \
    ((BL_Err_Type (*) (PDS_GPIO_Type pin, PDS_GPIO_Cfg_Type *cfg))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_Cfg])

#define RomDriver_PDS_GPIO_Write                          \
    ((BL_Err_Type (*) (PDS_GPIO_GROUP_SET_Type grp, uint32_t val))ROM_APITABLE[ROM_API_INDEX_PDS_GPIO_Write])

#define RomDriver_PDS_Set_GPIO_Pad_IntMask                \
    ((BL_Err_Type (*) (PDS_GPIO_GROUP_SET_Type grp, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMask])

#define RomDriver_PDS_Set_GPIO_Pad_IntMode                \
    ((BL_Err_Type (*) (PDS_GPIO_GROUP_SET_Type grp, PDS_GPIO_INT_TRIG_Type trig))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntMode])

#define RomDriver_PDS_Set_GPIO_Pad_IntClr                 \
    ((BL_Err_Type (*) (PDS_GPIO_GROUP_SET_Type grp))ROM_APITABLE[ROM_API_INDEX_PDS_Set_GPIO_Pad_IntClr])

#define RomDriver_PDS_Set_All_GPIO_IntClear               \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Set_All_GPIO_IntClear])

#define RomDriver_PDS_Get_GPIO_Pad_IntStatus              \
    ((BL_Sts_Type (*) (PDS_GPIO_Type pin))ROM_APITABLE[ROM_API_INDEX_PDS_Get_GPIO_Pad_IntStatus])

#define RomDriver_PDS_Disable_GPIO_Keep                   \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Disable_GPIO_Keep])

#define RomDriver_PDS_Enable                              \
    ((BL_Err_Type (*) (PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt))ROM_APITABLE[ROM_API_INDEX_PDS_Enable])

#define RomDriver_PDS_Force_Config                        \
    ((BL_Err_Type (*) (PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3))ROM_APITABLE[ROM_API_INDEX_PDS_Force_Config])

#define RomDriver_PDS_RAM_Config                          \
    ((BL_Err_Type (*) (PDS_RAM_CFG_Type *ramCfg))ROM_APITABLE[ROM_API_INDEX_PDS_RAM_Config])

#define RomDriver_PDS_Default_Level_Config                \
    ((BL_Err_Type (*) (PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt))ROM_APITABLE[ROM_API_INDEX_PDS_Default_Level_Config])

#define RomDriver_PDS_Wakeup_Src_En                       \
    ((BL_Err_Type (*) (PDS_WAKEUP_SRC_Type intType, BL_Fun_Type enable))ROM_APITABLE[ROM_API_INDEX_PDS_Wakeup_Src_En])

#define RomDriver_PDS_Get_Wakeup_Src                      \
    ((BL_Sts_Type (*) (PDS_WAKEUP_SRC_Type intType))ROM_APITABLE[ROM_API_INDEX_PDS_Get_Wakeup_Src])

#define RomDriver_PDS_IntMask                             \
    ((BL_Err_Type (*) (PDS_INT_Type intType, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_PDS_IntMask])

#define RomDriver_PDS_Get_IntStatus                       \
    ((BL_Sts_Type (*) (PDS_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_PDS_Get_IntStatus])

#define RomDriver_PDS_IntClear                            \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_IntClear])

#define RomDriver_PDS_Get_PdsRfStstus                     \
    ((PDS_RF_STS_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Get_PdsRfStstus])

#define RomDriver_PDS_Get_PdsStstus                       \
    ((PDS_STS_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Get_PdsStstus])

#define RomDriver_PDS_Clear_Reset_Event                   \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_PDS_Clear_Reset_Event])

#define RomDriver_PDS_Get_Reset_Event                     \
    ((BL_Sts_Type (*) (PDS_RST_EVENT_Type event))ROM_APITABLE[ROM_API_INDEX_PDS_Get_Reset_Event])

#define RomDriver_PDS_Auto_Enable                         \
    ((void (*) (PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, BL_Fun_Type enable))ROM_APITABLE[ROM_API_INDEX_PDS_Auto_Enable])

#define RomDriver_PDS_Manual_Force_Turn_Off               \
    ((void (*) (PDS_FORCE_Type domain))ROM_APITABLE[ROM_API_INDEX_PDS_Manual_Force_Turn_Off])

#define RomDriver_PDS_Manual_Force_Turn_On                \
    ((void (*) (PDS_FORCE_Type domain))ROM_APITABLE[ROM_API_INDEX_PDS_Manual_Force_Turn_On])

#define RomDriver_PDS_Set_KYD_Matrix_Size                 \
    ((BL_Err_Type (*) (uint8_t col_size, uint8_t row_size))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Matrix_Size])

#define RomDriver_PDS_Set_KYD_Col_Value                   \
    ((BL_Err_Type (*) (uint8_t val))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Col_Value])

#define RomDriver_PDS_Set_KYD_Row_Pull                    \
    ((BL_Err_Type (*) (uint8_t en))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Row_Pull])

#define RomDriver_PDS_Set_KYD_Wakeup_En                   \
    ((BL_Err_Type (*) (uint8_t en))ROM_APITABLE[ROM_API_INDEX_PDS_Set_KYD_Wakeup_En])

#define RomDriver_SF_Cfg_Init_Internal_Flash_Gpio         \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Init_Internal_Flash_Gpio])

#define RomDriver_SF_Cfg_Init_Ext_Flash_Gpio              \
    ((int (*) (uint8_t extFlashPin))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Init_Ext_Flash_Gpio])

#define RomDriver_SF_Cfg_Deinit_Ext_Flash_Gpio            \
    ((int (*) (uint8_t extFlashPin))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Deinit_Ext_Flash_Gpio])

#define RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock          \
    ((BL_Err_Type (*) (uint32_t flashID, spi_flash_cfg_type *pFlashCfg))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Get_Flash_Cfg_Need_Lock])

#define RomDriver_SF_Cfg_Init_Flash_Gpio                  \
    ((int (*) (uint8_t flashPinCfg, uint8_t restoreDefault))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Init_Flash_Gpio])

#define RomDriver_SF_Cfg_Flash_Identify                   \
    ((uint32_t (*) (uint8_t callFromFlash, uint32_t autoScan, uint32_t flashPinCfg, uint8_t restoreDefault, spi_flash_cfg_type *pFlashCfg))ROM_APITABLE[ROM_API_INDEX_SF_Cfg_Flash_Identify])

#define RomDriver_SF_Ctrl_Enable                          \
    ((void (*) (const struct sf_ctrl_cfg_type *cfg))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Enable])

#define RomDriver_SF_Ctrl_Psram_Init                      \
    ((void (*) (struct sf_ctrl_psram_cfg *sfCtrlPsramCfg))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Psram_Init])

#define RomDriver_SF_Ctrl_Get_Clock_Delay                 \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Get_Clock_Delay])

#define RomDriver_SF_Ctrl_Set_Clock_Delay                 \
    ((void (*) (uint8_t delay))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Set_Clock_Delay])

#define RomDriver_SF_Ctrl_Cmds_Set                        \
    ((void (*) (struct sf_ctrl_cmds_cfg *cmdsCfg, uint8_t sel))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Cmds_Set])

#define RomDriver_SF_Ctrl_Burst_Toggle_Set                \
    ((void (*) (uint8_t burstToggleEn, uint8_t mode))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Burst_Toggle_Set])

#define RomDriver_SF_Ctrl_Select_Pad                      \
    ((void (*) (uint8_t sel))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Select_Pad])

#define RomDriver_SF_Ctrl_Select_Bank                     \
    ((void (*) (uint8_t sel))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Select_Bank])

#define RomDriver_SF_Ctrl_Set_Owner                       \
    ((void (*) (uint8_t owner))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Set_Owner])

#define RomDriver_SF_Ctrl_Disable                         \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Disable])

#define RomDriver_SF_Ctrl_AES_Enable_BE                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Enable_BE])

#define RomDriver_SF_Ctrl_AES_Enable_LE                   \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Enable_LE])

#define RomDriver_SF_Ctrl_AES_Set_Region                  \
    ((void (*) (uint8_t region, uint8_t enable, uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_Region])

#define RomDriver_SF_Ctrl_AES_Set_Key                     \
    ((void (*) (uint8_t region, uint8_t *key, uint8_t keyType))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_Key])

#define RomDriver_SF_Ctrl_AES_Set_Key_BE                  \
    ((void (*) (uint8_t region, uint8_t *key, uint8_t keyType))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_Key_BE])

#define RomDriver_SF_Ctrl_AES_Set_IV                      \
    ((void (*) (uint8_t region, uint8_t *iv, uint32_t addrOffset))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_IV])

#define RomDriver_SF_Ctrl_AES_Set_IV_BE                   \
    ((void (*) (uint8_t region, uint8_t *iv, uint32_t addrOffset))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Set_IV_BE])

#define RomDriver_SF_Ctrl_AES_Enable                      \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Enable])

#define RomDriver_SF_Ctrl_AES_Disable                     \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_AES_Disable])

#define RomDriver_SF_Ctrl_Is_AES_Enable                   \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Is_AES_Enable])

#define RomDriver_SF_Ctrl_Set_Flash_Image_Offset          \
    ((void (*) (uint32_t addrOffset))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Set_Flash_Image_Offset])

#define RomDriver_SF_Ctrl_Get_Flash_Image_Offset          \
    ((uint32_t (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Get_Flash_Image_Offset])

#define RomDriver_SF_Ctrl_SendCmd                         \
    ((void (*) (struct sf_ctrl_cmd_cfg_type *cfg))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_SendCmd])

#define RomDriver_SF_Ctrl_Flash_Read_Icache_Set           \
    ((void (*) (struct sf_ctrl_cmd_cfg_type *cfg, uint8_t cmdValid))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Flash_Read_Icache_Set])

#define RomDriver_SF_Ctrl_Psram_Write_Icache_Set          \
    ((void (*) (struct sf_ctrl_cmd_cfg_type *cfg, uint8_t cmdValid))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Psram_Write_Icache_Set])

#define RomDriver_SF_Ctrl_Psram_Read_Icache_Set           \
    ((void (*) (struct sf_ctrl_cmd_cfg_type *cfg, uint8_t cmdValid))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_Psram_Read_Icache_Set])

#define RomDriver_SF_Ctrl_GetBusyState                    \
    ((BL_Sts_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_SF_Ctrl_GetBusyState])

#define RomDriver_SFlash_Init                             \
    ((void (*) (const struct sf_ctrl_cfg_type *pSfCtrlCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Init])

#define RomDriver_SFlash_SetSPIMode                       \
    ((BL_Err_Type (*) (uint8_t mode))ROM_APITABLE[ROM_API_INDEX_SFlash_SetSPIMode])

#define RomDriver_SFlash_Read_Reg                         \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Read_Reg])

#define RomDriver_SFlash_Write_Reg                        \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Write_Reg])

#define RomDriver_SFlash_Read_Reg_With_Cmd                \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Read_Reg_With_Cmd])

#define RomDriver_SFlash_Write_Reg_With_Cmd               \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen))ROM_APITABLE[ROM_API_INDEX_SFlash_Write_Reg_With_Cmd])

#define RomDriver_SFlash_Clear_Status_Register            \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Clear_Status_Register])

#define RomDriver_SFlash_Busy                             \
    ((BL_Sts_Type (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Busy])

#define RomDriver_SFlash_Write_Enable                     \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Write_Enable])

#define RomDriver_SFlash_Qspi_Enable                      \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Qspi_Enable])

#define RomDriver_SFlash_Volatile_Reg_Write_Enable        \
    ((void (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Volatile_Reg_Write_Enable])

#define RomDriver_SFlash_Chip_Erase                       \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Chip_Erase])

#define RomDriver_SFlash_Sector_Erase                     \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint32_t secNum))ROM_APITABLE[ROM_API_INDEX_SFlash_Sector_Erase])

#define RomDriver_SFlash_Blk32_Erase                      \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint32_t blkNum))ROM_APITABLE[ROM_API_INDEX_SFlash_Blk32_Erase])

#define RomDriver_SFlash_Blk64_Erase                      \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint32_t blkNum))ROM_APITABLE[ROM_API_INDEX_SFlash_Blk64_Erase])

#define RomDriver_SFlash_Erase                            \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint32_t startaddr, uint32_t endaddr))ROM_APITABLE[ROM_API_INDEX_SFlash_Erase])

#define RomDriver_SFlash_Program                          \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t ioMode, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_SFlash_Program])

#define RomDriver_SFlash_GetUniqueId                      \
    ((void (*) (uint8_t *data, uint8_t idLen))ROM_APITABLE[ROM_API_INDEX_SFlash_GetUniqueId])

#define RomDriver_SFlash_GetJedecId                       \
    ((void (*) (spi_flash_cfg_type *flashCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_SFlash_GetJedecId])

#define RomDriver_SFlash_GetDeviceId                      \
    ((void (*) (uint8_t *data))ROM_APITABLE[ROM_API_INDEX_SFlash_GetDeviceId])

#define RomDriver_SFlash_Powerdown                        \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SFlash_Powerdown])

#define RomDriver_SFlash_Releae_Powerdown                 \
    ((void (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Releae_Powerdown])

#define RomDriver_SFlash_Restore_From_Powerdown           \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint8_t flashContRead))ROM_APITABLE[ROM_API_INDEX_SFlash_Restore_From_Powerdown])

#define RomDriver_SFlash_SetBurstWrap                     \
    ((void (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_SetBurstWrap])

#define RomDriver_SFlash_DisableBurstWrap                 \
    ((void (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_DisableBurstWrap])

#define RomDriver_SFlash_Software_Reset                   \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Software_Reset])

#define RomDriver_SFlash_Reset_Continue_Read              \
    ((void (*) (spi_flash_cfg_type *flashCfg))ROM_APITABLE[ROM_API_INDEX_SFlash_Reset_Continue_Read])

#define RomDriver_SFlash_Set_IDbus_Cfg                    \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t ioMode, uint8_t contRead, uint32_t addr, uint32_t len))ROM_APITABLE[ROM_API_INDEX_SFlash_Set_IDbus_Cfg])

#define RomDriver_SFlash_IDbus_Read_Enable                \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t ioMode, uint8_t contRead))ROM_APITABLE[ROM_API_INDEX_SFlash_IDbus_Read_Enable])

#define RomDriver_SFlash_Cache_Read_Enable                \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t ioMode, uint8_t contRead, uint8_t wayDisable))ROM_APITABLE[ROM_API_INDEX_SFlash_Cache_Read_Enable])

#define RomDriver_SFlash_Cache_Read_Disable               \
    ((void (*) (void))ROM_APITABLE[ROM_API_INDEX_SFlash_Cache_Read_Disable])

#define RomDriver_SFlash_Read                             \
    ((BL_Err_Type (*) (spi_flash_cfg_type *flashCfg, uint8_t ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_SFlash_Read])

#define RomDriver_UART_SetBaudRate                        \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint32_t baudRate))ROM_APITABLE[ROM_API_INDEX_UART_SetBaudRate])

#define RomDriver_UART_Init                               \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_CFG_Type *uartCfg))ROM_APITABLE[ROM_API_INDEX_UART_Init])

#define RomDriver_UART_DeInit                             \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_DeInit])

#define RomDriver_UART_FifoConfig                         \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_FifoCfg_Type *fifoCfg))ROM_APITABLE[ROM_API_INDEX_UART_FifoConfig])

#define RomDriver_UART_Enable                             \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_UART_Enable])

#define RomDriver_UART_Disable                            \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_UART_Disable])

#define RomDriver_UART_SetTxDataLength                    \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint16_t length))ROM_APITABLE[ROM_API_INDEX_UART_SetTxDataLength])

#define RomDriver_UART_SetRxDataLength                    \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint16_t length))ROM_APITABLE[ROM_API_INDEX_UART_SetRxDataLength])

#define RomDriver_UART_SetRxTimeoutValue                  \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint8_t time))ROM_APITABLE[ROM_API_INDEX_UART_SetRxTimeoutValue])

#define RomDriver_UART_SetRxByteCount                     \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint16_t count))ROM_APITABLE[ROM_API_INDEX_UART_SetRxByteCount])

#define RomDriver_UART_SetDeglitchCount                   \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint8_t deglitchCnt))ROM_APITABLE[ROM_API_INDEX_UART_SetDeglitchCount])

#define RomDriver_UART_ApplyAbrResult                     \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet))ROM_APITABLE[ROM_API_INDEX_UART_ApplyAbrResult])

#define RomDriver_UART_SetRtsValue                        \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_SetRtsValue])

#define RomDriver_UART_ClrRtsValue                        \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_ClrRtsValue])

#define RomDriver_UART_SetTxValue                         \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_SetTxValue])

#define RomDriver_UART_ClrTxValue                         \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_ClrTxValue])

#define RomDriver_UART_TxFreeRun                          \
    ((BL_Err_Type (*) (UART_ID_Type uartId, BL_Fun_Type txFreeRun))ROM_APITABLE[ROM_API_INDEX_UART_TxFreeRun])

#define RomDriver_UART_AutoBaudDetection                  \
    ((BL_Err_Type (*) (UART_ID_Type uartId, BL_Fun_Type autoBaud))ROM_APITABLE[ROM_API_INDEX_UART_AutoBaudDetection])

#define RomDriver_UART_SetAllowableError0X55              \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint8_t allowableError))ROM_APITABLE[ROM_API_INDEX_UART_SetAllowableError0X55])

#define RomDriver_UART_GetBitWidth0X55                    \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint16_t *width))ROM_APITABLE[ROM_API_INDEX_UART_GetBitWidth0X55])

#define RomDriver_UART_SetRS485                           \
    ((BL_Err_Type (*) (UART_ID_Type uartId, BL_Fun_Type enable, UART_RS485Polarity_Type polarity))ROM_APITABLE[ROM_API_INDEX_UART_SetRS485])

#define RomDriver_UART_TxFifoClear                        \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_TxFifoClear])

#define RomDriver_UART_RxFifoClear                        \
    ((BL_Err_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_RxFifoClear])

#define RomDriver_UART_IntMask                            \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_INT_Type intType, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_UART_IntMask])

#define RomDriver_UART_IntClear                           \
    ((BL_Err_Type (*) (UART_ID_Type uartId, UART_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_UART_IntClear])

#define RomDriver_UART_SendData                           \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_UART_SendData])

#define RomDriver_UART_SendDataBlock                      \
    ((BL_Err_Type (*) (UART_ID_Type uartId, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_UART_SendDataBlock])

#define RomDriver_UART_ReceiveData                        \
    ((uint32_t (*) (UART_ID_Type uartId, uint8_t *data, uint32_t maxLen))ROM_APITABLE[ROM_API_INDEX_UART_ReceiveData])

#define RomDriver_UART_GetAutoBaudCount                   \
    ((uint16_t (*) (UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet))ROM_APITABLE[ROM_API_INDEX_UART_GetAutoBaudCount])

#define RomDriver_UART_GetRxByteCount                     \
    ((uint16_t (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetRxByteCount])

#define RomDriver_UART_GetTxFifoCount                     \
    ((uint8_t (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetTxFifoCount])

#define RomDriver_UART_GetRxFifoCount                     \
    ((uint8_t (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetRxFifoCount])

#define RomDriver_UART_GetIntStatus                       \
    ((BL_Sts_Type (*) (UART_ID_Type uartId, UART_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_UART_GetIntStatus])

#define RomDriver_UART_GetTxBusBusyStatus                 \
    ((BL_Sts_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetTxBusBusyStatus])

#define RomDriver_UART_GetRxBusBusyStatus                 \
    ((BL_Sts_Type (*) (UART_ID_Type uartId))ROM_APITABLE[ROM_API_INDEX_UART_GetRxBusBusyStatus])

#define RomDriver_UART_GetOverflowStatus                  \
    ((BL_Sts_Type (*) (UART_ID_Type uartId, UART_Overflow_Type overflow))ROM_APITABLE[ROM_API_INDEX_UART_GetOverflowStatus])

#define RomDriver_XIP_SFlash_Opt_Enter                    \
    ((void (*) (uint8_t *aesEnable))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Opt_Enter])

#define RomDriver_XIP_SFlash_Opt_Exit                     \
    ((void (*) (uint8_t aesEnable))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Opt_Exit])

#define RomDriver_XIP_SFlash_State_Save                   \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint32_t *offset))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_State_Save])

#define RomDriver_XIP_SFlash_State_Restore                \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint32_t offset))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_State_Restore])

#define RomDriver_XIP_SFlash_Erase_Need_Lock              \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint32_t startaddr, uint32_t endaddr))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Erase_Need_Lock])

#define RomDriver_XIP_SFlash_Write_Need_Lock              \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Write_Need_Lock])

#define RomDriver_XIP_SFlash_Read_Need_Lock               \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Read_Need_Lock])

#define RomDriver_XIP_SFlash_GetJedecId_Need_Lock         \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_GetJedecId_Need_Lock])

#define RomDriver_XIP_SFlash_GetDeviceId_Need_Lock        \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_GetDeviceId_Need_Lock])

#define RomDriver_XIP_SFlash_GetUniqueId_Need_Lock        \
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg, uint8_t *data, uint8_t idLen))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_GetUniqueId_Need_Lock])

#define RomDriver_XIP_SFlash_Read_Via_Cache_Need_Lock     \
    ((BL_Err_Type (*) (uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Read_Via_Cache_Need_Lock])

#define RomDriver_XIP_SFlash_Clear_Status_Register_Need_Lock\
    ((BL_Err_Type (*) (spi_flash_cfg_type *pFlashCfg))ROM_APITABLE[ROM_API_INDEX_XIP_SFlash_Clear_Status_Register_Need_Lock])

#define RomDriver_IR_TxInit                               \
    ((BL_Err_Type (*) (IR_TxCfg_Type *irTxCfg))ROM_APITABLE[ROM_API_INDEX_IR_TxInit])

#define RomDriver_IR_TxPulseWidthConfig                   \
    ((BL_Err_Type (*) (IR_TxPulseWidthCfg_Type *irTxPulseWidthCfg))ROM_APITABLE[ROM_API_INDEX_IR_TxPulseWidthConfig])

#define RomDriver_IR_FifoConfig                           \
    ((BL_Err_Type (*) (IR_FifoCfg_Type *fifoCfg))ROM_APITABLE[ROM_API_INDEX_IR_FifoConfig])

#define RomDriver_IR_DeInit                               \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_DeInit])

#define RomDriver_IR_Enable                               \
    ((BL_Err_Type (*) (IR_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_IR_Enable])

#define RomDriver_IR_Disable                              \
    ((BL_Err_Type (*) (IR_Direction_Type direct))ROM_APITABLE[ROM_API_INDEX_IR_Disable])

#define RomDriver_IR_TxSWM                                \
    ((BL_Err_Type (*) (BL_Fun_Type txSWM))ROM_APITABLE[ROM_API_INDEX_IR_TxSWM])

#define RomDriver_IR_TxFifoClear                          \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_TxFifoClear])

#define RomDriver_IR_SendData                             \
    ((BL_Err_Type (*) (uint32_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SendData])

#define RomDriver_IR_SWMSendData                          \
    ((BL_Err_Type (*) (uint16_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SWMSendData])

#define RomDriver_IR_SendCommand                          \
    ((BL_Err_Type (*) (uint32_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SendCommand])

#define RomDriver_IR_SWMSendCommand                       \
    ((BL_Err_Type (*) (uint16_t *data, uint8_t length))ROM_APITABLE[ROM_API_INDEX_IR_SWMSendCommand])

#define RomDriver_IR_SendNEC                              \
    ((BL_Err_Type (*) (uint8_t address, uint8_t command))ROM_APITABLE[ROM_API_INDEX_IR_SendNEC])

#define RomDriver_IR_IntMask                              \
    ((BL_Err_Type (*) (IR_INT_Type intType, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_IR_IntMask])

#define RomDriver_IR_ClrIntStatus                         \
    ((BL_Err_Type (*) (IR_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_IR_ClrIntStatus])

#define RomDriver_IR_GetIntStatus                         \
    ((BL_Sts_Type (*) (IR_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_IR_GetIntStatus])

#define RomDriver_IR_GetFifoStatus                        \
    ((BL_Sts_Type (*) (IR_FifoStatus_Type fifoSts))ROM_APITABLE[ROM_API_INDEX_IR_GetFifoStatus])

#define RomDriver_IR_GetTxFifoCount                       \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_IR_GetTxFifoCount])

#define RomDriver_IR_LEDInit                              \
    ((BL_Err_Type (*) (uint8_t clk, uint8_t div, uint8_t unit, uint8_t code0H, uint8_t code0L, uint8_t code1H, uint8_t code1L))ROM_APITABLE[ROM_API_INDEX_IR_LEDInit])

#define RomDriver_IR_LEDSend                              \
    ((BL_Err_Type (*) (uint32_t data))ROM_APITABLE[ROM_API_INDEX_IR_LEDSend])

#define RomDriver_KYS_Init                                \
    ((BL_Err_Type (*) (KYS_CFG_Type *kysCfg))ROM_APITABLE[ROM_API_INDEX_KYS_Init])

#define RomDriver_KYS_Enable                              \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_Enable])

#define RomDriver_KYS_Disable                             \
    ((BL_Err_Type (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_Disable])

#define RomDriver_KYS_IntMask                             \
    ((BL_Err_Type (*) (KYS_INT_Type intType, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_KYS_IntMask])

#define RomDriver_KYS_IntClear                            \
    ((BL_Err_Type (*) (KYS_INT_CLR_Type intType))ROM_APITABLE[ROM_API_INDEX_KYS_IntClear])

#define RomDriver_KYS_GetIntStatus                        \
    ((BL_Sts_Type (*) (KYS_INT_Type intType))ROM_APITABLE[ROM_API_INDEX_KYS_GetIntStatus])

#define RomDriver_KYS_Get_FIFO_Idx                        \
    ((void (*) (uint8_t *fifo_head, uint8_t *fifo_tail))ROM_APITABLE[ROM_API_INDEX_KYS_Get_FIFO_Idx])

#define RomDriver_KYS_ReadKeyfifo                         \
    ((uint8_t (*) (void))ROM_APITABLE[ROM_API_INDEX_KYS_ReadKeyfifo])

#define RomDriver_Psram_Init                              \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, struct sf_ctrl_cmds_cfg *cmdsCfg, struct sf_ctrl_psram_cfg *sfCtrlPsramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_Init])

#define RomDriver_Psram_ReadReg                           \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue))ROM_APITABLE[ROM_API_INDEX_Psram_ReadReg])

#define RomDriver_Psram_WriteReg                          \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue))ROM_APITABLE[ROM_API_INDEX_Psram_WriteReg])

#define RomDriver_Psram_SetDriveStrength                  \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_SetDriveStrength])

#define RomDriver_Psram_SetBurstWrap                      \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_SetBurstWrap])

#define RomDriver_Psram_ReadId                            \
    ((void (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t *data))ROM_APITABLE[ROM_API_INDEX_Psram_ReadId])

#define RomDriver_Psram_EnterQuadMode                     \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_EnterQuadMode])

#define RomDriver_Psram_ExitQuadMode                      \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg))ROM_APITABLE[ROM_API_INDEX_Psram_ExitQuadMode])

#define RomDriver_Psram_ToggleBurstLength                 \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode))ROM_APITABLE[ROM_API_INDEX_Psram_ToggleBurstLength])

#define RomDriver_Psram_SoftwareReset                     \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode))ROM_APITABLE[ROM_API_INDEX_Psram_SoftwareReset])

#define RomDriver_Psram_Set_IDbus_Cfg                     \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t ioMode, uint32_t addr, uint32_t len))ROM_APITABLE[ROM_API_INDEX_Psram_Set_IDbus_Cfg])

#define RomDriver_Psram_Cache_Write_Set                   \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t ioMode, BL_Fun_Type wtEn, BL_Fun_Type wbEn, BL_Fun_Type waEn))ROM_APITABLE[ROM_API_INDEX_Psram_Cache_Write_Set])

#define RomDriver_Psram_Write                             \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t ioMode, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_Psram_Write])

#define RomDriver_Psram_Read                              \
    ((BL_Err_Type (*) (SPI_Psram_Cfg_Type *psramCfg, uint8_t ioMode, uint32_t addr, uint8_t *data, uint32_t len))ROM_APITABLE[ROM_API_INDEX_Psram_Read])

#define RomDriver_TIMER_GetCompValue                      \
    ((uint32_t (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo))ROM_APITABLE[ROM_API_INDEX_TIMER_GetCompValue])

#define RomDriver_TIMER_SetCompValue                      \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val))ROM_APITABLE[ROM_API_INDEX_TIMER_SetCompValue])

#define RomDriver_TIMER_CompValueEffectImmediately        \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, BL_Fun_Type enable))ROM_APITABLE[ROM_API_INDEX_TIMER_CompValueEffectImmediately])

#define RomDriver_TIMER_GetCounterValue                   \
    ((uint32_t (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_GetCounterValue])

#define RomDriver_TIMER_ResetCounterValue                 \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_ResetCounterValue])

#define RomDriver_TIMER_GetMatchStatus                    \
    ((BL_Sts_Type (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo))ROM_APITABLE[ROM_API_INDEX_TIMER_GetMatchStatus])

#define RomDriver_TIMER_GetPreloadValue                   \
    ((uint32_t (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_GetPreloadValue])

#define RomDriver_TIMER_SetPreloadValue                   \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint32_t val))ROM_APITABLE[ROM_API_INDEX_TIMER_SetPreloadValue])

#define RomDriver_TIMER_SetPreloadTrigSrc                 \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc))ROM_APITABLE[ROM_API_INDEX_TIMER_SetPreloadTrigSrc])

#define RomDriver_TIMER_SetCountMode                      \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode))ROM_APITABLE[ROM_API_INDEX_TIMER_SetCountMode])

#define RomDriver_TIMER_ClearIntStatus                    \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo))ROM_APITABLE[ROM_API_INDEX_TIMER_ClearIntStatus])

#define RomDriver_TIMER_Init                              \
    ((BL_Err_Type (*) (TIMER_ID_Type timerId, TIMER_CFG_Type *timerCfg))ROM_APITABLE[ROM_API_INDEX_TIMER_Init])

#define RomDriver_TIMER_DeInit                            \
    ((BL_Err_Type (*) (TIMER_ID_Type timerId))ROM_APITABLE[ROM_API_INDEX_TIMER_DeInit])

#define RomDriver_TIMER_Enable                            \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_Enable])

#define RomDriver_TIMER_Disable                           \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_Disable])

#define RomDriver_TIMER_IntMask                           \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_INT_Type intType, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_TIMER_IntMask])

#define RomDriver_TIMER_GPIOSetPolarity                   \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_GPIO_Polarity_Type polarity))ROM_APITABLE[ROM_API_INDEX_TIMER_GPIOSetPolarity])

#define RomDriver_TIMER_CH0_SetMeasurePulseWidth          \
    ((void (*) (TIMER_ID_Type timerId, BL_Fun_Type enable))ROM_APITABLE[ROM_API_INDEX_TIMER_CH0_SetMeasurePulseWidth])

#define RomDriver_TIMER_CH0_GetMeasurePulseWidth          \
    ((uint32_t (*) (TIMER_ID_Type timerId))ROM_APITABLE[ROM_API_INDEX_TIMER_CH0_GetMeasurePulseWidth])

#define RomDriver_TIMER_ForceClockDivision                \
    ((void (*) (TIMER_ID_Type timerId, TIMER_Chan_Type timerCh))ROM_APITABLE[ROM_API_INDEX_TIMER_ForceClockDivision])

#define RomDriver_WDT_Set_Clock                           \
    ((void (*) (WDT_ID_Type wdtId, TIMER_ClkSrc_Type clkSrc, uint8_t div))ROM_APITABLE[ROM_API_INDEX_WDT_Set_Clock])

#define RomDriver_WDT_GetMatchValue                       \
    ((uint16_t (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_GetMatchValue])

#define RomDriver_WDT_SetCompValue                        \
    ((void (*) (WDT_ID_Type wdtId, uint16_t val))ROM_APITABLE[ROM_API_INDEX_WDT_SetCompValue])

#define RomDriver_WDT_CompValueEffectImmediately          \
    ((void (*) (WDT_ID_Type wdtId, BL_Fun_Type enable))ROM_APITABLE[ROM_API_INDEX_WDT_CompValueEffectImmediately])

#define RomDriver_WDT_GetCounterValue                     \
    ((uint16_t (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_GetCounterValue])

#define RomDriver_WDT_ResetCounterValue                   \
    ((void (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_ResetCounterValue])

#define RomDriver_WDT_GetResetStatus                      \
    ((BL_Sts_Type (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_GetResetStatus])

#define RomDriver_WDT_ClearResetStatus                    \
    ((void (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_ClearResetStatus])

#define RomDriver_WDT_Enable                              \
    ((void (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_Enable])

#define RomDriver_WDT_Disable                             \
    ((void (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_Disable])

#define RomDriver_WDT_ForceClockDivision                  \
    ((void (*) (WDT_ID_Type wdtId))ROM_APITABLE[ROM_API_INDEX_WDT_ForceClockDivision])

#define RomDriver_WDT_IntMask                             \
    ((void (*) (WDT_ID_Type wdtId, WDT_INT_Type intType, BL_Mask_Type intMask))ROM_APITABLE[ROM_API_INDEX_WDT_IntMask])

#define RomDriver_WDT_GPIOSetPolarity                     \
    ((void (*) (WDT_ID_Type wdtId, TIMER_GPIO_Polarity_Type polarity))ROM_APITABLE[ROM_API_INDEX_WDT_GPIOSetPolarity])

#define RomDriver_arch_memcpy                             \
    ((void * (*) (void *dst, const void *src, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcpy])

#define RomDriver_arch_memcpy4                            \
    ((uint32_t * (*) (uint32_t *dst, const uint32_t *src, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcpy4])

#define RomDriver_arch_memcpy_fast                        \
    ((void * (*) (void *pdst, const void *psrc, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcpy_fast])

#define RomDriver_arch_memset                             \
    ((void * (*) (void *s, uint8_t c, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memset])

#define RomDriver_arch_memset4                            \
    ((uint32_t * (*) (uint32_t *dst, const uint32_t val, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memset4])

#define RomDriver_arch_memcmp                             \
    ((int (*) (const void *s1, const void *s2, uint32_t n))ROM_APITABLE[ROM_API_INDEX_arch_memcmp])

#define RomDriver_memcopy_to_fifo                         \
    ((void (*) (void *fifo_addr, uint8_t *data, uint32_t length))ROM_APITABLE[ROM_API_INDEX_memcopy_to_fifo])

#define RomDriver_fifocopy_to_mem                         \
    ((void (*) (void *fifo_addr, uint8_t *data, uint32_t length))ROM_APITABLE[ROM_API_INDEX_fifocopy_to_mem])

#define RomDriver_BFLB_Soft_CRC32_Ex                      \
    ((uint32_t (*) (uint32_t initial, void *dataIn, uint32_t len))ROM_APITABLE[ROM_API_INDEX_BFLB_Soft_CRC32_Ex])

#define RomDriver_BFLB_Soft_CRC32                         \
    ((uint32_t (*) (void *dataIn, uint32_t len))ROM_APITABLE[ROM_API_INDEX_BFLB_Soft_CRC32])

/*@} end of group ROMDRIVER_Public_Macros */

/** @defgroup  ROMDRIVER_Public_Functions
 *  @{
 */

/*@} end of group ROMDRIVER_Public_Functions */

/*@} end of group ROMDRIVER */

/*@} end of group BL702L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_ROMDRIVER_H__ */
