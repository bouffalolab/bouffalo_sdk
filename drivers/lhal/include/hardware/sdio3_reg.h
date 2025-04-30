/**
  ******************************************************************************
  * @file    sdio3_reg.h
  * @version V1.0
  * @date    2022-10-26
  * @brief   This file is the description of.IP register
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
#ifndef __SDIO3_REG_H__
#define __SDIO3_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define SDIO3_CONTROL_REGISTER_OFFSET                  (0x00) /* Control_Register */
#define SDIO3_COMMAND_REGISTER_OFFSET                  (0x04) /* Command_Register */
#define SDIO3_ARGUMENT_REGISTER_OFFSET                 (0x08) /* Argument_Register */
#define SDIO3_BLOCK_COUNT_REGISTER_OFFSET              (0x0C) /* Block_Count_Register */
#define SDIO3_DMA1_ADDRESS_REGISTER_OFFSET             (0x10) /* DMA1_Address_Register */
#define SDIO3_DMA1_CONTROL_REGISTER_OFFSET             (0x14) /* DMA1_Control_Register */
#define SDIO3_DMA2_ADDRESS_REGISTER_OFFSET             (0x18) /* DMA2_Address_Register */
#define SDIO3_DMA2_CONTROL_REGISTER_OFFSET             (0x1C) /* DMA2_Control_Register */
#define SDIO3_ERASE_WRITE_BLOCK_START_REGISTER_OFFSET  (0x20) /* Erase_Write_Block_Start_Register */
#define SDIO3_ERASE_WRITE_BLOCK_END_REGISTER_OFFSET    (0x24) /* Erase_Write_block_End_Register */
#define SDIO3_PASSWORD_LENGTH_REGISTER_OFFSET          (0x28) /* Password_Length_Register */
#define SDIO3_SECURE_BLOCK_COUNT_REGISTER_OFFSET       (0x2C) /* Secure_Block_Count_Register */
#define SDIO3_CUSTOM_ADDRESS_REGISTER_OFFSET           (0x30) /* Custom_Address_Register */
#define SDIO3_CUSTOM_DATA_REGISTER_OFFSET              (0x34) /* Custom_Data_Register */
#define SDIO3_INTERRUPT_STATUS_REGISTER_OFFSET         (0x3C) /* Interrupt_Status_Register */
#define SDIO3_INTERRUPT_STATUS_ENABLE_REGISTER_OFFSET  (0x40) /* Interrupt_Status_Enable_Register */
#define SDIO3_INTERRUPT_SIGNAL_ENABLE_REGISTER_OFFSET  (0x44) /* Interrupt_Signal_Enable_Register */
#define SDIO3_CARD_ADDRESS_REGISTER_OFFSET             (0x48) /* Card_Address_Register */
#define SDIO3_CARD_DATA_REGISTER_OFFSET                (0x4C) /* Card_Data_Register */
#define SDIO3_IOREADY_REGISTER_OFFSET                  (0x50) /* IOREADY_Register */
#define SDIO3_FUNCTION1_CONTROL_REGISTER_OFFSET        (0x54) /* Function1_Control_Register */
#define SDIO3_FUNCTION2_CONTROL_REGISTER_OFFSET        (0x58) /* Function2_Control_Register */
#define SDIO3_SDIO_CCCR_CONTROL_REGISTER_OFFSET        (0x5C) /* SDIO_CCCR_Control_Register */
#define SDIO3_SDIO_FBR1_CONTROL_REGISTERS_OFFSET       (0x60) /* SDIO_FBR1_Control_Registers */
#define SDIO3_SDIO_FBR2_CONTROL_REGISTERS_OFFSET       (0x64) /* SDIO_FBR2_Control_Registers */
#define SDIO3_CARD_SIZE_REGISTER_OFFSET                (0x80) /* Card_Size_Register */
#define SDIO3_CARD_OCR_REGISTER_OFFSET                 (0x84) /* Card_OCR_Register */
#define SDIO3_CONTROL2_REGISTER_OFFSET                 (0x88) /* Control2_Register */
#define SDIO3_FUNCTION3_CONTROL_REGISTER_OFFSET        (0x90) /* Function3_Control_Register */
#define SDIO3_FUNCTION4_CONTROL_REGISTER_OFFSET        (0x94) /* Function4_Control_Register */
#define SDIO3_FUNCTION5_CONTROL_REGISTER_OFFSET        (0x98) /* Function5_Control_Register */
#define SDIO3_INTERRUPT_STATUS2_REGISTER_OFFSET        (0x9C) /* Interrupt_Status2_Register */
#define SDIO3_INTERRUPT_STATUS_ENABLE2_REGISTER_OFFSET (0xA0) /* Interrupt_Status_Enable2_Register */
#define SDIO3_INTERRUPT_SIGNAL_ENABLE2_REGISTER_OFFSET (0xA4) /* Interrupt_Signal_Enable2_Register */
#define SDIO3_PASSWORD_127_96_OFFSET                   (0xA8) /* Password_127_96 */
#define SDIO3_PASSWORD_95_64_OFFSET                    (0xAC) /* Password_95_64 */
#define SDIO3_PASSWORD_63_32_OFFSET                    (0xB0) /* Password_63_32 */
#define SDIO3_PASSOWRD_31_0_OFFSET                     (0xB4) /* Passowrd_31_0 */
#define SDIO3_ADMA_ERROR_STATUS_REGISTER_OFFSET        (0xB8) /* ADMA_Error_Status_Register */
#define SDIO3_RCA_REGISTER_OFFSET                      (0xBC) /* RCA_Register */
#define SDIO3_DBG0_DEBUG_REGISTER_OFFSET               (0xC0) /* dbg0_Debug_Register */
#define SDIO3_DBG1_DEBUG_REGISTER_OFFSET               (0xC4) /* dbg1_Debug_Register */
#define SDIO3_DBG2_DEBUG_REGISTER_OFFSET               (0xC8) /* dbg2_Debug_Register */
#define SDIO3_DBG3_DEBUG_REGISTER_OFFSET               (0xCC) /* dbg3_Debug_Register */
#define SDIO3_DBG4_DEBUG_REGISTER_OFFSET               (0xD0) /* dbg4_Debug_Register */
#define SDIO3_DBG5_DEBUG_REGISTER_OFFSET               (0xD4) /* dbg5_Debug_Register */
#define SDIO3_DBG6_DEBUG_REGISTER_OFFSET               (0xD8) /* dbg6_Debug_Register */
#define SDIO3_AHB_MASTER_BURSTSIZE_REGISTER_OFFSET     (0xDC) /* AHB_Master_Burstsize_register */
#define SDIO3_ARGUMENT2_REGISTER_OFFSET                (0xE0) /* Argument2_Register */

/* Register Bitfield definitions *****************************************************/

/* 0x00 : Control_Register */
#define SDIO3_PROGRAM_DONE                             (1 << 0U)
#define SDIO3_CARD_INIT_DONE                           (1 << 2U)
#define SDIO3_ADDRESS_OUT_OF_RANGE                     (1 << 3U)
#define SDIO3_ADDRESS_MISALIGN                         (1 << 4U)
#define SDIO3_RPMB_DIS_EN                              (1 << 5U)
#define SDIO3_ERASE_PARAM                              (1 << 6U)
#define SDIO3_CARD_ECC_FAILED                          (1 << 7U)
#define SDIO3_CC_ERROR                                 (1 << 8U)
#define SDIO3_ERROR                                    (1 << 9U)
#define SDIO3_MMC_IRQ_TRIGGER                          (1 << 10U)
#define SDIO3_CMD_DATA_OUTPUT_EDGE                     (1 << 11U)
#define SDIO3_CMD32_CMD33_ENABLE                       (1 << 12U)
#define SDIO3_BOOT_SEQUENCE_SUPPORT                    (1 << 13U)
#define SDIO3_SWITCH_ERROR                             (1 << 14U)
#define SDIO3_BOOT_ACK                                 (1 << 15U)
#define SDIO3_WP_VIOLATION                             (1 << 16U)
#define SDIO3_WP_ERASE_SKIP                            (1 << 17U)
#define SDIO3_CID_CSD_OVERWRITE                        (1 << 18U)
#define SDIO3_AKE_SEQ_ERROR                            (1 << 19U)
#define SDIO3_CARD_ECC_DISABLED                        (1 << 20U)
#define SDIO3_STREAM_THRESHOLD_SIZE_SHIFT              (21U)
#define SDIO3_STREAM_THRESHOLD_SIZE_MASK               (0x7 << SDIO3_STREAM_THRESHOLD_SIZE_SHIFT)
#define SDIO3_PERMANENT_WRITE_PROTECT                  (1 << 24U)
#define SDIO3_TEMPORARY_WRITE_PROTECT                  (1 << 25U)
#define SDIO3_WP_COMMANDS_ENABLED                      (1 << 26U)
#define SDIO3_ALLOW_AKE                                (1 << 27U)
#define SDIO3_SECURED_MODE                             (1 << 28U)
#define SDIO3_AKE_SEQ_OK                               (1 << 29U)
#define SDIO3_ASSD_DIS_EN                              (1 << 30U)
#define SDIO3_BOOT_DATA_RDY                            (1 << 31U)

/* 0x04 : Command_Register */
#define SDIO3_APPLICATION                              (1 << 0U)
#define SDIO3_BLOCK_SIZE_SHIFT                         (1U)
#define SDIO3_BLOCK_SIZE_MASK                          (0xfff << SDIO3_BLOCK_SIZE_SHIFT)
#define SDIO3_COMMAND_INDEX_SHIFT                      (13U)
#define SDIO3_COMMAND_INDEX_MASK                       (0x3f << SDIO3_COMMAND_INDEX_SHIFT)
#define SDIO3_CURRENT_BUS_WIDTH_SHIFT                  (19U)
#define SDIO3_CURRENT_BUS_WIDTH_MASK                   (0x3 << SDIO3_CURRENT_BUS_WIDTH_SHIFT)
#define SDIO3_CURRENT_SPEED_SHIFT                      (21U)
#define SDIO3_CURRENT_SPEED_MASK                       (0x7 << SDIO3_CURRENT_SPEED_SHIFT)
#define SDIO3_CARD_STATE_SHIFT                         (24U)
#define SDIO3_CARD_STATE_MASK                          (0xf << SDIO3_CARD_STATE_SHIFT)
#define SDIO3_ERASE_SEQUENCE                           (1 << 28U)

/* 0x08 : Argument_Register */
#define SDIO3_ARGUMENT_SHIFT                           (0U)
#define SDIO3_ARGUMENT_MASK                            (0xffffffffL << SDIO3_ARGUMENT_SHIFT)

/* 0x0C : Block_Count_Register */
#define SDIO3_BLOCK_COUNT_SHIFT                        (0U)
#define SDIO3_BLOCK_COUNT_MASK                         (0xffffffffL << SDIO3_BLOCK_COUNT_SHIFT)

/* 0x10 : DMA1_Address_Register */
#define SDIO3_DMA1_ADDRESS_REGISTER_SHIFT              (0U)
#define SDIO3_DMA1_ADDRESS_REGISTER_MASK               (0xffffffffL << SDIO3_DMA1_ADDRESS_REGISTER_SHIFT)

/* 0x14 : DMA1_Control_Register */
#define SDIO3_DMA1_ADDRESS_VALID                       (1 << 0U)
#define SDIO3_DMA1_BUFFER_SIZE_SHIFT                   (1U)
#define SDIO3_DMA1_BUFFER_SIZE_MASK                    (0x7 << SDIO3_DMA1_BUFFER_SIZE_SHIFT)

/* 0x18 : DMA2_Address_Register */
#define SDIO3_DMA2_ADDRESS_REGISTER_SHIFT              (0U)
#define SDIO3_DMA2_ADDRESS_REGISTER_MASK               (0xffffffffL << SDIO3_DMA2_ADDRESS_REGISTER_SHIFT)

/* 0x1C : DMA2_Control_Register */
#define SDIO3_DMA2_ADDRESS_VALID                       (1 << 0U)
#define SDIO3_DMA2_BUFFER_SIZE_SHIFT                   (1U)
#define SDIO3_DMA2_BUFFER_SIZE_MASK                    (0x7 << SDIO3_DMA2_BUFFER_SIZE_SHIFT)

/* 0x20 : Erase_Write_Block_Start_Register */
#define SDIO3_ERASE_WRITE_BLOCK_START_SHIFT            (0U)
#define SDIO3_ERASE_WRITE_BLOCK_START_MASK             (0xffffffffL << SDIO3_ERASE_WRITE_BLOCK_START_SHIFT)

/* 0x24 : Erase_Write_block_End_Register */
#define SDIO3_ERASE_WRITE_BLOCK_END_SHIFT              (0U)
#define SDIO3_ERASE_WRITE_BLOCK_END_MASK               (0xffffffffL << SDIO3_ERASE_WRITE_BLOCK_END_SHIFT)

/* 0x28 : Password_Length_Register */
#define SDIO3_PWDS_LEN_SHIFT                           (0U)
#define SDIO3_PWDS_LEN_MASK                            (0xff << SDIO3_PWDS_LEN_SHIFT)
#define SDIO3_DISABLE_LOCK_UNLOCK                      (1 << 8U)

/* 0x2C : Secure_Block_Count_Register */
#define SDIO3_SECURE_BLOCK_COUNT_SHIFT                 (0U)
#define SDIO3_SECURE_BLOCK_COUNT_MASK                  (0xff << SDIO3_SECURE_BLOCK_COUNT_SHIFT)

/* 0x30 : Custom_Address_Register */
#define SDIO3_CUSTOM_REGISTER_STARTING_ADDRESS_SHIFT   (0U)
#define SDIO3_CUSTOM_REGISTER_STARTING_ADDRESS_MASK    (0x1ff << SDIO3_STARTING_ADDRESS_SHIFT)

/* 0x34 : Custom_Data_Register */
#define SDIO3_CUSTOM_DATA_SHIFT                        (0U)
#define SDIO3_CUSTOM_DATA_MASK                         (0xff << SDIO3_STARTING_ADDRESS_SHIFT)

/* 0x3C : Interrupt_Status_Register */
/* 0x40 : Interrupt_Status_Enable_Register */
/* 0x44 : Interrupt_Signal_Enable_Register */
#define SDIO3_TRANSFER_COMPLETE_INTERRUPT              (1 << 0U)
#define SDIO3_DMA1_INTERRUPT                           (1 << 1U)
#define SDIO3_SLEEP_AWAKE_INTERRUPT                    (1 << 2U)
#define SDIO3_WRITE_START_INTERRUPT                    (1 << 3U)
#define SDIO3_READ_START_INTERRUPT                     (1 << 4U)
#define SDIO3_PASSWORD_SET_INTERRUPT                   (1 << 5U)
#define SDIO3_PASSWORD_RESET_INTERRUPT                 (1 << 6U)
#define SDIO3_LOCK_CARD_INTERRUPT                      (1 << 7U)
#define SDIO3_UNLOCK_CARD_INTERRUPT                    (1 << 8U)
#define SDIO3_FORCE_ERASE_INTERRUPT                    (1 << 9U)
#define SDIO3_ERASE_INTERRUPT                          (1 << 10U)
#define SDIO3_CMD11_INTERRUPT                          (1 << 11U)
#define SDIO3_CMD0_CMD52_SOFT_RESET                    (1 << 12U)
#define SDIO3_CMD6_CHECK_DONE_INTERRUPT                (1 << 13U)
#define SDIO3_CMD6_SWITCH_DONE_INTERRUPT               (1 << 14U)
#define SDIO3_PROGRAM_CSD_INTERRUPT                    (1 << 15U)
#define SDIO3_ACMD23_INTERRUPT                         (1 << 16U)
#define SDIO3_CMD20_INTERRUPT                          (1 << 17U)
#define SDIO3_CMD4_INTERRUPT                           (1 << 19U)
#define SDIO3_BOOT_START                               (1 << 20U)
#define SDIO3_FUNCTION1_RESET                          (1 << 21U)
#define SDIO3_FUNCTION2_RESET                          (1 << 22U)
#define SDIO3_CMD11_CLK_STOP                           (1 << 23U)
#define SDIO3_CMD11_CLK_START                          (1 << 24U)
#define SDIO3_PROGRAM_START                            (1 << 25U)
#define SDIO3_CMD40_INTERRUPT                          (1 << 26U)
#define SDIO3_CMD_R1B_INTERRUPT                        (1 << 27U)
#define SDIO3_FUNCTIONX_CRC_END_ERROR_INTERRUPT        (1 << 28U)
#define SDIO3_FUNCTIONX_ABORT_INTERRUPT                (1 << 29U)
#define SDIO3_LRST_INTERRUPT                           (1 << 30U)
#define SDIO3_BOOT_COMPLETE_INTERRUPT                  (1 << 31U)

/* 0x48 : Card_Address_Register */
#define SDIO3_STARTING_ADDRESS_SHIFT                   (0U)
#define SDIO3_STARTING_ADDRESS_MASK                    (0x3ff << SDIO3_STARTING_ADDRESS_SHIFT)

/* 0x4C : Card_Data_Register */
#define SDIO3_CARD_DATA_REGISTER_SHIFT                 (0U)
#define SDIO3_CARD_DATA_REGISTER_MASK                  (0xffffffffL << SDIO3_CARD_DATA_REGISTER_SHIFT)

/* 0x50 : IOREADY_Register */
#define SDIO3_FUNCTION1_READY                          (1 << 1U)
#define SDIO3_FUNCTION2_READY                          (1 << 2U)
#define SDIO3_FUNCTION3_READY                          (1 << 3U)
#define SDIO3_FUNCTION4_READY                          (1 << 4U)
#define SDIO3_FUNCTION5_READY                          (1 << 5U)

/* 0x54 : Function1_Control_Register */
#define SDIO3_FUNCTION1_READ_COUNT_SHIFT               (0U)
#define SDIO3_FUNCTION1_READ_COUNT_MASK                (0xffff << SDIO3_FUNCTION1_READ_COUNT_SHIFT)

/* 0x58 : Function2_Control_Register */
#define SDIO3_FUNCTION2_READ_COUNT_SHIFT               (16U)
#define SDIO3_FUNCTION2_READ_COUNT_MASK                (0xffff << SDIO3_FUNCTION2_READ_COUNT_SHIFT)

/* 0x5C : SDIO_CCCR_Control_Register */
#define SDIO3_CCCR_REVISION_SHIFT                      (0U)
#define SDIO3_CCCR_REVISION_MASK                       (0xf << SDIO3_CCCR_REVISION_SHIFT)
#define SDIO3_SDIO_SPECIFICATION_REVISION_SHIFT        (4U)
#define SDIO3_SDIO_SPECIFICATION_REVISION_MASK         (0xf << SDIO3_SDIO_SPECIFICATION_REVISION_SHIFT)
#define SDIO3_SDX_SHIFT                                (8U)
#define SDIO3_SDX_MASK                                 (0xf << SDIO3_SDX_SHIFT)
#define SDIO3_S8B                                      (1 << 12U)
#define SDIO3_SCSI                                     (1 << 13U)
#define SDIO3_SDC                                      (1 << 14U)
#define SDIO3_SMB                                      (1 << 15U)
#define SDIO3_SRW                                      (1 << 16U)
#define SDIO3_SBS                                      (1 << 17U)
#define SDIO3_S4MI                                     (1 << 18U)
#define SDIO3_LSC                                      (1 << 19U)
#define SDIO3_LS4B                                     (1 << 20U)
#define SDIO3_SMPC                                     (1 << 21U)
#define SDIO3_SHS                                      (1 << 22U)
#define SDIO3_SDR50                                    (1 << 23U)
#define SDIO3_SDR104                                   (1 << 24U)
#define SDIO3_DDR50                                    (1 << 25U)
#define SDIO3_SDTA                                     (1 << 26U)
#define SDIO3_SDTC                                     (1 << 27U)
#define SDIO3_SDTD                                     (1 << 28U)
#define SDIO3_SAI                                      (1 << 29U)

/* 0x60 : SDIO_FBR1_Control_Registers */
#define SDIO3_FUNCTION1_STANDARD_INTERFACE_CODE_SHIFT  (0U)
#define SDIO3_FUNCTION1_STANDARD_INTERFACE_CODE_MASK   (0xf << SDIO3_FUNCTION1_STANDARD_INTERFACE_CODE_SHIFT)
#define SDIO3_FUNCTION1_INTERFACE_CODE_SHIFT           (4U)
#define SDIO3_FUNCTION1_INTERFACE_CODE_MASK            (0xff << SDIO3_FUNCTION1_INTERFACE_CODE_SHIFT)
#define SDIO3_FUNCTION1_SUPPORTS_CSA                   (1 << 15U)
#define SDIO3_FUNCTION1_SPS                            (1 << 16U)

/* 0x64 : SDIO_FBR1_Control_Registers */
#define SDIO3_FUNCTION2_STANDARD_INTERFACE_CODE_SHIFT  (0U)
#define SDIO3_FUNCTION2_STANDARD_INTERFACE_CODE_MASK   (0xf << SDIO3_FUNCTION2_STANDARD_INTERFACE_CODE_SHIFT)
#define SDIO3_FUNCTION2_INTERFACE_CODE_SHIFT           (4U)
#define SDIO3_FUNCTION2_INTERFACE_CODE_MASK            (0xff << SDIO3_FUNCTION2_INTERFACE_CODE_SHIFT)
#define SDIO3_FUNCTION2_SUPPORTS_CSA                   (1 << 15U)
#define SDIO3_FUNCTION2_SPS                            (1 << 16U)

/* 0x80 : Card_Size_Register */
#define SDIO3_CARD_SIZE_SHIFT                          (0U)
#define SDIO3_CARD_SIZE_MASK                           (0xffffffffL << SDIO3_CARD_SIZE_SHIFT)

/* 0x84 : Card_OCR_Register */
#define SDIO3_CARD_OCR_SHIFT                           (0U)
#define SDIO3_CARD_OCR_MASK                            (0xffffff << SDIO3_CARD_OCR_SHIFT)
#define SDIO3_SWITCHIN_TO_1P8V                         (1 << 24U)
#define SDIO3_ACCESS_MODE                              (1 << 29U)
#define SDIO3_ACCESS_MODE_CCS                          (1 << 30U)

/* 0x88 : Control2_Register */
#define SDIO3_CMD60_BUSY_EN_DIS                        (1 << 0U)
#define SDIO3_ADMA_ENABLE                              (1 << 2U)

/* 0x90 : Function3_Control_Register */
#define SDIO3_FUNCTION3_READ_COUNT_SHIFT               (0U)
#define SDIO3_FUNCTION3_READ_COUNT_MASK                (0xffff << SDIO3_FUNCTION3_READ_COUNT_SHIFT)

/* 0x94 : Function4_Control_Register */
#define SDIO3_FUNCTION4_READ_COUNT_SHIFT               (0U)
#define SDIO3_FUNCTION4_READ_COUNT_MASK                (0xffff << SDIO3_FUNCTION4_READ_COUNT_SHIFT)

/* 0x98 : Function5_Control_Register */
#define SDIO3_FUNCTION5_READ_COUNT_SHIFT               (0U)
#define SDIO3_FUNCTION5_READ_COUNT_MASK                (0xffff << SDIO3_FUNCTION5_READ_COUNT_SHIFT)

/* 0x9C : Interrupt_Status2_Register */
/* 0xA0 : Interrupt_Status_Enable2_Register */
/* 0xA4 : Interrupt_Signal_Enable2_Register */
#define SDIO3_FUNCTION3_RESET                          (1 << 0U)
#define SDIO3_FUNCTION4_RESET                          (1 << 1U)
#define SDIO3_FUNCTION5_RESET                          (1 << 2U)
#define SDIO3_PACKED_INTERRUPT                         (1 << 5U)
#define SDIO3_ADMA_ERROR_INTERRUPT                     (1 << 6U)

/* 0xA8 : Password_127_96 */
#define SDIO3_PASSWORD_127_96_SHIFT                    (0U)
#define SDIO3_PASSWORD_127_96_MASK                     (0xffffffffL << SDIO3_PASSWORD_127_96_SHIFT)

/* 0xAC : Password_95_64 */
#define SDIO3_PASSWORD_95_64_SHIFT                     (0U)
#define SDIO3_PASSWORD_95_64_MASK                      (0xffffffffL << SDIO3_PASSWORD_95_64_SHIFT)

/* 0xB0 : Password_63_32 */
#define SDIO3_PASSWORD_63_32_SHIFT                     (0U)
#define SDIO3_PASSWORD_63_32_MASK                      (0xffffffffL << SDIO3_PASSWORD_63_32_SHIFT)

/* 0xB4 : Passowrd_31_0 */
#define SDIO3_PASSWORD_31_0_SHIFT                      (0U)
#define SDIO3_PASSWORD_31_0_MASK                       (0xffffffffL << SDIO3_PASSWORD_31_0_SHIFT)

/* 0xB8 : ADMA_Error_Status_Register */
#define SDIO3_ADMA_ERROR_STATE_SHIFT                   (0U)
#define SDIO3_ADMA_ERROR_STATE_MASK                    (0x3 << SDIO3_ADMA_ERROR_STATE_SHIFT)
#define SDIO3_ADMA_LENGTH_MISMATCH_ERROR               (1 << 2U)

/* 0xBC : RCA_Register */
#define SDIO3_RCA_SHIFT                                (0U)
#define SDIO3_RCA_MASK                                 (0xffff << SDIO3_RCA_SHIFT)

/* 0xC0 : dbg0_Debug_Register */
#define SDIO3_DBG0_SHIFT                               (0U)
#define SDIO3_DBG0_MASK                                (0xffffffffL << SDIO3_DBG0_SHIFT)

/* 0xC4 : dbg1_Debug_Register */
#define SDIO3_DBG1_SHIFT                               (0U)
#define SDIO3_DBG1_MASK                                (0xffffffffL << SDIO3_DBG1_SHIFT)

/* 0xC8 : dbg2_Debug_Register */
#define SDIO3_DBG2_SHIFT                               (0U)
#define SDIO3_DBG2_MASK                                (0xffffffffL << SDIO3_DBG2_SHIFT)

/* 0xCC : dbg3_Debug_Register */
#define SDIO3_DBG3_SHIFT                               (0U)
#define SDIO3_DBG3_MASK                                (0xffffffffL << SDIO3_DBG3_SHIFT)

/* 0xD0 : dbg4_Debug_Register */
#define SDIO3_DBG4_SHIFT                               (0U)
#define SDIO3_DBG4_MASK                                (0x3fffffff << SDIO3_DBG4_SHIFT)

/* 0xD4 : dbg5_Debug_Register */
#define SDIO3_DBG5_SHIFT                               (0U)
#define SDIO3_DBG5_MASK                                (0x7ffffff << SDIO3_DBG5_SHIFT)

/* 0xD8 : dbg6_Debug_Register */
#define SDIO3_DBG6_SHIFT                               (0U)
#define SDIO3_DBG6_MASK                                (0x7ffff << SDIO3_DBG6_SHIFT)

/* 0xDC : AHB_Master_Burstsize_register */
#define SDIO3_AHBMAS_BURST_SIZE_ENABLE_SHIFT           (0U)
#define SDIO3_AHBMAS_BURST_SIZE_ENABLE_MASK            (0x7f << SDIO3_AHBMAS_BURST_SIZE_ENABLE_SHIFT)

/* 0xE0 : Argument2_Register */
#define SDIO3_ARGUMENT2_SHIFT                          (0U)
#define SDIO3_ARGUMENT2_MASK                           (0xffffffffL << SDIO3_ARGUMENT2_SHIFT)

#endif /* __SDIO3_REG_H__ */
