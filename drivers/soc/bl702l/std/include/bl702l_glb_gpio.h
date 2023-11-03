/**
  ******************************************************************************
  * @file    bl702l_glb_gpio.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2022 Bouffalo Lab</center></h2>
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
#ifndef __BL702L_GLB_GPIO_H__
#define __BL702L_GLB_GPIO_H__

#include "glb_reg.h"
#include "pds_reg.h"
#include "bl702l_gpio.h"
#include "bl702l_l1c.h"
#include "bl702l_hbn.h"
#include "bl702l_aon.h"
#include "bl702l_pds.h"
#include "bl702l_common.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  GLB_GPIO
 *  @{
 */

#define GLB_GPIO_OFFSET           0x100
#define GLB_GPIO_INPUT_OFFSET     0x180
#define GLB_GPIO_OUTPUT_OFFSET    0x188
#define GLB_GPIO_OUTPUT_EN_OFFSET 0x190

/**
 *  @brief GLB GPIO interrupt trigger mode type definition
 */
typedef enum {
    GLB_GPIO_INT_TRIG_SYNC_FALLING_EDGE = 0,        /*!< GPIO interrupt sync mode, GPIO falling edge trigger interrupt */
    GLB_GPIO_INT_TRIG_SYNC_RISING_EDGE = 1,         /*!< GPIO interrupt sync mode, GPIO rising edge trigger interrupt */
    GLB_GPIO_INT_TRIG_SYNC_LOW_LEVEL = 2,           /*!< GPIO interrupt sync mode, GPIO low level trigger interrupt (32k 3T) */
    GLB_GPIO_INT_TRIG_SYNC_HIGH_LEVEL = 3,          /*!< GPIO interrupt sync mode, GPIO high level trigger interrupt (32k 3T) */
    GLB_GPIO_INT_TRIG_SYNC_FALLING_RISING_EDGE = 4, /*!< GPIO interrupt sync mode, GPIO falling and rising edge trigger interrupt */
    GLB_GPIO_INT_TRIG_ASYNC_FALLING_EDGE = 8,       /*!< GPIO interrupt async mode, GPIO falling edge trigger interrupt */
    GLB_GPIO_INT_TRIG_ASYNC_RISING_EDGE = 9,        /*!< GPIO interrupt async mode, GPIO rising edge trigger interrupt */
    GLB_GPIO_INT_TRIG_ASYNC_LOW_LEVEL = 10,         /*!< GPIO interrupt async mode, GPIO low level trigger interrupt (32k 3T) */
    GLB_GPIO_INT_TRIG_ASYNC_HIGH_LEVEL = 11,        /*!< GPIO interrupt async mode, GPIO high level trigger interrupt (32k 3T) */
} GLB_GPIO_INT_TRIG_Type;

/**
 *  @brief GLB GPIO output mode type definition
 */
typedef enum {
    GLB_GPIO_OUTPUT_MODE_WRITE = 0,  /*!< GPIO output mode write */
    GLB_GPIO_OUTPUT_MODE_SETCLR = 1, /*!< GPIO output mode set/clr */
} GLB_GPIO_OUTPUT_MODE_Type;

/**
 *  @brief GPIO interrupt configuration structure type definition
 */
typedef struct
{
    GLB_GPIO_Type gpioPin;       /*!< GPIO pin num */
    GLB_GPIO_INT_TRIG_Type trig; /*!< GPIO interrupt trig mode */
    BL_Mask_Type intMask;        /*!< GPIO interrupt mask config */
} GLB_GPIO_INT_Cfg_Type;

/** @defgroup  GLB_Public_Functions
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
void GPIO_INT0_IRQHandler(void);
#endif
BL_Err_Type GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg);
BL_Err_Type GLB_GPIO_Func_Init(GLB_GPIO_FUNC_Type gpioFun, GLB_GPIO_Type *pinList, uint8_t cnt);
BL_Err_Type GLB_GPIO_OUTPUT_Mode_Set(GLB_GPIO_Type gpioPin, GLB_GPIO_OUTPUT_MODE_Type mode);
BL_Err_Type GLB_GPIO_INPUT_Enable(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_INPUT_Disable(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_OUTPUT_Enable(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_OUTPUT_Disable(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_Set_HZ(GLB_GPIO_Type gpioPin);
uint8_t GLB_GPIO_Get_Fun(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_Write(GLB_GPIO_Type gpioPin, uint32_t val);
uint32_t GLB_GPIO_Read(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_Set(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_Clr(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_IntMask(GLB_GPIO_Type gpioPin, BL_Mask_Type intMask);
BL_Err_Type GLB_Clr_GPIO_IntStatus(GLB_GPIO_Type gpioPin);
BL_Sts_Type GLB_Get_GPIO_IntStatus(GLB_GPIO_Type gpioPin);
BL_Err_Type GLB_GPIO_Int_Init(GLB_GPIO_INT_Cfg_Type *intCfg);
BL_Err_Type GLB_GPIO_INT0_IRQHandler_Install(void);
BL_Err_Type GLB_GPIO_INT0_Callback_Install(GLB_GPIO_Type gpioPin, intCallback_Type *cbFun);

/*----------*/;

/*@} end of group GLB_GPIO_Public_Functions */

/*@} end of group GLB_GPIO */

/*@} end of group BL702L_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL702L_GLB_GPIO_H__ */
