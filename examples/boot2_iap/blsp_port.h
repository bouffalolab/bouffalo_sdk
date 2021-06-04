/**
  ******************************************************************************
  * @file    blsp_port.h
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
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
#ifndef __BLSP_PORT_H__
#define __BLSP_PORT_H__

#include "stdint.h"
#include "bl702_glb.h"
#include "bl702_aon.h"
#include "bl702_hbn.h"
#include "bl702_ef_ctrl.h"
#include "bl702_sflash.h"
#include "bl702_xip_sflash.h"
#include "bl702_xip_sflash_ext.h"
#include "bl702_sf_cfg.h"
#include "bl702_timer.h"
#include "bl702_sec_eng.h"
//#include "bflb_ecdsa.h"

/** @addtogroup  BL606_BLSP_Boot2
 *  @{
 */

/** @addtogroup  BLSP_PORT
 *  @{
 */

/** @defgroup  BLSP_PORT_Public_Types
 *  @{
 */

/*@} end of group BLSP_PORT_Public_Types */

/** @defgroup  BLSP_PORT_Public_Constants
 *  @{
 */
/*CPU1's MSP and PC from NP bootrom*/
//#define BFLB_BOOT2_CPU1_APP_PC_ADDR             (BL60X_SRAM_END-4)
//#define BFLB_BOOT2_CPU1_APP_MSP_ADDR            (BL60X_SRAM_END-8)
//#define BFLB_BOOT2_CPU1_DBG_INFO_ADDR           (BL60X_SRAM_END-12)
#define BFLB_BOOT2_CPU1_APP_PC_ADDR             (0)
#define BFLB_BOOT2_CPU1_APP_MSP_ADDR            (0)
#define BFLB_BOOT2_CPU1_DBG_INFO_ADDR           (0)
#define MFG_START_REQUEST_OFFSET                ((4+184)*1024)
#define BLSP_BOOT2_XIP_BASE                     BL702_FLASH_XIP_BASE
#define ARCH_Delay_MS                           BL702_Delay_MS

/*@} end of group BLSP_PORT_Public_Constants */

/** @defgroup  BLSP_PORT_Public_Macros
 *  @{
 */

/*@} end of group BLSP_PORT_Public_Macros */

/** @defgroup  BLSP_PORT_Public_Functions
 *  @{
 */
void blsp_boot2_init_timer(void);
void blsp_boot2_disable_other_cache(void);
void blsp_boot2_flush_xip_cache(void);
void blsp_boot2_show_timer(void);
void blsp_boot2_reset_sec_eng(void);
void blsp_boot2_init_sec_eng_pka(void);
uint32_t blsp_boot2_get_cpu_count(void);
uint8_t blsp_read_power_save_mode(void);
void blsp_boot2_pass_parameter(void *data,uint32_t len);
uint32_t blsp_boot2_get_xip_offset();
uint8_t* blsp_get_user_specified_fw(void);
void blsp_clr_user_specified_fw(void);
void blsp_boot2_releae_other_cpu(void);
int32_t blsp_is_msp_valid(uint32_t msp_val);
int32_t blsp_is_pc_valid(uint32_t pc_val);
void ATTR_TCM_SECTION blsp_sboot_finish(void);;
void blsp_fix_invalid_msp_pc(void);
uint8_t blsp_boot2_get_tx_gpio(void);
uint8_t blsp_boot2_get_uart_port(void);
uint8_t blsp_boot2_get_feature_flag(void);
uint8_t blsp_boot2_get_log_disable_flag(void);
uint8_t blsp_boot2_8m_support_flag(void);
uint8_t blsp_boot2_dump_critical_flag(void);
uint32_t blsp_boot2_get_baudrate(void);
void blsp_boot2_init_gpio_for_bootrom();

/*@} end of group BLSP_PORT_Public_Functions */

/*@} end of group BLSP_PORT */

/*@} end of group BL606_BLSP_Boot2 */

#endif /* __BLSP_PORT_H__ */
