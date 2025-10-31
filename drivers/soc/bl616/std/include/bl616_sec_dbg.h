/**
  ******************************************************************************
  * @file    bl616_sec_dbg.h
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
#ifndef __BL616_SEC_DBG_H__
#define __BL616_SEC_DBG_H__

#include "sec_dbg_reg.h"
#include "bl616_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SEC_DBG
 *  @{
 */

/** @defgroup  SEC_DBG_Public_Types
 *  @{
 */

/**
 *  @brief SEC DBG debug mode type definition
 */
#define SEC_DBG_STATE_OPEN_MODE     (0) /*!< debug open mode */
#define SEC_DBG_STATE_PASSWORD_MODE (1) /*!< debug password mode */
#define SEC_DBG_STATE_CLOSE_MODE    (2) /*!< debug close mode */

/*@} end of group SEC_DBG_Public_Types */

/** @defgroup  SEC_DBG_Public_Constants
 *  @{
 */

#define IS_SEC_DBG_STATE_TYPE(type) (((type) == SEC_DBG_STATE_OPEN_MODE) ||     \
                                     ((type) == SEC_DBG_STATE_PASSWORD_MODE) || \
                                     ((type) == SEC_DBG_STATE_CLOSE_MODE))

/*@} end of group SEC_DBG_Public_Constants */

/** @defgroup  SEC_DBG_Public_Macros
 *  @{
 */

/*@} end of group SEC_DBG_Public_Macros */

/** @defgroup  SEC_DBG_Public_Functions
 *  @{
 */
/*----------*/
void Sec_Dbg_Read_Chip_ID(uint8_t id[8]);
uint32_t Sec_Dbg_Read_Dbg_State(void);
uint32_t Sec_Dbg_Read_Dbg_Mode(void);
uint32_t Sec_Dbg_Read_Dbg_Enable(void);
/*----------*/
void Sec_Dbg_Set_Dbg_Pwd(const uint32_t pwd[4]);
void Sec_Dbg_Set_Dbg_Trigger(void);
/*----------*/
uint32_t Sec_Dbg_Wait_Ready(void);

/*@} end of group SEC_DBG_Public_Functions */

/*@} end of group SEC_DBG */

/*@} end of group BL616_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616_SEC_DBG_H__ */
