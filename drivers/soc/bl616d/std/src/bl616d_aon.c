/**
  ******************************************************************************
  * @file    bl616d_aon.c
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

#include "bl616d_aon.h"
#include "bl616d_ef_cfg.h"
#include "bl616d_hbn.h"
/** @addtogroup  BL616D_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AON
 *  @{
 */

/** @defgroup  AON_Private_Macros
 *  @{
 */
#define AON_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }

/*@} end of group AON_Private_Macros */

/** @defgroup  AON_Private_Types
 *  @{
 */

/*@} end of group AON_Private_Types */

/** @defgroup  AON_Private_Variables
 *  @{
 */

/*@} end of group AON_Private_Variables */

/** @defgroup  AON_Global_Variables
 *  @{
 */

/*@} end of group AON_Global_Variables */

/** @defgroup  AON_Private_Fun_Declaration
 *  @{
 */

/*@} end of group AON_Private_Fun_Declaration */

/** @defgroup  AON_Private_Functions
 *  @{
 */

/*@} end of group AON_Private_Functions */

/** @defgroup  AON_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Power on Main band gap
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_On_MBG(void)
{
    uint32_t tmpVal = 0;

    /* Power up RF for PLL to work */
    tmpVal = BL_RD_REG(AON_BASE, AON_MBG_SFREG);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_MBG_AON);
    BL_WR_REG(AON_BASE, AON_MBG_SFREG, tmpVal);

    arch_delay_us(55);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off Main band gap
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_Off_MBG(void)
{
    uint32_t tmpVal = 0;

    /* Power OFF */
    tmpVal = BL_RD_REG(AON_BASE, AON_MBG_SFREG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_MBG_AON);
    BL_WR_REG(AON_BASE, AON_MBG_SFREG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on SFREG
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_On_SFReg(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_MBG_SFREG);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_SFREG_AON);
    BL_WR_REG(AON_BASE, AON_MBG_SFREG, tmpVal);

    arch_delay_us(55);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off SFREG
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_Off_SFReg(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_MBG_SFREG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_SFREG_AON);
    BL_WR_REG(AON_BASE, AON_MBG_SFREG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on RF
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_On_RF(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_1);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_SW_WB_EN_AON);
    BL_WR_REG(AON_BASE, AON_1, tmpVal);

    arch_delay_us(55);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off RF
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_Off_RF(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_SW_WB_EN_AON);
    BL_WR_REG(AON_BASE, AON_1, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on XTAL
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Power_On_XTAL(void)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_PU);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_XTAL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_PU, tmpVal);

    /* Polling for ready */
    do {
        arch_delay_us(10);
        timeOut++;
        tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);
    } while (!BL_IS_REG_BIT_SET(tmpVal, AON_XTAL_RDY) && timeOut < 120);

    if (timeOut >= 120) {
        return TIMEOUT;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set XTAL cap code
 *
 * @param  capIn: Cap code in
 * @param  capOut: Cap code out
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, capIn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, capOut);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE0, tmpVal);

    arch_delay_us(100);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get XTAL cap code
 *
 * @param  None
 *
 * @return Cap code
 *
*******************************************************************************/
uint8_t ATTR_CLOCK_SECTION AON_Get_Xtal_CapCode(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE0);

    return BL_GET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON);
}

/****************************************************************************/ /**
 * @brief  Power off XTAL
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Power_Off_XTAL(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_PU);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_XTAL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_PU, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on bandgap system
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_On_BG(void)
{
    uint32_t tmpVal = 0;

    /* power up RF for PLL to work */
    tmpVal = BL_RD_REG(AON_BASE, AON_BG_SYS_TOP);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_BG_SYS_AON);
    BL_WR_REG(AON_BASE, AON_BG_SYS_TOP, tmpVal);

    arch_delay_us(55);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off bandgap system
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_Off_BG(void)
{
    uint32_t tmpVal = 0;

    /* power up RF for PLL to work */
    tmpVal = BL_RD_REG(AON_BASE, AON_BG_SYS_TOP);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_BG_SYS_AON);
    BL_WR_REG(AON_BASE, AON_BG_SYS_TOP, tmpVal);

    arch_delay_us(55);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on LDO15_RF
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_On_LDO15_RF(void)
{
    uint32_t tmpVal = 0;

    /* ldo15rf power on */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_LDO15RF_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    arch_delay_us(90);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off LDO15_RF
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Power_Off_LDO15_RF(void)
{
    uint32_t tmpVal = 0;

    /* ldo15rf power off */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_LDO15RF_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  LDO15_RF Output Float
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Output_Float_LDO15_RF(void)
{
    uint32_t tmpVal = 0;

    /* ldo15rf pulldown select by aon */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    /* ldo15rf float output */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO15RF_PULLDOWN_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief   LDO15_RF Pulldown Output to Groud
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Output_Pulldown_LDO15_RF(void)
{
    uint32_t tmpVal = 0;

    /* ldo15rf pulldown select by aon */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    /* ldo15rf pulldown output to ground */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO15RF_PULLDOWN_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  LDO15_RF Output Pulldown Controlled by PD Signal
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Output_LDO15_RF_Ctrl_by_Pd(void)
{
    uint32_t tmpVal = 0;

    /* output ldo15rf pulldown select by pd signal */
    tmpVal = BL_RD_REG(AON_BASE, AON_RF_TOP_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO15RF_PULLDOWN_SEL_AON);
    BL_WR_REG(AON_BASE, AON_RF_TOP_AON, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set xtal32k's inverter amplify strength
 *
 * @param  value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Set_Xtal_32K_Inverter_Amplify_Strength(uint8_t value)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL32K_INV_STRE_AON, value);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set xtal32k_regulator
 *
 * @param  level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Set_Xtal_32K_Regulator(uint8_t level)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL32K_REG_AON, level);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Start XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note can't use GPIO16&17 after calling this function
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Start_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL32K_HIZ_EN_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_AC_CAP_SHORT_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_RDY_RSTB_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_FAST_STARTUP_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_XTAL32K_AON);
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Clear Xtal_32K_Fast_Startup
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 * @note  500uS after starting XTAL32K, call this function
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Clear_Xtal_32K_Fast_Startup(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL32K_FAST_STARTUP_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Xtal 32K Ready State
 *
 * @param  None
 *
 * @return SET or RESET
 * @note  At least 100ms after starting XTAL32K, Xtal 32K Ready State changes from RESET to SET
 *
*******************************************************************************/
BL_Sts_Type ATTR_CLOCK_SECTION AON_Get_Xtal_32K_Ready_State(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    return BL_IS_REG_BIT_SET(tmpVal, AON_XTAL32K_CLK_RDY);
}

/****************************************************************************/ /**
 * @brief  Power on XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    AON_Start_Xtal_32K();

    arch_delay_us(500);

    AON_Clear_Xtal_32K_Fast_Startup();

    /* Polling for ready */
    do {
        arch_delay_us(10);
        timeOut++;
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    } while (!BL_IS_REG_BIT_SET(tmpVal, AON_XTAL32K_CLK_RDY) && timeOut < 120000);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Power_Off_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_XTAL32K_AON);
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_HIZ_EN_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL32K_AC_CAP_SHORT_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    /* Delay >1us */
    arch_delay_us(2);
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL32K_RDY_RSTB_AON);
    BL_WR_REG(AON_BASE, AON_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON set LDO18_AON voltage out
 *
 * @param  level: LDO18_AON volatge level, this parameter can be one of the following values:
 *           @arg AON_LDO18_AON_LEVEL_1P65V
 *           @arg AON_LDO18_AON_LEVEL_1P70V
 *           @arg AON_LDO18_AON_LEVEL_1P75V
 *           @arg AON_LDO18_AON_LEVEL_1P80V
 *           @arg AON_LDO18_AON_LEVEL_1P85V
 *           @arg AON_LDO18_AON_LEVEL_1P90V
 *           @arg AON_LDO18_AON_LEVEL_1P95V
 *           @arg AON_LDO18_AON_LEVEL_2P00V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo18_Aon_Vout(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO18_AON_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_VOUT_SEL_AON, level);
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON trim LDO18_AON voltage
 *
 * @param  trim:LDO18_AON volatge trim value, this parameter can be one of the following values:
 *          @arg AON_LDO18_AON_TRIM_0P92
 *          @arg AON_LDO18_AON_TRIM_0P93
 *          @arg AON_LDO18_AON_TRIM_0P94
 *          @arg AON_LDO18_AON_TRIM_0P95
 *          @arg AON_LDO18_AON_TRIM_0P96
 *          @arg AON_LDO18_AON_TRIM_0P97
 *          @arg AON_LDO18_AON_TRIM_0P98
 *          @arg AON_LDO18_AON_TRIM_0P99
 *          @arg AON_LDO18_AON_TRIM_1P00
 *          @arg AON_LDO18_AON_TRIM_1P01
 *          @arg AON_LDO18_AON_TRIM_1P02
 *          @arg AON_LDO18_AON_TRIM_1P03
 *          @arg AON_LDO18_AON_TRIM_1P04
 *          @arg AON_LDO18_AON_TRIM_1P05
 *          @arg AON_LDO18_AON_TRIM_1P06
 *          @arg AON_LDO18_AON_TRIM_1P07
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_SW_Trim_Ldo18_Aon_Vout(uint8_t trim)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO18_AON_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_VOUT_TRIM_AON, trim);
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON set LDO08_AON voltage out
 *
 * @param  level: LDO08_AON volatge level, this parameter can be one of the following values:
 *           @arg AON_LDO08_AON_LEVEL_0P575V
 *           @arg AON_LDO08_AON_LEVEL_0P600V
 *           @arg AON_LDO08_AON_LEVEL_0P625V
 *           @arg AON_LDO08_AON_LEVEL_0P650V
 *           @arg AON_LDO08_AON_LEVEL_0P675V
 *           @arg AON_LDO08_AON_LEVEL_0P700V
 *           @arg AON_LDO08_AON_LEVEL_0P725V
 *           @arg AON_LDO08_AON_LEVEL_0P750V
 *           @arg AON_LDO08_AON_LEVEL_0P775V
 *           @arg AON_LDO08_AON_LEVEL_0P800V
 *           @arg AON_LDO08_AON_LEVEL_0P825V
 *           @arg AON_LDO08_AON_LEVEL_0P850V
 *           @arg AON_LDO08_AON_LEVEL_0P875V
 *           @arg AON_LDO08_AON_LEVEL_0P900V
 *           @arg AON_LDO08_AON_LEVEL_0P925V
 *           @arg AON_LDO08_AON_LEVEL_0P950V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo08_Aon_Vout(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO08_AON_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO08AON_VOUT_SEL_AON, level);
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON trim LDO08_AON voltage
 *
 * @param  trim:LDO08_AON volatge trim value, this parameter can be one of the following values:
 *          @arg AON_LDO08_AON_TRIM_0P92
 *          @arg AON_LDO08_AON_TRIM_0P93
 *          @arg AON_LDO08_AON_TRIM_0P94
 *          @arg AON_LDO08_AON_TRIM_0P95
 *          @arg AON_LDO08_AON_TRIM_0P96
 *          @arg AON_LDO08_AON_TRIM_0P97
 *          @arg AON_LDO08_AON_TRIM_0P98
 *          @arg AON_LDO08_AON_TRIM_0P99
 *          @arg AON_LDO08_AON_TRIM_1P00
 *          @arg AON_LDO08_AON_TRIM_1P01
 *          @arg AON_LDO08_AON_TRIM_1P02
 *          @arg AON_LDO08_AON_TRIM_1P03
 *          @arg AON_LDO08_AON_TRIM_1P04
 *          @arg AON_LDO08_AON_TRIM_1P05
 *          @arg AON_LDO08_AON_TRIM_1P06
 *          @arg AON_LDO08_AON_TRIM_1P07
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_SW_Trim_Ldo08_Aon_Vout(uint8_t trim)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO08_AON_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO08AON_VOUT_TRIM_AON, trim);
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  AON set LDO soc voltage out
 *
 * @param  level: LDO_SOC volatge level, this parameter can be one of the following values:
 *           @arg AON_LDO_SOC_LEVEL_0P700V
 *           @arg AON_LDO_SOC_LEVEL_0P725V
 *           @arg AON_LDO_SOC_LEVEL_0P750V
 *           @arg AON_LDO_SOC_LEVEL_0P775V
 *           @arg AON_LDO_SOC_LEVEL_0P800V
 *           @arg AON_LDO_SOC_LEVEL_0P825V
 *           @arg AON_LDO_SOC_LEVEL_0P850V
 *           @arg AON_LDO_SOC_LEVEL_0P875V
 *           @arg AON_LDO_SOC_LEVEL_0P900V
 *           @arg AON_LDO_SOC_LEVEL_0P925V
 *           @arg AON_LDO_SOC_LEVEL_0P950V
 *           @arg AON_LDO_SOC_LEVEL_0P975V
 *           @arg AON_LDO_SOC_LEVEL_1P000V
 *           @arg AON_LDO_SOC_LEVEL_1P025V
 *           @arg AON_LDO_SOC_LEVEL_1P050V
 *           @arg AON_LDO_SOC_LEVEL_1P075V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo_Soc_Vout(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO_SOC_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_VOUT_SEL_AON, level);
    BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON trim LDO SOC voltage
 *
 * @param  trim:LDO_SOC volatge trim value, this parameter can be one of the following values:
 *          @arg AON_LDO_SOC_TRIM_0P93
 *          @arg AON_LDO_SOC_TRIM_0P94
 *          @arg AON_LDO_SOC_TRIM_0P95
 *          @arg AON_LDO_SOC_TRIM_0P96
 *          @arg AON_LDO_SOC_TRIM_0P97
 *          @arg AON_LDO_SOC_TRIM_0P98
 *          @arg AON_LDO_SOC_TRIM_0P99
 *          @arg AON_LDO_SOC_TRIM_1P00
 *          @arg AON_LDO_SOC_TRIM_1P01
 *          @arg AON_LDO_SOC_TRIM_1P02
 *          @arg AON_LDO_SOC_TRIM_1P03
 *          @arg AON_LDO_SOC_TRIM_1P04
 *          @arg AON_LDO_SOC_TRIM_1P05
 *          @arg AON_LDO_SOC_TRIM_1P06
 *          @arg AON_LDO_SOC_TRIM_1P07
 *          @arg AON_LDO_SOC_TRIM_1P08
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_SW_Trim_Ldo_Soc_Vout(uint8_t trim)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO_SOC_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_VOUT_TRIM_AON, trim);
    BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Switch_For_Efuse(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_DCDC12_3);

    if (enable == ENABLE) {
        tmpVal |= (1 << 25);
    } else {
        tmpVal &= ~(1 << 25);
    }

    BL_WR_REG(AON_BASE, AON_DCDC12_3, tmpVal);

    arch_delay_us(500);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo18_AON_Power_Switch_For_FLASH(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_DCDC12_3);

    if (enable == ENABLE) {
        tmpVal |= (1 << 24);
    } else {
        tmpVal &= ~(1 << 24);
    }

    BL_WR_REG(AON_BASE, AON_DCDC12_3, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_PSW);

    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_PSW18_PERI_AON);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_PSW18_PERI_AON);
    }

    BL_WR_REG(AON_BASE, AON_PSW, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON set LDO SYS voltage
 *
 * @param  level:DCDC_SYS volatge Vout value, this parameter can be one of the following values:
 *          @arg AON_DCDC_SYS_LEVEL_1P125V
 *          @arg AON_DCDC_SYS_LEVEL_1P150V
 *          @arg AON_DCDC_SYS_LEVEL_1P175V
 *          @arg AON_DCDC_SYS_LEVEL_1P200V
 *          @arg AON_DCDC_SYS_LEVEL_1P225V
 *          @arg AON_DCDC_SYS_LEVEL_1P250V
 *          @arg AON_DCDC_SYS_LEVEL_1P275V
 *          @arg AON_DCDC_SYS_LEVEL_1P300V
 *          @arg AON_DCDC_SYS_LEVEL_1P325V
 *          @arg AON_DCDC_SYS_LEVEL_1P350V
 *          @arg AON_DCDC_SYS_LEVEL_1P375V
 *          @arg AON_DCDC_SYS_LEVEL_1P400V
 *          @arg AON_DCDC_SYS_LEVEL_1P425V
 *          @arg AON_DCDC_SYS_LEVEL_1P450V
 *          @arg AON_DCDC_SYS_LEVEL_1P475V
 *          @arg AON_DCDC_SYS_LEVEL_1P500V
 *          @arg AON_DCDC_SYS_LEVEL_1P525V
 *          @arg AON_DCDC_SYS_LEVEL_1P550V
 *          @arg AON_DCDC_SYS_LEVEL_1P575V
 *          @arg AON_DCDC_SYS_LEVEL_1P600V
 *          @arg AON_DCDC_SYS_LEVEL_1P625V
 *          @arg AON_DCDC_SYS_LEVEL_1P650V
 *          @arg AON_DCDC_SYS_LEVEL_1P675V
 *          @arg AON_DCDC_SYS_LEVEL_1P700V
 *          @arg AON_DCDC_SYS_LEVEL_1P725V
 *          @arg AON_DCDC_SYS_LEVEL_1P750V
 *          @arg AON_DCDC_SYS_LEVEL_1P775V
 *          @arg AON_DCDC_SYS_LEVEL_1P800V
 *          @arg AON_DCDC_SYS_LEVEL_1P825V
 *          @arg AON_DCDC_SYS_LEVEL_1P850V
 *          @arg AON_DCDC_SYS_LEVEL_1P875V
 *          @arg AON_DCDC_SYS_LEVEL_1P900V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_Dcdc_Sys_Vout(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_DCDC_SYS_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_DCDC12_3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_VOUT_SEL_AON, level);
    BL_WR_REG(AON_BASE, AON_DCDC12_3, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON trim DCDC SYS voltage in lowpower mode
 *
 * @param  trim:DCDC_SYS voltage trim value, this parameter can be one of the following values:
 *          @arg AON_DCDC_SYS_TRIM_0P93
 *          @arg AON_DCDC_SYS_TRIM_0P94
 *          @arg AON_DCDC_SYS_TRIM_0P95
 *          @arg AON_DCDC_SYS_TRIM_0P96
 *          @arg AON_DCDC_SYS_TRIM_0P97
 *          @arg AON_DCDC_SYS_TRIM_0P98
 *          @arg AON_DCDC_SYS_TRIM_0P99
 *          @arg AON_DCDC_SYS_TRIM_1P00
 *          @arg AON_DCDC_SYS_TRIM_1P01
 *          @arg AON_DCDC_SYS_TRIM_1P02
 *          @arg AON_DCDC_SYS_TRIM_1P03
 *          @arg AON_DCDC_SYS_TRIM_1P04
 *          @arg AON_DCDC_SYS_TRIM_1P05
 *          @arg AON_DCDC_SYS_TRIM_1P06
 *          @arg AON_DCDC_SYS_TRIM_1P07
 *          @arg AON_DCDC_SYS_TRIM_1P08
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_SW_Trim_Dcdc_Sys_Vout(uint8_t trim)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AON_DCDC_SYS_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(AON_BASE, AON_DCDC12_3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_VOUT_TRIM_AON, trim);
    BL_WR_REG(AON_BASE, AON_DCDC12_3, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo18_Aon_Vout_in_Lowpower(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO18_AON_VOUT_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_VOUT_SEL_AON_LP, level);
    BL_WR_REG(AON_BASE, AON_LP_EN_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON set LDO SYS voltage
 *
 * @param  level:DCDC_SYS volatge Vout value, this parameter can be one of the following values:
 *          @arg AON_DCDC_SYS_LEVEL_1P125V
 *          @arg AON_DCDC_SYS_LEVEL_1P150V
 *          @arg AON_DCDC_SYS_LEVEL_1P175V
 *          @arg AON_DCDC_SYS_LEVEL_1P200V
 *          @arg AON_DCDC_SYS_LEVEL_1P225V
 *          @arg AON_DCDC_SYS_LEVEL_1P250V
 *          @arg AON_DCDC_SYS_LEVEL_1P275V
 *          @arg AON_DCDC_SYS_LEVEL_1P300V
 *          @arg AON_DCDC_SYS_LEVEL_1P325V
 *          @arg AON_DCDC_SYS_LEVEL_1P350V
 *          @arg AON_DCDC_SYS_LEVEL_1P375V
 *          @arg AON_DCDC_SYS_LEVEL_1P400V
 *          @arg AON_DCDC_SYS_LEVEL_1P425V
 *          @arg AON_DCDC_SYS_LEVEL_1P450V
 *          @arg AON_DCDC_SYS_LEVEL_1P475V
 *          @arg AON_DCDC_SYS_LEVEL_1P500V
 *          @arg AON_DCDC_SYS_LEVEL_1P525V
 *          @arg AON_DCDC_SYS_LEVEL_1P550V
 *          @arg AON_DCDC_SYS_LEVEL_1P575V
 *          @arg AON_DCDC_SYS_LEVEL_1P600V
 *          @arg AON_DCDC_SYS_LEVEL_1P625V
 *          @arg AON_DCDC_SYS_LEVEL_1P650V
 *          @arg AON_DCDC_SYS_LEVEL_1P675V
 *          @arg AON_DCDC_SYS_LEVEL_1P700V
 *          @arg AON_DCDC_SYS_LEVEL_1P725V
 *          @arg AON_DCDC_SYS_LEVEL_1P750V
 *          @arg AON_DCDC_SYS_LEVEL_1P775V
 *          @arg AON_DCDC_SYS_LEVEL_1P800V
 *          @arg AON_DCDC_SYS_LEVEL_1P825V
 *          @arg AON_DCDC_SYS_LEVEL_1P850V
 *          @arg AON_DCDC_SYS_LEVEL_1P875V
 *          @arg AON_DCDC_SYS_LEVEL_1P900V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_Dcdc_Sys_Vout_in_Lowpower(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_DCDC_SYS_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_VOUT_SEL_AON_LP, level);
    BL_WR_REG(AON_BASE, AON_LP_EN_CTRL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  AON set LDO soc voltage out in lowpower mode
 *
 * @param  ldoLevel: LDO_SOC volatge level, this parameter can be one of the following values:
 *           @arg AON_LDO_SOC_LEVEL_0P700V
 *           @arg AON_LDO_SOC_LEVEL_0P725V
 *           @arg AON_LDO_SOC_LEVEL_0P750V
 *           @arg AON_LDO_SOC_LEVEL_0P775V
 *           @arg AON_LDO_SOC_LEVEL_0P800V
 *           @arg AON_LDO_SOC_LEVEL_0P825V
 *           @arg AON_LDO_SOC_LEVEL_0P850V
 *           @arg AON_LDO_SOC_LEVEL_0P875V
 *           @arg AON_LDO_SOC_LEVEL_0P900V
 *           @arg AON_LDO_SOC_LEVEL_0P925V
 *           @arg AON_LDO_SOC_LEVEL_0P950V
 *           @arg AON_LDO_SOC_LEVEL_0P975V
 *           @arg AON_LDO_SOC_LEVEL_1P000V
 *           @arg AON_LDO_SOC_LEVEL_1P025V
 *           @arg AON_LDO_SOC_LEVEL_1P050V
 *           @arg AON_LDO_SOC_LEVEL_1P075V
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo_Soc_Vout_in_Lowpower(uint8_t level)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO_SOC_LEVEL_TYPE(level));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_VOUT_SEL_AON_LP, level);
    BL_WR_REG(AON_BASE, AON_LP_EN_CTRL, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Ctrl_Ldo18_Aon_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
    tmpVal ^= (0x7 << 4);
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO18AON_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO18AON_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Ctrl_Dcdc_Sys_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
    tmpVal ^= (0x7 << 4);
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_DCDC12_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_DCDC12_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Ctrl_Ldo_Soc_Mode_by_HW(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
    tmpVal ^= (0x7 << 4);
    if (enable == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_LDO09SOC_LP_EN_AON_CTRL_HW);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO09SOC_LP_EN_AON_CTRL_HW);
    }

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo18_Aon_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO18_AON_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
    tmpVal ^= (0x7 << 4);

    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO18AON_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_LP_EN_AON, mode);

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Dcdc_Sys_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_DCDC_SYS_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
    tmpVal ^= (0x7 << 4);

    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_DCDC12_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_LP_EN_AON, mode);

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Ldo_Soc_Mode(uint8_t mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_LDO_SOC_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_LP_EN_HW);
    tmpVal ^= (0x7 << 4);

    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_LDO09SOC_LP_EN_AON_CTRL_HW);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_LP_EN_AON, mode);

    BL_WR_REG(AON_BASE, AON_LP_EN_HW, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_Set_Dcdc_Sys_Sstart_Delay(uint8_t delay)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((delay <= 0x3));

    /* config dcdc12_sstart_time_aon */
    tmpVal = BL_RD_REG(AON_BASE, AON_DCDC12_2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_SSTART_TIME_AON, delay);
    BL_WR_REG(AON_BASE, AON_DCDC12_2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get AON bod out state
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type AON_Get_BOD_OUT_State(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(AON_BASE, AON_1), AON_R_BOD_OUT) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set AON bod config
 *
 * @param  enable: ENABLE or DISABLE, if enable, Power up Brown Out Reset
 * @param  threshold: bod threshold, this parameter can be one of the following values:
 *           @arg AON_BOD_THRES_2P05V
 *           @arg AON_BOD_THRES_2P10V
 *           @arg AON_BOD_THRES_2P15V
 *           @arg AON_BOD_THRES_2P20V
 *           @arg AON_BOD_THRES_2P25V
 *           @arg AON_BOD_THRES_2P30V
 *           @arg AON_BOD_THRES_2P35V
 *           @arg AON_BOD_THRES_2P40V
 * @param  mode: bod work mode with por, this parameter can be one of the following values:
 *           @arg AON_BOD_MODE_POR_INDEPENDENT
 *           @arg AON_BOD_MODE_POR_RELEVANT
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type AON_Set_BOD_Config(uint8_t enable, uint8_t threshold, uint8_t mode)
{
#ifndef BOOTROM
    uint32_t tmpVal;

    CHECK_PARAM(IS_AON_BOD_THRES_TYPE(threshold));
    CHECK_PARAM(IS_AON_BOD_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(AON_BASE, AON_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_BOD_TH_AON, threshold);
    BL_WR_REG(AON_BASE, AON_1, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_BOR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_SEL, mode);
    BL_WR_REG(HBN_BASE, HBN_BOR_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_0);
    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_PU_BOD_AON, 1);
        HBN_Enable_BOD_IRQ();
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_PU_BOD_AON, 0);
        HBN_Disable_BOD_IRQ();
    }
    BL_WR_REG(AON_BASE, AON_0, tmpVal);

#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Init WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void AON_Wdg_Init(uint16_t comp_val)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_WDT);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_CR_CNT_MASK_BIT, comp_val);
    BL_WR_REG(AON_BASE, AON_WDT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Start WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void AON_Wdg_Start(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_WDT);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_CR_ANO_WDT_EN);
    BL_WR_REG(AON_BASE, AON_WDT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Get state of WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type AON_Wdg_Status_Get(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_BOR_CFG), HBN_ST_AON_WDT) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Clear state of WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void AON_Wdg_Status_Clear(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_WDT);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_CR_AON_WDT_CLR);
    BL_WR_REG(AON_BASE, AON_WDT, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_WDT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_CR_AON_WDT_CLR);
    BL_WR_REG(AON_BASE, AON_WDT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Feed WatchDog in hbn domain
 *
 * @param  NULL
 *
 * @return NULL
 *
*******************************************************************************/
void AON_Wdg_Kick(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_WDT);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_CR_SW_KICK_POS);
    BL_WR_REG(AON_BASE, AON_WDT, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_WDT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_CR_SW_KICK_POS);
    BL_WR_REG(AON_BASE, AON_WDT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  enable or disable RC32M run speed as 8M or 32M
 *
 * @param  enable: Enable or disable
 *
 * @return NONE
 *
*******************************************************************************/
void ATTR_CLOCK_SECTION AON_Set_RC32M_Speed_As_8M(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL1_AON);
    if (enable) {
        tmpVal |= (1 << 25);
    } else {
        tmpVal &= ~(1 << 25);
    }
    BL_WR_REG(AON_BASE, AON_RC32M_CTRL1_AON, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set rc32m external code in for frequency setting
 *
 * @param  code: code value
 *
 * @return NONE
 *
*******************************************************************************/
void AON_Set_RC32M_Code_Fr_Ext(uint8_t code)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(AON_BASE, AON_RC32M_CTRL0_AON);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_RC32M_CODE_FR_EXT, code);
    BL_WR_REG(AON_BASE, AON_RC32M_CTRL0_AON, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Trim LDO_SOC According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo_Soc_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo09", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO09SOC);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO09SOC_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_LDO09SOC, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo08_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo08_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo08", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO08AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim Ldo18_Aon According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Trim_Ldo18_Aon_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18AON_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Trim DCDC12_SYS According to Efuse
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Trim_Dcdc12_Sys_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "dcdc12_vout", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_DCDC12_3);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_DCDC12_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_DCDC12_3, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}

/*@} end of group AON_Public_Functions */

/*@} end of group AON */

/*@} end of group BL616D_Peripheral_Driver */
