/**
  ******************************************************************************
  * @file    bl616d_pds.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "bl616d.h"
#include "bl616d_pds.h"
#include "bflb_gpio.h"

/** @addtogroup  BL616D_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PDS
 *  @{
 */

/** @defgroup  PDS_Private_Macros
 *  @{
 */

/*@} end of group PDS_Private_Macros */

/** @defgroup  PDS_Private_Types
 *  @{
 */

/*@} end of group PDS_Private_Types */

/** @defgroup  PDS_Private_Variables
 *  @{
 */

/*@} end of group PDS_Private_Variables */

/** @defgroup  PDS_Global_Variables
 *  @{
 */

/*@} end of group PDS_Global_Variables */

/** @defgroup  PDS_Private_Fun_Declaration
 *  @{
 */

/*@} end of group PDS_Private_Fun_Declaration */

/** @defgroup  PDS_Private_Functions
 *  @{
 */

/*@} end of group PDS_Private_Functions */

/** @defgroup  PDS_Public_Functions
 *  @{
 */
/****************************************************************************/ /**
 * @brief  set gpio pad ie in pds
 *
 * @param  pad: GPIO pin
 *
 * @param  ie: Active IE (interrupt)
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Ie(uint8_t pad, uint8_t ie)
{
    uint32_t tmpVal;
    uint32_t pos = 0;

    if ((pad >= 0) && (pad <= 31)) {
        pos = pad;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_0);
        if (ie) {
            tmpVal |= (1 << pos);
        } else {
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_IE_0, tmpVal);

    } else if ((pad >= 32) && (pad < GPIO_PIN_MAX)) {
        pos = pad - 32;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_1);
        if (ie) {
            tmpVal |= (1 << pos);
        } else {
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_IE_1, tmpVal);
    } else {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad pd in pds
 *
 * @param  pad: GPIO pin
 *
 * @param  pd: pull down
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pd(uint8_t pad, uint8_t pd)
{
    uint32_t tmpVal;
    uint32_t pos = 0;

    if ((pad >= 0) && (pad <= 31)) {
        pos = pad;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_0);
        if (pd) {
            tmpVal |= (1 << pos);
        } else {
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_0, tmpVal);

    } else if ((pad >= 32) && (pad < GPIO_PIN_MAX)) {
        pos = pad - 32;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_1);
        if (pd) {
            tmpVal |= (1 << pos);
        } else {
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_1, tmpVal);
    } else {
        return ERROR;
    }

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  set gpio pad pu in pds
 *
 * @param  pad: GPIO pin
 *
 * @param  pu: pull up
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pu(uint8_t pad, uint8_t pu)
{
    uint32_t tmpVal;
    uint32_t pos = 0;

    if ((pad >= 0) && (pad <= 31)) {
        pos = pad;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PU_0);
        if (pu) {
            tmpVal |= (1 << pos);
        } else {
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_PU_0, tmpVal);

    } else if ((pad >= 32) && (pad < GPIO_PIN_MAX)) {
        pos = pad - 32;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PU_1);
        if (pu) {
            tmpVal |= (1 << pos);
        } else {
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_PU_1, tmpVal);
    } else {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad pull type in pds
 *
 * @param  pad: gpio
 *
 * @param  pu: power up
 * @param  pd: power down
 * @param  ie: Active IE (interrupt)
 *
 * @return SUCCESS or ERROR
 *
 * @note   Pu and Pd not depend on IE
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(uint8_t pad, uint8_t pu, uint8_t pd, uint8_t ie)
{
    /* pu/pd/ie config */
    PDS_Set_GPIO_Pu(pad, pu);
    PDS_Set_GPIO_Pd(pad, pd);
    PDS_Set_GPIO_Ie(pad, ie);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int mask type in pds
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;
    uint32_t pos = 0;

    if ((pad >= 0) && (pad <= 31)) {
        pos = pad;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET);
        if (UNMASK != intMask) {
            /* MASK */
            tmpVal |= (1 << pos);
        } else {
            /* UNMASK */
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET, tmpVal);

    } else if ((pad >= 32) && (pad < GPIO_PIN_MAX)) {
        pos = pad - 32;

        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_SET2);
        if (UNMASK != intMask) {
            /* MASK */
            tmpVal |= (1 << pos);
        } else {
            /* UNMASK */
            tmpVal &= ~(1 << pos);
        }
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_SET2, tmpVal);
    } else {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad trig type in pds
 *
 * @param  pin: GPIO pin number
 *
* @param  trig: trig type, this parameter can be one of the following values:
 *           @arg PDS_GPIO_INT_SYNC_FALLING_EDGE
 *           @arg PDS_GPIO_INT_SYNC_RISING_EDGE
 *           @arg PDS_GPIO_INT_SYNC_HIGH_LEVEL
 *           @arg PDS_GPIO_INT_ASYNC_FALLING_EDGE
 *           @arg PDS_GPIO_INT_ASYNC_RISING_EDGE
 *           @arg PDS_GPIO_INT_ASYNC_HIGH_LEVEL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntMode(uint8_t pin, uint8_t trig)
{
    uint32_t tmpVal1 = 0;
    uint32_t tmpVal2 = 0;

    tmpVal1 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    tmpVal2 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT2);
    if (pin < 8) {
        tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, PDS_GPIO_SET1_INT_MODE, trig);
    } else if (pin < 16) {
        tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, PDS_GPIO_SET2_INT_MODE, trig);
    } else if (pin < 24) {
        tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, PDS_GPIO_SET3_INT_MODE, trig);
    } else if (pin < 32) {
        tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, PDS_GPIO_SET4_INT_MODE, trig);
    } else if (pin < 36) {
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, PDS_GPIO_SET5_INT_MODE, trig);
    } else if (pin < 40) {
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, PDS_GPIO_SET6_INT_MODE, trig);
    } else if (pin < 44) {
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, PDS_GPIO_SET7_INT_MODE, trig);
    } else if (pin < 52) {
        tmpVal2 = BL_SET_REG_BITS_VAL(tmpVal2, PDS_GPIO_SET8_INT_MODE, trig);
    } else {
        return ERROR;
    }

    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal1);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT2, tmpVal2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set gpio pad int clear in pds
 *
 * @param  pin: GPIO pin number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_GPIO_Pad_IntClr(uint8_t pin)
{
    uint32_t tmpVal1 = 0;
    uint32_t tmpVal2 = 0;

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal1 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    tmpVal2 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT2);
    if (pin < 8) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET1_INT_CLR);
    } else if (pin < 16) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET2_INT_CLR);
    } else if (pin < 24) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET3_INT_CLR);
    } else if (pin < 32) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET4_INT_CLR);
    } else if (pin < 36) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET5_INT_CLR);
    } else if (pin < 40) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET6_INT_CLR);
    } else if (pin < 44) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET7_INT_CLR);
    } else if (pin < 52) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET8_INT_CLR);
    } else {
        return ERROR;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal1);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT2, tmpVal2);

    /* pds_gpio_setx_int_clr = 1 */
    tmpVal1 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    tmpVal2 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT2);
    if (pin < 8) {
        tmpVal1 = BL_SET_REG_BIT(tmpVal1, PDS_GPIO_SET1_INT_CLR);
    } else if (pin < 16) {
        tmpVal1 = BL_SET_REG_BIT(tmpVal1, PDS_GPIO_SET2_INT_CLR);
    } else if (pin < 24) {
        tmpVal1 = BL_SET_REG_BIT(tmpVal1, PDS_GPIO_SET3_INT_CLR);
    } else if (pin < 32) {
        tmpVal1 = BL_SET_REG_BIT(tmpVal1, PDS_GPIO_SET4_INT_CLR);
    } else if (pin < 36) {
        tmpVal2 = BL_SET_REG_BIT(tmpVal2, PDS_GPIO_SET5_INT_CLR);
    } else if (pin < 40) {
        tmpVal2 = BL_SET_REG_BIT(tmpVal2, PDS_GPIO_SET6_INT_CLR);
    } else if (pin < 44) {
        tmpVal2 = BL_SET_REG_BIT(tmpVal2, PDS_GPIO_SET7_INT_CLR);
    } else if (pin < 52) {
        tmpVal2 = BL_SET_REG_BIT(tmpVal2, PDS_GPIO_SET8_INT_CLR);
    } else {
        return ERROR;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal1);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT2, tmpVal2);

    /* pds_gpio_setx_int_clr = 0 */
    tmpVal1 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    tmpVal2 = BL_RD_REG(PDS_BASE, PDS_GPIO_INT2);
    if (pin < 8) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET1_INT_CLR);
    } else if (pin < 16) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET2_INT_CLR);
    } else if (pin < 24) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET3_INT_CLR);
    } else if (pin < 32) {
        tmpVal1 = BL_CLR_REG_BIT(tmpVal1, PDS_GPIO_SET4_INT_CLR);
    } else if (pin < 36) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET5_INT_CLR);
    } else if (pin < 40) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET6_INT_CLR);
    } else if (pin < 44) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET7_INT_CLR);
    } else if (pin < 52) {
        tmpVal2 = BL_CLR_REG_BIT(tmpVal2, PDS_GPIO_SET8_INT_CLR);
    } else {
        return ERROR;
    }
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal1);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT2, tmpVal2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get gpio pad int status
 *
 * @param  pad: gpio type, this parameter can be GLB_GPIO_PIN_xx where xx is 0~34
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Get_GPIO_Pad_IntStatus(uint8_t pad)
{
    uint32_t pos = 0;

    if ((pad >= 0) && (pad <= 31)) {
        pos = pad;

        return (BL_RD_REG(PDS_BASE, PDS_GPIO_STAT) & (1 << pos)) ? SET : RESET;

    } else if ((pad >= 32) && (pad < GPIO_PIN_MAX)) {
        pos = pad - 32;

        return (BL_RD_REG(PDS_BASE, PDS_GPIO_INT_STAT2) & (1 << pos)) ? SET : RESET;
    } else {
        return ERROR;
    }
}

/****************************************************************************/ /**
 * @brief  set all gpio pad int clear in pds
 *
 * @param  NULL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_GPIO_Pad_IntClr(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET1_INT_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET2_INT_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET3_INT_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET4_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT2);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET5_INT_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET6_INT_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET7_INT_CLR);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_GPIO_SET8_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT2, tmpVal);

    __NOP();
    __NOP();
    __NOP();

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET1_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET2_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET3_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET4_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET5_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET6_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET7_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_SET8_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_GPIO_INT2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get all gpio pad int status in pds
 *
 * @param  NULL
 *
 * @return pds_gpio_int_stat
 *
*******************************************************************************/
uint64_t ATTR_TCM_SECTION PDS_Get_All_GPIO_Pad_IntStatus(void)
{
    return (((uint64_t)BL_RD_REG(PDS_BASE, PDS_GPIO_INT_STAT2)) << 32) | ((uint64_t)BL_RD_REG(PDS_BASE, PDS_GPIO_STAT));
}

/****************************************************************************/ /**
 * @brief  set flash pad pull none
 *
 * @param  pinCfg: flash pin type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None(uint8_t pinCfg)
{
    if ((pinCfg == SF_IO_EXT_SF2_SWAP_IO3IO0) || (pinCfg >= SF_IO_EMB_SWAP_IO3IO0_AND_SF2_SWAP_IO3IO0)) {
        for (uint8_t i = 34; i <= 39; i++) {
            PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(i, 0, 0, 0);
        }
    } else if (pinCfg == SF_IO_EXT_SF3) {
        for (uint8_t i = 43; i <= 48; i++) {
            PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(i, 0, 0, 0);
        }
    } else {
        return INVALID;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set flash pad pull none
 *
 * @param  pinCfg: flash pin type
 *
 * @return SUCCESS or ERROR
 *
 * @note ext_flash need call this function after pds mode
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Flash_Pad_Pull_None_Fast(uint8_t pinCfg)
{
    uint32_t tmpVal;

    if ((pinCfg == SF_IO_EXT_SF2_SWAP_IO3IO0) || (pinCfg >= SF_IO_EMB_SWAP_IO3IO0_AND_SF2_SWAP_IO3IO0)) {
        /* ie config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_1);
        /* GPIO34~39 = 0 */
        tmpVal &= ~(0x3f << (34 - 32));
        BL_WR_REG(PDS_BASE, PDS_GPIO_IE_1, tmpVal);

        /* pd config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_1);
        /* GPIO34~39 = 0 */
        tmpVal &= ~(0x3f << (34 - 32));
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_1, tmpVal);

        /* pu config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PU_1);
        /* GPIO34~39 = 0 */
        tmpVal &= ~(0x3f << (34 - 32));
        BL_WR_REG(PDS_BASE, PDS_GPIO_PU_1, tmpVal);

    } else if (pinCfg == SF_IO_EXT_SF3) {
        /* ie config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_1);
        /* GPIO43~48 = 0 */
        tmpVal &= ~(0x3f << (43 - 32));
        BL_WR_REG(PDS_BASE, PDS_GPIO_IE_1, tmpVal);

        /* pd config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PD_1);
        /* GPIO43~48 = 0 */
        tmpVal &= ~(0x3f << (43 - 32));
        BL_WR_REG(PDS_BASE, PDS_GPIO_PD_1, tmpVal);

        /* pu config */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_PU_1);
        /* GPIO43~48 = 0 */
        tmpVal &= ~(0x3f << (43 - 32));
        BL_WR_REG(PDS_BASE, PDS_GPIO_PU_1, tmpVal);

    } else {
        return INVALID;
    }
    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  Disable PDS GPIO Keep
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Disable_GPIO_Keep(void)
{
    uint32_t tmpVal = 0;

    /* PDS_IO keep disable */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_ISO_MODE);
    /* don't entry PDS */
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_START_PS);
    BL_WR_REG(PDS_BASE, PDS_CTL, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL5);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_GPIO_KEEP_EN);
    BL_WR_REG(PDS_BASE, PDS_CTL5, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU0 clock enable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU0_Clock_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_MCU_MAIN_CLK_EN);
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU0 clock disable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU0_Clock_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_MCU_MAIN_CLK_EN);
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU1 clock enable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU1_Clock_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_REG_MCU_WL_CLK_EN);
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU1 clock disable
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU1_Clock_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CPU_CORE_CFG1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_REG_MCU_WL_CLK_EN);
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set MCU0 reset address
 *
 * @param  addr: address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Set_MCU0_Reset_Address(uint32_t addr)
{
    BL_WR_REG(PDS_BASE, PDS_CPU_CORE_CFG14, addr);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  Enable PDS power on PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Pu_PLL_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_PU_RST_CLKPLL);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    BL_WR_REG(PDS_BASE, PDS_PU_RST_CLKPLL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS power on PLL
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Pu_PLL_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_PU_RST_CLKPLL);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL_SFREG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_PU_CLKPLL);
    BL_WR_REG(PDS_BASE, PDS_PU_RST_CLKPLL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable power down sleep
 *
 * @param  cfg: power down sleep configuration 1
 * @param  cfg4: power down sleep configuration 2
 * @param  pdsSleepCnt: power down sleep count cycle
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt)
{
    uint32_t tmpVal = 0;
    /* sleepForever set 1, chip will sleep forever */
    /* PDS sleep time 1~PDS_WARMUP_LATENCY_CNT <=> error */
    /* PDS sleep time >PDS_WARMUP_LATENCY_CNT <=> correct */
    if ((pdsSleepCnt) && (pdsSleepCnt <= PDS_WARMUP_LATENCY_CNT)) {
        return ERROR;
    } else if (pdsSleepCnt > PDS_WARMUP_LATENCY_CNT) {
        BL_WR_REG(PDS_BASE, PDS_TIME1, pdsSleepCnt - PDS_WARMUP_LATENCY_CNT);
    } else {
    }

    /* PDS_CTL4 config */
    BL_WR_REG(PDS_BASE, PDS_CTL4, *(uint32_t *)cfg4);

    /* PDS_CTL config */
    if (cfg->pdsStart) {
        /* clear pds int */
        tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
        tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
        BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

        tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
        BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

        BL_WR_REG(PDS_BASE, PDS_CTL, (*(uint32_t *)cfg & ~(1 << 0)));
        BL_WR_REG(PDS_BASE, PDS_CTL, (*(uint32_t *)cfg | (1 << 0)));
    } else {
        BL_WR_REG(PDS_BASE, PDS_CTL, *(uint32_t *)cfg);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep force configure
 *
 * @param  cfg2: power down sleep force configuration 1
 * @param  cfg3: power down sleep force configuration 2
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5)
{
    /* PDS_CTL2 config */
    BL_WR_REG(PDS_BASE, PDS_CTL2, *(uint32_t *)cfg2);

    /* PDS_CTL3 config */
    BL_WR_REG(PDS_BASE, PDS_CTL3, *(uint32_t *)cfg3);

    /* PDS_CTL5 config */
    BL_WR_REG(PDS_BASE, PDS_CTL5, *(uint32_t *)cfg5);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep ram configure
 *
 * @param  ocramCfg: power down sleep force ocram configuration
 * @param  wramCfg: power down sleep force wram configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg,
                                            PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg)
{
    if ((NULL == ram1Cfg) || (NULL == ram2Cfg) || (NULL == ram3Cfg) || (NULL == ram4Cfg)) {
        return ERROR;
    }

    BL_WR_REG(PDS_BASE, PDS_RAM1, *(uint32_t *)ram1Cfg);
    BL_WR_REG(PDS_BASE, PDS_RAM2, *(uint32_t *)ram2Cfg);
    BL_WR_REG(PDS_BASE, PDS_RAM3, *(uint32_t *)ram3Cfg);
    BL_WR_REG(PDS_BASE, PDS_RAM4, *(uint32_t *)ram4Cfg);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All WRAM(320KB) Retention
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_WRAM_Retention(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0x3FF);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All OCRAM(320KB) Retention
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_OCRAM_Retention(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM4, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_RET, 0xFFFFF);
    BL_WR_REG(PDS_BASE, PDS_RAM3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All WRAM(320KB) Sleep
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_WRAM_Sleep(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0x3FF);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Set All OCRAM(320KB) Sleep
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_All_OCRAM_Sleep(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM3, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_SLP, 0xFFFFF);
    BL_WR_REG(PDS_BASE, PDS_RAM4, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable PDS Control WRAM and OCRAM
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Disable_Ctrl_Ram(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_CTRL_RAM_CLK);
    BL_WR_REG(PDS_BASE, PDS_RAM1, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_WRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM2, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_RET, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM3, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_RAM4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_OCRAM_SLP, 0);
    BL_WR_REG(PDS_BASE, PDS_RAM4, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep force configure
 *
 * @param  defaultLvCfg: power down sleep default level configuration
 * @param  ramCfg: ram configuration
 * @param  pdsSleepCnt: power down sleep time count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt)
{
    PDS_Force_Config((PDS_CTL2_Type *)&(defaultLvCfg->pdsCtl2), (PDS_CTL3_Type *)&(defaultLvCfg->pdsCtl3), (PDS_CTL5_Type *)&(defaultLvCfg->pdsCtl5));
    PDS_Enable((PDS_CTL_Type *)&(defaultLvCfg->pdsCtl), (PDS_CTL4_Type *)&(defaultLvCfg->pdsCtl4), pdsSleepCnt);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  power down sleep int mask
 *
 * @param  intType: PDS int type, this parameter can be one of the following values:
 *           @arg PDS_INT_WAKEUP
 *           @arg PDS_INT_RF_DONE
 *           @arg PDS_INT_WIFI_TBTT_SLEEP
 *           @arg PDS_INT_WIFI_TBTT_WAKEUP
 *           @arg PDS_INT_MAX
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_IntMask(uint8_t intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_PDS_INT_TYPE(intType));

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    if (intMask != UNMASK) {
        tmpVal = tmpVal | (1 << (intType + PDS_INT_MASK_BIT_OFFSET));
    } else {
        tmpVal = tmpVal & ~(1 << (intType + PDS_INT_MASK_BIT_OFFSET));
    }
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get power down sleep int status
 *
 * @param  intType: PDS int type, this parameter can be one of the following values:
 *           @arg PDS_INT_WAKEUP
 *           @arg PDS_INT_RF_DONE
 *           @arg PDS_INT_WIFI_TBTT_SLEEP
 *           @arg PDS_INT_WIFI_TBTT_WAKEUP
 *           @arg PDS_INT_MAX
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Get_IntStatus(uint8_t intType)
{
    CHECK_PARAM(IS_PDS_INT_TYPE(intType));

    return (BL_RD_REG(PDS_BASE, PDS_INT) & (1 << intType)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  clear power down sleep int status
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_IntClear(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_INT_CLR);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  unmask or mask PDS wakeup source
 *
 * @param  WakeupType: wakeup pds source, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_IRRX_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USB_SDU
 *           @arg PDS_WAKEUP_BY_KEYSCAN
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 *           @arg PDS_WAKEUP_BY_SW_CONFIG
 * @param  intMask: MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Set_Wakeup_Src_IntMask(uint32_t WakeupType, BL_Mask_Type intMask)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;
    uint32_t pds_wakeup_src_en_bits = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);

    pds_wakeup_src_en_bits = BL_GET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);

    if (intMask != UNMASK) {
        pds_wakeup_src_en_bits &= ~((uint32_t)1 << WakeupType);
    } else {
        pds_wakeup_src_en_bits |= ((uint32_t)1 << WakeupType);
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN, pds_wakeup_src_en_bits);

    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get wakeup source
 *
 * @param  WakeupType: wakeup pds source, this parameter can be one of the following values:
 *           @arg PDS_WAKEUP_BY_PDS_TIMER
 *           @arg PDS_WAKEUP_BY_HBN_IRQ_OUT
 *           @arg PDS_WAKEUP_BY_GPIO_IRQ
 *           @arg PDS_WAKEUP_BY_PDS_GPIO_INT
 *           @arg PDS_WAKEUP_BY_IRRX_INT
 *           @arg PDS_WAKEUP_BY_WIFI_WAKEUP
 *           @arg PDS_WAKEUP_BY_DM_SLP_IRQ
 *           @arg PDS_WAKEUP_BY_USB_SDU
 *           @arg PDS_WAKEUP_BY_KEYSCAN
 *           @arg PDS_WAKEUP_BY_WIFI_TBTT_WAKEUP_IRQ
 *           @arg PDS_WAKEUP_BY_SW_CONFIG
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type ATTR_TCM_SECTION PDS_Get_Wakeup_Src(uint32_t WakeupType)
{
    CHECK_PARAM(IS_PDS_WAKEUP_EVENT_TYPE(WakeupType));

    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);
    tmpVal &= ((uint32_t)1 << WakeupType);

    return tmpVal ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Get wakeup source
 *
 * @return all Wakeup source state
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION PDS_Get_All_Wakeup_Src(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PDS_RO_PDS_WAKEUP_EVENT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  Mask All Wakeup Source
 *
*******************************************************************************/
void ATTR_TCM_SECTION PDS_Mask_All_Wakeup_Src(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_RF_DONE_INT_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_WIFI_TBTT_SLEEP_IRQ_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_WIFI_TBTT_WAKEUP_IRQ_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);
}

void ATTR_TCM_SECTION PDS_Triger_SW_Wakeup_PDS(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, MINI_MISC_CR_LPCPU_TRIG_INT);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_INT, tmpVal);
}

void ATTR_TCM_SECTION PDS_Clear_SW_Wakeup_PDS(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(MINI_MISC, MINI_MISC_MINI_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, MINI_MISC_CR_LPCPU_TRIG_INT);
    BL_WR_REG(MINI_MISC, MINI_MISC_MINI_INT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Trim_RC32M(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "rc32m", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 8)) {
            tmpVal = BL_RD_REG(PDS_BASE, PDS_RC32M_CTRL0);
            tmpVal = BL_SET_REG_BIT(tmpVal, PDS_RC32M_EXT_CODE_EN);
            BL_WR_REG(PDS_BASE, PDS_RC32M_CTRL0, tmpVal);
            arch_delay_us(2);
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL2_AON);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_RC32M_CODE_FR_EXT2, trim.value);
            BL_WR_REG(AON_BASE, AON_RC32M_CTRL2_AON, tmpVal);
            tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL2_AON);
            tmpVal = BL_SET_REG_BIT(tmpVal, AON_RC32M_EXT_CODE_SEL);
            BL_WR_REG(AON_BASE, AON_RC32M_CTRL2_AON, tmpVal);
            /* hw_5T + sw_5T  */
            arch_delay_us(1);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim RC32M
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION PDS_Trim_USB20(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "usb20", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 7)) {
            tmpVal = BL_RD_REG(PDS_BASE, PDS_U2PHY_2);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_USB20_RCAL_COARSE, (trim.value >> 5) & 0x3);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_USB20_RCAL_CODE, trim.value & 0x1F);
            BL_WR_REG(PDS_BASE, PDS_U2PHY_2, tmpVal);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  PDS Power Off PD_WB
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Power_Off_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PDS Power On PD_WB
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION PDS_Power_On_WB(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_CTL2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_PWR_OFF);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_FORCE_WB_ISO_EN);
    BL_WR_REG(PDS_BASE, PDS_CTL2, tmpVal);

    return SUCCESS;
}

/*@} end of group PDS_Public_Functions */

/*@} end of group PDS */

/*@} end of group BL616D_Peripheral_Driver */
