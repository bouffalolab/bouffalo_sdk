/**
  ******************************************************************************
  * @file    bl702l_glb_gpio.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "bl702l_glb.h"
#include "bl702l_glb_gpio.h"
#include "bl702l_hbn.h"

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  GLB
 *  @{
 */

/** @defgroup  GLB_Private_Macros
 *  @{
 */

#define GLB_GPIO_Get_Reg(pin)       (glb_gpio_reg_t *)(GLB_BASE + GLB_GPIO_OFFSET + (pin / 2) * 4)
#define GLB_GPIO_INT0_NUM           (32)
#define GLB_GPIO_INT0_CLEAR_TIMEOUT (32)

/*@} end of group GLB_Private_Macros */

/** @defgroup  GLB_Private_Types
 *  @{
 */

/*@} end of group GLB_Private_Types */

/** @defgroup  GLB_Private_Variables
 *  @{
 */
static intCallback_Type *glbGpioInt0CbfArra[GLB_GPIO_INT0_NUM] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                                   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                                   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                                                   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*@} end of group GLB_Private_Variables */

/** @defgroup  GLB_Global_Variables
 *  @{
 */

/*@} end of group GLB_Global_Variables */

/** @defgroup  GLB_Private_Fun_Declaration
 *  @{
 */

/*@} end of group GLB_Private_Fun_Declaration */

/** @defgroup  GLB_Private_Functions
 *  @{
 */

/*@} end of group GLB_Private_Functions */

/** @defgroup  GLB_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  GPIO initialization
 *
 * @param  cfg: GPIO configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg)
{
    uint8_t gpioPin = cfg->gpioPin;
    uint32_t *pOut;
    uint32_t pos;
    uint32_t tmpOut;
    uint32_t tmpVal;

    /* drive strength(drive) = 0  <=>  8.0mA  @ 3.3V */
    /* drive strength(drive) = 1  <=>  9.6mA  @ 3.3V */
    /* drive strength(drive) = 2  <=>  11.2mA @ 3.3V */
    /* drive strength(drive) = 3  <=>  12.8mA @ 3.3V */

    pOut = (uint32_t *)(GLB_BASE + GLB_GPIO_OUTPUT_EN_OFFSET + ((gpioPin >> 5) << 2));
    pos = gpioPin % 32;
    tmpOut = *pOut;

    /* Disable output anyway*/
    tmpOut &= (~(1 << pos));
    *pOut = tmpOut;

    tmpVal = BL_RD_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4);
    if (gpioPin % 2 == 0) {
        if (cfg->gpioMode != GPIO_MODE_ANALOG) {
            /* not analog mode */

            /* Set input or output */
            if (cfg->gpioMode == GPIO_MODE_OUTPUT) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
                tmpOut |= (1 << pos);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
            }

            /* Set pull up or down */
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_PD);
            if (cfg->pullType == GPIO_PULL_UP) {
                tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_0_PU);
            } else if (cfg->pullType == GPIO_PULL_DOWN) {
                tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_0_PD);
            }
            if (gpioPin == GLB_GPIO_PIN_30) {
                *(uint32_t *)(HBN_BASE + HBN_PAD_CTRL_0_OFFSET) &= ~(1 << 25);
            }
        } else {
            /* analog mode */

            /* clear ie && oe */
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
            tmpOut &= ~(1 << pos);

            /* clear pu && pd */
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_PU);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_PD);
        }

        /* set drive && smt && func */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_0_DRV, cfg->drive);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_0_SMT, cfg->smtCtrl);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_0_FUNC_SEL, cfg->gpioFun);
    } else {
        if (cfg->gpioMode != GPIO_MODE_ANALOG) {
            /* not analog mode */

            /* Set input or output */
            if (cfg->gpioMode == GPIO_MODE_OUTPUT) {
                tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_IE);
                tmpOut |= (1 << pos);
            } else {
                tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_1_IE);
            }

            /* Set pull up or down */
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_PU);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_PD);
            if (cfg->pullType == GPIO_PULL_UP) {
                tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_1_PU);
            } else if (cfg->pullType == GPIO_PULL_DOWN) {
                tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_1_PD);
            }
            if (gpioPin == GLB_GPIO_PIN_31) {
                *(uint32_t *)(HBN_BASE + HBN_PAD_CTRL_0_OFFSET) &= ~(1 << 26);
            }
        } else {
            /* analog mode */

            /* clear ie && oe */
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_IE);
            tmpOut &= ~(1 << pos);

            /* clear pu && pd */
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_PU);
            tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_PD);
        }

        /* set drive && smt && func */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_1_DRV, cfg->drive);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_1_SMT, cfg->smtCtrl);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_1_FUNC_SEL, cfg->gpioFun);
    }
    BL_WR_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4, tmpVal);

    *pOut = tmpOut;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  init GPIO function in pin list
 *
 * @param  gpioFun: GPIO pin function
 * @param  pinList: GPIO pin list
 * @param  cnt: GPIO pin count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_Func_Init(GLB_GPIO_FUNC_Type gpioFun, GLB_GPIO_Type *pinList, uint8_t cnt)
{
    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = (uint8_t)gpioFun,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive = 1,
        .smtCtrl = 1
    };

    if (gpioFun == GPIO_FUN_ANALOG) {
        gpioCfg.gpioMode = GPIO_MODE_ANALOG;
    }

    for (uint8_t i = 0; i < cnt; i++) {
        gpioCfg.gpioPin = pinList[i];
        GLB_GPIO_Init(&gpioCfg);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set GPIO output mode in SWGPIO function
 *
 * @param  gpioPin: GPIO pin
 * @param  mode: output mode
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_OUTPUT_Mode_Set(GLB_GPIO_Type gpioPin, GLB_GPIO_OUTPUT_MODE_Type mode)
{
    uint32_t tmpVal;
    uint32_t pinOffset;

    pinOffset = (gpioPin >> 1) << 2;
    tmpVal = *(uint32_t *)(GLB_BASE + GLB_GPIO_OFFSET + pinOffset);
    if (gpioPin % 2 == 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_0_MODE, mode);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_1_MODE, mode);
    }
    *(uint32_t *)(GLB_BASE + GLB_GPIO_OFFSET + pinOffset) = tmpVal;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO set input function enable
 *
 * @param  gpioPin: GPIO pin
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_GPIO_INPUT_Enable(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal;
    uint32_t pinOffset;

    pinOffset = (gpioPin >> 1) << 2;
    tmpVal = *(uint32_t *)(GLB_BASE + GLB_GPIO_OFFSET + pinOffset);
    if (gpioPin % 2 == 0) {
        /* [0] is ie */
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
    } else {
        /* [16] is ie */
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_REG_GPIO_1_IE);
    }
    *(uint32_t *)(GLB_BASE + GLB_GPIO_OFFSET + pinOffset) = tmpVal;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO set input function disable
 *
 * @param  gpioPin: GPIO pin
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_GPIO_INPUT_Disable(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal;
    uint32_t pinOffset;

    pinOffset = (gpioPin >> 1) << 2;
    tmpVal = *(uint32_t *)(GLB_BASE + GLB_GPIO_OFFSET + pinOffset);
    if (gpioPin % 2 == 0) {
        /* [0] is ie */
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_0_IE);
    } else {
        /* [16] is ie */
        tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_REG_GPIO_1_IE);
    }
    *(uint32_t *)(GLB_BASE + GLB_GPIO_OFFSET + pinOffset) = tmpVal;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO set output function enable
 *
 * @param  gpioPin: GPIO pin
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_GPIO_OUTPUT_Enable(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_CFGCTL34);
    tmpVal = tmpVal | (1 << gpioPin);
    BL_WR_REG(GLB_BASE, GLB_GPIO_CFGCTL34, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO set output function disable
 *
 * @param  gpioPin: GPIO pin
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_GPIO_OUTPUT_Disable(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_CFGCTL34);
    tmpVal = tmpVal & ~(1 << gpioPin);
    BL_WR_REG(GLB_BASE, GLB_GPIO_CFGCTL34, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO set High-Z
 *
 * @param  gpioPin: GPIO pin
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION GLB_GPIO_Set_HZ(GLB_GPIO_Type gpioPin)
{
    uint32_t *pOut;
    uint32_t pos;
    uint32_t tmpOut;
    uint32_t tmpVal;

    tmpVal = BL_RD_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4);

    /* pu=0, pd=0, ie=0 */
    if (gpioPin % 2 == 0) {
        tmpVal = (tmpVal & 0xffffff00);
    } else {
        tmpVal = (tmpVal & 0xff00ffff);
    }

    BL_WR_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4, tmpVal);

    pOut = (uint32_t *)(GLB_BASE + GLB_GPIO_OUTPUT_EN_OFFSET + ((gpioPin >> 5) << 2));
    pos = gpioPin % 32;
    tmpOut = *pOut;

    /* Disable output anyway*/
    tmpOut &= (~(1 << pos));
    *pOut = tmpOut;

    tmpVal = BL_RD_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4);

    /* func_sel=swgpio */
    if (gpioPin % 2 == 0) {
        tmpVal = (tmpVal & 0xffff00ff);
        tmpVal |= 0x0B00;
    } else {
        tmpVal = (tmpVal & 0x00ffffff);
        tmpVal |= (0x0B00 << 16);
    }

    BL_WR_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4, tmpVal);

    /* Disable output anyway*/
    *pOut = tmpOut;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get GPIO function
 *
 * @param  gpioPin: GPIO type
 *
 * @return GPIO function
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_TCM_SECTION GLB_GPIO_Get_Fun(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_WORD(GLB_BASE + GLB_GPIO_OFFSET + gpioPin / 2 * 4);

    if (gpioPin % 2 == 0) {
        return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_0_FUNC_SEL);
    } else {
        return BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_GPIO_1_FUNC_SEL);
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Write GPIO
 *
 * @param  gpioPin: GPIO type
 * @param  val: GPIO value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_Write(GLB_GPIO_Type gpioPin, uint32_t val)
{
    uint32_t *pOut = (uint32_t *)(GLB_BASE + GLB_GPIO_OUTPUT_OFFSET + ((gpioPin >> 5) << 2));
    uint32_t pos = gpioPin % 32;
    uint32_t tmpOut;

    tmpOut = *pOut;
    if (val > 0) {
        tmpOut |= (1 << pos);
    } else {
        tmpOut &= (~(1 << pos));
    }
    *pOut = tmpOut;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Read GPIO
 *
 * @param  gpioPin: GPIO type
 *
 * @return GPIO value
 *
*******************************************************************************/
uint32_t GLB_GPIO_Read(GLB_GPIO_Type gpioPin)
{
    uint32_t *p = (uint32_t *)(GLB_BASE + GLB_GPIO_INPUT_OFFSET + ((gpioPin >> 5) << 2));
    uint32_t pos = gpioPin % 32;

    if ((*p) & (1 << pos)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  turn GPIO output high
 *
 * @param  gpioPin: GPIO type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_Set(GLB_GPIO_Type gpioPin)
{
    BL_WR_WORD(GLB_BASE + GLB_GPIO_CFGCTL35_OFFSET, 1 << gpioPin);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  turn GPIO output low
 *
 * @param  gpioPin: GPIO type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_Clr(GLB_GPIO_Type gpioPin)
{
    BL_WR_WORD(GLB_BASE + GLB_GPIO_CFGCTL36_OFFSET, 1 << gpioPin);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set GLB GPIO interrupt mask
 *
 * @param  gpioPin: GPIO type
 * @param  intMask: GPIO interrupt MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_IntMask(GLB_GPIO_Type gpioPin, BL_Mask_Type intMask)
{
    uint32_t tmpVal;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MASK1);
        if (intMask == MASK) {
            tmpVal = tmpVal | (1 << gpioPin);
        } else {
            tmpVal = tmpVal & ~(1 << gpioPin);
        }
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_MASK1, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set GLB GPIO interrupt mask
 *
 * @param  gpioPin: GPIO type
 * @param  intClear: GPIO interrupt clear or unclear
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_Clr_GPIO_IntStatus(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_CLR1);
        tmpVal = tmpVal | (1 << gpioPin);
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_CLR1, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get GLB GPIO interrrupt status
 *
 * @param  gpioPin: GPIO type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type GLB_Get_GPIO_IntStatus(GLB_GPIO_Type gpioPin)
{
    uint32_t tmpVal = 0;

    if (gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_STAT1);
    }

    return (tmpVal & (1 << gpioPin)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Set GLB GPIO interrupt mode
 *
 * @param  gpioPin: GPIO type
 * @param  intCtlMod: GPIO interrupt control mode
 * @param  intTrgMod: GPIO interrupt trigger mode
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_Int_Init(GLB_GPIO_INT_Cfg_Type *intCfg)
{
    uint32_t tmpVal;
    uint32_t tmpGpioPin;

    if (intCfg->gpioPin < GLB_GPIO_PIN_8) {
        /* GPIO0 ~ GPIO7 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET1);
        tmpGpioPin = intCfg->gpioPin;
        tmpVal = (tmpVal & ~(0xf << (4 * tmpGpioPin))) | (intCfg->trig << (4 * tmpGpioPin));
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET1, tmpVal);
    } else if (intCfg->gpioPin < GLB_GPIO_PIN_16) {
        /* GPIO8 ~ GPIO15 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET2);
        tmpGpioPin = intCfg->gpioPin - GLB_GPIO_PIN_8;
        tmpVal = (tmpVal & ~(0xf << (4 * tmpGpioPin))) | (intCfg->trig << (4 * tmpGpioPin));
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET2, tmpVal);
    } else if (intCfg->gpioPin < GLB_GPIO_PIN_24) {
        /* GPIO16 ~ GPIO23 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET3);
        tmpGpioPin = intCfg->gpioPin - GLB_GPIO_PIN_16;
        tmpVal = (tmpVal & ~(0xf << (4 * tmpGpioPin))) | (intCfg->trig << (4 * tmpGpioPin));
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET3, tmpVal);
    } else if (intCfg->gpioPin < GLB_GPIO_PIN_MAX) {
        /* GPIO24 ~ GPIO32 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET4);
        tmpGpioPin = intCfg->gpioPin - GLB_GPIO_PIN_24;
        tmpVal = (tmpVal & ~(0xf << (4 * tmpGpioPin))) | (intCfg->trig << (4 * tmpGpioPin));
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_MODE_SET4, tmpVal);
    }
    if (intCfg->gpioPin < 32) {
        /* GPIO0 ~ GPIO31 */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MASK1);
        if (intCfg->intMask == MASK) {
            tmpVal = tmpVal | (1 << intCfg->gpioPin);
        } else {
            tmpVal = tmpVal & ~(1 << intCfg->gpioPin);
        }
        BL_WR_REG(GLB_BASE, GLB_GPIO_INT_MASK1, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO INT0 IRQHandler install
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_INT0_IRQHandler_Install(void)
{
#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(GPIO_INT0_IRQn, GPIO_INT0_IRQHandler);
#endif

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO interrupt IRQ handler callback install
 *
 * @param  gpioPin: GPIO pin type
 * @param  cbFun: callback function
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type GLB_GPIO_INT0_Callback_Install(GLB_GPIO_Type gpioPin, intCallback_Type *cbFun)
{
    if (gpioPin < 32) {
        glbGpioInt0CbfArra[gpioPin] = cbFun;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  GPIO interrupt IRQ handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void GPIO_INT0_IRQHandler(void)
{
    GLB_GPIO_Type gpioPin;
    uint32_t timeOut = 0;

    for (gpioPin = GLB_GPIO_PIN_0; gpioPin <= GLB_GPIO_PIN_31; gpioPin++) {
        if (SET == GLB_Get_GPIO_IntStatus(gpioPin)) {
            GLB_Clr_GPIO_IntStatus(gpioPin);

            /* timeout check */
            timeOut = GLB_GPIO_INT0_CLEAR_TIMEOUT;
            do {
                timeOut--;
            } while ((SET == GLB_Get_GPIO_IntStatus(gpioPin)) && timeOut);
            if (!timeOut) {
                //MSG("WARNING: Clear GPIO interrupt status fail.\r\n");
            }

            /* if timeOut==0, GPIO interrupt status not cleared */
            //GLB_GPIO_IntClear(gpioPin, RESET);

            if (glbGpioInt0CbfArra[gpioPin] != NULL) {
                /* Call the callback function */
                glbGpioInt0CbfArra[gpioPin]();
            }
        }
    }
}
#endif

/*@} end of group GLB_Public_Functions */

/*@} end of group GLB */

/*@} end of group BL702L_Peripheral_Driver */
