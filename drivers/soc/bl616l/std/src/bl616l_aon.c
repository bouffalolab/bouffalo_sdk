/**
  ******************************************************************************
  * @file    bl616l_aon.c
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

#include "bl616l_aon.h"
#include "bl616l_ef_cfg.h"
/** @addtogroup  BL616L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  AON
 *  @{
 */

/** @defgroup  AON_Private_Macros
 *  @{
 */

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
 * @brief  Power on RF
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION AON_Power_On_RF(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_MISC);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_SW_WB_EN_AON);
    BL_WR_REG(AON_BASE, AON_MISC, tmpVal);

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
BL_Err_Type ATTR_CLOCK_SECTION AON_Power_Off_RF(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_MISC);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_SW_WB_EN_AON);
    BL_WR_REG(AON_BASE, AON_MISC, tmpVal);

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
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_PU);
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
 * @brief  XTAL switch from normal mode to low power mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_XTAL_Enter_LowPower(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE1);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_XTAL_LP_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE1, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE1);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL_LP_EN_AON);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL_SOCBUF_SEL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE1, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE2);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL_EXT_SEL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE2, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_PU);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_XTAL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_PU, tmpVal);

    // arch_delay_us(100);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  XTAL switch from low power mode to normal mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_XTAL_Exit_LowPower(void)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_LP_EN_AON);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_SOCBUF_SEL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE1, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_EXT_SEL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE2, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_PU);
    tmpVal = BL_SET_REG_BIT(tmpVal, AON_PU_XTAL_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_PU, tmpVal);

    /* Polling for ready */
    do {
        arch_delay_us(10);
        timeOut++;
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_PU);
    } while (!BL_IS_REG_BIT_SET(tmpVal, AON_XTAL_RDY) && timeOut < 60);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CORE1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_PU_XTAL_LP_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CORE1, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_LDO18_IO_Switch_Efuse(uint8_t en)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_PSW_PMIP_RESV);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_PU_PSW18_EFUSE_AON, en);
    BL_WR_REG(AON_BASE, AON_PSW_PMIP_RESV, tmpVal);
    arch_delay_us(500);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_LDO18_IO_Switch_Psram(uint8_t en)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_PSW_PMIP_RESV);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_PU_PSW18_PSRAM_AON, en);
    BL_WR_REG(AON_BASE, AON_PSW_PMIP_RESV, tmpVal);

    return SUCCESS;
}

BL_Err_Type ATTR_TCM_SECTION AON_LDO18_IO_Switch_Flash(uint8_t en)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_PSW_PMIP_RESV);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_PU_PSW18_FLASH_AON, en);
    BL_WR_REG(AON_BASE, AON_PSW_PMIP_RESV, tmpVal);

    return SUCCESS;
}
/******************************************************************************
 * @brief  AON Set LDO18_IO's startup Delay
 *
 * @param  delay: None
 *
 * @return SUCCESS or ERROR
 *
 *******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_Set_LDO18_IO_Sstart_Delay(uint8_t delay)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((delay <= 0x3));

    /* config ldo18io_sstart_sel_aon */
    tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_SSTART_SEL_AON, delay);
    BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo18io voltage out
 *
 * @param  ldoLevel: LDO18_IO volatge level, this parameter can be one of the following values:
 *           @arg AON_LDO18_IO_LEVEL_1P65
 *           @arg AON_LDO18_IO_LEVEL_1P70
 *           @arg AON_LDO18_IO_LEVEL_1P75
 *           @arg AON_LDO18_IO_LEVEL_1P80
 *           @arg AON_LDO18_IO_LEVEL_1P85
 *           @arg AON_LDO18_IO_LEVEL_1P90
 *           @arg AON_LDO18_IO_LEVEL_1P95
 *           @arg AON_LDO18_IO_LEVEL_2P00
 *           @arg AON_LDO18_IO_LEVEL_2P60
 *           @arg AON_LDO18_IO_LEVEL_2P70
 *           @arg AON_LDO18_IO_LEVEL_2P80
 *           @arg AON_LDO18_IO_LEVEL_2P90
 *           @arg AON_LDO18_IO_LEVEL_2P95
 *           @arg AON_LDO18_IO_LEVEL_3P00
 *           @arg AON_LDO18_IO_LEVEL_3P05
 *           @arg AON_LDO18_IO_LEVEL_3P10
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION AON_SW_Set_Ldo18_IO_Vout(uint8_t ldoLevel)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AON_LDO18_IO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);

    return SUCCESS;
}

BL_Err_Type AON_SW_Trim_Ldo18_IO_Vout(uint8_t trim)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM(IS_AON_LDO18_IO_TRIM_TYPE(trim));

    tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_VOUT_TRIM_AON, trim);
    BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);

    return SUCCESS;
}


BL_Err_Type AON_Trim_Ldo18_IO_Vout(void)
{
    bflb_ef_ctrl_com_trim_t trim;
    int32_t tmpVal = 0;
    struct bflb_device_s *ef_ctrl;

    ef_ctrl = bflb_device_get_by_name("ef_ctrl");
    bflb_ef_ctrl_read_common_trim(ef_ctrl, "ldo18_trim", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, 4)) {
            tmpVal = BL_RD_REG(AON_BASE, AON_LDO18IO);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_LDO18IO_VOUT_TRIM_AON, trim.value);
            BL_WR_REG(AON_BASE, AON_LDO18IO, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return SUCCESS;
}
/*@} end of group AON_Public_Functions */

/*@} end of group AON */

/*@} end of group BL616L_Peripheral_Driver */
