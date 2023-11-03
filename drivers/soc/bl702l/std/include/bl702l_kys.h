/**
 * @file bl702l_kys.h
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

#ifndef __BL702L_KYS_H__
#define __BL702L_KYS_H__

#include "kys_reg.h"
#include "bl702l_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  KYS
 *  @{
 */

/** @defgroup  KYS_Public_Types
 *  @{
 */

/**
 *  @brief KYS keycode type definition
 */
// typedef enum {
//     KYS_KEYCODE_0, /*!< KYS keycode 0 */
//     KYS_KEYCODE_1, /*!< KYS keycode 1 */
//     KYS_KEYCODE_2, /*!< KYS keycode 2 */
//     KYS_KEYCODE_3, /*!< KYS keycode 3 */
//     KYS_KEYCODE_4, /*!< KYS keycode 4 */
//     KYS_KEYCODE_5, /*!< KYS keycode 5 */
// } KYS_Keycode_Type;

// /**
//  * @brief KYS Interrupt type definition
//  */
// typedef enum {
//     KYS_KEYCODE_INT,    /*!< keycode mode interrupt */
//     KYS_KEYFIFO_INT,    /*!< keycode fifo mode interrupt*/
//     KYS_GHOST_INT,      /*!<  */
//     KYS_INT_ALL,
// } KYS_INT_Type;

/**
 * @brief KYS Interrupt Status type definition
 *
 */
typedef enum {
    KYS_INT_KS_DONE,          /*!< keyscan done status */
    KYS_INT_KEYFIFO_FULL,     /*!< keycode FIFO full detection */
    KYS_INT_KEYFIFO_HALF,     /*!< keycode FIFO half detection */
    KYS_INT_KEYFIFO_QUARTER,  /*!< keycode FIFO quarter detection */
    KYS_INT_KEYFIFO_NONEMPTY, /*!< keycode FIFO nonempty detection */
    KYS_INT_GHOST_DET,        /*!< ghost key event detection status (keycode=0xF5 is ghost key) */
    KYS_INT_ALL,
} KYS_INT_Type;

typedef enum {
    KYS_INT_KS_DONE_CLR = 7,
    KYS_INT_KEYFIFO_CLR = 8,
    KYS_INT_GHOST_CLR = 12,
    KYS_INT_CLR_ALL,
} KYS_INT_CLR_Type;

/**
 *  @brief KYS configuration structure type definition
 */
typedef struct
{
    uint8_t col;             /*!< Col of keyboard,max:8 */
    uint8_t row;             /*!< Row of keyboard,max:8 */
    uint8_t idle_duration;   /*!< Idle duration between column scans */
    uint8_t fifo_mode;       /*!< Fifo mode for keycode (incremental) */
    BL_Fun_Type ghost_en;    /*!< Enable or disable ghost key event detection */
    BL_Fun_Type deglitch_en; /*!< Enable or disable deglitch function */
    uint8_t deglitch_cnt;    /*!< Deglitch count */
} KYS_CFG_Type;

/*@} end of group KYS_Public_Types */

/** @defgroup  KYS_Public_Constants
 *  @{
 */

/** @defgroup  KYS_INT_TYPE
 *  @{
 */
#define IS_KYS_INT_TYPE(type) (((type) == KYS_INT_KS_DONE) ||          \
                               ((type) == KYS_INT_KEYFIFO_FULL) ||     \
                               ((type) == KYS_INT_KEYFIFO_HALF) ||     \
                               ((type) == KYS_INT_KEYFIFO_QUARTER) ||  \
                               ((type) == KYS_INT_KEYFIFO_NONEMPTY) || \
                               ((type) == KYS_INT_GHOST_DET) ||        \
                               ((type) == KYS_INT_ALL))

/** @defgroup  KYS_INT_CLR_TYPE
 *  @{
 */
#define IS_KYS_INT_CLR_TYPE(type) (((type) == KYS_INT_KS_DONE_CLR) || \
                                   ((type) == KYS_INT_KEYFIFO_CLR) || \
                                   ((type) == KYS_INT_GHOST_CLR) ||   \
                                   ((type) == KYS_INT_CLR_ALL))

/*@} end of group KYS_Public_Constants */

/** @defgroup  KYS_Public_Macros
 *  @{
 */

/*@} end of group KYS_Public_Macros */

/** @defgroup  KYS_Public_Functions
 *  @{
 */

/**
 *  @brief UART Functions
 */
// #ifndef BFLB_USE_HAL_DRIVER
void KYS_IRQHandler(void);
// #endif
BL_Err_Type KYS_Init(KYS_CFG_Type *kysCfg);
BL_Err_Type KYS_Enable(void);
BL_Err_Type KYS_Disable(void);
BL_Err_Type KYS_IntMask(KYS_INT_Type intType, BL_Mask_Type intMask);
BL_Err_Type KYS_IntClear(KYS_INT_CLR_Type intType);
BL_Err_Type KYS_Int_Callback_Install(KYS_INT_Type intType, intCallback_Type *cbFun);
BL_Sts_Type KYS_GetIntStatus(KYS_INT_Type intType);
uint8_t KYS_ReadKeyfifo(void);
void KYS_Get_FIFO_Idx(uint8_t *fifo_head, uint8_t *fifo_tail);
// uint8_t KYS_GetKeycode(KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row);

/*@} end of group KYS_Public_Functions */

/*@} end of group KYS */

/*@} end of group BL702_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_KYS_H__ */
